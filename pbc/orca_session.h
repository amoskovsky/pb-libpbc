#pragma once

#include <pbc/buffer.h>
#include <util/scoped_dll.h>

#include <string>
#include <list>
#include <stdexcept>

#include <windows.h>
#include "pborca.h"

#include <vector>
#include <boost/shared_ptr.hpp>

namespace pbc {

#define WALK_ORCA_METHODS(code) \
    code(SessionClose); \
    code(SessionOpen); \
    code(SessionSetCurrentAppl); \
    code(SessionSetLibraryList); \
    code(CompileEntryRegenerate); \
    code(CompileEntryImport); \
    code(DynamicLibraryCreate); \
    code(SessionGetError); \
    code(SetExeInfo); \
    code(ExecutableCreate); \
    code(LibraryCreate);

#define DECLARE_ORCA_METHOD(m) FP_##m m

using namespace std;

typedef pbc::buffer orca_string;

struct orca_compile_error_item {
    int level;
    tstring message_number;
    tstring message_text;
    size_t column_number;
    size_t line_number;
};

class orca_session
{
public:
    typedef boost::shared_ptr<orca_session> ptr;
    orca_session(const wstring& pbrt_dll, int pb_ver, bool unicode);
    ~orca_session();
    void set_current_app(orca_string lib, orca_string app);
    void set_library_list(std::vector<orca_string>& lib_list);
    std::string get_error();
    void compile_entry_regenerate(orca_string lib, orca_string entry, PBORCA_TYPE entry_type);
private:
    orca_session(); // no implementation
    void session_open();
    void session_close();
    static void WINAPI compile_error_callback(PPBORCA_COMPERR err, LPVOID state);
    void compile_error_callback(PPBORCA_COMPERR err);
    void recover();
private:
    WALK_ORCA_METHODS(DECLARE_ORCA_METHOD);
    void init_orca_method(const std::string& method_name, void ** method_addr);
private:
    scoped_dll m_dll;
    buffer_type m_encoding;
    HPBORCA m_session;
    std::vector<orca_string> m_lib_list;
    orca_string m_app_lib;
    orca_string m_app;
    list<orca_compile_error_item> m_compile_errors;
};

class orca_error: public std::runtime_error {
public:
    orca_error(int error_code, const std::string& message);
    int error_code() const { return m_error_code; }
private:
    int m_error_code;
};

class orca_method_error: public std::runtime_error {
public:
    orca_method_error(const std::string& message) : std::runtime_error(message) {}
};

class orca_compile_error: public orca_error {
public:
    enum {GPF_ERROR = -0x200};
    orca_compile_error(int error_code, const std::string& message, list<orca_compile_error_item>& error_items);
    const list<orca_compile_error_item>& error_items() const { return m_error_items; }
private:
    list<orca_compile_error_item> m_error_items;
};

string to_string(PBORCA_TYPE value);


} // namespace pbc
