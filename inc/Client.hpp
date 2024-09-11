
#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <unistd.h>

class Client {
    private:
        int fd;

    public:
        Client(int fd);
        ~Client();

        int get_fd() const;
};

#endif //CLIENT_H
