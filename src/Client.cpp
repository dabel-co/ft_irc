
#include "../inc/Client.hpp"
#include <sys/socket.h>


Client::Client(int Fd) : fd_(Fd), isAuth_(false), channel_(NULL) {
  //std::cout << "Debug: Client constructor" << std::endl;
}

Client::~Client() {
  //std::cout << "Debug: Client destructor" << std::endl;
  if (fd_ != -1) {
    close(fd_);
  }
}

std::string Client::GetPrefix() const {
  return nickname_ + (username_.empty() ? "" : "!" + username_) + "@127.0.0.1";
}

void Client::Write(const std::string &message) const {
  std::cout << "Debug Write = " << message << std::endl;
  std::string buffer = message + "\r\n";
  if (send(fd_, buffer.c_str(), buffer.length(), 0) < 0)
    throw std::runtime_error("Error while sending message to client.");
}

void Client::Reply(const std::string &reply) const {
  Write(":" + GetPrefix() + " " + reply);
}