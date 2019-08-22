#pragma once

namespace ant::parallel::rcu {

template<typename TValue>
struct add_or_replace_result {

    using valueType = TValue;

    TValue& value;
    TValue& previous;
    bool replaced;

    add_or_replace_result(TValue& value, TValue& previous, bool replaced) noexcept
            : value(value), previous(previous), replaced(replaced) {
    }

    add_or_replace_result(add_or_replace_result&& other) = default;

    add_or_replace_result& operator=(add_or_replace_result&& other) = default;
};

}
