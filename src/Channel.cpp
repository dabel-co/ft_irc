
#include "../inc/Channel.hpp"
#include "../inc/Command.hpp"

Channel::Channel(const std::string& name, const std::string& password) : name_(name), password_(password), maxClients_(0), invite_(false), topic_restriction_(false) {
  std::cout << "Channel created!" << std::endl;
}

Channel::~Channel(){
    std::cout << "Channel destroyed!" << std::endl;
    clients_.clear();
    //server_->DestroyChannel(this->name_);
    //std::cout << "Channel destroyed!" << std::endl;
}

void	Channel::Broadcast(const std::string& message, const Client *src){
	for (std::map<Client *, bool>::iterator it = clients_.begin(); it != clients_.end(); ++it) {
        if (it->first != src || src == NULL)
          it->first->Write(message);
    }
}
void Channel::AddClient(Client *client, const std::string& password) {
    if (this->password_ != password) {
        client->Reply(ERR_BADCHANNELKEY(client->GetNickname(), this->name_));
        return;
    }
    if (maxClients_ == clients_.size() && maxClients_ != 0) {
        client->Reply(ERR_CHANNELISFULL(client->GetNickname(), this->name_));
        return;
    }
    clients_.empty() ? clients_[client] = true : clients_[client] = false;
    std::string client_list;
    for (std::map<Client *, bool>::iterator it = clients_.begin(); it != clients_.end(); ++it) {
        client_list.append(" " + it->first->GetNickname());
    }
    client_list.erase(0,1);
    client->SetChannel(this);
    client->Reply("JOIN :" + this->name_);
    client->Reply(RPL_NOTOPIC(this->name_));
    client->Reply(RPL_NAMREPLY(client->GetNickname(), this->name_, client_list));
    client->Reply(RPL_ENDOFNAMES(client->GetNickname(), this->name_));
    Broadcast(RPL_JOIN(client->GetPrefix(), this->name_), client);
}

void Channel::EraseClient(Client *client, std::string reason, std::string message) {
    if (reason == "QUIT")
        Broadcast(RPL_QUIT(client->GetPrefix(), message), NULL);
    else if (reason == "PART")
        Broadcast(RPL_PART(client->GetPrefix(), this->name_, message), NULL);
    //else if (reason == "KICK")
    client->SetChannel(NULL);
    clients_.erase(client);
    if (clients_.empty()) {
        this->password_ = "";
        this->maxClients_ = 0;
        this->topic_ = "";
        this->invite_ = false;
        this->topic_restriction_ = false;
    }
        //server_->DestroyChannel(this->name_);
}
