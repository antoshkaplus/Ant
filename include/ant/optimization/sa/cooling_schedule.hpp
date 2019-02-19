#pragma once

namespace ant::opt::sa {

//    class CoolingSchedule {
//        double temperature(Index iteration);
//        double next()
//    };


////////////////////////////////////////////////////////////
// MONOTONIC ONLY
//
// materials:
// http://what-when-how.com/artificial-intelligence/a-comparison-of-cooling-schedules-for-simulated-annealing-artificial-intelligence/


////////////////////////////////////////////////////////////
// Multiplicative

// T_k = T_0 * alpha^k, 0.8 <= alpha <= 0.9
// good for max of thousand iterations or less
class ExponentialMultiplicativeCooling {
    double T_0;
    double base_alpha;

    Index lastIteration = 0;
    double lastAlpha = 1;

public:
    ExponentialMultiplicativeCooling(double T_0, double base_alpha)
        : T_0(T_0), base_alpha(base_alpha) {

        if (base_alpha < 0.8 || base_alpha > 0.9) throw std::invalid_argument("ExponentialMultiplicativeCooling invalid alpha");
    }

    double temperature(Index iteration) {
        lastAlpha = lastAlpha * std::pow(base_alpha, iteration - lastIteration);
        lastIteration = iteration;

        return T_0 * lastAlpha;
    }
};

// T_k = T_0 / (1 + alpha * log(1+k)), alpha > 1
class LogarithmicalMultiplicativeCooling {

    double T_0;
    double alpha;

public:
    LogarithmicalMultiplicativeCooling(double T_0, double alpha)
        : T_0(T_0), alpha(alpha) {

        if (alpha <= 1) throw std::invalid_argument("LogarithmicalMultiplicativeCooling invalid alpha");
    }

    double temperature(Index iteration) {
        return T_0 / ( 1. + alpha * std::log(1 + iteration) );
    }
};

// one of the good ones
class LinearMultiplicativeCooling {

    double T_0;
    double alpha;

public:
    LinearMultiplicativeCooling(double T_0, double alpha)
        : T_0(T_0), alpha(alpha) {

        if (alpha <= 0) throw std::invalid_argument("LinearMultiplicativeCooling invalid alpha");
    }

    double temperature(Index iteration) {
        return T_0 / (1. + alpha * iteration);
    }
};

class QuadraticMultiplicativeCooling {

    double T_0;
    double alpha;

public:
    QuadraticMultiplicativeCooling(double T_0, double alpha)
        : T_0(T_0), alpha(alpha) {

        if (alpha <= 0) throw std::invalid_argument("QuadraticMultiplicativeCooling invalid alpha");
    }

    double temperature(Index iteration) {
        return T_0 / (1. + alpha * iteration * iteration);
    }
};

class PolynomialCooling {
    double power;

public:
    PolynomialCooling(double power) : power(power) {}

    double temperature(Index iteration) {
        return 1. / (1. + std::pow(iteration, power));
    }
};

///////////////////////////////////////////////////////////////////
// Additive

class LinearAdditiveCooling {

    double T_0;
    double T_n;
    Count iteration_count;

    double alpha;

public:
    LinearAdditiveCooling(double T_0, double T_n, Count iteration_count)
        : T_0(T_0), T_n(T_n), iteration_count(iteration_count) {

        alpha = T_n + (T_0 - T_n) / iteration_count;
    }

    double temperature(Index iteration) {
        return (iteration_count - iteration) * alpha;
    }
};

class QuadraticAdditiveCooling {
    double T_0;
    double T_n;
    Count iteration_count;

    double alpha;

public:
    QuadraticAdditiveCooling(double T_0, double T_n, Count iteration_count)
        : T_0(T_0), T_n(T_n), iteration_count(iteration_count) {

        alpha = T_n + (T_0 - T_n) / (iteration_count*iteration_count);
    }

    double temperature(Index iteration) {
        return alpha * (iteration_count - iteration) * (iteration_count - iteration);
    }
};

class ExponentialAdditiveCooling {
    double T_0;
    double T_n;
    Count iteration_count;

    double alpha;

public:
    ExponentialAdditiveCooling(double T_0, double T_n, Count iteration_count)
        : T_0(T_0), T_n(T_n), iteration_count(iteration_count) {

        alpha = 2. * std::log(T_0 - T_n) / iteration_count;
    }

    double temperature(Index iteration) {
        return T_n + (T_0 - T_n) / (1. + std::exp( alpha * (iteration - iteration_count/2.)));
    }
};

class TrigonometricAdditiveCooling {

    double T_0;
    double T_n;
    Count iteration_count;

public:
    TrigonometricAdditiveCooling(double T_0, double T_n, Count iteration_count)
        : T_0(T_0), T_n(T_n), iteration_count(iteration_count) {}

    double temperature(Index iteration) {
        return T_n + (T_0 - T_n) * (1. + std::cos(iteration * M_PI / iteration_count)) / 2.;
    }
};

}