#include "common.h"

int main()
{
    signal(SIGINT, sigint_handler);

    int server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket");
        return 1;
    }

    int opt = 1;
    if (::setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        ::close(server_fd);
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (::bind(server_fd, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        ::close(server_fd);
        return 1;
    }

    if (::listen(server_fd, BACKLOG) < 0)
    {
        perror("listen");
        ::close(server_fd);
        return 1;
    }

    std::cout << "Server started on port " << PORT << '\n';
    std::cout << "Waiting for clients...\n";

    while (true)
    {
        int client_fd = ::accept(server_fd, nullptr, nullptr);
        if (client_fd < 0)
        {
            perror("accept");
            continue;
        }

        std::cout << "Client connected (fd=" << client_fd << ")\n";

        char buffer[BUFFER_SIZE + 1];

        // 1. 닉네임 요청
        const char *ask_msg = "Please enter your nickname:\n";
        if (::write(client_fd, ask_msg, std::strlen(ask_msg)) < 0)
        {
            perror("write ask_msg");
            ::close(client_fd);
            continue;
        }

        // 2. 닉네임 수신
        int n = ::read(client_fd, buffer, BUFFER_SIZE);
        if (n < 0)
        {
            perror("read nickname");
            ::close(client_fd);
            continue;
        }
        if (n == 0)
        {
            std::cout << "Client disconnected before sending nickname\n";
            ::close(client_fd);
            continue;
        }

        buffer[n] = '\0';
        std::string nickname(buffer);

        while (!nickname.empty() &&
               (nickname.back() == '\n' || nickname.back() == '\r'))
        {
            nickname.pop_back();
        }

        if (nickname.empty())
            nickname = "anonymous";

        std::cout << "Client fd=" << client_fd
                  << " identified as [" << nickname << "]\n";

        // 3. 환영 메시지
        std::string welcome =
            "Welcome, " + nickname + "\n"
                                     "Enter text (q to quit):\n";

        if (::write(client_fd, welcome.c_str(), welcome.length()) < 0)
        {
            perror("write welcome");
            ::close(client_fd);
            continue;
        }

        // 4. 일반 메시지 처리
        while (true)
        {
            int bytes = ::read(client_fd, buffer, BUFFER_SIZE);

            if (bytes < 0)
            {
                perror("read");
                break;
            }

            if (bytes == 0)
            {
                std::cout << "[" << nickname << "] disconnected\n";
                break;
            }

            buffer[bytes] = '\0';
            std::cout << "[" << nickname << "] says: " << buffer;

            if (std::tolower((unsigned char)buffer[0]) == 'q' &&
                (buffer[1] == '\n' || buffer[1] == '\0'))
            {
                std::cout << "[" << nickname << "] requested quit\n";
                std::string goodbye = "Goodbye, " + nickname + "\n";
                ::write(client_fd, goodbye.c_str(), goodbye.length());
                break;
            }

            std::string reply = "You said: ";
            if (::write(client_fd, reply.c_str(), reply.length()) < 0 ||
                ::write(client_fd, buffer, bytes) < 0)
            {
                perror("write echo");
                break;
            }
        }

        ::close(client_fd);
    }

    ::close(server_fd);
    return 0;
}