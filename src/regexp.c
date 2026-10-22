/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * Handling of regular expressions: vim_regcomp(), vim_regexec(), vim_regsub()
 */

// By default: do not create debugging logs or files related to regular
// expressions, even when compiling with -DDEBUG.
// Uncomment the second line to get the regexp debugging.
#undef DEBUG
// #define DEBUG

#അന്തര്ഭാവ "vim.h"

#ifdef DEBUG
// show/save debugging data when BT engine is used
# define BT_REGEXP_DUMP
// save the debugging data to a file instead of displaying it
# define BT_REGEXP_LOG
# define BT_REGEXP_DEBUG_LOG
# define BT_REGEXP_DEBUG_LOG_NAME	"bt_regexp_debug.log"
#endif

#ifdef FEAT_RELTIME
സ്ഥിതം sig_atomic_t dummy_timeout_flag = 0;
സ്ഥിതം അസ്ഥിര sig_atomic_t *timeout_flag = &dummy_timeout_flag;
#endif

/*
 * Magic characters have a special meaning, they don't match literally.
 * Magic characters are negative.  This separates them from literal characters
 * (possibly multi-byte).  Only ASCII characters can be Magic.
 */
#define Magic(x)	((പൂർണ്ണസംഖ്യ)(x) - 256)
#define un_Magic(x)	((x) + 256)
#define is_Magic(x)	((x) < 0)

    സ്ഥിതം പൂർണ്ണസംഖ്യ
no_Magic(പൂർണ്ണസംഖ്യ x)
{
    എങ്കിൽ (is_Magic(x))
	തിരികെ un_Magic(x);
    തിരികെ x;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
toggle_Magic(പൂർണ്ണസംഖ്യ x)
{
    എങ്കിൽ (is_Magic(x))
	തിരികെ un_Magic(x);
    തിരികെ Magic(x);
}

#എങ്കിൽ defined(FEAT_RELTIME)
സ്ഥിതം പൂർണ്ണസംഖ്യ timeout_nesting = 0;

/*
 * Start a timer that will cause the regexp to abort after "msec".
 * This doesn't work well recursively.  In case it happens anyway, the first
 * set timeout will prevail, nested ones are ignored.
 * The caller must make sure there is a matching disable_regexp_timeout() call!
 */
    ശൂന്യം
init_regexp_timeout(long msec)
{
    എങ്കിൽ (timeout_nesting == 0)
	timeout_flag = start_timeout(msec);
    ++timeout_nesting;
}

    ശൂന്യം
disable_regexp_timeout(ശൂന്യം)
{
    എങ്കിൽ (timeout_nesting == 0)
	iemsg("disable_regexp_timeout() called without active timer");
    അല്ലെങ്കിൽ എങ്കിൽ (--timeout_nesting == 0)
    {
	stop_timeout();
	timeout_flag = &dummy_timeout_flag;
    }
}
#endif

#എങ്കിൽ defined(FEAT_EVAL)
# ifdef FEAT_RELTIME
സ്ഥിതം sig_atomic_t *saved_timeout_flag;
# endif

/*
 * Used at the debug prompt: disable the timeout so that expression evaluation
 * can used patterns.
 * Must be followed by calling restore_timeout_for_debugging().
 */
    ശൂന്യം
save_timeout_for_debugging(ശൂന്യം)
{
# ifdef FEAT_RELTIME
    saved_timeout_flag = (sig_atomic_t *)timeout_flag;
    timeout_flag = &dummy_timeout_flag;
# endif
}

    ശൂന്യം
restore_timeout_for_debugging(ശൂന്യം)
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
#define UCHARAT(p)	((പൂർണ്ണസംഖ്യ)*(char_u *)(p))

// Used for an error (down from) vim_regcomp(): give the error message, set
// rc_did_emsg and return NULL
#define EMSG_RET_NULL(m) തിരികെ (emsg((m)), rc_did_emsg = TRUE, (ശൂന്യം *)ശൂന്യ)
#define IEMSG_RET_NULL(m) തിരികെ (iemsg((m)), rc_did_emsg = TRUE, (ശൂന്യം *)ശൂന്യ)
#define EMSG_RET_FAIL(m) തിരികെ (emsg((m)), rc_did_emsg = TRUE, FAIL)
#define EMSG2_RET_NULL(m, c) തിരികെ (semsg((സ്ഥിരം അക്ഷരം *)(m), (c) ? "" : "\\"), rc_did_emsg = TRUE, (ശൂന്യം *)ശൂന്യ)
#define EMSG3_RET_NULL(m, c, a) തിരികെ (semsg((സ്ഥിരം അക്ഷരം *)(m), (c) ? "" : "\\", (a)), rc_did_emsg = TRUE, (ശൂന്യം *)ശൂന്യ)
#define EMSG2_RET_FAIL(m, c) തിരികെ (semsg((സ്ഥിരം അക്ഷരം *)(m), (c) ? "" : "\\"), rc_did_emsg = TRUE, FAIL)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
re_multi_type(പൂർണ്ണസംഖ്യ c)
{
    എങ്കിൽ (c == Magic('@') || c == Magic('=') || c == Magic('?'))
	തിരികെ MULTI_ONE;
    എങ്കിൽ (c == Magic('*') || c == Magic('+') || c == Magic('{'))
	തിരികെ MULTI_MULT;
    തിരികെ NOT_MULTI;
}

സ്ഥിതം char_u		*reg_prev_sub = ശൂന്യ;
സ്ഥിതം size_t		reg_prev_sublen = 0;

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
സ്ഥിതം char_u REGEXP_INRANGE[] = "]^-n\\";
സ്ഥിതം char_u REGEXP_ABBR[] = "nrtebdoxuU";

/*
 * Translate '\x' to its control character, except "\n", which is Magic.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
backslash_trans(പൂർണ്ണസംഖ്യ c)
{
    തിരഞ്ഞെടുപ്പ് (c)
    {
	അവസ്ഥ 'r':   തിരികെ CAR;
	അവസ്ഥ 't':   തിരികെ TAB;
	അവസ്ഥ 'e':   തിരികെ ESC;
	അവസ്ഥ 'b':   തിരികെ BS;
    }
    തിരികെ c;
}

എണ്ണൽ
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_char_class(char_u **pp)
{
    // must be sorted by the 'value' field because it is used by bsearch()!
    സ്ഥിതം keyvalue_T char_class_tab[] =
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
    എങ്കിൽ ((*pp)[1] == ':' && ASCII_ISLOWER((*pp)[2])
			&& ASCII_ISLOWER((*pp)[3]) && ASCII_ISLOWER((*pp)[4]))
    {
	keyvalue_T target;
	keyvalue_T *entry;
	// this function can be called repeatedly with the same value for "pp"
	// so we cache the last found entry.
	സ്ഥിതം keyvalue_T *last_entry = ശൂന്യ;

	target.താക്കോൽ = 0;
	target.മൂല്യം.string = *pp + 2;
	target.മൂല്യം.length = 0;	// not used, see cmp_keyvalue_value_n()

	എങ്കിൽ (last_entry != ശൂന്യ && cmp_keyvalue_മൂല്യം_n(&target, last_entry) == 0)
	    entry = last_entry;
	അല്ലെങ്കിൽ
	    entry = (keyvalue_T *)bsearch(&target, &char_class_tab,
					ARRAY_LENGTH(char_class_tab),
					വലുപ്പം(char_class_tab[0]), cmp_keyvalue_മൂല്യം_n);
	എങ്കിൽ (entry != ശൂന്യ)
	{
	    last_entry = entry;
	    *pp += entry->മൂല്യം.length + 2;
	    തിരികെ entry->താക്കോൽ;
	}
    }
    തിരികെ CLASS_NONE;
}

/*
 * Specific version of character class functions.
 * Using a table to keep this fast.
 */
സ്ഥിതം short	class_tab[256];

#define	    RI_DIGIT	0x01
#define	    RI_HEX	0x02
#define	    RI_OCTAL	0x04
#define	    RI_WORD	0x08
#define	    RI_HEAD	0x10
#define	    RI_ALPHA	0x20
#define	    RI_LOWER	0x40
#define	    RI_UPPER	0x80
#define	    RI_WHITE	0x100

    സ്ഥിതം ശൂന്യം
init_class_tab(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		i;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	done = FALSE;

    എങ്കിൽ (done)
	തിരികെ;

    വേണ്ടി (i = 0; i < 256; ++i)
    {
	എങ്കിൽ (i >= '0' && i <= '7')
	    class_tab[i] = RI_DIGIT + RI_HEX + RI_OCTAL + RI_WORD;
	അല്ലെങ്കിൽ എങ്കിൽ (i >= '8' && i <= '9')
	    class_tab[i] = RI_DIGIT + RI_HEX + RI_WORD;
	അല്ലെങ്കിൽ എങ്കിൽ (i >= 'a' && i <= 'f')
	    class_tab[i] = RI_HEX + RI_WORD + RI_HEAD + RI_ALPHA + RI_LOWER;
	അല്ലെങ്കിൽ എങ്കിൽ (i >= 'g' && i <= 'z')
	    class_tab[i] = RI_WORD + RI_HEAD + RI_ALPHA + RI_LOWER;
	അല്ലെങ്കിൽ എങ്കിൽ (i >= 'A' && i <= 'F')
	    class_tab[i] = RI_HEX + RI_WORD + RI_HEAD + RI_ALPHA + RI_UPPER;
	അല്ലെങ്കിൽ എങ്കിൽ (i >= 'G' && i <= 'Z')
	    class_tab[i] = RI_WORD + RI_HEAD + RI_ALPHA + RI_UPPER;
	അല്ലെങ്കിൽ എങ്കിൽ (i == '_')
	    class_tab[i] = RI_WORD + RI_HEAD;
	അല്ലെങ്കിൽ
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

സ്ഥിതം char_u	*regparse;	// Input-scan pointer.
സ്ഥിതം പൂർണ്ണസംഖ്യ	regnpar;	// () count.
സ്ഥിതം പൂർണ്ണസംഖ്യ	wants_nfa;	// regex should use NFA engine
#ifdef FEAT_SYN_HL
സ്ഥിതം പൂർണ്ണസംഖ്യ	regnzpar;	// \z() count.
സ്ഥിതം പൂർണ്ണസംഖ്യ	re_has_z;	// \z item detected
#endif
സ്ഥിതം unsigned	regflags;	// RF_ flags for prog
#എങ്കിൽ defined(FEAT_SYN_HL)
സ്ഥിതം പൂർണ്ണസംഖ്യ	had_eol;	// TRUE when EOL found by vim_regcomp()
#endif

സ്ഥിതം magic_T	reg_magic;	// magicness of the pattern

സ്ഥിതം പൂർണ്ണസംഖ്യ	reg_string;	// matching with a string instead of a buffer
				//വരി
സ്ഥിതം പൂർണ്ണസംഖ്യ	reg_strict;	// "[abc" is illegal

/*
 * META contains all characters that may be magic, except '^' and '$'.
 */

// META[] is used often enough to justify turning it into a table.
സ്ഥിതം സ്ഥിരം char_u META_flags[] = {
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

സ്ഥിതം പൂർണ്ണസംഖ്യ	curchr;		// currently parsed character
// Previous character.  Note: prevchr is sometimes -1 when we are not at the
// start, eg in /[ ^I]^ the pattern was never found even if it existed,
// because ^ was taken to be magic -- webb
സ്ഥിതം പൂർണ്ണസംഖ്യ	prevchr;
സ്ഥിതം പൂർണ്ണസംഖ്യ	prevprevchr;	// previous-previous character
സ്ഥിതം പൂർണ്ണസംഖ്യ	nextchr;	// used for ungetchr()

// arguments for reg()
#define REG_NOPAREN	0	// toplevel reg()
#define REG_PAREN	1	// \(\)
#define REG_ZPAREN	2	// \z(\)
#define REG_NPAREN	3	// \%(\)

// Limit recursive parsing of nested regexp atoms to avoid using up the C stack.
#define REG_MAX_PAREN_DEPTH	1000

തരനിർവചനം ഘടന
{
    char_u	*regparse;
    പൂർണ്ണസംഖ്യ		prevchr_len;
    പൂർണ്ണസംഖ്യ		curchr;
    പൂർണ്ണസംഖ്യ		prevchr;
    പൂർണ്ണസംഖ്യ		prevprevchr;
    പൂർണ്ണസംഖ്യ		nextchr;
    പൂർണ്ണസംഖ്യ		at_start;
    പൂർണ്ണസംഖ്യ		prev_at_start;
    പൂർണ്ണസംഖ്യ		regnpar;
} parse_state_T;

സ്ഥിതം ശൂന്യം	initchr(char_u *);
സ്ഥിതം പൂർണ്ണസംഖ്യ	getchr(ശൂന്യം);
സ്ഥിതം ശൂന്യം	skipchr_keepstart(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ	peekchr(ശൂന്യം);
സ്ഥിതം ശൂന്യം	skipchr(ശൂന്യം);
സ്ഥിതം ശൂന്യം	ungetchr(ശൂന്യം);
സ്ഥിതം vimlong_T	gethexchrs(പൂർണ്ണസംഖ്യ maxinputlen);
സ്ഥിതം long	getoctchrs(ശൂന്യം);
സ്ഥിതം vimlong_T	getdecchrs(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ	coll_get_char(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ	prog_magic_wrong(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ	cstrncmp(char_u *s1, char_u *s2, പൂർണ്ണസംഖ്യ *n);
സ്ഥിതം char_u	*cstrchr(char_u *, പൂർണ്ണസംഖ്യ);
സ്ഥിതം പൂർണ്ണസംഖ്യ	re_mult_next(അക്ഷരം *what);
സ്ഥിതം പൂർണ്ണസംഖ്യ	reg_iswordc(പൂർണ്ണസംഖ്യ);
#ifdef FEAT_EVAL
സ്ഥിതം ശൂന്യം report_re_switch(char_u *pat);
#endif

സ്ഥിതം regengine_T bt_regengine;
സ്ഥിതം regengine_T nfa_regengine;

/*
 * Return TRUE if compiled regular expression "prog" can match a line break.
 */
    പൂർണ്ണസംഖ്യ
re_multiline(regprog_T *prog)
{
    തിരികെ (prog->regflags & RF_HASNL);
}

/*
 * Check for an equivalence class name "[=a=]".  "pp" points to the '['.
 * Returns a character representing the class. Zero means that no item was
 * recognized.  Otherwise "pp" is advanced to after the item.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_equi_class(char_u **pp)
{
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		l = 1;
    char_u	*p = *pp;

    എങ്കിൽ (p[1] == '=' && p[2] != NUL)
    {
	എങ്കിൽ (has_mbyte)
	    l = (*mb_ptr2len)(p + 2);
	എങ്കിൽ (p[l + 2] == '=' && p[l + 3] == ']')
	{
	    എങ്കിൽ (has_mbyte)
		c = mb_ptr2char(p + 2);
	    അല്ലെങ്കിൽ
		c = p[2];
	    *pp += l + 4;
	    തിരികെ c;
	}
    }
    തിരികെ 0;
}

/*
 * Check for a collating element "[.a.]".  "pp" points to the '['.
 * Returns a character. Zero means that no item was recognized.  Otherwise
 * "pp" is advanced to after the item.
 * Currently only single characters are recognized!
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_coll_element(char_u **pp)
{
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		l = 1;
    char_u	*p = *pp;

    എങ്കിൽ (p[0] != NUL && p[1] == '.' && p[2] != NUL)
    {
	എങ്കിൽ (has_mbyte)
	    l = (*mb_ptr2len)(p + 2);
	എങ്കിൽ (p[l + 2] == '.' && p[l + 3] == ']')
	{
	    എങ്കിൽ (has_mbyte)
		c = mb_ptr2char(p + 2);
	    അല്ലെങ്കിൽ
		c = p[2];
	    *pp += l + 4;
	    തിരികെ c;
	}
    }
    തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ reg_cpo_lit; // 'cpoptions' contains 'l' flag
സ്ഥിതം പൂർണ്ണസംഖ്യ reg_cpo_bsl; // 'cpoptions' contains '\' flag

    സ്ഥിതം ശൂന്യം
get_cpo_flags(ശൂന്യം)
{
    reg_cpo_lit = vim_strchr(p_cpo, CPO_LITERAL) != ശൂന്യ;
    reg_cpo_bsl = vim_strchr(p_cpo, CPO_BACKSL) != ശൂന്യ;
}

/*
 * Skip over a "[]" range.
 * "p" must point to the character after the '['.
 * The returned pointer is on the matching ']', or the terminating NUL.
 */
    സ്ഥിതം char_u *
skip_anyof(char_u *p)
{
    പൂർണ്ണസംഖ്യ		l;

    എങ്കിൽ (*p == '^')	// Complement of range.
	++p;
    എങ്കിൽ (*p == ']' || *p == '-')
	++p;
    ആകുമ്പോൾ (*p != NUL && *p != ']')
    {
	എങ്കിൽ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	    p += l;
	അല്ലെങ്കിൽ
	    എങ്കിൽ (*p == '-')
	    {
		++p;
		എങ്കിൽ (*p != ']' && *p != NUL)
		    MB_PTR_ADV(p);
	    }
	അല്ലെങ്കിൽ എങ്കിൽ (*p == '\\'
		&& !reg_cpo_bsl
		&& (vim_strchr(REGEXP_INRANGE, p[1]) != ശൂന്യ
		    || (!reg_cpo_lit && vim_strchr(REGEXP_ABBR, p[1]) != ശൂന്യ)))
	    p += 2;
	അല്ലെങ്കിൽ എങ്കിൽ (*p == '[')
	{
	    എങ്കിൽ (get_char_class(&p) == CLASS_NONE
		    && get_equi_class(&p) == 0
		    && get_coll_element(&p) == 0
		    && *p != NUL)
		++p; // it is not a class name and not NUL
	}
	അല്ലെങ്കിൽ
	    ++p;
    }

    തിരികെ p;
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
    പൂർണ്ണസംഖ്യ		delim,
    പൂർണ്ണസംഖ്യ		magic)
{
    തിരികെ skip_regexp_ex(startp, delim, magic, ശൂന്യ, ശൂന്യ, ശൂന്യ);
}

/*
 * Call skip_regexp() and when the delimiter does not match give an error and
 * return NULL.
 */
    char_u *
skip_regexp_err(
    char_u	*startp,
    പൂർണ്ണസംഖ്യ		delim,
    പൂർണ്ണസംഖ്യ		magic)
{
    char_u *p = skip_regexp(startp, delim, magic);

    എങ്കിൽ (*p != delim)
    {
	semsg(_(e_missing_delimiter_after_തിരയൽ_pattern_str), startp);
	തിരികെ ശൂന്യ;
    }
    തിരികെ p;
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
    പൂർണ്ണസംഖ്യ		dirc,
    പൂർണ്ണസംഖ്യ		magic,
    char_u	**newp,
    പൂർണ്ണസംഖ്യ		*dropped,
    magic_T	*magic_val)
{
    magic_T	mymagic;
    char_u	*p = startp;
    size_t	startplen = 0;

    എങ്കിൽ (magic)
	mymagic = MAGIC_ON;
    അല്ലെങ്കിൽ
	mymagic = MAGIC_OFF;
    get_cpo_flags();

    വേണ്ടി (; p[0] != NUL; MB_PTR_ADV(p))
    {
	എങ്കിൽ (p[0] == dirc)	// found end of regexp
	    നിർത്തുക;
	എങ്കിൽ ((p[0] == '[' && mymagic >= MAGIC_ON)
		|| (p[0] == '\\' && p[1] == '[' && mymagic <= MAGIC_OFF))
	{
	    p = skip_anyof(p + 1);
	    എങ്കിൽ (p[0] == NUL)
		നിർത്തുക;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (p[0] == '\\' && p[1] != NUL)
	{
	    എങ്കിൽ (dirc == '?' && newp != ശൂന്യ && p[1] == '?')
	    {
		// change "\?" to "?", make a copy first.
		എങ്കിൽ (startplen == 0)
		    startplen = STRLEN(startp);
		എങ്കിൽ (*newp == ശൂന്യ)
		{
		    *newp = vim_strnsave(startp, startplen);
		    എങ്കിൽ (*newp != ശൂന്യ)
		    {
			p = *newp + (p - startp);
			startp = *newp;
		    }
		}
		എങ്കിൽ (dropped != ശൂന്യ)
		    ++*dropped;
		എങ്കിൽ (*newp != ശൂന്യ)
		    mch_memmove(p, p + 1, startplen - ((p + 1) - startp) + 1);
		അല്ലെങ്കിൽ
		    ++p;
	    }
	    അല്ലെങ്കിൽ
		++p;    // skip next character
	    എങ്കിൽ (*p == 'v')
		mymagic = MAGIC_ALL;
	    അല്ലെങ്കിൽ എങ്കിൽ (*p == 'V')
		mymagic = MAGIC_NONE;
	}
    }
    എങ്കിൽ (magic_val != ശൂന്യ)
	*magic_val = mymagic;
    തിരികെ p;
}

/*
 * Functions for getting characters from the regexp input.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ	prevchr_len;	// byte length of previous char
സ്ഥിതം പൂർണ്ണസംഖ്യ	at_start;	// True when on the first character
സ്ഥിതം പൂർണ്ണസംഖ്യ	prev_at_start;  // True when on the second character

/*
 * Start parsing at "str".
 */
    സ്ഥിതം ശൂന്യം
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
    സ്ഥിതം ശൂന്യം
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
    സ്ഥിതം ശൂന്യം
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
peekchr(ശൂന്യം)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	after_slash = FALSE;

    എങ്കിൽ (curchr != -1)
	തിരികെ curchr;

    തിരഞ്ഞെടുപ്പ് (curchr = regparse[0])
    {
	അവസ്ഥ '.':
	അവസ്ഥ '[':
	അവസ്ഥ '~':
	    // magic when 'magic' is on
	    എങ്കിൽ (reg_magic >= MAGIC_ON)
		curchr = Magic(curchr);
	    നിർത്തുക;
	അവസ്ഥ '(':
	അവസ്ഥ ')':
	അവസ്ഥ '{':
	അവസ്ഥ '%':
	അവസ്ഥ '+':
	അവസ്ഥ '=':
	അവസ്ഥ '?':
	അവസ്ഥ '@':
	അവസ്ഥ '!':
	അവസ്ഥ '&':
	അവസ്ഥ '|':
	അവസ്ഥ '<':
	അവസ്ഥ '>':
	അവസ്ഥ '#':	// future ext.
	അവസ്ഥ '"':	// future ext.
	അവസ്ഥ '\'':	// future ext.
	അവസ്ഥ ',':	// future ext.
	അവസ്ഥ '-':	// future ext.
	അവസ്ഥ ':':	// future ext.
	അവസ്ഥ ';':	// future ext.
	അവസ്ഥ '`':	// future ext.
	അവസ്ഥ '/':	// Can't be used in / command
			// magic only after "\v"
	    എങ്കിൽ (reg_magic == MAGIC_ALL)
		curchr = Magic(curchr);
	    നിർത്തുക;
	അവസ്ഥ '*':
	    // * is not magic as the very first character, eg "?*ptr", when
	    // after '^', eg "/^*ptr" and when after "\(", "\|", "\&".  But
	    // "\(\*" is not magic, thus must be magic if "after_slash"
	    എങ്കിൽ (reg_magic >= MAGIC_ON
		    && !at_start
		    && !(prev_at_start && prevchr == Magic('^'))
		    && (after_slash
			|| (prevchr != Magic('(')
			    && prevchr != Magic('&')
			    && prevchr != Magic('|'))))
		curchr = Magic('*');
	    നിർത്തുക;
	അവസ്ഥ '^':
	    // '^' is only magic as the very first character and if it's after
	    // "\(", "\|", "\&' or "\n"
	    എങ്കിൽ (reg_magic >= MAGIC_OFF
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
	    നിർത്തുക;
	അവസ്ഥ '$':
	    // '$' is only magic as the very last char and if it's in front of
	    // either "\|", "\)", "\&", or "\n"
	    എങ്കിൽ (reg_magic >= MAGIC_OFF)
	    {
		char_u *p = regparse + 1;
		പൂർണ്ണസംഖ്യ is_magic_all = (reg_magic == MAGIC_ALL);

		// ignore \c \C \m \M \v \V and \Z after '$'
		ആകുമ്പോൾ (p[0] == '\\' && (p[1] == 'c' || p[1] == 'C'
			    || p[1] == 'm' || p[1] == 'M'
			    || p[1] == 'v' || p[1] == 'V' || p[1] == 'Z'))
		{
		    എങ്കിൽ (p[1] == 'v')
			is_magic_all = TRUE;
		    അല്ലെങ്കിൽ എങ്കിൽ (p[1] == 'm' || p[1] == 'M' || p[1] == 'V')
			is_magic_all = FALSE;
		    p += 2;
		}
		എങ്കിൽ (p[0] == NUL
			|| (p[0] == '\\'
			    && (p[1] == '|' || p[1] == '&' || p[1] == ')'
				|| p[1] == 'n'))
			|| (is_magic_all
			    && (p[0] == '|' || p[0] == '&' || p[0] == ')'))
			|| reg_magic == MAGIC_ALL)
		    curchr = Magic('$');
	    }
	    നിർത്തുക;
	അവസ്ഥ '\\':
	    {
		പൂർണ്ണസംഖ്യ c = regparse[1];

		എങ്കിൽ (c == NUL)
		    curchr = '\\';	// trailing '\'
		അല്ലെങ്കിൽ എങ്കിൽ (c <= '~' && META_flags[c])
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
		അല്ലെങ്കിൽ എങ്കിൽ (vim_strchr(REGEXP_ABBR, c))
		{
		    /*
		     * Handle abbreviations, like "\t" for TAB -- webb
		     */
		    curchr = backslash_trans(c);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (reg_magic == MAGIC_NONE && (c == '$' || c == '^'))
		    curchr = toggle_Magic(c);
		അല്ലെങ്കിൽ
		{
		    /*
		     * Next character can never be (made) magic?
		     * Then backslashing it won't do anything.
		     */
		    എങ്കിൽ (has_mbyte && c >= 0x80)
			curchr = (*mb_ptr2char)(regparse + 1);
		    അല്ലെങ്കിൽ
			curchr = c;
		}
		നിർത്തുക;
	    }

	സ്ഥിരസ്ഥിതി:
	    // curchr already holds regparse[0]; only a multi-byte lead byte
	    // needs decoding.
	    എങ്കിൽ (has_mbyte && curchr >= 0x80)
		curchr = (*mb_ptr2char)(regparse);
    }

    തിരികെ curchr;
}

/*
 * Eat one lexed character.  Do this in a way that we can undo it.
 */
    സ്ഥിതം ശൂന്യം
skipchr(ശൂന്യം)
{
    // peekchr() eats a backslash, do the same here
    എങ്കിൽ (*regparse == '\\')
	prevchr_len = 1;
    അല്ലെങ്കിൽ
	prevchr_len = 0;
    എങ്കിൽ (regparse[prevchr_len] != NUL)
    {
	എങ്കിൽ (enc_utf8)
	{
	    // Exclude composing chars that mb_ptr2len does include.  A byte
	    // below 0x80 is always a single character.
	    എങ്കിൽ (regparse[prevchr_len] < 0x80)
		++prevchr_len;
	    അല്ലെങ്കിൽ
		prevchr_len += utf_ptr2len(regparse + prevchr_len);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte)
	    prevchr_len += (*mb_ptr2len)(regparse + prevchr_len);
	അല്ലെങ്കിൽ
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
    സ്ഥിതം ശൂന്യം
skipchr_keepstart(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ as = prev_at_start;
    പൂർണ്ണസംഖ്യ pr = prevchr;
    പൂർണ്ണസംഖ്യ prpr = prevprevchr;

    skipchr();
    at_start = as;
    prevchr = pr;
    prevprevchr = prpr;
}

/*
 * Get the next character from the pattern. We know about magic and such, so
 * therefore we need a lexical analyzer.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
getchr(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ chr = peekchr();

    skipchr();
    തിരികെ chr;
}

/*
 * put character back.  Works only once!
 */
    സ്ഥിതം ശൂന്യം
ungetchr(ശൂന്യം)
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
    സ്ഥിതം vimlong_T
gethexchrs(പൂർണ്ണസംഖ്യ maxinputlen)
{
    long_u	nr = 0;
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		i;

    വേണ്ടി (i = 0; i < maxinputlen; ++i)
    {
	c = regparse[0];
	എങ്കിൽ (!vim_isxdigit(c))
	    നിർത്തുക;
	nr <<= 4;
	nr |= hex2nr(c);
	++regparse;
    }

    എങ്കിൽ (i == 0)
	തിരികെ -1;
    തിരികെ nr;
}

/*
 * Get and return the value of the decimal string immediately after the
 * current position. Return -1 for invalid.  Consumes all digits.
 */
    സ്ഥിതം vimlong_T
getdecchrs(ശൂന്യം)
{
    long_u	nr = 0;
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		i;

    വേണ്ടി (i = 0; ; ++i)
    {
	c = regparse[0];
	എങ്കിൽ (c < '0' || c > '9')
	    നിർത്തുക;
	nr *= 10;
	nr += c - '0';
	++regparse;
	curchr = -1; // no longer valid
    }

    എങ്കിൽ (i == 0)
	തിരികെ -1;
    തിരികെ nr;
}

/*
 * get and return the value of the octal string immediately after the current
 * position. Return -1 for invalid, or 0-255 for valid. Smart enough to handle
 * numbers > 377 correctly (for example, 400 is treated as 40) and doesn't
 * treat 8 or 9 as recognised characters. Position is updated:
 *     blahblah\%o210asdf
 *	   before-^  ^-after
 */
    സ്ഥിതം long
getoctchrs(ശൂന്യം)
{
    long_u	nr = 0;
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		i;

    വേണ്ടി (i = 0; i < 3 && nr < 040; ++i)
    {
	c = regparse[0];
	എങ്കിൽ (c < '0' || c > '7')
	    നിർത്തുക;
	nr <<= 3;
	nr |= hex2nr(c);
	++regparse;
    }

    എങ്കിൽ (i == 0)
	തിരികെ -1;
    തിരികെ (long)nr;
}

/*
 * read_limits - Read two integers to be taken as a minimum and maximum.
 * If the first character is '-', then the range is reversed.
 * Should end with 'end'.  If minval is missing, zero is default, if maxval is
 * missing, a very big number is the default.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
read_limits(long *minval, long *maxval)
{
    പൂർണ്ണസംഖ്യ		reverse = FALSE;
    char_u	*first_char;
    long	tmp;

    എങ്കിൽ (*regparse == '-')
    {
	// Starts with '-', so reverse the range later
	regparse++;
	reverse = TRUE;
    }
    first_char = regparse;
    *minval = getdigits(&regparse);
    എങ്കിൽ (*regparse == ',')	    // There is a comma
    {
	എങ്കിൽ (vim_isdigit(*++regparse))
	    *maxval = getdigits(&regparse);
	അല്ലെങ്കിൽ
	    *maxval = MAX_LIMIT;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (VIM_ISDIGIT(*first_char))
	*maxval = *minval;	    // It was \{n} or \{-n}
    അല്ലെങ്കിൽ
	*maxval = MAX_LIMIT;	    // It was \{} or \{-}
    എങ്കിൽ (*regparse == '\\')
	regparse++;	// Allow either \{...} or \{...\}
    എങ്കിൽ (*regparse != '}')
	EMSG2_RET_FAIL(_(e_syntax_error_in_str_curlies),
						       reg_magic == MAGIC_ALL);

    /*
     * Reverse the range if there was a '-', or make sure it is in the right
     * order otherwise.
     */
    എങ്കിൽ ((!reverse && *minval > *maxval) || (reverse && *minval < *maxval))
    {
	tmp = *minval;
	*minval = *maxval;
	*maxval = tmp;
    }
    skipchr();		// let's be friends with the lexer again
    തിരികെ OK;
}

/*
 * vim_regexec and friends
 */

/*
 * Global work variables for vim_regexec().
 */

സ്ഥിതം ശൂന്യം	cleanup_subexpr(ശൂന്യം);
#ifdef FEAT_SYN_HL
സ്ഥിതം ശൂന്യം	cleanup_zsubexpr(ശൂന്യം);
#endif
സ്ഥിതം പൂർണ്ണസംഖ്യ	match_with_backref(linenr_T start_lnum, colnr_T start_col, linenr_T end_lnum, colnr_T end_col, പൂർണ്ണസംഖ്യ *bytelen);

/*
 * Sometimes need to save a copy of a line.  Since alloc()/free() is very
 * slow, we keep one allocated piece of memory and only re-allocate it when
 * it's too small.  It's freed in bt_regexec_both() when finished.
 */
സ്ഥിതം char_u	*reg_tofree = ശൂന്യ;
സ്ഥിതം unsigned	reg_tofreelen;

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
തരനിർവചനം ഘടന {
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
    പൂർണ്ണസംഖ്യ			reg_വരി_lbr;	// "\n" in string is line break

    // The current match-position is stord in these variables:
    linenr_T	lnum;		// line number, relative to first line
    char_u	*വരി;		// start of current line
    char_u	*നിവേശം;		// current input, points into "line"

    പൂർണ്ണസംഖ്യ	need_clear_subexpr;	// subexpressions still need to be cleared
#ifdef FEAT_SYN_HL
    പൂർണ്ണസംഖ്യ	need_clear_zsubexpr;	// extmatch subexpressions still need to be
				// cleared
#endif

    // Internal copy of 'ignorecase'.  It is set at each call to vim_regexec().
    // Normally it gets the value of "rm_ic" or "rmm_ic", but when the pattern
    // contains '\c' or '\C' the value is overruled.
    പൂർണ്ണസംഖ്യ			reg_ic;

    // Similar to "reg_ic", but only for 'combining' characters.  Set with \Z
    // flag in the regexp.  Defaults to false, always.
    പൂർണ്ണസംഖ്യ			reg_icombine;

    // Copy of "rmm_maxcol": maximum column to search for a match.  Zero when
    // there is no maximum.
    colnr_T		reg_maxcol;

    // State for the NFA engine regexec.
    പൂർണ്ണസംഖ്യ nfa_has_zend;	    // NFA regexp \ze operator encountered.
    പൂർണ്ണസംഖ്യ nfa_has_backref;    // NFA regexp \1 .. \9 encountered.
    പൂർണ്ണസംഖ്യ nfa_nsubexpr;	    // Number of sub expressions actually being used
			    // during execution. 1 if only the whole match
			    // (subexpr 0) is used.
    // listid is global, so that it increases on recursive calls to
    // nfa_regmatch(), which means we don't have to clear the lastlist field of
    // all the states.
    പൂർണ്ണസംഖ്യ nfa_listid;
    പൂർണ്ണസംഖ്യ nfa_alt_listid;

#ifdef FEAT_SYN_HL
    പൂർണ്ണസംഖ്യ nfa_has_zsubexpr;   // NFA regexp has \z( ), set zsubexpr.
#endif
} regexec_T;

സ്ഥിതം regexec_T	rex;
സ്ഥിതം പൂർണ്ണസംഖ്യ		rex_in_use = FALSE;

/*
 * Return TRUE if character 'c' is included in 'iskeyword' option for
 * "reg_buf" buffer.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
reg_iswordc(പൂർണ്ണസംഖ്യ c)
{
    തിരികെ vim_iswordc_buf(c, rex.reg_buf);
}

#ifdef FEAT_EVAL
സ്ഥിതം പൂർണ്ണസംഖ്യ can_f_submatch = FALSE;	// TRUE when submatch() can be used

// This struct is used for reg_submatch(). Needed for when the
// substitution string is an expression that contains a call to substitute()
// and submatch().
തരനിർവചനം ഘടന {
    regmatch_T	*sm_match;
    regmmatch_T	*sm_mmatch;
    linenr_T	sm_firstlnum;
    linenr_T	sm_maxline;
    പൂർണ്ണസംഖ്യ		sm_വരി_lbr;
} regsubmatch_T;

സ്ഥിതം regsubmatch_T rsm;  // can only be used when can_f_submatch is TRUE
#endif

തരനിർവചനം എണ്ണൽ
{
    RGLF_വരി = 0x01,
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
    സ്ഥിതം ശൂന്യം
reg_getline_common(
    linenr_T		lnum,
    reg_getline_flags_T	flags,
    char_u		**വരി,
    colnr_T		*length)
{
    പൂർണ്ണസംഖ്യ get_വരി = flags & RGLF_വരി;
    പൂർണ്ണസംഖ്യ get_length = flags & RGLF_LENGTH;
    linenr_T firstlnum;
    linenr_T maxline;

#ifdef FEAT_EVAL
    എങ്കിൽ (flags & RGLF_SUBMATCH)
    {
	firstlnum = rsm.sm_firstlnum + lnum;
	maxline = rsm.sm_maxline;
    }
    അല്ലെങ്കിൽ
#endif
    {
	firstlnum = rex.reg_firstlnum + lnum;
	maxline = rex.reg_maxline;
    }

    // when looking behind for a match/no-match lnum is negative. but we
    // can't go before line 1.
    എങ്കിൽ (firstlnum < 1)
    {
	എങ്കിൽ (get_വരി)
	    *വരി = ശൂന്യ;
	എങ്കിൽ (get_length)
	    *length = 0;

	തിരികെ;
    }

    എങ്കിൽ (lnum > maxline)
    {
	// must have matched the "\n" in the last line.
	എങ്കിൽ (get_വരി)
	    *വരി = (char_u *)"";
	എങ്കിൽ (get_length)
	    *length = 0;

	തിരികെ;
    }

    എങ്കിൽ (get_വരി)
	*വരി = ml_get_buf(rex.reg_buf, firstlnum, FALSE);
    എങ്കിൽ (get_length)
	*length = ml_get_buf_len(rex.reg_buf, firstlnum);
}

/*
 * Get pointer to the line "lnum", which is relative to "reg_firstlnum".
 */
    സ്ഥിതം char_u *
reg_getline(linenr_T lnum)
{
    char_u *വരി;

    reg_getline_common(lnum, RGLF_വരി, &വരി, ശൂന്യ);

    തിരികെ വരി;
}

/*
 * Get length of line "lnum", which is relative to "reg_firstlnum".
 */
    സ്ഥിതം colnr_T
reg_getline_len(linenr_T lnum)
{
    colnr_T length;

    reg_getline_common(lnum, RGLF_LENGTH, ശൂന്യ, &length);

    തിരികെ length;
}

#ifdef FEAT_SYN_HL
സ്ഥിതം char_u	*reg_startzp[NSUBEXP];	// Workspace to mark beginning
സ്ഥിതം char_u	*reg_endzp[NSUBEXP];	//   and end of \z(...\) matches
സ്ഥിതം lpos_T	reg_startzpos[NSUBEXP];	// idem, beginning pos
സ്ഥിതം lpos_T	reg_endzpos[NSUBEXP];	// idem, end pos
#endif

// TRUE if using multi-line regexp.
#define REG_MULTI	(rex.reg_match == ശൂന്യ)

#ifdef FEAT_SYN_HL
/*
 * Create a new extmatch and mark it as referenced once.
 */
    സ്ഥിതം reg_extmatch_T *
make_extmatch(ശൂന്യം)
{
    reg_extmatch_T	*em;

    em = ALLOC_CLEAR_ONE(reg_extmatch_T);
    എങ്കിൽ (em != ശൂന്യ)
	em->refcnt = 1;
    തിരികെ em;
}

/*
 * Add a reference to an extmatch.
 */
    reg_extmatch_T *
ref_extmatch(reg_extmatch_T *em)
{
    എങ്കിൽ (em != ശൂന്യ)
	em->refcnt++;
    തിരികെ em;
}

/*
 * Remove a reference to an extmatch.  If there are no references left, free
 * the info.
 */
    ശൂന്യം
unref_extmatch(reg_extmatch_T *em)
{
    പൂർണ്ണസംഖ്യ i;

    എങ്കിൽ (em != ശൂന്യ && --em->refcnt <= 0)
    {
	വേണ്ടി (i = 0; i < NSUBEXP; ++i)
	    vim_free(em->matches[i]);
	vim_free(em);
    }
}
#endif

/*
 * Get class of previous character.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
reg_prev_class(ശൂന്യം)
{
    എങ്കിൽ (rex.നിവേശം > rex.വരി)
	തിരികെ mb_get_class_buf(rex.നിവേശം - 1
		       - (*mb_head_off)(rex.വരി, rex.നിവേശം - 1), rex.reg_buf);
    തിരികെ -1;
}

/*
 * Return TRUE if the current rex.input position matches the Visual area.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
reg_match_visual(ശൂന്യം)
{
    pos_T	മുകൾ, bot;
    linenr_T    lnum;
    colnr_T	col;
    win_T	*wp = rex.reg_win == ശൂന്യ ? curwin : rex.reg_win;
    പൂർണ്ണസംഖ്യ		mode;
    colnr_T	start, end;
    colnr_T	start2, end2;
    colnr_T	cols;
    colnr_T	curswant;

    // Check if the buffer is the current buffer and not using a string.
    എങ്കിൽ (rex.reg_buf != curbuf || VIsual.lnum == 0 || !REG_MULTI)
	തിരികെ FALSE;

    എങ്കിൽ (VIsual_active)
    {
	എങ്കിൽ (LT_POS(VIsual, wp->w_cursor))
	{
	    മുകൾ = VIsual;
	    bot = wp->w_cursor;
	}
	അല്ലെങ്കിൽ
	{
	    മുകൾ = wp->w_cursor;
	    bot = VIsual;
	}
	mode = VIsual_mode;
	curswant = wp->w_curswant;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (LT_POS(curbuf->b_visual.vi_start, curbuf->b_visual.vi_end))
	{
	    മുകൾ = curbuf->b_visual.vi_start;
	    bot = curbuf->b_visual.vi_end;
	}
	അല്ലെങ്കിൽ
	{
	    മുകൾ = curbuf->b_visual.vi_end;
	    bot = curbuf->b_visual.vi_start;
	}
	// a substitute command may have removed some lines
	എങ്കിൽ (bot.lnum > curbuf->b_ml.ml_വരി_എണ്ണം)
	    bot.lnum = curbuf->b_ml.ml_വരി_എണ്ണം;
	mode = curbuf->b_visual.vi_mode;
	curswant = curbuf->b_visual.vi_curswant;
    }
    lnum = rex.lnum + rex.reg_firstlnum;
    എങ്കിൽ (lnum < മുകൾ.lnum || lnum > bot.lnum)
	തിരികെ FALSE;

    col = (colnr_T)(rex.നിവേശം - rex.വരി);
    എങ്കിൽ (mode == 'v')
    {
	എങ്കിൽ ((lnum == മുകൾ.lnum && col < മുകൾ.col)
		|| (lnum == bot.lnum && col >= bot.col + (*p_sel != 'e')))
	    തിരികെ FALSE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (mode == Ctrl_V)
    {
	getvvcol(wp, &മുകൾ, &start, ശൂന്യ, &end, 0);
	getvvcol(wp, &bot, &start2, ശൂന്യ, &end2, 0);
	എങ്കിൽ (start2 < start)
	    start = start2;
	എങ്കിൽ (end2 > end)
	    end = end2;
	എങ്കിൽ (മുകൾ.col == MAXCOL || bot.col == MAXCOL || curswant == MAXCOL)
	    end = MAXCOL;

	// getvvcol() flushes rex.line, need to get it again
	rex.വരി = reg_getline(rex.lnum);
	rex.നിവേശം = rex.വരി + col;

	cols = win_linetabsize(wp, rex.reg_firstlnum + rex.lnum, rex.വരി, col);
	എങ്കിൽ (cols < start || cols > end - (*p_sel == 'e'))
	    തിരികെ FALSE;
    }
    തിരികെ TRUE;
}

/*
 * Check the regexp program for its magic number.
 * Return TRUE if it's wrong.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
prog_magic_wrong(ശൂന്യം)
{
    regprog_T	*prog;

    prog = REG_MULTI ? rex.reg_mmatch->regprog : rex.reg_match->regprog;
    എങ്കിൽ (prog->engine == &nfa_regengine)
	// For NFA matcher we don't check the magic
	തിരികെ FALSE;

    എങ്കിൽ (UCHARAT(((bt_regprog_T *)prog)->program) != REGMAGIC)
    {
	iemsg(e_corrupted_regexp_program);
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Cleanup the subexpressions, if this wasn't done yet.
 * This construction is used to clear the subexpressions only when they are
 * used (to increase speed).
 */
    സ്ഥിതം ശൂന്യം
cleanup_subexpr(ശൂന്യം)
{
    എങ്കിൽ (!rex.need_clear_subexpr)
	തിരികെ;

    എങ്കിൽ (REG_MULTI)
    {
	// Use 0xff to set lnum to -1
	vim_memset(rex.reg_startpos, 0xff, വലുപ്പം(lpos_T) * NSUBEXP);
	vim_memset(rex.reg_endpos, 0xff, വലുപ്പം(lpos_T) * NSUBEXP);
    }
    അല്ലെങ്കിൽ
    {
	vim_memset(rex.reg_startp, 0, വലുപ്പം(char_u *) * NSUBEXP);
	vim_memset(rex.reg_endp, 0, വലുപ്പം(char_u *) * NSUBEXP);
    }
    rex.need_clear_subexpr = FALSE;
}

#ifdef FEAT_SYN_HL
    സ്ഥിതം ശൂന്യം
cleanup_zsubexpr(ശൂന്യം)
{
    എങ്കിൽ (!rex.need_clear_zsubexpr)
	തിരികെ;

    എങ്കിൽ (REG_MULTI)
    {
	// Use 0xff to set lnum to -1
	vim_memset(reg_startzpos, 0xff, വലുപ്പം(lpos_T) * NSUBEXP);
	vim_memset(reg_endzpos, 0xff, വലുപ്പം(lpos_T) * NSUBEXP);
    }
    അല്ലെങ്കിൽ
    {
	vim_memset(reg_startzp, 0, വലുപ്പം(char_u *) * NSUBEXP);
	vim_memset(reg_endzp, 0, വലുപ്പം(char_u *) * NSUBEXP);
    }
    rex.need_clear_zsubexpr = FALSE;
}
#endif

/*
 * Advance rex.lnum, rex.line and rex.input to the next line.
 */
    സ്ഥിതം ശൂന്യം
reg_nextline(ശൂന്യം)
{
    rex.വരി = reg_getline(++rex.lnum);
    rex.നിവേശം = rex.വരി;
    fast_breakcheck();
}

/*
 * Check whether a backreference matches.
 * Returns RA_FAIL, RA_NOMATCH or RA_MATCH.
 * If "bytelen" is not NULL, it is set to the byte length of the match in the
 * last line.
 * Optional: ignore case if rex.reg_ic is set.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
match_with_backref(
    linenr_T start_lnum,
    colnr_T  start_col,
    linenr_T end_lnum,
    colnr_T  end_col,
    പൂർണ്ണസംഖ്യ	     *bytelen)
{
    linenr_T	clnum = start_lnum;
    colnr_T	ccol = start_col;
    പൂർണ്ണസംഖ്യ		len;
    char_u	*p;

    എങ്കിൽ (bytelen != ശൂന്യ)
	*bytelen = 0;
    വേണ്ടി (;;)
    {
	// Since getting one line may invalidate the other, need to make copy.
	// Slow!
	എങ്കിൽ (rex.വരി != reg_tofree)
	{
	    len = (പൂർണ്ണസംഖ്യ)STRLEN(rex.വരി);
	    എങ്കിൽ (reg_tofree == ശൂന്യ || len >= (പൂർണ്ണസംഖ്യ)reg_tofreelen)
	    {
		len += 50;	// get some extra
		vim_free(reg_tofree);
		reg_tofree = alloc(len);
		എങ്കിൽ (reg_tofree == ശൂന്യ)
		    തിരികെ RA_FAIL; // out of memory!
		reg_tofreelen = len;
	    }
	    STRCPY(reg_tofree, rex.വരി);
	    rex.നിവേശം = reg_tofree + (rex.നിവേശം - rex.വരി);
	    rex.വരി = reg_tofree;
	}

	// Get the line to compare with.
	p = reg_getline(clnum);
	എങ്കിൽ (clnum == end_lnum)
	    len = end_col - ccol;
	അല്ലെങ്കിൽ
	    len = (പൂർണ്ണസംഖ്യ)reg_getline_len(clnum) - ccol;

	// Use case-insensitive compare if rex.reg_ic is set
	എങ്കിൽ ((!rex.reg_ic && cstrncmp(p + ccol, rex.നിവേശം, &len) != 0)
	    || (rex.reg_ic && MB_STRNICMP(p + ccol, rex.നിവേശം, len) != 0))
	    തിരികെ RA_NOMATCH;  // doesn't match
	എങ്കിൽ (bytelen != ശൂന്യ)
	    *bytelen += len;
	എങ്കിൽ (clnum == end_lnum)
	    നിർത്തുക;		// match and at end!
	എങ്കിൽ (rex.lnum >= rex.reg_maxline)
	    തിരികെ RA_NOMATCH;  // text too short

	// Advance to next line.
	reg_nextline();
	എങ്കിൽ (bytelen != ശൂന്യ)
	    *bytelen = 0;
	++clnum;
	ccol = 0;
	എങ്കിൽ (got_int)
	    തിരികെ RA_FAIL;
    }

    // found a match!  Note that rex.line may now point to a copy of the line,
    // that should not matter.
    തിരികെ RA_MATCH;
}

/*
 * Used in a place where no * or \+ can follow.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
re_mult_next(അക്ഷരം *what)
{
    എങ്കിൽ (re_multi_type(peekchr()) == MULTI_MULT)
    {
	semsg(_(e_nfa_regexp_cannot_repeat_str), what);
	rc_did_emsg = TRUE;
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

തരനിർവചനം ഘടന
{
    പൂർണ്ണസംഖ്യ a, b, c;
} decomp_T;


// 0xfb20 - 0xfb4f
സ്ഥിതം decomp_T decomp_table[0xfb4f-0xfb20+1] =
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

    സ്ഥിതം ശൂന്യം
mb_decompose(പൂർണ്ണസംഖ്യ c, പൂർണ്ണസംഖ്യ *c1, പൂർണ്ണസംഖ്യ *c2, പൂർണ്ണസംഖ്യ *c3)
{
    decomp_T d;

    എങ്കിൽ (c >= 0xfb20 && c <= 0xfb4f)
    {
	d = decomp_table[c - 0xfb20];
	*c1 = d.a;
	*c2 = d.b;
	*c3 = d.c;
    }
    അല്ലെങ്കിൽ
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cstrncmp(char_u *s1, char_u *s2, പൂർണ്ണസംഖ്യ *n)
{
    പൂർണ്ണസംഖ്യ		result;

    എങ്കിൽ (!rex.reg_ic)
	result = STRNCMP(s1, s2, *n);
    അല്ലെങ്കിൽ എങ്കിൽ (enc_utf8)
    {
	char_u *p = s1;
	പൂർണ്ണസംഖ്യ n2 = 0;
	പൂർണ്ണസംഖ്യ n1 = *n;
	// count the number of characters for byte-length of s1
	ആകുമ്പോൾ (n1 > 0 && *p != NUL)
	{
	    n1 -= mb_ptr2len(p);
	    MB_PTR_ADV(p);
	    n2++;
	}
	// count the number of bytes to advance the same number of chars for s2
	p = s2;
	ആകുമ്പോൾ (n2-- > 0 && *p != NUL)
	    MB_PTR_ADV(p);

	n2 = p - s2;

	result = MB_STRNICMP2(s1, s2, *n, n2);
	എങ്കിൽ (result == 0)
	    *n = n2;
    }
    അല്ലെങ്കിൽ
	result = MB_STRNICMP(s1, s2, *n);

    // if it failed and it's utf8 and we want to combineignore:
    എങ്കിൽ (result != 0 && enc_utf8 && rex.reg_icombine)
    {
	char_u	*str1, *str2;
	പൂർണ്ണസംഖ്യ	c1, c2, c11, c12;
	പൂർണ്ണസംഖ്യ	junk;

	// we have to handle the strcmp ourselves, since it is necessary to
	// deal with the composing characters by ignoring them:
	str1 = s1;
	str2 = s2;
	c1 = c2 = 0;
	ആകുമ്പോൾ ((പൂർണ്ണസംഖ്യ)(str1 - s1) < *n)
	{
	    c1 = mb_ptr2char_adv(&str1);
	    c2 = mb_ptr2char_adv(&str2);

	    // Decompose the character if necessary, into 'base' characters.
	    // Currently hard-coded for Hebrew, Arabic to be done...
	    എങ്കിൽ (c1 != c2 && (!rex.reg_ic || utf_fold(c1) != utf_fold(c2)))
	    {
		// decomposition necessary?
		mb_decompose(c1, &c11, &junk, &junk);
		mb_decompose(c2, &c12, &junk, &junk);
		c1 = c11;
		c2 = c12;
		എങ്കിൽ (c11 != c12
			    && (!rex.reg_ic || utf_fold(c11) != utf_fold(c12)))
		    നിർത്തുക;
	    }
	}
	result = c2 - c1;
	എങ്കിൽ (result == 0)
	    *n = (പൂർണ്ണസംഖ്യ)(str2 - s2);
    }

    തിരികെ result;
}

/*
 * cstrchr: This function is used a lot for simple searches, keep it fast!
 */
    സ്ഥിതം char_u *
cstrchr(char_u *s, പൂർണ്ണസംഖ്യ c)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		cc, lc;

    എങ്കിൽ (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	തിരികെ vim_strchr(s, c);

    // tolower() and toupper() can be slow, comparing twice should be a lot
    // faster (esp. when using MS Visual C++!).
    // For UTF-8 need to use folded case.
    എങ്കിൽ (enc_utf8 && c > 0x80)
    {
	cc = utf_fold(c);
	lc = cc;
    }
    അല്ലെങ്കിൽ
	എങ്കിൽ (MB_ISUPPER(c))
	{
	    cc = MB_TOLOWER(c);
	    lc = cc;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (MB_ISLOWER(c))
	{
	    cc = MB_TOUPPER(c);
	    lc = c;
	}
	അല്ലെങ്കിൽ
	    തിരികെ vim_strchr(s, c);

    എങ്കിൽ (has_mbyte)
    {
	വേണ്ടി (p = s; *p != NUL; p += (*mb_ptr2len)(p))
	{
	    പൂർണ്ണസംഖ്യ uc = utf_ptr2char(p);
	    എങ്കിൽ (enc_utf8 && (c > 0x80 || uc > 0x80))
	    {
		// Do not match an illegal byte.  E.g. 0xff matches 0xc3 0xbf,
		// not 0xff.
		// compare with lower case of the character
		എങ്കിൽ ((uc < 0x80 || uc != *p) && utf_fold(uc) == lc)
		    തിരികെ p;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (*p == c || *p == cc)
		തിരികെ p;
	}
    }
    അല്ലെങ്കിൽ
	// Faster version for when there are no multi-byte characters.
	വേണ്ടി (p = s; *p != NUL; ++p)
	    എങ്കിൽ (*p == c || *p == cc)
		തിരികെ p;

    തിരികെ ശൂന്യ;
}

////////////////////////////////////////////////////////////////
//		      regsub stuff			      //
////////////////////////////////////////////////////////////////

തരനിർവചനം ശൂന്യം (*fptr_T)(പൂർണ്ണസംഖ്യ *, പൂർണ്ണസംഖ്യ);

സ്ഥിതം പൂർണ്ണസംഖ്യ vim_regsub_both(char_u *source, typval_T *വ്യഞ്ജകം, char_u *dest, പൂർണ്ണസംഖ്യ destlen, പൂർണ്ണസംഖ്യ flags);

    സ്ഥിതം ശൂന്യം
do_upper(പൂർണ്ണസംഖ്യ *d, പൂർണ്ണസംഖ്യ c)
{
    *d = MB_TOUPPER(c);
}

    സ്ഥിതം ശൂന്യം
do_lower(പൂർണ്ണസംഖ്യ *d, പൂർണ്ണസംഖ്യ c)
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
regtilde(char_u *source, പൂർണ്ണസംഖ്യ magic)
{
    char_u	*newsub = source;
    char_u	*p;
    size_t	newsublen = 0;
    char_u	tilde[3] = {'~', NUL, NUL};
    size_t	tildelen = 1;
    പൂർണ്ണസംഖ്യ		error = FALSE;

    എങ്കിൽ (!magic)
    {
	tilde[0] = '\\';
	tilde[1] = '~';
	tilde[2] = NUL;
	tildelen = 2;
    }

    വേണ്ടി (p = newsub; *p; ++p)
    {
	എങ്കിൽ (STRNCMP(p, tilde, tildelen) == 0)
	{
	    size_t prefixlen = p - newsub;		// not including the tilde
	    char_u *postfix = p + tildelen;
	    size_t postfixlen;
	    size_t tmpsublen;

	    എങ്കിൽ (newsublen == 0)
		newsublen = STRLEN(newsub);
	    newsublen -= tildelen;
	    postfixlen = newsublen - prefixlen;
	    tmpsublen = prefixlen + reg_prev_sublen + postfixlen;

	    എങ്കിൽ (tmpsublen > 0 && reg_prev_sub != ശൂന്യ)
	    {
		char_u *tmpsub;

		// Avoid making the text longer than MAXCOL, it will cause
		// trouble at some point.
		എങ്കിൽ (tmpsublen > MAXCOL)
		{
		    emsg(_(e_resulting_text_too_long));
		    error = TRUE;
		    നിർത്തുക;
		}

		tmpsub = alloc(tmpsublen + 1);
		എങ്കിൽ (tmpsub == ശൂന്യ)
		{
		    emsg(_(e_out_of_memory));
		    error = TRUE;
		    നിർത്തുക;
		}

		// copy prefix
		mch_memmove(tmpsub, newsub, prefixlen);
		// interpret tilde
		mch_memmove(tmpsub + prefixlen, reg_prev_sub, reg_prev_sublen);
		// copy postfix
		STRCPY(tmpsub + prefixlen + reg_prev_sublen, postfix);

		എങ്കിൽ (newsub != source)	// allocated newsub before
		    vim_free(newsub);
		newsub = tmpsub;
		newsublen = tmpsublen;
		p = newsub + prefixlen + reg_prev_sublen;
	    }
	    അല്ലെങ്കിൽ
		mch_memmove(p, postfix, postfixlen + 1);	// remove the tilde (+1 for the NUL)

	    --p;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (*p == '\\' && p[1])		// skip escaped characters
		++p;
	    എങ്കിൽ (has_mbyte)
		p += (*mb_ptr2len)(p) - 1;
	}
    }

    എങ്കിൽ (error)
    {
	എങ്കിൽ (newsub != source)
	    vim_free(newsub);
	തിരികെ source;
    }

    // Store a copy of newsub  in reg_prev_sub.  It is always allocated,
    // because recursive calls may make the returned string invalid.
    // Only store it if there something to store.
    newsublen = p - newsub;
    എങ്കിൽ (newsublen == 0)
	VIM_CLEAR(reg_prev_sub);
    അല്ലെങ്കിൽ
    {
	vim_free(reg_prev_sub);
	reg_prev_sub = vim_strnsave(newsub, newsublen);
    }

    എങ്കിൽ (reg_prev_sub == ശൂന്യ)
	reg_prev_sublen = 0;
    അല്ലെങ്കിൽ
	reg_prev_sublen = newsublen;

    തിരികെ newsub;
}

#ifdef FEAT_EVAL

/*
 * Put the submatches in "argv[argskip]" which is a list passed into
 * call_func() by vim_regsub_both().
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
fill_submatch_list(പൂർണ്ണസംഖ്യ argc UNUSED, typval_T *argv, പൂർണ്ണസംഖ്യ argskip, ufunc_T *fp)
{
    listitem_T	*li;
    പൂർണ്ണസംഖ്യ		i;
    char_u	*s;
    typval_T	*listarg = argv + argskip;

    എങ്കിൽ (!has_varargs(fp) && fp->uf_args.ga_len <= argskip)
	// called function doesn't take a submatches argument
	തിരികെ argskip;

    // Relies on sl_list to be the first item in staticList10_T.
    init_static_list((staticList10_T *)(listarg->vval.v_list));

    // There are always 10 list items in staticList10_T.
    li = listarg->vval.v_list->lv_first;
    വേണ്ടി (i = 0; i < 10; ++i)
    {
	s = rsm.sm_match->startp[i];
	എങ്കിൽ (s == ശൂന്യ || rsm.sm_match->endp[i] == ശൂന്യ)
	    s = ശൂന്യ;
	അല്ലെങ്കിൽ
	    s = vim_strnsave(s, rsm.sm_match->endp[i] - s);
	li->li_tv.v_type = VAR_STRING;
	li->li_tv.vval.v_string = s;
	li = li->li_next;
    }
    തിരികെ argskip + 1;
}

    സ്ഥിതം ശൂന്യം
clear_submatch_list(staticList10_T *sl)
{
    പൂർണ്ണസംഖ്യ i;

    വേണ്ടി (i = 0; i < 10; ++i)
	vim_free(sl->sl_ഇനങ്ങൾ[i].li_tv.vval.v_string);
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
    പൂർണ്ണസംഖ്യ
vim_regsub(
    regmatch_T	*rmp,
    char_u	*source,
    typval_T	*വ്യഞ്ജകം,
    char_u	*dest,
    പൂർണ്ണസംഖ്യ		destlen,
    പൂർണ്ണസംഖ്യ		flags)
{
    പൂർണ്ണസംഖ്യ		result;
    regexec_T	rex_save;
    പൂർണ്ണസംഖ്യ		rex_in_use_save = rex_in_use;

    എങ്കിൽ (rex_in_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_in_use = TRUE;

    rex.reg_match = rmp;
    rex.reg_mmatch = ശൂന്യ;
    rex.reg_maxline = 0;
    rex.reg_buf = curbuf;
    rex.reg_വരി_lbr = TRUE;
    result = vim_regsub_both(source, വ്യഞ്ജകം, dest, destlen, flags);

    rex_in_use = rex_in_use_save;
    എങ്കിൽ (rex_in_use)
	rex = rex_save;

    തിരികെ result;
}

    പൂർണ്ണസംഖ്യ
vim_regsub_multi(
    regmmatch_T	*rmp,
    linenr_T	lnum,
    char_u	*source,
    char_u	*dest,
    പൂർണ്ണസംഖ്യ		destlen,
    പൂർണ്ണസംഖ്യ		flags)
{
    പൂർണ്ണസംഖ്യ		result;
    regexec_T	rex_save;
    പൂർണ്ണസംഖ്യ		rex_in_use_save = rex_in_use;

    എങ്കിൽ (rex_in_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_in_use = TRUE;

    rex.reg_match = ശൂന്യ;
    rex.reg_mmatch = rmp;
    rex.reg_buf = curbuf;	// always works on the current buffer!
    rex.reg_firstlnum = lnum;
    rex.reg_maxline = curbuf->b_ml.ml_വരി_എണ്ണം - lnum;
    rex.reg_വരി_lbr = FALSE;
    result = vim_regsub_both(source, ശൂന്യ, dest, destlen, flags);

    rex_in_use = rex_in_use_save;
    എങ്കിൽ (rex_in_use)
	rex = rex_save;

    തിരികെ result;
}

#എങ്കിൽ defined(FEAT_EVAL)
// When nesting more than a couple levels it's probably a mistake.
# define MAX_REGSUB_NESTING 4
സ്ഥിതം char_u   *eval_result[MAX_REGSUB_NESTING] = {ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ};

# എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
free_resub_eval_result(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ i;

    വേണ്ടി (i = 0; i < MAX_REGSUB_NESTING; ++i)
	VIM_CLEAR(eval_result[i]);
}
# endif
#endif

    സ്ഥിതം പൂർണ്ണസംഖ്യ
vim_regsub_both(
    char_u	*source,
    typval_T	*വ്യഞ്ജകം,
    char_u	*dest,
    പൂർണ്ണസംഖ്യ		destlen,
    പൂർണ്ണസംഖ്യ		flags)
{
    char_u	*src;
    char_u	*dst;
    char_u	*s;
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		cc;
    പൂർണ്ണസംഖ്യ		no = -1;
    fptr_T	func_all = (fptr_T)ശൂന്യ;
    fptr_T	func_one = (fptr_T)ശൂന്യ;
    linenr_T	clnum = 0;	// init for GCC
    പൂർണ്ണസംഖ്യ		len = 0;	// init for GCC
#ifdef FEAT_EVAL
    സ്ഥിതം പൂർണ്ണസംഖ്യ  nesting = 0;
    പൂർണ്ണസംഖ്യ		nested;
#endif
    പൂർണ്ണസംഖ്യ		copy = flags & REGSUB_COPY;

    // Be paranoid...
    എങ്കിൽ ((source == ശൂന്യ && വ്യഞ്ജകം == ശൂന്യ) || dest == ശൂന്യ)
    {
	iemsg(e_null_argument);
	തിരികെ 0;
    }
    എങ്കിൽ (prog_magic_wrong())
	തിരികെ 0;
#ifdef FEAT_EVAL
    എങ്കിൽ (nesting == MAX_REGSUB_NESTING)
    {
	emsg(_(e_substitute_nesting_too_deep));
	തിരികെ 0;
    }
    nested = nesting;
#endif
    src = source;
    dst = dest;

    /*
     * When the substitute part starts with "\=" evaluate it as an expression.
     */
    എങ്കിൽ (വ്യഞ്ജകം != ശൂന്യ || (source[0] == '\\' && source[1] == '='))
    {
#ifdef FEAT_EVAL
	// To make sure that the length doesn't change between checking the
	// length and copying the string, and to speed up things, the
	// resulting string is saved from the call with
	// "flags & REGSUB_COPY" == 0 to the call with
	// "flags & REGSUB_COPY" != 0.
	എങ്കിൽ (copy)
	{
	    എങ്കിൽ (eval_result[nested] != ശൂന്യ)
	    {
		പൂർണ്ണസംഖ്യ eval_len = (പൂർണ്ണസംഖ്യ)STRLEN(eval_result[nested]);

		എങ്കിൽ (eval_len < destlen)
		{
		    STRCPY(dest, eval_result[nested]);
		    dst += eval_len;
		    VIM_CLEAR(eval_result[nested]);
		}
	    }
	}
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ		    prev_can_f_submatch = can_f_submatch;
	    regsubmatch_T   rsm_save;

	    VIM_CLEAR(eval_result[nested]);

	    // The expression may contain substitute(), which calls us
	    // recursively.  Make sure submatch() gets the text from the first
	    // level.
	    എങ്കിൽ (can_f_submatch)
		rsm_save = rsm;
	    can_f_submatch = TRUE;
	    rsm.sm_match = rex.reg_match;
	    rsm.sm_mmatch = rex.reg_mmatch;
	    rsm.sm_firstlnum = rex.reg_firstlnum;
	    rsm.sm_maxline = rex.reg_maxline;
	    rsm.sm_വരി_lbr = rex.reg_വരി_lbr;

	    // Although unlikely, it is possible that the expression invokes a
	    // substitute command (it might fail, but still).  Therefore keep
	    // an array of eval results.
	    ++nesting;

	    എങ്കിൽ (വ്യഞ്ജകം != ശൂന്യ)
	    {
		typval_T	argv[2];
		char_u		buf[NUMBUFLEN];
		typval_T	rettv;
		staticList10_T	matchList;
		funcexe_T	funcexe;

		rettv.v_type = VAR_STRING;
		rettv.vval.v_string = ശൂന്യ;
		argv[0].v_type = VAR_LIST;
		argv[0].vval.v_list = &matchList.sl_list;
		matchList.sl_list.lv_len = 0;
		CLEAR_FIELD(funcexe);
		funcexe.fe_argv_func = fill_submatch_list;
		funcexe.fe_evaluate = TRUE;
		എങ്കിൽ (വ്യഞ്ജകം->v_type == VAR_FUNC)
		{
		    s = വ്യഞ്ജകം->vval.v_string;
		    call_func(s, -1, &rettv, 1, argv, &funcexe);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (വ്യഞ്ജകം->v_type == VAR_PARTIAL)
		{
		    partial_T   *partial = വ്യഞ്ജകം->vval.v_partial;

		    s = partial_name(partial);
		    funcexe.fe_partial = partial;
		    call_func(s, -1, &rettv, 1, argv, &funcexe);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (വ്യഞ്ജകം->v_type == VAR_INSTR)
		{
		    exe_typval_instr(വ്യഞ്ജകം, &rettv);
		}
		എങ്കിൽ (matchList.sl_list.lv_len > 0)
		    // fill_submatch_list() was called
		    clear_submatch_list(&matchList);

		എങ്കിൽ (rettv.v_type == VAR_UNKNOWN)
		    // something failed, no need to report another error
		    eval_result[nested] = ശൂന്യ;
		അല്ലെങ്കിൽ
		{
		    eval_result[nested] = tv_get_string_buf_chk(&rettv, buf);
		    എങ്കിൽ (eval_result[nested] != ശൂന്യ)
			eval_result[nested] = vim_strsave(eval_result[nested]);
		}
		clear_tv(&rettv);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (substitute_instr != ശൂന്യ)
		// Execute instructions from ISN_SUBSTITUTE.
		eval_result[nested] = exe_substitute_instr();
	    അല്ലെങ്കിൽ
		eval_result[nested] = eval_to_string(source + 2, TRUE, FALSE);
	    --nesting;

	    എങ്കിൽ (eval_result[nested] != ശൂന്യ)
	    {
		പൂർണ്ണസംഖ്യ had_backslash = FALSE;

		വേണ്ടി (s = eval_result[nested]; *s != NUL; MB_PTR_ADV(s))
		{
		    // Change NL to CR, so that it becomes a line break,
		    // unless called from vim_regexec_nl().
		    // Skip over a backslashed character.
		    എങ്കിൽ (*s == NL && !rsm.sm_വരി_lbr)
			*s = CAR;
		    അല്ലെങ്കിൽ എങ്കിൽ (*s == '\\' && s[1] != NUL)
		    {
			++s;
			/* Change NL to CR here too, so that this works:
			 * :s/abc\\\ndef/\="aaa\\\nbbb"/  on text:
			 *   abc\
			 *   def
			 * Not when called from vim_regexec_nl().
			 */
			എങ്കിൽ (*s == NL && !rsm.sm_വരി_lbr)
			    *s = CAR;
			had_backslash = TRUE;
		    }
		}
		എങ്കിൽ (had_backslash && (flags & REGSUB_BACKSLASH))
		{
		    // Backslashes will be consumed, need to double them.
		    s = vim_strsave_escaped(eval_result[nested], (char_u *)"\\");
		    എങ്കിൽ (s != ശൂന്യ)
		    {
			vim_free(eval_result[nested]);
			eval_result[nested] = s;
		    }
		}

		dst += STRLEN(eval_result[nested]);
	    }

	    can_f_submatch = prev_can_f_submatch;
	    എങ്കിൽ (can_f_submatch)
		rsm = rsm_save;
	}
#endif
    }
    അല്ലെങ്കിൽ
      ആകുമ്പോൾ ((c = *src++) != NUL)
      {
	എങ്കിൽ (c == '&' && (flags & REGSUB_MAGIC))
	    no = 0;
	അല്ലെങ്കിൽ എങ്കിൽ (c == '\\' && *src != NUL)
	{
	    എങ്കിൽ (*src == '&' && !(flags & REGSUB_MAGIC))
	    {
		++src;
		no = 0;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ ('0' <= *src && *src <= '9')
	    {
		no = *src++ - '0';
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (vim_strchr((char_u *)"uUlLeE", *src))
	    {
		തിരഞ്ഞെടുപ്പ് (*src++)
		{
		അവസ്ഥ 'u':   func_one = do_upper;
			    തുടരുക;
		അവസ്ഥ 'U':   func_all = do_upper;
			    തുടരുക;
		അവസ്ഥ 'l':   func_one = do_lower;
			    തുടരുക;
		അവസ്ഥ 'L':   func_all = do_lower;
			    തുടരുക;
		അവസ്ഥ 'e':
		അവസ്ഥ 'E':   func_one = func_all = (fptr_T)ശൂന്യ;
			    തുടരുക;
		}
	    }
	}
	എങ്കിൽ (no < 0)	      // Ordinary character.
	{
	    എങ്കിൽ (c == K_SPECIAL && src[0] != NUL && src[1] != NUL)
	    {
		// Copy a special key as-is.
		എങ്കിൽ (copy)
		{
		    എങ്കിൽ (dst + 3 > dest + destlen)
		    {
			iemsg("vim_regsub_both(): not enough space");
			തിരികെ 0;
		    }
		    *dst++ = c;
		    *dst++ = *src++;
		    *dst++ = *src++;
		}
		അല്ലെങ്കിൽ
		{
		    dst += 3;
		    src += 2;
		}
		തുടരുക;
	    }

	    എങ്കിൽ (c == '\\' && *src != NUL)
	    {
		// Check for abbreviations -- webb
		തിരഞ്ഞെടുപ്പ് (*src)
		{
		    അവസ്ഥ 'r':	c = CAR;	++src;	നിർത്തുക;
		    അവസ്ഥ 'n':	c = NL;		++src;	നിർത്തുക;
		    അവസ്ഥ 't':	c = TAB;	++src;	നിർത്തുക;
		 // Oh no!  \e already has meaning in subst pat :-(
		 // case 'e':   c = ESC;	++src;	break;
		    അവസ്ഥ 'b':	c = Ctrl_H;	++src;	നിർത്തുക;

		    // If "backslash" is TRUE the backslash will be removed
		    // later.  Used to insert a literal CR.
		    സ്ഥിരസ്ഥിതി:	എങ്കിൽ (flags & REGSUB_BACKSLASH)
				{
				    എങ്കിൽ (copy)
				    {
					എങ്കിൽ (dst + 1 > dest + destlen)
					{
					    iemsg("vim_regsub_both(): not enough space");
					    തിരികെ 0;
					}
					*dst = '\\';
				    }
				    ++dst;
				}
				c = *src++;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte)
		c = mb_ptr2char(src - 1);

	    // Write to buffer, if copy is set.
	    എങ്കിൽ (func_one != (fptr_T)ശൂന്യ)
	    {
		func_one(&cc, c);
		func_one = ശൂന്യ;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (func_all != (fptr_T)ശൂന്യ)
		func_all(&cc, c);
	    അല്ലെങ്കിൽ // just copy
		cc = c;

	    എങ്കിൽ (has_mbyte)
	    {
		പൂർണ്ണസംഖ്യ totlen = mb_ptr2len(src - 1);
		പൂർണ്ണസംഖ്യ charlen = mb_char2len(cc);

		എങ്കിൽ (copy)
		{
		    എങ്കിൽ (dst + charlen > dest + destlen)
		    {
			iemsg("vim_regsub_both(): not enough space");
			തിരികെ 0;
		    }
		    mb_char2bytes(cc, dst);
		}
		dst += charlen - 1;
		എങ്കിൽ (enc_utf8)
		{
		    പൂർണ്ണസംഖ്യ clen = utf_ptr2len(src - 1);

		    // If the character length is shorter than "totlen", there
		    // are composing characters; copy them as-is.
		    എങ്കിൽ (clen < totlen)
		    {
			എങ്കിൽ (copy)
			{
			    എങ്കിൽ (dst + totlen - clen > dest + destlen)
			    {
				iemsg("vim_regsub_both(): not enough space");
				തിരികെ 0;
			    }
			    mch_memmove(dst + 1, src - 1 + clen,
						     (size_t)(totlen - clen));
			}
			dst += totlen - clen;
		    }
		}
		src += totlen - 1;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (copy)
	    {
		എങ്കിൽ (dst + 1 > dest + destlen)
		{
		    iemsg("vim_regsub_both(): not enough space");
		    തിരികെ 0;
		}
		*dst = cc;
	    }
	    dst++;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (REG_MULTI)
	    {
		clnum = rex.reg_mmatch->startpos[no].lnum;
		എങ്കിൽ (clnum < 0 || rex.reg_mmatch->endpos[no].lnum < 0)
		    s = ശൂന്യ;
		അല്ലെങ്കിൽ
		{
		    s = reg_getline(clnum) + rex.reg_mmatch->startpos[no].col;
		    എങ്കിൽ (rex.reg_mmatch->endpos[no].lnum == clnum)
			len = rex.reg_mmatch->endpos[no].col
					    - rex.reg_mmatch->startpos[no].col;
		    അല്ലെങ്കിൽ
			len = (പൂർണ്ണസംഖ്യ)reg_getline_len(clnum) - rex.reg_mmatch->startpos[no].col;
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		s = rex.reg_match->startp[no];
		എങ്കിൽ (rex.reg_match->endp[no] == ശൂന്യ)
		    s = ശൂന്യ;
		അല്ലെങ്കിൽ
		    len = (പൂർണ്ണസംഖ്യ)(rex.reg_match->endp[no] - s);
	    }
	    എങ്കിൽ (s != ശൂന്യ)
	    {
		വേണ്ടി (;;)
		{
		    എങ്കിൽ (len == 0)
		    {
			എങ്കിൽ (REG_MULTI)
			{
			    എങ്കിൽ (rex.reg_mmatch->endpos[no].lnum == clnum)
				നിർത്തുക;
			    എങ്കിൽ (copy)
			    {
				എങ്കിൽ (dst + 1 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    തിരികെ 0;
				}
				*dst = CAR;
			    }
			    ++dst;
			    s = reg_getline(++clnum);
			    എങ്കിൽ (rex.reg_mmatch->endpos[no].lnum == clnum)
				len = rex.reg_mmatch->endpos[no].col;
			    അല്ലെങ്കിൽ
				len = (പൂർണ്ണസംഖ്യ)reg_getline_len(clnum);
			}
			അല്ലെങ്കിൽ
			    നിർത്തുക;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (*s == NUL) // we hit NUL.
		    {
			എങ്കിൽ (copy)
			    iemsg(e_damaged_match_string);
			പോകുക exit;
		    }
		    അല്ലെങ്കിൽ
		    {
			എങ്കിൽ ((flags & REGSUB_BACKSLASH)
						  && (*s == CAR || *s == '\\'))
			{
			    /*
			     * Insert a backslash in front of a CR, otherwise
			     * it will be replaced by a line break.
			     * Number of backslashes will be halved later,
			     * double them here.
			     */
			    എങ്കിൽ (copy)
			    {
				എങ്കിൽ (dst + 2 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    തിരികെ 0;
				}
				dst[0] = '\\';
				dst[1] = *s;
			    }
			    dst += 2;
			}
			അല്ലെങ്കിൽ
			{
			    എങ്കിൽ (has_mbyte)
				c = mb_ptr2char(s);
			    അല്ലെങ്കിൽ
				c = *s;

			    എങ്കിൽ (func_one != (fptr_T)ശൂന്യ)
			    {
				func_one(&cc, c);
				func_one = ശൂന്യ;
			    }
			    അല്ലെങ്കിൽ എങ്കിൽ (func_all != (fptr_T)ശൂന്യ)
				func_all(&cc, c);
			    അല്ലെങ്കിൽ // just copy
				cc = c;

			    എങ്കിൽ (has_mbyte)
			    {
				പൂർണ്ണസംഖ്യ l;
				പൂർണ്ണസംഖ്യ charlen;

				// Copy composing characters separately, one
				// at a time.
				എങ്കിൽ (enc_utf8)
				    l = utf_ptr2len(s) - 1;
				അല്ലെങ്കിൽ
				    l = mb_ptr2len(s) - 1;

				s += l;
				len -= l;
				charlen = mb_char2len(cc);
				എങ്കിൽ (copy)
				{
				    എങ്കിൽ (dst + charlen > dest + destlen)
				    {
					iemsg("vim_regsub_both(): not enough space");
					തിരികെ 0;
				    }
				    mb_char2bytes(cc, dst);
				}
				dst += charlen - 1;
			    }
			    അല്ലെങ്കിൽ എങ്കിൽ (copy)
			    {
				എങ്കിൽ (dst + 1 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    തിരികെ 0;
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
    എങ്കിൽ (copy)
	*dst = NUL;

exit:
    തിരികെ (പൂർണ്ണസംഖ്യ)((dst - dest) + 1);
}

#ifdef FEAT_EVAL

    സ്ഥിതം char_u *
reg_getline_submatch(linenr_T lnum)
{
    char_u *വരി;

    reg_getline_common(lnum, RGLF_വരി | RGLF_SUBMATCH, &വരി, ശൂന്യ);

    തിരികെ വരി;
}

    സ്ഥിതം colnr_T
reg_getline_submatch_len(linenr_T lnum)
{
    colnr_T length;

    reg_getline_common(lnum, RGLF_LENGTH | RGLF_SUBMATCH, ശൂന്യ, &length);

    തിരികെ length;
}

/*
 * Used for the submatch() function: get the string from the n'th submatch in
 * allocated memory.
 * Returns NULL when not in a ":s" command and for a non-existing submatch.
 */
    char_u *
reg_submatch(പൂർണ്ണസംഖ്യ no)
{
    char_u	*retval = ശൂന്യ;
    char_u	*s;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		round;
    linenr_T	lnum;

    എങ്കിൽ (!can_f_submatch || no < 0)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (rsm.sm_match == ശൂന്യ)
    {
	/*
	 * First round: compute the length and allocate memory.
	 * Second round: copy the text.
	 */
	വേണ്ടി (round = 1; round <= 2; ++round)
	{
	    lnum = rsm.sm_mmatch->startpos[no].lnum;
	    എങ്കിൽ (lnum < 0 || rsm.sm_mmatch->endpos[no].lnum < 0)
		തിരികെ ശൂന്യ;

	    s = reg_getline_submatch(lnum);
	    എങ്കിൽ (s == ശൂന്യ)  // anti-crash check, cannot happen?
		നിർത്തുക;
	    s += rsm.sm_mmatch->startpos[no].col;
	    എങ്കിൽ (rsm.sm_mmatch->endpos[no].lnum == lnum)
	    {
		// Within one line: take form start to end col.
		len = rsm.sm_mmatch->endpos[no].col
					  - rsm.sm_mmatch->startpos[no].col;
		എങ്കിൽ (round == 2)
		    vim_strncpy(retval, s, len);
		++len;
	    }
	    അല്ലെങ്കിൽ
	    {
		// Multiple lines: take start line from start col, middle
		// lines completely and end line up to end col.
		len = (പൂർണ്ണസംഖ്യ)reg_getline_submatch_len(lnum) - rsm.sm_mmatch->startpos[no].col;
		എങ്കിൽ (round == 2)
		{
		    STRCPY(retval, s);
		    retval[len] = '\n';
		}
		++len;
		++lnum;
		ആകുമ്പോൾ (lnum < rsm.sm_mmatch->endpos[no].lnum)
		{
		    s = reg_getline_submatch(lnum);
		    എങ്കിൽ (round == 2)
			STRCPY(retval + len, s);
		    len += (പൂർണ്ണസംഖ്യ)reg_getline_submatch_len(lnum);
		    എങ്കിൽ (round == 2)
			retval[len] = '\n';
		    ++len;
		    ++lnum;
		}
		എങ്കിൽ (round == 2)
		    STRNCPY(retval + len, reg_getline_submatch(lnum),
					     rsm.sm_mmatch->endpos[no].col);
		len += rsm.sm_mmatch->endpos[no].col;
		എങ്കിൽ (round == 2)
		    retval[len] = NUL;
		++len;
	    }

	    എങ്കിൽ (retval == ശൂന്യ)
	    {
		retval = alloc(len);
		എങ്കിൽ (retval == ശൂന്യ)
		    തിരികെ ശൂന്യ;
	    }
	}
    }
    അല്ലെങ്കിൽ
    {
	s = rsm.sm_match->startp[no];
	എങ്കിൽ (s == ശൂന്യ || rsm.sm_match->endp[no] == ശൂന്യ)
	    retval = ശൂന്യ;
	അല്ലെങ്കിൽ
	    retval = vim_strnsave(s, rsm.sm_match->endp[no] - s);
    }

    തിരികെ retval;
}

/*
 * Used for the submatch() function with the optional non-zero argument: get
 * the list of strings from the n'th submatch in allocated memory with NULs
 * represented in NLs.
 * Returns a list of allocated strings.  Returns NULL when not in a ":s"
 * command, for a non-existing submatch and for any error.
 */
    list_T *
reg_submatch_list(പൂർണ്ണസംഖ്യ no)
{
    char_u	*s;
    linenr_T	slnum;
    linenr_T	elnum;
    colnr_T	scol;
    colnr_T	ecol;
    പൂർണ്ണസംഖ്യ		i;
    list_T	*list;
    പൂർണ്ണസംഖ്യ		error = FALSE;

    എങ്കിൽ (!can_f_submatch || no < 0)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (rsm.sm_match == ശൂന്യ)
    {
	slnum = rsm.sm_mmatch->startpos[no].lnum;
	elnum = rsm.sm_mmatch->endpos[no].lnum;
	എങ്കിൽ (slnum < 0 || elnum < 0)
	    തിരികെ ശൂന്യ;

	scol = rsm.sm_mmatch->startpos[no].col;
	ecol = rsm.sm_mmatch->endpos[no].col;

	list = list_alloc();
	എങ്കിൽ (list == ശൂന്യ)
	    തിരികെ ശൂന്യ;

	s = reg_getline_submatch(slnum) + scol;
	എങ്കിൽ (slnum == elnum)
	{
	    എങ്കിൽ (list_append_string(list, s, ecol - scol) == FAIL)
		error = TRUE;
	}
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ max_lnum = elnum - slnum;

	    എങ്കിൽ (list_append_string(list, s, -1) == FAIL)
		error = TRUE;
	    വേണ്ടി (i = 1; i < max_lnum; i++)
	    {
		s = reg_getline_submatch(slnum + i);
		എങ്കിൽ (list_append_string(list, s, -1) == FAIL)
		    error = TRUE;
	    }
	    s = reg_getline_submatch(elnum);
	    എങ്കിൽ (list_append_string(list, s, ecol) == FAIL)
		error = TRUE;
	}
    }
    അല്ലെങ്കിൽ
    {
	s = rsm.sm_match->startp[no];
	എങ്കിൽ (s == ശൂന്യ || rsm.sm_match->endp[no] == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	list = list_alloc();
	എങ്കിൽ (list == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	എങ്കിൽ (list_append_string(list, s,
				 (പൂർണ്ണസംഖ്യ)(rsm.sm_match->endp[no] - s)) == FAIL)
	    error = TRUE;
    }

    എങ്കിൽ (error)
    {
	list_free(list);
	തിരികെ ശൂന്യ;
    }
    ++list->lv_refcount;
    തിരികെ list;
}
#endif

/*
 * Initialize the values used for matching against multiple lines
 */
    സ്ഥിതം ശൂന്യം
init_regexec_multi(
	regmmatch_T	*rmp,
	win_T		*win,	// window in which to search or NULL
	buf_T		*buf,	// buffer in which to search
	linenr_T	lnum)	// nr of line to start looking for match
{
    rex.reg_match = ശൂന്യ;
    rex.reg_mmatch = rmp;
    rex.reg_buf = buf;
    rex.reg_win = win;
    rex.reg_firstlnum = lnum;
    rex.reg_maxline = rex.reg_buf->b_ml.ml_വരി_എണ്ണം - lnum;
    rex.reg_വരി_lbr = FALSE;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_char_class_bits(char_u *start, char_u *end, പൂർണ്ണസംഖ്യ *newl)
{
    char_u	*p = start;
    പൂർണ്ണസംഖ്യ		config = 0;

    *newl = FALSE;
    എങ്കിൽ (*end != ']')
	തിരികെ -1;
    എങ്കിൽ (*p == '^')
    {
	config |= CLASS_not;
	p++;
    }

    ആകുമ്പോൾ (p < end)
    {
	എങ്കിൽ (p + 2 < end && *(p + 1) == '-')
	{
	    തിരഞ്ഞെടുപ്പ് (*p)
	    {
		അവസ്ഥ '0':
		    എങ്കിൽ (*(p + 2) == '9')
		    {
			config |= CLASS_o9;
			നിർത്തുക;
		    }
		    എങ്കിൽ (*(p + 2) == '7')
		    {
			config |= CLASS_o7;
			നിർത്തുക;
		    }
		    തിരികെ -1;

		അവസ്ഥ 'a':
		    എങ്കിൽ (*(p + 2) == 'z')
		    {
			config |= CLASS_az;
			നിർത്തുക;
		    }
		    എങ്കിൽ (*(p + 2) == 'f')
		    {
			config |= CLASS_af;
			നിർത്തുക;
		    }
		    തിരികെ -1;

		അവസ്ഥ 'A':
		    എങ്കിൽ (*(p + 2) == 'Z')
		    {
			config |= CLASS_AZ;
			നിർത്തുക;
		    }
		    എങ്കിൽ (*(p + 2) == 'F')
		    {
			config |= CLASS_AF;
			നിർത്തുക;
		    }
		    തിരികെ -1;

		സ്ഥിരസ്ഥിതി:
		    തിരികെ -1;
	    }
	    p += 3;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (p + 1 < end && *p == '\\' && *(p + 1) == 'n')
	{
	    *newl = TRUE;
	    p += 2;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*p == '_')
	{
	    config |= CLASS_underscore;
	    p++;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*p == '\n')
	{
	    *newl = TRUE;
	    p++;
	}
	അല്ലെങ്കിൽ
	    തിരികെ -1;
    } // while (p < end)

    എങ്കിൽ (p != end)
	തിരികെ -1;

    // U+017F and U+212A fold to ASCII 's' and 'k', missed by the class opcodes
    എങ്കിൽ (enc_utf8 && (config & (CLASS_az | CLASS_AZ)))
	തിരികെ -1;

    തിരികെ config;
}

#അന്തര്ഭാവ "regexp_bt.c"

സ്ഥിതം regengine_T bt_regengine =
{
    bt_regcomp,
    bt_regfree,
    bt_regexec_nl,
    bt_regexec_multi
#ifdef DEBUG
    ,(char_u *)""
#endif
};

#അന്തര്ഭാവ "regexp_nfa.c"

സ്ഥിതം regengine_T nfa_regengine =
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
സ്ഥിതം പൂർണ്ണസംഖ്യ regexp_engine = 0;

#ifdef DEBUG
സ്ഥിതം സ്ഥിരം char_u regname[][30] = {
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
vim_regcomp(char_u *വ്യഞ്ജകം_arg, പൂർണ്ണസംഖ്യ re_flags)
{
    regprog_T   *prog = ശൂന്യ;
    char_u	*വ്യഞ്ജകം = വ്യഞ്ജകം_arg;
    പൂർണ്ണസംഖ്യ		called_emsg_before;

    regexp_engine = p_re;

    // Check for prefix "\%#=", that sets the regexp engine
    എങ്കിൽ (STRNCMP(വ്യഞ്ജകം, "\\%#=", 4) == 0)
    {
	പൂർണ്ണസംഖ്യ newengine = വ്യഞ്ജകം[4] - '0';

	എങ്കിൽ (newengine == AUTOMATIC_ENGINE
	    || newengine == BACKTRACKING_ENGINE
	    || newengine == NFA_ENGINE)
	{
	    regexp_engine = വ്യഞ്ജകം[4] - '0';
	    വ്യഞ്ജകം += 5;
#ifdef DEBUG
	    smsg("New regexp mode selected (%d): %s",
					   regexp_engine, regname[newengine]);
#endif
	}
	അല്ലെങ്കിൽ
	{
	    emsg(_(e_percent_hash_can_only_be_followed_by_zero_one_two_automatic_engine_will_be_used));
	    regexp_engine = AUTOMATIC_ENGINE;
	}
    }
#ifdef DEBUG
    bt_regengine.വ്യഞ്ജകം = വ്യഞ്ജകം;
    nfa_regengine.വ്യഞ്ജകം = വ്യഞ്ജകം;
#endif
    // reg_iswordc() uses rex.reg_buf
    rex.reg_buf = curbuf;

    /*
     * First try the NFA engine, unless backtracking was requested.
     */
    called_emsg_before = called_emsg;
    എങ്കിൽ (regexp_engine != BACKTRACKING_ENGINE)
	prog = nfa_regengine.regcomp(വ്യഞ്ജകം,
		re_flags + (regexp_engine == AUTOMATIC_ENGINE ? RE_AUTO : 0));
    അല്ലെങ്കിൽ
	prog = bt_regengine.regcomp(വ്യഞ്ജകം, re_flags);

    // Check for error compiling regexp with initial engine.
    എങ്കിൽ (prog == ശൂന്യ)
    {
#ifdef BT_REGEXP_DEBUG_LOG
	എങ്കിൽ (regexp_engine == BACKTRACKING_ENGINE)   // debugging log for BT engine
	{
	    FILE *f;
	    f = fopen(BT_REGEXP_DEBUG_LOG_NAME, "a");
	    എങ്കിൽ (f)
	    {
		fprintf(f, "Syntax error in \"%s\"\n", വ്യഞ്ജകം);
		fclose(f);
	    }
	    അല്ലെങ്കിൽ
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
	എങ്കിൽ (regexp_engine == AUTOMATIC_ENGINE
					  && called_emsg == called_emsg_before)
	{
	    regexp_engine = BACKTRACKING_ENGINE;
#ifdef FEAT_EVAL
	    report_re_switch(വ്യഞ്ജകം);
#endif
	    prog = bt_regengine.regcomp(വ്യഞ്ജകം, re_flags);
	}
    }

    എങ്കിൽ (prog != ശൂന്യ)
    {
	// Store the info needed to call regcomp() again when the engine turns
	// out to be very slow when executing it.
	prog->re_engine = regexp_engine;
	prog->re_flags  = re_flags;
    }

    തിരികെ prog;
}

/*
 * Free a compiled regexp program, returned by vim_regcomp().
 */
    ശൂന്യം
vim_regfree(regprog_T *prog)
{
    എങ്കിൽ (prog != ശൂന്യ)
	prog->engine->regfree(prog);
}

#എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
free_regexp_stuff(ശൂന്യം)
{
    ga_clear(&regstack);
    ga_clear(&backpos);
    vim_free(reg_tofree);
    vim_free(reg_prev_sub);
    vim_free(post_start);   // NFA postfix buffer, reused across compilations
    post_start = ശൂന്യ;
    post_start_len = 0;
    vim_free(nfa_അട്ടി);    // NFA fragment stack, reused across compilations
    nfa_അട്ടി = ശൂന്യ;
    nfa_അട്ടി_len = 0;
}
#endif

#ifdef FEAT_EVAL
    സ്ഥിതം ശൂന്യം
report_re_switch(char_u *pat)
{
    എങ്കിൽ (p_verbose > 0)
    {
	verbose_enter();
	സന്ദേശം_puts(_("Switching to backtracking RE engine for pattern: "));
	സന്ദേശം_puts((അക്ഷരം *)pat);
	verbose_leave();
    }
}
#endif

#എങ്കിൽ defined(FEAT_X11)
/*
 * Return whether "prog" is currently being executed.
 */
    പൂർണ്ണസംഖ്യ
regprog_in_use(regprog_T *prog)
{
    തിരികെ prog->re_in_use;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
vim_regexec_string(
    regmatch_T	*rmp,
    char_u	*വരി,  // string to match against
    colnr_T	col,    // column to start looking for match
    പൂർണ്ണസംഖ്യ		nl)
{
    പൂർണ്ണസംഖ്യ		result;
    regexec_T	rex_save;
    പൂർണ്ണസംഖ്യ		rex_in_use_save = rex_in_use;

    // Cannot use the same prog recursively, it contains state.
    എങ്കിൽ (rmp->regprog->re_in_use)
    {
	emsg(_(e_cannot_use_pattern_recursively));
	തിരികെ FALSE;
    }
    rmp->regprog->re_in_use = TRUE;

    എങ്കിൽ (rex_in_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_in_use = TRUE;

    rex.reg_startp = ശൂന്യ;
    rex.reg_endp = ശൂന്യ;
    rex.reg_startpos = ശൂന്യ;
    rex.reg_endpos = ശൂന്യ;

    result = rmp->regprog->engine->regexec_nl(rmp, വരി, col, nl);
    rmp->regprog->re_in_use = FALSE;

    // NFA engine aborted because it's very slow.
    എങ്കിൽ (rmp->regprog->re_engine == AUTOMATIC_ENGINE
					       && result == NFA_TOO_EXPENSIVE)
    {
	പൂർണ്ണസംഖ്യ    save_p_re = p_re;
	പൂർണ്ണസംഖ്യ    re_flags = rmp->regprog->re_flags;
	char_u *pat = vim_strsave(((nfa_regprog_T *)rmp->regprog)->pattern);

	p_re = BACKTRACKING_ENGINE;
	എങ്കിൽ (pat != ശൂന്യ)
	{
	    regprog_T *prev_prog = rmp->regprog;

#ifdef FEAT_EVAL
	    report_re_switch(pat);
#endif
	    rmp->regprog = vim_regcomp(pat, re_flags);
	    എങ്കിൽ (rmp->regprog == ശൂന്യ)
	    {
		// Somehow compiling the pattern failed now, put back the
		// previous one to avoid "regprog" becoming NULL.
		rmp->regprog = prev_prog;
	    }
	    അല്ലെങ്കിൽ
	    {
		vim_regfree(prev_prog);
		rmp->regprog->re_in_use = TRUE;
		result = rmp->regprog->engine->regexec_nl(rmp, വരി, col, nl);
		rmp->regprog->re_in_use = FALSE;
	    }
	    vim_free(pat);
	}

	p_re = save_p_re;
    }

    rex_in_use = rex_in_use_save;
    എങ്കിൽ (rex_in_use)
	rex = rex_save;

    തിരികെ result > 0;
}

#എങ്കിൽ defined(FEAT_SPELL) || defined(FEAT_EVAL) || defined(FEAT_X11)
/*
 * Note: "*prog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    പൂർണ്ണസംഖ്യ
vim_regexec_prog(
    regprog_T	**prog,
    പൂർണ്ണസംഖ്യ		ignore_case,
    char_u	*വരി,
    colnr_T	col)
{
    പൂർണ്ണസംഖ്യ		r;
    regmatch_T	regmatch;

    regmatch.regprog = *prog;
    regmatch.rm_ic = ignore_case;
    r = vim_regexec_string(&regmatch, വരി, col, FALSE);
    *prog = regmatch.regprog;
    തിരികെ r;
}
#endif

/*
 * Note: "rmp->regprog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    പൂർണ്ണസംഖ്യ
vim_regexec(regmatch_T *rmp, char_u *വരി, colnr_T col)
{
    തിരികെ vim_regexec_string(rmp, വരി, col, FALSE);
}

/*
 * Like vim_regexec(), but consider a "\n" in "line" to be a line break.
 * Note: "rmp->regprog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    പൂർണ്ണസംഖ്യ
vim_regexec_nl(regmatch_T *rmp, char_u *വരി, colnr_T col)
{
    തിരികെ vim_regexec_string(rmp, വരി, col, TRUE);
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
    പൂർണ്ണസംഖ്യ		*timed_out)	// flag is set when timeout limit reached
{
    പൂർണ്ണസംഖ്യ		result;
    regexec_T	rex_save;
    പൂർണ്ണസംഖ്യ		rex_in_use_save = rex_in_use;

    // Cannot use the same prog recursively, it contains state.
    എങ്കിൽ (rmp->regprog->re_in_use)
    {
	emsg(_(e_cannot_use_pattern_recursively));
	തിരികെ FALSE;
    }
    rmp->regprog->re_in_use = TRUE;

    എങ്കിൽ (rex_in_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_in_use = TRUE;

    result = rmp->regprog->engine->regexec_multi(
				      rmp, win, buf, lnum, col, timed_out);
    rmp->regprog->re_in_use = FALSE;

    // NFA engine aborted because it's very slow.
    എങ്കിൽ (rmp->regprog->re_engine == AUTOMATIC_ENGINE
					       && result == NFA_TOO_EXPENSIVE)
    {
	പൂർണ്ണസംഖ്യ    save_p_re = p_re;
	പൂർണ്ണസംഖ്യ    re_flags = rmp->regprog->re_flags;
	char_u *pat = vim_strsave(((nfa_regprog_T *)rmp->regprog)->pattern);

	p_re = BACKTRACKING_ENGINE;
	എങ്കിൽ (pat != ശൂന്യ)
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
	    എങ്കിൽ (rmp->regprog == ശൂന്യ)
	    {
		// Somehow compiling the pattern failed now, put back the
		// previous one to avoid "regprog" becoming NULL.
		rmp->regprog = prev_prog;
	    }
	    അല്ലെങ്കിൽ
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
    എങ്കിൽ (rex_in_use)
	rex = rex_save;

    തിരികെ result <= 0 ? 0 : result;
}
