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
#include <nlohmann/json.hpp>
#include "RequestHandler.h"
#include "server.h"
#include "MessageHandler.h"

int main(int argc, char *argv[])
{
    Server s = Server(in6addr_any, 57076);
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
            db_host = getenv("DATABASE_HOST") ? getenv("DATABASE_HOST") : "localhost",
            db_port = getenv("DATABASE_PORT") ? getenv("DATABASE_PORT") : "5432";

    this->database = Database("dbname =" + db_name + "user = " + db_user + " password = " + db_password + " hostaddr = " + db_host + " port = " + db_port, logger);

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
        auto server_message =  MessageHandler::server_error_message(e.what());
        handler.send_message(server_message);
        close_single_connection(args);
        return nullptr;
    }
    std::cout << clientMessage.token << " " << clientMessage.code << " " << clientMessage.body << std::endl;
    JSONParser::server_message serverMessage{};
    handler.send_message(serverMessage);

    /*return message*/
    auto server_message = MessageHandler(clientMessage, args->database).run_as_server();
    try{
        handler.send_message(server_message);
        args->logger->info("message: " + JSONParser::generate_server_message(server_message) +
                           "sent from thread from socket: " + std::to_string(args->new_socket));
    } catch (const std::exception& e) {
        args->logger->error(e.what());
    }

    close_single_connection(args);
    return nullptr;
}

void Server::close_single_connection(thread_args *args)
{
    close((args->new_socket));
    args->logger->info("end of thread for socket: " + std::to_string(args->new_socket));
    delete args;
}