#include "transaction.hpp"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::vector<Transaction> transactions {};

HttpResponse createTransaction(const HttpRequest &req) {

}


HttpResponse getTransaction(const HttpRequest &req) {

    HttpResponse res{};

    json array = json::array();

    for (auto& t : transactions) {
        json obj;
        obj["id"] = t.id;
        obj["description"] = t.description;
        obj["amount"] = t.amount;
        array.push_back(obj);
    }
    res.status_code=200;
    res.status_text="OK";
    res.body = array.dump();
    res.headers["Content-Type"] = "application/json";
    res.headers["Content-Length"] = std::to_string(res.body.size());
    return res;   
}


HttpResponse deleteTransaction(const HttpRequest &req) {

}

HttpResponse serveFile(const HttpRequest &req) {
    HttpResponse res{};
    std::string filePath{};
    if (req.resource == "/") {
        filePath = "./public/index.html";
    }
    else {
        filePath = "./public" + req.resource;
    }

    if (!std::filesystem::exists(filePath)) {
        res.status_code = 404;
        res.status_text = "Not Found";
        res.headers["Date"] = getTimestamp();
        res.headers["Content-Type"] = "text/html";
        res.body = "<html>\n<head>\n<title>\n404 NOT FOUND\n</title>\n</head>\n<body>\n<p>This resource does Not Exist</p>\n</body>\n</html>";
        res.headers["Content-Length"] = std::to_string(res.body.length());
        return res;
    }

    std::ifstream file(filePath);
    if (!file.is_open()) {
        res.status_code = 403;
        res.status_text = "Forbidden";
        res.headers["Date"] = getTimestamp();
        res.headers["Content-Type"] = "text/html";
        res.body = "<html>\n<head>\n<title>\n403 Forbidden\n</title>\n</head>\n<body>\n<p>Do not Have Permission to access this resource</p>\n</body>\n</html>";
        res.headers["Content-Length"] = std::to_string(res.body.length());
        return res;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    res.status_code = 200;
    res.status_text = "OK";
    res.body = content;
    res.headers["Date"] = getTimestamp();
    auto mime = mimeTypes.find(std::filesystem::path(filePath).extension());
    res.headers["Content-Type"] = (mime != mimeTypes.end()) ? mime->second : "application/octet-stream";
    res.headers["Content-Length"] = std::to_string(content.length());
    return res;
}