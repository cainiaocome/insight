/*-
 * Copyright (C) 2010-2012, Centre National de la Recherche Scientifique,
 *                          Institut Polytechnique de Bordeaux,
 *                          Universite Bordeaux 1.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <atf-c++.hpp>
#include <string>
#include <sstream>
#include <iostream>

#include <kernel/Architecture.hh>
#include <kernel/Expressions.hh>
#include <kernel/insight.hh>
#include <io/expressions/smtlib-writer.hh>
#include <io/expressions/expr-parser.hh>

using namespace std;

#define ALL_X86_CC							\
  X86_32_CC (NP,  "(NOT %pf)",						\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert (bvnot ((_ extract 2 2 ) eflags))) \n") \
  X86_32_CC (A,   "(NOT (OR %cf %zf){0;1})",				\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert (not (bvor ((_ extract 0 0 ) eflags) ((_ extract 6 6 ) eflags)))) \n")		\
  X86_32_CC (AE,  "(NOT %cf)",						\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert (not ((_ extract 0 0 ) eflags))) \n")					\
  X86_32_CC (B,   "%cf",						\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert ((_ extract 0 0 ) eflags)) \n")						\
  X86_32_CC (BE,  "(OR %cf %zf){0;1}",					\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert (bvor ((_ extract 0 0 ) eflags) ((_ extract 6 6 ) eflags))) \n")			\
  X86_32_CC (E,   "%zf",						\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert (not (= ((_ extract 6 6 ) eflags) #b0))) \n")						\
  X86_32_CC (G,   "(NOT (OR (XOR %sf %of){0;1} %zf){0;1})",		\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert (not (bvor (bvxor ((_ extract 7 7 ) eflags) ((_ extract 11 11 ) eflags)) ((_ extract 6 6 ) eflags)))) \n")				\
  X86_32_CC (GE,  "(NOT (XOR %sf %of){0;1})",				\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert (not (bvxor ((_ extract 7 7 ) eflags) ((_ extract 11 11 ) eflags)))) \n")	\
  X86_32_CC (L,   "(XOR %sf %of){0;1}",					\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert (bvxor ((_ extract 7 7 ) eflags) ((_ extract 11 11 ) eflags))) \n")			\
  X86_32_CC (LE,  "(OR (XOR %sf %of){0;1} %zf){0;1}",			\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert (bvor (bvxor ((_ extract 7 7 ) eflags) ((_ extract 11 11 ) eflags)) ((_ extract 6 6 ) eflags))) \n")					\
  X86_32_CC (NA,  "(OR %cf %zf){0;1}",					\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert (bvor ((_ extract 0 0 ) eflags) ((_ extract 6 6 ) eflags))) \n")			\
  X86_32_CC (NAE, "%cf",						\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert ((_ extract 0 0 ) eflags)) \n")						\
  X86_32_CC (NB,  "(NOT %cf)",						\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert (not ((_ extract 0 0 ) eflags))) \n")					\
  X86_32_CC (NBE, "(NOT (OR %cf %zf){0;1})",				\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert (not (bvor ((_ extract 0 0 ) eflags) ((_ extract 6 6 ) eflags)))) \n")		\
  X86_32_CC (NE,  "(NOT %zf)",						\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert (bvnot ((_ extract 6 6 ) eflags))) \n")					\
  X86_32_CC (NG,  "(OR (XOR %sf %of){0;1} %zf){0;1}",			\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert (bvor (bvxor ((_ extract 7 7 ) eflags) ((_ extract 11 11 ) eflags)) ((_ extract 6 6 ) eflags))) \n")					\
  X86_32_CC (NGE, "(XOR %sf %of){0;1}",					\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert (bvxor ((_ extract 7 7 ) eflags) ((_ extract 11 11 ) eflags))) \n")			\
  X86_32_CC (NL,  "(NOT (XOR %sf %of){0;1})",				\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert (not (bvxor ((_ extract 7 7 ) eflags) ((_ extract 11 11 ) eflags)))) \n")	\
  X86_32_CC (NLE, "(NOT (OR (XOR %sf %of){0;1} %zf){0;1})",		\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert (not (bvor (bvxor ((_ extract 7 7 ) eflags) ((_ extract 11 11 ) eflags)) ((_ extract 6 6 ) eflags)))) \n")				\
  X86_32_CC (NO,  "(NOT %of)",						\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert (bvnot ((_ extract 11 11 ) eflags))) \n")				\
  X86_32_CC (NS,  "(NOT %sf)",						\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert (bvnot ((_ extract 7 7 ) eflags))) \n")					\
  X86_32_CC (NZ,  "(NOT %zf)",						\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert (bvnot ((_ extract 6 6 ) eflags))) \n")					\
  X86_32_CC (O,   "%of",						\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert (not (= ((_ extract 11 11 ) eflags) #b0))) \n")						\
  X86_32_CC (P,   "%pf",						\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert (not (= ((_ extract 2 2 ) eflags) #b0))) \n")						\
  X86_32_CC (PE,  "%pf",						\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert (not (= ((_ extract 2 2 ) eflags) #b0))) \n")						\
  X86_32_CC (PO,  "(NOT %pf)",						\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert (bvnot ((_ extract 2 2 ) eflags))) \n")					\
  X86_32_CC (S,   "%sf",						\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert (not (= ((_ extract 7 7 ) eflags) #b0))) \n")						\
  X86_32_CC (Z,   "%zf",						\
	     "(declare-fun memory () (Array (_ BitVec 32 ) (_ BitVec 8 ) )) \n(declare-fun eflags () (_ BitVec 32) ) \n(assert (not (= ((_ extract 6 6 ) eflags) #b0))) \n")

#define X86_32_CC(id, e, expout) \
ATF_TEST_CASE(smtlib_ ## id); \
\
ATF_TEST_CASE_HEAD(smtlib_ ## id)	\
{ \
  set_md_var ("descr", \
	      "Check expression parser against x86_32 condition code " # id); \
} \
\
ATF_TEST_CASE_BODY(smtlib_ ## id) \
{ \
  s_check_expr_to_smtlib (# id, e, expout); \
}

static void
s_check_expr_to_smtlib (const string &, const string &expr, 
			const string &expectedout)
{
  insight::init ();
  const Architecture *x86_32 = 
    Architecture::getArchitecture (Architecture::X86_32);
  MicrocodeArchitecture ma (x86_32);

  Expr *e = expr_parser (expr, &ma);
  ostringstream oss;

  smtlib_writer (oss, e, &ma, true);
  
  ATF_REQUIRE_EQ (oss.str (), expectedout);

  insight::terminate ();
}

ALL_X86_CC
#undef X86_32_CC

#if 1
#define X86_32_CC(id, e, expout) \
  ATF_ADD_TEST_CASE(tcs, smtlib_ ## id);


ATF_INIT_TEST_CASES(tcs)
{
  ALL_X86_CC
}
#else

static void 
gen_string (const string &e, const MicrocodeArchitecture &ma)
{					      
  Expr *ex = expr_parser (e, &ma);            
  ostringstream oss;				
  smtlib_writer (oss, ex, &ma, true);	
  string smte = oss.str ();			
  string::size_type i = smte.find ('\n'); 
  while (i != string::npos)			
    {						
      string s = smte.replace (i, 1, "\\n");	
      smte = s;					
      i = smte.find ('\n');			
    } 
  cout << *ex << "-->" << smte << endl;
  ex->deref ();					
}

#define X86_32_CC(id, e, expout) gen_string (e, ma);

int 
main()
{
  insight::init ();
  const Architecture *x86_32 = 
    Architecture::getArchitecture (Architecture::X86_32);
  MicrocodeArchitecture ma (x86_32);
  ALL_X86_CC 
  insight::terminate ();

  return 0;
}
#endif