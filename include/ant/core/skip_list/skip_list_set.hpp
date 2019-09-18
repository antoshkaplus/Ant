#pragma once

#include "ant/core/core.hpp"
#include "ant/core/skip_list/skip_list_height_gen.hpp"

namespace ant {

// lets implement set like skip list

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


    explicit SkipListSet(int maxNumElems) : count(0), curHeight(0) {

        int maxHeight = std::max(1, static_cast<ant::Count>(std::log2(maxNumElems)));
        heightGen = HeightGen(maxHeight, 0.5);

        head = std::make_shared<Node>(maxHeight);
    }

    ~SkipListSet() {
        auto cur = head;
        // tail items are empty
        while (cur) {
            auto next = cur->next[0];
            for (auto& sh : cur->next) {
                sh.reset();
            }
            cur = next;
        }

    }

    bool empty() const {
        return true;
    }

    ant::Count size() const {
        return 1;
    }

	// we don't check if already exists or not
    void Insert(const T& val) {

        auto height = heightGen();
        std::shared_ptr<Node> newNode = std::make_shared<Node>(height, val);

		curHeight = std::max(curHeight, height);

		// TODO consider equal keys

        auto cur = head;
        for (auto i = curHeight-1; i >= 0; --i) {
			while (cur->next[i] && cur->next[i]->value < val) {
                cur = cur->next[i];
            }
            if (i < height) {
                insert(cur, newNode, i);
            }
        }
        ++count;
    }

    void Remove(const T& val) {

        auto cur = head;
        for (auto i = curHeight-1; i >= 0; --i) {
            while (cur->next[i] && cur->next[i]->value < val) {
                cur = cur->next[i];
            }

			if (cur->next[i]->value == val) {
                remove(cur, cur->next[i], i);
            }
        }
        --count;
    }

    ant::Count Count(const T& val) const {

        auto cur = head;
        for (auto i = curHeight-1; i >= 0; --i) {
            while (cur->next[i] && cur->next[i]->value < val) {
                cur = cur->next[i];
            }

            if (cur->next[i]->value == val) {
                return cur->next[i]->value;
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

private:

    auto FindPrev(auto& val) {
        auto cur = head;
        for (auto i = curHeight-1; i >= 0; --i) {
            while (cur->next[i] && cur->next[i]->value < val) {
                cur = cur->next[i];
            }
        }
        return cur;
    }

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
