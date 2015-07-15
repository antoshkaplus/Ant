
#ifndef __ANT__D2_h__
#define __ANT__D2_h__


#include <vector>
#include <cmath>
#include <queue>
#include <random>


#include "ant/core/core.hpp"


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

using Size = d2::Size<Int>;

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
    
    void Swap() {
        std::swap(fst, snd);
    }
    
    Segment Swapped() const {
        return Segment(snd, fst);
    }
    
private:
    // are listed in counterclockwise order
    // later should put it outside
    static bool CCW(const Point& A, const Point& B, const Point& C) {
        return (C.y-A.y) * (B.x-A.x) > (B.y-A.y) * (C.x-A.x);
    }

public:

    bool Intersect(const Segment& s) const {
        return CCW(fst, s.fst, s.snd) != CCW(snd, s.fst, s.snd) && 
            CCW(fst, snd, s.fst) != CCW(fst, snd, s.snd);
    }
    
    bool Lie(Point q) const
    {
        Int x_min, x_max, y_min, y_max;
        std::tie(x_min, x_max) = std::minmax(fst.x, snd.x);
        std::tie(y_min, y_max) = std::minmax(fst.y, snd.y);
        return ((q.x - fst.x)*(snd.y - fst.y) == (q.y - fst.y)*(snd.x - fst.x) &&
                q.x <= x_max && q.x >= x_min && q.y <= y_max && q.y >= y_min);
    }
        
    bool IntersectOrLie(const Segment& s) const {
        int o1 = Orientation(fst, snd, s.fst);
        int o2 = Orientation(fst, snd, s.snd);
        int o3 = Orientation(s.fst, s.snd, fst);
        int o4 = Orientation(s.fst, s.snd, snd);
        
        // General case
        if (o1 != o2 && o3 != o4)
            return true;
        
        // Special Cases
        // p1, q1 and p2 are colinear and p2 lies on segment p1q1
        if (o1 == 0 && Lie(s.fst)) return true;
        
        // p1, q1 and p2 are colinear and q2 lies on segment p1q1
        if (o2 == 0 && Lie(s.snd)) return true;
        
        // p2, q2 and p1 are colinear and p1 lies on segment p2q2
        if (o3 == 0 && s.Lie(fst)) return true;
        
        // p2, q2 and q1 are colinear and q1 lies on segment p2q2
        if (o4 == 0 && s.Lie(snd)) return true;
        
        return false; // Doesn't fall in any of the above cases
    }
    
private:
    // To find orientation of ordered triplet (p, q, r).
    // The function returns following values
    // 0 --> p, q and r are colinear
    // 1 --> Clockwise
    // 2 --> Counterclockwise
    int Orientation(Point p, Point q, Point r) const
    {
        // See 10th slides from following link for derivation of the formula
        // http://www.dcs.gla.ac.uk/~pat/52233/slides/Geometry1x1.pdf
        int val = (q.y - p.y) * (r.x - q.x) -
                    (q.x - p.x) * (r.y - q.y);
        if (val == 0) return 0;  // colinear
        return (val > 0) ? 1 : 2; // clock or counterclock wise
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

using Triangle = std::array<Point, 3>;

// works only for simple polygons    
int ShoelaceFormula(const std::vector<Point>& ps);
int ShoelaceFormula(const std::vector<Point>& ps, const std::vector<Index>& order);
bool Collinear(const Point& p_0, const Point& p_1, const Point& p_2);
Triangle CircumTriangle(const Rectangle& r);
Rectangle CircumRectangle(const vector<Point>& ps);

struct Polygon {
    std::vector<Point> points;
    
    double ComputeArea() const {
        
        
        
        return 0;
    }
};



    
} // namespace i


namespace f {
    
    
struct Point {
    Point() : Point(0, 0) {}
    Point(Float x, Float y) : x(x), y(y) {}
    
    Float Distance(const Point& p) const {
        Float 
        dx = p.x - x,
        dy = p.y - y;
        return sqrt(dx*dx + dy*dy);
    } 
    
    static Float Distance(const Point& p_0, const Point& p_1) {
        return p_0.Distance(p_1);
    }
    
    template<class RNG>
    static Point Random(RNG& rng) {
        std::uniform_real_distribution<> distr;
        return {distr(rng), distr(rng)};
    }
    
    void set(Float x, Float y) {
        this->x = x;
        this->y = y;
    }
    
    void Rotate(const Point& center, Float angle) {
        double s = std::sin(angle);
        double c = std::cos(angle);
        x -= center.x;
        y -= center.y;
        Point p;
        p.x = x * c - y * s + center.x;
        p.y = x * s + y * c + center.y;
        *this = p;
    }
    
    Float x, y;
};

Point& operator+=(Point& p_0, const Point& p_1);    
Point& operator/=(Point& p_0, Float f);
Point operator+(Point p_0, const Point& p_1);
Point operator/(Point p_0, Float f);
Point operator*(Float f, Point p_0);

struct Indent {
    constexpr Indent() : Indent(0, 0) {}
    constexpr Indent(Float dx, Float dy) : dx(dx), dy(dy) {}
    
    Indent& operator+=(const Indent& d) {
        dx += d.dx;
        dy += d.dy;
        return *this;
    }
    
    Float distance() const {
        return sqrt(dx*dx + dy*dy);
    }
    
    Indent normed() const {
        auto d = distance();
        return {dx/d, dy/d};
    }
    
    Float dx, dy;
};

Indent& operator/=(Indent& i, Float f);
Indent& operator*=(Indent& i, Float f); 
Indent operator/(Indent i, Float f);
Indent operator*(Indent i, Float f);
Indent operator+(Indent i_0, Indent i_1);

Point operator+(Indent i, Point p);
Point operator-(Indent i, Point p); 
Point operator+(Point p, Indent i);
Point operator-(Point p, Indent i); 

Point& operator+=(Point& p, Indent i);
Indent operator-(const Point& p_0, const Point& p_1); 
Indent operator*(Float d, Indent i);


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
    
    bool Overlap(const Circle& c) const {
        return radius+c.radius - center.Distance(c.center) > 0;
    }
    
    Float Area() const {
        return radius*radius*M_PI;
    }
    
    bool IsInside(const Point& p) {
        return center.Distance(p) < radius;
    }
    
    Point center;
    double radius;
};

Circle CircumCircle(const Point& p_0, const Point& p_1, const Point& p_2);


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

struct Segment {
    Point p_0, p_1;
    
    Segment(Point p_0, Point p_1)
        : p_0(p_0), p_1(p_1) {}
};


std::pair<Point, bool> Intersection(const Segment& s_0, const Segment& s_1);
bool operator==(const Point& p_0, const Point& p_1);
std::ostream& operator<<(std::ostream& output, const Point& p);
std::pair<Point, Point> circleLineIntersection(const Circle& circle, const Line& line);
    
} // namespace f

    

double HeronFormula(double a, double b, double c);


// oab
// Returns a positive value, if p_0, p_1, p_2 makes a counter-clockwise turn,
// negative for clockwise turn, and zero if the points are collinear.
template<class P>
double CrossProduct(const P& p_0, const P& p_1, const P& p_2) {
    return (p_1.x - p_0.x)*(p_2.y - p_0.y) - (p_1.y - p_0.y)*(p_2.x - p_0.x);
}

// can make it general for any number of points
template<class P>
double DotProduct(const P& p_0, const P& p_1) {
    return p_0.dx*p_1.dx + p_0.dy*p_1.dy;
}





//  before optimization
//  ap = (p_1.y - p_2.y)*(p.x - p_2.x) + (p_2.x - p_1.x)*(p.y - p_2.y);
//  bp = (p_2.y - p_0.y)*(p.x - p_2.x) + (p_0.x - p_2.x)*(p.y - p_2.y);
//  k = (p_1.y - p_2.y)*(p_0.x - p_2.x) + (p_2.x - p_1.x)*(p_0.y - p_2.y);
template<class P, class V = double>
class PointTriangle {
    P p_2;
    P d_12;
    P d_20;
    V k;
    
    struct Coords {
        V ap;
        V bp;
        V ck;
    };
    
    Coords ComputeCoords(const P& p) {
        Coords s;
        s.ap = d_12.y * (p.x - p_2.x) + d_12.x * (p_2.y - p.y);
        s.bp = d_20.y * (p.x - p_2.x) + d_20.x * (p_2.y - p.y);
        s.ck = k - s.ap - s.bp;
        return s;
    }
    
public:
    
    PointTriangle(const P& p_0, const P& p_1, const P& p_2)
        : p_2(p_2) {
        
        d_12 = P{p_1.x - p_2.x, p_1.y - p_2.y};
        d_20 = P{p_2.x - p_0.x, p_2.y - p_0.y};
        k = -d_20.x * d_12.y + d_12.x * d_20.y; 
    }
    
    bool Lies(const P& p) {
        auto s = ComputeCoords(p);
        return s.ap == 0 || s.bp == 0 || s.ck == 0;
    } 
    
    bool IsInside(const P& p) {
        auto s = ComputeCoords(p);
        if (k < 0) {
            // change signs
            return 0 > s.ap && s.ap >= k && 0 > s.bp && s.bp >= k && 0 > s.ck && s.ck >= k;
        } 
        return 0 < s.ap && s.ap <= k && 0 < s.bp && s.bp <= k && 0 < s.ck && s.ck <= k;
    }
    
    bool IsInsideOrLies(const P& p) {
        auto s = ComputeCoords(p);
        if (k < 0) {
            // change signs
            return 0 >= s.ap && s.ap >= k && 0 >= s.bp && s.bp >= k && 0 >= s.ck && s.ck >= k;
        } 
        return 0 <= s.ap && s.ap <= k && 0 <= s.bp && s.bp <= k && 0 <= s.ck && s.ck <= k;
    }
};




  
// always counter clockwise
// to make clockwise just reverse
template<class P> // should have x and y inside
std::vector<Index> ConvexHull(const std::vector<P>& ps) {
    std::vector<Index> inds;
    int n = (int)ps.size();
    std::vector<Index> order(n);
    for (int i = 0; i < n; i++) order[i] = i;
    // Sort points lexicographically
    sort(order.begin(), order.end(), 
         [&ps](int i1, int i2){ return ps[i1].x < ps[i2].x || 
             (ps[i1].x == ps[i2].x && ps[i1].y < ps[i2].y); });
    inds.resize(2*n);
    int k = 0;
    // Build lower hull
    for (int i = 0; i < n; i++) {
        while (k >= 2 && CrossProduct(ps[inds[k-1]], ps[inds[k-2]], ps[order[i]]) <= 0) k--;
        inds[k++] = order[i];
    }
    // Build upper hull
    for (int i = n-2, t = k+1; i >= 0; i--) {
        while (k >= t && CrossProduct(ps[inds[k-1]], ps[inds[k-2]], ps[order[i]]) <= 0) k--;
        inds[k++] = order[i];
    }
    inds.resize(k-1);
    return inds;
}

template<class P>
double Perimeter(const std::vector<P>& ps, const std::vector<Index>& order, bool isClosed) {
    double s = 0.;
    for (Index i = 0; i < order.size()-1; i++) {
        s += ps[order[i]].Distance(ps[order[i+1]]);
    }
    if (isClosed) s += ps[order[0]].Distance(ps[order[order.size()-1]]);
    return s;
}

// result is stored incide inds. inds - from which indices we choose
template<class P>
void K_NearestPoints(const std::vector<P>& ps, const P& p, 
                                   std::vector<Index>& inds, int k) {
    k = std::min(k, (int)inds.size());
    partial_sort(inds.begin(), inds.begin()+k, inds.end(), 
                 [&ps, p](int i1, int i2){ return ps[i1].Distance(p) < ps[i2].Distance(p); });
    inds.resize(k);
}

template<class P>
Index NearestPoint(const std::vector<P>& ps, const std::vector<Index>& indices, const P& p) {
    Index i_min = indices[0];
    for (Index i : indices) {
        if (p.Distance(ps[i]) < p.Distance(ps[i_min])) {
            i_min = i;
        }
    }
    return i_min;
}


template <class ForwardIterator, class P>
ForwardIterator NearestPoint(ForwardIterator first, ForwardIterator last, const P& p) {
    return std::min_element(first, last, [&p](const P& p_0, const P& p_1) {
        return p_0.distance(p) < p_1.distance(p);
    });
}

template <class ForwardIterator, class P>
ForwardIterator NearestPoint(const std::vector<P>& points, 
                             ForwardIterator firstIndex, 
                             ForwardIterator lastIndex, 
                             const P& p) {
    return std::min_element(firstIndex, lastIndex, [&](size_t i_0, size_t i_1) {
        return points[i_0].Distance(p) < points[i_1].Distance(p);
    });
}

template <class ForwardIterator, class P>
ForwardIterator FarthestPoint(const std::vector<P>& points,
                              ForwardIterator firstIndex,
                              ForwardIterator lastIndex,
                              const P& p) {
    return std::max_element(firstIndex, lastIndex, [&](Index i_0, Index i_1) {
        return points[i_0].distance(p) < points[i_1].distance(p);
    });
}






} // namespace d2

} // namespace geometry

} // namespace ant


#endif

