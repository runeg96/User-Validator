#pragma once

#include <rapidjson/document.h>

#include "user.hpp"

class UserJsonMapper
{
public:
    /// @brief Maps a JSON object to a User object.
    /// @param i_jsonUser The JSON object representing the user.
    /// @param io_user The User object to populate with data from the JSON object.
    /// @return True if the mapping was successful, false otherwise.
    static bool fromJson(const rapidjson::Value& i_jsonUser, User& io_user);

    /// @brief Maps a User object to a JSON object.
    /// @param i_user The User object to map.
    /// @param i_allocator The allocator to use for the JSON object.
    /// @return The JSON object representing the user.
    static rapidjson::Value toJson(const User& i_user, rapidjson::Document::AllocatorType& i_allocator);
};