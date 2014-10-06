//
//  glue.h
//  Ant
//
//  Created by Anton Logunov on 6/29/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef GLUE_LINALG_H
#define GLUE_LINALG_H

#include "../core.h"
#include "../grid.h"



namespace ant { 

namespace linalg {

    using ant::Int;
    using ant::Count;
    using grid::Size;
    using grid::Region;
    using grid::Position;
    
    struct Base {};
    
    enum struct Operator {
        plus,
        minus,
        multiplication,
        division,
        remainder
    };
    
    template<Operator op, class T>
    T applyOperator(const T& t_0, const T& t_1) {
        using Op = Operator;
        switch (op) {
            case Op::plus: return t_0 + t_1;
            case Op::minus: return t_0 - t_1;
            case Op::multiplication: return t_0 * t_1;
            case Op::division: return t_0 / t_1;
            //case Op::remainder: return t_0 % t_1;
                //default: static_assert(false, "wtf");
        }
    }
    
    
//    template<class T> struct Matrix;
//    template<class T> struct MatrixView;
//        
        
    template<class T>
    struct is_matricial {
        static constexpr bool value = std::is_base_of<Base, T>::value;
    };
    
    //
        
    template<class M_0, class M_1, Operator Op> 
    struct M_M_Glue : Base {
        using value_type = typename M_0::value_type;
        
        
        M_M_Glue(const M_0& m_0, const M_1& m_1) : m_0(m_0), m_1(m_1) {}
        
        value_type operator()(Int row, Int col) const {
            return applyOperator<Op>(m_0(row, col), m_1(row, col));
        }
        
        value_type operator[](Int i) const {
            return applyOperator<Op>(m_0[i], m_1[i]);
        }
        
        Size size() const {
            return m_0.size();
        }
        
        Count row_count() const {
            return m_0.row_count();
        }
        
        Count col_count() const {
            return m_0.col_count();
        }
        
        Count element_count() const {
            return m_0.element_count();
        }
        
        static constexpr bool solid = M_0::solid && M_1::solid;
    private:
        const M_0& m_0;
        const M_1& m_1;

    };
    
    template<class M, class P, Operator Op> 
    struct M_P_Glue : Base {
        using value_type = typename M::value_type;
    
        M_P_Glue(const M& m, const P& p) : m(m), p(static_cast<value_type>(p)) {}
        
        value_type operator()(Int row, Int col) const {
            return applyOperator<Op>(m(row, col), p);
        }
        
        value_type operator[](Int i) const {
            return applyOperator<Op>(m[i], p);
        }
        
        Size size() const {
            return m.size();
        }
        
        Count row_count() const {
            return m.row_count();
        }
        
        Count col_count() const {
            return m.col_count();
        }
        
        Count element_count() const {
            return m.element_count();
        }
        
        static constexpr bool solid = M::solid;
    private:
        const M& m;
        const value_type p;
    };

    template<class P, class M, Operator Op> 
    struct P_M_Glue : Base {
        using value_type = typename M::value_type;
    
        P_M_Glue(const P& p, const M& m) : m(m), p(static_cast<value_type>(p)) {}
        
        value_type operator()(Int row, Int col) const {
            return applyOperator<Op>(p, m(row, col));
        }
        
        value_type operator[](Int i) const {
            return applyOperator<Op>(p, m[i]);
        }
        
        Size size() const {
            return m.size();
        }
        
        Count row_count() const {
            return m.row_count();
        }
        
        Count col_count() const {
            return m.col_count();
        }
        
        Count element_count() const {
            return m.element_count();
        }
        
        static constexpr bool solid = M::solid;
    private:
        const M& m;
        const value_type p;
    };
    
    
} }

#endif