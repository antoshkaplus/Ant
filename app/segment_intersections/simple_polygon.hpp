// ===================================================================
// simple_polygon.h - Class for a polygon
// Written by Glenn Burkhardt (2014)
/*
 * simple_polygon.h
 *
 */

#ifndef SIMPLE_POLYGON_H_
#define SIMPLE_POLYGON_H_

#include <stdlib.h>
#include <vector>

struct Point {
    double x, y;
    
    Point() {}
    Point(double x, double y) : x(x), y(y) {}
};

struct Polygon {
    Polygon(int npts) : n(npts), V(n) {}
    
public:
    int n;
    vector<Point> V;
    // should have n elements, V[n-1] != V[0]
};

typedef struct Polygon Polygon;

// simple_Polygon(): test if a Polygon P is simple or not
//     Input:  Pn = a polygon with n vertices V[]
//     Return: FALSE(0) = is NOT simple
//             TRUE(1)  = IS simple
bool simple_Polygon( Polygon &Pn );

#endif /* SIMPLE_POLYGON_H_ */
