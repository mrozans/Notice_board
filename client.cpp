/**
 * @authors Tomasz Jóźwik, Marcin Różański
 *
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <utility>
#include "client.h"

void example_client(char const *server_name, uint16_t port, std::shared_ptr<spdlog::logger> logger);

void example_client(char const *server_name, uint16_t port, std::shared_ptr<spdlog::logger> logger)
{
    //create new connection
    JSONParser::server_message server_response{};
    try{
        auto client = Client(server_name, port, 5, std::move(logger));
        server_response = client.authorization("my_token");
    } catch (const std::exception &e) {
        logger->error(e.what());
    }

    std::cout << server_response.body << " " << server_response.code << std::endl;
    //do something with response message. better close connection before message operations
    //end connection
    //clint will be automatically destroyed, because
}

int main(int argc, char *argv[])
{
    std::shared_ptr<spdlog::logger> logger;
    try
    {
        logger = spdlog::basic_logger_mt("client", "client-logs.txt");
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cout << "Log init failed: " << ex.what() << std::endl;
    }
    logger->flush_on(spdlog::level::info);

    example_client("127.0.0.1", 57076, logger);
}

void Client::connect_to_server()
{
    if(check_if_ipv6(server_name))
    {
        connect_to_ipv6();
    }else{
        connect_to_ipv4();
    }
    this->handler = RequestHandler(this->sock,this->logger, this->timeout);
}

void Client::send(const JSONParser::client_message& message)
{
    handler.send_message(message);
}

JSONParser::server_message Client::receive()
{
    return JSONParser::get_server_message(handler.read_message());
}

bool Client::check_if_ipv6(const char *ip)
{
    return std::string(ip).find(':') != std::string::npos;
}

void Client::connect_to_ipv4()
{
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock == -1)
    {
        throw std::logic_error("String socket can't be opened!");
    }

    server.ipv4.sin_family = AF_INET;

    auto hp = gethostbyname2(server_name, AF_INET);

    if(hp == (struct hostent *) nullptr)
    {
        throw std::logic_error(std::string("Unknown host: ") + server_name);
    }

    memcpy((char *) &server.ipv4.sin_addr, hp->h_addr, (size_t)hp->h_length);

    server.ipv4.sin_port = htons(port);

    if(connect(sock, (struct sockaddr *) &server.ipv4, sizeof server.ipv4) == -1)
    {
        throw std::logic_error("Connecting stream socket");
    }
}

void Client::connect_to_ipv6()
{
    sock = socket(AF_INET6, SOCK_STREAM, 0);

    if(sock == -1)
    {
        throw std::logic_error("String socket can't be opened!");
    }

    server.ipv6.sin6_family = AF_INET6;

    auto hp = gethostbyname2(server_name, AF_INET6);

    if(hp == (struct hostent *) nullptr)
    {
        throw std::logic_error(std::string("Unknown host: ") + server_name);
    }

    memcpy((char *) &server.ipv6.sin6_addr, hp->h_addr, (size_t)(hp->h_length));

    server.ipv6.sin6_port = htons(port);

    if(connect(sock, (struct sockaddr *) &server.ipv6, sizeof server.ipv6) == -1)
    {
        throw std::logic_error("Connecting stream socket");
    }
}

JSONParser::server_message Client::send_and_receive(const JSONParser::client_message &message) noexcept(false)
{
    send(message);
    return receive();
}

JSONParser::server_message Client::authorization(const std::string& token) noexcept(false)
{
    JSONParser::client_message message = {
            token,
            1,
            ""
    };
    return send_and_receive(message);
}

JSONParser::server_message Client::create_new_message(const std::string& token) noexcept(false)
{
    //todo
    JSONParser::client_message message = {
            token,
            2,
            ""
    };
    return send_and_receive(message);
}

JSONParser::server_message Client::get_new_messages(const std::string& token) noexcept(false)
{
    //todo
    JSONParser::client_message message = {
            token,
            3,
            ""
    };
    return send_and_receive(message);
}

JSONParser::server_message Client::remove_message(const std::string& token, const std::string& message_id) noexcept(false)
{
    JSONParser::client_message message = {
            token,
            4,
            message_id
    };
    return send_and_receive(message);
}
