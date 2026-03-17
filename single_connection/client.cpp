#include "common.h"

int main(int argc, char *argv[])
{
    signal(SIGINT, sigint_handler);

    std::string nickname;

    // 1. 닉네임 결정
    if (argc >= 2 && std::strlen(argv[1]) < NICKNAME_MAX)
    {
        nickname = argv[1];
    }
    else
    {
        while (true)
        {
            std::cout << "Enter nickname (less than " << NICKNAME_MAX << " characters): ";
            std::getline(std::cin, nickname);

            if (!nickname.empty() && nickname.length() < NICKNAME_MAX)
                break;
            else
                std::cout << "Invalid nickname. Please try again.\n";
        }
    }

    std::cout << "Your nickname: " << nickname << '\n';

    // 2. 소켓 생성
    int sock_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        perror("socket");
        return 1;
    }

    // 3. 서버 주소 설정
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (::inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        ::close(sock_fd);
        return 1;
    }

    // 4. 서버 연결
    std::cout << "Connecting to server...\n";
    if (::connect(sock_fd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect");
        ::close(sock_fd);
        return 1;
    }

    char buffer[BUFFER_SIZE + 1];

    // 5. 서버의 닉네임 요청 받기
    int n = ::read(sock_fd, buffer, BUFFER_SIZE);
    if (n <= 0)
    {
        if (n < 0)
            perror("read");
        else
            std::cout << "Server closed connection.\n";

        ::close(sock_fd);
        return 1;
    }

    buffer[n] = '\0';
    std::cout << buffer;

    // 6. 닉네임 전송
    std::string name_msg = nickname + "\n";
    if (::write(sock_fd, name_msg.c_str(), name_msg.length()) < 0)
    {
        perror("write nickname");
        ::close(sock_fd);
        return 1;
    }

    // 7. 환영 메시지 읽기
    n = ::read(sock_fd, buffer, BUFFER_SIZE);
    if (n <= 0)
    {
        if (n < 0)
            perror("read");
        else
            std::cout << "Server closed connection.\n";

        ::close(sock_fd);
        return 1;
    }

    buffer[n] = '\0';
    std::cout << "Connected and accepted by server.\n";
    std::cout << buffer;

    // 8. 일반 채팅
    while (true)
    {
        std::cout << "> ";
        std::string input;
        std::getline(std::cin, input);

        input += "\n";

        if (::write(sock_fd, input.c_str(), input.length()) < 0)
        {
            perror("write");
            break;
        }

        int bytes = ::read(sock_fd, buffer, BUFFER_SIZE);
        if (bytes < 0)
        {
            perror("read");
            break;
        }
        if (bytes == 0)
        {
            std::cout << "Server disconnected.\n";
            break;
        }

        buffer[bytes] = '\0';
        std::cout << buffer;

        if (std::tolower((unsigned char)input[0]) == 'q' &&
            (input[1] == '\n' || input[1] == '\0'))
        {
            break;
        }
    }

    ::close(sock_fd);
    return 0;
}