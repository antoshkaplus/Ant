#pragma once

#include <urcu-qsbr.h>    /* RCU flavor */
#include <urcu/rculfhash.h>    /* RCU Lock-free hash table */

#include "read_section.h"
#include "destructor.h"

namespace ant::parallel::rcu {

template<typename TKey>
class set {
    static_assert(std::is_same<std::size_t, unsigned long>::value, "std::size_t must be unsigned long");

    struct item {
        TKey key;
        cds_lfht_node node;
        rcu_head head;

        item(const TKey& key)
                : key(key), node(), head() {
            cds_lfht_node_init(&node);
        }

        item(const item&) = delete;

        item& operator=(const item&) = delete;
    };

    cds_lfht* hashTable;

public:
    set()
            : hashTable(cds_lfht_new(1, 1, 0, CDS_LFHT_AUTO_RESIZE | CDS_LFHT_ACCOUNTING, nullptr)) {
        if (hashTable == nullptr) throw Exception();
    }

    set(const set&) = delete;

    set& operator=(const set&) = delete;

    ~set() {
        clear();

        cds_lfht_destroy(hashTable, nullptr);
        hashTable = nullptr;
    }

    void clear() {
        read_section lock;

        set::item* item;
        cds_lfht_iter it;
        cds_lfht_for_each_entry(hashTable, &it, item, node) {
            cds_lfht_node* node = cds_lfht_iter_get_node(&it);
            if (cds_lfht_del(hashTable, node) == 0) {
                call_rcu(&item->head, erase_node);
            }
        }
    }

    bool empty() const {
        read_section lock;

        set::item* item;
        cds_lfht_iter it;
        cds_lfht_for_each_entry(hashTable, &it, item, node) {
            if (cds_lfht_iter_get_node(&it) != nullptr) return false;
        }

        return true;
    }

    bool contains(const TKey& key) const {
        read_section lock;

        cds_lfht_iter it;
        cds_lfht_lookup(hashTable, std::hash<TKey>()(key), equal_to, &key, &it);

        return cds_lfht_iter_get_node(&it) != nullptr;
    }

    bool insert(const TKey& key) {
        if (contains(key)) return false;

        read_section lock;

        std::unique_ptr<set::item> item(new set::item(key));
        if (item == nullptr) throw Exception();

        cds_lfht_node* node = cds_lfht_add_unique(hashTable, std::hash<TKey>()(key), equal_to, &key, &item->node);

        bool result = node == &item->node;
        if (result) item.release();

        return result;
    }

    bool erase(const TKey& key) {
        read_section lock;

        cds_lfht_iter it;
        cds_lfht_lookup(hashTable, std::hash<TKey>()(key), equal_to, &key, &it);

        cds_lfht_node* node = cds_lfht_iter_get_node(&it);
        if (node == nullptr) return false;

        int result = cds_lfht_del(hashTable, node);
        if (result != 0) return false;

        call_rcu(&caa_container_of(node, set::item, node)->head, erase_node);

        return true;
    }

    template<typename TFunctor>
    void for_each(TFunctor&& functor) const {
        read_section lock;

        set::item* item;
        cds_lfht_iter it;
        cds_lfht_for_each_entry(hashTable, &it, item, node) {
            functor(item->key);
        }
    }

private:
    inline static int equal_to(cds_lfht_node* node, const void* key) {
        return *(TKey*) key == caa_container_of(node, set::item, node)->key;
    }

    inline static void erase_node(rcu_head* head) {
        delete caa_container_of(head, struct set::item, head);
    }

    static void destroy_hash_table(cds_lfht* hashTable) {
        /*
         * cds_lfht_destroy() must be called from a very specific
         * context: it needs to be called from a registered RCU reader
         * thread. However, this thread should _not_ be within a RCU
         * read-side critical section. Also, it should _not_ be called
         * from a call_rcu thread.
         */
        cds_lfht_destroy(hashTable, nullptr);
    }
};

}
