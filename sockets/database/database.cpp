/**
 * @authors Tomasz Jóźwik, Marcin Różański
 * @date 05.05.2020
 * @name Tablica ogłoszeń
 */

#include "database.h"

pqxx::result Database::notransaction_query(const std::string& sql)
{
    try
    {
        pqxx::connection C(connection_string);
        pqxx::nontransaction N(C);
        pqxx::result R(N.exec(sql));
        C.disconnect();
        return R;
    }
    catch(const std::exception &e)
    {
        return pqxx::result();
    }
}

std::string Database::transaction_query(const std::string& sql)
{
    try
    {
        pqxx::connection C(connection_string);
        pqxx::work W(C);
        W.exec(sql);
        W.commit();
        C.disconnect();
        return "1";
    }
    catch (const std::exception &e)
    {
        return "-1";
    }
}

pqxx::result Database::select_with_specified_attribute(const std::string& table, const std::string& attribute, const std::string& value, bool key)
{
    try
    {
        std::string sql = "SELECT * FROM " + table + " WHERE " + attribute + " = ";
        if(key)
        {
            sql += "'";
            sql += value;
            sql += "'";
        }
        else sql += value;
        pqxx::result R = notransaction_query(sql);
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
        std::string sql = "UPDATE " + table + " SET " + attribute + " = '" + new_value + "' WHERE " + where_attribute + " = '" + value + "';";
        std::string R = transaction_query(sql);
        return R;
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what()<<std::endl;
        return "-1";
    }
}

std::string Database::insert(const std::string& table, std::vector<std::string> attributes, std::vector<std::pair<std::string, bool>> values)
{
    try
    {
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
        std::string R = transaction_query(sql);
        return R;
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what()<<std::endl;
        return "-1";
    }
}

std::string Database::delete_record(const std::string& table, const std::string& id)
{
    try
    {
        std::string sql = "DELETE FROM " + table + " WHERE id = " + id;
        std::string R = transaction_query(sql);
        return R;
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what()<<std::endl;
        return "-1";
    }
}

std::string Database::select_public_key_where_fingerprint(const std::string& fingerprint)
{
    pqxx::result R = select_with_specified_attribute("clients", "fingerprint", fingerprint, true);
    pqxx::result::const_iterator c = R.begin();
    std::string result = c[5].as<std::string>();
    return result;
}

std::string Database::select_client_id_where_change_id(const std::string& table, const std::string& change_id)
{
    pqxx::result R = select_with_specified_attribute(table, "id", change_id, true);
    pqxx::result::const_iterator c = R.begin();
    std::string result = c[1].as<std::string>();
    return result;
}

std::vector<std::string> Database::select_local_request()
{
    try
    {
        std::string sql = "SELECT * FROM requests LIMIT 1";
        pqxx::result R = notransaction_query(sql);
        std::vector<std::string> result;
        for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c)
        {
            for(auto i = 0; i < 6; i++)
            {
                if(!c[i].is_null())
                {
                    result.insert(result.end(), c[i].as<std::string>());
                }
            }
        }
        return result;
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what()<<std::endl;
        return std::vector<std::string>();
    }
}

std::string Database::select_user_where_fingerprint(const std::string& fingerprint)
{
    try
    {
        std::string sql = "SELECT users.email FROM users INNER JOIN clients ON users.id = clients.user_id WHERE clients.fingerprint='" + fingerprint + "';";
        pqxx::result R = notransaction_query(sql);
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
        return "-1";
    }
}

std::string Database::select_owner_email_where_message_id(const std::string& message_id)
{
    try
    {
        std::string sql = "SELECT u.email FROM users u INNER JOIN categories ca ON u.id = ca.owner_id INNER JOIN messages m ON m.category_id = ca.id WHERE m.id = '" + message_id + "';";
        pqxx::result R = notransaction_query(sql);
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
        return "-1";
    }
}

std::string Database::select_client_id_where_fingerprint(const std::string& fingerprint)
{
    try
    {
        std::string sql = "SELECT id FROM clients WHERE fingerprint = '" + fingerprint + "';";
        pqxx::result R = notransaction_query(sql);
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
        return "-1";
    }
}

message Database::select_message_where_id(const std::string& id)
{
    try
    {
        std::string sql = "SELECT * FROM messages WHERE id = '" + id + "'";
        pqxx::result R = notransaction_query(sql);
        message result;
        result.title = "";
        result.content = "";
        result.category_id = "";
        for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c)
        {
            result.title = c[2].as<std::string>();
            result.content = c[3].as<std::string>();
            result.category_id = c[1].as<std::string>();
        }
        return result;
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what()<<std::endl;
        message result;
        result.title = "";
        result.content = "";
        result.category_id = "";
        return result;
    }

}

std::vector<message_info> Database::select_messages_info(const std::string& client_id, bool first)
{
    try
    {
        std::string sql = "SELECT * FROM pending_changes WHERE client_id = '" + client_id + "'";
        if(first) sql += " LIMIT 1";
        pqxx::result R = notransaction_query(sql);
        std::vector<message_info> result;
        for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c)
        {
            message_info m;
            m.id = c[0].as<std::string>();
            m.message_id = c[2].as<std::string>();
            m.state = c[3].as<int>();
            result.insert(result.end(), m);
        }
        return result;
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what()<<std::endl;
        std::vector<message_info> result;
        return result;
    }
}

std::vector<message_info> Database::select_categories_info(const std::string& client_id)
{
    try
    {
        std::string sql = "SELECT * FROM pending_categories WHERE client_id = '" + client_id + "' LIMIT 1";
        pqxx::result R = notransaction_query(sql);
        std::vector<message_info> result;
        for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c)
        {
            message_info m;
            m.id = c[0].as<std::string>();
            m.message_id = c[2].as<std::string>();
            m.state = c[3].as<int>();
            result.insert(result.end(), m);
        }
        return result;
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what()<<std::endl;
        std::vector<message_info> result;
        return result;
    }
}

std::string Database::select_category_where_id(const std::string& id)
{
    pqxx::result R = select_with_specified_attribute("categories", "id", id, false);
    std::string result;
    for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c)
    {
        result = c[1].as<std::string>();
    }
    return result;
}

std::string Database::select_category_by_name(const std::string& id)
{
    pqxx::result R = select_with_specified_attribute("categories", "name", "'" + id + "'", false);
    std::string result;
    for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c)
    {
        result = c[0].as<std::string>();
    }
    return result;
}

std::string Database::update_hostname_where_fingerprint(const std::string& fingerprint, const std::string& hostname)
{
    return update("clients", "hostname", "fingerprint", fingerprint, hostname);
}

std::string Database::insert_into_messages(const std::string& category_id, const std::string& title, const std::string& message, const std::string& number_of_days)
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
    values.insert(values.begin(), std::make_pair("CURRENT_TIMESTAMP + INTERVAL '" + number_of_days + " day'", false));
    values.insert(values.begin(), std::make_pair(message, true));
    values.insert(values.begin(), std::make_pair(title, true));
    values.insert(values.begin(), std::make_pair(category_id, false));
    return insert("messages", attributes, values);
}

std::string Database::insert_local_message(const std::string& id, const std::string& category, const std::string& title, const std::string& content)
{
    std::vector<std::string> attributes;
    std::vector<std::pair <std::string, bool>> values;
    attributes.insert(attributes.begin(), "content");
    attributes.insert(attributes.begin(), "title");
    attributes.insert(attributes.begin(), "category");
    attributes.insert(attributes.begin(), "id");
    values.insert(values.begin(), std::make_pair(content, true));
    values.insert(values.begin(), std::make_pair(title, true));
    values.insert(values.begin(), std::make_pair(category, true));
    values.insert(values.begin(), std::make_pair(id, true));
    return insert("messages", attributes, values);
}

std::string Database::insert_local_category(const std::string& id, const std::string& name)
{
    std::vector<std::string> attributes;
    std::vector<std::pair <std::string, bool>> values;
    attributes.insert(attributes.begin(), "name");
    attributes.insert(attributes.begin(), "id");
    values.insert(values.begin(), std::make_pair(name, true));
    values.insert(values.begin(), std::make_pair(id, true));
    return insert("categories", attributes, values);
}

std::string Database::delete_message_with_id(const std::string& id, const std::string& fingerprint)
{
    try
    {
//        if(select_user_where_fingerprint(fingerprint) == select_owner_email_where_message_id(id))
//            return update("messages", "hidden","id" , id, "true");
//        else
//            return "";
        return update("messages", "hidden","id" , id, "true");
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what()<<std::endl;
        return "";
    }
}

std::string Database::delete_local_record_with_id(const std::string &table, const std::string &id)
{
    try
    {
        return delete_record(table, id);
    }
    catch (const std::exception &e)
    {
        throw std::logic_error("Message deletion error");
    }
}

std::string Database::delete_pending_change(const std::string& id, const std::string& fingerprint)
{
    try
    {
        if(select_client_id_where_fingerprint(fingerprint) != select_client_id_where_change_id("pending_changes", id))
            return "-1";

        return delete_record("pending_changes", id);
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what()<<std::endl;
        return "-1";
    }
}

std::string Database::delete_pending_category(const std::string& id, const std::string& fingerprint)
{
    try
    {
        if(select_client_id_where_fingerprint(fingerprint) != select_client_id_where_change_id("pending_categories", id))
            return "-1";

        return delete_record("pending_categories", id);
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what()<<std::endl;
        return "-1";
    }
}

void Database::test_connection()
{
    try
    {
        pqxx::result R = notransaction_query("");
    }
    catch(const std::exception &e)
    {
        throw std::logic_error("Database connection failed!");
    }
}
