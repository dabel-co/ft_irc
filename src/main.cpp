
#include <iostream>
#include <stdexcept>

#include "../inc/Server.hpp"

int main(int argc, char **argv)
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <passwd>" << std::endl;
        return (0);
    }
    try {
        Server server(argv[1], argv[2]);
        server.Run();
    }
    catch(const std::runtime_error & e) {
        std::cout << "Error : " << e.what() << std::endl;
    }
    return 0;
}
