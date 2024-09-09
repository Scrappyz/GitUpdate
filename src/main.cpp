#include <iostream>
#include "CLI11.hpp"
#include "os.hpp"
#include "json.hpp"
#include "updater.hpp"

namespace path = os::path;
using json = nlohmann::json;

int main(int argc, char** argv)
{
    CLI::App app("Updater");
    app.set_version_flag("-v,--version", "v0.1.0");

    std::vector<std::string> caller_info;
    std::string url = "https://github.com/Scrappyz/Test-Updater.git";
    std::string tag = "v1.1.0";
    // app.add_option("-c,--caller", caller_info, "Path to the source caller and its current version.\n(E.g. \"path/to/caller.exe\" \"v1.0.0\")")->expected(2);
    // app.add_option("-u,--url", url, "URL to get the update from.")->expected(1);

    // CLI11_PARSE(app, argc, argv);

    // Get update from url then place it in the caller path with a unique name.
    // Delete the caller executable then rename the new executable.

    std::string latest_tag = updater::getLatestReleaseTag(updater::convertToAPIUrl(url));
    std::string output = path::joinPath(path::sourcePath(), "updater.exe1");
    std::cout << output << std::endl;
    
    std::cout << updater::downloadAsset(updater::cleanUrl(url), 
                    "v1.0.1", "test_updater.exe",
                    "\"" + output + "\"");

    updater::updateApp(path::sourcePath(false), output);

    // Launch the updater thread just before exiting
    // std::thread updater_thread(updater::updateApp, path::sourcePath(), output);
    // updater_thread.detach();

    return 0;
}