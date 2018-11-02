#pragma once

#include <optional>

#include <urcu-qsbr.h>
#include <urcu-defer.h>
#include <urcu/rculfqueue.h>


namespace ant::parallel::rcu {


void defer_rcu_like_call_rcu(rcu_head* head, void (*func)(struct rcu_head *head))
{
    defer_rcu_qsbr((void(*)(void*))func, head);
}


template<typename TValue>
class queue_defer {

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
    queue_defer() {
        // call_rcu is passed to free dummy nodes created by the queue
        // no reason to use defer_rcu here
        cds_lfq_init_rcu(&q, call_rcu);
    }

    queue_defer(const queue_defer&) = delete;
    queue_defer& operator=(const queue_defer&) = delete;

    ~queue_defer()  {
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

            defer_rcu_like_call_rcu(&caa_container_of(node, queue_defer::handle, node)->head, erase_node);
        }
    }

    template<typename... TArgs>
    void enqueue(TArgs&& ... args) {

        std::unique_ptr<queue_defer::item> item(new queue_defer::item(std::forward<TArgs>(args)...));

        cds_lfq_enqueue_rcu(&q, &item->node);

        item.release();
    }

    std::optional<std::reference_wrapper<TValue>> dequeue() {
        cds_lfq_node_rcu* node = cds_lfq_dequeue_rcu(&q);
        if (node == nullptr) return {};

        auto item = static_cast<queue_defer::item*>(caa_container_of(node, queue_defer::handle, node));
        defer_rcu_like_call_rcu(&item->head, erase_node);

        return {item->value};
    }

private:
    inline static void erase_node(rcu_head* head) {
        delete static_cast<queue_defer::item*>(caa_container_of(head, queue_defer::handle, head));
    }
};

}
