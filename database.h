#include <pqxx/pqxx>
#include <iostream>

class Database
{
public:
    int check_connection(pqxx::connection *C)
    {
        if (C->is_open())
        {
            std::cout << "Opened database successfully: " << C->dbname() << std::endl;
            return 0;
        }
        else
        {
            std::cout << "Can't open database" << std::endl;
            return 1;
        }
    }

    pqxx::result select_all(pqxx::connection *C, const std::string& table)
    {
        std::string sql = "SELECT * FROM " + table;
        pqxx::nontransaction N(*C);
        pqxx::result R( N.exec( sql ));
        return R;
    }

    pqxx::result select_with_specified_attribiute(pqxx::connection *C, const std::string& table, const std::string& attribute, const std::string& value)
    {
        std::string sql = "SELECT * FROM " + table + " WHERE " + attribute + " = " + value;
        pqxx::nontransaction N(*C);
        pqxx::result R( N.exec( sql ));
        return R;
    }

    void insert(pqxx::connection *C, const std::string& table, std::vector<std::string> attributes, std::vector<std::string> values)
    {
        if(attributes.size() != values.size()) return;
        std::string sql1, sql2;
        for(int i = 0; i < attributes.size(); i++)
        {
            if(i != 0)
            {
                sql1 += ",";
                sql2 += ",";
            }
            sql1 += attributes[i];
            sql2 += values[i];
        }
        std::string sql = "INSERT INTO " + table + " (" + sql1 + ") " + "VALUES (" + sql2 + ");";
        pqxx::work W(*C);
        W.exec(sql);
        W.commit();
    }

    void delete_record(pqxx::connection *C, const std::string& table, const std::string& id)
    {
        std::string sql = "DELETE FROM " + table + " WHERE id = " + id;
        pqxx::work W(*C);
        W.exec(sql);
        W.commit();
    }

    void delete_record_form_intersection_table(pqxx::connection *C, const std::string& category_id, const std::string& user_id)
    {
        std::string sql = "DELETE FROM categories_users WHERE category_id = " + category_id + " AND user_id = " + user_id;
        pqxx::work W(*C);
        W.exec(sql);
        W.commit();
    }

    int disconnect(pqxx::connection *C)
    {
        C->disconnect();
        return 0;
    }
};
