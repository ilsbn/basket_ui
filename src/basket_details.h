#pragma once

#include <array>

namespace basket {

enum class BasketStatus { Empty, InProgress, Pending, Sold };

struct BasketDetails
{
    int index;
    int sequence;
    BasketStatus status;
};

using BasketArray = std::array<BasketDetails, 20>;

} // namespace basket
