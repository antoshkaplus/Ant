
#include <thread>
#include <atomic>

#include <gtest/gtest.h>
#include <urcu-qsbr.h>

#include "ant/core/core.hpp"
#include "ant/parallel/rcu/queue_defer.hpp"

struct Item {
    double d_1, d_2;

    bool operator==(const Item& t) const {
        return d_1 == t.d_1 && d_2 == t.d_2;
    }
};


TEST(RcuQueueDefer, _1) {
    rcu_register_thread();
    rcu_defer_register_thread();

    ant::parallel::rcu::queue_defer<Item> queue;

    Item m_1 {1, 1}, m_2 {2, 2};
    queue.enqueue(m_1);
    queue.enqueue(m_2);

    auto t_1 = queue.dequeue();
    ASSERT_TRUE(t_1 && t_1.value().get() == m_1);
    auto t_2 = queue.dequeue();
    ASSERT_TRUE(t_2 && t_2.value().get() == m_2);

    ASSERT_FALSE(queue.dequeue());

    rcu_barrier();
    rcu_defer_barrier();

//    rcu_defer_unregister_thread();
//    rcu_unregister_thread();
}


TEST(RcuQueueDefer, EnqueueDequeue_1) {
//    rcu_register_thread();
//    rcu_defer_register_thread();

    ant::parallel::rcu::queue_defer<int> queue;

    const auto N = 10000;
    for (auto i = 0; i < N; ++i) queue.enqueue(i);

    for (auto i = 0; i < N; ++i) {
        auto t = queue.dequeue();
        ASSERT_TRUE(t && *t == i);
    }

    rcu_barrier();
    rcu_defer_barrier();
//
//    rcu_defer_unregister_thread();
//    rcu_unregister_thread();
}

// TODO because only one thread changes the variable can call fence release to after all changes are done
template <class TValue, class TGenerator = TValue(*)()>
class Enqueue {

    ant::parallel::rcu::queue_defer<TValue>& queue;
    std::atomic_int count_;
    TGenerator generator;
    std::thread thread;

public:

    Enqueue(ant::parallel::rcu::queue_defer<TValue>& queue, int count, TGenerator generator = []() {return TValue();})
            : queue(queue), count_(count), thread(&Enqueue::Run, this), generator(std::move(generator)) {}

    ~Enqueue() {
        if (thread.joinable()) thread.join();
    }

    void Run() {
        rcu_register_thread();
        rcu_defer_register_thread();

        int local_count = count_;

        int g = generator();
        for (auto i = 0; i < local_count; ++i) queue.enqueue(generator());

        count_ = local_count;

        rcu_defer_barrier();

        rcu_defer_unregister_thread();
        rcu_unregister_thread();
    }

    void Join() {
        thread.join();
    }

    int count() const {
        return count_;
    }
};

template <class TValue>
class Dequeue {

    ant::parallel::rcu::queue_defer<TValue>& queue;
    std::atomic_int count_ = 0;
    std::atomic_bool stop_ {false};
    std::thread thread;

public:
    Dequeue(ant::parallel::rcu::queue_defer<TValue>& queue)
            : queue(queue), thread(&Dequeue::Run, this) {}

    ~Dequeue() {
        stop_ = true;
        if (thread.joinable()) thread.join();
    }

    void Run() {
        rcu_register_thread();
        rcu_defer_register_thread();

        int local_count = 0;
        while (!stop_) {
            auto elem = queue.dequeue();
            if (elem) ++local_count;
        }

        count_ = local_count;

        rcu_defer_barrier();

        rcu_defer_unregister_thread();
        rcu_unregister_thread();
    }

    void Join() {
        stop_ = true;
        thread.join();
    }

    int count() const {
        return count_;
    }
};


TEST(RcuQueueDefer, Enqueue_1_Dequeue_1) {
//    rcu_register_thread();
//    rcu_defer_register_thread();

    ant::parallel::rcu::queue_defer<int> queue;
    Enqueue en(queue, 10000);
    Dequeue de(queue);

    en.Join();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    de.Join();

    ASSERT_EQ(en.count(), de.count());

    rcu_barrier();
    rcu_defer_barrier();

//    rcu_defer_unregister_thread();
//    rcu_unregister_thread();
}

TEST(RcuQueueDefer, Enqueue_3_Dequeue_3) {
//    rcu_register_thread();
//    rcu_defer_register_thread();

    ant::parallel::rcu::queue_defer<int> queue;
    std::deque<Enqueue<int>> ens;
    std::deque<Dequeue<int>> des;

    int N = 100000;
    for (auto i = 0; i < 3; ++i) ens.emplace_back(queue, N);
    for (auto i = 0; i < 3; ++i) des.emplace_back(queue);

    for (auto& en : ens) en.Join();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    for (auto& de : des) de.Join();

    auto total_en = std::accumulate(ens.begin(), ens.end(), 0, [](int init, Enqueue<int>& en) {
        return init + en.count();
    });
    auto total_de = std::accumulate(des.begin(), des.end(), 0, [](int init, Dequeue<int>& de) {
        return init + de.count();
    });

    ASSERT_EQ(total_en, total_de);
    ASSERT_EQ(total_en, N * 3);

    rcu_barrier();
    rcu_defer_barrier();

//    rcu_defer_unregister_thread();
//    rcu_unregister_thread();
}

struct ItemCounter {
    std::atomic_int& deleted_counter;

    ItemCounter(std::atomic_int& deleted_counter) : deleted_counter(deleted_counter) {}

    ~ItemCounter() {
        ++deleted_counter;
        ant::Println(std::cout, "Destructor called");
    }
};

TEST(RcuQueueDefer, Enqueue_1_Dequeue_1_Deleted) {
    std::atomic_int deleted_counter = 0;

//    rcu_register_thread();
//    rcu_defer_register_thread();

    ant::parallel::rcu::queue_defer<ItemCounter> queue;
    Enqueue en(queue, 10000, [&]() -> std::atomic_int& {return deleted_counter;});
    Dequeue de(queue);

    en.Join();
    std::this_thread::sleep_for(std::chrono::seconds(1));

    ASSERT_EQ(deleted_counter, 0);

    de.Join();

    ASSERT_EQ(en.count(), de.count());

    rcu_barrier();
    rcu_defer_barrier();

//    rcu_defer_unregister_thread();
//    rcu_unregister_thread();


    ASSERT_EQ(deleted_counter, 10000);

    rcu_defer_unregister_thread();
    rcu_unregister_thread();
}