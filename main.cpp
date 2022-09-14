#include <iostream>
#include <unistd.h>
#include <random>
#include <string>
#include "FileOperator.h"

using namespace std;


int main() {
    FileOperator *a = new FileOperator();
    a->CreateFile("/home/neil/mine/MyProject/C++/multhread/234");
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(100, 100000000);
    for (int i = 0; i < 200; ++i) {
        long k = distr(gen);
        string res = std::to_string(k);
        a->WriteFile(res.c_str());
    }
    return 0 ;
}
