/**
 * @authors Tomasz Mazur, Marcin Mozolewski
 *
 */
#ifndef TIN_TABLICA_OGLOSZEN_JSONPARSER_H
#define TIN_TABLICA_OGLOSZEN_JSONPARSER_H

#include <string>

class JSONParser {
public:
    struct client_message{
        std::string token;
        unsigned short code;
        std::string body;
    };
    struct server_message{
        unsigned short code;
        std::string body;
    };
    struct message_container{
        std::string cid;
        std::string title;
        std::string content;
        std::string days;
    };

    static std::string generate_message_container(message_container container);
    static std::string generate_client_message(client_message message);
    static std::string generate_server_message(server_message message);

    static client_message get_client_message(const std::string& message) noexcept(false);
    static server_message get_server_message(const std::string& message) noexcept(false);

    JSONParser() = default;
};

#endif //TIN_TABLICA_OGLOSZEN_JSONPARSER_H
