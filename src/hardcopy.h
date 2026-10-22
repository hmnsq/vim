/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * hardcopy.h: common stuff for hardcopy feature
 */

#ifndef HARDCOPY_H
#define HARDCOPY_H

#include "vim.h"

#གལ་ཏེ defined(FEAT_PRINTER)

# define PRCOLOR_BLACK	(long_u)0
# define PRCOLOR_WHITE	(long_u)0xFFFFFFL

extern ཧྲིལ	prt_curr_italic;
extern ཧྲིལ	prt_curr_bold;
extern ཧྲིལ	prt_curr_underline;
extern long_u	prt_curr_bg;
extern long_u	prt_curr_fg;
extern ཧྲིལ	prt_page_count;

# གལ་ཏེ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
typedef sa_nracanaa
{
    var_na	*name;
    dviguu_na	width; // Width and height in points for portrait
    dviguu_na	height;
} prt_mediasize_T;

#  define PRT_MEDIASIZE_LEN  14

extern prt_mediasize_T prt_mediasize[PRT_MEDIASIZE_LEN];
# endif

#endif

#endif
