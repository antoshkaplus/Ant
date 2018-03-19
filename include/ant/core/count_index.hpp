#pragma once

#include "ant/core/core.hpp"

namespace ant {

template<bool sortedIndex=false, class IndexType=Index, class CountType=Count>
class CountIndex;

template<class IndexType, class CountType>
class CountIndex<false, IndexType, CountType> {

    struct Item {
        CountType count;
        IndexType keyIndex;
    };

    std::vector<Item> items;
    std::vector<IndexType> keys_;

public:
    Count operator[](IndexType index) const {
        if (index >= items.size()) return 0;
        return items[index].count;
    }

    const auto& keys() const {
        return keys_;
    }

    IndexType size() const {
        return keys_.size();
    }

    bool empty() const {
        return keys_.empty();
    }

    void Increase(IndexType index) {
        auto& item = get(index);
        if (item.count == 0) {
            item.keyIndex = keys_.size();
            keys_.push_back(index);
        }
        ++item.count;
    }

    void Decrease(IndexType index) {
        assert(index < items.size());
        auto& item = items[index];
        assert(item.count > 0);
        --item.count;
        if (item.count == 0) {
            auto& freedKey = keys_[item.keyIndex];
            std::swap(freedKey, keys_.back());
            items[freedKey].keyIndex = item.keyIndex;
            item.keyIndex = -1;
            keys_.pop_back();
        }
    }

private:

    Item& get(IndexType index) {
        if (index >= items.size()) items.resize(index+1);
        return items[index];
    }
};

template<class IndexType, class CountType>
class CountIndex<true, IndexType, CountType> {

    std::vector<CountType> items;
    std::set<IndexType> keys_;

public:
    Count operator[](IndexType index) const {
        if (index >= items.size()) return 0;
        return items[index];
    }

    const auto& keys() const {
        return keys_;
    }

    IndexType size() const {
        return keys_.size();
    }

    bool empty() const {
        return keys_.empty();
    }

    void Increase(IndexType index) {
        auto& count = get(index);
        if (count == 0) {
            keys_.insert(index);
        }
        ++count;
    }

    void Decrease(IndexType index) {
        assert(index < items.size());
        auto& count = items[index];
        assert(count > 0);
        --count;
        if (count == 0) {
            keys_.erase(index);
        }
    }

private:

    CountType& get(IndexType index) {
        if (index >= items.size()) items.resize(index+1);
        return items[index];
    }
};

}
