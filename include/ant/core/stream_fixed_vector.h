#pragma once

#pragma once


namespace ant
{

class PendingQueue
{
    constexpr static int ringBufferCapacity = 5000;

    std::array<BulkQuoteUpdate, ringBufferCapacity> ringBuffer {};
    std::unordered_map<BulkQuoteId, BulkQuoteUpdate<Allocator>> lateResponses {};

public:
    BulkQuoteUpdate Extract(BulkQuoteId bulkQuoteId)
    {
        BulkQuoteUpdate result {};
        auto index = bulkQuoteId.getIndex();
        auto& item = ringBuffer[index % ringBufferCapacity];
        if (item.getId().getIndex() != index)
        {
            auto it = lateResponses.find(bulkQuoteId);
            if (it == lateResponses.end())
            {
                throw Exception("Error");
            }
            result = std::move(it->second);
            lateResponses.erase(bulkQuoteId);
        }
        result = std::move(item);
        item = {};
    };

    template <template <typename T> typename OtherAllocator>
    BulkQuoteUpdate<Allocator>& Insert(BulkQuoteUpdate<OtherAllocator>&& update)
    {
        if (update.empty())
        {
            throw Exception("try insert empty BulkQuoteUpdate");
        }
        auto index = update.getId().getIndex();
        auto& item = ringBuffer[index % ringBufferCapacity];
        if (!item.empty())
        {
            logging::LogWarn("not empty item");
            if (item.getId().getIndex() == index)
            {
                throw Exception("same bulk quote id indexes");
            }
            lateResponses[item.getId()] = std::move(item);
        }
        item = update;
        return item;
    }
};

}
