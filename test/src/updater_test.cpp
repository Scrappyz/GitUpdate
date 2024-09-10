#include <gtest/gtest.h>
#include "updater.hpp"
#include "os.hpp"

namespace path = os::path;
using json = nlohmann::json;

std::string test_path = path::joinPath(path::sourcePath(), "../test_path");
std::string repo_url = "https://github.com/Scrappyz/Test-Updater";
std::string resource_path = path::joinPath(test_path, "resources");

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

TEST(getAPIUrl, git_extension)
{
    EXPECT_EQ(updater::getAPIUrl(repo_url), "https://api.github.com/repos/Scrappyz/Test-Updater");
}

TEST(getAPIUrl, no_git_extesion)
{
    EXPECT_EQ(updater::getAPIUrl(repo_url), "https://api.github.com/repos/Scrappyz/Test-Updater");
}

TEST(getReleaseJson, working)
{
    json actual = updater::getReleaseJson(repo_url, "v1.0.0");

    EXPECT_EQ(actual, readJsonFromFile(path::joinPath(resource_path, "v1_0_0.json")));
}

TEST(getLatestReleaseJson, working)
{
    json actual = updater::getLatestReleaseJson(repo_url);
    
    EXPECT_EQ(actual, readJsonFromFile(path::joinPath(resource_path, "latest.json")));
}

TEST(getTagListJson, working)
{
    json actual = updater::getTagListJson(repo_url);

    EXPECT_EQ(actual, readJsonFromFile(path::joinPath(resource_path, "tags.json")));
}