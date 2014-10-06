//
//  operations.h
//  Ant
//
//  Created by Anton Logunov on 6/29/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef __OPERATIONS_LINALG_H__
#define __OPERATIONS_LINALG_H__

namespace ant { 

namespace linalg { 

template<class X, class Y, Operator Op> 
typename std::enable_if<is_matricial<X>::value && is_matricial<Y>::value, 
M_M_Glue<X, Y, Op>>::type  
applyOperator(const X& x, const Y& y) { 
    return M_M_Glue<X, Y, Op>(x, y); 
} 

template<class X, class Y, Operator Op> 
typename std::enable_if<is_matricial<X>::value && !is_matricial<Y>::value,
M_P_Glue<X, Y, Op>>::type
applyOperator(const X& x, const Y& y) { 
    return M_P_Glue<X, Y, Op>(x, y); 
} 

template<class X, class Y, Operator Op>
typename std::enable_if<!is_matricial<X>::value && is_matricial<Y>::value,
P_M_Glue<X, Y, Op>>::type
applyOperator(const X& x, const Y& y) {
    return P_M_Glue<X, Y, Op>(x, y);
} 

// elementwise multiplication
template<class X, class Y>
auto operator%(const X& x, const Y& y) -> decltype(applyOperator<X, Y, Operator::multiplication>(x, y)) { 
    return applyOperator<X, Y, Operator::multiplication>(x, y); 
} 

// elementwise sum
template<class X, class Y> 
auto operator+(const X& x, const Y& y) -> decltype(applyOperator<X, Y, Operator::plus>(x, y)) { 
    return applyOperator<X, Y, Operator::plus>(x, y); 
} 

// elementwise minus
template<class X, class Y> 
auto operator-(const X& x, const Y& y) -> decltype(applyOperator<X, Y, Operator::minus>(x, y)) { 
    return applyOperator<X, Y, Operator::minus>(x, y); 
} 

template<class X, class Y> 
auto operator*(const X& x, const Y& y) -> decltype(applyOperator<X, Y, Operator::multiplication>(x, y)) {
    return applyOperator<X, Y, Operator::multiplication>(x, y);
}




template<class X, class Y>
typename std::enable_if<is_matricial<X>::value 
                        && is_matricial<Y>::value 
                        && X::solid && Y::solid, X&>::type
operator+=(X& x, const Y& y) {
    for (auto i = 0; i < x.element_count(); ++i) {
        x[i] += y[i];
    }
    return x;
}

template<class X, class Y>
typename std::enable_if<is_matricial<X>::value 
                        && is_matricial<Y>::value 
                        && !(X::solid && Y::solid), X&>::type
operator+=(X& x, const Y& y) {
    for (auto r = 0; r < x.row_count(); ++r) {
        for (auto c = 0; c < x.col_count(); ++c) {
            x(r, c) += y(r, c);
        }
    }
    return x;
}

    
template<class X, class Y>
typename std::enable_if<is_matricial<X>::value 
                        && is_matricial<Y>::value 
                        && X::solid && Y::solid, X&>::type
operator-=(X& x, const Y& y) {
    for (auto i = 0; i < x.element_count(); ++i) {
        x[i] -= y[i];
    }
    return x;
}

template<class X, class Y>
typename std::enable_if<is_matricial<X>::value 
                        && is_matricial<Y>::value 
                        && !(X::solid && Y::solid), X&>::type
operator-=(X& x, const Y& y) {
    for (auto r = 0; r < x.row_count(); ++r) {
        for (auto c = 0; c < x.col_count(); ++c) {
            x(r, c) -= y(r, c);
        }
    }
    return x;
}




// think more about this method
template<class M>
typename std::enable_if<is_matricial<M>::value && M::solid, typename M::value_type>::type
sum(const M& m) {
    typename M::value_type s = 0;
    for (auto i = 0; i < m.element_count(); ++i) {
        s += m[i];
    }
    return s;
}

template<class M>
typename std::enable_if<is_matricial<M>::value && !M::solid, typename M::value_type>::type
sum(const M& m) {
    typename M::value_type s = 0;
    for (auto r = 0; r < m.row_count(); ++r) {
        for (auto c = 0; c < m.col_count(); ++c) {
            s += m(r, c);
        }
    }
    return s;
}


} 

}

#endif