/*******************************************************************\

Module: Incremental Solver Interface

Author: Peter Schrammel

\*******************************************************************/

#ifndef CPROVER_INCREMENTAL_SOLVER_H
#define CPROVER_INCREMENTAL_SOLVER_H 

#include <map>
#include <iostream>

#include <solvers/flattening/bv_pointers.h>
#include <solvers/sat/satcheck.h>

#include "domain.h"
#include "util.h"

//#define DISPLAY_FORMULA
//#define DEBUG_FORMULA
//#define DEBUG_OUTPUT

class incremental_solvert : public messaget
{

 public:
  typedef std::list<exprt> constraintst;

  explicit incremental_solvert(
    const namespacet &_ns) :
    sat_check(),
    solver(_ns,sat_check), 
    ns(_ns),
    activation_literal_counter(0),
    domain_number(0),
    solver_calls(0)
  { 
  }

  virtual void set_message_handler(message_handlert &handler)
  {
    messaget::set_message_handler(handler);
#if 0
    sat_check.set_message_handler(handler);
    solver.set_message_handler(handler);
#endif
  }

  decision_proceduret::resultt operator()()
  {
    solver_calls++;

#ifdef DEBUG_FORMULA
    bvt whole_formula = formula;
    whole_formula.insert(whole_formula.end(),activation_literals.begin(),
			 activation_literals.end());
    solver.set_assumptions(whole_formula);
#endif

    return solver();    
  }

  unsigned get_number_of_solver_calls() { return solver_calls; }

  unsigned next_domain_number() { return domain_number++; }

  static incremental_solvert *allocate(const namespacet &_ns) 
  { 
    return new incremental_solvert(_ns);
  }

  satcheck_minisat_no_simplifiert sat_check;
  bv_pointerst solver;
  const namespacet &ns;

  literalt new_context();
  void pop_context();
  void make_context_permanent();

  //for debugging
  bvt formula;
  void debug_add_to_formula(const exprt &expr);

  //context assumption literals
  bvt activation_literals;
 protected: 
  unsigned activation_literal_counter;
  unsigned domain_number; //ids for each domain instance to make symbols unique

  //statistics
  unsigned solver_calls;
};

static inline incremental_solvert & operator << (
  incremental_solvert &dest,
  const exprt &src)
{
#ifdef DISPLAY_FORMULA
  if(!dest.activation_literals.empty())
    std::cout << "add_to_solver(" << !dest.activation_literals.back() << "): " 
	      << from_expr(dest.ns,"",src) << std::endl;
  else
      std::cout << "add_to_solver: " << from_expr(dest.ns,"",src) << std::endl;
#endif
#ifndef DEBUG_FORMULA
  if(!dest.activation_literals.empty())
    dest.solver << or_exprt(src,
			    literal_exprt(!dest.activation_literals.back()));
  else 
    dest.solver << src;
#else
  if(!dest.activation_literals.empty())
    dest.debug_add_to_formula(
      or_exprt(src,literal_exprt(!dest.activation_literals.back())));
  else 
    dest.debug_add_to_formula(src);
#endif
  return dest;
}

static inline incremental_solvert& operator << (
  incremental_solvert &dest,
  const incremental_solvert::constraintst &src)
{
    for(incremental_solvert::constraintst::const_iterator it = src.begin(); 
        it != src.end(); it++)
    {
#ifndef DEBUG_FORMULA
      dest.solver << *it;
#else
      dest.debug_add_to_formula(*it);
#endif
    }
  return dest;
}

#endif
