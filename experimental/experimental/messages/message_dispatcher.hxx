#include <messages/message_dispatcher.h>
#include <messages/message_handler.h>
#include <algorithm>
#include <utility>

namespace experimental
{
    template <typename MarketTraits>
    MessageDispatcher<MarketTraits>::MessageDispatcher()
    {
        this->m_MessageTable.fill(&MessageHandler<>::TryProcess);
    }

    template <typename MarketTraits>
    template <typename MarketTraits::MessageTypeT MessageId, template <typename> typename... MessagesT>
    void MessageDispatcher<MarketTraits>::RegisterMessage()
    {
        static_assert(((MessageId == MessagesT<MarketTraits>::GetExpectedMessageId()) && ...), "MessageType mismastch!");
        static_assert(((true == MessagesT<MarketTraits>::EnsureLayoutAndMessageMatch()) && ...));

        this->m_MessageTable[static_cast<std::size_t>(MessageId)] = &MessageHandler<MessagesT<MarketTraits>...>::TryProcess;
    }

    template <typename MarketTraits>
    force_inline bool MessageDispatcher<MarketTraits>::TryProcess(MessageTypeT iMessageId, std::byte* iBuffer, std::size_t iBufferSize) const
    {
        return (*this->m_MessageTable[static_cast<std::size_t>(iMessageId)])(iBuffer, iBufferSize);
    }
}
