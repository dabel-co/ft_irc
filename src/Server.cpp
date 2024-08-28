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
#include <algorithm>
#include <cstdio>
#include <unistd.h>

Server::Server(const std::string& port, const std::string& pw) : port(port), pw(pw), status(1) {
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
    if (listen(server_socket, 100) == -1)   //100 is the max number of connections this IRC will handle
        throw std::runtime_error("Failed to listen on socket");
}
Server::~Server(){}

void Server::run() {
    pollfd pf = {server_socket, POLLIN, 0};
    this->fds.push_back(pf);
    while(this->status == 1) {
        if (poll(&fds[0], this->fds.size(), -1) < 0)
            throw std::runtime_error("Failed to poll");
        for (std::vector<pollfd>::iterator it = fds.begin(); it != fds.end(); ++it) {
          std::cout << "size = " << fds.size() << std::endl;
            if (it->revents == 0)
                continue;
            if ((it->revents & POLLIN) == POLLIN) {
                if (it->fd == server_socket) {
                    connect();
                    break;
                }
                char buffer[1024];
                int s = read(it->fd, buffer, 1024);
                if (s == 0){
                    fds.erase(it);
                    close(it->fd);
                    std::cout << "Debug : A client left!" << std::endl;
                    break;
                }
                message(it->fd, buffer);
            }
        }
    }

}

void Server::connect() {
    std::cout << "Debug : New client!" << std::endl;
    sockaddr_in client_addr = {};
    bzero(&client_addr, sizeof(client_addr));
    socklen_t s_size = sizeof(client_addr);
    int fd = accept(server_socket, (sockaddr *) &client_addr, &s_size);
    pollfd pf = {fd, POLLIN, 0};
    fds.push_back(pf);
}

void Server::message(int fd, char *buffer) {
  std::cout << fd <<  buffer << std::endl;
}



