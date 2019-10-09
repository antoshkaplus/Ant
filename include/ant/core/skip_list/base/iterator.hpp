#pragma once

namespace ant::core::skip_list::base {

template <typename NodeType, typename ValueType>
class IteratorWrapper : public std::iterator<std::input_iterator_tag, ValueType> {
public:

    explicit IteratorWrapper(const std::shared_ptr<NodeType>& node) : node(node) {}
    IteratorWrapper() {}


    template <class OtherNodeType>
    bool operator==(const IteratorWrapper<OtherNodeType, ValueType>& iterator) {
        return node == iterator.node;
    }

    template <class OtherNodeType>
    bool operator!=(const IteratorWrapper<OtherNodeType, ValueType>& iterator) {
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

private:
    std::shared_ptr<NodeType> node {};
};

}