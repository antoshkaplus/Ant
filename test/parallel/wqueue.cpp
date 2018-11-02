//
//  wqueue.cpp
//  Ant
//
//  Created by Anton Logunov on 10/22/15.
//
//

#include "gtest/gtest.h"

#include "ant/parallel/wqueue.hpp"

namespace {

TEST(wqueue, allin) {
    // Process command line arguments
    int iterations = 1000;

    // Create the queue and consumer (worker) threads
    ant::parallel::wqueue<ant::parallel::WorkItem*>  queue;
    ant::parallel::ConsumerThread* thread1 = new ant::parallel::ConsumerThread(queue);
    ant::parallel::ConsumerThread* thread2 = new ant::parallel::ConsumerThread(queue);
    thread1->run();
    thread2->run();
    // Add items to the queue
    ant::parallel::WorkItem* item;
    for (int i = 0; i < iterations; i++) {
        item = new ant::parallel::WorkItem("abc", 123);
        queue.add(item);
        item = new ant::parallel::WorkItem("def", 456);
        queue.add(item);
        item = new ant::parallel::WorkItem("ghi", 789);
        queue.add(item);
        sleep(2);
    }

    // Ctrl-C to end program
    sleep(1);
    printf("Enter Ctrl-C to end the program...\n");
    while (1);
}

}