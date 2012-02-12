#pragma once

#include <pbc/buffer.h>

#include <vector>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

class IPBLMI;

namespace pbc {

class pblmi: private boost::noncopyable {
public:
    typedef boost::shared_ptr<pblmi> ptr;
    struct entry {
        time_t mod_time;
        pbc::buffer comment;
        pbc::buffer data;
        bool is_unicode;
    };
    struct dir_entry {
        pbc::buffer name;
        time_t mod_time;
    };
    typedef boost::function<bool(dir_entry&)> list_callback_t;
    ~pblmi();
    static pblmi::ptr instance();
    static pblmi::ptr create();

    pblmi::entry export_entry(const std::string& lib_name, const std::string& entry_name);
    pblmi::entry export_entry(const std::wstring& lib_name, const std::wstring& entry_name);
    
    void import_entry(const std::string& lib_name, const std::string& entry_name, pbc::buffer data, pbc::buffer comment = pbc::buffer(), time_t mod_time = 0);
    void import_entry(const std::wstring& lib_name, const std::wstring& entry_name, pbc::buffer data, pbc::buffer comment = pbc::buffer(), time_t mod_time = 0);

    static bool is_source_entry(const std::string& entry_name);
    static bool is_source_entry(const std::wstring& entry_name);

    void list_entries(const std::string& lib_name, list_callback_t handler);
    void list_entries(const std::wstring& lib_name, list_callback_t handler);

    void delete_entry(const std::string& lib_name, const std::string& entry_name);
    void delete_entry(const std::wstring& lib_name, const std::wstring& entry_name);

private:
    pblmi();
    template <class Str> pblmi::entry export_entry_impl(const Str& lib_name, const Str& entry_name);
    template <class Str> void import_entry_impl(const Str& lib_name, const Str& entry_name, pbc::buffer data, pbc::buffer comment = pbc::buffer(), time_t mod_time = 0);
    template <class Str> static bool is_source_entry_impl(const Str& entry_name);
    template <class Str> void list_entries_impl(const Str& lib_name, list_callback_t handler);
    template <class Str> void delete_entry_impl(const Str& lib_name, const Str& entry_name);
private:
    IPBLMI* m_impl;
    static pblmi::ptr m_instance;
};

class pblmi_error: public std::runtime_error {
public:
    pblmi_error(const std::string& message) : std::runtime_error(message) {}
};

class pbl_open_error: public pblmi_error {
public:
    pbl_open_error(const std::string& message) : pblmi_error(message) {}
};

class pbl_entry_not_found: public pblmi_error {
public:
    pbl_entry_not_found(const std::string& message) : pblmi_error(message) {}
};



} // namespace pbc
