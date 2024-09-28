
#include "../inc/Server.hpp"
#include "../inc/Command.hpp"
#include "../inc/Channel.hpp"


Server::Server(const std::string& port, const std::string& password) : port_(port), password_(password), host_("127.0.0.1"), running_(true) {
    // Init socket
    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_ == -1)
        throw std::runtime_error("Error: Failed to create socket.");

    // Set socket options
    int val = 1;
    if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == -1)
        throw std::runtime_error("Error: Failed to set socket options.");

    // Convert port to uint16_t
    uint16_t hostport;
    std::stringstream ss(port_);
    if (!(ss >> hostport) || hostport == 0)
        throw std::runtime_error("Error: Invalid port: must be between 0 and 65535.");

    // Socket configuration
    sockaddr_in socket_conf = {};
    memset(&socket_conf, 0, sizeof(socket_conf));
    socket_conf.sin_family = AF_INET; // IPv4
    socket_conf.sin_port = htons(hostport); // Translate to network byte order
    socket_conf.sin_addr.s_addr = INADDR_ANY; // Bind to all interfaces

    // Set socket to non blocking
    if (fcntl(server_socket_, F_SETFL, O_NONBLOCK) == -1)
        throw std::runtime_error("Error: Failed to set socket to non blocking.");

    // Bind socket to port
    if (bind(server_socket_, (sockaddr *)&socket_conf, sizeof(socket_conf)) == -1)
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
    // Aquí hay que añadir el command handler, que es un mapa de commandos que parten de un mismo objeto
    // y del cual se extrae y ejecuta el comando que se recibe en el mensaje
}

Server::~Server(){
    // Close server socket
    if (server_socket_ != -1) {
        close(server_socket_);
    }

    // Close epoll instance
    if (epfd_ != -1) {
        close(epfd_);
    }

    // Delete all clients
    for (std::map<int, Client*>::iterator it = clients_.begin(); it != clients_.end(); ++it) {
        delete it->second;
    }
    // Clear clients map
    clients_.clear();
}


void Server::InitCommands(){
    commands_["PING"] =     new PingCommand(this);
    commands_["CAP"] =      new CapCommand(this);
    commands_["PASS"] =     new PassCommand(this);
    commands_["NICK"] =     new NickCommand(this);
    commands_["USER"] =     new UserCommand(this);
    commands_["QUIT"] =     new QuitCommand(this);
    commands_["JOIN"] =     new JoinCommand(this);
    commands_["PRIVMSG"] =  new MsgCommand(this);
    commands_["KICK"] =     new KickCommand(this);
	commands_["MODE"] = 	new ModeCommand(this);
    commands_["PART"] = 	new PartCommand(this);
}

void Server::Run() {
    while (running_)
        HandleEvents();
}

void Server::AddEpoll(int fd, uint32_t events) const {
    epoll_event ev = {};
    ev.events = events;
    ev.data.fd = fd;

    // Add file descriptor to epoll
    if (epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) == -1)
        throw std::runtime_error("Error: failed to add file descriptor to epoll.");
}

void Server::HandleEvents() {
    // Wait for events
    epoll_event ev_fd[MAX_EVENTS];
    int num_events = epoll_wait(epfd_, ev_fd, MAX_EVENTS, -1);
    if (num_events == -1) 
        throw std::runtime_error("Error: Error while waiting on epoll.");

    // Process events
    for (int i = 0; i < num_events; ++i) {
        int fd = ev_fd[i].data.fd;
        uint32_t events = ev_fd[i].events;

        if (events & (EPOLLRDHUP | EPOLLHUP))
            ClientDisconnect(fd);
        else if (fd == server_socket_ && (events & EPOLLIN))
            ClientConnect();
        else if (events & EPOLLIN)
            ClientMessage(fd);
    }
}

void Server::ClientConnect() {
    //std::cout << "Debug: Server.client_connect: New client connection!" << std::endl;

    // Accept new connection
    sockaddr_in client_addr = {};
    socklen_t s_size = sizeof(client_addr);
    int fd = accept(server_socket_, (sockaddr*)&client_addr, &s_size);
    if (fd == -1)
        throw std::runtime_error("Error: Error accepting new client.");

    // Set socket to non blocking
    fcntl(fd, F_SETFL, O_NONBLOCK);

    // Add client to epoll
    AddEpoll(fd, EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP);

    // Add client to map
    clients_[fd] = new Client(fd);

    std::cout << "Debug: Server.client_connect: Number of Clients = " << clients_.size() << "\n";

}

void Server::ClientDisconnect(int fd) {
    //std::cout << "Debug: Server.client_disconnect: Client disconnected!" << std::endl;

    // Remove client from channel
    if (clients_[fd]->GetChannel())
        clients_[fd]->GetChannel()->EraseClient(clients_[fd]);

    // Remove client from epoll
    if (epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, NULL) == -1)
        throw std::runtime_error("Error: Error removing client from epoll.");

    // Delete client object
    delete clients_[fd];

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
            std::cout << "JWIERGJWOIERJGIWER THIS IS NOT GOOD" << std::endl;
            ClientDisconnect(fd);
            return;
        }
        message << buffer;
    }
    //std::cout << "Debug: new message from " << fd << ":" << message.str() << std::endl;
    //std::cout << "wtf = " << message << std::endl;
    Client *current_client = clients_.at(fd);
    std::string aux;

    while (std::getline(message, aux)) {
        std::string cmd = aux.substr(0, aux.find(' '));
        try {
            std::cout << "command is = " << cmd << std::endl;
            Command *command = commands_.at(cmd);
            std::vector<std::string> tokens;
            std::string buf;
            std::stringstream ss(aux.substr(cmd.length(), aux.length()));
            while (ss >> buf)
                tokens.push_back(buf);
            std::cout << "command is = " << cmd << " ";
            for (long unsigned int i = 0; i < tokens.size(); i++) {
                std::cout << tokens[i] << " ";
            }
            std::cout << std::endl;
            command->Execute(current_client, tokens);
        }
        catch(const std::runtime_error & e) {
            std::cout << "Error: " << e.what() << std::endl;
            break;
        }
        catch(const std::out_of_range & e) {
            std::cout << "Error: command not found " << e.what() << std::endl;
        }
        //Client *aux_client = s_clients.at(fd);
    }
}

Client*	Server::FindClient(std::string &nick){
	for (std::map<int, Client *>::iterator it = clients_.begin(); it != clients_.end(); it++) {
		if (!nick.compare(it->second->GetNickname())) // change to =
			return it->second;
	}
	return (NULL);
}

Channel* Server::FindChannel(std::string &name) {
    try {
        Channel* aux = channels_.at(name);
        return aux;
    }
    catch(const std::out_of_range & e) {
        return NULL;
    }
}

Channel* Server::CreateChannel(std::string &name, std::string &password) {
    channels_[name] = new Channel(name, password);
    return channels_.at(name);
}
