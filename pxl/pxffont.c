/* Portions Copyright (C) 2001 artofcode LLC.
   Portions Copyright (C) 1996, 2001 Artifex Software Inc.
   Portions Copyright (C) 1988, 2000 Aladdin Enterprises.
   This software is based in part on the work of the Independent JPEG Group.
   All Rights Reserved.

   This software is distributed under license and may not be copied, modified
   or distributed except as expressly authorized under the terms of that
   license.  Refer to licensing information at http://www.artifex.com/ or
   contact Artifex Software, Inc., 101 Lucas Valley Road #110,
   San Rafael, CA  94903, (415)492-9861, for further information. */
/*$Id$ */

/* pxffont.c */
/* PCL XL font-finding procedures */

#include "string_.h"
#include "gx.h"
#include "gschar.h"
#include "gsmatrix.h"		/* for gsfont.h */
#include "gxfont.h"
#include "gxfont42.h"
#include "pxoper.h"
#include "pxstate.h"
#include "pxfont.h"
#include "pjtop.h"

/* ---------------- Operator utilities ---------------- */

#if ARCH_IS_BIG_ENDIAN
#  define pxd_native_byte_order pxd_big_endian
#else
#  define pxd_native_byte_order 0
#endif

/* Widen and/or byte-swap a font name to Unicode if needed. */
/* The argument is a ubyte or uint16 array; the result is a uint16 array */
/* with the elements in native byte order. */
/* We don't deal with mappings: we just widen 8-bit to 16-bit characters */
/* and hope for the best.... */
private int
px_widen_font_name(px_value_t *pfnv, px_state_t *pxs)
{	uint type = pfnv->type;

	if ( (type & (pxd_uint16 | pxd_big_endian)) ==
	     (pxd_uint16 | pxd_native_byte_order)
	   )
	  return 0;		/* already in correct format */
	{ byte *old_data = (byte *)pfnv->value.array.data; /* break const */
	  uint size = pfnv->value.array.size;
	  char16 *new_data;
	  uint i;

	  if ( type & pxd_on_heap )
	    old_data = (byte *)
	      (new_data =
	       (char16 *)gs_resize_object(pxs->memory, old_data,
					  size * 2, "px_widen_font_name"));
	  else
	    new_data =
	      (char16 *)gs_alloc_byte_array(pxs->memory, size, sizeof(char16),
					    "px_widen_font_name");
	  if ( new_data == 0 )
	    return_error(pxs->memory, errorInsufficientMemory);
	  for ( i = size; i; )
	    { --i;
	      new_data[i] =
		(type & pxd_ubyte ? old_data[i] :
		 uint16at(old_data + i * 2, type & pxd_big_endian));
	    }
	  pfnv->value.array.data = (byte *)new_data;
	}	  
	pfnv->type = (type & ~(pxd_ubyte | pxd_big_endian)) |
	  (pxd_uint16 | pxd_native_byte_order | pxd_on_heap);
	return 0;
}

/** It seems that PXL has 10 different names for the lineprinter font 
 * one for each of the font symbol set pairs in pcl.
 * this maps all PXL lineprinter requests to the 0N version 
 * saves ram/rom not storing 10 different versions.
 */

private int
px_lineprinter_font_alias_name(px_value_t *pfnv, px_state_t *pxs)
{	
    uint size = pfnv->value.array.size;

    /* NB:coupling: depends on this being the first LinePrinter font in plftable.c */
    static  const unsigned short linePrinter[16] = 
	{'L','i','n','e',' ','P','r','i','n','t','e','r',' ',' ','0','N'};
    unsigned short *ptr = (unsigned short *)&pfnv->value.array.data[0];
    uint i;

    if ( pfnv->value.array.size != 16 )
	return 0;
    for( i = 0; i < 12; i++) {
	if ( ptr[i] != linePrinter[i] )
	    return 0; /* no match */
    }
    for( i = 12; i < 16; i++) 
	ptr[i] = linePrinter[i];  /* front matched now make it identical */
    return 0;
}

/* ---------------- Operator implementations ---------------- */

/* Look up a font name and return an existing font. */
/* This procedure may widen and/or byte-swap the font name. */
/* If this font is supposed to be built in but no .TTF file is available, */
/* or if loading the font fails, return >= 0 and store 0 in *ppxfont. */
int
px_find_existing_font(px_value_t *pfnv, px_font_t **ppxfont,
  px_state_t *pxs)
{	int code;
        void *pxfont;

	/* Normalize the font name to Unicode. */
	code = px_widen_font_name(pfnv, pxs);
	if ( code < 0 )
	  return code;

	/* Do pxl font aliasing */
	code = px_lineprinter_font_alias_name(pfnv, pxs);
	if ( code < 0 )
	  return code;

	if ( px_dict_find(&pxs->font_dict, pfnv, &pxfont) ) { 
	    /* Make sure this isn't a partially defined font */
	      if ( ((px_font_t *)pxfont)->pfont )
		*ppxfont = pxfont;
	      else {
		  /* in the process of being downloaded. */
		  dprintf(pxs->memory, "font is being downloaded???\n");
		  return -1;
	      }
	} else if ( px_dict_find(&pxs->builtin_font_dict, pfnv, &pxfont) ) 
	    if ( ((px_font_t *)pxfont)->pfont )
		*ppxfont = pxfont;
	    else {
		dprintf(pxs->memory, "corrupt pxl builtin font\n");
		return -1;
	    }
	else
	    return -1; /* font not found  or corrupt builtin font */
	
	return 0;
}

/* Look up a font name and return a font, possibly with substitution. */
/* This procedure implements most of the SetFont operator. */
/* This procedure may widen and/or byte-swap the font name. */
int
px_find_font(px_value_t *pfnv, uint symbol_set, px_font_t **ppxfont,
  px_state_t *pxs)
{

    int code;
    /* Check if we know the font already. */
    /* Note that px_find_existing_font normalizes the font name. */
    code = px_find_existing_font(pfnv, ppxfont, pxs);
    /* NB this logic doesn't make sense */
    if ( code >= 0 )
	return pl_load_resident_font_data_from_file(pxs->memory, *ppxfont);
    else
	return code;
}
