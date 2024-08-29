//
// Created by dabel-co on 27/08/24.
//

#include "Server.h"
#include <sys/socket.h>
#include <stdexcept>
#include <netinet/in.h>
#include <string>
#include <sstream>
#include <strings.h>
#include <fcntl.h>
#include <iostream>
#include <poll.h>
#include <unistd.h>

Server::Server(const std::string& port, const std::string& pw) : port(port), pw(pw), running(true) {
    this->host = "127.0.0.1"; //Default for running local
    this->server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (!server_socket)
        throw std::runtime_error("Failed to create socket");
    int val = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == -1)
        throw std::runtime_error("Failed to set socket options");
    std::stringstream ss(port);
    int hostport;
    if (!(ss >> hostport) || hostport < 0 || hostport > 65536)
        throw std::runtime_error("Invalid port");
    sockaddr_in socket_conf = {};
    bzero(&socket_conf, sizeof(socket_conf));
    socket_conf.sin_family = AF_INET;
    socket_conf.sin_port = htons(hostport);
    socket_conf.sin_addr.s_addr = INADDR_ANY;
    if (fcntl(server_socket, F_SETFL, O_NONBLOCK) == -1)
        throw std::runtime_error("Failed to set socket to non blocking");
    if (bind(server_socket, (sockaddr *)&socket_conf, sizeof(socket_conf)) == -1)
        throw std::runtime_error("Failed to bind socket");
    if (listen(server_socket, 100) == -1)
        throw std::runtime_error("Failed to listen on socket");
}
Server::~Server(){}

void Server::run() {
    epfd = epoll_create1(0);
    if (epfd == -1)
        throw std::runtime_error("Error creating epoll instance.");
    epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = server_socket;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, server_socket, &ev) == -1)
        throw std::runtime_error("Error adding server socket to epoll.");
    while (running) {
        epoll_event ev_fd[MAX_EVENTS]; // Adjust MAX_EVENTS as needed
        int num_events = epoll_wait(epfd, ev_fd, MAX_EVENTS, -1);
        if (num_events == -1)
            throw std::runtime_error("Error while waiting on epoll.");
        for (int i = 0; i < num_events; ++i) {
            if (ev_fd[i].events & (EPOLLRDHUP | EPOLLHUP))
                disconnect(ev_fd[i].data.fd);
            else if ((ev_fd[i].data.fd == server_socket) & EPOLLIN)
                connect();
            else if (ev_fd[i].events & EPOLLIN)
                message(ev_fd[i].data.fd);
        }
    }
    close(epfd);
}

void Server::connect() const {
    std::cout << "Debug : New client!" << std::endl;
    sockaddr_in client_addr = {};
    bzero(&client_addr, sizeof(client_addr));
    socklen_t s_size = sizeof(client_addr);
    int fd = accept(server_socket, (sockaddr *) &client_addr, &s_size);

    epoll_event ev;
    ev.events = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP;
    ev.data.fd = fd;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
        throw std::runtime_error("Error adding client to epoll");
}

void Server::disconnect(int fd) const {
    std::cout << "Debug : Client left!" << std::endl;
    if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL) == -1)
        throw std::runtime_error("Error removing client from epoll");
}

void Server::message(int fd) {
    char buffer[1024] = {};
    read(fd, buffer, 1024);
    std::cout << "Message : " << buffer << std::endl;
}



