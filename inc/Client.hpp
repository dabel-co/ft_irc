//
// Created by dabel-co on 29/08/24.
//

#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>

class Client {
    private:
        int fd;

    public:
        Client(int fd);
        ~Client();

};

#endif //CLIENT_H
