/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * Handling of regular expressions: vim_regcomp(), vim_regexec(), vim_regsub()
 */

// By default: do not create debugging logs or files related to regular
// expressions, even when compiling with -DDEBUG.
// Uncomment the second line to get the regexp debugging.
#undef DEBUG
// #define DEBUG

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ifdef DEBUG
// show/save debugging data when BT engine is used
# define BT_REGEXP_DUMP
// save the debugging data to a file instead of displaying it
# define BT_REGEXP_LOG
# define BT_REGEXP_DEBUG_LOG
# define BT_REGEXP_DEBUG_LOG_NAME	"bt_regexp_debug.log"
#endif

#ifdef FEAT_RELTIME
ସ୍ଥିର sig_atomic_t dummy_timeout_flag = 0;
ସ୍ଥିର ଅସ୍ଥିର sig_atomic_t *timeout_flag = &dummy_timeout_flag;
#endif

/*
 * Magic characters have a special meaning, they don't match literally.
 * Magic characters are negative.  This separates them from literal characters
 * (possibly multi-byte).  Only ASCII characters can be Magic.
 */
#define Magic(x)	((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(x) - 256)
#define un_Magic(x)	((x) + 256)
#define is_Magic(x)	((x) < 0)

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
no_Magic(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x)
{
    ଯଦି (is_Magic(x))
	ଫେରନ୍ତୁ un_Magic(x);
    ଫେରନ୍ତୁ x;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
toggle_Magic(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x)
{
    ଯଦି (is_Magic(x))
	ଫେରନ୍ତୁ un_Magic(x);
    ଫେରନ୍ତୁ Magic(x);
}

#ଯଦି defined(FEAT_RELTIME)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା timeout_nesting = 0;

/*
 * Start a timer that will cause the regexp to abort after "msec".
 * This doesn't work well recursively.  In case it happens anyway, the first
 * set timeout will prevail, nested ones are ignored.
 * The caller must make sure there is a matching disable_regexp_timeout() call!
 */
    ଶୂନ୍ୟ
init_regexp_timeout(long msec)
{
    ଯଦି (timeout_nesting == 0)
	timeout_flag = start_timeout(msec);
    ++timeout_nesting;
}

    ଶୂନ୍ୟ
disable_regexp_timeout(ଶୂନ୍ୟ)
{
    ଯଦି (timeout_nesting == 0)
	iemsg("disable_regexp_timeout() called without active timer");
    ଅନ୍ୟଥା ଯଦି (--timeout_nesting == 0)
    {
	stop_timeout();
	timeout_flag = &dummy_timeout_flag;
    }
}
#endif

#ଯଦି defined(FEAT_EVAL)
# ifdef FEAT_RELTIME
ସ୍ଥିର sig_atomic_t *saved_timeout_flag;
# endif

/*
 * Used at the debug prompt: disable the timeout so that expression evaluation
 * can used patterns.
 * Must be followed by calling restore_timeout_for_debugging().
 */
    ଶୂନ୍ୟ
save_timeout_for_debugging(ଶୂନ୍ୟ)
{
# ifdef FEAT_RELTIME
    saved_timeout_flag = (sig_atomic_t *)timeout_flag;
    timeout_flag = &dummy_timeout_flag;
# endif
}

    ଶୂନ୍ୟ
restore_timeout_for_debugging(ଶୂନ୍ୟ)
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
#define UCHARAT(p)	((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)*(char_u *)(p))

// Used for an error (down from) vim_regcomp(): give the error message, set
// rc_did_emsg and return NULL
#define EMSG_RET_NULL(m) ଫେରନ୍ତୁ (emsg((m)), rc_did_emsg = TRUE, (ଶୂନ୍ୟ *)ଖାଲି)
#define IEMSG_RET_NULL(m) ଫେରନ୍ତୁ (iemsg((m)), rc_did_emsg = TRUE, (ଶୂନ୍ୟ *)ଖାଲି)
#define EMSG_RET_FAIL(m) ଫେରନ୍ତୁ (emsg((m)), rc_did_emsg = TRUE, FAIL)
#define EMSG2_RET_NULL(m, c) ଫେରନ୍ତୁ (semsg((ଅଚଳ ଅକ୍ଷର *)(m), (c) ? "" : "\\"), rc_did_emsg = TRUE, (ଶୂନ୍ୟ *)ଖାଲି)
#define EMSG3_RET_NULL(m, c, a) ଫେରନ୍ତୁ (semsg((ଅଚଳ ଅକ୍ଷର *)(m), (c) ? "" : "\\", (a)), rc_did_emsg = TRUE, (ଶୂନ୍ୟ *)ଖାଲି)
#define EMSG2_RET_FAIL(m, c) ଫେରନ୍ତୁ (semsg((ଅଚଳ ଅକ୍ଷର *)(m), (c) ? "" : "\\"), rc_did_emsg = TRUE, FAIL)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
re_multi_type(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (c == Magic('@') || c == Magic('=') || c == Magic('?'))
	ଫେରନ୍ତୁ MULTI_ONE;
    ଯଦି (c == Magic('*') || c == Magic('+') || c == Magic('{'))
	ଫେରନ୍ତୁ MULTI_MULT;
    ଫେରନ୍ତୁ NOT_MULTI;
}

ସ୍ଥିର char_u		*reg_prev_sub = ଖାଲି;
ସ୍ଥିର size_t		reg_prev_sublen = 0;

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
ସ୍ଥିର char_u REGEXP_INRANGE[] = "]^-n\\";
ସ୍ଥିର char_u REGEXP_ABBR[] = "nrtebdoxuU";

/*
 * Translate '\x' to its control character, except "\n", which is Magic.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
backslash_trans(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ବାଛ (c)
    {
	ସ୍ଥିତି 'r':   ଫେରନ୍ତୁ CAR;
	ସ୍ଥିତି 't':   ଫେରନ୍ତୁ TAB;
	ସ୍ଥିତି 'e':   ଫେରନ୍ତୁ ESC;
	ସ୍ଥିତି 'b':   ଫେରନ୍ତୁ BS;
    }
    ଫେରନ୍ତୁ c;
}

ଗଣନା
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_char_class(char_u **pp)
{
    // must be sorted by the 'value' field because it is used by bsearch()!
    ସ୍ଥିର keyvalue_T char_class_tab[] =
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
    ଯଦି ((*pp)[1] == ':' && ASCII_ISLOWER((*pp)[2])
			&& ASCII_ISLOWER((*pp)[3]) && ASCII_ISLOWER((*pp)[4]))
    {
	keyvalue_T target;
	keyvalue_T *entry;
	// this function can be called repeatedly with the same value for "pp"
	// so we cache the last found entry.
	ସ୍ଥିର keyvalue_T *last_entry = ଖାଲି;

	target.ଚାବି = 0;
	target.ମୂଲ୍ୟ.string = *pp + 2;
	target.ମୂଲ୍ୟ.length = 0;	// not used, see cmp_keyvalue_value_n()

	ଯଦି (last_entry != ଖାଲି && cmp_keyvalue_ମୂଲ୍ୟ_n(&target, last_entry) == 0)
	    entry = last_entry;
	ଅନ୍ୟଥା
	    entry = (keyvalue_T *)bsearch(&target, &char_class_tab,
					ARRAY_LENGTH(char_class_tab),
					ଆକାର(char_class_tab[0]), cmp_keyvalue_ମୂଲ୍ୟ_n);
	ଯଦି (entry != ଖାଲି)
	{
	    last_entry = entry;
	    *pp += entry->ମୂଲ୍ୟ.length + 2;
	    ଫେରନ୍ତୁ entry->ଚାବି;
	}
    }
    ଫେରନ୍ତୁ CLASS_NONE;
}

/*
 * Specific version of character class functions.
 * Using a table to keep this fast.
 */
ସ୍ଥିର short	class_tab[256];

#define	    RI_DIGIT	0x01
#define	    RI_HEX	0x02
#define	    RI_OCTAL	0x04
#define	    RI_WORD	0x08
#define	    RI_HEAD	0x10
#define	    RI_ALPHA	0x20
#define	    RI_LOWER	0x40
#define	    RI_UPPER	0x80
#define	    RI_WHITE	0x100

    ସ୍ଥିର ଶୂନ୍ୟ
init_class_tab(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	done = FALSE;

    ଯଦି (done)
	ଫେରନ୍ତୁ;

    ପାଇଁ (i = 0; i < 256; ++i)
    {
	ଯଦି (i >= '0' && i <= '7')
	    class_tab[i] = RI_DIGIT + RI_HEX + RI_OCTAL + RI_WORD;
	ଅନ୍ୟଥା ଯଦି (i >= '8' && i <= '9')
	    class_tab[i] = RI_DIGIT + RI_HEX + RI_WORD;
	ଅନ୍ୟଥା ଯଦି (i >= 'a' && i <= 'f')
	    class_tab[i] = RI_HEX + RI_WORD + RI_HEAD + RI_ALPHA + RI_LOWER;
	ଅନ୍ୟଥା ଯଦି (i >= 'g' && i <= 'z')
	    class_tab[i] = RI_WORD + RI_HEAD + RI_ALPHA + RI_LOWER;
	ଅନ୍ୟଥା ଯଦି (i >= 'A' && i <= 'F')
	    class_tab[i] = RI_HEX + RI_WORD + RI_HEAD + RI_ALPHA + RI_UPPER;
	ଅନ୍ୟଥା ଯଦି (i >= 'G' && i <= 'Z')
	    class_tab[i] = RI_WORD + RI_HEAD + RI_ALPHA + RI_UPPER;
	ଅନ୍ୟଥା ଯଦି (i == '_')
	    class_tab[i] = RI_WORD + RI_HEAD;
	ଅନ୍ୟଥା
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

ସ୍ଥିର char_u	*regparse;	// Input-scan pointer.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	regnpar;	// () count.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	wants_nfa;	// regex should use NFA engine
#ifdef FEAT_SYN_HL
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	regnzpar;	// \z() count.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	re_has_z;	// \z item detected
#endif
ସ୍ଥିର unsigned	regflags;	// RF_ flags for prog
#ଯଦି defined(FEAT_SYN_HL)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	had_eol;	// TRUE when EOL found by vim_regcomp()
#endif

ସ୍ଥିର magic_T	reg_magic;	// magicness of the pattern

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	reg_string;	// matching with a string instead of a buffer
				//ଧାଡ଼ି
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	reg_strict;	// "[abc" is illegal

/*
 * META contains all characters that may be magic, except '^' and '$'.
 */

// META[] is used often enough to justify turning it into a table.
ସ୍ଥିର ଅଚଳ char_u META_flags[] = {
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

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	curchr;		// currently parsed character
// Previous character.  Note: prevchr is sometimes -1 when we are not at the
// start, eg in /[ ^I]^ the pattern was never found even if it existed,
// because ^ was taken to be magic -- webb
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	prevchr;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	prevprevchr;	// previous-previous character
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	nextchr;	// used for ungetchr()

// arguments for reg()
#define REG_NOPAREN	0	// toplevel reg()
#define REG_PAREN	1	// \(\)
#define REG_ZPAREN	2	// \z(\)
#define REG_NPAREN	3	// \%(\)

// Limit recursive parsing of nested regexp atoms to avoid using up the C stack.
#define REG_MAX_PAREN_DEPTH	1000

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    char_u	*regparse;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prevchr_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		curchr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prevchr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prevprevchr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nextchr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		at_start;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_at_start;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		regnpar;
} parse_state_T;

ସ୍ଥିର ଶୂନ୍ୟ	initchr(char_u *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	getchr(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ	skipchr_keepstart(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	peekchr(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ	skipchr(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ	ungetchr(ଶୂନ୍ୟ);
ସ୍ଥିର vimlong_T	gethexchrs(ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxinputlen);
ସ୍ଥିର long	getoctchrs(ଶୂନ୍ୟ);
ସ୍ଥିର vimlong_T	getdecchrs(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	coll_get_char(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	prog_magic_wrong(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cstrncmp(char_u *s1, char_u *s2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *n);
ସ୍ଥିର char_u	*cstrchr(char_u *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	re_mult_next(ଅକ୍ଷର *what);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	reg_iswordc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
#ifdef FEAT_EVAL
ସ୍ଥିର ଶୂନ୍ୟ report_re_switch(char_u *pat);
#endif

ସ୍ଥିର regengine_T bt_regengine;
ସ୍ଥିର regengine_T nfa_regengine;

/*
 * Return TRUE if compiled regular expression "prog" can match a line break.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
re_multiline(regprog_T *prog)
{
    ଫେରନ୍ତୁ (prog->regflags & RF_HASNL);
}

/*
 * Check for an equivalence class name "[=a=]".  "pp" points to the '['.
 * Returns a character representing the class. Zero means that no item was
 * recognized.  Otherwise "pp" is advanced to after the item.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_equi_class(char_u **pp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l = 1;
    char_u	*p = *pp;

    ଯଦି (p[1] == '=' && p[2] != NUL)
    {
	ଯଦି (has_mbyte)
	    l = (*mb_ptr2len)(p + 2);
	ଯଦି (p[l + 2] == '=' && p[l + 3] == ']')
	{
	    ଯଦି (has_mbyte)
		c = mb_ptr2char(p + 2);
	    ଅନ୍ୟଥା
		c = p[2];
	    *pp += l + 4;
	    ଫେରନ୍ତୁ c;
	}
    }
    ଫେରନ୍ତୁ 0;
}

/*
 * Check for a collating element "[.a.]".  "pp" points to the '['.
 * Returns a character. Zero means that no item was recognized.  Otherwise
 * "pp" is advanced to after the item.
 * Currently only single characters are recognized!
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_coll_element(char_u **pp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l = 1;
    char_u	*p = *pp;

    ଯଦି (p[0] != NUL && p[1] == '.' && p[2] != NUL)
    {
	ଯଦି (has_mbyte)
	    l = (*mb_ptr2len)(p + 2);
	ଯଦି (p[l + 2] == '.' && p[l + 3] == ']')
	{
	    ଯଦି (has_mbyte)
		c = mb_ptr2char(p + 2);
	    ଅନ୍ୟଥା
		c = p[2];
	    *pp += l + 4;
	    ଫେରନ୍ତୁ c;
	}
    }
    ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା reg_cpo_lit; // 'cpoptions' contains 'l' flag
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା reg_cpo_bsl; // 'cpoptions' contains '\' flag

    ସ୍ଥିର ଶୂନ୍ୟ
get_cpo_flags(ଶୂନ୍ୟ)
{
    reg_cpo_lit = vim_strchr(p_cpo, CPO_LITERAL) != ଖାଲି;
    reg_cpo_bsl = vim_strchr(p_cpo, CPO_BACKSL) != ଖାଲି;
}

/*
 * Skip over a "[]" range.
 * "p" must point to the character after the '['.
 * The returned pointer is on the matching ']', or the terminating NUL.
 */
    ସ୍ଥିର char_u *
skip_anyof(char_u *p)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;

    ଯଦି (*p == '^')	// Complement of range.
	++p;
    ଯଦି (*p == ']' || *p == '-')
	++p;
    ଯେତେବେଳେ (*p != NUL && *p != ']')
    {
	ଯଦି (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	    p += l;
	ଅନ୍ୟଥା
	    ଯଦି (*p == '-')
	    {
		++p;
		ଯଦି (*p != ']' && *p != NUL)
		    MB_PTR_ADV(p);
	    }
	ଅନ୍ୟଥା ଯଦି (*p == '\\'
		&& !reg_cpo_bsl
		&& (vim_strchr(REGEXP_INRANGE, p[1]) != ଖାଲି
		    || (!reg_cpo_lit && vim_strchr(REGEXP_ABBR, p[1]) != ଖାଲି)))
	    p += 2;
	ଅନ୍ୟଥା ଯଦି (*p == '[')
	{
	    ଯଦି (get_char_class(&p) == CLASS_NONE
		    && get_equi_class(&p) == 0
		    && get_coll_element(&p) == 0
		    && *p != NUL)
		++p; // it is not a class name and not NUL
	}
	ଅନ୍ୟଥା
	    ++p;
    }

    ଫେରନ୍ତୁ p;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		delim,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		magic)
{
    ଫେରନ୍ତୁ skip_regexp_ex(startp, delim, magic, ଖାଲି, ଖାଲି, ଖାଲି);
}

/*
 * Call skip_regexp() and when the delimiter does not match give an error and
 * return NULL.
 */
    char_u *
skip_regexp_err(
    char_u	*startp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		delim,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		magic)
{
    char_u *p = skip_regexp(startp, delim, magic);

    ଯଦି (*p != delim)
    {
	semsg(_(e_missing_delimiter_after_ସନ୍ଧାନ_pattern_str), startp);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ p;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dirc,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		magic,
    char_u	**newp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*dropped,
    magic_T	*magic_val)
{
    magic_T	mymagic;
    char_u	*p = startp;
    size_t	startplen = 0;

    ଯଦି (magic)
	mymagic = MAGIC_ON;
    ଅନ୍ୟଥା
	mymagic = MAGIC_OFF;
    get_cpo_flags();

    ପାଇଁ (; p[0] != NUL; MB_PTR_ADV(p))
    {
	ଯଦି (p[0] == dirc)	// found end of regexp
	    ଭାଙ୍ଗ;
	ଯଦି ((p[0] == '[' && mymagic >= MAGIC_ON)
		|| (p[0] == '\\' && p[1] == '[' && mymagic <= MAGIC_OFF))
	{
	    p = skip_anyof(p + 1);
	    ଯଦି (p[0] == NUL)
		ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା ଯଦି (p[0] == '\\' && p[1] != NUL)
	{
	    ଯଦି (dirc == '?' && newp != ଖାଲି && p[1] == '?')
	    {
		// change "\?" to "?", make a copy first.
		ଯଦି (startplen == 0)
		    startplen = STRLEN(startp);
		ଯଦି (*newp == ଖାଲି)
		{
		    *newp = vim_strnsave(startp, startplen);
		    ଯଦି (*newp != ଖାଲି)
		    {
			p = *newp + (p - startp);
			startp = *newp;
		    }
		}
		ଯଦି (dropped != ଖାଲି)
		    ++*dropped;
		ଯଦି (*newp != ଖାଲି)
		    mch_memmove(p, p + 1, startplen - ((p + 1) - startp) + 1);
		ଅନ୍ୟଥା
		    ++p;
	    }
	    ଅନ୍ୟଥା
		++p;    // skip next character
	    ଯଦି (*p == 'v')
		mymagic = MAGIC_ALL;
	    ଅନ୍ୟଥା ଯଦି (*p == 'V')
		mymagic = MAGIC_NONE;
	}
    }
    ଯଦି (magic_val != ଖାଲି)
	*magic_val = mymagic;
    ଫେରନ୍ତୁ p;
}

/*
 * Functions for getting characters from the regexp input.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	prevchr_len;	// byte length of previous char
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	at_start;	// True when on the first character
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	prev_at_start;  // True when on the second character

/*
 * Start parsing at "str".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
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
    ସ୍ଥିର ଶୂନ୍ୟ
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
    ସ୍ଥିର ଶୂନ୍ୟ
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
peekchr(ଶୂନ୍ୟ)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	after_slash = FALSE;

    ଯଦି (curchr != -1)
	ଫେରନ୍ତୁ curchr;

    ବାଛ (curchr = regparse[0])
    {
	ସ୍ଥିତି '.':
	ସ୍ଥିତି '[':
	ସ୍ଥିତି '~':
	    // magic when 'magic' is on
	    ଯଦି (reg_magic >= MAGIC_ON)
		curchr = Magic(curchr);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି '(':
	ସ୍ଥିତି ')':
	ସ୍ଥିତି '{':
	ସ୍ଥିତି '%':
	ସ୍ଥିତି '+':
	ସ୍ଥିତି '=':
	ସ୍ଥିତି '?':
	ସ୍ଥିତି '@':
	ସ୍ଥିତି '!':
	ସ୍ଥିତି '&':
	ସ୍ଥିତି '|':
	ସ୍ଥିତି '<':
	ସ୍ଥିତି '>':
	ସ୍ଥିତି '#':	// future ext.
	ସ୍ଥିତି '"':	// future ext.
	ସ୍ଥିତି '\'':	// future ext.
	ସ୍ଥିତି ',':	// future ext.
	ସ୍ଥିତି '-':	// future ext.
	ସ୍ଥିତି ':':	// future ext.
	ସ୍ଥିତି ';':	// future ext.
	ସ୍ଥିତି '`':	// future ext.
	ସ୍ଥିତି '/':	// Can't be used in / command
			// magic only after "\v"
	    ଯଦି (reg_magic == MAGIC_ALL)
		curchr = Magic(curchr);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି '*':
	    // * is not magic as the very first character, eg "?*ptr", when
	    // after '^', eg "/^*ptr" and when after "\(", "\|", "\&".  But
	    // "\(\*" is not magic, thus must be magic if "after_slash"
	    ଯଦି (reg_magic >= MAGIC_ON
		    && !at_start
		    && !(prev_at_start && prevchr == Magic('^'))
		    && (after_slash
			|| (prevchr != Magic('(')
			    && prevchr != Magic('&')
			    && prevchr != Magic('|'))))
		curchr = Magic('*');
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି '^':
	    // '^' is only magic as the very first character and if it's after
	    // "\(", "\|", "\&' or "\n"
	    ଯଦି (reg_magic >= MAGIC_OFF
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
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି '$':
	    // '$' is only magic as the very last char and if it's in front of
	    // either "\|", "\)", "\&", or "\n"
	    ଯଦି (reg_magic >= MAGIC_OFF)
	    {
		char_u *p = regparse + 1;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_magic_all = (reg_magic == MAGIC_ALL);

		// ignore \c \C \m \M \v \V and \Z after '$'
		ଯେତେବେଳେ (p[0] == '\\' && (p[1] == 'c' || p[1] == 'C'
			    || p[1] == 'm' || p[1] == 'M'
			    || p[1] == 'v' || p[1] == 'V' || p[1] == 'Z'))
		{
		    ଯଦି (p[1] == 'v')
			is_magic_all = TRUE;
		    ଅନ୍ୟଥା ଯଦି (p[1] == 'm' || p[1] == 'M' || p[1] == 'V')
			is_magic_all = FALSE;
		    p += 2;
		}
		ଯଦି (p[0] == NUL
			|| (p[0] == '\\'
			    && (p[1] == '|' || p[1] == '&' || p[1] == ')'
				|| p[1] == 'n'))
			|| (is_magic_all
			    && (p[0] == '|' || p[0] == '&' || p[0] == ')'))
			|| reg_magic == MAGIC_ALL)
		    curchr = Magic('$');
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି '\\':
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = regparse[1];

		ଯଦି (c == NUL)
		    curchr = '\\';	// trailing '\'
		ଅନ୍ୟଥା ଯଦି (c <= '~' && META_flags[c])
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
		ଅନ୍ୟଥା ଯଦି (vim_strchr(REGEXP_ABBR, c))
		{
		    /*
		     * Handle abbreviations, like "\t" for TAB -- webb
		     */
		    curchr = backslash_trans(c);
		}
		ଅନ୍ୟଥା ଯଦି (reg_magic == MAGIC_NONE && (c == '$' || c == '^'))
		    curchr = toggle_Magic(c);
		ଅନ୍ୟଥା
		{
		    /*
		     * Next character can never be (made) magic?
		     * Then backslashing it won't do anything.
		     */
		    ଯଦି (has_mbyte && c >= 0x80)
			curchr = (*mb_ptr2char)(regparse + 1);
		    ଅନ୍ୟଥା
			curchr = c;
		}
		ଭାଙ୍ଗ;
	    }

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    // curchr already holds regparse[0]; only a multi-byte lead byte
	    // needs decoding.
	    ଯଦି (has_mbyte && curchr >= 0x80)
		curchr = (*mb_ptr2char)(regparse);
    }

    ଫେରନ୍ତୁ curchr;
}

/*
 * Eat one lexed character.  Do this in a way that we can undo it.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
skipchr(ଶୂନ୍ୟ)
{
    // peekchr() eats a backslash, do the same here
    ଯଦି (*regparse == '\\')
	prevchr_len = 1;
    ଅନ୍ୟଥା
	prevchr_len = 0;
    ଯଦି (regparse[prevchr_len] != NUL)
    {
	ଯଦି (enc_utf8)
	{
	    // Exclude composing chars that mb_ptr2len does include.  A byte
	    // below 0x80 is always a single character.
	    ଯଦି (regparse[prevchr_len] < 0x80)
		++prevchr_len;
	    ଅନ୍ୟଥା
		prevchr_len += utf_ptr2len(regparse + prevchr_len);
	}
	ଅନ୍ୟଥା ଯଦି (has_mbyte)
	    prevchr_len += (*mb_ptr2len)(regparse + prevchr_len);
	ଅନ୍ୟଥା
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
    ସ୍ଥିର ଶୂନ୍ୟ
skipchr_keepstart(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା as = prev_at_start;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pr = prevchr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା prpr = prevprevchr;

    skipchr();
    at_start = as;
    prevchr = pr;
    prevprevchr = prpr;
}

/*
 * Get the next character from the pattern. We know about magic and such, so
 * therefore we need a lexical analyzer.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
getchr(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା chr = peekchr();

    skipchr();
    ଫେରନ୍ତୁ chr;
}

/*
 * put character back.  Works only once!
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ungetchr(ଶୂନ୍ୟ)
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
    ସ୍ଥିର vimlong_T
gethexchrs(ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxinputlen)
{
    long_u	nr = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = 0; i < maxinputlen; ++i)
    {
	c = regparse[0];
	ଯଦି (!vim_isxdigit(c))
	    ଭାଙ୍ଗ;
	nr <<= 4;
	nr |= hex2nr(c);
	++regparse;
    }

    ଯଦି (i == 0)
	ଫେରନ୍ତୁ -1;
    ଫେରନ୍ତୁ nr;
}

/*
 * Get and return the value of the decimal string immediately after the
 * current position. Return -1 for invalid.  Consumes all digits.
 */
    ସ୍ଥିର vimlong_T
getdecchrs(ଶୂନ୍ୟ)
{
    long_u	nr = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = 0; ; ++i)
    {
	c = regparse[0];
	ଯଦି (c < '0' || c > '9')
	    ଭାଙ୍ଗ;
	nr *= 10;
	nr += c - '0';
	++regparse;
	curchr = -1; // no longer valid
    }

    ଯଦି (i == 0)
	ଫେରନ୍ତୁ -1;
    ଫେରନ୍ତୁ nr;
}

/*
 * get and return the value of the octal string immediately after the current
 * position. Return -1 for invalid, or 0-255 for valid. Smart enough to handle
 * numbers > 377 correctly (for example, 400 is treated as 40) and doesn't
 * treat 8 or 9 as recognised characters. Position is updated:
 *     blahblah\%o210asdf
 *	   before-^  ^-after
 */
    ସ୍ଥିର long
getoctchrs(ଶୂନ୍ୟ)
{
    long_u	nr = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = 0; i < 3 && nr < 040; ++i)
    {
	c = regparse[0];
	ଯଦି (c < '0' || c > '7')
	    ଭାଙ୍ଗ;
	nr <<= 3;
	nr |= hex2nr(c);
	++regparse;
    }

    ଯଦି (i == 0)
	ଫେରନ୍ତୁ -1;
    ଫେରନ୍ତୁ (long)nr;
}

/*
 * read_limits - Read two integers to be taken as a minimum and maximum.
 * If the first character is '-', then the range is reversed.
 * Should end with 'end'.  If minval is missing, zero is default, if maxval is
 * missing, a very big number is the default.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_limits(long *minval, long *maxval)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		reverse = FALSE;
    char_u	*first_char;
    long	tmp;

    ଯଦି (*regparse == '-')
    {
	// Starts with '-', so reverse the range later
	regparse++;
	reverse = TRUE;
    }
    first_char = regparse;
    *minval = getdigits(&regparse);
    ଯଦି (*regparse == ',')	    // There is a comma
    {
	ଯଦି (vim_isdigit(*++regparse))
	    *maxval = getdigits(&regparse);
	ଅନ୍ୟଥା
	    *maxval = MAX_LIMIT;
    }
    ଅନ୍ୟଥା ଯଦି (VIM_ISDIGIT(*first_char))
	*maxval = *minval;	    // It was \{n} or \{-n}
    ଅନ୍ୟଥା
	*maxval = MAX_LIMIT;	    // It was \{} or \{-}
    ଯଦି (*regparse == '\\')
	regparse++;	// Allow either \{...} or \{...\}
    ଯଦି (*regparse != '}')
	EMSG2_RET_FAIL(_(e_syntax_error_in_str_curlies),
						       reg_magic == MAGIC_ALL);

    /*
     * Reverse the range if there was a '-', or make sure it is in the right
     * order otherwise.
     */
    ଯଦି ((!reverse && *minval > *maxval) || (reverse && *minval < *maxval))
    {
	tmp = *minval;
	*minval = *maxval;
	*maxval = tmp;
    }
    skipchr();		// let's be friends with the lexer again
    ଫେରନ୍ତୁ OK;
}

/*
 * vim_regexec and friends
 */

/*
 * Global work variables for vim_regexec().
 */

ସ୍ଥିର ଶୂନ୍ୟ	cleanup_subexpr(ଶୂନ୍ୟ);
#ifdef FEAT_SYN_HL
ସ୍ଥିର ଶୂନ୍ୟ	cleanup_zsubexpr(ଶୂନ୍ୟ);
#endif
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	match_with_backref(linenr_T start_lnum, colnr_T start_col, linenr_T end_lnum, colnr_T end_col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *bytelen);

/*
 * Sometimes need to save a copy of a line.  Since alloc()/free() is very
 * slow, we keep one allocated piece of memory and only re-allocate it when
 * it's too small.  It's freed in bt_regexec_both() when finished.
 */
ସ୍ଥିର char_u	*reg_tofree = ଖାଲି;
ସ୍ଥିର unsigned	reg_tofreelen;

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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			reg_ଧାଡ଼ି_lbr;	// "\n" in string is line break

    // The current match-position is stord in these variables:
    linenr_T	lnum;		// line number, relative to first line
    char_u	*ଧାଡ଼ି;		// start of current line
    char_u	*ନିବେଶ;		// current input, points into "line"

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	need_clear_subexpr;	// subexpressions still need to be cleared
#ifdef FEAT_SYN_HL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	need_clear_zsubexpr;	// extmatch subexpressions still need to be
				// cleared
#endif

    // Internal copy of 'ignorecase'.  It is set at each call to vim_regexec().
    // Normally it gets the value of "rm_ic" or "rmm_ic", but when the pattern
    // contains '\c' or '\C' the value is overruled.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			reg_ic;

    // Similar to "reg_ic", but only for 'combining' characters.  Set with \Z
    // flag in the regexp.  Defaults to false, always.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			reg_icombine;

    // Copy of "rmm_maxcol": maximum column to search for a match.  Zero when
    // there is no maximum.
    colnr_T		reg_maxcol;

    // State for the NFA engine regexec.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା nfa_has_zend;	    // NFA regexp \ze operator encountered.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା nfa_has_backref;    // NFA regexp \1 .. \9 encountered.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା nfa_nsubexpr;	    // Number of sub expressions actually being used
			    // during execution. 1 if only the whole match
			    // (subexpr 0) is used.
    // listid is global, so that it increases on recursive calls to
    // nfa_regmatch(), which means we don't have to clear the lastlist field of
    // all the states.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା nfa_listid;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା nfa_alt_listid;

#ifdef FEAT_SYN_HL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା nfa_has_zsubexpr;   // NFA regexp has \z( ), set zsubexpr.
#endif
} regexec_T;

ସ୍ଥିର regexec_T	rex;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		rex_in_use = FALSE;

/*
 * Return TRUE if character 'c' is included in 'iskeyword' option for
 * "reg_buf" buffer.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
reg_iswordc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଫେରନ୍ତୁ vim_iswordc_buf(c, rex.reg_buf);
}

#ifdef FEAT_EVAL
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା can_f_submatch = FALSE;	// TRUE when submatch() can be used

// This struct is used for reg_submatch(). Needed for when the
// substitution string is an expression that contains a call to substitute()
// and submatch().
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    regmatch_T	*sm_match;
    regmmatch_T	*sm_mmatch;
    linenr_T	sm_firstlnum;
    linenr_T	sm_maxline;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sm_ଧାଡ଼ି_lbr;
} regsubmatch_T;

ସ୍ଥିର regsubmatch_T rsm;  // can only be used when can_f_submatch is TRUE
#endif

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା
{
    RGLF_ଧାଡ଼ି = 0x01,
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
    ସ୍ଥିର ଶୂନ୍ୟ
reg_getline_common(
    linenr_T		lnum,
    reg_getline_flags_T	flags,
    char_u		**ଧାଡ଼ି,
    colnr_T		*length)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_ଧାଡ଼ି = flags & RGLF_ଧାଡ଼ି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_length = flags & RGLF_LENGTH;
    linenr_T firstlnum;
    linenr_T maxline;

#ifdef FEAT_EVAL
    ଯଦି (flags & RGLF_SUBMATCH)
    {
	firstlnum = rsm.sm_firstlnum + lnum;
	maxline = rsm.sm_maxline;
    }
    ଅନ୍ୟଥା
#endif
    {
	firstlnum = rex.reg_firstlnum + lnum;
	maxline = rex.reg_maxline;
    }

    // when looking behind for a match/no-match lnum is negative. but we
    // can't go before line 1.
    ଯଦି (firstlnum < 1)
    {
	ଯଦି (get_ଧାଡ଼ି)
	    *ଧାଡ଼ି = ଖାଲି;
	ଯଦି (get_length)
	    *length = 0;

	ଫେରନ୍ତୁ;
    }

    ଯଦି (lnum > maxline)
    {
	// must have matched the "\n" in the last line.
	ଯଦି (get_ଧାଡ଼ି)
	    *ଧାଡ଼ି = (char_u *)"";
	ଯଦି (get_length)
	    *length = 0;

	ଫେରନ୍ତୁ;
    }

    ଯଦି (get_ଧାଡ଼ି)
	*ଧାଡ଼ି = ml_get_buf(rex.reg_buf, firstlnum, FALSE);
    ଯଦି (get_length)
	*length = ml_get_buf_len(rex.reg_buf, firstlnum);
}

/*
 * Get pointer to the line "lnum", which is relative to "reg_firstlnum".
 */
    ସ୍ଥିର char_u *
reg_getline(linenr_T lnum)
{
    char_u *ଧାଡ଼ି;

    reg_getline_common(lnum, RGLF_ଧାଡ଼ି, &ଧାଡ଼ି, ଖାଲି);

    ଫେରନ୍ତୁ ଧାଡ଼ି;
}

/*
 * Get length of line "lnum", which is relative to "reg_firstlnum".
 */
    ସ୍ଥିର colnr_T
reg_getline_len(linenr_T lnum)
{
    colnr_T length;

    reg_getline_common(lnum, RGLF_LENGTH, ଖାଲି, &length);

    ଫେରନ୍ତୁ length;
}

#ifdef FEAT_SYN_HL
ସ୍ଥିର char_u	*reg_startzp[NSUBEXP];	// Workspace to mark beginning
ସ୍ଥିର char_u	*reg_endzp[NSUBEXP];	//   and end of \z(...\) matches
ସ୍ଥିର lpos_T	reg_startzpos[NSUBEXP];	// idem, beginning pos
ସ୍ଥିର lpos_T	reg_endzpos[NSUBEXP];	// idem, end pos
#endif

// TRUE if using multi-line regexp.
#define REG_MULTI	(rex.reg_match == ଖାଲି)

#ifdef FEAT_SYN_HL
/*
 * Create a new extmatch and mark it as referenced once.
 */
    ସ୍ଥିର reg_extmatch_T *
make_extmatch(ଶୂନ୍ୟ)
{
    reg_extmatch_T	*em;

    em = ALLOC_CLEAR_ONE(reg_extmatch_T);
    ଯଦି (em != ଖାଲି)
	em->refcnt = 1;
    ଫେରନ୍ତୁ em;
}

/*
 * Add a reference to an extmatch.
 */
    reg_extmatch_T *
ref_extmatch(reg_extmatch_T *em)
{
    ଯଦି (em != ଖାଲି)
	em->refcnt++;
    ଫେରନ୍ତୁ em;
}

/*
 * Remove a reference to an extmatch.  If there are no references left, free
 * the info.
 */
    ଶୂନ୍ୟ
unref_extmatch(reg_extmatch_T *em)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ଯଦି (em != ଖାଲି && --em->refcnt <= 0)
    {
	ପାଇଁ (i = 0; i < NSUBEXP; ++i)
	    vim_free(em->matches[i]);
	vim_free(em);
    }
}
#endif

/*
 * Get class of previous character.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
reg_prev_class(ଶୂନ୍ୟ)
{
    ଯଦି (rex.ନିବେଶ > rex.ଧାଡ଼ି)
	ଫେରନ୍ତୁ mb_get_class_buf(rex.ନିବେଶ - 1
		       - (*mb_head_off)(rex.ଧାଡ଼ି, rex.ନିବେଶ - 1), rex.reg_buf);
    ଫେରନ୍ତୁ -1;
}

/*
 * Return TRUE if the current rex.input position matches the Visual area.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
reg_match_visual(ଶୂନ୍ୟ)
{
    pos_T	ଶୀର୍ଷ, bot;
    linenr_T    lnum;
    colnr_T	col;
    win_T	*wp = rex.reg_win == ଖାଲି ? curwin : rex.reg_win;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mode;
    colnr_T	start, end;
    colnr_T	start2, end2;
    colnr_T	cols;
    colnr_T	curswant;

    // Check if the buffer is the current buffer and not using a string.
    ଯଦି (rex.reg_buf != curbuf || VIsual.lnum == 0 || !REG_MULTI)
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (VIsual_active)
    {
	ଯଦି (LT_POS(VIsual, wp->w_cursor))
	{
	    ଶୀର୍ଷ = VIsual;
	    bot = wp->w_cursor;
	}
	ଅନ୍ୟଥା
	{
	    ଶୀର୍ଷ = wp->w_cursor;
	    bot = VIsual;
	}
	mode = VIsual_mode;
	curswant = wp->w_curswant;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (LT_POS(curbuf->b_visual.vi_start, curbuf->b_visual.vi_end))
	{
	    ଶୀର୍ଷ = curbuf->b_visual.vi_start;
	    bot = curbuf->b_visual.vi_end;
	}
	ଅନ୍ୟଥା
	{
	    ଶୀର୍ଷ = curbuf->b_visual.vi_end;
	    bot = curbuf->b_visual.vi_start;
	}
	// a substitute command may have removed some lines
	ଯଦି (bot.lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    bot.lnum = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	mode = curbuf->b_visual.vi_mode;
	curswant = curbuf->b_visual.vi_curswant;
    }
    lnum = rex.lnum + rex.reg_firstlnum;
    ଯଦି (lnum < ଶୀର୍ଷ.lnum || lnum > bot.lnum)
	ଫେରନ୍ତୁ FALSE;

    col = (colnr_T)(rex.ନିବେଶ - rex.ଧାଡ଼ି);
    ଯଦି (mode == 'v')
    {
	ଯଦି ((lnum == ଶୀର୍ଷ.lnum && col < ଶୀର୍ଷ.col)
		|| (lnum == bot.lnum && col >= bot.col + (*p_sel != 'e')))
	    ଫେରନ୍ତୁ FALSE;
    }
    ଅନ୍ୟଥା ଯଦି (mode == Ctrl_V)
    {
	getvvcol(wp, &ଶୀର୍ଷ, &start, ଖାଲି, &end, 0);
	getvvcol(wp, &bot, &start2, ଖାଲି, &end2, 0);
	ଯଦି (start2 < start)
	    start = start2;
	ଯଦି (end2 > end)
	    end = end2;
	ଯଦି (ଶୀର୍ଷ.col == MAXCOL || bot.col == MAXCOL || curswant == MAXCOL)
	    end = MAXCOL;

	// getvvcol() flushes rex.line, need to get it again
	rex.ଧାଡ଼ି = reg_getline(rex.lnum);
	rex.ନିବେଶ = rex.ଧାଡ଼ି + col;

	cols = win_linetabsize(wp, rex.reg_firstlnum + rex.lnum, rex.ଧାଡ଼ି, col);
	ଯଦି (cols < start || cols > end - (*p_sel == 'e'))
	    ଫେରନ୍ତୁ FALSE;
    }
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Check the regexp program for its magic number.
 * Return TRUE if it's wrong.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prog_magic_wrong(ଶୂନ୍ୟ)
{
    regprog_T	*prog;

    prog = REG_MULTI ? rex.reg_mmatch->regprog : rex.reg_match->regprog;
    ଯଦି (prog->engine == &nfa_regengine)
	// For NFA matcher we don't check the magic
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (UCHARAT(((bt_regprog_T *)prog)->program) != REGMAGIC)
    {
	iemsg(e_corrupted_regexp_program);
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Cleanup the subexpressions, if this wasn't done yet.
 * This construction is used to clear the subexpressions only when they are
 * used (to increase speed).
 */
    ସ୍ଥିର ଶୂନ୍ୟ
cleanup_subexpr(ଶୂନ୍ୟ)
{
    ଯଦି (!rex.need_clear_subexpr)
	ଫେରନ୍ତୁ;

    ଯଦି (REG_MULTI)
    {
	// Use 0xff to set lnum to -1
	vim_memset(rex.reg_startpos, 0xff, ଆକାର(lpos_T) * NSUBEXP);
	vim_memset(rex.reg_endpos, 0xff, ଆକାର(lpos_T) * NSUBEXP);
    }
    ଅନ୍ୟଥା
    {
	vim_memset(rex.reg_startp, 0, ଆକାର(char_u *) * NSUBEXP);
	vim_memset(rex.reg_endp, 0, ଆକାର(char_u *) * NSUBEXP);
    }
    rex.need_clear_subexpr = FALSE;
}

#ifdef FEAT_SYN_HL
    ସ୍ଥିର ଶୂନ୍ୟ
cleanup_zsubexpr(ଶୂନ୍ୟ)
{
    ଯଦି (!rex.need_clear_zsubexpr)
	ଫେରନ୍ତୁ;

    ଯଦି (REG_MULTI)
    {
	// Use 0xff to set lnum to -1
	vim_memset(reg_startzpos, 0xff, ଆକାର(lpos_T) * NSUBEXP);
	vim_memset(reg_endzpos, 0xff, ଆକାର(lpos_T) * NSUBEXP);
    }
    ଅନ୍ୟଥା
    {
	vim_memset(reg_startzp, 0, ଆକାର(char_u *) * NSUBEXP);
	vim_memset(reg_endzp, 0, ଆକାର(char_u *) * NSUBEXP);
    }
    rex.need_clear_zsubexpr = FALSE;
}
#endif

/*
 * Advance rex.lnum, rex.line and rex.input to the next line.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
reg_nextline(ଶୂନ୍ୟ)
{
    rex.ଧାଡ଼ି = reg_getline(++rex.lnum);
    rex.ନିବେଶ = rex.ଧାଡ଼ି;
    fast_breakcheck();
}

/*
 * Check whether a backreference matches.
 * Returns RA_FAIL, RA_NOMATCH or RA_MATCH.
 * If "bytelen" is not NULL, it is set to the byte length of the match in the
 * last line.
 * Optional: ignore case if rex.reg_ic is set.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
match_with_backref(
    linenr_T start_lnum,
    colnr_T  start_col,
    linenr_T end_lnum,
    colnr_T  end_col,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	     *bytelen)
{
    linenr_T	clnum = start_lnum;
    colnr_T	ccol = start_col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    char_u	*p;

    ଯଦି (bytelen != ଖାଲି)
	*bytelen = 0;
    ପାଇଁ (;;)
    {
	// Since getting one line may invalidate the other, need to make copy.
	// Slow!
	ଯଦି (rex.ଧାଡ଼ି != reg_tofree)
	{
	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(rex.ଧାଡ଼ି);
	    ଯଦି (reg_tofree == ଖାଲି || len >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)reg_tofreelen)
	    {
		len += 50;	// get some extra
		vim_free(reg_tofree);
		reg_tofree = alloc(len);
		ଯଦି (reg_tofree == ଖାଲି)
		    ଫେରନ୍ତୁ RA_FAIL; // out of memory!
		reg_tofreelen = len;
	    }
	    STRCPY(reg_tofree, rex.ଧାଡ଼ି);
	    rex.ନିବେଶ = reg_tofree + (rex.ନିବେଶ - rex.ଧାଡ଼ି);
	    rex.ଧାଡ଼ି = reg_tofree;
	}

	// Get the line to compare with.
	p = reg_getline(clnum);
	ଯଦି (clnum == end_lnum)
	    len = end_col - ccol;
	ଅନ୍ୟଥା
	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)reg_getline_len(clnum) - ccol;

	// Use case-insensitive compare if rex.reg_ic is set
	ଯଦି ((!rex.reg_ic && cstrncmp(p + ccol, rex.ନିବେଶ, &len) != 0)
	    || (rex.reg_ic && MB_STRNICMP(p + ccol, rex.ନିବେଶ, len) != 0))
	    ଫେରନ୍ତୁ RA_NOMATCH;  // doesn't match
	ଯଦି (bytelen != ଖାଲି)
	    *bytelen += len;
	ଯଦି (clnum == end_lnum)
	    ଭାଙ୍ଗ;		// match and at end!
	ଯଦି (rex.lnum >= rex.reg_maxline)
	    ଫେରନ୍ତୁ RA_NOMATCH;  // text too short

	// Advance to next line.
	reg_nextline();
	ଯଦି (bytelen != ଖାଲି)
	    *bytelen = 0;
	++clnum;
	ccol = 0;
	ଯଦି (got_int)
	    ଫେରନ୍ତୁ RA_FAIL;
    }

    // found a match!  Note that rex.line may now point to a copy of the line,
    // that should not matter.
    ଫେରନ୍ତୁ RA_MATCH;
}

/*
 * Used in a place where no * or \+ can follow.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
re_mult_next(ଅକ୍ଷର *what)
{
    ଯଦି (re_multi_type(peekchr()) == MULTI_MULT)
    {
	semsg(_(e_nfa_regexp_cannot_repeat_str), what);
	rc_did_emsg = TRUE;
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା a, b, c;
} decomp_T;


// 0xfb20 - 0xfb4f
ସ୍ଥିର decomp_T decomp_table[0xfb4f-0xfb20+1] =
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

    ସ୍ଥିର ଶୂନ୍ୟ
mb_decompose(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *c1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *c2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *c3)
{
    decomp_T d;

    ଯଦି (c >= 0xfb20 && c <= 0xfb4f)
    {
	d = decomp_table[c - 0xfb20];
	*c1 = d.a;
	*c2 = d.b;
	*c3 = d.c;
    }
    ଅନ୍ୟଥା
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cstrncmp(char_u *s1, char_u *s2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *n)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result;

    ଯଦି (!rex.reg_ic)
	result = STRNCMP(s1, s2, *n);
    ଅନ୍ୟଥା ଯଦି (enc_utf8)
    {
	char_u *p = s1;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା n2 = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା n1 = *n;
	// count the number of characters for byte-length of s1
	ଯେତେବେଳେ (n1 > 0 && *p != NUL)
	{
	    n1 -= mb_ptr2len(p);
	    MB_PTR_ADV(p);
	    n2++;
	}
	// count the number of bytes to advance the same number of chars for s2
	p = s2;
	ଯେତେବେଳେ (n2-- > 0 && *p != NUL)
	    MB_PTR_ADV(p);

	n2 = p - s2;

	result = MB_STRNICMP2(s1, s2, *n, n2);
	ଯଦି (result == 0)
	    *n = n2;
    }
    ଅନ୍ୟଥା
	result = MB_STRNICMP(s1, s2, *n);

    // if it failed and it's utf8 and we want to combineignore:
    ଯଦି (result != 0 && enc_utf8 && rex.reg_icombine)
    {
	char_u	*str1, *str2;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	c1, c2, c11, c12;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	junk;

	// we have to handle the strcmp ourselves, since it is necessary to
	// deal with the composing characters by ignoring them:
	str1 = s1;
	str2 = s2;
	c1 = c2 = 0;
	ଯେତେବେଳେ ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(str1 - s1) < *n)
	{
	    c1 = mb_ptr2char_adv(&str1);
	    c2 = mb_ptr2char_adv(&str2);

	    // Decompose the character if necessary, into 'base' characters.
	    // Currently hard-coded for Hebrew, Arabic to be done...
	    ଯଦି (c1 != c2 && (!rex.reg_ic || utf_fold(c1) != utf_fold(c2)))
	    {
		// decomposition necessary?
		mb_decompose(c1, &c11, &junk, &junk);
		mb_decompose(c2, &c12, &junk, &junk);
		c1 = c11;
		c2 = c12;
		ଯଦି (c11 != c12
			    && (!rex.reg_ic || utf_fold(c11) != utf_fold(c12)))
		    ଭାଙ୍ଗ;
	    }
	}
	result = c2 - c1;
	ଯଦି (result == 0)
	    *n = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(str2 - s2);
    }

    ଫେରନ୍ତୁ result;
}

/*
 * cstrchr: This function is used a lot for simple searches, keep it fast!
 */
    ସ୍ଥିର char_u *
cstrchr(char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cc, lc;

    ଯଦି (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	ଫେରନ୍ତୁ vim_strchr(s, c);

    // tolower() and toupper() can be slow, comparing twice should be a lot
    // faster (esp. when using MS Visual C++!).
    // For UTF-8 need to use folded case.
    ଯଦି (enc_utf8 && c > 0x80)
    {
	cc = utf_fold(c);
	lc = cc;
    }
    ଅନ୍ୟଥା
	ଯଦି (MB_ISUPPER(c))
	{
	    cc = MB_TOLOWER(c);
	    lc = cc;
	}
	ଅନ୍ୟଥା ଯଦି (MB_ISLOWER(c))
	{
	    cc = MB_TOUPPER(c);
	    lc = c;
	}
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ vim_strchr(s, c);

    ଯଦି (has_mbyte)
    {
	ପାଇଁ (p = s; *p != NUL; p += (*mb_ptr2len)(p))
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା uc = utf_ptr2char(p);
	    ଯଦି (enc_utf8 && (c > 0x80 || uc > 0x80))
	    {
		// Do not match an illegal byte.  E.g. 0xff matches 0xc3 0xbf,
		// not 0xff.
		// compare with lower case of the character
		ଯଦି ((uc < 0x80 || uc != *p) && utf_fold(uc) == lc)
		    ଫେରନ୍ତୁ p;
	    }
	    ଅନ୍ୟଥା ଯଦି (*p == c || *p == cc)
		ଫେରନ୍ତୁ p;
	}
    }
    ଅନ୍ୟଥା
	// Faster version for when there are no multi-byte characters.
	ପାଇଁ (p = s; *p != NUL; ++p)
	    ଯଦି (*p == c || *p == cc)
		ଫେରନ୍ତୁ p;

    ଫେରନ୍ତୁ ଖାଲି;
}

////////////////////////////////////////////////////////////////
//		      regsub stuff			      //
////////////////////////////////////////////////////////////////

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଶୂନ୍ୟ (*fptr_T)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା vim_regsub_both(char_u *source, typval_T *ପଦସମଷ୍ଟି, char_u *dest, ପୂର୍ଣ୍ଣସଂଖ୍ୟା destlen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);

    ସ୍ଥିର ଶୂନ୍ୟ
do_upper(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *d, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    *d = MB_TOUPPER(c);
}

    ସ୍ଥିର ଶୂନ୍ୟ
do_lower(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *d, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
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
regtilde(char_u *source, ପୂର୍ଣ୍ଣସଂଖ୍ୟା magic)
{
    char_u	*newsub = source;
    char_u	*p;
    size_t	newsublen = 0;
    char_u	tilde[3] = {'~', NUL, NUL};
    size_t	tildelen = 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

    ଯଦି (!magic)
    {
	tilde[0] = '\\';
	tilde[1] = '~';
	tilde[2] = NUL;
	tildelen = 2;
    }

    ପାଇଁ (p = newsub; *p; ++p)
    {
	ଯଦି (STRNCMP(p, tilde, tildelen) == 0)
	{
	    size_t prefixlen = p - newsub;		// not including the tilde
	    char_u *postfix = p + tildelen;
	    size_t postfixlen;
	    size_t tmpsublen;

	    ଯଦି (newsublen == 0)
		newsublen = STRLEN(newsub);
	    newsublen -= tildelen;
	    postfixlen = newsublen - prefixlen;
	    tmpsublen = prefixlen + reg_prev_sublen + postfixlen;

	    ଯଦି (tmpsublen > 0 && reg_prev_sub != ଖାଲି)
	    {
		char_u *tmpsub;

		// Avoid making the text longer than MAXCOL, it will cause
		// trouble at some point.
		ଯଦି (tmpsublen > MAXCOL)
		{
		    emsg(_(e_resulting_text_too_long));
		    error = TRUE;
		    ଭାଙ୍ଗ;
		}

		tmpsub = alloc(tmpsublen + 1);
		ଯଦି (tmpsub == ଖାଲି)
		{
		    emsg(_(e_out_of_memory));
		    error = TRUE;
		    ଭାଙ୍ଗ;
		}

		// copy prefix
		mch_memmove(tmpsub, newsub, prefixlen);
		// interpret tilde
		mch_memmove(tmpsub + prefixlen, reg_prev_sub, reg_prev_sublen);
		// copy postfix
		STRCPY(tmpsub + prefixlen + reg_prev_sublen, postfix);

		ଯଦି (newsub != source)	// allocated newsub before
		    vim_free(newsub);
		newsub = tmpsub;
		newsublen = tmpsublen;
		p = newsub + prefixlen + reg_prev_sublen;
	    }
	    ଅନ୍ୟଥା
		mch_memmove(p, postfix, postfixlen + 1);	// remove the tilde (+1 for the NUL)

	    --p;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (*p == '\\' && p[1])		// skip escaped characters
		++p;
	    ଯଦି (has_mbyte)
		p += (*mb_ptr2len)(p) - 1;
	}
    }

    ଯଦି (error)
    {
	ଯଦି (newsub != source)
	    vim_free(newsub);
	ଫେରନ୍ତୁ source;
    }

    // Store a copy of newsub  in reg_prev_sub.  It is always allocated,
    // because recursive calls may make the returned string invalid.
    // Only store it if there something to store.
    newsublen = p - newsub;
    ଯଦି (newsublen == 0)
	VIM_CLEAR(reg_prev_sub);
    ଅନ୍ୟଥା
    {
	vim_free(reg_prev_sub);
	reg_prev_sub = vim_strnsave(newsub, newsublen);
    }

    ଯଦି (reg_prev_sub == ଖାଲି)
	reg_prev_sublen = 0;
    ଅନ୍ୟଥା
	reg_prev_sublen = newsublen;

    ଫେରନ୍ତୁ newsub;
}

#ifdef FEAT_EVAL

/*
 * Put the submatches in "argv[argskip]" which is a list passed into
 * call_func() by vim_regsub_both().
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fill_submatch_list(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc UNUSED, typval_T *argv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argskip, ufunc_T *fp)
{
    listitem_T	*li;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*s;
    typval_T	*listarg = argv + argskip;

    ଯଦି (!has_varargs(fp) && fp->uf_args.ga_len <= argskip)
	// called function doesn't take a submatches argument
	ଫେରନ୍ତୁ argskip;

    // Relies on sl_list to be the first item in staticList10_T.
    init_static_list((staticList10_T *)(listarg->vval.v_list));

    // There are always 10 list items in staticList10_T.
    li = listarg->vval.v_list->lv_first;
    ପାଇଁ (i = 0; i < 10; ++i)
    {
	s = rsm.sm_match->startp[i];
	ଯଦି (s == ଖାଲି || rsm.sm_match->endp[i] == ଖାଲି)
	    s = ଖାଲି;
	ଅନ୍ୟଥା
	    s = vim_strnsave(s, rsm.sm_match->endp[i] - s);
	li->li_tv.v_type = VAR_STRING;
	li->li_tv.vval.v_string = s;
	li = li->li_next;
    }
    ଫେରନ୍ତୁ argskip + 1;
}

    ସ୍ଥିର ଶୂନ୍ୟ
clear_submatch_list(staticList10_T *sl)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ପାଇଁ (i = 0; i < 10; ++i)
	vim_free(sl->sl_ବସ୍ତୁଗୁଡ଼ିକ[i].li_tv.vval.v_string);
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_regsub(
    regmatch_T	*rmp,
    char_u	*source,
    typval_T	*ପଦସମଷ୍ଟି,
    char_u	*dest,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		destlen,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result;
    regexec_T	rex_save;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		rex_in_use_save = rex_in_use;

    ଯଦି (rex_in_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_in_use = TRUE;

    rex.reg_match = rmp;
    rex.reg_mmatch = ଖାଲି;
    rex.reg_maxline = 0;
    rex.reg_buf = curbuf;
    rex.reg_ଧାଡ଼ି_lbr = TRUE;
    result = vim_regsub_both(source, ପଦସମଷ୍ଟି, dest, destlen, flags);

    rex_in_use = rex_in_use_save;
    ଯଦି (rex_in_use)
	rex = rex_save;

    ଫେରନ୍ତୁ result;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_regsub_multi(
    regmmatch_T	*rmp,
    linenr_T	lnum,
    char_u	*source,
    char_u	*dest,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		destlen,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result;
    regexec_T	rex_save;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		rex_in_use_save = rex_in_use;

    ଯଦି (rex_in_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_in_use = TRUE;

    rex.reg_match = ଖାଲି;
    rex.reg_mmatch = rmp;
    rex.reg_buf = curbuf;	// always works on the current buffer!
    rex.reg_firstlnum = lnum;
    rex.reg_maxline = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା - lnum;
    rex.reg_ଧାଡ଼ି_lbr = FALSE;
    result = vim_regsub_both(source, ଖାଲି, dest, destlen, flags);

    rex_in_use = rex_in_use_save;
    ଯଦି (rex_in_use)
	rex = rex_save;

    ଫେରନ୍ତୁ result;
}

#ଯଦି defined(FEAT_EVAL)
// When nesting more than a couple levels it's probably a mistake.
# define MAX_REGSUB_NESTING 4
ସ୍ଥିର char_u   *eval_result[MAX_REGSUB_NESTING] = {ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି};

# ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
free_resub_eval_result(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ପାଇଁ (i = 0; i < MAX_REGSUB_NESTING; ++i)
	VIM_CLEAR(eval_result[i]);
}
# endif
#endif

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_regsub_both(
    char_u	*source,
    typval_T	*ପଦସମଷ୍ଟି,
    char_u	*dest,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		destlen,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)
{
    char_u	*src;
    char_u	*dst;
    char_u	*s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		no = -1;
    fptr_T	func_all = (fptr_T)ଖାଲି;
    fptr_T	func_one = (fptr_T)ଖାଲି;
    linenr_T	clnum = 0;	// init for GCC
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 0;	// init for GCC
#ifdef FEAT_EVAL
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  nesting = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nested;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		copy = flags & REGSUB_COPY;

    // Be paranoid...
    ଯଦି ((source == ଖାଲି && ପଦସମଷ୍ଟି == ଖାଲି) || dest == ଖାଲି)
    {
	iemsg(e_null_argument);
	ଫେରନ୍ତୁ 0;
    }
    ଯଦି (prog_magic_wrong())
	ଫେରନ୍ତୁ 0;
#ifdef FEAT_EVAL
    ଯଦି (nesting == MAX_REGSUB_NESTING)
    {
	emsg(_(e_substitute_nesting_too_deep));
	ଫେରନ୍ତୁ 0;
    }
    nested = nesting;
#endif
    src = source;
    dst = dest;

    /*
     * When the substitute part starts with "\=" evaluate it as an expression.
     */
    ଯଦି (ପଦସମଷ୍ଟି != ଖାଲି || (source[0] == '\\' && source[1] == '='))
    {
#ifdef FEAT_EVAL
	// To make sure that the length doesn't change between checking the
	// length and copying the string, and to speed up things, the
	// resulting string is saved from the call with
	// "flags & REGSUB_COPY" == 0 to the call with
	// "flags & REGSUB_COPY" != 0.
	ଯଦି (copy)
	{
	    ଯଦି (eval_result[nested] != ଖାଲି)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା eval_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(eval_result[nested]);

		ଯଦି (eval_len < destlen)
		{
		    STRCPY(dest, eval_result[nested]);
		    dst += eval_len;
		    VIM_CLEAR(eval_result[nested]);
		}
	    }
	}
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    prev_can_f_submatch = can_f_submatch;
	    regsubmatch_T   rsm_save;

	    VIM_CLEAR(eval_result[nested]);

	    // The expression may contain substitute(), which calls us
	    // recursively.  Make sure submatch() gets the text from the first
	    // level.
	    ଯଦି (can_f_submatch)
		rsm_save = rsm;
	    can_f_submatch = TRUE;
	    rsm.sm_match = rex.reg_match;
	    rsm.sm_mmatch = rex.reg_mmatch;
	    rsm.sm_firstlnum = rex.reg_firstlnum;
	    rsm.sm_maxline = rex.reg_maxline;
	    rsm.sm_ଧାଡ଼ି_lbr = rex.reg_ଧାଡ଼ି_lbr;

	    // Although unlikely, it is possible that the expression invokes a
	    // substitute command (it might fail, but still).  Therefore keep
	    // an array of eval results.
	    ++nesting;

	    ଯଦି (ପଦସମଷ୍ଟି != ଖାଲି)
	    {
		typval_T	argv[2];
		char_u		buf[NUMBUFLEN];
		typval_T	rettv;
		staticList10_T	matchList;
		funcexe_T	funcexe;

		rettv.v_type = VAR_STRING;
		rettv.vval.v_string = ଖାଲି;
		argv[0].v_type = VAR_LIST;
		argv[0].vval.v_list = &matchList.sl_list;
		matchList.sl_list.lv_len = 0;
		CLEAR_FIELD(funcexe);
		funcexe.fe_argv_func = fill_submatch_list;
		funcexe.fe_evaluate = TRUE;
		ଯଦି (ପଦସମଷ୍ଟି->v_type == VAR_FUNC)
		{
		    s = ପଦସମଷ୍ଟି->vval.v_string;
		    call_func(s, -1, &rettv, 1, argv, &funcexe);
		}
		ଅନ୍ୟଥା ଯଦି (ପଦସମଷ୍ଟି->v_type == VAR_PARTIAL)
		{
		    partial_T   *partial = ପଦସମଷ୍ଟି->vval.v_partial;

		    s = partial_name(partial);
		    funcexe.fe_partial = partial;
		    call_func(s, -1, &rettv, 1, argv, &funcexe);
		}
		ଅନ୍ୟଥା ଯଦି (ପଦସମଷ୍ଟି->v_type == VAR_INSTR)
		{
		    exe_typval_instr(ପଦସମଷ୍ଟି, &rettv);
		}
		ଯଦି (matchList.sl_list.lv_len > 0)
		    // fill_submatch_list() was called
		    clear_submatch_list(&matchList);

		ଯଦି (rettv.v_type == VAR_UNKNOWN)
		    // something failed, no need to report another error
		    eval_result[nested] = ଖାଲି;
		ଅନ୍ୟଥା
		{
		    eval_result[nested] = tv_get_string_buf_chk(&rettv, buf);
		    ଯଦି (eval_result[nested] != ଖାଲି)
			eval_result[nested] = vim_strsave(eval_result[nested]);
		}
		clear_tv(&rettv);
	    }
	    ଅନ୍ୟଥା ଯଦି (substitute_instr != ଖାଲି)
		// Execute instructions from ISN_SUBSTITUTE.
		eval_result[nested] = exe_substitute_instr();
	    ଅନ୍ୟଥା
		eval_result[nested] = eval_to_string(source + 2, TRUE, FALSE);
	    --nesting;

	    ଯଦି (eval_result[nested] != ଖାଲି)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା had_backslash = FALSE;

		ପାଇଁ (s = eval_result[nested]; *s != NUL; MB_PTR_ADV(s))
		{
		    // Change NL to CR, so that it becomes a line break,
		    // unless called from vim_regexec_nl().
		    // Skip over a backslashed character.
		    ଯଦି (*s == NL && !rsm.sm_ଧାଡ଼ି_lbr)
			*s = CAR;
		    ଅନ୍ୟଥା ଯଦି (*s == '\\' && s[1] != NUL)
		    {
			++s;
			/* Change NL to CR here too, so that this works:
			 * :s/abc\\\ndef/\="aaa\\\nbbb"/  on text:
			 *   abc\
			 *   def
			 * Not when called from vim_regexec_nl().
			 */
			ଯଦି (*s == NL && !rsm.sm_ଧାଡ଼ି_lbr)
			    *s = CAR;
			had_backslash = TRUE;
		    }
		}
		ଯଦି (had_backslash && (flags & REGSUB_BACKSLASH))
		{
		    // Backslashes will be consumed, need to double them.
		    s = vim_strsave_escaped(eval_result[nested], (char_u *)"\\");
		    ଯଦି (s != ଖାଲି)
		    {
			vim_free(eval_result[nested]);
			eval_result[nested] = s;
		    }
		}

		dst += STRLEN(eval_result[nested]);
	    }

	    can_f_submatch = prev_can_f_submatch;
	    ଯଦି (can_f_submatch)
		rsm = rsm_save;
	}
#endif
    }
    ଅନ୍ୟଥା
      ଯେତେବେଳେ ((c = *src++) != NUL)
      {
	ଯଦି (c == '&' && (flags & REGSUB_MAGIC))
	    no = 0;
	ଅନ୍ୟଥା ଯଦି (c == '\\' && *src != NUL)
	{
	    ଯଦି (*src == '&' && !(flags & REGSUB_MAGIC))
	    {
		++src;
		no = 0;
	    }
	    ଅନ୍ୟଥା ଯଦି ('0' <= *src && *src <= '9')
	    {
		no = *src++ - '0';
	    }
	    ଅନ୍ୟଥା ଯଦି (vim_strchr((char_u *)"uUlLeE", *src))
	    {
		ବାଛ (*src++)
		{
		ସ୍ଥିତି 'u':   func_one = do_upper;
			    ଜାରି;
		ସ୍ଥିତି 'U':   func_all = do_upper;
			    ଜାରି;
		ସ୍ଥିତି 'l':   func_one = do_lower;
			    ଜାରି;
		ସ୍ଥିତି 'L':   func_all = do_lower;
			    ଜାରି;
		ସ୍ଥିତି 'e':
		ସ୍ଥିତି 'E':   func_one = func_all = (fptr_T)ଖାଲି;
			    ଜାରି;
		}
	    }
	}
	ଯଦି (no < 0)	      // Ordinary character.
	{
	    ଯଦି (c == K_SPECIAL && src[0] != NUL && src[1] != NUL)
	    {
		// Copy a special key as-is.
		ଯଦି (copy)
		{
		    ଯଦି (dst + 3 > dest + destlen)
		    {
			iemsg("vim_regsub_both(): not enough space");
			ଫେରନ୍ତୁ 0;
		    }
		    *dst++ = c;
		    *dst++ = *src++;
		    *dst++ = *src++;
		}
		ଅନ୍ୟଥା
		{
		    dst += 3;
		    src += 2;
		}
		ଜାରି;
	    }

	    ଯଦି (c == '\\' && *src != NUL)
	    {
		// Check for abbreviations -- webb
		ବାଛ (*src)
		{
		    ସ୍ଥିତି 'r':	c = CAR;	++src;	ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'n':	c = NL;		++src;	ଭାଙ୍ଗ;
		    ସ୍ଥିତି 't':	c = TAB;	++src;	ଭାଙ୍ଗ;
		 // Oh no!  \e already has meaning in subst pat :-(
		 // case 'e':   c = ESC;	++src;	break;
		    ସ୍ଥିତି 'b':	c = Ctrl_H;	++src;	ଭାଙ୍ଗ;

		    // If "backslash" is TRUE the backslash will be removed
		    // later.  Used to insert a literal CR.
		    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:	ଯଦି (flags & REGSUB_BACKSLASH)
				{
				    ଯଦି (copy)
				    {
					ଯଦି (dst + 1 > dest + destlen)
					{
					    iemsg("vim_regsub_both(): not enough space");
					    ଫେରନ୍ତୁ 0;
					}
					*dst = '\\';
				    }
				    ++dst;
				}
				c = *src++;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (has_mbyte)
		c = mb_ptr2char(src - 1);

	    // Write to buffer, if copy is set.
	    ଯଦି (func_one != (fptr_T)ଖାଲି)
	    {
		func_one(&cc, c);
		func_one = ଖାଲି;
	    }
	    ଅନ୍ୟଥା ଯଦି (func_all != (fptr_T)ଖାଲି)
		func_all(&cc, c);
	    ଅନ୍ୟଥା // just copy
		cc = c;

	    ଯଦି (has_mbyte)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା totlen = mb_ptr2len(src - 1);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା charlen = mb_char2len(cc);

		ଯଦି (copy)
		{
		    ଯଦି (dst + charlen > dest + destlen)
		    {
			iemsg("vim_regsub_both(): not enough space");
			ଫେରନ୍ତୁ 0;
		    }
		    mb_char2bytes(cc, dst);
		}
		dst += charlen - 1;
		ଯଦି (enc_utf8)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା clen = utf_ptr2len(src - 1);

		    // If the character length is shorter than "totlen", there
		    // are composing characters; copy them as-is.
		    ଯଦି (clen < totlen)
		    {
			ଯଦି (copy)
			{
			    ଯଦି (dst + totlen - clen > dest + destlen)
			    {
				iemsg("vim_regsub_both(): not enough space");
				ଫେରନ୍ତୁ 0;
			    }
			    mch_memmove(dst + 1, src - 1 + clen,
						     (size_t)(totlen - clen));
			}
			dst += totlen - clen;
		    }
		}
		src += totlen - 1;
	    }
	    ଅନ୍ୟଥା ଯଦି (copy)
	    {
		ଯଦି (dst + 1 > dest + destlen)
		{
		    iemsg("vim_regsub_both(): not enough space");
		    ଫେରନ୍ତୁ 0;
		}
		*dst = cc;
	    }
	    dst++;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (REG_MULTI)
	    {
		clnum = rex.reg_mmatch->startpos[no].lnum;
		ଯଦି (clnum < 0 || rex.reg_mmatch->endpos[no].lnum < 0)
		    s = ଖାଲି;
		ଅନ୍ୟଥା
		{
		    s = reg_getline(clnum) + rex.reg_mmatch->startpos[no].col;
		    ଯଦି (rex.reg_mmatch->endpos[no].lnum == clnum)
			len = rex.reg_mmatch->endpos[no].col
					    - rex.reg_mmatch->startpos[no].col;
		    ଅନ୍ୟଥା
			len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)reg_getline_len(clnum) - rex.reg_mmatch->startpos[no].col;
		}
	    }
	    ଅନ୍ୟଥା
	    {
		s = rex.reg_match->startp[no];
		ଯଦି (rex.reg_match->endp[no] == ଖାଲି)
		    s = ଖାଲି;
		ଅନ୍ୟଥା
		    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(rex.reg_match->endp[no] - s);
	    }
	    ଯଦି (s != ଖାଲି)
	    {
		ପାଇଁ (;;)
		{
		    ଯଦି (len == 0)
		    {
			ଯଦି (REG_MULTI)
			{
			    ଯଦି (rex.reg_mmatch->endpos[no].lnum == clnum)
				ଭାଙ୍ଗ;
			    ଯଦି (copy)
			    {
				ଯଦି (dst + 1 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    ଫେରନ୍ତୁ 0;
				}
				*dst = CAR;
			    }
			    ++dst;
			    s = reg_getline(++clnum);
			    ଯଦି (rex.reg_mmatch->endpos[no].lnum == clnum)
				len = rex.reg_mmatch->endpos[no].col;
			    ଅନ୍ୟଥା
				len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)reg_getline_len(clnum);
			}
			ଅନ୍ୟଥା
			    ଭାଙ୍ଗ;
		    }
		    ଅନ୍ୟଥା ଯଦି (*s == NUL) // we hit NUL.
		    {
			ଯଦି (copy)
			    iemsg(e_damaged_match_string);
			ଯାଅ exit;
		    }
		    ଅନ୍ୟଥା
		    {
			ଯଦି ((flags & REGSUB_BACKSLASH)
						  && (*s == CAR || *s == '\\'))
			{
			    /*
			     * Insert a backslash in front of a CR, otherwise
			     * it will be replaced by a line break.
			     * Number of backslashes will be halved later,
			     * double them here.
			     */
			    ଯଦି (copy)
			    {
				ଯଦି (dst + 2 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    ଫେରନ୍ତୁ 0;
				}
				dst[0] = '\\';
				dst[1] = *s;
			    }
			    dst += 2;
			}
			ଅନ୍ୟଥା
			{
			    ଯଦି (has_mbyte)
				c = mb_ptr2char(s);
			    ଅନ୍ୟଥା
				c = *s;

			    ଯଦି (func_one != (fptr_T)ଖାଲି)
			    {
				func_one(&cc, c);
				func_one = ଖାଲି;
			    }
			    ଅନ୍ୟଥା ଯଦି (func_all != (fptr_T)ଖାଲି)
				func_all(&cc, c);
			    ଅନ୍ୟଥା // just copy
				cc = c;

			    ଯଦି (has_mbyte)
			    {
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା l;
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା charlen;

				// Copy composing characters separately, one
				// at a time.
				ଯଦି (enc_utf8)
				    l = utf_ptr2len(s) - 1;
				ଅନ୍ୟଥା
				    l = mb_ptr2len(s) - 1;

				s += l;
				len -= l;
				charlen = mb_char2len(cc);
				ଯଦି (copy)
				{
				    ଯଦି (dst + charlen > dest + destlen)
				    {
					iemsg("vim_regsub_both(): not enough space");
					ଫେରନ୍ତୁ 0;
				    }
				    mb_char2bytes(cc, dst);
				}
				dst += charlen - 1;
			    }
			    ଅନ୍ୟଥା ଯଦି (copy)
			    {
				ଯଦି (dst + 1 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    ଫେରନ୍ତୁ 0;
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
    ଯଦି (copy)
	*dst = NUL;

exit:
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((dst - dest) + 1);
}

#ifdef FEAT_EVAL

    ସ୍ଥିର char_u *
reg_getline_submatch(linenr_T lnum)
{
    char_u *ଧାଡ଼ି;

    reg_getline_common(lnum, RGLF_ଧାଡ଼ି | RGLF_SUBMATCH, &ଧାଡ଼ି, ଖାଲି);

    ଫେରନ୍ତୁ ଧାଡ଼ି;
}

    ସ୍ଥିର colnr_T
reg_getline_submatch_len(linenr_T lnum)
{
    colnr_T length;

    reg_getline_common(lnum, RGLF_LENGTH | RGLF_SUBMATCH, ଖାଲି, &length);

    ଫେରନ୍ତୁ length;
}

/*
 * Used for the submatch() function: get the string from the n'th submatch in
 * allocated memory.
 * Returns NULL when not in a ":s" command and for a non-existing submatch.
 */
    char_u *
reg_submatch(ପୂର୍ଣ୍ଣସଂଖ୍ୟା no)
{
    char_u	*retval = ଖାଲି;
    char_u	*s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		round;
    linenr_T	lnum;

    ଯଦି (!can_f_submatch || no < 0)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (rsm.sm_match == ଖାଲି)
    {
	/*
	 * First round: compute the length and allocate memory.
	 * Second round: copy the text.
	 */
	ପାଇଁ (round = 1; round <= 2; ++round)
	{
	    lnum = rsm.sm_mmatch->startpos[no].lnum;
	    ଯଦି (lnum < 0 || rsm.sm_mmatch->endpos[no].lnum < 0)
		ଫେରନ୍ତୁ ଖାଲି;

	    s = reg_getline_submatch(lnum);
	    ଯଦି (s == ଖାଲି)  // anti-crash check, cannot happen?
		ଭାଙ୍ଗ;
	    s += rsm.sm_mmatch->startpos[no].col;
	    ଯଦି (rsm.sm_mmatch->endpos[no].lnum == lnum)
	    {
		// Within one line: take form start to end col.
		len = rsm.sm_mmatch->endpos[no].col
					  - rsm.sm_mmatch->startpos[no].col;
		ଯଦି (round == 2)
		    vim_strncpy(retval, s, len);
		++len;
	    }
	    ଅନ୍ୟଥା
	    {
		// Multiple lines: take start line from start col, middle
		// lines completely and end line up to end col.
		len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)reg_getline_submatch_len(lnum) - rsm.sm_mmatch->startpos[no].col;
		ଯଦି (round == 2)
		{
		    STRCPY(retval, s);
		    retval[len] = '\n';
		}
		++len;
		++lnum;
		ଯେତେବେଳେ (lnum < rsm.sm_mmatch->endpos[no].lnum)
		{
		    s = reg_getline_submatch(lnum);
		    ଯଦି (round == 2)
			STRCPY(retval + len, s);
		    len += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)reg_getline_submatch_len(lnum);
		    ଯଦି (round == 2)
			retval[len] = '\n';
		    ++len;
		    ++lnum;
		}
		ଯଦି (round == 2)
		    STRNCPY(retval + len, reg_getline_submatch(lnum),
					     rsm.sm_mmatch->endpos[no].col);
		len += rsm.sm_mmatch->endpos[no].col;
		ଯଦି (round == 2)
		    retval[len] = NUL;
		++len;
	    }

	    ଯଦି (retval == ଖାଲି)
	    {
		retval = alloc(len);
		ଯଦି (retval == ଖାଲି)
		    ଫେରନ୍ତୁ ଖାଲି;
	    }
	}
    }
    ଅନ୍ୟଥା
    {
	s = rsm.sm_match->startp[no];
	ଯଦି (s == ଖାଲି || rsm.sm_match->endp[no] == ଖାଲି)
	    retval = ଖାଲି;
	ଅନ୍ୟଥା
	    retval = vim_strnsave(s, rsm.sm_match->endp[no] - s);
    }

    ଫେରନ୍ତୁ retval;
}

/*
 * Used for the submatch() function with the optional non-zero argument: get
 * the list of strings from the n'th submatch in allocated memory with NULs
 * represented in NLs.
 * Returns a list of allocated strings.  Returns NULL when not in a ":s"
 * command, for a non-existing submatch and for any error.
 */
    list_T *
reg_submatch_list(ପୂର୍ଣ୍ଣସଂଖ୍ୟା no)
{
    char_u	*s;
    linenr_T	slnum;
    linenr_T	elnum;
    colnr_T	scol;
    colnr_T	ecol;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    list_T	*list;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

    ଯଦି (!can_f_submatch || no < 0)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (rsm.sm_match == ଖାଲି)
    {
	slnum = rsm.sm_mmatch->startpos[no].lnum;
	elnum = rsm.sm_mmatch->endpos[no].lnum;
	ଯଦି (slnum < 0 || elnum < 0)
	    ଫେରନ୍ତୁ ଖାଲି;

	scol = rsm.sm_mmatch->startpos[no].col;
	ecol = rsm.sm_mmatch->endpos[no].col;

	list = list_alloc();
	ଯଦି (list == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;

	s = reg_getline_submatch(slnum) + scol;
	ଯଦି (slnum == elnum)
	{
	    ଯଦି (list_append_string(list, s, ecol - scol) == FAIL)
		error = TRUE;
	}
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_lnum = elnum - slnum;

	    ଯଦି (list_append_string(list, s, -1) == FAIL)
		error = TRUE;
	    ପାଇଁ (i = 1; i < max_lnum; i++)
	    {
		s = reg_getline_submatch(slnum + i);
		ଯଦି (list_append_string(list, s, -1) == FAIL)
		    error = TRUE;
	    }
	    s = reg_getline_submatch(elnum);
	    ଯଦି (list_append_string(list, s, ecol) == FAIL)
		error = TRUE;
	}
    }
    ଅନ୍ୟଥା
    {
	s = rsm.sm_match->startp[no];
	ଯଦି (s == ଖାଲି || rsm.sm_match->endp[no] == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	list = list_alloc();
	ଯଦି (list == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	ଯଦି (list_append_string(list, s,
				 (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(rsm.sm_match->endp[no] - s)) == FAIL)
	    error = TRUE;
    }

    ଯଦି (error)
    {
	list_free(list);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ++list->lv_refcount;
    ଫେରନ୍ତୁ list;
}
#endif

/*
 * Initialize the values used for matching against multiple lines
 */
    ସ୍ଥିର ଶୂନ୍ୟ
init_regexec_multi(
	regmmatch_T	*rmp,
	win_T		*win,	// window in which to search or NULL
	buf_T		*buf,	// buffer in which to search
	linenr_T	lnum)	// nr of line to start looking for match
{
    rex.reg_match = ଖାଲି;
    rex.reg_mmatch = rmp;
    rex.reg_buf = buf;
    rex.reg_win = win;
    rex.reg_firstlnum = lnum;
    rex.reg_maxline = rex.reg_buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା - lnum;
    rex.reg_ଧାଡ଼ି_lbr = FALSE;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_char_class_bits(char_u *start, char_u *end, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *newl)
{
    char_u	*p = start;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		config = 0;

    *newl = FALSE;
    ଯଦି (*end != ']')
	ଫେରନ୍ତୁ -1;
    ଯଦି (*p == '^')
    {
	config |= CLASS_not;
	p++;
    }

    ଯେତେବେଳେ (p < end)
    {
	ଯଦି (p + 2 < end && *(p + 1) == '-')
	{
	    ବାଛ (*p)
	    {
		ସ୍ଥିତି '0':
		    ଯଦି (*(p + 2) == '9')
		    {
			config |= CLASS_o9;
			ଭାଙ୍ଗ;
		    }
		    ଯଦି (*(p + 2) == '7')
		    {
			config |= CLASS_o7;
			ଭାଙ୍ଗ;
		    }
		    ଫେରନ୍ତୁ -1;

		ସ୍ଥିତି 'a':
		    ଯଦି (*(p + 2) == 'z')
		    {
			config |= CLASS_az;
			ଭାଙ୍ଗ;
		    }
		    ଯଦି (*(p + 2) == 'f')
		    {
			config |= CLASS_af;
			ଭାଙ୍ଗ;
		    }
		    ଫେରନ୍ତୁ -1;

		ସ୍ଥିତି 'A':
		    ଯଦି (*(p + 2) == 'Z')
		    {
			config |= CLASS_AZ;
			ଭାଙ୍ଗ;
		    }
		    ଯଦି (*(p + 2) == 'F')
		    {
			config |= CLASS_AF;
			ଭାଙ୍ଗ;
		    }
		    ଫେରନ୍ତୁ -1;

		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		    ଫେରନ୍ତୁ -1;
	    }
	    p += 3;
	}
	ଅନ୍ୟଥା ଯଦି (p + 1 < end && *p == '\\' && *(p + 1) == 'n')
	{
	    *newl = TRUE;
	    p += 2;
	}
	ଅନ୍ୟଥା ଯଦି (*p == '_')
	{
	    config |= CLASS_underscore;
	    p++;
	}
	ଅନ୍ୟଥା ଯଦି (*p == '\n')
	{
	    *newl = TRUE;
	    p++;
	}
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ -1;
    } // while (p < end)

    ଯଦି (p != end)
	ଫେରନ୍ତୁ -1;

    // U+017F and U+212A fold to ASCII 's' and 'k', missed by the class opcodes
    ଯଦି (enc_utf8 && (config & (CLASS_az | CLASS_AZ)))
	ଫେରନ୍ତୁ -1;

    ଫେରନ୍ତୁ config;
}

#ଅନ୍ତର୍ଭାଵ "regexp_bt.c"

ସ୍ଥିର regengine_T bt_regengine =
{
    bt_regcomp,
    bt_regfree,
    bt_regexec_nl,
    bt_regexec_multi
#ifdef DEBUG
    ,(char_u *)""
#endif
};

#ଅନ୍ତର୍ଭାଵ "regexp_nfa.c"

ସ୍ଥିର regengine_T nfa_regengine =
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା regexp_engine = 0;

#ifdef DEBUG
ସ୍ଥିର ଅଚଳ char_u regname[][30] = {
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
vim_regcomp(char_u *ପଦସମଷ୍ଟି_arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା re_flags)
{
    regprog_T   *prog = ଖାଲି;
    char_u	*ପଦସମଷ୍ଟି = ପଦସମଷ୍ଟି_arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		called_emsg_before;

    regexp_engine = p_re;

    // Check for prefix "\%#=", that sets the regexp engine
    ଯଦି (STRNCMP(ପଦସମଷ୍ଟି, "\\%#=", 4) == 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା newengine = ପଦସମଷ୍ଟି[4] - '0';

	ଯଦି (newengine == AUTOMATIC_ENGINE
	    || newengine == BACKTRACKING_ENGINE
	    || newengine == NFA_ENGINE)
	{
	    regexp_engine = ପଦସମଷ୍ଟି[4] - '0';
	    ପଦସମଷ୍ଟି += 5;
#ifdef DEBUG
	    smsg("New regexp mode selected (%d): %s",
					   regexp_engine, regname[newengine]);
#endif
	}
	ଅନ୍ୟଥା
	{
	    emsg(_(e_percent_hash_can_only_be_followed_by_zero_one_two_automatic_engine_will_be_used));
	    regexp_engine = AUTOMATIC_ENGINE;
	}
    }
#ifdef DEBUG
    bt_regengine.ପଦସମଷ୍ଟି = ପଦସମଷ୍ଟି;
    nfa_regengine.ପଦସମଷ୍ଟି = ପଦସମଷ୍ଟି;
#endif
    // reg_iswordc() uses rex.reg_buf
    rex.reg_buf = curbuf;

    /*
     * First try the NFA engine, unless backtracking was requested.
     */
    called_emsg_before = called_emsg;
    ଯଦି (regexp_engine != BACKTRACKING_ENGINE)
	prog = nfa_regengine.regcomp(ପଦସମଷ୍ଟି,
		re_flags + (regexp_engine == AUTOMATIC_ENGINE ? RE_AUTO : 0));
    ଅନ୍ୟଥା
	prog = bt_regengine.regcomp(ପଦସମଷ୍ଟି, re_flags);

    // Check for error compiling regexp with initial engine.
    ଯଦି (prog == ଖାଲି)
    {
#ifdef BT_REGEXP_DEBUG_LOG
	ଯଦି (regexp_engine == BACKTRACKING_ENGINE)   // debugging log for BT engine
	{
	    FILE *f;
	    f = fopen(BT_REGEXP_DEBUG_LOG_NAME, "a");
	    ଯଦି (f)
	    {
		fprintf(f, "Syntax error in \"%s\"\n", ପଦସମଷ୍ଟି);
		fclose(f);
	    }
	    ଅନ୍ୟଥା
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
	ଯଦି (regexp_engine == AUTOMATIC_ENGINE
					  && called_emsg == called_emsg_before)
	{
	    regexp_engine = BACKTRACKING_ENGINE;
#ifdef FEAT_EVAL
	    report_re_switch(ପଦସମଷ୍ଟି);
#endif
	    prog = bt_regengine.regcomp(ପଦସମଷ୍ଟି, re_flags);
	}
    }

    ଯଦି (prog != ଖାଲି)
    {
	// Store the info needed to call regcomp() again when the engine turns
	// out to be very slow when executing it.
	prog->re_engine = regexp_engine;
	prog->re_flags  = re_flags;
    }

    ଫେରନ୍ତୁ prog;
}

/*
 * Free a compiled regexp program, returned by vim_regcomp().
 */
    ଶୂନ୍ୟ
vim_regfree(regprog_T *prog)
{
    ଯଦି (prog != ଖାଲି)
	prog->engine->regfree(prog);
}

#ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
free_regexp_stuff(ଶୂନ୍ୟ)
{
    ga_clear(&regstack);
    ga_clear(&backpos);
    vim_free(reg_tofree);
    vim_free(reg_prev_sub);
    vim_free(post_start);   // NFA postfix buffer, reused across compilations
    post_start = ଖାଲି;
    post_start_len = 0;
    vim_free(nfa_ଗଦା);    // NFA fragment stack, reused across compilations
    nfa_ଗଦା = ଖାଲି;
    nfa_ଗଦା_len = 0;
}
#endif

#ifdef FEAT_EVAL
    ସ୍ଥିର ଶୂନ୍ୟ
report_re_switch(char_u *pat)
{
    ଯଦି (p_verbose > 0)
    {
	verbose_enter();
	ସନ୍ଦେଶ_puts(_("Switching to backtracking RE engine for pattern: "));
	ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)pat);
	verbose_leave();
    }
}
#endif

#ଯଦି defined(FEAT_X11)
/*
 * Return whether "prog" is currently being executed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
regprog_in_use(regprog_T *prog)
{
    ଫେରନ୍ତୁ prog->re_in_use;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_regexec_string(
    regmatch_T	*rmp,
    char_u	*ଧାଡ଼ି,  // string to match against
    colnr_T	col,    // column to start looking for match
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nl)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result;
    regexec_T	rex_save;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		rex_in_use_save = rex_in_use;

    // Cannot use the same prog recursively, it contains state.
    ଯଦି (rmp->regprog->re_in_use)
    {
	emsg(_(e_cannot_use_pattern_recursively));
	ଫେରନ୍ତୁ FALSE;
    }
    rmp->regprog->re_in_use = TRUE;

    ଯଦି (rex_in_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_in_use = TRUE;

    rex.reg_startp = ଖାଲି;
    rex.reg_endp = ଖାଲି;
    rex.reg_startpos = ଖାଲି;
    rex.reg_endpos = ଖାଲି;

    result = rmp->regprog->engine->regexec_nl(rmp, ଧାଡ଼ି, col, nl);
    rmp->regprog->re_in_use = FALSE;

    // NFA engine aborted because it's very slow.
    ଯଦି (rmp->regprog->re_engine == AUTOMATIC_ENGINE
					       && result == NFA_TOO_EXPENSIVE)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା    save_p_re = p_re;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା    re_flags = rmp->regprog->re_flags;
	char_u *pat = vim_strsave(((nfa_regprog_T *)rmp->regprog)->pattern);

	p_re = BACKTRACKING_ENGINE;
	ଯଦି (pat != ଖାଲି)
	{
	    regprog_T *prev_prog = rmp->regprog;

#ifdef FEAT_EVAL
	    report_re_switch(pat);
#endif
	    rmp->regprog = vim_regcomp(pat, re_flags);
	    ଯଦି (rmp->regprog == ଖାଲି)
	    {
		// Somehow compiling the pattern failed now, put back the
		// previous one to avoid "regprog" becoming NULL.
		rmp->regprog = prev_prog;
	    }
	    ଅନ୍ୟଥା
	    {
		vim_regfree(prev_prog);
		rmp->regprog->re_in_use = TRUE;
		result = rmp->regprog->engine->regexec_nl(rmp, ଧାଡ଼ି, col, nl);
		rmp->regprog->re_in_use = FALSE;
	    }
	    vim_free(pat);
	}

	p_re = save_p_re;
    }

    rex_in_use = rex_in_use_save;
    ଯଦି (rex_in_use)
	rex = rex_save;

    ଫେରନ୍ତୁ result > 0;
}

#ଯଦି defined(FEAT_SPELL) || defined(FEAT_EVAL) || defined(FEAT_X11)
/*
 * Note: "*prog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_regexec_prog(
    regprog_T	**prog,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ignore_case,
    char_u	*ଧାଡ଼ି,
    colnr_T	col)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;
    regmatch_T	regmatch;

    regmatch.regprog = *prog;
    regmatch.rm_ic = ignore_case;
    r = vim_regexec_string(&regmatch, ଧାଡ଼ି, col, FALSE);
    *prog = regmatch.regprog;
    ଫେରନ୍ତୁ r;
}
#endif

/*
 * Note: "rmp->regprog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_regexec(regmatch_T *rmp, char_u *ଧାଡ଼ି, colnr_T col)
{
    ଫେରନ୍ତୁ vim_regexec_string(rmp, ଧାଡ଼ି, col, FALSE);
}

/*
 * Like vim_regexec(), but consider a "\n" in "line" to be a line break.
 * Note: "rmp->regprog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_regexec_nl(regmatch_T *rmp, char_u *ଧାଡ଼ି, colnr_T col)
{
    ଫେରନ୍ତୁ vim_regexec_string(rmp, ଧାଡ଼ି, col, TRUE);
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*timed_out)	// flag is set when timeout limit reached
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result;
    regexec_T	rex_save;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		rex_in_use_save = rex_in_use;

    // Cannot use the same prog recursively, it contains state.
    ଯଦି (rmp->regprog->re_in_use)
    {
	emsg(_(e_cannot_use_pattern_recursively));
	ଫେରନ୍ତୁ FALSE;
    }
    rmp->regprog->re_in_use = TRUE;

    ଯଦି (rex_in_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_in_use = TRUE;

    result = rmp->regprog->engine->regexec_multi(
				      rmp, win, buf, lnum, col, timed_out);
    rmp->regprog->re_in_use = FALSE;

    // NFA engine aborted because it's very slow.
    ଯଦି (rmp->regprog->re_engine == AUTOMATIC_ENGINE
					       && result == NFA_TOO_EXPENSIVE)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା    save_p_re = p_re;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା    re_flags = rmp->regprog->re_flags;
	char_u *pat = vim_strsave(((nfa_regprog_T *)rmp->regprog)->pattern);

	p_re = BACKTRACKING_ENGINE;
	ଯଦି (pat != ଖାଲି)
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
	    ଯଦି (rmp->regprog == ଖାଲି)
	    {
		// Somehow compiling the pattern failed now, put back the
		// previous one to avoid "regprog" becoming NULL.
		rmp->regprog = prev_prog;
	    }
	    ଅନ୍ୟଥା
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
    ଯଦି (rex_in_use)
	rex = rex_save;

    ଫେରନ୍ତୁ result <= 0 ? 0 : result;
}
