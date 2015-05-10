// 
// core types are considered similar to std
// that's why low case names used
// it's a huge honor to be low case underscore delimeter name
// 


#ifndef __ANT__CORE_H__
#define __ANT__CORE_H__

#include <vector>
#include <tuple>
#include <map>
#include <numeric>
#include <random>
#include <set>
#include <cassert>
#include <queue>
#include <unordered_set>


namespace ant {

using Int = int;
// index and count should be of type int
// because 
// 1) unsigned types increase probability of making a bug
// 2) lesser type will create problem of casting or being too little
// 3) bigger type impossible to iterate through
// the only thing is unsigned integers is good for bitwise operations
using Count = int; 
using Index = int;

using Long = int64_t;
using Float = double;
using Double = double;


enum struct Enabler {}; 
constexpr Enabler enabler = {};

// need to avoid usage with one or 0 parameters sometimes
    
template<typename... Nothing> 
struct All {
    static constexpr bool value = true;
};   
template<typename Condition, typename... OtherConditions>
struct All<Condition, OtherConditions...> {
    static constexpr bool value = Condition::value && All<OtherConditions...>::value;
};

template<typename... Nothing> 
struct Any {
    static constexpr bool value = true;
};   
template<typename Condition, typename... OtherConditions>
struct Any<Condition, OtherConditions...> {
    static constexpr bool value = Condition::value || All<OtherConditions...>::value;
};


template<typename Condition>
using EnableIf = typename std::enable_if<Condition::value, Enabler>::type;

template<typename Condition>
using EnableIfNot = typename std::enable_if<!Condition::value, Enabler>::type;

template<typename Condition, typename... OtherConditions>
using EnableIfAll = EnableIf<All<Condition, OtherConditions...>>;

template<typename Condition, typename... OtherConditions>
using EnableIfAny = EnableIf<Any<Condition, OtherConditions...>>;

 
template<typename... Nothing>
struct IsAnySame {
    static constexpr bool value = false;
};
template<typename Type, typename Another, typename... Other>
struct IsAnySame<Type, Another, Other...> {
    static constexpr bool value = std::is_same<Type, Another>::value || IsAnySame<Type, Other...>::value;
};


unsigned GetMillisCount();


template<class Key, class Value>
std::tuple<std::vector<Key>, std::vector<Value>> Zip(std::map<Key, Value>& m) {
    std::tuple<std::vector<Key>, std::vector<Value>> r;
    auto& keys = std::get<0>(r);
    auto& values = std::get<1>(r);  
    keys.reserve(m.size());
    values.reserve(m.size());
    for (auto& p : m) {
        keys.push_back(p.first);
        values.push_back(p.second);
    }
    return r;
}

// sometimes someone can use with Long, not just Int type
template<class T>
class Range {
public:
    class Iterator : std::iterator<std::input_iterator_tag, T> {
    public:
        Iterator(const Range& range, T current) 
        : range_(range), current_(current) {
            // just copied that lol
            if (range_.step_*(current_-range_.last_) > 0) current_ = range_.last_;
        }
        const T operator*() const { return current_; }
        bool operator==(const Iterator& it) const {
            return current_ == *it;
        }
        bool operator!=(const Iterator& it) const {
            return current_ != *it;
        }
        Iterator& operator++() {
            current_ += range_._step;
            if (range_.step_*(current_-range_.last_) > 0) current_ = range_.last_;
            return *this;
        }
        Iterator operator++(int) { 
            Iterator it(*this); 
            operator++(); 
            return it;
        }
    private:
        const Range& range_;
        T current_;
    };
    
    friend class Iterator;
       
    Range(T last) : first_(0), last_(last), step_(1) {}
    Range(T first, T last, T step = 1)
    : first_(first), last_(last), step_(step) {}

    Iterator begin() const { return Iterator(*this, first_); }
    Iterator end()   const { return Iterator(*this, last_); }

private:
    T first_, last_, step_;
};



class DisjointSet {
public:
    DisjointSet() {}
    DisjointSet(Count element_count) {
        Init(element_count);
    }

    void Init(Count element_count) {
        element_count_ = element_count;
        set_count_ = element_count;
        data_.resize(element_count);
        size_.resize(element_count);
        Reset();
    }

    void Reset() {
        std::iota(data_.begin(), data_.end(), 0);
        fill(size_.begin(), size_.end(), 1);
    }

    void Unite(Index i_0, Index i_1) {
        --set_count_;
        Index
        r_0 = root(i_0),
        r_1 = root(i_1);
        // will join r_0 to r_1, so r_1 height should be bigger
        if (size_[r_0] > size_[r_1]) {
            std::swap(r_0, r_1);
        }
        data_[r_0] = r_1;
        size_[r_1] += size_[r_0];
        
    }

    bool is_separate(Index i_0, Index i_1) {
        return root(i_0) != root(i_1);
    }

    Index root(Index i) {
        while (i != data_[i]) {
            i = data_[i] = data_[data_[i]];
        }
        return i;
    }

    size_t size() {
        return element_count_;
    }

    Count set_count() {
        return set_count_;
    }

private:
    Count element_count_;
    Count set_count_;
    std::vector<Index> data_;
    // how many elements in set with index, if index is root
    std::vector<size_t> size_;
}; 




//struct discrete_distribution {
//
//    template<class ForwardIterator>
//    discrete_distribution(ForwardIterator first, ForwardIterator last) {
//        weight_.assign(first, last);
//        cumulative_.resize(weight_.size());
//        std::partial_sum(weight_.begin(), weight_.end(), cumulative_.begin());
//        uni_ = std::uniform_real_distribution<double>(0, cumulative_.back());
//    }
//    discrete_distribution(const std::initializer_list<double>& weights) 
//    : discrete_distribution(weights.begin(), weights.end()) {}
//
//    void set_weight(Index i, double w) {
//        assert(w >= 0);
//        //            auto d = w-weight_[i];
//        //            for (auto k = i; k < weight_.size(); ++k) {
//        //                cumulative_[k] += d;
//        //            }
//        //            
//        weight_[i] = w;
//        std::fill(cumulative_.begin(), cumulative_.end(), 0);
//        std::partial_sum(weight_.begin(), weight_.end(), cumulative_.begin());
//        uni_ = std::uniform_real_distribution<double>(0, cumulative_.back());
//    }
//
//    double get_weight(Index i) {
//        return weight_[i];
//    }
//
//    template<class RNG> 
//    Index operator()(RNG& rng) {
//        Index i = std::lower_bound(cumulative_.begin(), cumulative_.end(), uni_(rng))-cumulative_.begin();
//        if (cumulative_.back() != 0.) while ( weight_[i] == 0.) --i;
//        return i;
//    }
//
//    std::uniform_real_distribution<double> uni_;
//    std::vector<double> cumulative_;
//    std::vector<double> weight_;
//};



// current stack supports iteration!
template<class T>
class Stack {
public:
    // can't inherit from vector iterator
    // too open class
    class ConstIterator : std::iterator<std::input_iterator_tag, T> {
    public:
        ConstIterator(const Stack& stack, typename std::vector<T>::const_iterator current) 
        : stack_(stack), current_(current) {}
        const T& operator*() const { return *current_; }
        bool operator==(const ConstIterator& it) const {
            return current_ == it;
        }
        bool operator!=(const ConstIterator& it) const {
            return current_ != it.current_;
        }
        ConstIterator& operator++() {
            ++current_;
            return *this;
        }
        // post iterator
        ConstIterator operator++(int) { 
            ConstIterator it(*this); 
            operator++(); 
            return it;
        }
    private:
        const Stack& stack_;
        typename std::vector<T>::const_iterator current_;
    };

    friend class ConstIterator;

    ConstIterator begin() const { return ConstIterator(*this, data_.begin()); }
    ConstIterator end()   const { return ConstIterator(*this, data_.end()); }

    T& top() {
        return data_.back();
    }
    void push(const T& val) {
        data_.push_back(val);
    }
    void pop() {
        data_.pop_back();
    }
    bool empty() const {
        return data_.empty();
    }
    size_t size() const {
        return data_.size();
    }

private:
    std::vector<T> data_;
};



// probably should be an inheritance
template <class T>
class CountMap : public std::map<T, Count> {
public:
    void decrease(const T& t) { decrease(t, 1); }

    void decrease(const T& t, Count val) {
        auto it = this->find(t);
        if ((it->second-=val) == 0) {
            this->erase(it);
        }
    }

    void increase(const T& t) { increase(t, 1); }

    void increase(const T& t, Count val) {
        this->emplace(t, 0).first->second+=val;
    }

    std::set<T> keys() const {
        std::set<T> r;
        for (auto p : *this) {
            r.insert(p.first);
        }
        return r;
    }

    size_t get(const T& t) const {
        auto it = this->find(t);
        return it == this->end() ? 0 : it->second;
    }
};

template<class T>
class CircularList {
private:
    struct Node {
        T value;
        Node* prev;
        Node* next;
    };

    template<class V>
    struct BaseIterator : std::iterator<std::bidirectional_iterator_tag, V> {
        BaseIterator(Node* n) : node_(n) {} 
        
        V& operator*() const { return node_->value; }
        V* operator->() const { return node_->value; }
        
        bool operator==(const BaseIterator& it) const {
            return node_ == it.node_;
        }
        bool operator!=(const BaseIterator& it) const {
            return node_ != it.node_;
        }
        BaseIterator& operator++() {
            node_ = node_->next;
            return *this;
        }
        // post iterator
        BaseIterator operator++(int) { 
            BaseIterator it(node_); 
            node_ = node_->next; 
            return it;
        }
        
        BaseIterator operator--() {
            node_ = node_->prev;
            return *this;
        }
        BaseIterator operator--(int) {
            BaseIterator it(node_);
            node_ = node_->prev;
            return it;
        }
        
    private:
        Node* node_;
        friend struct circular_list;
    };

public:
    using Iterator = BaseIterator<T>;
    using ConstIterator = BaseIterator<const T>;

    CircularList() : focus_(nullptr) {}

    void InitFocus(const T& value) {
        focus_ = new Node();
        focus_->value = value;
        focus_->prev = focus_->next = focus_;
    }

    template<typename It, EnableIf<IsAnySame<It, Iterator, ConstIterator>>>
    It InsertAfter(It it_pos, const T& value) {
        ++count_;
        Node *pos = it_pos.node_;
        if (pos == nullptr) {
            init_focus(value);
            return Iterator(focus_);
        }
        Node *prev = pos;
        Node *next = pos->next;
        Node *cur = new Node();
        cur->next = next;
        cur->prev = prev;
        cur->value = value;
        next->prev = cur;
        prev->next = cur;
        return It(cur);
    }

    template<typename It, EnableIf<IsAnySame<It, Iterator, ConstIterator>>>
    It InsertBefore(Iterator it_pos, const T& value) {
        ++count_;
        Node *pos = it_pos.node_;
        if (pos == nullptr) {
            init_focus(value);
            return Iterator(focus_);
        }
        Node *prev = pos->prev;
        Node *next = pos;
        Node *cur = new Node();
        cur->next = next;
        cur->prev = prev;
        cur->value = value;
        prev->next = cur;
        next->prev = cur;
        return It(cur);
    }
    
    template<typename It, EnableIf<IsAnySame<It, Iterator, ConstIterator>>>
    void Erase(It it_pos) {
        --count_;
        Node *pos = it_pos.node_;
        if (pos == focus_) focus_ = pos->next;
        pos->prev->next = pos->next;
        pos->next->prev = pos->prev;
        delete pos;
        if (count_ == 0) focus_ = nullptr;
    }

    Iterator focus() {
        return Iterator(focus_);
    }

    ConstIterator focus() const {
        return ConstIterator(focus_);
    }

    Count size() const {
        return count_;
    }

    bool is_empty() const {
        return focus_ == nullptr;
    }

private:
    Node* focus_;
    Count count_ = 0;
};


    
std::map<std::string, std::string> command_line_options(const char* argv[], int argc);

int atoi(char* str, Int n);
int atoi(char* first, char *last);


struct command_line_parser {
    command_line_parser(const char* argv[], int argc) {
        options_ = command_line_options(argv, argc);
    }
    
    bool exists(const std::string& option) const {
        return options_.find(option) != options_.end();
    }
    
    bool hasValue(const std::string& option) const {
        return options_.at(option) != "";
    }
    
    std::string getValue(std::string option) const {
        std::string value = options_.at(option);
        if (value == "") {
            throw std::logic_error("command line option has no value");
        }
        return value;
    }
private:
    std::map<std::string, std::string> options_;
};


// let it be unsigned char, int or long
template<class T>
struct CombinationGenerator {
    // should be like iterator

    static const Int kByteBitCount = 8;
    static const Int kMaxElementCount = sizeof(T)*kByteBitCount;

    struct Tails : std::array<T, kMaxElementCount> {
        using std::array<T, kMaxElementCount>::at;
        
        Tails() {
            at(0) = 1;
            init(1);
        }
        
        void init(Int i) {
            if (i == kMaxElementCount) return;
            at(i) = at(i-1) << 1;
            at(i) += 1;
            init(i+1);
        } 
    };

    static constexpr Tails tails = Tails();

    CombinationGenerator(Int selection_count, Int element_count, T starting_combination = 0)
    : selection_count_(selection_count), element_count_(element_count), data_(starting_combination) {
        if (data_ == 0) {
            data_ = tails[selection_count-1];
        }
    }

    const T& next() {
        Int i = 0;
        Int k = 0; // how many elements behind
        while (i < element_count_) {
            
            if ((data_ >> i) && 1) {
                if (!((data_ >> (i+1)) & 1)) {
                    //data_ |= (1 << i+1);
                    data_ >>= i+1;
                    data_ += 1;
                    data_ <<= i+1;
                    data_ |= tails[k-1];
                    break;
                }
                ++k;
            }
            // can shift father
        }
        return data_;
    } 

    bool hasNext() {
        return data_ != (tails[selection_count_-1] << (element_count_ - selection_count_)); 
    }

    T data_;
    Int element_count_;
    Int selection_count_;
};

template<class T>
struct BinomialHeap {
    const T& min() {
        auto& x = data_[0];
        auto* min = data_;
        while (x < data_.size()) {
            if (data_[x] < min) {}
        } 
    }
    std::vector<T> data_;
};


//
//
//P LogicBinarySearch(P a, P b, Func func, Distance dist, Condition cond, double eps) {
//    
//    bool v_a = func(a);
//    bool v_b = func(b);
//    while (dist(b, a) > eps) {
//        if (cond())
//    }
//    
//    
//    
//}

template<class T>
uint64_t Hash(T c_0, T c_1, T c_2, T c_3) {
    uint64_t r = 0;
    r += c_0;
    r <<= 16;
    r += c_1;
    r <<= 16;
    r += c_2;
    r <<= 16;
    r += c_3;
    return r;
}


// should be used only when you are going to use 
// a lot of look ups
template<typename T, typename Comp = std::less<T>> 
class HeapFind {
    std::priority_queue<T, std::vector<T>, Comp> queue;
    std::unordered_set<T> set; 
    
public:    
    void Pop() {
        set.erase(queue.top());
        queue.pop();
    }
    
    void Push(const T& t) {
        queue.push(t);
        set.insert(t);
    }
    
    const T& Top() const {
        return queue.top();
    }
    
    bool Exists(const T& t) const {
        return set.count(t) == 1;
    }
    
    bool Empty() const {
        return queue.empty();
    }
    
    Count Size() const {
        return queue.size();
    }
};




}


#endif