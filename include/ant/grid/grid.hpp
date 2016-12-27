
#ifndef __ANT__GRID_H__
#define __ANT__GRID_H__

#include <algorithm>
#include <unordered_map>
#include <cassert>
#include <iostream>
#include <list>
#include <bitset>
#include <vector>
#include <random>
#include <limits>
#include <array>


#include "ant/core/core.hpp"
#include "ant/geometry/d2.hpp"

namespace ant {
    
namespace grid {

//constexpr int kSideTop = 0;
//constexpr int kSideBottom = 1;
//constexpr int kSideRight = 2;
//constexpr int kSideLeft = 3;
//kOppositeSide = ;


// this is int to not 


using Direction = int;

constexpr Direction kDirUp = 0;
constexpr Direction kDirDown = 1;
constexpr Direction kDirRight = 2;
constexpr Direction kDirLeft = 3;    
constexpr const std::array<Direction, 4> kDirOpposite = { {
    kDirDown, 
    kDirUp, 
    kDirLeft, 
    kDirRight
} };
constexpr const std::array<std::array<Direction, 2>, 4> kDirTurn = { {
    {{ kDirLeft, kDirRight }},
    {{ kDirLeft, kDirRight }},
    {{ kDirDown, kDirUp }},
    {{ kDirDown, kDirUp }}
} };
constexpr const std::array<Direction, 4> kDirClockwise = { {
    kDirRight,
    kDirLeft,
    kDirDown,
    kDirUp
} };
constexpr const std::array<Direction, 4> kDirAntiClockwise = { {
    kDirLeft,
    kDirRight,
    kDirUp,
    kDirDown
} };


constexpr const std::array<Direction, 4> kDirections = { { 
    kDirUp,
    kDirDown,
    kDirRight,
    kDirLeft
} };


constexpr const Count kDirCount = 4;


struct Indent;

struct Size {
    Int row, col;
    
    Size() : row(0), col(0) {}
    Size(Int row, Int col)
    : row(row), col(col) {}
    explicit Size(const Indent& indent);
    
    void set(Int row, Int col) {
        this->row = row;
        this->col = col;
    }
    
    Count cell_count() const {
        return row*col;
    }
    
    Size swapped() const {
        return Size(col, row);
    }
    
    void swap() {
        std::swap(row, col);
    }
    
    static Size min(Size s_0, Size s_1) {
        return {std::min(s_0.row, s_1.row), std::min(s_0.col, s_1.col)};
    }
};

bool operator==(const Size& s_0, const Size& s_1);
bool operator!=(const Size& s_0, const Size& s_1);
Size operator-(const Size& s_0, const Size& s_1);
inline std::istream& operator>>(std::istream& in, Size& sz) {
    return in >> sz.row >> sz.col;
}

// make it possible to substruct
struct Indent {
    constexpr Indent() : Indent(0, 0) {}
    constexpr Indent(Int row, Int col) : row(row), col(col) {}
    explicit Indent(const Size& sz) : Indent(sz.row, sz.col) {}
    
    void set(Int row, Int col) {
        this->row = row;
        this->col = col;
    }
    
    Int area() const {
        return row*col;
    }
    
    Int row, col;
};


// maybe don't need that as I alredy have kDirVector
constexpr const std::array<Indent, 4> kDirShift { {
    { -1, 0 },
    {  1, 0 },
    {  0, 1 },
    {  0,-1 }
} };


bool operator!=(const Indent& d_0, const Indent& d_1);
bool operator==(const Indent& d_0, const Indent& d_1);
inline Indent& operator*=(Indent& ind, int t) {
    ind.row *= t;
    ind.col *= t;
    return ind;
} 

// don't really know what to do with it
//template<class T>
//struct Position {
//    using value_type = T;
//        
//    T row, col;
//    
//    Position() {}
//    Position(T row, T col) : row(row), col(col) {}
//    
//    void set(Int row, Int col) {
//        this->row = row;
//        this->col = col;
//    }
//    
//    void shift(Int row, Int col) {
//        this->row += row;
//        this->col += col;
//    }
//    
//    void swap() {
//        std::swap(row, col);
//    }
//    Position swapped() const {
//        return Position(col, row);
//    }
//    
//    Position shifted(Int row_shift, Int col_shift) const {
//        return {row + row_shift, col + col_shift};
//    }
//    
//    struct TopLeftComparator {
//        bool operator()(const Position& p_0, const Position& p_1) {
//            return p_0.row < p_1.row || (p_0.row == p_1.row && p_0.col < p_1.col);
//        }
//    };
//    struct BottomRightComparator {
//        bool operator()(const Position& p_0, const Position& p_1) {
//            return p_0.row > p_1.row || (p_0.row == p_1.row && p_0.col > p_1.col);
//        }
//    };
//};


// don't know where to put it if anywhere else
constexpr const std::array<Indent, 4> kDirVector = { {
    { -1, 0 },
    {  1, 0 },
    {  0, 1 },
    {  0,-1 }
} }; 

Direction FromDirVector(Indent ind);

struct Position;

    
Position operator-(const Position& p, const Indent& n);
Position operator+(const Position& p, const Indent& n);        
Position operator-(const Indent& n, const Position& p);
Position operator+(const Indent& n, const Position& p);        
bool operator==(const Position& p_0, const Position& p_1);        
bool operator!=(const Position& p_0, const Position& p_1);
Position& operator+=(Position& p, const Size& s);
Indent operator-(const Position& p_0, const Position& p_1);
Position Centroid(const Position& p_0, const Position& p_1);
Indent operator+(const Indent& n_0, const Indent& n_1);


struct Position {
    // operators see below
    constexpr Position() : Position(0, 0) {}
    constexpr Position(Int row, Int col) : row(row), col(col) {}
    
    //Position(Int row, Int col) : row(row), col(col) {}
    
    void set(Int row, Int col) {
        this->row = row;
        this->col = col;
    }
    
    void shift(Int row, Int col) {
        this->row += row;
        this->col += col;
    }
    
    Position& operator+=(const Indent& indent) {
        row += indent.row;
        col += indent.col;
        return *this;
    }
    
    Position& operator=(const Position& p) {
        row = p.row;
        col = p.col;
        return *this;
    }
    
    Int ManhattanDistance(const Position& p) const {
        auto ind = p - *this;
        return std::abs(ind.row) + std::abs(ind.col);
    }
    
    void Shift(Direction dir) {
        *this += kDirVector[dir];
    }
    
    void swap() {
        std::swap(row, col);
    }
    Position swapped() const {
        return Position(col, row);
    }
    
    Position shifted(Int row_shift, Int col_shift) const {
        return {row + row_shift, col + col_shift};
    }
    
    Position Shifted(Direction dir) const;
        
    Int row, col;
    
    struct TopLeftComparator {
        bool operator()(const Position& p_0, const Position& p_1) {
            return p_0.row < p_1.row || (p_0.row == p_1.row && p_0.col < p_1.col);
        }
    };
    struct BottomRightComparator {
        bool operator()(const Position& p_0, const Position& p_1) {
            return p_0.row > p_1.row || (p_0.row == p_1.row && p_0.col > p_1.col);
        }
    };
};   


inline std::istream& operator>>(std::istream& in, Position& p) {
    return in >> p.row >> p.col;
}
inline std::ostream& operator<<(std::ostream& out, const Position& p) {
    return out << p.row << " " << p.col;
} 


struct Region {
    
    class Iterator : public std::iterator<std::input_iterator_tag, Position> {
    private:
        const Region& region_;
        Position current_;
    public:
    
        Iterator(const Region& region, Position current) 
        : region_(region), current_(current) {}
        
        const Position& operator*() const { return current_; }
        
        bool operator==(const Iterator& it) const {
            return current_ == it.current_;
        }
        bool operator!=(const Iterator& it) const {
            return current_ != it.current_;
        }
        Iterator& operator++() {
            if (current_.col != region_.col_end()-1) {
                current_.col += 1;   
            }
            else {
                current_.row += 1;
                current_.col = 0;
            }
            return *this;
        }
        // post iterator
        Iterator operator++(int) { 
            Iterator it(*this); 
            operator++(); 
            return it;
        }
    };
    

    Region() : Region(0, 0, 0, 0) {}
    Region(Int row, Int col, Int row_count, Int col_count)
    : position(row, col), size(row_count, col_count) {}
    Region(const Position& p, const Size& d) 
    : position(p), size(d) {}
    Region(Size size) : Region({0, 0}, size) {}
    Region(const Position& topleft, const Position& botright) 
    : position(topleft), size(botright-topleft+Indent{1,1}) {}
    
    void set(Int row, Int col, Int row_indent, Int col_indent) {
        position.set(row, col);
        size.set(row_indent, col_indent);
    }
    void set(const Position& p, const Size& d) {
        position = p;
        size = d;
    }
    
    void shift(Int row, Int col) {
        position.row += row;
        position.col += col;
    } 
    
    Iterator begin() const {
        return Iterator{*this, {0, 0}};
    }
    
    Iterator end() const {
        return Iterator{*this, {row_end(), 0}};
    }
    
    
    // maybe someone will find those getters useful 
    Int row_begin() const { return position.row; }
    Int row_end()   const { return position.row + size.row; }
    Int col_begin() const { return position.col; }
    Int col_end()   const { return position.col + size.col; }
    
    Int col_count() const { return size.col; }
    Int row_count() const { return size.row; }
    
    void set_row_begin(Int r) { 
        size.row += position.row - r; 
        position.row = r; 
    }
    void set_row_end(Int r) { 
        size.row = r - position.row; 
    }
    void set_col_begin(Int c) { 
        size.col += position.col - c;
        position.col = c; 
    }
    void set_col_end(Int c) { 
        size.col = c - position.col; 
    }
    
    bool hasInside(const Position& p) const {
        return p.row >= position.row && 
        p.row  < position.row+size.row &&
        p.col >= position.col &&
        p.col  < position.col+size.col;
    }
    bool hasInside(const Region& region) const {
        return row_begin() <= region.row_begin() 
        && col_begin() <= region.col_begin()
        && row_end() >= region.row_end()
        && col_end() >= region.col_end();
    }
    
    Region intersection(const Region& r) const {
        Region n;
        n.position.row = std::max(position.row, r.position.row);
        n.position.col = std::max(position.col, r.position.col);
        n.size.row = std::min(row_end(), r.row_end()) - n.position.row;
        n.size.col = std::min(col_end(), r.col_end()) - n.position.col;
        
        if (n.size.row < 0 || n.size.col < 0) {
            n.size.row = 0;
            n.size.col = 0;
        }
        return n;
    }
    
    template<class Process> 
    void ForEach(Process& proc) const {
        for (Index r = position.row; r < position.row+size.row; ++r) {
            for (Index c = position.col; c < position.col+size.col; ++c) {
                proc({r, c});
            }
        }
    }
    
    bool hasIntersection(const Region& r) const {
        return !intersection(r).isEmpty();
    }
    
    bool isEmpty() const { return size.row == 0 || size.col == 0; }
    
    template<class ForwardIterator>
    static Region unite(ForwardIterator first, ForwardIterator last) {
        Region rect = *first;
        while (++first != last) {
            // should update "end" first because it dependent on "begin"
            rect.set_row_end(std::max(first->row_end(), rect.row_end()));
            rect.set_col_end(std::max(first->col_end(), rect.col_end()));
            rect.set_row_begin(std::min(first->row_begin(), rect.row_begin()));
            rect.set_col_begin(std::min(first->col_begin(), rect.col_begin()));
            
        }
        return rect;
    }
    
    void Unite(const Region& r) {
        set_row_end(std::max(r.row_end(), row_end()));
        set_col_end(std::max(r.col_end(), col_end()));
        set_row_begin(std::min(r.row_begin(), row_begin()));
        set_col_begin(std::min(r.col_begin(), col_begin()));
    }
    
    Region United(const Region& r) {
        Region rr = *this;
        rr.Unite(r);
        return rr;
    }
    
    Int cell_count() const {
        return size.row*size.col;
    }
    
    Position position;
    Size size;
};

std::ostream& operator<<(std::ostream& output, const Region& r);
bool operator==(const Region& r_0, const Region& r_1);



// G - grid
template<class G>
struct GridView {

    using value_type = typename G::value_type;
    using const_reference = const value_type&;
private:
    using real_type = typename std::conditional<std::is_const<G>::value, const value_type, value_type>::type;
public:
    
    
    GridView(std::shared_ptr<G> grid, const Position& origin) : grid_(grid), origin_(origin) {}
    GridView() : origin_(0, 0) {}
    
    const_reference operator()(const Position& p) const {
        return (*grid_)(p + origin_);
    }
    const_reference operator()(Index row, Index col) const {
        return (*grid_)(row + origin_.row, col + origin_.col);
    }
    
    
    real_type operator()(const Position& p) {
        return (*grid_)(p + origin_);
    }
    real_type operator()(Index row, Index col) {
        return (*grid_)(row + origin_.row, col + origin_.col);
    }
    
    
    void set_origin(const Position& p) {
        origin_ = p;
    }
    
    const Size& size() const {
        return grid_->size();
    }
    
private:

    
    std::shared_ptr<G> grid_;
    Position origin_;
};

template<class G>
class OriginGrid {
public:
    using value_type = typename G::value_type;

    OriginGrid() {}
    OriginGrid(Position origin, Size size) : origin_(origin), grid_(size) {}
  
    
    value_type& operator()(const Position& p) {
        return grid_(p.row - origin_.row, p.col - origin_.col);
    }
    const value_type& operator()(const Position& p) const {
        return grid_(p.row - origin_.row, p.col - origin_.col);
    }
    
    value_type& operator[](const Position& p) {
        return grid_(p.row - origin_.row, p.col - origin_.col);
    }
    const value_type& operator[](const Position& p) const {
        return grid_(p.row - origin_.row, p.col - origin_.col);
    }
    
    value_type& operator()(Int row, Int col) {
        return grid_(row - origin_.row, col - origin_.col);
    }
    const value_type& operator()(Int row, Int col) const {
        return grid_(row - origin_.row, col - origin_.col);
    }

    
    G& grid() {
        return grid_;
    }
    
    const G& grid() const {
        return grid_;
    }
    
    void set_grid(G&& grid) {
        grid_ = std::move(grid);
    } 
    
    void set_grid(const G& grid) {
        grid_ = grid;
    }
    
    void set_origin(const Position& p) {
        origin_ = p;
    }
    
    Position& origin() {
        return origin_;
    }
    const Position origin() const {
        return origin_;
    }
    
private:
    Position origin_;
    G grid_;
};

template<class T>
struct Grid {
    using value_type = T;
    
    typename std::vector<T>::const_iterator begin() const {
        return grid_.begin();
    }
    typename std::vector<T>::iterator begin() {
        return grid_.begin();
    }
    typename std::vector<T>::const_iterator end() const {
        return grid_.end();
    }
    typename std::vector<T>::iterator end() {
        return grid_.end();
    }
    
    Grid() : Grid(0, 0) {}
    Grid(Count row_count, Count col_count)
    :   row_count_(row_count), 
        col_count_(col_count), 
        grid_(row_count*col_count) {}
    
    Grid(Count row_count, Count col_count, const T& value_type) 
    :   row_count_(row_count),
        col_count_(col_count),
        grid_(row_count*col_count, value_type) {}
                
    Grid(const Size& size) : Grid(size.row, size.col) {}
    Grid(std::initializer_list<std::vector<T>> list)
    :   Grid(list.size(), list.size() == 0 ? 0 : list.begin()->size()){
        auto b = list.begin();
        for (auto r = 0; r < row_count(); ++r, ++b) {
            std::copy(b->begin(), b->end(), grid_.begin() + r*col_count());
        }
    }
    
    bool isInside(const Position& p) const {
        return isInside(p.row, p.col);
    }
    bool isInside(Int row, Int col) const {
        return row >= 0 && row < row_count_ && 
        col >= 0 && col < col_count_;
    }
    
    
    bool IsInside(const Position& p) const {
        return isInside(p.row, p.col);
    }
    bool IsInside(Int row, Int col) const {
        return row >= 0 && row < row_count_ && 
        col >= 0 && col < col_count_;
    }
    
    void resize(Count row_count, Count col_count) {
        row_count_ = row_count;
        col_count_ = col_count;
        grid_.resize(row_count*col_count);
    }
    void resize(const Size& size) {
        resize(size.row, size.col);
    }
    
    void fill(const T& t) {
        std::fill(grid_.begin(), grid_.end(), t);
    }
    
    Count row_count() const { return row_count_; }
    Count col_count() const { return col_count_; }
    Size size() const { return Size(row_count(), col_count()); } 
    Count cell_count() const { return row_count()*col_count(); } 
    
    T& operator()(const Position& p) {
        return grid_[p.row*col_count_ + p.col];
    }
    const T& operator()(const Position& p) const {
        return grid_[p.row*col_count_ + p.col];
    }
    
    T& operator[](const Position& p) {
        return grid_[p.row*col_count_ + p.col];
    }
    const T& operator[](const Position& p) const {
        return grid_[p.row*col_count_ + p.col];
    }
    
    T& operator()(Int row, Int col) {
        return grid_[row*col_count_ + col];
    }
    
    const T& operator()(Int row, Int col) const {
        return grid_[row*col_count_ + col];
    }
    
    // function takes in element
    template<class Process>
    void ForEach(Process& proc) {
        for (Index r = 0; r < row_count(); ++r) {
            for (Index c = 0; c < col_count(); ++c) {
                proc((*this)(r, c));
            }
        }
    }
    
    // proc args:
    //      grid itself
    //      from direction
    //      position to process
    template<class Process>
    void ForEachNearby(const Position& t, Process& proc) {
        if (t.row > 0) {
            proc(*this, kDirDown, t+Indent{-1, 0});
        }
        if (t.row < row_count_-1) {
            proc(*this, kDirUp, t+Indent{ 1, 0});
        }
        if (t.col > 0) {
            proc(*this, kDirRight, t+Indent{ 0,-1});
        }
        if (t.col < col_count()-1) {
            proc(*this, kDirLeft, t+Indent{ 0, 1});
        }
    }
    
    // counts diagonal elements too
    template<class Proc>
    void ForEachAround(const Position& t, Proc& proc) {
        for (auto dr = -1; dr <= 1; ++dr) {
            for (auto dc = -1; dc <= 1; ++dc) {
                auto p = t.shifted(dr, dc);
                if (isInside(p)) proc(p, t);
            }
        }
    }
    
    // proc arg: const Position&
    template<class Process> 
    void ForEachPosition(Process& proc) const {
        for (Index r = 0; r < row_count(); ++r) {
            for (Index c = 0; c < col_count(); ++c) {
                proc(Position{r, c});
            }
        }
    } 
    
    Grid<T> Transposed() {
        Grid<T> g(col_count(), row_count());
        auto func = [&](const Position& p) {
            g(p.col, p.row) = grid_[index(p)];
        };
        ForEachPosition(func);
        return g;
    }
    
private:
    Index index(const Position& p) {
        return p.row*col_count_ + p.col;
    }


    Count row_count_, col_count_;
    std::vector<T> grid_;
    
    friend struct const_iterator;
    template<class U>
    friend bool operator==(const Grid<U>& g_0, const Grid<U>& g_1);
};



template<class T>
bool operator==(const Grid<T>& g_0, const Grid<T>& g_1) {
    return g_0.row_count_ == g_1.row_count_ && g_0.grid_ == g_1.grid_;
}
template<class T>
bool operator!=(const Grid<T>& g_0, const Grid<T>& g_1) {
    return !(g_0 == g_1);
}


template<class T>
std::ostream& operator<<(std::ostream& output, const Grid<T>& g) {
    for (auto r = 0; r < g.row_count(); ++r) {
        for (auto c = 0; c < g.col_count(); ++c) {
            output << g(r, c) << " ";
        }
        output << std::endl;
    }
    return output;
} 
template<class T>
std::istream& operator>>(std::istream& in, Grid<T>& g) {
    Size sz;
    in >> sz;
    g.resize(sz);
    auto func = [&](const Position& p) {
        in >> g[p]; 
    };
    g.ForEachPosition(func);
    return in;
}



template<class T>
struct SparceGrid {
    SparceGrid() : SparceGrid(0, 0) {}
    SparceGrid(Count row_count, Count col_count)
    : row_count_(row_count), 
    col_count_(col_count), 
    grid_(row_count*col_count) {
    }
    
    bool isInside(const Position& p) const {
        return isInside(p.row, p.col);
    }
    bool isInside(Int row, Int col) const {
        return row >= 0 && row < row_count_ && 
        col >= 0 && col < col_count_;
    }
    
    void resize(Count row_count, Count col_count) {
        row_count_ = row_count;
        col_count_ = col_count;
        grid_.resize(row_count*col_count);
    }
    
    void fill(const T& t) {
        std::fill(grid_.begin(), grid_.end(), t);
    }
    
    Count row_count() const { return row_count_; }
    Count col_count() const { return col_count_; }
    
    T& operator()(const Position& p) {
        return grid_[p.row*col_count_ + p.col];
    }
    const T& operator()(const Position& p) const {
        return grid_[p.row*col_count_ + p.col];
    }
    
    T& operator()(Int row, Int col) {
        return grid_[row*col_count_ + col];
    }
    const T& operator()(Int row, Int col) const {
        return grid_[row*col_count_ + col];
    }
private:
    Count row_count_, col_count_;
    std::unordered_map<Index ,T> grid_;
};


// we think that top left is start

// should go geomentry probably, as it more stronger
// grid is more basic stuff

// TODO particle should be a template
// no virtual methods ever!
// capitilize method names  
//
//    struct Particle {
//        virtual bool intersects(const Particle& p) const = 0;
//        // maybe to use position (but it can be confused with Position and left top angle)
//        virtual Point center() const = 0;
//    };
//    
template<class ParticlePtr>
class ParticleGrid {
public:
    using Point = ant::geometry::d2::f::Point;
    using Indent = ant::geometry::d2::f::Indent;
    using Size = ant::geometry::d2::f::Size;
    
private:
    Grid<std::list<ParticlePtr>> grid_;
    Point min_; 
    Point max_;
    Size cell_size_;
   
public:
    ParticleGrid() {}
    ParticleGrid(Point min, Point max,
                 Size max_particle_size) {
        
        Reset(min, max, max_particle_size);
    }
    
    Size cell_size() const {
        return cell_size_;
    }
    
    void Clear() {
        for (auto row = 0; row < grid_.row_count(); ++row) {
            for (auto col = 0; col < grid_.col_count(); ++col) {
                grid_(row, col).clear();
            }
        }
    }
    
    void Reset(Point min, 
               Point max,
               Size size) {
        Clear();
        
        min_ = min;
        max_ = max;
        cell_size_ = size;
        
        Count 
        row_count = static_cast<Count>((max.x-min.x)/cell_size_.width) + 1, 
        col_count = static_cast<Count>((max.y-min.y)/cell_size_.height) + 1;
        grid_.resize(row_count, col_count);
    }
    
    void Reset(const ParticleGrid& grid) {
        cell_size_ = grid.cell_size();
        min_ = grid.x_min_;
        max_ = grid.x_max_;
    }
    
    void Add(const ParticlePtr& p) {
        grid_(position(p)).push_back(p);
    }
    
    // before you are going to change location of particle remove it from grid
    bool Remove(const ParticlePtr& p) {
        // maybe will need to ... make equality operator
        // use find to produce 2 factor
        auto& list = grid_(position(p));
        auto it = find(list.begin(), list.end(), p);
        if (it == list.end()) return false;
        else {
            list.erase(it);
            return true;
        }
    }
    
    // input can be particle that aren't inside grid 
    // can overload with just one particle without shared ptr
    std::vector<ParticlePtr> Intersections(const ParticlePtr& p) const {
        std::vector<ParticlePtr> result;
        auto pp = position(p);
        auto 
        r_first = std::max(0, pp.row-1),
        c_first = std::max(0, pp.col-1),
        r_c = static_cast<Int>(grid_.row_count())-1, r_last = std::min(r_c, pp.row+1),
        c_c = static_cast<Int>(grid_.col_count())-1, c_last = std::min(c_c, pp.col+1);
        for (auto row = r_first; row <= r_last; ++row) {
            for (auto col = c_first; col <= c_last; ++col) {
                for (auto& gp : grid_(row, col)) {
                    if (gp == p || !p->Intersects(*gp)) continue;
                    result.push_back(gp); 
                }
            }
        }
        return result;
    }
    
    bool HasIntersection(const ParticlePtr& p) const {
        bool result = false;
        auto pp = position(p);
        auto 
        r_first = std::max(0, pp.row-1),
        c_first = std::max(0, pp.col-1),
        r_c = static_cast<Int>(grid_.row_count())-1, r_last = std::min(r_c, pp.row+1),
        c_c = static_cast<Int>(grid_.col_count())-1, c_last = std::min(c_c, pp.col+1);
        for (auto row = r_first; row <= r_last; ++row) {
            for (auto col = c_first; col <= c_last; ++col) {
                for (auto& gp : grid_(row, col)) {
                    if (gp == p || !p->Intersects(*gp)) continue;
                    result = true;
                    goto finish; 
                }
            }
        }
    finish:
        return result;
    }
    
    // why this one is here ??? 
    bool hasInside(const Point p) const {
        return p.x >= min_.x && p.x <= max_.x && p.y >= min_.y && p.y <= max_.y;
    }
    
private:
    Position position(const ParticlePtr& p) const {
        Point pt = p->center();
        Position pp{
            static_cast<Int>((pt.y - min_.y)/cell_size_.height), 
            static_cast<Int>((pt.x - min_.x)/cell_size_.width)
        };
        assert(!(pp.row < 0 || pp.col < 0 || pp.row >= grid_.row_count() || pp.col >= grid_.col_count()));
        return pp;
    } 
    
};

struct MaxEmptyRegionsFinder {
    MaxEmptyRegionsFinder() {}
    MaxEmptyRegionsFinder(Int row_count, Int col_count)
    : MaxEmptyRegionsFinder(Size(row_count, col_count)) {}
    MaxEmptyRegionsFinder(Size size) 
    : dN(size), dS(size) {}
    
    void set(Int row_count, Int col_count) {
        dN.resize(row_count, col_count);
        dS.resize(row_count, col_count);
    }
    
    void set(Size size) {
        dN.resize(size);
        dS.resize(size);
    }
    
    // grid values : is FILLED ? 
    std::vector<Region> find(const Grid<char>& grid) {
        int 
        row_count = static_cast<Int>(grid.row_count()),
        col_count = static_cast<Int>(grid.col_count()); 
        std::vector<Region> regs;
        
        for (Int c = 0; c < col_count; ++c) {
            dN(0, c) = grid(0, c) ? -1 : 0;
            dS(row_count-1, c) = grid(row_count-1, c) ? -1 : 0;
        }
        
        for (Int r = 1; r < row_count; ++r) {
            for (Int c = 0; c < col_count; ++c) {
                dN(r, c) = grid(r, c) ? -1 : dN(r-1, c)+1; 
            }
        }
        for (Int r = row_count-2; r >= 0; --r) {
            for (Int c = 0; c < col_count; ++c) {
                dS(r, c) = grid(r, c) ? -1 : dS(r+1, c)+1;
            }
        }
        for (Int c = col_count-1; c >= 0; --c) {
            Int maxS = row_count;
            for (Int r = row_count-1; r >= 0; --r) {
                ++maxS;
                if (!grid(r, c) && (c == 0 || grid(r, c-1))) {
                    Int N = dN(r, c);
                    Int S = dS(r, c);
                    Int local_col_count = 1;
                    while (c + local_col_count < col_count && !grid(r, c + local_col_count)) {
                        Int nextN = dN(r, c + local_col_count);
                        Int nextS = dS(r, c + local_col_count);
                        if (nextN < N || nextS < S) {
                            if (S < maxS) regs.push_back(Region(r-N, c, N+S+1, local_col_count));
                            if (nextN < N) N = nextN;
                            if (nextS < S) S = nextS;
                        }
                        ++local_col_count;
                    }
                    if (S < maxS) regs.push_back(Region(r-N, c, N+S+1, local_col_count));
                    maxS = 0;
                } 
            }
        }
        return regs;
    }
private:
    Grid<Int> dN, dS;
};

struct MaxEmptyRegions {
    MaxEmptyRegions() {}
    // starting region
    MaxEmptyRegions(const Position& p, const Size& s)
    : MaxEmptyRegions(p.row, p.col, s.row, s.col) {} 
    MaxEmptyRegions(Int row, Int col, Int row_count, Int col_count) {
        max_empty_regions_.push_back(Region(row, col, row_count, col_count));
    }
    
    void set(Int row, Int col, Int row_count, Int col_count) {
        max_empty_regions_.clear();
        max_empty_regions_.push_back(Region(row, col, row_count, col_count));
    }
    
    void FillRegion(const Region& reg) {
        removeSubMaxEmptyRegions(insertCuts(reg));
    }
    
    const std::vector<Region>& max_empty_regions() {
        return max_empty_regions_;
    }
    
    bool IsEmpty() const {
        return max_empty_regions_.empty();
    }
    
private:
    // returns first inserted
    std::vector<Region>::iterator insertCuts(const Region& filled_region) {
        std::vector<Region> empty_bits;
        // go through all rects with it
        auto reg_it = max_empty_regions_.end();
        // first intersected
        auto end_it = max_empty_regions_.end();
        Int r, c, r_sz, c_sz;
        auto push = [&](Int r, Int c, Int r_sz, Int c_sz) {
            empty_bits.push_back(Region(r, c, r_sz, c_sz));
        };
        // we do it from the cuz of future tail removing
        while (reg_it-- != max_empty_regions_.begin()) {
            Region& region = *reg_it;
            if (!filled_region.hasIntersection(region)) continue;
            
            // top
            if ((r_sz = filled_region.row_begin() - region.row_begin()) > 0) {
                push(
                     region.row_begin(), 
                     region.col_begin(), 
                     r_sz, 
                     region.col_count());
            }
            // left
            if ((c_sz = filled_region.col_begin() - region.col_begin()) > 0) {
                push(
                     region.row_begin(),
                     region.col_begin(),
                     region.row_count(),
                     c_sz);
            }
            // right 
            c = filled_region.col_end();
            if ((c_sz = region.col_end() - c) > 0) {
                push(
                     region.row_begin(),
                     c,
                     region.row_count(),
                     c_sz);
            }
            // bottom
            r = filled_region.row_end();
            if ((r_sz = region.row_end() - r) > 0) {
                push(
                     r,
                     region.col_begin(),
                     r_sz,
                     region.col_count());
            }
            std::swap(*reg_it, *(--end_it));
        }
        max_empty_regions_.erase(end_it, max_empty_regions_.end());
        // need to know before inserting
        Int region_count = (Int)max_empty_regions_.size();
        max_empty_regions_.insert(max_empty_regions_.end(), empty_bits.begin(), empty_bits.end());
        return max_empty_regions_.begin() + region_count;
    }
    
    void removeSubMaxEmptyRegions(std::vector<Region>::iterator start_it) {
        auto 
        sub_count = max_empty_regions_.end() - start_it,
        full_count = start_it - max_empty_regions_.begin();
        // sub inside sub
        if (sub_count > 1) {
            std::sort(start_it, max_empty_regions_.end(), 
                      [](const Region& r_0, const Region& r_1) {
                          return r_0.cell_count() > r_1.cell_count();
                      });
            auto end_it = max_empty_regions_.end();
            auto reg_outer_it = start_it;
            while (reg_outer_it < end_it) {
                auto reg_inner_it = reg_outer_it;
                while (++reg_inner_it < end_it) {
                    if (reg_outer_it->hasInside(*reg_inner_it)) {
                        std::swap(*reg_inner_it, *(--end_it));
                    }
                }
                ++reg_outer_it;
            }
            max_empty_regions_.erase(end_it, max_empty_regions_.end());
        }
        // sub inside full empty  
        if (sub_count > 0 && full_count > 0) {
            auto end_it = max_empty_regions_.end();
            auto reg_outer_it = max_empty_regions_.begin();
            // start iterator don't exchage
            while (reg_outer_it != start_it) {
                auto reg_inner_it = start_it;
                // careful here
                while (reg_inner_it < end_it) {
                    if (reg_outer_it->hasInside(*reg_inner_it)) {
                        std::swap(*reg_inner_it, *(--end_it));
                    }
                    ++reg_inner_it;
                }
                ++reg_outer_it;
            }
            max_empty_regions_.erase(end_it, max_empty_regions_.end());
        }
    }
    
    std::vector<Region> max_empty_regions_;
};

    

// Syke'n'Sugarstarr - Ticket 2 Ride (Andrey Exx & Hot Hotels Remix )
template<size_t N = 64>
struct ZobristHashing {
public:
    using value = std::bitset<N>;
private:
    using Size = grid::Size;
    using States = std::vector<value>;
    using Grid = grid::Grid<States>;
    using Position = grid::Position;
    
    Grid hash_table_;
    // will be just all zeros
    static constexpr value NOTHING = 0;
    
    void initHashTable() {
        static const size_t S = 64;
        // will use 0 for nothing
        uint64_t min = 1;
        uint64_t max = std::numeric_limits<uint64_t>::max();
        std::uniform_int_distribution<uint64_t> distr(min, max);
        std::default_random_engine rng;
        auto randomBitset = [&]() {
            std::bitset<N> s;
            for (auto i = 0; i < (N-1)/S + 1; ++i) {
                s <<= S;
                s |= distr(rng);
            } 
            return s;
        };
        for (auto& cell : hash_table_) {
            for (auto& set : cell) {
                set = randomBitset();
            }
        }
    }
    
public:
    ZobristHashing(const Size& board_size, Count state_count) 
        : hash_table_(board_size) {
        
        States states(state_count);
        states.shrink_to_fit();
        hash_table_.fill(states);
        initHashTable();
    }
    
    void Init() {
        
    }
    
    
    // getter have to return index state by position
    template<class IsFilled, class PositionStateGetter>
    value Hash(IsFilled& is_filled, PositionStateGetter& getter) {
        value res = 0;
        auto func = [&](const Position& p) {
            if (is_filled(p)) {
                xorState(&res, p, getter(p));
            } else {
                xorNothing(&res);
            }
        };
        hash_table_.ForEachPosition(func);
        return res;
    }
    
    
    void xorState(std::bitset<N>* set, const Position& pos, Index state_index) {
        (*set) ^= hash_table_(pos)[state_index];
    }
    
    void xorNothing(std::bitset<N>* set) {
        (*set) ^= NOTHING;
    } 
    
    
};

template<size_t N> constexpr std::bitset<N> ZobristHashing<N>::NOTHING;

Grid<char> ToGrid(const std::vector<std::string>& ss);

template<class T>
Grid<T> ToGrid(const std::vector<std::vector<T>>& ss) {
    Grid<T> g(ss.size(), ss[0].size());
    auto it = g.begin();
    for (auto& s : ss) {
        for (auto& c : s) {
            *it++ = c;
        }
    }
    return g;
}


} // namespace grid

} // namespace ant


namespace std {

template <> struct hash<ant::grid::Position> {
    
    uint64_t operator()(const ant::grid::Position& p) const {
        return ant::Hash(p.row, p.col);
    }
    
};

}



#endif
