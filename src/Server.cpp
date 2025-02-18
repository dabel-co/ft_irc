
#include <utility>

#include "../inc/Server.hpp"
#include "../inc/Command.hpp"
#include "../inc/Channel.hpp"

Server::Server(const std::string &port, const std::string &password) : port_(port), password_(password), host_("127.0.0.1"), running_(true) {
    // Init socket
    server_socket_ = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (server_socket_ == -1)
        throw std::runtime_error("Error: Failed to create socket.");

    // Set socket options
    int val = 1;
    if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == -1)
        throw std::runtime_error("Error: Failed to set socket options.");

    // Convert port to uint16_t
    uint16_t hostport;
    std::stringstream ss(port_);
    ss >> hostport;
    if (ss.fail() || !ss.eof())
        throw std::runtime_error("Error: Invalid port.");

    // Socket configuration
    sockaddr_in socket_conf {};
    memset(&socket_conf, 0, sizeof(socket_conf));
    socket_conf.sin_family = AF_INET; // IPv4
    socket_conf.sin_port = htons(hostport); // Translate to network byte order
    socket_conf.sin_addr.s_addr = INADDR_ANY; // Bind to all interfaces

    // Bind socket to port
    if (bind(server_socket_, reinterpret_cast<sockaddr *>(&socket_conf), sizeof(socket_conf)) == -1)
        throw std::runtime_error("Error: Failed to bind socket.");

    // Listen to incoming connections
    if (listen(server_socket_, 100) == -1)
        throw std::runtime_error("Error: Failed to listen on socket.");

    // Create epoll instance
    epfd_ = epoll_create1(0);
    if (epfd_ == -1)
        throw std::runtime_error("Error: Error creating epoll instance.");

    // Add server socket to epoll
    AddEpoll(server_socket_, EPOLLIN);

    InitCommands();
}

Server::~Server(){
    running_ = false;

    // Close server socket
    if (server_socket_ != -1)
        close(server_socket_);

    // Close epoll instance
    if (epfd_ != -1)
        close(epfd_);

    // Clear maps
    channels_.clear();
    clients_.clear();
    commands_.clear();
}


void Server::InitCommands(){
    commands_["PING"] =     std::make_shared<PingCommand>(this);
    commands_["CAP"] =      std::make_shared<CapCommand>(this);
    commands_["PASS"] =     std::make_shared<PassCommand>(this);
    commands_["NICK"] =     std::make_shared<NickCommand>(this);
    commands_["USER"] =     std::make_shared<UserCommand>(this);
    commands_["QUIT"] =     std::make_shared<QuitCommand>(this);
    commands_["JOIN"] =     std::make_shared<JoinCommand>(this);
    commands_["PRIVMSG"] =  std::make_shared<MsgCommand>(this);
    commands_["KICK"] =     std::make_shared<KickCommand>(this);
    commands_["MODE"] =     std::make_shared<ModeCommand>(this);
    commands_["PART"] =     std::make_shared<PartCommand>(this);
    commands_["TOPIC"] =    std::make_shared<TopicCommand>(this);
    commands_["INVITE"] =   std::make_shared<InviteCommand>(this);
}

void Server::Run() {
    while (running_)
        HandleEvents();
}

void Server::AddEpoll(const int fd, const uint32_t events) const {
    epoll_event ev = {};
    ev.events = events;
    ev.data.fd = fd;

    // Add file descriptor to epoll
    if (epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) == -1)
        throw std::runtime_error("Error: failed to add file descriptor to epoll.");
}

void Server::HandleEvents() {
    epoll_event ev_fd[MAX_EVENTS];
    const int num_events = epoll_wait(epfd_, ev_fd, MAX_EVENTS, -1);
    if (num_events == -1) 
        throw std::runtime_error("Error: Error while waiting on epoll.");
    if (num_events > 0) {
        for (int i = 0; i < num_events; ++i) {
            const int fd = ev_fd[i].data.fd;
            const uint32_t events = ev_fd[i].events;
            if (events & (EPOLLRDHUP | EPOLLHUP))
                ClientDisconnect(fd);
            else if (fd == server_socket_ && (events & EPOLLIN))
                ClientConnect();
            else if (events & EPOLLIN)
                ClientMessage(fd);
        }
    }
}

void Server::ClientConnect() {
    sockaddr_in client_addr = {};
    socklen_t s_size = sizeof(client_addr);
    const int fd = accept(server_socket_, reinterpret_cast<sockaddr *>(&client_addr), &s_size);
    if (fd == -1)
        throw std::runtime_error("Error: Error accepting new client.");

    // Set socket to non blocking
    fcntl(fd, F_SETFL, O_NONBLOCK);

    // Add client to epoll
    AddEpoll(fd, EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP);

    // Add client to map
    clients_[fd] = std::make_shared<Client> (fd);

    std::cout << "Debug: Server.client_connect: Number of Clients = " << clients_.size() << "\n";

}

void Server::ClientDisconnect(const int fd) {

    // Remove client from channel
    if (clients_[fd]->GetChannel())
        clients_[fd]->GetChannel()->EraseClient(clients_[fd], "QUIT", " : connection broken");

    // Remove client from epoll
    if (epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, nullptr) == -1)
        throw std::runtime_error("Error: Error removing client from epoll.");

    // Remove client from map
    clients_.erase(fd);

    std::cout << "Debug: Server.client_disconnect: Number of Clients = " << clients_.size() << "\n";
}


void Server::ClientMessage(int fd) {
    std::stringstream message;
    char buffer[100];
    bzero(buffer, 100);
    while (!std::strstr(buffer, "\r\n")) {
        bzero(buffer, 100);
        ssize_t bytes_received = recv(fd, buffer, 100, 0);
        if (bytes_received < 0) {
            if (errno != EWOULDBLOCK)
                throw std::runtime_error("Error: Error while reading buffer from client.");
        }
        if (bytes_received == 0) {
            std::cout << "Client disconnected (recv returned 0)" << std::endl;
            ClientDisconnect(fd);
            return;
        }
        message << buffer;
    }
    std::shared_ptr<Client> current_client = clients_.at(fd);
    std::string aux;

    while (std::getline(message, aux)) {
        std::string cmd = aux.substr(0, aux.find(' '));
        try {
            std::cout << "command is = " << cmd << std::endl; //!!!!!
            auto command = commands_.at(cmd);
            std::vector<std::string> tokens;
            std::string buf;
            std::stringstream ss(aux.substr(cmd.length(), aux.length()));
            while (ss >> buf)
                tokens.push_back(buf);

            command->Execute(current_client, tokens);
        }
        catch(const std::runtime_error & e) {
            std::cout << "Error: " << e.what() << std::endl;
            break;
        }
        catch(const std::out_of_range & e) {
            std::cout << "Error: command not found " << e.what() << std::endl;
        }
    }
}

std::shared_ptr<Client>	Server::FindClient(const std::string &nick) const {

	for (auto & client : clients_) {
		if (nick == client.second->GetNickname())
			return client.second;
	}
	return (nullptr);
}

std::shared_ptr<Channel> Server::FindChannel(const std::string &name) const {
    try {
        const std::shared_ptr<Channel> aux = channels_.at(name);
        return (aux);
    }
    catch(const std::out_of_range &) {
        return (nullptr);
    }
}

std::shared_ptr<Channel> Server::CreateChannel(const std::string &name, const std::string &password) {
    channels_[name] = std::make_shared<Channel>(name, password);
    return channels_.at(name);
}


