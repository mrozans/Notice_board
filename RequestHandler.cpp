#include <cstdio>
#include <sys/socket.h>
#include "RequestHandler.h"

RequestHandler::RequestHandler(int socket) : socket(socket) {}

void RequestHandler::send_message(const char *message, size_t message_size) const
{
    ssize_t ret = send(socket, message, message_size, 0);
    if (ret == -1)
    {
        throw "send error";
    }
}

char* RequestHandler::read_message()
{
    ssize_t received_message_size = recv(socket, input_message, INPUT_MESSAGE_SIZE, 0);
    if (received_message_size > INPUT_MESSAGE_SIZE || received_message_size == -1)
    {
        perror("received_message_error");
    }
    return input_message;
}

char  * RequestHandler::send_and_receive_message(const char *message, size_t message_size) {
    send_message(message, message_size);
    return read_message();
}
