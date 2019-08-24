#pragma once

#include <experimental/memory_resource>

#include "ant/core/core.hpp"
#include "ant/alloc/alloc.hpp"

namespace ant::alloc {

class LocalResource;

class LocalResourceManager {

    // destructs as if only one element is inside
    std::unique_ptr<char> memory;
    char* next_alloc;

    std::size_t remaining;

    struct Item {
        LocalResource* resource;
        char* start_alloc;

        Item(LocalResource* resource, char* start_alloc) : resource(resource), start_alloc(start_alloc) {}
    };

    std::vector<Item> local;

    bool allocation_failed_ = false;

public:

    LocalResourceManager(std::size_t size) : memory(new char[size]), next_alloc(memory.get()), remaining(size) {}

    bool allocation_failed() const {
        return allocation_failed_;
    }

    std::size_t used() const {
        return next_alloc - memory.get();
    }

private:

    void* allocate(std::size_t sz, std::size_t alignment) {
        if (std::align(alignment, sz, (void*&)next_alloc, remaining)) {
            void* ret = next_alloc;
            next_alloc = next_alloc + sz;
            return ret;

        } else {
            allocation_failed_ = true;
            return nullptr;
        }
    }

    bool deallocate(void* ptr) {
        if (ptr >= memory.get() && ptr < next_alloc) return true;
        return false;
    }

    friend class LocalResource;
};

class LocalResource : public std::experimental::pmr::memory_resource {

    LocalResourceManager* manager;

public:
    LocalResource(LocalResourceManager& manager) {
        this->manager = &manager;
        manager.local.emplace_back(this, manager.next_alloc);
    }

    ~LocalResource() {
        if (manager->local.back().resource != this) {
            std::cerr << "out of order resource destruction" << std::endl;
        }
        manager->next_alloc = manager->local.back().start_alloc;
        manager->local.pop_back();
    }

    DISALLOW_COPY_AND_ASSIGN(LocalResource);

protected:
    void* do_allocate(std::size_t sz, std::size_t alignment) final {
        if (manager->local.back().resource == this) {
            auto res = manager->allocate(sz, alignment);
            if (res != nullptr) return res;
        }
        return std::experimental::pmr::new_delete_resource()->allocate(sz, alignment);
    }

    void do_deallocate(void* ptr, std::size_t sz, std::size_t alignment) final {
        if (!manager->deallocate(ptr)) {
            std::experimental::pmr::new_delete_resource()->deallocate(ptr, sz, alignment);
        }
    }

    bool do_is_equal(const std::experimental::pmr::memory_resource& other) const noexcept final {
        return this == &other;
    }
};

}