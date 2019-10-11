#pragma once

#include "ant/core/core.hpp"

namespace ant::core::skip_list::base {

template <typename Value>
struct Node
{
    // number of those is equal to height
    std::vector<std::shared_ptr<Node>> next;
    Value value;

    Node() {}

    Node(int h, const Value& t)
            : next(h), value(t) {}

    Node(int h)
            : next(h) {}

    int height() const {
        return next.size();
    }
};

template <typename Value>
std::ostream operator<<(std::ostream& out, std::shared_ptr<Node<Value>>& node) {
    return out << "h:" << node->height() << ",v:" << node->value;
}

template <typename Node>
void PrintlnSkipList(std::ostream& out, std::shared_ptr<Node>& head) {
    auto cur = head;
    out << "skip list: ";
    while (cur->next[0]) {
        cur = cur->next[0];
        out << "{" << cur << "}";
    }
    out << std::endl;
}

}