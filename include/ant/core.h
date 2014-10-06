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


namespace ant {

using Int = int;
using Count = int; 
using Index = int;
using Float = double;


template<class Key, class Value>
std::tuple<std::vector<Key>, std::vector<Value>> zip(std::map<Key, Value>& m) {
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


template<class T>
struct range {

    struct range_iterator : std::iterator<std::input_iterator_tag, T> {
        const range& _range;
        T _current;
        
        range_iterator(const range& range, T current) 
        : _range(range), _current(current) {
            // just copied that lol
            if (_range._step*(_current-_range._last) > 0) _current = _range._last;
        }
        const T operator*() const { return _current; }
        bool operator==(const range_iterator& it) const {
            return _current == *it;
        }
        bool operator!=(const range_iterator& it) const {
            return _current != *it;
        }
        range_iterator& operator++() {
            _current += _range._step;
            if (_range._step*(_current-_range._last) > 0) _current = _range._last;
            return *this;
        }
        range_iterator operator++(int) { 
            range_iterator it(*this); 
            operator++(); 
            return it;
        }
    };
    friend class range_iterator;
    typedef range_iterator iterator;

public:
    range(T last) : _first(0), _last(last), _step(1) {}
    range(T first, T last, T step = 1)
    : _first(first), _last(last), _step(step) {}

    iterator begin() const { return iterator(*this, _first); }
    iterator end()   const { return iterator(*this, _last); }

private:
    T _first, _last, _step;
    };

    typedef range<Int> irange;
    typedef range<double> frange;

    struct disjoint_set {
    disjoint_set() {}
    disjoint_set(Count element_count) {
        init(element_count);
    }

    void init(Count element_count) {
        element_count_ = element_count;
        set_count_ = element_count;
        data_.resize(element_count);
        size_.resize(element_count);
        reset();
    }

    void reset() {
        std::iota(data_.begin(), data_.end(), 0);
        fill(size_.begin(), size_.end(), 1);
    }

    void unite(Index i_0, Index i_1) {
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

    Count element_count_;
    Count set_count_;
    std::vector<Index> data_;
    std::vector<size_t> size_;
}; // struct range




struct discrete_distribution {

    template<class ForwardIterator>
    discrete_distribution(ForwardIterator first, ForwardIterator last) {
        weight_.assign(first, last);
        cumulative_.resize(weight_.size());
        std::partial_sum(weight_.begin(), weight_.end(), cumulative_.begin());
        uni_ = std::uniform_real_distribution<double>(0, cumulative_.back());
    }
    discrete_distribution(const std::initializer_list<double>& weights) 
    : discrete_distribution(weights.begin(), weights.end()) {}

    void set_weight(Index i, double w) {
        assert(w >= 0);
        //            auto d = w-weight_[i];
        //            for (auto k = i; k < weight_.size(); ++k) {
        //                cumulative_[k] += d;
        //            }
        //            
        weight_[i] = w;
        std::fill(cumulative_.begin(), cumulative_.end(), 0);
        std::partial_sum(weight_.begin(), weight_.end(), cumulative_.begin());
        uni_ = std::uniform_real_distribution<double>(0, cumulative_.back());
    }

    double get_weight(Index i) {
        return weight_[i];
    }

    template<class RNG> 
    Index operator()(RNG& rng) {
        Index i = std::lower_bound(cumulative_.begin(), cumulative_.end(), uni_(rng))-cumulative_.begin();
        if (cumulative_.back() != 0.) while ( weight_[i] == 0.) --i;
        return i;
    }

    std::uniform_real_distribution<double> uni_;
    std::vector<double> cumulative_;
    std::vector<double> weight_;
};




template<class T, Count Cap>
struct cap_stack {
    cap_stack() : size_(0) {}

    T& top() {
        return data_[size_-1];
    }

    void push(const T& val) {
        data_[size_++] = val;
    }

    void pop() {
        --size_;
    }

    bool empty() {
        return size_ == 0;
    }

    size_t size() {
        return size_;
    }

    private:
    Count size_;
    T data_[Cap];
};




// current stack supports iteration!
template<class T>
struct stack {
    // can't inherit from vector iterator
    // too open class
    struct stack_iterator : std::iterator<std::input_iterator_tag, T> {
        const stack& stack_;
        typename std::vector<T>::const_iterator current_;
        
        stack_iterator(const stack& stack, typename std::vector<T>::const_iterator current) 
        : stack_(stack), current_(current) {}
        const T& operator*() const { return *current_; }
        bool operator==(const stack_iterator& it) const {
            return current_ == it;
        }
        bool operator!=(const stack_iterator& it) const {
            return current_ != it.current_;
        }
        stack_iterator& operator++() {
            ++current_;
            return *this;
        }
        // post iterator
        stack_iterator operator++(int) { 
            stack_iterator it(*this); 
            operator++(); 
            return it;
        }
    };

    friend class stack_iterator;
    using iterator = stack_iterator;

    iterator begin() const { return iterator(*this, data_.begin()); }
    iterator end()   const { return iterator(*this, data_.end()); }

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




template <class T>
class count_map : public std::map<T, size_t> {
public:
    void decrease(const T& t) { decrease(t, 1); }

    void decrease(const T& t, size_t val) {
        auto it = this->find(t);
        if ((it->second-=val) == 0) {
            this->erase(it);
        }
    }

    void increase(const T& t) { increase(t, 1); }

    void increase(const T& t, size_t val) {
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
struct circular_list {
    struct node {
        T value;
        node* prev;
        node* next;
    };

    template<class V>
    struct base_iterator : std::iterator<std::bidirectional_iterator_tag, V> {
        base_iterator(node* n) : node_(n) {} 
        
        const V& operator*() const { return node_->value; }
        V& operator*() { return node_->value; }
        
        const V* operator->() const { return node_->value; }
        V* operator->() { return &node_->value; }
        
        
        bool operator==(const base_iterator& it) const {
            return node_ == it.node_;
        }
        bool operator!=(const base_iterator& it) const {
            return node_ != it.node_;
        }
        base_iterator& operator++() {
            node_ = node_->next;
            return *this;
        }
        // post iterator
        base_iterator operator++(int) { 
            base_iterator it(node_); 
            node_ = node_->next; 
            return it;
        }
        
        base_iterator operator--() {
            node_ = node_->prev;
            return *this;
        }
        base_iterator operator--(int) {
            base_iterator it(node_);
            node_ = node_->prev;
            return it;
        }
        
    private:
        node* node_;
        friend struct circular_list;
    };

    using iterator = base_iterator<T>;
    using const_iterator = base_iterator<const T>;

    circular_list() : focus_(nullptr) {}

    void init_focus(const T& value) {
        focus_ = new node();
        focus_->value = value;
        focus_->prev = focus_->next = focus_;
    }

    iterator insert_after(iterator it_pos, const T& value) {
        ++count_;
        node *pos = it_pos.node_;
        if (pos == nullptr) {
            init_focus(value);
            return iterator(focus_);
        }
        node *prev = pos;
        node *next = pos->next;
        node *cur = new node();
        cur->next = next;
        cur->prev = prev;
        cur->value = value;
        next->prev = cur;
        prev->next = cur;
        return iterator(cur);
    }

    iterator insert_before(iterator it_pos, const T& value) {
        ++count_;
        node *pos = it_pos.node_;
        if (pos == nullptr) {
            init_focus(value);
            return iterator(focus_);
        }
        node *prev = pos->prev;
        node *next = pos;
        node *cur = new node();
        cur->next = next;
        cur->prev = prev;
        cur->value = value;
        prev->next = cur;
        next->prev = cur;
        return iterator(cur);
    }

    void erase(iterator it_pos) {
        --count_;
        node *pos = it_pos.node_;
        if (pos == focus_) focus_ = pos->next;
        pos->prev->next = pos->next;
        pos->next->prev = pos->prev;
        delete pos;
        if (count_ == 0) focus_ = nullptr;
    }

    iterator focus() {
        return iterator(focus_);
    }

    Count size() const {
        return count_;
    }

    bool empty() const {
        return focus_ == nullptr;
    }

    bool isFeasible() {
        auto p_end = focus_, p = focus_;
        if (p == nullptr) return true;
        bool result = true;
        while (true) {
            if (p->next == nullptr || p->prev == nullptr) {
                result = false;
            }
            p = p->next;
            if (p == p_end) break;
        } 
        return result;
    }

private:
    node* focus_;
    Count count_{0};
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




}


#endif