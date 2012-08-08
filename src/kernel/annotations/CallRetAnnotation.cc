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
#include "CallRetAnnotation.hh"

Annotable::AnnotationId CallRetAnnotation::ID ("callret");

CallRetAnnotation::CallRetAnnotation (const Expr *target)
  : ExprAnnotation (target)
{  
}

CallRetAnnotation::~CallRetAnnotation() 
{ 
}

CallRetAnnotation *
CallRetAnnotation::create_call (const Expr *target)
{
  return new CallRetAnnotation (target);
}

CallRetAnnotation *
CallRetAnnotation::create_ret ()
{
  return new CallRetAnnotation (NULL);
}

void 
CallRetAnnotation::output_text (std::ostream &out) const 
{
  out << (is_call () ? "CALL" : "RET");
}

void *
CallRetAnnotation::clone () const
{  
  return is_call () ? create_call (get_expr ()) : create_ret ();
}

bool 
CallRetAnnotation::is_call () const 
{
  return get_expr () != NULL;
}

const Expr *
CallRetAnnotation::get_target () const 
{
  return get_expr ();
}
