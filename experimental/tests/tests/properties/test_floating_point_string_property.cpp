#include <gtest/gtest.h>
#include <properties/floating_point_string_property.h>

TEST(FloatingPointStringPropertyTest, Zero) // NOLINT
{
    const char* s = "00000";
    auto p = reinterpret_cast<const experimental::FloatingPointStringProperty<5, 2>*>(s);

    EXPECT_NEAR(0u, p->Get(), 1e-6);
}

TEST(FloatingPointStringPropertyTest, Default) // NOLINT
{
    const char* s = "12345";
    auto p = reinterpret_cast<const experimental::FloatingPointStringProperty<5, 2>*>(s);

    EXPECT_NEAR(123.45, p->Get(), 1e-6);
}

TEST(FloatingPointStringPropertyTest, Truncate) // NOLINT
{
    const char* s = "12345";
    auto p = reinterpret_cast<const experimental::FloatingPointStringProperty<3, 2>*>(s);

    EXPECT_NEAR(1.23, p->Get(), 1e-6);
}
