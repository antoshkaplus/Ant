// source: Dr.Dobbs
// after we write one right implementation of the thing we can find ways
// to improve it for more general case. to generalize.


#pragma once

#include "ant/core/core.hpp"

namespace ant {

// Op takes in two T values and returns one
// it's something like function that you pass to reduce method
template <class T, class Op>
class OpSkipList {
    
public:

    // this one is usual
    struct HeightGen {
        HeightGen() {}
    
        HeightGen(int max, float prob) {
            std::vector<double> probs(max);
            for (auto& p : probs) {
                p = prob;
                prob *= 0.5;
            }
            distr = std::discrete_distribution<int>(probs.begin(), probs.end());
        }
        ~HeightGen() {}
        
        int operator()() {
            return distr(rng) + 1;
        }
        
    private:
        std::default_random_engine rng;
        std::discrete_distribution<int> distr;
    };
    
    
	struct Across {
		int num;
        // may want another name
		T opRes;
        
        Across(int num, T opRes) 
            : num(num), opRes(opRes) {}
            
        Across(int num)
            : num(num) {}
        
        Across() : Across(0) {}
        
        void add(const Across& a, Op op) {
            if (num == 0) {
                num = a.num;
                opRes = opRes;
            } else {
                num += a.num;
                opRes = op(opRes, a.opRes);
            }
        }
	};
	
	
    struct Node
    {
        // number of those is equal to height
        std::vector<std::shared_ptr<Node>> next;
        std::vector<Across> afterPrev;
        T value;
        
        Node() {}
        
        Node(int h, const T& t) 
            : next(h), afterPrev(h, Across{t, 1}), value(t) {}
            
        Node(int h) 
            : next(h), afterPrev(h, Across{1}) {}
        
        int height() const {
            return next.size();
        }
    };    
    
    
    OpSkipList(int maxNumElems, Op op) 
        : count(0), curHeight(0), heightGen(std::log2(maxNumElems), 0.5), op(op) {
        
        int maxHeight = std::log2(maxNumElems);
        
        head = std::make_shared<Node>(maxHeight);
        tail = std::make_shared<Node>(maxHeight);
    
        for (Index i = 0; i < maxHeight; ++i) {
            head->next[i] = tail;
        }
    }
    
    ~OpSkipList() {
        auto cur = head;
        // tail items are empty
        while (cur != tail) {
            auto next = cur->next[0];
            for (auto& sh : cur->next) {
                sh.reset();
            }
            cur = next;
        }
    
    }

    struct B {
        Across v_nn;
        Across v_n_2;
        
        B() {}
        
        B(T r_nn, T r_n_2) 
            : v_nn(r_nn), v_n_2(r_n_2) {}
    };
    
    // we have 2 consequitive nodes on one level and decide that new element should be inserted in between
    // so with those two nodes we descend one level down
	B insertBetween(std::shared_ptr<Node> n_1, std::shared_ptr<Node> n_2, int i, std::shared_ptr<Node> nn) {
		
        Across r_nn, r_n_2;

        // we look for the next pair of nodes 
        auto cur = n_1;
        // after insertion nn and n_2 on upper level would need opRes new values 
        while (cur->next[i] != tail && cur->next[i]->value < nn->value) {
			cur = cur->next[i];
			// have to pur cur value under opRes
            r_nn.add(cur->afterPrev[i], op);
        }
        // now next cur is greater so it would be new next
        
        
		// now we have element in cur right before insertion point
		auto cur_2 = cur->next[i];
        while (cur_2 != n_2) {
            cur_2 = cur_2->next[i];
            // tail does not have op value
            // we just have to skip using result of it?
            // that;s why we keep overall count and op separately
            // we put there something but we never use it.
            r_n_2.add(cur_2->afterPrev[i], op);
        }
		
        if (i == 0) {
            // we can't continue to descend. we should insert value here and plan to go on upper levels
            r_nn->add({1, nn->value}, op);
        } else {
            auto b = insertBetween(cur, cur->next[i], i-1, nn);
            r_nn.add(b.v_nn, op);
            r_n_2.add(b.v_n_2, op);
        }
		
        if (i < nn->height) {
        // no good // should not be here
            nn->next[i] = cur->next[i];
            nn->afterPrev[i] = r_nn;
            nn->next[i]->afterPrev[i] = r_n_2; 
        } else {
            cur->next[i]->afterPrev[i].add({1, nn->value}, op);
        }
		
		return B{r_nn.val, r_n_2.val};
	}
	
	void insert(const T& val) {
		auto height = heightGen();
        std::shared_ptr<Node> newNode = std::make_shared<Node>(height, val);
        
        if (height > curHeight) {
            for (auto i = curHeight; i < height; ++i) {
				// that means we also have to keep overall opRes as global variable...
				// and we have to update on each insert and remove
                // we do only this one because of by position insert
                tail->afterPrev[i].num += count;
			}
            curHeight = height;
        }
		
        insertBetween(head, tail, curHeight-1, newNode);
 		
        ++count;
		// this one gets complicated really fast
        // opRes = op(opRes, val);
	}
	
    
    // we care only about n_2 update on remove
    Across removeBetween(std::shared_ptr<Node> n_1, std::shared_ptr<Node> n_2, int i, T val) {
        Across r_n_2;
        
        auto cur = n_1;
        while (cur->next[i] != tail && cur->next[i]->value < val) {
            cur = cur->next[i];
            r_n_2.add(cur->afterPrev[i], op);
        }
    
        auto cur_2 = cur->next[i];
        while (cur_2 != n_2) {
            cur_2 = cur_2->next[i];
            r_n_2.add(cur_2->afterPrev[i], op);
        }
        
        if (i == 0) {
        } else {
            auto r = insertBetween(cur, cur->next[i], i-1, val);
            // here put at the end
            r_n_2.add(r, op);
        }
        
        if (val == cur->next[i]->val) {
            cur->next[i] = cur->next[i]->next[i];
        }
        cur->next[i]->afterPrev[i] = r_n_2;
        
        return r_n_2;
    }
    
    
    
    // when we do remove we have to recompute opRes
    // similar to insert but ending actions are different
    void remove(const T& val) {
        
        removeBetween(head, tail, curHeight-1, val);
        --count;
    }
    
            
    const T& operator[](Index pos) const {
        pos += 1;
        
        auto cur = head;
        for (auto i = curHeight-1; i >= 0; --i) {
            while (cur->next[i] != tail && cur->next[i]->numAfterPrev[i] <= pos) {
                cur = cur->next[i];
                pos -= cur->numAfterPrev[i];
            }
            if (pos == 0) {
                return cur->value;
            } 
        }
        throw std::runtime_error("out of range");
    }
    
    T query(Index pos_1, Index pos_2) const {
        return queryMiddle(head, tail, pos_1+1, pos_2+1, curHeight-1);
    }
	
    // i is like height
    T queryMiddle(std::shared_ptr<Node> n_1, std::shared_ptr<Node> n_2, Index pos_1, Index pos_2, Index i) const {
        auto cur = head;
        while (cur->next[i] != tail && cur->next[i]->afterPrev[i].num < pos_1) {
            cur = cur->next[i];
            pos_1 -= cur->afterPrev[i].num;
            pos_2 -= cur->afterPrev[i].num;
        }
		
		// pos_1 is after cur
		
        // we found left guy
		auto p_2 = pos_2;
        auto cur_2 = cur;
        
		T rr;
		while (cur_2->next[i] != tail && cur_2->next[i]->afterPrev[i].num < pos_2) {
            cur_2 = cur_2->next[i];
			p_2 -= cur->afterPrev[i].num; // ???
			
        }
		
		T val;
		if (cur == cur_2) {
			val = queryMiddle(cur, cur->next[i], pos_1, pos_2, i-1);
		} else {
			
			// we could get left with level high ??? 
			// but this high level exists... that's the question
			// this implementation looks more clear to me
			/*
            val = op(queryLeft(cur, cur->next[i], pos_1, i-1),
				reduce(cur->next[i], cur_2, i-1),
				queryRight(cur_2, cur_2->next[i], p_2, i-1));
			
             */
        }
		return val;
    }
    
    T reduce(std::shared_ptr<Node> n_1, std::shared_ptr<Node>& n_2, int i) const {
		if (n_1 == n_2) {
			return;  // nothing;
		}
		T res;
		for (;;) {
			n_1 = n_1->next[i];
			res = op(n_1->val, res);
			if (n_1 == n_2) break;
		}
		return res;
	}
	
	
    // take everything from left node
    T queryRight(std::shared_ptr<Node> n_1, std::shared_ptr<Node> n_2, Index pos, Index i) const {
		auto cur = n_1;
        while (cur->next[i] != tail && cur->next[i]->afterPrev[i].num < pos) {
            cur = cur->next[i];
            pos -= cur->afterPrev[i].num;
        }
		// should we really take n_1 into account
		// we don't touch anything else
		return reduce(n_1->next[i], cur->next[i]) + queryRight(cur, cur->next[i], pos, i-1);
    }
    
    
    T queryLeft(std::shared_ptr<Node> n_1, std::shared_ptr<Node> n_2, Index pos, Index i) const {
        auto cur = n_1;
        while (cur->next[i] != tail && cur->next[i]->afterPrev[i].num < pos) {
            cur = cur->next[i];
            pos -= cur->afterPrev[i].num;
        }
		return queryLeft(cur, cur->next[i], pos, i-1) + reduce(cur->next[i], n_2, i-1);
    }
	
	// i don't know about having both those two values.
    // opRes seems not needed at all
	T opRes;
    int count;
    
    
    int curHeight;
    std::shared_ptr<Node> head;
    std::shared_ptr<Node> tail;
    HeightGen heightGen;
    Op op;

};

}
