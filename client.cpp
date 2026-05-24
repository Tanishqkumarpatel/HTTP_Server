#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include "http.hpp"

int main() {
    int client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_fd < 0) {
        std::cerr << "Failed to create the client Socket" << std::endl;
        return 1;
    }

    sockaddr_in client_addr = {};
    client_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(8080);
    socklen_t client_len = sizeof(client_addr);


    if (connect(client_fd, (sockaddr *)&client_addr, client_len) < 0) {
        std::cerr << "Failed to connet to server!" << std::endl;
        return 1;
    }

    std::cout << "Connection Succesful!\n";


    
    std::string message{"GET /index.html HTTP/1.1\r\nHost: localhost:8080\r\nContent-Length: 2\r\n\r\nHi"};
    
    std::cout << message << "\n";
    
    if (send(client_fd, message.c_str(), message.size(), 0) < 0) {
        std::cerr << "Could Not send the last message: " << message << "\n";
    }

    char buffer[1024] {};
    if (recv(client_fd, buffer, sizeof(buffer), 0) < 0) {
        std::cerr << "Did not get the response from server.\n";
    }

    std::cout << buffer << "\n";

    

    return 0;
}