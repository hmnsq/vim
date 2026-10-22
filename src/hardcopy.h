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

#অন্তর্ভাৱ "vim.h"

#যদি defined(FEAT_PRINTER)

# define PRCOLOR_BLACK	(long_u)0
# define PRCOLOR_WHITE	(long_u)0xFFFFFFL

বাহ্য পূর্ণাঙ্ক	prt_curr_italic;
বাহ্য পূর্ণাঙ্ক	prt_curr_bold;
বাহ্য পূর্ণাঙ্ক	prt_curr_underline;
বাহ্য long_u	prt_curr_bg;
বাহ্য long_u	prt_curr_fg;
বাহ্য পূর্ণাঙ্ক	prt_page_গুনতি;

# যদি defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
প্রকারনির্দেশ সংরচনা
{
    ৱর্ণ	*name;
    দ্ৱিগুণ	width; // Width and height in points for portrait
    দ্ৱিগুণ	height;
} prt_mediasize_T;

#  define PRT_MEDIASIZE_LEN  14

বাহ্য prt_mediasize_T prt_mediasize[PRT_MEDIASIZE_LEN];
# endif

#endif

#endif
