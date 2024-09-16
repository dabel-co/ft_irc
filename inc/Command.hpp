
#ifndef COMMAND_H
#define COMMAND_H

#include "Server.hpp"
#include <iostream>

#define ERR_ALREADYREGISTRED(who)"462 " + who + " :You may not reregister"
#define ERR_NEEDMOREPARAMS(who, what)	"461 " + who + " " + what + " :Not enough parameters"
#define ERR_PASSWDMISMATCH(who)"464 " + who + " :Password incorrect"
#define ERR_NONICKNAMEGIVEN(who)"431 " + who + " :No nickname given"

class Command{

    protected:
        Server *server;

    public:
        explicit Command(Server *server) : server(server){};
        virtual ~Command(){};


        virtual void execute(Client *client, std::vector<std::string> tokens) = 0;
};

class PingCommand : public Command{
    public:
        PingCommand(Server *server) : Command(server) {}
        ~PingCommand() {}

        void execute(Client *client, std::vector<std::string> tokens);
};

class PongCommand : public Command{
    public:
        PongCommand(Server *server) : Command(server) {}
        ~PongCommand() {}

        void execute(Client *client, std::vector<std::string> tokens);
};

class CapCommand : public Command{
    public:
        CapCommand(Server *server) : Command(server) {}
        ~CapCommand() {}

        void execute(Client *client, std::vector<std::string> tokens);
};

class PassCommand : public Command{
    public:
        PassCommand(Server *server) : Command(server) {}
        ~PassCommand() {}

        void execute(Client *client, std::vector<std::string> tokens);
};

class NickCommand : public Command{
    public:
        NickCommand(Server *server) : Command(server) {}
        ~NickCommand() {}

        void execute(Client *client, std::vector<std::string> tokens);
};

class UserCommand : public Command{
    public:
        UserCommand(Server *server) : Command(server) {}
        ~UserCommand() {}

        void execute(Client *client, std::vector<std::string> tokens);
};

class QuitCommand : public Command{
    public:
        QuitCommand(Server *server) : Command(server) {}
        ~QuitCommand() {}

        void execute(Client *client, std::vector<std::string> tokens);
};
#endif //COMMAND_H
