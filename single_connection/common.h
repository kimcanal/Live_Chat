#include <iostream>
#include <cstring>
#include <cctype>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define BACKLOG 10

void sigint_handler(int sig)
{
    std::cout << "\nExiting..." << std::endl;
    exit(0); // 프로그램 안전 종료
}