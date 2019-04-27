#include <gtest/gtest.h>
#include <properties/binary_coded_floating_point_property.h>

TEST(BinaryCodedFloatingPointPropertyTest, Packed_Default) // NOLINT
{
    std::array<std::uint8_t, 5> a{0b00000001, 0b00100011, 0b01000101, 0b01100111, 0b10001001};
    auto p = reinterpret_cast<const experimental::BinaryCodedFloatingPointProperty<a.size(), 5>*>(a.data());

    EXPECT_NEAR(1234.56789, p->Get(), 1e-6);
}

TEST(BinaryCodedFloatingPointPropertyTest, Unpacked_Default) // NOLINT
{
    std::array<std::uint8_t, 2> a{0b00000001, 0b00000011};
    auto p = reinterpret_cast<const experimental::BinaryCodedFloatingPointProperty<a.size(), 1, experimental::BinaryCodedDecimalType::Unpacked>*>(a.data());

    EXPECT_NEAR(1.3, p->Get(), 1e-6);
}
