#include "exporter/xml_exporter.hpp"

#include "pugixml.hpp"

namespace {

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

bool XmlExporter::exportUsers(const std::vector<User>& i_users, const std::string& i_exportPath) const
{
    pugi::xml_document doc;

    // XML declaration
    pugi::xml_node declaration               = doc.append_child(pugi::node_declaration);
    declaration.append_attribute("version")  = "1.0";
    declaration.append_attribute("encoding") = "UTF-8";

    // Root element
    pugi::xml_node usersNode = doc.append_child("users");

    for (const auto& user : i_users)
    {
        pugi::xml_node userNode = usersNode.append_child("user");

        userNode.append_child("Navn").text().set(user.name.c_str());
        userNode.append_child("Alder").text().set(user.age);
        userNode.append_child("Email").text().set(user.email.c_str());
        userNode.append_child("Spilleplatform").text().set(joinPlatforms(user.platforms).c_str());
        userNode.append_child("Oprettelsesdato").text().set(user.createdAt.c_str());

        pugi::xml_node mobileNode = userNode.append_child("Mobilnummer");
        if (user.mobileNumber.has_value())
        {
            mobileNode.text().set(user.mobileNumber->c_str());
        }

        pugi::xml_node phoneNode = userNode.append_child("Telefonnummer");
        if (user.phoneNumber.has_value())
        {
            phoneNode.text().set(user.phoneNumber->c_str());
        }
    }

    // Save with indentation for readability
    return doc.save_file(i_exportPath.c_str(), PUGIXML_TEXT("  "));
}