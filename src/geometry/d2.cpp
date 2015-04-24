

#include "ant/geometry/d2.h"

namespace ant {

namespace geometry {
    
namespace d2 {

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



}
    
}

}

}

