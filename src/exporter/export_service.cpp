#include "exporter/export_service.hpp"

#include <iostream>
#include <memory>

#include "exporter/exporter_factory.hpp"
#include "exporter/i_exporter.hpp"

bool ExportService::exportUsers(const std::vector<User>& i_users, const std::string& i_exportPath) const
{
    std::unique_ptr<IExporter> exporter = ExporterFactory::createFromPath(i_exportPath);
    if (!exporter)
    {
        std::cerr << "Unsupported export format for path: " << i_exportPath << std::endl;
        return false;
    }

    return exporter->exportUsers(i_users, i_exportPath);
}