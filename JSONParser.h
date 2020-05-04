#ifndef TIN_TABLICA_OGLOSZEN_JSONPARSER_H
#define TIN_TABLICA_OGLOSZEN_JSONPARSER_H

#include <string>

class JSONParser {
public:
    struct client_message{
        std::string token;
        std::string code;
        std::string body;
    };
    struct server_message{
        std::string respCode;
        std::string body;
    };

    static std::string generate_client_message(client_message message);
    static std::string generate_server_message(server_message message);

    static client_message get_client_message(const std::string& message) noexcept(false);
    static server_message get_server_message(const std::string& message) noexcept(false);

    JSONParser() = default;
};

#endif //TIN_TABLICA_OGLOSZEN_JSONPARSER_H
