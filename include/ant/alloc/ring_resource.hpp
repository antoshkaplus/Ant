#pragma once

#include <experimental/memory_resource>

namespace ant::alloc {

// if out of alloc slots or buffer, allocate with standard alloc
template <typename slot_size_type>
class RingResource : public std::experimental::pmr::memory_resource {

    std::vector<char> buffer;
    std::vector<slot_size_type> slots;

    slot_size_type slots_start = 0;
    slot_size_type slots_taken = 0;

    char* buffer_next_free;

    constexpr static char TAKEN = 1;
    constexpr static char EMPTY = 0;

    slot_size_type max_dealloc_count;

    size_t local_allocations_ = 0;
    size_t local_deallocations_ = 0;

    size_t standard_allocations_ = 0;
    size_t standard_deallocations_ = 0;

public:
    RingResource(size_t buffer_size, slot_size_type slot_count, slot_size_type max_dealloc_count = 10)
        : buffer(buffer_size, EMPTY), slots(slot_count), buffer_next_free(buffer.data()),
          max_dealloc_count(max_dealloc_count) {}

    size_t standard_allocations() const {
        return standard_allocations_;
    }

    size_t standard_deallocations() const {
        return standard_deallocations_;
    }

    size_t local_allocations() const {
        return local_allocations_;
    }

    size_t local_deallocations() const {
        return local_deallocations_;
    }


protected:

    void* do_allocate(std::size_t sz, std::size_t alignment) final {
        if (slots_taken == slots.size()) {
            return allocate_standard(sz, alignment);
        }

        if (slots_taken == 0)
        {
            return allocate_no_slots_taken(sz, alignment);
        }

        auto slots_start_ptr = buffer.data() + slots[slots_start];

        if (buffer_next_free > slots_start_ptr)
        {
            return allocate_start_behind(sz, alignment);
        }

        // buffer_next_free < slots_start_ptr
        // equality not allowed
        return allocate_start_ahead(sz, alignment);
    }

    void do_deallocate(void* ptr, std::size_t sz, std::size_t alignment) final {
        if (ptr < buffer.data() && ptr >= buffer.data() + sz) {
            ++standard_deallocations_;
            std::experimental::pmr::new_delete_resource()->deallocate(ptr, sz, alignment);
            return;
        }

        reinterpret_cast<char*>(ptr)[-1] = EMPTY;

        auto dealloc_count = 0;
        while ( slots_taken > 0 &&
                slots[slots_start] == EMPTY &&
                ++dealloc_count <= max_dealloc_count) {

            ++local_deallocations_;

            slots_start += (slots_start + 1) % slots.size();
            --slots_taken;
        }
    }

    bool do_is_equal(const std::experimental::pmr::memory_resource& other) const noexcept final {
        return this == &other;
    }

private:

    void* allocate_no_slots_taken(std::size_t sz, std::size_t alignment) {

        size_t remaining = 0;
        auto mark = buffer.data();
        auto alloc_start = mark + 1;

        if (!std::align(alignment, sz, (void*&)alloc_start, remaining)) {
            return allocate_standard(sz, alignment);
        }

        if (alloc_start + sz >= buffer.data() + buffer.size())
        {
            return allocate_standard(sz, alignment);
        }

        commit_allocation(alloc_start, sz);

        return mark;
    }

    void* allocate_start_behind(std::size_t sz, std::size_t alignment) {

        size_t remaining = 0;
        auto mark = buffer_next_free;
        auto alloc_start = mark + 1;

        if (!std::align(alignment, sz, (void*&)alloc_start, remaining)) {
            return allocate_standard(sz, alignment);
        }

        if (alloc_start + sz >= buffer.data() + buffer.size())
        {
            if (slots[slots_start] == 0)
            {
                return allocate_standard(sz, alignment);
            }
            buffer_next_free = 0;
            return allocate_start_ahead(sz, alignment);
        }

        commit_allocation(alloc_start, sz);

        return mark;

    }

    void* allocate_start_ahead(std::size_t sz, std::size_t alignment) {

        size_t remaining = 0;
        auto mark = buffer_next_free;
        auto alloc_start = mark + 1;

        if (!std::align(alignment, sz, (void*&)alloc_start, remaining)) {
            return allocate_standard(sz, alignment);
        }

        if (alloc_start + sz >= buffer.data() + slots[slots_start])
        {
            return allocate_standard(sz, alignment);
        }

        commit_allocation(alloc_start, sz);

        return mark;
    }

    void commit_allocation(char* alloc_start, size_t sz) {
        ++local_allocations_;

        *(alloc_start - 1) = TAKEN;
        buffer_next_free = alloc_start + sz;
        slots[(slots_start + slots_taken) % slots.size()] = alloc_start - 1 - buffer.data();
        ++slots_taken;
    }

    void* allocate_standard(std::size_t sz, std::size_t alignment) {
        ++standard_allocations_;
        return std::experimental::pmr::new_delete_resource()->allocate(sz, alignment);
    }
};

}