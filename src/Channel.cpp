
#include "../inc/Channel.hpp"
#include "../inc/Command.hpp"

Channel::Channel(const std::string& name, const std::string& password) : name_(name), password_(password), maxClients_(0), invite_(false), topic_restriction_(true) {}

Channel::~Channel(){
    clients_.clear();
}

// Broadcast message to all clients in the channel
void    Channel::Broadcast(const std::string& message, const Client *src){
	for (std::map<Client *, bool>::iterator it = clients_.begin(); it != clients_.end(); ++it) {
        if (it->first != src || src == NULL)
          it->first->Write(message);
    }
}

// Add a client to the channel
void    Channel::AddClient(Client *client, const std::string& password) {
    // Check if the client is already in the channel
    if (this->password_ != password) {
        client->Reply(ERR_BADCHANNELKEY(client->GetNickname(), this->name_));
        return;
    }
    // Check if the channel is full
    if (maxClients_ == clients_.size() && maxClients_ != 0) {
        client->Reply(ERR_CHANNELISFULL(client->GetNickname(), this->name_));
        return;
    }
    // Check if the channel is invite only
    if (invite_ == true && std::find(invite_list_.begin(), invite_list_.end(), client->GetNickname()) == invite_list_.end()) {
        client->Reply(ERR_INVITEONLYCHAN(client->GetNickname(), this->name_));
        return;
    }

    // Check if the client is already in the channel
    clients_.empty() ? clients_[client] = true : clients_[client] = false;

    // Get the list of clients in the channel
    std::string client_list;
    for (std::map<Client *, bool>::iterator it = clients_.begin(); it != clients_.end(); ++it) {
        client_list.append(" " + it->first->GetNickname());
    }

    // Remove the leading space
    client_list.erase(0,1);

    // Set the channel for the client and send the welcome message
    client->SetChannel(this);
    client->Reply("JOIN :" + this->name_);

    // Comprobe if the channel has a topic and send it to the client
    if (topic_.empty())
        client->Reply(RPL_NOTOPIC(this->name_));
    else
        client->Reply("332 " + client->GetNickname() + " " + this->name_ + " :" + topic_);
    // Send the list of clients in the channel and the end of the list
    client->Reply(RPL_NAMREPLY(client->GetNickname(), this->name_, client_list));
    client->Reply(RPL_ENDOFNAMES(client->GetNickname(), this->name_));

    // Broadcast the join message to all clients in the channel
    Broadcast(RPL_JOIN(client->GetPrefix(), this->name_), client);
}

void    Channel::EraseClient(Client *client, std::string reason, std::string message) {
    // Broadcast the quit, part or kick message to all clients in the channel
    if (reason == "QUIT")
        Broadcast(RPL_QUIT(client->GetPrefix(), message), NULL);
    else if (reason == "PART")
        Broadcast(RPL_PART(client->GetPrefix(), this->name_, message), NULL);
    else
        Broadcast(RPL_KICK(reason, this->name_, client->GetNickname(), message), NULL);

    // Set the channel to NULL for the client and remove it from the channel
    client->SetChannel(NULL);
    clients_.erase(client);

    // If the client was the last one in the channel, reset the channel
    if (clients_.empty()) {
        this->password_ = "";
        this->maxClients_ = 0;
        this->topic_ = "";
        this->invite_ = false;
        this->topic_restriction_ = false;
        invite_list_.clear();
    }
}

void    Channel::SetOperator(Client *dst, const bool mode) {
    std::cout << "Client " << dst << " is now = " << mode << std::endl;
    clients_[dst] = mode;
};

void    Channel::AddInvite(std::string client) {
    invite_list_.push_back(client);
}
