#include <iostream>
#include "CLI11.hpp"
#include "os.hpp"

namespace path = os::path;

int main(int argc, char** argv)
{
    std::string command = R"(D:\"My Files"\Codes\C++\Tools\ctemplate\bin\Release\ctemplate.exe info "python")";
    std::string output;
    std::cout << (os::execute(command, output)) << std::endl;
    std::cout << output << std::endl;

    return 0;
}