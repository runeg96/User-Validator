#include "exporter/exporter_factory.hpp"

#include <memory>
#include <string>

#include "exporter/json_exporter.hpp"
#include "exporter/xml_exporter.hpp"

std::unique_ptr<IExporter> ExporterFactory::create(ExportFormat i_format)
{
    switch (i_format)
    {
        case ExportFormat::Json:
            return std::make_unique<JsonExporter>();

        case ExportFormat::Xml:
            return std::make_unique<XmlExporter>();

        default:
            return nullptr;
    }
}

std::unique_ptr<IExporter> ExporterFactory::createFromPath(const std::string& i_exportPath)
{
    if (i_exportPath.size() >= 5 &&
        i_exportPath.substr(i_exportPath.size() - 5) == ".json")
    {
        return create(ExportFormat::Json);
    }

    if (i_exportPath.size() >= 4 &&
        i_exportPath.substr(i_exportPath.size() - 4) == ".xml")
    {
        return create(ExportFormat::Xml);
    }

    return nullptr;
}