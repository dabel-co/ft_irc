
#include "../inc/Server.hpp"
#include "../inc/Command.hpp"

Server::Server(const std::string& port, const std::string& pw) : port(port), pw(pw), host("127.0.0.1"), running(true) {
    // Init socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
        throw std::runtime_error("Debug: Server.server: Failed to create socket");
    init_commands();
    // Set socket options
    int val = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == -1)
        throw std::runtime_error("Debug: Server.server: Failed to set socket options");

    // Convert port to uint16_t
    uint16_t hostport;
    std::stringstream ss(port);
    if (!(ss >> hostport) || hostport == 0)
        throw std::runtime_error("Debug: Server.server: Invalid port: must be between 0 and 65535");

    // Socket configuration
    sockaddr_in socket_conf = {};
    memset(&socket_conf, 0, sizeof(socket_conf));
    socket_conf.sin_family = AF_INET; // IPv4
    socket_conf.sin_port = htons(hostport); // Translate to network byte order
    socket_conf.sin_addr.s_addr = INADDR_ANY; // Bind to all interfaces

    // Set socket to non blocking
    if (fcntl(server_socket, F_SETFL, O_NONBLOCK) == -1)
        throw std::runtime_error("Debug: Server.server: Failed to set socket to non blocking");

    // Bind socket to port
    if (bind(server_socket, (sockaddr *)&socket_conf, sizeof(socket_conf)) == -1)
        throw std::runtime_error("Debug: Server.server: Failed to bind socket");

    // Listen to incoming connections
    if (listen(server_socket, 100) == -1)
        throw std::runtime_error("Debug: Server.server: Failed to listen on socket");

    // Create epoll instance
    epfd = epoll_create1(0);
    if (epfd == -1)
        throw std::runtime_error("Debug: Server.run: Error creating epoll instance.");

    // Add server socket to epoll
    add_to_epoll(server_socket, EPOLLIN);

    // Aquí hay que añadir el command handler, que es un mapa de commandos que parten de un mismo objeto
    // y del cual se extrae y ejecuta el comando que se recibe en el mensaje
}

Server::~Server(){
    // Close server socket
    if (server_socket != -1) {
        close(server_socket);
    }

    // Close epoll instance
    if (epfd != -1) {
        close(epfd);
    }

    // Delete all clients
    for (std::map<int, Client*>::iterator it = s_clients.begin(); it != s_clients.end(); ++it) {
        delete it->second;
    }
    // Clear clients map
    s_clients.clear();
}

std::string Server::getPw() const {
    return pw;
}

void Server::init_commands(){
    s_commands["PING"] = new PingCommand(this);
    s_commands["PONG"] = new PongCommand(this);
}

void Server::run() {
    while (running)
        handle_events();
}

void Server::add_to_epoll(int fd, uint32_t events) {
    epoll_event ev = {};
    ev.events = events;
    ev.data.fd = fd;

    // Add file descriptor to epoll
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
        throw std::runtime_error("Debug: Server.run: Error adding file descriptor to epoll");
}

void Server::handle_events() {
    // Wait for events
    epoll_event ev_fd[MAX_EVENTS];
    int num_events = epoll_wait(epfd, ev_fd, MAX_EVENTS, -1);
    if (num_events == -1) 
        throw std::runtime_error("Debug: Server.handle_events: Error while waiting on epoll.");

    // Process events
    for (int i = 0; i < num_events; ++i) {
        int fd = ev_fd[i].data.fd;
        uint32_t events = ev_fd[i].events;

        if (events & (EPOLLRDHUP | EPOLLHUP))
            client_disconnect(fd);
        else if (fd == server_socket && (events & EPOLLIN))
            client_connect();
        else if (events & EPOLLIN)
            client_message(fd);
    }
}

void Server::client_connect() {
    std::cout << "Debug: Server.client_connect: New client connection!" << std::endl;

    // Accept new connection
    sockaddr_in client_addr = {};
    socklen_t s_size = sizeof(client_addr);
    int fd = accept(server_socket, (sockaddr*)&client_addr, &s_size);
    if (fd == -1)
        throw std::runtime_error("Debug: Server.client_connect: Error accepting new client.");

    // Set socket to non blocking
    fcntl(fd, F_SETFL, O_NONBLOCK);

    // Add client to epoll
    add_to_epoll(fd, EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP);

    // Add client to map
    s_clients[fd] = new Client(fd);

    std::cout << "Debug: Server.client_connect: Number of Clients = " << s_clients.size() << "\n";
    char aux[] = "001 MyNickname :Welcome to the ExampleNet IRC Network MyNickname!MyUsername@127.0.0.1\r\n";
    send(fd, aux, strlen(aux), 0);
}

void Server::client_disconnect(int fd) {
    std::cout << "Debug: Server.client_disconnect: Client client_disconnected!" << std::endl;

    // Remove client from epoll
    if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL) == -1)
        throw std::runtime_error("Debug: Server.client_disconnect: Error removing client from epoll.");

    // Delete client object
    delete s_clients[fd];

    // Remove client from map
    s_clients.erase(fd);

    std::cout << "Debug: Server.client_disconnect: Number of Clients = " << s_clients.size() << "\n";
}

void Server::client_message(int fd) {
    /*char buffer[1024] = {};
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);

    if (bytes_read == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) // Temporally no data available because the socket is non-blocking
            return;
        std::cerr << "Debug: Server.client_message: Error reading from socket " << fd << std::endl;
        client_disconnect(fd);
        return;
    }

    buffer[bytes_read] = '\0';
    std::string message(buffer);

    std::cout << "Debug: Server.client_message: Message from client " << fd << ": \n" << buffer << std::endl;

    // Extract command from message
    std::string command = extract_command(message);

    std::cout << "Debug: Server.client_message: command " << fd << ": \n" << command << std::endl;*/
    std::string message;
    char buffer[100];
    bzero(buffer, 100);

    while (!std::strstr(buffer, "\r\n")) {
        bzero(buffer, 100);

        if (recv(fd, buffer, 100, 0) < 0) {
            if (errno != EWOULDBLOCK)
                throw std::runtime_error("Error while reading buffer from client.");
        }
        message.append(buffer);
    }
    Client *current_client = s_clients.at(fd);
    std::stringstream sstream(message);
    std::string aux;

    try{
        while (std::getline(sstream, aux)){
            std::string name = aux.substr(0, aux.find(' '));
            //std::vector<std::string> tokens = get everything else
            if (name == "PING"){
                Command *okaywork = s_commands.at("PONG");
                okaywork->execute(current_client);
            }
        }
        //Client *aux_client = s_clients.at(fd);
    }
    catch(const std::runtime_error & e) {
        std::cout << "Error : " << e.what() << std::endl;
    }
}



std::string Server::extract_command(const std::string& msg) {
    std::string command;
    std::stringstream ss(msg);
    ss >> command;
    return command;
}
