#pragma once

#include <vector>
#include <unordered_map>
#include <experimental/memory_resource>


namespace std::experimental::pmr {

memory_resource *new_delete_resource() noexcept {
    static resource_adaptor <std::allocator<char>> __r;
    return static_cast<std::experimental::pmr::memory_resource *>(&__r);
}

memory_resource *get_default_resource() noexcept {
    return new_delete_resource();
}

}

namespace ant::alloc {

template<class T>
using vector = std::vector<T, std::experimental::pmr::polymorphic_allocator<T>>;

template<class Key, class Value>
using unordered_map = std::unordered_map<Key, Value, std::hash<Key>, std::equal_to<Key>,
    std::experimental::pmr::polymorphic_allocator<std::pair<const Key, Value>>>;

}