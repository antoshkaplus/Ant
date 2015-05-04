

#include "ant/geometry/d2.h"

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

}



namespace f {


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



} // end namespace f
    
    
double HeronFormula(double a, double b, double c) {
    double p = (a + b + c) / 2;
    return sqrt(p * (p - a) * (p - b) * (p - c));
}
    
    
    
    
} // end namespace d2

}

}

