#pragma once

#include "json.hpp"
#include <string>
#include <cstdio>
#include <filesystem>

#if defined(_WIN32)
    #include <windows.h>
    #include <strsafe.h>
#else
    #include <unistd.h>
    #include <libgen.h>
#endif

namespace updater {

    #if defined(_WIN32)
        std::string curl_path = "curl.exe";
    #else
        std::string curl_path = "curl";
    #endif

    namespace _private_ {
        bool execute(const std::string& command, std::string& output, const std::string& mode = "r");
        bool execute(const std::string& command, const std::string& mode = "r");
    }

    inline std::string sourcePath(bool parent_path = true) 
    {
        std::filesystem::path source_path;
        #if defined(_WIN32)
            char path[MAX_PATH];
            GetModuleFileName(NULL, path, MAX_PATH);
            source_path = path;
        #elif defined(__linux__) || defined(__apple__)
            source_path = std::filesystem::canonical("/proc/self/exe");
        #else
            throw std::runtime_error(_private::errorMessage(__func__, "Unknown Operating System"));
        #endif

        if(parent_path) {
            return source_path.parent_path().string();
        }

        return source_path.string();
    }

    #if defined(_WIN32)
        /*
            Deletes the running executable.

            Parameters:
            `source_path`: Path to the running executable. (Defaults to running executable)

            Notes:
            - Only works if the program is terminated after 2 seconds so it would be best to run this right before program exit.
        */
        inline void removeSelf(const std::filesystem::path& source_path = "")
        {
            std::string remove_cmd = "cmd.exe /C ping 1.1.1.1 -n 1 -w 2000 > Nul & Del /f /q \"%s\"";
            TCHAR szCmd[2 * MAX_PATH];
            STARTUPINFO si = {0};
            PROCESS_INFORMATION pi = {0};

            std::string source_path_str = source_path.string();
            if(source_path_str.empty()) {
                source_path_str = sourcePath(false);
            }

            StringCbPrintf(szCmd, 2 * MAX_PATH, remove_cmd.c_str(), source_path_str.c_str());

            CreateProcess(NULL, szCmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
        }
    #else
        inline void removeSelf()
        {
            char szModuleName[1024];
            ssize_t len = readlink("/proc/self/exe", szModuleName, sizeof(szModuleName) - 1);

            if (len != -1) {
                szModuleName[len] = '\0';
                char* dir = dirname(szModuleName);
                char command[2 * 1024];

                snprintf(command, sizeof(command), "sleep 3 && rm -f \"%s\"", szModuleName);
                if (fork() == 0) {
                    execl("/bin/sh", "sh", "-c", command, (char *)NULL);
                    exit(EXIT_SUCCESS);
                }
            }
        }
    #endif

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

    /*
        Gets the release information from github of the given tag.

        Parameters:
        `repo_url`: URL to the github repository.
        `tag`: The tag name to get.
    */
    inline nlohmann::json getReleaseJson(const std::string& repo_url, const std::string& tag)
    {
        std::string api_url = convertToAPIUrl(repo_url);
        std::string output;

        _private_::execute(curl_path + " -s " + api_url + "/releases/tags/" + tag, output);

        return nlohmann::json::parse(output);
    }

    /*
        Gets the latest release information from github.

        Parameters:
        `repo_url`: URL to the github repository.
    */
    inline nlohmann::json getLatestReleaseJson(const std::string& repo_url)
    {
        std::string api_url = convertToAPIUrl(repo_url);
        std::string output;

        _private_::execute(curl_path + " -s " + api_url + "/releases/latest", output);

        return nlohmann::json::parse(output);
    }

    /*
        Gets the list of tags.

        Parameters:
        `repo_url`: URL to the github repository.
    */
    inline nlohmann::json getTagsJson(const std::string& repo_url)
    {
        std::string api_url = convertToAPIUrl(repo_url);
        std::string output;

        _private_::execute(curl_path + " -s " + api_url + "/tags", output);

        return nlohmann::json::parse(output);
    }

    /*
        Downloads an asset from a given repository and tag.

        Parameters:
        `repo_url`: URL to the repo.
        `tag`: Tag to download asset from.
        `asset_name`: Name of the asset to download from the given tag.
        `output_path`: Path to the output file.
    */
    inline bool downloadAsset(const std::string& repo_url, const std::string& tag, const std::string& asset_name, const std::filesystem::path& output_path)
    {
        std::string download_url = cleanUrl(repo_url) + "/releases/download/" + tag + "/" + asset_name;
        std::string command = curl_path + " -s -L " + download_url + " -o " + "\"" + output_path.string() + "\"";
        return _private_::execute(command);
    }

    /*
        Updates the application by replacing the current app with the new one.

        Parameters:
        `repo_url`: URL to the github repo.
        `tag`: Tag to update application to.
        `asset_name`: The asset to download.

        Notes:
        - Only works if the program is terminated after 2 seconds so it would be best to run this right before program exit.
    */
    inline void update(const std::string& repo_url, const std::string& tag, const std::string& asset_name)
    {
        nlohmann::json release_info = getReleaseJson(repo_url, tag);
        
        std::filesystem::path source_path = sourcePath(false);
        std::filesystem::path source_temp = source_path.string() + "1";
        std::filesystem::path new_source_path = source_path.string() + "2";

        for(const auto& i : release_info.at("assets")) {
            if(i.at("name") == asset_name) {
                downloadAsset(repo_url, release_info.at("tag_name"), asset_name, source_temp);
                break;
            }
        }

        if(!std::filesystem::exists(source_temp)) {
            return;
        }

        std::filesystem::rename(source_path, new_source_path);
        std::filesystem::rename(source_temp, source_path);

        removeSelf(new_source_path);
    }

    /*
        Updates the application by replacing the current app with the new one.

        Parameters:
        `repo_url`: URL to the github repo.
        `asset_name`: The asset to download.

        Notes:
        - Only works if the program is terminated after 2 seconds so it would be best to run this right before program exit.
    */
    inline void update(const std::string& repo_url, const std::string& asset_name)
    {
        update(repo_url, "", asset_name);
    }

    /*
        Checks if cURL is installed.
    */
    inline bool isCurlInstalled()
    {
        return _private_::execute(curl_path + " -V");
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
