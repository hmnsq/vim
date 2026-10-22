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

#अन्तर्भाव "vim.h"

#यदि defined(FEAT_PRINTER)

# define PRCOLOR_BLACK	(long_u)0
# define PRCOLOR_WHITE	(long_u)0xFFFFFFL

बाह्य पूर्णाङ्क	prt_curr_italic;
बाह्य पूर्णाङ्क	prt_curr_bold;
बाह्य पूर्णाङ्क	prt_curr_underline;
बाह्य long_u	prt_curr_bg;
बाह्य long_u	prt_curr_fg;
बाह्य पूर्णाङ्क	prt_page_मोजणी;

# यदि defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
प्रकारनिर्देश संरचना
{
    वर्ण	*name;
    द्विगुण	width; // Width and height in points for portrait
    द्विगुण	height;
} prt_mediasize_T;

#  define PRT_MEDIASIZE_LEN  14

बाह्य prt_mediasize_T prt_mediasize[PRT_MEDIASIZE_LEN];
# endif

#endif

#endif
