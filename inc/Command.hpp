
#ifndef COMMAND_H
#define COMMAND_H

#include "Server.hpp"

#define ERR_ALREADYREGISTRED(who)                   ("462 " + who + " :You may not reregister")
#define ERR_NEEDMOREPARAMS(who, what)               ("461 " + who + " " + what + " :Not enough parameters")
#define ERR_PASSWDMISMATCH(who)                     ("464 " + who + " :Password incorrect")
#define ERR_NONICKNAMEGIVEN(who)                    ("431 " + who + " :No nickname given")
#define ERR_NICKNAMEINUSE(who)		                ("433 " + who + " :Nickname is already in use")
#define ERR_TOOMANYCHANNELS(who, channel)           ("405 " + who + " " + channel + " :You have joined too many channels")
#define ERR_CHANNELISFULL(who, channel)	            ("471 " + who + " " + channel + " :Cannot join channel (+l)")
#define ERR_BADCHANNELKEY(who, channel)	            ("475 " + who + " " + channel + " :Cannot join channel (+k)")
#define ERR_NOSUCHCHANNEL(who, channel)	            ("403 " + who + " " + channel + " :No such channel")
#define ERR_NOSUCHNICK(who, nickname)               ("401 " + who + " " + nickname + " :No such nick/channel")
#define RPL_NAMREPLY(who, channel, clients)	        ("353 " + who + " = " + channel + " :" + clients)
#define RPL_ENDOFNAMES(who, channel)                ("366 " + who + " " + channel + " :End of /NAMES list.")
#define ERR_CHANOPRIVSNEEDED(who, channel)          ("482 " + who + " " + channel + " :You're not channel operator")
#define ERR_NOTONCHANNEL(who, channel)			    ("442 " + who + " " + channel + " :You're not on that channel")
#define ERR_UNKNOWNMODE(who, what)					("472 " + who + " :" + what)
#define ERR_USERONCHANNEL(who, channel)             ("443" + who + " " + channel + " :is already on channel")
#define ERR_INVITEONLYCHAN(channel)                 ("473 " + channel + " :Cannot join channel (+i)")
#define RPL_JOIN(who, channel)					    (":" + who + " JOIN :" + channel)
#define RPL_PRIVMSG(who, dst, message)		        (":" + who + " PRIVMSG " + dst + " :" + message)
#define RPL_PING(who, token)			            (":" + who + " PONG :" + token)
#define RPL_KICK(who, channel, target, reason)	    (":" + who + " KICK " + channel + " " + target + message)
#define RPL_PART(who, channel, reason)				(":" + who + " PART " + channel + reason)
#define RPL_QUIT(who, message)		                (":" + who + " QUIT " + message)
#define RPL_NOTOPIC(channel)		                ("331 " + channel + " :No topic is set")
#define RPL_TOPIC(channel, topic)		            ("332 " + channel + " :" + topic)
#define RPL_WELCOME(who)		                    ("001 " + who + " :Welcome " + who + " to the ft_irc network")
#define RPL_INVITING(channel, who)                  ("341 " + channel + " " + who)

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
        explicit PingCommand(Server *server_) : Command(server_) {}
        ~PingCommand() {}

        void Execute(Client *client, std::vector<std::string> tokens);
};

class CapCommand : public Command{
    public:
        explicit CapCommand(Server *server_) : Command(server_) {}
        ~CapCommand() {}

        void Execute(Client *client, std::vector<std::string> tokens);
};

class PassCommand : public Command{
    public:
        explicit PassCommand(Server *server_) : Command(server_) {}
        ~PassCommand() {}

        void Execute(Client *client, std::vector<std::string> tokens);
};

class NickCommand : public Command{
    public:
        explicit NickCommand(Server *server_) : Command(server_) {}
        ~NickCommand() {}

        void Execute(Client *client, std::vector<std::string> tokens);
};

class UserCommand : public Command{
    public:
        explicit UserCommand(Server *server_) : Command(server_) {}
        ~UserCommand() {}

        void Execute(Client *client, std::vector<std::string> tokens);
};

class QuitCommand : public Command{
    public:
        explicit QuitCommand(Server *server_) : Command(server_) {}
        ~QuitCommand() {}

        void Execute(Client *client, std::vector<std::string> tokens);
};

class JoinCommand : public Command{
    public:
        explicit JoinCommand(Server *server_) : Command(server_) {}
        ~JoinCommand() {}

        void Execute(Client *client, std::vector<std::string> tokens);
};

class MsgCommand : public Command{
    public:
        explicit MsgCommand(Server *server_) : Command(server_) {}
        ~MsgCommand() {}

        void Execute(Client *client, std::vector<std::string> tokens);
};

class KickCommand : public Command{
    public:
        explicit KickCommand(Server *server_) : Command(server_) {};
        ~KickCommand() {}

        void Execute(Client *client, std::vector<std::string> tokens);
};

class ModeCommand : public Command{
    public:
        explicit ModeCommand(Server *server_) : Command(server_) {};
        ~ModeCommand() {}

        void Execute(Client *client, std::vector<std::string> tokens);
};

class PartCommand : public Command{
    public:
        explicit PartCommand(Server *server_) : Command(server_) {};
        ~PartCommand() {}

        void Execute(Client *client, std::vector<std::string> tokens);
};

class TopicCommand : public Command{
public:
    explicit TopicCommand(Server *server_) : Command(server_) {};
    ~TopicCommand() {}

    void Execute(Client *client, std::vector<std::string> tokens);
};

class InviteCommand : public Command{
public:
    explicit InviteCommand(Server *server_) : Command(server_) {};
    ~InviteCommand() {}

    void Execute(Client *client, std::vector<std::string> tokens);
};

#endif //COMMAND_H
