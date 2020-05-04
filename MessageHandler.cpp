/**
 * @authors Tomasz Mazur, Marcin Mozolewski
 *
 */
#include "MessageHandler.h"

#include <utility>

JSONParser::server_message MessageHandler::run_as_server()
{
    switch (client_message.code) {
        case 0:
            return server_error_message("got error code in input message");
        case 1:
            return client_authorization();
        case 2:
            return create_new_message();
        case 3:
            return get_new_messages();
        case 4:
            return remove_message();
        default:
            return server_error_message("client code is unknown");
    }
}

JSONParser::client_message MessageHandler::run_as_client()
{
    switch (server_message.code) {
        case 0:
            return client_error_message("error code");
        case 1:
            return confirm_message();
        default:
            return client_error_message("error_code");
    }
}

JSONParser::server_message MessageHandler::server_error_message(std::string message_body)
{
    JSONParser::server_message serverMessage = {};
    serverMessage.code = 0;
    serverMessage.body = std::move(message_body);
    return serverMessage;
}

JSONParser::client_message MessageHandler::client_error_message(std::string message_body)
{
    JSONParser::client_message clientMessage = {};
    clientMessage.code = 0;
    clientMessage.body = std::move(message_body);
    return clientMessage;
}

JSONParser::server_message MessageHandler::remove_message()
{
    //ToDo
    return this->server_message;
}

JSONParser::server_message MessageHandler::get_new_messages()
{
    //ToDo
    return this->server_message;
}

JSONParser::server_message MessageHandler::create_new_message()
{
    //ToDo
    return this->server_message;
}

JSONParser::server_message MessageHandler::client_authorization()
{
    //ToDo
    return this->server_message;
}

JSONParser::client_message MessageHandler::confirm_message()
{
    return this->client_message;
}