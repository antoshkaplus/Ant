#pragma once

#include <optional>

#include <urcu-qsbr.h>
#include <urcu-defer.h>
#include <urcu/rculfqueue.h>


namespace ant::parallel::rcu {

template<typename TValue>
class queue {

    struct handle {
        cds_lfq_node_rcu node;
        rcu_head head;

        handle() : node(), head() {
            cds_lfq_node_init_rcu(&node);
        }
    };

    struct item : public handle {
        TValue value;

        template<typename... TArgs>
        item(TArgs&& ... args)
                : handle(), value(std::forward<TArgs>(args)...) {
        }

        item(const item&) = delete;

        item& operator=(const item&) = delete;
    };

    cds_lfq_queue_rcu q;

public:
    queue() {
        // call_rcu is passed to free dummy nodes created by the queue
        // no reason to use defer_rcu here
        cds_lfq_init_rcu(&q, call_rcu);
    }

    queue(const queue&) = delete;
    queue& operator=(const queue&) = delete;

    ~queue()  {
        clear();
        auto res = cds_lfq_destroy_rcu(&q);
        if (res != 0) {
            // TODO "try destroy non-empty queue"
        }
    }

    void clear() {
        for (;;) {
            cds_lfq_node_rcu* node = cds_lfq_dequeue_rcu(&q);
            if (node == nullptr) return;

            call_rcu(&caa_container_of(node, queue::handle, node)->head, erase_node);
        }
    }

    template<typename... TArgs>
    void enqueue(TArgs&& ... args) {

        std::unique_ptr<queue::item> item(new queue::item(std::forward<TArgs>(args)...));

        cds_lfq_enqueue_rcu(&q, &item->node);

        item.release();
    }

    std::optional<std::reference_wrapper<TValue>> dequeue() {
        cds_lfq_node_rcu* node = cds_lfq_dequeue_rcu(&q);
        if (node == nullptr) return {};

        auto item = static_cast<queue::item*>(caa_container_of(node, queue::handle, node));
        call_rcu(&item->head, erase_node);

        return {item->value};
    }

private:
    inline static void erase_node(rcu_head* head) {
        delete static_cast<queue::item*>(caa_container_of(head, queue::handle, head));
    }
};

}
