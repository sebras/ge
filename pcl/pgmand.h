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

/* pgmand.h - Definitions for HP-GL/2 commands and parser */

#ifndef pgmand_INCLUDED
#define pgmand_INCLUDED

#include "stdio_.h"		/* for gdebug.h */
#include <setjmp.h>		/* must come after std.h */
#include "gdebug.h"
#include "pcommand.h"
#include "pcstate.h"
/* Define the type for the HP-GL state. */
typedef struct pcl_state_s hpgl_state_t;

/* Define the type for HP-GL/2 command arguments. */
#ifndef hpgl_parser_state_DEFINED
#  define hpgl_parser_state_DEFINED
typedef struct hpgl_args_s hpgl_parser_state_t;
#endif

typedef struct hpgl_args_s hpgl_args_t;

/* Define a command processing procedure. */
#define hpgl_command_proc(proc)\
  int proc(P2(hpgl_args_t *, hpgl_state_t *))
typedef hpgl_command_proc((*hpgl_command_proc_t));

/*
 * Because HP-GL/2 commands have irregular syntax, we have to let them parse
 * their arguments themselves.  But parsing can be interrupted at any point
 * by reaching an input buffer boundary, so we can't just let the commands
 * call procedures that get the next argument of a given type out of the
 * input stream.  At the same time, we would like to hide buffer boundaries
 * from the individual command implementations to the greatest extent
 * possible.
 *
 * For simple commands that take a maximum number of numeric arguments, we
 * do indeed let the commands call get-argument procedures.  In addition to
 * parsing the input, these procedures save the scanned arguments in an
 * array.  If one of these procedures runs out of input data, it does a
 * longjmp back to the main parser, which exits to its client to request
 * more input.  When the parser regains control, it simply restarts the
 * command procedure from the beginning.  This time, the get-argument
 * procedures return the saved values from the array before continuing to
 * scan the (new) input.  In this way, buffer boundaries cause some extra
 * work, but the command implementations don't notice them.  The only
 * requirement is that a command must not change the permanent state until
 * it has read all of its arguments.
 *
 * For commands that take other kinds of arguments, or a variable
 * (potentially large) number of numeric arguments, the situation is more
 * complicated.  The command is still expected to detect running out of
 * input data, and is still restarted, but it must keep track of its
 * progress itself.  We provide a variable called 'phase' for this
 * purpose: it is set to 0 just after the parser recognizes a command,
 * and is free for the command to use thereafter.
 */
typedef struct hpgl_command_s {
  hpgl_command_proc_t proc;
  byte flags;
#define hpgl_cdf_polygon 1	          /* execute command even in polygon mode */
#define hpgl_cdf_lost_mode_cleared 2      /* exectute command only if lost mode cleared */
#define hpgl_cdf_rtl 4                    /* execute only in rtl mode */
#define hpgl_cdf_pcl 8                    /* execute only in pcl mode */
#define hpgl_cdf_pcl_rtl_both (hpgl_cdf_rtl|hpgl_cdf_pcl)
} hpgl_command_definition_t;

/* Define a HP-GL/2 command argument value. */
typedef struct hpgl_value_s {
  union v_ {
    int32 i;
    hpgl_real_t r;
  } v;
  bool is_real;
} hpgl_value_t;

/* Define the structure passed to HP-GL/2 commands. */
struct hpgl_args_s {
    /* Parsing state */
    stream_cursor_read source;
    int first_letter;		/* -1 if we haven't seen the first letter of */
				/* a command, the letter (0-25) if we have */
    bool done;			/* true if we've seen an argument */
				/* terminator */
    hpgl_command_definition_t *command; /* command being executed, */
				/* 0 if none */
    jmp_buf exit_to_parser;	/* longjmp here if we ran out of data */
				/* while scanning an argument, or we */
				/* found a syntax error */
    struct arg_ {			/* argument scanning state */
	/* State within the current argument */
	int have_value;		/* 0 = no value, 1 = int, 2 = real */
	double frac_scale;		/* 10 ^ # of digits after dot */
	int sign;			/* 0 = none, +/-1 = sign */
	/* State of argument list collection */
	int count;			/* # of fully scanned arguments */
	int next;			/* # of next scanned arg to return */
	hpgl_value_t scanned[21];	/* args already scanned */
    } arg;
    /* Command execution state */
    int phase;			/* phase within command, see above */

    /*
     * We register all the HP-GL/2 commands dynamically, for maximum
     * configuration flexibility.  hpgl_command_list points to the individual
     * command definitions; as each command is registered, we enter it in the
     * list, and then store its index in the actual dispatch table
     * (hpgl_command_indices).
     */
    hpgl_command_definition_t *hpgl_command_list[100];
    int hpgl_command_next_index;
    /* Dispatch tables */
    byte hpgl_command_indices[26][26];
};

/* Register HP-GL/2 commands. */
typedef struct {
  char char1, char2;
  hpgl_command_definition_t defn;
} hpgl_named_command_t;
int hpgl_init_command_index(P2(hpgl_parser_state_t **pgl_parser_state, gs_memory_t *mem));
void hpgl_define_commands(P2(const hpgl_named_command_t *, hpgl_parser_state_t *pgl_parser_state));
#define DEFINE_HPGL_COMMANDS \
{ static const hpgl_named_command_t defs_[] = {
#define HPGL_COMMAND(c1, c2, proc, flag)\
  {c1, c2, {proc, flag}}
#define END_HPGL_COMMANDS\
    {0, 0}\
  };\
  hpgl_define_commands(defs_, pcl_parser_state->hpgl_parser_state);\
}

/* Define a return code asking for more data. */
#define e_NeedData (-200)

/* Initialize the HP-GL/2 parser. */
void hpgl_process_init(P1(hpgl_parser_state_t *));

/* Process a buffer of HP-GL/2 commands. */
/* Return 0 if more input needed, 1 if ESC seen, or an error code. */
int hpgl_process(P3(hpgl_parser_state_t *pst, hpgl_state_t *pgls,
		    stream_cursor_read *pr));

/* Prepare to scan the next (numeric) argument. */
#define hpgl_arg_init(pargs)\
  ((pargs)->arg.have_value = 0, (pargs)->arg.sign = 0)

/* Prepare to scan a sequence of numeric arguments.  Execute this */
/* once per sequence, when incrementing the phase. */
#define hpgl_args_init(pargs)\
  (hpgl_arg_init(pargs), (pargs)->arg.count = 0)
/* Move to the next phase of a command. */
#define hpgl_next_phase(pargs)\
  ((pargs)->phase++, hpgl_args_init(pargs))

/*
 * Scan a numeric argument, returning true if there was one, or false
 * if we have reached the end of the arguments.  Note that these
 * procedures longjmp back to the parser if they run out of input data.
 */
bool hpgl_arg_real(P2(hpgl_args_t *pargs, hpgl_real_t *pr));
bool hpgl_arg_c_real(P2(hpgl_args_t *pargs, hpgl_real_t *pr));
bool hpgl_arg_int(P2(hpgl_args_t *pargs, int32 *pi));
bool hpgl_arg_c_int(P2(hpgl_args_t *pargs, int *pi));
/*
 * Many vector commands are defined as taking parameters whose format is
 * "current units".  This is nonsensical: whether scaling is on or off
 * has no bearing on whether coordinates are represented as integers or
 * reals.  Nevertheless, in deference to this definition (and just in case
 * it turns out it actually matters), we define a separate procedure for
 * this.
 */
bool hpgl_arg_units(P2(hpgl_args_t *pargs, hpgl_real_t *pu));

/* In some cases, it is convenient for the implementation of command A to
 * call another command B.  While we don't particularly like this approach
 * in general, we do support it, as long as command B doesn't do its own
 * argument parsing (e.g., PE, LB).  The framework for doing this is the
 * following:
 *	hpgl_args_t args;
 *	...
 *	hpgl_args_setup(&args);
 *	<< As many times as desired: >>
 *		hpgl_args_add_int/real(&args, value);
 *	hpgl_B(&args, pgls);
 */
#define args_setup_count_(pargs, numargs)\
  ((void)((pargs)->done = true, (pargs)->arg.count = (numargs),\
	  (pargs)->arg.next = (pargs)->phase = 0))
#define hpgl_args_setup(pargs)\
  args_setup_count_(pargs, 0)
#define args_put_int_(pargs, index, iplus, ival)\
  ((void)((pargs)->arg.scanned[index].v.i = (ival),\
	  (pargs)->arg.scanned[iplus].is_real = false))
#define hpgl_args_add_int(pargs, ival)\
  args_put_int_(pargs, (pargs)->arg.count, (pargs)->arg.count++, ival)
#define args_put_real_(pargs, index, iplus, rval)\
  ((void)((pargs)->arg.scanned[index].v.r = (rval),\
	  (pargs)->arg.scanned[iplus].is_real = true))
#define hpgl_args_add_real(pargs, rval)\
  args_put_real_(pargs, (pargs)->arg.count, (pargs)->arg.count++, rval)
/*
 * We provide shortcuts for commands that take just 1 or 2 arguments.
 */
#define hpgl_args_set_int(pargs, ival)\
  (args_setup_count_(pargs, 1), args_put_int_(pargs, 0, 0, ival))
#define hpgl_args_set_real(pargs, rval)\
  (args_setup_count_(pargs, 1), args_put_real_(pargs, 0, 0, rval))
#define hpgl_args_set_real2(pargs, rval1, rval2)\
  (args_setup_count_(pargs, 2), args_put_real_(pargs, 0, 0, rval1),\
   args_put_real_(pargs, 1, 1, rval2))

/*
 * HPGL mnemonics
 */

/* commands from pgchar.c -- HP-GL/2 character commands */
int hpgl_AD(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_CF(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_CP(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_DI(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_DR(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_DT(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_DV(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_ES(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_FI(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_FN(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_LB(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_LM(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_LO(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_SA(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_SB(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_SD(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_SI(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_SL(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_SR(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_SS(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_TD(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));

/* commands from pgcolor.c - HP-GL/2 color vector graphics commands */
int hpgl_MC(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_NP(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_PP(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));

/* commands from pgconfig.c - HP-GL/2 configuration and status commands */
int hpgl_CO(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_DF(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));

int hpgl_IN(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_IN_implicit(P1(hpgl_state_t *pgls));

int hpgl_IP(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_IR(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_IW(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_PG(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_RO(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_RP(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_SC(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));

/* commands from pglfill.c - HP-GL/2 line and fill attributes commands */
int hpgl_AC(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_FT(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_LA(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_LT(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_PW(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_RF(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_SM(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_SP(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_SV(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_TR(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_UL(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_WU(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));

/* commands from pgpoly.c -- HP-GL/2 polygon commands */
int hpgl_EA(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_EP(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_ER(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_EW(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_FP(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_PM(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_RA(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_RR(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_WG(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));

/* commands from pgvector.c - HP-GL/2 vector commands */
int hpgl_AA(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_AR(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_AT(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_BR(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_BZ(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_CI(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_PA(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_PD(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_PE(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_PR(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_PU(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));
int hpgl_RT(P2(hpgl_args_t *pargs, hpgl_state_t *pgls));

/* commands from pgframe.c -- PCL5/HP-GL/2 picture frame commands */
int pcl_horiz_pic_frame_size_decipoints(P2(pcl_args_t *pargs, hpgl_state_t *pgls));
int pcl_vert_pic_frame_size_decipoints(P2(pcl_args_t *pargs, hpgl_state_t *pgls));
int pcl_set_pic_frame_anchor_point(P2(pcl_args_t *pargs, hpgl_state_t *pgls));
int pcl_hpgl_plot_horiz_size(P2(pcl_args_t *pargs, hpgl_state_t *pgls));
int pcl_hpgl_plot_vert_size(P2(pcl_args_t *pargs, hpgl_state_t *pgls));

/* reset required for overlay macros - a partial DF command */
void hpgl_reset_overlay(P1(hpgl_state_t *pgls));

/* this should find a new home but for now we list it here. */
int hpgl_print_symbol_mode_char(P1(hpgl_state_t *pgls));

/* reset LT parameters */
void hpgl_set_line_pattern_defaults(P1(hpgl_state_t *pgls));

/* reset LA parameters */
void hpgl_set_line_attribute_defaults(P1(hpgl_state_t *pgls));

void hpgl_free_stick_fonts(P1(hpgl_state_t *pgls));
#endif                                         /* pgmand_INCLUDED */
