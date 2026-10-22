/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HARDCOPY_POSTSCRIPT_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HARDCOPY_POSTSCRIPT_H

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_POSTSCRIPT)

# define OPT_MBFONT_USECOURIER  0
# define OPT_MBFONT_ASCII       1
# define OPT_MBFONT_REGULAR     2
# define OPT_MBFONT_BOLD	3
# define OPT_MBFONT_OBLIQUE     4
# define OPT_MBFONT_BOLDOBLIQUE 5
# define OPT_MBFONT_ਅੰਕ_OPTIONS 6

ਬਾਹਰੀ option_ਸਾਰਣੀ_T mbfont_opts[OPT_MBFONT_ਅੰਕ_OPTIONS];

#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ
