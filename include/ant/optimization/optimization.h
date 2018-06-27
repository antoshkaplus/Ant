//
//  opt.h
//  Ant
//
//  Created by Anton Logunov on 5/9/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef __Ant__opt__
#define __Ant__opt__

#include <iostream>
#include <memory>
#include <stack>
#include <unordered_map>

#include "ant/core/core.hpp"

namespace ant { 

namespace opt {


template<class T>
struct Node {

    T value;
    std::shared_ptr<Node<T>> previous;
    
    // current element considered last
    static std::vector<T> ToVector(std::shared_ptr<Node<T>> node) {
        std::vector<T> r;
        while (node) {
            r.push_back(node->value);
            node = node->previous;
        }
        std::reverse(r.begin(), r.end());
        return r;
    }
    
    static std::shared_ptr<Node<T>> DeepCopy(std::shared_ptr<Node<T>> node) {
        std::shared_ptr<Node<T>> cur, next, result;
        if (node) {
            next.reset(new Node<T>());
            next->value = node->value;
            result = next;
        }  
        while ((node = node->previous)) {
            cur.reset(new Node<T>());
            cur->value = node->value;
            next->previous = cur;
            next = cur;
        }
        return result;
    }
    
    // may change head thats why return 
    static std::shared_ptr<Node<T>> Erase(std::shared_ptr<Node<T>> node, const T& value) {
        if (!node) return node;
        if (node->value == value) {
            return node->previous;
        }
        auto result = node;
        auto next = node;
        node = node->previous;
        while (node) {
            if (node->value == value) {
                next->previous = node->previous;
                break;
            }
            next = node;
            node = node->previous;
        }
        return result;
    }
    
    static bool Exists(std::shared_ptr<Node<T>> node, const T& value) {
        while (node) {
            if (node->value == value) {
                return true;
            }
            node = node->previous;
        }
        return false;
    }
};







/// need some washing method to decrease memory usage by this structure
template <class Key, class Hash = std::hash<Key>>
struct TabuList {
    // keep key and iteration index until which we keep key as tabu
    std::unordered_map<Key, Index, Hash> list;
    
    bool hasInside(const Key& key, Index current_iteration) {
        auto it = list.find(key);
        if (it == list.end()) return false;
        if (it->second < current_iteration) {
            list.erase(it);
            return false;
        } 
        return true;
    }
    
    
    void insert(const Key& key, Count tenure, Index current_iteration) {
        list[key] = tenure + current_iteration;
    }
    
    
};

// return x of minimum
// always minimize
template<class Func>
double GoldenSectionSearch(double a, double b, const Func& f, double eps) {
    double tau = (1. + sqrt(5.))/2.;
    auto f_a = f(a);
    auto f_b = f(b);
    double x_1 = b + (a - b) / tau;
    double x_2 = a + (b - a) / tau;
    auto f_x_1 = f(x_1);
    auto f_x_2 = f(x_2);

    auto onGoLeft = [&]() {
        b = x_2;
        f_b = f_x_2;
        x_2 = x_1;
        f_x_2 = f_x_1;
        x_1 = a + b - x_2;
        f_x_1 = f(x_1);
    };
    auto onGoRight = [&] () {
        a = x_1;
        f_a = f_x_1;
        x_1 = x_2;
        f_x_1 = f_x_2;
        x_2 = a + b - x_1;
        f_x_2 = f(x_2);
    };

    while (b - a > eps) {
        if (f_x_1 < f_x_2) {
            if (f_x_2 > f_b) throw std::runtime_error("function is not unimodal");
            onGoLeft();
        } else if (f_x_1 > f_x_2) {
            if (f_x_1 > f_a) throw std::runtime_error("function is not unimodal");
            onGoRight();
        } else {
            // f_x_1 == f_x_2
            if (f_x_1 > f_a && f_x_1 > f_b) throw std::runtime_error("function is not unimodal");

            if (f_x_1 > f_a) {
                // f_x_1 < f_b || f_x_1 == f_b
                onGoLeft();

            } else if (f_x_1 > f_b) {
                // f_x_1 < f_a || f_x_1 == f_a
                onGoRight();

            } else {
                // another one has to be equal
                if (f_a > f_x_1) {
                    return b;
                }
                if (f_b > f_x_2) {
                    return a;
                }
                // everything on one level
                return a;
            }
        }
    }
    return (a + b)/2.;
}



 
struct BeamSearch {
    // element that is applyed on State to get father
    struct Component {};
    
    // element that allows to compare States
    // should also probably consider amount of 
    // applyed components
    // looking for small values in optimization problem
    struct Value {
        virtual bool operator<(const Value& v) const = 0; 
        virtual bool operator>(const Value& v) {
            return v < *this;
        }

        virtual ~Value() {}
    };
    
    struct State {
        virtual bool isFinal() const = 0;
        
        // can't make it const // smetimes candidates require some inner work
        virtual std::vector<std::unique_ptr<Component>> candidateComponents() = 0;
        
        // move state father by applying component
        virtual void push(const Component& comp) = 0;
        
        // cancels last applied component (moving backward)
        // probably won't need it
        virtual void pop() = 0; 
        
        // should return pointer bacause of inheritance 
        // but it's kind of outdated
        virtual std::unique_ptr<Value> value() const = 0;
        
        // to try out multiple candidate states out of this state need 
        // to create multiple copies
        virtual std::unique_ptr<State> clone() const = 0;
        
        virtual State& operator=(const State& s) { return *this; }
        virtual bool operator==(const State& s) const = 0;

        virtual ~State() {}
        // maybe think of some kind of hash function // with Index type or something like that 
    };
    
    
    // maybe add variable length width beam possibility
    std::unique_ptr<State> solve(const State& initial_state, Count max_state_count) {
        max_state_count_ = max_state_count;
        state_cands.emplace_back(initial_state.clone(), initial_state.value());
        while (true) {
            initCandidates();
            
            // maybe add more strategies for evacuating from main loop
            
            if (candidates.empty()) break;
            initFutureStateCands();
            updateStateCands();
        }
        // take biggest state (value) and make it s state
        return std::move(std::min_element(state_cands.begin(), state_cands.end(), state_candidate_comparator_)->state);
    }

private:    
    
    // triple that define future step for state 
    // because one state can be shared between multiple instances
    struct ComponentCandidate {
        ComponentCandidate(std::shared_ptr<State> state, 
                           std::unique_ptr<Component> component, 
                           std::unique_ptr<Value> value) 
            :   state(state), 
                component(std::move(component)), 
                value(std::move(value)) {}
        // original state
        std::shared_ptr<State> state;
        // component that is applyed
        std::unique_ptr<Component> component;
        // resulting state score
        std::unique_ptr<Value> value;
    };
    
    struct StateCandidate {
        StateCandidate() {}
        StateCandidate(std::unique_ptr<State> state, std::unique_ptr<Value> value)
        : state(std::move(state)), value(std::move(value)) {}
        
        StateCandidate(StateCandidate&& cand) 
        : state(std::move(cand.state)), value(std::move(cand.value)){}
        
        StateCandidate& operator=(StateCandidate&& cand) {
            state = std::move(cand.state);
            value = std::move(cand.value);
            return *this;
        }
        
        bool operator==(const StateCandidate& s) const {
            return *state == *s.state;
        }
        
        std::unique_ptr<State> state;
        std::unique_ptr<Value> value;
    };
    
    // i could keep value in array and don't touch virtual method
    // what if it's difficult to create those objects
    struct StateCandidateLess {
        bool operator()(const StateCandidate& s_0, const StateCandidate& s_1) {
            return *s_0.value < *s_1.value;
        }
    };
    
    struct ComponentCandidateLess {
        bool operator()(const ComponentCandidate& t_0, const ComponentCandidate& t_1) {
            return *t_0.value < *t_1.value;
        }
    };
    
    // state_cands should be already initialized
    void initCandidates() {
        // some component were added
        candidates.clear();
        std::unique_ptr<Value> value;
        for (auto& s_c : state_cands) {
            std::shared_ptr<State> s(std::move(s_c.state));
            // should have any candidate if not in final state
            if (s->isFinal()) continue;
            // user should know by himself how many candidates to 
            // produce.. i will process all of them
            for (auto& c : s->candidateComponents()) {
                s->push(*c);
                value = s->value();
                s->pop();
                candidates.emplace_back(s, std::move(c), std::move(value));
            }
        }
    }
    
    void initFutureStateCands() {
        // it's ok if there is only one candidate
        // but no candidates is pretty bad
        //Count state_count = std::min(candidates.size()-1, (size_t)max_state_count_);
        
        // here i should compare state values... not component ones
        std::sort(
            candidates.begin(), 
            candidates.end(), 
            component_candidate_comparator_);
        
        future_state_cands.clear();
        for (auto i = 0, k = 0; k < candidates.size() && i < max_state_count_; ++i, ++k) {
            StateCandidate fsc;
            auto &c = candidates[k];
            if (state_pool.empty()) {
                state_pool.push((*state_cands.back().state).clone());
            }
            fsc.state = std::move(state_pool.top());
            state_pool.pop();
            *fsc.state = *c.state;
            fsc.state->push(*c.component);
            fsc.value = std::move(c.value);
            
            // takes too much time. need to introduce priority_queoe
            
            if (find(future_state_cands.begin(), 
                     future_state_cands.end(), 
                     fsc) != future_state_cands.end()) {
                --i;
                state_pool.push(std::move(fsc.state));
            }
            else {
                future_state_cands.push_back(std::move(fsc));
            }
        }
    }
    
    void updateStateCands() {
        // can use iterator here
        // go through states and try to keep final ones
        for (auto& sc : state_cands) {
            auto& s = *sc.state;
            if (!s.isFinal()) {
                state_pool.push(std::move(sc.state));
                continue;
            }
            // now we know that state is final
            if (future_state_cands.size() < max_state_count_) {
                future_state_cands.push_back(std::move(sc));
                continue;
            }
            auto it = std::max_element(
                future_state_cands.begin(), 
                future_state_cands.end(), 
                state_candidate_comparator_);
            if (*(*it).value > *sc.value) {
                std::swap(*it, sc);
            } 
            // actually we always take state here
            state_pool.push(std::move(sc.state));
        }
        state_cands.clear();
        swap(future_state_cands, state_cands);
    }
    

    Count max_state_count_;
    ComponentCandidateLess component_candidate_comparator_;
    StateCandidateLess state_candidate_comparator_;
    std::vector<StateCandidate> state_cands;
    std::vector<ComponentCandidate> candidates;
    std::vector<StateCandidate> future_state_cands;
    std::stack<std::unique_ptr<State>> state_pool;    
};




} 

}


#endif /* defined(__Ant__opt__) */
