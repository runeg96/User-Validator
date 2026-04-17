#include <gtest/gtest.h>

#include "utils/utils.hpp"

TEST(SplitPlatformsTest, SplitsCommaSeparatedValues)
{
    const auto result = utils::splitPlatforms("PC, PS5, Xbox");
    ASSERT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0], "PC");
    EXPECT_EQ(result[1], "PS5");
    EXPECT_EQ(result[2], "Xbox");
}

TEST(SplitPlatformsTest, HandlesSingleValue)
{
    const auto result = utils::splitPlatforms("PC");
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], "PC");
}

TEST(SplitPlatformsTest, HandlesEmptyString)
{
    const auto result = utils::splitPlatforms("");
    EXPECT_TRUE(result.empty());
}

TEST(SplitPlatformsTest, TrimsWhitespace)
{
    const auto result = utils::splitPlatforms("  PC ,  PS5  ,Xbox  ");
    ASSERT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0], "PC");
    EXPECT_EQ(result[1], "PS5");
    EXPECT_EQ(result[2], "Xbox");
}

TEST(JoinPlatformsTest, JoinsWithCommaSpace)
{
    const std::vector<std::string> platforms = { "PC", "PS5", "Xbox" };
    EXPECT_EQ(utils::joinPlatforms(platforms), "PC, PS5, Xbox");
}

TEST(JoinPlatformsTest, HandlesSingleElement)
{
    EXPECT_EQ(utils::joinPlatforms({ "PC" }), "PC");
}

TEST(JoinPlatformsTest, HandlesEmpty)
{
    EXPECT_EQ(utils::joinPlatforms({}), "");
}

TEST(SplitJoinRoundTrip, RoundTripPreservesValues)
{
    const std::string original = "PC, PS5, Xbox";
    const auto split           = utils::splitPlatforms(original);
    const auto joined          = utils::joinPlatforms(split);
    EXPECT_EQ(joined, original);
}
