
#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <unistd.h>

class Client {
    private:
        int         fd;
        bool        authenticated;
        std::string username;
        std::string nickname;

    public:
        Client(int fd);
        ~Client();

        int     get_fd() const;
        bool    is_authenticated() const;
        void    authenticate();
};

#endif //CLIENT_H
