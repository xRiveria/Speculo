#pragma once
#include <string>
#include "Serializer_Core.h"
#include "Serializer_Text_Utilities.h"

namespace Speculo
{
    // This is a data container for serialization/deserialization purposes. It can only be used for either one at any point in time, and not both together at the same time.
    class Serializer_Text : public Serializer_Core
    {
    public:
        Serializer_Text() = delete;
        ~Serializer_Text();
        explicit Serializer_Text(Serializer_Operation_Type operationType, const std::string& filePath, const std::string& fileType) noexcept;

        // Serialize
        template <typename T>
        void SerializeProperty(const std::string& propertyName, T value)
        {
            if (m_IsStreamOpen)
            {
                m_ActiveEmitter << YAML::Key << propertyName << YAML::Value << value;
            }
            else
            {
                SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_FILESTREAM_UNOPEN, m_FilePath);
                return;
            }
        }

        // Deserialize
        template <typename T>
        void DeserializeProperty(const std::string& propertyName, T* value)
        {
            if (m_IsStreamOpen)
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
            else
            {
                SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_FILESTREAM_UNOPEN, m_FilePath);
                return;
            }
        }

        template <typename T>
        T DeserializePropertyAs(const std::string& propertyName)
        {
            T value;
            DeserializeProperty(propertyName, &value);
            return value;
        }

        virtual void EndSerialization() override;
        virtual void EndDeserialization() override;

    private:
        virtual void BeginSerialization() override;
        virtual void BeginDeserialization() override;
        virtual bool ValidateMetadata() override;

    private:
        // YAML
        YAML::Emitter m_ActiveEmitter; // Serialization
        YAML::Node m_ActiveNode;       // Deserialization

        bool m_IsStreamOpen = false;
    };
}