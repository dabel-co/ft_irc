
#include "../inc/Command.hpp"


void PongCommand::Execute(Client *client, std::vector<std::string> tokens){
    // Este comando creo que no tenemos que soportarlo
}

void PingCommand::Execute(Client *client, std::vector<std::string> tokens){
    std::cout << &client << "------ PING cmd ------" << std::endl;
    char aux [] = ": PONG :127.0.0.1\r\n";
    send(client->GetFd(), aux, strlen(aux), 0);
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
	else if (tokens.size() == 0 || (tokens.size() == 1 && tokens[0].length() == 0))
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
    else if (tokens[0].length() > 0){
    	client->SetUsername(tokens[0]);
    	client->Reply(RPL_WELCOME(client->GetNickname()));
    }
}

void QuitCommand::Execute(Client *client, std::vector<std::string> tokens){
    server_->ClientDisconnect(client->GetFd());
}