#pragma once
#include "Core/Settings.h"
#include <string>

namespace Speculo
{
    enum class Serializer_Operation_Type
    {
        Serialization,
        Deserialization,
        Unknown
    };

    class Serializer_Core
    {
    protected:
        Serializer_Core(Serializer_Operation_Type operationType, const std::string& filePath, const std::string& fileType) : 
                        m_FilePath(filePath), m_OperationType(operationType), m_FileType(fileType),
                        m_Version_Major(SPECULO_VERSION_MAJOR), m_Version_Minor(SPECULO_VERSION_MINOR), m_Version_Revision(SPECULO_VERSION_REVISION) { }
        
        virtual void BeginSerialization() = 0;
        virtual void EndSerialization() = 0;

        virtual void BeginDeserialization() = 0;
        virtual void EndDeserialization() = 0;
        virtual bool ValidateMetadata() = 0;

    protected:
        Serializer_Operation_Type m_OperationType = Serializer_Operation_Type::Unknown;
        const std::string m_FilePath = "";
        const std::string m_FileType = "";

        const int m_Version_Major = 0;
        const int m_Version_Minor = 0;
        const int m_Version_Revision = 0;
    };
}