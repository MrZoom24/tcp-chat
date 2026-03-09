#include <atomic>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

void receive_messages(int client_fd, std::atomic<bool>& running) {
    while (running) {
        char buffer[1024];
        ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received == 0) {
            std::cout << "[CLIENT] Server closed connection.\n";
            running = false;
            break;
        }

        if (bytes_received < 0) {
            std::cerr << "[CLIENT] Error: " << strerror(errno) << "\n";
            running = false;
            break;
        }

        buffer[bytes_received] = '\0';
        std::cout << buffer << std::flush;
    }
}

int main() {
    std::atomic<bool> running = true;

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        std::cerr << "Socket creation failed: " << strerror(errno) << "\n";
        return 1;
    }

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

    std::string username;
    std::cout << "Enter your username: ";
    std::getline(std::cin, username);
    if (username.empty()) {
        std::cout << "Username cannot be empty\n";
        close(client_fd);
        return 1;
    }

    std::cout << "Connected to chat server.\n";

    std::thread receive_thread(receive_messages, client_fd, std::ref(running));

    std::string msg;
    while (running) {
        std::cout << "> " << std::flush;
        if (!std::getline(std::cin, msg)) {
            std::cout << "Input closed\n";
            running = false;
            shutdown(client_fd, SHUT_RDWR);
            break;
        }

        if (msg.empty()) {
            continue;
        }

        if (msg == "quit") {
            running = false;
            shutdown(client_fd, SHUT_RDWR);
            break;
        }

        std::string full_message = username + ": " + msg + "\n";
        ssize_t bytes_sent = send(client_fd, full_message.c_str(), full_message.size(), 0);
        if (bytes_sent == -1) {
            std::cerr << "[CLIENT] Send failed: " << strerror(errno) << "\n";
            running = false;
            shutdown(client_fd, SHUT_RDWR);
            receive_thread.join();
            close(client_fd);
            return 1;
        }
    }

    receive_thread.join();
    close(client_fd);
}