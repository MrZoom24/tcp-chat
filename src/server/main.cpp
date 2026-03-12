#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>

struct ClientInfo {
    int fd;
    std::string username;
};

std::vector<ClientInfo> clients;
std::mutex clients_mutex;

int create_server_socket() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Socket creation failed: " << strerror(errno) << "\n";
        return -1;
    }

    sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(54000);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Socket bind failed: " << strerror(errno) << "\n";
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, 5) < 0) {
        std::cerr << "Socket listening failed: " << strerror(errno) << "\n";
        close(server_fd);
        return -1;
    }

    std::cout << "Server listening on port " << ntohs(server_addr.sin_port) << "\n";
    return server_fd;
}

void add_client(int client_fd, const std::string& username) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    clients.push_back({client_fd, username});
    std::cout << "Connected clients: " << clients.size() << std::endl;
}

void remove_client(int client_fd) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    clients.erase(std::remove_if(clients.begin(), clients.end(), [client_fd](const ClientInfo& client){
        return client.fd == client_fd;
    }), clients.end());
    std::cout << "Connected clients: " << clients.size() << std::endl;
}

void broadcast_message(const char* buffer, ssize_t message_size, int sender_fd) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (const auto& client : clients) {
        if (client.fd != sender_fd) {
            ssize_t bytes_sent = send(client.fd, buffer, message_size, 0);
            if (bytes_sent == -1) {
                std::cerr << "[SERVER fd=" << client.fd << "] Message failed to send: " << strerror(errno) << "\n";
            }
        }
    }
}

void handle_client(int client_fd) {
    char buffer[1024];

    while (true) {
        ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received == 0) {
            std::cout << "[SERVER fd=" << client_fd << "] Client closed connection\n";
            break;
        }

        if (bytes_received < 0) {
            std::cerr << "Receive failed: " << strerror(errno) << "\n";
            break;
        }
        buffer[bytes_received] = '\0';
        std::cout << "[SERVER fd=" << client_fd << "] Received: " << buffer << "\n";

        broadcast_message(buffer, bytes_received, client_fd);
    }

    remove_client(client_fd);
    close(client_fd);
}

int main() {
    int server_fd = create_server_socket();
    if (server_fd == -1) {
        return 1;
    }

    while (true) {
        sockaddr_in client_addr {};
        socklen_t client_size = sizeof(client_addr);

        int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_size);
        if (client_fd == -1) {
            std::cerr << "Accept failed: " << strerror(errno) << "\n";
            continue;
        }

        char buffer[1024];
        ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received == 0) {
            std::cerr << "Client disconnected before sending username\n";
            close(client_fd);
            continue;
        }

        if (bytes_received < 0) {
            std::cerr << "Receive failed: " << strerror(errno) << "\n";
            close(client_fd);
            continue;
        }

        buffer[bytes_received] = '\0';
        std::string intro = buffer;

        const std::string prefix = "/username ";
        if (intro.rfind(prefix, 0) != 0) {
            std::cout << "Invalid first message\n";
            close(client_fd);
            continue;
        }

        std::string username = intro.substr(prefix.size());

        if (!username.empty() && username.back() == '\n') {
            username.pop_back();
        }

        if (username.empty()) {
            std::cerr << "Empty username received\n";
            close(client_fd);
            continue;
        }

        std::cout << "Client \"" << username << "\" connected from: " << inet_ntoa(client_addr.sin_addr) << "\n";
        add_client(client_fd, username);

        std::thread client_thread(handle_client, client_fd);
        client_thread.detach();
    }

    close(server_fd);
}