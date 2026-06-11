#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <vector>
#include "http.hpp"
#include <mutex>

extern std::mutex transactions_mutex;

struct Transaction {
    int id;
    std::string description;
    double amount;
};

extern std::vector<Transaction> transactions;

HttpResponse createTransaction(const HttpRequest& req);
HttpResponse getTransactions(const HttpRequest& req);
HttpResponse deleteTransaction(const HttpRequest& req);
HttpResponse serveFile(const HttpRequest& req);
HttpResponse getTransactionByID(const HttpRequest& req);
HttpResponse editTransaction(const HttpRequest& req);

#endif