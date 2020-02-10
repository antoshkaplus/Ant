#pragma once

#include "ant/core/core.hpp"

namespace ant {

struct FlatIteratorSentinel {};

template <typename BeginIt, typename EndIt, typename ...OtherGetters>
class FlatIterator;

template <typename BeginIt, typename EndIt>
class FlatIterator<BeginIt, EndIt> {

    BeginIt begin_;
    EndIt end_;

public:
    FlatIterator(BeginIt begin, EndIt end) : begin_(begin), end_(end) {}

    bool Next() {
        ++begin_;
        return begin_ != end_;
    }

    bool FindFirst(BeginIt begin, EndIt end) {
        begin_ = begin;
        end_ = end;
        return begin_ != end_;
    }

    // based on dereference type can determine how to return auto, type or reference.
    decltype(*begin_) operator*() {
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


template <typename BeginIt, typename EndIt, typename RangeGetter, typename ...OtherGetters>
class FlatIterator<BeginIt, EndIt, RangeGetter, OtherGetters...> {

    using InnerBeginIt = decltype(std::declval<RangeGetter>()(*std::declval<BeginIt>()).begin());
    using InnerEndIt = decltype(std::declval<RangeGetter>()(*std::declval<BeginIt>()).end());

    BeginIt begin_;
    EndIt end_;

    RangeGetter range_getter;

    // need inner iterator type
    FlatIterator<InnerBeginIt, InnerEndIt, OtherGetters...> inner;

public:
    FlatIterator(BeginIt begin, EndIt end, RangeGetter&& range_getter, OtherGetters&& ...other_getters)
        : range_getter(std::forward<RangeGetter>(range_getter)),
          inner(InnerBeginIt{}, InnerEndIt{}, std::forward<OtherGetters>(other_getters)...) {
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

    bool FindFirst(BeginIt begin, EndIt end) {
        begin_ = begin;
        end_ = end;
        while (begin_ != end_) {
            auto&& range = range_getter(*begin_);
            if (inner.FindFirst(range.begin(), range.end())) return true;
            ++begin_;
        }
        return false;
    }

    decltype(*inner) operator*() {
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

template <typename BeginIt, typename EndIt, typename ...OtherGetters>
auto FlatRange(BeginIt begin, EndIt end, OtherGetters&&... other_getters) {
    return IteratorRange(FlatIterator<BeginIt, EndIt, OtherGetters...>(begin, end, std::forward<OtherGetters>(other_getters)...),
                         FlatIteratorSentinel{});
}

}