/* Copyright (C) 1992, 1996, 1997, 1998, 1999 Aladdin Enterprises.  All rights reserved.

   This software is licensed to a single customer by Artifex Software Inc.
   under the terms of a specific OEM agreement.
 */

/*$RCSfile$ $Revision$ */
/* Dictionary API with implicit dict_stack argument */

#ifndef iddict_INCLUDED
#  define iddict_INCLUDED

#include "idict.h"
#include "icstate.h"		/* for access to dict_stack */

/* Define the dictionary stack instance for operators. */
#define idict_stack (i_ctx_p->dict_stack)

#define idict_put(pdref, key, pvalue)\
  dict_put(pdref, key, pvalue, &idict_stack)
#define idict_put_string(pdref, kstr, pvalue)\
  dict_put_string(pdref, kstr, pvalue, &idict_stack)
#define idict_undef(pdref, key)\
  dict_undef(pdref, key, &idict_stack)
#define idict_copy(dfrom, dto)\
  dict_copy(dfrom, dto, &idict_stack)
#define idict_copy_new(dfrom, dto)\
  dict_copy_new(dfrom, dto, &idict_stack)
#define idict_resize(pdref, newmax)\
  dict_resize(pdref, newmax, &idict_stack)
#define idict_grow(pdref)\
  dict_grow(pdref, &idict_stack)
#define idict_unpack(pdref)\
  dict_unpack(pdref, &idict_stack)

#endif /* iddict_INCLUDED */
