#include "SpeculoPCH.h"
#include "Serializer_Text.h"
#include <cassert>
#include <fstream>

namespace Speculo
{
    Serializer_Text::Serializer_Text(OperationType operationType, const std::string& filePath, const std::string& fileType) noexcept 
                                   : m_FilePath(Speculo::FileSystem::ValidateAndAppendFileExtension(filePath, ".yml")), m_OperationType(operationType)
    {
        SPECULO_ASSERT(Speculo::FileSystem::ValidateFileDirectory(m_FilePath));

        if (operationType == OperationType::Serialization)
        {
            m_ActiveEmitter << YAML::BeginMap;

            m_ActiveEmitter << YAML::Key << "Type" << YAML::Value << fileType;
            m_ActiveEmitter << YAML::Key << "Version_Major" << YAML::Value << m_Version_Major;
            m_ActiveEmitter << YAML::Key << "Version_Minor" << YAML::Value << m_Version_Minor;
            m_ActiveEmitter << YAML::Key << "Version_Revision" << YAML::Value << m_Version_Revision;

            m_ActiveEmitter << YAML::Newline << YAML::Newline;
        }
        else if (operationType == OperationType::Deserialization)
        {
            m_ActiveNode = YAML::LoadFile(filePath);
            SPECULO_ASSERT(ValidateFileTypeAndVersion(fileType));
        }
    }

    void Serializer_Text::EndSerialization() noexcept
    {
        m_ActiveEmitter << YAML::EndMap;

        std::ofstream outputFile(m_FilePath);
        outputFile << m_ActiveEmitter.c_str();
    }

    bool Serializer_Text::ValidateFileTypeAndVersion(const std::string& fileType)
    {
        if (!m_ActiveNode.IsNull())
        {
            if (m_ActiveNode["Type"].as<std::string>() != fileType) 
            {
                SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_DESERIALIZATION_TYPE_MISMATCH, m_FilePath);
                return false;
            }

            if (m_ActiveNode["Version_Major"].as<int>() != m_Version_Major)
            {   
                SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_VERSION_MAJOR_MISMATCH, m_FilePath);
                return false;
            }

            if (m_ActiveNode["Version_Minor"].as<int>() != m_Version_Minor)
            {
                SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_VERSION_MINOR_MISMATCH, m_FilePath);
                return false;
            }

            if (m_ActiveNode["Version_Revision"].as<int>() != m_Version_Revision)
            {
                SPECULO_THROW_WARNING(SpeculoResult::SPECULO_WARNING_VERSION_REVISION_MISMATCH, m_FilePath);
            }

            return true;
        }

        SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_DESERIALIZATION_FAILURE, m_FilePath);
        return false;
    }
}