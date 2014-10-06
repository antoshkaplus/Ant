//
//  simplex_method.cpp
//  Ant
//
//  Created by Anton Logunov on 4/23/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#include "ant/simplex_method.h"

namespace ant { namespace opt { namespace lp {
    
    void CanonicalForm::set(const StandardForm& form) {
        std::vector<Index> non_bv(form.variable_count());
        iota(non_bv.begin(), non_bv.end(), 0);
        std::vector<Index> bv(form.constraint_count());
        iota(bv.begin(), bv.end(), form.variable_count());
        CanonicalForm cf(form, bv, non_bv);
        
        std::cout << cf << std::endl;
        
        Matrix obj(1, form.variable_count()+1);
        obj.fill(0);
        // go away from ones in objective
        for (auto r = 1; r < form.system_.row_count(); ++r) {
            obj -= form.system_.row(r);
        }
        cf.system_.row(0) = obj;
        
        std::cout << cf << std::endl;
        
        SimplexMethod sm;
        cf = sm.solve(cf);
        
        std::cout << cf << std::endl;
        
        auto c = -1; // because of future increment
        for (auto r = 0; r < form.constraint_count(); ++r) {
            if (cf.basic_vars_[r] >= form.variable_count()) {
                while (cf.non_basic_vars_[++c] >= form.variable_count());
                // need this method here
                // don't know why, but should be able to pivot
                cf.pivot(r, c);
            }
        }
        
        std::vector<Index> added_var_inds;
        for (auto i = 0; i < cf.non_basic_vars().size(); ++i) {
            if (cf.non_basic_vars_[i] >= form.variable_count()) {
                // don't forget about value column
                added_var_inds.push_back(i+1); // +1 ?
            }
        }
        // removed cuz of constructing
        //system_ = cf.system_.withoutCols(added_var_inds);
        basic_vars_ = cf.basic_vars_;
        non_basic_vars_.clear();
        for (auto i = 0, k = 0; i < cf.non_basic_vars_.size(); ++i) {
            // can just compare with variable count of form
            if (added_var_inds[k]-1 == i) {
                ++k; continue;
            }
            non_basic_vars_.push_back(cf.non_basic_vars_[i]);
        }

        // now express objective
        system_.row(0).fill(0);
        system_(0, 0) = form.system_(0, 0);
        for (auto i = 0; i < non_basic_vars_.size(); ++i) {
            system_(0, i+1) = form.system_(0, non_basic_vars_[i]+1);
        }
        for (auto i = 0; i < basic_vars_.size(); ++i) {
            MutMatView srow = system_.row(0); 
            srow -= system_.row(i+1)*form.system_(0, basic_vars_[i]+1);
        }
    }
    
} } }

std::ostream& operator<<(std::ostream& output, const ant::opt::lp::CanonicalForm& form) {
    //output << "system:" << std::endl << form.system();
    output << "basic variables:" << std::endl;
    for (auto b : form.basic_vars()) std::cout << b << ", ";
    output << std::endl;
    output << "non basic variables:" << std::endl;
    for (auto b : form.non_basic_vars()) std::cout << b << ", ";
    return output << std::endl;
}

