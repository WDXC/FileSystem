#include <fstream>
#include <iostream>
#include <string.h>
#include "FileOperator.h"

FileOperator::FileOperator() {
    memset(path_, 0, MAX_PATH_LENGTH);
}

FileOperator::~FileOperator() {

}

// filter create method by input pathname
void FileOperator::CreateFile(const char* pathname) {
    memcpy(path_, (void*)pathname, sizeof(pathname));
    for (int i = 0; i < sizeof(path_); ++i) {
        if (path_[i] == '/') {
            CreateFileSpecifyPath();
            return;
        }
    }
    CreateFileCurPath();
    return;
}

void FileOperator::DestoryFile() {

}

void FileOperator::WriteFile() {

}

void FileOperator::ReadFile() {

}

void FileOperator::CreateFileCurPath() {
    output_stream = std::ofstream(path_);
}

void FileOperator::CreateFileSpecifyPath() {
}

void FileOperator::DestoryFileCurPath() {

}

void FileOperator::DestoryFileSpecifyPath() {

}

void FileOperator::WriteAll() {

}

void FileOperator::WriteFileByBytes() {

}

void FileOperator::WriteSpecifyLocation() {

}

void FileOperator::ReadFileBytes() {

}

void FileOperator::ReadSpecifyLabel() {

}
