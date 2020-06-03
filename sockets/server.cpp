/**
 * @authors Tomasz Mazur, Marcin Mozolewski
 * @date 04.05.2020
 * @name Tablica ogłoszeń
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
#include <nlohmann/json.hpp>
#include <netdb.h>
#include "communication/RequestHandler.h"
#include "server.h"
#include "communication/Message.h"

int main(int argc, char *argv[])
{
    Server s = Server(in6addr_any, getenv("SERVER_PORT") ? std::stoi(getenv("SERVER_PORT")) : 57076);
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

    std::string db_name = getenv("DATABASE_NAME") ? getenv("DATABASE_NAME") : "noticeboard",
            db_user = getenv("DATABASE_USER") ? getenv("DATABASE_USER") : "noticeboard",
            db_password = getenv("DATABASE_PASSWORD") ? getenv("DATABASE_PASSWORD") : "noticeboard",
            db_host = getenv("DATABASE_HOST") ? getenv("DATABASE_HOST") : "0.0.0.0",
            db_port = getenv("DATABASE_PORT") ? getenv("DATABASE_PORT") : "5432";

    auto db_host_hostent = gethostbyname2(db_host.c_str(), AF_INET);
    if(db_host_hostent == nullptr)
    {
        logger->critical("Cannot connect to database! Unknown host");
        return;
    }

    std::string db_host_ip = inet_ntoa(*((struct in_addr*) db_host_hostent->h_addr_list[0]));

    this->database = Database("dbname = " + db_name + " user = " + db_user + " password = " + db_password + " hostaddr = " + db_host_ip + " port = " + db_port, logger);

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
        args->database = this->database;
        args->logger = logger;
        args->timeout = 5;
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
    auto handler = RequestHandler(args->new_socket,args->logger, args->timeout);

    args->logger->info("start of thread for socket: " + std::to_string(args->new_socket));
    //receive and
    char * message;
    JSONParser::client_message clientMessage;
    try{
        message = handler.read_message();
        clientMessage = JSONParser::get_client_message(message);
        delete message;
    } catch (const std::exception& e) {
        args->logger->error(e.what());
//        try{
//            auto server_message =  Message::server_error_message(e.what());
//            handler.send_message(server_message);
//        } catch (const std::exception& e) {
//            args->logger->error(e.what());
//        }
        close_single_connection(args);
        return nullptr;
    }
    /*return message*/

    auto server_message = Message(clientMessage, args->database).run();
    response(handler, server_message, args);
    close_single_connection(args);

    return nullptr;
}

void Server::response(RequestHandler handler, JSONParser::server_message message, thread_args *args)
{
    try{
        handler.send_message(message);
        args->logger->info("message: " + JSONParser::generate_server_message(message) +
                           "sent from thread from socket: " + std::to_string(args->new_socket));
    } catch (const std::exception& e) {
        args->logger->error(e.what());
    }
}

void Server::close_single_connection(thread_args *args)
{
    close((args->new_socket));
    args->logger->info("end of thread for socket: " + std::to_string(args->new_socket));
    delete args;
}
