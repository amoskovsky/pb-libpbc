#pragma once

#include <vector>
#include <boost/shared_ptr.hpp>

namespace pbc {

class buffer {
public:
    enum buffer_type {
        BT_BINARY,
        BT_ANSI,
        BT_ANSI16,
        BT_UTF8,
        BT_UTF16
    };
    buffer();
    buffer(buffer_type type, size_t size);
    // intentionally no copy ctor and assignment operator 

    char* make_writable(buffer_type type, size_t size);
    const char* make_ansi();
    operator bool () const { return m_data; }
    char* buf() const { return m_data && !m_data->buf.empty() ? &m_data->buf[0] : 0; }
private:
    struct data {
        buffer_type type;
        std::vector<char> buf;
        data() : type(BT_BINARY), buf() {}
    };
    typedef boost::shared_ptr<data> data_ptr;
    void make_writable();
    data_ptr copy_ansi16_to_ansi(data_ptr src);
    void copy_ansi16_to_ansi(char* dest, short* src, size_t size);
    void copy_ansi_to_ansi16(short* dest, char* src, size_t size);
private:
    data_ptr m_data;
};

} // namespace pbc
