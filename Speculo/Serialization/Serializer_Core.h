#pragma once
#include "Core/Settings.h"

namespace Serializer
{
    class Serializer_Core
    {
    protected:
        Serializer_Core() : m_Version_Major(SPECULO_VERSION_MAJOR), m_Version_Minor(SPECULO_VERSION_MINOR), m_Version_Revision(SPECULO_VERSION_REVISION) { }

    private:
        const uint8_t m_Version_Major = 0;
        const uint8_t m_Version_Minor = 0;
        const uint8_t m_Version_Revision = 0;
    };
}