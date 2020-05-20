/**
 * @authors Tomasz Mazur, Marcin Mozolewski
 *
 */

#ifndef TIN_TABLICA_OGLOSZEN_SERVER_H
#define TIN_TABLICA_OGLOSZEN_SERVER_H

#include <netinet/in.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/base_sink.h"
#include "database.h"

class Server{
public:
    const in6_addr addr;
    uint16_t port;
    char IP[16];
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
    void configure_server();
    void start_server(int connections);

private:
    static void *handle_message(void *voidArgs);

    static void close_single_connection(thread_args *args);
};

#endif //TIN_TABLICA_OGLOSZEN_SERVER_H