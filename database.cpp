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
//    R = db->select_with_specified_attribute("messages", "id", "3");
//    for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c)
//    {
//        std::cout << "Message= " << c[3].as<std::string>() << std::endl;
//    }

//    std::vector<std::string> attributes;
//    std::vector<std::string> values;
//
//    attributes.insert(attributes.begin(), "updated_at");
//    attributes.insert(attributes.begin(), "created_at");
//    attributes.insert(attributes.begin(), "access_key");
//    attributes.insert(attributes.begin(), "user_id");
//    values.insert(values.begin(), "CURRENT_TIMESTAMP");
//    values.insert(values.begin(), "CURRENT_TIMESTAMP");
//    values.insert(values.begin(), "123");
//    values.insert(values.begin(), "1");
//    db->insert("clients", attributes, values);


//    db->delete_record("clients", "4");
//    db->delete_record_form_intersection_table(6, 1);
}