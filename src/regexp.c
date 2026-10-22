/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * Handling of regular expressions: vim_regcomp(), vim_regexec(), vim_regsub()
 */

// By default: do not create debugging logs or files related to regular
// expressions, even when compiling with -DDEBUG.
// Uncomment the second line to get the regexp debugging.
#undef DEBUG
// #define DEBUG

#अन्तर्भाव "vim.h"

#ifdef DEBUG
// show/save debugging data when BT engine is used
# define BT_REGEXP_DUMP
// save the debugging data to a file instead of displaying it
# define BT_REGEXP_LOG
# define BT_REGEXP_DEBUG_LOG
# define BT_REGEXP_DEBUG_LOG_NAME	"bt_regexp_debug.log"
#endif

#ifdef FEAT_RELTIME
स्थिर sig_atomic_t dummy_timeout_flag = 0;
स्थिर चञ्चल sig_atomic_t *timeout_flag = &dummy_timeout_flag;
#endif

/*
 * Magic characters have a special meaning, they don't match literally.
 * Magic characters are negative.  This separates them from literal characters
 * (possibly multi-byte).  Only ASCII characters can be Magic.
 */
#define Magic(x)	((पूर्णाङ्क)(x) - 256)
#define un_Magic(x)	((x) + 256)
#define is_Magic(x)	((x) < 0)

    स्थिर पूर्णाङ्क
no_Magic(पूर्णाङ्क x)
{
    यदि (is_Magic(x))
	प्रत्यागम un_Magic(x);
    प्रत्यागम x;
}

    स्थिर पूर्णाङ्क
toggle_Magic(पूर्णाङ्क x)
{
    यदि (is_Magic(x))
	प्रत्यागम un_Magic(x);
    प्रत्यागम Magic(x);
}

#यदि defined(FEAT_RELTIME)
स्थिर पूर्णाङ्क timeout_nesting = 0;

/*
 * Start a timer that will cause the regexp to abort after "msec".
 * This doesn't work well recursively.  In case it happens anyway, the first
 * set timeout will prevail, nested ones are ignored.
 * The caller must make sure there is a matching disable_regexp_timeout() call!
 */
    शून्य
init_regexp_timeout(long msec)
{
    यदि (timeout_nesting == 0)
	timeout_flag = start_timeout(msec);
    ++timeout_nesting;
}

    शून्य
disable_regexp_timeout(शून्य)
{
    यदि (timeout_nesting == 0)
	iemsg("disable_regexp_timeout() called without active timer");
    अन्यथा यदि (--timeout_nesting == 0)
    {
	stop_timeout();
	timeout_flag = &dummy_timeout_flag;
    }
}
#endif

#यदि defined(FEAT_EVAL)
# ifdef FEAT_RELTIME
स्थिर sig_atomic_t *saved_timeout_flag;
# endif

/*
 * Used at the debug prompt: disable the timeout so that expression evaluation
 * can used patterns.
 * Must be followed by calling restore_timeout_for_debugging().
 */
    शून्य
save_timeout_for_debugging(शून्य)
{
# ifdef FEAT_RELTIME
    saved_timeout_flag = (sig_atomic_t *)timeout_flag;
    timeout_flag = &dummy_timeout_flag;
# endif
}

    शून्य
restore_timeout_for_debugging(शून्य)
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
#define UCHARAT(p)	((पूर्णाङ्क)*(char_u *)(p))

// Used for an error (down from) vim_regcomp(): give the error message, set
// rc_did_emsg and return NULL
#define EMSG_RET_NULL(m) प्रत्यागम (emsg((m)), rc_did_emsg = TRUE, (शून्य *)रिक्त)
#define IEMSG_RET_NULL(m) प्रत्यागम (iemsg((m)), rc_did_emsg = TRUE, (शून्य *)रिक्त)
#define EMSG_RET_FAIL(m) प्रत्यागम (emsg((m)), rc_did_emsg = TRUE, FAIL)
#define EMSG2_RET_NULL(m, c) प्रत्यागम (semsg((ध्रुव वर्ण *)(m), (c) ? "" : "\\"), rc_did_emsg = TRUE, (शून्य *)रिक्त)
#define EMSG3_RET_NULL(m, c, a) प्रत्यागम (semsg((ध्रुव वर्ण *)(m), (c) ? "" : "\\", (a)), rc_did_emsg = TRUE, (शून्य *)रिक्त)
#define EMSG2_RET_FAIL(m, c) प्रत्यागम (semsg((ध्रुव वर्ण *)(m), (c) ? "" : "\\"), rc_did_emsg = TRUE, FAIL)
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
    स्थिर पूर्णाङ्क
re_multi_type(पूर्णाङ्क c)
{
    यदि (c == Magic('@') || c == Magic('=') || c == Magic('?'))
	प्रत्यागम MULTI_ONE;
    यदि (c == Magic('*') || c == Magic('+') || c == Magic('{'))
	प्रत्यागम MULTI_MULT;
    प्रत्यागम NOT_MULTI;
}

स्थिर char_u		*reg_prev_sub = रिक्त;
स्थिर size_t		reg_prev_sublen = 0;

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
स्थिर char_u REGEXP_INRANGE[] = "]^-n\\";
स्थिर char_u REGEXP_ABBR[] = "nrtebdoxuU";

/*
 * Translate '\x' to its control character, except "\n", which is Magic.
 */
    स्थिर पूर्णाङ्क
backslash_trans(पूर्णाङ्क c)
{
    विकल्प (c)
    {
	पक्ष 'r':   प्रत्यागम CAR;
	पक्ष 't':   प्रत्यागम TAB;
	पक्ष 'e':   प्रत्यागम ESC;
	पक्ष 'b':   प्रत्यागम BS;
    }
    प्रत्यागम c;
}

गणना
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
    स्थिर पूर्णाङ्क
get_char_class(char_u **pp)
{
    // must be sorted by the 'value' field because it is used by bsearch()!
    स्थिर keyvalue_T char_class_tab[] =
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
    यदि ((*pp)[1] == ':' && ASCII_ISLOWER((*pp)[2])
			&& ASCII_ISLOWER((*pp)[3]) && ASCII_ISLOWER((*pp)[4]))
    {
	keyvalue_T target;
	keyvalue_T *entry;
	// this function can be called repeatedly with the same value for "pp"
	// so we cache the last found entry.
	स्थिर keyvalue_T *last_entry = रिक्त;

	target.कुंजी = 0;
	target.मूल्य.string = *pp + 2;
	target.मूल्य.length = 0;	// not used, see cmp_keyvalue_value_n()

	यदि (last_entry != रिक्त && cmp_keyvalue_मूल्य_n(&target, last_entry) == 0)
	    entry = last_entry;
	अन्यथा
	    entry = (keyvalue_T *)bsearch(&target, &char_class_tab,
					ARRAY_LENGTH(char_class_tab),
					परिमाण(char_class_tab[0]), cmp_keyvalue_मूल्य_n);
	यदि (entry != रिक्त)
	{
	    last_entry = entry;
	    *pp += entry->मूल्य.length + 2;
	    प्रत्यागम entry->कुंजी;
	}
    }
    प्रत्यागम CLASS_NONE;
}

/*
 * Specific version of character class functions.
 * Using a table to keep this fast.
 */
स्थिर short	class_tab[256];

#define	    RI_DIGIT	0x01
#define	    RI_HEX	0x02
#define	    RI_OCTAL	0x04
#define	    RI_WORD	0x08
#define	    RI_HEAD	0x10
#define	    RI_ALPHA	0x20
#define	    RI_LOWER	0x40
#define	    RI_UPPER	0x80
#define	    RI_WHITE	0x100

    स्थिर शून्य
init_class_tab(शून्य)
{
    पूर्णाङ्क		i;
    स्थिर पूर्णाङ्क	done = FALSE;

    यदि (done)
	प्रत्यागम;

    प्रति (i = 0; i < 256; ++i)
    {
	यदि (i >= '0' && i <= '7')
	    class_tab[i] = RI_DIGIT + RI_HEX + RI_OCTAL + RI_WORD;
	अन्यथा यदि (i >= '8' && i <= '9')
	    class_tab[i] = RI_DIGIT + RI_HEX + RI_WORD;
	अन्यथा यदि (i >= 'a' && i <= 'f')
	    class_tab[i] = RI_HEX + RI_WORD + RI_HEAD + RI_ALPHA + RI_LOWER;
	अन्यथा यदि (i >= 'g' && i <= 'z')
	    class_tab[i] = RI_WORD + RI_HEAD + RI_ALPHA + RI_LOWER;
	अन्यथा यदि (i >= 'A' && i <= 'F')
	    class_tab[i] = RI_HEX + RI_WORD + RI_HEAD + RI_ALPHA + RI_UPPER;
	अन्यथा यदि (i >= 'G' && i <= 'Z')
	    class_tab[i] = RI_WORD + RI_HEAD + RI_ALPHA + RI_UPPER;
	अन्यथा यदि (i == '_')
	    class_tab[i] = RI_WORD + RI_HEAD;
	अन्यथा
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

स्थिर char_u	*regparse;	// Input-scan pointer.
स्थिर पूर्णाङ्क	regnpar;	// () count.
स्थिर पूर्णाङ्क	wants_nfa;	// regex should use NFA engine
#ifdef FEAT_SYN_HL
स्थिर पूर्णाङ्क	regnzpar;	// \z() count.
स्थिर पूर्णाङ्क	re_has_z;	// \z item detected
#endif
स्थिर unsigned	regflags;	// RF_ flags for prog
#यदि defined(FEAT_SYN_HL)
स्थिर पूर्णाङ्क	had_eol;	// TRUE when EOL found by vim_regcomp()
#endif

स्थिर magic_T	reg_magic;	// magicness of the pattern

स्थिर पूर्णाङ्क	reg_string;	// matching with a string instead of a buffer
				//पंक्ति
स्थिर पूर्णाङ्क	reg_strict;	// "[abc" is illegal

/*
 * META contains all characters that may be magic, except '^' and '$'.
 */

// META[] is used often enough to justify turning it into a table.
स्थिर ध्रुव char_u META_flags[] = {
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

स्थिर पूर्णाङ्क	curchr;		// currently parsed character
// Previous character.  Note: prevchr is sometimes -1 when we are not at the
// start, eg in /[ ^I]^ the pattern was never found even if it existed,
// because ^ was taken to be magic -- webb
स्थिर पूर्णाङ्क	prevchr;
स्थिर पूर्णाङ्क	prevprevchr;	// previous-previous character
स्थिर पूर्णाङ्क	nextchr;	// used for ungetchr()

// arguments for reg()
#define REG_NOPAREN	0	// toplevel reg()
#define REG_PAREN	1	// \(\)
#define REG_ZPAREN	2	// \z(\)
#define REG_NPAREN	3	// \%(\)

// Limit recursive parsing of nested regexp atoms to avoid using up the C stack.
#define REG_MAX_PAREN_DEPTH	1000

प्रकारनिर्देश संरचना
{
    char_u	*regparse;
    पूर्णाङ्क		prevchr_len;
    पूर्णाङ्क		curchr;
    पूर्णाङ्क		prevchr;
    पूर्णाङ्क		prevprevchr;
    पूर्णाङ्क		nextchr;
    पूर्णाङ्क		at_start;
    पूर्णाङ्क		prev_at_start;
    पूर्णाङ्क		regnpar;
} parse_state_T;

स्थिर शून्य	initchr(char_u *);
स्थिर पूर्णाङ्क	getchr(शून्य);
स्थिर शून्य	skipchr_keepstart(शून्य);
स्थिर पूर्णाङ्क	peekchr(शून्य);
स्थिर शून्य	skipchr(शून्य);
स्थिर शून्य	ungetchr(शून्य);
स्थिर vimlong_T	gethexchrs(पूर्णाङ्क maxinputlen);
स्थिर long	getoctchrs(शून्य);
स्थिर vimlong_T	getdecchrs(शून्य);
स्थिर पूर्णाङ्क	coll_get_char(शून्य);
स्थिर पूर्णाङ्क	prog_magic_wrong(शून्य);
स्थिर पूर्णाङ्क	cstrncmp(char_u *s1, char_u *s2, पूर्णाङ्क *n);
स्थिर char_u	*cstrchr(char_u *, पूर्णाङ्क);
स्थिर पूर्णाङ्क	re_mult_next(वर्ण *what);
स्थिर पूर्णाङ्क	reg_iswordc(पूर्णाङ्क);
#ifdef FEAT_EVAL
स्थिर शून्य report_re_switch(char_u *pat);
#endif

स्थिर regengine_T bt_regengine;
स्थिर regengine_T nfa_regengine;

/*
 * Return TRUE if compiled regular expression "prog" can match a line break.
 */
    पूर्णाङ्क
re_multiline(regprog_T *prog)
{
    प्रत्यागम (prog->regflags & RF_HASNL);
}

/*
 * Check for an equivalence class name "[=a=]".  "pp" points to the '['.
 * Returns a character representing the class. Zero means that no item was
 * recognized.  Otherwise "pp" is advanced to after the item.
 */
    स्थिर पूर्णाङ्क
get_equi_class(char_u **pp)
{
    पूर्णाङ्क		c;
    पूर्णाङ्क		l = 1;
    char_u	*p = *pp;

    यदि (p[1] == '=' && p[2] != NUL)
    {
	यदि (has_mbyte)
	    l = (*mb_ptr2len)(p + 2);
	यदि (p[l + 2] == '=' && p[l + 3] == ']')
	{
	    यदि (has_mbyte)
		c = mb_ptr2char(p + 2);
	    अन्यथा
		c = p[2];
	    *pp += l + 4;
	    प्रत्यागम c;
	}
    }
    प्रत्यागम 0;
}

/*
 * Check for a collating element "[.a.]".  "pp" points to the '['.
 * Returns a character. Zero means that no item was recognized.  Otherwise
 * "pp" is advanced to after the item.
 * Currently only single characters are recognized!
 */
    स्थिर पूर्णाङ्क
get_coll_element(char_u **pp)
{
    पूर्णाङ्क		c;
    पूर्णाङ्क		l = 1;
    char_u	*p = *pp;

    यदि (p[0] != NUL && p[1] == '.' && p[2] != NUL)
    {
	यदि (has_mbyte)
	    l = (*mb_ptr2len)(p + 2);
	यदि (p[l + 2] == '.' && p[l + 3] == ']')
	{
	    यदि (has_mbyte)
		c = mb_ptr2char(p + 2);
	    अन्यथा
		c = p[2];
	    *pp += l + 4;
	    प्रत्यागम c;
	}
    }
    प्रत्यागम 0;
}

स्थिर पूर्णाङ्क reg_cpo_lit; // 'cpoptions' contains 'l' flag
स्थिर पूर्णाङ्क reg_cpo_bsl; // 'cpoptions' contains '\' flag

    स्थिर शून्य
get_cpo_flags(शून्य)
{
    reg_cpo_lit = vim_strchr(p_cpo, CPO_LITERAL) != रिक्त;
    reg_cpo_bsl = vim_strchr(p_cpo, CPO_BACKSL) != रिक्त;
}

/*
 * Skip over a "[]" range.
 * "p" must point to the character after the '['.
 * The returned pointer is on the matching ']', or the terminating NUL.
 */
    स्थिर char_u *
skip_anyof(char_u *p)
{
    पूर्णाङ्क		l;

    यदि (*p == '^')	// Complement of range.
	++p;
    यदि (*p == ']' || *p == '-')
	++p;
    यावत् (*p != NUL && *p != ']')
    {
	यदि (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	    p += l;
	अन्यथा
	    यदि (*p == '-')
	    {
		++p;
		यदि (*p != ']' && *p != NUL)
		    MB_PTR_ADV(p);
	    }
	अन्यथा यदि (*p == '\\'
		&& !reg_cpo_bsl
		&& (vim_strchr(REGEXP_INRANGE, p[1]) != रिक्त
		    || (!reg_cpo_lit && vim_strchr(REGEXP_ABBR, p[1]) != रिक्त)))
	    p += 2;
	अन्यथा यदि (*p == '[')
	{
	    यदि (get_char_class(&p) == CLASS_NONE
		    && get_equi_class(&p) == 0
		    && get_coll_element(&p) == 0
		    && *p != NUL)
		++p; // it is not a class name and not NUL
	}
	अन्यथा
	    ++p;
    }

    प्रत्यागम p;
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
    पूर्णाङ्क		delim,
    पूर्णाङ्क		magic)
{
    प्रत्यागम skip_regexp_ex(startp, delim, magic, रिक्त, रिक्त, रिक्त);
}

/*
 * Call skip_regexp() and when the delimiter does not match give an error and
 * return NULL.
 */
    char_u *
skip_regexp_err(
    char_u	*startp,
    पूर्णाङ्क		delim,
    पूर्णाङ्क		magic)
{
    char_u *p = skip_regexp(startp, delim, magic);

    यदि (*p != delim)
    {
	semsg(_(e_missing_delimiter_after_खोज_pattern_str), startp);
	प्रत्यागम रिक्त;
    }
    प्रत्यागम p;
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
    पूर्णाङ्क		dirc,
    पूर्णाङ्क		magic,
    char_u	**newp,
    पूर्णाङ्क		*dropped,
    magic_T	*magic_val)
{
    magic_T	mymagic;
    char_u	*p = startp;
    size_t	startplen = 0;

    यदि (magic)
	mymagic = MAGIC_ON;
    अन्यथा
	mymagic = MAGIC_OFF;
    get_cpo_flags();

    प्रति (; p[0] != NUL; MB_PTR_ADV(p))
    {
	यदि (p[0] == dirc)	// found end of regexp
	    भङ्ग;
	यदि ((p[0] == '[' && mymagic >= MAGIC_ON)
		|| (p[0] == '\\' && p[1] == '[' && mymagic <= MAGIC_OFF))
	{
	    p = skip_anyof(p + 1);
	    यदि (p[0] == NUL)
		भङ्ग;
	}
	अन्यथा यदि (p[0] == '\\' && p[1] != NUL)
	{
	    यदि (dirc == '?' && newp != रिक्त && p[1] == '?')
	    {
		// change "\?" to "?", make a copy first.
		यदि (startplen == 0)
		    startplen = STRLEN(startp);
		यदि (*newp == रिक्त)
		{
		    *newp = vim_strnsave(startp, startplen);
		    यदि (*newp != रिक्त)
		    {
			p = *newp + (p - startp);
			startp = *newp;
		    }
		}
		यदि (dropped != रिक्त)
		    ++*dropped;
		यदि (*newp != रिक्त)
		    mch_memmove(p, p + 1, startplen - ((p + 1) - startp) + 1);
		अन्यथा
		    ++p;
	    }
	    अन्यथा
		++p;    // skip next character
	    यदि (*p == 'v')
		mymagic = MAGIC_ALL;
	    अन्यथा यदि (*p == 'V')
		mymagic = MAGIC_NONE;
	}
    }
    यदि (magic_val != रिक्त)
	*magic_val = mymagic;
    प्रत्यागम p;
}

/*
 * Functions for getting characters from the regexp input.
 */
स्थिर पूर्णाङ्क	prevchr_len;	// byte length of previous char
स्थिर पूर्णाङ्क	at_start;	// True when on the first character
स्थिर पूर्णाङ्क	prev_at_start;  // True when on the second character

/*
 * Start parsing at "str".
 */
    स्थिर शून्य
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
    स्थिर शून्य
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
    स्थिर शून्य
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
    स्थिर पूर्णाङ्क
peekchr(शून्य)
{
    स्थिर पूर्णाङ्क	after_slash = FALSE;

    यदि (curchr != -1)
	प्रत्यागम curchr;

    विकल्प (curchr = regparse[0])
    {
	पक्ष '.':
	पक्ष '[':
	पक्ष '~':
	    // magic when 'magic' is on
	    यदि (reg_magic >= MAGIC_ON)
		curchr = Magic(curchr);
	    भङ्ग;
	पक्ष '(':
	पक्ष ')':
	पक्ष '{':
	पक्ष '%':
	पक्ष '+':
	पक्ष '=':
	पक्ष '?':
	पक्ष '@':
	पक्ष '!':
	पक्ष '&':
	पक्ष '|':
	पक्ष '<':
	पक्ष '>':
	पक्ष '#':	// future ext.
	पक्ष '"':	// future ext.
	पक्ष '\'':	// future ext.
	पक्ष ',':	// future ext.
	पक्ष '-':	// future ext.
	पक्ष ':':	// future ext.
	पक्ष ';':	// future ext.
	पक्ष '`':	// future ext.
	पक्ष '/':	// Can't be used in / command
			// magic only after "\v"
	    यदि (reg_magic == MAGIC_ALL)
		curchr = Magic(curchr);
	    भङ्ग;
	पक्ष '*':
	    // * is not magic as the very first character, eg "?*ptr", when
	    // after '^', eg "/^*ptr" and when after "\(", "\|", "\&".  But
	    // "\(\*" is not magic, thus must be magic if "after_slash"
	    यदि (reg_magic >= MAGIC_ON
		    && !at_start
		    && !(prev_at_start && prevchr == Magic('^'))
		    && (after_slash
			|| (prevchr != Magic('(')
			    && prevchr != Magic('&')
			    && prevchr != Magic('|'))))
		curchr = Magic('*');
	    भङ्ग;
	पक्ष '^':
	    // '^' is only magic as the very first character and if it's after
	    // "\(", "\|", "\&' or "\n"
	    यदि (reg_magic >= MAGIC_OFF
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
	    भङ्ग;
	पक्ष '$':
	    // '$' is only magic as the very last char and if it's in front of
	    // either "\|", "\)", "\&", or "\n"
	    यदि (reg_magic >= MAGIC_OFF)
	    {
		char_u *p = regparse + 1;
		पूर्णाङ्क is_magic_all = (reg_magic == MAGIC_ALL);

		// ignore \c \C \m \M \v \V and \Z after '$'
		यावत् (p[0] == '\\' && (p[1] == 'c' || p[1] == 'C'
			    || p[1] == 'm' || p[1] == 'M'
			    || p[1] == 'v' || p[1] == 'V' || p[1] == 'Z'))
		{
		    यदि (p[1] == 'v')
			is_magic_all = TRUE;
		    अन्यथा यदि (p[1] == 'm' || p[1] == 'M' || p[1] == 'V')
			is_magic_all = FALSE;
		    p += 2;
		}
		यदि (p[0] == NUL
			|| (p[0] == '\\'
			    && (p[1] == '|' || p[1] == '&' || p[1] == ')'
				|| p[1] == 'n'))
			|| (is_magic_all
			    && (p[0] == '|' || p[0] == '&' || p[0] == ')'))
			|| reg_magic == MAGIC_ALL)
		    curchr = Magic('$');
	    }
	    भङ्ग;
	पक्ष '\\':
	    {
		पूर्णाङ्क c = regparse[1];

		यदि (c == NUL)
		    curchr = '\\';	// trailing '\'
		अन्यथा यदि (c <= '~' && META_flags[c])
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
		अन्यथा यदि (vim_strchr(REGEXP_ABBR, c))
		{
		    /*
		     * Handle abbreviations, like "\t" for TAB -- webb
		     */
		    curchr = backslash_trans(c);
		}
		अन्यथा यदि (reg_magic == MAGIC_NONE && (c == '$' || c == '^'))
		    curchr = toggle_Magic(c);
		अन्यथा
		{
		    /*
		     * Next character can never be (made) magic?
		     * Then backslashing it won't do anything.
		     */
		    यदि (has_mbyte && c >= 0x80)
			curchr = (*mb_ptr2char)(regparse + 1);
		    अन्यथा
			curchr = c;
		}
		भङ्ग;
	    }

	स्वभाव:
	    // curchr already holds regparse[0]; only a multi-byte lead byte
	    // needs decoding.
	    यदि (has_mbyte && curchr >= 0x80)
		curchr = (*mb_ptr2char)(regparse);
    }

    प्रत्यागम curchr;
}

/*
 * Eat one lexed character.  Do this in a way that we can undo it.
 */
    स्थिर शून्य
skipchr(शून्य)
{
    // peekchr() eats a backslash, do the same here
    यदि (*regparse == '\\')
	prevchr_len = 1;
    अन्यथा
	prevchr_len = 0;
    यदि (regparse[prevchr_len] != NUL)
    {
	यदि (enc_utf8)
	{
	    // Exclude composing chars that mb_ptr2len does include.  A byte
	    // below 0x80 is always a single character.
	    यदि (regparse[prevchr_len] < 0x80)
		++prevchr_len;
	    अन्यथा
		prevchr_len += utf_ptr2len(regparse + prevchr_len);
	}
	अन्यथा यदि (has_mbyte)
	    prevchr_len += (*mb_ptr2len)(regparse + prevchr_len);
	अन्यथा
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
    स्थिर शून्य
skipchr_keepstart(शून्य)
{
    पूर्णाङ्क as = prev_at_start;
    पूर्णाङ्क pr = prevchr;
    पूर्णाङ्क prpr = prevprevchr;

    skipchr();
    at_start = as;
    prevchr = pr;
    prevprevchr = prpr;
}

/*
 * Get the next character from the pattern. We know about magic and such, so
 * therefore we need a lexical analyzer.
 */
    स्थिर पूर्णाङ्क
getchr(शून्य)
{
    पूर्णाङ्क chr = peekchr();

    skipchr();
    प्रत्यागम chr;
}

/*
 * put character back.  Works only once!
 */
    स्थिर शून्य
ungetchr(शून्य)
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
    स्थिर vimlong_T
gethexchrs(पूर्णाङ्क maxinputlen)
{
    long_u	nr = 0;
    पूर्णाङ्क		c;
    पूर्णाङ्क		i;

    प्रति (i = 0; i < maxinputlen; ++i)
    {
	c = regparse[0];
	यदि (!vim_isxdigit(c))
	    भङ्ग;
	nr <<= 4;
	nr |= hex2nr(c);
	++regparse;
    }

    यदि (i == 0)
	प्रत्यागम -1;
    प्रत्यागम nr;
}

/*
 * Get and return the value of the decimal string immediately after the
 * current position. Return -1 for invalid.  Consumes all digits.
 */
    स्थिर vimlong_T
getdecchrs(शून्य)
{
    long_u	nr = 0;
    पूर्णाङ्क		c;
    पूर्णाङ्क		i;

    प्रति (i = 0; ; ++i)
    {
	c = regparse[0];
	यदि (c < '0' || c > '9')
	    भङ्ग;
	nr *= 10;
	nr += c - '0';
	++regparse;
	curchr = -1; // no longer valid
    }

    यदि (i == 0)
	प्रत्यागम -1;
    प्रत्यागम nr;
}

/*
 * get and return the value of the octal string immediately after the current
 * position. Return -1 for invalid, or 0-255 for valid. Smart enough to handle
 * numbers > 377 correctly (for example, 400 is treated as 40) and doesn't
 * treat 8 or 9 as recognised characters. Position is updated:
 *     blahblah\%o210asdf
 *	   before-^  ^-after
 */
    स्थिर long
getoctchrs(शून्य)
{
    long_u	nr = 0;
    पूर्णाङ्क		c;
    पूर्णाङ्क		i;

    प्रति (i = 0; i < 3 && nr < 040; ++i)
    {
	c = regparse[0];
	यदि (c < '0' || c > '7')
	    भङ्ग;
	nr <<= 3;
	nr |= hex2nr(c);
	++regparse;
    }

    यदि (i == 0)
	प्रत्यागम -1;
    प्रत्यागम (long)nr;
}

/*
 * read_limits - Read two integers to be taken as a minimum and maximum.
 * If the first character is '-', then the range is reversed.
 * Should end with 'end'.  If minval is missing, zero is default, if maxval is
 * missing, a very big number is the default.
 */
    स्थिर पूर्णाङ्क
read_limits(long *minval, long *maxval)
{
    पूर्णाङ्क		reverse = FALSE;
    char_u	*first_char;
    long	tmp;

    यदि (*regparse == '-')
    {
	// Starts with '-', so reverse the range later
	regparse++;
	reverse = TRUE;
    }
    first_char = regparse;
    *minval = getdigits(&regparse);
    यदि (*regparse == ',')	    // There is a comma
    {
	यदि (vim_isdigit(*++regparse))
	    *maxval = getdigits(&regparse);
	अन्यथा
	    *maxval = MAX_LIMIT;
    }
    अन्यथा यदि (VIM_ISDIGIT(*first_char))
	*maxval = *minval;	    // It was \{n} or \{-n}
    अन्यथा
	*maxval = MAX_LIMIT;	    // It was \{} or \{-}
    यदि (*regparse == '\\')
	regparse++;	// Allow either \{...} or \{...\}
    यदि (*regparse != '}')
	EMSG2_RET_FAIL(_(e_syntax_error_in_str_curlies),
						       reg_magic == MAGIC_ALL);

    /*
     * Reverse the range if there was a '-', or make sure it is in the right
     * order otherwise.
     */
    यदि ((!reverse && *minval > *maxval) || (reverse && *minval < *maxval))
    {
	tmp = *minval;
	*minval = *maxval;
	*maxval = tmp;
    }
    skipchr();		// let's be friends with the lexer again
    प्रत्यागम OK;
}

/*
 * vim_regexec and friends
 */

/*
 * Global work variables for vim_regexec().
 */

स्थिर शून्य	cleanup_subexpr(शून्य);
#ifdef FEAT_SYN_HL
स्थिर शून्य	cleanup_zsubexpr(शून्य);
#endif
स्थिर पूर्णाङ्क	match_with_backref(linenr_T start_lnum, colnr_T start_col, linenr_T end_lnum, colnr_T end_col, पूर्णाङ्क *bytelen);

/*
 * Sometimes need to save a copy of a line.  Since alloc()/free() is very
 * slow, we keep one allocated piece of memory and only re-allocate it when
 * it's too small.  It's freed in bt_regexec_both() when finished.
 */
स्थिर char_u	*reg_tofree = रिक्त;
स्थिर unsigned	reg_tofreelen;

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
प्रकारनिर्देश संरचना {
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
    पूर्णाङ्क			reg_पंक्ति_lbr;	// "\n" in string is line break

    // The current match-position is stord in these variables:
    linenr_T	lnum;		// line number, relative to first line
    char_u	*पंक्ति;		// start of current line
    char_u	*निवेश;		// current input, points into "line"

    पूर्णाङ्क	need_clear_subexpr;	// subexpressions still need to be cleared
#ifdef FEAT_SYN_HL
    पूर्णाङ्क	need_clear_zsubexpr;	// extmatch subexpressions still need to be
				// cleared
#endif

    // Internal copy of 'ignorecase'.  It is set at each call to vim_regexec().
    // Normally it gets the value of "rm_ic" or "rmm_ic", but when the pattern
    // contains '\c' or '\C' the value is overruled.
    पूर्णाङ्क			reg_ic;

    // Similar to "reg_ic", but only for 'combining' characters.  Set with \Z
    // flag in the regexp.  Defaults to false, always.
    पूर्णाङ्क			reg_icombine;

    // Copy of "rmm_maxcol": maximum column to search for a match.  Zero when
    // there is no maximum.
    colnr_T		reg_maxcol;

    // State for the NFA engine regexec.
    पूर्णाङ्क nfa_has_zend;	    // NFA regexp \ze operator encountered.
    पूर्णाङ्क nfa_has_backref;    // NFA regexp \1 .. \9 encountered.
    पूर्णाङ्क nfa_nsubexpr;	    // Number of sub expressions actually being used
			    // during execution. 1 if only the whole match
			    // (subexpr 0) is used.
    // listid is global, so that it increases on recursive calls to
    // nfa_regmatch(), which means we don't have to clear the lastlist field of
    // all the states.
    पूर्णाङ्क nfa_listid;
    पूर्णाङ्क nfa_alt_listid;

#ifdef FEAT_SYN_HL
    पूर्णाङ्क nfa_has_zsubexpr;   // NFA regexp has \z( ), set zsubexpr.
#endif
} regexec_T;

स्थिर regexec_T	rex;
स्थिर पूर्णाङ्क		rex_in_use = FALSE;

/*
 * Return TRUE if character 'c' is included in 'iskeyword' option for
 * "reg_buf" buffer.
 */
    स्थिर पूर्णाङ्क
reg_iswordc(पूर्णाङ्क c)
{
    प्रत्यागम vim_iswordc_buf(c, rex.reg_buf);
}

#ifdef FEAT_EVAL
स्थिर पूर्णाङ्क can_f_submatch = FALSE;	// TRUE when submatch() can be used

// This struct is used for reg_submatch(). Needed for when the
// substitution string is an expression that contains a call to substitute()
// and submatch().
प्रकारनिर्देश संरचना {
    regmatch_T	*sm_match;
    regmmatch_T	*sm_mmatch;
    linenr_T	sm_firstlnum;
    linenr_T	sm_maxline;
    पूर्णाङ्क		sm_पंक्ति_lbr;
} regsubmatch_T;

स्थिर regsubmatch_T rsm;  // can only be used when can_f_submatch is TRUE
#endif

प्रकारनिर्देश गणना
{
    RGLF_पंक्ति = 0x01,
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
    स्थिर शून्य
reg_getline_common(
    linenr_T		lnum,
    reg_getline_flags_T	flags,
    char_u		**पंक्ति,
    colnr_T		*length)
{
    पूर्णाङ्क get_पंक्ति = flags & RGLF_पंक्ति;
    पूर्णाङ्क get_length = flags & RGLF_LENGTH;
    linenr_T firstlnum;
    linenr_T maxline;

#ifdef FEAT_EVAL
    यदि (flags & RGLF_SUBMATCH)
    {
	firstlnum = rsm.sm_firstlnum + lnum;
	maxline = rsm.sm_maxline;
    }
    अन्यथा
#endif
    {
	firstlnum = rex.reg_firstlnum + lnum;
	maxline = rex.reg_maxline;
    }

    // when looking behind for a match/no-match lnum is negative. but we
    // can't go before line 1.
    यदि (firstlnum < 1)
    {
	यदि (get_पंक्ति)
	    *पंक्ति = रिक्त;
	यदि (get_length)
	    *length = 0;

	प्रत्यागम;
    }

    यदि (lnum > maxline)
    {
	// must have matched the "\n" in the last line.
	यदि (get_पंक्ति)
	    *पंक्ति = (char_u *)"";
	यदि (get_length)
	    *length = 0;

	प्रत्यागम;
    }

    यदि (get_पंक्ति)
	*पंक्ति = ml_get_buf(rex.reg_buf, firstlnum, FALSE);
    यदि (get_length)
	*length = ml_get_buf_len(rex.reg_buf, firstlnum);
}

/*
 * Get pointer to the line "lnum", which is relative to "reg_firstlnum".
 */
    स्थिर char_u *
reg_getline(linenr_T lnum)
{
    char_u *पंक्ति;

    reg_getline_common(lnum, RGLF_पंक्ति, &पंक्ति, रिक्त);

    प्रत्यागम पंक्ति;
}

/*
 * Get length of line "lnum", which is relative to "reg_firstlnum".
 */
    स्थिर colnr_T
reg_getline_len(linenr_T lnum)
{
    colnr_T length;

    reg_getline_common(lnum, RGLF_LENGTH, रिक्त, &length);

    प्रत्यागम length;
}

#ifdef FEAT_SYN_HL
स्थिर char_u	*reg_startzp[NSUBEXP];	// Workspace to mark beginning
स्थिर char_u	*reg_endzp[NSUBEXP];	//   and end of \z(...\) matches
स्थिर lpos_T	reg_startzpos[NSUBEXP];	// idem, beginning pos
स्थिर lpos_T	reg_endzpos[NSUBEXP];	// idem, end pos
#endif

// TRUE if using multi-line regexp.
#define REG_MULTI	(rex.reg_match == रिक्त)

#ifdef FEAT_SYN_HL
/*
 * Create a new extmatch and mark it as referenced once.
 */
    स्थिर reg_extmatch_T *
make_extmatch(शून्य)
{
    reg_extmatch_T	*em;

    em = ALLOC_CLEAR_ONE(reg_extmatch_T);
    यदि (em != रिक्त)
	em->refcnt = 1;
    प्रत्यागम em;
}

/*
 * Add a reference to an extmatch.
 */
    reg_extmatch_T *
ref_extmatch(reg_extmatch_T *em)
{
    यदि (em != रिक्त)
	em->refcnt++;
    प्रत्यागम em;
}

/*
 * Remove a reference to an extmatch.  If there are no references left, free
 * the info.
 */
    शून्य
unref_extmatch(reg_extmatch_T *em)
{
    पूर्णाङ्क i;

    यदि (em != रिक्त && --em->refcnt <= 0)
    {
	प्रति (i = 0; i < NSUBEXP; ++i)
	    vim_free(em->matches[i]);
	vim_free(em);
    }
}
#endif

/*
 * Get class of previous character.
 */
    स्थिर पूर्णाङ्क
reg_prev_class(शून्य)
{
    यदि (rex.निवेश > rex.पंक्ति)
	प्रत्यागम mb_get_class_buf(rex.निवेश - 1
		       - (*mb_head_off)(rex.पंक्ति, rex.निवेश - 1), rex.reg_buf);
    प्रत्यागम -1;
}

/*
 * Return TRUE if the current rex.input position matches the Visual area.
 */
    स्थिर पूर्णाङ्क
reg_match_visual(शून्य)
{
    pos_T	शीर्ष, bot;
    linenr_T    lnum;
    colnr_T	col;
    win_T	*wp = rex.reg_win == रिक्त ? curwin : rex.reg_win;
    पूर्णाङ्क		mode;
    colnr_T	start, end;
    colnr_T	start2, end2;
    colnr_T	cols;
    colnr_T	curswant;

    // Check if the buffer is the current buffer and not using a string.
    यदि (rex.reg_buf != curbuf || VIsual.lnum == 0 || !REG_MULTI)
	प्रत्यागम FALSE;

    यदि (VIsual_active)
    {
	यदि (LT_POS(VIsual, wp->w_cursor))
	{
	    शीर्ष = VIsual;
	    bot = wp->w_cursor;
	}
	अन्यथा
	{
	    शीर्ष = wp->w_cursor;
	    bot = VIsual;
	}
	mode = VIsual_mode;
	curswant = wp->w_curswant;
    }
    अन्यथा
    {
	यदि (LT_POS(curbuf->b_visual.vi_start, curbuf->b_visual.vi_end))
	{
	    शीर्ष = curbuf->b_visual.vi_start;
	    bot = curbuf->b_visual.vi_end;
	}
	अन्यथा
	{
	    शीर्ष = curbuf->b_visual.vi_end;
	    bot = curbuf->b_visual.vi_start;
	}
	// a substitute command may have removed some lines
	यदि (bot.lnum > curbuf->b_ml.ml_पंक्ति_गिनती)
	    bot.lnum = curbuf->b_ml.ml_पंक्ति_गिनती;
	mode = curbuf->b_visual.vi_mode;
	curswant = curbuf->b_visual.vi_curswant;
    }
    lnum = rex.lnum + rex.reg_firstlnum;
    यदि (lnum < शीर्ष.lnum || lnum > bot.lnum)
	प्रत्यागम FALSE;

    col = (colnr_T)(rex.निवेश - rex.पंक्ति);
    यदि (mode == 'v')
    {
	यदि ((lnum == शीर्ष.lnum && col < शीर्ष.col)
		|| (lnum == bot.lnum && col >= bot.col + (*p_sel != 'e')))
	    प्रत्यागम FALSE;
    }
    अन्यथा यदि (mode == Ctrl_V)
    {
	getvvcol(wp, &शीर्ष, &start, रिक्त, &end, 0);
	getvvcol(wp, &bot, &start2, रिक्त, &end2, 0);
	यदि (start2 < start)
	    start = start2;
	यदि (end2 > end)
	    end = end2;
	यदि (शीर्ष.col == MAXCOL || bot.col == MAXCOL || curswant == MAXCOL)
	    end = MAXCOL;

	// getvvcol() flushes rex.line, need to get it again
	rex.पंक्ति = reg_getline(rex.lnum);
	rex.निवेश = rex.पंक्ति + col;

	cols = win_linetabsize(wp, rex.reg_firstlnum + rex.lnum, rex.पंक्ति, col);
	यदि (cols < start || cols > end - (*p_sel == 'e'))
	    प्रत्यागम FALSE;
    }
    प्रत्यागम TRUE;
}

/*
 * Check the regexp program for its magic number.
 * Return TRUE if it's wrong.
 */
    स्थिर पूर्णाङ्क
prog_magic_wrong(शून्य)
{
    regprog_T	*prog;

    prog = REG_MULTI ? rex.reg_mmatch->regprog : rex.reg_match->regprog;
    यदि (prog->engine == &nfa_regengine)
	// For NFA matcher we don't check the magic
	प्रत्यागम FALSE;

    यदि (UCHARAT(((bt_regprog_T *)prog)->program) != REGMAGIC)
    {
	iemsg(e_corrupted_regexp_program);
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

/*
 * Cleanup the subexpressions, if this wasn't done yet.
 * This construction is used to clear the subexpressions only when they are
 * used (to increase speed).
 */
    स्थिर शून्य
cleanup_subexpr(शून्य)
{
    यदि (!rex.need_clear_subexpr)
	प्रत्यागम;

    यदि (REG_MULTI)
    {
	// Use 0xff to set lnum to -1
	vim_memset(rex.reg_startpos, 0xff, परिमाण(lpos_T) * NSUBEXP);
	vim_memset(rex.reg_endpos, 0xff, परिमाण(lpos_T) * NSUBEXP);
    }
    अन्यथा
    {
	vim_memset(rex.reg_startp, 0, परिमाण(char_u *) * NSUBEXP);
	vim_memset(rex.reg_endp, 0, परिमाण(char_u *) * NSUBEXP);
    }
    rex.need_clear_subexpr = FALSE;
}

#ifdef FEAT_SYN_HL
    स्थिर शून्य
cleanup_zsubexpr(शून्य)
{
    यदि (!rex.need_clear_zsubexpr)
	प्रत्यागम;

    यदि (REG_MULTI)
    {
	// Use 0xff to set lnum to -1
	vim_memset(reg_startzpos, 0xff, परिमाण(lpos_T) * NSUBEXP);
	vim_memset(reg_endzpos, 0xff, परिमाण(lpos_T) * NSUBEXP);
    }
    अन्यथा
    {
	vim_memset(reg_startzp, 0, परिमाण(char_u *) * NSUBEXP);
	vim_memset(reg_endzp, 0, परिमाण(char_u *) * NSUBEXP);
    }
    rex.need_clear_zsubexpr = FALSE;
}
#endif

/*
 * Advance rex.lnum, rex.line and rex.input to the next line.
 */
    स्थिर शून्य
reg_nextline(शून्य)
{
    rex.पंक्ति = reg_getline(++rex.lnum);
    rex.निवेश = rex.पंक्ति;
    fast_breakcheck();
}

/*
 * Check whether a backreference matches.
 * Returns RA_FAIL, RA_NOMATCH or RA_MATCH.
 * If "bytelen" is not NULL, it is set to the byte length of the match in the
 * last line.
 * Optional: ignore case if rex.reg_ic is set.
 */
    स्थिर पूर्णाङ्क
match_with_backref(
    linenr_T start_lnum,
    colnr_T  start_col,
    linenr_T end_lnum,
    colnr_T  end_col,
    पूर्णाङ्क	     *bytelen)
{
    linenr_T	clnum = start_lnum;
    colnr_T	ccol = start_col;
    पूर्णाङ्क		len;
    char_u	*p;

    यदि (bytelen != रिक्त)
	*bytelen = 0;
    प्रति (;;)
    {
	// Since getting one line may invalidate the other, need to make copy.
	// Slow!
	यदि (rex.पंक्ति != reg_tofree)
	{
	    len = (पूर्णाङ्क)STRLEN(rex.पंक्ति);
	    यदि (reg_tofree == रिक्त || len >= (पूर्णाङ्क)reg_tofreelen)
	    {
		len += 50;	// get some extra
		vim_free(reg_tofree);
		reg_tofree = alloc(len);
		यदि (reg_tofree == रिक्त)
		    प्रत्यागम RA_FAIL; // out of memory!
		reg_tofreelen = len;
	    }
	    STRCPY(reg_tofree, rex.पंक्ति);
	    rex.निवेश = reg_tofree + (rex.निवेश - rex.पंक्ति);
	    rex.पंक्ति = reg_tofree;
	}

	// Get the line to compare with.
	p = reg_getline(clnum);
	यदि (clnum == end_lnum)
	    len = end_col - ccol;
	अन्यथा
	    len = (पूर्णाङ्क)reg_getline_len(clnum) - ccol;

	// Use case-insensitive compare if rex.reg_ic is set
	यदि ((!rex.reg_ic && cstrncmp(p + ccol, rex.निवेश, &len) != 0)
	    || (rex.reg_ic && MB_STRNICMP(p + ccol, rex.निवेश, len) != 0))
	    प्रत्यागम RA_NOMATCH;  // doesn't match
	यदि (bytelen != रिक्त)
	    *bytelen += len;
	यदि (clnum == end_lnum)
	    भङ्ग;		// match and at end!
	यदि (rex.lnum >= rex.reg_maxline)
	    प्रत्यागम RA_NOMATCH;  // text too short

	// Advance to next line.
	reg_nextline();
	यदि (bytelen != रिक्त)
	    *bytelen = 0;
	++clnum;
	ccol = 0;
	यदि (got_int)
	    प्रत्यागम RA_FAIL;
    }

    // found a match!  Note that rex.line may now point to a copy of the line,
    // that should not matter.
    प्रत्यागम RA_MATCH;
}

/*
 * Used in a place where no * or \+ can follow.
 */
    स्थिर पूर्णाङ्क
re_mult_next(वर्ण *what)
{
    यदि (re_multi_type(peekchr()) == MULTI_MULT)
    {
	semsg(_(e_nfa_regexp_cannot_repeat_str), what);
	rc_did_emsg = TRUE;
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

प्रकारनिर्देश संरचना
{
    पूर्णाङ्क a, b, c;
} decomp_T;


// 0xfb20 - 0xfb4f
स्थिर decomp_T decomp_table[0xfb4f-0xfb20+1] =
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

    स्थिर शून्य
mb_decompose(पूर्णाङ्क c, पूर्णाङ्क *c1, पूर्णाङ्क *c2, पूर्णाङ्क *c3)
{
    decomp_T d;

    यदि (c >= 0xfb20 && c <= 0xfb4f)
    {
	d = decomp_table[c - 0xfb20];
	*c1 = d.a;
	*c2 = d.b;
	*c3 = d.c;
    }
    अन्यथा
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
    स्थिर पूर्णाङ्क
cstrncmp(char_u *s1, char_u *s2, पूर्णाङ्क *n)
{
    पूर्णाङ्क		result;

    यदि (!rex.reg_ic)
	result = STRNCMP(s1, s2, *n);
    अन्यथा यदि (enc_utf8)
    {
	char_u *p = s1;
	पूर्णाङ्क n2 = 0;
	पूर्णाङ्क n1 = *n;
	// count the number of characters for byte-length of s1
	यावत् (n1 > 0 && *p != NUL)
	{
	    n1 -= mb_ptr2len(p);
	    MB_PTR_ADV(p);
	    n2++;
	}
	// count the number of bytes to advance the same number of chars for s2
	p = s2;
	यावत् (n2-- > 0 && *p != NUL)
	    MB_PTR_ADV(p);

	n2 = p - s2;

	result = MB_STRNICMP2(s1, s2, *n, n2);
	यदि (result == 0)
	    *n = n2;
    }
    अन्यथा
	result = MB_STRNICMP(s1, s2, *n);

    // if it failed and it's utf8 and we want to combineignore:
    यदि (result != 0 && enc_utf8 && rex.reg_icombine)
    {
	char_u	*str1, *str2;
	पूर्णाङ्क	c1, c2, c11, c12;
	पूर्णाङ्क	junk;

	// we have to handle the strcmp ourselves, since it is necessary to
	// deal with the composing characters by ignoring them:
	str1 = s1;
	str2 = s2;
	c1 = c2 = 0;
	यावत् ((पूर्णाङ्क)(str1 - s1) < *n)
	{
	    c1 = mb_ptr2char_adv(&str1);
	    c2 = mb_ptr2char_adv(&str2);

	    // Decompose the character if necessary, into 'base' characters.
	    // Currently hard-coded for Hebrew, Arabic to be done...
	    यदि (c1 != c2 && (!rex.reg_ic || utf_fold(c1) != utf_fold(c2)))
	    {
		// decomposition necessary?
		mb_decompose(c1, &c11, &junk, &junk);
		mb_decompose(c2, &c12, &junk, &junk);
		c1 = c11;
		c2 = c12;
		यदि (c11 != c12
			    && (!rex.reg_ic || utf_fold(c11) != utf_fold(c12)))
		    भङ्ग;
	    }
	}
	result = c2 - c1;
	यदि (result == 0)
	    *n = (पूर्णाङ्क)(str2 - s2);
    }

    प्रत्यागम result;
}

/*
 * cstrchr: This function is used a lot for simple searches, keep it fast!
 */
    स्थिर char_u *
cstrchr(char_u *s, पूर्णाङ्क c)
{
    char_u	*p;
    पूर्णाङ्क		cc, lc;

    यदि (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	प्रत्यागम vim_strchr(s, c);

    // tolower() and toupper() can be slow, comparing twice should be a lot
    // faster (esp. when using MS Visual C++!).
    // For UTF-8 need to use folded case.
    यदि (enc_utf8 && c > 0x80)
    {
	cc = utf_fold(c);
	lc = cc;
    }
    अन्यथा
	यदि (MB_ISUPPER(c))
	{
	    cc = MB_TOLOWER(c);
	    lc = cc;
	}
	अन्यथा यदि (MB_ISLOWER(c))
	{
	    cc = MB_TOUPPER(c);
	    lc = c;
	}
	अन्यथा
	    प्रत्यागम vim_strchr(s, c);

    यदि (has_mbyte)
    {
	प्रति (p = s; *p != NUL; p += (*mb_ptr2len)(p))
	{
	    पूर्णाङ्क uc = utf_ptr2char(p);
	    यदि (enc_utf8 && (c > 0x80 || uc > 0x80))
	    {
		// Do not match an illegal byte.  E.g. 0xff matches 0xc3 0xbf,
		// not 0xff.
		// compare with lower case of the character
		यदि ((uc < 0x80 || uc != *p) && utf_fold(uc) == lc)
		    प्रत्यागम p;
	    }
	    अन्यथा यदि (*p == c || *p == cc)
		प्रत्यागम p;
	}
    }
    अन्यथा
	// Faster version for when there are no multi-byte characters.
	प्रति (p = s; *p != NUL; ++p)
	    यदि (*p == c || *p == cc)
		प्रत्यागम p;

    प्रत्यागम रिक्त;
}

////////////////////////////////////////////////////////////////
//		      regsub stuff			      //
////////////////////////////////////////////////////////////////

प्रकारनिर्देश शून्य (*fptr_T)(पूर्णाङ्क *, पूर्णाङ्क);

स्थिर पूर्णाङ्क vim_regsub_both(char_u *source, typval_T *व्यंजक, char_u *dest, पूर्णाङ्क destlen, पूर्णाङ्क flags);

    स्थिर शून्य
do_upper(पूर्णाङ्क *d, पूर्णाङ्क c)
{
    *d = MB_TOUPPER(c);
}

    स्थिर शून्य
do_lower(पूर्णाङ्क *d, पूर्णाङ्क c)
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
regtilde(char_u *source, पूर्णाङ्क magic)
{
    char_u	*newsub = source;
    char_u	*p;
    size_t	newsublen = 0;
    char_u	tilde[3] = {'~', NUL, NUL};
    size_t	tildelen = 1;
    पूर्णाङ्क		error = FALSE;

    यदि (!magic)
    {
	tilde[0] = '\\';
	tilde[1] = '~';
	tilde[2] = NUL;
	tildelen = 2;
    }

    प्रति (p = newsub; *p; ++p)
    {
	यदि (STRNCMP(p, tilde, tildelen) == 0)
	{
	    size_t prefixlen = p - newsub;		// not including the tilde
	    char_u *postfix = p + tildelen;
	    size_t postfixlen;
	    size_t tmpsublen;

	    यदि (newsublen == 0)
		newsublen = STRLEN(newsub);
	    newsublen -= tildelen;
	    postfixlen = newsublen - prefixlen;
	    tmpsublen = prefixlen + reg_prev_sublen + postfixlen;

	    यदि (tmpsublen > 0 && reg_prev_sub != रिक्त)
	    {
		char_u *tmpsub;

		// Avoid making the text longer than MAXCOL, it will cause
		// trouble at some point.
		यदि (tmpsublen > MAXCOL)
		{
		    emsg(_(e_resulting_text_too_long));
		    error = TRUE;
		    भङ्ग;
		}

		tmpsub = alloc(tmpsublen + 1);
		यदि (tmpsub == रिक्त)
		{
		    emsg(_(e_out_of_memory));
		    error = TRUE;
		    भङ्ग;
		}

		// copy prefix
		mch_memmove(tmpsub, newsub, prefixlen);
		// interpret tilde
		mch_memmove(tmpsub + prefixlen, reg_prev_sub, reg_prev_sublen);
		// copy postfix
		STRCPY(tmpsub + prefixlen + reg_prev_sublen, postfix);

		यदि (newsub != source)	// allocated newsub before
		    vim_free(newsub);
		newsub = tmpsub;
		newsublen = tmpsublen;
		p = newsub + prefixlen + reg_prev_sublen;
	    }
	    अन्यथा
		mch_memmove(p, postfix, postfixlen + 1);	// remove the tilde (+1 for the NUL)

	    --p;
	}
	अन्यथा
	{
	    यदि (*p == '\\' && p[1])		// skip escaped characters
		++p;
	    यदि (has_mbyte)
		p += (*mb_ptr2len)(p) - 1;
	}
    }

    यदि (error)
    {
	यदि (newsub != source)
	    vim_free(newsub);
	प्रत्यागम source;
    }

    // Store a copy of newsub  in reg_prev_sub.  It is always allocated,
    // because recursive calls may make the returned string invalid.
    // Only store it if there something to store.
    newsublen = p - newsub;
    यदि (newsublen == 0)
	VIM_CLEAR(reg_prev_sub);
    अन्यथा
    {
	vim_free(reg_prev_sub);
	reg_prev_sub = vim_strnsave(newsub, newsublen);
    }

    यदि (reg_prev_sub == रिक्त)
	reg_prev_sublen = 0;
    अन्यथा
	reg_prev_sublen = newsublen;

    प्रत्यागम newsub;
}

#ifdef FEAT_EVAL

/*
 * Put the submatches in "argv[argskip]" which is a list passed into
 * call_func() by vim_regsub_both().
 */
    स्थिर पूर्णाङ्क
fill_submatch_list(पूर्णाङ्क argc UNUSED, typval_T *argv, पूर्णाङ्क argskip, ufunc_T *fp)
{
    listitem_T	*li;
    पूर्णाङ्क		i;
    char_u	*s;
    typval_T	*listarg = argv + argskip;

    यदि (!has_varargs(fp) && fp->uf_args.ga_len <= argskip)
	// called function doesn't take a submatches argument
	प्रत्यागम argskip;

    // Relies on sl_list to be the first item in staticList10_T.
    init_static_list((staticList10_T *)(listarg->vval.v_list));

    // There are always 10 list items in staticList10_T.
    li = listarg->vval.v_list->lv_first;
    प्रति (i = 0; i < 10; ++i)
    {
	s = rsm.sm_match->startp[i];
	यदि (s == रिक्त || rsm.sm_match->endp[i] == रिक्त)
	    s = रिक्त;
	अन्यथा
	    s = vim_strnsave(s, rsm.sm_match->endp[i] - s);
	li->li_tv.v_type = VAR_STRING;
	li->li_tv.vval.v_string = s;
	li = li->li_next;
    }
    प्रत्यागम argskip + 1;
}

    स्थिर शून्य
clear_submatch_list(staticList10_T *sl)
{
    पूर्णाङ्क i;

    प्रति (i = 0; i < 10; ++i)
	vim_free(sl->sl_वस्तुएँ[i].li_tv.vval.v_string);
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
    पूर्णाङ्क
vim_regsub(
    regmatch_T	*rmp,
    char_u	*source,
    typval_T	*व्यंजक,
    char_u	*dest,
    पूर्णाङ्क		destlen,
    पूर्णाङ्क		flags)
{
    पूर्णाङ्क		result;
    regexec_T	rex_save;
    पूर्णाङ्क		rex_in_use_save = rex_in_use;

    यदि (rex_in_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_in_use = TRUE;

    rex.reg_match = rmp;
    rex.reg_mmatch = रिक्त;
    rex.reg_maxline = 0;
    rex.reg_buf = curbuf;
    rex.reg_पंक्ति_lbr = TRUE;
    result = vim_regsub_both(source, व्यंजक, dest, destlen, flags);

    rex_in_use = rex_in_use_save;
    यदि (rex_in_use)
	rex = rex_save;

    प्रत्यागम result;
}

    पूर्णाङ्क
vim_regsub_multi(
    regmmatch_T	*rmp,
    linenr_T	lnum,
    char_u	*source,
    char_u	*dest,
    पूर्णाङ्क		destlen,
    पूर्णाङ्क		flags)
{
    पूर्णाङ्क		result;
    regexec_T	rex_save;
    पूर्णाङ्क		rex_in_use_save = rex_in_use;

    यदि (rex_in_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_in_use = TRUE;

    rex.reg_match = रिक्त;
    rex.reg_mmatch = rmp;
    rex.reg_buf = curbuf;	// always works on the current buffer!
    rex.reg_firstlnum = lnum;
    rex.reg_maxline = curbuf->b_ml.ml_पंक्ति_गिनती - lnum;
    rex.reg_पंक्ति_lbr = FALSE;
    result = vim_regsub_both(source, रिक्त, dest, destlen, flags);

    rex_in_use = rex_in_use_save;
    यदि (rex_in_use)
	rex = rex_save;

    प्रत्यागम result;
}

#यदि defined(FEAT_EVAL)
// When nesting more than a couple levels it's probably a mistake.
# define MAX_REGSUB_NESTING 4
स्थिर char_u   *eval_result[MAX_REGSUB_NESTING] = {रिक्त, रिक्त, रिक्त, रिक्त};

# यदि defined(EXITFREE)
    शून्य
free_resub_eval_result(शून्य)
{
    पूर्णाङ्क i;

    प्रति (i = 0; i < MAX_REGSUB_NESTING; ++i)
	VIM_CLEAR(eval_result[i]);
}
# endif
#endif

    स्थिर पूर्णाङ्क
vim_regsub_both(
    char_u	*source,
    typval_T	*व्यंजक,
    char_u	*dest,
    पूर्णाङ्क		destlen,
    पूर्णाङ्क		flags)
{
    char_u	*src;
    char_u	*dst;
    char_u	*s;
    पूर्णाङ्क		c;
    पूर्णाङ्क		cc;
    पूर्णाङ्क		no = -1;
    fptr_T	func_all = (fptr_T)रिक्त;
    fptr_T	func_one = (fptr_T)रिक्त;
    linenr_T	clnum = 0;	// init for GCC
    पूर्णाङ्क		len = 0;	// init for GCC
#ifdef FEAT_EVAL
    स्थिर पूर्णाङ्क  nesting = 0;
    पूर्णाङ्क		nested;
#endif
    पूर्णाङ्क		copy = flags & REGSUB_COPY;

    // Be paranoid...
    यदि ((source == रिक्त && व्यंजक == रिक्त) || dest == रिक्त)
    {
	iemsg(e_null_argument);
	प्रत्यागम 0;
    }
    यदि (prog_magic_wrong())
	प्रत्यागम 0;
#ifdef FEAT_EVAL
    यदि (nesting == MAX_REGSUB_NESTING)
    {
	emsg(_(e_substitute_nesting_too_deep));
	प्रत्यागम 0;
    }
    nested = nesting;
#endif
    src = source;
    dst = dest;

    /*
     * When the substitute part starts with "\=" evaluate it as an expression.
     */
    यदि (व्यंजक != रिक्त || (source[0] == '\\' && source[1] == '='))
    {
#ifdef FEAT_EVAL
	// To make sure that the length doesn't change between checking the
	// length and copying the string, and to speed up things, the
	// resulting string is saved from the call with
	// "flags & REGSUB_COPY" == 0 to the call with
	// "flags & REGSUB_COPY" != 0.
	यदि (copy)
	{
	    यदि (eval_result[nested] != रिक्त)
	    {
		पूर्णाङ्क eval_len = (पूर्णाङ्क)STRLEN(eval_result[nested]);

		यदि (eval_len < destlen)
		{
		    STRCPY(dest, eval_result[nested]);
		    dst += eval_len;
		    VIM_CLEAR(eval_result[nested]);
		}
	    }
	}
	अन्यथा
	{
	    पूर्णाङ्क		    prev_can_f_submatch = can_f_submatch;
	    regsubmatch_T   rsm_save;

	    VIM_CLEAR(eval_result[nested]);

	    // The expression may contain substitute(), which calls us
	    // recursively.  Make sure submatch() gets the text from the first
	    // level.
	    यदि (can_f_submatch)
		rsm_save = rsm;
	    can_f_submatch = TRUE;
	    rsm.sm_match = rex.reg_match;
	    rsm.sm_mmatch = rex.reg_mmatch;
	    rsm.sm_firstlnum = rex.reg_firstlnum;
	    rsm.sm_maxline = rex.reg_maxline;
	    rsm.sm_पंक्ति_lbr = rex.reg_पंक्ति_lbr;

	    // Although unlikely, it is possible that the expression invokes a
	    // substitute command (it might fail, but still).  Therefore keep
	    // an array of eval results.
	    ++nesting;

	    यदि (व्यंजक != रिक्त)
	    {
		typval_T	argv[2];
		char_u		buf[NUMBUFLEN];
		typval_T	rettv;
		staticList10_T	matchList;
		funcexe_T	funcexe;

		rettv.v_type = VAR_STRING;
		rettv.vval.v_string = रिक्त;
		argv[0].v_type = VAR_LIST;
		argv[0].vval.v_list = &matchList.sl_list;
		matchList.sl_list.lv_len = 0;
		CLEAR_FIELD(funcexe);
		funcexe.fe_argv_func = fill_submatch_list;
		funcexe.fe_evaluate = TRUE;
		यदि (व्यंजक->v_type == VAR_FUNC)
		{
		    s = व्यंजक->vval.v_string;
		    call_func(s, -1, &rettv, 1, argv, &funcexe);
		}
		अन्यथा यदि (व्यंजक->v_type == VAR_PARTIAL)
		{
		    partial_T   *partial = व्यंजक->vval.v_partial;

		    s = partial_name(partial);
		    funcexe.fe_partial = partial;
		    call_func(s, -1, &rettv, 1, argv, &funcexe);
		}
		अन्यथा यदि (व्यंजक->v_type == VAR_INSTR)
		{
		    exe_typval_instr(व्यंजक, &rettv);
		}
		यदि (matchList.sl_list.lv_len > 0)
		    // fill_submatch_list() was called
		    clear_submatch_list(&matchList);

		यदि (rettv.v_type == VAR_UNKNOWN)
		    // something failed, no need to report another error
		    eval_result[nested] = रिक्त;
		अन्यथा
		{
		    eval_result[nested] = tv_get_string_buf_chk(&rettv, buf);
		    यदि (eval_result[nested] != रिक्त)
			eval_result[nested] = vim_strsave(eval_result[nested]);
		}
		clear_tv(&rettv);
	    }
	    अन्यथा यदि (substitute_instr != रिक्त)
		// Execute instructions from ISN_SUBSTITUTE.
		eval_result[nested] = exe_substitute_instr();
	    अन्यथा
		eval_result[nested] = eval_to_string(source + 2, TRUE, FALSE);
	    --nesting;

	    यदि (eval_result[nested] != रिक्त)
	    {
		पूर्णाङ्क had_backslash = FALSE;

		प्रति (s = eval_result[nested]; *s != NUL; MB_PTR_ADV(s))
		{
		    // Change NL to CR, so that it becomes a line break,
		    // unless called from vim_regexec_nl().
		    // Skip over a backslashed character.
		    यदि (*s == NL && !rsm.sm_पंक्ति_lbr)
			*s = CAR;
		    अन्यथा यदि (*s == '\\' && s[1] != NUL)
		    {
			++s;
			/* Change NL to CR here too, so that this works:
			 * :s/abc\\\ndef/\="aaa\\\nbbb"/  on text:
			 *   abc\
			 *   def
			 * Not when called from vim_regexec_nl().
			 */
			यदि (*s == NL && !rsm.sm_पंक्ति_lbr)
			    *s = CAR;
			had_backslash = TRUE;
		    }
		}
		यदि (had_backslash && (flags & REGSUB_BACKSLASH))
		{
		    // Backslashes will be consumed, need to double them.
		    s = vim_strsave_escaped(eval_result[nested], (char_u *)"\\");
		    यदि (s != रिक्त)
		    {
			vim_free(eval_result[nested]);
			eval_result[nested] = s;
		    }
		}

		dst += STRLEN(eval_result[nested]);
	    }

	    can_f_submatch = prev_can_f_submatch;
	    यदि (can_f_submatch)
		rsm = rsm_save;
	}
#endif
    }
    अन्यथा
      यावत् ((c = *src++) != NUL)
      {
	यदि (c == '&' && (flags & REGSUB_MAGIC))
	    no = 0;
	अन्यथा यदि (c == '\\' && *src != NUL)
	{
	    यदि (*src == '&' && !(flags & REGSUB_MAGIC))
	    {
		++src;
		no = 0;
	    }
	    अन्यथा यदि ('0' <= *src && *src <= '9')
	    {
		no = *src++ - '0';
	    }
	    अन्यथा यदि (vim_strchr((char_u *)"uUlLeE", *src))
	    {
		विकल्प (*src++)
		{
		पक्ष 'u':   func_one = do_upper;
			    अनुवर्त;
		पक्ष 'U':   func_all = do_upper;
			    अनुवर्त;
		पक्ष 'l':   func_one = do_lower;
			    अनुवर्त;
		पक्ष 'L':   func_all = do_lower;
			    अनुवर्त;
		पक्ष 'e':
		पक्ष 'E':   func_one = func_all = (fptr_T)रिक्त;
			    अनुवर्त;
		}
	    }
	}
	यदि (no < 0)	      // Ordinary character.
	{
	    यदि (c == K_SPECIAL && src[0] != NUL && src[1] != NUL)
	    {
		// Copy a special key as-is.
		यदि (copy)
		{
		    यदि (dst + 3 > dest + destlen)
		    {
			iemsg("vim_regsub_both(): not enough space");
			प्रत्यागम 0;
		    }
		    *dst++ = c;
		    *dst++ = *src++;
		    *dst++ = *src++;
		}
		अन्यथा
		{
		    dst += 3;
		    src += 2;
		}
		अनुवर्त;
	    }

	    यदि (c == '\\' && *src != NUL)
	    {
		// Check for abbreviations -- webb
		विकल्प (*src)
		{
		    पक्ष 'r':	c = CAR;	++src;	भङ्ग;
		    पक्ष 'n':	c = NL;		++src;	भङ्ग;
		    पक्ष 't':	c = TAB;	++src;	भङ्ग;
		 // Oh no!  \e already has meaning in subst pat :-(
		 // case 'e':   c = ESC;	++src;	break;
		    पक्ष 'b':	c = Ctrl_H;	++src;	भङ्ग;

		    // If "backslash" is TRUE the backslash will be removed
		    // later.  Used to insert a literal CR.
		    स्वभाव:	यदि (flags & REGSUB_BACKSLASH)
				{
				    यदि (copy)
				    {
					यदि (dst + 1 > dest + destlen)
					{
					    iemsg("vim_regsub_both(): not enough space");
					    प्रत्यागम 0;
					}
					*dst = '\\';
				    }
				    ++dst;
				}
				c = *src++;
		}
	    }
	    अन्यथा यदि (has_mbyte)
		c = mb_ptr2char(src - 1);

	    // Write to buffer, if copy is set.
	    यदि (func_one != (fptr_T)रिक्त)
	    {
		func_one(&cc, c);
		func_one = रिक्त;
	    }
	    अन्यथा यदि (func_all != (fptr_T)रिक्त)
		func_all(&cc, c);
	    अन्यथा // just copy
		cc = c;

	    यदि (has_mbyte)
	    {
		पूर्णाङ्क totlen = mb_ptr2len(src - 1);
		पूर्णाङ्क charlen = mb_char2len(cc);

		यदि (copy)
		{
		    यदि (dst + charlen > dest + destlen)
		    {
			iemsg("vim_regsub_both(): not enough space");
			प्रत्यागम 0;
		    }
		    mb_char2bytes(cc, dst);
		}
		dst += charlen - 1;
		यदि (enc_utf8)
		{
		    पूर्णाङ्क clen = utf_ptr2len(src - 1);

		    // If the character length is shorter than "totlen", there
		    // are composing characters; copy them as-is.
		    यदि (clen < totlen)
		    {
			यदि (copy)
			{
			    यदि (dst + totlen - clen > dest + destlen)
			    {
				iemsg("vim_regsub_both(): not enough space");
				प्रत्यागम 0;
			    }
			    mch_memmove(dst + 1, src - 1 + clen,
						     (size_t)(totlen - clen));
			}
			dst += totlen - clen;
		    }
		}
		src += totlen - 1;
	    }
	    अन्यथा यदि (copy)
	    {
		यदि (dst + 1 > dest + destlen)
		{
		    iemsg("vim_regsub_both(): not enough space");
		    प्रत्यागम 0;
		}
		*dst = cc;
	    }
	    dst++;
	}
	अन्यथा
	{
	    यदि (REG_MULTI)
	    {
		clnum = rex.reg_mmatch->startpos[no].lnum;
		यदि (clnum < 0 || rex.reg_mmatch->endpos[no].lnum < 0)
		    s = रिक्त;
		अन्यथा
		{
		    s = reg_getline(clnum) + rex.reg_mmatch->startpos[no].col;
		    यदि (rex.reg_mmatch->endpos[no].lnum == clnum)
			len = rex.reg_mmatch->endpos[no].col
					    - rex.reg_mmatch->startpos[no].col;
		    अन्यथा
			len = (पूर्णाङ्क)reg_getline_len(clnum) - rex.reg_mmatch->startpos[no].col;
		}
	    }
	    अन्यथा
	    {
		s = rex.reg_match->startp[no];
		यदि (rex.reg_match->endp[no] == रिक्त)
		    s = रिक्त;
		अन्यथा
		    len = (पूर्णाङ्क)(rex.reg_match->endp[no] - s);
	    }
	    यदि (s != रिक्त)
	    {
		प्रति (;;)
		{
		    यदि (len == 0)
		    {
			यदि (REG_MULTI)
			{
			    यदि (rex.reg_mmatch->endpos[no].lnum == clnum)
				भङ्ग;
			    यदि (copy)
			    {
				यदि (dst + 1 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    प्रत्यागम 0;
				}
				*dst = CAR;
			    }
			    ++dst;
			    s = reg_getline(++clnum);
			    यदि (rex.reg_mmatch->endpos[no].lnum == clnum)
				len = rex.reg_mmatch->endpos[no].col;
			    अन्यथा
				len = (पूर्णाङ्क)reg_getline_len(clnum);
			}
			अन्यथा
			    भङ्ग;
		    }
		    अन्यथा यदि (*s == NUL) // we hit NUL.
		    {
			यदि (copy)
			    iemsg(e_damaged_match_string);
			गच्छ exit;
		    }
		    अन्यथा
		    {
			यदि ((flags & REGSUB_BACKSLASH)
						  && (*s == CAR || *s == '\\'))
			{
			    /*
			     * Insert a backslash in front of a CR, otherwise
			     * it will be replaced by a line break.
			     * Number of backslashes will be halved later,
			     * double them here.
			     */
			    यदि (copy)
			    {
				यदि (dst + 2 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    प्रत्यागम 0;
				}
				dst[0] = '\\';
				dst[1] = *s;
			    }
			    dst += 2;
			}
			अन्यथा
			{
			    यदि (has_mbyte)
				c = mb_ptr2char(s);
			    अन्यथा
				c = *s;

			    यदि (func_one != (fptr_T)रिक्त)
			    {
				func_one(&cc, c);
				func_one = रिक्त;
			    }
			    अन्यथा यदि (func_all != (fptr_T)रिक्त)
				func_all(&cc, c);
			    अन्यथा // just copy
				cc = c;

			    यदि (has_mbyte)
			    {
				पूर्णाङ्क l;
				पूर्णाङ्क charlen;

				// Copy composing characters separately, one
				// at a time.
				यदि (enc_utf8)
				    l = utf_ptr2len(s) - 1;
				अन्यथा
				    l = mb_ptr2len(s) - 1;

				s += l;
				len -= l;
				charlen = mb_char2len(cc);
				यदि (copy)
				{
				    यदि (dst + charlen > dest + destlen)
				    {
					iemsg("vim_regsub_both(): not enough space");
					प्रत्यागम 0;
				    }
				    mb_char2bytes(cc, dst);
				}
				dst += charlen - 1;
			    }
			    अन्यथा यदि (copy)
			    {
				यदि (dst + 1 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    प्रत्यागम 0;
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
    यदि (copy)
	*dst = NUL;

exit:
    प्रत्यागम (पूर्णाङ्क)((dst - dest) + 1);
}

#ifdef FEAT_EVAL

    स्थिर char_u *
reg_getline_submatch(linenr_T lnum)
{
    char_u *पंक्ति;

    reg_getline_common(lnum, RGLF_पंक्ति | RGLF_SUBMATCH, &पंक्ति, रिक्त);

    प्रत्यागम पंक्ति;
}

    स्थिर colnr_T
reg_getline_submatch_len(linenr_T lnum)
{
    colnr_T length;

    reg_getline_common(lnum, RGLF_LENGTH | RGLF_SUBMATCH, रिक्त, &length);

    प्रत्यागम length;
}

/*
 * Used for the submatch() function: get the string from the n'th submatch in
 * allocated memory.
 * Returns NULL when not in a ":s" command and for a non-existing submatch.
 */
    char_u *
reg_submatch(पूर्णाङ्क no)
{
    char_u	*retval = रिक्त;
    char_u	*s;
    पूर्णाङ्क		len;
    पूर्णाङ्क		round;
    linenr_T	lnum;

    यदि (!can_f_submatch || no < 0)
	प्रत्यागम रिक्त;

    यदि (rsm.sm_match == रिक्त)
    {
	/*
	 * First round: compute the length and allocate memory.
	 * Second round: copy the text.
	 */
	प्रति (round = 1; round <= 2; ++round)
	{
	    lnum = rsm.sm_mmatch->startpos[no].lnum;
	    यदि (lnum < 0 || rsm.sm_mmatch->endpos[no].lnum < 0)
		प्रत्यागम रिक्त;

	    s = reg_getline_submatch(lnum);
	    यदि (s == रिक्त)  // anti-crash check, cannot happen?
		भङ्ग;
	    s += rsm.sm_mmatch->startpos[no].col;
	    यदि (rsm.sm_mmatch->endpos[no].lnum == lnum)
	    {
		// Within one line: take form start to end col.
		len = rsm.sm_mmatch->endpos[no].col
					  - rsm.sm_mmatch->startpos[no].col;
		यदि (round == 2)
		    vim_strncpy(retval, s, len);
		++len;
	    }
	    अन्यथा
	    {
		// Multiple lines: take start line from start col, middle
		// lines completely and end line up to end col.
		len = (पूर्णाङ्क)reg_getline_submatch_len(lnum) - rsm.sm_mmatch->startpos[no].col;
		यदि (round == 2)
		{
		    STRCPY(retval, s);
		    retval[len] = '\n';
		}
		++len;
		++lnum;
		यावत् (lnum < rsm.sm_mmatch->endpos[no].lnum)
		{
		    s = reg_getline_submatch(lnum);
		    यदि (round == 2)
			STRCPY(retval + len, s);
		    len += (पूर्णाङ्क)reg_getline_submatch_len(lnum);
		    यदि (round == 2)
			retval[len] = '\n';
		    ++len;
		    ++lnum;
		}
		यदि (round == 2)
		    STRNCPY(retval + len, reg_getline_submatch(lnum),
					     rsm.sm_mmatch->endpos[no].col);
		len += rsm.sm_mmatch->endpos[no].col;
		यदि (round == 2)
		    retval[len] = NUL;
		++len;
	    }

	    यदि (retval == रिक्त)
	    {
		retval = alloc(len);
		यदि (retval == रिक्त)
		    प्रत्यागम रिक्त;
	    }
	}
    }
    अन्यथा
    {
	s = rsm.sm_match->startp[no];
	यदि (s == रिक्त || rsm.sm_match->endp[no] == रिक्त)
	    retval = रिक्त;
	अन्यथा
	    retval = vim_strnsave(s, rsm.sm_match->endp[no] - s);
    }

    प्रत्यागम retval;
}

/*
 * Used for the submatch() function with the optional non-zero argument: get
 * the list of strings from the n'th submatch in allocated memory with NULs
 * represented in NLs.
 * Returns a list of allocated strings.  Returns NULL when not in a ":s"
 * command, for a non-existing submatch and for any error.
 */
    list_T *
reg_submatch_list(पूर्णाङ्क no)
{
    char_u	*s;
    linenr_T	slnum;
    linenr_T	elnum;
    colnr_T	scol;
    colnr_T	ecol;
    पूर्णाङ्क		i;
    list_T	*list;
    पूर्णाङ्क		error = FALSE;

    यदि (!can_f_submatch || no < 0)
	प्रत्यागम रिक्त;

    यदि (rsm.sm_match == रिक्त)
    {
	slnum = rsm.sm_mmatch->startpos[no].lnum;
	elnum = rsm.sm_mmatch->endpos[no].lnum;
	यदि (slnum < 0 || elnum < 0)
	    प्रत्यागम रिक्त;

	scol = rsm.sm_mmatch->startpos[no].col;
	ecol = rsm.sm_mmatch->endpos[no].col;

	list = list_alloc();
	यदि (list == रिक्त)
	    प्रत्यागम रिक्त;

	s = reg_getline_submatch(slnum) + scol;
	यदि (slnum == elnum)
	{
	    यदि (list_append_string(list, s, ecol - scol) == FAIL)
		error = TRUE;
	}
	अन्यथा
	{
	    पूर्णाङ्क max_lnum = elnum - slnum;

	    यदि (list_append_string(list, s, -1) == FAIL)
		error = TRUE;
	    प्रति (i = 1; i < max_lnum; i++)
	    {
		s = reg_getline_submatch(slnum + i);
		यदि (list_append_string(list, s, -1) == FAIL)
		    error = TRUE;
	    }
	    s = reg_getline_submatch(elnum);
	    यदि (list_append_string(list, s, ecol) == FAIL)
		error = TRUE;
	}
    }
    अन्यथा
    {
	s = rsm.sm_match->startp[no];
	यदि (s == रिक्त || rsm.sm_match->endp[no] == रिक्त)
	    प्रत्यागम रिक्त;
	list = list_alloc();
	यदि (list == रिक्त)
	    प्रत्यागम रिक्त;
	यदि (list_append_string(list, s,
				 (पूर्णाङ्क)(rsm.sm_match->endp[no] - s)) == FAIL)
	    error = TRUE;
    }

    यदि (error)
    {
	list_free(list);
	प्रत्यागम रिक्त;
    }
    ++list->lv_refcount;
    प्रत्यागम list;
}
#endif

/*
 * Initialize the values used for matching against multiple lines
 */
    स्थिर शून्य
init_regexec_multi(
	regmmatch_T	*rmp,
	win_T		*win,	// window in which to search or NULL
	buf_T		*buf,	// buffer in which to search
	linenr_T	lnum)	// nr of line to start looking for match
{
    rex.reg_match = रिक्त;
    rex.reg_mmatch = rmp;
    rex.reg_buf = buf;
    rex.reg_win = win;
    rex.reg_firstlnum = lnum;
    rex.reg_maxline = rex.reg_buf->b_ml.ml_पंक्ति_गिनती - lnum;
    rex.reg_पंक्ति_lbr = FALSE;
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
    स्थिर पूर्णाङ्क
get_char_class_bits(char_u *start, char_u *end, पूर्णाङ्क *newl)
{
    char_u	*p = start;
    पूर्णाङ्क		config = 0;

    *newl = FALSE;
    यदि (*end != ']')
	प्रत्यागम -1;
    यदि (*p == '^')
    {
	config |= CLASS_not;
	p++;
    }

    यावत् (p < end)
    {
	यदि (p + 2 < end && *(p + 1) == '-')
	{
	    विकल्प (*p)
	    {
		पक्ष '0':
		    यदि (*(p + 2) == '9')
		    {
			config |= CLASS_o9;
			भङ्ग;
		    }
		    यदि (*(p + 2) == '7')
		    {
			config |= CLASS_o7;
			भङ्ग;
		    }
		    प्रत्यागम -1;

		पक्ष 'a':
		    यदि (*(p + 2) == 'z')
		    {
			config |= CLASS_az;
			भङ्ग;
		    }
		    यदि (*(p + 2) == 'f')
		    {
			config |= CLASS_af;
			भङ्ग;
		    }
		    प्रत्यागम -1;

		पक्ष 'A':
		    यदि (*(p + 2) == 'Z')
		    {
			config |= CLASS_AZ;
			भङ्ग;
		    }
		    यदि (*(p + 2) == 'F')
		    {
			config |= CLASS_AF;
			भङ्ग;
		    }
		    प्रत्यागम -1;

		स्वभाव:
		    प्रत्यागम -1;
	    }
	    p += 3;
	}
	अन्यथा यदि (p + 1 < end && *p == '\\' && *(p + 1) == 'n')
	{
	    *newl = TRUE;
	    p += 2;
	}
	अन्यथा यदि (*p == '_')
	{
	    config |= CLASS_underscore;
	    p++;
	}
	अन्यथा यदि (*p == '\n')
	{
	    *newl = TRUE;
	    p++;
	}
	अन्यथा
	    प्रत्यागम -1;
    } // while (p < end)

    यदि (p != end)
	प्रत्यागम -1;

    // U+017F and U+212A fold to ASCII 's' and 'k', missed by the class opcodes
    यदि (enc_utf8 && (config & (CLASS_az | CLASS_AZ)))
	प्रत्यागम -1;

    प्रत्यागम config;
}

#अन्तर्भाव "regexp_bt.c"

स्थिर regengine_T bt_regengine =
{
    bt_regcomp,
    bt_regfree,
    bt_regexec_nl,
    bt_regexec_multi
#ifdef DEBUG
    ,(char_u *)""
#endif
};

#अन्तर्भाव "regexp_nfa.c"

स्थिर regengine_T nfa_regengine =
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
स्थिर पूर्णाङ्क regexp_engine = 0;

#ifdef DEBUG
स्थिर ध्रुव char_u regname[][30] = {
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
vim_regcomp(char_u *व्यंजक_arg, पूर्णाङ्क re_flags)
{
    regprog_T   *prog = रिक्त;
    char_u	*व्यंजक = व्यंजक_arg;
    पूर्णाङ्क		called_emsg_before;

    regexp_engine = p_re;

    // Check for prefix "\%#=", that sets the regexp engine
    यदि (STRNCMP(व्यंजक, "\\%#=", 4) == 0)
    {
	पूर्णाङ्क newengine = व्यंजक[4] - '0';

	यदि (newengine == AUTOMATIC_ENGINE
	    || newengine == BACKTRACKING_ENGINE
	    || newengine == NFA_ENGINE)
	{
	    regexp_engine = व्यंजक[4] - '0';
	    व्यंजक += 5;
#ifdef DEBUG
	    smsg("New regexp mode selected (%d): %s",
					   regexp_engine, regname[newengine]);
#endif
	}
	अन्यथा
	{
	    emsg(_(e_percent_hash_can_only_be_followed_by_zero_one_two_automatic_engine_will_be_used));
	    regexp_engine = AUTOMATIC_ENGINE;
	}
    }
#ifdef DEBUG
    bt_regengine.व्यंजक = व्यंजक;
    nfa_regengine.व्यंजक = व्यंजक;
#endif
    // reg_iswordc() uses rex.reg_buf
    rex.reg_buf = curbuf;

    /*
     * First try the NFA engine, unless backtracking was requested.
     */
    called_emsg_before = called_emsg;
    यदि (regexp_engine != BACKTRACKING_ENGINE)
	prog = nfa_regengine.regcomp(व्यंजक,
		re_flags + (regexp_engine == AUTOMATIC_ENGINE ? RE_AUTO : 0));
    अन्यथा
	prog = bt_regengine.regcomp(व्यंजक, re_flags);

    // Check for error compiling regexp with initial engine.
    यदि (prog == रिक्त)
    {
#ifdef BT_REGEXP_DEBUG_LOG
	यदि (regexp_engine == BACKTRACKING_ENGINE)   // debugging log for BT engine
	{
	    FILE *f;
	    f = fopen(BT_REGEXP_DEBUG_LOG_NAME, "a");
	    यदि (f)
	    {
		fprintf(f, "Syntax error in \"%s\"\n", व्यंजक);
		fclose(f);
	    }
	    अन्यथा
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
	यदि (regexp_engine == AUTOMATIC_ENGINE
					  && called_emsg == called_emsg_before)
	{
	    regexp_engine = BACKTRACKING_ENGINE;
#ifdef FEAT_EVAL
	    report_re_switch(व्यंजक);
#endif
	    prog = bt_regengine.regcomp(व्यंजक, re_flags);
	}
    }

    यदि (prog != रिक्त)
    {
	// Store the info needed to call regcomp() again when the engine turns
	// out to be very slow when executing it.
	prog->re_engine = regexp_engine;
	prog->re_flags  = re_flags;
    }

    प्रत्यागम prog;
}

/*
 * Free a compiled regexp program, returned by vim_regcomp().
 */
    शून्य
vim_regfree(regprog_T *prog)
{
    यदि (prog != रिक्त)
	prog->engine->regfree(prog);
}

#यदि defined(EXITFREE)
    शून्य
free_regexp_stuff(शून्य)
{
    ga_clear(&regstack);
    ga_clear(&backpos);
    vim_free(reg_tofree);
    vim_free(reg_prev_sub);
    vim_free(post_start);   // NFA postfix buffer, reused across compilations
    post_start = रिक्त;
    post_start_len = 0;
    vim_free(nfa_ढेर);    // NFA fragment stack, reused across compilations
    nfa_ढेर = रिक्त;
    nfa_ढेर_len = 0;
}
#endif

#ifdef FEAT_EVAL
    स्थिर शून्य
report_re_switch(char_u *pat)
{
    यदि (p_verbose > 0)
    {
	verbose_enter();
	संदेश_puts(_("Switching to backtracking RE engine for pattern: "));
	संदेश_puts((वर्ण *)pat);
	verbose_leave();
    }
}
#endif

#यदि defined(FEAT_X11)
/*
 * Return whether "prog" is currently being executed.
 */
    पूर्णाङ्क
regprog_in_use(regprog_T *prog)
{
    प्रत्यागम prog->re_in_use;
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
    स्थिर पूर्णाङ्क
vim_regexec_string(
    regmatch_T	*rmp,
    char_u	*पंक्ति,  // string to match against
    colnr_T	col,    // column to start looking for match
    पूर्णाङ्क		nl)
{
    पूर्णाङ्क		result;
    regexec_T	rex_save;
    पूर्णाङ्क		rex_in_use_save = rex_in_use;

    // Cannot use the same prog recursively, it contains state.
    यदि (rmp->regprog->re_in_use)
    {
	emsg(_(e_cannot_use_pattern_recursively));
	प्रत्यागम FALSE;
    }
    rmp->regprog->re_in_use = TRUE;

    यदि (rex_in_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_in_use = TRUE;

    rex.reg_startp = रिक्त;
    rex.reg_endp = रिक्त;
    rex.reg_startpos = रिक्त;
    rex.reg_endpos = रिक्त;

    result = rmp->regprog->engine->regexec_nl(rmp, पंक्ति, col, nl);
    rmp->regprog->re_in_use = FALSE;

    // NFA engine aborted because it's very slow.
    यदि (rmp->regprog->re_engine == AUTOMATIC_ENGINE
					       && result == NFA_TOO_EXPENSIVE)
    {
	पूर्णाङ्क    save_p_re = p_re;
	पूर्णाङ्क    re_flags = rmp->regprog->re_flags;
	char_u *pat = vim_strsave(((nfa_regprog_T *)rmp->regprog)->pattern);

	p_re = BACKTRACKING_ENGINE;
	यदि (pat != रिक्त)
	{
	    regprog_T *prev_prog = rmp->regprog;

#ifdef FEAT_EVAL
	    report_re_switch(pat);
#endif
	    rmp->regprog = vim_regcomp(pat, re_flags);
	    यदि (rmp->regprog == रिक्त)
	    {
		// Somehow compiling the pattern failed now, put back the
		// previous one to avoid "regprog" becoming NULL.
		rmp->regprog = prev_prog;
	    }
	    अन्यथा
	    {
		vim_regfree(prev_prog);
		rmp->regprog->re_in_use = TRUE;
		result = rmp->regprog->engine->regexec_nl(rmp, पंक्ति, col, nl);
		rmp->regprog->re_in_use = FALSE;
	    }
	    vim_free(pat);
	}

	p_re = save_p_re;
    }

    rex_in_use = rex_in_use_save;
    यदि (rex_in_use)
	rex = rex_save;

    प्रत्यागम result > 0;
}

#यदि defined(FEAT_SPELL) || defined(FEAT_EVAL) || defined(FEAT_X11)
/*
 * Note: "*prog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    पूर्णाङ्क
vim_regexec_prog(
    regprog_T	**prog,
    पूर्णाङ्क		ignore_case,
    char_u	*पंक्ति,
    colnr_T	col)
{
    पूर्णाङ्क		r;
    regmatch_T	regmatch;

    regmatch.regprog = *prog;
    regmatch.rm_ic = ignore_case;
    r = vim_regexec_string(&regmatch, पंक्ति, col, FALSE);
    *prog = regmatch.regprog;
    प्रत्यागम r;
}
#endif

/*
 * Note: "rmp->regprog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    पूर्णाङ्क
vim_regexec(regmatch_T *rmp, char_u *पंक्ति, colnr_T col)
{
    प्रत्यागम vim_regexec_string(rmp, पंक्ति, col, FALSE);
}

/*
 * Like vim_regexec(), but consider a "\n" in "line" to be a line break.
 * Note: "rmp->regprog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    पूर्णाङ्क
vim_regexec_nl(regmatch_T *rmp, char_u *पंक्ति, colnr_T col)
{
    प्रत्यागम vim_regexec_string(rmp, पंक्ति, col, TRUE);
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
    पूर्णाङ्क		*timed_out)	// flag is set when timeout limit reached
{
    पूर्णाङ्क		result;
    regexec_T	rex_save;
    पूर्णाङ्क		rex_in_use_save = rex_in_use;

    // Cannot use the same prog recursively, it contains state.
    यदि (rmp->regprog->re_in_use)
    {
	emsg(_(e_cannot_use_pattern_recursively));
	प्रत्यागम FALSE;
    }
    rmp->regprog->re_in_use = TRUE;

    यदि (rex_in_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_in_use = TRUE;

    result = rmp->regprog->engine->regexec_multi(
				      rmp, win, buf, lnum, col, timed_out);
    rmp->regprog->re_in_use = FALSE;

    // NFA engine aborted because it's very slow.
    यदि (rmp->regprog->re_engine == AUTOMATIC_ENGINE
					       && result == NFA_TOO_EXPENSIVE)
    {
	पूर्णाङ्क    save_p_re = p_re;
	पूर्णाङ्क    re_flags = rmp->regprog->re_flags;
	char_u *pat = vim_strsave(((nfa_regprog_T *)rmp->regprog)->pattern);

	p_re = BACKTRACKING_ENGINE;
	यदि (pat != रिक्त)
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
	    यदि (rmp->regprog == रिक्त)
	    {
		// Somehow compiling the pattern failed now, put back the
		// previous one to avoid "regprog" becoming NULL.
		rmp->regprog = prev_prog;
	    }
	    अन्यथा
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
    यदि (rex_in_use)
	rex = rex_save;

    प्रत्यागम result <= 0 ? 0 : result;
}
