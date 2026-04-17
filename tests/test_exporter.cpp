#include <gtest/gtest.h>
#include <rapidjson/document.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include "exporter/exporter_factory.hpp"
#include "exporter/json_exporter.hpp"
#include "exporter/xml_exporter.hpp"
#include "pugixml.hpp"
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

std::string readFileContents(const std::string& path)
{
    std::ifstream file(path);
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

class ExporterTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_jsonPath = std::filesystem::temp_directory_path() / "test_export.json";
        m_xmlPath  = std::filesystem::temp_directory_path() / "test_export.xml";
    }

    void TearDown() override
    {
        std::filesystem::remove(m_jsonPath);
        std::filesystem::remove(m_xmlPath);
    }

    std::filesystem::path m_jsonPath;
    std::filesystem::path m_xmlPath;
};

} // namespace

// ── JsonExporter ────────────────────────────────────────────────────

TEST_F(ExporterTest, JsonExportsValidFile)
{
    JsonExporter exporter;
    std::vector<User> users = { makeTestUser() };

    ASSERT_TRUE(exporter.exportUsers(users, m_jsonPath.string()));
    EXPECT_TRUE(std::filesystem::exists(m_jsonPath));
}

TEST_F(ExporterTest, JsonContainsCorrectUserData)
{
    JsonExporter exporter;
    std::vector<User> users = { makeTestUser() };

    ASSERT_TRUE(exporter.exportUsers(users, m_jsonPath.string()));

    const std::string contents = readFileContents(m_jsonPath.string());
    rapidjson::Document doc;
    doc.Parse(contents.c_str());

    ASSERT_FALSE(doc.HasParseError());
    ASSERT_TRUE(doc.IsArray());
    ASSERT_EQ(doc.Size(), 1u);

    const auto& userJson = doc[0];
    EXPECT_STREQ(userJson["Navn"].GetString(), "Test User");
    EXPECT_EQ(userJson["Alder"].GetInt(), 25);
    EXPECT_STREQ(userJson["Email"].GetString(), "test@example.com");
    EXPECT_STREQ(userJson["Oprettelsesdato"].GetString(), "2025-01-01");
}

TEST_F(ExporterTest, JsonExportsMultipleUsers)
{
    JsonExporter exporter;
    User user2              = makeTestUser();
    user2.name              = "Second User";
    std::vector<User> users = { makeTestUser(), user2 };

    ASSERT_TRUE(exporter.exportUsers(users, m_jsonPath.string()));

    const std::string contents = readFileContents(m_jsonPath.string());
    rapidjson::Document doc;
    doc.Parse(contents.c_str());

    ASSERT_TRUE(doc.IsArray());
    EXPECT_EQ(doc.Size(), 2u);
}

TEST_F(ExporterTest, JsonExportsEmptyArray)
{
    JsonExporter exporter;
    std::vector<User> users;

    ASSERT_TRUE(exporter.exportUsers(users, m_jsonPath.string()));

    const std::string contents = readFileContents(m_jsonPath.string());
    rapidjson::Document doc;
    doc.Parse(contents.c_str());

    ASSERT_TRUE(doc.IsArray());
    EXPECT_EQ(doc.Size(), 0u);
}

TEST_F(ExporterTest, JsonFailsOnInvalidPath)
{
    JsonExporter exporter;
    std::vector<User> users = { makeTestUser() };

    EXPECT_FALSE(exporter.exportUsers(users, "/nonexistent/dir/file.json"));
}

// ── XmlExporter ─────────────────────────────────────────────────────

TEST_F(ExporterTest, XmlExportsValidFile)
{
    XmlExporter exporter;
    std::vector<User> users = { makeTestUser() };

    ASSERT_TRUE(exporter.exportUsers(users, m_xmlPath.string()));
    EXPECT_TRUE(std::filesystem::exists(m_xmlPath));
}

TEST_F(ExporterTest, XmlContainsCorrectUserData)
{
    XmlExporter exporter;
    std::vector<User> users = { makeTestUser() };

    ASSERT_TRUE(exporter.exportUsers(users, m_xmlPath.string()));

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_file(m_xmlPath.c_str()));

    pugi::xml_node usersNode = doc.child("users");
    ASSERT_TRUE(usersNode);

    pugi::xml_node userNode = usersNode.child("user");
    ASSERT_TRUE(userNode);

    EXPECT_STREQ(userNode.child("Navn").text().get(), "Test User");
    EXPECT_EQ(userNode.child("Alder").text().as_int(), 25);
    EXPECT_STREQ(userNode.child("Email").text().get(), "test@example.com");
    EXPECT_STREQ(userNode.child("Oprettelsesdato").text().get(), "2025-01-01");
    EXPECT_STREQ(userNode.child("Spilleplatform").text().get(), "PC, PS5");
    EXPECT_STREQ(userNode.child("Mobilnummer").text().get(), "12345678");
}

TEST_F(ExporterTest, XmlHandlesNullPhoneFields)
{
    XmlExporter exporter;
    User user               = makeTestUser();
    user.mobileNumber       = std::nullopt;
    user.phoneNumber        = std::nullopt;
    std::vector<User> users = { user };

    ASSERT_TRUE(exporter.exportUsers(users, m_xmlPath.string()));

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_file(m_xmlPath.c_str()));

    pugi::xml_node userNode = doc.child("users").child("user");
    EXPECT_STREQ(userNode.child("Mobilnummer").text().get(), "");
    EXPECT_STREQ(userNode.child("Telefonnummer").text().get(), "");
}

TEST_F(ExporterTest, XmlExportsMultipleUsers)
{
    XmlExporter exporter;
    User user2              = makeTestUser();
    user2.name              = "Second User";
    std::vector<User> users = { makeTestUser(), user2 };

    ASSERT_TRUE(exporter.exportUsers(users, m_xmlPath.string()));

    pugi::xml_document doc;
    ASSERT_TRUE(doc.load_file(m_xmlPath.c_str()));

    int count = 0;
    for (pugi::xml_node user = doc.child("users").child("user"); user; user = user.next_sibling("user"))
    {
        ++count;
    }
    EXPECT_EQ(count, 2);
}

// ── ExporterFactory ─────────────────────────────────────────────────

TEST(ExporterFactoryTest, CreatesJsonExporterFromPath)
{
    auto exporter = ExporterFactory::createFromPath("output/users.json");
    EXPECT_NE(exporter, nullptr);
}

TEST(ExporterFactoryTest, CreatesXmlExporterFromPath)
{
    auto exporter = ExporterFactory::createFromPath("output/users.xml");
    EXPECT_NE(exporter, nullptr);
}

TEST(ExporterFactoryTest, ReturnsNullForUnsupportedExtension)
{
    auto exporter = ExporterFactory::createFromPath("output/users.csv");
    EXPECT_EQ(exporter, nullptr);
}

TEST(ExporterFactoryTest, CreatesFromEnum)
{
    EXPECT_NE(ExporterFactory::create(ExportFormat::Json), nullptr);
    EXPECT_NE(ExporterFactory::create(ExportFormat::Xml), nullptr);
}