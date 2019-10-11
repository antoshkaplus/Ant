#pragma once

#include "node.hpp"

namespace ant::core::skip_list::base {

template <typename Value>
struct Reduce {

    struct Across {
        int num {};
        // may want another name
        Value opRes {};

        Across(int num, Value opRes)
                : num(num), opRes(opRes) {}

        Across(int num)
                : num(num) {}

        Across() : Across(0) {}

        template <typename Op>
        void add(const Across& a, Op& op) {
            if (num == 0) {
                num = a.num;
                opRes = a.opRes;
            } else {
                num += a.num;
                opRes = op(opRes, a.opRes);
            }
        }

        template <typename Op>
        void add(const Value& opRes, Op& op) {
            num += 1;
            this->opRes = op(opRes, opRes);
        }
    };

    struct Node;

    struct Cell {
        std::shared_ptr<Node> next;
        Across afterPrev;
    };

    struct Node {
        std::vector<Cell> cells;
        Value value;

        Node() {}

        Node(int h, const Value& t)
            : cells(h, Cell({}, Across{1, t})), value(t) {}

        Node(int h)
            : cells(h, Cell({}, Across{1})) {}

        int height() const {
            return cells.size();
        }

        void Reset(int i) {
            cells[i].afterPrev = Across{1, value};
        }
    };


    // will I have to try to replace too ??? probably

    // we have 2 consequitive nodes on one level and decide that new element should be inserted in between
    // so with those two nodes we descend one level down

    // n_1 can be head but is not newValue
    template <typename Op>
    static auto InsertBetween(std::shared_ptr<Node> n_1, std::shared_ptr<Node> n_2, int i,
                             Count newHeight, Value&& newVal, Op& op) {
        struct B {
            Across v_nn;
            Across v_n_2;
            std::shared_ptr<Node> node;
            bool inserted;
        };

        Across r_n_2;

        auto [nn_prev, r_n_1] = ReduceBefore(n_1, i, newVal);
        if (nn_prev != n_1 && nn_prev->val == newVal) {
            return B{{}, {}, {}, false};
        }

        auto nn_next = nn_prev->next[i];

        Println(std::cout, "nn prev");
        PrintlnDebug(nn_prev);

        Println(std::cout, "nn next");
        PrintlnDebug(nn_next);

        if (i != 0) {

            auto b = InsertBetween(nn_prev, nn_next, i-1, newHeight, newVal);
            if (!b.inserted) return B{{}, {}, {}, false};

            b.node->cells[i].afterPrev.add(b.v_nn, op);
            if (nn_next) {
                nn_next->Reset(i);
                nn_next->afterPrev[i].add(b.v_n_2, op);
            }

            r_n_1.add(b.v_nn, op);
            r_n_2.add(b.v_n_2, op);

            insert(nn_prev, b.node, i);

        } else {
//            std::make_shared();
//            insert(nn_prev, , i);
        }

        Println(std::cout, "nn next");
        PrintlnDebug(nn_next);

        auto cur = nn_next;
        while (cur != n_2) {
            r_n_2.add(cur->afterPrev[i], op);
            cur = cur->next[i];
        }
        // have to return nn and true
        return B{r_n_1, r_n_2, };
    }

    // adds up everything after start and before value
    static auto ReduceBefore(const std::shared_ptr<Node>& start, int i, const Value& value) {
        struct Result {
            std::shared_ptr<Node> node;
            Across after;
        };

        Across after {};

        auto cur = start;
        while (cur->next[i] && cur->next[i]->value <= value) {
            cur = cur->next[i];
            after.add(cur->afterPrev[i], op);
        }

        return Result{cur, after};
    }
};




}