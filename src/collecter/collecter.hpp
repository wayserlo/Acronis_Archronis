#pragma once

#include <src/support/Istreams.hpp>

#include <vector>

namespace archiver {

std::vector<byte> UniteFilesInOneBinary(std::vector<std::string> filenames);

std::vector<std::vector<byte>> SeparateFiles(const std::vector<byte>& binary);

}   // namespace archiver