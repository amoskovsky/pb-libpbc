#pragma once

#include <pbc/buffer.h>
#include <util/tstring.h>

#include <list>
#include <map>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/utility/value_init.hpp>

namespace pbparser {

using namespace std;
using boost::value_initialized;

class dw_parser;

namespace dw_ast {

class node {
public:
    typedef boost::shared_ptr<node> ptr;
    node();
    node(const wstring& name);
    node(const string& name);
    node(pbc::buffer name);
    virtual ~node();
    void add_nested(node::ptr node);
    void property(const wstring& name, const wstring& value);
    const wstring& property(const wstring& name) const;
    bool has_property(const wstring& name) const;
    const list<wstring> properties() const;
    void name(const wstring& name);
    void name(const string& name);
    void name(pbc::buffer name);
    const wstring& name() const;
    const list<node::ptr>& nested_nodes() const;
    list<node::ptr> nested_nodes(const wstring& name) const;
    node::ptr nested_node(const wstring& name) const;
private:
    wstring m_name;
    list<node::ptr> m_nested_nodes;
    typedef map<wstring, wstring> properties_t;
    properties_t m_properties;
};

class dw {
public:
    typedef boost::shared_ptr<dw> ptr;
    dw(){}
    virtual ~dw(){}
    const wstring& release() const;

private:
    wstring m_release;
    node::ptr m_root;
    friend class pbparser::dw_parser;
};


}

namespace dw_lexer {
enum token_type {L_UNKNOWN = 0, L_SPACE, L_ID, L_NUM, L_STR, L_LPAREN, L_RPAREN, L_ASSIGN, L_SEMICOLON};
class token {
public:
    token(): m_type(), m_begin(), m_end(), m_line(), m_col() {}
    wstring value() const;
    void type(token_type type);
    token_type type() const;
    void begin(const wchar_t* begin);
    void grow(size_t delta);
    void line(size_t line);
    void col(size_t col);
    size_t line() const;
    size_t col() const;
private:
    token_type m_type;
    const wchar_t* m_begin;
    const wchar_t* m_end;
    size_t m_line;
    size_t m_col;
};
}

class dw_parser
{
public:
    dw_parser(void);
    ~dw_parser(void);

    dw_ast::node::ptr parse(pbc::buffer source);
private:
    void tokenize();
    wstring parse_release();
    pair<size_t, size_t> find_line_col(const wchar_t* begin, const wchar_t* pos);
    wstring read_token(dw_lexer::token_type type);
    wstring read_token();
    bool is_token(dw_lexer::token_type type);
    void require_token(dw_lexer::token_type type);
    void require_token(dw_lexer::token_type type, const wstring& value);
    void update_line_col(const wchar_t* begin, const wchar_t* end, size_t& line, size_t& col);
    bool eof() const;
    void not_eof() const;
    dw_ast::node::ptr parse_section();
    dw_ast::node::ptr parse_properties();
private:
    pbc::buffer m_source;
    typedef list<dw_lexer::token> token_list_t;
    token_list_t m_tokens;
    token_list_t::const_iterator m_cur_token;
};

class parse_error: public std::runtime_error {
public:
    parse_error(const string& message)
        : std::runtime_error(message)
    {
    }
};

} // namespace pbparser
