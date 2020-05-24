/**
 * @authors Tomasz Jóźwik, Marcin Różański
 *
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "client.h"
#include "database.h"

void get_new_content(const std::string& token, std::shared_ptr<spdlog::logger> logger, const std::string& server_name, const uint16_t& server_port, std::string content_type) noexcept(false);

void get_new_content(const std::string& token, std::shared_ptr<spdlog::logger> logger, const std::string& server_name, const uint16_t& server_port, std::string content_type) noexcept(false)
{
    std::string body = "-1";

    while(true)
    {
        JSONParser::server_message response;
        JSONParser::client_message message;
        if(content_type == "messages")
        {
            message = {
                    token,
                    3,
                    body
            };
        }
        else
        {
            message = {
                    token,
                    5,
                    body
            };
        }

        try{
            auto client = Client(server_name.c_str(), server_port, 5, logger);
            response = client.send_and_receive(message);
        } catch (const std::exception &e) {
            logger->critical(e.what());
            response={
                    0,
                    e.what()
            };
        }

        JSONParser::message_transfer_container message_transfer_container;

        switch(response.code)
        {
            case 0:
                logger->error("Server error!");
                return;
            case 1:
                try
                {
                    logger->info("Update completed successfully!");
                    return;
                }
                catch (const std::exception &e)
                {
                    logger->error("Error occurred while closing the updater!");
                    return;
                }
            case 2:
                try
                {
                    message_transfer_container = JSONParser::get_message_transfer_container(response.body);

                    if(database.delete_local_record_with_id(content_type, message_transfer_container.id) == "-1")
                        throw std::logic_error("Data processing error! Data can't be deleted.");
                }
                catch (const std::exception &e)
                {
                    logger->error("Data processing error! Data can't be deleted.");
                    return;
                }
                break;
            case 3:
                try
                {
                    message_transfer_container = JSONParser::get_message_transfer_container(response.body);
                    if(content_type == "messages")
                    {
                        if(database.select_message_where_id(message_transfer_container.id).category_id.empty())
                        {
                            if(database.insert_local_message(message_transfer_container.id, message_transfer_container.category,
                                                             message_transfer_container.title, message_transfer_container.content) == "-1")
                                throw std::logic_error("Message processing error! Message can't be inserted.");
                        }
                    }
                    else
                    {
                        if(database.select_category_where_id(message_transfer_container.id).empty())
                        {
                            if(database.insert_local_category(message_transfer_container.id, message_transfer_container.category) == "-1")
                                throw std::logic_error("Category processing error! Category can't be inserted.");
                        }
                    }
                }
                catch (const std::exception &e)
                {
                    logger->error("Message processing error! Message can't be inserted.");
                    return;
                }
        }

        body = message_transfer_container.rid;

        std::cout << response.body << " " << response.code << std::endl;
    }
}

void process_requests(const std::string& token, std::shared_ptr<spdlog::logger> logger, const std::string& server_name, const uint16_t& server_port) noexcept(false);

void process_requests(const std::string& token, std::shared_ptr<spdlog::logger> logger, const std::string& server_name, const uint16_t& server_port) noexcept(false)
{
    auto request = database.select_local_request();

    while(!request.empty())
    {
        auto client = Client(server_name.c_str(), server_port, 5, logger);

        JSONParser::server_message server_response{};
        try
        {
            if(request[1] == "1" && request.size() == 2)
            {
                server_response = client.authorization(token);
            }
            else if(request[1] == "2" && request.size() == 3)
            {
                server_response = client.remove_message(token, request[2]);
            }
            else if(request[1] == "3" && request.size() == 6)
            {
                // category, title, content, days of validity
                server_response = client.create_new_message(token, request[2], request[3], request[4], request[5]);
            }
            else
            {
                database.delete_local_record_with_id("requests", request[0]);
                throw std::logic_error("Request contained invalid arguments");
            }

            database.delete_local_record_with_id("requests", request[0]);

            request = database.select_local_request();
        }
        catch (const std::exception &e)
        {
            logger->error(e.what());
            return;
        }
    }
}

void handle_requests(std::shared_ptr<spdlog::logger> logger);

void handle_requests(std::shared_ptr<spdlog::logger> logger)
{
    auto token = "45:63:f1:fc:a7:6f:33:55:62:e0:10:60:b4:c6:f9:1a:5f:cc:e5:0d"; //todo

    std::string server_name = getenv("SERVER_NAME") ? getenv("SERVER_NAME") : "127.0.0.1",
        server_port = getenv("SERVER_PORT") ? getenv("SERVER_PORT") : "57076";

    while(true)
    {
        std::thread t1(get_new_content, token, logger, server_name, stoi(server_port), "categories");
        std::thread t2(process_requests, token, logger, server_name, stoi(server_port));
        std::thread t3(get_new_content, token, logger, server_name, stoi(server_port), "messages");

        t1.join();
        t2.join();
        t3.join();

        sleep(2);
    }
}

int main(int argc, char *argv[])
{
    std::string db_name = getenv("DATABASE_NAME") ? getenv("DATABASE_NAME") : "postgres",
            db_user = getenv("DATABASE_USER") ? getenv("DATABASE_USER") : "postgres",
            db_password = getenv("DATABASE_PASSWORD") ? getenv("DATABASE_PASSWORD") : "docker",
            db_host = getenv("DATABASE_HOST") ? getenv("DATABASE_HOST") : "0.0.0.0",
            db_port = getenv("DATABASE_PORT") ? getenv("DATABASE_PORT") : "5432";

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

    auto db_host_hostent = gethostbyname2(db_host.c_str(), AF_INET);
    if(db_host_hostent == nullptr)
    {
        logger->critical("Cannot connect to database! Unknown host");
        return 1;
    }
    std::string db_host_ip = inet_ntoa(*((struct in_addr*) db_host_hostent->h_addr_list[0]));

    try
    {
        database = Database("dbname = " + db_name + " user = " + db_user + " password = " + db_password + " hostaddr = " + db_host_ip + " port = " + db_port, logger);
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what()<<std::endl;
        return 1;
    }

    handle_requests(logger);
}

void Client::connect_to_server()
{
    if(check_if_ipv6(server_name))
    {
        connect_to_ipv6();
    }
    else
    {
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
    char hostname[256];
    gethostname(hostname, 256);

    JSONParser::client_message message = {
            token,
            1,
            std::string(hostname)
    };
    return send_and_receive(message);
}

JSONParser::server_message Client::create_new_message(const std::string& token, const std::string& category, const std::string &title, const std::string &content, const std::string &days) noexcept(false)
{
    JSONParser::message_container container = {
            category,
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

JSONParser::server_message Client::remove_message(const std::string& token, const std::string& message_id) noexcept(false)
{
    JSONParser::client_message message = {
            token,
            4,
            message_id
    };
    return send_and_receive(message);
}
