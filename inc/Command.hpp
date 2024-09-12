//
// Created by dabel-co on 12/09/24.
//

#ifndef COMMAND_H
#define COMMAND_H

#include "Server.hpp"
#include <iostream>
class Command{

    protected:
        Server *server;
        bool _authRequired;

    public:
        explicit Command(Server *server, bool authRequired = true) : server(server), _authRequired(authRequired){};
        virtual ~Command(){};

        bool authRequired() const { return _authRequired; };

        virtual void execute(Client *client) = 0;
};

class PingCommand : public Command{
    public:
        PingCommand(Server *server) : Command(server) {}
        ~PingCommand() {}

        void execute(Client *client);
};

class PongCommand : public Command{
    public:
        PongCommand(Server *server) : Command(server) {}
        ~PongCommand() {}

        void execute(Client *client);
};
#endif //COMMAND_H
