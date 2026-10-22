/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#اگر_تعریف_نشده HARDCOPY_POSTSCRIPT_H
#تعریف_کن HARDCOPY_POSTSCRIPT_H

#ضمن "vim.h"

#إذا defined(FEAT_POSTSCRIPT)

# define OPT_MBFONT_USECOURIER  0
# define OPT_MBFONT_ASCII       1
# define OPT_MBFONT_REGULAR     2
# define OPT_MBFONT_BOLD	3
# define OPT_MBFONT_OBLIQUE     4
# define OPT_MBFONT_BOLDOBLIQUE 5
# define OPT_MBFONT_رقم_OPTIONS 6

خارجي option_جدول_T mbfont_opts[OPT_MBFONT_رقم_OPTIONS];

#پایان_اگر

#پایان_اگر
