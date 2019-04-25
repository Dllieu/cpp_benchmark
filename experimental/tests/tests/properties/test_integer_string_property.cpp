#include <gtest/gtest.h>
#include <properties/integer_string_property.h>

#include <iostream>

TEST(IntegerStringPropertyTest, Zero) // NOLINT
{
    const char* s = "00000";
    auto p = reinterpret_cast<const experimental::IntegerStringProperty<5>*>(s);

    EXPECT_EQ(p->Get(), 0u);
}

TEST(IntegerStringPropertyTest, Default) // NOLINT
{
    const char* s = "12345";
    auto p = reinterpret_cast<const experimental::IntegerStringProperty<5>*>(s);

    EXPECT_EQ(p->Get(), 12345u);
}

TEST(IntegerStringPropertyTest, Truncate) // NOLINT
{
    const char* s = "12345";
    auto p = reinterpret_cast<const experimental::IntegerStringProperty<3>*>(s);

    EXPECT_EQ(p->Get(), 123u);
}
