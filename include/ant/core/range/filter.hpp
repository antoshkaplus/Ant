#pragma once

#include "ant/core/core.hpp"

namespace ant::core::range {

struct FilterIteratorSentinel {};

template <typename Range, typename Func>
class FilterIterator {

    using BeginIt = decltype(std::declval<Range>().begin());
    using EndIt = decltype(std::declval<Range>().end());

    BeginIt begin_;
    EndIt end_;
    Func func_;

public:
    FilterIterator(Range& range, Func&& func) : begin_(range.begin()), end_(range.end()),
        func_(std::forward<Func>(func)) {

        FindFirst(begin_, end_);
    }

    bool FindFirst(BeginIt begin, EndIt end) {
        begin_ = begin;
        end_ = end;
        while (begin_ != end_) {
            if (func_(*begin_)) return true;
            ++begin_;
        }
        return false;
    }

    // based on dereference type can determine how to return auto, type or reference.
    decltype(*begin_) operator*() {
        return *begin_;
    }

    BeginIt& operator++() {
        ++begin_;
        FindFirst(begin_, end_);
        return *this;
    }

    bool operator==(FlatIteratorSentinel sentinel) const {
        return begin_ == end_;
    }

    bool operator!=(FlatIteratorSentinel sentinel) const {
        return begin_ != end_;
    }
};

template <typename Range, typename Func>
auto FilterRange(Range& range, Func&& func) {
    return IteratorRange(FilterIterator(range, std::forward<Func>(func)),
                         FilterIteratorSentinel{});
}

}