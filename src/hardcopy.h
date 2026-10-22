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

#నిర్వచితం_కాకపోతే HARDCOPY_H
#నిర్వచించు HARDCOPY_H

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_PRINTER)

# define PRCOLOR_BLACK	(long_u)0
# define PRCOLOR_WHITE	(long_u)0xFFFFFFL

బాహ్య పూర్ణాంకం	prt_curr_italic;
బాహ్య పూర్ణాంకం	prt_curr_bold;
బాహ్య పూర్ణాంకం	prt_curr_underline;
బాహ్య long_u	prt_curr_bg;
బాహ్య long_u	prt_curr_fg;
బాహ్య పూర్ణాంకం	prt_పుట_లెక్క;

# ఒకవేళ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
రకనిర్వచనం నిర్మాణం
{
    అక్షరం	*name;
    ద్విగుణ	వెడల్పు; // Width and height in points for portrait
    ద్విగుణ	ఎత్తు;
} prt_mediasize_T;

#  define PRT_MEDIASIZE_LEN  14

బాహ్య prt_mediasize_T prt_mediasize[PRT_MEDIASIZE_LEN];
# endif

#అయితే_ముగింపు

#అయితే_ముగింపు
