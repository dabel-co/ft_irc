
#ifndef CHANNEL_H
#define CHANNEL_H

#include <iostream>
#include <list>

#include <algorithm>
#include "Client.hpp"
#include "Server.hpp"
#include <map>

class Channel : public std::enable_shared_from_this<Channel> {
    private:
        std::string               name_;
        std::string               password_;
        long unsigned int         maxClients_;
        std::map<std::shared_ptr<Client> , bool>  clients_; //true = operator
        std::string               topic_;
        bool                      invite_;
        std::list<std::string>    invite_list_;
        bool                      topic_restriction_;


    public:
        Channel(const std::string& name, const std::string& password);
        ~Channel();

        void	    Broadcast(const std::string& message, const std::shared_ptr<Client>& src) const;
        void        AddClient(const std::shared_ptr<Client>& client, const std::string& password);
        void            EraseClient(const std::shared_ptr<Client>& client, const std::string& reason, const std::string& message);

        void            SetTopic(const std::string & topic) { topic_ = topic; };
        void            SetTopicRestriction(bool restrict) {this->topic_restriction_ = restrict; };
        bool            GetTopicRestriction() const { return topic_restriction_; };
        std::string     GetPassword() const { return password_; };
        void            SetPassword(const std::string& password) {this->password_ = password;};

        std::string     GetName() const { return name_; };
        void            SetMaxClients(const int maxClients) {this->maxClients_ = maxClients;};
        bool            CheckPermission(const std::shared_ptr<Client>& client) { return clients_[client]; };

        void            SetInvite(const bool invite) {this->invite_ = invite;};

        void            SetOperator(const std::shared_ptr<Client>& dst, const bool mode) {
            std::cout << "Client " << dst << " is now = " << mode << std::endl;
            clients_[dst] = mode; //hmmm....
        };
        void            AddInvite(const std::string& client) {
            invite_list_.push_back(client);
        }
};

#endif //CHANNEL_H
