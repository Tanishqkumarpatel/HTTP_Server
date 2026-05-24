#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <vector>
#include "http.hpp"

struct Transaction {
    int id;
    std::string description;
    double amount;
};

extern std::vector<Transaction> transactions;

HttpResponse createTransaction(const HttpRequest& req);
HttpResponse getTransaction(const HttpRequest& req);
HttpResponse deleteTransaction(const HttpRequest& req);
HttpResponse serveFile(const HttpRequest& req);

#endif