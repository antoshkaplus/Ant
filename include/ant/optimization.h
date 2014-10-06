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


namespace ant { 

namespace opt {

 
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
        
        // maybe think of some kind of hash function // with Index type or something like that 
    };
    
    
    // maybe add variable length width beam possibility
    std::unique_ptr<State> solve(const State& initial_state, Count max_state_count) {
        max_state_count_ = max_state_count;
        state_cands.emplace_back(std::move(initial_state.clone()), std::move(initial_state.value()));
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
    struct ComponentCandidate {
        ComponentCandidate(State* state, 
                           std::unique_ptr<Component> component, 
                           std::unique_ptr<Value> value) 
            :   state(state), 
                component(std::move(component)), 
                value(std::move(value)) {}
        // original state
        State* state;
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
            auto& s = *s_c.state;
            // should have any candidate if not in final state
            if (s.isFinal()) continue;
            // user should know by himself how many candidates to 
            // produce.. i will process all of them
            for (auto& c : s.candidateComponents()) {
                s.push(*c);
                value = s.value();
                s.pop();
                candidates.emplace_back(s_c.state.get(), std::move(c), std::move(value));
            }
        }
    }
    
    void initFutureStateCands() {
        // it's ok if there is only one candidate
        // but no candidates is pretty bad
        Count state_count = std::min(candidates.size()-1, (size_t)max_state_count_);
        
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
                state_pool.push(std::move((*state_cands.back().state).clone()));
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
