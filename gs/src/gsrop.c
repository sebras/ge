/* Copyright (C) 1995, 1996, 1998 Aladdin Enterprises.  All rights reserved.

   This software is licensed to a single customer by Artifex Software Inc.
   under the terms of a specific OEM agreement.
 */

/*$RCSfile$ $Revision$ */
/* RasterOp / transparency accessing for library */
#include "gx.h"
#include "gserrors.h"
#include "gzstate.h"
#include "gsrop.h"

/* setrasterop */
int
gs_setrasterop(gs_state * pgs, gs_rop3_t rop)
{
    if (pgs->in_cachedevice)
	return_error(gs_error_undefined);
    pgs->log_op = (rop & rop3_1) | (pgs->log_op & ~rop3_1);
    return 0;
}

/* currentrasterop */
gs_rop3_t
gs_currentrasterop(const gs_state * pgs)
{
    return lop_rop(pgs->log_op);
}

/* setsourcetransparent */
int
gs_setsourcetransparent(gs_state * pgs, bool transparent)
{
    if (pgs->in_cachedevice)
	return_error(gs_error_undefined);
    pgs->log_op =
	(transparent ? pgs->log_op | lop_S_transparent :
	 pgs->log_op & ~lop_S_transparent);
    return 0;
}

/* currentsourcetransparent */
bool
gs_currentsourcetransparent(const gs_state * pgs)
{
    return (pgs->log_op & lop_S_transparent) != 0;
}

/* settexturetransparent */
int
gs_settexturetransparent(gs_state * pgs, bool transparent)
{
    if (pgs->in_cachedevice)
	return_error(gs_error_undefined);
    pgs->log_op =
	(transparent ? pgs->log_op | lop_T_transparent :
	 pgs->log_op & ~lop_T_transparent);
    return 0;
}

/* currenttexturetransparent */
bool
gs_currenttexturetransparent(const gs_state * pgs)
{
    return (pgs->log_op & lop_T_transparent) != 0;
}

/* Save/restore logical operation.  (For internal use only.) */
int
gs_set_logical_op(gs_state * pgs, gs_logical_operation_t lop)
{
    pgs->log_op = lop;
    return 0;
}
gs_logical_operation_t
gs_current_logical_op(const gs_state * pgs)
{
    return pgs->log_op;
}
