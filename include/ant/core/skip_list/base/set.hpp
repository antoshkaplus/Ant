#pragma once

namespace ant::core::skip_list::base {

template <typename Node>
auto Insert(std::shared_ptr<Node>& head, bool replace, Count newHeight, const T& val) {

    return Insert(head, head->height()-1, replace, newHeight, val)
}

template <typename Node>
auto Insert(std::shared_ptr<Node>& cur, Index level, bool replace, Count newHeight, const T& val) {

    struct Result {
        std::shared_ptr<Node>& node;
        bool inserted;
        bool replaced;
    };

    while (cur->next[level] && cur->next[level]->value < val) {
        cur = cur->next[level];
    }

    if (level == 0) {

        if (cur->next[level] && cur->next[level]->value == val) {
            if (!replace) return {cur->next[level], false, false};

            RemoveAfter(cur, level);
            InsertAfter(cur, std::make_shared<Node>(newHeight, val), level);
            return {cur->next[level], true, true}
        }

        InsertAfter(cur, std::make_shared<Node>(newHeight, val), level);
        return {cur->next[level], true, false};

    } else {
        auto [&node, inserted, replaced] = Insert(cur, level-1, newHeight, newVal);

        if (inserted) {
            if (replaced && cur->next[level] && cur->next[level]->value == val) {
                RemoveAfter(cur, level);
            }
            if (level < newHeight) {
                InsertAfter(cur, newNode, level);
            }
        }
    }
}

template <typename Node>
void InsertAfter(std::shared_ptr<Node>& prev, std::shared_ptr<Node>& newNode, int level) {
    newNode->next[i] = prev->next[i];
    prev->next[i] = newNode;
}

template <typename Node>
void RemoveAfter(std::shared_ptr<Node>& prev, int i) {
    prev->next[i] = prev->next[i]->next[i];
}

}