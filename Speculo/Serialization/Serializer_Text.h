#pragma once
#include <string>
#include "Serializer_Core.h"
#include "Serializer_Text_Utilities.h"
#pragma warning(push, 0)
#include "yaml-cpp/yaml.h"
#pragma warning(pop)

namespace Speculo
{
    class Serializer_Text : public Serializer_Core
    {
    public:
        Serializer_Text() = delete;
        explicit Serializer_Text(Serializer_Operation_Type operationType, const std::string& filePath, const std::string& fileType) noexcept;

        // Serialize
        template <typename T>
        void SerializeProperty(const std::string& propertyName, T value)
        {
            m_ActiveEmitter << YAML::Key << propertyName << YAML::Value << value;
        }

        // Deserialize
        template <typename T>
        void DeserializeProperty(const std::string& propertyName, T* value)
        {
            try
            {
                *value = m_ActiveNode[propertyName].as<T>();
            }
            catch (std::exception& thrownError)
            {
                SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_DESERIALIZATION_FAILURE, thrownError.what() + std::string(": ") + m_FilePath);
            }
        }

        void EndSerialization() noexcept;

    private:
        virtual bool ValidateMetadata(const std::string& fileType) override;

    private:
        Serializer_Operation_Type m_OperationType = Serializer_Operation_Type::Unknown;
        const std::string m_FilePath = "";

        // YAML
        YAML::Emitter m_ActiveEmitter; // Serialization
        YAML::Node m_ActiveNode;       // Deserialization
    };
}