//
//  glue.h
//  Ant
//
//  Created by Anton Logunov on 6/29/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef GLUE_LINALG_H
#define GLUE_LINALG_H

#include <stdexcept>

#include "ant/core/core.hpp"
#include "ant/grid.hpp"



namespace ant { 

namespace linalg {

using ant::Int;
using ant::Count;
using grid::Size;
using grid::Region;
using grid::Position;

// need solid also
// static constexpr bool solid
struct Base {
    Size size() const {
        return Size(row_count(), col_count());
    }
    virtual Count row_count() const = 0;
    virtual Count col_count() const = 0;
    
    Count element_count() const {
        return row_count() * col_count();
    }
};


template<class T>
struct Glue : Base {
    using value_type = T;
    
    // can't return reference because values may be temprorary
    virtual value_type operator()(Int row, Int col) const = 0;
};
    
//struct SolidGlue
    
    
template<class B>
struct is_matricial {
    static constexpr bool value = std::is_base_of<Base, B>::value;
};

template<class B_0, class B_1> 
struct max_size {
    using type = typename std::conditional<(sizeof(B_0) > sizeof(B_1)), B_0, B_1>::type;
    static constexpr size_t value = sizeof(type);
};



enum struct Operator {
    plus,
    minus,
    multiplication,
    division,
    remainder,
    power,
    equality,
    logarithm,
    exponent,
    square_root,
    square,
    opposite,
    less_than
};


// should to through enable if probably somehow
template<Operator op, class R, class T_0, class T_1>
R applyBinaryOperator(const T_0& t_0, const T_1& t_1) {
    using Op = Operator;
    switch (op) {
        case Op::plus: return t_0 + t_1;
        case Op::minus: return t_0 - t_1;
        case Op::multiplication: return t_0 * t_1;
        case Op::division: return t_0 / t_1;
        case Op::power: return std::pow(t_0, t_1);
        case Op::equality: return t_0 == t_1;
        case Op::less_than: return t_0 < t_1;
        default: throw std::runtime_error("gg wp"); 
        //case Op::remainder: return t_0 % t_1;
            //default: static_assert(false, "wtf");
    }
}


template<Operator op, class T>
T applyUnaryOperator(const T& t) {
    using Op = Operator;
    switch (op) {
//        case Op::logarithm: return log(t);
//        case Op::exponent: return exp(t);
        case Op::minus: return -t;
//        case Op::square_root: return sqrt(t);
        case Op::square: return t*t;
        case Op::opposite: return !t;
        default: throw std::runtime_error("unsupported operator");
    }
} 



// M - matrix, matrix_view
// P - value_type

// return type is biggest one of input
template<Operator Op, class M_0, class M_1, class T = typename max_size<typename M_0::value_type, typename M_1::value_type>::type> 
struct M_M_Glue : Glue<T> {
    using value_type = T;
    
    M_M_Glue(const M_0& m_0, const M_1& m_1) : m_0_(m_0), m_1_(m_1) {}
    
    value_type operator()(Int row, Int col) const override {
        return applyBinaryOperator<Op, value_type>(m_0_(row, col), m_1_(row, col));
    }
    
    value_type operator[](Int i) const {
        return applyBinaryOperator<Op, value_type>(m_0_[i], m_1_[i]);
    }
    
    Count row_count() const override {
        return m_0_.row_count();
    }
    Count col_count() const override {
        return m_0_.col_count();
    }

    static constexpr bool solid = M_0::solid && M_1::solid;
private:
    const M_0& m_0_;
    const M_1& m_1_;
};

template<Operator Op, typename M, class P, typename T = typename M::value_type> 
struct M_P_Glue : Glue<T> {
    using value_type = T;
    
    M_P_Glue(const M& m, const P& p) : m_(m), p_(p) {}
    
    value_type operator()(Int row, Int col) const override {
        return applyBinaryOperator<Op, value_type>(m_(row, col), p_);
    }
    value_type operator[](Int i) const {
        return applyBinaryOperator<Op, value_type>(m_[i], p_);
    }
    
    Count row_count() const override {
        return m_.row_count();
    }
    Count col_count() const override {
        return m_.col_count();
    }
    
    static constexpr bool solid = M::solid;
private:
    const M& m_;
    const P p_;
};

template<Operator Op, class P, class M, class T = typename M::value_type> 
struct P_M_Glue : Glue<T> {
    using value_type = T;
    
    P_M_Glue(const P& p, const M& m) : m_(m), p_(p) {}
    
    value_type operator()(Int row, Int col) const override {
        return applyBinaryOperator<Op, value_type>(p_, m_(row, col));
    }
    value_type operator[](Int i) const {
        return applyBinaryOperator<Op, value_type>(p_, m_[i]);
    }
    
    Count row_count() const override {
        return m_.row_count();
    }
    Count col_count() const override {
        return m_.col_count();
    }
    
    static constexpr bool solid = M::solid;
private:
    const M& m_;
    const P p_;
};


// some unary operators
template<Operator Op, class M, class T = typename M::value_type> 
struct M_Glue : Glue<T> {
    using value_type = T;

    M_Glue(const M& m) : m_(m) {}
    
    value_type operator()(Int row, Int col) const override {
        return applyUnaryOperator<Op>(m_(row, col));
    }
    value_type operator[](Int i) const {
        return applyUnaryOperator<Op>(m_[i]);
    }
    
    Count row_count() const override {
        return m_.row_count();
    }
    Count col_count() const override {
        return m_.col_count();
    }
    
    static constexpr bool solid = M::solid;
private:
    const M& m_;
};


template<class M, class T = typename M::value_type>
struct TransposeGlue : Glue<T> {
    using value_type = T;
    
    TransposeGlue(const M& m) : m_(m) {}
    
    value_type operator()(Int row, Int col) const override {
        return m_(col, row);
    }
    
    Count row_count() const override {
        return m_.col_count();
    }
    
    Count col_count() const override {
        return m_.row_count();
    }
      
    static constexpr bool solid = false;
private:
    const M& m_;
};


template<class M_0, class M_1, class T = typename max_size<typename M_0::value_type, typename M_1::value_type>::type>
struct MultiplyGlue : Glue<T> {
    using value_type = T;
    
    MultiplyGlue(const M_0& m_0, const M_1& m_1) : m_0_(m_0), m_1_(m_1) {}
    
    value_type operator()(Int row, Int col) const override {
        value_type v = 0;
        for (auto i = 0; i < m_0_.col_count(); ++i) {
            v += m_0_(row, i) * m_1_(i, col);
        }
        return v;
    }
        
    Count row_count() const override {
        return m_0_.row_count();
    }
    Count col_count() const override {
        return m_1_.col_count();
    }

    static constexpr bool solid = false;
private:
    const M_0& m_0_;
    const M_1& m_1_;
};


template<class T, class M> 
struct CastGlue : Glue<T> {
    using value_type = T;
    
    CastGlue(const M& m) : m_(m) {}

    value_type operator()(Int row, Int col) const override {
        return static_cast<value_type>(m_(row, col));
    }
    value_type operator[](Int i) const {
        return static_cast<value_type>(m_[i]);
    }
    
    Count row_count() const override {
        return m_.row_count();
    }
    Count col_count() const override {
        return m_.col_count();
    }

    static constexpr bool solid = M::solid;
private:
    const M& m_;
};


template<typename T, typename M, 
typename std::enable_if<is_matricial<M>::value && std::is_same<typename M::value_type, bool>::value, int>::type = 0> 
struct BoolCastGlue : Glue<T> {
    using value_type = T;
    
    BoolCastGlue(const M& m, const T& true_value, const T& false_value)
     : m_(m), true_value_(true_value), false_value_(false_value) {}
    
    value_type operator()(Int row, Int col) const override {
        return m_(row, col) ? true_value_ : false_value_;
    }
    value_type operator[](Int i) const {
        return m_[i] ? true_value_ : false_value_;
    }

    Count row_count() const override {
    return m_.row_count();
    }
    Count col_count() const override {
    return m_.col_count();
    }

    static constexpr bool solid = M::solid;
private:
    const M& m_;
    const T& true_value_;
    const T& false_value_;
};



} // end namespace linalg 

} // end namespace ant

#endif