#include <cstring>
#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>


int main() {
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        std::cerr << "Socket creation failed: " << strerror(errno) << "\n";
        return 1;
    }


    std::cout << "Socket creation succeeded\n";

    sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(54000);
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        close(client_fd);
        return 1;
    }


    if (connect(client_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Socket connection failed: " << strerror(errno) << "\n";
        close(client_fd);
        return 1;
    }

    std::cout << "Socket connection succeeded\n";

    std::string msg;
    while (true) {
        std::cout << "> " << std::flush;
        if (!std::getline(std::cin, msg)) {
            std::cout << "Input closed\n";
            break;
        }

        if (msg.empty()) {
            continue;
        }

        if (msg == "quit") {
            break;
        }

        ssize_t bytes_sent = send(client_fd, msg.c_str(), msg.size(), 0);
        if (bytes_sent == -1) {
            std::cerr << "Send failed: " << strerror(errno) << "\n";
            close(client_fd);
            return 1;
        }

        char buffer[1024];
        ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            close(client_fd);

            if (bytes_received == 0) {
                std::cout << "Server closed connection\n";
                break;
            } else {
                std::cerr << "Receive failed: " << strerror(errno) << "\n";
                return 1;
            }
        }

        buffer[bytes_received] = '\0';
        std::cout << "Server message: " << buffer << "\n";
    }
    close(client_fd);
}