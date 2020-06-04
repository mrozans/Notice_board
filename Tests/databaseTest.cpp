#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE tests2
#include <spdlog/spdlog.h>
#include <boost/test/unit_test.hpp>
#include "../database.cpp"

const std::string tested_fingerprint = "db:20:32:f2:40:c1:d8:dd:60:99:3a:f3:81:8e:c2:02:28:ae:5f:bf";
const std::string tested_category_id = "1";
const std::string tested_category = "assumenda";
const std::string tested_user_id = "1";
const std::string tested_user = "stephan@torphy.io";
const std::string tested_hostname = "101.231.104.172";
const std::string tested_message_id = "1";

std::string database_connection_string()
{
    std::string db_name = getenv("DATABASE_NAME") ? getenv("DATABASE_NAME") : "noticeboard",
            db_user = getenv("DATABASE_USER") ? getenv("DATABASE_USER") : "noticeboard",
            db_password = getenv("DATABASE_PASSWORD") ? getenv("DATABASE_PASSWORD") : "noticeboard",
            db_host = getenv("DATABASE_HOST") ? getenv("DATABASE_HOST") : "localhost",
            db_port = getenv("DATABASE_PORT") ? getenv("DATABASE_PORT") : "5432";
    return "dbname = " + db_name + " user = " + db_user + " password = " + db_password + " hostaddr = " + db_host + " port = " + db_port;
}

std::streambuf *redirect()
{
    std::streambuf *old = std::cerr.rdbuf();
    std::ostringstream str;
    std::cerr.rdbuf(str.rdbuf());
    std::cerr.rdbuf();
    return old;
}

BOOST_AUTO_TEST_CASE(Test_connection)
{
    Database database = Database(database_connection_string(), nullptr);
    bool status = database.check_connection();
    BOOST_CHECK_EQUAL(status, true);
}

BOOST_AUTO_TEST_CASE(Test_invalid_connection)
{
    Database database = Database();
    std::streambuf *old = std::cerr.rdbuf();
    std::ostringstream str;
    std::cerr.rdbuf(str.rdbuf());
    std::cerr.rdbuf();
    bool status = database.check_connection();
    BOOST_CHECK_EQUAL(status, false);
    std::cerr.rdbuf(old);
}

BOOST_AUTO_TEST_CASE(Test_select_all_categories)
{
    Database database = Database(database_connection_string(), nullptr);
    auto result = database.get_categories();
    int count  = result.size();
    bool condition = count != 0;
    BOOST_CHECK_EQUAL(condition, true);
}

BOOST_AUTO_TEST_CASE(Test_select_all_fail)
{
    Database database = Database();
    std::streambuf *old = redirect();
    auto result = database.get_categories();
    int count = result.size();
    BOOST_CHECK_EQUAL(count, 0);
    std::cerr.rdbuf(old);
}

BOOST_AUTO_TEST_CASE(select_owner_email_where_message_id)
{
    Database database = Database(database_connection_string(), nullptr);
    auto result = database.select_owner_email_where_message_id(tested_message_id);
    BOOST_CHECK_EQUAL(result, tested_user);
}

BOOST_AUTO_TEST_CASE(Test_select_category_where_id)
{
    Database database = Database(database_connection_string(), nullptr);
    auto result = database.select_category_where_id(tested_category_id);
    BOOST_CHECK_EQUAL(result, tested_category);
}

BOOST_AUTO_TEST_CASE(Test_category_by_name)
{
    Database database = Database(database_connection_string(), nullptr);
    auto result = database.select_category_by_name(tested_category);
    BOOST_CHECK_EQUAL(result, tested_category_id);
}

BOOST_AUTO_TEST_CASE(Test_select_user_where_fingerprint)
{
    Database database = Database(database_connection_string(), nullptr);
    auto result = database.select_user_where_fingerprint(tested_fingerprint);
    BOOST_CHECK_EQUAL(result, tested_user);
}

BOOST_AUTO_TEST_CASE(Test_select_not_existing_message_where_id)
{
    Database database = Database(database_connection_string(), nullptr);
    auto result = database.select_message_where_id("xyz");
    std::string value = result.category_id;
    BOOST_CHECK_EQUAL(value, "");
}

BOOST_AUTO_TEST_CASE(Test_select_invalid_value)
{
    Database database = Database();
    std::streambuf *old = redirect();
    auto result = database.update_hostname_where_fingerprint(tested_fingerprint, "1.1.1.1");
    BOOST_CHECK_EQUAL(result, "-1");
    std::cerr.rdbuf(old);
}

BOOST_AUTO_TEST_CASE(Test_select_fail)
{
    Database database = Database();
    std::streambuf *old = redirect();
    auto result = database.select_owner_email_where_message_id(tested_category);
    BOOST_CHECK_EQUAL(result, "");
    std::cerr.rdbuf(old);
}

BOOST_AUTO_TEST_CASE(Test_update_fail)
{
    Database database = Database();
    std::streambuf *old = redirect();
    auto result = database.update_hostname_where_fingerprint(tested_fingerprint, "1.1.1.1");
    BOOST_CHECK_EQUAL(result, "-1");
    std::cerr.rdbuf(old);
}

BOOST_AUTO_TEST_CASE(Test_insert_delete_count_message)
{
    Database database = Database(database_connection_string(), nullptr);
    auto categories = database.get_categories();
    BOOST_REQUIRE(!categories.empty());
    database.insert_into_messages(categories[0], "testtitle", "testcontent", "1");
    auto messages = database.select_messages_where_category(categories[0]);
    int size1 = messages.size();
    auto message = messages[messages.size()-1];
    BOOST_CHECK_EQUAL(message.title, "testtitle");
    BOOST_CHECK_EQUAL(message.content, "testcontent");
    auto id = database.select_message_id_where_title("testtitle");
    auto pending = database.get_pending();
    bool deleted = false;
    for(unsigned int i = 0; i < pending.size(); i++)
    {
        if(pending[i].message_id == id)
        {
            database.delete_record("pending_changes", pending[i].id);
            deleted = true;
        }
    }
    BOOST_CHECK_EQUAL(deleted, true);
    database.delete_record("messages", id);
    auto messages2 = database.select_messages_where_category(categories[0]);
    int size2 = messages2.size();
    BOOST_CHECK_EQUAL(size2+1, size1);
}

BOOST_AUTO_TEST_CASE(Test_insert_select_delete_message)
{
    Database database = Database(database_connection_string(), nullptr);
    auto categories = database.get_categories();
    BOOST_REQUIRE(!categories.empty());
    database.insert_into_messages(categories[0], "testtitle", "testcontent", "1");
    auto messages = database.select_messages_where_category(categories[0]);
    auto message = messages[messages.size()-1];
    BOOST_CHECK_EQUAL(message.title, "testtitle");
    BOOST_CHECK_EQUAL(message.content, "testcontent");
    auto id = database.select_message_id_where_title(message.title);
    auto info = database.select_message_where_id(id);
    BOOST_CHECK_EQUAL(message.title, info.title);
    BOOST_CHECK_EQUAL(message.content, info.content);
    auto pending = database.get_pending();
    bool deleted = false;
    for(unsigned int i = 0; i < pending.size(); i++)
    {
        if(pending[i].message_id == id)
        {
            database.delete_record("pending_changes", pending[i].id);
            deleted = true;
        }
    }
    BOOST_CHECK_EQUAL(deleted, true);
    database.delete_record("messages", id);
}

BOOST_AUTO_TEST_CASE(Test_insert_fail)
{
    Database database = Database();
    std::streambuf *old = redirect();
    std::string result = database.insert_into_messages("1", "1", "1", "1");
    BOOST_CHECK_EQUAL(result, "-1");
    std::cerr.rdbuf(old);
}

BOOST_AUTO_TEST_CASE(Test_delete_fail)
{
    Database database = Database();
    std::streambuf *old = redirect();
    auto result = database.delete_record("messages", "1");
    BOOST_CHECK_EQUAL(result, "-1");
    std::cerr.rdbuf(old);
}

BOOST_AUTO_TEST_CASE(Test_delete_invalid_table)
{
    Database database = Database(database_connection_string(), nullptr);
    std::streambuf *old = redirect();
    auto result = database.delete_record("xyz", "1");
    BOOST_CHECK_EQUAL(result, "-1");
    std::cerr.rdbuf(old);
}