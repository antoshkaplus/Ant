//
//  simplex_method.h
//  Ant
//
//  Created by Anton Logunov on 4/23/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef __Ant__simplex_method__
#define __Ant__simplex_method__

#include <iostream>
#include <limits>

#include "ant/linear_algebra/matrix.h"
#include "ant/core.h"

namespace ant { namespace opt { namespace lp {
    class CanonicalForm;
}}}


std::ostream& operator<<(std::ostream& output, const ant::opt::lp::CanonicalForm& form);


namespace ant { namespace opt { namespace lp {

    using Matrix = linalg::Matrix<double>;
    using MatView = linalg::MatrixView<double>;
    using MutMatView = linalg::MutableMatrixView<double>;
    
    class CanonicalForm;
    class SimplexMethod;
    
    
    class StandardForm {
    /* never forget that 
            first row : objective factors
            first col : constraint values
            (0, 0)    : objective free variable
    */
    
    public:
        explicit StandardForm(const Matrix& system) : system_(system) {}
    
        void pivot(Index row, Index col) {
            double g = system_(row+1, col+1);
            system_.row(row+1) /= g;
            for (Index r = 0; r < system_.row_count(); ++r) {
                if (r == row+1) continue;
                g = system_(r, col+1);
                //system_.row(r) -= system_.row(row+1)*g;
            }
        }
        
        Count constraint_count() const {
            return system_.row_count()-1;
        }
        
        Count variable_count() const {
            return system_.col_count()-1;
        }
        
        void set_system(const Matrix& system) {
            system_ = system;
        }
        const Matrix& system() const { return system_; }
        
    private:
        Matrix system_;
    friend class CanonicalForm;
    };
    
    
    class CanonicalForm {
    public:
        // basic variables stand in rows // non_basic - in columns
        
        CanonicalForm(const StandardForm& form, 
                      const std::vector<Index>& basic_vars, 
                      const std::vector<Index>& non_basic_vars) {
            set(form, basic_vars, non_basic_vars);
        }
        
        explicit CanonicalForm(const StandardForm& form) {
            set(form);
        }
        
        CanonicalForm() {}
        
        // this is best on
        // to give user permission to eliminate some variables without a need to shift others 
        // you need to provide both vectors (we could just ask basic variables)
        void set(const StandardForm& form, const std::vector<Index>& basic_vars, const std::vector<Index>& non_basic_vars) {
            system_ = form.system_;
            basic_vars_ = basic_vars;
            non_basic_vars_ = non_basic_vars;
        }
        
        // if your variables have specific order or named somehow special or something
        // we provide this function for you
        void set(const StandardForm& form, std::vector<Index> vars) {
            
        }

        // no basic variables // everything is non basic
        void set(const StandardForm& form);
                
        const Matrix& system() const {
            return system_;
        }
        
        const std::vector<Index> basic_vars() const {
            return basic_vars_;
        }
        
        const std::vector<Index> non_basic_vars() const {
            return non_basic_vars_;
        }
        
        Count variable_count() const {
            return basic_vars_.size() + non_basic_vars_.size();
        }
        
        // both indeces should be increased by one to skip objective factors and basic var values 
        // maybe you want to expand form interface with matViews
        void pivot(Index row, Index col) {
            auto &s = system_;
            double g = s(row+1, col+1);
            s.row(row+1) /= g;
            // swapping basis and non basis
            s(row+1, col+1) /= g;
            for (Index r = 0; r < s.row_count(); ++r) {
                if (r == row+1) continue;
                g = s(r, col+1);
                s(r, col+1) = 0;
                MutMatView srow = s.row(r); 
                srow -= s.row(row+1)*g;
            }
            // those only i don't need to increase
            std::swap(basic_vars_[row], non_basic_vars_[col]);
        }

    private:
        // matrix 
        // b vector of basic variables
        // non_b vector of non basic veriables
        // b[i] => variable is basic in row i
        // non_b[j] => variable is non_basic in column j
        // first row for coefficients
        // last column for free variables
    
        Matrix system_;
        std::vector<Index> basic_vars_;
        std::vector<Index> non_basic_vars_;
        
    friend class SimplexMethod;
    };
    
    
    class SimplexMethod {
    public:

        void init(const CanonicalForm& form) {
            form_ = form;
        }
        
                
        // ingoing variable
        Index findPivotColumn() {
            Index min_i = form_.variable_count()+1;
            double min_factor = MAXFLOAT, factor;
            for (auto i = 1; i < form_.system_.col_count(); ++i) {
                if ((factor = form_.system_(0, i)) < 0 && factor < min_factor) {
                    min_factor = factor;
                    min_i = i;
                }
            }
            return min_i-1;
        }
        
        Index findPivotRow(Index col) {
            Index min_row = form_.system().row_count();
            double min_factor = MAXFLOAT, factor;
            for (auto r = 1; r < form_.system_.row_count(); ++r) {
                if ((factor = form_.system_(r, 0)/form_.system_(r, col+1)) < min_factor &&
                     form_.system_(r, col+1) > 0) {
                    min_factor = factor;
                    min_row = r;
                }
            }
            return min_row-1;
        }
        
               
        bool isBounded(Index k) {
            for (auto i = 1; i < form_.system_.row_count(); ++i) {
                if (form_.system_(i, k+1) > epsilon_) {
                    return true;
                }
            }
            return false;
        }
        
        CanonicalForm solve(const CanonicalForm& form) {
            form_ = form;
            Index row, col;
            while (true) {
                if ((col = findPivotColumn()) == form_.variable_count()) break;
                
                std::cout << col << std::endl; 
                
                if (!isBounded(col)) break;
                row = findPivotRow(col);
                
                std::cout << row << std::endl;
                
                form_.pivot(row, col);
                
                std::cout << form_;
            }
            return form_;
        }
    private:
        CanonicalForm form_;
        double epsilon_;
    };

    
    class IntegerBranchAndBound {
    public:
        // 
//        static Index pickBranchingRow(const CanonicalForm& form) {
//            // min(m - (floor(m)+0.5))
//        } 
//    
        // on each step we add some sort of constraint to lenear program
        // then we have new lenear program to solve in standard form ??? .. get canonical form again
        //  
    
//        CanonicalForm solve(const CanonicalForm& form) {
//            
//            
//            
//            //return from_;
//        }
        
        
    private:
//        double epsilon_;
        CanonicalForm form_;
    
    
        
        
    };
    
} } }


#endif /* defined(__Ant__simplex_method__) */
