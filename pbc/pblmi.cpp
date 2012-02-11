#include "StdAfx.h"

#include "pblmi.h"

#include <logger.h>
#include <pblmi/pblsdk.h>

namespace pbc {

using namespace std;

pblmi::ptr pblmi::m_instance;

struct library {
    library()
        : m_library(0)
    {
    }
    library(IPBLMI_PBL* lib)
        : m_library(lib)
    {
    }
    ~library()
    {
        destroy();
    }
    library& operator = (IPBLMI_PBL* lib)
    {
        destroy();
        m_library = lib;
    }
    IPBLMI_PBL* operator ->() { return m_library; }
    operator bool() { return m_library != 0; }
private:
    void destroy()
    {
        if (m_library) {
            m_library->Close();
            m_library = 0;
        }
    }
private:
    IPBLMI_PBL* m_library;
};



pblmi::pblmi()
: m_impl(0)
{
    m_impl = PBLMI_GetInterface();
}

pblmi::~pblmi()
{
    if(m_impl) { 
        m_impl->Release();
        m_impl = 0;
    }
}

pblmi::ptr pblmi::instance()
{
    if (!m_instance)
        m_instance.reset(new pblmi());
    return m_instance;
}

template <class Str>
pblmi::entry pblmi::export_entry_impl( const Str& lib_name, const Str& entry_name )
{
    trace_log << "pblmi::export_entry lib_name=" << lib_name << " entry_name=" << entry_name << endl;
    library lib = m_impl->OpenLibrary(lib_name.c_str(), FALSE /*bReadWrite*/);
    if (!lib) {
        debug_log << "OpenLibrary failed for '" << lib_name << "'" << endl;
        throw pbl_open_error("Missing or invalid library: " + logger::string_cast<string>(lib_name));
    }
    PBL_ENTRYINFO entry;
    PBLMI_Result ret = lib->SeekEntry(entry_name.c_str(), &entry, FALSE /*bCreate*/);   
    if (ret != PBLMI_OK) {
        debug_log << "SeekEntry failed for '" << entry_name << "' code=" << (int)ret << endl;
        throw pbl_entry_not_found("Entry not found: " + logger::string_cast<string>(entry_name));
    }

    trace_log << "entry.comment_len=" << entry.comment_len << endl;
    trace_log << "entry.data_len=" << entry.data_len << endl;
    pblmi::entry result;
    result.mod_time = entry.mod_time;
    result.is_unicode = lib->isUnicode() != 0;
    result.comment.make_writable(BT_BINARY, entry.comment_len);
    result.data.make_writable(BT_BINARY, entry.comment_len + entry.data_len);
    ret = lib->ReadEntryData(&entry, result.data.buf());
    if (ret != PBLMI_OK) {
        debug_log << "ReadEntryData failed for '" << entry_name << "' code=" << (int)ret << endl;
        throw pblmi_error("Read entry failed: " + logger::string_cast<string>(entry_name));
    }
    if (entry.comment_len != 0) {
        memcpy(result.comment.buf(), result.data.buf(), entry.comment_len);
        result.data.erase(0, entry.comment_len);
    }
    result.comment.make(result.is_unicode ? pbc::BT_UTF16 : pbc::BT_ANSI);
    if (is_source_entry(entry_name)) {
        result.data.make(result.is_unicode ? pbc::BT_UTF16 : pbc::BT_ANSI);
    }
    return result;
}


pblmi::ptr pblmi::create()
{
    return ptr(new pblmi());
}

template <class Str> 
bool pblmi::is_source_entry_impl( const Str& entry_name )
{
    if (entry_name.size() < 5)  // min: "x.sr?"
        return false;
    size_t pos = entry_name.size() - 4;
    if (entry_name[pos + 0] != '.' 
        || entry_name[pos + 1] != 's'
        || entry_name[pos + 2] != 'r')
        return false;
    switch (entry_name[pos + 3]) {
        case 'a': 
        case 'd':
        case 'f':
        case 'm':
        case 'q':
        case 's':
        case 'u':
        case 'w':
        case 'p':
        case 'j':
        case 'x':
            return true;
        default:
            return false;
    }
}

pblmi::entry pblmi::export_entry( const std::string& lib_name, const std::string& entry_name )
{
    return export_entry_impl(lib_name, entry_name);
}

pblmi::entry pblmi::export_entry( const std::wstring& lib_name, const std::wstring& entry_name )
{
    return export_entry_impl(lib_name, entry_name);
}

bool pblmi::is_source_entry( const std::string& entry_name )
{
    return is_source_entry_impl(entry_name);
}

bool pblmi::is_source_entry( const std::wstring& entry_name )
{
    return is_source_entry_impl(entry_name);
}




} // namespace pbc
