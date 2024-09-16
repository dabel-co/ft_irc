#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <unistd.h>

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

        int     get_fd() const;
        std::string get_nickname() const;
        std::string get_username() const;
        bool    is_authenticated() const;
        void    authenticate();
        void    set_nick(std::string nick);
        void    set_username(std::string username);

        void reply(const std::string &reply);
        void write(const std::string &message) const;
        std::string getPrefix() const;
};

#endif //CLIENT_H