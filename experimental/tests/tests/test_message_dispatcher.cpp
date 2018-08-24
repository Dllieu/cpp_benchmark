#include <gtest/gtest.h>
#include <messages/message.h>
#include <messages/message_dispatcher.h>
#include <messages/default_messages/ignore_message.h>
#include <utils/user_defined_literals.h>
#include <utils/macros.h>

namespace
{
    DECLARE_LAYOUT_PACKED(NewOrderMessage, 18,
        (std::uint64_t, OrderId)
        (std::uint64_t, Price)
        (std::uint16_t, Quantity));

    DECLARE_LAYOUT_PACKED(DeprecatedNewOrderMessage, 20,
        (std::uint64_t, OrderId)
        (std::uint64_t, Price)
        (std::uint16_t, Quantity)
        (std::uint16_t, Reserved));

    DECLARE_LAYOUT_PACKED(CancelOrderMessage, 8,
        (std::uint64_t, OrderId));

    enum class MessageType : std::uint16_t
    {
        NewOrder = "NO"_AlphaNumeric,
        CancelOrder = "CO"_AlphaNumeric,
        Login = "LO"_AlphaNumeric,
    };

    DECLARE_DEFAULT_MESSAGE(NewOrderMessage, MessageType::NewOrder, experimental::IgnoreMessage);
    DECLARE_DEFAULT_MESSAGE(DeprecatedNewOrderMessage, MessageType::NewOrder, experimental::IgnoreMessage);
    DECLARE_DEFAULT_MESSAGE(CancelOrderMessage, MessageType::CancelOrder, experimental::IgnoreMessage);
}

namespace
{
    struct MarketTraits
    {
        using MessageTypeT = MessageType;
    };

    struct MessageDispatcherTest : public testing::Test
    {
        experimental::MessageDispatcher<MarketTraits> m_MessageDispatcher;
        std::array<std::byte, 1024> m_Buffer{};
    };
}

TEST_F(MessageDispatcherTest, Dispatcher)
{
    EXPECT_FALSE(this->m_MessageDispatcher.TryProcess(MessageType::NewOrder, this->m_Buffer.data(), sizeof(NewOrderMessageLayout)));

    this->m_MessageDispatcher.RegisterMessage<MessageType::NewOrder, NewOrderMessage, DeprecatedNewOrderMessage>();

    EXPECT_TRUE(this->m_MessageDispatcher.TryProcess(MessageType::NewOrder, this->m_Buffer.data(), sizeof(NewOrderMessageLayout)));
    EXPECT_TRUE(this->m_MessageDispatcher.TryProcess(MessageType::NewOrder, this->m_Buffer.data(), sizeof(DeprecatedNewOrderMessageLayout)));
    EXPECT_FALSE(this->m_MessageDispatcher.TryProcess(MessageType::NewOrder, this->m_Buffer.data(), sizeof(CancelOrderMessageLayout)));
}

namespace
{
    DECLARE_LAYOUT_PACKED(LoginMessage, 8,
        (std::uint64_t, LoginId));

    DECLARE_MESSAGE(LoginMessage, MessageType::Login);

    template <typename MarketTraits>
    void LoginMessage<MarketTraits>::Process()
    {
        this->SetLoginId(42);
    }
}

TEST_F(MessageDispatcherTest, DispatcherFlow)
{
    this->m_MessageDispatcher.RegisterMessage<MessageType::Login, LoginMessage>();

    EXPECT_TRUE(this->m_MessageDispatcher.TryProcess(MessageType::Login, this->m_Buffer.data(), sizeof(LoginMessageLayout)));

    auto* m = reinterpret_cast<const LoginMessageLayout*>(this->m_Buffer.data());

    EXPECT_EQ(42u, m->GetLoginId());
}
