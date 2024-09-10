//
// Created by dabel-co on 29/08/24.
//

#include "../inc/Client.hpp"


Client::Client(int fd) : fd(fd) {
  std::cout << "Debug: Client constructor" << std::endl;
}

Client::~Client() {
  std::cout << "Debug: Client destructor" << std::endl;
}