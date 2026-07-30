#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <thread>
#include <arpa/inet.h>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <vector>
#include "http.hpp"
#include "router.hpp"

std::queue<int> client_queue{};
std::condition_variable cv;
std::mutex queue_mutex;
bool stop {true};



int startServer()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd < 0)
    {
        std::cerr << "error creating the socket" << std::endl;
        return -1;
    }

    int opt{1};
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in server_addr{};
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cerr << "error binding to the socket" << std::endl;
        return -1;
    }

    if (listen(server_fd, 10) < 0)
    {
        std::cerr << "error listening on the socket" << std::endl;
        return -1;
    }
    stop = false;
    return server_fd;
}

int getClient(int server_fd)
{
    struct sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0)
    {
        std::cerr << "error accepting the client" << std::endl;
        return -1;
    }

    std::cout << "client connected to socket succesfully! \n";
    return client_fd;
}

void handleClient(int client_fd)
{
    while (true)
    {
        if (stop) {
            break;
        }
        char buffer[8193]{};
        int bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes < 0)
        {
            std::cerr << "Error receiving message \n";
            break;
        }
        if (bytes == 0)
        {
            std::cerr << "Client Disconected \n";
            break;
        }

        buffer[bytes] = '\0';
        
        HttpRequest req = parse_request(buffer);
        auto error = isValidRequest(req);
        HttpResponse res{};
        if (error)
        {
            res = error.value();
        }
        else
        {
            res = route(req);
        }

        std::string http_res = build_response(res);
        std::cout << "Received:\n"
                  << buffer << "\n";
        std::cout << "Sending:\n"
                  << http_res << "\n";
        if (send(client_fd, http_res.c_str(), http_res.size(), 0) < 0)
        {
            std::cerr << "Error sending reply: " << http_res << "\n";
        }
    }
    close(client_fd);
    std::cout << "Client: " << client_fd << " Has Discoonected\n";
}

void workerThread() {
    while (!stop)
    {
            
        std::cout << "worker waiting\n";
        std::unique_lock<std::mutex> lock(queue_mutex);
        cv.wait(lock, [] { return !client_queue.empty(); });
    
        int client_fd = client_queue.front();
        client_queue.pop();
        lock.unlock();
        std::cout << "thread picking up fd: " << client_fd << "\n";
        handleClient(client_fd);
        std::cout << "thread done with fd: " << client_fd << "\n";
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cout << "Requres 2 argument exactly" << std::endl;
        return 1;
    }

    const int N = atoi(argv[1]);
    if (N <= 0) {
        std::cout << "Second argument must be a interger greater than 0." << std::endl;
        return 1;
    } 

    int server_fd = startServer();
    if (server_fd < 0)
    {
        return 1;
    }
    transactions = {
        {1, "Salary", 5000.00},
        {2, "Rent", -1200.00},
        {3, "Groceries", -150.50},
        {4, "Freelance", 800.00},
        {5, "Netflix", -15.99}
    };

    std::vector<std::thread> thread_pool{};

    for (int i = 0; i < N; i++) {
        std::thread worker(workerThread);
        thread_pool.push_back(std::move(worker));
    }


    while (true)
    {
        int client_fd = getClient(server_fd);
        if (client_fd < 0)
        {
            continue;
        }
        // client queue is shared resource - lock needed.
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            client_queue.push(client_fd);
        } // unlocks as its get out of scope.

        cv.notify_one();
        std::cout << "pushed fd: " << client_fd << " queue size: " << client_queue.size() << "\n";

        // std::thread t(handleClient, client_fd);
        // t.detach();
    }

    // never reaches this part of the code - above loop runs inf.
    stop = true;
    auto it = thread_pool.begin();
    while (it != thread_pool.end()) {
        it->join();
        it++;
    }
    close(server_fd);
    return 0;
}
