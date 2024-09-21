
#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>

class Client {
private:
        int         fd_;
        bool        isAuth_;
        std::string username_;
        std::string nickname_;

public:
        Client(int fd);
        ~Client();

        int             GetFd() const;
        std::string     GetNickname() const;
        void            SetNickname(std::string nickname);
        std::string     GetUsername() const;
        void            SetUsername(std::string username);

        void            Authenticate();
        bool            IsAuth() const;
        std::string     GetPrefix() const;
        void            Write(const std::string &message) const;
        void            Reply(const std::string &reply);
};

#endif //CLIENT_H
