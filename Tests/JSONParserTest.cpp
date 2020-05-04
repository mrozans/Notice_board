#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE tests2
#include <boost/test/unit_test.hpp>
#include "../JSONParser.cpp"
#include <string>

/*generate_client_message*/
BOOST_AUTO_TEST_CASE(Generate_client_message_test)
{
    JSONParser::client_message clientMessage = {};
    clientMessage.token = "token";
    clientMessage.code = "01";
    clientMessage.body = "123";

    std::string s = JSONParser::generate_client_message(clientMessage);

    BOOST_CHECK_EQUAL(s, "{\"body\":\"123\",\"code\":\"01\",\"token\":\"token\"}");
}

BOOST_AUTO_TEST_CASE(Generate_empty_client_message_test)
{
    JSONParser::client_message clientMessage = {};

    std::string s = JSONParser::generate_client_message(clientMessage);

    BOOST_CHECK_EQUAL(s, "{\"body\":\"\",\"code\":\"\",\"token\":\"\"}");
}
/*generate_server_message*/
BOOST_AUTO_TEST_CASE(Generate_server_message_test)
{
    JSONParser::server_message serverMessage = {};
    serverMessage.body = "123";
    serverMessage.respCode = "1234";

    std::string s = JSONParser::generate_server_message(serverMessage);

    BOOST_CHECK_EQUAL(s, "{\"body\":\"123\",\"respCode\":\"1234\"}");
}

BOOST_AUTO_TEST_CASE(Generate_empty_server_message_test)
{
    JSONParser::server_message serverMessage = {};

    std::string s = JSONParser::generate_server_message(serverMessage);

    BOOST_CHECK_EQUAL(s, "{\"body\":\"\",\"respCode\":\"\"}");
}

/*get_client_message*/

BOOST_AUTO_TEST_CASE(get_client_message_test)
{
    std::string token = "token";
    std::string code = "code";
    std::string body = "body";
    std::string s = "{\"body\":\"" + body + "\",\"code\":\"" + code + "\",\"token\":\"" + token + "\"}";
    
    JSONParser::client_message clientMessage = JSONParser::get_client_message(s);

    BOOST_CHECK_EQUAL(token, clientMessage.token);
    BOOST_CHECK_EQUAL(code, clientMessage.code);
    BOOST_CHECK_EQUAL(body, clientMessage.body);
}

BOOST_AUTO_TEST_CASE(get_empty_client_message_test)
{
    std::string body = "body";
    std::string token = "token";
    std::string code = "code";
    std::string s = "{\",\"code\":\"" + code + "\",\"token\":\"" + token + "\"}";

    BOOST_CHECK_THROW(JSONParser::get_client_message(s), json::parse_error);
}

BOOST_AUTO_TEST_CASE(get_client_message_test_wrong_JSON)
{
    std::string token = "token";
    std::string code = "code";
    std::string body = "body";
    std::string s = "{\"body\":\"" + body + "\",\"code\":\"" + code + "\",\"token\":\"" + token + "\"";

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
    std::string respCode = "respCode";
    std::string body = "body";
    std::string s = "{\"body\":\"" + body + "\",\"respCode\":\"" + respCode + "\"}";

    JSONParser::server_message serverMessage = JSONParser::get_server_message(s);

    BOOST_CHECK_EQUAL(respCode, serverMessage.respCode);
    BOOST_CHECK_EQUAL(body, serverMessage.body);
}

BOOST_AUTO_TEST_CASE(get_empty_server_message_test)
{
    std::string respCode = "respCode";
    std::string body = "body";
    std::string s = "{\"body\":\"" + body + "\",\"respCode1\":\"" + respCode + "\"}";

    BOOST_CHECK_THROW(JSONParser::get_server_message(s), json::type_error);
}

BOOST_AUTO_TEST_CASE(get_server_message_test_wrong_JSON)
{
    std::string respCode = "respCode";
    std::string body = "body";
    std::string s = "{\"body\":\"" + body + "\",\"respCode\":\"" + respCode + "\"";

    BOOST_CHECK_THROW(JSONParser::get_server_message(s), json::parse_error);
}

BOOST_AUTO_TEST_CASE(get_server_message_test_wrong_JSON1)
{
    std::string s = "123098";

    BOOST_CHECK_THROW(JSONParser::get_server_message(s), json::type_error);
}