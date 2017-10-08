#include <gtest/gtest.h>

namespace
{
    struct Fixture : public ::testing::Test
    {
        Fixture()
            : m_Stuff(1)
        {
        }

        int m_Stuff;
    };
}

TEST_F(Fixture, TestFixture)
{
    EXPECT_EQ(1, this->m_Stuff);
}
