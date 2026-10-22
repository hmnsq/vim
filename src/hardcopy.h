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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_PRINTER)

# define PRCOLOR_BLACK	(long_u)0
# define PRCOLOR_WHITE	(long_u)0xFFFFFFL

બાહ્ય પૂર્ણાંક	prt_curr_italic;
બાહ્ય પૂર્ણાંક	prt_curr_bold;
બાહ્ય પૂર્ણાંક	prt_curr_underline;
બાહ્ય long_u	prt_curr_bg;
બાહ્ય long_u	prt_curr_fg;
બાહ્ય પૂર્ણાંક	prt_page_ગણતરી;

# જો defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
પ્રકારવ્યાખ્યા સંરચના
{
    અક્ષર	*name;
    દ્વિગુણ	width; // Width and height in points for portrait
    દ્વિગુણ	height;
} prt_mediasize_T;

#  define PRT_MEDIASIZE_LEN  14

બાહ્ય prt_mediasize_T prt_mediasize[PRT_MEDIASIZE_LEN];
# endif

#endif

#endif
