
#include <fstream>
#include <array>
#include <functional>

#include "gtest/gtest.h"

#include "ant/machine_learning.h"
#include "ant/geometry/d2.h"

namespace {

using namespace ant;
using namespace ant::geometry::d2::f;
using namespace ant::ml;

// i will need some data folder

TEST(knn_test, allin) {
    int point_count = 100;
    std::vector<Point> points(point_count);
    for (int i = 0; i < point_count; ++i) {
        points[i].set(i, 0);
    }
    Point p(-1, 0);
    int k = 1;
    auto distance = [](const Point& p_0, const Point& p_1) {
        return p_0.distance(p_1);
    };
    std::vector<int> indices = FindNearestNeighbors(points, p, k, distance);
    std::sort(indices.begin(), indices.end());
    
    for (int i = 0; i < k; ++i) {
        ASSERT_EQ(indices[i], i);
    }
    
    k = 0;
    ASSERT_EQ(0, FindNearestNeighbors(points, p, k, distance).size());
    
    
    k = 1000;
    ASSERT_EQ(points.size(), FindNearestNeighbors(points, p, k, distance).size());
}
 
} // end anonymous namespace


TEST(binary_decision_tree, allin) {
    ASSERT_EQ(1, 0);
    std::ifstream input("./../data/digits.csv");
    const static Count N = 784;
    const static Count DIGIT_COUNT = 10;
    // first column is label
    std::string line;
    input >> line; // read column names
    std::vector<std::array<int, N>> digits;
    std::vector<int> categories;
    int example_index = 0;
    while (input >> line) {
        // values from 0 - 255
        size_t pos = 0;
        digits.resize(digits.size()+1);
        std::array<int, N>& example = digits.back();//                                                                                       v   digits[example_index];  
        for (int i = 0; i <= N; ++i) {
            int k = std::atoi(line.c_str() + pos);
            if (i == 0) {
                categories.push_back(k);
            } else {
                example[i-1] = k;
            }
            pos = line.find(',', pos) + 1;
        }
        ++example_index;
    }
    
    
    class TrainSet {
    public:
        TrainSet(const std::vector<std::array<int, N>>& digits) : digits_(digits) {}
        
        Count feature_count() const { return N; }
        Count example_count() const { return digits_.size(); }
        const std::array<int, N>& operator[](Index example_index) const {
            return digits_[example_index];
        }
    private:
        const std::vector<std::array<int, N>>& digits_;
    };
    
    class Category {
    public:
        Category(const std::vector<int>& categories) : categories_(categories) {}
        Index operator[](Index example_index) const {
            return categories_[example_index];
        }
    private:
        const std::vector<int>& categories_;
    };
    
    TrainSet train_set(digits);
    Category category(categories);
    
    class ConditionSet {
    public:
        using Condition = std::function<bool(const std::array<int, N>& example)>;
        using value_type = Condition;
        
        ConditionSet() {
            for (int i = 0; i < N; ++i) {
                conditions.push_back([=](auto example) { return example[i] < 255/2; });
            } 
        }
        
        const Condition& operator[](Index i) const {
            return conditions[i];
        }
        
        Count count() const { return conditions.size(); }
    private:
        std::vector<Condition> conditions;
    };
    
    ConditionSet condition_set;
    
    using Tree = binary_decision_tree<TrainSet, Category, ConditionSet, std::array<int, N>>; 
    
    Tree tree(train_set, category, condition_set, 10);
    for (int i = 0; i < train_set.example_count(); ++i) {
        Index c = tree.categorize(digits[i]);
        EXPECT_EQ(c, category[i]);
        //ASSERT_EQ(c, category[i]);
    }
}


TEST(logistic_regression, allin) {
    std::ifstream input("./../data/digits.csv");
    const static Count N = 784;
    const static Count DIGIT_COUNT = 10;
    // first column is label
    std::string line;
    input >> line; // read column names
    std::vector<std::array<int, N>> digits;
    std::vector<int> categories;
    int example_index = 0;
    while (input >> line) {
        // values from 0 - 255
        size_t pos = 0;
        digits.resize(digits.size()+1);
        std::array<int, N>& example = digits.back();//                                                                                       v   digits[example_index];  
        for (int i = 0; i <= N; ++i) {
            int k = std::atoi(line.c_str() + pos);
            if (i == 0) {
                categories.push_back(k);
            } else {
                example[i-1] = k;
            }
            pos = line.find(',', pos) + 1;
        }
        ++example_index;
    }
    
    Matrix<float> train_set(digits.size(), N);
    Matrix<Index> category(digits.size(), 1);
    for (auto r = 0; r < digits.size(); ++r) {
        category(r, 0) = categories[r];
        for (auto c = 0; c < digits[r].size(); ++c) {
            train_set(r, c) = digits[r][c]/255.;
        }
    }
    
    logistic_regression<float> regression(train_set, category, DIGIT_COUNT);
    Matrix<Index> whats = regression.categorize(train_set);
    Count wrong_count = 0;
    for (int i = 0; i < whats.row_count(); ++i) {
         if (whats(i, 0) != category[i]) ++wrong_count;
        //ASSERT_EQ(c, category[i]);
    }
    std::cout << "wrong count: " << wrong_count << " from: " << train_set.row_count() << "\n";

}




