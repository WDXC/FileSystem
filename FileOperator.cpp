#include <fstream>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <string.h>
#include "FileOperator.h"

FileOperator::FileOperator() {
    memset(path_, 0, MAX_PATH_LENGTH);
}

FileOperator::~FileOperator() {

}

// filter create method by input pathname
void FileOperator::CreateFile(const char* pathname) {

    if (!pathname) {
        return;
    }

    memcpy(path_, (void*)pathname, strlen(pathname));
    char* ret = JudgeFilePath();

    if (!*ret) {
        CreateFileCurPath();
        return;
    }
    CreateFileSpecifyPath();
}

void FileOperator::DestoryFile() {
    std::remove(path_);
}

void FileOperator::WriteFile(const char* characters) {
    output_stream.open(path_, std::ios::app | std::ios::out);
    if (!output_stream) {
        std::cerr << "open " << path_ << " failed!" << std::endl;
        exit(1);
    }
    output_stream << characters << std::endl;
    output_stream.close();
}

void FileOperator::ReadFile() {

}

void FileOperator::CreateFileCurPath() {
    output_stream = std::ofstream(path_);
}

void FileOperator::CreateFileSpecifyPath() {
    char* dst_path;
    snprintf(dst_path, MAX_PATH_LENGTH + 6, "touch %s", path_);
    std::system(dst_path);
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

char* FileOperator::JudgeFilePath() {
    char* end = path_ + strlen(path_);
    char* ret = std::find(path_, end, '/');
    return ret;
}
