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


#include "./core/core.hpp"
#include "./linear_algebra/matrix.h"



namespace ant {

namespace ml {

using namespace linalg;


// possibly will use not only matrices but Glut objects and MatrixViews
template<class B>
Matrix<typename B::value_type> sigmoid(const B& X) {
    return 1./(1.+exp(-X));     
}
//
//template<class B> 
//Matrix<typename B::value_type> 



// PointArray :
//      meaning: array possible of points
//      interface: size(), [i]
// Point : 
//      meaning: origin point
//      interface: 
// Distance :
//      meaining: distance between two points
//      interface: (p_0, p_1)
// returns array of neariest points indices
template<class PointArray, class Point, class Distance>
std::vector<Index> FindNearestNeighbors(const PointArray& point_array, 
                                        const Point& point, 
                                        Count k, 
                                        const Distance& distance) {
    std::vector<Index> inds(point_array.size());
    std::iota(inds.begin(), inds.end(), 0);
    std::vector<double> dist(point_array.size());
    for (auto i = 0; i < point_array.size(); ++i) {
        dist[i] = distance(point_array[i], point);
    }
    // can be greater than number of inds
    Count valid_k = std::min(k, (Count)inds.size());
    std::nth_element(inds.begin(), inds.begin()+valid_k, inds.end(), [&](Index i_0, Index i_1) {
        return dist[i_0] < dist[i_1];
    });
    inds.resize(valid_k);
    return inds;
}



template<typename TrainX, typename TrainY, typename TestX>
class KnnClassification {
public:
    void classify(const TrainX& train_x, const TrainY& train_y, const TestX& test_x, Count k) {
//        float g;
    }

private:
    // column of classes
    Col<Index> test_y_;
    
    // row matrix
    //Matrix<Index> 
};




// Example : 
//      meaning: array of features of some point that we will classify
//      interface: [i], size() 
// ValueProb :
//      meaning: getter of probabilities
//      interface: (class index, feature index, value of feature) -> probability
// return predicted class for raw 
template<class Example, class ValueProb>
Index naive_bayes(const Example& example, const std::vector<double>& class_prob, ValueProb value_prob) {
    Index cl = 0;
    double prob_cl = 0, prob;
    for (Index i = 0; i < class_prob.size(); ++i) {
        prob = class_prob[i];
        for (Index k = 0; k < example.size(); ++k) {
            prob *= value_prob(i, k, example[k]);
        }
        if (prob > prob_cl) {
            cl = i;
            prob_cl = prob;
        }
    }
    return cl;
}


// Category :
//      meaning: returns category of example
//      interface: (example index)
template<class Container, class Category>
double entropy(const Container& sample_indices, const Category& category, Count category_count) {
    std::vector<Count> count(category_count, 0);
    for (Index si : sample_indices) {
        ++count[category[si]];
    }
    double ent = 0;
    for (auto c : count) {
        if (c == 0) continue;
        double p = (double)c/sample_indices.size();
        ent -= p*std::log(p);
    }
    return ent;
}


class TrainSet {
    virtual Count feature_count() = 0;
    virtual Count example_count() = 0;
};

// TrainSet :
//      meaning: training set, probably looks like a matrix, should store category in first column
//          rows correspond to samples
//      interface: (index of sample, index of feature), example_count(), feature_count() - returns count of examples
//              or should introduce new variable for constructor [i] - return example with index
// Category : 
//      meaning: returns category of example
//      interface: (index of sample)
// ConditionSet :
//      meaning: set of conditions, that return true or false. will choose on that fits in each 
//          on each tree branching
//      interface: iteration , value_type, probably index operator
// Condition : should be able bet bool value from (example)
// Example :
//      meaning: example that need classification
template<class TrainSet, class Category, class ConditionSet, class Example>
class binary_decision_tree {
   // using Condition = ConditionSet::value_type;
    using Condition = std::function<bool(const std::array<int, 784>& example)>;
    
    // define recursion procedure
    // leafs : category nodes, other nodes are decision ones
    struct Node {
        virtual Index category(const Example& example) const = 0;
    };
    
    struct DecisionNode : Node {
        DecisionNode(const Condition& condition, Node* yes, Node *no) 
            : condition_(condition), yes_(yes), no_(no) {}
        Index category(const Example& example) const override {
            Node* node = condition_(example) ? yes_ : no_;
            return node->category(example);
        }
    private:
        const Condition& condition_;
        Node* yes_;
        Node* no_; 
    };
    
    struct CategoryNode : Node {
        CategoryNode(Index category) : category_(category) {}
        Index category(const Example& raw) const override {
            return category_;
        }
    private:
        Index category_;
    };
    
    
    using Container = std::vector<Index>;
        
    struct SplitResult {
        Container yes;
        Container no;
    };
    
    SplitResult split(const Container& example_indices, Index condition_index) {
        SplitResult sr;
        for (auto i : example_indices) {
            auto& r = condition_set_[condition_index](train_set_[i]) ? sr.yes : sr.no;
            r.push_back(i);
        }
        return sr;
    }
    
    Index max_info_gain_condition(const Container& conditions, const Container& example_indices) {
        double total_entropy = entropy(example_indices);
        // information gain
        double max_ig = std::numeric_limits<double>::lowest();
        Index max_ig_condition = 0;
        for (Index i = 1; i < conditions.size(); ++i) {
            auto sr = split(example_indices, conditions[i]);
            double 
            yes_entropy = sr.yes.size() > 0 ? entropy(sr.yes) : 0.,
            no_entropy = sr.no.size() > 0 ? entropy(sr.no) : 0.; 
            double ig = total_entropy
            - yes_entropy*sr.yes.size()/example_indices.size()
            -  no_entropy* sr.no.size()/example_indices.size();
            if (max_ig < ig) {
                max_ig = ig;
                max_ig_condition = i;
            }
        }
        return max_ig_condition;
    }
    
    double entropy(const Container& example_indices) {
        return ml::entropy(example_indices, category_, category_count_);
    }
    
    bool same_category(const Container& examples) {
        Index c = category_[examples[0]];
        for (auto i : examples) {
            if (c != category_[i]) return false;
        }
        return true;
    }
    
    Node* build(Container conditions, Container examples) {
        Node* node;
        if (same_category(examples)) {
            node = new CategoryNode(category_[examples.front()]); 
        } else {
            assert(!conditions.empty());
            Index i = max_info_gain_condition(conditions, examples);
            SplitResult sr = split(examples, conditions[i]);
            std::swap(conditions[i], conditions.back());
            conditions.pop_back();
            Node* yes = build(conditions, sr.yes);
            Node* no  = build(conditions, sr.no); 
            conditions.push_back(i);
            node = new DecisionNode(condition_set_[conditions.back()], yes, no);
        }
        return node;
    }
    
    
public:
    binary_decision_tree(const TrainSet& train_set, 
                         const Category& category,
                         const ConditionSet& condition_set, 
                         Count category_count)
        :   train_set_(train_set),
            category_(category),
            condition_set_(condition_set),
            category_count_(category_count) {
        
        Container conditions(condition_set.count());
        std::iota(conditions.begin(), conditions.end(), 0);
        Container examples(train_set.example_count());
        std::iota(examples.begin(), examples.end(), 0);
        root = build(conditions, examples);     
    }
    
    Index categorize(const Example& example) {
        return root->category(example);
    }
private:
    // don't need that after classification, construction probably
    const TrainSet& train_set_;
    const Category& category_;
    const ConditionSet& condition_set_;
    Count category_count_;
    Node* root;
};



// for each category we are looking for feature count parameters,
// that will decrease cost for all examples of this class in training set
// TrainSet :
//      meaning: matrix with examples as rows and features as columns
//      interface: (example_index, feature_index)
// Category :
//      interface: [example_index] => category index 
template<class T> // letting decide float vs double
// category is column
struct logistic_regression {
    logistic_regression(const Matrix<T>& train_set, const Matrix<Index>& category, Count category_count)
    : category_coeffs(category_count, train_set.col_count()) {
        for (auto cat = 0; cat < category_count; ++cat) {
            // will be needed for cost computation
            Matrix<bool> is_category(category.size());
            for (int r = 0; r < category.row_count(); ++r) {
                is_category(r, 0) = category(r, 0) == cat ? 1. : 0.;
            }            
            Matrix<T> cat_coeffs(1, train_set.col_count()), cat_coeffs_new(cat_coeffs.size());
            cat_coeffs.random();
            cat_coeffs -= 0.5;
            // gradient descend method with dicreasing step
            Index iteration = 0;
            double step = 1., cat_cost = cost(train_set, is_category, cat_coeffs), cat_cost_new = 10; 
            while (true) {
                auto grad = cost_gradient(train_set, is_category, cat_coeffs);
                auto to = cat_coeffs - step * grad;
                cat_coeffs_new = to;
                if ((cat_cost_new = cost(train_set, is_category, cat_coeffs_new)) > cat_cost) {
                    step /= 2.;
                } else {
                    if (std::abs(cat_cost - cat_cost_new) < 1.e-3) break;
                    cat_coeffs = cat_coeffs_new;
                    cat_cost = cat_cost_new;
                }
                if (++iteration % 100 == 0) {
                    std::cout << "cat: " << cat << " it: " << iteration << " cost: " << cat_cost << std::endl;
                }
                if (iteration == 1000) break;
            }
            category_coeffs.row(cat) = cat_coeffs;
        }
    }
    
    // rows of features go in
    // returns rows of predicted categories
    Matrix<Index> categorize(const Matrix<T>& features) {
        auto prob = sigmoid(features, category_coeffs);
        Matrix<Index> cat = prob.each_row_argmax();
        return cat;
    }
    
    
    // auto needed!!!!!! fuck that shit!!!!!
    // is_category - column, is this example from current category
    Matrix<T> sigmoid(const Matrix<T>& features, 
                      const Matrix<T>& category_coeffs) {
        return ml::sigmoid(features * transposed(category_coeffs));
    }
    
    // is_category is also a column
    // cat_coeffs is a row
    double cost(const Matrix<T>& features, 
                const Matrix<bool>& is_category,  
                const Matrix<T>& cat_coeffs) {
        // column goes out
        auto h = sigmoid(features, cat_coeffs);
        double p_0 = sum(log(h), is_category);
        double p_1 = sum(log(1.-h), !is_category);
        return -1./features.row_count() * (p_0 + p_1);
    }
    
    // row : for each feature gradient
    Matrix<T> cost_gradient(const Matrix<T>& features,
                            const Matrix<bool>& is_category,
                            const Matrix<T>& cat_coeffs) {
        // return column with example count rows count
        auto h = sigmoid(features, cat_coeffs);
        auto c = 1./features.row_count() * (transposed(h - cast(is_category, 1., 0.)) * features);
        Matrix<T> c_mat(c.size());
        c_mat = c;
        return c_mat;
    }
    
private:
    // rows     : categories 
    // columns  : coeff for each feature
    Matrix<T> category_coeffs;
};


// for X each test case is in the separate vector
class NeuralNetwork {
public:
    // bias coefficient
    NeuralNetwork(const std::vector<int>& layer_sizes, double delta) :  delta_(delta), layer_sizes_(layer_sizes) {
        theta_.resize(layer_sizes_.size()-1);
        for (int i = 0; i < layer_sizes_.size()-1; i++) {
            // +1 for bias node
            theta_[i].set_size(layer_sizes_[i+1], layer_sizes_[i]+1);
            // randu - uniform distribution in the [0,1] interval
            theta_[i].random();
            theta_[i]-=0.5;
            theta_[i]*=2.;
        }
    }
    Matrix<double> activate(const Matrix<double>& X) {
        return forward_propagation(X)[layer_sizes_.size()-1];
    }
    std::vector<Matrix<double>> forward_propagation(const Matrix<double>& X);
    // back propagation
    std::vector<Matrix<double>> gradient(const Matrix<double>& X, const Matrix<double>& Y);
    std::vector<Matrix<double>> gradient_approximation(const Matrix<double>& X, const Matrix<double>& Y, double eps);
    // A - last matrix after forward Propagation
    double cost(const Matrix<double>& A, const Matrix<double>& Y);
    double efficiency(const Matrix<double>& A, const Matrix<double>& Y);
    std::vector<std::vector<int>> report(const Matrix<double>& A, const Matrix<double>& Y);

private:
    double delta_;
    // size is equal to layer count
    std::vector<int> layer_sizes_;
    std::vector<Matrix<double>> theta_;
    
};







} 

} // end namespace ant



#endif /* defined(__Ant__machine_learning__) */
