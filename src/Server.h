//
// Created by dabel-co on 27/08/24.
//

#ifndef SERVER_H
#define SERVER_H
#define MAX_EVENTS 100
#include <string>
#include <vector>
#include <sys/poll.h>
#include <sys/epoll.h>

class Server {
public:
    Server(const std::string& port, const std::string& pw);
    ~Server();
    void run();
    void connect() const;
    void disconnect(int fd) const;
    void message(int fd);
private:
    std::string port;
    std::string pw;
    std::string host;
    bool        running; //0 = off, 1 = running
    int         server_socket;
    int epfd;

};
#endif //SERVER_H
