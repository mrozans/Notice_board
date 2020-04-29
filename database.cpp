#include "database.h"
#include <vector>
#include <pqxx/pqxx>
int main(int argc, char *argv[])
{
    std::string connection_string = "dbname = dbvpmci84hd4l0 user = ddnncgvhzpetgu password = f31f30ead499939a8b89498d95f609f45b25f3587363cd84c5bed78326452931 hostaddr = 54.75.246.118 port = 5432";
    auto db = new Database(connection_string);

    pqxx::result R = db->select_all("messages");
    for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c)
    {
        std::cout << "ID = " << c[0].as<int>() << std::endl;
        std::cout << "Name = " << c[1].as<std::string>() << std::endl;
        std::cout << "Owner id = " << c[2].as<int>() << std::endl;
    }
    delete db;
}

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

pqxx::result Database::select_with_specified_attribute(const std::string& table, const std::string& attribute, const std::string& value)
{
    try
    {
        pqxx::connection C(connection_string);
        std::string sql = "SELECT * FROM " + table + " WHERE " + attribute + " = " + value;
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

int Database::insert(const std::string& table, std::vector<std::string> attributes, std::vector<std::string> values)
{
    try
    {
        pqxx::connection C(connection_string);
        if(attributes.size() != values.size()) return 1;
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
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what()<<std::endl;
        return 1;
    }
}

int Database::delete_record(const std::string& table, const std::string& id)
{
    try
    {
        pqxx::connection C(connection_string);
        std::string sql = "DELETE FROM " + table + " WHERE id = " + id;
        pqxx::work W(C);
        W.exec(sql);
        W.commit();
        C.disconnect();
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what()<<std::endl;
        return 1;
    }
}

int Database::delete_record_form_intersection_table(const std::string& category_id, const std::string& user_id)
{
    try
    {
        pqxx::connection C(connection_string);
        std::string sql = "DELETE FROM categories_users WHERE category_id = " + category_id + " AND user_id = " + user_id;
        pqxx::work W(C);
        W.exec(sql);
        W.commit();
        C.disconnect();
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what()<<std::endl;
        return 1;
    }
}