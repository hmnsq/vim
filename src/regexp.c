/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * Handling of regular expressions: vim_regcomp(), vim_regexec(), vim_regsub()
 */

// By default: do not create debugging logs or files related to regular
// expressions, even when compiling with -DDEBUG.
// Uncomment the second line to get the regexp debugging.
#నిర్వచనం_రద్దు DEBUG
// #define DEBUG

#అన్తర్భావ "vim.h"

#నిర్వచితమైతే DEBUG
// show/save debugging data when BT engine is used
# define BT_REGEXP_DUMP
// save the debugging data to a file instead of displaying it
# define BT_REGEXP_LOG
# define BT_REGEXP_DEBUG_LOG
# define BT_REGEXP_DEBUG_LOG_NAME	"bt_regexp_debug.log"
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_RELTIME
స్థిర sig_atomic_t dummy_timeout_flag = 0;
స్థిర అస్థిర sig_atomic_t *timeout_flag = &dummy_timeout_flag;
#అయితే_ముగింపు

/*
 * Magic characters have a special meaning, they don't match literally.
 * Magic characters are negative.  This separates them from literal characters
 * (possibly multi-byte).  Only ASCII characters can be Magic.
 */
#నిర్వచించు Magic(x)	((పూర్ణాంకం)(x) - 256)
#నిర్వచించు un_Magic(x)	((x) + 256)
#నిర్వచించు is_Magic(x)	((x) < 0)

    స్థిర పూర్ణాంకం
no_Magic(పూర్ణాంకం x)
{
    ఒకవేళ (is_Magic(x))
	తిరిగి un_Magic(x);
    తిరిగి x;
}

    స్థిర పూర్ణాంకం
toggle_Magic(పూర్ణాంకం x)
{
    ఒకవేళ (is_Magic(x))
	తిరిగి un_Magic(x);
    తిరిగి Magic(x);
}

#ఒకవేళ defined(FEAT_RELTIME)
స్థిర పూర్ణాంకం timeout_nesting = 0;

/*
 * Start a timer that will cause the regexp to abort after "msec".
 * This doesn't work well recursively.  In case it happens anyway, the first
 * set timeout will prevail, nested ones are ignored.
 * The caller must make sure there is a matching disable_regexp_timeout() call!
 */
    శూన్య
init_regexp_timeout(దీర్ఘ msec)
{
    ఒకవేళ (timeout_nesting == 0)
	timeout_flag = ప్రారంభం_timeout(msec);
    ++timeout_nesting;
}

    శూన్య
disable_regexp_timeout(శూన్య)
{
    ఒకవేళ (timeout_nesting == 0)
	iemsg("disable_regexp_timeout() called without active timer");
    లేకపోతే ఒకవేళ (--timeout_nesting == 0)
    {
	stop_timeout();
	timeout_flag = &dummy_timeout_flag;
    }
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL)
# ifdef FEAT_RELTIME
స్థిర sig_atomic_t *saved_timeout_flag;
# endif

/*
 * Used at the debug prompt: disable the timeout so that expression evaluation
 * can used patterns.
 * Must be followed by calling restore_timeout_for_debugging().
 */
    శూన్య
save_timeout_for_debugging(శూన్య)
{
# ifdef FEAT_RELTIME
    saved_timeout_flag = (sig_atomic_t *)timeout_flag;
    timeout_flag = &dummy_timeout_flag;
# endif
}

    శూన్య
restore_timeout_for_debugging(శూన్య)
{
# ifdef FEAT_RELTIME
    timeout_flag = saved_timeout_flag;
# endif
}
#అయితే_ముగింపు

/*
 * The first byte of the BT regexp internal "program" is actually this magic
 * number; the start node begins in the second byte.  It's used to catch the
 * most severe mutilation of the program by the caller.
 */

#నిర్వచించు REGMAGIC	0234

/*
 * Utility definitions.
 */
#నిర్వచించు UCHARAT(p)	((పూర్ణాంకం)*(char_u *)(p))

// Used for an error (down from) vim_regcomp(): give the error message, set
// rc_did_emsg and return NULL
#నిర్వచించు EMSG_RET_NULL(m) తిరిగి (emsg((m)), rc_did_emsg = TRUE, (శూన్య *)ఖాళీ)
#నిర్వచించు IEMSG_RET_NULL(m) తిరిగి (iemsg((m)), rc_did_emsg = TRUE, (శూన్య *)ఖాళీ)
#నిర్వచించు EMSG_RET_FAIL(m) తిరిగి (emsg((m)), rc_did_emsg = TRUE, FAIL)
#నిర్వచించు EMSG2_RET_NULL(m, c) తిరిగి (semsg((స్థిరరాశి అక్షరం *)(m), (c) ? "" : "\\"), rc_did_emsg = TRUE, (శూన్య *)ఖాళీ)
#నిర్వచించు EMSG3_RET_NULL(m, c, a) తిరిగి (semsg((స్థిరరాశి అక్షరం *)(m), (c) ? "" : "\\", (a)), rc_did_emsg = TRUE, (శూన్య *)ఖాళీ)
#నిర్వచించు EMSG2_RET_FAIL(m, c) తిరిగి (semsg((స్థిరరాశి అక్షరం *)(m), (c) ? "" : "\\"), rc_did_emsg = TRUE, FAIL)
#నిర్వచించు EMSG_ONE_RET_NULL EMSG2_RET_NULL(_(e_invalid_అంశం_లోపల_str_brackets), reg_magic == MAGIC_ALL)


#నిర్వచించు MAX_హద్దు	(32767L << 16L)

#నిర్వచించు NOT_MULTI	0
#నిర్వచించు MULTI_ONE	1
#నిర్వచించు MULTI_MULT	2

// return values for regmatch()
#నిర్వచించు RA_FAIL		1	// something failed, abort
#నిర్వచించు RA_CONT		2	// continue in inner loop
#నిర్వచించు RA_BREAK	3	// break inner loop
#నిర్వచించు RA_సరిపోలిక	4	// successful match
#నిర్వచించు RA_NOMATCH	5	// didn't match

/*
 * Return NOT_MULTI if c is not a "multi" operator.
 * Return MULTI_ONE if c is a single "multi" operator.
 * Return MULTI_MULT if c is a multi "multi" operator.
 */
    స్థిర పూర్ణాంకం
re_multi_type(పూర్ణాంకం c)
{
    ఒకవేళ (c == Magic('@') || c == Magic('=') || c == Magic('?'))
	తిరిగి MULTI_ONE;
    ఒకవేళ (c == Magic('*') || c == Magic('+') || c == Magic('{'))
	తిరిగి MULTI_MULT;
    తిరిగి NOT_MULTI;
}

స్థిర char_u		*reg_prev_sub = ఖాళీ;
స్థిర size_t		reg_prev_sublen = 0;

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
స్థిర char_u REGEXP_INRANGE[] = "]^-n\\";
స్థిర char_u REGEXP_ABBR[] = "nrtebdoxuU";

/*
 * Translate '\x' to its control character, except "\n", which is Magic.
 */
    స్థిర పూర్ణాంకం
backslash_trans(పూర్ణాంకం c)
{
    ఎంపిక (c)
    {
	స్థితి 'r':   తిరిగి CAR;
	స్థితి 't':   తిరిగి TAB;
	స్థితి 'e':   తిరిగి ESC;
	స్థితి 'b':   తిరిగి BS;
    }
    తిరిగి c;
}

గణన
{
    CLASS_ALNUM = 0,
    CLASS_ALPHA,
    CLASS_BLANK,
    CLASS_CNTRL,
    CLASS_DIGIT,
    CLASS_లేఖాచిత్రం,
    CLASS_క్రింది,
    CLASS_PRINT,
    CLASS_PUNCT,
    CLASS_SPACE,
    CLASS_పై,
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
    స్థిర పూర్ణాంకం
get_char_class(char_u **pp)
{
    // must be sorted by the 'value' field because it is used by bsearch()!
    స్థిర keyvalue_T char_class_tab[] =
    {
	KEYVALUE_నమోదు(CLASS_ALNUM, "alnum:]"),
	KEYVALUE_నమోదు(CLASS_ALPHA, "alpha:]"),
	KEYVALUE_నమోదు(CLASS_BACKSPACE, "backspace:]"),
	KEYVALUE_నమోదు(CLASS_BLANK, "blank:]"),
	KEYVALUE_నమోదు(CLASS_CNTRL, "cntrl:]"),
	KEYVALUE_నమోదు(CLASS_DIGIT, "digit:]"),
	KEYVALUE_నమోదు(CLASS_ESCAPE, "escape:]"),
	KEYVALUE_నమోదు(CLASS_FNAME, "fname:]"),
	KEYVALUE_నమోదు(CLASS_లేఖాచిత్రం, "graph:]"),
	KEYVALUE_నమోదు(CLASS_IDENT, "ident:]"),
	KEYVALUE_నమోదు(CLASS_KEYWORD, "keyword:]"),
	KEYVALUE_నమోదు(CLASS_క్రింది, "lower:]"),
	KEYVALUE_నమోదు(CLASS_PRINT, "print:]"),
	KEYVALUE_నమోదు(CLASS_PUNCT, "punct:]"),
	KEYVALUE_నమోదు(CLASS_RETURN, "return:]"),
	KEYVALUE_నమోదు(CLASS_SPACE, "space:]"),
	KEYVALUE_నమోదు(CLASS_TAB, "tab:]"),
	KEYVALUE_నమోదు(CLASS_పై, "upper:]"),
	KEYVALUE_నమోదు(CLASS_XDIGIT, "xdigit:]")
    };

    // check that the value of "pp" has a chance of matching
    ఒకవేళ ((*pp)[1] == ':' && ASCII_ISLOWER((*pp)[2])
			&& ASCII_ISLOWER((*pp)[3]) && ASCII_ISLOWER((*pp)[4]))
    {
	keyvalue_T లక్ష్యం;
	keyvalue_T *నమోదు_2;
	// this function can be called repeatedly with the same value for "pp"
	// so we cache the last found entry.
	స్థిర keyvalue_T *చివరి_నమోదు = ఖాళీ;

	లక్ష్యం.కీలకం = 0;
	లక్ష్యం.విలువ.string = *pp + 2;
	లక్ష్యం.విలువ.నిడివి = 0;	// not used, see cmp_keyvalue_value_n()

	ఒకవేళ (చివరి_నమోదు != ఖాళీ && cmp_keyvalue_విలువ_n(&లక్ష్యం, చివరి_నమోదు) == 0)
	    నమోదు_2 = చివరి_నమోదు;
	లేకపోతే
	    నమోదు_2 = (keyvalue_T *)bsearch(&లక్ష్యం, &char_class_tab,
					ARRAY_నిడివి(char_class_tab),
					పరిమాణం(char_class_tab[0]), cmp_keyvalue_విలువ_n);
	ఒకవేళ (నమోదు_2 != ఖాళీ)
	{
	    చివరి_నమోదు = నమోదు_2;
	    *pp += నమోదు_2->విలువ.నిడివి + 2;
	    తిరిగి నమోదు_2->కీలకం;
	}
    }
    తిరిగి CLASS_NONE;
}

/*
 * Specific version of character class functions.
 * Using a table to keep this fast.
 */
స్థిర పొట్టి	class_tab[256];

#నిర్వచించు	    RI_DIGIT	0x01
#నిర్వచించు	    RI_HEX	0x02
#నిర్వచించు	    RI_OCTAL	0x04
#నిర్వచించు	    RI_మాట	0x08
#నిర్వచించు	    RI_తల	0x10
#నిర్వచించు	    RI_ALPHA	0x20
#నిర్వచించు	    RI_క్రింది	0x40
#నిర్వచించు	    RI_పై	0x80
#నిర్వచించు	    RI_WHITE	0x100

    స్థిర శూన్య
init_class_tab(శూన్య)
{
    పూర్ణాంకం		i;
    స్థిర పూర్ణాంకం	పూర్తి = FALSE;

    ఒకవేళ (పూర్తి)
	తిరిగి;

    కొరకు (i = 0; i < 256; ++i)
    {
	ఒకవేళ (i >= '0' && i <= '7')
	    class_tab[i] = RI_DIGIT + RI_HEX + RI_OCTAL + RI_మాట;
	లేకపోతే ఒకవేళ (i >= '8' && i <= '9')
	    class_tab[i] = RI_DIGIT + RI_HEX + RI_మాట;
	లేకపోతే ఒకవేళ (i >= 'a' && i <= 'f')
	    class_tab[i] = RI_HEX + RI_మాట + RI_తల + RI_ALPHA + RI_క్రింది;
	లేకపోతే ఒకవేళ (i >= 'g' && i <= 'z')
	    class_tab[i] = RI_మాట + RI_తల + RI_ALPHA + RI_క్రింది;
	లేకపోతే ఒకవేళ (i >= 'A' && i <= 'F')
	    class_tab[i] = RI_HEX + RI_మాట + RI_తల + RI_ALPHA + RI_పై;
	లేకపోతే ఒకవేళ (i >= 'G' && i <= 'Z')
	    class_tab[i] = RI_మాట + RI_తల + RI_ALPHA + RI_పై;
	లేకపోతే ఒకవేళ (i == '_')
	    class_tab[i] = RI_మాట + RI_తల;
	లేకపోతే
	    class_tab[i] = 0;
    }
    class_tab[' '] |= RI_WHITE;
    class_tab['\t'] |= RI_WHITE;
    పూర్తి = TRUE;
}

#నిర్వచించు ri_digit(c)	((c) < 0x100 && (class_tab[c] & RI_DIGIT))
#నిర్వచించు ri_hex(c)	((c) < 0x100 && (class_tab[c] & RI_HEX))
#నిర్వచించు ri_octal(c)	((c) < 0x100 && (class_tab[c] & RI_OCTAL))
#నిర్వచించు ri_మాట(c)	((c) < 0x100 && (class_tab[c] & RI_మాట))
#నిర్వచించు ri_తల(c)	((c) < 0x100 && (class_tab[c] & RI_తల))
#నిర్వచించు ri_alpha(c)	((c) < 0x100 && (class_tab[c] & RI_ALPHA))
#నిర్వచించు ri_క్రింది(c)	((c) < 0x100 && (class_tab[c] & RI_క్రింది))
#నిర్వచించు ri_పై(c)	((c) < 0x100 && (class_tab[c] & RI_పై))
#నిర్వచించు ri_white(c)	((c) < 0x100 && (class_tab[c] & RI_WHITE))

// flags for regflags
#నిర్వచించు RF_ICASE    1	// ignore case
#నిర్వచించు RF_NOICASE  2	// don't ignore case
#నిర్వచించు RF_HASNL    4	// can match a NL
#నిర్వచించు RF_ICOMBINE 8	// ignore combining characters
#నిర్వచించు RF_LOOKBH   16	// uses "\@<=" or "\@<!"

/*
 * Global work variables for vim_regcomp().
 */

స్థిర char_u	*regparse;	// Input-scan pointer.
స్థిర పూర్ణాంకం	regnpar;	// () count.
స్థిర పూర్ణాంకం	కోరుతుంది_nfa;	// regex should use NFA engine
#నిర్వచితమైతే FEAT_SYN_HL
స్థిర పూర్ణాంకం	regnzpar;	// \z() count.
స్థిర పూర్ణాంకం	re_has_z;	// \z item detected
#అయితే_ముగింపు
స్థిర గుర్తులేని	regflags;	// RF_ flags for prog
#ఒకవేళ defined(FEAT_SYN_HL)
స్థిర పూర్ణాంకం	had_eol;	// TRUE when EOL found by vim_regcomp()
#అయితే_ముగింపు

స్థిర magic_T	reg_magic;	// magicness of the pattern

స్థిర పూర్ణాంకం	reg_string;	// matching with a string instead of a buffer
				//పంక్తి
స్థిర పూర్ణాంకం	reg_strict;	// "[abc" is illegal

/*
 * META contains all characters that may be magic, except '^' and '$'.
 */

// META[] is used often enough to justify turning it into a table.
స్థిర స్థిరరాశి char_u META_flags[] = {
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

స్థిర పూర్ణాంకం	curchr;		// currently parsed character
// Previous character.  Note: prevchr is sometimes -1 when we are not at the
// start, eg in /[ ^I]^ the pattern was never found even if it existed,
// because ^ was taken to be magic -- webb
స్థిర పూర్ణాంకం	prevchr;
స్థిర పూర్ణాంకం	prevprevchr;	// previous-previous character
స్థిర పూర్ణాంకం	nextchr;	// used for ungetchr()

// arguments for reg()
#నిర్వచించు REG_NOPAREN	0	// toplevel reg()
#నిర్వచించు REG_PAREN	1	// \(\)
#నిర్వచించు REG_ZPAREN	2	// \z(\)
#నిర్వచించు REG_NPAREN	3	// \%(\)

// Limit recursive parsing of nested regexp atoms to avoid using up the C stack.
#నిర్వచించు REG_MAX_PAREN_లోతు	1000

రకనిర్వచనం నిర్మాణం
{
    char_u	*regparse;
    పూర్ణాంకం		prevchr_len;
    పూర్ణాంకం		curchr;
    పూర్ణాంకం		prevchr;
    పూర్ణాంకం		prevprevchr;
    పూర్ణాంకం		nextchr;
    పూర్ణాంకం		at_ప్రారంభం;
    పూర్ణాంకం		prev_at_ప్రారంభం;
    పూర్ణాంకం		regnpar;
} parse_state_T;

స్థిర శూన్య	initchr(char_u *);
స్థిర పూర్ణాంకం	getchr(శూన్య);
స్థిర శూన్య	skipchr_keepstart(శూన్య);
స్థిర పూర్ణాంకం	peekchr(శూన్య);
స్థిర శూన్య	skipchr(శూన్య);
స్థిర శూన్య	ungetchr(శూన్య);
స్థిర vimlong_T	gethexchrs(పూర్ణాంకం maxinputlen);
స్థిర దీర్ఘ	getoctchrs(శూన్య);
స్థిర vimlong_T	getdecchrs(శూన్య);
స్థిర పూర్ణాంకం	coll_get_char(శూన్య);
స్థిర పూర్ణాంకం	prog_magic_wrong(శూన్య);
స్థిర పూర్ణాంకం	cstrncmp(char_u *s1, char_u *s2, పూర్ణాంకం *n);
స్థిర char_u	*cstrchr(char_u *, పూర్ణాంకం);
స్థిర పూర్ణాంకం	re_mult_తదుపరి(అక్షరం *what);
స్థిర పూర్ణాంకం	reg_iswordc(పూర్ణాంకం);
#నిర్వచితమైతే FEAT_EVAL
స్థిర శూన్య report_re_switch(char_u *pat);
#అయితే_ముగింపు

స్థిర regengine_T bt_regengine;
స్థిర regengine_T nfa_regengine;

/*
 * Return TRUE if compiled regular expression "prog" can match a line break.
 */
    పూర్ణాంకం
re_multiline(regprog_T *prog)
{
    తిరిగి (prog->regflags & RF_HASNL);
}

/*
 * Check for an equivalence class name "[=a=]".  "pp" points to the '['.
 * Returns a character representing the class. Zero means that no item was
 * recognized.  Otherwise "pp" is advanced to after the item.
 */
    స్థిర పూర్ణాంకం
get_equi_class(char_u **pp)
{
    పూర్ణాంకం		c;
    పూర్ణాంకం		l = 1;
    char_u	*p = *pp;

    ఒకవేళ (p[1] == '=' && p[2] != NUL)
    {
	ఒకవేళ (has_mbyte)
	    l = (*mb_ptr2len)(p + 2);
	ఒకవేళ (p[l + 2] == '=' && p[l + 3] == ']')
	{
	    ఒకవేళ (has_mbyte)
		c = mb_ptr2char(p + 2);
	    లేకపోతే
		c = p[2];
	    *pp += l + 4;
	    తిరిగి c;
	}
    }
    తిరిగి 0;
}

/*
 * Check for a collating element "[.a.]".  "pp" points to the '['.
 * Returns a character. Zero means that no item was recognized.  Otherwise
 * "pp" is advanced to after the item.
 * Currently only single characters are recognized!
 */
    స్థిర పూర్ణాంకం
get_coll_element(char_u **pp)
{
    పూర్ణాంకం		c;
    పూర్ణాంకం		l = 1;
    char_u	*p = *pp;

    ఒకవేళ (p[0] != NUL && p[1] == '.' && p[2] != NUL)
    {
	ఒకవేళ (has_mbyte)
	    l = (*mb_ptr2len)(p + 2);
	ఒకవేళ (p[l + 2] == '.' && p[l + 3] == ']')
	{
	    ఒకవేళ (has_mbyte)
		c = mb_ptr2char(p + 2);
	    లేకపోతే
		c = p[2];
	    *pp += l + 4;
	    తిరిగి c;
	}
    }
    తిరిగి 0;
}

స్థిర పూర్ణాంకం reg_cpo_lit; // 'cpoptions' contains 'l' flag
స్థిర పూర్ణాంకం reg_cpo_bsl; // 'cpoptions' contains '\' flag

    స్థిర శూన్య
get_cpo_flags(శూన్య)
{
    reg_cpo_lit = vim_strchr(p_cpo, CPO_LITERAL) != ఖాళీ;
    reg_cpo_bsl = vim_strchr(p_cpo, CPO_BACKSL) != ఖాళీ;
}

/*
 * Skip over a "[]" range.
 * "p" must point to the character after the '['.
 * The returned pointer is on the matching ']', or the terminating NUL.
 */
    స్థిర char_u *
skip_anyof(char_u *p)
{
    పూర్ణాంకం		l;

    ఒకవేళ (*p == '^')	// Complement of range.
	++p;
    ఒకవేళ (*p == ']' || *p == '-')
	++p;
    అయితే (*p != NUL && *p != ']')
    {
	ఒకవేళ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	    p += l;
	లేకపోతే
	    ఒకవేళ (*p == '-')
	    {
		++p;
		ఒకవేళ (*p != ']' && *p != NUL)
		    MB_PTR_ADV(p);
	    }
	లేకపోతే ఒకవేళ (*p == '\\'
		&& !reg_cpo_bsl
		&& (vim_strchr(REGEXP_INRANGE, p[1]) != ఖాళీ
		    || (!reg_cpo_lit && vim_strchr(REGEXP_ABBR, p[1]) != ఖాళీ)))
	    p += 2;
	లేకపోతే ఒకవేళ (*p == '[')
	{
	    ఒకవేళ (get_char_class(&p) == CLASS_NONE
		    && get_equi_class(&p) == 0
		    && get_coll_element(&p) == 0
		    && *p != NUL)
		++p; // it is not a class name and not NUL
	}
	లేకపోతే
	    ++p;
    }

    తిరిగి p;
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
    పూర్ణాంకం		delim,
    పూర్ణాంకం		magic)
{
    తిరిగి skip_regexp_ex(startp, delim, magic, ఖాళీ, ఖాళీ, ఖాళీ);
}

/*
 * Call skip_regexp() and when the delimiter does not match give an error and
 * return NULL.
 */
    char_u *
skip_regexp_err(
    char_u	*startp,
    పూర్ణాంకం		delim,
    పూర్ణాంకం		magic)
{
    char_u *p = skip_regexp(startp, delim, magic);

    ఒకవేళ (*p != delim)
    {
	semsg(_(e_missing_delimiter_after_శోధన_నమూనా_str), startp);
	తిరిగి ఖాళీ;
    }
    తిరిగి p;
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
    పూర్ణాంకం		dirc,
    పూర్ణాంకం		magic,
    char_u	**newp,
    పూర్ణాంకం		*dropped,
    magic_T	*magic_val)
{
    magic_T	mymagic;
    char_u	*p = startp;
    size_t	startplen = 0;

    ఒకవేళ (magic)
	mymagic = MAGIC_ON;
    లేకపోతే
	mymagic = MAGIC_OFF;
    get_cpo_flags();

    కొరకు (; p[0] != NUL; MB_PTR_ADV(p))
    {
	ఒకవేళ (p[0] == dirc)	// found end of regexp
	    ఆపు;
	ఒకవేళ ((p[0] == '[' && mymagic >= MAGIC_ON)
		|| (p[0] == '\\' && p[1] == '[' && mymagic <= MAGIC_OFF))
	{
	    p = skip_anyof(p + 1);
	    ఒకవేళ (p[0] == NUL)
		ఆపు;
	}
	లేకపోతే ఒకవేళ (p[0] == '\\' && p[1] != NUL)
	{
	    ఒకవేళ (dirc == '?' && newp != ఖాళీ && p[1] == '?')
	    {
		// change "\?" to "?", make a copy first.
		ఒకవేళ (startplen == 0)
		    startplen = STRLEN(startp);
		ఒకవేళ (*newp == ఖాళీ)
		{
		    *newp = vim_strnsave(startp, startplen);
		    ఒకవేళ (*newp != ఖాళీ)
		    {
			p = *newp + (p - startp);
			startp = *newp;
		    }
		}
		ఒకవేళ (dropped != ఖాళీ)
		    ++*dropped;
		ఒకవేళ (*newp != ఖాళీ)
		    mch_memmove(p, p + 1, startplen - ((p + 1) - startp) + 1);
		లేకపోతే
		    ++p;
	    }
	    లేకపోతే
		++p;    // skip next character
	    ఒకవేళ (*p == 'v')
		mymagic = MAGIC_ALL;
	    లేకపోతే ఒకవేళ (*p == 'V')
		mymagic = MAGIC_NONE;
	}
    }
    ఒకవేళ (magic_val != ఖాళీ)
	*magic_val = mymagic;
    తిరిగి p;
}

/*
 * Functions for getting characters from the regexp input.
 */
స్థిర పూర్ణాంకం	prevchr_len;	// byte length of previous char
స్థిర పూర్ణాంకం	at_ప్రారంభం;	// True when on the first character
స్థిర పూర్ణాంకం	prev_at_ప్రారంభం;  // True when on the second character

/*
 * Start parsing at "str".
 */
    స్థిర శూన్య
initchr(char_u *str)
{
    regparse = str;
    prevchr_len = 0;
    curchr = prevprevchr = prevchr = nextchr = -1;
    at_ప్రారంభం = TRUE;
    prev_at_ప్రారంభం = FALSE;
}

/*
 * Save the current parse state, so that it can be restored and parsing
 * starts in the same state again.
 */
    స్థిర శూన్య
save_parse_state(parse_state_T *ps)
{
    ps->regparse = regparse;
    ps->prevchr_len = prevchr_len;
    ps->curchr = curchr;
    ps->prevchr = prevchr;
    ps->prevprevchr = prevprevchr;
    ps->nextchr = nextchr;
    ps->at_ప్రారంభం = at_ప్రారంభం;
    ps->prev_at_ప్రారంభం = prev_at_ప్రారంభం;
    ps->regnpar = regnpar;
}

/*
 * Restore a previously saved parse state.
 */
    స్థిర శూన్య
restore_parse_state(parse_state_T *ps)
{
    regparse = ps->regparse;
    prevchr_len = ps->prevchr_len;
    curchr = ps->curchr;
    prevchr = ps->prevchr;
    prevprevchr = ps->prevprevchr;
    nextchr = ps->nextchr;
    at_ప్రారంభం = ps->at_ప్రారంభం;
    prev_at_ప్రారంభం = ps->prev_at_ప్రారంభం;
    regnpar = ps->regnpar;
}


/*
 * Get the next character without advancing.
 */
    స్థిర పూర్ణాంకం
peekchr(శూన్య)
{
    స్థిర పూర్ణాంకం	after_slash = FALSE;

    ఒకవేళ (curchr != -1)
	తిరిగి curchr;

    ఎంపిక (curchr = regparse[0])
    {
	స్థితి '.':
	స్థితి '[':
	స్థితి '~':
	    // magic when 'magic' is on
	    ఒకవేళ (reg_magic >= MAGIC_ON)
		curchr = Magic(curchr);
	    ఆపు;
	స్థితి '(':
	స్థితి ')':
	స్థితి '{':
	స్థితి '%':
	స్థితి '+':
	స్థితి '=':
	స్థితి '?':
	స్థితి '@':
	స్థితి '!':
	స్థితి '&':
	స్థితి '|':
	స్థితి '<':
	స్థితి '>':
	స్థితి '#':	// future ext.
	స్థితి '"':	// future ext.
	స్థితి '\'':	// future ext.
	స్థితి ',':	// future ext.
	స్థితి '-':	// future ext.
	స్థితి ':':	// future ext.
	స్థితి ';':	// future ext.
	స్థితి '`':	// future ext.
	స్థితి '/':	// Can't be used in / command
			// magic only after "\v"
	    ఒకవేళ (reg_magic == MAGIC_ALL)
		curchr = Magic(curchr);
	    ఆపు;
	స్థితి '*':
	    // * is not magic as the very first character, eg "?*ptr", when
	    // after '^', eg "/^*ptr" and when after "\(", "\|", "\&".  But
	    // "\(\*" is not magic, thus must be magic if "after_slash"
	    ఒకవేళ (reg_magic >= MAGIC_ON
		    && !at_ప్రారంభం
		    && !(prev_at_ప్రారంభం && prevchr == Magic('^'))
		    && (after_slash
			|| (prevchr != Magic('(')
			    && prevchr != Magic('&')
			    && prevchr != Magic('|'))))
		curchr = Magic('*');
	    ఆపు;
	స్థితి '^':
	    // '^' is only magic as the very first character and if it's after
	    // "\(", "\|", "\&' or "\n"
	    ఒకవేళ (reg_magic >= MAGIC_OFF
		    && (at_ప్రారంభం
			|| reg_magic == MAGIC_ALL
			|| prevchr == Magic('(')
			|| prevchr == Magic('|')
			|| prevchr == Magic('&')
			|| prevchr == Magic('n')
			|| (no_Magic(prevchr) == '('
			    && prevprevchr == Magic('%'))))
	    {
		curchr = Magic('^');
		at_ప్రారంభం = TRUE;
		prev_at_ప్రారంభం = FALSE;
	    }
	    ఆపు;
	స్థితి '$':
	    // '$' is only magic as the very last char and if it's in front of
	    // either "\|", "\)", "\&", or "\n"
	    ఒకవేళ (reg_magic >= MAGIC_OFF)
	    {
		char_u *p = regparse + 1;
		పూర్ణాంకం is_magic_all = (reg_magic == MAGIC_ALL);

		// ignore \c \C \m \M \v \V and \Z after '$'
		అయితే (p[0] == '\\' && (p[1] == 'c' || p[1] == 'C'
			    || p[1] == 'm' || p[1] == 'M'
			    || p[1] == 'v' || p[1] == 'V' || p[1] == 'Z'))
		{
		    ఒకవేళ (p[1] == 'v')
			is_magic_all = TRUE;
		    లేకపోతే ఒకవేళ (p[1] == 'm' || p[1] == 'M' || p[1] == 'V')
			is_magic_all = FALSE;
		    p += 2;
		}
		ఒకవేళ (p[0] == NUL
			|| (p[0] == '\\'
			    && (p[1] == '|' || p[1] == '&' || p[1] == ')'
				|| p[1] == 'n'))
			|| (is_magic_all
			    && (p[0] == '|' || p[0] == '&' || p[0] == ')'))
			|| reg_magic == MAGIC_ALL)
		    curchr = Magic('$');
	    }
	    ఆపు;
	స్థితి '\\':
	    {
		పూర్ణాంకం c = regparse[1];

		ఒకవేళ (c == NUL)
		    curchr = '\\';	// trailing '\'
		లేకపోతే ఒకవేళ (c <= '~' && META_flags[c])
		{
		    /*
		     * META contains everything that may be magic sometimes,
		     * except ^ and $ ("\^" and "\$" are only magic after
		     * "\V").  We now fetch the next character and toggle its
		     * magicness.  Therefore, \ is so meta-magic that it is
		     * not in META.
		     */
		    curchr = -1;
		    prev_at_ప్రారంభం = at_ప్రారంభం;
		    at_ప్రారంభం = FALSE;	// be able to say "/\*ptr"
		    ++regparse;
		    ++after_slash;
		    peekchr();
		    --regparse;
		    --after_slash;
		    curchr = toggle_Magic(curchr);
		}
		లేకపోతే ఒకవేళ (vim_strchr(REGEXP_ABBR, c))
		{
		    /*
		     * Handle abbreviations, like "\t" for TAB -- webb
		     */
		    curchr = backslash_trans(c);
		}
		లేకపోతే ఒకవేళ (reg_magic == MAGIC_NONE && (c == '$' || c == '^'))
		    curchr = toggle_Magic(c);
		లేకపోతే
		{
		    /*
		     * Next character can never be (made) magic?
		     * Then backslashing it won't do anything.
		     */
		    ఒకవేళ (has_mbyte && c >= 0x80)
			curchr = (*mb_ptr2char)(regparse + 1);
		    లేకపోతే
			curchr = c;
		}
		ఆపు;
	    }

	అప్రమేయ:
	    // curchr already holds regparse[0]; only a multi-byte lead byte
	    // needs decoding.
	    ఒకవేళ (has_mbyte && curchr >= 0x80)
		curchr = (*mb_ptr2char)(regparse);
    }

    తిరిగి curchr;
}

/*
 * Eat one lexed character.  Do this in a way that we can undo it.
 */
    స్థిర శూన్య
skipchr(శూన్య)
{
    // peekchr() eats a backslash, do the same here
    ఒకవేళ (*regparse == '\\')
	prevchr_len = 1;
    లేకపోతే
	prevchr_len = 0;
    ఒకవేళ (regparse[prevchr_len] != NUL)
    {
	ఒకవేళ (enc_utf8)
	{
	    // Exclude composing chars that mb_ptr2len does include.  A byte
	    // below 0x80 is always a single character.
	    ఒకవేళ (regparse[prevchr_len] < 0x80)
		++prevchr_len;
	    లేకపోతే
		prevchr_len += utf_ptr2len(regparse + prevchr_len);
	}
	లేకపోతే ఒకవేళ (has_mbyte)
	    prevchr_len += (*mb_ptr2len)(regparse + prevchr_len);
	లేకపోతే
	    ++prevchr_len;
    }
    regparse += prevchr_len;
    prev_at_ప్రారంభం = at_ప్రారంభం;
    at_ప్రారంభం = FALSE;
    prevprevchr = prevchr;
    prevchr = curchr;
    curchr = nextchr;	    // use previously unget char, or -1
    nextchr = -1;
}

/*
 * Skip a character while keeping the value of prev_at_start for at_start.
 * prevchr and prevprevchr are also kept.
 */
    స్థిర శూన్య
skipchr_keepstart(శూన్య)
{
    పూర్ణాంకం as = prev_at_ప్రారంభం;
    పూర్ణాంకం pr = prevchr;
    పూర్ణాంకం prpr = prevprevchr;

    skipchr();
    at_ప్రారంభం = as;
    prevchr = pr;
    prevprevchr = prpr;
}

/*
 * Get the next character from the pattern. We know about magic and such, so
 * therefore we need a lexical analyzer.
 */
    స్థిర పూర్ణాంకం
getchr(శూన్య)
{
    పూర్ణాంకం chr = peekchr();

    skipchr();
    తిరిగి chr;
}

/*
 * put character back.  Works only once!
 */
    స్థిర శూన్య
ungetchr(శూన్య)
{
    nextchr = curchr;
    curchr = prevchr;
    prevchr = prevprevchr;
    at_ప్రారంభం = prev_at_ప్రారంభం;
    prev_at_ప్రారంభం = FALSE;

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
    స్థిర vimlong_T
gethexchrs(పూర్ణాంకం maxinputlen)
{
    long_u	nr = 0;
    పూర్ణాంకం		c;
    పూర్ణాంకం		i;

    కొరకు (i = 0; i < maxinputlen; ++i)
    {
	c = regparse[0];
	ఒకవేళ (!vim_isxdigit(c))
	    ఆపు;
	nr <<= 4;
	nr |= hex2nr(c);
	++regparse;
    }

    ఒకవేళ (i == 0)
	తిరిగి -1;
    తిరిగి nr;
}

/*
 * Get and return the value of the decimal string immediately after the
 * current position. Return -1 for invalid.  Consumes all digits.
 */
    స్థిర vimlong_T
getdecchrs(శూన్య)
{
    long_u	nr = 0;
    పూర్ణాంకం		c;
    పూర్ణాంకం		i;

    కొరకు (i = 0; ; ++i)
    {
	c = regparse[0];
	ఒకవేళ (c < '0' || c > '9')
	    ఆపు;
	nr *= 10;
	nr += c - '0';
	++regparse;
	curchr = -1; // no longer valid
    }

    ఒకవేళ (i == 0)
	తిరిగి -1;
    తిరిగి nr;
}

/*
 * get and return the value of the octal string immediately after the current
 * position. Return -1 for invalid, or 0-255 for valid. Smart enough to handle
 * numbers > 377 correctly (for example, 400 is treated as 40) and doesn't
 * treat 8 or 9 as recognised characters. Position is updated:
 *     blahblah\%o210asdf
 *	   before-^  ^-after
 */
    స్థిర దీర్ఘ
getoctchrs(శూన్య)
{
    long_u	nr = 0;
    పూర్ణాంకం		c;
    పూర్ణాంకం		i;

    కొరకు (i = 0; i < 3 && nr < 040; ++i)
    {
	c = regparse[0];
	ఒకవేళ (c < '0' || c > '7')
	    ఆపు;
	nr <<= 3;
	nr |= hex2nr(c);
	++regparse;
    }

    ఒకవేళ (i == 0)
	తిరిగి -1;
    తిరిగి (దీర్ఘ)nr;
}

/*
 * read_limits - Read two integers to be taken as a minimum and maximum.
 * If the first character is '-', then the range is reversed.
 * Should end with 'end'.  If minval is missing, zero is default, if maxval is
 * missing, a very big number is the default.
 */
    స్థిర పూర్ణాంకం
read_limits(దీర్ఘ *minval, దీర్ఘ *maxval)
{
    పూర్ణాంకం		తిరగేయి = FALSE;
    char_u	*మొదటి_char;
    దీర్ఘ	tmp;

    ఒకవేళ (*regparse == '-')
    {
	// Starts with '-', so reverse the range later
	regparse++;
	తిరగేయి = TRUE;
    }
    మొదటి_char = regparse;
    *minval = getdigits(&regparse);
    ఒకవేళ (*regparse == ',')	    // There is a comma
    {
	ఒకవేళ (vim_isdigit(*++regparse))
	    *maxval = getdigits(&regparse);
	లేకపోతే
	    *maxval = MAX_హద్దు;
    }
    లేకపోతే ఒకవేళ (VIM_ISDIGIT(*మొదటి_char))
	*maxval = *minval;	    // It was \{n} or \{-n}
    లేకపోతే
	*maxval = MAX_హద్దు;	    // It was \{} or \{-}
    ఒకవేళ (*regparse == '\\')
	regparse++;	// Allow either \{...} or \{...\}
    ఒకవేళ (*regparse != '}')
	EMSG2_RET_FAIL(_(e_syntax_error_లోపల_str_curlies),
						       reg_magic == MAGIC_ALL);

    /*
     * Reverse the range if there was a '-', or make sure it is in the right
     * order otherwise.
     */
    ఒకవేళ ((!తిరగేయి && *minval > *maxval) || (తిరగేయి && *minval < *maxval))
    {
	tmp = *minval;
	*minval = *maxval;
	*maxval = tmp;
    }
    skipchr();		// let's be friends with the lexer again
    తిరిగి OK;
}

/*
 * vim_regexec and friends
 */

/*
 * Global work variables for vim_regexec().
 */

స్థిర శూన్య	cleanup_subexpr(శూన్య);
#నిర్వచితమైతే FEAT_SYN_HL
స్థిర శూన్య	cleanup_zsubexpr(శూన్య);
#అయితే_ముగింపు
స్థిర పూర్ణాంకం	సరిపోలిక_with_backref(linenr_T ప్రారంభం_lnum, colnr_T ప్రారంభం_నిలువు, linenr_T చివర_lnum, colnr_T చివర_నిలువు, పూర్ణాంకం *bytelen);

/*
 * Sometimes need to save a copy of a line.  Since alloc()/free() is very
 * slow, we keep one allocated piece of memory and only re-allocate it when
 * it's too small.  It's freed in bt_regexec_both() when finished.
 */
స్థిర char_u	*reg_tofree = ఖాళీ;
స్థిర గుర్తులేని	reg_tofreelen;

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
రకనిర్వచనం నిర్మాణం {
    regmatch_T		*reg_సరిపోలిక;
    regmmatch_T		*reg_mmatch;

    char_u		**reg_startp;
    char_u		**reg_endp;
    lpos_T		*reg_startpos;
    lpos_T		*reg_endpos;

    win_T		*reg_win;
    buf_T		*reg_buf;
    linenr_T		reg_firstlnum;
    linenr_T		reg_maxline;
    పూర్ణాంకం			reg_పంక్తి_lbr;	// "\n" in string is line break

    // The current match-position is stord in these variables:
    linenr_T	lnum;		// line number, relative to first line
    char_u	*పంక్తి;		// start of current line
    char_u	*ఆదానం;		// current input, points into "line"

    పూర్ణాంకం	need_clear_subexpr;	// subexpressions still need to be cleared
#నిర్వచితమైతే FEAT_SYN_HL
    పూర్ణాంకం	need_clear_zsubexpr;	// extmatch subexpressions still need to be
				// cleared
#అయితే_ముగింపు

    // Internal copy of 'ignorecase'.  It is set at each call to vim_regexec().
    // Normally it gets the value of "rm_ic" or "rmm_ic", but when the pattern
    // contains '\c' or '\C' the value is overruled.
    పూర్ణాంకం			reg_ic;

    // Similar to "reg_ic", but only for 'combining' characters.  Set with \Z
    // flag in the regexp.  Defaults to false, always.
    పూర్ణాంకం			reg_icombine;

    // Copy of "rmm_maxcol": maximum column to search for a match.  Zero when
    // there is no maximum.
    colnr_T		reg_maxcol;

    // State for the NFA engine regexec.
    పూర్ణాంకం nfa_has_zend;	    // NFA regexp \ze operator encountered.
    పూర్ణాంకం nfa_has_backref;    // NFA regexp \1 .. \9 encountered.
    పూర్ణాంకం nfa_nsubexpr;	    // Number of sub expressions actually being used
			    // during execution. 1 if only the whole match
			    // (subexpr 0) is used.
    // listid is global, so that it increases on recursive calls to
    // nfa_regmatch(), which means we don't have to clear the lastlist field of
    // all the states.
    పూర్ణాంకం nfa_listid;
    పూర్ణాంకం nfa_alt_listid;

#నిర్వచితమైతే FEAT_SYN_HL
    పూర్ణాంకం nfa_has_zsubexpr;   // NFA regexp has \z( ), set zsubexpr.
#అయితే_ముగింపు
} regexec_T;

స్థిర regexec_T	rex;
స్థిర పూర్ణాంకం		rex_లోపల_use = FALSE;

/*
 * Return TRUE if character 'c' is included in 'iskeyword' option for
 * "reg_buf" buffer.
 */
    స్థిర పూర్ణాంకం
reg_iswordc(పూర్ణాంకం c)
{
    తిరిగి vim_iswordc_buf(c, rex.reg_buf);
}

#నిర్వచితమైతే FEAT_EVAL
స్థిర పూర్ణాంకం can_f_submatch = FALSE;	// TRUE when submatch() can be used

// This struct is used for reg_submatch(). Needed for when the
// substitution string is an expression that contains a call to substitute()
// and submatch().
రకనిర్వచనం నిర్మాణం {
    regmatch_T	*sm_సరిపోలిక;
    regmmatch_T	*sm_mmatch;
    linenr_T	sm_firstlnum;
    linenr_T	sm_maxline;
    పూర్ణాంకం		sm_పంక్తి_lbr;
} regsubmatch_T;

స్థిర regsubmatch_T rsm;  // can only be used when can_f_submatch is TRUE
#అయితే_ముగింపు

రకనిర్వచనం గణన
{
    RGLF_పంక్తి = 0x01,
    RGLF_నిడివి = 0x02
#నిర్వచితమైతే FEAT_EVAL
    ,
    RGLF_SUBMATCH = 0x04
#అయితే_ముగింపు
} reg_getline_flags_T;

//
// common code for reg_getline(), reg_getline_len(), reg_getline_submatch() and
// reg_getline_submatch_len().
// the flags argument (which is a bitmask) controls what info is to be returned and whether
// or not submatch is in effect.
// note:
//     submatch is available only if FEAT_EVAL is defined.
    స్థిర శూన్య
reg_getline_common(
    linenr_T		lnum,
    reg_getline_flags_T	flags,
    char_u		**పంక్తి,
    colnr_T		*నిడివి)
{
    పూర్ణాంకం get_పంక్తి = flags & RGLF_పంక్తి;
    పూర్ణాంకం get_నిడివి = flags & RGLF_నిడివి;
    linenr_T firstlnum;
    linenr_T maxline;

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (flags & RGLF_SUBMATCH)
    {
	firstlnum = rsm.sm_firstlnum + lnum;
	maxline = rsm.sm_maxline;
    }
    లేకపోతే
#అయితే_ముగింపు
    {
	firstlnum = rex.reg_firstlnum + lnum;
	maxline = rex.reg_maxline;
    }

    // when looking behind for a match/no-match lnum is negative. but we
    // can't go before line 1.
    ఒకవేళ (firstlnum < 1)
    {
	ఒకవేళ (get_పంక్తి)
	    *పంక్తి = ఖాళీ;
	ఒకవేళ (get_నిడివి)
	    *నిడివి = 0;

	తిరిగి;
    }

    ఒకవేళ (lnum > maxline)
    {
	// must have matched the "\n" in the last line.
	ఒకవేళ (get_పంక్తి)
	    *పంక్తి = (char_u *)"";
	ఒకవేళ (get_నిడివి)
	    *నిడివి = 0;

	తిరిగి;
    }

    ఒకవేళ (get_పంక్తి)
	*పంక్తి = ml_get_buf(rex.reg_buf, firstlnum, FALSE);
    ఒకవేళ (get_నిడివి)
	*నిడివి = ml_get_buf_len(rex.reg_buf, firstlnum);
}

/*
 * Get pointer to the line "lnum", which is relative to "reg_firstlnum".
 */
    స్థిర char_u *
reg_getline(linenr_T lnum)
{
    char_u *పంక్తి;

    reg_getline_common(lnum, RGLF_పంక్తి, &పంక్తి, ఖాళీ);

    తిరిగి పంక్తి;
}

/*
 * Get length of line "lnum", which is relative to "reg_firstlnum".
 */
    స్థిర colnr_T
reg_getline_len(linenr_T lnum)
{
    colnr_T నిడివి;

    reg_getline_common(lnum, RGLF_నిడివి, ఖాళీ, &నిడివి);

    తిరిగి నిడివి;
}

#నిర్వచితమైతే FEAT_SYN_HL
స్థిర char_u	*reg_startzp[NSUBEXP];	// Workspace to mark beginning
స్థిర char_u	*reg_endzp[NSUBEXP];	//   and end of \z(...\) matches
స్థిర lpos_T	reg_startzpos[NSUBEXP];	// idem, beginning pos
స్థిర lpos_T	reg_endzpos[NSUBEXP];	// idem, end pos
#అయితే_ముగింపు

// TRUE if using multi-line regexp.
#నిర్వచించు REG_MULTI	(rex.reg_సరిపోలిక == ఖాళీ)

#నిర్వచితమైతే FEAT_SYN_HL
/*
 * Create a new extmatch and mark it as referenced once.
 */
    స్థిర reg_extmatch_T *
make_extmatch(శూన్య)
{
    reg_extmatch_T	*em;

    em = ALLOC_CLEAR_ONE(reg_extmatch_T);
    ఒకవేళ (em != ఖాళీ)
	em->refcnt = 1;
    తిరిగి em;
}

/*
 * Add a reference to an extmatch.
 */
    reg_extmatch_T *
ref_extmatch(reg_extmatch_T *em)
{
    ఒకవేళ (em != ఖాళీ)
	em->refcnt++;
    తిరిగి em;
}

/*
 * Remove a reference to an extmatch.  If there are no references left, free
 * the info.
 */
    శూన్య
unref_extmatch(reg_extmatch_T *em)
{
    పూర్ణాంకం i;

    ఒకవేళ (em != ఖాళీ && --em->refcnt <= 0)
    {
	కొరకు (i = 0; i < NSUBEXP; ++i)
	    vim_free(em->matches[i]);
	vim_free(em);
    }
}
#అయితే_ముగింపు

/*
 * Get class of previous character.
 */
    స్థిర పూర్ణాంకం
reg_prev_class(శూన్య)
{
    ఒకవేళ (rex.ఆదానం > rex.పంక్తి)
	తిరిగి mb_get_class_buf(rex.ఆదానం - 1
		       - (*mb_తల_off)(rex.పంక్తి, rex.ఆదానం - 1), rex.reg_buf);
    తిరిగి -1;
}

/*
 * Return TRUE if the current rex.input position matches the Visual area.
 */
    స్థిర పూర్ణాంకం
reg_సరిపోలిక_visual(శూన్య)
{
    pos_T	శిఖరం, bot;
    linenr_T    lnum;
    colnr_T	నిలువు;
    win_T	*wp = rex.reg_win == ఖాళీ ? curwin : rex.reg_win;
    పూర్ణాంకం		బహుళకం;
    colnr_T	ప్రారంభం, చివర;
    colnr_T	start2, end2;
    colnr_T	నిలువుల_సంఖ్య;
    colnr_T	curswant;

    // Check if the buffer is the current buffer and not using a string.
    ఒకవేళ (rex.reg_buf != curbuf || VIsual.lnum == 0 || !REG_MULTI)
	తిరిగి FALSE;

    ఒకవేళ (VIsual_చురుకైన)
    {
	ఒకవేళ (LT_POS(VIsual, wp->w_cursor))
	{
	    శిఖరం = VIsual;
	    bot = wp->w_cursor;
	}
	లేకపోతే
	{
	    శిఖరం = wp->w_cursor;
	    bot = VIsual;
	}
	బహుళకం = VIsual_బహుళకం;
	curswant = wp->w_curswant;
    }
    లేకపోతే
    {
	ఒకవేళ (LT_POS(curbuf->b_visual.vi_ప్రారంభం, curbuf->b_visual.vi_చివర))
	{
	    శిఖరం = curbuf->b_visual.vi_ప్రారంభం;
	    bot = curbuf->b_visual.vi_చివర;
	}
	లేకపోతే
	{
	    శిఖరం = curbuf->b_visual.vi_చివర;
	    bot = curbuf->b_visual.vi_ప్రారంభం;
	}
	// a substitute command may have removed some lines
	ఒకవేళ (bot.lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
	    bot.lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;
	బహుళకం = curbuf->b_visual.vi_బహుళకం;
	curswant = curbuf->b_visual.vi_curswant;
    }
    lnum = rex.lnum + rex.reg_firstlnum;
    ఒకవేళ (lnum < శిఖరం.lnum || lnum > bot.lnum)
	తిరిగి FALSE;

    నిలువు = (colnr_T)(rex.ఆదానం - rex.పంక్తి);
    ఒకవేళ (బహుళకం == 'v')
    {
	ఒకవేళ ((lnum == శిఖరం.lnum && నిలువు < శిఖరం.నిలువు)
		|| (lnum == bot.lnum && నిలువు >= bot.నిలువు + (*p_sel != 'e')))
	    తిరిగి FALSE;
    }
    లేకపోతే ఒకవేళ (బహుళకం == Ctrl_V)
    {
	getvvcol(wp, &శిఖరం, &ప్రారంభం, ఖాళీ, &చివర, 0);
	getvvcol(wp, &bot, &start2, ఖాళీ, &end2, 0);
	ఒకవేళ (start2 < ప్రారంభం)
	    ప్రారంభం = start2;
	ఒకవేళ (end2 > చివర)
	    చివర = end2;
	ఒకవేళ (శిఖరం.నిలువు == MAXCOL || bot.నిలువు == MAXCOL || curswant == MAXCOL)
	    చివర = MAXCOL;

	// getvvcol() flushes rex.line, need to get it again
	rex.పంక్తి = reg_getline(rex.lnum);
	rex.ఆదానం = rex.పంక్తి + నిలువు;

	నిలువుల_సంఖ్య = win_linetabsize(wp, rex.reg_firstlnum + rex.lnum, rex.పంక్తి, నిలువు);
	ఒకవేళ (నిలువుల_సంఖ్య < ప్రారంభం || నిలువుల_సంఖ్య > చివర - (*p_sel == 'e'))
	    తిరిగి FALSE;
    }
    తిరిగి TRUE;
}

/*
 * Check the regexp program for its magic number.
 * Return TRUE if it's wrong.
 */
    స్థిర పూర్ణాంకం
prog_magic_wrong(శూన్య)
{
    regprog_T	*prog;

    prog = REG_MULTI ? rex.reg_mmatch->regprog : rex.reg_సరిపోలిక->regprog;
    ఒకవేళ (prog->engine == &nfa_regengine)
	// For NFA matcher we don't check the magic
	తిరిగి FALSE;

    ఒకవేళ (UCHARAT(((bt_regprog_T *)prog)->కార్యక్రమం) != REGMAGIC)
    {
	iemsg(e_corrupted_regexp_కార్యక్రమం);
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Cleanup the subexpressions, if this wasn't done yet.
 * This construction is used to clear the subexpressions only when they are
 * used (to increase speed).
 */
    స్థిర శూన్య
cleanup_subexpr(శూన్య)
{
    ఒకవేళ (!rex.need_clear_subexpr)
	తిరిగి;

    ఒకవేళ (REG_MULTI)
    {
	// Use 0xff to set lnum to -1
	vim_memset(rex.reg_startpos, 0xff, పరిమాణం(lpos_T) * NSUBEXP);
	vim_memset(rex.reg_endpos, 0xff, పరిమాణం(lpos_T) * NSUBEXP);
    }
    లేకపోతే
    {
	vim_memset(rex.reg_startp, 0, పరిమాణం(char_u *) * NSUBEXP);
	vim_memset(rex.reg_endp, 0, పరిమాణం(char_u *) * NSUBEXP);
    }
    rex.need_clear_subexpr = FALSE;
}

#నిర్వచితమైతే FEAT_SYN_HL
    స్థిర శూన్య
cleanup_zsubexpr(శూన్య)
{
    ఒకవేళ (!rex.need_clear_zsubexpr)
	తిరిగి;

    ఒకవేళ (REG_MULTI)
    {
	// Use 0xff to set lnum to -1
	vim_memset(reg_startzpos, 0xff, పరిమాణం(lpos_T) * NSUBEXP);
	vim_memset(reg_endzpos, 0xff, పరిమాణం(lpos_T) * NSUBEXP);
    }
    లేకపోతే
    {
	vim_memset(reg_startzp, 0, పరిమాణం(char_u *) * NSUBEXP);
	vim_memset(reg_endzp, 0, పరిమాణం(char_u *) * NSUBEXP);
    }
    rex.need_clear_zsubexpr = FALSE;
}
#అయితే_ముగింపు

/*
 * Advance rex.lnum, rex.line and rex.input to the next line.
 */
    స్థిర శూన్య
reg_nextline(శూన్య)
{
    rex.పంక్తి = reg_getline(++rex.lnum);
    rex.ఆదానం = rex.పంక్తి;
    fast_breakcheck();
}

/*
 * Check whether a backreference matches.
 * Returns RA_FAIL, RA_NOMATCH or RA_MATCH.
 * If "bytelen" is not NULL, it is set to the byte length of the match in the
 * last line.
 * Optional: ignore case if rex.reg_ic is set.
 */
    స్థిర పూర్ణాంకం
సరిపోలిక_with_backref(
    linenr_T ప్రారంభం_lnum,
    colnr_T  ప్రారంభం_నిలువు,
    linenr_T చివర_lnum,
    colnr_T  చివర_నిలువు,
    పూర్ణాంకం	     *bytelen)
{
    linenr_T	clnum = ప్రారంభం_lnum;
    colnr_T	ccol = ప్రారంభం_నిలువు;
    పూర్ణాంకం		len;
    char_u	*p;

    ఒకవేళ (bytelen != ఖాళీ)
	*bytelen = 0;
    కొరకు (;;)
    {
	// Since getting one line may invalidate the other, need to make copy.
	// Slow!
	ఒకవేళ (rex.పంక్తి != reg_tofree)
	{
	    len = (పూర్ణాంకం)STRLEN(rex.పంక్తి);
	    ఒకవేళ (reg_tofree == ఖాళీ || len >= (పూర్ణాంకం)reg_tofreelen)
	    {
		len += 50;	// get some extra
		vim_free(reg_tofree);
		reg_tofree = alloc(len);
		ఒకవేళ (reg_tofree == ఖాళీ)
		    తిరిగి RA_FAIL; // out of memory!
		reg_tofreelen = len;
	    }
	    STRCPY(reg_tofree, rex.పంక్తి);
	    rex.ఆదానం = reg_tofree + (rex.ఆదానం - rex.పంక్తి);
	    rex.పంక్తి = reg_tofree;
	}

	// Get the line to compare with.
	p = reg_getline(clnum);
	ఒకవేళ (clnum == చివర_lnum)
	    len = చివర_నిలువు - ccol;
	లేకపోతే
	    len = (పూర్ణాంకం)reg_getline_len(clnum) - ccol;

	// Use case-insensitive compare if rex.reg_ic is set
	ఒకవేళ ((!rex.reg_ic && cstrncmp(p + ccol, rex.ఆదానం, &len) != 0)
	    || (rex.reg_ic && MB_STRNICMP(p + ccol, rex.ఆదానం, len) != 0))
	    తిరిగి RA_NOMATCH;  // doesn't match
	ఒకవేళ (bytelen != ఖాళీ)
	    *bytelen += len;
	ఒకవేళ (clnum == చివర_lnum)
	    ఆపు;		// match and at end!
	ఒకవేళ (rex.lnum >= rex.reg_maxline)
	    తిరిగి RA_NOMATCH;  // text too short

	// Advance to next line.
	reg_nextline();
	ఒకవేళ (bytelen != ఖాళీ)
	    *bytelen = 0;
	++clnum;
	ccol = 0;
	ఒకవేళ (got_int)
	    తిరిగి RA_FAIL;
    }

    // found a match!  Note that rex.line may now point to a copy of the line,
    // that should not matter.
    తిరిగి RA_సరిపోలిక;
}

/*
 * Used in a place where no * or \+ can follow.
 */
    స్థిర పూర్ణాంకం
re_mult_తదుపరి(అక్షరం *what)
{
    ఒకవేళ (re_multi_type(peekchr()) == MULTI_MULT)
    {
	semsg(_(e_nfa_regexp_cannot_repeat_str), what);
	rc_did_emsg = TRUE;
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం a, b, c;
} decomp_T;


// 0xfb20 - 0xfb4f
స్థిర decomp_T decomp_పట్టిక[0xfb4f-0xfb20+1] =
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

    స్థిర శూన్య
mb_decompose(పూర్ణాంకం c, పూర్ణాంకం *c1, పూర్ణాంకం *c2, పూర్ణాంకం *c3)
{
    decomp_T d;

    ఒకవేళ (c >= 0xfb20 && c <= 0xfb4f)
    {
	d = decomp_పట్టిక[c - 0xfb20];
	*c1 = d.a;
	*c2 = d.b;
	*c3 = d.c;
    }
    లేకపోతే
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
    స్థిర పూర్ణాంకం
cstrncmp(char_u *s1, char_u *s2, పూర్ణాంకం *n)
{
    పూర్ణాంకం		ఫలితం;

    ఒకవేళ (!rex.reg_ic)
	ఫలితం = STRNCMP(s1, s2, *n);
    లేకపోతే ఒకవేళ (enc_utf8)
    {
	char_u *p = s1;
	పూర్ణాంకం n2 = 0;
	పూర్ణాంకం n1 = *n;
	// count the number of characters for byte-length of s1
	అయితే (n1 > 0 && *p != NUL)
	{
	    n1 -= mb_ptr2len(p);
	    MB_PTR_ADV(p);
	    n2++;
	}
	// count the number of bytes to advance the same number of chars for s2
	p = s2;
	అయితే (n2-- > 0 && *p != NUL)
	    MB_PTR_ADV(p);

	n2 = p - s2;

	ఫలితం = MB_STRNICMP2(s1, s2, *n, n2);
	ఒకవేళ (ఫలితం == 0)
	    *n = n2;
    }
    లేకపోతే
	ఫలితం = MB_STRNICMP(s1, s2, *n);

    // if it failed and it's utf8 and we want to combineignore:
    ఒకవేళ (ఫలితం != 0 && enc_utf8 && rex.reg_icombine)
    {
	char_u	*str1, *str2;
	పూర్ణాంకం	c1, c2, c11, c12;
	పూర్ణాంకం	junk;

	// we have to handle the strcmp ourselves, since it is necessary to
	// deal with the composing characters by ignoring them:
	str1 = s1;
	str2 = s2;
	c1 = c2 = 0;
	అయితే ((పూర్ణాంకం)(str1 - s1) < *n)
	{
	    c1 = mb_ptr2char_adv(&str1);
	    c2 = mb_ptr2char_adv(&str2);

	    // Decompose the character if necessary, into 'base' characters.
	    // Currently hard-coded for Hebrew, Arabic to be done...
	    ఒకవేళ (c1 != c2 && (!rex.reg_ic || utf_fold(c1) != utf_fold(c2)))
	    {
		// decomposition necessary?
		mb_decompose(c1, &c11, &junk, &junk);
		mb_decompose(c2, &c12, &junk, &junk);
		c1 = c11;
		c2 = c12;
		ఒకవేళ (c11 != c12
			    && (!rex.reg_ic || utf_fold(c11) != utf_fold(c12)))
		    ఆపు;
	    }
	}
	ఫలితం = c2 - c1;
	ఒకవేళ (ఫలితం == 0)
	    *n = (పూర్ణాంకం)(str2 - s2);
    }

    తిరిగి ఫలితం;
}

/*
 * cstrchr: This function is used a lot for simple searches, keep it fast!
 */
    స్థిర char_u *
cstrchr(char_u *s, పూర్ణాంకం c)
{
    char_u	*p;
    పూర్ణాంకం		cc, lc;

    ఒకవేళ (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	తిరిగి vim_strchr(s, c);

    // tolower() and toupper() can be slow, comparing twice should be a lot
    // faster (esp. when using MS Visual C++!).
    // For UTF-8 need to use folded case.
    ఒకవేళ (enc_utf8 && c > 0x80)
    {
	cc = utf_fold(c);
	lc = cc;
    }
    లేకపోతే
	ఒకవేళ (MB_ISUPPER(c))
	{
	    cc = MB_TOLOWER(c);
	    lc = cc;
	}
	లేకపోతే ఒకవేళ (MB_ISLOWER(c))
	{
	    cc = MB_TOUPPER(c);
	    lc = c;
	}
	లేకపోతే
	    తిరిగి vim_strchr(s, c);

    ఒకవేళ (has_mbyte)
    {
	కొరకు (p = s; *p != NUL; p += (*mb_ptr2len)(p))
	{
	    పూర్ణాంకం uc = utf_ptr2char(p);
	    ఒకవేళ (enc_utf8 && (c > 0x80 || uc > 0x80))
	    {
		// Do not match an illegal byte.  E.g. 0xff matches 0xc3 0xbf,
		// not 0xff.
		// compare with lower case of the character
		ఒకవేళ ((uc < 0x80 || uc != *p) && utf_fold(uc) == lc)
		    తిరిగి p;
	    }
	    లేకపోతే ఒకవేళ (*p == c || *p == cc)
		తిరిగి p;
	}
    }
    లేకపోతే
	// Faster version for when there are no multi-byte characters.
	కొరకు (p = s; *p != NUL; ++p)
	    ఒకవేళ (*p == c || *p == cc)
		తిరిగి p;

    తిరిగి ఖాళీ;
}

////////////////////////////////////////////////////////////////
//		      regsub stuff			      //
////////////////////////////////////////////////////////////////

రకనిర్వచనం శూన్య (*fptr_T)(పూర్ణాంకం *, పూర్ణాంకం);

స్థిర పూర్ణాంకం vim_regsub_both(char_u *ఆధారం, typval_T *సమాసం, char_u *dest, పూర్ణాంకం destlen, పూర్ణాంకం flags);

    స్థిర శూన్య
do_పై(పూర్ణాంకం *d, పూర్ణాంకం c)
{
    *d = MB_TOUPPER(c);
}

    స్థిర శూన్య
do_క్రింది(పూర్ణాంకం *d, పూర్ణాంకం c)
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
regtilde(char_u *ఆధారం, పూర్ణాంకం magic)
{
    char_u	*newsub = ఆధారం;
    char_u	*p;
    size_t	newsublen = 0;
    char_u	tilde[3] = {'~', NUL, NUL};
    size_t	tildelen = 1;
    పూర్ణాంకం		error = FALSE;

    ఒకవేళ (!magic)
    {
	tilde[0] = '\\';
	tilde[1] = '~';
	tilde[2] = NUL;
	tildelen = 2;
    }

    కొరకు (p = newsub; *p; ++p)
    {
	ఒకవేళ (STRNCMP(p, tilde, tildelen) == 0)
	{
	    size_t prefixlen = p - newsub;		// not including the tilde
	    char_u *postfix = p + tildelen;
	    size_t postfixlen;
	    size_t tmpsublen;

	    ఒకవేళ (newsublen == 0)
		newsublen = STRLEN(newsub);
	    newsublen -= tildelen;
	    postfixlen = newsublen - prefixlen;
	    tmpsublen = prefixlen + reg_prev_sublen + postfixlen;

	    ఒకవేళ (tmpsublen > 0 && reg_prev_sub != ఖాళీ)
	    {
		char_u *tmpsub;

		// Avoid making the text longer than MAXCOL, it will cause
		// trouble at some point.
		ఒకవేళ (tmpsublen > MAXCOL)
		{
		    emsg(_(e_resulting_పాఠ్యం_too_long));
		    error = TRUE;
		    ఆపు;
		}

		tmpsub = alloc(tmpsublen + 1);
		ఒకవేళ (tmpsub == ఖాళీ)
		{
		    emsg(_(e_బహిర్గతం_యొక్క_జ్ఞాపకం));
		    error = TRUE;
		    ఆపు;
		}

		// copy prefix
		mch_memmove(tmpsub, newsub, prefixlen);
		// interpret tilde
		mch_memmove(tmpsub + prefixlen, reg_prev_sub, reg_prev_sublen);
		// copy postfix
		STRCPY(tmpsub + prefixlen + reg_prev_sublen, postfix);

		ఒకవేళ (newsub != ఆధారం)	// allocated newsub before
		    vim_free(newsub);
		newsub = tmpsub;
		newsublen = tmpsublen;
		p = newsub + prefixlen + reg_prev_sublen;
	    }
	    లేకపోతే
		mch_memmove(p, postfix, postfixlen + 1);	// remove the tilde (+1 for the NUL)

	    --p;
	}
	లేకపోతే
	{
	    ఒకవేళ (*p == '\\' && p[1])		// skip escaped characters
		++p;
	    ఒకవేళ (has_mbyte)
		p += (*mb_ptr2len)(p) - 1;
	}
    }

    ఒకవేళ (error)
    {
	ఒకవేళ (newsub != ఆధారం)
	    vim_free(newsub);
	తిరిగి ఆధారం;
    }

    // Store a copy of newsub  in reg_prev_sub.  It is always allocated,
    // because recursive calls may make the returned string invalid.
    // Only store it if there something to store.
    newsublen = p - newsub;
    ఒకవేళ (newsublen == 0)
	VIM_CLEAR(reg_prev_sub);
    లేకపోతే
    {
	vim_free(reg_prev_sub);
	reg_prev_sub = vim_strnsave(newsub, newsublen);
    }

    ఒకవేళ (reg_prev_sub == ఖాళీ)
	reg_prev_sublen = 0;
    లేకపోతే
	reg_prev_sublen = newsublen;

    తిరిగి newsub;
}

#నిర్వచితమైతే FEAT_EVAL

/*
 * Put the submatches in "argv[argskip]" which is a list passed into
 * call_func() by vim_regsub_both().
 */
    స్థిర పూర్ణాంకం
fill_submatch_list(పూర్ణాంకం argc వాడని, typval_T *argv, పూర్ణాంకం argskip, ufunc_T *fp)
{
    listitem_T	*li;
    పూర్ణాంకం		i;
    char_u	*s;
    typval_T	*listarg = argv + argskip;

    ఒకవేళ (!has_varargs(fp) && fp->uf_వాదనలు.ga_len <= argskip)
	// called function doesn't take a submatches argument
	తిరిగి argskip;

    // Relies on sl_list to be the first item in staticList10_T.
    init_static_list((staticList10_T *)(listarg->vval.v_list));

    // There are always 10 list items in staticList10_T.
    li = listarg->vval.v_list->lv_మొదటి;
    కొరకు (i = 0; i < 10; ++i)
    {
	s = rsm.sm_సరిపోలిక->startp[i];
	ఒకవేళ (s == ఖాళీ || rsm.sm_సరిపోలిక->endp[i] == ఖాళీ)
	    s = ఖాళీ;
	లేకపోతే
	    s = vim_strnsave(s, rsm.sm_సరిపోలిక->endp[i] - s);
	li->li_tv.v_type = VAR_STRING;
	li->li_tv.vval.v_string = s;
	li = li->li_తదుపరి;
    }
    తిరిగి argskip + 1;
}

    స్థిర శూన్య
clear_submatch_list(staticList10_T *sl)
{
    పూర్ణాంకం i;

    కొరకు (i = 0; i < 10; ++i)
	vim_free(sl->sl_వస్తువులు[i].li_tv.vval.v_string);
}
#అయితే_ముగింపు

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
    పూర్ణాంకం
vim_regsub(
    regmatch_T	*rmp,
    char_u	*ఆధారం,
    typval_T	*సమాసం,
    char_u	*dest,
    పూర్ణాంకం		destlen,
    పూర్ణాంకం		flags)
{
    పూర్ణాంకం		ఫలితం;
    regexec_T	rex_save;
    పూర్ణాంకం		rex_లోపల_use_save = rex_లోపల_use;

    ఒకవేళ (rex_లోపల_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_లోపల_use = TRUE;

    rex.reg_సరిపోలిక = rmp;
    rex.reg_mmatch = ఖాళీ;
    rex.reg_maxline = 0;
    rex.reg_buf = curbuf;
    rex.reg_పంక్తి_lbr = TRUE;
    ఫలితం = vim_regsub_both(ఆధారం, సమాసం, dest, destlen, flags);

    rex_లోపల_use = rex_లోపల_use_save;
    ఒకవేళ (rex_లోపల_use)
	rex = rex_save;

    తిరిగి ఫలితం;
}

    పూర్ణాంకం
vim_regsub_multi(
    regmmatch_T	*rmp,
    linenr_T	lnum,
    char_u	*ఆధారం,
    char_u	*dest,
    పూర్ణాంకం		destlen,
    పూర్ణాంకం		flags)
{
    పూర్ణాంకం		ఫలితం;
    regexec_T	rex_save;
    పూర్ణాంకం		rex_లోపల_use_save = rex_లోపల_use;

    ఒకవేళ (rex_లోపల_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_లోపల_use = TRUE;

    rex.reg_సరిపోలిక = ఖాళీ;
    rex.reg_mmatch = rmp;
    rex.reg_buf = curbuf;	// always works on the current buffer!
    rex.reg_firstlnum = lnum;
    rex.reg_maxline = curbuf->b_ml.ml_పంక్తి_లెక్క - lnum;
    rex.reg_పంక్తి_lbr = FALSE;
    ఫలితం = vim_regsub_both(ఆధారం, ఖాళీ, dest, destlen, flags);

    rex_లోపల_use = rex_లోపల_use_save;
    ఒకవేళ (rex_లోపల_use)
	rex = rex_save;

    తిరిగి ఫలితం;
}

#ఒకవేళ defined(FEAT_EVAL)
// When nesting more than a couple levels it's probably a mistake.
# define MAX_REGSUB_NESTING 4
స్థిర char_u   *eval_ఫలితం[MAX_REGSUB_NESTING] = {ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ};

# ఒకవేళ defined(EXITFREE)
    శూన్య
free_resub_eval_ఫలితం(శూన్య)
{
    పూర్ణాంకం i;

    కొరకు (i = 0; i < MAX_REGSUB_NESTING; ++i)
	VIM_CLEAR(eval_ఫలితం[i]);
}
# endif
#అయితే_ముగింపు

    స్థిర పూర్ణాంకం
vim_regsub_both(
    char_u	*ఆధారం,
    typval_T	*సమాసం,
    char_u	*dest,
    పూర్ణాంకం		destlen,
    పూర్ణాంకం		flags)
{
    char_u	*src;
    char_u	*dst;
    char_u	*s;
    పూర్ణాంకం		c;
    పూర్ణాంకం		cc;
    పూర్ణాంకం		no = -1;
    fptr_T	func_all = (fptr_T)ఖాళీ;
    fptr_T	func_one = (fptr_T)ఖాళీ;
    linenr_T	clnum = 0;	// init for GCC
    పూర్ణాంకం		len = 0;	// init for GCC
#నిర్వచితమైతే FEAT_EVAL
    స్థిర పూర్ణాంకం  nesting = 0;
    పూర్ణాంకం		nested;
#అయితే_ముగింపు
    పూర్ణాంకం		copy = flags & REGSUB_COPY;

    // Be paranoid...
    ఒకవేళ ((ఆధారం == ఖాళీ && సమాసం == ఖాళీ) || dest == ఖాళీ)
    {
	iemsg(e_null_వాదం);
	తిరిగి 0;
    }
    ఒకవేళ (prog_magic_wrong())
	తిరిగి 0;
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (nesting == MAX_REGSUB_NESTING)
    {
	emsg(_(e_substitute_nesting_too_deep));
	తిరిగి 0;
    }
    nested = nesting;
#అయితే_ముగింపు
    src = ఆధారం;
    dst = dest;

    /*
     * When the substitute part starts with "\=" evaluate it as an expression.
     */
    ఒకవేళ (సమాసం != ఖాళీ || (ఆధారం[0] == '\\' && ఆధారం[1] == '='))
    {
#నిర్వచితమైతే FEAT_EVAL
	// To make sure that the length doesn't change between checking the
	// length and copying the string, and to speed up things, the
	// resulting string is saved from the call with
	// "flags & REGSUB_COPY" == 0 to the call with
	// "flags & REGSUB_COPY" != 0.
	ఒకవేళ (copy)
	{
	    ఒకవేళ (eval_ఫలితం[nested] != ఖాళీ)
	    {
		పూర్ణాంకం eval_len = (పూర్ణాంకం)STRLEN(eval_ఫలితం[nested]);

		ఒకవేళ (eval_len < destlen)
		{
		    STRCPY(dest, eval_ఫలితం[nested]);
		    dst += eval_len;
		    VIM_CLEAR(eval_ఫలితం[nested]);
		}
	    }
	}
	లేకపోతే
	{
	    పూర్ణాంకం		    prev_can_f_submatch = can_f_submatch;
	    regsubmatch_T   rsm_save;

	    VIM_CLEAR(eval_ఫలితం[nested]);

	    // The expression may contain substitute(), which calls us
	    // recursively.  Make sure submatch() gets the text from the first
	    // level.
	    ఒకవేళ (can_f_submatch)
		rsm_save = rsm;
	    can_f_submatch = TRUE;
	    rsm.sm_సరిపోలిక = rex.reg_సరిపోలిక;
	    rsm.sm_mmatch = rex.reg_mmatch;
	    rsm.sm_firstlnum = rex.reg_firstlnum;
	    rsm.sm_maxline = rex.reg_maxline;
	    rsm.sm_పంక్తి_lbr = rex.reg_పంక్తి_lbr;

	    // Although unlikely, it is possible that the expression invokes a
	    // substitute command (it might fail, but still).  Therefore keep
	    // an array of eval results.
	    ++nesting;

	    ఒకవేళ (సమాసం != ఖాళీ)
	    {
		typval_T	argv[2];
		char_u		buf[NUMBUFLEN];
		typval_T	rettv;
		staticList10_T	సరిపోలిక_List;
		funcexe_T	funcexe;

		rettv.v_type = VAR_STRING;
		rettv.vval.v_string = ఖాళీ;
		argv[0].v_type = VAR_LIST;
		argv[0].vval.v_list = &సరిపోలిక_List.sl_list;
		సరిపోలిక_List.sl_list.lv_len = 0;
		CLEAR_క్షేత్రం(funcexe);
		funcexe.fe_argv_func = fill_submatch_list;
		funcexe.fe_evaluate = TRUE;
		ఒకవేళ (సమాసం->v_type == VAR_FUNC)
		{
		    s = సమాసం->vval.v_string;
		    call_func(s, -1, &rettv, 1, argv, &funcexe);
		}
		లేకపోతే ఒకవేళ (సమాసం->v_type == VAR_PARTIAL)
		{
		    partial_T   *partial = సమాసం->vval.v_partial;

		    s = partial_name(partial);
		    funcexe.fe_partial = partial;
		    call_func(s, -1, &rettv, 1, argv, &funcexe);
		}
		లేకపోతే ఒకవేళ (సమాసం->v_type == VAR_INSTR)
		{
		    exe_typval_instr(సమాసం, &rettv);
		}
		ఒకవేళ (సరిపోలిక_List.sl_list.lv_len > 0)
		    // fill_submatch_list() was called
		    clear_submatch_list(&సరిపోలిక_List);

		ఒకవేళ (rettv.v_type == VAR_UNKNOWN)
		    // something failed, no need to report another error
		    eval_ఫలితం[nested] = ఖాళీ;
		లేకపోతే
		{
		    eval_ఫలితం[nested] = tv_get_string_buf_chk(&rettv, buf);
		    ఒకవేళ (eval_ఫలితం[nested] != ఖాళీ)
			eval_ఫలితం[nested] = vim_strsave(eval_ఫలితం[nested]);
		}
		clear_tv(&rettv);
	    }
	    లేకపోతే ఒకవేళ (substitute_instr != ఖాళీ)
		// Execute instructions from ISN_SUBSTITUTE.
		eval_ఫలితం[nested] = exe_substitute_instr();
	    లేకపోతే
		eval_ఫలితం[nested] = eval_గమ్యం_string(ఆధారం + 2, TRUE, FALSE);
	    --nesting;

	    ఒకవేళ (eval_ఫలితం[nested] != ఖాళీ)
	    {
		పూర్ణాంకం had_backslash = FALSE;

		కొరకు (s = eval_ఫలితం[nested]; *s != NUL; MB_PTR_ADV(s))
		{
		    // Change NL to CR, so that it becomes a line break,
		    // unless called from vim_regexec_nl().
		    // Skip over a backslashed character.
		    ఒకవేళ (*s == NL && !rsm.sm_పంక్తి_lbr)
			*s = CAR;
		    లేకపోతే ఒకవేళ (*s == '\\' && s[1] != NUL)
		    {
			++s;
			/* Change NL to CR here too, so that this works:
			 * :s/abc\\\ndef/\="aaa\\\nbbb"/  on text:
			 *   abc\
			 *   def
			 * Not when called from vim_regexec_nl().
			 */
			ఒకవేళ (*s == NL && !rsm.sm_పంక్తి_lbr)
			    *s = CAR;
			had_backslash = TRUE;
		    }
		}
		ఒకవేళ (had_backslash && (flags & REGSUB_BACKSLASH))
		{
		    // Backslashes will be consumed, need to double them.
		    s = vim_strsave_escaped(eval_ఫలితం[nested], (char_u *)"\\");
		    ఒకవేళ (s != ఖాళీ)
		    {
			vim_free(eval_ఫలితం[nested]);
			eval_ఫలితం[nested] = s;
		    }
		}

		dst += STRLEN(eval_ఫలితం[nested]);
	    }

	    can_f_submatch = prev_can_f_submatch;
	    ఒకవేళ (can_f_submatch)
		rsm = rsm_save;
	}
#అయితే_ముగింపు
    }
    లేకపోతే
      అయితే ((c = *src++) != NUL)
      {
	ఒకవేళ (c == '&' && (flags & REGSUB_MAGIC))
	    no = 0;
	లేకపోతే ఒకవేళ (c == '\\' && *src != NUL)
	{
	    ఒకవేళ (*src == '&' && !(flags & REGSUB_MAGIC))
	    {
		++src;
		no = 0;
	    }
	    లేకపోతే ఒకవేళ ('0' <= *src && *src <= '9')
	    {
		no = *src++ - '0';
	    }
	    లేకపోతే ఒకవేళ (vim_strchr((char_u *)"uUlLeE", *src))
	    {
		ఎంపిక (*src++)
		{
		స్థితి 'u':   func_one = do_పై;
			    కొనసాగు;
		స్థితి 'U':   func_all = do_పై;
			    కొనసాగు;
		స్థితి 'l':   func_one = do_క్రింది;
			    కొనసాగు;
		స్థితి 'L':   func_all = do_క్రింది;
			    కొనసాగు;
		స్థితి 'e':
		స్థితి 'E':   func_one = func_all = (fptr_T)ఖాళీ;
			    కొనసాగు;
		}
	    }
	}
	ఒకవేళ (no < 0)	      // Ordinary character.
	{
	    ఒకవేళ (c == K_SPECIAL && src[0] != NUL && src[1] != NUL)
	    {
		// Copy a special key as-is.
		ఒకవేళ (copy)
		{
		    ఒకవేళ (dst + 3 > dest + destlen)
		    {
			iemsg("vim_regsub_both(): not enough space");
			తిరిగి 0;
		    }
		    *dst++ = c;
		    *dst++ = *src++;
		    *dst++ = *src++;
		}
		లేకపోతే
		{
		    dst += 3;
		    src += 2;
		}
		కొనసాగు;
	    }

	    ఒకవేళ (c == '\\' && *src != NUL)
	    {
		// Check for abbreviations -- webb
		ఎంపిక (*src)
		{
		    స్థితి 'r':	c = CAR;	++src;	ఆపు;
		    స్థితి 'n':	c = NL;		++src;	ఆపు;
		    స్థితి 't':	c = TAB;	++src;	ఆపు;
		 // Oh no!  \e already has meaning in subst pat :-(
		 // case 'e':   c = ESC;	++src;	break;
		    స్థితి 'b':	c = Ctrl_H;	++src;	ఆపు;

		    // If "backslash" is TRUE the backslash will be removed
		    // later.  Used to insert a literal CR.
		    అప్రమేయ:	ఒకవేళ (flags & REGSUB_BACKSLASH)
				{
				    ఒకవేళ (copy)
				    {
					ఒకవేళ (dst + 1 > dest + destlen)
					{
					    iemsg("vim_regsub_both(): not enough space");
					    తిరిగి 0;
					}
					*dst = '\\';
				    }
				    ++dst;
				}
				c = *src++;
		}
	    }
	    లేకపోతే ఒకవేళ (has_mbyte)
		c = mb_ptr2char(src - 1);

	    // Write to buffer, if copy is set.
	    ఒకవేళ (func_one != (fptr_T)ఖాళీ)
	    {
		func_one(&cc, c);
		func_one = ఖాళీ;
	    }
	    లేకపోతే ఒకవేళ (func_all != (fptr_T)ఖాళీ)
		func_all(&cc, c);
	    లేకపోతే // just copy
		cc = c;

	    ఒకవేళ (has_mbyte)
	    {
		పూర్ణాంకం totlen = mb_ptr2len(src - 1);
		పూర్ణాంకం charlen = mb_char2len(cc);

		ఒకవేళ (copy)
		{
		    ఒకవేళ (dst + charlen > dest + destlen)
		    {
			iemsg("vim_regsub_both(): not enough space");
			తిరిగి 0;
		    }
		    mb_char2bytes(cc, dst);
		}
		dst += charlen - 1;
		ఒకవేళ (enc_utf8)
		{
		    పూర్ణాంకం clen = utf_ptr2len(src - 1);

		    // If the character length is shorter than "totlen", there
		    // are composing characters; copy them as-is.
		    ఒకవేళ (clen < totlen)
		    {
			ఒకవేళ (copy)
			{
			    ఒకవేళ (dst + totlen - clen > dest + destlen)
			    {
				iemsg("vim_regsub_both(): not enough space");
				తిరిగి 0;
			    }
			    mch_memmove(dst + 1, src - 1 + clen,
						     (size_t)(totlen - clen));
			}
			dst += totlen - clen;
		    }
		}
		src += totlen - 1;
	    }
	    లేకపోతే ఒకవేళ (copy)
	    {
		ఒకవేళ (dst + 1 > dest + destlen)
		{
		    iemsg("vim_regsub_both(): not enough space");
		    తిరిగి 0;
		}
		*dst = cc;
	    }
	    dst++;
	}
	లేకపోతే
	{
	    ఒకవేళ (REG_MULTI)
	    {
		clnum = rex.reg_mmatch->startpos[no].lnum;
		ఒకవేళ (clnum < 0 || rex.reg_mmatch->endpos[no].lnum < 0)
		    s = ఖాళీ;
		లేకపోతే
		{
		    s = reg_getline(clnum) + rex.reg_mmatch->startpos[no].నిలువు;
		    ఒకవేళ (rex.reg_mmatch->endpos[no].lnum == clnum)
			len = rex.reg_mmatch->endpos[no].నిలువు
					    - rex.reg_mmatch->startpos[no].నిలువు;
		    లేకపోతే
			len = (పూర్ణాంకం)reg_getline_len(clnum) - rex.reg_mmatch->startpos[no].నిలువు;
		}
	    }
	    లేకపోతే
	    {
		s = rex.reg_సరిపోలిక->startp[no];
		ఒకవేళ (rex.reg_సరిపోలిక->endp[no] == ఖాళీ)
		    s = ఖాళీ;
		లేకపోతే
		    len = (పూర్ణాంకం)(rex.reg_సరిపోలిక->endp[no] - s);
	    }
	    ఒకవేళ (s != ఖాళీ)
	    {
		కొరకు (;;)
		{
		    ఒకవేళ (len == 0)
		    {
			ఒకవేళ (REG_MULTI)
			{
			    ఒకవేళ (rex.reg_mmatch->endpos[no].lnum == clnum)
				ఆపు;
			    ఒకవేళ (copy)
			    {
				ఒకవేళ (dst + 1 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    తిరిగి 0;
				}
				*dst = CAR;
			    }
			    ++dst;
			    s = reg_getline(++clnum);
			    ఒకవేళ (rex.reg_mmatch->endpos[no].lnum == clnum)
				len = rex.reg_mmatch->endpos[no].నిలువు;
			    లేకపోతే
				len = (పూర్ణాంకం)reg_getline_len(clnum);
			}
			లేకపోతే
			    ఆపు;
		    }
		    లేకపోతే ఒకవేళ (*s == NUL) // we hit NUL.
		    {
			ఒకవేళ (copy)
			    iemsg(e_damaged_సరిపోలిక_string);
			వెళ్ళు నిష్క్రమించు;
		    }
		    లేకపోతే
		    {
			ఒకవేళ ((flags & REGSUB_BACKSLASH)
						  && (*s == CAR || *s == '\\'))
			{
			    /*
			     * Insert a backslash in front of a CR, otherwise
			     * it will be replaced by a line break.
			     * Number of backslashes will be halved later,
			     * double them here.
			     */
			    ఒకవేళ (copy)
			    {
				ఒకవేళ (dst + 2 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    తిరిగి 0;
				}
				dst[0] = '\\';
				dst[1] = *s;
			    }
			    dst += 2;
			}
			లేకపోతే
			{
			    ఒకవేళ (has_mbyte)
				c = mb_ptr2char(s);
			    లేకపోతే
				c = *s;

			    ఒకవేళ (func_one != (fptr_T)ఖాళీ)
			    {
				func_one(&cc, c);
				func_one = ఖాళీ;
			    }
			    లేకపోతే ఒకవేళ (func_all != (fptr_T)ఖాళీ)
				func_all(&cc, c);
			    లేకపోతే // just copy
				cc = c;

			    ఒకవేళ (has_mbyte)
			    {
				పూర్ణాంకం l;
				పూర్ణాంకం charlen;

				// Copy composing characters separately, one
				// at a time.
				ఒకవేళ (enc_utf8)
				    l = utf_ptr2len(s) - 1;
				లేకపోతే
				    l = mb_ptr2len(s) - 1;

				s += l;
				len -= l;
				charlen = mb_char2len(cc);
				ఒకవేళ (copy)
				{
				    ఒకవేళ (dst + charlen > dest + destlen)
				    {
					iemsg("vim_regsub_both(): not enough space");
					తిరిగి 0;
				    }
				    mb_char2bytes(cc, dst);
				}
				dst += charlen - 1;
			    }
			    లేకపోతే ఒకవేళ (copy)
			    {
				ఒకవేళ (dst + 1 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    తిరిగి 0;
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
    ఒకవేళ (copy)
	*dst = NUL;

నిష్క్రమించు:
    తిరిగి (పూర్ణాంకం)((dst - dest) + 1);
}

#నిర్వచితమైతే FEAT_EVAL

    స్థిర char_u *
reg_getline_submatch(linenr_T lnum)
{
    char_u *పంక్తి;

    reg_getline_common(lnum, RGLF_పంక్తి | RGLF_SUBMATCH, &పంక్తి, ఖాళీ);

    తిరిగి పంక్తి;
}

    స్థిర colnr_T
reg_getline_submatch_len(linenr_T lnum)
{
    colnr_T నిడివి;

    reg_getline_common(lnum, RGLF_నిడివి | RGLF_SUBMATCH, ఖాళీ, &నిడివి);

    తిరిగి నిడివి;
}

/*
 * Used for the submatch() function: get the string from the n'th submatch in
 * allocated memory.
 * Returns NULL when not in a ":s" command and for a non-existing submatch.
 */
    char_u *
reg_submatch(పూర్ణాంకం no)
{
    char_u	*retval = ఖాళీ;
    char_u	*s;
    పూర్ణాంకం		len;
    పూర్ణాంకం		విడత;
    linenr_T	lnum;

    ఒకవేళ (!can_f_submatch || no < 0)
	తిరిగి ఖాళీ;

    ఒకవేళ (rsm.sm_సరిపోలిక == ఖాళీ)
    {
	/*
	 * First round: compute the length and allocate memory.
	 * Second round: copy the text.
	 */
	కొరకు (విడత = 1; విడత <= 2; ++విడత)
	{
	    lnum = rsm.sm_mmatch->startpos[no].lnum;
	    ఒకవేళ (lnum < 0 || rsm.sm_mmatch->endpos[no].lnum < 0)
		తిరిగి ఖాళీ;

	    s = reg_getline_submatch(lnum);
	    ఒకవేళ (s == ఖాళీ)  // anti-crash check, cannot happen?
		ఆపు;
	    s += rsm.sm_mmatch->startpos[no].నిలువు;
	    ఒకవేళ (rsm.sm_mmatch->endpos[no].lnum == lnum)
	    {
		// Within one line: take form start to end col.
		len = rsm.sm_mmatch->endpos[no].నిలువు
					  - rsm.sm_mmatch->startpos[no].నిలువు;
		ఒకవేళ (విడత == 2)
		    vim_strncpy(retval, s, len);
		++len;
	    }
	    లేకపోతే
	    {
		// Multiple lines: take start line from start col, middle
		// lines completely and end line up to end col.
		len = (పూర్ణాంకం)reg_getline_submatch_len(lnum) - rsm.sm_mmatch->startpos[no].నిలువు;
		ఒకవేళ (విడత == 2)
		{
		    STRCPY(retval, s);
		    retval[len] = '\n';
		}
		++len;
		++lnum;
		అయితే (lnum < rsm.sm_mmatch->endpos[no].lnum)
		{
		    s = reg_getline_submatch(lnum);
		    ఒకవేళ (విడత == 2)
			STRCPY(retval + len, s);
		    len += (పూర్ణాంకం)reg_getline_submatch_len(lnum);
		    ఒకవేళ (విడత == 2)
			retval[len] = '\n';
		    ++len;
		    ++lnum;
		}
		ఒకవేళ (విడత == 2)
		    STRNCPY(retval + len, reg_getline_submatch(lnum),
					     rsm.sm_mmatch->endpos[no].నిలువు);
		len += rsm.sm_mmatch->endpos[no].నిలువు;
		ఒకవేళ (విడత == 2)
		    retval[len] = NUL;
		++len;
	    }

	    ఒకవేళ (retval == ఖాళీ)
	    {
		retval = alloc(len);
		ఒకవేళ (retval == ఖాళీ)
		    తిరిగి ఖాళీ;
	    }
	}
    }
    లేకపోతే
    {
	s = rsm.sm_సరిపోలిక->startp[no];
	ఒకవేళ (s == ఖాళీ || rsm.sm_సరిపోలిక->endp[no] == ఖాళీ)
	    retval = ఖాళీ;
	లేకపోతే
	    retval = vim_strnsave(s, rsm.sm_సరిపోలిక->endp[no] - s);
    }

    తిరిగి retval;
}

/*
 * Used for the submatch() function with the optional non-zero argument: get
 * the list of strings from the n'th submatch in allocated memory with NULs
 * represented in NLs.
 * Returns a list of allocated strings.  Returns NULL when not in a ":s"
 * command, for a non-existing submatch and for any error.
 */
    list_T *
reg_submatch_list(పూర్ణాంకం no)
{
    char_u	*s;
    linenr_T	slnum;
    linenr_T	elnum;
    colnr_T	scol;
    colnr_T	ecol;
    పూర్ణాంకం		i;
    list_T	*list;
    పూర్ణాంకం		error = FALSE;

    ఒకవేళ (!can_f_submatch || no < 0)
	తిరిగి ఖాళీ;

    ఒకవేళ (rsm.sm_సరిపోలిక == ఖాళీ)
    {
	slnum = rsm.sm_mmatch->startpos[no].lnum;
	elnum = rsm.sm_mmatch->endpos[no].lnum;
	ఒకవేళ (slnum < 0 || elnum < 0)
	    తిరిగి ఖాళీ;

	scol = rsm.sm_mmatch->startpos[no].నిలువు;
	ecol = rsm.sm_mmatch->endpos[no].నిలువు;

	list = list_alloc();
	ఒకవేళ (list == ఖాళీ)
	    తిరిగి ఖాళీ;

	s = reg_getline_submatch(slnum) + scol;
	ఒకవేళ (slnum == elnum)
	{
	    ఒకవేళ (list_append_string(list, s, ecol - scol) == FAIL)
		error = TRUE;
	}
	లేకపోతే
	{
	    పూర్ణాంకం max_lnum = elnum - slnum;

	    ఒకవేళ (list_append_string(list, s, -1) == FAIL)
		error = TRUE;
	    కొరకు (i = 1; i < max_lnum; i++)
	    {
		s = reg_getline_submatch(slnum + i);
		ఒకవేళ (list_append_string(list, s, -1) == FAIL)
		    error = TRUE;
	    }
	    s = reg_getline_submatch(elnum);
	    ఒకవేళ (list_append_string(list, s, ecol) == FAIL)
		error = TRUE;
	}
    }
    లేకపోతే
    {
	s = rsm.sm_సరిపోలిక->startp[no];
	ఒకవేళ (s == ఖాళీ || rsm.sm_సరిపోలిక->endp[no] == ఖాళీ)
	    తిరిగి ఖాళీ;
	list = list_alloc();
	ఒకవేళ (list == ఖాళీ)
	    తిరిగి ఖాళీ;
	ఒకవేళ (list_append_string(list, s,
				 (పూర్ణాంకం)(rsm.sm_సరిపోలిక->endp[no] - s)) == FAIL)
	    error = TRUE;
    }

    ఒకవేళ (error)
    {
	list_free(list);
	తిరిగి ఖాళీ;
    }
    ++list->lv_refcount;
    తిరిగి list;
}
#అయితే_ముగింపు

/*
 * Initialize the values used for matching against multiple lines
 */
    స్థిర శూన్య
init_regexec_multi(
	regmmatch_T	*rmp,
	win_T		*win,	// window in which to search or NULL
	buf_T		*buf,	// buffer in which to search
	linenr_T	lnum)	// nr of line to start looking for match
{
    rex.reg_సరిపోలిక = ఖాళీ;
    rex.reg_mmatch = rmp;
    rex.reg_buf = buf;
    rex.reg_win = win;
    rex.reg_firstlnum = lnum;
    rex.reg_maxline = rex.reg_buf->b_ml.ml_పంక్తి_లెక్క - lnum;
    rex.reg_పంక్తి_lbr = FALSE;
    rex.reg_ic = rmp->rmm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = rmp->rmm_maxcol;
}

#నిర్వచించు CLASS_not		0x80
#నిర్వచించు CLASS_af		0x40
#నిర్వచించు CLASS_AF		0x20
#నిర్వచించు CLASS_az		0x10
#నిర్వచించు CLASS_AZ		0x08
#నిర్వచించు CLASS_o7		0x04
#నిర్వచించు CLASS_o9		0x02
#నిర్వచించు CLASS_underscore	0x01

/*
 * Parse the "[...]" collection between "start" (the first character after the
 * "[") and "end" (the "]") into a combination of CLASS_ bits, so that it can
 * be turned into a faster character class like \d or \x.  Sets "*newl" to TRUE
 * when the collection also matches a newline.  Returns -1 when the collection
 * is not a plain combination of the recognized ranges.
 */
    స్థిర పూర్ణాంకం
get_char_class_బిట్లు(char_u *ప్రారంభం, char_u *చివర, పూర్ణాంకం *newl)
{
    char_u	*p = ప్రారంభం;
    పూర్ణాంకం		config = 0;

    *newl = FALSE;
    ఒకవేళ (*చివర != ']')
	తిరిగి -1;
    ఒకవేళ (*p == '^')
    {
	config |= CLASS_not;
	p++;
    }

    అయితే (p < చివర)
    {
	ఒకవేళ (p + 2 < చివర && *(p + 1) == '-')
	{
	    ఎంపిక (*p)
	    {
		స్థితి '0':
		    ఒకవేళ (*(p + 2) == '9')
		    {
			config |= CLASS_o9;
			ఆపు;
		    }
		    ఒకవేళ (*(p + 2) == '7')
		    {
			config |= CLASS_o7;
			ఆపు;
		    }
		    తిరిగి -1;

		స్థితి 'a':
		    ఒకవేళ (*(p + 2) == 'z')
		    {
			config |= CLASS_az;
			ఆపు;
		    }
		    ఒకవేళ (*(p + 2) == 'f')
		    {
			config |= CLASS_af;
			ఆపు;
		    }
		    తిరిగి -1;

		స్థితి 'A':
		    ఒకవేళ (*(p + 2) == 'Z')
		    {
			config |= CLASS_AZ;
			ఆపు;
		    }
		    ఒకవేళ (*(p + 2) == 'F')
		    {
			config |= CLASS_AF;
			ఆపు;
		    }
		    తిరిగి -1;

		అప్రమేయ:
		    తిరిగి -1;
	    }
	    p += 3;
	}
	లేకపోతే ఒకవేళ (p + 1 < చివర && *p == '\\' && *(p + 1) == 'n')
	{
	    *newl = TRUE;
	    p += 2;
	}
	లేకపోతే ఒకవేళ (*p == '_')
	{
	    config |= CLASS_underscore;
	    p++;
	}
	లేకపోతే ఒకవేళ (*p == '\n')
	{
	    *newl = TRUE;
	    p++;
	}
	లేకపోతే
	    తిరిగి -1;
    } // while (p < end)

    ఒకవేళ (p != చివర)
	తిరిగి -1;

    // U+017F and U+212A fold to ASCII 's' and 'k', missed by the class opcodes
    ఒకవేళ (enc_utf8 && (config & (CLASS_az | CLASS_AZ)))
	తిరిగి -1;

    తిరిగి config;
}

#అన్తర్భావ "regexp_bt.c"

స్థిర regengine_T bt_regengine =
{
    bt_regcomp,
    bt_regfree,
    bt_regexec_nl,
    bt_regexec_multi
#నిర్వచితమైతే DEBUG
    ,(char_u *)""
#అయితే_ముగింపు
};

#అన్తర్భావ "regexp_nfa.c"

స్థిర regengine_T nfa_regengine =
{
    nfa_regcomp,
    nfa_regfree,
    nfa_regexec_nl,
    nfa_regexec_multi
#నిర్వచితమైతే DEBUG
    ,(char_u *)""
#అయితే_ముగింపు
};

// Which regexp engine to use? Needed for vim_regcomp().
// Must match with 'regexpengine'.
స్థిర పూర్ణాంకం regexp_engine = 0;

#నిర్వచితమైతే DEBUG
స్థిర స్థిరరాశి char_u regname[][30] = {
		    "AUTOMATIC Regexp Engine",
		    "BACKTRACKING Regexp Engine",
		    "NFA Regexp Engine"
			    };
#అయితే_ముగింపు

/*
 * Compile a regular expression into internal code.
 * Returns the program in allocated memory.
 * Use vim_regfree() to free the memory.
 * Returns NULL for an error.
 */
    regprog_T *
vim_regcomp(char_u *సమాసం_arg, పూర్ణాంకం re_flags)
{
    regprog_T   *prog = ఖాళీ;
    char_u	*సమాసం = సమాసం_arg;
    పూర్ణాంకం		called_emsg_before;

    regexp_engine = p_re;

    // Check for prefix "\%#=", that sets the regexp engine
    ఒకవేళ (STRNCMP(సమాసం, "\\%#=", 4) == 0)
    {
	పూర్ణాంకం newengine = సమాసం[4] - '0';

	ఒకవేళ (newengine == AUTOMATIC_ENGINE
	    || newengine == BACKTRACKING_ENGINE
	    || newengine == NFA_ENGINE)
	{
	    regexp_engine = సమాసం[4] - '0';
	    సమాసం += 5;
#నిర్వచితమైతే DEBUG
	    smsg("New regexp mode selected (%d): %s",
					   regexp_engine, regname[newengine]);
#అయితే_ముగింపు
	}
	లేకపోతే
	{
	    emsg(_(e_percent_hash_can_only_be_followed_by_zero_one_two_automatic_engine_will_be_వాడిన));
	    regexp_engine = AUTOMATIC_ENGINE;
	}
    }
#నిర్వచితమైతే DEBUG
    bt_regengine.సమాసం = సమాసం;
    nfa_regengine.సమాసం = సమాసం;
#అయితే_ముగింపు
    // reg_iswordc() uses rex.reg_buf
    rex.reg_buf = curbuf;

    /*
     * First try the NFA engine, unless backtracking was requested.
     */
    called_emsg_before = called_emsg;
    ఒకవేళ (regexp_engine != BACKTRACKING_ENGINE)
	prog = nfa_regengine.regcomp(సమాసం,
		re_flags + (regexp_engine == AUTOMATIC_ENGINE ? RE_AUTO : 0));
    లేకపోతే
	prog = bt_regengine.regcomp(సమాసం, re_flags);

    // Check for error compiling regexp with initial engine.
    ఒకవేళ (prog == ఖాళీ)
    {
#నిర్వచితమైతే BT_REGEXP_DEBUG_LOG
	ఒకవేళ (regexp_engine == BACKTRACKING_ENGINE)   // debugging log for BT engine
	{
	    FILE *f;
	    f = fopen(BT_REGEXP_DEBUG_LOG_NAME, "a");
	    ఒకవేళ (f)
	    {
		fprintf(f, "Syntax error in \"%s\"\n", సమాసం);
		fclose(f);
	    }
	    లేకపోతే
		semsg("(NFA) Could not open \"%s\" to write !!!",
			BT_REGEXP_DEBUG_LOG_NAME);
	}
#అయితే_ముగింపు
	/*
	 * If the NFA engine failed, try the backtracking engine.
	 * The NFA engine also fails for patterns that it can't handle well
	 * but are still valid patterns, thus a retry should work.
	 * But don't try if an error message was given.
	 */
	ఒకవేళ (regexp_engine == AUTOMATIC_ENGINE
					  && called_emsg == called_emsg_before)
	{
	    regexp_engine = BACKTRACKING_ENGINE;
#నిర్వచితమైతే FEAT_EVAL
	    report_re_switch(సమాసం);
#అయితే_ముగింపు
	    prog = bt_regengine.regcomp(సమాసం, re_flags);
	}
    }

    ఒకవేళ (prog != ఖాళీ)
    {
	// Store the info needed to call regcomp() again when the engine turns
	// out to be very slow when executing it.
	prog->re_engine = regexp_engine;
	prog->re_flags  = re_flags;
    }

    తిరిగి prog;
}

/*
 * Free a compiled regexp program, returned by vim_regcomp().
 */
    శూన్య
vim_regfree(regprog_T *prog)
{
    ఒకవేళ (prog != ఖాళీ)
	prog->engine->regfree(prog);
}

#ఒకవేళ defined(EXITFREE)
    శూన్య
free_regexp_stuff(శూన్య)
{
    ga_clear(&regstack);
    ga_clear(&backpos);
    vim_free(reg_tofree);
    vim_free(reg_prev_sub);
    vim_free(post_ప్రారంభం);   // NFA postfix buffer, reused across compilations
    post_ప్రారంభం = ఖాళీ;
    post_ప్రారంభం_len = 0;
    vim_free(nfa_దొంతర);    // NFA fragment stack, reused across compilations
    nfa_దొంతర = ఖాళీ;
    nfa_దొంతర_len = 0;
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_EVAL
    స్థిర శూన్య
report_re_switch(char_u *pat)
{
    ఒకవేళ (p_verbose > 0)
    {
	verbose_enter();
	సందేశం_puts(_("Switching to backtracking RE engine for pattern: "));
	సందేశం_puts((అక్షరం *)pat);
	verbose_leave();
    }
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_X11)
/*
 * Return whether "prog" is currently being executed.
 */
    పూర్ణాంకం
regprog_లోపల_use(regprog_T *prog)
{
    తిరిగి prog->re_లోపల_use;
}
#అయితే_ముగింపు

/*
 * Match a regexp against a string.
 * "rmp->regprog" must be a compiled regexp as returned by vim_regcomp().
 * Note: "rmp->regprog" may be freed and changed.
 * Uses curbuf for line count and 'iskeyword'.
 * When "nl" is TRUE consider a "\n" in "line" to be a line break.
 *
 * Return TRUE if there is a match, FALSE if not.
 */
    స్థిర పూర్ణాంకం
vim_regexec_string(
    regmatch_T	*rmp,
    char_u	*పంక్తి,  // string to match against
    colnr_T	నిలువు,    // column to start looking for match
    పూర్ణాంకం		nl)
{
    పూర్ణాంకం		ఫలితం;
    regexec_T	rex_save;
    పూర్ణాంకం		rex_లోపల_use_save = rex_లోపల_use;

    // Cannot use the same prog recursively, it contains state.
    ఒకవేళ (rmp->regprog->re_లోపల_use)
    {
	emsg(_(e_cannot_use_నమూనా_recursively));
	తిరిగి FALSE;
    }
    rmp->regprog->re_లోపల_use = TRUE;

    ఒకవేళ (rex_లోపల_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_లోపల_use = TRUE;

    rex.reg_startp = ఖాళీ;
    rex.reg_endp = ఖాళీ;
    rex.reg_startpos = ఖాళీ;
    rex.reg_endpos = ఖాళీ;

    ఫలితం = rmp->regprog->engine->regexec_nl(rmp, పంక్తి, నిలువు, nl);
    rmp->regprog->re_లోపల_use = FALSE;

    // NFA engine aborted because it's very slow.
    ఒకవేళ (rmp->regprog->re_engine == AUTOMATIC_ENGINE
					       && ఫలితం == NFA_TOO_EXPENSIVE)
    {
	పూర్ణాంకం    save_p_re = p_re;
	పూర్ణాంకం    re_flags = rmp->regprog->re_flags;
	char_u *pat = vim_strsave(((nfa_regprog_T *)rmp->regprog)->నమూనా);

	p_re = BACKTRACKING_ENGINE;
	ఒకవేళ (pat != ఖాళీ)
	{
	    regprog_T *prev_prog = rmp->regprog;

#నిర్వచితమైతే FEAT_EVAL
	    report_re_switch(pat);
#అయితే_ముగింపు
	    rmp->regprog = vim_regcomp(pat, re_flags);
	    ఒకవేళ (rmp->regprog == ఖాళీ)
	    {
		// Somehow compiling the pattern failed now, put back the
		// previous one to avoid "regprog" becoming NULL.
		rmp->regprog = prev_prog;
	    }
	    లేకపోతే
	    {
		vim_regfree(prev_prog);
		rmp->regprog->re_లోపల_use = TRUE;
		ఫలితం = rmp->regprog->engine->regexec_nl(rmp, పంక్తి, నిలువు, nl);
		rmp->regprog->re_లోపల_use = FALSE;
	    }
	    vim_free(pat);
	}

	p_re = save_p_re;
    }

    rex_లోపల_use = rex_లోపల_use_save;
    ఒకవేళ (rex_లోపల_use)
	rex = rex_save;

    తిరిగి ఫలితం > 0;
}

#ఒకవేళ defined(FEAT_SPELL) || defined(FEAT_EVAL) || defined(FEAT_X11)
/*
 * Note: "*prog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    పూర్ణాంకం
vim_regexec_prog(
    regprog_T	**prog,
    పూర్ణాంకం		ignore_case,
    char_u	*పంక్తి,
    colnr_T	నిలువు)
{
    పూర్ణాంకం		r;
    regmatch_T	regmatch;

    regmatch.regprog = *prog;
    regmatch.rm_ic = ignore_case;
    r = vim_regexec_string(&regmatch, పంక్తి, నిలువు, FALSE);
    *prog = regmatch.regprog;
    తిరిగి r;
}
#అయితే_ముగింపు

/*
 * Note: "rmp->regprog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    పూర్ణాంకం
vim_regexec(regmatch_T *rmp, char_u *పంక్తి, colnr_T నిలువు)
{
    తిరిగి vim_regexec_string(rmp, పంక్తి, నిలువు, FALSE);
}

/*
 * Like vim_regexec(), but consider a "\n" in "line" to be a line break.
 * Note: "rmp->regprog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    పూర్ణాంకం
vim_regexec_nl(regmatch_T *rmp, char_u *పంక్తి, colnr_T నిలువు)
{
    తిరిగి vim_regexec_string(rmp, పంక్తి, నిలువు, TRUE);
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
    దీర్ఘ
vim_regexec_multi(
    regmmatch_T *rmp,
    win_T       *win,		// window in which to search or NULL
    buf_T       *buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	నిలువు,		// column to start looking for match
    పూర్ణాంకం		*timed_బహిర్గతం)	// flag is set when timeout limit reached
{
    పూర్ణాంకం		ఫలితం;
    regexec_T	rex_save;
    పూర్ణాంకం		rex_లోపల_use_save = rex_లోపల_use;

    // Cannot use the same prog recursively, it contains state.
    ఒకవేళ (rmp->regprog->re_లోపల_use)
    {
	emsg(_(e_cannot_use_నమూనా_recursively));
	తిరిగి FALSE;
    }
    rmp->regprog->re_లోపల_use = TRUE;

    ఒకవేళ (rex_లోపల_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_లోపల_use = TRUE;

    ఫలితం = rmp->regprog->engine->regexec_multi(
				      rmp, win, buf, lnum, నిలువు, timed_బహిర్గతం);
    rmp->regprog->re_లోపల_use = FALSE;

    // NFA engine aborted because it's very slow.
    ఒకవేళ (rmp->regprog->re_engine == AUTOMATIC_ENGINE
					       && ఫలితం == NFA_TOO_EXPENSIVE)
    {
	పూర్ణాంకం    save_p_re = p_re;
	పూర్ణాంకం    re_flags = rmp->regprog->re_flags;
	char_u *pat = vim_strsave(((nfa_regprog_T *)rmp->regprog)->నమూనా);

	p_re = BACKTRACKING_ENGINE;
	ఒకవేళ (pat != ఖాళీ)
	{
	    regprog_T *prev_prog = rmp->regprog;

#నిర్వచితమైతే FEAT_EVAL
	    report_re_switch(pat);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SYN_HL
	    // checking for \z misuse was already done when compiling for NFA,
	    // allow all here
	    reg_do_extmatch = REX_ALL;
#అయితే_ముగింపు
	    rmp->regprog = vim_regcomp(pat, re_flags);
#నిర్వచితమైతే FEAT_SYN_HL
	    reg_do_extmatch = 0;
#అయితే_ముగింపు
	    ఒకవేళ (rmp->regprog == ఖాళీ)
	    {
		// Somehow compiling the pattern failed now, put back the
		// previous one to avoid "regprog" becoming NULL.
		rmp->regprog = prev_prog;
	    }
	    లేకపోతే
	    {
		vim_regfree(prev_prog);

		rmp->regprog->re_లోపల_use = TRUE;
		ఫలితం = rmp->regprog->engine->regexec_multi(
				      rmp, win, buf, lnum, నిలువు, timed_బహిర్గతం);
		rmp->regprog->re_లోపల_use = FALSE;
	    }
	    vim_free(pat);
	}
	p_re = save_p_re;
    }

    rex_లోపల_use = rex_లోపల_use_save;
    ఒకవేళ (rex_లోపల_use)
	rex = rex_save;

    తిరిగి ఫలితం <= 0 ? 0 : ఫలితం;
}
