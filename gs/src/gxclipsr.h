/* Copyright (C) 1998 Aladdin Enterprises.  All rights reserved.

   This software is licensed to a single customer by Artifex Software Inc.
   under the terms of a specific OEM agreement.
 */

/*$RCSfile$ $Revision$ */
/* Internals of clipsave/cliprestore */

#ifndef gxclipsr_INCLUDED
#  define gxclipsr_INCLUDED

#include "gsrefct.h"

/*
 * Unlike the graphics state stack, which is threaded through the actual
 * gstate objects, the clipping path stack is implemented with separate,
 * small objects.  These are reference-counted, because they may be
 * shared by off-stack graphics states.
 */

#ifndef gx_clip_path_DEFINED
#  define gx_clip_path_DEFINED
typedef struct gx_clip_path_s gx_clip_path;
#endif
#ifndef gx_clip_stack_DEFINED
#  define gx_clip_stack_DEFINED
typedef struct gx_clip_stack_s gx_clip_stack_t;
#endif

struct gx_clip_stack_s {
    rc_header rc;
    gx_clip_path *clip_path;
    gx_clip_stack_t *next;
};

#define private_st_clip_stack()	/* in gsclipsr.c */\
  gs_private_st_ptrs2(st_clip_stack, gx_clip_stack_t,\
    "gx_clip_stack_t", clip_stack_enum_ptrs, clip_stack_reloc_ptrs,\
    clip_path, next)

#endif /* gxclipsr_INCLUDED */
