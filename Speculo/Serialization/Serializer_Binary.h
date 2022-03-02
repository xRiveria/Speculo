#pragma once
#include "Serializer_Core.h"
#include <fstream>

namespace Speculo
{
    // Data flow is always FIFO for binary emissions.

    class Serializer_Binary : public Serializer_Core
    {
    public:
        Serializer_Binary() = delete;
        ~Serializer_Binary();
        Serializer_Binary(Serializer_Operation_Type operationType, const std::string& filePath, const std::string& fileType) noexcept;

        template <typename T, typename = typename std::enable_if<!std::is_same<T, std::string>::value>::type>
        void SerializeProperty(T value)
        {
            if (m_IsStreamOpen)
            {
                m_OutputStream.write(reinterpret_cast<char*>(&value), sizeof(value));
            }
            else
            {
                SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_FILESTREAM_UNOPEN, m_FilePath);
                return;
            }
        }

        void SerializeProperty(const std::string& value);

        template <typename T, typename = typename std::enable_if<!std::is_same<T, std::string>::value>::type>
        void DeserializeProperty(T* value)
        {
            if (m_IsStreamOpen)
            {
                m_InputStream.read(reinterpret_cast<char*>(value), sizeof(T));
            }
            else
            {
                SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_FILESTREAM_UNOPEN, m_FilePath);
                return;
            }
        }

        void DeserializeProperty(std::string* value);

        template <typename T>
        T DeserializePropertyAs()
        {
            T value;
            DeserializeProperty(&value);
            return value;
        }

        virtual void EndSerialization() override;
        virtual void EndDeserialization() override;

    private:
        virtual void BeginSerialization() override;
        virtual void BeginDeserialization() override;
        virtual bool ValidateMetadata() override;

    private:
        std::ofstream m_OutputStream;
        std::ifstream m_InputStream;
        bool m_IsStreamOpen = false;
    };
}