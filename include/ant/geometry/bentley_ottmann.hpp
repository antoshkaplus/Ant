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

        SweepLineCompare(const BentleyOttmann& algo, double& y) : cur_sweep_line_y(y), algo_(algo) {}

        bool operator()(SegPtr s_1, SegPtr s_2) const {
            auto x_1 = s_1->atY(cur_sweep_line_y);
            auto x_2 = s_2->atY(cur_sweep_line_y);

            auto i_1 = algo_.get_seg_index(s_1);
            auto i_2 = algo_.get_seg_index(s_2);

            std::cout << "comp: " << i_1 << " " << x_1 << " " << i_2 << " " << x_2 << " " << (x_1 < x_2) << " " << std::abs(x_1 - x_2) << std::endl;
            if (std::abs(x_1 - x_2) < 1e-14) return false;
            return x_1 < x_2;
        }

        double& cur_sweep_line_y;
        const BentleyOttmann& algo_;
    };

    // intersect at cur_sweep_line_y
    struct InsertCompare {

        InsertCompare(BentleyOttmann& algo) : algo_(algo) {}

        bool operator()(SegPtr s_1, SegPtr s_2) const {
            auto y = algo_.cur_sweep_line_y;

            auto y_any_1 = (*s_1)[1].y;
            auto y_any_2 = (*s_2)[1].y;

            // horizontal lines go to the back
            auto dy_1 = (y_any_1 - y);
            if (dy_1 == 0) return false;
            auto dy_2 = (y_any_2 - y);
            if (dy_2 == 0) return true;

            auto i_1 = algo_.get_seg_index(s_1);
            auto i_2 = algo_.get_seg_index(s_2);

            auto& L_1 = algo_.lower_points_[i_1];
            auto& L_2 = algo_.lower_points_[i_2];

            auto y_L_max = std::max(L_1.y, L_2.y);

            auto x = algo_.cur_x;
            auto x_1 = (y_L_max - y) * (L_1.x - x) / (L_1.y - y) + x;
            auto x_2 = (y_L_max - y) * (L_2.x - x) / (L_2.y - y) + x;
            return x_1 < x_2;
        }

    private:
        BentleyOttmann& algo_;
    };


    double cur_sweep_line_y;
    double cur_x;

    std::multiset<SegPtr, SweepLineCompare> sweep_line;

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

    BentleyOttmann() : sweep_line(SweepLineCompare(*this, cur_sweep_line_y)) {}

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
            cur_x = p.x;

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

        std::ofstream out("temp/out.txt");
        std::vector<f::Segment> segs;
        std::for_each(sweep_line.begin(), sweep_line.end(), [&](auto ptr) {
            segs.push_back(*ptr);
        });
        out << segs.size() << std::endl;
        for (auto& s : segs) {
            out << s.p_0.x << " " << s.p_0.y << " " << s.p_1.x << " " << s.p_1.y << std::endl;
        }
        out.close();

        Segment s(p, p.Shifted(0., 10.));
        auto it = sweep_line.lower_bound(&s);
        auto first = it;
        std::experimental::optional<SegPtr> before_first;
        if (first != sweep_line.begin()) before_first.emplace(*std::prev(first));

        Index first_index = *first - segs_->data();

        while (it != sweep_line.end() && HandleIt(it, p)) ++it;
        auto last = it;
        std::experimental::optional<SegPtr> is_last;
        if (last != sweep_line.end()) is_last.emplace(*last);

        Index last_index = *last - segs_->data();

        if (U_p.size() + L_p.size() + C_p.size() > 1) {
            res.add(U_p.begin(), U_p.end());
            auto f = [&](auto it) { res.add(*it); };
            std::for_each(L_p.begin(), L_p.end(), f);
            std::for_each(C_p.begin(), C_p.end(), f);
        }

        std::for_each(L_p.begin(), L_p.end(), [&](auto it) {
            // have to use iterator for quick removal out of sweep_line
            sweep_line.erase(it);
        });

        std::for_each(C_p.begin(), C_p.end(), [&](auto it) {
            U_p.push_back(*it);
            sweep_line.erase(it);
        });

        // all of them are on the same point.. why do I try to compare them as if y is really far down...
        // they are literally all together
        std::sort(U_p.begin(), U_p.end(), InsertCompare(*this));
        sweep_line.insert(U_p.begin(), U_p.end());

        if (U_p.empty()) {
            if (before_first && is_last) FindNewEvent(before_first.value(), is_last.value(), p);
        } else {
            if (before_first) FindNewEvent(before_first.value(), U_p.front(), p);
            if (is_last) FindNewEvent(U_p.back(), is_last.value(), p);
        }
        std::cout << "events\n";
        for (auto e : events) {
            std::cout << e;
        }
        std::cout << "sweepline\n";
        for (auto s : sweep_line) {
            std::cout << get_seg_index(s) << " ";
        }
        std::cout.flush();
        std::cout << "\n";
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