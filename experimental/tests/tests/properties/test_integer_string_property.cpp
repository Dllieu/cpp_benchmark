#include <gtest/gtest.h>
#include <properties/integer_string_property.h>

TEST(IntegerStringPropertyTest, Zero) // NOLINT
{
    const char* s = "00000";
    auto p = reinterpret_cast<const experimental::IntegerStringProperty<5>*>(s);

    EXPECT_EQ(0u, p->Get());
}

TEST(IntegerStringPropertyTest, Default) // NOLINT
{
    const char* s = "12345";
    auto p = reinterpret_cast<const experimental::IntegerStringProperty<5>*>(s);

    EXPECT_EQ(12345u, p->Get());
}

TEST(IntegerStringPropertyTest, Truncate) // NOLINT
{
    const char* s = "12345";
    auto p = reinterpret_cast<const experimental::IntegerStringProperty<3>*>(s);

    EXPECT_EQ(123u, p->Get());
}
