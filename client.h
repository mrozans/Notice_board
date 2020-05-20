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
#include <utility>
#include <sys/socket.h>
#include <sys/types.h>
#include <zconf.h>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include "JSONParser.h"
#include "RequestHandler.h"

class Client
{
    char const *server_name;
    uint16_t port;
    int timeout;
    std::shared_ptr<spdlog::logger> logger;
    RequestHandler handler;
    union sockaddr_t
    {
        struct sockaddr_in ipv4;
        struct sockaddr_in6 ipv6;
    } server{};
    int sock{};

public:
    Client(char const *server_name, uint16_t port, int timeout, std::shared_ptr<spdlog::logger> logger) noexcept(false) : server_name(server_name), port(port), timeout(timeout), logger(std::move(logger))
    {
        connect_to_server();
    };

    ~Client()
    {
        close(sock);
    };

    void send(const JSONParser::client_message& message);

    JSONParser::server_message read_message();

private:

    static bool check_if_ipv6(const char *ip);
    void connect_to_ipv4() noexcept(false);;
    void connect_to_ipv6() noexcept(false);;
    void connect_to_server() noexcept(false);
};