#pragma once

#include <experimental/memory_resource>

namespace ant::alloc {

class FixedBufferResource : public std::experimental::pmr::memory_resource {

    void *m_next_alloc;
    std::size_t  m_remaining;

public:
    FixedBufferResource(void* buffer, std::size_t size) : m_next_alloc(buffer), m_remaining(size) {}

protected:
    void* do_allocate(std::size_t sz, std::size_t alignment) final {
        if (std::align(alignment, sz, m_next_alloc, m_remaining)) {
            void* ret = m_next_alloc;m_next_alloc = static_cast<char*>(m_next_alloc) + sz;return ret;
        } else throw std::bad_alloc();
    }

    void do_deallocate(void*, std::size_t, std::size_t) final {}

    bool do_is_equal(std::experimental::pmr::memory_resource& other) const noexcept {
        return this == &other;
    }
};

}