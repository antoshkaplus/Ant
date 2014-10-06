//
//  math.h
//  Ant
//
//  Created by Anton Logunov on 4/23/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//
// should be deprecated



#ifndef __Ant__math__
#define __Ant__math__

#include <iostream>
#include <valarray>

#include "./grid.h"


namespace ant { namespace algebra {
    
    template<class T> class Mat;
    template<class T> class MatView;
    template<class T> class MutableMatView;
    
    template<class T>
    Mat<T> ones(Count row_count, Count col_count);
    template<class T>
    T sum(const Mat<T>& mat);
    template<class T>
    T sum(const MatView<T>& mat_view);
    
    template<class T>
    Mat<T>& operator+=(Mat<T>& mat, Mat<T>& mat_other);
    template<class T>
    Mat<T>& operator+=(Mat<T>& mat, double g);
    template <class T>
    const MutableMatView<T>& operator-=(const MutableMatView<T>& view, const Mat<T>& mat);
    template <class T> 
    const MutableMatView<T>& operator-=(const MutableMatView<T>& v_0, const MatView<T>& v_1);
    template<class T>
    Mat<T>& operator/=(Mat<T>& mat, double g);
    template<class T>
    const MutableMatView<T>& operator/=(const MutableMatView<T>& view, double g);
    
    template<class T>
    Mat<T> operator==(const Mat<T>& mat, const T& g);
    
    template<class T>
    Mat<T> operator+(const Mat<T>& mat, double g);
    template<class T>
    Mat<T> operator-(double g, const Mat<T>& mat);
    template<class T>
    Mat<T> operator-(const Mat<T>& m_0, const Mat<T>& m_1);
    template<class T>
    Mat<T> operator-(const Mat<T>& m, const MatView<T>& m_v);
    
    template<class T>
    Mat<T> operator*(double g, const Mat<T>& mat);
    template<class T>
    Mat<T> operator/(const Mat<T>& mat, double g);
    template<class T>
    Mat<T> operator/(double g, const Mat<T>& mat);
    
    template<class T>
    Mat<T> elem_mult(const Mat<T>& m_0, const Mat<T>& m_1);
    template<class T>
    Mat<T> elem_pow(const Mat<T>& mat, double p);
    template<class T>
    Mat<T> elem_pow(double numb, const Mat<T>& p);
    
    template<class T>
    Mat<T> elem_log(const Mat<T>& mat);
    
    template<class T>
    std::ostream& operator<<(std::ostream& output, const MutableMatView<T>& view);
} }


// forward declaration to get friendship
template<class T>
ant::algebra::Mat<T>& operator*=(ant::algebra::Mat<T>& m, double g);



namespace ant { namespace algebra {
    using grid::Size;
    using grid::Region;
    using grid::Position;
    
    template<typename T>
    class MatView {
    public:
        MatView(const Mat<T>& mat, Index row, Index col, Count row_count, Count col_count) 
            : mat_(mat), region_((Int)row, (Int)col, (Int)row_count, (Int)col_count) {}
        MatView(const Mat<T>& mat, const Region& region)
            : mat_(mat), region_(region) {}
        
        const T& operator()(Index row, Index col) const {
            return mat()(row + region_.position.row, col + region_.position.col);
        }
        
        const Mat<T>& mat() const {
            return mat_;
        }
        
        T max() const {
            Index 
            r_b = region_.row_begin(),
            r_e = region_.row_end(),
            c_b = region_.col_begin(),
            c_e = region_.col_end();
        
            T m = mat_(r_b, c_b);
            for (auto row = r_b; row < r_e; ++row) {
                for (auto col = c_b; col < c_e; ++col) {
                    if (m < mat_(row, col)) m = mat_(row, col);
                }
            }
            return m;
        }
        
        T min() const {
            Index 
            r_b = region_.row_begin(),
            r_e = region_.row_end(),
            c_b = region_.col_begin(),
            c_e = region_.col_end();
            
            T m = mat_(r_b, c_b);
            for (auto row = r_b; row < r_e; ++row) {
                for (auto col = c_b; col < c_e; ++col) {
                    if (m > mat_(row, col)) m = mat_(row, col);
                }
            }
            return m;
        }
        
        Mat<T> transposed();
    private:
        
        std::gslice gslice() const {
            uint 
            m_c_sz = mat_.col_count(),
            v_r_sz = region_.row_count(),
            v_c_sz = region_.col_count(),
            m_r = region_.row_begin(),
            m_c = region_.col_begin();
            return std::gslice(m_r*m_c_sz + m_c, {v_r_sz, v_c_sz}, {m_c_sz, 1});
        }
    
        const Region region_;
        const Mat<T> &mat_;
                
        friend class Mat<T>;
        friend class MutableMatView<T>;
        friend T      sum <>(const MatView<T>& mat_view);
        friend Mat<T> operator- <>(const Mat<T>& m, const MatView<T>& m_v);
        friend const MutableMatView<T>& operator-= <>(const MutableMatView<T>& v_0, const MatView<T>& v_1);
    }; 
    
    template<class T>
    class MutableMatView : public MatView<T> {
    public:
        MutableMatView(Mat<T>& mat, Index row, Index col, Count row_count, Count col_count) 
            : MatView<T>(mat, row, col, row_count, col_count) {}
        MutableMatView(Mat<T>& mat, const Region& region)
            : MatView<T>(mat, region) {}
        
        Mat<T>& mat() const {
            return const_cast<Mat<T>&>(MatView<T>::mat_);
        }
        
        T& operator()(Index row, Index col) {
            return mat()(row + MatView<T>::region_.position.row, col + MatView<T>::region_.position.col);
        }
        
        const MutableMatView<T>& operator=(const Mat<T>& m) const {
            const Region &rect = MatView<T>::region_;
            Mat<T> &v_m = mat();
            for (auto r_v = rect.row_begin(), r_m = 0; r_m < m.row_count(); ++r_v, ++r_m) {
                for (auto c_v = rect.col_begin(), c_m = 0; c_m < m.col_count(); ++c_v, ++c_m) {
                    v_m(r_v, c_v) = m(r_m, c_m);
                }
            }
            return *this;
        }
        const MutableMatView<T>& operator=(const T& g) const {
            const Region &rect = MatView<T>::region_;
            Mat<T> &m = mat();
            for (auto r = rect.row_begin(); r < rect.row_end(); ++r) {
                for (auto c = rect.col_begin(); c < rect.col_end(); ++c) {
                    m(r, c) = g;
                }
            }
            return *this;
        }
        
        void fill(const T& v) {
            const Region &rect = MatView<T>::region_;
            Mat<T> &m = mat();
            for (auto r_v = rect.row_begin(); r_v < rect.row_end(); ++r_v) {
                for (auto c_v = rect.col_begin(); c_v < rect.col_end(); ++c_v) {
                    m(r_v, c_v) = v;
                }
            }
        }
        
        friend const MutableMatView<T>& operator-=(const MutableMatView<T>& view, const Mat<T>& mat);
        friend std::ostream& operator<< <>(std::ostream& output, const MutableMatView<T>& view);
    };
    
    template<typename T>
    class Mat {
    public:
        Mat() : Mat(0, 0) {}
        Mat(const Size& size) : Mat(size.row, size.col) {}
        Mat(Count row_count, Count col_count) 
        : row_count_(row_count), col_count_(col_count) {
            resize(row_count, col_count);
        }
        Mat(const std::initializer_list<std::valarray<double>>& list) {
            row_count_ = list.size();
            col_count_ = list.begin()->size();
            resize(row_count_, col_count_);
            auto i = 0;
            for (auto &v : list) {
                elements_[std::slice(i, col_count_, 1)] = v;
                i += col_count_;
            }
            
        }
        Mat(const MatView<T>& v) {
            *this = v;
        }
        
        
        Int col_count() const { return (Int)col_count_; }
        Int row_count() const { return (Int)row_count_; }
        Size size() const {
            return Size(row_count_, col_count_);
        }
        
        void resize(Count row_count, Count col_count) {
            row_count_ = row_count;
            col_count_ = col_count;
            elements_.resize(row_count*col_count);
        }
        
        T& operator()(Index row, Index col) {
            return elements_[index(row, col)];
        }
        const T& operator()(Index row, Index col) const {
            return elements_[index(row, col)];
        }
        
        T& operator()(const Position& p) {
            return elements_[index(p)];
        }
        const T& operator()(const Position& p) const {
            return elements_[index(p)];
        }
        
        MatView<T> operator()(const Region& region) const {
            return MatView<T>(*this, region);
        }
        MutableMatView<T> operator()(const Region& region) {
            return MutableMatView<T>(*this, region);
        }
        
        T& at(Index row, Index col) {
            return elements_[index(row, col)];
        }
        const T& at(Index row, Index col) const {
            return elements_[index(row, col)];
        }
        
        Mat<T>& operator=(const MatView<T>& v) {
            auto &rect = v.region_;
            auto 
            r_begin = rect.row_begin(),
            r_end   = rect.row_end(),
            c_begin = rect.col_begin(),
            c_end   = rect.col_end();
            resize(r_end-r_begin, c_end-c_begin);
            auto &m = v.mat_; 
            for (auto r = 0, r_v = r_begin; r_v < r_end; ++r_v, ++r) {
                for (auto c = 0, c_v = c_begin; c_v < c_end; ++c_v, ++c) {
                    (*this)(r, c) = m(r_v, c_v);
                }
            }
            return *this;
        }
        
        MutableMatView<T> row(Index r) {
            return MutableMatView<T>(*this, r, 0, 1, col_count_);
        }
        
        MatView<T> row(Index r) const {
            return MatView<T>(*this, r, 0, 1, col_count_);
        }
        
        MatView<T> col(Index c) const {
            return MatView<T>(*this, 0, c, row_count_, 1);
        }
        
        MatView<T> submat(Index r, Index c, Count r_count, Count c_count) const {
            return MatView<T>(*this, r, c, r_count, c_count);
        }
        MutableMatView<T> submat(Index r, Index c, Count r_count, Count c_count) {
            return MutableMatView<T>(*this, r, c, r_count, c_count);
        }
        
        // withCols()
        // should be sorted
        Mat<T> withoutRows(const std::vector<Index>& indices) const {
            Mat<T> m(row_count_ - indices.size(), col_count_);
            for (auto r = 0, i = 0; r < row_count_; ++r) {
                if (i < indices.size() && r == indices[i]) {
                    ++i; continue;
                }
                m.row(r-i) = row(r);
            }
            return m; 
        }
        
        Mat<T> withoutCols(const std::vector<Index>& indices) const {
            Mat<T> m(row_count_, col_count_ - indices.size());
            // can't just assign col, because of memory ordering
            // but here is more if statements
            for (auto r = 0; r < row_count_; ++r) {
                for (auto c = 0, i = 0; c < col_count_; ++c) {
                    if (i < indices.size() && c == indices[i]) {
                        ++i; continue;
                    }
                    m(r, c-i) = (*this)(r, c);
                }
            }
            return m; 
        }
        
        Mat<T> rows(const std::vector<Index>& indices) {
            return Mat<T>();
        }
        
        // lol
        Mat<T>& eye() {
            elements_ = 0;
            for (auto i = 0; i < std::min(row_count_, col_count_); ++i) {
                elements_[index(i, i)];
            }
        }
        
        void fill(const T& val) {
            elements_ = val;
        }
        
        Mat<T> transposed() const {
            Mat<T> mat(col_count(), row_count());
            for (auto row = 0; row < row_count(); ++row) {
                for (auto col = 0; col < col_count(); ++col) {
                    mat(col, row) = at(row, col);
                }
            }
            return mat;
        }
        
        Mat<T> max_each_row() const {
            Mat<T> r(row_count(), 1);
            for (auto i = 0; i < row_count(); ++i) {
                r(i, 0) = submat(i, 0, 1, col_count()).max();
            }
            return r;
        }
        
        Mat<T> max_each_col() const {
            Mat<T> r(1, col_count());
            for (auto i = 0; i < col_count(); ++i) {
                r(0, i) = submat(0, i, row_count(), 1).max();
            }
            return r;
        }
        
        Mat<Index> argmax_each_row() const {
            Mat<Index> r(row_count(), 1);
            for (auto i = 0; i < row_count(); ++i) {
                Index k_max = 0;
                for (auto k = 1; k < col_count(); ++k) {
                    if (at(i, k_max) < at(i, k)) k_max = k;
                }
                r(i, 0) = k_max;
            }
            return r;
        }
        
        Mat<Index> argmax_each_col() const {
            Mat<Index> r(1, col_count());
            for (auto i = 0; i < col_count(); ++i) {
                Index k_max = 0;
                for (auto k = 1; k < row_count(); ++k) {
                    if (at(k_max, i) < at(k, i)) k_max = k;
                }
                r(0, i) = k_max;
            }
            return r;
        }
        
        
    private: 
        Index index(Index row, Index col) const {
            return row*col_count_ + col;
        }
        Index index(const Position& p) const {
            return p.row*col_count_ + p.col;
        }
        
    
        std::valarray<T> elements_;
        Count row_count_;
        Count col_count_;
    
    friend class MatView<T>;
    friend const MutableMatView<T>& operator/= <>(const MutableMatView<T>& view, double g);
    friend Mat<T>& operator/= <>(Mat<T>& mat, double g);  
    friend Mat<T>& operator*= <>(Mat<T>& mat, double g); 
    friend Mat<T>& operator+= <>(Mat<T>& mat, Mat<T>& mat_other);
    friend Mat<T>& operator+= <>(Mat<T>& mat, double g);
    friend Mat<T> operator== <>(const Mat<T>& mat, const T& g);
    
    friend Mat<T> operator+ <>(const Mat<T>& mat, double g);
    friend Mat<T> operator- <>(double g, const Mat<T>& mat);
    friend Mat<T> operator- <>(const Mat<T>& m_0, const Mat<T>& m_1);
    friend Mat<T> operator- <>(const Mat<T>& m, const MatView<T>& m_v);
    friend Mat<T> operator* <>(double g, const Mat<T>& mat);
    friend Mat<T> operator/ <>(const Mat<T>& mat, double g);
    friend Mat<T> operator/ <>(double g, const Mat<T>& mat);
    
    friend Mat<T> elem_mult <> (const Mat<T>& m_0, const Mat<T>& m_1);
    friend Mat<T> elem_pow  <> (const Mat<T>& mat, double p);
    friend Mat<T> elem_pow  <> (double numb, const Mat<T>& p);
    friend Mat<T> elem_log  <> (const Mat<T>& mat);
    
    friend Mat<T> ones  <>(Count, Count);
    friend T      sum   <>(const Mat<T>& mat);
    friend T      sum   <>(const MatView<T>& mat_view);
    };
    
    template<class T>
    Mat<T> MatView<T>::transposed() {
        return Mat<T>();
    }
    
    template<class T>
    Mat<T> eye(Count row_count, Count col_count) {
        Mat<T> m(row_count, col_count);
        m.eye();
        return m;
    }
    
    template<class T>
    Mat<T> eye(Count size) {
        return eye<T>(size, size);
    }
    
    template<class T>
    Mat<T> join_rows(const Mat<T>& m_0, const Mat<T>& m_1) {
        Mat<T> m(m_0.row_count(), m_0.col_count() + m_1.col_count());
        m.submat(0, 0, m_0.row_count(), m_0.col_count()) = m_0;
        m.submat(0, m_0.col_count(), m_1.row_count(), m_1.col_count()) = m_1;
        return m;
    }
    
    template<class T>
    Mat<T> ones(Count row_count, Count col_count) {
        Mat<T> m(row_count, col_count);
        m.elements = 0;
    }
    
    template<class T>
    Mat<T> ones(Count size) {
        return ones<T>(size, size);
    }
    
    template<class T>
    T sum(const Mat<T>& mat) {
        return mat.elements_.sum();
    }
    template<class T>
    T sum(const MatView<T>& mat_view) {
        auto &rect = mat_view.region_; 
        const Mat<T>& mat = mat_view.mat();
        auto g = std::gslice(rect.row_begin()*mat.col_count() + rect.col_begin(), 
                             {(unsigned)rect.row_count(), (unsigned)rect.col_count()},
                             {(unsigned)mat.col_count(), 1});
        auto res = mat.elements_[g].sum();
        return res;
    }
    
    template<class T>
    Mat<T> elem_mult(const Mat<T>& m_0, const Mat<T>& m_1) {
        Mat<T> r(m_0.size());
        r.elements_ = m_0.elements_ * m_1.elements_;
        return r;
    }
    
    template<class T>
    Mat<T> elem_mult(const Mat<T>& m_0, const MatView<T>& mv_1) {
        Mat<T> res(m_0.size());
        for (auto r = 0; r < m_0.row_count(); ++r) {
            for (auto c = 0; c < m_0.col_count(); ++c) {
                res(r, c) = m_0(r, c) * mv_1(r, c);
            }
        } 
        return res;
    }
    
    template<class T>
    Mat<T> elem_pow(const Mat<T>& mat, double p) {
        Mat<T> r(mat.row_count(), mat.col_count());
        r.elements_ = std::pow(mat.elements_, p);
        return r;
    }
    template<class T>
    Mat<T> elem_pow(double numb, const Mat<T>& p) {
        Mat<T> r(p.row_count(), p.col_count());
        r.elements_ = std::pow(numb, p.elements_);
        return r;
    }
    
    template<class T>
    Mat<T> elem_log(const Mat<T>& mat) {
        Mat<T> r(mat.row_count(), mat.col_count());
        r.elements_ = std::log(mat.elements_);
        return r;
    }
    
        
    template<class T>
    Mat<T>& operator+=(Mat<T>& mat, Mat<T>& mat_other) {
        mat.elements_ += mat_other.elements_;
        return mat;
    }
     
    /* unfortunately we can't iterate over elements somewhere else... hard core loops for speed neccessary */
    template<class T> 
    const MutableMatView<T>& operator/=(const MutableMatView<T>& view, double g) {
        auto &rect = view.region_;
        auto 
        r_begin = rect.row_begin(),
        r_end   = rect.row_end(),
        c_begin = rect.col_begin(),
        c_end   = rect.col_end();
        auto &m = view.mat(); 
        for (; r_begin < r_end; ++r_begin) {
            for (; c_begin < c_end; ++c_begin) {
                m(r_begin, c_begin) /= g;
            }
        }
        return view;
    }
    
    template<class T>
    Mat<T>& operator/=(Mat<T>& mat, double g) {
        mat.elements_ /= g;
        return mat;
    }
    
    template<class T>
    Mat<T>& operator+=(Mat<T>& mat, double g) {
        mat.elements_ += g;
        return mat;
    }
    
    template <class T>
    const MutableMatView<T>& operator-=(const MutableMatView<T>& view, 
                                        const Mat<T>& mat) {
    
        view.elements() -= mat.elements_;
        return view;
    }
    
    template <class T> 
    const MutableMatView<T>& operator-=(const MutableMatView<T>& v_0, const MatView<T>& v_1) {
//        Region 
//        g_0 = v_0.region_,
//        g_1 = v_1.region_;
//        for (auto r_0 = g_0.row_begin(), r_1 = g_1.row_begin(); r_0 != g_0.row_end(); ++r_0, ++r_1) {
//            for (auto c_0 = g_)
//            v_0.mat_.elements_[std::slice(0 , 0 ,35)] -= v_1.mat_.elements_[std::slice(0, 0, 35)];
//        }
        return v_0;
    }

    
    template<class T>
    Mat<T> operator==(const Mat<T>& mat, const T& g) {
        Mat<T> r(mat.row_count(), mat.col_count());
        r.elements_ = mat.elements_ == g;
        return r;
    }
    template<class T>
    Mat<T> operator==(const T& g, Mat<T>& mat) {
        return mat == g;
    }
    
    template<class T>
    Mat<T> operator*(double g, const Mat<T>& mat) {
        Mat<T> r(mat.size());
        r.elements_ = g * mat.elements_;
        return r;
    }
    
    template<class T>
    Mat<T> operator*(const Mat<T>& m_0, const Mat<T>& m_1) {
        Mat<T> m(m_0.row_count(), m_1.col_count());
        for (auto r_0 = 0; r_0 < m_0.row_count(); ++r_0) {
            for (auto c_1 = 0; c_1 < m_1.col_count(); ++c_1) {
                double d = 0;
                for (auto k = 0; k < m_0.col_count(); ++k) {
                    d += m_0(r_0, k)*m_1(k, c_1);
                }
                m(r_0, c_1) = d;
            }
        }
        return m;
    }
    
    template<class T>
    Mat<T> operator+(const Mat<T>& mat, double g) {
        Mat<T> r(mat.row_count(), mat.col_count());
        r.elements_ = mat.elements_ + g;
        return r;
    }
    template<class T>
    Mat<T> operator+(double g, const Mat<T>& mat) {
        return mat + g;
    }
    
    template<class T>
    Mat<T> operator-(double g, const Mat<T>& mat) {
        Mat<T> r(mat.size());
        r.elements_ = g - mat.elements_;
        return r;
    }
    template<class T> 
    Mat<T> operator-(const Mat<T>& mat, double g) {
        return mat + -g;
    }
    
    template<class T>
    Mat<T> operator-(const Mat<T>& m_0, const Mat<T>& m_1) {
        Mat<T> r(m_0.size());
        r.elements_ = m_0.elements_ - m_1.elements_;
        return r;
    }
    
    template<class T>
    Mat<T> operator-(const Mat<T>& m, const MatView<T>& m_v) {
        Mat<T> r(m.size());
        r.elements_ = m.elements_ - m_v.elements();
        return r;
    }
    
    template<class T>
    Mat<T> operator/(const Mat<T>& mat, double g) {
        Mat<T> r(mat.row_count(), mat.col_count());
        r.elements_ = mat.elements_ / g;
        return r;
    }
    template<class T>
    Mat<T> operator/(double g, const Mat<T>& mat) {
        Mat<T> r(mat.row_count(), mat.col_count());
        r.elements_ = g / mat.elements_;
        return r;
    }
    
    template<class T>
    std::ostream& operator<<(std::ostream& output, const MatView<T>& view) {
        auto& m = view.mat();
        for (auto r = view.region_.row_begin(); r < view.region_.row_end(); ++r) {
            for (auto c = view.region_.col_begin(); c < view.region_.col_end(); ++c) {
                output << m(r, c) << " | ";
            }
            output << std::endl;
        }
        return output;
    }
    
} }




template <typename T>
typename ant::algebra::MatView<T>& operator*=(ant::algebra::MatView<T>& view, double g) {
    return view;
}

template <class T>
ant::algebra::Mat<T>& operator*=(ant::algebra::Mat<T>& mat, double g) {
    mat.elements_ *= g;
    return mat;
}

template <class T> 
ant::algebra::Mat<T>& operator-=(ant::algebra::Mat<T>& mat, const ant::algebra::MatView<T>& view) {
    ant::algebra::MutableMatView<T>(mat, 0, 0, mat.row_count(), mat.col_count()) -= view;
    return mat;
}


template <class T> 
ant::algebra::Mat<T> operator*(const ant::algebra::MatView<T>& view, double g) {
    ant::algebra::Mat<T> m(view);
    return m *= g;
}

template <class T>
std::ostream& operator<<(std::ostream& output, const ant::algebra::Mat<T>& m) {
    for (auto r = 0; r < m.row_count(); ++r) {
        for (auto c = 0; c < m.col_count(); ++c) {
            output << m(r, c) << " | ";
        }
        output << std::endl;
    }
    return output;
}

#endif /* defined(__Ant__math__) */
