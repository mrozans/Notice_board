#include <pqxx/pqxx>
#include <iostream>
#include <utility>

class Database
{
private:
    std::string connection_string;
public:
    explicit Database(std::string connection_string){
        this->connection_string = std::move(connection_string);
    }

    pqxx::result select_all(const std::string& table)
    {
        try
        {
            pqxx::connection C(connection_string);
            std::string sql = "SELECT * FROM " + table;
            pqxx::nontransaction N(C);
            pqxx::result R( N.exec( sql ));
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

    pqxx::result select_with_specified_attribute(const std::string& table, const std::string& attribute, const std::string& value)
    {
        try
        {
            pqxx::connection C(connection_string);
            std::string sql = "SELECT * FROM " + table + " WHERE " + attribute + " = " + value;
            pqxx::nontransaction N(C);
            pqxx::result R( N.exec( sql ));
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

    void insert(const std::string& table, std::vector<std::string> attributes, std::vector<std::string> values)
    {
        try
        {
            pqxx::connection C(connection_string);
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
            pqxx::work W(C);
            W.exec(sql);
            W.commit();
            C.disconnect();
        }
        catch (const std::exception &e)
        {
            std::cerr<<e.what()<<std::endl;
        }
    }

    void delete_record(const std::string& table, const std::string& id)
    {
        try
        {
            pqxx::connection C(connection_string);
            std::string sql = "DELETE FROM " + table + " WHERE id = " + id;
            pqxx::work W(C);
            W.exec(sql);
            W.commit();
            C.disconnect();
        }
        catch (const std::exception &e)
        {
            std::cerr<<e.what()<<std::endl;
        }
    }

    void delete_record_form_intersection_table(const std::string& category_id, const std::string& user_id)
    {
        try
        {
            pqxx::connection C(connection_string);
            std::string sql = "DELETE FROM categories_users WHERE category_id = " + category_id + " AND user_id = " + user_id;
            pqxx::work W(C);
            W.exec(sql);
            W.commit();
            C.disconnect();
        }
        catch (const std::exception &e)
        {
            std::cerr<<e.what()<<std::endl;
        }
    }
};
