
#ifndef CHANNEL_H
#define CHANNEL_H

#include <iostream>
#include "Client.hpp"
#include "Server.hpp"
#include <map>

class Channel {
    private:
        std::string               name_;
        std::string               password_;
        unsigned long             maxClients_;
        std::map<Client *, bool>  clients_; //the bool is true if the Client is an operator
        std::string               topic_;
        bool                      invite_;
        bool                      topic_restriction_;
        //Server              *server_;

    public:
        Channel(const std::string& name, const std::string& password);
        ~Channel();

		void			Broadcast(const std::string& message, const Client *src);
        void            AddClient(Client *client, const std::string& password);
        void            EraseClient(Client *client, std::string reason, std::string message);

        std::string     GetPassword() const { return password_; };
        void            SetPassword(const std::string& password) {this->password_ = password; };

        std::string     GetName() const { return name_; };
        void            SetMaxClients(const unsigned long maxClients) {this->maxClients_ = maxClients; };
        bool            CheckPermission(Client *client) { return clients_[client]; };

        void            SetInvite(const bool invite) { this->invite_ = invite; };
        void            SetTopicRestriction(bool restrict) { this->topic_restriction_ = restrict; };
        void            SetOperator(Client *dst, const bool mode) { clients_[dst] = mode; };
};

#endif //CHANNEL_H
