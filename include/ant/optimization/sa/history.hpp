#pragma once

namespace ant::opt::sa {

    // expecting the minimization
    class HistoryItem {
    public:
        double best_cost = std::numeric_limits<double>::max();
        MinMax<double> temp;
        MinMax<double> accept_prob;

        void Add(double best_cost, const MinMax<double>& temp, const MinMax<double>& accept_prob) {
            this->best_cost = std::min(this->best_cost, best_cost);
            this->temp += temp;
            this->accept_prob += accept_prob;
        }

        void Add(double best_cost, double temp, double accept_prob) {
            this->best_cost = std::min(this->best_cost, best_cost);
            this->temp += temp;
            this->accept_prob += accept_prob;
        }

        void AddCost(double cost) {
            best_cost = std::min(best_cost, cost);
        }
    };

    class History {

        int64_t iter_count_;
        std::vector<HistoryItem> items_;
        Index current_item_ = -1;

    public:

        History() : iter_count_(0), items_(0) {}
        History(int64_t iter_count, Count item_count) : iter_count_(iter_count), items_(item_count) {
            assert(item_count <= iter_count);
        }

        HistoryItem& operator[](int64_t iter) {
            auto index = items_.size() * iter / iter_count_;
            return items_[index];
        }

        const std::vector<HistoryItem>& items() const {
            return items_;
        }
    };
}