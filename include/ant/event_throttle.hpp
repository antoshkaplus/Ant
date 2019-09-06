#pragma once

#include <type_traits>

namespace ant {

template <typename Duration = std::chrono::milliseconds>
class EventThrottle {

    static_assert(std::is_same<Duration, std::chrono::duration<typename Duration::rep, typename Duration::period>>::value);

    std::vector<std::chrono::steady_clock::time_point> slots{};
    size_t index = 0;
    Duration interval_ = 1000;

public:

    EventThrottle() = default;

    EventThrottle(size_t limit, Duration interval)
            : slots(limit), interval_(interval) {
    }

    void Authorize(const std::chrono::steady_clock::time_point& now) {
        if (slots.empty()) return;

        slots[index] = now;
        index = (index + 1) % slots.size();
    }

    bool TryAuthorize(const std::chrono::steady_clock::time_point& now) {
        if (!can_authorize(now, 1)) return false;
        Authorize(now);
        return true;
    }

    bool can_authorize(const std::chrono::steady_clock::time_point& now, size_t count) {
        if (count == 0) return true;
        if (count > slots.size()) return false;

        auto nextIndex = (this->index + count - 1) % slots.size();
        auto&& duration = std::chrono::duration_cast<Duration>(now - slots[nextIndex]);
        return !(duration.count() >= 0 && duration < interval_);
    }

    size_t limit() const {
        return slots.size();
    }

    void set_limit(size_t value) {
        slots.resize(value);
        if (index >= value) index = 0;
    }

    Duration interval() const {
        return interval_;
    }
};

}