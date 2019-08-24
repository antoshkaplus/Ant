#pragma once

#include <vector>
#include <unordered_map>
#include <experimental/memory_resource>

#ifdef __clang__

namespace std::experimental::pmr {

memory_resource *new_delete_resource() noexcept {
    static resource_adaptor <std::allocator<char>> __r;
    return static_cast<std::experimental::pmr::memory_resource *>(&__r);
}

memory_resource *get_default_resource() noexcept {
    return new_delete_resource();
}

}

#endif

namespace ant::alloc {

template<class T>
using vector = std::vector<T, std::experimental::pmr::polymorphic_allocator<T>>;

template<class Key, class Value>
using unordered_map = std::unordered_map<Key, Value, std::hash<Key>, std::equal_to<Key>,
    std::experimental::pmr::polymorphic_allocator<std::pair<const Key, Value>>>;


struct DefaultResourceGuard {
    template <typename Resource>
    DefaultResourceGuard(Resource& resource) {
        std::experimental::pmr::set_default_resource(&resource);
    }

    ~DefaultResourceGuard() {
        std::experimental::pmr::set_default_resource(std::experimental::pmr::new_delete_resource());
    }
};

}

namespace std {

template<typename T>
bool operator==(const std::vector<T>& v_1, const std::vector<T, std::experimental::pmr::polymorphic_allocator<T>>& v_2) {
    return v_1.size() == v_2.size() &&
           std::equal(v_1.begin(), v_1.end(), v_2.begin());
}

template<typename T>
bool operator==(const std::vector<T, std::experimental::pmr::polymorphic_allocator<T>>& v_1, const std::vector<T>& v_2) {
    return v_2 == v_1;
}

}