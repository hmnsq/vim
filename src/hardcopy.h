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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_PRINTER)

# define PRCOLOR_BLACK	(long_u)0
# define PRCOLOR_WHITE	(long_u)0xFFFFFFL

ബാഹ്യ പൂർണ്ണസംഖ്യ	prt_curr_italic;
ബാഹ്യ പൂർണ്ണസംഖ്യ	prt_curr_bold;
ബാഹ്യ പൂർണ്ണസംഖ്യ	prt_curr_underline;
ബാഹ്യ long_u	prt_curr_bg;
ബാഹ്യ long_u	prt_curr_fg;
ബാഹ്യ പൂർണ്ണസംഖ്യ	prt_page_എണ്ണം;

# എങ്കിൽ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
തരനിർവചനം ഘടന
{
    അക്ഷരം	*name;
    ഇരട്ട	width; // Width and height in points for portrait
    ഇരട്ട	height;
} prt_mediasize_T;

#  define PRT_MEDIASIZE_LEN  14

ബാഹ്യ prt_mediasize_T prt_mediasize[PRT_MEDIASIZE_LEN];
# endif

#endif

#endif
