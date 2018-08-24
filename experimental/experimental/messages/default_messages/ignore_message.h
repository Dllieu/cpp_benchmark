#pragma once

#include <utils/macros.h>

namespace experimental
{
    template <typename MarketTraits>
    struct IgnoreMessage
    {
        force_inline void Process();
    };
}

#include <messages/default_messages/ignore_message.hxx>
