#pragma once

#include <optional>

#include <urcu-qsbr.h>
#include <urcu/rculfhash.h>

#include "add_or_replace_result.h"

namespace ant::parallel::rcu {

template<typename TKey, typename TValue>
class unordered_map {

    struct handle {
        cds_lfht_node node;
        rcu_head head;

        handle() : node(), head() {
            cds_lfht_node_init(&node);
        }
    };

    struct item : public handle {
        TKey key;
        TValue value;

        template<typename... TArgs>
        item(const TKey& key, TArgs&& ... args)
                : handle(), key(key), value(std::forward<TArgs>(args)...) {
        }

        item(const item&) = delete;
        item& operator=(const item&) = delete;
    };

    cds_lfht* hashTable;

public:
    unordered_map() : hashTable(cds_lfht_new(1, 1, 0, CDS_LFHT_AUTO_RESIZE | CDS_LFHT_ACCOUNTING, nullptr)) {

        if (hashTable == nullptr) throw std::runtime_error("unable to create rcu::unordered_map");
    }

    unordered_map(const unordered_map&) = delete;
    unordered_map& operator=(const unordered_map&) = delete;

    ~unordered_map() {
        clear();

        cds_lfht_destroy(hashTable, nullptr);
        hashTable = nullptr;
    }

    void clear() {

        cds_lfht_iter it;
        cds_lfht_node* node;
        cds_lfht_for_each(hashTable, &it, node) {
            if (cds_lfht_del(hashTable, node) == 0) {
                call_rcu(&caa_container_of(node, unordered_map::handle, node)->head, erase_node);
            }
        }
    }

    const TValue* find(const TKey& key) const {

        cds_lfht_iter it;
        cds_lfht_lookup(hashTable, std::hash<TKey>()(key), equal_to, &key, &it);

        cds_lfht_node* node = cds_lfht_iter_get_node(&it);
        if (node == nullptr) return nullptr;

        return &static_cast<unordered_map::item*>(caa_container_of(node, unordered_map::handle, node))->value;
    }

    const TValue& operator[](const TKey& key) const {

        // element should be inserted using default constructor
        cds_lfht_iter it;
        cds_lfht_lookup(hashTable, std::hash<TKey>()(key), equal_to, &key, &it);

        cds_lfht_node* node = cds_lfht_iter_get_node(&it);
        if (node == nullptr) throw std::runtime_error("key not found");

        return static_cast<unordered_map::item*>(caa_container_of(node, unordered_map::handle, node))->value;
    }

    template<typename... TArgs>
    std::pair<const TValue*, bool> emplace(const TKey& key, TArgs&& ... args) {

        std::unique_ptr<unordered_map::item> item(new unordered_map::item(key, std::forward<TArgs>(args)...));

        cds_lfht_node* node = cds_lfht_add_unique(hashTable, std::hash<TKey>()(key), equal_to, &key, &item->node);

        bool result = node == &item->node;
        if (result) item.release();

        return {&static_cast<unordered_map::item*>(caa_container_of(node, unordered_map::handle, node))->value,
                                      result};
    }

    template<typename TValue2>
    bool update_if_not_equal(const TKey& key, TValue2&& value) {

        cds_lfht_iter it;
        cds_lfht_lookup(hashTable, std::hash<TKey>()(key), equal_to, &key, &it);

        cds_lfht_node* findNode = cds_lfht_iter_get_node(&it);
        if (findNode != nullptr &&
            static_cast<unordered_map::item*>(caa_container_of(findNode, unordered_map::handle, node))->value == value)
            return false;

        unordered_map::item* item{new unordered_map::item(key, std::forward<TValue2>(value))};

        cds_lfht_node* node = cds_lfht_add_replace(hashTable, std::hash<TKey>()(key), equal_to, &key, &item->node);
        if (node == nullptr) return true;

        call_rcu(&caa_container_of(node, unordered_map::handle, node)->head, erase_node);
        return true;
    }

    template<typename... TArgs>
    auto add_or_replace(const TKey& key, TArgs&& ... args) {

        // TODO use unique_ptr maybe ???
        unordered_map::item* item{new unordered_map::item(key, std::forward<TArgs>(args)...)};

        cds_lfht_node* node = cds_lfht_add_replace(hashTable, std::hash<TKey>()(key), equal_to, &key, &item->node);

        // probably can just use optional here
        if (node == nullptr) return add_or_replace_result<TValue>(item->value, *(TValue*) nullptr, false);

        call_rcu(&caa_container_of(node, unordered_map::handle, node)->head, erase_node);

        return add_or_replace_result<TValue>(item->value,
                                             static_cast<unordered_map::item*>(caa_container_of(node, unordered_map::handle, node))->value,
                                             true);
    }

    bool erase(const TKey& key) {

        cds_lfht_iter it;
        cds_lfht_lookup(hashTable, std::hash<TKey>()(key), equal_to, &key, &it);

        cds_lfht_node* node = cds_lfht_iter_get_node(&it);
        if (node == nullptr) return false;

        int result = cds_lfht_del(hashTable, node);
        if (result != 0) return false;

        call_rcu(&caa_container_of(node, unordered_map::handle, node)->head, erase_node);

        return true;
    }

    template<typename TFunctor>
    void for_each(TFunctor&& functor) const {

        cds_lfht_iter it;
        cds_lfht_node* node;
        cds_lfht_for_each(hashTable, &it, node) {
            unordered_map::item* item = static_cast<unordered_map::item*>(caa_container_of(node, unordered_map::handle, node));
            functor(std::pair<const TKey&, TValue&>(item->key, item->value));
        }
    }

    template<typename TFunctor>
    void for_each_value(TFunctor&& functor) const {

        cds_lfht_iter it;
        cds_lfht_node* node;
        cds_lfht_for_each(hashTable, &it, node) {
            unordered_map::item* item = static_cast<unordered_map::item*>(caa_container_of(node, unordered_map::handle, node));
            functor(item->value);
        }
    }

    template<typename TFunctor>
    void for_each_key(TFunctor&& functor) const {

        cds_lfht_iter it;
        cds_lfht_node* node;
        cds_lfht_for_each(hashTable, &it, node) {
            unordered_map::item* item = static_cast<unordered_map::item*>(caa_container_of(node, unordered_map::handle, node));
            functor((const TKey&) item->key);
        }
    }

private:
    inline static int equal_to(cds_lfht_node* node, const void* key) {
        return *(TKey*) key == static_cast<unordered_map::item*>(caa_container_of(node, unordered_map::handle, node))->key;
    }

    inline static void erase_node(rcu_head* head) {
        delete static_cast<unordered_map::item*>(caa_container_of(head, unordered_map::handle, head));
    }
};

}

