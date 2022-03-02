#pragma once
#include <iostream>

namespace Speculo
{

#define SPECULO_THROW_ERROR(errorCode, description)         std::cout << "ERROR: " << SpeculoResultToString(errorCode) << ": " << description << ".\n";

#define SPECULO_THROW_WARNING(warningCode, description)     std::cout << "WARNING: " << SpeculoResultToString(warningCode) << ": " << description << "\n";

#define SPECULO_ASSERT(value, errorCode, description)       if (value == false) { std::cout << "ERROR: " << SpeculoResultToString(errorCode) << ": " << description << "\n"; \
                                                            abort(); }

    // Warnings -> Flag out and continue execution.
    // Error    -> Flag out and return. Execution denied.
    // Critical -> Engine breakdown.

    enum class SpeculoResult
    {
        SPECULO_ERROR_FILESYSTEM_DIRECTORY_NOT_FOUND,
        SPECULO_ERROR_FILESYSTEM_FILE_NOT_FOUND,
        SPECULO_ERROR_FILESTREAM_UNOPEN,
        SPECULO_ERROR_SERIALIZATION_FAILURE,
        SPECULO_ERROR_DESERIALIZATION_FAILURE,
        SPECULO_ERROR_DESERIALIZATION_TYPE_MISMATCH,
        SPECULO_ERROR_VERSION_MAJOR_MISMATCH,
        SPECULO_ERROR_VERSION_MINOR_MISMATCH,
        SPECULO_WARNING_VERSION_REVISION_MISMATCH,
        SPECULO_WARNING_BEST_PRACTICES
    };
    
    inline std::string SpeculoResultToString(const SpeculoResult result)
    {
        switch (result)
        {
            case SpeculoResult::SPECULO_ERROR_FILESYSTEM_DIRECTORY_NOT_FOUND:       return "SPECULO_ERROR_FILESYSTEM_DIRECTORY_NOT_FOUND";
            case SpeculoResult::SPECULO_ERROR_FILESYSTEM_FILE_NOT_FOUND:            return "SPECULO_ERROR_FILESYSTEM_FILE_NOT_FOUND";
            case SpeculoResult::SPECULO_ERROR_FILESTREAM_UNOPEN:                    return "SPECULO_WARNING_FILESTREAM_UNOPEN";
            case SpeculoResult::SPECULO_ERROR_SERIALIZATION_FAILURE:                return "SPECULO_ERROR_SERIALIZATION_FAILURE";
            case SpeculoResult::SPECULO_ERROR_DESERIALIZATION_FAILURE:              return "SPECULO_ERROR_DESERIALIZATION_FAILURE";
            case SpeculoResult::SPECULO_ERROR_DESERIALIZATION_TYPE_MISMATCH:        return "SPECULO_ERROR_DESERIALIZATION_TYPE_MISMATCH";
            case SpeculoResult::SPECULO_ERROR_VERSION_MAJOR_MISMATCH:               return "SPECULO_ERROR_VERSION_MAJOR_MISMATCH";
            case SpeculoResult::SPECULO_ERROR_VERSION_MINOR_MISMATCH:               return "SPECULO_ERROR_VERSION_MINOR_MISMATCH";
            case SpeculoResult::SPECULO_WARNING_VERSION_REVISION_MISMATCH:          return "SPECULO_ERROR_VERSION_REVISION_MISMATCH";
            case SpeculoResult::SPECULO_WARNING_BEST_PRACTICES:                     return "SPECULO_WARNING_BEST_PRACTICES";
        }
    
        return "Unknown Result Macro Specified!";
    }
}