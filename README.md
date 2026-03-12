# TCP Chat Server (C++)

A simple multi client TCP chat application written in C++ using POSIX sockets.
Clients connect to a central server, choose a username, and exchange messages in real time.

---

# Features

* Multi client chat server
* TCP socket communication (POSIX sockets)
* Thread-per-client server architecture
* Username handshake protocol
* Join / leave notifications
* Thread safe shared state
* Client receive thread for asynchronous message handling
* Configurable host and port via command line arguments

---

# Architecture

## Server

* Listens on a TCP port
* Accepts incoming client connections
* Reads a username handshake
* Spawns a dedicated thread for each client
* Broadcasts messages to other connected clients
* Handles client disconnects

---

## Client

1. Connects to the server
2. Prompts the user for a username
3. Sends a username handshake
4. Starts a background thread to receive messages
5. Reads user input and sends chat messages

The client runs two logical flows simultaneously:

```
Main thread:
    read user input
    send messages

Receiver thread:
    receive server messages
    print them to terminal
```

---

# Build

This project uses CMake.

## Requirements

* C++17 compatible compiler
* CMake
* Linux / macOS (POSIX sockets)

## Build steps

```bash
git clone https://github.com/MrZoom24/tcp-chat
cd tcp-chat

mkdir build
cd build

cmake ..
make
```

This produces two executables:

```
server
client
```

---

# Running the Server

Default port:

```bash
./server
```

Custom port:

```bash
./server 7000
```

Example output:

```
Server listening on port 54000
Client "CoolGuy24" connected from: 127.0.0.1
```

---

# Running the Client

Default connection (localhost):

```bash
./client
```

Custom host:

```bash
./client 192.168.1.50
```

Custom host and port:

```bash
./client 192.168.1.50 7000
```

Example session:

```
Enter your username: CoolGuy24
Connected to chat server at 127.0.0.1:54000
> Hello everyone
```

---

# Chat Protocol

When a client connects, it sends a username handshake:

```
/username CoolGuy24
```

After this, all messages follow the format:

```
username: message
```

Example:

```
CoolGuy24: Hello
LameGuy42: Hi 
```

Server-generated events:

```
CoolGuy24 joined the chat
LameGuy42 left the chat
```

---

# Networking Concepts Demonstrated

I have created this project to demonstrate my knowledge in several fundamental networking concepts:

* IPv4 TCP sockets
* `socket()`, `bind()`, `listen()`, `accept()`
* `connect()`
* `send()` / `recv()`
* byte order conversion
* thread-per-connection server model
* synchronization using `std::mutex`
* simple application level protocol design

---

# Limitations

This project intentionally keeps the design simple.

Current limitations:

* IPv4 only
* no message framing protocol
* no encryption (plain TCP)
* no authentication
* minimal error recovery
* no UI beyond terminal
