#include <iostream>
#include <stdexcept>

#include "Server.h"

int main(int argc, char **argv)
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <passwd>" << std::endl;
        return (0);
    }
    try {
        Server server(argv[1], argv[2]);
        server.run();
    }
    catch(const std::runtime_error & e) {
        std::cout << "Error : " << e.what() << std::endl;
    }
    return 0;
}
