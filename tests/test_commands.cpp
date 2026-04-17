#include <gtest/gtest.h>

#include <cstdio>
#include <filesystem>
#include <fstream>

#include "app/command_dispatcher.hpp"
#include "app/command_names.hpp"
#include "logging/logger.hpp"

namespace {

/// @brief Creates a temporary directory with a valid users.json and returns an AppState pointing at it.
class CommandTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_tempDir = std::filesystem::temp_directory_path() / "command_test";
        std::filesystem::create_directories(m_tempDir);

        // Write a minimal valid users.json
        const std::string json = R"json([
            {
                "Navn": "Test User",
                "Alder": 25,
                "Email": "test@example.com",
                "Spilleplatform": "PC, PS5",
                "Oprettelsesdato": "2025-01-01",
                "Mobilnummer": "12345678",
                "Telefonnummer": null
            }
        ])json";

        std::ofstream file(m_tempDir / "users.json");
        file << json;
        file.close();

        Logger::initializeDefault(m_tempDir / "test.log", false);

        m_state.inputJsonPath  = m_tempDir / "users.json";
        m_state.jsonExportPath = m_tempDir / "export" / "users_export.json";
        m_state.xmlExportPath  = m_tempDir / "export" / "users_export.xml";
        m_state.logPath        = m_tempDir / "test.log";
    }

    void TearDown() override
    {
        std::filesystem::remove_all(m_tempDir);
    }

    std::filesystem::path m_tempDir;
    AppState m_state;
    AppCommandDispatcher m_dispatcher;
};

} // namespace

// ── parse_users ─────────────────────────────────────────────────────

TEST_F(CommandTest, ParseUsersSucceeds)
{
    const auto result = m_dispatcher.dispatch(app_commands::c_parseUsersCommandName, m_state);

    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.shouldPrintSummary);
    EXPECT_EQ(m_state.users.size(), 1u);
}

TEST_F(CommandTest, ParseUsersPopulatesUserFields)
{
    m_dispatcher.dispatch(app_commands::c_parseUsersCommandName, m_state);

    ASSERT_EQ(m_state.users.size(), 1u);
    EXPECT_EQ(m_state.users[0].name, "Test User");
    EXPECT_EQ(m_state.users[0].age, 25);
    EXPECT_EQ(m_state.users[0].email, "test@example.com");
}

TEST_F(CommandTest, ParseUsersDoesNotValidate)
{
    m_dispatcher.dispatch(app_commands::c_parseUsersCommandName, m_state);

    EXPECT_TRUE(m_state.validUsers.empty());
    EXPECT_TRUE(m_state.invalidUsers.empty());
}

// ── validate_users ──────────────────────────────────────────────────

TEST_F(CommandTest, ValidateUsersSucceeds)
{
    const auto result = m_dispatcher.dispatch(app_commands::c_validateUsersCommandName, m_state);

    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.shouldPrintSummary);
}

TEST_F(CommandTest, ValidateUsersProducesValidUsers)
{
    m_dispatcher.dispatch(app_commands::c_validateUsersCommandName, m_state);

    EXPECT_EQ(m_state.validUsers.size(), 1u);
    EXPECT_TRUE(m_state.invalidUsers.empty());
}

TEST_F(CommandTest, ValidateUsersCatchesInvalidUser)
{
    // Overwrite with a user that has only 1 platform (fails business rules)
    const std::string json = R"json([
        {
            "Navn": "Bad User",
            "Alder": 20,
            "Email": "bad@example.com",
            "Spilleplatform": "PC",
            "Oprettelsesdato": "2025-01-01",
            "Mobilnummer": "11111111",
            "Telefonnummer": null
        }
    ])json";

    std::ofstream file(m_state.inputJsonPath);
    file << json;
    file.close();

    m_dispatcher.dispatch(app_commands::c_validateUsersCommandName, m_state);

    EXPECT_TRUE(m_state.validUsers.empty());
    EXPECT_EQ(m_state.invalidUsers.size(), 1u);
}

TEST_F(CommandTest, ValidateUsersDoesNotExport)
{
    m_dispatcher.dispatch(app_commands::c_validateUsersCommandName, m_state);

    EXPECT_FALSE(std::filesystem::exists(m_state.jsonExportPath));
    EXPECT_FALSE(std::filesystem::exists(m_state.xmlExportPath));
}

// ── export_users ────────────────────────────────────────────────────

TEST_F(CommandTest, ExportUsersSucceeds)
{
    const auto result = m_dispatcher.dispatch(app_commands::c_exportUsersCommandName, m_state);

    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.shouldPrintSummary);
}

TEST_F(CommandTest, ExportUsersCreatesFiles)
{
    m_dispatcher.dispatch(app_commands::c_exportUsersCommandName, m_state);

    EXPECT_TRUE(std::filesystem::exists(m_state.jsonExportPath));
    EXPECT_TRUE(std::filesystem::exists(m_state.xmlExportPath));
}

// ── run_pipeline ────────────────────────────────────────────────────

TEST_F(CommandTest, RunPipelineSucceeds)
{
    const auto result = m_dispatcher.dispatch(app_commands::c_runPipelineCommandName, m_state);

    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.shouldPrintSummary);
}

TEST_F(CommandTest, RunPipelineExportsFiles)
{
    m_dispatcher.dispatch(app_commands::c_runPipelineCommandName, m_state);

    EXPECT_TRUE(std::filesystem::exists(m_state.jsonExportPath));
    EXPECT_TRUE(std::filesystem::exists(m_state.xmlExportPath));
    EXPECT_EQ(m_state.validUsers.size(), 1u);
}

// ── Unknown command ─────────────────────────────────────────────────

TEST_F(CommandTest, UnknownCommandFails)
{
    const auto result = m_dispatcher.dispatch("nonexistent_command", m_state);

    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.shouldPrintSummary);
}

// ── Invalid input file ──────────────────────────────────────────────

TEST_F(CommandTest, ParseFailsWithMissingFile)
{
    m_state.inputJsonPath = m_tempDir / "nonexistent.json";

    const auto result = m_dispatcher.dispatch(app_commands::c_parseUsersCommandName, m_state);

    EXPECT_FALSE(result.success);
}

// ── command_names ───────────────────────────────────────────────────

TEST(CommandNamesTest, RecognizesAllKnownCommands)
{
    EXPECT_TRUE(app_commands::isKnownCommandName("parse_users"));
    EXPECT_TRUE(app_commands::isKnownCommandName("validate_users"));
    EXPECT_TRUE(app_commands::isKnownCommandName("export_users"));
    EXPECT_TRUE(app_commands::isKnownCommandName("run_pipeline"));
}

TEST(CommandNamesTest, RejectsUnknownCommands)
{
    EXPECT_FALSE(app_commands::isKnownCommandName(""));
    EXPECT_FALSE(app_commands::isKnownCommandName("foo"));
    EXPECT_FALSE(app_commands::isKnownCommandName("Parse_users"));
}
