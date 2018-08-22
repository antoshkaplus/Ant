#pragma once

#include "ant/core/core.hpp"


namespace ant {

template<class TContainer, class TIndex>
class SequentialIndexer {
protected:
    TContainer& container;
    TIndex index;

    using Iterator = typename TContainer::iterator;
public:
    using Container = TContainer;

    SequentialIndexer(TContainer& container, TIndex&& index) : container(container), index(std::forward<TIndex>(index)) {
        Reindex();
    }

    Index Reset(Iterator it) {
        auto prev = 0;
        if (it != container.begin()) {
            prev = index(std::prev(it));
        }
        auto next = limit();
        if (std::next(it) != container.end()) {
            next = index(std::next(it));
        }

        index(it) = (prev + next) / 2;
        if (index(it) == prev) {
            Reindex();
        }
        return index(it);
    }

private:
    inline static Count limit() {
        return std::numeric_limits<Index>::max() / 2;
    }

    void Reindex() {
        if (container.empty()) return;
        auto it = container.begin();
        index(it) = 0;
        auto interval = limit() / container.size();
        auto pos = interval;
        for (it = std::next(it); it != container.end(); ++it) {
            index(it) = pos;
            pos += interval;
        }
    }
};

}