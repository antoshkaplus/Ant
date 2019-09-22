#pragma once

#include <iterator>

#include "ant/core/core.hpp"
#include "ant/core/skip_list/skip_list_height_gen.hpp"


namespace ant {

// benchmark shows that losing 2 - 5 times to just set
// on insertions / deletions

// traversal is also about 2 times slower, lookup as well on big sets
// it's probably due to use of shared pointer

// web research shows that people usually use SkipList for concurrency
// the point though is that it's deterministic data structure and rely
// only on amortized time

template <class T>
class SkipListSet {

public:

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

private:
    template <typename NodeType>
    class IteratorWrapper : public std::iterator<std::input_iterator_tag, T> {
    public:

        explicit IteratorWrapper(const std::shared_ptr<NodeType>& node) : node(node) {}
        IteratorWrapper() {}


        template <class OtherNodeType>
        bool operator==(const IteratorWrapper<OtherNodeType>& iterator) {
            return node == iterator.node;
        }

        template <class OtherNodeType>
        bool operator!=(const IteratorWrapper<OtherNodeType>& iterator) {
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

        const T& operator*() const {
            return node->value;
        }

    private:
        std::shared_ptr<NodeType> node {};
    };

public:
    using Iterator = IteratorWrapper<Node>;
    using ConstIterator = IteratorWrapper<const Node>;


    explicit SkipListSet(int maxNumElems) : count(0), curHeight(0) {

        int maxHeight = std::max(1, static_cast<ant::Count>(std::log2(maxNumElems)));
        heightGen = HeightGen(maxHeight, 0.5);

        head = std::make_shared<Node>(maxHeight);
    }

    ~SkipListSet() {
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
        return size() == 0;
    }

    ant::Count size() const {
        return count;
    }

	// we don't check if already exists or not
    void Insert(const T& val) {

        auto height = heightGen();
        std::shared_ptr<Node> newNode = std::make_shared<Node>(height, val);

		curHeight = std::max(curHeight, height);

		// TODO consider equal keys

        auto cur = head;
        for (auto i = curHeight-1; i > 0; --i) {
			while (cur->next[i] && cur->next[i]->value < val) {
                cur = cur->next[i];
            }

            if (cur->next[i] && cur->next[i]->value == val) {
                remove(cur, cur->next[i], i);
            }

            if (i < height) {
                insert(cur, newNode, i);
            }
        }

        while (cur->next[0] && cur->next[0]->value < val) {
            cur = cur->next[0];
        }

        if (cur->next[0] && cur->next[0]->value == val) {
            remove(cur, cur->next[0], 0);
            --count;
        }

        insert(cur, newNode, 0);
        ++count;
    }

    void Remove(const T& val) {

        auto cur = head;
        for (auto i = curHeight-1; i > 0; --i) {
            while (cur->next[i] && cur->next[i]->value < val) {
                cur = cur->next[i];
            }

			if (cur->next[i] && cur->next[i]->value == val) {
                remove(cur, cur->next[i], i);
			}
        }

        while (cur->next[0] && cur->next[0]->value < val) {
            cur = cur->next[0];
        }

        if (cur->next[0] && cur->next[0]->value == val) {
            remove(cur, cur->next[0], 0);
            --count;
        }
    }

    void Erase(const T& val) {
        Remove(val);
    }

    ant::Count Count(const T& val) const {

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

	// we should terminate if not found
    const T& operator[](const T& val) const {

        auto cur = head;
        for (auto i = curHeight-1; i >= 0; --i) {
            while (cur->next[i] && cur->next[i]->value < val) {
                cur = cur->next[i];
            }

            if (cur->next[i]->value == val) {
                return cur->next[i]->value;
            }
        }
        throw std::runtime_error("out of range");
    }

	T* at(const T val) const {

		auto cur = head;
        for (auto i = curHeight-1; i >= 0; --i) {
            while (cur->next[i] && cur->next[i]->value < val) {
                cur = cur->next[i];
            }

            if (cur->next[i]->value == val) {
                return &cur->next[i]->value;
            }
        }
		return nullptr;
	}

	template <typename Func>
	void ForEach(Func&& func) {
        auto cur = head;
        while (cur->next[0]) {
            func(cur->next[0]->value);
            cur = cur->next[0];
        }
    }



private:

    void insert(std::shared_ptr<Node> prev, std::shared_ptr<Node> newNode, int i) {
        newNode->next[i] = prev->next[i];
        prev->next[i] = newNode;
    }

    void remove(std::shared_ptr<Node> prev, std::shared_ptr<Node> cur, int i) {
        prev->next[i] = cur->next[i];
    }

    int count;
    int curHeight;
    std::shared_ptr<Node> head;
    HeightGen heightGen;
};

}
