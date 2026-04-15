#include "exporter/json_exporter.hpp"

#include <rapidjson/document.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/prettywriter.h>

#include <cstdio>

#include "mapper/user_json_mapper.hpp"

bool JsonExporter::exportUsers(const std::vector<User>& i_users, const std::string& i_outputPath) const
{
    rapidjson::Document doc;
    doc.SetArray();

    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

    for (const User& user : i_users)
    {
        doc.PushBack(UserJsonMapper::toJson(user, allocator), allocator);
    }

    FILE* file = std::fopen(i_outputPath.c_str(), "w");
    if (file == nullptr)
    {
        return false;
    }

    char writeBuffer[65536];
    rapidjson::FileWriteStream stream(file, writeBuffer, sizeof(writeBuffer));
    rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(stream);
    doc.Accept(writer);

    std::fclose(file);
    return true;
}