
#include "../inc/Client.hpp"
#include <sys/socket.h>


Client::Client(int Fd) : fd_(Fd), isAuth_(false), username_("") {
  //std::cout << "Debug: Client constructor" << std::endl;
}

Client::~Client() {
  //std::cout << "Debug: Client destructor" << std::endl;
  if (fd_ != -1) {
    close(fd_);
  }
}

int          Client::GetFd() const { return fd_; }
std::string  Client::GetNickname() const { return nickname_; }
void         Client::SetNickname(std::string nickname) {this->nickname_ = nickname; }

std::string  Client::GetUsername() const { return username_; }
void         Client::SetUsername(std::string username) {this->username_ = username; }
bool         Client::IsAuth() const { return isAuth_; }
void         Client::Authenticate() { isAuth_ = true; }





std::string Client::GetPrefix() const {
  return nickname_ + (username_.empty() ? "" : "!" + username_) + "@127.0.0.1";
}

void Client::Write(const std::string &message) const {
  std::string buffer = message + "\r\n";
  if (send(fd_, buffer.c_str(), buffer.length(), 0) < 0)
    throw std::runtime_error("Error while sending message to client.");
}

void Client::Reply(const std::string &reply) {
  Write(":" + GetPrefix() + " " + reply);
}


