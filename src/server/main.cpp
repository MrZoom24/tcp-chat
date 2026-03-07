#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Socket creation failed: " << strerror(errno) << "\n";
        return 1;
    } else {
        std::cout << "Socket creation succeeded\n";
    }

    sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(54000);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Socket bind failed: " << strerror(errno) << "\n";
        close(server_fd);
        return 1;
    } else {
        std::cout << "Socket bind succeeded\n";
    }


    if (listen(server_fd, 5) < 0) {
        std::cerr << "Socket listening failed: " << strerror(errno) << "\n";
        close(server_fd);
        return 1;
    } else {
        std::cout << "Socket listening succeeded\n";
        std::cout << "Server listening on port " << ntohs(server_addr.sin_port) << "\n";
    }

    sockaddr_in client_addr {};
    socklen_t client_size = sizeof(client_addr);

    int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_size);
    if (client_fd == -1) {
        std::cerr << "Accept failed: " << strerror(errno) << "\n";
        close(server_fd);
        return 1;
    } else {
        std::cout << "Client Connected from: " << inet_ntoa(client_addr.sin_addr) << "\n";
        close(server_fd);
        close(client_fd);
    }
}