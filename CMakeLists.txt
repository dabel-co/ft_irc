CMAKE_MINIMUM_REQUIRED(VERSION 3.13)
PROJECT(ft_irc)
SET(CMAKE_CXX_STANDARD 20)
INCLUDE_DIRECTORIES(
        "${PROJECT_SOURCE_DIR}/inc"
)

SET(CFiles
        src/main.cpp
        src/Server.cpp
        src/Command.cpp
        src/Client.cpp
        src/Channel.cpp
)
SET (HFiles
        inc/Channel.hpp
        inc/Client.hpp
        inc/Command.hpp
        inc/Server.hpp
)
ADD_EXECUTABLE("${PROJECT_NAME}" ${CFiles} ${HFiles})
