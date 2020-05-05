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
    clientMessage.token = "";
    clientMessage.code = 0;
    clientMessage.body = std::move(message_body);
    return clientMessage;
}

JSONParser::server_message MessageHandler::remove_message()
{
    //ToDo
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
    struct s1{
        int f1;
        std::string f2;
        float f3;
    };
    s1 q;
    try{
        nlohmann::json j = nlohmann::json::parse(client_message.body);
        q = s1{
                j["f1"].get<int>(),
                j["f2"].get<std::string>(),
                j["f3"].get<float>()
        };
    } catch (const std::exception& e) {
        this->server_message.code = 0;
        this->server_message.body = e.what();
        return this->server_message;
    }

    //q.f1;
    //ToDo
    this->server_message.code = 1;
    this->server_message.body = "";
    return this->server_message;
}

JSONParser::server_message MessageHandler::client_authorization()
{
    this->server_message.code = 1;
    this->server_message.body = database.select_user_where_fingerprint(this->client_message.token);
    return this->server_message;
}

JSONParser::client_message MessageHandler::confirm_message()
{
    //ToDo
    this->client_message.code = 1;
    this->client_message.body = "";
    this->client_message.token = "";
    return this->client_message;
}