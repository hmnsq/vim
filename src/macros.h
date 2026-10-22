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
#నిర్వచించు LT_POS(a, b) (((a).lnum != (b).lnum) \
		   ? (a).lnum < (b).lnum \
		   : (a).నిలువు != (b).నిలువు \
		       ? (a).నిలువు < (b).నిలువు \
		       : (a).coladd < (b).coladd)
#నిర్వచించు LT_POSP(a, b) (((a)->lnum != (b)->lnum) \
		   ? (a)->lnum < (b)->lnum \
		   : (a)->నిలువు != (b)->నిలువు \
		       ? (a)->నిలువు < (b)->నిలువు \
		       : (a)->coladd < (b)->coladd)
#నిర్వచించు EQUAL_POS(a, b) (((a).lnum == (b).lnum) && ((a).నిలువు == (b).నిలువు) && ((a).coladd == (b).coladd))
#నిర్వచించు CLEAR_POS(a) చేయి {(a)->lnum = 0; (a)->నిలువు = 0; (a)->coladd = 0;} అయితే (0)
#నిర్వచించు ఖాళీలు_POS(a) ((a).lnum == 0 && (a).నిలువు == 0 && (a).coladd == 0)

#నిర్వచించు LTOREQ_POS(a, b) (LT_POS(a, b) || EQUAL_POS(a, b))

/*
 * VIM_ISWHITE() differs from isspace() because it doesn't include <CR> and
 * <LF> and the like.
 */
#నిర్వచించు VIM_ISWHITE(x)		((x) == ' ' || (x) == '\t')
#నిర్వచించు IS_WHITE_OR_NUL(x)	((x) == ' ' || (x) == '\t' || (x) == NUL)
#నిర్వచించు IS_WHITE_NL_OR_NUL(x)	((x) == ' ' || (x) == '\t' || (x) == '\n' || (x) == NUL)

/*
 * LINEEMPTY() - return TRUE if the line is empty
 */
#నిర్వచించు LINEEMPTY(p) (*ml_get(p) == NUL)

/*
 * BUFEMPTY() - return TRUE if the current buffer is empty
 */
#నిర్వచించు BUFEMPTY() (curbuf->b_ml.ml_పంక్తి_లెక్క == 1 && *ml_get((linenr_T)1) == NUL)

// The is*() and to*() functions declared in <ctype.h> have
// undefined behavior for values other than EOF outside the range of
// unsigned char.  If plain char is signed, a call with a negative
// value has undefined behavior.  These macros cast the argument to
// unsigned char.  (Most implementations behave more or less sanely
// with negative values, and most character values in practice are
// positive, but we want to avoid undefined behavior anyway.)
#నిర్వచించు సురక్షిత_isalnum(c)  (అక్షరమా_అంకెనా ((గుర్తులేని అక్షరం)(c)))
#నిర్వచించు సురక్షిత_isalpha(c)  (అక్షరమా ((గుర్తులేని అక్షరం)(c)))
#నిర్వచించు సురక్షిత_isblank(c)  (isblank ((గుర్తులేని అక్షరం)(c)))
#నిర్వచించు సురక్షిత_iscntrl(c)  (iscntrl ((గుర్తులేని అక్షరం)(c)))
#నిర్వచించు సురక్షిత_isdigit(c)  (అంకెనా ((గుర్తులేని అక్షరం)(c)))
#నిర్వచించు సురక్షిత_isgraph(c)  (isgraph ((గుర్తులేని అక్షరం)(c)))
#నిర్వచించు సురక్షిత_islower(c)  (islower ((గుర్తులేని అక్షరం)(c)))
#నిర్వచించు సురక్షిత_isprint(c)  (isprint ((గుర్తులేని అక్షరం)(c)))
#నిర్వచించు సురక్షిత_ispunct(c)  (ispunct ((గుర్తులేని అక్షరం)(c)))
#నిర్వచించు సురక్షిత_isspace(c)  (ఖాళీనా ((గుర్తులేని అక్షరం)(c)))
#నిర్వచించు సురక్షిత_isupper(c)  (isupper ((గుర్తులేని అక్షరం)(c)))
#నిర్వచించు సురక్షిత_isxdigit(c) (isxdigit((గుర్తులేని అక్షరం)(c)))
#నిర్వచించు సురక్షిత_tolower(c)  (tolower ((గుర్తులేని అక్షరం)(c)))
#నిర్వచించు సురక్షిత_toupper(c)  (పెద్ద_అక్షరం ((గుర్తులేని అక్షరం)(c)))

/*
 * toupper() and tolower() that use the current locale.
 * On some systems toupper()/tolower() only work on lower/uppercase
 * characters, first use islower() or isupper() then.
 * Careful: Only call TOUPPER_LOC() and TOLOWER_LOC() with a character in the
 * range 0 - 255.  toupper()/tolower() on some systems can't handle others.
 * Note: It is often better to use MB_TOLOWER() and MB_TOUPPER(), because many
 * toupper() and tolower() implementations only work for ASCII.
 */
#నిర్వచితమైతే MSWIN
# define TOUPPER_LOC(c)	toupper_tab[(c) & 255]
# define TOLOWER_LOC(c)	tolower_tab[(c) & 255]
#లేకపోతే
# ifdef BROKEN_TOUPPER
#  define TOUPPER_LOC(c)	(సురక్షిత_islower(c) ? సురక్షిత_toupper(c) : (c))
#  define TOLOWER_LOC(c)	(సురక్షిత_isupper(c) ? సురక్షిత_tolower(c) : (c))
# లేకపోతే
#  define TOUPPER_LOC		సురక్షిత_toupper
#  define TOLOWER_LOC		సురక్షిత_tolower
# endif
#అయితే_ముగింపు

// toupper() and tolower() for ASCII only and ignore the current locale.
#నిర్వచించు TOUPPER_ASC(c)	(((c) < 'a' || (c) > 'z') ? (c) : (c) - ('a' - 'A'))
#నిర్వచించు TOLOWER_ASC(c)	(((c) < 'A' || (c) > 'Z') ? (c) : (c) + ('a' - 'A'))

/*
 * MB_ISLOWER() and MB_ISUPPER() are to be used on multi-byte characters.  But
 * don't use them for negative values!
 */
#నిర్వచించు MB_ISLOWER(c)	vim_islower(c)
#నిర్వచించు MB_ISUPPER(c)	vim_isupper(c)
#నిర్వచించు MB_TOLOWER(c)	vim_tolower(c)
#నిర్వచించు MB_TOUPPER(c)	vim_toupper(c)
#నిర్వచించు MB_CASEFOLD(c)	(enc_utf8 ? utf_fold(c) : MB_TOLOWER(c))

// Use our own isdigit() replacement, because on MS-Windows isdigit() returns
// non-zero for superscript 1.  Also avoids that isdigit() crashes for numbers
// below 0 and above 255.
#నిర్వచించు VIM_ISDIGIT(c) ((గుర్తులేని)(c) - '0' < 10)

// Like isalpha() but reject non-ASCII characters.  Can't be used with a
// special key (negative value).
#నిర్వచించు ASCII_ISLOWER(c) ((గుర్తులేని)(c) - 'a' < 26)
#నిర్వచించు ASCII_ISUPPER(c) ((గుర్తులేని)(c) - 'A' < 26)
#నిర్వచించు ASCII_ISALPHA(c) (ASCII_ISUPPER(c) || ASCII_ISLOWER(c))
#నిర్వచించు ASCII_ISALNUM(c) (ASCII_ISALPHA(c) || VIM_ISDIGIT(c))

// Returns empty string if it is NULL.
#నిర్వచించు ఖాళీలు_IF_NULL(x) ((x) ? (x) : (char_u *)"")

#నిర్వచితమైతే FEAT_LANGMAP
/*
 * Adjust chars in a language according to 'langmap' option.
 * NOTE that there is no noticeable overhead if 'langmap' is not set.
 * When set the overhead for characters < 256 is small.
 * Don't apply 'langmap' if the character comes from the Stuff buffer or from
 * a mapping and the langnoremap option was set.
 * The do-while is just to ignore a ';' after the macro.
 */
# define LANGMAP_ADJUST(c, condition) \
    చేయి { \
	ఒకవేళ (*p_langmap \
		&& (condition) \
		&& (p_lrm || (!p_lrm && కీలకం_Typed)) \
		&& !కీలకం_Stuffed \
		&& (c) >= 0) \
	{ \
	    ఒకవేళ ((c) < 256) \
		c = langmap_mapchar[c]; \
	    లేకపోతే \
		c = langmap_adjust_mb(c); \
	} \
    } అయితే (0)
#లేకపోతే
# define LANGMAP_ADJUST(c, condition) // nop
#అయితే_ముగింపు

/*
 * VIM_ISBREAK() is used very often if 'linebreak' is set, use a macro to make
 * it work fast.  Only works for single byte characters!
 */
#నిర్వచించు VIM_ISBREAK(c) ((c) < 256 && breakat_flags[(char_u)(c)])

/*
 * On VMS file names are different and require a translation.
 * On the Mac open() has only two arguments.
 */
#నిర్వచితమైతే VMS
# define mch_access(n, p)	access(vms_fixfilename(n), (p))
				// see mch_open() comment
# define mch_fopen(n, p)	fopen(vms_fixfilename(n), (p))
# define mch_fstat(n, p)	fstat((n), (p))
# undef HAVE_LSTAT		// VMS does not have lstat()
# define mch_stat(n, p)		stat(vms_fixfilename(n), (p))
#లేకపోతే
# ఒకవేళ !defined(MSWIN) && !defined(PROTO)
#  define mch_access(n, p)	access((n), (p))
# endif

// Use 64-bit fstat function on MS-Windows.
// NOTE: This condition is the same as for the stat_T type.
# ifdef MSWIN
#  define mch_fstat(n, p)	_fstat64((n), (p))
# లేకపోతే
#  define mch_fstat(n, p)	fstat((n), (p))
# endif

# ifdef MSWIN	// has its own mch_stat() function
#  define mch_stat(n, p)	vim_stat((n), (p))
# లేకపోతే
#  ifdef STAT_IGNORES_SLASH
#   define mch_stat(n, p)	vim_stat((n), (p))
#  లేకపోతే
#   define mch_stat(n, p)	stat((n), (p))
#  endif
# endif
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_LSTAT
# define mch_lstat(n, p)	lstat((n), (p))
#లేకపోతే
# ifdef MSWIN
#  define mch_lstat(n, p)	vim_lstat((n), (p))
# లేకపోతే
#  define mch_lstat(n, p)	mch_stat((n), (p))
# endif
#అయితే_ముగింపు

#నిర్వచితమైతే VMS
/*
 * It is possible to force some record format with:
 * #  define mch_open(n, m, p) open(vms_fixfilename(n), (m), (p)), "rat=cr", "rfm=stmlf", "mrs=0")
 * but it is not recommended, because it can destroy indexes etc.
 */
# define mch_open(n, m, p)	open(vms_fixfilename(n), (m), (p))
#అయితే_ముగింపు

// mch_open_rw(): invoke mch_open() with third argument for user R/W.
#ఒకవేళ defined(UNIX) || defined(VMS)  // open in rw------- mode
# define mch_open_rw(n, f)	mch_open((n), (f), (mode_t)0600)
#లేకపోతే
# ఒకవేళ defined(MSWIN)  // open read/write
#  define mch_open_rw(n, f)	mch_open((n), (f), S_IREAD | S_IWRITE)
# లేకపోతే
#  define mch_open_rw(n, f)	mch_open((n), (f), 0)
# endif
#అయితే_ముగింపు

#నిర్వచితమైతే STARTUPTIME
# define సమయం_సందేశం(s) చేయి { ఒకవేళ (సమయం_fd != ఖాళీ) సమయం_సందేశం_2(s, ఖాళీ); } అయితే (0)
#లేకపోతే
# define సమయం_సందేశం(s) చేయి { /**/ } అయితే (0)
#అయితే_ముగింపు

#నిర్వచించు REPLACE_సాధారణ(s) (((s) & REPLACE_FLAG) && !((s) & VREPLACE_FLAG))

#నిర్వచితమైతే FEAT_ARABIC
# define ARABIC_CHAR(ch)	    (((ch) & 0xFF00) == 0x0600)
# define UTF_COMPOSINGLIKE(p1, p2)  utf_composinglike((p1), (p2))
#లేకపోతే
# define UTF_COMPOSINGLIKE(p1, p2)  utf_iscomposing(utf_ptr2char(p2))
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_RIGHTLEFT
    // Whether to draw the vertical bar on the right side of the cell.
# define CURSOR_BAR_కుడి (curwin->w_p_rl && (!(State & బహుళకం_CMDLINE) || cmdmsg_rl))
#అయితే_ముగింపు

/*
 * MB_PTR_ADV(): advance a pointer to the next character, taking care of
 * multi-byte characters if needed.
 * MB_PTR_BACK(): backup a pointer to the previous character, taking care of
 * multi-byte characters if needed.
 * MB_COPY_CHAR(f, t): copy one char from "f" to "t" and advance the pointers.
 * PTR2CHAR(): get character from pointer.
 */
// Advance multi-byte pointer, skip over composing chars.
#నిర్వచించు MB_PTR_ADV(p)	    p += (*mb_ptr2len)(p)
// Advance multi-byte pointer, do not skip over composing chars.
#నిర్వచించు MB_CPTR_ADV(p)	    p += enc_utf8 ? utf_ptr2len(p) : (*mb_ptr2len)(p)
// Backup multi-byte pointer. Only use with "p" > "s" !
#నిర్వచించు MB_PTR_వెనుక(s, p)  p -= has_mbyte ? ((*mb_తల_off)(s, (p) - 1) + 1) : 1
// get length of multi-byte char, not including composing chars
#నిర్వచించు MB_CPTR2LEN(p)	    (enc_utf8 ? utf_ptr2len(p) : (*mb_ptr2len)(p))

#నిర్వచించు MB_COPY_CHAR(f, t) చేయి { ఒకవేళ (has_mbyte) mb_copy_char(&(f), &(t)); లేకపోతే *(t)++ = *(f)++; } అయితే (0)
#నిర్వచించు MB_CHARLEN(p)	    (has_mbyte ? mb_charlen(p) : (పూర్ణాంకం)STRLEN(p))
#నిర్వచించు MB_CHAR2LEN(c)	    (has_mbyte ? mb_char2len(c) : 1)
#నిర్వచించు PTR2CHAR(p)	    (has_mbyte ? mb_ptr2char(p) : (పూర్ణాంకం)*(p))
#నిర్వచించు MB_CHAR2_బైట్లు(c, b) చేయి { ఒకవేళ (has_mbyte) (b) += (*mb_char2bytes)((c), (b)); లేకపోతే *(b)++ = (c); } అయితే (0)

#నిర్వచితమైతే FEAT_AUTOCHDIR
# define DO_AUTOCHDIR చేయి { ఒకవేళ (p_acd) do_autochdir(); } అయితే (0)
#లేకపోతే
# define DO_AUTOCHDIR చేయి { /**/ } అయితే (0)
#అయితే_ముగింపు

#నిర్వచించు RESET_BINDING(wp)  చేయి { (wp)->w_p_scb = FALSE; (wp)->w_p_crb = FALSE; \
			    } అయితే (0)

#నిర్వచితమైతే FEAT_DIFF
# define PLINES_NOFILL(x) plines_nofill(x)
# define PLINES_WIN_NOFILL(w, l, h) plines_win_nofill((w), (l), (h))
#లేకపోతే
# define PLINES_NOFILL(x) plines(x)
# define PLINES_WIN_NOFILL(w, l, h) plines_win((w), (l), (h))
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_పని_CHANNEL) || defined(FEAT_CLIENTSERVER)
# define సందేశం_వరుస
#అయితే_ముగింపు

#అన్తర్భావ <float.h>
#ఒకవేళ defined(HAVE_MATH_H)
  // for isnan() and isinf()
# include <math.h>
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL)
# ifdef USING_FLOAT_STUFF
#  ifdef MSWIN
#   ifndef isnan
#    define isnan(x) _isnan(x)
     స్థిర __inline పూర్ణాంకం isinf(ద్విగుణ x)
	{ తిరిగి !_finite(x) && !_isnan(x); }
#   endif
#  లేకపోతే
#   ifndef HAVE_ISNAN
     స్థిర అంతఃవరుస పూర్ణాంకం isnan(ద్విగుణ x)
	{ తిరిగి x != x; }
#   endif
#   ifndef HAVE_ISINF
     స్థిర అంతఃవరుస పూర్ణాంకం isinf(ద్విగుణ x)
	{ తిరిగి !isnan(x) && isnan(x - x); }
#   endif
#  endif
#  ఒకవేళ !defined(INFINITY)
#   ఒకవేళ defined(DBL_MAX)
#    ifdef VMS
#     define INFINITY DBL_MAX
#    లేకపోతే
#     define INFINITY (DBL_MAX+DBL_MAX)
#    endif
#   లేకపోతే
#    define INFINITY (1.0 / 0.0)
#   endif
#  endif
#  ఒకవేళ !defined(NAN)
#   define NAN (INFINITY-INFINITY)
#  endif
#  ఒకవేళ !defined(DBL_EPSILON)
#   define DBL_EPSILON 2.2204460492503131e-16
#  endif
# endif
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_EVAL
# define FUNCARG(fp, j)	((char_u **)(fp->uf_వాదనలు.ga_దత్తాంశం))[j]
#అయితే_ముగింపు

/*
 * In a hashtab item "hi_key" points to "di_key" in a dictitem.
 * This avoids adding a pointer to the hashtab item.
 * DI2HIKEY() converts a dictitem pointer to a hashitem key pointer.
 * HIKEY2DI() converts a hashitem key pointer to a dictitem pointer.
 * HI2DI() converts a hashitem pointer to a dictitem pointer.
 */
#నిర్వచించు DI2HIKEY(di) ((di)->di_కీలకం)
#నిర్వచించు HIKEY2DI(p)  ((dictitem_T *)((p) - offsetof(dictitem_T, di_కీలకం)))
#నిర్వచించు HI2DI(hi)     HIKEY2DI((hi)->hi_కీలకం)

/*
 * Flush control functions.
 */
#నిర్వచితమైతే FEAT_GUI
# define mch_enable_flush()	gui_enable_flush()
# define mch_disable_flush()	gui_disable_flush()
#లేకపోతే
# define mch_enable_flush()
# define mch_disable_flush()
#అయితే_ముగింపు

/*
 * Like vim_free(), and also set the pointer to NULL.
 */
#నిర్వచించు VIM_CLEAR(p) \
    చేయి { \
	vim_free(p); \
	(p) = ఖాళీ; \
    } అయితే (0)

/*
 * Free a string and set it's pointer to NULL and length to 0
 */
#నిర్వచించు VIM_CLEAR_STRING(s) \
    చేయి { \
	VIM_CLEAR(s.string); \
	s.నిడివి = 0; \
    } అయితే (0)

#నిర్వచించు STR_LITERAL_INIT(s) \
    {(char_u *)(s), STRLEN_LITERAL(s)}

#నిర్వచించు STR_LITERAL_సమితి(str, s) \
    చేయి { \
	(str).string = (char_u *)(s); \
	(str).నిడివి = STRLEN_LITERAL(s); \
    } అయితే (0)

// Whether a command index indicates a user command.
#నిర్వచించు IS_USER_CMDIDX(idx) ((పూర్ణాంకం)(idx) < 0)

// Give an error in curwin is a popup window and evaluate to TRUE.
#నిర్వచితమైతే FEAT_PROP_POPUP
# define WIN_IS_POPUP(wp) ((wp)->w_popup_flags != 0)
# define ERROR_IF_POPUP_కిటికీ error_if_popup_కిటికీ(FALSE)
# define ERROR_IF_ANY_POPUP_కిటికీ error_if_popup_కిటికీ(TRUE)
#లేకపోతే
# define WIN_IS_POPUP(wp) 0
# define ERROR_IF_POPUP_కిటికీ 0
# define ERROR_IF_ANY_POPUP_కిటికీ 0
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
# define ERROR_IF_పదం_POPUP_కిటికీ error_if_పదం_popup_కిటికీ()
#లేకపోతే
# define ERROR_IF_పదం_POPUP_కిటికీ 0
#అయితే_ముగింపు


#నిర్వచితమైతే ABORT_ON_అంతర్గత_ERROR
# define ESTACK_CHECK_DECLARATION పూర్ణాంకం estack_len_before
# define ESTACK_CHECK_అమరిక చేయి { estack_len_before = exestack.ga_len; } అయితే (0)
# define ESTACK_CHECK_NOW \
    చేయి { \
	ఒకవేళ (estack_len_before != exestack.ga_len) \
	    siemsg("Exestack length expected: %d, actual: %d", estack_len_before, exestack.ga_len); \
    } అయితే (0)
# define CHECK_CURBUF \
    చేయి { \
	ఒకవేళ (curwin != ఖాళీ && curwin->w_బఫరు != curbuf) \
	    iemsg("curbuf != curwin->w_buffer"); \
    } అయితే (0)
#లేకపోతే
# define ESTACK_CHECK_DECLARATION చేయి { /**/ } అయితే (0)
# define ESTACK_CHECK_అమరిక చేయి { /**/ } అయితే (0)
# define ESTACK_CHECK_NOW చేయి { /**/ } అయితే (0)
# define CHECK_CURBUF చేయి { /**/ } అయితే (0)
#అయితే_ముగింపు

// Inline the condition for performance.
#నిర్వచించు CHECK_LIST_MATERIALIZE(l) \
    చేయి { \
	ఒకవేళ ((l)->lv_మొదటి == &range_list_అంశం) \
	    range_list_materialize(l); \
    } అయితే (0)

// Inlined version of ga_grow() with optimized condition that it fails.
#నిర్వచించు GA_GROW_FAILS(gap, n) unlikely((((gap)->ga_maxlen - (gap)->ga_len < (n)) ? ga_grow_inner((gap), (n)) : OK) == FAIL)
// Inlined version of ga_grow() with optimized condition that it succeeds.
#నిర్వచించు GA_GROW_OK(gap, n) likely((((gap)->ga_maxlen - (gap)->ga_len < (n)) ? ga_grow_inner((gap), (n)) : OK) == OK)
#నిర్వచించు GA_CONCAT_LITERAL(gap, s) ga_concat_len((gap), (char_u *)(s), STRLEN_LITERAL(s))

#నిర్వచితం_కాకపోతే MIN
# define MIN(a, b) ((a) < (b) ? (a) : (b))
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే MAX
# define MAX(a, b) ((a) > (b) ? (a) : (b))
#అయితే_ముగింపు

// Length of the array.
#నిర్వచించు ARRAY_నిడివి(a) (పరిమాణం(a) / పరిమాణం((a)[0]))

#నిర్వచితమైతే FEAT_MENU
# define FOR_ALL_MENUS(m) \
    కొరకు ((m) = మూలం_menu; (m) != ఖాళీ; (m) = (m)->తదుపరి)
# define FOR_ALL_బిడ్డ_MENUS(p, c) \
    కొరకు ((c) = (p)->children; (c) != ఖాళీ; (c) = (c)->తదుపరి)
#అయితే_ముగింపు

#నిర్వచించు FOR_ALL_WINDOWS(wp) \
    కొరకు ((wp) = firstwin; (wp) != ఖాళీ; (wp) = (wp)->w_తదుపరి)
#నిర్వచించు FOR_ALL_చట్రాలు(frp, మొదటి_చట్రం) \
    కొరకు ((frp) = మొదటి_చట్రం; (frp) != ఖాళీ; (frp) = (frp)->fr_తదుపరి)
#నిర్వచించు FOR_ALL_TABPAGES(tp) \
    కొరకు ((tp) = మొదటి_tabpage; (tp) != ఖాళీ; (tp) = (tp)->tp_తదుపరి)
#నిర్వచించు FOR_ALL_WINDOWS_లోపల_TAB(tp, wp) \
    కొరకు ((wp) = ((tp) == ఖాళీ || (tp) == curtab) \
	    ? firstwin : (tp)->tp_firstwin; (wp); (wp) = (wp)->w_తదుపరి)
/*
 * When using this macro "break" only breaks out of the inner loop. Use "goto"
 * to break out of the tabpage loop.
 */
#నిర్వచించు FOR_ALL_TAB_WINDOWS(tp, wp) \
    కొరకు ((tp) = మొదటి_tabpage; (tp) != ఖాళీ; (tp) = (tp)->tp_తదుపరి) \
	కొరకు ((wp) = ((tp) == curtab) \
		? firstwin : (tp)->tp_firstwin; (wp); (wp) = (wp)->w_తదుపరి)

#నిర్వచించు FOR_ALL_POPUPWINS(wp) \
    కొరకు ((wp) = మొదటి_popupwin; (wp) != ఖాళీ; (wp) = (wp)->w_తదుపరి)
#నిర్వచించు FOR_ALL_POPUPWINS_లోపల_TAB(tp, wp) \
    కొరకు ((wp) = (tp)->tp_మొదటి_popupwin; (wp) != ఖాళీ; (wp) = (wp)->w_తదుపరి)

#నిర్వచించు FOR_ALL_BUFFERS(buf) \
    కొరకు ((buf) = firstbuf; (buf) != ఖాళీ; (buf) = (buf)->b_తదుపరి)

#నిర్వచించు FOR_ALL_BUF_WININFO(buf, wip) \
    కొరకు ((wip) = (buf)->b_wininfo; (wip) != ఖాళీ; (wip) = (wip)->wi_తదుపరి)

// Iterate through all the signs placed in a buffer
#నిర్వచించు FOR_ALL_SIGNS_లోపల_BUF(buf, sign) \
    కొరకు ((sign) = (buf)->b_signlist; (sign) != ఖాళీ; (sign) = (sign)->se_తదుపరి)

#నిర్వచితమైతే FEAT_SPELL
# define FOR_ALL_SPELL_LANGS(slang) \
    కొరకు ((slang) = మొదటి_lang; (slang) != ఖాళీ; (slang) = (slang)->sl_తదుపరి)
#అయితే_ముగింపు

// Iterate over all the items in a List
#నిర్వచించు FOR_ALL_LIST_వస్తువులు(l, li) \
    కొరకు ((li) = (l) == ఖాళీ ? ఖాళీ : (l)->lv_మొదటి; (li) != ఖాళీ; (li) = (li)->li_తదుపరి)

// Iterate over all the items in a hash table
#నిర్వచించు FOR_ALL_HASHTAB_వస్తువులు(ht, hi, todo) \
    కొరకు ((hi) = (ht)->ht_array; (todo) > 0; ++(hi))

#నిర్వచించు TUPLE_LEN(t)	    (t->tv_వస్తువులు.ga_len)
#నిర్వచించు TUPLE_అంశం(t, i) \
	    (((typval_T *)t->tv_వస్తువులు.ga_దత్తాంశం) + i)

// Avoid open() on a non-existing file on MS-Windows
#ఒకవేళ !defined(MSWIN) && !defined(PROTO)
# define mch_isdir_or_missing(f) mch_isdir(f)
#అయితే_ముగింపు
