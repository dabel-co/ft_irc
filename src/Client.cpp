
#include "../inc/Client.hpp"
#include <sys/socket.h>


Client::Client(int fd) : fd(fd), authenticated(false) {
  //std::cout << "Debug: Client constructor" << std::endl;
}

Client::~Client() {
  //std::cout << "Debug: Client destructor" << std::endl;
  if (fd != -1) {
    close(fd);
  }
}

int          Client::get_fd() const { return fd; }
std::string  Client::get_nickname() const { return nickname; }
void         Client::set_nick(std::string nick) {this->nickname = nick; }

std::string  Client::get_username() const { return username; }
void         Client::set_username(std::string username) {this->username = username; }
bool         Client::is_authenticated() const { return authenticated; }
void         Client::authenticate() { authenticated = true; }





/*std::string Client::getPrefix() const {
  return nickname + (username.empty() ? "" : "!" + username) + "@127.0.0.1";
}

void Client::write(const std::string &message) const {
  std::string buffer = message + "\r\n";
  if (send(fd, buffer.c_str(), buffer.length(), 0) < 0)
    throw std::runtime_error("Error while sending message to client.");
}

void Client::reply(const std::string &reply) {
  write(":" + getPrefix() + " " + reply);
}*/


