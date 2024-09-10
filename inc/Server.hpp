//
// Created by dabel-co on 27/08/24.
// Coauthor: lvarela
//

#ifndef SERVER_H
#define SERVER_H

#define MAX_EVENTS 100

#include "Client.hpp"
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

class Server {
    private:
        const std::string   port;
        const std::string   pw;
        const std::string   host;
        bool                running;
        int                 server_socket;
        int                 epfd;

    public:
        std::map<int, Client*> s_clients;

        Server(const std::string& port, const std::string& pw);
        ~Server();

        std::string getPw() const;

        void run();
        void add_to_epoll(int fd, uint32_t events);
        void handle_events();
        void accept_new_connection();
        void disconnect(int fd);
        void handle_message(int fd);
};

#endif //SERVER_H
