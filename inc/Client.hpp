
#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>

class Client {
private:
        int         fd;
        bool        password;
        bool        authenticated;
        std::string username;
        std::string nickname;

public:
        Client(int fd);
        ~Client();

        int             get_fd() const;
        std::string     get_nickname() const;
        std::string     get_username() const;
        void            set_nick(std::string nick);
        void            set_username(std::string username);

        void            authenticate();
        bool            is_authenticated() const;
        std::string     get_prefix() const;
        void            write(const std::string &message) const;
        void            send_prefixed_message(const std::string &reply);
};

#endif //CLIENT_H
