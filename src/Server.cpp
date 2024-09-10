//
// Created by dabel-co on 27/08/24.
// Coauthor: lvarela
//

#include "../inc/Server.hpp"

Server::Server(const std::string& port, const std::string& pw) : port(port), pw(pw), host("127.0.0.1"), running(true) {
    this->server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
        throw std::runtime_error("Debug: Server.server: Failed to create socket");

    int val = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == -1) // Set socket options
        throw std::runtime_error("Debug: Server.server: Failed to set socket options");

    uint16_t hostport;
    std::stringstream ss(port);
    if (!(ss >> hostport) || hostport == 0)
        throw std::runtime_error("Debug: Server.server: Invalid port: must be between 0 and 65535");

    sockaddr_in socket_conf = {};
    memset(&socket_conf, 0, sizeof(socket_conf));
    socket_conf.sin_family = AF_INET; // IPv4
    socket_conf.sin_port = htons(hostport); // Translate to network byte order
    socket_conf.sin_addr.s_addr = INADDR_ANY; // Bind to all interfaces

    if (fcntl(server_socket, F_SETFL, O_NONBLOCK) == -1) // Set socket to non blocking
        throw std::runtime_error("Debug: Server.server: Failed to set socket to non blocking");

    if (bind(server_socket, (sockaddr *)&socket_conf, sizeof(socket_conf)) == -1) // Bind socket to port
        throw std::runtime_error("Debug: Server.server: Failed to bind socket");

    if (listen(server_socket, 100) == -1) // Listen on socket
        throw std::runtime_error("Debug: Server.server: Failed to listen on socket");

    // Aquí hay que añadir el command handler, que es un mapa de commandos que parten de un mismo objeto
    // y del cual se extrae y ejecuta el comando que se recibe en el mensaje
}

Server::~Server(){
    if (server_socket != -1) {
        close(server_socket);
    }
    if (epfd != -1) {
        close(epfd);
    }

    // for (std::map<int, Client*>::iterator it = s_clients.begin(); it != s_clients.end(); ++it) {
    //     delete it->second;
    // }
    // s_clients.clear();
}
std::string Server::getPw() const {
    return pw;
}

void Server::run() {
    epfd = epoll_create1(0);
    if (epfd == -1)
        throw std::runtime_error("Debug: Server.run: Error creating epoll instance.");

    add_to_epoll(server_socket, EPOLLIN);

    while (running)
        handle_events();

    close(epfd);
}

void Server::add_to_epoll(int fd, uint32_t events) {
    epoll_event ev = {};
    ev.events = events;
    ev.data.fd = fd;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
        throw std::runtime_error("Debug: Server.run: Error adding file descriptor to epoll");
}

void Server::handle_events() {
    epoll_event ev_fd[MAX_EVENTS];
    int num_events = epoll_wait(epfd, ev_fd, MAX_EVENTS, -1);

    if (num_events == -1) 
        throw std::runtime_error("Debug: Server.handle_events: Error while waiting on epoll.");

    for (int i = 0; i < num_events; ++i) {
        int fd = ev_fd[i].data.fd;
        uint32_t events = ev_fd[i].events;

        if (events & (EPOLLRDHUP | EPOLLHUP))
            disconnect(fd);
        else if (fd == server_socket && (events & EPOLLIN))
            accept_new_connection();
        else if (events & EPOLLIN)
            handle_message(fd);
    }
}

void Server::accept_new_connection() {
    std::cout << "Debug: Server.accept_new_connection: New client connection!" << std::endl;

    sockaddr_in client_addr = {};
    socklen_t s_size = sizeof(client_addr);
    int fd = accept(server_socket, (sockaddr*)&client_addr, &s_size);

    if (fd == -1)
        throw std::runtime_error("Debug: Server.accept_new_connection: Error accepting new client.");

    add_to_epoll(fd, EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP);

    s_clients[fd] = new Client(fd);

    std::cout << "Debug: Server.accept_new_connection: Number of Clients = " << s_clients.size() << "\n";
}

void Server::disconnect(int fd) {
    std::cout << "Debug: Server.disconnect: Client disconnected!" << std::endl;

    if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL) == -1)
        throw std::runtime_error("Debug: Server.disconnect: Error removing client from epoll.");

    delete s_clients[fd]; // Delete client object
    s_clients.erase(fd); // Remove client from the map

    std::cout << "Debug: Server.disconnect: Number of Clients = " << s_clients.size() << "\n";
}

void Server::handle_message(int fd) {
    char buffer[1024] = {};
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);

    if (bytes_read == -1) {
        std::cerr << "Debug: Server.handle_message: Error reading from socket " << fd << std::endl;
        disconnect(fd);
        return;
    }

    buffer[bytes_read] = '\0';
    std::cout << "Debug: Server.handle_message: Message from client " << fd << ": \n" << buffer << std::endl;
}
