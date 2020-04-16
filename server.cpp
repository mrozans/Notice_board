#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string>
#include <cstring>
#include "RequestHandler.h"

#include <unistd.h>

#define MAX_CONNECTIONS 40
struct thread_args {
    int *new_socket;
};

void *handle_message(void *args);

int main(int argc, char *argv[])
{
    int server_socket, length;
    struct sockaddr_in server{};
    struct sockaddr_storage server_storage{};

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("open stream");
        exit(1);
    }

    //server config
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = 0;

    int bind_status = bind(server_socket, (struct sockaddr *) &server, sizeof(server));
    if (bind_status == -1) {
        perror("binding stream socket");
        exit(1);
    }

    length = sizeof(server);
    if (getsockname(server_socket, (struct sockaddr *) &server, (socklen_t *) (&length)) == -1) {
        perror("getting socket name");
        exit(1);
    }

    //write to file port
    FILE *fp;
    fp = fopen("port.txt", "wb");
    if (fp == nullptr)
    {
        perror("write to file");
        exit(1);
    }
    fprintf(fp, "%d", ntohs(server.sin_port));
    fclose(fp);

    if (listen(server_socket, MAX_CONNECTIONS) != 0)
    {
        perror("listen start");
        exit(1);
    }
    while (true) {
        socklen_t address_size = sizeof(server_storage);
        int *new_socket = new int;
        *new_socket = accept(server_socket, (struct sockaddr *) &server_storage, &address_size);
        auto *args = new thread_args;
        args->new_socket = new_socket;

        pthread_attr_t t_attr;
        pthread_t tid;
        int ret;
        pthread_attr_init(&t_attr);
        pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);
        ret = pthread_create(&tid, &t_attr, handle_message, args);

        if (ret != 0) {
            perror("thread create");
            continue;
        }
    }
}

void *handle_message(void *voidArgs)
{
    auto *args = (thread_args *) voidArgs;
    auto handler = RequestHandler(*args->new_socket);

    //create example message
    const int size = 40;
    char *buffer = new char[size];
    for (int i = 0; i < size; ++i)
        buffer[i] = '\0';

    strcpy(buffer, "Hello Client\n");

    //receive and send message
    char *response = handler.receive_and_send_message(buffer, size);
    printf("%s\n", response);

    //close connection
    close(*(args->new_socket));
    delete[] buffer;
    delete args->new_socket;
    delete args;

    return nullptr;
}