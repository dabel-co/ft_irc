
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
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <functional>
#include <cerrno>

class Command;

class Server {
    private:
        const std::string       port;
        const std::string       pw;
        const std::string       host;
        bool                    running;
        int                     server_socket;
        int                     epfd;
        std::map<int, Client*>  s_clients;
        // std::map<std::string, std::function<void(const std::string&)>>  command_handlers;
        std::map<std::string, Command*> s_commands;

    public:

        Server(const std::string& port, const std::string& pw);
        ~Server();

        std::string     getPw() const;
        void            init_commands();
        void            run();
        void            add_to_epoll(int fd, uint32_t events);
        void            handle_events();
        void            client_connect();
        void            client_disconnect(int fd);
        void            client_message(int fd);
        std::string     extract_command(const std::string& msg);
};

#endif //SERVER_H
