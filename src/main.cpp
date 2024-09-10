#include <iostream>
#include "gitupdate.hpp"

int main()
{
    std::string url = "https://github.com/Scrappyz/Test-Updater";

    gitupdate::updateApp(url, "test_updater.exe", true);
    return 0;
}