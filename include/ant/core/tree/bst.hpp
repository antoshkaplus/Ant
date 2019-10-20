#pragma once

#include "ant/core/core.hpp"

namespace ant::core::tree {

    template <typename Node>
    struct BST {

        using UN = std::unique_ptr<Node>;

        static Node* Prev(Node* n) {
            if (Exists(Left(n))) {
                n = Left(n);
                while (Exists(Right(n))) {
                    n = Right(n);
                }
                return n;
            }
            auto n_2 = Parent(n);
            if (!Exists(n_2)) return nullptr;

            if (Right(n_2) == n) {
                return n_2;
            } else {
                // n right child
                // will return nullptr if can't find anything
                while (Exists(n_2) && Right(n_2) != n) {
                    n = n_2;
                    n_2 = Parent(n_2);
                }
                return n_2;
            }
        }

        static Node* Next(Node* n) {
            if (Exists(Right(n))) {
                n = Right(n);
                while (Exists(Left(n))) {
                    n = Left(n);
                }
                return n;
            }
            auto n_2 = Parent(n);
            if (!Exists(n_2)) return nullptr;

            if (Left(n_2) == n) {
                return n_2;
            } else {
                // n left child
                // will return nullptr if can't find anything
                while (Exists(n_2) && Left(n_2) != n) {
                    n = n_2;
                    n_2 = Parent(n_2);
                }
                return n_2;
            }
        }

        // min element in subtree
        static Node* Min(Node* n) {
            while (Exists(Left(n))) {
                n = Left(n);
            }
            return n;
        }

        static Node* Max(Node* n) {
            while (Exists(Right(n))) {
                n = Right(n);
            }
            return n;
        }

        static bool Exists(const Node* n) {
            return n != nullptr;
        }

        static ant::Count Size(const Node* n) {
            if (n == nullptr) return 0;
            return 1 + Size(Right(n)) + Size(Left(n));
        }
    };

}