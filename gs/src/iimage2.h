/* Copyright (C) 1997, 1999 Aladdin Enterprises.  All rights reserved.

   This software is licensed to a single customer by Artifex Software Inc.
   under the terms of a specific OEM agreement.
 */

/*$RCSfile$ $Revision$ */
/* Level 2 image operator support */
/* Requires gsiparam.h */

#ifndef iimage2_INCLUDED
#  define iimage2_INCLUDED

/* These procedures are exported by zimage2.c for other modules. */

/*
 * Define a structure for image parameters other than those defined
 * in the gs_*image*_t structure.
 */
typedef struct image_params_s {
    bool MultipleDataSources;
    ref DataSource[gs_image_max_components];
    const float *pDecode;
} image_params;

/* Extract and check parameters for an image. */
int data_image_params(P6(const ref *op, gs_data_image_t *pim,
			 image_params *pip, bool require_DataSource,
			 int num_components, int max_bits_per_component));
int pixel_image_params(P5(i_ctx_t *i_ctx_p, const ref *op,
			  gs_pixel_image_t *pim, image_params * pip,
			  int max_bits_per_component));

/*
 * Process an image that has no explicit source data.  This isn't used by
 * standard Level 2, but it's a very small procedure and is needed by
 * both zdps.c and zdpnext.c.
 */
int process_non_source_image(P3(i_ctx_t *i_ctx_p,
				const gs_image_common_t * pim,
				client_name_t cname));

#endif /* iimage2_INCLUDED */
