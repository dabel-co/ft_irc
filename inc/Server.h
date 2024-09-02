//
// Created by dabel-co on 27/08/24.
// Coauthor: lvarela
//

#ifndef SERVER_H
#define SERVER_H
#define MAX_EVENTS 100
#include <string>
#include <sys/epoll.h>
#include <map>
#include "Client.h"

class Server {
    private:
        std::string port;
        std::string pw;
        std::string host;
        bool        running;
        int         server_socket;
        int         epfd;

    public:
        std::map<int, Client*> s_clients;

        Server(const std::string& port, const std::string& pw);
        ~Server();
        void run();
        void connect();
        void disconnect(int fd);
        void message(int fd);
};

#endif //SERVER_H
