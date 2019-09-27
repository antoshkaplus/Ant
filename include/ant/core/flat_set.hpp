#pragma once

#include "ant/core/core.hpp"

namespace ant {

// should be flat_set
template<class T>
class FlatSet {
    using FlatSetIt = typename std::vector<T>::iterator;

    std::vector<T> vals_;

public:
    auto begin() const {
        return vals_.begin();
    }
    auto end() const {
        return vals_.end();
    }

    bool empty() const {
        return vals_.empty();
    }
    ant::Count size() const {
        return vals_.size();
    }

    std::pair<FlatSetIt, bool> Insert(const T& val) {
        FlatSetIt it = std::lower_bound(vals_.begin(), vals_.end(), val);
        if (it != vals_.end() && *it == val) {
            return {vals_.end(), false};
        }
        it = vals_.insert(it, val);
        return {it, true};
    }

    void Reserve(Count count) {
        vals_.reserve(count);
    }

    auto Remove(const T& val) {
        return Erase(val);
    }

    auto Count(const T& val) const {
        auto it = std::lower_bound(vals_.begin(), vals_.end(), val);
        if (it != vals_.end() && *it == val) return 1;

        return 0;
    }

    std::pair<FlatSetIt, bool> Erase(const T& val) {
        auto it = std::lower_bound(vals_.begin(), vals_.end(), val);
        if (it != vals_.end() && *it == val) {
            return {vals_.erase(it), true};
        }
        return {vals_.end(), false};
    };

    FlatSetIt Erase(FlatSetIt it) {
        return vals_.erase(it);
    }

    std::vector<T> Detach() {
        decltype(vals_) temp;
        std::swap(vals_, temp);
        return temp;
    }

    const std::vector<T>& get() const {
        return vals_;
    }
};

}