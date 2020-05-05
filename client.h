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
    const static unsigned int MESSAGE_MAX_LEN = 4;
    thread_args *args{};

    Client(char *server_name, char *port) : server_name(server_name), port(port) { };

    int connect_to_server();

    int send(char *message);

    int disconnect() const;

    static void *read_message(void *voidArgs);

private:
    bool check_if_ipv6(const char *ip);
    int connect_to_ipv4();
    int connect_to_ipv6();
    int send_to_ipv4(char *message);
    int send_to_ipv6(char *message);
    int send_message(char *message) const;
};