#include "router.hpp"
#include "http.hpp"

std::vector<Route> routes = {
    {"GET", std::regex("^/transactions$"), getTransactions},
    {"GET", std::regex("^/transactions/(\\d+)$"), getTransactionByID},
    {"HEAD", std::regex("^/transactions$"), headTransactions},
    {"HEAD", std::regex("^/transactions/(\\d+)$"), headTransactionByID},
    {"POST", std::regex("^/transactions$"), createTransaction},
    {"DELETE", std::regex("^/transactions/(\\d+)$"), deleteTransaction},
    {"PUT", std::regex("^/transactions/(\\d+)$"), editTransaction}
};


HttpResponse route(const HttpRequest& req) {
    HttpResponse res{};
    std::smatch matches{};
    for (int i = 0; i<int(routes.size()); i++) {
        if (req.method == routes[i].method && std::regex_match(req.resource, matches, routes[i].pattern)) {
            if (matches.size()>1) {
                req.params["id"] = matches[1].str();
            }
            return routes[i].handler(req);
        }
    }
    if (req.method == "GET") {
        return serveFile(req);
    }
    if (req.method == "HEAD") {
        HttpResponse res = serveFile(req);
        res.body="";
        return res;
    }
    res.status_code = 404;
    res.status_text = "Not Found";
    res.headers["Date"] = getTimestamp();
    res.headers["Content-Type"] = "text/html";
    res.body = "<html>\n<head>\n<title>\n404 NOT FOUND\n</title>\n</head>\n<body>\n<p>No route exists for the given resource</p>\n</body>\n</html>";
    res.headers["Content-Length"] = std::to_string(res.body.length());
    return res;
}