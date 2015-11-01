//
//  wqueue.hpp
//  Ant
//
//  Created by Anton Logunov on 10/22/15.
//
//

#ifndef wqueue_h
#define wqueue_h

#include <thread>
#include <list>

using namespace std;

template <typename T> 
class wqueue
{ 
    list<T>   m_queue;
    pthread_mutex_t m_mutex;
    pthread_cond_t  m_condv;

public:
    wqueue() {
        pthread_mutex_init(&m_mutex, NULL);
        pthread_cond_init(&m_condv, NULL);
    }
    
    ~wqueue() {
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_condv);
    }
    
    void add(T item) {
        pthread_mutex_lock(&m_mutex);
        m_queue.push_back(item);
        pthread_cond_signal(&m_condv);
        pthread_mutex_unlock(&m_mutex);
    }
    
    T remove() {
        pthread_mutex_lock(&m_mutex);
        while (m_queue.size() == 0) {
            pthread_cond_wait(&m_condv, &m_mutex);
        }
        T item = m_queue.front();
        m_queue.pop_front();
        pthread_mutex_unlock(&m_mutex);
        return item;
    }
    
    int size() {
        pthread_mutex_lock(&m_mutex);
        int size = m_queue.size();
        pthread_mutex_unlock(&m_mutex);
        return size;
    }
};



class WorkItem
{
    string m_message;
    int    m_number;
    
public:
    WorkItem(const char* message, int number) 
        : m_message(message), m_number(number) {}
    ~WorkItem() {}
    
    const char* getMessage() { return m_message.c_str(); }
    int getNumber() { return m_number; }
};

class ConsumerThread : public thread
{
    wqueue<WorkItem*>& m_queue;
    
public:
    ConsumerThread(wqueue<WorkItem*>& queue) : m_queue(queue) {}
    
    void run() {
        // Remove 1 item at a time and process it. Blocks if no items are 
        // available to process.
        for (int i = 0;; i++) {
            WorkItem* item = (WorkItem*)m_queue.remove();
            printf("thread loop %d - item: message - %s, number - %d\n", 
                   i, item->getMessage(), 
                   item->getNumber());
            delete item;
        }
        return;
    }
};



#endif /* wqueue_h */
