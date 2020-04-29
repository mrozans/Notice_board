/**
 * @authors Tomasz Mazur, Marcin Mozolewski
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include <arpa/inet.h>
#include "RequestHandler.h"
#include "server.h"

int main(int argc, char *argv[])
{
    Server s = Server(in6addr_any, 57069);
    s.configure_server();
    s.start_server(40);
    //s.stop();
}

Server::Server(in6_addr addr, uint16_t port_number): addr(addr), port(port_number)
{
    try
    {
        logger = spdlog::basic_logger_mt("server", "server-logs.txt");
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cout << "Log init failed: " << ex.what() << std::endl;
    }
    logger->flush_on(spdlog::level::info);

    server_socket = socket(AF_INET6, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        logger->error(strerror(errno));
        exit(1);
    }
    setsockopt(server_socket, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&mode, sizeof(mode));
}

void Server::configure_server()
{
    server.sin6_family = AF_INET6;
    server.sin6_addr = addr;
    server.sin6_port = htons(port);

    int bind_status = bind(server_socket, (struct sockaddr *) &server, sizeof(server));
    if (bind_status == -1)
    {
        logger->error(strerror(errno));
        exit(1);
    }

    int length = sizeof(server);
    if (getsockname(server_socket, (struct sockaddr *) &server, (socklen_t *) (&length)) == -1)
    {
        logger->error(strerror(errno));
        exit(1);
    }

    inet_ntop(AF_INET, &server.sin6_addr, IP, sizeof(IP));

    std::string message = "server configured with port: " + std::to_string(ntohs(server.sin6_port)) + " address: " + IP;
    logger->info(message);
}

void Server::start_server(const int connections)
{
    if (listen(server_socket, connections) != 0)
    {
        logger->error(strerror(errno));
        return;
    }
    socklen_t address_size = sizeof(server_storage);

    thread_args *args;
    pthread_t tid;
    pthread_attr_t t_attr;
    pthread_attr_init(&t_attr);
    pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);

    logger->info("server started");

    while (true)
    {
        args = new thread_args;
        args->logger = logger;
        args->new_socket = accept(server_socket, (struct sockaddr *) &server_storage, &address_size);

        setsockopt(args->new_socket, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&mode, sizeof(mode));

        if (pthread_create(&tid, &t_attr, Server::handle_message, args) != 0)
        {
            logger->error(strerror(errno));
            continue;
        }
    }
}

void *Server::handle_message(void *voidArgs)
{
    auto *args = (thread_args *) voidArgs;
    auto handler = RequestHandler(args->new_socket,args->logger, 5);

    args->logger->info("start of thread for socket: " + std::to_string(args->new_socket));

    //create example message
    const int size = 40;
    char *buffer = new char[size];
    for (int i = 0; i < size; ++i)
    {
        buffer[i] = '\0';
    }

    strcpy(buffer, "Hello Client\n");

//    try{
//        handler.send_message(buffer, size);
//    } catch (const std::exception& e) {
//        args->logger->error(e.what());
//    }

    //response
    char *response;
    try
    {
        response = handler.read_message();
        printf("%s\n", response);
        delete[] response;
    } catch (const std::exception& e)
    {
        args->logger->error(e.what());
    }

    //close connection
    close((args->new_socket));
    delete[] buffer;
    delete args;
    args->logger->info("end of thread for socket: " + std::to_string(args->new_socket));
    return nullptr;
}