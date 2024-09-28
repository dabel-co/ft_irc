
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
        bool                      invite;
        bool                      topic_restriction;

    public:
        Channel(std::string name, std::string password);
        ~Channel();

		void			Broadcast(std::string message, Client *src);
        void            AddClient(Client *client, std::string password);
        void            EraseClient(Client *client);

        std::string     GetPassword() const { return password_; };
        void            SetPassword(const std::string& password) {this->password_ = password; };

        std::string     GetName() const { return name_; };
        void            SetMaxClients(const unsigned long maxClients) {this->maxClients_ = maxClients; };
        bool            CheckPermission(Client *client) { return clients_[client]; };

        void            SetInvite(bool invite) { this->invite = invite; };
        void            SetTopicRestriction(bool restrict) { this->topic_restriction = restrict; };
        void            SetOperator(Client *dst, bool mode) { clients_[dst] = mode; };
};


#endif //CHANNEL_H
