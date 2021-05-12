#include <src/zipper.hpp>

#include <iostream>
#include <string.h>


int main(int argc, char* argv[]) {
    bool encode = true;

    if (strcmp(argv[1], "-e") == 0) {
        encode = true;
    } else if (strcmp(argv[1], "-d") == 0) {
        encode = false;
    } else {
        std::cerr << "Flag -e or -d needed" << std::endl;
        exit(1);
    }

    if (encode) {
        // Archive files
        archiver::ArchiveFiles(argc - 2, argv + 2, "archive.myzip");
    } else {
        // Unarchive files
        archiver::UnarhiveFiles(argv[2]);
    }

    return 0;
}