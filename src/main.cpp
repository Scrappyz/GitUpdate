#include <iostream>
#include "CLI11.hpp"
#include "os.hpp"

namespace path = os::path;

int main(int argc, char** argv)
{
    CLI::App app("Updater");
    app.set_version_flag("-v,--version", "v0.1.0");

    std::vector<std::string> caller_info;
    std::string url;
    app.add_option("-c,--caller", caller_info, "Path to the source caller and its current version.\n(E.g. \"path/to/caller.exe\" \"v1.0.0\")")->expected(2);
    app.add_option("-u,--url", url, "URL to get the update from.")->expected(1);

    CLI11_PARSE(app, argc, argv);

    // Get update from url then place it in the caller path with a unique name.
    // Delete the caller executable then rename the new executable.

    return 0;
}