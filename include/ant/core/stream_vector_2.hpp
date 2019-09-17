#pragma once

template <typename Size, typename Value>
class StreamVector
{
    std::vector<std::pair<Size, Value>> buffer {};
    std::unordered_map<Size, Value> lagging {};

    constexpr static Size EMPTY = std::numeric_limits<Size>::max();

public:

    StreamVector() = default;

    StreamVector(Size capacity) : buffer(capacity, std::pair<Size, Value>{EMPTY, Value{}})
    {
    }

    Value Extract(Size index)
    {
        Value result {};
        auto buffer_index = index % buffer.size();
        if (buffer[buffer_index].first != index)
        {
            auto node = lagging.extract(index);
            if (!node)
            {
                throw Exception("item not found");
            }
            return std::move(node.mapped());
        }
        buffer[buffer_index].first = EMPTY;
        return std::move(buffer[buffer_index].second);
    };

    Value& Insert(Size index, Value&& update)
    {
        auto buffer_index = index % buffer.size();
        if (buffer[buffer_index].first != EMPTY)
        {
            lagging[buffer[buffer_index].first] = std::move(buffer[buffer_index].second);
            return (buffer[buffer_index] = {index, std::move(update)}).second;
        }
        return (buffer[buffer_index] = {index, std::move(update)}).second;
    }

    Size lagging_count() const
    {
        return lagging.size();
    }

    Size size() const
    {
        return buffer.size() - std::count_if(buffer.begin(), buffer.end(), [](auto& item) { return item.first == EMPTY; }) + lagging.size();
    }
};
