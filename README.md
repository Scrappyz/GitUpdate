# GitUpdate
A header-only library to update C++ applications using the Github API.

## Installation
### Requirements
- [cURL](https://curl.se/download.html): To make https requests.
- [json](https://github.com/nlohmann/json): To read and parse JSON files.

[cURL](https://curl.se/download.html) is required to be installed on the user's machine in order for the auto-update to work. This shouldn't be much of a problem since cURL is usually installed by default on Windows 10 and above. To check if cURL is installed, open your terminal and type in `curl.exe -V` for **Windows** or `curl -V` for **Linux**. [Json](https://github.com/nlohmann/json) needs to be included in your project for `gitupdate.hpp` to work. Thankfully, `nlohmann/json` is a header-only library so including it in your project should be straightforward.

### Setup
To use this library, simply download [gitupdate.hpp](https://github.com/Scrappyz/gitupdate/releases) and [json.hpp](https://github.com/nlohmann/json/releases) then add them to the include directory of your project.

## Usage
```
#include <iostream>
#include "gitupdate.hpp"

using json = nlohmann::json;

int main(int argc, char** argv)
{
    // url to the github repository
    std::string url = "https://github.com/Scrappyz/gitupdate";

    // Gets latest release information
    json release_info = gitupdate::getLatestReleaseJson(url);

    // Updates the application. {asset_name} is whatever your application is called on the release page.
    gitupdate::updateApp(release_info, "{asset_name}");

    return 0;
}
```

All you need is the URL to your github repository. The latest release can then be acquired using the `getLatestReleaseJson()` function. To see how to parse the information on the JSON, refer to the [Github API](https://docs.github.com/en/rest/releases/releases?apiVersion=2022-11-28#get-the-latest-release). You can also update to a previous version by using `getReleaseJson()` instead then passing the resulting JSON to `updateApp()`. 