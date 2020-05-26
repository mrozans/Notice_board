/**
 * @authors Tomasz Mazur, Marcin Mozolewski
 * @date 20.05.2020
 * @name Tablica ogłoszeń
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
        case 5:
            return get_new_category();
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

bool Message::prepare_stored_message()
{
    if(this->client_message.body != "-1")
    {
        database.delete_pending_change(this->client_message.body, this->client_message.token);
    }

    auto client_id = database.select_client_id_where_fingerprint(this->client_message.token);

    if(client_id.empty())
        return false;
    else
    {
        auto vector_of_change = (database.select_messages_info(client_id, true));
        if(vector_of_change.empty())
        {
            // No request available. Everything is up to date.
            this->server_message.code = 1;
            this->server_message.body = "";
            return true;
        }
        else if(vector_of_change[0].state)
        {
            // Message deletion request
            JSONParser::message_transfer_container container = {
                    vector_of_change[0].message_id,
                    "",
                    "",
                    "",
                    vector_of_change[0].id
            };
            this->server_message.code = 2;
            this->server_message.body = JSONParser::generate_message_transfer_container(container);
        }
        else
        {
            // Message creation request
            auto message = database.select_message_where_id(vector_of_change[0].message_id);

            if(message.category_id.empty())
                return false;

            auto category_name = database.select_category_where_id(message.category_id);

            JSONParser::message_transfer_container container = {
                    vector_of_change[0].message_id,
                    category_name,
                    message.title,
                    message.content,
                    vector_of_change[0].id
            };

            this->server_message.code = 3;
            this->server_message.body = JSONParser::generate_message_transfer_container(container);
        }
    }

    return true;
}

bool Message::prepare_stored_category()
{
    if(this->client_message.body != "-1")
    {
        database.delete_pending_category(this->client_message.body, this->client_message.token);
    }

    auto client_id = database.select_client_id_where_fingerprint(this->client_message.token);

    if(client_id.empty())
        return false;
    else
    {
        auto vector_of_change = (database.select_categories_info(client_id));
        if(vector_of_change.empty())
        {
            // No request available. Everything is up to date.
            this->server_message.code = 1;
            this->server_message.body = "";
            return true;
        }
        else if(vector_of_change[0].state)
        {
            // Category deletion request
            JSONParser::message_transfer_container container = {
                    vector_of_change[0].message_id,
                    "",
                    "",
                    "",
                    vector_of_change[0].id
            };
            this->server_message.code = 2;
            this->server_message.body = JSONParser::generate_message_transfer_container(container);
        }
        else
        {
            // Message creation request
            auto category = database.select_category_where_id(vector_of_change[0].message_id);


            JSONParser::message_transfer_container container = {
                    vector_of_change[0].message_id,
                    category,
                    "",
                    "",
                    vector_of_change[0].id
            };

            this->server_message.code = 3;
            this->server_message.body = JSONParser::generate_message_transfer_container(container);
        }
    }

    return true;
}

JSONParser::server_message Message::get_new_messages()
{
    if(!prepare_stored_message())
    {
        this->server_message.code = 0;
        this->server_message.body = "Message transaction is not valid!";
    }
    return this->server_message;
}

JSONParser::server_message Message::get_new_category()
{
    if(!prepare_stored_category())
    {
        this->server_message.code = 0;
        this->server_message.body = "Category transaction is not valid!";
    }
    return this->server_message;
}

JSONParser::server_message Message::create_new_message()
{
    JSONParser::message_container message_container;
    try {
        nlohmann::json j = nlohmann::json::parse(client_message.body);
        message_container = JSONParser::message_container {
                j["category"].get<std::string>(),
                j["title"].get<std::string>(),
                j["content"].get<std::string>(),
                j["days"].get<std::string>()
        };
    } catch (const std::exception& e) {
        this->server_message.code = 0;
        this->server_message.body = e.what();
        return this->server_message;
    }

    auto category_id = database.select_category_by_name(message_container.category);

    if(category_id.empty())
    {
        this->server_message.code = 1;
        this->server_message.body = "Category not found!";
        return this->server_message;
    }

    database.insert_into_messages(category_id, message_container.title, message_container.content, message_container.days);
    this->server_message.body = "1";
    return this->server_message;
}

JSONParser::server_message Message::client_authorization()
{
    this->server_message.code = 1;
    this->server_message.body = database.update_hostname_where_fingerprint(this->client_message.token, this->client_message.body);
    return this->server_message;
}