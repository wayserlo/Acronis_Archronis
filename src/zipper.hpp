#pragma once

#include <src/collecter/collecter.hpp>

#include <vector>

namespace archiver {

void ArchiveFiles(int files_num, char* files[], char* archive_name);

void UnarhiveFiles(char* filepath);

}   // namespace archiver