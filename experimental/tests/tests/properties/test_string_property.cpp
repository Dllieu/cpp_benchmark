#include <gtest/gtest.h>
#include <properties/string_property.h>

TEST(StringPropertyTest, None) // NOLINT
{
    const char* s = " 234 ";
    auto p = reinterpret_cast<const experimental::StringProperty<5, experimental::StringPropertyType::None>*>(s);

    EXPECT_EQ(" 234 ", p->Get());
}

TEST(StringPropertyTest, LeftJustified_Default) // NOLINT
{
    const char s[] = "12345";
    auto p = reinterpret_cast<const experimental::StringProperty<5, experimental::StringPropertyType::LeftJustified, ' '>*>(s);

    EXPECT_EQ("12345", p->Get());
}

TEST(StringPropertyTest, LeftJustified_PaddedLeft) // NOLINT
{
    const char s[] = " 123 ";
    auto p = reinterpret_cast<const experimental::StringProperty<5, experimental::StringPropertyType::LeftJustified, ' '>*>(s);

    EXPECT_EQ(" 123", p->Get());
}

TEST(StringPropertyTest, LeftJustified_Empty) // NOLINT
{
    const char s[] = "     ";
    auto p = reinterpret_cast<const experimental::StringProperty<5, experimental::StringPropertyType::LeftJustified, ' '>*>(s);

    EXPECT_TRUE(p->Get().empty());
}

TEST(StringPropertyTest, RightJustified_Default) // NOLINT
{
    const char s[] = "12345";
    auto p = reinterpret_cast<const experimental::StringProperty<5, experimental::StringPropertyType::RightJustified, ' '>*>(s);

    EXPECT_EQ("12345", p->Get());
}

TEST(StringPropertyTest, RightJustified_PaddedLeft) // NOLINT
{
    const char s[] = " 123 ";
    auto p = reinterpret_cast<const experimental::StringProperty<5, experimental::StringPropertyType::RightJustified, ' '>*>(s);

    EXPECT_EQ("123 ", p->Get());
}

TEST(StringPropertyTest, RightJustified_Empty) // NOLINT
{
    const char s[] = "     ";
    auto p = reinterpret_cast<const experimental::StringProperty<5, experimental::StringPropertyType::RightJustified, ' '>*>(s);

    EXPECT_TRUE(p->Get().empty());
}
