#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include "client.h"

int main(int argc, char *argv[])
{
    auto client = new Client(argv[1], argv[2]);

    client->connect_to_server();
    client->send(argv[3]);
    client->disconnect();
}