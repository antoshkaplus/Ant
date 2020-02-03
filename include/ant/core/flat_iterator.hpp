#pragma once

#include "ant/core/core.hpp"

namespace ant {

struct FlatIteratorSentinel {};

template <typename OuterIt, typename ...OtherGetters>
class FlatIterator;

template <typename It>
class FlatIterator<It> {

    It begin_;
    It end_;

public:
    FlatIterator(It begin, It end) : begin_(begin), end_(end) {
        for (auto k : IteratorRange(begin, end)) ant::Println(std::cout, k);

        begin_ == end_;
    }

    bool Next() {
        ++begin_;
        return begin_ != end_;
    }

    bool FindFirst(It begin, It end) {
        begin_ = begin;
        end_ = end;
        return begin_ != end_;
    }

    // based on dereference type can determine how to return auto, type or reference.
    auto& operator*() {
        return *begin_;
    }

    FlatIterator& operator++() {
        Next();
        return *this;
    }

    bool operator==(FlatIteratorSentinel sentinel) const {
        return begin_ == end_;
    }

    bool operator!=(FlatIteratorSentinel sentinel) const {
        return begin_ != end_;
    }
};


template <typename OuterIt, typename RangeGetter, typename ...OtherGetters>
class FlatIterator<OuterIt, RangeGetter, OtherGetters...> {

    using InnerIt = decltype(std::declval<RangeGetter>()(*std::declval<OuterIt>()).begin());

    OuterIt begin_;
    OuterIt end_;

    RangeGetter range_getter;

    // need inner iterator type
    FlatIterator<InnerIt, OtherGetters...> inner;

public:
    FlatIterator(OuterIt begin, OuterIt end, RangeGetter&& range_getter, OtherGetters&& ...other_getters)
        : range_getter(std::forward<RangeGetter>(range_getter)),
          inner(InnerIt{}, InnerIt{}, std::forward<OtherGetters>(other_getters)...) {
        // only the most outer one make sense
        FindFirst(begin, end);
    }

    bool Next() {
        if (inner.Next()) return true;
        while (++begin_ != end_) {
            auto&& range = range_getter(*begin_);
            if (inner.FindFirst(range.begin(), range.end())) return true;
        }
        return false;
    }

    bool FindFirst(OuterIt begin, OuterIt end) {
        begin_ = begin;
        end_ = end;
        while (begin_ != end_) {
            auto&& range = range_getter(*begin_);
            if (inner.FindFirst(range.begin(), range.end())) return true;
            ++begin_;
        }
        return false;
    }

    auto& operator*() {
        return *inner;
    }

    // pred
    // called only on most outer layer
    FlatIterator& operator++() {
        Next();
        return *this;
    }

    bool operator==(FlatIteratorSentinel sentinel) const {
        return begin_ == end_;
    }

    bool operator!=(FlatIteratorSentinel sentinel) const {
        return begin_ != end_;
    }
};

template <typename OuterIt, typename ...OtherGetters>
auto FlatRange(OuterIt begin, OuterIt end, OtherGetters&&... other_getters) {
    return IteratorRange(FlatIterator<OuterIt, OtherGetters...>(begin, end, std::forward<OtherGetters>(other_getters)...),
                         FlatIteratorSentinel{});
}

}