#pragma once

#include <array>
#include <cstddef>
#include <utils/macros.h>

namespace experimental
{
    template <typename MarketTraits>
    class MessageDispatcher
    {
    public:
        using MessageCallbackT = bool (*)(std::byte*, std::size_t);
        using MessageTypeT = typename MarketTraits::MessageTypeT;

        MessageDispatcher();

        template <MessageTypeT MessageId, template <typename> typename... MessagesT>
        void RegisterMessage();

        force_inline bool TryProcess(MessageTypeT iMessageId, std::byte* iBuffer, std::size_t iBufferSize) const;

    private:
        static_assert(sizeof(MessageTypeT) <= sizeof(std::int16_t), "MessageType too big to be stored in a table");
        std::array<MessageCallbackT, std::numeric_limits<std::make_unsigned_t<std::underlying_type_t<MessageTypeT>>>::max() + 1> m_MessageTable;
    };
}

#include <messages/message_dispatcher.hxx>
