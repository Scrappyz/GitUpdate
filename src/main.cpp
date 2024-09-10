#include <iostream>
#include "updater.hpp"

int main()
{
    std::string url = "https://github.com/Scrappyz/Test-Updater";

    updater::updateApp(url, "test_updater.exe", true);
    return 0;
}