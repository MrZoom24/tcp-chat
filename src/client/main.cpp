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
    } else {
        std::cout << "Socket creation succeeded\n";
    }

    sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(54000);
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address\n";
        return 1;
    }


    if (connect(client_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Socket connection failed: " << strerror(errno) << "\n";
        close(client_fd);
        return 1;
    } else {
        std::cout << "Socket connection succeeded\n";
        close(client_fd);
    }
}
