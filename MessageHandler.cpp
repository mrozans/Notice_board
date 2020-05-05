/**
 * @authors Tomasz Mazur, Marcin Mozolewski
 *
 */
#include "MessageHandler.h"

#include <utility>
#include <nlohmann/json.hpp>

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

JSONParser::client_message MessageHandler::run_as_client() const
{
    switch (server_message.code) {
        case 0:
            return client_error_message("error code");
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
    clientMessage.token = "";
    clientMessage.code = 0;
    clientMessage.body = std::move(message_body);
    return clientMessage;
}

JSONParser::server_message MessageHandler::remove_message()
{
    this->server_message.code = 1;
    this->server_message.body = database.delete_message_with_id(this->client_message.body, this->client_message.token);
    return this->server_message;
}

JSONParser::server_message MessageHandler::get_new_messages()
{
    //ToDo
    this->server_message.code = 1;
    this->server_message.body = "";
    return this->server_message;
}

JSONParser::server_message MessageHandler::create_new_message()
{
    struct msg {
        std::string cid;
        std::string title;
        std::string content;
    };
    msg msg_json;
    try {
        nlohmann::json j = nlohmann::json::parse(client_message.body);
        msg_json = msg {
                j["cid"].get<std::string>(),
                j["title"].get<std::string>(),
                j["content"].get<std::string>()
        };
    } catch (const std::exception& e) {
        this->server_message.code = 0;
        this->server_message.body = e.what();
        return this->server_message;
    }

    this->server_message.code = 1;
    this->server_message.body = database.insert_into_messages(msg_json.cid, msg_json.title, msg_json.content);
    return this->server_message;
}

JSONParser::server_message MessageHandler::client_authorization()
{
    this->server_message.code = 1;
    this->server_message.body = database.select_user_where_fingerprint(this->client_message.token);
    return this->server_message;
}