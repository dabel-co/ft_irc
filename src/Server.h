//
// Created by dabel-co on 27/08/24.
//

#ifndef SERVER_H
#define SERVER_H
#include <string>
#include <vector>
#include <sys/poll.h>


class Server {
public:
    Server(const std::string& port, const std::string& pw);
    ~Server();
    void run();
    void connect();
    void disconnect(int fd);
    void message(int fd);
private:
    std::string port;
    std::string pw;
    std::string host;
    int         status; //0 = off, 1 = running
    int         server_fd;
    std::vector<pollfd> fds;

};



#endif //SERVER_H
