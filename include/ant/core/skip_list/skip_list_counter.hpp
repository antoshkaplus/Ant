// source: Dr.Dobbs
// after we write one right implementation of the thing we can find ways
// to improve it for more general case. to generalize.


#pragma once

#include "ant/core/core.hpp"
#include "ant/core/skip_list/skip_list_height_gen.hpp"

namespace ant {

template <class T>
class CounterSkipList {
    
public:
    struct Node
    {
        // number of those is equal to height
        std::vector<std::shared_ptr<Node>> next;
        std::vector<Count> numAfterPrev;
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
    
    
    CounterSkipList(int maxNumElems) : count(0), curHeight(0), heightGen(std::log2(maxNumElems), 0.5) {
        int maxHeight = std::log2(maxNumElems);
        
        head = std::make_shared<Node>(maxHeight);
        tail = std::make_shared<Node>(maxHeight);
    
        for (Index i = 0; i < maxHeight; ++i) {
            head->next[i] = tail;
        }
    }
    
    ~CounterSkipList() {
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
    
    int count;
    int curHeight;
    std::shared_ptr<Node> head;
    std::shared_ptr<Node> tail;
    HeightGen heightGen;
};

}
