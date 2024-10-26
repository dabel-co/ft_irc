
#include "../inc/Client.hpp"

std::string Client::GetPrefix() const {
  return nickname_ + (username_.empty() ? "" : "!" + username_) + "@127.0.0.1";
}

void Client::Write(const std::string &message) const {

  time_t rawtime;
  struct tm *timeinfo;
  char timebuffer[10];
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(timebuffer, sizeof(timebuffer), "%H:%M:%S", timeinfo);
  std::string str(timebuffer);
  std::cout << "[" << str << "] " << message << std::endl;


  const std::string buffer = message + "\r\n";
  if (send(fd_, buffer.c_str(), buffer.length(), 0) < 0)
    throw std::runtime_error("Error while sending message to client.");
}

void Client::Reply(const std::string &reply) const {
  Write(":" + GetPrefix() + " " + reply);
}
