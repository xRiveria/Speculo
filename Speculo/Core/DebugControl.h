#pragma once
#include <iostream>

namespace Speculo
{

#define SPECULO_THROW_ERROR(errorCode, description)         std::cout << "ERROR: " << SpeculoResultToString(errorCode) << ": " << description << "\n"; \
                                                            abort();

#define SPECULO_THROW_WARNING(warningCode, description)     std::cout << "WARNING: " << SpeculoResultToString(warningCode) << ": " << description << "\n";

    // Warnings -> Flag out and continue execution.
    // Error    -> Flag out and return. Execution denied.
    // Critical -> Engine breakdown.

    enum class SpeculoResult
    {
        SPECULO_ERROR_FILESYSTEM_DIRECTORY_NOT_FOUND,
        SPECULO_ERROR_SERIALIZATION_FAILURE,
        SPECULO_ERROR_DESERIALIZATION_FAILURE,
        SPECULO_ERROR_DESERIALIZATION_TYPE_MISMATCH,
        SPECULO_ERROR_VERSION_MAJOR_MISMATCH,
        SPECULO_ERROR_VERSION_MINOR_MISMATCH,
        SPECULO_WARNING_VERSION_REVISION_MISMATCH
    };
    
    inline std::string SpeculoResultToString(const SpeculoResult result)
    {
        switch (result)
        {
            case SpeculoResult::SPECULO_ERROR_FILESYSTEM_DIRECTORY_NOT_FOUND:       return "SPECULO_ERROR_FILESYSTEM_DIRECTORY_NOT_FOUND";
            case SpeculoResult::SPECULO_ERROR_SERIALIZATION_FAILURE:                return "SPECULO_ERROR_SERIALIZATION_FAILURE";
            case SpeculoResult::SPECULO_ERROR_DESERIALIZATION_FAILURE:              return "SPECULO_ERROR_DESERIALIZATION_FAILURE";
            case SpeculoResult::SPECULO_ERROR_VERSION_MAJOR_MISMATCH:               return "SPECULO_ERROR_VERSION_MAJOR_MISMATCH";
            case SpeculoResult::SPECULO_ERROR_VERSION_MINOR_MISMATCH:               return "SPECULO_ERROR_VERSION_MINOR_MISMATCH";
            case SpeculoResult::SPECULO_WARNING_VERSION_REVISION_MISMATCH:          return "SPECULO_ERROR_VERSION_REVISION_MISMATCH";
        }
    
        return "Unknown Result Macro Specified!";
    }
}