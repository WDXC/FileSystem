#ifndef FILEOPERATOR_H
#define FILEOPERATOR_H

#include <fstream>
#include <iostream>
#include "common.h"

class FileOperator {
    public:
        FileOperator();
        ~FileOperator();

    public:
        void CreateFile(const char* pathname);
        void DestoryFile();
        void WriteFile(const char* characters);
        void ReadFile();

    private:
        void CreateFileCurPath();
        void CreateFileSpecifyPath();

        void DestoryFileCurPath();
        void DestoryFileSpecifyPath();

        void WriteAll();
        void WriteFileByBytes();
        void WriteSpecifyLocation();


        void ReadAll();
        void ReadFileBytes();
        void ReadSpecifyLabel();

        char* JudgeFilePath();

    private:
        std::ifstream input_stream;
        std::ofstream output_stream;
        char path_[MAX_PATH_LENGTH];
};


#endif
