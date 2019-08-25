#pragma once

#include <experimental/memory_resource>

#include "ring_resource_base.hpp"

namespace ant::alloc {

// if out of alloc slots or buffer, allocate with standard alloc

class RingResource : public std::experimental::pmr::memory_resource {

    RingResourceBase base_;

public:
    RingResource(uint32_t buffer_size, uint32_t slot_count, uint32_t max_dealloc_count = 10)
        : base_(buffer_size, slot_count, max_dealloc_count) {}

protected:

    void* do_allocate(std::size_t sz, std::size_t alignment) final {
        return base_.do_allocate(sz, alignment);
    }

    void do_deallocate(void* ptr, std::size_t sz, std::size_t alignment) final {
        return base_.do_deallocate(ptr, sz, alignment);
    }

    bool do_is_equal(const std::experimental::pmr::memory_resource& other) const noexcept final {
        return base_.do_is_equal(other);
    }
};

}