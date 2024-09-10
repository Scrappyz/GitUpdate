#include <iostream>
#include "os.hpp"
#include "json.hpp"
#include "updater.hpp"

namespace path = os::path;
using json = nlohmann::json;

int main(int argc, char** argv)
{
    std::string url = "https://github.com/Scrappyz/Test-Updater.git";
    std::string tag = "v1.1.0";

    std::cout << updater::isCurlInstalled() << std::endl;

    // updater::update(url, "test_updater.exe");
    return 0;
}