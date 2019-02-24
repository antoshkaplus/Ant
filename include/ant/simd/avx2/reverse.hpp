#pragma once

#include <immintrin.h>

#include "ant/core/core.hpp"

namespace ant::simd::impl {

template <>
void Reverse<1>(void* arr, Count Count) {

    std::uint8_t *Array8 = reinterpret_cast<std::uint8_t *>(arr);
    std::size_t i = 0;

    for( std::size_t j = i / 32; j < ((Count / 2) / 32); ++j )
    {
        const __m256i ShuffleRev = _mm256_set_epi8(
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15
        );
        // Load 32 elements at once into one 32-byte register
        __m256i Lower = _mm256_loadu_si256(
            reinterpret_cast<__m256i*>(&Array8[i])
        );
        __m256i Upper = _mm256_loadu_si256(
            reinterpret_cast<__m256i*>(&Array8[Count - i - 32])
        );

        // Reverse the byte order of our 32-byte vectors
        Lower = _mm256_shuffle_epi8(Lower,ShuffleRev);
        Upper = _mm256_shuffle_epi8(Upper,ShuffleRev);

        Lower = _mm256_permute2x128_si256(Lower,Lower,1);
        Upper = _mm256_permute2x128_si256(Upper,Upper,1);

        // Place them at their swapped position
        _mm256_storeu_si256(
            reinterpret_cast<__m256i*>(&Array8[i]),
            Upper
        );
        _mm256_storeu_si256(
            reinterpret_cast<__m256i*>(&Array8[Count - i - 32]),
            Lower
        );

        // 32 elements at a time
        i += 32;
    }

    std::reverse(Array8+i, Array8+Count-i);
}

template <>
void Reverse<2>(void* arr, Count Count) {

    std::uint16_t *Array16 = reinterpret_cast<std::uint16_t *>(arr);
    std::size_t i = 0;

    for( std::size_t j = i / 16; j < ((Count / 2) / 16); ++j )
    {
        const __m256i ShuffleRev = _mm256_set_epi8(
            1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14,
            1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14
        );
        // Load 16 elements at once into one 32-byte register
        __m256i Lower = _mm256_loadu_si256(
            reinterpret_cast<__m256i*>(&Array16[i])
        );
        __m256i Upper = _mm256_loadu_si256(
            reinterpret_cast<__m256i*>(&Array16[Count - i - 16])
        );

        Lower = _mm256_shuffle_epi8(Lower,ShuffleRev);
        Upper = _mm256_shuffle_epi8(Upper,ShuffleRev);

        Lower = _mm256_permute2x128_si256(Lower,Lower,1);
        Upper = _mm256_permute2x128_si256(Upper,Upper,1);

        // Place them at their swapped position
        _mm256_storeu_si256(
            reinterpret_cast<__m256i*>(&Array16[i]),
            Upper
        );
        _mm256_storeu_si256(
            reinterpret_cast<__m256i*>(&Array16[Count - i - 16]),
            Lower
        );

        // 32 elements at a time
        i += 16;
    }

    std::reverse(Array16+i, Array16+Count-i);
}

template <>
void Reverse<4>(void* arr, Count Count) {

    std::uint32_t *Array32 = reinterpret_cast<std::uint32_t *>(arr);
    std::size_t i = 0;

    for( std::size_t j = i / 8; j < ((Count / 2) / 8); ++j )
    {
        // Load 8 elements at once into one 32-byte register
        __m256i Lower = _mm256_loadu_si256(
            reinterpret_cast<__m256i*>(&Array32[i])
        );
        __m256i Upper = _mm256_loadu_si256(
            reinterpret_cast<__m256i*>(&Array32[Count - i - 8])
        );

        Lower = _mm256_shuffle_epi32(Lower, _MM_SHUFFLE(0,1,2,3) );
        Upper = _mm256_shuffle_epi32(Upper, _MM_SHUFFLE(0,1,2,3) );

        Lower = _mm256_permute2x128_si256(Lower,Lower,1);
        Upper = _mm256_permute2x128_si256(Upper,Upper,1);

        // Place them at their swapped position
        _mm256_storeu_si256(
            reinterpret_cast<__m256i*>(&Array32[i]),
            Upper
        );
        _mm256_storeu_si256(
            reinterpret_cast<__m256i*>(&Array32[Count - i - 8]),
            Lower
        );

        // 8 elements at a time
        i += 8;
    }

    std::reverse(Array32+i, Array32+Count-i);
}

template <>
void Reverse<8>(void* arr, Count Count) {

    std::uint64_t *Array64 = reinterpret_cast<std::uint64_t *>(arr);
    std::size_t i = 0;

    for( std::size_t j = i / 4; j < ((Count / 2) / 4); ++j )
    {
        // Load 4 elements at once into one 32-byte register
        __m256i Lower = _mm256_loadu_si256(
            reinterpret_cast<__m256i*>(&Array64[i])
        );
        __m256i Upper = _mm256_loadu_si256(
            reinterpret_cast<__m256i*>(&Array64[Count - i - 4])
        );

        Lower = _mm256_alignr_epi8(Lower, Lower, 8);
        Upper = _mm256_alignr_epi8(Upper, Upper, 8);

        Lower = _mm256_permute2x128_si256(Lower,Lower,1);
        Upper = _mm256_permute2x128_si256(Upper,Upper,1);

        // Place them at their swapped position
        _mm256_storeu_si256(
            reinterpret_cast<__m256i*>(&Array64[i]),
            Upper
        );
        _mm256_storeu_si256(
            reinterpret_cast<__m256i*>(&Array64[Count - i - 4]),
            Lower
        );

        // 4 elements at a time
        i += 4;
    }

    std::reverse(Array64+i, Array64+Count-i);
}


}