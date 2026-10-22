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
#تعریف_کرو LT_POS(a, b) (((a).lnum != (b).lnum) \
		   ? (a).lnum < (b).lnum \
		   : (a).ستون != (b).ستون \
		       ? (a).ستون < (b).ستون \
		       : (a).coladd < (b).coladd)
#تعریف_کرو LT_POSP(a, b) (((a)->lnum != (b)->lnum) \
		   ? (a)->lnum < (b)->lnum \
		   : (a)->ستون != (b)->ستون \
		       ? (a)->ستون < (b)->ستون \
		       : (a)->coladd < (b)->coladd)
#تعریف_کرو EQUAL_POS(a, b) (((a).lnum == (b).lnum) && ((a).ستون == (b).ستون) && ((a).coladd == (b).coladd))
#تعریف_کرو CLEAR_POS(a) کرو {(a)->lnum = 0; (a)->ستون = 0; (a)->coladd = 0;} جب (0)
#تعریف_کرو خالی_خانے_POS(a) ((a).lnum == 0 && (a).ستون == 0 && (a).coladd == 0)

#تعریف_کرو LTOREQ_POS(a, b) (LT_POS(a, b) || EQUAL_POS(a, b))

/*
 * VIM_ISWHITE() differs from isspace() because it doesn't include <CR> and
 * <LF> and the like.
 */
#تعریف_کرو VIM_ISWHITE(x)		((x) == ' ' || (x) == '\t')
#تعریف_کرو IS_WHITE_OR_NUL(x)	((x) == ' ' || (x) == '\t' || (x) == NUL)
#تعریف_کرو IS_WHITE_NL_OR_NUL(x)	((x) == ' ' || (x) == '\t' || (x) == '\n' || (x) == NUL)

/*
 * LINEEMPTY() - return TRUE if the line is empty
 */
#تعریف_کرو LINEEMPTY(p) (*ml_get(p) == NUL)

/*
 * BUFEMPTY() - return TRUE if the current buffer is empty
 */
#تعریف_کرو BUFEMPTY() (curbuf->b_ml.ml_سطر_گنتی == 1 && *ml_get((linenr_T)1) == NUL)

// The is*() and to*() functions declared in <ctype.h> have
// undefined behavior for values other than EOF outside the range of
// unsigned char.  If plain char is signed, a call with a negative
// value has undefined behavior.  These macros cast the argument to
// unsigned char.  (Most implementations behave more or less sanely
// with negative values, and most character values in practice are
// positive, but we want to avoid undefined behavior anyway.)
#تعریف_کرو محفوظ_isalnum(c)  (حرف_یا_ہندسہ ((غیرعلامتی حرف)(c)))
#تعریف_کرو محفوظ_isalpha(c)  (حرف_ہے ((غیرعلامتی حرف)(c)))
#تعریف_کرو محفوظ_isblank(c)  (isblank ((غیرعلامتی حرف)(c)))
#تعریف_کرو محفوظ_iscntrl(c)  (iscntrl ((غیرعلامتی حرف)(c)))
#تعریف_کرو محفوظ_isdigit(c)  (ہندسہ_ہے ((غیرعلامتی حرف)(c)))
#تعریف_کرو محفوظ_isgraph(c)  (isgraph ((غیرعلامتی حرف)(c)))
#تعریف_کرو محفوظ_islower(c)  (islower ((غیرعلامتی حرف)(c)))
#تعریف_کرو محفوظ_isprint(c)  (isprint ((غیرعلامتی حرف)(c)))
#تعریف_کرو محفوظ_ispunct(c)  (ispunct ((غیرعلامتی حرف)(c)))
#تعریف_کرو محفوظ_isspace(c)  (خلا_ہے ((غیرعلامتی حرف)(c)))
#تعریف_کرو محفوظ_isupper(c)  (isupper ((غیرعلامتی حرف)(c)))
#تعریف_کرو محفوظ_isxdigit(c) (isxdigit((غیرعلامتی حرف)(c)))
#تعریف_کرو محفوظ_tolower(c)  (tolower ((غیرعلامتی حرف)(c)))
#تعریف_کرو محفوظ_toupper(c)  (بڑا_حرف ((غیرعلامتی حرف)(c)))

/*
 * toupper() and tolower() that use the current locale.
 * On some systems toupper()/tolower() only work on lower/uppercase
 * characters, first use islower() or isupper() then.
 * Careful: Only call TOUPPER_LOC() and TOLOWER_LOC() with a character in the
 * range 0 - 255.  toupper()/tolower() on some systems can't handle others.
 * Note: It is often better to use MB_TOLOWER() and MB_TOUPPER(), because many
 * toupper() and tolower() implementations only work for ASCII.
 */
#اگر_تعریف_شدہ MSWIN
# define TOUPPER_LOC(c)	toupper_tab[(c) & 255]
# define TOLOWER_LOC(c)	tolower_tab[(c) & 255]
#ورنہ
# ifdef BROKEN_TOUPPER
#  define TOUPPER_LOC(c)	(محفوظ_islower(c) ? محفوظ_toupper(c) : (c))
#  define TOLOWER_LOC(c)	(محفوظ_isupper(c) ? محفوظ_tolower(c) : (c))
# ورنہ
#  define TOUPPER_LOC		محفوظ_toupper
#  define TOLOWER_LOC		محفوظ_tolower
# endif
#اگر_ختم

// toupper() and tolower() for ASCII only and ignore the current locale.
#تعریف_کرو TOUPPER_ASC(c)	(((c) < 'a' || (c) > 'z') ? (c) : (c) - ('a' - 'A'))
#تعریف_کرو TOLOWER_ASC(c)	(((c) < 'A' || (c) > 'Z') ? (c) : (c) + ('a' - 'A'))

/*
 * MB_ISLOWER() and MB_ISUPPER() are to be used on multi-byte characters.  But
 * don't use them for negative values!
 */
#تعریف_کرو MB_ISLOWER(c)	vim_islower(c)
#تعریف_کرو MB_ISUPPER(c)	vim_isupper(c)
#تعریف_کرو MB_TOLOWER(c)	vim_tolower(c)
#تعریف_کرو MB_TOUPPER(c)	vim_toupper(c)
#تعریف_کرو MB_CASEFOLD(c)	(enc_utf8 ? utf_fold(c) : MB_TOLOWER(c))

// Use our own isdigit() replacement, because on MS-Windows isdigit() returns
// non-zero for superscript 1.  Also avoids that isdigit() crashes for numbers
// below 0 and above 255.
#تعریف_کرو VIM_ISDIGIT(c) ((غیرعلامتی)(c) - '0' < 10)

// Like isalpha() but reject non-ASCII characters.  Can't be used with a
// special key (negative value).
#تعریف_کرو ASCII_ISLOWER(c) ((غیرعلامتی)(c) - 'a' < 26)
#تعریف_کرو ASCII_ISUPPER(c) ((غیرعلامتی)(c) - 'A' < 26)
#تعریف_کرو ASCII_ISALPHA(c) (ASCII_ISUPPER(c) || ASCII_ISLOWER(c))
#تعریف_کرو ASCII_ISALNUM(c) (ASCII_ISALPHA(c) || VIM_ISDIGIT(c))

// Returns empty string if it is NULL.
#تعریف_کرو خالی_خانے_IF_NULL(x) ((x) ? (x) : (char_u *)"")

#اگر_تعریف_شدہ FEAT_LANGMAP
/*
 * Adjust chars in a language according to 'langmap' option.
 * NOTE that there is no noticeable overhead if 'langmap' is not set.
 * When set the overhead for characters < 256 is small.
 * Don't apply 'langmap' if the character comes from the Stuff buffer or from
 * a mapping and the langnoremap option was set.
 * The do-while is just to ignore a ';' after the macro.
 */
# define LANGMAP_ADJUST(c, condition) \
    کرو { \
	اگر (*p_langmap \
		&& (condition) \
		&& (p_lrm || (!p_lrm && کلید_Typed)) \
		&& !کلید_Stuffed \
		&& (c) >= 0) \
	{ \
	    اگر ((c) < 256) \
		c = langmap_mapchar[c]; \
	    ورنہ \
		c = langmap_adjust_mb(c); \
	} \
    } جب (0)
#ورنہ
# define LANGMAP_ADJUST(c, condition) // nop
#اگر_ختم

/*
 * VIM_ISBREAK() is used very often if 'linebreak' is set, use a macro to make
 * it work fast.  Only works for single byte characters!
 */
#تعریف_کرو VIM_ISBREAK(c) ((c) < 256 && breakat_flags[(char_u)(c)])

/*
 * On VMS file names are different and require a translation.
 * On the Mac open() has only two arguments.
 */
#اگر_تعریف_شدہ VMS
# define mch_access(n, p)	access(vms_fixfilename(n), (p))
				// see mch_open() comment
# define mch_fopen(n, p)	fopen(vms_fixfilename(n), (p))
# define mch_fstat(n, p)	fstat((n), (p))
# undef HAVE_LSTAT		// VMS does not have lstat()
# define mch_stat(n, p)		stat(vms_fixfilename(n), (p))
#ورنہ
# اگر !defined(MSWIN) && !defined(PROTO)
#  define mch_access(n, p)	access((n), (p))
# endif

// Use 64-bit fstat function on MS-Windows.
// NOTE: This condition is the same as for the stat_T type.
# ifdef MSWIN
#  define mch_fstat(n, p)	_fstat64((n), (p))
# ورنہ
#  define mch_fstat(n, p)	fstat((n), (p))
# endif

# ifdef MSWIN	// has its own mch_stat() function
#  define mch_stat(n, p)	vim_stat((n), (p))
# ورنہ
#  ifdef STAT_IGNORES_SLASH
#   define mch_stat(n, p)	vim_stat((n), (p))
#  ورنہ
#   define mch_stat(n, p)	stat((n), (p))
#  endif
# endif
#اگر_ختم

#اگر_تعریف_شدہ HAVE_LSTAT
# define mch_lstat(n, p)	lstat((n), (p))
#ورنہ
# ifdef MSWIN
#  define mch_lstat(n, p)	vim_lstat((n), (p))
# ورنہ
#  define mch_lstat(n, p)	mch_stat((n), (p))
# endif
#اگر_ختم

#اگر_تعریف_شدہ VMS
/*
 * It is possible to force some record format with:
 * #  define mch_open(n, m, p) open(vms_fixfilename(n), (m), (p)), "rat=cr", "rfm=stmlf", "mrs=0")
 * but it is not recommended, because it can destroy indexes etc.
 */
# define mch_open(n, m, p)	open(vms_fixfilename(n), (m), (p))
#اگر_ختم

// mch_open_rw(): invoke mch_open() with third argument for user R/W.
#اگر defined(UNIX) || defined(VMS)  // open in rw------- mode
# define mch_open_rw(n, f)	mch_open((n), (f), (mode_t)0600)
#ورنہ
# اگر defined(MSWIN)  // open read/write
#  define mch_open_rw(n, f)	mch_open((n), (f), S_IREAD | S_IWRITE)
# ورنہ
#  define mch_open_rw(n, f)	mch_open((n), (f), 0)
# endif
#اگر_ختم

#اگر_تعریف_شدہ STARTUPTIME
# define وقت_پیغام(s) کرو { اگر (وقت_fd != تہی) وقت_پیغام_2(s, تہی); } جب (0)
#ورنہ
# define وقت_پیغام(s) کرو { /**/ } جب (0)
#اگر_ختم

#تعریف_کرو REPLACE_معمول(s) (((s) & REPLACE_FLAG) && !((s) & VREPLACE_FLAG))

#اگر_تعریف_شدہ FEAT_ARABIC
# define ARABIC_CHAR(ch)	    (((ch) & 0xFF00) == 0x0600)
# define UTF_COMPOSINGLIKE(p1, p2)  utf_composinglike((p1), (p2))
#ورنہ
# define UTF_COMPOSINGLIKE(p1, p2)  utf_iscomposing(utf_ptr2char(p2))
#اگر_ختم

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    // Whether to draw the vertical bar on the right side of the cell.
# define CURSOR_BAR_دایاں (curwin->w_p_rl && (!(State & منوال_CMDLINE) || cmdmsg_rl))
#اگر_ختم

/*
 * MB_PTR_ADV(): advance a pointer to the next character, taking care of
 * multi-byte characters if needed.
 * MB_PTR_BACK(): backup a pointer to the previous character, taking care of
 * multi-byte characters if needed.
 * MB_COPY_CHAR(f, t): copy one char from "f" to "t" and advance the pointers.
 * PTR2CHAR(): get character from pointer.
 */
// Advance multi-byte pointer, skip over composing chars.
#تعریف_کرو MB_PTR_ADV(p)	    p += (*mb_ptr2len)(p)
// Advance multi-byte pointer, do not skip over composing chars.
#تعریف_کرو MB_CPTR_ADV(p)	    p += enc_utf8 ? utf_ptr2len(p) : (*mb_ptr2len)(p)
// Backup multi-byte pointer. Only use with "p" > "s" !
#تعریف_کرو MB_PTR_عقب(s, p)  p -= has_mbyte ? ((*mb_سر_off)(s, (p) - 1) + 1) : 1
// get length of multi-byte char, not including composing chars
#تعریف_کرو MB_CPTR2LEN(p)	    (enc_utf8 ? utf_ptr2len(p) : (*mb_ptr2len)(p))

#تعریف_کرو MB_COPY_CHAR(f, t) کرو { اگر (has_mbyte) mb_copy_char(&(f), &(t)); ورنہ *(t)++ = *(f)++; } جب (0)
#تعریف_کرو MB_CHARLEN(p)	    (has_mbyte ? mb_charlen(p) : (صحیح)STRLEN(p))
#تعریف_کرو MB_CHAR2LEN(c)	    (has_mbyte ? mb_char2len(c) : 1)
#تعریف_کرو PTR2CHAR(p)	    (has_mbyte ? mb_ptr2char(p) : (صحیح)*(p))
#تعریف_کرو MB_CHAR2_بائٹیں(c, b) کرو { اگر (has_mbyte) (b) += (*mb_char2bytes)((c), (b)); ورنہ *(b)++ = (c); } جب (0)

#اگر_تعریف_شدہ FEAT_AUTOCHDIR
# define DO_AUTOCHDIR کرو { اگر (p_acd) do_autochdir(); } جب (0)
#ورنہ
# define DO_AUTOCHDIR کرو { /**/ } جب (0)
#اگر_ختم

#تعریف_کرو RESET_BINDING(wp)  کرو { (wp)->w_p_scb = FALSE; (wp)->w_p_crb = FALSE; \
			    } جب (0)

#اگر_تعریف_شدہ FEAT_DIFF
# define PLINES_NOFILL(x) plines_nofill(x)
# define PLINES_WIN_NOFILL(w, l, h) plines_win_nofill((w), (l), (h))
#ورنہ
# define PLINES_NOFILL(x) plines(x)
# define PLINES_WIN_NOFILL(w, l, h) plines_win((w), (l), (h))
#اگر_ختم

#اگر defined(FEAT_کام_CHANNEL) || defined(FEAT_CLIENTSERVER)
# define پیغام_قطار
#اگر_ختم

#شامل <float.h>
#اگر defined(HAVE_MATH_H)
  // for isnan() and isinf()
# include <math.h>
#اگر_ختم

#اگر defined(FEAT_EVAL)
# ifdef USING_FLOAT_STUFF
#  ifdef MSWIN
#   ifndef isnan
#    define isnan(x) _isnan(x)
     ساکن __inline صحیح isinf(دوہرا x)
	{ واپس !_finite(x) && !_isnan(x); }
#   endif
#  ورنہ
#   ifndef HAVE_ISNAN
     ساکن اندرونی صحیح isnan(دوہرا x)
	{ واپس x != x; }
#   endif
#   ifndef HAVE_ISINF
     ساکن اندرونی صحیح isinf(دوہرا x)
	{ واپس !isnan(x) && isnan(x - x); }
#   endif
#  endif
#  اگر !defined(INFINITY)
#   اگر defined(DBL_MAX)
#    ifdef VMS
#     define INFINITY DBL_MAX
#    ورنہ
#     define INFINITY (DBL_MAX+DBL_MAX)
#    endif
#   ورنہ
#    define INFINITY (1.0 / 0.0)
#   endif
#  endif
#  اگر !defined(NAN)
#   define NAN (INFINITY-INFINITY)
#  endif
#  اگر !defined(DBL_EPSILON)
#   define DBL_EPSILON 2.2204460492503131e-16
#  endif
# endif
#اگر_ختم

#اگر_تعریف_شدہ FEAT_EVAL
# define FUNCARG(fp, j)	((char_u **)(fp->uf_دلائل.ga_ڈیٹا))[j]
#اگر_ختم

/*
 * In a hashtab item "hi_key" points to "di_key" in a dictitem.
 * This avoids adding a pointer to the hashtab item.
 * DI2HIKEY() converts a dictitem pointer to a hashitem key pointer.
 * HIKEY2DI() converts a hashitem key pointer to a dictitem pointer.
 * HI2DI() converts a hashitem pointer to a dictitem pointer.
 */
#تعریف_کرو DI2HIKEY(di) ((di)->di_کلید)
#تعریف_کرو HIKEY2DI(p)  ((dictitem_T *)((p) - offsetof(dictitem_T, di_کلید)))
#تعریف_کرو HI2DI(hi)     HIKEY2DI((hi)->hi_کلید)

/*
 * Flush control functions.
 */
#اگر_تعریف_شدہ FEAT_GUI
# define mch_enable_flush()	gui_enable_flush()
# define mch_disable_flush()	gui_disable_flush()
#ورنہ
# define mch_enable_flush()
# define mch_disable_flush()
#اگر_ختم

/*
 * Like vim_free(), and also set the pointer to NULL.
 */
#تعریف_کرو VIM_CLEAR(p) \
    کرو { \
	vim_free(p); \
	(p) = تہی; \
    } جب (0)

/*
 * Free a string and set it's pointer to NULL and length to 0
 */
#تعریف_کرو VIM_CLEAR_STRING(s) \
    کرو { \
	VIM_CLEAR(s.string); \
	s.لمبائی = 0; \
    } جب (0)

#تعریف_کرو STR_LITERAL_INIT(s) \
    {(char_u *)(s), STRLEN_LITERAL(s)}

#تعریف_کرو STR_LITERAL_سیٹ(str, s) \
    کرو { \
	(str).string = (char_u *)(s); \
	(str).لمبائی = STRLEN_LITERAL(s); \
    } جب (0)

// Whether a command index indicates a user command.
#تعریف_کرو IS_USER_CMDIDX(idx) ((صحیح)(idx) < 0)

// Give an error in curwin is a popup window and evaluate to TRUE.
#اگر_تعریف_شدہ FEAT_PROP_POPUP
# define WIN_IS_POPUP(wp) ((wp)->w_popup_flags != 0)
# define ERROR_IF_POPUP_کھڑکی error_if_popup_کھڑکی(FALSE)
# define ERROR_IF_ANY_POPUP_کھڑکی error_if_popup_کھڑکی(TRUE)
#ورنہ
# define WIN_IS_POPUP(wp) 0
# define ERROR_IF_POPUP_کھڑکی 0
# define ERROR_IF_ANY_POPUP_کھڑکی 0
#اگر_ختم
#اگر defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
# define ERROR_IF_رکن_POPUP_کھڑکی error_if_رکن_popup_کھڑکی()
#ورنہ
# define ERROR_IF_رکن_POPUP_کھڑکی 0
#اگر_ختم


#اگر_تعریف_شدہ ABORT_ON_اندرونی_ERROR
# define ESTACK_CHECK_DECLARATION صحیح estack_len_before
# define ESTACK_CHECK_تیاری کرو { estack_len_before = exestack.ga_len; } جب (0)
# define ESTACK_CHECK_NOW \
    کرو { \
	اگر (estack_len_before != exestack.ga_len) \
	    siemsg("Exestack length expected: %d, actual: %d", estack_len_before, exestack.ga_len); \
    } جب (0)
# define CHECK_CURBUF \
    کرو { \
	اگر (curwin != تہی && curwin->w_بفر != curbuf) \
	    iemsg("curbuf != curwin->w_buffer"); \
    } جب (0)
#ورنہ
# define ESTACK_CHECK_DECLARATION کرو { /**/ } جب (0)
# define ESTACK_CHECK_تیاری کرو { /**/ } جب (0)
# define ESTACK_CHECK_NOW کرو { /**/ } جب (0)
# define CHECK_CURBUF کرو { /**/ } جب (0)
#اگر_ختم

// Inline the condition for performance.
#تعریف_کرو CHECK_LIST_MATERIALIZE(l) \
    کرو { \
	اگر ((l)->lv_پہلا == &range_list_شے) \
	    range_list_materialize(l); \
    } جب (0)

// Inlined version of ga_grow() with optimized condition that it fails.
#تعریف_کرو GA_GROW_FAILS(gap, n) unlikely((((gap)->ga_maxlen - (gap)->ga_len < (n)) ? ga_grow_inner((gap), (n)) : OK) == FAIL)
// Inlined version of ga_grow() with optimized condition that it succeeds.
#تعریف_کرو GA_GROW_OK(gap, n) likely((((gap)->ga_maxlen - (gap)->ga_len < (n)) ? ga_grow_inner((gap), (n)) : OK) == OK)
#تعریف_کرو GA_CONCAT_LITERAL(gap, s) ga_concat_len((gap), (char_u *)(s), STRLEN_LITERAL(s))

#اگر_غیر_تعریف_شدہ MIN
# define MIN(a, b) ((a) < (b) ? (a) : (b))
#اگر_ختم
#اگر_غیر_تعریف_شدہ MAX
# define MAX(a, b) ((a) > (b) ? (a) : (b))
#اگر_ختم

// Length of the array.
#تعریف_کرو ARRAY_لمبائی(a) (حجم(a) / حجم((a)[0]))

#اگر_تعریف_شدہ FEAT_MENU
# define FOR_ALL_MENUS(m) \
    برائے ((m) = جڑ_menu; (m) != تہی; (m) = (m)->اگلا)
# define FOR_ALL_بچہ_MENUS(p, c) \
    برائے ((c) = (p)->children; (c) != تہی; (c) = (c)->اگلا)
#اگر_ختم

#تعریف_کرو FOR_ALL_WINDOWS(wp) \
    برائے ((wp) = firstwin; (wp) != تہی; (wp) = (wp)->w_اگلا)
#تعریف_کرو FOR_ALL_چوکھٹے(frp, پہلا_چوکھٹا) \
    برائے ((frp) = پہلا_چوکھٹا; (frp) != تہی; (frp) = (frp)->fr_اگلا)
#تعریف_کرو FOR_ALL_TABPAGES(tp) \
    برائے ((tp) = پہلا_tabpage; (tp) != تہی; (tp) = (tp)->tp_اگلا)
#تعریف_کرو FOR_ALL_WINDOWS_اندر_TAB(tp, wp) \
    برائے ((wp) = ((tp) == تہی || (tp) == curtab) \
	    ? firstwin : (tp)->tp_firstwin; (wp); (wp) = (wp)->w_اگلا)
/*
 * When using this macro "break" only breaks out of the inner loop. Use "goto"
 * to break out of the tabpage loop.
 */
#تعریف_کرو FOR_ALL_TAB_WINDOWS(tp, wp) \
    برائے ((tp) = پہلا_tabpage; (tp) != تہی; (tp) = (tp)->tp_اگلا) \
	برائے ((wp) = ((tp) == curtab) \
		? firstwin : (tp)->tp_firstwin; (wp); (wp) = (wp)->w_اگلا)

#تعریف_کرو FOR_ALL_POPUPWINS(wp) \
    برائے ((wp) = پہلا_popupwin; (wp) != تہی; (wp) = (wp)->w_اگلا)
#تعریف_کرو FOR_ALL_POPUPWINS_اندر_TAB(tp, wp) \
    برائے ((wp) = (tp)->tp_پہلا_popupwin; (wp) != تہی; (wp) = (wp)->w_اگلا)

#تعریف_کرو FOR_ALL_BUFFERS(buf) \
    برائے ((buf) = firstbuf; (buf) != تہی; (buf) = (buf)->b_اگلا)

#تعریف_کرو FOR_ALL_BUF_WININFO(buf, wip) \
    برائے ((wip) = (buf)->b_wininfo; (wip) != تہی; (wip) = (wip)->wi_اگلا)

// Iterate through all the signs placed in a buffer
#تعریف_کرو FOR_ALL_SIGNS_اندر_BUF(buf, sign) \
    برائے ((sign) = (buf)->b_signlist; (sign) != تہی; (sign) = (sign)->se_اگلا)

#اگر_تعریف_شدہ FEAT_SPELL
# define FOR_ALL_SPELL_LANGS(slang) \
    برائے ((slang) = پہلا_lang; (slang) != تہی; (slang) = (slang)->sl_اگلا)
#اگر_ختم

// Iterate over all the items in a List
#تعریف_کرو FOR_ALL_LIST_اشیاء(l, li) \
    برائے ((li) = (l) == تہی ? تہی : (l)->lv_پہلا; (li) != تہی; (li) = (li)->li_اگلا)

// Iterate over all the items in a hash table
#تعریف_کرو FOR_ALL_HASHTAB_اشیاء(ht, hi, todo) \
    برائے ((hi) = (ht)->ht_array; (todo) > 0; ++(hi))

#تعریف_کرو TUPLE_LEN(t)	    (t->tv_اشیاء.ga_len)
#تعریف_کرو TUPLE_شے(t, i) \
	    (((typval_T *)t->tv_اشیاء.ga_ڈیٹا) + i)

// Avoid open() on a non-existing file on MS-Windows
#اگر !defined(MSWIN) && !defined(PROTO)
# define mch_isdir_or_missing(f) mch_isdir(f)
#اگر_ختم
