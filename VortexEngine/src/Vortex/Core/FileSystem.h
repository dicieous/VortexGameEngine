#pragma once

#include "Vortex/Core/Buffer.h"
#include <filesystem>

namespace Vortex
{
    class FileSystem
    {
    public:
        static Buffer ReadFileBinary(const std::filesystem::path& filepath);
    };
}