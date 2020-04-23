#include "database.h"
#include <vector>
#include <pqxx/pqxx>
int main(int argc, char *argv[])
{
    auto db = new Database();
    pqxx::connection C("dbname = tjozw user = tjozw password = yFpNtl5MXeJ2f@iE*qp% hostaddr = 34.231.167.17 port = 5432");
    db->check_connection(&C);
    pqxx::result R = db->select_all(&C, "categories");
    for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c) {
        std::cout << "ID = " << c[0].as<int>() << std::endl;
        std::cout << "Name = " << c[1].as<std::string>() << std::endl;
        std::cout << "Owner id = " << c[2].as<int>() << std::endl;
    }
    R = db->select_with_specified_attribiute(&C, "messages", "id", "3");
    for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c) {
        std::cout << "Message= " << c[3].as<std::string>() << std::endl;
    }

    std::vector<std::string> attributes;
    std::vector<std::string> values;

    attributes.insert(attributes.begin(), "category_id");
    attributes.insert(attributes.begin(), "user_id");
    values.insert(values.begin(), "6");
    values.insert(values.begin(), "1");
    db->insert(&C, "categories_users", attributes, values);
    //db->delete_record(&C, "messages", "2");
    //db->delete_record_form_intersection_table(&C, 6, 1);
    db->disconnect(&C);
}