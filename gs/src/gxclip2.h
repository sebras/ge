/* Copyright (C) 1993, 1996, 1998, 1999 Aladdin Enterprises.  All rights reserved.

   This software is licensed to a single customer by Artifex Software Inc.
   under the terms of a specific OEM agreement.
 */

/*$RCSfile$ $Revision$ */
/* Tiled mask clipping device and interface */

#ifndef gxclip2_INCLUDED
#  define gxclip2_INCLUDED

#include "gxmclip.h"

/* The structure for tile clipping is the same as for simple mask clipping. */
typedef gx_device_mask_clip gx_device_tile_clip;
#define st_device_tile_clip st_device_mask_clip
/*
 * We can't just make this macro empty, since it is processed as a top-level
 * declaration and would lead to an extraneous semicolon.  The least damage
 * we can do is make it declare a constant (and not static, since static
 * would lead to a compiler warning about an unreferenced variable).
 */
#define private_st_device_tile_clip() /* in gxclip2.c */\
  const byte gxclip2_dummy = 0

/*
 * Initialize a tile clipping device from a mask.
 * We supply an explicit phase.
 */
int tile_clip_initialize(P6(gx_device_tile_clip * cdev,
			    const gx_strip_bitmap * tiles,
			    gx_device * tdev, int px, int py,
			    gs_memory_t *mem));

/*
 * Set the phase of the tile -- used in the tiling loop when
 * the tile doesn't simply fill the plane.
 */
void tile_clip_set_phase(P3(gx_device_tile_clip * cdev, int px, int py));

#endif /* gxclip2_INCLUDED */
