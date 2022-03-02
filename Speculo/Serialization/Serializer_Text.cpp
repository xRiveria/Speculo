#include "SpeculoPCH.h"
#include "Serializer_Text.h"
#include <cassert>
#include <fstream>

// Serialization files are split explictly into two sections of data: Metadata (Versioning) and Data (Contents).
namespace Speculo
{
    Serializer_Text::Serializer_Text(Serializer_Operation_Type operationType, const std::string& filePath, const std::string& fileType) noexcept
                                   : m_FilePath(Speculo::FileSystem::ValidateAndAppendFileExtension(filePath, ".yml")), m_OperationType(operationType)
    {
        if (operationType == Serializer_Operation_Type::Serialization)
        {
            if (Speculo::FileSystem::ValidateFileDirectory(m_FilePath) != true)
            {
                SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_FILESYSTEM_DIRECTORY_NOT_FOUND, m_FilePath);
                return;
            }

            m_ActiveEmitter << YAML::BeginMap;

            m_ActiveEmitter << YAML::Key << "Metadata";
            m_ActiveEmitter << YAML::Value << YAML::BeginMap;
            m_ActiveEmitter << YAML::Key << "Type" << YAML::Value << fileType;
            m_ActiveEmitter << YAML::Key << "Version_Major" << YAML::Value << m_Version_Major;
            m_ActiveEmitter << YAML::Key << "Version_Minor" << YAML::Value << m_Version_Minor;
            m_ActiveEmitter << YAML::Key << "Version_Revision" << YAML::Value << m_Version_Revision;
            m_ActiveEmitter << YAML::EndMap; // Metadata

            m_ActiveEmitter << YAML::Newline << YAML::Newline;

            m_ActiveEmitter << YAML::Key << "Data";
            m_ActiveEmitter << YAML::Value << YAML::BeginMap;
        }
        else if (operationType == Serializer_Operation_Type::Deserialization)
        {
            if (Speculo::FileSystem::ValidateFileExistence(m_FilePath) != true)
            {
                SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_FILESYSTEM_FILE_NOT_FOUND, m_FilePath);
                return;
            }

            // Explicit error handling as YAML functions don't throw useful asserts internally on errors.
            try
            {
                m_ActiveNode = YAML::LoadFile(m_FilePath);
            }
            catch (std::exception& thrownError)
            {
                SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_DESERIALIZATION_FAILURE, thrownError.what());
            }

            ValidateMetadata(fileType);
            m_ActiveNode = m_ActiveNode["Data"];
        }
    }

    bool Serializer_Text::ValidateMetadata(const std::string& fileType)
    {
        YAML::Node metaData = m_ActiveNode["Metadata"];

        if (!metaData.IsNull())
        {
            if (metaData["Type"].as<std::string>() != fileType)
            {
                SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_DESERIALIZATION_TYPE_MISMATCH, m_FilePath);
                return false;
            }

            if (metaData["Version_Major"].as<int>() != m_Version_Major)
            {
                SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_VERSION_MAJOR_MISMATCH, m_FilePath);
                return false;
            }

            if (metaData["Version_Minor"].as<int>() != m_Version_Minor)
            {
                SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_VERSION_MINOR_MISMATCH, m_FilePath);
                return false;
            }

            if (metaData["Version_Revision"].as<int>() != m_Version_Revision)
            {
                SPECULO_THROW_WARNING(SpeculoResult::SPECULO_WARNING_VERSION_REVISION_MISMATCH, m_FilePath);
            }

            return true;
        }

        SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_DESERIALIZATION_FAILURE, m_FilePath);
        return false;
    }

    void Serializer_Text::EndSerialization() noexcept
    {
        m_ActiveEmitter << YAML::EndMap; // Data Map
        m_ActiveEmitter << YAML::EndMap; // Object Map

        std::ofstream outputFile(m_FilePath);
        outputFile << m_ActiveEmitter.c_str();
    }
}