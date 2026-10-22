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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HARDCOPY_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HARDCOPY_H

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_PRINTER)

# define PRCOLOR_BLACK	(long_u)0
# define PRCOLOR_WHITE	(long_u)0xFFFFFFL

ਬਾਹਰੀ ਪੂਰਨ	prt_curr_italic;
ਬਾਹਰੀ ਪੂਰਨ	prt_curr_bold;
ਬਾਹਰੀ ਪੂਰਨ	prt_curr_underline;
ਬਾਹਰੀ long_u	prt_curr_bg;
ਬਾਹਰੀ long_u	prt_curr_fg;
ਬਾਹਰੀ ਪੂਰਨ	prt_ਪੰਨਾ_ਗਿਣਤੀ;

# ਜੇ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਅੱਖਰ	*name;
    ਦੁੱਗਣਾ	ਚੌੜਾਈ; // Width and height in points for portrait
    ਦੁੱਗਣਾ	ਉਚਾਈ;
} prt_mediasize_T;

#  define PRT_MEDIASIZE_LEN  14

ਬਾਹਰੀ prt_mediasize_T prt_mediasize[PRT_MEDIASIZE_LEN];
# endif

#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ
