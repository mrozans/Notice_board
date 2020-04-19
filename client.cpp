#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include "client.h"

int main(int argc, char *argv[])
{
    auto client = new Client(argv[1], argv[2]);

    client->connect_to_server();
    pthread_t tid;
    if(pthread_create(&tid, nullptr, client->read_message, client->args) != 0)
    {
        perror("Opening listening thread");
        return 1;
    }
    client->send(argv[3]);
    pthread_join(tid, nullptr);
    client->disconnect();
}