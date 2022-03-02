#include "SpeculoPCH.h"
#include "Serializer_Text.h"
#include <cassert>
#include <fstream>

// Serialization files are split explictly into two sections of data: Metadata (Versioning) and Data (Contents).
namespace Speculo
{
    Serializer_Text::Serializer_Text(Serializer_Operation_Type operationType, const std::string& filePath, const std::string& fileType) noexcept
                                   : Serializer_Core(operationType, Speculo::FileSystem::ValidateAndAppendFileExtension(filePath, ".yml"), fileType)
    {
        if (operationType == Serializer_Operation_Type::Serialization)
        {
            if (!Speculo::FileSystem::ValidateFileDirectory(m_FilePath))
            {
                SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_FILESYSTEM_DIRECTORY_NOT_FOUND, m_FilePath);
                return;
            }

            BeginSerialization();
        }
        else if (operationType == Serializer_Operation_Type::Deserialization)
        {
            if (!Speculo::FileSystem::ValidateFileExistence(m_FilePath))
            {
                SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_FILESYSTEM_FILE_NOT_FOUND, m_FilePath);
                return;
            }

            BeginDeserialization();
        }
    }

    Serializer_Text::~Serializer_Text()
    {
        if (m_IsStreamOpen)
        {
            SPECULO_THROW_WARNING(SpeculoResult::SPECULO_WARNING_BEST_PRACTICES, std::string("Consider explicitly ending serialization/deserialization with their respective functions to avoid possible issues. Errors may occur otherwise with destructors: ") + m_FilePath);
            if (m_OperationType == Serializer_Operation_Type::Serialization)
            {
                EndSerialization();
            }
            else if (m_OperationType == Serializer_Operation_Type::Deserialization)
            {
                EndDeserialization();
            }
        }
    }

    void Serializer_Text::BeginSerialization()
    {
        m_IsStreamOpen = true;

        m_ActiveEmitter << YAML::BeginMap;

        m_ActiveEmitter << YAML::Key << "Metadata";
        m_ActiveEmitter << YAML::Value << YAML::BeginMap;
        m_ActiveEmitter << YAML::Key << "Type" << YAML::Value << m_FileType;
        m_ActiveEmitter << YAML::Key << "Version_Major" << YAML::Value << m_Version_Major;
        m_ActiveEmitter << YAML::Key << "Version_Minor" << YAML::Value << m_Version_Minor;
        m_ActiveEmitter << YAML::Key << "Version_Revision" << YAML::Value << m_Version_Revision;
        m_ActiveEmitter << YAML::EndMap; // Metadata

        m_ActiveEmitter << YAML::Newline << YAML::Newline;

        m_ActiveEmitter << YAML::Key << "Data";
        m_ActiveEmitter << YAML::Value << YAML::BeginMap;
    }

    void Serializer_Text::EndSerialization()
    {
        if (m_IsStreamOpen)
        {
            m_ActiveEmitter << YAML::EndMap; // Metadata Map
            m_ActiveEmitter << YAML::EndMap; // Data Map

            // Output file.
            std::ofstream outputFile(m_FilePath);
            outputFile << m_ActiveEmitter.c_str();
            
            m_IsStreamOpen = false;
        }
        else
        {
            SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_FILESTREAM_UNOPEN, m_FilePath);
            return;
        }
    }

    void Serializer_Text::EndDeserialization()
    {
        if (m_IsStreamOpen)
        {
            m_IsStreamOpen = false;
        }
        else
        {
            SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_FILESTREAM_UNOPEN, m_FilePath);
            return;
        }
    }

    void Serializer_Text::BeginDeserialization()
    {
        // Explicit error handling as YAML functions don't throw useful asserts internally on errors.
        try
        {
            m_ActiveNode = YAML::LoadFile(m_FilePath);
        }
        catch (std::exception& thrownError)
        {
            SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_DESERIALIZATION_FAILURE, thrownError.what());
        }

        m_IsStreamOpen = true;

        ValidateMetadata();
        m_ActiveNode = m_ActiveNode["Data"];
    }

    bool Serializer_Text::ValidateMetadata()
    {
        YAML::Node metaData = m_ActiveNode["Metadata"];

        if (!metaData.IsNull())
        {
            if (metaData["Type"].as<std::string>() != m_FileType)
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
}