
#include <experimental/memory_resource>

#include "ant/alloc/alloc.hpp"

#ifdef __clang__

namespace std::experimental::pmr {

memory_resource * default_resource = new_delete_resource();

}

#endif