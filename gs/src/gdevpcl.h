/* Copyright (C) 1992, 1994, 1999 Aladdin Enterprises.  All rights reserved.

   This software is licensed to a single customer by Artifex Software Inc.
   under the terms of a specific OEM agreement.
 */

/*$RCSfile$ $Revision$ */
/* Support for PCL-based printer drivers */
/* Requires gdevprn.h */

#ifndef gdevpcl_INCLUDED
#  define gdevpcl_INCLUDED

/* Define the PCL paper size codes. */
#define PAPER_SIZE_LETTER 2
#define PAPER_SIZE_LEGAL 3
#define PAPER_SIZE_A4 26
#define PAPER_SIZE_A3 27
#define PAPER_SIZE_A2 28
#define PAPER_SIZE_A1 29
#define PAPER_SIZE_A0 30

/* Get the paper size code, based on width and height. */
int gdev_pcl_paper_size(P1(gx_device *));

/* Color mapping procedures for 3-bit-per-pixel RGB printers */
dev_proc_map_rgb_color(gdev_pcl_3bit_map_rgb_color);
dev_proc_map_color_rgb(gdev_pcl_3bit_map_color_rgb);

/* Row compression routines */
typedef ulong word;
int
    gdev_pcl_mode2compress(P3(const word * row, const word * end_row, byte * compressed)),
    gdev_pcl_mode2compress_padded(P4(const word * row, const word * end_row, byte * compressed, bool pad)),
    gdev_pcl_mode3compress(P4(int bytecount, const byte * current, byte * previous, byte * compressed)),
    gdev_pcl_mode9compress(P4(int bytecount, const byte * current, const byte * previous, byte * compressed));

#endif /* gdevpcl_INCLUDED */
