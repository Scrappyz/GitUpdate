#include <gtest/gtest.h>
#include "gitupdate.hpp"
#include "os.hpp"

namespace path = os::path;
using json = nlohmann::json;

std::string test_path = path::joinPath(path::sourcePath(), "../test_path");
std::string repo_url = "https://github.com/Scrappyz/Test-Updater";
std::string resource_path = path::joinPath(test_path, "resources");
std::string temp_path = path::joinPath(test_path, "temp");
std::string asset_name = "test_updater.exe";

std::string readTextFromFile(const std::string& file_path)
{
    std::ifstream file(file_path);
    std::stringstream ss;
    ss << file.rdbuf();
    file.close();

    return ss.str();
}

json readJsonFromFile(const std::string& file)
{
    json j;
    std::ifstream i(file);
    i >> j;
    i.close();
    
    return j;
}

TEST(getGithubAPIUrl, git_extension)
{
    EXPECT_EQ(gitupdate::getGithubAPIUrl(repo_url), "https://api.github.com/repos/Scrappyz/Test-Updater");
}

TEST(getGithubAPIUrl, no_git_extesion)
{
    EXPECT_EQ(gitupdate::getGithubAPIUrl(repo_url), "https://api.github.com/repos/Scrappyz/Test-Updater");
}

TEST(getReleaseJson, working)
{
    json actual = gitupdate::getReleaseJson(repo_url, "v1.0.0");

    EXPECT_EQ(actual, readJsonFromFile(path::joinPath(resource_path, "v1_0_0.json")));
}

TEST(getReleaseJson, failure)
{
    json actual = gitupdate::getReleaseJson(repo_url, "v0.0.0");

    EXPECT_EQ(actual, json());
}

TEST(getTagListJson, working)
{
    json actual = gitupdate::getTagListJson(repo_url);

    EXPECT_EQ(actual, readJsonFromFile(path::joinPath(resource_path, "tags.json")));
}

TEST(downloadAsset, working)
{
    if(!path::exists(temp_path)) {
        path::createDirectory(temp_path);
    }

    json j = gitupdate::getLatestReleaseJson(repo_url, true);
    std::string output_path = path::joinPath(temp_path, asset_name);

    gitupdate::downloadAsset(repo_url, j.at("tag_name"), asset_name, output_path);

    ASSERT_TRUE(path::exists(output_path));

    path::remove(output_path);
}