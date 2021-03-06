/**
 * @authors Tomasz Jóźwik, Marcin Różański
 * @date 20.05.2020
 * @name Tablica ogłoszeń
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
#include "communication/JSONParser.h"
#include "communication/RequestHandler.h"
#include "database/database.h"

Database database;

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
    static std::mutex mutex;

    void connect_to_server() noexcept(false);

public:
    Client(char const *server_name, uint16_t port, int timeout, std::shared_ptr<spdlog::logger> logger) noexcept(false) : server_name(server_name), port(port), timeout(timeout), logger(std::move(logger))
    {
        mutex.lock();
    };

    ~Client()
    {
        close(sock);
        mutex.unlock();
    };

    JSONParser::server_message send_and_receive(const JSONParser::client_message& message) noexcept(false);
    JSONParser::server_message authorization(const std::string& token) noexcept(false);
    JSONParser::server_message create_new_message(const std::string& token, const std::string& category, const std::string &title, const std::string &content, const std::string& days) noexcept(false);
    JSONParser::server_message remove_message(const std::string& token, const std::string& message_id) noexcept(false);


protected:

    void send(const JSONParser::client_message& message);

    JSONParser::server_message receive();

private:

    static bool check_if_ipv6(const char *ip);
    void connect_to_ipv4() noexcept(false);
    void connect_to_ipv6() noexcept(false);
};