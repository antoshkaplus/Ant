#pragma once

//
// core types are considered similar to std
// that's why low case names used
// it's a huge honor to be low case underscore delimeter name
//

#include <vector>
#include <tuple>
#include <map>
#include <numeric>
#include <random>
#include <set>
#include <cassert>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <stack>
#include <memory>
#include <algorithm>
#include <sstream>
#include <cstring>
#include <type_traits>
#include <functional>
#include <chrono>
#include <list>
#include <array>
#include <cstdlib>


// need to specify template arguments explicitly somehow
template<class T, uint64_t N>
std::ostream& operator<<(std::ostream& o, const std::array<T, N>& arr) {
    o << "array: ";
    for (auto& a : arr) {
        o << a << " ";
    }
    return o << std::endl;
}

template<class T_1, class T_2>
std::ostream& operator<<(std::ostream& o, const std::pair<T_1, T_2>& p) {
    return o << "(" << p.first << "," << p.second << ")";
}


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
using Amount = int;
using Id = int;

using Long = int64_t;
using Float = double;
using Double = double;

using Duration = std::chrono::nanoseconds;

template <typename TOtherDuration>
Duration ToDuration(const TOtherDuration& duration) {
    return std::chrono::duration_cast<Duration>(duration);
}

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
TypeName(const TypeName&) = delete;      \
void operator=(const TypeName&) = delete


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

template <bool condition, typename returnType>
using ResultEnableIf = typename std::enable_if<condition, returnType>::type;



// to use with static_assert, to force parameter deduction first
template<class T> struct AlwaysFalse : std::false_type {};

// to use with visitor
// std::visit(overloaded {
//    [](auto arg) { std::cout << arg << ' '; },
//    [](double arg) { std::cout << std::fixed << arg << ' '; },
//    [](const std::string& arg) { std::cout << std::quoted(arg) << ' '; },
//}, v);
template<class... Ts> struct Overload : Ts... { using Ts::operator()...; };
template<class... Ts> Overload(Ts...) -> Overload<Ts...>;


template <template<typename ...Ts> typename Temp, int i, typename Type>
struct With {
};

template <template<typename T1, typename, typename ...Ts> typename Temp, typename Type_>
struct With<Temp, 2, Type_> {
    template <typename T1, typename ...Ts>
    using Type = Temp<T1, Type_, Ts...>;
};

auto Identity_ConstRef = [](const auto& item) -> const auto& {
    return item;
};

auto TrueFunctor = [](const auto&){ return true; };

unsigned GetMillisCount();

class Timer {
public:
    Timer(int millis) {
        end_millis_ = GetMillisCount() + millis;
    }

    template <class Rep, class Period>
    Timer(std::chrono::duration<Rep, Period> duration)
            : Timer(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()) {}

    int left() const {
        return end_millis_ - GetMillisCount();
    }

    bool timeout() const {
        return GetMillisCount() >= end_millis_;
    }

private:
    int end_millis_;
};

class Stopwatch {
public:
    Stopwatch() {
        start_millis_ = GetMillisCount();
    }

    int passed() const {
        return GetMillisCount() - start_millis_;
    }

private:
    int start_millis_;
};


enum class LoopControl : uint8_t {
    Continue,
    Break
};


// maybe some sort of data structure would be nice
inline Index next_ring_index(Index cur, Count elem_count) {
    return (cur + 1) % elem_count;
}

inline Index prev_ring_index(Index cur, Count elem_count) {
    return (cur - 1 + elem_count) % elem_count;
}

// better call it ring
class CircularIndexer {
public:
    CircularIndexer(Count elem_count)
        : elem_count_(elem_count) {}

    void Init(Count elem_count) {
        elem_count_ = elem_count;
    }

    Index next(Index index) {
        return next_ring_index(index, elem_count_);
    }
    Index prev(Index index) {
        return prev_ring_index(index, elem_count_);
    }

private:
    Count elem_count_;
};



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

template<class Key, class Value>
std::map<Value, Key> FlipMap(const std::map<Key, Value>& m) {
    std::map<Value, Key> flip_m;
    for (auto& p : m) {
        flip_m[p.second] = p.first;
    }
    return flip_m;
}

template<typename Container, typename Func>
void Flatten(const Container& nested, Func func) {
    for (auto& vs : nested) {
        for (auto& v : vs) {
            func(v);
        }
    }
};

template<class Key>
class KeyIndexer {
public:

    KeyIndexer() : newIndex_(0) {}

    bool exists(const Key& key) const {
        return m_.count(key) != 0;
    }

    Index index(const Key& key) {
        auto p = m_.emplace(key, newIndex_);
        if (p.second) {
            ++newIndex_;
        }
        return p.first->second;
    }

    Index index(const Key& key) const {
        return m_.at(key);
    }

    auto begin() {
        return m_.begin();
    }

    auto end() {
        return m_.end();
    }

    int size() const {
        return m_.size();
    }

private:
    Index newIndex_;
    std::map<Key, Index> m_;
};

// sometimes someone can use with Long, not just Int type
template<class T = Index>
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
            current_ += range_.step_;
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

    Index begin_index() const { return first_; }
    Index end_index() const { return last_; }

private:
    T first_, last_, step_;
};


// value should be immutable probably
template<class V>
struct TrailNode {
    const V value;
    const std::shared_ptr<TrailNode> previous;

    TrailNode(V value) : value(value) {}

    TrailNode(V value, const std::shared_ptr<TrailNode>& previous)
        : value(value), previous(previous) {}

    // current element considered last
    std::vector<V> Path() const {
        std::vector<V> r;
        const TrailNode* node = this;
        while (node != nullptr) {
            r.push_back(node->value);
            node = node->previous.get();
        }
        std::reverse(r.begin(), r.end());
        return r;
    }
};

template<class V>
using TrailNodePtr = std::shared_ptr<TrailNode<V>>;



class DisjointSet {
public:
    DisjointSet() {}
    DisjointSet(Count element_count) {
        Init(element_count);
    }

    void Init(Count element_count) {
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
        auto r_0 = root(i_0);
        auto r_1 = root(i_1);
        if (r_0 == r_1) return;
        --set_count_;
        // will join r_0 to r_1, so r_1 height should be bigger
        if (size_[r_0] > size_[r_1]) {
            std::swap(r_0, r_1);
        }
        data_[r_0] = r_1;
        size_[r_1] += size_[r_0];

    }

    Index Add() {
        Count sz = data_.size();
        data_.resize(sz + 1);
        size_.resize(sz + 1);
        data_.back() = sz;
        size_.back() = 1;
        ++set_count_;
        return sz;
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

    size_t size() const {
        return data_.size();
    }

    size_t root_size(Index i) {
        return size_[root(i)];
    }

    Count set_count() {
        return set_count_;
    }

private:
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

template<class Key, class Value>
using Map = std::map<Key, Value>;


template<class Key, class Value>
using UnorderedMap = std::unordered_map<Key, Value>;


// best possible approach for now. but later probably should change this

// probably should be an inheritance
template <class T, template<class, class> class M = Map>
class CountMap : public M<T, Count> {
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

    Count Total() const {
        Count total = 0;
        for (auto& p : *this) {
            total += p.second;
        }
        return total;
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
        friend class CircularList<T>;
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

    template<typename It, EnableIf<IsAnySame<It, Iterator, ConstIterator>> = enabler>
    It InsertAfter(It it_pos, const T& value) {
        ++count_;
        Node *pos = it_pos.node_;
        if (pos == nullptr) {
            InitFocus(value);
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
int atoi(const std::string& str);

int pow_2(int power);


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


// but also have to include last one
template<Count count>
std::array<std::string, count> Split(std::string str, char delimeter) {
    std::array<std::string, count> r;
    Index c = 0; // current index in array
    Index s_i = 0; // starting index for sustr
    for (Index i = 0; i < str.size(); ++i) {
        if (str[i] == delimeter) {
            r[c++] = str.substr(s_i, i-s_i);
            s_i = i+1;
        }
    }
    r[c] = str.substr(s_i);
    return r;
}

inline std::vector<std::string> Split(std::string str, char delim) {
    std::vector<std::string> r;
    Index s_i = 0; // starting index for sustr
    for (Index i = 0; i < str.size(); ++i) {
        if (str[i] == delim) {
            r.push_back(str.substr(s_i, i-s_i));
            s_i = i+1;
        }
    }
    r.push_back(str.substr(s_i));
    return r;
}


inline std::string ToLowerCase(const std::string& str) {
    auto res = str;
    std::transform(res.begin(), res.end(), res.begin(), ::tolower);
    return res;
}

inline void ToLowerCaseInPlace(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}


// trim from start
inline std::string& TrimLeft(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not_fn<int(int)>(std::isspace)));
    return s;
}

// trim from end
inline std::string& TrimRight(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not_fn<int(int)>(std::isspace)).base(), s.end());
    return s;
}

// trim from both ends
inline std::string& Trim(std::string& s) {
    return TrimLeft(TrimRight(s));
}

template <typename Iter>
std::string Join(Iter begin, Iter end, std::string const& separator)
{
    std::ostringstream result;
    if (begin != end) {
        result << *begin++;
    }
    while (begin != end) {
        result << separator << *begin++;
    }
    return result.str();
}



template <typename... Ts>
std::string Format_2(const std::string &fmt, Ts... vs)
{
    char b;
    unsigned required = std::snprintf(&b, 0, fmt.c_str(), vs...) + 1;
    // See comments: the +1 is necessary, while the first parameter
    //               can also be set to nullptr

    char bytes[required];
    std::snprintf(bytes, required, fmt.c_str(), vs...);

    return std::string(bytes);
}


template<typename ... Args>
std::string Format( const std::string& format, Args ... args ) {

    int size = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
    if (size < 0) throw std::runtime_error("ant core Format encoding error");
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args ...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

inline Duration ParseDuration(std::istream& in) {
    Duration result {};

    auto to_duration = [](const std::string& number, const std::string& suffix) {
        auto d = std::stoll(number);
        if (suffix == "h") return std::chrono::duration_cast<Duration>(std::chrono::hours(d));
        if (suffix == "m") return std::chrono::duration_cast<Duration>(std::chrono::minutes(d));
        if (suffix == "s") return std::chrono::duration_cast<Duration>(std::chrono::seconds(d));
        if (suffix == "ms") return std::chrono::duration_cast<Duration>(std::chrono::milliseconds(d));
        if (suffix == "us") return ToDuration(std::chrono::microseconds(d));
        if (suffix == "ns") return std::chrono::nanoseconds(d);
        throw std::runtime_error("duration suffix not recognized");
    };

    char ch;
    std::string number;
    std::string suffix;
    bool prev_space = true;
    for (;;) {
        if (!(in >> ch)) break;
        if (ch == ' ') {
            // can be number before: let's continue to suffix
            // can be suffix before: we no number is the and we close
            if (!number.empty() && !suffix.empty()) {
                result += to_duration(number, suffix);
                number = suffix = "";
            }
            prev_space = true;

        } else if (std::isdigit(ch)) {
            // can be space before
            //   if number was before - error
            //   if suffix not empty - add to duration
            // suffix before - add to duration

            if (!number.empty() && prev_space) {
                throw std::runtime_error("duration suffix skipped");
            }

            if (!suffix.empty()) {
                result += to_duration(number, suffix);
                number = suffix = "";
            }
            number += ch;

            prev_space = false;
        } else if (std::isalpha(ch)) {
            // can be space before
            //   if suffix before - error
            //   if number before - ok

            if (!suffix.empty() && prev_space) {
                throw std::runtime_error("duration number skipped");
            }

            if (number.empty()) throw std::runtime_error("no number before duration suffix");

            suffix += ch;

            prev_space = false;
        } else {
            throw std::runtime_error("unexpected character");
        }
    }
    if (!number.empty() && !suffix.empty()) {
        result += to_duration(number, suffix);
        number = suffix = "";
    }
    if (number.empty() != suffix.empty()) {
        throw std::runtime_error("unexpected");
    }

    return result;
}

inline Duration ParseDuration(const std::string& str) {
    std::istringstream in(str);
    return ParseDuration(in);
}

inline std::string FormatDuration(Duration d) {
    std::string res;

    auto r = std::div(d.count(), static_cast<int64_t>(1000));
    if (r.rem) res = std::to_string(r.rem) + "ns" + res;
    r = std::div(r.quot, static_cast<int64_t>(1000));
    if (r.rem) res = std::to_string(r.rem) + "us" + res;
    r = std::div(r.quot, static_cast<int64_t>(1000));
    if (r.rem) res = std::to_string(r.rem) + "ms" + res;
    r = std::div(r.quot, static_cast<int64_t>(60));
    if (r.rem) res = std::to_string(r.rem) + "s" + res;
    r = std::div(r.quot, static_cast<int64_t>(60));
    if (r.rem) res = std::to_string(r.rem) + "m" + res;
    if (r.quot) res = std::to_string(r.quot) + "h" + res;

    if (res.empty()) res = "0ns";
    return res;
}



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

struct LogicalBinarySearch {
    // we expect integer over here
    // smallest value that satisfies condition
    template<class P, class Cond>
    static P Min(P a, P b, const Cond&& cond) {

        auto y_a = cond(a);
        auto y_b = cond(b);
        if (y_a) {
            return a;
        }
        if (!y_b) {
            throw std::runtime_error("no solution");
        }

        while (a != b) {
            // we want to hit first if two are left
            auto m = (a + b)/2;
            if (cond(m)) {
                b = m;
            } else {
                a = m+1;
            }
        }
        return a;
    }

    // biggest value that satisfies condition
    template<class P, class Cond>
    static P Max(P a, P b, const Cond&& cond) {

        auto y_a = cond(a);
        auto y_b = cond(b);
        if (y_b) {
            return b;
        }
        if (!y_a) {
            throw std::runtime_error("no solution");
        }

        while (a != b) {
            // we want to hit last if two are left
            auto m = (a + b + 1)/2;
            if (cond(m)) {
                a = m;
            } else {
                b = m;
            }
        }
        return a;
    }
};



// TODO special hash function for multi dimentional indices
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

template<class T>
uint64_t Hash(T c_0, T c_1) {
    uint64_t r = 0;
    r += c_0;
    r <<= 32;
    r += c_1;
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


// reason: you have an ordered container of values. values consequently repeat.
// you want to know indexes when each new value starts

// index when starts new value
// end goes in for simplicity
// that means last index is always inside
template<class ForwardIt, class Equals>
std::vector<Index> ValueBounds(ForwardIt begin, ForwardIt end, Equals& equals) {
    std::vector<Index> bounds;
    Index i = 0;
    ForwardIt prev = begin;
    for (ForwardIt it = begin; it != end; ++it) {
        if (!equals(it, prev)) {
            bounds.push_back(i);
        }
        prev = it;
        ++i;
    }
    bounds.push_back(i);
    return bounds;
}

// with this one we compare values that are coming from Score callable object
template<class ForwardIt, class Score>
ForwardIt MinElement(ForwardIt begin, ForwardIt end, Score&& score) {
    assert(begin != end); // no elements
    ForwardIt min = begin;
    decltype(score(*min)) S = score(*min), S_new;
    while (++begin != end) {
        S_new = score(*begin);
        if (S_new < S) {
            min = begin;
            S = S_new;
        }
    }
    return min;
}


// with this one we compare values that are coming from Score callable object
template<class ForwardIt, class Score>
ForwardIt MaxElement(ForwardIt begin, ForwardIt end, Score&& score) {
    assert(begin != end); // no elements
    ForwardIt max = begin;
    decltype(score(*max)) S = score(*max), S_new;
    while (++begin != end) {
        S_new = score(*begin);
        if (S_new > S) {
            max = begin;
            S = S_new;
        }
    }
    return max;
}


template<class T, Count N>
void Println(std::ostream& o, const std::array<T, N>& arr) {
    o << "array: ";
    for (auto& a : arr) {
        o << a << " ";
    }
    o << std::endl;
}

template<class T>
void Println(std::ostream& o, const std::vector<T>& arr) {
    for (auto& a : arr) {
        o << a << " ";
    }
    o << std::endl;
}

template<class T>
void Println(std::ostream& o, const std::set<T>& arr) {
    for (auto& a : arr) {
        o << a << " ";
    }
    o << std::endl;
}


template<class ForwardIt>
void PrintlnSequence(std::ostream& o, ForwardIt first, ForwardIt last, std::string_view title = "") {
    if (!title.empty()) {
        o << title << ": ";
    }
    for (auto it = first; it != last; ++it) {
        o << *it << " ";
    }
    o << std::endl;
}


template<class Connector, class ItemId, ItemId NoneItemId>
class AdjacentItems {

    using Array = std::array<ItemId, 2>;
    using Neighbors = std::unordered_map<Connector, Array>;

public:

    void Replace(const Connector& e, ItemId from, ItemId to) {
        auto& ni = neighbors_[e];
        assert(ni[0] == from || ni[1] == from);
        std::swap(ni[0] == from ? ni[0] : ni[1], to);
    }

    void Remove(const Connector& e) {
        neighbors_.erase(e);
    }

    void Insert(const Connector& e, ItemId i_0, ItemId i_1) {
        neighbors_[e] = {{i_0, i_1}};
    }

    void Insert(const Connector& e, ItemId i) {
        auto it = neighbors_.find(e);
        if (it == neighbors_.end()) {
            neighbors_[e] = {{ i, NoneItemId }};
        } else {
            auto& n = it->second;
            char ch = (n[0] == NoneItemId) ? 0 : 1;
            n[ch] = i;
        }
    }

    bool Empty() const {
        return neighbors_.empty();
    }

    ItemId another(const Connector& e, ItemId id) const {
        auto& ni = neighbors_.at(e);
        return ni[0] == id ? ni[1] : ni[0];
    }

    const Array& operator[](const Connector& e) const {
        return neighbors_.at(e);
    }

    auto begin() const {
        return neighbors_.begin();
    }

    auto end() const {
        return neighbors_.end();
    }

private:
    Neighbors neighbors_;

};


template<class T>
void SwapBackPop(std::vector<T>& v, Index i) {
    std::swap(v[i], v.back());
    v.pop_back();
}


// res - element per cluster, which are available
//
// it's invalid input to have partial usage of a cluster
// indexes of clusters shift accordingly
//
// note: we use word "clustering" as it describes set of
// elements distributed into clusters. "descrease" means
// that we take out some of the clusters
void DecreaseClustering(std::vector<Index>& belong, const std::vector<bool>& res);

void DecreaseClustering(std::vector<Index>& belong);

template<class T>
void Println(std::ostream& out, const T& v) {
    out << v << std::endl;
}

template<class T, class ...Args>
void Println(std::ostream& out, const T& v, Args&&... args) {
    out << v;
    Println(out, std::forward<Args>(args)...);
}

#ifdef ANT_LOG_TO_FILE

static std::ofstream ANT_LOG_FILE("my.log");
template<class T>
void Log(const T& v) {
    ANT_LOG_FILE << v << std::endl;
}

template<class T, class ...Args>
void Log(const T& v, Args... args) {
    ANT_LOG_FILE << v;
    Log(args...);
}

#else

inline void Log(...) {}

#endif


// number of digits in int 0 is 0
template<class T>
Count CountDigits(T t) {
    Count count = 0;
    while (t != 0) {
        t /= 10;
        ++count;
    }
    return count;
}


// Longest Increasing Subsequence
// returns index vector
// can implement algorithm with O(NlonN). current is O(N^2)
template<class T>
std::vector<int> LIS(std::vector<T>& arr ) {
    std::vector<int> lis(arr.size(), 1);

    for (int i = 1; i < arr.size(); i++ ) {
        for (int j = 0; j < i; j++ ) {
            if ((arr[i] > arr[j]) && lis[i] < lis[j] + 1) {
                lis[i] = lis[j] + 1;
            }
        }
    }

    int max_end = max_element(lis.begin(), lis.end()) - lis.begin();
    int max = lis[max_end];
    std::vector<int> res(max);

    // backtracking to fill res with indexes
    int cur = max;
    int cur_elem = arr[max_end];
    res[cur-1] = max_end;
    for (int i = max_end-1; i >= 0; --i) {
        if (lis[i] == cur-1 && arr[i] < cur_elem) {
            --cur;
            res[cur-1] = i;
            cur_elem = arr[i];
        }
    }

    return res;
}


class Stats {
    double min_ = std::numeric_limits<double>::max();
    double max_ = std::numeric_limits<double>::min();
    double total_ = 0;
    Count count_ = 0;

public:
    double min() const {
        return min_;
    }
    double max() const {
        return max_;
    }
    double total() const {
        return total_;
    }
    double average() const {
        return total_/count_;
    }
    void add(double val) {
        min_ = std::min(min_, val);
        max_ = std::max(max_, val);
        total_ += val;
        ++count_;
    }
    bool empty() const {
        return count_ == 0;
    }
};

// would be cool to have shared object somewhere
template<class T>
class NestedVectors {
public:

    // use this in case multiple insertions (addings) per container item
    struct ItemScope {
        ItemScope(NestedVectors& nv)
                : nv(nv), current_index(nv.indices_.back()) {}

        ~ItemScope() {
            while (nv.indices_.back() != current_index) {
                nv.indices_.pop_back();
            }
            if (current_index != nv.data_.size()) {
                nv.indices_.push_back(nv.data_.size());
            }
        }

        NestedVectors& nv;
        Index current_index;
    };

    NestedVectors() {
        indices_.push_back(0);
    }

    template<class It>
    void add(It first, It last) {
        if (first == last) return;
        data_.insert(data_.end(), first, last);
        indices_.push_back(data_.size());
    }

    void add(T t) {
        data_.push_back(t);
        indices_.push_back(data_.size());
    }

    int item_size() const {
        return data_.size();
    }

    int size() const {
        return indices_.size()-1;
    }

    std::pair<Index, Index> range(Index i) const {
        return {indices_[i], indices_[i+1]};
    };

    const T& operator[](Index i) const {
        return data_[i];
    }

    T& operator[](Index i) {
        return data_[i];
    }

private:
    std::vector<T> data_;
    std::vector<Index> indices_;
};


template <typename TValue>
struct MinMax {
    TValue min {};
    TValue max {};

    template<typename Q = TValue, typename std::enable_if<std::is_arithmetic<Q>::value>::type* = nullptr>
    MinMax() : min(std::numeric_limits<Q>::max()), max(std::numeric_limits<Q>::min()) {}

    MinMax(TValue min, TValue max) : min(std::move(min)), max(std::move(max)) {}

    MinMax<TValue>& operator+=(const MinMax<TValue>& other) {
        min = std::min(min, other.min);
        max = std::max(max, other.max);
        return *this;
    }

    MinMax<TValue>& operator+=(const TValue& value) {
        min = std::min(min, value);
        max = std::max(max, value);
        return *this;
    }
};

template <typename Value, size_t kCapacity>
class FixedVector {
    Count size_ {};
    alignas( alignof(Value) ) char array[sizeof(Value) * kCapacity];

public:
    FixedVector() {}

    FixedVector(FixedVector&& fv) {
        *this = std::move(fv);
    }

    ~FixedVector() {
        for (auto i = 0; i < size_; ++i) {
            (*this) [i] . ~Value();
        }
    }

    bool push_back(Value value) {
        if (size_ == kCapacity) return false;
        new ( array[sizeof(Value) * (size_++)] ) Value (std::move(value));
    }

    auto& operator=(FixedVector&& fv) {
        for (auto i = 0; i < std::min(size_, fv.size_); ++i) {
            array[i] = fv[i];
        }
        for (auto i = size_; i < fv.size_; ++i) {
            new ( array[sizeof(Value) * (size_++)] ) Value (std::move(fv[i]));
        }
        for (auto i = fv.size_; i < size_; ++i) {
            (*this) [i] . ~Value();
        }
        size_ = fv.size_;
        return *this;
    }

    bool empty() const {
        return size_ == 0;
    }

    Count size() const {
        return size_;
    }

    Value& operator[](Count index) {
        return reinterpret_cast< Value& > (array[sizeof(Value) * index]);
    }

    const Value& operator[](Count index) const {
        return reinterpret_cast< Value& > (array[sizeof(Value) * index]);
    }
};

struct StreamMean {
    double mean_;
    Count count_;

    void Add(double value) {
        mean_ = (mean_ * count_ + value) / (count_ + 1);
        ++count_;
    }

    double mean() const {
        return mean_;
    }

    Count count() const {
        return count_;
    }
};

} // end namespace ant

// enum operations
template<class TEnum, typename std::enable_if<std::is_enum<TEnum>::value>::type* = nullptr>
constexpr TEnum operator& (const TEnum left, const TEnum right) {
    using enum_t = typename std::underlying_type<TEnum>::type;
    return static_cast<TEnum>(static_cast<enum_t>(left) & static_cast<enum_t>(right));
}

template<class TEnum, typename std::enable_if<std::is_enum<TEnum>::value>::type* = nullptr>
constexpr TEnum operator&= (const TEnum left, const TEnum right) {
    left = left & right;
    return left;
}

template<class TEnum, typename std::enable_if<std::is_enum<TEnum>::value>::type* = nullptr>
constexpr TEnum operator| (const TEnum left, const TEnum right) {
    using enum_t = typename std::underlying_type<TEnum>::type;
    return static_cast<TEnum>(static_cast<enum_t>(left) | static_cast<enum_t>(right));
}

template<class TEnum, typename std::enable_if<std::is_enum<TEnum>::value>::type* = nullptr>
constexpr TEnum operator|= (const TEnum left, const TEnum right) {
    left = left | right;
    return left;
}

template<class TEnum, typename std::enable_if<std::is_enum<TEnum>::value>::type* = nullptr>
constexpr TEnum operator^ (const TEnum left, const TEnum right) {
    using enum_t = typename std::underlying_type<TEnum>::type;
    return static_cast<TEnum>(static_cast<enum_t>(left) ^ static_cast<enum_t>(right));
}

template<class TEnum, typename std::enable_if<std::is_enum<TEnum>::value>::type* = nullptr>
constexpr TEnum operator^= (const TEnum left, const TEnum right) {
    left = left ^ right;
    return left;
}

template<class TEnum, typename std::enable_if<std::is_enum<TEnum>::value>::type* = nullptr>
constexpr TEnum operator~ (const TEnum value) {
    using enum_t = typename std::underlying_type<TEnum>::type;
    return static_cast<TEnum>(~static_cast<enum_t>(value));
}

namespace ant {

template<typename ...Iterators>
class ZipIterator {
public:
    ZipIterator(std::tuple<Iterators...>& it) : it(it) {}

    ZipIterator(const Iterators& ... its) : it(std::make_tuple(its...)) {}

    ZipIterator& operator++() {
        std::apply([](auto& ...xs) { (++xs, ...); }, it);
        return *this;
    }

    auto operator*() {
        return std::apply([](auto&& ...xs) constexpr { return std::tie(*xs...); }, it);
    }

    bool operator==(const ZipIterator& it) const {
        return this->it == it.it;
    }

    bool operator!=(const ZipIterator& it) const {
        return this->it != it.it;
    }

public:
    std::tuple<Iterators...> it;
};

template<typename ...Iterators>
class iZipIterator {
public:
    iZipIterator(std::tuple<Iterators...>& it) : it(it) {}

    iZipIterator(const Iterators& ... its) : it(std::make_tuple(its...)) {}

    iZipIterator& operator++() {
        ++index;
        std::apply([](auto& ...xs) { (++xs, ...); }, it);
        return *this;
    }

    auto operator*() {
        return std::tuple_cat(std::make_tuple(index), std::apply([](auto& ...xs) { return std::tie(*xs...); }, it));
    }

    bool operator==(const iZipIterator& it) const {
        return this->it == it.it;
    }

    bool operator!=(const iZipIterator& it) const {
        return this->it != it.it;
    }

public:
    Index index = 0;
    std::tuple<Iterators...> it;
};

template<typename IteratorBegin, typename IteratorEnd>
struct IteratorRange {
    IteratorRange(IteratorBegin it_begin, IteratorEnd it_end)
            : it_begin(it_begin), it_end(it_end) {}

    auto begin() {
        return it_begin;
    }

    auto end() {
        return it_end;
    }

    auto begin() const {
        return it_begin;
    }

    auto end() const {
        return it_end;
    }

private:
    IteratorBegin it_begin;
    IteratorEnd it_end;
};

template<typename ...Containers>
auto ZipRange(Containers& ... containers) {
    return IteratorRange(ZipIterator(containers.begin()...), ZipIterator(containers.end()...));
}

template<typename ...Containers>
auto iZipRange(Containers& ... containers) {
    return IteratorRange(iZipIterator(containers.begin()...), iZipIterator(containers.end()...));
}

}