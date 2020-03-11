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
    FilterIterator() : func_([](decltype(*begin_)&) { return true; }) {}
    FilterIterator(Range& range, Func func)
        : begin_(range.begin()), end_(range.end()), func_(std::move(func)) {

        FindFirst();
    }

    bool FindFirst() {
        while (begin_ != end_) {
            auto&& value = *begin_;
            if (func_(value)) return true;
            ++begin_;
        }
        return false;
    }

    // based on dereference type can determine how to return auto, type or reference.
    decltype(*begin_) operator*() {
        return *begin_;
    }

    FilterIterator& operator++() {
        ++begin_;
        FindFirst();
        return *this;
    }

    bool operator==(FilterIteratorSentinel sentinel) const {
        return begin_ == end_;
    }

    bool operator!=(FilterIteratorSentinel sentinel) const {
        return begin_ != end_;
    }
};

template <typename Range, typename Func>
auto FilterRange(Range&& range, Func&& func) {
    return IteratorRange(FilterIterator(range, std::forward<Func>(func)),
                         FilterIteratorSentinel{});
}

}