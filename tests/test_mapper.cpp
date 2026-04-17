#include <gtest/gtest.h>

#include <rapidjson/document.h>

#include "mapper/user_json_mapper.hpp"
#include "user.hpp"

namespace {

User makeTestUser()
{
    User user;
    user.name         = "Test User";
    user.age          = 25;
    user.email        = "test@example.com";
    user.platforms    = { "PC", "PS5" };
    user.createdAt    = "2025-01-01";
    user.mobileNumber = "12345678";
    user.phoneNumber  = std::nullopt;
    return user;
}

rapidjson::Document makeValidJsonUser()
{
    rapidjson::Document doc;
    doc.Parse(R"json({
        "Navn": "Test User",
        "Alder": 25,
        "Email": "test@example.com",
        "Spilleplatform": "PC, PS5",
        "Oprettelsesdato": "2025-01-01",
        "Mobilnummer": "12345678",
        "Telefonnummer": null
    })json");
    return doc;
}

} // namespace

// ── fromJson ────────────────────────────────────────────────────────

TEST(UserJsonMapperTest, FromJsonMapsRequiredFields)
{
    rapidjson::Document doc = makeValidJsonUser();
    User user;

    ASSERT_TRUE(UserJsonMapper::fromJson(doc, user));
    EXPECT_EQ(user.name, "Test User");
    EXPECT_EQ(user.age, 25);
    EXPECT_EQ(user.email, "test@example.com");
    EXPECT_EQ(user.createdAt, "2025-01-01");
}

TEST(UserJsonMapperTest, FromJsonSplitsPlatforms)
{
    rapidjson::Document doc = makeValidJsonUser();
    User user;

    ASSERT_TRUE(UserJsonMapper::fromJson(doc, user));
    ASSERT_EQ(user.platforms.size(), 2u);
    EXPECT_EQ(user.platforms[0], "PC");
    EXPECT_EQ(user.platforms[1], "PS5");
}

TEST(UserJsonMapperTest, FromJsonMapsOptionalMobile)
{
    rapidjson::Document doc = makeValidJsonUser();
    User user;

    ASSERT_TRUE(UserJsonMapper::fromJson(doc, user));
    ASSERT_TRUE(user.mobileNumber.has_value());
    EXPECT_EQ(user.mobileNumber.value(), "12345678");
}

TEST(UserJsonMapperTest, FromJsonLeavesNullPhoneAsNullopt)
{
    rapidjson::Document doc = makeValidJsonUser();
    User user;

    ASSERT_TRUE(UserJsonMapper::fromJson(doc, user));
    EXPECT_FALSE(user.phoneNumber.has_value());
}

TEST(UserJsonMapperTest, FromJsonRejectsNonObject)
{
    rapidjson::Document doc;
    doc.Parse("[]");
    User user;

    EXPECT_FALSE(UserJsonMapper::fromJson(doc, user));
}

// ── toJson ──────────────────────────────────────────────────────────

TEST(UserJsonMapperTest, ToJsonMapsRequiredFields)
{
    rapidjson::Document doc;
    auto& allocator = doc.GetAllocator();

    rapidjson::Value json = UserJsonMapper::toJson(makeTestUser(), allocator);

    EXPECT_STREQ(json["Navn"].GetString(), "Test User");
    EXPECT_EQ(json["Alder"].GetInt(), 25);
    EXPECT_STREQ(json["Email"].GetString(), "test@example.com");
    EXPECT_STREQ(json["Oprettelsesdato"].GetString(), "2025-01-01");
}

TEST(UserJsonMapperTest, ToJsonJoinsPlatforms)
{
    rapidjson::Document doc;
    auto& allocator = doc.GetAllocator();

    rapidjson::Value json = UserJsonMapper::toJson(makeTestUser(), allocator);

    EXPECT_STREQ(json["Spilleplatform"].GetString(), "PC, PS5");
}

TEST(UserJsonMapperTest, ToJsonIncludesMobileWhenPresent)
{
    rapidjson::Document doc;
    auto& allocator = doc.GetAllocator();

    rapidjson::Value json = UserJsonMapper::toJson(makeTestUser(), allocator);

    ASSERT_TRUE(json["Mobilnummer"].IsString());
    EXPECT_STREQ(json["Mobilnummer"].GetString(), "12345678");
}

TEST(UserJsonMapperTest, ToJsonSetsNullWhenPhoneAbsent)
{
    rapidjson::Document doc;
    auto& allocator = doc.GetAllocator();

    rapidjson::Value json = UserJsonMapper::toJson(makeTestUser(), allocator);

    EXPECT_TRUE(json["Telefonnummer"].IsNull());
}

TEST(UserJsonMapperTest, ToJsonSetsNullWhenMobileAbsent)
{
    User user         = makeTestUser();
    user.mobileNumber = std::nullopt;

    rapidjson::Document doc;
    auto& allocator = doc.GetAllocator();

    rapidjson::Value json = UserJsonMapper::toJson(user, allocator);

    EXPECT_TRUE(json["Mobilnummer"].IsNull());
}

// ── Round-trip ──────────────────────────────────────────────────────

TEST(UserJsonMapperTest, RoundTripPreservesData)
{
    const User original = makeTestUser();

    rapidjson::Document doc;
    auto& allocator = doc.GetAllocator();
    rapidjson::Value json = UserJsonMapper::toJson(original, allocator);

    User restored;
    ASSERT_TRUE(UserJsonMapper::fromJson(json, restored));

    EXPECT_EQ(restored.name, original.name);
    EXPECT_EQ(restored.age, original.age);
    EXPECT_EQ(restored.email, original.email);
    EXPECT_EQ(restored.platforms, original.platforms);
    EXPECT_EQ(restored.createdAt, original.createdAt);
    EXPECT_EQ(restored.mobileNumber, original.mobileNumber);
    EXPECT_EQ(restored.phoneNumber, original.phoneNumber);
}
