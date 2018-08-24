#include <messages/message_handler.h>
#include <utils/macros.h>

namespace experimental
{
    template <typename MessageT, typename... MessagesT>
    bool MessageHandler<MessageT, MessagesT...>::TryProcess(std::byte* iBuffer, std::size_t iBufferSize)
    {
        if (likely(sizeof(MessageT) == iBufferSize))
        {
            MessageT* pMessage = reinterpret_cast<MessageT*>(iBuffer);
            pMessage->Process();

            return true;
        }
        else
        {
            return MessageHandler<MessagesT...>::TryProcess(iBuffer, iBufferSize);
        }
    }

    bool MessageHandler<>::TryProcess(std::byte* iBuffer, std::size_t iBufferSize)
    {
        return false;
    }
}
