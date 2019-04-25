#include <gtest/gtest.h>
#include <properties/integral_string_property.h>

#include <iostream>

TEST(IntegralStringPropertyTest, Zero)
{
    const char* s = "00000";
    auto p = reinterpret_cast<const experimental::IntegralStringProperty<5>*>(s);

    EXPECT_EQ(p->Get(), 0u);
}

TEST(IntegralStringPropertyTest, Default)
{
    const char* s = "12345";
    auto p = reinterpret_cast<const experimental::IntegralStringProperty<5>*>(s);

    EXPECT_EQ(p->Get(), 12345u);
}

TEST(IntegralStringPropertyTest, Truncate)
{
    const char* s = "12345";
    auto p = reinterpret_cast<const experimental::IntegralStringProperty<3>*>(s);

    EXPECT_EQ(p->Get(), 123u);
}
