#include "JSONParser.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::string JSONParser::generate_client_message(JSONParser::client_message message)
{
    json j;
    j["token"] = message.token;
    j["code"] = message.code;
    j["body"] = message.body;
    return  j.dump();
}

std::string JSONParser::generate_server_message(JSONParser::server_message message)
{
    json j;
    j["respCode"] = message.respCode;
    j["body"] = message.body;
    return j.dump();
}

JSONParser::client_message JSONParser::get_client_message(const std::string& message)
{

    json j= json::parse(message);
    return {
            j["token"].get<std::string>(),
            j["code"].get<std::string>(),
            j["body"].get<std::string>()
    };
}

JSONParser::server_message JSONParser::get_server_message(const std::string& message)
{
    json j= json::parse(message);
    return {
            j["respCode"].get<std::string>(),
            j["body"].get<std::string>()
    };
}
