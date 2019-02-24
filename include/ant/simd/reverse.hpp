#pragma once

#include "ant/core/core.hpp"


namespace ant::simd::impl {

    template<Count kElementSize>
    void Reverse(void* arr, Count elementCount);

}


#if defined(__AVX2__)
    #include "avx2/reverse.hpp"
#elif defined(__SSSE3__)
    #include "ssse3/reverse.hpp"
#endif

namespace ant::simd {

    template <typename TValue>
    void Reverse(TValue* arr, Count count) {
        impl::Reverse<sizeof(TValue)>(arr, count);
    }


}