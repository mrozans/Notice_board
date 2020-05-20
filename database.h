/**
 * @authors Tomasz Jóźwik, Marcin Różański
 *
 */

#ifndef TIN_TABLICA_OGLOSZEN_DATABASE_H
#define TIN_TABLICA_OGLOSZEN_DATABASE_H
#include <pqxx/pqxx>
#include <iostream>
#include <utility>
#include <spdlog/logger.h>

struct name_id
{
    std::string name;
    std::string id;
};

struct message
{
    std::string title;
    std::string content;
};

class Database
{
private:
    std::string connection_string;

    std::shared_ptr<spdlog::logger> logger;

    pqxx::result select_all(const std::string& table);

    pqxx::result select_with_specified_attribute(const std::string& table, const std::string& attribute, const std::string& value, bool key);

    std::string update(const std::string& table, const std::string& attribute, const std::string& where_attribute, const std::string& value, const std::string& new_value);

    std::string insert(const std::string& table, std::vector<std::string> attributes, std::vector<std::pair<std::string, bool>> values);

    std::string delete_record(const std::string& table, const std::string& id);

    std::string delete_record_form_intersection_table(const std::string& category_id, const std::string& user_id);
public:
    Database(std::string connection_string, std::shared_ptr<spdlog::logger> logger) : connection_string(std::move(connection_string)), logger(std::move(logger)){}
    explicit Database()= default;

    std::string select_ip_where_fingerprint(const std::string& fingerprint);

    std::string select_public_key_where_fingerprint(const std::string& fingerprint);

    std::vector<std::string> select_ip_where_category(const std::string& category_name);

    std::string select_user_where_fingerprint(const std::string& fingerprint);

    std::string select_owner_email_where_message_id(const std::string& message_id);

    std::vector<message> select_messages_where_category(const std::string& category_id);

    std::vector<name_id> select_category_where_owner(const std::string& user_id);

    std::vector<name_id> select_user_categories(const std::string& user_id);

    std::string update_ip_where_fingerprint(const std::string& fingerprint, const std::string& ip);

    std::string insert_into_messages(const std::string& category_id, const std::string& title, const std::string& massage);

    std::string delete_message_with_id(const std::string& id, const std::string& fingerprint);

};
#endif