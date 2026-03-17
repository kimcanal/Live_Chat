#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <string>
#include <cstring>
#include <cctype>
#include <csignal>
#include <cstdlib>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define BACKLOG 10
#define NICKNAME_MAX 20

inline void sigint_handler(int sig)
{
    std::cout << "\nExiting..." << std::endl;
    exit(0);
}

#endif