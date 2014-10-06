//
//  machine_learning.h
//  Ant
//
//  Created by Anton Logunov on 6/12/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef __Ant__machine_learning__
#define __Ant__machine_learning__

#include <iostream>
#include <cmath>
#include <stack>
#include <limits>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>


#include "./core.h"
#include "./alg.h"



namespace ant {

namespace ml {

using namespace linalg;

template<class TrSet, class Raw, class Distance>
std::vector<Index> find_k_nearest_neighbors(const TrSet& tr_set, const Raw& raw, Count k, Distance distance) {
    std::vector<Index> inds(tr_set.size());
    std::iota(inds.begin(), inds.end(), 0);
    std::vector<double> dist(tr_set.size());
    for (auto i = 0; i < tr_set.size(); ++i) {
        dist[i] = distance(tr_set[i], raw);
    }
    std::nth_element(inds.begin(), inds.begin()+k, inds.end(), [&](Index i_0, Index i_1) {
        return dist[i_0] < dist[i_1];
    });
    inds.resize(k);
    return inds;
}


template<class Raw, class ValueProb>
Index naive_bayes(const Raw& raw, const std::vector<double>& class_prob, ValueProb value_prob) {
    Index cl = 0;
    double prob_cl = 0, prob;
    for (Index i = 0; i < class_prob.size(); ++i) {
        prob = class_prob[i];
        for (Index k = 0; k < raw.size(); ++k) {
            prob *= value_prob(i, k, raw[k]);
        }
        if (prob > prob_cl) {
            cl = i;
            prob_cl = prob;
        }
    }
    return cl;
}

template<class TrSet, class Condition, class Raw>
struct binary_decision_tree {
    
    struct Node {
        Node(const binary_decision_tree& tree) : tree_(&tree) {}
        virtual Index category(const Raw& raw) const = 0;
        const binary_decision_tree* tree_;
    };
    
    struct DecisionNode : Node {
        DecisionNode(const binary_decision_tree& tree, Index feature, Node* yes, Node *no) 
        : Node(tree), yes(yes), no(no), feature(feature) {}
        Index category(const Raw& raw) const override {
            return (*Node::tree_->condition_)(raw, feature) ? yes->category(raw) : no->category(raw);
        }
        Node* yes;
        Node* no; 
        Index feature;
        
    };
    
    struct CategoryNode : Node {
        CategoryNode(const binary_decision_tree& tree, Index category)
        : Node(tree), category_(category) {}
        Index category(const Raw& raw) const override {
            return category_;
        }
    private:
        Index category_;
    };
    
    
    using Container = std::vector<Index>;
    
    struct Item {
        Container features;
        Container samples;
        DecisionNode* node;
        // YES, NO
        bool decision;
    };
    
    binary_decision_tree() {}
    
    double entropy(const std::vector<Index>& sample_indices) {
        auto& tr_set = *tr_set_;
        std::vector<Count> count(category_count_);
        std::fill(count.begin(), count.end(), 0);
        for (Index si : sample_indices) {
            ++count[tr_set[si][0]];
        }
        Index i = 0;
        while (i != count.size()) {
            if (count[i] == 0) {
                std::swap(count[i], count.back());
                count.pop_back();
            } else ++i;
        }
        double ent = 0;
        for (auto c : count) {
            double p = (double)c/sample_indices.size();
            ent -= p*std::log(p);
        }
        return ent;
    }
    
    struct SplitResult {
        Container yes;
        Container no;
    };
    
    SplitResult split(const Container& si, Index feature) {
        SplitResult sr;
        auto& tr_set = *tr_set_; 
        for (auto i : si) {
            auto& r = (*condition_)(tr_set[i], feature) ? sr.yes : sr.no;
            r.push_back(i);
        }
        return sr;
    }
    
    Index maxInfoGainFeature(const Container& features, const Container& samples) {
        double glob_entropy = entropy(samples);
        double max_ig = std::numeric_limits<double>::lowest();
        Index max_ig_feat = 0;
        for (Index i = 0; i < features.size(); ++i) {
            auto sr = split(samples, features[i]);
            double 
            yes_entropy = sr.yes.size() > 0 ? entropy(sr.yes) : 0.,
            no_entropy = sr.no.size() > 0 ? entropy(sr.no) : 0.; 
            double ig = glob_entropy
            - yes_entropy*sr.yes.size()/samples.size()
            - no_entropy*sr.no.size() /samples.size();
            if (max_ig < ig) {
                max_ig = ig;
                max_ig_feat = i;
            }
        }
        return max_ig_feat;
    }
    
    bool same_category(const Container& samples) {
        auto& tr_set = *tr_set_; 
        Index c = tr_set[samples[0]][0];
        for (auto i : samples) {
            if (c != tr_set[i][0]) return false;
        }
        return true;
    }
    
    
    void construct(const TrSet& tr_set, Condition condition, Count category_count) {
        tr_set_ = &tr_set;
        category_count_ = category_count;
        condition_ = &condition;
        Item item;
        item.samples.resize(tr_set.size());
        std::iota(item.samples.begin(), item.samples.end(), 0);
        item.features.resize(tr_set[0].size()-1);
        std::iota(item.features.begin(), item.features.end(), 1);
        item.node = nullptr;
        item.decision = true;
        
        std::stack<Item> st;
        st.push(std::move(item));
        
        Node *node;
        while (!st.empty()) {
            Item item = std::move(st.top());
            st.pop();
            // if entropy if 0 should create category node 
            if (same_category(item.samples)) {
                node = new CategoryNode(*this, tr_set[item.samples.front()][0]); 
            } else {
                Index i = maxInfoGainFeature(item.features, item.samples);
                
                node = new DecisionNode(*this, item.features[i], nullptr, nullptr);
                
                SplitResult sr = split(item.samples, item.features[i]);
                std::swap(item.features[i], item.features.back());
                item.features.pop_back();
                
                Item it;
                it.samples = sr.yes;
                it.features = item.features;
                it.decision = true;
                it.node = static_cast<DecisionNode*>(node);
                st.push(it);
                
                it.samples = sr.no;
                it.features = item.features;
                it.decision = false;
                it.node = static_cast<DecisionNode*>(node);
                st.push(it);
            }
            
            if (item.node == nullptr) {
                root = node;
            } else {
                if (item.decision) {
                    item.node->yes = node;
                } else { // NO
                    item.node->no = node;
                }
            }
        }        
    }
    
    Index category(const Raw& raw) {
        return root->category(raw);
    }
private:
    const TrSet* tr_set_;
    Count category_count_;
    Condition *condition_;
    Node* root;
};



struct logistic_regression {
    void train(const Matrix<double>& features, const Matrix<Index>& categories, Count category_count) {
        Index iteration;
        train_features = &features;
        train_categories = &categories;
        Count feature_count = features.col_count();
        coeffs.set_size({(Int)category_count, (Int)feature_count});
        for (auto i = 0; i < category_count; ++i) {
            Matrix<double> cat_coeffs(1, feature_count), cat_coeffs_new;
            cat_coeffs.fill(1.);
            
            std::uniform_real_distribution<double> d(0., 1.); 
            std::default_random_engine rng;
            for (auto r = 0; r < cat_coeffs.row_count(); ++r) {
                for (auto c = 0; c < cat_coeffs.col_count(); ++c) {
                    cat_coeffs(r, c) = d(rng)-0.5;
                }
            }
            
            iteration = 0;
            double step = 1., cat_cost = cost(cat_coeffs, i), cat_cost_new = 10; 
            while (true) {
                cat_coeffs_new = cat_coeffs - step * costGradient(cat_coeffs, i);
                if ((cat_cost_new = cost(cat_coeffs_new, i)) > cat_cost) {
                    step /= 2.;
                } else {
                    cat_coeffs = cat_coeffs_new;
                    cat_cost = cat_cost_new;
                }
                if (++iteration % 100 == 0) {
                    std::cout << "cat: " << i << " it: " << iteration << " cost: " << cat_cost << std::endl;
                }
                if (iteration == 1000) break;
            }
            coeffs.row(i) = cat_coeffs;
        }
    }
    
    // rows of features go in
    // returns column
    Matrix<Index> predict(const Matrix<double>& features) {
        auto prob = sigmoid(features, coeffs);
        return prob.argmax_each_row();
    }
    
    // features are rows, coeffs are also rows
    // return rows for every feature row and column for every coeff row
    Matrix<double> sigmoid(const Matrix<double>& features, const Matrix<double>& coeffs) {
        auto z = features * coeffs.transposed();
        z *= -1;
        Matrix<double> h = 1./(1. + elem_pow(M_E, z));
        return h;
    }
    
    // cat_coeffs is a row
    double cost(const Matrix<double>& cat_coeffs, Index category) {
        // row
        auto y_bool = (*train_categories == category).transposed();
        Matrix<double> y(y_bool.size());
        for (auto r = 0; r < y.row_count(); ++r) {
            for (auto c = 0; c < y.col_count(); ++c) {
                y(r, c) = y_bool(r, c);
            }
        }
        // column goes out
        auto h = sigmoid(*train_features, cat_coeffs);
        for (auto r = 0; r < h.row_count(); ++r) {
            for (auto c = 0; c < h.col_count(); ++c) {
                assert(h(r, c) != 1.); 
            }
        }
        double p_0 = (y*elem_log(h))(0, 0);
        double p_1 = ((1.-y)*elem_log(1.-h))(0, 0);
        return -1./train_categories->row_count() * (p_0 + p_1);
    }
    
    // row : for each feature gradient
    Matrix<double> costGradient(const Matrix<double>& cat_coeffs, Index category) {
        auto y_bool = (*train_categories == category);
        Matrix<double> y(y_bool.size());
        for (auto r = 0; r < y.row_count(); ++r) {
            for (auto c = 0; c < y.col_count(); ++c) {
                y(r, c) = y_bool(r, c);
            }
        }
        auto h = sigmoid(*train_features, cat_coeffs);
        return 1./train_categories->row_count() * ((h - y).transposed() * *train_features);
    }
    
private:
    // raws : categories, columns : coeff for each feature
    Matrix<double> coeffs;
    // row : example, column : feature
    const Matrix<double> *train_features;
    // column
    const Matrix<Index> *train_categories;
    
};



} 

} // end namespace ant



#endif /* defined(__Ant__machine_learning__) */
