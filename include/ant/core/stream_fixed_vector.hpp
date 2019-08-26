#pragma once

#include <vector>
#include <unordered_map>
#include <algorithm>

namespace ant
{

template <typename Size, typename Value>
class StreamFixedVector
{
    std::vector<Value> buffer {};
    std::vector<Size> indices {};
    std::unordered_map<Size, Value> lagging {};

    constexpr static Size EMPTY = std::numeric_limits<Size>::max();

public:

    StreamFixedVector() = default;
    StreamFixedVector(Size capacity) : buffer(capacity), indices(capacity, EMPTY) {}

    Value Extract(Size index)
    {
        Value result {};
        auto buffer_index = index % buffer.size();
        if (indices[buffer_index] != index)
        {
            auto node = lagging.extract(index);
            if (!node)
            {
                throw std::runtime_error("item not found");
            }
            return std::move(node.mapped());
        }
        indices[buffer_index] = EMPTY;
        return std::move(buffer[buffer_index]);
    };

    Value& Insert(Size index, Value&& update)
    {
        auto buffer_index = index % buffer.size();
        if (indices[buffer_index] != EMPTY) {
            lagging[indices[buffer_index]] = std::move(buffer[buffer_index]);
            indices[buffer_index] = index;
            return buffer[buffer_index] = std::move(update);
        }
        indices[buffer_index] = index;
        return buffer[buffer_index] = std::move(update);
    }

    Size lagging_count() const {
        return lagging.size();
    }

    Size size() const {
        return indices.size() - std::count(indices.begin(), indices.end(), EMPTY) + lagging.size();
    }
};

}
