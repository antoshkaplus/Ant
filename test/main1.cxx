
#include <vector>
#include <tuple>

#include "ant/grid.h"


using namespace std;


using Count = size_t;
using Index = size_t;




// can actually use index operator


// number of dimentions in array
template <Count count>
struct Size {
    size_t size_[count];
    size_t operator()(size_t s) {
        return size_[s];
    }
    
    static constexpr Count C = count;
    
};

template <Count count>
class Position {
public:
    int pos_[count];
    int operator[](size_t s) {
        return pos_[s];
    }
    

};

template <class T, Count ...counts>
class BaseMultiArray;


template <class T, Count ...sizes>
class MultiArray {
    BaseMultiArray<T, sizes...> base_;
    
public:
    
//    MultiArray(S... ss) {
//        
//    }
//    
    
};


template <class T, Count c_0, Count ...counts>
class BaseMultiArray<T, c_0, counts...> {
BaseMultiArray<BaseMultiArray<T, counts...>, c_0> base_;
public:
    template<class ...Sizes>
    BaseMultiArray(Size<c_0> s_0, Sizes... otherSizes) {
        
    }
    
};

template <class T, Count count>
class BaseMultiArray<T, count> {
    T* data_;
    Size<count> size_;
    Index index(Position<count>& p) {
        //Index i = 0;
        Count factor = 1;
        // backward computation
        // 0 is like row should be multiplyed in the end
        for (int k = count-1; k >= 0; --k) {
            factor * p[k];
        }
    }
public:

    T& operator[](Position<count>& p) {
        
    }
   
    const T& operator[](Position<count>& p) const {
        
    } 
};





int main(int argv, char** args) {

    ant::grid::ZobristHashing<64> zooz(ant::grid::Size(54, 67), 8); 




}














