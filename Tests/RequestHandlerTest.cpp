#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE tests
#include <boost/test/unit_test.hpp>

int dodaj(int a, int b)
{
    return a+b;
}

BOOST_AUTO_TEST_CASE(dodaj1)
{
    BOOST_CHECK_EQUAL(dodaj(1,2),3);
}

//https://www.codeproject.com/Articles/1264257/Socket-Programming-in-Cplusplus-using-boost-asio-T