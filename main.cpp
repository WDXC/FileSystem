#include <iostream>
#include "FileOperator.h"

using namespace std;


int main() {
    FileOperator *a = new FileOperator();
    a->CreateFile("abc");
    return 0 ;
}
