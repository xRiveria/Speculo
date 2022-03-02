#pragma once
#include "Core/Settings.h"

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
        Serializer_Core() : m_Version_Major(SPECULO_VERSION_MAJOR), m_Version_Minor(SPECULO_VERSION_MINOR), m_Version_Revision(SPECULO_VERSION_REVISION) { }
        virtual bool ValidateMetadata(const std::string& fileType) = 0;

    protected:
        const int m_Version_Major = 0;
        const int m_Version_Minor = 0;
        const int m_Version_Revision = 0;
    };
}