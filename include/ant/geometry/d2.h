
#ifndef __ANT__D2_h__
#define __ANT__D2_h__


#include <vector>
#include <cmath>


#include "ant/core.h"


namespace ant {

namespace geometry {
    
namespace d2 {



template<class T>
struct Size {
    Size() : Size(0, 0) {}
    Size(T width, T height)
    : width(width), height(height) {}
    
    void set(T width, T height) {
        this->width = width;
        this->height = height;
    }
    T area() const {
        return width*height;
    }
    T perimeter() const {
        return 2*(height + width); 
    }
    bool isCovering(const Size<T>& s) const {
        return width >= s.width && height >= s.height;
    }
    void swap() {
        std::swap(height, width);
    }
    
    Size swapped() const {
        return Size(height, width);
    }
    
    T width, height;
};

template<class T>
bool operator==(const Size<T>& s_0, const Size<T>& s_1) {
    return s_0.width == s_1.width && s_0.height == s_1.height;
}
template<class T>
bool operator!=(const Size<T>& s_0, const Size<T>& s_1) {
    return s_0.width != s_1.width || s_0.height != s_1.height;
}


namespace i {
    
    using Size = d2::Size<size_t>;
    
    struct Point {
        Point() {}
        Point(Int x, Int y) : x(x), y(y) {}
        void set(Int x, Int y) {
            this->x = x;
            this->y = y;
        }
        void swap() {
            std::swap(x, y);
        }
        Int x, y;
    };
    
    struct Segment {
        Segment() {}
        Segment(const Point& fst, const Point& snd)
        : fst(fst), snd(snd) {}
        Point fst, snd;
        
        void swap() {
            std::swap(fst, snd);
        }
        
        Segment swapped() const {
            return Segment(snd, fst);
        }
    };
    
    bool operator==(const Point& p_0, const Point& p_1);
    bool operator!=(const Point& p_0, const Point& p_1);
    
    struct Rectangle {
        Rectangle() {}
        Rectangle(const Point& origin, const Size& size) 
        : origin(origin), size(size) {}
        Rectangle(Int x, Int y, Int width, Int height) 
        : origin(x, y), size(width, height) {}
        
        void set(Int x, Int y, size_t width, size_t height) {
            origin.set(x, y);
            size.set(width, height);
        }
        
        void set(const Point& origin, const Point& diag) {
            this->origin = origin;
            size.set(diag.x - origin.x, diag.y - origin.y);
        }
        
        void swap() {
            origin.swap();
            size.swap();
        }
        
        size_t area() const {
            return size.area();
        }
        
        size_t perimeter() const {
            return size.perimeter(); 
        }
        
        bool isIntersect(const Rectangle& r) const {
            return origin.x < r.origin.x + r.size.width  && origin.x + size.width  > r.origin.x &&
            origin.y < r.origin.y + r.size.height && origin.y + size.height > r.origin.y;
        }
        
        Point origin;
        Size size;
    };
    
    bool operator==(const Rectangle& r_0, const Rectangle& r_1);
    bool operator!=(const Rectangle& r_0, const Rectangle& r_1);    
    
    // oab
    // Returns a positive value, if p_0, p_1, p_2 makes a counter-clockwise turn,
    // negative for clockwise turn, and zero if the points are collinear.
    static int cross_product(const Point& p_0, const Point& p_1, const Point& p_2) {
        return (p_1.x - p_0.x)*(p_2.y - p_0.y) - (p_1.y - p_0.y)*(p_2.x - p_0.x);
    }
    
    std::vector<Index> convex_hull(const std::vector<Point>& ps, bool is_clockwise);    
    
} // namespace i


namespace f {
    
    struct Point {
        Point() : Point(0, 0) {}
        Point(Float x, Float y) : x(x), y(y) {}
        
        Float distance(const Point& p) const {
            Float 
            dx = p.x - x,
            dy = p.y - y;
            return sqrt(dx*dx + dy*dy);
        } 
        
        static Float distance(const Point& p_0, const Point& p_1) {
            return p_0.distance(p_1);
        }
        
        void set(Float x, Float y) {
            this->x = x;
            this->y = y;
        }
        
        Float x, y;
    };
    
    struct Indent {
        Indent() : Indent(0, 0) {}
        Indent(Float dx, Float dy) : dx(dx), dy(dy) {}
        Indent& operator+=(const Indent& d) {
            dx += d.dx;
            dy += d.dy;
            return *this;
        }
        double distance() {
            return sqrt(dx*dx + dy*dy);
        }
        
        Float dx, dy;
    };
    
    using Size = d2::Size<double>;
    
    struct Line {
        Line() : Line(0, 0, 0) {}
        Line(double a, double b, double c) : a(a), b(b), c(c) {}
        Line(const Point& p_0, const Point& p_1) {
            a = p_1.y - p_0.y;
            b = p_0.x - p_1.x;
            c = p_0.x*(p_0.y - p_1.y) + p_0.y*(p_1.x - p_0.x);
        }
        
        double a, b, c;
    };
    
    struct Circle {
        Circle() : radius(0) {}
        Circle(Point center, double radius) : center(center), radius(radius) {}
        Point center;
        double radius;
    };
    
    struct Rectangle {
        Rectangle() : origin(0, 0), size(0, 0) {}
        Rectangle(Float x, Float y, Float width, Float height) 
        : origin(x, y), size(width, height) {}
        Rectangle(Point origin, Size size) 
        : origin(origin), size(size) {}
        
        bool isInside(const Point& p) const {
            return p.x >= origin.x && p.y >= origin.y && 
            p.x <= origin.x+size.width && p.y <= origin.y+size.height;
        }
        
        Point origin;
        Size size; 
    };
    
    bool operator==(const Point& p_0, const Point& p_1);
    Indent operator-(const Point& p_0, const Point& p_1);
    Point operator+(const Point& p_0, const Point& p_1);            
    Indent operator/(const Indent& ind, Float d);   
    Point operator/(const Point& p, Float d);         
    Indent operator*(Float d, const Indent& ind);
    std::ostream& operator<<(std::ostream& output, const Point& p);
    
    std::pair<Point, Point> circleLineIntersection(const Circle& circle, const Line& line);
    
} // namespace f



} // namespace d2

} // namespace geometry

} // namespace ant


#endif

