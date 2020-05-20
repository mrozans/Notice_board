/**
 * @authors Tomasz Jóźwik, Marcin Różański
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <cstdio>
#include <utility>
#include "client.h"
#include "JSONParser.h"
#include "RequestHandler.h"
void example_client(char const *server_name, char const *port, const std::string& token, unsigned short code, std::string body);

void example_client(char const *server_name, char const *port, const std::string& token, unsigned short code, std::string body)
{
    auto client = Client(server_name, port, 5);
    client.connect_to_server();
    //set fields
    JSONParser::client_message clientMessage;
    clientMessage.body = std::move(body);
    clientMessage.code = code;
    clientMessage.token = token;
    //send message
    client.send(clientMessage);
    //get response
    //auto server_response = client.read_message()
}

int main(int argc, char *argv[])
{
    example_client("127.0.0.1", "57076", "token", 1, "body");
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

void Client::disconnect() const
{
    close(sock);
}

void Client::read_message(void *voidArgs)
{
//    auto *args = (thread_args *) voidArgs;
//    const int size = 4000;
//    char *buffer = new char[size];
//    for (int i = 0; i < size; ++i)
//    {
//        buffer[i] = '\0';
//    }
//    time_t current_time = time(nullptr);
//    while(true)
//    {
//        if(read(args->socket, buffer, size) > 1)
//        {
//            printf("%s\n", buffer);
//            break;
//        }
//        if(time(nullptr) - current_time > 10)
//        {
//            printf("no confirmation form server\n");
//            break;
//        }
//    }
//    delete[] buffer;
//    delete args;
//    return nullptr;
}

bool Client::check_if_ipv6(const char *ip)
{
    for(int i = 0; ip[i] != '\0'; i++){
        if(ip[i] == ':')
            return true;
    }
    return false;
}

void Client::connect_to_ipv4()
{
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock == -1)
    {
        perror("String socket can't be opened!");
        //todo logger
    }

    server.ipv4.sin_family = AF_INET;

    hp = gethostbyname2(server_name, AF_INET);

    if(hp == (struct hostent *) nullptr)
    {
        std::cout << "Unknown host: " << server_name << std::endl;
        //todo logger
    }


    memcpy((char *) &server.ipv4.sin_addr, (char *) hp->h_addr, hp->h_length);

    server.ipv4.sin_port = htons(atoi(port));

    if(connect(sock, (struct sockaddr *) &server.ipv4, sizeof server.ipv4) == -1)
    {
        perror("Connecting stream socket");
        //todo logger
    }
}

void Client::connect_to_ipv6()
{
    sock = socket(AF_INET6, SOCK_STREAM, 0);

    if(sock == -1)
    {
        perror("String socket can't be opened!");
        //todo logger
    }

    server.ipv6.sin6_family = AF_INET6;

    hp = gethostbyname2(server_name, AF_INET6);

    if(hp == (struct hostent *) nullptr)
    {
        std::cout << "Unknown host: " << server_name << std::endl;
        //todo logger
    }

    memcpy((char *) &server.ipv6.sin6_addr, (char *) hp->h_addr, hp->h_length);

    server.ipv6.sin6_port = htons(atoi(port));

    if(connect(sock, (struct sockaddr *) &server.ipv6, sizeof server.ipv6) == -1)
    {
        perror("Connecting stream socket");
        //todo logger
    }
}