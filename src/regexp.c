/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * Handling of regular expressions: vim_regcomp(), vim_regexec(), vim_regsub()
 */

// By default: do not create debugging logs or files related to regular
// expressions, even when compiling with -DDEBUG.
// Uncomment the second line to get the regexp debugging.
#undef DEBUG
// #define DEBUG

#অন্তর্ভাৱ "vim.h"

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
স্থির চঞ্চল sig_atomic_t *timeout_flag = &dummy_timeout_flag;
#endif

/*
 * Magic characters have a special meaning, they don't match literally.
 * Magic characters are negative.  This separates them from literal characters
 * (possibly multi-byte).  Only ASCII characters can be Magic.
 */
#define Magic(x)	((পূর্ণাঙ্ক)(x) - 256)
#define un_Magic(x)	((x) + 256)
#define is_Magic(x)	((x) < 0)

    স্থির পূর্ণাঙ্ক
no_Magic(পূর্ণাঙ্ক x)
{
    যদি (is_Magic(x))
	ঘূৰাই un_Magic(x);
    ঘূৰাই x;
}

    স্থির পূর্ণাঙ্ক
toggle_Magic(পূর্ণাঙ্ক x)
{
    যদি (is_Magic(x))
	ঘূৰাই un_Magic(x);
    ঘূৰাই Magic(x);
}

#যদি defined(FEAT_RELTIME)
স্থির পূর্ণাঙ্ক timeout_nesting = 0;

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
#define UCHARAT(p)	((পূর্ণাঙ্ক)*(char_u *)(p))

// Used for an error (down from) vim_regcomp(): give the error message, set
// rc_did_emsg and return NULL
#define EMSG_RET_NULL(m) ঘূৰাই (emsg((m)), rc_did_emsg = TRUE, (শূন্য *)রিক্ত)
#define IEMSG_RET_NULL(m) ঘূৰাই (iemsg((m)), rc_did_emsg = TRUE, (শূন্য *)রিক্ত)
#define EMSG_RET_FAIL(m) ঘূৰাই (emsg((m)), rc_did_emsg = TRUE, FAIL)
#define EMSG2_RET_NULL(m, c) ঘূৰাই (semsg((ধ্রুৱ ৱর্ণ *)(m), (c) ? "" : "\\"), rc_did_emsg = TRUE, (শূন্য *)রিক্ত)
#define EMSG3_RET_NULL(m, c, a) ঘূৰাই (semsg((ধ্রুৱ ৱর্ণ *)(m), (c) ? "" : "\\", (a)), rc_did_emsg = TRUE, (শূন্য *)রিক্ত)
#define EMSG2_RET_FAIL(m, c) ঘূৰাই (semsg((ধ্রুৱ ৱর্ণ *)(m), (c) ? "" : "\\"), rc_did_emsg = TRUE, FAIL)
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
    স্থির পূর্ণাঙ্ক
re_multi_type(পূর্ণাঙ্ক c)
{
    যদি (c == Magic('@') || c == Magic('=') || c == Magic('?'))
	ঘূৰাই MULTI_ONE;
    যদি (c == Magic('*') || c == Magic('+') || c == Magic('{'))
	ঘূৰাই MULTI_MULT;
    ঘূৰাই NOT_MULTI;
}

স্থির char_u		*reg_prev_sub = রিক্ত;
স্থির size_t		reg_prev_sublen = 0;

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
    স্থির পূর্ণাঙ্ক
backslash_trans(পূর্ণাঙ্ক c)
{
    ৱিকল্প (c)
    {
	পক্ষ 'r':   ঘূৰাই CAR;
	পক্ষ 't':   ঘূৰাই TAB;
	পক্ষ 'e':   ঘূৰাই ESC;
	পক্ষ 'b':   ঘূৰাই BS;
    }
    ঘূৰাই c;
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
    স্থির পূর্ণাঙ্ক
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
	স্থির keyvalue_T *last_entry = রিক্ত;

	target.চাবি = 0;
	target.মান.string = *pp + 2;
	target.মান.length = 0;	// not used, see cmp_keyvalue_value_n()

	যদি (last_entry != রিক্ত && cmp_keyvalue_মান_n(&target, last_entry) == 0)
	    entry = last_entry;
	অন্যথা
	    entry = (keyvalue_T *)bsearch(&target, &char_class_tab,
					ARRAY_LENGTH(char_class_tab),
					পরিমাণ(char_class_tab[0]), cmp_keyvalue_মান_n);
	যদি (entry != রিক্ত)
	{
	    last_entry = entry;
	    *pp += entry->মান.length + 2;
	    ঘূৰাই entry->চাবি;
	}
    }
    ঘূৰাই CLASS_NONE;
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
#define	    RI_HEAD	0x10
#define	    RI_ALPHA	0x20
#define	    RI_LOWER	0x40
#define	    RI_UPPER	0x80
#define	    RI_WHITE	0x100

    স্থির শূন্য
init_class_tab(শূন্য)
{
    পূর্ণাঙ্ক		i;
    স্থির পূর্ণাঙ্ক	done = FALSE;

    যদি (done)
	ঘূৰাই;

    বাবে (i = 0; i < 256; ++i)
    {
	যদি (i >= '0' && i <= '7')
	    class_tab[i] = RI_DIGIT + RI_HEX + RI_OCTAL + RI_WORD;
	অন্যথা যদি (i >= '8' && i <= '9')
	    class_tab[i] = RI_DIGIT + RI_HEX + RI_WORD;
	অন্যথা যদি (i >= 'a' && i <= 'f')
	    class_tab[i] = RI_HEX + RI_WORD + RI_HEAD + RI_ALPHA + RI_LOWER;
	অন্যথা যদি (i >= 'g' && i <= 'z')
	    class_tab[i] = RI_WORD + RI_HEAD + RI_ALPHA + RI_LOWER;
	অন্যথা যদি (i >= 'A' && i <= 'F')
	    class_tab[i] = RI_HEX + RI_WORD + RI_HEAD + RI_ALPHA + RI_UPPER;
	অন্যথা যদি (i >= 'G' && i <= 'Z')
	    class_tab[i] = RI_WORD + RI_HEAD + RI_ALPHA + RI_UPPER;
	অন্যথা যদি (i == '_')
	    class_tab[i] = RI_WORD + RI_HEAD;
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
#define ri_head(c)	((c) < 0x100 && (class_tab[c] & RI_HEAD))
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
স্থির পূর্ণাঙ্ক	regnpar;	// () count.
স্থির পূর্ণাঙ্ক	wants_nfa;	// regex should use NFA engine
#ifdef FEAT_SYN_HL
স্থির পূর্ণাঙ্ক	regnzpar;	// \z() count.
স্থির পূর্ণাঙ্ক	re_has_z;	// \z item detected
#endif
স্থির unsigned	regflags;	// RF_ flags for prog
#যদি defined(FEAT_SYN_HL)
স্থির পূর্ণাঙ্ক	had_eol;	// TRUE when EOL found by vim_regcomp()
#endif

স্থির magic_T	reg_magic;	// magicness of the pattern

স্থির পূর্ণাঙ্ক	reg_string;	// matching with a string instead of a buffer
				//সাৰি
স্থির পূর্ণাঙ্ক	reg_strict;	// "[abc" is illegal

/*
 * META contains all characters that may be magic, except '^' and '$'.
 */

// META[] is used often enough to justify turning it into a table.
স্থির ধ্রুৱ char_u META_flags[] = {
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

স্থির পূর্ণাঙ্ক	curchr;		// currently parsed character
// Previous character.  Note: prevchr is sometimes -1 when we are not at the
// start, eg in /[ ^I]^ the pattern was never found even if it existed,
// because ^ was taken to be magic -- webb
স্থির পূর্ণাঙ্ক	prevchr;
স্থির পূর্ণাঙ্ক	prevprevchr;	// previous-previous character
স্থির পূর্ণাঙ্ক	nextchr;	// used for ungetchr()

// arguments for reg()
#define REG_NOPAREN	0	// toplevel reg()
#define REG_PAREN	1	// \(\)
#define REG_ZPAREN	2	// \z(\)
#define REG_NPAREN	3	// \%(\)

// Limit recursive parsing of nested regexp atoms to avoid using up the C stack.
#define REG_MAX_PAREN_DEPTH	1000

প্রকারনির্দেশ সংরচনা
{
    char_u	*regparse;
    পূর্ণাঙ্ক		prevchr_len;
    পূর্ণাঙ্ক		curchr;
    পূর্ণাঙ্ক		prevchr;
    পূর্ণাঙ্ক		prevprevchr;
    পূর্ণাঙ্ক		nextchr;
    পূর্ণাঙ্ক		at_start;
    পূর্ণাঙ্ক		prev_at_start;
    পূর্ণাঙ্ক		regnpar;
} parse_state_T;

স্থির শূন্য	initchr(char_u *);
স্থির পূর্ণাঙ্ক	getchr(শূন্য);
স্থির শূন্য	skipchr_keepstart(শূন্য);
স্থির পূর্ণাঙ্ক	peekchr(শূন্য);
স্থির শূন্য	skipchr(শূন্য);
স্থির শূন্য	ungetchr(শূন্য);
স্থির vimlong_T	gethexchrs(পূর্ণাঙ্ক maxinputlen);
স্থির long	getoctchrs(শূন্য);
স্থির vimlong_T	getdecchrs(শূন্য);
স্থির পূর্ণাঙ্ক	coll_get_char(শূন্য);
স্থির পূর্ণাঙ্ক	prog_magic_wrong(শূন্য);
স্থির পূর্ণাঙ্ক	cstrncmp(char_u *s1, char_u *s2, পূর্ণাঙ্ক *n);
স্থির char_u	*cstrchr(char_u *, পূর্ণাঙ্ক);
স্থির পূর্ণাঙ্ক	re_mult_next(ৱর্ণ *what);
স্থির পূর্ণাঙ্ক	reg_iswordc(পূর্ণাঙ্ক);
#ifdef FEAT_EVAL
স্থির শূন্য report_re_switch(char_u *pat);
#endif

স্থির regengine_T bt_regengine;
স্থির regengine_T nfa_regengine;

/*
 * Return TRUE if compiled regular expression "prog" can match a line break.
 */
    পূর্ণাঙ্ক
re_multiline(regprog_T *prog)
{
    ঘূৰাই (prog->regflags & RF_HASNL);
}

/*
 * Check for an equivalence class name "[=a=]".  "pp" points to the '['.
 * Returns a character representing the class. Zero means that no item was
 * recognized.  Otherwise "pp" is advanced to after the item.
 */
    স্থির পূর্ণাঙ্ক
get_equi_class(char_u **pp)
{
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		l = 1;
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
	    ঘূৰাই c;
	}
    }
    ঘূৰাই 0;
}

/*
 * Check for a collating element "[.a.]".  "pp" points to the '['.
 * Returns a character. Zero means that no item was recognized.  Otherwise
 * "pp" is advanced to after the item.
 * Currently only single characters are recognized!
 */
    স্থির পূর্ণাঙ্ক
get_coll_element(char_u **pp)
{
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		l = 1;
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
	    ঘূৰাই c;
	}
    }
    ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক reg_cpo_lit; // 'cpoptions' contains 'l' flag
স্থির পূর্ণাঙ্ক reg_cpo_bsl; // 'cpoptions' contains '\' flag

    স্থির শূন্য
get_cpo_flags(শূন্য)
{
    reg_cpo_lit = vim_strchr(p_cpo, CPO_LITERAL) != রিক্ত;
    reg_cpo_bsl = vim_strchr(p_cpo, CPO_BACKSL) != রিক্ত;
}

/*
 * Skip over a "[]" range.
 * "p" must point to the character after the '['.
 * The returned pointer is on the matching ']', or the terminating NUL.
 */
    স্থির char_u *
skip_anyof(char_u *p)
{
    পূর্ণাঙ্ক		l;

    যদি (*p == '^')	// Complement of range.
	++p;
    যদি (*p == ']' || *p == '-')
	++p;
    যেতিয়ালৈ (*p != NUL && *p != ']')
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
		&& (vim_strchr(REGEXP_INRANGE, p[1]) != রিক্ত
		    || (!reg_cpo_lit && vim_strchr(REGEXP_ABBR, p[1]) != রিক্ত)))
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

    ঘূৰাই p;
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
    পূর্ণাঙ্ক		delim,
    পূর্ণাঙ্ক		magic)
{
    ঘূৰাই skip_regexp_ex(startp, delim, magic, রিক্ত, রিক্ত, রিক্ত);
}

/*
 * Call skip_regexp() and when the delimiter does not match give an error and
 * return NULL.
 */
    char_u *
skip_regexp_err(
    char_u	*startp,
    পূর্ণাঙ্ক		delim,
    পূর্ণাঙ্ক		magic)
{
    char_u *p = skip_regexp(startp, delim, magic);

    যদি (*p != delim)
    {
	semsg(_(e_missing_delimiter_after_অনুসন্ধান_pattern_str), startp);
	ঘূৰাই রিক্ত;
    }
    ঘূৰাই p;
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
    পূর্ণাঙ্ক		dirc,
    পূর্ণাঙ্ক		magic,
    char_u	**newp,
    পূর্ণাঙ্ক		*dropped,
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

    বাবে (; p[0] != NUL; MB_PTR_ADV(p))
    {
	যদি (p[0] == dirc)	// found end of regexp
	    ভঙ্গ;
	যদি ((p[0] == '[' && mymagic >= MAGIC_ON)
		|| (p[0] == '\\' && p[1] == '[' && mymagic <= MAGIC_OFF))
	{
	    p = skip_anyof(p + 1);
	    যদি (p[0] == NUL)
		ভঙ্গ;
	}
	অন্যথা যদি (p[0] == '\\' && p[1] != NUL)
	{
	    যদি (dirc == '?' && newp != রিক্ত && p[1] == '?')
	    {
		// change "\?" to "?", make a copy first.
		যদি (startplen == 0)
		    startplen = STRLEN(startp);
		যদি (*newp == রিক্ত)
		{
		    *newp = vim_strnsave(startp, startplen);
		    যদি (*newp != রিক্ত)
		    {
			p = *newp + (p - startp);
			startp = *newp;
		    }
		}
		যদি (dropped != রিক্ত)
		    ++*dropped;
		যদি (*newp != রিক্ত)
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
    যদি (magic_val != রিক্ত)
	*magic_val = mymagic;
    ঘূৰাই p;
}

/*
 * Functions for getting characters from the regexp input.
 */
স্থির পূর্ণাঙ্ক	prevchr_len;	// byte length of previous char
স্থির পূর্ণাঙ্ক	at_start;	// True when on the first character
স্থির পূর্ণাঙ্ক	prev_at_start;  // True when on the second character

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
    prev_at_start = FALSE;
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
    ps->prev_at_start = prev_at_start;
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
    prev_at_start = ps->prev_at_start;
    regnpar = ps->regnpar;
}


/*
 * Get the next character without advancing.
 */
    স্থির পূর্ণাঙ্ক
peekchr(শূন্য)
{
    স্থির পূর্ণাঙ্ক	after_slash = FALSE;

    যদি (curchr != -1)
	ঘূৰাই curchr;

    ৱিকল্প (curchr = regparse[0])
    {
	পক্ষ '.':
	পক্ষ '[':
	পক্ষ '~':
	    // magic when 'magic' is on
	    যদি (reg_magic >= MAGIC_ON)
		curchr = Magic(curchr);
	    ভঙ্গ;
	পক্ষ '(':
	পক্ষ ')':
	পক্ষ '{':
	পক্ষ '%':
	পক্ষ '+':
	পক্ষ '=':
	পক্ষ '?':
	পক্ষ '@':
	পক্ষ '!':
	পক্ষ '&':
	পক্ষ '|':
	পক্ষ '<':
	পক্ষ '>':
	পক্ষ '#':	// future ext.
	পক্ষ '"':	// future ext.
	পক্ষ '\'':	// future ext.
	পক্ষ ',':	// future ext.
	পক্ষ '-':	// future ext.
	পক্ষ ':':	// future ext.
	পক্ষ ';':	// future ext.
	পক্ষ '`':	// future ext.
	পক্ষ '/':	// Can't be used in / command
			// magic only after "\v"
	    যদি (reg_magic == MAGIC_ALL)
		curchr = Magic(curchr);
	    ভঙ্গ;
	পক্ষ '*':
	    // * is not magic as the very first character, eg "?*ptr", when
	    // after '^', eg "/^*ptr" and when after "\(", "\|", "\&".  But
	    // "\(\*" is not magic, thus must be magic if "after_slash"
	    যদি (reg_magic >= MAGIC_ON
		    && !at_start
		    && !(prev_at_start && prevchr == Magic('^'))
		    && (after_slash
			|| (prevchr != Magic('(')
			    && prevchr != Magic('&')
			    && prevchr != Magic('|'))))
		curchr = Magic('*');
	    ভঙ্গ;
	পক্ষ '^':
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
		prev_at_start = FALSE;
	    }
	    ভঙ্গ;
	পক্ষ '$':
	    // '$' is only magic as the very last char and if it's in front of
	    // either "\|", "\)", "\&", or "\n"
	    যদি (reg_magic >= MAGIC_OFF)
	    {
		char_u *p = regparse + 1;
		পূর্ণাঙ্ক is_magic_all = (reg_magic == MAGIC_ALL);

		// ignore \c \C \m \M \v \V and \Z after '$'
		যেতিয়ালৈ (p[0] == '\\' && (p[1] == 'c' || p[1] == 'C'
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
	    ভঙ্গ;
	পক্ষ '\\':
	    {
		পূর্ণাঙ্ক c = regparse[1];

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
		    prev_at_start = at_start;
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
		ভঙ্গ;
	    }

	স্ৱভাৱ:
	    // curchr already holds regparse[0]; only a multi-byte lead byte
	    // needs decoding.
	    যদি (has_mbyte && curchr >= 0x80)
		curchr = (*mb_ptr2char)(regparse);
    }

    ঘূৰাই curchr;
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
    prev_at_start = at_start;
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
    পূর্ণাঙ্ক as = prev_at_start;
    পূর্ণাঙ্ক pr = prevchr;
    পূর্ণাঙ্ক prpr = prevprevchr;

    skipchr();
    at_start = as;
    prevchr = pr;
    prevprevchr = prpr;
}

/*
 * Get the next character from the pattern. We know about magic and such, so
 * therefore we need a lexical analyzer.
 */
    স্থির পূর্ণাঙ্ক
getchr(শূন্য)
{
    পূর্ণাঙ্ক chr = peekchr();

    skipchr();
    ঘূৰাই chr;
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
    at_start = prev_at_start;
    prev_at_start = FALSE;

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
gethexchrs(পূর্ণাঙ্ক maxinputlen)
{
    long_u	nr = 0;
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		i;

    বাবে (i = 0; i < maxinputlen; ++i)
    {
	c = regparse[0];
	যদি (!vim_isxdigit(c))
	    ভঙ্গ;
	nr <<= 4;
	nr |= hex2nr(c);
	++regparse;
    }

    যদি (i == 0)
	ঘূৰাই -1;
    ঘূৰাই nr;
}

/*
 * Get and return the value of the decimal string immediately after the
 * current position. Return -1 for invalid.  Consumes all digits.
 */
    স্থির vimlong_T
getdecchrs(শূন্য)
{
    long_u	nr = 0;
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		i;

    বাবে (i = 0; ; ++i)
    {
	c = regparse[0];
	যদি (c < '0' || c > '9')
	    ভঙ্গ;
	nr *= 10;
	nr += c - '0';
	++regparse;
	curchr = -1; // no longer valid
    }

    যদি (i == 0)
	ঘূৰাই -1;
    ঘূৰাই nr;
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
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		i;

    বাবে (i = 0; i < 3 && nr < 040; ++i)
    {
	c = regparse[0];
	যদি (c < '0' || c > '7')
	    ভঙ্গ;
	nr <<= 3;
	nr |= hex2nr(c);
	++regparse;
    }

    যদি (i == 0)
	ঘূৰাই -1;
    ঘূৰাই (long)nr;
}

/*
 * read_limits - Read two integers to be taken as a minimum and maximum.
 * If the first character is '-', then the range is reversed.
 * Should end with 'end'.  If minval is missing, zero is default, if maxval is
 * missing, a very big number is the default.
 */
    স্থির পূর্ণাঙ্ক
read_limits(long *minval, long *maxval)
{
    পূর্ণাঙ্ক		reverse = FALSE;
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
    ঘূৰাই OK;
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
স্থির পূর্ণাঙ্ক	match_with_backref(linenr_T start_lnum, colnr_T start_col, linenr_T end_lnum, colnr_T end_col, পূর্ণাঙ্ক *bytelen);

/*
 * Sometimes need to save a copy of a line.  Since alloc()/free() is very
 * slow, we keep one allocated piece of memory and only re-allocate it when
 * it's too small.  It's freed in bt_regexec_both() when finished.
 */
স্থির char_u	*reg_tofree = রিক্ত;
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
প্রকারনির্দেশ সংরচনা {
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
    পূর্ণাঙ্ক			reg_সাৰি_lbr;	// "\n" in string is line break

    // The current match-position is stord in these variables:
    linenr_T	lnum;		// line number, relative to first line
    char_u	*সাৰি;		// start of current line
    char_u	*প্ৰবেশ;		// current input, points into "line"

    পূর্ণাঙ্ক	need_clear_subexpr;	// subexpressions still need to be cleared
#ifdef FEAT_SYN_HL
    পূর্ণাঙ্ক	need_clear_zsubexpr;	// extmatch subexpressions still need to be
				// cleared
#endif

    // Internal copy of 'ignorecase'.  It is set at each call to vim_regexec().
    // Normally it gets the value of "rm_ic" or "rmm_ic", but when the pattern
    // contains '\c' or '\C' the value is overruled.
    পূর্ণাঙ্ক			reg_ic;

    // Similar to "reg_ic", but only for 'combining' characters.  Set with \Z
    // flag in the regexp.  Defaults to false, always.
    পূর্ণাঙ্ক			reg_icombine;

    // Copy of "rmm_maxcol": maximum column to search for a match.  Zero when
    // there is no maximum.
    colnr_T		reg_maxcol;

    // State for the NFA engine regexec.
    পূর্ণাঙ্ক nfa_has_zend;	    // NFA regexp \ze operator encountered.
    পূর্ণাঙ্ক nfa_has_backref;    // NFA regexp \1 .. \9 encountered.
    পূর্ণাঙ্ক nfa_nsubexpr;	    // Number of sub expressions actually being used
			    // during execution. 1 if only the whole match
			    // (subexpr 0) is used.
    // listid is global, so that it increases on recursive calls to
    // nfa_regmatch(), which means we don't have to clear the lastlist field of
    // all the states.
    পূর্ণাঙ্ক nfa_listid;
    পূর্ণাঙ্ক nfa_alt_listid;

#ifdef FEAT_SYN_HL
    পূর্ণাঙ্ক nfa_has_zsubexpr;   // NFA regexp has \z( ), set zsubexpr.
#endif
} regexec_T;

স্থির regexec_T	rex;
স্থির পূর্ণাঙ্ক		rex_in_use = FALSE;

/*
 * Return TRUE if character 'c' is included in 'iskeyword' option for
 * "reg_buf" buffer.
 */
    স্থির পূর্ণাঙ্ক
reg_iswordc(পূর্ণাঙ্ক c)
{
    ঘূৰাই vim_iswordc_buf(c, rex.reg_buf);
}

#ifdef FEAT_EVAL
স্থির পূর্ণাঙ্ক can_f_submatch = FALSE;	// TRUE when submatch() can be used

// This struct is used for reg_submatch(). Needed for when the
// substitution string is an expression that contains a call to substitute()
// and submatch().
প্রকারনির্দেশ সংরচনা {
    regmatch_T	*sm_match;
    regmmatch_T	*sm_mmatch;
    linenr_T	sm_firstlnum;
    linenr_T	sm_maxline;
    পূর্ণাঙ্ক		sm_সাৰি_lbr;
} regsubmatch_T;

স্থির regsubmatch_T rsm;  // can only be used when can_f_submatch is TRUE
#endif

প্রকারনির্দেশ গণনা
{
    RGLF_সাৰি = 0x01,
    RGLF_LENGTH = 0x02
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
    char_u		**সাৰি,
    colnr_T		*length)
{
    পূর্ণাঙ্ক get_সাৰি = flags & RGLF_সাৰি;
    পূর্ণাঙ্ক get_length = flags & RGLF_LENGTH;
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
	যদি (get_সাৰি)
	    *সাৰি = রিক্ত;
	যদি (get_length)
	    *length = 0;

	ঘূৰাই;
    }

    যদি (lnum > maxline)
    {
	// must have matched the "\n" in the last line.
	যদি (get_সাৰি)
	    *সাৰি = (char_u *)"";
	যদি (get_length)
	    *length = 0;

	ঘূৰাই;
    }

    যদি (get_সাৰি)
	*সাৰি = ml_get_buf(rex.reg_buf, firstlnum, FALSE);
    যদি (get_length)
	*length = ml_get_buf_len(rex.reg_buf, firstlnum);
}

/*
 * Get pointer to the line "lnum", which is relative to "reg_firstlnum".
 */
    স্থির char_u *
reg_getline(linenr_T lnum)
{
    char_u *সাৰি;

    reg_getline_common(lnum, RGLF_সাৰি, &সাৰি, রিক্ত);

    ঘূৰাই সাৰি;
}

/*
 * Get length of line "lnum", which is relative to "reg_firstlnum".
 */
    স্থির colnr_T
reg_getline_len(linenr_T lnum)
{
    colnr_T length;

    reg_getline_common(lnum, RGLF_LENGTH, রিক্ত, &length);

    ঘূৰাই length;
}

#ifdef FEAT_SYN_HL
স্থির char_u	*reg_startzp[NSUBEXP];	// Workspace to mark beginning
স্থির char_u	*reg_endzp[NSUBEXP];	//   and end of \z(...\) matches
স্থির lpos_T	reg_startzpos[NSUBEXP];	// idem, beginning pos
স্থির lpos_T	reg_endzpos[NSUBEXP];	// idem, end pos
#endif

// TRUE if using multi-line regexp.
#define REG_MULTI	(rex.reg_match == রিক্ত)

#ifdef FEAT_SYN_HL
/*
 * Create a new extmatch and mark it as referenced once.
 */
    স্থির reg_extmatch_T *
make_extmatch(শূন্য)
{
    reg_extmatch_T	*em;

    em = ALLOC_CLEAR_ONE(reg_extmatch_T);
    যদি (em != রিক্ত)
	em->refcnt = 1;
    ঘূৰাই em;
}

/*
 * Add a reference to an extmatch.
 */
    reg_extmatch_T *
ref_extmatch(reg_extmatch_T *em)
{
    যদি (em != রিক্ত)
	em->refcnt++;
    ঘূৰাই em;
}

/*
 * Remove a reference to an extmatch.  If there are no references left, free
 * the info.
 */
    শূন্য
unref_extmatch(reg_extmatch_T *em)
{
    পূর্ণাঙ্ক i;

    যদি (em != রিক্ত && --em->refcnt <= 0)
    {
	বাবে (i = 0; i < NSUBEXP; ++i)
	    vim_free(em->matches[i]);
	vim_free(em);
    }
}
#endif

/*
 * Get class of previous character.
 */
    স্থির পূর্ণাঙ্ক
reg_prev_class(শূন্য)
{
    যদি (rex.প্ৰবেশ > rex.সাৰি)
	ঘূৰাই mb_get_class_buf(rex.প্ৰবেশ - 1
		       - (*mb_head_off)(rex.সাৰি, rex.প্ৰবেশ - 1), rex.reg_buf);
    ঘূৰাই -1;
}

/*
 * Return TRUE if the current rex.input position matches the Visual area.
 */
    স্থির পূর্ণাঙ্ক
reg_match_visual(শূন্য)
{
    pos_T	শীৰ্ষ, bot;
    linenr_T    lnum;
    colnr_T	col;
    win_T	*wp = rex.reg_win == রিক্ত ? curwin : rex.reg_win;
    পূর্ণাঙ্ক		mode;
    colnr_T	start, end;
    colnr_T	start2, end2;
    colnr_T	cols;
    colnr_T	curswant;

    // Check if the buffer is the current buffer and not using a string.
    যদি (rex.reg_buf != curbuf || VIsual.lnum == 0 || !REG_MULTI)
	ঘূৰাই FALSE;

    যদি (VIsual_active)
    {
	যদি (LT_POS(VIsual, wp->w_cursor))
	{
	    শীৰ্ষ = VIsual;
	    bot = wp->w_cursor;
	}
	অন্যথা
	{
	    শীৰ্ষ = wp->w_cursor;
	    bot = VIsual;
	}
	mode = VIsual_mode;
	curswant = wp->w_curswant;
    }
    অন্যথা
    {
	যদি (LT_POS(curbuf->b_visual.vi_start, curbuf->b_visual.vi_end))
	{
	    শীৰ্ষ = curbuf->b_visual.vi_start;
	    bot = curbuf->b_visual.vi_end;
	}
	অন্যথা
	{
	    শীৰ্ষ = curbuf->b_visual.vi_end;
	    bot = curbuf->b_visual.vi_start;
	}
	// a substitute command may have removed some lines
	যদি (bot.lnum > curbuf->b_ml.ml_সাৰি_গুনতি)
	    bot.lnum = curbuf->b_ml.ml_সাৰি_গুনতি;
	mode = curbuf->b_visual.vi_mode;
	curswant = curbuf->b_visual.vi_curswant;
    }
    lnum = rex.lnum + rex.reg_firstlnum;
    যদি (lnum < শীৰ্ষ.lnum || lnum > bot.lnum)
	ঘূৰাই FALSE;

    col = (colnr_T)(rex.প্ৰবেশ - rex.সাৰি);
    যদি (mode == 'v')
    {
	যদি ((lnum == শীৰ্ষ.lnum && col < শীৰ্ষ.col)
		|| (lnum == bot.lnum && col >= bot.col + (*p_sel != 'e')))
	    ঘূৰাই FALSE;
    }
    অন্যথা যদি (mode == Ctrl_V)
    {
	getvvcol(wp, &শীৰ্ষ, &start, রিক্ত, &end, 0);
	getvvcol(wp, &bot, &start2, রিক্ত, &end2, 0);
	যদি (start2 < start)
	    start = start2;
	যদি (end2 > end)
	    end = end2;
	যদি (শীৰ্ষ.col == MAXCOL || bot.col == MAXCOL || curswant == MAXCOL)
	    end = MAXCOL;

	// getvvcol() flushes rex.line, need to get it again
	rex.সাৰি = reg_getline(rex.lnum);
	rex.প্ৰবেশ = rex.সাৰি + col;

	cols = win_linetabsize(wp, rex.reg_firstlnum + rex.lnum, rex.সাৰি, col);
	যদি (cols < start || cols > end - (*p_sel == 'e'))
	    ঘূৰাই FALSE;
    }
    ঘূৰাই TRUE;
}

/*
 * Check the regexp program for its magic number.
 * Return TRUE if it's wrong.
 */
    স্থির পূর্ণাঙ্ক
prog_magic_wrong(শূন্য)
{
    regprog_T	*prog;

    prog = REG_MULTI ? rex.reg_mmatch->regprog : rex.reg_match->regprog;
    যদি (prog->engine == &nfa_regengine)
	// For NFA matcher we don't check the magic
	ঘূৰাই FALSE;

    যদি (UCHARAT(((bt_regprog_T *)prog)->program) != REGMAGIC)
    {
	iemsg(e_corrupted_regexp_program);
	ঘূৰাই TRUE;
    }
    ঘূৰাই FALSE;
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
	ঘূৰাই;

    যদি (REG_MULTI)
    {
	// Use 0xff to set lnum to -1
	vim_memset(rex.reg_startpos, 0xff, পরিমাণ(lpos_T) * NSUBEXP);
	vim_memset(rex.reg_endpos, 0xff, পরিমাণ(lpos_T) * NSUBEXP);
    }
    অন্যথা
    {
	vim_memset(rex.reg_startp, 0, পরিমাণ(char_u *) * NSUBEXP);
	vim_memset(rex.reg_endp, 0, পরিমাণ(char_u *) * NSUBEXP);
    }
    rex.need_clear_subexpr = FALSE;
}

#ifdef FEAT_SYN_HL
    স্থির শূন্য
cleanup_zsubexpr(শূন্য)
{
    যদি (!rex.need_clear_zsubexpr)
	ঘূৰাই;

    যদি (REG_MULTI)
    {
	// Use 0xff to set lnum to -1
	vim_memset(reg_startzpos, 0xff, পরিমাণ(lpos_T) * NSUBEXP);
	vim_memset(reg_endzpos, 0xff, পরিমাণ(lpos_T) * NSUBEXP);
    }
    অন্যথা
    {
	vim_memset(reg_startzp, 0, পরিমাণ(char_u *) * NSUBEXP);
	vim_memset(reg_endzp, 0, পরিমাণ(char_u *) * NSUBEXP);
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
    rex.সাৰি = reg_getline(++rex.lnum);
    rex.প্ৰবেশ = rex.সাৰি;
    fast_breakcheck();
}

/*
 * Check whether a backreference matches.
 * Returns RA_FAIL, RA_NOMATCH or RA_MATCH.
 * If "bytelen" is not NULL, it is set to the byte length of the match in the
 * last line.
 * Optional: ignore case if rex.reg_ic is set.
 */
    স্থির পূর্ণাঙ্ক
match_with_backref(
    linenr_T start_lnum,
    colnr_T  start_col,
    linenr_T end_lnum,
    colnr_T  end_col,
    পূর্ণাঙ্ক	     *bytelen)
{
    linenr_T	clnum = start_lnum;
    colnr_T	ccol = start_col;
    পূর্ণাঙ্ক		len;
    char_u	*p;

    যদি (bytelen != রিক্ত)
	*bytelen = 0;
    বাবে (;;)
    {
	// Since getting one line may invalidate the other, need to make copy.
	// Slow!
	যদি (rex.সাৰি != reg_tofree)
	{
	    len = (পূর্ণাঙ্ক)STRLEN(rex.সাৰি);
	    যদি (reg_tofree == রিক্ত || len >= (পূর্ণাঙ্ক)reg_tofreelen)
	    {
		len += 50;	// get some extra
		vim_free(reg_tofree);
		reg_tofree = alloc(len);
		যদি (reg_tofree == রিক্ত)
		    ঘূৰাই RA_FAIL; // out of memory!
		reg_tofreelen = len;
	    }
	    STRCPY(reg_tofree, rex.সাৰি);
	    rex.প্ৰবেশ = reg_tofree + (rex.প্ৰবেশ - rex.সাৰি);
	    rex.সাৰি = reg_tofree;
	}

	// Get the line to compare with.
	p = reg_getline(clnum);
	যদি (clnum == end_lnum)
	    len = end_col - ccol;
	অন্যথা
	    len = (পূর্ণাঙ্ক)reg_getline_len(clnum) - ccol;

	// Use case-insensitive compare if rex.reg_ic is set
	যদি ((!rex.reg_ic && cstrncmp(p + ccol, rex.প্ৰবেশ, &len) != 0)
	    || (rex.reg_ic && MB_STRNICMP(p + ccol, rex.প্ৰবেশ, len) != 0))
	    ঘূৰাই RA_NOMATCH;  // doesn't match
	যদি (bytelen != রিক্ত)
	    *bytelen += len;
	যদি (clnum == end_lnum)
	    ভঙ্গ;		// match and at end!
	যদি (rex.lnum >= rex.reg_maxline)
	    ঘূৰাই RA_NOMATCH;  // text too short

	// Advance to next line.
	reg_nextline();
	যদি (bytelen != রিক্ত)
	    *bytelen = 0;
	++clnum;
	ccol = 0;
	যদি (got_int)
	    ঘূৰাই RA_FAIL;
    }

    // found a match!  Note that rex.line may now point to a copy of the line,
    // that should not matter.
    ঘূৰাই RA_MATCH;
}

/*
 * Used in a place where no * or \+ can follow.
 */
    স্থির পূর্ণাঙ্ক
re_mult_next(ৱর্ণ *what)
{
    যদি (re_multi_type(peekchr()) == MULTI_MULT)
    {
	semsg(_(e_nfa_regexp_cannot_repeat_str), what);
	rc_did_emsg = TRUE;
	ঘূৰাই FAIL;
    }
    ঘূৰাই OK;
}

প্রকারনির্দেশ সংরচনা
{
    পূর্ণাঙ্ক a, b, c;
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
mb_decompose(পূর্ণাঙ্ক c, পূর্ণাঙ্ক *c1, পূর্ণাঙ্ক *c2, পূর্ণাঙ্ক *c3)
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
    স্থির পূর্ণাঙ্ক
cstrncmp(char_u *s1, char_u *s2, পূর্ণাঙ্ক *n)
{
    পূর্ণাঙ্ক		result;

    যদি (!rex.reg_ic)
	result = STRNCMP(s1, s2, *n);
    অন্যথা যদি (enc_utf8)
    {
	char_u *p = s1;
	পূর্ণাঙ্ক n2 = 0;
	পূর্ণাঙ্ক n1 = *n;
	// count the number of characters for byte-length of s1
	যেতিয়ালৈ (n1 > 0 && *p != NUL)
	{
	    n1 -= mb_ptr2len(p);
	    MB_PTR_ADV(p);
	    n2++;
	}
	// count the number of bytes to advance the same number of chars for s2
	p = s2;
	যেতিয়ালৈ (n2-- > 0 && *p != NUL)
	    MB_PTR_ADV(p);

	n2 = p - s2;

	result = MB_STRNICMP2(s1, s2, *n, n2);
	যদি (result == 0)
	    *n = n2;
    }
    অন্যথা
	result = MB_STRNICMP(s1, s2, *n);

    // if it failed and it's utf8 and we want to combineignore:
    যদি (result != 0 && enc_utf8 && rex.reg_icombine)
    {
	char_u	*str1, *str2;
	পূর্ণাঙ্ক	c1, c2, c11, c12;
	পূর্ণাঙ্ক	junk;

	// we have to handle the strcmp ourselves, since it is necessary to
	// deal with the composing characters by ignoring them:
	str1 = s1;
	str2 = s2;
	c1 = c2 = 0;
	যেতিয়ালৈ ((পূর্ণাঙ্ক)(str1 - s1) < *n)
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
		    ভঙ্গ;
	    }
	}
	result = c2 - c1;
	যদি (result == 0)
	    *n = (পূর্ণাঙ্ক)(str2 - s2);
    }

    ঘূৰাই result;
}

/*
 * cstrchr: This function is used a lot for simple searches, keep it fast!
 */
    স্থির char_u *
cstrchr(char_u *s, পূর্ণাঙ্ক c)
{
    char_u	*p;
    পূর্ণাঙ্ক		cc, lc;

    যদি (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	ঘূৰাই vim_strchr(s, c);

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
	    ঘূৰাই vim_strchr(s, c);

    যদি (has_mbyte)
    {
	বাবে (p = s; *p != NUL; p += (*mb_ptr2len)(p))
	{
	    পূর্ণাঙ্ক uc = utf_ptr2char(p);
	    যদি (enc_utf8 && (c > 0x80 || uc > 0x80))
	    {
		// Do not match an illegal byte.  E.g. 0xff matches 0xc3 0xbf,
		// not 0xff.
		// compare with lower case of the character
		যদি ((uc < 0x80 || uc != *p) && utf_fold(uc) == lc)
		    ঘূৰাই p;
	    }
	    অন্যথা যদি (*p == c || *p == cc)
		ঘূৰাই p;
	}
    }
    অন্যথা
	// Faster version for when there are no multi-byte characters.
	বাবে (p = s; *p != NUL; ++p)
	    যদি (*p == c || *p == cc)
		ঘূৰাই p;

    ঘূৰাই রিক্ত;
}

////////////////////////////////////////////////////////////////
//		      regsub stuff			      //
////////////////////////////////////////////////////////////////

প্রকারনির্দেশ শূন্য (*fptr_T)(পূর্ণাঙ্ক *, পূর্ণাঙ্ক);

স্থির পূর্ণাঙ্ক vim_regsub_both(char_u *source, typval_T *ৰাশিমালা, char_u *dest, পূর্ণাঙ্ক destlen, পূর্ণাঙ্ক flags);

    স্থির শূন্য
do_upper(পূর্ণাঙ্ক *d, পূর্ণাঙ্ক c)
{
    *d = MB_TOUPPER(c);
}

    স্থির শূন্য
do_lower(পূর্ণাঙ্ক *d, পূর্ণাঙ্ক c)
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
regtilde(char_u *source, পূর্ণাঙ্ক magic)
{
    char_u	*newsub = source;
    char_u	*p;
    size_t	newsublen = 0;
    char_u	tilde[3] = {'~', NUL, NUL};
    size_t	tildelen = 1;
    পূর্ণাঙ্ক		error = FALSE;

    যদি (!magic)
    {
	tilde[0] = '\\';
	tilde[1] = '~';
	tilde[2] = NUL;
	tildelen = 2;
    }

    বাবে (p = newsub; *p; ++p)
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
	    tmpsublen = prefixlen + reg_prev_sublen + postfixlen;

	    যদি (tmpsublen > 0 && reg_prev_sub != রিক্ত)
	    {
		char_u *tmpsub;

		// Avoid making the text longer than MAXCOL, it will cause
		// trouble at some point.
		যদি (tmpsublen > MAXCOL)
		{
		    emsg(_(e_resulting_text_too_long));
		    error = TRUE;
		    ভঙ্গ;
		}

		tmpsub = alloc(tmpsublen + 1);
		যদি (tmpsub == রিক্ত)
		{
		    emsg(_(e_out_of_memory));
		    error = TRUE;
		    ভঙ্গ;
		}

		// copy prefix
		mch_memmove(tmpsub, newsub, prefixlen);
		// interpret tilde
		mch_memmove(tmpsub + prefixlen, reg_prev_sub, reg_prev_sublen);
		// copy postfix
		STRCPY(tmpsub + prefixlen + reg_prev_sublen, postfix);

		যদি (newsub != source)	// allocated newsub before
		    vim_free(newsub);
		newsub = tmpsub;
		newsublen = tmpsublen;
		p = newsub + prefixlen + reg_prev_sublen;
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
	ঘূৰাই source;
    }

    // Store a copy of newsub  in reg_prev_sub.  It is always allocated,
    // because recursive calls may make the returned string invalid.
    // Only store it if there something to store.
    newsublen = p - newsub;
    যদি (newsublen == 0)
	VIM_CLEAR(reg_prev_sub);
    অন্যথা
    {
	vim_free(reg_prev_sub);
	reg_prev_sub = vim_strnsave(newsub, newsublen);
    }

    যদি (reg_prev_sub == রিক্ত)
	reg_prev_sublen = 0;
    অন্যথা
	reg_prev_sublen = newsublen;

    ঘূৰাই newsub;
}

#ifdef FEAT_EVAL

/*
 * Put the submatches in "argv[argskip]" which is a list passed into
 * call_func() by vim_regsub_both().
 */
    স্থির পূর্ণাঙ্ক
fill_submatch_list(পূর্ণাঙ্ক argc UNUSED, typval_T *argv, পূর্ণাঙ্ক argskip, ufunc_T *fp)
{
    listitem_T	*li;
    পূর্ণাঙ্ক		i;
    char_u	*s;
    typval_T	*listarg = argv + argskip;

    যদি (!has_varargs(fp) && fp->uf_args.ga_len <= argskip)
	// called function doesn't take a submatches argument
	ঘূৰাই argskip;

    // Relies on sl_list to be the first item in staticList10_T.
    init_static_list((staticList10_T *)(listarg->vval.v_list));

    // There are always 10 list items in staticList10_T.
    li = listarg->vval.v_list->lv_first;
    বাবে (i = 0; i < 10; ++i)
    {
	s = rsm.sm_match->startp[i];
	যদি (s == রিক্ত || rsm.sm_match->endp[i] == রিক্ত)
	    s = রিক্ত;
	অন্যথা
	    s = vim_strnsave(s, rsm.sm_match->endp[i] - s);
	li->li_tv.v_type = VAR_STRING;
	li->li_tv.vval.v_string = s;
	li = li->li_next;
    }
    ঘূৰাই argskip + 1;
}

    স্থির শূন্য
clear_submatch_list(staticList10_T *sl)
{
    পূর্ণাঙ্ক i;

    বাবে (i = 0; i < 10; ++i)
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
    পূর্ণাঙ্ক
vim_regsub(
    regmatch_T	*rmp,
    char_u	*source,
    typval_T	*ৰাশিমালা,
    char_u	*dest,
    পূর্ণাঙ্ক		destlen,
    পূর্ণাঙ্ক		flags)
{
    পূর্ণাঙ্ক		result;
    regexec_T	rex_save;
    পূর্ণাঙ্ক		rex_in_use_save = rex_in_use;

    যদি (rex_in_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_in_use = TRUE;

    rex.reg_match = rmp;
    rex.reg_mmatch = রিক্ত;
    rex.reg_maxline = 0;
    rex.reg_buf = curbuf;
    rex.reg_সাৰি_lbr = TRUE;
    result = vim_regsub_both(source, ৰাশিমালা, dest, destlen, flags);

    rex_in_use = rex_in_use_save;
    যদি (rex_in_use)
	rex = rex_save;

    ঘূৰাই result;
}

    পূর্ণাঙ্ক
vim_regsub_multi(
    regmmatch_T	*rmp,
    linenr_T	lnum,
    char_u	*source,
    char_u	*dest,
    পূর্ণাঙ্ক		destlen,
    পূর্ণাঙ্ক		flags)
{
    পূর্ণাঙ্ক		result;
    regexec_T	rex_save;
    পূর্ণাঙ্ক		rex_in_use_save = rex_in_use;

    যদি (rex_in_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_in_use = TRUE;

    rex.reg_match = রিক্ত;
    rex.reg_mmatch = rmp;
    rex.reg_buf = curbuf;	// always works on the current buffer!
    rex.reg_firstlnum = lnum;
    rex.reg_maxline = curbuf->b_ml.ml_সাৰি_গুনতি - lnum;
    rex.reg_সাৰি_lbr = FALSE;
    result = vim_regsub_both(source, রিক্ত, dest, destlen, flags);

    rex_in_use = rex_in_use_save;
    যদি (rex_in_use)
	rex = rex_save;

    ঘূৰাই result;
}

#যদি defined(FEAT_EVAL)
// When nesting more than a couple levels it's probably a mistake.
# define MAX_REGSUB_NESTING 4
স্থির char_u   *eval_result[MAX_REGSUB_NESTING] = {রিক্ত, রিক্ত, রিক্ত, রিক্ত};

# যদি defined(EXITFREE)
    শূন্য
free_resub_eval_result(শূন্য)
{
    পূর্ণাঙ্ক i;

    বাবে (i = 0; i < MAX_REGSUB_NESTING; ++i)
	VIM_CLEAR(eval_result[i]);
}
# endif
#endif

    স্থির পূর্ণাঙ্ক
vim_regsub_both(
    char_u	*source,
    typval_T	*ৰাশিমালা,
    char_u	*dest,
    পূর্ণাঙ্ক		destlen,
    পূর্ণাঙ্ক		flags)
{
    char_u	*src;
    char_u	*dst;
    char_u	*s;
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		cc;
    পূর্ণাঙ্ক		no = -1;
    fptr_T	func_all = (fptr_T)রিক্ত;
    fptr_T	func_one = (fptr_T)রিক্ত;
    linenr_T	clnum = 0;	// init for GCC
    পূর্ণাঙ্ক		len = 0;	// init for GCC
#ifdef FEAT_EVAL
    স্থির পূর্ণাঙ্ক  nesting = 0;
    পূর্ণাঙ্ক		nested;
#endif
    পূর্ণাঙ্ক		copy = flags & REGSUB_COPY;

    // Be paranoid...
    যদি ((source == রিক্ত && ৰাশিমালা == রিক্ত) || dest == রিক্ত)
    {
	iemsg(e_null_argument);
	ঘূৰাই 0;
    }
    যদি (prog_magic_wrong())
	ঘূৰাই 0;
#ifdef FEAT_EVAL
    যদি (nesting == MAX_REGSUB_NESTING)
    {
	emsg(_(e_substitute_nesting_too_deep));
	ঘূৰাই 0;
    }
    nested = nesting;
#endif
    src = source;
    dst = dest;

    /*
     * When the substitute part starts with "\=" evaluate it as an expression.
     */
    যদি (ৰাশিমালা != রিক্ত || (source[0] == '\\' && source[1] == '='))
    {
#ifdef FEAT_EVAL
	// To make sure that the length doesn't change between checking the
	// length and copying the string, and to speed up things, the
	// resulting string is saved from the call with
	// "flags & REGSUB_COPY" == 0 to the call with
	// "flags & REGSUB_COPY" != 0.
	যদি (copy)
	{
	    যদি (eval_result[nested] != রিক্ত)
	    {
		পূর্ণাঙ্ক eval_len = (পূর্ণাঙ্ক)STRLEN(eval_result[nested]);

		যদি (eval_len < destlen)
		{
		    STRCPY(dest, eval_result[nested]);
		    dst += eval_len;
		    VIM_CLEAR(eval_result[nested]);
		}
	    }
	}
	অন্যথা
	{
	    পূর্ণাঙ্ক		    prev_can_f_submatch = can_f_submatch;
	    regsubmatch_T   rsm_save;

	    VIM_CLEAR(eval_result[nested]);

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
	    rsm.sm_সাৰি_lbr = rex.reg_সাৰি_lbr;

	    // Although unlikely, it is possible that the expression invokes a
	    // substitute command (it might fail, but still).  Therefore keep
	    // an array of eval results.
	    ++nesting;

	    যদি (ৰাশিমালা != রিক্ত)
	    {
		typval_T	argv[2];
		char_u		buf[NUMBUFLEN];
		typval_T	rettv;
		staticList10_T	matchList;
		funcexe_T	funcexe;

		rettv.v_type = VAR_STRING;
		rettv.vval.v_string = রিক্ত;
		argv[0].v_type = VAR_LIST;
		argv[0].vval.v_list = &matchList.sl_list;
		matchList.sl_list.lv_len = 0;
		CLEAR_FIELD(funcexe);
		funcexe.fe_argv_func = fill_submatch_list;
		funcexe.fe_evaluate = TRUE;
		যদি (ৰাশিমালা->v_type == VAR_FUNC)
		{
		    s = ৰাশিমালা->vval.v_string;
		    call_func(s, -1, &rettv, 1, argv, &funcexe);
		}
		অন্যথা যদি (ৰাশিমালা->v_type == VAR_PARTIAL)
		{
		    partial_T   *partial = ৰাশিমালা->vval.v_partial;

		    s = partial_name(partial);
		    funcexe.fe_partial = partial;
		    call_func(s, -1, &rettv, 1, argv, &funcexe);
		}
		অন্যথা যদি (ৰাশিমালা->v_type == VAR_INSTR)
		{
		    exe_typval_instr(ৰাশিমালা, &rettv);
		}
		যদি (matchList.sl_list.lv_len > 0)
		    // fill_submatch_list() was called
		    clear_submatch_list(&matchList);

		যদি (rettv.v_type == VAR_UNKNOWN)
		    // something failed, no need to report another error
		    eval_result[nested] = রিক্ত;
		অন্যথা
		{
		    eval_result[nested] = tv_get_string_buf_chk(&rettv, buf);
		    যদি (eval_result[nested] != রিক্ত)
			eval_result[nested] = vim_strsave(eval_result[nested]);
		}
		clear_tv(&rettv);
	    }
	    অন্যথা যদি (substitute_instr != রিক্ত)
		// Execute instructions from ISN_SUBSTITUTE.
		eval_result[nested] = exe_substitute_instr();
	    অন্যথা
		eval_result[nested] = eval_to_string(source + 2, TRUE, FALSE);
	    --nesting;

	    যদি (eval_result[nested] != রিক্ত)
	    {
		পূর্ণাঙ্ক had_backslash = FALSE;

		বাবে (s = eval_result[nested]; *s != NUL; MB_PTR_ADV(s))
		{
		    // Change NL to CR, so that it becomes a line break,
		    // unless called from vim_regexec_nl().
		    // Skip over a backslashed character.
		    যদি (*s == NL && !rsm.sm_সাৰি_lbr)
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
			যদি (*s == NL && !rsm.sm_সাৰি_lbr)
			    *s = CAR;
			had_backslash = TRUE;
		    }
		}
		যদি (had_backslash && (flags & REGSUB_BACKSLASH))
		{
		    // Backslashes will be consumed, need to double them.
		    s = vim_strsave_escaped(eval_result[nested], (char_u *)"\\");
		    যদি (s != রিক্ত)
		    {
			vim_free(eval_result[nested]);
			eval_result[nested] = s;
		    }
		}

		dst += STRLEN(eval_result[nested]);
	    }

	    can_f_submatch = prev_can_f_submatch;
	    যদি (can_f_submatch)
		rsm = rsm_save;
	}
#endif
    }
    অন্যথা
      যেতিয়ালৈ ((c = *src++) != NUL)
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
		ৱিকল্প (*src++)
		{
		পক্ষ 'u':   func_one = do_upper;
			    অনুৱর্ত;
		পক্ষ 'U':   func_all = do_upper;
			    অনুৱর্ত;
		পক্ষ 'l':   func_one = do_lower;
			    অনুৱর্ত;
		পক্ষ 'L':   func_all = do_lower;
			    অনুৱর্ত;
		পক্ষ 'e':
		পক্ষ 'E':   func_one = func_all = (fptr_T)রিক্ত;
			    অনুৱর্ত;
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
			ঘূৰাই 0;
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
		অনুৱর্ত;
	    }

	    যদি (c == '\\' && *src != NUL)
	    {
		// Check for abbreviations -- webb
		ৱিকল্প (*src)
		{
		    পক্ষ 'r':	c = CAR;	++src;	ভঙ্গ;
		    পক্ষ 'n':	c = NL;		++src;	ভঙ্গ;
		    পক্ষ 't':	c = TAB;	++src;	ভঙ্গ;
		 // Oh no!  \e already has meaning in subst pat :-(
		 // case 'e':   c = ESC;	++src;	break;
		    পক্ষ 'b':	c = Ctrl_H;	++src;	ভঙ্গ;

		    // If "backslash" is TRUE the backslash will be removed
		    // later.  Used to insert a literal CR.
		    স্ৱভাৱ:	যদি (flags & REGSUB_BACKSLASH)
				{
				    যদি (copy)
				    {
					যদি (dst + 1 > dest + destlen)
					{
					    iemsg("vim_regsub_both(): not enough space");
					    ঘূৰাই 0;
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
	    যদি (func_one != (fptr_T)রিক্ত)
	    {
		func_one(&cc, c);
		func_one = রিক্ত;
	    }
	    অন্যথা যদি (func_all != (fptr_T)রিক্ত)
		func_all(&cc, c);
	    অন্যথা // just copy
		cc = c;

	    যদি (has_mbyte)
	    {
		পূর্ণাঙ্ক totlen = mb_ptr2len(src - 1);
		পূর্ণাঙ্ক charlen = mb_char2len(cc);

		যদি (copy)
		{
		    যদি (dst + charlen > dest + destlen)
		    {
			iemsg("vim_regsub_both(): not enough space");
			ঘূৰাই 0;
		    }
		    mb_char2bytes(cc, dst);
		}
		dst += charlen - 1;
		যদি (enc_utf8)
		{
		    পূর্ণাঙ্ক clen = utf_ptr2len(src - 1);

		    // If the character length is shorter than "totlen", there
		    // are composing characters; copy them as-is.
		    যদি (clen < totlen)
		    {
			যদি (copy)
			{
			    যদি (dst + totlen - clen > dest + destlen)
			    {
				iemsg("vim_regsub_both(): not enough space");
				ঘূৰাই 0;
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
		    ঘূৰাই 0;
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
		    s = রিক্ত;
		অন্যথা
		{
		    s = reg_getline(clnum) + rex.reg_mmatch->startpos[no].col;
		    যদি (rex.reg_mmatch->endpos[no].lnum == clnum)
			len = rex.reg_mmatch->endpos[no].col
					    - rex.reg_mmatch->startpos[no].col;
		    অন্যথা
			len = (পূর্ণাঙ্ক)reg_getline_len(clnum) - rex.reg_mmatch->startpos[no].col;
		}
	    }
	    অন্যথা
	    {
		s = rex.reg_match->startp[no];
		যদি (rex.reg_match->endp[no] == রিক্ত)
		    s = রিক্ত;
		অন্যথা
		    len = (পূর্ণাঙ্ক)(rex.reg_match->endp[no] - s);
	    }
	    যদি (s != রিক্ত)
	    {
		বাবে (;;)
		{
		    যদি (len == 0)
		    {
			যদি (REG_MULTI)
			{
			    যদি (rex.reg_mmatch->endpos[no].lnum == clnum)
				ভঙ্গ;
			    যদি (copy)
			    {
				যদি (dst + 1 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    ঘূৰাই 0;
				}
				*dst = CAR;
			    }
			    ++dst;
			    s = reg_getline(++clnum);
			    যদি (rex.reg_mmatch->endpos[no].lnum == clnum)
				len = rex.reg_mmatch->endpos[no].col;
			    অন্যথা
				len = (পূর্ণাঙ্ক)reg_getline_len(clnum);
			}
			অন্যথা
			    ভঙ্গ;
		    }
		    অন্যথা যদি (*s == NUL) // we hit NUL.
		    {
			যদি (copy)
			    iemsg(e_damaged_match_string);
			গচ্ছ exit;
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
				    ঘূৰাই 0;
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

			    যদি (func_one != (fptr_T)রিক্ত)
			    {
				func_one(&cc, c);
				func_one = রিক্ত;
			    }
			    অন্যথা যদি (func_all != (fptr_T)রিক্ত)
				func_all(&cc, c);
			    অন্যথা // just copy
				cc = c;

			    যদি (has_mbyte)
			    {
				পূর্ণাঙ্ক l;
				পূর্ণাঙ্ক charlen;

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
					ঘূৰাই 0;
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
				    ঘূৰাই 0;
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
    ঘূৰাই (পূর্ণাঙ্ক)((dst - dest) + 1);
}

#ifdef FEAT_EVAL

    স্থির char_u *
reg_getline_submatch(linenr_T lnum)
{
    char_u *সাৰি;

    reg_getline_common(lnum, RGLF_সাৰি | RGLF_SUBMATCH, &সাৰি, রিক্ত);

    ঘূৰাই সাৰি;
}

    স্থির colnr_T
reg_getline_submatch_len(linenr_T lnum)
{
    colnr_T length;

    reg_getline_common(lnum, RGLF_LENGTH | RGLF_SUBMATCH, রিক্ত, &length);

    ঘূৰাই length;
}

/*
 * Used for the submatch() function: get the string from the n'th submatch in
 * allocated memory.
 * Returns NULL when not in a ":s" command and for a non-existing submatch.
 */
    char_u *
reg_submatch(পূর্ণাঙ্ক no)
{
    char_u	*retval = রিক্ত;
    char_u	*s;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		round;
    linenr_T	lnum;

    যদি (!can_f_submatch || no < 0)
	ঘূৰাই রিক্ত;

    যদি (rsm.sm_match == রিক্ত)
    {
	/*
	 * First round: compute the length and allocate memory.
	 * Second round: copy the text.
	 */
	বাবে (round = 1; round <= 2; ++round)
	{
	    lnum = rsm.sm_mmatch->startpos[no].lnum;
	    যদি (lnum < 0 || rsm.sm_mmatch->endpos[no].lnum < 0)
		ঘূৰাই রিক্ত;

	    s = reg_getline_submatch(lnum);
	    যদি (s == রিক্ত)  // anti-crash check, cannot happen?
		ভঙ্গ;
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
		len = (পূর্ণাঙ্ক)reg_getline_submatch_len(lnum) - rsm.sm_mmatch->startpos[no].col;
		যদি (round == 2)
		{
		    STRCPY(retval, s);
		    retval[len] = '\n';
		}
		++len;
		++lnum;
		যেতিয়ালৈ (lnum < rsm.sm_mmatch->endpos[no].lnum)
		{
		    s = reg_getline_submatch(lnum);
		    যদি (round == 2)
			STRCPY(retval + len, s);
		    len += (পূর্ণাঙ্ক)reg_getline_submatch_len(lnum);
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

	    যদি (retval == রিক্ত)
	    {
		retval = alloc(len);
		যদি (retval == রিক্ত)
		    ঘূৰাই রিক্ত;
	    }
	}
    }
    অন্যথা
    {
	s = rsm.sm_match->startp[no];
	যদি (s == রিক্ত || rsm.sm_match->endp[no] == রিক্ত)
	    retval = রিক্ত;
	অন্যথা
	    retval = vim_strnsave(s, rsm.sm_match->endp[no] - s);
    }

    ঘূৰাই retval;
}

/*
 * Used for the submatch() function with the optional non-zero argument: get
 * the list of strings from the n'th submatch in allocated memory with NULs
 * represented in NLs.
 * Returns a list of allocated strings.  Returns NULL when not in a ":s"
 * command, for a non-existing submatch and for any error.
 */
    list_T *
reg_submatch_list(পূর্ণাঙ্ক no)
{
    char_u	*s;
    linenr_T	slnum;
    linenr_T	elnum;
    colnr_T	scol;
    colnr_T	ecol;
    পূর্ণাঙ্ক		i;
    list_T	*list;
    পূর্ণাঙ্ক		error = FALSE;

    যদি (!can_f_submatch || no < 0)
	ঘূৰাই রিক্ত;

    যদি (rsm.sm_match == রিক্ত)
    {
	slnum = rsm.sm_mmatch->startpos[no].lnum;
	elnum = rsm.sm_mmatch->endpos[no].lnum;
	যদি (slnum < 0 || elnum < 0)
	    ঘূৰাই রিক্ত;

	scol = rsm.sm_mmatch->startpos[no].col;
	ecol = rsm.sm_mmatch->endpos[no].col;

	list = list_alloc();
	যদি (list == রিক্ত)
	    ঘূৰাই রিক্ত;

	s = reg_getline_submatch(slnum) + scol;
	যদি (slnum == elnum)
	{
	    যদি (list_append_string(list, s, ecol - scol) == FAIL)
		error = TRUE;
	}
	অন্যথা
	{
	    পূর্ণাঙ্ক max_lnum = elnum - slnum;

	    যদি (list_append_string(list, s, -1) == FAIL)
		error = TRUE;
	    বাবে (i = 1; i < max_lnum; i++)
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
	যদি (s == রিক্ত || rsm.sm_match->endp[no] == রিক্ত)
	    ঘূৰাই রিক্ত;
	list = list_alloc();
	যদি (list == রিক্ত)
	    ঘূৰাই রিক্ত;
	যদি (list_append_string(list, s,
				 (পূর্ণাঙ্ক)(rsm.sm_match->endp[no] - s)) == FAIL)
	    error = TRUE;
    }

    যদি (error)
    {
	list_free(list);
	ঘূৰাই রিক্ত;
    }
    ++list->lv_refcount;
    ঘূৰাই list;
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
    rex.reg_match = রিক্ত;
    rex.reg_mmatch = rmp;
    rex.reg_buf = buf;
    rex.reg_win = win;
    rex.reg_firstlnum = lnum;
    rex.reg_maxline = rex.reg_buf->b_ml.ml_সাৰি_গুনতি - lnum;
    rex.reg_সাৰি_lbr = FALSE;
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
    স্থির পূর্ণাঙ্ক
get_char_class_bits(char_u *start, char_u *end, পূর্ণাঙ্ক *newl)
{
    char_u	*p = start;
    পূর্ণাঙ্ক		config = 0;

    *newl = FALSE;
    যদি (*end != ']')
	ঘূৰাই -1;
    যদি (*p == '^')
    {
	config |= CLASS_not;
	p++;
    }

    যেতিয়ালৈ (p < end)
    {
	যদি (p + 2 < end && *(p + 1) == '-')
	{
	    ৱিকল্প (*p)
	    {
		পক্ষ '0':
		    যদি (*(p + 2) == '9')
		    {
			config |= CLASS_o9;
			ভঙ্গ;
		    }
		    যদি (*(p + 2) == '7')
		    {
			config |= CLASS_o7;
			ভঙ্গ;
		    }
		    ঘূৰাই -1;

		পক্ষ 'a':
		    যদি (*(p + 2) == 'z')
		    {
			config |= CLASS_az;
			ভঙ্গ;
		    }
		    যদি (*(p + 2) == 'f')
		    {
			config |= CLASS_af;
			ভঙ্গ;
		    }
		    ঘূৰাই -1;

		পক্ষ 'A':
		    যদি (*(p + 2) == 'Z')
		    {
			config |= CLASS_AZ;
			ভঙ্গ;
		    }
		    যদি (*(p + 2) == 'F')
		    {
			config |= CLASS_AF;
			ভঙ্গ;
		    }
		    ঘূৰাই -1;

		স্ৱভাৱ:
		    ঘূৰাই -1;
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
	    ঘূৰাই -1;
    } // while (p < end)

    যদি (p != end)
	ঘূৰাই -1;

    // U+017F and U+212A fold to ASCII 's' and 'k', missed by the class opcodes
    যদি (enc_utf8 && (config & (CLASS_az | CLASS_AZ)))
	ঘূৰাই -1;

    ঘূৰাই config;
}

#অন্তর্ভাৱ "regexp_bt.c"

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

#অন্তর্ভাৱ "regexp_nfa.c"

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
স্থির পূর্ণাঙ্ক regexp_engine = 0;

#ifdef DEBUG
স্থির ধ্রুৱ char_u regname[][30] = {
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
vim_regcomp(char_u *ৰাশিমালা_arg, পূর্ণাঙ্ক re_flags)
{
    regprog_T   *prog = রিক্ত;
    char_u	*ৰাশিমালা = ৰাশিমালা_arg;
    পূর্ণাঙ্ক		called_emsg_before;

    regexp_engine = p_re;

    // Check for prefix "\%#=", that sets the regexp engine
    যদি (STRNCMP(ৰাশিমালা, "\\%#=", 4) == 0)
    {
	পূর্ণাঙ্ক newengine = ৰাশিমালা[4] - '0';

	যদি (newengine == AUTOMATIC_ENGINE
	    || newengine == BACKTRACKING_ENGINE
	    || newengine == NFA_ENGINE)
	{
	    regexp_engine = ৰাশিমালা[4] - '0';
	    ৰাশিমালা += 5;
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
    bt_regengine.ৰাশিমালা = ৰাশিমালা;
    nfa_regengine.ৰাশিমালা = ৰাশিমালা;
#endif
    // reg_iswordc() uses rex.reg_buf
    rex.reg_buf = curbuf;

    /*
     * First try the NFA engine, unless backtracking was requested.
     */
    called_emsg_before = called_emsg;
    যদি (regexp_engine != BACKTRACKING_ENGINE)
	prog = nfa_regengine.regcomp(ৰাশিমালা,
		re_flags + (regexp_engine == AUTOMATIC_ENGINE ? RE_AUTO : 0));
    অন্যথা
	prog = bt_regengine.regcomp(ৰাশিমালা, re_flags);

    // Check for error compiling regexp with initial engine.
    যদি (prog == রিক্ত)
    {
#ifdef BT_REGEXP_DEBUG_LOG
	যদি (regexp_engine == BACKTRACKING_ENGINE)   // debugging log for BT engine
	{
	    FILE *f;
	    f = fopen(BT_REGEXP_DEBUG_LOG_NAME, "a");
	    যদি (f)
	    {
		fprintf(f, "Syntax error in \"%s\"\n", ৰাশিমালা);
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
	    report_re_switch(ৰাশিমালা);
#endif
	    prog = bt_regengine.regcomp(ৰাশিমালা, re_flags);
	}
    }

    যদি (prog != রিক্ত)
    {
	// Store the info needed to call regcomp() again when the engine turns
	// out to be very slow when executing it.
	prog->re_engine = regexp_engine;
	prog->re_flags  = re_flags;
    }

    ঘূৰাই prog;
}

/*
 * Free a compiled regexp program, returned by vim_regcomp().
 */
    শূন্য
vim_regfree(regprog_T *prog)
{
    যদি (prog != রিক্ত)
	prog->engine->regfree(prog);
}

#যদি defined(EXITFREE)
    শূন্য
free_regexp_stuff(শূন্য)
{
    ga_clear(&regstack);
    ga_clear(&backpos);
    vim_free(reg_tofree);
    vim_free(reg_prev_sub);
    vim_free(post_start);   // NFA postfix buffer, reused across compilations
    post_start = রিক্ত;
    post_start_len = 0;
    vim_free(nfa_স্তূপ);    // NFA fragment stack, reused across compilations
    nfa_স্তূপ = রিক্ত;
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
	বাৰ্তা_puts(_("Switching to backtracking RE engine for pattern: "));
	বাৰ্তা_puts((ৱর্ণ *)pat);
	verbose_leave();
    }
}
#endif

#যদি defined(FEAT_X11)
/*
 * Return whether "prog" is currently being executed.
 */
    পূর্ণাঙ্ক
regprog_in_use(regprog_T *prog)
{
    ঘূৰাই prog->re_in_use;
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
    স্থির পূর্ণাঙ্ক
vim_regexec_string(
    regmatch_T	*rmp,
    char_u	*সাৰি,  // string to match against
    colnr_T	col,    // column to start looking for match
    পূর্ণাঙ্ক		nl)
{
    পূর্ণাঙ্ক		result;
    regexec_T	rex_save;
    পূর্ণাঙ্ক		rex_in_use_save = rex_in_use;

    // Cannot use the same prog recursively, it contains state.
    যদি (rmp->regprog->re_in_use)
    {
	emsg(_(e_cannot_use_pattern_recursively));
	ঘূৰাই FALSE;
    }
    rmp->regprog->re_in_use = TRUE;

    যদি (rex_in_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_in_use = TRUE;

    rex.reg_startp = রিক্ত;
    rex.reg_endp = রিক্ত;
    rex.reg_startpos = রিক্ত;
    rex.reg_endpos = রিক্ত;

    result = rmp->regprog->engine->regexec_nl(rmp, সাৰি, col, nl);
    rmp->regprog->re_in_use = FALSE;

    // NFA engine aborted because it's very slow.
    যদি (rmp->regprog->re_engine == AUTOMATIC_ENGINE
					       && result == NFA_TOO_EXPENSIVE)
    {
	পূর্ণাঙ্ক    save_p_re = p_re;
	পূর্ণাঙ্ক    re_flags = rmp->regprog->re_flags;
	char_u *pat = vim_strsave(((nfa_regprog_T *)rmp->regprog)->pattern);

	p_re = BACKTRACKING_ENGINE;
	যদি (pat != রিক্ত)
	{
	    regprog_T *prev_prog = rmp->regprog;

#ifdef FEAT_EVAL
	    report_re_switch(pat);
#endif
	    rmp->regprog = vim_regcomp(pat, re_flags);
	    যদি (rmp->regprog == রিক্ত)
	    {
		// Somehow compiling the pattern failed now, put back the
		// previous one to avoid "regprog" becoming NULL.
		rmp->regprog = prev_prog;
	    }
	    অন্যথা
	    {
		vim_regfree(prev_prog);
		rmp->regprog->re_in_use = TRUE;
		result = rmp->regprog->engine->regexec_nl(rmp, সাৰি, col, nl);
		rmp->regprog->re_in_use = FALSE;
	    }
	    vim_free(pat);
	}

	p_re = save_p_re;
    }

    rex_in_use = rex_in_use_save;
    যদি (rex_in_use)
	rex = rex_save;

    ঘূৰাই result > 0;
}

#যদি defined(FEAT_SPELL) || defined(FEAT_EVAL) || defined(FEAT_X11)
/*
 * Note: "*prog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    পূর্ণাঙ্ক
vim_regexec_prog(
    regprog_T	**prog,
    পূর্ণাঙ্ক		ignore_case,
    char_u	*সাৰি,
    colnr_T	col)
{
    পূর্ণাঙ্ক		r;
    regmatch_T	regmatch;

    regmatch.regprog = *prog;
    regmatch.rm_ic = ignore_case;
    r = vim_regexec_string(&regmatch, সাৰি, col, FALSE);
    *prog = regmatch.regprog;
    ঘূৰাই r;
}
#endif

/*
 * Note: "rmp->regprog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    পূর্ণাঙ্ক
vim_regexec(regmatch_T *rmp, char_u *সাৰি, colnr_T col)
{
    ঘূৰাই vim_regexec_string(rmp, সাৰি, col, FALSE);
}

/*
 * Like vim_regexec(), but consider a "\n" in "line" to be a line break.
 * Note: "rmp->regprog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    পূর্ণাঙ্ক
vim_regexec_nl(regmatch_T *rmp, char_u *সাৰি, colnr_T col)
{
    ঘূৰাই vim_regexec_string(rmp, সাৰি, col, TRUE);
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
    পূর্ণাঙ্ক		*timed_out)	// flag is set when timeout limit reached
{
    পূর্ণাঙ্ক		result;
    regexec_T	rex_save;
    পূর্ণাঙ্ক		rex_in_use_save = rex_in_use;

    // Cannot use the same prog recursively, it contains state.
    যদি (rmp->regprog->re_in_use)
    {
	emsg(_(e_cannot_use_pattern_recursively));
	ঘূৰাই FALSE;
    }
    rmp->regprog->re_in_use = TRUE;

    যদি (rex_in_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_in_use = TRUE;

    result = rmp->regprog->engine->regexec_multi(
				      rmp, win, buf, lnum, col, timed_out);
    rmp->regprog->re_in_use = FALSE;

    // NFA engine aborted because it's very slow.
    যদি (rmp->regprog->re_engine == AUTOMATIC_ENGINE
					       && result == NFA_TOO_EXPENSIVE)
    {
	পূর্ণাঙ্ক    save_p_re = p_re;
	পূর্ণাঙ্ক    re_flags = rmp->regprog->re_flags;
	char_u *pat = vim_strsave(((nfa_regprog_T *)rmp->regprog)->pattern);

	p_re = BACKTRACKING_ENGINE;
	যদি (pat != রিক্ত)
	{
	    regprog_T *prev_prog = rmp->regprog;

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
	    যদি (rmp->regprog == রিক্ত)
	    {
		// Somehow compiling the pattern failed now, put back the
		// previous one to avoid "regprog" becoming NULL.
		rmp->regprog = prev_prog;
	    }
	    অন্যথা
	    {
		vim_regfree(prev_prog);

		rmp->regprog->re_in_use = TRUE;
		result = rmp->regprog->engine->regexec_multi(
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

    ঘূৰাই result <= 0 ? 0 : result;
}
