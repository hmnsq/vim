/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * macros.h: macro definitions for often used code
 *
 * Macros should be ALL_CAPS.  An exception is for where a function is
 * replaced and an argument is not used more than once.
 */

/*
 * Position comparisons
 */
#ವ್ಯಾಖ್ಯಾನಿಸು LT_POS(a, b) (((a).lnum != (b).lnum) \
		   ? (a).lnum < (b).lnum \
		   : (a).ಲಂಬಸಾಲು != (b).ಲಂಬಸಾಲು \
		       ? (a).ಲಂಬಸಾಲು < (b).ಲಂಬಸಾಲು \
		       : (a).coladd < (b).coladd)
#ವ್ಯಾಖ್ಯಾನಿಸು LT_POSP(a, b) (((a)->lnum != (b)->lnum) \
		   ? (a)->lnum < (b)->lnum \
		   : (a)->ಲಂಬಸಾಲು != (b)->ಲಂಬಸಾಲು \
		       ? (a)->ಲಂಬಸಾಲು < (b)->ಲಂಬಸಾಲು \
		       : (a)->coladd < (b)->coladd)
#ವ್ಯಾಖ್ಯಾನಿಸು EQUAL_POS(a, b) (((a).lnum == (b).lnum) && ((a).ಲಂಬಸಾಲು == (b).ಲಂಬಸಾಲು) && ((a).coladd == (b).coladd))
#ವ್ಯಾಖ್ಯಾನಿಸು CLEAR_POS(a) ಮಾಡು {(a)->lnum = 0; (a)->ಲಂಬಸಾಲು = 0; (a)->coladd = 0;} ಆಗ (0)
#ವ್ಯಾಖ್ಯಾನಿಸು ಖಾಲಿಗಳು_POS(a) ((a).lnum == 0 && (a).ಲಂಬಸಾಲು == 0 && (a).coladd == 0)

#ವ್ಯಾಖ್ಯಾನಿಸು LTOREQ_POS(a, b) (LT_POS(a, b) || EQUAL_POS(a, b))

/*
 * VIM_ISWHITE() differs from isspace() because it doesn't include <CR> and
 * <LF> and the like.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_ISWHITE(x)		((x) == ' ' || (x) == '\t')
#ವ್ಯಾಖ್ಯಾನಿಸು IS_WHITE_OR_NUL(x)	((x) == ' ' || (x) == '\t' || (x) == NUL)
#ವ್ಯಾಖ್ಯಾನಿಸು IS_WHITE_NL_OR_NUL(x)	((x) == ' ' || (x) == '\t' || (x) == '\n' || (x) == NUL)

/*
 * LINEEMPTY() - return TRUE if the line is empty
 */
#ವ್ಯಾಖ್ಯಾನಿಸು LINEEMPTY(p) (*ml_get(p) == NUL)

/*
 * BUFEMPTY() - return TRUE if the current buffer is empty
 */
#ವ್ಯಾಖ್ಯಾನಿಸು BUFEMPTY() (curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ == 1 && *ml_get((linenr_T)1) == NUL)

// The is*() and to*() functions declared in <ctype.h> have
// undefined behavior for values other than EOF outside the range of
// unsigned char.  If plain char is signed, a call with a negative
// value has undefined behavior.  These macros cast the argument to
// unsigned char.  (Most implementations behave more or less sanely
// with negative values, and most character values in practice are
// positive, but we want to avoid undefined behavior anyway.)
#ವ್ಯಾಖ್ಯಾನಿಸು ಸುರಕ್ಷಿತ_isalnum(c)  (ಅಕ್ಷರವೇ_ಅಂಕಿಯೇ ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(c)))
#ವ್ಯಾಖ್ಯಾನಿಸು ಸುರಕ್ಷಿತ_isalpha(c)  (ಅಕ್ಷರವೇ ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(c)))
#ವ್ಯಾಖ್ಯಾನಿಸು ಸುರಕ್ಷಿತ_isblank(c)  (isblank ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(c)))
#ವ್ಯಾಖ್ಯಾನಿಸು ಸುರಕ್ಷಿತ_iscntrl(c)  (iscntrl ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(c)))
#ವ್ಯಾಖ್ಯಾನಿಸು ಸುರಕ್ಷಿತ_isdigit(c)  (ಅಂಕಿಯೇ ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(c)))
#ವ್ಯಾಖ್ಯಾನಿಸು ಸುರಕ್ಷಿತ_isgraph(c)  (isgraph ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(c)))
#ವ್ಯಾಖ್ಯಾನಿಸು ಸುರಕ್ಷಿತ_islower(c)  (islower ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(c)))
#ವ್ಯಾಖ್ಯಾನಿಸು ಸುರಕ್ಷಿತ_isprint(c)  (isprint ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(c)))
#ವ್ಯಾಖ್ಯಾನಿಸು ಸುರಕ್ಷಿತ_ispunct(c)  (ispunct ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(c)))
#ವ್ಯಾಖ್ಯಾನಿಸು ಸುರಕ್ಷಿತ_isspace(c)  (ಖಾಲಿಯೇ ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(c)))
#ವ್ಯಾಖ್ಯಾನಿಸು ಸುರಕ್ಷಿತ_isupper(c)  (isupper ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(c)))
#ವ್ಯಾಖ್ಯಾನಿಸು ಸುರಕ್ಷಿತ_isxdigit(c) (isxdigit((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(c)))
#ವ್ಯಾಖ್ಯಾನಿಸು ಸುರಕ್ಷಿತ_tolower(c)  (tolower ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(c)))
#ವ್ಯಾಖ್ಯಾನಿಸು ಸುರಕ್ಷಿತ_toupper(c)  (ದೊಡ್ಡ_ಅಕ್ಷರ ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(c)))

/*
 * toupper() and tolower() that use the current locale.
 * On some systems toupper()/tolower() only work on lower/uppercase
 * characters, first use islower() or isupper() then.
 * Careful: Only call TOUPPER_LOC() and TOLOWER_LOC() with a character in the
 * range 0 - 255.  toupper()/tolower() on some systems can't handle others.
 * Note: It is often better to use MB_TOLOWER() and MB_TOUPPER(), because many
 * toupper() and tolower() implementations only work for ASCII.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
# define TOUPPER_LOC(c)	toupper_tab[(c) & 255]
# define TOLOWER_LOC(c)	tolower_tab[(c) & 255]
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef BROKEN_TOUPPER
#  define TOUPPER_LOC(c)	(ಸುರಕ್ಷಿತ_islower(c) ? ಸುರಕ್ಷಿತ_toupper(c) : (c))
#  define TOLOWER_LOC(c)	(ಸುರಕ್ಷಿತ_isupper(c) ? ಸುರಕ್ಷಿತ_tolower(c) : (c))
# ಇಲ್ಲದಿದ್ದರೆ
#  define TOUPPER_LOC		ಸುರಕ್ಷಿತ_toupper
#  define TOLOWER_LOC		ಸುರಕ್ಷಿತ_tolower
# endif
#ಆದರೆ_ಅಂತ್ಯ

// toupper() and tolower() for ASCII only and ignore the current locale.
#ವ್ಯಾಖ್ಯಾನಿಸು TOUPPER_ASC(c)	(((c) < 'a' || (c) > 'z') ? (c) : (c) - ('a' - 'A'))
#ವ್ಯಾಖ್ಯಾನಿಸು TOLOWER_ASC(c)	(((c) < 'A' || (c) > 'Z') ? (c) : (c) + ('a' - 'A'))

/*
 * MB_ISLOWER() and MB_ISUPPER() are to be used on multi-byte characters.  But
 * don't use them for negative values!
 */
#ವ್ಯಾಖ್ಯಾನಿಸು MB_ISLOWER(c)	vim_islower(c)
#ವ್ಯಾಖ್ಯಾನಿಸು MB_ISUPPER(c)	vim_isupper(c)
#ವ್ಯಾಖ್ಯಾನಿಸು MB_TOLOWER(c)	vim_tolower(c)
#ವ್ಯಾಖ್ಯಾನಿಸು MB_TOUPPER(c)	vim_toupper(c)
#ವ್ಯಾಖ್ಯಾನಿಸು MB_CASEFOLD(c)	(enc_utf8 ? utf_fold(c) : MB_TOLOWER(c))

// Use our own isdigit() replacement, because on MS-Windows isdigit() returns
// non-zero for superscript 1.  Also avoids that isdigit() crashes for numbers
// below 0 and above 255.
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_ISDIGIT(c) ((ಚಿಹ್ನೆರಹಿತ)(c) - '0' < 10)

// Like isalpha() but reject non-ASCII characters.  Can't be used with a
// special key (negative value).
#ವ್ಯಾಖ್ಯಾನಿಸು ASCII_ISLOWER(c) ((ಚಿಹ್ನೆರಹಿತ)(c) - 'a' < 26)
#ವ್ಯಾಖ್ಯಾನಿಸು ASCII_ISUPPER(c) ((ಚಿಹ್ನೆರಹಿತ)(c) - 'A' < 26)
#ವ್ಯಾಖ್ಯಾನಿಸು ASCII_ISALPHA(c) (ASCII_ISUPPER(c) || ASCII_ISLOWER(c))
#ವ್ಯಾಖ್ಯಾನಿಸು ASCII_ISALNUM(c) (ASCII_ISALPHA(c) || VIM_ISDIGIT(c))

// Returns empty string if it is NULL.
#ವ್ಯಾಖ್ಯಾನಿಸು ಖಾಲಿಗಳು_IF_NULL(x) ((x) ? (x) : (char_u *)"")

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LANGMAP
/*
 * Adjust chars in a language according to 'langmap' option.
 * NOTE that there is no noticeable overhead if 'langmap' is not set.
 * When set the overhead for characters < 256 is small.
 * Don't apply 'langmap' if the character comes from the Stuff buffer or from
 * a mapping and the langnoremap option was set.
 * The do-while is just to ignore a ';' after the macro.
 */
# define LANGMAP_ADJUST(c, condition) \
    ಮಾಡು { \
	ಆದರೆ (*p_langmap \
		&& (condition) \
		&& (p_lrm || (!p_lrm && ಕೀಲಿ_Typed)) \
		&& !ಕೀಲಿ_Stuffed \
		&& (c) >= 0) \
	{ \
	    ಆದರೆ ((c) < 256) \
		c = langmap_mapchar[c]; \
	    ಇಲ್ಲದಿದ್ದರೆ \
		c = langmap_adjust_mb(c); \
	} \
    } ಆಗ (0)
#ಇಲ್ಲದಿದ್ದರೆ
# define LANGMAP_ADJUST(c, condition) // nop
#ಆದರೆ_ಅಂತ್ಯ

/*
 * VIM_ISBREAK() is used very often if 'linebreak' is set, use a macro to make
 * it work fast.  Only works for single byte characters!
 */
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_ISBREAK(c) ((c) < 256 && breakat_flags[(char_u)(c)])

/*
 * On VMS file names are different and require a translation.
 * On the Mac open() has only two arguments.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
# define mch_access(n, p)	access(vms_fixfilename(n), (p))
				// see mch_open() comment
# define mch_fopen(n, p)	fopen(vms_fixfilename(n), (p))
# define mch_fstat(n, p)	fstat((n), (p))
# undef HAVE_LSTAT		// VMS does not have lstat()
# define mch_stat(n, p)		stat(vms_fixfilename(n), (p))
#ಇಲ್ಲದಿದ್ದರೆ
# ಆದರೆ !defined(MSWIN) && !defined(PROTO)
#  define mch_access(n, p)	access((n), (p))
# endif

// Use 64-bit fstat function on MS-Windows.
// NOTE: This condition is the same as for the stat_T type.
# ifdef MSWIN
#  define mch_fstat(n, p)	_fstat64((n), (p))
# ಇಲ್ಲದಿದ್ದರೆ
#  define mch_fstat(n, p)	fstat((n), (p))
# endif

# ifdef MSWIN	// has its own mch_stat() function
#  define mch_stat(n, p)	vim_stat((n), (p))
# ಇಲ್ಲದಿದ್ದರೆ
#  ifdef STAT_IGNORES_SLASH
#   define mch_stat(n, p)	vim_stat((n), (p))
#  ಇಲ್ಲದಿದ್ದರೆ
#   define mch_stat(n, p)	stat((n), (p))
#  endif
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_LSTAT
# define mch_lstat(n, p)	lstat((n), (p))
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef MSWIN
#  define mch_lstat(n, p)	vim_lstat((n), (p))
# ಇಲ್ಲದಿದ್ದರೆ
#  define mch_lstat(n, p)	mch_stat((n), (p))
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
/*
 * It is possible to force some record format with:
 * #  define mch_open(n, m, p) open(vms_fixfilename(n), (m), (p)), "rat=cr", "rfm=stmlf", "mrs=0")
 * but it is not recommended, because it can destroy indexes etc.
 */
# define mch_open(n, m, p)	open(vms_fixfilename(n), (m), (p))
#ಆದರೆ_ಅಂತ್ಯ

// mch_open_rw(): invoke mch_open() with third argument for user R/W.
#ಆದರೆ defined(UNIX) || defined(VMS)  // open in rw------- mode
# define mch_open_rw(n, f)	mch_open((n), (f), (mode_t)0600)
#ಇಲ್ಲದಿದ್ದರೆ
# ಆದರೆ defined(MSWIN)  // open read/write
#  define mch_open_rw(n, f)	mch_open((n), (f), S_IREAD | S_IWRITE)
# ಇಲ್ಲದಿದ್ದರೆ
#  define mch_open_rw(n, f)	mch_open((n), (f), 0)
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ STARTUPTIME
# define ಸಮಯ_ಸಂದೇಶ(s) ಮಾಡು { ಆದರೆ (ಸಮಯ_fd != ಖಾಲಿ) ಸಮಯ_ಸಂದೇಶ_2(s, ಖಾಲಿ); } ಆಗ (0)
#ಇಲ್ಲದಿದ್ದರೆ
# define ಸಮಯ_ಸಂದೇಶ(s) ಮಾಡು { /**/ } ಆಗ (0)
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು REPLACE_ಸಾಮಾನ್ಯ(s) (((s) & REPLACE_FLAG) && !((s) & VREPLACE_FLAG))

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARABIC
# define ARABIC_CHAR(ch)	    (((ch) & 0xFF00) == 0x0600)
# define UTF_COMPOSINGLIKE(p1, p2)  utf_composinglike((p1), (p2))
#ಇಲ್ಲದಿದ್ದರೆ
# define UTF_COMPOSINGLIKE(p1, p2)  utf_iscomposing(utf_ptr2char(p2))
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    // Whether to draw the vertical bar on the right side of the cell.
# define CURSOR_BAR_ಬಲ (curwin->w_p_rl && (!(State & ಬಹುಲಕ_CMDLINE) || cmdmsg_rl))
#ಆದರೆ_ಅಂತ್ಯ

/*
 * MB_PTR_ADV(): advance a pointer to the next character, taking care of
 * multi-byte characters if needed.
 * MB_PTR_BACK(): backup a pointer to the previous character, taking care of
 * multi-byte characters if needed.
 * MB_COPY_CHAR(f, t): copy one char from "f" to "t" and advance the pointers.
 * PTR2CHAR(): get character from pointer.
 */
// Advance multi-byte pointer, skip over composing chars.
#ವ್ಯಾಖ್ಯಾನಿಸು MB_PTR_ADV(p)	    p += (*mb_ptr2len)(p)
// Advance multi-byte pointer, do not skip over composing chars.
#ವ್ಯಾಖ್ಯಾನಿಸು MB_CPTR_ADV(p)	    p += enc_utf8 ? utf_ptr2len(p) : (*mb_ptr2len)(p)
// Backup multi-byte pointer. Only use with "p" > "s" !
#ವ್ಯಾಖ್ಯಾನಿಸು MB_PTR_ಹಿಂಭಾಗ(s, p)  p -= has_mbyte ? ((*mb_ತಲೆ_off)(s, (p) - 1) + 1) : 1
// get length of multi-byte char, not including composing chars
#ವ್ಯಾಖ್ಯಾನಿಸು MB_CPTR2LEN(p)	    (enc_utf8 ? utf_ptr2len(p) : (*mb_ptr2len)(p))

#ವ್ಯಾಖ್ಯಾನಿಸು MB_COPY_CHAR(f, t) ಮಾಡು { ಆದರೆ (has_mbyte) mb_copy_char(&(f), &(t)); ಇಲ್ಲದಿದ್ದರೆ *(t)++ = *(f)++; } ಆಗ (0)
#ವ್ಯಾಖ್ಯಾನಿಸು MB_CHARLEN(p)	    (has_mbyte ? mb_charlen(p) : (ಪೂರ್ಣಾಂಕ)STRLEN(p))
#ವ್ಯಾಖ್ಯಾನಿಸು MB_CHAR2LEN(c)	    (has_mbyte ? mb_char2len(c) : 1)
#ವ್ಯಾಖ್ಯಾನಿಸು PTR2CHAR(p)	    (has_mbyte ? mb_ptr2char(p) : (ಪೂರ್ಣಾಂಕ)*(p))
#ವ್ಯಾಖ್ಯಾನಿಸು MB_CHAR2_ಬೈಟುಗಳು(c, b) ಮಾಡು { ಆದರೆ (has_mbyte) (b) += (*mb_char2bytes)((c), (b)); ಇಲ್ಲದಿದ್ದರೆ *(b)++ = (c); } ಆಗ (0)

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_AUTOCHDIR
# define DO_AUTOCHDIR ಮಾಡು { ಆದರೆ (p_acd) do_autochdir(); } ಆಗ (0)
#ಇಲ್ಲದಿದ್ದರೆ
# define DO_AUTOCHDIR ಮಾಡು { /**/ } ಆಗ (0)
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು RESET_BINDING(wp)  ಮಾಡು { (wp)->w_p_scb = FALSE; (wp)->w_p_crb = FALSE; \
			    } ಆಗ (0)

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
# define PLINES_NOFILL(x) plines_nofill(x)
# define PLINES_WIN_NOFILL(w, l, h) plines_win_nofill((w), (l), (h))
#ಇಲ್ಲದಿದ್ದರೆ
# define PLINES_NOFILL(x) plines(x)
# define PLINES_WIN_NOFILL(w, l, h) plines_win((w), (l), (h))
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_ಕೆಲಸ_CHANNEL) || defined(FEAT_CLIENTSERVER)
# define ಸಂದೇಶ_ಸರತಿ
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <float.h>
#ಆದರೆ defined(HAVE_MATH_H)
  // for isnan() and isinf()
# include <math.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL)
# ifdef USING_FLOAT_STUFF
#  ifdef MSWIN
#   ifndef isnan
#    define isnan(x) _isnan(x)
     ಸ್ಥಿರ __inline ಪೂರ್ಣಾಂಕ isinf(ದ್ವಿಗುಣ x)
	{ ಹಿಂತಿರುಗಿ !_finite(x) && !_isnan(x); }
#   endif
#  ಇಲ್ಲದಿದ್ದರೆ
#   ifndef HAVE_ISNAN
     ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ isnan(ದ್ವಿಗುಣ x)
	{ ಹಿಂತಿರುಗಿ x != x; }
#   endif
#   ifndef HAVE_ISINF
     ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ isinf(ದ್ವಿಗುಣ x)
	{ ಹಿಂತಿರುಗಿ !isnan(x) && isnan(x - x); }
#   endif
#  endif
#  ಆದರೆ !defined(INFINITY)
#   ಆದರೆ defined(DBL_MAX)
#    ifdef VMS
#     define INFINITY DBL_MAX
#    ಇಲ್ಲದಿದ್ದರೆ
#     define INFINITY (DBL_MAX+DBL_MAX)
#    endif
#   ಇಲ್ಲದಿದ್ದರೆ
#    define INFINITY (1.0 / 0.0)
#   endif
#  endif
#  ಆದರೆ !defined(NAN)
#   define NAN (INFINITY-INFINITY)
#  endif
#  ಆದರೆ !defined(DBL_EPSILON)
#   define DBL_EPSILON 2.2204460492503131e-16
#  endif
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
# define FUNCARG(fp, j)	((char_u **)(fp->uf_ವಾದಗಳು.ga_ದತ್ತಾಂಶ))[j]
#ಆದರೆ_ಅಂತ್ಯ

/*
 * In a hashtab item "hi_key" points to "di_key" in a dictitem.
 * This avoids adding a pointer to the hashtab item.
 * DI2HIKEY() converts a dictitem pointer to a hashitem key pointer.
 * HIKEY2DI() converts a hashitem key pointer to a dictitem pointer.
 * HI2DI() converts a hashitem pointer to a dictitem pointer.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು DI2HIKEY(di) ((di)->di_ಕೀಲಿ)
#ವ್ಯಾಖ್ಯಾನಿಸು HIKEY2DI(p)  ((dictitem_T *)((p) - offsetof(dictitem_T, di_ಕೀಲಿ)))
#ವ್ಯಾಖ್ಯಾನಿಸು HI2DI(hi)     HIKEY2DI((hi)->hi_ಕೀಲಿ)

/*
 * Flush control functions.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
# define mch_enable_flush()	gui_enable_flush()
# define mch_disable_flush()	gui_disable_flush()
#ಇಲ್ಲದಿದ್ದರೆ
# define mch_enable_flush()
# define mch_disable_flush()
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Like vim_free(), and also set the pointer to NULL.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_CLEAR(p) \
    ಮಾಡು { \
	vim_free(p); \
	(p) = ಖಾಲಿ; \
    } ಆಗ (0)

/*
 * Free a string and set it's pointer to NULL and length to 0
 */
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_CLEAR_STRING(s) \
    ಮಾಡು { \
	VIM_CLEAR(s.string); \
	s.ಉದ್ದಳತೆ = 0; \
    } ಆಗ (0)

#ವ್ಯಾಖ್ಯಾನಿಸು STR_LITERAL_INIT(s) \
    {(char_u *)(s), STRLEN_LITERAL(s)}

#ವ್ಯಾಖ್ಯಾನಿಸು STR_LITERAL_ಗಣ(str, s) \
    ಮಾಡು { \
	(str).string = (char_u *)(s); \
	(str).ಉದ್ದಳತೆ = STRLEN_LITERAL(s); \
    } ಆಗ (0)

// Whether a command index indicates a user command.
#ವ್ಯಾಖ್ಯಾನಿಸು IS_USER_CMDIDX(idx) ((ಪೂರ್ಣಾಂಕ)(idx) < 0)

// Give an error in curwin is a popup window and evaluate to TRUE.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
# define WIN_IS_POPUP(wp) ((wp)->w_popup_flags != 0)
# define ERROR_IF_POPUP_ಕಿಟಕಿ error_if_popup_ಕಿಟಕಿ(FALSE)
# define ERROR_IF_ANY_POPUP_ಕಿಟಕಿ error_if_popup_ಕಿಟಕಿ(TRUE)
#ಇಲ್ಲದಿದ್ದರೆ
# define WIN_IS_POPUP(wp) 0
# define ERROR_IF_POPUP_ಕಿಟಕಿ 0
# define ERROR_IF_ANY_POPUP_ಕಿಟಕಿ 0
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
# define ERROR_IF_ಪದ_POPUP_ಕಿಟಕಿ error_if_ಪದ_popup_ಕಿಟಕಿ()
#ಇಲ್ಲದಿದ್ದರೆ
# define ERROR_IF_ಪದ_POPUP_ಕಿಟಕಿ 0
#ಆದರೆ_ಅಂತ್ಯ


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ABORT_ON_ಆಂತರಿಕ_ERROR
# define ESTACK_CHECK_DECLARATION ಪೂರ್ಣಾಂಕ estack_len_before
# define ESTACK_CHECK_ಸಿದ್ಧತೆ ಮಾಡು { estack_len_before = exestack.ga_len; } ಆಗ (0)
# define ESTACK_CHECK_NOW \
    ಮಾಡು { \
	ಆದರೆ (estack_len_before != exestack.ga_len) \
	    siemsg("Exestack length expected: %d, actual: %d", estack_len_before, exestack.ga_len); \
    } ಆಗ (0)
# define CHECK_CURBUF \
    ಮಾಡು { \
	ಆದರೆ (curwin != ಖಾಲಿ && curwin->w_ಬಫರ್ != curbuf) \
	    iemsg("curbuf != curwin->w_buffer"); \
    } ಆಗ (0)
#ಇಲ್ಲದಿದ್ದರೆ
# define ESTACK_CHECK_DECLARATION ಮಾಡು { /**/ } ಆಗ (0)
# define ESTACK_CHECK_ಸಿದ್ಧತೆ ಮಾಡು { /**/ } ಆಗ (0)
# define ESTACK_CHECK_NOW ಮಾಡು { /**/ } ಆಗ (0)
# define CHECK_CURBUF ಮಾಡು { /**/ } ಆಗ (0)
#ಆದರೆ_ಅಂತ್ಯ

// Inline the condition for performance.
#ವ್ಯಾಖ್ಯಾನಿಸು CHECK_LIST_MATERIALIZE(l) \
    ಮಾಡು { \
	ಆದರೆ ((l)->lv_ಮೊದಲ == &range_list_ಅಂಶ) \
	    range_list_materialize(l); \
    } ಆಗ (0)

// Inlined version of ga_grow() with optimized condition that it fails.
#ವ್ಯಾಖ್ಯಾನಿಸು GA_GROW_FAILS(gap, n) unlikely((((gap)->ga_maxlen - (gap)->ga_len < (n)) ? ga_grow_inner((gap), (n)) : OK) == FAIL)
// Inlined version of ga_grow() with optimized condition that it succeeds.
#ವ್ಯಾಖ್ಯಾನಿಸು GA_GROW_OK(gap, n) likely((((gap)->ga_maxlen - (gap)->ga_len < (n)) ? ga_grow_inner((gap), (n)) : OK) == OK)
#ವ್ಯಾಖ್ಯಾನಿಸು GA_CONCAT_LITERAL(gap, s) ga_concat_len((gap), (char_u *)(s), STRLEN_LITERAL(s))

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MIN
# define MIN(a, b) ((a) < (b) ? (a) : (b))
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MAX
# define MAX(a, b) ((a) > (b) ? (a) : (b))
#ಆದರೆ_ಅಂತ್ಯ

// Length of the array.
#ವ್ಯಾಖ್ಯಾನಿಸು ARRAY_ಉದ್ದಳತೆ(a) (ಗಾತ್ರ(a) / ಗಾತ್ರ((a)[0]))

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
# define FOR_ALL_MENUS(m) \
    ಗಾಗಿ ((m) = ಬೇರು_menu; (m) != ಖಾಲಿ; (m) = (m)->ಮುಂದಿನ)
# define FOR_ALL_ಮಗು_MENUS(p, c) \
    ಗಾಗಿ ((c) = (p)->children; (c) != ಖಾಲಿ; (c) = (c)->ಮುಂದಿನ)
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು FOR_ALL_WINDOWS(wp) \
    ಗಾಗಿ ((wp) = firstwin; (wp) != ಖಾಲಿ; (wp) = (wp)->w_ಮುಂದಿನ)
#ವ್ಯಾಖ್ಯಾನಿಸು FOR_ALL_ಚೌಕಟ್ಟುಗಳು(frp, ಮೊದಲ_ಚೌಕಟ್ಟು) \
    ಗಾಗಿ ((frp) = ಮೊದಲ_ಚೌಕಟ್ಟು; (frp) != ಖಾಲಿ; (frp) = (frp)->fr_ಮುಂದಿನ)
#ವ್ಯಾಖ್ಯಾನಿಸು FOR_ALL_TABPAGES(tp) \
    ಗಾಗಿ ((tp) = ಮೊದಲ_tabpage; (tp) != ಖಾಲಿ; (tp) = (tp)->tp_ಮುಂದಿನ)
#ವ್ಯಾಖ್ಯಾನಿಸು FOR_ALL_WINDOWS_ಒಳ_TAB(tp, wp) \
    ಗಾಗಿ ((wp) = ((tp) == ಖಾಲಿ || (tp) == curtab) \
	    ? firstwin : (tp)->tp_firstwin; (wp); (wp) = (wp)->w_ಮುಂದಿನ)
/*
 * When using this macro "break" only breaks out of the inner loop. Use "goto"
 * to break out of the tabpage loop.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು FOR_ALL_TAB_WINDOWS(tp, wp) \
    ಗಾಗಿ ((tp) = ಮೊದಲ_tabpage; (tp) != ಖಾಲಿ; (tp) = (tp)->tp_ಮುಂದಿನ) \
	ಗಾಗಿ ((wp) = ((tp) == curtab) \
		? firstwin : (tp)->tp_firstwin; (wp); (wp) = (wp)->w_ಮುಂದಿನ)

#ವ್ಯಾಖ್ಯಾನಿಸು FOR_ALL_POPUPWINS(wp) \
    ಗಾಗಿ ((wp) = ಮೊದಲ_popupwin; (wp) != ಖಾಲಿ; (wp) = (wp)->w_ಮುಂದಿನ)
#ವ್ಯಾಖ್ಯಾನಿಸು FOR_ALL_POPUPWINS_ಒಳ_TAB(tp, wp) \
    ಗಾಗಿ ((wp) = (tp)->tp_ಮೊದಲ_popupwin; (wp) != ಖಾಲಿ; (wp) = (wp)->w_ಮುಂದಿನ)

#ವ್ಯಾಖ್ಯಾನಿಸು FOR_ALL_BUFFERS(buf) \
    ಗಾಗಿ ((buf) = firstbuf; (buf) != ಖಾಲಿ; (buf) = (buf)->b_ಮುಂದಿನ)

#ವ್ಯಾಖ್ಯಾನಿಸು FOR_ALL_BUF_WININFO(buf, wip) \
    ಗಾಗಿ ((wip) = (buf)->b_wininfo; (wip) != ಖಾಲಿ; (wip) = (wip)->wi_ಮುಂದಿನ)

// Iterate through all the signs placed in a buffer
#ವ್ಯಾಖ್ಯಾನಿಸು FOR_ALL_SIGNS_ಒಳ_BUF(buf, sign) \
    ಗಾಗಿ ((sign) = (buf)->b_signlist; (sign) != ಖಾಲಿ; (sign) = (sign)->se_ಮುಂದಿನ)

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
# define FOR_ALL_SPELL_LANGS(slang) \
    ಗಾಗಿ ((slang) = ಮೊದಲ_lang; (slang) != ಖಾಲಿ; (slang) = (slang)->sl_ಮುಂದಿನ)
#ಆದರೆ_ಅಂತ್ಯ

// Iterate over all the items in a List
#ವ್ಯಾಖ್ಯಾನಿಸು FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li) \
    ಗಾಗಿ ((li) = (l) == ಖಾಲಿ ? ಖಾಲಿ : (l)->lv_ಮೊದಲ; (li) != ಖಾಲಿ; (li) = (li)->li_ಮುಂದಿನ)

// Iterate over all the items in a hash table
#ವ್ಯಾಖ್ಯಾನಿಸು FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(ht, hi, todo) \
    ಗಾಗಿ ((hi) = (ht)->ht_array; (todo) > 0; ++(hi))

#ವ್ಯಾಖ್ಯಾನಿಸು TUPLE_LEN(t)	    (t->tv_ವಸ್ತುಗಳು.ga_len)
#ವ್ಯಾಖ್ಯಾನಿಸು TUPLE_ಅಂಶ(t, i) \
	    (((typval_T *)t->tv_ವಸ್ತುಗಳು.ga_ದತ್ತಾಂಶ) + i)

// Avoid open() on a non-existing file on MS-Windows
#ಆದರೆ !defined(MSWIN) && !defined(PROTO)
# define mch_isdir_or_missing(f) mch_isdir(f)
#ಆದರೆ_ಅಂತ್ಯ
