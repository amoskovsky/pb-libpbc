#pragma once

#include <pbc/buffer.h>
#include <util/scoped_dll.h>

#include <string>
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

class orca_session
{
public:
    typedef boost::shared_ptr<orca_session> ptr;
    orca_session(const wstring& pbrt_dll, int pb_ver, bool unicode);
    ~orca_session();
    void set_current_app(orca_string app, orca_string lib);
    void set_library_list(std::vector<orca_string>& lib_list);
    std::string get_error();
private:
    orca_session(); // no implementation
    void session_open();
    void session_close();
private:
    WALK_ORCA_METHODS(DECLARE_ORCA_METHOD);
    void init_orca_method(const std::string& method_name, void ** method_addr);
private:
    scoped_dll m_dll;
    buffer_type m_encoding;
    HPBORCA m_session;
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

} // namespace pbc
