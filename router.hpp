#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <map>
#include <string>
#include <functional>
#include "http.hpp"
#include "transaction.hpp"
#include <regex>


struct Route {
    std::string method;
    std::regex pattern;
    std::function<HttpResponse(const HttpRequest&)> handler;
};

extern std::vector<Route> routes;

HttpResponse route(const HttpRequest& req);

#endif