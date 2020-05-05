#include "database.h"
#include <vector>
#include <utility>
#include <pqxx/pqxx>

Database::Database(std::string connection_string)
{
    this->connection_string = std::move(connection_string);
}

pqxx::result Database::select_all(const std::string& table)
{
    try
    {
        pqxx::connection C(connection_string);
        std::string sql = "SELECT * FROM " + table;
        pqxx::nontransaction N(C);
        pqxx::result R(N.exec(sql));
        C.disconnect();
        return R;
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what()<<std::endl;
        pqxx::result R;
        return R;
    }
}

pqxx::result Database::select_with_specified_attribute(const std::string& table, const std::string& attribute, const std::string& value, bool key)
{
    try
    {
        pqxx::connection C(connection_string);
        std::string sql = "SELECT * FROM " + table + " WHERE " + attribute + " = ";
        if(key)
        {
            sql += "'";
            sql += value;
            sql += "'";
        }
        else sql += value;
        pqxx::nontransaction N(C);
        pqxx::result R(N.exec(sql));
        C.disconnect();
        return R;
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what()<<std::endl;
        pqxx::result R;
        return R;
    }
}

std::string Database::update(const std::string& table, const std::string& attribute, const std::string& where_attribute, const std::string& value, const std::string& new_value)
{
    try
    {
        pqxx::connection C(connection_string);
        std::string sql = "UPDATE " + table + " SET " + attribute + " = '" + new_value + "' WHERE " + where_attribute + " = '" + value + "';";
        pqxx::work W(C);
        W.exec(sql);
        W.commit();
        C.disconnect();
        return "1";
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what()<<std::endl;
        return "0";
    }
}

std::string Database::insert(const std::string& table, std::vector<std::string> attributes, std::vector<std::pair<std::string, bool>> values)
{
    try
    {
        pqxx::connection C(connection_string);
        if(attributes.size() != values.size()) return std::to_string(1);
        std::string sql1, sql2;
        for(unsigned int i = 0; i < attributes.size(); i++)
        {
            if(i != 0)
            {
                sql1 += ",";
                sql2 += ",";
            }
            sql1 += attributes[i];
            if(values[i].second)
            {
                sql2 += "'";
                sql2 += values[i].first;
                sql2 += "'";
            }
            else sql2 += values[i].first;
        }
        std::string sql = "INSERT INTO " + table + " (" + sql1 + ") " + "VALUES (" + sql2 + ");";
        pqxx::work W(C);
        W.exec(sql);
        W.commit();
        C.disconnect();
        return "1";
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what()<<std::endl;
        return "0";
    }
}

std::string Database::delete_record(const std::string& table, const std::string& id)
{
    try
    {
        pqxx::connection C(connection_string);
        std::string sql = "DELETE FROM " + table + " WHERE id = " + id;
        pqxx::work W(C);
        W.exec(sql);
        W.commit();
        C.disconnect();
        return "1";
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what()<<std::endl;
        return "0";
    }
}

std::string Database::delete_record_form_intersection_table(const std::string& category_id, const std::string& user_id)
{
    try
    {
        pqxx::connection C(connection_string);
        std::string sql = "DELETE FROM categories_users WHERE category_id = " + category_id + " AND user_id = " + user_id;
        pqxx::work W(C);
        W.exec(sql);
        W.commit();
        C.disconnect();
        return "1";
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what()<<std::endl;
        return "0";
    }
}

std::string Database::select_ip_where_fingerprint(const std::string& fingerprint)
{
    pqxx::result R = select_with_specified_attribute("clients", "fingerprint", fingerprint, true);
    pqxx::result::const_iterator c = R.begin();
    std::string result = c[2].as<std::string>();
    return result;
}

std::string Database::select_public_key_where_fingerprint(const std::string& fingerprint)
{
    pqxx::result R = select_with_specified_attribute("clients", "fingerprint", fingerprint, true);
    pqxx::result::const_iterator c = R.begin();
    std::string result = c[5].as<std::string>();
    return result;
}

std::vector<std::string> Database::select_ip_where_category(const std::string& category_name)
{
    try
    {
        pqxx::connection C(connection_string);
        std::string sql = "SELECT ip_address FROM clients c INNER JOIN categories_users cu ON cu.user_id = c.user_id INNER JOIN categories ca ON ca.id = cu.category_id WHERE ca.name = '" + category_name + "';";
        pqxx::nontransaction N(C);
        pqxx::result R(N.exec(sql));
        C.disconnect();
        std::vector<std::string> result;
        for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c)
        {
            if(c[0].is_null()) continue;
            result.insert(result.end(), c[0].as<std::string>());
        }
        return result;
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what()<<std::endl;
        std::vector<std::string> result;
        return result;
    }
}

std::string Database::select_user_where_fingerprint(const std::string& fingerprint)
{
    try
    {
        pqxx::connection C(connection_string);
        std::string sql = "SELECT users.email FROM users INNER JOIN clients ON users.id = clients.user_id WHERE clients.fingerprint='" + fingerprint + "';";
        pqxx::nontransaction N(C);
        pqxx::result R(N.exec(sql));
        C.disconnect();
        std::string result;
        for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c)
        {
            result = c[0].as<std::string>();
        }
        return result;
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what()<<std::endl;
        std::string result = "0";
        return result;
    }
}

std::string Database::select_owner_email_where_message_id(const std::string& message_id)
{
    try
    {
        pqxx::connection C(connection_string);
        std::string sql = "SELECT u.email FROM users u INNER JOIN categories ca ON u.id = ca.owner_id INNER JOIN messages m ON m.category_id = ca.id WHERE m.id = '" + message_id + "';";
        pqxx::nontransaction N(C);
        pqxx::result R(N.exec(sql));
        C.disconnect();
        std::string result;
        for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c)
        {
            result = c[0].as<std::string>();
        }
        return result;
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what()<<std::endl;
        std::string result = "0";
        return result;
    }
}

std::vector<message> Database::select_messages_where_category(const std::string& category_id)
{
    try
    {
        pqxx::connection C(connection_string);
        std::string sql = "SELECT * FROM messages WHERE category_id = '" + category_id + "' AND ( expires_at >= CURRENT_TIMESTAMP OR expires_at is null);";
        pqxx::nontransaction N(C);
        pqxx::result R(N.exec(sql));
        C.disconnect();
        std::vector<message> result;
        for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c)
        {
            message m;
            m.title = c[1].as<std::string>();
            m.content = c[2].as<std::string>();
            result.insert(result.end(), m);
        }
        return result;
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what()<<std::endl;
        std::vector<message> result;
        return result;
    };

}

std::vector<name_id> Database::select_category_where_owner(const std::string& user_id)
{
    pqxx::result R = select_with_specified_attribute("categories", "owner_id", user_id, false);
    std::vector<name_id> result;
    for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c)
    {
        name_id n;
        n.name = c[1].as<std::string>();
        n.id = c[0].as<std::string>();
        result.insert(result.end(), n);
    }
    return result;
}

std::vector<name_id> Database::select_user_categories(const std::string& user_id)
{
    try
    {
        pqxx::connection C(connection_string);
        std::string sql = "SELECT ca.id, ca.name FROM categories ca INNER JOIN categories_users cu ON cu.category_id = ca.id WHERE cu.user_id = " + user_id + ";";
        pqxx::nontransaction N(C);
        pqxx::result R(N.exec(sql));
        C.disconnect();
        std::vector<name_id> result;
        for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c)
        {
            name_id n;
            n.name = c[1].as<std::string>();
            n.id = c[0].as<std::string>();
            result.insert(result.end(), n);
        }
        return result;
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what()<<std::endl;
        std::vector<name_id> result;
        return result;
    }
}
std::string Database::update_ip_where_fingerprint(const std::string& fingerprint, const std::string& ip)
{
    return update("clients", "ip_address", "fingerprint", fingerprint, ip);
}

std::string Database::insert_into_messages(const std::string& category_id, const std::string& title, const std::string& massage)
{
    std::vector<std::string> attributes;
    std::vector<std::pair <std::string, bool>> values;
    attributes.insert(attributes.begin(), "updated_at");
    attributes.insert(attributes.begin(), "created_at");
    attributes.insert(attributes.begin(), "expires_at");
    attributes.insert(attributes.begin(), "content");
    attributes.insert(attributes.begin(), "title");
    attributes.insert(attributes.begin(), "category_id");
    values.insert(values.begin(), std::make_pair("CURRENT_TIMESTAMP", false));
    values.insert(values.begin(), std::make_pair("CURRENT_TIMESTAMP", false));
    values.insert(values.begin(), std::make_pair("CURRENT_TIMESTAMP + INTERVAL '7 day'", false));
    values.insert(values.begin(), std::make_pair(massage, true));
    values.insert(values.begin(), std::make_pair(title, true));
    values.insert(values.begin(), std::make_pair(category_id, false));
    return insert("messages", attributes, values);
}

std::string Database::delete_message_with_id(const std::string& id, const std::string& fingerprint)
{
    try
    {
        if(select_user_where_fingerprint(fingerprint) == select_owner_email_where_message_id(id))
            return delete_record("messages", id);
        else
            return "";
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what()<<std::endl;
        return "";
    }
}