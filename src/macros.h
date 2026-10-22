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
#تعریف_کړه LT_POS(a, b) (((a).lnum != (b).lnum) \
		   ? (a).lnum < (b).lnum \
		   : (a).ستون != (b).ستون \
		       ? (a).ستون < (b).ستون \
		       : (a).coladd < (b).coladd)
#تعریف_کړه LT_POSP(a, b) (((a)->lnum != (b)->lnum) \
		   ? (a)->lnum < (b)->lnum \
		   : (a)->ستون != (b)->ستون \
		       ? (a)->ستون < (b)->ستون \
		       : (a)->coladd < (b)->coladd)
#تعریف_کړه EQUAL_POS(a, b) (((a).lnum == (b).lnum) && ((a).ستون == (b).ستون) && ((a).coladd == (b).coladd))
#تعریف_کړه CLEAR_POS(a) افعل {(a)->lnum = 0; (a)->ستون = 0; (a)->coladd = 0;} طالما (0)
#تعریف_کړه تشه_POS(a) ((a).lnum == 0 && (a).ستون == 0 && (a).coladd == 0)

#تعریف_کړه LTOREQ_POS(a, b) (LT_POS(a, b) || EQUAL_POS(a, b))

/*
 * VIM_ISWHITE() differs from isspace() because it doesn't include <CR> and
 * <LF> and the like.
 */
#تعریف_کړه VIM_ISWHITE(x)		((x) == ' ' || (x) == '\t')
#تعریف_کړه IS_WHITE_OR_NUL(x)	((x) == ' ' || (x) == '\t' || (x) == NUL)
#تعریف_کړه IS_WHITE_NL_OR_NUL(x)	((x) == ' ' || (x) == '\t' || (x) == '\n' || (x) == NUL)

/*
 * LINEEMPTY() - return TRUE if the line is empty
 */
#تعریف_کړه LINEEMPTY(p) (*ml_get(p) == NUL)

/*
 * BUFEMPTY() - return TRUE if the current buffer is empty
 */
#تعریف_کړه BUFEMPTY() (curbuf->b_ml.ml_کرښه_شمېر == 1 && *ml_get((linenr_T)1) == NUL)

// The is*() and to*() functions declared in <ctype.h> have
// undefined behavior for values other than EOF outside the range of
// unsigned char.  If plain char is signed, a call with a negative
// value has undefined behavior.  These macros cast the argument to
// unsigned char.  (Most implementations behave more or less sanely
// with negative values, and most character values in practice are
// positive, but we want to avoid undefined behavior anyway.)
#تعریف_کړه خوندي_isalnum(c)  (که_توری_یا_عدد ((بینښه حرف)(c)))
#تعریف_کړه خوندي_isalpha(c)  (که_توری ((بینښه حرف)(c)))
#تعریف_کړه خوندي_isblank(c)  (isblank ((بینښه حرف)(c)))
#تعریف_کړه خوندي_iscntrl(c)  (iscntrl ((بینښه حرف)(c)))
#تعریف_کړه خوندي_isdigit(c)  (که_عدد ((بینښه حرف)(c)))
#تعریف_کړه خوندي_isgraph(c)  (isgraph ((بینښه حرف)(c)))
#تعریف_کړه خوندي_islower(c)  (islower ((بینښه حرف)(c)))
#تعریف_کړه خوندي_isprint(c)  (isprint ((بینښه حرف)(c)))
#تعریف_کړه خوندي_ispunct(c)  (ispunct ((بینښه حرف)(c)))
#تعریف_کړه خوندي_isspace(c)  (که_تشه ((بینښه حرف)(c)))
#تعریف_کړه خوندي_isupper(c)  (isupper ((بینښه حرف)(c)))
#تعریف_کړه خوندي_isxdigit(c) (isxdigit((بینښه حرف)(c)))
#تعریف_کړه خوندي_tolower(c)  (tolower ((بینښه حرف)(c)))
#تعریف_کړه خوندي_toupper(c)  (لوی_توری ((بینښه حرف)(c)))

/*
 * toupper() and tolower() that use the current locale.
 * On some systems toupper()/tolower() only work on lower/uppercase
 * characters, first use islower() or isupper() then.
 * Careful: Only call TOUPPER_LOC() and TOLOWER_LOC() with a character in the
 * range 0 - 255.  toupper()/tolower() on some systems can't handle others.
 * Note: It is often better to use MB_TOLOWER() and MB_TOUPPER(), because many
 * toupper() and tolower() implementations only work for ASCII.
 */
#که_تعریف_شوی MSWIN
# define TOUPPER_LOC(c)	toupper_tab[(c) & 255]
# define TOLOWER_LOC(c)	tolower_tab[(c) & 255]
#وإلا
# ifdef BROKEN_TOUPPER
#  define TOUPPER_LOC(c)	(خوندي_islower(c) ? خوندي_toupper(c) : (c))
#  define TOLOWER_LOC(c)	(خوندي_isupper(c) ? خوندي_tolower(c) : (c))
# وإلا
#  define TOUPPER_LOC		خوندي_toupper
#  define TOLOWER_LOC		خوندي_tolower
# endif
#د_که_پای

// toupper() and tolower() for ASCII only and ignore the current locale.
#تعریف_کړه TOUPPER_ASC(c)	(((c) < 'a' || (c) > 'z') ? (c) : (c) - ('a' - 'A'))
#تعریف_کړه TOLOWER_ASC(c)	(((c) < 'A' || (c) > 'Z') ? (c) : (c) + ('a' - 'A'))

/*
 * MB_ISLOWER() and MB_ISUPPER() are to be used on multi-byte characters.  But
 * don't use them for negative values!
 */
#تعریف_کړه MB_ISLOWER(c)	vim_islower(c)
#تعریف_کړه MB_ISUPPER(c)	vim_isupper(c)
#تعریف_کړه MB_TOLOWER(c)	vim_tolower(c)
#تعریف_کړه MB_TOUPPER(c)	vim_toupper(c)
#تعریف_کړه MB_CASEFOLD(c)	(enc_utf8 ? utf_fold(c) : MB_TOLOWER(c))

// Use our own isdigit() replacement, because on MS-Windows isdigit() returns
// non-zero for superscript 1.  Also avoids that isdigit() crashes for numbers
// below 0 and above 255.
#تعریف_کړه VIM_ISDIGIT(c) ((بینښه)(c) - '0' < 10)

// Like isalpha() but reject non-ASCII characters.  Can't be used with a
// special key (negative value).
#تعریف_کړه ASCII_ISLOWER(c) ((بینښه)(c) - 'a' < 26)
#تعریف_کړه ASCII_ISUPPER(c) ((بینښه)(c) - 'A' < 26)
#تعریف_کړه ASCII_ISALPHA(c) (ASCII_ISUPPER(c) || ASCII_ISLOWER(c))
#تعریف_کړه ASCII_ISALNUM(c) (ASCII_ISALPHA(c) || VIM_ISDIGIT(c))

// Returns empty string if it is NULL.
#تعریف_کړه تشه_IF_NULL(x) ((x) ? (x) : (char_u *)"")

#که_تعریف_شوی FEAT_LANGMAP
/*
 * Adjust chars in a language according to 'langmap' option.
 * NOTE that there is no noticeable overhead if 'langmap' is not set.
 * When set the overhead for characters < 256 is small.
 * Don't apply 'langmap' if the character comes from the Stuff buffer or from
 * a mapping and the langnoremap option was set.
 * The do-while is just to ignore a ';' after the macro.
 */
# define LANGMAP_ADJUST(c, condition) \
    افعل { \
	إذا (*p_langmap \
		&& (condition) \
		&& (p_lrm || (!p_lrm && کلی_Typed)) \
		&& !کلی_Stuffed \
		&& (c) >= 0) \
	{ \
	    إذا ((c) < 256) \
		c = langmap_mapchar[c]; \
	    وإلا \
		c = langmap_adjust_mb(c); \
	} \
    } طالما (0)
#وإلا
# define LANGMAP_ADJUST(c, condition) // nop
#د_که_پای

/*
 * VIM_ISBREAK() is used very often if 'linebreak' is set, use a macro to make
 * it work fast.  Only works for single byte characters!
 */
#تعریف_کړه VIM_ISBREAK(c) ((c) < 256 && breakat_flags[(char_u)(c)])

/*
 * On VMS file names are different and require a translation.
 * On the Mac open() has only two arguments.
 */
#که_تعریف_شوی VMS
# define mch_access(n, p)	access(vms_fixfilename(n), (p))
				// see mch_open() comment
# define mch_fopen(n, p)	fopen(vms_fixfilename(n), (p))
# define mch_fstat(n, p)	fstat((n), (p))
# undef HAVE_LSTAT		// VMS does not have lstat()
# define mch_stat(n, p)		stat(vms_fixfilename(n), (p))
#وإلا
# إذا !defined(MSWIN) && !defined(PROTO)
#  define mch_access(n, p)	access((n), (p))
# endif

// Use 64-bit fstat function on MS-Windows.
// NOTE: This condition is the same as for the stat_T type.
# ifdef MSWIN
#  define mch_fstat(n, p)	_fstat64((n), (p))
# وإلا
#  define mch_fstat(n, p)	fstat((n), (p))
# endif

# ifdef MSWIN	// has its own mch_stat() function
#  define mch_stat(n, p)	vim_stat((n), (p))
# وإلا
#  ifdef STAT_IGNORES_SLASH
#   define mch_stat(n, p)	vim_stat((n), (p))
#  وإلا
#   define mch_stat(n, p)	stat((n), (p))
#  endif
# endif
#د_که_پای

#که_تعریف_شوی HAVE_LSTAT
# define mch_lstat(n, p)	lstat((n), (p))
#وإلا
# ifdef MSWIN
#  define mch_lstat(n, p)	vim_lstat((n), (p))
# وإلا
#  define mch_lstat(n, p)	mch_stat((n), (p))
# endif
#د_که_پای

#که_تعریف_شوی VMS
/*
 * It is possible to force some record format with:
 * #  define mch_open(n, m, p) open(vms_fixfilename(n), (m), (p)), "rat=cr", "rfm=stmlf", "mrs=0")
 * but it is not recommended, because it can destroy indexes etc.
 */
# define mch_open(n, m, p)	open(vms_fixfilename(n), (m), (p))
#د_که_پای

// mch_open_rw(): invoke mch_open() with third argument for user R/W.
#إذا defined(UNIX) || defined(VMS)  // open in rw------- mode
# define mch_open_rw(n, f)	mch_open((n), (f), (mode_t)0600)
#وإلا
# إذا defined(MSWIN)  // open read/write
#  define mch_open_rw(n, f)	mch_open((n), (f), S_IREAD | S_IWRITE)
# وإلا
#  define mch_open_rw(n, f)	mch_open((n), (f), 0)
# endif
#د_که_پای

#که_تعریف_شوی STARTUPTIME
# define وخت_پیغام(s) افعل { إذا (وخت_fd != عدم) وخت_پیغام_2(s, عدم); } طالما (0)
#وإلا
# define وخت_پیغام(s) افعل { /**/ } طالما (0)
#د_که_پای

#تعریف_کړه REPLACE_نورمال(s) (((s) & REPLACE_FLAG) && !((s) & VREPLACE_FLAG))

#که_تعریف_شوی FEAT_ARABIC
# define ARABIC_CHAR(ch)	    (((ch) & 0xFF00) == 0x0600)
# define UTF_COMPOSINGLIKE(p1, p2)  utf_composinglike((p1), (p2))
#وإلا
# define UTF_COMPOSINGLIKE(p1, p2)  utf_iscomposing(utf_ptr2char(p2))
#د_که_پای

#که_تعریف_شوی FEAT_RIGHTLEFT
    // Whether to draw the vertical bar on the right side of the cell.
# define CURSOR_BAR_ښی (curwin->w_p_rl && (!(State & ډېرپېښ_CMDLINE) || cmdmsg_rl))
#د_که_پای

/*
 * MB_PTR_ADV(): advance a pointer to the next character, taking care of
 * multi-byte characters if needed.
 * MB_PTR_BACK(): backup a pointer to the previous character, taking care of
 * multi-byte characters if needed.
 * MB_COPY_CHAR(f, t): copy one char from "f" to "t" and advance the pointers.
 * PTR2CHAR(): get character from pointer.
 */
// Advance multi-byte pointer, skip over composing chars.
#تعریف_کړه MB_PTR_ADV(p)	    p += (*mb_ptr2len)(p)
// Advance multi-byte pointer, do not skip over composing chars.
#تعریف_کړه MB_CPTR_ADV(p)	    p += enc_utf8 ? utf_ptr2len(p) : (*mb_ptr2len)(p)
// Backup multi-byte pointer. Only use with "p" > "s" !
#تعریف_کړه MB_PTR_شا(s, p)  p -= has_mbyte ? ((*mb_سر_off)(s, (p) - 1) + 1) : 1
// get length of multi-byte char, not including composing chars
#تعریف_کړه MB_CPTR2LEN(p)	    (enc_utf8 ? utf_ptr2len(p) : (*mb_ptr2len)(p))

#تعریف_کړه MB_COPY_CHAR(f, t) افعل { إذا (has_mbyte) mb_copy_char(&(f), &(t)); وإلا *(t)++ = *(f)++; } طالما (0)
#تعریف_کړه MB_CHARLEN(p)	    (has_mbyte ? mb_charlen(p) : (صحيح)STRLEN(p))
#تعریف_کړه MB_CHAR2LEN(c)	    (has_mbyte ? mb_char2len(c) : 1)
#تعریف_کړه PTR2CHAR(p)	    (has_mbyte ? mb_ptr2char(p) : (صحيح)*(p))
#تعریف_کړه MB_CHAR2_بایټونه(c, b) افعل { إذا (has_mbyte) (b) += (*mb_char2bytes)((c), (b)); وإلا *(b)++ = (c); } طالما (0)

#که_تعریف_شوی FEAT_AUTOCHDIR
# define DO_AUTOCHDIR افعل { إذا (p_acd) do_autochdir(); } طالما (0)
#وإلا
# define DO_AUTOCHDIR افعل { /**/ } طالما (0)
#د_که_پای

#تعریف_کړه RESET_BINDING(wp)  افعل { (wp)->w_p_scb = FALSE; (wp)->w_p_crb = FALSE; \
			    } طالما (0)

#که_تعریف_شوی FEAT_DIFF
# define PLINES_NOFILL(x) plines_nofill(x)
# define PLINES_WIN_NOFILL(w, l, h) plines_win_nofill((w), (l), (h))
#وإلا
# define PLINES_NOFILL(x) plines(x)
# define PLINES_WIN_NOFILL(w, l, h) plines_win((w), (l), (h))
#د_که_پای

#إذا defined(FEAT_دنده_CHANNEL) || defined(FEAT_CLIENTSERVER)
# define پیغام_کتار
#د_که_پای

#ضمن <float.h>
#إذا defined(HAVE_MATH_H)
  // for isnan() and isinf()
# include <math.h>
#د_که_پای

#إذا defined(FEAT_EVAL)
# ifdef USING_FLOAT_STUFF
#  ifdef MSWIN
#   ifndef isnan
#    define isnan(x) _isnan(x)
     ساكن __inline صحيح isinf(مزدوج x)
	{ أرجع !_finite(x) && !_isnan(x); }
#   endif
#  وإلا
#   ifndef HAVE_ISNAN
     ساكن مضمن صحيح isnan(مزدوج x)
	{ أرجع x != x; }
#   endif
#   ifndef HAVE_ISINF
     ساكن مضمن صحيح isinf(مزدوج x)
	{ أرجع !isnan(x) && isnan(x - x); }
#   endif
#  endif
#  إذا !defined(INFINITY)
#   إذا defined(DBL_MAX)
#    ifdef VMS
#     define INFINITY DBL_MAX
#    وإلا
#     define INFINITY (DBL_MAX+DBL_MAX)
#    endif
#   وإلا
#    define INFINITY (1.0 / 0.0)
#   endif
#  endif
#  إذا !defined(NAN)
#   define NAN (INFINITY-INFINITY)
#  endif
#  إذا !defined(DBL_EPSILON)
#   define DBL_EPSILON 2.2204460492503131e-16
#  endif
# endif
#د_که_پای

#که_تعریف_شوی FEAT_EVAL
# define FUNCARG(fp, j)	((char_u **)(fp->uf_دلیلونه.ga_معلومات))[j]
#د_که_پای

/*
 * In a hashtab item "hi_key" points to "di_key" in a dictitem.
 * This avoids adding a pointer to the hashtab item.
 * DI2HIKEY() converts a dictitem pointer to a hashitem key pointer.
 * HIKEY2DI() converts a hashitem key pointer to a dictitem pointer.
 * HI2DI() converts a hashitem pointer to a dictitem pointer.
 */
#تعریف_کړه DI2HIKEY(di) ((di)->di_کلی)
#تعریف_کړه HIKEY2DI(p)  ((dictitem_T *)((p) - offsetof(dictitem_T, di_کلی)))
#تعریف_کړه HI2DI(hi)     HIKEY2DI((hi)->hi_کلی)

/*
 * Flush control functions.
 */
#که_تعریف_شوی FEAT_GUI
# define mch_enable_flush()	gui_enable_flush()
# define mch_disable_flush()	gui_disable_flush()
#وإلا
# define mch_enable_flush()
# define mch_disable_flush()
#د_که_پای

/*
 * Like vim_free(), and also set the pointer to NULL.
 */
#تعریف_کړه VIM_CLEAR(p) \
    افعل { \
	vim_free(p); \
	(p) = عدم; \
    } طالما (0)

/*
 * Free a string and set it's pointer to NULL and length to 0
 */
#تعریف_کړه VIM_CLEAR_STRING(s) \
    افعل { \
	VIM_CLEAR(s.string); \
	s.اوږدتیا = 0; \
    } طالما (0)

#تعریف_کړه STR_LITERAL_INIT(s) \
    {(char_u *)(s), STRLEN_LITERAL(s)}

#تعریف_کړه STR_LITERAL_سېټ(str, s) \
    افعل { \
	(str).string = (char_u *)(s); \
	(str).اوږدتیا = STRLEN_LITERAL(s); \
    } طالما (0)

// Whether a command index indicates a user command.
#تعریف_کړه IS_USER_CMDIDX(idx) ((صحيح)(idx) < 0)

// Give an error in curwin is a popup window and evaluate to TRUE.
#که_تعریف_شوی FEAT_PROP_POPUP
# define WIN_IS_POPUP(wp) ((wp)->w_popup_flags != 0)
# define ERROR_IF_POPUP_کړکۍ error_if_popup_کړکۍ(FALSE)
# define ERROR_IF_ANY_POPUP_کړکۍ error_if_popup_کړکۍ(TRUE)
#وإلا
# define WIN_IS_POPUP(wp) 0
# define ERROR_IF_POPUP_کړکۍ 0
# define ERROR_IF_ANY_POPUP_کړکۍ 0
#د_که_پای
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
# define ERROR_IF_غړی_POPUP_کړکۍ error_if_غړی_popup_کړکۍ()
#وإلا
# define ERROR_IF_غړی_POPUP_کړکۍ 0
#د_که_پای


#که_تعریف_شوی ABORT_ON_دننی_ERROR
# define ESTACK_CHECK_DECLARATION صحيح estack_len_before
# define ESTACK_CHECK_تیاری افعل { estack_len_before = exestack.ga_len; } طالما (0)
# define ESTACK_CHECK_NOW \
    افعل { \
	إذا (estack_len_before != exestack.ga_len) \
	    siemsg("Exestack length expected: %d, actual: %d", estack_len_before, exestack.ga_len); \
    } طالما (0)
# define CHECK_CURBUF \
    افعل { \
	إذا (curwin != عدم && curwin->w_بفر != curbuf) \
	    iemsg("curbuf != curwin->w_buffer"); \
    } طالما (0)
#وإلا
# define ESTACK_CHECK_DECLARATION افعل { /**/ } طالما (0)
# define ESTACK_CHECK_تیاری افعل { /**/ } طالما (0)
# define ESTACK_CHECK_NOW افعل { /**/ } طالما (0)
# define CHECK_CURBUF افعل { /**/ } طالما (0)
#د_که_پای

// Inline the condition for performance.
#تعریف_کړه CHECK_LIST_MATERIALIZE(l) \
    افعل { \
	إذا ((l)->lv_لومړی == &range_list_توکی) \
	    range_list_materialize(l); \
    } طالما (0)

// Inlined version of ga_grow() with optimized condition that it fails.
#تعریف_کړه GA_GROW_FAILS(gap, n) unlikely((((gap)->ga_maxlen - (gap)->ga_len < (n)) ? ga_grow_inner((gap), (n)) : OK) == FAIL)
// Inlined version of ga_grow() with optimized condition that it succeeds.
#تعریف_کړه GA_GROW_OK(gap, n) likely((((gap)->ga_maxlen - (gap)->ga_len < (n)) ? ga_grow_inner((gap), (n)) : OK) == OK)
#تعریف_کړه GA_CONCAT_LITERAL(gap, s) ga_concat_len((gap), (char_u *)(s), STRLEN_LITERAL(s))

#که_نه_تعریف_شوی MIN
# define MIN(a, b) ((a) < (b) ? (a) : (b))
#د_که_پای
#که_نه_تعریف_شوی MAX
# define MAX(a, b) ((a) > (b) ? (a) : (b))
#د_که_پای

// Length of the array.
#تعریف_کړه ARRAY_اوږدتیا(a) (حجم(a) / حجم((a)[0]))

#که_تعریف_شوی FEAT_MENU
# define FOR_ALL_MENUS(m) \
    لكل ((m) = ریښه_menu; (m) != عدم; (m) = (m)->بل)
# define FOR_ALL_ماشوم_MENUS(p, c) \
    لكل ((c) = (p)->children; (c) != عدم; (c) = (c)->بل)
#د_که_پای

#تعریف_کړه FOR_ALL_WINDOWS(wp) \
    لكل ((wp) = firstwin; (wp) != عدم; (wp) = (wp)->w_بل)
#تعریف_کړه FOR_ALL_چوکاټونه(frp, لومړی_چوکاټ) \
    لكل ((frp) = لومړی_چوکاټ; (frp) != عدم; (frp) = (frp)->fr_بل)
#تعریف_کړه FOR_ALL_TABPAGES(tp) \
    لكل ((tp) = لومړی_tabpage; (tp) != عدم; (tp) = (tp)->tp_بل)
#تعریف_کړه FOR_ALL_WINDOWS_دننه_TAB(tp, wp) \
    لكل ((wp) = ((tp) == عدم || (tp) == curtab) \
	    ? firstwin : (tp)->tp_firstwin; (wp); (wp) = (wp)->w_بل)
/*
 * When using this macro "break" only breaks out of the inner loop. Use "goto"
 * to break out of the tabpage loop.
 */
#تعریف_کړه FOR_ALL_TAB_WINDOWS(tp, wp) \
    لكل ((tp) = لومړی_tabpage; (tp) != عدم; (tp) = (tp)->tp_بل) \
	لكل ((wp) = ((tp) == curtab) \
		? firstwin : (tp)->tp_firstwin; (wp); (wp) = (wp)->w_بل)

#تعریف_کړه FOR_ALL_POPUPWINS(wp) \
    لكل ((wp) = لومړی_popupwin; (wp) != عدم; (wp) = (wp)->w_بل)
#تعریف_کړه FOR_ALL_POPUPWINS_دننه_TAB(tp, wp) \
    لكل ((wp) = (tp)->tp_لومړی_popupwin; (wp) != عدم; (wp) = (wp)->w_بل)

#تعریف_کړه FOR_ALL_BUFFERS(buf) \
    لكل ((buf) = firstbuf; (buf) != عدم; (buf) = (buf)->b_بل)

#تعریف_کړه FOR_ALL_BUF_WININFO(buf, wip) \
    لكل ((wip) = (buf)->b_wininfo; (wip) != عدم; (wip) = (wip)->wi_بل)

// Iterate through all the signs placed in a buffer
#تعریف_کړه FOR_ALL_SIGNS_دننه_BUF(buf, sign) \
    لكل ((sign) = (buf)->b_signlist; (sign) != عدم; (sign) = (sign)->se_بل)

#که_تعریف_شوی FEAT_SPELL
# define FOR_ALL_SPELL_LANGS(slang) \
    لكل ((slang) = لومړی_lang; (slang) != عدم; (slang) = (slang)->sl_بل)
#د_که_پای

// Iterate over all the items in a List
#تعریف_کړه FOR_ALL_LIST_توکي(l, li) \
    لكل ((li) = (l) == عدم ? عدم : (l)->lv_لومړی; (li) != عدم; (li) = (li)->li_بل)

// Iterate over all the items in a hash table
#تعریف_کړه FOR_ALL_HASHTAB_توکي(ht, hi, todo) \
    لكل ((hi) = (ht)->ht_array; (todo) > 0; ++(hi))

#تعریف_کړه TUPLE_LEN(t)	    (t->tv_توکي.ga_len)
#تعریف_کړه TUPLE_توکی(t, i) \
	    (((typval_T *)t->tv_توکي.ga_معلومات) + i)

// Avoid open() on a non-existing file on MS-Windows
#إذا !defined(MSWIN) && !defined(PROTO)
# define mch_isdir_or_missing(f) mch_isdir(f)
#د_که_پای
