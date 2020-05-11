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
#include "client.h"

int main(int argc, char *argv[])
{
    auto client = new Client(argv[1], argv[2]);

    client->connect_to_server();
    pthread_t tid;
    if(pthread_create(&tid, nullptr, (Client::read_message), client->args) != 0)
    {
        perror("Opening listening thread");
        return 1;
    }

    std::string s = "{\"token\":\"1234\",\"code\":2,\"body\":\"{\\\"cid\\\":\\\"5\\\",\\\"title\\\":\\\"test1\\\",\\\"content\\\":\\\"test2\\\"}\"}";

    int len = (int)((int)Client::MESSAGE_MAX_LEN - 1 - std::to_string((int)s.length()).length());
    if(len < 0){
        throw std::logic_error("message is too long");
    }
    auto message_length  =std::string(len, '0') + std::to_string(s.length()+ 1) + "\n";

    std::cout<<message_length + s <<std::endl;



    client->send(const_cast<char*>((message_length + s).c_str()));
    pthread_join(tid, nullptr);
    client->disconnect();
}

int Client::connect_to_server()
{
    if(check_if_ipv6(server_name))
        return connect_to_ipv6();

    return connect_to_ipv4();
}

int Client::send(char *message)
{
    if(connect(sock, (struct sockaddr *) &server, sizeof server) == -1)
    {
        perror("Connecting stream socket");
        return 1;
    }

    if(write(sock, message, (strlen(message) + 1) * sizeof(char)) == -1)
    {
        perror("Writing on stream socket");
        return 1;
    }

    return 0;
}

int Client::disconnect() const
{
    close(sock);
    return 0;
}

void *Client::read_message(void *voidArgs)
{
    auto *args = (thread_args *) voidArgs;
    const int size = 4000;
    char *buffer = new char[size];
    for (int i = 0; i < size; ++i)
    {
        buffer[i] = '\0';
    }
    time_t current_time = time(nullptr);
    while(true)
    {
        if(read(args->socket, buffer, size) > 1)
        {
            printf("%s\n", buffer);
            break;
        }
        if(time(nullptr) - current_time > 10)
        {
            printf("no confirmation form server\n");
            break;
        }
    }
    delete[] buffer;
    delete args;
    return 0;
}

bool Client::check_if_ipv6(const char *ip)
{
    for(int i = 0; ip[i] != '\0'; i++)
        if(ip[i] == ':') return true;

    return false;
}

int Client::connect_to_ipv4()
{
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock == -1)
    {
        perror("String socket can't be opened!");
        return 1;
    }

    args = new thread_args;
    args->socket = sock;

    server.ipv4.sin_family = AF_INET;

    hp = gethostbyname2(server_name, AF_INET);

    if(hp == (struct hostent *) nullptr)
    {
        std::cout << "Unknown host: " << server_name << std::endl;
        return 1;
    }


    memcpy((char *) &server.ipv4.sin_addr, (char *) hp->h_addr, hp->h_length);

    server.ipv4.sin_port = htons(atoi(port));

    return 0;
}

int Client::connect_to_ipv6()
{
    sock = socket(AF_INET6, SOCK_STREAM, 0);

    if(sock == -1)
    {
        perror("String socket can't be opened!");
        return 1;
    }

    args = new thread_args;
    args->socket = sock;

    server.ipv6.sin6_family = AF_INET6;

    hp = gethostbyname2(server_name, AF_INET6);

    if(hp == (struct hostent *) nullptr)
    {
        std::cout << "Unknown host: " << server_name << std::endl;
        return 1;
    }

    memcpy((char *) &server.ipv6.sin6_addr, (char *) hp->h_addr, hp->h_length);

    server.ipv6.sin6_port = htons(atoi(port));

    return 0;
}

int Client::send_to_ipv4(char *message)
{
    if(connect(sock, (struct sockaddr *) &server.ipv4, sizeof server.ipv4) == -1)
    {
        perror("Connecting stream socket");
        return 1;
    }

    return send_message(message);
}

int Client::send_to_ipv6(char *message)
{
    if(connect(sock, (struct sockaddr *) &server.ipv6, sizeof server.ipv6) == -1)
    {
        perror("Connecting stream socket");
        return 1;
    }

    return send_message(message);
}

int Client::send_message(char *message) const
{
    if(write(sock, message, (strlen(message) + 1) * sizeof(char)) == -1)
    {
        perror("Writing on stream socket");
        return 1;
    }

    return 0;
}