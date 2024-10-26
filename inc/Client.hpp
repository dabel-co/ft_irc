
#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>

class Channel;

class Client {
    private:
        int         fd_;
        bool        isAuth_;
        std::string username_;
        std::string nickname_;
        Channel     *channel_;

    public:
        explicit Client(const int Fd) : fd_(Fd), isAuth_(false), channel_(NULL) {}
        ~Client(){ close(fd_); };

        void            SetNickname(const std::string& nickname) {this->nickname_ = nickname; };
        void            SetUsername(const std::string& username) {this->username_ = username; };
        void            SetChannel(Channel *channel) { channel_ = channel; };
        std::string     GetNickname() const { return nickname_; };
        std::string     GetUsername() const { return username_; };
        Channel*        GetChannel() const {return channel_;};
        int             GetFd() const { return fd_; };

        void            Authenticate() { isAuth_ = true; };
        bool            IsAuth() const { return isAuth_; };
        std::string     GetPrefix() const;
        void            Write(const std::string &message) const;
        void            Reply(const std::string &reply) const;
};

#endif //CLIENT_H
