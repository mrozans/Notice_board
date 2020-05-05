/**
 * @authors Tomasz Mazur, Marcin Mozolewski
 *
 */
#include <string>
#include <utility>
#include <sys/socket.h>
#include "RequestHandler.h"

RequestHandler::RequestHandler(int socket, std::shared_ptr<spdlog::logger> logger, int timeout_value) : socket(socket), logger(std::move(logger))
{
    timeout.tv_sec = timeout_value;
    if(setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout)) < 0)
    {
        this->logger->error("socket timeout init");
    }
}

void RequestHandler::send_message(char *message, size_t message_size) const
{
    ssize_t ret = send(socket, message, message_size, 0);
    if (ret == -1)
    {
        throw std::logic_error("send function returned: " + std::to_string(ret));
    }
    logger->info("message sent: " + std::string(message));
}

char *RequestHandler::read_message()
{
    ssize_t input_message_len_size = recv(socket, input_message_len, INPUT_MESSAGE_LEN, 0);

    if(input_message_len_size != INPUT_MESSAGE_LEN)
    {
        throw std::logic_error("message size read returned: " + std::to_string(input_message_len_size));
    }

    logger->info("received message size: " + std::string(input_message_len));

    unsigned int message_len = str_to_int(input_message_len) + 1;

    if(message_len <= 1)
    {
        throw std::logic_error("message_len must be > 1 when is: " + std::to_string(message_len));
    }
    char * message = new char[message_len];

    memset(message, 0, message_len * sizeof(*message));

    ssize_t received_message_size = recv(socket, message, message_len, 0);

    if(received_message_size != message_len - 1)
    {
        throw std::logic_error("received message size: " + std::to_string(received_message_size) +
        " ,when expecting: " + std::to_string(message_len - 1));
    }

    logger->info("received message: " + std::string(message));

    return message;
}

unsigned int RequestHandler::str_to_int(char *buff)
{
    return (unsigned int) std::stoul(buff, nullptr, 10);
}

void RequestHandler::send_message(const std::string& message) const noexcept(false)
{
    int len = (int)((int)INPUT_MESSAGE_LEN - 1 - std::to_string((int)message.length()).length());
    if(len < 0){
        throw std::logic_error("message is too long");
    }
    auto message_length  = std::string(len, '0') + std::to_string(message.length()) + "\0";

    std::string message_to_client = message_length + message;

    send_message(const_cast<char*>(message_to_client.c_str()), message_to_client.length());
}

void RequestHandler::send_message(JSONParser::server_message message) const
{
    send_message(JSONParser::generate_server_message(std::move(message)));
}
