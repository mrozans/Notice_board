/**
 * @authors Tomasz Mazur, Marcin Mozolewski
 *
 */

#ifndef TIN_TABLICA_OGLOSZEN_MESSAGEHANDLER_H
#define TIN_TABLICA_OGLOSZEN_MESSAGEHANDLER_H

#include <utility>

#include "JSONParser.h"

class MessageHandler {
private:
    JSONParser::server_message server_message;
    JSONParser::client_message client_message;
public:
    explicit MessageHandler(JSONParser::client_message message): client_message(std::move(message))
    {
        server_message = {};
    };
    explicit MessageHandler(JSONParser::server_message message): server_message(std::move(message))
    {
        client_message = {};
    };

    JSONParser::server_message run_as_server();
    JSONParser::client_message run_as_client();

    static JSONParser::server_message server_error_message(std::string message_body);
    static JSONParser::client_message client_error_message(std::string message);
private:
    JSONParser::server_message client_authorization();
    JSONParser::server_message create_new_message();
    JSONParser::server_message get_new_messages();
    JSONParser::server_message remove_message();
    JSONParser::client_message confirm_message();
};

#endif //TIN_TABLICA_OGLOSZEN_MESSAGEHANDLER_H
