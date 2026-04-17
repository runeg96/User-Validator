#include <gtest/gtest.h>

#include "parser/user_parser.hpp"
#include "user.hpp"

// ── Single-user parsing ─────────────────────────────────────────────

TEST(UserParserTest, ParsesValidSingleUser)
{
    const std::string json = R"json({
        "Navn": "Anders Jensen",
        "Alder": 30,
        "Email": "anders@example.com",
        "Spilleplatform": "PC, PS5",
        "Oprettelsesdato": "2025-03-15",
        "Mobilnummer": "12345678"
    })json";

    UserParser parser;
    User user;
    ASSERT_TRUE(parser.parseUser(json, user));

    EXPECT_EQ(user.name, "Anders Jensen");
    EXPECT_EQ(user.age, 30);
    EXPECT_EQ(user.email, "anders@example.com");
    EXPECT_EQ(user.platforms.size(), 2u);
    EXPECT_EQ(user.createdAt, "2025-03-15");
    EXPECT_TRUE(user.mobileNumber.has_value());
    EXPECT_EQ(user.mobileNumber.value(), "12345678");
    EXPECT_FALSE(user.phoneNumber.has_value());
}

TEST(UserParserTest, RejectsUserMissingRequiredField)
{
    // Missing "Email"
    const std::string json = R"json({
        "Navn": "Anders Jensen",
        "Alder": 30,
        "Spilleplatform": "PC, PS5",
        "Oprettelsesdato": "2025-03-15"
    })json";

    UserParser parser;
    User user;
    EXPECT_FALSE(parser.parseUser(json, user));
}

TEST(UserParserTest, RejectsInvalidJson)
{
    const std::string json = "{ not valid json }";

    UserParser parser;
    User user;
    EXPECT_FALSE(parser.parseUser(json, user));
}

// ── Multi-user parsing ──────────────────────────────────────────────

TEST(UserParserTest, ParsesMultipleUsers)
{
    const std::string json = R"json([
        {
            "Navn": "User A",
            "Alder": 20,
            "Email": "a@example.com",
            "Spilleplatform": "PC",
            "Oprettelsesdato": "2025-01-01",
            "Mobilnummer": "11111111"
        },
        {
            "Navn": "User B",
            "Alder": 25,
            "Email": "b@example.com",
            "Spilleplatform": "PS5, Xbox",
            "Oprettelsesdato": "2025-02-01",
            "Telefonnummer": "22222222"
        }
    ])json";

    UserParser parser;
    std::vector<User> users;
    std::vector<RejectedRecord> rejected;

    ASSERT_TRUE(parser.parseUsers(json, users, rejected));
    EXPECT_EQ(users.size(), 2u);
    EXPECT_TRUE(rejected.empty());
    EXPECT_EQ(users[0].name, "User A");
    EXPECT_EQ(users[1].name, "User B");
}

TEST(UserParserTest, RejectsInvalidUsersInArray)
{
    const std::string json = R"json([
        {
            "Navn": "Good User",
            "Alder": 20,
            "Email": "good@example.com",
            "Spilleplatform": "PC",
            "Oprettelsesdato": "2025-01-01",
            "Mobilnummer": "11111111"
        },
        {
            "Navn": "Bad User"
        }
    ])json";

    UserParser parser;
    std::vector<User> users;
    std::vector<RejectedRecord> rejected;

    ASSERT_TRUE(parser.parseUsers(json, users, rejected));
    EXPECT_EQ(users.size(), 1u);
    EXPECT_EQ(rejected.size(), 1u);
    EXPECT_EQ(rejected[0].name, "Bad User");
}
