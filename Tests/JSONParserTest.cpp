/**
 * @authors Tomasz Mazur, Marcin Mozolewski
 *
 */
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE tests2
#include <boost/test/unit_test.hpp>
#include <nlohmann/json.hpp>
#include "../sockets/communication/JSONParser.h"
using json = nlohmann::json;
/*generate_client_message*/
BOOST_AUTO_TEST_CASE(Generate_client_message_test)
{
    JSONParser::client_message clientMessage = {};
    clientMessage.token = "token";
    clientMessage.code = (unsigned short int) 1;
    clientMessage.body = "123";

    std::string s = JSONParser::generate_client_message(clientMessage);

    BOOST_CHECK_EQUAL(s, "{\"body\":\"123\",\"code\":1,\"token\":\"token\"}");
}

BOOST_AUTO_TEST_CASE(Generate_empty_client_message_test)
{
    JSONParser::client_message clientMessage = {};

    std::string s = JSONParser::generate_client_message(clientMessage);

    BOOST_CHECK_EQUAL(s, "{\"body\":\"\",\"code\":0,\"token\":\"\"}");
}
/*generate_server_message*/
BOOST_AUTO_TEST_CASE(Generate_server_message_test)
{
    JSONParser::server_message serverMessage = {};
    serverMessage.body = "123";
    serverMessage.code = (unsigned short int)1;

    std::string s = JSONParser::generate_server_message(serverMessage);

    BOOST_CHECK_EQUAL(s, "{\"body\":\"123\",\"code\":1}");
}

BOOST_AUTO_TEST_CASE(Generate_empty_server_message_test)
{
    JSONParser::server_message serverMessage = {};

    std::string s = JSONParser::generate_server_message(serverMessage);

    BOOST_CHECK_EQUAL(s, "{\"body\":\"\",\"code\":0}");
}

/*get_client_message*/

BOOST_AUTO_TEST_CASE(get_client_message_test)
{
    std::string token = "token";
    unsigned short int code = 1;
    std::string body = "body";
    std::string s = R"({"body":")" + body + R"(","code":)" + std::to_string(code) + R"(,"token":")" + token + "\"}";
    
    JSONParser::client_message clientMessage = JSONParser::get_client_message(s);

    BOOST_CHECK_EQUAL(token, clientMessage.token);
    BOOST_CHECK_EQUAL(code, clientMessage.code);
    BOOST_CHECK_EQUAL(body, clientMessage.body);
}

BOOST_AUTO_TEST_CASE(get_empty_client_message_test)
{
    std::string body = "body";
    std::string token = "token";
    unsigned short int code = 1;
    std::string s = R"({","code":")" + std::to_string(code) + R"(","token":")" + token + "\"}";

    BOOST_CHECK_THROW(JSONParser::get_client_message(s), json::parse_error);
}

BOOST_AUTO_TEST_CASE(get_client_message_test_wrong_JSON)
{
    std::string token = "token";
    unsigned short int code = 1;
    std::string body = "body";
    std::string s = R"({"body":")" + body + R"(","code":")" + std::to_string(code) + R"(","token":")" + token + "\"";

    BOOST_CHECK_THROW(JSONParser::get_client_message(s), json::parse_error);
}

BOOST_AUTO_TEST_CASE(get_client_message_test_wrong_JSON1)
{
    std::string s = "1234";

    BOOST_CHECK_THROW(JSONParser::get_client_message(s), json::type_error);
}

/*get_server_message*/

BOOST_AUTO_TEST_CASE(get_server_message_test)
{
    unsigned short int code = 1;
    std::string body = "body";
    std::string s = R"({"body":")" + body + R"(","code":)" + std::to_string(code) + "}";

    JSONParser::server_message serverMessage = JSONParser::get_server_message(s);

    BOOST_CHECK_EQUAL(code, serverMessage.code);
    BOOST_CHECK_EQUAL(body, serverMessage.body);
}

BOOST_AUTO_TEST_CASE(get_empty_server_message_test)
{
    unsigned short int code = 1;
    std::string body = "body";
    std::string s = R"({"body":")" + body + R"(","code1":")" + std::to_string(code) + "\"}";

    BOOST_CHECK_THROW(JSONParser::get_server_message(s), json::type_error);
}

BOOST_AUTO_TEST_CASE(get_server_message_test_wrong_JSON)
{
    unsigned short int code = 1;
    std::string body = "body";
    std::string s = R"({"body":")" + body + R"(","code":")" + std::to_string(code) + "\"";

    BOOST_CHECK_THROW(JSONParser::get_server_message(s), json::parse_error);
}

BOOST_AUTO_TEST_CASE(get_server_message_test_wrong_JSON1)
{
    std::string s = "123098";

    BOOST_CHECK_THROW(JSONParser::get_server_message(s), json::type_error);
}
#pragma clang diagnostic pop