#pragma once

#include "json.hpp"
#include <string>
#include <cstdio>
#include <filesystem>
#include <thread>
#include <chrono>

namespace updater {

    namespace _private_ {
        bool execute(const std::string& command, std::string& output, const std::string& mode = "r");
        bool execute(const std::string& command, const std::string& mode = "r");
    }

    /*
        Gets the release information from github of the given tag.

        Parameters:
        `url`: URL to the github repository.
        `tag`: The tag name to get.

        Notes:
        - Gets the latest release if no tag is specified.
    */
    inline nlohmann::json getRelease(const std::string& url, const std::string& tag = "")
    {
        std::string output;

        if(tag.empty()) {
            _private_::execute("curl.exe -s " + url + "/releases/latest", output);
        } else {
            _private_::execute("curl.exe -s " + url + "/releases/tags/" + tag, output);
        }

        return nlohmann::json::parse(output);
    }

    inline std::string getLatestReleaseTag(const std::string& url)
    {
        std::string output;
        _private_::execute("curl.exe -s " + url + "/releases/latest", output);

        return nlohmann::json::parse(output).at("tag_name");
    }

    /*
        Downloads an asset from a given repository and tag.

        Parameters:
        `repo_url`: URL to the repo. (E.g. `https://github.com/{USER}/{REPO}.git`)
        `tag`: Tag to download asset from.
        `asset_name`: Name of the asset to download from the given tag.
        `output_path`: Path to the output file. (Surround with quotations to avoid errors)
    */
    inline bool downloadAsset(const std::string& repo_url, const std::string& tag, const std::string& asset_name, const std::string& output_path)
    {
        std::string download_url = repo_url + "/releases/download/" + tag + "/" + asset_name;
        std::cout << download_url << std::endl;
        return _private_::execute("curl.exe -s -L " + download_url + " -o " + output_path);
    }

    inline bool updateApp(const std::filesystem::path& original_app_path, const std::filesystem::path& new_app_path)
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));

        std::string original_name = original_app_path.filename().string();
        std::string new_name = new_app_path.filename().string();
        std::filesystem::path renamed_original_path = original_app_path.parent_path() / std::filesystem::path("old_" + original_name);

        std::filesystem::rename(original_app_path, renamed_original_path);
        std::filesystem::rename(new_app_path, new_app_path.parent_path() / std::filesystem::path(original_name));

        std::filesystem::remove_all(renamed_original_path);

        return true;
    }   

    inline bool removeOldApp(const std::filesystem::path& source_path = "")
    {
        if(source_path.empty()) {
            return false;
        }

        std::string filename = source_path.filename().string();

        std::filesystem::path old_app = std::filesystem::weakly_canonical(source_path / std::filesystem::path("old_" + filename));
        if(!std::filesystem::exists(old_app)) {
            return false;
        }

        std::filesystem::remove_all(old_app);

        return true;
    }

    /*
        Cleans the url.
    */
    inline std::string cleanUrl(const std::string& url)
    {
        std::string result = url;

        std::string appended = result.substr(result.size() - 4, 4);
        if(appended == ".git") {
            for(int i = 0; i < 4; i++) result.pop_back();
        }

        return result;
    }

    /*
        Converts the given github repo url to an API url.

        Parameters:
        `repo_url`: URL to the github repository. (E.g. `https://github.com/{USER}/{REPO}.git`)
    */
    inline std::string convertToAPIUrl(const std::string& repo_url)
    {
        std::string result = "https://api.github.com/repos/" + repo_url.substr(19, repo_url.size() - 19);
        return cleanUrl(result);
    }

    namespace _private_ {

        /*
            Execute a command in the shell.

            Return Value:
            - Returns `true` if the command executes successfully. Else it will return `false`.

            Parameters:
            `command`: Command to execute.
            `output`: A string reference to get the output of the executed command.
            `mode`: Use `r` for read-mode, `w` for write-mode.
        */
        inline bool execute(const std::string& command, std::string& output, const std::string& mode)
        {
            // Open the pipe using platform-specific popen function
            FILE* pipe = popen(command.c_str(), mode.c_str());
            if(!pipe) {
                return false;  // Command failed to execute
            }

            char buffer[128];
            output.clear();  // Clear the output string before capturing the result

            // Read the command's output chunk by chunk
            while(fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                output.append(buffer);  // Append each chunk to the output string
            }

            // Get the command exit status
            int exit_code = pclose(pipe);
            
            // Return true if the command was successful (exit code is 0)
            return (exit_code == 0);
        }

        /*
            Execute a command in the shell.

            Return Value:
            - Returns `true` if the command executes successfully. Else it will return `false`.

            Parameters:
            `command`: Command to execute.
            `mode`: Use `r` for read-mode, `w` for write-mode.
        */
        inline bool execute(const std::string& command, const std::string& mode)
        {
            std::string dummy_output;
            return execute(command, dummy_output, mode);
        }

    }
}
