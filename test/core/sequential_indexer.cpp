
#include "gtest/gtest.h"

#include "ant/core/sequential_indexer.hpp"

using namespace ant;

void Check(const std::list<Index>& list) {
    for (auto it = list.begin(); it != list.end(); ++it) {
        auto next = std::next(it);
        if (next == list.end()) return;

        ASSERT_LT(*it, *next);
    }
}

TEST(SequentialIndexer, allin) {
    const Count N = 100;
    std::list<Index> list(N, 1);
    SequentialIndexer indexer(list, [](std::list<Index>::iterator it) -> Index& {
        return *it;
    });
    Check(list);

    for (auto it = list.begin(); it != list.end(); ++it, ++it) {
        it = list.emplace(it, 1);
        indexer.Reset(it);
        Check(list);
    }

    for (auto i = 0; i < N; ++i) {
        indexer.Reset(list.emplace(list.end(), 1));
        Check(list);
    }

    for (auto i = 0; i < N; ++i) {
        indexer.Reset(list.emplace(list.begin(), 1));
        Check(list);
    }
}