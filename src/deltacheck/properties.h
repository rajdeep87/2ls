/*******************************************************************\

Module: Property Management

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

#ifndef DELTACHECK_PROPERTIES_H
#define DELTACHECK_PROPERTIES_H

#include <util/threeval.h>

#include <goto-programs/goto_program.h>

class propertyt
{
public:
  goto_programt::const_targett loc;
  tvt status;
  exprt guard, condition;
};
  
typedef std::list<propertyt> propertiest;

void html_report(const propertiest &, std::ostream &);

#endif