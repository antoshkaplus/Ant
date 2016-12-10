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
		T opRes;
	};
	
	
    struct Node
    {
        // number of those is equal to height
        std::vector<std::shared_ptr<Node>> next;
        std::vector<Across> afterPrev;
        T value;
        
        Node() {}
        
        Node(int h, const T& t) 
            : next(h), numAfterPrev(h, 1), value(t) {}
            
        Node(int h) 
            : next(h), numAfterPrev(h, 1) {}
        
        int height() const {
            return next.size();
        }
    };    
    
    
    OpSkipList(int maxNumElems) : count(0), curHeight(0), heightGen(std::log2(maxNumElems), 0.5) {
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
    
    void insert(std::shared_ptr<Node> prev, std::shared_ptr<Node> newNode, int pos, int i) {
        newNode->next[i] = prev->next[i];
        prev->next[i] = newNode;
        newNode->numAfterPrev[i] = pos;
        newNode->next[i]->numAfterPrev[i] -= pos-1;
    }
    
    void remove(std::shared_ptr<Node> prev, std::shared_ptr<Node> cur, int i) {
        cur->next[i]->numAfterPrev[i] += cur->numAfterPrev[i] - 1;
        prev->next[i] = cur->next[i];
    }
    
	// you can't do it anymore as sequence is ordered now
    void insertAt(Index pos, const T& val) {
        pos += 1;
        
        auto height = heightGen();
        std::shared_ptr<Node> newNode = std::make_shared<Node>(height, val);
        
        if (height > curHeight) {
            for (auto i = curHeight; i < height; ++i) {
                tail->numAfterPrev[i] += count;
            }
            curHeight = height;
        }
        
        auto cur = head;
        for (auto i = curHeight-1; i >= 0; --i) {
            while (cur->next[i] != tail && cur->next[i]->numAfterPrev[i] < pos) {
                cur = cur->next[i];
                pos -= cur->numAfterPrev[i];
            }
            if (i < height) {
                insert(cur, newNode, pos, i);
            } else {
                ++cur->next[i]->numAfterPrev[i];
            }
        }
        ++count;
    }
    
	
	// inserts right after node
	// it's helper function. be careful while uisng it
	
	// nn - new node
	struct V {
		T v_1; 
		T v_2;
	}
	
	V insertBetween(Node n_1, Node n_2, int h, Node nn) {
		T v_1, v_2;
		while (cur.next.val < nn.val) {
			cur = cur.next;
			// may need to do it differently
			// maybe do it in different cycle
			//v_1 = op(v_1, cur);
		}
		// now we have element in cur right before insertion point
		next = cur.next;
		
		V vv;
		if (h == 0) {

			vv.v_1 = nn.val;
			vv.v_2 = next.val;
			
		} else {
			vv = insertBetween(cur, next, h-1, nn);
		}
		// we can initialize current level from it
		
		insert(cur, nn, next, vv);
		
		return {v_1, v_2};
	}
	
	void insert(Node cur, Node mid, Node next, V vv, int i) {
		mid->next[i] = cur->next[i];
        cur->next[i] = mid;
        mid->afterPrev[i].opRes = vv.v_1;
		next->afterPrev[i].opRes = vv.v_2;
		// we have to do the shit with position too
		// maybe have Across object and update it instead of just values.
		
		// because we won't insert by position, but by something else.
		// this num thing is actually opOperation... like count
		
		// what about having multiple operations on one tree
		
		// or somehow put it on top of each other
	}
	
	
	
	void insert(const T& val) {
		auto height = heightGen();
        std::shared_ptr<Node> newNode = std::make_shared<Node>(height, val);
        
        if (height > curHeight) {
            for (auto i = curHeight; i < height; ++i) {
				// that means we also have to keep overall opRes as global variable...
				// and we have to update on each insert and remove
                tail->afterPrev[i].num += count;
				tail->afterPrev[i].opRes = opRes; // real ???
			}
            curHeight = height;
        }
		
		auto cur = head;
        for (auto i = curHeight-1; i >= 0; --i) {
            while (cur->next[i] != tail && cur->next[i]->numAfterPrev[i] < pos) {
                cur = cur->next[i];
                pos -= cur->numAfterPrev[i];
            }
            if (i < height) {
                // and now we actually insert
				insert(cur, newNode, pos, i);
            } else {
                ++cur->next[i]->numAfterPrev[i];
            }
        }
        
		++count;
		opRes = op(opRes, val);
	}
	
	// remove is harder to imlement maybe even impossible... maybe not.
    void removeAt(Index pos) {
        pos += 1;
    
        auto cur = head;
        auto prev = head;
        for (auto i = curHeight-1; i >= 0; --i) {
            while (pos > 0) {
                prev = cur;
                cur = cur->next[i];
                pos -= cur->numAfterPrev[i];
            }
            
            if (pos == 0) {
                remove(prev, cur, i);
            } 
            pos += cur->numAfterPrev[i];
            // if cur is not deleted we have to reduce num
            --cur->numAfterPrev[i];
            cur = prev; 
        }
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
    
	
	Op op;
	
	T opRes;
    int count;
    int curHeight;
    std::shared_ptr<Node> head;
    std::shared_ptr<Node> tail;
    HeightGen heightGen;
};

}
