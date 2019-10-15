#pragma once

#include "node.hpp"
#include "iterator.hpp"


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

        bool operator==(const Across& other) const {
            return num == other.num && opRes == other.opRes;
        }

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
            this->opRes = op(this->opRes, opRes);
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
            : cells(h, Cell{{}, Across{1, t}}), value(t) {}

        Node(int h)
            : cells(h, Cell{{}, Across{1}}) {}

        bool operator==(const Node& node) const {
            return value == node.value &&
                std::equal(cells.begin(), cells.end(),
                    node.cells.begin(), node.cells.end(),
                    [](auto& c_1, auto& c_2) {
                        return c_1.afterPrev == c_2.afterPrev &&
                            static_cast<bool>(c_1.next) == static_cast<bool>(c_2.next) &&
                            (!c_1.next || c_1.next->value == c_2.next->value);
                    });
        }

        int height() const {
            return cells.size();
        }

        const std::shared_ptr<Node>& next(int level) {
            return cells[level].next;
        }

        void Reset(int i) {
            cells[i].afterPrev = Across{1, value};
        }
    };

    template <typename Op>
    static std::shared_ptr<Node> PushBack(std::shared_ptr<Node>& head, const Value& value, const Count height, Op& op) {
        auto newNode = std::make_shared<Node>(height, value);
        PushBack(head, newNode, op);
        return newNode;
    }

    template <typename Op>
    static void PushBack(std::shared_ptr<Node>& head, const std::shared_ptr<Node>& newNode, Op& op) {

        // at first keep newNode prev inside it's  cells next
        for (auto& cell : newNode->cells) cell.next = head;

        for (auto cur : PtrRange(head)) {
            for (auto i = 0; i < std::min(cur->height(), newNode->height()); i++) {
                newNode->cells[i].next = cur;
                newNode->Reset(i);
            }
            for (auto i = cur->height(); i < newNode->height(); i++) {
                newNode->cells[i].afterPrev.add(cur->value, op);
            }
        }

        for (auto i = 0; i < newNode->height(); ++i) {
            auto& cell = newNode->cells[i];
            cell.next->cells[i].next = newNode;
            cell.next.reset();
        }
    }

    struct ReduceAfterResult {
        std::shared_ptr<Node> node;
        Across after;
    };

    // adds up everything after start and before/including value
    template <typename Op>
    static ReduceAfterResult ReduceAfter(const std::shared_ptr<Node>& start, int i, const Value& value, Op& op) {

        ReduceAfterResult result;

        auto& cur = result.node = start;
        while (cur->cells[i].next && cur->cells[i].next->value <= value) {
            cur = cur->cells[i].next;
            result.after.add(cur->cells[i].afterPrev, op);
        }

        return result;
    }

    struct InsertBetweenResult {
        Across between_1;
        Across between_2;
        std::shared_ptr<Node> node;
        bool inserted;
    };

    // n_1 can be head but is not newValue
    template <typename Op>
    static InsertBetweenResult InsertBetween(std::shared_ptr<Node> n_1, std::shared_ptr<Node> n_2, int i,
                             Value&& newVal, Count newHeight, Op& op) {

        auto InsertAfter = [](std::shared_ptr<Node>& prev, std::shared_ptr<Node>& newNode, int i) {
            newNode->cells[i].next = prev->cells[i].next;
            prev->cells[i].next = newNode;
        };

        Across r_n_2;

        auto [nn_prev, r_n_1] = ReduceAfter(n_1, i, newVal, op);
        if (nn_prev != n_1 && nn_prev->value == newVal) {
            return {{}, {}, nn_prev, false};
        }

        auto nn_next = nn_prev->cells[i].next;

        std::shared_ptr<Node> inserted_node {};
        if (i != 0) {

            auto b = InsertBetween(nn_prev, nn_next, i-1, std::move(newVal), newHeight, op);
            if (!b.inserted) return {{}, {}, b.node, false};

            inserted_node = b.node;

            b.node->cells[i].afterPrev.add(b.between_1, op);
            if (nn_next) {
                nn_next->Reset(i);
                nn_next->cells[i].afterPrev.add(b.between_2, op);
            }

            r_n_1.add(b.between_1, op);
            r_n_2.add(b.between_2, op);

            // don't want to insert all the time, only until certain level
            // have it as another function or all in one???
            InsertAfter(nn_prev, b.node, i);

        } else {
            inserted_node = std::make_shared<Node>(newHeight, std::move(newVal));
            InsertAfter(nn_prev, inserted_node, i);
        }

        auto cur = nn_next;
        while (cur != n_2) {
            r_n_2.add(cur->cells[i].afterPrev, op);
            cur = cur->cells[i].next;
        }

        return {r_n_1, r_n_2, inserted_node, true};
    }

    struct InsertBetweenLaterResult {
        std::shared_ptr<Node> node;
        bool inserted;
    };

    template <typename Op>
    static InsertBetweenLaterResult InsertBetweenLater(std::shared_ptr<Node> n_1, std::shared_ptr<Node> n_2, int i, Value&& newVal, Count newHeight, Op& op) {
        if (newHeight-1 == i) {
            auto res = InsertBetween(n_1, n_2, i, std::move(newVal), newHeight, op);
            return {std::move(res.node), res.inserted};
        }

        auto [nn_prev, r_n_1] = ReduceAfter(n_1, i, newVal, op);
        if (nn_prev != n_1 && nn_prev->value == newVal) {
            return {nn_prev, false};
        }

        auto res = InsertBetweenLater(nn_prev, nn_prev->cells[i].next, i-1, std::move(newVal), newHeight, op);
        if (res.inserted && nn_prev->cells[i].next) {
            nn_prev->cells[i].next->cells[i].afterPrev.add(res.node->value, op);
        }
        return res;
    }

    // should probably return something else
    template <typename Op>
    static auto Insert(const std::shared_ptr<Node>& head, Value newVal, Count newHeight, Op& op) {
        return InsertBetweenLater(head, {}, head->height()-1, std::move(newVal), newHeight, op);
    }

    static void Println(std::ostream& out, std::shared_ptr<Node>& head) {
        out << "SkipList{" << std::endl;
        for (auto& node : Range(head)) {
            Println(out, node);
        }
        out << "}" << std::endl;
    }

    static void Println(std::ostream& out, const typename Reduce<Value>::Cell& cell) {
        out << "[next: " << (cell.next ? std::to_string(cell.next->value) : "");
        Print(out, cell.afterPrev);
        out << "]" << std::endl;
    }

    static void Println(std::ostream& out, const typename Reduce<Value>::Node& node) {
        out << "Node{" << std::endl;
        out << "[v:" << node.value << "h:" << node.height() << "]"  << std::endl;
        for (auto& cell : node.cells) {
            Println(out, cell);
        }
        out << "}" << std::endl;
    }

    static void Print(std::ostream& out, const Across& across) {
        out << "[afterPrev: num: " << across.num << " op: " << across.opRes << "]";
    }

    static void Println(std::ostream& out, const ReduceAfterResult& result) {
        out << "{node: " << (result.node ? std::to_string(result.node->value) : "") << " ";
        Print(out, result.after);
        out << "}" << std::endl;
    }
};

}