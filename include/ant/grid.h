
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


#include "ant/geometry/d2.h"



namespace ant {
    
namespace grid {


// make it possible to substruct
struct Indent {
    Indent() : Indent(0, 0) {}
    Indent(Int row, Int col) : row(row), col(col) {}
    
    void set(Int row, Int col) {
        this->row = row;
        this->col = col;
    }
    
    Int area() const {
        return row*col;
    }
    
    Int row, col;
};

struct Size {
    Size() : row(0), col(0) {}
    Size(Int row, Int col)
    : row(row), col(col) {}
    
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
    
    Int row, col;
};

bool operator==(const Size& s_0, const Size& s_1);

struct Position {
    // operators see below
    Position() : Position(0, 0) {}
    Position(Int row, Int col) : row(row), col(col) {}
    
    void set(Int row, Int col) {
        this->row = row;
        this->col = col;
    }
    
    void shift(Int row, Int col) {
        this->row += row;
        this->col += col;
    }
    
    void swap() {
        std::swap(row, col);
    }
    Position swapped() const {
        return Position(col, row);
    }
    
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

Position operator-(const Position& p, const Indent& n);
Position operator+(const Position& p, const Indent& n);        
bool operator==(const Position& p_0, const Position& p_1);        
bool operator!=(const Position& p_0, const Position& p_1);

struct Region {
    Region() : Region(0, 0, 0, 0) {}
    Region(Int row, Int col, Int row_count, Int col_count)
    : position(row, col), size(row_count, col_count) {}
    Region(const Position& p, const Size& d) 
    : position(p), size(d) {}
    
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
    
    Int cell_count() const {
        return size.row*size.col;
    }
    
    Position position;
    Size size;
};

std::ostream& operator<<(std::ostream& output, const Region& r);
bool operator==(const Region& r_0, const Region& r_1);

template<class T>
struct Grid {
    struct const_iterator : std::iterator<std::random_access_iterator_tag, T> {
        
        const_iterator(const Grid<T>& grid, Index index = 0) : grid_(grid), index_(index) {}
        const T& operator*() const { 
            return grid_.grid_[index_]; 
        } 
        bool operator==(const const_iterator& it) {
            return index_ == it.index_; 
        }
        bool operator!=(const const_iterator& it) {
            return index_ != it.index_;
        }
        
        // pred
        const_iterator& operator++() {
            ++index_;
            return *this;
        }
        // post
        const_iterator operator++(int) { 
            const_iterator it(grid_, index_);
            ++index_; 
            return it;
        }
        
    protected:
        const Grid<T>& grid() {
            return grid_;
        }
        
        const Grid<T>& grid_;
        Index index_;
    };
    
    struct iterator : const_iterator {
        using const_iterator::index_;
        using const_iterator::grid_;
        
        iterator(Grid<T>& grid, Index index = 0) : const_iterator(grid, index) {}
        
        iterator& operator++() {
            return static_cast<iterator&>(const_iterator::operator++());
        }
        iterator operator++(int) {
            iterator it(grid(), index_); 
            ++index_;
            return it;
        }
        
        T& operator*() { 
            return grid().grid_[index_]; 
        } 
        
    private:
        Grid<T>& grid() {
            return const_cast<Grid<T>&>(const_iterator::grid_);
        }
    };
    
    const_iterator begin() const {
        return const_iterator(*this);
    }
    iterator begin() {
        return iterator(*this);
    }
    const_iterator end() const {
        return const_iterator(*this, cell_count());
    }
    iterator end() {
        return iterator(*this, cell_count());
    }
    
    
    Grid() : Grid(0, 0) {}
    Grid(Count row_count, Count col_count)
    :   row_count_(row_count), 
    col_count_(col_count), 
    grid_(row_count*col_count) {
    }
    Grid(const Size& size) : Grid(size.row, size.col) {}
    Grid(std::initializer_list<std::vector<T>> list)
    :   Grid(list.size(), list.size() == 0 ? 0 : list.begin()->size()){
        auto b = list.begin();
        for (auto r = 0; r < row_count(); ++r, ++b) {
            std::copy(b->begin(), b->end(), grid_.begin() + r*col_count());
        }
    }
    
    bool isInside(const Position& p) {
        return isInside(p.row, p.col);
    }
    bool isInside(Int row, Int col) {
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
private:
    Count row_count_, col_count_;
    std::vector<T> grid_;
    
    friend class const_iterator;
    template<class U>
    friend bool operator==(const Grid<U>& g_0, const Grid<U>& g_1);
};


template<class T>
bool operator==(const Grid<T>& g_0, const Grid<T>& g_1) {
    return g_0.row_count_ == g_1.row_count_ && g_0.grid_ == g_1.grid_;
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
struct SparceGrid {
    SparceGrid() : SparceGrid(0, 0) {}
    SparceGrid(Count row_count, Count col_count)
    : row_count_(row_count), 
    col_count_(col_count), 
    grid_(row_count*col_count) {
    }
    
    bool isInside(const Position& p) {
        return isInside(p.row, p.col);
    }
    bool isInside(Int row, Int col) {
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
struct ParticleGrid {
    using Point = ant::geometry::d2::f::Point;
    using indent = ant::geometry::d2::f::Indent;
    
    // particles will be compared by address
    struct Particle {
        virtual bool intersects(const Particle& p) const = 0;
        // maybe to use position (but it can be confused with Position and left top angle)
        virtual Point center() const = 0;
    };
    
    ParticleGrid() {}
    ParticleGrid(
                 Float x_min, Float y_min, 
                 Float x_max, Float y_max,
                 Float max_particle_width, 
                 Float max_particle_height) {
        
        reset(x_min, y_min, x_max, y_max, max_particle_width, max_particle_height);
    }
    
    void clear() {
        for (auto row = 0; row < grid.row_count(); ++row) {
            for (auto col = 0; col < grid.col_count(); ++col) {
                grid(row, col).clear();
            }
        }
    }
    
    void reset(Float x_min, Float y_min, 
               Float x_max, Float y_max,
               Float max_particle_width, 
               Float max_particle_height) {
        clear();
        
        this->x_min = x_min;
        this->x_max = x_max;
        this->y_min = y_min;
        this->y_max = y_max;
        
        side_width = max_particle_width; // maybe need to add + some offset
        side_height = max_particle_height;
        
        Count 
        row_count = static_cast<Count>((x_max-x_min)/max_particle_width) + 1, 
        col_count = static_cast<Count>((y_max-y_min)/max_particle_height) + 1;
        grid.resize(row_count, col_count);
    }
    
    void add(const std::shared_ptr<Particle>& p) {
        grid(position(*p)).push_back(p);
    }
    
    // before you are going to change location of particle remove it from grid
    bool remove(const std::shared_ptr<Particle>& p) {
        // maybe will need to ... make equality operator
        // use find to produce 2 factor
        auto& list = grid(position(*p));
        auto it = find(list.begin(), list.end(), p);
        if (it == list.end()) return false;
        else {
            list.erase(it);
            return true;
        }
    }
    
    void relocate(std::shared_ptr<Particle>& p, geometry::d2::f::Point center) {
        
    }
    
    void shift(std::shared_ptr<Particle>& p, Indent indent) {
        
    }
    
    // input can be particle that aren't inside grid 
    // can overload with just one particle without shared ptr
    std::vector<std::shared_ptr<Particle>> intersections(const std::shared_ptr<Particle>& p) {
        std::vector<std::shared_ptr<Particle>> result;
        auto pp = position(*p);
        auto 
        r_first = std::max(0, pp.row-1),
        c_first = std::max(0, pp.col-1),
        r_c = static_cast<Int>(grid.row_count())-1, r_last = std::min(r_c, pp.row+1),
        c_c = static_cast<Int>(grid.col_count())-1, c_last = std::min(c_c, pp.col+1);
        for (auto row = r_first; row <= r_last; ++row) {
            for (auto col = c_first; col <= c_last; ++col) {
                for (auto& gp : grid(row, col)) {
                    if (gp == p || !p->intersects(*gp)) continue;
                    result.push_back(gp); 
                }
            }
        }
        return result;
    }
    
    bool hasIntersection(const std::shared_ptr<Particle>& p) {
        bool result = false;
        auto pp = position(*p);
        auto 
        r_first = std::max(0, pp.row-1),
        c_first = std::max(0, pp.col-1),
        r_c = static_cast<Int>(grid.row_count())-1, r_last = std::min(r_c, pp.row+1),
        c_c = static_cast<Int>(grid.col_count())-1, c_last = std::min(c_c, pp.col+1);
        for (auto row = r_first; row <= r_last; ++row) {
            for (auto col = c_first; col <= c_last; ++col) {
                for (auto& gp : grid(row, col)) {
                    if (gp == p || !p->intersects(*gp)) continue;
                    result = true;
                    goto finish; 
                }
            }
        }
    finish:
        return result;
    }
    
    bool hasInside(const Point p) {
        return p.x >= x_min && p.x <= x_max && p.y >= y_min && p.y <= y_max;
    }
    
private:
    Position position(const Particle& p) {
        Point pt = p.center();
        Position pp{
            static_cast<Int>((pt.y - y_min)/side_height), 
            static_cast<Int>((pt.x - x_min)/side_width)
        };
        assert(!(pp.row < 0 || pp.col < 0 || pp.row >= grid.row_count() || pp.col >= grid.col_count()));
        return pp;
    } 
    
    Grid<std::list<std::shared_ptr<Particle>>> grid;
    double
    side_width, 
    side_height,
    x_min,
    x_max,
    y_min,
    y_max;
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
    std::vector<Region> find(const Grid<bool>& grid) {
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
    
    void insertRegion(const Region& reg) {
        removeSubMaxEmptyRegions(insertCuts(reg));
    }
    
    const std::vector<Region>& max_empty_regions() {
        return max_empty_regions_;
    }
    
    bool isEmpty() const {
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
template<size_t N>
struct ZobristHashing {
private:
    using Size = grid::Size;
    using States = std::vector<std::bitset<N>>;
    using Grid = grid::Grid<States>;
    using Position = grid::Position;
    
    Grid hash_table_;
    // will be just all zeros
    static constexpr std::bitset<N> NOTHING = 0;
    
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
    ZobristHashing(const Size& board_size, Count state_count) :
    hash_table_(board_size) {
        
        States states(state_count);
        states.shrink_to_fit();
        hash_table_.fill(states);
        initHashTable();
    }
    
    void xorState(std::bitset<N>* set, const Position& pos, Index state_index) {
        (*set) ^= hash_table_(pos)[state_index];
    }
    
    void xorNothing(std::bitset<N>* set) {
        (*set) ^= NOTHING;
    } 
    
    using value = std::bitset<N>;
};

template<size_t N> constexpr std::bitset<N> ZobristHashing<N>::NOTHING;



} // namespace grid

} // namespace ant


#endif