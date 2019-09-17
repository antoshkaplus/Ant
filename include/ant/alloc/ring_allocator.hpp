#pragma once

namespace ant::alloc {

template <typename T, size_t item_capacity, size_t allocated_capacity>
struct RingAllocator {
    using value_type = T;

    class Mark {
        bool value[sizeof(T)];

    public:
        void set() {
            value[0] = true;
        }

        void reset() {
            value[0] = false;
        }

        bool marked() const {
            return value[0];
        }
    };

    std::allocator<T> underlying;

    std::vector<T> items;

    std::vector<int> allocated;
    // indices in allocated array, and by those indices you find which items are used
    // when equal should consider there is no allocations made
    size_t allocated_start = 0;
    size_t allocated_next = 0;

    RingAllocator() : allocated(allocated_capacity, -1) {
        items.reserve(item_capacity);

        // maybe initialize allocated capacity
//        allocated_start;
//        allocated_last;
//        if (allocated_start == allocated_last) <- no elements
    }

    T* allocate(std::size_t n) {
        auto allocate_n = n+1;
        T* ptr = nullptr;

        // gives back starting index of last allocated

        // still depends on allocation start
        if (allocated_next + allocate_n - 1 > item_capacity) {
            if (allocate_n > allocated_start)
            {
                // not enough space for allocation
                return underlying.allocate(n);
            }
            ptr = items.data();
            allocated[0] = 0;
            allocated_next = allocate_n;
        } else {
            if (allocated_next + allocate_n - 1 > allocated_start)
                ;
        }

        reinterpret_cast<Mark>(ptr[0]).set();
        return ptr + 1;
    }
    void deallocate(T* ptr, std::size_t n) {
        // take a look to which allocator belongs
        reinterpret_cast<Mark*>(ptr - 1)->reset();
        if (ptr - items.data() == allocated[allocated_start])
        {
            // move the pointer
        }
    }
};

}