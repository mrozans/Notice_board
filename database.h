#ifndef TIN_TABLICA_OGLOSZEN_DATABASE_H
#define TIN_TABLICA_OGLOSZEN_DATABASE_H
#include <pqxx/pqxx>
#include <iostream>
#include <utility>

class Database
{
private:
    std::string connection_string;
public:
    explicit Database(std::string connection_string);

    pqxx::result select_all(const std::string& table);

    pqxx::result select_with_specified_attribute(const std::string& table, const std::string& attribute, const std::string& value);

    int insert(const std::string& table, std::vector<std::string> attributes, std::vector<std::string> values);

    int delete_record(const std::string& table, const std::string& id);

    int delete_record_form_intersection_table(const std::string& category_id, const std::string& user_id);
};
#endif