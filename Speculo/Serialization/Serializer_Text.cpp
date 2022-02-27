#include "Serializer_Text.h"
#include "IO/FileSystem.h"
#include <cassert>
#include <fstream>

namespace Speculo
{
    Serializer_Text::Serializer_Text(OperationType operationType, const std::string& filePath, const std::string& fileType) noexcept 
                                   : m_FilePath(filePath), m_OperationType(operationType)
    {
        /// Custom Engine Assert
        assert(Speculo::FileSystem::ValidateFileDirectory(filePath));

        if (operationType == OperationType::Serialization)
        {
            m_ActiveEmitter << YAML::BeginMap;

            m_ActiveEmitter << YAML::Key << "Version_Major" << YAML::Value << m_Version_Major;
            m_ActiveEmitter << YAML::Key << "Version_Minor" << YAML::Value << m_Version_Minor;
            m_ActiveEmitter << YAML::Key << "Version_Revision" << YAML::Value << m_Version_Revision;
            m_ActiveEmitter << YAML::Key << "Type" << YAML::Value << fileType;

            m_ActiveEmitter << YAML::EndMap;

            m_ActiveEmitter << YAML::BeginMap;
        }
        else if (operationType == OperationType::Deserialization)
        {
            m_ActiveNode = YAML::LoadFile(filePath);
            assert(ValidateFileType(fileType));
        }
    }

    void Serializer_Text::EndSerialization() noexcept
    {
        m_ActiveEmitter << YAML::EndMap;

        std::ofstream outputFile(m_FilePath);
        outputFile << m_ActiveEmitter.c_str();
    }

    bool Serializer_Text::ValidateFileType(const std::string& fileType)
    {
        if (m_ActiveNode["Type"].as<std::string>() != fileType)
        {
            return false;
        }

        return true;
    }
}