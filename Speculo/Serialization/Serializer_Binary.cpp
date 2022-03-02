#include "SpeculoPCH.h"
#include "Serializer_Binary.h"

namespace Speculo
{
    Serializer_Binary::Serializer_Binary(Serializer_Operation_Type operationType, const std::string& filePath, const std::string& fileType) noexcept
                                       : Serializer_Core(operationType, Speculo::FileSystem::ValidateAndAppendFileExtension(filePath, ".dat"), fileType)
    {
        if (operationType == Serializer_Operation_Type::Serialization)
        {
            if (!FileSystem::ValidateFileDirectory(m_FilePath))
            {
                SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_FILESYSTEM_DIRECTORY_NOT_FOUND, m_FilePath);
                return;
            }

            BeginSerialization();
        }
        else if (operationType == Serializer_Operation_Type::Deserialization)
        {
            if (!FileSystem::ValidateFileExistence(m_FilePath))
            {
                SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_FILESYSTEM_FILE_NOT_FOUND, m_FilePath);
                return;
            }

            BeginDeserialization();
        }
    }

    Serializer_Binary::~Serializer_Binary()
    {
        if (m_IsStreamOpen)
        {
            SPECULO_THROW_WARNING(SpeculoResult::SPECULO_WARNING_BEST_PRACTICES, std::string("Consider explicitly ending serialization/deserialization with their respective functions to avoid possible issues. Errors may occur otherwise with destructors: ") + m_FilePath);
            if (m_OperationType == Serializer_Operation_Type::Serialization)
            {
                EndSerialization();
            }
            else if (m_OperationType == Serializer_Operation_Type::Deserialization)
            {
                EndDeserialization();
            }
        }
    }

    void Serializer_Binary::BeginSerialization()
    {
        int iosFlags = std::ios::binary | std::ios::out;
        m_OutputStream.open(m_FilePath, iosFlags); // Creates file if it does not exist.

        if (m_OutputStream.fail())
        {
            SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_SERIALIZATION_FAILURE, m_FilePath);
            return;
        }

        // Scattering stream checks all around to prevent people from trying to serialize/deserialize after ending the process.
        m_IsStreamOpen = true;

        SerializeProperty(m_FileType);
        SerializeProperty(m_Version_Major);
        SerializeProperty(m_Version_Minor);
        SerializeProperty(m_Version_Revision);
    }

    void Serializer_Binary::BeginDeserialization()
    {
        int iosFlags = std::ios::binary | std::ios::in;
        m_InputStream.open(m_FilePath, iosFlags);

        if (m_InputStream.fail())
        {
            SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_DESERIALIZATION_FAILURE, m_FilePath);
            return;
        }

        m_IsStreamOpen = true;

        ValidateMetadata();
    }

    void Serializer_Binary::SerializeProperty(const std::string& value)
    {
        if (m_IsStreamOpen)
        {
            const uint32_t stringSize = static_cast<uint32_t>(value.size());
            SerializeProperty(stringSize); // To resize our string on deserialization.
            m_OutputStream.write(const_cast<char*>(value.c_str()), stringSize);
        }
        else
        {
            SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_FILESTREAM_UNOPEN, m_FilePath);
            return;
        }
    }

    void Serializer_Binary::DeserializeProperty(std::string* value)
    {
        if (m_IsStreamOpen)
        {
            uint32_t stringSize = 0;
            DeserializeProperty(&stringSize);
            value->resize(stringSize);
            m_InputStream.read(const_cast<char*>(value->c_str()), stringSize);
        }
        else
        {
            SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_FILESTREAM_UNOPEN, m_FilePath);
            return;
        }
    }

    bool Serializer_Binary::ValidateMetadata()
    {
        if (DeserializePropertyAs<std::string>() != m_FileType)
        {
            SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_DESERIALIZATION_TYPE_MISMATCH, m_FilePath);
            return false;
        }
        
        if (DeserializePropertyAs<int>() != m_Version_Major)
        {
            SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_VERSION_MAJOR_MISMATCH, m_FilePath);
            return false;
        }

        if (DeserializePropertyAs<int>() != m_Version_Minor)
        {
            SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_VERSION_MINOR_MISMATCH, m_FilePath);
            return false;
        }

        if (DeserializePropertyAs<int>() != m_Version_Revision)
        {
            SPECULO_THROW_WARNING(SpeculoResult::SPECULO_WARNING_VERSION_REVISION_MISMATCH, m_FilePath);
        }

        return true;
    }

    void Serializer_Binary::EndSerialization()
    {
        if (m_IsStreamOpen)
        {
            m_OutputStream.flush();
            m_OutputStream.close();
            m_IsStreamOpen = false;
        }
        else
        {
            SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_FILESTREAM_UNOPEN, m_FilePath);
            return;
        }
    }

    void Serializer_Binary::EndDeserialization()
    {
        if (m_IsStreamOpen)
        {
            m_InputStream.clear();
            m_InputStream.close();
            m_IsStreamOpen = false;
        }
        else
        {
            SPECULO_THROW_ERROR(SpeculoResult::SPECULO_ERROR_FILESTREAM_UNOPEN, m_FilePath);
            return;
        }
    }
}