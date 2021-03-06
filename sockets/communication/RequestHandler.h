/**
 * @authors Tomasz Mazur, Marcin Mozolewski
 * @date 29.04.2020
 * @name Tablica ogłoszeń
 */
#ifndef TIN_TABLICA_OGLOSZEN_REQUESTHANDLER_H
#define TIN_TABLICA_OGLOSZEN_REQUESTHANDLER_H

#include <spdlog/logger.h>
#include "JSONParser.h"

class RequestHandler{
private:
    int socket{};
    std::shared_ptr<spdlog::logger> logger;

    const static unsigned int INPUT_MESSAGE_LEN = 4;
    char input_message_len[INPUT_MESSAGE_LEN]{};
    struct timeval timeout{};

public:
    RequestHandler()= default;
    RequestHandler(int socket,std::shared_ptr<spdlog::logger> logger, int timeout);

    void send_message(JSONParser::server_message message) const;
    void send_message(JSONParser::client_message message) const;

    char *read_message() noexcept(false);

private:
    void send_message(const std::string& message) const noexcept(false);
    void send_message(char *message, size_t message_size) const noexcept(false);

    static unsigned int str_to_int(char *buff);
};
#endif //TIN_TABLICA_OGLOSZEN_REQUESTHANDLER_H
