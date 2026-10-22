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

#شامل "vim.h"

#اگر defined(FEAT_PRINTER)

# define PRCOLOR_BLACK	(long_u)0
# define PRCOLOR_WHITE	(long_u)0xFFFFFFL

بیرونی صحیح	prt_curr_italic;
بیرونی صحیح	prt_curr_bold;
بیرونی صحیح	prt_curr_underline;
بیرونی long_u	prt_curr_bg;
بیرونی long_u	prt_curr_fg;
بیرونی صحیح	prt_page_شمار;

# اگر defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
تعریفنوع ساختار
{
    نویسه	*name;
    دوگانه	width; // Width and height in points for portrait
    دوگانه	height;
} prt_mediasize_T;

#  define PRT_MEDIASIZE_LEN  14

بیرونی prt_mediasize_T prt_mediasize[PRT_MEDIASIZE_LEN];
# endif

#endif

#endif
