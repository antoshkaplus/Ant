
#include <vector>
#include <string>
#include <list>

#include "benchmark/benchmark.h"

#include "ant/alloc/fixed_buffer_resource.hpp"

class ShoppingList {
    // Define a vector of strings using polymorphic allocators.  polymorphic_allocator is scoped,
    // so every element of the vector will use the same allocator as the vector itself.

    using string_type = std::basic_string<char, std::char_traits<char>, std::experimental::pmr::polymorphic_allocator<char>>;
    using strvec_type = std::vector<string_type, std::experimental::pmr::polymorphic_allocator<string_type>> ;
    strvec_type m_strvec;

public:
    using allocator_type = std::experimental::pmr::memory_resource*;

    // could constract inplace if memory resource is provided

    // this is sounds like not cool at all
    ShoppingList(allocator_type alloc = nullptr) : m_strvec(alloc) {}

    // Copy construct with optional memory_resource.
    //  If alloc is not specified, uses pmr::get_default_resource().
    ShoppingList(const ShoppingList& other) = default;

    ShoppingList(allocator_type a, const ShoppingList& other) : m_strvec(other.m_strvec, a) {


    }

    allocator_type get_allocator() const {
        return m_strvec.get_allocator().resource();
    }

    void add_item(const string_type& item) {
        m_strvec.push_back(item);
    }

};

bool operator==(const ShoppingList &a, const ShoppingList &b);

template<class T>
using list = std::list<T, std::experimental::pmr::polymorphic_allocator<T>>;

namespace std::experimental { inline namespace fundamentals_v1 { namespace pmr {

    memory_resource* new_delete_resource() noexcept {
        static resource_adaptor<std::allocator<char>> __r;
        return static_cast<memory_resource*>(&__r);
    }

    memory_resource* get_default_resource() noexcept {
        return new_delete_resource();
    }
}}}

//bool processShoppingList(ShoppingList&) { return true; }
//
//static void FixedBufferAllocator_0(benchmark::State& state) {
//    list<ShoppingList> folder;
//    // Default allocator resource
//    {
//        char buffer[1024];
//        ant::alloc::FixedBufferResource buf_rsrc(&buffer, 1024);
//        ShoppingList temporaryShoppingList(&buf_rsrc);
//        assert(&buf_rsrc == temporaryShoppingList.get_allocator());
//        temporaryShoppingList.add_item("salt");
//        temporaryShoppingList.add_item("pepper");
//        if (processShoppingList(temporaryShoppingList)) {
//            folder.push_back(temporaryShoppingList);
//            assert(std::experimental::pmr::get_default_resource() ==folder.back().get_allocator());
//        }
//        // temporaryShoppingList, buf_rsrc, andbuffer go out of scope
//    }
//}
//
//BENCHMARK(FixedBufferAllocator_0);