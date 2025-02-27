# ft_irc
A lightweight IRC (Internet Relay Chat) server implementation written in C++98, utilizing POSIX sockets and epoll for efficient asynchronous I/O. Supports standard IRC commands for multi-user communication and channel management.

Check the small article about the multiplexing choice [here](https://0x2a.cx/2024/12/20/hr-example.html)

## Features

- **Core IRC Commands**:  
  `CAP`, `PASS`, `NICK`, `USER`, `QUIT`, `JOIN`, `PRIVMSG`, `KICK`, `MODE`, `PART`, `TOPIC`, `INVITE`, `PING`.
- **Channel Management**:  
  Create/join channels, set passwords, manage topics, and moderate users.
- **Client Authentication**:  
  Requires password, nickname, and username for connection.
- **Efficient I/O Handling**:  
  Uses `epoll` for non-blocking socket operations and scalable event monitoring.
- **Multi-Client Support**:  
  Handles multiple simultaneous connections with asynchronous I/O.
- **Resource Safety**:  
  Proper cleanup of clients, channels, and commands on shutdown.

## Prerequisites

- Linux environment (uses `epoll` which is Linux-specific).
- C++98-compatible compiler (e.g, `g++`).
- `make` for building.
- IRC client (our main pick is [irssi](https://irssi.org/)) for testing.

## Installation

1. Clone the repository:  
   ```bash
   git clone https://github.com/dabel-co/ft_irc
   cd ft_irc
   ```
2. Build the project:  
   ```bash
   make
   ```
3. Run the server:  
   ```bash
   ./ft_irc <port> <password>
   ```
   Example:  
   ```bash
   ./ft_irc 6667 myultrasupersecretpassword
   ```

## Usage

1. **Connect to the Server**:  
   Use an IRC client or `telnet`:  
   ```bash
   telnet localhost 6667
   ```
2. **Authenticate**:  
   Send commands in order:  
   ```
   PASS myultrasupersecretpassword
   NICK your_nickname
   USER your_username 0 * :Your Real Name
   ```
3. **Join a Channel**:  
   ```
   JOIN #general
   ```
4. **Send Messages**:  
   ```
   PRIVMSG #general :Hello, world!
   PRIVMSG user2 :Private message here.
   ```

## Supported Commands

| Command    | Description                                | Example                          |
|------------|--------------------------------------------|----------------------------------|
| `JOIN`     | Join/create a channel                      | `JOIN #coding`                   |
| `PRIVMSG`  | Send message to channel/user               | `PRIVMSG #coding :Hi everyone!`  |
| `KICK`     | Remove a user from a channel               | `KICK #coding user1 :No spam`    |
| `MODE`     | Set channel modes (e.g, +o for operator)   | `MODE #coding +o user1`          |
| `TOPIC`    | Set or view channel topic                  | `TOPIC #coding :New topic`       |
| `INVITE`   | Invite a user to a channel                 | `INVITE user2 #coding`           |
| `PART`     | Leave a channel                            | `PART #coding`                   |
| `QUIT`     | Disconnect from the server                 | `QUIT :Goodbye!`                 |
