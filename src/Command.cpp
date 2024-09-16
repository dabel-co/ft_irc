
#include "../inc/Command.hpp"
void PongCommand::execute(Client *client, std::vector<std::string> tokens){
    // Este comando creo que no tenemos que soportarlo
}

void PingCommand::execute(Client *client, std::vector<std::string> tokens){
    std::cout << &client << "------ PING cmd ------" << std::endl;
    char aux [] = ": PONG :127.0.0.1\r\n";
    send(client->get_fd(), aux, strlen(aux), 0);
}

void CapCommand::execute(Client *client, std::vector<std::string> tokens){
    std::cout << &client << "------ CAP cmd ------" << std::endl;
    // Creo que este comando no ha de hacer nada así que por ahora lo dejamos así
}

void PassCommand::execute(Client *client, std::vector<std::string> tokens){
    if (client->is_authenticated())
        std::cout << "ERR_ALREADYREGISTRED" << std::endl;
    else if (tokens.size() != 1)
        std::cout << "ERR_NEEDMOREPARAMS" << std::endl;
    else if (tokens[0] == server->getPw())
        client->authenticate();
    else if (tokens[0] != server->getPw()){
        std::cout << "ERR_PASSWDMISMATCH" << std::endl;
        client->reply(ERR_PASSWDMISMATCH(client->get_nickname()));
        server->client_disconnect(client->get_fd());
        throw(std::runtime_error("bad pass was given"));
        }
}

void NickCommand::execute(Client *client, std::vector<std::string> tokens){
    if(!client->is_authenticated()){
      std::cout << "Password not given" << std::endl;
      server->client_disconnect(client->get_fd());
      throw(std::runtime_error("Nick not given"));
    }
    if (tokens.size() == 0)
        std::cout << "ERR_NEEDMOREPARAMS" << std::endl;
	else if (tokens.size() == 1 && tokens[0].length() == 0)
        std::cout << "ERR_NONICKNAMEGIVEN = " << tokens[0] << std::endl;
	else
		client->set_nick(tokens[0]);
}

void UserCommand::execute(Client *client, std::vector<std::string> tokens){
    if(client->get_nickname().empty()){
      std::cout << "No nickname" << std::endl;
      server->client_disconnect(client->get_fd());
    }
    if (tokens.size() != 4)
      std::cout << "ERR_NEEDMOREPARAMS" << std::endl;
    if (tokens[0].length() > 0)
      client->set_username(tokens[0]);
    char aux[] = ": 001 MyNickname :Welcome to the ExampleNet IRC Network MyNickname!MyUsername@127.0.0.1\r\n";
    //std::string auxx = "001 " + client->get_nickname() + " :Welcome to our IRC " + client->get_nickname() + "!" + client->get_username() + "@127.0.0.1\r\n";
    //send(client->get_fd(), auxx.c_str(), auxx.length(), 0);
    send(client->get_fd(), aux, strlen(aux), 0);
    //authorize client
    std::string auxx = "001 " + client->get_nickname() + " :Welcome " + client->get_nickname() + " to the ft_irc network";
    send(client->get_fd(), auxx.c_str(), auxx.length(), 0);
}

void QuitCommand::execute(Client *client, std::vector<std::string> tokens){
    std::cout << &client << "Quit!" << std::endl;
    // print the token that says reason to quit
    server->client_disconnect(client->get_fd());
    throw(std::runtime_error("Quit command"));
}