
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
   if (client)
       tokens[0].empty();
}

//Nick
void NickCommand::Execute(Client *client, const std::vector<std::string> tokens){
    if(!client->IsAuth()){
      client->Reply(ERR_PASSWDMISMATCH(client->GetNickname()));
      server_->ClientDisconnect(client->GetFd());
      throw(std::runtime_error("Tried to give NICK without being authenticated"));
    }
    if (tokens[0].empty()) {
        client->Reply(ERR_NONICKNAMEGIVEN(client->GetNickname()));
        throw(std::runtime_error("Nickname must be given"));
    }
    if (server_->FindClient(tokens[0]) && client->GetNickname() != tokens[0]){
		client->Reply(ERR_NICKNAMEINUSE(client->GetNickname()));
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
    //  message.erase(0,1);
    if (tokens[0].empty())
        message = " Leaving";
    if (client->GetChannel()) {
        Channel *aux = client->GetChannel();
        aux->EraseClient(client, "QUIT", message);
    }
    server_->ClientDisconnect(client->GetFd());
}

// Join
void JoinCommand::Execute(Client *client, std::vector<std::string> tokens){
    if (!client->IsAuth())
        return;
    if (tokens.empty()) {
        client->Reply(ERR_NEEDMOREPARAMS(client->GetNickname(), "JOIN"));
        return ;
    }
    if (client->GetChannel() != NULL) { //check it this works
            client->Reply(ERR_TOOMANYCHANNELS(client->GetNickname(), tokens[0]));
        return ;
    }
    const std::string& name = tokens[0];
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
        //std::cout << "lol" << std::endl;
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
    if (tokens.size() < 3) {
        client->Reply(ERR_NEEDMOREPARAMS(client->GetNickname(), "TOPIC"));
        return ;
    }
    Channel *channel = server_->FindChannel(tokens[0]);
    if (channel == NULL)
        client->Reply(ERR_NOSUCHCHANNEL(client->GetNickname(), tokens[0]));
    else if (client->GetChannel() != channel)
        client->Reply(ERR_NOTONCHANNEL(client->GetNickname(), tokens[0]));
    else if (channel->GetTopicRestriction() == true && channel->CheckPermission(client) == false)
        client->Reply(ERR_CHANOPRIVSNEEDED(client->GetNickname(), tokens[0]));
    else {
        std::string message;
        for (unsigned long i = 2; i < tokens.size(); i++)
            message.append(" " + tokens[i]);
        message = message.at(0) == ' ' ? message.substr(1) : message;
        message = message.at(0) == ':' ? message.substr(1) : message;
        channel->SetTopic(message);
        channel->Broadcast("TOPIC " + channel->GetName() + " :" + message, NULL);
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
        client->Reply(RPL_INVITING(channel->GetName(), dst->GetNickname()));
    }
}