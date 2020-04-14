#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string>
#include <iostream>

#define MAX_CONNECTIONS 40

void *thread1(void *arg) {
    std::cout << arg << std::endl;
    return nullptr;
}

int main(int argc, char *argv[]) {
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
    if (fp == nullptr) {
        perror("write to file");
        exit(1);
    }
    fprintf(fp, "%d", ntohs(server.sin_port));
    fclose(fp);

    if (listen(server_socket, MAX_CONNECTIONS) != 0) {
        perror("listen start");
        exit(1);
    }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

    while (true) {
        socklen_t address_size = sizeof(server_storage);
        int new_socket = accept(server_socket, (struct sockaddr *) &server_storage, &address_size);
        pthread_t thread_id;

        if (pthread_create(&thread_id, nullptr, &thread1, &new_socket) != 0) {
            perror("thread create");
            continue;
        }
    }
    return 0;
}
