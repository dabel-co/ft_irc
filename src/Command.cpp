
#include "../inc/Command.hpp"
void PongCommand::execute(Client *client){
    char aux [] = "PONG :127.0.0.1\r\n";
    send(client->get_fd(), aux, strlen(aux), 0);
}

void PingCommand::execute(Client *client){
    std::cout << &client << "------ PING cmd ------" << std::endl; // Al igual que en PONG, habría que ir añadiendo la lógica de cada comando
}

void CatCommand::execute(Client *client){
    std::cout << &client << "------ CAT cmd ------" << std::endl;
}

void PassCommand::execute(Client *client){
    std::cout << &client << "------ PASS cmd ------" << std::endl;
}

void NickCommand::execute(Client *client){
    std::cout << &client << "------ NICK cmd ------" << std::endl;
}

void UserCommand::execute(Client *client){
    std::cout << &client << "------ USER cmd ------" << std::endl;
}
