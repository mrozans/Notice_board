/**
 * @authors Tomasz Mazur, Marcin Mozolewski
 * @date 04.05.2020
 * @name Tablica ogłoszeń
 */
#include "JSONParser.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::string JSONParser::generate_message_container(JSONParser::message_container container)
{
    json j;
    j["category"] = container.category;
    j["title"] = container.title;
    j["content"] = container.content;
    j["days"] = container.days;
    return j.dump();
}

std::string JSONParser::generate_message_transfer_container(JSONParser::message_transfer_container container)
{
    json j;
    j["id"] = container.id;
    j["category"] = container.category;
    j["title"] = container.title;
    j["content"] = container.content;
    j["rid"] = container.rid;
    return j.dump();
}

std::string JSONParser::generate_client_message(JSONParser::client_message message)
{
    json j;
    j["token"] = message.token;
    j["code"] = message.code;
    j["body"] = message.body;
    return j.dump();
}

std::string JSONParser::generate_server_message(JSONParser::server_message message)
{
    json j;
    j["code"] = message.code;
    j["body"] = message.body;
    return j.dump();
}

JSONParser::message_transfer_container JSONParser::get_message_transfer_container(const std::string& container)
{
    json j = json::parse(container);
    return {
            j["id"].get<std::string>(),
            (j["category"].get<std::string>()),
            j["title"].get<std::string>(),
            j["content"].get<std::string>(),
            j["rid"].get<std::string>()
    };
}

JSONParser::client_message JSONParser::get_client_message(const std::string& message)
{

    json j = json::parse(message);
    return {
            j["token"].get<std::string>(),
            (j["code"].get<unsigned short>()),
            j["body"].get<std::string>()
    };
}

JSONParser::server_message JSONParser::get_server_message(const std::string& message)
{
    json j = json::parse(message);
    return {
            (j["code"].get<unsigned short>()),
            j["body"].get<std::string>()
    };
}
