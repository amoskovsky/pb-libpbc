#include "StdAfx.h"
#include "dw_parser.h"
#include <logger.h>

#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

namespace pbparser {

using boost::make_shared;
using boost::lexical_cast;

dw_parser::dw_parser(void)
{
}

dw_parser::~dw_parser(void)
{
}

dw_ast::node::ptr dw_parser::parse( pbc::buffer source )
{
    dw_ast::node::ptr dw = make_shared<dw_ast::node>();
    source.make_utf16(); // make_utf16 before copying buffer avoids actual copying if already utf16
    m_source = source;
    tokenize();
    dw->property(L"release", parse_release());
    while (!eof()) {
        dw->add_nested(parse_section());
    }
    return dw;
}

wstring dw_parser::parse_release()
{
    require_token(dw_lexer::L_ID, L"release");
    wstring release = read_token(dw_lexer::L_NUM);
    require_token(dw_lexer::L_SEMICOLON);
    return  release;
}

void dw_parser::tokenize()
{
    const wchar_t* begin = m_source.wide_buf();
    const wchar_t* next = begin;
    const wchar_t* end = next + m_source.size();
    boost::wregex token_rx(
        L"^([\\w_\\@\\#\\%\\$\\-.]+"
        L"|\\d+(?:\\.\\d+)?"
        L"|'(?:~.|[^'])*'"
        L"|\"(?:~.|[^\"])*\""
        L"|\\("
        L"|\\)"
        L"|\\="
        L"|\\;"
        L"|\\/\\*.*?\\*\\/"
        L"|\\s+"
        L")");
    boost::wcmatch m;

    size_t line = 1;
    size_t col = 1;
    while (next < end) {
        if (!regex_search(next, end, m, token_rx)) {
            throw parse_error("Unexpected token at line " + lexical_cast<string>(line) + " column " + lexical_cast<string>(col) );
        }
        assert_throw(m.length());
        wchar_t ch = *next;
        dw_lexer::token tok;
        tok.begin(next);
        tok.grow(m.length());
        
        switch (ch) {
        case '\r': case '\n': case '\t': case ' ': case '/':
            tok.type(dw_lexer::L_SPACE);
            break;
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            tok.type(dw_lexer::L_NUM);
            break;
        case '\'': case '"': 
            tok.type(dw_lexer::L_STR);
            break;
        case '=':  
            tok.type(dw_lexer::L_ASSIGN);
            break;
        case ';':  
            tok.type(dw_lexer::L_SEMICOLON);
            break;
        case '(':  
            tok.type(dw_lexer::L_LPAREN);
            break;
        case ')':  
            tok.type(dw_lexer::L_RPAREN);
            break;
        default:
            tok.type(dw_lexer::L_ID);
            break;
        }
        assert_throw(tok.type() != dw_lexer::L_UNKNOWN);
        if (tok.type() != dw_lexer::L_SPACE) {
            m_tokens.push_back(tok);
        }
        update_line_col(next, next + m.length(), line, col);
        next += m.length();
    }
    m_cur_token = m_tokens.begin();
}

pair<size_t, size_t> dw_parser::find_line_col( const wchar_t* begin, const wchar_t* pos )
{
    size_t line = 1;
    size_t col = 1;
    update_line_col(begin, pos, line, col);
    return pair<size_t, size_t>(line, col);
}

wstring dw_parser::read_token( dw_lexer::token_type type )
{
    not_eof();
    if (m_cur_token->type() != type)
        throw parse_error("Expected token of type " + lexical_cast<string>(type) + " but got " + lexical_cast<string>(m_cur_token->type()));
    wstring value = m_cur_token->value();
    ++m_cur_token;
    return value;
}

std::wstring dw_parser::read_token()
{
    not_eof();
    wstring value = m_cur_token->value();
    ++m_cur_token;
    return value;
}

void dw_parser::update_line_col( const wchar_t* begin, const wchar_t* end, size_t& line, size_t& col )
{
    for (const wchar_t* next = begin; next < end; next ++) {
        if (*next == '\n') {
            line ++;
            col = 1;
        }
        else {
            col ++;
        }
    }
}

bool dw_parser::eof() const
{
    return m_cur_token == m_tokens.end();
}

void dw_parser::not_eof() const
{
    if (eof())    
        throw parse_error("Unexpected end of source");
}

void dw_parser::require_token( dw_lexer::token_type type )
{
    not_eof();
    if (m_cur_token->type() != type)
        throw parse_error("Expected token of type " + lexical_cast<string>(type) + " but got " + lexical_cast<string>(m_cur_token->type()));
    ++m_cur_token;
}

void dw_parser::require_token( dw_lexer::token_type type, const wstring& value )
{
    not_eof();
    if (m_cur_token->type() != type)
        throw parse_error("Expected token of type " + lexical_cast<string>(type) + " but got " + lexical_cast<string>(m_cur_token->type()));
    wstring v = m_cur_token->value();
    if (_wcsicmp(v.c_str(), value.c_str()) != 0)
        throw parse_error("Expected token " + pbc::buffer(value).to_string() + " but got " + pbc::buffer(v).to_string());
    ++m_cur_token;
}

dw_ast::node::ptr dw_parser::parse_section()
{
    // name ( properties )
    wstring name = read_token(dw_lexer::L_ID);
    boost::to_lower(name);
    require_token(dw_lexer::L_LPAREN);
    dw_ast::node::ptr section = parse_properties();
    require_token(dw_lexer::L_RPAREN);
    section->name(name);
    return section;
}

dw_ast::node::ptr dw_parser::parse_properties()
{
    dw_ast::node::ptr properties = make_shared<dw_ast::node>();
    while (!is_token(dw_lexer::L_RPAREN)) {
        wstring name = read_token(dw_lexer::L_ID);
        boost::to_lower(name);
        require_token(dw_lexer::L_ASSIGN);
        if (is_token(dw_lexer::L_LPAREN)) {
            //nested section
            require_token(dw_lexer::L_LPAREN);
            dw_ast::node::ptr section = parse_properties();
            require_token(dw_lexer::L_RPAREN);
            section->name(name);
            properties->add_nested(section);
        }
        else {
            // scalar value: num | str | id | type(num)
            wstring value = read_token();
            if (is_token(dw_lexer::L_LPAREN)) {
                value += read_token(dw_lexer::L_LPAREN);
                value += read_token(dw_lexer::L_NUM);
                value += read_token(dw_lexer::L_RPAREN);
            }
            properties->property(name, value);
        }
    }
    return properties;
}

bool dw_parser::is_token( dw_lexer::token_type type )
{
    if (eof())
        return false;
    return m_cur_token->type() == type;
}

dw_ast::node::node()
{
}

dw_ast::node::node( const wstring& name )
: m_name(name)
{
}

dw_ast::node::node( const string& name )
: m_name(pbc::buffer(name).make_utf16())
{
}

dw_ast::node::node( pbc::buffer name )
: m_name(name.make_utf16())
{
}

dw_ast::node::~node()
{
}

const list<wstring> dw_ast::node::properties() const
{
    list<wstring> result;
    BOOST_FOREACH(const properties_t::value_type& p, m_properties) {
        result.push_back(p.first);
    }
    return result;
}

const wstring& dw_ast::node::property( const wstring& name ) const
{
    properties_t::const_iterator i = m_properties.find(name);
    assert_throw(i != m_properties.end());
    return i->second;
}

void dw_ast::node::property( const wstring& name, const wstring& value )
{
    m_properties[name] = value;
}

void dw_ast::node::add_nested( node::ptr node )
{
    assert_throw(this != node.get());
    m_nested_nodes.push_back(node);
}

void dw_ast::node::name( const wstring& name )
{
    m_name = name;
}

void dw_ast::node::name( const string& name )
{
    m_name = pbc::buffer(name).make_utf16();
}

void dw_ast::node::name( pbc::buffer name )
{
    m_name = name.make_utf16();
}

const wstring& dw_ast::node::name() const
{
    return m_name;
}

bool dw_ast::node::has_property( const wstring& name ) const
{
    return m_properties.find(name) != m_properties.end();
}

const list<dw_ast::node::ptr>& dw_ast::node::nested_nodes() const
{
    return m_nested_nodes;
}

list<dw_ast::node::ptr> dw_ast::node::nested_nodes( const wstring& name ) const
{
    list<dw_ast::node::ptr> nodes;
    BOOST_FOREACH(dw_ast::node::ptr node, m_nested_nodes) {
        if (node->name() == name)
            nodes.push_back(node);
    }
    return nodes;
}

dw_ast::node::ptr dw_ast::node::nested_node( const wstring& name ) const
{
    list<dw_ast::node::ptr> nodes = nested_nodes(name);
    assert_throw(nodes.size() == 1);
    return nodes.front();    
}


std::wstring dw_lexer::token::value() const
{
    assert_throw(m_type != L_UNKNOWN);
    assert_throw(m_begin && m_end);
    return wstring(m_begin, m_end);
}

void dw_lexer::token::begin( const wchar_t* begin )
{
    m_end = m_begin = begin;
}

void dw_lexer::token::grow( size_t delta )
{
    m_end += delta;
}

void dw_lexer::token::type( token_type type )
{
    assert_throw(type != L_UNKNOWN);
    m_type = type;
}

pbparser::dw_lexer::token_type dw_lexer::token::type() const
{
    return m_type;
}

void dw_lexer::token::line( size_t line)
{
    m_line = line;
}

size_t dw_lexer::token::line() const
{
    return m_line;
}

void dw_lexer::token::col( size_t col )
{
    m_col = col;
}

size_t dw_lexer::token::col() const
{
    return m_col;
}


const wstring& dw_ast::dw::release() const
{
    return m_release;
}
} // namespace pbparser 
