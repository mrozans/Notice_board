/**
 * @authors Tomasz Mazur, Marcin Mozolewski
 *
 */
#include "Message.h"

#include <nlohmann/json.hpp>

JSONParser::server_message Message::run()
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

JSONParser::server_message Message::server_error_message(std::string message_body)
{
    JSONParser::server_message serverMessage = {};
    serverMessage.code = 0;
    serverMessage.body = std::move(message_body);
    return serverMessage;
}

JSONParser::server_message Message::remove_message()
{
    this->server_message.code = 1;
    this->server_message.body = database.delete_message_with_id(this->client_message.body, this->client_message.token);
    return this->server_message;
}

JSONParser::server_message Message::get_new_messages()
{
    //ToDo
    this->server_message.code = 1;
    this->server_message.body = "";
    return this->server_message;
}

JSONParser::server_message Message::create_new_message()
{
    JSONParser::message_container message_container;
    try {
        nlohmann::json j = nlohmann::json::parse(client_message.body);
        message_container = JSONParser::message_container {
                j["cid"].get<std::string>(),
                j["title"].get<std::string>(),
                j["content"].get<std::string>(),
                j["days"].get<std::string>()
        };
    } catch (const std::exception& e) {
        this->server_message.code = 0;
        this->server_message.body = e.what();
        return this->server_message;
    }

    this->server_message.code = 1;
    this->server_message.body = database.insert_into_messages(message_container.cid, message_container.title, message_container.content, message_container.days);
    return this->server_message;
}

JSONParser::server_message Message::client_authorization()
{
    this->server_message.code = 1;
    this->server_message.body = database.select_user_where_fingerprint(this->client_message.token);
    return this->server_message;
}