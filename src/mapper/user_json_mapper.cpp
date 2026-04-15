#include "mapper/user_json_mapper.hpp"

#include <string>
#include <utility>
#include <vector>

namespace {

std::vector<std::string> splitPlatforms(const std::string& i_platforms)
{
    std::vector<std::string> result;
    size_t start = 0;

    while (start < i_platforms.size())
    {
        size_t end = i_platforms.find(',', start);
        if (end == std::string::npos)
        {
            end = i_platforms.size();
        }

        std::string item = i_platforms.substr(start, end - start);

        const size_t left  = item.find_first_not_of(' ');
        const size_t right = item.find_last_not_of(' ');

        if (left != std::string::npos)
        {
            result.push_back(item.substr(left, right - left + 1));
        }

        start = end + 1;
    }

    return result;
}

std::string joinPlatforms(const std::vector<std::string>& i_platforms)
{
    std::string result;

    for (size_t i = 0; i < i_platforms.size(); ++i)
    {
        if (i > 0)
        {
            result += ", ";
        }

        result += i_platforms[i];
    }

    return result;
}

} // namespace

bool UserJsonMapper::fromJson(const rapidjson::Value& i_jsonUser, User& io_user)
{
    if (!i_jsonUser.IsObject())
    {
        return false;
    }

    io_user = User{};

    // Required fields
    io_user.name      = i_jsonUser["Navn"].GetString();
    io_user.age       = i_jsonUser["Alder"].GetInt();
    io_user.email     = i_jsonUser["Email"].GetString();
    io_user.platforms = splitPlatforms(i_jsonUser["Spilleplatform"].GetString());
    io_user.createdAt = i_jsonUser["Oprettelsesdato"].GetString();

    // Optional fields
    if (i_jsonUser.HasMember("Mobilnummer") && i_jsonUser["Mobilnummer"].IsString())
    {
        io_user.mobileNumber = i_jsonUser["Mobilnummer"].GetString();
    }

    if (i_jsonUser.HasMember("Telefonnummer") && i_jsonUser["Telefonnummer"].IsString())
    {
        io_user.phoneNumber = i_jsonUser["Telefonnummer"].GetString();
    }

    return true;
}

rapidjson::Value UserJsonMapper::toJson(const User& i_user, rapidjson::Document::AllocatorType& i_allocator)
{
    rapidjson::Value jsonUser(rapidjson::kObjectType);

    jsonUser.AddMember("Navn", rapidjson::Value(i_user.name.c_str(), i_allocator), i_allocator);
    jsonUser.AddMember("Alder", i_user.age, i_allocator);
    jsonUser.AddMember("Email", rapidjson::Value(i_user.email.c_str(), i_allocator), i_allocator);
    jsonUser.AddMember("Spilleplatform", rapidjson::Value(joinPlatforms(i_user.platforms).c_str(), i_allocator), i_allocator);
    jsonUser.AddMember("Oprettelsesdato", rapidjson::Value(i_user.createdAt.c_str(), i_allocator), i_allocator);

    if (i_user.mobileNumber.has_value())
    {
        jsonUser.AddMember("Mobilnummer", rapidjson::Value(i_user.mobileNumber->c_str(), i_allocator), i_allocator);
    }
    else
    {
        jsonUser.AddMember("Mobilnummer", rapidjson::Value().SetNull(), i_allocator);
    }

    if (i_user.phoneNumber.has_value())
    {
        jsonUser.AddMember("Telefonnummer", rapidjson::Value(i_user.phoneNumber->c_str(), i_allocator), i_allocator);
    }
    else
    {
        jsonUser.AddMember("Telefonnummer", rapidjson::Value().SetNull(), i_allocator);
    }

    return jsonUser;
}