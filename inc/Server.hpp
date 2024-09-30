
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
#include <vector>

class Command;

class Server {
    private:
        const std::string               port_;
        const std::string               password_;
        const std::string               host_;
        bool                            running_;
        int                             server_socket_;
        int                             epfd_;
        std::map<int, Client*>          clients_;
        std::map<std::string, Command*> commands_;
        std::map<std::string, Channel *>channels_;

    public:
        Server(const std::string& port, const std::string& pw);
        ~Server();

        std::string     getPw() const { return password_;}
        void            InitCommands();
        void            Run();
        void            AddEpoll(int fd, uint32_t events) const;
        void            HandleEvents();
        void            ClientConnect();
        void            ClientDisconnect(int fd);
        void            ClientMessage(int fd);
		Client *        FindClient(const std::string &nick);
        Channel*        FindChannel(const std::string &name) const;
        Channel*        CreateChannel(const std::string &name, const std::string &password);
};

#endif //SERVER_H
