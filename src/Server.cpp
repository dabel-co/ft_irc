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
#include <bits/algorithmfwd.h>
#include <algorithm>
#include <cstdio>
#include <unistd.h>

Server::Server(const std::string& port, const std::string& pw) : port(port), pw(pw), status(1) {
    this->host = "127.0.0.1"; //Default for running local
    this->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (!server_fd)
        throw std::runtime_error("Failed to create socket");
    int val = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == -1)
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
    if (fcntl(server_fd, F_SETFL, O_NONBLOCK) == -1)
        throw std::runtime_error("Failed to set socket to non blocking");
    if (bind(server_fd, (sockaddr *)&socket_conf, sizeof(socket_conf)) == -1)
        throw std::runtime_error("Failed to bind socket");
    if (listen(server_fd, 100) == -1)   //100 is the max number of connections this IRC will handle
        throw std::runtime_error("Failed to listen on socket");
}
Server::~Server(){}

void Server::run() {
    pollfd pf = {};
    pf.fd = this->server_fd;
    pf.events = POLLIN;
    pf.revents = 0;
    this->fds.push_back(pf);
    while(this->status == 1) {
        if (poll(&fds[0], this->fds.size(), -1) < 0)
            throw std::runtime_error("Failed to poll");
        for (std::vector<pollfd>::iterator it = fds.begin(); it != fds.end(); ++it) {
            if (it->revents == 0)
                continue;
            if ((it->revents & POLLHUP) == POLLHUP) {
                disconnect(it->fd);
                break;
            }
            if ((it->revents & POLLIN) == POLLIN) {

                if (it->fd == server_fd) {
                    connect();
                    break;
                }
                message(it->fd);
            }
        }
    }

}

void Server::connect() {
    std::cout << "New client!" << std::endl;
    sockaddr_in client_addr = {};
    //bzero(&client_addr, sizeof(client_addr));
    socklen_t s_size = sizeof(client_addr);
    int fd = accept(server_fd, (sockaddr *) &client_addr, &s_size);
    pollfd pf = {fd, POLLIN, 0};
    fds.push_back(pf);
}

void Server::disconnect(int fd) {
    std::cout << "A client left" << std::endl;
    for (std::vector<pollfd>::iterator it = fds.begin(); it != fds.end(); ++it) {
        if (it->fd == fd) {
            fds.erase(it);
            close(fd);
        }
    }
}

void Server::message(int fd) {
    std::cout << "There is a request" << std::endl;
    char buffer[1024];
    read(fd, buffer, 1024);
    printf("%s\n", buffer);
    sleep(1);
}


