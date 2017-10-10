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
    using SegPtr = const Segment*;

    // don't keep horizontal segments here
    struct SweepLineCompare {

        SweepLineCompare(double& y) : cur_sweep_line_y(y) {}

        bool operator()(SegPtr s_1, SegPtr s_2) const {
            auto x_1 = s_1->atY(cur_sweep_line_y);
            auto x_2 = s_2->atY(cur_sweep_line_y);

            return x_1 < x_2;
        }

        double& cur_sweep_line_y;
    };

    // intersect at cur_sweep_line_y
    struct InsertCompare {

        InsertCompare(BentleyOttmann& algo) : algo_(algo) {}

        bool operator()(SegPtr s_1, SegPtr s_2) const {
            auto y = algo_.cur_sweep_line_y;
            auto y_L_1 = (*s_1)[1].y;
            auto y_L_2 = (*s_2)[1].y;

            // horizontal lines go to the back
            auto dy_1 = (y_L_1 - y);
            if (dy_1 == 0) return false;
            auto dy_2 = (y_L_2 - y);
            if (dy_2 == 0) return true;

            auto x_L_1 = algo_.get_seg_index(s_1);
            auto x_L_2 = algo_.get_seg_index(s_2);

            int det = (x_L_1 - algo_.cur_x) * (y_L_2 - y) - (x_L_2 - algo_.cur_x) * (y_L_1 - y);
            if (det < 0)
                return true;
            if (det > 0)
                return false;

            // can't be on the same line
            throw std::runtime_error("unreachable point is reached");
        }

    private:
        BentleyOttmann& algo_;
    };


    double cur_sweep_line_y;
    double cur_x;

    std::set<SegPtr, SweepLineCompare> sweep_line{SweepLineCompare(cur_sweep_line_y)};

    using SweepLineIt = typename std::set<SegPtr>::const_iterator;

    std::set<Point, TopLeftComparator> events;

    const std::vector<Segment>* segs_;
    std::vector<Point> upper_points_;
    std::vector<Point> lower_points_;

    std::vector<Index> segs_by_upper_point_;
    Index cur_upper_point_segs_index_{0};

    std::vector<SegPtr> U_p;
    std::vector<SweepLineIt> L_p;
    std::vector<SweepLineIt> C_p;

    std::map<double, SegPtr> horizontal;

    TopLeftComparator top_left_compare;

    NestedVectors<SegPtr> res;

public:

    NestedVectors<SegPtr> FindIntersections(const std::vector<Segment>& segs) {
        segs_ = &segs;

        upper_points_.resize(segs.size());
        lower_points_.resize(segs.size());
        for (auto i = 0; i < segs.size(); ++i) {
            std::tie(upper_points_[i], lower_points_[i]) = std::minmax(segs[i][0], segs[i][1], top_left_compare);
        }

        segs_by_upper_point_.resize(segs.size());
        std::iota(segs_by_upper_point_.begin(), segs_by_upper_point_.end(), 0);
        std::sort(segs_by_upper_point_.begin(), segs_by_upper_point_.end(), [&](Index i_1, Index i_2) {
            return top_left_compare(upper_points_[i_1], upper_points_[i_2]);
        });

        for (auto &s : segs) {
            events.insert(s[0]);
            events.insert(s[1]);
        }

        while (!events.empty()) {
            auto p = *events.begin();
            events.erase(events.begin());
            cur_sweep_line_y = p.y;

            typename NestedVectors<SegPtr>::ItemScope scope(res);

            HandleEventPoint(p);
            U_p.clear();
            L_p.clear();
            C_p.clear();
        }

        return res;
    }

private:

    Index get_seg_index(const Segment* s) const {
        return s - segs_->data();
    }

    const Segment& get_seg(Index i) const {
        return (*segs_)[i];
    }

    std::experimental::optional<Point> Intersect(const Segment& s_1, const Segment& s_2) {
        auto res = Intersection(s_1, s_2);
        if (res.second) return {res.first};
        else return {};
    }

    bool IsEndpoint(SegPtr s, Point p) {
        return (*s)[1] == p || (*s)[0] == p;
    }

    bool IsInterior(SegPtr s, Point p) {
        return s->Lie(p);
    }

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
        while (cur_upper_point_segs_index_ < segs_->size() && upper_points_[segs_by_upper_point_[cur_upper_point_segs_index_]] == p) {
            U_p.push_back(&get_seg(segs_by_upper_point_[cur_upper_point_segs_index_]));
            ++cur_upper_point_segs_index_;
        }

        Segment s(p, p.Shifted(0., 10.));
        auto it = sweep_line.lower_bound(&s);
        auto first = it;
        std::experimental::optional<SegPtr> before_first;
        if (first != sweep_line.begin()) before_first.emplace(*std::prev(first));

        while (it != sweep_line.end() && HandleIt(it, p)) ++it;
        auto last = it;
        std::experimental::optional<SegPtr> is_last;
        if (last != sweep_line.end()) is_last.emplace(*last);


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

        std::sort(U_p.begin(), U_p.end(), InsertCompare(*this));
        sweep_line.insert(U_p.begin(), U_p.end());

        if (U_p.empty()) {
            if (before_first && is_last) FindNewEvent(before_first.value(), is_last.value(), p);
        } else {
            if (before_first) FindNewEvent(before_first.value(), U_p.front(), p);
            if (is_last) FindNewEvent(U_p.back(), is_last.value(), p);
        }
    }

    void FindNewEvent(SegPtr seg_1, SegPtr seg_2, Point p) {
        auto i = Intersect(*seg_1, *seg_2);
        if (!i) return;
        if (top_left_compare(p, i.value()) && events.count(i.value()) == 0) {
            events.insert(i.value());
        }
    }
};


}