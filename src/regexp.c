/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * Handling of regular expressions: vim_regcomp(), vim_regexec(), vim_regsub()
 */

// By default: do not create debugging logs or files related to regular
// expressions, even when compiling with -DDEBUG.
// Uncomment the second line to get the regexp debugging.
#وصف_رد DEBUG
// #define DEBUG

#ضمن "vim.h"

#جيڪڏهن_وصف_ٿيل DEBUG
// show/save debugging data when BT engine is used
# define BT_REGEXP_DUMP
// save the debugging data to a file instead of displaying it
# define BT_REGEXP_LOG
# define BT_REGEXP_DEBUG_LOG
# define BT_REGEXP_DEBUG_LOG_NAME	"bt_regexp_debug.log"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_RELTIME
ساكن sig_atomic_t dummy_timeout_flag = 0;
ساكن متطاير sig_atomic_t *timeout_flag = &dummy_timeout_flag;
#جيڪڏهن_پورو

/*
 * Magic characters have a special meaning, they don't match literally.
 * Magic characters are negative.  This separates them from literal characters
 * (possibly multi-byte).  Only ASCII characters can be Magic.
 */
#وصف_ڪريو Magic(x)	((صحيح)(x) - 256)
#وصف_ڪريو un_Magic(x)	((x) + 256)
#وصف_ڪريو is_Magic(x)	((x) < 0)

    ساكن صحيح
no_Magic(صحيح x)
{
    إذا (is_Magic(x))
	أرجع un_Magic(x);
    أرجع x;
}

    ساكن صحيح
toggle_Magic(صحيح x)
{
    إذا (is_Magic(x))
	أرجع un_Magic(x);
    أرجع Magic(x);
}

#إذا defined(FEAT_RELTIME)
ساكن صحيح timeout_nesting = 0;

/*
 * Start a timer that will cause the regexp to abort after "msec".
 * This doesn't work well recursively.  In case it happens anyway, the first
 * set timeout will prevail, nested ones are ignored.
 * The caller must make sure there is a matching disable_regexp_timeout() call!
 */
    فراغ
init_regexp_timeout(ڊگهو msec)
{
    إذا (timeout_nesting == 0)
	timeout_flag = شروعات_timeout(msec);
    ++timeout_nesting;
}

    فراغ
disable_regexp_timeout(فراغ)
{
    إذا (timeout_nesting == 0)
	iemsg("disable_regexp_timeout() called without active timer");
    وإلا إذا (--timeout_nesting == 0)
    {
	stop_timeout();
	timeout_flag = &dummy_timeout_flag;
    }
}
#جيڪڏهن_پورو

#إذا defined(FEAT_EVAL)
# ifdef FEAT_RELTIME
ساكن sig_atomic_t *saved_timeout_flag;
# endif

/*
 * Used at the debug prompt: disable the timeout so that expression evaluation
 * can used patterns.
 * Must be followed by calling restore_timeout_for_debugging().
 */
    فراغ
save_timeout_for_debugging(فراغ)
{
# ifdef FEAT_RELTIME
    saved_timeout_flag = (sig_atomic_t *)timeout_flag;
    timeout_flag = &dummy_timeout_flag;
# endif
}

    فراغ
restore_timeout_for_debugging(فراغ)
{
# ifdef FEAT_RELTIME
    timeout_flag = saved_timeout_flag;
# endif
}
#جيڪڏهن_پورو

/*
 * The first byte of the BT regexp internal "program" is actually this magic
 * number; the start node begins in the second byte.  It's used to catch the
 * most severe mutilation of the program by the caller.
 */

#وصف_ڪريو REGMAGIC	0234

/*
 * Utility definitions.
 */
#وصف_ڪريو UCHARAT(p)	((صحيح)*(char_u *)(p))

// Used for an error (down from) vim_regcomp(): give the error message, set
// rc_did_emsg and return NULL
#وصف_ڪريو EMSG_RET_NULL(m) أرجع (emsg((m)), rc_did_emsg = TRUE, (فراغ *)عدم)
#وصف_ڪريو IEMSG_RET_NULL(m) أرجع (iemsg((m)), rc_did_emsg = TRUE, (فراغ *)عدم)
#وصف_ڪريو EMSG_RET_FAIL(m) أرجع (emsg((m)), rc_did_emsg = TRUE, FAIL)
#وصف_ڪريو EMSG2_RET_NULL(m, c) أرجع (semsg((ثابت حرف *)(m), (c) ? "" : "\\"), rc_did_emsg = TRUE, (فراغ *)عدم)
#وصف_ڪريو EMSG3_RET_NULL(m, c, a) أرجع (semsg((ثابت حرف *)(m), (c) ? "" : "\\", (a)), rc_did_emsg = TRUE, (فراغ *)عدم)
#وصف_ڪريو EMSG2_RET_FAIL(m, c) أرجع (semsg((ثابت حرف *)(m), (c) ? "" : "\\"), rc_did_emsg = TRUE, FAIL)
#وصف_ڪريو EMSG_ONE_RET_NULL EMSG2_RET_NULL(_(e_invalid_شي_اندريون_str_brackets), reg_magic == MAGIC_ALL)


#وصف_ڪريو MAX_حد_بندي	(32767L << 16L)

#وصف_ڪريو NOT_MULTI	0
#وصف_ڪريو MULTI_ONE	1
#وصف_ڪريو MULTI_MULT	2

// return values for regmatch()
#وصف_ڪريو RA_FAIL		1	// something failed, abort
#وصف_ڪريو RA_CONT		2	// continue in inner loop
#وصف_ڪريو RA_BREAK	3	// break inner loop
#وصف_ڪريو RA_ميلاپ	4	// successful match
#وصف_ڪريو RA_NOMATCH	5	// didn't match

/*
 * Return NOT_MULTI if c is not a "multi" operator.
 * Return MULTI_ONE if c is a single "multi" operator.
 * Return MULTI_MULT if c is a multi "multi" operator.
 */
    ساكن صحيح
re_multi_type(صحيح c)
{
    إذا (c == Magic('@') || c == Magic('=') || c == Magic('?'))
	أرجع MULTI_ONE;
    إذا (c == Magic('*') || c == Magic('+') || c == Magic('{'))
	أرجع MULTI_MULT;
    أرجع NOT_MULTI;
}

ساكن char_u		*reg_prev_sub = عدم;
ساكن size_t		reg_prev_sublen = 0;

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
ساكن char_u REGEXP_INRANGE[] = "]^-n\\";
ساكن char_u REGEXP_ABBR[] = "nrtebdoxuU";

/*
 * Translate '\x' to its control character, except "\n", which is Magic.
 */
    ساكن صحيح
backslash_trans(صحيح c)
{
    بدل (c)
    {
	حالة 'r':   أرجع CAR;
	حالة 't':   أرجع TAB;
	حالة 'e':   أرجع ESC;
	حالة 'b':   أرجع BS;
    }
    أرجع c;
}

تعداد
{
    CLASS_ALNUM = 0,
    CLASS_ALPHA,
    CLASS_BLANK,
    CLASS_CNTRL,
    CLASS_DIGIT,
    CLASS_گراف,
    CLASS_هيٺاهون,
    CLASS_PRINT,
    CLASS_PUNCT,
    CLASS_SPACE,
    CLASS_مٿانهون,
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
    ساكن صحيح
get_char_class(char_u **pp)
{
    // must be sorted by the 'value' field because it is used by bsearch()!
    ساكن keyvalue_T char_class_tab[] =
    {
	KEYVALUE_داخلا(CLASS_ALNUM, "alnum:]"),
	KEYVALUE_داخلا(CLASS_ALPHA, "alpha:]"),
	KEYVALUE_داخلا(CLASS_BACKSPACE, "backspace:]"),
	KEYVALUE_داخلا(CLASS_BLANK, "blank:]"),
	KEYVALUE_داخلا(CLASS_CNTRL, "cntrl:]"),
	KEYVALUE_داخلا(CLASS_DIGIT, "digit:]"),
	KEYVALUE_داخلا(CLASS_ESCAPE, "escape:]"),
	KEYVALUE_داخلا(CLASS_FNAME, "fname:]"),
	KEYVALUE_داخلا(CLASS_گراف, "graph:]"),
	KEYVALUE_داخلا(CLASS_IDENT, "ident:]"),
	KEYVALUE_داخلا(CLASS_KEYWORD, "keyword:]"),
	KEYVALUE_داخلا(CLASS_هيٺاهون, "lower:]"),
	KEYVALUE_داخلا(CLASS_PRINT, "print:]"),
	KEYVALUE_داخلا(CLASS_PUNCT, "punct:]"),
	KEYVALUE_داخلا(CLASS_RETURN, "return:]"),
	KEYVALUE_داخلا(CLASS_SPACE, "space:]"),
	KEYVALUE_داخلا(CLASS_TAB, "tab:]"),
	KEYVALUE_داخلا(CLASS_مٿانهون, "upper:]"),
	KEYVALUE_داخلا(CLASS_XDIGIT, "xdigit:]")
    };

    // check that the value of "pp" has a chance of matching
    إذا ((*pp)[1] == ':' && ASCII_ISLOWER((*pp)[2])
			&& ASCII_ISLOWER((*pp)[3]) && ASCII_ISLOWER((*pp)[4]))
    {
	keyvalue_T هدف;
	keyvalue_T *داخلا;
	// this function can be called repeatedly with the same value for "pp"
	// so we cache the last found entry.
	ساكن keyvalue_T *آخري_داخلا = عدم;

	هدف.ڪنجي = 0;
	هدف.قدر.string = *pp + 2;
	هدف.قدر.ڊيگهائي = 0;	// not used, see cmp_keyvalue_value_n()

	إذا (آخري_داخلا != عدم && cmp_keyvalue_قدر_n(&هدف, آخري_داخلا) == 0)
	    داخلا = آخري_داخلا;
	وإلا
	    داخلا = (keyvalue_T *)bsearch(&هدف, &char_class_tab,
					ARRAY_ڊيگهائي(char_class_tab),
					حجم(char_class_tab[0]), cmp_keyvalue_قدر_n);
	إذا (داخلا != عدم)
	{
	    آخري_داخلا = داخلا;
	    *pp += داخلا->قدر.ڊيگهائي + 2;
	    أرجع داخلا->ڪنجي;
	}
    }
    أرجع CLASS_NONE;
}

/*
 * Specific version of character class functions.
 * Using a table to keep this fast.
 */
ساكن ننڍو	class_tab[256];

#وصف_ڪريو	    RI_DIGIT	0x01
#وصف_ڪريو	    RI_HEX	0x02
#وصف_ڪريو	    RI_OCTAL	0x04
#وصف_ڪريو	    RI_لفظ	0x08
#وصف_ڪريو	    RI_سر	0x10
#وصف_ڪريو	    RI_ALPHA	0x20
#وصف_ڪريو	    RI_هيٺاهون	0x40
#وصف_ڪريو	    RI_مٿانهون	0x80
#وصف_ڪريو	    RI_WHITE	0x100

    ساكن فراغ
init_class_tab(فراغ)
{
    صحيح		i;
    ساكن صحيح	پورو = FALSE;

    إذا (پورو)
	أرجع;

    لكل (i = 0; i < 256; ++i)
    {
	إذا (i >= '0' && i <= '7')
	    class_tab[i] = RI_DIGIT + RI_HEX + RI_OCTAL + RI_لفظ;
	وإلا إذا (i >= '8' && i <= '9')
	    class_tab[i] = RI_DIGIT + RI_HEX + RI_لفظ;
	وإلا إذا (i >= 'a' && i <= 'f')
	    class_tab[i] = RI_HEX + RI_لفظ + RI_سر + RI_ALPHA + RI_هيٺاهون;
	وإلا إذا (i >= 'g' && i <= 'z')
	    class_tab[i] = RI_لفظ + RI_سر + RI_ALPHA + RI_هيٺاهون;
	وإلا إذا (i >= 'A' && i <= 'F')
	    class_tab[i] = RI_HEX + RI_لفظ + RI_سر + RI_ALPHA + RI_مٿانهون;
	وإلا إذا (i >= 'G' && i <= 'Z')
	    class_tab[i] = RI_لفظ + RI_سر + RI_ALPHA + RI_مٿانهون;
	وإلا إذا (i == '_')
	    class_tab[i] = RI_لفظ + RI_سر;
	وإلا
	    class_tab[i] = 0;
    }
    class_tab[' '] |= RI_WHITE;
    class_tab['\t'] |= RI_WHITE;
    پورو = TRUE;
}

#وصف_ڪريو ri_digit(c)	((c) < 0x100 && (class_tab[c] & RI_DIGIT))
#وصف_ڪريو ri_hex(c)	((c) < 0x100 && (class_tab[c] & RI_HEX))
#وصف_ڪريو ri_octal(c)	((c) < 0x100 && (class_tab[c] & RI_OCTAL))
#وصف_ڪريو ri_لفظ(c)	((c) < 0x100 && (class_tab[c] & RI_لفظ))
#وصف_ڪريو ri_سر(c)	((c) < 0x100 && (class_tab[c] & RI_سر))
#وصف_ڪريو ri_alpha(c)	((c) < 0x100 && (class_tab[c] & RI_ALPHA))
#وصف_ڪريو ri_هيٺاهون(c)	((c) < 0x100 && (class_tab[c] & RI_هيٺاهون))
#وصف_ڪريو ri_مٿانهون(c)	((c) < 0x100 && (class_tab[c] & RI_مٿانهون))
#وصف_ڪريو ri_white(c)	((c) < 0x100 && (class_tab[c] & RI_WHITE))

// flags for regflags
#وصف_ڪريو RF_ICASE    1	// ignore case
#وصف_ڪريو RF_NOICASE  2	// don't ignore case
#وصف_ڪريو RF_HASNL    4	// can match a NL
#وصف_ڪريو RF_ICOMBINE 8	// ignore combining characters
#وصف_ڪريو RF_LOOKBH   16	// uses "\@<=" or "\@<!"

/*
 * Global work variables for vim_regcomp().
 */

ساكن char_u	*regparse;	// Input-scan pointer.
ساكن صحيح	regnpar;	// () count.
ساكن صحيح	چاهي_nfa;	// regex should use NFA engine
#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
ساكن صحيح	regnzpar;	// \z() count.
ساكن صحيح	re_has_z;	// \z item detected
#جيڪڏهن_پورو
ساكن بنانشان	regflags;	// RF_ flags for prog
#إذا defined(FEAT_SYN_HL)
ساكن صحيح	had_eol;	// TRUE when EOL found by vim_regcomp()
#جيڪڏهن_پورو

ساكن magic_T	reg_magic;	// magicness of the pattern

ساكن صحيح	reg_string;	// matching with a string instead of a buffer
				//سٽ
ساكن صحيح	reg_strict;	// "[abc" is illegal

/*
 * META contains all characters that may be magic, except '^' and '$'.
 */

// META[] is used often enough to justify turning it into a table.
ساكن ثابت char_u META_flags[] = {
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

ساكن صحيح	curchr;		// currently parsed character
// Previous character.  Note: prevchr is sometimes -1 when we are not at the
// start, eg in /[ ^I]^ the pattern was never found even if it existed,
// because ^ was taken to be magic -- webb
ساكن صحيح	prevchr;
ساكن صحيح	prevprevchr;	// previous-previous character
ساكن صحيح	nextchr;	// used for ungetchr()

// arguments for reg()
#وصف_ڪريو REG_NOPAREN	0	// toplevel reg()
#وصف_ڪريو REG_PAREN	1	// \(\)
#وصف_ڪريو REG_ZPAREN	2	// \z(\)
#وصف_ڪريو REG_NPAREN	3	// \%(\)

// Limit recursive parsing of nested regexp atoms to avoid using up the C stack.
#وصف_ڪريو REG_MAX_PAREN_اونهائي	1000

تعريفنوع بنية
{
    char_u	*regparse;
    صحيح		prevchr_len;
    صحيح		curchr;
    صحيح		prevchr;
    صحيح		prevprevchr;
    صحيح		nextchr;
    صحيح		at_شروعات;
    صحيح		prev_at_شروعات;
    صحيح		regnpar;
} parse_state_T;

ساكن فراغ	initchr(char_u *);
ساكن صحيح	getchr(فراغ);
ساكن فراغ	skipchr_keepstart(فراغ);
ساكن صحيح	peekchr(فراغ);
ساكن فراغ	skipchr(فراغ);
ساكن فراغ	ungetchr(فراغ);
ساكن vimlong_T	gethexchrs(صحيح maxinputlen);
ساكن ڊگهو	getoctchrs(فراغ);
ساكن vimlong_T	getdecchrs(فراغ);
ساكن صحيح	coll_get_char(فراغ);
ساكن صحيح	prog_magic_wrong(فراغ);
ساكن صحيح	cstrncmp(char_u *s1, char_u *s2, صحيح *n);
ساكن char_u	*cstrchr(char_u *, صحيح);
ساكن صحيح	re_mult_ايندڙ(حرف *what);
ساكن صحيح	reg_iswordc(صحيح);
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
ساكن فراغ report_re_switch(char_u *pat);
#جيڪڏهن_پورو

ساكن regengine_T bt_regengine;
ساكن regengine_T nfa_regengine;

/*
 * Return TRUE if compiled regular expression "prog" can match a line break.
 */
    صحيح
re_multiline(regprog_T *prog)
{
    أرجع (prog->regflags & RF_HASNL);
}

/*
 * Check for an equivalence class name "[=a=]".  "pp" points to the '['.
 * Returns a character representing the class. Zero means that no item was
 * recognized.  Otherwise "pp" is advanced to after the item.
 */
    ساكن صحيح
get_equi_class(char_u **pp)
{
    صحيح		c;
    صحيح		l = 1;
    char_u	*p = *pp;

    إذا (p[1] == '=' && p[2] != NUL)
    {
	إذا (has_mbyte)
	    l = (*mb_ptr2len)(p + 2);
	إذا (p[l + 2] == '=' && p[l + 3] == ']')
	{
	    إذا (has_mbyte)
		c = mb_ptr2char(p + 2);
	    وإلا
		c = p[2];
	    *pp += l + 4;
	    أرجع c;
	}
    }
    أرجع 0;
}

/*
 * Check for a collating element "[.a.]".  "pp" points to the '['.
 * Returns a character. Zero means that no item was recognized.  Otherwise
 * "pp" is advanced to after the item.
 * Currently only single characters are recognized!
 */
    ساكن صحيح
get_coll_element(char_u **pp)
{
    صحيح		c;
    صحيح		l = 1;
    char_u	*p = *pp;

    إذا (p[0] != NUL && p[1] == '.' && p[2] != NUL)
    {
	إذا (has_mbyte)
	    l = (*mb_ptr2len)(p + 2);
	إذا (p[l + 2] == '.' && p[l + 3] == ']')
	{
	    إذا (has_mbyte)
		c = mb_ptr2char(p + 2);
	    وإلا
		c = p[2];
	    *pp += l + 4;
	    أرجع c;
	}
    }
    أرجع 0;
}

ساكن صحيح reg_cpo_lit; // 'cpoptions' contains 'l' flag
ساكن صحيح reg_cpo_bsl; // 'cpoptions' contains '\' flag

    ساكن فراغ
get_cpo_flags(فراغ)
{
    reg_cpo_lit = vim_strchr(p_cpo, CPO_LITERAL) != عدم;
    reg_cpo_bsl = vim_strchr(p_cpo, CPO_BACKSL) != عدم;
}

/*
 * Skip over a "[]" range.
 * "p" must point to the character after the '['.
 * The returned pointer is on the matching ']', or the terminating NUL.
 */
    ساكن char_u *
skip_anyof(char_u *p)
{
    صحيح		l;

    إذا (*p == '^')	// Complement of range.
	++p;
    إذا (*p == ']' || *p == '-')
	++p;
    طالما (*p != NUL && *p != ']')
    {
	إذا (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	    p += l;
	وإلا
	    إذا (*p == '-')
	    {
		++p;
		إذا (*p != ']' && *p != NUL)
		    MB_PTR_ADV(p);
	    }
	وإلا إذا (*p == '\\'
		&& !reg_cpo_bsl
		&& (vim_strchr(REGEXP_INRANGE, p[1]) != عدم
		    || (!reg_cpo_lit && vim_strchr(REGEXP_ABBR, p[1]) != عدم)))
	    p += 2;
	وإلا إذا (*p == '[')
	{
	    إذا (get_char_class(&p) == CLASS_NONE
		    && get_equi_class(&p) == 0
		    && get_coll_element(&p) == 0
		    && *p != NUL)
		++p; // it is not a class name and not NUL
	}
	وإلا
	    ++p;
    }

    أرجع p;
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
    صحيح		delim,
    صحيح		magic)
{
    أرجع skip_regexp_ex(startp, delim, magic, عدم, عدم, عدم);
}

/*
 * Call skip_regexp() and when the delimiter does not match give an error and
 * return NULL.
 */
    char_u *
skip_regexp_err(
    char_u	*startp,
    صحيح		delim,
    صحيح		magic)
{
    char_u *p = skip_regexp(startp, delim, magic);

    إذا (*p != delim)
    {
	semsg(_(e_missing_delimiter_after_ڳولا_نمونو_str), startp);
	أرجع عدم;
    }
    أرجع p;
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
    صحيح		dirc,
    صحيح		magic,
    char_u	**newp,
    صحيح		*dropped,
    magic_T	*magic_val)
{
    magic_T	mymagic;
    char_u	*p = startp;
    size_t	startplen = 0;

    إذا (magic)
	mymagic = MAGIC_ON;
    وإلا
	mymagic = MAGIC_OFF;
    get_cpo_flags();

    لكل (; p[0] != NUL; MB_PTR_ADV(p))
    {
	إذا (p[0] == dirc)	// found end of regexp
	    اكسر;
	إذا ((p[0] == '[' && mymagic >= MAGIC_ON)
		|| (p[0] == '\\' && p[1] == '[' && mymagic <= MAGIC_OFF))
	{
	    p = skip_anyof(p + 1);
	    إذا (p[0] == NUL)
		اكسر;
	}
	وإلا إذا (p[0] == '\\' && p[1] != NUL)
	{
	    إذا (dirc == '?' && newp != عدم && p[1] == '?')
	    {
		// change "\?" to "?", make a copy first.
		إذا (startplen == 0)
		    startplen = STRLEN(startp);
		إذا (*newp == عدم)
		{
		    *newp = vim_strnsave(startp, startplen);
		    إذا (*newp != عدم)
		    {
			p = *newp + (p - startp);
			startp = *newp;
		    }
		}
		إذا (dropped != عدم)
		    ++*dropped;
		إذا (*newp != عدم)
		    mch_memmove(p, p + 1, startplen - ((p + 1) - startp) + 1);
		وإلا
		    ++p;
	    }
	    وإلا
		++p;    // skip next character
	    إذا (*p == 'v')
		mymagic = MAGIC_ALL;
	    وإلا إذا (*p == 'V')
		mymagic = MAGIC_NONE;
	}
    }
    إذا (magic_val != عدم)
	*magic_val = mymagic;
    أرجع p;
}

/*
 * Functions for getting characters from the regexp input.
 */
ساكن صحيح	prevchr_len;	// byte length of previous char
ساكن صحيح	at_شروعات;	// True when on the first character
ساكن صحيح	prev_at_شروعات;  // True when on the second character

/*
 * Start parsing at "str".
 */
    ساكن فراغ
initchr(char_u *str)
{
    regparse = str;
    prevchr_len = 0;
    curchr = prevprevchr = prevchr = nextchr = -1;
    at_شروعات = TRUE;
    prev_at_شروعات = FALSE;
}

/*
 * Save the current parse state, so that it can be restored and parsing
 * starts in the same state again.
 */
    ساكن فراغ
save_parse_state(parse_state_T *ps)
{
    ps->regparse = regparse;
    ps->prevchr_len = prevchr_len;
    ps->curchr = curchr;
    ps->prevchr = prevchr;
    ps->prevprevchr = prevprevchr;
    ps->nextchr = nextchr;
    ps->at_شروعات = at_شروعات;
    ps->prev_at_شروعات = prev_at_شروعات;
    ps->regnpar = regnpar;
}

/*
 * Restore a previously saved parse state.
 */
    ساكن فراغ
restore_parse_state(parse_state_T *ps)
{
    regparse = ps->regparse;
    prevchr_len = ps->prevchr_len;
    curchr = ps->curchr;
    prevchr = ps->prevchr;
    prevprevchr = ps->prevprevchr;
    nextchr = ps->nextchr;
    at_شروعات = ps->at_شروعات;
    prev_at_شروعات = ps->prev_at_شروعات;
    regnpar = ps->regnpar;
}


/*
 * Get the next character without advancing.
 */
    ساكن صحيح
peekchr(فراغ)
{
    ساكن صحيح	after_slash = FALSE;

    إذا (curchr != -1)
	أرجع curchr;

    بدل (curchr = regparse[0])
    {
	حالة '.':
	حالة '[':
	حالة '~':
	    // magic when 'magic' is on
	    إذا (reg_magic >= MAGIC_ON)
		curchr = Magic(curchr);
	    اكسر;
	حالة '(':
	حالة ')':
	حالة '{':
	حالة '%':
	حالة '+':
	حالة '=':
	حالة '?':
	حالة '@':
	حالة '!':
	حالة '&':
	حالة '|':
	حالة '<':
	حالة '>':
	حالة '#':	// future ext.
	حالة '"':	// future ext.
	حالة '\'':	// future ext.
	حالة ',':	// future ext.
	حالة '-':	// future ext.
	حالة ':':	// future ext.
	حالة ';':	// future ext.
	حالة '`':	// future ext.
	حالة '/':	// Can't be used in / command
			// magic only after "\v"
	    إذا (reg_magic == MAGIC_ALL)
		curchr = Magic(curchr);
	    اكسر;
	حالة '*':
	    // * is not magic as the very first character, eg "?*ptr", when
	    // after '^', eg "/^*ptr" and when after "\(", "\|", "\&".  But
	    // "\(\*" is not magic, thus must be magic if "after_slash"
	    إذا (reg_magic >= MAGIC_ON
		    && !at_شروعات
		    && !(prev_at_شروعات && prevchr == Magic('^'))
		    && (after_slash
			|| (prevchr != Magic('(')
			    && prevchr != Magic('&')
			    && prevchr != Magic('|'))))
		curchr = Magic('*');
	    اكسر;
	حالة '^':
	    // '^' is only magic as the very first character and if it's after
	    // "\(", "\|", "\&' or "\n"
	    إذا (reg_magic >= MAGIC_OFF
		    && (at_شروعات
			|| reg_magic == MAGIC_ALL
			|| prevchr == Magic('(')
			|| prevchr == Magic('|')
			|| prevchr == Magic('&')
			|| prevchr == Magic('n')
			|| (no_Magic(prevchr) == '('
			    && prevprevchr == Magic('%'))))
	    {
		curchr = Magic('^');
		at_شروعات = TRUE;
		prev_at_شروعات = FALSE;
	    }
	    اكسر;
	حالة '$':
	    // '$' is only magic as the very last char and if it's in front of
	    // either "\|", "\)", "\&", or "\n"
	    إذا (reg_magic >= MAGIC_OFF)
	    {
		char_u *p = regparse + 1;
		صحيح is_magic_all = (reg_magic == MAGIC_ALL);

		// ignore \c \C \m \M \v \V and \Z after '$'
		طالما (p[0] == '\\' && (p[1] == 'c' || p[1] == 'C'
			    || p[1] == 'm' || p[1] == 'M'
			    || p[1] == 'v' || p[1] == 'V' || p[1] == 'Z'))
		{
		    إذا (p[1] == 'v')
			is_magic_all = TRUE;
		    وإلا إذا (p[1] == 'm' || p[1] == 'M' || p[1] == 'V')
			is_magic_all = FALSE;
		    p += 2;
		}
		إذا (p[0] == NUL
			|| (p[0] == '\\'
			    && (p[1] == '|' || p[1] == '&' || p[1] == ')'
				|| p[1] == 'n'))
			|| (is_magic_all
			    && (p[0] == '|' || p[0] == '&' || p[0] == ')'))
			|| reg_magic == MAGIC_ALL)
		    curchr = Magic('$');
	    }
	    اكسر;
	حالة '\\':
	    {
		صحيح c = regparse[1];

		إذا (c == NUL)
		    curchr = '\\';	// trailing '\'
		وإلا إذا (c <= '~' && META_flags[c])
		{
		    /*
		     * META contains everything that may be magic sometimes,
		     * except ^ and $ ("\^" and "\$" are only magic after
		     * "\V").  We now fetch the next character and toggle its
		     * magicness.  Therefore, \ is so meta-magic that it is
		     * not in META.
		     */
		    curchr = -1;
		    prev_at_شروعات = at_شروعات;
		    at_شروعات = FALSE;	// be able to say "/\*ptr"
		    ++regparse;
		    ++after_slash;
		    peekchr();
		    --regparse;
		    --after_slash;
		    curchr = toggle_Magic(curchr);
		}
		وإلا إذا (vim_strchr(REGEXP_ABBR, c))
		{
		    /*
		     * Handle abbreviations, like "\t" for TAB -- webb
		     */
		    curchr = backslash_trans(c);
		}
		وإلا إذا (reg_magic == MAGIC_NONE && (c == '$' || c == '^'))
		    curchr = toggle_Magic(c);
		وإلا
		{
		    /*
		     * Next character can never be (made) magic?
		     * Then backslashing it won't do anything.
		     */
		    إذا (has_mbyte && c >= 0x80)
			curchr = (*mb_ptr2char)(regparse + 1);
		    وإلا
			curchr = c;
		}
		اكسر;
	    }

	افتراضي:
	    // curchr already holds regparse[0]; only a multi-byte lead byte
	    // needs decoding.
	    إذا (has_mbyte && curchr >= 0x80)
		curchr = (*mb_ptr2char)(regparse);
    }

    أرجع curchr;
}

/*
 * Eat one lexed character.  Do this in a way that we can undo it.
 */
    ساكن فراغ
skipchr(فراغ)
{
    // peekchr() eats a backslash, do the same here
    إذا (*regparse == '\\')
	prevchr_len = 1;
    وإلا
	prevchr_len = 0;
    إذا (regparse[prevchr_len] != NUL)
    {
	إذا (enc_utf8)
	{
	    // Exclude composing chars that mb_ptr2len does include.  A byte
	    // below 0x80 is always a single character.
	    إذا (regparse[prevchr_len] < 0x80)
		++prevchr_len;
	    وإلا
		prevchr_len += utf_ptr2len(regparse + prevchr_len);
	}
	وإلا إذا (has_mbyte)
	    prevchr_len += (*mb_ptr2len)(regparse + prevchr_len);
	وإلا
	    ++prevchr_len;
    }
    regparse += prevchr_len;
    prev_at_شروعات = at_شروعات;
    at_شروعات = FALSE;
    prevprevchr = prevchr;
    prevchr = curchr;
    curchr = nextchr;	    // use previously unget char, or -1
    nextchr = -1;
}

/*
 * Skip a character while keeping the value of prev_at_start for at_start.
 * prevchr and prevprevchr are also kept.
 */
    ساكن فراغ
skipchr_keepstart(فراغ)
{
    صحيح as = prev_at_شروعات;
    صحيح pr = prevchr;
    صحيح prpr = prevprevchr;

    skipchr();
    at_شروعات = as;
    prevchr = pr;
    prevprevchr = prpr;
}

/*
 * Get the next character from the pattern. We know about magic and such, so
 * therefore we need a lexical analyzer.
 */
    ساكن صحيح
getchr(فراغ)
{
    صحيح chr = peekchr();

    skipchr();
    أرجع chr;
}

/*
 * put character back.  Works only once!
 */
    ساكن فراغ
ungetchr(فراغ)
{
    nextchr = curchr;
    curchr = prevchr;
    prevchr = prevprevchr;
    at_شروعات = prev_at_شروعات;
    prev_at_شروعات = FALSE;

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
    ساكن vimlong_T
gethexchrs(صحيح maxinputlen)
{
    long_u	nr = 0;
    صحيح		c;
    صحيح		i;

    لكل (i = 0; i < maxinputlen; ++i)
    {
	c = regparse[0];
	إذا (!vim_isxdigit(c))
	    اكسر;
	nr <<= 4;
	nr |= hex2nr(c);
	++regparse;
    }

    إذا (i == 0)
	أرجع -1;
    أرجع nr;
}

/*
 * Get and return the value of the decimal string immediately after the
 * current position. Return -1 for invalid.  Consumes all digits.
 */
    ساكن vimlong_T
getdecchrs(فراغ)
{
    long_u	nr = 0;
    صحيح		c;
    صحيح		i;

    لكل (i = 0; ; ++i)
    {
	c = regparse[0];
	إذا (c < '0' || c > '9')
	    اكسر;
	nr *= 10;
	nr += c - '0';
	++regparse;
	curchr = -1; // no longer valid
    }

    إذا (i == 0)
	أرجع -1;
    أرجع nr;
}

/*
 * get and return the value of the octal string immediately after the current
 * position. Return -1 for invalid, or 0-255 for valid. Smart enough to handle
 * numbers > 377 correctly (for example, 400 is treated as 40) and doesn't
 * treat 8 or 9 as recognised characters. Position is updated:
 *     blahblah\%o210asdf
 *	   before-^  ^-after
 */
    ساكن ڊگهو
getoctchrs(فراغ)
{
    long_u	nr = 0;
    صحيح		c;
    صحيح		i;

    لكل (i = 0; i < 3 && nr < 040; ++i)
    {
	c = regparse[0];
	إذا (c < '0' || c > '7')
	    اكسر;
	nr <<= 3;
	nr |= hex2nr(c);
	++regparse;
    }

    إذا (i == 0)
	أرجع -1;
    أرجع (ڊگهو)nr;
}

/*
 * read_limits - Read two integers to be taken as a minimum and maximum.
 * If the first character is '-', then the range is reversed.
 * Should end with 'end'.  If minval is missing, zero is default, if maxval is
 * missing, a very big number is the default.
 */
    ساكن صحيح
read_limits(ڊگهو *minval, ڊگهو *maxval)
{
    صحيح		ابتو = FALSE;
    char_u	*پهريون_char;
    ڊگهو	tmp;

    إذا (*regparse == '-')
    {
	// Starts with '-', so reverse the range later
	regparse++;
	ابتو = TRUE;
    }
    پهريون_char = regparse;
    *minval = getdigits(&regparse);
    إذا (*regparse == ',')	    // There is a comma
    {
	إذا (vim_isdigit(*++regparse))
	    *maxval = getdigits(&regparse);
	وإلا
	    *maxval = MAX_حد_بندي;
    }
    وإلا إذا (VIM_ISDIGIT(*پهريون_char))
	*maxval = *minval;	    // It was \{n} or \{-n}
    وإلا
	*maxval = MAX_حد_بندي;	    // It was \{} or \{-}
    إذا (*regparse == '\\')
	regparse++;	// Allow either \{...} or \{...\}
    إذا (*regparse != '}')
	EMSG2_RET_FAIL(_(e_syntax_error_اندريون_str_curlies),
						       reg_magic == MAGIC_ALL);

    /*
     * Reverse the range if there was a '-', or make sure it is in the right
     * order otherwise.
     */
    إذا ((!ابتو && *minval > *maxval) || (ابتو && *minval < *maxval))
    {
	tmp = *minval;
	*minval = *maxval;
	*maxval = tmp;
    }
    skipchr();		// let's be friends with the lexer again
    أرجع OK;
}

/*
 * vim_regexec and friends
 */

/*
 * Global work variables for vim_regexec().
 */

ساكن فراغ	cleanup_subexpr(فراغ);
#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
ساكن فراغ	cleanup_zsubexpr(فراغ);
#جيڪڏهن_پورو
ساكن صحيح	ميلاپ_with_backref(linenr_T شروعات_lnum, colnr_T شروعات_کانو, linenr_T پڇاڙي_lnum, colnr_T پڇاڙي_کانو, صحيح *bytelen);

/*
 * Sometimes need to save a copy of a line.  Since alloc()/free() is very
 * slow, we keep one allocated piece of memory and only re-allocate it when
 * it's too small.  It's freed in bt_regexec_both() when finished.
 */
ساكن char_u	*reg_tofree = عدم;
ساكن بنانشان	reg_tofreelen;

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
تعريفنوع بنية {
    regmatch_T		*reg_ميلاپ;
    regmmatch_T		*reg_mmatch;

    char_u		**reg_startp;
    char_u		**reg_endp;
    lpos_T		*reg_startpos;
    lpos_T		*reg_endpos;

    win_T		*reg_win;
    buf_T		*reg_buf;
    linenr_T		reg_firstlnum;
    linenr_T		reg_maxline;
    صحيح			reg_سٽ_lbr;	// "\n" in string is line break

    // The current match-position is stord in these variables:
    linenr_T	lnum;		// line number, relative to first line
    char_u	*سٽ;		// start of current line
    char_u	*داخلا_2;		// current input, points into "line"

    صحيح	need_clear_subexpr;	// subexpressions still need to be cleared
#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
    صحيح	need_clear_zsubexpr;	// extmatch subexpressions still need to be
				// cleared
#جيڪڏهن_پورو

    // Internal copy of 'ignorecase'.  It is set at each call to vim_regexec().
    // Normally it gets the value of "rm_ic" or "rmm_ic", but when the pattern
    // contains '\c' or '\C' the value is overruled.
    صحيح			reg_ic;

    // Similar to "reg_ic", but only for 'combining' characters.  Set with \Z
    // flag in the regexp.  Defaults to false, always.
    صحيح			reg_icombine;

    // Copy of "rmm_maxcol": maximum column to search for a match.  Zero when
    // there is no maximum.
    colnr_T		reg_maxcol;

    // State for the NFA engine regexec.
    صحيح nfa_has_zend;	    // NFA regexp \ze operator encountered.
    صحيح nfa_has_backref;    // NFA regexp \1 .. \9 encountered.
    صحيح nfa_nsubexpr;	    // Number of sub expressions actually being used
			    // during execution. 1 if only the whole match
			    // (subexpr 0) is used.
    // listid is global, so that it increases on recursive calls to
    // nfa_regmatch(), which means we don't have to clear the lastlist field of
    // all the states.
    صحيح nfa_listid;
    صحيح nfa_alt_listid;

#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
    صحيح nfa_has_zsubexpr;   // NFA regexp has \z( ), set zsubexpr.
#جيڪڏهن_پورو
} regexec_T;

ساكن regexec_T	rex;
ساكن صحيح		rex_اندريون_use = FALSE;

/*
 * Return TRUE if character 'c' is included in 'iskeyword' option for
 * "reg_buf" buffer.
 */
    ساكن صحيح
reg_iswordc(صحيح c)
{
    أرجع vim_iswordc_buf(c, rex.reg_buf);
}

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
ساكن صحيح can_f_submatch = FALSE;	// TRUE when submatch() can be used

// This struct is used for reg_submatch(). Needed for when the
// substitution string is an expression that contains a call to substitute()
// and submatch().
تعريفنوع بنية {
    regmatch_T	*sm_ميلاپ;
    regmmatch_T	*sm_mmatch;
    linenr_T	sm_firstlnum;
    linenr_T	sm_maxline;
    صحيح		sm_سٽ_lbr;
} regsubmatch_T;

ساكن regsubmatch_T rsm;  // can only be used when can_f_submatch is TRUE
#جيڪڏهن_پورو

تعريفنوع تعداد
{
    RGLF_سٽ = 0x01,
    RGLF_ڊيگهائي = 0x02
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    ,
    RGLF_SUBMATCH = 0x04
#جيڪڏهن_پورو
} reg_getline_flags_T;

//
// common code for reg_getline(), reg_getline_len(), reg_getline_submatch() and
// reg_getline_submatch_len().
// the flags argument (which is a bitmask) controls what info is to be returned and whether
// or not submatch is in effect.
// note:
//     submatch is available only if FEAT_EVAL is defined.
    ساكن فراغ
reg_getline_common(
    linenr_T		lnum,
    reg_getline_flags_T	flags,
    char_u		**سٽ,
    colnr_T		*ڊيگهائي)
{
    صحيح get_سٽ = flags & RGLF_سٽ;
    صحيح get_ڊيگهائي = flags & RGLF_ڊيگهائي;
    linenr_T firstlnum;
    linenr_T maxline;

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    إذا (flags & RGLF_SUBMATCH)
    {
	firstlnum = rsm.sm_firstlnum + lnum;
	maxline = rsm.sm_maxline;
    }
    وإلا
#جيڪڏهن_پورو
    {
	firstlnum = rex.reg_firstlnum + lnum;
	maxline = rex.reg_maxline;
    }

    // when looking behind for a match/no-match lnum is negative. but we
    // can't go before line 1.
    إذا (firstlnum < 1)
    {
	إذا (get_سٽ)
	    *سٽ = عدم;
	إذا (get_ڊيگهائي)
	    *ڊيگهائي = 0;

	أرجع;
    }

    إذا (lnum > maxline)
    {
	// must have matched the "\n" in the last line.
	إذا (get_سٽ)
	    *سٽ = (char_u *)"";
	إذا (get_ڊيگهائي)
	    *ڊيگهائي = 0;

	أرجع;
    }

    إذا (get_سٽ)
	*سٽ = ml_get_buf(rex.reg_buf, firstlnum, FALSE);
    إذا (get_ڊيگهائي)
	*ڊيگهائي = ml_get_buf_len(rex.reg_buf, firstlnum);
}

/*
 * Get pointer to the line "lnum", which is relative to "reg_firstlnum".
 */
    ساكن char_u *
reg_getline(linenr_T lnum)
{
    char_u *سٽ;

    reg_getline_common(lnum, RGLF_سٽ, &سٽ, عدم);

    أرجع سٽ;
}

/*
 * Get length of line "lnum", which is relative to "reg_firstlnum".
 */
    ساكن colnr_T
reg_getline_len(linenr_T lnum)
{
    colnr_T ڊيگهائي;

    reg_getline_common(lnum, RGLF_ڊيگهائي, عدم, &ڊيگهائي);

    أرجع ڊيگهائي;
}

#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
ساكن char_u	*reg_startzp[NSUBEXP];	// Workspace to mark beginning
ساكن char_u	*reg_endzp[NSUBEXP];	//   and end of \z(...\) matches
ساكن lpos_T	reg_startzpos[NSUBEXP];	// idem, beginning pos
ساكن lpos_T	reg_endzpos[NSUBEXP];	// idem, end pos
#جيڪڏهن_پورو

// TRUE if using multi-line regexp.
#وصف_ڪريو REG_MULTI	(rex.reg_ميلاپ == عدم)

#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
/*
 * Create a new extmatch and mark it as referenced once.
 */
    ساكن reg_extmatch_T *
make_extmatch(فراغ)
{
    reg_extmatch_T	*em;

    em = ALLOC_CLEAR_ONE(reg_extmatch_T);
    إذا (em != عدم)
	em->refcnt = 1;
    أرجع em;
}

/*
 * Add a reference to an extmatch.
 */
    reg_extmatch_T *
ref_extmatch(reg_extmatch_T *em)
{
    إذا (em != عدم)
	em->refcnt++;
    أرجع em;
}

/*
 * Remove a reference to an extmatch.  If there are no references left, free
 * the info.
 */
    فراغ
unref_extmatch(reg_extmatch_T *em)
{
    صحيح i;

    إذا (em != عدم && --em->refcnt <= 0)
    {
	لكل (i = 0; i < NSUBEXP; ++i)
	    vim_free(em->matches[i]);
	vim_free(em);
    }
}
#جيڪڏهن_پورو

/*
 * Get class of previous character.
 */
    ساكن صحيح
reg_prev_class(فراغ)
{
    إذا (rex.داخلا_2 > rex.سٽ)
	أرجع mb_get_class_buf(rex.داخلا_2 - 1
		       - (*mb_سر_off)(rex.سٽ, rex.داخلا_2 - 1), rex.reg_buf);
    أرجع -1;
}

/*
 * Return TRUE if the current rex.input position matches the Visual area.
 */
    ساكن صحيح
reg_ميلاپ_visual(فراغ)
{
    pos_T	چوٽي, bot;
    linenr_T    lnum;
    colnr_T	کانو;
    win_T	*wp = rex.reg_win == عدم ? curwin : rex.reg_win;
    صحيح		گهڻائي;
    colnr_T	شروعات, پڇاڙي;
    colnr_T	start2, end2;
    colnr_T	کانن_ڳڻپ;
    colnr_T	curswant;

    // Check if the buffer is the current buffer and not using a string.
    إذا (rex.reg_buf != curbuf || VIsual.lnum == 0 || !REG_MULTI)
	أرجع FALSE;

    إذا (VIsual_سرگرم)
    {
	إذا (LT_POS(VIsual, wp->w_cursor))
	{
	    چوٽي = VIsual;
	    bot = wp->w_cursor;
	}
	وإلا
	{
	    چوٽي = wp->w_cursor;
	    bot = VIsual;
	}
	گهڻائي = VIsual_گهڻائي;
	curswant = wp->w_curswant;
    }
    وإلا
    {
	إذا (LT_POS(curbuf->b_visual.vi_شروعات, curbuf->b_visual.vi_پڇاڙي))
	{
	    چوٽي = curbuf->b_visual.vi_شروعات;
	    bot = curbuf->b_visual.vi_پڇاڙي;
	}
	وإلا
	{
	    چوٽي = curbuf->b_visual.vi_پڇاڙي;
	    bot = curbuf->b_visual.vi_شروعات;
	}
	// a substitute command may have removed some lines
	إذا (bot.lnum > curbuf->b_ml.ml_سٽ_ڳڻپ)
	    bot.lnum = curbuf->b_ml.ml_سٽ_ڳڻپ;
	گهڻائي = curbuf->b_visual.vi_گهڻائي;
	curswant = curbuf->b_visual.vi_curswant;
    }
    lnum = rex.lnum + rex.reg_firstlnum;
    إذا (lnum < چوٽي.lnum || lnum > bot.lnum)
	أرجع FALSE;

    کانو = (colnr_T)(rex.داخلا_2 - rex.سٽ);
    إذا (گهڻائي == 'v')
    {
	إذا ((lnum == چوٽي.lnum && کانو < چوٽي.کانو)
		|| (lnum == bot.lnum && کانو >= bot.کانو + (*p_sel != 'e')))
	    أرجع FALSE;
    }
    وإلا إذا (گهڻائي == Ctrl_V)
    {
	getvvcol(wp, &چوٽي, &شروعات, عدم, &پڇاڙي, 0);
	getvvcol(wp, &bot, &start2, عدم, &end2, 0);
	إذا (start2 < شروعات)
	    شروعات = start2;
	إذا (end2 > پڇاڙي)
	    پڇاڙي = end2;
	إذا (چوٽي.کانو == MAXCOL || bot.کانو == MAXCOL || curswant == MAXCOL)
	    پڇاڙي = MAXCOL;

	// getvvcol() flushes rex.line, need to get it again
	rex.سٽ = reg_getline(rex.lnum);
	rex.داخلا_2 = rex.سٽ + کانو;

	کانن_ڳڻپ = win_linetabsize(wp, rex.reg_firstlnum + rex.lnum, rex.سٽ, کانو);
	إذا (کانن_ڳڻپ < شروعات || کانن_ڳڻپ > پڇاڙي - (*p_sel == 'e'))
	    أرجع FALSE;
    }
    أرجع TRUE;
}

/*
 * Check the regexp program for its magic number.
 * Return TRUE if it's wrong.
 */
    ساكن صحيح
prog_magic_wrong(فراغ)
{
    regprog_T	*prog;

    prog = REG_MULTI ? rex.reg_mmatch->regprog : rex.reg_ميلاپ->regprog;
    إذا (prog->engine == &nfa_regengine)
	// For NFA matcher we don't check the magic
	أرجع FALSE;

    إذا (UCHARAT(((bt_regprog_T *)prog)->پروگرام) != REGMAGIC)
    {
	iemsg(e_corrupted_regexp_پروگرام);
	أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Cleanup the subexpressions, if this wasn't done yet.
 * This construction is used to clear the subexpressions only when they are
 * used (to increase speed).
 */
    ساكن فراغ
cleanup_subexpr(فراغ)
{
    إذا (!rex.need_clear_subexpr)
	أرجع;

    إذا (REG_MULTI)
    {
	// Use 0xff to set lnum to -1
	vim_memset(rex.reg_startpos, 0xff, حجم(lpos_T) * NSUBEXP);
	vim_memset(rex.reg_endpos, 0xff, حجم(lpos_T) * NSUBEXP);
    }
    وإلا
    {
	vim_memset(rex.reg_startp, 0, حجم(char_u *) * NSUBEXP);
	vim_memset(rex.reg_endp, 0, حجم(char_u *) * NSUBEXP);
    }
    rex.need_clear_subexpr = FALSE;
}

#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
    ساكن فراغ
cleanup_zsubexpr(فراغ)
{
    إذا (!rex.need_clear_zsubexpr)
	أرجع;

    إذا (REG_MULTI)
    {
	// Use 0xff to set lnum to -1
	vim_memset(reg_startzpos, 0xff, حجم(lpos_T) * NSUBEXP);
	vim_memset(reg_endzpos, 0xff, حجم(lpos_T) * NSUBEXP);
    }
    وإلا
    {
	vim_memset(reg_startzp, 0, حجم(char_u *) * NSUBEXP);
	vim_memset(reg_endzp, 0, حجم(char_u *) * NSUBEXP);
    }
    rex.need_clear_zsubexpr = FALSE;
}
#جيڪڏهن_پورو

/*
 * Advance rex.lnum, rex.line and rex.input to the next line.
 */
    ساكن فراغ
reg_nextline(فراغ)
{
    rex.سٽ = reg_getline(++rex.lnum);
    rex.داخلا_2 = rex.سٽ;
    fast_breakcheck();
}

/*
 * Check whether a backreference matches.
 * Returns RA_FAIL, RA_NOMATCH or RA_MATCH.
 * If "bytelen" is not NULL, it is set to the byte length of the match in the
 * last line.
 * Optional: ignore case if rex.reg_ic is set.
 */
    ساكن صحيح
ميلاپ_with_backref(
    linenr_T شروعات_lnum,
    colnr_T  شروعات_کانو,
    linenr_T پڇاڙي_lnum,
    colnr_T  پڇاڙي_کانو,
    صحيح	     *bytelen)
{
    linenr_T	clnum = شروعات_lnum;
    colnr_T	ccol = شروعات_کانو;
    صحيح		len;
    char_u	*p;

    إذا (bytelen != عدم)
	*bytelen = 0;
    لكل (;;)
    {
	// Since getting one line may invalidate the other, need to make copy.
	// Slow!
	إذا (rex.سٽ != reg_tofree)
	{
	    len = (صحيح)STRLEN(rex.سٽ);
	    إذا (reg_tofree == عدم || len >= (صحيح)reg_tofreelen)
	    {
		len += 50;	// get some extra
		vim_free(reg_tofree);
		reg_tofree = alloc(len);
		إذا (reg_tofree == عدم)
		    أرجع RA_FAIL; // out of memory!
		reg_tofreelen = len;
	    }
	    STRCPY(reg_tofree, rex.سٽ);
	    rex.داخلا_2 = reg_tofree + (rex.داخلا_2 - rex.سٽ);
	    rex.سٽ = reg_tofree;
	}

	// Get the line to compare with.
	p = reg_getline(clnum);
	إذا (clnum == پڇاڙي_lnum)
	    len = پڇاڙي_کانو - ccol;
	وإلا
	    len = (صحيح)reg_getline_len(clnum) - ccol;

	// Use case-insensitive compare if rex.reg_ic is set
	إذا ((!rex.reg_ic && cstrncmp(p + ccol, rex.داخلا_2, &len) != 0)
	    || (rex.reg_ic && MB_STRNICMP(p + ccol, rex.داخلا_2, len) != 0))
	    أرجع RA_NOMATCH;  // doesn't match
	إذا (bytelen != عدم)
	    *bytelen += len;
	إذا (clnum == پڇاڙي_lnum)
	    اكسر;		// match and at end!
	إذا (rex.lnum >= rex.reg_maxline)
	    أرجع RA_NOMATCH;  // text too short

	// Advance to next line.
	reg_nextline();
	إذا (bytelen != عدم)
	    *bytelen = 0;
	++clnum;
	ccol = 0;
	إذا (got_int)
	    أرجع RA_FAIL;
    }

    // found a match!  Note that rex.line may now point to a copy of the line,
    // that should not matter.
    أرجع RA_ميلاپ;
}

/*
 * Used in a place where no * or \+ can follow.
 */
    ساكن صحيح
re_mult_ايندڙ(حرف *what)
{
    إذا (re_multi_type(peekchr()) == MULTI_MULT)
    {
	semsg(_(e_nfa_regexp_cannot_repeat_str), what);
	rc_did_emsg = TRUE;
	أرجع FAIL;
    }
    أرجع OK;
}

تعريفنوع بنية
{
    صحيح a, b, c;
} decomp_T;


// 0xfb20 - 0xfb4f
ساكن decomp_T decomp_جدول[0xfb4f-0xfb20+1] =
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

    ساكن فراغ
mb_decompose(صحيح c, صحيح *c1, صحيح *c2, صحيح *c3)
{
    decomp_T d;

    إذا (c >= 0xfb20 && c <= 0xfb4f)
    {
	d = decomp_جدول[c - 0xfb20];
	*c1 = d.a;
	*c2 = d.b;
	*c3 = d.c;
    }
    وإلا
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
    ساكن صحيح
cstrncmp(char_u *s1, char_u *s2, صحيح *n)
{
    صحيح		نتيجو;

    إذا (!rex.reg_ic)
	نتيجو = STRNCMP(s1, s2, *n);
    وإلا إذا (enc_utf8)
    {
	char_u *p = s1;
	صحيح n2 = 0;
	صحيح n1 = *n;
	// count the number of characters for byte-length of s1
	طالما (n1 > 0 && *p != NUL)
	{
	    n1 -= mb_ptr2len(p);
	    MB_PTR_ADV(p);
	    n2++;
	}
	// count the number of bytes to advance the same number of chars for s2
	p = s2;
	طالما (n2-- > 0 && *p != NUL)
	    MB_PTR_ADV(p);

	n2 = p - s2;

	نتيجو = MB_STRNICMP2(s1, s2, *n, n2);
	إذا (نتيجو == 0)
	    *n = n2;
    }
    وإلا
	نتيجو = MB_STRNICMP(s1, s2, *n);

    // if it failed and it's utf8 and we want to combineignore:
    إذا (نتيجو != 0 && enc_utf8 && rex.reg_icombine)
    {
	char_u	*str1, *str2;
	صحيح	c1, c2, c11, c12;
	صحيح	junk;

	// we have to handle the strcmp ourselves, since it is necessary to
	// deal with the composing characters by ignoring them:
	str1 = s1;
	str2 = s2;
	c1 = c2 = 0;
	طالما ((صحيح)(str1 - s1) < *n)
	{
	    c1 = mb_ptr2char_adv(&str1);
	    c2 = mb_ptr2char_adv(&str2);

	    // Decompose the character if necessary, into 'base' characters.
	    // Currently hard-coded for Hebrew, Arabic to be done...
	    إذا (c1 != c2 && (!rex.reg_ic || utf_fold(c1) != utf_fold(c2)))
	    {
		// decomposition necessary?
		mb_decompose(c1, &c11, &junk, &junk);
		mb_decompose(c2, &c12, &junk, &junk);
		c1 = c11;
		c2 = c12;
		إذا (c11 != c12
			    && (!rex.reg_ic || utf_fold(c11) != utf_fold(c12)))
		    اكسر;
	    }
	}
	نتيجو = c2 - c1;
	إذا (نتيجو == 0)
	    *n = (صحيح)(str2 - s2);
    }

    أرجع نتيجو;
}

/*
 * cstrchr: This function is used a lot for simple searches, keep it fast!
 */
    ساكن char_u *
cstrchr(char_u *s, صحيح c)
{
    char_u	*p;
    صحيح		cc, lc;

    إذا (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	أرجع vim_strchr(s, c);

    // tolower() and toupper() can be slow, comparing twice should be a lot
    // faster (esp. when using MS Visual C++!).
    // For UTF-8 need to use folded case.
    إذا (enc_utf8 && c > 0x80)
    {
	cc = utf_fold(c);
	lc = cc;
    }
    وإلا
	إذا (MB_ISUPPER(c))
	{
	    cc = MB_TOLOWER(c);
	    lc = cc;
	}
	وإلا إذا (MB_ISLOWER(c))
	{
	    cc = MB_TOUPPER(c);
	    lc = c;
	}
	وإلا
	    أرجع vim_strchr(s, c);

    إذا (has_mbyte)
    {
	لكل (p = s; *p != NUL; p += (*mb_ptr2len)(p))
	{
	    صحيح uc = utf_ptr2char(p);
	    إذا (enc_utf8 && (c > 0x80 || uc > 0x80))
	    {
		// Do not match an illegal byte.  E.g. 0xff matches 0xc3 0xbf,
		// not 0xff.
		// compare with lower case of the character
		إذا ((uc < 0x80 || uc != *p) && utf_fold(uc) == lc)
		    أرجع p;
	    }
	    وإلا إذا (*p == c || *p == cc)
		أرجع p;
	}
    }
    وإلا
	// Faster version for when there are no multi-byte characters.
	لكل (p = s; *p != NUL; ++p)
	    إذا (*p == c || *p == cc)
		أرجع p;

    أرجع عدم;
}

////////////////////////////////////////////////////////////////
//		      regsub stuff			      //
////////////////////////////////////////////////////////////////

تعريفنوع فراغ (*fptr_T)(صحيح *, صحيح);

ساكن صحيح vim_regsub_both(char_u *ذريعو, typval_T *اظهار, char_u *dest, صحيح destlen, صحيح flags);

    ساكن فراغ
do_مٿانهون(صحيح *d, صحيح c)
{
    *d = MB_TOUPPER(c);
}

    ساكن فراغ
do_هيٺاهون(صحيح *d, صحيح c)
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
regtilde(char_u *ذريعو, صحيح magic)
{
    char_u	*newsub = ذريعو;
    char_u	*p;
    size_t	newsublen = 0;
    char_u	tilde[3] = {'~', NUL, NUL};
    size_t	tildelen = 1;
    صحيح		error = FALSE;

    إذا (!magic)
    {
	tilde[0] = '\\';
	tilde[1] = '~';
	tilde[2] = NUL;
	tildelen = 2;
    }

    لكل (p = newsub; *p; ++p)
    {
	إذا (STRNCMP(p, tilde, tildelen) == 0)
	{
	    size_t prefixlen = p - newsub;		// not including the tilde
	    char_u *postfix = p + tildelen;
	    size_t postfixlen;
	    size_t tmpsublen;

	    إذا (newsublen == 0)
		newsublen = STRLEN(newsub);
	    newsublen -= tildelen;
	    postfixlen = newsublen - prefixlen;
	    tmpsublen = prefixlen + reg_prev_sublen + postfixlen;

	    إذا (tmpsublen > 0 && reg_prev_sub != عدم)
	    {
		char_u *tmpsub;

		// Avoid making the text longer than MAXCOL, it will cause
		// trouble at some point.
		إذا (tmpsublen > MAXCOL)
		{
		    emsg(_(e_resulting_متن_too_long));
		    error = TRUE;
		    اكسر;
		}

		tmpsub = alloc(tmpsublen + 1);
		إذا (tmpsub == عدم)
		{
		    emsg(_(e_ٻاهريون_جو_يادگيري));
		    error = TRUE;
		    اكسر;
		}

		// copy prefix
		mch_memmove(tmpsub, newsub, prefixlen);
		// interpret tilde
		mch_memmove(tmpsub + prefixlen, reg_prev_sub, reg_prev_sublen);
		// copy postfix
		STRCPY(tmpsub + prefixlen + reg_prev_sublen, postfix);

		إذا (newsub != ذريعو)	// allocated newsub before
		    vim_free(newsub);
		newsub = tmpsub;
		newsublen = tmpsublen;
		p = newsub + prefixlen + reg_prev_sublen;
	    }
	    وإلا
		mch_memmove(p, postfix, postfixlen + 1);	// remove the tilde (+1 for the NUL)

	    --p;
	}
	وإلا
	{
	    إذا (*p == '\\' && p[1])		// skip escaped characters
		++p;
	    إذا (has_mbyte)
		p += (*mb_ptr2len)(p) - 1;
	}
    }

    إذا (error)
    {
	إذا (newsub != ذريعو)
	    vim_free(newsub);
	أرجع ذريعو;
    }

    // Store a copy of newsub  in reg_prev_sub.  It is always allocated,
    // because recursive calls may make the returned string invalid.
    // Only store it if there something to store.
    newsublen = p - newsub;
    إذا (newsublen == 0)
	VIM_CLEAR(reg_prev_sub);
    وإلا
    {
	vim_free(reg_prev_sub);
	reg_prev_sub = vim_strnsave(newsub, newsublen);
    }

    إذا (reg_prev_sub == عدم)
	reg_prev_sublen = 0;
    وإلا
	reg_prev_sublen = newsublen;

    أرجع newsub;
}

#جيڪڏهن_وصف_ٿيل FEAT_EVAL

/*
 * Put the submatches in "argv[argskip]" which is a list passed into
 * call_func() by vim_regsub_both().
 */
    ساكن صحيح
fill_submatch_list(صحيح argc اڻ_استعمال_ٿيل, typval_T *argv, صحيح argskip, ufunc_T *fp)
{
    listitem_T	*li;
    صحيح		i;
    char_u	*s;
    typval_T	*listarg = argv + argskip;

    إذا (!has_varargs(fp) && fp->uf_دليلون.ga_len <= argskip)
	// called function doesn't take a submatches argument
	أرجع argskip;

    // Relies on sl_list to be the first item in staticList10_T.
    init_static_list((staticList10_T *)(listarg->vval.v_list));

    // There are always 10 list items in staticList10_T.
    li = listarg->vval.v_list->lv_پهريون;
    لكل (i = 0; i < 10; ++i)
    {
	s = rsm.sm_ميلاپ->startp[i];
	إذا (s == عدم || rsm.sm_ميلاپ->endp[i] == عدم)
	    s = عدم;
	وإلا
	    s = vim_strnsave(s, rsm.sm_ميلاپ->endp[i] - s);
	li->li_tv.v_type = VAR_STRING;
	li->li_tv.vval.v_string = s;
	li = li->li_ايندڙ;
    }
    أرجع argskip + 1;
}

    ساكن فراغ
clear_submatch_list(staticList10_T *sl)
{
    صحيح i;

    لكل (i = 0; i < 10; ++i)
	vim_free(sl->sl_شيون[i].li_tv.vval.v_string);
}
#جيڪڏهن_پورو

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
    صحيح
vim_regsub(
    regmatch_T	*rmp,
    char_u	*ذريعو,
    typval_T	*اظهار,
    char_u	*dest,
    صحيح		destlen,
    صحيح		flags)
{
    صحيح		نتيجو;
    regexec_T	rex_save;
    صحيح		rex_اندريون_use_save = rex_اندريون_use;

    إذا (rex_اندريون_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_اندريون_use = TRUE;

    rex.reg_ميلاپ = rmp;
    rex.reg_mmatch = عدم;
    rex.reg_maxline = 0;
    rex.reg_buf = curbuf;
    rex.reg_سٽ_lbr = TRUE;
    نتيجو = vim_regsub_both(ذريعو, اظهار, dest, destlen, flags);

    rex_اندريون_use = rex_اندريون_use_save;
    إذا (rex_اندريون_use)
	rex = rex_save;

    أرجع نتيجو;
}

    صحيح
vim_regsub_multi(
    regmmatch_T	*rmp,
    linenr_T	lnum,
    char_u	*ذريعو,
    char_u	*dest,
    صحيح		destlen,
    صحيح		flags)
{
    صحيح		نتيجو;
    regexec_T	rex_save;
    صحيح		rex_اندريون_use_save = rex_اندريون_use;

    إذا (rex_اندريون_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_اندريون_use = TRUE;

    rex.reg_ميلاپ = عدم;
    rex.reg_mmatch = rmp;
    rex.reg_buf = curbuf;	// always works on the current buffer!
    rex.reg_firstlnum = lnum;
    rex.reg_maxline = curbuf->b_ml.ml_سٽ_ڳڻپ - lnum;
    rex.reg_سٽ_lbr = FALSE;
    نتيجو = vim_regsub_both(ذريعو, عدم, dest, destlen, flags);

    rex_اندريون_use = rex_اندريون_use_save;
    إذا (rex_اندريون_use)
	rex = rex_save;

    أرجع نتيجو;
}

#إذا defined(FEAT_EVAL)
// When nesting more than a couple levels it's probably a mistake.
# define MAX_REGSUB_NESTING 4
ساكن char_u   *eval_نتيجو[MAX_REGSUB_NESTING] = {عدم, عدم, عدم, عدم};

# إذا defined(EXITFREE)
    فراغ
free_resub_eval_نتيجو(فراغ)
{
    صحيح i;

    لكل (i = 0; i < MAX_REGSUB_NESTING; ++i)
	VIM_CLEAR(eval_نتيجو[i]);
}
# endif
#جيڪڏهن_پورو

    ساكن صحيح
vim_regsub_both(
    char_u	*ذريعو,
    typval_T	*اظهار,
    char_u	*dest,
    صحيح		destlen,
    صحيح		flags)
{
    char_u	*src;
    char_u	*dst;
    char_u	*s;
    صحيح		c;
    صحيح		cc;
    صحيح		no = -1;
    fptr_T	func_all = (fptr_T)عدم;
    fptr_T	func_one = (fptr_T)عدم;
    linenr_T	clnum = 0;	// init for GCC
    صحيح		len = 0;	// init for GCC
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    ساكن صحيح  nesting = 0;
    صحيح		nested;
#جيڪڏهن_پورو
    صحيح		copy = flags & REGSUB_COPY;

    // Be paranoid...
    إذا ((ذريعو == عدم && اظهار == عدم) || dest == عدم)
    {
	iemsg(e_null_دليل);
	أرجع 0;
    }
    إذا (prog_magic_wrong())
	أرجع 0;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    إذا (nesting == MAX_REGSUB_NESTING)
    {
	emsg(_(e_substitute_nesting_too_deep));
	أرجع 0;
    }
    nested = nesting;
#جيڪڏهن_پورو
    src = ذريعو;
    dst = dest;

    /*
     * When the substitute part starts with "\=" evaluate it as an expression.
     */
    إذا (اظهار != عدم || (ذريعو[0] == '\\' && ذريعو[1] == '='))
    {
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	// To make sure that the length doesn't change between checking the
	// length and copying the string, and to speed up things, the
	// resulting string is saved from the call with
	// "flags & REGSUB_COPY" == 0 to the call with
	// "flags & REGSUB_COPY" != 0.
	إذا (copy)
	{
	    إذا (eval_نتيجو[nested] != عدم)
	    {
		صحيح eval_len = (صحيح)STRLEN(eval_نتيجو[nested]);

		إذا (eval_len < destlen)
		{
		    STRCPY(dest, eval_نتيجو[nested]);
		    dst += eval_len;
		    VIM_CLEAR(eval_نتيجو[nested]);
		}
	    }
	}
	وإلا
	{
	    صحيح		    prev_can_f_submatch = can_f_submatch;
	    regsubmatch_T   rsm_save;

	    VIM_CLEAR(eval_نتيجو[nested]);

	    // The expression may contain substitute(), which calls us
	    // recursively.  Make sure submatch() gets the text from the first
	    // level.
	    إذا (can_f_submatch)
		rsm_save = rsm;
	    can_f_submatch = TRUE;
	    rsm.sm_ميلاپ = rex.reg_ميلاپ;
	    rsm.sm_mmatch = rex.reg_mmatch;
	    rsm.sm_firstlnum = rex.reg_firstlnum;
	    rsm.sm_maxline = rex.reg_maxline;
	    rsm.sm_سٽ_lbr = rex.reg_سٽ_lbr;

	    // Although unlikely, it is possible that the expression invokes a
	    // substitute command (it might fail, but still).  Therefore keep
	    // an array of eval results.
	    ++nesting;

	    إذا (اظهار != عدم)
	    {
		typval_T	argv[2];
		char_u		buf[NUMBUFLEN];
		typval_T	rettv;
		staticList10_T	ميلاپ_List;
		funcexe_T	funcexe;

		rettv.v_type = VAR_STRING;
		rettv.vval.v_string = عدم;
		argv[0].v_type = VAR_LIST;
		argv[0].vval.v_list = &ميلاپ_List.sl_list;
		ميلاپ_List.sl_list.lv_len = 0;
		CLEAR_ميدان(funcexe);
		funcexe.fe_argv_func = fill_submatch_list;
		funcexe.fe_evaluate = TRUE;
		إذا (اظهار->v_type == VAR_FUNC)
		{
		    s = اظهار->vval.v_string;
		    call_func(s, -1, &rettv, 1, argv, &funcexe);
		}
		وإلا إذا (اظهار->v_type == VAR_PARTIAL)
		{
		    partial_T   *partial = اظهار->vval.v_partial;

		    s = partial_name(partial);
		    funcexe.fe_partial = partial;
		    call_func(s, -1, &rettv, 1, argv, &funcexe);
		}
		وإلا إذا (اظهار->v_type == VAR_INSTR)
		{
		    exe_typval_instr(اظهار, &rettv);
		}
		إذا (ميلاپ_List.sl_list.lv_len > 0)
		    // fill_submatch_list() was called
		    clear_submatch_list(&ميلاپ_List);

		إذا (rettv.v_type == VAR_UNKNOWN)
		    // something failed, no need to report another error
		    eval_نتيجو[nested] = عدم;
		وإلا
		{
		    eval_نتيجو[nested] = tv_get_string_buf_chk(&rettv, buf);
		    إذا (eval_نتيجو[nested] != عدم)
			eval_نتيجو[nested] = vim_strsave(eval_نتيجو[nested]);
		}
		clear_tv(&rettv);
	    }
	    وإلا إذا (substitute_instr != عدم)
		// Execute instructions from ISN_SUBSTITUTE.
		eval_نتيجو[nested] = exe_substitute_instr();
	    وإلا
		eval_نتيجو[nested] = eval_منزل_string(ذريعو + 2, TRUE, FALSE);
	    --nesting;

	    إذا (eval_نتيجو[nested] != عدم)
	    {
		صحيح had_backslash = FALSE;

		لكل (s = eval_نتيجو[nested]; *s != NUL; MB_PTR_ADV(s))
		{
		    // Change NL to CR, so that it becomes a line break,
		    // unless called from vim_regexec_nl().
		    // Skip over a backslashed character.
		    إذا (*s == NL && !rsm.sm_سٽ_lbr)
			*s = CAR;
		    وإلا إذا (*s == '\\' && s[1] != NUL)
		    {
			++s;
			/* Change NL to CR here too, so that this works:
			 * :s/abc\\\ndef/\="aaa\\\nbbb"/  on text:
			 *   abc\
			 *   def
			 * Not when called from vim_regexec_nl().
			 */
			إذا (*s == NL && !rsm.sm_سٽ_lbr)
			    *s = CAR;
			had_backslash = TRUE;
		    }
		}
		إذا (had_backslash && (flags & REGSUB_BACKSLASH))
		{
		    // Backslashes will be consumed, need to double them.
		    s = vim_strsave_escaped(eval_نتيجو[nested], (char_u *)"\\");
		    إذا (s != عدم)
		    {
			vim_free(eval_نتيجو[nested]);
			eval_نتيجو[nested] = s;
		    }
		}

		dst += STRLEN(eval_نتيجو[nested]);
	    }

	    can_f_submatch = prev_can_f_submatch;
	    إذا (can_f_submatch)
		rsm = rsm_save;
	}
#جيڪڏهن_پورو
    }
    وإلا
      طالما ((c = *src++) != NUL)
      {
	إذا (c == '&' && (flags & REGSUB_MAGIC))
	    no = 0;
	وإلا إذا (c == '\\' && *src != NUL)
	{
	    إذا (*src == '&' && !(flags & REGSUB_MAGIC))
	    {
		++src;
		no = 0;
	    }
	    وإلا إذا ('0' <= *src && *src <= '9')
	    {
		no = *src++ - '0';
	    }
	    وإلا إذا (vim_strchr((char_u *)"uUlLeE", *src))
	    {
		بدل (*src++)
		{
		حالة 'u':   func_one = do_مٿانهون;
			    استمر;
		حالة 'U':   func_all = do_مٿانهون;
			    استمر;
		حالة 'l':   func_one = do_هيٺاهون;
			    استمر;
		حالة 'L':   func_all = do_هيٺاهون;
			    استمر;
		حالة 'e':
		حالة 'E':   func_one = func_all = (fptr_T)عدم;
			    استمر;
		}
	    }
	}
	إذا (no < 0)	      // Ordinary character.
	{
	    إذا (c == K_SPECIAL && src[0] != NUL && src[1] != NUL)
	    {
		// Copy a special key as-is.
		إذا (copy)
		{
		    إذا (dst + 3 > dest + destlen)
		    {
			iemsg("vim_regsub_both(): not enough space");
			أرجع 0;
		    }
		    *dst++ = c;
		    *dst++ = *src++;
		    *dst++ = *src++;
		}
		وإلا
		{
		    dst += 3;
		    src += 2;
		}
		استمر;
	    }

	    إذا (c == '\\' && *src != NUL)
	    {
		// Check for abbreviations -- webb
		بدل (*src)
		{
		    حالة 'r':	c = CAR;	++src;	اكسر;
		    حالة 'n':	c = NL;		++src;	اكسر;
		    حالة 't':	c = TAB;	++src;	اكسر;
		 // Oh no!  \e already has meaning in subst pat :-(
		 // case 'e':   c = ESC;	++src;	break;
		    حالة 'b':	c = Ctrl_H;	++src;	اكسر;

		    // If "backslash" is TRUE the backslash will be removed
		    // later.  Used to insert a literal CR.
		    افتراضي:	إذا (flags & REGSUB_BACKSLASH)
				{
				    إذا (copy)
				    {
					إذا (dst + 1 > dest + destlen)
					{
					    iemsg("vim_regsub_both(): not enough space");
					    أرجع 0;
					}
					*dst = '\\';
				    }
				    ++dst;
				}
				c = *src++;
		}
	    }
	    وإلا إذا (has_mbyte)
		c = mb_ptr2char(src - 1);

	    // Write to buffer, if copy is set.
	    إذا (func_one != (fptr_T)عدم)
	    {
		func_one(&cc, c);
		func_one = عدم;
	    }
	    وإلا إذا (func_all != (fptr_T)عدم)
		func_all(&cc, c);
	    وإلا // just copy
		cc = c;

	    إذا (has_mbyte)
	    {
		صحيح totlen = mb_ptr2len(src - 1);
		صحيح charlen = mb_char2len(cc);

		إذا (copy)
		{
		    إذا (dst + charlen > dest + destlen)
		    {
			iemsg("vim_regsub_both(): not enough space");
			أرجع 0;
		    }
		    mb_char2bytes(cc, dst);
		}
		dst += charlen - 1;
		إذا (enc_utf8)
		{
		    صحيح clen = utf_ptr2len(src - 1);

		    // If the character length is shorter than "totlen", there
		    // are composing characters; copy them as-is.
		    إذا (clen < totlen)
		    {
			إذا (copy)
			{
			    إذا (dst + totlen - clen > dest + destlen)
			    {
				iemsg("vim_regsub_both(): not enough space");
				أرجع 0;
			    }
			    mch_memmove(dst + 1, src - 1 + clen,
						     (size_t)(totlen - clen));
			}
			dst += totlen - clen;
		    }
		}
		src += totlen - 1;
	    }
	    وإلا إذا (copy)
	    {
		إذا (dst + 1 > dest + destlen)
		{
		    iemsg("vim_regsub_both(): not enough space");
		    أرجع 0;
		}
		*dst = cc;
	    }
	    dst++;
	}
	وإلا
	{
	    إذا (REG_MULTI)
	    {
		clnum = rex.reg_mmatch->startpos[no].lnum;
		إذا (clnum < 0 || rex.reg_mmatch->endpos[no].lnum < 0)
		    s = عدم;
		وإلا
		{
		    s = reg_getline(clnum) + rex.reg_mmatch->startpos[no].کانو;
		    إذا (rex.reg_mmatch->endpos[no].lnum == clnum)
			len = rex.reg_mmatch->endpos[no].کانو
					    - rex.reg_mmatch->startpos[no].کانو;
		    وإلا
			len = (صحيح)reg_getline_len(clnum) - rex.reg_mmatch->startpos[no].کانو;
		}
	    }
	    وإلا
	    {
		s = rex.reg_ميلاپ->startp[no];
		إذا (rex.reg_ميلاپ->endp[no] == عدم)
		    s = عدم;
		وإلا
		    len = (صحيح)(rex.reg_ميلاپ->endp[no] - s);
	    }
	    إذا (s != عدم)
	    {
		لكل (;;)
		{
		    إذا (len == 0)
		    {
			إذا (REG_MULTI)
			{
			    إذا (rex.reg_mmatch->endpos[no].lnum == clnum)
				اكسر;
			    إذا (copy)
			    {
				إذا (dst + 1 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    أرجع 0;
				}
				*dst = CAR;
			    }
			    ++dst;
			    s = reg_getline(++clnum);
			    إذا (rex.reg_mmatch->endpos[no].lnum == clnum)
				len = rex.reg_mmatch->endpos[no].کانو;
			    وإلا
				len = (صحيح)reg_getline_len(clnum);
			}
			وإلا
			    اكسر;
		    }
		    وإلا إذا (*s == NUL) // we hit NUL.
		    {
			إذا (copy)
			    iemsg(e_damaged_ميلاپ_string);
			اذهب ٻاهر_نڪرو;
		    }
		    وإلا
		    {
			إذا ((flags & REGSUB_BACKSLASH)
						  && (*s == CAR || *s == '\\'))
			{
			    /*
			     * Insert a backslash in front of a CR, otherwise
			     * it will be replaced by a line break.
			     * Number of backslashes will be halved later,
			     * double them here.
			     */
			    إذا (copy)
			    {
				إذا (dst + 2 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    أرجع 0;
				}
				dst[0] = '\\';
				dst[1] = *s;
			    }
			    dst += 2;
			}
			وإلا
			{
			    إذا (has_mbyte)
				c = mb_ptr2char(s);
			    وإلا
				c = *s;

			    إذا (func_one != (fptr_T)عدم)
			    {
				func_one(&cc, c);
				func_one = عدم;
			    }
			    وإلا إذا (func_all != (fptr_T)عدم)
				func_all(&cc, c);
			    وإلا // just copy
				cc = c;

			    إذا (has_mbyte)
			    {
				صحيح l;
				صحيح charlen;

				// Copy composing characters separately, one
				// at a time.
				إذا (enc_utf8)
				    l = utf_ptr2len(s) - 1;
				وإلا
				    l = mb_ptr2len(s) - 1;

				s += l;
				len -= l;
				charlen = mb_char2len(cc);
				إذا (copy)
				{
				    إذا (dst + charlen > dest + destlen)
				    {
					iemsg("vim_regsub_both(): not enough space");
					أرجع 0;
				    }
				    mb_char2bytes(cc, dst);
				}
				dst += charlen - 1;
			    }
			    وإلا إذا (copy)
			    {
				إذا (dst + 1 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    أرجع 0;
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
    إذا (copy)
	*dst = NUL;

ٻاهر_نڪرو:
    أرجع (صحيح)((dst - dest) + 1);
}

#جيڪڏهن_وصف_ٿيل FEAT_EVAL

    ساكن char_u *
reg_getline_submatch(linenr_T lnum)
{
    char_u *سٽ;

    reg_getline_common(lnum, RGLF_سٽ | RGLF_SUBMATCH, &سٽ, عدم);

    أرجع سٽ;
}

    ساكن colnr_T
reg_getline_submatch_len(linenr_T lnum)
{
    colnr_T ڊيگهائي;

    reg_getline_common(lnum, RGLF_ڊيگهائي | RGLF_SUBMATCH, عدم, &ڊيگهائي);

    أرجع ڊيگهائي;
}

/*
 * Used for the submatch() function: get the string from the n'th submatch in
 * allocated memory.
 * Returns NULL when not in a ":s" command and for a non-existing submatch.
 */
    char_u *
reg_submatch(صحيح no)
{
    char_u	*retval = عدم;
    char_u	*s;
    صحيح		len;
    صحيح		ڦيرو;
    linenr_T	lnum;

    إذا (!can_f_submatch || no < 0)
	أرجع عدم;

    إذا (rsm.sm_ميلاپ == عدم)
    {
	/*
	 * First round: compute the length and allocate memory.
	 * Second round: copy the text.
	 */
	لكل (ڦيرو = 1; ڦيرو <= 2; ++ڦيرو)
	{
	    lnum = rsm.sm_mmatch->startpos[no].lnum;
	    إذا (lnum < 0 || rsm.sm_mmatch->endpos[no].lnum < 0)
		أرجع عدم;

	    s = reg_getline_submatch(lnum);
	    إذا (s == عدم)  // anti-crash check, cannot happen?
		اكسر;
	    s += rsm.sm_mmatch->startpos[no].کانو;
	    إذا (rsm.sm_mmatch->endpos[no].lnum == lnum)
	    {
		// Within one line: take form start to end col.
		len = rsm.sm_mmatch->endpos[no].کانو
					  - rsm.sm_mmatch->startpos[no].کانو;
		إذا (ڦيرو == 2)
		    vim_strncpy(retval, s, len);
		++len;
	    }
	    وإلا
	    {
		// Multiple lines: take start line from start col, middle
		// lines completely and end line up to end col.
		len = (صحيح)reg_getline_submatch_len(lnum) - rsm.sm_mmatch->startpos[no].کانو;
		إذا (ڦيرو == 2)
		{
		    STRCPY(retval, s);
		    retval[len] = '\n';
		}
		++len;
		++lnum;
		طالما (lnum < rsm.sm_mmatch->endpos[no].lnum)
		{
		    s = reg_getline_submatch(lnum);
		    إذا (ڦيرو == 2)
			STRCPY(retval + len, s);
		    len += (صحيح)reg_getline_submatch_len(lnum);
		    إذا (ڦيرو == 2)
			retval[len] = '\n';
		    ++len;
		    ++lnum;
		}
		إذا (ڦيرو == 2)
		    STRNCPY(retval + len, reg_getline_submatch(lnum),
					     rsm.sm_mmatch->endpos[no].کانو);
		len += rsm.sm_mmatch->endpos[no].کانو;
		إذا (ڦيرو == 2)
		    retval[len] = NUL;
		++len;
	    }

	    إذا (retval == عدم)
	    {
		retval = alloc(len);
		إذا (retval == عدم)
		    أرجع عدم;
	    }
	}
    }
    وإلا
    {
	s = rsm.sm_ميلاپ->startp[no];
	إذا (s == عدم || rsm.sm_ميلاپ->endp[no] == عدم)
	    retval = عدم;
	وإلا
	    retval = vim_strnsave(s, rsm.sm_ميلاپ->endp[no] - s);
    }

    أرجع retval;
}

/*
 * Used for the submatch() function with the optional non-zero argument: get
 * the list of strings from the n'th submatch in allocated memory with NULs
 * represented in NLs.
 * Returns a list of allocated strings.  Returns NULL when not in a ":s"
 * command, for a non-existing submatch and for any error.
 */
    list_T *
reg_submatch_list(صحيح no)
{
    char_u	*s;
    linenr_T	slnum;
    linenr_T	elnum;
    colnr_T	scol;
    colnr_T	ecol;
    صحيح		i;
    list_T	*list;
    صحيح		error = FALSE;

    إذا (!can_f_submatch || no < 0)
	أرجع عدم;

    إذا (rsm.sm_ميلاپ == عدم)
    {
	slnum = rsm.sm_mmatch->startpos[no].lnum;
	elnum = rsm.sm_mmatch->endpos[no].lnum;
	إذا (slnum < 0 || elnum < 0)
	    أرجع عدم;

	scol = rsm.sm_mmatch->startpos[no].کانو;
	ecol = rsm.sm_mmatch->endpos[no].کانو;

	list = list_alloc();
	إذا (list == عدم)
	    أرجع عدم;

	s = reg_getline_submatch(slnum) + scol;
	إذا (slnum == elnum)
	{
	    إذا (list_append_string(list, s, ecol - scol) == FAIL)
		error = TRUE;
	}
	وإلا
	{
	    صحيح max_lnum = elnum - slnum;

	    إذا (list_append_string(list, s, -1) == FAIL)
		error = TRUE;
	    لكل (i = 1; i < max_lnum; i++)
	    {
		s = reg_getline_submatch(slnum + i);
		إذا (list_append_string(list, s, -1) == FAIL)
		    error = TRUE;
	    }
	    s = reg_getline_submatch(elnum);
	    إذا (list_append_string(list, s, ecol) == FAIL)
		error = TRUE;
	}
    }
    وإلا
    {
	s = rsm.sm_ميلاپ->startp[no];
	إذا (s == عدم || rsm.sm_ميلاپ->endp[no] == عدم)
	    أرجع عدم;
	list = list_alloc();
	إذا (list == عدم)
	    أرجع عدم;
	إذا (list_append_string(list, s,
				 (صحيح)(rsm.sm_ميلاپ->endp[no] - s)) == FAIL)
	    error = TRUE;
    }

    إذا (error)
    {
	list_free(list);
	أرجع عدم;
    }
    ++list->lv_refcount;
    أرجع list;
}
#جيڪڏهن_پورو

/*
 * Initialize the values used for matching against multiple lines
 */
    ساكن فراغ
init_regexec_multi(
	regmmatch_T	*rmp,
	win_T		*win,	// window in which to search or NULL
	buf_T		*buf,	// buffer in which to search
	linenr_T	lnum)	// nr of line to start looking for match
{
    rex.reg_ميلاپ = عدم;
    rex.reg_mmatch = rmp;
    rex.reg_buf = buf;
    rex.reg_win = win;
    rex.reg_firstlnum = lnum;
    rex.reg_maxline = rex.reg_buf->b_ml.ml_سٽ_ڳڻپ - lnum;
    rex.reg_سٽ_lbr = FALSE;
    rex.reg_ic = rmp->rmm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = rmp->rmm_maxcol;
}

#وصف_ڪريو CLASS_not		0x80
#وصف_ڪريو CLASS_af		0x40
#وصف_ڪريو CLASS_AF		0x20
#وصف_ڪريو CLASS_az		0x10
#وصف_ڪريو CLASS_AZ		0x08
#وصف_ڪريو CLASS_o7		0x04
#وصف_ڪريو CLASS_o9		0x02
#وصف_ڪريو CLASS_underscore	0x01

/*
 * Parse the "[...]" collection between "start" (the first character after the
 * "[") and "end" (the "]") into a combination of CLASS_ bits, so that it can
 * be turned into a faster character class like \d or \x.  Sets "*newl" to TRUE
 * when the collection also matches a newline.  Returns -1 when the collection
 * is not a plain combination of the recognized ranges.
 */
    ساكن صحيح
get_char_class_بٽ(char_u *شروعات, char_u *پڇاڙي, صحيح *newl)
{
    char_u	*p = شروعات;
    صحيح		config = 0;

    *newl = FALSE;
    إذا (*پڇاڙي != ']')
	أرجع -1;
    إذا (*p == '^')
    {
	config |= CLASS_not;
	p++;
    }

    طالما (p < پڇاڙي)
    {
	إذا (p + 2 < پڇاڙي && *(p + 1) == '-')
	{
	    بدل (*p)
	    {
		حالة '0':
		    إذا (*(p + 2) == '9')
		    {
			config |= CLASS_o9;
			اكسر;
		    }
		    إذا (*(p + 2) == '7')
		    {
			config |= CLASS_o7;
			اكسر;
		    }
		    أرجع -1;

		حالة 'a':
		    إذا (*(p + 2) == 'z')
		    {
			config |= CLASS_az;
			اكسر;
		    }
		    إذا (*(p + 2) == 'f')
		    {
			config |= CLASS_af;
			اكسر;
		    }
		    أرجع -1;

		حالة 'A':
		    إذا (*(p + 2) == 'Z')
		    {
			config |= CLASS_AZ;
			اكسر;
		    }
		    إذا (*(p + 2) == 'F')
		    {
			config |= CLASS_AF;
			اكسر;
		    }
		    أرجع -1;

		افتراضي:
		    أرجع -1;
	    }
	    p += 3;
	}
	وإلا إذا (p + 1 < پڇاڙي && *p == '\\' && *(p + 1) == 'n')
	{
	    *newl = TRUE;
	    p += 2;
	}
	وإلا إذا (*p == '_')
	{
	    config |= CLASS_underscore;
	    p++;
	}
	وإلا إذا (*p == '\n')
	{
	    *newl = TRUE;
	    p++;
	}
	وإلا
	    أرجع -1;
    } // while (p < end)

    إذا (p != پڇاڙي)
	أرجع -1;

    // U+017F and U+212A fold to ASCII 's' and 'k', missed by the class opcodes
    إذا (enc_utf8 && (config & (CLASS_az | CLASS_AZ)))
	أرجع -1;

    أرجع config;
}

#ضمن "regexp_bt.c"

ساكن regengine_T bt_regengine =
{
    bt_regcomp,
    bt_regfree,
    bt_regexec_nl,
    bt_regexec_multi
#جيڪڏهن_وصف_ٿيل DEBUG
    ,(char_u *)""
#جيڪڏهن_پورو
};

#ضمن "regexp_nfa.c"

ساكن regengine_T nfa_regengine =
{
    nfa_regcomp,
    nfa_regfree,
    nfa_regexec_nl,
    nfa_regexec_multi
#جيڪڏهن_وصف_ٿيل DEBUG
    ,(char_u *)""
#جيڪڏهن_پورو
};

// Which regexp engine to use? Needed for vim_regcomp().
// Must match with 'regexpengine'.
ساكن صحيح regexp_engine = 0;

#جيڪڏهن_وصف_ٿيل DEBUG
ساكن ثابت char_u regname[][30] = {
		    "AUTOMATIC Regexp Engine",
		    "BACKTRACKING Regexp Engine",
		    "NFA Regexp Engine"
			    };
#جيڪڏهن_پورو

/*
 * Compile a regular expression into internal code.
 * Returns the program in allocated memory.
 * Use vim_regfree() to free the memory.
 * Returns NULL for an error.
 */
    regprog_T *
vim_regcomp(char_u *اظهار_arg, صحيح re_flags)
{
    regprog_T   *prog = عدم;
    char_u	*اظهار = اظهار_arg;
    صحيح		called_emsg_before;

    regexp_engine = p_re;

    // Check for prefix "\%#=", that sets the regexp engine
    إذا (STRNCMP(اظهار, "\\%#=", 4) == 0)
    {
	صحيح newengine = اظهار[4] - '0';

	إذا (newengine == AUTOMATIC_ENGINE
	    || newengine == BACKTRACKING_ENGINE
	    || newengine == NFA_ENGINE)
	{
	    regexp_engine = اظهار[4] - '0';
	    اظهار += 5;
#جيڪڏهن_وصف_ٿيل DEBUG
	    smsg("New regexp mode selected (%d): %s",
					   regexp_engine, regname[newengine]);
#جيڪڏهن_پورو
	}
	وإلا
	{
	    emsg(_(e_percent_hash_can_only_be_followed_by_zero_one_two_automatic_engine_will_be_استعمال_ٿيل));
	    regexp_engine = AUTOMATIC_ENGINE;
	}
    }
#جيڪڏهن_وصف_ٿيل DEBUG
    bt_regengine.اظهار = اظهار;
    nfa_regengine.اظهار = اظهار;
#جيڪڏهن_پورو
    // reg_iswordc() uses rex.reg_buf
    rex.reg_buf = curbuf;

    /*
     * First try the NFA engine, unless backtracking was requested.
     */
    called_emsg_before = called_emsg;
    إذا (regexp_engine != BACKTRACKING_ENGINE)
	prog = nfa_regengine.regcomp(اظهار,
		re_flags + (regexp_engine == AUTOMATIC_ENGINE ? RE_AUTO : 0));
    وإلا
	prog = bt_regengine.regcomp(اظهار, re_flags);

    // Check for error compiling regexp with initial engine.
    إذا (prog == عدم)
    {
#جيڪڏهن_وصف_ٿيل BT_REGEXP_DEBUG_LOG
	إذا (regexp_engine == BACKTRACKING_ENGINE)   // debugging log for BT engine
	{
	    FILE *f;
	    f = fopen(BT_REGEXP_DEBUG_LOG_NAME, "a");
	    إذا (f)
	    {
		fprintf(f, "Syntax error in \"%s\"\n", اظهار);
		fclose(f);
	    }
	    وإلا
		semsg("(NFA) Could not open \"%s\" to write !!!",
			BT_REGEXP_DEBUG_LOG_NAME);
	}
#جيڪڏهن_پورو
	/*
	 * If the NFA engine failed, try the backtracking engine.
	 * The NFA engine also fails for patterns that it can't handle well
	 * but are still valid patterns, thus a retry should work.
	 * But don't try if an error message was given.
	 */
	إذا (regexp_engine == AUTOMATIC_ENGINE
					  && called_emsg == called_emsg_before)
	{
	    regexp_engine = BACKTRACKING_ENGINE;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	    report_re_switch(اظهار);
#جيڪڏهن_پورو
	    prog = bt_regengine.regcomp(اظهار, re_flags);
	}
    }

    إذا (prog != عدم)
    {
	// Store the info needed to call regcomp() again when the engine turns
	// out to be very slow when executing it.
	prog->re_engine = regexp_engine;
	prog->re_flags  = re_flags;
    }

    أرجع prog;
}

/*
 * Free a compiled regexp program, returned by vim_regcomp().
 */
    فراغ
vim_regfree(regprog_T *prog)
{
    إذا (prog != عدم)
	prog->engine->regfree(prog);
}

#إذا defined(EXITFREE)
    فراغ
free_regexp_stuff(فراغ)
{
    ga_clear(&regstack);
    ga_clear(&backpos);
    vim_free(reg_tofree);
    vim_free(reg_prev_sub);
    vim_free(post_شروعات);   // NFA postfix buffer, reused across compilations
    post_شروعات = عدم;
    post_شروعات_len = 0;
    vim_free(nfa_ڍير);    // NFA fragment stack, reused across compilations
    nfa_ڍير = عدم;
    nfa_ڍير_len = 0;
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    ساكن فراغ
report_re_switch(char_u *pat)
{
    إذا (p_verbose > 0)
    {
	verbose_enter();
	نياپو_puts(_("Switching to backtracking RE engine for pattern: "));
	نياپو_puts((حرف *)pat);
	verbose_leave();
    }
}
#جيڪڏهن_پورو

#إذا defined(FEAT_X11)
/*
 * Return whether "prog" is currently being executed.
 */
    صحيح
regprog_اندريون_use(regprog_T *prog)
{
    أرجع prog->re_اندريون_use;
}
#جيڪڏهن_پورو

/*
 * Match a regexp against a string.
 * "rmp->regprog" must be a compiled regexp as returned by vim_regcomp().
 * Note: "rmp->regprog" may be freed and changed.
 * Uses curbuf for line count and 'iskeyword'.
 * When "nl" is TRUE consider a "\n" in "line" to be a line break.
 *
 * Return TRUE if there is a match, FALSE if not.
 */
    ساكن صحيح
vim_regexec_string(
    regmatch_T	*rmp,
    char_u	*سٽ,  // string to match against
    colnr_T	کانو,    // column to start looking for match
    صحيح		nl)
{
    صحيح		نتيجو;
    regexec_T	rex_save;
    صحيح		rex_اندريون_use_save = rex_اندريون_use;

    // Cannot use the same prog recursively, it contains state.
    إذا (rmp->regprog->re_اندريون_use)
    {
	emsg(_(e_cannot_use_نمونو_recursively));
	أرجع FALSE;
    }
    rmp->regprog->re_اندريون_use = TRUE;

    إذا (rex_اندريون_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_اندريون_use = TRUE;

    rex.reg_startp = عدم;
    rex.reg_endp = عدم;
    rex.reg_startpos = عدم;
    rex.reg_endpos = عدم;

    نتيجو = rmp->regprog->engine->regexec_nl(rmp, سٽ, کانو, nl);
    rmp->regprog->re_اندريون_use = FALSE;

    // NFA engine aborted because it's very slow.
    إذا (rmp->regprog->re_engine == AUTOMATIC_ENGINE
					       && نتيجو == NFA_TOO_EXPENSIVE)
    {
	صحيح    save_p_re = p_re;
	صحيح    re_flags = rmp->regprog->re_flags;
	char_u *pat = vim_strsave(((nfa_regprog_T *)rmp->regprog)->نمونو);

	p_re = BACKTRACKING_ENGINE;
	إذا (pat != عدم)
	{
	    regprog_T *prev_prog = rmp->regprog;

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	    report_re_switch(pat);
#جيڪڏهن_پورو
	    rmp->regprog = vim_regcomp(pat, re_flags);
	    إذا (rmp->regprog == عدم)
	    {
		// Somehow compiling the pattern failed now, put back the
		// previous one to avoid "regprog" becoming NULL.
		rmp->regprog = prev_prog;
	    }
	    وإلا
	    {
		vim_regfree(prev_prog);
		rmp->regprog->re_اندريون_use = TRUE;
		نتيجو = rmp->regprog->engine->regexec_nl(rmp, سٽ, کانو, nl);
		rmp->regprog->re_اندريون_use = FALSE;
	    }
	    vim_free(pat);
	}

	p_re = save_p_re;
    }

    rex_اندريون_use = rex_اندريون_use_save;
    إذا (rex_اندريون_use)
	rex = rex_save;

    أرجع نتيجو > 0;
}

#إذا defined(FEAT_SPELL) || defined(FEAT_EVAL) || defined(FEAT_X11)
/*
 * Note: "*prog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    صحيح
vim_regexec_prog(
    regprog_T	**prog,
    صحيح		ignore_case,
    char_u	*سٽ,
    colnr_T	کانو)
{
    صحيح		r;
    regmatch_T	regmatch;

    regmatch.regprog = *prog;
    regmatch.rm_ic = ignore_case;
    r = vim_regexec_string(&regmatch, سٽ, کانو, FALSE);
    *prog = regmatch.regprog;
    أرجع r;
}
#جيڪڏهن_پورو

/*
 * Note: "rmp->regprog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    صحيح
vim_regexec(regmatch_T *rmp, char_u *سٽ, colnr_T کانو)
{
    أرجع vim_regexec_string(rmp, سٽ, کانو, FALSE);
}

/*
 * Like vim_regexec(), but consider a "\n" in "line" to be a line break.
 * Note: "rmp->regprog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    صحيح
vim_regexec_nl(regmatch_T *rmp, char_u *سٽ, colnr_T کانو)
{
    أرجع vim_regexec_string(rmp, سٽ, کانو, TRUE);
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
    ڊگهو
vim_regexec_multi(
    regmmatch_T *rmp,
    win_T       *win,		// window in which to search or NULL
    buf_T       *buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	کانو,		// column to start looking for match
    صحيح		*timed_ٻاهريون)	// flag is set when timeout limit reached
{
    صحيح		نتيجو;
    regexec_T	rex_save;
    صحيح		rex_اندريون_use_save = rex_اندريون_use;

    // Cannot use the same prog recursively, it contains state.
    إذا (rmp->regprog->re_اندريون_use)
    {
	emsg(_(e_cannot_use_نمونو_recursively));
	أرجع FALSE;
    }
    rmp->regprog->re_اندريون_use = TRUE;

    إذا (rex_اندريون_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_اندريون_use = TRUE;

    نتيجو = rmp->regprog->engine->regexec_multi(
				      rmp, win, buf, lnum, کانو, timed_ٻاهريون);
    rmp->regprog->re_اندريون_use = FALSE;

    // NFA engine aborted because it's very slow.
    إذا (rmp->regprog->re_engine == AUTOMATIC_ENGINE
					       && نتيجو == NFA_TOO_EXPENSIVE)
    {
	صحيح    save_p_re = p_re;
	صحيح    re_flags = rmp->regprog->re_flags;
	char_u *pat = vim_strsave(((nfa_regprog_T *)rmp->regprog)->نمونو);

	p_re = BACKTRACKING_ENGINE;
	إذا (pat != عدم)
	{
	    regprog_T *prev_prog = rmp->regprog;

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	    report_re_switch(pat);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
	    // checking for \z misuse was already done when compiling for NFA,
	    // allow all here
	    reg_do_extmatch = REX_ALL;
#جيڪڏهن_پورو
	    rmp->regprog = vim_regcomp(pat, re_flags);
#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
	    reg_do_extmatch = 0;
#جيڪڏهن_پورو
	    إذا (rmp->regprog == عدم)
	    {
		// Somehow compiling the pattern failed now, put back the
		// previous one to avoid "regprog" becoming NULL.
		rmp->regprog = prev_prog;
	    }
	    وإلا
	    {
		vim_regfree(prev_prog);

		rmp->regprog->re_اندريون_use = TRUE;
		نتيجو = rmp->regprog->engine->regexec_multi(
				      rmp, win, buf, lnum, کانو, timed_ٻاهريون);
		rmp->regprog->re_اندريون_use = FALSE;
	    }
	    vim_free(pat);
	}
	p_re = save_p_re;
    }

    rex_اندريون_use = rex_اندريون_use_save;
    إذا (rex_اندريون_use)
	rex = rex_save;

    أرجع نتيجو <= 0 ? 0 : نتيجو;
}
