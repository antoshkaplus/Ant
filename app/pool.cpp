
// you should be able to provide your params like this:
// -max_cap 89 -min_cap 90 -n_threads 3 -obj_per_thread 10000

// deallocation is free, because of that we have restriction for objects to have trivial destructor 
// we don't care about capacity growth of stack with ready to go objects
// but if queue of taken objects increases on alloc and it's capacity needs to grow we
// are a bit in trouble.

// fixed_capacity, predefined capacity should look cool
// or maybe queue item is small enough that allocation won't take long

// try to increase capacity asynchronously ??? 

// would be cool to know real world solution to this problem, guys
// please send it if this is possible


#include <memory>
#include <mutex>
#include <thread>
#include <map>
#include <string>
#include <vector>
#include <chrono>
#include <iostream>

#include <boost/lockfree/queue.hpp>
#include <boost/lockfree/stack.hpp>
#include <boost/type_traits.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>

using Index = int;

std::map<std::string, std::string> command_line_options(const char* argv[], int argc) {
    std::map<std::string, std::string> res;
    for (Index i = 0; i < argc; ++i) {
        if (argv[i][0] == '-') {
            std::string key(argv[i]+1);
            res[key] = "";
            if (i+1 < argc && argv[i+1][0] != '-') {
                res[key] = std::string(argv[i+1]);
                ++i;
            } 
        }
    }
    return res;
} 


template <std::size_t MAX_OBJ_SIZE>
class pool {

    using SH_V = std::shared_ptr<void>;

public:
    pool(std::size_t maxCapacity, std::size_t minCapacity)
        : maxCapacity(maxCapacity), minCapacity(minCapacity) {
        
        qu_sz = 0;
        st_sz = 0;
        stop = false;
        th = std::thread(&pool::run, this);
    }
    
    ~pool() {
        stop = true;
        {
            std::lock_guard<std::mutex> lck(mtx);
            cv.notify_one();
        }
        th.join();
        
        SH_V* v;
        while (!st.empty()) {
            st.unsynchronized_pop(v);
            delete v;
        }
        while (!qu.empty()) {
            qu.unsynchronized_pop(v);
            delete v;
        }
    }
    
    template<typename OBJECT, typename ...ARGS>
    void alloc(std::shared_ptr<OBJECT>& object, ARGS ...args) {
        // unable to track every object type
        BOOST_STATIC_ASSERT((boost::has_trivial_destructor<OBJECT>::value));
        if (st_sz < minCapacity) {
            cv.notify_one();
        }
        SH_V* v;
        bool pop = st.pop(v);
        if (!pop) {
            object = std::make_shared<OBJECT>(args...);
        }
        else {
            --st_sz;
            new (v->get()) OBJECT(args...);
            // share with object first to increase counter 
            // else may be collected
            object = std::static_pointer_cast<OBJECT>(*v);
            qu.push(v);
            ++qu_sz;
        }
    }
    
private:

    void run() {
        std::unique_lock<std::mutex> lck(mtx);
        while (!stop) {
            collect();
            replenish();            
            cv.wait(lck);
        }
    }
    
    void collect() {
        // qu sz may decrease only here
        SH_V* v;
        int collected = 0;
        for (auto i = 0; i < qu_sz; ++i) {
            qu.pop(v);
            if (v->use_count() == 1) {
                ++collected;
                if (st_sz < maxCapacity) {
                    st.push(v);
                    ++st_sz;
                } else {
                    delete v;
                }
            } else {
                qu.push(v);
            }
        }
        qu_sz -= collected;
    }
    
    
    void replenish() {
        while (st_sz < maxCapacity) {
            void* m = malloc(MAX_OBJ_SIZE);
            if (m == nullptr) {
                // i 
                // break;
                // should we really check ???
                // throw error
            }
            // only place where shared ptr created
            SH_V* v = new SH_V{m, [](void* p){free(p);} };
            st.push(v);
            ++st_sz;
        }
    }
    
    // ready to go objects
    boost::lockfree::stack<std::shared_ptr<void>*> st{1};
    std::atomic_int st_sz;
    
    // taken objects
    boost::lockfree::queue<std::shared_ptr<void>*> qu{1};
    // possible to avoid using sz
    std::atomic_int qu_sz;
    
    size_t maxCapacity;
    size_t minCapacity;
    
    // for asynchronous replenishment
    std::condition_variable cv;
    std::thread th;
    std::mutex mtx;
    std::atomic_bool stop;
};


constexpr int BUF_SZ = 100;

struct P {
    char buffer[BUF_SZ];
};

using Pool = pool<BUF_SZ>;


uint64_t Alloc(Pool& pool, std::shared_ptr<P>& p) {
    auto b = std::chrono::high_resolution_clock::now();
    pool.alloc(p);
    auto d = std::chrono::high_resolution_clock::now() - b;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(d).count();
}

using namespace boost::accumulators;
using STATS = accumulator_set<uint64_t, stats<tag::min, tag::max, tag::mean>>;

// this is overall test
// successful collect
void test(Pool& pool, int obj_count, STATS& s) {
    std::shared_ptr<P> p;
    for (auto i = 0; i < obj_count; ++i) {
        s(Alloc(pool, p));
    }
}

// may go to replinish
void test_replenish(Pool& pool, int obj_count, STATS& s) {
    std::vector<std::shared_ptr<P>> ps(obj_count);
    for (auto i = 0; i < obj_count; ++i) {
        s(Alloc(pool, ps[i]));
    }
}



int main(int argc, const char* argv[]) {
    // read command line options
    auto options = command_line_options(argv, argc);
    if (options.count("max_cap") == 0) {
        options["max_cap"] = "1000";
    }
    if (options.count("min_cap") == 0) {
        options["min_cap"] = "10";
    }
    if (options.count("n_threads") == 0) {
        options["n_threads"] = "4";
    }
    if (options.count("obj_per_thread") == 0) {
        options["obj_per_thread"] = "100000";
    }
    
    int maxCapacity = boost::lexical_cast<int>(options["max_cap"]);
    int minCapacity = boost::lexical_cast<int>(options["min_cap"]);
    int threadCount = boost::lexical_cast<int>(options["n_threads"]);
    int objPerThread = boost::lexical_cast<int>(options["obj_per_thread"]);

    Pool pool(maxCapacity, minCapacity);
    std::vector<std::thread> ths(threadCount);
    std::vector<STATS> ss(threadCount);
    for (auto i = 0; i < threadCount; ++i) {
        ths[i] = std::thread(test_replenish, std::ref(pool), objPerThread, std::ref(ss[i]));
    }
    for (auto& t : ths) {
        t.join();
    }
    std::vector<uint64_t> minv(threadCount);
    std::transform(ss.begin(), ss.end(), minv.begin(), [](const STATS& s) { return min(s); });
    auto minn = *std::min_element(minv.begin(), minv.end());
    std::cout << "min: " << minn << " ns" << std::endl;
    
    std::vector<uint64_t> maxv(threadCount);
    std::transform(ss.begin(), ss.end(), maxv.begin(), [](const STATS& s) { return max(s); });
    auto maxx = *std::max_element(maxv.begin(), maxv.end());
    std::cout << "max: " << maxx << " ns" << std::endl;
    
    accumulator_set<uint64_t, stats<tag::mean>> help;
    for (auto i = 0; i < threadCount; ++i) {
        help(mean(ss[i]));
    }
    auto meann = mean(help);
    std::cout << "mean: " << meann << " ns" << std::endl;
}












