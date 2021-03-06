
#include <fstream>

#include "gtest/gtest.h"

#include "ant/optimization/optimization.h"
#include "ant/grid/grid.hpp"
#include "ant/core/core.hpp"
//#include "ant/physics/circle_collision_system.hpp"


using namespace std;
using namespace ant;
using namespace grid;



struct BoardBase {

    const static char MAX_SIZE = 100;

    // can't make 0 be nothing cuz of indexing
    const static char kMirRight     = 0;
    const static char kMirLeft      = 1;
    const static char kMirNothing   = 2;

    virtual void cast(const Position& p) = 0;
    virtual void restore() = 0;
    Count mirrorsLeft() const {
        return (Count)(size()*size() - mirrorsDestroyed());
    }
    virtual double filling() { return mirrorsLeft()/(double)(size()*size()); }
    virtual Count mirrorsDestroyed() const = 0;
    virtual bool isDestroyed(Index row, Index col) const = 0;
    virtual char mirror(Index row, Index col) const = 0;
    virtual size_t size() const = 0;

    virtual Count rowMirrorsLeft(Index row) const = 0;
    virtual Count colMirrorsLeft(Index col) const = 0;

    virtual Count emptyRowCount() const = 0;
    virtual Count emptyColCount() const = 0;

    virtual bool isRowEmpty(Index row) const {
        return rowMirrorsLeft(row) == 0;
    }
    virtual bool isColEmpty(Index col) const {
        return colMirrorsLeft(col) == 0;
    }

    vector<Position> candidateCasts() {
        vector<Position> cs;
        size_t sz = size();
        for (auto i = 0; i < sz; ++i) {
            if (!isRowEmpty(i)) {
                cs.emplace_back(i, -1);
                cs.emplace_back(i, sz);
            }
            if (!isColEmpty(i)) {
                cs.emplace_back(-1, i);
                cs.emplace_back(sz, i);
            }
        }
        return cs;
    }
};

ostream& operator<<(ostream& output, const BoardBase& board);




struct Board : BoardBase {
    const static Index kHor = 0;
    const static Index kVer = 1;

    using Direction = char;
    // can't make 0 be nothing cuz of indexing
    const constexpr static int kDirTop      = 0;
    const constexpr static int kDirBottom   = 1;
    const constexpr static int kDirLeft     = 2;
    const constexpr static int kDirRight    = 3;
    const constexpr static int kDirNothing  = 4;

    static constexpr const int kDirOpposite[] = {
        kDirBottom,
        kDirTop,
        kDirRight,
        kDirLeft,
        kDirNothing
    };


    // can actually store just integers of indent
    struct Item {
        // here we have 4 pointers // index points to direction
        // top, right, left, bottom
        Position neighbors[4];
        // relative to mirrors
        char mirror;

        // takes direction from which ray is coming
        // returns next position
        // direction after reflection
        tuple<Position, char> nextFrom(char dir) {
            using P = tuple<Position, char>;
            switch (mirror) {
                case kMirLeft: {
                    switch (dir) {
                        case kDirTop:       return P{neighbors[kDirLeft], kDirLeft} ;
                        case kDirBottom:    return P{neighbors[kDirRight], kDirRight};
                        case kDirLeft:      return P{neighbors[kDirTop], kDirTop};
                        case kDirRight:     return P{neighbors[kDirBottom], kDirBottom};
                        default: assert(false);
                    }
                }
                case kMirRight: {
                    switch (dir) {
                        case kDirTop:       return P{neighbors[kDirRight], kDirRight} ;
                        case kDirBottom:    return P{neighbors[kDirLeft], kDirLeft};
                        case kDirLeft:      return P{neighbors[kDirBottom], kDirBottom};
                        case kDirRight:     return P{neighbors[kDirTop], kDirTop};
                        default: assert(false);
                    }
                }
                default: return P{neighbors[dir], dir};
            }
        }
    };

    Board() {}

    Board(const vector<string>& board) {
        board_size_ = (Int)board.size()+2;
        destroyed_count_ = 0;
        mirrors_left_[kHor].resize((Count)board.size(), (Count)board.size());
        mirrors_left_[kVer].resize((Count)board.size(), (Count)board.size());
        empty_row_count_ = 0;
        empty_col_count_ = 0;

        cumulative_history_.push_back(0);
        hash_function_.reset(new ZobristHashing<HASH_BITS_COUNT>({board_size_, board_size_}, 1));

        Position ns[4];
        default_random_engine rng;
        uniform_int_distribution<Index> distr(0, numeric_limits<Index>::max()/(size()*size()));
        items_.resize((Count)board.size()+2, (Count)board.size()+2);
        destroyed_items_.resize((Count)board_size_, (Count)board_size_);
        destroyed_items_.fill(false);
        //hashed_items_.resize((Count)board_size_, (Count)board_size_);
        //hashed_items_.fill(0);
        for (auto row = 1; row <= board.size(); ++row) {
            for (auto col = 1; col <= board.size(); ++col) {
                ns[kDirTop]     = {row-1, col};
                ns[kDirBottom]  = {row+1, col};
                ns[kDirLeft]    = {row, col-1};
                ns[kDirRight]   = {row, col+1};
                memcpy(items_(row,col).neighbors, ns, sizeof(Position[4]));
                items_(row,col).mirror = board[row-1][col-1] == 'R'? kMirRight : kMirLeft;
                //hash += hashed_items_(row, col) = distr(rng);
                hash_function_->xorNothing(&hash_); // xor out
                hash_function_->xorState(&hash_, Position{row, col}, 0);
            }
        }
        for (auto i = 0; i < board.size()+2; ++i) {
            items_(i, 0).mirror = kMirNothing;
            items_(i, (Count)board.size()+1).mirror = kMirNothing;
            items_(0, i).mirror = kMirNothing;
            items_((Count)board.size()+1, i).mirror = kMirNothing;

            items_(i, 0).neighbors[kDirRight] = {i, 1};
            items_(i, (Count)board.size()+1).neighbors[kDirLeft] = {i, static_cast<Int>(board.size())};
            items_(0, i).neighbors[kDirBottom] = {1, i};
            items_((Count)board.size()+1, i).neighbors[kDirTop] = {static_cast<Int>(board.size()), i};
        }
    }

    Direction castDirection(const Position& p) {
        if (p.row == 0) {
            return kDirBottom;
        } else if (p.col == 0) {
            return kDirRight;
        } else if (p.row == items_.row_count()-1) {
            return kDirTop;
        } else {
            return kDirLeft;
        }
    }

    Count mirrorsDestroyed() const override {
        return destroyed_count_;
    }

    bool isDestroyed(Index row, Index col) const override {
        return (bool)destroyed_items_(row+1, col+1);
    }

    char mirror(Index row, Index col) const override {
        return items_(row+1, col+1).mirror;
    }

    size_t size() const override {
        return board_size_-2;
    }

    void cast(const Position& pp) override {
        Position p(pp);
        p.shift(1, 1);
        char dir = castDirection(p);
        //assert(!Rectangle(1, 1, size(), size()).hasInside(p));
        tie(p, dir) = items_(p).nextFrom(dir);
        destroyed_.push_back({});
        Count count = 0;
        while (items_(p).mirror != kMirNothing) {
            assert(dir != kDirNothing);
            destroyed_.back().push(p);
            destroy(p);
            ++count;
            tie(p, dir) = items_(p).nextFrom(kDirOpposite[dir]);
        }
        history_.push_back(count);
        cumulative_history_.push_back(count + cumulative_history_.back());
    }

    Count tryCast(const Position& pp) {
        static ant::Stack<Position> recover_stack;
        // stack is cleared.. by nature
        Position p(pp);
        p.shift(1, 1);
        char dir = castDirection(p);
        tie(p, dir) = items_(p).nextFrom(dir);
        Count destroyed_count = destroyed_count_;
        while (items_(p).mirror != kMirNothing) {
            if (destroyed_items_(p)) {
                p = items_(p).neighbors[dir];
                continue;
            }
            destroyed_items_(p) = true;
            --mirrors_left_[kHor][p.row-1];
            --mirrors_left_[kVer][p.col-1];
            recover_stack.push(p);
            ++destroyed_count;
            tie(p, dir) = items_(p).nextFrom(kDirOpposite[dir]);
        }

        destroyed_count *= 2;
        for (auto& p : recover_stack) {
            destroyed_count += (rowMirrorsLeft(p.row-1) == 0) + (colMirrorsLeft(p.col-1) == 0);
            destroyed_count -= (rowMirrorsLeft(p.row-1) == 1) + (colMirrorsLeft(p.col-1) == 1);
            destroyed_count -= (rowMirrorsLeft(p.row-1) == 3) + (colMirrorsLeft(p.col-1) == 3);
            destroyed_count -= (rowMirrorsLeft(p.row-1) == 5) + (colMirrorsLeft(p.col-1) == 5);
        }

        while (!recover_stack.empty()) {
            p = recover_stack.top();
            ++mirrors_left_[kHor][p.row-1];
            ++mirrors_left_[kVer][p.col-1];
            destroyed_items_(p) = false;
            recover_stack.pop();
        }
        return destroyed_count;
    }

    void restore() override {
        history_.pop_back();
        cumulative_history_.pop_back();
        auto& st = destroyed_.back();
        while (!st.empty()) {
            restore(st.top());
            st.pop();
        }
        destroyed_.pop_back();
    }

    void restoreAll() {
        while (!destroyed_.empty()) {
            restore();
        }
    }

    bool isRowEmpty(Index row) const override {
        return mirrors_left_[kHor][row] == 0;
    }

    bool isColEmpty(Index col) const override {
        return mirrors_left_[kVer][col] == 0;
    }

    Count rowMirrorsLeft(Index row) const override {
        return mirrors_left_[kHor][row];
    }
    Count colMirrorsLeft(Index col) const override {
        return mirrors_left_[kVer][col];
    }

    Count emptyRowCount() const override {
        return empty_row_count_;
    }

    Count emptyColCount() const override {
        return empty_col_count_;
    }

    //private:
    void destroy(const Position& p) {
        //assert(Rectangle(1, 1, size(), size()).hasInside(p));
        if (--mirrors_left_[kHor][p.row-1] == 0) ++empty_row_count_;
        if (--mirrors_left_[kVer][p.col-1] == 0) ++empty_col_count_;
        ++destroyed_count_;
        Item &item = items_(p);
        items_(item.neighbors[kDirTop]).neighbors[kDirBottom] = item.neighbors[kDirBottom];
        items_(item.neighbors[kDirBottom]).neighbors[kDirTop] = item.neighbors[kDirTop];
        items_(item.neighbors[kDirLeft]).neighbors[kDirRight] = item.neighbors[kDirRight];
        items_(item.neighbors[kDirRight]).neighbors[kDirLeft] = item.neighbors[kDirLeft];
        destroyed_items_(p) = true;
        //hash -= hashed_items_(p);
        hash_function_->xorState(&hash_, p, 0); // xor out
        hash_function_->xorNothing(&hash_); // xor in
    }

    void restore(const Position& p) {
        //assert(Rectangle(1, 1, size(), size()).hasInside(p));
        if (mirrors_left_[kHor][p.row-1]++ == 0) --empty_row_count_;
        if (mirrors_left_[kVer][p.col-1]++ == 0) --empty_col_count_;
        --destroyed_count_;
        Item& item = items_(p);
        items_(item.neighbors[kDirTop]).neighbors[kDirBottom] = p;
        items_(item.neighbors[kDirBottom]).neighbors[kDirTop] = p;
        items_(item.neighbors[kDirLeft]).neighbors[kDirRight] = p;
        items_(item.neighbors[kDirRight]).neighbors[kDirLeft] = p;
        destroyed_items_(p) = false;
        //hash += hashed_items_(p);
        hash_function_->xorNothing(&hash_); // xor out
        hash_function_->xorState(&hash_, p, 0); // xor in
    }

    bool operator==(const Board& b) const {
        return hash_ == b.hash_;
        //return hash == b.hash;
    }

    // counted from 0
    const vector<Count>& CumulativeHistory() const {
        return cumulative_history_;
    }

    const vector<Count>& History() const {
        return history_;
    }

    Count CastCount() const {
        return history_.size();
    }

    const ant::Stack<Position>& LastCast() const {
        return destroyed_.back();
    }

    Count LinkCount(const Position& p) const {
        Count c = 0;
        const Position *ns = items_[p].neighbors;
        for (auto i = 0; i < 4; ++i) {
            if (ns[i].row > 0 && ns[i].col > 0 && ns[i].row <= board_size_ && ns[i].col <= board_size_) ++c;
        }
        return c;
    }

private:
    //Index hash;
    Count destroyed_count_;
    vector<ant::Stack<Position>> destroyed_;
    vector<Count> mirrors_left_[2];
    Count empty_row_count_;
    Count empty_col_count_;
    // here 0 and last indices will be placed for
    // special elements that will keep next element
    // to destroy or just opposite entry
    //Grid<Index> hashed_items_;
    Grid<Item> items_;
    Grid<char> destroyed_items_;
    Int board_size_;
    vector<Count> cumulative_history_;
    vector<Count> history_;


    constexpr static size_t HASH_BITS_COUNT = 64;
    shared_ptr<ZobristHashing<HASH_BITS_COUNT>> hash_function_;
    ZobristHashing<HASH_BITS_COUNT>::value hash_;
public:
    using HashType = ZobristHashing<HASH_BITS_COUNT>::value;

    const ZobristHashing<HASH_BITS_COUNT>::value& Hash() const {
        return hash_;
    }

};


struct Solver {
    Solver() : output("out.txt") {}
    virtual vector<int> destroy(vector<string>& board) = 0;
    ofstream output;
};



struct BeamSearchSolver : Solver, opt::BeamSearch {

    using BS = opt::BeamSearch;

    // simple double
    struct Value : BS::Value {
        Value(double val) : val(val) {}

        bool operator<(const BS::Value& v) const override {
            return val < static_cast<const Value&>(v).val;
        }
        // need to consider count of moves
        double val;
    };

    // cast position
    struct Component : BS::Component, Position {
        Component(Int row, Int col) : Position(row, col) {}
    };

    // everyone is candidate
    struct State : BS::State {

        State(const Board& board) : board_(board) {}

        bool isFinal() const override {
            return board_.mirrorsLeft() == 0;
        }

        virtual vector<unique_ptr<BS::Component>> candidateComponents() override {
            vector<unique_ptr<BS::Component>> ccs;
            for (auto i = 0; i < board_.size(); ++i) {
                if (board_.rowMirrorsLeft(i) > 0) {
                    ccs.emplace_back(new Component(i, -1));
                    ccs.emplace_back(new Component(i, (Int)board_.size()));
                }
                if (board_.colMirrorsLeft(i) > 0) {
                    ccs.emplace_back(new Component(-1, i));
                    ccs.emplace_back(new Component((Int)board_.size(), i));
                }
            }
            return ccs;
        }

        void push(const opt::BeamSearch::Component& comp) override {
            const Component& c = static_cast<const Component&>(comp);
            int_casts_.push_back(c.row);
            int_casts_.push_back(c.col);
            board_.cast(c);
        }

        // maybe board should keep all the canst.... and board needs manager to keep track of restore... possiblity
        void pop() override {
            int_casts_.pop_back();
            int_casts_.pop_back();
            board_.restore();
        }

        // can remake it later
        virtual unique_ptr<BS::Value> value() const override {
            Count m_d = board_.mirrorsDestroyed();
            return unique_ptr<BS::Value>(new Value((double)int_casts_.size()/m_d));
        }

        virtual State& operator=(const BS::State& s) override {
            return *this = dynamic_cast<const State&>(s);
        }

        virtual unique_ptr<BS::State> clone() const override {
            return unique_ptr<BS::State>(new State(*this));
        }

        virtual bool operator==(const BS::State& state) const override {
            return board_ == static_cast<const State&>(state).board_;
        }


        Board board_;
        // kCastOne kCastDouble
        vector<char> cast_type_;
        vector<int> int_casts_;
    };

    struct NarrowState : State {

        NarrowState(const Board& board) : State(board) {}

        vector<double> values;
        vector<Index> order;

        virtual vector<unique_ptr<BS::Component>> candidateComponents() override {
            vector<unique_ptr<BS::Component>> cands = State::candidateComponents();
            values.resize(cands.size());
            for (auto i = 0; i < cands.size(); ++i) {
                values[i] = board_.tryCast(static_cast<Component&>(*cands[i]));
            }
            order.resize(cands.size());
            iota(order.begin(), order.end(), 0);
            Count count = min<Count>(10, cands.size());
            partial_sort(order.begin(), order.begin()+count, order.end(),
                         [&](Index i_0, Index i_1) { return values[i_0] > values[i_1]; });
            decltype(cands) res(count);
            for (auto i = 0; i < count; ++i) res[i] = std::move(cands[order[i]]);
            return res;
        }

        virtual NarrowState& operator=(const BS::State& s) override {
            return *this = dynamic_cast<const NarrowState&>(s);
        }

        virtual unique_ptr<BS::State> clone() const override {
            return unique_ptr<BS::State>(new NarrowState(*this));
        }
    };



    using CurrentState = NarrowState;
    vector<int> destroy(vector<string>& board) override {
        Board sb(board);
        BeamSearchSolver bss;
        Count count = board.size() < 75 ? 1000 : 500;
        count = 2000;
        //Count count = 1000;
        auto ptr = bss.solve(CurrentState(sb), count);
        return static_cast<CurrentState&>(*ptr).int_casts_;
    }

    Board Destroy(const Board& board, Count beam_width) {
        BeamSearchSolver bss;
        auto ptr = bss.solve(NarrowState(board), beam_width);
        return static_cast<NarrowState&>(*ptr).board_;
    }
};


int main(int argc, char **argv) {
    cout << "working directory: " << argv[0] << endl;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
