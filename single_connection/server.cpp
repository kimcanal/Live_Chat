#include "common.h"

int main()
{
    signal(SIGINT, sigint_handler);
    // 1. 서버 소켓 생성
    int server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket");
        return 1;
    }

    // 2. 포트 재사용 옵션
    int opt = 1;
    if (::setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        ::close(server_fd);
        return 1;
    }

    // 3. 주소 구조체 설정
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    // 4. bind
    if (::bind(server_fd, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        ::close(server_fd);
        return 1;
    }

    // 5. listen
    if (::listen(server_fd, BACKLOG) < 0)
    {
        perror("listen");
        ::close(server_fd);
        return 1;
    }

    std::cout << "Server started on port " << PORT << '\n';
    std::cout << "You may enter CTRL+C to stop the server\n";
    std ::cout << "Waiting for clients...\n";

    while (true)
    {
        // 6. 클라이언트 접속 대기
        int client_fd = ::accept(server_fd, nullptr, nullptr);
        if (client_fd < 0)
        {
            perror("accept");
            continue;
        }

        std::cout << "Client " << client_fd << " connected\n";

        // 7. 환영 메시지 전송
        const char *msg1 = "You are connected to the server\n";
        const char *msg2 = "Enter text (q to quit):\n";

        if (::write(client_fd, msg1, std::strlen(msg1)) < 0)
        {
            perror("write msg1");
            ::close(client_fd);
            continue;
        }

        if (::write(client_fd, msg2, std::strlen(msg2)) < 0)
        {
            perror("write msg2");
            ::close(client_fd);
            continue;
        }

        // 8. 입력 받기
        while (true)
        {
            char buffer[BUFFER_SIZE + 1];
            int n = ::read(client_fd, buffer, BUFFER_SIZE);

            if (n < 0)
            {
                perror("read");
                break;
            }

            if (n == 0)
            {
                std::cout << "Client disconnected without sending data\n";
                break;
            }

            // 9. 문자열 종료 처리
            buffer[n] = '\0';

            std::cout << "Received: " << buffer;

            // 10. q 입력이면 종료
            if (std::tolower((unsigned char)buffer[0]) == 'q' &&
                (buffer[1] == '\n' || buffer[1] == '\0'))
            {
                std::cout << "Client " << client_fd << " requested quit\n";
                std::string goodbye_msg = "Goodbye, Client " + std::to_string(client_fd) + "\n";
                write(client_fd, goodbye_msg.c_str(), goodbye_msg.length());
                break;
            }

            // 11. echo
            if (::write(client_fd, "You said: ", strlen("You said: ")) < 0 || ::write(client_fd, buffer, n) < 0)
            {
                perror("write echo");
                break;
            }
        }

        // 12. 클라이언트 소켓 닫기
        ::close(client_fd);
    }

    ::close(server_fd);
    return 0;
}