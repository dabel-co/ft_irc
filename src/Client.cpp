
#include "../inc/Client.hpp"


Client::Client(int fd) : fd(fd) {
  std::cout << "Debug: Client constructor" << std::endl;
}

Client::~Client() {
  std::cout << "Debug: Client destructor" << std::endl;
  if (fd != -1) {
    close(fd);
  }
}

int Client::get_fd() const {
  return fd;
}
