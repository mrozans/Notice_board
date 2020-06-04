/**
 * @authors Tomasz Mazur, Marcin Mozolewski
 * @date 04.05.2020
 * @name Tablica ogłoszeń
 */

#ifndef TIN_TABLICA_OGLOSZEN_SERVER_H
#define TIN_TABLICA_OGLOSZEN_SERVER_H

#include <netinet/in.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/base_sink.h"
#include "database/database.h"

class Server{
public:
    const in6_addr addr;
    uint16_t port;
    char IP[16]{};
private:
    int server_socket;
    const int mode = 0;
    struct sockaddr_in6 server{};
    struct sockaddr_storage server_storage{};
    std::shared_ptr<spdlog::logger> logger;
    struct thread_args {
        int new_socket;
        std::shared_ptr<spdlog::logger> logger;
        int timeout;
        Database database;
    };
    Database database;
//methods
public:
    explicit Server(in6_addr addr = in6addr_any, uint16_t port_number = 0);
    ~Server()
    {
        stop();
    }
    void configure_server();
    void start_server(int connections);
    void stop();

private:

    static void *handle_message(void *voidArgs);

    static void close_single_connection(thread_args *args);

    static void response(const RequestHandler& handler, const JSONParser::server_message& message, thread_args *args);

    bool run{};
    int timeout = 5;

    static JSONParser::client_message get_message(RequestHandler handler, thread_args *args);
};

#endif //TIN_TABLICA_OGLOSZEN_SERVER_H