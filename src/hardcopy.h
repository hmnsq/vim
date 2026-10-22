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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_PRINTER)

# define PRCOLOR_BLACK	(long_u)0
# define PRCOLOR_WHITE	(long_u)0xFFFFFFL

ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା	prt_curr_italic;
ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା	prt_curr_bold;
ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା	prt_curr_underline;
ବାହ୍ୟ long_u	prt_curr_bg;
ବାହ୍ୟ long_u	prt_curr_fg;
ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା	prt_page_ଗଣନା;

# ଯଦି defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    ଅକ୍ଷର	*name;
    ଦ୍ୱିଗୁଣ	width; // Width and height in points for portrait
    ଦ୍ୱିଗୁଣ	height;
} prt_mediasize_T;

#  define PRT_MEDIASIZE_LEN  14

ବାହ୍ୟ prt_mediasize_T prt_mediasize[PRT_MEDIASIZE_LEN];
# endif

#endif

#endif
