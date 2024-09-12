//
// Created by dabel-co on 12/09/24.
//

#include "../inc/Command.hpp"
void PongCommand::execute(Client *client){
    char aux [] = "PONG :127.0.0.1\r\n";
    send(client->get_fd(), aux, strlen(aux), 0);
}

void PingCommand::execute(Client *client){
    std::cout << &client << "TEST" << std::endl;
}
