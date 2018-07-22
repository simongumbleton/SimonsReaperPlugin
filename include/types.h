#pragma once
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <string>

using uint32 = std::uint32_t;
using uint16 = std::uint16_t;

using int32 = std::int32_t;
using int16 = std::int16_t;

//TODO: sort this out with CMake
#define FILESYSTEM_EXPERIMENTAL

#ifdef FILESYSTEM_EXPERIMENTAL
#include <filesystem>
namespace fs = std::experimental::filesystem;
#endif
#ifdef FILESYSTEM_NATIVE
#include <filesystem> 
namespace fs = std::filesystem;
#endif
#ifdef FILESYSTEM_BOOST
namespace fs = boost::filesystem;
#endif

enum class ImportObjectType
{
    SFX,
    Voice,
    Music
};

enum class WAAPIImportOperation
{
    createNew,
    useExisting,
    replaceExisting
};

