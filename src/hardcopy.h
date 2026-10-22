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

#اگر_تعریف_نشده HARDCOPY_H
#تعریف_کن HARDCOPY_H

#ضمن "vim.h"

#إذا defined(FEAT_PRINTER)

# define PRCOLOR_BLACK	(long_u)0
# define PRCOLOR_WHITE	(long_u)0xFFFFFFL

خارجي صحيح	prt_curr_italic;
خارجي صحيح	prt_curr_bold;
خارجي صحيح	prt_curr_underline;
خارجي long_u	prt_curr_bg;
خارجي long_u	prt_curr_fg;
خارجي صحيح	prt_صفحه_شمار;

# إذا defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
تعريفنوع بنية
{
    حرف	*name;
    مزدوج	عرض; // Width and height in points for portrait
    مزدوج	ارتفاع;
} prt_mediasize_T;

#  define PRT_MEDIASIZE_LEN  14

خارجي prt_mediasize_T prt_mediasize[PRT_MEDIASIZE_LEN];
# endif

#پایان_اگر

#پایان_اگر
