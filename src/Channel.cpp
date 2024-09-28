
#include "../inc/Channel.hpp"
#include "../inc/Command.hpp"

Channel::Channel(std::string name, std::string password) : name_(name), password_(password), maxClients_(0) {
  std::cout << "Channel created!" << std::endl;
}

Channel::~Channel(){
  std::cout << "Channel destroyed!" << std::endl;
}

void	Channel::Broadcast(std::string message, Client *src){
	for (std::map<Client *, bool>::iterator it = clients_.begin(); it != clients_.end(); it++) {
        if (it->first != src)
          it->first->Write(message);
    }
}
void Channel::AddClient(Client *client, std::string password) {
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
    for (std::map<Client *, bool>::iterator it = clients_.begin(); it != clients_.end(); it++) {
        client_list.append(it->first->GetNickname() + " ");
    }
    client->SetChannel(this);
    client->Reply(RPL_NAMREPLY(client->GetNickname(), this->name_, client_list));
    client->Reply(RPL_ENDOFNAMES(client->GetNickname(), this->name_));
}

void Channel::EraseClient(Client *client) {
    clients_.erase(client);
    if (clients_.empty())
        std::cout << "Delete this channel" << std::endl;
}
