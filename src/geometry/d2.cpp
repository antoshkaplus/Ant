

#include "ant/geometry/d2.hpp"

#include <array>


namespace ant {

namespace geometry {
    
namespace d2 {

namespace i {

bool operator==(const Point& p_0, const Point& p_1) {
    return p_0.x == p_1.x && p_0.y == p_1.y;
}

bool operator!=(const Point& p_0, const Point& p_1) {
    return p_0.x != p_1.x || p_0.y != p_1.y;
}

int ShoelaceFormula(const std::vector<Point>& ps) {
    double s = 0;
    s += ps[ps.size()-1].x*ps[0].y - ps[0].x*ps[ps.size()-1].y;
    for (auto i = 0; i < ps.size()-1; ++i) {
        s += ps[i].x*ps[i+1].y - ps[i+1].x*ps[i].y;
    }
    return std::abs(s)/2;
}

int ShoelaceFormula(const std::vector<Point>& ps, const std::vector<Index>& order) {
    double s = 0;
    s += ps[order[order.size()-1]].x*ps[order[0]].y 
            - ps[order[0]].x*ps[order[order.size()-1]].y;
    for (auto i = 0; i < order.size()-1; ++i) {
        s += ps[order[i]].x*ps[order[i+1]].y - ps[order[i+1]].x*ps[order[i]].y;
    }
    return std::abs(s)/2;
}

bool Collinear(const Point& p_0, const Point& p_1, const Point& p_2) {
    return (p_1.y - p_0.y) * (p_2.x - p_1.x) == (p_2.y - p_1.y) * (p_1.x - p_0.x);
}

Triangle CircumTriangle(const Rectangle& r) {
    std::array<Point, 3> a;
    a[0] = r.origin;
    a[1] = r.origin; a[1].y += 2*r.size.height;
    a[2] = r.origin; a[2].x += 2*r.size.width;
    return a;
}

Rectangle CircumRectangle(const std::vector<Point>& ps) {
    if (ps.empty()) throw std::runtime_error("CircumRectangle: no points"); 
    Int x_min = std::numeric_limits<Int>::max();
    Int y_min = std::numeric_limits<Int>::max();
    Int x_max = std::numeric_limits<Int>::min();
    Int y_max = std::numeric_limits<Int>::min();
    for (auto& p : ps) {
        if (p.x < x_min) x_min = p.x;
        else if (p.x > x_max) x_max = p.x;
        if (p.y < y_min) y_min = p.y;
        else if (p.y > y_max) y_max = p.y;
    }
    return Rectangle{{x_min, y_min}, {x_max-x_min, y_max-y_min}};
}

bool IsInConvexPolygon(const std::vector<Point>& ps, Point p) {

    auto sign_1 = 0;
    auto p_1 = ps.back();
    for (auto i = 0; i < ps.size(); i++)
    {
        auto p_2 = ps[i];
        
        auto d = (p.x - p_1.x)*(p_2.y - p_1.y) - (p.y - p_1.y)*(p_2.x - p_1.x);
        if (d == 0) {
            int x_1, x_2, y_1, y_2;
            std::tie(x_1, x_2) = std::minmax(p_1.x, p_2.x);
            std::tie(y_1, y_2) = std::minmax(p_1.y, p_2.y);
            // on segment
            return p.x >= x_1 && p.x <= x_2 && p.y >= y_1 && p.y <= y_2;
        } 
        int sign_2 = d > 0 ? 1 : -1;
        // sign change
        if (sign_1 + sign_2 == 0) {
            return false;
        }
        sign_1 = sign_2;
        p_1 = p_2;
    }
    return true;
}


}



namespace f {


std::ostream& operator<<(std::ostream& output, const Point& p) {
    return output << "x: " << p.x << ", y: " << p.y << std::endl;
}

std::ostream& operator<<(std::ostream& output, const Segment& s) {
    return output << "seg:\n" << "p0: " << s.p_0 << "p1: " << s.p_1;
}

Point& operator+=(Point& p_0, const Point& p_1) {
    p_0.x += p_1.x;
    p_0.y += p_1.y;
    return p_0;
}

Point& operator/=(Point& p_0, Float f) {
    p_0.x /= f;
    p_0.y /= f;
    return p_0;
} 

Point operator+(Point p_0, const Point& p_1) {
    return p_0 += p_1;
}

Point operator/(Point p_0, Float f) {
    return p_0 /= f;
}

Point operator*(Float f, Point p) {
    p.x *= f;
    p.y *= f;
    return p;
}

Point Centroid(const Point& p_0, const Point& p_1) {
    return {(p_0.x + p_1.x)/2., (p_0.y + p_1.y)/2.}; 
}

Point Between(const Point& from, const Point& to, double alpha) {
    return from + (to - from) * alpha; 
}

Indent& operator/=(Indent& i, Float f) {
    i.dx /= f;
    i.dy /= f;
    return i;
}

Indent& operator*=(Indent& i, Float f) {
    i.dx *= f;
    i.dy *= f;
    return i;
}

Indent operator/(Indent i, Float f) {
    i /= f;
    return i;
}

Indent operator*(Indent i, Float f) {
    i *= f;
    return i;
}

Indent operator+(Indent i_0, Indent i_1) {
    i_0.dx += i_1.dx;
    i_0.dy += i_1.dy;
    return i_0;
}

Point& operator+=(Point& p, Indent i) {
    p.x += i.dx;
    p.y += i.dy;
    return p;
}

Indent operator-(const Point& p_0, const Point& p_1) {
    Indent i;
    i.dx = p_0.x - p_1.x;
    i.dy = p_0.y - p_1.y;
    return i;
}

Indent operator*(Float d, Indent i) {
    i *= d;
    return i;
}
    
Point operator+(Indent i, Point p) {
    p += i;
    return p;
}

Point operator-(Indent i, Point p) {
    return Point(i.dx - p.x, i.dy - p.y);
}
 
Point operator+(Point p, Indent i) {
    return i + p;
}

Point operator-(Point p, Indent i) {
    return Point(p.x - i.dx, p.y - i.dy);
} 

Point RotatePoint(Point p, Point center, double angle) {
    return Point();
}
    
// following https://stackoverflow.com/a/1968345/2635720
// Andre LeMothe's "Tricks of the Windows Game Programming Gurus"
std::pair<Point, bool> Intersection(const Segment& s_0, const Segment& s_1) {
    auto i_0 = s_0.p_1 - s_0.p_0;
    auto i_1 = s_1.p_1 - s_1.p_0;

    auto d = -i_1.dx * i_0.dy + i_0.dx * i_1.dy;
    auto s = (-i_0.dy * (s_0.p_0.x - s_1.p_0.x) + i_0.dx * (s_0.p_0.y - s_1.p_0.y)) / d;
    auto t = ( i_1.dx * (s_0.p_0.y - s_1.p_0.y) - i_1.dy * (s_0.p_0.x - s_1.p_0.x)) / d;

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        return std::make_pair(Point{s_0.p_0.x + (t * i_0.dx), s_0.p_0.y + (t * i_0.dy)}, true);
    }
    return {{}, false};
}


// collinearity is not my problem
Circle CircumCircle(const Point& p_0, const Point& p_1, const Point& p_2) {
    auto a = p_0 - p_1;
    auto c = p_2 - p_1;
    double det = a.dx*c.dy - c.dx*a.dy;
    det = 0.5 / det;
    double asq = DotProduct(a, a);
    double csq = DotProduct(c, c);
    
    Indent ctr{asq*c.dy - csq*a.dy, csq*a.dx - asq*c.dx};
    ctr *= det;
    double r = sqrt(DotProduct(ctr, ctr));
    Circle res{ctr + p_1,  r};
    return res;
}

Circle CircumCircle(std::array<Point, 3>& ps) {
    return CircumCircle(ps[0], ps[1], ps[2]);
}


} // end namespace f
    
    
double HeronFormula(double a, double b, double c) {
    double p = (a + b + c) / 2;
    return sqrt(p * (p - a) * (p - b) * (p - c));
}
    
    
    
    
} // end namespace d2

}

}

