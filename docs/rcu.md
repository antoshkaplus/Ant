rcu_barrier 
    Wait until all in-flight "call_rcu" callbacks complete.
    Note that this primitive does not necessarily wait for an RCU grace 
    period to complete. For example, if there are no RCU callbacks queued 
    anywhere in the system, then rcu_barrier is within its rights to 
    return immediately, without waiting for anything, 
    much less an RCU grace period.
 
synchronize_rcu    
    Control will return to the caller some time after a full grace period has elapsed, 
    in other words after all currently executing RCU read-side critical 
    sections have completed.
    
RCU and other mutexes:
    One must be careful to do not cause deadlocks due to interaction of synchronize_rcu() 
    and RCU read-side with mutexes. If synchronize_rcu() is called with a mutex held, 
    this mutex (or any mutex which has this mutex in its dependency chain) should not 
    be acquired from within a RCU read-side critical section.    
    
QSBR flavor:
    This is especially important to understand in the context of the QSBR flavor: 
    a registered reader thread being "online" by default should be considered as 
    within a RCU read-side critical section unless explicitly put "offline". 
    Therefore, if synchronize_rcu() is called with a mutex held, this mutex, 
    as well as any mutex which has this mutex in its dependency chain should only 
    be taken when the RCU reader thread is "offline" (this can be performed by 
    calling rcu_thread_offline()).

For QSBR rcu_register_thread starts critical read side section by default.
    Quiescent specified by the user calling rcu_quiescent_state() periodically and
    rcu_thread_offline() / rcu_thread_online() during being inactive for a long period.
    
call_rcu - After the end of a subsequent grace period, invokes callback.
    Threads invoking call_rcu() therefore must be registered readers and furthermore 
    cannot be in an RCU extended quiescent state. Doesn't gurantee callback will be called
    immediately after that recent grace period, maybe scheduled for later (in urcu special 
    dedicated threads (usually 1) empty the queue).
    
defer_rcu - Similar to call_rcu() in overall effect, but will sometimes block waiting 
    for a grace period, and can also directly invoke the callbacks. This means that 
    defer_rcu() cannot be called from an RCU read-side critical section, 
    but also that it cannot be called holding any lock acquired by a callback function. 
    In return, defer_rcu() is the most efficient of the three approaches 
    (synchronize_rcu(), call_rcu(), and defer_rcu()) from a real-fast viewpoint.
    
No RCU read-side critical section may contain any operation that waits on the completion 
of an RCU grace period.

In the Linux kernel, the RCU callbacks registered by the call_rcu() function are 
usually invoked in softirq context on the same CPU that registered them. 
Given that userspace does not have a softirq context, some other approach is required. 
The chosen approach is the use of special callback-invocation threads that, 
strangely enough, invoke callbacks. By default, there will be one such thread 
for each multithreaded process, which will likely work quite well in almost all cases.

From paper "Open questions surrounding RCU":

RCU readers execute in RCU read-side critical sections.
Anything not within some RCU read-side critical section is
a quiescent state. A grace period is any time interval during
which each thread has resided in at least one quiescent
state.1 Given these definitions, the fundamental RCU guarantee
follows directly: any RCU read-side critical section in
progress at the beginning of a given grace period will complete
before that same grace period ends.


rcu_assign_pointer
rcu_dereference


rcu hash table:

Threads invoking cds_lfht_destroy() must be registered as RCU readers (using rcu_register_thread()).
It is illegal to invoke cds_lfht_destroy() from within either an RCU read-side 
critical section or any function passed to call_rcu().




defer_rcu EXPERIMENTAL
    Manages a fixed per CPU pool of callbacks, thus allowing a 
    programmer to specify a sharp bound on the deferred memory.

defer_rcu() - Primitive enqueues delayed callbacks. Queued
    callbacks are executed in batch periodically after a grace period.
    Do not use defer_rcu() within a read-side critical section, because
    it may call synchronize_rcu() if the thread queue is full.
    This can lead to deadlock or worse.

rcu_defer_barrier() - Must be called in library destructor
    if a library queues callbacks and is expected to be unloaded with
    dlclose().


rcu_defer_register_thread(void) - Returns 0 if everything went good
rcu_defer_unregister_thread(void)

rcu_defer_barrier(void) - Run callbacks from all threads
rcu_defer_barrier_thread(void) - Run callbacks from the local thread
    synchronize_rcu will be needed in both cases anyway...

Execute all RCU callbacks queued before rcu_defer_barrier() execution.
All callbacks queued on the local thread prior to a rcu_defer_barrier() call
are guaranteed to be executed.
Callbacks queued by other threads concurrently with rcu_defer_barrier()
execution are not guaranteed to be executed in the current batch (could
be left for the next batch). These callbacks queued by other threads are only
guaranteed to be executed if there is explicit synchronization between
the thread adding to the queue and the thread issuing the defer_barrier call.


As defer_rcu is experimental it looks like it doesn't work with qbsr, probably because
there everything is a critical section.