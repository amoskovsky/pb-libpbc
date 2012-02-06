
#include "stdafx.h"

#ifndef WIN32
#define BOOST_TEST_DYN_LINK
#endif

#define BOOST_TEST_MAIN LibPbCompilerTest
#include <boost/test/unit_test.hpp>

#include <pbc/buffer.h>
#include <pbc/orca_session.h>
#include <util/scoped_dll.h>
#include <logger.h>

#include <string>
#include <sstream>
#include <iomanip>

#include <boost/filesystem/v3/operations.hpp>

using namespace std;
using pbc::orca_string;
using boost::filesystem::absolute;

BOOST_AUTO_TEST_CASE(setup_app)
{
    logger::setup("debug/error.log", true, 4);
    logger::truncate();
}
///////////////////////////////////////

string hexdump(const wstring& s) 
{
    ostringstream os;
    for (size_t i = 0; i < s.size(); i ++) {
        unsigned short ch = (unsigned short)s[i];
        os << setw(4) << setfill('0') << hex << ch;
        os << "'" << (ch < 127 ? (char)ch : '?') << "'";
        os << " ";
    }
    return os.str();
}

string hexdump(const string& s) 
{
    ostringstream os;
    for (size_t i = 0; i < s.size(); i ++) {
        unsigned char ch = (unsigned char)s[i];
        os << setw(2) << setfill('0') << hex << (unsigned)ch;
        os << "'" << (char)ch << "'";
        os << " ";
    }
    return os.str();
}

string abs_path(const string& p) 
{
    return absolute(p).make_preferred().string();
}
BOOST_AUTO_TEST_CASE(test_use_count)
{
    pbc::buffer b1("test");
    BOOST_CHECK(b1);
    BOOST_CHECK(b1.use_count() == 1);
    pbc::buffer b2 = b1;
    BOOST_CHECK(b1.use_count() == 2);
    BOOST_CHECK(b2.use_count() == 2);
    b2.make_writable();
    BOOST_CHECK(b1.use_count() == 1);
    BOOST_CHECK(b1.use_count() == 1);

    pbc::buffer bnull;
    BOOST_CHECK(bnull.use_count() == 0);
    BOOST_CHECK(!bnull);

}

BOOST_AUTO_TEST_CASE(test_ansi16_ansi)
{
    pbc::buffer b1 (pbc::BT_ANSI16, L"test", 4);
    BOOST_CHECK(b1.type() == pbc::BT_ANSI16);
    BOOST_CHECK(b1.size() == 4);
    b1.make_ansi();
    BOOST_CHECK(b1.type() == pbc::BT_ANSI);
    BOOST_CHECK(b1.size() == 4);
    BOOST_CHECK(string(b1.ansi_buf()) == "test");
}


BOOST_AUTO_TEST_CASE(test_utf16_ansi)
{
    pbc::buffer b1 (L"testПроба");
    BOOST_CHECK(b1.type() == pbc::BT_UTF16);
    BOOST_CHECK(b1.size() == 9);
    b1.make_ansi();
    BOOST_CHECK(b1.type() == pbc::BT_ANSI);
    BOOST_CHECK(b1.size() == 9);
    //trace_log << hexdump(b1.ansi_buf()) << endl;
    BOOST_CHECK(string(b1.ansi_buf()) == "testПроба");
    b1.make_utf16();
    BOOST_CHECK(b1.type() == pbc::BT_UTF16);
    BOOST_CHECK(b1.size() == 9);
    //trace_log << hexdump(b1.wide_buf()) << endl;
    BOOST_CHECK(wstring(b1.wide_buf()) == L"testПроба");
}

BOOST_AUTO_TEST_CASE(test_from_fake_tchar)
{
    pbc::buffer b1(pbc::BT_UTF16, L"testПроба");
    BOOST_CHECK(wstring(b1.wide_buf()) == L"testПроба");
    BOOST_CHECK(b1.to_tstring() == TEXT("testПроба"));

    pbc::buffer b2(pbc::BT_ANSI, "testПроба");
    BOOST_CHECK(string(b2.ansi_buf()) == "testПроба");
    BOOST_CHECK(b2.to_tstring() == TEXT("testПроба"));
}

BOOST_AUTO_TEST_CASE(test_to_tstring)
{
    pbc::buffer b;
    BOOST_CHECK(b.to_tstring() == TEXT("<NULL>"));

    b = pbc::buffer(L"testПроба");
    BOOST_CHECK(b.to_tstring() == TEXT("testПроба"));
    
    b = pbc::buffer("testПроба");
    BOOST_CHECK(b.to_tstring() == TEXT("testПроба"));
}

BOOST_AUTO_TEST_CASE(test_lexical_cast)
{
    wstring s1 = L"testПроба";
    string s2 = logger::string_cast<string>(s1);
    BOOST_CHECK(s2 == "testПроба");
    trace_log << "s1:'" << s1 << "'" << endl;
    trace_log << "s2:'" << s2 << "'" << endl;
}

bool test_dll_load_failed_pred(const std::runtime_error& e)
{
    return string(e.what()).find("failed") != string::npos;
}

BOOST_AUTO_TEST_CASE(test_dll_load_failed)
{
    scoped_dll d;
    BOOST_REQUIRE_EXCEPTION(
        d.load(L"nonexistent.dll"), 
        std::runtime_error, 
        test_dll_load_failed_pred
        );
}

BOOST_AUTO_TEST_CASE(test_orca_load)
{
    pbc::orca_session::ptr orca(new pbc::orca_session(L"pborc90.dll", 90, false));
}

BOOST_AUTO_TEST_CASE(test_orca_basic_ops)
{
    pbc::orca_session::ptr orca(new pbc::orca_session(L"pborc90.dll", 90, false));

    vector<orca_string> libs;
    libs.push_back(orca_string(abs_path("testapp/pb9/main.pbl")));
    libs.push_back(orca_string(abs_path("testapp/pb9/menus.pbl")));
    libs.push_back(orca_string(abs_path("testapp/pb9/windows.pbl")));
    orca->set_library_list(libs);
    orca->set_current_app(libs[0], orca_string("app"));

    orca->compile_entry_regenerate(libs[1], orca_string("m_genapp_frame"), PBORCA_MENU);

    try {
        orca->compile_entry_regenerate(libs[1], orca_string("m_genapp_frame_nonexistent"), PBORCA_MENU);
        BOOST_CHECK(!"compile_entry_regenerate did not fail");
    }
    catch (const pbc::orca_compile_error& e) {
        debug_log << "orca_compile_error" 
            << " error_code=" << e.error_code()
            << " message=" << e.what()
            << " errors=" << e.error_items().size()
            << endl;        
    }
}


//////////////////////////////////////
BOOST_AUTO_TEST_CASE(shutdown_app)
{
    logger::cleanup();
}
