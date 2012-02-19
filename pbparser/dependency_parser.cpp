#include "StdAfx.h"
#include "dependency_parser.h"

#include <logger.h>

#include <set>

#include <boost/regex.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

namespace pbparser {

dependency_parser::dependency_parser()
{
}

dependency_parser::~dependency_parser()
{
}

bool dependency_parser::has_dependencies( PBORCA_TYPE entry_type )
{
    switch (entry_type) {
    case PBORCA_APPLICATION:      
    case PBORCA_FUNCTION:
    case PBORCA_MENU:
    case PBORCA_STRUCTURE:
    case PBORCA_USEROBJECT:
    case PBORCA_WINDOW:
    case PBORCA_PROXYOBJECT:
        return true;
    case PBORCA_DATAWINDOW:       
    case PBORCA_QUERY:   
    case PBORCA_PIPELINE:
    case PBORCA_PROJECT:
    case PBORCA_BINARY:
    default:
        return false; 
    }
}

const list<wstring>& dependency_parser::get_dependencies()
{
    return get_ancestors();
}

boost::wregex global_type_re(L"^\\s*global\\s+type\\s+\\S+\\s+from\\s+(\\S+)");
boost::wregex nested_type_re(L"^\\s*type\\s+\\S+\\s+from\\s+(\\S+)\\s+within");

void dependency_parser::parse( pbc::buffer source )
{
    const wchar_t* begin = source.make_utf16();
    const wchar_t* end = wcsstr(begin, L"end forward");
    if (!end)
        end = begin + source.size();
    boost::wcmatch m;
    if (!regex_search(begin, end, m, global_type_re)) {
        trace_log << "No global type found" << endl;
        return;
    }
    set<wstring> anc;
    wstring id = m[1].str();
    boost::to_lower(id);
    trace_log << "global type ancestor: " << id << endl;
    anc.insert(id);
    begin += m.length();
    
    while (regex_search(begin, end, m, nested_type_re)) {
        wstring id = m[1].str();
        boost::to_lower(id);
        trace_log << "nested type ancestor: " << id << endl;
        anc.insert(id);
        begin += m.length();
    }

    BOOST_FOREACH(const wstring& a, anc) {
        m_ancestors.push_back(a);
    }
}


} // namespace pbparser 
