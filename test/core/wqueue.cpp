//
//  wqueue.cpp
//  Ant
//
//  Created by Anton Logunov on 10/22/15.
//
//

#include "gtest/gtest.h"

#include "ant/core/wqueue.hpp"

namespace {

TEST(wqueue, allin) {
    // Process command line arguments
    int iterations = 1000;

    // Create the queue and consumer (worker) threads
    wqueue<WorkItem*>  queue;
    ConsumerThread* thread1 = new ConsumerThread(queue);
    ConsumerThread* thread2 = new ConsumerThread(queue);
    thread1->run();
    thread2->run();
    // Add items to the queue
    WorkItem* item;
    for (int i = 0; i < iterations; i++) {
        item = new WorkItem("abc", 123);
        queue.add(item);
        item = new WorkItem("def", 456);
        queue.add(item);
        item = new WorkItem("ghi", 789);
        queue.add(item);
        sleep(2);
    }

    // Ctrl-C to end program
    sleep(1);
    printf("Enter Ctrl-C to end the program...\n");
    while (1);
}

}