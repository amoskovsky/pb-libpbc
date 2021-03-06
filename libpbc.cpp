
#include "stdafx.h"

#ifndef WIN32
#define BOOST_TEST_DYN_LINK
#endif

#define BOOST_TEST_MAIN LibPbCompilerTest
#include <boost/test/unit_test.hpp>

#include <pbc/buffer.h>
#include <pbc/orca_session.h>
#include <util/scoped_dll.h>
#include <pbc/pblmi.h>
#include <logger.h>
#include <pbparser/dependency_parser.h>
#include <pbparser/dw_parser.h>

#include <string>
#include <sstream>
#include <iomanip>
#include <map>

#include <boost/filesystem/v3/operations.hpp>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>

using namespace std;
using pbc::orca_string;
using boost::filesystem::absolute;

#define PROBA "test\xCF\xF0\xEE\xE1\xE0"
#define L_PROBA L"test\u041F\u0440\u043E\u0431\u0430"
#ifdef UNICODE
#define T_PROBA L_PROBA
#else
#define T_PROBA PROBA
#endif

BOOST_AUTO_TEST_CASE(setup_app)
{
    logger::setup("debug/error.log", true, 3);
    logger::truncate();
    system("chcp 1251");
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
    pbc::buffer b1 (L_PROBA);
    BOOST_CHECK(b1.type() == pbc::BT_UTF16);
    BOOST_CHECK(b1.size() == 9);
    b1.make_ansi();
    BOOST_CHECK(b1.type() == pbc::BT_ANSI);
    BOOST_CHECK(b1.size() == 9);
    //trace_log << hexdump(b1.ansi_buf()) << endl;
    BOOST_CHECK(string(b1.ansi_buf()) == PROBA);
    b1.make_utf16();
    BOOST_CHECK(b1.type() == pbc::BT_UTF16);
    BOOST_CHECK(b1.size() == 9);
    //trace_log << hexdump(b1.wide_buf()) << endl;
    BOOST_CHECK(wstring(b1.wide_buf()) == L_PROBA);
}

BOOST_AUTO_TEST_CASE(test_from_fake_tchar)
{
    pbc::buffer b1(pbc::BT_UTF16, L_PROBA);
    BOOST_CHECK(wstring(b1.wide_buf()) == L_PROBA);
    BOOST_CHECK(b1.to_tstring() == T_PROBA);

    pbc::buffer b2(pbc::BT_ANSI, PROBA);
    BOOST_CHECK(string(b2.ansi_buf()) == PROBA);
    BOOST_CHECK(b2.to_tstring() == T_PROBA);
}

BOOST_AUTO_TEST_CASE(test_buf_erase)
{
    pbc::buffer b;
    b = pbc::buffer(pbc::BT_ANSI, "12345");
    b.erase(2, 4);
    BOOST_CHECK(string(b.ansi_buf()) == "125");
    BOOST_CHECK(b.size() == 3);

    b = pbc::buffer(pbc::BT_UTF16, L"12345");
    b.erase(2, 4);
    BOOST_CHECK(wstring(b.wide_buf()) == L"125");
    BOOST_CHECK(b.size() == 3);
}


BOOST_AUTO_TEST_CASE(test_buf_insert)
{
    //logger::scoped_level l(5);
    pbc::buffer b1, b2;
    b1 = pbc::buffer(pbc::BT_ANSI, "12345");
    b2 = pbc::buffer(pbc::BT_ANSI, "678");
    b1.insert(2, b2, 1, 3);
    BOOST_CHECK(string(b1.ansi_buf()) == "1278345");
    BOOST_CHECK(b1.size() == 7);
    b1.insert(2, pbc::buffer(), 1, 1);
    BOOST_CHECK(string(b1.ansi_buf()) == "1278345");

    b1 = pbc::buffer(pbc::BT_UTF16, L"12345");
    b2 = pbc::buffer(pbc::BT_UTF16, L"678");
    b1.insert(2, b2, 1, 3);
    BOOST_CHECK(wstring(b1.wide_buf()) == L"1278345");
    BOOST_CHECK(b1.size() == 7);
    b1.insert(2, pbc::buffer(), 1, 1);
    BOOST_CHECK(wstring(b1.wide_buf()) == L"1278345");

}

BOOST_AUTO_TEST_CASE(test_to_tstring)
{
    pbc::buffer b;
    BOOST_CHECK(b.to_tstring() == TEXT("<NULL>"));

    b = pbc::buffer(L_PROBA);
    BOOST_CHECK(b.to_tstring() == T_PROBA);
    
    b = pbc::buffer(PROBA);
    BOOST_CHECK(b.to_tstring() == T_PROBA);
}

BOOST_AUTO_TEST_CASE(test_lexical_cast)
{
    wstring s1 = L_PROBA;
    string s2 = logger::string_cast<string>(s1);
    BOOST_CHECK(s2 == PROBA);
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

pbc::orca_session::ptr load_pb9_project(vector<orca_string>& libs)
{
    pbc::orca_session::ptr orca(new pbc::orca_session(L"pborc90.dll", 90, false));

    libs.clear();
    libs.push_back(orca_string(abs_path("testapp/pb9/main.pbl")));
    libs.push_back(orca_string(abs_path("testapp/pb9/menus.pbl")));
    libs.push_back(orca_string(abs_path("testapp/pb9/windows.pbl")));
    orca->set_library_list(libs);
    orca->set_current_app(libs[0], orca_string("app"));
    return orca;
}

BOOST_AUTO_TEST_CASE(test_orca_regen)
{
    vector<orca_string> libs;
    pbc::orca_session::ptr orca = load_pb9_project(libs);

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


BOOST_AUTO_TEST_CASE(test_orca_entry_delete)
{
    vector<orca_string> libs;
    pbc::orca_session::ptr orca = load_pb9_project(libs);

    try {
        orca->library_entry_delete(libs[1], orca_string("m_genapp_frame_nonexistent1"), PBORCA_MENU);
        BOOST_CHECK(!"library_entry_delete did not fail");
    }
    catch (const pbc::orca_error& e) {
        debug_log << "orca_compile_error" 
            << " error_code=" << e.error_code()
            << " message=" << e.what()
            << endl;        
        BOOST_CHECK(e.error_code() == PBORCA_OBJNOTFOUND);
    }
    orca->library_entry_delete(libs[1], orca_string("m_genapp_frame_nonexistent2"), PBORCA_MENU, false/*throw*/);
}


BOOST_AUTO_TEST_CASE(test_orca_import)
{
    vector<orca_string> libs;
    pbc::orca_session::ptr orca = load_pb9_project(libs);

#undef FNAME 
#define FNAME "bool2num1"
    string syntax = 
        "global type " FNAME " from function_object\r\n"
        "end type\r\n"
        "\r\n"
        "forward prototypes\r\n"
        "global function integer " FNAME " (boolean ab_expr)\r\n"
        "end prototypes\r\n"
        "\r\n"
        "global function integer " FNAME " (boolean ab_expr);If ab_expr Then\r\n"
        "Return 1\r\n"
        "Else\r\n"
        "Return 0\r\n"
        "End IF\r\n"
        "Return 0\r\n"
        "end function\r\n"
        "\r\n"
        ;

    orca->library_entry_delete(libs[2], orca_string(FNAME), PBORCA_FUNCTION, false/*throw*/);
    orca->compile_entry_import(libs[2], orca_string(FNAME), PBORCA_FUNCTION,
        orca_string("comment"), orca_string(syntax));


}

BOOST_AUTO_TEST_CASE(test_orca_import_with_errors)
{
    vector<orca_string> libs;
    pbc::orca_session::ptr orca = load_pb9_project(libs);

#undef FNAME 
#define FNAME "bool2num2"
    string syntax = 
        "global type " FNAME " from function_object\r\n"
        "end type\r\n"
        "\r\n"
        "forward prototypes\r\n"
        "global function integer " FNAME " (boolean ab_expr)\r\n"
        "end prototypes\r\n"
        "\r\n"
        "global function integer " FNAME " (boolean ab_expr);If ab_expr Then\r\n"
        "Return 1\r\n"
        "Else\r\n"
        "Return 0\r\n"
        "End IF\r\n"
        "uo_bad1 luo_bad1\r\n"
        "Return 0\r\n"
        "end function\r\n"
        "\r\n"
        ;

    orca->library_entry_delete(libs[2], orca_string(FNAME), PBORCA_FUNCTION, false/*throw*/);
    try {
        orca->compile_entry_import(libs[2], orca_string(FNAME), PBORCA_FUNCTION, orca_string("comment"), orca_string(syntax));
        BOOST_CHECK(!"compile_entry_import did not fail");
    }
    catch (const pbc::orca_compile_error& e) {
        debug_log << "orca_compile_error" 
            << " error_code=" << e.error_code()
            << " message=" << e.what()
            << endl;        
        BOOST_FOREACH(const pbc::orca_compile_error_item& i, e.error_items()) {
            debug_log << "item: " << i.message_text << endl; 
        }
    }
}

using pbc::pblmi;

BOOST_AUTO_TEST_CASE(test_pblmi_is_source)
{
    BOOST_CHECK(!pblmi::is_source_entry("a"));
    BOOST_CHECK(pblmi::is_source_entry("a.sra"));
    BOOST_CHECK(pblmi::is_source_entry("a.srd"));
    BOOST_CHECK(pblmi::is_source_entry("a.srf"));
    BOOST_CHECK(pblmi::is_source_entry("a.srm"));
    BOOST_CHECK(pblmi::is_source_entry("a.srq"));
    BOOST_CHECK(pblmi::is_source_entry("a.srs"));
    BOOST_CHECK(pblmi::is_source_entry("a.sru"));
    BOOST_CHECK(pblmi::is_source_entry("a.srw"));
    BOOST_CHECK(pblmi::is_source_entry("a.srp"));
    BOOST_CHECK(pblmi::is_source_entry("a.srj"));
    BOOST_CHECK(pblmi::is_source_entry("a.srx"));
    BOOST_CHECK(!pblmi::is_source_entry("a.bin"));
    BOOST_CHECK(!pblmi::is_source_entry("c:\\xxx.jpg"));
    BOOST_CHECK(pblmi::is_source_entry("w_genapp_about.srw"));
}

BOOST_AUTO_TEST_CASE(test_pblmi_is_pcode)
{
    BOOST_CHECK(!pblmi::is_pcode_entry("a"));
    BOOST_CHECK(!pblmi::is_pcode_entry("a.sra"));
    BOOST_CHECK(!pblmi::is_pcode_entry("c:\\xxx.jpg"));
    BOOST_CHECK(pblmi::is_pcode_entry("a.udo"));
    BOOST_CHECK(pblmi::is_pcode_entry("a.dwo"));
    BOOST_CHECK(pblmi::is_pcode_entry("a.str"));
    BOOST_CHECK(pblmi::is_pcode_entry("a.win"));
    BOOST_CHECK(pblmi::is_pcode_entry("a.fun"));
    BOOST_CHECK(pblmi::is_pcode_entry("a.xxy"));
    BOOST_CHECK(pblmi::is_pcode_entry("a.apl"));
    BOOST_CHECK(pblmi::is_pcode_entry("a.pra"));
    BOOST_CHECK(pblmi::is_pcode_entry("a.exe"));
}




template <class Str>
void test_pblmi_export_impl(const Str& lib_name, const Str& entry_name)
{
    //logger::scoped_level l(5);
    pbc::pblmi::entry e = pbc::pblmi::create()->export_entry(lib_name, entry_name);
    BOOST_CHECK(e.comment.size() == sizeof("Generated About window") - 1);
    BOOST_CHECK(e.data.size() == 1417);
    tstring comment = e.comment.to_tstring();
    tstring data = e.data.to_tstring();
    trace_log << "comment='" << comment << "'" << endl;
    BOOST_CHECK(comment == TEXT("Generated About window"));
    trace_log << "data.size=" << data.size() << endl;
    BOOST_CHECK(data.size() == 1417);
}


BOOST_AUTO_TEST_CASE(test_pblmi_export_text)
{
    debug_log << "ansi app + ansi pbl" << endl;
    test_pblmi_export_impl(string("testapp/pb9/windows.pbl"), string("w_genapp_about.srw"));

    debug_log << "ansi app + unicode pbl" << endl;
    test_pblmi_export_impl(string("testapp/pb10/windows.pbl"), string("w_genapp_about.srw"));

    debug_log << "unicode app + ansi pbl" << endl;
    test_pblmi_export_impl(wstring(L"testapp/pb9/windows.pbl"), wstring(L"w_genapp_about.srw"));

    debug_log << "unicode app + unicode pbl" << endl;
    test_pblmi_export_impl(wstring(L"testapp/pb10/windows.pbl"), wstring(L"w_genapp_about.srw"));
}


BOOST_AUTO_TEST_CASE(test_pblmi_export_bin)
{
    pbc::pblmi::entry e = pbc::pblmi::create()->export_entry(string("testapp/pb9/windows.pbl"), string("w_genapp_about.win"));
    tstring comment = e.comment.to_tstring();
    trace_log << "comment='" << comment << "'" << endl;
    BOOST_CHECK(comment.empty());
    trace_log << "data.size=" << e.data.size() << endl;
    BOOST_CHECK(e.data.size() == 5090);
}

BOOST_AUTO_TEST_CASE(test_pblmi_export_errors)
{
    try {
        pbc::pblmi::entry e = pbc::pblmi::create()
            ->export_entry(string("testapp/pb9/windows.pbl"), string("test.missing"));
        BOOST_CHECK(!"Missing not found error");
    }
    catch (const pbc::pbl_entry_not_found& e) {
        trace_log << "error=" << e.what() << endl;
    }

    try {
        pbc::pblmi::entry e = pbc::pblmi::create()
            ->export_entry(string("testapp/pb9/windows.missing.pbl"), string("test.missing"));
        BOOST_CHECK(!"Missing not found error");
    }
    catch (const pbc::pbl_open_error& e) {
        trace_log << "error=" << e.what() << endl;
    }

}

template <class Str>
void test_pblmi_import_impl(const Str& lib_name, const Str& entry_name)
{
    //logger::scoped_level l(5);
    pbc::pblmi::ptr p = pbc::pblmi::create();
    pbc::buffer c, d;
    c = pbc::buffer(L"test comm11");
    d = pbc::buffer(L_PROBA);
    p->import_entry(lib_name, entry_name, d, c);

    pbc::pblmi::entry e = p->export_entry(lib_name, entry_name);
    trace_log << "e.comment.size()=" << e.comment.size() << endl;
    trace_log << "e.comment='" << e.comment.to_tstring() << "'" << endl;
    BOOST_CHECK(e.comment.to_tstring() == TEXT("test comm11"));
    trace_log << "e.data.size()=" << e.data.size() << endl;
    BOOST_CHECK(e.data.size() == sizeof(T_PROBA)/sizeof(TCHAR) - 1);
    trace_log << "e.data=" << e.data.to_tstring() << endl;
    BOOST_CHECK(e.data.to_tstring() == T_PROBA);

}

BOOST_AUTO_TEST_CASE(test_pblmi_import)
{
    test_pblmi_import_impl(string("testapp/pb9/windows.pbl"), string("u_test.sru"));
    test_pblmi_import_impl(string("testapp/pb10/windows.pbl"), string("u_test.sru"));
}

BOOST_AUTO_TEST_CASE(test_pblmi_bad_import_pb10)
{
    _unlink("testapp/pb10/pbres_data_after.pbd");
    boost::filesystem::copy_file("testapp/pb10/pbres_data_before.pbd", "testapp/pb10/pbres_data_after.pbd");
    size_t size = (size_t)boost::filesystem::file_size("testapp/pb10/cancel1.bmp");
    ifstream in("testapp/pb10/cancel1.bmp", ios::binary);
    BOOST_CHECK(in.good());
    pbc::buffer d(pbc::BT_BINARY, size);
    in.read(d.ansi_buf(), size);
    
    pbc::pblmi::ptr p = pbc::pblmi::create();
    pbc::buffer c;
    p->import_entry(wstring(L"testapp/pb10/pbres_data_after.pbd"), wstring(L"res\\cancel1.bmp"), d, c);
    //p->import_entry(string("testapp/pb10/pbres_data_after.pbd"), string("res\\cancel1.bmp"), d, c);

    pbc::pblmi::entry e = p->export_entry(wstring(L"testapp/pb10/pbres_data_after.pbd"), wstring(L"res\\cancel1.bmp"));
    trace_log << "e.data.size()=" << e.data.size() << endl;
    BOOST_CHECK(e.data.size() == d.size());
}

BOOST_AUTO_TEST_CASE(test_pblmi_bad_export)
{
    //logger::scoped_level l(5);
    pbc::pblmi::ptr p = pbc::pblmi::create();
    pbc::pblmi::entry e = p->export_entry(wstring(L"testapp/pb10/pbres_data_before.pbd"), wstring(L"res\\cancel.bmp"));
    trace_log << "e.data.size()=" << e.data.size() << endl;
    BOOST_CHECK(e.data.size() == 246);
}

struct test_pblmi_list_impl_handler {
    map<tstring, int> names;
    bool handler(pbc::pblmi::dir_entry& e)
    {
        debug_log << "name=" << e.name.to_tstring() << " mod_time=" << e.mod_time << "(" << (e.mod_time - time(0)) << ")" << endl;
        names[e.name.to_tstring()] ++;
        return true;
    }
};
template <class Str>
void test_pblmi_list_impl(const Str& lib_name)
{
    //logger::scoped_level l(4);
    pbc::pblmi::ptr p = pbc::pblmi::create();
    test_pblmi_list_impl_handler h;
    p->list_entries(lib_name, boost::bind(&test_pblmi_list_impl_handler::handler, &h, _1));

    BOOST_CHECK(h.names[TEXT("w_genapp_about.srw")] == 1);
    BOOST_CHECK(h.names[TEXT("w_genapp_about.win")] == 1);
    BOOST_CHECK(h.names[TEXT("w_genapp_frame.srw")] == 1);
    BOOST_CHECK(h.names[TEXT("w_genapp_frame.win")] == 1);
    BOOST_CHECK(h.names[TEXT("w_genapp_sheet.srw")] == 1);
    BOOST_CHECK(h.names[TEXT("w_genapp_sheet.win")] == 1);
    BOOST_CHECK(h.names[TEXT("w_genapp_toolbars.srw")] == 1);
    BOOST_CHECK(h.names[TEXT("w_genapp_toolbars.win")] == 1);
}

BOOST_AUTO_TEST_CASE(test_pblmi_list)
{
    test_pblmi_list_impl(string("testapp/pb9/windows.pbl"));
    test_pblmi_list_impl(wstring(L"testapp/pb9/windows.pbl"));
    test_pblmi_list_impl(string("testapp/pb10/windows.pbl"));
    test_pblmi_list_impl(wstring(L"testapp/pb10/windows.pbl"));
}

BOOST_AUTO_TEST_CASE(test_pblmi_delete_entry)
{
    //logger::scoped_level l(5);
    pbc::pblmi::ptr p = pbc::pblmi::create();
    string lib_name = "testapp/pb9/windows.pbl";
    string entry_name = "w_genapp_about.srw";
    pbc::pblmi::entry e1 = p->export_entry(lib_name, entry_name);
    p->import_entry(lib_name, entry_name + ".tmp", e1.data, e1.comment);
    pbc::pblmi::entry e2 = p->export_entry(lib_name, entry_name + ".tmp");
    debug_log << "e2.data=" << e2.data.to_tstring() << endl;
    BOOST_CHECK(e1.comment.to_tstring() == e2.comment.to_tstring());
    BOOST_CHECK(e1.data.size() == e2.data.size());
    
    p->delete_entry(lib_name, entry_name + ".tmp");
    try {
        pbc::pblmi::entry e3 = p->export_entry(lib_name, entry_name + ".tmp");
        BOOST_CHECK(!"Entry eas not deleted");    
    }
    catch (pbc::pbl_entry_not_found& e) {
        debug_log << "exception pbl_entry_not_found=" << e.what() << endl;
    }

}

BOOST_AUTO_TEST_CASE(test_dependency_parser)
{
    //logger::scoped_level l(5);
    pbc::pblmi::ptr p = pbc::pblmi::create();
    pbc::pblmi::entry e;

    e = p->export_entry("testapp/pb9/windows.pbl", "w_genapp_about.srw");
    pbparser::dependency_parser dp;
    dp.parse(e.data);
    wstring dep;
    BOOST_FOREACH(wstring a, dp.get_dependencies()) {
        dep += L"[" + a + L"]";
    }
    trace_log << "dep: " << dep << endl;
    BOOST_CHECK(dep == L"[commandbutton][statictext][window]");
}

BOOST_AUTO_TEST_CASE(test_dw_parser_ast)
{
    //logger::scoped_level l(5);
    pbparser::dw_ast::node node("name_ansi");
    BOOST_CHECK(node.name() == L"name_ansi");
    node.name(L"name");
    BOOST_CHECK(node.name() == L"name");
    node.property(L"p2", L"p2v");
    node.property(L"p1", L"p1v");
    node.property(L"p3", L"p3v");
    wstring list;
    BOOST_FOREACH(wstring a, node.properties()) {
        list += L"[" + a + L"/" + node.property(a)+ L"]";
    }
    trace_log << "props: " << list << endl;
    BOOST_CHECK(list == L"[p1/p1v][p2/p2v][p3/p3v]");
}

BOOST_AUTO_TEST_CASE(test_dw_parser_lexer)
{
    //logger::scoped_level l(5);
    pbparser::dw_parser p;
    pbparser::dw_ast::node::ptr dw = p.parse(pbc::buffer(
        "Release 11.5;\n"
        "ddd()\n"
        "ddd(x=123 y$=(a=b(1) c='sss'))\n"
        "\n"
        "\n"
        "\n"
        ));
    BOOST_CHECK(dw->property(L"release") == L"11.5");
    BOOST_CHECK(dw->nested_nodes().size() == 2);


}

BOOST_AUTO_TEST_CASE(test_dw_parser_samples)
{
    pbc::pblmi::entry e = pbc::pblmi::create()->export_entry(string("testapp/pb9/main.pbl"), "d_1.srd");
    logger::scoped_level l(5);
    pbparser::dw_parser p;
    pbparser::dw_ast::node::ptr dw = p.parse(e.data);
    BOOST_CHECK(dw->property(L"release") == L"9");
    BOOST_CHECK(dw->nested_nodes().size() == 11);
    BOOST_CHECK(dw->nested_node(L"table")->nested_nodes().front()->property(L"type") == L"char(10)");
}

//////////////////////////////////////
BOOST_AUTO_TEST_CASE(shutdown_app)
{
    logger::cleanup();
}
