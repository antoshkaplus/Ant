#pragma once

#include "core.hpp"


namespace ant {

template<class T>
static void SwapNotInOrder(T& i_1, T& i_2) {
    if (i_1 > i_2) std::swap(i_1, i_2);
}

template<class T>
void Sort(std::array<T, 2>& arr) {
    SwapNotInOrder(arr[0], arr[1]);
}

template<class T>
void Sort(std::array<T, 3>& arr) {
    SwapNotInOrder(arr[1], arr[2]);
    SwapNotInOrder(arr[0], arr[2]);
    SwapNotInOrder(arr[0], arr[1]);
}

template<class T>
bool Find(std::array<T, 3>& arr, const T& t) {
    return arr[0] == t || arr[1] == t || arr[2] == t;
}



}