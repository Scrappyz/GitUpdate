#pragma once

#include "json.hpp"
#include "os.hpp"

namespace updater {

    /*
        Gets the release information from github of the given tag.

        Parameters:
        `url`: URL to the github repository.
        `tag`: The tag name to get.

        Notes:
        - Gets the latest release if no tag is specified.
    */
    nlohmann::json getRelease(const std::string& url, const std::string& tag = "")
    {
        std::string output;

        if(tag.empty()) {
            os::execute("curl.exe -s " + url + "/releases/latest", output);
        } else {
            os::execute("curl.exe -s " + url + "/releases/tags/" + tag, output);
        }

        return nlohmann::json::parse(output);
    }

    std::string getLatestReleaseTag(const std::string& url)
    {
        std::string output;
        os::execute("curl.exe -s " + url + "/releases/latest", output);

        return nlohmann::json::parse(output).at("tag_name");
    }

    bool downloadAsset(const std::string& url, const std::string& tag, const std::string& asset_name, const std::string& output_path)
    {
        std::string download_url = url + "/releases/download/" + tag + "/" + asset_name;
        std::cout << download_url << std::endl;
        return os::execute("curl.exe -s -L " + download_url + " -o " + output_path);
    }

    std::string parseUrl(const std::string& url)
    {
        std::string result = "https://api.github.com/repos/" + url.substr(19, url.size() - 19);

        std::string appended = result.substr(result.size() - 4, 4);
        if(appended == ".git") {
            for(int i = 0; i < 4; i++) result.pop_back();
        }
        return result;
    }
}
