#pragma once

namespace ant {

struct HeightGen {
    HeightGen() {}

    HeightGen(int max, float prob) {
        std::vector<double> probs(max);
        for (auto& p : probs) {
            p = prob;
            prob *= 0.5;
        }
        distr = std::discrete_distribution<int>(probs.begin(), probs.end());
    }
    ~HeightGen() {}

    int operator()() {
        return distr(rng) + 1;
    }

private:
    std::default_random_engine rng;
    std::discrete_distribution<int> distr;
};

}