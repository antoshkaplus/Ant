#pragma once

namespace ant::skip_list {

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

template <class Value, class Op>
class SetIndexedReduceBase {

    struct B {
        Across v_nn;
        Across v_n_2;
    };

public:
    SetIndexedReduceBase(ant::Count max_height, Op op) : op(std::move(op)) {

        head = std::make_shared<Node>(maxHeight);
    }

    void Insert(const Value& val) {

        auto height = heightGen();
        std::shared_ptr<Node> newNode = std::make_shared<Node>(height, val);

        curHeight = std::max(curHeight, height);

        Println(std::cout, *this);

        auto cur = head;
        auto i = curHeight-1;
        for (; i >= height; --i) {
            cur = ReduceBefore(cur, i, val).node;
            auto next = cur->next[i];
            if (next) {
                next->afterPrev[i].add(val, op);
            }
        }

        Println(std::cout, "insert at");
        PrintlnDebug(cur);

        insertBetween(cur, {}, i, newNode);
        ++count;

        Println(std::cout, *this);
    }

    // we have 2 consequitive nodes on one level and decide that new element should be inserted in between
    // so with those two nodes we descend one level down
    B insertBetween(std::shared_ptr<Node> n_1, std::shared_ptr<Node> n_2, int i, std::shared_ptr<Node> nn) {

        Println(std::cout, i);

        Across r_n_2;

        auto [nn_prev, r_n_1] = ReduceBefore(n_1, i, nn->value);

        auto nn_next = nn_prev->next[i];

        Println(std::cout, "nn prev");
        PrintlnDebug(nn_prev);

        Println(std::cout, "nn next");
        PrintlnDebug(nn_next);

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

        Println(std::cout, "nn");
        PrintlnDebug(nn);

        Println(std::cout, "nn next");
        PrintlnDebug(nn_next);

        auto cur = nn_next;
        while (cur != n_2) {
            r_n_2.add(cur->afterPrev[i], op);
            cur = cur->next[i];
        }

        return B{r_n_1, r_n_2};
    }

    void insert(std::shared_ptr<Node> prev, std::shared_ptr<Node> newNode, int i) {
        newNode->next[i] = prev->next[i];
        prev->next[i] = newNode;
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

private:
    std::shared_ptr<Node> head;
    Op op;
};

}
