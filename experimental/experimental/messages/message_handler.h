#pragma once

#include <cstddef>

namespace experimental
{
    template <typename... MessagesT>
    struct MessageHandler;

    template <typename MessageT, typename... MessagesT>
    struct MessageHandler<MessageT, MessagesT...>
    {
        static bool TryProcess(std::byte* iBuffer, std::size_t iBufferSize);
    };

    template <>
    struct MessageHandler<>
    {
        static bool TryProcess(std::byte* iBuffer, std::size_t iBufferSize);
    };
}

#include <messages/message_handler.hxx>
