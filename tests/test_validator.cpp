#include <gtest/gtest.h>

#include "user.hpp"
#include "validator/user_rules.hpp"
#include "validator/user_validator.hpp"

namespace {

User makeValidUser()
{
    User user;
    user.name         = "Test User";
    user.age          = 25;
    user.email        = "test@example.com";
    user.platforms    = { "PC", "PS5" };
    user.createdAt    = "2025-01-01";
    user.mobileNumber = "12345678";
    return user;
}

} // namespace

// ── EmailValidator ──────────────────────────────────────────────────

TEST(EmailValidatorTest, AcceptsValidEmail)
{
    EmailValidator validator;
    std::vector<ErrorDetail> errors;
    User user = makeValidUser();

    validator.validate(user, errors);
    EXPECT_TRUE(errors.empty());
}

TEST(EmailValidatorTest, RejectsEmailWithoutAt)
{
    EmailValidator validator;
    std::vector<ErrorDetail> errors;
    User user  = makeValidUser();
    user.email = "invalid-email";

    validator.validate(user, errors);
    EXPECT_FALSE(errors.empty());
}

TEST(EmailValidatorTest, RejectsEmptyEmail)
{
    EmailValidator validator;
    std::vector<ErrorDetail> errors;
    User user  = makeValidUser();
    user.email = "";

    validator.validate(user, errors);
    EXPECT_FALSE(errors.empty());
}

// ── ContactValidator ────────────────────────────────────────────────

TEST(ContactValidatorTest, AcceptsUserWithMobile)
{
    ContactValidator validator;
    std::vector<ErrorDetail> errors;
    User user         = makeValidUser();
    user.mobileNumber = "12345678";
    user.phoneNumber  = std::nullopt;

    validator.validate(user, errors);
    EXPECT_TRUE(errors.empty());
}

TEST(ContactValidatorTest, AcceptsUserWithPhone)
{
    ContactValidator validator;
    std::vector<ErrorDetail> errors;
    User user         = makeValidUser();
    user.mobileNumber = std::nullopt;
    user.phoneNumber  = "87654321";

    validator.validate(user, errors);
    EXPECT_TRUE(errors.empty());
}

TEST(ContactValidatorTest, RejectsUserWithNoContact)
{
    ContactValidator validator;
    std::vector<ErrorDetail> errors;
    User user         = makeValidUser();
    user.mobileNumber = std::nullopt;
    user.phoneNumber  = std::nullopt;

    validator.validate(user, errors);
    EXPECT_FALSE(errors.empty());
}

// ── PlatformsValidator ──────────────────────────────────────────────

TEST(PlatformsValidatorTest, AcceptsTwoOrMorePlatforms)
{
    PlatformsValidator validator;
    std::vector<ErrorDetail> errors;
    User user = makeValidUser();

    validator.validate(user, errors);
    EXPECT_TRUE(errors.empty());
}

TEST(PlatformsValidatorTest, RejectsSinglePlatform)
{
    PlatformsValidator validator;
    std::vector<ErrorDetail> errors;
    User user      = makeValidUser();
    user.platforms = { "PC" };

    validator.validate(user, errors);
    EXPECT_FALSE(errors.empty());
}

TEST(PlatformsValidatorTest, RejectsNoPlatforms)
{
    PlatformsValidator validator;
    std::vector<ErrorDetail> errors;
    User user      = makeValidUser();
    user.platforms = {};

    validator.validate(user, errors);
    EXPECT_FALSE(errors.empty());
}

// ── UserValidator (composite) ───────────────────────────────────────

TEST(UserValidatorTest, ValidUserHasNoErrors)
{
    UserValidator validator;
    const auto errors = validator.validate(makeValidUser());
    EXPECT_TRUE(errors.empty());
}

TEST(UserValidatorTest, InvalidUserCollectsMultipleErrors)
{
    User user;
    user.name      = "Bad User";
    user.age       = 30;
    user.email     = "not-an-email";
    user.platforms = { "PC" }; // too few
    user.createdAt = "2025-01-01";
    // no contact info

    UserValidator validator;
    const auto errors = validator.validate(user);
    EXPECT_GE(errors.size(), 2u);
}
