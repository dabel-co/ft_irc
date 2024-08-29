//
// Created by dabel-co on 29/08/24.
//

#ifndef CLIENT_H
#define CLIENT_H



class Client {
public:
    Client(int fd);
    ~Client();
private:
    int fd;
};



#endif //CLIENT_H
