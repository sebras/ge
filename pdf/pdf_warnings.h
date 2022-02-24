/* Copyright (C) 2022 Artifex Software, Inc.
   All Rights Reserved.

   This software is provided AS-IS with no warranty, either express or
   implied.

   This software is distributed under license and may not be copied,
   modified or distributed except as expressly authorized under the terms
   of the license contained in the file LICENSE in this distribution.

   Refer to licensing information at http://www.artifex.com or contact
   Artifex Software, Inc.,  1305 Grant Avenue - Suite 200, Novato,
   CA 94945, U.S.A., +1(415)492-9861, for further information.
*/

#ifndef PARAM
#define PARAM(A,B) A
#endif
PARAM(W_PDF_NOWARNING,	            "no warning"),
PARAM(W_PDF_BAD_XREF_SIZE,          "incorrect xref size"),
PARAM(W_PDF_BAD_INLINEFILTER,       "used inline filter name inappropriately"),
PARAM(W_PDF_BAD_INLINECOLORSPACE,   "used inline colour space inappropriately"),
PARAM(W_PDF_BAD_INLINEIMAGEKEY,     "used inline image key inappropriately"),
PARAM(W_PDF_IMAGE_ERROR,            "recoverable image error"),
PARAM(W_PDF_BAD_IMAGEDICT,          "recoverable error in image dictionary"),
PARAM(W_PDF_TOOMANYQ,               "encountered more Q than q"),
PARAM(W_PDF_TOOMANYq,               "encountered more q than Q"),
PARAM(W_PDF_STACKGARBAGE,           "garbage left on stack"),
PARAM(W_PDF_STACKUNDERFLOW,         "stack underflow"),
PARAM(W_PDF_GROUPERROR,             "error in group definition"),
PARAM(W_PDF_OPINVALIDINTEXT,        "invalid operator used in text block"),
PARAM(W_PDF_NOTINCHARPROC,          "used invalid operator in CharProc"),
PARAM(W_PDF_NESTEDTEXTBLOCK,        "BT found inside a text block"),
PARAM(W_PDF_ETNOTEXTBLOCK,          "ET found outside text block"),
PARAM(W_PDF_TEXTOPNOBT,             "text operator outside text block"),
PARAM(W_PDF_DEGENERATETM,           "degenerate text matrix"),
PARAM(W_PDF_BADICC_USE_ALT,         "bad ICC colour profile, using alternate"),
PARAM(W_PDF_BADICC_USECOMPS,        "bad ICC vs number components, using components"),
PARAM(W_PDF_BADTRSWITCH,            "bad value for text rendering mode"),
PARAM(W_PDF_BADSHADING,             "error in shading"),
PARAM(W_PDF_BADPATTERN,             "error in pattern"),
PARAM(W_PDF_NONSTANDARD_OP,         "non standard operator found - ignoring"),
PARAM(W_PDF_NUM_EXPONENT,           "number uses illegal exponent form"),
PARAM(W_PDF_STREAM_HAS_CONTENTS,    "Stream has inappropriate /Contents entry"),
PARAM(W_PDF_STREAM_BAD_DECODEPARMS, "bad DecodeParms"),
PARAM(W_PDF_MASK_ERROR,             "error in Mask"),
PARAM(W_PDF_ANNOT_AP_ERROR,         "error in annotation Appearance"),
PARAM(W_PDF_BAD_NAME_ESCAPE,        "badly escaped name"),
PARAM(W_PDF_TYPECHECK,              "typecheck error"),
PARAM(W_PDF_BAD_TRAILER,            "bad trailer dictionary"),
PARAM(W_PDF_ANNOT_ERROR,            "error in annotation"),
PARAM(W_PDF_BAD_ICC_PROFILE_LINK,   "failed to create ICC profile link"),
PARAM(W_PDF_OVERFLOW_REAL,          "overflowed a real reading a number, assuming 0"),
PARAM(W_PDF_INVALID_REAL,           "failed to read a valid number, assuming 0"),
PARAM(W_PDF_DEVICEN_USES_ALL,       "A DeviceN space used the /All ink name."),
PARAM(W_PDF_BAD_MEDIABOX,           "Couldn't retrieve MediaBox for page, using current media size"),
PARAM(W_PDF_CA_OUTOFRANGE,          "CA or ca value not in range 0.0 to 1.0, clamped to range."),
PARAM(W_PDF_INVALID_DEFAULTSPACE,   "Invalid DefaultGray, DefaultRGB or DefaultCMYK space specified, ignored."),
PARAM(W_PDF_INVALID_DECRYPT_LEN,    "Invalid /Length supplied in Encryption dictionary."),

#undef PARAM
