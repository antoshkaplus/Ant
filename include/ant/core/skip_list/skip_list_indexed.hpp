// source: Dr.Dobbs
// after we write one right implementation of the thing we can find ways
// to improve it for more general case. to generalize.


#pragma once

#include "ant/core/core.hpp"
#include "ant/core/skip_list/skip_list_height_gen.hpp"
#include "ant/core/skip_list/skip_list_iterator.hpp"

namespace ant {

template <class T>
class SkipListIndexed {
    
public:
    struct Node
    {
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

    friend class SkipListIteratorWrapper<Node, T>;
    friend class SkipListIteratorWrapper<const Node, T>;

    using Iterator = SkipListIteratorWrapper<Node, T>;
    using ConstIterator = SkipListIteratorWrapper<const Node, T>;


    explicit SkipListIndexed(int maxNumElems) : count(0), curHeight(0) {

        int maxHeight = std::max(1, static_cast<ant::Count>(std::log2(maxNumElems)));
        heightGen = HeightGen(maxHeight, 0.5);

        head = std::make_shared<Node>(maxHeight);
        tail = std::make_shared<Node>(maxHeight);

        for (Index i = 0; i < maxHeight; ++i) {
            head->next[i] = tail;
        }
    }
    
    ~SkipListIndexed() {
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

    ConstIterator begin() const {
        return ConstIterator{std::const_pointer_cast<const Node>(head->next[0])};
    }

    ConstIterator end() const {
        return ConstIterator{std::const_pointer_cast<const Node>(tail)};
    }

    Iterator begin() {
        return Iterator(head->next[0]);
    }

    Iterator end() {
        return Iterator(tail);
    }

    bool empty() const {
        return count == 0;
    }

    ant::Count size() const {
        return count;
    }

    void InsertAt(Index pos, const T& val) {
        assert(pos > count);

        pos += 1;
        
        auto height = heightGen();
        std::shared_ptr<Node> newNode = std::make_shared<Node>(height, val);

        curHeight = std::max(curHeight, height);
        
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

    void RemoveAt(Index pos) {
        assert(pos >= count);

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
private:
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

    int count;
    int curHeight;
    std::shared_ptr<Node> head;
    std::shared_ptr<Node> tail;
    HeightGen heightGen;
};

}
