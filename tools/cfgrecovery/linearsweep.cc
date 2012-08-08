/*-
 * Copyright (C) 2012, Centre National de la Recherche Scientifique,
 *                     Institut Polytechnique de Bordeaux,
 *                     Universite Bordeaux 1.
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

#include <stdlib.h>

#include "ConcreteMemoryTraversal.hh"
#include "linearsweep.hh"

using namespace std;

LinearSweepTraversal::LinearSweepTraversal (bool scan_all, 
					    const ConcreteMemory *memory, 
					    Decoder *decoder)
  : ConcreteMemoryTraversal (memory, decoder), scan_all (scan_all)
{
}

LinearSweepTraversal::~LinearSweepTraversal ()
{
}

void 
LinearSweepTraversal::treat_new_arrow (const MicrocodeNode *, 
				       const StmtArrow *arrow,
				       const ConcreteAddress &next)
{
  const Architecture *arch = decoder->get_arch ()->get_reference_arch ();

  const StaticArrow *sa = dynamic_cast<const StaticArrow *> (arrow);
  MicrocodeAddress tgt;
  bool tgt_is_defined = false;
  
  ConcreteAddress src (arrow->get_src ()->get_loc().getGlobal ());

  if (mem->is_defined (src) && can_visit (src))
    add_to_todo_list (src);

  if (sa == NULL)
    {
      const DynamicArrow *da = dynamic_cast<const DynamicArrow *> (arrow);
      MemCell *mc = dynamic_cast<MemCell *> (da->get_target ());
      
      if (mc != NULL && mc->get_addr ()->is_Constant ())
	{
	  Constant *cst = dynamic_cast<Constant *> (mc->get_addr ());
	  ConcreteAddress a (cst->get_val());
	  
	  if (mem->is_defined(a))
	    {
	      ConcreteValue val = 
		mem->get (a, arch->address_range, arch->endianness);
	      tgt = MicrocodeAddress (val.get ());
	      tgt_is_defined = true;
	    }
	}
    }
  else
    {
      tgt = sa->get_target ();
      tgt_is_defined = true;
    }
  
  if (tgt_is_defined && tgt.getLocal () == 0)
    {
      ConcreteAddress ctgt (tgt.getGlobal ());
      
      if (mem->is_defined (ctgt) && can_visit (ctgt))
	add_to_todo_list (ctgt);
    }

  if (scan_all && can_visit (next.get_address ()))
    add_to_todo_list (next);
}

/* Linear sweep disassembly method */
Microcode *
linearsweep (const ConcreteAddress * entrypoint,
	     ConcreteMemory * memory,
	     Decoder * decoder)
{
  Microcode * mc = new Microcode();
  LinearSweepTraversal lst (true, memory, decoder);

  lst.compute (mc, *entrypoint);
  
  return mc;
}
