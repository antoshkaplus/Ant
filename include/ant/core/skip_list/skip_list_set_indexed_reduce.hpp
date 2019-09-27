// source: Dr.Dobbs
// after we write one right implementation of the thing we can find ways
// to improve it for more general case. to generalize.


#pragma once

#include "ant/core/core.hpp"
#include "ant/core/skip_list/skip_list_height_gen.hpp"
#include "ant/core/skip_list/skip_list_iterator.hpp"

namespace ant {

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
        int num;
        // may want another name
        Value opRes;

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

    using Iterator = SkipListIteratorWrapper<Node, Value>;
    using ConstIterator = SkipListIteratorWrapper<const Node, Value>;

    SkipListSetIndexedReduce(int maxNumElems, Op op)
        : count(0), curHeight(0), heightGen(std::log2(maxNumElems), 0.5), op(op) {

        int maxHeight = std::log2(maxNumElems);

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

        insertBetween(head, {}, curHeight-1, newNode);
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
        pos += 1;

        auto cur = head;
        for (auto i = curHeight-1; i >= 0; --i) {
            while (cur->next[i] && cur->next[i]->numAfterPrev[i] <= pos) {
                cur = cur->next[i];
                pos -= cur->numAfterPrev[i];
            }
            if (pos == 0) {
                return cur->value;
            }
        }
        throw std::runtime_error("out of range");
    }

    Value Query(Index pos_1, Index pos_2) const {
        return queryMiddle(head, {}, pos_1+1, pos_2+1, curHeight-1);
    }

private:

    struct B {
        Across v_nn;
        Across v_n_2;
    };

    // we have 2 consequitive nodes on one level and decide that new element should be inserted in between
    // so with those two nodes we descend one level down
    B insertBetween(std::shared_ptr<Node> n_1, std::shared_ptr<Node> n_2, int i, std::shared_ptr<Node> nn) {

        Across r_nn, r_n_2;

        auto nn_prev = n_1;
        while (nn_prev->next[i] && nn_prev->next[i]->value < nn->value) {
            nn_prev = nn_prev->next[i];
            r_nn.add(nn_prev->afterPrev[i], op);
        }

        auto cur = nn_prev->next[i];
        while (cur != n_2) {
            r_n_2.add(cur->afterPrev[i], op);
            cur = cur->next[i];
        }

        if (i != 0) {

            auto b = insertBetween(nn_prev, nn_prev->next[i], i-1, nn);
            r_nn.add(b.v_nn, op);
            r_n_2.add(b.v_n_2, op);
        }

        insert(nn_prev, nn, i);
        if (n_2) {
            n_2->Reset(i);
            n_2->afterPrev[i].add(r_n_2, op);
        }

        return B{r_nn, r_n_2};
    }

    // we care only about n_2 update on remove
    Across removeBetween(std::shared_ptr<Node> n_1, std::shared_ptr<Node> n_2, int i, Value val) {

        Across r_n_2;

        auto cur = n_1;
        while (cur->next[i] != n_2 && cur->next[i]->value < val) {
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

        if (val == cur->next[i]->value) {
            cur->next[i] = cur->next[i]->next[i];
        }
        cur->next[i]->afterPrev[i] = r_n_2;

        return r_n_2;
    }

    // i is like height
    Value queryMiddle(std::shared_ptr<Node> n_1, std::shared_ptr<Node> n_2, Index pos_1, Index pos_2, Index i) const {
        auto cur = head;
        while (cur->next[i] && cur->next[i]->afterPrev[i].num < pos_1) {
            cur = cur->next[i];
            pos_1 -= cur->afterPrev[i].num;
            pos_2 -= cur->afterPrev[i].num;
        }         auto cur = nn_prev->next[i];
            while (cur != n_2) {
                r_n_2.add(cur->afterPrev[i], op);
                cur = cur->next[i];
            }


		// pos_1 is after cur

        // we found left guy
		auto p_2 = pos_2;
        auto cur_2 = cur;

		Value rr;
		while (cur_2->next[i] && cur_2->next[i]->afterPrev[i].num < pos_2) {
            cur_2 = cur_2->next[i];
			p_2 -= cur->afterPrev[i].num; // ???

        }

		Value val;
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

    Value reduce(std::shared_ptr<Node> n_1, std::shared_ptr<Node>& n_2, int i) const {
		if (n_1 == n_2) {
			return;  // nothing;
		}
		Value res;
		for (;;) {
			n_1 = n_1->next[i];
			res = op(n_1->val, res);
			if (n_1 == n_2) break;
		}
		return res;
	}


    // take everything from left node
    Value queryRight(std::shared_ptr<Node> n_1, std::shared_ptr<Node> n_2, Index pos, Index i) const {
		auto cur = n_1;
        while (cur->next[i] && cur->next[i]->afterPrev[i].num < pos) {
            cur = cur->next[i];
            pos -= cur->afterPrev[i].num;
        }
		// should we really take n_1 into account
		// we don't touch anything else
		return reduce(n_1->next[i], cur->next[i]) + queryRight(cur, cur->next[i], pos, i-1);
    }


    Value queryLeft(std::shared_ptr<Node> n_1, std::shared_ptr<Node> n_2, Index pos, Index i) const {
        auto cur = n_1;
        while (cur->next[i] && cur->next[i]->afterPrev[i].num < pos) {
            cur = cur->next[i];
            pos -= cur->afterPrev[i].num;
        }
		return queryLeft(cur, cur->next[i], pos, i-1) + reduce(cur->next[i], n_2, i-1);
    }

    void insert(std::shared_ptr<Node> prev, std::shared_ptr<Node> newNode, int i) {
        newNode->next[i] = prev->next[i];
        prev->next[i] = newNode;
    }

	int count;
    int curHeight;
    std::shared_ptr<Node> head;
    HeightGen heightGen;
    Op op;
};

}
