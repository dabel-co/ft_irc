
#include "../inc/Command.hpp"
#include "../inc/Channel.hpp"

// Ping
void PingCommand::Execute(Client *client, const std::vector<std::string> tokens){
    if (tokens.empty()) {
        std::cout << "PongCommand::Execute: missing arguments" << std::endl;
        return;
    }
    client->Write(RPL_PING(client->GetPrefix(), tokens[0]));
}

// Password
void PassCommand::Execute(Client *client, const std::vector<std::string> tokens){
    if (client->IsAuth())
        client->Reply(ERR_ALREADYREGISTRED(client->GetNickname()));
    else if (tokens.size() != 1)
        client->Reply(ERR_NEEDMOREPARAMS(client->GetNickname(), "PASS"));
    else if (tokens[0] == server_->getPw())
        client->Authenticate();
    else {
        client->Reply(ERR_PASSWDMISMATCH(client->GetNickname()));
        server_->ClientDisconnect(client->GetFd());
        throw(std::runtime_error("bad pass was given"));
    }
}
// Cap
void CapCommand::Execute(Client *client, std::vector<std::string> tokens) {
    //client->Write("CAP * LS :");
   if (client && !tokens.empty())
       tokens[0].empty();
}

//Nick
void NickCommand::Execute(Client *client, const std::vector<std::string> tokens){
    if(!client->IsAuth()){
        client->Reply("464 :Password incorrect");
        server_->ClientDisconnect(client->GetFd());
        throw(std::runtime_error("Tried to give NICK without being authenticated"));
    }
    if (tokens.size() != 1) {
        client->Reply(ERR_NEEDMOREPARAMS(client->GetNickname(), "NICK"));
        return ;
    }
    if (tokens[0].empty()) {
        client->Reply("431 :No nickname given");
        throw(std::runtime_error("Nickname must be given"));
    }

    // Enforce nickname length constraint
    if (tokens[0].length() > 9) {
        client->Reply("432 " + tokens[0] + " :Nick is too long");
        throw(std::runtime_error("Nickname is too long"));
    }

    // Define allowed special characters
    const std::string special_chars = "-[]\\`^{}";

    // Check if the first character is valid
    const char first_char = tokens[0][0];
    if (!isalpha(first_char) && special_chars.find(first_char) == std::string::npos) {
        client->Reply("432 " + tokens[0] + " :Erroneous nickname");
        throw(std::runtime_error("Invalid first character in nickname"));
    }

    // Check remaining characters
    for (size_t i = 1; i < tokens[0].length(); ++i) {
        const char c = tokens[0][i];
        if (!isalpha(c) && !isdigit(c) && special_chars.find(c) == std::string::npos) {
            client->Reply("432 " + tokens[0] + " :Erroneous nickname");
            throw(std::runtime_error("Invalid character in nickname"));
        }
    }

    // Check if the nickname is already in use
    if (server_->FindClient(tokens[0]) && client->GetNickname() != tokens[0]){
        client->Reply("433 " + tokens[0] + " :Nickname is already in use");
        server_->ClientDisconnect(client->GetFd());
        throw(std::runtime_error("Tried to change nickname to one in use"));
    }

    if (!client->GetNickname().empty()){
        client->Write(":" + client->GetPrefix() + " NICK " + tokens[0]);
        if (client->GetChannel())
            client->GetChannel()->Broadcast(":" + client->GetPrefix() + " NICK " + tokens[0], client);
    }
    client->SetNickname(tokens[0]);
}


// User
void UserCommand::Execute(Client *client, const std::vector<std::string> tokens){
    if(client->GetNickname().empty()){
    	client->Reply(ERR_NONICKNAMEGIVEN(client->GetNickname()));
    	server_->ClientDisconnect(client->GetFd());
	 	throw(std::runtime_error("Tried to give USER without a NICK in use"));
    }
    if (!client->GetUsername().empty())
		client->Reply(ERR_ALREADYREGISTRED(client->GetNickname()));
    else if (tokens.empty()) {
        client->Reply(ERR_NEEDMOREPARAMS(client->GetNickname(), "USER"));
    }
    else if (!tokens[0].empty()){
    	client->SetUsername(tokens[0]);
    	client->Reply(RPL_WELCOME(client->GetNickname()));
    }
}

// Quit
void QuitCommand::Execute(Client *client, const std::vector<std::string> tokens){
    std::string message;
    std::cout << "tokens size = " << tokens.size() << std::endl;
    for (unsigned long i = 0; i < tokens.size(); i++)
            message.append(" " + tokens[i]);
    if (tokens.empty())
        message = " Leaving";
    if (client->GetChannel()) {
        Channel *aux = client->GetChannel();
        aux->EraseClient(client, "QUIT", message);
    }
    server_->ClientDisconnect(client->GetFd());
}

// Join
void JoinCommand::Execute(Client *client, const std::vector<std::string> tokens){
    if (client->GetUsername().empty())
        return;
    if (tokens.empty()) {
        client->Reply("461 " + client->GetNickname() + " JOIN :Not enough parameters");
        return;
    }

    const std::string& name = tokens[0];
    // Enforce channel name length constraint
    if (name.length() > 200) {
        client->Reply("476 " + client->GetNickname() + " " + name + " :Bad Channel Mask");
        return;
    }
    // Check if the channel name starts with '#'
    if (name[0] != '#') {
        client->Reply("476 " + client->GetNickname() + " " + name + " :Bad Channel Mask");
        return;
    }

    // Check for invalid characters in the channel name
    for (size_t i = 0; i < name.length(); ++i) {
        char c = name[i];
        if (c == ' ' || c == ',' || c == 7) { // ASCII 7 is BEL (^G)
            client->Reply("476 " + client->GetNickname() + " " + name + " :Bad Channel Mask");
            return;
        }
    }
    if (client->GetChannel() != NULL) {
        client->Reply("405 " + client->GetNickname() + " " + name + " :You have joined too many channels");
        return;
    }

    const std::string password = tokens.size() > 1 ? tokens[1] : "";
    Channel *aux = server_->FindChannel(name);
    if (aux == NULL) {
        aux = server_->CreateChannel(name, password);
    }
    aux->AddClient(client, password);
}

// Msg
void MsgCommand::Execute(Client *client, const std::vector<std::string> tokens) {
    if (tokens.size() < 2) {
        client->Reply(ERR_NEEDMOREPARAMS(client->GetNickname(), "PRIVMSG"));
        return ;
    }
    std::string message;
	for (unsigned long i = 1; i < tokens.size(); i++)
    	message.append(tokens[i] + " ");
    message = message.at(0) == ':' ? message.substr(1) : message;
    for (size_t i = 0; i < message.length(); ++i) {
        const char c = message[i];
        if (c == '\0' || c == '\r' || c == '\n') {
            client->Reply("416 " + client->GetNickname() + " :Invalid character in message");
            return;
        }
    }
    if (message.length() > 510) {
        client->Reply("417 " + client->GetNickname() + " :Message too long");
        return;
    }
    if (tokens[0].at(0) == '#') {
        Channel *aux = server_->FindChannel(tokens[0]);
        if (aux == NULL || client->GetChannel() == NULL) {
            client->Reply(ERR_NOSUCHCHANNEL(client->GetNickname(), tokens[0]));
            return;
        }
        aux->Broadcast(RPL_PRIVMSG(client->GetPrefix(), tokens[0], message), client);
        return;
    }
    Client *dst = server_->FindClient(tokens[0]);
    if (dst == NULL) {
        client->Reply(ERR_NOSUCHNICK(client->GetNickname(), tokens[0]));
        return ;
    }
    dst->Write(RPL_PRIVMSG(client->GetPrefix(), tokens[0], message));
}

// Kick
void KickCommand::Execute(Client *client, const std::vector<std::string> tokens) {
    if (tokens.size() < 2) {
        client->Reply(ERR_NEEDMOREPARAMS(client->GetNickname(), "KICK"));
        return ;
    }
    Channel *channel = server_->FindChannel(tokens[0]);
    if (channel == NULL) {
        client->Reply(ERR_NOSUCHCHANNEL(client->GetNickname(), tokens[0]));
        return ;
    }
    if (client->GetChannel() != channel) {
        client->Reply(ERR_NOTONCHANNEL(client->GetNickname(), tokens[0]));
        return ;
    }
    if (channel->CheckPermission(client) == false) {
        client->Reply(ERR_CHANOPRIVSNEEDED(client->GetNickname(), tokens[0]));
        return ;
    }
    Client *dst = server_->FindClient(tokens[1]);
    if (dst == NULL) {
        client->Reply(ERR_NOSUCHNICK(client->GetNickname(), tokens[1]));
        return ;
    }
    if (dst->GetChannel() != channel) {
        dst->Reply(ERR_NOTONCHANNEL(dst->GetNickname(), tokens[0]));
        return ;
    }
    std::string message;
    for (unsigned long i = 2; i < tokens.size(); i++)
        message.append(" " + tokens[i]);
    channel->EraseClient(dst, client->GetPrefix(), message);
}

// Part
void PartCommand::Execute(Client *client, std::vector<std::string> tokens) {
    if (tokens.empty()) {
        client->Reply(ERR_NEEDMOREPARAMS(client->GetNickname(), "PART"));
        return ;
    }
    Channel *channel = server_->FindChannel(tokens[0]);
    if (channel == NULL) {
        client->Reply(ERR_NOSUCHCHANNEL(client->GetNickname(), tokens[0]));
        return ;
    }
    if (client->GetChannel() != channel) {          
        client->Reply(ERR_NOTONCHANNEL(client->GetNickname(), tokens[0]));
        return ;
    }
    std::string message;
    for (unsigned long i = 1; i < tokens.size(); i++)
        message.append(" " + tokens[i]);
    //std::cout << "Part message is = " << message << std::endl;
    channel->EraseClient(client, "PART", message);
}

void ModeCommand::Execute(Client *client, std::vector<std::string> tokens) {
    if (tokens.size() < 2 || tokens[1].size() < 2) {
        client->Reply(ERR_NEEDMOREPARAMS(client->GetNickname(), "MODE"));
        return;
    }
    if (tokens[0].at(0) != '#') {
      return ;
    }
    if (client->GetChannel() == NULL) {
        client->Reply(ERR_NOSUCHCHANNEL(client->GetNickname(), tokens[0]));
        return ;
    }

    Channel *aux = server_->FindChannel(tokens[0]);
    if (aux == NULL) {
        client->Reply(ERR_NOSUCHCHANNEL(client->GetNickname(), tokens[0]));
        return ;
    }

    if (aux != client->GetChannel()) {
        client->Reply(ERR_NOTONCHANNEL(client->GetNickname(), tokens[0]));
        return ;
    }

    if (aux->CheckPermission(client) == false) {
        client->Reply(ERR_CHANOPRIVSNEEDED(client->GetNickname(), tokens[0]));
        return ;
    }
    char op = tokens[1].at(0);
    char flag = tokens[1].at(1);
    Client *dst = NULL;
    //std::cout << "Token size = " << tokens.size() << std::endl;
    switch (flag) {
        case 'i': // invite only
            op == '+' ? aux->SetInvite(true) : aux->SetInvite(false);
            break;
        case 't': //topic can change only if operator
            op == '+' ? aux->SetTopicRestriction(true) : aux->SetTopicRestriction(false);
            break;
        case 'k': // password
            if (op == '+' && (tokens.size() != 3 || tokens[2].empty())) {
                client->Reply(ERR_NEEDMOREPARAMS(client->GetNickname(), "MODE"));
                return ;
            }
            op == '+' ? aux->SetPassword(tokens[2]) : aux->SetPassword("");
            break;
        case 'o' ://give operator
            if (tokens.size() != 3 || tokens[2].empty()) {
                client->Reply(ERR_NEEDMOREPARAMS(client->GetNickname(), "MODE"));
                return ;
            }
            dst = server_->FindClient(tokens[2]);
            if (dst == NULL || dst->GetChannel() != aux) {
                client->Reply(ERR_NOTONCHANNEL(tokens[2], aux->GetName())); //changies
                return;
            }
            op == '+' ? aux->SetOperator(dst, true) : aux->SetOperator(dst, false);
            break;
        case 'l' : { //user limit
            if (op == '-')
                aux->SetMaxClients(0);
            else if (op == '+' && (tokens.size() != 3 || tokens[2].empty()))
                client->Reply(ERR_NEEDMOREPARAMS(client->GetNickname(), "MODE"));
            else {
            int limit;
            std::stringstream ss(tokens[2]);
            ss >> limit;
            if (ss.fail() || !ss.eof() || (limit > 100) || limit < 0)
                client->Reply(ERR_UNKNOWNMODE(client->GetNickname(), "Invalid value"));
            else
                aux->SetMaxClients(limit);
            }
            break;
        }
        default:
            client->Reply(ERR_UNKNOWNMODE(client->GetNickname(), flag + " :Unknown flag"));
            break;
    }
}

//topic
void TopicCommand::Execute(Client *client, const std::vector<std::string> tokens) {
    if (tokens.size() < 1) {
        client->Reply(ERR_NEEDMOREPARAMS(client->GetNickname(), "TOPIC"));
        return ;
    }
    Channel *channel = server_->FindChannel(tokens[0]);
    if (channel == NULL)
        client->Reply(ERR_NOSUCHCHANNEL(client->GetNickname(), tokens[0]));
    else if (client->GetChannel() != channel)
        client->Reply(ERR_NOTONCHANNEL(client->GetNickname(), tokens[0]));
    else if (tokens.size() > 1 && channel->GetTopicRestriction() == true && channel->CheckPermission(client) == false)
        client->Reply(ERR_CHANOPRIVSNEEDED(client->GetNickname(), tokens[0]));
    else if (tokens.size() == 1) {
        // No new topic provided, return the current topic
        const std::string& currentTopic = channel->GetTopic();
        if (currentTopic.empty()) {
            client->Reply("331 " + client->GetNickname() + " " + channel->GetName() + " :No topic is set");
        } else {
            client->Reply("332 " + client->GetNickname() + " " + channel->GetName() + " :" + currentTopic);
        }
    }
    else {
        // A new topic is provided
        if (channel->GetTopicRestriction() && !channel->CheckPermission(client)) {
            client->Reply("482 " + client->GetNickname() + " " + channel->GetName() + " :You're not channel operator");
            return;
        }
        // Assemble the new topic from tokens[1] onwards
        std::string newTopic;
        for (size_t i = 1; i < tokens.size(); ++i) {
            if (i > 1)
                newTopic += " ";
            newTopic += tokens[i];
        }
        // Remove leading colon if present
        if (!newTopic.empty() && newTopic[0] == ':')
            newTopic = newTopic.substr(1);

        // Set the new topic
        channel->SetTopic(newTopic);
        // Broadcast the topic change to all channel members
        std::string topicMessage = ":" + client->GetPrefix() + " TOPIC " + channel->GetName() + " :" + newTopic;
        channel->Broadcast(topicMessage, NULL);
    }
}


void InviteCommand::Execute(Client *client, const std::vector<std::string> tokens) {
    if (tokens.size() != 2) {
        client->Reply(ERR_NEEDMOREPARAMS(client->GetNickname(), "TOPIC"));
        return ;
    }
    Channel *channel = server_->FindChannel(tokens[0]);
    Client *dst = server_->FindClient(tokens[1]);
    if (channel == NULL)
        client->Reply(ERR_NOSUCHCHANNEL(client->GetNickname(), tokens[0]));
    else if (client->GetChannel() != channel)
        client->Reply(ERR_NOTONCHANNEL(client->GetNickname(), tokens[0]));
    else if (channel->CheckPermission(client) == false)
        client->Reply(ERR_CHANOPRIVSNEEDED(client->GetNickname(), tokens[0]));
    else if (dst == NULL)
        client->Reply(ERR_NOSUCHNICK(client->GetNickname(), tokens[1]));
    else if (dst->GetChannel() == channel)
        dst->Reply(ERR_USERONCHANNEL(dst->GetNickname(), tokens[1]));
    else {
        channel->AddInvite(dst->GetNickname());
        //client->Reply(RPL_INVITING(channel->GetName(), dst->GetNickname()));
        dst->Reply("INVITE " + dst->GetNickname() + " :" + channel->GetName());
    }
}