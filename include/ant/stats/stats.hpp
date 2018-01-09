#pragma once

#include "ant/core/core.hpp"

namespace ant::stats {

// return optional for case of empty vector
//double median(std::vector<double>& vs) {
//    return *std::nth_element(vs.begin(), vs.end(), vs.size()/2);
//}
//
//double atStart(std::vector<double>& vs, double nextPersantage) {
//    return 0;
//}
//
//double atEnd(std::vector<double>& vs, double nextPersantage) {
//    return 0;
//}
//
//std::vector<double> skipLess(const std::vector<double>& vs, double factor, double limit) {
//    std::vector<double> ps;
//    for (auto v : vs) {
//        if (v*factor < limit) ps.push_back(v)
//    }
//
//}

//
//struct SampleStats {
//
//};
//
//
//SampleStats ComputeSampleStats(const std::vector<double>& vals) {
//    // mean
//    // median
//    // deviation
//    // variance
//    // quantile
//}

class Rescaling {

    double min;
    double max;

public:

    double scale(double val) {
        return (val - min) / (max - min);
    }

};



}
