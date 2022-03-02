#include "SpeculoPCH.h"
#include "FileSystem.h"
#include <filesystem>

namespace Speculo
{
    std::string FileSystem::GetProjectDirectory()
    {
        return std::filesystem::current_path().generic_string();
    }

    std::string FileSystem::GetFileDirectory(const std::string& filePath)
    {
        const size_t lastIndex = filePath.find_last_of("\\/");
        if (lastIndex != std::string::npos)
        {
            return filePath.substr(0, lastIndex + 1);
        }

        return "";
    }

    bool FileSystem::ValidateFileDirectory(const std::string& filePath)
    {
        std::string fileDirectory = GetFileDirectory(filePath);
        return std::filesystem::exists(fileDirectory) && std::filesystem::is_directory(fileDirectory);
    }

    bool FileSystem::ValidateFileExistence(const std::string& filePath)
    {
        return std::filesystem::exists(filePath);
    }

    std::string FileSystem::ValidateAndAppendFileExtension(const std::string& filePath, const std::string& fileExtension)
    {
        const size_t lastIndex = filePath.find_last_of('.');
        if (lastIndex != std::string::npos)
        {
            if (filePath.substr(lastIndex) != fileExtension)
            {
                return filePath.substr(lastIndex - 1) + fileExtension;
            }
            else
            {
                return filePath;
            }
        }
        else
        {
            return filePath + fileExtension;
        }
    }
}