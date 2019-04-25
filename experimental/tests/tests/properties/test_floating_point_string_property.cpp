#include <gtest/gtest.h>
#include <properties/floating_point_string_property.h>

TEST(FloatingPointStringPropertyTest, Zero)
{
    const char* s = "00000";
    auto p = reinterpret_cast<const experimental::FloatingPointStringProperty<5, 2>*>(s);

    EXPECT_NEAR(p->Get(), 0u, 1e-6);
}

TEST(FloatingPointStringPropertyTest, Default)
{
    const char* s = "12345";
    auto p = reinterpret_cast<const experimental::FloatingPointStringProperty<5, 2>*>(s);

    EXPECT_NEAR(p->Get(), 123.45, 1e-6);
}

TEST(FloatingPointStringPropertyTest, Truncate)
{
    const char* s = "12345";
    auto p = reinterpret_cast<const experimental::FloatingPointStringProperty<3, 2>*>(s);

    EXPECT_NEAR(p->Get(), 1.23, 1e-6);
}
