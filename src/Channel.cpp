
#include "../inc/Channel.hpp"
#include "../inc/Command.hpp"

Channel::Channel(const std::string& name, const std::string& password) : name_(name), password_(password), maxClients_(0), invite_(false), topic_restriction_(true) {
  //std::cout << "Channel created!" << std::endl;
}

Channel::~Channel(){
    //std::cout << "Channel destroyed!" << std::endl;
    //clients_.clear();
    //server_->DestroyChannel(this->name_);
    //std::cout << "Channel destroyed!" << std::endl;
}

void	Channel::Broadcast(const std::string& message, const std::shared_ptr<Client>& src) const {
	for (auto & client : clients_) {
        if (client.first != src || src == nullptr)
          client.first->Write(message);
    }
}
void Channel::AddClient(const std::shared_ptr<Client>& client, const std::string& password) {
    if (this->password_ != password) {
        client->Reply(ERR_BADCHANNELKEY(client->GetNickname(), this->name_));
        return;
    }
    if (maxClients_ == clients_.size() && maxClients_ != 0) {
        client->Reply(ERR_CHANNELISFULL(client->GetNickname(), this->name_));
        return;
    }
    if (invite_ == true && std::ranges::find(invite_list_, client->GetNickname()) == invite_list_.end()) {
        client->Reply(ERR_INVITEONLYCHAN(name_));
        return;
    }
    clients_.empty() ? clients_[client] = true : clients_[client] = false;
    std::string client_list;
    for (auto & client : clients_) {
        client_list.append(" " + client.first->GetNickname());
    }
    client_list.erase(0,1);
    client->SetChannel(shared_from_this());
    client->Reply("JOIN :" + this->name_);
    if (topic_.empty())
        client->Reply(RPL_NOTOPIC(this->name_));
    else
        client->Reply(RPL_TOPIC(this->name_, topic_));
    client->Reply(RPL_NAMREPLY(client->GetNickname(), this->name_, client_list));
    client->Reply(RPL_ENDOFNAMES(client->GetNickname(), this->name_));
    Broadcast(RPL_JOIN(client->GetPrefix(), this->name_), client);
}

void Channel::EraseClient(const std::shared_ptr<Client>& client, const std::string& reason, const std::string& message) {
    if (reason == "QUIT")
        Broadcast(RPL_QUIT(client->GetPrefix(), message), nullptr);
    else if (reason == "PART")
        Broadcast(RPL_PART(client->GetPrefix(), this->name_, message), nullptr);
    else
        Broadcast(RPL_KICK(reason, this->name_, client->GetNickname(), message), nullptr);
    clients_.erase(client);
    client->SetChannel(nullptr);
    if (clients_.empty()) {
        this->password_ = "";
        this->maxClients_ = 0;
        this->topic_ = "";
        this->invite_ = false;
        this->topic_restriction_ = false;
    }
        //server_->DestroyChannel(this->name_);
}