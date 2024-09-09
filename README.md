# Updater
A header-only library to update your C++ applications using the Github API.

## Installation
### Requirements
- [cURL](https://curl.se/download.html): To make https requests.
- [json](https://github.com/nlohmann/json): To read and parse JSON files.

[cURL](https://curl.se/download.html) is required to be installed on the user's machine in order for the auto-update to work. This shouldn't be much of a problem since cURL is usually installed by default on Windows 10 and above. [Json](https://github.com/nlohmann/json) needs to be included in your project for `updater.hpp` to work. Thankfully, `nlohmann/json` is a header-only library so including it in your project should be straightforward.