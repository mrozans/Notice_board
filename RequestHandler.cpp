#include <cstdio>
#include <sys/socket.h>
#include "RequestHandler.h"

RequestHandler::RequestHandler(int socket, int timeout_value) : socket(socket)
{
    timeout.tv_sec = timeout_value;
    if(setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout)) < 0)
    {
        perror("socket timeout init");
    }
}

void RequestHandler::send_message(char *message, size_t message_size) const
{
    ssize_t ret = send(socket, message, message_size, 0);
    if (ret == -1) {
        perror("send");
        throw "send error";
    }
}

char *RequestHandler::read_message()
{
    ssize_t received_message_size = recv(socket, input_message, INPUT_MESSAGE_SIZE, 0);
    if (received_message_size > (int) INPUT_MESSAGE_SIZE || received_message_size == -1) {
        perror("received_message_error");
    }
    return input_message;
}

char *RequestHandler::send_and_receive_message(char *message, size_t message_size)
{
    send_message(message, message_size);
    return read_message();
}

char *RequestHandler::receive_and_send_message(char *message, size_t message_size)
{
    char *response = read_message();
    send_message(message, message_size);
    return response;
}
