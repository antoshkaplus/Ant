#pragma once


#include <iostream>
#include <array>
#include <map>
#include <experimental/optional>

#include "d2.hpp"


namespace ant::geometry::d2 {


template<typename Point, typename Segment>
class BentleyOttmann {

    using P = std::array<Index, 2>;

    struct SweepLineCompare {

        SweepLineCompare(double& y) : cur_sweep_line_y(y) {}

        bool operator()(const Segment* s_1, const Segment* s_2) const {
            auto x_1 = s_1->atY(cur_sweep_line_y);
            auto x_2 = s_2->atY(cur_sweep_line_y);

            return x_1 < x_2;
        }

        double& cur_sweep_line_y;
    };

    struct InsertCompare {
        bool operator()(const Segment* s_1, const Segment* s_2) const {
            auto y = cur_sweep_line_y;
            auto y_L_1 = s_1[1].y;
            auto y_L_2 = s_2[1].y;

            auto dy_1 = (y_L_1 - y);
            if (dy_1 == 0) return false;
            auto dy_2 = (y_L_2 - y);
            if (dy_2 == 0) return true;

            // if not horizontal, try to compute...
            int det = (x_L_1 - x_1) * (y_L_2 - y) - (x_L_2 - x_1) * (y_L_1 - y);
            if (det < 0)
                return true;
            if (det > 0)
                return false;

            // can't be on the same line
            throw std::runtime_error();
        }
    };


    double cur_sweep_line_y;

    std::set<Segment*, SweepLineCompare> sweep_line(SweepLineCompare(cur_sweep_line_y));

    using SweepLineIt = typename std::set<Segment*>::iterator;

    std::set<Point> events;

    const std::vector<Segment>* segs_;
    std::vector<Point> upper_points_;

    std::vector<Index> segs_by_upper_point_;
    Index cur_upper_point_segs_index_{0};

    std::vector<Segment*> U_p;
    std::vector<SweepLineIt> L_p;
    std::vector<SweepLineIt> C_p;

    NestedVectors<Segment*> res;

public:

    NestedVectors<Segment*> FindIntersections(const std::vector<Segment>& segs) {
        segs_ = &segs;

        auto comp = TopLeftComparator();
        upper_points_.resize(segs.size());
        std::transform(segs.begin(), segs.end(), upper_points_.begin(), [&](const Segment& s) {
            return min(s[0], s[1], comp);
        });

        segs_by_upper_point_.resize(segs.size());
        std::iota(segs_by_upper_point_.begin(), segs_by_upper_point_.end(), 0);
        std::sort(segs_by_upper_point_.begin(), segs_by_upper_point_.end(), [&](Index i_1, Index i_2) {
            return comp(upper_points_[i_1], upper_points_[i_2]);
        });

        for (auto &p : ps) {
            q.push(p);
        }

        while (!events.empty()) {
            auto p = events.top();
            events.pop();
            cur_sweep_line_y = p.y;

            NestedVectors<Segment*>::ItemScope scope(res);

            HandleEventPoint(p);
            U_p.clear();
            L_p.clear();
            C_p.clear();
        }
    }

private:

    const Segment& get_seg(Index i) const {
        return (*original_segs_)[i];
    }

    std::experimental::optional<Point> Intersect(Index s_1, Index s_2) {
        auto res = Intersection(segs_[s_1], segs_[s_2]);
        if (res.second) return {res.first};
        else return {};
    }

    bool IsEndpoint(Segment* s, Point p) {
        return (*s)[1] == p || (*s)[0] == p;
    }

    bool IsInterior(Segment* s, Point p) {
        return s->Lie(p);
    }

    template<class T>
    bool HandleIt(SweepLineIt it, Point p) {

        if (IsEndpoint(*it, p)) {
            L_p.push_back(it);
            return true;
        } else if (IsInterior(*it, p)) {
            C_p.push_back(it);
            return true;
        }
        return false;
    }

    void HandleEventPoint(const Point &p) {
        while (cur_upper_point_segs_index_ < segs_->size() && upper_points_[get_seg(segs_by_upper_point_[cur_upper_point_segs_index_])] == p) {
            U_p.push_back(segs_by_upper_point_[cur_upper_point_segs_index_]);
            ++cur_upper_point_segs_index_;
        }

        auto it = sweep_line.lower_bound(Segment(p, p.Shifted(0, 10)));
        auto first = it;
        std::experimental::optional<Segment*> before_first;
        if (first != sweep_line.begin()) *before_first = *std::prev(first);

        while (it != sweep_line.end() && (b = HandleIt(it->second))) ++it;
        auto last = it;
        std::experimental::optional<Segment*> is_last;
        if (last != sweep_line.end()) *is_last = last;


        res.add(U_p.begin(), U_p.end());
        auto f = [&](auto it) {res.add(*it);};
        std::for_each(L_p.begin(), L_p.end(), f);
        std::for_each(C_p.begin(), C_p.end(), f);

        std::for_each(L_p.begin(), L_p.end(), [&](auto it) {
            sweep_line.erase(it);
        });

        std::for_each(C_p.begin(), C_p.end(), [&](auto it) {
            U_p.push_back(*it);
            sweep_line.erase(it);
        });

        std::sort(U_p.begin(), U_p.end(), InsertCompare());
        sweep_line.insert(U_p.begin(), U_p.end());

        if (U_p.empty()) {
            if (before_first && is_last) FindNewEvent(before_first.value(), is_last.value(), p);
        } else {
            if (before_first) FindNewEvent(before_first.value(), U_p.front(), p);
            if (is_last) FindNewEvent(U_p.back(), is_last.value(), p);
        }
    }

    void FindNewEvent(Segment* seg_1, Segment* seg_2, Point p) {
        auto i = Intersect(*seg_1, *seg_2);
        if (!i) return;
        if (i.value() > p && events.count(i.value()) == 0) {
            events.insert(i.value());
        }
    }
};


}