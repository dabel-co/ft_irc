
#ifndef CHANNEL_H
#define CHANNEL_H

#include <iostream>
#include "Client.hpp"
#include <vector>
#include <map>

class Channel {
    private:
        std::string               name_;
        std::string               password_;
        unsigned long             maxClients_;
        std::map<Client *, bool>  clients_; //the bool is true if the Client is an operator

    public:
        Channel(std::string name, std::string password);
        ~Channel();

        void            AddClient(Client *client, std::string password);
        void            EraseClient(Client *client);

        std::string     GetPassword() const { return password_; };
        void            SetPassword(const std::string& password) {this->password_ = password_; };

        void            SetMaxClients(const unsigned long maxClients) {this->maxClients_ = maxClients; };
        bool            CheckPermission(Client *client) { return clients_[client]; };
};

#endif //CHANNEL_H
