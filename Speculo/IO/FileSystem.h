#pragma once
#include <string>

namespace Speculo
{
    class FileSystem
    {
    public:
        static std::string GetProjectDirectory();
        static std::string GetFileDirectory(const std::string& filePath);
        static bool ValidateFileDirectory(const std::string& filePath);
    };
}