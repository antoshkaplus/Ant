
#include <array>
#include <fstream>


#include "ant/geometry/d2.hpp"
#include "ant/geometry/triangle/delaunay_triangulation_i.hpp"


using namespace std;
using namespace ant;
using namespace ant::geometry::d2::i;

vector<Point> CreateTestCase(Count c) {
    vector<Point> v;
    PointGenerator g({-1000, -1000}, {1000, 1000});
    Point p;
    for (Index i = 0; i < c; ++i) {
        while (count(v.begin(), v.end(), p = g()) != 0);
        v.push_back(p);
    }
    return v;
}


void Output(ostream& output, const vector<Point>& ps) {
    output << ps.size() << endl;
    for (auto p : ps) {
        output << p.x << " " << p.y << endl;
    }
}


int main(int argc, char **argv) {
//    ofstream out("./../data/points_1000.txt");
//    auto ps = CreateTestCase(1000);
//    Output(out, ps);
//
//    return 0;

    std::ifstream input("./../data/points_1000.txt");
    int point_count;
    input >> point_count;
    std::vector<Point> points(point_count);
    for (int i = 0; i < point_count; ++i) {
        input >> points[i].x >> points[i].y;
    }
    Rectangle r = CircumRectangle(points);
    r.origin.x -= 1;
    r.origin.y -= 1;
    r.size.width += 2;
    r.size.height += 2;
    auto t = CircumTriangle(r);
    
    DelaunayTriangulation triangulation;
    auto trgs = triangulation.Compute(points, t);
    
    std::ofstream output("./../temp/points_1000.txt");
    output << trgs.size() << endl;
    for (auto t : trgs) {
        output << t[0] << " " << t[1] << " " << t[2] << endl;
    }
}

