#pragma once

#include <thread>
#include <queue>
#include <list>
#include <atomic>
#include <pthread.h>

namespace ant::parallel::rcu {

class Destructor {
    std::queue<std::pair<void (*)(cds_lfht*), cds_lfht*>> destructors{};
    std::thread worker{};

    pthread_mutex_t m_mutex{};
    pthread_cond_t m_condv{};

    std::atomic_bool stop{false};

public:
    Destructor() {
        pthread_mutex_init(&m_mutex, nullptr);
        pthread_cond_init(&m_condv, nullptr);
        worker = std::move(std::thread(&Destructor::Run, this));
    }

    ~Destructor() {
        if (stop) return;

        logging::LogInfo("User has to rcu::Destructor::Stop before exiting application");
        Stop();
    }

    void Run() {
        rcu_register_thread();
        for (;;) {
            pthread_mutex_lock(&m_mutex);
            while (destructors.empty()) {
                if (stop) goto STOP_DESTRUCTOR;
                rcu_thread_offline();
                pthread_cond_wait(&m_condv, &m_mutex);
                rcu_thread_online();
            }
            auto item = destructors.front();
            destructors.pop();
            pthread_mutex_unlock(&m_mutex);

            item.first(item.second);
        }
        STOP_DESTRUCTOR:
        rcu_unregister_thread();
    }

    void Add(void(* destructor)(cds_lfht*), cds_lfht* head) {
        pthread_mutex_lock(&m_mutex);
        destructors.emplace(destructor, head);
        pthread_cond_signal(&m_condv);
        pthread_mutex_unlock(&m_mutex);
    }

    // should be executed in the end of application
    // after rcu_thread_unregister
    void Stop() {
        stop = true;
        pthread_cond_signal(&m_condv);
        worker.join();

        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_condv);
    }
};

Destructor& getDestructor() {
    static Destructor destructor;
    return destructor;
}

}