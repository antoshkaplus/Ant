#pragma once

namespace ant {

template <typename NodeType, typename ValueType>
class SkipListIteratorWrapper : public std::iterator<std::input_iterator_tag, ValueType> {
public:

    explicit SkipListIteratorWrapper(const std::shared_ptr<NodeType>& node) : node(node) {}
    SkipListIteratorWrapper() {}


    template <class OtherNodeType>
    bool operator==(const SkipListIteratorWrapper<OtherNodeType, ValueType>& iterator) {
        return node == iterator.node;
    }

    template <class OtherNodeType>
    bool operator!=(const SkipListIteratorWrapper<OtherNodeType, ValueType>& iterator) {
        return node != iterator.node;
    }

    auto& operator++() {
        node = node->next[0];
        return *this;
    }

    auto operator++(int) {
        auto temp = *this;
        node = node->next[0];
        return temp;
    }

    const ValueType& operator*() const {
        return node->value;
    }

    auto& operator*() {
        return node->value;
    }

private:
    std::shared_ptr<NodeType> node {};
};

}