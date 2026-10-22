/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * Handling of regular expressions: vim_regcomp(), vim_regexec(), vim_regsub()
 */

// By default: do not create debugging logs or files related to regular
// expressions, even when compiling with -DDEBUG.
// Uncomment the second line to get the regexp debugging.
#undef DEBUG
// #define DEBUG

#অন্তর্ভাব "vim.h"

#ifdef DEBUG
// show/save debugging data when BT engine is used
# define BT_REGEXP_DUMP
// save the debugging data to a file instead of displaying it
# define BT_REGEXP_LOG
# define BT_REGEXP_DEBUG_LOG
# define BT_REGEXP_DEBUG_LOG_NAME	"bt_regexp_debug.log"
#endif

#ifdef FEAT_RELTIME
স্থির sig_atomic_t dummy_timeout_flag = 0;
স্থির অস্থির sig_atomic_t *timeout_flag = &dummy_timeout_flag;
#endif

/*
 * Magic characters have a special meaning, they don't match literally.
 * Magic characters are negative.  This separates them from literal characters
 * (possibly multi-byte).  Only ASCII characters can be Magic.
 */
#define Magic(x)	((পূর্ণসংখ্যা)(x) - 256)
#define un_Magic(x)	((x) + 256)
#define is_Magic(x)	((x) < 0)

    স্থির পূর্ণসংখ্যা
no_Magic(পূর্ণসংখ্যা x)
{
    যদি (is_Magic(x))
	ফেরত un_Magic(x);
    ফেরত x;
}

    স্থির পূর্ণসংখ্যা
toggle_Magic(পূর্ণসংখ্যা x)
{
    যদি (is_Magic(x))
	ফেরত un_Magic(x);
    ফেরত Magic(x);
}

#যদি defined(FEAT_RELTIME)
স্থির পূর্ণসংখ্যা timeout_nesting = 0;

/*
 * Start a timer that will cause the regexp to abort after "msec".
 * This doesn't work well recursively.  In case it happens anyway, the first
 * set timeout will prevail, nested ones are ignored.
 * The caller must make sure there is a matching disable_regexp_timeout() call!
 */
    শূন্য
init_regexp_timeout(long msec)
{
    যদি (timeout_nesting == 0)
	timeout_flag = start_timeout(msec);
    ++timeout_nesting;
}

    শূন্য
disable_regexp_timeout(শূন্য)
{
    যদি (timeout_nesting == 0)
	iemsg("disable_regexp_timeout() called without active timer");
    অন্যথা যদি (--timeout_nesting == 0)
    {
	stop_timeout();
	timeout_flag = &dummy_timeout_flag;
    }
}
#endif

#যদি defined(FEAT_EVAL)
# ifdef FEAT_RELTIME
স্থির sig_atomic_t *saved_timeout_flag;
# endif

/*
 * Used at the debug prompt: disable the timeout so that expression evaluation
 * can used patterns.
 * Must be followed by calling restore_timeout_for_debugging().
 */
    শূন্য
save_timeout_for_debugging(শূন্য)
{
# ifdef FEAT_RELTIME
    saved_timeout_flag = (sig_atomic_t *)timeout_flag;
    timeout_flag = &dummy_timeout_flag;
# endif
}

    শূন্য
restore_timeout_for_debugging(শূন্য)
{
# ifdef FEAT_RELTIME
    timeout_flag = saved_timeout_flag;
# endif
}
#endif

/*
 * The first byte of the BT regexp internal "program" is actually this magic
 * number; the start node begins in the second byte.  It's used to catch the
 * most severe mutilation of the program by the caller.
 */

#define REGMAGIC	0234

/*
 * Utility definitions.
 */
#define UCHARAT(p)	((পূর্ণসংখ্যা)*(char_u *)(p))

// Used for an error (down from) vim_regcomp(): give the error message, set
// rc_did_emsg and return NULL
#define EMSG_RET_NULL(m) ফেরত (emsg((m)), rc_did_emsg = TRUE, (শূন্য *)ফাঁকা)
#define IEMSG_RET_NULL(m) ফেরত (iemsg((m)), rc_did_emsg = TRUE, (শূন্য *)ফাঁকা)
#define EMSG_RET_FAIL(m) ফেরত (emsg((m)), rc_did_emsg = TRUE, FAIL)
#define EMSG2_RET_NULL(m, c) ফেরত (semsg((ধ্রুবক অক্ষর *)(m), (c) ? "" : "\\"), rc_did_emsg = TRUE, (শূন্য *)ফাঁকা)
#define EMSG3_RET_NULL(m, c, a) ফেরত (semsg((ধ্রুবক অক্ষর *)(m), (c) ? "" : "\\", (a)), rc_did_emsg = TRUE, (শূন্য *)ফাঁকা)
#define EMSG2_RET_FAIL(m, c) ফেরত (semsg((ধ্রুবক অক্ষর *)(m), (c) ? "" : "\\"), rc_did_emsg = TRUE, FAIL)
#define EMSG_ONE_RET_NULL EMSG2_RET_NULL(_(e_invalid_item_in_str_brackets), reg_magic == MAGIC_ALL)


#define MAX_LIMIT	(32767L << 16L)

#define NOT_MULTI	0
#define MULTI_ONE	1
#define MULTI_MULT	2

// return values for regmatch()
#define RA_FAIL		1	// something failed, abort
#define RA_CONT		2	// continue in inner loop
#define RA_BREAK	3	// break inner loop
#define RA_MATCH	4	// successful match
#define RA_NOMATCH	5	// didn't match

/*
 * Return NOT_MULTI if c is not a "multi" operator.
 * Return MULTI_ONE if c is a single "multi" operator.
 * Return MULTI_MULT if c is a multi "multi" operator.
 */
    স্থির পূর্ণসংখ্যা
re_multi_type(পূর্ণসংখ্যা c)
{
    যদি (c == Magic('@') || c == Magic('=') || c == Magic('?'))
	ফেরত MULTI_ONE;
    যদি (c == Magic('*') || c == Magic('+') || c == Magic('{'))
	ফেরত MULTI_MULT;
    ফেরত NOT_MULTI;
}

স্থির char_u		*reg_পূর্ব_sub = ফাঁকা;
স্থির size_t		reg_পূর্ব_sublen = 0;

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
স্থির char_u REGEXP_INRANGE[] = "]^-n\\";
স্থির char_u REGEXP_ABBR[] = "nrtebdoxuU";

/*
 * Translate '\x' to its control character, except "\n", which is Magic.
 */
    স্থির পূর্ণসংখ্যা
backslash_trans(পূর্ণসংখ্যা c)
{
    নির্বাচন (c)
    {
	ক্ষেত্র 'r':   ফেরত CAR;
	ক্ষেত্র 't':   ফেরত TAB;
	ক্ষেত্র 'e':   ফেরত ESC;
	ক্ষেত্র 'b':   ফেরত BS;
    }
    ফেরত c;
}

গণনা
{
    CLASS_ALNUM = 0,
    CLASS_ALPHA,
    CLASS_BLANK,
    CLASS_CNTRL,
    CLASS_DIGIT,
    CLASS_GRAPH,
    CLASS_LOWER,
    CLASS_PRINT,
    CLASS_PUNCT,
    CLASS_SPACE,
    CLASS_UPPER,
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
    স্থির পূর্ণসংখ্যা
get_char_class(char_u **pp)
{
    // must be sorted by the 'value' field because it is used by bsearch()!
    স্থির keyvalue_T char_class_tab[] =
    {
	KEYVALUE_ENTRY(CLASS_ALNUM, "alnum:]"),
	KEYVALUE_ENTRY(CLASS_ALPHA, "alpha:]"),
	KEYVALUE_ENTRY(CLASS_BACKSPACE, "backspace:]"),
	KEYVALUE_ENTRY(CLASS_BLANK, "blank:]"),
	KEYVALUE_ENTRY(CLASS_CNTRL, "cntrl:]"),
	KEYVALUE_ENTRY(CLASS_DIGIT, "digit:]"),
	KEYVALUE_ENTRY(CLASS_ESCAPE, "escape:]"),
	KEYVALUE_ENTRY(CLASS_FNAME, "fname:]"),
	KEYVALUE_ENTRY(CLASS_GRAPH, "graph:]"),
	KEYVALUE_ENTRY(CLASS_IDENT, "ident:]"),
	KEYVALUE_ENTRY(CLASS_KEYWORD, "keyword:]"),
	KEYVALUE_ENTRY(CLASS_LOWER, "lower:]"),
	KEYVALUE_ENTRY(CLASS_PRINT, "print:]"),
	KEYVALUE_ENTRY(CLASS_PUNCT, "punct:]"),
	KEYVALUE_ENTRY(CLASS_RETURN, "return:]"),
	KEYVALUE_ENTRY(CLASS_SPACE, "space:]"),
	KEYVALUE_ENTRY(CLASS_TAB, "tab:]"),
	KEYVALUE_ENTRY(CLASS_UPPER, "upper:]"),
	KEYVALUE_ENTRY(CLASS_XDIGIT, "xdigit:]")
    };

    // check that the value of "pp" has a chance of matching
    যদি ((*pp)[1] == ':' && ASCII_ISLOWER((*pp)[2])
			&& ASCII_ISLOWER((*pp)[3]) && ASCII_ISLOWER((*pp)[4]))
    {
	keyvalue_T target;
	keyvalue_T *entry;
	// this function can be called repeatedly with the same value for "pp"
	// so we cache the last found entry.
	স্থির keyvalue_T *last_entry = ফাঁকা;

	target.চাবি = 0;
	target.মান.string = *pp + 2;
	target.মান.দৈর্ঘ্য = 0;	// not used, see cmp_keyvalue_value_n()

	যদি (last_entry != ফাঁকা && cmp_keyvalue_মান_n(&target, last_entry) == 0)
	    entry = last_entry;
	অন্যথা
	    entry = (keyvalue_T *)bsearch(&target, &char_class_tab,
					ARRAY_দৈর্ঘ্য(char_class_tab),
					আকার(char_class_tab[0]), cmp_keyvalue_মান_n);
	যদি (entry != ফাঁকা)
	{
	    last_entry = entry;
	    *pp += entry->মান.দৈর্ঘ্য + 2;
	    ফেরত entry->চাবি;
	}
    }
    ফেরত CLASS_NONE;
}

/*
 * Specific version of character class functions.
 * Using a table to keep this fast.
 */
স্থির short	class_tab[256];

#define	    RI_DIGIT	0x01
#define	    RI_HEX	0x02
#define	    RI_OCTAL	0x04
#define	    RI_WORD	0x08
#define	    RI_শীর্ষ	0x10
#define	    RI_ALPHA	0x20
#define	    RI_LOWER	0x40
#define	    RI_UPPER	0x80
#define	    RI_WHITE	0x100

    স্থির শূন্য
init_class_tab(শূন্য)
{
    পূর্ণসংখ্যা		i;
    স্থির পূর্ণসংখ্যা	done = FALSE;

    যদি (done)
	ফেরত;

    জন্য (i = 0; i < 256; ++i)
    {
	যদি (i >= '0' && i <= '7')
	    class_tab[i] = RI_DIGIT + RI_HEX + RI_OCTAL + RI_WORD;
	অন্যথা যদি (i >= '8' && i <= '9')
	    class_tab[i] = RI_DIGIT + RI_HEX + RI_WORD;
	অন্যথা যদি (i >= 'a' && i <= 'f')
	    class_tab[i] = RI_HEX + RI_WORD + RI_শীর্ষ + RI_ALPHA + RI_LOWER;
	অন্যথা যদি (i >= 'g' && i <= 'z')
	    class_tab[i] = RI_WORD + RI_শীর্ষ + RI_ALPHA + RI_LOWER;
	অন্যথা যদি (i >= 'A' && i <= 'F')
	    class_tab[i] = RI_HEX + RI_WORD + RI_শীর্ষ + RI_ALPHA + RI_UPPER;
	অন্যথা যদি (i >= 'G' && i <= 'Z')
	    class_tab[i] = RI_WORD + RI_শীর্ষ + RI_ALPHA + RI_UPPER;
	অন্যথা যদি (i == '_')
	    class_tab[i] = RI_WORD + RI_শীর্ষ;
	অন্যথা
	    class_tab[i] = 0;
    }
    class_tab[' '] |= RI_WHITE;
    class_tab['\t'] |= RI_WHITE;
    done = TRUE;
}

#define ri_digit(c)	((c) < 0x100 && (class_tab[c] & RI_DIGIT))
#define ri_hex(c)	((c) < 0x100 && (class_tab[c] & RI_HEX))
#define ri_octal(c)	((c) < 0x100 && (class_tab[c] & RI_OCTAL))
#define ri_word(c)	((c) < 0x100 && (class_tab[c] & RI_WORD))
#define ri_শীর্ষ(c)	((c) < 0x100 && (class_tab[c] & RI_শীর্ষ))
#define ri_alpha(c)	((c) < 0x100 && (class_tab[c] & RI_ALPHA))
#define ri_lower(c)	((c) < 0x100 && (class_tab[c] & RI_LOWER))
#define ri_upper(c)	((c) < 0x100 && (class_tab[c] & RI_UPPER))
#define ri_white(c)	((c) < 0x100 && (class_tab[c] & RI_WHITE))

// flags for regflags
#define RF_ICASE    1	// ignore case
#define RF_NOICASE  2	// don't ignore case
#define RF_HASNL    4	// can match a NL
#define RF_ICOMBINE 8	// ignore combining characters
#define RF_LOOKBH   16	// uses "\@<=" or "\@<!"

/*
 * Global work variables for vim_regcomp().
 */

স্থির char_u	*regparse;	// Input-scan pointer.
স্থির পূর্ণসংখ্যা	regnpar;	// () count.
স্থির পূর্ণসংখ্যা	wants_nfa;	// regex should use NFA engine
#ifdef FEAT_SYN_HL
স্থির পূর্ণসংখ্যা	regnzpar;	// \z() count.
স্থির পূর্ণসংখ্যা	re_has_z;	// \z item detected
#endif
স্থির unsigned	regflags;	// RF_ flags for prog
#যদি defined(FEAT_SYN_HL)
স্থির পূর্ণসংখ্যা	had_eol;	// TRUE when EOL found by vim_regcomp()
#endif

স্থির magic_T	reg_magic;	// magicness of the pattern

স্থির পূর্ণসংখ্যা	reg_string;	// matching with a string instead of a buffer
				//সারি
স্থির পূর্ণসংখ্যা	reg_strict;	// "[abc" is illegal

/*
 * META contains all characters that may be magic, except '^' and '$'.
 */

// META[] is used often enough to justify turning it into a table.
স্থির ধ্রুবক char_u META_flags[] = {
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

স্থির পূর্ণসংখ্যা	curchr;		// currently parsed character
// Previous character.  Note: prevchr is sometimes -1 when we are not at the
// start, eg in /[ ^I]^ the pattern was never found even if it existed,
// because ^ was taken to be magic -- webb
স্থির পূর্ণসংখ্যা	prevchr;
স্থির পূর্ণসংখ্যা	prevprevchr;	// previous-previous character
স্থির পূর্ণসংখ্যা	nextchr;	// used for ungetchr()

// arguments for reg()
#define REG_NOPAREN	0	// toplevel reg()
#define REG_PAREN	1	// \(\)
#define REG_ZPAREN	2	// \z(\)
#define REG_NPAREN	3	// \%(\)

// Limit recursive parsing of nested regexp atoms to avoid using up the C stack.
#define REG_MAX_PAREN_DEPTH	1000

প্রকারলেখ কাঠামো
{
    char_u	*regparse;
    পূর্ণসংখ্যা		prevchr_len;
    পূর্ণসংখ্যা		curchr;
    পূর্ণসংখ্যা		prevchr;
    পূর্ণসংখ্যা		prevprevchr;
    পূর্ণসংখ্যা		nextchr;
    পূর্ণসংখ্যা		at_start;
    পূর্ণসংখ্যা		পূর্ব_at_start;
    পূর্ণসংখ্যা		regnpar;
} parse_state_T;

স্থির শূন্য	initchr(char_u *);
স্থির পূর্ণসংখ্যা	getchr(শূন্য);
স্থির শূন্য	skipchr_keepstart(শূন্য);
স্থির পূর্ণসংখ্যা	peekchr(শূন্য);
স্থির শূন্য	skipchr(শূন্য);
স্থির শূন্য	ungetchr(শূন্য);
স্থির vimlong_T	gethexchrs(পূর্ণসংখ্যা maxinputlen);
স্থির long	getoctchrs(শূন্য);
স্থির vimlong_T	getdecchrs(শূন্য);
স্থির পূর্ণসংখ্যা	coll_get_char(শূন্য);
স্থির পূর্ণসংখ্যা	prog_magic_wrong(শূন্য);
স্থির পূর্ণসংখ্যা	cstrncmp(char_u *s1, char_u *s2, পূর্ণসংখ্যা *n);
স্থির char_u	*cstrchr(char_u *, পূর্ণসংখ্যা);
স্থির পূর্ণসংখ্যা	re_mult_পরবর্তী(অক্ষর *what);
স্থির পূর্ণসংখ্যা	reg_iswordc(পূর্ণসংখ্যা);
#ifdef FEAT_EVAL
স্থির শূন্য report_re_switch(char_u *pat);
#endif

স্থির regengine_T bt_regengine;
স্থির regengine_T nfa_regengine;

/*
 * Return TRUE if compiled regular expression "prog" can match a line break.
 */
    পূর্ণসংখ্যা
re_multiline(regprog_T *prog)
{
    ফেরত (prog->regflags & RF_HASNL);
}

/*
 * Check for an equivalence class name "[=a=]".  "pp" points to the '['.
 * Returns a character representing the class. Zero means that no item was
 * recognized.  Otherwise "pp" is advanced to after the item.
 */
    স্থির পূর্ণসংখ্যা
get_equi_class(char_u **pp)
{
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		l = 1;
    char_u	*p = *pp;

    যদি (p[1] == '=' && p[2] != NUL)
    {
	যদি (has_mbyte)
	    l = (*mb_ptr2len)(p + 2);
	যদি (p[l + 2] == '=' && p[l + 3] == ']')
	{
	    যদি (has_mbyte)
		c = mb_ptr2char(p + 2);
	    অন্যথা
		c = p[2];
	    *pp += l + 4;
	    ফেরত c;
	}
    }
    ফেরত 0;
}

/*
 * Check for a collating element "[.a.]".  "pp" points to the '['.
 * Returns a character. Zero means that no item was recognized.  Otherwise
 * "pp" is advanced to after the item.
 * Currently only single characters are recognized!
 */
    স্থির পূর্ণসংখ্যা
get_coll_element(char_u **pp)
{
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		l = 1;
    char_u	*p = *pp;

    যদি (p[0] != NUL && p[1] == '.' && p[2] != NUL)
    {
	যদি (has_mbyte)
	    l = (*mb_ptr2len)(p + 2);
	যদি (p[l + 2] == '.' && p[l + 3] == ']')
	{
	    যদি (has_mbyte)
		c = mb_ptr2char(p + 2);
	    অন্যথা
		c = p[2];
	    *pp += l + 4;
	    ফেরত c;
	}
    }
    ফেরত 0;
}

স্থির পূর্ণসংখ্যা reg_cpo_lit; // 'cpoptions' contains 'l' flag
স্থির পূর্ণসংখ্যা reg_cpo_bsl; // 'cpoptions' contains '\' flag

    স্থির শূন্য
get_cpo_flags(শূন্য)
{
    reg_cpo_lit = vim_strchr(p_cpo, CPO_LITERAL) != ফাঁকা;
    reg_cpo_bsl = vim_strchr(p_cpo, CPO_BACKSL) != ফাঁকা;
}

/*
 * Skip over a "[]" range.
 * "p" must point to the character after the '['.
 * The returned pointer is on the matching ']', or the terminating NUL.
 */
    স্থির char_u *
skip_anyof(char_u *p)
{
    পূর্ণসংখ্যা		l;

    যদি (*p == '^')	// Complement of range.
	++p;
    যদি (*p == ']' || *p == '-')
	++p;
    যখন (*p != NUL && *p != ']')
    {
	যদি (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	    p += l;
	অন্যথা
	    যদি (*p == '-')
	    {
		++p;
		যদি (*p != ']' && *p != NUL)
		    MB_PTR_ADV(p);
	    }
	অন্যথা যদি (*p == '\\'
		&& !reg_cpo_bsl
		&& (vim_strchr(REGEXP_INRANGE, p[1]) != ফাঁকা
		    || (!reg_cpo_lit && vim_strchr(REGEXP_ABBR, p[1]) != ফাঁকা)))
	    p += 2;
	অন্যথা যদি (*p == '[')
	{
	    যদি (get_char_class(&p) == CLASS_NONE
		    && get_equi_class(&p) == 0
		    && get_coll_element(&p) == 0
		    && *p != NUL)
		++p; // it is not a class name and not NUL
	}
	অন্যথা
	    ++p;
    }

    ফেরত p;
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
    পূর্ণসংখ্যা		delim,
    পূর্ণসংখ্যা		magic)
{
    ফেরত skip_regexp_ex(startp, delim, magic, ফাঁকা, ফাঁকা, ফাঁকা);
}

/*
 * Call skip_regexp() and when the delimiter does not match give an error and
 * return NULL.
 */
    char_u *
skip_regexp_err(
    char_u	*startp,
    পূর্ণসংখ্যা		delim,
    পূর্ণসংখ্যা		magic)
{
    char_u *p = skip_regexp(startp, delim, magic);

    যদি (*p != delim)
    {
	semsg(_(e_missing_delimiter_after_অনুসন্ধান_pattern_str), startp);
	ফেরত ফাঁকা;
    }
    ফেরত p;
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
    পূর্ণসংখ্যা		dirc,
    পূর্ণসংখ্যা		magic,
    char_u	**newp,
    পূর্ণসংখ্যা		*dropped,
    magic_T	*magic_val)
{
    magic_T	mymagic;
    char_u	*p = startp;
    size_t	startplen = 0;

    যদি (magic)
	mymagic = MAGIC_ON;
    অন্যথা
	mymagic = MAGIC_OFF;
    get_cpo_flags();

    জন্য (; p[0] != NUL; MB_PTR_ADV(p))
    {
	যদি (p[0] == dirc)	// found end of regexp
	    বিরতি;
	যদি ((p[0] == '[' && mymagic >= MAGIC_ON)
		|| (p[0] == '\\' && p[1] == '[' && mymagic <= MAGIC_OFF))
	{
	    p = skip_anyof(p + 1);
	    যদি (p[0] == NUL)
		বিরতি;
	}
	অন্যথা যদি (p[0] == '\\' && p[1] != NUL)
	{
	    যদি (dirc == '?' && newp != ফাঁকা && p[1] == '?')
	    {
		// change "\?" to "?", make a copy first.
		যদি (startplen == 0)
		    startplen = STRLEN(startp);
		যদি (*newp == ফাঁকা)
		{
		    *newp = vim_strnsave(startp, startplen);
		    যদি (*newp != ফাঁকা)
		    {
			p = *newp + (p - startp);
			startp = *newp;
		    }
		}
		যদি (dropped != ফাঁকা)
		    ++*dropped;
		যদি (*newp != ফাঁকা)
		    mch_memmove(p, p + 1, startplen - ((p + 1) - startp) + 1);
		অন্যথা
		    ++p;
	    }
	    অন্যথা
		++p;    // skip next character
	    যদি (*p == 'v')
		mymagic = MAGIC_ALL;
	    অন্যথা যদি (*p == 'V')
		mymagic = MAGIC_NONE;
	}
    }
    যদি (magic_val != ফাঁকা)
	*magic_val = mymagic;
    ফেরত p;
}

/*
 * Functions for getting characters from the regexp input.
 */
স্থির পূর্ণসংখ্যা	prevchr_len;	// byte length of previous char
স্থির পূর্ণসংখ্যা	at_start;	// True when on the first character
স্থির পূর্ণসংখ্যা	পূর্ব_at_start;  // True when on the second character

/*
 * Start parsing at "str".
 */
    স্থির শূন্য
initchr(char_u *str)
{
    regparse = str;
    prevchr_len = 0;
    curchr = prevprevchr = prevchr = nextchr = -1;
    at_start = TRUE;
    পূর্ব_at_start = FALSE;
}

/*
 * Save the current parse state, so that it can be restored and parsing
 * starts in the same state again.
 */
    স্থির শূন্য
save_parse_state(parse_state_T *ps)
{
    ps->regparse = regparse;
    ps->prevchr_len = prevchr_len;
    ps->curchr = curchr;
    ps->prevchr = prevchr;
    ps->prevprevchr = prevprevchr;
    ps->nextchr = nextchr;
    ps->at_start = at_start;
    ps->পূর্ব_at_start = পূর্ব_at_start;
    ps->regnpar = regnpar;
}

/*
 * Restore a previously saved parse state.
 */
    স্থির শূন্য
restore_parse_state(parse_state_T *ps)
{
    regparse = ps->regparse;
    prevchr_len = ps->prevchr_len;
    curchr = ps->curchr;
    prevchr = ps->prevchr;
    prevprevchr = ps->prevprevchr;
    nextchr = ps->nextchr;
    at_start = ps->at_start;
    পূর্ব_at_start = ps->পূর্ব_at_start;
    regnpar = ps->regnpar;
}


/*
 * Get the next character without advancing.
 */
    স্থির পূর্ণসংখ্যা
peekchr(শূন্য)
{
    স্থির পূর্ণসংখ্যা	after_slash = FALSE;

    যদি (curchr != -1)
	ফেরত curchr;

    নির্বাচন (curchr = regparse[0])
    {
	ক্ষেত্র '.':
	ক্ষেত্র '[':
	ক্ষেত্র '~':
	    // magic when 'magic' is on
	    যদি (reg_magic >= MAGIC_ON)
		curchr = Magic(curchr);
	    বিরতি;
	ক্ষেত্র '(':
	ক্ষেত্র ')':
	ক্ষেত্র '{':
	ক্ষেত্র '%':
	ক্ষেত্র '+':
	ক্ষেত্র '=':
	ক্ষেত্র '?':
	ক্ষেত্র '@':
	ক্ষেত্র '!':
	ক্ষেত্র '&':
	ক্ষেত্র '|':
	ক্ষেত্র '<':
	ক্ষেত্র '>':
	ক্ষেত্র '#':	// future ext.
	ক্ষেত্র '"':	// future ext.
	ক্ষেত্র '\'':	// future ext.
	ক্ষেত্র ',':	// future ext.
	ক্ষেত্র '-':	// future ext.
	ক্ষেত্র ':':	// future ext.
	ক্ষেত্র ';':	// future ext.
	ক্ষেত্র '`':	// future ext.
	ক্ষেত্র '/':	// Can't be used in / command
			// magic only after "\v"
	    যদি (reg_magic == MAGIC_ALL)
		curchr = Magic(curchr);
	    বিরতি;
	ক্ষেত্র '*':
	    // * is not magic as the very first character, eg "?*ptr", when
	    // after '^', eg "/^*ptr" and when after "\(", "\|", "\&".  But
	    // "\(\*" is not magic, thus must be magic if "after_slash"
	    যদি (reg_magic >= MAGIC_ON
		    && !at_start
		    && !(পূর্ব_at_start && prevchr == Magic('^'))
		    && (after_slash
			|| (prevchr != Magic('(')
			    && prevchr != Magic('&')
			    && prevchr != Magic('|'))))
		curchr = Magic('*');
	    বিরতি;
	ক্ষেত্র '^':
	    // '^' is only magic as the very first character and if it's after
	    // "\(", "\|", "\&' or "\n"
	    যদি (reg_magic >= MAGIC_OFF
		    && (at_start
			|| reg_magic == MAGIC_ALL
			|| prevchr == Magic('(')
			|| prevchr == Magic('|')
			|| prevchr == Magic('&')
			|| prevchr == Magic('n')
			|| (no_Magic(prevchr) == '('
			    && prevprevchr == Magic('%'))))
	    {
		curchr = Magic('^');
		at_start = TRUE;
		পূর্ব_at_start = FALSE;
	    }
	    বিরতি;
	ক্ষেত্র '$':
	    // '$' is only magic as the very last char and if it's in front of
	    // either "\|", "\)", "\&", or "\n"
	    যদি (reg_magic >= MAGIC_OFF)
	    {
		char_u *p = regparse + 1;
		পূর্ণসংখ্যা is_magic_all = (reg_magic == MAGIC_ALL);

		// ignore \c \C \m \M \v \V and \Z after '$'
		যখন (p[0] == '\\' && (p[1] == 'c' || p[1] == 'C'
			    || p[1] == 'm' || p[1] == 'M'
			    || p[1] == 'v' || p[1] == 'V' || p[1] == 'Z'))
		{
		    যদি (p[1] == 'v')
			is_magic_all = TRUE;
		    অন্যথা যদি (p[1] == 'm' || p[1] == 'M' || p[1] == 'V')
			is_magic_all = FALSE;
		    p += 2;
		}
		যদি (p[0] == NUL
			|| (p[0] == '\\'
			    && (p[1] == '|' || p[1] == '&' || p[1] == ')'
				|| p[1] == 'n'))
			|| (is_magic_all
			    && (p[0] == '|' || p[0] == '&' || p[0] == ')'))
			|| reg_magic == MAGIC_ALL)
		    curchr = Magic('$');
	    }
	    বিরতি;
	ক্ষেত্র '\\':
	    {
		পূর্ণসংখ্যা c = regparse[1];

		যদি (c == NUL)
		    curchr = '\\';	// trailing '\'
		অন্যথা যদি (c <= '~' && META_flags[c])
		{
		    /*
		     * META contains everything that may be magic sometimes,
		     * except ^ and $ ("\^" and "\$" are only magic after
		     * "\V").  We now fetch the next character and toggle its
		     * magicness.  Therefore, \ is so meta-magic that it is
		     * not in META.
		     */
		    curchr = -1;
		    পূর্ব_at_start = at_start;
		    at_start = FALSE;	// be able to say "/\*ptr"
		    ++regparse;
		    ++after_slash;
		    peekchr();
		    --regparse;
		    --after_slash;
		    curchr = toggle_Magic(curchr);
		}
		অন্যথা যদি (vim_strchr(REGEXP_ABBR, c))
		{
		    /*
		     * Handle abbreviations, like "\t" for TAB -- webb
		     */
		    curchr = backslash_trans(c);
		}
		অন্যথা যদি (reg_magic == MAGIC_NONE && (c == '$' || c == '^'))
		    curchr = toggle_Magic(c);
		অন্যথা
		{
		    /*
		     * Next character can never be (made) magic?
		     * Then backslashing it won't do anything.
		     */
		    যদি (has_mbyte && c >= 0x80)
			curchr = (*mb_ptr2char)(regparse + 1);
		    অন্যথা
			curchr = c;
		}
		বিরতি;
	    }

	পূর্বনির্ধারিত:
	    // curchr already holds regparse[0]; only a multi-byte lead byte
	    // needs decoding.
	    যদি (has_mbyte && curchr >= 0x80)
		curchr = (*mb_ptr2char)(regparse);
    }

    ফেরত curchr;
}

/*
 * Eat one lexed character.  Do this in a way that we can undo it.
 */
    স্থির শূন্য
skipchr(শূন্য)
{
    // peekchr() eats a backslash, do the same here
    যদি (*regparse == '\\')
	prevchr_len = 1;
    অন্যথা
	prevchr_len = 0;
    যদি (regparse[prevchr_len] != NUL)
    {
	যদি (enc_utf8)
	{
	    // Exclude composing chars that mb_ptr2len does include.  A byte
	    // below 0x80 is always a single character.
	    যদি (regparse[prevchr_len] < 0x80)
		++prevchr_len;
	    অন্যথা
		prevchr_len += utf_ptr2len(regparse + prevchr_len);
	}
	অন্যথা যদি (has_mbyte)
	    prevchr_len += (*mb_ptr2len)(regparse + prevchr_len);
	অন্যথা
	    ++prevchr_len;
    }
    regparse += prevchr_len;
    পূর্ব_at_start = at_start;
    at_start = FALSE;
    prevprevchr = prevchr;
    prevchr = curchr;
    curchr = nextchr;	    // use previously unget char, or -1
    nextchr = -1;
}

/*
 * Skip a character while keeping the value of prev_at_start for at_start.
 * prevchr and prevprevchr are also kept.
 */
    স্থির শূন্য
skipchr_keepstart(শূন্য)
{
    পূর্ণসংখ্যা as = পূর্ব_at_start;
    পূর্ণসংখ্যা pr = prevchr;
    পূর্ণসংখ্যা prpr = prevprevchr;

    skipchr();
    at_start = as;
    prevchr = pr;
    prevprevchr = prpr;
}

/*
 * Get the next character from the pattern. We know about magic and such, so
 * therefore we need a lexical analyzer.
 */
    স্থির পূর্ণসংখ্যা
getchr(শূন্য)
{
    পূর্ণসংখ্যা chr = peekchr();

    skipchr();
    ফেরত chr;
}

/*
 * put character back.  Works only once!
 */
    স্থির শূন্য
ungetchr(শূন্য)
{
    nextchr = curchr;
    curchr = prevchr;
    prevchr = prevprevchr;
    at_start = পূর্ব_at_start;
    পূর্ব_at_start = FALSE;

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
    স্থির vimlong_T
gethexchrs(পূর্ণসংখ্যা maxinputlen)
{
    long_u	nr = 0;
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		i;

    জন্য (i = 0; i < maxinputlen; ++i)
    {
	c = regparse[0];
	যদি (!vim_isxdigit(c))
	    বিরতি;
	nr <<= 4;
	nr |= hex2nr(c);
	++regparse;
    }

    যদি (i == 0)
	ফেরত -1;
    ফেরত nr;
}

/*
 * Get and return the value of the decimal string immediately after the
 * current position. Return -1 for invalid.  Consumes all digits.
 */
    স্থির vimlong_T
getdecchrs(শূন্য)
{
    long_u	nr = 0;
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		i;

    জন্য (i = 0; ; ++i)
    {
	c = regparse[0];
	যদি (c < '0' || c > '9')
	    বিরতি;
	nr *= 10;
	nr += c - '0';
	++regparse;
	curchr = -1; // no longer valid
    }

    যদি (i == 0)
	ফেরত -1;
    ফেরত nr;
}

/*
 * get and return the value of the octal string immediately after the current
 * position. Return -1 for invalid, or 0-255 for valid. Smart enough to handle
 * numbers > 377 correctly (for example, 400 is treated as 40) and doesn't
 * treat 8 or 9 as recognised characters. Position is updated:
 *     blahblah\%o210asdf
 *	   before-^  ^-after
 */
    স্থির long
getoctchrs(শূন্য)
{
    long_u	nr = 0;
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		i;

    জন্য (i = 0; i < 3 && nr < 040; ++i)
    {
	c = regparse[0];
	যদি (c < '0' || c > '7')
	    বিরতি;
	nr <<= 3;
	nr |= hex2nr(c);
	++regparse;
    }

    যদি (i == 0)
	ফেরত -1;
    ফেরত (long)nr;
}

/*
 * read_limits - Read two integers to be taken as a minimum and maximum.
 * If the first character is '-', then the range is reversed.
 * Should end with 'end'.  If minval is missing, zero is default, if maxval is
 * missing, a very big number is the default.
 */
    স্থির পূর্ণসংখ্যা
read_limits(long *minval, long *maxval)
{
    পূর্ণসংখ্যা		reverse = FALSE;
    char_u	*first_char;
    long	tmp;

    যদি (*regparse == '-')
    {
	// Starts with '-', so reverse the range later
	regparse++;
	reverse = TRUE;
    }
    first_char = regparse;
    *minval = getdigits(&regparse);
    যদি (*regparse == ',')	    // There is a comma
    {
	যদি (vim_isdigit(*++regparse))
	    *maxval = getdigits(&regparse);
	অন্যথা
	    *maxval = MAX_LIMIT;
    }
    অন্যথা যদি (VIM_ISDIGIT(*first_char))
	*maxval = *minval;	    // It was \{n} or \{-n}
    অন্যথা
	*maxval = MAX_LIMIT;	    // It was \{} or \{-}
    যদি (*regparse == '\\')
	regparse++;	// Allow either \{...} or \{...\}
    যদি (*regparse != '}')
	EMSG2_RET_FAIL(_(e_syntax_error_in_str_curlies),
						       reg_magic == MAGIC_ALL);

    /*
     * Reverse the range if there was a '-', or make sure it is in the right
     * order otherwise.
     */
    যদি ((!reverse && *minval > *maxval) || (reverse && *minval < *maxval))
    {
	tmp = *minval;
	*minval = *maxval;
	*maxval = tmp;
    }
    skipchr();		// let's be friends with the lexer again
    ফেরত OK;
}

/*
 * vim_regexec and friends
 */

/*
 * Global work variables for vim_regexec().
 */

স্থির শূন্য	cleanup_subexpr(শূন্য);
#ifdef FEAT_SYN_HL
স্থির শূন্য	cleanup_zsubexpr(শূন্য);
#endif
স্থির পূর্ণসংখ্যা	match_with_backref(linenr_T start_lnum, colnr_T start_col, linenr_T end_lnum, colnr_T end_col, পূর্ণসংখ্যা *bytelen);

/*
 * Sometimes need to save a copy of a line.  Since alloc()/free() is very
 * slow, we keep one allocated piece of memory and only re-allocate it when
 * it's too small.  It's freed in bt_regexec_both() when finished.
 */
স্থির char_u	*reg_tofree = ফাঁকা;
স্থির unsigned	reg_tofreelen;

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
প্রকারলেখ কাঠামো {
    regmatch_T		*reg_match;
    regmmatch_T		*reg_mmatch;

    char_u		**reg_startp;
    char_u		**reg_endp;
    lpos_T		*reg_startpos;
    lpos_T		*reg_endpos;

    win_T		*reg_win;
    buf_T		*reg_buf;
    linenr_T		reg_firstlnum;
    linenr_T		reg_maxline;
    পূর্ণসংখ্যা			reg_সারি_lbr;	// "\n" in string is line break

    // The current match-position is stord in these variables:
    linenr_T	lnum;		// line number, relative to first line
    char_u	*সারি;		// start of current line
    char_u	*প্রবেশ;		// current input, points into "line"

    পূর্ণসংখ্যা	need_clear_subexpr;	// subexpressions still need to be cleared
#ifdef FEAT_SYN_HL
    পূর্ণসংখ্যা	need_clear_zsubexpr;	// extmatch subexpressions still need to be
				// cleared
#endif

    // Internal copy of 'ignorecase'.  It is set at each call to vim_regexec().
    // Normally it gets the value of "rm_ic" or "rmm_ic", but when the pattern
    // contains '\c' or '\C' the value is overruled.
    পূর্ণসংখ্যা			reg_ic;

    // Similar to "reg_ic", but only for 'combining' characters.  Set with \Z
    // flag in the regexp.  Defaults to false, always.
    পূর্ণসংখ্যা			reg_icombine;

    // Copy of "rmm_maxcol": maximum column to search for a match.  Zero when
    // there is no maximum.
    colnr_T		reg_maxcol;

    // State for the NFA engine regexec.
    পূর্ণসংখ্যা nfa_has_zend;	    // NFA regexp \ze operator encountered.
    পূর্ণসংখ্যা nfa_has_backref;    // NFA regexp \1 .. \9 encountered.
    পূর্ণসংখ্যা nfa_nsubexpr;	    // Number of sub expressions actually being used
			    // during execution. 1 if only the whole match
			    // (subexpr 0) is used.
    // listid is global, so that it increases on recursive calls to
    // nfa_regmatch(), which means we don't have to clear the lastlist field of
    // all the states.
    পূর্ণসংখ্যা nfa_listid;
    পূর্ণসংখ্যা nfa_alt_listid;

#ifdef FEAT_SYN_HL
    পূর্ণসংখ্যা nfa_has_zsubexpr;   // NFA regexp has \z( ), set zsubexpr.
#endif
} regexec_T;

স্থির regexec_T	rex;
স্থির পূর্ণসংখ্যা		rex_in_use = FALSE;

/*
 * Return TRUE if character 'c' is included in 'iskeyword' option for
 * "reg_buf" buffer.
 */
    স্থির পূর্ণসংখ্যা
reg_iswordc(পূর্ণসংখ্যা c)
{
    ফেরত vim_iswordc_buf(c, rex.reg_buf);
}

#ifdef FEAT_EVAL
স্থির পূর্ণসংখ্যা can_f_submatch = FALSE;	// TRUE when submatch() can be used

// This struct is used for reg_submatch(). Needed for when the
// substitution string is an expression that contains a call to substitute()
// and submatch().
প্রকারলেখ কাঠামো {
    regmatch_T	*sm_match;
    regmmatch_T	*sm_mmatch;
    linenr_T	sm_firstlnum;
    linenr_T	sm_maxline;
    পূর্ণসংখ্যা		sm_সারি_lbr;
} regsubmatch_T;

স্থির regsubmatch_T rsm;  // can only be used when can_f_submatch is TRUE
#endif

প্রকারলেখ গণনা
{
    RGLF_সারি = 0x01,
    RGLF_দৈর্ঘ্য = 0x02
#ifdef FEAT_EVAL
    ,
    RGLF_SUBMATCH = 0x04
#endif
} reg_getline_flags_T;

//
// common code for reg_getline(), reg_getline_len(), reg_getline_submatch() and
// reg_getline_submatch_len().
// the flags argument (which is a bitmask) controls what info is to be returned and whether
// or not submatch is in effect.
// note:
//     submatch is available only if FEAT_EVAL is defined.
    স্থির শূন্য
reg_getline_common(
    linenr_T		lnum,
    reg_getline_flags_T	flags,
    char_u		**সারি,
    colnr_T		*দৈর্ঘ্য)
{
    পূর্ণসংখ্যা get_সারি = flags & RGLF_সারি;
    পূর্ণসংখ্যা get_দৈর্ঘ্য = flags & RGLF_দৈর্ঘ্য;
    linenr_T firstlnum;
    linenr_T maxline;

#ifdef FEAT_EVAL
    যদি (flags & RGLF_SUBMATCH)
    {
	firstlnum = rsm.sm_firstlnum + lnum;
	maxline = rsm.sm_maxline;
    }
    অন্যথা
#endif
    {
	firstlnum = rex.reg_firstlnum + lnum;
	maxline = rex.reg_maxline;
    }

    // when looking behind for a match/no-match lnum is negative. but we
    // can't go before line 1.
    যদি (firstlnum < 1)
    {
	যদি (get_সারি)
	    *সারি = ফাঁকা;
	যদি (get_দৈর্ঘ্য)
	    *দৈর্ঘ্য = 0;

	ফেরত;
    }

    যদি (lnum > maxline)
    {
	// must have matched the "\n" in the last line.
	যদি (get_সারি)
	    *সারি = (char_u *)"";
	যদি (get_দৈর্ঘ্য)
	    *দৈর্ঘ্য = 0;

	ফেরত;
    }

    যদি (get_সারি)
	*সারি = ml_get_buf(rex.reg_buf, firstlnum, FALSE);
    যদি (get_দৈর্ঘ্য)
	*দৈর্ঘ্য = ml_get_buf_len(rex.reg_buf, firstlnum);
}

/*
 * Get pointer to the line "lnum", which is relative to "reg_firstlnum".
 */
    স্থির char_u *
reg_getline(linenr_T lnum)
{
    char_u *সারি;

    reg_getline_common(lnum, RGLF_সারি, &সারি, ফাঁকা);

    ফেরত সারি;
}

/*
 * Get length of line "lnum", which is relative to "reg_firstlnum".
 */
    স্থির colnr_T
reg_getline_len(linenr_T lnum)
{
    colnr_T দৈর্ঘ্য;

    reg_getline_common(lnum, RGLF_দৈর্ঘ্য, ফাঁকা, &দৈর্ঘ্য);

    ফেরত দৈর্ঘ্য;
}

#ifdef FEAT_SYN_HL
স্থির char_u	*reg_startzp[NSUBEXP];	// Workspace to mark beginning
স্থির char_u	*reg_endzp[NSUBEXP];	//   and end of \z(...\) matches
স্থির lpos_T	reg_startzpos[NSUBEXP];	// idem, beginning pos
স্থির lpos_T	reg_endzpos[NSUBEXP];	// idem, end pos
#endif

// TRUE if using multi-line regexp.
#define REG_MULTI	(rex.reg_match == ফাঁকা)

#ifdef FEAT_SYN_HL
/*
 * Create a new extmatch and mark it as referenced once.
 */
    স্থির reg_extmatch_T *
make_extmatch(শূন্য)
{
    reg_extmatch_T	*em;

    em = ALLOC_CLEAR_ONE(reg_extmatch_T);
    যদি (em != ফাঁকা)
	em->refcnt = 1;
    ফেরত em;
}

/*
 * Add a reference to an extmatch.
 */
    reg_extmatch_T *
ref_extmatch(reg_extmatch_T *em)
{
    যদি (em != ফাঁকা)
	em->refcnt++;
    ফেরত em;
}

/*
 * Remove a reference to an extmatch.  If there are no references left, free
 * the info.
 */
    শূন্য
unref_extmatch(reg_extmatch_T *em)
{
    পূর্ণসংখ্যা i;

    যদি (em != ফাঁকা && --em->refcnt <= 0)
    {
	জন্য (i = 0; i < NSUBEXP; ++i)
	    vim_free(em->matches[i]);
	vim_free(em);
    }
}
#endif

/*
 * Get class of previous character.
 */
    স্থির পূর্ণসংখ্যা
reg_পূর্ব_class(শূন্য)
{
    যদি (rex.প্রবেশ > rex.সারি)
	ফেরত mb_get_class_buf(rex.প্রবেশ - 1
		       - (*mb_শীর্ষ_off)(rex.সারি, rex.প্রবেশ - 1), rex.reg_buf);
    ফেরত -1;
}

/*
 * Return TRUE if the current rex.input position matches the Visual area.
 */
    স্থির পূর্ণসংখ্যা
reg_match_visual(শূন্য)
{
    pos_T	শীর্ষ_2, bot;
    linenr_T    lnum;
    colnr_T	col;
    win_T	*wp = rex.reg_win == ফাঁকা ? curwin : rex.reg_win;
    পূর্ণসংখ্যা		mode;
    colnr_T	start, end;
    colnr_T	start2, end2;
    colnr_T	cols;
    colnr_T	curswant;

    // Check if the buffer is the current buffer and not using a string.
    যদি (rex.reg_buf != curbuf || VIsual.lnum == 0 || !REG_MULTI)
	ফেরত FALSE;

    যদি (VIsual_active)
    {
	যদি (LT_POS(VIsual, wp->w_cursor))
	{
	    শীর্ষ_2 = VIsual;
	    bot = wp->w_cursor;
	}
	অন্যথা
	{
	    শীর্ষ_2 = wp->w_cursor;
	    bot = VIsual;
	}
	mode = VIsual_mode;
	curswant = wp->w_curswant;
    }
    অন্যথা
    {
	যদি (LT_POS(curbuf->b_visual.vi_start, curbuf->b_visual.vi_end))
	{
	    শীর্ষ_2 = curbuf->b_visual.vi_start;
	    bot = curbuf->b_visual.vi_end;
	}
	অন্যথা
	{
	    শীর্ষ_2 = curbuf->b_visual.vi_end;
	    bot = curbuf->b_visual.vi_start;
	}
	// a substitute command may have removed some lines
	যদি (bot.lnum > curbuf->b_ml.ml_সারি_গুনতি)
	    bot.lnum = curbuf->b_ml.ml_সারি_গুনতি;
	mode = curbuf->b_visual.vi_mode;
	curswant = curbuf->b_visual.vi_curswant;
    }
    lnum = rex.lnum + rex.reg_firstlnum;
    যদি (lnum < শীর্ষ_2.lnum || lnum > bot.lnum)
	ফেরত FALSE;

    col = (colnr_T)(rex.প্রবেশ - rex.সারি);
    যদি (mode == 'v')
    {
	যদি ((lnum == শীর্ষ_2.lnum && col < শীর্ষ_2.col)
		|| (lnum == bot.lnum && col >= bot.col + (*p_sel != 'e')))
	    ফেরত FALSE;
    }
    অন্যথা যদি (mode == Ctrl_V)
    {
	getvvcol(wp, &শীর্ষ_2, &start, ফাঁকা, &end, 0);
	getvvcol(wp, &bot, &start2, ফাঁকা, &end2, 0);
	যদি (start2 < start)
	    start = start2;
	যদি (end2 > end)
	    end = end2;
	যদি (শীর্ষ_2.col == MAXCOL || bot.col == MAXCOL || curswant == MAXCOL)
	    end = MAXCOL;

	// getvvcol() flushes rex.line, need to get it again
	rex.সারি = reg_getline(rex.lnum);
	rex.প্রবেশ = rex.সারি + col;

	cols = win_linetabsize(wp, rex.reg_firstlnum + rex.lnum, rex.সারি, col);
	যদি (cols < start || cols > end - (*p_sel == 'e'))
	    ফেরত FALSE;
    }
    ফেরত TRUE;
}

/*
 * Check the regexp program for its magic number.
 * Return TRUE if it's wrong.
 */
    স্থির পূর্ণসংখ্যা
prog_magic_wrong(শূন্য)
{
    regprog_T	*prog;

    prog = REG_MULTI ? rex.reg_mmatch->regprog : rex.reg_match->regprog;
    যদি (prog->engine == &nfa_regengine)
	// For NFA matcher we don't check the magic
	ফেরত FALSE;

    যদি (UCHARAT(((bt_regprog_T *)prog)->program) != REGMAGIC)
    {
	iemsg(e_corrupted_regexp_program);
	ফেরত TRUE;
    }
    ফেরত FALSE;
}

/*
 * Cleanup the subexpressions, if this wasn't done yet.
 * This construction is used to clear the subexpressions only when they are
 * used (to increase speed).
 */
    স্থির শূন্য
cleanup_subexpr(শূন্য)
{
    যদি (!rex.need_clear_subexpr)
	ফেরত;

    যদি (REG_MULTI)
    {
	// Use 0xff to set lnum to -1
	vim_memset(rex.reg_startpos, 0xff, আকার(lpos_T) * NSUBEXP);
	vim_memset(rex.reg_endpos, 0xff, আকার(lpos_T) * NSUBEXP);
    }
    অন্যথা
    {
	vim_memset(rex.reg_startp, 0, আকার(char_u *) * NSUBEXP);
	vim_memset(rex.reg_endp, 0, আকার(char_u *) * NSUBEXP);
    }
    rex.need_clear_subexpr = FALSE;
}

#ifdef FEAT_SYN_HL
    স্থির শূন্য
cleanup_zsubexpr(শূন্য)
{
    যদি (!rex.need_clear_zsubexpr)
	ফেরত;

    যদি (REG_MULTI)
    {
	// Use 0xff to set lnum to -1
	vim_memset(reg_startzpos, 0xff, আকার(lpos_T) * NSUBEXP);
	vim_memset(reg_endzpos, 0xff, আকার(lpos_T) * NSUBEXP);
    }
    অন্যথা
    {
	vim_memset(reg_startzp, 0, আকার(char_u *) * NSUBEXP);
	vim_memset(reg_endzp, 0, আকার(char_u *) * NSUBEXP);
    }
    rex.need_clear_zsubexpr = FALSE;
}
#endif

/*
 * Advance rex.lnum, rex.line and rex.input to the next line.
 */
    স্থির শূন্য
reg_nextline(শূন্য)
{
    rex.সারি = reg_getline(++rex.lnum);
    rex.প্রবেশ = rex.সারি;
    fast_breakcheck();
}

/*
 * Check whether a backreference matches.
 * Returns RA_FAIL, RA_NOMATCH or RA_MATCH.
 * If "bytelen" is not NULL, it is set to the byte length of the match in the
 * last line.
 * Optional: ignore case if rex.reg_ic is set.
 */
    স্থির পূর্ণসংখ্যা
match_with_backref(
    linenr_T start_lnum,
    colnr_T  start_col,
    linenr_T end_lnum,
    colnr_T  end_col,
    পূর্ণসংখ্যা	     *bytelen)
{
    linenr_T	clnum = start_lnum;
    colnr_T	ccol = start_col;
    পূর্ণসংখ্যা		len;
    char_u	*p;

    যদি (bytelen != ফাঁকা)
	*bytelen = 0;
    জন্য (;;)
    {
	// Since getting one line may invalidate the other, need to make copy.
	// Slow!
	যদি (rex.সারি != reg_tofree)
	{
	    len = (পূর্ণসংখ্যা)STRLEN(rex.সারি);
	    যদি (reg_tofree == ফাঁকা || len >= (পূর্ণসংখ্যা)reg_tofreelen)
	    {
		len += 50;	// get some extra
		vim_free(reg_tofree);
		reg_tofree = alloc(len);
		যদি (reg_tofree == ফাঁকা)
		    ফেরত RA_FAIL; // out of memory!
		reg_tofreelen = len;
	    }
	    STRCPY(reg_tofree, rex.সারি);
	    rex.প্রবেশ = reg_tofree + (rex.প্রবেশ - rex.সারি);
	    rex.সারি = reg_tofree;
	}

	// Get the line to compare with.
	p = reg_getline(clnum);
	যদি (clnum == end_lnum)
	    len = end_col - ccol;
	অন্যথা
	    len = (পূর্ণসংখ্যা)reg_getline_len(clnum) - ccol;

	// Use case-insensitive compare if rex.reg_ic is set
	যদি ((!rex.reg_ic && cstrncmp(p + ccol, rex.প্রবেশ, &len) != 0)
	    || (rex.reg_ic && MB_STRNICMP(p + ccol, rex.প্রবেশ, len) != 0))
	    ফেরত RA_NOMATCH;  // doesn't match
	যদি (bytelen != ফাঁকা)
	    *bytelen += len;
	যদি (clnum == end_lnum)
	    বিরতি;		// match and at end!
	যদি (rex.lnum >= rex.reg_maxline)
	    ফেরত RA_NOMATCH;  // text too short

	// Advance to next line.
	reg_nextline();
	যদি (bytelen != ফাঁকা)
	    *bytelen = 0;
	++clnum;
	ccol = 0;
	যদি (got_int)
	    ফেরত RA_FAIL;
    }

    // found a match!  Note that rex.line may now point to a copy of the line,
    // that should not matter.
    ফেরত RA_MATCH;
}

/*
 * Used in a place where no * or \+ can follow.
 */
    স্থির পূর্ণসংখ্যা
re_mult_পরবর্তী(অক্ষর *what)
{
    যদি (re_multi_type(peekchr()) == MULTI_MULT)
    {
	semsg(_(e_nfa_regexp_cannot_repeat_str), what);
	rc_did_emsg = TRUE;
	ফেরত FAIL;
    }
    ফেরত OK;
}

প্রকারলেখ কাঠামো
{
    পূর্ণসংখ্যা a, b, c;
} decomp_T;


// 0xfb20 - 0xfb4f
স্থির decomp_T decomp_table[0xfb4f-0xfb20+1] =
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

    স্থির শূন্য
mb_decompose(পূর্ণসংখ্যা c, পূর্ণসংখ্যা *c1, পূর্ণসংখ্যা *c2, পূর্ণসংখ্যা *c3)
{
    decomp_T d;

    যদি (c >= 0xfb20 && c <= 0xfb4f)
    {
	d = decomp_table[c - 0xfb20];
	*c1 = d.a;
	*c2 = d.b;
	*c3 = d.c;
    }
    অন্যথা
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
    স্থির পূর্ণসংখ্যা
cstrncmp(char_u *s1, char_u *s2, পূর্ণসংখ্যা *n)
{
    পূর্ণসংখ্যা		ফলাফল;

    যদি (!rex.reg_ic)
	ফলাফল = STRNCMP(s1, s2, *n);
    অন্যথা যদি (enc_utf8)
    {
	char_u *p = s1;
	পূর্ণসংখ্যা n2 = 0;
	পূর্ণসংখ্যা n1 = *n;
	// count the number of characters for byte-length of s1
	যখন (n1 > 0 && *p != NUL)
	{
	    n1 -= mb_ptr2len(p);
	    MB_PTR_ADV(p);
	    n2++;
	}
	// count the number of bytes to advance the same number of chars for s2
	p = s2;
	যখন (n2-- > 0 && *p != NUL)
	    MB_PTR_ADV(p);

	n2 = p - s2;

	ফলাফল = MB_STRNICMP2(s1, s2, *n, n2);
	যদি (ফলাফল == 0)
	    *n = n2;
    }
    অন্যথা
	ফলাফল = MB_STRNICMP(s1, s2, *n);

    // if it failed and it's utf8 and we want to combineignore:
    যদি (ফলাফল != 0 && enc_utf8 && rex.reg_icombine)
    {
	char_u	*str1, *str2;
	পূর্ণসংখ্যা	c1, c2, c11, c12;
	পূর্ণসংখ্যা	junk;

	// we have to handle the strcmp ourselves, since it is necessary to
	// deal with the composing characters by ignoring them:
	str1 = s1;
	str2 = s2;
	c1 = c2 = 0;
	যখন ((পূর্ণসংখ্যা)(str1 - s1) < *n)
	{
	    c1 = mb_ptr2char_adv(&str1);
	    c2 = mb_ptr2char_adv(&str2);

	    // Decompose the character if necessary, into 'base' characters.
	    // Currently hard-coded for Hebrew, Arabic to be done...
	    যদি (c1 != c2 && (!rex.reg_ic || utf_fold(c1) != utf_fold(c2)))
	    {
		// decomposition necessary?
		mb_decompose(c1, &c11, &junk, &junk);
		mb_decompose(c2, &c12, &junk, &junk);
		c1 = c11;
		c2 = c12;
		যদি (c11 != c12
			    && (!rex.reg_ic || utf_fold(c11) != utf_fold(c12)))
		    বিরতি;
	    }
	}
	ফলাফল = c2 - c1;
	যদি (ফলাফল == 0)
	    *n = (পূর্ণসংখ্যা)(str2 - s2);
    }

    ফেরত ফলাফল;
}

/*
 * cstrchr: This function is used a lot for simple searches, keep it fast!
 */
    স্থির char_u *
cstrchr(char_u *s, পূর্ণসংখ্যা c)
{
    char_u	*p;
    পূর্ণসংখ্যা		cc, lc;

    যদি (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	ফেরত vim_strchr(s, c);

    // tolower() and toupper() can be slow, comparing twice should be a lot
    // faster (esp. when using MS Visual C++!).
    // For UTF-8 need to use folded case.
    যদি (enc_utf8 && c > 0x80)
    {
	cc = utf_fold(c);
	lc = cc;
    }
    অন্যথা
	যদি (MB_ISUPPER(c))
	{
	    cc = MB_TOLOWER(c);
	    lc = cc;
	}
	অন্যথা যদি (MB_ISLOWER(c))
	{
	    cc = MB_TOUPPER(c);
	    lc = c;
	}
	অন্যথা
	    ফেরত vim_strchr(s, c);

    যদি (has_mbyte)
    {
	জন্য (p = s; *p != NUL; p += (*mb_ptr2len)(p))
	{
	    পূর্ণসংখ্যা uc = utf_ptr2char(p);
	    যদি (enc_utf8 && (c > 0x80 || uc > 0x80))
	    {
		// Do not match an illegal byte.  E.g. 0xff matches 0xc3 0xbf,
		// not 0xff.
		// compare with lower case of the character
		যদি ((uc < 0x80 || uc != *p) && utf_fold(uc) == lc)
		    ফেরত p;
	    }
	    অন্যথা যদি (*p == c || *p == cc)
		ফেরত p;
	}
    }
    অন্যথা
	// Faster version for when there are no multi-byte characters.
	জন্য (p = s; *p != NUL; ++p)
	    যদি (*p == c || *p == cc)
		ফেরত p;

    ফেরত ফাঁকা;
}

////////////////////////////////////////////////////////////////
//		      regsub stuff			      //
////////////////////////////////////////////////////////////////

প্রকারলেখ শূন্য (*fptr_T)(পূর্ণসংখ্যা *, পূর্ণসংখ্যা);

স্থির পূর্ণসংখ্যা vim_regsub_both(char_u *source, typval_T *রাশিমালা, char_u *dest, পূর্ণসংখ্যা destlen, পূর্ণসংখ্যা flags);

    স্থির শূন্য
do_upper(পূর্ণসংখ্যা *d, পূর্ণসংখ্যা c)
{
    *d = MB_TOUPPER(c);
}

    স্থির শূন্য
do_lower(পূর্ণসংখ্যা *d, পূর্ণসংখ্যা c)
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
regtilde(char_u *source, পূর্ণসংখ্যা magic)
{
    char_u	*newsub = source;
    char_u	*p;
    size_t	newsublen = 0;
    char_u	tilde[3] = {'~', NUL, NUL};
    size_t	tildelen = 1;
    পূর্ণসংখ্যা		error = FALSE;

    যদি (!magic)
    {
	tilde[0] = '\\';
	tilde[1] = '~';
	tilde[2] = NUL;
	tildelen = 2;
    }

    জন্য (p = newsub; *p; ++p)
    {
	যদি (STRNCMP(p, tilde, tildelen) == 0)
	{
	    size_t prefixlen = p - newsub;		// not including the tilde
	    char_u *postfix = p + tildelen;
	    size_t postfixlen;
	    size_t tmpsublen;

	    যদি (newsublen == 0)
		newsublen = STRLEN(newsub);
	    newsublen -= tildelen;
	    postfixlen = newsublen - prefixlen;
	    tmpsublen = prefixlen + reg_পূর্ব_sublen + postfixlen;

	    যদি (tmpsublen > 0 && reg_পূর্ব_sub != ফাঁকা)
	    {
		char_u *tmpsub;

		// Avoid making the text longer than MAXCOL, it will cause
		// trouble at some point.
		যদি (tmpsublen > MAXCOL)
		{
		    emsg(_(e_resulting_text_too_long));
		    error = TRUE;
		    বিরতি;
		}

		tmpsub = alloc(tmpsublen + 1);
		যদি (tmpsub == ফাঁকা)
		{
		    emsg(_(e_out_of_memory));
		    error = TRUE;
		    বিরতি;
		}

		// copy prefix
		mch_memmove(tmpsub, newsub, prefixlen);
		// interpret tilde
		mch_memmove(tmpsub + prefixlen, reg_পূর্ব_sub, reg_পূর্ব_sublen);
		// copy postfix
		STRCPY(tmpsub + prefixlen + reg_পূর্ব_sublen, postfix);

		যদি (newsub != source)	// allocated newsub before
		    vim_free(newsub);
		newsub = tmpsub;
		newsublen = tmpsublen;
		p = newsub + prefixlen + reg_পূর্ব_sublen;
	    }
	    অন্যথা
		mch_memmove(p, postfix, postfixlen + 1);	// remove the tilde (+1 for the NUL)

	    --p;
	}
	অন্যথা
	{
	    যদি (*p == '\\' && p[1])		// skip escaped characters
		++p;
	    যদি (has_mbyte)
		p += (*mb_ptr2len)(p) - 1;
	}
    }

    যদি (error)
    {
	যদি (newsub != source)
	    vim_free(newsub);
	ফেরত source;
    }

    // Store a copy of newsub  in reg_prev_sub.  It is always allocated,
    // because recursive calls may make the returned string invalid.
    // Only store it if there something to store.
    newsublen = p - newsub;
    যদি (newsublen == 0)
	VIM_CLEAR(reg_পূর্ব_sub);
    অন্যথা
    {
	vim_free(reg_পূর্ব_sub);
	reg_পূর্ব_sub = vim_strnsave(newsub, newsublen);
    }

    যদি (reg_পূর্ব_sub == ফাঁকা)
	reg_পূর্ব_sublen = 0;
    অন্যথা
	reg_পূর্ব_sublen = newsublen;

    ফেরত newsub;
}

#ifdef FEAT_EVAL

/*
 * Put the submatches in "argv[argskip]" which is a list passed into
 * call_func() by vim_regsub_both().
 */
    স্থির পূর্ণসংখ্যা
fill_submatch_list(পূর্ণসংখ্যা argc UNUSED, typval_T *argv, পূর্ণসংখ্যা argskip, ufunc_T *fp)
{
    listitem_T	*li;
    পূর্ণসংখ্যা		i;
    char_u	*s;
    typval_T	*listarg = argv + argskip;

    যদি (!has_varargs(fp) && fp->uf_args.ga_len <= argskip)
	// called function doesn't take a submatches argument
	ফেরত argskip;

    // Relies on sl_list to be the first item in staticList10_T.
    init_static_list((staticList10_T *)(listarg->vval.v_list));

    // There are always 10 list items in staticList10_T.
    li = listarg->vval.v_list->lv_first;
    জন্য (i = 0; i < 10; ++i)
    {
	s = rsm.sm_match->startp[i];
	যদি (s == ফাঁকা || rsm.sm_match->endp[i] == ফাঁকা)
	    s = ফাঁকা;
	অন্যথা
	    s = vim_strnsave(s, rsm.sm_match->endp[i] - s);
	li->li_tv.v_type = VAR_STRING;
	li->li_tv.vval.v_string = s;
	li = li->li_পরবর্তী;
    }
    ফেরত argskip + 1;
}

    স্থির শূন্য
clear_submatch_list(staticList10_T *sl)
{
    পূর্ণসংখ্যা i;

    জন্য (i = 0; i < 10; ++i)
	vim_free(sl->sl_বস্তুসমূহ[i].li_tv.vval.v_string);
}
#endif

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
    পূর্ণসংখ্যা
vim_regsub(
    regmatch_T	*rmp,
    char_u	*source,
    typval_T	*রাশিমালা,
    char_u	*dest,
    পূর্ণসংখ্যা		destlen,
    পূর্ণসংখ্যা		flags)
{
    পূর্ণসংখ্যা		ফলাফল;
    regexec_T	rex_save;
    পূর্ণসংখ্যা		rex_in_use_save = rex_in_use;

    যদি (rex_in_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_in_use = TRUE;

    rex.reg_match = rmp;
    rex.reg_mmatch = ফাঁকা;
    rex.reg_maxline = 0;
    rex.reg_buf = curbuf;
    rex.reg_সারি_lbr = TRUE;
    ফলাফল = vim_regsub_both(source, রাশিমালা, dest, destlen, flags);

    rex_in_use = rex_in_use_save;
    যদি (rex_in_use)
	rex = rex_save;

    ফেরত ফলাফল;
}

    পূর্ণসংখ্যা
vim_regsub_multi(
    regmmatch_T	*rmp,
    linenr_T	lnum,
    char_u	*source,
    char_u	*dest,
    পূর্ণসংখ্যা		destlen,
    পূর্ণসংখ্যা		flags)
{
    পূর্ণসংখ্যা		ফলাফল;
    regexec_T	rex_save;
    পূর্ণসংখ্যা		rex_in_use_save = rex_in_use;

    যদি (rex_in_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_in_use = TRUE;

    rex.reg_match = ফাঁকা;
    rex.reg_mmatch = rmp;
    rex.reg_buf = curbuf;	// always works on the current buffer!
    rex.reg_firstlnum = lnum;
    rex.reg_maxline = curbuf->b_ml.ml_সারি_গুনতি - lnum;
    rex.reg_সারি_lbr = FALSE;
    ফলাফল = vim_regsub_both(source, ফাঁকা, dest, destlen, flags);

    rex_in_use = rex_in_use_save;
    যদি (rex_in_use)
	rex = rex_save;

    ফেরত ফলাফল;
}

#যদি defined(FEAT_EVAL)
// When nesting more than a couple levels it's probably a mistake.
# define MAX_REGSUB_NESTING 4
স্থির char_u   *eval_ফলাফল[MAX_REGSUB_NESTING] = {ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা};

# যদি defined(EXITFREE)
    শূন্য
free_resub_eval_ফলাফল(শূন্য)
{
    পূর্ণসংখ্যা i;

    জন্য (i = 0; i < MAX_REGSUB_NESTING; ++i)
	VIM_CLEAR(eval_ফলাফল[i]);
}
# endif
#endif

    স্থির পূর্ণসংখ্যা
vim_regsub_both(
    char_u	*source,
    typval_T	*রাশিমালা,
    char_u	*dest,
    পূর্ণসংখ্যা		destlen,
    পূর্ণসংখ্যা		flags)
{
    char_u	*src;
    char_u	*dst;
    char_u	*s;
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		cc;
    পূর্ণসংখ্যা		no = -1;
    fptr_T	func_all = (fptr_T)ফাঁকা;
    fptr_T	func_one = (fptr_T)ফাঁকা;
    linenr_T	clnum = 0;	// init for GCC
    পূর্ণসংখ্যা		len = 0;	// init for GCC
#ifdef FEAT_EVAL
    স্থির পূর্ণসংখ্যা  nesting = 0;
    পূর্ণসংখ্যা		nested;
#endif
    পূর্ণসংখ্যা		copy = flags & REGSUB_COPY;

    // Be paranoid...
    যদি ((source == ফাঁকা && রাশিমালা == ফাঁকা) || dest == ফাঁকা)
    {
	iemsg(e_null_argument);
	ফেরত 0;
    }
    যদি (prog_magic_wrong())
	ফেরত 0;
#ifdef FEAT_EVAL
    যদি (nesting == MAX_REGSUB_NESTING)
    {
	emsg(_(e_substitute_nesting_too_deep));
	ফেরত 0;
    }
    nested = nesting;
#endif
    src = source;
    dst = dest;

    /*
     * When the substitute part starts with "\=" evaluate it as an expression.
     */
    যদি (রাশিমালা != ফাঁকা || (source[0] == '\\' && source[1] == '='))
    {
#ifdef FEAT_EVAL
	// To make sure that the length doesn't change between checking the
	// length and copying the string, and to speed up things, the
	// resulting string is saved from the call with
	// "flags & REGSUB_COPY" == 0 to the call with
	// "flags & REGSUB_COPY" != 0.
	যদি (copy)
	{
	    যদি (eval_ফলাফল[nested] != ফাঁকা)
	    {
		পূর্ণসংখ্যা eval_len = (পূর্ণসংখ্যা)STRLEN(eval_ফলাফল[nested]);

		যদি (eval_len < destlen)
		{
		    STRCPY(dest, eval_ফলাফল[nested]);
		    dst += eval_len;
		    VIM_CLEAR(eval_ফলাফল[nested]);
		}
	    }
	}
	অন্যথা
	{
	    পূর্ণসংখ্যা		    পূর্ব_can_f_submatch = can_f_submatch;
	    regsubmatch_T   rsm_save;

	    VIM_CLEAR(eval_ফলাফল[nested]);

	    // The expression may contain substitute(), which calls us
	    // recursively.  Make sure submatch() gets the text from the first
	    // level.
	    যদি (can_f_submatch)
		rsm_save = rsm;
	    can_f_submatch = TRUE;
	    rsm.sm_match = rex.reg_match;
	    rsm.sm_mmatch = rex.reg_mmatch;
	    rsm.sm_firstlnum = rex.reg_firstlnum;
	    rsm.sm_maxline = rex.reg_maxline;
	    rsm.sm_সারি_lbr = rex.reg_সারি_lbr;

	    // Although unlikely, it is possible that the expression invokes a
	    // substitute command (it might fail, but still).  Therefore keep
	    // an array of eval results.
	    ++nesting;

	    যদি (রাশিমালা != ফাঁকা)
	    {
		typval_T	argv[2];
		char_u		buf[NUMBUFLEN];
		typval_T	rettv;
		staticList10_T	matchList;
		funcexe_T	funcexe;

		rettv.v_type = VAR_STRING;
		rettv.vval.v_string = ফাঁকা;
		argv[0].v_type = VAR_LIST;
		argv[0].vval.v_list = &matchList.sl_list;
		matchList.sl_list.lv_len = 0;
		CLEAR_FIELD(funcexe);
		funcexe.fe_argv_func = fill_submatch_list;
		funcexe.fe_evaluate = TRUE;
		যদি (রাশিমালা->v_type == VAR_FUNC)
		{
		    s = রাশিমালা->vval.v_string;
		    call_func(s, -1, &rettv, 1, argv, &funcexe);
		}
		অন্যথা যদি (রাশিমালা->v_type == VAR_PARTIAL)
		{
		    partial_T   *partial = রাশিমালা->vval.v_partial;

		    s = partial_name(partial);
		    funcexe.fe_partial = partial;
		    call_func(s, -1, &rettv, 1, argv, &funcexe);
		}
		অন্যথা যদি (রাশিমালা->v_type == VAR_INSTR)
		{
		    exe_typval_instr(রাশিমালা, &rettv);
		}
		যদি (matchList.sl_list.lv_len > 0)
		    // fill_submatch_list() was called
		    clear_submatch_list(&matchList);

		যদি (rettv.v_type == VAR_UNKNOWN)
		    // something failed, no need to report another error
		    eval_ফলাফল[nested] = ফাঁকা;
		অন্যথা
		{
		    eval_ফলাফল[nested] = tv_get_string_buf_chk(&rettv, buf);
		    যদি (eval_ফলাফল[nested] != ফাঁকা)
			eval_ফলাফল[nested] = vim_strsave(eval_ফলাফল[nested]);
		}
		clear_tv(&rettv);
	    }
	    অন্যথা যদি (substitute_instr != ফাঁকা)
		// Execute instructions from ISN_SUBSTITUTE.
		eval_ফলাফল[nested] = exe_substitute_instr();
	    অন্যথা
		eval_ফলাফল[nested] = eval_to_string(source + 2, TRUE, FALSE);
	    --nesting;

	    যদি (eval_ফলাফল[nested] != ফাঁকা)
	    {
		পূর্ণসংখ্যা had_backslash = FALSE;

		জন্য (s = eval_ফলাফল[nested]; *s != NUL; MB_PTR_ADV(s))
		{
		    // Change NL to CR, so that it becomes a line break,
		    // unless called from vim_regexec_nl().
		    // Skip over a backslashed character.
		    যদি (*s == NL && !rsm.sm_সারি_lbr)
			*s = CAR;
		    অন্যথা যদি (*s == '\\' && s[1] != NUL)
		    {
			++s;
			/* Change NL to CR here too, so that this works:
			 * :s/abc\\\ndef/\="aaa\\\nbbb"/  on text:
			 *   abc\
			 *   def
			 * Not when called from vim_regexec_nl().
			 */
			যদি (*s == NL && !rsm.sm_সারি_lbr)
			    *s = CAR;
			had_backslash = TRUE;
		    }
		}
		যদি (had_backslash && (flags & REGSUB_BACKSLASH))
		{
		    // Backslashes will be consumed, need to double them.
		    s = vim_strsave_escaped(eval_ফলাফল[nested], (char_u *)"\\");
		    যদি (s != ফাঁকা)
		    {
			vim_free(eval_ফলাফল[nested]);
			eval_ফলাফল[nested] = s;
		    }
		}

		dst += STRLEN(eval_ফলাফল[nested]);
	    }

	    can_f_submatch = পূর্ব_can_f_submatch;
	    যদি (can_f_submatch)
		rsm = rsm_save;
	}
#endif
    }
    অন্যথা
      যখন ((c = *src++) != NUL)
      {
	যদি (c == '&' && (flags & REGSUB_MAGIC))
	    no = 0;
	অন্যথা যদি (c == '\\' && *src != NUL)
	{
	    যদি (*src == '&' && !(flags & REGSUB_MAGIC))
	    {
		++src;
		no = 0;
	    }
	    অন্যথা যদি ('0' <= *src && *src <= '9')
	    {
		no = *src++ - '0';
	    }
	    অন্যথা যদি (vim_strchr((char_u *)"uUlLeE", *src))
	    {
		নির্বাচন (*src++)
		{
		ক্ষেত্র 'u':   func_one = do_upper;
			    চালু;
		ক্ষেত্র 'U':   func_all = do_upper;
			    চালু;
		ক্ষেত্র 'l':   func_one = do_lower;
			    চালু;
		ক্ষেত্র 'L':   func_all = do_lower;
			    চালু;
		ক্ষেত্র 'e':
		ক্ষেত্র 'E':   func_one = func_all = (fptr_T)ফাঁকা;
			    চালু;
		}
	    }
	}
	যদি (no < 0)	      // Ordinary character.
	{
	    যদি (c == K_SPECIAL && src[0] != NUL && src[1] != NUL)
	    {
		// Copy a special key as-is.
		যদি (copy)
		{
		    যদি (dst + 3 > dest + destlen)
		    {
			iemsg("vim_regsub_both(): not enough space");
			ফেরত 0;
		    }
		    *dst++ = c;
		    *dst++ = *src++;
		    *dst++ = *src++;
		}
		অন্যথা
		{
		    dst += 3;
		    src += 2;
		}
		চালু;
	    }

	    যদি (c == '\\' && *src != NUL)
	    {
		// Check for abbreviations -- webb
		নির্বাচন (*src)
		{
		    ক্ষেত্র 'r':	c = CAR;	++src;	বিরতি;
		    ক্ষেত্র 'n':	c = NL;		++src;	বিরতি;
		    ক্ষেত্র 't':	c = TAB;	++src;	বিরতি;
		 // Oh no!  \e already has meaning in subst pat :-(
		 // case 'e':   c = ESC;	++src;	break;
		    ক্ষেত্র 'b':	c = Ctrl_H;	++src;	বিরতি;

		    // If "backslash" is TRUE the backslash will be removed
		    // later.  Used to insert a literal CR.
		    পূর্বনির্ধারিত:	যদি (flags & REGSUB_BACKSLASH)
				{
				    যদি (copy)
				    {
					যদি (dst + 1 > dest + destlen)
					{
					    iemsg("vim_regsub_both(): not enough space");
					    ফেরত 0;
					}
					*dst = '\\';
				    }
				    ++dst;
				}
				c = *src++;
		}
	    }
	    অন্যথা যদি (has_mbyte)
		c = mb_ptr2char(src - 1);

	    // Write to buffer, if copy is set.
	    যদি (func_one != (fptr_T)ফাঁকা)
	    {
		func_one(&cc, c);
		func_one = ফাঁকা;
	    }
	    অন্যথা যদি (func_all != (fptr_T)ফাঁকা)
		func_all(&cc, c);
	    অন্যথা // just copy
		cc = c;

	    যদি (has_mbyte)
	    {
		পূর্ণসংখ্যা totlen = mb_ptr2len(src - 1);
		পূর্ণসংখ্যা charlen = mb_char2len(cc);

		যদি (copy)
		{
		    যদি (dst + charlen > dest + destlen)
		    {
			iemsg("vim_regsub_both(): not enough space");
			ফেরত 0;
		    }
		    mb_char2bytes(cc, dst);
		}
		dst += charlen - 1;
		যদি (enc_utf8)
		{
		    পূর্ণসংখ্যা clen = utf_ptr2len(src - 1);

		    // If the character length is shorter than "totlen", there
		    // are composing characters; copy them as-is.
		    যদি (clen < totlen)
		    {
			যদি (copy)
			{
			    যদি (dst + totlen - clen > dest + destlen)
			    {
				iemsg("vim_regsub_both(): not enough space");
				ফেরত 0;
			    }
			    mch_memmove(dst + 1, src - 1 + clen,
						     (size_t)(totlen - clen));
			}
			dst += totlen - clen;
		    }
		}
		src += totlen - 1;
	    }
	    অন্যথা যদি (copy)
	    {
		যদি (dst + 1 > dest + destlen)
		{
		    iemsg("vim_regsub_both(): not enough space");
		    ফেরত 0;
		}
		*dst = cc;
	    }
	    dst++;
	}
	অন্যথা
	{
	    যদি (REG_MULTI)
	    {
		clnum = rex.reg_mmatch->startpos[no].lnum;
		যদি (clnum < 0 || rex.reg_mmatch->endpos[no].lnum < 0)
		    s = ফাঁকা;
		অন্যথা
		{
		    s = reg_getline(clnum) + rex.reg_mmatch->startpos[no].col;
		    যদি (rex.reg_mmatch->endpos[no].lnum == clnum)
			len = rex.reg_mmatch->endpos[no].col
					    - rex.reg_mmatch->startpos[no].col;
		    অন্যথা
			len = (পূর্ণসংখ্যা)reg_getline_len(clnum) - rex.reg_mmatch->startpos[no].col;
		}
	    }
	    অন্যথা
	    {
		s = rex.reg_match->startp[no];
		যদি (rex.reg_match->endp[no] == ফাঁকা)
		    s = ফাঁকা;
		অন্যথা
		    len = (পূর্ণসংখ্যা)(rex.reg_match->endp[no] - s);
	    }
	    যদি (s != ফাঁকা)
	    {
		জন্য (;;)
		{
		    যদি (len == 0)
		    {
			যদি (REG_MULTI)
			{
			    যদি (rex.reg_mmatch->endpos[no].lnum == clnum)
				বিরতি;
			    যদি (copy)
			    {
				যদি (dst + 1 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    ফেরত 0;
				}
				*dst = CAR;
			    }
			    ++dst;
			    s = reg_getline(++clnum);
			    যদি (rex.reg_mmatch->endpos[no].lnum == clnum)
				len = rex.reg_mmatch->endpos[no].col;
			    অন্যথা
				len = (পূর্ণসংখ্যা)reg_getline_len(clnum);
			}
			অন্যথা
			    বিরতি;
		    }
		    অন্যথা যদি (*s == NUL) // we hit NUL.
		    {
			যদি (copy)
			    iemsg(e_damaged_match_string);
			যাও exit;
		    }
		    অন্যথা
		    {
			যদি ((flags & REGSUB_BACKSLASH)
						  && (*s == CAR || *s == '\\'))
			{
			    /*
			     * Insert a backslash in front of a CR, otherwise
			     * it will be replaced by a line break.
			     * Number of backslashes will be halved later,
			     * double them here.
			     */
			    যদি (copy)
			    {
				যদি (dst + 2 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    ফেরত 0;
				}
				dst[0] = '\\';
				dst[1] = *s;
			    }
			    dst += 2;
			}
			অন্যথা
			{
			    যদি (has_mbyte)
				c = mb_ptr2char(s);
			    অন্যথা
				c = *s;

			    যদি (func_one != (fptr_T)ফাঁকা)
			    {
				func_one(&cc, c);
				func_one = ফাঁকা;
			    }
			    অন্যথা যদি (func_all != (fptr_T)ফাঁকা)
				func_all(&cc, c);
			    অন্যথা // just copy
				cc = c;

			    যদি (has_mbyte)
			    {
				পূর্ণসংখ্যা l;
				পূর্ণসংখ্যা charlen;

				// Copy composing characters separately, one
				// at a time.
				যদি (enc_utf8)
				    l = utf_ptr2len(s) - 1;
				অন্যথা
				    l = mb_ptr2len(s) - 1;

				s += l;
				len -= l;
				charlen = mb_char2len(cc);
				যদি (copy)
				{
				    যদি (dst + charlen > dest + destlen)
				    {
					iemsg("vim_regsub_both(): not enough space");
					ফেরত 0;
				    }
				    mb_char2bytes(cc, dst);
				}
				dst += charlen - 1;
			    }
			    অন্যথা যদি (copy)
			    {
				যদি (dst + 1 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    ফেরত 0;
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
    যদি (copy)
	*dst = NUL;

exit:
    ফেরত (পূর্ণসংখ্যা)((dst - dest) + 1);
}

#ifdef FEAT_EVAL

    স্থির char_u *
reg_getline_submatch(linenr_T lnum)
{
    char_u *সারি;

    reg_getline_common(lnum, RGLF_সারি | RGLF_SUBMATCH, &সারি, ফাঁকা);

    ফেরত সারি;
}

    স্থির colnr_T
reg_getline_submatch_len(linenr_T lnum)
{
    colnr_T দৈর্ঘ্য;

    reg_getline_common(lnum, RGLF_দৈর্ঘ্য | RGLF_SUBMATCH, ফাঁকা, &দৈর্ঘ্য);

    ফেরত দৈর্ঘ্য;
}

/*
 * Used for the submatch() function: get the string from the n'th submatch in
 * allocated memory.
 * Returns NULL when not in a ":s" command and for a non-existing submatch.
 */
    char_u *
reg_submatch(পূর্ণসংখ্যা no)
{
    char_u	*retval = ফাঁকা;
    char_u	*s;
    পূর্ণসংখ্যা		len;
    পূর্ণসংখ্যা		round;
    linenr_T	lnum;

    যদি (!can_f_submatch || no < 0)
	ফেরত ফাঁকা;

    যদি (rsm.sm_match == ফাঁকা)
    {
	/*
	 * First round: compute the length and allocate memory.
	 * Second round: copy the text.
	 */
	জন্য (round = 1; round <= 2; ++round)
	{
	    lnum = rsm.sm_mmatch->startpos[no].lnum;
	    যদি (lnum < 0 || rsm.sm_mmatch->endpos[no].lnum < 0)
		ফেরত ফাঁকা;

	    s = reg_getline_submatch(lnum);
	    যদি (s == ফাঁকা)  // anti-crash check, cannot happen?
		বিরতি;
	    s += rsm.sm_mmatch->startpos[no].col;
	    যদি (rsm.sm_mmatch->endpos[no].lnum == lnum)
	    {
		// Within one line: take form start to end col.
		len = rsm.sm_mmatch->endpos[no].col
					  - rsm.sm_mmatch->startpos[no].col;
		যদি (round == 2)
		    vim_strncpy(retval, s, len);
		++len;
	    }
	    অন্যথা
	    {
		// Multiple lines: take start line from start col, middle
		// lines completely and end line up to end col.
		len = (পূর্ণসংখ্যা)reg_getline_submatch_len(lnum) - rsm.sm_mmatch->startpos[no].col;
		যদি (round == 2)
		{
		    STRCPY(retval, s);
		    retval[len] = '\n';
		}
		++len;
		++lnum;
		যখন (lnum < rsm.sm_mmatch->endpos[no].lnum)
		{
		    s = reg_getline_submatch(lnum);
		    যদি (round == 2)
			STRCPY(retval + len, s);
		    len += (পূর্ণসংখ্যা)reg_getline_submatch_len(lnum);
		    যদি (round == 2)
			retval[len] = '\n';
		    ++len;
		    ++lnum;
		}
		যদি (round == 2)
		    STRNCPY(retval + len, reg_getline_submatch(lnum),
					     rsm.sm_mmatch->endpos[no].col);
		len += rsm.sm_mmatch->endpos[no].col;
		যদি (round == 2)
		    retval[len] = NUL;
		++len;
	    }

	    যদি (retval == ফাঁকা)
	    {
		retval = alloc(len);
		যদি (retval == ফাঁকা)
		    ফেরত ফাঁকা;
	    }
	}
    }
    অন্যথা
    {
	s = rsm.sm_match->startp[no];
	যদি (s == ফাঁকা || rsm.sm_match->endp[no] == ফাঁকা)
	    retval = ফাঁকা;
	অন্যথা
	    retval = vim_strnsave(s, rsm.sm_match->endp[no] - s);
    }

    ফেরত retval;
}

/*
 * Used for the submatch() function with the optional non-zero argument: get
 * the list of strings from the n'th submatch in allocated memory with NULs
 * represented in NLs.
 * Returns a list of allocated strings.  Returns NULL when not in a ":s"
 * command, for a non-existing submatch and for any error.
 */
    list_T *
reg_submatch_list(পূর্ণসংখ্যা no)
{
    char_u	*s;
    linenr_T	slnum;
    linenr_T	elnum;
    colnr_T	scol;
    colnr_T	ecol;
    পূর্ণসংখ্যা		i;
    list_T	*list;
    পূর্ণসংখ্যা		error = FALSE;

    যদি (!can_f_submatch || no < 0)
	ফেরত ফাঁকা;

    যদি (rsm.sm_match == ফাঁকা)
    {
	slnum = rsm.sm_mmatch->startpos[no].lnum;
	elnum = rsm.sm_mmatch->endpos[no].lnum;
	যদি (slnum < 0 || elnum < 0)
	    ফেরত ফাঁকা;

	scol = rsm.sm_mmatch->startpos[no].col;
	ecol = rsm.sm_mmatch->endpos[no].col;

	list = list_alloc();
	যদি (list == ফাঁকা)
	    ফেরত ফাঁকা;

	s = reg_getline_submatch(slnum) + scol;
	যদি (slnum == elnum)
	{
	    যদি (list_append_string(list, s, ecol - scol) == FAIL)
		error = TRUE;
	}
	অন্যথা
	{
	    পূর্ণসংখ্যা max_lnum = elnum - slnum;

	    যদি (list_append_string(list, s, -1) == FAIL)
		error = TRUE;
	    জন্য (i = 1; i < max_lnum; i++)
	    {
		s = reg_getline_submatch(slnum + i);
		যদি (list_append_string(list, s, -1) == FAIL)
		    error = TRUE;
	    }
	    s = reg_getline_submatch(elnum);
	    যদি (list_append_string(list, s, ecol) == FAIL)
		error = TRUE;
	}
    }
    অন্যথা
    {
	s = rsm.sm_match->startp[no];
	যদি (s == ফাঁকা || rsm.sm_match->endp[no] == ফাঁকা)
	    ফেরত ফাঁকা;
	list = list_alloc();
	যদি (list == ফাঁকা)
	    ফেরত ফাঁকা;
	যদি (list_append_string(list, s,
				 (পূর্ণসংখ্যা)(rsm.sm_match->endp[no] - s)) == FAIL)
	    error = TRUE;
    }

    যদি (error)
    {
	list_free(list);
	ফেরত ফাঁকা;
    }
    ++list->lv_refcount;
    ফেরত list;
}
#endif

/*
 * Initialize the values used for matching against multiple lines
 */
    স্থির শূন্য
init_regexec_multi(
	regmmatch_T	*rmp,
	win_T		*win,	// window in which to search or NULL
	buf_T		*buf,	// buffer in which to search
	linenr_T	lnum)	// nr of line to start looking for match
{
    rex.reg_match = ফাঁকা;
    rex.reg_mmatch = rmp;
    rex.reg_buf = buf;
    rex.reg_win = win;
    rex.reg_firstlnum = lnum;
    rex.reg_maxline = rex.reg_buf->b_ml.ml_সারি_গুনতি - lnum;
    rex.reg_সারি_lbr = FALSE;
    rex.reg_ic = rmp->rmm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = rmp->rmm_maxcol;
}

#define CLASS_not		0x80
#define CLASS_af		0x40
#define CLASS_AF		0x20
#define CLASS_az		0x10
#define CLASS_AZ		0x08
#define CLASS_o7		0x04
#define CLASS_o9		0x02
#define CLASS_underscore	0x01

/*
 * Parse the "[...]" collection between "start" (the first character after the
 * "[") and "end" (the "]") into a combination of CLASS_ bits, so that it can
 * be turned into a faster character class like \d or \x.  Sets "*newl" to TRUE
 * when the collection also matches a newline.  Returns -1 when the collection
 * is not a plain combination of the recognized ranges.
 */
    স্থির পূর্ণসংখ্যা
get_char_class_bits(char_u *start, char_u *end, পূর্ণসংখ্যা *newl)
{
    char_u	*p = start;
    পূর্ণসংখ্যা		config = 0;

    *newl = FALSE;
    যদি (*end != ']')
	ফেরত -1;
    যদি (*p == '^')
    {
	config |= CLASS_not;
	p++;
    }

    যখন (p < end)
    {
	যদি (p + 2 < end && *(p + 1) == '-')
	{
	    নির্বাচন (*p)
	    {
		ক্ষেত্র '0':
		    যদি (*(p + 2) == '9')
		    {
			config |= CLASS_o9;
			বিরতি;
		    }
		    যদি (*(p + 2) == '7')
		    {
			config |= CLASS_o7;
			বিরতি;
		    }
		    ফেরত -1;

		ক্ষেত্র 'a':
		    যদি (*(p + 2) == 'z')
		    {
			config |= CLASS_az;
			বিরতি;
		    }
		    যদি (*(p + 2) == 'f')
		    {
			config |= CLASS_af;
			বিরতি;
		    }
		    ফেরত -1;

		ক্ষেত্র 'A':
		    যদি (*(p + 2) == 'Z')
		    {
			config |= CLASS_AZ;
			বিরতি;
		    }
		    যদি (*(p + 2) == 'F')
		    {
			config |= CLASS_AF;
			বিরতি;
		    }
		    ফেরত -1;

		পূর্বনির্ধারিত:
		    ফেরত -1;
	    }
	    p += 3;
	}
	অন্যথা যদি (p + 1 < end && *p == '\\' && *(p + 1) == 'n')
	{
	    *newl = TRUE;
	    p += 2;
	}
	অন্যথা যদি (*p == '_')
	{
	    config |= CLASS_underscore;
	    p++;
	}
	অন্যথা যদি (*p == '\n')
	{
	    *newl = TRUE;
	    p++;
	}
	অন্যথা
	    ফেরত -1;
    } // while (p < end)

    যদি (p != end)
	ফেরত -1;

    // U+017F and U+212A fold to ASCII 's' and 'k', missed by the class opcodes
    যদি (enc_utf8 && (config & (CLASS_az | CLASS_AZ)))
	ফেরত -1;

    ফেরত config;
}

#অন্তর্ভাব "regexp_bt.c"

স্থির regengine_T bt_regengine =
{
    bt_regcomp,
    bt_regfree,
    bt_regexec_nl,
    bt_regexec_multi
#ifdef DEBUG
    ,(char_u *)""
#endif
};

#অন্তর্ভাব "regexp_nfa.c"

স্থির regengine_T nfa_regengine =
{
    nfa_regcomp,
    nfa_regfree,
    nfa_regexec_nl,
    nfa_regexec_multi
#ifdef DEBUG
    ,(char_u *)""
#endif
};

// Which regexp engine to use? Needed for vim_regcomp().
// Must match with 'regexpengine'.
স্থির পূর্ণসংখ্যা regexp_engine = 0;

#ifdef DEBUG
স্থির ধ্রুবক char_u regname[][30] = {
		    "AUTOMATIC Regexp Engine",
		    "BACKTRACKING Regexp Engine",
		    "NFA Regexp Engine"
			    };
#endif

/*
 * Compile a regular expression into internal code.
 * Returns the program in allocated memory.
 * Use vim_regfree() to free the memory.
 * Returns NULL for an error.
 */
    regprog_T *
vim_regcomp(char_u *রাশিমালা_arg, পূর্ণসংখ্যা re_flags)
{
    regprog_T   *prog = ফাঁকা;
    char_u	*রাশিমালা = রাশিমালা_arg;
    পূর্ণসংখ্যা		called_emsg_before;

    regexp_engine = p_re;

    // Check for prefix "\%#=", that sets the regexp engine
    যদি (STRNCMP(রাশিমালা, "\\%#=", 4) == 0)
    {
	পূর্ণসংখ্যা newengine = রাশিমালা[4] - '0';

	যদি (newengine == AUTOMATIC_ENGINE
	    || newengine == BACKTRACKING_ENGINE
	    || newengine == NFA_ENGINE)
	{
	    regexp_engine = রাশিমালা[4] - '0';
	    রাশিমালা += 5;
#ifdef DEBUG
	    smsg("New regexp mode selected (%d): %s",
					   regexp_engine, regname[newengine]);
#endif
	}
	অন্যথা
	{
	    emsg(_(e_percent_hash_can_only_be_followed_by_zero_one_two_automatic_engine_will_be_used));
	    regexp_engine = AUTOMATIC_ENGINE;
	}
    }
#ifdef DEBUG
    bt_regengine.রাশিমালা = রাশিমালা;
    nfa_regengine.রাশিমালা = রাশিমালা;
#endif
    // reg_iswordc() uses rex.reg_buf
    rex.reg_buf = curbuf;

    /*
     * First try the NFA engine, unless backtracking was requested.
     */
    called_emsg_before = called_emsg;
    যদি (regexp_engine != BACKTRACKING_ENGINE)
	prog = nfa_regengine.regcomp(রাশিমালা,
		re_flags + (regexp_engine == AUTOMATIC_ENGINE ? RE_AUTO : 0));
    অন্যথা
	prog = bt_regengine.regcomp(রাশিমালা, re_flags);

    // Check for error compiling regexp with initial engine.
    যদি (prog == ফাঁকা)
    {
#ifdef BT_REGEXP_DEBUG_LOG
	যদি (regexp_engine == BACKTRACKING_ENGINE)   // debugging log for BT engine
	{
	    FILE *f;
	    f = fopen(BT_REGEXP_DEBUG_LOG_NAME, "a");
	    যদি (f)
	    {
		fprintf(f, "Syntax error in \"%s\"\n", রাশিমালা);
		fclose(f);
	    }
	    অন্যথা
		semsg("(NFA) Could not open \"%s\" to write !!!",
			BT_REGEXP_DEBUG_LOG_NAME);
	}
#endif
	/*
	 * If the NFA engine failed, try the backtracking engine.
	 * The NFA engine also fails for patterns that it can't handle well
	 * but are still valid patterns, thus a retry should work.
	 * But don't try if an error message was given.
	 */
	যদি (regexp_engine == AUTOMATIC_ENGINE
					  && called_emsg == called_emsg_before)
	{
	    regexp_engine = BACKTRACKING_ENGINE;
#ifdef FEAT_EVAL
	    report_re_switch(রাশিমালা);
#endif
	    prog = bt_regengine.regcomp(রাশিমালা, re_flags);
	}
    }

    যদি (prog != ফাঁকা)
    {
	// Store the info needed to call regcomp() again when the engine turns
	// out to be very slow when executing it.
	prog->re_engine = regexp_engine;
	prog->re_flags  = re_flags;
    }

    ফেরত prog;
}

/*
 * Free a compiled regexp program, returned by vim_regcomp().
 */
    শূন্য
vim_regfree(regprog_T *prog)
{
    যদি (prog != ফাঁকা)
	prog->engine->regfree(prog);
}

#যদি defined(EXITFREE)
    শূন্য
free_regexp_stuff(শূন্য)
{
    ga_clear(&regstack);
    ga_clear(&backpos);
    vim_free(reg_tofree);
    vim_free(reg_পূর্ব_sub);
    vim_free(post_start);   // NFA postfix buffer, reused across compilations
    post_start = ফাঁকা;
    post_start_len = 0;
    vim_free(nfa_স্তূপ);    // NFA fragment stack, reused across compilations
    nfa_স্তূপ = ফাঁকা;
    nfa_স্তূপ_len = 0;
}
#endif

#ifdef FEAT_EVAL
    স্থির শূন্য
report_re_switch(char_u *pat)
{
    যদি (p_verbose > 0)
    {
	verbose_enter();
	বার্তা_puts(_("Switching to backtracking RE engine for pattern: "));
	বার্তা_puts((অক্ষর *)pat);
	verbose_leave();
    }
}
#endif

#যদি defined(FEAT_X11)
/*
 * Return whether "prog" is currently being executed.
 */
    পূর্ণসংখ্যা
regprog_in_use(regprog_T *prog)
{
    ফেরত prog->re_in_use;
}
#endif

/*
 * Match a regexp against a string.
 * "rmp->regprog" must be a compiled regexp as returned by vim_regcomp().
 * Note: "rmp->regprog" may be freed and changed.
 * Uses curbuf for line count and 'iskeyword'.
 * When "nl" is TRUE consider a "\n" in "line" to be a line break.
 *
 * Return TRUE if there is a match, FALSE if not.
 */
    স্থির পূর্ণসংখ্যা
vim_regexec_string(
    regmatch_T	*rmp,
    char_u	*সারি,  // string to match against
    colnr_T	col,    // column to start looking for match
    পূর্ণসংখ্যা		nl)
{
    পূর্ণসংখ্যা		ফলাফল;
    regexec_T	rex_save;
    পূর্ণসংখ্যা		rex_in_use_save = rex_in_use;

    // Cannot use the same prog recursively, it contains state.
    যদি (rmp->regprog->re_in_use)
    {
	emsg(_(e_cannot_use_pattern_recursively));
	ফেরত FALSE;
    }
    rmp->regprog->re_in_use = TRUE;

    যদি (rex_in_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_in_use = TRUE;

    rex.reg_startp = ফাঁকা;
    rex.reg_endp = ফাঁকা;
    rex.reg_startpos = ফাঁকা;
    rex.reg_endpos = ফাঁকা;

    ফলাফল = rmp->regprog->engine->regexec_nl(rmp, সারি, col, nl);
    rmp->regprog->re_in_use = FALSE;

    // NFA engine aborted because it's very slow.
    যদি (rmp->regprog->re_engine == AUTOMATIC_ENGINE
					       && ফলাফল == NFA_TOO_EXPENSIVE)
    {
	পূর্ণসংখ্যা    save_p_re = p_re;
	পূর্ণসংখ্যা    re_flags = rmp->regprog->re_flags;
	char_u *pat = vim_strsave(((nfa_regprog_T *)rmp->regprog)->pattern);

	p_re = BACKTRACKING_ENGINE;
	যদি (pat != ফাঁকা)
	{
	    regprog_T *পূর্ব_prog = rmp->regprog;

#ifdef FEAT_EVAL
	    report_re_switch(pat);
#endif
	    rmp->regprog = vim_regcomp(pat, re_flags);
	    যদি (rmp->regprog == ফাঁকা)
	    {
		// Somehow compiling the pattern failed now, put back the
		// previous one to avoid "regprog" becoming NULL.
		rmp->regprog = পূর্ব_prog;
	    }
	    অন্যথা
	    {
		vim_regfree(পূর্ব_prog);
		rmp->regprog->re_in_use = TRUE;
		ফলাফল = rmp->regprog->engine->regexec_nl(rmp, সারি, col, nl);
		rmp->regprog->re_in_use = FALSE;
	    }
	    vim_free(pat);
	}

	p_re = save_p_re;
    }

    rex_in_use = rex_in_use_save;
    যদি (rex_in_use)
	rex = rex_save;

    ফেরত ফলাফল > 0;
}

#যদি defined(FEAT_SPELL) || defined(FEAT_EVAL) || defined(FEAT_X11)
/*
 * Note: "*prog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    পূর্ণসংখ্যা
vim_regexec_prog(
    regprog_T	**prog,
    পূর্ণসংখ্যা		ignore_case,
    char_u	*সারি,
    colnr_T	col)
{
    পূর্ণসংখ্যা		r;
    regmatch_T	regmatch;

    regmatch.regprog = *prog;
    regmatch.rm_ic = ignore_case;
    r = vim_regexec_string(&regmatch, সারি, col, FALSE);
    *prog = regmatch.regprog;
    ফেরত r;
}
#endif

/*
 * Note: "rmp->regprog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    পূর্ণসংখ্যা
vim_regexec(regmatch_T *rmp, char_u *সারি, colnr_T col)
{
    ফেরত vim_regexec_string(rmp, সারি, col, FALSE);
}

/*
 * Like vim_regexec(), but consider a "\n" in "line" to be a line break.
 * Note: "rmp->regprog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    পূর্ণসংখ্যা
vim_regexec_nl(regmatch_T *rmp, char_u *সারি, colnr_T col)
{
    ফেরত vim_regexec_string(rmp, সারি, col, TRUE);
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
    long
vim_regexec_multi(
    regmmatch_T *rmp,
    win_T       *win,		// window in which to search or NULL
    buf_T       *buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	col,		// column to start looking for match
    পূর্ণসংখ্যা		*timed_out)	// flag is set when timeout limit reached
{
    পূর্ণসংখ্যা		ফলাফল;
    regexec_T	rex_save;
    পূর্ণসংখ্যা		rex_in_use_save = rex_in_use;

    // Cannot use the same prog recursively, it contains state.
    যদি (rmp->regprog->re_in_use)
    {
	emsg(_(e_cannot_use_pattern_recursively));
	ফেরত FALSE;
    }
    rmp->regprog->re_in_use = TRUE;

    যদি (rex_in_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_in_use = TRUE;

    ফলাফল = rmp->regprog->engine->regexec_multi(
				      rmp, win, buf, lnum, col, timed_out);
    rmp->regprog->re_in_use = FALSE;

    // NFA engine aborted because it's very slow.
    যদি (rmp->regprog->re_engine == AUTOMATIC_ENGINE
					       && ফলাফল == NFA_TOO_EXPENSIVE)
    {
	পূর্ণসংখ্যা    save_p_re = p_re;
	পূর্ণসংখ্যা    re_flags = rmp->regprog->re_flags;
	char_u *pat = vim_strsave(((nfa_regprog_T *)rmp->regprog)->pattern);

	p_re = BACKTRACKING_ENGINE;
	যদি (pat != ফাঁকা)
	{
	    regprog_T *পূর্ব_prog = rmp->regprog;

#ifdef FEAT_EVAL
	    report_re_switch(pat);
#endif
#ifdef FEAT_SYN_HL
	    // checking for \z misuse was already done when compiling for NFA,
	    // allow all here
	    reg_do_extmatch = REX_ALL;
#endif
	    rmp->regprog = vim_regcomp(pat, re_flags);
#ifdef FEAT_SYN_HL
	    reg_do_extmatch = 0;
#endif
	    যদি (rmp->regprog == ফাঁকা)
	    {
		// Somehow compiling the pattern failed now, put back the
		// previous one to avoid "regprog" becoming NULL.
		rmp->regprog = পূর্ব_prog;
	    }
	    অন্যথা
	    {
		vim_regfree(পূর্ব_prog);

		rmp->regprog->re_in_use = TRUE;
		ফলাফল = rmp->regprog->engine->regexec_multi(
				      rmp, win, buf, lnum, col, timed_out);
		rmp->regprog->re_in_use = FALSE;
	    }
	    vim_free(pat);
	}
	p_re = save_p_re;
    }

    rex_in_use = rex_in_use_save;
    যদি (rex_in_use)
	rex = rex_save;

    ফেরত ফলাফল <= 0 ? 0 : ফলাফল;
}
