/**
 * @authors Tomasz Jóźwik, Marcin Różański
 *
 */

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
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include "JSONParser.h"
#include "RequestHandler.h"

class Client
{
private:
    union sockaddr_t
    {
        struct sockaddr_in ipv4;
        struct sockaddr_in6 ipv6;
    };

    char const *server_name, *port;
    int sock{};
    sockaddr_t server{};
    struct hostent *hp{};

    int timeout;
public:
    Client(char const *server_name, char const *port, int timeout) : server_name(server_name), port(port), timeout(timeout)
    {
        try
        {
            logger = spdlog::basic_logger_mt("client", "client-logs.txt");
        }
        catch (const spdlog::spdlog_ex &ex)
        {
            std::cout << "Log init failed: " << ex.what() << std::endl;
        }
        logger->flush_on(spdlog::level::info);
    };

    void connect_to_server();

    void send(const JSONParser::client_message& message);

    void disconnect() const;

    static void read_message(void *voidArgs);

private:
    static bool check_if_ipv6(const char *ip);
    void connect_to_ipv4();
    void connect_to_ipv6();
    std::shared_ptr<spdlog::logger> logger;
    RequestHandler handler;
};