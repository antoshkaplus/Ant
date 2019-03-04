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
        Count item_capacity_;

    public:

        History() : iter_count_(0), items_(0), item_capacity_(0) {}
        History(int64_t iter_count, Count item_capacity) : iter_count_(iter_count),
            items_(std::min<int64_t>(iter_count, item_capacity)), item_capacity_(item_capacity)
        {
            assert(items_.size() <= iter_count);
        }

        void IncreaseIterations(int64_t new_iter_count) {
            assert(new_iter_count >= iter_count_);

            if (new_iter_count == iter_count_) return;

            if (new_iter_count <= item_capacity_) {

                items_.resize(new_iter_count);

            } else if (items_.size() < item_capacity_) {

                // it's still one item per iteration right now
                // new_size equals item_capacity
                for (auto i = 0; i < items_.size(); ++i) {
                    auto new_i = item_capacity_ * i / new_iter_count;
                    items_[new_i] = items_[i];
                    items_[i] = {};
                }
                items_.resize(item_capacity_);
            } else {

                for (auto i = 0; i < items_.size(); ++i) {
                    auto new_i = iter_count_ * i / new_iter_count;
                    items_[new_i] = items_[i];
                    items_[i] = {};
                }
            }

            // super important
            iter_count_ = new_iter_count;
        }

        HistoryItem& operator[](int64_t iter) {
            auto index = items_.size() * iter / iter_count_;
            return items_[index];
        }

        const std::vector<HistoryItem>& items() const {
            return items_;
        }

        int64_t iter_count() const {
            return iter_count_;
        }
    };
}