#pragma once

#include <iterator>

#include "ant/core/core.hpp"
#include "ant/core/skip_list/skip_list_height_gen.hpp"
#include "ant/core/skip_list/skip_list_iterator.hpp"

namespace ant {

// have to do more testing to make sure it's working

template <typename Key, typename Value>
class SkipListMap {
public:

    struct Node
    {
        // number of those is equal to height
        std::vector<std::shared_ptr<Node>> next;
        std::pair<const Key, Value> value;

        Node() {}

        Node(int h, const Key& key, const Value& value)
                : next(h), value(key, value) {}

        Node(int h)
                : next(h) {}

        int height() const {
            return next.size();
        }
    };

    friend class SkipListIteratorWrapper<Node, std::pair<const Key, Value>>;
    friend class SkipListIteratorWrapper<const Node, std::pair<const Key, Value>>;

    using Iterator = SkipListIteratorWrapper<Node, std::pair<const Key, Value>>;
    using ConstIterator = SkipListIteratorWrapper<const Node, std::pair<const Key, Value>>;


    explicit SkipListMap(int maxNumElems) : count(0), curHeight(0) {

        int maxHeight = std::max(1, static_cast<ant::Count>(std::log2(maxNumElems)));
        heightGen = HeightGen(maxHeight, 0.5);

        head = std::make_shared<Node>(maxHeight);
    }

    ~SkipListMap() {
        auto cur = head;
        while (cur) {
            auto next = cur->next[0];
            for (auto& sh : cur->next) {
                sh.reset();
            }
            cur = next;
        }

    }

    ConstIterator begin() const {
        return ConstIterator{std::const_pointer_cast<const Node>(head->next[0])};
    }

    ConstIterator end() const {
        return {};
    }

    Iterator begin() {
        return Iterator(head->next[0]);
    }

    Iterator end() {
        return {};
    }

    bool empty() const {
        return size() == 0;
    }

    ant::Count size() const {
        return count;
    }

    // we don't check if already exists or not
    void Insert(const Key& key, const Value& value) {

        auto height = heightGen();
        std::shared_ptr<Node> newNode = std::make_shared<Node>(height, key, value);

        curHeight = std::max(curHeight, height);

        // TODO consider equal keys

        auto cur = head;
        for (auto i = curHeight-1; i > 0; --i) {
            while (cur->next[i] && cur->next[i]->value.first < key) {
                cur = cur->next[i];
            }

            if (cur->next[i] && cur->next[i]->value.first == key) {
                remove(cur, cur->next[i], i);
            }

            if (i < height) {
                insert(cur, newNode, i);
            }
        }

        while (cur->next[0] && cur->next[0]->value.first < key) {
            cur = cur->next[0];
        }

        if (cur->next[0] && cur->next[0]->value.first == key) {
            remove(cur, cur->next[0], 0);
            --count;
        }

        insert(cur, newNode, 0);
        ++count;
    }

    void Remove(const Key& key) {

        auto cur = head;
        for (auto i = curHeight-1; i > 0; --i) {
            while (cur->next[i] && cur->next[i]->value.first < key) {
                cur = cur->next[i];
            }

            if (cur->next[i] && cur->next[i]->value.first == key) {
                remove(cur, cur->next[i], i);
            }
        }

        while (cur->next[0] && cur->next[0]->value.first < key) {
            cur = cur->next[0];
        }

        if (cur->next[0] && cur->next[0]->value.first == key) {
            remove(cur, cur->next[0], 0);
            --count;
        }
    }

    void Erase(const Key& val) {
        Remove(val);
    }

    ant::Count Count(const Key& key) const {

        auto cur = head;
        for (auto i = curHeight-1; i >= 0; --i) {
            while (cur->next[i] && cur->next[i]->value.first < key) {
                cur = cur->next[i];
            }

            if (cur->next[i] && cur->next[i]->value.first == key) {
                return 1;
            }
        }
        return 0;
    }

    // we should terminate if not found
    const Value& operator[](const Key& key) const {

        auto cur = head;
        for (auto i = curHeight-1; i >= 0; --i) {
            while (cur->next[i] && cur->next[i]->value.first < key) {
                cur = cur->next[i];
            }

            if (cur->next[i]->value == key) {
                return cur->next[i]->value.second;
            }
        }
        throw std::runtime_error("out of range");
    }

    Value* at(const Key& key) const {

        auto cur = head;
        for (auto i = curHeight-1; i >= 0; --i) {
            while (cur->next[i] && cur->next[i]->value.first < key) {
                cur = cur->next[i];
            }

            if (cur->next[i]->value == key) {
                return &cur->next[i]->value.second;
            }
        }
        return nullptr;
    }

    template <typename Func>
    void ForEach(Func&& func) {
        auto cur = head;
        while (cur->next[0]) {
            func(cur->next[0]->value);
            cur = cur->next[0];
        }
    }



private:

    void insert(std::shared_ptr<Node> prev, std::shared_ptr<Node> newNode, int i) {
        newNode->next[i] = prev->next[i];
        prev->next[i] = newNode;
    }

    void remove(std::shared_ptr<Node> prev, std::shared_ptr<Node> cur, int i) {
        prev->next[i] = cur->next[i];
    }

    int count;
    int curHeight;
    std::shared_ptr<Node> head;
    HeightGen heightGen;
};

}
