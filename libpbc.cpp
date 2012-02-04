
#include "stdafx.h"

#ifndef WIN32
#define BOOST_TEST_DYN_LINK
#endif

#define BOOST_TEST_MAIN LibPbCompilerTest
#include <boost/test/unit_test.hpp>

#include "pbc/buffer.h"
#include <logger.h>

#include <string>

using namespace std;



BOOST_AUTO_TEST_CASE(setup_app)
{
    logger::setup("debug/error.log", true, 5);
    logger::truncate();
}
///////////////////////////////////////

BOOST_AUTO_TEST_CASE(test_1)
{
    pbc::buffer b1 (pbc::buffer::BT_ANSI, 5);
    pbc::buffer b2 ("test");
    BOOST_CHECK(b2.size() == 4);
    BOOST_CHECK(string(b2.ansi_buf()) == "test");

    pbc::buffer b3 (pbc::buffer::BT_ANSI16, L"test5", 5);
    BOOST_CHECK(b3.type() == pbc::buffer::BT_ANSI16);
    BOOST_CHECK(wstring(b3.wide_buf()) == L"test5");
    b3.make_ansi();
    BOOST_CHECK(b3.type() == pbc::buffer::BT_ANSI);
    //trace_log << "b3.ansi '" << string(b3.ansi_buf()) << "'" <<  endl;
    BOOST_CHECK(string(b3.ansi_buf()) == "test5");

    BOOST_CHECK(b3.use_count() == 1);
    pbc::buffer b4 = b3;
    BOOST_CHECK(b3.use_count() == 2);
    BOOST_CHECK(b4.use_count() == 2);
    b4.make_writable();
    BOOST_CHECK(b3.use_count() == 1);
    BOOST_CHECK(b4.use_count() == 1);
    BOOST_CHECK(string(b4.ansi_buf()) == "test5");
}


//////////////////////////////////////
BOOST_AUTO_TEST_CASE(shutdown_app)
{
    logger::cleanup();
}
