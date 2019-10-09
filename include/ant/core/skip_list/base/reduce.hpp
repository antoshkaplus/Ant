#pragma once

namespace ant::core::skip_list::base {

// will I have to try to replace too ??? probably

// we have 2 consequitive nodes on one level and decide that new element should be inserted in between
// so with those two nodes we descend one level down
auto ReduceInsertBetween(std::shared_ptr<Node> n_1, std::shared_ptr<Node> n_2, int i, std::shared_ptr<Node> nn) {
    struct B {
        Across v_nn;
        Across v_n_2;
    };

    Println(std::cout, i);

    Across r_n_2;

    auto [nn_prev, r_n_1] = ReduceBefore(n_1, i, nn->value);

    auto nn_next = nn_prev->next[i];

    Println(std::cout, "nn prev");
    PrintlnDebug(nn_prev);

    Println(std::cout, "nn next");
    PrintlnDebug(nn_next);

    if (i != 0) {

        auto b = insertBetween(nn_prev, nn_next, i-1, nn);
        nn->afterPrev[i].add(b.v_nn, op);
        if (nn_next) {
            nn_next->Reset(i);
            nn_next->afterPrev[i].add(b.v_n_2, op);
        }

        r_n_1.add(b.v_nn, op);
        r_n_2.add(b.v_n_2, op);
    }

    insert(nn_prev, nn, i);

    Println(std::cout, "nn");
    PrintlnDebug(nn);

    Println(std::cout, "nn next");
    PrintlnDebug(nn_next);

    auto cur = nn_next;
    while (cur != n_2) {
        r_n_2.add(cur->afterPrev[i], op);
        cur = cur->next[i];
    }

    return B{r_n_1, r_n_2};
}

// adds up everything after start and before value
auto ReduceBefore(const std::shared_ptr<Node>& start, int i, const Value& value) {
    struct Result {
        std::shared_ptr<Node> node;
        Across after;
    };

    Across after {};

    auto cur = start;
    while (cur->next[i] && cur->next[i]->value < value) {
        cur = cur->next[i];
        after.add(cur->afterPrev[i], op);
    }

    return Result{cur, after};
}


}