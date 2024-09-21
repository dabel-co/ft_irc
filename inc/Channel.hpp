//
// Created by dabel-co on 21/09/24.
//

#ifndef CHANNEL_H
#define CHANNEL_H

#include <iostream>
#include "Client.hpp"
#include <vector>
class Channel {
private:
    std::string name_;
    std::string password_;
    Client *operator_;
    std::vector<Client *> clients_;

public:
    Channel();
    ~Channel();
};



#endif //CHANNEL_H
