#pragma once
#include <string>
#include "Serializer_Core.h"
#pragma warning(push, 0)
#include "yaml-cpp/yaml.h"
#pragma warning(pop)

namespace Speculo
{
    enum class OperationType
    {
        Serialization,
        Deserialization,
        Unknown
    };

    class Serializer_Text : public Serializer_Core
    {
    public:
        Serializer_Text() = delete;
        explicit Serializer_Text(OperationType operationType, const std::string& filePath, const std::string& fileType) noexcept;

        // Serialize
        template <typename T, typename = typename std::enable_if<!std::is_same<T, std::string>::value>::type>
        void SerializeProperty(const std::string& propertyName, T value)
        {
            m_ActiveEmitter << YAML::Key << propertyName << YAML::Value << value;
        }

        // Deserialize
        template <typename T, typename = typename std::enable_if<!std::is_same<T, std::string>::value>::type>
        void DeserializeProperty(const std::string& propertyName, T* value)
        {
            *value = m_ActiveNode[propertyName].as<T>();
        }

        void EndSerialization() noexcept;

    private:
        bool ValidateFileType(const std::string& fileType);

    private:
        OperationType m_OperationType = OperationType::Unknown;
        const std::string m_FilePath = "";

        // YAML
        YAML::Emitter m_ActiveEmitter; // Serialization
        YAML::Node m_ActiveNode;       // Deserialization
    };
}