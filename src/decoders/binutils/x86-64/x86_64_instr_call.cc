/*-
 * Copyright (C) 2010-2013, Centre National de la Recherche Scientifique,
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
#include <kernel/annotations/CallRetAnnotation.hh>
#include "x86_64_translation_functions.hh"
 
using namespace std;

X86_64_TRANSLATE_1_OP (CALL)
{
  address_t next = data.next_ma.getGlobal ();
  MicrocodeAddress start = data.start_ma;
  MemCell *mc = dynamic_cast<MemCell *> (op1);

  assert (mc != NULL);

  x86_64_push (start, data, Constant::create (next,0, BV_DEFAULT_SIZE));

  Expr *addr = mc->get_addr ();
  MicrocodeAddress here (start);
  if (addr->is_Constant ())
    {
      MicrocodeAddress fct (dynamic_cast<Constant *>(addr)->get_val ());
      data.mc->add_skip (start, fct);
    }
  else
    {
      data.mc->add_jump (start, addr ->ref ());
    }
  MicrocodeNode *start_node = data.mc->get_node (here);
  start_node->add_annotation (CallRetAnnotation::ID,
			      CallRetAnnotation::create_call (addr));
  mc->deref ();
}

			/* --------------- */

X86_64_TRANSLATE_0_OP (RET)
{
  LValue *tmpr0 = data.get_tmp_register (TMPREG (0), BV_DEFAULT_SIZE);
  MicrocodeAddress start = data.start_ma;
  x86_64_pop (start, data, tmpr0);

  MicrocodeNode *start_node = data.mc->get_node (start);
  start_node->add_annotation (CallRetAnnotation::ID,
			      CallRetAnnotation::create_ret ());
  data.mc->add_jump (start, tmpr0->ref ());
  if (data.has_prefix)
    data.has_prefix = false;
}

