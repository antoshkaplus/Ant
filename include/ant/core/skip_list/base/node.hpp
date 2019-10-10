#pragma once

namespace ant::core::skip_list::base {

template <typename Value>
struct Node
{
    // number of those is equal to height
    std::vector<std::shared_ptr<Node>> next;
    T value;

    Node() {}

    Node(int h, const T& t)
            : next(h), value(t) {}

    Node(int h)
            : next(h) {}

    int height() const {
        return next.size();
    }
};

std::ostream operator<<(std::ostream& out, std::shared_ptr<Node>& node) {
    return out << "h:" << cur->height() << ",v:" << cur->value;
}

template <typename Value>
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

template <typename Value>
struct NodeIndexedReduce
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

template <typename Node>
void PrintlnSkipList(std::shared_ptr<Node>& head) {
    auto cur = head;
    out << "skip list: ";
    while (cur->next[0]) {
        cur = cur->next[0];
        out << "{" << cur << "}";
    }
    out << std::endl;
}

}