
#include "../inc/Command.hpp"

#include "../inc/Channel.hpp"

void PingCommand::Execute(Client *client, std::vector<std::string> tokens){
    if (tokens.empty()) {
        std::cout << "PongCommand::Execute: missing arguments" << std::endl;
        return;
    }
    client->Write(RPL_PING(client->GetPrefix(), tokens[0]));
}

//Password
void PassCommand::Execute(Client *client, std::vector<std::string> tokens){
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
//Nick
void NickCommand::Execute(Client *client, std::vector<std::string> tokens){
    if(!client->IsAuth()){
      client->Reply(ERR_PASSWDMISMATCH(client->GetNickname())); //maybe reply something else
      server_->ClientDisconnect(client->GetFd());
      throw(std::runtime_error("Tried to give NICK without being authenticated"));
    }
    else if (tokens.empty() || (tokens.size() == 1 && tokens[0].empty()))
        client->Reply(ERR_NONICKNAMEGIVEN(client->GetNickname()));
    else if (server_->FindClient(tokens[0]) && client->GetNickname() != tokens[0]){
		client->Reply(ERR_NICKNAMEINUSE(client->GetNickname()));
		server_->ClientDisconnect(client->GetFd());
        throw(std::runtime_error("Username in use"));
	}
    else
		client->SetNickname(tokens[0]);
}
//User
void UserCommand::Execute(Client *client, std::vector<std::string> tokens){
    if(client->GetNickname().empty()){
    	client->Reply(ERR_NONICKNAMEGIVEN(client->GetNickname()));
    	server_->ClientDisconnect(client->GetFd());
	 	throw(std::runtime_error("Tried to give USER without a NICK in use"));
    }
    else if (!client->GetUsername().empty())
		client->Reply(ERR_ALREADYREGISTRED(client->GetNickname()));
    else if (!tokens[0].empty()){
    	client->SetUsername(tokens[0]);
    	client->Reply(RPL_WELCOME(client->GetNickname()));
    }
}

void QuitCommand::Execute(Client *client, std::vector<std::string> tokens){
    if (tokens.size() < 2){
		client->Reply(ERR_NEEDMOREPARAMS(client->GetNickname(), "KICK"));
        return ;
    }
    client->Write(RPL_QUIT(client->GetPrefix(), tokens[0]));
    server_->ClientDisconnect(client->GetFd());
}

void JoinCommand::Execute(Client *client, std::vector<std::string> tokens){
    if (tokens.empty()) {
        client->Reply(ERR_NEEDMOREPARAMS(client->GetNickname(), "JOIN"));
        return ;
    }
    if (client->GetChannel() != NULL) { //check it this works
        client->Reply(ERR_TOOMANYCHANNELS(client->GetNickname(), tokens[0]));
        return ;
    }
    std::string name = tokens[0];
    std::string password = tokens.size() > 1 ? tokens[1] : "";
    //server_->FindChannelChannel(name, password)->AddClient(client, password);
    Channel *aux = server_->FindChannel(name);
    if (aux == NULL) {
        aux = server_->CreateChannel(name, password);
    }
    aux->AddClient(client, password);
}

void MsgCommand::Execute(Client *client, std::vector<std::string> tokens) {
    if (tokens.size() < 2) {
        client->Reply(ERR_NEEDMOREPARAMS(client->GetNickname(), "PRIVMSG"));
        return ;
    }
    std::string message;
    for (unsigned long i = 1; i < tokens.size(); i++)
        message.append(tokens[i] + " ");
    if (tokens[0].at(0) == '#') {
        Channel *aux = server_->FindChannel(tokens[0]);
        if (aux == NULL) {
            client->Reply(ERR_NOSUCHCHANNEL(client->GetNickname(), tokens[0]));
            return;
        }
        std::cout << "broadcast" << std::endl;
        return;
    }
    Client *dst = server_->FindClient(tokens[0]);
    if (dst == NULL) {
        client->Reply(ERR_NOSUCHNICK(client->GetNickname(), tokens[0]));
        return ;
    }
    dst->Write(RPL_PRIVMSG(client->GetPrefix(), dst->GetNickname(), message));
}

void KickCommand::Execute(Client *client, std::vector<std::string> tokens) {
    if (tokens.size() < 2) {
        client->Reply(ERR_NEEDMOREPARAMS(client->GetNickname(), "KICK"));
        return ;
    }
    Channel *aux = server_->FindChannel(tokens[0]);
    if (aux == NULL) {
        client->Reply(ERR_NOSUCHCHANNEL(client->GetNickname(), tokens[0]));
        return ;
    }
    if (aux->CheckPermission(client) == false) {
        client->Reply(ERR_CLIHASNOPRIVSNEEDED(client->GetNickname(), tokens[0]));
    }
    Client *dst = server_->FindClient(tokens[1]);
    if (dst == NULL) {
        client->Reply(ERR_NOSUCHNICK(client->GetNickname(), tokens[1]));
        return ;
    }
    aux->EraseClient(dst);
}
