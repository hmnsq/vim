/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * Handling of regular expressions: vim_regcomp(), vim_regexec(), vim_regsub()
 */

// By default: do not create debugging logs or files related to regular
// expressions, even when compiling with -DDEBUG.
// Uncomment the second line to get the regexp debugging.
#تعریف_منسوخ DEBUG
// #define DEBUG

#شامل "vim.h"

#اگر_تعریف_شدہ DEBUG
// show/save debugging data when BT engine is used
# define BT_REGEXP_DUMP
// save the debugging data to a file instead of displaying it
# define BT_REGEXP_LOG
# define BT_REGEXP_DEBUG_LOG
# define BT_REGEXP_DEBUG_LOG_NAME	"bt_regexp_debug.log"
#اگر_ختم

#اگر_تعریف_شدہ FEAT_RELTIME
ساکن sig_atomic_t dummy_timeout_flag = 0;
ساکن متلون sig_atomic_t *timeout_flag = &dummy_timeout_flag;
#اگر_ختم

/*
 * Magic characters have a special meaning, they don't match literally.
 * Magic characters are negative.  This separates them from literal characters
 * (possibly multi-byte).  Only ASCII characters can be Magic.
 */
#تعریف_کرو Magic(x)	((صحیح)(x) - 256)
#تعریف_کرو un_Magic(x)	((x) + 256)
#تعریف_کرو is_Magic(x)	((x) < 0)

    ساکن صحیح
no_Magic(صحیح x)
{
    اگر (is_Magic(x))
	واپس un_Magic(x);
    واپس x;
}

    ساکن صحیح
toggle_Magic(صحیح x)
{
    اگر (is_Magic(x))
	واپس un_Magic(x);
    واپس Magic(x);
}

#اگر defined(FEAT_RELTIME)
ساکن صحیح timeout_nesting = 0;

/*
 * Start a timer that will cause the regexp to abort after "msec".
 * This doesn't work well recursively.  In case it happens anyway, the first
 * set timeout will prevail, nested ones are ignored.
 * The caller must make sure there is a matching disable_regexp_timeout() call!
 */
    خالی
init_regexp_timeout(طویل msec)
{
    اگر (timeout_nesting == 0)
	timeout_flag = آغاز_timeout(msec);
    ++timeout_nesting;
}

    خالی
disable_regexp_timeout(خالی)
{
    اگر (timeout_nesting == 0)
	iemsg("disable_regexp_timeout() called without active timer");
    ورنہ اگر (--timeout_nesting == 0)
    {
	stop_timeout();
	timeout_flag = &dummy_timeout_flag;
    }
}
#اگر_ختم

#اگر defined(FEAT_EVAL)
# ifdef FEAT_RELTIME
ساکن sig_atomic_t *saved_timeout_flag;
# endif

/*
 * Used at the debug prompt: disable the timeout so that expression evaluation
 * can used patterns.
 * Must be followed by calling restore_timeout_for_debugging().
 */
    خالی
save_timeout_for_debugging(خالی)
{
# ifdef FEAT_RELTIME
    saved_timeout_flag = (sig_atomic_t *)timeout_flag;
    timeout_flag = &dummy_timeout_flag;
# endif
}

    خالی
restore_timeout_for_debugging(خالی)
{
# ifdef FEAT_RELTIME
    timeout_flag = saved_timeout_flag;
# endif
}
#اگر_ختم

/*
 * The first byte of the BT regexp internal "program" is actually this magic
 * number; the start node begins in the second byte.  It's used to catch the
 * most severe mutilation of the program by the caller.
 */

#تعریف_کرو REGMAGIC	0234

/*
 * Utility definitions.
 */
#تعریف_کرو UCHARAT(p)	((صحیح)*(char_u *)(p))

// Used for an error (down from) vim_regcomp(): give the error message, set
// rc_did_emsg and return NULL
#تعریف_کرو EMSG_RET_NULL(m) واپس (emsg((m)), rc_did_emsg = TRUE, (خالی *)تہی)
#تعریف_کرو IEMSG_RET_NULL(m) واپس (iemsg((m)), rc_did_emsg = TRUE, (خالی *)تہی)
#تعریف_کرو EMSG_RET_FAIL(m) واپس (emsg((m)), rc_did_emsg = TRUE, FAIL)
#تعریف_کرو EMSG2_RET_NULL(m, c) واپس (semsg((ثابت حرف *)(m), (c) ? "" : "\\"), rc_did_emsg = TRUE, (خالی *)تہی)
#تعریف_کرو EMSG3_RET_NULL(m, c, a) واپس (semsg((ثابت حرف *)(m), (c) ? "" : "\\", (a)), rc_did_emsg = TRUE, (خالی *)تہی)
#تعریف_کرو EMSG2_RET_FAIL(m, c) واپس (semsg((ثابت حرف *)(m), (c) ? "" : "\\"), rc_did_emsg = TRUE, FAIL)
#تعریف_کرو EMSG_ONE_RET_NULL EMSG2_RET_NULL(_(e_invalid_شے_اندر_str_brackets), reg_magic == MAGIC_ALL)


#تعریف_کرو MAX_حد_بندی	(32767L << 16L)

#تعریف_کرو NOT_MULTI	0
#تعریف_کرو MULTI_ONE	1
#تعریف_کرو MULTI_MULT	2

// return values for regmatch()
#تعریف_کرو RA_FAIL		1	// something failed, abort
#تعریف_کرو RA_CONT		2	// continue in inner loop
#تعریف_کرو RA_BREAK	3	// break inner loop
#تعریف_کرو RA_مطابقت	4	// successful match
#تعریف_کرو RA_NOMATCH	5	// didn't match

/*
 * Return NOT_MULTI if c is not a "multi" operator.
 * Return MULTI_ONE if c is a single "multi" operator.
 * Return MULTI_MULT if c is a multi "multi" operator.
 */
    ساکن صحیح
re_multi_type(صحیح c)
{
    اگر (c == Magic('@') || c == Magic('=') || c == Magic('?'))
	واپس MULTI_ONE;
    اگر (c == Magic('*') || c == Magic('+') || c == Magic('{'))
	واپس MULTI_MULT;
    واپس NOT_MULTI;
}

ساکن char_u		*reg_پچھلا_sub = تہی;
ساکن size_t		reg_پچھلا_sublen = 0;

/*
 * REGEXP_INRANGE contains all characters which are always special in a []
 * range after '\'.
 * REGEXP_ABBR contains all characters which act as abbreviations after '\'.
 * These are:
 *  \n	- New line (NL).
 *  \r	- Carriage Return (CR).
 *  \t	- Tab (TAB).
 *  \e	- Escape (ESC).
 *  \b	- Backspace (Ctrl_H).
 *  \d  - Character code in decimal, eg \d123
 *  \o	- Character code in octal, eg \o80
 *  \x	- Character code in hex, eg \x4a
 *  \u	- Multibyte character code, eg \u20ac
 *  \U	- Long multibyte character code, eg \U12345678
 */
ساکن char_u REGEXP_INRANGE[] = "]^-n\\";
ساکن char_u REGEXP_ABBR[] = "nrtebdoxuU";

/*
 * Translate '\x' to its control character, except "\n", which is Magic.
 */
    ساکن صحیح
backslash_trans(صحیح c)
{
    انتخاب (c)
    {
	حالت 'r':   واپس CAR;
	حالت 't':   واپس TAB;
	حالت 'e':   واپس ESC;
	حالت 'b':   واپس BS;
    }
    واپس c;
}

شمار
{
    CLASS_ALNUM = 0,
    CLASS_ALPHA,
    CLASS_BLANK,
    CLASS_CNTRL,
    CLASS_DIGIT,
    CLASS_گراف,
    CLASS_زیریں,
    CLASS_PRINT,
    CLASS_PUNCT,
    CLASS_SPACE,
    CLASS_بالائی,
    CLASS_XDIGIT,
    CLASS_TAB,
    CLASS_RETURN,
    CLASS_BACKSPACE,
    CLASS_ESCAPE,
    CLASS_IDENT,
    CLASS_KEYWORD,
    CLASS_FNAME,
    CLASS_NONE = 99
};

/*
 * Check for a character class name "[:name:]".  "pp" points to the '['.
 * Returns one of the CLASS_ items. CLASS_NONE means that no item was
 * recognized.  Otherwise "pp" is advanced to after the item.
 */
    ساکن صحیح
get_char_class(char_u **pp)
{
    // must be sorted by the 'value' field because it is used by bsearch()!
    ساکن keyvalue_T char_class_tab[] =
    {
	KEYVALUE_اندراج(CLASS_ALNUM, "alnum:]"),
	KEYVALUE_اندراج(CLASS_ALPHA, "alpha:]"),
	KEYVALUE_اندراج(CLASS_BACKSPACE, "backspace:]"),
	KEYVALUE_اندراج(CLASS_BLANK, "blank:]"),
	KEYVALUE_اندراج(CLASS_CNTRL, "cntrl:]"),
	KEYVALUE_اندراج(CLASS_DIGIT, "digit:]"),
	KEYVALUE_اندراج(CLASS_ESCAPE, "escape:]"),
	KEYVALUE_اندراج(CLASS_FNAME, "fname:]"),
	KEYVALUE_اندراج(CLASS_گراف, "graph:]"),
	KEYVALUE_اندراج(CLASS_IDENT, "ident:]"),
	KEYVALUE_اندراج(CLASS_KEYWORD, "keyword:]"),
	KEYVALUE_اندراج(CLASS_زیریں, "lower:]"),
	KEYVALUE_اندراج(CLASS_PRINT, "print:]"),
	KEYVALUE_اندراج(CLASS_PUNCT, "punct:]"),
	KEYVALUE_اندراج(CLASS_RETURN, "return:]"),
	KEYVALUE_اندراج(CLASS_SPACE, "space:]"),
	KEYVALUE_اندراج(CLASS_TAB, "tab:]"),
	KEYVALUE_اندراج(CLASS_بالائی, "upper:]"),
	KEYVALUE_اندراج(CLASS_XDIGIT, "xdigit:]")
    };

    // check that the value of "pp" has a chance of matching
    اگر ((*pp)[1] == ':' && ASCII_ISLOWER((*pp)[2])
			&& ASCII_ISLOWER((*pp)[3]) && ASCII_ISLOWER((*pp)[4]))
    {
	keyvalue_T ہدف;
	keyvalue_T *اندراج_2;
	// this function can be called repeatedly with the same value for "pp"
	// so we cache the last found entry.
	ساکن keyvalue_T *آخری_اندراج = تہی;

	ہدف.کلید = 0;
	ہدف.قدر.string = *pp + 2;
	ہدف.قدر.لمبائی = 0;	// not used, see cmp_keyvalue_value_n()

	اگر (آخری_اندراج != تہی && cmp_keyvalue_قدر_n(&ہدف, آخری_اندراج) == 0)
	    اندراج_2 = آخری_اندراج;
	ورنہ
	    اندراج_2 = (keyvalue_T *)bsearch(&ہدف, &char_class_tab,
					ARRAY_لمبائی(char_class_tab),
					حجم(char_class_tab[0]), cmp_keyvalue_قدر_n);
	اگر (اندراج_2 != تہی)
	{
	    آخری_اندراج = اندراج_2;
	    *pp += اندراج_2->قدر.لمبائی + 2;
	    واپس اندراج_2->کلید;
	}
    }
    واپس CLASS_NONE;
}

/*
 * Specific version of character class functions.
 * Using a table to keep this fast.
 */
ساکن مختصر	class_tab[256];

#تعریف_کرو	    RI_DIGIT	0x01
#تعریف_کرو	    RI_HEX	0x02
#تعریف_کرو	    RI_OCTAL	0x04
#تعریف_کرو	    RI_لفظ	0x08
#تعریف_کرو	    RI_سر	0x10
#تعریف_کرو	    RI_ALPHA	0x20
#تعریف_کرو	    RI_زیریں	0x40
#تعریف_کرو	    RI_بالائی	0x80
#تعریف_کرو	    RI_WHITE	0x100

    ساکن خالی
init_class_tab(خالی)
{
    صحیح		i;
    ساکن صحیح	مکمل = FALSE;

    اگر (مکمل)
	واپس;

    برائے (i = 0; i < 256; ++i)
    {
	اگر (i >= '0' && i <= '7')
	    class_tab[i] = RI_DIGIT + RI_HEX + RI_OCTAL + RI_لفظ;
	ورنہ اگر (i >= '8' && i <= '9')
	    class_tab[i] = RI_DIGIT + RI_HEX + RI_لفظ;
	ورنہ اگر (i >= 'a' && i <= 'f')
	    class_tab[i] = RI_HEX + RI_لفظ + RI_سر + RI_ALPHA + RI_زیریں;
	ورنہ اگر (i >= 'g' && i <= 'z')
	    class_tab[i] = RI_لفظ + RI_سر + RI_ALPHA + RI_زیریں;
	ورنہ اگر (i >= 'A' && i <= 'F')
	    class_tab[i] = RI_HEX + RI_لفظ + RI_سر + RI_ALPHA + RI_بالائی;
	ورنہ اگر (i >= 'G' && i <= 'Z')
	    class_tab[i] = RI_لفظ + RI_سر + RI_ALPHA + RI_بالائی;
	ورنہ اگر (i == '_')
	    class_tab[i] = RI_لفظ + RI_سر;
	ورنہ
	    class_tab[i] = 0;
    }
    class_tab[' '] |= RI_WHITE;
    class_tab['\t'] |= RI_WHITE;
    مکمل = TRUE;
}

#تعریف_کرو ri_digit(c)	((c) < 0x100 && (class_tab[c] & RI_DIGIT))
#تعریف_کرو ri_hex(c)	((c) < 0x100 && (class_tab[c] & RI_HEX))
#تعریف_کرو ri_octal(c)	((c) < 0x100 && (class_tab[c] & RI_OCTAL))
#تعریف_کرو ri_لفظ(c)	((c) < 0x100 && (class_tab[c] & RI_لفظ))
#تعریف_کرو ri_سر(c)	((c) < 0x100 && (class_tab[c] & RI_سر))
#تعریف_کرو ri_alpha(c)	((c) < 0x100 && (class_tab[c] & RI_ALPHA))
#تعریف_کرو ri_زیریں(c)	((c) < 0x100 && (class_tab[c] & RI_زیریں))
#تعریف_کرو ri_بالائی(c)	((c) < 0x100 && (class_tab[c] & RI_بالائی))
#تعریف_کرو ri_white(c)	((c) < 0x100 && (class_tab[c] & RI_WHITE))

// flags for regflags
#تعریف_کرو RF_ICASE    1	// ignore case
#تعریف_کرو RF_NOICASE  2	// don't ignore case
#تعریف_کرو RF_HASNL    4	// can match a NL
#تعریف_کرو RF_ICOMBINE 8	// ignore combining characters
#تعریف_کرو RF_LOOKBH   16	// uses "\@<=" or "\@<!"

/*
 * Global work variables for vim_regcomp().
 */

ساکن char_u	*regparse;	// Input-scan pointer.
ساکن صحیح	regnpar;	// () count.
ساکن صحیح	چاہتا_nfa;	// regex should use NFA engine
#اگر_تعریف_شدہ FEAT_SYN_HL
ساکن صحیح	regnzpar;	// \z() count.
ساکن صحیح	re_has_z;	// \z item detected
#اگر_ختم
ساکن غیرعلامتی	regflags;	// RF_ flags for prog
#اگر defined(FEAT_SYN_HL)
ساکن صحیح	had_eol;	// TRUE when EOL found by vim_regcomp()
#اگر_ختم

ساکن magic_T	reg_magic;	// magicness of the pattern

ساکن صحیح	reg_string;	// matching with a string instead of a buffer
				//سطر
ساکن صحیح	reg_strict;	// "[abc" is illegal

/*
 * META contains all characters that may be magic, except '^' and '$'.
 */

// META[] is used often enough to justify turning it into a table.
ساکن ثابت char_u META_flags[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//		   %  &     (  )  *  +	      .
    0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0,
//     1  2  3	4  5  6  7  8  9	<  =  >  ?
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1,
//  @  A     C	D     F     H  I     K	L  M	 O
    1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1,
//  P	     S	   U  V  W  X	  Z  [		 _
    1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1,
//     a     c	d     f     h  i     k	l  m  n  o
    0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1,
//  p	     s	   u  v  w  x	  z  {	|     ~
    1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1
};

ساکن صحیح	curchr;		// currently parsed character
// Previous character.  Note: prevchr is sometimes -1 when we are not at the
// start, eg in /[ ^I]^ the pattern was never found even if it existed,
// because ^ was taken to be magic -- webb
ساکن صحیح	prevchr;
ساکن صحیح	prevprevchr;	// previous-previous character
ساکن صحیح	nextchr;	// used for ungetchr()

// arguments for reg()
#تعریف_کرو REG_NOPAREN	0	// toplevel reg()
#تعریف_کرو REG_PAREN	1	// \(\)
#تعریف_کرو REG_ZPAREN	2	// \z(\)
#تعریف_کرو REG_NPAREN	3	// \%(\)

// Limit recursive parsing of nested regexp atoms to avoid using up the C stack.
#تعریف_کرو REG_MAX_PAREN_گہرائی	1000

قسمتعریف ساخت
{
    char_u	*regparse;
    صحیح		prevchr_len;
    صحیح		curchr;
    صحیح		prevchr;
    صحیح		prevprevchr;
    صحیح		nextchr;
    صحیح		at_آغاز;
    صحیح		پچھلا_at_آغاز;
    صحیح		regnpar;
} parse_state_T;

ساکن خالی	initchr(char_u *);
ساکن صحیح	getchr(خالی);
ساکن خالی	skipchr_keepstart(خالی);
ساکن صحیح	peekchr(خالی);
ساکن خالی	skipchr(خالی);
ساکن خالی	ungetchr(خالی);
ساکن vimlong_T	gethexchrs(صحیح maxinputlen);
ساکن طویل	getoctchrs(خالی);
ساکن vimlong_T	getdecchrs(خالی);
ساکن صحیح	coll_get_char(خالی);
ساکن صحیح	prog_magic_wrong(خالی);
ساکن صحیح	cstrncmp(char_u *s1, char_u *s2, صحیح *n);
ساکن char_u	*cstrchr(char_u *, صحیح);
ساکن صحیح	re_mult_اگلا(حرف *what);
ساکن صحیح	reg_iswordc(صحیح);
#اگر_تعریف_شدہ FEAT_EVAL
ساکن خالی report_re_switch(char_u *pat);
#اگر_ختم

ساکن regengine_T bt_regengine;
ساکن regengine_T nfa_regengine;

/*
 * Return TRUE if compiled regular expression "prog" can match a line break.
 */
    صحیح
re_multiline(regprog_T *prog)
{
    واپس (prog->regflags & RF_HASNL);
}

/*
 * Check for an equivalence class name "[=a=]".  "pp" points to the '['.
 * Returns a character representing the class. Zero means that no item was
 * recognized.  Otherwise "pp" is advanced to after the item.
 */
    ساکن صحیح
get_equi_class(char_u **pp)
{
    صحیح		c;
    صحیح		l = 1;
    char_u	*p = *pp;

    اگر (p[1] == '=' && p[2] != NUL)
    {
	اگر (has_mbyte)
	    l = (*mb_ptr2len)(p + 2);
	اگر (p[l + 2] == '=' && p[l + 3] == ']')
	{
	    اگر (has_mbyte)
		c = mb_ptr2char(p + 2);
	    ورنہ
		c = p[2];
	    *pp += l + 4;
	    واپس c;
	}
    }
    واپس 0;
}

/*
 * Check for a collating element "[.a.]".  "pp" points to the '['.
 * Returns a character. Zero means that no item was recognized.  Otherwise
 * "pp" is advanced to after the item.
 * Currently only single characters are recognized!
 */
    ساکن صحیح
get_coll_element(char_u **pp)
{
    صحیح		c;
    صحیح		l = 1;
    char_u	*p = *pp;

    اگر (p[0] != NUL && p[1] == '.' && p[2] != NUL)
    {
	اگر (has_mbyte)
	    l = (*mb_ptr2len)(p + 2);
	اگر (p[l + 2] == '.' && p[l + 3] == ']')
	{
	    اگر (has_mbyte)
		c = mb_ptr2char(p + 2);
	    ورنہ
		c = p[2];
	    *pp += l + 4;
	    واپس c;
	}
    }
    واپس 0;
}

ساکن صحیح reg_cpo_lit; // 'cpoptions' contains 'l' flag
ساکن صحیح reg_cpo_bsl; // 'cpoptions' contains '\' flag

    ساکن خالی
get_cpo_flags(خالی)
{
    reg_cpo_lit = vim_strchr(p_cpo, CPO_LITERAL) != تہی;
    reg_cpo_bsl = vim_strchr(p_cpo, CPO_BACKSL) != تہی;
}

/*
 * Skip over a "[]" range.
 * "p" must point to the character after the '['.
 * The returned pointer is on the matching ']', or the terminating NUL.
 */
    ساکن char_u *
skip_anyof(char_u *p)
{
    صحیح		l;

    اگر (*p == '^')	// Complement of range.
	++p;
    اگر (*p == ']' || *p == '-')
	++p;
    جب (*p != NUL && *p != ']')
    {
	اگر (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	    p += l;
	ورنہ
	    اگر (*p == '-')
	    {
		++p;
		اگر (*p != ']' && *p != NUL)
		    MB_PTR_ADV(p);
	    }
	ورنہ اگر (*p == '\\'
		&& !reg_cpo_bsl
		&& (vim_strchr(REGEXP_INRANGE, p[1]) != تہی
		    || (!reg_cpo_lit && vim_strchr(REGEXP_ABBR, p[1]) != تہی)))
	    p += 2;
	ورنہ اگر (*p == '[')
	{
	    اگر (get_char_class(&p) == CLASS_NONE
		    && get_equi_class(&p) == 0
		    && get_coll_element(&p) == 0
		    && *p != NUL)
		++p; // it is not a class name and not NUL
	}
	ورنہ
	    ++p;
    }

    واپس p;
}

/*
 * Skip past regular expression.
 * Stop at end of "startp" or where "delim" is found ('/', '?', etc).
 * Take care of characters with a backslash in front of it.
 * Skip strings inside [ and ].
 */
    char_u *
skip_regexp(
    char_u	*startp,
    صحیح		delim,
    صحیح		magic)
{
    واپس skip_regexp_ex(startp, delim, magic, تہی, تہی, تہی);
}

/*
 * Call skip_regexp() and when the delimiter does not match give an error and
 * return NULL.
 */
    char_u *
skip_regexp_err(
    char_u	*startp,
    صحیح		delim,
    صحیح		magic)
{
    char_u *p = skip_regexp(startp, delim, magic);

    اگر (*p != delim)
    {
	semsg(_(e_missing_delimiter_after_تلاش_نمونہ_str), startp);
	واپس تہی;
    }
    واپس p;
}

/*
 * skip_regexp() with extra arguments:
 * When "newp" is not NULL and "dirc" is '?', make an allocated copy of the
 * expression and change "\?" to "?".  If "*newp" is not NULL the expression
 * is changed in-place.
 * If a "\?" is changed to "?" then "dropped" is incremented, unless NULL.
 * If "magic_val" is not NULL, returns the effective magicness of the pattern
 */
    char_u *
skip_regexp_ex(
    char_u	*startp,
    صحیح		dirc,
    صحیح		magic,
    char_u	**newp,
    صحیح		*dropped,
    magic_T	*magic_val)
{
    magic_T	mymagic;
    char_u	*p = startp;
    size_t	startplen = 0;

    اگر (magic)
	mymagic = MAGIC_ON;
    ورنہ
	mymagic = MAGIC_OFF;
    get_cpo_flags();

    برائے (; p[0] != NUL; MB_PTR_ADV(p))
    {
	اگر (p[0] == dirc)	// found end of regexp
	    توڑو;
	اگر ((p[0] == '[' && mymagic >= MAGIC_ON)
		|| (p[0] == '\\' && p[1] == '[' && mymagic <= MAGIC_OFF))
	{
	    p = skip_anyof(p + 1);
	    اگر (p[0] == NUL)
		توڑو;
	}
	ورنہ اگر (p[0] == '\\' && p[1] != NUL)
	{
	    اگر (dirc == '?' && newp != تہی && p[1] == '?')
	    {
		// change "\?" to "?", make a copy first.
		اگر (startplen == 0)
		    startplen = STRLEN(startp);
		اگر (*newp == تہی)
		{
		    *newp = vim_strnsave(startp, startplen);
		    اگر (*newp != تہی)
		    {
			p = *newp + (p - startp);
			startp = *newp;
		    }
		}
		اگر (dropped != تہی)
		    ++*dropped;
		اگر (*newp != تہی)
		    mch_memmove(p, p + 1, startplen - ((p + 1) - startp) + 1);
		ورنہ
		    ++p;
	    }
	    ورنہ
		++p;    // skip next character
	    اگر (*p == 'v')
		mymagic = MAGIC_ALL;
	    ورنہ اگر (*p == 'V')
		mymagic = MAGIC_NONE;
	}
    }
    اگر (magic_val != تہی)
	*magic_val = mymagic;
    واپس p;
}

/*
 * Functions for getting characters from the regexp input.
 */
ساکن صحیح	prevchr_len;	// byte length of previous char
ساکن صحیح	at_آغاز;	// True when on the first character
ساکن صحیح	پچھلا_at_آغاز;  // True when on the second character

/*
 * Start parsing at "str".
 */
    ساکن خالی
initchr(char_u *str)
{
    regparse = str;
    prevchr_len = 0;
    curchr = prevprevchr = prevchr = nextchr = -1;
    at_آغاز = TRUE;
    پچھلا_at_آغاز = FALSE;
}

/*
 * Save the current parse state, so that it can be restored and parsing
 * starts in the same state again.
 */
    ساکن خالی
save_parse_state(parse_state_T *ps)
{
    ps->regparse = regparse;
    ps->prevchr_len = prevchr_len;
    ps->curchr = curchr;
    ps->prevchr = prevchr;
    ps->prevprevchr = prevprevchr;
    ps->nextchr = nextchr;
    ps->at_آغاز = at_آغاز;
    ps->پچھلا_at_آغاز = پچھلا_at_آغاز;
    ps->regnpar = regnpar;
}

/*
 * Restore a previously saved parse state.
 */
    ساکن خالی
restore_parse_state(parse_state_T *ps)
{
    regparse = ps->regparse;
    prevchr_len = ps->prevchr_len;
    curchr = ps->curchr;
    prevchr = ps->prevchr;
    prevprevchr = ps->prevprevchr;
    nextchr = ps->nextchr;
    at_آغاز = ps->at_آغاز;
    پچھلا_at_آغاز = ps->پچھلا_at_آغاز;
    regnpar = ps->regnpar;
}


/*
 * Get the next character without advancing.
 */
    ساکن صحیح
peekchr(خالی)
{
    ساکن صحیح	after_slash = FALSE;

    اگر (curchr != -1)
	واپس curchr;

    انتخاب (curchr = regparse[0])
    {
	حالت '.':
	حالت '[':
	حالت '~':
	    // magic when 'magic' is on
	    اگر (reg_magic >= MAGIC_ON)
		curchr = Magic(curchr);
	    توڑو;
	حالت '(':
	حالت ')':
	حالت '{':
	حالت '%':
	حالت '+':
	حالت '=':
	حالت '?':
	حالت '@':
	حالت '!':
	حالت '&':
	حالت '|':
	حالت '<':
	حالت '>':
	حالت '#':	// future ext.
	حالت '"':	// future ext.
	حالت '\'':	// future ext.
	حالت ',':	// future ext.
	حالت '-':	// future ext.
	حالت ':':	// future ext.
	حالت ';':	// future ext.
	حالت '`':	// future ext.
	حالت '/':	// Can't be used in / command
			// magic only after "\v"
	    اگر (reg_magic == MAGIC_ALL)
		curchr = Magic(curchr);
	    توڑو;
	حالت '*':
	    // * is not magic as the very first character, eg "?*ptr", when
	    // after '^', eg "/^*ptr" and when after "\(", "\|", "\&".  But
	    // "\(\*" is not magic, thus must be magic if "after_slash"
	    اگر (reg_magic >= MAGIC_ON
		    && !at_آغاز
		    && !(پچھلا_at_آغاز && prevchr == Magic('^'))
		    && (after_slash
			|| (prevchr != Magic('(')
			    && prevchr != Magic('&')
			    && prevchr != Magic('|'))))
		curchr = Magic('*');
	    توڑو;
	حالت '^':
	    // '^' is only magic as the very first character and if it's after
	    // "\(", "\|", "\&' or "\n"
	    اگر (reg_magic >= MAGIC_OFF
		    && (at_آغاز
			|| reg_magic == MAGIC_ALL
			|| prevchr == Magic('(')
			|| prevchr == Magic('|')
			|| prevchr == Magic('&')
			|| prevchr == Magic('n')
			|| (no_Magic(prevchr) == '('
			    && prevprevchr == Magic('%'))))
	    {
		curchr = Magic('^');
		at_آغاز = TRUE;
		پچھلا_at_آغاز = FALSE;
	    }
	    توڑو;
	حالت '$':
	    // '$' is only magic as the very last char and if it's in front of
	    // either "\|", "\)", "\&", or "\n"
	    اگر (reg_magic >= MAGIC_OFF)
	    {
		char_u *p = regparse + 1;
		صحیح is_magic_all = (reg_magic == MAGIC_ALL);

		// ignore \c \C \m \M \v \V and \Z after '$'
		جب (p[0] == '\\' && (p[1] == 'c' || p[1] == 'C'
			    || p[1] == 'm' || p[1] == 'M'
			    || p[1] == 'v' || p[1] == 'V' || p[1] == 'Z'))
		{
		    اگر (p[1] == 'v')
			is_magic_all = TRUE;
		    ورنہ اگر (p[1] == 'm' || p[1] == 'M' || p[1] == 'V')
			is_magic_all = FALSE;
		    p += 2;
		}
		اگر (p[0] == NUL
			|| (p[0] == '\\'
			    && (p[1] == '|' || p[1] == '&' || p[1] == ')'
				|| p[1] == 'n'))
			|| (is_magic_all
			    && (p[0] == '|' || p[0] == '&' || p[0] == ')'))
			|| reg_magic == MAGIC_ALL)
		    curchr = Magic('$');
	    }
	    توڑو;
	حالت '\\':
	    {
		صحیح c = regparse[1];

		اگر (c == NUL)
		    curchr = '\\';	// trailing '\'
		ورنہ اگر (c <= '~' && META_flags[c])
		{
		    /*
		     * META contains everything that may be magic sometimes,
		     * except ^ and $ ("\^" and "\$" are only magic after
		     * "\V").  We now fetch the next character and toggle its
		     * magicness.  Therefore, \ is so meta-magic that it is
		     * not in META.
		     */
		    curchr = -1;
		    پچھلا_at_آغاز = at_آغاز;
		    at_آغاز = FALSE;	// be able to say "/\*ptr"
		    ++regparse;
		    ++after_slash;
		    peekchr();
		    --regparse;
		    --after_slash;
		    curchr = toggle_Magic(curchr);
		}
		ورنہ اگر (vim_strchr(REGEXP_ABBR, c))
		{
		    /*
		     * Handle abbreviations, like "\t" for TAB -- webb
		     */
		    curchr = backslash_trans(c);
		}
		ورنہ اگر (reg_magic == MAGIC_NONE && (c == '$' || c == '^'))
		    curchr = toggle_Magic(c);
		ورنہ
		{
		    /*
		     * Next character can never be (made) magic?
		     * Then backslashing it won't do anything.
		     */
		    اگر (has_mbyte && c >= 0x80)
			curchr = (*mb_ptr2char)(regparse + 1);
		    ورنہ
			curchr = c;
		}
		توڑو;
	    }

	طےشدہ:
	    // curchr already holds regparse[0]; only a multi-byte lead byte
	    // needs decoding.
	    اگر (has_mbyte && curchr >= 0x80)
		curchr = (*mb_ptr2char)(regparse);
    }

    واپس curchr;
}

/*
 * Eat one lexed character.  Do this in a way that we can undo it.
 */
    ساکن خالی
skipchr(خالی)
{
    // peekchr() eats a backslash, do the same here
    اگر (*regparse == '\\')
	prevchr_len = 1;
    ورنہ
	prevchr_len = 0;
    اگر (regparse[prevchr_len] != NUL)
    {
	اگر (enc_utf8)
	{
	    // Exclude composing chars that mb_ptr2len does include.  A byte
	    // below 0x80 is always a single character.
	    اگر (regparse[prevchr_len] < 0x80)
		++prevchr_len;
	    ورنہ
		prevchr_len += utf_ptr2len(regparse + prevchr_len);
	}
	ورنہ اگر (has_mbyte)
	    prevchr_len += (*mb_ptr2len)(regparse + prevchr_len);
	ورنہ
	    ++prevchr_len;
    }
    regparse += prevchr_len;
    پچھلا_at_آغاز = at_آغاز;
    at_آغاز = FALSE;
    prevprevchr = prevchr;
    prevchr = curchr;
    curchr = nextchr;	    // use previously unget char, or -1
    nextchr = -1;
}

/*
 * Skip a character while keeping the value of prev_at_start for at_start.
 * prevchr and prevprevchr are also kept.
 */
    ساکن خالی
skipchr_keepstart(خالی)
{
    صحیح as = پچھلا_at_آغاز;
    صحیح pr = prevchr;
    صحیح prpr = prevprevchr;

    skipchr();
    at_آغاز = as;
    prevchr = pr;
    prevprevchr = prpr;
}

/*
 * Get the next character from the pattern. We know about magic and such, so
 * therefore we need a lexical analyzer.
 */
    ساکن صحیح
getchr(خالی)
{
    صحیح chr = peekchr();

    skipchr();
    واپس chr;
}

/*
 * put character back.  Works only once!
 */
    ساکن خالی
ungetchr(خالی)
{
    nextchr = curchr;
    curchr = prevchr;
    prevchr = prevprevchr;
    at_آغاز = پچھلا_at_آغاز;
    پچھلا_at_آغاز = FALSE;

    // Backup regparse, so that it's at the same position as before the
    // getchr().
    regparse -= prevchr_len;
}

/*
 * Get and return the value of the hex string at the current position.
 * Return -1 if there is no valid hex number.
 * The position is updated:
 *     blahblah\%x20asdf
 *	   before-^ ^-after
 * The parameter controls the maximum number of input characters. This will be
 * 2 when reading a \%x20 sequence and 4 when reading a \%u20AC sequence.
 */
    ساکن vimlong_T
gethexchrs(صحیح maxinputlen)
{
    long_u	nr = 0;
    صحیح		c;
    صحیح		i;

    برائے (i = 0; i < maxinputlen; ++i)
    {
	c = regparse[0];
	اگر (!vim_isxdigit(c))
	    توڑو;
	nr <<= 4;
	nr |= hex2nr(c);
	++regparse;
    }

    اگر (i == 0)
	واپس -1;
    واپس nr;
}

/*
 * Get and return the value of the decimal string immediately after the
 * current position. Return -1 for invalid.  Consumes all digits.
 */
    ساکن vimlong_T
getdecchrs(خالی)
{
    long_u	nr = 0;
    صحیح		c;
    صحیح		i;

    برائے (i = 0; ; ++i)
    {
	c = regparse[0];
	اگر (c < '0' || c > '9')
	    توڑو;
	nr *= 10;
	nr += c - '0';
	++regparse;
	curchr = -1; // no longer valid
    }

    اگر (i == 0)
	واپس -1;
    واپس nr;
}

/*
 * get and return the value of the octal string immediately after the current
 * position. Return -1 for invalid, or 0-255 for valid. Smart enough to handle
 * numbers > 377 correctly (for example, 400 is treated as 40) and doesn't
 * treat 8 or 9 as recognised characters. Position is updated:
 *     blahblah\%o210asdf
 *	   before-^  ^-after
 */
    ساکن طویل
getoctchrs(خالی)
{
    long_u	nr = 0;
    صحیح		c;
    صحیح		i;

    برائے (i = 0; i < 3 && nr < 040; ++i)
    {
	c = regparse[0];
	اگر (c < '0' || c > '7')
	    توڑو;
	nr <<= 3;
	nr |= hex2nr(c);
	++regparse;
    }

    اگر (i == 0)
	واپس -1;
    واپس (طویل)nr;
}

/*
 * read_limits - Read two integers to be taken as a minimum and maximum.
 * If the first character is '-', then the range is reversed.
 * Should end with 'end'.  If minval is missing, zero is default, if maxval is
 * missing, a very big number is the default.
 */
    ساکن صحیح
read_limits(طویل *minval, طویل *maxval)
{
    صحیح		الٹ = FALSE;
    char_u	*پہلا_char;
    طویل	tmp;

    اگر (*regparse == '-')
    {
	// Starts with '-', so reverse the range later
	regparse++;
	الٹ = TRUE;
    }
    پہلا_char = regparse;
    *minval = getdigits(&regparse);
    اگر (*regparse == ',')	    // There is a comma
    {
	اگر (vim_isdigit(*++regparse))
	    *maxval = getdigits(&regparse);
	ورنہ
	    *maxval = MAX_حد_بندی;
    }
    ورنہ اگر (VIM_ISDIGIT(*پہلا_char))
	*maxval = *minval;	    // It was \{n} or \{-n}
    ورنہ
	*maxval = MAX_حد_بندی;	    // It was \{} or \{-}
    اگر (*regparse == '\\')
	regparse++;	// Allow either \{...} or \{...\}
    اگر (*regparse != '}')
	EMSG2_RET_FAIL(_(e_syntax_error_اندر_str_curlies),
						       reg_magic == MAGIC_ALL);

    /*
     * Reverse the range if there was a '-', or make sure it is in the right
     * order otherwise.
     */
    اگر ((!الٹ && *minval > *maxval) || (الٹ && *minval < *maxval))
    {
	tmp = *minval;
	*minval = *maxval;
	*maxval = tmp;
    }
    skipchr();		// let's be friends with the lexer again
    واپس OK;
}

/*
 * vim_regexec and friends
 */

/*
 * Global work variables for vim_regexec().
 */

ساکن خالی	cleanup_subexpr(خالی);
#اگر_تعریف_شدہ FEAT_SYN_HL
ساکن خالی	cleanup_zsubexpr(خالی);
#اگر_ختم
ساکن صحیح	مطابقت_with_backref(linenr_T آغاز_lnum, colnr_T آغاز_ستون, linenr_T انتہا_lnum, colnr_T انتہا_ستون, صحیح *bytelen);

/*
 * Sometimes need to save a copy of a line.  Since alloc()/free() is very
 * slow, we keep one allocated piece of memory and only re-allocate it when
 * it's too small.  It's freed in bt_regexec_both() when finished.
 */
ساکن char_u	*reg_tofree = تہی;
ساکن غیرعلامتی	reg_tofreelen;

/*
 * Structure used to store the execution state of the regex engine.
 * Which ones are set depends on whether a single-line or multi-line match is
 * done:
 *			single-line		multi-line
 * reg_match		&regmatch_T		NULL
 * reg_mmatch		NULL			&regmmatch_T
 * reg_startp		reg_match->startp	<invalid>
 * reg_endp		reg_match->endp		<invalid>
 * reg_startpos		<invalid>		reg_mmatch->startpos
 * reg_endpos		<invalid>		reg_mmatch->endpos
 * reg_win		NULL			window in which to search
 * reg_buf		curbuf			buffer in which to search
 * reg_firstlnum	<invalid>		first line in which to search
 * reg_maxline		0			last line nr
 * reg_line_lbr		FALSE or TRUE		FALSE
 */
قسمتعریف ساخت {
    regmatch_T		*reg_مطابقت;
    regmmatch_T		*reg_mmatch;

    char_u		**reg_startp;
    char_u		**reg_endp;
    lpos_T		*reg_startpos;
    lpos_T		*reg_endpos;

    win_T		*reg_win;
    buf_T		*reg_buf;
    linenr_T		reg_firstlnum;
    linenr_T		reg_maxline;
    صحیح			reg_سطر_lbr;	// "\n" in string is line break

    // The current match-position is stord in these variables:
    linenr_T	lnum;		// line number, relative to first line
    char_u	*سطر;		// start of current line
    char_u	*داخلہ;		// current input, points into "line"

    صحیح	need_clear_subexpr;	// subexpressions still need to be cleared
#اگر_تعریف_شدہ FEAT_SYN_HL
    صحیح	need_clear_zsubexpr;	// extmatch subexpressions still need to be
				// cleared
#اگر_ختم

    // Internal copy of 'ignorecase'.  It is set at each call to vim_regexec().
    // Normally it gets the value of "rm_ic" or "rmm_ic", but when the pattern
    // contains '\c' or '\C' the value is overruled.
    صحیح			reg_ic;

    // Similar to "reg_ic", but only for 'combining' characters.  Set with \Z
    // flag in the regexp.  Defaults to false, always.
    صحیح			reg_icombine;

    // Copy of "rmm_maxcol": maximum column to search for a match.  Zero when
    // there is no maximum.
    colnr_T		reg_maxcol;

    // State for the NFA engine regexec.
    صحیح nfa_has_zend;	    // NFA regexp \ze operator encountered.
    صحیح nfa_has_backref;    // NFA regexp \1 .. \9 encountered.
    صحیح nfa_nsubexpr;	    // Number of sub expressions actually being used
			    // during execution. 1 if only the whole match
			    // (subexpr 0) is used.
    // listid is global, so that it increases on recursive calls to
    // nfa_regmatch(), which means we don't have to clear the lastlist field of
    // all the states.
    صحیح nfa_listid;
    صحیح nfa_alt_listid;

#اگر_تعریف_شدہ FEAT_SYN_HL
    صحیح nfa_has_zsubexpr;   // NFA regexp has \z( ), set zsubexpr.
#اگر_ختم
} regexec_T;

ساکن regexec_T	rex;
ساکن صحیح		rex_اندر_use = FALSE;

/*
 * Return TRUE if character 'c' is included in 'iskeyword' option for
 * "reg_buf" buffer.
 */
    ساکن صحیح
reg_iswordc(صحیح c)
{
    واپس vim_iswordc_buf(c, rex.reg_buf);
}

#اگر_تعریف_شدہ FEAT_EVAL
ساکن صحیح can_f_submatch = FALSE;	// TRUE when submatch() can be used

// This struct is used for reg_submatch(). Needed for when the
// substitution string is an expression that contains a call to substitute()
// and submatch().
قسمتعریف ساخت {
    regmatch_T	*sm_مطابقت;
    regmmatch_T	*sm_mmatch;
    linenr_T	sm_firstlnum;
    linenr_T	sm_maxline;
    صحیح		sm_سطر_lbr;
} regsubmatch_T;

ساکن regsubmatch_T rsm;  // can only be used when can_f_submatch is TRUE
#اگر_ختم

قسمتعریف شمار
{
    RGLF_سطر = 0x01,
    RGLF_لمبائی = 0x02
#اگر_تعریف_شدہ FEAT_EVAL
    ,
    RGLF_SUBMATCH = 0x04
#اگر_ختم
} reg_getline_flags_T;

//
// common code for reg_getline(), reg_getline_len(), reg_getline_submatch() and
// reg_getline_submatch_len().
// the flags argument (which is a bitmask) controls what info is to be returned and whether
// or not submatch is in effect.
// note:
//     submatch is available only if FEAT_EVAL is defined.
    ساکن خالی
reg_getline_common(
    linenr_T		lnum,
    reg_getline_flags_T	flags,
    char_u		**سطر,
    colnr_T		*لمبائی)
{
    صحیح get_سطر = flags & RGLF_سطر;
    صحیح get_لمبائی = flags & RGLF_لمبائی;
    linenr_T firstlnum;
    linenr_T maxline;

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (flags & RGLF_SUBMATCH)
    {
	firstlnum = rsm.sm_firstlnum + lnum;
	maxline = rsm.sm_maxline;
    }
    ورنہ
#اگر_ختم
    {
	firstlnum = rex.reg_firstlnum + lnum;
	maxline = rex.reg_maxline;
    }

    // when looking behind for a match/no-match lnum is negative. but we
    // can't go before line 1.
    اگر (firstlnum < 1)
    {
	اگر (get_سطر)
	    *سطر = تہی;
	اگر (get_لمبائی)
	    *لمبائی = 0;

	واپس;
    }

    اگر (lnum > maxline)
    {
	// must have matched the "\n" in the last line.
	اگر (get_سطر)
	    *سطر = (char_u *)"";
	اگر (get_لمبائی)
	    *لمبائی = 0;

	واپس;
    }

    اگر (get_سطر)
	*سطر = ml_get_buf(rex.reg_buf, firstlnum, FALSE);
    اگر (get_لمبائی)
	*لمبائی = ml_get_buf_len(rex.reg_buf, firstlnum);
}

/*
 * Get pointer to the line "lnum", which is relative to "reg_firstlnum".
 */
    ساکن char_u *
reg_getline(linenr_T lnum)
{
    char_u *سطر;

    reg_getline_common(lnum, RGLF_سطر, &سطر, تہی);

    واپس سطر;
}

/*
 * Get length of line "lnum", which is relative to "reg_firstlnum".
 */
    ساکن colnr_T
reg_getline_len(linenr_T lnum)
{
    colnr_T لمبائی;

    reg_getline_common(lnum, RGLF_لمبائی, تہی, &لمبائی);

    واپس لمبائی;
}

#اگر_تعریف_شدہ FEAT_SYN_HL
ساکن char_u	*reg_startzp[NSUBEXP];	// Workspace to mark beginning
ساکن char_u	*reg_endzp[NSUBEXP];	//   and end of \z(...\) matches
ساکن lpos_T	reg_startzpos[NSUBEXP];	// idem, beginning pos
ساکن lpos_T	reg_endzpos[NSUBEXP];	// idem, end pos
#اگر_ختم

// TRUE if using multi-line regexp.
#تعریف_کرو REG_MULTI	(rex.reg_مطابقت == تہی)

#اگر_تعریف_شدہ FEAT_SYN_HL
/*
 * Create a new extmatch and mark it as referenced once.
 */
    ساکن reg_extmatch_T *
make_extmatch(خالی)
{
    reg_extmatch_T	*em;

    em = ALLOC_CLEAR_ONE(reg_extmatch_T);
    اگر (em != تہی)
	em->refcnt = 1;
    واپس em;
}

/*
 * Add a reference to an extmatch.
 */
    reg_extmatch_T *
ref_extmatch(reg_extmatch_T *em)
{
    اگر (em != تہی)
	em->refcnt++;
    واپس em;
}

/*
 * Remove a reference to an extmatch.  If there are no references left, free
 * the info.
 */
    خالی
unref_extmatch(reg_extmatch_T *em)
{
    صحیح i;

    اگر (em != تہی && --em->refcnt <= 0)
    {
	برائے (i = 0; i < NSUBEXP; ++i)
	    vim_free(em->matches[i]);
	vim_free(em);
    }
}
#اگر_ختم

/*
 * Get class of previous character.
 */
    ساکن صحیح
reg_پچھلا_class(خالی)
{
    اگر (rex.داخلہ > rex.سطر)
	واپس mb_get_class_buf(rex.داخلہ - 1
		       - (*mb_سر_off)(rex.سطر, rex.داخلہ - 1), rex.reg_buf);
    واپس -1;
}

/*
 * Return TRUE if the current rex.input position matches the Visual area.
 */
    ساکن صحیح
reg_مطابقت_visual(خالی)
{
    pos_T	چوٹی, bot;
    linenr_T    lnum;
    colnr_T	ستون;
    win_T	*wp = rex.reg_win == تہی ? curwin : rex.reg_win;
    صحیح		منوال;
    colnr_T	آغاز, انتہا;
    colnr_T	start2, end2;
    colnr_T	ستون_تعداد;
    colnr_T	curswant;

    // Check if the buffer is the current buffer and not using a string.
    اگر (rex.reg_buf != curbuf || VIsual.lnum == 0 || !REG_MULTI)
	واپس FALSE;

    اگر (VIsual_فعال)
    {
	اگر (LT_POS(VIsual, wp->w_cursor))
	{
	    چوٹی = VIsual;
	    bot = wp->w_cursor;
	}
	ورنہ
	{
	    چوٹی = wp->w_cursor;
	    bot = VIsual;
	}
	منوال = VIsual_منوال;
	curswant = wp->w_curswant;
    }
    ورنہ
    {
	اگر (LT_POS(curbuf->b_visual.vi_آغاز, curbuf->b_visual.vi_انتہا))
	{
	    چوٹی = curbuf->b_visual.vi_آغاز;
	    bot = curbuf->b_visual.vi_انتہا;
	}
	ورنہ
	{
	    چوٹی = curbuf->b_visual.vi_انتہا;
	    bot = curbuf->b_visual.vi_آغاز;
	}
	// a substitute command may have removed some lines
	اگر (bot.lnum > curbuf->b_ml.ml_سطر_گنتی)
	    bot.lnum = curbuf->b_ml.ml_سطر_گنتی;
	منوال = curbuf->b_visual.vi_منوال;
	curswant = curbuf->b_visual.vi_curswant;
    }
    lnum = rex.lnum + rex.reg_firstlnum;
    اگر (lnum < چوٹی.lnum || lnum > bot.lnum)
	واپس FALSE;

    ستون = (colnr_T)(rex.داخلہ - rex.سطر);
    اگر (منوال == 'v')
    {
	اگر ((lnum == چوٹی.lnum && ستون < چوٹی.ستون)
		|| (lnum == bot.lnum && ستون >= bot.ستون + (*p_sel != 'e')))
	    واپس FALSE;
    }
    ورنہ اگر (منوال == Ctrl_V)
    {
	getvvcol(wp, &چوٹی, &آغاز, تہی, &انتہا, 0);
	getvvcol(wp, &bot, &start2, تہی, &end2, 0);
	اگر (start2 < آغاز)
	    آغاز = start2;
	اگر (end2 > انتہا)
	    انتہا = end2;
	اگر (چوٹی.ستون == MAXCOL || bot.ستون == MAXCOL || curswant == MAXCOL)
	    انتہا = MAXCOL;

	// getvvcol() flushes rex.line, need to get it again
	rex.سطر = reg_getline(rex.lnum);
	rex.داخلہ = rex.سطر + ستون;

	ستون_تعداد = win_linetabsize(wp, rex.reg_firstlnum + rex.lnum, rex.سطر, ستون);
	اگر (ستون_تعداد < آغاز || ستون_تعداد > انتہا - (*p_sel == 'e'))
	    واپس FALSE;
    }
    واپس TRUE;
}

/*
 * Check the regexp program for its magic number.
 * Return TRUE if it's wrong.
 */
    ساکن صحیح
prog_magic_wrong(خالی)
{
    regprog_T	*prog;

    prog = REG_MULTI ? rex.reg_mmatch->regprog : rex.reg_مطابقت->regprog;
    اگر (prog->engine == &nfa_regengine)
	// For NFA matcher we don't check the magic
	واپس FALSE;

    اگر (UCHARAT(((bt_regprog_T *)prog)->پروگرام) != REGMAGIC)
    {
	iemsg(e_corrupted_regexp_پروگرام);
	واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Cleanup the subexpressions, if this wasn't done yet.
 * This construction is used to clear the subexpressions only when they are
 * used (to increase speed).
 */
    ساکن خالی
cleanup_subexpr(خالی)
{
    اگر (!rex.need_clear_subexpr)
	واپس;

    اگر (REG_MULTI)
    {
	// Use 0xff to set lnum to -1
	vim_memset(rex.reg_startpos, 0xff, حجم(lpos_T) * NSUBEXP);
	vim_memset(rex.reg_endpos, 0xff, حجم(lpos_T) * NSUBEXP);
    }
    ورنہ
    {
	vim_memset(rex.reg_startp, 0, حجم(char_u *) * NSUBEXP);
	vim_memset(rex.reg_endp, 0, حجم(char_u *) * NSUBEXP);
    }
    rex.need_clear_subexpr = FALSE;
}

#اگر_تعریف_شدہ FEAT_SYN_HL
    ساکن خالی
cleanup_zsubexpr(خالی)
{
    اگر (!rex.need_clear_zsubexpr)
	واپس;

    اگر (REG_MULTI)
    {
	// Use 0xff to set lnum to -1
	vim_memset(reg_startzpos, 0xff, حجم(lpos_T) * NSUBEXP);
	vim_memset(reg_endzpos, 0xff, حجم(lpos_T) * NSUBEXP);
    }
    ورنہ
    {
	vim_memset(reg_startzp, 0, حجم(char_u *) * NSUBEXP);
	vim_memset(reg_endzp, 0, حجم(char_u *) * NSUBEXP);
    }
    rex.need_clear_zsubexpr = FALSE;
}
#اگر_ختم

/*
 * Advance rex.lnum, rex.line and rex.input to the next line.
 */
    ساکن خالی
reg_nextline(خالی)
{
    rex.سطر = reg_getline(++rex.lnum);
    rex.داخلہ = rex.سطر;
    fast_breakcheck();
}

/*
 * Check whether a backreference matches.
 * Returns RA_FAIL, RA_NOMATCH or RA_MATCH.
 * If "bytelen" is not NULL, it is set to the byte length of the match in the
 * last line.
 * Optional: ignore case if rex.reg_ic is set.
 */
    ساکن صحیح
مطابقت_with_backref(
    linenr_T آغاز_lnum,
    colnr_T  آغاز_ستون,
    linenr_T انتہا_lnum,
    colnr_T  انتہا_ستون,
    صحیح	     *bytelen)
{
    linenr_T	clnum = آغاز_lnum;
    colnr_T	ccol = آغاز_ستون;
    صحیح		len;
    char_u	*p;

    اگر (bytelen != تہی)
	*bytelen = 0;
    برائے (;;)
    {
	// Since getting one line may invalidate the other, need to make copy.
	// Slow!
	اگر (rex.سطر != reg_tofree)
	{
	    len = (صحیح)STRLEN(rex.سطر);
	    اگر (reg_tofree == تہی || len >= (صحیح)reg_tofreelen)
	    {
		len += 50;	// get some extra
		vim_free(reg_tofree);
		reg_tofree = alloc(len);
		اگر (reg_tofree == تہی)
		    واپس RA_FAIL; // out of memory!
		reg_tofreelen = len;
	    }
	    STRCPY(reg_tofree, rex.سطر);
	    rex.داخلہ = reg_tofree + (rex.داخلہ - rex.سطر);
	    rex.سطر = reg_tofree;
	}

	// Get the line to compare with.
	p = reg_getline(clnum);
	اگر (clnum == انتہا_lnum)
	    len = انتہا_ستون - ccol;
	ورنہ
	    len = (صحیح)reg_getline_len(clnum) - ccol;

	// Use case-insensitive compare if rex.reg_ic is set
	اگر ((!rex.reg_ic && cstrncmp(p + ccol, rex.داخلہ, &len) != 0)
	    || (rex.reg_ic && MB_STRNICMP(p + ccol, rex.داخلہ, len) != 0))
	    واپس RA_NOMATCH;  // doesn't match
	اگر (bytelen != تہی)
	    *bytelen += len;
	اگر (clnum == انتہا_lnum)
	    توڑو;		// match and at end!
	اگر (rex.lnum >= rex.reg_maxline)
	    واپس RA_NOMATCH;  // text too short

	// Advance to next line.
	reg_nextline();
	اگر (bytelen != تہی)
	    *bytelen = 0;
	++clnum;
	ccol = 0;
	اگر (got_int)
	    واپس RA_FAIL;
    }

    // found a match!  Note that rex.line may now point to a copy of the line,
    // that should not matter.
    واپس RA_مطابقت;
}

/*
 * Used in a place where no * or \+ can follow.
 */
    ساکن صحیح
re_mult_اگلا(حرف *what)
{
    اگر (re_multi_type(peekchr()) == MULTI_MULT)
    {
	semsg(_(e_nfa_regexp_cannot_repeat_str), what);
	rc_did_emsg = TRUE;
	واپس FAIL;
    }
    واپس OK;
}

قسمتعریف ساخت
{
    صحیح a, b, c;
} decomp_T;


// 0xfb20 - 0xfb4f
ساکن decomp_T decomp_جدول[0xfb4f-0xfb20+1] =
{
    {0x5e2,0,0},		// 0xfb20	alt ayin
    {0x5d0,0,0},		// 0xfb21	alt alef
    {0x5d3,0,0},		// 0xfb22	alt dalet
    {0x5d4,0,0},		// 0xfb23	alt he
    {0x5db,0,0},		// 0xfb24	alt kaf
    {0x5dc,0,0},		// 0xfb25	alt lamed
    {0x5dd,0,0},		// 0xfb26	alt mem-sofit
    {0x5e8,0,0},		// 0xfb27	alt resh
    {0x5ea,0,0},		// 0xfb28	alt tav
    {'+', 0, 0},		// 0xfb29	alt plus
    {0x5e9, 0x5c1, 0},		// 0xfb2a	shin+shin-dot
    {0x5e9, 0x5c2, 0},		// 0xfb2b	shin+sin-dot
    {0x5e9, 0x5c1, 0x5bc},	// 0xfb2c	shin+shin-dot+dagesh
    {0x5e9, 0x5c2, 0x5bc},	// 0xfb2d	shin+sin-dot+dagesh
    {0x5d0, 0x5b7, 0},		// 0xfb2e	alef+patah
    {0x5d0, 0x5b8, 0},		// 0xfb2f	alef+qamats
    {0x5d0, 0x5b4, 0},		// 0xfb30	alef+hiriq
    {0x5d1, 0x5bc, 0},		// 0xfb31	bet+dagesh
    {0x5d2, 0x5bc, 0},		// 0xfb32	gimel+dagesh
    {0x5d3, 0x5bc, 0},		// 0xfb33	dalet+dagesh
    {0x5d4, 0x5bc, 0},		// 0xfb34	he+dagesh
    {0x5d5, 0x5bc, 0},		// 0xfb35	vav+dagesh
    {0x5d6, 0x5bc, 0},		// 0xfb36	zayin+dagesh
    {0xfb37, 0, 0},		// 0xfb37 -- UNUSED
    {0x5d8, 0x5bc, 0},		// 0xfb38	tet+dagesh
    {0x5d9, 0x5bc, 0},		// 0xfb39	yud+dagesh
    {0x5da, 0x5bc, 0},		// 0xfb3a	kaf sofit+dagesh
    {0x5db, 0x5bc, 0},		// 0xfb3b	kaf+dagesh
    {0x5dc, 0x5bc, 0},		// 0xfb3c	lamed+dagesh
    {0xfb3d, 0, 0},		// 0xfb3d -- UNUSED
    {0x5de, 0x5bc, 0},		// 0xfb3e	mem+dagesh
    {0xfb3f, 0, 0},		// 0xfb3f -- UNUSED
    {0x5e0, 0x5bc, 0},		// 0xfb40	nun+dagesh
    {0x5e1, 0x5bc, 0},		// 0xfb41	samech+dagesh
    {0xfb42, 0, 0},		// 0xfb42 -- UNUSED
    {0x5e3, 0x5bc, 0},		// 0xfb43	pe sofit+dagesh
    {0x5e4, 0x5bc,0},		// 0xfb44	pe+dagesh
    {0xfb45, 0, 0},		// 0xfb45 -- UNUSED
    {0x5e6, 0x5bc, 0},		// 0xfb46	tsadi+dagesh
    {0x5e7, 0x5bc, 0},		// 0xfb47	qof+dagesh
    {0x5e8, 0x5bc, 0},		// 0xfb48	resh+dagesh
    {0x5e9, 0x5bc, 0},		// 0xfb49	shin+dagesh
    {0x5ea, 0x5bc, 0},		// 0xfb4a	tav+dagesh
    {0x5d5, 0x5b9, 0},		// 0xfb4b	vav+holam
    {0x5d1, 0x5bf, 0},		// 0xfb4c	bet+rafe
    {0x5db, 0x5bf, 0},		// 0xfb4d	kaf+rafe
    {0x5e4, 0x5bf, 0},		// 0xfb4e	pe+rafe
    {0x5d0, 0x5dc, 0}		// 0xfb4f	alef-lamed
};

    ساکن خالی
mb_decompose(صحیح c, صحیح *c1, صحیح *c2, صحیح *c3)
{
    decomp_T d;

    اگر (c >= 0xfb20 && c <= 0xfb4f)
    {
	d = decomp_جدول[c - 0xfb20];
	*c1 = d.a;
	*c2 = d.b;
	*c3 = d.c;
    }
    ورنہ
    {
	*c1 = c;
	*c2 = 0;
	*c3 = 0;
    }
}

/*
 * Compare two strings, ignore case if rex.reg_ic set.
 * Return 0 if strings match, non-zero otherwise.
 * "*n" is the byte length of s1 on entry and of the match in s2 on success.
 * Correct the length "*n" when composing characters are ignored
 * or for utf8 when both utf codepoints are considered equal because of
 * case-folding but have different length (e.g. 's' and 'ſ')
 */
    ساکن صحیح
cstrncmp(char_u *s1, char_u *s2, صحیح *n)
{
    صحیح		نتیجہ;

    اگر (!rex.reg_ic)
	نتیجہ = STRNCMP(s1, s2, *n);
    ورنہ اگر (enc_utf8)
    {
	char_u *p = s1;
	صحیح n2 = 0;
	صحیح n1 = *n;
	// count the number of characters for byte-length of s1
	جب (n1 > 0 && *p != NUL)
	{
	    n1 -= mb_ptr2len(p);
	    MB_PTR_ADV(p);
	    n2++;
	}
	// count the number of bytes to advance the same number of chars for s2
	p = s2;
	جب (n2-- > 0 && *p != NUL)
	    MB_PTR_ADV(p);

	n2 = p - s2;

	نتیجہ = MB_STRNICMP2(s1, s2, *n, n2);
	اگر (نتیجہ == 0)
	    *n = n2;
    }
    ورنہ
	نتیجہ = MB_STRNICMP(s1, s2, *n);

    // if it failed and it's utf8 and we want to combineignore:
    اگر (نتیجہ != 0 && enc_utf8 && rex.reg_icombine)
    {
	char_u	*str1, *str2;
	صحیح	c1, c2, c11, c12;
	صحیح	junk;

	// we have to handle the strcmp ourselves, since it is necessary to
	// deal with the composing characters by ignoring them:
	str1 = s1;
	str2 = s2;
	c1 = c2 = 0;
	جب ((صحیح)(str1 - s1) < *n)
	{
	    c1 = mb_ptr2char_adv(&str1);
	    c2 = mb_ptr2char_adv(&str2);

	    // Decompose the character if necessary, into 'base' characters.
	    // Currently hard-coded for Hebrew, Arabic to be done...
	    اگر (c1 != c2 && (!rex.reg_ic || utf_fold(c1) != utf_fold(c2)))
	    {
		// decomposition necessary?
		mb_decompose(c1, &c11, &junk, &junk);
		mb_decompose(c2, &c12, &junk, &junk);
		c1 = c11;
		c2 = c12;
		اگر (c11 != c12
			    && (!rex.reg_ic || utf_fold(c11) != utf_fold(c12)))
		    توڑو;
	    }
	}
	نتیجہ = c2 - c1;
	اگر (نتیجہ == 0)
	    *n = (صحیح)(str2 - s2);
    }

    واپس نتیجہ;
}

/*
 * cstrchr: This function is used a lot for simple searches, keep it fast!
 */
    ساکن char_u *
cstrchr(char_u *s, صحیح c)
{
    char_u	*p;
    صحیح		cc, lc;

    اگر (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	واپس vim_strchr(s, c);

    // tolower() and toupper() can be slow, comparing twice should be a lot
    // faster (esp. when using MS Visual C++!).
    // For UTF-8 need to use folded case.
    اگر (enc_utf8 && c > 0x80)
    {
	cc = utf_fold(c);
	lc = cc;
    }
    ورنہ
	اگر (MB_ISUPPER(c))
	{
	    cc = MB_TOLOWER(c);
	    lc = cc;
	}
	ورنہ اگر (MB_ISLOWER(c))
	{
	    cc = MB_TOUPPER(c);
	    lc = c;
	}
	ورنہ
	    واپس vim_strchr(s, c);

    اگر (has_mbyte)
    {
	برائے (p = s; *p != NUL; p += (*mb_ptr2len)(p))
	{
	    صحیح uc = utf_ptr2char(p);
	    اگر (enc_utf8 && (c > 0x80 || uc > 0x80))
	    {
		// Do not match an illegal byte.  E.g. 0xff matches 0xc3 0xbf,
		// not 0xff.
		// compare with lower case of the character
		اگر ((uc < 0x80 || uc != *p) && utf_fold(uc) == lc)
		    واپس p;
	    }
	    ورنہ اگر (*p == c || *p == cc)
		واپس p;
	}
    }
    ورنہ
	// Faster version for when there are no multi-byte characters.
	برائے (p = s; *p != NUL; ++p)
	    اگر (*p == c || *p == cc)
		واپس p;

    واپس تہی;
}

////////////////////////////////////////////////////////////////
//		      regsub stuff			      //
////////////////////////////////////////////////////////////////

قسمتعریف خالی (*fptr_T)(صحیح *, صحیح);

ساکن صحیح vim_regsub_both(char_u *ماخذ, typval_T *اظہاریہ, char_u *dest, صحیح destlen, صحیح flags);

    ساکن خالی
do_بالائی(صحیح *d, صحیح c)
{
    *d = MB_TOUPPER(c);
}

    ساکن خالی
do_زیریں(صحیح *d, صحیح c)
{
    *d = MB_TOLOWER(c);
}

/*
 * regtilde(): Replace tildes in the pattern by the old pattern.
 *
 * Short explanation of the tilde: It stands for the previous replacement
 * pattern.  If that previous pattern also contains a ~ we should go back a
 * step further...  But we insert the previous pattern into the current one
 * and remember that.
 * This still does not handle the case where "magic" changes.  So require the
 * user to keep his hands off of "magic".
 *
 * The tildes are parsed once before the first call to vim_regsub().
 */
    char_u *
regtilde(char_u *ماخذ, صحیح magic)
{
    char_u	*newsub = ماخذ;
    char_u	*p;
    size_t	newsublen = 0;
    char_u	tilde[3] = {'~', NUL, NUL};
    size_t	tildelen = 1;
    صحیح		error = FALSE;

    اگر (!magic)
    {
	tilde[0] = '\\';
	tilde[1] = '~';
	tilde[2] = NUL;
	tildelen = 2;
    }

    برائے (p = newsub; *p; ++p)
    {
	اگر (STRNCMP(p, tilde, tildelen) == 0)
	{
	    size_t prefixlen = p - newsub;		// not including the tilde
	    char_u *postfix = p + tildelen;
	    size_t postfixlen;
	    size_t tmpsublen;

	    اگر (newsublen == 0)
		newsublen = STRLEN(newsub);
	    newsublen -= tildelen;
	    postfixlen = newsublen - prefixlen;
	    tmpsublen = prefixlen + reg_پچھلا_sublen + postfixlen;

	    اگر (tmpsublen > 0 && reg_پچھلا_sub != تہی)
	    {
		char_u *tmpsub;

		// Avoid making the text longer than MAXCOL, it will cause
		// trouble at some point.
		اگر (tmpsublen > MAXCOL)
		{
		    emsg(_(e_resulting_متن_too_long));
		    error = TRUE;
		    توڑو;
		}

		tmpsub = alloc(tmpsublen + 1);
		اگر (tmpsub == تہی)
		{
		    emsg(_(e_برآمد_کا_حافظہ));
		    error = TRUE;
		    توڑو;
		}

		// copy prefix
		mch_memmove(tmpsub, newsub, prefixlen);
		// interpret tilde
		mch_memmove(tmpsub + prefixlen, reg_پچھلا_sub, reg_پچھلا_sublen);
		// copy postfix
		STRCPY(tmpsub + prefixlen + reg_پچھلا_sublen, postfix);

		اگر (newsub != ماخذ)	// allocated newsub before
		    vim_free(newsub);
		newsub = tmpsub;
		newsublen = tmpsublen;
		p = newsub + prefixlen + reg_پچھلا_sublen;
	    }
	    ورنہ
		mch_memmove(p, postfix, postfixlen + 1);	// remove the tilde (+1 for the NUL)

	    --p;
	}
	ورنہ
	{
	    اگر (*p == '\\' && p[1])		// skip escaped characters
		++p;
	    اگر (has_mbyte)
		p += (*mb_ptr2len)(p) - 1;
	}
    }

    اگر (error)
    {
	اگر (newsub != ماخذ)
	    vim_free(newsub);
	واپس ماخذ;
    }

    // Store a copy of newsub  in reg_prev_sub.  It is always allocated,
    // because recursive calls may make the returned string invalid.
    // Only store it if there something to store.
    newsublen = p - newsub;
    اگر (newsublen == 0)
	VIM_CLEAR(reg_پچھلا_sub);
    ورنہ
    {
	vim_free(reg_پچھلا_sub);
	reg_پچھلا_sub = vim_strnsave(newsub, newsublen);
    }

    اگر (reg_پچھلا_sub == تہی)
	reg_پچھلا_sublen = 0;
    ورنہ
	reg_پچھلا_sublen = newsublen;

    واپس newsub;
}

#اگر_تعریف_شدہ FEAT_EVAL

/*
 * Put the submatches in "argv[argskip]" which is a list passed into
 * call_func() by vim_regsub_both().
 */
    ساکن صحیح
fill_submatch_list(صحیح argc غیر_مستعمل, typval_T *argv, صحیح argskip, ufunc_T *fp)
{
    listitem_T	*li;
    صحیح		i;
    char_u	*s;
    typval_T	*listarg = argv + argskip;

    اگر (!has_varargs(fp) && fp->uf_دلائل.ga_len <= argskip)
	// called function doesn't take a submatches argument
	واپس argskip;

    // Relies on sl_list to be the first item in staticList10_T.
    init_static_list((staticList10_T *)(listarg->vval.v_list));

    // There are always 10 list items in staticList10_T.
    li = listarg->vval.v_list->lv_پہلا;
    برائے (i = 0; i < 10; ++i)
    {
	s = rsm.sm_مطابقت->startp[i];
	اگر (s == تہی || rsm.sm_مطابقت->endp[i] == تہی)
	    s = تہی;
	ورنہ
	    s = vim_strnsave(s, rsm.sm_مطابقت->endp[i] - s);
	li->li_tv.v_type = VAR_STRING;
	li->li_tv.vval.v_string = s;
	li = li->li_اگلا;
    }
    واپس argskip + 1;
}

    ساکن خالی
clear_submatch_list(staticList10_T *sl)
{
    صحیح i;

    برائے (i = 0; i < 10; ++i)
	vim_free(sl->sl_اشیاء[i].li_tv.vval.v_string);
}
#اگر_ختم

/*
 * vim_regsub() - perform substitutions after a vim_regexec() or
 * vim_regexec_multi() match.
 *
 * If "flags" has REGSUB_COPY really copy into "dest[destlen]".
 * Otherwise nothing is copied, only compute the length of the result.
 *
 * If "flags" has REGSUB_MAGIC then behave like 'magic' is set.
 *
 * If "flags" has REGSUB_BACKSLASH a backslash will be removed later, need to
 * double them to keep them, and insert a backslash before a CR to avoid it
 * being replaced with a line break later.
 *
 * Note: The matched text must not change between the call of
 * vim_regexec()/vim_regexec_multi() and vim_regsub()!  It would make the back
 * references invalid!
 *
 * Returns the size of the replacement, including terminating NUL.
 */
    صحیح
vim_regsub(
    regmatch_T	*rmp,
    char_u	*ماخذ,
    typval_T	*اظہاریہ,
    char_u	*dest,
    صحیح		destlen,
    صحیح		flags)
{
    صحیح		نتیجہ;
    regexec_T	rex_save;
    صحیح		rex_اندر_use_save = rex_اندر_use;

    اگر (rex_اندر_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_اندر_use = TRUE;

    rex.reg_مطابقت = rmp;
    rex.reg_mmatch = تہی;
    rex.reg_maxline = 0;
    rex.reg_buf = curbuf;
    rex.reg_سطر_lbr = TRUE;
    نتیجہ = vim_regsub_both(ماخذ, اظہاریہ, dest, destlen, flags);

    rex_اندر_use = rex_اندر_use_save;
    اگر (rex_اندر_use)
	rex = rex_save;

    واپس نتیجہ;
}

    صحیح
vim_regsub_multi(
    regmmatch_T	*rmp,
    linenr_T	lnum,
    char_u	*ماخذ,
    char_u	*dest,
    صحیح		destlen,
    صحیح		flags)
{
    صحیح		نتیجہ;
    regexec_T	rex_save;
    صحیح		rex_اندر_use_save = rex_اندر_use;

    اگر (rex_اندر_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_اندر_use = TRUE;

    rex.reg_مطابقت = تہی;
    rex.reg_mmatch = rmp;
    rex.reg_buf = curbuf;	// always works on the current buffer!
    rex.reg_firstlnum = lnum;
    rex.reg_maxline = curbuf->b_ml.ml_سطر_گنتی - lnum;
    rex.reg_سطر_lbr = FALSE;
    نتیجہ = vim_regsub_both(ماخذ, تہی, dest, destlen, flags);

    rex_اندر_use = rex_اندر_use_save;
    اگر (rex_اندر_use)
	rex = rex_save;

    واپس نتیجہ;
}

#اگر defined(FEAT_EVAL)
// When nesting more than a couple levels it's probably a mistake.
# define MAX_REGSUB_NESTING 4
ساکن char_u   *eval_نتیجہ[MAX_REGSUB_NESTING] = {تہی, تہی, تہی, تہی};

# اگر defined(EXITFREE)
    خالی
free_resub_eval_نتیجہ(خالی)
{
    صحیح i;

    برائے (i = 0; i < MAX_REGSUB_NESTING; ++i)
	VIM_CLEAR(eval_نتیجہ[i]);
}
# endif
#اگر_ختم

    ساکن صحیح
vim_regsub_both(
    char_u	*ماخذ,
    typval_T	*اظہاریہ,
    char_u	*dest,
    صحیح		destlen,
    صحیح		flags)
{
    char_u	*src;
    char_u	*dst;
    char_u	*s;
    صحیح		c;
    صحیح		cc;
    صحیح		no = -1;
    fptr_T	func_all = (fptr_T)تہی;
    fptr_T	func_one = (fptr_T)تہی;
    linenr_T	clnum = 0;	// init for GCC
    صحیح		len = 0;	// init for GCC
#اگر_تعریف_شدہ FEAT_EVAL
    ساکن صحیح  nesting = 0;
    صحیح		nested;
#اگر_ختم
    صحیح		copy = flags & REGSUB_COPY;

    // Be paranoid...
    اگر ((ماخذ == تہی && اظہاریہ == تہی) || dest == تہی)
    {
	iemsg(e_null_دلیل);
	واپس 0;
    }
    اگر (prog_magic_wrong())
	واپس 0;
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (nesting == MAX_REGSUB_NESTING)
    {
	emsg(_(e_substitute_nesting_too_deep));
	واپس 0;
    }
    nested = nesting;
#اگر_ختم
    src = ماخذ;
    dst = dest;

    /*
     * When the substitute part starts with "\=" evaluate it as an expression.
     */
    اگر (اظہاریہ != تہی || (ماخذ[0] == '\\' && ماخذ[1] == '='))
    {
#اگر_تعریف_شدہ FEAT_EVAL
	// To make sure that the length doesn't change between checking the
	// length and copying the string, and to speed up things, the
	// resulting string is saved from the call with
	// "flags & REGSUB_COPY" == 0 to the call with
	// "flags & REGSUB_COPY" != 0.
	اگر (copy)
	{
	    اگر (eval_نتیجہ[nested] != تہی)
	    {
		صحیح eval_len = (صحیح)STRLEN(eval_نتیجہ[nested]);

		اگر (eval_len < destlen)
		{
		    STRCPY(dest, eval_نتیجہ[nested]);
		    dst += eval_len;
		    VIM_CLEAR(eval_نتیجہ[nested]);
		}
	    }
	}
	ورنہ
	{
	    صحیح		    پچھلا_can_f_submatch = can_f_submatch;
	    regsubmatch_T   rsm_save;

	    VIM_CLEAR(eval_نتیجہ[nested]);

	    // The expression may contain substitute(), which calls us
	    // recursively.  Make sure submatch() gets the text from the first
	    // level.
	    اگر (can_f_submatch)
		rsm_save = rsm;
	    can_f_submatch = TRUE;
	    rsm.sm_مطابقت = rex.reg_مطابقت;
	    rsm.sm_mmatch = rex.reg_mmatch;
	    rsm.sm_firstlnum = rex.reg_firstlnum;
	    rsm.sm_maxline = rex.reg_maxline;
	    rsm.sm_سطر_lbr = rex.reg_سطر_lbr;

	    // Although unlikely, it is possible that the expression invokes a
	    // substitute command (it might fail, but still).  Therefore keep
	    // an array of eval results.
	    ++nesting;

	    اگر (اظہاریہ != تہی)
	    {
		typval_T	argv[2];
		char_u		buf[NUMBUFLEN];
		typval_T	rettv;
		staticList10_T	مطابقت_List;
		funcexe_T	funcexe;

		rettv.v_type = VAR_STRING;
		rettv.vval.v_string = تہی;
		argv[0].v_type = VAR_LIST;
		argv[0].vval.v_list = &مطابقت_List.sl_list;
		مطابقت_List.sl_list.lv_len = 0;
		CLEAR_میدان(funcexe);
		funcexe.fe_argv_func = fill_submatch_list;
		funcexe.fe_evaluate = TRUE;
		اگر (اظہاریہ->v_type == VAR_FUNC)
		{
		    s = اظہاریہ->vval.v_string;
		    call_func(s, -1, &rettv, 1, argv, &funcexe);
		}
		ورنہ اگر (اظہاریہ->v_type == VAR_PARTIAL)
		{
		    partial_T   *partial = اظہاریہ->vval.v_partial;

		    s = partial_name(partial);
		    funcexe.fe_partial = partial;
		    call_func(s, -1, &rettv, 1, argv, &funcexe);
		}
		ورنہ اگر (اظہاریہ->v_type == VAR_INSTR)
		{
		    exe_typval_instr(اظہاریہ, &rettv);
		}
		اگر (مطابقت_List.sl_list.lv_len > 0)
		    // fill_submatch_list() was called
		    clear_submatch_list(&مطابقت_List);

		اگر (rettv.v_type == VAR_UNKNOWN)
		    // something failed, no need to report another error
		    eval_نتیجہ[nested] = تہی;
		ورنہ
		{
		    eval_نتیجہ[nested] = tv_get_string_buf_chk(&rettv, buf);
		    اگر (eval_نتیجہ[nested] != تہی)
			eval_نتیجہ[nested] = vim_strsave(eval_نتیجہ[nested]);
		}
		clear_tv(&rettv);
	    }
	    ورنہ اگر (substitute_instr != تہی)
		// Execute instructions from ISN_SUBSTITUTE.
		eval_نتیجہ[nested] = exe_substitute_instr();
	    ورنہ
		eval_نتیجہ[nested] = eval_مقصد_string(ماخذ + 2, TRUE, FALSE);
	    --nesting;

	    اگر (eval_نتیجہ[nested] != تہی)
	    {
		صحیح had_backslash = FALSE;

		برائے (s = eval_نتیجہ[nested]; *s != NUL; MB_PTR_ADV(s))
		{
		    // Change NL to CR, so that it becomes a line break,
		    // unless called from vim_regexec_nl().
		    // Skip over a backslashed character.
		    اگر (*s == NL && !rsm.sm_سطر_lbr)
			*s = CAR;
		    ورنہ اگر (*s == '\\' && s[1] != NUL)
		    {
			++s;
			/* Change NL to CR here too, so that this works:
			 * :s/abc\\\ndef/\="aaa\\\nbbb"/  on text:
			 *   abc\
			 *   def
			 * Not when called from vim_regexec_nl().
			 */
			اگر (*s == NL && !rsm.sm_سطر_lbr)
			    *s = CAR;
			had_backslash = TRUE;
		    }
		}
		اگر (had_backslash && (flags & REGSUB_BACKSLASH))
		{
		    // Backslashes will be consumed, need to double them.
		    s = vim_strsave_escaped(eval_نتیجہ[nested], (char_u *)"\\");
		    اگر (s != تہی)
		    {
			vim_free(eval_نتیجہ[nested]);
			eval_نتیجہ[nested] = s;
		    }
		}

		dst += STRLEN(eval_نتیجہ[nested]);
	    }

	    can_f_submatch = پچھلا_can_f_submatch;
	    اگر (can_f_submatch)
		rsm = rsm_save;
	}
#اگر_ختم
    }
    ورنہ
      جب ((c = *src++) != NUL)
      {
	اگر (c == '&' && (flags & REGSUB_MAGIC))
	    no = 0;
	ورنہ اگر (c == '\\' && *src != NUL)
	{
	    اگر (*src == '&' && !(flags & REGSUB_MAGIC))
	    {
		++src;
		no = 0;
	    }
	    ورنہ اگر ('0' <= *src && *src <= '9')
	    {
		no = *src++ - '0';
	    }
	    ورنہ اگر (vim_strchr((char_u *)"uUlLeE", *src))
	    {
		انتخاب (*src++)
		{
		حالت 'u':   func_one = do_بالائی;
			    جاری;
		حالت 'U':   func_all = do_بالائی;
			    جاری;
		حالت 'l':   func_one = do_زیریں;
			    جاری;
		حالت 'L':   func_all = do_زیریں;
			    جاری;
		حالت 'e':
		حالت 'E':   func_one = func_all = (fptr_T)تہی;
			    جاری;
		}
	    }
	}
	اگر (no < 0)	      // Ordinary character.
	{
	    اگر (c == K_SPECIAL && src[0] != NUL && src[1] != NUL)
	    {
		// Copy a special key as-is.
		اگر (copy)
		{
		    اگر (dst + 3 > dest + destlen)
		    {
			iemsg("vim_regsub_both(): not enough space");
			واپس 0;
		    }
		    *dst++ = c;
		    *dst++ = *src++;
		    *dst++ = *src++;
		}
		ورنہ
		{
		    dst += 3;
		    src += 2;
		}
		جاری;
	    }

	    اگر (c == '\\' && *src != NUL)
	    {
		// Check for abbreviations -- webb
		انتخاب (*src)
		{
		    حالت 'r':	c = CAR;	++src;	توڑو;
		    حالت 'n':	c = NL;		++src;	توڑو;
		    حالت 't':	c = TAB;	++src;	توڑو;
		 // Oh no!  \e already has meaning in subst pat :-(
		 // case 'e':   c = ESC;	++src;	break;
		    حالت 'b':	c = Ctrl_H;	++src;	توڑو;

		    // If "backslash" is TRUE the backslash will be removed
		    // later.  Used to insert a literal CR.
		    طےشدہ:	اگر (flags & REGSUB_BACKSLASH)
				{
				    اگر (copy)
				    {
					اگر (dst + 1 > dest + destlen)
					{
					    iemsg("vim_regsub_both(): not enough space");
					    واپس 0;
					}
					*dst = '\\';
				    }
				    ++dst;
				}
				c = *src++;
		}
	    }
	    ورنہ اگر (has_mbyte)
		c = mb_ptr2char(src - 1);

	    // Write to buffer, if copy is set.
	    اگر (func_one != (fptr_T)تہی)
	    {
		func_one(&cc, c);
		func_one = تہی;
	    }
	    ورنہ اگر (func_all != (fptr_T)تہی)
		func_all(&cc, c);
	    ورنہ // just copy
		cc = c;

	    اگر (has_mbyte)
	    {
		صحیح totlen = mb_ptr2len(src - 1);
		صحیح charlen = mb_char2len(cc);

		اگر (copy)
		{
		    اگر (dst + charlen > dest + destlen)
		    {
			iemsg("vim_regsub_both(): not enough space");
			واپس 0;
		    }
		    mb_char2bytes(cc, dst);
		}
		dst += charlen - 1;
		اگر (enc_utf8)
		{
		    صحیح clen = utf_ptr2len(src - 1);

		    // If the character length is shorter than "totlen", there
		    // are composing characters; copy them as-is.
		    اگر (clen < totlen)
		    {
			اگر (copy)
			{
			    اگر (dst + totlen - clen > dest + destlen)
			    {
				iemsg("vim_regsub_both(): not enough space");
				واپس 0;
			    }
			    mch_memmove(dst + 1, src - 1 + clen,
						     (size_t)(totlen - clen));
			}
			dst += totlen - clen;
		    }
		}
		src += totlen - 1;
	    }
	    ورنہ اگر (copy)
	    {
		اگر (dst + 1 > dest + destlen)
		{
		    iemsg("vim_regsub_both(): not enough space");
		    واپس 0;
		}
		*dst = cc;
	    }
	    dst++;
	}
	ورنہ
	{
	    اگر (REG_MULTI)
	    {
		clnum = rex.reg_mmatch->startpos[no].lnum;
		اگر (clnum < 0 || rex.reg_mmatch->endpos[no].lnum < 0)
		    s = تہی;
		ورنہ
		{
		    s = reg_getline(clnum) + rex.reg_mmatch->startpos[no].ستون;
		    اگر (rex.reg_mmatch->endpos[no].lnum == clnum)
			len = rex.reg_mmatch->endpos[no].ستون
					    - rex.reg_mmatch->startpos[no].ستون;
		    ورنہ
			len = (صحیح)reg_getline_len(clnum) - rex.reg_mmatch->startpos[no].ستون;
		}
	    }
	    ورنہ
	    {
		s = rex.reg_مطابقت->startp[no];
		اگر (rex.reg_مطابقت->endp[no] == تہی)
		    s = تہی;
		ورنہ
		    len = (صحیح)(rex.reg_مطابقت->endp[no] - s);
	    }
	    اگر (s != تہی)
	    {
		برائے (;;)
		{
		    اگر (len == 0)
		    {
			اگر (REG_MULTI)
			{
			    اگر (rex.reg_mmatch->endpos[no].lnum == clnum)
				توڑو;
			    اگر (copy)
			    {
				اگر (dst + 1 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    واپس 0;
				}
				*dst = CAR;
			    }
			    ++dst;
			    s = reg_getline(++clnum);
			    اگر (rex.reg_mmatch->endpos[no].lnum == clnum)
				len = rex.reg_mmatch->endpos[no].ستون;
			    ورنہ
				len = (صحیح)reg_getline_len(clnum);
			}
			ورنہ
			    توڑو;
		    }
		    ورنہ اگر (*s == NUL) // we hit NUL.
		    {
			اگر (copy)
			    iemsg(e_damaged_مطابقت_string);
			جاؤ باہر_نکلو;
		    }
		    ورنہ
		    {
			اگر ((flags & REGSUB_BACKSLASH)
						  && (*s == CAR || *s == '\\'))
			{
			    /*
			     * Insert a backslash in front of a CR, otherwise
			     * it will be replaced by a line break.
			     * Number of backslashes will be halved later,
			     * double them here.
			     */
			    اگر (copy)
			    {
				اگر (dst + 2 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    واپس 0;
				}
				dst[0] = '\\';
				dst[1] = *s;
			    }
			    dst += 2;
			}
			ورنہ
			{
			    اگر (has_mbyte)
				c = mb_ptr2char(s);
			    ورنہ
				c = *s;

			    اگر (func_one != (fptr_T)تہی)
			    {
				func_one(&cc, c);
				func_one = تہی;
			    }
			    ورنہ اگر (func_all != (fptr_T)تہی)
				func_all(&cc, c);
			    ورنہ // just copy
				cc = c;

			    اگر (has_mbyte)
			    {
				صحیح l;
				صحیح charlen;

				// Copy composing characters separately, one
				// at a time.
				اگر (enc_utf8)
				    l = utf_ptr2len(s) - 1;
				ورنہ
				    l = mb_ptr2len(s) - 1;

				s += l;
				len -= l;
				charlen = mb_char2len(cc);
				اگر (copy)
				{
				    اگر (dst + charlen > dest + destlen)
				    {
					iemsg("vim_regsub_both(): not enough space");
					واپس 0;
				    }
				    mb_char2bytes(cc, dst);
				}
				dst += charlen - 1;
			    }
			    ورنہ اگر (copy)
			    {
				اگر (dst + 1 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    واپس 0;
				}
				*dst = cc;
			    }
			    dst++;
			}

			++s;
			--len;
		    }
		}
	    }
	    no = -1;
	}
      }
    اگر (copy)
	*dst = NUL;

باہر_نکلو:
    واپس (صحیح)((dst - dest) + 1);
}

#اگر_تعریف_شدہ FEAT_EVAL

    ساکن char_u *
reg_getline_submatch(linenr_T lnum)
{
    char_u *سطر;

    reg_getline_common(lnum, RGLF_سطر | RGLF_SUBMATCH, &سطر, تہی);

    واپس سطر;
}

    ساکن colnr_T
reg_getline_submatch_len(linenr_T lnum)
{
    colnr_T لمبائی;

    reg_getline_common(lnum, RGLF_لمبائی | RGLF_SUBMATCH, تہی, &لمبائی);

    واپس لمبائی;
}

/*
 * Used for the submatch() function: get the string from the n'th submatch in
 * allocated memory.
 * Returns NULL when not in a ":s" command and for a non-existing submatch.
 */
    char_u *
reg_submatch(صحیح no)
{
    char_u	*retval = تہی;
    char_u	*s;
    صحیح		len;
    صحیح		باری;
    linenr_T	lnum;

    اگر (!can_f_submatch || no < 0)
	واپس تہی;

    اگر (rsm.sm_مطابقت == تہی)
    {
	/*
	 * First round: compute the length and allocate memory.
	 * Second round: copy the text.
	 */
	برائے (باری = 1; باری <= 2; ++باری)
	{
	    lnum = rsm.sm_mmatch->startpos[no].lnum;
	    اگر (lnum < 0 || rsm.sm_mmatch->endpos[no].lnum < 0)
		واپس تہی;

	    s = reg_getline_submatch(lnum);
	    اگر (s == تہی)  // anti-crash check, cannot happen?
		توڑو;
	    s += rsm.sm_mmatch->startpos[no].ستون;
	    اگر (rsm.sm_mmatch->endpos[no].lnum == lnum)
	    {
		// Within one line: take form start to end col.
		len = rsm.sm_mmatch->endpos[no].ستون
					  - rsm.sm_mmatch->startpos[no].ستون;
		اگر (باری == 2)
		    vim_strncpy(retval, s, len);
		++len;
	    }
	    ورنہ
	    {
		// Multiple lines: take start line from start col, middle
		// lines completely and end line up to end col.
		len = (صحیح)reg_getline_submatch_len(lnum) - rsm.sm_mmatch->startpos[no].ستون;
		اگر (باری == 2)
		{
		    STRCPY(retval, s);
		    retval[len] = '\n';
		}
		++len;
		++lnum;
		جب (lnum < rsm.sm_mmatch->endpos[no].lnum)
		{
		    s = reg_getline_submatch(lnum);
		    اگر (باری == 2)
			STRCPY(retval + len, s);
		    len += (صحیح)reg_getline_submatch_len(lnum);
		    اگر (باری == 2)
			retval[len] = '\n';
		    ++len;
		    ++lnum;
		}
		اگر (باری == 2)
		    STRNCPY(retval + len, reg_getline_submatch(lnum),
					     rsm.sm_mmatch->endpos[no].ستون);
		len += rsm.sm_mmatch->endpos[no].ستون;
		اگر (باری == 2)
		    retval[len] = NUL;
		++len;
	    }

	    اگر (retval == تہی)
	    {
		retval = alloc(len);
		اگر (retval == تہی)
		    واپس تہی;
	    }
	}
    }
    ورنہ
    {
	s = rsm.sm_مطابقت->startp[no];
	اگر (s == تہی || rsm.sm_مطابقت->endp[no] == تہی)
	    retval = تہی;
	ورنہ
	    retval = vim_strnsave(s, rsm.sm_مطابقت->endp[no] - s);
    }

    واپس retval;
}

/*
 * Used for the submatch() function with the optional non-zero argument: get
 * the list of strings from the n'th submatch in allocated memory with NULs
 * represented in NLs.
 * Returns a list of allocated strings.  Returns NULL when not in a ":s"
 * command, for a non-existing submatch and for any error.
 */
    list_T *
reg_submatch_list(صحیح no)
{
    char_u	*s;
    linenr_T	slnum;
    linenr_T	elnum;
    colnr_T	scol;
    colnr_T	ecol;
    صحیح		i;
    list_T	*list;
    صحیح		error = FALSE;

    اگر (!can_f_submatch || no < 0)
	واپس تہی;

    اگر (rsm.sm_مطابقت == تہی)
    {
	slnum = rsm.sm_mmatch->startpos[no].lnum;
	elnum = rsm.sm_mmatch->endpos[no].lnum;
	اگر (slnum < 0 || elnum < 0)
	    واپس تہی;

	scol = rsm.sm_mmatch->startpos[no].ستون;
	ecol = rsm.sm_mmatch->endpos[no].ستون;

	list = list_alloc();
	اگر (list == تہی)
	    واپس تہی;

	s = reg_getline_submatch(slnum) + scol;
	اگر (slnum == elnum)
	{
	    اگر (list_append_string(list, s, ecol - scol) == FAIL)
		error = TRUE;
	}
	ورنہ
	{
	    صحیح max_lnum = elnum - slnum;

	    اگر (list_append_string(list, s, -1) == FAIL)
		error = TRUE;
	    برائے (i = 1; i < max_lnum; i++)
	    {
		s = reg_getline_submatch(slnum + i);
		اگر (list_append_string(list, s, -1) == FAIL)
		    error = TRUE;
	    }
	    s = reg_getline_submatch(elnum);
	    اگر (list_append_string(list, s, ecol) == FAIL)
		error = TRUE;
	}
    }
    ورنہ
    {
	s = rsm.sm_مطابقت->startp[no];
	اگر (s == تہی || rsm.sm_مطابقت->endp[no] == تہی)
	    واپس تہی;
	list = list_alloc();
	اگر (list == تہی)
	    واپس تہی;
	اگر (list_append_string(list, s,
				 (صحیح)(rsm.sm_مطابقت->endp[no] - s)) == FAIL)
	    error = TRUE;
    }

    اگر (error)
    {
	list_free(list);
	واپس تہی;
    }
    ++list->lv_refcount;
    واپس list;
}
#اگر_ختم

/*
 * Initialize the values used for matching against multiple lines
 */
    ساکن خالی
init_regexec_multi(
	regmmatch_T	*rmp,
	win_T		*win,	// window in which to search or NULL
	buf_T		*buf,	// buffer in which to search
	linenr_T	lnum)	// nr of line to start looking for match
{
    rex.reg_مطابقت = تہی;
    rex.reg_mmatch = rmp;
    rex.reg_buf = buf;
    rex.reg_win = win;
    rex.reg_firstlnum = lnum;
    rex.reg_maxline = rex.reg_buf->b_ml.ml_سطر_گنتی - lnum;
    rex.reg_سطر_lbr = FALSE;
    rex.reg_ic = rmp->rmm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = rmp->rmm_maxcol;
}

#تعریف_کرو CLASS_not		0x80
#تعریف_کرو CLASS_af		0x40
#تعریف_کرو CLASS_AF		0x20
#تعریف_کرو CLASS_az		0x10
#تعریف_کرو CLASS_AZ		0x08
#تعریف_کرو CLASS_o7		0x04
#تعریف_کرو CLASS_o9		0x02
#تعریف_کرو CLASS_underscore	0x01

/*
 * Parse the "[...]" collection between "start" (the first character after the
 * "[") and "end" (the "]") into a combination of CLASS_ bits, so that it can
 * be turned into a faster character class like \d or \x.  Sets "*newl" to TRUE
 * when the collection also matches a newline.  Returns -1 when the collection
 * is not a plain combination of the recognized ranges.
 */
    ساکن صحیح
get_char_class_بٹ(char_u *آغاز, char_u *انتہا, صحیح *newl)
{
    char_u	*p = آغاز;
    صحیح		config = 0;

    *newl = FALSE;
    اگر (*انتہا != ']')
	واپس -1;
    اگر (*p == '^')
    {
	config |= CLASS_not;
	p++;
    }

    جب (p < انتہا)
    {
	اگر (p + 2 < انتہا && *(p + 1) == '-')
	{
	    انتخاب (*p)
	    {
		حالت '0':
		    اگر (*(p + 2) == '9')
		    {
			config |= CLASS_o9;
			توڑو;
		    }
		    اگر (*(p + 2) == '7')
		    {
			config |= CLASS_o7;
			توڑو;
		    }
		    واپس -1;

		حالت 'a':
		    اگر (*(p + 2) == 'z')
		    {
			config |= CLASS_az;
			توڑو;
		    }
		    اگر (*(p + 2) == 'f')
		    {
			config |= CLASS_af;
			توڑو;
		    }
		    واپس -1;

		حالت 'A':
		    اگر (*(p + 2) == 'Z')
		    {
			config |= CLASS_AZ;
			توڑو;
		    }
		    اگر (*(p + 2) == 'F')
		    {
			config |= CLASS_AF;
			توڑو;
		    }
		    واپس -1;

		طےشدہ:
		    واپس -1;
	    }
	    p += 3;
	}
	ورنہ اگر (p + 1 < انتہا && *p == '\\' && *(p + 1) == 'n')
	{
	    *newl = TRUE;
	    p += 2;
	}
	ورنہ اگر (*p == '_')
	{
	    config |= CLASS_underscore;
	    p++;
	}
	ورنہ اگر (*p == '\n')
	{
	    *newl = TRUE;
	    p++;
	}
	ورنہ
	    واپس -1;
    } // while (p < end)

    اگر (p != انتہا)
	واپس -1;

    // U+017F and U+212A fold to ASCII 's' and 'k', missed by the class opcodes
    اگر (enc_utf8 && (config & (CLASS_az | CLASS_AZ)))
	واپس -1;

    واپس config;
}

#شامل "regexp_bt.c"

ساکن regengine_T bt_regengine =
{
    bt_regcomp,
    bt_regfree,
    bt_regexec_nl,
    bt_regexec_multi
#اگر_تعریف_شدہ DEBUG
    ,(char_u *)""
#اگر_ختم
};

#شامل "regexp_nfa.c"

ساکن regengine_T nfa_regengine =
{
    nfa_regcomp,
    nfa_regfree,
    nfa_regexec_nl,
    nfa_regexec_multi
#اگر_تعریف_شدہ DEBUG
    ,(char_u *)""
#اگر_ختم
};

// Which regexp engine to use? Needed for vim_regcomp().
// Must match with 'regexpengine'.
ساکن صحیح regexp_engine = 0;

#اگر_تعریف_شدہ DEBUG
ساکن ثابت char_u regname[][30] = {
		    "AUTOMATIC Regexp Engine",
		    "BACKTRACKING Regexp Engine",
		    "NFA Regexp Engine"
			    };
#اگر_ختم

/*
 * Compile a regular expression into internal code.
 * Returns the program in allocated memory.
 * Use vim_regfree() to free the memory.
 * Returns NULL for an error.
 */
    regprog_T *
vim_regcomp(char_u *اظہاریہ_arg, صحیح re_flags)
{
    regprog_T   *prog = تہی;
    char_u	*اظہاریہ = اظہاریہ_arg;
    صحیح		called_emsg_before;

    regexp_engine = p_re;

    // Check for prefix "\%#=", that sets the regexp engine
    اگر (STRNCMP(اظہاریہ, "\\%#=", 4) == 0)
    {
	صحیح newengine = اظہاریہ[4] - '0';

	اگر (newengine == AUTOMATIC_ENGINE
	    || newengine == BACKTRACKING_ENGINE
	    || newengine == NFA_ENGINE)
	{
	    regexp_engine = اظہاریہ[4] - '0';
	    اظہاریہ += 5;
#اگر_تعریف_شدہ DEBUG
	    smsg("New regexp mode selected (%d): %s",
					   regexp_engine, regname[newengine]);
#اگر_ختم
	}
	ورنہ
	{
	    emsg(_(e_percent_hash_can_only_be_followed_by_zero_one_two_automatic_engine_will_be_مستعمل));
	    regexp_engine = AUTOMATIC_ENGINE;
	}
    }
#اگر_تعریف_شدہ DEBUG
    bt_regengine.اظہاریہ = اظہاریہ;
    nfa_regengine.اظہاریہ = اظہاریہ;
#اگر_ختم
    // reg_iswordc() uses rex.reg_buf
    rex.reg_buf = curbuf;

    /*
     * First try the NFA engine, unless backtracking was requested.
     */
    called_emsg_before = called_emsg;
    اگر (regexp_engine != BACKTRACKING_ENGINE)
	prog = nfa_regengine.regcomp(اظہاریہ,
		re_flags + (regexp_engine == AUTOMATIC_ENGINE ? RE_AUTO : 0));
    ورنہ
	prog = bt_regengine.regcomp(اظہاریہ, re_flags);

    // Check for error compiling regexp with initial engine.
    اگر (prog == تہی)
    {
#اگر_تعریف_شدہ BT_REGEXP_DEBUG_LOG
	اگر (regexp_engine == BACKTRACKING_ENGINE)   // debugging log for BT engine
	{
	    FILE *f;
	    f = fopen(BT_REGEXP_DEBUG_LOG_NAME, "a");
	    اگر (f)
	    {
		fprintf(f, "Syntax error in \"%s\"\n", اظہاریہ);
		fclose(f);
	    }
	    ورنہ
		semsg("(NFA) Could not open \"%s\" to write !!!",
			BT_REGEXP_DEBUG_LOG_NAME);
	}
#اگر_ختم
	/*
	 * If the NFA engine failed, try the backtracking engine.
	 * The NFA engine also fails for patterns that it can't handle well
	 * but are still valid patterns, thus a retry should work.
	 * But don't try if an error message was given.
	 */
	اگر (regexp_engine == AUTOMATIC_ENGINE
					  && called_emsg == called_emsg_before)
	{
	    regexp_engine = BACKTRACKING_ENGINE;
#اگر_تعریف_شدہ FEAT_EVAL
	    report_re_switch(اظہاریہ);
#اگر_ختم
	    prog = bt_regengine.regcomp(اظہاریہ, re_flags);
	}
    }

    اگر (prog != تہی)
    {
	// Store the info needed to call regcomp() again when the engine turns
	// out to be very slow when executing it.
	prog->re_engine = regexp_engine;
	prog->re_flags  = re_flags;
    }

    واپس prog;
}

/*
 * Free a compiled regexp program, returned by vim_regcomp().
 */
    خالی
vim_regfree(regprog_T *prog)
{
    اگر (prog != تہی)
	prog->engine->regfree(prog);
}

#اگر defined(EXITFREE)
    خالی
free_regexp_stuff(خالی)
{
    ga_clear(&regstack);
    ga_clear(&backpos);
    vim_free(reg_tofree);
    vim_free(reg_پچھلا_sub);
    vim_free(post_آغاز);   // NFA postfix buffer, reused across compilations
    post_آغاز = تہی;
    post_آغاز_len = 0;
    vim_free(nfa_ڈھیر);    // NFA fragment stack, reused across compilations
    nfa_ڈھیر = تہی;
    nfa_ڈھیر_len = 0;
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_EVAL
    ساکن خالی
report_re_switch(char_u *pat)
{
    اگر (p_verbose > 0)
    {
	verbose_enter();
	پیغام_puts(_("Switching to backtracking RE engine for pattern: "));
	پیغام_puts((حرف *)pat);
	verbose_leave();
    }
}
#اگر_ختم

#اگر defined(FEAT_X11)
/*
 * Return whether "prog" is currently being executed.
 */
    صحیح
regprog_اندر_use(regprog_T *prog)
{
    واپس prog->re_اندر_use;
}
#اگر_ختم

/*
 * Match a regexp against a string.
 * "rmp->regprog" must be a compiled regexp as returned by vim_regcomp().
 * Note: "rmp->regprog" may be freed and changed.
 * Uses curbuf for line count and 'iskeyword'.
 * When "nl" is TRUE consider a "\n" in "line" to be a line break.
 *
 * Return TRUE if there is a match, FALSE if not.
 */
    ساکن صحیح
vim_regexec_string(
    regmatch_T	*rmp,
    char_u	*سطر,  // string to match against
    colnr_T	ستون,    // column to start looking for match
    صحیح		nl)
{
    صحیح		نتیجہ;
    regexec_T	rex_save;
    صحیح		rex_اندر_use_save = rex_اندر_use;

    // Cannot use the same prog recursively, it contains state.
    اگر (rmp->regprog->re_اندر_use)
    {
	emsg(_(e_cannot_use_نمونہ_recursively));
	واپس FALSE;
    }
    rmp->regprog->re_اندر_use = TRUE;

    اگر (rex_اندر_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_اندر_use = TRUE;

    rex.reg_startp = تہی;
    rex.reg_endp = تہی;
    rex.reg_startpos = تہی;
    rex.reg_endpos = تہی;

    نتیجہ = rmp->regprog->engine->regexec_nl(rmp, سطر, ستون, nl);
    rmp->regprog->re_اندر_use = FALSE;

    // NFA engine aborted because it's very slow.
    اگر (rmp->regprog->re_engine == AUTOMATIC_ENGINE
					       && نتیجہ == NFA_TOO_EXPENSIVE)
    {
	صحیح    save_p_re = p_re;
	صحیح    re_flags = rmp->regprog->re_flags;
	char_u *pat = vim_strsave(((nfa_regprog_T *)rmp->regprog)->نمونہ);

	p_re = BACKTRACKING_ENGINE;
	اگر (pat != تہی)
	{
	    regprog_T *پچھلا_prog = rmp->regprog;

#اگر_تعریف_شدہ FEAT_EVAL
	    report_re_switch(pat);
#اگر_ختم
	    rmp->regprog = vim_regcomp(pat, re_flags);
	    اگر (rmp->regprog == تہی)
	    {
		// Somehow compiling the pattern failed now, put back the
		// previous one to avoid "regprog" becoming NULL.
		rmp->regprog = پچھلا_prog;
	    }
	    ورنہ
	    {
		vim_regfree(پچھلا_prog);
		rmp->regprog->re_اندر_use = TRUE;
		نتیجہ = rmp->regprog->engine->regexec_nl(rmp, سطر, ستون, nl);
		rmp->regprog->re_اندر_use = FALSE;
	    }
	    vim_free(pat);
	}

	p_re = save_p_re;
    }

    rex_اندر_use = rex_اندر_use_save;
    اگر (rex_اندر_use)
	rex = rex_save;

    واپس نتیجہ > 0;
}

#اگر defined(FEAT_SPELL) || defined(FEAT_EVAL) || defined(FEAT_X11)
/*
 * Note: "*prog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    صحیح
vim_regexec_prog(
    regprog_T	**prog,
    صحیح		ignore_case,
    char_u	*سطر,
    colnr_T	ستون)
{
    صحیح		r;
    regmatch_T	regmatch;

    regmatch.regprog = *prog;
    regmatch.rm_ic = ignore_case;
    r = vim_regexec_string(&regmatch, سطر, ستون, FALSE);
    *prog = regmatch.regprog;
    واپس r;
}
#اگر_ختم

/*
 * Note: "rmp->regprog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    صحیح
vim_regexec(regmatch_T *rmp, char_u *سطر, colnr_T ستون)
{
    واپس vim_regexec_string(rmp, سطر, ستون, FALSE);
}

/*
 * Like vim_regexec(), but consider a "\n" in "line" to be a line break.
 * Note: "rmp->regprog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    صحیح
vim_regexec_nl(regmatch_T *rmp, char_u *سطر, colnr_T ستون)
{
    واپس vim_regexec_string(rmp, سطر, ستون, TRUE);
}

/*
 * Match a regexp against multiple lines.
 * "rmp->regprog" must be a compiled regexp as returned by vim_regcomp().
 * Note: "rmp->regprog" may be freed and changed, even set to NULL.
 * Uses curbuf for line count and 'iskeyword'.
 *
 * Return zero if there is no match.  Return number of lines contained in the
 * match otherwise.
 */
    طویل
vim_regexec_multi(
    regmmatch_T *rmp,
    win_T       *win,		// window in which to search or NULL
    buf_T       *buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	ستون,		// column to start looking for match
    صحیح		*timed_برآمد)	// flag is set when timeout limit reached
{
    صحیح		نتیجہ;
    regexec_T	rex_save;
    صحیح		rex_اندر_use_save = rex_اندر_use;

    // Cannot use the same prog recursively, it contains state.
    اگر (rmp->regprog->re_اندر_use)
    {
	emsg(_(e_cannot_use_نمونہ_recursively));
	واپس FALSE;
    }
    rmp->regprog->re_اندر_use = TRUE;

    اگر (rex_اندر_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_اندر_use = TRUE;

    نتیجہ = rmp->regprog->engine->regexec_multi(
				      rmp, win, buf, lnum, ستون, timed_برآمد);
    rmp->regprog->re_اندر_use = FALSE;

    // NFA engine aborted because it's very slow.
    اگر (rmp->regprog->re_engine == AUTOMATIC_ENGINE
					       && نتیجہ == NFA_TOO_EXPENSIVE)
    {
	صحیح    save_p_re = p_re;
	صحیح    re_flags = rmp->regprog->re_flags;
	char_u *pat = vim_strsave(((nfa_regprog_T *)rmp->regprog)->نمونہ);

	p_re = BACKTRACKING_ENGINE;
	اگر (pat != تہی)
	{
	    regprog_T *پچھلا_prog = rmp->regprog;

#اگر_تعریف_شدہ FEAT_EVAL
	    report_re_switch(pat);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SYN_HL
	    // checking for \z misuse was already done when compiling for NFA,
	    // allow all here
	    reg_do_extmatch = REX_ALL;
#اگر_ختم
	    rmp->regprog = vim_regcomp(pat, re_flags);
#اگر_تعریف_شدہ FEAT_SYN_HL
	    reg_do_extmatch = 0;
#اگر_ختم
	    اگر (rmp->regprog == تہی)
	    {
		// Somehow compiling the pattern failed now, put back the
		// previous one to avoid "regprog" becoming NULL.
		rmp->regprog = پچھلا_prog;
	    }
	    ورنہ
	    {
		vim_regfree(پچھلا_prog);

		rmp->regprog->re_اندر_use = TRUE;
		نتیجہ = rmp->regprog->engine->regexec_multi(
				      rmp, win, buf, lnum, ستون, timed_برآمد);
		rmp->regprog->re_اندر_use = FALSE;
	    }
	    vim_free(pat);
	}
	p_re = save_p_re;
    }

    rex_اندر_use = rex_اندر_use_save;
    اگر (rex_اندر_use)
	rex = rex_save;

    واپس نتیجہ <= 0 ? 0 : نتیجہ;
}
