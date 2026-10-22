/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * NFA regular expression implementation.
 *
 * This file is included in "regexp.c".
 */

/*
 * Logging of NFA engine.
 *
 * The NFA engine can write four log files:
 * - Error log: Contains NFA engine's fatal errors.
 * - Dump log: Contains compiled NFA state machine's information.
 * - Run log: Contains information of matching procedure.
 * - Debug log: Contains detailed information of matching procedure. Can be
 *   disabled by undefining NFA_REGEXP_DEBUG_LOG.
 * The first one can also be used without debug mode.
 * The last three are enabled when compiled as debug mode and individually
 * disabled by commenting them out.
 * The log files can get quite big!
 * To disable all of this when compiling Vim for debugging, undefine DEBUG in
 * regexp.c
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
# define NFA_REGEXP_ERROR_LOG	"nfa_regexp_error.log"
# define ENABLE_LOG
# define NFA_REGEXP_DUMP_LOG	"nfa_regexp_dump.log"
# define NFA_REGEXP_ಸತತ_LOG	"nfa_regexp_run.log"
# define NFA_REGEXP_DEBUG_LOG	"nfa_regexp_debug.log"
#ಆದರೆ_ಅಂತ್ಯ

// Added to NFA_ANY - NFA_NUPPER_IC to include a NL.
#ವ್ಯಾಖ್ಯಾನಿಸು NFA_ಕೂಡಿಸು_NL		31

ಎಣಿಕೆ
{
    NFA_ವಿಭಜಿಸು = -1024,
    NFA_ಹೊಂದಿಕೆ,
    NFA_ಖಾಲಿಗಳು,			    // matches 0-length

    NFA_ಆರಂಭ_COLL,		    // [abc] start
    NFA_ಕೊನೆ_COLL,		    // [abc] end
    NFA_ಆರಂಭ_NEG_COLL,		    // [^abc] start
    NFA_ಕೊನೆ_NEG_COLL,		    // [^abc] end (postfix only)
    NFA_RANGE,			    // range of the two previous items
				    // (postfix only)
    NFA_RANGE_MIN,		    // low end of a range
    NFA_RANGE_MAX,		    // high end of a range

    NFA_CONCAT,			    // concatenate two previous items (postfix
				    // only)
    NFA_OR,			    // \| (postfix only)
    NFA_ನಕ್ಷತ್ರ,			    // greedy * (postfix only)
    NFA_ನಕ್ಷತ್ರ_NONGREEDY,		    // non-greedy * (postfix only)
    NFA_QUEST,			    // greedy \? (postfix only)
    NFA_QUEST_NONGREEDY,	    // non-greedy \? (postfix only)

    NFA_BOL,			    // ^    Begin line
    NFA_EOL,			    // $    End line
    NFA_BOW,			    // \<   Begin word
    NFA_EOW,			    // \>   End word
    NFA_BOF,			    // \%^  Begin file
    NFA_EOF,			    // \%$  End file
    NFA_NEWL,
    NFA_ZSTART,			    // Used for \zs
    NFA_ZEND,			    // Used for \ze
    NFA_NOPEN,			    // Start of subexpression marked with \%(
    NFA_NCLOSE,			    // End of subexpr. marked with \%( ... \)
    NFA_ಆರಂಭ_INVISIBLE,
    NFA_ಆರಂಭ_INVISIBLE_ಮೊದಲ,
    NFA_ಆರಂಭ_INVISIBLE_NEG,
    NFA_ಆರಂಭ_INVISIBLE_NEG_ಮೊದಲ,
    NFA_ಆರಂಭ_INVISIBLE_BEFORE,
    NFA_ಆರಂಭ_INVISIBLE_BEFORE_ಮೊದಲ,
    NFA_ಆರಂಭ_INVISIBLE_BEFORE_NEG,
    NFA_ಆರಂಭ_INVISIBLE_BEFORE_NEG_ಮೊದಲ,
    NFA_ಆರಂಭ_ನಮೂನೆ,
    NFA_ಕೊನೆ_INVISIBLE,
    NFA_ಕೊನೆ_INVISIBLE_NEG,
    NFA_ಕೊನೆ_ನಮೂನೆ,
    NFA_COMPOSING,		    // Next nodes in NFA are part of the
				    // composing multibyte char
    NFA_ಕೊನೆ_COMPOSING,		    // End of a composing char in the NFA
    NFA_ANY_COMPOSING,		    // \%C: Any composing characters.
    NFA_OPT_CHARS,		    // \%[abc]

    // The following are used only in the postfix form, not in the NFA
    NFA_PREV_ATOM_NO_ಅಗಲ,	    // Used for \@=
    NFA_PREV_ATOM_NO_ಅಗಲ_NEG,	    // Used for \@!
    NFA_PREV_ATOM_JUST_BEFORE,	    // Used for \@<=
    NFA_PREV_ATOM_JUST_BEFORE_NEG,  // Used for \@<!
    NFA_PREV_ATOM_LIKE_ನಮೂನೆ,	    // Used for \@>

    NFA_BACKREF1,		    // \1
    NFA_BACKREF2,		    // \2
    NFA_BACKREF3,		    // \3
    NFA_BACKREF4,		    // \4
    NFA_BACKREF5,		    // \5
    NFA_BACKREF6,		    // \6
    NFA_BACKREF7,		    // \7
    NFA_BACKREF8,		    // \8
    NFA_BACKREF9,		    // \9
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    NFA_ZREF1,			    // \z1
    NFA_ZREF2,			    // \z2
    NFA_ZREF3,			    // \z3
    NFA_ZREF4,			    // \z4
    NFA_ZREF5,			    // \z5
    NFA_ZREF6,			    // \z6
    NFA_ZREF7,			    // \z7
    NFA_ZREF8,			    // \z8
    NFA_ZREF9,			    // \z9
#ಆದರೆ_ಅಂತ್ಯ
    NFA_SKIP,			    // Skip characters

    NFA_MOPEN,
    NFA_MOPEN1,
    NFA_MOPEN2,
    NFA_MOPEN3,
    NFA_MOPEN4,
    NFA_MOPEN5,
    NFA_MOPEN6,
    NFA_MOPEN7,
    NFA_MOPEN8,
    NFA_MOPEN9,

    NFA_MCLOSE,
    NFA_MCLOSE1,
    NFA_MCLOSE2,
    NFA_MCLOSE3,
    NFA_MCLOSE4,
    NFA_MCLOSE5,
    NFA_MCLOSE6,
    NFA_MCLOSE7,
    NFA_MCLOSE8,
    NFA_MCLOSE9,

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    NFA_ZOPEN,
    NFA_ZOPEN1,
    NFA_ZOPEN2,
    NFA_ZOPEN3,
    NFA_ZOPEN4,
    NFA_ZOPEN5,
    NFA_ZOPEN6,
    NFA_ZOPEN7,
    NFA_ZOPEN8,
    NFA_ZOPEN9,

    NFA_ZCLOSE,
    NFA_ZCLOSE1,
    NFA_ZCLOSE2,
    NFA_ZCLOSE3,
    NFA_ZCLOSE4,
    NFA_ZCLOSE5,
    NFA_ZCLOSE6,
    NFA_ZCLOSE7,
    NFA_ZCLOSE8,
    NFA_ZCLOSE9,
#ಆದರೆ_ಅಂತ್ಯ

    // NFA_FIRST_NL
    NFA_ANY,		//	Match any one character.
    NFA_IDENT,		//	Match identifier char
    NFA_SIDENT,		//	Match identifier char but no digit
    NFA_KWORD,		//	Match keyword char
    NFA_SKWORD,		//	Match word char but no digit
    NFA_FNAME,		//	Match file name char
    NFA_SFNAME,		//	Match file name char but no digit
    NFA_PRINT,		//	Match printable char
    NFA_SPRINT,		//	Match printable char but no digit
    NFA_WHITE,		//	Match whitespace char
    NFA_NWHITE,		//	Match non-whitespace char
    NFA_DIGIT,		//	Match digit char
    NFA_NDIGIT,		//	Match non-digit char
    NFA_HEX,		//	Match hex char
    NFA_NHEX,		//	Match non-hex char
    NFA_OCTAL,		//	Match octal char
    NFA_NOCTAL,		//	Match non-octal char
    NFA_ಶಬ್ದ,		//	Match word char
    NFA_NWORD,		//	Match non-word char
    NFA_ತಲೆ,		//	Match head char
    NFA_NHEAD,		//	Match non-head char
    NFA_ALPHA,		//	Match alpha char
    NFA_NALPHA,		//	Match non-alpha char
    NFA_ಅಧೋ,		//	Match lowercase char
    NFA_NLOWER,		//	Match non-lowercase char
    NFA_ಊರ್ಧ್ವ,		//	Match uppercase char
    NFA_NUPPER,		//	Match non-uppercase char
    NFA_ಅಧೋ_IC,	//	Match [a-z]
    NFA_NLOWER_IC,	//	Match [^a-z]
    NFA_ಊರ್ಧ್ವ_IC,	//	Match [A-Z]
    NFA_NUPPER_IC,	//	Match [^A-Z]

    NFA_ಮೊದಲ_NL = NFA_ANY + NFA_ಕೂಡಿಸು_NL,
    NFA_ಕೊನೆಯ_NL = NFA_NUPPER_IC + NFA_ಕೂಡಿಸು_NL,

    NFA_CURSOR,		//	Match cursor pos
    NFA_LNUM,		//	Match line number
    NFA_LNUM_GT,	//	Match > line number
    NFA_LNUM_LT,	//	Match < line number
    NFA_ಲಂಬಸಾಲು,		//	Match cursor column
    NFA_ಲಂಬಸಾಲು_GT,		//	Match > cursor column
    NFA_ಲಂಬಸಾಲು_LT,		//	Match < cursor column
    NFA_VCOL,		//	Match cursor virtual column
    NFA_VCOL_GT,	//	Match > cursor virtual column
    NFA_VCOL_LT,	//	Match < cursor virtual column
    NFA_MARK,		//	Match mark
    NFA_MARK_GT,	//	Match > mark
    NFA_MARK_LT,	//	Match < mark
    NFA_VISUAL,		//	Match Visual area

    // Character classes [:alnum:] etc
    NFA_CLASS_ALNUM,
    NFA_CLASS_ALPHA,
    NFA_CLASS_BLANK,
    NFA_CLASS_CNTRL,
    NFA_CLASS_DIGIT,
    NFA_CLASS_ನಕ್ಷೆ,
    NFA_CLASS_ಅಧೋ,
    NFA_CLASS_PRINT,
    NFA_CLASS_PUNCT,
    NFA_CLASS_SPACE,
    NFA_CLASS_ಊರ್ಧ್ವ,
    NFA_CLASS_XDIGIT,
    NFA_CLASS_TAB,
    NFA_CLASS_RETURN,
    NFA_CLASS_BACKSPACE,
    NFA_CLASS_ESCAPE,
    NFA_CLASS_IDENT,
    NFA_CLASS_KEYWORD,
    NFA_CLASS_FNAME
};

// Keep in sync with classchars.
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ nfa_classcodes[] = {
    NFA_ANY, NFA_IDENT, NFA_SIDENT, NFA_KWORD,NFA_SKWORD,
    NFA_FNAME, NFA_SFNAME, NFA_PRINT, NFA_SPRINT,
    NFA_WHITE, NFA_NWHITE, NFA_DIGIT, NFA_NDIGIT,
    NFA_HEX, NFA_NHEX, NFA_OCTAL, NFA_NOCTAL,
    NFA_ಶಬ್ದ, NFA_NWORD, NFA_ತಲೆ, NFA_NHEAD,
    NFA_ALPHA, NFA_NALPHA, NFA_ಅಧೋ, NFA_NLOWER,
    NFA_ಊರ್ಧ್ವ, NFA_NUPPER
};

// Variables only used in nfa_regcomp() and descendants.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ nfa_re_flags; // re_flags passed to nfa_regcomp()
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ *post_ಆರಂಭ;  // holds the postfix form of r.e.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ post_ಆರಂಭ_len;  // size of allocated post_start (in ints)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ *post_ಕೊನೆ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ *post_ptr;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ nfa_reg_parse_ಆಳ;	// nesting depth in nfa_reg()

// The postfix list (post_start) and fragment stack (nfa_stack) are reused
// across compilations; a buffer that grew past these sizes for a big pattern
// is freed afterwards instead of being kept, to avoid holding much memory.
#ವ್ಯಾಖ್ಯಾನಿಸು NFA_POSTFIX_KEEP	10000	// number of ints (~40 Kbyte)
#ವ್ಯಾಖ್ಯಾನಿಸು NFA_ರಾಶಿ_KEEP		4000	// number of Frag_T (~64 Kbyte)

// Set when the pattern should use the NFA engine.
// E.g. [[:upper:]] only allows 8bit characters for BT engine,
// while NFA engine handles multibyte characters correctly.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಬಯಸುತ್ತದೆ_nfa;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ nstate;	// Number of states in the NFA.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ istate;	// Index in the state vector, used in alloc_state()

// If not NULL match must end at this position
ಸ್ಥಿರ save_se_T *nfa_endp = ಖಾಲಿ;

// 0 for first call to nfa_regmatch(), 1 for recursive call.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ nfa_ll_ಸೂಚ್ಯಂಕ = 0;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ realloc_post_list(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ nfa_reg(ಪೂರ್ಣಾಂಕ paren);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
ಸ್ಥಿರ ಶೂನ್ಯ nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಹೊಂದಿಕೆ_follows(nfa_state_T *startstate, ಪೂರ್ಣಾಂಕ ಆಳ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ failure_chance(nfa_state_T *state, ಪೂರ್ಣಾಂಕ ಆಳ);

// helper functions used when doing re2post() ... regatom() parsing
#ವ್ಯಾಖ್ಯಾನಿಸು EMIT(c)	ಮಾಡು {				\
		    ಆದರೆ (post_ptr >= post_ಕೊನೆ && realloc_post_list() == FAIL) \
			ಹಿಂತಿರುಗಿ FAIL;		\
		    *post_ptr++ = c;		\
		} ಆಗ (0)

/*
 * Initialize internal variables before NFA compilation.
 * Return OK on success, FAIL otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
nfa_regcomp_ಆರಂಭ(
    char_u	*ಅಭಿವ್ಯಕ್ತಿ,
    ಪೂರ್ಣಾಂಕ		re_flags)	    // see vim_regcomp()
{
    size_t	postfix_ಗಾತ್ರ;
    ಪೂರ್ಣಾಂಕ		nstate_max;

    nstate = 0;
    istate = 0;
    // A reasonable estimation for maximum size
    nstate_max = (ಪೂರ್ಣಾಂಕ)(STRLEN(ಅಭಿವ್ಯಕ್ತಿ) + 1) * 25;

    // Some items blow up in size, such as [A-z].  Add more space for that.
    // When it is still not enough realloc_post_list() will be used.
    nstate_max += 1000;

    // Size for postfix representation of expr.
    postfix_ಗಾತ್ರ = ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ) * nstate_max;

    // Reuse the postfix buffer across compilations, only growing it when the
    // estimate exceeds the current size; it is freed in free_regexp_stuff().
    ಆದರೆ (post_ಆರಂಭ == ಖಾಲಿ || post_ಆರಂಭ_len < nstate_max)
    {
	vim_free(post_ಆರಂಭ);
	post_ಆರಂಭ = alloc(postfix_ಗಾತ್ರ);
	ಆದರೆ (post_ಆರಂಭ == ಖಾಲಿ)
	{
	    post_ಆರಂಭ_len = 0;
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	post_ಆರಂಭ_len = nstate_max;
    }
    post_ptr = post_ಆರಂಭ;
    post_ಕೊನೆ = post_ಆರಂಭ + post_ಆರಂಭ_len;
    ಬಯಸುತ್ತದೆ_nfa = FALSE;
    rex.nfa_has_zend = FALSE;
    rex.nfa_has_backref = FALSE;
    nfa_reg_parse_ಆಳ = 0;

    // shared with BT engine
    regcomp_ಆರಂಭ(ಅಭಿವ್ಯಕ್ತಿ, re_flags);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Figure out if the NFA state list starts with an anchor, must match at start
 * of the line.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
nfa_get_reganch(nfa_state_T *ಆರಂಭ, ಪೂರ್ಣಾಂಕ ಆಳ)
{
    nfa_state_T *p = ಆರಂಭ;

    ಆದರೆ (ಆಳ > 4)
	ಹಿಂತಿರುಗಿ 0;

    ಆಗ (p != ಖಾಲಿ)
    {
	ಆಯ್ಕೆ (p->c)
	{
	    ಸ್ಥಿತಿ NFA_BOL:
	    ಸ್ಥಿತಿ NFA_BOF:
		ಹಿಂತಿರುಗಿ 1; // yes!

	    ಸ್ಥಿತಿ NFA_ZSTART:
	    ಸ್ಥಿತಿ NFA_ZEND:
	    ಸ್ಥಿತಿ NFA_CURSOR:
	    ಸ್ಥಿತಿ NFA_VISUAL:

	    ಸ್ಥಿತಿ NFA_MOPEN:
	    ಸ್ಥಿತಿ NFA_MOPEN1:
	    ಸ್ಥಿತಿ NFA_MOPEN2:
	    ಸ್ಥಿತಿ NFA_MOPEN3:
	    ಸ್ಥಿತಿ NFA_MOPEN4:
	    ಸ್ಥಿತಿ NFA_MOPEN5:
	    ಸ್ಥಿತಿ NFA_MOPEN6:
	    ಸ್ಥಿತಿ NFA_MOPEN7:
	    ಸ್ಥಿತಿ NFA_MOPEN8:
	    ಸ್ಥಿತಿ NFA_MOPEN9:
	    ಸ್ಥಿತಿ NFA_NOPEN:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	    ಸ್ಥಿತಿ NFA_ZOPEN:
	    ಸ್ಥಿತಿ NFA_ZOPEN1:
	    ಸ್ಥಿತಿ NFA_ZOPEN2:
	    ಸ್ಥಿತಿ NFA_ZOPEN3:
	    ಸ್ಥಿತಿ NFA_ZOPEN4:
	    ಸ್ಥಿತಿ NFA_ZOPEN5:
	    ಸ್ಥಿತಿ NFA_ZOPEN6:
	    ಸ್ಥಿತಿ NFA_ZOPEN7:
	    ಸ್ಥಿತಿ NFA_ZOPEN8:
	    ಸ್ಥಿತಿ NFA_ZOPEN9:
#ಆದರೆ_ಅಂತ್ಯ
		p = p->ಹೊರಹೋಗು;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_ವಿಭಜಿಸು:
		ಹಿಂತಿರುಗಿ nfa_get_reganch(p->ಹೊರಹೋಗು, ಆಳ + 1)
				       && nfa_get_reganch(p->out1, ಆಳ + 1);

	    ಪೂರ್ವನಿಯೋಜಿತ:
		ಹಿಂತಿರುಗಿ 0; // noooo
	}
    }
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Figure out if the NFA state list starts with a character which must match
 * at start of the match.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
nfa_get_regstart(nfa_state_T *ಆರಂಭ, ಪೂರ್ಣಾಂಕ ಆಳ)
{
    nfa_state_T *p = ಆರಂಭ;

    ಆದರೆ (ಆಳ > 4)
	ಹಿಂತಿರುಗಿ 0;

    ಆಗ (p != ಖಾಲಿ)
    {
	ಆಯ್ಕೆ (p->c)
	{
	    // all kinds of zero-width matches
	    ಸ್ಥಿತಿ NFA_BOL:
	    ಸ್ಥಿತಿ NFA_BOF:
	    ಸ್ಥಿತಿ NFA_BOW:
	    ಸ್ಥಿತಿ NFA_EOW:
	    ಸ್ಥಿತಿ NFA_ZSTART:
	    ಸ್ಥಿತಿ NFA_ZEND:
	    ಸ್ಥಿತಿ NFA_CURSOR:
	    ಸ್ಥಿತಿ NFA_VISUAL:
	    ಸ್ಥಿತಿ NFA_LNUM:
	    ಸ್ಥಿತಿ NFA_LNUM_GT:
	    ಸ್ಥಿತಿ NFA_LNUM_LT:
	    ಸ್ಥಿತಿ NFA_ಲಂಬಸಾಲು:
	    ಸ್ಥಿತಿ NFA_ಲಂಬಸಾಲು_GT:
	    ಸ್ಥಿತಿ NFA_ಲಂಬಸಾಲು_LT:
	    ಸ್ಥಿತಿ NFA_VCOL:
	    ಸ್ಥಿತಿ NFA_VCOL_GT:
	    ಸ್ಥಿತಿ NFA_VCOL_LT:
	    ಸ್ಥಿತಿ NFA_MARK:
	    ಸ್ಥಿತಿ NFA_MARK_GT:
	    ಸ್ಥಿತಿ NFA_MARK_LT:

	    ಸ್ಥಿತಿ NFA_MOPEN:
	    ಸ್ಥಿತಿ NFA_MOPEN1:
	    ಸ್ಥಿತಿ NFA_MOPEN2:
	    ಸ್ಥಿತಿ NFA_MOPEN3:
	    ಸ್ಥಿತಿ NFA_MOPEN4:
	    ಸ್ಥಿತಿ NFA_MOPEN5:
	    ಸ್ಥಿತಿ NFA_MOPEN6:
	    ಸ್ಥಿತಿ NFA_MOPEN7:
	    ಸ್ಥಿತಿ NFA_MOPEN8:
	    ಸ್ಥಿತಿ NFA_MOPEN9:
	    ಸ್ಥಿತಿ NFA_NOPEN:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	    ಸ್ಥಿತಿ NFA_ZOPEN:
	    ಸ್ಥಿತಿ NFA_ZOPEN1:
	    ಸ್ಥಿತಿ NFA_ZOPEN2:
	    ಸ್ಥಿತಿ NFA_ZOPEN3:
	    ಸ್ಥಿತಿ NFA_ZOPEN4:
	    ಸ್ಥಿತಿ NFA_ZOPEN5:
	    ಸ್ಥಿತಿ NFA_ZOPEN6:
	    ಸ್ಥಿತಿ NFA_ZOPEN7:
	    ಸ್ಥಿತಿ NFA_ZOPEN8:
	    ಸ್ಥಿತಿ NFA_ZOPEN9:
#ಆದರೆ_ಅಂತ್ಯ
		p = p->ಹೊರಹೋಗು;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_ವಿಭಜಿಸು:
	    {
		ಪೂರ್ಣಾಂಕ c1 = nfa_get_regstart(p->ಹೊರಹೋಗು, ಆಳ + 1);
		ಪೂರ್ಣಾಂಕ c2 = nfa_get_regstart(p->out1, ಆಳ + 1);

		ಆದರೆ (c1 == c2)
		    ಹಿಂತಿರುಗಿ c1; // yes!
		ಹಿಂತಿರುಗಿ 0;
	    }

	    ಪೂರ್ವನಿಯೋಜಿತ:
		ಆದರೆ (p->c > 0)
		    ಹಿಂತಿರುಗಿ p->c; // yes!
		ಹಿಂತಿರುಗಿ 0;
	}
    }
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Figure out if the NFA state list contains just literal text and nothing
 * else.  If so return a string in allocated memory with what must match after
 * regstart.  Otherwise return NULL.
 */
    ಸ್ಥಿರ char_u *
nfa_get_ಹೊಂದಿಕೆ_ಪಠ್ಯ(nfa_state_T *ಆರಂಭ)
{
    nfa_state_T *p = ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ		len = 0;
    char_u	*ret;
    char_u	*s;

    ಆದರೆ (p->c != NFA_MOPEN)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ; // just in case
    p = p->ಹೊರಹೋಗು;
    ಆಗ (p->c > 0)
    {
	len += MB_CHAR2LEN(p->c);
	p = p->ಹೊರಹೋಗು;
    }
    ಆದರೆ (p->c != NFA_MCLOSE || p->ಹೊರಹೋಗು->c != NFA_ಹೊಂದಿಕೆ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ret = alloc(len);
    ಆದರೆ (ret == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    p = ಆರಂಭ->ಹೊರಹೋಗು->ಹೊರಹೋಗು; // skip first char, it goes into regstart
    s = ret;
    ಆಗ (p->c > 0)
    {
	ಆದರೆ (has_mbyte)
	    s += (*mb_char2bytes)(p->c, s);
	ಇಲ್ಲದಿದ್ದರೆ
	    *s++ = p->c;
	p = p->ಹೊರಹೋಗು;
    }
    *s = NUL;
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Allocate more space for post_start.  Called when
 * running above the estimated number of states.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
realloc_post_list(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ   nstate_max = (ಪೂರ್ಣಾಂಕ)(post_ಕೊನೆ - post_ಆರಂಭ);
    ಪೂರ್ಣಾಂಕ   new_max;
    ಪೂರ್ಣಾಂಕ   *new_ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ	  *old_ಆರಂಭ;

    // For weird patterns the number of states can be very high. Increasing by
    // 50% seems a reasonable compromise between memory use and speed.
    new_max = nstate_max * 3 / 2;
    new_ಆರಂಭ = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, new_max);
    ಆದರೆ (new_ಆರಂಭ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    mch_memmove(new_ಆರಂಭ, post_ಆರಂಭ, nstate_max * ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ));
    old_ಆರಂಭ = post_ಆರಂಭ;
    post_ಆರಂಭ = new_ಆರಂಭ;
    post_ಆರಂಭ_len = new_max;
    post_ptr = new_ಆರಂಭ + (post_ptr - old_ಆರಂಭ);
    post_ಕೊನೆ = post_ಆರಂಭ + new_max;
    vim_free(old_ಆರಂಭ);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Search between "start" and "end" and try to recognize a
 * character class in expanded form. For example [0-9].
 * On success, return the id the character class to be emitted.
 * On failure, return 0 (=FAIL)
 * Start points to the first char of the range, while end should point
 * to the closing brace.
 * Keep in mind that 'ignorecase' applies at execution time, thus [a-z] may
 * need to be interpreted as [a-zA-Z].
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
nfa_recognize_char_class(char_u *ಆರಂಭ, char_u *ಕೊನೆ, ಪೂರ್ಣಾಂಕ extra_newl)
{
    ಪೂರ್ಣಾಂಕ		newl;
    ಪೂರ್ಣಾಂಕ		config = get_char_class_ಬಿಟ್ಟುಗಳು(ಆರಂಭ, ಕೊನೆ, &newl);

    ಆದರೆ (config < 0)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (extra_newl == TRUE)
	newl = TRUE;

    ಆದರೆ (newl == TRUE)
	extra_newl = NFA_ಕೂಡಿಸು_NL;

    ಆಯ್ಕೆ (config)
    {
	ಸ್ಥಿತಿ CLASS_o9:
	    ಹಿಂತಿರುಗಿ extra_newl + NFA_DIGIT;
	ಸ್ಥಿತಿ CLASS_not |  CLASS_o9:
	    ಹಿಂತಿರುಗಿ extra_newl + NFA_NDIGIT;
	ಸ್ಥಿತಿ CLASS_af | CLASS_AF | CLASS_o9:
	    ಹಿಂತಿರುಗಿ extra_newl + NFA_HEX;
	ಸ್ಥಿತಿ CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    ಹಿಂತಿರುಗಿ extra_newl + NFA_NHEX;
	ಸ್ಥಿತಿ CLASS_o7:
	    ಹಿಂತಿರುಗಿ extra_newl + NFA_OCTAL;
	ಸ್ಥಿತಿ CLASS_not | CLASS_o7:
	    ಹಿಂತಿರುಗಿ extra_newl + NFA_NOCTAL;
	ಸ್ಥಿತಿ CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    ಹಿಂತಿರುಗಿ extra_newl + NFA_ಶಬ್ದ;
	ಸ್ಥಿತಿ CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    ಹಿಂತಿರುಗಿ extra_newl + NFA_NWORD;
	ಸ್ಥಿತಿ CLASS_az | CLASS_AZ | CLASS_underscore:
	    ಹಿಂತಿರುಗಿ extra_newl + NFA_ತಲೆ;
	ಸ್ಥಿತಿ CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    ಹಿಂತಿರುಗಿ extra_newl + NFA_NHEAD;
	ಸ್ಥಿತಿ CLASS_az | CLASS_AZ:
	    ಹಿಂತಿರುಗಿ extra_newl + NFA_ALPHA;
	ಸ್ಥಿತಿ CLASS_not | CLASS_az | CLASS_AZ:
	    ಹಿಂತಿರುಗಿ extra_newl + NFA_NALPHA;
	ಸ್ಥಿತಿ CLASS_az:
	   ಹಿಂತಿರುಗಿ extra_newl + NFA_ಅಧೋ_IC;
	ಸ್ಥಿತಿ CLASS_not | CLASS_az:
	    ಹಿಂತಿರುಗಿ extra_newl + NFA_NLOWER_IC;
	ಸ್ಥಿತಿ CLASS_AZ:
	    ಹಿಂತಿರುಗಿ extra_newl + NFA_ಊರ್ಧ್ವ_IC;
	ಸ್ಥಿತಿ CLASS_not | CLASS_AZ:
	    ಹಿಂತಿರುಗಿ extra_newl + NFA_NUPPER_IC;
    }
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * Emits bytes in postfix notation: 'a,b,NFA_OR,c,NFA_OR' is
 * equivalent to 'a OR b OR c'
 *
 * NOTE! When changing this function, also update reg_equi_class()
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
nfa_emit_equi_class(ಪೂರ್ಣಾಂಕ c)
{
#ವ್ಯಾಖ್ಯಾನಿಸು EMIT2(c)    EMIT(c); EMIT(NFA_CONCAT);

    ಆದರೆ (enc_utf8 || STRCMP(p_enc, "latin1") == 0
					 || STRCMP(p_enc, "iso-8859-15") == 0)
    {
#ವ್ಯಾಖ್ಯಾನಿಸು A_grave 0xc0
#ವ್ಯಾಖ್ಯಾನಿಸು A_acute 0xc1
#ವ್ಯಾಖ್ಯಾನಿಸು A_circumflex 0xc2
#ವ್ಯಾಖ್ಯಾನಿಸು A_virguilla 0xc3
#ವ್ಯಾಖ್ಯಾನಿಸು A_diaeresis 0xc4
#ವ್ಯಾಖ್ಯಾನಿಸು A_ring 0xc5
#ವ್ಯಾಖ್ಯಾನಿಸು C_cedilla 0xc7
#ವ್ಯಾಖ್ಯಾನಿಸು E_grave 0xc8
#ವ್ಯಾಖ್ಯಾನಿಸು E_acute 0xc9
#ವ್ಯಾಖ್ಯಾನಿಸು E_circumflex 0xca
#ವ್ಯಾಖ್ಯಾನಿಸು E_diaeresis 0xcb
#ವ್ಯಾಖ್ಯಾನಿಸು I_grave 0xcc
#ವ್ಯಾಖ್ಯಾನಿಸು I_acute 0xcd
#ವ್ಯಾಖ್ಯಾನಿಸು I_circumflex 0xce
#ವ್ಯಾಖ್ಯಾನಿಸು I_diaeresis 0xcf
#ವ್ಯಾಖ್ಯಾನಿಸು N_virguilla 0xd1
#ವ್ಯಾಖ್ಯಾನಿಸು O_grave 0xd2
#ವ್ಯಾಖ್ಯಾನಿಸು O_acute 0xd3
#ವ್ಯಾಖ್ಯಾನಿಸು O_circumflex 0xd4
#ವ್ಯಾಖ್ಯಾನಿಸು O_virguilla 0xd5
#ವ್ಯಾಖ್ಯಾನಿಸು O_diaeresis 0xd6
#ವ್ಯಾಖ್ಯಾನಿಸು O_slash 0xd8
#ವ್ಯಾಖ್ಯಾನಿಸು U_grave 0xd9
#ವ್ಯಾಖ್ಯಾನಿಸು U_acute 0xda
#ವ್ಯಾಖ್ಯಾನಿಸು U_circumflex 0xdb
#ವ್ಯಾಖ್ಯಾನಿಸು U_diaeresis 0xdc
#ವ್ಯಾಖ್ಯಾನಿಸು Y_acute 0xdd
#ವ್ಯಾಖ್ಯಾನಿಸು a_grave 0xe0
#ವ್ಯಾಖ್ಯಾನಿಸು a_acute 0xe1
#ವ್ಯಾಖ್ಯಾನಿಸು a_circumflex 0xe2
#ವ್ಯಾಖ್ಯಾನಿಸು a_virguilla 0xe3
#ವ್ಯಾಖ್ಯಾನಿಸು a_diaeresis 0xe4
#ವ್ಯಾಖ್ಯಾನಿಸು a_ring 0xe5
#ವ್ಯಾಖ್ಯಾನಿಸು c_cedilla 0xe7
#ವ್ಯಾಖ್ಯಾನಿಸು e_grave 0xe8
#ವ್ಯಾಖ್ಯಾನಿಸು e_acute 0xe9
#ವ್ಯಾಖ್ಯಾನಿಸು e_circumflex 0xea
#ವ್ಯಾಖ್ಯಾನಿಸು e_diaeresis 0xeb
#ವ್ಯಾಖ್ಯಾನಿಸು i_grave 0xec
#ವ್ಯಾಖ್ಯಾನಿಸು i_acute 0xed
#ವ್ಯಾಖ್ಯಾನಿಸು i_circumflex 0xee
#ವ್ಯಾಖ್ಯಾನಿಸು i_diaeresis 0xef
#ವ್ಯಾಖ್ಯಾನಿಸು n_virguilla 0xf1
#ವ್ಯಾಖ್ಯಾನಿಸು o_grave 0xf2
#ವ್ಯಾಖ್ಯಾನಿಸು o_acute 0xf3
#ವ್ಯಾಖ್ಯಾನಿಸು o_circumflex 0xf4
#ವ್ಯಾಖ್ಯಾನಿಸು o_virguilla 0xf5
#ವ್ಯಾಖ್ಯಾನಿಸು o_diaeresis 0xf6
#ವ್ಯಾಖ್ಯಾನಿಸು o_slash 0xf8
#ವ್ಯಾಖ್ಯಾನಿಸು u_grave 0xf9
#ವ್ಯಾಖ್ಯಾನಿಸು u_acute 0xfa
#ವ್ಯಾಖ್ಯಾನಿಸು u_circumflex 0xfb
#ವ್ಯಾಖ್ಯಾನಿಸು u_diaeresis 0xfc
#ವ್ಯಾಖ್ಯಾನಿಸು y_acute 0xfd
#ವ್ಯಾಖ್ಯಾನಿಸು y_diaeresis 0xff
	ಆಯ್ಕೆ (c)
	{
	    ಸ್ಥಿತಿ 'A': ಸ್ಥಿತಿ A_grave: ಸ್ಥಿತಿ A_acute: ಸ್ಥಿತಿ A_circumflex:
	    ಸ್ಥಿತಿ A_virguilla: ಸ್ಥಿತಿ A_diaeresis: ಸ್ಥಿತಿ A_ring:
	    ಸ್ಥಿತಿ 0x100: ಸ್ಥಿತಿ 0x102: ಸ್ಥಿತಿ 0x104: ಸ್ಥಿತಿ 0x1cd:
	    ಸ್ಥಿತಿ 0x1de: ಸ್ಥಿತಿ 0x1e0: ಸ್ಥಿತಿ 0x1fa: ಸ್ಥಿತಿ 0x200:
	    ಸ್ಥಿತಿ 0x202: ಸ್ಥಿತಿ 0x226: ಸ್ಥಿತಿ 0x23a: ಸ್ಥಿತಿ 0x1e00:
	    ಸ್ಥಿತಿ 0x1ea0: ಸ್ಥಿತಿ 0x1ea2: ಸ್ಥಿತಿ 0x1ea4: ಸ್ಥಿತಿ 0x1ea6:
	    ಸ್ಥಿತಿ 0x1ea8: ಸ್ಥಿತಿ 0x1eaa: ಸ್ಥಿತಿ 0x1eac: ಸ್ಥಿತಿ 0x1eae:
	    ಸ್ಥಿತಿ 0x1eb0: ಸ್ಥಿತಿ 0x1eb2: ಸ್ಥಿತಿ 0x1eb4: ಸ್ಥಿತಿ 0x1eb6:
		    EMIT2('A') EMIT2(A_grave) EMIT2(A_acute)
		    EMIT2(A_circumflex) EMIT2(A_virguilla)
		    EMIT2(A_diaeresis) EMIT2(A_ring)
		    EMIT2(0x100) EMIT2(0x102) EMIT2(0x104)
		    EMIT2(0x1cd) EMIT2(0x1de) EMIT2(0x1e0)
		    EMIT2(0x1fa) EMIT2(0x200) EMIT2(0x202)
		    EMIT2(0x226) EMIT2(0x23a) EMIT2(0x1e00)
		    EMIT2(0x1ea0) EMIT2(0x1ea2) EMIT2(0x1ea4)
		    EMIT2(0x1ea6) EMIT2(0x1ea8) EMIT2(0x1eaa)
		    EMIT2(0x1eac) EMIT2(0x1eae) EMIT2(0x1eb0)
		    EMIT2(0x1eb2) EMIT2(0x1eb6) EMIT2(0x1eb4)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'B': ಸ್ಥಿತಿ 0x181: ಸ್ಥಿತಿ 0x243: ಸ್ಥಿತಿ 0x1e02:
	    ಸ್ಥಿತಿ 0x1e04: ಸ್ಥಿತಿ 0x1e06:
		    EMIT2('B')
		    EMIT2(0x181) EMIT2(0x243) EMIT2(0x1e02)
		    EMIT2(0x1e04) EMIT2(0x1e06)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'C': ಸ್ಥಿತಿ C_cedilla: ಸ್ಥಿತಿ 0x106: ಸ್ಥಿತಿ 0x108:
	    ಸ್ಥಿತಿ 0x10a: ಸ್ಥಿತಿ 0x10c: ಸ್ಥಿತಿ 0x187: ಸ್ಥಿತಿ 0x23b:
	    ಸ್ಥಿತಿ 0x1e08: ಸ್ಥಿತಿ 0xa792:
		    EMIT2('C') EMIT2(C_cedilla)
		    EMIT2(0x106) EMIT2(0x108) EMIT2(0x10a)
		    EMIT2(0x10c) EMIT2(0x187) EMIT2(0x23b)
		    EMIT2(0x1e08) EMIT2(0xa792)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'D': ಸ್ಥಿತಿ 0x10e: ಸ್ಥಿತಿ 0x110: ಸ್ಥಿತಿ 0x18a:
	    ಸ್ಥಿತಿ 0x1e0a: ಸ್ಥಿತಿ 0x1e0c: ಸ್ಥಿತಿ 0x1e0e: ಸ್ಥಿತಿ 0x1e10:
	    ಸ್ಥಿತಿ 0x1e12:
		    EMIT2('D') EMIT2(0x10e) EMIT2(0x110) EMIT2(0x18a)
		    EMIT2(0x1e0a) EMIT2(0x1e0c) EMIT2(0x1e0e)
		    EMIT2(0x1e10) EMIT2(0x1e12)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'E': ಸ್ಥಿತಿ E_grave: ಸ್ಥಿತಿ E_acute: ಸ್ಥಿತಿ E_circumflex:
	    ಸ್ಥಿತಿ E_diaeresis: ಸ್ಥಿತಿ 0x112: ಸ್ಥಿತಿ 0x114: ಸ್ಥಿತಿ 0x116:
	    ಸ್ಥಿತಿ 0x118: ಸ್ಥಿತಿ 0x11a: ಸ್ಥಿತಿ 0x204: ಸ್ಥಿತಿ 0x206:
	    ಸ್ಥಿತಿ 0x228: ಸ್ಥಿತಿ 0x246: ಸ್ಥಿತಿ 0x1e14: ಸ್ಥಿತಿ 0x1e16:
	    ಸ್ಥಿತಿ 0x1e18: ಸ್ಥಿತಿ 0x1e1a: ಸ್ಥಿತಿ 0x1e1c: ಸ್ಥಿತಿ 0x1eb8:
	    ಸ್ಥಿತಿ 0x1eba: ಸ್ಥಿತಿ 0x1ebc: ಸ್ಥಿತಿ 0x1ebe: ಸ್ಥಿತಿ 0x1ec0:
	    ಸ್ಥಿತಿ 0x1ec2: ಸ್ಥಿತಿ 0x1ec4: ಸ್ಥಿತಿ 0x1ec6:
		    EMIT2('E') EMIT2(E_grave) EMIT2(E_acute)
		    EMIT2(E_circumflex) EMIT2(E_diaeresis)
		    EMIT2(0x112) EMIT2(0x114) EMIT2(0x116)
		    EMIT2(0x118) EMIT2(0x11a) EMIT2(0x204)
		    EMIT2(0x206) EMIT2(0x228) EMIT2(0x246)
		    EMIT2(0x1e14) EMIT2(0x1e16) EMIT2(0x1e18)
		    EMIT2(0x1e1a) EMIT2(0x1e1c) EMIT2(0x1eb8)
		    EMIT2(0x1eba) EMIT2(0x1ebc) EMIT2(0x1ebe)
		    EMIT2(0x1ec0) EMIT2(0x1ec2) EMIT2(0x1ec4)
		    EMIT2(0x1ec6)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'F': ಸ್ಥಿತಿ 0x191: ಸ್ಥಿತಿ 0x1e1e: ಸ್ಥಿತಿ 0xa798:
		    EMIT2('F') EMIT2(0x191) EMIT2(0x1e1e) EMIT2(0xa798)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'G': ಸ್ಥಿತಿ 0x11c: ಸ್ಥಿತಿ 0x11e: ಸ್ಥಿತಿ 0x120:
	    ಸ್ಥಿತಿ 0x122: ಸ್ಥಿತಿ 0x193: ಸ್ಥಿತಿ 0x1e4: ಸ್ಥಿತಿ 0x1e6:
	    ಸ್ಥಿತಿ 0x1f4: ಸ್ಥಿತಿ 0x1e20: ಸ್ಥಿತಿ 0xa7a0:
		    EMIT2('G') EMIT2(0x11c) EMIT2(0x11e) EMIT2(0x120)
		    EMIT2(0x122) EMIT2(0x193) EMIT2(0x1e4)
		    EMIT2(0x1e6) EMIT2(0x1f4) EMIT2(0x1e20)
		    EMIT2(0xa7a0)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'H': ಸ್ಥಿತಿ 0x124: ಸ್ಥಿತಿ 0x126: ಸ್ಥಿತಿ 0x21e:
	    ಸ್ಥಿತಿ 0x1e22: ಸ್ಥಿತಿ 0x1e24: ಸ್ಥಿತಿ 0x1e26: ಸ್ಥಿತಿ 0x1e28:
	    ಸ್ಥಿತಿ 0x1e2a: ಸ್ಥಿತಿ 0x2c67:
		    EMIT2('H') EMIT2(0x124) EMIT2(0x126) EMIT2(0x21e)
		    EMIT2(0x1e22) EMIT2(0x1e24) EMIT2(0x1e26)
		    EMIT2(0x1e28) EMIT2(0x1e2a) EMIT2(0x2c67)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'I': ಸ್ಥಿತಿ I_grave: ಸ್ಥಿತಿ I_acute: ಸ್ಥಿತಿ I_circumflex:
	    ಸ್ಥಿತಿ I_diaeresis: ಸ್ಥಿತಿ 0x128: ಸ್ಥಿತಿ 0x12a: ಸ್ಥಿತಿ 0x12c:
	    ಸ್ಥಿತಿ 0x12e: ಸ್ಥಿತಿ 0x130: ಸ್ಥಿತಿ 0x197: ಸ್ಥಿತಿ 0x1cf:
	    ಸ್ಥಿತಿ 0x208: ಸ್ಥಿತಿ 0x20a: ಸ್ಥಿತಿ 0x1e2c: ಸ್ಥಿತಿ 0x1e2e:
	    ಸ್ಥಿತಿ 0x1ec8: ಸ್ಥಿತಿ 0x1eca:
		    EMIT2('I') EMIT2(I_grave) EMIT2(I_acute)
		    EMIT2(I_circumflex) EMIT2(I_diaeresis)
		    EMIT2(0x128) EMIT2(0x12a) EMIT2(0x12c)
		    EMIT2(0x12e) EMIT2(0x130) EMIT2(0x197)
		    EMIT2(0x1cf) EMIT2(0x208) EMIT2(0x20a)
		    EMIT2(0x1e2c) EMIT2(0x1e2e) EMIT2(0x1ec8)
		    EMIT2(0x1eca)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'J': ಸ್ಥಿತಿ 0x134: ಸ್ಥಿತಿ 0x248:
		    EMIT2('J') EMIT2(0x134) EMIT2(0x248)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'K': ಸ್ಥಿತಿ 0x136: ಸ್ಥಿತಿ 0x198: ಸ್ಥಿತಿ 0x1e8: ಸ್ಥಿತಿ 0x1e30:
	    ಸ್ಥಿತಿ 0x1e32: ಸ್ಥಿತಿ 0x1e34: ಸ್ಥಿತಿ 0x2c69: ಸ್ಥಿತಿ 0xa740:
		    EMIT2('K') EMIT2(0x136) EMIT2(0x198) EMIT2(0x1e8)
		    EMIT2(0x1e30) EMIT2(0x1e32) EMIT2(0x1e34)
		    EMIT2(0x2c69) EMIT2(0xa740)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'L': ಸ್ಥಿತಿ 0x139: ಸ್ಥಿತಿ 0x13b: ಸ್ಥಿತಿ 0x13d:
	    ಸ್ಥಿತಿ 0x13f: ಸ್ಥಿತಿ 0x141: ಸ್ಥಿತಿ 0x23d: ಸ್ಥಿತಿ 0x1e36:
	    ಸ್ಥಿತಿ 0x1e38: ಸ್ಥಿತಿ 0x1e3a: ಸ್ಥಿತಿ 0x1e3c: ಸ್ಥಿತಿ 0x2c60:
		    EMIT2('L') EMIT2(0x139) EMIT2(0x13b)
		    EMIT2(0x13d) EMIT2(0x13f) EMIT2(0x141)
		    EMIT2(0x23d) EMIT2(0x1e36) EMIT2(0x1e38)
		    EMIT2(0x1e3a) EMIT2(0x1e3c) EMIT2(0x2c60)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'M': ಸ್ಥಿತಿ 0x1e3e: ಸ್ಥಿತಿ 0x1e40: ಸ್ಥಿತಿ 0x1e42:
		    EMIT2('M') EMIT2(0x1e3e) EMIT2(0x1e40)
		    EMIT2(0x1e42)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'N': ಸ್ಥಿತಿ N_virguilla:
	    ಸ್ಥಿತಿ 0x143: ಸ್ಥಿತಿ 0x145: ಸ್ಥಿತಿ 0x147: ಸ್ಥಿತಿ 0x1f8:
	    ಸ್ಥಿತಿ 0x1e44: ಸ್ಥಿತಿ 0x1e46: ಸ್ಥಿತಿ 0x1e48: ಸ್ಥಿತಿ 0x1e4a:
	    ಸ್ಥಿತಿ 0xa7a4:
		    EMIT2('N') EMIT2(N_virguilla)
		    EMIT2(0x143) EMIT2(0x145) EMIT2(0x147)
		    EMIT2(0x1f8) EMIT2(0x1e44) EMIT2(0x1e46)
		    EMIT2(0x1e48) EMIT2(0x1e4a) EMIT2(0xa7a4)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'O': ಸ್ಥಿತಿ O_grave: ಸ್ಥಿತಿ O_acute: ಸ್ಥಿತಿ O_circumflex:
	    ಸ್ಥಿತಿ O_virguilla: ಸ್ಥಿತಿ O_diaeresis: ಸ್ಥಿತಿ O_slash:
	    ಸ್ಥಿತಿ 0x14c: ಸ್ಥಿತಿ 0x14e: ಸ್ಥಿತಿ 0x150: ಸ್ಥಿತಿ 0x19f:
	    ಸ್ಥಿತಿ 0x1a0: ಸ್ಥಿತಿ 0x1d1: ಸ್ಥಿತಿ 0x1ea: ಸ್ಥಿತಿ 0x1ec:
	    ಸ್ಥಿತಿ 0x1fe: ಸ್ಥಿತಿ 0x20c: ಸ್ಥಿತಿ 0x20e: ಸ್ಥಿತಿ 0x22a:
	    ಸ್ಥಿತಿ 0x22c: ಸ್ಥಿತಿ 0x22e: ಸ್ಥಿತಿ 0x230: ಸ್ಥಿತಿ 0x1e4c:
	    ಸ್ಥಿತಿ 0x1e4e: ಸ್ಥಿತಿ 0x1e50: ಸ್ಥಿತಿ 0x1e52: ಸ್ಥಿತಿ 0x1ecc:
	    ಸ್ಥಿತಿ 0x1ece: ಸ್ಥಿತಿ 0x1ed0: ಸ್ಥಿತಿ 0x1ed2: ಸ್ಥಿತಿ 0x1ed4:
	    ಸ್ಥಿತಿ 0x1ed6: ಸ್ಥಿತಿ 0x1ed8: ಸ್ಥಿತಿ 0x1eda: ಸ್ಥಿತಿ 0x1edc:
	    ಸ್ಥಿತಿ 0x1ede: ಸ್ಥಿತಿ 0x1ee0: ಸ್ಥಿತಿ 0x1ee2:
		    EMIT2('O') EMIT2(O_grave) EMIT2(O_acute)
		    EMIT2(O_circumflex) EMIT2(O_virguilla)
		    EMIT2(O_diaeresis) EMIT2(O_slash)
		    EMIT2(0x14c) EMIT2(0x14e) EMIT2(0x150)
		    EMIT2(0x19f) EMIT2(0x1a0) EMIT2(0x1d1)
		    EMIT2(0x1ea) EMIT2(0x1ec) EMIT2(0x1fe)
		    EMIT2(0x20c) EMIT2(0x20e) EMIT2(0x22a)
		    EMIT2(0x22c) EMIT2(0x22e) EMIT2(0x230)
		    EMIT2(0x1e4c) EMIT2(0x1e4e) EMIT2(0x1e50)
		    EMIT2(0x1e52) EMIT2(0x1ecc) EMIT2(0x1ece)
		    EMIT2(0x1ed0) EMIT2(0x1ed2) EMIT2(0x1ed4)
		    EMIT2(0x1ed6) EMIT2(0x1ed8) EMIT2(0x1eda)
		    EMIT2(0x1edc) EMIT2(0x1ede) EMIT2(0x1ee0)
		    EMIT2(0x1ee2)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'P': ಸ್ಥಿತಿ 0x1a4: ಸ್ಥಿತಿ 0x1e54: ಸ್ಥಿತಿ 0x1e56: ಸ್ಥಿತಿ 0x2c63:
		    EMIT2('P') EMIT2(0x1a4) EMIT2(0x1e54) EMIT2(0x1e56)
		    EMIT2(0x2c63)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'Q': ಸ್ಥಿತಿ 0x24a:
		    EMIT2('Q') EMIT2(0x24a)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'R': ಸ್ಥಿತಿ 0x154: ಸ್ಥಿತಿ 0x156: ಸ್ಥಿತಿ 0x158: ಸ್ಥಿತಿ 0x210:
	    ಸ್ಥಿತಿ 0x212: ಸ್ಥಿತಿ 0x24c: ಸ್ಥಿತಿ 0x1e58: ಸ್ಥಿತಿ 0x1e5a:
	    ಸ್ಥಿತಿ 0x1e5c: ಸ್ಥಿತಿ 0x1e5e: ಸ್ಥಿತಿ 0x2c64: ಸ್ಥಿತಿ 0xa7a6:
		    EMIT2('R') EMIT2(0x154) EMIT2(0x156) EMIT2(0x158)
		    EMIT2(0x210) EMIT2(0x212) EMIT2(0x24c) EMIT2(0x1e58)
		    EMIT2(0x1e5a) EMIT2(0x1e5c) EMIT2(0x1e5e) EMIT2(0x2c64)
		    EMIT2(0xa7a6)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'S': ಸ್ಥಿತಿ 0x15a: ಸ್ಥಿತಿ 0x15c: ಸ್ಥಿತಿ 0x15e: ಸ್ಥಿತಿ 0x160:
	    ಸ್ಥಿತಿ 0x218: ಸ್ಥಿತಿ 0x1e60: ಸ್ಥಿತಿ 0x1e62: ಸ್ಥಿತಿ 0x1e64:
	    ಸ್ಥಿತಿ 0x1e66: ಸ್ಥಿತಿ 0x1e68: ಸ್ಥಿತಿ 0x2c7e: ಸ್ಥಿತಿ 0xa7a8:
		    EMIT2('S') EMIT2(0x15a) EMIT2(0x15c) EMIT2(0x15e)
		    EMIT2(0x160) EMIT2(0x218) EMIT2(0x1e60) EMIT2(0x1e62)
		    EMIT2(0x1e64) EMIT2(0x1e66) EMIT2(0x1e68) EMIT2(0x2c7e)
		    EMIT2(0xa7a8)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'T': ಸ್ಥಿತಿ 0x162: ಸ್ಥಿತಿ 0x164: ಸ್ಥಿತಿ 0x166: ಸ್ಥಿತಿ 0x1ac:
	    ಸ್ಥಿತಿ 0x1ae: ಸ್ಥಿತಿ 0x21a: ಸ್ಥಿತಿ 0x23e: ಸ್ಥಿತಿ 0x1e6a: ಸ್ಥಿತಿ 0x1e6c:
	    ಸ್ಥಿತಿ 0x1e6e: ಸ್ಥಿತಿ 0x1e70:
		    EMIT2('T') EMIT2(0x162) EMIT2(0x164) EMIT2(0x166)
		    EMIT2(0x1ac) EMIT2(0x1ae) EMIT2(0x23e) EMIT2(0x21a)
		    EMIT2(0x1e6a) EMIT2(0x1e6c) EMIT2(0x1e6e) EMIT2(0x1e70)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'U': ಸ್ಥಿತಿ U_grave: ಸ್ಥಿತಿ U_acute: ಸ್ಥಿತಿ U_diaeresis:
	    ಸ್ಥಿತಿ U_circumflex: ಸ್ಥಿತಿ 0x168: ಸ್ಥಿತಿ 0x16a: ಸ್ಥಿತಿ 0x16c:
	    ಸ್ಥಿತಿ 0x16e: ಸ್ಥಿತಿ 0x170: ಸ್ಥಿತಿ 0x172: ಸ್ಥಿತಿ 0x1af:
	    ಸ್ಥಿತಿ 0x1d3: ಸ್ಥಿತಿ 0x1d5: ಸ್ಥಿತಿ 0x1d7: ಸ್ಥಿತಿ 0x1d9:
	    ಸ್ಥಿತಿ 0x1db: ಸ್ಥಿತಿ 0x214: ಸ್ಥಿತಿ 0x216: ಸ್ಥಿತಿ 0x244:
	    ಸ್ಥಿತಿ 0x1e72: ಸ್ಥಿತಿ 0x1e74: ಸ್ಥಿತಿ 0x1e76: ಸ್ಥಿತಿ 0x1e78:
	    ಸ್ಥಿತಿ 0x1e7a: ಸ್ಥಿತಿ 0x1ee4: ಸ್ಥಿತಿ 0x1ee6: ಸ್ಥಿತಿ 0x1ee8:
	    ಸ್ಥಿತಿ 0x1eea: ಸ್ಥಿತಿ 0x1eec: ಸ್ಥಿತಿ 0x1eee: ಸ್ಥಿತಿ 0x1ef0:
		    EMIT2('U') EMIT2(U_grave) EMIT2(U_acute)
		    EMIT2(U_diaeresis) EMIT2(U_circumflex)
		    EMIT2(0x168) EMIT2(0x16a)
		    EMIT2(0x16c) EMIT2(0x16e) EMIT2(0x170)
		    EMIT2(0x172) EMIT2(0x1af) EMIT2(0x1d3)
		    EMIT2(0x1d5) EMIT2(0x1d7) EMIT2(0x1d9)
		    EMIT2(0x1db) EMIT2(0x214) EMIT2(0x216)
		    EMIT2(0x244) EMIT2(0x1e72) EMIT2(0x1e74)
		    EMIT2(0x1e76) EMIT2(0x1e78) EMIT2(0x1e7a)
		    EMIT2(0x1ee4) EMIT2(0x1ee6) EMIT2(0x1ee8)
		    EMIT2(0x1eea) EMIT2(0x1eec) EMIT2(0x1eee)
		    EMIT2(0x1ef0)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'V': ಸ್ಥಿತಿ 0x1b2: ಸ್ಥಿತಿ 0x1e7c: ಸ್ಥಿತಿ 0x1e7e:
		    EMIT2('V') EMIT2(0x1b2) EMIT2(0x1e7c) EMIT2(0x1e7e)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'W': ಸ್ಥಿತಿ 0x174: ಸ್ಥಿತಿ 0x1e80: ಸ್ಥಿತಿ 0x1e82: ಸ್ಥಿತಿ 0x1e84:
	    ಸ್ಥಿತಿ 0x1e86: ಸ್ಥಿತಿ 0x1e88:
		    EMIT2('W') EMIT2(0x174) EMIT2(0x1e80) EMIT2(0x1e82)
		    EMIT2(0x1e84) EMIT2(0x1e86) EMIT2(0x1e88)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'X': ಸ್ಥಿತಿ 0x1e8a: ಸ್ಥಿತಿ 0x1e8c:
		    EMIT2('X') EMIT2(0x1e8a) EMIT2(0x1e8c)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'Y': ಸ್ಥಿತಿ Y_acute: ಸ್ಥಿತಿ 0x176: ಸ್ಥಿತಿ 0x178:
	    ಸ್ಥಿತಿ 0x1b3: ಸ್ಥಿತಿ 0x232: ಸ್ಥಿತಿ 0x24e: ಸ್ಥಿತಿ 0x1e8e:
	    ಸ್ಥಿತಿ 0x1ef2: ಸ್ಥಿತಿ 0x1ef4: ಸ್ಥಿತಿ 0x1ef6: ಸ್ಥಿತಿ 0x1ef8:
		    EMIT2('Y') EMIT2(Y_acute)
		    EMIT2(0x176) EMIT2(0x178) EMIT2(0x1b3)
		    EMIT2(0x232) EMIT2(0x24e) EMIT2(0x1e8e)
		    EMIT2(0x1ef2) EMIT2(0x1ef4) EMIT2(0x1ef6)
		    EMIT2(0x1ef8)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'Z': ಸ್ಥಿತಿ 0x179: ಸ್ಥಿತಿ 0x17b: ಸ್ಥಿತಿ 0x17d:
	    ಸ್ಥಿತಿ 0x1b5: ಸ್ಥಿತಿ 0x1e90: ಸ್ಥಿತಿ 0x1e92: ಸ್ಥಿತಿ 0x1e94:
	    ಸ್ಥಿತಿ 0x2c6b:
		    EMIT2('Z') EMIT2(0x179) EMIT2(0x17b) EMIT2(0x17d)
		    EMIT2(0x1b5) EMIT2(0x1e90) EMIT2(0x1e92)
		    EMIT2(0x1e94) EMIT2(0x2c6b)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'a': ಸ್ಥಿತಿ a_grave: ಸ್ಥಿತಿ a_acute: ಸ್ಥಿತಿ a_circumflex:
	    ಸ್ಥಿತಿ a_virguilla: ಸ್ಥಿತಿ a_diaeresis: ಸ್ಥಿತಿ a_ring:
	    ಸ್ಥಿತಿ 0x101: ಸ್ಥಿತಿ 0x103: ಸ್ಥಿತಿ 0x105: ಸ್ಥಿತಿ 0x1ce:
	    ಸ್ಥಿತಿ 0x1df: ಸ್ಥಿತಿ 0x1e1: ಸ್ಥಿತಿ 0x1fb: ಸ್ಥಿತಿ 0x201:
	    ಸ್ಥಿತಿ 0x203: ಸ್ಥಿತಿ 0x227: ಸ್ಥಿತಿ 0x1d8f: ಸ್ಥಿತಿ 0x1e01:
	    ಸ್ಥಿತಿ 0x1e9a: ಸ್ಥಿತಿ 0x1ea1: ಸ್ಥಿತಿ 0x1ea3: ಸ್ಥಿತಿ 0x1ea5:
	    ಸ್ಥಿತಿ 0x1ea7: ಸ್ಥಿತಿ 0x1ea9: ಸ್ಥಿತಿ 0x1eab: ಸ್ಥಿತಿ 0x1ead:
	    ಸ್ಥಿತಿ 0x1eaf: ಸ್ಥಿತಿ 0x1eb1: ಸ್ಥಿತಿ 0x1eb3: ಸ್ಥಿತಿ 0x1eb5:
	    ಸ್ಥಿತಿ 0x1eb7: ಸ್ಥಿತಿ 0x2c65:
		    EMIT2('a') EMIT2(a_grave) EMIT2(a_acute)
		    EMIT2(a_circumflex) EMIT2(a_virguilla)
		    EMIT2(a_diaeresis) EMIT2(a_ring)
		    EMIT2(0x101) EMIT2(0x103) EMIT2(0x105)
		    EMIT2(0x1ce) EMIT2(0x1df) EMIT2(0x1e1)
		    EMIT2(0x1fb) EMIT2(0x201) EMIT2(0x203)
		    EMIT2(0x227) EMIT2(0x1d8f) EMIT2(0x1e01)
		    EMIT2(0x1e9a) EMIT2(0x1ea1) EMIT2(0x1ea3)
		    EMIT2(0x1ea5) EMIT2(0x1ea7) EMIT2(0x1ea9)
		    EMIT2(0x1eab) EMIT2(0x1ead) EMIT2(0x1eaf)
		    EMIT2(0x1eb1) EMIT2(0x1eb3) EMIT2(0x1eb5)
		    EMIT2(0x1eb7) EMIT2(0x2c65)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'b': ಸ್ಥಿತಿ 0x180: ಸ್ಥಿತಿ 0x253: ಸ್ಥಿತಿ 0x1d6c: ಸ್ಥಿತಿ 0x1d80:
	    ಸ್ಥಿತಿ 0x1e03: ಸ್ಥಿತಿ 0x1e05: ಸ್ಥಿತಿ 0x1e07:
		    EMIT2('b') EMIT2(0x180) EMIT2(0x253) EMIT2(0x1d6c)
		    EMIT2(0x1d80) EMIT2(0x1e03) EMIT2(0x1e05) EMIT2(0x1e07)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'c': ಸ್ಥಿತಿ c_cedilla: ಸ್ಥಿತಿ 0x107: ಸ್ಥಿತಿ 0x109: ಸ್ಥಿತಿ 0x10b:
	    ಸ್ಥಿತಿ 0x10d: ಸ್ಥಿತಿ 0x188: ಸ್ಥಿತಿ 0x23c: ಸ್ಥಿತಿ 0x1e09: ಸ್ಥಿತಿ 0xa793:
	    ಸ್ಥಿತಿ 0xa794:
		    EMIT2('c') EMIT2(c_cedilla)
		    EMIT2(0x107) EMIT2(0x109) EMIT2(0x10b)
		    EMIT2(0x10d) EMIT2(0x188) EMIT2(0x23c)
		    EMIT2(0x1e09) EMIT2(0xa793) EMIT2(0xa794)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'd': ಸ್ಥಿತಿ 0x10f: ಸ್ಥಿತಿ 0x111: ಸ್ಥಿತಿ 0x257: ಸ್ಥಿತಿ 0x1d6d:
	    ಸ್ಥಿತಿ 0x1d81: ಸ್ಥಿತಿ 0x1d91: ಸ್ಥಿತಿ 0x1e0b: ಸ್ಥಿತಿ 0x1e0d: ಸ್ಥಿತಿ 0x1e0f:
	    ಸ್ಥಿತಿ 0x1e11: ಸ್ಥಿತಿ 0x1e13:
		    EMIT2('d') EMIT2(0x10f) EMIT2(0x111)
		    EMIT2(0x257) EMIT2(0x1d6d) EMIT2(0x1d81)
		    EMIT2(0x1d91) EMIT2(0x1e0b) EMIT2(0x1e0d)
		    EMIT2(0x1e0f) EMIT2(0x1e11) EMIT2(0x1e13)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'e': ಸ್ಥಿತಿ e_grave: ಸ್ಥಿತಿ e_acute: ಸ್ಥಿತಿ e_circumflex:
	    ಸ್ಥಿತಿ e_diaeresis: ಸ್ಥಿತಿ 0x113: ಸ್ಥಿತಿ 0x115: ಸ್ಥಿತಿ 0x117:
	    ಸ್ಥಿತಿ 0x119: ಸ್ಥಿತಿ 0x11b: ಸ್ಥಿತಿ 0x205: ಸ್ಥಿತಿ 0x207:
	    ಸ್ಥಿತಿ 0x229: ಸ್ಥಿತಿ 0x247: ಸ್ಥಿತಿ 0x1d92: ಸ್ಥಿತಿ 0x1e15:
	    ಸ್ಥಿತಿ 0x1e17: ಸ್ಥಿತಿ 0x1e19: ಸ್ಥಿತಿ 0x1e1b: ಸ್ಥಿತಿ 0x1e1d:
	    ಸ್ಥಿತಿ 0x1eb9: ಸ್ಥಿತಿ 0x1ebb: ಸ್ಥಿತಿ 0x1ebd: ಸ್ಥಿತಿ 0x1ebf:
	    ಸ್ಥಿತಿ 0x1ec1: ಸ್ಥಿತಿ 0x1ec3: ಸ್ಥಿತಿ 0x1ec5: ಸ್ಥಿತಿ 0x1ec7:
		    EMIT2('e') EMIT2(e_grave) EMIT2(e_acute)
		    EMIT2(e_circumflex) EMIT2(e_diaeresis)
		    EMIT2(0x113) EMIT2(0x115)
		    EMIT2(0x117) EMIT2(0x119) EMIT2(0x11b)
		    EMIT2(0x205) EMIT2(0x207) EMIT2(0x229)
		    EMIT2(0x247) EMIT2(0x1d92) EMIT2(0x1e15)
		    EMIT2(0x1e17) EMIT2(0x1e19) EMIT2(0x1e1b)
		    EMIT2(0x1e1d) EMIT2(0x1eb9) EMIT2(0x1ebb)
		    EMIT2(0x1ebd) EMIT2(0x1ebf) EMIT2(0x1ec1)
		    EMIT2(0x1ec3) EMIT2(0x1ec5) EMIT2(0x1ec7)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'f': ಸ್ಥಿತಿ 0x192: ಸ್ಥಿತಿ 0x1d6e: ಸ್ಥಿತಿ 0x1d82:
	    ಸ್ಥಿತಿ 0x1e1f: ಸ್ಥಿತಿ 0xa799:
		    EMIT2('f') EMIT2(0x192) EMIT2(0x1d6e) EMIT2(0x1d82)
		    EMIT2(0x1e1f) EMIT2(0xa799)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'g': ಸ್ಥಿತಿ 0x11d: ಸ್ಥಿತಿ 0x11f: ಸ್ಥಿತಿ 0x121: ಸ್ಥಿತಿ 0x123:
	    ಸ್ಥಿತಿ 0x1e5: ಸ್ಥಿತಿ 0x1e7: ಸ್ಥಿತಿ 0x1f5: ಸ್ಥಿತಿ 0x260: ಸ್ಥಿತಿ 0x1d83:
	    ಸ್ಥಿತಿ 0x1e21: ಸ್ಥಿತಿ 0xa7a1:
		    EMIT2('g') EMIT2(0x11d) EMIT2(0x11f) EMIT2(0x121)
		    EMIT2(0x123) EMIT2(0x1e5) EMIT2(0x1e7)
		    EMIT2(0x1f5) EMIT2(0x260) EMIT2(0x1d83)
		    EMIT2(0x1e21) EMIT2(0xa7a1)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'h': ಸ್ಥಿತಿ 0x125: ಸ್ಥಿತಿ 0x127: ಸ್ಥಿತಿ 0x21f: ಸ್ಥಿತಿ 0x1e23:
	    ಸ್ಥಿತಿ 0x1e25: ಸ್ಥಿತಿ 0x1e27: ಸ್ಥಿತಿ 0x1e29: ಸ್ಥಿತಿ 0x1e2b:
	    ಸ್ಥಿತಿ 0x1e96: ಸ್ಥಿತಿ 0x2c68: ಸ್ಥಿತಿ 0xa795:
		    EMIT2('h') EMIT2(0x125) EMIT2(0x127) EMIT2(0x21f)
		    EMIT2(0x1e23) EMIT2(0x1e25) EMIT2(0x1e27)
		    EMIT2(0x1e29) EMIT2(0x1e2b) EMIT2(0x1e96)
		    EMIT2(0x2c68) EMIT2(0xa795)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'i': ಸ್ಥಿತಿ i_grave: ಸ್ಥಿತಿ i_acute: ಸ್ಥಿತಿ i_circumflex:
	    ಸ್ಥಿತಿ i_diaeresis: ಸ್ಥಿತಿ 0x129: ಸ್ಥಿತಿ 0x12b: ಸ್ಥಿತಿ 0x12d:
	    ಸ್ಥಿತಿ 0x12f: ಸ್ಥಿತಿ 0x1d0: ಸ್ಥಿತಿ 0x209: ಸ್ಥಿತಿ 0x20b:
	    ಸ್ಥಿತಿ 0x268: ಸ್ಥಿತಿ 0x1d96: ಸ್ಥಿತಿ 0x1e2d: ಸ್ಥಿತಿ 0x1e2f:
	    ಸ್ಥಿತಿ 0x1ec9: ಸ್ಥಿತಿ 0x1ecb:
		    EMIT2('i') EMIT2(i_grave) EMIT2(i_acute)
		    EMIT2(i_circumflex) EMIT2(i_diaeresis)
		    EMIT2(0x129) EMIT2(0x12b) EMIT2(0x12d)
		    EMIT2(0x12f) EMIT2(0x1d0) EMIT2(0x209)
		    EMIT2(0x20b) EMIT2(0x268) EMIT2(0x1d96)
		    EMIT2(0x1e2d) EMIT2(0x1e2f) EMIT2(0x1ec9)
		    EMIT2(0x1ecb)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'j': ಸ್ಥಿತಿ 0x135: ಸ್ಥಿತಿ 0x1f0: ಸ್ಥಿತಿ 0x249:
		    EMIT2('j') EMIT2(0x135) EMIT2(0x1f0) EMIT2(0x249)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'k': ಸ್ಥಿತಿ 0x137: ಸ್ಥಿತಿ 0x199: ಸ್ಥಿತಿ 0x1e9: ಸ್ಥಿತಿ 0x1d84:
	    ಸ್ಥಿತಿ 0x1e31: ಸ್ಥಿತಿ 0x1e33: ಸ್ಥಿತಿ 0x1e35: ಸ್ಥಿತಿ 0x2c6a: ಸ್ಥಿತಿ 0xa741:
		    EMIT2('k') EMIT2(0x137) EMIT2(0x199) EMIT2(0x1e9)
		    EMIT2(0x1d84) EMIT2(0x1e31) EMIT2(0x1e33)
		    EMIT2(0x1e35) EMIT2(0x2c6a) EMIT2(0xa741)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'l': ಸ್ಥಿತಿ 0x13a: ಸ್ಥಿತಿ 0x13c: ಸ್ಥಿತಿ 0x13e: ಸ್ಥಿತಿ 0x140:
	    ಸ್ಥಿತಿ 0x142: ಸ್ಥಿತಿ 0x19a: ಸ್ಥಿತಿ 0x1e37: ಸ್ಥಿತಿ 0x1e39: ಸ್ಥಿತಿ 0x1e3b:
	    ಸ್ಥಿತಿ 0x1e3d: ಸ್ಥಿತಿ 0x2c61:
		    EMIT2('l') EMIT2(0x13a) EMIT2(0x13c)
		    EMIT2(0x13e) EMIT2(0x140) EMIT2(0x142)
		    EMIT2(0x19a) EMIT2(0x1e37) EMIT2(0x1e39)
		    EMIT2(0x1e3b) EMIT2(0x1e3d) EMIT2(0x2c61)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'm': ಸ್ಥಿತಿ 0x1d6f: ಸ್ಥಿತಿ 0x1e3f: ಸ್ಥಿತಿ 0x1e41: ಸ್ಥಿತಿ 0x1e43:
		    EMIT2('m') EMIT2(0x1d6f) EMIT2(0x1e3f)
		    EMIT2(0x1e41) EMIT2(0x1e43)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'n': ಸ್ಥಿತಿ n_virguilla: ಸ್ಥಿತಿ 0x144: ಸ್ಥಿತಿ 0x146: ಸ್ಥಿತಿ 0x148:
	    ಸ್ಥಿತಿ 0x149: ಸ್ಥಿತಿ 0x1f9: ಸ್ಥಿತಿ 0x1d70: ಸ್ಥಿತಿ 0x1d87: ಸ್ಥಿತಿ 0x1e45:
	    ಸ್ಥಿತಿ 0x1e47: ಸ್ಥಿತಿ 0x1e49: ಸ್ಥಿತಿ 0x1e4b: ಸ್ಥಿತಿ 0xa7a5:
		    EMIT2('n') EMIT2(n_virguilla)
		    EMIT2(0x144) EMIT2(0x146) EMIT2(0x148)
		    EMIT2(0x149) EMIT2(0x1f9) EMIT2(0x1d70)
		    EMIT2(0x1d87) EMIT2(0x1e45) EMIT2(0x1e47)
		    EMIT2(0x1e49) EMIT2(0x1e4b) EMIT2(0xa7a5)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'o': ಸ್ಥಿತಿ o_grave: ಸ್ಥಿತಿ o_acute: ಸ್ಥಿತಿ o_circumflex:
	    ಸ್ಥಿತಿ o_virguilla: ಸ್ಥಿತಿ o_diaeresis: ಸ್ಥಿತಿ o_slash:
	    ಸ್ಥಿತಿ 0x14d: ಸ್ಥಿತಿ 0x14f: ಸ್ಥಿತಿ 0x151: ಸ್ಥಿತಿ 0x1a1:
	    ಸ್ಥಿತಿ 0x1d2: ಸ್ಥಿತಿ 0x1eb: ಸ್ಥಿತಿ 0x1ed: ಸ್ಥಿತಿ 0x1ff:
	    ಸ್ಥಿತಿ 0x20d: ಸ್ಥಿತಿ 0x20f: ಸ್ಥಿತಿ 0x22b: ಸ್ಥಿತಿ 0x22d:
	    ಸ್ಥಿತಿ 0x22f: ಸ್ಥಿತಿ 0x231: ಸ್ಥಿತಿ 0x275: ಸ್ಥಿತಿ 0x1e4d:
	    ಸ್ಥಿತಿ 0x1e4f: ಸ್ಥಿತಿ 0x1e51: ಸ್ಥಿತಿ 0x1e53: ಸ್ಥಿತಿ 0x1ecd:
	    ಸ್ಥಿತಿ 0x1ecf: ಸ್ಥಿತಿ 0x1ed1: ಸ್ಥಿತಿ 0x1ed3: ಸ್ಥಿತಿ 0x1ed5:
	    ಸ್ಥಿತಿ 0x1ed7: ಸ್ಥಿತಿ 0x1ed9: ಸ್ಥಿತಿ 0x1edb: ಸ್ಥಿತಿ 0x1edd:
	    ಸ್ಥಿತಿ 0x1edf: ಸ್ಥಿತಿ 0x1ee1: ಸ್ಥಿತಿ 0x1ee3:
		    EMIT2('o') EMIT2(o_grave) EMIT2(o_acute)
		    EMIT2(o_circumflex) EMIT2(o_virguilla)
		    EMIT2(o_diaeresis) EMIT2(o_slash)
		    EMIT2(0x14d) EMIT2(0x14f) EMIT2(0x151)
		    EMIT2(0x1a1) EMIT2(0x1d2) EMIT2(0x1eb)
		    EMIT2(0x1ed) EMIT2(0x1ff) EMIT2(0x20d)
		    EMIT2(0x20f) EMIT2(0x22b) EMIT2(0x22d)
		    EMIT2(0x22f) EMIT2(0x231) EMIT2(0x275)
		    EMIT2(0x1e4d) EMIT2(0x1e4f) EMIT2(0x1e51)
		    EMIT2(0x1e53) EMIT2(0x1ecd) EMIT2(0x1ecf)
		    EMIT2(0x1ed1) EMIT2(0x1ed3) EMIT2(0x1ed5)
		    EMIT2(0x1ed7) EMIT2(0x1ed9) EMIT2(0x1edb)
		    EMIT2(0x1edd) EMIT2(0x1edf) EMIT2(0x1ee1)
		    EMIT2(0x1ee3)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'p': ಸ್ಥಿತಿ 0x1a5: ಸ್ಥಿತಿ 0x1d71: ಸ್ಥಿತಿ 0x1d7d: ಸ್ಥಿತಿ 0x1d88:
	    ಸ್ಥಿತಿ 0x1e55: ಸ್ಥಿತಿ 0x1e57:
		    EMIT2('p') EMIT2(0x1a5) EMIT2(0x1d71) EMIT2(0x1d7d)
		    EMIT2(0x1d88) EMIT2(0x1e55) EMIT2(0x1e57)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'q': ಸ್ಥಿತಿ 0x24b: ಸ್ಥಿತಿ 0x2a0:
		    EMIT2('q') EMIT2(0x24b) EMIT2(0x2a0)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'r': ಸ್ಥಿತಿ 0x155: ಸ್ಥಿತಿ 0x157: ಸ್ಥಿತಿ 0x159: ಸ್ಥಿತಿ 0x211:
	    ಸ್ಥಿತಿ 0x213: ಸ್ಥಿತಿ 0x24d: ಸ್ಥಿತಿ 0x27d: ಸ್ಥಿತಿ 0x1d72: ಸ್ಥಿತಿ 0x1d73:
	    ಸ್ಥಿತಿ 0x1d89: ಸ್ಥಿತಿ 0x1e59: ಸ್ಥಿತಿ 0x1e5b: ಸ್ಥಿತಿ 0x1e5d: ಸ್ಥಿತಿ 0x1e5f:
	    ಸ್ಥಿತಿ 0xa7a7:
		    EMIT2('r') EMIT2(0x155) EMIT2(0x157) EMIT2(0x159)
		    EMIT2(0x211) EMIT2(0x213) EMIT2(0x24d) EMIT2(0x27d)
		    EMIT2(0x1d72) EMIT2(0x1d73) EMIT2(0x1d89) EMIT2(0x1e59)
		    EMIT2(0x1e5b) EMIT2(0x1e5d) EMIT2(0x1e5f) EMIT2(0xa7a7)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 's': ಸ್ಥಿತಿ 0x15b: ಸ್ಥಿತಿ 0x15d: ಸ್ಥಿತಿ 0x15f: ಸ್ಥಿತಿ 0x161:
	    ಸ್ಥಿತಿ 0x219: ಸ್ಥಿತಿ 0x23f: ಸ್ಥಿತಿ 0x1d74: ಸ್ಥಿತಿ 0x1d8a: ಸ್ಥಿತಿ 0x1e61:
	    ಸ್ಥಿತಿ 0x1e63: ಸ್ಥಿತಿ 0x1e65: ಸ್ಥಿತಿ 0x1e67: ಸ್ಥಿತಿ 0x1e69: ಸ್ಥಿತಿ 0xa7a9:
		    EMIT2('s') EMIT2(0x15b) EMIT2(0x15d) EMIT2(0x15f)
		    EMIT2(0x161) EMIT2(0x219) EMIT2(0x23f) EMIT2(0x1d74)
		    EMIT2(0x1d8a) EMIT2(0x1e61) EMIT2(0x1e63) EMIT2(0x1e65)
		    EMIT2(0x1e67) EMIT2(0x1e69) EMIT2(0xa7a9)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 't': ಸ್ಥಿತಿ 0x163: ಸ್ಥಿತಿ 0x165: ಸ್ಥಿತಿ 0x167: ಸ್ಥಿತಿ 0x1ab:
	    ಸ್ಥಿತಿ 0x1ad: ಸ್ಥಿತಿ 0x21b: ಸ್ಥಿತಿ 0x288: ಸ್ಥಿತಿ 0x1d75: ಸ್ಥಿತಿ 0x1e6b:
	    ಸ್ಥಿತಿ 0x1e6d: ಸ್ಥಿತಿ 0x1e6f: ಸ್ಥಿತಿ 0x1e71: ಸ್ಥಿತಿ 0x1e97: ಸ್ಥಿತಿ 0x2c66:
		    EMIT2('t') EMIT2(0x163) EMIT2(0x165) EMIT2(0x167)
		    EMIT2(0x1ab) EMIT2(0x1ad) EMIT2(0x21b) EMIT2(0x288)
		    EMIT2(0x1d75) EMIT2(0x1e6b) EMIT2(0x1e6d) EMIT2(0x1e6f)
		    EMIT2(0x1e71) EMIT2(0x1e97) EMIT2(0x2c66)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'u': ಸ್ಥಿತಿ u_grave: ಸ್ಥಿತಿ u_acute: ಸ್ಥಿತಿ u_circumflex:
	    ಸ್ಥಿತಿ u_diaeresis: ಸ್ಥಿತಿ 0x169: ಸ್ಥಿತಿ 0x16b: ಸ್ಥಿತಿ 0x16d:
	    ಸ್ಥಿತಿ 0x16f: ಸ್ಥಿತಿ 0x171: ಸ್ಥಿತಿ 0x173: ಸ್ಥಿತಿ 0x1b0: ಸ್ಥಿತಿ 0x1d4:
	    ಸ್ಥಿತಿ 0x1d6: ಸ್ಥಿತಿ 0x1d8: ಸ್ಥಿತಿ 0x1da: ಸ್ಥಿತಿ 0x1dc: ಸ್ಥಿತಿ 0x215:
	    ಸ್ಥಿತಿ 0x217: ಸ್ಥಿತಿ 0x289: ಸ್ಥಿತಿ 0x1d7e: ಸ್ಥಿತಿ 0x1d99: ಸ್ಥಿತಿ 0x1e73:
	    ಸ್ಥಿತಿ 0x1e75: ಸ್ಥಿತಿ 0x1e77: ಸ್ಥಿತಿ 0x1e79: ಸ್ಥಿತಿ 0x1e7b:
	    ಸ್ಥಿತಿ 0x1ee5: ಸ್ಥಿತಿ 0x1ee7: ಸ್ಥಿತಿ 0x1ee9: ಸ್ಥಿತಿ 0x1eeb:
	    ಸ್ಥಿತಿ 0x1eed: ಸ್ಥಿತಿ 0x1eef: ಸ್ಥಿತಿ 0x1ef1:
		    EMIT2('u') EMIT2(u_grave) EMIT2(u_acute)
		    EMIT2(u_circumflex) EMIT2(u_diaeresis)
		    EMIT2(0x169) EMIT2(0x16b)
		    EMIT2(0x16d) EMIT2(0x16f) EMIT2(0x171)
		    EMIT2(0x173) EMIT2(0x1d6) EMIT2(0x1d8)
		    EMIT2(0x215) EMIT2(0x217) EMIT2(0x1b0)
		    EMIT2(0x1d4) EMIT2(0x1da) EMIT2(0x1dc)
		    EMIT2(0x289) EMIT2(0x1e73) EMIT2(0x1d7e)
		    EMIT2(0x1d99) EMIT2(0x1e75) EMIT2(0x1e77)
		    EMIT2(0x1e79) EMIT2(0x1e7b) EMIT2(0x1ee5)
		    EMIT2(0x1ee7) EMIT2(0x1ee9) EMIT2(0x1eeb)
		    EMIT2(0x1eed) EMIT2(0x1eef) EMIT2(0x1ef1)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'v': ಸ್ಥಿತಿ 0x28b: ಸ್ಥಿತಿ 0x1d8c: ಸ್ಥಿತಿ 0x1e7d: ಸ್ಥಿತಿ 0x1e7f:
		    EMIT2('v') EMIT2(0x28b) EMIT2(0x1d8c) EMIT2(0x1e7d)
		    EMIT2(0x1e7f)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'w': ಸ್ಥಿತಿ 0x175: ಸ್ಥಿತಿ 0x1e81: ಸ್ಥಿತಿ 0x1e83: ಸ್ಥಿತಿ 0x1e85:
	    ಸ್ಥಿತಿ 0x1e87: ಸ್ಥಿತಿ 0x1e89: ಸ್ಥಿತಿ 0x1e98:
		    EMIT2('w') EMIT2(0x175) EMIT2(0x1e81) EMIT2(0x1e83)
		    EMIT2(0x1e85) EMIT2(0x1e87) EMIT2(0x1e89) EMIT2(0x1e98)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'x': ಸ್ಥಿತಿ 0x1e8b: ಸ್ಥಿತಿ 0x1e8d:
		    EMIT2('x') EMIT2(0x1e8b) EMIT2(0x1e8d)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'y': ಸ್ಥಿತಿ y_acute: ಸ್ಥಿತಿ y_diaeresis: ಸ್ಥಿತಿ 0x177:
	    ಸ್ಥಿತಿ 0x1b4: ಸ್ಥಿತಿ 0x233: ಸ್ಥಿತಿ 0x24f: ಸ್ಥಿತಿ 0x1e8f:
	    ಸ್ಥಿತಿ 0x1e99: ಸ್ಥಿತಿ 0x1ef3: ಸ್ಥಿತಿ 0x1ef5: ಸ್ಥಿತಿ 0x1ef7:
	    ಸ್ಥಿತಿ 0x1ef9:
		    EMIT2('y') EMIT2(y_acute) EMIT2(y_diaeresis)
		    EMIT2(0x177) EMIT2(0x1b4) EMIT2(0x233) EMIT2(0x24f)
		    EMIT2(0x1e8f) EMIT2(0x1e99) EMIT2(0x1ef3)
		    EMIT2(0x1ef5) EMIT2(0x1ef7) EMIT2(0x1ef9)
		    ಹಿಂತಿರುಗಿ OK;

	    ಸ್ಥಿತಿ 'z': ಸ್ಥಿತಿ 0x17a: ಸ್ಥಿತಿ 0x17c: ಸ್ಥಿತಿ 0x17e: ಸ್ಥಿತಿ 0x1b6:
	    ಸ್ಥಿತಿ 0x1d76: ಸ್ಥಿತಿ 0x1d8e: ಸ್ಥಿತಿ 0x1e91: ಸ್ಥಿತಿ 0x1e93:
	    ಸ್ಥಿತಿ 0x1e95: ಸ್ಥಿತಿ 0x2c6c:
		    EMIT2('z') EMIT2(0x17a) EMIT2(0x17c) EMIT2(0x17e)
		    EMIT2(0x1b6) EMIT2(0x1d76) EMIT2(0x1d8e) EMIT2(0x1e91)
		    EMIT2(0x1e93) EMIT2(0x1e95) EMIT2(0x2c6c)
		    ಹಿಂತಿರುಗಿ OK;

	    // default: character itself
	}
    }

    EMIT2(c);
    ಹಿಂತಿರುಗಿ OK;
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು EMIT2
}

/*
 * Code to parse regular expression.
 *
 * We try to reuse parsing functions in regexp.c to
 * minimize surprise and keep the syntax consistent.
 */

/*
 * Parse the lowest level.
 *
 * An atom can be one of a long list of items.  Many atoms match one character
 * in the text.  It is often an ordinary character or a character class.
 * Braces can be used to make a pattern into an atom.  The "\z(\)" construct
 * is only for syntax highlighting.
 *
 * atom    ::=     ordinary-atom
 *     or  \( pattern \)
 *     or  \%( pattern \)
 *     or  \z( pattern \)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
nfa_regatom(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		charclass;
    ಪೂರ್ಣಾಂಕ		equiclass;
    ಪೂರ್ಣಾಂಕ		collclass;
    ಪೂರ್ಣಾಂಕ		got_coll_char;
    char_u	*p;
    char_u	*endp;
    char_u	*old_regparse = regparse;
    ಪೂರ್ಣಾಂಕ		extra = 0;
    ಪೂರ್ಣಾಂಕ		emit_range;
    ಪೂರ್ಣಾಂಕ		negated;
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ;
    ಪೂರ್ಣಾಂಕ		startc = -1;
    ಪೂರ್ಣಾಂಕ		save_prev_at_ಆರಂಭ = prev_at_ಆರಂಭ;

    c = getchr();
    ಆಯ್ಕೆ (c)
    {
	ಸ್ಥಿತಿ NUL:
	    EMSG_RET_FAIL(_(e_nfa_regexp_ಕೊನೆ_encountered_prematurely));

	ಸ್ಥಿತಿ Magic('^'):
	    EMIT(NFA_BOL);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Magic('$'):
	    EMIT(NFA_EOL);
#ಆದರೆ defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Magic('<'):
	    EMIT(NFA_BOW);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Magic('>'):
	    EMIT(NFA_EOW);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Magic('_'):
	    c = no_Magic(getchr());
	    ಆದರೆ (c == NUL)
		EMSG_RET_FAIL(_(e_nfa_regexp_ಕೊನೆ_encountered_prematurely));

	    ಆದರೆ (c == '^')	// "\_^" is start-of-line
	    {
		EMIT(NFA_BOL);
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (c == '$')	// "\_$" is end-of-line
	    {
		EMIT(NFA_EOL);
#ಆದರೆ defined(FEAT_SYN_HL)
		had_eol = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
		ನಿಲ್ಲಿಸು;
	    }

	    extra = NFA_ಕೂಡಿಸು_NL;

	    // "\_[" is collection plus newline
	    ಆದರೆ (c == '[')
		ಹೋಗು collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	/*
	 * Character classes.
	 */
	ಸ್ಥಿತಿ Magic('.'):
	ಸ್ಥಿತಿ Magic('i'):
	ಸ್ಥಿತಿ Magic('I'):
	ಸ್ಥಿತಿ Magic('k'):
	ಸ್ಥಿತಿ Magic('K'):
	ಸ್ಥಿತಿ Magic('f'):
	ಸ್ಥಿತಿ Magic('F'):
	ಸ್ಥಿತಿ Magic('p'):
	ಸ್ಥಿತಿ Magic('P'):
	ಸ್ಥಿತಿ Magic('s'):
	ಸ್ಥಿತಿ Magic('S'):
	ಸ್ಥಿತಿ Magic('d'):
	ಸ್ಥಿತಿ Magic('D'):
	ಸ್ಥಿತಿ Magic('x'):
	ಸ್ಥಿತಿ Magic('X'):
	ಸ್ಥಿತಿ Magic('o'):
	ಸ್ಥಿತಿ Magic('O'):
	ಸ್ಥಿತಿ Magic('w'):
	ಸ್ಥಿತಿ Magic('W'):
	ಸ್ಥಿತಿ Magic('h'):
	ಸ್ಥಿತಿ Magic('H'):
	ಸ್ಥಿತಿ Magic('a'):
	ಸ್ಥಿತಿ Magic('A'):
	ಸ್ಥಿತಿ Magic('l'):
	ಸ್ಥಿತಿ Magic('L'):
	ಸ್ಥಿತಿ Magic('u'):
	ಸ್ಥಿತಿ Magic('U'):
	    p = vim_strchr(classchars, no_Magic(c));
	    ಆದರೆ (p == ಖಾಲಿ)
	    {
		ಆದರೆ (extra == NFA_ಕೂಡಿಸು_NL)
		{
		    semsg(_(e_nfa_regexp_invalid_character_class_nr), c);
		    rc_did_emsg = TRUE;
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		siemsg("Unknown character class char: %d", c);
		ಹಿಂತಿರುಗಿ FAIL;
	    }

	    // When '.' is followed by a composing char ignore the dot, so that
	    // the composing char is matched here.
	    ಆದರೆ (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	    {
		old_regparse = regparse;
		c = getchr();
		ಹೋಗು nfa_do_multibyte;
	    }
	    EMIT(nfa_classcodes[p - classchars]);
	    ಆದರೆ (extra == NFA_ಕೂಡಿಸು_NL)
	    {
		EMIT(NFA_NEWL);
		EMIT(NFA_OR);
		regflags |= RF_HASNL;
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Magic('n'):
	    ಆದರೆ (reg_string)
		// In a string "\n" matches a newline character.
		EMIT(NL);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// In buffer text "\n" matches the end of a line.
		EMIT(NFA_NEWL);
		regflags |= RF_HASNL;
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Magic('('):
	    ಆದರೆ (nfa_reg(REG_PAREN) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;	    // cascaded error
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Magic('|'):
	ಸ್ಥಿತಿ Magic('&'):
	ಸ್ಥಿತಿ Magic(')'):
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    ಹಿಂತಿರುಗಿ FAIL;

	ಸ್ಥಿತಿ Magic('='):
	ಸ್ಥಿತಿ Magic('?'):
	ಸ್ಥಿತಿ Magic('+'):
	ಸ್ಥಿತಿ Magic('@'):
	ಸ್ಥಿತಿ Magic('*'):
	ಸ್ಥಿತಿ Magic('{'):
	    // these should follow an atom, not form an atom
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    ಹಿಂತಿರುಗಿ FAIL;

	ಸ್ಥಿತಿ Magic('~'):
	    {
		char_u	    *lp;

		// Previous substitute pattern.
		// Generated as "\%(pattern\)".
		ಆದರೆ (reg_prev_sub == ಖಾಲಿ)
		{
		    emsg(_(e_no_ಹಿಂದಿನ_substitute_regular_ಅಭಿವ್ಯಕ್ತಿ));
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		ಗಾಗಿ (lp = reg_prev_sub; *lp != NUL; MB_CPTR_ADV(lp))
		{
		    EMIT(PTR2CHAR(lp));
		    ಆದರೆ (lp != reg_prev_sub)
			EMIT(NFA_CONCAT);
		}
		EMIT(NFA_NOPEN);
		ನಿಲ್ಲಿಸು;
	    }

	ಸ್ಥಿತಿ Magic('1'):
	ಸ್ಥಿತಿ Magic('2'):
	ಸ್ಥಿತಿ Magic('3'):
	ಸ್ಥಿತಿ Magic('4'):
	ಸ್ಥಿತಿ Magic('5'):
	ಸ್ಥಿತಿ Magic('6'):
	ಸ್ಥಿತಿ Magic('7'):
	ಸ್ಥಿತಿ Magic('8'):
	ಸ್ಥಿತಿ Magic('9'):
	    {
		ಪೂರ್ಣಾಂಕ refnum = no_Magic(c) - '1';

		ಆದರೆ (!ನೋಡಿದ_endbrace(refnum + 1))
		    ಹಿಂತಿರುಗಿ FAIL;
		EMIT(NFA_BACKREF1 + refnum);
		rex.nfa_has_backref = TRUE;
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Magic('z'):
	    c = no_Magic(getchr());
	    ಆಯ್ಕೆ (c)
	    {
		ಸ್ಥಿತಿ 's':
		    EMIT(NFA_ZSTART);
		    ಆದರೆ (re_mult_ಮುಂದಿನ("\\zs") == FAIL)
			ಹಿಂತಿರುಗಿ FAIL;
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'e':
		    EMIT(NFA_ZEND);
		    rex.nfa_has_zend = TRUE;
		    ಆದರೆ (re_mult_ಮುಂದಿನ("\\ze") == FAIL)
			ಹಿಂತಿರುಗಿ FAIL;
		    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
		ಸ್ಥಿತಿ '1':
		ಸ್ಥಿತಿ '2':
		ಸ್ಥಿತಿ '3':
		ಸ್ಥಿತಿ '4':
		ಸ್ಥಿತಿ '5':
		ಸ್ಥಿತಿ '6':
		ಸ್ಥಿತಿ '7':
		ಸ್ಥಿತಿ '8':
		ಸ್ಥಿತಿ '9':
		    // \z1...\z9
		    ಆದರೆ ((reg_do_extmatch & REX_USE) == 0)
			EMSG_RET_FAIL(_(e_z1_z9_not_allowed_ಇಲ್ಲಿ));
		    EMIT(NFA_ZREF1 + (no_Magic(c) - '1'));
		    // No need to set rex.nfa_has_backref, the sub-matches don't
		    // change when \z1 .. \z9 matches or not.
		    re_has_z = REX_USE;
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '(':
		    // \z(
		    ಆದರೆ ((reg_do_extmatch & REX_ಗಣ) == 0)
			EMSG_RET_FAIL(_(e_z_not_allowed_ಇಲ್ಲಿ));
		    ಆದರೆ (nfa_reg(REG_ZPAREN) == FAIL)
			ಹಿಂತಿರುಗಿ FAIL;	    // cascaded error
		    re_has_z = REX_ಗಣ;
		    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
		ಪೂರ್ವನಿಯೋಜಿತ:
		    semsg(_(e_nfa_regexp_unknown_operator_z_chr), no_Magic(c));
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Magic('%'):
	    c = no_Magic(getchr());
	    ಆಯ್ಕೆ (c)
	    {
		// () without a back reference
		ಸ್ಥಿತಿ '(':
		    ಆದರೆ (nfa_reg(REG_NPAREN) == FAIL)
			ಹಿಂತಿರುಗಿ FAIL;
		    EMIT(NFA_NOPEN);
		    ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 'd':   // %d123 decimal
		ಸ್ಥಿತಿ 'o':   // %o123 octal
		ಸ್ಥಿತಿ 'x':   // %xab hex 2
		ಸ್ಥಿತಿ 'u':   // %uabcd hex 4
		ಸ್ಥಿತಿ 'U':   // %U1234abcd hex 8
		    {
			vimlong_T nr;

			ಆಯ್ಕೆ (c)
			{
			    ಸ್ಥಿತಿ 'd': nr = getdecchrs(); ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ 'o': nr = getoctchrs(); ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ 'x': nr = gethexchrs(2); ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ 'u': nr = gethexchrs(4); ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ 'U': nr = gethexchrs(8); ನಿಲ್ಲಿಸು;
			    ಪೂರ್ವನಿಯೋಜಿತ:  nr = -1; ನಿಲ್ಲಿಸು;
			}

			ಆದರೆ (nr < 0 || nr > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
			    EMSG2_RET_FAIL(_(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			// A NUL is stored in the text as NL
			// TODO: what if a composing character follows?
			EMIT(nr == 0 ? 0x0a : (ದೀರ್ಘ)nr);
		    }
		    ನಿಲ್ಲಿಸು;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		ಸ್ಥಿತಿ '^':
		    EMIT(NFA_BOF);
		    ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ '$':
		    EMIT(NFA_EOF);
		    ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ '#':
		    ಆದರೆ (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_ಆರಂಭ_ದ_ನಮೂನೆ),
								  regparse[1]);
			ಹಿಂತಿರುಗಿ FAIL;
		    }
		    EMIT(NFA_CURSOR);
		    ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 'V':
		    EMIT(NFA_VISUAL);
		    ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 'C':
		    EMIT(NFA_ANY_COMPOSING);
		    ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ '[':
		    {
			ಪೂರ್ಣಾಂಕ	    n;

			// \%[abc]
			ಗಾಗಿ (n = 0; (c = peekchr()) != ']'; ++n)
			{
			    ಆದರೆ (c == NUL)
				EMSG2_RET_FAIL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
			    // recursive call!
			    ಆದರೆ (nfa_regatom() == FAIL)
				ಹಿಂತಿರುಗಿ FAIL;
			}
			getchr();  // get the ]
			ಆದರೆ (n == 0)
			    EMSG2_RET_FAIL(_(e_ಖಾಲಿಗಳು_str_brackets),
						      reg_magic == MAGIC_ALL);
			EMIT(NFA_OPT_CHARS);
			EMIT(n);

			// Emit as "\%(\%[abc]\)" to be able to handle
			// "\%[abc]*" which would cause the empty string to be
			// matched an unlimited number of times. NFA_NOPEN is
			// added only once at a position, while NFA_SPLIT is
			// added multiple times.  This is more efficient than
			// not allowing NFA_SPLIT multiple times, it is used
			// a lot.
			EMIT(NFA_NOPEN);
			ನಿಲ್ಲಿಸು;
		    }

		ಪೂರ್ವನಿಯೋಜಿತ:
		    {
			long_u	n = 0;
			ಪೂರ್ಣಾಂಕ	cmp = c;
			ಪೂರ್ಣಾಂಕ	cur = FALSE;
			ಪೂರ್ಣಾಂಕ	got_digit = FALSE;

			ಆದರೆ (c == '<' || c == '>')
			    c = getchr();
			ಆದರೆ (no_Magic(c) == '.')
			{
			    cur = TRUE;
			    c = getchr();
			}
			ಆಗ (VIM_ISDIGIT(c))
			{
			    long_u tmp;

			    ಆದರೆ (cur)
			    {
				semsg(_(e_regexp_ಸಂಖ್ಯೆ_after_dot_pos_ಹುಡುಕು_chr),
								  no_Magic(c));
				ಹಿಂತಿರುಗಿ FAIL;
			    }
			    tmp = n * 10 + (c - '0');

			    ಆದರೆ (tmp < n)
			    {
				// overflow.
				emsg(_(e_percent_ಮೌಲ್ಯ_too_large));
				ಹಿಂತಿರುಗಿ FAIL;
			    }
			    n = tmp;
			    c = getchr();
			    got_digit = TRUE;
			}
			ಆದರೆ (c == 'l' || c == 'c' || c == 'v')
			{
			    long_u ಎಲ್ಲೆ = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;

			    ಆದರೆ (!cur && !got_digit)
			    {
				semsg(_(e_nfa_regexp_missing_ಮೌಲ್ಯ_ಒಳ_chr),
								  no_Magic(c));
				ಹಿಂತಿರುಗಿ FAIL;
			    }
			    ಆದರೆ (c == 'l')
			    {
				ಆದರೆ (cur)
				    n = curwin->w_cursor.lnum;
				// \%{n}l  \%{n}<l  \%{n}>l
				EMIT(cmp == '<' ? NFA_LNUM_LT :
				     cmp == '>' ? NFA_LNUM_GT : NFA_LNUM);
				ಆದರೆ (save_prev_at_ಆರಂಭ)
				    at_ಆರಂಭ = TRUE;
			    }
			    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 'c')
			    {
				ಆದರೆ (cur)
				{
				    n = curwin->w_cursor.ಲಂಬಸಾಲು;
				    n++;
				}
				// \%{n}c  \%{n}<c  \%{n}>c
				EMIT(cmp == '<' ? NFA_ಲಂಬಸಾಲು_LT :
				     cmp == '>' ? NFA_ಲಂಬಸಾಲು_GT : NFA_ಲಂಬಸಾಲು);
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
			    {
				ಆದರೆ (cur)
				{
				    colnr_T vcol = 0;

				    getvvcol(curwin, &curwin->w_cursor,
							 ಖಾಲಿ, ಖಾಲಿ, &vcol, 0);
				    n = ++vcol;
				}
				// \%{n}v  \%{n}<v  \%{n}>v
				EMIT(cmp == '<' ? NFA_VCOL_LT :
				     cmp == '>' ? NFA_VCOL_GT : NFA_VCOL);
				ಎಲ್ಲೆ = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ / MB_MAXBYTES;
			    }
			    ಆದರೆ (n >= ಎಲ್ಲೆ)
			    {
				emsg(_(e_percent_ಮೌಲ್ಯ_too_large));
				ಹಿಂತಿರುಗಿ FAIL;
			    }
			    EMIT((ಪೂರ್ಣಾಂಕ)n);
			    ನಿಲ್ಲಿಸು;
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (no_Magic(c) == '\'' && n == 0)
			{
			    // \%'m  \%<'m  \%>'m
			    EMIT(cmp == '<' ? NFA_MARK_LT :
				 cmp == '>' ? NFA_MARK_GT : NFA_MARK);
			    EMIT(getchr());
			    ನಿಲ್ಲಿಸು;
			}
		    }
		    semsg(_(e_nfa_regexp_unknown_operator_percent_chr),
								  no_Magic(c));
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Magic('['):
collection:
	    /*
	     * [abc]  uses NFA_START_COLL - NFA_END_COLL
	     * [^abc] uses NFA_START_NEG_COLL - NFA_END_NEG_COLL
	     * Each character is produced as a regular state, using
	     * NFA_CONCAT to bind them together.
	     * Besides normal characters there can be:
	     * - character classes  NFA_CLASS_*
	     * - ranges, two characters followed by NFA_RANGE.
	     */

	    p = regparse;
	    endp = skip_anyof(p);
	    ಆದರೆ (*endp == ']')
	    {
		ಪೂರ್ಣಾಂಕ plen;
		bool range_endpoint;
		/*
		 * Try to reverse engineer character classes. For example,
		 * recognize that [0-9] stands for \d and [A-Za-z_] for \h,
		 * and perform the necessary substitutions in the NFA.
		 */
		ಫಲಿತಾಂಶ = nfa_recognize_char_class(regparse, endp,
							 extra == NFA_ಕೂಡಿಸು_NL);
		ಆದರೆ (ಫಲಿತಾಂಶ != FAIL)
		{
		    ಆದರೆ (ಫಲಿತಾಂಶ >= NFA_ಮೊದಲ_NL && ಫಲಿತಾಂಶ <= NFA_ಕೊನೆಯ_NL)
		    {
			EMIT(ಫಲಿತಾಂಶ - NFA_ಕೂಡಿಸು_NL);
			EMIT(NFA_NEWL);
			EMIT(NFA_OR);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			EMIT(ಫಲಿತಾಂಶ);
		    regparse = endp;
		    MB_PTR_ADV(regparse);
		    ಹಿಂತಿರುಗಿ OK;
		}
		/*
		 * Failed to recognize a character class. Use the simple
		 * version that turns [abc] into 'a' OR 'b' OR 'c'
		 */
		startc = -1;
		negated = FALSE;
		ಆದರೆ (*regparse == '^')			// negated range
		{
		    negated = TRUE;
		    MB_PTR_ADV(regparse);
		    EMIT(NFA_ಆರಂಭ_NEG_COLL);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    EMIT(NFA_ಆರಂಭ_COLL);
		ಆದರೆ (*regparse == '-')
		{
		    startc = '-';
		    EMIT(startc);
		    EMIT(NFA_CONCAT);
		    MB_PTR_ADV(regparse);
		}
		// Emit the OR branches for each character in the []
		emit_range = FALSE;
		ಆಗ (regparse < endp)
		{
		    ಪೂರ್ಣಾಂಕ	    oldstartc = startc;
		    range_endpoint = false;

		    startc = -1;
		    got_coll_char = FALSE;
		    ಆದರೆ (*regparse == '[')
		    {
			// Check for [: :], [= =], [. .]
			equiclass = collclass = 0;
			charclass = get_char_class(&regparse);
			ಆದರೆ (charclass == CLASS_NONE)
			{
			    equiclass = get_equi_class(&regparse);
			    ಆದರೆ (equiclass == 0)
				collclass = get_coll_element(&regparse);
			}

			// Character class like [:alpha:]
			ಆದರೆ (charclass != CLASS_NONE)
			{
			    ಆಯ್ಕೆ (charclass)
			    {
				ಸ್ಥಿತಿ CLASS_ALNUM:
				    EMIT(NFA_CLASS_ALNUM);
				    ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ CLASS_ALPHA:
				    EMIT(NFA_CLASS_ALPHA);
				    ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ CLASS_BLANK:
				    EMIT(NFA_CLASS_BLANK);
				    ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ CLASS_CNTRL:
				    EMIT(NFA_CLASS_CNTRL);
				    ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ CLASS_DIGIT:
				    EMIT(NFA_CLASS_DIGIT);
				    ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ CLASS_ನಕ್ಷೆ:
				    EMIT(NFA_CLASS_ನಕ್ಷೆ);
				    ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ CLASS_ಅಧೋ:
				    ಬಯಸುತ್ತದೆ_nfa = TRUE;
				    EMIT(NFA_CLASS_ಅಧೋ);
				    ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ CLASS_PRINT:
				    EMIT(NFA_CLASS_PRINT);
				    ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ CLASS_PUNCT:
				    EMIT(NFA_CLASS_PUNCT);
				    ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ CLASS_SPACE:
				    EMIT(NFA_CLASS_SPACE);
				    ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ CLASS_ಊರ್ಧ್ವ:
				    ಬಯಸುತ್ತದೆ_nfa = TRUE;
				    EMIT(NFA_CLASS_ಊರ್ಧ್ವ);
				    ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ CLASS_XDIGIT:
				    EMIT(NFA_CLASS_XDIGIT);
				    ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ CLASS_TAB:
				    EMIT(NFA_CLASS_TAB);
				    ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ CLASS_RETURN:
				    EMIT(NFA_CLASS_RETURN);
				    ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ CLASS_BACKSPACE:
				    EMIT(NFA_CLASS_BACKSPACE);
				    ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ CLASS_ESCAPE:
				    EMIT(NFA_CLASS_ESCAPE);
				    ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ CLASS_IDENT:
				    EMIT(NFA_CLASS_IDENT);
				    ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ CLASS_KEYWORD:
				    EMIT(NFA_CLASS_KEYWORD);
				    ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ CLASS_FNAME:
				    EMIT(NFA_CLASS_FNAME);
				    ನಿಲ್ಲಿಸು;
			    }
			    EMIT(NFA_CONCAT);
			    ಮುಂದುವರಿ;
			}
			// Try equivalence class [=a=] and the like
			ಆದರೆ (equiclass != 0)
			{
			    ಫಲಿತಾಂಶ = nfa_emit_equi_class(equiclass);
			    ಆದರೆ (ಫಲಿತಾಂಶ == FAIL)
			    {
				// should never happen
				EMSG_RET_FAIL(_(e_error_building_nfa_with_equivalence_class));
			    }
			    ಮುಂದುವರಿ;
			}
			// Try collating class like [. .]
			ಆದರೆ (collclass != 0)
			{
			    startc = collclass;	 // allow [.a.]-x as a range
			    // Will emit the proper atom at the end of the
			    // while loop.
			}
		    }
		    // Try a range like 'a-x' or '\t-z'. Also allows '-' as a
		    // start character.
		    ಆದರೆ (*regparse == '-' && oldstartc != -1)
		    {
			emit_range = TRUE;
			startc = oldstartc;
			MB_PTR_ADV(regparse);
			ಮುಂದುವರಿ;	    // reading the end of the range
		    }

		    // Now handle simple and escaped characters.
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    ಆದರೆ (*regparse == '\\'
			    && !reg_cpo_bsl
			    && regparse + 1 <= endp
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != ಖಾಲಿ
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR, regparse[1])
								      != ಖಾಲಿ)
			    )
			)
		    {
			MB_PTR_ADV(regparse);

			ಆದರೆ (*regparse == 'n')
			    startc = (reg_string || emit_range
					|| regparse[1] == '-') ? NL : NFA_NEWL;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*regparse == 'd'
				    || *regparse == 'o'
				    || *regparse == 'x'
				    || *regparse == 'u'
				    || *regparse == 'U'
				)
			    {
				// TODO(RE) This needs more testing
				startc = coll_get_char();
				// max UTF-8 Codepoint is U+10FFFF,
				// but allow values until INT_MAX
				ಆದರೆ (startc == ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
				    EMSG_RET_FAIL(_(e_unicode_val_too_large));
				got_coll_char = TRUE;
				MB_PTR_ಹಿಂಭಾಗ(old_regparse, regparse);
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
			    {
				// \r,\t,\e,\b
				startc = backslash_trans(*regparse);
			    }
		    }

		    // Normal printable char
		    ಆದರೆ (startc == -1)
			startc = PTR2CHAR(regparse);

		    // Previous char was '-', so this char is end of range.
		    ಆದರೆ (emit_range)
		    {
			ಪೂರ್ಣಾಂಕ	endc = startc;
			range_endpoint = true;

			startc = oldstartc;
			ಆದರೆ (startc > endc)
			    EMSG_RET_FAIL(_(e_ಹಿಮ್ಮುಖ_range_ಒಳ_character_class));

			ಆದರೆ (endc > startc + 2)
			{
			    // Emit a range instead of the sequence of
			    // individual characters.
			    ಆದರೆ (startc == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    ಇಲ್ಲದಿದ್ದರೆ
				--post_ptr; // remove NFA_CONCAT
			    EMIT(endc);
			    EMIT(NFA_RANGE);
			    EMIT(NFA_CONCAT);
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte && ((*mb_char2len)(startc) > 1
				    || (*mb_char2len)(endc) > 1))
			{
			    // Emit the characters in the range.
			    // "startc" was already emitted, so skip it.
			    //
			    ಗಾಗಿ (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    // Emit the range. "startc" was already emitted, so
			    // skip it.
			    ಗಾಗಿ (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			emit_range = FALSE;
			startc = -1;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// This char (startc) is not part of a range. Just
			// emit it.
			// Normally, simply emit startc. But if we get char
			// code=0 from a collating char, then replace it with
			// 0x0a.
			// This is needed to completely mimic the behaviour of
			// the backtracking engine.
			ಆದರೆ (startc == NFA_NEWL)
			{
			    // Line break can't be matched as part of the
			    // collection, add an OR below. But not for negated
			    // range.
			    ಆದರೆ (!negated)
				extra = NFA_ಕೂಡಿಸು_NL;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    ಆದರೆ (got_coll_char == TRUE && startc == 0)
			    {
				EMIT(0x0a);
				EMIT(NFA_CONCAT);
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
			    {
				EMIT(startc);
				ಆದರೆ (!(enc_utf8 && (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse)))))
				{
				    EMIT(NFA_CONCAT);
				}
			    }
			}
		    }

		    //
		    // If this character was consumed as the end of a range, do not emit its
		    // composing characters separately.  Range handling only uses the base
		    // codepoint; emitting the composing part again would duplicate the
		    // character in the postfix stream and corrupt the NFA stack.
		    //
		    ಆದರೆ (!range_endpoint && enc_utf8 &&
			    (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse))))
		    {
			ಪೂರ್ಣಾಂಕ i = utf_ptr2len(regparse);

			c = utf_ptr2char(regparse + i);

			// Add composing characters
			ಗಾಗಿ (;;)
			{
			    ಆದರೆ (c == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    ಇಲ್ಲದಿದ್ದರೆ
				EMIT(c);
			    EMIT(NFA_CONCAT);
			    ಆದರೆ ((i += utf_char2len(c)) >= plen)
				ನಿಲ್ಲಿಸು;
			    c = utf_ptr2char(regparse + i);
			}
			EMIT(NFA_COMPOSING);
			EMIT(NFA_CONCAT);
		    }
		    MB_PTR_ADV(regparse);
		} // while (p < endp)

		MB_PTR_ಹಿಂಭಾಗ(old_regparse, regparse);
		ಆದರೆ (*regparse == '-')	    // if last, '-' is just a char
		{
		    EMIT('-');
		    EMIT(NFA_CONCAT);
		}

		// skip the trailing ]
		regparse = endp;
		MB_PTR_ADV(regparse);

		// Mark end of the collection.
		ಆದರೆ (negated == TRUE)
		    EMIT(NFA_ಕೊನೆ_NEG_COLL);
		ಇಲ್ಲದಿದ್ದರೆ
		    EMIT(NFA_ಕೊನೆ_COLL);

		// \_[] also matches \n but it's not negated
		ಆದರೆ (extra == NFA_ಕೂಡಿಸು_NL)
		{
		    EMIT(reg_string ? NL : NFA_NEWL);
		    EMIT(NFA_OR);
		}

		ಹಿಂತಿರುಗಿ OK;
	    } // if exists closing ]

	    ಆದರೆ (reg_strict)
		EMSG_RET_FAIL(_(e_missing_rsb_after_str_lsb));
	    // FALLTHROUGH

	ಪೂರ್ವನಿಯೋಜಿತ:
	    {
		ಪೂರ್ಣಾಂಕ	plen;

nfa_do_multibyte:
		// plen is length of current char with composing chars
		ಆದರೆ (enc_utf8 && ((*mb_char2len)(c)
			    != (plen = utfc_ptr2len(old_regparse))
						       || utf_iscomposing(c)))
		{
		    ಪೂರ್ಣಾಂಕ i = 0;

		    // A base character plus composing characters, or just one
		    // or more composing characters.
		    // This requires creating a separate atom as if enclosing
		    // the characters in (), where NFA_COMPOSING is the ( and
		    // NFA_END_COMPOSING is the ). Note that right now we are
		    // building the postfix form, not the NFA itself;
		    // a composing char could be: a, b, c, NFA_COMPOSING
		    // where 'b' and 'c' are chars with codes > 256.
		    ಗಾಗಿ (;;)
		    {
			EMIT(c);
			ಆದರೆ (i > 0)
			    EMIT(NFA_CONCAT);
			ಆದರೆ ((i += utf_char2len(c)) >= plen)
			    ನಿಲ್ಲಿಸು;
			c = utf_ptr2char(old_regparse + i);
		    }
		    EMIT(NFA_COMPOSING);
		    regparse = old_regparse + plen;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    c = no_Magic(c);
		    EMIT(c);
		}
		ಹಿಂತಿರುಗಿ OK;
	    }
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Parse something followed by possible [*+=].
 *
 * A piece is an atom, possibly followed by a multi, an indication of how many
 * times the atom can be matched.  Example: "a*" matches any sequence of "a"
 * characters: "", "a", "aa", etc.
 *
 * piece   ::=	    atom
 *	or  atom  multi
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
nfa_regpiece(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		op;
    ಪೂರ್ಣಾಂಕ		ret;
    ದೀರ್ಘ	minval, maxval;
    ಪೂರ್ಣಾಂಕ		greedy = TRUE;      // Braces are prefixed with '-' ?
    parse_state_T old_state;
    parse_state_T new_state;
    ದೀರ್ಘ	c2;
    ಪೂರ್ಣಾಂಕ		old_post_pos;
    ಪೂರ್ಣಾಂಕ		my_post_ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ		quest;

    // Save the current parse state, so that we can use it if <atom>{m,n} is
    // next.
    save_parse_state(&old_state);

    // store current pos in the postfix form, for \{m,n} involving 0s
    my_post_ಆರಂಭ = (ಪೂರ್ಣಾಂಕ)(post_ptr - post_ಆರಂಭ);

    ret = nfa_regatom();
    ಆದರೆ (ret == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;	    // cascaded error

    op = peekchr();
    ಆದರೆ (re_multi_type(op) == NOT_MULTI)
	ಹಿಂತಿರುಗಿ OK;

    skipchr();
    ಆಯ್ಕೆ (op)
    {
	ಸ್ಥಿತಿ Magic('*'):
	    EMIT(NFA_ನಕ್ಷತ್ರ);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Magic('+'):
	    /*
	     * Trick: Normally, (a*)\+ would match the whole input "aaa".  The
	     * first and only submatch would be "aaa". But the backtracking
	     * engine interprets the plus as "try matching one more time", and
	     * a* matches a second time at the end of the input, the empty
	     * string.
	     * The submatch will be the empty string.
	     *
	     * In order to be consistent with the old engine, we replace
	     * <atom>+ with <atom><atom>*
	     */
	    restore_parse_state(&old_state);
	    curchr = -1;
	    ಆದರೆ (nfa_regatom() == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    EMIT(NFA_ನಕ್ಷತ್ರ);
	    EMIT(NFA_CONCAT);
	    skipchr();		// skip the \+
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Magic('@'):
	    c2 = (ದೀರ್ಘ)getdecchrs();
	    op = no_Magic(getchr());
	    i = 0;
	    ಆಯ್ಕೆ(op)
	    {
		ಸ್ಥಿತಿ '=':
		    // \@=
		    i = NFA_PREV_ATOM_NO_ಅಗಲ;
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '!':
		    // \@!
		    i = NFA_PREV_ATOM_NO_ಅಗಲ_NEG;
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '<':
		    op = no_Magic(getchr());
		    ಆದರೆ (op == '=')
			// \@<=
			i = NFA_PREV_ATOM_JUST_BEFORE;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (op == '!')
			// \@<!
			i = NFA_PREV_ATOM_JUST_BEFORE_NEG;
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '>':
		    // \@>
		    i = NFA_PREV_ATOM_LIKE_ನಮೂನೆ;
		    ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (i == 0)
	    {
		semsg(_(e_nfa_regexp_unknown_operator_at_chr), op);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    EMIT(i);
	    ಆದರೆ (i == NFA_PREV_ATOM_JUST_BEFORE
					|| i == NFA_PREV_ATOM_JUST_BEFORE_NEG)
		EMIT(c2);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Magic('?'):
	ಸ್ಥಿತಿ Magic('='):
	    EMIT(NFA_QUEST);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Magic('{'):
	    // a{2,5} will expand to 'aaa?a?a?'
	    // a{-1,3} will expand to 'aa??a??', where ?? is the nongreedy
	    // version of '?'
	    // \v(ab){2,3} will expand to '(ab)(ab)(ab)?', where all the
	    // parenthesis have the same id

	    greedy = TRUE;
	    c2 = peekchr();
	    ಆದರೆ (c2 == '-' || c2 == Magic('-'))
	    {
		skipchr();
		greedy = FALSE;
	    }
	    ಆದರೆ (!read_limits(&minval, &maxval))
		EMSG_RET_FAIL(_(e_nfa_regexp_error_reading_repetition_limits));

	    //  <atom>{0,inf}, <atom>{0,} and <atom>{}  are equivalent to
	    //  <atom>*
	    ಆದರೆ (minval == 0 && maxval == MAX_ಎಲ್ಲೆ)
	    {
		ಆದರೆ (greedy)		// { { (match the braces)
		    // \{}, \{0,}
		    EMIT(NFA_ನಕ್ಷತ್ರ);
		ಇಲ್ಲದಿದ್ದರೆ			// { { (match the braces)
		    // \{-}, \{-0,}
		    EMIT(NFA_ನಕ್ಷತ್ರ_NONGREEDY);
		ನಿಲ್ಲಿಸು;
	    }

	    // Special case: x{0} or x{-0}
	    ಆದರೆ (maxval == 0)
	    {
		// Ignore result of previous call to nfa_regatom()
		post_ptr = post_ಆರಂಭ + my_post_ಆರಂಭ;
		// NFA_EMPTY is 0-length and works everywhere
		EMIT(NFA_ಖಾಲಿಗಳು);
		ಹಿಂತಿರುಗಿ OK;
	    }

	    // The engine is very inefficient (uses too many states) when the
	    // maximum is much larger than the minimum and when the maximum is
	    // large.  However, when maxval is MAX_LIMIT, it is okay, as this
	    // will emit NFA_STAR.
	    // Bail out if we can use the other engine, but only, when the
	    // pattern does not need the NFA engine like (e.g. [[:upper:]]\{2,\}
	    // does not work with characters > 8 bit with the BT engine)
	    ಆದರೆ ((nfa_re_flags & RE_AUTO)
				   && (maxval > 500 || maxval > minval + 200)
				   && (maxval != MAX_ಎಲ್ಲೆ && minval < 200)
				   && !ಬಯಸುತ್ತದೆ_nfa)
		ಹಿಂತಿರುಗಿ FAIL;

	    // Ignore previous call to nfa_regatom()
	    post_ptr = post_ಆರಂಭ + my_post_ಆರಂಭ;
	    // Save parse state after the repeated atom and the \{}
	    save_parse_state(&new_state);

	    quest = (greedy == TRUE? NFA_QUEST : NFA_QUEST_NONGREEDY);
	    ಗಾಗಿ (i = 0; i < maxval; i++)
	    {
		// Goto beginning of the repeated atom
		restore_parse_state(&old_state);
		old_post_pos = (ಪೂರ್ಣಾಂಕ)(post_ptr - post_ಆರಂಭ);
		ಆದರೆ (nfa_regatom() == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
		// after "minval" times, atoms are optional
		ಆದರೆ (i + 1 > minval)
		{
		    ಆದರೆ (maxval == MAX_ಎಲ್ಲೆ)
		    {
			ಆದರೆ (greedy)
			    EMIT(NFA_ನಕ್ಷತ್ರ);
			ಇಲ್ಲದಿದ್ದರೆ
			    EMIT(NFA_ನಕ್ಷತ್ರ_NONGREEDY);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			EMIT(quest);
		}
		ಆದರೆ (old_post_pos != my_post_ಆರಂಭ)
		    EMIT(NFA_CONCAT);
		ಆದರೆ (i + 1 > minval && maxval == MAX_ಎಲ್ಲೆ)
		    ನಿಲ್ಲಿಸು;
	    }

	    // Go to just after the repeated atom and the \{}
	    restore_parse_state(&new_state);
	    curchr = -1;

	    ನಿಲ್ಲಿಸು;


	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
    }	// end switch

    ಆದರೆ (re_multi_type(peekchr()) != NOT_MULTI)
	// Can't have a multi follow a multi.
	EMSG_RET_FAIL(_(e_nfa_regexp_cant_have_multi_follow_multi));

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Parse one or more pieces, concatenated.  It matches a match for the
 * first piece, followed by a match for the second piece, etc.  Example:
 * "f[0-9]b", first matches "f", then a digit and then "b".
 *
 * concat  ::=	    piece
 *	or  piece piece
 *	or  piece piece piece
 *	etc.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
nfa_regconcat(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		cont = TRUE;
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ = TRUE;

    ಆಗ (cont)
    {
	ಆಯ್ಕೆ (peekchr())
	{
	    ಸ್ಥಿತಿ NUL:
	    ಸ್ಥಿತಿ Magic('|'):
	    ಸ್ಥಿತಿ Magic('&'):
	    ಸ್ಥಿತಿ Magic(')'):
		cont = FALSE;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ Magic('Z'):
		regflags |= RF_ICOMBINE;
		skipchr_keepstart();
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ Magic('c'):
		regflags |= RF_ICASE;
		skipchr_keepstart();
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ Magic('C'):
		regflags |= RF_NOICASE;
		skipchr_keepstart();
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ Magic('v'):
		reg_magic = MAGIC_ALL;
		skipchr_keepstart();
		curchr = -1;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ Magic('m'):
		reg_magic = MAGIC_ON;
		skipchr_keepstart();
		curchr = -1;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ Magic('M'):
		reg_magic = MAGIC_OFF;
		skipchr_keepstart();
		curchr = -1;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ Magic('V'):
		reg_magic = MAGIC_NONE;
		skipchr_keepstart();
		curchr = -1;
		ನಿಲ್ಲಿಸು;

	    ಪೂರ್ವನಿಯೋಜಿತ:
		ಆದರೆ (nfa_regpiece() == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
		ಆದರೆ (ಮೊದಲ == FALSE)
		    EMIT(NFA_CONCAT);
		ಇಲ್ಲದಿದ್ದರೆ
		    ಮೊದಲ = FALSE;
		ನಿಲ್ಲಿಸು;
	}
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Parse a branch, one or more concats, separated by "\&".  It matches the
 * last concat, but only if all the preceding concats also match at the same
 * position.  Examples:
 *      "foobeep\&..." matches "foo" in "foobeep".
 *      ".*Peter\&.*Bob" matches in a line containing both "Peter" and "Bob"
 *
 * branch ::=	    concat
 *		or  concat \& concat
 *		or  concat \& concat \& concat
 *		etc.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
nfa_regbranch(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		old_post_pos;

    old_post_pos = (ಪೂರ್ಣಾಂಕ)(post_ptr - post_ಆರಂಭ);

    // First branch, possibly the only one
    ಆದರೆ (nfa_regconcat() == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    // Try next concats
    ಆಗ (peekchr() == Magic('&'))
    {
	skipchr();
	// if concat is empty do emit a node
	ಆದರೆ (old_post_pos == (ಪೂರ್ಣಾಂಕ)(post_ptr - post_ಆರಂಭ))
	    EMIT(NFA_ಖಾಲಿಗಳು);
	EMIT(NFA_NOPEN);
	EMIT(NFA_PREV_ATOM_NO_ಅಗಲ);
	old_post_pos = (ಪೂರ್ಣಾಂಕ)(post_ptr - post_ಆರಂಭ);
	ಆದರೆ (nfa_regconcat() == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	// if concat is empty do emit a node
	ಆದರೆ (old_post_pos == (ಪೂರ್ಣಾಂಕ)(post_ptr - post_ಆರಂಭ))
	    EMIT(NFA_ಖಾಲಿಗಳು);
	EMIT(NFA_CONCAT);
    }

    // if a branch is empty, emit one node for it
    ಆದರೆ (old_post_pos == (ಪೂರ್ಣಾಂಕ)(post_ptr - post_ಆರಂಭ))
	EMIT(NFA_ಖಾಲಿಗಳು);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 *  Parse a pattern, one or more branches, separated by "\|".  It matches
 *  anything that matches one of the branches.  Example: "foo\|beep" matches
 *  "foo" and matches "beep".  If more than one branch matches, the first one
 *  is used.
 *
 *  pattern ::=	    branch
 *	or  branch \| branch
 *	or  branch \| branch \| branch
 *	etc.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
nfa_reg(
    ಪೂರ್ಣಾಂಕ		paren)	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
{
    ಪೂರ್ಣಾಂಕ		parno = 0;
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2 = FAIL;

    ಆದರೆ (paren == REG_PAREN)
    {
	ಆದರೆ (regnpar >= NSUBEXP) // Too many `('
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_parens));
	parno = regnpar++;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	ಆದರೆ (regnzpar >= NSUBEXP)
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_z));
	parno = regnzpar++;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (nfa_reg_parse_ಆಳ >= REG_MAX_PAREN_ಆಳ)
	EMSG_RET_FAIL(_(e_command_too_complex));
    ++nfa_reg_parse_ಆಳ;

    ಆದರೆ (nfa_regbranch() == FAIL)
	ಹೋಗು theend;	    // cascaded error

    ಆಗ (peekchr() == Magic('|'))
    {
	skipchr();
	ಆದರೆ (nfa_regbranch() == FAIL)
	    ಹೋಗು theend;    // cascaded error
	EMIT(NFA_OR);
    }

    // Check for proper termination.
    ಆದರೆ (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
	ಆದರೆ (paren == REG_NPAREN)
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	ಇಲ್ಲದಿದ್ದರೆ
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	rc_did_emsg = TRUE;
	ಹೋಗು theend;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (paren == REG_NOPAREN && peekchr() != NUL)
    {
	ಆದರೆ (peekchr() == Magic(')'))
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(e_nfa_regexp_proper_termination_error));
	rc_did_emsg = TRUE;
	ಹೋಗು theend;
    }
    /*
     * Here we set the flag allowing back references to this set of
     * parentheses.
     */
    ಆದರೆ (paren == REG_PAREN)
    {
	had_endbrace[parno] = TRUE;     // have seen the close paren
	EMIT(NFA_MOPEN + parno);
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (paren == REG_ZPAREN)
	EMIT(NFA_ZOPEN + parno);
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿತಿ_2 = OK;

theend:
    --nfa_reg_parse_ಆಳ;
    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
ಸ್ಥಿರ char_u ಸಂಕೇತ_2[50];

    ಸ್ಥಿರ ಶೂನ್ಯ
nfa_ಗಣ_ಸಂಕೇತ(ಪೂರ್ಣಾಂಕ c)
{
    ಪೂರ್ಣಾಂಕ	    addnl = FALSE;

    ಆದರೆ (c >= NFA_ಮೊದಲ_NL && c <= NFA_ಕೊನೆಯ_NL)
    {
	addnl = TRUE;
	c -= NFA_ಕೂಡಿಸು_NL;
    }

    STRCPY(ಸಂಕೇತ_2, "");
    ಆಯ್ಕೆ (c)
    {
	ಸ್ಥಿತಿ NFA_ಹೊಂದಿಕೆ:	    STRCPY(ಸಂಕೇತ_2, "NFA_MATCH "); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ವಿಭಜಿಸು:	    STRCPY(ಸಂಕೇತ_2, "NFA_SPLIT "); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CONCAT:    STRCPY(ಸಂಕೇತ_2, "NFA_CONCAT "); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_NEWL:	    STRCPY(ಸಂಕೇತ_2, "NFA_NEWL "); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ZSTART:    STRCPY(ಸಂಕೇತ_2, "NFA_ZSTART"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ZEND:	    STRCPY(ಸಂಕೇತ_2, "NFA_ZEND"); ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ NFA_BACKREF1:  STRCPY(ಸಂಕೇತ_2, "NFA_BACKREF1"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_BACKREF2:  STRCPY(ಸಂಕೇತ_2, "NFA_BACKREF2"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_BACKREF3:  STRCPY(ಸಂಕೇತ_2, "NFA_BACKREF3"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_BACKREF4:  STRCPY(ಸಂಕೇತ_2, "NFA_BACKREF4"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_BACKREF5:  STRCPY(ಸಂಕೇತ_2, "NFA_BACKREF5"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_BACKREF6:  STRCPY(ಸಂಕೇತ_2, "NFA_BACKREF6"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_BACKREF7:  STRCPY(ಸಂಕೇತ_2, "NFA_BACKREF7"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_BACKREF8:  STRCPY(ಸಂಕೇತ_2, "NFA_BACKREF8"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_BACKREF9:  STRCPY(ಸಂಕೇತ_2, "NFA_BACKREF9"); ನಿಲ್ಲಿಸು;
# ifdef FEAT_SYN_HL
	ಸ್ಥಿತಿ NFA_ZREF1:	    STRCPY(ಸಂಕೇತ_2, "NFA_ZREF1"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ZREF2:	    STRCPY(ಸಂಕೇತ_2, "NFA_ZREF2"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ZREF3:	    STRCPY(ಸಂಕೇತ_2, "NFA_ZREF3"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ZREF4:	    STRCPY(ಸಂಕೇತ_2, "NFA_ZREF4"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ZREF5:	    STRCPY(ಸಂಕೇತ_2, "NFA_ZREF5"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ZREF6:	    STRCPY(ಸಂಕೇತ_2, "NFA_ZREF6"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ZREF7:	    STRCPY(ಸಂಕೇತ_2, "NFA_ZREF7"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ZREF8:	    STRCPY(ಸಂಕೇತ_2, "NFA_ZREF8"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ZREF9:	    STRCPY(ಸಂಕೇತ_2, "NFA_ZREF9"); ನಿಲ್ಲಿಸು;
# endif
	ಸ್ಥಿತಿ NFA_SKIP:	    STRCPY(ಸಂಕೇತ_2, "NFA_SKIP"); ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ NFA_PREV_ATOM_NO_ಅಗಲ:
			    STRCPY(ಸಂಕೇತ_2, "NFA_PREV_ATOM_NO_WIDTH"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_PREV_ATOM_NO_ಅಗಲ_NEG:
			    STRCPY(ಸಂಕೇತ_2, "NFA_PREV_ATOM_NO_WIDTH_NEG"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_PREV_ATOM_JUST_BEFORE:
			    STRCPY(ಸಂಕೇತ_2, "NFA_PREV_ATOM_JUST_BEFORE"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_PREV_ATOM_JUST_BEFORE_NEG:
			 STRCPY(ಸಂಕೇತ_2, "NFA_PREV_ATOM_JUST_BEFORE_NEG"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_PREV_ATOM_LIKE_ನಮೂನೆ:
			    STRCPY(ಸಂಕೇತ_2, "NFA_PREV_ATOM_LIKE_PATTERN"); ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ NFA_NOPEN:		    STRCPY(ಸಂಕೇತ_2, "NFA_NOPEN"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_NCLOSE:	    STRCPY(ಸಂಕೇತ_2, "NFA_NCLOSE"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE:   STRCPY(ಸಂಕೇತ_2, "NFA_START_INVISIBLE"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_ಮೊದಲ:
			     STRCPY(ಸಂಕೇತ_2, "NFA_START_INVISIBLE_FIRST"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_NEG:
			       STRCPY(ಸಂಕೇತ_2, "NFA_START_INVISIBLE_NEG"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_NEG_ಮೊದಲ:
			 STRCPY(ಸಂಕೇತ_2, "NFA_START_INVISIBLE_NEG_FIRST"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_BEFORE:
			    STRCPY(ಸಂಕೇತ_2, "NFA_START_INVISIBLE_BEFORE"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_BEFORE_ಮೊದಲ:
		      STRCPY(ಸಂಕೇತ_2, "NFA_START_INVISIBLE_BEFORE_FIRST"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_BEFORE_NEG:
			STRCPY(ಸಂಕೇತ_2, "NFA_START_INVISIBLE_BEFORE_NEG"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_BEFORE_NEG_ಮೊದಲ:
		  STRCPY(ಸಂಕೇತ_2, "NFA_START_INVISIBLE_BEFORE_NEG_FIRST"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ಆರಂಭ_ನಮೂನೆ:   STRCPY(ಸಂಕೇತ_2, "NFA_START_PATTERN"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ಕೊನೆ_INVISIBLE:	    STRCPY(ಸಂಕೇತ_2, "NFA_END_INVISIBLE"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ಕೊನೆ_INVISIBLE_NEG: STRCPY(ಸಂಕೇತ_2, "NFA_END_INVISIBLE_NEG"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ಕೊನೆ_ನಮೂನೆ:	    STRCPY(ಸಂಕೇತ_2, "NFA_END_PATTERN"); ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ NFA_COMPOSING:	    STRCPY(ಸಂಕೇತ_2, "NFA_COMPOSING"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ಕೊನೆ_COMPOSING:	    STRCPY(ಸಂಕೇತ_2, "NFA_END_COMPOSING"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_OPT_CHARS:	    STRCPY(ಸಂಕೇತ_2, "NFA_OPT_CHARS"); ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ NFA_MOPEN:
	ಸ್ಥಿತಿ NFA_MOPEN1:
	ಸ್ಥಿತಿ NFA_MOPEN2:
	ಸ್ಥಿತಿ NFA_MOPEN3:
	ಸ್ಥಿತಿ NFA_MOPEN4:
	ಸ್ಥಿತಿ NFA_MOPEN5:
	ಸ್ಥಿತಿ NFA_MOPEN6:
	ಸ್ಥಿತಿ NFA_MOPEN7:
	ಸ್ಥಿತಿ NFA_MOPEN8:
	ಸ್ಥಿತಿ NFA_MOPEN9:
	    STRCPY(ಸಂಕೇತ_2, "NFA_MOPEN(x)");
	    ಸಂಕೇತ_2[10] = c - NFA_MOPEN + '0';
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_MCLOSE:
	ಸ್ಥಿತಿ NFA_MCLOSE1:
	ಸ್ಥಿತಿ NFA_MCLOSE2:
	ಸ್ಥಿತಿ NFA_MCLOSE3:
	ಸ್ಥಿತಿ NFA_MCLOSE4:
	ಸ್ಥಿತಿ NFA_MCLOSE5:
	ಸ್ಥಿತಿ NFA_MCLOSE6:
	ಸ್ಥಿತಿ NFA_MCLOSE7:
	ಸ್ಥಿತಿ NFA_MCLOSE8:
	ಸ್ಥಿತಿ NFA_MCLOSE9:
	    STRCPY(ಸಂಕೇತ_2, "NFA_MCLOSE(x)");
	    ಸಂಕೇತ_2[11] = c - NFA_MCLOSE + '0';
	    ನಿಲ್ಲಿಸು;
# ifdef FEAT_SYN_HL
	ಸ್ಥಿತಿ NFA_ZOPEN:
	ಸ್ಥಿತಿ NFA_ZOPEN1:
	ಸ್ಥಿತಿ NFA_ZOPEN2:
	ಸ್ಥಿತಿ NFA_ZOPEN3:
	ಸ್ಥಿತಿ NFA_ZOPEN4:
	ಸ್ಥಿತಿ NFA_ZOPEN5:
	ಸ್ಥಿತಿ NFA_ZOPEN6:
	ಸ್ಥಿತಿ NFA_ZOPEN7:
	ಸ್ಥಿತಿ NFA_ZOPEN8:
	ಸ್ಥಿತಿ NFA_ZOPEN9:
	    STRCPY(ಸಂಕೇತ_2, "NFA_ZOPEN(x)");
	    ಸಂಕೇತ_2[10] = c - NFA_ZOPEN + '0';
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ZCLOSE:
	ಸ್ಥಿತಿ NFA_ZCLOSE1:
	ಸ್ಥಿತಿ NFA_ZCLOSE2:
	ಸ್ಥಿತಿ NFA_ZCLOSE3:
	ಸ್ಥಿತಿ NFA_ZCLOSE4:
	ಸ್ಥಿತಿ NFA_ZCLOSE5:
	ಸ್ಥಿತಿ NFA_ZCLOSE6:
	ಸ್ಥಿತಿ NFA_ZCLOSE7:
	ಸ್ಥಿತಿ NFA_ZCLOSE8:
	ಸ್ಥಿತಿ NFA_ZCLOSE9:
	    STRCPY(ಸಂಕೇತ_2, "NFA_ZCLOSE(x)");
	    ಸಂಕೇತ_2[11] = c - NFA_ZCLOSE + '0';
	    ನಿಲ್ಲಿಸು;
# endif
	ಸ್ಥಿತಿ NFA_EOL:		STRCPY(ಸಂಕೇತ_2, "NFA_EOL "); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_BOL:		STRCPY(ಸಂಕೇತ_2, "NFA_BOL "); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_EOW:		STRCPY(ಸಂಕೇತ_2, "NFA_EOW "); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_BOW:		STRCPY(ಸಂಕೇತ_2, "NFA_BOW "); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_EOF:		STRCPY(ಸಂಕೇತ_2, "NFA_EOF "); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_BOF:		STRCPY(ಸಂಕೇತ_2, "NFA_BOF "); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_LNUM:		STRCPY(ಸಂಕೇತ_2, "NFA_LNUM "); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_LNUM_GT:	STRCPY(ಸಂಕೇತ_2, "NFA_LNUM_GT "); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_LNUM_LT:	STRCPY(ಸಂಕೇತ_2, "NFA_LNUM_LT "); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ಲಂಬಸಾಲು:		STRCPY(ಸಂಕೇತ_2, "NFA_COL "); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ಲಂಬಸಾಲು_GT:	STRCPY(ಸಂಕೇತ_2, "NFA_COL_GT "); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ಲಂಬಸಾಲು_LT:	STRCPY(ಸಂಕೇತ_2, "NFA_COL_LT "); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_VCOL:		STRCPY(ಸಂಕೇತ_2, "NFA_VCOL "); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_VCOL_GT:	STRCPY(ಸಂಕೇತ_2, "NFA_VCOL_GT "); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_VCOL_LT:	STRCPY(ಸಂಕೇತ_2, "NFA_VCOL_LT "); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_MARK:		STRCPY(ಸಂಕೇತ_2, "NFA_MARK "); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_MARK_GT:	STRCPY(ಸಂಕೇತ_2, "NFA_MARK_GT "); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_MARK_LT:	STRCPY(ಸಂಕೇತ_2, "NFA_MARK_LT "); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CURSOR:	STRCPY(ಸಂಕೇತ_2, "NFA_CURSOR "); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_VISUAL:	STRCPY(ಸಂಕೇತ_2, "NFA_VISUAL "); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ANY_COMPOSING:	STRCPY(ಸಂಕೇತ_2, "NFA_ANY_COMPOSING "); ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ NFA_ನಕ್ಷತ್ರ:		STRCPY(ಸಂಕೇತ_2, "NFA_STAR "); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ನಕ್ಷತ್ರ_NONGREEDY: STRCPY(ಸಂಕೇತ_2, "NFA_STAR_NONGREEDY "); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_QUEST:		STRCPY(ಸಂಕೇತ_2, "NFA_QUEST"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_QUEST_NONGREEDY: STRCPY(ಸಂಕೇತ_2, "NFA_QUEST_NON_GREEDY"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ಖಾಲಿಗಳು:		STRCPY(ಸಂಕೇತ_2, "NFA_EMPTY"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_OR:		STRCPY(ಸಂಕೇತ_2, "NFA_OR"); ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ NFA_ಆರಂಭ_COLL:	STRCPY(ಸಂಕೇತ_2, "NFA_START_COLL"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ಕೊನೆ_COLL:	STRCPY(ಸಂಕೇತ_2, "NFA_END_COLL"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ಆರಂಭ_NEG_COLL: STRCPY(ಸಂಕೇತ_2, "NFA_START_NEG_COLL"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ಕೊನೆ_NEG_COLL:	STRCPY(ಸಂಕೇತ_2, "NFA_END_NEG_COLL"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_RANGE:		STRCPY(ಸಂಕೇತ_2, "NFA_RANGE"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_RANGE_MIN:	STRCPY(ಸಂಕೇತ_2, "NFA_RANGE_MIN"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_RANGE_MAX:	STRCPY(ಸಂಕೇತ_2, "NFA_RANGE_MAX"); ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ NFA_CLASS_ALNUM:	STRCPY(ಸಂಕೇತ_2, "NFA_CLASS_ALNUM"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_ALPHA:	STRCPY(ಸಂಕೇತ_2, "NFA_CLASS_ALPHA"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_BLANK:	STRCPY(ಸಂಕೇತ_2, "NFA_CLASS_BLANK"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_CNTRL:	STRCPY(ಸಂಕೇತ_2, "NFA_CLASS_CNTRL"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_DIGIT:	STRCPY(ಸಂಕೇತ_2, "NFA_CLASS_DIGIT"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_ನಕ್ಷೆ:	STRCPY(ಸಂಕೇತ_2, "NFA_CLASS_GRAPH"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_ಅಧೋ:	STRCPY(ಸಂಕೇತ_2, "NFA_CLASS_LOWER"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_PRINT:	STRCPY(ಸಂಕೇತ_2, "NFA_CLASS_PRINT"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_PUNCT:	STRCPY(ಸಂಕೇತ_2, "NFA_CLASS_PUNCT"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_SPACE:	STRCPY(ಸಂಕೇತ_2, "NFA_CLASS_SPACE"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_ಊರ್ಧ್ವ:	STRCPY(ಸಂಕೇತ_2, "NFA_CLASS_UPPER"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_XDIGIT:	STRCPY(ಸಂಕೇತ_2, "NFA_CLASS_XDIGIT"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_TAB:	STRCPY(ಸಂಕೇತ_2, "NFA_CLASS_TAB"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_RETURN:	STRCPY(ಸಂಕೇತ_2, "NFA_CLASS_RETURN"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_BACKSPACE:   STRCPY(ಸಂಕೇತ_2, "NFA_CLASS_BACKSPACE"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_ESCAPE:	STRCPY(ಸಂಕೇತ_2, "NFA_CLASS_ESCAPE"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_IDENT:	STRCPY(ಸಂಕೇತ_2, "NFA_CLASS_IDENT"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_KEYWORD:	STRCPY(ಸಂಕೇತ_2, "NFA_CLASS_KEYWORD"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_FNAME:	STRCPY(ಸಂಕೇತ_2, "NFA_CLASS_FNAME"); ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ NFA_ANY:	STRCPY(ಸಂಕೇತ_2, "NFA_ANY"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_IDENT:	STRCPY(ಸಂಕೇತ_2, "NFA_IDENT"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_SIDENT:STRCPY(ಸಂಕೇತ_2, "NFA_SIDENT"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_KWORD:	STRCPY(ಸಂಕೇತ_2, "NFA_KWORD"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_SKWORD:STRCPY(ಸಂಕೇತ_2, "NFA_SKWORD"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_FNAME:	STRCPY(ಸಂಕೇತ_2, "NFA_FNAME"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_SFNAME:STRCPY(ಸಂಕೇತ_2, "NFA_SFNAME"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_PRINT:	STRCPY(ಸಂಕೇತ_2, "NFA_PRINT"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_SPRINT:STRCPY(ಸಂಕೇತ_2, "NFA_SPRINT"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_WHITE:	STRCPY(ಸಂಕೇತ_2, "NFA_WHITE"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_NWHITE:STRCPY(ಸಂಕೇತ_2, "NFA_NWHITE"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_DIGIT:	STRCPY(ಸಂಕೇತ_2, "NFA_DIGIT"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_NDIGIT:STRCPY(ಸಂಕೇತ_2, "NFA_NDIGIT"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_HEX:	STRCPY(ಸಂಕೇತ_2, "NFA_HEX"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_NHEX:	STRCPY(ಸಂಕೇತ_2, "NFA_NHEX"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_OCTAL:	STRCPY(ಸಂಕೇತ_2, "NFA_OCTAL"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_NOCTAL:STRCPY(ಸಂಕೇತ_2, "NFA_NOCTAL"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ಶಬ್ದ:	STRCPY(ಸಂಕೇತ_2, "NFA_WORD"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_NWORD:	STRCPY(ಸಂಕೇತ_2, "NFA_NWORD"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ತಲೆ:	STRCPY(ಸಂಕೇತ_2, "NFA_HEAD"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_NHEAD:	STRCPY(ಸಂಕೇತ_2, "NFA_NHEAD"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ALPHA:	STRCPY(ಸಂಕೇತ_2, "NFA_ALPHA"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_NALPHA:STRCPY(ಸಂಕೇತ_2, "NFA_NALPHA"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ಅಧೋ:	STRCPY(ಸಂಕೇತ_2, "NFA_LOWER"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_NLOWER:STRCPY(ಸಂಕೇತ_2, "NFA_NLOWER"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ಊರ್ಧ್ವ:	STRCPY(ಸಂಕೇತ_2, "NFA_UPPER"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_NUPPER:STRCPY(ಸಂಕೇತ_2, "NFA_NUPPER"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ಅಧೋ_IC:  STRCPY(ಸಂಕೇತ_2, "NFA_LOWER_IC"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_NLOWER_IC: STRCPY(ಸಂಕೇತ_2, "NFA_NLOWER_IC"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_ಊರ್ಧ್ವ_IC:  STRCPY(ಸಂಕೇತ_2, "NFA_UPPER_IC"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_NUPPER_IC: STRCPY(ಸಂಕೇತ_2, "NFA_NUPPER_IC"); ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
	    STRCPY(ಸಂಕೇತ_2, "CHAR(x)");
	    ಸಂಕೇತ_2[5] = c;
    }

    ಆದರೆ (addnl == TRUE)
	STRCAT(ಸಂಕೇತ_2, " + NEWLINE ");

}

# ifdef ENABLE_LOG
ಸ್ಥಿರ FILE *log_fd;
ಸ್ಥಿರ char_u e_log_open_failed[] = N_("Could not open temporary log file for writing, displaying on stderr... ");

/*
 * Print the postfix notation of the current regexp.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
nfa_postfix_dump(char_u *ಅಭಿವ್ಯಕ್ತಿ, ಪೂರ್ಣಾಂಕ retval)
{
    ಪೂರ್ಣಾಂಕ *p;
    FILE *f;

    f = fopen(NFA_REGEXP_DUMP_LOG, "a");
    ಆದರೆ (f == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    fprintf(f, "\n-------------------------\n");
    ಆದರೆ (retval == FAIL)
	fprintf(f, ">>> NFA engine failed... \n");
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (retval == OK)
	fprintf(f, ">>> NFA engine succeeded !\n");
    fprintf(f, "Regexp: \"%s\"\nPostfix notation (char): \"", ಅಭಿವ್ಯಕ್ತಿ);
    ಗಾಗಿ (p = post_ಆರಂಭ; *p && p < post_ptr; p++)
    {
	nfa_ಗಣ_ಸಂಕೇತ(*p);
	fprintf(f, "%s, ", ಸಂಕೇತ_2);
    }
    fprintf(f, "\"\nPostfix notation (int): ");
    ಗಾಗಿ (p = post_ಆರಂಭ; *p && p < post_ptr; p++)
	fprintf(f, "%d ", *p);
    fprintf(f, "\n\n");
    fclose(f);
}

/*
 * Print the NFA starting with a root node "state".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
nfa_print_state(FILE *debugf, nfa_state_T *state)
{
    garray_T indent;

    ga_init2(&indent, 1, 64);
    ga_append(&indent, '\0');
    nfa_print_state2(debugf, state, &indent);
    ga_clear(&indent);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent)
{
    char_u  *p;

    ಆದರೆ (state == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    fprintf(debugf, "(%2d)", ನಿರಪೇಕ್ಷ(state->id));

    // Output indent
    p = (char_u *)indent->ga_ದತ್ತಾಂಶ;
    ಆದರೆ (indent->ga_len >= 3)
    {
	ಪೂರ್ಣಾಂಕ	ಕೊನೆಯ = indent->ga_len - 3;
	char_u	save[2];

	STRNCPY(save, &p[ಕೊನೆಯ], 2);
	ಸ್ಮರಣೆ_ನಕಲು(&p[ಕೊನೆಯ], "+-", 2);
	fprintf(debugf, " %s", p);
	STRNCPY(&p[ಕೊನೆಯ], save, 2);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	fprintf(debugf, " %s", p);

    nfa_ಗಣ_ಸಂಕೇತ(state->c);
    fprintf(debugf, "%s (%d) (id=%d) val=%d\n",
		 ಸಂಕೇತ_2,
		 state->c,
		 ನಿರಪೇಕ್ಷ(state->id),
		 state->val);
    ಆದರೆ (state->id < 0)
	ಹಿಂತಿರುಗಿ;

    state->id = ನಿರಪೇಕ್ಷ(state->id) * -1;

    // grow indent for state->out
    indent->ga_len -= 1;
    ಆದರೆ (state->out1)
	GA_CONCAT_LITERAL(indent, "| ");
    ಇಲ್ಲದಿದ್ದರೆ
	GA_CONCAT_LITERAL(indent, "  ");
    ga_append(indent, NUL);

    nfa_print_state2(debugf, state->ಹೊರಹೋಗು, indent);

    // replace last part of indent for state->out1
    indent->ga_len -= 3;
    GA_CONCAT_LITERAL(indent, "  ");
    ga_append(indent, NUL);

    nfa_print_state2(debugf, state->out1, indent);

    // shrink indent
    indent->ga_len -= 3;
    ga_append(indent, NUL);
}

/*
 * Print the NFA state machine.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
nfa_dump(nfa_regprog_T *prog)
{
    FILE *debugf = fopen(NFA_REGEXP_DUMP_LOG, "a");

    ಆದರೆ (debugf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    nfa_print_state(debugf, prog->ಆರಂಭ);

    ಆದರೆ (prog->reganch)
	fprintf(debugf, "reganch: %d\n", prog->reganch);
    ಆದರೆ (prog->regstart != NUL)
	fprintf(debugf, "regstart: %c (decimal: %d)\n",
		prog->regstart, prog->regstart);
    ಆದರೆ (prog->ಹೊಂದಿಕೆ_ಪಠ್ಯ != ಖಾಲಿ)
	fprintf(debugf, "match_text: \"%s\"\n", prog->ಹೊಂದಿಕೆ_ಪಠ್ಯ);

    fclose(debugf);
}
# endif	    // ENABLE_LOG
#ಆದರೆ_ಅಂತ್ಯ	    // DEBUG

/*
 * Parse r.e. @expr and convert it into postfix form.
 * Return the postfix string on success, NULL otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ *
re2post(ಶೂನ್ಯ)
{
    ಆದರೆ (nfa_reg(REG_NOPAREN) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    EMIT(NFA_MOPEN);
    ಹಿಂತಿರುಗಿ post_ಆರಂಭ;
}

// NB. Some of the code below is inspired by Russ's.

/*
 * Represents an NFA state plus zero or one or two arrows exiting.
 * if c == MATCH, no arrows out; matching state.
 * If c == SPLIT, unlabeled arrows to out and out1 (if != NULL).
 * If c < 256, labeled arrow with character c to out.
 */

ಸ್ಥಿರ nfa_state_T	*state_ptr; // points to nfa_prog->state

/*
 * Allocate and initialize nfa_state_T.
 */
    ಸ್ಥಿರ nfa_state_T *
alloc_state(ಪೂರ್ಣಾಂಕ c, nfa_state_T *ಹೊರಹೋಗು, nfa_state_T *out1)
{
    nfa_state_T *s;

    ಆದರೆ (istate >= nstate)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    s = &state_ptr[istate++];

    s->c    = c;
    s->ಹೊರಹೋಗು  = ಹೊರಹೋಗು;
    s->out1 = out1;
    s->val  = 0;

    s->id   = istate;
    s->lastlist[0] = 0;
    s->lastlist[1] = 0;

    ಹಿಂತಿರುಗಿ s;
}

/*
 * A partially built NFA without the matching state filled in.
 * Frag_T.start points at the start state.
 * Frag_T.out is a list of places that need to be set to the
 * next state for this fragment.
 */

// Since the out pointers in the list are always
// uninitialized, we use the pointers themselves
// as storage for the Ptrlists.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಒಕ್ಕೂಟ Ptrlist Ptrlist;
ಒಕ್ಕೂಟ Ptrlist
{
    Ptrlist	*ಮುಂದಿನ;
    nfa_state_T	*s;
};

ರಚನೆ Frag
{
    nfa_state_T *ಆರಂಭ;
    Ptrlist	*ಹೊರಹೋಗು;
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Frag Frag_T;

// Reused across compilations by post2nfa(); freed in free_regexp_stuff().
ಸ್ಥಿರ Frag_T *nfa_ರಾಶಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ nfa_ರಾಶಿ_len;

/*
 * Initialize a Frag_T struct and return it.
 */
    ಸ್ಥಿರ Frag_T
frag(nfa_state_T *ಆರಂಭ, Ptrlist *ಹೊರಹೋಗು)
{
    Frag_T n;

    n.ಆರಂಭ = ಆರಂಭ;
    n.ಹೊರಹೋಗು = ಹೊರಹೋಗು;
    ಹಿಂತಿರುಗಿ n;
}

/*
 * Create singleton list containing just outp.
 */
    ಸ್ಥಿರ Ptrlist *
list1(
    nfa_state_T	**outp)
{
    Ptrlist *l;

    l = (Ptrlist *)outp;
    l->ಮುಂದಿನ = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ l;
}

/*
 * Patch the list of states at out to point to start.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
patch(Ptrlist *l, nfa_state_T *s)
{
    Ptrlist *ಮುಂದಿನ;

    ಗಾಗಿ (; l; l = ಮುಂದಿನ)
    {
	ಮುಂದಿನ = l->ಮುಂದಿನ;
	l->s = s;
    }
}


/*
 * Join the two lists l1 and l2, returning the combination.
 */
    ಸ್ಥಿರ Ptrlist *
append(Ptrlist *l1, Ptrlist *l2)
{
    Ptrlist *oldl1;

    oldl1 = l1;
    ಆಗ (l1->ಮುಂದಿನ)
	l1 = l1->ಮುಂದಿನ;
    l1->ಮುಂದಿನ = l2;
    ಹಿಂತಿರುಗಿ oldl1;
}

/*
 * Stack used for transforming postfix form into NFA.
 */
ಸ್ಥಿರ Frag_T ಖಾಲಿಗಳು_2;

    ಸ್ಥಿರ ಶೂನ್ಯ
st_error(ಪೂರ್ಣಾಂಕ *postfix ಬಳಸದ, ಪೂರ್ಣಾಂಕ *ಕೊನೆ ಬಳಸದ, ಪೂರ್ಣಾಂಕ *p ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NFA_REGEXP_ERROR_LOG
    FILE *df;
    ಪೂರ್ಣಾಂಕ *p2;

    df = fopen(NFA_REGEXP_ERROR_LOG, "a");
    ಆದರೆ (df)
    {
	fprintf(df, "Error popping the stack!\n");
# ifdef DEBUG
	fprintf(df, "Current regexp is \"%s\"\n", nfa_regengine.ಅಭಿವ್ಯಕ್ತಿ);
# endif
	fprintf(df, "Postfix form is: ");
# ifdef DEBUG
	ಗಾಗಿ (p2 = postfix; p2 < ಕೊನೆ; p2++)
	{
	    nfa_ಗಣ_ಸಂಕೇತ(*p2);
	    fprintf(df, "%s, ", ಸಂಕೇತ_2);
	}
	nfa_ಗಣ_ಸಂಕೇತ(*p);
	fprintf(df, "\nCurrent position is: ");
	ಗಾಗಿ (p2 = postfix; p2 <= p; p2 ++)
	{
	    nfa_ಗಣ_ಸಂಕೇತ(*p2);
	    fprintf(df, "%s, ", ಸಂಕೇತ_2);
	}
# ಇಲ್ಲದಿದ್ದರೆ
	ಗಾಗಿ (p2 = postfix; p2 < ಕೊನೆ; p2++)
	    fprintf(df, "%d, ", *p2);
	fprintf(df, "\nCurrent position is: ");
	ಗಾಗಿ (p2 = postfix; p2 <= p; p2 ++)
	    fprintf(df, "%d, ", *p2);
# endif
	fprintf(df, "\n--------------------------\n");
	fclose(df);
    }
#ಆದರೆ_ಅಂತ್ಯ
    emsg(_(e_nfa_regexp_could_not_ತೆಗೆ_ರಾಶಿ));
}

/*
 * Push an item onto the stack.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
st_ತಳ್ಳು(Frag_T s, Frag_T **p, Frag_T *ರಾಶಿ_ಕೊನೆ)
{
    Frag_T *stackp = *p;

    ಆದರೆ (stackp >= ರಾಶಿ_ಕೊನೆ)
	ಹಿಂತಿರುಗಿ;
    *stackp = s;
    *p = *p + 1;
}

/*
 * Pop an item from the stack.
 */
    ಸ್ಥಿರ Frag_T
st_ತೆಗೆ(Frag_T **p, Frag_T *ರಾಶಿ)
{
    Frag_T *stackp;

    *p = *p - 1;
    stackp = *p;
    ಆದರೆ (stackp < ರಾಶಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿಗಳು_2;
    ಹಿಂತಿರುಗಿ **p;
}

/*
 * Estimate the maximum byte length of anything matching "state".
 * When unknown or unlimited return -1.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
nfa_max_ಅಗಲ(nfa_state_T *startstate, ಪೂರ್ಣಾಂಕ ಆಳ)
{
    ಪೂರ್ಣಾಂಕ		    l, r;
    nfa_state_T	    *state = startstate;
    ಪೂರ್ಣಾಂಕ		    len = 0;

    // detect looping in a NFA_SPLIT
    ಆದರೆ (ಆಳ > 4)
	ಹಿಂತಿರುಗಿ -1;

    ಆಗ (state != ಖಾಲಿ)
    {
	ಆಯ್ಕೆ (state->c)
	{
	    ಸ್ಥಿತಿ NFA_ಕೊನೆ_INVISIBLE:
	    ಸ್ಥಿತಿ NFA_ಕೊನೆ_INVISIBLE_NEG:
		// the end, return what we have
		ಹಿಂತಿರುಗಿ len;

	    ಸ್ಥಿತಿ NFA_ವಿಭಜಿಸು:
		// two alternatives, use the maximum
		l = nfa_max_ಅಗಲ(state->ಹೊರಹೋಗು, ಆಳ + 1);
		r = nfa_max_ಅಗಲ(state->out1, ಆಳ + 1);
		ಆದರೆ (l < 0 || r < 0)
		    ಹಿಂತಿರುಗಿ -1;
		ಹಿಂತಿರುಗಿ len + (l > r ? l : r);

	    ಸ್ಥಿತಿ NFA_ANY:
	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_COLL:
	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_NEG_COLL:
		// matches some character, including composing chars
		ಆದರೆ (enc_utf8)
		    len += MB_MAXBYTES;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte)
		    len += 2;
		ಇಲ್ಲದಿದ್ದರೆ
		    ++len;
		ಆದರೆ (state->c != NFA_ANY)
		{
		    // Skip over the compiled collection.
		    // malformed NFAs must not crash width estimation.
		    ಆದರೆ (state->out1 == ಖಾಲಿ || state->out1->ಹೊರಹೋಗು == ಖಾಲಿ)
			ಹಿಂತಿರುಗಿ -1;
		    state = state->out1->ಹೊರಹೋಗು;
		    ಮುಂದುವರಿ;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_DIGIT:
	    ಸ್ಥಿತಿ NFA_WHITE:
	    ಸ್ಥಿತಿ NFA_HEX:
	    ಸ್ಥಿತಿ NFA_OCTAL:
		// ascii
		++len;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_IDENT:
	    ಸ್ಥಿತಿ NFA_SIDENT:
	    ಸ್ಥಿತಿ NFA_KWORD:
	    ಸ್ಥಿತಿ NFA_SKWORD:
	    ಸ್ಥಿತಿ NFA_FNAME:
	    ಸ್ಥಿತಿ NFA_SFNAME:
	    ಸ್ಥಿತಿ NFA_PRINT:
	    ಸ್ಥಿತಿ NFA_SPRINT:
	    ಸ್ಥಿತಿ NFA_NWHITE:
	    ಸ್ಥಿತಿ NFA_NDIGIT:
	    ಸ್ಥಿತಿ NFA_NHEX:
	    ಸ್ಥಿತಿ NFA_NOCTAL:
	    ಸ್ಥಿತಿ NFA_ಶಬ್ದ:
	    ಸ್ಥಿತಿ NFA_NWORD:
	    ಸ್ಥಿತಿ NFA_ತಲೆ:
	    ಸ್ಥಿತಿ NFA_NHEAD:
	    ಸ್ಥಿತಿ NFA_ALPHA:
	    ಸ್ಥಿತಿ NFA_NALPHA:
	    ಸ್ಥಿತಿ NFA_ಅಧೋ:
	    ಸ್ಥಿತಿ NFA_NLOWER:
	    ಸ್ಥಿತಿ NFA_ಊರ್ಧ್ವ:
	    ಸ್ಥಿತಿ NFA_NUPPER:
	    ಸ್ಥಿತಿ NFA_ಅಧೋ_IC:
	    ಸ್ಥಿತಿ NFA_NLOWER_IC:
	    ಸ್ಥಿತಿ NFA_ಊರ್ಧ್ವ_IC:
	    ಸ್ಥಿತಿ NFA_NUPPER_IC:
	    ಸ್ಥಿತಿ NFA_ANY_COMPOSING:
		// possibly non-ascii
		ಆದರೆ (has_mbyte)
		    len += 3;
		ಇಲ್ಲದಿದ್ದರೆ
		    ++len;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE:
	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_NEG:
	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_BEFORE:
	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_BEFORE_NEG:
		// zero-width, out1 points to the END state
		state = state->out1->ಹೊರಹೋಗು;
		ಮುಂದುವರಿ;

	    ಸ್ಥಿತಿ NFA_BACKREF1:
	    ಸ್ಥಿತಿ NFA_BACKREF2:
	    ಸ್ಥಿತಿ NFA_BACKREF3:
	    ಸ್ಥಿತಿ NFA_BACKREF4:
	    ಸ್ಥಿತಿ NFA_BACKREF5:
	    ಸ್ಥಿತಿ NFA_BACKREF6:
	    ಸ್ಥಿತಿ NFA_BACKREF7:
	    ಸ್ಥಿತಿ NFA_BACKREF8:
	    ಸ್ಥಿತಿ NFA_BACKREF9:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	    ಸ್ಥಿತಿ NFA_ZREF1:
	    ಸ್ಥಿತಿ NFA_ZREF2:
	    ಸ್ಥಿತಿ NFA_ZREF3:
	    ಸ್ಥಿತಿ NFA_ZREF4:
	    ಸ್ಥಿತಿ NFA_ZREF5:
	    ಸ್ಥಿತಿ NFA_ZREF6:
	    ಸ್ಥಿತಿ NFA_ZREF7:
	    ಸ್ಥಿತಿ NFA_ZREF8:
	    ಸ್ಥಿತಿ NFA_ZREF9:
#ಆದರೆ_ಅಂತ್ಯ
	    ಸ್ಥಿತಿ NFA_NEWL:
	    ಸ್ಥಿತಿ NFA_SKIP:
		// unknown width
		ಹಿಂತಿರುಗಿ -1;

	    ಸ್ಥಿತಿ NFA_BOL:
	    ಸ್ಥಿತಿ NFA_EOL:
	    ಸ್ಥಿತಿ NFA_BOF:
	    ಸ್ಥಿತಿ NFA_EOF:
	    ಸ್ಥಿತಿ NFA_BOW:
	    ಸ್ಥಿತಿ NFA_EOW:
	    ಸ್ಥಿತಿ NFA_MOPEN:
	    ಸ್ಥಿತಿ NFA_MOPEN1:
	    ಸ್ಥಿತಿ NFA_MOPEN2:
	    ಸ್ಥಿತಿ NFA_MOPEN3:
	    ಸ್ಥಿತಿ NFA_MOPEN4:
	    ಸ್ಥಿತಿ NFA_MOPEN5:
	    ಸ್ಥಿತಿ NFA_MOPEN6:
	    ಸ್ಥಿತಿ NFA_MOPEN7:
	    ಸ್ಥಿತಿ NFA_MOPEN8:
	    ಸ್ಥಿತಿ NFA_MOPEN9:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	    ಸ್ಥಿತಿ NFA_ZOPEN:
	    ಸ್ಥಿತಿ NFA_ZOPEN1:
	    ಸ್ಥಿತಿ NFA_ZOPEN2:
	    ಸ್ಥಿತಿ NFA_ZOPEN3:
	    ಸ್ಥಿತಿ NFA_ZOPEN4:
	    ಸ್ಥಿತಿ NFA_ZOPEN5:
	    ಸ್ಥಿತಿ NFA_ZOPEN6:
	    ಸ್ಥಿತಿ NFA_ZOPEN7:
	    ಸ್ಥಿತಿ NFA_ZOPEN8:
	    ಸ್ಥಿತಿ NFA_ZOPEN9:
	    ಸ್ಥಿತಿ NFA_ZCLOSE:
	    ಸ್ಥಿತಿ NFA_ZCLOSE1:
	    ಸ್ಥಿತಿ NFA_ZCLOSE2:
	    ಸ್ಥಿತಿ NFA_ZCLOSE3:
	    ಸ್ಥಿತಿ NFA_ZCLOSE4:
	    ಸ್ಥಿತಿ NFA_ZCLOSE5:
	    ಸ್ಥಿತಿ NFA_ZCLOSE6:
	    ಸ್ಥಿತಿ NFA_ZCLOSE7:
	    ಸ್ಥಿತಿ NFA_ZCLOSE8:
	    ಸ್ಥಿತಿ NFA_ZCLOSE9:
#ಆದರೆ_ಅಂತ್ಯ
	    ಸ್ಥಿತಿ NFA_MCLOSE:
	    ಸ್ಥಿತಿ NFA_MCLOSE1:
	    ಸ್ಥಿತಿ NFA_MCLOSE2:
	    ಸ್ಥಿತಿ NFA_MCLOSE3:
	    ಸ್ಥಿತಿ NFA_MCLOSE4:
	    ಸ್ಥಿತಿ NFA_MCLOSE5:
	    ಸ್ಥಿತಿ NFA_MCLOSE6:
	    ಸ್ಥಿತಿ NFA_MCLOSE7:
	    ಸ್ಥಿತಿ NFA_MCLOSE8:
	    ಸ್ಥಿತಿ NFA_MCLOSE9:
	    ಸ್ಥಿತಿ NFA_NOPEN:
	    ಸ್ಥಿತಿ NFA_NCLOSE:

	    ಸ್ಥಿತಿ NFA_LNUM_GT:
	    ಸ್ಥಿತಿ NFA_LNUM_LT:
	    ಸ್ಥಿತಿ NFA_ಲಂಬಸಾಲು_GT:
	    ಸ್ಥಿತಿ NFA_ಲಂಬಸಾಲು_LT:
	    ಸ್ಥಿತಿ NFA_VCOL_GT:
	    ಸ್ಥಿತಿ NFA_VCOL_LT:
	    ಸ್ಥಿತಿ NFA_MARK_GT:
	    ಸ್ಥಿತಿ NFA_MARK_LT:
	    ಸ್ಥಿತಿ NFA_VISUAL:
	    ಸ್ಥಿತಿ NFA_LNUM:
	    ಸ್ಥಿತಿ NFA_CURSOR:
	    ಸ್ಥಿತಿ NFA_ಲಂಬಸಾಲು:
	    ಸ್ಥಿತಿ NFA_VCOL:
	    ಸ್ಥಿತಿ NFA_MARK:

	    ಸ್ಥಿತಿ NFA_ZSTART:
	    ಸ್ಥಿತಿ NFA_ZEND:
	    ಸ್ಥಿತಿ NFA_OPT_CHARS:
	    ಸ್ಥಿತಿ NFA_ಖಾಲಿಗಳು:
	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_ನಮೂನೆ:
	    ಸ್ಥಿತಿ NFA_ಕೊನೆ_ನಮೂನೆ:
	    ಸ್ಥಿತಿ NFA_COMPOSING:
	    ಸ್ಥಿತಿ NFA_ಕೊನೆ_COMPOSING:
		// zero-width
		ನಿಲ್ಲಿಸು;

	    ಪೂರ್ವನಿಯೋಜಿತ:
		ಆದರೆ (state->c < 0)
		    // don't know what this is
		    ಹಿಂತಿರುಗಿ -1;
		// normal character
		len += MB_CHAR2LEN(state->c);
		ನಿಲ್ಲಿಸು;
	}

	// normal way to continue
	state = state->ಹೊರಹೋಗು;
    }

    // unrecognized, "cannot happen"
    ಹಿಂತಿರುಗಿ -1;
}

/*
 * Convert a postfix form into its equivalent NFA.
 * Return the NFA start state on success, NULL otherwise.
 */
    ಸ್ಥಿರ nfa_state_T *
post2nfa(ಪೂರ್ಣಾಂಕ *postfix, ಪೂರ್ಣಾಂಕ *ಕೊನೆ, ಪೂರ್ಣಾಂಕ nfa_calc_ಗಾತ್ರ)
{
    ಪೂರ್ಣಾಂಕ		*p;
    ಪೂರ್ಣಾಂಕ		mopen;
    ಪೂರ್ಣಾಂಕ		mclose;
    Frag_T	*ರಾಶಿ = ಖಾಲಿ;
    Frag_T	*stackp = ಖಾಲಿ;
    Frag_T	*ರಾಶಿ_ಕೊನೆ = ಖಾಲಿ;
    Frag_T	e1;
    Frag_T	e2;
    Frag_T	e;
    nfa_state_T	*s;
    nfa_state_T	*s1;
    nfa_state_T	*matchstate;
    nfa_state_T	*ret = ಖಾಲಿ;

    ಆದರೆ (postfix == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿಸು ತಳ್ಳು_2(s)	    st_ತಳ್ಳು((s), &stackp, ರಾಶಿ_ಕೊನೆ)
#ವ್ಯಾಖ್ಯಾನಿಸು ತೆಗೆ_2()	    st_ತೆಗೆ(&stackp, ರಾಶಿ);		\
		    ಆದರೆ (stackp < ರಾಶಿ)			\
		    {					\
			st_error(postfix, ಕೊನೆ, p);	\
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;			\
		    }

    ಆದರೆ (nfa_calc_ಗಾತ್ರ == FALSE)
    {
	// Reuse the fragment stack across compilations, growing when needed;
	// it is freed in free_regexp_stuff().
	ಆದರೆ (nfa_ರಾಶಿ == ಖಾಲಿ || nfa_ರಾಶಿ_len < nstate + 1)
	{
	    vim_free(nfa_ರಾಶಿ);
	    nfa_ರಾಶಿ = ALLOC_MULT(Frag_T, nstate + 1);
	    ಆದರೆ (nfa_ರಾಶಿ == ಖಾಲಿ)
	    {
		nfa_ರಾಶಿ_len = 0;
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    nfa_ರಾಶಿ_len = nstate + 1;
	}
	ರಾಶಿ = nfa_ರಾಶಿ;
	stackp = ರಾಶಿ;
	ರಾಶಿ_ಕೊನೆ = ರಾಶಿ + (nstate + 1);
    }

    ಗಾಗಿ (p = postfix; p < ಕೊನೆ; ++p)
    {
	ಆಯ್ಕೆ (*p)
	{
	ಸ್ಥಿತಿ NFA_CONCAT:
	    // Concatenation.
	    // Pay attention: this operator does not exist in the r.e. itself
	    // (it is implicit, really).  It is added when r.e. is translated
	    // to postfix form in re2post().
	    ಆದರೆ (nfa_calc_ಗಾತ್ರ == TRUE)
	    {
		// nstate += 0;
		ನಿಲ್ಲಿಸು;
	    }
	    e2 = ತೆಗೆ_2();
	    e1 = ತೆಗೆ_2();
	    patch(e1.ಹೊರಹೋಗು, e2.ಆರಂಭ);
	    ತಳ್ಳು_2(frag(e1.ಆರಂಭ, e2.ಹೊರಹೋಗು));
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ NFA_OR:
	    // Alternation
	    ಆದರೆ (nfa_calc_ಗಾತ್ರ == TRUE)
	    {
		nstate++;
		ನಿಲ್ಲಿಸು;
	    }
	    e2 = ತೆಗೆ_2();
	    e1 = ತೆಗೆ_2();
	    s = alloc_state(NFA_ವಿಭಜಿಸು, e1.ಆರಂಭ, e2.ಆರಂಭ);
	    ಆದರೆ (s == ಖಾಲಿ)
		ಹೋಗು theend;
	    ತಳ್ಳು_2(frag(s, append(e1.ಹೊರಹೋಗು, e2.ಹೊರಹೋಗು)));
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ NFA_ನಕ್ಷತ್ರ:
	    // Zero or more, prefer more
	    ಆದರೆ (nfa_calc_ಗಾತ್ರ == TRUE)
	    {
		nstate++;
		ನಿಲ್ಲಿಸು;
	    }
	    e = ತೆಗೆ_2();
	    s = alloc_state(NFA_ವಿಭಜಿಸು, e.ಆರಂಭ, ಖಾಲಿ);
	    ಆದರೆ (s == ಖಾಲಿ)
		ಹೋಗು theend;
	    patch(e.ಹೊರಹೋಗು, s);
	    ತಳ್ಳು_2(frag(s, list1(&s->out1)));
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ NFA_ನಕ್ಷತ್ರ_NONGREEDY:
	    // Zero or more, prefer zero
	    ಆದರೆ (nfa_calc_ಗಾತ್ರ == TRUE)
	    {
		nstate++;
		ನಿಲ್ಲಿಸು;
	    }
	    e = ತೆಗೆ_2();
	    s = alloc_state(NFA_ವಿಭಜಿಸು, ಖಾಲಿ, e.ಆರಂಭ);
	    ಆದರೆ (s == ಖಾಲಿ)
		ಹೋಗು theend;
	    patch(e.ಹೊರಹೋಗು, s);
	    ತಳ್ಳು_2(frag(s, list1(&s->ಹೊರಹೋಗು)));
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ NFA_QUEST:
	    // one or zero atoms=> greedy match
	    ಆದರೆ (nfa_calc_ಗಾತ್ರ == TRUE)
	    {
		nstate++;
		ನಿಲ್ಲಿಸು;
	    }
	    e = ತೆಗೆ_2();
	    s = alloc_state(NFA_ವಿಭಜಿಸು, e.ಆರಂಭ, ಖಾಲಿ);
	    ಆದರೆ (s == ಖಾಲಿ)
		ಹೋಗು theend;
	    ತಳ್ಳು_2(frag(s, append(e.ಹೊರಹೋಗು, list1(&s->out1))));
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ NFA_QUEST_NONGREEDY:
	    // zero or one atoms => non-greedy match
	    ಆದರೆ (nfa_calc_ಗಾತ್ರ == TRUE)
	    {
		nstate++;
		ನಿಲ್ಲಿಸು;
	    }
	    e = ತೆಗೆ_2();
	    s = alloc_state(NFA_ವಿಭಜಿಸು, ಖಾಲಿ, e.ಆರಂಭ);
	    ಆದರೆ (s == ಖಾಲಿ)
		ಹೋಗು theend;
	    ತಳ್ಳು_2(frag(s, append(e.ಹೊರಹೋಗು, list1(&s->ಹೊರಹೋಗು))));
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ NFA_ಕೊನೆ_COLL:
	ಸ್ಥಿತಿ NFA_ಕೊನೆ_NEG_COLL:
	    // On the stack is the sequence starting with NFA_START_COLL or
	    // NFA_START_NEG_COLL and all possible characters. Patch it to
	    // add the output to the start.
	    ಆದರೆ (nfa_calc_ಗಾತ್ರ == TRUE)
	    {
		nstate++;
		ನಿಲ್ಲಿಸು;
	    }
	    e = ತೆಗೆ_2();
	    s = alloc_state(NFA_ಕೊನೆ_COLL, ಖಾಲಿ, ಖಾಲಿ);
	    ಆದರೆ (s == ಖಾಲಿ)
		ಹೋಗು theend;
	    patch(e.ಹೊರಹೋಗು, s);
	    e.ಆರಂಭ->out1 = s;
	    ತಳ್ಳು_2(frag(e.ಆರಂಭ, list1(&s->ಹೊರಹೋಗು)));
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ NFA_RANGE:
	    // Before this are two characters, the low and high end of a
	    // range.  Turn them into two states with MIN and MAX.
	    ಆದರೆ (nfa_calc_ಗಾತ್ರ == TRUE)
	    {
		// nstate += 0;
		ನಿಲ್ಲಿಸು;
	    }
	    e2 = ತೆಗೆ_2();
	    e1 = ತೆಗೆ_2();
	    e2.ಆರಂಭ->val = e2.ಆರಂಭ->c;
	    e2.ಆರಂಭ->c = NFA_RANGE_MAX;
	    e1.ಆರಂಭ->val = e1.ಆರಂಭ->c;
	    e1.ಆರಂಭ->c = NFA_RANGE_MIN;
	    patch(e1.ಹೊರಹೋಗು, e2.ಆರಂಭ);
	    ತಳ್ಳು_2(frag(e1.ಆರಂಭ, e2.ಹೊರಹೋಗು));
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ NFA_ಖಾಲಿಗಳು:
	    // 0-length, used in a repetition with max/min count of 0
	    ಆದರೆ (nfa_calc_ಗಾತ್ರ == TRUE)
	    {
		nstate++;
		ನಿಲ್ಲಿಸು;
	    }
	    s = alloc_state(NFA_ಖಾಲಿಗಳು, ಖಾಲಿ, ಖಾಲಿ);
	    ಆದರೆ (s == ಖಾಲಿ)
		ಹೋಗು theend;
	    ತಳ್ಳು_2(frag(s, list1(&s->ಹೊರಹೋಗು)));
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ NFA_OPT_CHARS:
	  {
	    ಪೂರ್ಣಾಂಕ    n;

	    // \%[abc] implemented as:
	    //    NFA_SPLIT
	    //    +-CHAR(a)
	    //    | +-NFA_SPLIT
	    //    |   +-CHAR(b)
	    //    |   | +-NFA_SPLIT
	    //    |   |   +-CHAR(c)
	    //    |   |   | +-next
	    //    |   |   +- next
	    //    |   +- next
	    //    +- next
	    n = *++p; // get number of characters
	    ಆದರೆ (nfa_calc_ಗಾತ್ರ == TRUE)
	    {
		nstate += n;
		ನಿಲ್ಲಿಸು;
	    }
	    s = ಖಾಲಿ; // avoid compiler warning
	    e1.ಹೊರಹೋಗು = ಖಾಲಿ; // stores list with out1's
	    s1 = ಖಾಲಿ; // previous NFA_SPLIT to connect to
	    ಆಗ (n-- > 0)
	    {
		e = ತೆಗೆ_2(); // get character
		s = alloc_state(NFA_ವಿಭಜಿಸು, e.ಆರಂಭ, ಖಾಲಿ);
		ಆದರೆ (s == ಖಾಲಿ)
		    ಹೋಗು theend;
		ಆದರೆ (e1.ಹೊರಹೋಗು == ಖಾಲಿ)
		    e1 = e;
		patch(e.ಹೊರಹೋಗು, s1);
		append(e1.ಹೊರಹೋಗು, list1(&s->out1));
		s1 = s;
	    }
	    ತಳ್ಳು_2(frag(s, e1.ಹೊರಹೋಗು));
	    ನಿಲ್ಲಿಸು;
	  }

	ಸ್ಥಿತಿ NFA_PREV_ATOM_NO_ಅಗಲ:
	ಸ್ಥಿತಿ NFA_PREV_ATOM_NO_ಅಗಲ_NEG:
	ಸ್ಥಿತಿ NFA_PREV_ATOM_JUST_BEFORE:
	ಸ್ಥಿತಿ NFA_PREV_ATOM_JUST_BEFORE_NEG:
	ಸ್ಥಿತಿ NFA_PREV_ATOM_LIKE_ನಮೂನೆ:
	  {
	    ಪೂರ್ಣಾಂಕ before = (*p == NFA_PREV_ATOM_JUST_BEFORE
				      || *p == NFA_PREV_ATOM_JUST_BEFORE_NEG);
	    ಪೂರ್ಣಾಂಕ ನಮೂನೆ = (*p == NFA_PREV_ATOM_LIKE_ನಮೂನೆ);
	    ಪೂರ್ಣಾಂಕ ಆರಂಭ_state;
	    ಪೂರ್ಣಾಂಕ ಕೊನೆ_state;
	    ಪೂರ್ಣಾಂಕ n = 0;
	    nfa_state_T *zend;
	    nfa_state_T *skip;

	    ಆಯ್ಕೆ (*p)
	    {
		ಸ್ಥಿತಿ NFA_PREV_ATOM_NO_ಅಗಲ:
		    ಆರಂಭ_state = NFA_ಆರಂಭ_INVISIBLE;
		    ಕೊನೆ_state = NFA_ಕೊನೆ_INVISIBLE;
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ NFA_PREV_ATOM_NO_ಅಗಲ_NEG:
		    ಆರಂಭ_state = NFA_ಆರಂಭ_INVISIBLE_NEG;
		    ಕೊನೆ_state = NFA_ಕೊನೆ_INVISIBLE_NEG;
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ NFA_PREV_ATOM_JUST_BEFORE:
		    ಆರಂಭ_state = NFA_ಆರಂಭ_INVISIBLE_BEFORE;
		    ಕೊನೆ_state = NFA_ಕೊನೆ_INVISIBLE;
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ NFA_PREV_ATOM_JUST_BEFORE_NEG:
		    ಆರಂಭ_state = NFA_ಆರಂಭ_INVISIBLE_BEFORE_NEG;
		    ಕೊನೆ_state = NFA_ಕೊನೆ_INVISIBLE_NEG;
		    ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ: // NFA_PREV_ATOM_LIKE_PATTERN:
		    ಆರಂಭ_state = NFA_ಆರಂಭ_ನಮೂನೆ;
		    ಕೊನೆ_state = NFA_ಕೊನೆ_ನಮೂನೆ;
		    ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ (before)
		n = *++p; // get the count

	    // The \@= operator: match the preceding atom with zero width.
	    // The \@! operator: no match for the preceding atom.
	    // The \@<= operator: match for the preceding atom.
	    // The \@<! operator: no match for the preceding atom.
	    // Surrounds the preceding atom with START_INVISIBLE and
	    // END_INVISIBLE, similarly to MOPEN.

	    ಆದರೆ (nfa_calc_ಗಾತ್ರ == TRUE)
	    {
		nstate += ನಮೂನೆ ? 4 : 2;
		ನಿಲ್ಲಿಸು;
	    }
	    e = ತೆಗೆ_2();
	    s1 = alloc_state(ಕೊನೆ_state, ಖಾಲಿ, ಖಾಲಿ);
	    ಆದರೆ (s1 == ಖಾಲಿ)
		ಹೋಗು theend;

	    s = alloc_state(ಆರಂಭ_state, e.ಆರಂಭ, s1);
	    ಆದರೆ (s == ಖಾಲಿ)
		ಹೋಗು theend;
	    ಆದರೆ (ನಮೂನೆ)
	    {
		// NFA_ZEND -> NFA_END_PATTERN -> NFA_SKIP -> what follows.
		skip = alloc_state(NFA_SKIP, ಖಾಲಿ, ಖಾಲಿ);
		ಆದರೆ (skip == ಖಾಲಿ)
		    ಹೋಗು theend;
		zend = alloc_state(NFA_ZEND, s1, ಖಾಲಿ);
		ಆದರೆ (zend == ಖಾಲಿ)
		    ಹೋಗು theend;
		s1->ಹೊರಹೋಗು= skip;
		patch(e.ಹೊರಹೋಗು, zend);
		ತಳ್ಳು_2(frag(s, list1(&skip->ಹೊರಹೋಗು)));
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		patch(e.ಹೊರಹೋಗು, s1);
		ತಳ್ಳು_2(frag(s, list1(&s1->ಹೊರಹೋಗು)));
		ಆದರೆ (before)
		{
		    ಆದರೆ (n <= 0)
			// See if we can guess the maximum width, it avoids a
			// lot of pointless tries.
			n = nfa_max_ಅಗಲ(e.ಆರಂಭ, 0);
		    s->val = n; // store the count
		}
	    }
	    ನಿಲ್ಲಿಸು;
	  }

	ಸ್ಥಿತಿ NFA_COMPOSING:	// char with composing char
#ಆದರೆ 0
	    // TODO
	    ಆದರೆ (regflags & RF_ICOMBINE)
	    {
		// use the base character only
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    // FALLTHROUGH

	ಸ್ಥಿತಿ NFA_MOPEN:	// \( \) Submatch
	ಸ್ಥಿತಿ NFA_MOPEN1:
	ಸ್ಥಿತಿ NFA_MOPEN2:
	ಸ್ಥಿತಿ NFA_MOPEN3:
	ಸ್ಥಿತಿ NFA_MOPEN4:
	ಸ್ಥಿತಿ NFA_MOPEN5:
	ಸ್ಥಿತಿ NFA_MOPEN6:
	ಸ್ಥಿತಿ NFA_MOPEN7:
	ಸ್ಥಿತಿ NFA_MOPEN8:
	ಸ್ಥಿತಿ NFA_MOPEN9:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	ಸ್ಥಿತಿ NFA_ZOPEN:	// \z( \) Submatch
	ಸ್ಥಿತಿ NFA_ZOPEN1:
	ಸ್ಥಿತಿ NFA_ZOPEN2:
	ಸ್ಥಿತಿ NFA_ZOPEN3:
	ಸ್ಥಿತಿ NFA_ZOPEN4:
	ಸ್ಥಿತಿ NFA_ZOPEN5:
	ಸ್ಥಿತಿ NFA_ZOPEN6:
	ಸ್ಥಿತಿ NFA_ZOPEN7:
	ಸ್ಥಿತಿ NFA_ZOPEN8:
	ಸ್ಥಿತಿ NFA_ZOPEN9:
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ NFA_NOPEN:	// \%( \) "Invisible Submatch"
	    ಆದರೆ (nfa_calc_ಗಾತ್ರ == TRUE)
	    {
		nstate += 2;
		ನಿಲ್ಲಿಸು;
	    }

	    mopen = *p;
	    ಆಯ್ಕೆ (*p)
	    {
		ಸ್ಥಿತಿ NFA_NOPEN: mclose = NFA_NCLOSE; ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
		ಸ್ಥಿತಿ NFA_ZOPEN: mclose = NFA_ZCLOSE; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ NFA_ZOPEN1: mclose = NFA_ZCLOSE1; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ NFA_ZOPEN2: mclose = NFA_ZCLOSE2; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ NFA_ZOPEN3: mclose = NFA_ZCLOSE3; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ NFA_ZOPEN4: mclose = NFA_ZCLOSE4; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ NFA_ZOPEN5: mclose = NFA_ZCLOSE5; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ NFA_ZOPEN6: mclose = NFA_ZCLOSE6; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ NFA_ZOPEN7: mclose = NFA_ZCLOSE7; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ NFA_ZOPEN8: mclose = NFA_ZCLOSE8; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ NFA_ZOPEN9: mclose = NFA_ZCLOSE9; ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
		ಸ್ಥಿತಿ NFA_COMPOSING: mclose = NFA_ಕೊನೆ_COMPOSING; ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		    // NFA_MOPEN, NFA_MOPEN1 .. NFA_MOPEN9
		    mclose = *p + NSUBEXP;
		    ನಿಲ್ಲಿಸು;
	    }

	    // Allow "NFA_MOPEN" as a valid postfix representation for
	    // the empty regexp "". In this case, the NFA will be
	    // NFA_MOPEN -> NFA_MCLOSE. Note that this also allows
	    // empty groups of parenthesis, and empty mbyte chars
	    ಆದರೆ (stackp == ರಾಶಿ)
	    {
		s = alloc_state(mopen, ಖಾಲಿ, ಖಾಲಿ);
		ಆದರೆ (s == ಖಾಲಿ)
		    ಹೋಗು theend;
		s1 = alloc_state(mclose, ಖಾಲಿ, ಖಾಲಿ);
		ಆದರೆ (s1 == ಖಾಲಿ)
		    ಹೋಗು theend;
		patch(list1(&s->ಹೊರಹೋಗು), s1);
		ತಳ್ಳು_2(frag(s, list1(&s1->ಹೊರಹೋಗು)));
		ನಿಲ್ಲಿಸು;
	    }

	    // At least one node was emitted before NFA_MOPEN, so
	    // at least one node will be between NFA_MOPEN and NFA_MCLOSE
	    e = ತೆಗೆ_2();
	    s = alloc_state(mopen, e.ಆರಂಭ, ಖಾಲಿ);   // `('
	    ಆದರೆ (s == ಖಾಲಿ)
		ಹೋಗು theend;

	    s1 = alloc_state(mclose, ಖಾಲಿ, ಖಾಲಿ);   // `)'
	    ಆದರೆ (s1 == ಖಾಲಿ)
		ಹೋಗು theend;
	    patch(e.ಹೊರಹೋಗು, s1);

	    ಆದರೆ (mopen == NFA_COMPOSING)
		// COMPOSING->out1 = END_COMPOSING
		patch(list1(&s->out1), s1);

	    ತಳ್ಳು_2(frag(s, list1(&s1->ಹೊರಹೋಗು)));
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ NFA_BACKREF1:
	ಸ್ಥಿತಿ NFA_BACKREF2:
	ಸ್ಥಿತಿ NFA_BACKREF3:
	ಸ್ಥಿತಿ NFA_BACKREF4:
	ಸ್ಥಿತಿ NFA_BACKREF5:
	ಸ್ಥಿತಿ NFA_BACKREF6:
	ಸ್ಥಿತಿ NFA_BACKREF7:
	ಸ್ಥಿತಿ NFA_BACKREF8:
	ಸ್ಥಿತಿ NFA_BACKREF9:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	ಸ್ಥಿತಿ NFA_ZREF1:
	ಸ್ಥಿತಿ NFA_ZREF2:
	ಸ್ಥಿತಿ NFA_ZREF3:
	ಸ್ಥಿತಿ NFA_ZREF4:
	ಸ್ಥಿತಿ NFA_ZREF5:
	ಸ್ಥಿತಿ NFA_ZREF6:
	ಸ್ಥಿತಿ NFA_ZREF7:
	ಸ್ಥಿತಿ NFA_ZREF8:
	ಸ್ಥಿತಿ NFA_ZREF9:
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (nfa_calc_ಗಾತ್ರ == TRUE)
	    {
		nstate += 2;
		ನಿಲ್ಲಿಸು;
	    }
	    s = alloc_state(*p, ಖಾಲಿ, ಖಾಲಿ);
	    ಆದರೆ (s == ಖಾಲಿ)
		ಹೋಗು theend;
	    s1 = alloc_state(NFA_SKIP, ಖಾಲಿ, ಖಾಲಿ);
	    ಆದರೆ (s1 == ಖಾಲಿ)
		ಹೋಗು theend;
	    patch(list1(&s->ಹೊರಹೋಗು), s1);
	    ತಳ್ಳು_2(frag(s, list1(&s1->ಹೊರಹೋಗು)));
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ NFA_LNUM:
	ಸ್ಥಿತಿ NFA_LNUM_GT:
	ಸ್ಥಿತಿ NFA_LNUM_LT:
	ಸ್ಥಿತಿ NFA_VCOL:
	ಸ್ಥಿತಿ NFA_VCOL_GT:
	ಸ್ಥಿತಿ NFA_VCOL_LT:
	ಸ್ಥಿತಿ NFA_ಲಂಬಸಾಲು:
	ಸ್ಥಿತಿ NFA_ಲಂಬಸಾಲು_GT:
	ಸ್ಥಿತಿ NFA_ಲಂಬಸಾಲು_LT:
	ಸ್ಥಿತಿ NFA_MARK:
	ಸ್ಥಿತಿ NFA_MARK_GT:
	ಸ್ಥಿತಿ NFA_MARK_LT:
	  {
	    ಪೂರ್ಣಾಂಕ n = *++p; // lnum, col or mark name

	    ಆದರೆ (nfa_calc_ಗಾತ್ರ == TRUE)
	    {
		nstate += 1;
		ನಿಲ್ಲಿಸು;
	    }
	    s = alloc_state(p[-1], ಖಾಲಿ, ಖಾಲಿ);
	    ಆದರೆ (s == ಖಾಲಿ)
		ಹೋಗು theend;
	    s->val = n;
	    ತಳ್ಳು_2(frag(s, list1(&s->ಹೊರಹೋಗು)));
	    ನಿಲ್ಲಿಸು;
	  }

	ಸ್ಥಿತಿ NFA_ZSTART:
	ಸ್ಥಿತಿ NFA_ZEND:
	ಪೂರ್ವನಿಯೋಜಿತ:
	    // Operands
	    ಆದರೆ (nfa_calc_ಗಾತ್ರ == TRUE)
	    {
		nstate++;
		ನಿಲ್ಲಿಸು;
	    }
	    s = alloc_state(*p, ಖಾಲಿ, ಖಾಲಿ);
	    ಆದರೆ (s == ಖಾಲಿ)
		ಹೋಗು theend;
	    ತಳ್ಳು_2(frag(s, list1(&s->ಹೊರಹೋಗು)));
	    ನಿಲ್ಲಿಸು;

	} // switch(*p)

    } // for (p = postfix; *p; ++p)

    ಆದರೆ (nfa_calc_ಗಾತ್ರ == TRUE)
    {
	nstate++;
	ಹೋಗು theend;	// Return value when counting size is ignored anyway
    }

    e = ತೆಗೆ_2();
    ಆದರೆ (stackp != ರಾಶಿ)
	EMSG_RET_NULL(_(e_nfa_regexp_while_converting_from_postfix_ಗಮ್ಯ_nfa_too_many_stats_ಎಡ_on_ರಾಶಿ));

    ಆದರೆ (istate >= nstate)
	EMSG_RET_NULL(_(e_nfa_regexp_not_enough_space_ಗಮ್ಯ_store_ಸಂಪೂರ್ಣ_nfa));

    matchstate = &state_ptr[istate++]; // the match state
    matchstate->c = NFA_ಹೊಂದಿಕೆ;
    matchstate->ಹೊರಹೋಗು = matchstate->out1 = ಖಾಲಿ;
    matchstate->id = 0;

    patch(e.ಹೊರಹೋಗು, matchstate);
    ret = e.ಆರಂಭ;

theend:
    ಹಿಂತಿರುಗಿ ret;

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು POP1
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು PUSH1
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು POP2
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು PUSH2
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು ತೆಗೆ_2
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು ತಳ್ಳು_2
}

/*
 * After building the NFA program, inspect it to add optimization hints.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
nfa_postprocess(nfa_regprog_T *prog)
{
    ಪೂರ್ಣಾಂಕ i;
    ಪೂರ್ಣಾಂಕ c;

    ಗಾಗಿ (i = 0; i < prog->nstate; ++i)
    {
	c = prog->state[i].c;
	ಆದರೆ (c == NFA_ಆರಂಭ_INVISIBLE
		|| c == NFA_ಆರಂಭ_INVISIBLE_NEG
		|| c == NFA_ಆರಂಭ_INVISIBLE_BEFORE
		|| c == NFA_ಆರಂಭ_INVISIBLE_BEFORE_NEG)
	{
	    ಪೂರ್ಣಾಂಕ directly;

	    // Do it directly when what follows is possibly the end of the
	    // match.
	    ಆದರೆ (ಹೊಂದಿಕೆ_follows(prog->state[i].out1->ಹೊರಹೋಗು, 0))
		directly = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಪೂರ್ಣಾಂಕ ch_invisible = failure_chance(prog->state[i].ಹೊರಹೋಗು, 0);
		ಪೂರ್ಣಾಂಕ ch_follows = failure_chance(prog->state[i].out1->ಹೊರಹೋಗು, 0);

		// Postpone when the invisible match is expensive or has a
		// lower chance of failing.
		ಆದರೆ (c == NFA_ಆರಂಭ_INVISIBLE_BEFORE
		     || c == NFA_ಆರಂಭ_INVISIBLE_BEFORE_NEG)
		{
		    // "before" matches are very expensive when
		    // unbounded, always prefer what follows then,
		    // unless what follows will always match.
		    // Otherwise strongly prefer what follows.
		    ಆದರೆ (prog->state[i].val <= 0 && ch_follows > 0)
			directly = FALSE;
		    ಇಲ್ಲದಿದ್ದರೆ
			directly = ch_follows * 10 < ch_invisible;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // normal invisible, first do the one with the
		    // highest failure chance
		    directly = ch_follows < ch_invisible;
		}
	    }
	    ಆದರೆ (directly)
		// switch to the _FIRST state
		++prog->state[i].c;
	}
    }
}

/////////////////////////////////////////////////////////////////
// NFA execution code.
/////////////////////////////////////////////////////////////////

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ	    ಒಳ_use; // number of subexpr with useful info

    // When REG_MULTI is TRUE list.multi is used, otherwise list.line.
    ಒಕ್ಕೂಟ
    {
	ರಚನೆ multipos
	{
	    linenr_T	ಆರಂಭ_lnum;
	    linenr_T	ಕೊನೆ_lnum;
	    colnr_T	ಆರಂಭ_ಲಂಬಸಾಲು;
	    colnr_T	ಕೊನೆ_ಲಂಬಸಾಲು;
	} multi[NSUBEXP];
	ರಚನೆ linepos
	{
	    char_u	*ಆರಂಭ;
	    char_u	*ಕೊನೆ;
	} ಸಾಲು[NSUBEXP];
    } list;
    colnr_T	orig_ಆರಂಭ_ಲಂಬಸಾಲು;  // list.multi[0].start_col without \zs
} regsub_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    regsub_T	norm; // \( .. \) matches
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    regsub_T	synt; // \z( .. \) matches
#ಆದರೆ_ಅಂತ್ಯ
} regsubs_T;

// nfa_pim_T stores a Postponed Invisible Match.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ nfa_pim_S nfa_pim_T;
ರಚನೆ nfa_pim_S
{
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ;		// NFA_PIM_*, see below
    nfa_state_T	*state;		// the invisible match start state
    regsubs_T	subs;		// submatch info, only party used
    ಒಕ್ಕೂಟ
    {
	lpos_T	pos;
	char_u	*ptr;
    } ಕೊನೆ;			// where the match must end
};

// Values for done in nfa_pim_T.
#ವ್ಯಾಖ್ಯಾನಿಸು NFA_PIM_ಬಳಸದ   0	// pim not used
#ವ್ಯಾಖ್ಯಾನಿಸು NFA_PIM_TODO     1	// pim not done yet
#ವ್ಯಾಖ್ಯಾನಿಸು NFA_PIM_ಹೊಂದಿಕೆ    2	// pim executed, matches
#ವ್ಯಾಖ್ಯಾನಿಸು NFA_PIM_NOMATCH  3	// pim executed, no match


// nfa_thread_T contains execution information of a NFA state
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    nfa_state_T	*state;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2;
    nfa_pim_T	pim;		// if pim.result != NFA_PIM_UNUSED: postponed
				// invisible match
    regsubs_T	subs;		// submatch info, only party used
} nfa_thread_T;

// nfa_list_T contains the alternative NFA execution states.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    nfa_thread_T    *t;		// allocated array of states
    ಪೂರ್ಣಾಂಕ		    n;		// nr of states currently in "t"
    ಪೂರ್ಣಾಂಕ		    len;	// max nr of states in "t"
    ಪೂರ್ಣಾಂಕ		    id;		// ID of the list
    ಪೂರ್ಣಾಂಕ		    has_pim;	// TRUE when any state has a PIM
} nfa_list_T;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
ಸ್ಥಿರ ಶೂನ್ಯ log_subexpr(regsub_T *sub);

    ಸ್ಥಿರ ಶೂನ್ಯ
log_subsexpr(regsubs_T *subs)
{
    log_subexpr(&subs->norm);
# ifdef FEAT_SYN_HL
    ಆದರೆ (rex.nfa_has_zsubexpr)
	log_subexpr(&subs->synt);
# endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
log_subexpr(regsub_T *sub)
{
    ಪೂರ್ಣಾಂಕ j;

    ಗಾಗಿ (j = 0; j < sub->ಒಳ_use; j++)
	ಆದರೆ (REG_MULTI)
	    fprintf(log_fd,
		    "*** group %d, start: c=%d, l=%d, end: c=%d, l=%d\n",
		    j,
		    sub->list.multi[j].ಆರಂಭ_ಲಂಬಸಾಲು,
		    (ಪೂರ್ಣಾಂಕ)sub->list.multi[j].ಆರಂಭ_lnum,
		    sub->list.multi[j].ಕೊನೆ_ಲಂಬಸಾಲು,
		    (ಪೂರ್ಣಾಂಕ)sub->list.multi[j].ಕೊನೆ_lnum);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಅಕ್ಷರ *s = (ಅಕ್ಷರ *)sub->list.ಸಾಲು[j].ಆರಂಭ;
	    ಅಕ್ಷರ *e = (ಅಕ್ಷರ *)sub->list.ಸಾಲು[j].ಕೊನೆ;

	    fprintf(log_fd, "*** group %d, start: \"%s\", end: \"%s\"\n",
		    j,
		    s == ಖಾಲಿ ? "NULL" : s,
		    e == ಖಾಲಿ ? "NULL" : e);
	}
}

    ಸ್ಥಿರ ಅಕ್ಷರ *
pim_info(nfa_pim_T *pim)
{
    ಸ್ಥಿರ ಅಕ್ಷರ buf[30];

    ಆದರೆ (pim == ಖಾಲಿ || pim->ಫಲಿತಾಂಶ == NFA_PIM_ಬಳಸದ)
	buf[0] = NUL;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	sprintf(buf, " PIM col %d", REG_MULTI ? (ಪೂರ್ಣಾಂಕ)pim->ಕೊನೆ.pos.ಲಂಬಸಾಲು
		: (ಪೂರ್ಣಾಂಕ)(pim->ಕೊನೆ.ptr - rex.ಒಳಹರಿವು));
    }
    ಹಿಂತಿರುಗಿ buf;
}

#ಆದರೆ_ಅಂತ್ಯ

// Used during execution: whether a match has been found.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    nfa_ಹೊಂದಿಕೆ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	   *nfa_timed_ಹೊರಹೋಗು;
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ copy_sub(regsub_T *ಗಮ್ಯ, regsub_T *from);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pim_equal(nfa_pim_T *one, nfa_pim_T *two);

/*
 * Copy postponed invisible match info from "from" to "to".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
copy_pim(nfa_pim_T *ಗಮ್ಯ, nfa_pim_T *from)
{
    ಗಮ್ಯ->ಫಲಿತಾಂಶ = from->ಫಲಿತಾಂಶ;
    ಗಮ್ಯ->state = from->state;
    copy_sub(&ಗಮ್ಯ->subs.norm, &from->subs.norm);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ಆದರೆ (rex.nfa_has_zsubexpr)
	copy_sub(&ಗಮ್ಯ->subs.synt, &from->subs.synt);
#ಆದರೆ_ಅಂತ್ಯ
    ಗಮ್ಯ->ಕೊನೆ = from->ಕೊನೆ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clear_sub(regsub_T *sub)
{
    ಆದರೆ (REG_MULTI)
	// Use 0xff to set lnum to -1
	vim_memset(sub->list.multi, 0xff,
				  ಗಾತ್ರ(ರಚನೆ multipos) * rex.nfa_nsubexpr);
    ಇಲ್ಲದಿದ್ದರೆ
	vim_memset(sub->list.ಸಾಲು, 0,
				   ಗಾತ್ರ(ರಚನೆ linepos) * rex.nfa_nsubexpr);
    sub->ಒಳ_use = 0;
}

/*
 * Copy the submatches from "from" to "to".
 */
    ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
copy_sub(regsub_T *ಗಮ್ಯ, regsub_T *from)
{
    ಗಮ್ಯ->ಒಳ_use = from->ಒಳ_use;
    ಆದರೆ (from->ಒಳ_use <= 0)
	ಹಿಂತಿರುಗಿ;

    // Copy the match start and end positions.  addstate() calls this for
    // every state it adds, so it is very hot.  Patterns without capturing
    // groups only use subexpression zero (the whole match), so special-case
    // "in_use == 1" with a direct assignment to avoid the mch_memmove() call.
    ಆದರೆ (REG_MULTI)
    {
	ಆದರೆ (from->ಒಳ_use == 1)
	    ಗಮ್ಯ->list.multi[0] = from->list.multi[0];
	ಇಲ್ಲದಿದ್ದರೆ
	    mch_memmove(&ಗಮ್ಯ->list.multi[0],
		    &from->list.multi[0],
		    ಗಾತ್ರ(ರಚನೆ multipos) * from->ಒಳ_use);
	ಗಮ್ಯ->orig_ಆರಂಭ_ಲಂಬಸಾಲು = from->orig_ಆರಂಭ_ಲಂಬಸಾಲು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (from->ಒಳ_use == 1)
	    ಗಮ್ಯ->list.ಸಾಲು[0] = from->list.ಸಾಲು[0];
	ಇಲ್ಲದಿದ್ದರೆ
	    mch_memmove(&ಗಮ್ಯ->list.ಸಾಲು[0],
		    &from->list.ಸಾಲು[0],
		    ಗಾತ್ರ(ರಚನೆ linepos) * from->ಒಳ_use);
    }
}

/*
 * Like copy_sub() but exclude the main match.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
copy_sub_off(regsub_T *ಗಮ್ಯ, regsub_T *from)
{
    ಆದರೆ (ಗಮ್ಯ->ಒಳ_use < from->ಒಳ_use)
	ಗಮ್ಯ->ಒಳ_use = from->ಒಳ_use;
    ಆದರೆ (from->ಒಳ_use <= 1)
	ಹಿಂತಿರುಗಿ;

    // Copy the match start and end positions.
    ಆದರೆ (REG_MULTI)
	mch_memmove(&ಗಮ್ಯ->list.multi[1],
		&from->list.multi[1],
		ಗಾತ್ರ(ರಚನೆ multipos) * (from->ಒಳ_use - 1));
    ಇಲ್ಲದಿದ್ದರೆ
	mch_memmove(&ಗಮ್ಯ->list.ಸಾಲು[1],
		&from->list.ಸಾಲು[1],
		ಗಾತ್ರ(ರಚನೆ linepos) * (from->ಒಳ_use - 1));
}

/*
 * Like copy_sub() but only do the end of the main match if \ze is present.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
copy_ze_off(regsub_T *ಗಮ್ಯ, regsub_T *from)
{
    ಆದರೆ (!rex.nfa_has_zend)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (REG_MULTI)
    {
	ಆದರೆ (from->list.multi[0].ಕೊನೆ_lnum >= 0)
	{
	    ಗಮ್ಯ->list.multi[0].ಕೊನೆ_lnum = from->list.multi[0].ಕೊನೆ_lnum;
	    ಗಮ್ಯ->list.multi[0].ಕೊನೆ_ಲಂಬಸಾಲು = from->list.multi[0].ಕೊನೆ_ಲಂಬಸಾಲು;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (from->list.ಸಾಲು[0].ಕೊನೆ != ಖಾಲಿ)
	    ಗಮ್ಯ->list.ಸಾಲು[0].ಕೊನೆ = from->list.ಸಾಲು[0].ಕೊನೆ;
    }
}

/*
 * Return TRUE if "sub1" and "sub2" have the same start positions.
 * When using back-references also check the end position.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
sub_equal(regsub_T *sub1, regsub_T *sub2)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		todo;
    linenr_T	s1;
    linenr_T	s2;
    char_u	*sp1;
    char_u	*sp2;

    todo = sub1->ಒಳ_use > sub2->ಒಳ_use ? sub1->ಒಳ_use : sub2->ಒಳ_use;
    ಆದರೆ (REG_MULTI)
    {
	ಗಾಗಿ (i = 0; i < todo; ++i)
	{
	    ಆದರೆ (i < sub1->ಒಳ_use)
		s1 = sub1->list.multi[i].ಆರಂಭ_lnum;
	    ಇಲ್ಲದಿದ್ದರೆ
		s1 = -1;
	    ಆದರೆ (i < sub2->ಒಳ_use)
		s2 = sub2->list.multi[i].ಆರಂಭ_lnum;
	    ಇಲ್ಲದಿದ್ದರೆ
		s2 = -1;
	    ಆದರೆ (s1 != s2)
		ಹಿಂತಿರುಗಿ FALSE;
	    ಆದರೆ (s1 != -1 && sub1->list.multi[i].ಆರಂಭ_ಲಂಬಸಾಲು
					     != sub2->list.multi[i].ಆರಂಭ_ಲಂಬಸಾಲು)
		ಹಿಂತಿರುಗಿ FALSE;

	    ಆದರೆ (rex.nfa_has_backref)
	    {
		ಆದರೆ (i < sub1->ಒಳ_use)
		    s1 = sub1->list.multi[i].ಕೊನೆ_lnum;
		ಇಲ್ಲದಿದ್ದರೆ
		    s1 = -1;
		ಆದರೆ (i < sub2->ಒಳ_use)
		    s2 = sub2->list.multi[i].ಕೊನೆ_lnum;
		ಇಲ್ಲದಿದ್ದರೆ
		    s2 = -1;
		ಆದರೆ (s1 != s2)
		    ಹಿಂತಿರುಗಿ FALSE;
		ಆದರೆ (s1 != -1 && sub1->list.multi[i].ಕೊನೆ_ಲಂಬಸಾಲು
					       != sub2->list.multi[i].ಕೊನೆ_ಲಂಬಸಾಲು)
		ಹಿಂತಿರುಗಿ FALSE;
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಾಗಿ (i = 0; i < todo; ++i)
	{
	    ಆದರೆ (i < sub1->ಒಳ_use)
		sp1 = sub1->list.ಸಾಲು[i].ಆರಂಭ;
	    ಇಲ್ಲದಿದ್ದರೆ
		sp1 = ಖಾಲಿ;
	    ಆದರೆ (i < sub2->ಒಳ_use)
		sp2 = sub2->list.ಸಾಲು[i].ಆರಂಭ;
	    ಇಲ್ಲದಿದ್ದರೆ
		sp2 = ಖಾಲಿ;
	    ಆದರೆ (sp1 != sp2)
		ಹಿಂತಿರುಗಿ FALSE;
	    ಆದರೆ (rex.nfa_has_backref)
	    {
		ಆದರೆ (i < sub1->ಒಳ_use)
		    sp1 = sub1->list.ಸಾಲು[i].ಕೊನೆ;
		ಇಲ್ಲದಿದ್ದರೆ
		    sp1 = ಖಾಲಿ;
		ಆದರೆ (i < sub2->ಒಳ_use)
		    sp2 = sub2->list.ಸಾಲು[i].ಕೊನೆ;
		ಇಲ್ಲದಿದ್ದರೆ
		    sp2 = ಖಾಲಿ;
		ಆದರೆ (sp1 != sp2)
		    ಹಿಂತಿರುಗಿ FALSE;
	    }
	}
    }

    ಹಿಂತಿರುಗಿ TRUE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
/*
 * Check if we are past the time limit, if there is one.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
nfa_did_ಸಮಯ_ಹೊರಹೋಗು(ಶೂನ್ಯ)
{
    ಆದರೆ (*timeout_flag)
    {
	ಆದರೆ (nfa_timed_ಹೊರಹೋಗು != ಖಾಲಿ)
	{
# ifdef FEAT_EVAL
	    ಆದರೆ (!*nfa_timed_ಹೊರಹೋಗು)
		ch_log(ಖಾಲಿ, "NFA regexp timed out");
# endif
	    *nfa_timed_ಹೊರಹೋಗು = TRUE;
	}
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
    ಸ್ಥಿರ ಶೂನ್ಯ
open_debug_log(ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ)
{
    log_fd = fopen(NFA_REGEXP_ಸತತ_LOG, "a");
    ಆದರೆ (log_fd == ಖಾಲಿ)
    {
	emsg(_(e_log_open_failed));
	log_fd = stderr;
    }

    fprintf(log_fd, "****************************\n");
    fprintf(log_fd, "FINISHED RUNNING nfa_regmatch() recursively\n");
    fprintf(log_fd, "MATCH = %s\n", ಫಲಿತಾಂಶ == TRUE ? "OK" : ಫಲಿತಾಂಶ == MAYBE
	    ? "MAYBE" : "FALSE");
    fprintf(log_fd, "****************************\n");
}

    ಸ್ಥಿರ ಶೂನ್ಯ
report_state(ಅಕ್ಷರ *action,
	     regsub_T *sub,
	     nfa_state_T *state,
	     ಪೂರ್ಣಾಂಕ lid,
	     nfa_pim_T *pim)
{
    ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು;

    ಆದರೆ (sub->ಒಳ_use <= 0)
	ಲಂಬಸಾಲು = -1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (REG_MULTI)
	ಲಂಬಸಾಲು = sub->list.multi[0].ಆರಂಭ_ಲಂಬಸಾಲು;
    ಇಲ್ಲದಿದ್ದರೆ
	ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)(sub->list.ಸಾಲು[0].ಆರಂಭ - rex.ಸಾಲು);
    nfa_ಗಣ_ಸಂಕೇತ(state->c);
    ಆದರೆ (log_fd == ಖಾಲಿ)
	open_debug_log(MAYBE);

    fprintf(log_fd, "> %s state %d to list %d. char %d: %s (start col %d)%s\n",
	    action, ನಿರಪೇಕ್ಷ(state->id), lid, state->c, ಸಂಕೇತ_2, ಲಂಬಸಾಲು,
	    pim_info(pim));
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return TRUE if the same state is already in list "l" with the same
 * positions as "subs".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
has_state_with_pos(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim)	// postponed match or NULL
{
    nfa_thread_T	*thread;
    ಪೂರ್ಣಾಂಕ			i;

    ಗಾಗಿ (i = 0; i < l->n; ++i)
    {
	thread = &l->t[i];
	ಆದರೆ (thread->state->id == state->id
		&& sub_equal(&thread->subs.norm, &subs->norm)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
		&& (!rex.nfa_has_zsubexpr
				|| sub_equal(&thread->subs.synt, &subs->synt))
#ಆದರೆ_ಅಂತ್ಯ
		&& pim_equal(&thread->pim, pim))
	    ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Return TRUE if "one" and "two" are equal.  That includes when both are not
 * set.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
pim_equal(nfa_pim_T *one, nfa_pim_T *two)
{
    ಪೂರ್ಣಾಂಕ one_ಬಳಸದ = (one == ಖಾಲಿ || one->ಫಲಿತಾಂಶ == NFA_PIM_ಬಳಸದ);
    ಪೂರ್ಣಾಂಕ two_ಬಳಸದ = (two == ಖಾಲಿ || two->ಫಲಿತಾಂಶ == NFA_PIM_ಬಳಸದ);

    ಆದರೆ (one_ಬಳಸದ)
	// one is unused: equal when two is also unused
	ಹಿಂತಿರುಗಿ two_ಬಳಸದ;
    ಆದರೆ (two_ಬಳಸದ)
	// one is used and two is not: not equal
	ಹಿಂತಿರುಗಿ FALSE;
    // compare the state id
    ಆದರೆ (one->state->id != two->state->id)
	ಹಿಂತಿರುಗಿ FALSE;
    // compare the position
    ಆದರೆ (REG_MULTI)
	ಹಿಂತಿರುಗಿ one->ಕೊನೆ.pos.lnum == two->ಕೊನೆ.pos.lnum
	    && one->ಕೊನೆ.pos.ಲಂಬಸಾಲು == two->ಕೊನೆ.pos.ಲಂಬಸಾಲು;
    ಹಿಂತಿರುಗಿ one->ಕೊನೆ.ptr == two->ಕೊನೆ.ptr;
}

/*
 * Return TRUE if "state" leads to a NFA_MATCH without advancing the input.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಹೊಂದಿಕೆ_follows(nfa_state_T *startstate, ಪೂರ್ಣಾಂಕ ಆಳ)
{
    nfa_state_T	    *state = startstate;

    // avoid too much recursion
    ಆದರೆ (ಆಳ > 10)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆಗ (state != ಖಾಲಿ)
    {
	ಆಯ್ಕೆ (state->c)
	{
	    ಸ್ಥಿತಿ NFA_ಹೊಂದಿಕೆ:
	    ಸ್ಥಿತಿ NFA_MCLOSE:
	    ಸ್ಥಿತಿ NFA_ಕೊನೆ_INVISIBLE:
	    ಸ್ಥಿತಿ NFA_ಕೊನೆ_INVISIBLE_NEG:
	    ಸ್ಥಿತಿ NFA_ಕೊನೆ_ನಮೂನೆ:
		ಹಿಂತಿರುಗಿ TRUE;

	    ಸ್ಥಿತಿ NFA_ವಿಭಜಿಸು:
		ಹಿಂತಿರುಗಿ ಹೊಂದಿಕೆ_follows(state->ಹೊರಹೋಗು, ಆಳ + 1)
				     || ಹೊಂದಿಕೆ_follows(state->out1, ಆಳ + 1);

	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE:
	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_ಮೊದಲ:
	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_BEFORE:
	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_BEFORE_ಮೊದಲ:
	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_NEG:
	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_NEG_ಮೊದಲ:
	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_BEFORE_NEG:
	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_BEFORE_NEG_ಮೊದಲ:
	    ಸ್ಥಿತಿ NFA_COMPOSING:
		// skip ahead to next state
		state = state->out1->ಹೊರಹೋಗು;
		ಮುಂದುವರಿ;

	    ಸ್ಥಿತಿ NFA_ANY:
	    ಸ್ಥಿತಿ NFA_ANY_COMPOSING:
	    ಸ್ಥಿತಿ NFA_IDENT:
	    ಸ್ಥಿತಿ NFA_SIDENT:
	    ಸ್ಥಿತಿ NFA_KWORD:
	    ಸ್ಥಿತಿ NFA_SKWORD:
	    ಸ್ಥಿತಿ NFA_FNAME:
	    ಸ್ಥಿತಿ NFA_SFNAME:
	    ಸ್ಥಿತಿ NFA_PRINT:
	    ಸ್ಥಿತಿ NFA_SPRINT:
	    ಸ್ಥಿತಿ NFA_WHITE:
	    ಸ್ಥಿತಿ NFA_NWHITE:
	    ಸ್ಥಿತಿ NFA_DIGIT:
	    ಸ್ಥಿತಿ NFA_NDIGIT:
	    ಸ್ಥಿತಿ NFA_HEX:
	    ಸ್ಥಿತಿ NFA_NHEX:
	    ಸ್ಥಿತಿ NFA_OCTAL:
	    ಸ್ಥಿತಿ NFA_NOCTAL:
	    ಸ್ಥಿತಿ NFA_ಶಬ್ದ:
	    ಸ್ಥಿತಿ NFA_NWORD:
	    ಸ್ಥಿತಿ NFA_ತಲೆ:
	    ಸ್ಥಿತಿ NFA_NHEAD:
	    ಸ್ಥಿತಿ NFA_ALPHA:
	    ಸ್ಥಿತಿ NFA_NALPHA:
	    ಸ್ಥಿತಿ NFA_ಅಧೋ:
	    ಸ್ಥಿತಿ NFA_NLOWER:
	    ಸ್ಥಿತಿ NFA_ಊರ್ಧ್ವ:
	    ಸ್ಥಿತಿ NFA_NUPPER:
	    ಸ್ಥಿತಿ NFA_ಅಧೋ_IC:
	    ಸ್ಥಿತಿ NFA_NLOWER_IC:
	    ಸ್ಥಿತಿ NFA_ಊರ್ಧ್ವ_IC:
	    ಸ್ಥಿತಿ NFA_NUPPER_IC:
	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_COLL:
	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_NEG_COLL:
	    ಸ್ಥಿತಿ NFA_NEWL:
		// state will advance input
		ಹಿಂತಿರುಗಿ FALSE;

	    ಪೂರ್ವನಿಯೋಜಿತ:
		ಆದರೆ (state->c > 0)
		    // state will advance input
		    ಹಿಂತಿರುಗಿ FALSE;

		// Others: zero-width or possibly zero-width, might still find
		// a match at the same position, keep looking.
		ನಿಲ್ಲಿಸು;
	}
	state = state->ಹೊರಹೋಗು;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}


/*
 * Return TRUE if "state" is already in list "l".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
state_ಒಳ_list(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs)	// pointers to subexpressions
{
    ಆದರೆ (state->lastlist[nfa_ll_ಸೂಚ್ಯಂಕ] == l->id)
    {
	ಆದರೆ (!rex.nfa_has_backref || has_state_with_pos(l, state, subs, ಖಾಲಿ))
	    ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

// Offset used for "off" by addstate_here().
#ವ್ಯಾಖ್ಯಾನಿಸು ADDSTATE_ಇಲ್ಲಿ_ಸ್ಥಾನಾಂತರ 10

/*
 * Add "state" and possibly what follows to state list ".".
 * Returns "subs_arg", possibly copied into temp_subs.
 * Returns NULL when recursiveness is too deep or timed out.
 */
    ಸ್ಥಿರ regsubs_T *
addstate(
    nfa_list_T		*l,	    // runtime state list
    nfa_state_T		*state,	    // state to update
    regsubs_T		*subs_arg,  // pointers to subexpressions
    nfa_pim_T		*pim,	    // postponed look-behind match
    ಪೂರ್ಣಾಂಕ			off_arg,    // byte offset, when -1 go to next line
    ಪೂರ್ಣಾಂಕ			ಆಳ)	    // recursion depth
{
    ಪೂರ್ಣಾಂಕ			subidx;
    ಪೂರ್ಣಾಂಕ			off = off_arg;
    ಪೂರ್ಣಾಂಕ			ಕೂಡಿಸು_ಇಲ್ಲಿ = FALSE;
    ಪೂರ್ಣಾಂಕ			listindex = 0;
    ಪೂರ್ಣಾಂಕ			k;
    ಪೂರ್ಣಾಂಕ			ಸಿಕ್ಕಿತು = FALSE;
    nfa_thread_T	*thread;
    ರಚನೆ multipos	save_multipos;
    ಪೂರ್ಣಾಂಕ			save_ಒಳ_use;
    char_u		*save_ptr;
    ಪೂರ್ಣಾಂಕ			i;
    regsub_T		*sub;
    regsubs_T		*subs = subs_arg;
    ಸ್ಥಿರ regsubs_T	ತಾತ್ಕಾಲಿಕ_subs;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
    ಪೂರ್ಣಾಂಕ			did_print = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
    ಆದರೆ (nfa_did_ಸಮಯ_ಹೊರಹೋಗು())
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

    // This function is called recursively.  When the depth is too much we run
    // out of stack and crash, limit recursiveness here.
    ಆದರೆ (ಆಳ >= 5000 || subs == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (off_arg <= -ADDSTATE_ಇಲ್ಲಿ_ಸ್ಥಾನಾಂತರ)
    {
	ಕೂಡಿಸು_ಇಲ್ಲಿ = TRUE;
	off = 0;
	listindex = -(off_arg + ADDSTATE_ಇಲ್ಲಿ_ಸ್ಥಾನಾಂತರ);
    }

    ಆಯ್ಕೆ (state->c)
    {
	ಸ್ಥಿತಿ NFA_NCLOSE:
	ಸ್ಥಿತಿ NFA_MCLOSE:
	ಸ್ಥಿತಿ NFA_MCLOSE1:
	ಸ್ಥಿತಿ NFA_MCLOSE2:
	ಸ್ಥಿತಿ NFA_MCLOSE3:
	ಸ್ಥಿತಿ NFA_MCLOSE4:
	ಸ್ಥಿತಿ NFA_MCLOSE5:
	ಸ್ಥಿತಿ NFA_MCLOSE6:
	ಸ್ಥಿತಿ NFA_MCLOSE7:
	ಸ್ಥಿತಿ NFA_MCLOSE8:
	ಸ್ಥಿತಿ NFA_MCLOSE9:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	ಸ್ಥಿತಿ NFA_ZCLOSE:
	ಸ್ಥಿತಿ NFA_ZCLOSE1:
	ಸ್ಥಿತಿ NFA_ZCLOSE2:
	ಸ್ಥಿತಿ NFA_ZCLOSE3:
	ಸ್ಥಿತಿ NFA_ZCLOSE4:
	ಸ್ಥಿತಿ NFA_ZCLOSE5:
	ಸ್ಥಿತಿ NFA_ZCLOSE6:
	ಸ್ಥಿತಿ NFA_ZCLOSE7:
	ಸ್ಥಿತಿ NFA_ZCLOSE8:
	ಸ್ಥಿತಿ NFA_ZCLOSE9:
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ NFA_MOPEN:
	ಸ್ಥಿತಿ NFA_ZEND:
	ಸ್ಥಿತಿ NFA_ವಿಭಜಿಸು:
	ಸ್ಥಿತಿ NFA_ಖಾಲಿಗಳು:
	    // These nodes are not added themselves but their "out" and/or
	    // "out1" may be added below.
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ NFA_BOL:
	ಸ್ಥಿತಿ NFA_BOF:
	    // "^" won't match past end-of-line, don't bother trying.
	    // Except when at the end of the line, or when we are going to the
	    // next line for a look-behind match.
	    ಆದರೆ (rex.ಒಳಹರಿವು > rex.ಸಾಲು
		    && *rex.ಒಳಹರಿವು != NUL
		    && (nfa_endp == ಖಾಲಿ
			|| !REG_MULTI
			|| rex.lnum == nfa_endp->se_u.pos.lnum))
		ಹೋಗು skip_ಕೂಡಿಸು;
	    // FALLTHROUGH

	ಸ್ಥಿತಿ NFA_MOPEN1:
	ಸ್ಥಿತಿ NFA_MOPEN2:
	ಸ್ಥಿತಿ NFA_MOPEN3:
	ಸ್ಥಿತಿ NFA_MOPEN4:
	ಸ್ಥಿತಿ NFA_MOPEN5:
	ಸ್ಥಿತಿ NFA_MOPEN6:
	ಸ್ಥಿತಿ NFA_MOPEN7:
	ಸ್ಥಿತಿ NFA_MOPEN8:
	ಸ್ಥಿತಿ NFA_MOPEN9:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	ಸ್ಥಿತಿ NFA_ZOPEN:
	ಸ್ಥಿತಿ NFA_ZOPEN1:
	ಸ್ಥಿತಿ NFA_ZOPEN2:
	ಸ್ಥಿತಿ NFA_ZOPEN3:
	ಸ್ಥಿತಿ NFA_ZOPEN4:
	ಸ್ಥಿತಿ NFA_ZOPEN5:
	ಸ್ಥಿತಿ NFA_ZOPEN6:
	ಸ್ಥಿತಿ NFA_ZOPEN7:
	ಸ್ಥಿತಿ NFA_ZOPEN8:
	ಸ್ಥಿತಿ NFA_ZOPEN9:
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ NFA_NOPEN:
	ಸ್ಥಿತಿ NFA_ZSTART:
	    // These nodes need to be added so that we can bail out when it
	    // was added to this list before at the same position to avoid an
	    // endless loop for "\(\)*"

	ಪೂರ್ವನಿಯೋಜಿತ:
	    ಆದರೆ (state->lastlist[nfa_ll_ಸೂಚ್ಯಂಕ] == l->id && state->c != NFA_SKIP)
	    {
		// This state is already in the list, don't add it again,
		// unless it is an MOPEN that is used for a backreference or
		// when there is a PIM. For NFA_MATCH check the position,
		// lower position is preferred.
		ಆದರೆ (!rex.nfa_has_backref && pim == ಖಾಲಿ && !l->has_pim
						     && state->c != NFA_ಹೊಂದಿಕೆ)
		{
		    // When called from addstate_here() do insert before
		    // existing states.
		    ಆದರೆ (ಕೂಡಿಸು_ಇಲ್ಲಿ)
		    {
			ಗಾಗಿ (k = 0; k < l->n && k < listindex; ++k)
			    ಆದರೆ (l->t[k].state->id == state->id)
			    {
				ಸಿಕ್ಕಿತು = TRUE;
				ನಿಲ್ಲಿಸು;
			    }
		    }
		    ಆದರೆ (!ಕೂಡಿಸು_ಇಲ್ಲಿ || ಸಿಕ್ಕಿತು)
		    {
skip_ಕೂಡಿಸು:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
			nfa_ಗಣ_ಸಂಕೇತ(state->c);
			fprintf(log_fd, "> Not adding state %d to list %d. char %d: %s pim: %s has_pim: %d found: %d\n",
			    ನಿರಪೇಕ್ಷ(state->id), l->id, state->c, ಸಂಕೇತ_2,
			    pim == ಖಾಲಿ ? "NULL" : "yes", l->has_pim, ಸಿಕ್ಕಿತು);
#ಆದರೆ_ಅಂತ್ಯ
			ಹಿಂತಿರುಗಿ subs;
		    }
		}

		// Do not add the state again when it exists with the same
		// positions.
		ಆದರೆ (has_state_with_pos(l, state, subs, pim))
		    ಹೋಗು skip_ಕೂಡಿಸು;
	    }

	    // When there are backreferences or PIMs the number of states may
	    // be (a lot) bigger than anticipated.
	    ಆದರೆ (l->n == l->len)
	    {
		ಪೂರ್ಣಾಂಕ		newlen = l->len * 3 / 2 + 50;
		size_t		newsize = newlen * ಗಾತ್ರ(nfa_thread_T);
		nfa_thread_T	*newt;

		ಆದರೆ ((ದೀರ್ಘ)(newsize >> 10) >= p_mmp)
		{
		    emsg(_(e_ನಮೂನೆ_ಬಳಸುತ್ತದೆ_ಇನ್ನಷ್ಟು_ಸ್ಮರಣೆ_than_maxmempattern));
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		ಆದರೆ (subs != &ತಾತ್ಕಾಲಿಕ_subs)
		{
		    // "subs" may point into the current array, need to make a
		    // copy before it becomes invalid.
		    copy_sub(&ತಾತ್ಕಾಲಿಕ_subs.norm, &subs->norm);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
		    ಆದರೆ (rex.nfa_has_zsubexpr)
			copy_sub(&ತಾತ್ಕಾಲಿಕ_subs.synt, &subs->synt);
#ಆದರೆ_ಅಂತ್ಯ
		    subs = &ತಾತ್ಕಾಲಿಕ_subs;
		}

		newt = vim_realloc(l->t, newsize);
		ಆದರೆ (newt == ಖಾಲಿ)
		{
		    // out of memory
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		l->t = newt;
		l->len = newlen;
	    }

	    // add the state to the list
	    state->lastlist[nfa_ll_ಸೂಚ್ಯಂಕ] = l->id;
	    thread = &l->t[l->n++];
	    thread->state = state;
	    ಆದರೆ (pim == ಖಾಲಿ)
		thread->pim.ಫಲಿತಾಂಶ = NFA_PIM_ಬಳಸದ;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		copy_pim(&thread->pim, pim);
		l->has_pim = TRUE;
	    }
	    copy_sub(&thread->subs.norm, &subs->norm);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	    ಆದರೆ (rex.nfa_has_zsubexpr)
		copy_sub(&thread->subs.synt, &subs->synt);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
	    report_state("Adding", &thread->subs.norm, state, l->id, pim);
	    did_print = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
    ಆದರೆ (!did_print)
	report_state("Processing", &subs->norm, state, l->id, pim);
#ಆದರೆ_ಅಂತ್ಯ
    ಆಯ್ಕೆ (state->c)
    {
	ಸ್ಥಿತಿ NFA_ಹೊಂದಿಕೆ:
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ NFA_ವಿಭಜಿಸು:
	    // order matters here
	    subs = addstate(l, state->ಹೊರಹೋಗು, subs, pim, off_arg, ಆಳ + 1);
	    subs = addstate(l, state->out1, subs, pim, off_arg, ಆಳ + 1);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ NFA_ಖಾಲಿಗಳು:
	ಸ್ಥಿತಿ NFA_NOPEN:
	ಸ್ಥಿತಿ NFA_NCLOSE:
	    subs = addstate(l, state->ಹೊರಹೋಗು, subs, pim, off_arg, ಆಳ + 1);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ NFA_MOPEN:
	ಸ್ಥಿತಿ NFA_MOPEN1:
	ಸ್ಥಿತಿ NFA_MOPEN2:
	ಸ್ಥಿತಿ NFA_MOPEN3:
	ಸ್ಥಿತಿ NFA_MOPEN4:
	ಸ್ಥಿತಿ NFA_MOPEN5:
	ಸ್ಥಿತಿ NFA_MOPEN6:
	ಸ್ಥಿತಿ NFA_MOPEN7:
	ಸ್ಥಿತಿ NFA_MOPEN8:
	ಸ್ಥಿತಿ NFA_MOPEN9:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	ಸ್ಥಿತಿ NFA_ZOPEN:
	ಸ್ಥಿತಿ NFA_ZOPEN1:
	ಸ್ಥಿತಿ NFA_ZOPEN2:
	ಸ್ಥಿತಿ NFA_ZOPEN3:
	ಸ್ಥಿತಿ NFA_ZOPEN4:
	ಸ್ಥಿತಿ NFA_ZOPEN5:
	ಸ್ಥಿತಿ NFA_ZOPEN6:
	ಸ್ಥಿತಿ NFA_ZOPEN7:
	ಸ್ಥಿತಿ NFA_ZOPEN8:
	ಸ್ಥಿತಿ NFA_ZOPEN9:
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ NFA_ZSTART:
	    ಆದರೆ (state->c == NFA_ZSTART)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
	    {
		subidx = state->c - NFA_ZOPEN;
		sub = &subs->synt;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		subidx = state->c - NFA_MOPEN;
		sub = &subs->norm;
	    }

	    // avoid compiler warnings
	    save_ptr = ಖಾಲಿ;
	    CLEAR_ಕ್ಷೇತ್ರ(save_multipos);

	    // Set the position (with "off" added) in the subexpression.  Save
	    // and restore it when it was in use.  Otherwise fill any gap.
	    ಆದರೆ (REG_MULTI)
	    {
		ಆದರೆ (subidx < sub->ಒಳ_use)
		{
		    save_multipos = sub->list.multi[subidx];
		    save_ಒಳ_use = -1;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    save_ಒಳ_use = sub->ಒಳ_use;
		    ಗಾಗಿ (i = sub->ಒಳ_use; i < subidx; ++i)
		    {
			sub->list.multi[i].ಆರಂಭ_lnum = -1;
			sub->list.multi[i].ಕೊನೆ_lnum = -1;
		    }
		    sub->ಒಳ_use = subidx + 1;
		}
		ಆದರೆ (off == -1)
		{
		    sub->list.multi[subidx].ಆರಂಭ_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].ಆರಂಭ_ಲಂಬಸಾಲು = 0;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    sub->list.multi[subidx].ಆರಂಭ_lnum = rex.lnum;
		    sub->list.multi[subidx].ಆರಂಭ_ಲಂಬಸಾಲು =
					 (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು + off);
		}
		sub->list.multi[subidx].ಕೊನೆ_lnum = -1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (subidx < sub->ಒಳ_use)
		{
		    save_ptr = sub->list.ಸಾಲು[subidx].ಆರಂಭ;
		    save_ಒಳ_use = -1;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    save_ಒಳ_use = sub->ಒಳ_use;
		    ಗಾಗಿ (i = sub->ಒಳ_use; i < subidx; ++i)
		    {
			sub->list.ಸಾಲು[i].ಆರಂಭ = ಖಾಲಿ;
			sub->list.ಸಾಲು[i].ಕೊನೆ = ಖಾಲಿ;
		    }
		    sub->ಒಳ_use = subidx + 1;
		}
		sub->list.ಸಾಲು[subidx].ಆರಂಭ = rex.ಒಳಹರಿವು + off;
	    }

	    subs = addstate(l, state->ಹೊರಹೋಗು, subs, pim, off_arg, ಆಳ + 1);
	    ಆದರೆ (subs == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    // "subs" may have changed, need to set "sub" again
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	    ಆದರೆ (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
		sub = &subs->synt;
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		sub = &subs->norm;

	    ಆದರೆ (save_ಒಳ_use == -1)
	    {
		ಆದರೆ (REG_MULTI)
		    sub->list.multi[subidx] = save_multipos;
		ಇಲ್ಲದಿದ್ದರೆ
		    sub->list.ಸಾಲು[subidx].ಆರಂಭ = save_ptr;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		sub->ಒಳ_use = save_ಒಳ_use;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ NFA_MCLOSE:
	    ಆದರೆ (rex.nfa_has_zend && (REG_MULTI
			? subs->norm.list.multi[0].ಕೊನೆ_lnum >= 0
			: subs->norm.list.ಸಾಲು[0].ಕೊನೆ != ಖಾಲಿ))
	    {
		// Do not overwrite the position set by \ze.
		subs = addstate(l, state->ಹೊರಹೋಗು, subs, pim, off_arg, ಆಳ + 1);
		ನಿಲ್ಲಿಸು;
	    }
	    // FALLTHROUGH
	ಸ್ಥಿತಿ NFA_MCLOSE1:
	ಸ್ಥಿತಿ NFA_MCLOSE2:
	ಸ್ಥಿತಿ NFA_MCLOSE3:
	ಸ್ಥಿತಿ NFA_MCLOSE4:
	ಸ್ಥಿತಿ NFA_MCLOSE5:
	ಸ್ಥಿತಿ NFA_MCLOSE6:
	ಸ್ಥಿತಿ NFA_MCLOSE7:
	ಸ್ಥಿತಿ NFA_MCLOSE8:
	ಸ್ಥಿತಿ NFA_MCLOSE9:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	ಸ್ಥಿತಿ NFA_ZCLOSE:
	ಸ್ಥಿತಿ NFA_ZCLOSE1:
	ಸ್ಥಿತಿ NFA_ZCLOSE2:
	ಸ್ಥಿತಿ NFA_ZCLOSE3:
	ಸ್ಥಿತಿ NFA_ZCLOSE4:
	ಸ್ಥಿತಿ NFA_ZCLOSE5:
	ಸ್ಥಿತಿ NFA_ZCLOSE6:
	ಸ್ಥಿತಿ NFA_ZCLOSE7:
	ಸ್ಥಿತಿ NFA_ZCLOSE8:
	ಸ್ಥಿತಿ NFA_ZCLOSE9:
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ NFA_ZEND:
	    ಆದರೆ (state->c == NFA_ZEND)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
	    {
		subidx = state->c - NFA_ZCLOSE;
		sub = &subs->synt;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		subidx = state->c - NFA_MCLOSE;
		sub = &subs->norm;
	    }

	    // We don't fill in gaps here, there must have been an MOPEN that
	    // has done that.
	    save_ಒಳ_use = sub->ಒಳ_use;
	    ಆದರೆ (sub->ಒಳ_use <= subidx)
		sub->ಒಳ_use = subidx + 1;
	    ಆದರೆ (REG_MULTI)
	    {
		save_multipos = sub->list.multi[subidx];
		ಆದರೆ (off == -1)
		{
		    sub->list.multi[subidx].ಕೊನೆ_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].ಕೊನೆ_ಲಂಬಸಾಲು = 0;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    sub->list.multi[subidx].ಕೊನೆ_lnum = rex.lnum;
		    sub->list.multi[subidx].ಕೊನೆ_ಲಂಬಸಾಲು =
					  (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು + off);
		}
		// avoid compiler warnings
		save_ptr = ಖಾಲಿ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		save_ptr = sub->list.ಸಾಲು[subidx].ಕೊನೆ;
		sub->list.ಸಾಲು[subidx].ಕೊನೆ = rex.ಒಳಹರಿವು + off;
		// avoid compiler warnings
		CLEAR_ಕ್ಷೇತ್ರ(save_multipos);
	    }

	    subs = addstate(l, state->ಹೊರಹೋಗು, subs, pim, off_arg, ಆಳ + 1);
	    ಆದರೆ (subs == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    // "subs" may have changed, need to set "sub" again
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	    ಆದರೆ (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
		sub = &subs->synt;
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		sub = &subs->norm;

	    ಆದರೆ (REG_MULTI)
		sub->list.multi[subidx] = save_multipos;
	    ಇಲ್ಲದಿದ್ದರೆ
		sub->list.ಸಾಲು[subidx].ಕೊನೆ = save_ptr;
	    sub->ಒಳ_use = save_ಒಳ_use;
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ subs;
}

/*
 * Like addstate(), but the new state(s) are put at position "*ip".
 * Used for zero-width matches, next state to use is the added one.
 * This makes sure the order of states to be tried does not change, which
 * matters for alternatives.
 */
    ಸ್ಥಿರ regsubs_T *
addstate_ಇಲ್ಲಿ(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim,   // postponed look-behind match
    ಪೂರ್ಣಾಂಕ			*ip)
{
    ಪೂರ್ಣಾಂಕ tlen = l->n;
    ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2;
    ಪೂರ್ಣಾಂಕ listidx = *ip;
    regsubs_T *r;

    // First add the state(s) at the end, so that we know how many there are.
    // Pass the listidx as offset (avoids adding another argument to
    // addstate()).
    r = addstate(l, state, subs, pim, -listidx - ADDSTATE_ಇಲ್ಲಿ_ಸ್ಥಾನಾಂತರ, 0);
    ಆದರೆ (r == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // when "*ip" was at the end of the list, nothing to do
    ಆದರೆ (listidx + 1 == tlen)
	ಹಿಂತಿರುಗಿ r;

    // re-order to put the new state at the current position
    ಎಣಿಕೆ_2 = l->n - tlen;
    ಆದರೆ (ಎಣಿಕೆ_2 == 0)
	ಹಿಂತಿರುಗಿ r; // no state got added
    ಆದರೆ (ಎಣಿಕೆ_2 == 1)
    {
	// overwrite the current state
	l->t[listidx] = l->t[l->n - 1];
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಎಣಿಕೆ_2 > 1)
    {
	ಆದರೆ (l->n + ಎಣಿಕೆ_2 - 1 >= l->len)
	{
	    // not enough space to move the new states, reallocate the list
	    // and move the states to the right position
	    ಪೂರ್ಣಾಂಕ		    newlen = l->len * 3 / 2 + 50;
	    size_t	    newsize = newlen * ಗಾತ್ರ(nfa_thread_T);
	    nfa_thread_T    *newl;

	    ಆದರೆ ((ದೀರ್ಘ)(newsize >> 10) >= p_mmp)
	    {
		emsg(_(e_ನಮೂನೆ_ಬಳಸುತ್ತದೆ_ಇನ್ನಷ್ಟು_ಸ್ಮರಣೆ_than_maxmempattern));
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    newl = alloc(newsize);
	    ಆದರೆ (newl == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    l->len = newlen;
	    mch_memmove(&(newl[0]),
		    &(l->t[0]),
		    ಗಾತ್ರ(nfa_thread_T) * listidx);
	    mch_memmove(&(newl[listidx]),
		    &(l->t[l->n - ಎಣಿಕೆ_2]),
		    ಗಾತ್ರ(nfa_thread_T) * ಎಣಿಕೆ_2);
	    mch_memmove(&(newl[listidx + ಎಣಿಕೆ_2]),
		    &(l->t[listidx + 1]),
		    ಗಾತ್ರ(nfa_thread_T) * (l->n - ಎಣಿಕೆ_2 - listidx - 1));
	    vim_free(l->t);
	    l->t = newl;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // make space for new states, then move them from the
	    // end to the current position
	    mch_memmove(&(l->t[listidx + ಎಣಿಕೆ_2]),
		    &(l->t[listidx + 1]),
		    ಗಾತ್ರ(nfa_thread_T) * (l->n - listidx - 1));
	    mch_memmove(&(l->t[listidx]),
		    &(l->t[l->n - 1]),
		    ಗಾತ್ರ(nfa_thread_T) * ಎಣಿಕೆ_2);
	}
    }
    --l->n;
    *ip = listidx - 1;

    ಹಿಂತಿರುಗಿ r;
}

/*
 * Check character class "class" against current character c.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_char_class(ಪೂರ್ಣಾಂಕ class, ಪೂರ್ಣಾಂಕ c)
{
    ಆಯ್ಕೆ (class)
    {
	ಸ್ಥಿತಿ NFA_CLASS_ALNUM:
	    ಆದರೆ (c >= 1 && c < 128 && ಅಕ್ಷರವೇ_ಅಂಕಿಯೇ(c))
		ಹಿಂತಿರುಗಿ OK;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_ALPHA:
	    ಆದರೆ (c >= 1 && c < 128 && ಅಕ್ಷರವೇ(c))
		ಹಿಂತಿರುಗಿ OK;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_BLANK:
	    ಆದರೆ (c == ' ' || c == '\t')
		ಹಿಂತಿರುಗಿ OK;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_CNTRL:
	    ಆದರೆ (c >= 1 && c <= 127 && iscntrl(c))
		ಹಿಂತಿರುಗಿ OK;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_DIGIT:
	    ಆದರೆ (VIM_ISDIGIT(c))
		ಹಿಂತಿರುಗಿ OK;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_ನಕ್ಷೆ:
	    ಆದರೆ (c >= 1 && c <= 127 && isgraph(c))
		ಹಿಂತಿರುಗಿ OK;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_ಅಧೋ:
	    ಆದರೆ (MB_ISLOWER(c) && c != 170 && c != 186)
		ಹಿಂತಿರುಗಿ OK;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_PRINT:
	    ಆದರೆ (vim_isprintc(c))
		ಹಿಂತಿರುಗಿ OK;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_PUNCT:
	    ಆದರೆ (c >= 1 && c < 128 && ispunct(c))
		ಹಿಂತಿರುಗಿ OK;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_SPACE:
	    ಆದರೆ ((c >= 9 && c <= 13) || (c == ' '))
		ಹಿಂತಿರುಗಿ OK;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_ಊರ್ಧ್ವ:
	    ಆದರೆ (MB_ISUPPER(c))
		ಹಿಂತಿರುಗಿ OK;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_XDIGIT:
	    ಆದರೆ (vim_isxdigit(c))
		ಹಿಂತಿರುಗಿ OK;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_TAB:
	    ಆದರೆ (c == '\t')
		ಹಿಂತಿರುಗಿ OK;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_RETURN:
	    ಆದರೆ (c == '\r')
		ಹಿಂತಿರುಗಿ OK;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_BACKSPACE:
	    ಆದರೆ (c == '\b')
		ಹಿಂತಿರುಗಿ OK;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_ESCAPE:
	    ಆದರೆ (c == '\033')
		ಹಿಂತಿರುಗಿ OK;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_IDENT:
	    ಆದರೆ (vim_isIDc(c))
		ಹಿಂತಿರುಗಿ OK;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_KEYWORD:
	    ಆದರೆ (reg_iswordc(c))
		ಹಿಂತಿರುಗಿ OK;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NFA_CLASS_FNAME:
	    ಆದರೆ (vim_isfilec(c))
		ಹಿಂತಿರುಗಿ OK;
	    ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
	    // should not be here :P
	    siemsg(_(e_nfa_regexp_invalid_character_class_nr), class);
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Check for a match with subexpression "subidx".
 * Return TRUE if it matches.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಹೊಂದಿಕೆ_backref(
    regsub_T	*sub,	    // pointers to subexpressions
    ಪೂರ್ಣಾಂಕ		subidx,
    ಪೂರ್ಣಾಂಕ		*bytelen)   // out: length of match in bytes
{
    ಪೂರ್ಣಾಂಕ		len;

    ಆದರೆ (sub->ಒಳ_use <= subidx)
    {
retempty:
	// backref was not set, match an empty string
	*bytelen = 0;
	ಹಿಂತಿರುಗಿ TRUE;
    }

    ಆದರೆ (REG_MULTI)
    {
	ಆದರೆ (sub->list.multi[subidx].ಆರಂಭ_lnum < 0
				       || sub->list.multi[subidx].ಕೊನೆ_lnum < 0)
	    ಹೋಗು retempty;
	ಆದರೆ (sub->list.multi[subidx].ಆರಂಭ_lnum == rex.lnum
			       && sub->list.multi[subidx].ಕೊನೆ_lnum == rex.lnum)
	{
	    len = sub->list.multi[subidx].ಕೊನೆ_ಲಂಬಸಾಲು
					  - sub->list.multi[subidx].ಆರಂಭ_ಲಂಬಸಾಲು;
	    ಆದರೆ (cstrncmp(rex.ಸಾಲು + sub->list.multi[subidx].ಆರಂಭ_ಲಂಬಸಾಲು,
							 rex.ಒಳಹರಿವು, &len) == 0)
	    {
		*bytelen = len;
		ಹಿಂತಿರುಗಿ TRUE;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (ಹೊಂದಿಕೆ_with_backref(
			sub->list.multi[subidx].ಆರಂಭ_lnum,
			sub->list.multi[subidx].ಆರಂಭ_ಲಂಬಸಾಲು,
			sub->list.multi[subidx].ಕೊನೆ_lnum,
			sub->list.multi[subidx].ಕೊನೆ_ಲಂಬಸಾಲು,
			bytelen) == RA_ಹೊಂದಿಕೆ)
		ಹಿಂತಿರುಗಿ TRUE;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (sub->list.ಸಾಲು[subidx].ಆರಂಭ == ಖಾಲಿ
					|| sub->list.ಸಾಲು[subidx].ಕೊನೆ == ಖಾಲಿ)
	    ಹೋಗು retempty;
	len = (ಪೂರ್ಣಾಂಕ)(sub->list.ಸಾಲು[subidx].ಕೊನೆ - sub->list.ಸಾಲು[subidx].ಆರಂಭ);
	ಆದರೆ (cstrncmp(sub->list.ಸಾಲು[subidx].ಆರಂಭ, rex.ಒಳಹರಿವು, &len) == 0)
	{
	    *bytelen = len;
	    ಹಿಂತಿರುಗಿ TRUE;
	}
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL

/*
 * Check for a match with \z subexpression "subidx".
 * Return TRUE if it matches.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಹೊಂದಿಕೆ_zref(
    ಪೂರ್ಣಾಂಕ		subidx,
    ಪೂರ್ಣಾಂಕ		*bytelen)   // out: length of match in bytes
{
    ಪೂರ್ಣಾಂಕ		len;

    cleanup_zsubexpr();
    ಆದರೆ (re_extmatch_ಒಳ == ಖಾಲಿ || re_extmatch_ಒಳ->matches[subidx] == ಖಾಲಿ)
    {
	// backref was not set, match an empty string
	*bytelen = 0;
	ಹಿಂತಿರುಗಿ TRUE;
    }

    len = (ಪೂರ್ಣಾಂಕ)STRLEN(re_extmatch_ಒಳ->matches[subidx]);
    ಆದರೆ (cstrncmp(re_extmatch_ಒಳ->matches[subidx], rex.ಒಳಹರಿವು, &len) == 0)
    {
	*bytelen = len;
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Save list IDs for all NFA states of "prog" into "list".
 * Also reset the IDs to zero.
 * Only used for the recursive value lastlist[1].
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
nfa_save_listids(nfa_regprog_T *prog, ಪೂರ್ಣಾಂಕ *list)
{
    ಪೂರ್ಣಾಂಕ		    i;
    nfa_state_T	    *p;

    // Order in the list is reverse, it's a bit faster that way.
    p = &prog->state[0];
    ಗಾಗಿ (i = prog->nstate; --i >= 0; )
    {
	list[i] = p->lastlist[1];
	p->lastlist[1] = 0;
	++p;
    }
}

/*
 * Restore list IDs from "list" to all NFA states.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
nfa_restore_listids(nfa_regprog_T *prog, ಪೂರ್ಣಾಂಕ *list)
{
    ಪೂರ್ಣಾಂಕ		    i;
    nfa_state_T	    *p;

    p = &prog->state[0];
    ಗಾಗಿ (i = prog->nstate; --i >= 0; )
    {
	p->lastlist[1] = list[i];
	++p;
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
nfa_re_ಅಂಕೆ_cmp(long_u val, ಪೂರ್ಣಾಂಕ op, long_u pos)
{
    ಆದರೆ (op == 1) ಹಿಂತಿರುಗಿ pos > val;
    ಆದರೆ (op == 2) ಹಿಂತಿರುಗಿ pos < val;
    ಹಿಂತಿರುಗಿ val == pos;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ nfa_regmatch(nfa_regprog_T *prog, nfa_state_T *ಆರಂಭ, regsubs_T *submatch, regsubs_T *m);

/*
 * Recursively call nfa_regmatch()
 * "pim" is NULL or contains info about a Postponed Invisible Match (start
 * position).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
recursive_regmatch(
    nfa_state_T	    *state,
    nfa_pim_T	    *pim,
    nfa_regprog_T   *prog,
    regsubs_T	    *submatch,
    regsubs_T	    *m,
    ಪೂರ್ಣಾಂಕ		    **listids,
    ಪೂರ್ಣಾಂಕ		    *listids_len)
{
    ಪೂರ್ಣಾಂಕ		save_reginput_ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)(rex.ಒಳಹರಿವು - rex.ಸಾಲು);
    ಪೂರ್ಣಾಂಕ		save_reglnum = rex.lnum;
    ಪೂರ್ಣಾಂಕ		save_nfa_ಹೊಂದಿಕೆ = nfa_ಹೊಂದಿಕೆ;
    ಪೂರ್ಣಾಂಕ		save_nfa_listid = rex.nfa_listid;
    save_se_T   *save_nfa_endp = nfa_endp;
    save_se_T   endpos;
    save_se_T   *endposp = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ;
    ಪೂರ್ಣಾಂಕ		need_restore = FALSE;

    ಆದರೆ (pim != ಖಾಲಿ)
    {
	// start at the position where the postponed match was
	ಆದರೆ (REG_MULTI)
	    rex.ಒಳಹರಿವು = rex.ಸಾಲು + pim->ಕೊನೆ.pos.ಲಂಬಸಾಲು;
	ಇಲ್ಲದಿದ್ದರೆ
	    rex.ಒಳಹರಿವು = pim->ಕೊನೆ.ptr;
    }

    ಆದರೆ (state->c == NFA_ಆರಂಭ_INVISIBLE_BEFORE
	    || state->c == NFA_ಆರಂಭ_INVISIBLE_BEFORE_ಮೊದಲ
	    || state->c == NFA_ಆರಂಭ_INVISIBLE_BEFORE_NEG
	    || state->c == NFA_ಆರಂಭ_INVISIBLE_BEFORE_NEG_ಮೊದಲ)
    {
	// The recursive match must end at the current position. When "pim" is
	// not NULL it specifies the current position.
	endposp = &endpos;
	ಆದರೆ (REG_MULTI)
	{
	    ಆದರೆ (pim == ಖಾಲಿ)
	    {
		endpos.se_u.pos.ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)(rex.ಒಳಹರಿವು - rex.ಸಾಲು);
		endpos.se_u.pos.lnum = rex.lnum;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		endpos.se_u.pos = pim->ಕೊನೆ.pos;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (pim == ಖಾಲಿ)
		endpos.se_u.ptr = rex.ಒಳಹರಿವು;
	    ಇಲ್ಲದಿದ್ದರೆ
		endpos.se_u.ptr = pim->ಕೊನೆ.ptr;
	}

	// Go back the specified number of bytes, or as far as the
	// start of the previous line, to try matching "\@<=" or
	// not matching "\@<!". This is very inefficient, limit the number of
	// bytes if possible.
	ಆದರೆ (state->val <= 0)
	{
	    ಆದರೆ (REG_MULTI)
	    {
		rex.ಸಾಲು = reg_getline(--rex.lnum);
		ಆದರೆ (rex.ಸಾಲು == ಖಾಲಿ)
		    // can't go before the first line
		    rex.ಸಾಲು = reg_getline(++rex.lnum);
	    }
	    rex.ಒಳಹರಿವು = rex.ಸಾಲು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (REG_MULTI && (ಪೂರ್ಣಾಂಕ)(rex.ಒಳಹರಿವು - rex.ಸಾಲು) < state->val)
	    {
		// Not enough bytes in this line, go to end of
		// previous line.
		rex.ಸಾಲು = reg_getline(--rex.lnum);
		ಆದರೆ (rex.ಸಾಲು == ಖಾಲಿ)
		{
		    // can't go before the first line
		    rex.ಸಾಲು = reg_getline(++rex.lnum);
		    rex.ಒಳಹರಿವು = rex.ಸಾಲು;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    rex.ಒಳಹರಿವು = rex.ಸಾಲು + reg_getline_len(rex.lnum);
	    }
	    ಆದರೆ ((ಪೂರ್ಣಾಂಕ)(rex.ಒಳಹರಿವು - rex.ಸಾಲು) >= state->val)
	    {
		rex.ಒಳಹರಿವು -= state->val;
		ಆದರೆ (has_mbyte)
		    rex.ಒಳಹರಿವು -= mb_ತಲೆ_off(rex.ಸಾಲು, rex.ಒಳಹರಿವು);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		rex.ಒಳಹರಿವು = rex.ಸಾಲು;
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
    ಆದರೆ (log_fd != stderr)
	fclose(log_fd);
    log_fd = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    // Have to clear the lastlist field of the NFA nodes, so that
    // nfa_regmatch() and addstate() can run properly after recursion.
    ಆದರೆ (nfa_ll_ಸೂಚ್ಯಂಕ == 1)
    {
	// Already calling nfa_regmatch() recursively.  Save the lastlist[1]
	// values and clear them.
	ಆದರೆ (*listids == ಖಾಲಿ || *listids_len < prog->nstate)
	{
	    vim_free(*listids);
	    *listids = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, prog->nstate);
	    ಆದರೆ (*listids == ಖಾಲಿ)
	    {
		emsg(_(e_nfa_regexp_could_not_allocate_ಸ್ಮರಣೆ_for_branch_traversal));
		ಹಿಂತಿರುಗಿ 0;
	    }
	    *listids_len = prog->nstate;
	}
	nfa_save_listids(prog, *listids);
	need_restore = TRUE;
	// any value of rex.nfa_listid will do
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// First recursive nfa_regmatch() call, switch to the second lastlist
	// entry.  Make sure rex.nfa_listid is different from a previous
	// recursive call, because some states may still have this ID.
	++nfa_ll_ಸೂಚ್ಯಂಕ;
	ಆದರೆ (rex.nfa_listid <= rex.nfa_alt_listid)
	    rex.nfa_listid = rex.nfa_alt_listid;
    }

    // Call nfa_regmatch() to check if the current concat matches at this
    // position. The concat ends with the node NFA_END_INVISIBLE
    nfa_endp = endposp;
    ಫಲಿತಾಂಶ = nfa_regmatch(prog, state->ಹೊರಹೋಗು, submatch, m);

    ಆದರೆ (need_restore)
	nfa_restore_listids(prog, *listids);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	--nfa_ll_ಸೂಚ್ಯಂಕ;
	rex.nfa_alt_listid = rex.nfa_listid;
    }

    // restore position in input text
    rex.lnum = save_reglnum;
    ಆದರೆ (REG_MULTI)
	rex.ಸಾಲು = reg_getline(rex.lnum);
    rex.ಒಳಹರಿವು = rex.ಸಾಲು + save_reginput_ಲಂಬಸಾಲು;
    ಆದರೆ (ಫಲಿತಾಂಶ != NFA_TOO_EXPENSIVE)
    {
	nfa_ಹೊಂದಿಕೆ = save_nfa_ಹೊಂದಿಕೆ;
	rex.nfa_listid = save_nfa_listid;
    }
    nfa_endp = save_nfa_endp;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
    open_debug_log(ಫಲಿತಾಂಶ);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Estimate the chance of a match with "state" failing.
 * empty match: 0
 * NFA_ANY: 1
 * specific character: 99
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
failure_chance(nfa_state_T *state, ಪೂರ್ಣಾಂಕ ಆಳ)
{
    ಪೂರ್ಣಾಂಕ c = state->c;
    ಪೂರ್ಣಾಂಕ l, r;

    // detect looping
    ಆದರೆ (ಆಳ > 4)
	ಹಿಂತಿರುಗಿ 1;

    ಆಯ್ಕೆ (c)
    {
	ಸ್ಥಿತಿ NFA_ವಿಭಜಿಸು:
	    ಆದರೆ (state->ಹೊರಹೋಗು->c == NFA_ವಿಭಜಿಸು || state->out1->c == NFA_ವಿಭಜಿಸು)
		// avoid recursive stuff
		ಹಿಂತಿರುಗಿ 1;
	    // two alternatives, use the lowest failure chance
	    l = failure_chance(state->ಹೊರಹೋಗು, ಆಳ + 1);
	    r = failure_chance(state->out1, ಆಳ + 1);
	    ಹಿಂತಿರುಗಿ l < r ? l : r;

	ಸ್ಥಿತಿ NFA_ANY:
	    // matches anything, unlikely to fail
	    ಹಿಂತಿರುಗಿ 1;

	ಸ್ಥಿತಿ NFA_ಹೊಂದಿಕೆ:
	ಸ್ಥಿತಿ NFA_MCLOSE:
	ಸ್ಥಿತಿ NFA_ANY_COMPOSING:
	    // empty match works always
	    ಹಿಂತಿರುಗಿ 0;

	ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE:
	ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_ಮೊದಲ:
	ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_NEG:
	ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_NEG_ಮೊದಲ:
	ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_BEFORE:
	ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_BEFORE_ಮೊದಲ:
	ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_BEFORE_NEG:
	ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_BEFORE_NEG_ಮೊದಲ:
	ಸ್ಥಿತಿ NFA_ಆರಂಭ_ನಮೂನೆ:
	    // recursive regmatch is expensive, use low failure chance
	    ಹಿಂತಿರುಗಿ 5;

	ಸ್ಥಿತಿ NFA_BOL:
	ಸ್ಥಿತಿ NFA_EOL:
	ಸ್ಥಿತಿ NFA_BOF:
	ಸ್ಥಿತಿ NFA_EOF:
	ಸ್ಥಿತಿ NFA_NEWL:
	    ಹಿಂತಿರುಗಿ 99;

	ಸ್ಥಿತಿ NFA_BOW:
	ಸ್ಥಿತಿ NFA_EOW:
	    ಹಿಂತಿರುಗಿ 90;

	ಸ್ಥಿತಿ NFA_MOPEN:
	ಸ್ಥಿತಿ NFA_MOPEN1:
	ಸ್ಥಿತಿ NFA_MOPEN2:
	ಸ್ಥಿತಿ NFA_MOPEN3:
	ಸ್ಥಿತಿ NFA_MOPEN4:
	ಸ್ಥಿತಿ NFA_MOPEN5:
	ಸ್ಥಿತಿ NFA_MOPEN6:
	ಸ್ಥಿತಿ NFA_MOPEN7:
	ಸ್ಥಿತಿ NFA_MOPEN8:
	ಸ್ಥಿತಿ NFA_MOPEN9:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	ಸ್ಥಿತಿ NFA_ZOPEN:
	ಸ್ಥಿತಿ NFA_ZOPEN1:
	ಸ್ಥಿತಿ NFA_ZOPEN2:
	ಸ್ಥಿತಿ NFA_ZOPEN3:
	ಸ್ಥಿತಿ NFA_ZOPEN4:
	ಸ್ಥಿತಿ NFA_ZOPEN5:
	ಸ್ಥಿತಿ NFA_ZOPEN6:
	ಸ್ಥಿತಿ NFA_ZOPEN7:
	ಸ್ಥಿತಿ NFA_ZOPEN8:
	ಸ್ಥಿತಿ NFA_ZOPEN9:
	ಸ್ಥಿತಿ NFA_ZCLOSE:
	ಸ್ಥಿತಿ NFA_ZCLOSE1:
	ಸ್ಥಿತಿ NFA_ZCLOSE2:
	ಸ್ಥಿತಿ NFA_ZCLOSE3:
	ಸ್ಥಿತಿ NFA_ZCLOSE4:
	ಸ್ಥಿತಿ NFA_ZCLOSE5:
	ಸ್ಥಿತಿ NFA_ZCLOSE6:
	ಸ್ಥಿತಿ NFA_ZCLOSE7:
	ಸ್ಥಿತಿ NFA_ZCLOSE8:
	ಸ್ಥಿತಿ NFA_ZCLOSE9:
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ NFA_NOPEN:
	ಸ್ಥಿತಿ NFA_MCLOSE1:
	ಸ್ಥಿತಿ NFA_MCLOSE2:
	ಸ್ಥಿತಿ NFA_MCLOSE3:
	ಸ್ಥಿತಿ NFA_MCLOSE4:
	ಸ್ಥಿತಿ NFA_MCLOSE5:
	ಸ್ಥಿತಿ NFA_MCLOSE6:
	ಸ್ಥಿತಿ NFA_MCLOSE7:
	ಸ್ಥಿತಿ NFA_MCLOSE8:
	ಸ್ಥಿತಿ NFA_MCLOSE9:
	ಸ್ಥಿತಿ NFA_NCLOSE:
	    ಹಿಂತಿರುಗಿ failure_chance(state->ಹೊರಹೋಗು, ಆಳ + 1);

	ಸ್ಥಿತಿ NFA_BACKREF1:
	ಸ್ಥಿತಿ NFA_BACKREF2:
	ಸ್ಥಿತಿ NFA_BACKREF3:
	ಸ್ಥಿತಿ NFA_BACKREF4:
	ಸ್ಥಿತಿ NFA_BACKREF5:
	ಸ್ಥಿತಿ NFA_BACKREF6:
	ಸ್ಥಿತಿ NFA_BACKREF7:
	ಸ್ಥಿತಿ NFA_BACKREF8:
	ಸ್ಥಿತಿ NFA_BACKREF9:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	ಸ್ಥಿತಿ NFA_ZREF1:
	ಸ್ಥಿತಿ NFA_ZREF2:
	ಸ್ಥಿತಿ NFA_ZREF3:
	ಸ್ಥಿತಿ NFA_ZREF4:
	ಸ್ಥಿತಿ NFA_ZREF5:
	ಸ್ಥಿತಿ NFA_ZREF6:
	ಸ್ಥಿತಿ NFA_ZREF7:
	ಸ್ಥಿತಿ NFA_ZREF8:
	ಸ್ಥಿತಿ NFA_ZREF9:
#ಆದರೆ_ಅಂತ್ಯ
	    // backreferences don't match in many places
	    ಹಿಂತಿರುಗಿ 94;

	ಸ್ಥಿತಿ NFA_LNUM_GT:
	ಸ್ಥಿತಿ NFA_LNUM_LT:
	ಸ್ಥಿತಿ NFA_ಲಂಬಸಾಲು_GT:
	ಸ್ಥಿತಿ NFA_ಲಂಬಸಾಲು_LT:
	ಸ್ಥಿತಿ NFA_VCOL_GT:
	ಸ್ಥಿತಿ NFA_VCOL_LT:
	ಸ್ಥಿತಿ NFA_MARK_GT:
	ಸ್ಥಿತಿ NFA_MARK_LT:
	ಸ್ಥಿತಿ NFA_VISUAL:
	    // before/after positions don't match very often
	    ಹಿಂತಿರುಗಿ 85;

	ಸ್ಥಿತಿ NFA_LNUM:
	    ಹಿಂತಿರುಗಿ 90;

	ಸ್ಥಿತಿ NFA_CURSOR:
	ಸ್ಥಿತಿ NFA_ಲಂಬಸಾಲು:
	ಸ್ಥಿತಿ NFA_VCOL:
	ಸ್ಥಿತಿ NFA_MARK:
	    // specific positions rarely match
	    ಹಿಂತಿರುಗಿ 98;

	ಸ್ಥಿತಿ NFA_COMPOSING:
	    ಹಿಂತಿರುಗಿ 95;

	ಪೂರ್ವನಿಯೋಜಿತ:
	    ಆದರೆ (c > 0)
		// character match fails often
		ಹಿಂತಿರುಗಿ 95;
    }

    // something else, includes character classes
    ಹಿಂತಿರುಗಿ 50;
}

/*
 * Skip until the char "c" we know a match must start with.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
skip_ಗಮ್ಯ_ಆರಂಭ(ಪೂರ್ಣಾಂಕ c, colnr_T *colp)
{
    char_u *s;

    // Used often, do some work to avoid call overhead.
    ಆದರೆ (!rex.reg_ic && !has_mbyte)
	s = vim_strbyte(rex.ಸಾಲು + *colp, c);
    ಇಲ್ಲದಿದ್ದರೆ
	s = cstrchr(rex.ಸಾಲು + *colp, c);
    ಆದರೆ (s == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    *colp = (ಪೂರ್ಣಾಂಕ)(s - rex.ಸಾಲು);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check for a match with match_text.
 * Called after skip_to_start() has found regstart.
 * Returns zero for no match, 1 for a match.
 */
    ಸ್ಥಿರ ದೀರ್ಘ
ಕಂಡುಹಿಡಿ_ಹೊಂದಿಕೆ_ಪಠ್ಯ(colnr_T *startcol, ಪೂರ್ಣಾಂಕ regstart, char_u *ಹೊಂದಿಕೆ_ಪಠ್ಯ)
{
    colnr_T ಲಂಬಸಾಲು = *startcol;
    ಪೂರ್ಣಾಂಕ	    c1, c2;
    ಪೂರ್ಣಾಂಕ	    len1, len2;
    ಪೂರ್ಣಾಂಕ	    ಹೊಂದಿಕೆ;

    ಗಾಗಿ (;;)
    {
	ಹೊಂದಿಕೆ = TRUE;
	// Skip the input character: case-folding can change its byte length.
	len2 = MB_CPTR2LEN(rex.ಸಾಲು + ಲಂಬಸಾಲು);
	ಗಾಗಿ (len1 = 0; ಹೊಂದಿಕೆ_ಪಠ್ಯ[len1] != NUL; len1 += MB_CHAR2LEN(c1))
	{
	    c1 = PTR2CHAR(ಹೊಂದಿಕೆ_ಪಠ್ಯ + len1);
	    c2 = PTR2CHAR(rex.ಸಾಲು + ಲಂಬಸಾಲು + len2);
	    ಆದರೆ (c1 != c2 && (!rex.reg_ic || MB_CASEFOLD(c1) != MB_CASEFOLD(c2)))
	    {
		ಹೊಂದಿಕೆ = FALSE;
		ನಿಲ್ಲಿಸು;
	    }
	    len2 += enc_utf8 ? utf_ptr2len(rex.ಸಾಲು + ಲಂಬಸಾಲು + len2)
							     : MB_CHAR2LEN(c2);
	}
	ಆದರೆ (ಹೊಂದಿಕೆ
		// check that no composing char follows
		&& !(enc_utf8
			  && utf_iscomposing(PTR2CHAR(rex.ಸಾಲು + ಲಂಬಸಾಲು + len2))))
	{
	    cleanup_subexpr();
	    ಆದರೆ (REG_MULTI)
	    {
		rex.reg_startpos[0].lnum = rex.lnum;
		rex.reg_startpos[0].ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
		rex.reg_endpos[0].lnum = rex.lnum;
		rex.reg_endpos[0].ಲಂಬಸಾಲು = ಲಂಬಸಾಲು + len2;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		rex.reg_startp[0] = rex.ಸಾಲು + ಲಂಬಸಾಲು;
		rex.reg_endp[0] = rex.ಸಾಲು + ಲಂಬಸಾಲು + len2;
	    }
	    *startcol = ಲಂಬಸಾಲು;
	    ಹಿಂತಿರುಗಿ 1L;
	}

	// Try finding regstart after the current match.
	ಲಂಬಸಾಲು += MB_CPTR2LEN(rex.ಸಾಲು + ಲಂಬಸಾಲು); // skip the input character
	ಆದರೆ (skip_ಗಮ್ಯ_ಆರಂಭ(regstart, &ಲಂಬಸಾಲು) == FAIL)
	    ನಿಲ್ಲಿಸು;
    }

    *startcol = ಲಂಬಸಾಲು;
    ಹಿಂತಿರುಗಿ 0L;
}

/*
 * Check whether the composing characters at the current input position match
 * the NFA composing sub-expression that starts at "sta" (the first state
 * after NFA_COMPOSING).  "curc" is the base character and "clen" its byte
 * length.
 * Returns OK when it matches, FAIL otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಹೊಂದಿಕೆ_composing(nfa_state_T *sta, ಪೂರ್ಣಾಂಕ curc, ಪೂರ್ಣಾಂಕ clen)
{
    ಪೂರ್ಣಾಂಕ		mc = curc;
    ಪೂರ್ಣಾಂಕ		len = 0;
    ಪೂರ್ಣಾಂಕ		cchars[MAX_MCO];
    ಪೂರ್ಣಾಂಕ		ccount = 0;
    ಪೂರ್ಣಾಂಕ		j;

    ಆದರೆ (utf_iscomposing(sta->c))
	// Only match composing character(s), ignore base character.  Used
	// for ".{composing}" and "{composing}" (no preceding character).
	len += mb_char2len(mc);

    ಆದರೆ (rex.reg_icombine && len == 0)
	// If \Z was present, then ignore composing characters.  When
	// ignoring the base character this always matches.
	ಹಿಂತಿರುಗಿ sta->c == curc ? OK : FAIL;

    // Check base character matches first, unless ignored.
    ಆದರೆ (len == 0 && mc != sta->c)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (len == 0)
    {
	len += mb_char2len(mc);
	sta = sta->ಹೊರಹೋಗು;
    }

    // We don't care about the order of composing characters.  Get them into
    // cchars[] first.
    ಆಗ (len < clen)
    {
	mc = mb_ptr2char(rex.ಒಳಹರಿವು + len);
	cchars[ccount++] = mc;
	len += mb_char2len(mc);
	ಆದರೆ (ccount == MAX_MCO)
	    ನಿಲ್ಲಿಸು;
    }

    // Check that each composing char in the pattern matches a composing char
    // in the text.  We do not check if all composing chars are matched.
    ಆಗ (sta->c != NFA_ಕೊನೆ_COMPOSING)
    {
	ಗಾಗಿ (j = 0; j < ccount; ++j)
	    ಆದರೆ (cchars[j] == sta->c)
		ನಿಲ್ಲಿಸು;
	ಆದರೆ (j == ccount)
	    ಹಿಂತಿರುಗಿ FAIL;
	sta = sta->ಹೊರಹೋಗು;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Main matching routine.
 *
 * Run NFA to determine whether it matches rex.input.
 *
 * When "nfa_endp" is not NULL it is a required end-of-match position.
 *
 * Return TRUE if there is a match, FALSE if there is no match,
 * NFA_TOO_EXPENSIVE if we end up with too many states.
 * When there is a match "submatch" contains the positions.
 *
 * Note: Caller must ensure that: start != NULL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
nfa_regmatch(
    nfa_regprog_T	*prog,
    nfa_state_T		*ಆರಂಭ,
    regsubs_T		*submatch,
    regsubs_T		*m)
{
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ = FALSE;
    size_t	ಗಾತ್ರ_2 = 0;
    ಪೂರ್ಣಾಂಕ		flag = 0;
    ಪೂರ್ಣಾಂಕ		go_ಗಮ್ಯ_nextline = FALSE;
    nfa_thread_T *t;
    nfa_list_T	list[2];
    ಪೂರ್ಣಾಂಕ		listidx;
    nfa_list_T	*thislist;
    nfa_list_T	*nextlist;
    ಪೂರ್ಣಾಂಕ		*listids = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		listids_len = 0;
    nfa_state_T *ಕೂಡಿಸು_state;
    ಪೂರ್ಣಾಂಕ		ಕೂಡಿಸು_ಇಲ್ಲಿ;
    ಪೂರ್ಣಾಂಕ		ಕೂಡಿಸು_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಂಕ		ಕೂಡಿಸು_off = 0;
    ಪೂರ್ಣಾಂಕ		toplevel = ಆರಂಭ->c == NFA_MOPEN;
    regsubs_T	*r;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NFA_REGEXP_DEBUG_LOG
    FILE	*debug;
#ಆದರೆ_ಅಂತ್ಯ

    // Some patterns may take a long time to match, especially when using
    // recursive_regmatch(). Allow interrupting them with CTRL-C.
    fast_breakcheck();
    ಆದರೆ (got_int)
	ಹಿಂತಿರುಗಿ FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
    ಆದರೆ (nfa_did_ಸಮಯ_ಹೊರಹೋಗು())
	ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NFA_REGEXP_DEBUG_LOG
    debug = fopen(NFA_REGEXP_DEBUG_LOG, "a");
    ಆದರೆ (debug == ಖಾಲಿ)
    {
	semsg("(NFA) COULD NOT OPEN %s!", NFA_REGEXP_DEBUG_LOG);
	ಹಿಂತಿರುಗಿ FALSE;
    }
#ಆದರೆ_ಅಂತ್ಯ
    nfa_ಹೊಂದಿಕೆ = FALSE;

    // Allocate memory for the lists of nodes.
    ಗಾತ್ರ_2 = (prog->nstate + 1) * ಗಾತ್ರ(nfa_thread_T);

    // Reuse cached list buffers from prog when available (top-level call).
    // Recursive calls must allocate their own buffers.
    ಆದರೆ (toplevel && prog->listbuf[0] != ಖಾಲಿ)
    {
	list[0].t = (nfa_thread_T *)prog->listbuf[0];
	list[1].t = (nfa_thread_T *)prog->listbuf[1];
	prog->listbuf[0] = ಖಾಲಿ;
	prog->listbuf[1] = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	list[0].t = alloc(ಗಾತ್ರ_2);
	list[1].t = alloc(ಗಾತ್ರ_2);
    }
    list[0].len = prog->nstate + 1;
    list[1].len = prog->nstate + 1;
    ಆದರೆ (list[0].t == ಖಾಲಿ || list[1].t == ಖಾಲಿ)
	ಹೋಗು theend;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
    log_fd = fopen(NFA_REGEXP_ಸತತ_LOG, "a");
    ಆದರೆ (log_fd == ಖಾಲಿ)
    {
	emsg(_(e_log_open_failed));
	log_fd = stderr;
    }
    fprintf(log_fd, "**********************************\n");
    nfa_ಗಣ_ಸಂಕೇತ(ಆರಂಭ->c);
    fprintf(log_fd, " RUNNING nfa_regmatch() starting with state %d, code %s\n",
    ನಿರಪೇಕ್ಷ(ಆರಂಭ->id), ಸಂಕೇತ_2);
    fprintf(log_fd, "**********************************\n");
#ಆದರೆ_ಅಂತ್ಯ

    thislist = &list[0];
    thislist->n = 0;
    thislist->has_pim = FALSE;
    nextlist = &list[1];
    nextlist->n = 0;
    nextlist->has_pim = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
    fprintf(log_fd, "(---) STARTSTATE first\n");
#ಆದರೆ_ಅಂತ್ಯ
    thislist->id = rex.nfa_listid + 1;

    // Inline optimized code for addstate(thislist, start, m, 0) if we know
    // it's the first MOPEN.
    ಆದರೆ (toplevel)
    {
	ಆದರೆ (REG_MULTI)
	{
	    m->norm.list.multi[0].ಆರಂಭ_lnum = rex.lnum;
	    m->norm.list.multi[0].ಆರಂಭ_ಲಂಬಸಾಲು = (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು);
	    m->norm.orig_ಆರಂಭ_ಲಂಬಸಾಲು = m->norm.list.multi[0].ಆರಂಭ_ಲಂಬಸಾಲು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    m->norm.list.ಸಾಲು[0].ಆರಂಭ = rex.ಒಳಹರಿವು;
	m->norm.ಒಳ_use = 1;
	r = addstate(thislist, ಆರಂಭ->ಹೊರಹೋಗು, m, ಖಾಲಿ, 0, 0);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	r = addstate(thislist, ಆರಂಭ, m, ಖಾಲಿ, 0, 0);
    ಆದರೆ (r == ಖಾಲಿ)
    {
	nfa_ಹೊಂದಿಕೆ = NFA_TOO_EXPENSIVE;
	ಹೋಗು theend;
    }

#ವ್ಯಾಖ್ಯಾನಿಸು	ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(state)	\
    ಆದರೆ (ಫಲಿತಾಂಶ)				\
    {					\
	ಕೂಡಿಸು_state = state->ಹೊರಹೋಗು;		\
	ಕೂಡಿಸು_off = clen;			\
    }

    /*
     * Run for each character.
     */
    ಗಾಗಿ (;;)
    {
	ಪೂರ್ಣಾಂಕ	curc;
	ಪೂರ್ಣಾಂಕ	clen;

	ಆದರೆ (has_mbyte)
	{
	    // Fast path for an ASCII byte not followed by a composing
	    // character, matching the check in utfc_ptr2len().  Avoids two
	    // indirect calls for the common case.
	    ಆದರೆ (rex.ಒಳಹರಿವು[0] != NUL && rex.ಒಳಹರಿವು[0] < 0x80 && rex.ಒಳಹರಿವು[1] < 0x80)
	    {
		curc = rex.ಒಳಹರಿವು[0];
		clen = 1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		curc = (*mb_ptr2char)(rex.ಒಳಹರಿವು);
		clen = (*mb_ptr2len)(rex.ಒಳಹರಿವು);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    curc = *rex.ಒಳಹರಿವು;
	    clen = 1;
	}
	ಆದರೆ (curc == NUL)
	{
	    clen = 0;
	    go_ಗಮ್ಯ_nextline = FALSE;
	}

	// swap lists
	thislist = &list[flag];
	nextlist = &list[flag ^= 1];
	nextlist->n = 0;	    // clear nextlist
	nextlist->has_pim = FALSE;
	++rex.nfa_listid;
	ಆದರೆ (prog->re_engine == AUTOMATIC_ENGINE
		&& (rex.nfa_listid >= NFA_MAX_ಸ್ಥಿತಿಗಳು
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		    || nfa_fail_for_testing
#ಆದರೆ_ಅಂತ್ಯ
		    ))
	{
	    // too many states, retry with old engine
	    nfa_ಹೊಂದಿಕೆ = NFA_TOO_EXPENSIVE;
	    ಹೋಗು theend;
	}

	thislist->id = rex.nfa_listid;
	nextlist->id = rex.nfa_listid + 1;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
	fprintf(log_fd, "------------------------------------------\n");
	fprintf(log_fd, ">>> Reginput is \"%s\"\n", rex.ಒಳಹರಿವು);
	fprintf(log_fd, ">>> Advanced one character... Current char is %c (code %d) \n", curc, (ಪೂರ್ಣಾಂಕ)curc);
	fprintf(log_fd, ">>> Thislist has %d states available: ", thislist->n);
	{
	    ಪೂರ್ಣಾಂಕ i;

	    ಗಾಗಿ (i = 0; i < thislist->n; i++)
		fprintf(log_fd, "%d  ", ನಿರಪೇಕ್ಷ(thislist->t[i].state->id));
	}
	fprintf(log_fd, "\n");
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NFA_REGEXP_DEBUG_LOG
	fprintf(debug, "\n-------------------\n");
#ಆದರೆ_ಅಂತ್ಯ
	/*
	 * If the state lists are empty we can stop.
	 */
	ಆದರೆ (thislist->n == 0)
	    ನಿಲ್ಲಿಸು;

	// compute nextlist
	ಗಾಗಿ (listidx = 0; listidx < thislist->n; ++listidx)
	{
	    // If the list gets very long there probably is something wrong.
	    // At least allow interrupting with CTRL-C.  Only check once in a
	    // while, calling ui_breakcheck() for every state is too slow.
	    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	breakcheck_ಎಣಿಕೆ = 0;  // using "static" makes it faster
	    ಆದರೆ (unlikely(++breakcheck_ಎಣಿಕೆ >= 100000))
	    {
		ui_breakcheck();
		breakcheck_ಎಣಿಕೆ = 0;
	    }
	    ಆದರೆ (got_int)
		ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
	    ಆದರೆ (nfa_did_ಸಮಯ_ಹೊರಹೋಗು())
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	    t = &thislist->t[listidx];

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NFA_REGEXP_DEBUG_LOG
	    nfa_ಗಣ_ಸಂಕೇತ(t->state->c);
	    fprintf(debug, "%s, ", ಸಂಕೇತ_2);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
	    {
		ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು;

		ಆದರೆ (t->subs.norm.ಒಳ_use <= 0)
		    ಲಂಬಸಾಲು = -1;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (REG_MULTI)
		    ಲಂಬಸಾಲು = t->subs.norm.list.multi[0].ಆರಂಭ_ಲಂಬಸಾಲು;
		ಇಲ್ಲದಿದ್ದರೆ
		    ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)(t->subs.norm.list.ಸಾಲು[0].ಆರಂಭ - rex.ಸಾಲು);
		nfa_ಗಣ_ಸಂಕೇತ(t->state->c);
		fprintf(log_fd, "(%d) char %d %s (start col %d)%s... \n",
			ನಿರಪೇಕ್ಷ(t->state->id), (ಪೂರ್ಣಾಂಕ)t->state->c, ಸಂಕೇತ_2, ಲಂಬಸಾಲು,
			pim_info(&t->pim));
	    }
#ಆದರೆ_ಅಂತ್ಯ

	    /*
	     * Handle the possible codes of the current state.
	     * The most important is NFA_MATCH.
	     */
	    ಕೂಡಿಸು_state = ಖಾಲಿ;
	    ಕೂಡಿಸು_ಇಲ್ಲಿ = FALSE;
	    ಕೂಡಿಸು_ಎಣಿಕೆ = 0;
	    ಆಯ್ಕೆ (t->state->c)
	    {
	    ಸ್ಥಿತಿ NFA_ಹೊಂದಿಕೆ:
	      {
		// If the match is not at the start of the line, ends before a
		// composing characters and rex.reg_icombine is not set, that
		// is not really a match.
		ಆದರೆ (enc_utf8 && !rex.reg_icombine
			     && rex.ಒಳಹರಿವು != rex.ಸಾಲು && utf_iscomposing(curc))
		    ನಿಲ್ಲಿಸು;

		nfa_ಹೊಂದಿಕೆ = TRUE;
		copy_sub(&submatch->norm, &t->subs.norm);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
		ಆದರೆ (rex.nfa_has_zsubexpr)
		    copy_sub(&submatch->synt, &t->subs.synt);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
		log_subsexpr(&t->subs);
#ಆದರೆ_ಅಂತ್ಯ
		// Found the left-most longest match, do not look at any other
		// states at this position.  When the list of states is going
		// to be empty quit without advancing, so that "rex.input" is
		// correct.
		ಆದರೆ (nextlist->n == 0)
		    clen = 0;
		ಹೋಗು nextchar;
	      }

	    ಸ್ಥಿತಿ NFA_ಕೊನೆ_INVISIBLE:
	    ಸ್ಥಿತಿ NFA_ಕೊನೆ_INVISIBLE_NEG:
	    ಸ್ಥಿತಿ NFA_ಕೊನೆ_ನಮೂನೆ:
		/*
		 * This is only encountered after a NFA_START_INVISIBLE or
		 * NFA_START_INVISIBLE_BEFORE node.
		 * They surround a zero-width group, used with "\@=", "\&",
		 * "\@!", "\@<=" and "\@<!".
		 * If we got here, it means that the current "invisible" group
		 * finished successfully, so return control to the parent
		 * nfa_regmatch().  For a look-behind match only when it ends
		 * in the position in "nfa_endp".
		 * Submatches are stored in *m, and used in the parent call.
		 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
		ಆದರೆ (nfa_endp != ಖಾಲಿ)
		{
		    ಆದರೆ (REG_MULTI)
			fprintf(log_fd, "Current lnum: %d, endp lnum: %d; current col: %d, endp col: %d\n",
				(ಪೂರ್ಣಾಂಕ)rex.lnum,
				(ಪೂರ್ಣಾಂಕ)nfa_endp->se_u.pos.lnum,
				(ಪೂರ್ಣಾಂಕ)(rex.ಒಳಹರಿವು - rex.ಸಾಲು),
				nfa_endp->se_u.pos.ಲಂಬಸಾಲು);
		    ಇಲ್ಲದಿದ್ದರೆ
			fprintf(log_fd, "Current col: %d, endp col: %d\n",
				(ಪೂರ್ಣಾಂಕ)(rex.ಒಳಹರಿವು - rex.ಸಾಲು),
				(ಪೂರ್ಣಾಂಕ)(nfa_endp->se_u.ptr - rex.ಒಳಹರಿವು));
		}
#ಆದರೆ_ಅಂತ್ಯ
		// If "nfa_endp" is set it's only a match if it ends at
		// "nfa_endp"
		ಆದರೆ (nfa_endp != ಖಾಲಿ && (REG_MULTI
			? (rex.lnum != nfa_endp->se_u.pos.lnum
			    || (ಪೂರ್ಣಾಂಕ)(rex.ಒಳಹರಿವು - rex.ಸಾಲು)
						!= nfa_endp->se_u.pos.ಲಂಬಸಾಲು)
			: rex.ಒಳಹರಿವು != nfa_endp->se_u.ptr))
		    ನಿಲ್ಲಿಸು;

		// do not set submatches for \@!
		ಆದರೆ (t->state->c != NFA_ಕೊನೆ_INVISIBLE_NEG)
		{
		    copy_sub(&m->norm, &t->subs.norm);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
		    ಆದರೆ (rex.nfa_has_zsubexpr)
			copy_sub(&m->synt, &t->subs.synt);
#ಆದರೆ_ಅಂತ್ಯ
		}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
		fprintf(log_fd, "Match found:\n");
		log_subsexpr(m);
#ಆದರೆ_ಅಂತ್ಯ
		nfa_ಹೊಂದಿಕೆ = TRUE;
		// See comment above at "goto nextchar".
		ಆದರೆ (nextlist->n == 0)
		    clen = 0;
		ಹೋಗು nextchar;

	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE:
	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_ಮೊದಲ:
	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_NEG:
	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_NEG_ಮೊದಲ:
	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_BEFORE:
	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_BEFORE_ಮೊದಲ:
	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_BEFORE_NEG:
	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_INVISIBLE_BEFORE_NEG_ಮೊದಲ:
		{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
		    fprintf(log_fd, "Failure chance invisible: %d, what follows: %d\n",
			    failure_chance(t->state->ಹೊರಹೋಗು, 0),
			    failure_chance(t->state->out1->ಹೊರಹೋಗು, 0));
#ಆದರೆ_ಅಂತ್ಯ
		    // Do it directly if there already is a PIM or when
		    // nfa_postprocess() detected it will work better.
		    ಆದರೆ (t->pim.ಫಲಿತಾಂಶ != NFA_PIM_ಬಳಸದ
			 || t->state->c == NFA_ಆರಂಭ_INVISIBLE_ಮೊದಲ
			 || t->state->c == NFA_ಆರಂಭ_INVISIBLE_NEG_ಮೊದಲ
			 || t->state->c == NFA_ಆರಂಭ_INVISIBLE_BEFORE_ಮೊದಲ
			 || t->state->c == NFA_ಆರಂಭ_INVISIBLE_BEFORE_NEG_ಮೊದಲ)
		    {
			ಪೂರ್ಣಾಂಕ ಒಳ_use = m->norm.ಒಳ_use;

			// Copy submatch info for the recursive call, opposite
			// of what happens on success below.
			copy_sub_off(&m->norm, &t->subs.norm);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
			ಆದರೆ (rex.nfa_has_zsubexpr)
			    copy_sub_off(&m->synt, &t->subs.synt);
#ಆದರೆ_ಅಂತ್ಯ

			/*
			 * First try matching the invisible match, then what
			 * follows.
			 */
			ಫಲಿತಾಂಶ = recursive_regmatch(t->state, ಖಾಲಿ, prog,
					  submatch, m, &listids, &listids_len);
			ಆದರೆ (ಫಲಿತಾಂಶ == NFA_TOO_EXPENSIVE)
			{
			    nfa_ಹೊಂದಿಕೆ = ಫಲಿತಾಂಶ;
			    ಹೋಗು theend;
			}

			// for \@! and \@<! it is a match when the result is
			// FALSE
			ಆದರೆ (ಫಲಿತಾಂಶ != (t->state->c == NFA_ಆರಂಭ_INVISIBLE_NEG
			       || t->state->c == NFA_ಆರಂಭ_INVISIBLE_NEG_ಮೊದಲ
			       || t->state->c
					   == NFA_ಆರಂಭ_INVISIBLE_BEFORE_NEG
			       || t->state->c
				     == NFA_ಆರಂಭ_INVISIBLE_BEFORE_NEG_ಮೊದಲ))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&t->subs.norm, &m->norm);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
			    ಆದರೆ (rex.nfa_has_zsubexpr)
				copy_sub_off(&t->subs.synt, &m->synt);
#ಆದರೆ_ಅಂತ್ಯ
			    // If the pattern has \ze and it matched in the
			    // sub pattern, use it.
			    copy_ze_off(&t->subs.norm, &m->norm);

			    // t->state->out1 is the corresponding
			    // END_INVISIBLE node; Add its out to the current
			    // list (zero-width match).
			    ಕೂಡಿಸು_ಇಲ್ಲಿ = TRUE;
			    ಕೂಡಿಸು_state = t->state->out1->ಹೊರಹೋಗು;
			}
			m->norm.ಒಳ_use = ಒಳ_use;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			nfa_pim_T pim;

			/*
			 * First try matching what follows.  Only if a match
			 * is found verify the invisible match matches.  Add a
			 * nfa_pim_T to the following states, it contains info
			 * about the invisible match.
			 */
			pim.state = t->state;
			pim.ಫಲಿತಾಂಶ = NFA_PIM_TODO;
			pim.subs.norm.ಒಳ_use = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
			pim.subs.synt.ಒಳ_use = 0;
#ಆದರೆ_ಅಂತ್ಯ
			ಆದರೆ (REG_MULTI)
			{
			    pim.ಕೊನೆ.pos.ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)(rex.ಒಳಹರಿವು - rex.ಸಾಲು);
			    pim.ಕೊನೆ.pos.lnum = rex.lnum;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			    pim.ಕೊನೆ.ptr = rex.ಒಳಹರಿವು;

			// t->state->out1 is the corresponding END_INVISIBLE
			// node; Add its out to the current list (zero-width
			// match).
			ಆದರೆ (addstate_ಇಲ್ಲಿ(thislist, t->state->out1->ಹೊರಹೋಗು,
					     &t->subs, &pim, &listidx) == ಖಾಲಿ)
			{
			    nfa_ಹೊಂದಿಕೆ = NFA_TOO_EXPENSIVE;
			    ಹೋಗು theend;
			}
		    }
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_ನಮೂನೆ:
	      {
		nfa_state_T *skip = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
		ಪೂರ್ಣಾಂಕ	    skip_lid = 0;
#ಆದರೆ_ಅಂತ್ಯ

		// There is no point in trying to match the pattern if the
		// output state is not going to be added to the list.
		ಆದರೆ (state_ಒಳ_list(nextlist, t->state->out1->ಹೊರಹೋಗು, &t->subs))
		{
		    skip = t->state->out1->ಹೊರಹೋಗು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
		    skip_lid = nextlist->id;
#ಆದರೆ_ಅಂತ್ಯ
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (state_ಒಳ_list(nextlist,
					  t->state->out1->ಹೊರಹೋಗು->ಹೊರಹೋಗು, &t->subs))
		{
		    skip = t->state->out1->ಹೊರಹೋಗು->ಹೊರಹೋಗು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
		    skip_lid = nextlist->id;
#ಆದರೆ_ಅಂತ್ಯ
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (state_ಒಳ_list(thislist,
					  t->state->out1->ಹೊರಹೋಗು->ಹೊರಹೋಗು, &t->subs))
		{
		    skip = t->state->out1->ಹೊರಹೋಗು->ಹೊರಹೋಗು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
		    skip_lid = thislist->id;
#ಆದರೆ_ಅಂತ್ಯ
		}
		ಆದರೆ (skip != ಖಾಲಿ)
		{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
		    nfa_ಗಣ_ಸಂಕೇತ(skip->c);
		    fprintf(log_fd, "> Not trying to match pattern, output state %d is already in list %d. char %d: %s\n",
			    ನಿರಪೇಕ್ಷ(skip->id), skip_lid, skip->c, ಸಂಕೇತ_2);
#ಆದರೆ_ಅಂತ್ಯ
		    ನಿಲ್ಲಿಸು;
		}
		// Copy submatch info to the recursive call, opposite of what
		// happens afterwards.
		copy_sub_off(&m->norm, &t->subs.norm);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
		ಆದರೆ (rex.nfa_has_zsubexpr)
		    copy_sub_off(&m->synt, &t->subs.synt);
#ಆದರೆ_ಅಂತ್ಯ

		// First try matching the pattern.
		ಫಲಿತಾಂಶ = recursive_regmatch(t->state, ಖಾಲಿ, prog,
					  submatch, m, &listids, &listids_len);
		ಆದರೆ (ಫಲಿತಾಂಶ == NFA_TOO_EXPENSIVE)
		{
		    nfa_ಹೊಂದಿಕೆ = ಫಲಿತಾಂಶ;
		    ಹೋಗು theend;
		}
		ಆದರೆ (ಫಲಿತಾಂಶ)
		{
		    ಪೂರ್ಣಾಂಕ bytelen;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN matches:\n");
		    log_subsexpr(m);
#ಆದರೆ_ಅಂತ್ಯ
		    // Copy submatch info from the recursive call
		    copy_sub_off(&t->subs.norm, &m->norm);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
		    ಆದರೆ (rex.nfa_has_zsubexpr)
			copy_sub_off(&t->subs.synt, &m->synt);
#ಆದರೆ_ಅಂತ್ಯ
		    // Now we need to skip over the matched text and then
		    // continue with what follows.
		    ಆದರೆ (REG_MULTI)
			// TODO: multi-line match
			bytelen = m->norm.list.multi[0].ಕೊನೆ_ಲಂಬಸಾಲು
						  - (ಪೂರ್ಣಾಂಕ)(rex.ಒಳಹರಿವು - rex.ಸಾಲು);
		    ಇಲ್ಲದಿದ್ದರೆ
			bytelen = (ಪೂರ್ಣಾಂಕ)(m->norm.list.ಸಾಲು[0].ಕೊನೆ - rex.ಒಳಹರಿವು);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN length: %d\n", bytelen);
#ಆದರೆ_ಅಂತ್ಯ
		    ಆದರೆ (bytelen == 0)
		    {
			// empty match, output of corresponding
			// NFA_END_PATTERN/NFA_SKIP to be used at current
			// position
			ಕೂಡಿಸು_ಇಲ್ಲಿ = TRUE;
			ಕೂಡಿಸು_state = t->state->out1->ಹೊರಹೋಗು->ಹೊರಹೋಗು;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (bytelen <= clen)
		    {
			// match current character, output of corresponding
			// NFA_END_PATTERN to be used at next position.
			ಕೂಡಿಸು_state = t->state->out1->ಹೊರಹೋಗು->ಹೊರಹೋಗು;
			ಕೂಡಿಸು_off = clen;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			ಕೂಡಿಸು_state = t->state->out1->ಹೊರಹೋಗು;
			ಕೂಡಿಸು_off = bytelen;
			ಕೂಡಿಸು_ಎಣಿಕೆ = bytelen - clen;
		    }
		}
		ನಿಲ್ಲಿಸು;
	      }

	    ಸ್ಥಿತಿ NFA_BOL:
		ಆದರೆ (rex.ಒಳಹರಿವು == rex.ಸಾಲು)
		{
		    ಕೂಡಿಸು_ಇಲ್ಲಿ = TRUE;
		    ಕೂಡಿಸು_state = t->state->ಹೊರಹೋಗು;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_EOL:
		ಆದರೆ (curc == NUL)
		{
		    ಕೂಡಿಸು_ಇಲ್ಲಿ = TRUE;
		    ಕೂಡಿಸು_state = t->state->ಹೊರಹೋಗು;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_BOW:
		ಫಲಿತಾಂಶ = TRUE;

		ಆದರೆ (curc == NUL)
		    ಫಲಿತಾಂಶ = FALSE;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte)
		{
		    ಪೂರ್ಣಾಂಕ this_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.ಒಳಹರಿವು, rex.reg_buf);
		    ಆದರೆ (this_class <= 1)
			ಫಲಿತಾಂಶ = FALSE;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (reg_prev_class() == this_class)
			ಫಲಿತಾಂಶ = FALSE;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!vim_iswordc_buf(curc, rex.reg_buf)
			   || (rex.ಒಳಹರಿವು > rex.ಸಾಲು
			       && vim_iswordc_buf(rex.ಒಳಹರಿವು[-1], rex.reg_buf)))
		    ಫಲಿತಾಂಶ = FALSE;
		ಆದರೆ (ಫಲಿತಾಂಶ)
		{
		    ಕೂಡಿಸು_ಇಲ್ಲಿ = TRUE;
		    ಕೂಡಿಸು_state = t->state->ಹೊರಹೋಗು;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_EOW:
		ಫಲಿತಾಂಶ = TRUE;
		ಆದರೆ (rex.ಒಳಹರಿವು == rex.ಸಾಲು)
		    ಫಲಿತಾಂಶ = FALSE;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte)
		{
		    ಪೂರ್ಣಾಂಕ this_class, prev_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.ಒಳಹರಿವು, rex.reg_buf);
		    prev_class = reg_prev_class();
		    ಆದರೆ (this_class == prev_class
					|| prev_class == 0 || prev_class == 1)
			ಫಲಿತಾಂಶ = FALSE;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!vim_iswordc_buf(rex.ಒಳಹರಿವು[-1], rex.reg_buf)
			|| (rex.ಒಳಹರಿವು[0] != NUL
					&& vim_iswordc_buf(curc, rex.reg_buf)))
		    ಫಲಿತಾಂಶ = FALSE;
		ಆದರೆ (ಫಲಿತಾಂಶ)
		{
		    ಕೂಡಿಸು_ಇಲ್ಲಿ = TRUE;
		    ಕೂಡಿಸು_state = t->state->ಹೊರಹೋಗು;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_BOF:
		ಆದರೆ (rex.lnum == 0 && rex.ಒಳಹರಿವು == rex.ಸಾಲು
				     && (!REG_MULTI || rex.reg_firstlnum == 1))
		{
		    ಕೂಡಿಸು_ಇಲ್ಲಿ = TRUE;
		    ಕೂಡಿಸು_state = t->state->ಹೊರಹೋಗು;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_EOF:
		ಆದರೆ (rex.lnum == rex.reg_maxline && curc == NUL)
		{
		    ಕೂಡಿಸು_ಇಲ್ಲಿ = TRUE;
		    ಕೂಡಿಸು_state = t->state->ಹೊರಹೋಗು;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_COMPOSING:
	    {
		nfa_state_T *ಕೊನೆ;

		ಫಲಿತಾಂಶ = ಹೊಂದಿಕೆ_composing(t->state->ಹೊರಹೋಗು, curc, clen);

		ಕೊನೆ = t->state->out1;	    // NFA_END_COMPOSING
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(ಕೊನೆ);
		ನಿಲ್ಲಿಸು;
	    }

	    ಸ್ಥಿತಿ NFA_NEWL:
		ಆದರೆ (curc == NUL && !rex.reg_ಸಾಲು_lbr && REG_MULTI
						&& rex.lnum <= rex.reg_maxline)
		{
		    go_ಗಮ್ಯ_nextline = TRUE;
		    // Pass -1 for the offset, which means taking the position
		    // at the start of the next line.
		    ಕೂಡಿಸು_state = t->state->ಹೊರಹೋಗು;
		    ಕೂಡಿಸು_off = -1;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curc == '\n' && rex.reg_ಸಾಲು_lbr)
		{
		    // match \n as if it is an ordinary character
		    ಕೂಡಿಸು_state = t->state->ಹೊರಹೋಗು;
		    ಕೂಡಿಸು_off = 1;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_COLL:
	    ಸ್ಥಿತಿ NFA_ಆರಂಭ_NEG_COLL:
	      {
		// What follows is a list of characters, until NFA_END_COLL.
		// One of them must match or none of them must match.
		nfa_state_T	*state;
		ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ_if_matched;
		ಪೂರ್ಣಾಂಕ		c1, c2;

		// Never match EOL. If it's part of the collection it is added
		// as a separate state with an OR.
		ಆದರೆ (curc == NUL)
		    ನಿಲ್ಲಿಸು;

		state = t->state->ಹೊರಹೋಗು;
		ಫಲಿತಾಂಶ_if_matched = (t->state->c == NFA_ಆರಂಭ_COLL);
		ಗಾಗಿ (;;)
		{
		    ಆದರೆ (state->c == NFA_COMPOSING)
		    {
			nfa_state_T *ಕೊನೆ;

			ಫಲಿತಾಂಶ = ಹೊಂದಿಕೆ_composing(t->state->ಹೊರಹೋಗು->ಹೊರಹೋಗು, curc,
									clen);

			ಆದರೆ (t->state->ಹೊರಹೋಗು->out1 != ಖಾಲಿ
				&& t->state->ಹೊರಹೋಗು->out1->c == NFA_ಕೊನೆ_COMPOSING)
			{
			    ಕೊನೆ = t->state->ಹೊರಹೋಗು->out1;
			    ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(ಕೊನೆ);
			}
			ನಿಲ್ಲಿಸು;
		    }
		    ಆದರೆ (state->c == NFA_ಕೊನೆ_COLL)
		    {
			ಫಲಿತಾಂಶ = !ಫಲಿತಾಂಶ_if_matched;
			ನಿಲ್ಲಿಸು;
		    }
		    ಆದರೆ (state->c == NFA_RANGE_MIN)
		    {
			c1 = state->val;
			state = state->ಹೊರಹೋಗು; // advance to NFA_RANGE_MAX
			c2 = state->val;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
			fprintf(log_fd, "NFA_RANGE_MIN curc=%d c1=%d c2=%d\n",
				curc, c1, c2);
#ಆದರೆ_ಅಂತ್ಯ
			ಆದರೆ (curc >= c1 && curc <= c2)
			{
			    ಫಲಿತಾಂಶ = ಫಲಿತಾಂಶ_if_matched;
			    ನಿಲ್ಲಿಸು;
			}
			ಆದರೆ (rex.reg_ic)
			{
			    ಪೂರ್ಣಾಂಕ curc_ಕೆಳಗಿನ = MB_CASEFOLD(curc);
			    ಪೂರ್ಣಾಂಕ ಮುಗಿದ = FALSE;

			    ಗಾಗಿ ( ; c1 <= c2; ++c1)
				ಆದರೆ (MB_CASEFOLD(c1) == curc_ಕೆಳಗಿನ)
				{
				    ಫಲಿತಾಂಶ = ಫಲಿತಾಂಶ_if_matched;
				    ಮುಗಿದ = TRUE;
				    ನಿಲ್ಲಿಸು;
				}
			    ಆದರೆ (ಮುಗಿದ)
				ನಿಲ್ಲಿಸು;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (state->c < 0 ? check_char_class(state->c, curc)
			       : (curc == state->c
				   || (rex.reg_ic && MB_CASEFOLD(curc)
						    == MB_CASEFOLD(state->c))))
		    {
			ಫಲಿತಾಂಶ = ಫಲಿತಾಂಶ_if_matched;
			ನಿಲ್ಲಿಸು;
		    }
		    state = state->ಹೊರಹೋಗು;
		}
		ಆದರೆ (ಫಲಿತಾಂಶ)
		{
		    // next state is in out of the NFA_END_COLL, out1 of
		    // START points to the END state
		    ಕೂಡಿಸು_state = t->state->out1->ಹೊರಹೋಗು;
		    ಕೂಡಿಸು_off = clen;
		}
		ನಿಲ್ಲಿಸು;
	      }

	    ಸ್ಥಿತಿ NFA_ANY:
		// Any char except '\0', (end of input) does not match.
		ಆದರೆ (curc > 0)
		{
		    ಕೂಡಿಸು_state = t->state->ಹೊರಹೋಗು;
		    ಕೂಡಿಸು_off = clen;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_ANY_COMPOSING:
		// On a composing character skip over it.  Otherwise do
		// nothing.  Always matches.
		ಆದರೆ (enc_utf8 && utf_iscomposing(curc))
		{
		    ಕೂಡಿಸು_off = clen;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಕೂಡಿಸು_ಇಲ್ಲಿ = TRUE;
		    ಕೂಡಿಸು_off = 0;
		}
		ಕೂಡಿಸು_state = t->state->ಹೊರಹೋಗು;
		ನಿಲ್ಲಿಸು;

	    /*
	     * Character classes like \a for alpha, \d for digit etc.
	     */
	    ಸ್ಥಿತಿ NFA_IDENT:	//  \i
		ಫಲಿತಾಂಶ = vim_isIDc(curc);
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_SIDENT:	//  \I
		ಫಲಿತಾಂಶ = !VIM_ISDIGIT(curc) && vim_isIDc(curc);
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_KWORD:	//  \k
		ಫಲಿತಾಂಶ = vim_iswordp_buf(rex.ಒಳಹರಿವು, rex.reg_buf);
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_SKWORD:	//  \K
		ಫಲಿತಾಂಶ = !VIM_ISDIGIT(curc)
				     && vim_iswordp_buf(rex.ಒಳಹರಿವು, rex.reg_buf);
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_FNAME:	//  \f
		ಫಲಿತಾಂಶ = vim_isfilec(curc);
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_SFNAME:	//  \F
		ಫಲಿತಾಂಶ = !VIM_ISDIGIT(curc) && vim_isfilec(curc);
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_PRINT:	//  \p
		ಫಲಿತಾಂಶ = vim_isprintc(PTR2CHAR(rex.ಒಳಹರಿವು));
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_SPRINT:	//  \P
		ಫಲಿತಾಂಶ = !VIM_ISDIGIT(curc) && vim_isprintc(PTR2CHAR(rex.ಒಳಹರಿವು));
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_WHITE:	//  \s
		ಫಲಿತಾಂಶ = VIM_ISWHITE(curc);
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_NWHITE:	//  \S
		ಫಲಿತಾಂಶ = curc != NUL && !VIM_ISWHITE(curc);
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_DIGIT:	//  \d
		ಫಲಿತಾಂಶ = ri_digit(curc);
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_NDIGIT:	//  \D
		ಫಲಿತಾಂಶ = curc != NUL && !ri_digit(curc);
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_HEX:	//  \x
		ಫಲಿತಾಂಶ = ri_hex(curc);
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_NHEX:	//  \X
		ಫಲಿತಾಂಶ = curc != NUL && !ri_hex(curc);
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_OCTAL:	//  \o
		ಫಲಿತಾಂಶ = ri_octal(curc);
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_NOCTAL:	//  \O
		ಫಲಿತಾಂಶ = curc != NUL && !ri_octal(curc);
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_ಶಬ್ದ:	//  \w
		ಫಲಿತಾಂಶ = ri_ಶಬ್ದ(curc);
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_NWORD:	//  \W
		ಫಲಿತಾಂಶ = curc != NUL && !ri_ಶಬ್ದ(curc);
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_ತಲೆ:	//  \h
		ಫಲಿತಾಂಶ = ri_ತಲೆ(curc);
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_NHEAD:	//  \H
		ಫಲಿತಾಂಶ = curc != NUL && !ri_ತಲೆ(curc);
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_ALPHA:	//  \a
		ಫಲಿತಾಂಶ = ri_alpha(curc);
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_NALPHA:	//  \A
		ಫಲಿತಾಂಶ = curc != NUL && !ri_alpha(curc);
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_ಅಧೋ:	//  \l
		ಫಲಿತಾಂಶ = ri_ಅಧೋ(curc);
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_NLOWER:	//  \L
		ಫಲಿತಾಂಶ = curc != NUL && !ri_ಅಧೋ(curc);
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_ಊರ್ಧ್ವ:	//  \u
		ಫಲಿತಾಂಶ = ri_ಊರ್ಧ್ವ(curc);
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_NUPPER:	// \U
		ಫಲಿತಾಂಶ = curc != NUL && !ri_ಊರ್ಧ್ವ(curc);
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_ಅಧೋ_IC:	// [a-z]
		ಫಲಿತಾಂಶ = ri_ಅಧೋ(curc) || (rex.reg_ic && ri_ಊರ್ಧ್ವ(curc));
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_NLOWER_IC:	// [^a-z]
		ಫಲಿತಾಂಶ = curc != NUL
			&& !(ri_ಅಧೋ(curc) || (rex.reg_ic && ri_ಊರ್ಧ್ವ(curc)));
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_ಊರ್ಧ್ವ_IC:	// [A-Z]
		ಫಲಿತಾಂಶ = ri_ಊರ್ಧ್ವ(curc) || (rex.reg_ic && ri_ಅಧೋ(curc));
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_NUPPER_IC:	// ^[A-Z]
		ಫಲಿತಾಂಶ = curc != NUL
			&& !(ri_ಊರ್ಧ್ವ(curc) || (rex.reg_ic && ri_ಅಧೋ(curc)));
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_BACKREF1:
	    ಸ್ಥಿತಿ NFA_BACKREF2:
	    ಸ್ಥಿತಿ NFA_BACKREF3:
	    ಸ್ಥಿತಿ NFA_BACKREF4:
	    ಸ್ಥಿತಿ NFA_BACKREF5:
	    ಸ್ಥಿತಿ NFA_BACKREF6:
	    ಸ್ಥಿತಿ NFA_BACKREF7:
	    ಸ್ಥಿತಿ NFA_BACKREF8:
	    ಸ್ಥಿತಿ NFA_BACKREF9:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	    ಸ್ಥಿತಿ NFA_ZREF1:
	    ಸ್ಥಿತಿ NFA_ZREF2:
	    ಸ್ಥಿತಿ NFA_ZREF3:
	    ಸ್ಥಿತಿ NFA_ZREF4:
	    ಸ್ಥಿತಿ NFA_ZREF5:
	    ಸ್ಥಿತಿ NFA_ZREF6:
	    ಸ್ಥಿತಿ NFA_ZREF7:
	    ಸ್ಥಿತಿ NFA_ZREF8:
	    ಸ್ಥಿತಿ NFA_ZREF9:
#ಆದರೆ_ಅಂತ್ಯ
		// \1 .. \9  \z1 .. \z9
	      {
		ಪೂರ್ಣಾಂಕ subidx;
		ಪೂರ್ಣಾಂಕ bytelen;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
		ಆದರೆ (t->state->c >= NFA_BACKREF1 && t->state->c <= NFA_BACKREF9)
#ಆದರೆ_ಅಂತ್ಯ
		{
		    subidx = t->state->c - NFA_BACKREF1 + 1;
		    ಫಲಿತಾಂಶ = ಹೊಂದಿಕೆ_backref(&t->subs.norm, subidx, &bytelen);
		}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    subidx = t->state->c - NFA_ZREF1 + 1;
		    ಫಲಿತಾಂಶ = ಹೊಂದಿಕೆ_zref(subidx, &bytelen);
		}
#ಆದರೆ_ಅಂತ್ಯ

		ಆದರೆ (ಫಲಿತಾಂಶ)
		{
		    ಆದರೆ (bytelen == 0)
		    {
			// empty match always works, output of NFA_SKIP to be
			// used next
			ಕೂಡಿಸು_ಇಲ್ಲಿ = TRUE;
			ಕೂಡಿಸು_state = t->state->ಹೊರಹೋಗು->ಹೊರಹೋಗು;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (bytelen <= clen)
		    {
			// match current character, jump ahead to out of
			// NFA_SKIP
			ಕೂಡಿಸು_state = t->state->ಹೊರಹೋಗು->ಹೊರಹೋಗು;
			ಕೂಡಿಸು_off = clen;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			ಕೂಡಿಸು_state = t->state->ಹೊರಹೋಗು;
			ಕೂಡಿಸು_off = bytelen;
			ಕೂಡಿಸು_ಎಣಿಕೆ = bytelen - clen;
		    }
		}
		ನಿಲ್ಲಿಸು;
	      }
	    ಸ್ಥಿತಿ NFA_SKIP:
	      // character of previous matching \1 .. \9  or \@>
	      ಆದರೆ (t->ಎಣಿಕೆ_2 - clen <= 0)
	      {
		  // end of match, go to what follows
		  ಕೂಡಿಸು_state = t->state->ಹೊರಹೋಗು;
		  ಕೂಡಿಸು_off = clen;
	      }
	      ಇಲ್ಲದಿದ್ದರೆ
	      {
		  // add state again with decremented count
		  ಕೂಡಿಸು_state = t->state;
		  ಕೂಡಿಸು_off = 0;
		  ಕೂಡಿಸು_ಎಣಿಕೆ = t->ಎಣಿಕೆ_2 - clen;
	      }
	      ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_LNUM:
	    ಸ್ಥಿತಿ NFA_LNUM_GT:
	    ಸ್ಥಿತಿ NFA_LNUM_LT:
		ಫಲಿತಾಂಶ = (REG_MULTI &&
			nfa_re_ಅಂಕೆ_cmp(t->state->val, t->state->c - NFA_LNUM,
			    (long_u)(rex.lnum + rex.reg_firstlnum)));
		ಆದರೆ (ಫಲಿತಾಂಶ)
		{
		    ಕೂಡಿಸು_ಇಲ್ಲಿ = TRUE;
		    ಕೂಡಿಸು_state = t->state->ಹೊರಹೋಗು;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_ಲಂಬಸಾಲು:
	    ಸ್ಥಿತಿ NFA_ಲಂಬಸಾಲು_GT:
	    ಸ್ಥಿತಿ NFA_ಲಂಬಸಾಲು_LT:
		ಫಲಿತಾಂಶ = nfa_re_ಅಂಕೆ_cmp(t->state->val, t->state->c - NFA_ಲಂಬಸಾಲು,
			(long_u)(rex.ಒಳಹರಿವು - rex.ಸಾಲು) + 1);
		ಆದರೆ (ಫಲಿತಾಂಶ)
		{
		    ಕೂಡಿಸು_ಇಲ್ಲಿ = TRUE;
		    ಕೂಡಿಸು_state = t->state->ಹೊರಹೋಗು;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_VCOL:
	    ಸ್ಥಿತಿ NFA_VCOL_GT:
	    ಸ್ಥಿತಿ NFA_VCOL_LT:
		{
		    ಪೂರ್ಣಾಂಕ     op = t->state->c - NFA_VCOL;
		    colnr_T ಲಂಬಸಾಲು = (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು);
		    win_T   *wp = rex.reg_win == ಖಾಲಿ ? curwin : rex.reg_win;

		    // Bail out quickly when there can't be a match, avoid the
		    // overhead of win_linetabsize() on long lines.
		    ಆದರೆ (op != 1 && ಲಂಬಸಾಲು > t->state->val
			    * (has_mbyte ? MB_MAXBYTES : 1))
			ನಿಲ್ಲಿಸು;
		    ಫಲಿತಾಂಶ = FALSE;
		    ಆದರೆ (op == 1 && ಲಂಬಸಾಲು - 1 > t->state->val && ಲಂಬಸಾಲು > 100)
		    {
			ಪೂರ್ಣಾಂಕ ts = wp->w_ಬಫರ್->b_p_ts;

			// Guess that a character won't use more columns than
			// 'tabstop', with a minimum of 4.
			ಆದರೆ (ts < 4)
			    ts = 4;
			ಫಲಿತಾಂಶ = ಲಂಬಸಾಲು > t->state->val * ts;
		    }
		    ಆದರೆ (!ಫಲಿತಾಂಶ)
		    {
			linenr_T    lnum = REG_MULTI
					    ? rex.reg_firstlnum + rex.lnum : 1;
			long_u	    vcol;

			ಆದರೆ (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ))
			    lnum = 1;
			vcol = (long_u)win_linetabsize(wp, lnum, rex.ಸಾಲು, ಲಂಬಸಾಲು);
			ಫಲಿತಾಂಶ = nfa_re_ಅಂಕೆ_cmp(t->state->val, op, vcol + 1);
		    }
		    ಆದರೆ (ಫಲಿತಾಂಶ)
		    {
			ಕೂಡಿಸು_ಇಲ್ಲಿ = TRUE;
			ಕೂಡಿಸು_state = t->state->ಹೊರಹೋಗು;
		    }
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_MARK:
	    ಸ್ಥಿತಿ NFA_MARK_GT:
	    ಸ್ಥಿತಿ NFA_MARK_LT:
	      {
		pos_T	*pos;
		size_t	ಲಂಬಸಾಲು = REG_MULTI ? rex.ಒಳಹರಿವು - rex.ಸಾಲು : 0;

		pos = getmark_buf(rex.reg_buf, t->state->val, FALSE);

		// Line may have been freed, get it again.
		ಆದರೆ (REG_MULTI)
		{
		    rex.ಸಾಲು = reg_getline(rex.lnum);
		    rex.ಒಳಹರಿವು = rex.ಸಾಲು + ಲಂಬಸಾಲು;
		}

		// Compare the mark position to the match position, if the mark
		// exists and mark is set in reg_buf.
		ಆದರೆ (pos != ಖಾಲಿ && pos->lnum > 0)
		{
		    colnr_T pos_ಲಂಬಸಾಲು = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->ಲಂಬಸಾಲು == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->ಲಂಬಸಾಲು;

		    ಫಲಿತಾಂಶ = (pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_ಲಂಬಸಾಲು == (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು)
				    ? t->state->c == NFA_MARK
				    : (pos_ಲಂಬಸಾಲು < (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು)
					? t->state->c == NFA_MARK_GT
					: t->state->c == NFA_MARK_LT))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? t->state->c == NFA_MARK_GT
				    : t->state->c == NFA_MARK_LT));
		    ಆದರೆ (ಫಲಿತಾಂಶ)
		    {
			ಕೂಡಿಸು_ಇಲ್ಲಿ = TRUE;
			ಕೂಡಿಸು_state = t->state->ಹೊರಹೋಗು;
		    }
		}
		ನಿಲ್ಲಿಸು;
	      }

	    ಸ್ಥಿತಿ NFA_CURSOR:
		ಫಲಿತಾಂಶ = (rex.reg_win != ಖಾಲಿ
			&& (rex.lnum + rex.reg_firstlnum
						 == rex.reg_win->w_cursor.lnum)
			&& ((colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು)
						== rex.reg_win->w_cursor.ಲಂಬಸಾಲು));
		ಆದರೆ (ಫಲಿತಾಂಶ)
		{
		    ಕೂಡಿಸು_ಇಲ್ಲಿ = TRUE;
		    ಕೂಡಿಸು_state = t->state->ಹೊರಹೋಗು;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_VISUAL:
		ಫಲಿತಾಂಶ = reg_ಹೊಂದಿಕೆ_visual();
		ಆದರೆ (ಫಲಿತಾಂಶ)
		{
		    ಕೂಡಿಸು_ಇಲ್ಲಿ = TRUE;
		    ಕೂಡಿಸು_state = t->state->ಹೊರಹೋಗು;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ NFA_MOPEN1:
	    ಸ್ಥಿತಿ NFA_MOPEN2:
	    ಸ್ಥಿತಿ NFA_MOPEN3:
	    ಸ್ಥಿತಿ NFA_MOPEN4:
	    ಸ್ಥಿತಿ NFA_MOPEN5:
	    ಸ್ಥಿತಿ NFA_MOPEN6:
	    ಸ್ಥಿತಿ NFA_MOPEN7:
	    ಸ್ಥಿತಿ NFA_MOPEN8:
	    ಸ್ಥಿತಿ NFA_MOPEN9:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	    ಸ್ಥಿತಿ NFA_ZOPEN:
	    ಸ್ಥಿತಿ NFA_ZOPEN1:
	    ಸ್ಥಿತಿ NFA_ZOPEN2:
	    ಸ್ಥಿತಿ NFA_ZOPEN3:
	    ಸ್ಥಿತಿ NFA_ZOPEN4:
	    ಸ್ಥಿತಿ NFA_ZOPEN5:
	    ಸ್ಥಿತಿ NFA_ZOPEN6:
	    ಸ್ಥಿತಿ NFA_ZOPEN7:
	    ಸ್ಥಿತಿ NFA_ZOPEN8:
	    ಸ್ಥಿತಿ NFA_ZOPEN9:
#ಆದರೆ_ಅಂತ್ಯ
	    ಸ್ಥಿತಿ NFA_NOPEN:
	    ಸ್ಥಿತಿ NFA_ZSTART:
		// These states are only added to be able to bail out when
		// they are added again, nothing is to be done.
		ನಿಲ್ಲಿಸು;

	    ಪೂರ್ವನಿಯೋಜಿತ:	// regular character
	      {
		ಪೂರ್ಣಾಂಕ c = t->state->c;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
		ಆದರೆ (c < 0)
		    siemsg("Negative state char: %ld", (ದೀರ್ಘ)c);
#ಆದರೆ_ಅಂತ್ಯ
		ಫಲಿತಾಂಶ = (c == curc);

		ಆದರೆ (!ಫಲಿತಾಂಶ && rex.reg_ic)
		    ಫಲಿತಾಂಶ = MB_CASEFOLD(c) == MB_CASEFOLD(curc);
		// If rex.reg_icombine is not set only skip over the character
		// itself.  When it is set skip over composing characters.
		ಆದರೆ (ಫಲಿತಾಂಶ && enc_utf8 && !rex.reg_icombine)
		    clen = utf_ptr2len(rex.ಒಳಹರಿವು);
		ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ(t->state);
		ನಿಲ್ಲಿಸು;
	      }

	    } // switch (t->state->c)

	    ಆದರೆ (ಕೂಡಿಸು_state != ಖಾಲಿ)
	    {
		nfa_pim_T *pim;
		nfa_pim_T pim_copy;

		ಆದರೆ (t->pim.ಫಲಿತಾಂಶ == NFA_PIM_ಬಳಸದ)
		    pim = ಖಾಲಿ;
		ಇಲ್ಲದಿದ್ದರೆ
		    pim = &t->pim;

		// Handle the postponed invisible match if the match might end
		// without advancing and before the end of the line.
		ಆದರೆ (pim != ಖಾಲಿ && (clen == 0 || ಹೊಂದಿಕೆ_follows(ಕೂಡಿಸು_state, 0)))
		{
		    ಆದರೆ (pim->ಫಲಿತಾಂಶ == NFA_PIM_TODO)
		    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
			fprintf(log_fd, "\n");
			fprintf(log_fd, "==================================\n");
			fprintf(log_fd, "Postponed recursive nfa_regmatch()\n");
			fprintf(log_fd, "\n");
#ಆದರೆ_ಅಂತ್ಯ
			ಫಲಿತಾಂಶ = recursive_regmatch(pim->state, pim,
				    prog, submatch, m, &listids, &listids_len);
			pim->ಫಲಿತಾಂಶ = ಫಲಿತಾಂಶ ? NFA_PIM_ಹೊಂದಿಕೆ : NFA_PIM_NOMATCH;
			// for \@! and \@<! it is a match when the result is
			// FALSE
			ಆದರೆ (ಫಲಿತಾಂಶ != (pim->state->c == NFA_ಆರಂಭ_INVISIBLE_NEG
			     || pim->state->c == NFA_ಆರಂಭ_INVISIBLE_NEG_ಮೊದಲ
			     || pim->state->c
					   == NFA_ಆರಂಭ_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_ಆರಂಭ_INVISIBLE_BEFORE_NEG_ಮೊದಲ))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&pim->subs.norm, &m->norm);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
			    ಆದರೆ (rex.nfa_has_zsubexpr)
				copy_sub_off(&pim->subs.synt, &m->synt);
#ಆದರೆ_ಅಂತ್ಯ
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಫಲಿತಾಂಶ = (pim->ಫಲಿತಾಂಶ == NFA_PIM_ಹೊಂದಿಕೆ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
			fprintf(log_fd, "\n");
			fprintf(log_fd, "Using previous recursive nfa_regmatch() result, result == %d\n", pim->ಫಲಿತಾಂಶ);
			fprintf(log_fd, "MATCH = %s\n", ಫಲಿತಾಂಶ == TRUE ? "OK" : "FALSE");
			fprintf(log_fd, "\n");
#ಆದರೆ_ಅಂತ್ಯ
		    }

		    // for \@! and \@<! it is a match when result is FALSE
		    ಆದರೆ (ಫಲಿತಾಂಶ != (pim->state->c == NFA_ಆರಂಭ_INVISIBLE_NEG
			     || pim->state->c == NFA_ಆರಂಭ_INVISIBLE_NEG_ಮೊದಲ
			     || pim->state->c
					   == NFA_ಆರಂಭ_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_ಆರಂಭ_INVISIBLE_BEFORE_NEG_ಮೊದಲ))
		    {
			// Copy submatch info from the recursive call
			copy_sub_off(&t->subs.norm, &pim->subs.norm);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
			ಆದರೆ (rex.nfa_has_zsubexpr)
			    copy_sub_off(&t->subs.synt, &pim->subs.synt);
#ಆದರೆ_ಅಂತ್ಯ
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			// look-behind match failed, don't add the state
			ಮುಂದುವರಿ;

		    // Postponed invisible match was handled, don't add it to
		    // following states.
		    pim = ಖಾಲಿ;
		}

		// If "pim" points into l->t it will become invalid when
		// adding the state causes the list to be reallocated.  Make a
		// local copy to avoid that.
		ಆದರೆ (pim == &t->pim)
		{
		    copy_pim(&pim_copy, pim);
		    pim = &pim_copy;
		}

		ಆದರೆ (ಕೂಡಿಸು_ಇಲ್ಲಿ)
		    r = addstate_ಇಲ್ಲಿ(thislist, ಕೂಡಿಸು_state, &t->subs,
								pim, &listidx);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    r = addstate(nextlist, ಕೂಡಿಸು_state, &t->subs, pim, ಕೂಡಿಸು_off, 0);
		    ಆದರೆ (ಕೂಡಿಸು_ಎಣಿಕೆ > 0)
			nextlist->t[nextlist->n - 1].ಎಣಿಕೆ_2 = ಕೂಡಿಸು_ಎಣಿಕೆ;
		}
		ಆದರೆ (r == ಖಾಲಿ)
		{
		    nfa_ಹೊಂದಿಕೆ = NFA_TOO_EXPENSIVE;
		    ಹೋಗು theend;
		}
	    }

	} // for (thislist = thislist; thislist->state; thislist++)

	// Look for the start of a match in the current position by adding the
	// start state to the list of states.
	// The first found match is the leftmost one, thus the order of states
	// matters!
	// Do not add the start state in recursive calls of nfa_regmatch(),
	// because recursive calls should only start in the first position.
	// Unless "nfa_endp" is not NULL, then we match the end position.
	// Also don't start a match past the first line.
	ಆದರೆ (nfa_ಹೊಂದಿಕೆ == FALSE
		&& ((toplevel
			&& rex.lnum == 0
			&& clen != 0
			&& (rex.reg_maxcol == 0
			  || (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು) < rex.reg_maxcol))
		    || (nfa_endp != ಖಾಲಿ
			&& (REG_MULTI
			    ? (rex.lnum < nfa_endp->se_u.pos.lnum
			       || (rex.lnum == nfa_endp->se_u.pos.lnum
				   && (ಪೂರ್ಣಾಂಕ)(rex.ಒಳಹರಿವು - rex.ಸಾಲು)
						    < nfa_endp->se_u.pos.ಲಂಬಸಾಲು))
			    : rex.ಒಳಹರಿವು < nfa_endp->se_u.ptr))))
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
	    fprintf(log_fd, "(---) STARTSTATE\n");
#ಆದರೆ_ಅಂತ್ಯ
	    // Inline optimized code for addstate() if we know the state is
	    // the first MOPEN.
	    ಆದರೆ (toplevel)
	    {
		ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು = TRUE;
		ಪೂರ್ಣಾಂಕ c;

		ಆದರೆ (prog->regstart != NUL && clen != 0)
		{
		    ಆದರೆ (nextlist->n == 0)
		    {
			colnr_T ಲಂಬಸಾಲು = (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು) + clen;

			// Nextlist is empty, we can skip ahead to the
			// character that must appear at the start.
			ಆದರೆ (skip_ಗಮ್ಯ_ಆರಂಭ(prog->regstart, &ಲಂಬಸಾಲು) == FAIL)
			    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
			fprintf(log_fd, "  Skipping ahead %d bytes to regstart\n",
				ಲಂಬಸಾಲು - ((colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು) + clen));
#ಆದರೆ_ಅಂತ್ಯ
			rex.ಒಳಹರಿವು = rex.ಸಾಲು + ಲಂಬಸಾಲು - clen;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// Checking if the required start character matches is
			// cheaper than adding a state that won't match.
			c = PTR2CHAR(rex.ಒಳಹರಿವು + clen);
			ಆದರೆ (c != prog->regstart && (!rex.reg_ic
			     || MB_CASEFOLD(c) != MB_CASEFOLD(prog->regstart)))
			{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
			    fprintf(log_fd, "  Skipping start state, regstart does not match\n");
#ಆದರೆ_ಅಂತ್ಯ
			    ಕೂಡಿಸು = FALSE;
			}
		    }
		}

		ಆದರೆ (ಕೂಡಿಸು)
		{
		    ಆದರೆ (REG_MULTI)
		    {
			m->norm.list.multi[0].ಆರಂಭ_ಲಂಬಸಾಲು =
					 (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು) + clen;
			m->norm.orig_ಆರಂಭ_ಲಂಬಸಾಲು =
					       m->norm.list.multi[0].ಆರಂಭ_ಲಂಬಸಾಲು;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			m->norm.list.ಸಾಲು[0].ಆರಂಭ = rex.ಒಳಹರಿವು + clen;
		    ಆದರೆ (addstate(nextlist, ಆರಂಭ->ಹೊರಹೋಗು, m, ಖಾಲಿ, clen, 0) == ಖಾಲಿ)
		    {
			nfa_ಹೊಂದಿಕೆ = NFA_TOO_EXPENSIVE;
			ಹೋಗು theend;
		    }
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		char_u	    *save_ಸಾಲು = rex.ಸಾಲು;
		char_u	    *save_ಒಳಹರಿವು = rex.ಒಳಹರಿವು;
		linenr_T    save_lnum = rex.lnum;

		// At the end of a line the match can only start on the next
		// line, use that position instead of the line break.
		ಆದರೆ (REG_MULTI && clen == 0 && nfa_endp != ಖಾಲಿ
					 && rex.lnum < nfa_endp->se_u.pos.lnum)
		{
		    char_u  *ಮುಂದಿನ_ಸಾಲು = reg_getline(rex.lnum + 1);

		    ಆದರೆ (ಮುಂದಿನ_ಸಾಲು != ಖಾಲಿ)
		    {
			rex.ಸಾಲು = ಮುಂದಿನ_ಸಾಲು;
			rex.ಒಳಹರಿವು = rex.ಸಾಲು;
			++rex.lnum;
		    }
		}

		r = addstate(nextlist, ಆರಂಭ, m, ಖಾಲಿ, clen, 0);

		rex.ಸಾಲು = save_ಸಾಲು;
		rex.ಒಳಹರಿವು = save_ಒಳಹರಿವು;
		rex.lnum = save_lnum;

		ಆದರೆ (r == ಖಾಲಿ)
		{
		    nfa_ಹೊಂದಿಕೆ = NFA_TOO_EXPENSIVE;
		    ಹೋಗು theend;
		}
	    }
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
	fprintf(log_fd, ">>> Thislist had %d states available: ", thislist->n);
	{
	    ಪೂರ್ಣಾಂಕ i;

	    ಗಾಗಿ (i = 0; i < thislist->n; i++)
		fprintf(log_fd, "%d  ", ನಿರಪೇಕ್ಷ(thislist->t[i].state->id));
	}
	fprintf(log_fd, "\n");
#ಆದರೆ_ಅಂತ್ಯ

nextchar:
	// Advance to the next character, or advance to the next line, or
	// finish.
	ಆದರೆ (clen != 0)
	    rex.ಒಳಹರಿವು += clen;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (go_ಗಮ್ಯ_nextline || (nfa_endp != ಖಾಲಿ && REG_MULTI
					&& rex.lnum < nfa_endp->se_u.pos.lnum))
	    reg_nextline();
	ಇಲ್ಲದಿದ್ದರೆ
	    ನಿಲ್ಲಿಸು;

	// Allow interrupting with CTRL-C.
	ಸಾಲು_breakcheck();
	ಆದರೆ (got_int)
	    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
	ಆದರೆ (nfa_did_ಸಮಯ_ಹೊರಹೋಗು())
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
    ಆದರೆ (log_fd != stderr)
	fclose(log_fd);
    log_fd = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

theend:
    // Cache list buffers in prog for reuse, or free if prog already has
    // cached buffers (recursive call case).
    ಆದರೆ (prog->listbuf[0] == ಖಾಲಿ && list[0].t != ಖಾಲಿ && list[1].t != ಖಾಲಿ)
    {
	prog->listbuf[0] = list[0].t;
	prog->listbuf[1] = list[1].t;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	vim_free(list[0].t);
	vim_free(list[1].t);
    }
    vim_free(listids);
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು ಕೂಡಿಸು_STATE_IF_ಹೊಂದಿಕೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NFA_REGEXP_DEBUG_LOG
    fclose(debug);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ nfa_ಹೊಂದಿಕೆ;
}

/*
 * Try match of "prog" with at rex.line["col"].
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    ಸ್ಥಿರ ದೀರ್ಘ
nfa_regtry(
    nfa_regprog_T   *prog,
    colnr_T	    ಲಂಬಸಾಲು,
    ಪೂರ್ಣಾಂಕ		    *timed_ಹೊರಹೋಗು ಬಳಸದ)	// flag set on timeout or NULL
{
    ಪೂರ್ಣಾಂಕ		i;
    regsubs_T	subs, m;
    nfa_state_T	*ಆರಂಭ = prog->ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
    FILE	*f;
#ಆದರೆ_ಅಂತ್ಯ

    rex.ಒಳಹರಿವು = rex.ಸಾಲು + ಲಂಬಸಾಲು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
    nfa_timed_ಹೊರಹೋಗು = timed_ಹೊರಹೋಗು;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
    f = fopen(NFA_REGEXP_ಸತತ_LOG, "a");
    ಆದರೆ (f != ಖಾಲಿ)
    {
	fprintf(f, "\n\n\t=======================================================\n");
# ifdef DEBUG
	fprintf(f, "\tRegexp is \"%s\"\n", nfa_regengine.ಅಭಿವ್ಯಕ್ತಿ);
# endif
	fprintf(f, "\tInput text is \"%s\" \n", rex.ಒಳಹರಿವು);
	fprintf(f, "\t=======================================================\n\n");
	nfa_print_state(f, ಆರಂಭ);
	fprintf(f, "\n\n");
	fclose(f);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	emsg("Could not open temporary log file for writing");
#ಆದರೆ_ಅಂತ್ಯ

    clear_sub(&subs.norm);
    clear_sub(&m.norm);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    clear_sub(&subs.synt);
    clear_sub(&m.synt);
#ಆದರೆ_ಅಂತ್ಯ

    ಫಲಿತಾಂಶ = nfa_regmatch(prog, ಆರಂಭ, &subs, &m);
    ಆದರೆ (ಫಲಿತಾಂಶ == FALSE)
	ಹಿಂತಿರುಗಿ 0;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಫಲಿತಾಂಶ == NFA_TOO_EXPENSIVE)
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    cleanup_subexpr();
    ಆದರೆ (REG_MULTI)
    {
	ಗಾಗಿ (i = 0; i < subs.norm.ಒಳ_use; i++)
	{
	    rex.reg_startpos[i].lnum = subs.norm.list.multi[i].ಆರಂಭ_lnum;
	    rex.reg_startpos[i].ಲಂಬಸಾಲು = subs.norm.list.multi[i].ಆರಂಭ_ಲಂಬಸಾಲು;

	    rex.reg_endpos[i].lnum = subs.norm.list.multi[i].ಕೊನೆ_lnum;
	    rex.reg_endpos[i].ಲಂಬಸಾಲು = subs.norm.list.multi[i].ಕೊನೆ_ಲಂಬಸಾಲು;
	}
	ಆದರೆ (rex.reg_mmatch != ಖಾಲಿ)
	    rex.reg_mmatch->rmm_matchcol = subs.norm.orig_ಆರಂಭ_ಲಂಬಸಾಲು;

	ಆದರೆ (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
	}
	ಆದರೆ (rex.reg_endpos[0].lnum < 0)
	{
	    // pattern has a \ze but it didn't match, use current end
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)(rex.ಒಳಹರಿವು - rex.ಸಾಲು);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಾಗಿ (i = 0; i < subs.norm.ಒಳ_use; i++)
	{
	    rex.reg_startp[i] = subs.norm.list.ಸಾಲು[i].ಆರಂಭ;
	    rex.reg_endp[i] = subs.norm.list.ಸಾಲು[i].ಕೊನೆ;
	}

	ಆದರೆ (rex.reg_startp[0] == ಖಾಲಿ)
	    rex.reg_startp[0] = rex.ಸಾಲು + ಲಂಬಸಾಲು;
	ಆದರೆ (rex.reg_endp[0] == ಖಾಲಿ)
	    rex.reg_endp[0] = rex.ಒಳಹರಿವು;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_ಹೊರಹೋಗು);
    re_extmatch_ಹೊರಹೋಗು = ಖಾಲಿ;

    ಆದರೆ (prog->reghasz == REX_ಗಣ)
    {
	cleanup_zsubexpr();
	re_extmatch_ಹೊರಹೋಗು = make_extmatch();
	ಆದರೆ (re_extmatch_ಹೊರಹೋಗು == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ 0;
	// Loop over \z1, \z2, etc.  There is no \z0.
	ಗಾಗಿ (i = 1; i < subs.synt.ಒಳ_use; i++)
	{
	    ಆದರೆ (REG_MULTI)
	    {
		ರಚನೆ multipos *mpos = &subs.synt.list.multi[i];

		// Only accept single line matches that are valid.
		ಆದರೆ (mpos->ಆರಂಭ_lnum >= 0
			&& mpos->ಆರಂಭ_lnum == mpos->ಕೊನೆ_lnum
			&& mpos->ಕೊನೆ_ಲಂಬಸಾಲು >= mpos->ಆರಂಭ_ಲಂಬಸಾಲು)
		    re_extmatch_ಹೊರಹೋಗು->matches[i] =
			vim_strnsave(reg_getline(mpos->ಆರಂಭ_lnum)
							    + mpos->ಆರಂಭ_ಲಂಬಸಾಲು,
					     mpos->ಕೊನೆ_ಲಂಬಸಾಲು - mpos->ಆರಂಭ_ಲಂಬಸಾಲು);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ರಚನೆ linepos *lpos = &subs.synt.list.ಸಾಲು[i];

		ಆದರೆ (lpos->ಆರಂಭ != ಖಾಲಿ && lpos->ಕೊನೆ != ಖಾಲಿ)
		    re_extmatch_ಹೊರಹೋಗು->matches[i] =
			    vim_strnsave(lpos->ಆರಂಭ, lpos->ಕೊನೆ - lpos->ಆರಂಭ);
	    }
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 *
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    ಸ್ಥಿರ ದೀರ್ಘ
nfa_regexec_both(
    char_u	*ಸಾಲು,
    colnr_T	startcol,	// column to start looking for match
    ಪೂರ್ಣಾಂಕ		*timed_ಹೊರಹೋಗು)	// flag set on timeout or NULL
{
    nfa_regprog_T   *prog;
    ದೀರ್ಘ	    retval = 0L;
    ಪೂರ್ಣಾಂಕ		    i;
    colnr_T	    ಲಂಬಸಾಲು = startcol;

    ಆದರೆ (REG_MULTI)
    {
	prog = (nfa_regprog_T *)rex.reg_mmatch->regprog;
	ಸಾಲು = reg_getline((linenr_T)0);    // relative to the cursor
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	prog = (nfa_regprog_T *)rex.reg_ಹೊಂದಿಕೆ->regprog;
	rex.reg_startp = rex.reg_ಹೊಂದಿಕೆ->startp;
	rex.reg_endp = rex.reg_ಹೊಂದಿಕೆ->endp;
    }

    // Be paranoid...
    ಆದರೆ (prog == ಖಾಲಿ || ಸಾಲು == ಖಾಲಿ)
    {
	iemsg(e_null_ವಾದ);
	ಹೋಗು theend;
    }

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    ಆದರೆ (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    ಆದರೆ (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    rex.ಸಾಲು = ಸಾಲು;
    rex.lnum = 0;    // relative to line

    rex.nfa_has_zend = prog->has_zend;
    rex.nfa_has_backref = prog->has_backref;
    rex.nfa_nsubexpr = prog->nsubexp;
    rex.nfa_listid = 1;
    rex.nfa_alt_listid = 2;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
    nfa_regengine.ಅಭಿವ್ಯಕ್ತಿ = prog->ನಮೂನೆ;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (prog->reganch && ಲಂಬಸಾಲು > 0)
	ಹಿಂತಿರುಗಿ 0L;

    rex.need_clear_subexpr = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    ಆದರೆ (prog->reghasz == REX_ಗಣ)
    {
	rex.nfa_has_zsubexpr = TRUE;
	rex.need_clear_zsubexpr = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	rex.nfa_has_zsubexpr = FALSE;
	rex.need_clear_zsubexpr = FALSE;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (prog->regstart != NUL)
    {
	// Skip ahead until a character we know the match must start with.
	// When there is none there is no match.
	ಆದರೆ (skip_ಗಮ್ಯ_ಆರಂಭ(prog->regstart, &ಲಂಬಸಾಲು) == FAIL)
	    ಹಿಂತಿರುಗಿ 0L;

	// If match_text is set it contains the full text that must match.
	// Nothing else to try. Doesn't handle combining chars well.
	ಆದರೆ (prog->ಹೊಂದಿಕೆ_ಪಠ್ಯ != ಖಾಲಿ && *prog->ಹೊಂದಿಕೆ_ಪಠ್ಯ != NUL && !rex.reg_icombine)
	{
	    retval = ಕಂಡುಹಿಡಿ_ಹೊಂದಿಕೆ_ಪಠ್ಯ(&ಲಂಬಸಾಲು, prog->regstart, prog->ಹೊಂದಿಕೆ_ಪಠ್ಯ);
	    ಆದರೆ (REG_MULTI)
		rex.reg_mmatch->rmm_matchcol = ಲಂಬಸಾಲು;
	    ಇಲ್ಲದಿದ್ದರೆ
		rex.reg_ಹೊಂದಿಕೆ->rm_matchcol = ಲಂಬಸಾಲು;
	    ಹಿಂತಿರುಗಿ retval;
	}
    }

    // If the start column is past the maximum column: no need to try.
    ಆದರೆ (rex.reg_maxcol > 0 && ಲಂಬಸಾಲು >= rex.reg_maxcol)
	ಹೋಗು theend;

    // Set the "nstate" used by nfa_regcomp() to zero to trigger an error when
    // it's accidentally used during execution.
    nstate = 0;
    ಗಾಗಿ (i = 0; i < prog->nstate; ++i)
    {
	prog->state[i].id = i;
	prog->state[i].lastlist[0] = 0;
	prog->state[i].lastlist[1] = 0;
    }

    retval = nfa_regtry(prog, ಲಂಬಸಾಲು, timed_ಹೊರಹೋಗು);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
    nfa_regengine.ಅಭಿವ್ಯಕ್ತಿ = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

theend:
    ಆದರೆ (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs and
	// \ze are used.
	ಆದರೆ (REG_MULTI)
	{
	    lpos_T *ಆರಂಭ = &rex.reg_mmatch->startpos[0];
	    lpos_T *ಕೊನೆ = &rex.reg_mmatch->endpos[0];

	    ಆದರೆ (ಕೊನೆ->lnum < ಆರಂಭ->lnum
			|| (ಕೊನೆ->lnum == ಆರಂಭ->lnum && ಕೊನೆ->ಲಂಬಸಾಲು < ಆರಂಭ->ಲಂಬಸಾಲು))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (rex.reg_ಹೊಂದಿಕೆ->endp[0] < rex.reg_ಹೊಂದಿಕೆ->startp[0])
		rex.reg_ಹೊಂದಿಕೆ->endp[0] = rex.reg_ಹೊಂದಿಕೆ->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_ಹೊಂದಿಕೆ->rm_matchcol = ಲಂಬಸಾಲು;
	}
    }

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Compile a regular expression into internal code for the NFA matcher.
 * Returns the program in allocated space.  Returns NULL for an error.
 */
    ಸ್ಥಿರ regprog_T *
nfa_regcomp(char_u *ಅಭಿವ್ಯಕ್ತಿ, ಪೂರ್ಣಾಂಕ re_flags)
{
    nfa_regprog_T	*prog = ಖಾಲಿ;
    size_t		prog_ಗಾತ್ರ;
    ಪೂರ್ಣಾಂಕ			*postfix;

    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
    nfa_regengine.ಅಭಿವ್ಯಕ್ತಿ = ಅಭಿವ್ಯಕ್ತಿ;
#ಆದರೆ_ಅಂತ್ಯ
    nfa_re_flags = re_flags;

    init_class_tab();

    ಆದರೆ (nfa_regcomp_ಆರಂಭ(ಅಭಿವ್ಯಕ್ತಿ, re_flags) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Build postfix form of the regexp. Needed to build the NFA
    // (and count its size).
    postfix = re2post();
    ಆದರೆ (postfix == ಖಾಲಿ)
	ಹೋಗು fail;	    // Cascaded (syntax?) error

    /*
     * In order to build the NFA, we parse the input regexp twice:
     * 1. first pass to count size (so we can allocate space)
     * 2. second to emit code
     */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
    {
	FILE *f = fopen(NFA_REGEXP_ಸತತ_LOG, "a");

	ಆದರೆ (f != ಖಾಲಿ)
	{
	    fprintf(f, "\n*****************************\n\n\n\n\tCompiling regexp \"%s\"... hold on !\n", ಅಭಿವ್ಯಕ್ತಿ);
	    fclose(f);
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * PASS 1
     * Count number of NFA states in "nstate". Do not build the NFA.
     */
    post2nfa(postfix, post_ptr, TRUE);

    // allocate the regprog with space for the compiled regexp
    prog_ಗಾತ್ರ = offsetof(nfa_regprog_T, state) + ಗಾತ್ರ(nfa_state_T) * nstate;
    prog = alloc(prog_ಗಾತ್ರ);
    ಆದರೆ (prog == ಖಾಲಿ)
	ಹೋಗು fail;
    state_ptr = prog->state;
    prog->re_ಒಳ_use = FALSE;
    prog->listbuf[0] = ಖಾಲಿ;
    prog->listbuf[1] = ಖಾಲಿ;

    /*
     * PASS 2
     * Build the NFA
     */
    prog->ಆರಂಭ = post2nfa(postfix, post_ptr, FALSE);
    ಆದರೆ (prog->ಆರಂಭ == ಖಾಲಿ)
	ಹೋಗು fail;

    prog->regflags = regflags;
    prog->engine = &nfa_regengine;
    prog->nstate = nstate;
    prog->has_zend = rex.nfa_has_zend;
    prog->has_backref = rex.nfa_has_backref;
    prog->nsubexp = regnpar;

    nfa_postprocess(prog);

    prog->reganch = nfa_get_reganch(prog->ಆರಂಭ, 0);
    prog->regstart = nfa_get_regstart(prog->ಆರಂಭ, 0);
    prog->ಹೊಂದಿಕೆ_ಪಠ್ಯ = nfa_get_ಹೊಂದಿಕೆ_ಪಠ್ಯ(prog->ಆರಂಭ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
    nfa_postfix_dump(ಅಭಿವ್ಯಕ್ತಿ, OK);
    nfa_dump(prog);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    prog->reghasz = re_has_z;
#ಆದರೆ_ಅಂತ್ಯ
    prog->ನಮೂನೆ = vim_strsave(ಅಭಿವ್ಯಕ್ತಿ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
    nfa_regengine.ಅಭಿವ್ಯಕ್ತಿ = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

ಹೊರಹೋಗು:
    // The postfix list and fragment stack are reused by the next compilation
    // (and freed in free_regexp_stuff()), but drop ones that grew large for a
    // big pattern to avoid holding much memory.
    ಆದರೆ (post_ಆರಂಭ_len > NFA_POSTFIX_KEEP)
    {
	VIM_CLEAR(post_ಆರಂಭ);
	post_ಆರಂಭ_len = 0;
    }
    post_ptr = post_ಕೊನೆ = ಖಾಲಿ;
    ಆದರೆ (nfa_ರಾಶಿ_len > NFA_ರಾಶಿ_KEEP)
    {
	VIM_CLEAR(nfa_ರಾಶಿ);
	nfa_ರಾಶಿ_len = 0;
    }
    state_ptr = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ (regprog_T *)prog;

fail:
    VIM_CLEAR(prog);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_LOG
    nfa_postfix_dump(ಅಭಿವ್ಯಕ್ತಿ, FAIL);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
    nfa_regengine.ಅಭಿವ್ಯಕ್ತಿ = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    ಹೋಗು ಹೊರಹೋಗು;
}

/*
 * Free a compiled regexp program, returned by nfa_regcomp().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
nfa_regfree(regprog_T *prog)
{
    ಆದರೆ (prog == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    vim_free(((nfa_regprog_T *)prog)->ಹೊಂದಿಕೆ_ಪಠ್ಯ);
    vim_free(((nfa_regprog_T *)prog)->ನಮೂನೆ);
    vim_free(((nfa_regprog_T *)prog)->listbuf[0]);
    vim_free(((nfa_regprog_T *)prog)->listbuf[1]);
    vim_free(prog);
}

/*
 * Match a regexp against a string.
 * "rmp->regprog" is a compiled regexp as returned by nfa_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 * If "line_lbr" is TRUE consider a "\n" in "line" to be a line break.
 *
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
nfa_regexec_nl(
    regmatch_T	*rmp,
    char_u	*ಸಾಲು,	// string to match against
    colnr_T	ಲಂಬಸಾಲು,	// column to start looking for match
    ಪೂರ್ಣಾಂಕ		ಸಾಲು_lbr)
{
    rex.reg_ಹೊಂದಿಕೆ = rmp;
    rex.reg_mmatch = ಖಾಲಿ;
    rex.reg_maxline = 0;
    rex.reg_ಸಾಲು_lbr = ಸಾಲು_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = ಖಾಲಿ;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;
    ಹಿಂತಿರುಗಿ nfa_regexec_both(ಸಾಲು, ಲಂಬಸಾಲು, ಖಾಲಿ);
}


/*
 * Match a regexp against multiple lines.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 *
 * Return <= 0 if there is no match.  Return number of lines contained in the
 * match otherwise.
 *
 * Note: the body is the same as bt_regexec() except for nfa_regexec_both()
 *
 * ! Also NOTE : match may actually be in another line. e.g.:
 * when r.e. is \nc, cursor is at 'a' and the text buffer looks like
 *
 * +-------------------------+
 * |a                        |
 * |b                        |
 * |c                        |
 * |                         |
 * +-------------------------+
 *
 * then nfa_regexec_multi() returns 3. while the original
 * vim_regexec_multi() returns 0 and a second call at line 2 will return 2.
 *
 * FIXME if this behavior is not compatible.
 */
    ಸ್ಥಿರ ದೀರ್ಘ
nfa_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	ಲಂಬಸಾಲು,		// column to start looking for match
    ಪೂರ್ಣಾಂಕ		*timed_ಹೊರಹೋಗು)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    ಹಿಂತಿರುಗಿ nfa_regexec_both(ಖಾಲಿ, ಲಂಬಸಾಲು, timed_ಹೊರಹೋಗು);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
# undef ENABLE_LOG
#ಆದರೆ_ಅಂತ್ಯ
