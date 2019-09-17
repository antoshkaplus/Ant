#pragma once

#include <experimental/memory_resource>

namespace ant::alloc {

// if out of alloc slots or buffer, allocate with standard alloc

class RingResourceBase : public std::experimental::pmr::memory_resource {

    std::vector<char> buffer;
    std::vector<uint32_t> slots;

    uint32_t slots_start = 0;
    uint32_t slots_taken = 0;

    char* buffer_next_free;

    constexpr static char TAKEN = 1;
    constexpr static char EMPTY = 0;

    uint32_t max_dealloc_count;

    size_t local_allocations_ = 0;
    size_t local_deallocations_ = 0;
    size_t local_slot_releases_ = 0;

    size_t standard_allocations_ = 0;
    size_t standard_deallocations_ = 0;

public:
    RingResourceBase(uint32_t buffer_size, uint32_t slot_count, uint32_t max_dealloc_count = 10)
            : buffer(buffer_size, EMPTY), slots(slot_count), buffer_next_free(buffer.data()),
              max_dealloc_count(max_dealloc_count) {

        if (buffer_size <= 1) throw std::runtime_error("buffer smaller 2");
        if (slots.size() == 0) throw std::runtime_error("slots count is 0");
    }

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

    size_t local_slot_releases() const {
        return local_slot_releases_;
    }

public:
    void* do_allocate(std::size_t sz, std::size_t alignment) final override {

        assert(sz != 0);

        if (slots_taken == 0)
        {
            return allocate_no_slots_taken(sz, alignment);
        }

        try_release_slots();

        if (slots_taken == slots.size()) {
            return allocate_standard(sz, alignment);
        }

        auto slots_start_ptr = buffer.data() + slots[slots_start];

        assert(slots_start_ptr != buffer_next_free);

        if (buffer_next_free > slots_start_ptr)
        {
            return allocate_start_behind(sz, alignment);
        }

        // buffer_next_free < slots_start_ptr
        // equality not allowed
        return allocate_start_ahead(sz, alignment);
    }

    void do_deallocate(void* ptr, std::size_t sz, std::size_t alignment) final override {
        if (ptr < buffer.data() || ptr >= buffer.data() + buffer.size()) {
            ++standard_deallocations_;
            std::experimental::pmr::new_delete_resource()->deallocate(ptr, sz, alignment);
            return;
        }

        reinterpret_cast<char*>(ptr)[-1] = EMPTY;
        ++local_deallocations_;

        try_release_slots();
    }

    bool do_is_equal(const std::experimental::pmr::memory_resource& other) const noexcept final override {
        return this == &other;
    }

private:

    void try_release_slots()
    {
        auto dealloc_count = 0;
        while ( slots_taken > 0 &&
                buffer[slots[slots_start]] == EMPTY &&
                ++dealloc_count <= max_dealloc_count) {

            ++local_slot_releases_;

            slots_start = (slots_start + 1) % slots.size();
            --slots_taken;
        }
    }

    void* allocate_no_slots_taken(std::size_t sz, std::size_t alignment) {

        // mark + buffer_next_free
        size_t remaining = buffer.size() - 1;
        auto mark = buffer.data();
        auto alloc_start = mark + 1;

        if (!std::align(alignment, sz, (void*&)alloc_start, remaining) || remaining <= sz) {
            return allocate_standard(sz, alignment);
        }

        commit_allocation(alloc_start, sz);

        return alloc_start;
    }

    void* allocate_start_behind(std::size_t sz, std::size_t alignment) {

        // size - taken items - 1 (mark)
        size_t remaining = buffer.size() - (buffer_next_free - buffer.data()) - 1;
        auto mark = buffer_next_free;
        auto alloc_start = mark + 1;

        // require buffer_next_free to follow
        if (!std::align(alignment, sz, (void*&)alloc_start, remaining) || remaining <= sz) {
            if (slots[slots_start] == 0)
            {
                return allocate_standard(sz, alignment);
            }
            buffer_next_free = buffer.data();
            return allocate_start_ahead(sz, alignment);
        }

        commit_allocation(alloc_start, sz);

        return alloc_start;

    }

    void* allocate_start_ahead(std::size_t sz, std::size_t alignment) {

        size_t remaining = (buffer.data() + slots[slots_start]) - buffer_next_free - 1;
        auto mark = buffer_next_free;
        auto alloc_start = mark + 1;

        if (!std::align(alignment, sz, (void*&)alloc_start, remaining) || remaining <= sz) {
            return allocate_standard(sz, alignment);
        }

        commit_allocation(alloc_start, sz);

        return alloc_start;
    }

    void commit_allocation(char* alloc_start, size_t sz) {
        ++local_allocations_;

        *(alloc_start - 1) = TAKEN;
        buffer_next_free = alloc_start + sz;
        assert(buffer_next_free >= buffer.data() && buffer_next_free < buffer.data() + buffer.size());

        slots[(slots_start + slots_taken) % slots.size()] = alloc_start - 1 - buffer.data();
        ++slots_taken;
    }

    void* allocate_standard(std::size_t sz, std::size_t alignment) {
        ++standard_allocations_;
        return std::experimental::pmr::new_delete_resource()->allocate(sz, alignment);
    }
};

}