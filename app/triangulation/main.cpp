
#include <array>
#include <fstream>


#include "ant/geometry/d2.hpp"
#include "ant/geometry/triangle/delaunay_triangulation_i.hpp"


using namespace std;
using namespace ant;
using namespace ant::geometry::d2::i;
using namespace ant::geometry;

vector<Point> GenerateProblem(Count pointCount, const Point& minP, const Point& maxP) {
    PointGenerator g(minP, maxP);
    vector<Point> ps;
    ps.reserve(pointCount);
    Point p;
    for (Index i = 0; i < pointCount; ++i) {
        while (count(ps.begin(), ps.end(), p = g()) != 0);
        ps.push_back(p);
    }
    return ps;
}

vector<triangle::Triangle> Solve(const vector<Point>& ps) {
    Rectangle r = CircumRectangle(ps);
    r.origin.x -= 1;
    r.origin.y -= 1;
    r.size.width += 2;
    r.size.height += 2;
    auto t = CircumTriangle(r);

    DelaunayTriangulation triangulation;
    return triangulation.Compute(ps, t);
}

vector<Point> ReadProblem(istream& in) {
    int point_count;
    in >> point_count;
    std::vector<Point> points(point_count);
    for (int i = 0; i < point_count; ++i) {
        in >> points[i].x >> points[i].y;
    }
    return points;
}

void PrintProblem(ostream& out, const vector<Point>& ps) {
    out << ps.size() << endl;
    for (auto p : ps) {
        out << p.x << " " << p.y << endl;
    }
}

void PrintSolution(ostream& out, const vector<triangle::Triangle>& ts) {
    out << ts.size() << endl;
    for (auto t : ts) {
        out << t[0] << " " << t[1] << " " << t[2] << endl;
    }
}


int main(int argc, const char *argv[]) {
    command_line_parser parser(argv, argc);

    ofstream out(parser.getValue("out"));

    if (parser.exists("gen")) {
        auto p_x = Split(parser.getValue("x"), ':');
        auto x_min = atoi(p_x[0]);
        auto x_max = atoi(p_x[1]);

        auto p_y = Split(parser.getValue("y"), ':');
        auto y_min = atoi(p_y[0]);
        auto y_max = atoi(p_y[1]);

        auto p_count = atoi(parser.getValue("count"));

        auto problem = GenerateProblem(p_count, {x_min, y_min}, {x_max, y_max});

        PrintProblem(out, problem);

        if (parser.exists("solve")) {
            auto ts = Solve(problem);
            PrintSolution(out, ts);
        }
    } else if (parser.exists("in")) {

        ifstream in(parser.getValue("in"));
        auto problem = ReadProblem(in);
        PrintProblem(out, problem);
        auto ts = Solve(problem);
        PrintSolution(out, ts);
    }
}

