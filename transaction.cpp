#include "transaction.hpp"

#include <nlohmann/json.hpp>

using json = nlohmann::json;
std::mutex transactions_mutex{};
std::vector<Transaction> transactions {};

HttpResponse createTransaction(const HttpRequest &req) {
    HttpResponse res {};
    
    auto it = req.headers.find("Content-Type");
    if (it == req.headers.end() || it->second != "application/json") {
        // Malformed Header: missing or incorrect type.
        res.status_code = 415;
        res.status_text = "Unsupported Media Type";
        res.headers["Accept-Post"] = "application/json";
        return res;
    }

    json obj{};
    try {
        obj = json::parse(req.body);
    } catch (json::parse_error& e) {
        res.status_code = 400;
        res.status_text = "Bad Request";
        return res;
    }

    Transaction t {};
    
    if (!obj["description"].is_string() || !obj["amount"].is_number()) {
        res.status_code = 422;
        res.status_text = "Unprocessable Content";
        res.headers["Date"] = getTimestamp();
        return res;
    }
    std::lock_guard<std::mutex> lock(transactions_mutex);
    static int next_id = transactions.empty() ? 1 : transactions.back().id + 1;
    t.id = next_id++;
    t.description = obj["description"];
    t.amount = obj["amount"];
    transactions.push_back(t);
    // Using 204 because browers calls get after each create call.
    res.status_code = 204;
    res.status_text = "No Content";
    res.headers["Date"] = getTimestamp();
    return res;
}


HttpResponse getTransactions(const HttpRequest &req) {

    HttpResponse res{};

    json array = json::array();
    std::lock_guard<std::mutex> lock(transactions_mutex);
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
    HttpResponse res{};
    auto it = req.params.find("id");
    if (it == req.params.end()) {
        res.status_code = 400;
        res.status_text = "Bad Request";
        res.headers["Date"] = getTimestamp();
        return res;
    }
    int id = atoi(it->second.c_str());
    
    std::lock_guard<std::mutex> lock(transactions_mutex);
    auto t_it = transactions.begin();

    while (t_it != transactions.end()) {
        if (t_it->id == id) {
            transactions.erase(t_it);
            res.status_code = 200;
            res.status_text = "OK";
            res.headers["Content-Type"] = "application/json";
            json obj{};
            obj["message"] = "Deleted";
            res.body = obj.dump();
            res.headers["Content-Length"] = std::to_string(res.body.length());
            return res;
        }
        t_it++;
    }
    res.status_code = 404;
    res.status_text = "Not Found";
    res.headers["Date"] = getTimestamp();
    return res;
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