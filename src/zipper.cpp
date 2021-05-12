#include "zipper.hpp"

#include <src/support/Istreams.hpp>
#include <src/encoder/encoder.hpp>
#include <src/collecter/collecter.hpp>
#include <vector>
#include <string>
#include <iostream>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace archiver {

struct InputStream : IInputStream {
    InputStream(std::vector<byte>& input) : input(input) {
    }

    bool Read(byte& value) override {
        if (iter == input.size()) {
            return false;
        }

        value = input[iter++];
        return true;
    }
    
    size_t iter{0};
    std::vector<byte>& input;
};

struct OutputStream : IOutputStream {
    OutputStream(std::vector<byte>& output) : result(output) {
    }

    void Write(byte value) override {
        result.push_back(value);
    }

    std::vector<byte>& result;
};



void ArchiveFiles(int files_num, char* files[], char* archive_name) {
    std::vector<std::string> filenames;
    for (size_t i = 0; i < files_num; i++) {
        filenames.emplace_back(files[i]);
    }

    std::vector<byte> binary = UniteFilesInOneBinary(filenames);

    std::vector<byte> binary_result;

    InputStream input_stream(binary);
    OutputStream output_stream(binary_result);

    // Code binary
    Encode(input_stream, output_stream);


    int fd = open(archive_name, O_CREAT | O_RDWR);

    for (size_t i = 0; i < binary_result.size(); i++) {
        write(fd, &binary_result[i], 1);
    }

    close(fd);
}

void UnarhiveFiles(char* filepath) {
    struct stat st;
    stat(filepath, &st);

    std::vector<byte> binary;
    binary.reserve(st.st_size);

    int fd = open(filepath, O_RDONLY);

    byte num;

    while (read(fd, &num, 1) > 0) {
        binary.push_back(num);
    }

    close(fd);

    std::vector<byte> binary_result;

    InputStream input_stream(binary);
    OutputStream output_stream(binary_result);

    // Decode binary
    Decode(input_stream, output_stream);

    std::vector<std::vector<byte>> files_binary = SeparateFiles(binary_result);


    for (size_t i = 0; i < files_binary.size(); i++) {
        std::string name = "file_" + std::to_string(i + 1) + ".txt";

        int fd_file = open(name.c_str(), O_CREAT | O_RDWR);

        for (size_t j = 0; j < files_binary[i].size(); j++) {
            write(fd_file, &files_binary[i][j], 1);
        }

        close(fd_file);
    }
}

}   // namespace archiver