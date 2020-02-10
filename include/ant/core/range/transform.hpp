#pragma once

#include "ant/core/core.hpp"

namespace ant::core::range {

struct TransformIteratorSentinel {};

template <typename Range, typename Func>
class TransformIterator {

    using BeginIt = decltype(std::declval<Range>().begin());
    using EndIt = decltype(std::declval<Range>().end());

    BeginIt begin_;
    EndIt end_;
    Func func_;

public:
    TransformIterator(Range& range, Func&& func) : begin_(range.begin()), end_(range.end()),
                                                func_(std::forward<Func>(func)) {}

    // based on dereference type can determine how to return auto, type or reference.
    decltype(func_(*begin_)) operator*() {
        return func_(*begin_);
    }

    TransformIterator& operator++() {
        ++begin_;
        return *this;
    }

    bool operator==(TransformIteratorSentinel sentinel) const {
        return begin_ == end_;
    }

    bool operator!=(TransformIteratorSentinel sentinel) const {
        return begin_ != end_;
    }
};

template <typename Range, typename Func>
auto TransformRange(Range&& range, Func&& func) {
    return IteratorRange(TransformIterator(range, std::forward<Func>(func)),
                          TransformIteratorSentinel{});
}

}