#pragma once

#include <memory>
#include <string>

#include "exporter/i_exporter.hpp"

enum class ExportFormat
{
    Json,
    Xml
};

class ExporterFactory
{
public:
    /// @brief Creates an exporter based on the specified format.
    /// @param i_format The format of the exporter to create.
    /// @return A unique pointer to the created exporter, or nullptr if the format is not supported.
    static std::unique_ptr<IExporter> create(ExportFormat i_format);

    /// @brief Creates an exporter based on the file extension of the export path.
    /// @param i_exportPath The export file path.
    /// @return A unique pointer to the created exporter, or nullptr if the extension is not supported.
    static std::unique_ptr<IExporter> createFromPath(const std::string& i_exportPath);
};