#include <gtest/gtest.h>
#include <messages/message.h>
#include <cstddef>
#include <sstream>
#include <cstdint>

namespace
{
    DECLARE_LAYOUT_PACKED(ExampledPacketMessage, 20,
        (std::uint32_t, Id, ID)
        (std::uint64_t, Price)
        (std::uint64_t, Quantity));
}

TEST(MessageLayoutTest, Layout) // NOLINT
{
    std::array<std::byte, sizeof(ExampledPacketMessageLayout)> buffer{};
    auto* m = reinterpret_cast<ExampledPacketMessageLayout*>(buffer.data());

    EXPECT_EQ(0u, m->GetId());
    EXPECT_EQ(0u, m->GetPrice());
    EXPECT_EQ(0u, m->GetQuantity());

    m->SetId(42);

    std::stringstream ss;
    ss << *m;

    EXPECT_EQ("ExampledPacketMessage [Id=42 | Price=0 | Quantity=0]", ss.str());
}

namespace
{
    DECLARE_LAYOUT_CUSTOM_PACK(ExampleCustomPaddingMessage, 22, 2,
        (std::uint32_t, Id, ID)
        (std::uint64_t, Price)
        (std::uint64_t, Quantity)
        (std::int8_t, Flag));

    DECLARE_LAYOUT_DEFAULT_ALIGNMENT(ExampleDefaultAlignmentMessage, 24,
        (std::uint32_t, Id, ID)
        (std::uint64_t, Price)
        (std::uint64_t, Quantity));
}
