/**
 * @authors Tomasz Mazur, Marcin Mozolewski
 *
 */

#ifndef TIN_TABLICA_OGLOSZEN_MESSAGE_H
#define TIN_TABLICA_OGLOSZEN_MESSAGE_H

#include <utility>

#include "JSONParser.h"
#include "database.h"

class Message {
private:
    JSONParser::server_message server_message;
    JSONParser::client_message client_message;
    Database database;
public:
    explicit Message(JSONParser::client_message message, Database database): client_message(std::move(message)), database(std::move(database))
    {
        server_message = {};
    };

    JSONParser::server_message run();

    static JSONParser::server_message server_error_message(std::string message_body);
private:
    JSONParser::server_message client_authorization();
    JSONParser::server_message create_new_message();
    JSONParser::server_message get_new_messages();
    JSONParser::server_message remove_message();
    bool prepare_stored_message();
};

#endif //TIN_TABLICA_OGLOSZEN_MESSAGE_H
