
#include "../inc/Client.hpp"

std::string Client::GetPrefix() const {
  return nickname_ + (username_.empty() ? "" : "!" + username_) + "@127.0.0.1";
}

void Client::Write(const std::string &message) const {
  std::cout << "Debug Write = " << message << std::endl;
  const std::string buffer = message + "\r\n";
  if (send(fd_, buffer.c_str(), buffer.length(), 0) < 0)
    throw std::runtime_error("Error while sending message to client.");
}

void Client::Reply(const std::string &reply) const {
  Write(":" + GetPrefix() + " " + reply);
}
