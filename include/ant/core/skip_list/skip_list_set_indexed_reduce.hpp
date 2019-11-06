// source: Dr.Dobbs
// after we write one right implementation of the thing we can find ways
// to improve it for more general case. to generalize.


#pragma once

#include "ant/core/core.hpp"
#include "ant/core/skip_list/skip_list_height_gen.hpp"
#include "ant/core/skip_list/skip_list_iterator.hpp"

namespace ant {

template <class Value, class Op>
class SkipListSetIndexedReduce;

template <typename Value, typename  Op>
void Println(std::ostream& out, const SkipListSetIndexedReduce<Value, Op>& skip_list);

// consider num and opRes for something in between
// don't include bound element into the range
// that way implementation maybe simplified by a lot
// probably, we should check
// and we don't need tail in this case completely.
// just keep NULL at the end.
// should not be a problem

// Op takes in two T values and returns one
// it's something like function that you pass to reduce method
template <class Value, class Op>
class SkipListSetIndexedReduce {

    struct Across {
        int num {};
        // may want another name
        Value opRes {};

        Across(int num, Value opRes)
                : num(num), opRes(opRes) {}

        Across(int num)
                : num(num) {}

        Across() : Across(0) {}

        void add(const Across& a, Op& op) {
            if (num == 0) {
                num = a.num;
                opRes = a.opRes;
            } else {
                num += a.num;
                opRes = op(opRes, a.opRes);
            }
        }

        void add(const Value& opRes, Op& op) {
            num += 1;
            this->opRes = op(opRes, opRes);
        }
    };

    struct Node
    {
        // number of those is equal to height
        std::vector<std::shared_ptr<Node>> next;
        std::vector<Across> afterPrev;
        Value value;

        Node() {}

        Node(int h, const Value& t)
                : next(h), afterPrev(h, Across{1, t}), value(t) {}

        Node(int h)
                : next(h), afterPrev(h, Across{1}) {}

        int height() const {
            return next.size();
        }

        void Reset(int i) {
            afterPrev[i] = Across{1, value};
        }
    };

public:

    friend class SkipListIteratorWrapper<Node, Value>;
    friend class SkipListIteratorWrapper<const Node, Value>;

    friend void Println<Value, Op>(std::ostream& out, const SkipListSetIndexedReduce<Value, Op>& skip_list);

    using Iterator = SkipListIteratorWrapper<Node, Value>;
    using ConstIterator = SkipListIteratorWrapper<const Node, Value>;

    SkipListSetIndexedReduce(int maxNumElems, Op op)
        : count(0), curHeight(0), op(op) {

        int maxHeight = std::max(1, static_cast<ant::Count>(std::log2(maxNumElems)));
        heightGen = HeightGen(maxHeight, 0.5);

        head = std::make_shared<Node>(maxHeight);
    }

    ~SkipListSetIndexedReduce() {
        auto cur = head;
        while (cur) {
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
        return {};
    }

    Iterator begin() {
        return Iterator(head->next[0]);
    }

    Iterator end() {
        return {};
    }

    bool empty() const {
        return count == 0;
    }

    ant::Count size() const {
        return count;
    }

	void Insert(const Value& val) {
        if (Count(val) == 1) return;

		auto height = heightGen();
        std::shared_ptr<Node> newNode = std::make_shared<Node>(height, val);

        curHeight = std::max(curHeight, height);

        auto cur = head;
        auto i = curHeight-1;
        for (; i >= height; --i) {
            cur = ReduceBefore(cur, i, val).node;
            auto next = cur->next[i];
            if (next) {
                next->afterPrev[i].add(val, op);
            }
        }

        insertBetween(cur, {}, i, newNode);
        ++count;
	}

    void Remove(const Value& val) {
        if (Count(val) == 0) return;

        removeBetween(head, {}, curHeight-1, val);
        --count;
    }

    ant::Count Count(const Value& val) const {

        auto cur = head;
        for (auto i = curHeight-1; i >= 0; --i) {
            while (cur->next[i] && cur->next[i]->value < val) {
                cur = cur->next[i];
            }

            if (cur->next[i] && cur->next[i]->value == val) {
                return 1;
            }
        }
        return 0;
    }

    const Value& operator[](Index pos) const {
        if (pos >= count) throw std::runtime_error("out of range");

        return FindIndexed(pos+1)->value;
    }

    std::pair<ant::Index, bool> Index(const Value& value) const {
        ant::Count num = 0;

        auto cur = head;
        for (auto i = curHeight-1; i >= 0; --i) {
            while (cur->next[i] && cur->next[i]->value <= value) {
                cur = cur->next[i];
                num += cur->afterPrev[i].num;
            }

            if (cur && cur->value == value) return {num-1, true};
        }
        return {0, false};
    }

    Value Reduce(ant::Index pos, ant::Count count) const {
        if (pos+count > count) throw std::runtime_error("out of range");
        if (count == 0) return Value();

        auto node = FindIndexed(pos+1);
        auto result = node->value;

        --count;
        auto i = node->height()-1;
        while (count != 0) {
            if (node->next[i] && node->next[i]->afterPrev[i].num <= count) {
                node = node->next[i];
                count -= node->afterPrev[i].num;
                result = op(result, node->value);
                i = node->height()-1;
            } else {
                --i;
            }
        }
        return result;
    }


private:

    std::shared_ptr<Node> FindIndexed(ant::Count pos) const {

        auto cur = head;
        for (auto i = curHeight-1; i >= 0; --i) {
            while (cur->next[i] && cur->next[i]->afterPrev[i].num <= pos) {
                cur = cur->next[i];
                pos -= cur->afterPrev[i].num;
            }
        }
        return cur;
    }

    struct B {
        Across v_nn;
        Across v_n_2;
    };

    // we have 2 consequitive nodes on one level and decide that new element should be inserted in between
    // so with those two nodes we descend one level down
    B insertBetween(std::shared_ptr<Node> n_1, std::shared_ptr<Node> n_2, int i, std::shared_ptr<Node> nn) {

        Across r_n_2;

        auto [nn_prev, r_n_1] = ReduceBefore(n_1, i, nn->value);

        auto nn_next = nn_prev->next[i];

        if (i != 0) {

            auto b = insertBetween(nn_prev, nn_next, i-1, nn);
            nn->afterPrev[i].add(b.v_nn, op);
            if (nn_next) {
                nn_next->Reset(i);
                nn_next->afterPrev[i].add(b.v_n_2, op);
            }

            r_n_1.add(b.v_nn, op);
            r_n_2.add(b.v_n_2, op);
        }

        insert(nn_prev, nn, i);

        auto cur = nn_next;
        while (cur != n_2) {
            r_n_2.add(cur->afterPrev[i], op);
            cur = cur->next[i];
        }

        return B{r_n_1, r_n_2};
    }

    Across removeBetween(std::shared_ptr<Node> n_1, std::shared_ptr<Node> n_2, int i, Value val) {

        auto [cur, r_n_2] = ReduceBefore(n_1, i, val);

        if (cur->next[i] && cur->next[i]->value == val) {
            remove(cur, cur->next[i], i);
        }

        auto cur_2 = cur->next[i];
        if (cur_2 != n_2) {
            while (cur_2->next[i] != n_2) {
                cur_2 = cur_2->next[i];
                r_n_2.add(cur_2->afterPrev[i], op);
            }
        }

        if (i != 0) {
            auto r = removeBetween(cur, cur->next[i], i-1, val);
            if (cur->next[i]) {
                cur->next[i]->afterPrev[i] = r;
                cur->next[i]->afterPrev[i].add(cur->next[i]->value, op);
            }
            r_n_2.add(r, op);
        }

        return r_n_2;
    }

    void insert(std::shared_ptr<Node> prev, std::shared_ptr<Node> newNode, int i) {
        newNode->next[i] = prev->next[i];
        prev->next[i] = newNode;
    }

    void remove(std::shared_ptr<Node> prev, std::shared_ptr<Node> cur, int i) {
        prev->next[i] = cur->next[i];
    }

    // adds up everything after start and before value
    auto ReduceBefore(const std::shared_ptr<Node>& start, int i, const Value& value) {
        struct Result {
            std::shared_ptr<Node> node;
            Across after;
        };

        Across after {};

        auto cur = start;
        while (cur->next[i] && cur->next[i]->value < value) {
            cur = cur->next[i];
            after.add(cur->afterPrev[i], op);
        }

        return Result{cur, after};
    }

    void PrintlnDebug(const std::shared_ptr<Node>& node) const {
        if (!node) Println(std::cout, "node: tail");
        else if (node == head) Println(std::cout, "node: head");
        else {
            std::cout << "node:( v:" << node->value << ",h:" << node->height() << ",";
            for (auto& a : node->afterPrev) {
                std::cout << "{n:" << a.num << ",op:" << a.opRes << "}";
            }
            std::cout << ")" << std::endl;
        }
    }

	int count;
    int curHeight;
    std::shared_ptr<Node> head;
    HeightGen heightGen;
    Op op;
};

template <typename Value, typename  Op>
void Println(std::ostream& out, const SkipListSetIndexedReduce<Value, Op>& skip_list) {
    auto cur = skip_list.head;
    out << "skip list: ";
    while (cur->next[0]) {
        cur = cur->next[0];
        out << "{h:" << cur->height() << ",v:" << cur->value << "}";
    }
    out << std::endl;

    cur = skip_list.head;
    while (cur->next[0]) {
        cur = cur->next[0];
        skip_list.PrintlnDebug(cur);
    }

}

}
