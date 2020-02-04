
#include "gtest/gtest.h"

#include "ant/core/flat_iterator.hpp"


using namespace ant;
using namespace std;


TEST(FlatIterator, _1) {
    std::vector<int> vs;
    FlatRange(vs.begin(), vs.end());
}

TEST(FlatIterator, _2) {
    std::unordered_map<int, int> vs;
    FlatRange(vs.begin(), vs.end());
}

TEST(FlatIterator, _3) {
    std::unordered_map<int, std::vector<int>> vs;
    FlatRange(vs.begin(), vs.end(), [](auto& p) { return p.second; });
}

TEST(FlatIterator, _4) {
    auto identity = [](auto& v) -> auto& { return v; };

    std::vector<int> vs_1{1, 2};
    {
        auto r = FlatRange(vs_1.begin(), vs_1.end());
        std::vector<int> vs_flat{};
        for (auto p : r) {
            vs_flat.push_back(p);
        }
        ASSERT_EQ(vs_flat, vs_1);
    }
    std::vector<std::vector<int>> vs_2{1, vs_1};
    {
        auto r = FlatRange(vs_2.begin(), vs_2.end(), identity);
        std::vector<int> vs_flat{};
        for (auto p : r) {
            vs_flat.push_back(p);
        }
        ASSERT_EQ(vs_flat, vs_1);
    }
    std::vector<std::vector<std::vector<int>>> vs_3{1, vs_2};
    {
        auto r = FlatRange(vs_3.begin(), vs_3.end(), identity, identity);
        std::vector<int> vs_flat{};
        for (auto p : r) {
            vs_flat.push_back(p);
        }
        ASSERT_EQ(vs_flat, vs_1);
    }
}

// I need to check that reference is used instead of value where needed.
struct Item {
    static int constructor_call_count;

    Item() {
        ++constructor_call_count;
    }
    Item(const Item&) {
        ++constructor_call_count;
    }
};
int Item::constructor_call_count = 0;

TEST(FlatIterator, DereferenceUsesReference) {

    std::vector<Item> items(10);
    {
        auto r = FlatRange(items.begin(), items.end());
        ASSERT_EQ(Item::constructor_call_count, 10);

        for (auto& t : r) (void) t;
        ASSERT_EQ(Item::constructor_call_count, 10);
    }

    std::vector<vector<Item>> nested_items(10, items);
    {
        auto r = FlatRange(nested_items.begin(), nested_items.end());
        ASSERT_EQ(Item::constructor_call_count, 110);

        for (auto& t : r) (void) t;
        ASSERT_EQ(Item::constructor_call_count, 110);
    }
}

template <typename T>
class Iterator : std::iterator<std::forward_iterator_tag, T> {
public:
    Iterator() = default;
    Iterator(size_t position, T& value)
        : position(position), value(&value) {}

    auto operator*() {
        return *value;
    }

    bool operator==(const Iterator& it) const {
        return position == it.position;
    }

    bool operator!=(const Iterator& it) const {
        return position != it.position;
    }

    // pred
    Iterator& operator++() {
        ++position;
        return *this;
    }

private:
    size_t position = 0;
    T* value = nullptr;
};

struct ItemValue {
    static int constructor_call_count;

    ItemValue() {
        ++constructor_call_count;
    }
    ItemValue(const ItemValue&) {
        ++constructor_call_count;
    }
};
int ItemValue::constructor_call_count = 0;


TEST(FlatIterator, DereferenceUsesValue) {
    {
        Item::constructor_call_count = 0;
        Item item{};
        auto begin = Iterator(0, item);
        auto end = Iterator(10, item);
        auto r = FlatRange(begin, end);
        for (auto&& t : r) (void) t;
        ASSERT_EQ(Item::constructor_call_count, 11);
    }
}