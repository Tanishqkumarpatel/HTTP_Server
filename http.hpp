#ifndef HTTP_HPP
#define HTTP_HPP

#include <map>
#include <string>
#include <optional>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <filesystem>

const std::string methods[5] = {"GET", "POST", "PUT", "DELETE", "HEAD"};

extern const std::map<std::string, std::string> mimeTypes;

struct HttpRequest
{
    std::string method{};
    std::string resource{};
    std::string version{"HTTP/1.1"};
    std::map<std::string, std::string> headers{};
    mutable std::map<std::string, std::string> params{};
    std::string body{};
};

struct HttpResponse
{
    std::string version{"HTTP/1.1"};
    int status_code;
    std::string status_text;
    std::map<std::string, std::string> headers;
    std::string body;
};

std::string getTimestamp();
HttpRequest parse_request(const std::string &req);
std::string build_response(const HttpResponse &res);
std::optional<HttpResponse> isValidRequest(const HttpRequest &req);

#endif