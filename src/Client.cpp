//
// Created by dabel-co on 29/08/24.
//

#include "../inc/Client.h"
#include <iostream>
Client::Client(int fd) : fd(fd) {
  std::cout << "Client constructor" << std::endl;
}
Client::~Client() {
  std::cout << "Client destructor" << std::endl;
}