//
//  matrix.h
//  Ant
//
//  Created by Anton Logunov on 6/28/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef MATRIX_H
#define MATRIX_H


#include "glue.h"
#include "operations.h"
  
  
  
namespace ant { 

namespace linalg {
    
    template<class T> class Matrix;
    template<class T> class MatrixView;
    template<class T> class MutableMatrixView;

    
    template<class T>
    struct MatrixView : Base {
        using value_type = T;
        static constexpr bool solid = false;
        
        
        MatrixView(const Matrix<T>& m, Int r, Int c, Int r_count, Int c_count)
            : matrix_(m), region_(r, c, r_count, c_count) {
        }
        MatrixView(const Matrix<T>& m, const Region& region)
            : matrix_(m), region_(region) {
        }
        
        const T& operator()(Int row, Int col) const {
            auto& m = mat();
            return m(region_.row_begin() + row, region_.col_begin() + col);
        }
        
        Count row_count() const {
            return region_.row_count();
        }
        Count col_count() const {
            return region_.col_count();
        }
        Count element_count() const {
            return region_.cell_count();
        }
        
        const Matrix<T>& mat() const {
            return matrix_;
        }
        const Region& region() const {
            return region_;
        }
                                       
                                                     
    private:
        const Region region_;
        const Matrix<T>& matrix_;
    };
    
    
    template<class B>
    struct RowIndexMatrixView : Base {
        using value_type = typename B::value_type;
        static constexpr bool solid = false;
        
        // should probably take first and last iterators 
        template<class Iterator>
        RowIndexMatrixView(const B& base, Iterator first, Iterator last) : base_(base) {
            for (Iterator it = first; it != last; ++it) {
                row_indices_.push_back(*it);
            } 
        }
        
        RowIndexMatrixView(const B& base, std::vector<Index> row_indices)
         : base_(base), row_indices_(std::move(row_indices)) {} 
        
        const value_type& operator()(Int row, Int col) const {
            return base_(row_indices_[row], col);
        }
        
        Count row_count() const {
            return base_.col_count();
        }
        Count col_count() const {
            return base_.row_count();
        } 
        Count element_count() const {
            return row_indices_.size()*col_count();
        }
        
    private:
        const B& base_;
        std::vector<Index> row_indices_;
    };
    
    template<class T, class B>
    struct TransposedMatrixView : Base {
        using value_type = T;
        static constexpr bool solid = false;
        
        TransposedMatrixView(const B& base) : base_(base) {}
        
        const T& operator()(Int row, Int col) const {
            return base_(col, row);
        }
        
        Count row_count() const {
            return base_.col_count();
        }
        Count col_count() const {
            return base_.row_count();
        } 
        Count element_count() const {
            return base_.element_count();
        }
        
    private: 
        const B& base_;
    };
    
    
    template<class T>
    struct MutableMatrixView : MatrixView<T> {
        using MatrixView<T>::region;
        
        MutableMatrixView(Matrix<T>& m, Int r, Int c, Int r_count, Int c_count)
            : MatrixView<T>(m, r, c, r_count, c_count) {
        }
        MutableMatrixView(Matrix<T>& m, const Region& region)
            : MatrixView<T>(m, region) {
        }
        
        T& operator()(Int row, Int col) const {
            auto& m = mat();
            auto& reg = region(); 
            return m(reg.row_begin() + row, reg.col_begin() + col);
        }
        
        Matrix<T>& mat() const {
            return const_cast<Matrix<T>&>(MatrixView<T>::mat());
        }
        
        void fill(const T& t) {
            auto& r = MatrixView<T>::region();
            auto& m = mat();
            Int i_stride = static_cast<Int>(m.col_count());
            Int i_start = r.row_begin()*i_stride + r.col_begin();
            Int i_finish = r.row_count()*i_stride;
            for (Int i = i_start; i < i_finish; i += i_stride) {
                m.data_[i] = t;
            }
        }
        
        MutableMatrixView<T>& operator/=(const T& g) {
            auto& r = MatrixView<T>::region();
            auto& m = mat();
            Int i_stride = static_cast<Int>(m.col_count());
            Int i_start = r.row_begin()*i_stride + r.col_begin();
            Int i_finish = r.row_count()*i_stride;
            for (Int i = i_start; i < i_finish; i += i_stride) {
                m.data_[i] /= g;
            }
            return *this;
        }
        
        MutableMatrixView<T>& operator-=(const T& g) {
            auto& r = MatrixView<T>::region();
            auto& m = mat();
            Int i_stride = static_cast<Int>(m.col_count());
            Int i_start = r.row_begin()*i_stride + r.col_begin();
            Int i_finish = r.row_count()*i_stride;
            for (Int i = i_start; i < i_finish; i += i_stride) {
                m.data_[i] -= g;
            }
            return *this;
        }
        
        MutableMatrixView<T>& operator+=(const T& g) {
            auto& r = MatrixView<T>::region();
            auto& m = mat();
            Int i_stride = static_cast<Int>(m.col_count());
            Int i_start = r.row_begin()*i_stride + r.col_begin();
            Int i_finish = r.row_count()*i_stride;
            for (Int i = i_start; i < i_finish; i += i_stride) {
                m.data_[i] += g;
            }
            return *this;
        }
        
        template<class Y>
        typename std::enable_if<is_matricial<Y>::value, MutableMatrixView<T>&>::type
        operator+=(const Y& g) {
            auto& m = mat();
            auto& reg = region();  
            auto r_b = reg.row_begin();
            auto c_b = reg.col_begin();
            for (auto r = 0; r < reg.row_count(); ++r) {
                for (auto c = 0; c < reg.col_count(); ++c) {
                    m(r_b + r, c_b + c) += g(r, c);
                }
            }
            return *this;

        }
        
        // can be matricial can be not, can be solid or not
        template<class Y>
        typename std::enable_if<is_matricial<Y>::value, MutableMatrixView<T>&>::type
        operator=(const Y& g) {
            auto& m = mat();
            auto& reg = region();  
            auto r_b = reg.row_begin();
            auto c_b = reg.col_begin();
            for (auto r = 0; r < reg.row_count(); ++r) {
                for (auto c = 0; c < reg.col_count(); ++c) {
                    m(r_b + r, c_b + c) = g(r, c);
                }
            }
            return *this;
        }
        
        template<class Y>
        typename std::enable_if<!is_matricial<Y>::value, MutableMatrixView<T>&>::type
        operator=(const Y& g) {
            auto& m = mat();
            auto& reg = region();  
            auto r_b = reg.row_begin();
            auto c_b = reg.col_begin();
            for (auto r = 0; r < reg.row_count(); ++r) {
                std::fill_n(&m(r_b + r, c_b), reg.col_count(), g);
            }
            return *this;
        }
    };
   
   
   
    template<class T>
    struct Matrix : Base {
        using value_type = T;
        
        static constexpr bool solid = true;
        
        Matrix() : Matrix(0, 0) {} 
        Matrix(Size size) : Matrix((Int)size.row, (Int)size.col) {}
        Matrix(Int row_count, Int col_count) {
            data_ = new T[row_count*col_count];
            row_count_ = row_count;
            col_count_ = col_count;
        }
        Matrix(const Matrix<T>& m)
            :   row_count_(m.row_count()), 
                col_count_(m.col_count()),
                data_(new T[m.element_count()]) {
            std::copy_n(m.data_, m.element_count(), data_);
        }
        
        // something Glue ???
        Matrix(const Glue<T>& g) : Matrix(g.size()) {
            Int i = 0;
            for (Int r = 0; r < row_count(); ++r) {
                for (Int c = 0; c < col_count(); ++c) {
                    data_[i++] = g(r, c); 
                }
            }
        }
        
        
        void random() {
            std::default_random_engine rng;
            std::uniform_real_distribution<> distr;
            for (auto i = 0; i < element_count(); ++i) {
                data_[i] = distr(rng);
            }
        }
        
        T& operator()(Int row, Int col) {
            return data_[index(row, col)];
        }
        const T& operator()(Int row, Int col) const {
            return data_[index(row, col)];
        }
        
        T& operator[](const Position& p) {
            return data_[index(p.row, p.col)];
        }
        const T& operator[](const Position& p) const {
            return data_[index(p.row, p.col)];
        }
        
        MutableMatrixView<T> operator[](const Region& r) {
            return MutableMatrixView<T>(*this, r);
        }
        MatrixView<T> operator[](const Region& r) const {
            return MatrixView<T>(*this, r);
        }
        
        T& operator[](Int i) {
            return data_[i];
        }
        const T& operator[](Int i) const {
            return data_[i];
        }
        
        MatrixView<T> row(Int row) const {
            return MatrixView<T>(*this, row, 0, 1, static_cast<Int>(col_count_));
        } 
        MutableMatrixView<T> row(Int row) {
            return MutableMatrixView<T>(*this, row, 0, 1, static_cast<Int>(col_count_));
        }
        
        MatrixView<T> rows(Index row, Count count) const {
            return submat(row, 0, count, col_count());
        }
        
        MutableMatrixView<T> rows(Index row, Count count) {
            return submat(row, 0, count, col_count());
        }
        
        MatrixView<T> cols(Index col, Count count) const {
            return submat(0, col, row_count(), count);
        }
        
        MutableMatrixView<T> cols(Index col, Count count) {
            return submat(0, col, row_count(), count);
        }
        
        MatrixView<T> submat(Int row, Int col, Int row_count, Int col_count) const {
            return MatrixView<T>(*this, row, col, row_count, col_count);
        }
        MutableMatrixView<T> submat(Int row, Int col, Int row_count, Int col_count) {
            return MutableMatrixView<T>(*this, row, col, row_count, col_count);
        }
        
        MatrixView<T> submat(const Position& p, const Size& s) const {
            return MatrixView<T>(*this, p.row, p.col, s.row, s.col);
        }
        MutableMatrixView<T> submat(const Position& p, const Size& s) {
            return MutableMatrixView<T>(*this, p.row, p.col, s.row, s.col);
        }
        
        Count row_count() const {
            return row_count_;
        }
        Count col_count() const {
            return col_count_;
        }
        
        Count element_count() const {
            return row_count_*col_count_;
        }
        
        void set_size(Count row_count, Count col_count) {
            set_size(Size(row_count, col_count));
        }
        
        
        void set_size(const Size& new_size) {
            delete [] data_;
            row_count_ = new_size.row;
            col_count_ = new_size.col;
            data_ = new value_type[new_size.cell_count()];
        }
        
        Size size() const {
            return Size(row_count(), col_count());
        }
         
        void fill(const T& t) {
            std::fill_n(data_, row_count()*col_count(), t);
        }
        
        TransposedMatrixView<T, Matrix<T>> transposed() const {
            return TransposedMatrixView<T, Matrix<T>>(*this);
        }
        
        // should return row or column actually
        Matrix<Index> each_row_argmax() const {
            Matrix<Index> result(row_count(), 1);
            for (auto r = 0; r < row_count(); ++r) {
                auto c_max = 0;
                for (auto c = 1; c < col_count(); ++c) {
                    if ((*this)(r, c) > (*this)(r, c_max)) {
                        c_max = c;
                    }
                }
                result(r, 0) = c_max;
            }
            return result;
        }
        
        
        // should return row or column actually
        Matrix<Index> each_col_argmax() const {
            Matrix<Index> result(1, col_count());
            for (auto c = 0; c < col_count(); ++c) {
                auto r_max = 0;
                for (auto r = 1; r < row_count(); ++r) {
                    if ((*this)(r, c) > (*this)(r_max, c)) {
                        r_max = r;
                    }
                }
                result(0, c) = r_max;
            }
            return result;
        }
        
        
        // void resize(Int row_count, Int col_count) {}
        
        Matrix<T>& operator=(Matrix<T>&& m) {
            delete [] data_;
            row_count_ = m.row_count();
            col_count_ = m.col_count();
            data_ = m.data_;
            m.data_ = nullptr;
            m.row_count_ = 0;
            m.col_count_ = 0;
            return *this;
        }
        
        template<class Y> 
        typename std::enable_if<is_matricial<Y>::value && Y::solid, Matrix<T>&>::type
        operator=(const Y& g) {
            for (auto i = 0; i < element_count(); ++i) {
                data_[i] = g[i];
            }
            return *this;
        }
        
        template<typename Y, typename std::enable_if<is_matricial<Y>::value && !Y::solid, int>::type = 0>
        Matrix<T>& operator=(const Y& g) {
            for (auto r = 0; r < row_count(); ++r) {
                for (auto c = 0; c < col_count(); ++c) {
                    data_[index(r, c)] = g(r, c);
                }
            }
            return *this;
        }
        
        template<class Y> 
        typename std::enable_if<!is_matricial<Y>::value, Matrix<T>&>::type
        operator=(const Y& g) {
            fill(g);
            return *this;
        }
        
        // lol
        Matrix<T>& operator=(const Matrix<T>& m) {
            for (Int r = 0; r < row_count(); ++r) {
                for (Int c = 0; c < col_count(); ++c) {
                    (*this)(r, c) = m(r, c);
                }
            }
            return *this;
        }
        
        Matrix<T>& operator*=(const T& t) {
            for (int i = 0; i < element_count(); ++i) {
                data_[i] *= t;
            }
            return *this;
        }
        

        ~Matrix() {
            delete [] data_;
        }
        
    private:
        Int index(Int row, Int col) const {
            return row*static_cast<Int>(col_count_) + col;
        }
        
        Count row_count_, col_count_;
        T* data_;
        Count capacity_;

        friend struct MutableMatrixView<T>;
    };

    template<typename T>
    struct Row : Matrix<T> {
    
    };
    
    template<typename T>
    struct Col : Matrix<T> {
    
    };
    
    
} // end linalg

} // end ant

#endif