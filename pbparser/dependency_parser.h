#pragma once

#include <pbc/buffer.h>
#include <pbc/orca_session.h>
#include <util/tstring.h>

#include <list>

#include <boost/shared_ptr.hpp>


namespace pbparser {

using namespace std;

class dependency_parser
{
public:
    dependency_parser();
    ~dependency_parser();

    static bool has_dependencies(PBORCA_TYPE entry_type);
    void parse(pbc::buffer source);
    const list<wstring>& get_ancestors() { return m_ancestors; }
    const list<wstring>& get_dependencies();
private:
private:
    list<wstring> m_ancestors;
};

} // namespace pbparser 
