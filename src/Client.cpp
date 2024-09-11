
#include "../inc/Client.hpp"


Client::Client(int fd) : fd(fd), authenticated(false) {
  std::cout << "Debug: Client constructor" << std::endl;
}

Client::~Client() {
  std::cout << "Debug: Client destructor" << std::endl;
  if (fd != -1) {
    close(fd);
  }
}

int     Client::get_fd() const { return fd; }
bool    Client::is_authenticated() const { return authenticated; }
void    Client::authenticate() { authenticated = true; }