/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * Handling of regular expressions: vim_regcomp(), vim_regexec(), vim_regsub()
 */

// By default: do not create debugging logs or files related to regular
// expressions, even when compiling with -DDEBUG.
// Uncomment the second line to get the regexp debugging.
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು DEBUG
// #define DEBUG

#ಅನ್ತರ್ಭಾವ "vim.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
// show/save debugging data when BT engine is used
# define BT_REGEXP_DUMP
// save the debugging data to a file instead of displaying it
# define BT_REGEXP_LOG
# define BT_REGEXP_DEBUG_LOG
# define BT_REGEXP_DEBUG_LOG_NAME	"bt_regexp_debug.log"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
ಸ್ಥಿರ sig_atomic_t dummy_timeout_flag = 0;
ಸ್ಥಿರ ಅಸ್ಥಿರ sig_atomic_t *timeout_flag = &dummy_timeout_flag;
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Magic characters have a special meaning, they don't match literally.
 * Magic characters are negative.  This separates them from literal characters
 * (possibly multi-byte).  Only ASCII characters can be Magic.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು Magic(x)	((ಪೂರ್ಣಾಂಕ)(x) - 256)
#ವ್ಯಾಖ್ಯಾನಿಸು un_Magic(x)	((x) + 256)
#ವ್ಯಾಖ್ಯಾನಿಸು is_Magic(x)	((x) < 0)

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
no_Magic(ಪೂರ್ಣಾಂಕ x)
{
    ಆದರೆ (is_Magic(x))
	ಹಿಂತಿರುಗಿ un_Magic(x);
    ಹಿಂತಿರುಗಿ x;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
toggle_Magic(ಪೂರ್ಣಾಂಕ x)
{
    ಆದರೆ (is_Magic(x))
	ಹಿಂತಿರುಗಿ un_Magic(x);
    ಹಿಂತಿರುಗಿ Magic(x);
}

#ಆದರೆ defined(FEAT_RELTIME)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ timeout_nesting = 0;

/*
 * Start a timer that will cause the regexp to abort after "msec".
 * This doesn't work well recursively.  In case it happens anyway, the first
 * set timeout will prevail, nested ones are ignored.
 * The caller must make sure there is a matching disable_regexp_timeout() call!
 */
    ಶೂನ್ಯ
init_regexp_timeout(ದೀರ್ಘ msec)
{
    ಆದರೆ (timeout_nesting == 0)
	timeout_flag = ಆರಂಭ_timeout(msec);
    ++timeout_nesting;
}

    ಶೂನ್ಯ
disable_regexp_timeout(ಶೂನ್ಯ)
{
    ಆದರೆ (timeout_nesting == 0)
	iemsg("disable_regexp_timeout() called without active timer");
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (--timeout_nesting == 0)
    {
	stop_timeout();
	timeout_flag = &dummy_timeout_flag;
    }
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL)
# ifdef FEAT_RELTIME
ಸ್ಥಿರ sig_atomic_t *saved_timeout_flag;
# endif

/*
 * Used at the debug prompt: disable the timeout so that expression evaluation
 * can used patterns.
 * Must be followed by calling restore_timeout_for_debugging().
 */
    ಶೂನ್ಯ
save_timeout_for_debugging(ಶೂನ್ಯ)
{
# ifdef FEAT_RELTIME
    saved_timeout_flag = (sig_atomic_t *)timeout_flag;
    timeout_flag = &dummy_timeout_flag;
# endif
}

    ಶೂನ್ಯ
restore_timeout_for_debugging(ಶೂನ್ಯ)
{
# ifdef FEAT_RELTIME
    timeout_flag = saved_timeout_flag;
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The first byte of the BT regexp internal "program" is actually this magic
 * number; the start node begins in the second byte.  It's used to catch the
 * most severe mutilation of the program by the caller.
 */

#ವ್ಯಾಖ್ಯಾನಿಸು REGMAGIC	0234

/*
 * Utility definitions.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UCHARAT(p)	((ಪೂರ್ಣಾಂಕ)*(char_u *)(p))

// Used for an error (down from) vim_regcomp(): give the error message, set
// rc_did_emsg and return NULL
#ವ್ಯಾಖ್ಯಾನಿಸು EMSG_RET_NULL(m) ಹಿಂತಿರುಗಿ (emsg((m)), rc_did_emsg = TRUE, (ಶೂನ್ಯ *)ಖಾಲಿ)
#ವ್ಯಾಖ್ಯಾನಿಸು IEMSG_RET_NULL(m) ಹಿಂತಿರುಗಿ (iemsg((m)), rc_did_emsg = TRUE, (ಶೂನ್ಯ *)ಖಾಲಿ)
#ವ್ಯಾಖ್ಯಾನಿಸು EMSG_RET_FAIL(m) ಹಿಂತಿರುಗಿ (emsg((m)), rc_did_emsg = TRUE, FAIL)
#ವ್ಯಾಖ್ಯಾನಿಸು EMSG2_RET_NULL(m, c) ಹಿಂತಿರುಗಿ (semsg((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)(m), (c) ? "" : "\\"), rc_did_emsg = TRUE, (ಶೂನ್ಯ *)ಖಾಲಿ)
#ವ್ಯಾಖ್ಯಾನಿಸು EMSG3_RET_NULL(m, c, a) ಹಿಂತಿರುಗಿ (semsg((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)(m), (c) ? "" : "\\", (a)), rc_did_emsg = TRUE, (ಶೂನ್ಯ *)ಖಾಲಿ)
#ವ್ಯಾಖ್ಯಾನಿಸು EMSG2_RET_FAIL(m, c) ಹಿಂತಿರುಗಿ (semsg((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)(m), (c) ? "" : "\\"), rc_did_emsg = TRUE, FAIL)
#ವ್ಯಾಖ್ಯಾನಿಸು EMSG_ONE_RET_NULL EMSG2_RET_NULL(_(e_invalid_ಅಂಶ_ಒಳ_str_brackets), reg_magic == MAGIC_ALL)


#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ಎಲ್ಲೆ	(32767L << 16L)

#ವ್ಯಾಖ್ಯಾನಿಸು NOT_MULTI	0
#ವ್ಯಾಖ್ಯಾನಿಸು MULTI_ONE	1
#ವ್ಯಾಖ್ಯಾನಿಸು MULTI_MULT	2

// return values for regmatch()
#ವ್ಯಾಖ್ಯಾನಿಸು RA_FAIL		1	// something failed, abort
#ವ್ಯಾಖ್ಯಾನಿಸು RA_CONT		2	// continue in inner loop
#ವ್ಯಾಖ್ಯಾನಿಸು RA_BREAK	3	// break inner loop
#ವ್ಯಾಖ್ಯಾನಿಸು RA_ಹೊಂದಿಕೆ	4	// successful match
#ವ್ಯಾಖ್ಯಾನಿಸು RA_NOMATCH	5	// didn't match

/*
 * Return NOT_MULTI if c is not a "multi" operator.
 * Return MULTI_ONE if c is a single "multi" operator.
 * Return MULTI_MULT if c is a multi "multi" operator.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
re_multi_type(ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (c == Magic('@') || c == Magic('=') || c == Magic('?'))
	ಹಿಂತಿರುಗಿ MULTI_ONE;
    ಆದರೆ (c == Magic('*') || c == Magic('+') || c == Magic('{'))
	ಹಿಂತಿರುಗಿ MULTI_MULT;
    ಹಿಂತಿರುಗಿ NOT_MULTI;
}

ಸ್ಥಿರ char_u		*reg_prev_sub = ಖಾಲಿ;
ಸ್ಥಿರ size_t		reg_prev_sublen = 0;

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
ಸ್ಥಿರ char_u REGEXP_INRANGE[] = "]^-n\\";
ಸ್ಥಿರ char_u REGEXP_ABBR[] = "nrtebdoxuU";

/*
 * Translate '\x' to its control character, except "\n", which is Magic.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
backslash_trans(ಪೂರ್ಣಾಂಕ c)
{
    ಆಯ್ಕೆ (c)
    {
	ಸ್ಥಿತಿ 'r':   ಹಿಂತಿರುಗಿ CAR;
	ಸ್ಥಿತಿ 't':   ಹಿಂತಿರುಗಿ TAB;
	ಸ್ಥಿತಿ 'e':   ಹಿಂತಿರುಗಿ ESC;
	ಸ್ಥಿತಿ 'b':   ಹಿಂತಿರುಗಿ BS;
    }
    ಹಿಂತಿರುಗಿ c;
}

ಎಣಿಕೆ
{
    CLASS_ALNUM = 0,
    CLASS_ALPHA,
    CLASS_BLANK,
    CLASS_CNTRL,
    CLASS_DIGIT,
    CLASS_ನಕ್ಷೆ,
    CLASS_ಅಧೋ,
    CLASS_PRINT,
    CLASS_PUNCT,
    CLASS_SPACE,
    CLASS_ಊರ್ಧ್ವ,
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_char_class(char_u **pp)
{
    // must be sorted by the 'value' field because it is used by bsearch()!
    ಸ್ಥಿರ keyvalue_T char_class_tab[] =
    {
	KEYVALUE_ನಮೂದು(CLASS_ALNUM, "alnum:]"),
	KEYVALUE_ನಮೂದು(CLASS_ALPHA, "alpha:]"),
	KEYVALUE_ನಮೂದು(CLASS_BACKSPACE, "backspace:]"),
	KEYVALUE_ನಮೂದು(CLASS_BLANK, "blank:]"),
	KEYVALUE_ನಮೂದು(CLASS_CNTRL, "cntrl:]"),
	KEYVALUE_ನಮೂದು(CLASS_DIGIT, "digit:]"),
	KEYVALUE_ನಮೂದು(CLASS_ESCAPE, "escape:]"),
	KEYVALUE_ನಮೂದು(CLASS_FNAME, "fname:]"),
	KEYVALUE_ನಮೂದು(CLASS_ನಕ್ಷೆ, "graph:]"),
	KEYVALUE_ನಮೂದು(CLASS_IDENT, "ident:]"),
	KEYVALUE_ನಮೂದು(CLASS_KEYWORD, "keyword:]"),
	KEYVALUE_ನಮೂದು(CLASS_ಅಧೋ, "lower:]"),
	KEYVALUE_ನಮೂದು(CLASS_PRINT, "print:]"),
	KEYVALUE_ನಮೂದು(CLASS_PUNCT, "punct:]"),
	KEYVALUE_ನಮೂದು(CLASS_RETURN, "return:]"),
	KEYVALUE_ನಮೂದು(CLASS_SPACE, "space:]"),
	KEYVALUE_ನಮೂದು(CLASS_TAB, "tab:]"),
	KEYVALUE_ನಮೂದು(CLASS_ಊರ್ಧ್ವ, "upper:]"),
	KEYVALUE_ನಮೂದು(CLASS_XDIGIT, "xdigit:]")
    };

    // check that the value of "pp" has a chance of matching
    ಆದರೆ ((*pp)[1] == ':' && ASCII_ISLOWER((*pp)[2])
			&& ASCII_ISLOWER((*pp)[3]) && ASCII_ISLOWER((*pp)[4]))
    {
	keyvalue_T ಗುರಿ;
	keyvalue_T *ನಮೂದು;
	// this function can be called repeatedly with the same value for "pp"
	// so we cache the last found entry.
	ಸ್ಥಿರ keyvalue_T *ಕೊನೆಯ_ನಮೂದು = ಖಾಲಿ;

	ಗುರಿ.ಕೀಲಿ = 0;
	ಗುರಿ.ಮೌಲ್ಯ.string = *pp + 2;
	ಗುರಿ.ಮೌಲ್ಯ.ಉದ್ದಳತೆ = 0;	// not used, see cmp_keyvalue_value_n()

	ಆದರೆ (ಕೊನೆಯ_ನಮೂದು != ಖಾಲಿ && cmp_keyvalue_ಮೌಲ್ಯ_n(&ಗುರಿ, ಕೊನೆಯ_ನಮೂದು) == 0)
	    ನಮೂದು = ಕೊನೆಯ_ನಮೂದು;
	ಇಲ್ಲದಿದ್ದರೆ
	    ನಮೂದು = (keyvalue_T *)bsearch(&ಗುರಿ, &char_class_tab,
					ARRAY_ಉದ್ದಳತೆ(char_class_tab),
					ಗಾತ್ರ(char_class_tab[0]), cmp_keyvalue_ಮೌಲ್ಯ_n);
	ಆದರೆ (ನಮೂದು != ಖಾಲಿ)
	{
	    ಕೊನೆಯ_ನಮೂದು = ನಮೂದು;
	    *pp += ನಮೂದು->ಮೌಲ್ಯ.ಉದ್ದಳತೆ + 2;
	    ಹಿಂತಿರುಗಿ ನಮೂದು->ಕೀಲಿ;
	}
    }
    ಹಿಂತಿರುಗಿ CLASS_NONE;
}

/*
 * Specific version of character class functions.
 * Using a table to keep this fast.
 */
ಸ್ಥಿರ ಗಿಡ್ಡ	class_tab[256];

#ವ್ಯಾಖ್ಯಾನಿಸು	    RI_DIGIT	0x01
#ವ್ಯಾಖ್ಯಾನಿಸು	    RI_HEX	0x02
#ವ್ಯಾಖ್ಯಾನಿಸು	    RI_OCTAL	0x04
#ವ್ಯಾಖ್ಯಾನಿಸು	    RI_ಶಬ್ದ	0x08
#ವ್ಯಾಖ್ಯಾನಿಸು	    RI_ತಲೆ	0x10
#ವ್ಯಾಖ್ಯಾನಿಸು	    RI_ALPHA	0x20
#ವ್ಯಾಖ್ಯಾನಿಸು	    RI_ಅಧೋ	0x40
#ವ್ಯಾಖ್ಯಾನಿಸು	    RI_ಊರ್ಧ್ವ	0x80
#ವ್ಯಾಖ್ಯಾನಿಸು	    RI_WHITE	0x100

    ಸ್ಥಿರ ಶೂನ್ಯ
init_class_tab(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಮುಗಿದ = FALSE;

    ಆದರೆ (ಮುಗಿದ)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (i = 0; i < 256; ++i)
    {
	ಆದರೆ (i >= '0' && i <= '7')
	    class_tab[i] = RI_DIGIT + RI_HEX + RI_OCTAL + RI_ಶಬ್ದ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i >= '8' && i <= '9')
	    class_tab[i] = RI_DIGIT + RI_HEX + RI_ಶಬ್ದ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i >= 'a' && i <= 'f')
	    class_tab[i] = RI_HEX + RI_ಶಬ್ದ + RI_ತಲೆ + RI_ALPHA + RI_ಅಧೋ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i >= 'g' && i <= 'z')
	    class_tab[i] = RI_ಶಬ್ದ + RI_ತಲೆ + RI_ALPHA + RI_ಅಧೋ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i >= 'A' && i <= 'F')
	    class_tab[i] = RI_HEX + RI_ಶಬ್ದ + RI_ತಲೆ + RI_ALPHA + RI_ಊರ್ಧ್ವ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i >= 'G' && i <= 'Z')
	    class_tab[i] = RI_ಶಬ್ದ + RI_ತಲೆ + RI_ALPHA + RI_ಊರ್ಧ್ವ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i == '_')
	    class_tab[i] = RI_ಶಬ್ದ + RI_ತಲೆ;
	ಇಲ್ಲದಿದ್ದರೆ
	    class_tab[i] = 0;
    }
    class_tab[' '] |= RI_WHITE;
    class_tab['\t'] |= RI_WHITE;
    ಮುಗಿದ = TRUE;
}

#ವ್ಯಾಖ್ಯಾನಿಸು ri_digit(c)	((c) < 0x100 && (class_tab[c] & RI_DIGIT))
#ವ್ಯಾಖ್ಯಾನಿಸು ri_hex(c)	((c) < 0x100 && (class_tab[c] & RI_HEX))
#ವ್ಯಾಖ್ಯಾನಿಸು ri_octal(c)	((c) < 0x100 && (class_tab[c] & RI_OCTAL))
#ವ್ಯಾಖ್ಯಾನಿಸು ri_ಶಬ್ದ(c)	((c) < 0x100 && (class_tab[c] & RI_ಶಬ್ದ))
#ವ್ಯಾಖ್ಯಾನಿಸು ri_ತಲೆ(c)	((c) < 0x100 && (class_tab[c] & RI_ತಲೆ))
#ವ್ಯಾಖ್ಯಾನಿಸು ri_alpha(c)	((c) < 0x100 && (class_tab[c] & RI_ALPHA))
#ವ್ಯಾಖ್ಯಾನಿಸು ri_ಅಧೋ(c)	((c) < 0x100 && (class_tab[c] & RI_ಅಧೋ))
#ವ್ಯಾಖ್ಯಾನಿಸು ri_ಊರ್ಧ್ವ(c)	((c) < 0x100 && (class_tab[c] & RI_ಊರ್ಧ್ವ))
#ವ್ಯಾಖ್ಯಾನಿಸು ri_white(c)	((c) < 0x100 && (class_tab[c] & RI_WHITE))

// flags for regflags
#ವ್ಯಾಖ್ಯಾನಿಸು RF_ICASE    1	// ignore case
#ವ್ಯಾಖ್ಯಾನಿಸು RF_NOICASE  2	// don't ignore case
#ವ್ಯಾಖ್ಯಾನಿಸು RF_HASNL    4	// can match a NL
#ವ್ಯಾಖ್ಯಾನಿಸು RF_ICOMBINE 8	// ignore combining characters
#ವ್ಯಾಖ್ಯಾನಿಸು RF_LOOKBH   16	// uses "\@<=" or "\@<!"

/*
 * Global work variables for vim_regcomp().
 */

ಸ್ಥಿರ char_u	*regparse;	// Input-scan pointer.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	regnpar;	// () count.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಬಯಸುತ್ತದೆ_nfa;	// regex should use NFA engine
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	regnzpar;	// \z() count.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	re_has_z;	// \z item detected
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ	regflags;	// RF_ flags for prog
#ಆದರೆ defined(FEAT_SYN_HL)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	had_eol;	// TRUE when EOL found by vim_regcomp()
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ magic_T	reg_magic;	// magicness of the pattern

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	reg_string;	// matching with a string instead of a buffer
				//ಸಾಲು
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	reg_strict;	// "[abc" is illegal

/*
 * META contains all characters that may be magic, except '^' and '$'.
 */

// META[] is used often enough to justify turning it into a table.
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char_u META_flags[] = {
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

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	curchr;		// currently parsed character
// Previous character.  Note: prevchr is sometimes -1 when we are not at the
// start, eg in /[ ^I]^ the pattern was never found even if it existed,
// because ^ was taken to be magic -- webb
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	prevchr;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	prevprevchr;	// previous-previous character
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	nextchr;	// used for ungetchr()

// arguments for reg()
#ವ್ಯಾಖ್ಯಾನಿಸು REG_NOPAREN	0	// toplevel reg()
#ವ್ಯಾಖ್ಯಾನಿಸು REG_PAREN	1	// \(\)
#ವ್ಯಾಖ್ಯಾನಿಸು REG_ZPAREN	2	// \z(\)
#ವ್ಯಾಖ್ಯಾನಿಸು REG_NPAREN	3	// \%(\)

// Limit recursive parsing of nested regexp atoms to avoid using up the C stack.
#ವ್ಯಾಖ್ಯಾನಿಸು REG_MAX_PAREN_ಆಳ	1000

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    char_u	*regparse;
    ಪೂರ್ಣಾಂಕ		prevchr_len;
    ಪೂರ್ಣಾಂಕ		curchr;
    ಪೂರ್ಣಾಂಕ		prevchr;
    ಪೂರ್ಣಾಂಕ		prevprevchr;
    ಪೂರ್ಣಾಂಕ		nextchr;
    ಪೂರ್ಣಾಂಕ		at_ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ		prev_at_ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ		regnpar;
} parse_state_T;

ಸ್ಥಿರ ಶೂನ್ಯ	initchr(char_u *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	getchr(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ	skipchr_keepstart(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	peekchr(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ	skipchr(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ	ungetchr(ಶೂನ್ಯ);
ಸ್ಥಿರ vimlong_T	gethexchrs(ಪೂರ್ಣಾಂಕ maxinputlen);
ಸ್ಥಿರ ದೀರ್ಘ	getoctchrs(ಶೂನ್ಯ);
ಸ್ಥಿರ vimlong_T	getdecchrs(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	coll_get_char(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	prog_magic_wrong(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	cstrncmp(char_u *s1, char_u *s2, ಪೂರ್ಣಾಂಕ *n);
ಸ್ಥಿರ char_u	*cstrchr(char_u *, ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	re_mult_ಮುಂದಿನ(ಅಕ್ಷರ *what);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	reg_iswordc(ಪೂರ್ಣಾಂಕ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
ಸ್ಥಿರ ಶೂನ್ಯ report_re_switch(char_u *pat);
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ regengine_T bt_regengine;
ಸ್ಥಿರ regengine_T nfa_regengine;

/*
 * Return TRUE if compiled regular expression "prog" can match a line break.
 */
    ಪೂರ್ಣಾಂಕ
re_multiline(regprog_T *prog)
{
    ಹಿಂತಿರುಗಿ (prog->regflags & RF_HASNL);
}

/*
 * Check for an equivalence class name "[=a=]".  "pp" points to the '['.
 * Returns a character representing the class. Zero means that no item was
 * recognized.  Otherwise "pp" is advanced to after the item.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_equi_class(char_u **pp)
{
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		l = 1;
    char_u	*p = *pp;

    ಆದರೆ (p[1] == '=' && p[2] != NUL)
    {
	ಆದರೆ (has_mbyte)
	    l = (*mb_ptr2len)(p + 2);
	ಆದರೆ (p[l + 2] == '=' && p[l + 3] == ']')
	{
	    ಆದರೆ (has_mbyte)
		c = mb_ptr2char(p + 2);
	    ಇಲ್ಲದಿದ್ದರೆ
		c = p[2];
	    *pp += l + 4;
	    ಹಿಂತಿರುಗಿ c;
	}
    }
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Check for a collating element "[.a.]".  "pp" points to the '['.
 * Returns a character. Zero means that no item was recognized.  Otherwise
 * "pp" is advanced to after the item.
 * Currently only single characters are recognized!
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_coll_element(char_u **pp)
{
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		l = 1;
    char_u	*p = *pp;

    ಆದರೆ (p[0] != NUL && p[1] == '.' && p[2] != NUL)
    {
	ಆದರೆ (has_mbyte)
	    l = (*mb_ptr2len)(p + 2);
	ಆದರೆ (p[l + 2] == '.' && p[l + 3] == ']')
	{
	    ಆದರೆ (has_mbyte)
		c = mb_ptr2char(p + 2);
	    ಇಲ್ಲದಿದ್ದರೆ
		c = p[2];
	    *pp += l + 4;
	    ಹಿಂತಿರುಗಿ c;
	}
    }
    ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ reg_cpo_lit; // 'cpoptions' contains 'l' flag
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ reg_cpo_bsl; // 'cpoptions' contains '\' flag

    ಸ್ಥಿರ ಶೂನ್ಯ
get_cpo_flags(ಶೂನ್ಯ)
{
    reg_cpo_lit = vim_strchr(p_cpo, CPO_LITERAL) != ಖಾಲಿ;
    reg_cpo_bsl = vim_strchr(p_cpo, CPO_BACKSL) != ಖಾಲಿ;
}

/*
 * Skip over a "[]" range.
 * "p" must point to the character after the '['.
 * The returned pointer is on the matching ']', or the terminating NUL.
 */
    ಸ್ಥಿರ char_u *
skip_anyof(char_u *p)
{
    ಪೂರ್ಣಾಂಕ		l;

    ಆದರೆ (*p == '^')	// Complement of range.
	++p;
    ಆದರೆ (*p == ']' || *p == '-')
	++p;
    ಆಗ (*p != NUL && *p != ']')
    {
	ಆದರೆ (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	    p += l;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಆದರೆ (*p == '-')
	    {
		++p;
		ಆದರೆ (*p != ']' && *p != NUL)
		    MB_PTR_ADV(p);
	    }
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '\\'
		&& !reg_cpo_bsl
		&& (vim_strchr(REGEXP_INRANGE, p[1]) != ಖಾಲಿ
		    || (!reg_cpo_lit && vim_strchr(REGEXP_ABBR, p[1]) != ಖಾಲಿ)))
	    p += 2;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '[')
	{
	    ಆದರೆ (get_char_class(&p) == CLASS_NONE
		    && get_equi_class(&p) == 0
		    && get_coll_element(&p) == 0
		    && *p != NUL)
		++p; // it is not a class name and not NUL
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ++p;
    }

    ಹಿಂತಿರುಗಿ p;
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
    ಪೂರ್ಣಾಂಕ		delim,
    ಪೂರ್ಣಾಂಕ		magic)
{
    ಹಿಂತಿರುಗಿ skip_regexp_ex(startp, delim, magic, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
}

/*
 * Call skip_regexp() and when the delimiter does not match give an error and
 * return NULL.
 */
    char_u *
skip_regexp_err(
    char_u	*startp,
    ಪೂರ್ಣಾಂಕ		delim,
    ಪೂರ್ಣಾಂಕ		magic)
{
    char_u *p = skip_regexp(startp, delim, magic);

    ಆದರೆ (*p != delim)
    {
	semsg(_(e_missing_delimiter_after_ಹುಡುಕು_ನಮೂನೆ_str), startp);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ p;
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
    ಪೂರ್ಣಾಂಕ		dirc,
    ಪೂರ್ಣಾಂಕ		magic,
    char_u	**newp,
    ಪೂರ್ಣಾಂಕ		*dropped,
    magic_T	*magic_val)
{
    magic_T	mymagic;
    char_u	*p = startp;
    size_t	startplen = 0;

    ಆದರೆ (magic)
	mymagic = MAGIC_ON;
    ಇಲ್ಲದಿದ್ದರೆ
	mymagic = MAGIC_OFF;
    get_cpo_flags();

    ಗಾಗಿ (; p[0] != NUL; MB_PTR_ADV(p))
    {
	ಆದರೆ (p[0] == dirc)	// found end of regexp
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ ((p[0] == '[' && mymagic >= MAGIC_ON)
		|| (p[0] == '\\' && p[1] == '[' && mymagic <= MAGIC_OFF))
	{
	    p = skip_anyof(p + 1);
	    ಆದರೆ (p[0] == NUL)
		ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[0] == '\\' && p[1] != NUL)
	{
	    ಆದರೆ (dirc == '?' && newp != ಖಾಲಿ && p[1] == '?')
	    {
		// change "\?" to "?", make a copy first.
		ಆದರೆ (startplen == 0)
		    startplen = STRLEN(startp);
		ಆದರೆ (*newp == ಖಾಲಿ)
		{
		    *newp = vim_strnsave(startp, startplen);
		    ಆದರೆ (*newp != ಖಾಲಿ)
		    {
			p = *newp + (p - startp);
			startp = *newp;
		    }
		}
		ಆದರೆ (dropped != ಖಾಲಿ)
		    ++*dropped;
		ಆದರೆ (*newp != ಖಾಲಿ)
		    mch_memmove(p, p + 1, startplen - ((p + 1) - startp) + 1);
		ಇಲ್ಲದಿದ್ದರೆ
		    ++p;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		++p;    // skip next character
	    ಆದರೆ (*p == 'v')
		mymagic = MAGIC_ALL;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == 'V')
		mymagic = MAGIC_NONE;
	}
    }
    ಆದರೆ (magic_val != ಖಾಲಿ)
	*magic_val = mymagic;
    ಹಿಂತಿರುಗಿ p;
}

/*
 * Functions for getting characters from the regexp input.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	prevchr_len;	// byte length of previous char
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	at_ಆರಂಭ;	// True when on the first character
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	prev_at_ಆರಂಭ;  // True when on the second character

/*
 * Start parsing at "str".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
initchr(char_u *str)
{
    regparse = str;
    prevchr_len = 0;
    curchr = prevprevchr = prevchr = nextchr = -1;
    at_ಆರಂಭ = TRUE;
    prev_at_ಆರಂಭ = FALSE;
}

/*
 * Save the current parse state, so that it can be restored and parsing
 * starts in the same state again.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
save_parse_state(parse_state_T *ps)
{
    ps->regparse = regparse;
    ps->prevchr_len = prevchr_len;
    ps->curchr = curchr;
    ps->prevchr = prevchr;
    ps->prevprevchr = prevprevchr;
    ps->nextchr = nextchr;
    ps->at_ಆರಂಭ = at_ಆರಂಭ;
    ps->prev_at_ಆರಂಭ = prev_at_ಆರಂಭ;
    ps->regnpar = regnpar;
}

/*
 * Restore a previously saved parse state.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
restore_parse_state(parse_state_T *ps)
{
    regparse = ps->regparse;
    prevchr_len = ps->prevchr_len;
    curchr = ps->curchr;
    prevchr = ps->prevchr;
    prevprevchr = ps->prevprevchr;
    nextchr = ps->nextchr;
    at_ಆರಂಭ = ps->at_ಆರಂಭ;
    prev_at_ಆರಂಭ = ps->prev_at_ಆರಂಭ;
    regnpar = ps->regnpar;
}


/*
 * Get the next character without advancing.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
peekchr(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	after_slash = FALSE;

    ಆದರೆ (curchr != -1)
	ಹಿಂತಿರುಗಿ curchr;

    ಆಯ್ಕೆ (curchr = regparse[0])
    {
	ಸ್ಥಿತಿ '.':
	ಸ್ಥಿತಿ '[':
	ಸ್ಥಿತಿ '~':
	    // magic when 'magic' is on
	    ಆದರೆ (reg_magic >= MAGIC_ON)
		curchr = Magic(curchr);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ '(':
	ಸ್ಥಿತಿ ')':
	ಸ್ಥಿತಿ '{':
	ಸ್ಥಿತಿ '%':
	ಸ್ಥಿತಿ '+':
	ಸ್ಥಿತಿ '=':
	ಸ್ಥಿತಿ '?':
	ಸ್ಥಿತಿ '@':
	ಸ್ಥಿತಿ '!':
	ಸ್ಥಿತಿ '&':
	ಸ್ಥಿತಿ '|':
	ಸ್ಥಿತಿ '<':
	ಸ್ಥಿತಿ '>':
	ಸ್ಥಿತಿ '#':	// future ext.
	ಸ್ಥಿತಿ '"':	// future ext.
	ಸ್ಥಿತಿ '\'':	// future ext.
	ಸ್ಥಿತಿ ',':	// future ext.
	ಸ್ಥಿತಿ '-':	// future ext.
	ಸ್ಥಿತಿ ':':	// future ext.
	ಸ್ಥಿತಿ ';':	// future ext.
	ಸ್ಥಿತಿ '`':	// future ext.
	ಸ್ಥಿತಿ '/':	// Can't be used in / command
			// magic only after "\v"
	    ಆದರೆ (reg_magic == MAGIC_ALL)
		curchr = Magic(curchr);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ '*':
	    // * is not magic as the very first character, eg "?*ptr", when
	    // after '^', eg "/^*ptr" and when after "\(", "\|", "\&".  But
	    // "\(\*" is not magic, thus must be magic if "after_slash"
	    ಆದರೆ (reg_magic >= MAGIC_ON
		    && !at_ಆರಂಭ
		    && !(prev_at_ಆರಂಭ && prevchr == Magic('^'))
		    && (after_slash
			|| (prevchr != Magic('(')
			    && prevchr != Magic('&')
			    && prevchr != Magic('|'))))
		curchr = Magic('*');
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ '^':
	    // '^' is only magic as the very first character and if it's after
	    // "\(", "\|", "\&' or "\n"
	    ಆದರೆ (reg_magic >= MAGIC_OFF
		    && (at_ಆರಂಭ
			|| reg_magic == MAGIC_ALL
			|| prevchr == Magic('(')
			|| prevchr == Magic('|')
			|| prevchr == Magic('&')
			|| prevchr == Magic('n')
			|| (no_Magic(prevchr) == '('
			    && prevprevchr == Magic('%'))))
	    {
		curchr = Magic('^');
		at_ಆರಂಭ = TRUE;
		prev_at_ಆರಂಭ = FALSE;
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ '$':
	    // '$' is only magic as the very last char and if it's in front of
	    // either "\|", "\)", "\&", or "\n"
	    ಆದರೆ (reg_magic >= MAGIC_OFF)
	    {
		char_u *p = regparse + 1;
		ಪೂರ್ಣಾಂಕ is_magic_all = (reg_magic == MAGIC_ALL);

		// ignore \c \C \m \M \v \V and \Z after '$'
		ಆಗ (p[0] == '\\' && (p[1] == 'c' || p[1] == 'C'
			    || p[1] == 'm' || p[1] == 'M'
			    || p[1] == 'v' || p[1] == 'V' || p[1] == 'Z'))
		{
		    ಆದರೆ (p[1] == 'v')
			is_magic_all = TRUE;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[1] == 'm' || p[1] == 'M' || p[1] == 'V')
			is_magic_all = FALSE;
		    p += 2;
		}
		ಆದರೆ (p[0] == NUL
			|| (p[0] == '\\'
			    && (p[1] == '|' || p[1] == '&' || p[1] == ')'
				|| p[1] == 'n'))
			|| (is_magic_all
			    && (p[0] == '|' || p[0] == '&' || p[0] == ')'))
			|| reg_magic == MAGIC_ALL)
		    curchr = Magic('$');
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ '\\':
	    {
		ಪೂರ್ಣಾಂಕ c = regparse[1];

		ಆದರೆ (c == NUL)
		    curchr = '\\';	// trailing '\'
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c <= '~' && META_flags[c])
		{
		    /*
		     * META contains everything that may be magic sometimes,
		     * except ^ and $ ("\^" and "\$" are only magic after
		     * "\V").  We now fetch the next character and toggle its
		     * magicness.  Therefore, \ is so meta-magic that it is
		     * not in META.
		     */
		    curchr = -1;
		    prev_at_ಆರಂಭ = at_ಆರಂಭ;
		    at_ಆರಂಭ = FALSE;	// be able to say "/\*ptr"
		    ++regparse;
		    ++after_slash;
		    peekchr();
		    --regparse;
		    --after_slash;
		    curchr = toggle_Magic(curchr);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_strchr(REGEXP_ABBR, c))
		{
		    /*
		     * Handle abbreviations, like "\t" for TAB -- webb
		     */
		    curchr = backslash_trans(c);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (reg_magic == MAGIC_NONE && (c == '$' || c == '^'))
		    curchr = toggle_Magic(c);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    /*
		     * Next character can never be (made) magic?
		     * Then backslashing it won't do anything.
		     */
		    ಆದರೆ (has_mbyte && c >= 0x80)
			curchr = (*mb_ptr2char)(regparse + 1);
		    ಇಲ್ಲದಿದ್ದರೆ
			curchr = c;
		}
		ನಿಲ್ಲಿಸು;
	    }

	ಪೂರ್ವನಿಯೋಜಿತ:
	    // curchr already holds regparse[0]; only a multi-byte lead byte
	    // needs decoding.
	    ಆದರೆ (has_mbyte && curchr >= 0x80)
		curchr = (*mb_ptr2char)(regparse);
    }

    ಹಿಂತಿರುಗಿ curchr;
}

/*
 * Eat one lexed character.  Do this in a way that we can undo it.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
skipchr(ಶೂನ್ಯ)
{
    // peekchr() eats a backslash, do the same here
    ಆದರೆ (*regparse == '\\')
	prevchr_len = 1;
    ಇಲ್ಲದಿದ್ದರೆ
	prevchr_len = 0;
    ಆದರೆ (regparse[prevchr_len] != NUL)
    {
	ಆದರೆ (enc_utf8)
	{
	    // Exclude composing chars that mb_ptr2len does include.  A byte
	    // below 0x80 is always a single character.
	    ಆದರೆ (regparse[prevchr_len] < 0x80)
		++prevchr_len;
	    ಇಲ್ಲದಿದ್ದರೆ
		prevchr_len += utf_ptr2len(regparse + prevchr_len);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte)
	    prevchr_len += (*mb_ptr2len)(regparse + prevchr_len);
	ಇಲ್ಲದಿದ್ದರೆ
	    ++prevchr_len;
    }
    regparse += prevchr_len;
    prev_at_ಆರಂಭ = at_ಆರಂಭ;
    at_ಆರಂಭ = FALSE;
    prevprevchr = prevchr;
    prevchr = curchr;
    curchr = nextchr;	    // use previously unget char, or -1
    nextchr = -1;
}

/*
 * Skip a character while keeping the value of prev_at_start for at_start.
 * prevchr and prevprevchr are also kept.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
skipchr_keepstart(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ as = prev_at_ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ pr = prevchr;
    ಪೂರ್ಣಾಂಕ prpr = prevprevchr;

    skipchr();
    at_ಆರಂಭ = as;
    prevchr = pr;
    prevprevchr = prpr;
}

/*
 * Get the next character from the pattern. We know about magic and such, so
 * therefore we need a lexical analyzer.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
getchr(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ chr = peekchr();

    skipchr();
    ಹಿಂತಿರುಗಿ chr;
}

/*
 * put character back.  Works only once!
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ungetchr(ಶೂನ್ಯ)
{
    nextchr = curchr;
    curchr = prevchr;
    prevchr = prevprevchr;
    at_ಆರಂಭ = prev_at_ಆರಂಭ;
    prev_at_ಆರಂಭ = FALSE;

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
    ಸ್ಥಿರ vimlong_T
gethexchrs(ಪೂರ್ಣಾಂಕ maxinputlen)
{
    long_u	nr = 0;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 0; i < maxinputlen; ++i)
    {
	c = regparse[0];
	ಆದರೆ (!vim_isxdigit(c))
	    ನಿಲ್ಲಿಸು;
	nr <<= 4;
	nr |= hex2nr(c);
	++regparse;
    }

    ಆದರೆ (i == 0)
	ಹಿಂತಿರುಗಿ -1;
    ಹಿಂತಿರುಗಿ nr;
}

/*
 * Get and return the value of the decimal string immediately after the
 * current position. Return -1 for invalid.  Consumes all digits.
 */
    ಸ್ಥಿರ vimlong_T
getdecchrs(ಶೂನ್ಯ)
{
    long_u	nr = 0;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 0; ; ++i)
    {
	c = regparse[0];
	ಆದರೆ (c < '0' || c > '9')
	    ನಿಲ್ಲಿಸು;
	nr *= 10;
	nr += c - '0';
	++regparse;
	curchr = -1; // no longer valid
    }

    ಆದರೆ (i == 0)
	ಹಿಂತಿರುಗಿ -1;
    ಹಿಂತಿರುಗಿ nr;
}

/*
 * get and return the value of the octal string immediately after the current
 * position. Return -1 for invalid, or 0-255 for valid. Smart enough to handle
 * numbers > 377 correctly (for example, 400 is treated as 40) and doesn't
 * treat 8 or 9 as recognised characters. Position is updated:
 *     blahblah\%o210asdf
 *	   before-^  ^-after
 */
    ಸ್ಥಿರ ದೀರ್ಘ
getoctchrs(ಶೂನ್ಯ)
{
    long_u	nr = 0;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 0; i < 3 && nr < 040; ++i)
    {
	c = regparse[0];
	ಆದರೆ (c < '0' || c > '7')
	    ನಿಲ್ಲಿಸು;
	nr <<= 3;
	nr |= hex2nr(c);
	++regparse;
    }

    ಆದರೆ (i == 0)
	ಹಿಂತಿರುಗಿ -1;
    ಹಿಂತಿರುಗಿ (ದೀರ್ಘ)nr;
}

/*
 * read_limits - Read two integers to be taken as a minimum and maximum.
 * If the first character is '-', then the range is reversed.
 * Should end with 'end'.  If minval is missing, zero is default, if maxval is
 * missing, a very big number is the default.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
read_limits(ದೀರ್ಘ *minval, ದೀರ್ಘ *maxval)
{
    ಪೂರ್ಣಾಂಕ		ಹಿಮ್ಮುಖ = FALSE;
    char_u	*ಮೊದಲ_char;
    ದೀರ್ಘ	tmp;

    ಆದರೆ (*regparse == '-')
    {
	// Starts with '-', so reverse the range later
	regparse++;
	ಹಿಮ್ಮುಖ = TRUE;
    }
    ಮೊದಲ_char = regparse;
    *minval = getdigits(&regparse);
    ಆದರೆ (*regparse == ',')	    // There is a comma
    {
	ಆದರೆ (vim_isdigit(*++regparse))
	    *maxval = getdigits(&regparse);
	ಇಲ್ಲದಿದ್ದರೆ
	    *maxval = MAX_ಎಲ್ಲೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VIM_ISDIGIT(*ಮೊದಲ_char))
	*maxval = *minval;	    // It was \{n} or \{-n}
    ಇಲ್ಲದಿದ್ದರೆ
	*maxval = MAX_ಎಲ್ಲೆ;	    // It was \{} or \{-}
    ಆದರೆ (*regparse == '\\')
	regparse++;	// Allow either \{...} or \{...\}
    ಆದರೆ (*regparse != '}')
	EMSG2_RET_FAIL(_(e_syntax_error_ಒಳ_str_curlies),
						       reg_magic == MAGIC_ALL);

    /*
     * Reverse the range if there was a '-', or make sure it is in the right
     * order otherwise.
     */
    ಆದರೆ ((!ಹಿಮ್ಮುಖ && *minval > *maxval) || (ಹಿಮ್ಮುಖ && *minval < *maxval))
    {
	tmp = *minval;
	*minval = *maxval;
	*maxval = tmp;
    }
    skipchr();		// let's be friends with the lexer again
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * vim_regexec and friends
 */

/*
 * Global work variables for vim_regexec().
 */

ಸ್ಥಿರ ಶೂನ್ಯ	cleanup_subexpr(ಶೂನ್ಯ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
ಸ್ಥಿರ ಶೂನ್ಯ	cleanup_zsubexpr(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಹೊಂದಿಕೆ_with_backref(linenr_T ಆರಂಭ_lnum, colnr_T ಆರಂಭ_ಲಂಬಸಾಲು, linenr_T ಕೊನೆ_lnum, colnr_T ಕೊನೆ_ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ *bytelen);

/*
 * Sometimes need to save a copy of a line.  Since alloc()/free() is very
 * slow, we keep one allocated piece of memory and only re-allocate it when
 * it's too small.  It's freed in bt_regexec_both() when finished.
 */
ಸ್ಥಿರ char_u	*reg_tofree = ಖಾಲಿ;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ	reg_tofreelen;

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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    regmatch_T		*reg_ಹೊಂದಿಕೆ;
    regmmatch_T		*reg_mmatch;

    char_u		**reg_startp;
    char_u		**reg_endp;
    lpos_T		*reg_startpos;
    lpos_T		*reg_endpos;

    win_T		*reg_win;
    buf_T		*reg_buf;
    linenr_T		reg_firstlnum;
    linenr_T		reg_maxline;
    ಪೂರ್ಣಾಂಕ			reg_ಸಾಲು_lbr;	// "\n" in string is line break

    // The current match-position is stord in these variables:
    linenr_T	lnum;		// line number, relative to first line
    char_u	*ಸಾಲು;		// start of current line
    char_u	*ಒಳಹರಿವು;		// current input, points into "line"

    ಪೂರ್ಣಾಂಕ	need_clear_subexpr;	// subexpressions still need to be cleared
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ಪೂರ್ಣಾಂಕ	need_clear_zsubexpr;	// extmatch subexpressions still need to be
				// cleared
#ಆದರೆ_ಅಂತ್ಯ

    // Internal copy of 'ignorecase'.  It is set at each call to vim_regexec().
    // Normally it gets the value of "rm_ic" or "rmm_ic", but when the pattern
    // contains '\c' or '\C' the value is overruled.
    ಪೂರ್ಣಾಂಕ			reg_ic;

    // Similar to "reg_ic", but only for 'combining' characters.  Set with \Z
    // flag in the regexp.  Defaults to false, always.
    ಪೂರ್ಣಾಂಕ			reg_icombine;

    // Copy of "rmm_maxcol": maximum column to search for a match.  Zero when
    // there is no maximum.
    colnr_T		reg_maxcol;

    // State for the NFA engine regexec.
    ಪೂರ್ಣಾಂಕ nfa_has_zend;	    // NFA regexp \ze operator encountered.
    ಪೂರ್ಣಾಂಕ nfa_has_backref;    // NFA regexp \1 .. \9 encountered.
    ಪೂರ್ಣಾಂಕ nfa_nsubexpr;	    // Number of sub expressions actually being used
			    // during execution. 1 if only the whole match
			    // (subexpr 0) is used.
    // listid is global, so that it increases on recursive calls to
    // nfa_regmatch(), which means we don't have to clear the lastlist field of
    // all the states.
    ಪೂರ್ಣಾಂಕ nfa_listid;
    ಪೂರ್ಣಾಂಕ nfa_alt_listid;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ಪೂರ್ಣಾಂಕ nfa_has_zsubexpr;   // NFA regexp has \z( ), set zsubexpr.
#ಆದರೆ_ಅಂತ್ಯ
} regexec_T;

ಸ್ಥಿರ regexec_T	rex;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		rex_ಒಳ_use = FALSE;

/*
 * Return TRUE if character 'c' is included in 'iskeyword' option for
 * "reg_buf" buffer.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
reg_iswordc(ಪೂರ್ಣಾಂಕ c)
{
    ಹಿಂತಿರುಗಿ vim_iswordc_buf(c, rex.reg_buf);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ can_f_submatch = FALSE;	// TRUE when submatch() can be used

// This struct is used for reg_submatch(). Needed for when the
// substitution string is an expression that contains a call to substitute()
// and submatch().
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    regmatch_T	*sm_ಹೊಂದಿಕೆ;
    regmmatch_T	*sm_mmatch;
    linenr_T	sm_firstlnum;
    linenr_T	sm_maxline;
    ಪೂರ್ಣಾಂಕ		sm_ಸಾಲು_lbr;
} regsubmatch_T;

ಸ್ಥಿರ regsubmatch_T rsm;  // can only be used when can_f_submatch is TRUE
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
    RGLF_ಸಾಲು = 0x01,
    RGLF_ಉದ್ದಳತೆ = 0x02
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ,
    RGLF_SUBMATCH = 0x04
#ಆದರೆ_ಅಂತ್ಯ
} reg_getline_flags_T;

//
// common code for reg_getline(), reg_getline_len(), reg_getline_submatch() and
// reg_getline_submatch_len().
// the flags argument (which is a bitmask) controls what info is to be returned and whether
// or not submatch is in effect.
// note:
//     submatch is available only if FEAT_EVAL is defined.
    ಸ್ಥಿರ ಶೂನ್ಯ
reg_getline_common(
    linenr_T		lnum,
    reg_getline_flags_T	flags,
    char_u		**ಸಾಲು,
    colnr_T		*ಉದ್ದಳತೆ)
{
    ಪೂರ್ಣಾಂಕ get_ಸಾಲು = flags & RGLF_ಸಾಲು;
    ಪೂರ್ಣಾಂಕ get_ಉದ್ದಳತೆ = flags & RGLF_ಉದ್ದಳತೆ;
    linenr_T firstlnum;
    linenr_T maxline;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (flags & RGLF_SUBMATCH)
    {
	firstlnum = rsm.sm_firstlnum + lnum;
	maxline = rsm.sm_maxline;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	firstlnum = rex.reg_firstlnum + lnum;
	maxline = rex.reg_maxline;
    }

    // when looking behind for a match/no-match lnum is negative. but we
    // can't go before line 1.
    ಆದರೆ (firstlnum < 1)
    {
	ಆದರೆ (get_ಸಾಲು)
	    *ಸಾಲು = ಖಾಲಿ;
	ಆದರೆ (get_ಉದ್ದಳತೆ)
	    *ಉದ್ದಳತೆ = 0;

	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (lnum > maxline)
    {
	// must have matched the "\n" in the last line.
	ಆದರೆ (get_ಸಾಲು)
	    *ಸಾಲು = (char_u *)"";
	ಆದರೆ (get_ಉದ್ದಳತೆ)
	    *ಉದ್ದಳತೆ = 0;

	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (get_ಸಾಲು)
	*ಸಾಲು = ml_get_buf(rex.reg_buf, firstlnum, FALSE);
    ಆದರೆ (get_ಉದ್ದಳತೆ)
	*ಉದ್ದಳತೆ = ml_get_buf_len(rex.reg_buf, firstlnum);
}

/*
 * Get pointer to the line "lnum", which is relative to "reg_firstlnum".
 */
    ಸ್ಥಿರ char_u *
reg_getline(linenr_T lnum)
{
    char_u *ಸಾಲು;

    reg_getline_common(lnum, RGLF_ಸಾಲು, &ಸಾಲು, ಖಾಲಿ);

    ಹಿಂತಿರುಗಿ ಸಾಲು;
}

/*
 * Get length of line "lnum", which is relative to "reg_firstlnum".
 */
    ಸ್ಥಿರ colnr_T
reg_getline_len(linenr_T lnum)
{
    colnr_T ಉದ್ದಳತೆ;

    reg_getline_common(lnum, RGLF_ಉದ್ದಳತೆ, ಖಾಲಿ, &ಉದ್ದಳತೆ);

    ಹಿಂತಿರುಗಿ ಉದ್ದಳತೆ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
ಸ್ಥಿರ char_u	*reg_startzp[NSUBEXP];	// Workspace to mark beginning
ಸ್ಥಿರ char_u	*reg_endzp[NSUBEXP];	//   and end of \z(...\) matches
ಸ್ಥಿರ lpos_T	reg_startzpos[NSUBEXP];	// idem, beginning pos
ಸ್ಥಿರ lpos_T	reg_endzpos[NSUBEXP];	// idem, end pos
#ಆದರೆ_ಅಂತ್ಯ

// TRUE if using multi-line regexp.
#ವ್ಯಾಖ್ಯಾನಿಸು REG_MULTI	(rex.reg_ಹೊಂದಿಕೆ == ಖಾಲಿ)

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
/*
 * Create a new extmatch and mark it as referenced once.
 */
    ಸ್ಥಿರ reg_extmatch_T *
make_extmatch(ಶೂನ್ಯ)
{
    reg_extmatch_T	*em;

    em = ALLOC_CLEAR_ONE(reg_extmatch_T);
    ಆದರೆ (em != ಖಾಲಿ)
	em->refcnt = 1;
    ಹಿಂತಿರುಗಿ em;
}

/*
 * Add a reference to an extmatch.
 */
    reg_extmatch_T *
ref_extmatch(reg_extmatch_T *em)
{
    ಆದರೆ (em != ಖಾಲಿ)
	em->refcnt++;
    ಹಿಂತಿರುಗಿ em;
}

/*
 * Remove a reference to an extmatch.  If there are no references left, free
 * the info.
 */
    ಶೂನ್ಯ
unref_extmatch(reg_extmatch_T *em)
{
    ಪೂರ್ಣಾಂಕ i;

    ಆದರೆ (em != ಖಾಲಿ && --em->refcnt <= 0)
    {
	ಗಾಗಿ (i = 0; i < NSUBEXP; ++i)
	    vim_free(em->matches[i]);
	vim_free(em);
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Get class of previous character.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
reg_prev_class(ಶೂನ್ಯ)
{
    ಆದರೆ (rex.ಒಳಹರಿವು > rex.ಸಾಲು)
	ಹಿಂತಿರುಗಿ mb_get_class_buf(rex.ಒಳಹರಿವು - 1
		       - (*mb_ತಲೆ_off)(rex.ಸಾಲು, rex.ಒಳಹರಿವು - 1), rex.reg_buf);
    ಹಿಂತಿರುಗಿ -1;
}

/*
 * Return TRUE if the current rex.input position matches the Visual area.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
reg_ಹೊಂದಿಕೆ_visual(ಶೂನ್ಯ)
{
    pos_T	ತುದಿ, bot;
    linenr_T    lnum;
    colnr_T	ಲಂಬಸಾಲು;
    win_T	*wp = rex.reg_win == ಖಾಲಿ ? curwin : rex.reg_win;
    ಪೂರ್ಣಾಂಕ		ಬಹುಲಕ;
    colnr_T	ಆರಂಭ, ಕೊನೆ;
    colnr_T	start2, end2;
    colnr_T	ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    colnr_T	curswant;

    // Check if the buffer is the current buffer and not using a string.
    ಆದರೆ (rex.reg_buf != curbuf || VIsual.lnum == 0 || !REG_MULTI)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
    {
	ಆದರೆ (LT_POS(VIsual, wp->w_cursor))
	{
	    ತುದಿ = VIsual;
	    bot = wp->w_cursor;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ತುದಿ = wp->w_cursor;
	    bot = VIsual;
	}
	ಬಹುಲಕ = VIsual_ಬಹುಲಕ;
	curswant = wp->w_curswant;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (LT_POS(curbuf->b_visual.vi_ಆರಂಭ, curbuf->b_visual.vi_ಕೊನೆ))
	{
	    ತುದಿ = curbuf->b_visual.vi_ಆರಂಭ;
	    bot = curbuf->b_visual.vi_ಕೊನೆ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ತುದಿ = curbuf->b_visual.vi_ಕೊನೆ;
	    bot = curbuf->b_visual.vi_ಆರಂಭ;
	}
	// a substitute command may have removed some lines
	ಆದರೆ (bot.lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    bot.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	ಬಹುಲಕ = curbuf->b_visual.vi_ಬಹುಲಕ;
	curswant = curbuf->b_visual.vi_curswant;
    }
    lnum = rex.lnum + rex.reg_firstlnum;
    ಆದರೆ (lnum < ತುದಿ.lnum || lnum > bot.lnum)
	ಹಿಂತಿರುಗಿ FALSE;

    ಲಂಬಸಾಲು = (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು);
    ಆದರೆ (ಬಹುಲಕ == 'v')
    {
	ಆದರೆ ((lnum == ತುದಿ.lnum && ಲಂಬಸಾಲು < ತುದಿ.ಲಂಬಸಾಲು)
		|| (lnum == bot.lnum && ಲಂಬಸಾಲು >= bot.ಲಂಬಸಾಲು + (*p_sel != 'e')))
	    ಹಿಂತಿರುಗಿ FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಬಹುಲಕ == Ctrl_V)
    {
	getvvcol(wp, &ತುದಿ, &ಆರಂಭ, ಖಾಲಿ, &ಕೊನೆ, 0);
	getvvcol(wp, &bot, &start2, ಖಾಲಿ, &end2, 0);
	ಆದರೆ (start2 < ಆರಂಭ)
	    ಆರಂಭ = start2;
	ಆದರೆ (end2 > ಕೊನೆ)
	    ಕೊನೆ = end2;
	ಆದರೆ (ತುದಿ.ಲಂಬಸಾಲು == MAXCOL || bot.ಲಂಬಸಾಲು == MAXCOL || curswant == MAXCOL)
	    ಕೊನೆ = MAXCOL;

	// getvvcol() flushes rex.line, need to get it again
	rex.ಸಾಲು = reg_getline(rex.lnum);
	rex.ಒಳಹರಿವು = rex.ಸಾಲು + ಲಂಬಸಾಲು;

	ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = win_linetabsize(wp, rex.reg_firstlnum + rex.lnum, rex.ಸಾಲು, ಲಂಬಸಾಲು);
	ಆದರೆ (ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ < ಆರಂಭ || ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ > ಕೊನೆ - (*p_sel == 'e'))
	    ಹಿಂತಿರುಗಿ FALSE;
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Check the regexp program for its magic number.
 * Return TRUE if it's wrong.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
prog_magic_wrong(ಶೂನ್ಯ)
{
    regprog_T	*prog;

    prog = REG_MULTI ? rex.reg_mmatch->regprog : rex.reg_ಹೊಂದಿಕೆ->regprog;
    ಆದರೆ (prog->engine == &nfa_regengine)
	// For NFA matcher we don't check the magic
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (UCHARAT(((bt_regprog_T *)prog)->ಕಾರ್ಯಕ್ರಮ) != REGMAGIC)
    {
	iemsg(e_corrupted_regexp_ಕಾರ್ಯಕ್ರಮ);
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Cleanup the subexpressions, if this wasn't done yet.
 * This construction is used to clear the subexpressions only when they are
 * used (to increase speed).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cleanup_subexpr(ಶೂನ್ಯ)
{
    ಆದರೆ (!rex.need_clear_subexpr)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (REG_MULTI)
    {
	// Use 0xff to set lnum to -1
	vim_memset(rex.reg_startpos, 0xff, ಗಾತ್ರ(lpos_T) * NSUBEXP);
	vim_memset(rex.reg_endpos, 0xff, ಗಾತ್ರ(lpos_T) * NSUBEXP);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	vim_memset(rex.reg_startp, 0, ಗಾತ್ರ(char_u *) * NSUBEXP);
	vim_memset(rex.reg_endp, 0, ಗಾತ್ರ(char_u *) * NSUBEXP);
    }
    rex.need_clear_subexpr = FALSE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ಸ್ಥಿರ ಶೂನ್ಯ
cleanup_zsubexpr(ಶೂನ್ಯ)
{
    ಆದರೆ (!rex.need_clear_zsubexpr)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (REG_MULTI)
    {
	// Use 0xff to set lnum to -1
	vim_memset(reg_startzpos, 0xff, ಗಾತ್ರ(lpos_T) * NSUBEXP);
	vim_memset(reg_endzpos, 0xff, ಗಾತ್ರ(lpos_T) * NSUBEXP);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	vim_memset(reg_startzp, 0, ಗಾತ್ರ(char_u *) * NSUBEXP);
	vim_memset(reg_endzp, 0, ಗಾತ್ರ(char_u *) * NSUBEXP);
    }
    rex.need_clear_zsubexpr = FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Advance rex.lnum, rex.line and rex.input to the next line.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
reg_nextline(ಶೂನ್ಯ)
{
    rex.ಸಾಲು = reg_getline(++rex.lnum);
    rex.ಒಳಹರಿವು = rex.ಸಾಲು;
    fast_breakcheck();
}

/*
 * Check whether a backreference matches.
 * Returns RA_FAIL, RA_NOMATCH or RA_MATCH.
 * If "bytelen" is not NULL, it is set to the byte length of the match in the
 * last line.
 * Optional: ignore case if rex.reg_ic is set.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಹೊಂದಿಕೆ_with_backref(
    linenr_T ಆರಂಭ_lnum,
    colnr_T  ಆರಂಭ_ಲಂಬಸಾಲು,
    linenr_T ಕೊನೆ_lnum,
    colnr_T  ಕೊನೆ_ಲಂಬಸಾಲು,
    ಪೂರ್ಣಾಂಕ	     *bytelen)
{
    linenr_T	clnum = ಆರಂಭ_lnum;
    colnr_T	ccol = ಆರಂಭ_ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		len;
    char_u	*p;

    ಆದರೆ (bytelen != ಖಾಲಿ)
	*bytelen = 0;
    ಗಾಗಿ (;;)
    {
	// Since getting one line may invalidate the other, need to make copy.
	// Slow!
	ಆದರೆ (rex.ಸಾಲು != reg_tofree)
	{
	    len = (ಪೂರ್ಣಾಂಕ)STRLEN(rex.ಸಾಲು);
	    ಆದರೆ (reg_tofree == ಖಾಲಿ || len >= (ಪೂರ್ಣಾಂಕ)reg_tofreelen)
	    {
		len += 50;	// get some extra
		vim_free(reg_tofree);
		reg_tofree = alloc(len);
		ಆದರೆ (reg_tofree == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ RA_FAIL; // out of memory!
		reg_tofreelen = len;
	    }
	    STRCPY(reg_tofree, rex.ಸಾಲು);
	    rex.ಒಳಹರಿವು = reg_tofree + (rex.ಒಳಹರಿವು - rex.ಸಾಲು);
	    rex.ಸಾಲು = reg_tofree;
	}

	// Get the line to compare with.
	p = reg_getline(clnum);
	ಆದರೆ (clnum == ಕೊನೆ_lnum)
	    len = ಕೊನೆ_ಲಂಬಸಾಲು - ccol;
	ಇಲ್ಲದಿದ್ದರೆ
	    len = (ಪೂರ್ಣಾಂಕ)reg_getline_len(clnum) - ccol;

	// Use case-insensitive compare if rex.reg_ic is set
	ಆದರೆ ((!rex.reg_ic && cstrncmp(p + ccol, rex.ಒಳಹರಿವು, &len) != 0)
	    || (rex.reg_ic && MB_STRNICMP(p + ccol, rex.ಒಳಹರಿವು, len) != 0))
	    ಹಿಂತಿರುಗಿ RA_NOMATCH;  // doesn't match
	ಆದರೆ (bytelen != ಖಾಲಿ)
	    *bytelen += len;
	ಆದರೆ (clnum == ಕೊನೆ_lnum)
	    ನಿಲ್ಲಿಸು;		// match and at end!
	ಆದರೆ (rex.lnum >= rex.reg_maxline)
	    ಹಿಂತಿರುಗಿ RA_NOMATCH;  // text too short

	// Advance to next line.
	reg_nextline();
	ಆದರೆ (bytelen != ಖಾಲಿ)
	    *bytelen = 0;
	++clnum;
	ccol = 0;
	ಆದರೆ (got_int)
	    ಹಿಂತಿರುಗಿ RA_FAIL;
    }

    // found a match!  Note that rex.line may now point to a copy of the line,
    // that should not matter.
    ಹಿಂತಿರುಗಿ RA_ಹೊಂದಿಕೆ;
}

/*
 * Used in a place where no * or \+ can follow.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
re_mult_ಮುಂದಿನ(ಅಕ್ಷರ *what)
{
    ಆದರೆ (re_multi_type(peekchr()) == MULTI_MULT)
    {
	semsg(_(e_nfa_regexp_cannot_repeat_str), what);
	rc_did_emsg = TRUE;
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ a, b, c;
} decomp_T;


// 0xfb20 - 0xfb4f
ಸ್ಥಿರ decomp_T decomp_ಕೋಷ್ಟಕ[0xfb4f-0xfb20+1] =
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

    ಸ್ಥಿರ ಶೂನ್ಯ
mb_decompose(ಪೂರ್ಣಾಂಕ c, ಪೂರ್ಣಾಂಕ *c1, ಪೂರ್ಣಾಂಕ *c2, ಪೂರ್ಣಾಂಕ *c3)
{
    decomp_T d;

    ಆದರೆ (c >= 0xfb20 && c <= 0xfb4f)
    {
	d = decomp_ಕೋಷ್ಟಕ[c - 0xfb20];
	*c1 = d.a;
	*c2 = d.b;
	*c3 = d.c;
    }
    ಇಲ್ಲದಿದ್ದರೆ
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cstrncmp(char_u *s1, char_u *s2, ಪೂರ್ಣಾಂಕ *n)
{
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ;

    ಆದರೆ (!rex.reg_ic)
	ಫಲಿತಾಂಶ = STRNCMP(s1, s2, *n);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (enc_utf8)
    {
	char_u *p = s1;
	ಪೂರ್ಣಾಂಕ n2 = 0;
	ಪೂರ್ಣಾಂಕ n1 = *n;
	// count the number of characters for byte-length of s1
	ಆಗ (n1 > 0 && *p != NUL)
	{
	    n1 -= mb_ptr2len(p);
	    MB_PTR_ADV(p);
	    n2++;
	}
	// count the number of bytes to advance the same number of chars for s2
	p = s2;
	ಆಗ (n2-- > 0 && *p != NUL)
	    MB_PTR_ADV(p);

	n2 = p - s2;

	ಫಲಿತಾಂಶ = MB_STRNICMP2(s1, s2, *n, n2);
	ಆದರೆ (ಫಲಿತಾಂಶ == 0)
	    *n = n2;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಫಲಿತಾಂಶ = MB_STRNICMP(s1, s2, *n);

    // if it failed and it's utf8 and we want to combineignore:
    ಆದರೆ (ಫಲಿತಾಂಶ != 0 && enc_utf8 && rex.reg_icombine)
    {
	char_u	*str1, *str2;
	ಪೂರ್ಣಾಂಕ	c1, c2, c11, c12;
	ಪೂರ್ಣಾಂಕ	junk;

	// we have to handle the strcmp ourselves, since it is necessary to
	// deal with the composing characters by ignoring them:
	str1 = s1;
	str2 = s2;
	c1 = c2 = 0;
	ಆಗ ((ಪೂರ್ಣಾಂಕ)(str1 - s1) < *n)
	{
	    c1 = mb_ptr2char_adv(&str1);
	    c2 = mb_ptr2char_adv(&str2);

	    // Decompose the character if necessary, into 'base' characters.
	    // Currently hard-coded for Hebrew, Arabic to be done...
	    ಆದರೆ (c1 != c2 && (!rex.reg_ic || utf_fold(c1) != utf_fold(c2)))
	    {
		// decomposition necessary?
		mb_decompose(c1, &c11, &junk, &junk);
		mb_decompose(c2, &c12, &junk, &junk);
		c1 = c11;
		c2 = c12;
		ಆದರೆ (c11 != c12
			    && (!rex.reg_ic || utf_fold(c11) != utf_fold(c12)))
		    ನಿಲ್ಲಿಸು;
	    }
	}
	ಫಲಿತಾಂಶ = c2 - c1;
	ಆದರೆ (ಫಲಿತಾಂಶ == 0)
	    *n = (ಪೂರ್ಣಾಂಕ)(str2 - s2);
    }

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * cstrchr: This function is used a lot for simple searches, keep it fast!
 */
    ಸ್ಥಿರ char_u *
cstrchr(char_u *s, ಪೂರ್ಣಾಂಕ c)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		cc, lc;

    ಆದರೆ (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	ಹಿಂತಿರುಗಿ vim_strchr(s, c);

    // tolower() and toupper() can be slow, comparing twice should be a lot
    // faster (esp. when using MS Visual C++!).
    // For UTF-8 need to use folded case.
    ಆದರೆ (enc_utf8 && c > 0x80)
    {
	cc = utf_fold(c);
	lc = cc;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಆದರೆ (MB_ISUPPER(c))
	{
	    cc = MB_TOLOWER(c);
	    lc = cc;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (MB_ISLOWER(c))
	{
	    cc = MB_TOUPPER(c);
	    lc = c;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ vim_strchr(s, c);

    ಆದರೆ (has_mbyte)
    {
	ಗಾಗಿ (p = s; *p != NUL; p += (*mb_ptr2len)(p))
	{
	    ಪೂರ್ಣಾಂಕ uc = utf_ptr2char(p);
	    ಆದರೆ (enc_utf8 && (c > 0x80 || uc > 0x80))
	    {
		// Do not match an illegal byte.  E.g. 0xff matches 0xc3 0xbf,
		// not 0xff.
		// compare with lower case of the character
		ಆದರೆ ((uc < 0x80 || uc != *p) && utf_fold(uc) == lc)
		    ಹಿಂತಿರುಗಿ p;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == c || *p == cc)
		ಹಿಂತಿರುಗಿ p;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	// Faster version for when there are no multi-byte characters.
	ಗಾಗಿ (p = s; *p != NUL; ++p)
	    ಆದರೆ (*p == c || *p == cc)
		ಹಿಂತಿರುಗಿ p;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

////////////////////////////////////////////////////////////////
//		      regsub stuff			      //
////////////////////////////////////////////////////////////////

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*fptr_T)(ಪೂರ್ಣಾಂಕ *, ಪೂರ್ಣಾಂಕ);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ vim_regsub_both(char_u *ಮೂಲ, typval_T *ಅಭಿವ್ಯಕ್ತಿ, char_u *dest, ಪೂರ್ಣಾಂಕ destlen, ಪೂರ್ಣಾಂಕ flags);

    ಸ್ಥಿರ ಶೂನ್ಯ
do_ಊರ್ಧ್ವ(ಪೂರ್ಣಾಂಕ *d, ಪೂರ್ಣಾಂಕ c)
{
    *d = MB_TOUPPER(c);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
do_ಅಧೋ(ಪೂರ್ಣಾಂಕ *d, ಪೂರ್ಣಾಂಕ c)
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
regtilde(char_u *ಮೂಲ, ಪೂರ್ಣಾಂಕ magic)
{
    char_u	*newsub = ಮೂಲ;
    char_u	*p;
    size_t	newsublen = 0;
    char_u	tilde[3] = {'~', NUL, NUL};
    size_t	tildelen = 1;
    ಪೂರ್ಣಾಂಕ		error = FALSE;

    ಆದರೆ (!magic)
    {
	tilde[0] = '\\';
	tilde[1] = '~';
	tilde[2] = NUL;
	tildelen = 2;
    }

    ಗಾಗಿ (p = newsub; *p; ++p)
    {
	ಆದರೆ (STRNCMP(p, tilde, tildelen) == 0)
	{
	    size_t prefixlen = p - newsub;		// not including the tilde
	    char_u *postfix = p + tildelen;
	    size_t postfixlen;
	    size_t tmpsublen;

	    ಆದರೆ (newsublen == 0)
		newsublen = STRLEN(newsub);
	    newsublen -= tildelen;
	    postfixlen = newsublen - prefixlen;
	    tmpsublen = prefixlen + reg_prev_sublen + postfixlen;

	    ಆದರೆ (tmpsublen > 0 && reg_prev_sub != ಖಾಲಿ)
	    {
		char_u *tmpsub;

		// Avoid making the text longer than MAXCOL, it will cause
		// trouble at some point.
		ಆದರೆ (tmpsublen > MAXCOL)
		{
		    emsg(_(e_resulting_ಪಠ್ಯ_too_long));
		    error = TRUE;
		    ನಿಲ್ಲಿಸು;
		}

		tmpsub = alloc(tmpsublen + 1);
		ಆದರೆ (tmpsub == ಖಾಲಿ)
		{
		    emsg(_(e_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ));
		    error = TRUE;
		    ನಿಲ್ಲಿಸು;
		}

		// copy prefix
		mch_memmove(tmpsub, newsub, prefixlen);
		// interpret tilde
		mch_memmove(tmpsub + prefixlen, reg_prev_sub, reg_prev_sublen);
		// copy postfix
		STRCPY(tmpsub + prefixlen + reg_prev_sublen, postfix);

		ಆದರೆ (newsub != ಮೂಲ)	// allocated newsub before
		    vim_free(newsub);
		newsub = tmpsub;
		newsublen = tmpsublen;
		p = newsub + prefixlen + reg_prev_sublen;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		mch_memmove(p, postfix, postfixlen + 1);	// remove the tilde (+1 for the NUL)

	    --p;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (*p == '\\' && p[1])		// skip escaped characters
		++p;
	    ಆದರೆ (has_mbyte)
		p += (*mb_ptr2len)(p) - 1;
	}
    }

    ಆದರೆ (error)
    {
	ಆದರೆ (newsub != ಮೂಲ)
	    vim_free(newsub);
	ಹಿಂತಿರುಗಿ ಮೂಲ;
    }

    // Store a copy of newsub  in reg_prev_sub.  It is always allocated,
    // because recursive calls may make the returned string invalid.
    // Only store it if there something to store.
    newsublen = p - newsub;
    ಆದರೆ (newsublen == 0)
	VIM_CLEAR(reg_prev_sub);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	vim_free(reg_prev_sub);
	reg_prev_sub = vim_strnsave(newsub, newsublen);
    }

    ಆದರೆ (reg_prev_sub == ಖಾಲಿ)
	reg_prev_sublen = 0;
    ಇಲ್ಲದಿದ್ದರೆ
	reg_prev_sublen = newsublen;

    ಹಿಂತಿರುಗಿ newsub;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL

/*
 * Put the submatches in "argv[argskip]" which is a list passed into
 * call_func() by vim_regsub_both().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
fill_submatch_list(ಪೂರ್ಣಾಂಕ argc ಬಳಸದ, typval_T *argv, ಪೂರ್ಣಾಂಕ argskip, ufunc_T *fp)
{
    listitem_T	*li;
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*s;
    typval_T	*listarg = argv + argskip;

    ಆದರೆ (!has_varargs(fp) && fp->uf_ವಾದಗಳು.ga_len <= argskip)
	// called function doesn't take a submatches argument
	ಹಿಂತಿರುಗಿ argskip;

    // Relies on sl_list to be the first item in staticList10_T.
    init_static_list((staticList10_T *)(listarg->vval.v_list));

    // There are always 10 list items in staticList10_T.
    li = listarg->vval.v_list->lv_ಮೊದಲ;
    ಗಾಗಿ (i = 0; i < 10; ++i)
    {
	s = rsm.sm_ಹೊಂದಿಕೆ->startp[i];
	ಆದರೆ (s == ಖಾಲಿ || rsm.sm_ಹೊಂದಿಕೆ->endp[i] == ಖಾಲಿ)
	    s = ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	    s = vim_strnsave(s, rsm.sm_ಹೊಂದಿಕೆ->endp[i] - s);
	li->li_tv.v_type = VAR_STRING;
	li->li_tv.vval.v_string = s;
	li = li->li_ಮುಂದಿನ;
    }
    ಹಿಂತಿರುಗಿ argskip + 1;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clear_submatch_list(staticList10_T *sl)
{
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 0; i < 10; ++i)
	vim_free(sl->sl_ವಸ್ತುಗಳು[i].li_tv.vval.v_string);
}
#ಆದರೆ_ಅಂತ್ಯ

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
    ಪೂರ್ಣಾಂಕ
vim_regsub(
    regmatch_T	*rmp,
    char_u	*ಮೂಲ,
    typval_T	*ಅಭಿವ್ಯಕ್ತಿ,
    char_u	*dest,
    ಪೂರ್ಣಾಂಕ		destlen,
    ಪೂರ್ಣಾಂಕ		flags)
{
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ;
    regexec_T	rex_save;
    ಪೂರ್ಣಾಂಕ		rex_ಒಳ_use_save = rex_ಒಳ_use;

    ಆದರೆ (rex_ಒಳ_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_ಒಳ_use = TRUE;

    rex.reg_ಹೊಂದಿಕೆ = rmp;
    rex.reg_mmatch = ಖಾಲಿ;
    rex.reg_maxline = 0;
    rex.reg_buf = curbuf;
    rex.reg_ಸಾಲು_lbr = TRUE;
    ಫಲಿತಾಂಶ = vim_regsub_both(ಮೂಲ, ಅಭಿವ್ಯಕ್ತಿ, dest, destlen, flags);

    rex_ಒಳ_use = rex_ಒಳ_use_save;
    ಆದರೆ (rex_ಒಳ_use)
	rex = rex_save;

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

    ಪೂರ್ಣಾಂಕ
vim_regsub_multi(
    regmmatch_T	*rmp,
    linenr_T	lnum,
    char_u	*ಮೂಲ,
    char_u	*dest,
    ಪೂರ್ಣಾಂಕ		destlen,
    ಪೂರ್ಣಾಂಕ		flags)
{
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ;
    regexec_T	rex_save;
    ಪೂರ್ಣಾಂಕ		rex_ಒಳ_use_save = rex_ಒಳ_use;

    ಆದರೆ (rex_ಒಳ_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_ಒಳ_use = TRUE;

    rex.reg_ಹೊಂದಿಕೆ = ಖಾಲಿ;
    rex.reg_mmatch = rmp;
    rex.reg_buf = curbuf;	// always works on the current buffer!
    rex.reg_firstlnum = lnum;
    rex.reg_maxline = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - lnum;
    rex.reg_ಸಾಲು_lbr = FALSE;
    ಫಲಿತಾಂಶ = vim_regsub_both(ಮೂಲ, ಖಾಲಿ, dest, destlen, flags);

    rex_ಒಳ_use = rex_ಒಳ_use_save;
    ಆದರೆ (rex_ಒಳ_use)
	rex = rex_save;

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ defined(FEAT_EVAL)
// When nesting more than a couple levels it's probably a mistake.
# define MAX_REGSUB_NESTING 4
ಸ್ಥಿರ char_u   *eval_ಫಲಿತಾಂಶ[MAX_REGSUB_NESTING] = {ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ};

# ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
free_resub_eval_ಫಲಿತಾಂಶ(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 0; i < MAX_REGSUB_NESTING; ++i)
	VIM_CLEAR(eval_ಫಲಿತಾಂಶ[i]);
}
# endif
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
vim_regsub_both(
    char_u	*ಮೂಲ,
    typval_T	*ಅಭಿವ್ಯಕ್ತಿ,
    char_u	*dest,
    ಪೂರ್ಣಾಂಕ		destlen,
    ಪೂರ್ಣಾಂಕ		flags)
{
    char_u	*src;
    char_u	*dst;
    char_u	*s;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		cc;
    ಪೂರ್ಣಾಂಕ		no = -1;
    fptr_T	func_all = (fptr_T)ಖಾಲಿ;
    fptr_T	func_one = (fptr_T)ಖಾಲಿ;
    linenr_T	clnum = 0;	// init for GCC
    ಪೂರ್ಣಾಂಕ		len = 0;	// init for GCC
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  nesting = 0;
    ಪೂರ್ಣಾಂಕ		nested;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		copy = flags & REGSUB_COPY;

    // Be paranoid...
    ಆದರೆ ((ಮೂಲ == ಖಾಲಿ && ಅಭಿವ್ಯಕ್ತಿ == ಖಾಲಿ) || dest == ಖಾಲಿ)
    {
	iemsg(e_null_ವಾದ);
	ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (prog_magic_wrong())
	ಹಿಂತಿರುಗಿ 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (nesting == MAX_REGSUB_NESTING)
    {
	emsg(_(e_substitute_nesting_too_deep));
	ಹಿಂತಿರುಗಿ 0;
    }
    nested = nesting;
#ಆದರೆ_ಅಂತ್ಯ
    src = ಮೂಲ;
    dst = dest;

    /*
     * When the substitute part starts with "\=" evaluate it as an expression.
     */
    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ != ಖಾಲಿ || (ಮೂಲ[0] == '\\' && ಮೂಲ[1] == '='))
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	// To make sure that the length doesn't change between checking the
	// length and copying the string, and to speed up things, the
	// resulting string is saved from the call with
	// "flags & REGSUB_COPY" == 0 to the call with
	// "flags & REGSUB_COPY" != 0.
	ಆದರೆ (copy)
	{
	    ಆದರೆ (eval_ಫಲಿತಾಂಶ[nested] != ಖಾಲಿ)
	    {
		ಪೂರ್ಣಾಂಕ eval_len = (ಪೂರ್ಣಾಂಕ)STRLEN(eval_ಫಲಿತಾಂಶ[nested]);

		ಆದರೆ (eval_len < destlen)
		{
		    STRCPY(dest, eval_ಫಲಿತಾಂಶ[nested]);
		    dst += eval_len;
		    VIM_CLEAR(eval_ಫಲಿತಾಂಶ[nested]);
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ		    prev_can_f_submatch = can_f_submatch;
	    regsubmatch_T   rsm_save;

	    VIM_CLEAR(eval_ಫಲಿತಾಂಶ[nested]);

	    // The expression may contain substitute(), which calls us
	    // recursively.  Make sure submatch() gets the text from the first
	    // level.
	    ಆದರೆ (can_f_submatch)
		rsm_save = rsm;
	    can_f_submatch = TRUE;
	    rsm.sm_ಹೊಂದಿಕೆ = rex.reg_ಹೊಂದಿಕೆ;
	    rsm.sm_mmatch = rex.reg_mmatch;
	    rsm.sm_firstlnum = rex.reg_firstlnum;
	    rsm.sm_maxline = rex.reg_maxline;
	    rsm.sm_ಸಾಲು_lbr = rex.reg_ಸಾಲು_lbr;

	    // Although unlikely, it is possible that the expression invokes a
	    // substitute command (it might fail, but still).  Therefore keep
	    // an array of eval results.
	    ++nesting;

	    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ != ಖಾಲಿ)
	    {
		typval_T	argv[2];
		char_u		buf[NUMBUFLEN];
		typval_T	rettv;
		staticList10_T	ಹೊಂದಿಕೆ_List;
		funcexe_T	funcexe;

		rettv.v_type = VAR_STRING;
		rettv.vval.v_string = ಖಾಲಿ;
		argv[0].v_type = VAR_LIST;
		argv[0].vval.v_list = &ಹೊಂದಿಕೆ_List.sl_list;
		ಹೊಂದಿಕೆ_List.sl_list.lv_len = 0;
		CLEAR_ಕ್ಷೇತ್ರ(funcexe);
		funcexe.fe_argv_func = fill_submatch_list;
		funcexe.fe_evaluate = TRUE;
		ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ->v_type == VAR_FUNC)
		{
		    s = ಅಭಿವ್ಯಕ್ತಿ->vval.v_string;
		    call_func(s, -1, &rettv, 1, argv, &funcexe);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ->v_type == VAR_PARTIAL)
		{
		    partial_T   *partial = ಅಭಿವ್ಯಕ್ತಿ->vval.v_partial;

		    s = partial_name(partial);
		    funcexe.fe_partial = partial;
		    call_func(s, -1, &rettv, 1, argv, &funcexe);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ->v_type == VAR_INSTR)
		{
		    exe_typval_instr(ಅಭಿವ್ಯಕ್ತಿ, &rettv);
		}
		ಆದರೆ (ಹೊಂದಿಕೆ_List.sl_list.lv_len > 0)
		    // fill_submatch_list() was called
		    clear_submatch_list(&ಹೊಂದಿಕೆ_List);

		ಆದರೆ (rettv.v_type == VAR_UNKNOWN)
		    // something failed, no need to report another error
		    eval_ಫಲಿತಾಂಶ[nested] = ಖಾಲಿ;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    eval_ಫಲಿತಾಂಶ[nested] = tv_get_string_buf_chk(&rettv, buf);
		    ಆದರೆ (eval_ಫಲಿತಾಂಶ[nested] != ಖಾಲಿ)
			eval_ಫಲಿತಾಂಶ[nested] = vim_strsave(eval_ಫಲಿತಾಂಶ[nested]);
		}
		clear_tv(&rettv);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (substitute_instr != ಖಾಲಿ)
		// Execute instructions from ISN_SUBSTITUTE.
		eval_ಫಲಿತಾಂಶ[nested] = exe_substitute_instr();
	    ಇಲ್ಲದಿದ್ದರೆ
		eval_ಫಲಿತಾಂಶ[nested] = eval_ಗಮ್ಯ_string(ಮೂಲ + 2, TRUE, FALSE);
	    --nesting;

	    ಆದರೆ (eval_ಫಲಿತಾಂಶ[nested] != ಖಾಲಿ)
	    {
		ಪೂರ್ಣಾಂಕ had_backslash = FALSE;

		ಗಾಗಿ (s = eval_ಫಲಿತಾಂಶ[nested]; *s != NUL; MB_PTR_ADV(s))
		{
		    // Change NL to CR, so that it becomes a line break,
		    // unless called from vim_regexec_nl().
		    // Skip over a backslashed character.
		    ಆದರೆ (*s == NL && !rsm.sm_ಸಾಲು_lbr)
			*s = CAR;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*s == '\\' && s[1] != NUL)
		    {
			++s;
			/* Change NL to CR here too, so that this works:
			 * :s/abc\\\ndef/\="aaa\\\nbbb"/  on text:
			 *   abc\
			 *   def
			 * Not when called from vim_regexec_nl().
			 */
			ಆದರೆ (*s == NL && !rsm.sm_ಸಾಲು_lbr)
			    *s = CAR;
			had_backslash = TRUE;
		    }
		}
		ಆದರೆ (had_backslash && (flags & REGSUB_BACKSLASH))
		{
		    // Backslashes will be consumed, need to double them.
		    s = vim_strsave_escaped(eval_ಫಲಿತಾಂಶ[nested], (char_u *)"\\");
		    ಆದರೆ (s != ಖಾಲಿ)
		    {
			vim_free(eval_ಫಲಿತಾಂಶ[nested]);
			eval_ಫಲಿತಾಂಶ[nested] = s;
		    }
		}

		dst += STRLEN(eval_ಫಲಿತಾಂಶ[nested]);
	    }

	    can_f_submatch = prev_can_f_submatch;
	    ಆದರೆ (can_f_submatch)
		rsm = rsm_save;
	}
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಆಗ ((c = *src++) != NUL)
      {
	ಆದರೆ (c == '&' && (flags & REGSUB_MAGIC))
	    no = 0;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '\\' && *src != NUL)
	{
	    ಆದರೆ (*src == '&' && !(flags & REGSUB_MAGIC))
	    {
		++src;
		no = 0;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ('0' <= *src && *src <= '9')
	    {
		no = *src++ - '0';
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_strchr((char_u *)"uUlLeE", *src))
	    {
		ಆಯ್ಕೆ (*src++)
		{
		ಸ್ಥಿತಿ 'u':   func_one = do_ಊರ್ಧ್ವ;
			    ಮುಂದುವರಿ;
		ಸ್ಥಿತಿ 'U':   func_all = do_ಊರ್ಧ್ವ;
			    ಮುಂದುವರಿ;
		ಸ್ಥಿತಿ 'l':   func_one = do_ಅಧೋ;
			    ಮುಂದುವರಿ;
		ಸ್ಥಿತಿ 'L':   func_all = do_ಅಧೋ;
			    ಮುಂದುವರಿ;
		ಸ್ಥಿತಿ 'e':
		ಸ್ಥಿತಿ 'E':   func_one = func_all = (fptr_T)ಖಾಲಿ;
			    ಮುಂದುವರಿ;
		}
	    }
	}
	ಆದರೆ (no < 0)	      // Ordinary character.
	{
	    ಆದರೆ (c == K_SPECIAL && src[0] != NUL && src[1] != NUL)
	    {
		// Copy a special key as-is.
		ಆದರೆ (copy)
		{
		    ಆದರೆ (dst + 3 > dest + destlen)
		    {
			iemsg("vim_regsub_both(): not enough space");
			ಹಿಂತಿರುಗಿ 0;
		    }
		    *dst++ = c;
		    *dst++ = *src++;
		    *dst++ = *src++;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    dst += 3;
		    src += 2;
		}
		ಮುಂದುವರಿ;
	    }

	    ಆದರೆ (c == '\\' && *src != NUL)
	    {
		// Check for abbreviations -- webb
		ಆಯ್ಕೆ (*src)
		{
		    ಸ್ಥಿತಿ 'r':	c = CAR;	++src;	ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'n':	c = NL;		++src;	ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 't':	c = TAB;	++src;	ನಿಲ್ಲಿಸು;
		 // Oh no!  \e already has meaning in subst pat :-(
		 // case 'e':   c = ESC;	++src;	break;
		    ಸ್ಥಿತಿ 'b':	c = Ctrl_H;	++src;	ನಿಲ್ಲಿಸು;

		    // If "backslash" is TRUE the backslash will be removed
		    // later.  Used to insert a literal CR.
		    ಪೂರ್ವನಿಯೋಜಿತ:	ಆದರೆ (flags & REGSUB_BACKSLASH)
				{
				    ಆದರೆ (copy)
				    {
					ಆದರೆ (dst + 1 > dest + destlen)
					{
					    iemsg("vim_regsub_both(): not enough space");
					    ಹಿಂತಿರುಗಿ 0;
					}
					*dst = '\\';
				    }
				    ++dst;
				}
				c = *src++;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte)
		c = mb_ptr2char(src - 1);

	    // Write to buffer, if copy is set.
	    ಆದರೆ (func_one != (fptr_T)ಖಾಲಿ)
	    {
		func_one(&cc, c);
		func_one = ಖಾಲಿ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (func_all != (fptr_T)ಖಾಲಿ)
		func_all(&cc, c);
	    ಇಲ್ಲದಿದ್ದರೆ // just copy
		cc = c;

	    ಆದರೆ (has_mbyte)
	    {
		ಪೂರ್ಣಾಂಕ totlen = mb_ptr2len(src - 1);
		ಪೂರ್ಣಾಂಕ charlen = mb_char2len(cc);

		ಆದರೆ (copy)
		{
		    ಆದರೆ (dst + charlen > dest + destlen)
		    {
			iemsg("vim_regsub_both(): not enough space");
			ಹಿಂತಿರುಗಿ 0;
		    }
		    mb_char2bytes(cc, dst);
		}
		dst += charlen - 1;
		ಆದರೆ (enc_utf8)
		{
		    ಪೂರ್ಣಾಂಕ clen = utf_ptr2len(src - 1);

		    // If the character length is shorter than "totlen", there
		    // are composing characters; copy them as-is.
		    ಆದರೆ (clen < totlen)
		    {
			ಆದರೆ (copy)
			{
			    ಆದರೆ (dst + totlen - clen > dest + destlen)
			    {
				iemsg("vim_regsub_both(): not enough space");
				ಹಿಂತಿರುಗಿ 0;
			    }
			    mch_memmove(dst + 1, src - 1 + clen,
						     (size_t)(totlen - clen));
			}
			dst += totlen - clen;
		    }
		}
		src += totlen - 1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (copy)
	    {
		ಆದರೆ (dst + 1 > dest + destlen)
		{
		    iemsg("vim_regsub_both(): not enough space");
		    ಹಿಂತಿರುಗಿ 0;
		}
		*dst = cc;
	    }
	    dst++;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (REG_MULTI)
	    {
		clnum = rex.reg_mmatch->startpos[no].lnum;
		ಆದರೆ (clnum < 0 || rex.reg_mmatch->endpos[no].lnum < 0)
		    s = ಖಾಲಿ;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    s = reg_getline(clnum) + rex.reg_mmatch->startpos[no].ಲಂಬಸಾಲು;
		    ಆದರೆ (rex.reg_mmatch->endpos[no].lnum == clnum)
			len = rex.reg_mmatch->endpos[no].ಲಂಬಸಾಲು
					    - rex.reg_mmatch->startpos[no].ಲಂಬಸಾಲು;
		    ಇಲ್ಲದಿದ್ದರೆ
			len = (ಪೂರ್ಣಾಂಕ)reg_getline_len(clnum) - rex.reg_mmatch->startpos[no].ಲಂಬಸಾಲು;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		s = rex.reg_ಹೊಂದಿಕೆ->startp[no];
		ಆದರೆ (rex.reg_ಹೊಂದಿಕೆ->endp[no] == ಖಾಲಿ)
		    s = ಖಾಲಿ;
		ಇಲ್ಲದಿದ್ದರೆ
		    len = (ಪೂರ್ಣಾಂಕ)(rex.reg_ಹೊಂದಿಕೆ->endp[no] - s);
	    }
	    ಆದರೆ (s != ಖಾಲಿ)
	    {
		ಗಾಗಿ (;;)
		{
		    ಆದರೆ (len == 0)
		    {
			ಆದರೆ (REG_MULTI)
			{
			    ಆದರೆ (rex.reg_mmatch->endpos[no].lnum == clnum)
				ನಿಲ್ಲಿಸು;
			    ಆದರೆ (copy)
			    {
				ಆದರೆ (dst + 1 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    ಹಿಂತಿರುಗಿ 0;
				}
				*dst = CAR;
			    }
			    ++dst;
			    s = reg_getline(++clnum);
			    ಆದರೆ (rex.reg_mmatch->endpos[no].lnum == clnum)
				len = rex.reg_mmatch->endpos[no].ಲಂಬಸಾಲು;
			    ಇಲ್ಲದಿದ್ದರೆ
				len = (ಪೂರ್ಣಾಂಕ)reg_getline_len(clnum);
			}
			ಇಲ್ಲದಿದ್ದರೆ
			    ನಿಲ್ಲಿಸು;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*s == NUL) // we hit NUL.
		    {
			ಆದರೆ (copy)
			    iemsg(e_damaged_ಹೊಂದಿಕೆ_string);
			ಹೋಗು ನಿರ್ಗಮಿಸು;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ ((flags & REGSUB_BACKSLASH)
						  && (*s == CAR || *s == '\\'))
			{
			    /*
			     * Insert a backslash in front of a CR, otherwise
			     * it will be replaced by a line break.
			     * Number of backslashes will be halved later,
			     * double them here.
			     */
			    ಆದರೆ (copy)
			    {
				ಆದರೆ (dst + 2 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    ಹಿಂತಿರುಗಿ 0;
				}
				dst[0] = '\\';
				dst[1] = *s;
			    }
			    dst += 2;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    ಆದರೆ (has_mbyte)
				c = mb_ptr2char(s);
			    ಇಲ್ಲದಿದ್ದರೆ
				c = *s;

			    ಆದರೆ (func_one != (fptr_T)ಖಾಲಿ)
			    {
				func_one(&cc, c);
				func_one = ಖಾಲಿ;
			    }
			    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (func_all != (fptr_T)ಖಾಲಿ)
				func_all(&cc, c);
			    ಇಲ್ಲದಿದ್ದರೆ // just copy
				cc = c;

			    ಆದರೆ (has_mbyte)
			    {
				ಪೂರ್ಣಾಂಕ l;
				ಪೂರ್ಣಾಂಕ charlen;

				// Copy composing characters separately, one
				// at a time.
				ಆದರೆ (enc_utf8)
				    l = utf_ptr2len(s) - 1;
				ಇಲ್ಲದಿದ್ದರೆ
				    l = mb_ptr2len(s) - 1;

				s += l;
				len -= l;
				charlen = mb_char2len(cc);
				ಆದರೆ (copy)
				{
				    ಆದರೆ (dst + charlen > dest + destlen)
				    {
					iemsg("vim_regsub_both(): not enough space");
					ಹಿಂತಿರುಗಿ 0;
				    }
				    mb_char2bytes(cc, dst);
				}
				dst += charlen - 1;
			    }
			    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (copy)
			    {
				ಆದರೆ (dst + 1 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    ಹಿಂತಿರುಗಿ 0;
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
    ಆದರೆ (copy)
	*dst = NUL;

ನಿರ್ಗಮಿಸು:
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)((dst - dest) + 1);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL

    ಸ್ಥಿರ char_u *
reg_getline_submatch(linenr_T lnum)
{
    char_u *ಸಾಲು;

    reg_getline_common(lnum, RGLF_ಸಾಲು | RGLF_SUBMATCH, &ಸಾಲು, ಖಾಲಿ);

    ಹಿಂತಿರುಗಿ ಸಾಲು;
}

    ಸ್ಥಿರ colnr_T
reg_getline_submatch_len(linenr_T lnum)
{
    colnr_T ಉದ್ದಳತೆ;

    reg_getline_common(lnum, RGLF_ಉದ್ದಳತೆ | RGLF_SUBMATCH, ಖಾಲಿ, &ಉದ್ದಳತೆ);

    ಹಿಂತಿರುಗಿ ಉದ್ದಳತೆ;
}

/*
 * Used for the submatch() function: get the string from the n'th submatch in
 * allocated memory.
 * Returns NULL when not in a ":s" command and for a non-existing submatch.
 */
    char_u *
reg_submatch(ಪೂರ್ಣಾಂಕ no)
{
    char_u	*retval = ಖಾಲಿ;
    char_u	*s;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		ಸುತ್ತು;
    linenr_T	lnum;

    ಆದರೆ (!can_f_submatch || no < 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (rsm.sm_ಹೊಂದಿಕೆ == ಖಾಲಿ)
    {
	/*
	 * First round: compute the length and allocate memory.
	 * Second round: copy the text.
	 */
	ಗಾಗಿ (ಸುತ್ತು = 1; ಸುತ್ತು <= 2; ++ಸುತ್ತು)
	{
	    lnum = rsm.sm_mmatch->startpos[no].lnum;
	    ಆದರೆ (lnum < 0 || rsm.sm_mmatch->endpos[no].lnum < 0)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	    s = reg_getline_submatch(lnum);
	    ಆದರೆ (s == ಖಾಲಿ)  // anti-crash check, cannot happen?
		ನಿಲ್ಲಿಸು;
	    s += rsm.sm_mmatch->startpos[no].ಲಂಬಸಾಲು;
	    ಆದರೆ (rsm.sm_mmatch->endpos[no].lnum == lnum)
	    {
		// Within one line: take form start to end col.
		len = rsm.sm_mmatch->endpos[no].ಲಂಬಸಾಲು
					  - rsm.sm_mmatch->startpos[no].ಲಂಬಸಾಲು;
		ಆದರೆ (ಸುತ್ತು == 2)
		    vim_strncpy(retval, s, len);
		++len;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Multiple lines: take start line from start col, middle
		// lines completely and end line up to end col.
		len = (ಪೂರ್ಣಾಂಕ)reg_getline_submatch_len(lnum) - rsm.sm_mmatch->startpos[no].ಲಂಬಸಾಲು;
		ಆದರೆ (ಸುತ್ತು == 2)
		{
		    STRCPY(retval, s);
		    retval[len] = '\n';
		}
		++len;
		++lnum;
		ಆಗ (lnum < rsm.sm_mmatch->endpos[no].lnum)
		{
		    s = reg_getline_submatch(lnum);
		    ಆದರೆ (ಸುತ್ತು == 2)
			STRCPY(retval + len, s);
		    len += (ಪೂರ್ಣಾಂಕ)reg_getline_submatch_len(lnum);
		    ಆದರೆ (ಸುತ್ತು == 2)
			retval[len] = '\n';
		    ++len;
		    ++lnum;
		}
		ಆದರೆ (ಸುತ್ತು == 2)
		    STRNCPY(retval + len, reg_getline_submatch(lnum),
					     rsm.sm_mmatch->endpos[no].ಲಂಬಸಾಲು);
		len += rsm.sm_mmatch->endpos[no].ಲಂಬಸಾಲು;
		ಆದರೆ (ಸುತ್ತು == 2)
		    retval[len] = NUL;
		++len;
	    }

	    ಆದರೆ (retval == ಖಾಲಿ)
	    {
		retval = alloc(len);
		ಆದರೆ (retval == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	s = rsm.sm_ಹೊಂದಿಕೆ->startp[no];
	ಆದರೆ (s == ಖಾಲಿ || rsm.sm_ಹೊಂದಿಕೆ->endp[no] == ಖಾಲಿ)
	    retval = ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	    retval = vim_strnsave(s, rsm.sm_ಹೊಂದಿಕೆ->endp[no] - s);
    }

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Used for the submatch() function with the optional non-zero argument: get
 * the list of strings from the n'th submatch in allocated memory with NULs
 * represented in NLs.
 * Returns a list of allocated strings.  Returns NULL when not in a ":s"
 * command, for a non-existing submatch and for any error.
 */
    list_T *
reg_submatch_list(ಪೂರ್ಣಾಂಕ no)
{
    char_u	*s;
    linenr_T	slnum;
    linenr_T	elnum;
    colnr_T	scol;
    colnr_T	ecol;
    ಪೂರ್ಣಾಂಕ		i;
    list_T	*list;
    ಪೂರ್ಣಾಂಕ		error = FALSE;

    ಆದರೆ (!can_f_submatch || no < 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (rsm.sm_ಹೊಂದಿಕೆ == ಖಾಲಿ)
    {
	slnum = rsm.sm_mmatch->startpos[no].lnum;
	elnum = rsm.sm_mmatch->endpos[no].lnum;
	ಆದರೆ (slnum < 0 || elnum < 0)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	scol = rsm.sm_mmatch->startpos[no].ಲಂಬಸಾಲು;
	ecol = rsm.sm_mmatch->endpos[no].ಲಂಬಸಾಲು;

	list = list_alloc();
	ಆದರೆ (list == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	s = reg_getline_submatch(slnum) + scol;
	ಆದರೆ (slnum == elnum)
	{
	    ಆದರೆ (list_append_string(list, s, ecol - scol) == FAIL)
		error = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ max_lnum = elnum - slnum;

	    ಆದರೆ (list_append_string(list, s, -1) == FAIL)
		error = TRUE;
	    ಗಾಗಿ (i = 1; i < max_lnum; i++)
	    {
		s = reg_getline_submatch(slnum + i);
		ಆದರೆ (list_append_string(list, s, -1) == FAIL)
		    error = TRUE;
	    }
	    s = reg_getline_submatch(elnum);
	    ಆದರೆ (list_append_string(list, s, ecol) == FAIL)
		error = TRUE;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	s = rsm.sm_ಹೊಂದಿಕೆ->startp[no];
	ಆದರೆ (s == ಖಾಲಿ || rsm.sm_ಹೊಂದಿಕೆ->endp[no] == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	list = list_alloc();
	ಆದರೆ (list == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಆದರೆ (list_append_string(list, s,
				 (ಪೂರ್ಣಾಂಕ)(rsm.sm_ಹೊಂದಿಕೆ->endp[no] - s)) == FAIL)
	    error = TRUE;
    }

    ಆದರೆ (error)
    {
	list_free(list);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ++list->lv_refcount;
    ಹಿಂತಿರುಗಿ list;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Initialize the values used for matching against multiple lines
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_regexec_multi(
	regmmatch_T	*rmp,
	win_T		*win,	// window in which to search or NULL
	buf_T		*buf,	// buffer in which to search
	linenr_T	lnum)	// nr of line to start looking for match
{
    rex.reg_ಹೊಂದಿಕೆ = ಖಾಲಿ;
    rex.reg_mmatch = rmp;
    rex.reg_buf = buf;
    rex.reg_win = win;
    rex.reg_firstlnum = lnum;
    rex.reg_maxline = rex.reg_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - lnum;
    rex.reg_ಸಾಲು_lbr = FALSE;
    rex.reg_ic = rmp->rmm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = rmp->rmm_maxcol;
}

#ವ್ಯಾಖ್ಯಾನಿಸು CLASS_not		0x80
#ವ್ಯಾಖ್ಯಾನಿಸು CLASS_af		0x40
#ವ್ಯಾಖ್ಯಾನಿಸು CLASS_AF		0x20
#ವ್ಯಾಖ್ಯಾನಿಸು CLASS_az		0x10
#ವ್ಯಾಖ್ಯಾನಿಸು CLASS_AZ		0x08
#ವ್ಯಾಖ್ಯಾನಿಸು CLASS_o7		0x04
#ವ್ಯಾಖ್ಯಾನಿಸು CLASS_o9		0x02
#ವ್ಯಾಖ್ಯಾನಿಸು CLASS_underscore	0x01

/*
 * Parse the "[...]" collection between "start" (the first character after the
 * "[") and "end" (the "]") into a combination of CLASS_ bits, so that it can
 * be turned into a faster character class like \d or \x.  Sets "*newl" to TRUE
 * when the collection also matches a newline.  Returns -1 when the collection
 * is not a plain combination of the recognized ranges.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_char_class_ಬಿಟ್ಟುಗಳು(char_u *ಆರಂಭ, char_u *ಕೊನೆ, ಪೂರ್ಣಾಂಕ *newl)
{
    char_u	*p = ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ		config = 0;

    *newl = FALSE;
    ಆದರೆ (*ಕೊನೆ != ']')
	ಹಿಂತಿರುಗಿ -1;
    ಆದರೆ (*p == '^')
    {
	config |= CLASS_not;
	p++;
    }

    ಆಗ (p < ಕೊನೆ)
    {
	ಆದರೆ (p + 2 < ಕೊನೆ && *(p + 1) == '-')
	{
	    ಆಯ್ಕೆ (*p)
	    {
		ಸ್ಥಿತಿ '0':
		    ಆದರೆ (*(p + 2) == '9')
		    {
			config |= CLASS_o9;
			ನಿಲ್ಲಿಸು;
		    }
		    ಆದರೆ (*(p + 2) == '7')
		    {
			config |= CLASS_o7;
			ನಿಲ್ಲಿಸು;
		    }
		    ಹಿಂತಿರುಗಿ -1;

		ಸ್ಥಿತಿ 'a':
		    ಆದರೆ (*(p + 2) == 'z')
		    {
			config |= CLASS_az;
			ನಿಲ್ಲಿಸು;
		    }
		    ಆದರೆ (*(p + 2) == 'f')
		    {
			config |= CLASS_af;
			ನಿಲ್ಲಿಸು;
		    }
		    ಹಿಂತಿರುಗಿ -1;

		ಸ್ಥಿತಿ 'A':
		    ಆದರೆ (*(p + 2) == 'Z')
		    {
			config |= CLASS_AZ;
			ನಿಲ್ಲಿಸು;
		    }
		    ಆದರೆ (*(p + 2) == 'F')
		    {
			config |= CLASS_AF;
			ನಿಲ್ಲಿಸು;
		    }
		    ಹಿಂತಿರುಗಿ -1;

		ಪೂರ್ವನಿಯೋಜಿತ:
		    ಹಿಂತಿರುಗಿ -1;
	    }
	    p += 3;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p + 1 < ಕೊನೆ && *p == '\\' && *(p + 1) == 'n')
	{
	    *newl = TRUE;
	    p += 2;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '_')
	{
	    config |= CLASS_underscore;
	    p++;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '\n')
	{
	    *newl = TRUE;
	    p++;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ -1;
    } // while (p < end)

    ಆದರೆ (p != ಕೊನೆ)
	ಹಿಂತಿರುಗಿ -1;

    // U+017F and U+212A fold to ASCII 's' and 'k', missed by the class opcodes
    ಆದರೆ (enc_utf8 && (config & (CLASS_az | CLASS_AZ)))
	ಹಿಂತಿರುಗಿ -1;

    ಹಿಂತಿರುಗಿ config;
}

#ಅನ್ತರ್ಭಾವ "regexp_bt.c"

ಸ್ಥಿರ regengine_T bt_regengine =
{
    bt_regcomp,
    bt_regfree,
    bt_regexec_nl,
    bt_regexec_multi
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
    ,(char_u *)""
#ಆದರೆ_ಅಂತ್ಯ
};

#ಅನ್ತರ್ಭಾವ "regexp_nfa.c"

ಸ್ಥಿರ regengine_T nfa_regengine =
{
    nfa_regcomp,
    nfa_regfree,
    nfa_regexec_nl,
    nfa_regexec_multi
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
    ,(char_u *)""
#ಆದರೆ_ಅಂತ್ಯ
};

// Which regexp engine to use? Needed for vim_regcomp().
// Must match with 'regexpengine'.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ regexp_engine = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char_u regname[][30] = {
		    "AUTOMATIC Regexp Engine",
		    "BACKTRACKING Regexp Engine",
		    "NFA Regexp Engine"
			    };
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Compile a regular expression into internal code.
 * Returns the program in allocated memory.
 * Use vim_regfree() to free the memory.
 * Returns NULL for an error.
 */
    regprog_T *
vim_regcomp(char_u *ಅಭಿವ್ಯಕ್ತಿ_arg, ಪೂರ್ಣಾಂಕ re_flags)
{
    regprog_T   *prog = ಖಾಲಿ;
    char_u	*ಅಭಿವ್ಯಕ್ತಿ = ಅಭಿವ್ಯಕ್ತಿ_arg;
    ಪೂರ್ಣಾಂಕ		called_emsg_before;

    regexp_engine = p_re;

    // Check for prefix "\%#=", that sets the regexp engine
    ಆದರೆ (STRNCMP(ಅಭಿವ್ಯಕ್ತಿ, "\\%#=", 4) == 0)
    {
	ಪೂರ್ಣಾಂಕ newengine = ಅಭಿವ್ಯಕ್ತಿ[4] - '0';

	ಆದರೆ (newengine == AUTOMATIC_ENGINE
	    || newengine == BACKTRACKING_ENGINE
	    || newengine == NFA_ENGINE)
	{
	    regexp_engine = ಅಭಿವ್ಯಕ್ತಿ[4] - '0';
	    ಅಭಿವ್ಯಕ್ತಿ += 5;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
	    smsg("New regexp mode selected (%d): %s",
					   regexp_engine, regname[newengine]);
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    emsg(_(e_percent_hash_can_only_be_followed_by_zero_one_two_automatic_engine_will_be_ಬಳಸಿದ));
	    regexp_engine = AUTOMATIC_ENGINE;
	}
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
    bt_regengine.ಅಭಿವ್ಯಕ್ತಿ = ಅಭಿವ್ಯಕ್ತಿ;
    nfa_regengine.ಅಭಿವ್ಯಕ್ತಿ = ಅಭಿವ್ಯಕ್ತಿ;
#ಆದರೆ_ಅಂತ್ಯ
    // reg_iswordc() uses rex.reg_buf
    rex.reg_buf = curbuf;

    /*
     * First try the NFA engine, unless backtracking was requested.
     */
    called_emsg_before = called_emsg;
    ಆದರೆ (regexp_engine != BACKTRACKING_ENGINE)
	prog = nfa_regengine.regcomp(ಅಭಿವ್ಯಕ್ತಿ,
		re_flags + (regexp_engine == AUTOMATIC_ENGINE ? RE_AUTO : 0));
    ಇಲ್ಲದಿದ್ದರೆ
	prog = bt_regengine.regcomp(ಅಭಿವ್ಯಕ್ತಿ, re_flags);

    // Check for error compiling regexp with initial engine.
    ಆದರೆ (prog == ಖಾಲಿ)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BT_REGEXP_DEBUG_LOG
	ಆದರೆ (regexp_engine == BACKTRACKING_ENGINE)   // debugging log for BT engine
	{
	    FILE *f;
	    f = fopen(BT_REGEXP_DEBUG_LOG_NAME, "a");
	    ಆದರೆ (f)
	    {
		fprintf(f, "Syntax error in \"%s\"\n", ಅಭಿವ್ಯಕ್ತಿ);
		fclose(f);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg("(NFA) Could not open \"%s\" to write !!!",
			BT_REGEXP_DEBUG_LOG_NAME);
	}
#ಆದರೆ_ಅಂತ್ಯ
	/*
	 * If the NFA engine failed, try the backtracking engine.
	 * The NFA engine also fails for patterns that it can't handle well
	 * but are still valid patterns, thus a retry should work.
	 * But don't try if an error message was given.
	 */
	ಆದರೆ (regexp_engine == AUTOMATIC_ENGINE
					  && called_emsg == called_emsg_before)
	{
	    regexp_engine = BACKTRACKING_ENGINE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    report_re_switch(ಅಭಿವ್ಯಕ್ತಿ);
#ಆದರೆ_ಅಂತ್ಯ
	    prog = bt_regengine.regcomp(ಅಭಿವ್ಯಕ್ತಿ, re_flags);
	}
    }

    ಆದರೆ (prog != ಖಾಲಿ)
    {
	// Store the info needed to call regcomp() again when the engine turns
	// out to be very slow when executing it.
	prog->re_engine = regexp_engine;
	prog->re_flags  = re_flags;
    }

    ಹಿಂತಿರುಗಿ prog;
}

/*
 * Free a compiled regexp program, returned by vim_regcomp().
 */
    ಶೂನ್ಯ
vim_regfree(regprog_T *prog)
{
    ಆದರೆ (prog != ಖಾಲಿ)
	prog->engine->regfree(prog);
}

#ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
free_regexp_stuff(ಶೂನ್ಯ)
{
    ga_clear(&regstack);
    ga_clear(&backpos);
    vim_free(reg_tofree);
    vim_free(reg_prev_sub);
    vim_free(post_ಆರಂಭ);   // NFA postfix buffer, reused across compilations
    post_ಆರಂಭ = ಖಾಲಿ;
    post_ಆರಂಭ_len = 0;
    vim_free(nfa_ರಾಶಿ);    // NFA fragment stack, reused across compilations
    nfa_ರಾಶಿ = ಖಾಲಿ;
    nfa_ರಾಶಿ_len = 0;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಸ್ಥಿರ ಶೂನ್ಯ
report_re_switch(char_u *pat)
{
    ಆದರೆ (p_verbose > 0)
    {
	verbose_enter();
	ಸಂದೇಶ_puts(_("Switching to backtracking RE engine for pattern: "));
	ಸಂದೇಶ_puts((ಅಕ್ಷರ *)pat);
	verbose_leave();
    }
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_X11)
/*
 * Return whether "prog" is currently being executed.
 */
    ಪೂರ್ಣಾಂಕ
regprog_ಒಳ_use(regprog_T *prog)
{
    ಹಿಂತಿರುಗಿ prog->re_ಒಳ_use;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Match a regexp against a string.
 * "rmp->regprog" must be a compiled regexp as returned by vim_regcomp().
 * Note: "rmp->regprog" may be freed and changed.
 * Uses curbuf for line count and 'iskeyword'.
 * When "nl" is TRUE consider a "\n" in "line" to be a line break.
 *
 * Return TRUE if there is a match, FALSE if not.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
vim_regexec_string(
    regmatch_T	*rmp,
    char_u	*ಸಾಲು,  // string to match against
    colnr_T	ಲಂಬಸಾಲು,    // column to start looking for match
    ಪೂರ್ಣಾಂಕ		nl)
{
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ;
    regexec_T	rex_save;
    ಪೂರ್ಣಾಂಕ		rex_ಒಳ_use_save = rex_ಒಳ_use;

    // Cannot use the same prog recursively, it contains state.
    ಆದರೆ (rmp->regprog->re_ಒಳ_use)
    {
	emsg(_(e_cannot_use_ನಮೂನೆ_recursively));
	ಹಿಂತಿರುಗಿ FALSE;
    }
    rmp->regprog->re_ಒಳ_use = TRUE;

    ಆದರೆ (rex_ಒಳ_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_ಒಳ_use = TRUE;

    rex.reg_startp = ಖಾಲಿ;
    rex.reg_endp = ಖಾಲಿ;
    rex.reg_startpos = ಖಾಲಿ;
    rex.reg_endpos = ಖಾಲಿ;

    ಫಲಿತಾಂಶ = rmp->regprog->engine->regexec_nl(rmp, ಸಾಲು, ಲಂಬಸಾಲು, nl);
    rmp->regprog->re_ಒಳ_use = FALSE;

    // NFA engine aborted because it's very slow.
    ಆದರೆ (rmp->regprog->re_engine == AUTOMATIC_ENGINE
					       && ಫಲಿತಾಂಶ == NFA_TOO_EXPENSIVE)
    {
	ಪೂರ್ಣಾಂಕ    save_p_re = p_re;
	ಪೂರ್ಣಾಂಕ    re_flags = rmp->regprog->re_flags;
	char_u *pat = vim_strsave(((nfa_regprog_T *)rmp->regprog)->ನಮೂನೆ);

	p_re = BACKTRACKING_ENGINE;
	ಆದರೆ (pat != ಖಾಲಿ)
	{
	    regprog_T *prev_prog = rmp->regprog;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    report_re_switch(pat);
#ಆದರೆ_ಅಂತ್ಯ
	    rmp->regprog = vim_regcomp(pat, re_flags);
	    ಆದರೆ (rmp->regprog == ಖಾಲಿ)
	    {
		// Somehow compiling the pattern failed now, put back the
		// previous one to avoid "regprog" becoming NULL.
		rmp->regprog = prev_prog;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		vim_regfree(prev_prog);
		rmp->regprog->re_ಒಳ_use = TRUE;
		ಫಲಿತಾಂಶ = rmp->regprog->engine->regexec_nl(rmp, ಸಾಲು, ಲಂಬಸಾಲು, nl);
		rmp->regprog->re_ಒಳ_use = FALSE;
	    }
	    vim_free(pat);
	}

	p_re = save_p_re;
    }

    rex_ಒಳ_use = rex_ಒಳ_use_save;
    ಆದರೆ (rex_ಒಳ_use)
	rex = rex_save;

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ > 0;
}

#ಆದರೆ defined(FEAT_SPELL) || defined(FEAT_EVAL) || defined(FEAT_X11)
/*
 * Note: "*prog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    ಪೂರ್ಣಾಂಕ
vim_regexec_prog(
    regprog_T	**prog,
    ಪೂರ್ಣಾಂಕ		ignore_case,
    char_u	*ಸಾಲು,
    colnr_T	ಲಂಬಸಾಲು)
{
    ಪೂರ್ಣಾಂಕ		r;
    regmatch_T	regmatch;

    regmatch.regprog = *prog;
    regmatch.rm_ic = ignore_case;
    r = vim_regexec_string(&regmatch, ಸಾಲು, ಲಂಬಸಾಲು, FALSE);
    *prog = regmatch.regprog;
    ಹಿಂತಿರುಗಿ r;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Note: "rmp->regprog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    ಪೂರ್ಣಾಂಕ
vim_regexec(regmatch_T *rmp, char_u *ಸಾಲು, colnr_T ಲಂಬಸಾಲು)
{
    ಹಿಂತಿರುಗಿ vim_regexec_string(rmp, ಸಾಲು, ಲಂಬಸಾಲು, FALSE);
}

/*
 * Like vim_regexec(), but consider a "\n" in "line" to be a line break.
 * Note: "rmp->regprog" may be freed and changed.
 * Return TRUE if there is a match, FALSE if not.
 */
    ಪೂರ್ಣಾಂಕ
vim_regexec_nl(regmatch_T *rmp, char_u *ಸಾಲು, colnr_T ಲಂಬಸಾಲು)
{
    ಹಿಂತಿರುಗಿ vim_regexec_string(rmp, ಸಾಲು, ಲಂಬಸಾಲು, TRUE);
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
    ದೀರ್ಘ
vim_regexec_multi(
    regmmatch_T *rmp,
    win_T       *win,		// window in which to search or NULL
    buf_T       *buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	ಲಂಬಸಾಲು,		// column to start looking for match
    ಪೂರ್ಣಾಂಕ		*timed_ಹೊರಹೋಗು)	// flag is set when timeout limit reached
{
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ;
    regexec_T	rex_save;
    ಪೂರ್ಣಾಂಕ		rex_ಒಳ_use_save = rex_ಒಳ_use;

    // Cannot use the same prog recursively, it contains state.
    ಆದರೆ (rmp->regprog->re_ಒಳ_use)
    {
	emsg(_(e_cannot_use_ನಮೂನೆ_recursively));
	ಹಿಂತಿರುಗಿ FALSE;
    }
    rmp->regprog->re_ಒಳ_use = TRUE;

    ಆದರೆ (rex_ಒಳ_use)
	// Being called recursively, save the state.
	rex_save = rex;
    rex_ಒಳ_use = TRUE;

    ಫಲಿತಾಂಶ = rmp->regprog->engine->regexec_multi(
				      rmp, win, buf, lnum, ಲಂಬಸಾಲು, timed_ಹೊರಹೋಗು);
    rmp->regprog->re_ಒಳ_use = FALSE;

    // NFA engine aborted because it's very slow.
    ಆದರೆ (rmp->regprog->re_engine == AUTOMATIC_ENGINE
					       && ಫಲಿತಾಂಶ == NFA_TOO_EXPENSIVE)
    {
	ಪೂರ್ಣಾಂಕ    save_p_re = p_re;
	ಪೂರ್ಣಾಂಕ    re_flags = rmp->regprog->re_flags;
	char_u *pat = vim_strsave(((nfa_regprog_T *)rmp->regprog)->ನಮೂನೆ);

	p_re = BACKTRACKING_ENGINE;
	ಆದರೆ (pat != ಖಾಲಿ)
	{
	    regprog_T *prev_prog = rmp->regprog;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    report_re_switch(pat);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	    // checking for \z misuse was already done when compiling for NFA,
	    // allow all here
	    reg_do_extmatch = REX_ALL;
#ಆದರೆ_ಅಂತ್ಯ
	    rmp->regprog = vim_regcomp(pat, re_flags);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	    reg_do_extmatch = 0;
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (rmp->regprog == ಖಾಲಿ)
	    {
		// Somehow compiling the pattern failed now, put back the
		// previous one to avoid "regprog" becoming NULL.
		rmp->regprog = prev_prog;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		vim_regfree(prev_prog);

		rmp->regprog->re_ಒಳ_use = TRUE;
		ಫಲಿತಾಂಶ = rmp->regprog->engine->regexec_multi(
				      rmp, win, buf, lnum, ಲಂಬಸಾಲು, timed_ಹೊರಹೋಗು);
		rmp->regprog->re_ಒಳ_use = FALSE;
	    }
	    vim_free(pat);
	}
	p_re = save_p_re;
    }

    rex_ಒಳ_use = rex_ಒಳ_use_save;
    ಆದರೆ (rex_ಒಳ_use)
	rex = rex_save;

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ <= 0 ? 0 : ಫಲಿತಾಂಶ;
}
