/**
 * @authors Tomasz Jóźwik, Marcin Różański
 *
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <utility>
#include "client.h"

void handle_request(int argc, char** argv, std::shared_ptr<spdlog::logger> logger);

void handle_request(int argc, char** argv, std::shared_ptr<spdlog::logger> logger)
{
    //create new connection
    auto token = "a8:9f:67:d3:ee:08:90:3c:5c:7d:a6:4b:23:e5:7f:98:c6:f6:94:bd"; //todo

    JSONParser::server_message server_response{};
    try
    {
        auto client = Client(argv[2], std::stoi(argv[3]), 5, std::move(logger));

        switch (argc)
        {
            case 4:
                if(std::string(argv[1]) == "0")
                {
                    // database updater mode
                }
                else if(std::string(argv[1]) == "1")
                {
                    server_response = client.authorization(token);
                }
                else
                    throw std::logic_error("Invalid arguments");
                break;
            case 5:
                if(std::string(argv[1]) == "2")
                {
                    server_response = client.remove_message(token, argv[4]);
                }
                else
                    throw std::logic_error("Invalid arguments");
                break;
            case 8:
                if(std::string(argv[1]) == "3")
                {
                    server_response = client.create_new_message(token, argv[4], argv[5], argv[6], argv[7]);
                }
                else
                    throw std::logic_error("Invalid arguments");
                break;
            default:
                throw std::logic_error("Invalid arguments");
        }

        std::cout << server_response.body << " " << server_response.code << std::endl;
    }
    catch (const std::exception &e)
    {
        logger->error(e.what());
    }
}

int main(int argc, char *argv[])
{
    if(argc < 4)
    {
        throw std::logic_error("Invalid arguments");
    }

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

    handle_request(argc, argv, logger);
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

JSONParser::server_message Client::create_new_message(const std::string& token, const std::string& cid, const std::string &title, const std::string &content, const std::string &days) noexcept(false)
{
    JSONParser::message_container container = {
            cid,
            title,
            content,
            days
    };

    auto container_dump = JSONParser::generate_message_container(container);

    JSONParser::client_message message = {
            token,
            2,
            container_dump
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
