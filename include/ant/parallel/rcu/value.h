#pragma once

#include <urcu-qsbr.h>          /* RCU flavor */
#include <urcu/rculfhash.h>    /* RCU Lock-free hash table */

#include "read_section.h"

namespace ant::parallel::rcu {

template<typename TValue>
class value {
    struct handle {
        rcu_head head{};

        handle() = default;
    };

    using handle_type = value::handle;

    struct item : public handle {
        TValue value;

        template<typename... TArgs>
        item(TArgs&& ... args)
                : handle(), value(std::forward<TArgs>(args)...) {
        }

        item(const item&) = delete;

        item& operator=(const item&) = delete;
    };

    using item_type = value::item;

    item_type* item = nullptr;

public:
    value() {
        read_section lock;

        rcu_assign_pointer(item, new item_type());
    }

    value(const value&) = delete;

    value(value&&) = delete;

    value& operator=(const TValue& other) {
        read_section lock;

        call_rcu(&rcu_dereference(item)->head, erase);

        rcu_assign_pointer(item, new item_type(other));

        return *this;
    }

    TValue& get() {
        read_section lock;

        return rcu_dereference(item)->value;
    }

private:
    inline static void erase(rcu_head* head) {
        delete static_cast<item_type*>(caa_container_of(head, handle_type, head));
    }
};

}
