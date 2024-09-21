
#ifndef COMMAND_H
#define COMMAND_H

#include "Server.hpp"
#include <iostream>
#define RPL_WELCOME(who)		        "001 " + who + " :Welcome " + who + " to the ft_irc network"
#define RPL_QUIT(who, message)		    ":" + who + " QUIT :Quit: " + message
#define ERR_ALREADYREGISTRED(who)       "462 " + who + " :You may not reregister"
#define ERR_NEEDMOREPARAMS(who, what)   "461 " + who + " " + what + " :Not enough parameters"
#define ERR_PASSWDMISMATCH(who)         "464 " + who + " :Password incorrect"
#define ERR_NONICKNAMEGIVEN(who)        "431 " + who + " :No nickname given"
#define ERR_NICKNAMEINUSE(who)		    "433 " + who + " :Nickname is already in use"

class Command{
    protected:
        Server *server_;

    public:
        explicit Command(Server *server) : server_(server){};
        virtual ~Command(){};


        virtual void Execute(Client *client, std::vector<std::string> tokens) = 0;
};

class PingCommand : public Command{
    public:
        PingCommand(Server *server_) : Command(server_) {}
        ~PingCommand() {}

        void Execute(Client *client, std::vector<std::string> tokens);
};

class PongCommand : public Command{
    public:
        PongCommand(Server *server_) : Command(server_) {}
        ~PongCommand() {}

        void Execute(Client *client, std::vector<std::string> tokens);
};

class CapCommand : public Command{
    public:
        CapCommand(Server *server_) : Command(server_) {}
        ~CapCommand() {}

        void Execute(Client *client, std::vector<std::string> tokens){}
};

class PassCommand : public Command{
    public:
        PassCommand(Server *server_) : Command(server_) {}
        ~PassCommand() {}

        void Execute(Client *client, std::vector<std::string> tokens);
};

class NickCommand : public Command{
    public:
        NickCommand(Server *server_) : Command(server_) {}
        ~NickCommand() {}

        void Execute(Client *client, std::vector<std::string> tokens);
};

class UserCommand : public Command{
    public:
        UserCommand(Server *server_) : Command(server_) {}
        ~UserCommand() {}

        void Execute(Client *client, std::vector<std::string> tokens);
};

class QuitCommand : public Command{
    public:
        QuitCommand(Server *server_) : Command(server_) {}
        ~QuitCommand() {}

        void Execute(Client *client, std::vector<std::string> tokens);
};

#endif //COMMAND_H
