/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * Handling of regular expressions: vim_regcomp(), vim_regexec(), vim_regsub()
 */

// By default: do not create debugging logs or files related to regular
// expressions, even when compiling with -DDEBUG.
// Uncomment the second line to get the regexp debugging.
#undef DEBUG
// #define DEBUG

#include "vim.h"

#ifdef DEBUG
// show/save debugging data when BT engine is used
# define BT_REGEXP_DUMP
// save the debugging data to a file instead of displaying it
# define BT_REGEXP_LOG
# define BT_REGEXP_DEBUG_LOG
# define BT_REGEXP_DEBUG_LOG_NAME	"bt_regexp_debug.log"
#endif

#ifdef FEAT_RELTIME
static sig_atomic_t dummy_timeout_flag = 0;
static ca_ncala sig_atomic_t *timeout_flag = &dummy_timeout_flag;
#endif

/*
 * Magic characters have a special meaning, they don't match literally.
 * Magic characters are negative.  This separates them from literal characters
 * (possibly multi-byte).  Only ASCII characters can be Magic.
 */
#define Magic(x)	((පූර්ණ_සංඛ්‍යා)(x) - 256)
#define un_Magic(x)	((x) + 256)
#define is_Magic(x)	((x) < 0)

    static පූර්ණ_සංඛ්‍යා
no_Magic(පූර්ණ_සංඛ්‍යා x)
{
    නම් (is_Magic(x))
	ආපසු un_Magic(x);
    ආපසු x;
}

    static පූර්ණ_සංඛ්‍යා
toggle_Magic(පූර්ණ_සංඛ්‍යා x)
{
    නම් (is_Magic(x))
	ආපසු un_Magic(x);
    ආපසු Magic(x);
}

#නම් defined(FEAT_RELTIME)
static පූර්ණ_සංඛ්‍යා timeout_nesting = 0;

/*
 * Start a timer that will cause the regexp to abort after "msec".
 * This doesn't work well recursively.  In case it happens anyway, the first
 * set timeout will prevail, nested ones are ignored.
 * The caller must make sure there is a matching disable_regexp_timeout() call!
 */
    void
init_regexp_timeout(long msec)
{
    නම් (timeout_nesting == 0)
	timeout_flag = start_timeout(msec);
    ++timeout_nesting;
}

    void
disable_regexp_timeout(void)
{
    නම් (timeout_nesting == 0)
	iemsg("disable_regexp_timeout() called without active timer");
    නැතහොත් නම් (--timeout_nesting == 0)
    {
	stop_timeout();
	timeout_flag = &dummy_timeout_flag;
    }
}
#endif

#නම් defined(FEAT_EVAL)
# ifdef FEAT_RELTIME
static sig_atomic_t *saved_timeout_flag;
# endif

/*
 * Used at the debug prompt: disable the timeout so that expression evaluation
 * can used patterns.
 * Must be followed by calling restore_timeout_for_debugging().
 */
    void
save_timeout_for_debugging(void)
{
# ifdef FEAT_RELTIME
    saved_timeout_flag = (sig_atomic_t *)timeout_flag;
    timeout_flag = &dummy_timeout_flag;
# endif
}

    void
restore_timeout_for_debugging(void)
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
#define UCHARAT(p)	((පූර්ණ_සංඛ්‍යා)*(char_u *)(p))

// Used for an error (down from) vim_regcomp(): give the error message, set
// rc_did_emsg and return NULL
#define EMSG_RET_NULL(m) ආපසු (emsg((m)), rc_did_emsg = TRUE, (void *)NULL)
#define IEMSG_RET_NULL(m) ආපසු (iemsg((m)), rc_did_emsg = TRUE, (void *)NULL)
#define EMSG_RET_FAIL(m) ආපසු (emsg((m)), rc_did_emsg = TRUE, FAIL)
#define EMSG2_RET_NULL(m, c) ආපසු (semsg((const var_na *)(m), (c) ? "" : "\\"), rc_did_emsg = TRUE, (void *)NULL)
#define EMSG3_RET_NULL(m, c, a) ආපසු (semsg((const var_na *)(m), (c) ? "" : "\\", (a)), rc_did_emsg = TRUE, (void *)NULL)
#define EMSG2_RET_FAIL(m, c) ආපසු (semsg((const var_na *)(m), (c) ? "" : "\\"), rc_did_emsg = TRUE, FAIL)
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
    static පූර්ණ_සංඛ්‍යා
re_multi_type(පූර්ණ_සංඛ්‍යා c)
{
    නම් (c == Magic('@') || c == Magic('=') || c == Magic('?'))
	ආපසු MULTI_ONE;
    නම් (c == Magic('*') || c == Magic('+') || c == Magic('{'))
	ආපසු MULTI_MULT;
    ආපසු NOT_MULTI;
}

static char_u		*reg_prev_sub = NULL;
static size_t		reg_prev_sublen = 0;

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
static char_u REGEXP_INRANGE[] = "]^-n\\";
static char_u REGEXP_ABBR[] = "nrtebdoxuU";

/*
 * Translate '\x' to its control character, except "\n", which is Magic.
 */
    static පූර්ණ_සංඛ්‍යා
backslash_trans(පූර්ණ_සංඛ්‍යා c)
{
    switch (c)
    {
	pak_sa 'r':   ආපසු CAR;
	pak_sa 't':   ආපසු TAB;
	pak_sa 'e':   ආපසු ESC;
	pak_sa 'b':   ආපසු BS;
    }
    ආපසු c;
}

ga_nanaa
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
    static පූර්ණ_සංඛ්‍යා
get_char_class(char_u **pp)
{
    // must be sorted by the 'value' field because it is used by bsearch()!
    static keyvalue_T char_class_tab[] =
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
    නම් ((*pp)[1] == ':' && ASCII_ISLOWER((*pp)[2])
			&& ASCII_ISLOWER((*pp)[3]) && ASCII_ISLOWER((*pp)[4]))
    {
	keyvalue_T target;
	keyvalue_T *entry;
	// this function can be called repeatedly with the same value for "pp"
	// so we cache the last found entry.
	static keyvalue_T *last_entry = NULL;

	target.key = 0;
	target.value.string = *pp + 2;
	target.value.length = 0;	// not used, see cmp_keyvalue_value_n()

	නම් (last_entry != NULL && cmp_keyvalue_value_n(&target, last_entry) == 0)
	    entry = last_entry;
	නැතහොත්
	    entry = (keyvalue_T *)bsearch(&target, &char_class_tab,
					ARRAY_LENGTH(char_class_tab),
					parimaa_na(char_class_tab[0]), cmp_keyvalue_value_n);
	නම් (entry != NULL)
	{
	    last_entry = entry;
	    *pp += entry->value.length + 2;
	    ආපසු entry->key;
	}
    }
    ආපසු CLASS_NONE;
}

/*
 * Specific version of character class functions.
 * Using a table to keep this fast.
 */
static short	class_tab[256];

#define	    RI_DIGIT	0x01
#define	    RI_HEX	0x02
#define	    RI_OCTAL	0x04
#define	    RI_WORD	0x08
#define	    RI_HEAD	0x10
#define	    RI_ALPHA	0x20
#define	    RI_LOWER	0x40
#define	    RI_UPPER	0x80
#define	    RI_WHITE	0x100

    static void
init_class_tab(void)
{
    පූර්ණ_සංඛ්‍යා		i;
    static පූර්ණ_සංඛ්‍යා	done = FALSE;

    නම් (done)
	ආපසු;

    සඳහා (i = 0; i < 256; ++i)
    {
	නම් (i >= '0' && i <= '7')
	    class_tab[i] = RI_DIGIT + RI_HEX + RI_OCTAL + RI_WORD;
	නැතහොත් නම් (i >= '8' && i <= '9')
	    class_tab[i] = RI_DIGIT + RI_HEX + RI_WORD;
	නැතහොත් නම් (i >= 'a' && i <= 'f')
	    class_tab[i] = RI_HEX + RI_WORD + RI_HEAD + RI_ALPHA + RI_LOWER;
	නැතහොත් නම් (i >= 'g' && i <= 'z')
	    class_tab[i] = RI_WORD + RI_HEAD + RI_ALPHA + RI_LOWER;
	නැතහොත් නම් (i >= 'A' && i <= 'F')
	    class_tab[i] = RI_HEX + RI_WORD + RI_HEAD + RI_ALPHA + RI_UPPER;
	නැතහොත් නම් (i >= 'G' && i <= 'Z')
	    class_tab[i] = RI_WORD + RI_HEAD + RI_ALPHA + RI_UPPER;
	නැතහොත් නම් (i == '_')
	    class_tab[i] = RI_WORD + RI_HEAD;
	නැතහොත්
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

static char_u	*regparse;	// Input-scan pointer.
static පූර්ණ_සංඛ්‍යා	regnpar;	// () count.
static පූර්ණ_සංඛ්‍යා	wants_nfa;	// regex should use NFA engine
#ifdef FEAT_SYN_HL
static පූර්ණ_සංඛ්‍යා	regnzpar;	// \z() count.
static පූර්ණ_සංඛ්‍යා	re_has_z;	// \z item detected
#endif
static unsigned	regflags;	// RF_ flags for prog
#නම් defined(FEAT_SYN_HL)
static පූර්ණ_සංඛ්‍යා	had_eol;	// TRUE when EOL found by vim_regcomp()
#endif

static magic_T	reg_magic;	// magicness of the pattern

static පූර්ණ_සංඛ්‍යා	reg_string;	// matching with a string instead of a buffer
				// line
static පූර්ණ_සංඛ්‍යා	reg_strict;	// "[abc" is illegal

/*
 * META contains all characters that may be magic, except '^' and '$'.
 */

// META[] is used often enough to justify turning it into a table.
static const char_u META_flags[] = {
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

static පූර්ණ_සංඛ්‍යා	curchr;		// currently parsed character
// Previous character.  Note: prevchr is sometimes -1 when we are not at the
// start, eg in /[ ^I]^ the pattern was never found even if it existed,
// because ^ was taken to be magic -- webb
static පූර්ණ_සංඛ්‍යා	prevchr;
static පූර්ණ_සංඛ්‍යා	prevprevchr;	// previous-previous character
static පූර්ණ_සංඛ්‍යා	nextchr;	// used for ungetchr()

// arguments for reg()
#define REG_NOPAREN	0	// toplevel reg()
#define REG_PAREN	1	// \(\)
#define REG_ZPAREN	2	// \z(\)
#define REG_NPAREN	3	// \%(\)

// Limit recursive parsing of nested regexp atoms to avoid using up the C stack.
#define REG_MAX_PAREN_DEPTH	1000

typedef sa_nracanaa
{
    char_u	*regparse;
    පූර්ණ_සංඛ්‍යා		prevchr_len;
    පූර්ණ_සංඛ්‍යා		curchr;
    පූර්ණ_සංඛ්‍යා		prevchr;
    පූර්ණ_සංඛ්‍යා		prevprevchr;
    පූර්ණ_සංඛ්‍යා		nextchr;
    පූර්ණ_සංඛ්‍යා		at_start;
    පූර්ණ_සංඛ්‍යා		prev_at_start;
    පූර්ණ_සංඛ්‍යා		regnpar;
} parse_state_T;

static void	initchr(char_u *);
static පූර්ණ_සංඛ්‍යා	getchr(void);
static void	skipchr_keepstart(void);
static පූර්ණ_සංඛ්‍යා	peekchr(void);
static void	skipchr(void);
static void	ungetchr(void);
static vimlong_T	gethexchrs(පූර්ණ_සංඛ්‍යා maxinputlen);
static long	getoctchrs(void);
static vimlong_T	getdecchrs(void);
static පූර්ණ_සංඛ්‍යා	coll_get_char(void);
static පූර්ණ_සංඛ්‍යා	prog_magic_wrong(void);
static පූර්ණ_සංඛ්‍යා	cstrncmp(char_u *s1, char_u *s2, පූර්ණ_සංඛ්‍යා *n);
static char_u	*cstrchr(char_u *, පූර්ණ_සංඛ්‍යා);
static පූර්ණ_සංඛ්‍යා	re_mult_next(var_na *what);
static පූර්ණ_සංඛ්‍යා	reg_iswordc(පූර්ණ_සංඛ්‍යා);
#ifdef FEAT_EVAL
static void report_re_switch(char_u *pat);
#endif

static regengine_T bt_regengine;
static regengine_T nfa_regengine;

/*
 * Return TRUE if compiled regular expression "prog" can match a line break.
 */
    පූර්ණ_සංඛ්‍යා
re_multiline(regprog_T *prog)
{
    ආපසු (prog->regflags & RF_HASNL);
}

/*
 * Check for an equivalence class name "[=a=]".  "pp" points to the '['.
 * Returns a character representing the class. Zero means that no item was
 * recognized.  Otherwise "pp" is advanced to after the item.
 */
    static පූර්ණ_සංඛ්‍යා
get_equi_class(char_u **pp)
{
    පූර්ණ_සංඛ්‍යා		c;
    පූර්ණ_සංඛ්‍යා		l = 1;
    char_u	*p = *pp;

    නම් (p[1] == '=' && p[2] != NUL)
    {
	නම් (has_mbyte)
	    l = (*mb_ptr2len)(p + 2);
	නම් (p[l + 2] == '=' && p[l + 3] == ']')
	{
	    නම් (has_mbyte)
		c = mb_ptr2char(p + 2);
	    නැතහොත්
		c = p[2];
	    *pp += l + 4;
	    ආපසු c;
	}
    }
    ආපසු 0;
}

/*
 * Check for a collating element "[.a.]".  "pp" points to the '['.
 * Returns a character. Zero means that no item was recognized.  Otherwise
 * "pp" is advanced to after the item.
 * Currently only single characters are recognized!
 */
    static පූර්ණ_සංඛ්‍යා
get_coll_element(char_u **pp)
{
    පූර්ණ_සංඛ්‍යා		c;
    පූර්ණ_සංඛ්‍යා		l = 1;
    char_u	*p = *pp;

    නම් (p[0] != NUL && p[1] == '.' && p[2] != NUL)
    {
	නම් (has_mbyte)
	    l = (*mb_ptr2len)(p + 2);
	නම් (p[l + 2] == '.' && p[l + 3] == ']')
	{
	    නම් (has_mbyte)
		c = mb_ptr2char(p + 2);
	    නැතහොත්
		c = p[2];
	    *pp += l + 4;
	    ආපසු c;
	}
    }
    ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා reg_cpo_lit; // 'cpoptions' contains 'l' flag
static පූර්ණ_සංඛ්‍යා reg_cpo_bsl; // 'cpoptions' contains '\' flag

    static void
get_cpo_flags(void)
{
    reg_cpo_lit = vim_strchr(p_cpo, CPO_LITERAL) != NULL;
    reg_cpo_bsl = vim_strchr(p_cpo, CPO_BACKSL) != NULL;
}

/*
 * Skip over a "[]" range.
 * "p" must point to the character after the '['.
 * The returned pointer is on the matching ']', or the terminating NUL.
 */
    static char_u *
skip_anyof(char_u *p)
{
    පූර්ණ_සංඛ්‍යා		l;

    නම් (*p == '^')	// Complement of range.
	++p;
    නම් (*p == ']' || *p == '-')
	++p;
    අතරතුර (*p != NUL && *p != ']')
    {
	නම් (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	    p += l;
	නැතහොත්
	    නම් (*p == '-')
	    {
		++p;
		නම් (*p != ']' && *p != NUL)
		    MB_PTR_ADV(p);
	    }
	නැතහොත් නම් (*p == '\\'
		&& !reg_cpo_bsl
		&& (vim_strchr(REGEXP_INRANGE, p[1]) != NULL
		    || (!reg_cpo_lit && vim_strchr(REGEXP_ABBR, p[1]) != NULL)))
	    p += 2;
	නැතහොත් නම් (*p == '[')
	{
	    නම් (get_char_class(&p) == CLASS_NONE
		    && get_equi_class(&p) == 0
		    && get_coll_element(&p) == 0
		    && *p != NUL)
		++p; // it is not a class name and not NUL
	}
	නැතහොත්
	    ++p;
    }

    ආපසු p;
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
    පූර්ණ_සංඛ්‍යා		delim,
    පූර්ණ_සංඛ්‍යා		magic)
{
    ආපසු skip_regexp_ex(startp, delim, magic, NULL, NULL, NULL);
}

/*
 * Call skip_regexp() and when the delimiter does not match give an error and
 * return NULL.
 */
    char_u *
skip_regexp_err(
    char_u	*startp,
    පූර්ණ_සංඛ්‍යා		delim,
    පූර්ණ_සංඛ්‍යා		magic)
{
    char_u *p = skip_regexp(startp, delim, magic);

    නම් (*p != delim)
    {
	semsg(_(e_missing_delimiter_after_search_pattern_str), startp);
	ආපසු NULL;
    }
    ආපසු p;
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
    පූර්ණ_සංඛ්‍යා		dirc,
    පූර්ණ_සංඛ්‍යා		magic,
    char_u	**newp,
    පූර්ණ_සංඛ්‍යා		*dropped,
    magic_T	*magic_val)
{
    magic_T	mymagic;
    char_u	*p = startp;
    size_t	startplen = 0;

    නම් (magic)
	mymagic = MAGIC_ON;
    නැතහොත්
	mymagic = MAGIC_OFF;
    get_cpo_flags();

    සඳහා (; p[0] != NUL; MB_PTR_ADV(p))
    {
	නම් (p[0] == dirc)	// found end of regexp
	    bha_nga;
	නම් ((p[0] == '[' && mymagic >= MAGIC_ON)
		|| (p[0] == '\\' && p[1] == '[' && mymagic <= MAGIC_OFF))
	{
	    p = skip_anyof(p + 1);
	    නම් (p[0] == NUL)
		bha_nga;
	}
	නැතහොත් නම් (p[0] == '\\' && p[1] != NUL)
	{
	    නම් (dirc == '?' && newp != NULL && p[1] == '?')
	    {
		// change "\?" to "?", make a copy first.
		නම් (startplen == 0)
		    startplen = STRLEN(startp);
		නම් (*newp == NULL)
		{
		    *newp = vim_strnsave(startp, startplen);
		    නම් (*newp != NULL)
		    {
			p = *newp + (p - startp);
			startp = *newp;
		    }
		}
		නම් (dropped != NULL)
		    ++*dropped;
		නම් (*newp != NULL)
		    mch_memmove(p, p + 1, startplen - ((p + 1) - startp) + 1);
		නැතහොත්
		    ++p;
	    }
	    නැතහොත්
		++p;    // skip next character
	    නම් (*p == 'v')
		mymagic = MAGIC_ALL;
	    නැතහොත් නම් (*p == 'V')
		mymagic = MAGIC_NONE;
	}
    }
    නම් (magic_val != NULL)
	*magic_val = mymagic;
    ආපසු p;
}

/*
 * Functions for getting characters from the regexp input.
 */
static පූර්ණ_සංඛ්‍යා	prevchr_len;	// byte length of previous char
static පූර්ණ_සංඛ්‍යා	at_start;	// True when on the first character
static පූර්ණ_සංඛ්‍යා	prev_at_start;  // True when on the second character

/*
 * Start parsing at "str".
 */
    static void
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
    static void
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
    static void
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
    static පූර්ණ_සංඛ්‍යා
peekchr(void)
{
    static පූර්ණ_සංඛ්‍යා	after_slash = FALSE;

    නම් (curchr != -1)
	ආපසු curchr;

    switch (curchr = regparse[0])
    {
	pak_sa '.':
	pak_sa '[':
	pak_sa '~':
	    // magic when 'magic' is on
	    නම් (reg_magic >= MAGIC_ON)
		curchr = Magic(curchr);
	    bha_nga;
	pak_sa '(':
	pak_sa ')':
	pak_sa '{':
	pak_sa '%':
	pak_sa '+':
	pak_sa '=':
	pak_sa '?':
	pak_sa '@':
	pak_sa '!':
	pak_sa '&':
	pak_sa '|':
	pak_sa '<':
	pak_sa '>':
	pak_sa '#':	// future ext.
	pak_sa '"':	// future ext.
	pak_sa '\'':	// future ext.
	pak_sa ',':	// future ext.
	pak_sa '-':	// future ext.
	pak_sa ':':	// future ext.
	pak_sa ';':	// future ext.
	pak_sa '`':	// future ext.
	pak_sa '/':	// Can't be used in / command
			// magic only after "\v"
	    නම් (reg_magic == MAGIC_ALL)
		curchr = Magic(curchr);
	    bha_nga;
	pak_sa '*':
	    // * is not magic as the very first character, eg "?*ptr", when
	    // after '^', eg "/^*ptr" and when after "\(", "\|", "\&".  But
	    // "\(\*" is not magic, thus must be magic if "after_slash"
	    නම් (reg_magic >= MAGIC_ON
		    && !at_start
		    && !(prev_at_start && prevchr == Magic('^'))
		    && (after_slash
			|| (prevchr != Magic('(')
			    && prevchr != Magic('&')
			    && prevchr != Magic('|'))))
		curchr = Magic('*');
	    bha_nga;
	pak_sa '^':
	    // '^' is only magic as the very first character and if it's after
	    // "\(", "\|", "\&' or "\n"
	    නම් (reg_magic >= MAGIC_OFF
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
	    bha_nga;
	pak_sa '$':
	    // '$' is only magic as the very last char and if it's in front of
	    // either "\|", "\)", "\&", or "\n"
	    නම් (reg_magic >= MAGIC_OFF)
	    {
		char_u *p = regparse + 1;
		පූර්ණ_සංඛ්‍යා is_magic_all = (reg_magic == MAGIC_ALL);

		// ignore \c \C \m \M \v \V and \Z after '$'
		අතරතුර (p[0] == '\\' && (p[1] == 'c' || p[1] == 'C'
			    || p[1] == 'm' || p[1] == 'M'
			    || p[1] == 'v' || p[1] == 'V' || p[1] == 'Z'))
		{
		    නම් (p[1] == 'v')
			is_magic_all = TRUE;
		    නැතහොත් නම් (p[1] == 'm' || p[1] == 'M' || p[1] == 'V')
			is_magic_all = FALSE;
		    p += 2;
		}
		නම් (p[0] == NUL
			|| (p[0] == '\\'
			    && (p[1] == '|' || p[1] == '&' || p[1] == ')'
				|| p[1] == 'n'))
			|| (is_magic_all
			    && (p[0] == '|' || p[0] == '&' || p[0] == ')'))
			|| reg_magic == MAGIC_ALL)
		    curchr = Magic('$');
	    }
	    bha_nga;
	pak_sa '\\':
	    {
		පූර්ණ_සංඛ්‍යා c = regparse[1];

		නම් (c == NUL)
		    curchr = '\\';	// trailing '\'
		නැතහොත් නම් (c <= '~' && META_flags[c])
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
		නැතහොත් නම් (vim_strchr(REGEXP_ABBR, c))
		{
		    /*
		     * Handle abbreviations, like "\t" for TAB -- webb
		     */
		    curchr = backslash_trans(c);
		}
		නැතහොත් නම් (reg_magic == MAGIC_NONE && (c == '$' || c == '^'))
		    curchr = toggle_Magic(c);
		නැතහොත්
		{
		    /*
		     * Next character can never be (made) magic?
		     * Then backslashing it won't do anything.
		     */
		    නම් (has_mbyte && c >= 0x80)
			curchr = (*mb_ptr2char)(regparse + 1);
		    නැතහොත්
			curchr = c;
		}
		bha_nga;
	    }

	default:
	    // curchr already holds regparse[0]; only a multi-byte lead byte
	    // needs decoding.
	    නම් (has_mbyte && curchr >= 0x80)
		curchr = (*mb_ptr2char)(regparse);
    }

    ආපසු curchr;
}

/*
 * Eat one lexed character.  Do this in a way that we can undo it.
 */
    static void
skipchr(void)
{
    // peekchr() eats a backslash, do the same here
    නම් (*regparse == '\\')
	prevchr_len = 1;
    නැතහොත්
	prevchr_len = 0;
    නම් (regparse[prevchr_len] != NUL)
    {
	නම් (enc_utf8)
	{
	    // Exclude composing chars that mb_ptr2len does include.  A byte
	    // below 0x80 is always a single character.
	    නම් (regparse[prevchr_len] < 0x80)
		++prevchr_len;
	    නැතහොත්
		prevchr_len += utf_ptr2len(regparse + prevchr_len);
	}
	නැතහොත් නම් (has_mbyte)
	    prevchr_len += (*mb_ptr2len)(regparse + prevchr_len);
	නැතහොත්
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
    static void
skipchr_keepstart(void)
{
    පූර්ණ_සංඛ්‍යා as = prev_at_start;
    පූර්ණ_සංඛ්‍යා pr = prevchr;
    පූර්ණ_සංඛ්‍යා prpr = prevprevchr;

    skipchr();
    at_start = as;
    prevchr = pr;
    prevprevchr = prpr;
}

/*
 * Get the next character from the pattern. We know about magic and such, so
 * therefore we need a lexical analyzer.
 */
    static පූර්ණ_සංඛ්‍යා
getchr(void)
{
    පූර්ණ_සංඛ්‍යා chr = peekchr();

    skipchr();
    ආපසු chr;
}

/*
 * put character back.  Works only once!
 */
    static void
ungetchr(void)
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
    static vimlong_T
gethexchrs(පූර්ණ_සංඛ්‍යා maxinputlen)
{
    long_u	nr = 0;
    පූර්ණ_සංඛ්‍යා		c;
    පූර්ණ_සංඛ්‍යා		i;

    සඳහා (i = 0; i < maxinputlen; ++i)
    {
	c = regparse[0];
	නම් (!vim_isxdigit(c))
	    bha_nga;
	nr <<= 4;
	nr |= hex2nr(c);
	++regparse;
    }

    නම් (i == 0)
	ආපසු -1;
    ආපසු nr;
}

/*
 * Get and return the value of the decimal string immediately after the
 * current position. Return -1 for invalid.  Consumes all digits.
 */
    static vimlong_T
getdecchrs(void)
{
    long_u	nr = 0;
    පූර්ණ_සංඛ්‍යා		c;
    පූර්ණ_සංඛ්‍යා		i;

    සඳහා (i = 0; ; ++i)
    {
	c = regparse[0];
	නම් (c < '0' || c > '9')
	    bha_nga;
	nr *= 10;
	nr += c - '0';
	++regparse;
	curchr = -1; // no longer valid
    }

    නම් (i == 0)
	ආපසු -1;
    ආපසු nr;
}

/*
 * get and return the value of the octal string immediately after the current
 * position. Return -1 for invalid, or 0-255 for valid. Smart enough to handle
 * numbers > 377 correctly (for example, 400 is treated as 40) and doesn't
 * treat 8 or 9 as recognised characters. Position is updated:
 *     blahblah\%o210asdf
 *	   before-^  ^-after
 */
    static long
getoctchrs(void)
{
    long_u	nr = 0;
    පූර්ණ_සංඛ්‍යා		c;
    පූර්ණ_සංඛ්‍යා		i;

    සඳහා (i = 0; i < 3 && nr < 040; ++i)
    {
	c = regparse[0];
	නම් (c < '0' || c > '7')
	    bha_nga;
	nr <<= 3;
	nr |= hex2nr(c);
	++regparse;
    }

    නම් (i == 0)
	ආපසු -1;
    ආපසු (long)nr;
}

/*
 * read_limits - Read two integers to be taken as a minimum and maximum.
 * If the first character is '-', then the range is reversed.
 * Should end with 'end'.  If minval is missing, zero is default, if maxval is
 * missing, a very big number is the default.
 */
    static පූර්ණ_සංඛ්‍යා
read_limits(long *minval, long *maxval)
{
    පූර්ණ_සංඛ්‍යා		reverse = FALSE;
    char_u	*first_char;
    long	tmp;

    නම් (*regparse == '-')
    {
	// Starts with '-', so reverse the range later
	regparse++;
	reverse = TRUE;
    }
    first_char = regparse;
    *minval = getdigits(&regparse);
    නම් (*regparse == ',')	    // There is a comma
    {
	නම් (vim_isdigit(*++regparse))
	    *maxval = getdigits(&regparse);
	නැතහොත්
	    *maxval = MAX_LIMIT;
    }
    නැතහොත් නම් (VIM_ISDIGIT(*first_char))
	*maxval = *minval;	    // It was \{n} or \{-n}
    නැතහොත්
	*maxval = MAX_LIMIT;	    // It was \{} or \{-}
    නම් (*regparse == '\\')
	regparse++;	// Allow either \{...} or \{...\}
    නම් (*regparse != '}')
	EMSG2_RET_FAIL(_(e_syntax_error_in_str_curlies),
						       reg_magic == MAGIC_ALL);

    /*
     * Reverse the range if there was a '-', or make sure it is in the right
     * order otherwise.
     */
    නම් ((!reverse && *minval > *maxval) || (reverse && *minval < *maxval))
    {
	tmp = *minval;
	*minval = *maxval;
	*maxval = tmp;
    }
    skipchr();		// let's be friends with the lexer again
    ආපසු OK;
}

/*
 * vim_regexec and friends
 */

/*
 * Global work variables for vim_regexec().
 */

static void	cleanup_subexpr(void);
#ifdef FEAT_SYN_HL
static void	cleanup_zsubexpr(void);
#endif
static පූර්ණ_සංඛ්‍යා	match_with_backref(linenr_T start_lnum, colnr_T start_col, linenr_T end_lnum, colnr_T end_col, පූර්ණ_සංඛ්‍යා *bytelen);

/*
 * Sometimes need to save a copy of a line.  Since alloc()/free() is very
 * slow, we keep one allocated piece of memory and only re-allocate it when
 * it's too small.  It's freed in bt_regexec_both() when finished.
 */
static char_u	*reg_tofree = NULL;
static unsigned	reg_tofreelen;

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
typedef sa_nracanaa {
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
    පූර්ණ_සංඛ්‍යා			reg_line_lbr;	// "\n" in string is line break

    // The current match-position is stord in these variables:
    linenr_T	lnum;		// line number, relative to first line
    char_u	*line;		// start of current line
    char_u	*input;		// current input, points into "line"

    පූර්ණ_සංඛ්‍යා	need_clear_subexpr;	// subexpressions still need to be cleared
#ifdef FEAT_SYN_HL
    පූර්ණ_සංඛ්‍යා	need_clear_zsubexpr;	// extmatch subexpressions still need to be
				// cleared
#endif

    // Internal copy of 'ignorecase'.  It is set at each call to vim_regexec().
    // Normally it gets the value of "rm_ic" or "rmm_ic", but when the pattern
    // contains '\c' or '\C' the value is overruled.
    පූර්ණ_සංඛ්‍යා			reg_ic;

    // Similar to "reg_ic", but only for 'combining' characters.  Set with \Z
    // flag in the regexp.  Defaults to false, always.
    පූර්ණ_සංඛ්‍යා			reg_icombine;

    // Copy of "rmm_maxcol": maximum column to search for a match.  Zero when
    // there is no maximum.
    colnr_T		reg_maxcol;

    // State for the NFA engine regexec.
    පූර්ණ_සංඛ්‍යා nfa_has_zend;	    // NFA regexp \ze operator encountered.
    පූර්ණ_සංඛ්‍යා nfa_has_backref;    // NFA regexp \1 .. \9 encountered.
    පූර්ණ_සංඛ්‍යා nfa_nsubexpr;	    // Number of sub expressions actually being used
			    // during execution. 1 if only the whole match
			    // (subexpr 0) is used.
    // listid is global, so that it increases on recursive calls to
    // nfa_regmatch(), which means we don't have to clear the lastlist field of
    // all the states.
    පූර්ණ_සංඛ්‍යා nfa_listid;
    පූර්ණ_සංඛ්‍යා nfa_alt_listid;

#ifdef FEAT_SYN_HL
    පූර්ණ_සංඛ්‍යා nfa_has_zsubexpr;   // NFA regexp has \z( ), set zsubexpr.
#endif
} regexec_T;

static regexec_T	rex;
static පූර්ණ_සංඛ්‍යා		rex_in_use = FALSE;

/*
 * Return TRUE if character 'c' is included in 'iskeyword' option for
 * "reg_buf" buffer.
 */
    static පූර්ණ_සංඛ්‍යා
reg_iswordc(පූර්ණ_සංඛ්‍යා c)
{
    ආපසු vim_iswordc_buf(c, rex.reg_buf);
}

#ifdef FEAT_EVAL
static පූර්ණ_සංඛ්‍යා can_f_submatch = FALSE;	// TRUE when submatch() can be used

// This struct is used for reg_submatch(). Needed for when the
// substitution string is an expression that contains a call to substitute()
// and submatch().
typedef sa_nracanaa {
    regmatch_T	*sm_match;
    regmmatch_T	*sm_mmatch;
    linenr_T	sm_firstlnum;
    linenr_T	sm_maxline;
    පූර්ණ_සංඛ්‍යා		sm_line_lbr;
} regsubmatch_T;

static regsubmatch_T rsm;  // can only be used when can_f_submatch is TRUE
#endif

typedef ga_nanaa
{
    RGLF_LINE = 0x01,
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
    static void
reg_getline_common(
    linenr_T		lnum,
    reg_getline_flags_T	flags,
    char_u		**line,
    colnr_T		*length)
{
    පූර්ණ_සංඛ්‍යා get_line = flags & RGLF_LINE;
    පූර්ණ_සංඛ්‍යා get_length = flags & RGLF_LENGTH;
    linenr_T firstlnum;
    linenr_T maxline;

#ifdef FEAT_EVAL
    නම් (flags & RGLF_SUBMATCH)
    {
	firstlnum = rsm.sm_firstlnum + lnum;
	maxline = rsm.sm_maxline;
    }
    නැතහොත්
#endif
    {
	firstlnum = rex.reg_firstlnum + lnum;
	maxline = rex.reg_maxline;
    }

    // when looking behind for a match/no-match lnum is negative. but we
    // can't go before line 1.
    නම් (firstlnum < 1)
    {
	නම් (get_line)
	    *line = NULL;
	නම් (get_length)
	    *length = 0;

	ආපසු;
    }

    නම් (lnum > maxline)
    {
	// must have matched the "\n" in the last line.
	නම් (get_line)
	    *line = (char_u *)"";
	නම් (get_length)
	    *length = 0;

	ආපසු;
    }

    නම් (get_line)
	*line = ml_get_buf(rex.reg_buf, firstlnum, FALSE);
    නම් (get_length)
	*length = ml_get_buf_len(rex.reg_buf, firstlnum);
}

/*
 * Get pointer to the line "lnum", which is relative to "reg_firstlnum".
 */
    static char_u *
reg_getline(linenr_T lnum)
{
    char_u *line;

    reg_getline_common(lnum, RGLF_LINE, &line, NULL);

    ආපසු line;
}

/*
 * Get length of line "lnum", which is relative to "reg_firstlnum".
 */
    static colnr_T
reg_getline_len(linenr_T lnum)
{
    colnr_T length;

    reg_getline_common(lnum, RGLF_LENGTH, NULL, &length);

    ආපසු length;
}

#ifdef FEAT_SYN_HL
static char_u	*reg_startzp[NSUBEXP];	// Workspace to mark beginning
static char_u	*reg_endzp[NSUBEXP];	//   and end of \z(...\) matches
static lpos_T	reg_startzpos[NSUBEXP];	// idem, beginning pos
static lpos_T	reg_endzpos[NSUBEXP];	// idem, end pos
#endif

// TRUE if using multi-line regexp.
#define REG_MULTI	(rex.reg_match == NULL)

#ifdef FEAT_SYN_HL
/*
 * Create a new extmatch and mark it as referenced once.
 */
    static reg_extmatch_T *
make_extmatch(void)
{
    reg_extmatch_T	*em;

    em = ALLOC_CLEAR_ONE(reg_extmatch_T);
    නම් (em != NULL)
	em->refcnt = 1;
    ආපසු em;
}

/*
 * Add a reference to an extmatch.
 */
    reg_extmatch_T *
ref_extmatch(reg_extmatch_T *em)
{
    නම් (em != NULL)
	em->refcnt++;
    ආපසු em;
}

/*
 * Remove a reference to an extmatch.  If there are no references left, free
 * the info.
 */
    void
unref_extmatch(reg_extmatch_T *em)
{
    පූර්ණ_සංඛ්‍යා i;

    නම් (em != NULL && --em->refcnt <= 0)
    {
	සඳහා (i = 0; i < NSUBEXP; ++i)
	    vim_free(em->matches[i]);
	vim_free(em);
    }
}
#endif

/*
 * Get class of previous character.
 */
    static පූර්ණ_සංඛ්‍යා
reg_prev_class(void)
{
    නම් (rex.input > rex.line)
	ආපසු mb_get_class_buf(rex.input - 1
		       - (*mb_head_off)(rex.line, rex.input - 1), rex.reg_buf);
    ආපසු -1;
}

/*
 * Return TRUE if the current rex.input position matches the Visual area.
 */
    static පූර්ණ_සංඛ්‍යා
reg_match_visual(void)
{
    pos_T	top, bot;
    linenr_T    lnum;
    colnr_T	col;
    win_T	*wp = rex.reg_win == NULL ? curwin : rex.reg_win;
    පූර්ණ_සංඛ්‍යා		mode;
    colnr_T	start, end;
    colnr_T	start2, end2;
    colnr_T	cols;
    colnr_T	curswant;

    // Check if the buffer is the current buffer and not using a string.
    නම් (rex.reg_buf != curbuf || VIsual.lnum == 0 || !REG_MULTI)
	ආපසු FALSE;

    නම් (VIsual_active)
    {
	නම් (LT_POS(VIsual, wp->w_cursor))
	{
	    top = VIsual;
	    bot = wp->w_cursor;
	}
	නැතහොත්
	{
	    top = wp->w_cursor;
	    bot = VIsual;
	}
	mode = VIsual_mode;
	curswant = wp->w_curswant;
    }
    නැතහොත්
    {
	නම් (LT_POS(curbuf->b_visual.vi_start, curbuf->b_visual.vi_end))
	{
	    top = curbuf->b_visual.vi_start;
	    bot = curbuf->b_visual.vi_end;
	}
	නැතහොත්
	{
	    top = curbuf->b_visual.vi_end;
	    bot = curbuf->b_visual.vi_start;
	}
	// a substitute command may have removed some lines
	නම් (bot.lnum > curbuf->b_ml.ml_line_count)
	    bot.lnum = curbuf->b_ml.ml_line_count;
	mode = curbuf->b_visual.vi_mode;
	curswant = curbuf->b_visual.vi_curswant;
    }
    lnum = rex.lnum + rex.reg_firstlnum;
    නම් (lnum < top.lnum || lnum > bot.lnum)
	ආපසු FALSE;

    col = (colnr_T)(rex.input - rex.line);
    නම් (mode == 'v')
    {
	නම් ((lnum == top.lnum && col < top.col)
		|| (lnum == bot.lnum && col >= bot.col + (*p_sel != 'e')))
	    ආපසු FALSE;
    }
    නැතහොත් නම් (mode == Ctrl_V)
    {
	getvvcol(wp, &top, &start, NULL, &end, 0);
	getvvcol(wp, &bot, &start2, NULL, &end2, 0);
	නම් (start2 < start)
	    start = start2;
	නම් (end2 > end)
	    end = end2;
	නම් (top.col == MAXCOL || bot.col == MAXCOL || curswant == MAXCOL)
	    end = MAXCOL;

	// getvvcol() flushes rex.line, need to get it again
	rex.line = reg_getline(rex.lnum);
	rex.input = rex.line + col;

	cols = win_linetabsize(wp, rex.reg_firstlnum + rex.lnum, rex.line, col);
	නම් (cols < start || cols > end - (*p_sel == 'e'))
	    ආපසු FALSE;
    }
    ආපසු TRUE;
}

/*
 * Check the regexp program for its magic number.
 * Return TRUE if it's wrong.
 */
    static පූර්ණ_සංඛ්‍යා
prog_magic_wrong(void)
{
    regprog_T	*prog;

    prog = REG_MULTI ? rex.reg_mmatch->regprog : rex.reg_match->regprog;
    නම් (prog->engine == &nfa_regengine)
	// For NFA matcher we don't check the magic
	ආපසු FALSE;

    නම් (UCHARAT(((bt_regprog_T *)prog)->program) != REGMAGIC)
    {
	iemsg(e_corrupted_regexp_program);
	ආපසු TRUE;
    }
    ආපසු FALSE;
}

/*
 * Cleanup the subexpressions, if this wasn't done yet.
 * This construction is used to clear the subexpressions only when they are
 * used (to increase speed).
 */
    static void
cleanup_subexpr(void)
{
    නම් (!rex.need_clear_subexpr)
	ආපසු;

    නම් (REG_MULTI)
    {
	// Use 0xff to set lnum to -1
	vim_memset(rex.reg_startpos, 0xff, parimaa_na(lpos_T) * NSUBEXP);
	vim_memset(rex.reg_endpos, 0xff, parimaa_na(lpos_T) * NSUBEXP);
    }
    නැතහොත්
    {
	vim_memset(rex.reg_startp, 0, parimaa_na(char_u *) * NSUBEXP);
	vim_memset(rex.reg_endp, 0, parimaa_na(char_u *) * NSUBEXP);
    }
    rex.need_clear_subexpr = FALSE;
}

#ifdef FEAT_SYN_HL
    static void
cleanup_zsubexpr(void)
{
    නම් (!rex.need_clear_zsubexpr)
	ආපසු;

    නම් (REG_MULTI)
    {
	// Use 0xff to set lnum to -1
	vim_memset(reg_startzpos, 0xff, parimaa_na(lpos_T) * NSUBEXP);
	vim_memset(reg_endzpos, 0xff, parimaa_na(lpos_T) * NSUBEXP);
    }
    නැතහොත්
    {
	vim_memset(reg_startzp, 0, parimaa_na(char_u *) * NSUBEXP);
	vim_memset(reg_endzp, 0, parimaa_na(char_u *) * NSUBEXP);
    }
    rex.need_clear_zsubexpr = FALSE;
}
#endif

/*
 * Advance rex.lnum, rex.line and rex.input to the next line.
 */
    static void
reg_nextline(void)
{
    rex.line = reg_getline(++rex.lnum);
    rex.input = rex.line;
    fast_breakcheck();
}

/*
 * Check whether a backreference matches.
 * Returns RA_FAIL, RA_NOMATCH or RA_MATCH.
 * If "bytelen" is not NULL, it is set to the byte length of the match in the
 * last line.
 * Optional: ignore case if rex.reg_ic is set.
 */
    static පූර්ණ_සංඛ්‍යා
match_with_backref(
    linenr_T start_lnum,
    colnr_T  start_col,
    linenr_T end_lnum,
    colnr_T  end_col,
    පූර්ණ_සංඛ්‍යා	     *bytelen)
{
    linenr_T	clnum = start_lnum;
    colnr_T	ccol = start_col;
    පූර්ණ_සංඛ්‍යා		len;
    char_u	*p;

    නම් (bytelen != NULL)
	*bytelen = 0;
    සඳහා (;;)
    {
	// Since getting one line may invalidate the other, need to make copy.
	// Slow!
	නම් (rex.line != reg_tofree)
	{
	    len = (පූර්ණ_සංඛ්‍යා)STRLEN(rex.line);
	    නම් (reg_tofree == NULL || len >= (පූර්ණ_සංඛ්‍යා)reg_tofreelen)
	    {
		len += 50;	// get some extra
		vim_free(reg_tofree);
		reg_tofree = alloc(len);
		නම් (reg_tofree == NULL)
		    ආපසු RA_FAIL; // out of memory!
		reg_tofreelen = len;
	    }
	    STRCPY(reg_tofree, rex.line);
	    rex.input = reg_tofree + (rex.input - rex.line);
	    rex.line = reg_tofree;
	}

	// Get the line to compare with.
	p = reg_getline(clnum);
	නම් (clnum == end_lnum)
	    len = end_col - ccol;
	නැතහොත්
	    len = (පූර්ණ_සංඛ්‍යා)reg_getline_len(clnum) - ccol;

	// Use case-insensitive compare if rex.reg_ic is set
	නම් ((!rex.reg_ic && cstrncmp(p + ccol, rex.input, &len) != 0)
	    || (rex.reg_ic && MB_STRNICMP(p + ccol, rex.input, len) != 0))
	    ආපසු RA_NOMATCH;  // doesn't match
	නම් (bytelen != NULL)
	    *bytelen += len;
	නම් (clnum == end_lnum)
	    bha_nga;		// match and at end!
	නම් (rex.lnum >= rex.reg_maxline)
	    ආපසු RA_NOMATCH;  // text too short

	// Advance to next line.
	reg_nextline();
	නම් (bytelen != NULL)
	    *bytelen = 0;
	++clnum;
	ccol = 0;
	නම් (got_int)
	    ආපසු RA_FAIL;
    }

    // found a match!  Note that rex.line may now point to a copy of the line,
    // that should not matter.
    ආපසු RA_MATCH;
}

/*
 * Used in a place where no * or \+ can follow.
 */
    static පූර්ණ_සංඛ්‍යා
re_mult_next(var_na *what)
{
    නම් (re_multi_type(peekchr()) == MULTI_MULT)
    {
	semsg(_(e_nfa_regexp_cannot_repeat_str), what);
	rc_did_emsg = TRUE;
	ආපසු FAIL;
    }
    ආපසු OK;
}

typedef sa_nracanaa
{
    පූර්ණ_සංඛ්‍යා a, b, c;
} decomp_T;


// 0xfb20 - 0xfb4f
static decomp_T decomp_table[0xfb4f-0xfb20+1] =
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

    static void
mb_decompose(පූර්ණ_සංඛ්‍යා c, පූර්ණ_සංඛ්‍යා *c1, පූර්ණ_සංඛ්‍යා *c2, පූර්ණ_සංඛ්‍යා *c3)
{
    decomp_T d;

    නම් (c >= 0xfb20 && c <= 0xfb4f)
    {
	d = decomp_table[c - 0xfb20];
	*c1 = d.a;
	*c2 = d.b;
	*c3 = d.c;
    }
    නැතහොත්
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
    static පූර්ණ_සංඛ්‍යා
cstrncmp(char_u *s1, char_u *s2, පූර්ණ_සංඛ්‍යා *n)
{
    පූර්ණ_සංඛ්‍යා		result;

    නම් (!rex.reg_ic)
	result = STRNCMP(s1, s2, *n);
    නැතහොත් නම් (enc_utf8)
    {
	char_u *p = s1;
	පූර්ණ_සංඛ්‍යා n2 = 0;
	පූර්ණ_සංඛ්‍යා n1 = *n;
	// count the number of characters for byte-length of s1
	අතරතුර (n1 > 0 && *p != NUL)
	{
	    n1 -= mb_ptr2len(p);
	    MB_PTR_ADV(p);
	    n2++;
	}
	// count the number of bytes to advance the same number of chars for s2
	p = s2;
	අතරතුර (n2-- > 0 && *p != NUL)
	    MB_PTR_ADV(p);

	n2 = p - s2;

	result = MB_STRNICMP2(s1, s2, *n, n2);
	නම් (result == 0)
	    *n = n2;
    }
    නැතහොත්
	result = MB_STRNICMP(s1, s2, *n);

    // if it failed and it's utf8 and we want to combineignore:
    නම් (result != 0 && enc_utf8 && rex.reg_icombine)
    {
	char_u	*str1, *str2;
	පූර්ණ_සංඛ්‍යා	c1, c2, c11, c12;
	පූර්ණ_සංඛ්‍යා	junk;

	// we have to handle the strcmp ourselves, since it is necessary to
	// deal with the composing characters by ignoring them:
	str1 = s1;
	str2 = s2;
	c1 = c2 = 0;
	අතරතුර ((පූර්ණ_සංඛ්‍යා)(str1 - s1) < *n)
	{
	    c1 = mb_ptr2char_adv(&str1);
	    c2 = mb_ptr2char_adv(&str2);

	    // Decompose the character if necessary, into 'base' characters.
	    // Currently hard-coded for Hebrew, Arabic to be done...
	    නම් (c1 != c2 && (!rex.reg_ic || utf_fold(c1) != utf_fold(c2)))
	    {
		// decomposition necessary?
		mb_decompose(c1, &c11, &junk, &junk);
		mb_decompose(c2, &c12, &junk, &junk);
		c1 = c11;
		c2 = c12;
		නම් (c11 != c12
			    && (!rex.reg_ic || utf_fold(c11) != utf_fold(c12)))
		    bha_nga;
	    }
	}
	result = c2 - c1;
	නම් (result == 0)
	    *n = (පූර්ණ_සංඛ්‍යා)(str2 - s2);
    }

    ආපසු result;
}

/*
 * cstrchr: This function is used a lot for simple searches, keep it fast!
 */
    static char_u *
cstrchr(char_u *s, පූර්ණ_සංඛ්‍යා c)
{
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		cc, lc;

    නම් (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	ආපසු vim_strchr(s, c);

    // tolower() and toupper() can be slow, comparing twice should be a lot
    // faster (esp. when using MS Visual C++!).
    // For UTF-8 need to use folded case.
    නම් (enc_utf8 && c > 0x80)
    {
	cc = utf_fold(c);
	lc = cc;
    }
    නැතහොත්
	නම් (MB_ISUPPER(c))
	{
	    cc = MB_TOLOWER(c);
	    lc = cc;
	}
	නැතහොත් නම් (MB_ISLOWER(c))
	{
	    cc = MB_TOUPPER(c);
	    lc = c;
	}
	නැතහොත්
	    ආපසු vim_strchr(s, c);

    නම් (has_mbyte)
    {
	සඳහා (p = s; *p != NUL; p += (*mb_ptr2len)(p))
	{
	    පූර්ණ_සංඛ්‍යා uc = utf_ptr2char(p);
	    නම් (enc_utf8 && (c > 0x80 || uc > 0x80))
	    {
		// Do not match an illegal byte.  E.g. 0xff matches 0xc3 0xbf,
		// not 0xff.
		// compare with lower case of the character
		නම් ((uc < 0x80 || uc != *p) && utf_fold(uc) == lc)
		    ආපසු p;
	    }
	    නැතහොත් නම් (*p == c || *p == cc)
		ආපසු p;
	}
    }
    නැතහොත්
	// Faster version for when there are no multi-byte characters.
	සඳහා (p = s; *p != NUL; ++p)
	    නම් (*p == c || *p == cc)
		ආපසු p;

    ආපසු NULL;
}

////////////////////////////////////////////////////////////////
//		      regsub stuff			      //
////////////////////////////////////////////////////////////////

typedef void (*fptr_T)(පූර්ණ_සංඛ්‍යා *, පූර්ණ_සංඛ්‍යා);

static පූර්ණ_සංඛ්‍යා vim_regsub_both(char_u *source, typval_T *expr, char_u *dest, පූර්ණ_සංඛ්‍යා destlen, පූර්ණ_සංඛ්‍යා flags);

    static void
do_upper(පූර්ණ_සංඛ්‍යා *d, පූර්ණ_සංඛ්‍යා c)
{
    *d = MB_TOUPPER(c);
}

    static void
do_lower(පූර්ණ_සංඛ්‍යා *d, පූර්ණ_සංඛ්‍යා c)
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
regtilde(char_u *source, පූර්ණ_සංඛ්‍යා magic)
{
    char_u	*newsub = source;
    char_u	*p;
    size_t	newsublen = 0;
    char_u	tilde[3] = {'~', NUL, NUL};
    size_t	tildelen = 1;
    පූර්ණ_සංඛ්‍යා		error = FALSE;

    නම් (!magic)
    {
	tilde[0] = '\\';
	tilde[1] = '~';
	tilde[2] = NUL;
	tildelen = 2;
    }

    සඳහා (p = newsub; *p; ++p)
    {
	නම් (STRNCMP(p, tilde, tildelen) == 0)
	{
	    size_t prefixlen = p - newsub;		// not including the tilde
	    char_u *postfix = p + tildelen;
	    size_t postfixlen;
	    size_t tmpsublen;

	    නම් (newsublen == 0)
		newsublen = STRLEN(newsub);
	    newsublen -= tildelen;
	    postfixlen = newsublen - prefixlen;
	    tmpsublen = prefixlen + reg_prev_sublen + postfixlen;

	    නම් (tmpsublen > 0 && reg_prev_sub != NULL)
	    {
		char_u *tmpsub;

		// Avoid making the text longer than MAXCOL, it will cause
		// trouble at some point.
		නම් (tmpsublen > MAXCOL)
		{
		    emsg(_(e_resulting_text_too_long));
		    error = TRUE;
		    bha_nga;
		}

		tmpsub = alloc(tmpsublen + 1);
		නම් (tmpsub == NULL)
		{
		    emsg(_(e_out_of_memory));
		    error = TRUE;
		    bha_nga;
		}

		// copy prefix
		mch_memmove(tmpsub, newsub, prefixlen);
		// interpret tilde
		mch_memmove(tmpsub + prefixlen, reg_prev_sub, reg_prev_sublen);
		// copy postfix
		STRCPY(tmpsub + prefixlen + reg_prev_sublen, postfix);

		නම් (newsub != source)	// allocated newsub before
		    vim_free(newsub);
		newsub = tmpsub;
		newsublen = tmpsublen;
		p = newsub + prefixlen + reg_prev_sublen;
	    }
	    නැතහොත්
		mch_memmove(p, postfix, postfixlen + 1);	// remove the tilde (+1 for the NUL)

	    --p;
	}
	නැතහොත්
	{
	    නම් (*p == '\\' && p[1])		// skip escaped characters
		++p;
	    නම් (has_mbyte)
		p += (*mb_ptr2len)(p) - 1;
	}
    }

    නම් (error)
    {
	නම් (newsub != source)
	    vim_free(newsub);
	ආපසු source;
    }

    // Store a copy of newsub  in reg_prev_sub.  It is always allocated,
    // because recursive calls may make the returned string invalid.
    // Only store it if there something to store.
    newsublen = p - newsub;
    නම් (newsublen == 0)
	VIM_CLEAR(reg_prev_sub);
    නැතහොත්
    {
	vim_free(reg_prev_sub);
	reg_prev_sub = vim_strnsave(newsub, newsublen);
    }

    නම් (reg_prev_sub == NULL)
	reg_prev_sublen = 0;
    නැතහොත්
	reg_prev_sublen = newsublen;

    ආපසු newsub;
}

#ifdef FEAT_EVAL

/*
 * Put the submatches in "argv[argskip]" which is a list passed into
 * call_func() by vim_regsub_both().
 */
    static පූර්ණ_සංඛ්‍යා
fill_submatch_list(පූර්ණ_සංඛ්‍යා argc UNUSED, typval_T *argv, පූර්ණ_සංඛ්‍යා argskip, ufunc_T *fp)
{
    listitem_T	*li;
    පූර්ණ_සංඛ්‍යා		i;
    char_u	*s;
    typval_T	*listarg = argv + argskip;

    නම් (!has_varargs(fp) && fp->uf_args.ga_len <= argskip)
	// called function doesn't take a submatches argument
	ආපසු argskip;

    // Relies on sl_list to be the first item in staticList10_T.
    init_static_list((staticList10_T *)(listarg->vval.v_list));

    // There are always 10 list items in staticList10_T.
    li = listarg->vval.v_list->lv_first;
    සඳහා (i = 0; i < 10; ++i)
    {
	s = rsm.sm_match->startp[i];
	නම් (s == NULL || rsm.sm_match->endp[i] == NULL)
	    s = NULL;
	නැතහොත්
	    s = vim_strnsave(s, rsm.sm_match->endp[i] - s);
	li->li_tv.v_type = VAR_STRING;
	li->li_tv.vval.v_string = s;
	li = li->li_next;
    }
    ආපසු argskip + 1;
}

    static void
clear_submatch_list(staticList10_T *sl)
{
    පූර්ණ_සංඛ්‍යා i;

    සඳහා (i = 0; i < 10; ++i)
	vim_free(sl->sl_items[i].li_tv.vval.v_string);
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
    පූර්ණ_සංඛ්‍යා
vim_regsub(
    regmatch_T	*rmp,
    char_u	*source,
    typval_T	*expr,
    char_u	*dest,
    පූර්ණ_සංඛ්‍යා		destlen,
    පූර්ණ_සංඛ්‍යා		flags)
{
    පූර්ණ_සංඛ්‍යා		result;
    regexec_T	rex_save;
    පූර්ණ_සංඛ්‍යා		rex_in_use_save = rex_in_use;

    නම් (rex_in_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_in_use = TRUE;

    rex.reg_match = rmp;
    rex.reg_mmatch = NULL;
    rex.reg_maxline = 0;
    rex.reg_buf = curbuf;
    rex.reg_line_lbr = TRUE;
    result = vim_regsub_both(source, expr, dest, destlen, flags);

    rex_in_use = rex_in_use_save;
    නම් (rex_in_use)
	rex = rex_save;

    ආපසු result;
}

    පූර්ණ_සංඛ්‍යා
vim_regsub_multi(
    regmmatch_T	*rmp,
    linenr_T	lnum,
    char_u	*source,
    char_u	*dest,
    පූර්ණ_සංඛ්‍යා		destlen,
    පූර්ණ_සංඛ්‍යා		flags)
{
    පූර්ණ_සංඛ්‍යා		result;
    regexec_T	rex_save;
    පූර්ණ_සංඛ්‍යා		rex_in_use_save = rex_in_use;

    නම් (rex_in_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_in_use = TRUE;

    rex.reg_match = NULL;
    rex.reg_mmatch = rmp;
    rex.reg_buf = curbuf;	// always works on the current buffer!
    rex.reg_firstlnum = lnum;
    rex.reg_maxline = curbuf->b_ml.ml_line_count - lnum;
    rex.reg_line_lbr = FALSE;
    result = vim_regsub_both(source, NULL, dest, destlen, flags);

    rex_in_use = rex_in_use_save;
    නම් (rex_in_use)
	rex = rex_save;

    ආපසු result;
}

#නම් defined(FEAT_EVAL)
// When nesting more than a couple levels it's probably a mistake.
# define MAX_REGSUB_NESTING 4
static char_u   *eval_result[MAX_REGSUB_NESTING] = {NULL, NULL, NULL, NULL};

# නම් defined(EXITFREE)
    void
free_resub_eval_result(void)
{
    පූර්ණ_සංඛ්‍යා i;

    සඳහා (i = 0; i < MAX_REGSUB_NESTING; ++i)
	VIM_CLEAR(eval_result[i]);
}
# endif
#endif

    static පූර්ණ_සංඛ්‍යා
vim_regsub_both(
    char_u	*source,
    typval_T	*expr,
    char_u	*dest,
    පූර්ණ_සංඛ්‍යා		destlen,
    පූර්ණ_සංඛ්‍යා		flags)
{
    char_u	*src;
    char_u	*dst;
    char_u	*s;
    පූර්ණ_සංඛ්‍යා		c;
    පූර්ණ_සංඛ්‍යා		cc;
    පූර්ණ_සංඛ්‍යා		no = -1;
    fptr_T	func_all = (fptr_T)NULL;
    fptr_T	func_one = (fptr_T)NULL;
    linenr_T	clnum = 0;	// init for GCC
    පූර්ණ_සංඛ්‍යා		len = 0;	// init for GCC
#ifdef FEAT_EVAL
    static පූර්ණ_සංඛ්‍යා  nesting = 0;
    පූර්ණ_සංඛ්‍යා		nested;
#endif
    පූර්ණ_සංඛ්‍යා		copy = flags & REGSUB_COPY;

    // Be paranoid...
    නම් ((source == NULL && expr == NULL) || dest == NULL)
    {
	iemsg(e_null_argument);
	ආපසු 0;
    }
    නම් (prog_magic_wrong())
	ආපසු 0;
#ifdef FEAT_EVAL
    නම් (nesting == MAX_REGSUB_NESTING)
    {
	emsg(_(e_substitute_nesting_too_deep));
	ආපසු 0;
    }
    nested = nesting;
#endif
    src = source;
    dst = dest;

    /*
     * When the substitute part starts with "\=" evaluate it as an expression.
     */
    නම් (expr != NULL || (source[0] == '\\' && source[1] == '='))
    {
#ifdef FEAT_EVAL
	// To make sure that the length doesn't change between checking the
	// length and copying the string, and to speed up things, the
	// resulting string is saved from the call with
	// "flags & REGSUB_COPY" == 0 to the call with
	// "flags & REGSUB_COPY" != 0.
	නම් (copy)
	{
	    නම් (eval_result[nested] != NULL)
	    {
		පූර්ණ_සංඛ්‍යා eval_len = (පූර්ණ_සංඛ්‍යා)STRLEN(eval_result[nested]);

		නම් (eval_len < destlen)
		{
		    STRCPY(dest, eval_result[nested]);
		    dst += eval_len;
		    VIM_CLEAR(eval_result[nested]);
		}
	    }
	}
	නැතහොත්
	{
	    පූර්ණ_සංඛ්‍යා		    prev_can_f_submatch = can_f_submatch;
	    regsubmatch_T   rsm_save;

	    VIM_CLEAR(eval_result[nested]);

	    // The expression may contain substitute(), which calls us
	    // recursively.  Make sure submatch() gets the text from the first
	    // level.
	    නම් (can_f_submatch)
		rsm_save = rsm;
	    can_f_submatch = TRUE;
	    rsm.sm_match = rex.reg_match;
	    rsm.sm_mmatch = rex.reg_mmatch;
	    rsm.sm_firstlnum = rex.reg_firstlnum;
	    rsm.sm_maxline = rex.reg_maxline;
	    rsm.sm_line_lbr = rex.reg_line_lbr;

	    // Although unlikely, it is possible that the expression invokes a
	    // substitute command (it might fail, but still).  Therefore keep
	    // an array of eval results.
	    ++nesting;

	    නම් (expr != NULL)
	    {
		typval_T	argv[2];
		char_u		buf[NUMBUFLEN];
		typval_T	rettv;
		staticList10_T	matchList;
		funcexe_T	funcexe;

		rettv.v_type = VAR_STRING;
		rettv.vval.v_string = NULL;
		argv[0].v_type = VAR_LIST;
		argv[0].vval.v_list = &matchList.sl_list;
		matchList.sl_list.lv_len = 0;
		CLEAR_FIELD(funcexe);
		funcexe.fe_argv_func = fill_submatch_list;
		funcexe.fe_evaluate = TRUE;
		නම් (expr->v_type == VAR_FUNC)
		{
		    s = expr->vval.v_string;
		    call_func(s, -1, &rettv, 1, argv, &funcexe);
		}
		නැතහොත් නම් (expr->v_type == VAR_PARTIAL)
		{
		    partial_T   *partial = expr->vval.v_partial;

		    s = partial_name(partial);
		    funcexe.fe_partial = partial;
		    call_func(s, -1, &rettv, 1, argv, &funcexe);
		}
		නැතහොත් නම් (expr->v_type == VAR_INSTR)
		{
		    exe_typval_instr(expr, &rettv);
		}
		නම් (matchList.sl_list.lv_len > 0)
		    // fill_submatch_list() was called
		    clear_submatch_list(&matchList);

		නම් (rettv.v_type == VAR_UNKNOWN)
		    // something failed, no need to report another error
		    eval_result[nested] = NULL;
		නැතහොත්
		{
		    eval_result[nested] = tv_get_string_buf_chk(&rettv, buf);
		    නම් (eval_result[nested] != NULL)
			eval_result[nested] = vim_strsave(eval_result[nested]);
		}
		clear_tv(&rettv);
	    }
	    නැතහොත් නම් (substitute_instr != NULL)
		// Execute instructions from ISN_SUBSTITUTE.
		eval_result[nested] = exe_substitute_instr();
	    නැතහොත්
		eval_result[nested] = eval_to_string(source + 2, TRUE, FALSE);
	    --nesting;

	    නම් (eval_result[nested] != NULL)
	    {
		පූර්ණ_සංඛ්‍යා had_backslash = FALSE;

		සඳහා (s = eval_result[nested]; *s != NUL; MB_PTR_ADV(s))
		{
		    // Change NL to CR, so that it becomes a line break,
		    // unless called from vim_regexec_nl().
		    // Skip over a backslashed character.
		    නම් (*s == NL && !rsm.sm_line_lbr)
			*s = CAR;
		    නැතහොත් නම් (*s == '\\' && s[1] != NUL)
		    {
			++s;
			/* Change NL to CR here too, so that this works:
			 * :s/abc\\\ndef/\="aaa\\\nbbb"/  on text:
			 *   abc\
			 *   def
			 * Not when called from vim_regexec_nl().
			 */
			නම් (*s == NL && !rsm.sm_line_lbr)
			    *s = CAR;
			had_backslash = TRUE;
		    }
		}
		නම් (had_backslash && (flags & REGSUB_BACKSLASH))
		{
		    // Backslashes will be consumed, need to double them.
		    s = vim_strsave_escaped(eval_result[nested], (char_u *)"\\");
		    නම් (s != NULL)
		    {
			vim_free(eval_result[nested]);
			eval_result[nested] = s;
		    }
		}

		dst += STRLEN(eval_result[nested]);
	    }

	    can_f_submatch = prev_can_f_submatch;
	    නම් (can_f_submatch)
		rsm = rsm_save;
	}
#endif
    }
    නැතහොත්
      අතරතුර ((c = *src++) != NUL)
      {
	නම් (c == '&' && (flags & REGSUB_MAGIC))
	    no = 0;
	නැතහොත් නම් (c == '\\' && *src != NUL)
	{
	    නම් (*src == '&' && !(flags & REGSUB_MAGIC))
	    {
		++src;
		no = 0;
	    }
	    නැතහොත් නම් ('0' <= *src && *src <= '9')
	    {
		no = *src++ - '0';
	    }
	    නැතහොත් නම් (vim_strchr((char_u *)"uUlLeE", *src))
	    {
		switch (*src++)
		{
		pak_sa 'u':   func_one = do_upper;
			    continue;
		pak_sa 'U':   func_all = do_upper;
			    continue;
		pak_sa 'l':   func_one = do_lower;
			    continue;
		pak_sa 'L':   func_all = do_lower;
			    continue;
		pak_sa 'e':
		pak_sa 'E':   func_one = func_all = (fptr_T)NULL;
			    continue;
		}
	    }
	}
	නම් (no < 0)	      // Ordinary character.
	{
	    නම් (c == K_SPECIAL && src[0] != NUL && src[1] != NUL)
	    {
		// Copy a special key as-is.
		නම් (copy)
		{
		    නම් (dst + 3 > dest + destlen)
		    {
			iemsg("vim_regsub_both(): not enough space");
			ආපසු 0;
		    }
		    *dst++ = c;
		    *dst++ = *src++;
		    *dst++ = *src++;
		}
		නැතහොත්
		{
		    dst += 3;
		    src += 2;
		}
		continue;
	    }

	    නම් (c == '\\' && *src != NUL)
	    {
		// Check for abbreviations -- webb
		switch (*src)
		{
		    pak_sa 'r':	c = CAR;	++src;	bha_nga;
		    pak_sa 'n':	c = NL;		++src;	bha_nga;
		    pak_sa 't':	c = TAB;	++src;	bha_nga;
		 // Oh no!  \e already has meaning in subst pat :-(
		 // case 'e':   c = ESC;	++src;	break;
		    pak_sa 'b':	c = Ctrl_H;	++src;	bha_nga;

		    // If "backslash" is TRUE the backslash will be removed
		    // later.  Used to insert a literal CR.
		    default:	නම් (flags & REGSUB_BACKSLASH)
				{
				    නම් (copy)
				    {
					නම් (dst + 1 > dest + destlen)
					{
					    iemsg("vim_regsub_both(): not enough space");
					    ආපසු 0;
					}
					*dst = '\\';
				    }
				    ++dst;
				}
				c = *src++;
		}
	    }
	    නැතහොත් නම් (has_mbyte)
		c = mb_ptr2char(src - 1);

	    // Write to buffer, if copy is set.
	    නම් (func_one != (fptr_T)NULL)
	    {
		func_one(&cc, c);
		func_one = NULL;
	    }
	    නැතහොත් නම් (func_all != (fptr_T)NULL)
		func_all(&cc, c);
	    නැතහොත් // just copy
		cc = c;

	    නම් (has_mbyte)
	    {
		පූර්ණ_සංඛ්‍යා totlen = mb_ptr2len(src - 1);
		පූර්ණ_සංඛ්‍යා charlen = mb_char2len(cc);

		නම් (copy)
		{
		    නම් (dst + charlen > dest + destlen)
		    {
			iemsg("vim_regsub_both(): not enough space");
			ආපසු 0;
		    }
		    mb_char2bytes(cc, dst);
		}
		dst += charlen - 1;
		නම් (enc_utf8)
		{
		    පූර්ණ_සංඛ්‍යා clen = utf_ptr2len(src - 1);

		    // If the character length is shorter than "totlen", there
		    // are composing characters; copy them as-is.
		    නම් (clen < totlen)
		    {
			නම් (copy)
			{
			    නම් (dst + totlen - clen > dest + destlen)
			    {
				iemsg("vim_regsub_both(): not enough space");
				ආපසු 0;
			    }
			    mch_memmove(dst + 1, src - 1 + clen,
						     (size_t)(totlen - clen));
			}
			dst += totlen - clen;
		    }
		}
		src += totlen - 1;
	    }
	    නැතහොත් නම් (copy)
	    {
		නම් (dst + 1 > dest + destlen)
		{
		    iemsg("vim_regsub_both(): not enough space");
		    ආපසු 0;
		}
		*dst = cc;
	    }
	    dst++;
	}
	නැතහොත්
	{
	    නම් (REG_MULTI)
	    {
		clnum = rex.reg_mmatch->startpos[no].lnum;
		නම් (clnum < 0 || rex.reg_mmatch->endpos[no].lnum < 0)
		    s = NULL;
		නැතහොත්
		{
		    s = reg_getline(clnum) + rex.reg_mmatch->startpos[no].col;
		    නම් (rex.reg_mmatch->endpos[no].lnum == clnum)
			len = rex.reg_mmatch->endpos[no].col
					    - rex.reg_mmatch->startpos[no].col;
		    නැතහොත්
			len = (පූර්ණ_සංඛ්‍යා)reg_getline_len(clnum) - rex.reg_mmatch->startpos[no].col;
		}
	    }
	    නැතහොත්
	    {
		s = rex.reg_match->startp[no];
		නම් (rex.reg_match->endp[no] == NULL)
		    s = NULL;
		නැතහොත්
		    len = (පූර්ණ_සංඛ්‍යා)(rex.reg_match->endp[no] - s);
	    }
	    නම් (s != NULL)
	    {
		සඳහා (;;)
		{
		    නම් (len == 0)
		    {
			නම් (REG_MULTI)
			{
			    නම් (rex.reg_mmatch->endpos[no].lnum == clnum)
				bha_nga;
			    නම් (copy)
			    {
				නම් (dst + 1 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    ආපසු 0;
				}
				*dst = CAR;
			    }
			    ++dst;
			    s = reg_getline(++clnum);
			    නම් (rex.reg_mmatch->endpos[no].lnum == clnum)
				len = rex.reg_mmatch->endpos[no].col;
			    නැතහොත්
				len = (පූර්ණ_සංඛ්‍යා)reg_getline_len(clnum);
			}
			නැතහොත්
			    bha_nga;
		    }
		    නැතහොත් නම් (*s == NUL) // we hit NUL.
		    {
			නම් (copy)
			    iemsg(e_damaged_match_string);
			goto exit;
		    }
		    නැතහොත්
		    {
			නම් ((flags & REGSUB_BACKSLASH)
						  && (*s == CAR || *s == '\\'))
			{
			    /*
			     * Insert a backslash in front of a CR, otherwise
			     * it will be replaced by a line break.
			     * Number of backslashes will be halved later,
			     * double them here.
			     */
			    නම් (copy)
			    {
				නම් (dst + 2 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    ආපසු 0;
				}
				dst[0] = '\\';
				dst[1] = *s;
			    }
			    dst += 2;
			}
			නැතහොත්
			{
			    නම් (has_mbyte)
				c = mb_ptr2char(s);
			    නැතහොත්
				c = *s;

			    නම් (func_one != (fptr_T)NULL)
			    {
				func_one(&cc, c);
				func_one = NULL;
			    }
			    නැතහොත් නම් (func_all != (fptr_T)NULL)
				func_all(&cc, c);
			    නැතහොත් // just copy
				cc = c;

			    නම් (has_mbyte)
			    {
				පූර්ණ_සංඛ්‍යා l;
				පූර්ණ_සංඛ්‍යා charlen;

				// Copy composing characters separately, one
				// at a time.
				නම් (enc_utf8)
				    l = utf_ptr2len(s) - 1;
				නැතහොත්
				    l = mb_ptr2len(s) - 1;

				s += l;
				len -= l;
				charlen = mb_char2len(cc);
				නම් (copy)
				{
				    නම් (dst + charlen > dest + destlen)
				    {
					iemsg("vim_regsub_both(): not enough space");
					ආපසු 0;
				    }
				    mb_char2bytes(cc, dst);
				}
				dst += charlen - 1;
			    }
			    නැතහොත් නම් (copy)
			    {
				නම් (dst + 1 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    ආපසු 0;
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
    නම් (copy)
	*dst = NUL;

exit:
    ආපසු (පූර්ණ_සංඛ්‍යා)((dst - dest) + 1);
}

#ifdef FEAT_EVAL

    static char_u *
reg_getline_submatch(linenr_T lnum)
{
    char_u *line;

    reg_getline_common(lnum, RGLF_LINE | RGLF_SUBMATCH, &line, NULL);

    ආපසු line;
}

    static colnr_T
reg_getline_submatch_len(linenr_T lnum)
{
    colnr_T length;

    reg_getline_common(lnum, RGLF_LENGTH | RGLF_SUBMATCH, NULL, &length);

    ආපසු length;
}

/*
 * Used for the submatch() function: get the string from the n'th submatch in
 * allocated memory.
 * Returns NULL when not in a ":s" command and for a non-existing submatch.
 */
    char_u *
reg_submatch(පූර්ණ_සංඛ්‍යා no)
{
    char_u	*retval = NULL;
    char_u	*s;
    පූර්ණ_සංඛ්‍යා		len;
    පූර්ණ_සංඛ්‍යා		round;
    linenr_T	lnum;

    නම් (!can_f_submatch || no < 0)
	ආපසු NULL;

    නම් (rsm.sm_match == NULL)
    {
	/*
	 * First round: compute the length and allocate memory.
	 * Second round: copy the text.
	 */
	සඳහා (round = 1; round <= 2; ++round)
	{
	    lnum = rsm.sm_mmatch->startpos[no].lnum;
	    නම් (lnum < 0 || rsm.sm_mmatch->endpos[no].lnum < 0)
		ආපසු NULL;

	    s = reg_getline_submatch(lnum);
	    නම් (s == NULL)  // anti-crash check, cannot happen?
		bha_nga;
	    s += rsm.sm_mmatch->startpos[no].col;
	    නම් (rsm.sm_mmatch->endpos[no].lnum == lnum)
	    {
		// Within one line: take form start to end col.
		len = rsm.sm_mmatch->endpos[no].col
					  - rsm.sm_mmatch->startpos[no].col;
		නම් (round == 2)
		    vim_strncpy(retval, s, len);
		++len;
	    }
	    නැතහොත්
	    {
		// Multiple lines: take start line from start col, middle
		// lines completely and end line up to end col.
		len = (පූර්ණ_සංඛ්‍යා)reg_getline_submatch_len(lnum) - rsm.sm_mmatch->startpos[no].col;
		නම් (round == 2)
		{
		    STRCPY(retval, s);
		    retval[len] = '\n';
		}
		++len;
		++lnum;
		අතරතුර (lnum < rsm.sm_mmatch->endpos[no].lnum)
		{
		    s = reg_getline_submatch(lnum);
		    නම් (round == 2)
			STRCPY(retval + len, s);
		    len += (පූර්ණ_සංඛ්‍යා)reg_getline_submatch_len(lnum);
		    නම් (round == 2)
			retval[len] = '\n';
		    ++len;
		    ++lnum;
		}
		නම් (round == 2)
		    STRNCPY(retval + len, reg_getline_submatch(lnum),
					     rsm.sm_mmatch->endpos[no].col);
		len += rsm.sm_mmatch->endpos[no].col;
		නම් (round == 2)
		    retval[len] = NUL;
		++len;
	    }

	    නම් (retval == NULL)
	    {
		retval = alloc(len);
		නම් (retval == NULL)
		    ආපසු NULL;
	    }
	}
    }
    නැතහොත්
    {
	s = rsm.sm_match->startp[no];
	නම් (s == NULL || rsm.sm_match->endp[no] == NULL)
	    retval = NULL;
	නැතහොත්
	    retval = vim_strnsave(s, rsm.sm_match->endp[no] - s);
    }

    ආපසු retval;
}

/*
 * Used for the submatch() function with the optional non-zero argument: get
 * the list of strings from the n'th submatch in allocated memory with NULs
 * represented in NLs.
 * Returns a list of allocated strings.  Returns NULL when not in a ":s"
 * command, for a non-existing submatch and for any error.
 */
    list_T *
reg_submatch_list(පූර්ණ_සංඛ්‍යා no)
{
    char_u	*s;
    linenr_T	slnum;
    linenr_T	elnum;
    colnr_T	scol;
    colnr_T	ecol;
    පූර්ණ_සංඛ්‍යා		i;
    list_T	*list;
    පූර්ණ_සංඛ්‍යා		error = FALSE;

    නම් (!can_f_submatch || no < 0)
	ආපසු NULL;

    නම් (rsm.sm_match == NULL)
    {
	slnum = rsm.sm_mmatch->startpos[no].lnum;
	elnum = rsm.sm_mmatch->endpos[no].lnum;
	නම් (slnum < 0 || elnum < 0)
	    ආපසු NULL;

	scol = rsm.sm_mmatch->startpos[no].col;
	ecol = rsm.sm_mmatch->endpos[no].col;

	list = list_alloc();
	නම් (list == NULL)
	    ආපසු NULL;

	s = reg_getline_submatch(slnum) + scol;
	නම් (slnum == elnum)
	{
	    නම් (list_append_string(list, s, ecol - scol) == FAIL)
		error = TRUE;
	}
	නැතහොත්
	{
	    පූර්ණ_සංඛ්‍යා max_lnum = elnum - slnum;

	    නම් (list_append_string(list, s, -1) == FAIL)
		error = TRUE;
	    සඳහා (i = 1; i < max_lnum; i++)
	    {
		s = reg_getline_submatch(slnum + i);
		නම් (list_append_string(list, s, -1) == FAIL)
		    error = TRUE;
	    }
	    s = reg_getline_submatch(elnum);
	    නම් (list_append_string(list, s, ecol) == FAIL)
		error = TRUE;
	}
    }
    නැතහොත්
    {
	s = rsm.sm_match->startp[no];
	නම් (s == NULL || rsm.sm_match->endp[no] == NULL)
	    ආපසු NULL;
	list = list_alloc();
	නම් (list == NULL)
	    ආපසු NULL;
	නම් (list_append_string(list, s,
				 (පූර්ණ_සංඛ්‍යා)(rsm.sm_match->endp[no] - s)) == FAIL)
	    error = TRUE;
    }

    නම් (error)
    {
	list_free(list);
	ආපසු NULL;
    }
    ++list->lv_refcount;
    ආපසු list;
}
#endif

/*
 * Initialize the values used for matching against multiple lines
 */
    static void
init_regexec_multi(
	regmmatch_T	*rmp,
	win_T		*win,	// window in which to search or NULL
	buf_T		*buf,	// buffer in which to search
	linenr_T	lnum)	// nr of line to start looking for match
{
    rex.reg_match = NULL;
    rex.reg_mmatch = rmp;
    rex.reg_buf = buf;
    rex.reg_win = win;
    rex.reg_firstlnum = lnum;
    rex.reg_maxline = rex.reg_buf->b_ml.ml_line_count - lnum;
    rex.reg_line_lbr = FALSE;
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
    static පූර්ණ_සංඛ්‍යා
get_char_class_bits(char_u *start, char_u *end, පූර්ණ_සංඛ්‍යා *newl)
{
    char_u	*p = start;
    පූර්ණ_සංඛ්‍යා		config = 0;

    *newl = FALSE;
    නම් (*end != ']')
	ආපසු -1;
    නම් (*p == '^')
    {
	config |= CLASS_not;
	p++;
    }

    අතරතුර (p < end)
    {
	නම් (p + 2 < end && *(p + 1) == '-')
	{
	    switch (*p)
	    {
		pak_sa '0':
		    නම් (*(p + 2) == '9')
		    {
			config |= CLASS_o9;
			bha_nga;
		    }
		    නම් (*(p + 2) == '7')
		    {
			config |= CLASS_o7;
			bha_nga;
		    }
		    ආපසු -1;

		pak_sa 'a':
		    නම් (*(p + 2) == 'z')
		    {
			config |= CLASS_az;
			bha_nga;
		    }
		    නම් (*(p + 2) == 'f')
		    {
			config |= CLASS_af;
			bha_nga;
		    }
		    ආපසු -1;

		pak_sa 'A':
		    නම් (*(p + 2) == 'Z')
		    {
			config |= CLASS_AZ;
			bha_nga;
		    }
		    නම් (*(p + 2) == 'F')
		    {
			config |= CLASS_AF;
			bha_nga;
		    }
		    ආපසු -1;

		default:
		    ආපසු -1;
	    }
	    p += 3;
	}
	නැතහොත් නම් (p + 1 < end && *p == '\\' && *(p + 1) == 'n')
	{
	    *newl = TRUE;
	    p += 2;
	}
	නැතහොත් නම් (*p == '_')
	{
	    config |= CLASS_underscore;
	    p++;
	}
	නැතහොත් නම් (*p == '\n')
	{
	    *newl = TRUE;
	    p++;
	}
	නැතහොත්
	    ආපසු -1;
    } // while (p < end)

    නම් (p != end)
	ආපසු -1;

    // U+017F and U+212A fold to ASCII 's' and 'k', missed by the class opcodes
    නම් (enc_utf8 && (config & (CLASS_az | CLASS_AZ)))
	ආපසු -1;

    ආපසු config;
}

#include "regexp_bt.c"

static regengine_T bt_regengine =
{
    bt_regcomp,
    bt_regfree,
    bt_regexec_nl,
    bt_regexec_multi
#ifdef DEBUG
    ,(char_u *)""
#endif
};

#include "regexp_nfa.c"

static regengine_T nfa_regengine =
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
static පූර්ණ_සංඛ්‍යා regexp_engine = 0;

#ifdef DEBUG
static const char_u regname[][30] = {
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
vim_regcomp(char_u *expr_arg, පූර්ණ_සංඛ්‍යා re_flags)
{
    regprog_T   *prog = NULL;
    char_u	*expr = expr_arg;
    පූර්ණ_සංඛ්‍යා		called_emsg_before;

    regexp_engine = p_re;

    // Check for prefix "\%#=", that sets the regexp engine
    නම් (STRNCMP(expr, "\\%#=", 4) == 0)
    {
	පූර්ණ_සංඛ්‍යා newengine = expr[4] - '0';

	නම් (newengine == AUTOMATIC_ENGINE
	    || newengine == BACKTRACKING_ENGINE
	    || newengine == NFA_ENGINE)
	{
	    regexp_engine = expr[4] - '0';
	    expr += 5;
#ifdef DEBUG
	    smsg("New regexp mode selected (%d): %s",
					   regexp_engine, regname[newengine]);
#endif
	}
	නැතහොත්
	{
	    emsg(_(e_percent_hash_can_only_be_followed_by_zero_one_two_automatic_engine_will_be_used));
	    regexp_engine = AUTOMATIC_ENGINE;
	}
    }
#ifdef DEBUG
    bt_regengine.expr = expr;
    nfa_regengine.expr = expr;
#endif
    // reg_iswordc() uses rex.reg_buf
    rex.reg_buf = curbuf;

    /*
     * First try the NFA engine, unless backtracking was requested.
     */
    called_emsg_before = called_emsg;
    නම් (regexp_engine != BACKTRACKING_ENGINE)
	prog = nfa_regengine.regcomp(expr,
		re_flags + (regexp_engine == AUTOMATIC_ENGINE ? RE_AUTO : 0));
    නැතහොත්
	prog = bt_regengine.regcomp(expr, re_flags);

    // Check for error compiling regexp with initial engine.
    නම් (prog == NULL)
    {
#ifdef BT_REGEXP_DEBUG_LOG
	නම් (regexp_engine == BACKTRACKING_ENGINE)   // debugging log for BT engine
	{
	    FILE *f;
	    f = fopen(BT_REGEXP_DEBUG_LOG_NAME, "a");
	    නම් (f)
	    {
		fprintf(f, "Syntax error in \"%s\"\n", expr);
		fclose(f);
	    }
	    නැතහොත්
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
	නම් (regexp_engine == AUTOMATIC_ENGINE
					  && called_emsg == called_emsg_before)
	{
	    regexp_engine = BACKTRACKING_ENGINE;
#ifdef FEAT_EVAL
	    report_re_switch(expr);
#endif
	    prog = bt_regengine.regcomp(expr, re_flags);
	}
    }

    නම් (prog != NULL)
    {
	// Store the info needed to call regcomp() again when the engine turns
	// out to be very slow when executing it.
	prog->re_engine = regexp_engine;
	prog->re_flags  = re_flags;
    }

    ආපසු prog;
}

/*
 * Free a compiled regexp program, returned by vim_regcomp().
 */
    void
vim_regfree(regprog_T *prog)
{
    නම් (prog != NULL)
	prog->engine->regfree(prog);
}

#නම් defined(EXITFREE)
    void
free_regexp_stuff(void)
{
    ga_clear(&regstack);
    ga_clear(&backpos);
    vim_free(reg_tofree);
    vim_free(reg_prev_sub);
    vim_free(post_start);   // NFA postfix buffer, reused across compilations
    post_start = NULL;
    post_start_len = 0;
    vim_free(nfa_stack);    // NFA fragment stack, reused across compilations
    nfa_stack = NULL;
    nfa_stack_len = 0;
}
#endif

#ifdef FEAT_EVAL
    static void
report_re_switch(char_u *pat)
{
    නම් (p_verbose > 0)
    {
	verbose_enter();
	msg_puts(_("Switching to backtracking RE engine for pattern: "));
	msg_puts((var_na *)pat);
	verbose_leave();
    }
}
#endif

#නම් defined(FEAT_X11)
/*
 * Return whether "prog" is currently being executed.
 */
    පූර්ණ_සංඛ්‍යා
regprog_in_use(regprog_T *prog)
{
    ආපසු prog->re_in_use;
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
    static පූර්ණ_සංඛ්‍යා
vim_regexec_string(
    regmatch_T	*rmp,
    char_u	*line,  // string to match against
    colnr_T	col,    // column to start looking for match
    පූර්ණ_සංඛ්‍යා		nl)
{
    පූර්ණ_සංඛ්‍යා		result;
    regexec_T	rex_save;
    පූර්ණ_සංඛ්‍යා		rex_in_use_save = rex_in_use;

    // Cannot use the same prog recursively, it contains state.
    නම් (rmp->regprog->re_in_use)
    {
	emsg(_(e_cannot_use_pattern_recursively));
	ආපසු FALSE;
    }
    rmp->regprog->re_in_use = TRUE;

    නම් (rex_in_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_in_use = TRUE;

    rex.reg_startp = NULL;
    rex.reg_endp = NULL;
    rex.reg_startpos = NULL;
    rex.reg_endpos = NULL;

    result = rmp->regprog->engine->regexec_nl(rmp, line, col, nl);
    rmp->regprog->re_in_use = FALSE;

    // NFA engine aborted because it's very slow.
    නම් (rmp->regprog->re_engine == AUTOMATIC_ENGINE
					       && result == NFA_TOO_EXPENSIVE)
    {
	පූර්ණ_සංඛ්‍යා    save_p_re = p_re;
	පූර්ණ_සංඛ්‍යා    re_flags = rmp->regprog->re_flags;
	char_u *pat = vim_strsave(((nfa_regprog_T *)rmp->regprog)->pattern);

	p_re = BACKTRACKING_ENGINE;
	නම් (pat != NULL)
	{
	    regprog_T *prev_prog = rmp->regprog;

#ifdef FEAT_EVAL
	    report_re_switch(pat);
#endif
	    rmp->regprog = vim_regcomp(pat, re_flags);
	    නම් (rmp->regprog == NULL)
	    {
		// Somehow compiling the pattern failed now, put back the
		// previous one to avoid "regprog" becoming NULL.
		rmp->regprog = prev_prog;
	    }
	    නැතහොත්
	    {
		vim_regfree(prev_prog);
		rmp->regprog->re_in_use = TRUE;
		result = rmp->regprog->engine->regexec_nl(rmp, line, col, nl);
		rmp->regprog->re_in_use = FALSE;
	    }
	    vim_free(pat);
	}

	p_re = save_p_re;
    }

    rex_in_use = rex_in_use_save;
    නම් (rex_in_use)
	rex = rex_save;

    ආපසු result > 0;
}

#නම් defined(FEAT_SPELL) || defined(FEAT_EVAL) || defined(FEAT_X11)
/*
 * Note: "*prog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    පූර්ණ_සංඛ්‍යා
vim_regexec_prog(
    regprog_T	**prog,
    පූර්ණ_සංඛ්‍යා		ignore_case,
    char_u	*line,
    colnr_T	col)
{
    පූර්ණ_සංඛ්‍යා		r;
    regmatch_T	regmatch;

    regmatch.regprog = *prog;
    regmatch.rm_ic = ignore_case;
    r = vim_regexec_string(&regmatch, line, col, FALSE);
    *prog = regmatch.regprog;
    ආපසු r;
}
#endif

/*
 * Note: "rmp->regprog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    පූර්ණ_සංඛ්‍යා
vim_regexec(regmatch_T *rmp, char_u *line, colnr_T col)
{
    ආපසු vim_regexec_string(rmp, line, col, FALSE);
}

/*
 * Like vim_regexec(), but consider a "\n" in "line" to be a line break.
 * Note: "rmp->regprog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    පූර්ණ_සංඛ්‍යා
vim_regexec_nl(regmatch_T *rmp, char_u *line, colnr_T col)
{
    ආපසු vim_regexec_string(rmp, line, col, TRUE);
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
    පූර්ණ_සංඛ්‍යා		*timed_out)	// flag is set when timeout limit reached
{
    පූර්ණ_සංඛ්‍යා		result;
    regexec_T	rex_save;
    පූර්ණ_සංඛ්‍යා		rex_in_use_save = rex_in_use;

    // Cannot use the same prog recursively, it contains state.
    නම් (rmp->regprog->re_in_use)
    {
	emsg(_(e_cannot_use_pattern_recursively));
	ආපසු FALSE;
    }
    rmp->regprog->re_in_use = TRUE;

    නම් (rex_in_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_in_use = TRUE;

    result = rmp->regprog->engine->regexec_multi(
				      rmp, win, buf, lnum, col, timed_out);
    rmp->regprog->re_in_use = FALSE;

    // NFA engine aborted because it's very slow.
    නම් (rmp->regprog->re_engine == AUTOMATIC_ENGINE
					       && result == NFA_TOO_EXPENSIVE)
    {
	පූර්ණ_සංඛ්‍යා    save_p_re = p_re;
	පූර්ණ_සංඛ්‍යා    re_flags = rmp->regprog->re_flags;
	char_u *pat = vim_strsave(((nfa_regprog_T *)rmp->regprog)->pattern);

	p_re = BACKTRACKING_ENGINE;
	නම් (pat != NULL)
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
	    නම් (rmp->regprog == NULL)
	    {
		// Somehow compiling the pattern failed now, put back the
		// previous one to avoid "regprog" becoming NULL.
		rmp->regprog = prev_prog;
	    }
	    නැතහොත්
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
    නම් (rex_in_use)
	rex = rex_save;

    ආපසු result <= 0 ? 0 : result;
}
