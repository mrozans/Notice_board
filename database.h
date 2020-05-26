/**
 * @authors Tomasz Jóźwik, Marcin Różański
 * @date 05.05.2020
 * @name Tablica ogłoszeń
 */

#ifndef TIN_TABLICA_OGLOSZEN_DATABASE_H
#define TIN_TABLICA_OGLOSZEN_DATABASE_H
#include <pqxx/pqxx>
#include <iostream>
#include <utility>
#include <spdlog/logger.h>

struct message
{
    std::string title;
    std::string content;
    std::string category_id;
};

struct message_info
{
    std::string id;
    std::string message_id;
    bool state;
};

class Database
{
private:
    std::string connection_string;
    std::shared_ptr<spdlog::logger> logger;
    pqxx::result select_with_specified_attribute(const std::string& table, const std::string& attribute, const std::string& value, bool key);
    std::string update(const std::string& table, const std::string& attribute, const std::string& where_attribute, const std::string& value, const std::string& new_value);
    std::string insert(const std::string& table, std::vector<std::string> attributes, std::vector<std::pair<std::string, bool>> values);
    std::string delete_record(const std::string& table, const std::string& id);

public:
    Database(std::string connection_string, std::shared_ptr<spdlog::logger> logger) : connection_string(std::move(connection_string)), logger(std::move(logger)){}
    explicit Database() = default;

    std::string select_public_key_where_fingerprint(const std::string& fingerprint);
    std::string select_client_id_where_change_id(const std::string& table, const std::string& change_id);
    std::vector<std::string> select_local_request();
    std::string select_user_where_fingerprint(const std::string& fingerprint);
    std::string select_owner_email_where_message_id(const std::string& message_id);
    std::string select_category_where_id(const std::string& id);
    std::string select_category_by_name(const std::string& id);
    std::string update_hostname_where_fingerprint(const std::string& fingerprint, const std::string& hostname);
    std::string insert_into_messages(const std::string& category_id, const std::string& title, const std::string& massage, const int& number_of_days);
    std::string insert_local_message(const std::string& id, const std::string& category, const std::string& title, const std::string& content);
    std::string insert_local_category(const std::string& id, const std::string& name);
    std::string delete_message_with_id(const std::string& id, const std::string& fingerprint);
    std::string delete_local_record_with_id(const std::string &table, const std::string& id);
    message select_message_where_id(const std::string &id);
    std::vector<message_info> select_messages_info(const std::string &client_id, bool first);
    std::vector<message_info> select_categories_info(const std::string& client_id);
    std::string delete_pending_change(const std::string& id, const std::string& fingerprint);
    std::string delete_pending_category(const std::string& id, const std::string& fingerprint);
    std::string select_client_id_where_fingerprint(const std::string& fingerprint);
    void test_connection();
};

#endif