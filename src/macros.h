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
#define LT_POS(a, b) (((a).lnum != (b).lnum) \
		   ? (a).lnum < (b).lnum \
		   : (a).col != (b).col \
		       ? (a).col < (b).col \
		       : (a).coladd < (b).coladd)
#define LT_POSP(a, b) (((a)->lnum != (b)->lnum) \
		   ? (a)->lnum < (b)->lnum \
		   : (a)->col != (b)->col \
		       ? (a)->col < (b)->col \
		       : (a)->coladd < (b)->coladd)
#define EQUAL_POS(a, b) (((a).lnum == (b).lnum) && ((a).col == (b).col) && ((a).coladd == (b).coladd))
#define CLEAR_POS(a) செய் {(a)->lnum = 0; (a)->col = 0; (a)->coladd = 0;} போது (0)
#define EMPTY_POS(a) ((a).lnum == 0 && (a).col == 0 && (a).coladd == 0)

#define LTOREQ_POS(a, b) (LT_POS(a, b) || EQUAL_POS(a, b))

/*
 * VIM_ISWHITE() differs from isspace() because it doesn't include <CR> and
 * <LF> and the like.
 */
#define VIM_ISWHITE(x)		((x) == ' ' || (x) == '\t')
#define IS_WHITE_OR_NUL(x)	((x) == ' ' || (x) == '\t' || (x) == NUL)
#define IS_WHITE_NL_OR_NUL(x)	((x) == ' ' || (x) == '\t' || (x) == '\n' || (x) == NUL)

/*
 * LINEEMPTY() - return TRUE if the line is empty
 */
#define LINEEMPTY(p) (*ml_get(p) == NUL)

/*
 * BUFEMPTY() - return TRUE if the current buffer is empty
 */
#define BUFEMPTY() (curbuf->b_ml.ml_வரி_மொத்தம் == 1 && *ml_get((linenr_T)1) == NUL)

// The is*() and to*() functions declared in <ctype.h> have
// undefined behavior for values other than EOF outside the range of
// unsigned char.  If plain char is signed, a call with a negative
// value has undefined behavior.  These macros cast the argument to
// unsigned char.  (Most implementations behave more or less sanely
// with negative values, and most character values in practice are
// positive, but we want to avoid undefined behavior anyway.)
#define SAFE_isalnum(c)  (isalnum ((unsigned எழுத்து)(c)))
#define SAFE_isalpha(c)  (isalpha ((unsigned எழுத்து)(c)))
#define SAFE_isblank(c)  (isblank ((unsigned எழுத்து)(c)))
#define SAFE_iscntrl(c)  (iscntrl ((unsigned எழுத்து)(c)))
#define SAFE_isdigit(c)  (isdigit ((unsigned எழுத்து)(c)))
#define SAFE_isgraph(c)  (isgraph ((unsigned எழுத்து)(c)))
#define SAFE_islower(c)  (islower ((unsigned எழுத்து)(c)))
#define SAFE_isprint(c)  (isprint ((unsigned எழுத்து)(c)))
#define SAFE_ispunct(c)  (ispunct ((unsigned எழுத்து)(c)))
#define SAFE_isspace(c)  (isspace ((unsigned எழுத்து)(c)))
#define SAFE_isupper(c)  (isupper ((unsigned எழுத்து)(c)))
#define SAFE_isxdigit(c) (isxdigit((unsigned எழுத்து)(c)))
#define SAFE_tolower(c)  (tolower ((unsigned எழுத்து)(c)))
#define SAFE_toupper(c)  (toupper ((unsigned எழுத்து)(c)))

/*
 * toupper() and tolower() that use the current locale.
 * On some systems toupper()/tolower() only work on lower/uppercase
 * characters, first use islower() or isupper() then.
 * Careful: Only call TOUPPER_LOC() and TOLOWER_LOC() with a character in the
 * range 0 - 255.  toupper()/tolower() on some systems can't handle others.
 * Note: It is often better to use MB_TOLOWER() and MB_TOUPPER(), because many
 * toupper() and tolower() implementations only work for ASCII.
 */
#ifdef MSWIN
# define TOUPPER_LOC(c)	toupper_tab[(c) & 255]
# define TOLOWER_LOC(c)	tolower_tab[(c) & 255]
#இல்லையெனில்
# ifdef BROKEN_TOUPPER
#  define TOUPPER_LOC(c)	(SAFE_islower(c) ? SAFE_toupper(c) : (c))
#  define TOLOWER_LOC(c)	(SAFE_isupper(c) ? SAFE_tolower(c) : (c))
# இல்லையெனில்
#  define TOUPPER_LOC		SAFE_toupper
#  define TOLOWER_LOC		SAFE_tolower
# endif
#endif

// toupper() and tolower() for ASCII only and ignore the current locale.
#define TOUPPER_ASC(c)	(((c) < 'a' || (c) > 'z') ? (c) : (c) - ('a' - 'A'))
#define TOLOWER_ASC(c)	(((c) < 'A' || (c) > 'Z') ? (c) : (c) + ('a' - 'A'))

/*
 * MB_ISLOWER() and MB_ISUPPER() are to be used on multi-byte characters.  But
 * don't use them for negative values!
 */
#define MB_ISLOWER(c)	vim_islower(c)
#define MB_ISUPPER(c)	vim_isupper(c)
#define MB_TOLOWER(c)	vim_tolower(c)
#define MB_TOUPPER(c)	vim_toupper(c)
#define MB_CASEFOLD(c)	(enc_utf8 ? utf_fold(c) : MB_TOLOWER(c))

// Use our own isdigit() replacement, because on MS-Windows isdigit() returns
// non-zero for superscript 1.  Also avoids that isdigit() crashes for numbers
// below 0 and above 255.
#define VIM_ISDIGIT(c) ((unsigned)(c) - '0' < 10)

// Like isalpha() but reject non-ASCII characters.  Can't be used with a
// special key (negative value).
#define ASCII_ISLOWER(c) ((unsigned)(c) - 'a' < 26)
#define ASCII_ISUPPER(c) ((unsigned)(c) - 'A' < 26)
#define ASCII_ISALPHA(c) (ASCII_ISUPPER(c) || ASCII_ISLOWER(c))
#define ASCII_ISALNUM(c) (ASCII_ISALPHA(c) || VIM_ISDIGIT(c))

// Returns empty string if it is NULL.
#define EMPTY_IF_NULL(x) ((x) ? (x) : (char_u *)"")

#ifdef FEAT_LANGMAP
/*
 * Adjust chars in a language according to 'langmap' option.
 * NOTE that there is no noticeable overhead if 'langmap' is not set.
 * When set the overhead for characters < 256 is small.
 * Don't apply 'langmap' if the character comes from the Stuff buffer or from
 * a mapping and the langnoremap option was set.
 * The do-while is just to ignore a ';' after the macro.
 */
# define LANGMAP_ADJUST(c, condition) \
    செய் { \
	எனில் (*p_langmap \
		&& (condition) \
		&& (p_lrm || (!p_lrm && சாவி_Typed)) \
		&& !சாவி_Stuffed \
		&& (c) >= 0) \
	{ \
	    எனில் ((c) < 256) \
		c = langmap_mapchar[c]; \
	    இல்லையெனில் \
		c = langmap_adjust_mb(c); \
	} \
    } போது (0)
#இல்லையெனில்
# define LANGMAP_ADJUST(c, condition) // nop
#endif

/*
 * VIM_ISBREAK() is used very often if 'linebreak' is set, use a macro to make
 * it work fast.  Only works for single byte characters!
 */
#define VIM_ISBREAK(c) ((c) < 256 && breakat_flags[(char_u)(c)])

/*
 * On VMS file names are different and require a translation.
 * On the Mac open() has only two arguments.
 */
#ifdef VMS
# define mch_access(n, p)	access(vms_fixfilename(n), (p))
				// see mch_open() comment
# define mch_fopen(n, p)	fopen(vms_fixfilename(n), (p))
# define mch_fstat(n, p)	fstat((n), (p))
# undef HAVE_LSTAT		// VMS does not have lstat()
# define mch_stat(n, p)		stat(vms_fixfilename(n), (p))
#இல்லையெனில்
# எனில் !defined(MSWIN) && !defined(PROTO)
#  define mch_access(n, p)	access((n), (p))
# endif

// Use 64-bit fstat function on MS-Windows.
// NOTE: This condition is the same as for the stat_T type.
# ifdef MSWIN
#  define mch_fstat(n, p)	_fstat64((n), (p))
# இல்லையெனில்
#  define mch_fstat(n, p)	fstat((n), (p))
# endif

# ifdef MSWIN	// has its own mch_stat() function
#  define mch_stat(n, p)	vim_stat((n), (p))
# இல்லையெனில்
#  ifdef STAT_IGNORES_SLASH
#   define mch_stat(n, p)	vim_stat((n), (p))
#  இல்லையெனில்
#   define mch_stat(n, p)	stat((n), (p))
#  endif
# endif
#endif

#ifdef HAVE_LSTAT
# define mch_lstat(n, p)	lstat((n), (p))
#இல்லையெனில்
# ifdef MSWIN
#  define mch_lstat(n, p)	vim_lstat((n), (p))
# இல்லையெனில்
#  define mch_lstat(n, p)	mch_stat((n), (p))
# endif
#endif

#ifdef VMS
/*
 * It is possible to force some record format with:
 * #  define mch_open(n, m, p) open(vms_fixfilename(n), (m), (p)), "rat=cr", "rfm=stmlf", "mrs=0")
 * but it is not recommended, because it can destroy indexes etc.
 */
# define mch_open(n, m, p)	open(vms_fixfilename(n), (m), (p))
#endif

// mch_open_rw(): invoke mch_open() with third argument for user R/W.
#எனில் defined(UNIX) || defined(VMS)  // open in rw------- mode
# define mch_open_rw(n, f)	mch_open((n), (f), (mode_t)0600)
#இல்லையெனில்
# எனில் defined(MSWIN)  // open read/write
#  define mch_open_rw(n, f)	mch_open((n), (f), S_IREAD | S_IWRITE)
# இல்லையெனில்
#  define mch_open_rw(n, f)	mch_open((n), (f), 0)
# endif
#endif

#ifdef STARTUPTIME
# define TIME_செய்தி(s) செய் { எனில் (time_fd != வெற்று) time_செய்தி(s, வெற்று); } போது (0)
#இல்லையெனில்
# define TIME_செய்தி(s) செய் { /**/ } போது (0)
#endif

#define REPLACE_NORMAL(s) (((s) & REPLACE_FLAG) && !((s) & VREPLACE_FLAG))

#ifdef FEAT_ARABIC
# define ARABIC_CHAR(ch)	    (((ch) & 0xFF00) == 0x0600)
# define UTF_COMPOSINGLIKE(p1, p2)  utf_composinglike((p1), (p2))
#இல்லையெனில்
# define UTF_COMPOSINGLIKE(p1, p2)  utf_iscomposing(utf_ptr2char(p2))
#endif

#ifdef FEAT_RIGHTLEFT
    // Whether to draw the vertical bar on the right side of the cell.
# define CURSOR_BAR_வலது (curwin->w_p_rl && (!(State & MODE_CMDLINE) || cmdmsg_rl))
#endif

/*
 * MB_PTR_ADV(): advance a pointer to the next character, taking care of
 * multi-byte characters if needed.
 * MB_PTR_BACK(): backup a pointer to the previous character, taking care of
 * multi-byte characters if needed.
 * MB_COPY_CHAR(f, t): copy one char from "f" to "t" and advance the pointers.
 * PTR2CHAR(): get character from pointer.
 */
// Advance multi-byte pointer, skip over composing chars.
#define MB_PTR_ADV(p)	    p += (*mb_ptr2len)(p)
// Advance multi-byte pointer, do not skip over composing chars.
#define MB_CPTR_ADV(p)	    p += enc_utf8 ? utf_ptr2len(p) : (*mb_ptr2len)(p)
// Backup multi-byte pointer. Only use with "p" > "s" !
#define MB_PTR_BACK(s, p)  p -= has_mbyte ? ((*mb_head_off)(s, (p) - 1) + 1) : 1
// get length of multi-byte char, not including composing chars
#define MB_CPTR2LEN(p)	    (enc_utf8 ? utf_ptr2len(p) : (*mb_ptr2len)(p))

#define MB_COPY_CHAR(f, t) செய் { எனில் (has_mbyte) mb_copy_char(&(f), &(t)); இல்லையெனில் *(t)++ = *(f)++; } போது (0)
#define MB_CHARLEN(p)	    (has_mbyte ? mb_charlen(p) : (முழு)STRLEN(p))
#define MB_CHAR2LEN(c)	    (has_mbyte ? mb_char2len(c) : 1)
#define PTR2CHAR(p)	    (has_mbyte ? mb_ptr2char(p) : (முழு)*(p))
#define MB_CHAR2BYTES(c, b) செய் { எனில் (has_mbyte) (b) += (*mb_char2bytes)((c), (b)); இல்லையெனில் *(b)++ = (c); } போது (0)

#ifdef FEAT_AUTOCHDIR
# define DO_AUTOCHDIR செய் { எனில் (p_acd) do_autochdir(); } போது (0)
#இல்லையெனில்
# define DO_AUTOCHDIR செய் { /**/ } போது (0)
#endif

#define RESET_BINDING(wp)  செய் { (wp)->w_p_scb = FALSE; (wp)->w_p_crb = FALSE; \
			    } போது (0)

#ifdef FEAT_DIFF
# define PLINES_NOFILL(x) plines_nofill(x)
# define PLINES_WIN_NOFILL(w, l, h) plines_win_nofill((w), (l), (h))
#இல்லையெனில்
# define PLINES_NOFILL(x) plines(x)
# define PLINES_WIN_NOFILL(w, l, h) plines_win((w), (l), (h))
#endif

#எனில் defined(FEAT_JOB_CHANNEL) || defined(FEAT_CLIENTSERVER)
# define MESSAGE_QUEUE
#endif

#அந்தர்பாவ <float.h>
#எனில் defined(HAVE_MATH_H)
  // for isnan() and isinf()
# include <math.h>
#endif

#எனில் defined(FEAT_EVAL)
# ifdef USING_FLOAT_STUFF
#  ifdef MSWIN
#   ifndef isnan
#    define isnan(x) _isnan(x)
     நிலையான __inline முழு isinf(இரட்டை x)
	{ திரும்பு !_finite(x) && !_isnan(x); }
#   endif
#  இல்லையெனில்
#   ifndef HAVE_ISNAN
     நிலையான உள்வரி முழு isnan(இரட்டை x)
	{ திரும்பு x != x; }
#   endif
#   ifndef HAVE_ISINF
     நிலையான உள்வரி முழு isinf(இரட்டை x)
	{ திரும்பு !isnan(x) && isnan(x - x); }
#   endif
#  endif
#  எனில் !defined(INFINITY)
#   எனில் defined(DBL_MAX)
#    ifdef VMS
#     define INFINITY DBL_MAX
#    இல்லையெனில்
#     define INFINITY (DBL_MAX+DBL_MAX)
#    endif
#   இல்லையெனில்
#    define INFINITY (1.0 / 0.0)
#   endif
#  endif
#  எனில் !defined(NAN)
#   define NAN (INFINITY-INFINITY)
#  endif
#  எனில் !defined(DBL_EPSILON)
#   define DBL_EPSILON 2.2204460492503131e-16
#  endif
# endif
#endif

#ifdef FEAT_EVAL
# define FUNCARG(fp, j)	((char_u **)(fp->uf_args.ga_data))[j]
#endif

/*
 * In a hashtab item "hi_key" points to "di_key" in a dictitem.
 * This avoids adding a pointer to the hashtab item.
 * DI2HIKEY() converts a dictitem pointer to a hashitem key pointer.
 * HIKEY2DI() converts a hashitem key pointer to a dictitem pointer.
 * HI2DI() converts a hashitem pointer to a dictitem pointer.
 */
#define DI2HIKEY(di) ((di)->di_சாவி)
#define HIKEY2DI(p)  ((dictitem_T *)((p) - offsetof(dictitem_T, di_சாவி)))
#define HI2DI(hi)     HIKEY2DI((hi)->hi_சாவி)

/*
 * Flush control functions.
 */
#ifdef FEAT_GUI
# define mch_enable_flush()	gui_enable_flush()
# define mch_disable_flush()	gui_disable_flush()
#இல்லையெனில்
# define mch_enable_flush()
# define mch_disable_flush()
#endif

/*
 * Like vim_free(), and also set the pointer to NULL.
 */
#define VIM_CLEAR(p) \
    செய் { \
	vim_free(p); \
	(p) = வெற்று; \
    } போது (0)

/*
 * Free a string and set it's pointer to NULL and length to 0
 */
#define VIM_CLEAR_STRING(s) \
    செய் { \
	VIM_CLEAR(s.string); \
	s.length = 0; \
    } போது (0)

#define STR_LITERAL_INIT(s) \
    {(char_u *)(s), STRLEN_LITERAL(s)}

#define STR_LITERAL_SET(str, s) \
    செய் { \
	(str).string = (char_u *)(s); \
	(str).length = STRLEN_LITERAL(s); \
    } போது (0)

// Whether a command index indicates a user command.
#define IS_USER_CMDIDX(idx) ((முழு)(idx) < 0)

// Give an error in curwin is a popup window and evaluate to TRUE.
#ifdef FEAT_PROP_POPUP
# define WIN_IS_POPUP(wp) ((wp)->w_popup_flags != 0)
# define ERROR_IF_POPUP_WINDOW error_if_popup_window(FALSE)
# define ERROR_IF_ANY_POPUP_WINDOW error_if_popup_window(TRUE)
#இல்லையெனில்
# define WIN_IS_POPUP(wp) 0
# define ERROR_IF_POPUP_WINDOW 0
# define ERROR_IF_ANY_POPUP_WINDOW 0
#endif
#எனில் defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
# define ERROR_IF_TERM_POPUP_WINDOW error_if_term_popup_window()
#இல்லையெனில்
# define ERROR_IF_TERM_POPUP_WINDOW 0
#endif


#ifdef ABORT_ON_INTERNAL_ERROR
# define ESTACK_CHECK_DECLARATION முழு estack_len_before
# define ESTACK_CHECK_SETUP செய் { estack_len_before = exestack.ga_len; } போது (0)
# define ESTACK_CHECK_NOW \
    செய் { \
	எனில் (estack_len_before != exestack.ga_len) \
	    siemsg("Exestack length expected: %d, actual: %d", estack_len_before, exestack.ga_len); \
    } போது (0)
# define CHECK_CURBUF \
    செய் { \
	எனில் (curwin != வெற்று && curwin->w_buffer != curbuf) \
	    iemsg("curbuf != curwin->w_buffer"); \
    } போது (0)
#இல்லையெனில்
# define ESTACK_CHECK_DECLARATION செய் { /**/ } போது (0)
# define ESTACK_CHECK_SETUP செய் { /**/ } போது (0)
# define ESTACK_CHECK_NOW செய் { /**/ } போது (0)
# define CHECK_CURBUF செய் { /**/ } போது (0)
#endif

// Inline the condition for performance.
#define CHECK_LIST_MATERIALIZE(l) \
    செய் { \
	எனில் ((l)->lv_first == &range_list_item) \
	    range_list_materialize(l); \
    } போது (0)

// Inlined version of ga_grow() with optimized condition that it fails.
#define GA_GROW_FAILS(gap, n) unlikely((((gap)->ga_maxlen - (gap)->ga_len < (n)) ? ga_grow_inner((gap), (n)) : OK) == FAIL)
// Inlined version of ga_grow() with optimized condition that it succeeds.
#define GA_GROW_OK(gap, n) likely((((gap)->ga_maxlen - (gap)->ga_len < (n)) ? ga_grow_inner((gap), (n)) : OK) == OK)
#define GA_CONCAT_LITERAL(gap, s) ga_concat_len((gap), (char_u *)(s), STRLEN_LITERAL(s))

#ifndef MIN
# define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
# define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

// Length of the array.
#define ARRAY_LENGTH(a) (அளவு(a) / அளவு((a)[0]))

#ifdef FEAT_MENU
# define FOR_ALL_MENUS(m) \
    க்கு ((m) = வேர்_menu; (m) != வெற்று; (m) = (m)->next)
# define FOR_ALL_CHILD_MENUS(p, c) \
    க்கு ((c) = (p)->children; (c) != வெற்று; (c) = (c)->next)
#endif

#define FOR_ALL_WINDOWS(wp) \
    க்கு ((wp) = firstwin; (wp) != வெற்று; (wp) = (wp)->w_next)
#define FOR_ALL_FRAMES(frp, first_frame) \
    க்கு ((frp) = first_frame; (frp) != வெற்று; (frp) = (frp)->fr_next)
#define FOR_ALL_TABPAGES(tp) \
    க்கு ((tp) = first_tabpage; (tp) != வெற்று; (tp) = (tp)->tp_next)
#define FOR_ALL_WINDOWS_IN_TAB(tp, wp) \
    க்கு ((wp) = ((tp) == வெற்று || (tp) == curtab) \
	    ? firstwin : (tp)->tp_firstwin; (wp); (wp) = (wp)->w_next)
/*
 * When using this macro "break" only breaks out of the inner loop. Use "goto"
 * to break out of the tabpage loop.
 */
#define FOR_ALL_TAB_WINDOWS(tp, wp) \
    க்கு ((tp) = first_tabpage; (tp) != வெற்று; (tp) = (tp)->tp_next) \
	க்கு ((wp) = ((tp) == curtab) \
		? firstwin : (tp)->tp_firstwin; (wp); (wp) = (wp)->w_next)

#define FOR_ALL_POPUPWINS(wp) \
    க்கு ((wp) = first_popupwin; (wp) != வெற்று; (wp) = (wp)->w_next)
#define FOR_ALL_POPUPWINS_IN_TAB(tp, wp) \
    க்கு ((wp) = (tp)->tp_first_popupwin; (wp) != வெற்று; (wp) = (wp)->w_next)

#define FOR_ALL_BUFFERS(buf) \
    க்கு ((buf) = firstbuf; (buf) != வெற்று; (buf) = (buf)->b_next)

#define FOR_ALL_BUF_WININFO(buf, wip) \
    க்கு ((wip) = (buf)->b_wininfo; (wip) != வெற்று; (wip) = (wip)->wi_next)

// Iterate through all the signs placed in a buffer
#define FOR_ALL_SIGNS_IN_BUF(buf, sign) \
    க்கு ((sign) = (buf)->b_signlist; (sign) != வெற்று; (sign) = (sign)->se_next)

#ifdef FEAT_SPELL
# define FOR_ALL_SPELL_LANGS(slang) \
    க்கு ((slang) = first_lang; (slang) != வெற்று; (slang) = (slang)->sl_next)
#endif

// Iterate over all the items in a List
#define FOR_ALL_LIST_பொருட்கள்(l, li) \
    க்கு ((li) = (l) == வெற்று ? வெற்று : (l)->lv_first; (li) != வெற்று; (li) = (li)->li_next)

// Iterate over all the items in a hash table
#define FOR_ALL_HASHTAB_பொருட்கள்(ht, hi, todo) \
    க்கு ((hi) = (ht)->ht_array; (todo) > 0; ++(hi))

#define TUPLE_LEN(t)	    (t->tv_பொருட்கள்.ga_len)
#define TUPLE_ITEM(t, i) \
	    (((typval_T *)t->tv_பொருட்கள்.ga_data) + i)

// Avoid open() on a non-existing file on MS-Windows
#எனில் !defined(MSWIN) && !defined(PROTO)
# define mch_isdir_or_missing(f) mch_isdir(f)
#endif
