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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HARDCOPY_H
#ವ್ಯಾಖ್ಯಾನಿಸು HARDCOPY_H

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_PRINTER)

# define PRCOLOR_BLACK	(long_u)0
# define PRCOLOR_WHITE	(long_u)0xFFFFFFL

ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ	prt_curr_italic;
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ	prt_curr_bold;
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ	prt_curr_underline;
ಬಾಹ್ಯ long_u	prt_curr_bg;
ಬಾಹ್ಯ long_u	prt_curr_fg;
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ	prt_ಪುಟ_ಎಣಿಕೆ;

# ಆದರೆ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಅಕ್ಷರ	*name;
    ದ್ವಿಗುಣ	ಅಗಲ; // Width and height in points for portrait
    ದ್ವಿಗುಣ	ಎತ್ತರ;
} prt_mediasize_T;

#  define PRT_MEDIASIZE_LEN  14

ಬಾಹ್ಯ prt_mediasize_T prt_mediasize[PRT_MEDIASIZE_LEN];
# endif

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ
