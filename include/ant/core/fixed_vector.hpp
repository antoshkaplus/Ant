#pragma once



template <typename Value, size_t kCapacity>
class FixedVector {
    Count size_ {};
    std::array<Value, kCapacity> array {};

public:
    FixedVector() {}

    FixedVector(FixedVector&& fv) {
        *this = std::move(fv);
    }

    FixedVector(const FixedVector& fv) {
        *this = fv;
    }

//	bool push_back(Value&& value) {
//		if (size_ == kCapacity) return false;
//		array[size_++] = std::move<Value>(value);
//	}

    bool push_back(const Value& value) {
        if (size_ == kCapacity) return false;
        array[size_++] = value;
    }

    template <typename ...Args>
    bool emplace_back(Args&&... args) {
        if (size_ == kCapacity) return false;
        array[size_++] = Value(args...);
    }

    auto& operator=(FixedVector&& fv) {
        std::copy(fv.array.begin(), fv.array.begin()+fv.size(), array.begin());
        size_ = fv.size();
        fv.size_ = 0;
        return *this;
    }

    auto& operator=(const FixedVector& fv) {
        std::copy(fv.array.begin(), fv.array.begin()+fv.size(), array.begin());
        size_ = fv.size();
        return *this;
    }

    auto begin() {
        return array.begin();
    }

    auto end() {
        return array.begin() + size_;
    }

    bool empty() const {
        return size_ == 0;
    }

    Count size() const {
        return size_;
    }

    Value& operator[](Count index) {
        return array[index];
    }

    void clear() {
        size_ = 0;
    }

    const Value& operator[](Count index) const {
        return array[index];
    }
};