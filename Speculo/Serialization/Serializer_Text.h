#pragma once
#include <string>
#include <cassert>
#include "Serializer_Core.h"
#include "IO/FileSystem.h"

namespace Serializer
{
    enum class SerializerTextFormat
    {
        YAML,
        JSON,
        Unspecified
    };

    class Serializer_Text : public Serializer_Core
    {
    public:
        Serializer_Text() = delete;
        explicit Serializer_Text(SerializerTextFormat formatType, const std::string& filePath) noexcept : m_FormatType(formatType), m_FilePath(filePath)
        {
            /// Custom Engine Assert
            assert(Speculo::FileSystem::ValidateFileDirectory(filePath));
        }

        template <typename T, typename = typename std::enable_if<!std::is_same<T, std::string>::value>::type>
        void SerializeProperty(const std::string& propertyName, T value)
        {
            
        }

    private:
        SerializerTextFormat m_FormatType = SerializerTextFormat::Unspecified;
        const std::string m_FilePath = "";
    };
}