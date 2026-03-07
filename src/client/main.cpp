#include <cstring>
#include <iostream>
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

    const char* msg = "Hello server";
    ssize_t bytes_sent = send(client_fd, msg, strlen(msg), 0);
    if (bytes_sent == -1) {
        std::cerr << "Send failed: " << strerror(errno) << "\n";
        close(client_fd);
        return 1;
    }

    std::cout << bytes_sent << " bytes sent\n";

    char buffer[1024];

    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        std::cout << bytes_received << " bytes received\n";
        buffer[bytes_received] = '\0';
        std::cout << "Received string: " << buffer << "\n";
    } else if (bytes_received == 0) {
        std::cout << "Server closed connection\n";
    } else {
        std::cerr << "Receive failed: " << strerror(errno) << "\n";
        close(client_fd);
        return 1;
    }

    close(client_fd);
}