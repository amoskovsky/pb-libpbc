#include "StdAfx.h"
#include "buffer.h"

namespace pbc {

buffer::buffer()
    : m_data()
{
}

buffer::buffer( buffer_type type, size_t size )
{
    make_writable(type, size);
}

char* buffer::make_writable( buffer_type type, size_t size )
{
    if (!m_data || m_data.use_count() > 1)
        m_data.reset(new data());
    switch (type) {
    case BT_BINARY: 
        // use specified size
        break;
    case BT_UTF8: case BT_ANSI: 
        // append 1 char for EOS
        size += 1; 
        break;
    case BT_ANSI16: case BT_UTF16: 
        // append 1 char for EOS and convert to wide chars
        size += 1; 
        size *= 2; 
        break;
    }
    m_data->type = type;
    m_data->buf.resize(size, 0);
    return buf();
}

void buffer::make_writable()
{
    if (!m_data) 
        m_data.reset(new data());
    else if (m_data.use_count() > 1)
        m_data.reset(new data(*m_data));
}

const char* buffer::make_ansi()
{
    if (!m_data)
        return 0;
    switch (m_data->type) {
    case BT_BINARY:
        make_writable();
        m_data->type = BT_ANSI;
        m_data->buf.push_back(0);
        break;
    case BT_UTF8: 
        break;
    case BT_ANSI: 
        break;
    case BT_ANSI16: {
        m_data = copy_ansi16_to_ansi(m_data);
        break;
                    }
    case BT_UTF16: 
        break;
    }
    return buf();
}

buffer::data_ptr buffer::copy_ansi16_to_ansi( buffer::data_ptr src )
{
    if (src->type != BT_ANSI16)
        return data_ptr();
    data_ptr dest(new data());
    dest->type = BT_ANSI;
    size_t size = src->buf.size() / 2;
    dest->buf.resize(size, 0);
    copy_ansi16_to_ansi(&dest->buf[0], (short*)&src->buf[0], size);
    return dest;
}

void buffer::copy_ansi16_to_ansi( char* dest, short* src, size_t size )
{
    for (size_t i = 0; i < size; i ++, dest ++, src ++) {
        *dest = (char)*src;
    }
}

void buffer::copy_ansi_to_ansi16( short* dest, char* src, size_t size )
{
    for (size_t i = 0; i < size; i ++, dest ++, src ++) {
        *dest = *src;
    }
}

} // namespace pbc
