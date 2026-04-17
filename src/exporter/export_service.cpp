#include "exporter/export_service.hpp"

#include <filesystem>
#include <iostream>
#include <memory>

#include "exporter/exporter_factory.hpp"
#include "exporter/i_exporter.hpp"

bool ExportService::exportUsers(const std::vector<User>& i_users, const std::string& i_exportPath) const
{
    const std::filesystem::path exportPath(i_exportPath);
    const std::filesystem::path parentPath = exportPath.parent_path();

    if (!parentPath.empty())
    {
        std::error_code errorCode;
        std::filesystem::create_directories(parentPath, errorCode);

        if (errorCode)
        {
            std::cerr << "Failed to create export directory: " << parentPath << std::endl;
            return false;
        }
    }

    std::unique_ptr<IExporter> exporter = ExporterFactory::createFromPath(i_exportPath);
    if (!exporter)
    {
        std::cerr << "Unsupported export format for path: " << i_exportPath << std::endl;
        return false;
    }

    return exporter->exportUsers(i_users, i_exportPath);
}