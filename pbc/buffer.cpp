#include "StdAfx.h"
#include "buffer.h"

#include <logger.h>

#include <cstdlib>

#include <windows.h>

namespace pbc {

using namespace std;

buffer::buffer()
    : m_data()
{
    trace_log << "buffer::buffer" << endl;
}

buffer::buffer( buffer_type type, size_t size )
{
    trace_log << "buffer::buffer type=" << type << " size=" << size << endl;
    make_writable(type, size);
}

buffer::buffer( const string& ansi_str )
{
    trace_log << "buffer::buffer ansi_str='" << ansi_str << "'" << endl;
    char* buf = (char*)make_writable(BT_ANSI, ansi_str.size());
    std::strncpy(buf, ansi_str.c_str(), ansi_str.size());
}

buffer::buffer( buffer_type type, const char* src, size_t size )
{
    trace_log << "buffer::buffer type=" << type << " char buf=" << (void*)src << " size=" << size << endl;
    char* buf = (char*)make_writable(type, size);
    std::strncpy(buf, src, size);
}

buffer::buffer( const std::wstring& wide_str )
{
    trace_log << "buffer::buffer wide_str='" << wide_str << "'" << endl;
    wchar_t* buf = (wchar_t*)make_writable(BT_UTF16, wide_str.size());
    std::wcsncpy(buf, wide_str.c_str(), wide_str.size());
}

buffer::buffer( buffer_type type, const wchar_t* src, size_t size )
{
    trace_log << "buffer::buffer type=" << type << " wide_str='" << src << "' size=" << size << endl;
    wchar_t* buf = (wchar_t*)make_writable(type, size);
    std::wcsncpy(buf, src, size);
}

buffer::buffer( buffer_type type, const void* c_str )
{
    trace_log << "buffer::buffer type=" << type << " c_str=" << c_str << endl;
    switch (type) {
    case BT_ANSI: {
        size_t size = strlen((const char*)c_str);
        char* buf = (char*)make_writable(type, size);
        std::strncpy(buf, (const char*)c_str, size);
        break;
    }
    case BT_UTF16: {
        size_t size = wcslen((const wchar_t*)c_str);
        wchar_t* buf = (wchar_t*)make_writable(type, size);
        std::wcsncpy(buf, (const wchar_t*)c_str, size);
        break;
    }
    default:
        throw runtime_error("buffer::buffer(buffer_type type, const void* c_str) accepts only BT_ANSI or BT_UTF16");
    }
}

TCHAR* buffer::make_writable( buffer_type type, size_t size )
{
    trace_log << "buffer::make_writable type=" << type << " size=" << size << endl;
    if (!m_data || m_data.use_count() > 1)
        m_data.reset(new data());
    m_data->type = type;
    m_data->size = size;
    size_t byte_size = size;
    switch (type) {
    case BT_ANSI16: case BT_UTF16: 
        byte_size *= 2; 
        break;
    }
    byte_size += 2; // for ansi and wide trailing zero
    trace_log << "alloc byte_size=" << byte_size << endl;
    m_data->buf.resize(byte_size, 0);
    m_data->buf[byte_size - 2] = 0;
    m_data->buf[byte_size - 1] = 0;
    return buf();
}

void buffer::make_writable()
{
    if (!m_data) {
        trace_log << "buffer::make_writable from empty" << endl;
        m_data.reset(new data());
    }
    else if (m_data.use_count() > 1) {
        trace_log << "buffer::make_writable from shared use_count=" << m_data.use_count() << endl;
        m_data.reset(new data(*m_data));
    }
}

char* buffer::make_ansi()
{
    trace_log << "buffer::make_ansi m_data=" << m_data << endl;
    if (!m_data)
        return 0;
    trace_log << "type=" << m_data->type << endl;
    switch (m_data->type) {
    case BT_BINARY:
        make_writable();
        m_data->type = BT_ANSI;
        break;
    case BT_ANSI: 
        break;
    case BT_ANSI16: 
        trace_log << "from BT_ANSI16" << endl;
        m_data = copy_ansi16_to_ansi(m_data);
        break;
    case BT_UTF16:
        trace_log << "from BT_UTF16" << endl;
        m_data = copy_utf16_to_ansi(m_data);
        break;
    }
    return ansi_buf();
}

buffer::data_ptr buffer::copy_ansi16_to_ansi( buffer::data_ptr src )
{
    trace_log << "buffer::copy_ansi16_to_ansi" << endl;
    assert_throw(src);
    assert_throw(src->type == BT_ANSI16);
    data_ptr dest(new data());
    dest->type = BT_ANSI;
    dest->size = src->size;
    size_t byte_size = dest->size + 2;
    dest->buf.resize(byte_size, 0);
    copy_ansi16_to_ansi(&dest->buf[0], (wchar_t*)&src->buf[0], dest->size);
    return dest;
}

void buffer::copy_ansi16_to_ansi( char* dest, wchar_t* src, size_t size )
{
    for (size_t i = 0; i < size; i ++, dest ++, src ++) {
        *dest = (char)*src;
    }
}

void buffer::copy_ansi_to_ansi16( wchar_t* dest, char* src, size_t size )
{
    for (size_t i = 0; i < size; i ++, dest ++, src ++) {
        *dest = *src;
    }
}

buffer::data_ptr buffer::copy_utf16_to_ansi( data_ptr src )
{
    trace_log << "buffer::copy_utf16_to_ansi" << endl;
    assert_throw(src);
    assert_throw(src->type == BT_UTF16);
    data_ptr dest(new data());
    dest->type = BT_ANSI;
    if (src->size > 0) {
        dest->size = ::WideCharToMultiByte(
            CP_ACP,
            0, //WC_DEFAULTCHAR,
            (LPCWSTR) &src->buf[0],
            src->size, // not incl 0
            NULL,
            0, // return buf size in bytes
            NULL, //"?", // def char
            NULL // &lb_UsedDefChar
            );
        assert_throw(dest->size != 0);
        size_t byte_size = dest->size + 2;
        dest->buf.resize(byte_size, 0);
        ::WideCharToMultiByte(
            CP_ACP,
            0, //WC_DEFAULTCHAR,
            (LPCWSTR) &src->buf[0],
            src->size, // not incl 0
            (LPSTR)&dest->buf[0],
            dest->size,
            NULL, //"?", // def char
            NULL  //&lb_UsedDefChar
            );
    }
    return dest;    
}

buffer::data_ptr buffer::copy_ansi_to_utf16( data_ptr src )
{
    trace_log << "buffer::copy_ansi_to_utf16" << endl;
    assert_throw(src);
    assert_throw(src->type == BT_ANSI);
    data_ptr dest(new data());
    dest->type = BT_UTF16;
    if (src->size > 0) {
        dest->size = ::MultiByteToWideChar(
            CP_ACP,
            MB_PRECOMPOSED,
            (LPCSTR) &src->buf[0],
            src->size, // not incl 0
            NULL,
            0 // return buf size in wchar's
            );
        assert_throw(dest->size != 0);
        size_t byte_size = dest->size * 2 + 2;
        dest->buf.resize(byte_size, 0);
        ::MultiByteToWideChar(
            CP_ACP,
            MB_PRECOMPOSED,
            (LPCSTR) &src->buf[0],
            src->size, // not incl 0
            (LPWSTR)&dest->buf[0],
            dest->size
            );
    }
    return dest;
}

wchar_t* buffer::make_utf16()
{
    trace_log << "buffer::make_utf16 m_data=" << m_data << endl;
    if (!m_data)
        return 0;
    trace_log << "type=" << m_data->type << endl;
    switch (m_data->type) {
    case BT_BINARY:
        assert_throw(m_data->size % 2 == 0);
        make_writable();
        m_data->type = BT_UTF16;
        m_data->size /= 2;
        break;
    case BT_ANSI: 
        trace_log << "from BT_ANSI" << endl;
        m_data = copy_ansi_to_utf16(m_data);
        break;
    case BT_ANSI16: 
        trace_log << "from BT_ANSI16" << endl;
        m_data = copy_ansi16_to_ansi(m_data);
        m_data = copy_ansi_to_utf16(m_data);
        break;
    case BT_UTF16:
        break;
    }
    return wide_buf();
}

TCHAR* buffer::make( buffer_type type )
{
    switch (type) {
    case BT_ANSI: 
        return (TCHAR*)make_ansi();
    case BT_UTF16:
        return (TCHAR*)make_utf16();
    default:
        throw runtime_error("buffer::make() acepts only BT_ANSI and BT_UTF16");
    }
}

tstring buffer::to_tstring()
{
    if (!m_data)
        return tstring(TEXT("<NULL>"));
    buffer tmp = *this;
#ifdef UNICODE
    return tmp.make_utf16();
#else
    return tmp.make_ansi();
#endif
}


} // namespace pbc
