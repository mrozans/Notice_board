#include <ctime>
#include <iostream>
#include <netinet/in.h>
#include <netdb.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <zconf.h>

class Client
{
private:
    union sockaddr_t
    {
        struct sockaddr_in ipv4;
        struct sockaddr_in6 ipv6;
    };

    char *server_name, *port;
    int sock{};
    sockaddr_t server{};
    struct hostent *hp{};
    struct thread_args {
        int socket;
    };
public:
    thread_args *args{};

    Client(char *server_name, char *port) : server_name(server_name), port(port) { }

    int connect_to_server()
    {
        if(check_if_ipv6(server_name))
            return connect_to_ipv6();

        return connect_to_ipv4();
    }

    int send(char *message)
    {
        if(connect(sock, (struct sockaddr *) &server, sizeof server) == -1)
        {
            perror("Connecting stream socket");
            return 1;
        }

        if(write(sock, message, (strlen(message) + 1) * sizeof(char)) == -1)
        {
            perror("Writing on stream socket");
            return 1;
        }

        return 0;
    }

    int disconnect() const
    {
        close(sock);
        return 0;
    }

    static void *read_message(void *voidArgs)
    {
        auto *args = (thread_args *) voidArgs;
        const int size = 40;
        char *buffer = new char[size];
        for (int i = 0; i < size; ++i)
        {
            buffer[i] = '\0';
        }
        time_t current_time = time(nullptr);
        while(true)
        {
            if(read(args->socket, buffer, 40) > 1)
            {
                printf("%s\n", buffer);
                break;
            }
            if(time(nullptr) - current_time > 10)
            {
                printf("no confirmation form server\n");
                break;
            }
        }
        delete[] buffer;
        delete args;
        return 0;
    }

private:
    static bool check_if_ipv6(const char *ip)
    {
        for(int i = 0; ip[i] != '\0'; i++)
            if(ip[i] == ':') return true;

        return false;
    }

    int connect_to_ipv4()
    {
        sock = socket(AF_INET, SOCK_STREAM, 0);

        if(sock == -1)
        {
            perror("String socket can't be opened!");
            return 1;
        }

        args = new thread_args;
        args->socket = sock;

        server.ipv4.sin_family = AF_INET;

        hp = gethostbyname2(server_name, AF_INET);

        if(hp == (struct hostent *) nullptr)
        {
            std::cout << "Unknown host: " << server_name << std::endl;
            return 1;
        }


        memcpy((char *) &server.ipv4.sin_addr, (char *) hp->h_addr, hp->h_length);

        server.ipv4.sin_port = htons(atoi(port));

        return 0;
    }

    int connect_to_ipv6()
    {
        sock = socket(AF_INET6, SOCK_STREAM, 0);

        if(sock == -1)
        {
            perror("String socket can't be opened!");
            return 1;
        }

        args = new thread_args;
        args->socket = sock;

        server.ipv6.sin6_family = AF_INET6;

        hp = gethostbyname2(server_name, AF_INET6);

        if(hp == (struct hostent *) nullptr)
        {
            std::cout << "Unknown host: " << server_name << std::endl;
            return 1;
        }

        memcpy((char *) &server.ipv6.sin6_addr, (char *) hp->h_addr, hp->h_length);

        server.ipv6.sin6_port = htons(atoi(port));

        return 0;
    }

    int send_to_ipv4(char *message)
    {
        if(connect(sock, (struct sockaddr *) &server.ipv4, sizeof server.ipv4) == -1)
        {
            perror("Connecting stream socket");
            return 1;
        }

        return send_message(message);
    }

    int send_to_ipv6(char *message)
    {
        if(connect(sock, (struct sockaddr *) &server.ipv6, sizeof server.ipv6) == -1)
        {
            perror("Connecting stream socket");
            return 1;
        }

        return send_message(message);
    }

    int send_message(char *message) const
    {
        if(write(sock, message, (strlen(message) + 1) * sizeof(char)) == -1)
        {
            perror("Writing on stream socket");
            return 1;
        }

        return 0;
    }
};