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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LT_POS(a, b) (((a).lnum != (b).lnum) \
		   ? (a).lnum < (b).lnum \
		   : (a).ਖਾਨਾ != (b).ਖਾਨਾ \
		       ? (a).ਖਾਨਾ < (b).ਖਾਨਾ \
		       : (a).coladd < (b).coladd)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LT_POSP(a, b) (((a)->lnum != (b)->lnum) \
		   ? (a)->lnum < (b)->lnum \
		   : (a)->ਖਾਨਾ != (b)->ਖਾਨਾ \
		       ? (a)->ਖਾਨਾ < (b)->ਖਾਨਾ \
		       : (a)->coladd < (b)->coladd)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EQUAL_POS(a, b) (((a).lnum == (b).lnum) && ((a).ਖਾਨਾ == (b).ਖਾਨਾ) && ((a).coladd == (b).coladd))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLEAR_POS(a) ਕਰੋ {(a)->lnum = 0; (a)->ਖਾਨਾ = 0; (a)->coladd = 0;} ਜਦੋਂ (0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖਾਲੀ_ਥਾਵਾਂ_POS(a) ((a).lnum == 0 && (a).ਖਾਨਾ == 0 && (a).coladd == 0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LTOREQ_POS(a, b) (LT_POS(a, b) || EQUAL_POS(a, b))

/*
 * VIM_ISWHITE() differs from isspace() because it doesn't include <CR> and
 * <LF> and the like.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_ISWHITE(x)		((x) == ' ' || (x) == '\t')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_WHITE_OR_NUL(x)	((x) == ' ' || (x) == '\t' || (x) == NUL)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_WHITE_NL_OR_NUL(x)	((x) == ' ' || (x) == '\t' || (x) == '\n' || (x) == NUL)

/*
 * LINEEMPTY() - return TRUE if the line is empty
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LINEEMPTY(p) (*ml_get(p) == NUL)

/*
 * BUFEMPTY() - return TRUE if the current buffer is empty
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BUFEMPTY() (curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ == 1 && *ml_get((linenr_T)1) == NUL)

// The is*() and to*() functions declared in <ctype.h> have
// undefined behavior for values other than EOF outside the range of
// unsigned char.  If plain char is signed, a call with a negative
// value has undefined behavior.  These macros cast the argument to
// unsigned char.  (Most implementations behave more or less sanely
// with negative values, and most character values in practice are
// positive, but we want to avoid undefined behavior anyway.)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੁਰੱਖਿਅਤ_isalnum(c)  (ਅੱਖਰ_ਜਾਂ_ਅੰਕ ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(c)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੁਰੱਖਿਅਤ_isalpha(c)  (ਅੱਖਰ_ਹੈ ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(c)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੁਰੱਖਿਅਤ_isblank(c)  (isblank ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(c)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੁਰੱਖਿਅਤ_iscntrl(c)  (iscntrl ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(c)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੁਰੱਖਿਅਤ_isdigit(c)  (ਅੰਕ_ਹੈ ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(c)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੁਰੱਖਿਅਤ_isgraph(c)  (isgraph ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(c)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੁਰੱਖਿਅਤ_islower(c)  (islower ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(c)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੁਰੱਖਿਅਤ_isprint(c)  (isprint ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(c)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੁਰੱਖਿਅਤ_ispunct(c)  (ispunct ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(c)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੁਰੱਖਿਅਤ_isspace(c)  (ਖਾਲੀ_ਥਾਂ_ਹੈ ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(c)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੁਰੱਖਿਅਤ_isupper(c)  (isupper ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(c)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੁਰੱਖਿਅਤ_isxdigit(c) (isxdigit((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(c)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੁਰੱਖਿਅਤ_tolower(c)  (tolower ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(c)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੁਰੱਖਿਅਤ_toupper(c)  (ਵੱਡਾ_ਅੱਖਰ ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(c)))

/*
 * toupper() and tolower() that use the current locale.
 * On some systems toupper()/tolower() only work on lower/uppercase
 * characters, first use islower() or isupper() then.
 * Careful: Only call TOUPPER_LOC() and TOLOWER_LOC() with a character in the
 * range 0 - 255.  toupper()/tolower() on some systems can't handle others.
 * Note: It is often better to use MB_TOLOWER() and MB_TOUPPER(), because many
 * toupper() and tolower() implementations only work for ASCII.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
# define TOUPPER_LOC(c)	toupper_tab[(c) & 255]
# define TOLOWER_LOC(c)	tolower_tab[(c) & 255]
#ਨਹੀਂਤਾਂ
# ifdef BROKEN_TOUPPER
#  define TOUPPER_LOC(c)	(ਸੁਰੱਖਿਅਤ_islower(c) ? ਸੁਰੱਖਿਅਤ_toupper(c) : (c))
#  define TOLOWER_LOC(c)	(ਸੁਰੱਖਿਅਤ_isupper(c) ? ਸੁਰੱਖਿਅਤ_tolower(c) : (c))
# ਨਹੀਂਤਾਂ
#  define TOUPPER_LOC		ਸੁਰੱਖਿਅਤ_toupper
#  define TOLOWER_LOC		ਸੁਰੱਖਿਅਤ_tolower
# endif
#ਜੇ_ਅੰਤ

// toupper() and tolower() for ASCII only and ignore the current locale.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TOUPPER_ASC(c)	(((c) < 'a' || (c) > 'z') ? (c) : (c) - ('a' - 'A'))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TOLOWER_ASC(c)	(((c) < 'A' || (c) > 'Z') ? (c) : (c) + ('a' - 'A'))

/*
 * MB_ISLOWER() and MB_ISUPPER() are to be used on multi-byte characters.  But
 * don't use them for negative values!
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MB_ISLOWER(c)	vim_islower(c)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MB_ISUPPER(c)	vim_isupper(c)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MB_TOLOWER(c)	vim_tolower(c)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MB_TOUPPER(c)	vim_toupper(c)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MB_CASEFOLD(c)	(enc_utf8 ? utf_fold(c) : MB_TOLOWER(c))

// Use our own isdigit() replacement, because on MS-Windows isdigit() returns
// non-zero for superscript 1.  Also avoids that isdigit() crashes for numbers
// below 0 and above 255.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_ISDIGIT(c) ((ਬਿਨਾਨਿਸ਼ਾਨੀ)(c) - '0' < 10)

// Like isalpha() but reject non-ASCII characters.  Can't be used with a
// special key (negative value).
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ASCII_ISLOWER(c) ((ਬਿਨਾਨਿਸ਼ਾਨੀ)(c) - 'a' < 26)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ASCII_ISUPPER(c) ((ਬਿਨਾਨਿਸ਼ਾਨੀ)(c) - 'A' < 26)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ASCII_ISALPHA(c) (ASCII_ISUPPER(c) || ASCII_ISLOWER(c))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ASCII_ISALNUM(c) (ASCII_ISALPHA(c) || VIM_ISDIGIT(c))

// Returns empty string if it is NULL.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖਾਲੀ_ਥਾਵਾਂ_IF_NULL(x) ((x) ? (x) : (char_u *)"")

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LANGMAP
/*
 * Adjust chars in a language according to 'langmap' option.
 * NOTE that there is no noticeable overhead if 'langmap' is not set.
 * When set the overhead for characters < 256 is small.
 * Don't apply 'langmap' if the character comes from the Stuff buffer or from
 * a mapping and the langnoremap option was set.
 * The do-while is just to ignore a ';' after the macro.
 */
# define LANGMAP_ADJUST(c, condition) \
    ਕਰੋ { \
	ਜੇ (*p_langmap \
		&& (condition) \
		&& (p_lrm || (!p_lrm && ਕੁੰਜੀ_Typed)) \
		&& !ਕੁੰਜੀ_Stuffed \
		&& (c) >= 0) \
	{ \
	    ਜੇ ((c) < 256) \
		c = langmap_mapchar[c]; \
	    ਨਹੀਂਤਾਂ \
		c = langmap_adjust_mb(c); \
	} \
    } ਜਦੋਂ (0)
#ਨਹੀਂਤਾਂ
# define LANGMAP_ADJUST(c, condition) // nop
#ਜੇ_ਅੰਤ

/*
 * VIM_ISBREAK() is used very often if 'linebreak' is set, use a macro to make
 * it work fast.  Only works for single byte characters!
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_ISBREAK(c) ((c) < 256 && breakat_flags[(char_u)(c)])

/*
 * On VMS file names are different and require a translation.
 * On the Mac open() has only two arguments.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
# define mch_access(n, p)	access(vms_fixfilename(n), (p))
				// see mch_open() comment
# define mch_fopen(n, p)	fopen(vms_fixfilename(n), (p))
# define mch_fstat(n, p)	fstat((n), (p))
# undef HAVE_LSTAT		// VMS does not have lstat()
# define mch_stat(n, p)		stat(vms_fixfilename(n), (p))
#ਨਹੀਂਤਾਂ
# ਜੇ !defined(MSWIN) && !defined(PROTO)
#  define mch_access(n, p)	access((n), (p))
# endif

// Use 64-bit fstat function on MS-Windows.
// NOTE: This condition is the same as for the stat_T type.
# ifdef MSWIN
#  define mch_fstat(n, p)	_fstat64((n), (p))
# ਨਹੀਂਤਾਂ
#  define mch_fstat(n, p)	fstat((n), (p))
# endif

# ifdef MSWIN	// has its own mch_stat() function
#  define mch_stat(n, p)	vim_stat((n), (p))
# ਨਹੀਂਤਾਂ
#  ifdef STAT_IGNORES_SLASH
#   define mch_stat(n, p)	vim_stat((n), (p))
#  ਨਹੀਂਤਾਂ
#   define mch_stat(n, p)	stat((n), (p))
#  endif
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_LSTAT
# define mch_lstat(n, p)	lstat((n), (p))
#ਨਹੀਂਤਾਂ
# ifdef MSWIN
#  define mch_lstat(n, p)	vim_lstat((n), (p))
# ਨਹੀਂਤਾਂ
#  define mch_lstat(n, p)	mch_stat((n), (p))
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
/*
 * It is possible to force some record format with:
 * #  define mch_open(n, m, p) open(vms_fixfilename(n), (m), (p)), "rat=cr", "rfm=stmlf", "mrs=0")
 * but it is not recommended, because it can destroy indexes etc.
 */
# define mch_open(n, m, p)	open(vms_fixfilename(n), (m), (p))
#ਜੇ_ਅੰਤ

// mch_open_rw(): invoke mch_open() with third argument for user R/W.
#ਜੇ defined(UNIX) || defined(VMS)  // open in rw------- mode
# define mch_open_rw(n, f)	mch_open((n), (f), (mode_t)0600)
#ਨਹੀਂਤਾਂ
# ਜੇ defined(MSWIN)  // open read/write
#  define mch_open_rw(n, f)	mch_open((n), (f), S_IREAD | S_IWRITE)
# ਨਹੀਂਤਾਂ
#  define mch_open_rw(n, f)	mch_open((n), (f), 0)
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ STARTUPTIME
# define ਸਮਾਂ_ਸੁਨੇਹਾ(s) ਕਰੋ { ਜੇ (ਸਮਾਂ_fd != ਨਿਰਾ) ਸਮਾਂ_ਸੁਨੇਹਾ_2(s, ਨਿਰਾ); } ਜਦੋਂ (0)
#ਨਹੀਂਤਾਂ
# define ਸਮਾਂ_ਸੁਨੇਹਾ(s) ਕਰੋ { /**/ } ਜਦੋਂ (0)
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REPLACE_ਸਾਧਾਰਨ(s) (((s) & REPLACE_FLAG) && !((s) & VREPLACE_FLAG))

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARABIC
# define ARABIC_CHAR(ch)	    (((ch) & 0xFF00) == 0x0600)
# define UTF_COMPOSINGLIKE(p1, p2)  utf_composinglike((p1), (p2))
#ਨਹੀਂਤਾਂ
# define UTF_COMPOSINGLIKE(p1, p2)  utf_iscomposing(utf_ptr2char(p2))
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    // Whether to draw the vertical bar on the right side of the cell.
# define CURSOR_BAR_ਸੱਜਾ (curwin->w_p_rl && (!(State & ਬਹੁਲਕ_CMDLINE) || cmdmsg_rl))
#ਜੇ_ਅੰਤ

/*
 * MB_PTR_ADV(): advance a pointer to the next character, taking care of
 * multi-byte characters if needed.
 * MB_PTR_BACK(): backup a pointer to the previous character, taking care of
 * multi-byte characters if needed.
 * MB_COPY_CHAR(f, t): copy one char from "f" to "t" and advance the pointers.
 * PTR2CHAR(): get character from pointer.
 */
// Advance multi-byte pointer, skip over composing chars.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MB_PTR_ADV(p)	    p += (*mb_ptr2len)(p)
// Advance multi-byte pointer, do not skip over composing chars.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MB_CPTR_ADV(p)	    p += enc_utf8 ? utf_ptr2len(p) : (*mb_ptr2len)(p)
// Backup multi-byte pointer. Only use with "p" > "s" !
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MB_PTR_ਪਿੱਠ(s, p)  p -= has_mbyte ? ((*mb_ਸਿਰ_off)(s, (p) - 1) + 1) : 1
// get length of multi-byte char, not including composing chars
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MB_CPTR2LEN(p)	    (enc_utf8 ? utf_ptr2len(p) : (*mb_ptr2len)(p))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MB_COPY_CHAR(f, t) ਕਰੋ { ਜੇ (has_mbyte) mb_copy_char(&(f), &(t)); ਨਹੀਂਤਾਂ *(t)++ = *(f)++; } ਜਦੋਂ (0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MB_CHARLEN(p)	    (has_mbyte ? mb_charlen(p) : (ਪੂਰਨ)STRLEN(p))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MB_CHAR2LEN(c)	    (has_mbyte ? mb_char2len(c) : 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTR2CHAR(p)	    (has_mbyte ? mb_ptr2char(p) : (ਪੂਰਨ)*(p))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MB_CHAR2_ਬਾਈਟਾਂ(c, b) ਕਰੋ { ਜੇ (has_mbyte) (b) += (*mb_char2bytes)((c), (b)); ਨਹੀਂਤਾਂ *(b)++ = (c); } ਜਦੋਂ (0)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_AUTOCHDIR
# define DO_AUTOCHDIR ਕਰੋ { ਜੇ (p_acd) do_autochdir(); } ਜਦੋਂ (0)
#ਨਹੀਂਤਾਂ
# define DO_AUTOCHDIR ਕਰੋ { /**/ } ਜਦੋਂ (0)
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RESET_BINDING(wp)  ਕਰੋ { (wp)->w_p_scb = FALSE; (wp)->w_p_crb = FALSE; \
			    } ਜਦੋਂ (0)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
# define PLINES_NOFILL(x) plines_nofill(x)
# define PLINES_WIN_NOFILL(w, l, h) plines_win_nofill((w), (l), (h))
#ਨਹੀਂਤਾਂ
# define PLINES_NOFILL(x) plines(x)
# define PLINES_WIN_NOFILL(w, l, h) plines_win((w), (l), (h))
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_ਕੰਮ_CHANNEL) || defined(FEAT_CLIENTSERVER)
# define ਸੁਨੇਹਾ_ਕਤਾਰ
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <float.h>
#ਜੇ defined(HAVE_MATH_H)
  // for isnan() and isinf()
# include <math.h>
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL)
# ifdef USING_FLOAT_STUFF
#  ifdef MSWIN
#   ifndef isnan
#    define isnan(x) _isnan(x)
     ਸਥਿਰ __inline ਪੂਰਨ isinf(ਦੁੱਗਣਾ x)
	{ ਵਾਪਸ !_finite(x) && !_isnan(x); }
#   endif
#  ਨਹੀਂਤਾਂ
#   ifndef HAVE_ISNAN
     ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ isnan(ਦੁੱਗਣਾ x)
	{ ਵਾਪਸ x != x; }
#   endif
#   ifndef HAVE_ISINF
     ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ isinf(ਦੁੱਗਣਾ x)
	{ ਵਾਪਸ !isnan(x) && isnan(x - x); }
#   endif
#  endif
#  ਜੇ !defined(INFINITY)
#   ਜੇ defined(DBL_MAX)
#    ifdef VMS
#     define INFINITY DBL_MAX
#    ਨਹੀਂਤਾਂ
#     define INFINITY (DBL_MAX+DBL_MAX)
#    endif
#   ਨਹੀਂਤਾਂ
#    define INFINITY (1.0 / 0.0)
#   endif
#  endif
#  ਜੇ !defined(NAN)
#   define NAN (INFINITY-INFINITY)
#  endif
#  ਜੇ !defined(DBL_EPSILON)
#   define DBL_EPSILON 2.2204460492503131e-16
#  endif
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
# define FUNCARG(fp, j)	((char_u **)(fp->uf_ਦਲੀਲਾਂ.ga_ਅੰਕੜੇ))[j]
#ਜੇ_ਅੰਤ

/*
 * In a hashtab item "hi_key" points to "di_key" in a dictitem.
 * This avoids adding a pointer to the hashtab item.
 * DI2HIKEY() converts a dictitem pointer to a hashitem key pointer.
 * HIKEY2DI() converts a hashitem key pointer to a dictitem pointer.
 * HI2DI() converts a hashitem pointer to a dictitem pointer.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DI2HIKEY(di) ((di)->di_ਕੁੰਜੀ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HIKEY2DI(p)  ((dictitem_T *)((p) - offsetof(dictitem_T, di_ਕੁੰਜੀ)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HI2DI(hi)     HIKEY2DI((hi)->hi_ਕੁੰਜੀ)

/*
 * Flush control functions.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
# define mch_enable_flush()	gui_enable_flush()
# define mch_disable_flush()	gui_disable_flush()
#ਨਹੀਂਤਾਂ
# define mch_enable_flush()
# define mch_disable_flush()
#ਜੇ_ਅੰਤ

/*
 * Like vim_free(), and also set the pointer to NULL.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_CLEAR(p) \
    ਕਰੋ { \
	vim_free(p); \
	(p) = ਨਿਰਾ; \
    } ਜਦੋਂ (0)

/*
 * Free a string and set it's pointer to NULL and length to 0
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_CLEAR_STRING(s) \
    ਕਰੋ { \
	VIM_CLEAR(s.string); \
	s.ਲੰਮਾਈ = 0; \
    } ਜਦੋਂ (0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STR_LITERAL_INIT(s) \
    {(char_u *)(s), STRLEN_LITERAL(s)}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STR_LITERAL_ਸੈੱਟ(str, s) \
    ਕਰੋ { \
	(str).string = (char_u *)(s); \
	(str).ਲੰਮਾਈ = STRLEN_LITERAL(s); \
    } ਜਦੋਂ (0)

// Whether a command index indicates a user command.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_USER_CMDIDX(idx) ((ਪੂਰਨ)(idx) < 0)

// Give an error in curwin is a popup window and evaluate to TRUE.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
# define WIN_IS_POPUP(wp) ((wp)->w_popup_flags != 0)
# define ERROR_IF_POPUP_ਖਿੜਕੀ error_if_popup_ਖਿੜਕੀ(FALSE)
# define ERROR_IF_ANY_POPUP_ਖਿੜਕੀ error_if_popup_ਖਿੜਕੀ(TRUE)
#ਨਹੀਂਤਾਂ
# define WIN_IS_POPUP(wp) 0
# define ERROR_IF_POPUP_ਖਿੜਕੀ 0
# define ERROR_IF_ANY_POPUP_ਖਿੜਕੀ 0
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
# define ERROR_IF_ਪਦ_POPUP_ਖਿੜਕੀ error_if_ਪਦ_popup_ਖਿੜਕੀ()
#ਨਹੀਂਤਾਂ
# define ERROR_IF_ਪਦ_POPUP_ਖਿੜਕੀ 0
#ਜੇ_ਅੰਤ


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ABORT_ON_ਅੰਦਰੂਨੀ_ERROR
# define ESTACK_CHECK_DECLARATION ਪੂਰਨ estack_len_before
# define ESTACK_CHECK_ਤਿਆਰੀ ਕਰੋ { estack_len_before = exestack.ga_len; } ਜਦੋਂ (0)
# define ESTACK_CHECK_NOW \
    ਕਰੋ { \
	ਜੇ (estack_len_before != exestack.ga_len) \
	    siemsg("Exestack length expected: %d, actual: %d", estack_len_before, exestack.ga_len); \
    } ਜਦੋਂ (0)
# define CHECK_CURBUF \
    ਕਰੋ { \
	ਜੇ (curwin != ਨਿਰਾ && curwin->w_ਬਫਰ != curbuf) \
	    iemsg("curbuf != curwin->w_buffer"); \
    } ਜਦੋਂ (0)
#ਨਹੀਂਤਾਂ
# define ESTACK_CHECK_DECLARATION ਕਰੋ { /**/ } ਜਦੋਂ (0)
# define ESTACK_CHECK_ਤਿਆਰੀ ਕਰੋ { /**/ } ਜਦੋਂ (0)
# define ESTACK_CHECK_NOW ਕਰੋ { /**/ } ਜਦੋਂ (0)
# define CHECK_CURBUF ਕਰੋ { /**/ } ਜਦੋਂ (0)
#ਜੇ_ਅੰਤ

// Inline the condition for performance.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CHECK_LIST_MATERIALIZE(l) \
    ਕਰੋ { \
	ਜੇ ((l)->lv_ਪਹਿਲਾ == &range_list_ਵਸਤੂ) \
	    range_list_materialize(l); \
    } ਜਦੋਂ (0)

// Inlined version of ga_grow() with optimized condition that it fails.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GA_GROW_FAILS(gap, n) unlikely((((gap)->ga_maxlen - (gap)->ga_len < (n)) ? ga_grow_inner((gap), (n)) : OK) == FAIL)
// Inlined version of ga_grow() with optimized condition that it succeeds.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GA_GROW_OK(gap, n) likely((((gap)->ga_maxlen - (gap)->ga_len < (n)) ? ga_grow_inner((gap), (n)) : OK) == OK)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GA_CONCAT_LITERAL(gap, s) ga_concat_len((gap), (char_u *)(s), STRLEN_LITERAL(s))

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MIN
# define MIN(a, b) ((a) < (b) ? (a) : (b))
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MAX
# define MAX(a, b) ((a) > (b) ? (a) : (b))
#ਜੇ_ਅੰਤ

// Length of the array.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ARRAY_ਲੰਮਾਈ(a) (ਆਕਾਰ(a) / ਆਕਾਰ((a)[0]))

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
# define FOR_ALL_MENUS(m) \
    ਲਈ ((m) = ਜੜ੍ਹ_menu; (m) != ਨਿਰਾ; (m) = (m)->ਅਗਲਾ)
# define FOR_ALL_ਬੱਚਾ_MENUS(p, c) \
    ਲਈ ((c) = (p)->children; (c) != ਨਿਰਾ; (c) = (c)->ਅਗਲਾ)
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOR_ALL_WINDOWS(wp) \
    ਲਈ ((wp) = firstwin; (wp) != ਨਿਰਾ; (wp) = (wp)->w_ਅਗਲਾ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOR_ALL_ਚੌਖਟੇ(frp, ਪਹਿਲਾ_ਚੌਖਟਾ) \
    ਲਈ ((frp) = ਪਹਿਲਾ_ਚੌਖਟਾ; (frp) != ਨਿਰਾ; (frp) = (frp)->fr_ਅਗਲਾ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOR_ALL_TABPAGES(tp) \
    ਲਈ ((tp) = ਪਹਿਲਾ_tabpage; (tp) != ਨਿਰਾ; (tp) = (tp)->tp_ਅਗਲਾ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOR_ALL_WINDOWS_ਵਿੱਚ_TAB(tp, wp) \
    ਲਈ ((wp) = ((tp) == ਨਿਰਾ || (tp) == curtab) \
	    ? firstwin : (tp)->tp_firstwin; (wp); (wp) = (wp)->w_ਅਗਲਾ)
/*
 * When using this macro "break" only breaks out of the inner loop. Use "goto"
 * to break out of the tabpage loop.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOR_ALL_TAB_WINDOWS(tp, wp) \
    ਲਈ ((tp) = ਪਹਿਲਾ_tabpage; (tp) != ਨਿਰਾ; (tp) = (tp)->tp_ਅਗਲਾ) \
	ਲਈ ((wp) = ((tp) == curtab) \
		? firstwin : (tp)->tp_firstwin; (wp); (wp) = (wp)->w_ਅਗਲਾ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOR_ALL_POPUPWINS(wp) \
    ਲਈ ((wp) = ਪਹਿਲਾ_popupwin; (wp) != ਨਿਰਾ; (wp) = (wp)->w_ਅਗਲਾ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOR_ALL_POPUPWINS_ਵਿੱਚ_TAB(tp, wp) \
    ਲਈ ((wp) = (tp)->tp_ਪਹਿਲਾ_popupwin; (wp) != ਨਿਰਾ; (wp) = (wp)->w_ਅਗਲਾ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOR_ALL_BUFFERS(buf) \
    ਲਈ ((buf) = firstbuf; (buf) != ਨਿਰਾ; (buf) = (buf)->b_ਅਗਲਾ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOR_ALL_BUF_WININFO(buf, wip) \
    ਲਈ ((wip) = (buf)->b_wininfo; (wip) != ਨਿਰਾ; (wip) = (wip)->wi_ਅਗਲਾ)

// Iterate through all the signs placed in a buffer
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOR_ALL_SIGNS_ਵਿੱਚ_BUF(buf, sign) \
    ਲਈ ((sign) = (buf)->b_signlist; (sign) != ਨਿਰਾ; (sign) = (sign)->se_ਅਗਲਾ)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
# define FOR_ALL_SPELL_LANGS(slang) \
    ਲਈ ((slang) = ਪਹਿਲਾ_lang; (slang) != ਨਿਰਾ; (slang) = (slang)->sl_ਅਗਲਾ)
#ਜੇ_ਅੰਤ

// Iterate over all the items in a List
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOR_ALL_LIST_ਚੀਜ਼ਾਂ(l, li) \
    ਲਈ ((li) = (l) == ਨਿਰਾ ? ਨਿਰਾ : (l)->lv_ਪਹਿਲਾ; (li) != ਨਿਰਾ; (li) = (li)->li_ਅਗਲਾ)

// Iterate over all the items in a hash table
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(ht, hi, todo) \
    ਲਈ ((hi) = (ht)->ht_array; (todo) > 0; ++(hi))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TUPLE_LEN(t)	    (t->tv_ਚੀਜ਼ਾਂ.ga_len)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TUPLE_ਵਸਤੂ(t, i) \
	    (((typval_T *)t->tv_ਚੀਜ਼ਾਂ.ga_ਅੰਕੜੇ) + i)

// Avoid open() on a non-existing file on MS-Windows
#ਜੇ !defined(MSWIN) && !defined(PROTO)
# define mch_isdir_or_missing(f) mch_isdir(f)
#ਜੇ_ਅੰਤ
