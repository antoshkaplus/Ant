

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

bool operator==(const Point& p_0, const Point& p_1) {
    return p_0.x == p_1.x && p_1.y == p_1.y;
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
    

    
std::pair<Point, bool> Intersection(const Segment& s_0, const Segment& s_1) {
    double x_00 = s_0.p_0.x;
    double y_00 = s_0.p_0.y;
    double x_01 = s_0.p_1.x;
    double y_01 = s_0.p_1.y;
    double x_10 = s_1.p_0.x;
    double y_10 = s_1.p_0.y;
    double x_11 = s_1.p_1.x;
    double y_11 = s_1.p_1.y;
    
    std::pair<Point, bool> r;    
    double d = (x_00 - x_01) * (y_10 - y_11) - (y_00 - y_01) * (x_10 - x_11);
    // parallel lines probably
    if (std::abs(d) < 1.e-14) { 
        r.second = false;
        return r;
    }
    
    int x_i = ((x_10 - x_11) * (x_00 * y_01 - y_00 * x_01) 
              - (x_00 - x_01) * (x_10 * y_11 - y_10 * x_11))/d;
    int y_i = ((y_10 - y_11) * (x_00 * y_01 - y_00 * x_01)
              - (y_00 - y_01) * (x_10 * y_11 - y_10 * x_11))/d;
    
    double min, max;
    std::tie(min, max) = std::minmax(x_00, x_01);
    if (x_i < min || x_i > max) {
        r.second = false;
        return r;  
    }  
    std::tie(min, max) = std::minmax(x_10, x_11);
    if (x_i < min || x_i > max) {
        r.second = false;
        return r;
    }
    std::tie(min, max) = std::minmax(y_00, y_01);
    if (y_i < min || y_i > max) {
        r.second = false;
        return r;   
    } 
    std::tie(min, max) = std::minmax(y_10, y_11);
    if (y_i < min || y_i > max) {
        r.second = false;
        return r;
    }
    r.second = true;
    r.first.set(x_i, y_i);
    return r;
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

