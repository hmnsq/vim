/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * Handling of regular expressions: vim_regcomp(), vim_regexec(), vim_regsub()
 */

// By default: do not create debugging logs or files related to regular
// expressions, even when compiling with -DDEBUG.
// Uncomment the second line to get the regexp debugging.
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ DEBUG
// #define DEBUG

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
// show/save debugging data when BT engine is used
# define BT_REGEXP_DUMP
// save the debugging data to a file instead of displaying it
# define BT_REGEXP_LOG
# define BT_REGEXP_DEBUG_LOG
# define BT_REGEXP_DEBUG_LOG_NAME	"bt_regexp_debug.log"
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
ਸਥਿਰ sig_atomic_t dummy_timeout_flag = 0;
ਸਥਿਰ ਅਸਥਿਰ sig_atomic_t *timeout_flag = &dummy_timeout_flag;
#ਜੇ_ਅੰਤ

/*
 * Magic characters have a special meaning, they don't match literally.
 * Magic characters are negative.  This separates them from literal characters
 * (possibly multi-byte).  Only ASCII characters can be Magic.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Magic(x)	((ਪੂਰਨ)(x) - 256)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ un_Magic(x)	((x) + 256)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ is_Magic(x)	((x) < 0)

    ਸਥਿਰ ਪੂਰਨ
no_Magic(ਪੂਰਨ x)
{
    ਜੇ (is_Magic(x))
	ਵਾਪਸ un_Magic(x);
    ਵਾਪਸ x;
}

    ਸਥਿਰ ਪੂਰਨ
toggle_Magic(ਪੂਰਨ x)
{
    ਜੇ (is_Magic(x))
	ਵਾਪਸ un_Magic(x);
    ਵਾਪਸ Magic(x);
}

#ਜੇ defined(FEAT_RELTIME)
ਸਥਿਰ ਪੂਰਨ timeout_nesting = 0;

/*
 * Start a timer that will cause the regexp to abort after "msec".
 * This doesn't work well recursively.  In case it happens anyway, the first
 * set timeout will prevail, nested ones are ignored.
 * The caller must make sure there is a matching disable_regexp_timeout() call!
 */
    ਖਾਲੀ
init_regexp_timeout(ਲੰਮਾ msec)
{
    ਜੇ (timeout_nesting == 0)
	timeout_flag = ਸ਼ੁਰੂ_timeout(msec);
    ++timeout_nesting;
}

    ਖਾਲੀ
disable_regexp_timeout(ਖਾਲੀ)
{
    ਜੇ (timeout_nesting == 0)
	iemsg("disable_regexp_timeout() called without active timer");
    ਨਹੀਂਤਾਂ ਜੇ (--timeout_nesting == 0)
    {
	stop_timeout();
	timeout_flag = &dummy_timeout_flag;
    }
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL)
# ifdef FEAT_RELTIME
ਸਥਿਰ sig_atomic_t *saved_timeout_flag;
# endif

/*
 * Used at the debug prompt: disable the timeout so that expression evaluation
 * can used patterns.
 * Must be followed by calling restore_timeout_for_debugging().
 */
    ਖਾਲੀ
save_timeout_for_debugging(ਖਾਲੀ)
{
# ifdef FEAT_RELTIME
    saved_timeout_flag = (sig_atomic_t *)timeout_flag;
    timeout_flag = &dummy_timeout_flag;
# endif
}

    ਖਾਲੀ
restore_timeout_for_debugging(ਖਾਲੀ)
{
# ifdef FEAT_RELTIME
    timeout_flag = saved_timeout_flag;
# endif
}
#ਜੇ_ਅੰਤ

/*
 * The first byte of the BT regexp internal "program" is actually this magic
 * number; the start node begins in the second byte.  It's used to catch the
 * most severe mutilation of the program by the caller.
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REGMAGIC	0234

/*
 * Utility definitions.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCHARAT(p)	((ਪੂਰਨ)*(char_u *)(p))

// Used for an error (down from) vim_regcomp(): give the error message, set
// rc_did_emsg and return NULL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EMSG_RET_NULL(m) ਵਾਪਸ (emsg((m)), rc_did_emsg = TRUE, (ਖਾਲੀ *)ਨਿਰਾ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IEMSG_RET_NULL(m) ਵਾਪਸ (iemsg((m)), rc_did_emsg = TRUE, (ਖਾਲੀ *)ਨਿਰਾ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EMSG_RET_FAIL(m) ਵਾਪਸ (emsg((m)), rc_did_emsg = TRUE, FAIL)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EMSG2_RET_NULL(m, c) ਵਾਪਸ (semsg((ਅਚਲ ਅੱਖਰ *)(m), (c) ? "" : "\\"), rc_did_emsg = TRUE, (ਖਾਲੀ *)ਨਿਰਾ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EMSG3_RET_NULL(m, c, a) ਵਾਪਸ (semsg((ਅਚਲ ਅੱਖਰ *)(m), (c) ? "" : "\\", (a)), rc_did_emsg = TRUE, (ਖਾਲੀ *)ਨਿਰਾ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EMSG2_RET_FAIL(m, c) ਵਾਪਸ (semsg((ਅਚਲ ਅੱਖਰ *)(m), (c) ? "" : "\\"), rc_did_emsg = TRUE, FAIL)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EMSG_ONE_RET_NULL EMSG2_RET_NULL(_(e_invalid_ਵਸਤੂ_ਵਿੱਚ_str_brackets), reg_magic == MAGIC_ALL)


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ਹੱਦਬੰਦੀ	(32767L << 16L)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NOT_MULTI	0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MULTI_ONE	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MULTI_MULT	2

// return values for regmatch()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RA_FAIL		1	// something failed, abort
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RA_CONT		2	// continue in inner loop
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RA_BREAK	3	// break inner loop
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RA_ਮੇਲ	4	// successful match
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RA_NOMATCH	5	// didn't match

/*
 * Return NOT_MULTI if c is not a "multi" operator.
 * Return MULTI_ONE if c is a single "multi" operator.
 * Return MULTI_MULT if c is a multi "multi" operator.
 */
    ਸਥਿਰ ਪੂਰਨ
re_multi_type(ਪੂਰਨ c)
{
    ਜੇ (c == Magic('@') || c == Magic('=') || c == Magic('?'))
	ਵਾਪਸ MULTI_ONE;
    ਜੇ (c == Magic('*') || c == Magic('+') || c == Magic('{'))
	ਵਾਪਸ MULTI_MULT;
    ਵਾਪਸ NOT_MULTI;
}

ਸਥਿਰ char_u		*reg_prev_sub = ਨਿਰਾ;
ਸਥਿਰ size_t		reg_prev_sublen = 0;

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
ਸਥਿਰ char_u REGEXP_INRANGE[] = "]^-n\\";
ਸਥਿਰ char_u REGEXP_ABBR[] = "nrtebdoxuU";

/*
 * Translate '\x' to its control character, except "\n", which is Magic.
 */
    ਸਥਿਰ ਪੂਰਨ
backslash_trans(ਪੂਰਨ c)
{
    ਚੋਣ (c)
    {
	ਹਾਲਤ 'r':   ਵਾਪਸ CAR;
	ਹਾਲਤ 't':   ਵਾਪਸ TAB;
	ਹਾਲਤ 'e':   ਵਾਪਸ ESC;
	ਹਾਲਤ 'b':   ਵਾਪਸ BS;
    }
    ਵਾਪਸ c;
}

ਗਿਣਤੀ
{
    CLASS_ALNUM = 0,
    CLASS_ALPHA,
    CLASS_BLANK,
    CLASS_CNTRL,
    CLASS_DIGIT,
    CLASS_ਗ੍ਰਾਫ,
    CLASS_ਹੇਠਲਾ,
    CLASS_PRINT,
    CLASS_PUNCT,
    CLASS_SPACE,
    CLASS_ਉੱਤਲਾ,
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
    ਸਥਿਰ ਪੂਰਨ
get_char_class(char_u **pp)
{
    // must be sorted by the 'value' field because it is used by bsearch()!
    ਸਥਿਰ keyvalue_T char_class_tab[] =
    {
	KEYVALUE_ਇੰਦਰਾਜ(CLASS_ALNUM, "alnum:]"),
	KEYVALUE_ਇੰਦਰਾਜ(CLASS_ALPHA, "alpha:]"),
	KEYVALUE_ਇੰਦਰਾਜ(CLASS_BACKSPACE, "backspace:]"),
	KEYVALUE_ਇੰਦਰਾਜ(CLASS_BLANK, "blank:]"),
	KEYVALUE_ਇੰਦਰਾਜ(CLASS_CNTRL, "cntrl:]"),
	KEYVALUE_ਇੰਦਰਾਜ(CLASS_DIGIT, "digit:]"),
	KEYVALUE_ਇੰਦਰਾਜ(CLASS_ESCAPE, "escape:]"),
	KEYVALUE_ਇੰਦਰਾਜ(CLASS_FNAME, "fname:]"),
	KEYVALUE_ਇੰਦਰਾਜ(CLASS_ਗ੍ਰਾਫ, "graph:]"),
	KEYVALUE_ਇੰਦਰਾਜ(CLASS_IDENT, "ident:]"),
	KEYVALUE_ਇੰਦਰਾਜ(CLASS_KEYWORD, "keyword:]"),
	KEYVALUE_ਇੰਦਰਾਜ(CLASS_ਹੇਠਲਾ, "lower:]"),
	KEYVALUE_ਇੰਦਰਾਜ(CLASS_PRINT, "print:]"),
	KEYVALUE_ਇੰਦਰਾਜ(CLASS_PUNCT, "punct:]"),
	KEYVALUE_ਇੰਦਰਾਜ(CLASS_RETURN, "return:]"),
	KEYVALUE_ਇੰਦਰਾਜ(CLASS_SPACE, "space:]"),
	KEYVALUE_ਇੰਦਰਾਜ(CLASS_TAB, "tab:]"),
	KEYVALUE_ਇੰਦਰਾਜ(CLASS_ਉੱਤਲਾ, "upper:]"),
	KEYVALUE_ਇੰਦਰਾਜ(CLASS_XDIGIT, "xdigit:]")
    };

    // check that the value of "pp" has a chance of matching
    ਜੇ ((*pp)[1] == ':' && ASCII_ISLOWER((*pp)[2])
			&& ASCII_ISLOWER((*pp)[3]) && ASCII_ISLOWER((*pp)[4]))
    {
	keyvalue_T ਨਿਸ਼ਾਨਾ;
	keyvalue_T *ਇੰਦਰਾਜ;
	// this function can be called repeatedly with the same value for "pp"
	// so we cache the last found entry.
	ਸਥਿਰ keyvalue_T *ਆਖਰੀ_ਇੰਦਰਾਜ = ਨਿਰਾ;

	ਨਿਸ਼ਾਨਾ.ਕੁੰਜੀ = 0;
	ਨਿਸ਼ਾਨਾ.ਮੁੱਲ.string = *pp + 2;
	ਨਿਸ਼ਾਨਾ.ਮੁੱਲ.ਲੰਮਾਈ = 0;	// not used, see cmp_keyvalue_value_n()

	ਜੇ (ਆਖਰੀ_ਇੰਦਰਾਜ != ਨਿਰਾ && cmp_keyvalue_ਮੁੱਲ_n(&ਨਿਸ਼ਾਨਾ, ਆਖਰੀ_ਇੰਦਰਾਜ) == 0)
	    ਇੰਦਰਾਜ = ਆਖਰੀ_ਇੰਦਰਾਜ;
	ਨਹੀਂਤਾਂ
	    ਇੰਦਰਾਜ = (keyvalue_T *)bsearch(&ਨਿਸ਼ਾਨਾ, &char_class_tab,
					ARRAY_ਲੰਮਾਈ(char_class_tab),
					ਆਕਾਰ(char_class_tab[0]), cmp_keyvalue_ਮੁੱਲ_n);
	ਜੇ (ਇੰਦਰਾਜ != ਨਿਰਾ)
	{
	    ਆਖਰੀ_ਇੰਦਰਾਜ = ਇੰਦਰਾਜ;
	    *pp += ਇੰਦਰਾਜ->ਮੁੱਲ.ਲੰਮਾਈ + 2;
	    ਵਾਪਸ ਇੰਦਰਾਜ->ਕੁੰਜੀ;
	}
    }
    ਵਾਪਸ CLASS_NONE;
}

/*
 * Specific version of character class functions.
 * Using a table to keep this fast.
 */
ਸਥਿਰ ਛੋਟਾ	class_tab[256];

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	    RI_DIGIT	0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	    RI_HEX	0x02
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	    RI_OCTAL	0x04
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	    RI_ਸ਼ਬਦ	0x08
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	    RI_ਸਿਰ	0x10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	    RI_ALPHA	0x20
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	    RI_ਹੇਠਲਾ	0x40
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	    RI_ਉੱਤਲਾ	0x80
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	    RI_WHITE	0x100

    ਸਥਿਰ ਖਾਲੀ
init_class_tab(ਖਾਲੀ)
{
    ਪੂਰਨ		i;
    ਸਥਿਰ ਪੂਰਨ	ਪੂਰਾ = FALSE;

    ਜੇ (ਪੂਰਾ)
	ਵਾਪਸ;

    ਲਈ (i = 0; i < 256; ++i)
    {
	ਜੇ (i >= '0' && i <= '7')
	    class_tab[i] = RI_DIGIT + RI_HEX + RI_OCTAL + RI_ਸ਼ਬਦ;
	ਨਹੀਂਤਾਂ ਜੇ (i >= '8' && i <= '9')
	    class_tab[i] = RI_DIGIT + RI_HEX + RI_ਸ਼ਬਦ;
	ਨਹੀਂਤਾਂ ਜੇ (i >= 'a' && i <= 'f')
	    class_tab[i] = RI_HEX + RI_ਸ਼ਬਦ + RI_ਸਿਰ + RI_ALPHA + RI_ਹੇਠਲਾ;
	ਨਹੀਂਤਾਂ ਜੇ (i >= 'g' && i <= 'z')
	    class_tab[i] = RI_ਸ਼ਬਦ + RI_ਸਿਰ + RI_ALPHA + RI_ਹੇਠਲਾ;
	ਨਹੀਂਤਾਂ ਜੇ (i >= 'A' && i <= 'F')
	    class_tab[i] = RI_HEX + RI_ਸ਼ਬਦ + RI_ਸਿਰ + RI_ALPHA + RI_ਉੱਤਲਾ;
	ਨਹੀਂਤਾਂ ਜੇ (i >= 'G' && i <= 'Z')
	    class_tab[i] = RI_ਸ਼ਬਦ + RI_ਸਿਰ + RI_ALPHA + RI_ਉੱਤਲਾ;
	ਨਹੀਂਤਾਂ ਜੇ (i == '_')
	    class_tab[i] = RI_ਸ਼ਬਦ + RI_ਸਿਰ;
	ਨਹੀਂਤਾਂ
	    class_tab[i] = 0;
    }
    class_tab[' '] |= RI_WHITE;
    class_tab['\t'] |= RI_WHITE;
    ਪੂਰਾ = TRUE;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ri_digit(c)	((c) < 0x100 && (class_tab[c] & RI_DIGIT))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ri_hex(c)	((c) < 0x100 && (class_tab[c] & RI_HEX))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ri_octal(c)	((c) < 0x100 && (class_tab[c] & RI_OCTAL))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ri_ਸ਼ਬਦ(c)	((c) < 0x100 && (class_tab[c] & RI_ਸ਼ਬਦ))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ri_ਸਿਰ(c)	((c) < 0x100 && (class_tab[c] & RI_ਸਿਰ))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ri_alpha(c)	((c) < 0x100 && (class_tab[c] & RI_ALPHA))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ri_ਹੇਠਲਾ(c)	((c) < 0x100 && (class_tab[c] & RI_ਹੇਠਲਾ))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ri_ਉੱਤਲਾ(c)	((c) < 0x100 && (class_tab[c] & RI_ਉੱਤਲਾ))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ri_white(c)	((c) < 0x100 && (class_tab[c] & RI_WHITE))

// flags for regflags
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RF_ICASE    1	// ignore case
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RF_NOICASE  2	// don't ignore case
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RF_HASNL    4	// can match a NL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RF_ICOMBINE 8	// ignore combining characters
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RF_LOOKBH   16	// uses "\@<=" or "\@<!"

/*
 * Global work variables for vim_regcomp().
 */

ਸਥਿਰ char_u	*regparse;	// Input-scan pointer.
ਸਥਿਰ ਪੂਰਨ	regnpar;	// () count.
ਸਥਿਰ ਪੂਰਨ	ਚਾਹੁੰਦਾ_nfa;	// regex should use NFA engine
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
ਸਥਿਰ ਪੂਰਨ	regnzpar;	// \z() count.
ਸਥਿਰ ਪੂਰਨ	re_has_z;	// \z item detected
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ	regflags;	// RF_ flags for prog
#ਜੇ defined(FEAT_SYN_HL)
ਸਥਿਰ ਪੂਰਨ	had_eol;	// TRUE when EOL found by vim_regcomp()
#ਜੇ_ਅੰਤ

ਸਥਿਰ magic_T	reg_magic;	// magicness of the pattern

ਸਥਿਰ ਪੂਰਨ	reg_string;	// matching with a string instead of a buffer
				//ਸਤਰ
ਸਥਿਰ ਪੂਰਨ	reg_strict;	// "[abc" is illegal

/*
 * META contains all characters that may be magic, except '^' and '$'.
 */

// META[] is used often enough to justify turning it into a table.
ਸਥਿਰ ਅਚਲ char_u META_flags[] = {
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

ਸਥਿਰ ਪੂਰਨ	curchr;		// currently parsed character
// Previous character.  Note: prevchr is sometimes -1 when we are not at the
// start, eg in /[ ^I]^ the pattern was never found even if it existed,
// because ^ was taken to be magic -- webb
ਸਥਿਰ ਪੂਰਨ	prevchr;
ਸਥਿਰ ਪੂਰਨ	prevprevchr;	// previous-previous character
ਸਥਿਰ ਪੂਰਨ	nextchr;	// used for ungetchr()

// arguments for reg()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REG_NOPAREN	0	// toplevel reg()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REG_PAREN	1	// \(\)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REG_ZPAREN	2	// \z(\)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REG_NPAREN	3	// \%(\)

// Limit recursive parsing of nested regexp atoms to avoid using up the C stack.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REG_MAX_PAREN_ਡੂੰਘਾਈ	1000

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    char_u	*regparse;
    ਪੂਰਨ		prevchr_len;
    ਪੂਰਨ		curchr;
    ਪੂਰਨ		prevchr;
    ਪੂਰਨ		prevprevchr;
    ਪੂਰਨ		nextchr;
    ਪੂਰਨ		at_ਸ਼ੁਰੂ;
    ਪੂਰਨ		prev_at_ਸ਼ੁਰੂ;
    ਪੂਰਨ		regnpar;
} parse_state_T;

ਸਥਿਰ ਖਾਲੀ	initchr(char_u *);
ਸਥਿਰ ਪੂਰਨ	getchr(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ	skipchr_keepstart(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ	peekchr(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ	skipchr(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ	ungetchr(ਖਾਲੀ);
ਸਥਿਰ vimlong_T	gethexchrs(ਪੂਰਨ maxinputlen);
ਸਥਿਰ ਲੰਮਾ	getoctchrs(ਖਾਲੀ);
ਸਥਿਰ vimlong_T	getdecchrs(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ	coll_get_char(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ	prog_magic_wrong(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ	cstrncmp(char_u *s1, char_u *s2, ਪੂਰਨ *n);
ਸਥਿਰ char_u	*cstrchr(char_u *, ਪੂਰਨ);
ਸਥਿਰ ਪੂਰਨ	re_mult_ਅਗਲਾ(ਅੱਖਰ *what);
ਸਥਿਰ ਪੂਰਨ	reg_iswordc(ਪੂਰਨ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
ਸਥਿਰ ਖਾਲੀ report_re_switch(char_u *pat);
#ਜੇ_ਅੰਤ

ਸਥਿਰ regengine_T bt_regengine;
ਸਥਿਰ regengine_T nfa_regengine;

/*
 * Return TRUE if compiled regular expression "prog" can match a line break.
 */
    ਪੂਰਨ
re_multiline(regprog_T *prog)
{
    ਵਾਪਸ (prog->regflags & RF_HASNL);
}

/*
 * Check for an equivalence class name "[=a=]".  "pp" points to the '['.
 * Returns a character representing the class. Zero means that no item was
 * recognized.  Otherwise "pp" is advanced to after the item.
 */
    ਸਥਿਰ ਪੂਰਨ
get_equi_class(char_u **pp)
{
    ਪੂਰਨ		c;
    ਪੂਰਨ		l = 1;
    char_u	*p = *pp;

    ਜੇ (p[1] == '=' && p[2] != NUL)
    {
	ਜੇ (has_mbyte)
	    l = (*mb_ptr2len)(p + 2);
	ਜੇ (p[l + 2] == '=' && p[l + 3] == ']')
	{
	    ਜੇ (has_mbyte)
		c = mb_ptr2char(p + 2);
	    ਨਹੀਂਤਾਂ
		c = p[2];
	    *pp += l + 4;
	    ਵਾਪਸ c;
	}
    }
    ਵਾਪਸ 0;
}

/*
 * Check for a collating element "[.a.]".  "pp" points to the '['.
 * Returns a character. Zero means that no item was recognized.  Otherwise
 * "pp" is advanced to after the item.
 * Currently only single characters are recognized!
 */
    ਸਥਿਰ ਪੂਰਨ
get_coll_element(char_u **pp)
{
    ਪੂਰਨ		c;
    ਪੂਰਨ		l = 1;
    char_u	*p = *pp;

    ਜੇ (p[0] != NUL && p[1] == '.' && p[2] != NUL)
    {
	ਜੇ (has_mbyte)
	    l = (*mb_ptr2len)(p + 2);
	ਜੇ (p[l + 2] == '.' && p[l + 3] == ']')
	{
	    ਜੇ (has_mbyte)
		c = mb_ptr2char(p + 2);
	    ਨਹੀਂਤਾਂ
		c = p[2];
	    *pp += l + 4;
	    ਵਾਪਸ c;
	}
    }
    ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ reg_cpo_lit; // 'cpoptions' contains 'l' flag
ਸਥਿਰ ਪੂਰਨ reg_cpo_bsl; // 'cpoptions' contains '\' flag

    ਸਥਿਰ ਖਾਲੀ
get_cpo_flags(ਖਾਲੀ)
{
    reg_cpo_lit = vim_strchr(p_cpo, CPO_LITERAL) != ਨਿਰਾ;
    reg_cpo_bsl = vim_strchr(p_cpo, CPO_BACKSL) != ਨਿਰਾ;
}

/*
 * Skip over a "[]" range.
 * "p" must point to the character after the '['.
 * The returned pointer is on the matching ']', or the terminating NUL.
 */
    ਸਥਿਰ char_u *
skip_anyof(char_u *p)
{
    ਪੂਰਨ		l;

    ਜੇ (*p == '^')	// Complement of range.
	++p;
    ਜੇ (*p == ']' || *p == '-')
	++p;
    ਜਦੋਂ (*p != NUL && *p != ']')
    {
	ਜੇ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	    p += l;
	ਨਹੀਂਤਾਂ
	    ਜੇ (*p == '-')
	    {
		++p;
		ਜੇ (*p != ']' && *p != NUL)
		    MB_PTR_ADV(p);
	    }
	ਨਹੀਂਤਾਂ ਜੇ (*p == '\\'
		&& !reg_cpo_bsl
		&& (vim_strchr(REGEXP_INRANGE, p[1]) != ਨਿਰਾ
		    || (!reg_cpo_lit && vim_strchr(REGEXP_ABBR, p[1]) != ਨਿਰਾ)))
	    p += 2;
	ਨਹੀਂਤਾਂ ਜੇ (*p == '[')
	{
	    ਜੇ (get_char_class(&p) == CLASS_NONE
		    && get_equi_class(&p) == 0
		    && get_coll_element(&p) == 0
		    && *p != NUL)
		++p; // it is not a class name and not NUL
	}
	ਨਹੀਂਤਾਂ
	    ++p;
    }

    ਵਾਪਸ p;
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
    ਪੂਰਨ		delim,
    ਪੂਰਨ		magic)
{
    ਵਾਪਸ skip_regexp_ex(startp, delim, magic, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
}

/*
 * Call skip_regexp() and when the delimiter does not match give an error and
 * return NULL.
 */
    char_u *
skip_regexp_err(
    char_u	*startp,
    ਪੂਰਨ		delim,
    ਪੂਰਨ		magic)
{
    char_u *p = skip_regexp(startp, delim, magic);

    ਜੇ (*p != delim)
    {
	semsg(_(e_missing_delimiter_after_ਖੋਜ_ਨਮੂਨਾ_str), startp);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ p;
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
    ਪੂਰਨ		dirc,
    ਪੂਰਨ		magic,
    char_u	**newp,
    ਪੂਰਨ		*dropped,
    magic_T	*magic_val)
{
    magic_T	mymagic;
    char_u	*p = startp;
    size_t	startplen = 0;

    ਜੇ (magic)
	mymagic = MAGIC_ON;
    ਨਹੀਂਤਾਂ
	mymagic = MAGIC_OFF;
    get_cpo_flags();

    ਲਈ (; p[0] != NUL; MB_PTR_ADV(p))
    {
	ਜੇ (p[0] == dirc)	// found end of regexp
	    ਰੋਕੋ;
	ਜੇ ((p[0] == '[' && mymagic >= MAGIC_ON)
		|| (p[0] == '\\' && p[1] == '[' && mymagic <= MAGIC_OFF))
	{
	    p = skip_anyof(p + 1);
	    ਜੇ (p[0] == NUL)
		ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (p[0] == '\\' && p[1] != NUL)
	{
	    ਜੇ (dirc == '?' && newp != ਨਿਰਾ && p[1] == '?')
	    {
		// change "\?" to "?", make a copy first.
		ਜੇ (startplen == 0)
		    startplen = STRLEN(startp);
		ਜੇ (*newp == ਨਿਰਾ)
		{
		    *newp = vim_strnsave(startp, startplen);
		    ਜੇ (*newp != ਨਿਰਾ)
		    {
			p = *newp + (p - startp);
			startp = *newp;
		    }
		}
		ਜੇ (dropped != ਨਿਰਾ)
		    ++*dropped;
		ਜੇ (*newp != ਨਿਰਾ)
		    mch_memmove(p, p + 1, startplen - ((p + 1) - startp) + 1);
		ਨਹੀਂਤਾਂ
		    ++p;
	    }
	    ਨਹੀਂਤਾਂ
		++p;    // skip next character
	    ਜੇ (*p == 'v')
		mymagic = MAGIC_ALL;
	    ਨਹੀਂਤਾਂ ਜੇ (*p == 'V')
		mymagic = MAGIC_NONE;
	}
    }
    ਜੇ (magic_val != ਨਿਰਾ)
	*magic_val = mymagic;
    ਵਾਪਸ p;
}

/*
 * Functions for getting characters from the regexp input.
 */
ਸਥਿਰ ਪੂਰਨ	prevchr_len;	// byte length of previous char
ਸਥਿਰ ਪੂਰਨ	at_ਸ਼ੁਰੂ;	// True when on the first character
ਸਥਿਰ ਪੂਰਨ	prev_at_ਸ਼ੁਰੂ;  // True when on the second character

/*
 * Start parsing at "str".
 */
    ਸਥਿਰ ਖਾਲੀ
initchr(char_u *str)
{
    regparse = str;
    prevchr_len = 0;
    curchr = prevprevchr = prevchr = nextchr = -1;
    at_ਸ਼ੁਰੂ = TRUE;
    prev_at_ਸ਼ੁਰੂ = FALSE;
}

/*
 * Save the current parse state, so that it can be restored and parsing
 * starts in the same state again.
 */
    ਸਥਿਰ ਖਾਲੀ
save_parse_state(parse_state_T *ps)
{
    ps->regparse = regparse;
    ps->prevchr_len = prevchr_len;
    ps->curchr = curchr;
    ps->prevchr = prevchr;
    ps->prevprevchr = prevprevchr;
    ps->nextchr = nextchr;
    ps->at_ਸ਼ੁਰੂ = at_ਸ਼ੁਰੂ;
    ps->prev_at_ਸ਼ੁਰੂ = prev_at_ਸ਼ੁਰੂ;
    ps->regnpar = regnpar;
}

/*
 * Restore a previously saved parse state.
 */
    ਸਥਿਰ ਖਾਲੀ
restore_parse_state(parse_state_T *ps)
{
    regparse = ps->regparse;
    prevchr_len = ps->prevchr_len;
    curchr = ps->curchr;
    prevchr = ps->prevchr;
    prevprevchr = ps->prevprevchr;
    nextchr = ps->nextchr;
    at_ਸ਼ੁਰੂ = ps->at_ਸ਼ੁਰੂ;
    prev_at_ਸ਼ੁਰੂ = ps->prev_at_ਸ਼ੁਰੂ;
    regnpar = ps->regnpar;
}


/*
 * Get the next character without advancing.
 */
    ਸਥਿਰ ਪੂਰਨ
peekchr(ਖਾਲੀ)
{
    ਸਥਿਰ ਪੂਰਨ	after_slash = FALSE;

    ਜੇ (curchr != -1)
	ਵਾਪਸ curchr;

    ਚੋਣ (curchr = regparse[0])
    {
	ਹਾਲਤ '.':
	ਹਾਲਤ '[':
	ਹਾਲਤ '~':
	    // magic when 'magic' is on
	    ਜੇ (reg_magic >= MAGIC_ON)
		curchr = Magic(curchr);
	    ਰੋਕੋ;
	ਹਾਲਤ '(':
	ਹਾਲਤ ')':
	ਹਾਲਤ '{':
	ਹਾਲਤ '%':
	ਹਾਲਤ '+':
	ਹਾਲਤ '=':
	ਹਾਲਤ '?':
	ਹਾਲਤ '@':
	ਹਾਲਤ '!':
	ਹਾਲਤ '&':
	ਹਾਲਤ '|':
	ਹਾਲਤ '<':
	ਹਾਲਤ '>':
	ਹਾਲਤ '#':	// future ext.
	ਹਾਲਤ '"':	// future ext.
	ਹਾਲਤ '\'':	// future ext.
	ਹਾਲਤ ',':	// future ext.
	ਹਾਲਤ '-':	// future ext.
	ਹਾਲਤ ':':	// future ext.
	ਹਾਲਤ ';':	// future ext.
	ਹਾਲਤ '`':	// future ext.
	ਹਾਲਤ '/':	// Can't be used in / command
			// magic only after "\v"
	    ਜੇ (reg_magic == MAGIC_ALL)
		curchr = Magic(curchr);
	    ਰੋਕੋ;
	ਹਾਲਤ '*':
	    // * is not magic as the very first character, eg "?*ptr", when
	    // after '^', eg "/^*ptr" and when after "\(", "\|", "\&".  But
	    // "\(\*" is not magic, thus must be magic if "after_slash"
	    ਜੇ (reg_magic >= MAGIC_ON
		    && !at_ਸ਼ੁਰੂ
		    && !(prev_at_ਸ਼ੁਰੂ && prevchr == Magic('^'))
		    && (after_slash
			|| (prevchr != Magic('(')
			    && prevchr != Magic('&')
			    && prevchr != Magic('|'))))
		curchr = Magic('*');
	    ਰੋਕੋ;
	ਹਾਲਤ '^':
	    // '^' is only magic as the very first character and if it's after
	    // "\(", "\|", "\&' or "\n"
	    ਜੇ (reg_magic >= MAGIC_OFF
		    && (at_ਸ਼ੁਰੂ
			|| reg_magic == MAGIC_ALL
			|| prevchr == Magic('(')
			|| prevchr == Magic('|')
			|| prevchr == Magic('&')
			|| prevchr == Magic('n')
			|| (no_Magic(prevchr) == '('
			    && prevprevchr == Magic('%'))))
	    {
		curchr = Magic('^');
		at_ਸ਼ੁਰੂ = TRUE;
		prev_at_ਸ਼ੁਰੂ = FALSE;
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ '$':
	    // '$' is only magic as the very last char and if it's in front of
	    // either "\|", "\)", "\&", or "\n"
	    ਜੇ (reg_magic >= MAGIC_OFF)
	    {
		char_u *p = regparse + 1;
		ਪੂਰਨ is_magic_all = (reg_magic == MAGIC_ALL);

		// ignore \c \C \m \M \v \V and \Z after '$'
		ਜਦੋਂ (p[0] == '\\' && (p[1] == 'c' || p[1] == 'C'
			    || p[1] == 'm' || p[1] == 'M'
			    || p[1] == 'v' || p[1] == 'V' || p[1] == 'Z'))
		{
		    ਜੇ (p[1] == 'v')
			is_magic_all = TRUE;
		    ਨਹੀਂਤਾਂ ਜੇ (p[1] == 'm' || p[1] == 'M' || p[1] == 'V')
			is_magic_all = FALSE;
		    p += 2;
		}
		ਜੇ (p[0] == NUL
			|| (p[0] == '\\'
			    && (p[1] == '|' || p[1] == '&' || p[1] == ')'
				|| p[1] == 'n'))
			|| (is_magic_all
			    && (p[0] == '|' || p[0] == '&' || p[0] == ')'))
			|| reg_magic == MAGIC_ALL)
		    curchr = Magic('$');
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ '\\':
	    {
		ਪੂਰਨ c = regparse[1];

		ਜੇ (c == NUL)
		    curchr = '\\';	// trailing '\'
		ਨਹੀਂਤਾਂ ਜੇ (c <= '~' && META_flags[c])
		{
		    /*
		     * META contains everything that may be magic sometimes,
		     * except ^ and $ ("\^" and "\$" are only magic after
		     * "\V").  We now fetch the next character and toggle its
		     * magicness.  Therefore, \ is so meta-magic that it is
		     * not in META.
		     */
		    curchr = -1;
		    prev_at_ਸ਼ੁਰੂ = at_ਸ਼ੁਰੂ;
		    at_ਸ਼ੁਰੂ = FALSE;	// be able to say "/\*ptr"
		    ++regparse;
		    ++after_slash;
		    peekchr();
		    --regparse;
		    --after_slash;
		    curchr = toggle_Magic(curchr);
		}
		ਨਹੀਂਤਾਂ ਜੇ (vim_strchr(REGEXP_ABBR, c))
		{
		    /*
		     * Handle abbreviations, like "\t" for TAB -- webb
		     */
		    curchr = backslash_trans(c);
		}
		ਨਹੀਂਤਾਂ ਜੇ (reg_magic == MAGIC_NONE && (c == '$' || c == '^'))
		    curchr = toggle_Magic(c);
		ਨਹੀਂਤਾਂ
		{
		    /*
		     * Next character can never be (made) magic?
		     * Then backslashing it won't do anything.
		     */
		    ਜੇ (has_mbyte && c >= 0x80)
			curchr = (*mb_ptr2char)(regparse + 1);
		    ਨਹੀਂਤਾਂ
			curchr = c;
		}
		ਰੋਕੋ;
	    }

	ਮੂਲ:
	    // curchr already holds regparse[0]; only a multi-byte lead byte
	    // needs decoding.
	    ਜੇ (has_mbyte && curchr >= 0x80)
		curchr = (*mb_ptr2char)(regparse);
    }

    ਵਾਪਸ curchr;
}

/*
 * Eat one lexed character.  Do this in a way that we can undo it.
 */
    ਸਥਿਰ ਖਾਲੀ
skipchr(ਖਾਲੀ)
{
    // peekchr() eats a backslash, do the same here
    ਜੇ (*regparse == '\\')
	prevchr_len = 1;
    ਨਹੀਂਤਾਂ
	prevchr_len = 0;
    ਜੇ (regparse[prevchr_len] != NUL)
    {
	ਜੇ (enc_utf8)
	{
	    // Exclude composing chars that mb_ptr2len does include.  A byte
	    // below 0x80 is always a single character.
	    ਜੇ (regparse[prevchr_len] < 0x80)
		++prevchr_len;
	    ਨਹੀਂਤਾਂ
		prevchr_len += utf_ptr2len(regparse + prevchr_len);
	}
	ਨਹੀਂਤਾਂ ਜੇ (has_mbyte)
	    prevchr_len += (*mb_ptr2len)(regparse + prevchr_len);
	ਨਹੀਂਤਾਂ
	    ++prevchr_len;
    }
    regparse += prevchr_len;
    prev_at_ਸ਼ੁਰੂ = at_ਸ਼ੁਰੂ;
    at_ਸ਼ੁਰੂ = FALSE;
    prevprevchr = prevchr;
    prevchr = curchr;
    curchr = nextchr;	    // use previously unget char, or -1
    nextchr = -1;
}

/*
 * Skip a character while keeping the value of prev_at_start for at_start.
 * prevchr and prevprevchr are also kept.
 */
    ਸਥਿਰ ਖਾਲੀ
skipchr_keepstart(ਖਾਲੀ)
{
    ਪੂਰਨ as = prev_at_ਸ਼ੁਰੂ;
    ਪੂਰਨ pr = prevchr;
    ਪੂਰਨ prpr = prevprevchr;

    skipchr();
    at_ਸ਼ੁਰੂ = as;
    prevchr = pr;
    prevprevchr = prpr;
}

/*
 * Get the next character from the pattern. We know about magic and such, so
 * therefore we need a lexical analyzer.
 */
    ਸਥਿਰ ਪੂਰਨ
getchr(ਖਾਲੀ)
{
    ਪੂਰਨ chr = peekchr();

    skipchr();
    ਵਾਪਸ chr;
}

/*
 * put character back.  Works only once!
 */
    ਸਥਿਰ ਖਾਲੀ
ungetchr(ਖਾਲੀ)
{
    nextchr = curchr;
    curchr = prevchr;
    prevchr = prevprevchr;
    at_ਸ਼ੁਰੂ = prev_at_ਸ਼ੁਰੂ;
    prev_at_ਸ਼ੁਰੂ = FALSE;

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
    ਸਥਿਰ vimlong_T
gethexchrs(ਪੂਰਨ maxinputlen)
{
    long_u	nr = 0;
    ਪੂਰਨ		c;
    ਪੂਰਨ		i;

    ਲਈ (i = 0; i < maxinputlen; ++i)
    {
	c = regparse[0];
	ਜੇ (!vim_isxdigit(c))
	    ਰੋਕੋ;
	nr <<= 4;
	nr |= hex2nr(c);
	++regparse;
    }

    ਜੇ (i == 0)
	ਵਾਪਸ -1;
    ਵਾਪਸ nr;
}

/*
 * Get and return the value of the decimal string immediately after the
 * current position. Return -1 for invalid.  Consumes all digits.
 */
    ਸਥਿਰ vimlong_T
getdecchrs(ਖਾਲੀ)
{
    long_u	nr = 0;
    ਪੂਰਨ		c;
    ਪੂਰਨ		i;

    ਲਈ (i = 0; ; ++i)
    {
	c = regparse[0];
	ਜੇ (c < '0' || c > '9')
	    ਰੋਕੋ;
	nr *= 10;
	nr += c - '0';
	++regparse;
	curchr = -1; // no longer valid
    }

    ਜੇ (i == 0)
	ਵਾਪਸ -1;
    ਵਾਪਸ nr;
}

/*
 * get and return the value of the octal string immediately after the current
 * position. Return -1 for invalid, or 0-255 for valid. Smart enough to handle
 * numbers > 377 correctly (for example, 400 is treated as 40) and doesn't
 * treat 8 or 9 as recognised characters. Position is updated:
 *     blahblah\%o210asdf
 *	   before-^  ^-after
 */
    ਸਥਿਰ ਲੰਮਾ
getoctchrs(ਖਾਲੀ)
{
    long_u	nr = 0;
    ਪੂਰਨ		c;
    ਪੂਰਨ		i;

    ਲਈ (i = 0; i < 3 && nr < 040; ++i)
    {
	c = regparse[0];
	ਜੇ (c < '0' || c > '7')
	    ਰੋਕੋ;
	nr <<= 3;
	nr |= hex2nr(c);
	++regparse;
    }

    ਜੇ (i == 0)
	ਵਾਪਸ -1;
    ਵਾਪਸ (ਲੰਮਾ)nr;
}

/*
 * read_limits - Read two integers to be taken as a minimum and maximum.
 * If the first character is '-', then the range is reversed.
 * Should end with 'end'.  If minval is missing, zero is default, if maxval is
 * missing, a very big number is the default.
 */
    ਸਥਿਰ ਪੂਰਨ
read_limits(ਲੰਮਾ *minval, ਲੰਮਾ *maxval)
{
    ਪੂਰਨ		ਉਲਟ = FALSE;
    char_u	*ਪਹਿਲਾ_char;
    ਲੰਮਾ	tmp;

    ਜੇ (*regparse == '-')
    {
	// Starts with '-', so reverse the range later
	regparse++;
	ਉਲਟ = TRUE;
    }
    ਪਹਿਲਾ_char = regparse;
    *minval = getdigits(&regparse);
    ਜੇ (*regparse == ',')	    // There is a comma
    {
	ਜੇ (vim_isdigit(*++regparse))
	    *maxval = getdigits(&regparse);
	ਨਹੀਂਤਾਂ
	    *maxval = MAX_ਹੱਦਬੰਦੀ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (VIM_ISDIGIT(*ਪਹਿਲਾ_char))
	*maxval = *minval;	    // It was \{n} or \{-n}
    ਨਹੀਂਤਾਂ
	*maxval = MAX_ਹੱਦਬੰਦੀ;	    // It was \{} or \{-}
    ਜੇ (*regparse == '\\')
	regparse++;	// Allow either \{...} or \{...\}
    ਜੇ (*regparse != '}')
	EMSG2_RET_FAIL(_(e_syntax_error_ਵਿੱਚ_str_curlies),
						       reg_magic == MAGIC_ALL);

    /*
     * Reverse the range if there was a '-', or make sure it is in the right
     * order otherwise.
     */
    ਜੇ ((!ਉਲਟ && *minval > *maxval) || (ਉਲਟ && *minval < *maxval))
    {
	tmp = *minval;
	*minval = *maxval;
	*maxval = tmp;
    }
    skipchr();		// let's be friends with the lexer again
    ਵਾਪਸ OK;
}

/*
 * vim_regexec and friends
 */

/*
 * Global work variables for vim_regexec().
 */

ਸਥਿਰ ਖਾਲੀ	cleanup_subexpr(ਖਾਲੀ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
ਸਥਿਰ ਖਾਲੀ	cleanup_zsubexpr(ਖਾਲੀ);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਪੂਰਨ	ਮੇਲ_with_backref(linenr_T ਸ਼ੁਰੂ_lnum, colnr_T ਸ਼ੁਰੂ_ਖਾਨਾ, linenr_T ਸਿਰਾ_lnum, colnr_T ਸਿਰਾ_ਖਾਨਾ, ਪੂਰਨ *bytelen);

/*
 * Sometimes need to save a copy of a line.  Since alloc()/free() is very
 * slow, we keep one allocated piece of memory and only re-allocate it when
 * it's too small.  It's freed in bt_regexec_both() when finished.
 */
ਸਥਿਰ char_u	*reg_tofree = ਨਿਰਾ;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ	reg_tofreelen;

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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    regmatch_T		*reg_ਮੇਲ;
    regmmatch_T		*reg_mmatch;

    char_u		**reg_startp;
    char_u		**reg_endp;
    lpos_T		*reg_startpos;
    lpos_T		*reg_endpos;

    win_T		*reg_win;
    buf_T		*reg_buf;
    linenr_T		reg_firstlnum;
    linenr_T		reg_maxline;
    ਪੂਰਨ			reg_ਸਤਰ_lbr;	// "\n" in string is line break

    // The current match-position is stord in these variables:
    linenr_T	lnum;		// line number, relative to first line
    char_u	*ਸਤਰ;		// start of current line
    char_u	*ਨਿਵੇਸ਼;		// current input, points into "line"

    ਪੂਰਨ	need_clear_subexpr;	// subexpressions still need to be cleared
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ਪੂਰਨ	need_clear_zsubexpr;	// extmatch subexpressions still need to be
				// cleared
#ਜੇ_ਅੰਤ

    // Internal copy of 'ignorecase'.  It is set at each call to vim_regexec().
    // Normally it gets the value of "rm_ic" or "rmm_ic", but when the pattern
    // contains '\c' or '\C' the value is overruled.
    ਪੂਰਨ			reg_ic;

    // Similar to "reg_ic", but only for 'combining' characters.  Set with \Z
    // flag in the regexp.  Defaults to false, always.
    ਪੂਰਨ			reg_icombine;

    // Copy of "rmm_maxcol": maximum column to search for a match.  Zero when
    // there is no maximum.
    colnr_T		reg_maxcol;

    // State for the NFA engine regexec.
    ਪੂਰਨ nfa_has_zend;	    // NFA regexp \ze operator encountered.
    ਪੂਰਨ nfa_has_backref;    // NFA regexp \1 .. \9 encountered.
    ਪੂਰਨ nfa_nsubexpr;	    // Number of sub expressions actually being used
			    // during execution. 1 if only the whole match
			    // (subexpr 0) is used.
    // listid is global, so that it increases on recursive calls to
    // nfa_regmatch(), which means we don't have to clear the lastlist field of
    // all the states.
    ਪੂਰਨ nfa_listid;
    ਪੂਰਨ nfa_alt_listid;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ਪੂਰਨ nfa_has_zsubexpr;   // NFA regexp has \z( ), set zsubexpr.
#ਜੇ_ਅੰਤ
} regexec_T;

ਸਥਿਰ regexec_T	rex;
ਸਥਿਰ ਪੂਰਨ		rex_ਵਿੱਚ_use = FALSE;

/*
 * Return TRUE if character 'c' is included in 'iskeyword' option for
 * "reg_buf" buffer.
 */
    ਸਥਿਰ ਪੂਰਨ
reg_iswordc(ਪੂਰਨ c)
{
    ਵਾਪਸ vim_iswordc_buf(c, rex.reg_buf);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
ਸਥਿਰ ਪੂਰਨ can_f_submatch = FALSE;	// TRUE when submatch() can be used

// This struct is used for reg_submatch(). Needed for when the
// substitution string is an expression that contains a call to substitute()
// and submatch().
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    regmatch_T	*sm_ਮੇਲ;
    regmmatch_T	*sm_mmatch;
    linenr_T	sm_firstlnum;
    linenr_T	sm_maxline;
    ਪੂਰਨ		sm_ਸਤਰ_lbr;
} regsubmatch_T;

ਸਥਿਰ regsubmatch_T rsm;  // can only be used when can_f_submatch is TRUE
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    RGLF_ਸਤਰ = 0x01,
    RGLF_ਲੰਮਾਈ = 0x02
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ,
    RGLF_SUBMATCH = 0x04
#ਜੇ_ਅੰਤ
} reg_getline_flags_T;

//
// common code for reg_getline(), reg_getline_len(), reg_getline_submatch() and
// reg_getline_submatch_len().
// the flags argument (which is a bitmask) controls what info is to be returned and whether
// or not submatch is in effect.
// note:
//     submatch is available only if FEAT_EVAL is defined.
    ਸਥਿਰ ਖਾਲੀ
reg_getline_common(
    linenr_T		lnum,
    reg_getline_flags_T	flags,
    char_u		**ਸਤਰ,
    colnr_T		*ਲੰਮਾਈ)
{
    ਪੂਰਨ get_ਸਤਰ = flags & RGLF_ਸਤਰ;
    ਪੂਰਨ get_ਲੰਮਾਈ = flags & RGLF_ਲੰਮਾਈ;
    linenr_T firstlnum;
    linenr_T maxline;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (flags & RGLF_SUBMATCH)
    {
	firstlnum = rsm.sm_firstlnum + lnum;
	maxline = rsm.sm_maxline;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	firstlnum = rex.reg_firstlnum + lnum;
	maxline = rex.reg_maxline;
    }

    // when looking behind for a match/no-match lnum is negative. but we
    // can't go before line 1.
    ਜੇ (firstlnum < 1)
    {
	ਜੇ (get_ਸਤਰ)
	    *ਸਤਰ = ਨਿਰਾ;
	ਜੇ (get_ਲੰਮਾਈ)
	    *ਲੰਮਾਈ = 0;

	ਵਾਪਸ;
    }

    ਜੇ (lnum > maxline)
    {
	// must have matched the "\n" in the last line.
	ਜੇ (get_ਸਤਰ)
	    *ਸਤਰ = (char_u *)"";
	ਜੇ (get_ਲੰਮਾਈ)
	    *ਲੰਮਾਈ = 0;

	ਵਾਪਸ;
    }

    ਜੇ (get_ਸਤਰ)
	*ਸਤਰ = ml_get_buf(rex.reg_buf, firstlnum, FALSE);
    ਜੇ (get_ਲੰਮਾਈ)
	*ਲੰਮਾਈ = ml_get_buf_len(rex.reg_buf, firstlnum);
}

/*
 * Get pointer to the line "lnum", which is relative to "reg_firstlnum".
 */
    ਸਥਿਰ char_u *
reg_getline(linenr_T lnum)
{
    char_u *ਸਤਰ;

    reg_getline_common(lnum, RGLF_ਸਤਰ, &ਸਤਰ, ਨਿਰਾ);

    ਵਾਪਸ ਸਤਰ;
}

/*
 * Get length of line "lnum", which is relative to "reg_firstlnum".
 */
    ਸਥਿਰ colnr_T
reg_getline_len(linenr_T lnum)
{
    colnr_T ਲੰਮਾਈ;

    reg_getline_common(lnum, RGLF_ਲੰਮਾਈ, ਨਿਰਾ, &ਲੰਮਾਈ);

    ਵਾਪਸ ਲੰਮਾਈ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
ਸਥਿਰ char_u	*reg_startzp[NSUBEXP];	// Workspace to mark beginning
ਸਥਿਰ char_u	*reg_endzp[NSUBEXP];	//   and end of \z(...\) matches
ਸਥਿਰ lpos_T	reg_startzpos[NSUBEXP];	// idem, beginning pos
ਸਥਿਰ lpos_T	reg_endzpos[NSUBEXP];	// idem, end pos
#ਜੇ_ਅੰਤ

// TRUE if using multi-line regexp.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REG_MULTI	(rex.reg_ਮੇਲ == ਨਿਰਾ)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
/*
 * Create a new extmatch and mark it as referenced once.
 */
    ਸਥਿਰ reg_extmatch_T *
make_extmatch(ਖਾਲੀ)
{
    reg_extmatch_T	*em;

    em = ALLOC_CLEAR_ONE(reg_extmatch_T);
    ਜੇ (em != ਨਿਰਾ)
	em->refcnt = 1;
    ਵਾਪਸ em;
}

/*
 * Add a reference to an extmatch.
 */
    reg_extmatch_T *
ref_extmatch(reg_extmatch_T *em)
{
    ਜੇ (em != ਨਿਰਾ)
	em->refcnt++;
    ਵਾਪਸ em;
}

/*
 * Remove a reference to an extmatch.  If there are no references left, free
 * the info.
 */
    ਖਾਲੀ
unref_extmatch(reg_extmatch_T *em)
{
    ਪੂਰਨ i;

    ਜੇ (em != ਨਿਰਾ && --em->refcnt <= 0)
    {
	ਲਈ (i = 0; i < NSUBEXP; ++i)
	    vim_free(em->matches[i]);
	vim_free(em);
    }
}
#ਜੇ_ਅੰਤ

/*
 * Get class of previous character.
 */
    ਸਥਿਰ ਪੂਰਨ
reg_prev_class(ਖਾਲੀ)
{
    ਜੇ (rex.ਨਿਵੇਸ਼ > rex.ਸਤਰ)
	ਵਾਪਸ mb_get_class_buf(rex.ਨਿਵੇਸ਼ - 1
		       - (*mb_ਸਿਰ_off)(rex.ਸਤਰ, rex.ਨਿਵੇਸ਼ - 1), rex.reg_buf);
    ਵਾਪਸ -1;
}

/*
 * Return TRUE if the current rex.input position matches the Visual area.
 */
    ਸਥਿਰ ਪੂਰਨ
reg_ਮੇਲ_visual(ਖਾਲੀ)
{
    pos_T	ਸਿਖਰ, bot;
    linenr_T    lnum;
    colnr_T	ਖਾਨਾ;
    win_T	*wp = rex.reg_win == ਨਿਰਾ ? curwin : rex.reg_win;
    ਪੂਰਨ		ਬਹੁਲਕ;
    colnr_T	ਸ਼ੁਰੂ, ਸਿਰਾ;
    colnr_T	start2, end2;
    colnr_T	ਖਾਨੇ_ਗਿਣਤੀ;
    colnr_T	curswant;

    // Check if the buffer is the current buffer and not using a string.
    ਜੇ (rex.reg_buf != curbuf || VIsual.lnum == 0 || !REG_MULTI)
	ਵਾਪਸ FALSE;

    ਜੇ (VIsual_ਸਰਗਰਮ)
    {
	ਜੇ (LT_POS(VIsual, wp->w_cursor))
	{
	    ਸਿਖਰ = VIsual;
	    bot = wp->w_cursor;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਸਿਖਰ = wp->w_cursor;
	    bot = VIsual;
	}
	ਬਹੁਲਕ = VIsual_ਬਹੁਲਕ;
	curswant = wp->w_curswant;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (LT_POS(curbuf->b_visual.vi_ਸ਼ੁਰੂ, curbuf->b_visual.vi_ਸਿਰਾ))
	{
	    ਸਿਖਰ = curbuf->b_visual.vi_ਸ਼ੁਰੂ;
	    bot = curbuf->b_visual.vi_ਸਿਰਾ;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਸਿਖਰ = curbuf->b_visual.vi_ਸਿਰਾ;
	    bot = curbuf->b_visual.vi_ਸ਼ੁਰੂ;
	}
	// a substitute command may have removed some lines
	ਜੇ (bot.lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    bot.lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	ਬਹੁਲਕ = curbuf->b_visual.vi_ਬਹੁਲਕ;
	curswant = curbuf->b_visual.vi_curswant;
    }
    lnum = rex.lnum + rex.reg_firstlnum;
    ਜੇ (lnum < ਸਿਖਰ.lnum || lnum > bot.lnum)
	ਵਾਪਸ FALSE;

    ਖਾਨਾ = (colnr_T)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ);
    ਜੇ (ਬਹੁਲਕ == 'v')
    {
	ਜੇ ((lnum == ਸਿਖਰ.lnum && ਖਾਨਾ < ਸਿਖਰ.ਖਾਨਾ)
		|| (lnum == bot.lnum && ਖਾਨਾ >= bot.ਖਾਨਾ + (*p_sel != 'e')))
	    ਵਾਪਸ FALSE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਬਹੁਲਕ == Ctrl_V)
    {
	getvvcol(wp, &ਸਿਖਰ, &ਸ਼ੁਰੂ, ਨਿਰਾ, &ਸਿਰਾ, 0);
	getvvcol(wp, &bot, &start2, ਨਿਰਾ, &end2, 0);
	ਜੇ (start2 < ਸ਼ੁਰੂ)
	    ਸ਼ੁਰੂ = start2;
	ਜੇ (end2 > ਸਿਰਾ)
	    ਸਿਰਾ = end2;
	ਜੇ (ਸਿਖਰ.ਖਾਨਾ == MAXCOL || bot.ਖਾਨਾ == MAXCOL || curswant == MAXCOL)
	    ਸਿਰਾ = MAXCOL;

	// getvvcol() flushes rex.line, need to get it again
	rex.ਸਤਰ = reg_getline(rex.lnum);
	rex.ਨਿਵੇਸ਼ = rex.ਸਤਰ + ਖਾਨਾ;

	ਖਾਨੇ_ਗਿਣਤੀ = win_linetabsize(wp, rex.reg_firstlnum + rex.lnum, rex.ਸਤਰ, ਖਾਨਾ);
	ਜੇ (ਖਾਨੇ_ਗਿਣਤੀ < ਸ਼ੁਰੂ || ਖਾਨੇ_ਗਿਣਤੀ > ਸਿਰਾ - (*p_sel == 'e'))
	    ਵਾਪਸ FALSE;
    }
    ਵਾਪਸ TRUE;
}

/*
 * Check the regexp program for its magic number.
 * Return TRUE if it's wrong.
 */
    ਸਥਿਰ ਪੂਰਨ
prog_magic_wrong(ਖਾਲੀ)
{
    regprog_T	*prog;

    prog = REG_MULTI ? rex.reg_mmatch->regprog : rex.reg_ਮੇਲ->regprog;
    ਜੇ (prog->engine == &nfa_regengine)
	// For NFA matcher we don't check the magic
	ਵਾਪਸ FALSE;

    ਜੇ (UCHARAT(((bt_regprog_T *)prog)->ਪ੍ਰੋਗਰਾਮ) != REGMAGIC)
    {
	iemsg(e_corrupted_regexp_ਪ੍ਰੋਗਰਾਮ);
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Cleanup the subexpressions, if this wasn't done yet.
 * This construction is used to clear the subexpressions only when they are
 * used (to increase speed).
 */
    ਸਥਿਰ ਖਾਲੀ
cleanup_subexpr(ਖਾਲੀ)
{
    ਜੇ (!rex.need_clear_subexpr)
	ਵਾਪਸ;

    ਜੇ (REG_MULTI)
    {
	// Use 0xff to set lnum to -1
	vim_memset(rex.reg_startpos, 0xff, ਆਕਾਰ(lpos_T) * NSUBEXP);
	vim_memset(rex.reg_endpos, 0xff, ਆਕਾਰ(lpos_T) * NSUBEXP);
    }
    ਨਹੀਂਤਾਂ
    {
	vim_memset(rex.reg_startp, 0, ਆਕਾਰ(char_u *) * NSUBEXP);
	vim_memset(rex.reg_endp, 0, ਆਕਾਰ(char_u *) * NSUBEXP);
    }
    rex.need_clear_subexpr = FALSE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ਸਥਿਰ ਖਾਲੀ
cleanup_zsubexpr(ਖਾਲੀ)
{
    ਜੇ (!rex.need_clear_zsubexpr)
	ਵਾਪਸ;

    ਜੇ (REG_MULTI)
    {
	// Use 0xff to set lnum to -1
	vim_memset(reg_startzpos, 0xff, ਆਕਾਰ(lpos_T) * NSUBEXP);
	vim_memset(reg_endzpos, 0xff, ਆਕਾਰ(lpos_T) * NSUBEXP);
    }
    ਨਹੀਂਤਾਂ
    {
	vim_memset(reg_startzp, 0, ਆਕਾਰ(char_u *) * NSUBEXP);
	vim_memset(reg_endzp, 0, ਆਕਾਰ(char_u *) * NSUBEXP);
    }
    rex.need_clear_zsubexpr = FALSE;
}
#ਜੇ_ਅੰਤ

/*
 * Advance rex.lnum, rex.line and rex.input to the next line.
 */
    ਸਥਿਰ ਖਾਲੀ
reg_nextline(ਖਾਲੀ)
{
    rex.ਸਤਰ = reg_getline(++rex.lnum);
    rex.ਨਿਵੇਸ਼ = rex.ਸਤਰ;
    fast_breakcheck();
}

/*
 * Check whether a backreference matches.
 * Returns RA_FAIL, RA_NOMATCH or RA_MATCH.
 * If "bytelen" is not NULL, it is set to the byte length of the match in the
 * last line.
 * Optional: ignore case if rex.reg_ic is set.
 */
    ਸਥਿਰ ਪੂਰਨ
ਮੇਲ_with_backref(
    linenr_T ਸ਼ੁਰੂ_lnum,
    colnr_T  ਸ਼ੁਰੂ_ਖਾਨਾ,
    linenr_T ਸਿਰਾ_lnum,
    colnr_T  ਸਿਰਾ_ਖਾਨਾ,
    ਪੂਰਨ	     *bytelen)
{
    linenr_T	clnum = ਸ਼ੁਰੂ_lnum;
    colnr_T	ccol = ਸ਼ੁਰੂ_ਖਾਨਾ;
    ਪੂਰਨ		len;
    char_u	*p;

    ਜੇ (bytelen != ਨਿਰਾ)
	*bytelen = 0;
    ਲਈ (;;)
    {
	// Since getting one line may invalidate the other, need to make copy.
	// Slow!
	ਜੇ (rex.ਸਤਰ != reg_tofree)
	{
	    len = (ਪੂਰਨ)STRLEN(rex.ਸਤਰ);
	    ਜੇ (reg_tofree == ਨਿਰਾ || len >= (ਪੂਰਨ)reg_tofreelen)
	    {
		len += 50;	// get some extra
		vim_free(reg_tofree);
		reg_tofree = alloc(len);
		ਜੇ (reg_tofree == ਨਿਰਾ)
		    ਵਾਪਸ RA_FAIL; // out of memory!
		reg_tofreelen = len;
	    }
	    STRCPY(reg_tofree, rex.ਸਤਰ);
	    rex.ਨਿਵੇਸ਼ = reg_tofree + (rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ);
	    rex.ਸਤਰ = reg_tofree;
	}

	// Get the line to compare with.
	p = reg_getline(clnum);
	ਜੇ (clnum == ਸਿਰਾ_lnum)
	    len = ਸਿਰਾ_ਖਾਨਾ - ccol;
	ਨਹੀਂਤਾਂ
	    len = (ਪੂਰਨ)reg_getline_len(clnum) - ccol;

	// Use case-insensitive compare if rex.reg_ic is set
	ਜੇ ((!rex.reg_ic && cstrncmp(p + ccol, rex.ਨਿਵੇਸ਼, &len) != 0)
	    || (rex.reg_ic && MB_STRNICMP(p + ccol, rex.ਨਿਵੇਸ਼, len) != 0))
	    ਵਾਪਸ RA_NOMATCH;  // doesn't match
	ਜੇ (bytelen != ਨਿਰਾ)
	    *bytelen += len;
	ਜੇ (clnum == ਸਿਰਾ_lnum)
	    ਰੋਕੋ;		// match and at end!
	ਜੇ (rex.lnum >= rex.reg_maxline)
	    ਵਾਪਸ RA_NOMATCH;  // text too short

	// Advance to next line.
	reg_nextline();
	ਜੇ (bytelen != ਨਿਰਾ)
	    *bytelen = 0;
	++clnum;
	ccol = 0;
	ਜੇ (got_int)
	    ਵਾਪਸ RA_FAIL;
    }

    // found a match!  Note that rex.line may now point to a copy of the line,
    // that should not matter.
    ਵਾਪਸ RA_ਮੇਲ;
}

/*
 * Used in a place where no * or \+ can follow.
 */
    ਸਥਿਰ ਪੂਰਨ
re_mult_ਅਗਲਾ(ਅੱਖਰ *what)
{
    ਜੇ (re_multi_type(peekchr()) == MULTI_MULT)
    {
	semsg(_(e_nfa_regexp_cannot_repeat_str), what);
	rc_did_emsg = TRUE;
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ a, b, c;
} decomp_T;


// 0xfb20 - 0xfb4f
ਸਥਿਰ decomp_T decomp_ਸਾਰਣੀ[0xfb4f-0xfb20+1] =
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

    ਸਥਿਰ ਖਾਲੀ
mb_decompose(ਪੂਰਨ c, ਪੂਰਨ *c1, ਪੂਰਨ *c2, ਪੂਰਨ *c3)
{
    decomp_T d;

    ਜੇ (c >= 0xfb20 && c <= 0xfb4f)
    {
	d = decomp_ਸਾਰਣੀ[c - 0xfb20];
	*c1 = d.a;
	*c2 = d.b;
	*c3 = d.c;
    }
    ਨਹੀਂਤਾਂ
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
    ਸਥਿਰ ਪੂਰਨ
cstrncmp(char_u *s1, char_u *s2, ਪੂਰਨ *n)
{
    ਪੂਰਨ		ਨਤੀਜਾ;

    ਜੇ (!rex.reg_ic)
	ਨਤੀਜਾ = STRNCMP(s1, s2, *n);
    ਨਹੀਂਤਾਂ ਜੇ (enc_utf8)
    {
	char_u *p = s1;
	ਪੂਰਨ n2 = 0;
	ਪੂਰਨ n1 = *n;
	// count the number of characters for byte-length of s1
	ਜਦੋਂ (n1 > 0 && *p != NUL)
	{
	    n1 -= mb_ptr2len(p);
	    MB_PTR_ADV(p);
	    n2++;
	}
	// count the number of bytes to advance the same number of chars for s2
	p = s2;
	ਜਦੋਂ (n2-- > 0 && *p != NUL)
	    MB_PTR_ADV(p);

	n2 = p - s2;

	ਨਤੀਜਾ = MB_STRNICMP2(s1, s2, *n, n2);
	ਜੇ (ਨਤੀਜਾ == 0)
	    *n = n2;
    }
    ਨਹੀਂਤਾਂ
	ਨਤੀਜਾ = MB_STRNICMP(s1, s2, *n);

    // if it failed and it's utf8 and we want to combineignore:
    ਜੇ (ਨਤੀਜਾ != 0 && enc_utf8 && rex.reg_icombine)
    {
	char_u	*str1, *str2;
	ਪੂਰਨ	c1, c2, c11, c12;
	ਪੂਰਨ	junk;

	// we have to handle the strcmp ourselves, since it is necessary to
	// deal with the composing characters by ignoring them:
	str1 = s1;
	str2 = s2;
	c1 = c2 = 0;
	ਜਦੋਂ ((ਪੂਰਨ)(str1 - s1) < *n)
	{
	    c1 = mb_ptr2char_adv(&str1);
	    c2 = mb_ptr2char_adv(&str2);

	    // Decompose the character if necessary, into 'base' characters.
	    // Currently hard-coded for Hebrew, Arabic to be done...
	    ਜੇ (c1 != c2 && (!rex.reg_ic || utf_fold(c1) != utf_fold(c2)))
	    {
		// decomposition necessary?
		mb_decompose(c1, &c11, &junk, &junk);
		mb_decompose(c2, &c12, &junk, &junk);
		c1 = c11;
		c2 = c12;
		ਜੇ (c11 != c12
			    && (!rex.reg_ic || utf_fold(c11) != utf_fold(c12)))
		    ਰੋਕੋ;
	    }
	}
	ਨਤੀਜਾ = c2 - c1;
	ਜੇ (ਨਤੀਜਾ == 0)
	    *n = (ਪੂਰਨ)(str2 - s2);
    }

    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * cstrchr: This function is used a lot for simple searches, keep it fast!
 */
    ਸਥਿਰ char_u *
cstrchr(char_u *s, ਪੂਰਨ c)
{
    char_u	*p;
    ਪੂਰਨ		cc, lc;

    ਜੇ (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	ਵਾਪਸ vim_strchr(s, c);

    // tolower() and toupper() can be slow, comparing twice should be a lot
    // faster (esp. when using MS Visual C++!).
    // For UTF-8 need to use folded case.
    ਜੇ (enc_utf8 && c > 0x80)
    {
	cc = utf_fold(c);
	lc = cc;
    }
    ਨਹੀਂਤਾਂ
	ਜੇ (MB_ISUPPER(c))
	{
	    cc = MB_TOLOWER(c);
	    lc = cc;
	}
	ਨਹੀਂਤਾਂ ਜੇ (MB_ISLOWER(c))
	{
	    cc = MB_TOUPPER(c);
	    lc = c;
	}
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ vim_strchr(s, c);

    ਜੇ (has_mbyte)
    {
	ਲਈ (p = s; *p != NUL; p += (*mb_ptr2len)(p))
	{
	    ਪੂਰਨ uc = utf_ptr2char(p);
	    ਜੇ (enc_utf8 && (c > 0x80 || uc > 0x80))
	    {
		// Do not match an illegal byte.  E.g. 0xff matches 0xc3 0xbf,
		// not 0xff.
		// compare with lower case of the character
		ਜੇ ((uc < 0x80 || uc != *p) && utf_fold(uc) == lc)
		    ਵਾਪਸ p;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*p == c || *p == cc)
		ਵਾਪਸ p;
	}
    }
    ਨਹੀਂਤਾਂ
	// Faster version for when there are no multi-byte characters.
	ਲਈ (p = s; *p != NUL; ++p)
	    ਜੇ (*p == c || *p == cc)
		ਵਾਪਸ p;

    ਵਾਪਸ ਨਿਰਾ;
}

////////////////////////////////////////////////////////////////
//		      regsub stuff			      //
////////////////////////////////////////////////////////////////

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*fptr_T)(ਪੂਰਨ *, ਪੂਰਨ);

ਸਥਿਰ ਪੂਰਨ vim_regsub_both(char_u *ਸਰੋਤ, typval_T *ਵਿਅੰਜਕ, char_u *dest, ਪੂਰਨ destlen, ਪੂਰਨ flags);

    ਸਥਿਰ ਖਾਲੀ
do_ਉੱਤਲਾ(ਪੂਰਨ *d, ਪੂਰਨ c)
{
    *d = MB_TOUPPER(c);
}

    ਸਥਿਰ ਖਾਲੀ
do_ਹੇਠਲਾ(ਪੂਰਨ *d, ਪੂਰਨ c)
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
regtilde(char_u *ਸਰੋਤ, ਪੂਰਨ magic)
{
    char_u	*newsub = ਸਰੋਤ;
    char_u	*p;
    size_t	newsublen = 0;
    char_u	tilde[3] = {'~', NUL, NUL};
    size_t	tildelen = 1;
    ਪੂਰਨ		error = FALSE;

    ਜੇ (!magic)
    {
	tilde[0] = '\\';
	tilde[1] = '~';
	tilde[2] = NUL;
	tildelen = 2;
    }

    ਲਈ (p = newsub; *p; ++p)
    {
	ਜੇ (STRNCMP(p, tilde, tildelen) == 0)
	{
	    size_t prefixlen = p - newsub;		// not including the tilde
	    char_u *postfix = p + tildelen;
	    size_t postfixlen;
	    size_t tmpsublen;

	    ਜੇ (newsublen == 0)
		newsublen = STRLEN(newsub);
	    newsublen -= tildelen;
	    postfixlen = newsublen - prefixlen;
	    tmpsublen = prefixlen + reg_prev_sublen + postfixlen;

	    ਜੇ (tmpsublen > 0 && reg_prev_sub != ਨਿਰਾ)
	    {
		char_u *tmpsub;

		// Avoid making the text longer than MAXCOL, it will cause
		// trouble at some point.
		ਜੇ (tmpsublen > MAXCOL)
		{
		    emsg(_(e_resulting_ਪਾਠ_too_long));
		    error = TRUE;
		    ਰੋਕੋ;
		}

		tmpsub = alloc(tmpsublen + 1);
		ਜੇ (tmpsub == ਨਿਰਾ)
		{
		    emsg(_(e_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ));
		    error = TRUE;
		    ਰੋਕੋ;
		}

		// copy prefix
		mch_memmove(tmpsub, newsub, prefixlen);
		// interpret tilde
		mch_memmove(tmpsub + prefixlen, reg_prev_sub, reg_prev_sublen);
		// copy postfix
		STRCPY(tmpsub + prefixlen + reg_prev_sublen, postfix);

		ਜੇ (newsub != ਸਰੋਤ)	// allocated newsub before
		    vim_free(newsub);
		newsub = tmpsub;
		newsublen = tmpsublen;
		p = newsub + prefixlen + reg_prev_sublen;
	    }
	    ਨਹੀਂਤਾਂ
		mch_memmove(p, postfix, postfixlen + 1);	// remove the tilde (+1 for the NUL)

	    --p;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (*p == '\\' && p[1])		// skip escaped characters
		++p;
	    ਜੇ (has_mbyte)
		p += (*mb_ptr2len)(p) - 1;
	}
    }

    ਜੇ (error)
    {
	ਜੇ (newsub != ਸਰੋਤ)
	    vim_free(newsub);
	ਵਾਪਸ ਸਰੋਤ;
    }

    // Store a copy of newsub  in reg_prev_sub.  It is always allocated,
    // because recursive calls may make the returned string invalid.
    // Only store it if there something to store.
    newsublen = p - newsub;
    ਜੇ (newsublen == 0)
	VIM_CLEAR(reg_prev_sub);
    ਨਹੀਂਤਾਂ
    {
	vim_free(reg_prev_sub);
	reg_prev_sub = vim_strnsave(newsub, newsublen);
    }

    ਜੇ (reg_prev_sub == ਨਿਰਾ)
	reg_prev_sublen = 0;
    ਨਹੀਂਤਾਂ
	reg_prev_sublen = newsublen;

    ਵਾਪਸ newsub;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL

/*
 * Put the submatches in "argv[argskip]" which is a list passed into
 * call_func() by vim_regsub_both().
 */
    ਸਥਿਰ ਪੂਰਨ
fill_submatch_list(ਪੂਰਨ argc ਅਣਵਰਤਿਆ, typval_T *argv, ਪੂਰਨ argskip, ufunc_T *fp)
{
    listitem_T	*li;
    ਪੂਰਨ		i;
    char_u	*s;
    typval_T	*listarg = argv + argskip;

    ਜੇ (!has_varargs(fp) && fp->uf_ਦਲੀਲਾਂ.ga_len <= argskip)
	// called function doesn't take a submatches argument
	ਵਾਪਸ argskip;

    // Relies on sl_list to be the first item in staticList10_T.
    init_static_list((staticList10_T *)(listarg->vval.v_list));

    // There are always 10 list items in staticList10_T.
    li = listarg->vval.v_list->lv_ਪਹਿਲਾ;
    ਲਈ (i = 0; i < 10; ++i)
    {
	s = rsm.sm_ਮੇਲ->startp[i];
	ਜੇ (s == ਨਿਰਾ || rsm.sm_ਮੇਲ->endp[i] == ਨਿਰਾ)
	    s = ਨਿਰਾ;
	ਨਹੀਂਤਾਂ
	    s = vim_strnsave(s, rsm.sm_ਮੇਲ->endp[i] - s);
	li->li_tv.v_type = VAR_STRING;
	li->li_tv.vval.v_string = s;
	li = li->li_ਅਗਲਾ;
    }
    ਵਾਪਸ argskip + 1;
}

    ਸਥਿਰ ਖਾਲੀ
clear_submatch_list(staticList10_T *sl)
{
    ਪੂਰਨ i;

    ਲਈ (i = 0; i < 10; ++i)
	vim_free(sl->sl_ਚੀਜ਼ਾਂ[i].li_tv.vval.v_string);
}
#ਜੇ_ਅੰਤ

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
    ਪੂਰਨ
vim_regsub(
    regmatch_T	*rmp,
    char_u	*ਸਰੋਤ,
    typval_T	*ਵਿਅੰਜਕ,
    char_u	*dest,
    ਪੂਰਨ		destlen,
    ਪੂਰਨ		flags)
{
    ਪੂਰਨ		ਨਤੀਜਾ;
    regexec_T	rex_save;
    ਪੂਰਨ		rex_ਵਿੱਚ_use_save = rex_ਵਿੱਚ_use;

    ਜੇ (rex_ਵਿੱਚ_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_ਵਿੱਚ_use = TRUE;

    rex.reg_ਮੇਲ = rmp;
    rex.reg_mmatch = ਨਿਰਾ;
    rex.reg_maxline = 0;
    rex.reg_buf = curbuf;
    rex.reg_ਸਤਰ_lbr = TRUE;
    ਨਤੀਜਾ = vim_regsub_both(ਸਰੋਤ, ਵਿਅੰਜਕ, dest, destlen, flags);

    rex_ਵਿੱਚ_use = rex_ਵਿੱਚ_use_save;
    ਜੇ (rex_ਵਿੱਚ_use)
	rex = rex_save;

    ਵਾਪਸ ਨਤੀਜਾ;
}

    ਪੂਰਨ
vim_regsub_multi(
    regmmatch_T	*rmp,
    linenr_T	lnum,
    char_u	*ਸਰੋਤ,
    char_u	*dest,
    ਪੂਰਨ		destlen,
    ਪੂਰਨ		flags)
{
    ਪੂਰਨ		ਨਤੀਜਾ;
    regexec_T	rex_save;
    ਪੂਰਨ		rex_ਵਿੱਚ_use_save = rex_ਵਿੱਚ_use;

    ਜੇ (rex_ਵਿੱਚ_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_ਵਿੱਚ_use = TRUE;

    rex.reg_ਮੇਲ = ਨਿਰਾ;
    rex.reg_mmatch = rmp;
    rex.reg_buf = curbuf;	// always works on the current buffer!
    rex.reg_firstlnum = lnum;
    rex.reg_maxline = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - lnum;
    rex.reg_ਸਤਰ_lbr = FALSE;
    ਨਤੀਜਾ = vim_regsub_both(ਸਰੋਤ, ਨਿਰਾ, dest, destlen, flags);

    rex_ਵਿੱਚ_use = rex_ਵਿੱਚ_use_save;
    ਜੇ (rex_ਵਿੱਚ_use)
	rex = rex_save;

    ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ defined(FEAT_EVAL)
// When nesting more than a couple levels it's probably a mistake.
# define MAX_REGSUB_NESTING 4
ਸਥਿਰ char_u   *eval_ਨਤੀਜਾ[MAX_REGSUB_NESTING] = {ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ};

# ਜੇ defined(EXITFREE)
    ਖਾਲੀ
free_resub_eval_ਨਤੀਜਾ(ਖਾਲੀ)
{
    ਪੂਰਨ i;

    ਲਈ (i = 0; i < MAX_REGSUB_NESTING; ++i)
	VIM_CLEAR(eval_ਨਤੀਜਾ[i]);
}
# endif
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਪੂਰਨ
vim_regsub_both(
    char_u	*ਸਰੋਤ,
    typval_T	*ਵਿਅੰਜਕ,
    char_u	*dest,
    ਪੂਰਨ		destlen,
    ਪੂਰਨ		flags)
{
    char_u	*src;
    char_u	*dst;
    char_u	*s;
    ਪੂਰਨ		c;
    ਪੂਰਨ		cc;
    ਪੂਰਨ		no = -1;
    fptr_T	func_all = (fptr_T)ਨਿਰਾ;
    fptr_T	func_one = (fptr_T)ਨਿਰਾ;
    linenr_T	clnum = 0;	// init for GCC
    ਪੂਰਨ		len = 0;	// init for GCC
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਸਥਿਰ ਪੂਰਨ  nesting = 0;
    ਪੂਰਨ		nested;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		copy = flags & REGSUB_COPY;

    // Be paranoid...
    ਜੇ ((ਸਰੋਤ == ਨਿਰਾ && ਵਿਅੰਜਕ == ਨਿਰਾ) || dest == ਨਿਰਾ)
    {
	iemsg(e_null_ਦਲੀਲ);
	ਵਾਪਸ 0;
    }
    ਜੇ (prog_magic_wrong())
	ਵਾਪਸ 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (nesting == MAX_REGSUB_NESTING)
    {
	emsg(_(e_substitute_nesting_too_deep));
	ਵਾਪਸ 0;
    }
    nested = nesting;
#ਜੇ_ਅੰਤ
    src = ਸਰੋਤ;
    dst = dest;

    /*
     * When the substitute part starts with "\=" evaluate it as an expression.
     */
    ਜੇ (ਵਿਅੰਜਕ != ਨਿਰਾ || (ਸਰੋਤ[0] == '\\' && ਸਰੋਤ[1] == '='))
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	// To make sure that the length doesn't change between checking the
	// length and copying the string, and to speed up things, the
	// resulting string is saved from the call with
	// "flags & REGSUB_COPY" == 0 to the call with
	// "flags & REGSUB_COPY" != 0.
	ਜੇ (copy)
	{
	    ਜੇ (eval_ਨਤੀਜਾ[nested] != ਨਿਰਾ)
	    {
		ਪੂਰਨ eval_len = (ਪੂਰਨ)STRLEN(eval_ਨਤੀਜਾ[nested]);

		ਜੇ (eval_len < destlen)
		{
		    STRCPY(dest, eval_ਨਤੀਜਾ[nested]);
		    dst += eval_len;
		    VIM_CLEAR(eval_ਨਤੀਜਾ[nested]);
		}
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ		    prev_can_f_submatch = can_f_submatch;
	    regsubmatch_T   rsm_save;

	    VIM_CLEAR(eval_ਨਤੀਜਾ[nested]);

	    // The expression may contain substitute(), which calls us
	    // recursively.  Make sure submatch() gets the text from the first
	    // level.
	    ਜੇ (can_f_submatch)
		rsm_save = rsm;
	    can_f_submatch = TRUE;
	    rsm.sm_ਮੇਲ = rex.reg_ਮੇਲ;
	    rsm.sm_mmatch = rex.reg_mmatch;
	    rsm.sm_firstlnum = rex.reg_firstlnum;
	    rsm.sm_maxline = rex.reg_maxline;
	    rsm.sm_ਸਤਰ_lbr = rex.reg_ਸਤਰ_lbr;

	    // Although unlikely, it is possible that the expression invokes a
	    // substitute command (it might fail, but still).  Therefore keep
	    // an array of eval results.
	    ++nesting;

	    ਜੇ (ਵਿਅੰਜਕ != ਨਿਰਾ)
	    {
		typval_T	argv[2];
		char_u		buf[NUMBUFLEN];
		typval_T	rettv;
		staticList10_T	ਮੇਲ_List;
		funcexe_T	funcexe;

		rettv.v_type = VAR_STRING;
		rettv.vval.v_string = ਨਿਰਾ;
		argv[0].v_type = VAR_LIST;
		argv[0].vval.v_list = &ਮੇਲ_List.sl_list;
		ਮੇਲ_List.sl_list.lv_len = 0;
		CLEAR_ਖੇਤਰ(funcexe);
		funcexe.fe_argv_func = fill_submatch_list;
		funcexe.fe_evaluate = TRUE;
		ਜੇ (ਵਿਅੰਜਕ->v_type == VAR_FUNC)
		{
		    s = ਵਿਅੰਜਕ->vval.v_string;
		    call_func(s, -1, &rettv, 1, argv, &funcexe);
		}
		ਨਹੀਂਤਾਂ ਜੇ (ਵਿਅੰਜਕ->v_type == VAR_PARTIAL)
		{
		    partial_T   *partial = ਵਿਅੰਜਕ->vval.v_partial;

		    s = partial_name(partial);
		    funcexe.fe_partial = partial;
		    call_func(s, -1, &rettv, 1, argv, &funcexe);
		}
		ਨਹੀਂਤਾਂ ਜੇ (ਵਿਅੰਜਕ->v_type == VAR_INSTR)
		{
		    exe_typval_instr(ਵਿਅੰਜਕ, &rettv);
		}
		ਜੇ (ਮੇਲ_List.sl_list.lv_len > 0)
		    // fill_submatch_list() was called
		    clear_submatch_list(&ਮੇਲ_List);

		ਜੇ (rettv.v_type == VAR_UNKNOWN)
		    // something failed, no need to report another error
		    eval_ਨਤੀਜਾ[nested] = ਨਿਰਾ;
		ਨਹੀਂਤਾਂ
		{
		    eval_ਨਤੀਜਾ[nested] = tv_get_string_buf_chk(&rettv, buf);
		    ਜੇ (eval_ਨਤੀਜਾ[nested] != ਨਿਰਾ)
			eval_ਨਤੀਜਾ[nested] = vim_strsave(eval_ਨਤੀਜਾ[nested]);
		}
		clear_tv(&rettv);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (substitute_instr != ਨਿਰਾ)
		// Execute instructions from ISN_SUBSTITUTE.
		eval_ਨਤੀਜਾ[nested] = exe_substitute_instr();
	    ਨਹੀਂਤਾਂ
		eval_ਨਤੀਜਾ[nested] = eval_ਮੰਜ਼ਿਲ_string(ਸਰੋਤ + 2, TRUE, FALSE);
	    --nesting;

	    ਜੇ (eval_ਨਤੀਜਾ[nested] != ਨਿਰਾ)
	    {
		ਪੂਰਨ had_backslash = FALSE;

		ਲਈ (s = eval_ਨਤੀਜਾ[nested]; *s != NUL; MB_PTR_ADV(s))
		{
		    // Change NL to CR, so that it becomes a line break,
		    // unless called from vim_regexec_nl().
		    // Skip over a backslashed character.
		    ਜੇ (*s == NL && !rsm.sm_ਸਤਰ_lbr)
			*s = CAR;
		    ਨਹੀਂਤਾਂ ਜੇ (*s == '\\' && s[1] != NUL)
		    {
			++s;
			/* Change NL to CR here too, so that this works:
			 * :s/abc\\\ndef/\="aaa\\\nbbb"/  on text:
			 *   abc\
			 *   def
			 * Not when called from vim_regexec_nl().
			 */
			ਜੇ (*s == NL && !rsm.sm_ਸਤਰ_lbr)
			    *s = CAR;
			had_backslash = TRUE;
		    }
		}
		ਜੇ (had_backslash && (flags & REGSUB_BACKSLASH))
		{
		    // Backslashes will be consumed, need to double them.
		    s = vim_strsave_escaped(eval_ਨਤੀਜਾ[nested], (char_u *)"\\");
		    ਜੇ (s != ਨਿਰਾ)
		    {
			vim_free(eval_ਨਤੀਜਾ[nested]);
			eval_ਨਤੀਜਾ[nested] = s;
		    }
		}

		dst += STRLEN(eval_ਨਤੀਜਾ[nested]);
	    }

	    can_f_submatch = prev_can_f_submatch;
	    ਜੇ (can_f_submatch)
		rsm = rsm_save;
	}
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
      ਜਦੋਂ ((c = *src++) != NUL)
      {
	ਜੇ (c == '&' && (flags & REGSUB_MAGIC))
	    no = 0;
	ਨਹੀਂਤਾਂ ਜੇ (c == '\\' && *src != NUL)
	{
	    ਜੇ (*src == '&' && !(flags & REGSUB_MAGIC))
	    {
		++src;
		no = 0;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ ('0' <= *src && *src <= '9')
	    {
		no = *src++ - '0';
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (vim_strchr((char_u *)"uUlLeE", *src))
	    {
		ਚੋਣ (*src++)
		{
		ਹਾਲਤ 'u':   func_one = do_ਉੱਤਲਾ;
			    ਜਾਰੀ;
		ਹਾਲਤ 'U':   func_all = do_ਉੱਤਲਾ;
			    ਜਾਰੀ;
		ਹਾਲਤ 'l':   func_one = do_ਹੇਠਲਾ;
			    ਜਾਰੀ;
		ਹਾਲਤ 'L':   func_all = do_ਹੇਠਲਾ;
			    ਜਾਰੀ;
		ਹਾਲਤ 'e':
		ਹਾਲਤ 'E':   func_one = func_all = (fptr_T)ਨਿਰਾ;
			    ਜਾਰੀ;
		}
	    }
	}
	ਜੇ (no < 0)	      // Ordinary character.
	{
	    ਜੇ (c == K_SPECIAL && src[0] != NUL && src[1] != NUL)
	    {
		// Copy a special key as-is.
		ਜੇ (copy)
		{
		    ਜੇ (dst + 3 > dest + destlen)
		    {
			iemsg("vim_regsub_both(): not enough space");
			ਵਾਪਸ 0;
		    }
		    *dst++ = c;
		    *dst++ = *src++;
		    *dst++ = *src++;
		}
		ਨਹੀਂਤਾਂ
		{
		    dst += 3;
		    src += 2;
		}
		ਜਾਰੀ;
	    }

	    ਜੇ (c == '\\' && *src != NUL)
	    {
		// Check for abbreviations -- webb
		ਚੋਣ (*src)
		{
		    ਹਾਲਤ 'r':	c = CAR;	++src;	ਰੋਕੋ;
		    ਹਾਲਤ 'n':	c = NL;		++src;	ਰੋਕੋ;
		    ਹਾਲਤ 't':	c = TAB;	++src;	ਰੋਕੋ;
		 // Oh no!  \e already has meaning in subst pat :-(
		 // case 'e':   c = ESC;	++src;	break;
		    ਹਾਲਤ 'b':	c = Ctrl_H;	++src;	ਰੋਕੋ;

		    // If "backslash" is TRUE the backslash will be removed
		    // later.  Used to insert a literal CR.
		    ਮੂਲ:	ਜੇ (flags & REGSUB_BACKSLASH)
				{
				    ਜੇ (copy)
				    {
					ਜੇ (dst + 1 > dest + destlen)
					{
					    iemsg("vim_regsub_both(): not enough space");
					    ਵਾਪਸ 0;
					}
					*dst = '\\';
				    }
				    ++dst;
				}
				c = *src++;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (has_mbyte)
		c = mb_ptr2char(src - 1);

	    // Write to buffer, if copy is set.
	    ਜੇ (func_one != (fptr_T)ਨਿਰਾ)
	    {
		func_one(&cc, c);
		func_one = ਨਿਰਾ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (func_all != (fptr_T)ਨਿਰਾ)
		func_all(&cc, c);
	    ਨਹੀਂਤਾਂ // just copy
		cc = c;

	    ਜੇ (has_mbyte)
	    {
		ਪੂਰਨ totlen = mb_ptr2len(src - 1);
		ਪੂਰਨ charlen = mb_char2len(cc);

		ਜੇ (copy)
		{
		    ਜੇ (dst + charlen > dest + destlen)
		    {
			iemsg("vim_regsub_both(): not enough space");
			ਵਾਪਸ 0;
		    }
		    mb_char2bytes(cc, dst);
		}
		dst += charlen - 1;
		ਜੇ (enc_utf8)
		{
		    ਪੂਰਨ clen = utf_ptr2len(src - 1);

		    // If the character length is shorter than "totlen", there
		    // are composing characters; copy them as-is.
		    ਜੇ (clen < totlen)
		    {
			ਜੇ (copy)
			{
			    ਜੇ (dst + totlen - clen > dest + destlen)
			    {
				iemsg("vim_regsub_both(): not enough space");
				ਵਾਪਸ 0;
			    }
			    mch_memmove(dst + 1, src - 1 + clen,
						     (size_t)(totlen - clen));
			}
			dst += totlen - clen;
		    }
		}
		src += totlen - 1;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (copy)
	    {
		ਜੇ (dst + 1 > dest + destlen)
		{
		    iemsg("vim_regsub_both(): not enough space");
		    ਵਾਪਸ 0;
		}
		*dst = cc;
	    }
	    dst++;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (REG_MULTI)
	    {
		clnum = rex.reg_mmatch->startpos[no].lnum;
		ਜੇ (clnum < 0 || rex.reg_mmatch->endpos[no].lnum < 0)
		    s = ਨਿਰਾ;
		ਨਹੀਂਤਾਂ
		{
		    s = reg_getline(clnum) + rex.reg_mmatch->startpos[no].ਖਾਨਾ;
		    ਜੇ (rex.reg_mmatch->endpos[no].lnum == clnum)
			len = rex.reg_mmatch->endpos[no].ਖਾਨਾ
					    - rex.reg_mmatch->startpos[no].ਖਾਨਾ;
		    ਨਹੀਂਤਾਂ
			len = (ਪੂਰਨ)reg_getline_len(clnum) - rex.reg_mmatch->startpos[no].ਖਾਨਾ;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		s = rex.reg_ਮੇਲ->startp[no];
		ਜੇ (rex.reg_ਮੇਲ->endp[no] == ਨਿਰਾ)
		    s = ਨਿਰਾ;
		ਨਹੀਂਤਾਂ
		    len = (ਪੂਰਨ)(rex.reg_ਮੇਲ->endp[no] - s);
	    }
	    ਜੇ (s != ਨਿਰਾ)
	    {
		ਲਈ (;;)
		{
		    ਜੇ (len == 0)
		    {
			ਜੇ (REG_MULTI)
			{
			    ਜੇ (rex.reg_mmatch->endpos[no].lnum == clnum)
				ਰੋਕੋ;
			    ਜੇ (copy)
			    {
				ਜੇ (dst + 1 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    ਵਾਪਸ 0;
				}
				*dst = CAR;
			    }
			    ++dst;
			    s = reg_getline(++clnum);
			    ਜੇ (rex.reg_mmatch->endpos[no].lnum == clnum)
				len = rex.reg_mmatch->endpos[no].ਖਾਨਾ;
			    ਨਹੀਂਤਾਂ
				len = (ਪੂਰਨ)reg_getline_len(clnum);
			}
			ਨਹੀਂਤਾਂ
			    ਰੋਕੋ;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (*s == NUL) // we hit NUL.
		    {
			ਜੇ (copy)
			    iemsg(e_damaged_ਮੇਲ_string);
			ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ ((flags & REGSUB_BACKSLASH)
						  && (*s == CAR || *s == '\\'))
			{
			    /*
			     * Insert a backslash in front of a CR, otherwise
			     * it will be replaced by a line break.
			     * Number of backslashes will be halved later,
			     * double them here.
			     */
			    ਜੇ (copy)
			    {
				ਜੇ (dst + 2 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    ਵਾਪਸ 0;
				}
				dst[0] = '\\';
				dst[1] = *s;
			    }
			    dst += 2;
			}
			ਨਹੀਂਤਾਂ
			{
			    ਜੇ (has_mbyte)
				c = mb_ptr2char(s);
			    ਨਹੀਂਤਾਂ
				c = *s;

			    ਜੇ (func_one != (fptr_T)ਨਿਰਾ)
			    {
				func_one(&cc, c);
				func_one = ਨਿਰਾ;
			    }
			    ਨਹੀਂਤਾਂ ਜੇ (func_all != (fptr_T)ਨਿਰਾ)
				func_all(&cc, c);
			    ਨਹੀਂਤਾਂ // just copy
				cc = c;

			    ਜੇ (has_mbyte)
			    {
				ਪੂਰਨ l;
				ਪੂਰਨ charlen;

				// Copy composing characters separately, one
				// at a time.
				ਜੇ (enc_utf8)
				    l = utf_ptr2len(s) - 1;
				ਨਹੀਂਤਾਂ
				    l = mb_ptr2len(s) - 1;

				s += l;
				len -= l;
				charlen = mb_char2len(cc);
				ਜੇ (copy)
				{
				    ਜੇ (dst + charlen > dest + destlen)
				    {
					iemsg("vim_regsub_both(): not enough space");
					ਵਾਪਸ 0;
				    }
				    mb_char2bytes(cc, dst);
				}
				dst += charlen - 1;
			    }
			    ਨਹੀਂਤਾਂ ਜੇ (copy)
			    {
				ਜੇ (dst + 1 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    ਵਾਪਸ 0;
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
    ਜੇ (copy)
	*dst = NUL;

ਬਾਹਰ_ਨਿਕਲੋ:
    ਵਾਪਸ (ਪੂਰਨ)((dst - dest) + 1);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL

    ਸਥਿਰ char_u *
reg_getline_submatch(linenr_T lnum)
{
    char_u *ਸਤਰ;

    reg_getline_common(lnum, RGLF_ਸਤਰ | RGLF_SUBMATCH, &ਸਤਰ, ਨਿਰਾ);

    ਵਾਪਸ ਸਤਰ;
}

    ਸਥਿਰ colnr_T
reg_getline_submatch_len(linenr_T lnum)
{
    colnr_T ਲੰਮਾਈ;

    reg_getline_common(lnum, RGLF_ਲੰਮਾਈ | RGLF_SUBMATCH, ਨਿਰਾ, &ਲੰਮਾਈ);

    ਵਾਪਸ ਲੰਮਾਈ;
}

/*
 * Used for the submatch() function: get the string from the n'th submatch in
 * allocated memory.
 * Returns NULL when not in a ":s" command and for a non-existing submatch.
 */
    char_u *
reg_submatch(ਪੂਰਨ no)
{
    char_u	*retval = ਨਿਰਾ;
    char_u	*s;
    ਪੂਰਨ		len;
    ਪੂਰਨ		ਗੇੜਾ;
    linenr_T	lnum;

    ਜੇ (!can_f_submatch || no < 0)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (rsm.sm_ਮੇਲ == ਨਿਰਾ)
    {
	/*
	 * First round: compute the length and allocate memory.
	 * Second round: copy the text.
	 */
	ਲਈ (ਗੇੜਾ = 1; ਗੇੜਾ <= 2; ++ਗੇੜਾ)
	{
	    lnum = rsm.sm_mmatch->startpos[no].lnum;
	    ਜੇ (lnum < 0 || rsm.sm_mmatch->endpos[no].lnum < 0)
		ਵਾਪਸ ਨਿਰਾ;

	    s = reg_getline_submatch(lnum);
	    ਜੇ (s == ਨਿਰਾ)  // anti-crash check, cannot happen?
		ਰੋਕੋ;
	    s += rsm.sm_mmatch->startpos[no].ਖਾਨਾ;
	    ਜੇ (rsm.sm_mmatch->endpos[no].lnum == lnum)
	    {
		// Within one line: take form start to end col.
		len = rsm.sm_mmatch->endpos[no].ਖਾਨਾ
					  - rsm.sm_mmatch->startpos[no].ਖਾਨਾ;
		ਜੇ (ਗੇੜਾ == 2)
		    vim_strncpy(retval, s, len);
		++len;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Multiple lines: take start line from start col, middle
		// lines completely and end line up to end col.
		len = (ਪੂਰਨ)reg_getline_submatch_len(lnum) - rsm.sm_mmatch->startpos[no].ਖਾਨਾ;
		ਜੇ (ਗੇੜਾ == 2)
		{
		    STRCPY(retval, s);
		    retval[len] = '\n';
		}
		++len;
		++lnum;
		ਜਦੋਂ (lnum < rsm.sm_mmatch->endpos[no].lnum)
		{
		    s = reg_getline_submatch(lnum);
		    ਜੇ (ਗੇੜਾ == 2)
			STRCPY(retval + len, s);
		    len += (ਪੂਰਨ)reg_getline_submatch_len(lnum);
		    ਜੇ (ਗੇੜਾ == 2)
			retval[len] = '\n';
		    ++len;
		    ++lnum;
		}
		ਜੇ (ਗੇੜਾ == 2)
		    STRNCPY(retval + len, reg_getline_submatch(lnum),
					     rsm.sm_mmatch->endpos[no].ਖਾਨਾ);
		len += rsm.sm_mmatch->endpos[no].ਖਾਨਾ;
		ਜੇ (ਗੇੜਾ == 2)
		    retval[len] = NUL;
		++len;
	    }

	    ਜੇ (retval == ਨਿਰਾ)
	    {
		retval = alloc(len);
		ਜੇ (retval == ਨਿਰਾ)
		    ਵਾਪਸ ਨਿਰਾ;
	    }
	}
    }
    ਨਹੀਂਤਾਂ
    {
	s = rsm.sm_ਮੇਲ->startp[no];
	ਜੇ (s == ਨਿਰਾ || rsm.sm_ਮੇਲ->endp[no] == ਨਿਰਾ)
	    retval = ਨਿਰਾ;
	ਨਹੀਂਤਾਂ
	    retval = vim_strnsave(s, rsm.sm_ਮੇਲ->endp[no] - s);
    }

    ਵਾਪਸ retval;
}

/*
 * Used for the submatch() function with the optional non-zero argument: get
 * the list of strings from the n'th submatch in allocated memory with NULs
 * represented in NLs.
 * Returns a list of allocated strings.  Returns NULL when not in a ":s"
 * command, for a non-existing submatch and for any error.
 */
    list_T *
reg_submatch_list(ਪੂਰਨ no)
{
    char_u	*s;
    linenr_T	slnum;
    linenr_T	elnum;
    colnr_T	scol;
    colnr_T	ecol;
    ਪੂਰਨ		i;
    list_T	*list;
    ਪੂਰਨ		error = FALSE;

    ਜੇ (!can_f_submatch || no < 0)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (rsm.sm_ਮੇਲ == ਨਿਰਾ)
    {
	slnum = rsm.sm_mmatch->startpos[no].lnum;
	elnum = rsm.sm_mmatch->endpos[no].lnum;
	ਜੇ (slnum < 0 || elnum < 0)
	    ਵਾਪਸ ਨਿਰਾ;

	scol = rsm.sm_mmatch->startpos[no].ਖਾਨਾ;
	ecol = rsm.sm_mmatch->endpos[no].ਖਾਨਾ;

	list = list_alloc();
	ਜੇ (list == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;

	s = reg_getline_submatch(slnum) + scol;
	ਜੇ (slnum == elnum)
	{
	    ਜੇ (list_append_string(list, s, ecol - scol) == FAIL)
		error = TRUE;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ max_lnum = elnum - slnum;

	    ਜੇ (list_append_string(list, s, -1) == FAIL)
		error = TRUE;
	    ਲਈ (i = 1; i < max_lnum; i++)
	    {
		s = reg_getline_submatch(slnum + i);
		ਜੇ (list_append_string(list, s, -1) == FAIL)
		    error = TRUE;
	    }
	    s = reg_getline_submatch(elnum);
	    ਜੇ (list_append_string(list, s, ecol) == FAIL)
		error = TRUE;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	s = rsm.sm_ਮੇਲ->startp[no];
	ਜੇ (s == ਨਿਰਾ || rsm.sm_ਮੇਲ->endp[no] == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	list = list_alloc();
	ਜੇ (list == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	ਜੇ (list_append_string(list, s,
				 (ਪੂਰਨ)(rsm.sm_ਮੇਲ->endp[no] - s)) == FAIL)
	    error = TRUE;
    }

    ਜੇ (error)
    {
	list_free(list);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ++list->lv_refcount;
    ਵਾਪਸ list;
}
#ਜੇ_ਅੰਤ

/*
 * Initialize the values used for matching against multiple lines
 */
    ਸਥਿਰ ਖਾਲੀ
init_regexec_multi(
	regmmatch_T	*rmp,
	win_T		*win,	// window in which to search or NULL
	buf_T		*buf,	// buffer in which to search
	linenr_T	lnum)	// nr of line to start looking for match
{
    rex.reg_ਮੇਲ = ਨਿਰਾ;
    rex.reg_mmatch = rmp;
    rex.reg_buf = buf;
    rex.reg_win = win;
    rex.reg_firstlnum = lnum;
    rex.reg_maxline = rex.reg_buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - lnum;
    rex.reg_ਸਤਰ_lbr = FALSE;
    rex.reg_ic = rmp->rmm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = rmp->rmm_maxcol;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLASS_not		0x80
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLASS_af		0x40
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLASS_AF		0x20
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLASS_az		0x10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLASS_AZ		0x08
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLASS_o7		0x04
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLASS_o9		0x02
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLASS_underscore	0x01

/*
 * Parse the "[...]" collection between "start" (the first character after the
 * "[") and "end" (the "]") into a combination of CLASS_ bits, so that it can
 * be turned into a faster character class like \d or \x.  Sets "*newl" to TRUE
 * when the collection also matches a newline.  Returns -1 when the collection
 * is not a plain combination of the recognized ranges.
 */
    ਸਥਿਰ ਪੂਰਨ
get_char_class_ਬਿੱਟ(char_u *ਸ਼ੁਰੂ, char_u *ਸਿਰਾ, ਪੂਰਨ *newl)
{
    char_u	*p = ਸ਼ੁਰੂ;
    ਪੂਰਨ		config = 0;

    *newl = FALSE;
    ਜੇ (*ਸਿਰਾ != ']')
	ਵਾਪਸ -1;
    ਜੇ (*p == '^')
    {
	config |= CLASS_not;
	p++;
    }

    ਜਦੋਂ (p < ਸਿਰਾ)
    {
	ਜੇ (p + 2 < ਸਿਰਾ && *(p + 1) == '-')
	{
	    ਚੋਣ (*p)
	    {
		ਹਾਲਤ '0':
		    ਜੇ (*(p + 2) == '9')
		    {
			config |= CLASS_o9;
			ਰੋਕੋ;
		    }
		    ਜੇ (*(p + 2) == '7')
		    {
			config |= CLASS_o7;
			ਰੋਕੋ;
		    }
		    ਵਾਪਸ -1;

		ਹਾਲਤ 'a':
		    ਜੇ (*(p + 2) == 'z')
		    {
			config |= CLASS_az;
			ਰੋਕੋ;
		    }
		    ਜੇ (*(p + 2) == 'f')
		    {
			config |= CLASS_af;
			ਰੋਕੋ;
		    }
		    ਵਾਪਸ -1;

		ਹਾਲਤ 'A':
		    ਜੇ (*(p + 2) == 'Z')
		    {
			config |= CLASS_AZ;
			ਰੋਕੋ;
		    }
		    ਜੇ (*(p + 2) == 'F')
		    {
			config |= CLASS_AF;
			ਰੋਕੋ;
		    }
		    ਵਾਪਸ -1;

		ਮੂਲ:
		    ਵਾਪਸ -1;
	    }
	    p += 3;
	}
	ਨਹੀਂਤਾਂ ਜੇ (p + 1 < ਸਿਰਾ && *p == '\\' && *(p + 1) == 'n')
	{
	    *newl = TRUE;
	    p += 2;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p == '_')
	{
	    config |= CLASS_underscore;
	    p++;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p == '\n')
	{
	    *newl = TRUE;
	    p++;
	}
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ -1;
    } // while (p < end)

    ਜੇ (p != ਸਿਰਾ)
	ਵਾਪਸ -1;

    // U+017F and U+212A fold to ASCII 's' and 'k', missed by the class opcodes
    ਜੇ (enc_utf8 && (config & (CLASS_az | CLASS_AZ)))
	ਵਾਪਸ -1;

    ਵਾਪਸ config;
}

#ਅਨ੍ਤਰ੍ਭਾਵ "regexp_bt.c"

ਸਥਿਰ regengine_T bt_regengine =
{
    bt_regcomp,
    bt_regfree,
    bt_regexec_nl,
    bt_regexec_multi
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
    ,(char_u *)""
#ਜੇ_ਅੰਤ
};

#ਅਨ੍ਤਰ੍ਭਾਵ "regexp_nfa.c"

ਸਥਿਰ regengine_T nfa_regengine =
{
    nfa_regcomp,
    nfa_regfree,
    nfa_regexec_nl,
    nfa_regexec_multi
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
    ,(char_u *)""
#ਜੇ_ਅੰਤ
};

// Which regexp engine to use? Needed for vim_regcomp().
// Must match with 'regexpengine'.
ਸਥਿਰ ਪੂਰਨ regexp_engine = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
ਸਥਿਰ ਅਚਲ char_u regname[][30] = {
		    "AUTOMATIC Regexp Engine",
		    "BACKTRACKING Regexp Engine",
		    "NFA Regexp Engine"
			    };
#ਜੇ_ਅੰਤ

/*
 * Compile a regular expression into internal code.
 * Returns the program in allocated memory.
 * Use vim_regfree() to free the memory.
 * Returns NULL for an error.
 */
    regprog_T *
vim_regcomp(char_u *ਵਿਅੰਜਕ_arg, ਪੂਰਨ re_flags)
{
    regprog_T   *prog = ਨਿਰਾ;
    char_u	*ਵਿਅੰਜਕ = ਵਿਅੰਜਕ_arg;
    ਪੂਰਨ		called_emsg_before;

    regexp_engine = p_re;

    // Check for prefix "\%#=", that sets the regexp engine
    ਜੇ (STRNCMP(ਵਿਅੰਜਕ, "\\%#=", 4) == 0)
    {
	ਪੂਰਨ newengine = ਵਿਅੰਜਕ[4] - '0';

	ਜੇ (newengine == AUTOMATIC_ENGINE
	    || newengine == BACKTRACKING_ENGINE
	    || newengine == NFA_ENGINE)
	{
	    regexp_engine = ਵਿਅੰਜਕ[4] - '0';
	    ਵਿਅੰਜਕ += 5;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
	    smsg("New regexp mode selected (%d): %s",
					   regexp_engine, regname[newengine]);
#ਜੇ_ਅੰਤ
	}
	ਨਹੀਂਤਾਂ
	{
	    emsg(_(e_percent_hash_can_only_be_followed_by_zero_one_two_automatic_engine_will_be_ਵਰਤਿਆ));
	    regexp_engine = AUTOMATIC_ENGINE;
	}
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
    bt_regengine.ਵਿਅੰਜਕ = ਵਿਅੰਜਕ;
    nfa_regengine.ਵਿਅੰਜਕ = ਵਿਅੰਜਕ;
#ਜੇ_ਅੰਤ
    // reg_iswordc() uses rex.reg_buf
    rex.reg_buf = curbuf;

    /*
     * First try the NFA engine, unless backtracking was requested.
     */
    called_emsg_before = called_emsg;
    ਜੇ (regexp_engine != BACKTRACKING_ENGINE)
	prog = nfa_regengine.regcomp(ਵਿਅੰਜਕ,
		re_flags + (regexp_engine == AUTOMATIC_ENGINE ? RE_AUTO : 0));
    ਨਹੀਂਤਾਂ
	prog = bt_regengine.regcomp(ਵਿਅੰਜਕ, re_flags);

    // Check for error compiling regexp with initial engine.
    ਜੇ (prog == ਨਿਰਾ)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BT_REGEXP_DEBUG_LOG
	ਜੇ (regexp_engine == BACKTRACKING_ENGINE)   // debugging log for BT engine
	{
	    FILE *f;
	    f = fopen(BT_REGEXP_DEBUG_LOG_NAME, "a");
	    ਜੇ (f)
	    {
		fprintf(f, "Syntax error in \"%s\"\n", ਵਿਅੰਜਕ);
		fclose(f);
	    }
	    ਨਹੀਂਤਾਂ
		semsg("(NFA) Could not open \"%s\" to write !!!",
			BT_REGEXP_DEBUG_LOG_NAME);
	}
#ਜੇ_ਅੰਤ
	/*
	 * If the NFA engine failed, try the backtracking engine.
	 * The NFA engine also fails for patterns that it can't handle well
	 * but are still valid patterns, thus a retry should work.
	 * But don't try if an error message was given.
	 */
	ਜੇ (regexp_engine == AUTOMATIC_ENGINE
					  && called_emsg == called_emsg_before)
	{
	    regexp_engine = BACKTRACKING_ENGINE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    report_re_switch(ਵਿਅੰਜਕ);
#ਜੇ_ਅੰਤ
	    prog = bt_regengine.regcomp(ਵਿਅੰਜਕ, re_flags);
	}
    }

    ਜੇ (prog != ਨਿਰਾ)
    {
	// Store the info needed to call regcomp() again when the engine turns
	// out to be very slow when executing it.
	prog->re_engine = regexp_engine;
	prog->re_flags  = re_flags;
    }

    ਵਾਪਸ prog;
}

/*
 * Free a compiled regexp program, returned by vim_regcomp().
 */
    ਖਾਲੀ
vim_regfree(regprog_T *prog)
{
    ਜੇ (prog != ਨਿਰਾ)
	prog->engine->regfree(prog);
}

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
free_regexp_stuff(ਖਾਲੀ)
{
    ga_clear(&regstack);
    ga_clear(&backpos);
    vim_free(reg_tofree);
    vim_free(reg_prev_sub);
    vim_free(post_ਸ਼ੁਰੂ);   // NFA postfix buffer, reused across compilations
    post_ਸ਼ੁਰੂ = ਨਿਰਾ;
    post_ਸ਼ੁਰੂ_len = 0;
    vim_free(nfa_ਢੇਰ);    // NFA fragment stack, reused across compilations
    nfa_ਢੇਰ = ਨਿਰਾ;
    nfa_ਢੇਰ_len = 0;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਸਥਿਰ ਖਾਲੀ
report_re_switch(char_u *pat)
{
    ਜੇ (p_verbose > 0)
    {
	verbose_enter();
	ਸੁਨੇਹਾ_puts(_("Switching to backtracking RE engine for pattern: "));
	ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)pat);
	verbose_leave();
    }
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_X11)
/*
 * Return whether "prog" is currently being executed.
 */
    ਪੂਰਨ
regprog_ਵਿੱਚ_use(regprog_T *prog)
{
    ਵਾਪਸ prog->re_ਵਿੱਚ_use;
}
#ਜੇ_ਅੰਤ

/*
 * Match a regexp against a string.
 * "rmp->regprog" must be a compiled regexp as returned by vim_regcomp().
 * Note: "rmp->regprog" may be freed and changed.
 * Uses curbuf for line count and 'iskeyword'.
 * When "nl" is TRUE consider a "\n" in "line" to be a line break.
 *
 * Return TRUE if there is a match, FALSE if not.
 */
    ਸਥਿਰ ਪੂਰਨ
vim_regexec_string(
    regmatch_T	*rmp,
    char_u	*ਸਤਰ,  // string to match against
    colnr_T	ਖਾਨਾ,    // column to start looking for match
    ਪੂਰਨ		nl)
{
    ਪੂਰਨ		ਨਤੀਜਾ;
    regexec_T	rex_save;
    ਪੂਰਨ		rex_ਵਿੱਚ_use_save = rex_ਵਿੱਚ_use;

    // Cannot use the same prog recursively, it contains state.
    ਜੇ (rmp->regprog->re_ਵਿੱਚ_use)
    {
	emsg(_(e_cannot_use_ਨਮੂਨਾ_recursively));
	ਵਾਪਸ FALSE;
    }
    rmp->regprog->re_ਵਿੱਚ_use = TRUE;

    ਜੇ (rex_ਵਿੱਚ_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_ਵਿੱਚ_use = TRUE;

    rex.reg_startp = ਨਿਰਾ;
    rex.reg_endp = ਨਿਰਾ;
    rex.reg_startpos = ਨਿਰਾ;
    rex.reg_endpos = ਨਿਰਾ;

    ਨਤੀਜਾ = rmp->regprog->engine->regexec_nl(rmp, ਸਤਰ, ਖਾਨਾ, nl);
    rmp->regprog->re_ਵਿੱਚ_use = FALSE;

    // NFA engine aborted because it's very slow.
    ਜੇ (rmp->regprog->re_engine == AUTOMATIC_ENGINE
					       && ਨਤੀਜਾ == NFA_TOO_EXPENSIVE)
    {
	ਪੂਰਨ    save_p_re = p_re;
	ਪੂਰਨ    re_flags = rmp->regprog->re_flags;
	char_u *pat = vim_strsave(((nfa_regprog_T *)rmp->regprog)->ਨਮੂਨਾ);

	p_re = BACKTRACKING_ENGINE;
	ਜੇ (pat != ਨਿਰਾ)
	{
	    regprog_T *prev_prog = rmp->regprog;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    report_re_switch(pat);
#ਜੇ_ਅੰਤ
	    rmp->regprog = vim_regcomp(pat, re_flags);
	    ਜੇ (rmp->regprog == ਨਿਰਾ)
	    {
		// Somehow compiling the pattern failed now, put back the
		// previous one to avoid "regprog" becoming NULL.
		rmp->regprog = prev_prog;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		vim_regfree(prev_prog);
		rmp->regprog->re_ਵਿੱਚ_use = TRUE;
		ਨਤੀਜਾ = rmp->regprog->engine->regexec_nl(rmp, ਸਤਰ, ਖਾਨਾ, nl);
		rmp->regprog->re_ਵਿੱਚ_use = FALSE;
	    }
	    vim_free(pat);
	}

	p_re = save_p_re;
    }

    rex_ਵਿੱਚ_use = rex_ਵਿੱਚ_use_save;
    ਜੇ (rex_ਵਿੱਚ_use)
	rex = rex_save;

    ਵਾਪਸ ਨਤੀਜਾ > 0;
}

#ਜੇ defined(FEAT_SPELL) || defined(FEAT_EVAL) || defined(FEAT_X11)
/*
 * Note: "*prog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    ਪੂਰਨ
vim_regexec_prog(
    regprog_T	**prog,
    ਪੂਰਨ		ignore_case,
    char_u	*ਸਤਰ,
    colnr_T	ਖਾਨਾ)
{
    ਪੂਰਨ		r;
    regmatch_T	regmatch;

    regmatch.regprog = *prog;
    regmatch.rm_ic = ignore_case;
    r = vim_regexec_string(&regmatch, ਸਤਰ, ਖਾਨਾ, FALSE);
    *prog = regmatch.regprog;
    ਵਾਪਸ r;
}
#ਜੇ_ਅੰਤ

/*
 * Note: "rmp->regprog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    ਪੂਰਨ
vim_regexec(regmatch_T *rmp, char_u *ਸਤਰ, colnr_T ਖਾਨਾ)
{
    ਵਾਪਸ vim_regexec_string(rmp, ਸਤਰ, ਖਾਨਾ, FALSE);
}

/*
 * Like vim_regexec(), but consider a "\n" in "line" to be a line break.
 * Note: "rmp->regprog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    ਪੂਰਨ
vim_regexec_nl(regmatch_T *rmp, char_u *ਸਤਰ, colnr_T ਖਾਨਾ)
{
    ਵਾਪਸ vim_regexec_string(rmp, ਸਤਰ, ਖਾਨਾ, TRUE);
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
    ਲੰਮਾ
vim_regexec_multi(
    regmmatch_T *rmp,
    win_T       *win,		// window in which to search or NULL
    buf_T       *buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	ਖਾਨਾ,		// column to start looking for match
    ਪੂਰਨ		*timed_ਬਾਹਰਲਾ)	// flag is set when timeout limit reached
{
    ਪੂਰਨ		ਨਤੀਜਾ;
    regexec_T	rex_save;
    ਪੂਰਨ		rex_ਵਿੱਚ_use_save = rex_ਵਿੱਚ_use;

    // Cannot use the same prog recursively, it contains state.
    ਜੇ (rmp->regprog->re_ਵਿੱਚ_use)
    {
	emsg(_(e_cannot_use_ਨਮੂਨਾ_recursively));
	ਵਾਪਸ FALSE;
    }
    rmp->regprog->re_ਵਿੱਚ_use = TRUE;

    ਜੇ (rex_ਵਿੱਚ_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_ਵਿੱਚ_use = TRUE;

    ਨਤੀਜਾ = rmp->regprog->engine->regexec_multi(
				      rmp, win, buf, lnum, ਖਾਨਾ, timed_ਬਾਹਰਲਾ);
    rmp->regprog->re_ਵਿੱਚ_use = FALSE;

    // NFA engine aborted because it's very slow.
    ਜੇ (rmp->regprog->re_engine == AUTOMATIC_ENGINE
					       && ਨਤੀਜਾ == NFA_TOO_EXPENSIVE)
    {
	ਪੂਰਨ    save_p_re = p_re;
	ਪੂਰਨ    re_flags = rmp->regprog->re_flags;
	char_u *pat = vim_strsave(((nfa_regprog_T *)rmp->regprog)->ਨਮੂਨਾ);

	p_re = BACKTRACKING_ENGINE;
	ਜੇ (pat != ਨਿਰਾ)
	{
	    regprog_T *prev_prog = rmp->regprog;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    report_re_switch(pat);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	    // checking for \z misuse was already done when compiling for NFA,
	    // allow all here
	    reg_do_extmatch = REX_ALL;
#ਜੇ_ਅੰਤ
	    rmp->regprog = vim_regcomp(pat, re_flags);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	    reg_do_extmatch = 0;
#ਜੇ_ਅੰਤ
	    ਜੇ (rmp->regprog == ਨਿਰਾ)
	    {
		// Somehow compiling the pattern failed now, put back the
		// previous one to avoid "regprog" becoming NULL.
		rmp->regprog = prev_prog;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		vim_regfree(prev_prog);

		rmp->regprog->re_ਵਿੱਚ_use = TRUE;
		ਨਤੀਜਾ = rmp->regprog->engine->regexec_multi(
				      rmp, win, buf, lnum, ਖਾਨਾ, timed_ਬਾਹਰਲਾ);
		rmp->regprog->re_ਵਿੱਚ_use = FALSE;
	    }
	    vim_free(pat);
	}
	p_re = save_p_re;
    }

    rex_ਵਿੱਚ_use = rex_ਵਿੱਚ_use_save;
    ਜੇ (rex_ਵਿੱਚ_use)
	rex = rex_save;

    ਵਾਪਸ ਨਤੀਜਾ <= 0 ? 0 : ਨਤੀਜਾ;
}
