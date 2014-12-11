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


template<Operator Op, class X, class Y,  
typename std::enable_if< is_matricial<X>::value && is_matricial<Y>::value, int >::type = 0>
auto applyOperator(const X& x, const Y& y) { 
    return M_M_Glue<Op, X, Y>(x, y); 
} 

template<Operator Op, class X, class Y, class T,
typename std::enable_if< is_matricial<X>::value && is_matricial<Y>::value, int >::type = 0>
auto applyOperator(const X& x, const Y& y) { 
    return M_M_Glue<Op, X, Y, T>(x, y); 
} 

template<Operator Op, class X, class Y, 
typename std::enable_if< is_matricial<X>::value && !is_matricial<Y>::value, int >::type = 0> 
auto applyOperator(const X& x, const Y& y) { 
    return M_P_Glue<Op, X, Y>(x, y); 
} 

template<Operator Op, class X, class Y, class T,
typename std::enable_if< is_matricial<X>::value && !is_matricial<Y>::value, int >::type = 0> 
auto applyOperator(const X& x, const Y& y) { 
    return M_P_Glue<Op, X, Y, T>(x, y); 
} 

template<Operator Op, class X, class Y,
typename std::enable_if< !is_matricial<X>::value && is_matricial<Y>::value, int >::type = 0>
auto applyOperator(const X& x, const Y& y) {
    return P_M_Glue<Op, X, Y>(x, y);
} 
    
template<Operator Op, class X, class Y, class T, 
typename std::enable_if< !is_matricial<X>::value && is_matricial<Y>::value, int >::type = 0>
auto applyOperator(const X& x, const Y& y) {
    return P_M_Glue<Op, X, Y, T>(x, y);
} 




template<class X, class Y>
auto operator%(const X& x, const Y& y) -> decltype(applyOperator<Operator::multiplication, X, Y>(x, y)) { 
    return applyOperator<Operator::multiplication, X, Y>(x, y); 
} 

template<class X, class Y> 
auto operator+(const X& x, const Y& y) -> decltype(applyOperator<Operator::plus, X, Y>(x, y)) { 
    return applyOperator<Operator::plus, X, Y>(x, y); 
} 

template<class X, class Y> 
auto operator-(const X& x, const Y& y) -> decltype(applyOperator<Operator::minus, X, Y>(x, y)) { 
    return applyOperator<Operator::minus, X, Y>(x, y); 
}

template<typename X, typename Y, 
typename std::enable_if<is_matricial<X>::value && is_matricial<Y>::value, int>::type = 0> 
auto operator*(const X& x, const Y& y) {
    return MultiplyGlue<X, Y>(x, y);
}

template<typename X, typename Y, 
typename std::enable_if<is_matricial<X>::value ^ is_matricial<Y>::value, int>::type = 0>
auto operator*(const X& x, const Y& y) { //-> decltype(applyOperator<X, Y, Operator::multiplication>(x, y)) {
    return applyOperator<Operator::multiplication, X, Y>(x, y);
}



template<class X, class Y>
auto operator/(const X& x, const Y& y) {
    return applyOperator<Operator::division, X, Y>(x, y);
}


template<class X, class Y> 
auto pow(const X& x, const Y& y) { //-> decltype(applyOperator<X, Y, Operator::power>(x, y)) {
    return applyOperator<X, Y, Operator::power>(x, y);
}

template<class X> 
auto log(const X& x) {
    return M_Glue<Operator::logarithm, X>(x);
}

template<class X>
auto exp(const X& x) {
    return M_Glue<Operator::exponent, X>(x);
}
 
template<class T, class X>
auto cast(const X& x) {
    return CastGlue<T, X>(x);
} 
 
// matrix x should be bool T 
template<class T, class X>
auto cast(const X& x, const T& true_value, const T& false_value) {
    return BoolCastGlue<T, X>(x, true_value, false_value);
}
 
template<class X>
auto square(const X& x) {
    return M_Glue<Operator::square, X>(x);
} 
          
template<class X>
auto operator-(const X& x) {
    return M_Glue<Operator::minus, X>(x);
}

// Logical operators
// can actually add parameter to template
template<class X, class Y>
auto operator==(const X& x, const Y& y) {
    return applyOperator<Operator::equality, X, Y, bool>(x, y);
}

template<typename X, typename Y>
auto operator<(const X& x, const Y& y) {
    return applyOperator<Operator::less_than, X, Y, bool>(x, y);
}


template<class X> 
auto transposed(const X& x) {
    return TransposeGlue<X>(x);
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
typename std::enable_if<is_matricial<X>::value && !is_matricial<Y>::value, X&>::type
operator-=(X& x, const Y& y) {
    for (auto r = 0; r < x.row_count(); ++r) {
        for (auto c = 0; c < x.col_count(); ++c) {
            x(r, c) -= y;
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



template<class X, class Y>
typename std::enable_if<is_matricial<X>::value 
&& is_matricial<Y>::value 
&& X::solid && Y::solid, X&>::type
operator*=(X& x, const Y& y) {
    for (auto i = 0; i < x.element_count(); ++i) {
        x[i] *= y[i];
    }
    return x;
}

template<class X, class Y>
typename std::enable_if<is_matricial<X>::value 
&& is_matricial<Y>::value 
&& !(X::solid && Y::solid), X&>::type
operator*=(X& x, const Y& y) {
    for (auto r = 0; r < x.row_count(); ++r) {
        for (auto c = 0; c < x.col_count(); ++c) {
            x(r, c) *= y(r, c);
        }
    }
    return x;
}

template<typename M,
typename std::enable_if< is_matricial<M>::value && std::is_same<typename M::value_type, bool>::value, int >::type = 0>
bool any(const M& m) {
    for (Index r = 0; r < m.row_count(); ++r) {
        for (Index c = 0; c < m.col_count(); ++c) {
            if (m(r, c)) return true;
        }
    }
    return false;
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

template<typename M, typename P, 
typename std::enable_if< is_matricial<M>::value && is_matricial<P>::value, int >::type = 0>
typename M::value_type sum(const M& m, const P& p) {
    typename M::value_type s = 0;
    for (auto r = 0; r < m.row_count(); ++r) {
        for (auto c = 0; c < m.col_count(); ++c) {
            if (!p(r, c)) continue;
            s += m(r, c);
        }
    }
    return s;
}

template<typename M,
typename std::enable_if< is_matricial<M>::value 
                        && std::is_same<typename M::value_type, bool>::value, int >::type = 0> 
auto operator!(const M& m) {
    return M_Glue<Operator::opposite, M>(m);
}

//template<class M, class P> 
//typename std::enable_if<is_matricial<M>::value && !is_matricial<P>::value, bool> 
//operator==(const M& m, const P& p) {
//    for (auto r = 0; r < m.row_count(); ++r) {
//        for (auto c = 0; c < m.col_count(); ++c) {
//            if (m(r, c) != p) return false;
//        }
//    }
//    return true;
//}
//
//template<class M_0, class M_1>
//typename std::enable_if<is_matricial<M_0>::value && is_matricial<M_1>::value, bool> 
//operator==(const M_0& m_0, const M_1& m_1) {
//    for (auto r = 0; r < m_0.row_count(); ++r) {
//        for (auto c = 0; c < m_1.col_count(); ++c) {
//            if (m_0(r, c) != m_1(r, c)) return false;
//        }
//    }
//    return true;
//}




} 

}

#endif