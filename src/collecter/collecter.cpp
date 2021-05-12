#include "collecter.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace archiver {

void AddIntegerInBytes(std::vector<byte>& binary, uint32_t num) {
    for (size_t i = 0; i < 4; i++) {
        binary.push_back(static_cast<byte>((num << (i * 8)) >> 24));
    }
}

uint32_t GetIntegerFromBytes(std::vector<byte>::const_iterator& binary_iter) {
    uint32_t result = 0;
    for (size_t i = 0; i < 4; i++) {
        result |= (((static_cast<uint32_t>(*binary_iter) >> ((3u - i) * 8)) << 24) >> (i * 8));

        binary_iter++;
    }

    return result;
}


void ParseFileInBinary(std::vector<byte>& binary, const std::string& filename) {
    struct stat st;
    stat(filename.c_str(), &st);

    AddIntegerInBytes(binary, st.st_size);

    int fd = open(filename.c_str(), O_RDONLY);

    byte num;

    while (read(fd, &num, 1) > 0) {
        binary.push_back(num);
    }

    close(fd);
}

std::vector<byte> UniteFilesInOneBinary(std::vector<std::string> filenames) {
    std::vector<byte> binary;

    AddIntegerInBytes(binary, filenames.size());

    for (size_t i = 0; i < filenames.size(); i++) {
        ParseFileInBinary(binary, filenames[i]);
    }

    return std::move(binary);
}


std::vector<byte> ParseFile(std::vector<byte>::const_iterator& binary_iter) {
    uint32_t size = GetIntegerFromBytes(binary_iter);

    std::vector<byte> result;

    for (size_t i = 0; i < size; i++) {
        result.push_back(*binary_iter);
        binary_iter++;
    }

    return std::move(result);
}

std::vector<std::vector<byte>> SeparateFiles(const std::vector<byte>& binary) {
    auto binary_iter = binary.begin();
    uint32_t files_num = GetIntegerFromBytes(binary_iter);

    std::vector<std::vector<byte>> result;

    for (size_t i = 0; i < files_num; i++) {
        result.push_back(ParseFile(binary_iter));
    }       

    return std::move(result);
}

}   // namespace archiver