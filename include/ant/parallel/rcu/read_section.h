#pragma once

//#define _LGPL_SOURCE
//#define URCU_INLINE_SMALL_FUNCTIONS

#include <urcu-qsbr.h>    /* RCU flavor */

namespace ant::bluefin::rcu {

class read_section {
public:
    read_section() {
        rcu_read_lock();
    }

    read_section(const read_section&) = delete;

    read_section& operator=(const read_section&) = delete;

    ~read_section() {
        rcu_read_unlock();
    }
};

}
