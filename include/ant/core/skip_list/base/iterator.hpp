#pragma once

namespace ant::core::skip_list::base {

template <typename NodeType>
class IteratorWrapper : public std::iterator<std::input_iterator_tag, NodeType> {
public:

    explicit IteratorWrapper(const std::shared_ptr<NodeType>& node) : node(node) {}
    IteratorWrapper() = default;


    template <class OtherNodeType>
    bool operator==(const IteratorWrapper<OtherNodeType>& iterator) {
        return node == iterator.node;
    }

    template <class OtherNodeType>
    bool operator!=(const IteratorWrapper<OtherNodeType>& iterator) {
        return node != iterator.node;
    }

    auto& operator++() {
        node = node->next(0);
        return *this;
    }

    auto operator++(int) {
        auto temp = *this;
        node = node->next(0);
        return temp;
    }

    const NodeType& operator*() const {
        return *node;
    }

    NodeType& operator*() {
        return *node;
    }

private:
    std::shared_ptr<NodeType> node {};
};

template <typename Node>
struct Range {

    Range(const std::shared_ptr<Node>& head) : head(head) {}

    auto begin() const {
        return IteratorWrapper(head->next(0));
    }

    auto end() const {
        return IteratorWrapper<Node>();
    }

    std::shared_ptr<Node> head;
};

template <typename NodeType>
class PtrIteratorWrapper : public std::iterator<std::input_iterator_tag, NodeType> {
public:

    explicit PtrIteratorWrapper(const std::shared_ptr<NodeType>& node) : node(node) {}
    PtrIteratorWrapper() = default;


    template <class OtherNodeType>
    bool operator==(const PtrIteratorWrapper<OtherNodeType>& iterator) {
        return node == iterator.node;
    }

    template <class OtherNodeType>
    bool operator!=(const PtrIteratorWrapper<OtherNodeType>& iterator) {
        return node != iterator.node;
    }

    auto& operator++() {
        node = node->next(0);
        return *this;
    }

    auto operator++(int) {
        auto temp = *this;
        node = node->next(0);
        return temp;
    }

    const std::shared_ptr<NodeType>& operator*() const {
        return node;
    }

    std::shared_ptr<NodeType>& operator*() {
        return node;
    }

private:
    std::shared_ptr<NodeType> node {};
};

template <typename Node>
struct PtrRange {

    PtrRange(const std::shared_ptr<Node>& head) : head(head) {}

    auto begin() const {
        return PtrIteratorWrapper(head->next(0));
    }

    auto end() const {
        return PtrIteratorWrapper<Node>();
    }

    std::shared_ptr<Node> head;
};

}