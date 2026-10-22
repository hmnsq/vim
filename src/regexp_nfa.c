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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
# define NFA_REGEXP_ERROR_LOG	"nfa_regexp_error.log"
# define ENABLE_LOG
# define NFA_REGEXP_DUMP_LOG	"nfa_regexp_dump.log"
# define NFA_REGEXP_ਲੜੀਵਾਰ_LOG	"nfa_regexp_run.log"
# define NFA_REGEXP_DEBUG_LOG	"nfa_regexp_debug.log"
#ਜੇ_ਅੰਤ

// Added to NFA_ANY - NFA_NUPPER_IC to include a NL.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NFA_ਜੋੜੋ_NL		31

ਗਿਣਤੀ
{
    NFA_ਵੰਡੋ = -1024,
    NFA_ਮੇਲ,
    NFA_ਖਾਲੀ_ਥਾਵਾਂ,			    // matches 0-length

    NFA_ਸ਼ੁਰੂ_COLL,		    // [abc] start
    NFA_ਸਿਰਾ_COLL,		    // [abc] end
    NFA_ਸ਼ੁਰੂ_NEG_COLL,		    // [^abc] start
    NFA_ਸਿਰਾ_NEG_COLL,		    // [^abc] end (postfix only)
    NFA_RANGE,			    // range of the two previous items
				    // (postfix only)
    NFA_RANGE_MIN,		    // low end of a range
    NFA_RANGE_MAX,		    // high end of a range

    NFA_CONCAT,			    // concatenate two previous items (postfix
				    // only)
    NFA_OR,			    // \| (postfix only)
    NFA_ਤਾਰਾ,			    // greedy * (postfix only)
    NFA_ਤਾਰਾ_NONGREEDY,		    // non-greedy * (postfix only)
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
    NFA_ਸ਼ੁਰੂ_INVISIBLE,
    NFA_ਸ਼ੁਰੂ_INVISIBLE_ਪਹਿਲਾ,
    NFA_ਸ਼ੁਰੂ_INVISIBLE_NEG,
    NFA_ਸ਼ੁਰੂ_INVISIBLE_NEG_ਪਹਿਲਾ,
    NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE,
    NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_ਪਹਿਲਾ,
    NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_NEG,
    NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_NEG_ਪਹਿਲਾ,
    NFA_ਸ਼ੁਰੂ_ਨਮੂਨਾ,
    NFA_ਸਿਰਾ_INVISIBLE,
    NFA_ਸਿਰਾ_INVISIBLE_NEG,
    NFA_ਸਿਰਾ_ਨਮੂਨਾ,
    NFA_COMPOSING,		    // Next nodes in NFA are part of the
				    // composing multibyte char
    NFA_ਸਿਰਾ_COMPOSING,		    // End of a composing char in the NFA
    NFA_ANY_COMPOSING,		    // \%C: Any composing characters.
    NFA_OPT_CHARS,		    // \%[abc]

    // The following are used only in the postfix form, not in the NFA
    NFA_PREV_ATOM_NO_ਚੌੜਾਈ,	    // Used for \@=
    NFA_PREV_ATOM_NO_ਚੌੜਾਈ_NEG,	    // Used for \@!
    NFA_PREV_ATOM_JUST_BEFORE,	    // Used for \@<=
    NFA_PREV_ATOM_JUST_BEFORE_NEG,  // Used for \@<!
    NFA_PREV_ATOM_LIKE_ਨਮੂਨਾ,	    // Used for \@>

    NFA_BACKREF1,		    // \1
    NFA_BACKREF2,		    // \2
    NFA_BACKREF3,		    // \3
    NFA_BACKREF4,		    // \4
    NFA_BACKREF5,		    // \5
    NFA_BACKREF6,		    // \6
    NFA_BACKREF7,		    // \7
    NFA_BACKREF8,		    // \8
    NFA_BACKREF9,		    // \9
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    NFA_ZREF1,			    // \z1
    NFA_ZREF2,			    // \z2
    NFA_ZREF3,			    // \z3
    NFA_ZREF4,			    // \z4
    NFA_ZREF5,			    // \z5
    NFA_ZREF6,			    // \z6
    NFA_ZREF7,			    // \z7
    NFA_ZREF8,			    // \z8
    NFA_ZREF9,			    // \z9
#ਜੇ_ਅੰਤ
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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
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
#ਜੇ_ਅੰਤ

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
    NFA_ਸ਼ਬਦ,		//	Match word char
    NFA_NWORD,		//	Match non-word char
    NFA_ਸਿਰ,		//	Match head char
    NFA_NHEAD,		//	Match non-head char
    NFA_ALPHA,		//	Match alpha char
    NFA_NALPHA,		//	Match non-alpha char
    NFA_ਹੇਠਲਾ,		//	Match lowercase char
    NFA_NLOWER,		//	Match non-lowercase char
    NFA_ਉੱਤਲਾ,		//	Match uppercase char
    NFA_NUPPER,		//	Match non-uppercase char
    NFA_ਹੇਠਲਾ_IC,	//	Match [a-z]
    NFA_NLOWER_IC,	//	Match [^a-z]
    NFA_ਉੱਤਲਾ_IC,	//	Match [A-Z]
    NFA_NUPPER_IC,	//	Match [^A-Z]

    NFA_ਪਹਿਲਾ_NL = NFA_ANY + NFA_ਜੋੜੋ_NL,
    NFA_ਆਖਰੀ_NL = NFA_NUPPER_IC + NFA_ਜੋੜੋ_NL,

    NFA_CURSOR,		//	Match cursor pos
    NFA_LNUM,		//	Match line number
    NFA_LNUM_GT,	//	Match > line number
    NFA_LNUM_LT,	//	Match < line number
    NFA_ਖਾਨਾ,		//	Match cursor column
    NFA_ਖਾਨਾ_GT,		//	Match > cursor column
    NFA_ਖਾਨਾ_LT,		//	Match < cursor column
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
    NFA_CLASS_ਗ੍ਰਾਫ,
    NFA_CLASS_ਹੇਠਲਾ,
    NFA_CLASS_PRINT,
    NFA_CLASS_PUNCT,
    NFA_CLASS_SPACE,
    NFA_CLASS_ਉੱਤਲਾ,
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
ਸਥਿਰ ਅਚਲ ਪੂਰਨ nfa_classcodes[] = {
    NFA_ANY, NFA_IDENT, NFA_SIDENT, NFA_KWORD,NFA_SKWORD,
    NFA_FNAME, NFA_SFNAME, NFA_PRINT, NFA_SPRINT,
    NFA_WHITE, NFA_NWHITE, NFA_DIGIT, NFA_NDIGIT,
    NFA_HEX, NFA_NHEX, NFA_OCTAL, NFA_NOCTAL,
    NFA_ਸ਼ਬਦ, NFA_NWORD, NFA_ਸਿਰ, NFA_NHEAD,
    NFA_ALPHA, NFA_NALPHA, NFA_ਹੇਠਲਾ, NFA_NLOWER,
    NFA_ਉੱਤਲਾ, NFA_NUPPER
};

// Variables only used in nfa_regcomp() and descendants.
ਸਥਿਰ ਪੂਰਨ nfa_re_flags; // re_flags passed to nfa_regcomp()
ਸਥਿਰ ਪੂਰਨ *post_ਸ਼ੁਰੂ;  // holds the postfix form of r.e.
ਸਥਿਰ ਪੂਰਨ post_ਸ਼ੁਰੂ_len;  // size of allocated post_start (in ints)
ਸਥਿਰ ਪੂਰਨ *post_ਸਿਰਾ;
ਸਥਿਰ ਪੂਰਨ *post_ptr;
ਸਥਿਰ ਪੂਰਨ nfa_reg_parse_ਡੂੰਘਾਈ;	// nesting depth in nfa_reg()

// The postfix list (post_start) and fragment stack (nfa_stack) are reused
// across compilations; a buffer that grew past these sizes for a big pattern
// is freed afterwards instead of being kept, to avoid holding much memory.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NFA_POSTFIX_KEEP	10000	// number of ints (~40 Kbyte)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NFA_ਢੇਰ_KEEP		4000	// number of Frag_T (~64 Kbyte)

// Set when the pattern should use the NFA engine.
// E.g. [[:upper:]] only allows 8bit characters for BT engine,
// while NFA engine handles multibyte characters correctly.
ਸਥਿਰ ਪੂਰਨ ਚਾਹੁੰਦਾ_nfa;

ਸਥਿਰ ਪੂਰਨ nstate;	// Number of states in the NFA.
ਸਥਿਰ ਪੂਰਨ istate;	// Index in the state vector, used in alloc_state()

// If not NULL match must end at this position
ਸਥਿਰ save_se_T *nfa_endp = ਨਿਰਾ;

// 0 for first call to nfa_regmatch(), 1 for recursive call.
ਸਥਿਰ ਪੂਰਨ nfa_ll_ਸੂਚਕਾਂਕ = 0;

ਸਥਿਰ ਪੂਰਨ realloc_post_list(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ nfa_reg(ਪੂਰਨ paren);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
ਸਥਿਰ ਖਾਲੀ nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਪੂਰਨ ਮੇਲ_follows(nfa_state_T *startstate, ਪੂਰਨ ਡੂੰਘਾਈ);
ਸਥਿਰ ਪੂਰਨ failure_chance(nfa_state_T *state, ਪੂਰਨ ਡੂੰਘਾਈ);

// helper functions used when doing re2post() ... regatom() parsing
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EMIT(c)	ਕਰੋ {				\
		    ਜੇ (post_ptr >= post_ਸਿਰਾ && realloc_post_list() == FAIL) \
			ਵਾਪਸ FAIL;		\
		    *post_ptr++ = c;		\
		} ਜਦੋਂ (0)

/*
 * Initialize internal variables before NFA compilation.
 * Return OK on success, FAIL otherwise.
 */
    ਸਥਿਰ ਪੂਰਨ
nfa_regcomp_ਸ਼ੁਰੂ(
    char_u	*ਵਿਅੰਜਕ,
    ਪੂਰਨ		re_flags)	    // see vim_regcomp()
{
    size_t	postfix_ਮਾਪ;
    ਪੂਰਨ		nstate_max;

    nstate = 0;
    istate = 0;
    // A reasonable estimation for maximum size
    nstate_max = (ਪੂਰਨ)(STRLEN(ਵਿਅੰਜਕ) + 1) * 25;

    // Some items blow up in size, such as [A-z].  Add more space for that.
    // When it is still not enough realloc_post_list() will be used.
    nstate_max += 1000;

    // Size for postfix representation of expr.
    postfix_ਮਾਪ = ਆਕਾਰ(ਪੂਰਨ) * nstate_max;

    // Reuse the postfix buffer across compilations, only growing it when the
    // estimate exceeds the current size; it is freed in free_regexp_stuff().
    ਜੇ (post_ਸ਼ੁਰੂ == ਨਿਰਾ || post_ਸ਼ੁਰੂ_len < nstate_max)
    {
	vim_free(post_ਸ਼ੁਰੂ);
	post_ਸ਼ੁਰੂ = alloc(postfix_ਮਾਪ);
	ਜੇ (post_ਸ਼ੁਰੂ == ਨਿਰਾ)
	{
	    post_ਸ਼ੁਰੂ_len = 0;
	    ਵਾਪਸ FAIL;
	}
	post_ਸ਼ੁਰੂ_len = nstate_max;
    }
    post_ptr = post_ਸ਼ੁਰੂ;
    post_ਸਿਰਾ = post_ਸ਼ੁਰੂ + post_ਸ਼ੁਰੂ_len;
    ਚਾਹੁੰਦਾ_nfa = FALSE;
    rex.nfa_has_zend = FALSE;
    rex.nfa_has_backref = FALSE;
    nfa_reg_parse_ਡੂੰਘਾਈ = 0;

    // shared with BT engine
    regcomp_ਸ਼ੁਰੂ(ਵਿਅੰਜਕ, re_flags);

    ਵਾਪਸ OK;
}

/*
 * Figure out if the NFA state list starts with an anchor, must match at start
 * of the line.
 */
    ਸਥਿਰ ਪੂਰਨ
nfa_get_reganch(nfa_state_T *ਸ਼ੁਰੂ, ਪੂਰਨ ਡੂੰਘਾਈ)
{
    nfa_state_T *p = ਸ਼ੁਰੂ;

    ਜੇ (ਡੂੰਘਾਈ > 4)
	ਵਾਪਸ 0;

    ਜਦੋਂ (p != ਨਿਰਾ)
    {
	ਚੋਣ (p->c)
	{
	    ਹਾਲਤ NFA_BOL:
	    ਹਾਲਤ NFA_BOF:
		ਵਾਪਸ 1; // yes!

	    ਹਾਲਤ NFA_ZSTART:
	    ਹਾਲਤ NFA_ZEND:
	    ਹਾਲਤ NFA_CURSOR:
	    ਹਾਲਤ NFA_VISUAL:

	    ਹਾਲਤ NFA_MOPEN:
	    ਹਾਲਤ NFA_MOPEN1:
	    ਹਾਲਤ NFA_MOPEN2:
	    ਹਾਲਤ NFA_MOPEN3:
	    ਹਾਲਤ NFA_MOPEN4:
	    ਹਾਲਤ NFA_MOPEN5:
	    ਹਾਲਤ NFA_MOPEN6:
	    ਹਾਲਤ NFA_MOPEN7:
	    ਹਾਲਤ NFA_MOPEN8:
	    ਹਾਲਤ NFA_MOPEN9:
	    ਹਾਲਤ NFA_NOPEN:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	    ਹਾਲਤ NFA_ZOPEN:
	    ਹਾਲਤ NFA_ZOPEN1:
	    ਹਾਲਤ NFA_ZOPEN2:
	    ਹਾਲਤ NFA_ZOPEN3:
	    ਹਾਲਤ NFA_ZOPEN4:
	    ਹਾਲਤ NFA_ZOPEN5:
	    ਹਾਲਤ NFA_ZOPEN6:
	    ਹਾਲਤ NFA_ZOPEN7:
	    ਹਾਲਤ NFA_ZOPEN8:
	    ਹਾਲਤ NFA_ZOPEN9:
#ਜੇ_ਅੰਤ
		p = p->ਬਾਹਰਲਾ;
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_ਵੰਡੋ:
		ਵਾਪਸ nfa_get_reganch(p->ਬਾਹਰਲਾ, ਡੂੰਘਾਈ + 1)
				       && nfa_get_reganch(p->out1, ਡੂੰਘਾਈ + 1);

	    ਮੂਲ:
		ਵਾਪਸ 0; // noooo
	}
    }
    ਵਾਪਸ 0;
}

/*
 * Figure out if the NFA state list starts with a character which must match
 * at start of the match.
 */
    ਸਥਿਰ ਪੂਰਨ
nfa_get_regstart(nfa_state_T *ਸ਼ੁਰੂ, ਪੂਰਨ ਡੂੰਘਾਈ)
{
    nfa_state_T *p = ਸ਼ੁਰੂ;

    ਜੇ (ਡੂੰਘਾਈ > 4)
	ਵਾਪਸ 0;

    ਜਦੋਂ (p != ਨਿਰਾ)
    {
	ਚੋਣ (p->c)
	{
	    // all kinds of zero-width matches
	    ਹਾਲਤ NFA_BOL:
	    ਹਾਲਤ NFA_BOF:
	    ਹਾਲਤ NFA_BOW:
	    ਹਾਲਤ NFA_EOW:
	    ਹਾਲਤ NFA_ZSTART:
	    ਹਾਲਤ NFA_ZEND:
	    ਹਾਲਤ NFA_CURSOR:
	    ਹਾਲਤ NFA_VISUAL:
	    ਹਾਲਤ NFA_LNUM:
	    ਹਾਲਤ NFA_LNUM_GT:
	    ਹਾਲਤ NFA_LNUM_LT:
	    ਹਾਲਤ NFA_ਖਾਨਾ:
	    ਹਾਲਤ NFA_ਖਾਨਾ_GT:
	    ਹਾਲਤ NFA_ਖਾਨਾ_LT:
	    ਹਾਲਤ NFA_VCOL:
	    ਹਾਲਤ NFA_VCOL_GT:
	    ਹਾਲਤ NFA_VCOL_LT:
	    ਹਾਲਤ NFA_MARK:
	    ਹਾਲਤ NFA_MARK_GT:
	    ਹਾਲਤ NFA_MARK_LT:

	    ਹਾਲਤ NFA_MOPEN:
	    ਹਾਲਤ NFA_MOPEN1:
	    ਹਾਲਤ NFA_MOPEN2:
	    ਹਾਲਤ NFA_MOPEN3:
	    ਹਾਲਤ NFA_MOPEN4:
	    ਹਾਲਤ NFA_MOPEN5:
	    ਹਾਲਤ NFA_MOPEN6:
	    ਹਾਲਤ NFA_MOPEN7:
	    ਹਾਲਤ NFA_MOPEN8:
	    ਹਾਲਤ NFA_MOPEN9:
	    ਹਾਲਤ NFA_NOPEN:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	    ਹਾਲਤ NFA_ZOPEN:
	    ਹਾਲਤ NFA_ZOPEN1:
	    ਹਾਲਤ NFA_ZOPEN2:
	    ਹਾਲਤ NFA_ZOPEN3:
	    ਹਾਲਤ NFA_ZOPEN4:
	    ਹਾਲਤ NFA_ZOPEN5:
	    ਹਾਲਤ NFA_ZOPEN6:
	    ਹਾਲਤ NFA_ZOPEN7:
	    ਹਾਲਤ NFA_ZOPEN8:
	    ਹਾਲਤ NFA_ZOPEN9:
#ਜੇ_ਅੰਤ
		p = p->ਬਾਹਰਲਾ;
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_ਵੰਡੋ:
	    {
		ਪੂਰਨ c1 = nfa_get_regstart(p->ਬਾਹਰਲਾ, ਡੂੰਘਾਈ + 1);
		ਪੂਰਨ c2 = nfa_get_regstart(p->out1, ਡੂੰਘਾਈ + 1);

		ਜੇ (c1 == c2)
		    ਵਾਪਸ c1; // yes!
		ਵਾਪਸ 0;
	    }

	    ਮੂਲ:
		ਜੇ (p->c > 0)
		    ਵਾਪਸ p->c; // yes!
		ਵਾਪਸ 0;
	}
    }
    ਵਾਪਸ 0;
}

/*
 * Figure out if the NFA state list contains just literal text and nothing
 * else.  If so return a string in allocated memory with what must match after
 * regstart.  Otherwise return NULL.
 */
    ਸਥਿਰ char_u *
nfa_get_ਮੇਲ_ਪਾਠ(nfa_state_T *ਸ਼ੁਰੂ)
{
    nfa_state_T *p = ਸ਼ੁਰੂ;
    ਪੂਰਨ		len = 0;
    char_u	*ret;
    char_u	*s;

    ਜੇ (p->c != NFA_MOPEN)
	ਵਾਪਸ ਨਿਰਾ; // just in case
    p = p->ਬਾਹਰਲਾ;
    ਜਦੋਂ (p->c > 0)
    {
	len += MB_CHAR2LEN(p->c);
	p = p->ਬਾਹਰਲਾ;
    }
    ਜੇ (p->c != NFA_MCLOSE || p->ਬਾਹਰਲਾ->c != NFA_ਮੇਲ)
	ਵਾਪਸ ਨਿਰਾ;

    ret = alloc(len);
    ਜੇ (ret == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    p = ਸ਼ੁਰੂ->ਬਾਹਰਲਾ->ਬਾਹਰਲਾ; // skip first char, it goes into regstart
    s = ret;
    ਜਦੋਂ (p->c > 0)
    {
	ਜੇ (has_mbyte)
	    s += (*mb_char2bytes)(p->c, s);
	ਨਹੀਂਤਾਂ
	    *s++ = p->c;
	p = p->ਬਾਹਰਲਾ;
    }
    *s = NUL;
    ਵਾਪਸ ret;
}

/*
 * Allocate more space for post_start.  Called when
 * running above the estimated number of states.
 */
    ਸਥਿਰ ਪੂਰਨ
realloc_post_list(ਖਾਲੀ)
{
    ਪੂਰਨ   nstate_max = (ਪੂਰਨ)(post_ਸਿਰਾ - post_ਸ਼ੁਰੂ);
    ਪੂਰਨ   new_max;
    ਪੂਰਨ   *new_ਸ਼ੁਰੂ;
    ਪੂਰਨ	  *old_ਸ਼ੁਰੂ;

    // For weird patterns the number of states can be very high. Increasing by
    // 50% seems a reasonable compromise between memory use and speed.
    new_max = nstate_max * 3 / 2;
    new_ਸ਼ੁਰੂ = ALLOC_MULT(ਪੂਰਨ, new_max);
    ਜੇ (new_ਸ਼ੁਰੂ == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    mch_memmove(new_ਸ਼ੁਰੂ, post_ਸ਼ੁਰੂ, nstate_max * ਆਕਾਰ(ਪੂਰਨ));
    old_ਸ਼ੁਰੂ = post_ਸ਼ੁਰੂ;
    post_ਸ਼ੁਰੂ = new_ਸ਼ੁਰੂ;
    post_ਸ਼ੁਰੂ_len = new_max;
    post_ptr = new_ਸ਼ੁਰੂ + (post_ptr - old_ਸ਼ੁਰੂ);
    post_ਸਿਰਾ = post_ਸ਼ੁਰੂ + new_max;
    vim_free(old_ਸ਼ੁਰੂ);
    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
nfa_recognize_char_class(char_u *ਸ਼ੁਰੂ, char_u *ਸਿਰਾ, ਪੂਰਨ extra_newl)
{
    ਪੂਰਨ		newl;
    ਪੂਰਨ		config = get_char_class_ਬਿੱਟ(ਸ਼ੁਰੂ, ਸਿਰਾ, &newl);

    ਜੇ (config < 0)
	ਵਾਪਸ FAIL;

    ਜੇ (extra_newl == TRUE)
	newl = TRUE;

    ਜੇ (newl == TRUE)
	extra_newl = NFA_ਜੋੜੋ_NL;

    ਚੋਣ (config)
    {
	ਹਾਲਤ CLASS_o9:
	    ਵਾਪਸ extra_newl + NFA_DIGIT;
	ਹਾਲਤ CLASS_not |  CLASS_o9:
	    ਵਾਪਸ extra_newl + NFA_NDIGIT;
	ਹਾਲਤ CLASS_af | CLASS_AF | CLASS_o9:
	    ਵਾਪਸ extra_newl + NFA_HEX;
	ਹਾਲਤ CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    ਵਾਪਸ extra_newl + NFA_NHEX;
	ਹਾਲਤ CLASS_o7:
	    ਵਾਪਸ extra_newl + NFA_OCTAL;
	ਹਾਲਤ CLASS_not | CLASS_o7:
	    ਵਾਪਸ extra_newl + NFA_NOCTAL;
	ਹਾਲਤ CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    ਵਾਪਸ extra_newl + NFA_ਸ਼ਬਦ;
	ਹਾਲਤ CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    ਵਾਪਸ extra_newl + NFA_NWORD;
	ਹਾਲਤ CLASS_az | CLASS_AZ | CLASS_underscore:
	    ਵਾਪਸ extra_newl + NFA_ਸਿਰ;
	ਹਾਲਤ CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    ਵਾਪਸ extra_newl + NFA_NHEAD;
	ਹਾਲਤ CLASS_az | CLASS_AZ:
	    ਵਾਪਸ extra_newl + NFA_ALPHA;
	ਹਾਲਤ CLASS_not | CLASS_az | CLASS_AZ:
	    ਵਾਪਸ extra_newl + NFA_NALPHA;
	ਹਾਲਤ CLASS_az:
	   ਵਾਪਸ extra_newl + NFA_ਹੇਠਲਾ_IC;
	ਹਾਲਤ CLASS_not | CLASS_az:
	    ਵਾਪਸ extra_newl + NFA_NLOWER_IC;
	ਹਾਲਤ CLASS_AZ:
	    ਵਾਪਸ extra_newl + NFA_ਉੱਤਲਾ_IC;
	ਹਾਲਤ CLASS_not | CLASS_AZ:
	    ਵਾਪਸ extra_newl + NFA_NUPPER_IC;
    }
    ਵਾਪਸ FAIL;
}

/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * Emits bytes in postfix notation: 'a,b,NFA_OR,c,NFA_OR' is
 * equivalent to 'a OR b OR c'
 *
 * NOTE! When changing this function, also update reg_equi_class()
 */
    ਸਥਿਰ ਪੂਰਨ
nfa_emit_equi_class(ਪੂਰਨ c)
{
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EMIT2(c)    EMIT(c); EMIT(NFA_CONCAT);

    ਜੇ (enc_utf8 || STRCMP(p_enc, "latin1") == 0
					 || STRCMP(p_enc, "iso-8859-15") == 0)
    {
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ A_grave 0xc0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ A_acute 0xc1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ A_circumflex 0xc2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ A_virguilla 0xc3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ A_diaeresis 0xc4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ A_ring 0xc5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ C_cedilla 0xc7
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ E_grave 0xc8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ E_acute 0xc9
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ E_circumflex 0xca
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ E_diaeresis 0xcb
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ I_grave 0xcc
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ I_acute 0xcd
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ I_circumflex 0xce
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ I_diaeresis 0xcf
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ N_virguilla 0xd1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ O_grave 0xd2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ O_acute 0xd3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ O_circumflex 0xd4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ O_virguilla 0xd5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ O_diaeresis 0xd6
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ O_slash 0xd8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ U_grave 0xd9
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ U_acute 0xda
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ U_circumflex 0xdb
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ U_diaeresis 0xdc
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Y_acute 0xdd
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ a_grave 0xe0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ a_acute 0xe1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ a_circumflex 0xe2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ a_virguilla 0xe3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ a_diaeresis 0xe4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ a_ring 0xe5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ c_cedilla 0xe7
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ e_grave 0xe8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ e_acute 0xe9
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ e_circumflex 0xea
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ e_diaeresis 0xeb
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ i_grave 0xec
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ i_acute 0xed
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ i_circumflex 0xee
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ i_diaeresis 0xef
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ n_virguilla 0xf1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ o_grave 0xf2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ o_acute 0xf3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ o_circumflex 0xf4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ o_virguilla 0xf5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ o_diaeresis 0xf6
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ o_slash 0xf8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ u_grave 0xf9
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ u_acute 0xfa
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ u_circumflex 0xfb
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ u_diaeresis 0xfc
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ y_acute 0xfd
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ y_diaeresis 0xff
	ਚੋਣ (c)
	{
	    ਹਾਲਤ 'A': ਹਾਲਤ A_grave: ਹਾਲਤ A_acute: ਹਾਲਤ A_circumflex:
	    ਹਾਲਤ A_virguilla: ਹਾਲਤ A_diaeresis: ਹਾਲਤ A_ring:
	    ਹਾਲਤ 0x100: ਹਾਲਤ 0x102: ਹਾਲਤ 0x104: ਹਾਲਤ 0x1cd:
	    ਹਾਲਤ 0x1de: ਹਾਲਤ 0x1e0: ਹਾਲਤ 0x1fa: ਹਾਲਤ 0x200:
	    ਹਾਲਤ 0x202: ਹਾਲਤ 0x226: ਹਾਲਤ 0x23a: ਹਾਲਤ 0x1e00:
	    ਹਾਲਤ 0x1ea0: ਹਾਲਤ 0x1ea2: ਹਾਲਤ 0x1ea4: ਹਾਲਤ 0x1ea6:
	    ਹਾਲਤ 0x1ea8: ਹਾਲਤ 0x1eaa: ਹਾਲਤ 0x1eac: ਹਾਲਤ 0x1eae:
	    ਹਾਲਤ 0x1eb0: ਹਾਲਤ 0x1eb2: ਹਾਲਤ 0x1eb4: ਹਾਲਤ 0x1eb6:
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
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'B': ਹਾਲਤ 0x181: ਹਾਲਤ 0x243: ਹਾਲਤ 0x1e02:
	    ਹਾਲਤ 0x1e04: ਹਾਲਤ 0x1e06:
		    EMIT2('B')
		    EMIT2(0x181) EMIT2(0x243) EMIT2(0x1e02)
		    EMIT2(0x1e04) EMIT2(0x1e06)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'C': ਹਾਲਤ C_cedilla: ਹਾਲਤ 0x106: ਹਾਲਤ 0x108:
	    ਹਾਲਤ 0x10a: ਹਾਲਤ 0x10c: ਹਾਲਤ 0x187: ਹਾਲਤ 0x23b:
	    ਹਾਲਤ 0x1e08: ਹਾਲਤ 0xa792:
		    EMIT2('C') EMIT2(C_cedilla)
		    EMIT2(0x106) EMIT2(0x108) EMIT2(0x10a)
		    EMIT2(0x10c) EMIT2(0x187) EMIT2(0x23b)
		    EMIT2(0x1e08) EMIT2(0xa792)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'D': ਹਾਲਤ 0x10e: ਹਾਲਤ 0x110: ਹਾਲਤ 0x18a:
	    ਹਾਲਤ 0x1e0a: ਹਾਲਤ 0x1e0c: ਹਾਲਤ 0x1e0e: ਹਾਲਤ 0x1e10:
	    ਹਾਲਤ 0x1e12:
		    EMIT2('D') EMIT2(0x10e) EMIT2(0x110) EMIT2(0x18a)
		    EMIT2(0x1e0a) EMIT2(0x1e0c) EMIT2(0x1e0e)
		    EMIT2(0x1e10) EMIT2(0x1e12)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'E': ਹਾਲਤ E_grave: ਹਾਲਤ E_acute: ਹਾਲਤ E_circumflex:
	    ਹਾਲਤ E_diaeresis: ਹਾਲਤ 0x112: ਹਾਲਤ 0x114: ਹਾਲਤ 0x116:
	    ਹਾਲਤ 0x118: ਹਾਲਤ 0x11a: ਹਾਲਤ 0x204: ਹਾਲਤ 0x206:
	    ਹਾਲਤ 0x228: ਹਾਲਤ 0x246: ਹਾਲਤ 0x1e14: ਹਾਲਤ 0x1e16:
	    ਹਾਲਤ 0x1e18: ਹਾਲਤ 0x1e1a: ਹਾਲਤ 0x1e1c: ਹਾਲਤ 0x1eb8:
	    ਹਾਲਤ 0x1eba: ਹਾਲਤ 0x1ebc: ਹਾਲਤ 0x1ebe: ਹਾਲਤ 0x1ec0:
	    ਹਾਲਤ 0x1ec2: ਹਾਲਤ 0x1ec4: ਹਾਲਤ 0x1ec6:
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
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'F': ਹਾਲਤ 0x191: ਹਾਲਤ 0x1e1e: ਹਾਲਤ 0xa798:
		    EMIT2('F') EMIT2(0x191) EMIT2(0x1e1e) EMIT2(0xa798)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'G': ਹਾਲਤ 0x11c: ਹਾਲਤ 0x11e: ਹਾਲਤ 0x120:
	    ਹਾਲਤ 0x122: ਹਾਲਤ 0x193: ਹਾਲਤ 0x1e4: ਹਾਲਤ 0x1e6:
	    ਹਾਲਤ 0x1f4: ਹਾਲਤ 0x1e20: ਹਾਲਤ 0xa7a0:
		    EMIT2('G') EMIT2(0x11c) EMIT2(0x11e) EMIT2(0x120)
		    EMIT2(0x122) EMIT2(0x193) EMIT2(0x1e4)
		    EMIT2(0x1e6) EMIT2(0x1f4) EMIT2(0x1e20)
		    EMIT2(0xa7a0)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'H': ਹਾਲਤ 0x124: ਹਾਲਤ 0x126: ਹਾਲਤ 0x21e:
	    ਹਾਲਤ 0x1e22: ਹਾਲਤ 0x1e24: ਹਾਲਤ 0x1e26: ਹਾਲਤ 0x1e28:
	    ਹਾਲਤ 0x1e2a: ਹਾਲਤ 0x2c67:
		    EMIT2('H') EMIT2(0x124) EMIT2(0x126) EMIT2(0x21e)
		    EMIT2(0x1e22) EMIT2(0x1e24) EMIT2(0x1e26)
		    EMIT2(0x1e28) EMIT2(0x1e2a) EMIT2(0x2c67)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'I': ਹਾਲਤ I_grave: ਹਾਲਤ I_acute: ਹਾਲਤ I_circumflex:
	    ਹਾਲਤ I_diaeresis: ਹਾਲਤ 0x128: ਹਾਲਤ 0x12a: ਹਾਲਤ 0x12c:
	    ਹਾਲਤ 0x12e: ਹਾਲਤ 0x130: ਹਾਲਤ 0x197: ਹਾਲਤ 0x1cf:
	    ਹਾਲਤ 0x208: ਹਾਲਤ 0x20a: ਹਾਲਤ 0x1e2c: ਹਾਲਤ 0x1e2e:
	    ਹਾਲਤ 0x1ec8: ਹਾਲਤ 0x1eca:
		    EMIT2('I') EMIT2(I_grave) EMIT2(I_acute)
		    EMIT2(I_circumflex) EMIT2(I_diaeresis)
		    EMIT2(0x128) EMIT2(0x12a) EMIT2(0x12c)
		    EMIT2(0x12e) EMIT2(0x130) EMIT2(0x197)
		    EMIT2(0x1cf) EMIT2(0x208) EMIT2(0x20a)
		    EMIT2(0x1e2c) EMIT2(0x1e2e) EMIT2(0x1ec8)
		    EMIT2(0x1eca)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'J': ਹਾਲਤ 0x134: ਹਾਲਤ 0x248:
		    EMIT2('J') EMIT2(0x134) EMIT2(0x248)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'K': ਹਾਲਤ 0x136: ਹਾਲਤ 0x198: ਹਾਲਤ 0x1e8: ਹਾਲਤ 0x1e30:
	    ਹਾਲਤ 0x1e32: ਹਾਲਤ 0x1e34: ਹਾਲਤ 0x2c69: ਹਾਲਤ 0xa740:
		    EMIT2('K') EMIT2(0x136) EMIT2(0x198) EMIT2(0x1e8)
		    EMIT2(0x1e30) EMIT2(0x1e32) EMIT2(0x1e34)
		    EMIT2(0x2c69) EMIT2(0xa740)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'L': ਹਾਲਤ 0x139: ਹਾਲਤ 0x13b: ਹਾਲਤ 0x13d:
	    ਹਾਲਤ 0x13f: ਹਾਲਤ 0x141: ਹਾਲਤ 0x23d: ਹਾਲਤ 0x1e36:
	    ਹਾਲਤ 0x1e38: ਹਾਲਤ 0x1e3a: ਹਾਲਤ 0x1e3c: ਹਾਲਤ 0x2c60:
		    EMIT2('L') EMIT2(0x139) EMIT2(0x13b)
		    EMIT2(0x13d) EMIT2(0x13f) EMIT2(0x141)
		    EMIT2(0x23d) EMIT2(0x1e36) EMIT2(0x1e38)
		    EMIT2(0x1e3a) EMIT2(0x1e3c) EMIT2(0x2c60)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'M': ਹਾਲਤ 0x1e3e: ਹਾਲਤ 0x1e40: ਹਾਲਤ 0x1e42:
		    EMIT2('M') EMIT2(0x1e3e) EMIT2(0x1e40)
		    EMIT2(0x1e42)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'N': ਹਾਲਤ N_virguilla:
	    ਹਾਲਤ 0x143: ਹਾਲਤ 0x145: ਹਾਲਤ 0x147: ਹਾਲਤ 0x1f8:
	    ਹਾਲਤ 0x1e44: ਹਾਲਤ 0x1e46: ਹਾਲਤ 0x1e48: ਹਾਲਤ 0x1e4a:
	    ਹਾਲਤ 0xa7a4:
		    EMIT2('N') EMIT2(N_virguilla)
		    EMIT2(0x143) EMIT2(0x145) EMIT2(0x147)
		    EMIT2(0x1f8) EMIT2(0x1e44) EMIT2(0x1e46)
		    EMIT2(0x1e48) EMIT2(0x1e4a) EMIT2(0xa7a4)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'O': ਹਾਲਤ O_grave: ਹਾਲਤ O_acute: ਹਾਲਤ O_circumflex:
	    ਹਾਲਤ O_virguilla: ਹਾਲਤ O_diaeresis: ਹਾਲਤ O_slash:
	    ਹਾਲਤ 0x14c: ਹਾਲਤ 0x14e: ਹਾਲਤ 0x150: ਹਾਲਤ 0x19f:
	    ਹਾਲਤ 0x1a0: ਹਾਲਤ 0x1d1: ਹਾਲਤ 0x1ea: ਹਾਲਤ 0x1ec:
	    ਹਾਲਤ 0x1fe: ਹਾਲਤ 0x20c: ਹਾਲਤ 0x20e: ਹਾਲਤ 0x22a:
	    ਹਾਲਤ 0x22c: ਹਾਲਤ 0x22e: ਹਾਲਤ 0x230: ਹਾਲਤ 0x1e4c:
	    ਹਾਲਤ 0x1e4e: ਹਾਲਤ 0x1e50: ਹਾਲਤ 0x1e52: ਹਾਲਤ 0x1ecc:
	    ਹਾਲਤ 0x1ece: ਹਾਲਤ 0x1ed0: ਹਾਲਤ 0x1ed2: ਹਾਲਤ 0x1ed4:
	    ਹਾਲਤ 0x1ed6: ਹਾਲਤ 0x1ed8: ਹਾਲਤ 0x1eda: ਹਾਲਤ 0x1edc:
	    ਹਾਲਤ 0x1ede: ਹਾਲਤ 0x1ee0: ਹਾਲਤ 0x1ee2:
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
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'P': ਹਾਲਤ 0x1a4: ਹਾਲਤ 0x1e54: ਹਾਲਤ 0x1e56: ਹਾਲਤ 0x2c63:
		    EMIT2('P') EMIT2(0x1a4) EMIT2(0x1e54) EMIT2(0x1e56)
		    EMIT2(0x2c63)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'Q': ਹਾਲਤ 0x24a:
		    EMIT2('Q') EMIT2(0x24a)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'R': ਹਾਲਤ 0x154: ਹਾਲਤ 0x156: ਹਾਲਤ 0x158: ਹਾਲਤ 0x210:
	    ਹਾਲਤ 0x212: ਹਾਲਤ 0x24c: ਹਾਲਤ 0x1e58: ਹਾਲਤ 0x1e5a:
	    ਹਾਲਤ 0x1e5c: ਹਾਲਤ 0x1e5e: ਹਾਲਤ 0x2c64: ਹਾਲਤ 0xa7a6:
		    EMIT2('R') EMIT2(0x154) EMIT2(0x156) EMIT2(0x158)
		    EMIT2(0x210) EMIT2(0x212) EMIT2(0x24c) EMIT2(0x1e58)
		    EMIT2(0x1e5a) EMIT2(0x1e5c) EMIT2(0x1e5e) EMIT2(0x2c64)
		    EMIT2(0xa7a6)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'S': ਹਾਲਤ 0x15a: ਹਾਲਤ 0x15c: ਹਾਲਤ 0x15e: ਹਾਲਤ 0x160:
	    ਹਾਲਤ 0x218: ਹਾਲਤ 0x1e60: ਹਾਲਤ 0x1e62: ਹਾਲਤ 0x1e64:
	    ਹਾਲਤ 0x1e66: ਹਾਲਤ 0x1e68: ਹਾਲਤ 0x2c7e: ਹਾਲਤ 0xa7a8:
		    EMIT2('S') EMIT2(0x15a) EMIT2(0x15c) EMIT2(0x15e)
		    EMIT2(0x160) EMIT2(0x218) EMIT2(0x1e60) EMIT2(0x1e62)
		    EMIT2(0x1e64) EMIT2(0x1e66) EMIT2(0x1e68) EMIT2(0x2c7e)
		    EMIT2(0xa7a8)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'T': ਹਾਲਤ 0x162: ਹਾਲਤ 0x164: ਹਾਲਤ 0x166: ਹਾਲਤ 0x1ac:
	    ਹਾਲਤ 0x1ae: ਹਾਲਤ 0x21a: ਹਾਲਤ 0x23e: ਹਾਲਤ 0x1e6a: ਹਾਲਤ 0x1e6c:
	    ਹਾਲਤ 0x1e6e: ਹਾਲਤ 0x1e70:
		    EMIT2('T') EMIT2(0x162) EMIT2(0x164) EMIT2(0x166)
		    EMIT2(0x1ac) EMIT2(0x1ae) EMIT2(0x23e) EMIT2(0x21a)
		    EMIT2(0x1e6a) EMIT2(0x1e6c) EMIT2(0x1e6e) EMIT2(0x1e70)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'U': ਹਾਲਤ U_grave: ਹਾਲਤ U_acute: ਹਾਲਤ U_diaeresis:
	    ਹਾਲਤ U_circumflex: ਹਾਲਤ 0x168: ਹਾਲਤ 0x16a: ਹਾਲਤ 0x16c:
	    ਹਾਲਤ 0x16e: ਹਾਲਤ 0x170: ਹਾਲਤ 0x172: ਹਾਲਤ 0x1af:
	    ਹਾਲਤ 0x1d3: ਹਾਲਤ 0x1d5: ਹਾਲਤ 0x1d7: ਹਾਲਤ 0x1d9:
	    ਹਾਲਤ 0x1db: ਹਾਲਤ 0x214: ਹਾਲਤ 0x216: ਹਾਲਤ 0x244:
	    ਹਾਲਤ 0x1e72: ਹਾਲਤ 0x1e74: ਹਾਲਤ 0x1e76: ਹਾਲਤ 0x1e78:
	    ਹਾਲਤ 0x1e7a: ਹਾਲਤ 0x1ee4: ਹਾਲਤ 0x1ee6: ਹਾਲਤ 0x1ee8:
	    ਹਾਲਤ 0x1eea: ਹਾਲਤ 0x1eec: ਹਾਲਤ 0x1eee: ਹਾਲਤ 0x1ef0:
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
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'V': ਹਾਲਤ 0x1b2: ਹਾਲਤ 0x1e7c: ਹਾਲਤ 0x1e7e:
		    EMIT2('V') EMIT2(0x1b2) EMIT2(0x1e7c) EMIT2(0x1e7e)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'W': ਹਾਲਤ 0x174: ਹਾਲਤ 0x1e80: ਹਾਲਤ 0x1e82: ਹਾਲਤ 0x1e84:
	    ਹਾਲਤ 0x1e86: ਹਾਲਤ 0x1e88:
		    EMIT2('W') EMIT2(0x174) EMIT2(0x1e80) EMIT2(0x1e82)
		    EMIT2(0x1e84) EMIT2(0x1e86) EMIT2(0x1e88)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'X': ਹਾਲਤ 0x1e8a: ਹਾਲਤ 0x1e8c:
		    EMIT2('X') EMIT2(0x1e8a) EMIT2(0x1e8c)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'Y': ਹਾਲਤ Y_acute: ਹਾਲਤ 0x176: ਹਾਲਤ 0x178:
	    ਹਾਲਤ 0x1b3: ਹਾਲਤ 0x232: ਹਾਲਤ 0x24e: ਹਾਲਤ 0x1e8e:
	    ਹਾਲਤ 0x1ef2: ਹਾਲਤ 0x1ef4: ਹਾਲਤ 0x1ef6: ਹਾਲਤ 0x1ef8:
		    EMIT2('Y') EMIT2(Y_acute)
		    EMIT2(0x176) EMIT2(0x178) EMIT2(0x1b3)
		    EMIT2(0x232) EMIT2(0x24e) EMIT2(0x1e8e)
		    EMIT2(0x1ef2) EMIT2(0x1ef4) EMIT2(0x1ef6)
		    EMIT2(0x1ef8)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'Z': ਹਾਲਤ 0x179: ਹਾਲਤ 0x17b: ਹਾਲਤ 0x17d:
	    ਹਾਲਤ 0x1b5: ਹਾਲਤ 0x1e90: ਹਾਲਤ 0x1e92: ਹਾਲਤ 0x1e94:
	    ਹਾਲਤ 0x2c6b:
		    EMIT2('Z') EMIT2(0x179) EMIT2(0x17b) EMIT2(0x17d)
		    EMIT2(0x1b5) EMIT2(0x1e90) EMIT2(0x1e92)
		    EMIT2(0x1e94) EMIT2(0x2c6b)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'a': ਹਾਲਤ a_grave: ਹਾਲਤ a_acute: ਹਾਲਤ a_circumflex:
	    ਹਾਲਤ a_virguilla: ਹਾਲਤ a_diaeresis: ਹਾਲਤ a_ring:
	    ਹਾਲਤ 0x101: ਹਾਲਤ 0x103: ਹਾਲਤ 0x105: ਹਾਲਤ 0x1ce:
	    ਹਾਲਤ 0x1df: ਹਾਲਤ 0x1e1: ਹਾਲਤ 0x1fb: ਹਾਲਤ 0x201:
	    ਹਾਲਤ 0x203: ਹਾਲਤ 0x227: ਹਾਲਤ 0x1d8f: ਹਾਲਤ 0x1e01:
	    ਹਾਲਤ 0x1e9a: ਹਾਲਤ 0x1ea1: ਹਾਲਤ 0x1ea3: ਹਾਲਤ 0x1ea5:
	    ਹਾਲਤ 0x1ea7: ਹਾਲਤ 0x1ea9: ਹਾਲਤ 0x1eab: ਹਾਲਤ 0x1ead:
	    ਹਾਲਤ 0x1eaf: ਹਾਲਤ 0x1eb1: ਹਾਲਤ 0x1eb3: ਹਾਲਤ 0x1eb5:
	    ਹਾਲਤ 0x1eb7: ਹਾਲਤ 0x2c65:
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
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'b': ਹਾਲਤ 0x180: ਹਾਲਤ 0x253: ਹਾਲਤ 0x1d6c: ਹਾਲਤ 0x1d80:
	    ਹਾਲਤ 0x1e03: ਹਾਲਤ 0x1e05: ਹਾਲਤ 0x1e07:
		    EMIT2('b') EMIT2(0x180) EMIT2(0x253) EMIT2(0x1d6c)
		    EMIT2(0x1d80) EMIT2(0x1e03) EMIT2(0x1e05) EMIT2(0x1e07)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'c': ਹਾਲਤ c_cedilla: ਹਾਲਤ 0x107: ਹਾਲਤ 0x109: ਹਾਲਤ 0x10b:
	    ਹਾਲਤ 0x10d: ਹਾਲਤ 0x188: ਹਾਲਤ 0x23c: ਹਾਲਤ 0x1e09: ਹਾਲਤ 0xa793:
	    ਹਾਲਤ 0xa794:
		    EMIT2('c') EMIT2(c_cedilla)
		    EMIT2(0x107) EMIT2(0x109) EMIT2(0x10b)
		    EMIT2(0x10d) EMIT2(0x188) EMIT2(0x23c)
		    EMIT2(0x1e09) EMIT2(0xa793) EMIT2(0xa794)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'd': ਹਾਲਤ 0x10f: ਹਾਲਤ 0x111: ਹਾਲਤ 0x257: ਹਾਲਤ 0x1d6d:
	    ਹਾਲਤ 0x1d81: ਹਾਲਤ 0x1d91: ਹਾਲਤ 0x1e0b: ਹਾਲਤ 0x1e0d: ਹਾਲਤ 0x1e0f:
	    ਹਾਲਤ 0x1e11: ਹਾਲਤ 0x1e13:
		    EMIT2('d') EMIT2(0x10f) EMIT2(0x111)
		    EMIT2(0x257) EMIT2(0x1d6d) EMIT2(0x1d81)
		    EMIT2(0x1d91) EMIT2(0x1e0b) EMIT2(0x1e0d)
		    EMIT2(0x1e0f) EMIT2(0x1e11) EMIT2(0x1e13)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'e': ਹਾਲਤ e_grave: ਹਾਲਤ e_acute: ਹਾਲਤ e_circumflex:
	    ਹਾਲਤ e_diaeresis: ਹਾਲਤ 0x113: ਹਾਲਤ 0x115: ਹਾਲਤ 0x117:
	    ਹਾਲਤ 0x119: ਹਾਲਤ 0x11b: ਹਾਲਤ 0x205: ਹਾਲਤ 0x207:
	    ਹਾਲਤ 0x229: ਹਾਲਤ 0x247: ਹਾਲਤ 0x1d92: ਹਾਲਤ 0x1e15:
	    ਹਾਲਤ 0x1e17: ਹਾਲਤ 0x1e19: ਹਾਲਤ 0x1e1b: ਹਾਲਤ 0x1e1d:
	    ਹਾਲਤ 0x1eb9: ਹਾਲਤ 0x1ebb: ਹਾਲਤ 0x1ebd: ਹਾਲਤ 0x1ebf:
	    ਹਾਲਤ 0x1ec1: ਹਾਲਤ 0x1ec3: ਹਾਲਤ 0x1ec5: ਹਾਲਤ 0x1ec7:
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
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'f': ਹਾਲਤ 0x192: ਹਾਲਤ 0x1d6e: ਹਾਲਤ 0x1d82:
	    ਹਾਲਤ 0x1e1f: ਹਾਲਤ 0xa799:
		    EMIT2('f') EMIT2(0x192) EMIT2(0x1d6e) EMIT2(0x1d82)
		    EMIT2(0x1e1f) EMIT2(0xa799)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'g': ਹਾਲਤ 0x11d: ਹਾਲਤ 0x11f: ਹਾਲਤ 0x121: ਹਾਲਤ 0x123:
	    ਹਾਲਤ 0x1e5: ਹਾਲਤ 0x1e7: ਹਾਲਤ 0x1f5: ਹਾਲਤ 0x260: ਹਾਲਤ 0x1d83:
	    ਹਾਲਤ 0x1e21: ਹਾਲਤ 0xa7a1:
		    EMIT2('g') EMIT2(0x11d) EMIT2(0x11f) EMIT2(0x121)
		    EMIT2(0x123) EMIT2(0x1e5) EMIT2(0x1e7)
		    EMIT2(0x1f5) EMIT2(0x260) EMIT2(0x1d83)
		    EMIT2(0x1e21) EMIT2(0xa7a1)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'h': ਹਾਲਤ 0x125: ਹਾਲਤ 0x127: ਹਾਲਤ 0x21f: ਹਾਲਤ 0x1e23:
	    ਹਾਲਤ 0x1e25: ਹਾਲਤ 0x1e27: ਹਾਲਤ 0x1e29: ਹਾਲਤ 0x1e2b:
	    ਹਾਲਤ 0x1e96: ਹਾਲਤ 0x2c68: ਹਾਲਤ 0xa795:
		    EMIT2('h') EMIT2(0x125) EMIT2(0x127) EMIT2(0x21f)
		    EMIT2(0x1e23) EMIT2(0x1e25) EMIT2(0x1e27)
		    EMIT2(0x1e29) EMIT2(0x1e2b) EMIT2(0x1e96)
		    EMIT2(0x2c68) EMIT2(0xa795)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'i': ਹਾਲਤ i_grave: ਹਾਲਤ i_acute: ਹਾਲਤ i_circumflex:
	    ਹਾਲਤ i_diaeresis: ਹਾਲਤ 0x129: ਹਾਲਤ 0x12b: ਹਾਲਤ 0x12d:
	    ਹਾਲਤ 0x12f: ਹਾਲਤ 0x1d0: ਹਾਲਤ 0x209: ਹਾਲਤ 0x20b:
	    ਹਾਲਤ 0x268: ਹਾਲਤ 0x1d96: ਹਾਲਤ 0x1e2d: ਹਾਲਤ 0x1e2f:
	    ਹਾਲਤ 0x1ec9: ਹਾਲਤ 0x1ecb:
		    EMIT2('i') EMIT2(i_grave) EMIT2(i_acute)
		    EMIT2(i_circumflex) EMIT2(i_diaeresis)
		    EMIT2(0x129) EMIT2(0x12b) EMIT2(0x12d)
		    EMIT2(0x12f) EMIT2(0x1d0) EMIT2(0x209)
		    EMIT2(0x20b) EMIT2(0x268) EMIT2(0x1d96)
		    EMIT2(0x1e2d) EMIT2(0x1e2f) EMIT2(0x1ec9)
		    EMIT2(0x1ecb)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'j': ਹਾਲਤ 0x135: ਹਾਲਤ 0x1f0: ਹਾਲਤ 0x249:
		    EMIT2('j') EMIT2(0x135) EMIT2(0x1f0) EMIT2(0x249)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'k': ਹਾਲਤ 0x137: ਹਾਲਤ 0x199: ਹਾਲਤ 0x1e9: ਹਾਲਤ 0x1d84:
	    ਹਾਲਤ 0x1e31: ਹਾਲਤ 0x1e33: ਹਾਲਤ 0x1e35: ਹਾਲਤ 0x2c6a: ਹਾਲਤ 0xa741:
		    EMIT2('k') EMIT2(0x137) EMIT2(0x199) EMIT2(0x1e9)
		    EMIT2(0x1d84) EMIT2(0x1e31) EMIT2(0x1e33)
		    EMIT2(0x1e35) EMIT2(0x2c6a) EMIT2(0xa741)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'l': ਹਾਲਤ 0x13a: ਹਾਲਤ 0x13c: ਹਾਲਤ 0x13e: ਹਾਲਤ 0x140:
	    ਹਾਲਤ 0x142: ਹਾਲਤ 0x19a: ਹਾਲਤ 0x1e37: ਹਾਲਤ 0x1e39: ਹਾਲਤ 0x1e3b:
	    ਹਾਲਤ 0x1e3d: ਹਾਲਤ 0x2c61:
		    EMIT2('l') EMIT2(0x13a) EMIT2(0x13c)
		    EMIT2(0x13e) EMIT2(0x140) EMIT2(0x142)
		    EMIT2(0x19a) EMIT2(0x1e37) EMIT2(0x1e39)
		    EMIT2(0x1e3b) EMIT2(0x1e3d) EMIT2(0x2c61)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'm': ਹਾਲਤ 0x1d6f: ਹਾਲਤ 0x1e3f: ਹਾਲਤ 0x1e41: ਹਾਲਤ 0x1e43:
		    EMIT2('m') EMIT2(0x1d6f) EMIT2(0x1e3f)
		    EMIT2(0x1e41) EMIT2(0x1e43)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'n': ਹਾਲਤ n_virguilla: ਹਾਲਤ 0x144: ਹਾਲਤ 0x146: ਹਾਲਤ 0x148:
	    ਹਾਲਤ 0x149: ਹਾਲਤ 0x1f9: ਹਾਲਤ 0x1d70: ਹਾਲਤ 0x1d87: ਹਾਲਤ 0x1e45:
	    ਹਾਲਤ 0x1e47: ਹਾਲਤ 0x1e49: ਹਾਲਤ 0x1e4b: ਹਾਲਤ 0xa7a5:
		    EMIT2('n') EMIT2(n_virguilla)
		    EMIT2(0x144) EMIT2(0x146) EMIT2(0x148)
		    EMIT2(0x149) EMIT2(0x1f9) EMIT2(0x1d70)
		    EMIT2(0x1d87) EMIT2(0x1e45) EMIT2(0x1e47)
		    EMIT2(0x1e49) EMIT2(0x1e4b) EMIT2(0xa7a5)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'o': ਹਾਲਤ o_grave: ਹਾਲਤ o_acute: ਹਾਲਤ o_circumflex:
	    ਹਾਲਤ o_virguilla: ਹਾਲਤ o_diaeresis: ਹਾਲਤ o_slash:
	    ਹਾਲਤ 0x14d: ਹਾਲਤ 0x14f: ਹਾਲਤ 0x151: ਹਾਲਤ 0x1a1:
	    ਹਾਲਤ 0x1d2: ਹਾਲਤ 0x1eb: ਹਾਲਤ 0x1ed: ਹਾਲਤ 0x1ff:
	    ਹਾਲਤ 0x20d: ਹਾਲਤ 0x20f: ਹਾਲਤ 0x22b: ਹਾਲਤ 0x22d:
	    ਹਾਲਤ 0x22f: ਹਾਲਤ 0x231: ਹਾਲਤ 0x275: ਹਾਲਤ 0x1e4d:
	    ਹਾਲਤ 0x1e4f: ਹਾਲਤ 0x1e51: ਹਾਲਤ 0x1e53: ਹਾਲਤ 0x1ecd:
	    ਹਾਲਤ 0x1ecf: ਹਾਲਤ 0x1ed1: ਹਾਲਤ 0x1ed3: ਹਾਲਤ 0x1ed5:
	    ਹਾਲਤ 0x1ed7: ਹਾਲਤ 0x1ed9: ਹਾਲਤ 0x1edb: ਹਾਲਤ 0x1edd:
	    ਹਾਲਤ 0x1edf: ਹਾਲਤ 0x1ee1: ਹਾਲਤ 0x1ee3:
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
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'p': ਹਾਲਤ 0x1a5: ਹਾਲਤ 0x1d71: ਹਾਲਤ 0x1d7d: ਹਾਲਤ 0x1d88:
	    ਹਾਲਤ 0x1e55: ਹਾਲਤ 0x1e57:
		    EMIT2('p') EMIT2(0x1a5) EMIT2(0x1d71) EMIT2(0x1d7d)
		    EMIT2(0x1d88) EMIT2(0x1e55) EMIT2(0x1e57)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'q': ਹਾਲਤ 0x24b: ਹਾਲਤ 0x2a0:
		    EMIT2('q') EMIT2(0x24b) EMIT2(0x2a0)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'r': ਹਾਲਤ 0x155: ਹਾਲਤ 0x157: ਹਾਲਤ 0x159: ਹਾਲਤ 0x211:
	    ਹਾਲਤ 0x213: ਹਾਲਤ 0x24d: ਹਾਲਤ 0x27d: ਹਾਲਤ 0x1d72: ਹਾਲਤ 0x1d73:
	    ਹਾਲਤ 0x1d89: ਹਾਲਤ 0x1e59: ਹਾਲਤ 0x1e5b: ਹਾਲਤ 0x1e5d: ਹਾਲਤ 0x1e5f:
	    ਹਾਲਤ 0xa7a7:
		    EMIT2('r') EMIT2(0x155) EMIT2(0x157) EMIT2(0x159)
		    EMIT2(0x211) EMIT2(0x213) EMIT2(0x24d) EMIT2(0x27d)
		    EMIT2(0x1d72) EMIT2(0x1d73) EMIT2(0x1d89) EMIT2(0x1e59)
		    EMIT2(0x1e5b) EMIT2(0x1e5d) EMIT2(0x1e5f) EMIT2(0xa7a7)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 's': ਹਾਲਤ 0x15b: ਹਾਲਤ 0x15d: ਹਾਲਤ 0x15f: ਹਾਲਤ 0x161:
	    ਹਾਲਤ 0x219: ਹਾਲਤ 0x23f: ਹਾਲਤ 0x1d74: ਹਾਲਤ 0x1d8a: ਹਾਲਤ 0x1e61:
	    ਹਾਲਤ 0x1e63: ਹਾਲਤ 0x1e65: ਹਾਲਤ 0x1e67: ਹਾਲਤ 0x1e69: ਹਾਲਤ 0xa7a9:
		    EMIT2('s') EMIT2(0x15b) EMIT2(0x15d) EMIT2(0x15f)
		    EMIT2(0x161) EMIT2(0x219) EMIT2(0x23f) EMIT2(0x1d74)
		    EMIT2(0x1d8a) EMIT2(0x1e61) EMIT2(0x1e63) EMIT2(0x1e65)
		    EMIT2(0x1e67) EMIT2(0x1e69) EMIT2(0xa7a9)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 't': ਹਾਲਤ 0x163: ਹਾਲਤ 0x165: ਹਾਲਤ 0x167: ਹਾਲਤ 0x1ab:
	    ਹਾਲਤ 0x1ad: ਹਾਲਤ 0x21b: ਹਾਲਤ 0x288: ਹਾਲਤ 0x1d75: ਹਾਲਤ 0x1e6b:
	    ਹਾਲਤ 0x1e6d: ਹਾਲਤ 0x1e6f: ਹਾਲਤ 0x1e71: ਹਾਲਤ 0x1e97: ਹਾਲਤ 0x2c66:
		    EMIT2('t') EMIT2(0x163) EMIT2(0x165) EMIT2(0x167)
		    EMIT2(0x1ab) EMIT2(0x1ad) EMIT2(0x21b) EMIT2(0x288)
		    EMIT2(0x1d75) EMIT2(0x1e6b) EMIT2(0x1e6d) EMIT2(0x1e6f)
		    EMIT2(0x1e71) EMIT2(0x1e97) EMIT2(0x2c66)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'u': ਹਾਲਤ u_grave: ਹਾਲਤ u_acute: ਹਾਲਤ u_circumflex:
	    ਹਾਲਤ u_diaeresis: ਹਾਲਤ 0x169: ਹਾਲਤ 0x16b: ਹਾਲਤ 0x16d:
	    ਹਾਲਤ 0x16f: ਹਾਲਤ 0x171: ਹਾਲਤ 0x173: ਹਾਲਤ 0x1b0: ਹਾਲਤ 0x1d4:
	    ਹਾਲਤ 0x1d6: ਹਾਲਤ 0x1d8: ਹਾਲਤ 0x1da: ਹਾਲਤ 0x1dc: ਹਾਲਤ 0x215:
	    ਹਾਲਤ 0x217: ਹਾਲਤ 0x289: ਹਾਲਤ 0x1d7e: ਹਾਲਤ 0x1d99: ਹਾਲਤ 0x1e73:
	    ਹਾਲਤ 0x1e75: ਹਾਲਤ 0x1e77: ਹਾਲਤ 0x1e79: ਹਾਲਤ 0x1e7b:
	    ਹਾਲਤ 0x1ee5: ਹਾਲਤ 0x1ee7: ਹਾਲਤ 0x1ee9: ਹਾਲਤ 0x1eeb:
	    ਹਾਲਤ 0x1eed: ਹਾਲਤ 0x1eef: ਹਾਲਤ 0x1ef1:
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
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'v': ਹਾਲਤ 0x28b: ਹਾਲਤ 0x1d8c: ਹਾਲਤ 0x1e7d: ਹਾਲਤ 0x1e7f:
		    EMIT2('v') EMIT2(0x28b) EMIT2(0x1d8c) EMIT2(0x1e7d)
		    EMIT2(0x1e7f)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'w': ਹਾਲਤ 0x175: ਹਾਲਤ 0x1e81: ਹਾਲਤ 0x1e83: ਹਾਲਤ 0x1e85:
	    ਹਾਲਤ 0x1e87: ਹਾਲਤ 0x1e89: ਹਾਲਤ 0x1e98:
		    EMIT2('w') EMIT2(0x175) EMIT2(0x1e81) EMIT2(0x1e83)
		    EMIT2(0x1e85) EMIT2(0x1e87) EMIT2(0x1e89) EMIT2(0x1e98)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'x': ਹਾਲਤ 0x1e8b: ਹਾਲਤ 0x1e8d:
		    EMIT2('x') EMIT2(0x1e8b) EMIT2(0x1e8d)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'y': ਹਾਲਤ y_acute: ਹਾਲਤ y_diaeresis: ਹਾਲਤ 0x177:
	    ਹਾਲਤ 0x1b4: ਹਾਲਤ 0x233: ਹਾਲਤ 0x24f: ਹਾਲਤ 0x1e8f:
	    ਹਾਲਤ 0x1e99: ਹਾਲਤ 0x1ef3: ਹਾਲਤ 0x1ef5: ਹਾਲਤ 0x1ef7:
	    ਹਾਲਤ 0x1ef9:
		    EMIT2('y') EMIT2(y_acute) EMIT2(y_diaeresis)
		    EMIT2(0x177) EMIT2(0x1b4) EMIT2(0x233) EMIT2(0x24f)
		    EMIT2(0x1e8f) EMIT2(0x1e99) EMIT2(0x1ef3)
		    EMIT2(0x1ef5) EMIT2(0x1ef7) EMIT2(0x1ef9)
		    ਵਾਪਸ OK;

	    ਹਾਲਤ 'z': ਹਾਲਤ 0x17a: ਹਾਲਤ 0x17c: ਹਾਲਤ 0x17e: ਹਾਲਤ 0x1b6:
	    ਹਾਲਤ 0x1d76: ਹਾਲਤ 0x1d8e: ਹਾਲਤ 0x1e91: ਹਾਲਤ 0x1e93:
	    ਹਾਲਤ 0x1e95: ਹਾਲਤ 0x2c6c:
		    EMIT2('z') EMIT2(0x17a) EMIT2(0x17c) EMIT2(0x17e)
		    EMIT2(0x1b6) EMIT2(0x1d76) EMIT2(0x1d8e) EMIT2(0x1e91)
		    EMIT2(0x1e93) EMIT2(0x1e95) EMIT2(0x2c6c)
		    ਵਾਪਸ OK;

	    // default: character itself
	}
    }

    EMIT2(c);
    ਵਾਪਸ OK;
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ EMIT2
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
    ਸਥਿਰ ਪੂਰਨ
nfa_regatom(ਖਾਲੀ)
{
    ਪੂਰਨ		c;
    ਪੂਰਨ		charclass;
    ਪੂਰਨ		equiclass;
    ਪੂਰਨ		collclass;
    ਪੂਰਨ		got_coll_char;
    char_u	*p;
    char_u	*endp;
    char_u	*old_regparse = regparse;
    ਪੂਰਨ		extra = 0;
    ਪੂਰਨ		emit_range;
    ਪੂਰਨ		negated;
    ਪੂਰਨ		ਨਤੀਜਾ;
    ਪੂਰਨ		startc = -1;
    ਪੂਰਨ		save_prev_at_ਸ਼ੁਰੂ = prev_at_ਸ਼ੁਰੂ;

    c = getchr();
    ਚੋਣ (c)
    {
	ਹਾਲਤ NUL:
	    EMSG_RET_FAIL(_(e_nfa_regexp_ਸਿਰਾ_encountered_prematurely));

	ਹਾਲਤ Magic('^'):
	    EMIT(NFA_BOL);
	    ਰੋਕੋ;

	ਹਾਲਤ Magic('$'):
	    EMIT(NFA_EOL);
#ਜੇ defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#ਜੇ_ਅੰਤ
	    ਰੋਕੋ;

	ਹਾਲਤ Magic('<'):
	    EMIT(NFA_BOW);
	    ਰੋਕੋ;

	ਹਾਲਤ Magic('>'):
	    EMIT(NFA_EOW);
	    ਰੋਕੋ;

	ਹਾਲਤ Magic('_'):
	    c = no_Magic(getchr());
	    ਜੇ (c == NUL)
		EMSG_RET_FAIL(_(e_nfa_regexp_ਸਿਰਾ_encountered_prematurely));

	    ਜੇ (c == '^')	// "\_^" is start-of-line
	    {
		EMIT(NFA_BOL);
		ਰੋਕੋ;
	    }
	    ਜੇ (c == '$')	// "\_$" is end-of-line
	    {
		EMIT(NFA_EOL);
#ਜੇ defined(FEAT_SYN_HL)
		had_eol = TRUE;
#ਜੇ_ਅੰਤ
		ਰੋਕੋ;
	    }

	    extra = NFA_ਜੋੜੋ_NL;

	    // "\_[" is collection plus newline
	    ਜੇ (c == '[')
		ਜਾਓ collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	/*
	 * Character classes.
	 */
	ਹਾਲਤ Magic('.'):
	ਹਾਲਤ Magic('i'):
	ਹਾਲਤ Magic('I'):
	ਹਾਲਤ Magic('k'):
	ਹਾਲਤ Magic('K'):
	ਹਾਲਤ Magic('f'):
	ਹਾਲਤ Magic('F'):
	ਹਾਲਤ Magic('p'):
	ਹਾਲਤ Magic('P'):
	ਹਾਲਤ Magic('s'):
	ਹਾਲਤ Magic('S'):
	ਹਾਲਤ Magic('d'):
	ਹਾਲਤ Magic('D'):
	ਹਾਲਤ Magic('x'):
	ਹਾਲਤ Magic('X'):
	ਹਾਲਤ Magic('o'):
	ਹਾਲਤ Magic('O'):
	ਹਾਲਤ Magic('w'):
	ਹਾਲਤ Magic('W'):
	ਹਾਲਤ Magic('h'):
	ਹਾਲਤ Magic('H'):
	ਹਾਲਤ Magic('a'):
	ਹਾਲਤ Magic('A'):
	ਹਾਲਤ Magic('l'):
	ਹਾਲਤ Magic('L'):
	ਹਾਲਤ Magic('u'):
	ਹਾਲਤ Magic('U'):
	    p = vim_strchr(classchars, no_Magic(c));
	    ਜੇ (p == ਨਿਰਾ)
	    {
		ਜੇ (extra == NFA_ਜੋੜੋ_NL)
		{
		    semsg(_(e_nfa_regexp_invalid_character_class_nr), c);
		    rc_did_emsg = TRUE;
		    ਵਾਪਸ FAIL;
		}
		siemsg("Unknown character class char: %d", c);
		ਵਾਪਸ FAIL;
	    }

	    // When '.' is followed by a composing char ignore the dot, so that
	    // the composing char is matched here.
	    ਜੇ (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	    {
		old_regparse = regparse;
		c = getchr();
		ਜਾਓ nfa_do_multibyte;
	    }
	    EMIT(nfa_classcodes[p - classchars]);
	    ਜੇ (extra == NFA_ਜੋੜੋ_NL)
	    {
		EMIT(NFA_NEWL);
		EMIT(NFA_OR);
		regflags |= RF_HASNL;
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ Magic('n'):
	    ਜੇ (reg_string)
		// In a string "\n" matches a newline character.
		EMIT(NL);
	    ਨਹੀਂਤਾਂ
	    {
		// In buffer text "\n" matches the end of a line.
		EMIT(NFA_NEWL);
		regflags |= RF_HASNL;
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ Magic('('):
	    ਜੇ (nfa_reg(REG_PAREN) == FAIL)
		ਵਾਪਸ FAIL;	    // cascaded error
	    ਰੋਕੋ;

	ਹਾਲਤ Magic('|'):
	ਹਾਲਤ Magic('&'):
	ਹਾਲਤ Magic(')'):
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    ਵਾਪਸ FAIL;

	ਹਾਲਤ Magic('='):
	ਹਾਲਤ Magic('?'):
	ਹਾਲਤ Magic('+'):
	ਹਾਲਤ Magic('@'):
	ਹਾਲਤ Magic('*'):
	ਹਾਲਤ Magic('{'):
	    // these should follow an atom, not form an atom
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    ਵਾਪਸ FAIL;

	ਹਾਲਤ Magic('~'):
	    {
		char_u	    *lp;

		// Previous substitute pattern.
		// Generated as "\%(pattern\)".
		ਜੇ (reg_prev_sub == ਨਿਰਾ)
		{
		    emsg(_(e_no_ਪਿਛਲਾ_substitute_regular_ਵਿਅੰਜਕ));
		    ਵਾਪਸ FAIL;
		}
		ਲਈ (lp = reg_prev_sub; *lp != NUL; MB_CPTR_ADV(lp))
		{
		    EMIT(PTR2CHAR(lp));
		    ਜੇ (lp != reg_prev_sub)
			EMIT(NFA_CONCAT);
		}
		EMIT(NFA_NOPEN);
		ਰੋਕੋ;
	    }

	ਹਾਲਤ Magic('1'):
	ਹਾਲਤ Magic('2'):
	ਹਾਲਤ Magic('3'):
	ਹਾਲਤ Magic('4'):
	ਹਾਲਤ Magic('5'):
	ਹਾਲਤ Magic('6'):
	ਹਾਲਤ Magic('7'):
	ਹਾਲਤ Magic('8'):
	ਹਾਲਤ Magic('9'):
	    {
		ਪੂਰਨ refnum = no_Magic(c) - '1';

		ਜੇ (!ਵੇਖਿਆ_endbrace(refnum + 1))
		    ਵਾਪਸ FAIL;
		EMIT(NFA_BACKREF1 + refnum);
		rex.nfa_has_backref = TRUE;
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ Magic('z'):
	    c = no_Magic(getchr());
	    ਚੋਣ (c)
	    {
		ਹਾਲਤ 's':
		    EMIT(NFA_ZSTART);
		    ਜੇ (re_mult_ਅਗਲਾ("\\zs") == FAIL)
			ਵਾਪਸ FAIL;
		    ਰੋਕੋ;
		ਹਾਲਤ 'e':
		    EMIT(NFA_ZEND);
		    rex.nfa_has_zend = TRUE;
		    ਜੇ (re_mult_ਅਗਲਾ("\\ze") == FAIL)
			ਵਾਪਸ FAIL;
		    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
		ਹਾਲਤ '1':
		ਹਾਲਤ '2':
		ਹਾਲਤ '3':
		ਹਾਲਤ '4':
		ਹਾਲਤ '5':
		ਹਾਲਤ '6':
		ਹਾਲਤ '7':
		ਹਾਲਤ '8':
		ਹਾਲਤ '9':
		    // \z1...\z9
		    ਜੇ ((reg_do_extmatch & REX_USE) == 0)
			EMSG_RET_FAIL(_(e_z1_z9_not_allowed_ਇੱਥੇ));
		    EMIT(NFA_ZREF1 + (no_Magic(c) - '1'));
		    // No need to set rex.nfa_has_backref, the sub-matches don't
		    // change when \z1 .. \z9 matches or not.
		    re_has_z = REX_USE;
		    ਰੋਕੋ;
		ਹਾਲਤ '(':
		    // \z(
		    ਜੇ ((reg_do_extmatch & REX_ਸੈੱਟ) == 0)
			EMSG_RET_FAIL(_(e_z_not_allowed_ਇੱਥੇ));
		    ਜੇ (nfa_reg(REG_ZPAREN) == FAIL)
			ਵਾਪਸ FAIL;	    // cascaded error
		    re_has_z = REX_ਸੈੱਟ;
		    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
		ਮੂਲ:
		    semsg(_(e_nfa_regexp_unknown_operator_z_chr), no_Magic(c));
		    ਵਾਪਸ FAIL;
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ Magic('%'):
	    c = no_Magic(getchr());
	    ਚੋਣ (c)
	    {
		// () without a back reference
		ਹਾਲਤ '(':
		    ਜੇ (nfa_reg(REG_NPAREN) == FAIL)
			ਵਾਪਸ FAIL;
		    EMIT(NFA_NOPEN);
		    ਰੋਕੋ;

		ਹਾਲਤ 'd':   // %d123 decimal
		ਹਾਲਤ 'o':   // %o123 octal
		ਹਾਲਤ 'x':   // %xab hex 2
		ਹਾਲਤ 'u':   // %uabcd hex 4
		ਹਾਲਤ 'U':   // %U1234abcd hex 8
		    {
			vimlong_T nr;

			ਚੋਣ (c)
			{
			    ਹਾਲਤ 'd': nr = getdecchrs(); ਰੋਕੋ;
			    ਹਾਲਤ 'o': nr = getoctchrs(); ਰੋਕੋ;
			    ਹਾਲਤ 'x': nr = gethexchrs(2); ਰੋਕੋ;
			    ਹਾਲਤ 'u': nr = gethexchrs(4); ਰੋਕੋ;
			    ਹਾਲਤ 'U': nr = gethexchrs(8); ਰੋਕੋ;
			    ਮੂਲ:  nr = -1; ਰੋਕੋ;
			}

			ਜੇ (nr < 0 || nr > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
			    EMSG2_RET_FAIL(_(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			// A NUL is stored in the text as NL
			// TODO: what if a composing character follows?
			EMIT(nr == 0 ? 0x0a : (ਲੰਮਾ)nr);
		    }
		    ਰੋਕੋ;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		ਹਾਲਤ '^':
		    EMIT(NFA_BOF);
		    ਰੋਕੋ;

		ਹਾਲਤ '$':
		    EMIT(NFA_EOF);
		    ਰੋਕੋ;

		ਹਾਲਤ '#':
		    ਜੇ (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_ਸ਼ੁਰੂ_ਦਾ_ਨਮੂਨਾ),
								  regparse[1]);
			ਵਾਪਸ FAIL;
		    }
		    EMIT(NFA_CURSOR);
		    ਰੋਕੋ;

		ਹਾਲਤ 'V':
		    EMIT(NFA_VISUAL);
		    ਰੋਕੋ;

		ਹਾਲਤ 'C':
		    EMIT(NFA_ANY_COMPOSING);
		    ਰੋਕੋ;

		ਹਾਲਤ '[':
		    {
			ਪੂਰਨ	    n;

			// \%[abc]
			ਲਈ (n = 0; (c = peekchr()) != ']'; ++n)
			{
			    ਜੇ (c == NUL)
				EMSG2_RET_FAIL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
			    // recursive call!
			    ਜੇ (nfa_regatom() == FAIL)
				ਵਾਪਸ FAIL;
			}
			getchr();  // get the ]
			ਜੇ (n == 0)
			    EMSG2_RET_FAIL(_(e_ਖਾਲੀ_ਥਾਵਾਂ_str_brackets),
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
			ਰੋਕੋ;
		    }

		ਮੂਲ:
		    {
			long_u	n = 0;
			ਪੂਰਨ	cmp = c;
			ਪੂਰਨ	cur = FALSE;
			ਪੂਰਨ	got_digit = FALSE;

			ਜੇ (c == '<' || c == '>')
			    c = getchr();
			ਜੇ (no_Magic(c) == '.')
			{
			    cur = TRUE;
			    c = getchr();
			}
			ਜਦੋਂ (VIM_ISDIGIT(c))
			{
			    long_u tmp;

			    ਜੇ (cur)
			    {
				semsg(_(e_regexp_ਸੰਖਿਆ_after_dot_pos_ਖੋਜ_chr),
								  no_Magic(c));
				ਵਾਪਸ FAIL;
			    }
			    tmp = n * 10 + (c - '0');

			    ਜੇ (tmp < n)
			    {
				// overflow.
				emsg(_(e_percent_ਮੁੱਲ_too_large));
				ਵਾਪਸ FAIL;
			    }
			    n = tmp;
			    c = getchr();
			    got_digit = TRUE;
			}
			ਜੇ (c == 'l' || c == 'c' || c == 'v')
			{
			    long_u ਹੱਦਬੰਦੀ = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;

			    ਜੇ (!cur && !got_digit)
			    {
				semsg(_(e_nfa_regexp_missing_ਮੁੱਲ_ਵਿੱਚ_chr),
								  no_Magic(c));
				ਵਾਪਸ FAIL;
			    }
			    ਜੇ (c == 'l')
			    {
				ਜੇ (cur)
				    n = curwin->w_cursor.lnum;
				// \%{n}l  \%{n}<l  \%{n}>l
				EMIT(cmp == '<' ? NFA_LNUM_LT :
				     cmp == '>' ? NFA_LNUM_GT : NFA_LNUM);
				ਜੇ (save_prev_at_ਸ਼ੁਰੂ)
				    at_ਸ਼ੁਰੂ = TRUE;
			    }
			    ਨਹੀਂਤਾਂ ਜੇ (c == 'c')
			    {
				ਜੇ (cur)
				{
				    n = curwin->w_cursor.ਖਾਨਾ;
				    n++;
				}
				// \%{n}c  \%{n}<c  \%{n}>c
				EMIT(cmp == '<' ? NFA_ਖਾਨਾ_LT :
				     cmp == '>' ? NFA_ਖਾਨਾ_GT : NFA_ਖਾਨਾ);
			    }
			    ਨਹੀਂਤਾਂ
			    {
				ਜੇ (cur)
				{
				    colnr_T vcol = 0;

				    getvvcol(curwin, &curwin->w_cursor,
							 ਨਿਰਾ, ਨਿਰਾ, &vcol, 0);
				    n = ++vcol;
				}
				// \%{n}v  \%{n}<v  \%{n}>v
				EMIT(cmp == '<' ? NFA_VCOL_LT :
				     cmp == '>' ? NFA_VCOL_GT : NFA_VCOL);
				ਹੱਦਬੰਦੀ = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ / MB_MAXBYTES;
			    }
			    ਜੇ (n >= ਹੱਦਬੰਦੀ)
			    {
				emsg(_(e_percent_ਮੁੱਲ_too_large));
				ਵਾਪਸ FAIL;
			    }
			    EMIT((ਪੂਰਨ)n);
			    ਰੋਕੋ;
			}
			ਨਹੀਂਤਾਂ ਜੇ (no_Magic(c) == '\'' && n == 0)
			{
			    // \%'m  \%<'m  \%>'m
			    EMIT(cmp == '<' ? NFA_MARK_LT :
				 cmp == '>' ? NFA_MARK_GT : NFA_MARK);
			    EMIT(getchr());
			    ਰੋਕੋ;
			}
		    }
		    semsg(_(e_nfa_regexp_unknown_operator_percent_chr),
								  no_Magic(c));
		    ਵਾਪਸ FAIL;
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ Magic('['):
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
	    ਜੇ (*endp == ']')
	    {
		ਪੂਰਨ plen;
		bool range_endpoint;
		/*
		 * Try to reverse engineer character classes. For example,
		 * recognize that [0-9] stands for \d and [A-Za-z_] for \h,
		 * and perform the necessary substitutions in the NFA.
		 */
		ਨਤੀਜਾ = nfa_recognize_char_class(regparse, endp,
							 extra == NFA_ਜੋੜੋ_NL);
		ਜੇ (ਨਤੀਜਾ != FAIL)
		{
		    ਜੇ (ਨਤੀਜਾ >= NFA_ਪਹਿਲਾ_NL && ਨਤੀਜਾ <= NFA_ਆਖਰੀ_NL)
		    {
			EMIT(ਨਤੀਜਾ - NFA_ਜੋੜੋ_NL);
			EMIT(NFA_NEWL);
			EMIT(NFA_OR);
		    }
		    ਨਹੀਂਤਾਂ
			EMIT(ਨਤੀਜਾ);
		    regparse = endp;
		    MB_PTR_ADV(regparse);
		    ਵਾਪਸ OK;
		}
		/*
		 * Failed to recognize a character class. Use the simple
		 * version that turns [abc] into 'a' OR 'b' OR 'c'
		 */
		startc = -1;
		negated = FALSE;
		ਜੇ (*regparse == '^')			// negated range
		{
		    negated = TRUE;
		    MB_PTR_ADV(regparse);
		    EMIT(NFA_ਸ਼ੁਰੂ_NEG_COLL);
		}
		ਨਹੀਂਤਾਂ
		    EMIT(NFA_ਸ਼ੁਰੂ_COLL);
		ਜੇ (*regparse == '-')
		{
		    startc = '-';
		    EMIT(startc);
		    EMIT(NFA_CONCAT);
		    MB_PTR_ADV(regparse);
		}
		// Emit the OR branches for each character in the []
		emit_range = FALSE;
		ਜਦੋਂ (regparse < endp)
		{
		    ਪੂਰਨ	    oldstartc = startc;
		    range_endpoint = false;

		    startc = -1;
		    got_coll_char = FALSE;
		    ਜੇ (*regparse == '[')
		    {
			// Check for [: :], [= =], [. .]
			equiclass = collclass = 0;
			charclass = get_char_class(&regparse);
			ਜੇ (charclass == CLASS_NONE)
			{
			    equiclass = get_equi_class(&regparse);
			    ਜੇ (equiclass == 0)
				collclass = get_coll_element(&regparse);
			}

			// Character class like [:alpha:]
			ਜੇ (charclass != CLASS_NONE)
			{
			    ਚੋਣ (charclass)
			    {
				ਹਾਲਤ CLASS_ALNUM:
				    EMIT(NFA_CLASS_ALNUM);
				    ਰੋਕੋ;
				ਹਾਲਤ CLASS_ALPHA:
				    EMIT(NFA_CLASS_ALPHA);
				    ਰੋਕੋ;
				ਹਾਲਤ CLASS_BLANK:
				    EMIT(NFA_CLASS_BLANK);
				    ਰੋਕੋ;
				ਹਾਲਤ CLASS_CNTRL:
				    EMIT(NFA_CLASS_CNTRL);
				    ਰੋਕੋ;
				ਹਾਲਤ CLASS_DIGIT:
				    EMIT(NFA_CLASS_DIGIT);
				    ਰੋਕੋ;
				ਹਾਲਤ CLASS_ਗ੍ਰਾਫ:
				    EMIT(NFA_CLASS_ਗ੍ਰਾਫ);
				    ਰੋਕੋ;
				ਹਾਲਤ CLASS_ਹੇਠਲਾ:
				    ਚਾਹੁੰਦਾ_nfa = TRUE;
				    EMIT(NFA_CLASS_ਹੇਠਲਾ);
				    ਰੋਕੋ;
				ਹਾਲਤ CLASS_PRINT:
				    EMIT(NFA_CLASS_PRINT);
				    ਰੋਕੋ;
				ਹਾਲਤ CLASS_PUNCT:
				    EMIT(NFA_CLASS_PUNCT);
				    ਰੋਕੋ;
				ਹਾਲਤ CLASS_SPACE:
				    EMIT(NFA_CLASS_SPACE);
				    ਰੋਕੋ;
				ਹਾਲਤ CLASS_ਉੱਤਲਾ:
				    ਚਾਹੁੰਦਾ_nfa = TRUE;
				    EMIT(NFA_CLASS_ਉੱਤਲਾ);
				    ਰੋਕੋ;
				ਹਾਲਤ CLASS_XDIGIT:
				    EMIT(NFA_CLASS_XDIGIT);
				    ਰੋਕੋ;
				ਹਾਲਤ CLASS_TAB:
				    EMIT(NFA_CLASS_TAB);
				    ਰੋਕੋ;
				ਹਾਲਤ CLASS_RETURN:
				    EMIT(NFA_CLASS_RETURN);
				    ਰੋਕੋ;
				ਹਾਲਤ CLASS_BACKSPACE:
				    EMIT(NFA_CLASS_BACKSPACE);
				    ਰੋਕੋ;
				ਹਾਲਤ CLASS_ESCAPE:
				    EMIT(NFA_CLASS_ESCAPE);
				    ਰੋਕੋ;
				ਹਾਲਤ CLASS_IDENT:
				    EMIT(NFA_CLASS_IDENT);
				    ਰੋਕੋ;
				ਹਾਲਤ CLASS_KEYWORD:
				    EMIT(NFA_CLASS_KEYWORD);
				    ਰੋਕੋ;
				ਹਾਲਤ CLASS_FNAME:
				    EMIT(NFA_CLASS_FNAME);
				    ਰੋਕੋ;
			    }
			    EMIT(NFA_CONCAT);
			    ਜਾਰੀ;
			}
			// Try equivalence class [=a=] and the like
			ਜੇ (equiclass != 0)
			{
			    ਨਤੀਜਾ = nfa_emit_equi_class(equiclass);
			    ਜੇ (ਨਤੀਜਾ == FAIL)
			    {
				// should never happen
				EMSG_RET_FAIL(_(e_error_building_nfa_with_equivalence_class));
			    }
			    ਜਾਰੀ;
			}
			// Try collating class like [. .]
			ਜੇ (collclass != 0)
			{
			    startc = collclass;	 // allow [.a.]-x as a range
			    // Will emit the proper atom at the end of the
			    // while loop.
			}
		    }
		    // Try a range like 'a-x' or '\t-z'. Also allows '-' as a
		    // start character.
		    ਜੇ (*regparse == '-' && oldstartc != -1)
		    {
			emit_range = TRUE;
			startc = oldstartc;
			MB_PTR_ADV(regparse);
			ਜਾਰੀ;	    // reading the end of the range
		    }

		    // Now handle simple and escaped characters.
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    ਜੇ (*regparse == '\\'
			    && !reg_cpo_bsl
			    && regparse + 1 <= endp
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != ਨਿਰਾ
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR, regparse[1])
								      != ਨਿਰਾ)
			    )
			)
		    {
			MB_PTR_ADV(regparse);

			ਜੇ (*regparse == 'n')
			    startc = (reg_string || emit_range
					|| regparse[1] == '-') ? NL : NFA_NEWL;
			ਨਹੀਂਤਾਂ ਜੇ (*regparse == 'd'
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
				ਜੇ (startc == ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
				    EMSG_RET_FAIL(_(e_unicode_val_too_large));
				got_coll_char = TRUE;
				MB_PTR_ਪਿੱਠ(old_regparse, regparse);
			    }
			    ਨਹੀਂਤਾਂ
			    {
				// \r,\t,\e,\b
				startc = backslash_trans(*regparse);
			    }
		    }

		    // Normal printable char
		    ਜੇ (startc == -1)
			startc = PTR2CHAR(regparse);

		    // Previous char was '-', so this char is end of range.
		    ਜੇ (emit_range)
		    {
			ਪੂਰਨ	endc = startc;
			range_endpoint = true;

			startc = oldstartc;
			ਜੇ (startc > endc)
			    EMSG_RET_FAIL(_(e_ਉਲਟ_range_ਵਿੱਚ_character_class));

			ਜੇ (endc > startc + 2)
			{
			    // Emit a range instead of the sequence of
			    // individual characters.
			    ਜੇ (startc == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    ਨਹੀਂਤਾਂ
				--post_ptr; // remove NFA_CONCAT
			    EMIT(endc);
			    EMIT(NFA_RANGE);
			    EMIT(NFA_CONCAT);
			}
			ਨਹੀਂਤਾਂ ਜੇ (has_mbyte && ((*mb_char2len)(startc) > 1
				    || (*mb_char2len)(endc) > 1))
			{
			    // Emit the characters in the range.
			    // "startc" was already emitted, so skip it.
			    //
			    ਲਈ (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			ਨਹੀਂਤਾਂ
			{
			    // Emit the range. "startc" was already emitted, so
			    // skip it.
			    ਲਈ (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			emit_range = FALSE;
			startc = -1;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// This char (startc) is not part of a range. Just
			// emit it.
			// Normally, simply emit startc. But if we get char
			// code=0 from a collating char, then replace it with
			// 0x0a.
			// This is needed to completely mimic the behaviour of
			// the backtracking engine.
			ਜੇ (startc == NFA_NEWL)
			{
			    // Line break can't be matched as part of the
			    // collection, add an OR below. But not for negated
			    // range.
			    ਜੇ (!negated)
				extra = NFA_ਜੋੜੋ_NL;
			}
			ਨਹੀਂਤਾਂ
			{
			    ਜੇ (got_coll_char == TRUE && startc == 0)
			    {
				EMIT(0x0a);
				EMIT(NFA_CONCAT);
			    }
			    ਨਹੀਂਤਾਂ
			    {
				EMIT(startc);
				ਜੇ (!(enc_utf8 && (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse)))))
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
		    ਜੇ (!range_endpoint && enc_utf8 &&
			    (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse))))
		    {
			ਪੂਰਨ i = utf_ptr2len(regparse);

			c = utf_ptr2char(regparse + i);

			// Add composing characters
			ਲਈ (;;)
			{
			    ਜੇ (c == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    ਨਹੀਂਤਾਂ
				EMIT(c);
			    EMIT(NFA_CONCAT);
			    ਜੇ ((i += utf_char2len(c)) >= plen)
				ਰੋਕੋ;
			    c = utf_ptr2char(regparse + i);
			}
			EMIT(NFA_COMPOSING);
			EMIT(NFA_CONCAT);
		    }
		    MB_PTR_ADV(regparse);
		} // while (p < endp)

		MB_PTR_ਪਿੱਠ(old_regparse, regparse);
		ਜੇ (*regparse == '-')	    // if last, '-' is just a char
		{
		    EMIT('-');
		    EMIT(NFA_CONCAT);
		}

		// skip the trailing ]
		regparse = endp;
		MB_PTR_ADV(regparse);

		// Mark end of the collection.
		ਜੇ (negated == TRUE)
		    EMIT(NFA_ਸਿਰਾ_NEG_COLL);
		ਨਹੀਂਤਾਂ
		    EMIT(NFA_ਸਿਰਾ_COLL);

		// \_[] also matches \n but it's not negated
		ਜੇ (extra == NFA_ਜੋੜੋ_NL)
		{
		    EMIT(reg_string ? NL : NFA_NEWL);
		    EMIT(NFA_OR);
		}

		ਵਾਪਸ OK;
	    } // if exists closing ]

	    ਜੇ (reg_strict)
		EMSG_RET_FAIL(_(e_missing_rsb_after_str_lsb));
	    // FALLTHROUGH

	ਮੂਲ:
	    {
		ਪੂਰਨ	plen;

nfa_do_multibyte:
		// plen is length of current char with composing chars
		ਜੇ (enc_utf8 && ((*mb_char2len)(c)
			    != (plen = utfc_ptr2len(old_regparse))
						       || utf_iscomposing(c)))
		{
		    ਪੂਰਨ i = 0;

		    // A base character plus composing characters, or just one
		    // or more composing characters.
		    // This requires creating a separate atom as if enclosing
		    // the characters in (), where NFA_COMPOSING is the ( and
		    // NFA_END_COMPOSING is the ). Note that right now we are
		    // building the postfix form, not the NFA itself;
		    // a composing char could be: a, b, c, NFA_COMPOSING
		    // where 'b' and 'c' are chars with codes > 256.
		    ਲਈ (;;)
		    {
			EMIT(c);
			ਜੇ (i > 0)
			    EMIT(NFA_CONCAT);
			ਜੇ ((i += utf_char2len(c)) >= plen)
			    ਰੋਕੋ;
			c = utf_ptr2char(old_regparse + i);
		    }
		    EMIT(NFA_COMPOSING);
		    regparse = old_regparse + plen;
		}
		ਨਹੀਂਤਾਂ
		{
		    c = no_Magic(c);
		    EMIT(c);
		}
		ਵਾਪਸ OK;
	    }
    }

    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
nfa_regpiece(ਖਾਲੀ)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		op;
    ਪੂਰਨ		ret;
    ਲੰਮਾ	minval, maxval;
    ਪੂਰਨ		greedy = TRUE;      // Braces are prefixed with '-' ?
    parse_state_T old_state;
    parse_state_T new_state;
    ਲੰਮਾ	c2;
    ਪੂਰਨ		old_post_pos;
    ਪੂਰਨ		my_post_ਸ਼ੁਰੂ;
    ਪੂਰਨ		quest;

    // Save the current parse state, so that we can use it if <atom>{m,n} is
    // next.
    save_parse_state(&old_state);

    // store current pos in the postfix form, for \{m,n} involving 0s
    my_post_ਸ਼ੁਰੂ = (ਪੂਰਨ)(post_ptr - post_ਸ਼ੁਰੂ);

    ret = nfa_regatom();
    ਜੇ (ret == FAIL)
	ਵਾਪਸ FAIL;	    // cascaded error

    op = peekchr();
    ਜੇ (re_multi_type(op) == NOT_MULTI)
	ਵਾਪਸ OK;

    skipchr();
    ਚੋਣ (op)
    {
	ਹਾਲਤ Magic('*'):
	    EMIT(NFA_ਤਾਰਾ);
	    ਰੋਕੋ;

	ਹਾਲਤ Magic('+'):
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
	    ਜੇ (nfa_regatom() == FAIL)
		ਵਾਪਸ FAIL;
	    EMIT(NFA_ਤਾਰਾ);
	    EMIT(NFA_CONCAT);
	    skipchr();		// skip the \+
	    ਰੋਕੋ;

	ਹਾਲਤ Magic('@'):
	    c2 = (ਲੰਮਾ)getdecchrs();
	    op = no_Magic(getchr());
	    i = 0;
	    ਚੋਣ(op)
	    {
		ਹਾਲਤ '=':
		    // \@=
		    i = NFA_PREV_ATOM_NO_ਚੌੜਾਈ;
		    ਰੋਕੋ;
		ਹਾਲਤ '!':
		    // \@!
		    i = NFA_PREV_ATOM_NO_ਚੌੜਾਈ_NEG;
		    ਰੋਕੋ;
		ਹਾਲਤ '<':
		    op = no_Magic(getchr());
		    ਜੇ (op == '=')
			// \@<=
			i = NFA_PREV_ATOM_JUST_BEFORE;
		    ਨਹੀਂਤਾਂ ਜੇ (op == '!')
			// \@<!
			i = NFA_PREV_ATOM_JUST_BEFORE_NEG;
		    ਰੋਕੋ;
		ਹਾਲਤ '>':
		    // \@>
		    i = NFA_PREV_ATOM_LIKE_ਨਮੂਨਾ;
		    ਰੋਕੋ;
	    }
	    ਜੇ (i == 0)
	    {
		semsg(_(e_nfa_regexp_unknown_operator_at_chr), op);
		ਵਾਪਸ FAIL;
	    }
	    EMIT(i);
	    ਜੇ (i == NFA_PREV_ATOM_JUST_BEFORE
					|| i == NFA_PREV_ATOM_JUST_BEFORE_NEG)
		EMIT(c2);
	    ਰੋਕੋ;

	ਹਾਲਤ Magic('?'):
	ਹਾਲਤ Magic('='):
	    EMIT(NFA_QUEST);
	    ਰੋਕੋ;

	ਹਾਲਤ Magic('{'):
	    // a{2,5} will expand to 'aaa?a?a?'
	    // a{-1,3} will expand to 'aa??a??', where ?? is the nongreedy
	    // version of '?'
	    // \v(ab){2,3} will expand to '(ab)(ab)(ab)?', where all the
	    // parenthesis have the same id

	    greedy = TRUE;
	    c2 = peekchr();
	    ਜੇ (c2 == '-' || c2 == Magic('-'))
	    {
		skipchr();
		greedy = FALSE;
	    }
	    ਜੇ (!read_limits(&minval, &maxval))
		EMSG_RET_FAIL(_(e_nfa_regexp_error_reading_repetition_limits));

	    //  <atom>{0,inf}, <atom>{0,} and <atom>{}  are equivalent to
	    //  <atom>*
	    ਜੇ (minval == 0 && maxval == MAX_ਹੱਦਬੰਦੀ)
	    {
		ਜੇ (greedy)		// { { (match the braces)
		    // \{}, \{0,}
		    EMIT(NFA_ਤਾਰਾ);
		ਨਹੀਂਤਾਂ			// { { (match the braces)
		    // \{-}, \{-0,}
		    EMIT(NFA_ਤਾਰਾ_NONGREEDY);
		ਰੋਕੋ;
	    }

	    // Special case: x{0} or x{-0}
	    ਜੇ (maxval == 0)
	    {
		// Ignore result of previous call to nfa_regatom()
		post_ptr = post_ਸ਼ੁਰੂ + my_post_ਸ਼ੁਰੂ;
		// NFA_EMPTY is 0-length and works everywhere
		EMIT(NFA_ਖਾਲੀ_ਥਾਵਾਂ);
		ਵਾਪਸ OK;
	    }

	    // The engine is very inefficient (uses too many states) when the
	    // maximum is much larger than the minimum and when the maximum is
	    // large.  However, when maxval is MAX_LIMIT, it is okay, as this
	    // will emit NFA_STAR.
	    // Bail out if we can use the other engine, but only, when the
	    // pattern does not need the NFA engine like (e.g. [[:upper:]]\{2,\}
	    // does not work with characters > 8 bit with the BT engine)
	    ਜੇ ((nfa_re_flags & RE_AUTO)
				   && (maxval > 500 || maxval > minval + 200)
				   && (maxval != MAX_ਹੱਦਬੰਦੀ && minval < 200)
				   && !ਚਾਹੁੰਦਾ_nfa)
		ਵਾਪਸ FAIL;

	    // Ignore previous call to nfa_regatom()
	    post_ptr = post_ਸ਼ੁਰੂ + my_post_ਸ਼ੁਰੂ;
	    // Save parse state after the repeated atom and the \{}
	    save_parse_state(&new_state);

	    quest = (greedy == TRUE? NFA_QUEST : NFA_QUEST_NONGREEDY);
	    ਲਈ (i = 0; i < maxval; i++)
	    {
		// Goto beginning of the repeated atom
		restore_parse_state(&old_state);
		old_post_pos = (ਪੂਰਨ)(post_ptr - post_ਸ਼ੁਰੂ);
		ਜੇ (nfa_regatom() == FAIL)
		    ਵਾਪਸ FAIL;
		// after "minval" times, atoms are optional
		ਜੇ (i + 1 > minval)
		{
		    ਜੇ (maxval == MAX_ਹੱਦਬੰਦੀ)
		    {
			ਜੇ (greedy)
			    EMIT(NFA_ਤਾਰਾ);
			ਨਹੀਂਤਾਂ
			    EMIT(NFA_ਤਾਰਾ_NONGREEDY);
		    }
		    ਨਹੀਂਤਾਂ
			EMIT(quest);
		}
		ਜੇ (old_post_pos != my_post_ਸ਼ੁਰੂ)
		    EMIT(NFA_CONCAT);
		ਜੇ (i + 1 > minval && maxval == MAX_ਹੱਦਬੰਦੀ)
		    ਰੋਕੋ;
	    }

	    // Go to just after the repeated atom and the \{}
	    restore_parse_state(&new_state);
	    curchr = -1;

	    ਰੋਕੋ;


	ਮੂਲ:
	    ਰੋਕੋ;
    }	// end switch

    ਜੇ (re_multi_type(peekchr()) != NOT_MULTI)
	// Can't have a multi follow a multi.
	EMSG_RET_FAIL(_(e_nfa_regexp_cant_have_multi_follow_multi));

    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
nfa_regconcat(ਖਾਲੀ)
{
    ਪੂਰਨ		cont = TRUE;
    ਪੂਰਨ		ਪਹਿਲਾ = TRUE;

    ਜਦੋਂ (cont)
    {
	ਚੋਣ (peekchr())
	{
	    ਹਾਲਤ NUL:
	    ਹਾਲਤ Magic('|'):
	    ਹਾਲਤ Magic('&'):
	    ਹਾਲਤ Magic(')'):
		cont = FALSE;
		ਰੋਕੋ;

	    ਹਾਲਤ Magic('Z'):
		regflags |= RF_ICOMBINE;
		skipchr_keepstart();
		ਰੋਕੋ;
	    ਹਾਲਤ Magic('c'):
		regflags |= RF_ICASE;
		skipchr_keepstart();
		ਰੋਕੋ;
	    ਹਾਲਤ Magic('C'):
		regflags |= RF_NOICASE;
		skipchr_keepstart();
		ਰੋਕੋ;
	    ਹਾਲਤ Magic('v'):
		reg_magic = MAGIC_ALL;
		skipchr_keepstart();
		curchr = -1;
		ਰੋਕੋ;
	    ਹਾਲਤ Magic('m'):
		reg_magic = MAGIC_ON;
		skipchr_keepstart();
		curchr = -1;
		ਰੋਕੋ;
	    ਹਾਲਤ Magic('M'):
		reg_magic = MAGIC_OFF;
		skipchr_keepstart();
		curchr = -1;
		ਰੋਕੋ;
	    ਹਾਲਤ Magic('V'):
		reg_magic = MAGIC_NONE;
		skipchr_keepstart();
		curchr = -1;
		ਰੋਕੋ;

	    ਮੂਲ:
		ਜੇ (nfa_regpiece() == FAIL)
		    ਵਾਪਸ FAIL;
		ਜੇ (ਪਹਿਲਾ == FALSE)
		    EMIT(NFA_CONCAT);
		ਨਹੀਂਤਾਂ
		    ਪਹਿਲਾ = FALSE;
		ਰੋਕੋ;
	}
    }

    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
nfa_regbranch(ਖਾਲੀ)
{
    ਪੂਰਨ		old_post_pos;

    old_post_pos = (ਪੂਰਨ)(post_ptr - post_ਸ਼ੁਰੂ);

    // First branch, possibly the only one
    ਜੇ (nfa_regconcat() == FAIL)
	ਵਾਪਸ FAIL;

    // Try next concats
    ਜਦੋਂ (peekchr() == Magic('&'))
    {
	skipchr();
	// if concat is empty do emit a node
	ਜੇ (old_post_pos == (ਪੂਰਨ)(post_ptr - post_ਸ਼ੁਰੂ))
	    EMIT(NFA_ਖਾਲੀ_ਥਾਵਾਂ);
	EMIT(NFA_NOPEN);
	EMIT(NFA_PREV_ATOM_NO_ਚੌੜਾਈ);
	old_post_pos = (ਪੂਰਨ)(post_ptr - post_ਸ਼ੁਰੂ);
	ਜੇ (nfa_regconcat() == FAIL)
	    ਵਾਪਸ FAIL;
	// if concat is empty do emit a node
	ਜੇ (old_post_pos == (ਪੂਰਨ)(post_ptr - post_ਸ਼ੁਰੂ))
	    EMIT(NFA_ਖਾਲੀ_ਥਾਵਾਂ);
	EMIT(NFA_CONCAT);
    }

    // if a branch is empty, emit one node for it
    ਜੇ (old_post_pos == (ਪੂਰਨ)(post_ptr - post_ਸ਼ੁਰੂ))
	EMIT(NFA_ਖਾਲੀ_ਥਾਵਾਂ);

    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
nfa_reg(
    ਪੂਰਨ		paren)	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
{
    ਪੂਰਨ		parno = 0;
    ਪੂਰਨ		ਸਥਿਤੀ = FAIL;

    ਜੇ (paren == REG_PAREN)
    {
	ਜੇ (regnpar >= NSUBEXP) // Too many `('
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_parens));
	parno = regnpar++;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ਨਹੀਂਤਾਂ ਜੇ (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	ਜੇ (regnzpar >= NSUBEXP)
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_z));
	parno = regnzpar++;
    }
#ਜੇ_ਅੰਤ

    ਜੇ (nfa_reg_parse_ਡੂੰਘਾਈ >= REG_MAX_PAREN_ਡੂੰਘਾਈ)
	EMSG_RET_FAIL(_(e_command_too_complex));
    ++nfa_reg_parse_ਡੂੰਘਾਈ;

    ਜੇ (nfa_regbranch() == FAIL)
	ਜਾਓ theend;	    // cascaded error

    ਜਦੋਂ (peekchr() == Magic('|'))
    {
	skipchr();
	ਜੇ (nfa_regbranch() == FAIL)
	    ਜਾਓ theend;    // cascaded error
	EMIT(NFA_OR);
    }

    // Check for proper termination.
    ਜੇ (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
	ਜੇ (paren == REG_NPAREN)
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	ਨਹੀਂਤਾਂ
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	rc_did_emsg = TRUE;
	ਜਾਓ theend;
    }
    ਨਹੀਂਤਾਂ ਜੇ (paren == REG_NOPAREN && peekchr() != NUL)
    {
	ਜੇ (peekchr() == Magic(')'))
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	ਨਹੀਂਤਾਂ
	    emsg(_(e_nfa_regexp_proper_termination_error));
	rc_did_emsg = TRUE;
	ਜਾਓ theend;
    }
    /*
     * Here we set the flag allowing back references to this set of
     * parentheses.
     */
    ਜੇ (paren == REG_PAREN)
    {
	had_endbrace[parno] = TRUE;     // have seen the close paren
	EMIT(NFA_MOPEN + parno);
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ਨਹੀਂਤਾਂ ਜੇ (paren == REG_ZPAREN)
	EMIT(NFA_ZOPEN + parno);
#ਜੇ_ਅੰਤ

    ਸਥਿਤੀ = OK;

theend:
    --nfa_reg_parse_ਡੂੰਘਾਈ;
    ਵਾਪਸ ਸਥਿਤੀ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
ਸਥਿਰ char_u ਕੋਡ[50];

    ਸਥਿਰ ਖਾਲੀ
nfa_ਸੈੱਟ_ਕੋਡ(ਪੂਰਨ c)
{
    ਪੂਰਨ	    addnl = FALSE;

    ਜੇ (c >= NFA_ਪਹਿਲਾ_NL && c <= NFA_ਆਖਰੀ_NL)
    {
	addnl = TRUE;
	c -= NFA_ਜੋੜੋ_NL;
    }

    STRCPY(ਕੋਡ, "");
    ਚੋਣ (c)
    {
	ਹਾਲਤ NFA_ਮੇਲ:	    STRCPY(ਕੋਡ, "NFA_MATCH "); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਵੰਡੋ:	    STRCPY(ਕੋਡ, "NFA_SPLIT "); ਰੋਕੋ;
	ਹਾਲਤ NFA_CONCAT:    STRCPY(ਕੋਡ, "NFA_CONCAT "); ਰੋਕੋ;
	ਹਾਲਤ NFA_NEWL:	    STRCPY(ਕੋਡ, "NFA_NEWL "); ਰੋਕੋ;
	ਹਾਲਤ NFA_ZSTART:    STRCPY(ਕੋਡ, "NFA_ZSTART"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ZEND:	    STRCPY(ਕੋਡ, "NFA_ZEND"); ਰੋਕੋ;

	ਹਾਲਤ NFA_BACKREF1:  STRCPY(ਕੋਡ, "NFA_BACKREF1"); ਰੋਕੋ;
	ਹਾਲਤ NFA_BACKREF2:  STRCPY(ਕੋਡ, "NFA_BACKREF2"); ਰੋਕੋ;
	ਹਾਲਤ NFA_BACKREF3:  STRCPY(ਕੋਡ, "NFA_BACKREF3"); ਰੋਕੋ;
	ਹਾਲਤ NFA_BACKREF4:  STRCPY(ਕੋਡ, "NFA_BACKREF4"); ਰੋਕੋ;
	ਹਾਲਤ NFA_BACKREF5:  STRCPY(ਕੋਡ, "NFA_BACKREF5"); ਰੋਕੋ;
	ਹਾਲਤ NFA_BACKREF6:  STRCPY(ਕੋਡ, "NFA_BACKREF6"); ਰੋਕੋ;
	ਹਾਲਤ NFA_BACKREF7:  STRCPY(ਕੋਡ, "NFA_BACKREF7"); ਰੋਕੋ;
	ਹਾਲਤ NFA_BACKREF8:  STRCPY(ਕੋਡ, "NFA_BACKREF8"); ਰੋਕੋ;
	ਹਾਲਤ NFA_BACKREF9:  STRCPY(ਕੋਡ, "NFA_BACKREF9"); ਰੋਕੋ;
# ifdef FEAT_SYN_HL
	ਹਾਲਤ NFA_ZREF1:	    STRCPY(ਕੋਡ, "NFA_ZREF1"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ZREF2:	    STRCPY(ਕੋਡ, "NFA_ZREF2"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ZREF3:	    STRCPY(ਕੋਡ, "NFA_ZREF3"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ZREF4:	    STRCPY(ਕੋਡ, "NFA_ZREF4"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ZREF5:	    STRCPY(ਕੋਡ, "NFA_ZREF5"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ZREF6:	    STRCPY(ਕੋਡ, "NFA_ZREF6"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ZREF7:	    STRCPY(ਕੋਡ, "NFA_ZREF7"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ZREF8:	    STRCPY(ਕੋਡ, "NFA_ZREF8"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ZREF9:	    STRCPY(ਕੋਡ, "NFA_ZREF9"); ਰੋਕੋ;
# endif
	ਹਾਲਤ NFA_SKIP:	    STRCPY(ਕੋਡ, "NFA_SKIP"); ਰੋਕੋ;

	ਹਾਲਤ NFA_PREV_ATOM_NO_ਚੌੜਾਈ:
			    STRCPY(ਕੋਡ, "NFA_PREV_ATOM_NO_WIDTH"); ਰੋਕੋ;
	ਹਾਲਤ NFA_PREV_ATOM_NO_ਚੌੜਾਈ_NEG:
			    STRCPY(ਕੋਡ, "NFA_PREV_ATOM_NO_WIDTH_NEG"); ਰੋਕੋ;
	ਹਾਲਤ NFA_PREV_ATOM_JUST_BEFORE:
			    STRCPY(ਕੋਡ, "NFA_PREV_ATOM_JUST_BEFORE"); ਰੋਕੋ;
	ਹਾਲਤ NFA_PREV_ATOM_JUST_BEFORE_NEG:
			 STRCPY(ਕੋਡ, "NFA_PREV_ATOM_JUST_BEFORE_NEG"); ਰੋਕੋ;
	ਹਾਲਤ NFA_PREV_ATOM_LIKE_ਨਮੂਨਾ:
			    STRCPY(ਕੋਡ, "NFA_PREV_ATOM_LIKE_PATTERN"); ਰੋਕੋ;

	ਹਾਲਤ NFA_NOPEN:		    STRCPY(ਕੋਡ, "NFA_NOPEN"); ਰੋਕੋ;
	ਹਾਲਤ NFA_NCLOSE:	    STRCPY(ਕੋਡ, "NFA_NCLOSE"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE:   STRCPY(ਕੋਡ, "NFA_START_INVISIBLE"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_ਪਹਿਲਾ:
			     STRCPY(ਕੋਡ, "NFA_START_INVISIBLE_FIRST"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_NEG:
			       STRCPY(ਕੋਡ, "NFA_START_INVISIBLE_NEG"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_NEG_ਪਹਿਲਾ:
			 STRCPY(ਕੋਡ, "NFA_START_INVISIBLE_NEG_FIRST"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE:
			    STRCPY(ਕੋਡ, "NFA_START_INVISIBLE_BEFORE"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_ਪਹਿਲਾ:
		      STRCPY(ਕੋਡ, "NFA_START_INVISIBLE_BEFORE_FIRST"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_NEG:
			STRCPY(ਕੋਡ, "NFA_START_INVISIBLE_BEFORE_NEG"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_NEG_ਪਹਿਲਾ:
		  STRCPY(ਕੋਡ, "NFA_START_INVISIBLE_BEFORE_NEG_FIRST"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਸ਼ੁਰੂ_ਨਮੂਨਾ:   STRCPY(ਕੋਡ, "NFA_START_PATTERN"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਸਿਰਾ_INVISIBLE:	    STRCPY(ਕੋਡ, "NFA_END_INVISIBLE"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਸਿਰਾ_INVISIBLE_NEG: STRCPY(ਕੋਡ, "NFA_END_INVISIBLE_NEG"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਸਿਰਾ_ਨਮੂਨਾ:	    STRCPY(ਕੋਡ, "NFA_END_PATTERN"); ਰੋਕੋ;

	ਹਾਲਤ NFA_COMPOSING:	    STRCPY(ਕੋਡ, "NFA_COMPOSING"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਸਿਰਾ_COMPOSING:	    STRCPY(ਕੋਡ, "NFA_END_COMPOSING"); ਰੋਕੋ;
	ਹਾਲਤ NFA_OPT_CHARS:	    STRCPY(ਕੋਡ, "NFA_OPT_CHARS"); ਰੋਕੋ;

	ਹਾਲਤ NFA_MOPEN:
	ਹਾਲਤ NFA_MOPEN1:
	ਹਾਲਤ NFA_MOPEN2:
	ਹਾਲਤ NFA_MOPEN3:
	ਹਾਲਤ NFA_MOPEN4:
	ਹਾਲਤ NFA_MOPEN5:
	ਹਾਲਤ NFA_MOPEN6:
	ਹਾਲਤ NFA_MOPEN7:
	ਹਾਲਤ NFA_MOPEN8:
	ਹਾਲਤ NFA_MOPEN9:
	    STRCPY(ਕੋਡ, "NFA_MOPEN(x)");
	    ਕੋਡ[10] = c - NFA_MOPEN + '0';
	    ਰੋਕੋ;
	ਹਾਲਤ NFA_MCLOSE:
	ਹਾਲਤ NFA_MCLOSE1:
	ਹਾਲਤ NFA_MCLOSE2:
	ਹਾਲਤ NFA_MCLOSE3:
	ਹਾਲਤ NFA_MCLOSE4:
	ਹਾਲਤ NFA_MCLOSE5:
	ਹਾਲਤ NFA_MCLOSE6:
	ਹਾਲਤ NFA_MCLOSE7:
	ਹਾਲਤ NFA_MCLOSE8:
	ਹਾਲਤ NFA_MCLOSE9:
	    STRCPY(ਕੋਡ, "NFA_MCLOSE(x)");
	    ਕੋਡ[11] = c - NFA_MCLOSE + '0';
	    ਰੋਕੋ;
# ifdef FEAT_SYN_HL
	ਹਾਲਤ NFA_ZOPEN:
	ਹਾਲਤ NFA_ZOPEN1:
	ਹਾਲਤ NFA_ZOPEN2:
	ਹਾਲਤ NFA_ZOPEN3:
	ਹਾਲਤ NFA_ZOPEN4:
	ਹਾਲਤ NFA_ZOPEN5:
	ਹਾਲਤ NFA_ZOPEN6:
	ਹਾਲਤ NFA_ZOPEN7:
	ਹਾਲਤ NFA_ZOPEN8:
	ਹਾਲਤ NFA_ZOPEN9:
	    STRCPY(ਕੋਡ, "NFA_ZOPEN(x)");
	    ਕੋਡ[10] = c - NFA_ZOPEN + '0';
	    ਰੋਕੋ;
	ਹਾਲਤ NFA_ZCLOSE:
	ਹਾਲਤ NFA_ZCLOSE1:
	ਹਾਲਤ NFA_ZCLOSE2:
	ਹਾਲਤ NFA_ZCLOSE3:
	ਹਾਲਤ NFA_ZCLOSE4:
	ਹਾਲਤ NFA_ZCLOSE5:
	ਹਾਲਤ NFA_ZCLOSE6:
	ਹਾਲਤ NFA_ZCLOSE7:
	ਹਾਲਤ NFA_ZCLOSE8:
	ਹਾਲਤ NFA_ZCLOSE9:
	    STRCPY(ਕੋਡ, "NFA_ZCLOSE(x)");
	    ਕੋਡ[11] = c - NFA_ZCLOSE + '0';
	    ਰੋਕੋ;
# endif
	ਹਾਲਤ NFA_EOL:		STRCPY(ਕੋਡ, "NFA_EOL "); ਰੋਕੋ;
	ਹਾਲਤ NFA_BOL:		STRCPY(ਕੋਡ, "NFA_BOL "); ਰੋਕੋ;
	ਹਾਲਤ NFA_EOW:		STRCPY(ਕੋਡ, "NFA_EOW "); ਰੋਕੋ;
	ਹਾਲਤ NFA_BOW:		STRCPY(ਕੋਡ, "NFA_BOW "); ਰੋਕੋ;
	ਹਾਲਤ NFA_EOF:		STRCPY(ਕੋਡ, "NFA_EOF "); ਰੋਕੋ;
	ਹਾਲਤ NFA_BOF:		STRCPY(ਕੋਡ, "NFA_BOF "); ਰੋਕੋ;
	ਹਾਲਤ NFA_LNUM:		STRCPY(ਕੋਡ, "NFA_LNUM "); ਰੋਕੋ;
	ਹਾਲਤ NFA_LNUM_GT:	STRCPY(ਕੋਡ, "NFA_LNUM_GT "); ਰੋਕੋ;
	ਹਾਲਤ NFA_LNUM_LT:	STRCPY(ਕੋਡ, "NFA_LNUM_LT "); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਖਾਨਾ:		STRCPY(ਕੋਡ, "NFA_COL "); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਖਾਨਾ_GT:	STRCPY(ਕੋਡ, "NFA_COL_GT "); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਖਾਨਾ_LT:	STRCPY(ਕੋਡ, "NFA_COL_LT "); ਰੋਕੋ;
	ਹਾਲਤ NFA_VCOL:		STRCPY(ਕੋਡ, "NFA_VCOL "); ਰੋਕੋ;
	ਹਾਲਤ NFA_VCOL_GT:	STRCPY(ਕੋਡ, "NFA_VCOL_GT "); ਰੋਕੋ;
	ਹਾਲਤ NFA_VCOL_LT:	STRCPY(ਕੋਡ, "NFA_VCOL_LT "); ਰੋਕੋ;
	ਹਾਲਤ NFA_MARK:		STRCPY(ਕੋਡ, "NFA_MARK "); ਰੋਕੋ;
	ਹਾਲਤ NFA_MARK_GT:	STRCPY(ਕੋਡ, "NFA_MARK_GT "); ਰੋਕੋ;
	ਹਾਲਤ NFA_MARK_LT:	STRCPY(ਕੋਡ, "NFA_MARK_LT "); ਰੋਕੋ;
	ਹਾਲਤ NFA_CURSOR:	STRCPY(ਕੋਡ, "NFA_CURSOR "); ਰੋਕੋ;
	ਹਾਲਤ NFA_VISUAL:	STRCPY(ਕੋਡ, "NFA_VISUAL "); ਰੋਕੋ;
	ਹਾਲਤ NFA_ANY_COMPOSING:	STRCPY(ਕੋਡ, "NFA_ANY_COMPOSING "); ਰੋਕੋ;

	ਹਾਲਤ NFA_ਤਾਰਾ:		STRCPY(ਕੋਡ, "NFA_STAR "); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਤਾਰਾ_NONGREEDY: STRCPY(ਕੋਡ, "NFA_STAR_NONGREEDY "); ਰੋਕੋ;
	ਹਾਲਤ NFA_QUEST:		STRCPY(ਕੋਡ, "NFA_QUEST"); ਰੋਕੋ;
	ਹਾਲਤ NFA_QUEST_NONGREEDY: STRCPY(ਕੋਡ, "NFA_QUEST_NON_GREEDY"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਖਾਲੀ_ਥਾਵਾਂ:		STRCPY(ਕੋਡ, "NFA_EMPTY"); ਰੋਕੋ;
	ਹਾਲਤ NFA_OR:		STRCPY(ਕੋਡ, "NFA_OR"); ਰੋਕੋ;

	ਹਾਲਤ NFA_ਸ਼ੁਰੂ_COLL:	STRCPY(ਕੋਡ, "NFA_START_COLL"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਸਿਰਾ_COLL:	STRCPY(ਕੋਡ, "NFA_END_COLL"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਸ਼ੁਰੂ_NEG_COLL: STRCPY(ਕੋਡ, "NFA_START_NEG_COLL"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਸਿਰਾ_NEG_COLL:	STRCPY(ਕੋਡ, "NFA_END_NEG_COLL"); ਰੋਕੋ;
	ਹਾਲਤ NFA_RANGE:		STRCPY(ਕੋਡ, "NFA_RANGE"); ਰੋਕੋ;
	ਹਾਲਤ NFA_RANGE_MIN:	STRCPY(ਕੋਡ, "NFA_RANGE_MIN"); ਰੋਕੋ;
	ਹਾਲਤ NFA_RANGE_MAX:	STRCPY(ਕੋਡ, "NFA_RANGE_MAX"); ਰੋਕੋ;

	ਹਾਲਤ NFA_CLASS_ALNUM:	STRCPY(ਕੋਡ, "NFA_CLASS_ALNUM"); ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_ALPHA:	STRCPY(ਕੋਡ, "NFA_CLASS_ALPHA"); ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_BLANK:	STRCPY(ਕੋਡ, "NFA_CLASS_BLANK"); ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_CNTRL:	STRCPY(ਕੋਡ, "NFA_CLASS_CNTRL"); ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_DIGIT:	STRCPY(ਕੋਡ, "NFA_CLASS_DIGIT"); ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_ਗ੍ਰਾਫ:	STRCPY(ਕੋਡ, "NFA_CLASS_GRAPH"); ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_ਹੇਠਲਾ:	STRCPY(ਕੋਡ, "NFA_CLASS_LOWER"); ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_PRINT:	STRCPY(ਕੋਡ, "NFA_CLASS_PRINT"); ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_PUNCT:	STRCPY(ਕੋਡ, "NFA_CLASS_PUNCT"); ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_SPACE:	STRCPY(ਕੋਡ, "NFA_CLASS_SPACE"); ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_ਉੱਤਲਾ:	STRCPY(ਕੋਡ, "NFA_CLASS_UPPER"); ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_XDIGIT:	STRCPY(ਕੋਡ, "NFA_CLASS_XDIGIT"); ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_TAB:	STRCPY(ਕੋਡ, "NFA_CLASS_TAB"); ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_RETURN:	STRCPY(ਕੋਡ, "NFA_CLASS_RETURN"); ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_BACKSPACE:   STRCPY(ਕੋਡ, "NFA_CLASS_BACKSPACE"); ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_ESCAPE:	STRCPY(ਕੋਡ, "NFA_CLASS_ESCAPE"); ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_IDENT:	STRCPY(ਕੋਡ, "NFA_CLASS_IDENT"); ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_KEYWORD:	STRCPY(ਕੋਡ, "NFA_CLASS_KEYWORD"); ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_FNAME:	STRCPY(ਕੋਡ, "NFA_CLASS_FNAME"); ਰੋਕੋ;

	ਹਾਲਤ NFA_ANY:	STRCPY(ਕੋਡ, "NFA_ANY"); ਰੋਕੋ;
	ਹਾਲਤ NFA_IDENT:	STRCPY(ਕੋਡ, "NFA_IDENT"); ਰੋਕੋ;
	ਹਾਲਤ NFA_SIDENT:STRCPY(ਕੋਡ, "NFA_SIDENT"); ਰੋਕੋ;
	ਹਾਲਤ NFA_KWORD:	STRCPY(ਕੋਡ, "NFA_KWORD"); ਰੋਕੋ;
	ਹਾਲਤ NFA_SKWORD:STRCPY(ਕੋਡ, "NFA_SKWORD"); ਰੋਕੋ;
	ਹਾਲਤ NFA_FNAME:	STRCPY(ਕੋਡ, "NFA_FNAME"); ਰੋਕੋ;
	ਹਾਲਤ NFA_SFNAME:STRCPY(ਕੋਡ, "NFA_SFNAME"); ਰੋਕੋ;
	ਹਾਲਤ NFA_PRINT:	STRCPY(ਕੋਡ, "NFA_PRINT"); ਰੋਕੋ;
	ਹਾਲਤ NFA_SPRINT:STRCPY(ਕੋਡ, "NFA_SPRINT"); ਰੋਕੋ;
	ਹਾਲਤ NFA_WHITE:	STRCPY(ਕੋਡ, "NFA_WHITE"); ਰੋਕੋ;
	ਹਾਲਤ NFA_NWHITE:STRCPY(ਕੋਡ, "NFA_NWHITE"); ਰੋਕੋ;
	ਹਾਲਤ NFA_DIGIT:	STRCPY(ਕੋਡ, "NFA_DIGIT"); ਰੋਕੋ;
	ਹਾਲਤ NFA_NDIGIT:STRCPY(ਕੋਡ, "NFA_NDIGIT"); ਰੋਕੋ;
	ਹਾਲਤ NFA_HEX:	STRCPY(ਕੋਡ, "NFA_HEX"); ਰੋਕੋ;
	ਹਾਲਤ NFA_NHEX:	STRCPY(ਕੋਡ, "NFA_NHEX"); ਰੋਕੋ;
	ਹਾਲਤ NFA_OCTAL:	STRCPY(ਕੋਡ, "NFA_OCTAL"); ਰੋਕੋ;
	ਹਾਲਤ NFA_NOCTAL:STRCPY(ਕੋਡ, "NFA_NOCTAL"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਸ਼ਬਦ:	STRCPY(ਕੋਡ, "NFA_WORD"); ਰੋਕੋ;
	ਹਾਲਤ NFA_NWORD:	STRCPY(ਕੋਡ, "NFA_NWORD"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਸਿਰ:	STRCPY(ਕੋਡ, "NFA_HEAD"); ਰੋਕੋ;
	ਹਾਲਤ NFA_NHEAD:	STRCPY(ਕੋਡ, "NFA_NHEAD"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ALPHA:	STRCPY(ਕੋਡ, "NFA_ALPHA"); ਰੋਕੋ;
	ਹਾਲਤ NFA_NALPHA:STRCPY(ਕੋਡ, "NFA_NALPHA"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਹੇਠਲਾ:	STRCPY(ਕੋਡ, "NFA_LOWER"); ਰੋਕੋ;
	ਹਾਲਤ NFA_NLOWER:STRCPY(ਕੋਡ, "NFA_NLOWER"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਉੱਤਲਾ:	STRCPY(ਕੋਡ, "NFA_UPPER"); ਰੋਕੋ;
	ਹਾਲਤ NFA_NUPPER:STRCPY(ਕੋਡ, "NFA_NUPPER"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਹੇਠਲਾ_IC:  STRCPY(ਕੋਡ, "NFA_LOWER_IC"); ਰੋਕੋ;
	ਹਾਲਤ NFA_NLOWER_IC: STRCPY(ਕੋਡ, "NFA_NLOWER_IC"); ਰੋਕੋ;
	ਹਾਲਤ NFA_ਉੱਤਲਾ_IC:  STRCPY(ਕੋਡ, "NFA_UPPER_IC"); ਰੋਕੋ;
	ਹਾਲਤ NFA_NUPPER_IC: STRCPY(ਕੋਡ, "NFA_NUPPER_IC"); ਰੋਕੋ;

	ਮੂਲ:
	    STRCPY(ਕੋਡ, "CHAR(x)");
	    ਕੋਡ[5] = c;
    }

    ਜੇ (addnl == TRUE)
	STRCAT(ਕੋਡ, " + NEWLINE ");

}

# ifdef ENABLE_LOG
ਸਥਿਰ FILE *log_fd;
ਸਥਿਰ char_u e_log_open_failed[] = N_("Could not open temporary log file for writing, displaying on stderr... ");

/*
 * Print the postfix notation of the current regexp.
 */
    ਸਥਿਰ ਖਾਲੀ
nfa_postfix_dump(char_u *ਵਿਅੰਜਕ, ਪੂਰਨ retval)
{
    ਪੂਰਨ *p;
    FILE *f;

    f = fopen(NFA_REGEXP_DUMP_LOG, "a");
    ਜੇ (f == ਨਿਰਾ)
	ਵਾਪਸ;

    fprintf(f, "\n-------------------------\n");
    ਜੇ (retval == FAIL)
	fprintf(f, ">>> NFA engine failed... \n");
    ਨਹੀਂਤਾਂ ਜੇ (retval == OK)
	fprintf(f, ">>> NFA engine succeeded !\n");
    fprintf(f, "Regexp: \"%s\"\nPostfix notation (char): \"", ਵਿਅੰਜਕ);
    ਲਈ (p = post_ਸ਼ੁਰੂ; *p && p < post_ptr; p++)
    {
	nfa_ਸੈੱਟ_ਕੋਡ(*p);
	fprintf(f, "%s, ", ਕੋਡ);
    }
    fprintf(f, "\"\nPostfix notation (int): ");
    ਲਈ (p = post_ਸ਼ੁਰੂ; *p && p < post_ptr; p++)
	fprintf(f, "%d ", *p);
    fprintf(f, "\n\n");
    fclose(f);
}

/*
 * Print the NFA starting with a root node "state".
 */
    ਸਥਿਰ ਖਾਲੀ
nfa_print_state(FILE *debugf, nfa_state_T *state)
{
    garray_T indent;

    ga_init2(&indent, 1, 64);
    ga_append(&indent, '\0');
    nfa_print_state2(debugf, state, &indent);
    ga_clear(&indent);
}

    ਸਥਿਰ ਖਾਲੀ
nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent)
{
    char_u  *p;

    ਜੇ (state == ਨਿਰਾ)
	ਵਾਪਸ;

    fprintf(debugf, "(%2d)", ਨਿਰਪੇਖ(state->id));

    // Output indent
    p = (char_u *)indent->ga_ਅੰਕੜੇ;
    ਜੇ (indent->ga_len >= 3)
    {
	ਪੂਰਨ	ਆਖਰੀ = indent->ga_len - 3;
	char_u	save[2];

	STRNCPY(save, &p[ਆਖਰੀ], 2);
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(&p[ਆਖਰੀ], "+-", 2);
	fprintf(debugf, " %s", p);
	STRNCPY(&p[ਆਖਰੀ], save, 2);
    }
    ਨਹੀਂਤਾਂ
	fprintf(debugf, " %s", p);

    nfa_ਸੈੱਟ_ਕੋਡ(state->c);
    fprintf(debugf, "%s (%d) (id=%d) val=%d\n",
		 ਕੋਡ,
		 state->c,
		 ਨਿਰਪੇਖ(state->id),
		 state->val);
    ਜੇ (state->id < 0)
	ਵਾਪਸ;

    state->id = ਨਿਰਪੇਖ(state->id) * -1;

    // grow indent for state->out
    indent->ga_len -= 1;
    ਜੇ (state->out1)
	GA_CONCAT_LITERAL(indent, "| ");
    ਨਹੀਂਤਾਂ
	GA_CONCAT_LITERAL(indent, "  ");
    ga_append(indent, NUL);

    nfa_print_state2(debugf, state->ਬਾਹਰਲਾ, indent);

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
    ਸਥਿਰ ਖਾਲੀ
nfa_dump(nfa_regprog_T *prog)
{
    FILE *debugf = fopen(NFA_REGEXP_DUMP_LOG, "a");

    ਜੇ (debugf == ਨਿਰਾ)
	ਵਾਪਸ;

    nfa_print_state(debugf, prog->ਸ਼ੁਰੂ);

    ਜੇ (prog->reganch)
	fprintf(debugf, "reganch: %d\n", prog->reganch);
    ਜੇ (prog->regstart != NUL)
	fprintf(debugf, "regstart: %c (decimal: %d)\n",
		prog->regstart, prog->regstart);
    ਜੇ (prog->ਮੇਲ_ਪਾਠ != ਨਿਰਾ)
	fprintf(debugf, "match_text: \"%s\"\n", prog->ਮੇਲ_ਪਾਠ);

    fclose(debugf);
}
# endif	    // ENABLE_LOG
#ਜੇ_ਅੰਤ	    // DEBUG

/*
 * Parse r.e. @expr and convert it into postfix form.
 * Return the postfix string on success, NULL otherwise.
 */
    ਸਥਿਰ ਪੂਰਨ *
re2post(ਖਾਲੀ)
{
    ਜੇ (nfa_reg(REG_NOPAREN) == FAIL)
	ਵਾਪਸ ਨਿਰਾ;
    EMIT(NFA_MOPEN);
    ਵਾਪਸ post_ਸ਼ੁਰੂ;
}

// NB. Some of the code below is inspired by Russ's.

/*
 * Represents an NFA state plus zero or one or two arrows exiting.
 * if c == MATCH, no arrows out; matching state.
 * If c == SPLIT, unlabeled arrows to out and out1 (if != NULL).
 * If c < 256, labeled arrow with character c to out.
 */

ਸਥਿਰ nfa_state_T	*state_ptr; // points to nfa_prog->state

/*
 * Allocate and initialize nfa_state_T.
 */
    ਸਥਿਰ nfa_state_T *
alloc_state(ਪੂਰਨ c, nfa_state_T *ਬਾਹਰਲਾ, nfa_state_T *out1)
{
    nfa_state_T *s;

    ਜੇ (istate >= nstate)
	ਵਾਪਸ ਨਿਰਾ;

    s = &state_ptr[istate++];

    s->c    = c;
    s->ਬਾਹਰਲਾ  = ਬਾਹਰਲਾ;
    s->out1 = out1;
    s->val  = 0;

    s->id   = istate;
    s->lastlist[0] = 0;
    s->lastlist[1] = 0;

    ਵਾਪਸ s;
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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਸੰਘ Ptrlist Ptrlist;
ਸੰਘ Ptrlist
{
    Ptrlist	*ਅਗਲਾ;
    nfa_state_T	*s;
};

ਬਣਤਰ Frag
{
    nfa_state_T *ਸ਼ੁਰੂ;
    Ptrlist	*ਬਾਹਰਲਾ;
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Frag Frag_T;

// Reused across compilations by post2nfa(); freed in free_regexp_stuff().
ਸਥਿਰ Frag_T *nfa_ਢੇਰ;
ਸਥਿਰ ਪੂਰਨ nfa_ਢੇਰ_len;

/*
 * Initialize a Frag_T struct and return it.
 */
    ਸਥਿਰ Frag_T
frag(nfa_state_T *ਸ਼ੁਰੂ, Ptrlist *ਬਾਹਰਲਾ)
{
    Frag_T n;

    n.ਸ਼ੁਰੂ = ਸ਼ੁਰੂ;
    n.ਬਾਹਰਲਾ = ਬਾਹਰਲਾ;
    ਵਾਪਸ n;
}

/*
 * Create singleton list containing just outp.
 */
    ਸਥਿਰ Ptrlist *
list1(
    nfa_state_T	**outp)
{
    Ptrlist *l;

    l = (Ptrlist *)outp;
    l->ਅਗਲਾ = ਨਿਰਾ;
    ਵਾਪਸ l;
}

/*
 * Patch the list of states at out to point to start.
 */
    ਸਥਿਰ ਖਾਲੀ
patch(Ptrlist *l, nfa_state_T *s)
{
    Ptrlist *ਅਗਲਾ;

    ਲਈ (; l; l = ਅਗਲਾ)
    {
	ਅਗਲਾ = l->ਅਗਲਾ;
	l->s = s;
    }
}


/*
 * Join the two lists l1 and l2, returning the combination.
 */
    ਸਥਿਰ Ptrlist *
append(Ptrlist *l1, Ptrlist *l2)
{
    Ptrlist *oldl1;

    oldl1 = l1;
    ਜਦੋਂ (l1->ਅਗਲਾ)
	l1 = l1->ਅਗਲਾ;
    l1->ਅਗਲਾ = l2;
    ਵਾਪਸ oldl1;
}

/*
 * Stack used for transforming postfix form into NFA.
 */
ਸਥਿਰ Frag_T ਖਾਲੀ_ਥਾਵਾਂ_2;

    ਸਥਿਰ ਖਾਲੀ
st_error(ਪੂਰਨ *postfix ਅਣਵਰਤਿਆ, ਪੂਰਨ *ਸਿਰਾ ਅਣਵਰਤਿਆ, ਪੂਰਨ *p ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NFA_REGEXP_ERROR_LOG
    FILE *df;
    ਪੂਰਨ *p2;

    df = fopen(NFA_REGEXP_ERROR_LOG, "a");
    ਜੇ (df)
    {
	fprintf(df, "Error popping the stack!\n");
# ifdef DEBUG
	fprintf(df, "Current regexp is \"%s\"\n", nfa_regengine.ਵਿਅੰਜਕ);
# endif
	fprintf(df, "Postfix form is: ");
# ifdef DEBUG
	ਲਈ (p2 = postfix; p2 < ਸਿਰਾ; p2++)
	{
	    nfa_ਸੈੱਟ_ਕੋਡ(*p2);
	    fprintf(df, "%s, ", ਕੋਡ);
	}
	nfa_ਸੈੱਟ_ਕੋਡ(*p);
	fprintf(df, "\nCurrent position is: ");
	ਲਈ (p2 = postfix; p2 <= p; p2 ++)
	{
	    nfa_ਸੈੱਟ_ਕੋਡ(*p2);
	    fprintf(df, "%s, ", ਕੋਡ);
	}
# ਨਹੀਂਤਾਂ
	ਲਈ (p2 = postfix; p2 < ਸਿਰਾ; p2++)
	    fprintf(df, "%d, ", *p2);
	fprintf(df, "\nCurrent position is: ");
	ਲਈ (p2 = postfix; p2 <= p; p2 ++)
	    fprintf(df, "%d, ", *p2);
# endif
	fprintf(df, "\n--------------------------\n");
	fclose(df);
    }
#ਜੇ_ਅੰਤ
    emsg(_(e_nfa_regexp_could_not_ਕੱਢੋ_ਢੇਰ));
}

/*
 * Push an item onto the stack.
 */
    ਸਥਿਰ ਖਾਲੀ
st_ਧੱਕੋ(Frag_T s, Frag_T **p, Frag_T *ਢੇਰ_ਸਿਰਾ)
{
    Frag_T *stackp = *p;

    ਜੇ (stackp >= ਢੇਰ_ਸਿਰਾ)
	ਵਾਪਸ;
    *stackp = s;
    *p = *p + 1;
}

/*
 * Pop an item from the stack.
 */
    ਸਥਿਰ Frag_T
st_ਕੱਢੋ(Frag_T **p, Frag_T *ਢੇਰ)
{
    Frag_T *stackp;

    *p = *p - 1;
    stackp = *p;
    ਜੇ (stackp < ਢੇਰ)
	ਵਾਪਸ ਖਾਲੀ_ਥਾਵਾਂ_2;
    ਵਾਪਸ **p;
}

/*
 * Estimate the maximum byte length of anything matching "state".
 * When unknown or unlimited return -1.
 */
    ਸਥਿਰ ਪੂਰਨ
nfa_max_ਚੌੜਾਈ(nfa_state_T *startstate, ਪੂਰਨ ਡੂੰਘਾਈ)
{
    ਪੂਰਨ		    l, r;
    nfa_state_T	    *state = startstate;
    ਪੂਰਨ		    len = 0;

    // detect looping in a NFA_SPLIT
    ਜੇ (ਡੂੰਘਾਈ > 4)
	ਵਾਪਸ -1;

    ਜਦੋਂ (state != ਨਿਰਾ)
    {
	ਚੋਣ (state->c)
	{
	    ਹਾਲਤ NFA_ਸਿਰਾ_INVISIBLE:
	    ਹਾਲਤ NFA_ਸਿਰਾ_INVISIBLE_NEG:
		// the end, return what we have
		ਵਾਪਸ len;

	    ਹਾਲਤ NFA_ਵੰਡੋ:
		// two alternatives, use the maximum
		l = nfa_max_ਚੌੜਾਈ(state->ਬਾਹਰਲਾ, ਡੂੰਘਾਈ + 1);
		r = nfa_max_ਚੌੜਾਈ(state->out1, ਡੂੰਘਾਈ + 1);
		ਜੇ (l < 0 || r < 0)
		    ਵਾਪਸ -1;
		ਵਾਪਸ len + (l > r ? l : r);

	    ਹਾਲਤ NFA_ANY:
	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_COLL:
	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_NEG_COLL:
		// matches some character, including composing chars
		ਜੇ (enc_utf8)
		    len += MB_MAXBYTES;
		ਨਹੀਂਤਾਂ ਜੇ (has_mbyte)
		    len += 2;
		ਨਹੀਂਤਾਂ
		    ++len;
		ਜੇ (state->c != NFA_ANY)
		{
		    // Skip over the compiled collection.
		    // malformed NFAs must not crash width estimation.
		    ਜੇ (state->out1 == ਨਿਰਾ || state->out1->ਬਾਹਰਲਾ == ਨਿਰਾ)
			ਵਾਪਸ -1;
		    state = state->out1->ਬਾਹਰਲਾ;
		    ਜਾਰੀ;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_DIGIT:
	    ਹਾਲਤ NFA_WHITE:
	    ਹਾਲਤ NFA_HEX:
	    ਹਾਲਤ NFA_OCTAL:
		// ascii
		++len;
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_IDENT:
	    ਹਾਲਤ NFA_SIDENT:
	    ਹਾਲਤ NFA_KWORD:
	    ਹਾਲਤ NFA_SKWORD:
	    ਹਾਲਤ NFA_FNAME:
	    ਹਾਲਤ NFA_SFNAME:
	    ਹਾਲਤ NFA_PRINT:
	    ਹਾਲਤ NFA_SPRINT:
	    ਹਾਲਤ NFA_NWHITE:
	    ਹਾਲਤ NFA_NDIGIT:
	    ਹਾਲਤ NFA_NHEX:
	    ਹਾਲਤ NFA_NOCTAL:
	    ਹਾਲਤ NFA_ਸ਼ਬਦ:
	    ਹਾਲਤ NFA_NWORD:
	    ਹਾਲਤ NFA_ਸਿਰ:
	    ਹਾਲਤ NFA_NHEAD:
	    ਹਾਲਤ NFA_ALPHA:
	    ਹਾਲਤ NFA_NALPHA:
	    ਹਾਲਤ NFA_ਹੇਠਲਾ:
	    ਹਾਲਤ NFA_NLOWER:
	    ਹਾਲਤ NFA_ਉੱਤਲਾ:
	    ਹਾਲਤ NFA_NUPPER:
	    ਹਾਲਤ NFA_ਹੇਠਲਾ_IC:
	    ਹਾਲਤ NFA_NLOWER_IC:
	    ਹਾਲਤ NFA_ਉੱਤਲਾ_IC:
	    ਹਾਲਤ NFA_NUPPER_IC:
	    ਹਾਲਤ NFA_ANY_COMPOSING:
		// possibly non-ascii
		ਜੇ (has_mbyte)
		    len += 3;
		ਨਹੀਂਤਾਂ
		    ++len;
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE:
	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_NEG:
	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE:
	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_NEG:
		// zero-width, out1 points to the END state
		state = state->out1->ਬਾਹਰਲਾ;
		ਜਾਰੀ;

	    ਹਾਲਤ NFA_BACKREF1:
	    ਹਾਲਤ NFA_BACKREF2:
	    ਹਾਲਤ NFA_BACKREF3:
	    ਹਾਲਤ NFA_BACKREF4:
	    ਹਾਲਤ NFA_BACKREF5:
	    ਹਾਲਤ NFA_BACKREF6:
	    ਹਾਲਤ NFA_BACKREF7:
	    ਹਾਲਤ NFA_BACKREF8:
	    ਹਾਲਤ NFA_BACKREF9:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	    ਹਾਲਤ NFA_ZREF1:
	    ਹਾਲਤ NFA_ZREF2:
	    ਹਾਲਤ NFA_ZREF3:
	    ਹਾਲਤ NFA_ZREF4:
	    ਹਾਲਤ NFA_ZREF5:
	    ਹਾਲਤ NFA_ZREF6:
	    ਹਾਲਤ NFA_ZREF7:
	    ਹਾਲਤ NFA_ZREF8:
	    ਹਾਲਤ NFA_ZREF9:
#ਜੇ_ਅੰਤ
	    ਹਾਲਤ NFA_NEWL:
	    ਹਾਲਤ NFA_SKIP:
		// unknown width
		ਵਾਪਸ -1;

	    ਹਾਲਤ NFA_BOL:
	    ਹਾਲਤ NFA_EOL:
	    ਹਾਲਤ NFA_BOF:
	    ਹਾਲਤ NFA_EOF:
	    ਹਾਲਤ NFA_BOW:
	    ਹਾਲਤ NFA_EOW:
	    ਹਾਲਤ NFA_MOPEN:
	    ਹਾਲਤ NFA_MOPEN1:
	    ਹਾਲਤ NFA_MOPEN2:
	    ਹਾਲਤ NFA_MOPEN3:
	    ਹਾਲਤ NFA_MOPEN4:
	    ਹਾਲਤ NFA_MOPEN5:
	    ਹਾਲਤ NFA_MOPEN6:
	    ਹਾਲਤ NFA_MOPEN7:
	    ਹਾਲਤ NFA_MOPEN8:
	    ਹਾਲਤ NFA_MOPEN9:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	    ਹਾਲਤ NFA_ZOPEN:
	    ਹਾਲਤ NFA_ZOPEN1:
	    ਹਾਲਤ NFA_ZOPEN2:
	    ਹਾਲਤ NFA_ZOPEN3:
	    ਹਾਲਤ NFA_ZOPEN4:
	    ਹਾਲਤ NFA_ZOPEN5:
	    ਹਾਲਤ NFA_ZOPEN6:
	    ਹਾਲਤ NFA_ZOPEN7:
	    ਹਾਲਤ NFA_ZOPEN8:
	    ਹਾਲਤ NFA_ZOPEN9:
	    ਹਾਲਤ NFA_ZCLOSE:
	    ਹਾਲਤ NFA_ZCLOSE1:
	    ਹਾਲਤ NFA_ZCLOSE2:
	    ਹਾਲਤ NFA_ZCLOSE3:
	    ਹਾਲਤ NFA_ZCLOSE4:
	    ਹਾਲਤ NFA_ZCLOSE5:
	    ਹਾਲਤ NFA_ZCLOSE6:
	    ਹਾਲਤ NFA_ZCLOSE7:
	    ਹਾਲਤ NFA_ZCLOSE8:
	    ਹਾਲਤ NFA_ZCLOSE9:
#ਜੇ_ਅੰਤ
	    ਹਾਲਤ NFA_MCLOSE:
	    ਹਾਲਤ NFA_MCLOSE1:
	    ਹਾਲਤ NFA_MCLOSE2:
	    ਹਾਲਤ NFA_MCLOSE3:
	    ਹਾਲਤ NFA_MCLOSE4:
	    ਹਾਲਤ NFA_MCLOSE5:
	    ਹਾਲਤ NFA_MCLOSE6:
	    ਹਾਲਤ NFA_MCLOSE7:
	    ਹਾਲਤ NFA_MCLOSE8:
	    ਹਾਲਤ NFA_MCLOSE9:
	    ਹਾਲਤ NFA_NOPEN:
	    ਹਾਲਤ NFA_NCLOSE:

	    ਹਾਲਤ NFA_LNUM_GT:
	    ਹਾਲਤ NFA_LNUM_LT:
	    ਹਾਲਤ NFA_ਖਾਨਾ_GT:
	    ਹਾਲਤ NFA_ਖਾਨਾ_LT:
	    ਹਾਲਤ NFA_VCOL_GT:
	    ਹਾਲਤ NFA_VCOL_LT:
	    ਹਾਲਤ NFA_MARK_GT:
	    ਹਾਲਤ NFA_MARK_LT:
	    ਹਾਲਤ NFA_VISUAL:
	    ਹਾਲਤ NFA_LNUM:
	    ਹਾਲਤ NFA_CURSOR:
	    ਹਾਲਤ NFA_ਖਾਨਾ:
	    ਹਾਲਤ NFA_VCOL:
	    ਹਾਲਤ NFA_MARK:

	    ਹਾਲਤ NFA_ZSTART:
	    ਹਾਲਤ NFA_ZEND:
	    ਹਾਲਤ NFA_OPT_CHARS:
	    ਹਾਲਤ NFA_ਖਾਲੀ_ਥਾਵਾਂ:
	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_ਨਮੂਨਾ:
	    ਹਾਲਤ NFA_ਸਿਰਾ_ਨਮੂਨਾ:
	    ਹਾਲਤ NFA_COMPOSING:
	    ਹਾਲਤ NFA_ਸਿਰਾ_COMPOSING:
		// zero-width
		ਰੋਕੋ;

	    ਮੂਲ:
		ਜੇ (state->c < 0)
		    // don't know what this is
		    ਵਾਪਸ -1;
		// normal character
		len += MB_CHAR2LEN(state->c);
		ਰੋਕੋ;
	}

	// normal way to continue
	state = state->ਬਾਹਰਲਾ;
    }

    // unrecognized, "cannot happen"
    ਵਾਪਸ -1;
}

/*
 * Convert a postfix form into its equivalent NFA.
 * Return the NFA start state on success, NULL otherwise.
 */
    ਸਥਿਰ nfa_state_T *
post2nfa(ਪੂਰਨ *postfix, ਪੂਰਨ *ਸਿਰਾ, ਪੂਰਨ nfa_calc_ਮਾਪ)
{
    ਪੂਰਨ		*p;
    ਪੂਰਨ		mopen;
    ਪੂਰਨ		mclose;
    Frag_T	*ਢੇਰ = ਨਿਰਾ;
    Frag_T	*stackp = ਨਿਰਾ;
    Frag_T	*ਢੇਰ_ਸਿਰਾ = ਨਿਰਾ;
    Frag_T	e1;
    Frag_T	e2;
    Frag_T	e;
    nfa_state_T	*s;
    nfa_state_T	*s1;
    nfa_state_T	*matchstate;
    nfa_state_T	*ret = ਨਿਰਾ;

    ਜੇ (postfix == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਧੱਕੋ_2(s)	    st_ਧੱਕੋ((s), &stackp, ਢੇਰ_ਸਿਰਾ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਕੱਢੋ_2()	    st_ਕੱਢੋ(&stackp, ਢੇਰ);		\
		    ਜੇ (stackp < ਢੇਰ)			\
		    {					\
			st_error(postfix, ਸਿਰਾ, p);	\
			ਵਾਪਸ ਨਿਰਾ;			\
		    }

    ਜੇ (nfa_calc_ਮਾਪ == FALSE)
    {
	// Reuse the fragment stack across compilations, growing when needed;
	// it is freed in free_regexp_stuff().
	ਜੇ (nfa_ਢੇਰ == ਨਿਰਾ || nfa_ਢੇਰ_len < nstate + 1)
	{
	    vim_free(nfa_ਢੇਰ);
	    nfa_ਢੇਰ = ALLOC_MULT(Frag_T, nstate + 1);
	    ਜੇ (nfa_ਢੇਰ == ਨਿਰਾ)
	    {
		nfa_ਢੇਰ_len = 0;
		ਵਾਪਸ ਨਿਰਾ;
	    }
	    nfa_ਢੇਰ_len = nstate + 1;
	}
	ਢੇਰ = nfa_ਢੇਰ;
	stackp = ਢੇਰ;
	ਢੇਰ_ਸਿਰਾ = ਢੇਰ + (nstate + 1);
    }

    ਲਈ (p = postfix; p < ਸਿਰਾ; ++p)
    {
	ਚੋਣ (*p)
	{
	ਹਾਲਤ NFA_CONCAT:
	    // Concatenation.
	    // Pay attention: this operator does not exist in the r.e. itself
	    // (it is implicit, really).  It is added when r.e. is translated
	    // to postfix form in re2post().
	    ਜੇ (nfa_calc_ਮਾਪ == TRUE)
	    {
		// nstate += 0;
		ਰੋਕੋ;
	    }
	    e2 = ਕੱਢੋ_2();
	    e1 = ਕੱਢੋ_2();
	    patch(e1.ਬਾਹਰਲਾ, e2.ਸ਼ੁਰੂ);
	    ਧੱਕੋ_2(frag(e1.ਸ਼ੁਰੂ, e2.ਬਾਹਰਲਾ));
	    ਰੋਕੋ;

	ਹਾਲਤ NFA_OR:
	    // Alternation
	    ਜੇ (nfa_calc_ਮਾਪ == TRUE)
	    {
		nstate++;
		ਰੋਕੋ;
	    }
	    e2 = ਕੱਢੋ_2();
	    e1 = ਕੱਢੋ_2();
	    s = alloc_state(NFA_ਵੰਡੋ, e1.ਸ਼ੁਰੂ, e2.ਸ਼ੁਰੂ);
	    ਜੇ (s == ਨਿਰਾ)
		ਜਾਓ theend;
	    ਧੱਕੋ_2(frag(s, append(e1.ਬਾਹਰਲਾ, e2.ਬਾਹਰਲਾ)));
	    ਰੋਕੋ;

	ਹਾਲਤ NFA_ਤਾਰਾ:
	    // Zero or more, prefer more
	    ਜੇ (nfa_calc_ਮਾਪ == TRUE)
	    {
		nstate++;
		ਰੋਕੋ;
	    }
	    e = ਕੱਢੋ_2();
	    s = alloc_state(NFA_ਵੰਡੋ, e.ਸ਼ੁਰੂ, ਨਿਰਾ);
	    ਜੇ (s == ਨਿਰਾ)
		ਜਾਓ theend;
	    patch(e.ਬਾਹਰਲਾ, s);
	    ਧੱਕੋ_2(frag(s, list1(&s->out1)));
	    ਰੋਕੋ;

	ਹਾਲਤ NFA_ਤਾਰਾ_NONGREEDY:
	    // Zero or more, prefer zero
	    ਜੇ (nfa_calc_ਮਾਪ == TRUE)
	    {
		nstate++;
		ਰੋਕੋ;
	    }
	    e = ਕੱਢੋ_2();
	    s = alloc_state(NFA_ਵੰਡੋ, ਨਿਰਾ, e.ਸ਼ੁਰੂ);
	    ਜੇ (s == ਨਿਰਾ)
		ਜਾਓ theend;
	    patch(e.ਬਾਹਰਲਾ, s);
	    ਧੱਕੋ_2(frag(s, list1(&s->ਬਾਹਰਲਾ)));
	    ਰੋਕੋ;

	ਹਾਲਤ NFA_QUEST:
	    // one or zero atoms=> greedy match
	    ਜੇ (nfa_calc_ਮਾਪ == TRUE)
	    {
		nstate++;
		ਰੋਕੋ;
	    }
	    e = ਕੱਢੋ_2();
	    s = alloc_state(NFA_ਵੰਡੋ, e.ਸ਼ੁਰੂ, ਨਿਰਾ);
	    ਜੇ (s == ਨਿਰਾ)
		ਜਾਓ theend;
	    ਧੱਕੋ_2(frag(s, append(e.ਬਾਹਰਲਾ, list1(&s->out1))));
	    ਰੋਕੋ;

	ਹਾਲਤ NFA_QUEST_NONGREEDY:
	    // zero or one atoms => non-greedy match
	    ਜੇ (nfa_calc_ਮਾਪ == TRUE)
	    {
		nstate++;
		ਰੋਕੋ;
	    }
	    e = ਕੱਢੋ_2();
	    s = alloc_state(NFA_ਵੰਡੋ, ਨਿਰਾ, e.ਸ਼ੁਰੂ);
	    ਜੇ (s == ਨਿਰਾ)
		ਜਾਓ theend;
	    ਧੱਕੋ_2(frag(s, append(e.ਬਾਹਰਲਾ, list1(&s->ਬਾਹਰਲਾ))));
	    ਰੋਕੋ;

	ਹਾਲਤ NFA_ਸਿਰਾ_COLL:
	ਹਾਲਤ NFA_ਸਿਰਾ_NEG_COLL:
	    // On the stack is the sequence starting with NFA_START_COLL or
	    // NFA_START_NEG_COLL and all possible characters. Patch it to
	    // add the output to the start.
	    ਜੇ (nfa_calc_ਮਾਪ == TRUE)
	    {
		nstate++;
		ਰੋਕੋ;
	    }
	    e = ਕੱਢੋ_2();
	    s = alloc_state(NFA_ਸਿਰਾ_COLL, ਨਿਰਾ, ਨਿਰਾ);
	    ਜੇ (s == ਨਿਰਾ)
		ਜਾਓ theend;
	    patch(e.ਬਾਹਰਲਾ, s);
	    e.ਸ਼ੁਰੂ->out1 = s;
	    ਧੱਕੋ_2(frag(e.ਸ਼ੁਰੂ, list1(&s->ਬਾਹਰਲਾ)));
	    ਰੋਕੋ;

	ਹਾਲਤ NFA_RANGE:
	    // Before this are two characters, the low and high end of a
	    // range.  Turn them into two states with MIN and MAX.
	    ਜੇ (nfa_calc_ਮਾਪ == TRUE)
	    {
		// nstate += 0;
		ਰੋਕੋ;
	    }
	    e2 = ਕੱਢੋ_2();
	    e1 = ਕੱਢੋ_2();
	    e2.ਸ਼ੁਰੂ->val = e2.ਸ਼ੁਰੂ->c;
	    e2.ਸ਼ੁਰੂ->c = NFA_RANGE_MAX;
	    e1.ਸ਼ੁਰੂ->val = e1.ਸ਼ੁਰੂ->c;
	    e1.ਸ਼ੁਰੂ->c = NFA_RANGE_MIN;
	    patch(e1.ਬਾਹਰਲਾ, e2.ਸ਼ੁਰੂ);
	    ਧੱਕੋ_2(frag(e1.ਸ਼ੁਰੂ, e2.ਬਾਹਰਲਾ));
	    ਰੋਕੋ;

	ਹਾਲਤ NFA_ਖਾਲੀ_ਥਾਵਾਂ:
	    // 0-length, used in a repetition with max/min count of 0
	    ਜੇ (nfa_calc_ਮਾਪ == TRUE)
	    {
		nstate++;
		ਰੋਕੋ;
	    }
	    s = alloc_state(NFA_ਖਾਲੀ_ਥਾਵਾਂ, ਨਿਰਾ, ਨਿਰਾ);
	    ਜੇ (s == ਨਿਰਾ)
		ਜਾਓ theend;
	    ਧੱਕੋ_2(frag(s, list1(&s->ਬਾਹਰਲਾ)));
	    ਰੋਕੋ;

	ਹਾਲਤ NFA_OPT_CHARS:
	  {
	    ਪੂਰਨ    n;

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
	    ਜੇ (nfa_calc_ਮਾਪ == TRUE)
	    {
		nstate += n;
		ਰੋਕੋ;
	    }
	    s = ਨਿਰਾ; // avoid compiler warning
	    e1.ਬਾਹਰਲਾ = ਨਿਰਾ; // stores list with out1's
	    s1 = ਨਿਰਾ; // previous NFA_SPLIT to connect to
	    ਜਦੋਂ (n-- > 0)
	    {
		e = ਕੱਢੋ_2(); // get character
		s = alloc_state(NFA_ਵੰਡੋ, e.ਸ਼ੁਰੂ, ਨਿਰਾ);
		ਜੇ (s == ਨਿਰਾ)
		    ਜਾਓ theend;
		ਜੇ (e1.ਬਾਹਰਲਾ == ਨਿਰਾ)
		    e1 = e;
		patch(e.ਬਾਹਰਲਾ, s1);
		append(e1.ਬਾਹਰਲਾ, list1(&s->out1));
		s1 = s;
	    }
	    ਧੱਕੋ_2(frag(s, e1.ਬਾਹਰਲਾ));
	    ਰੋਕੋ;
	  }

	ਹਾਲਤ NFA_PREV_ATOM_NO_ਚੌੜਾਈ:
	ਹਾਲਤ NFA_PREV_ATOM_NO_ਚੌੜਾਈ_NEG:
	ਹਾਲਤ NFA_PREV_ATOM_JUST_BEFORE:
	ਹਾਲਤ NFA_PREV_ATOM_JUST_BEFORE_NEG:
	ਹਾਲਤ NFA_PREV_ATOM_LIKE_ਨਮੂਨਾ:
	  {
	    ਪੂਰਨ before = (*p == NFA_PREV_ATOM_JUST_BEFORE
				      || *p == NFA_PREV_ATOM_JUST_BEFORE_NEG);
	    ਪੂਰਨ ਨਮੂਨਾ = (*p == NFA_PREV_ATOM_LIKE_ਨਮੂਨਾ);
	    ਪੂਰਨ ਸ਼ੁਰੂ_state;
	    ਪੂਰਨ ਸਿਰਾ_state;
	    ਪੂਰਨ n = 0;
	    nfa_state_T *zend;
	    nfa_state_T *skip;

	    ਚੋਣ (*p)
	    {
		ਹਾਲਤ NFA_PREV_ATOM_NO_ਚੌੜਾਈ:
		    ਸ਼ੁਰੂ_state = NFA_ਸ਼ੁਰੂ_INVISIBLE;
		    ਸਿਰਾ_state = NFA_ਸਿਰਾ_INVISIBLE;
		    ਰੋਕੋ;
		ਹਾਲਤ NFA_PREV_ATOM_NO_ਚੌੜਾਈ_NEG:
		    ਸ਼ੁਰੂ_state = NFA_ਸ਼ੁਰੂ_INVISIBLE_NEG;
		    ਸਿਰਾ_state = NFA_ਸਿਰਾ_INVISIBLE_NEG;
		    ਰੋਕੋ;
		ਹਾਲਤ NFA_PREV_ATOM_JUST_BEFORE:
		    ਸ਼ੁਰੂ_state = NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE;
		    ਸਿਰਾ_state = NFA_ਸਿਰਾ_INVISIBLE;
		    ਰੋਕੋ;
		ਹਾਲਤ NFA_PREV_ATOM_JUST_BEFORE_NEG:
		    ਸ਼ੁਰੂ_state = NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_NEG;
		    ਸਿਰਾ_state = NFA_ਸਿਰਾ_INVISIBLE_NEG;
		    ਰੋਕੋ;
		ਮੂਲ: // NFA_PREV_ATOM_LIKE_PATTERN:
		    ਸ਼ੁਰੂ_state = NFA_ਸ਼ੁਰੂ_ਨਮੂਨਾ;
		    ਸਿਰਾ_state = NFA_ਸਿਰਾ_ਨਮੂਨਾ;
		    ਰੋਕੋ;
	    }

	    ਜੇ (before)
		n = *++p; // get the count

	    // The \@= operator: match the preceding atom with zero width.
	    // The \@! operator: no match for the preceding atom.
	    // The \@<= operator: match for the preceding atom.
	    // The \@<! operator: no match for the preceding atom.
	    // Surrounds the preceding atom with START_INVISIBLE and
	    // END_INVISIBLE, similarly to MOPEN.

	    ਜੇ (nfa_calc_ਮਾਪ == TRUE)
	    {
		nstate += ਨਮੂਨਾ ? 4 : 2;
		ਰੋਕੋ;
	    }
	    e = ਕੱਢੋ_2();
	    s1 = alloc_state(ਸਿਰਾ_state, ਨਿਰਾ, ਨਿਰਾ);
	    ਜੇ (s1 == ਨਿਰਾ)
		ਜਾਓ theend;

	    s = alloc_state(ਸ਼ੁਰੂ_state, e.ਸ਼ੁਰੂ, s1);
	    ਜੇ (s == ਨਿਰਾ)
		ਜਾਓ theend;
	    ਜੇ (ਨਮੂਨਾ)
	    {
		// NFA_ZEND -> NFA_END_PATTERN -> NFA_SKIP -> what follows.
		skip = alloc_state(NFA_SKIP, ਨਿਰਾ, ਨਿਰਾ);
		ਜੇ (skip == ਨਿਰਾ)
		    ਜਾਓ theend;
		zend = alloc_state(NFA_ZEND, s1, ਨਿਰਾ);
		ਜੇ (zend == ਨਿਰਾ)
		    ਜਾਓ theend;
		s1->ਬਾਹਰਲਾ= skip;
		patch(e.ਬਾਹਰਲਾ, zend);
		ਧੱਕੋ_2(frag(s, list1(&skip->ਬਾਹਰਲਾ)));
	    }
	    ਨਹੀਂਤਾਂ
	    {
		patch(e.ਬਾਹਰਲਾ, s1);
		ਧੱਕੋ_2(frag(s, list1(&s1->ਬਾਹਰਲਾ)));
		ਜੇ (before)
		{
		    ਜੇ (n <= 0)
			// See if we can guess the maximum width, it avoids a
			// lot of pointless tries.
			n = nfa_max_ਚੌੜਾਈ(e.ਸ਼ੁਰੂ, 0);
		    s->val = n; // store the count
		}
	    }
	    ਰੋਕੋ;
	  }

	ਹਾਲਤ NFA_COMPOSING:	// char with composing char
#ਜੇ 0
	    // TODO
	    ਜੇ (regflags & RF_ICOMBINE)
	    {
		// use the base character only
	    }
#ਜੇ_ਅੰਤ
	    // FALLTHROUGH

	ਹਾਲਤ NFA_MOPEN:	// \( \) Submatch
	ਹਾਲਤ NFA_MOPEN1:
	ਹਾਲਤ NFA_MOPEN2:
	ਹਾਲਤ NFA_MOPEN3:
	ਹਾਲਤ NFA_MOPEN4:
	ਹਾਲਤ NFA_MOPEN5:
	ਹਾਲਤ NFA_MOPEN6:
	ਹਾਲਤ NFA_MOPEN7:
	ਹਾਲਤ NFA_MOPEN8:
	ਹਾਲਤ NFA_MOPEN9:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	ਹਾਲਤ NFA_ZOPEN:	// \z( \) Submatch
	ਹਾਲਤ NFA_ZOPEN1:
	ਹਾਲਤ NFA_ZOPEN2:
	ਹਾਲਤ NFA_ZOPEN3:
	ਹਾਲਤ NFA_ZOPEN4:
	ਹਾਲਤ NFA_ZOPEN5:
	ਹਾਲਤ NFA_ZOPEN6:
	ਹਾਲਤ NFA_ZOPEN7:
	ਹਾਲਤ NFA_ZOPEN8:
	ਹਾਲਤ NFA_ZOPEN9:
#ਜੇ_ਅੰਤ
	ਹਾਲਤ NFA_NOPEN:	// \%( \) "Invisible Submatch"
	    ਜੇ (nfa_calc_ਮਾਪ == TRUE)
	    {
		nstate += 2;
		ਰੋਕੋ;
	    }

	    mopen = *p;
	    ਚੋਣ (*p)
	    {
		ਹਾਲਤ NFA_NOPEN: mclose = NFA_NCLOSE; ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
		ਹਾਲਤ NFA_ZOPEN: mclose = NFA_ZCLOSE; ਰੋਕੋ;
		ਹਾਲਤ NFA_ZOPEN1: mclose = NFA_ZCLOSE1; ਰੋਕੋ;
		ਹਾਲਤ NFA_ZOPEN2: mclose = NFA_ZCLOSE2; ਰੋਕੋ;
		ਹਾਲਤ NFA_ZOPEN3: mclose = NFA_ZCLOSE3; ਰੋਕੋ;
		ਹਾਲਤ NFA_ZOPEN4: mclose = NFA_ZCLOSE4; ਰੋਕੋ;
		ਹਾਲਤ NFA_ZOPEN5: mclose = NFA_ZCLOSE5; ਰੋਕੋ;
		ਹਾਲਤ NFA_ZOPEN6: mclose = NFA_ZCLOSE6; ਰੋਕੋ;
		ਹਾਲਤ NFA_ZOPEN7: mclose = NFA_ZCLOSE7; ਰੋਕੋ;
		ਹਾਲਤ NFA_ZOPEN8: mclose = NFA_ZCLOSE8; ਰੋਕੋ;
		ਹਾਲਤ NFA_ZOPEN9: mclose = NFA_ZCLOSE9; ਰੋਕੋ;
#ਜੇ_ਅੰਤ
		ਹਾਲਤ NFA_COMPOSING: mclose = NFA_ਸਿਰਾ_COMPOSING; ਰੋਕੋ;
		ਮੂਲ:
		    // NFA_MOPEN, NFA_MOPEN1 .. NFA_MOPEN9
		    mclose = *p + NSUBEXP;
		    ਰੋਕੋ;
	    }

	    // Allow "NFA_MOPEN" as a valid postfix representation for
	    // the empty regexp "". In this case, the NFA will be
	    // NFA_MOPEN -> NFA_MCLOSE. Note that this also allows
	    // empty groups of parenthesis, and empty mbyte chars
	    ਜੇ (stackp == ਢੇਰ)
	    {
		s = alloc_state(mopen, ਨਿਰਾ, ਨਿਰਾ);
		ਜੇ (s == ਨਿਰਾ)
		    ਜਾਓ theend;
		s1 = alloc_state(mclose, ਨਿਰਾ, ਨਿਰਾ);
		ਜੇ (s1 == ਨਿਰਾ)
		    ਜਾਓ theend;
		patch(list1(&s->ਬਾਹਰਲਾ), s1);
		ਧੱਕੋ_2(frag(s, list1(&s1->ਬਾਹਰਲਾ)));
		ਰੋਕੋ;
	    }

	    // At least one node was emitted before NFA_MOPEN, so
	    // at least one node will be between NFA_MOPEN and NFA_MCLOSE
	    e = ਕੱਢੋ_2();
	    s = alloc_state(mopen, e.ਸ਼ੁਰੂ, ਨਿਰਾ);   // `('
	    ਜੇ (s == ਨਿਰਾ)
		ਜਾਓ theend;

	    s1 = alloc_state(mclose, ਨਿਰਾ, ਨਿਰਾ);   // `)'
	    ਜੇ (s1 == ਨਿਰਾ)
		ਜਾਓ theend;
	    patch(e.ਬਾਹਰਲਾ, s1);

	    ਜੇ (mopen == NFA_COMPOSING)
		// COMPOSING->out1 = END_COMPOSING
		patch(list1(&s->out1), s1);

	    ਧੱਕੋ_2(frag(s, list1(&s1->ਬਾਹਰਲਾ)));
	    ਰੋਕੋ;

	ਹਾਲਤ NFA_BACKREF1:
	ਹਾਲਤ NFA_BACKREF2:
	ਹਾਲਤ NFA_BACKREF3:
	ਹਾਲਤ NFA_BACKREF4:
	ਹਾਲਤ NFA_BACKREF5:
	ਹਾਲਤ NFA_BACKREF6:
	ਹਾਲਤ NFA_BACKREF7:
	ਹਾਲਤ NFA_BACKREF8:
	ਹਾਲਤ NFA_BACKREF9:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	ਹਾਲਤ NFA_ZREF1:
	ਹਾਲਤ NFA_ZREF2:
	ਹਾਲਤ NFA_ZREF3:
	ਹਾਲਤ NFA_ZREF4:
	ਹਾਲਤ NFA_ZREF5:
	ਹਾਲਤ NFA_ZREF6:
	ਹਾਲਤ NFA_ZREF7:
	ਹਾਲਤ NFA_ZREF8:
	ਹਾਲਤ NFA_ZREF9:
#ਜੇ_ਅੰਤ
	    ਜੇ (nfa_calc_ਮਾਪ == TRUE)
	    {
		nstate += 2;
		ਰੋਕੋ;
	    }
	    s = alloc_state(*p, ਨਿਰਾ, ਨਿਰਾ);
	    ਜੇ (s == ਨਿਰਾ)
		ਜਾਓ theend;
	    s1 = alloc_state(NFA_SKIP, ਨਿਰਾ, ਨਿਰਾ);
	    ਜੇ (s1 == ਨਿਰਾ)
		ਜਾਓ theend;
	    patch(list1(&s->ਬਾਹਰਲਾ), s1);
	    ਧੱਕੋ_2(frag(s, list1(&s1->ਬਾਹਰਲਾ)));
	    ਰੋਕੋ;

	ਹਾਲਤ NFA_LNUM:
	ਹਾਲਤ NFA_LNUM_GT:
	ਹਾਲਤ NFA_LNUM_LT:
	ਹਾਲਤ NFA_VCOL:
	ਹਾਲਤ NFA_VCOL_GT:
	ਹਾਲਤ NFA_VCOL_LT:
	ਹਾਲਤ NFA_ਖਾਨਾ:
	ਹਾਲਤ NFA_ਖਾਨਾ_GT:
	ਹਾਲਤ NFA_ਖਾਨਾ_LT:
	ਹਾਲਤ NFA_MARK:
	ਹਾਲਤ NFA_MARK_GT:
	ਹਾਲਤ NFA_MARK_LT:
	  {
	    ਪੂਰਨ n = *++p; // lnum, col or mark name

	    ਜੇ (nfa_calc_ਮਾਪ == TRUE)
	    {
		nstate += 1;
		ਰੋਕੋ;
	    }
	    s = alloc_state(p[-1], ਨਿਰਾ, ਨਿਰਾ);
	    ਜੇ (s == ਨਿਰਾ)
		ਜਾਓ theend;
	    s->val = n;
	    ਧੱਕੋ_2(frag(s, list1(&s->ਬਾਹਰਲਾ)));
	    ਰੋਕੋ;
	  }

	ਹਾਲਤ NFA_ZSTART:
	ਹਾਲਤ NFA_ZEND:
	ਮੂਲ:
	    // Operands
	    ਜੇ (nfa_calc_ਮਾਪ == TRUE)
	    {
		nstate++;
		ਰੋਕੋ;
	    }
	    s = alloc_state(*p, ਨਿਰਾ, ਨਿਰਾ);
	    ਜੇ (s == ਨਿਰਾ)
		ਜਾਓ theend;
	    ਧੱਕੋ_2(frag(s, list1(&s->ਬਾਹਰਲਾ)));
	    ਰੋਕੋ;

	} // switch(*p)

    } // for (p = postfix; *p; ++p)

    ਜੇ (nfa_calc_ਮਾਪ == TRUE)
    {
	nstate++;
	ਜਾਓ theend;	// Return value when counting size is ignored anyway
    }

    e = ਕੱਢੋ_2();
    ਜੇ (stackp != ਢੇਰ)
	EMSG_RET_NULL(_(e_nfa_regexp_while_converting_from_postfix_ਮੰਜ਼ਿਲ_nfa_too_many_stats_ਖੱਬਾ_on_ਢੇਰ));

    ਜੇ (istate >= nstate)
	EMSG_RET_NULL(_(e_nfa_regexp_not_enough_space_ਮੰਜ਼ਿਲ_store_ਸਾਰਾ_nfa));

    matchstate = &state_ptr[istate++]; // the match state
    matchstate->c = NFA_ਮੇਲ;
    matchstate->ਬਾਹਰਲਾ = matchstate->out1 = ਨਿਰਾ;
    matchstate->id = 0;

    patch(e.ਬਾਹਰਲਾ, matchstate);
    ret = e.ਸ਼ੁਰੂ;

theend:
    ਵਾਪਸ ret;

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ POP1
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ PUSH1
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ POP2
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ PUSH2
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ ਕੱਢੋ_2
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ ਧੱਕੋ_2
}

/*
 * After building the NFA program, inspect it to add optimization hints.
 */
    ਸਥਿਰ ਖਾਲੀ
nfa_postprocess(nfa_regprog_T *prog)
{
    ਪੂਰਨ i;
    ਪੂਰਨ c;

    ਲਈ (i = 0; i < prog->nstate; ++i)
    {
	c = prog->state[i].c;
	ਜੇ (c == NFA_ਸ਼ੁਰੂ_INVISIBLE
		|| c == NFA_ਸ਼ੁਰੂ_INVISIBLE_NEG
		|| c == NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE
		|| c == NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_NEG)
	{
	    ਪੂਰਨ directly;

	    // Do it directly when what follows is possibly the end of the
	    // match.
	    ਜੇ (ਮੇਲ_follows(prog->state[i].out1->ਬਾਹਰਲਾ, 0))
		directly = TRUE;
	    ਨਹੀਂਤਾਂ
	    {
		ਪੂਰਨ ch_invisible = failure_chance(prog->state[i].ਬਾਹਰਲਾ, 0);
		ਪੂਰਨ ch_follows = failure_chance(prog->state[i].out1->ਬਾਹਰਲਾ, 0);

		// Postpone when the invisible match is expensive or has a
		// lower chance of failing.
		ਜੇ (c == NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE
		     || c == NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_NEG)
		{
		    // "before" matches are very expensive when
		    // unbounded, always prefer what follows then,
		    // unless what follows will always match.
		    // Otherwise strongly prefer what follows.
		    ਜੇ (prog->state[i].val <= 0 && ch_follows > 0)
			directly = FALSE;
		    ਨਹੀਂਤਾਂ
			directly = ch_follows * 10 < ch_invisible;
		}
		ਨਹੀਂਤਾਂ
		{
		    // normal invisible, first do the one with the
		    // highest failure chance
		    directly = ch_follows < ch_invisible;
		}
	    }
	    ਜੇ (directly)
		// switch to the _FIRST state
		++prog->state[i].c;
	}
    }
}

/////////////////////////////////////////////////////////////////
// NFA execution code.
/////////////////////////////////////////////////////////////////

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ	    ਵਿੱਚ_use; // number of subexpr with useful info

    // When REG_MULTI is TRUE list.multi is used, otherwise list.line.
    ਸੰਘ
    {
	ਬਣਤਰ multipos
	{
	    linenr_T	ਸ਼ੁਰੂ_lnum;
	    linenr_T	ਸਿਰਾ_lnum;
	    colnr_T	ਸ਼ੁਰੂ_ਖਾਨਾ;
	    colnr_T	ਸਿਰਾ_ਖਾਨਾ;
	} multi[NSUBEXP];
	ਬਣਤਰ linepos
	{
	    char_u	*ਸ਼ੁਰੂ;
	    char_u	*ਸਿਰਾ;
	} ਸਤਰ[NSUBEXP];
    } list;
    colnr_T	orig_ਸ਼ੁਰੂ_ਖਾਨਾ;  // list.multi[0].start_col without \zs
} regsub_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    regsub_T	norm; // \( .. \) matches
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    regsub_T	synt; // \z( .. \) matches
#ਜੇ_ਅੰਤ
} regsubs_T;

// nfa_pim_T stores a Postponed Invisible Match.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ nfa_pim_S nfa_pim_T;
ਬਣਤਰ nfa_pim_S
{
    ਪੂਰਨ		ਨਤੀਜਾ;		// NFA_PIM_*, see below
    nfa_state_T	*state;		// the invisible match start state
    regsubs_T	subs;		// submatch info, only party used
    ਸੰਘ
    {
	lpos_T	pos;
	char_u	*ptr;
    } ਸਿਰਾ;			// where the match must end
};

// Values for done in nfa_pim_T.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NFA_PIM_ਅਣਵਰਤਿਆ   0	// pim not used
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NFA_PIM_TODO     1	// pim not done yet
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NFA_PIM_ਮੇਲ    2	// pim executed, matches
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NFA_PIM_NOMATCH  3	// pim executed, no match


// nfa_thread_T contains execution information of a NFA state
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    nfa_state_T	*state;
    ਪੂਰਨ		ਗਿਣਤੀ_2;
    nfa_pim_T	pim;		// if pim.result != NFA_PIM_UNUSED: postponed
				// invisible match
    regsubs_T	subs;		// submatch info, only party used
} nfa_thread_T;

// nfa_list_T contains the alternative NFA execution states.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    nfa_thread_T    *t;		// allocated array of states
    ਪੂਰਨ		    n;		// nr of states currently in "t"
    ਪੂਰਨ		    len;	// max nr of states in "t"
    ਪੂਰਨ		    id;		// ID of the list
    ਪੂਰਨ		    has_pim;	// TRUE when any state has a PIM
} nfa_list_T;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
ਸਥਿਰ ਖਾਲੀ log_subexpr(regsub_T *sub);

    ਸਥਿਰ ਖਾਲੀ
log_subsexpr(regsubs_T *subs)
{
    log_subexpr(&subs->norm);
# ifdef FEAT_SYN_HL
    ਜੇ (rex.nfa_has_zsubexpr)
	log_subexpr(&subs->synt);
# endif
}

    ਸਥਿਰ ਖਾਲੀ
log_subexpr(regsub_T *sub)
{
    ਪੂਰਨ j;

    ਲਈ (j = 0; j < sub->ਵਿੱਚ_use; j++)
	ਜੇ (REG_MULTI)
	    fprintf(log_fd,
		    "*** group %d, start: c=%d, l=%d, end: c=%d, l=%d\n",
		    j,
		    sub->list.multi[j].ਸ਼ੁਰੂ_ਖਾਨਾ,
		    (ਪੂਰਨ)sub->list.multi[j].ਸ਼ੁਰੂ_lnum,
		    sub->list.multi[j].ਸਿਰਾ_ਖਾਨਾ,
		    (ਪੂਰਨ)sub->list.multi[j].ਸਿਰਾ_lnum);
	ਨਹੀਂਤਾਂ
	{
	    ਅੱਖਰ *s = (ਅੱਖਰ *)sub->list.ਸਤਰ[j].ਸ਼ੁਰੂ;
	    ਅੱਖਰ *e = (ਅੱਖਰ *)sub->list.ਸਤਰ[j].ਸਿਰਾ;

	    fprintf(log_fd, "*** group %d, start: \"%s\", end: \"%s\"\n",
		    j,
		    s == ਨਿਰਾ ? "NULL" : s,
		    e == ਨਿਰਾ ? "NULL" : e);
	}
}

    ਸਥਿਰ ਅੱਖਰ *
pim_info(nfa_pim_T *pim)
{
    ਸਥਿਰ ਅੱਖਰ buf[30];

    ਜੇ (pim == ਨਿਰਾ || pim->ਨਤੀਜਾ == NFA_PIM_ਅਣਵਰਤਿਆ)
	buf[0] = NUL;
    ਨਹੀਂਤਾਂ
    {
	sprintf(buf, " PIM col %d", REG_MULTI ? (ਪੂਰਨ)pim->ਸਿਰਾ.pos.ਖਾਨਾ
		: (ਪੂਰਨ)(pim->ਸਿਰਾ.ptr - rex.ਨਿਵੇਸ਼));
    }
    ਵਾਪਸ buf;
}

#ਜੇ_ਅੰਤ

// Used during execution: whether a match has been found.
ਸਥਿਰ ਪੂਰਨ	    nfa_ਮੇਲ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
ਸਥਿਰ ਪੂਰਨ	   *nfa_timed_ਬਾਹਰਲਾ;
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ copy_sub(regsub_T *ਮੰਜ਼ਿਲ, regsub_T *from);
ਸਥਿਰ ਪੂਰਨ pim_equal(nfa_pim_T *one, nfa_pim_T *two);

/*
 * Copy postponed invisible match info from "from" to "to".
 */
    ਸਥਿਰ ਖਾਲੀ
copy_pim(nfa_pim_T *ਮੰਜ਼ਿਲ, nfa_pim_T *from)
{
    ਮੰਜ਼ਿਲ->ਨਤੀਜਾ = from->ਨਤੀਜਾ;
    ਮੰਜ਼ਿਲ->state = from->state;
    copy_sub(&ਮੰਜ਼ਿਲ->subs.norm, &from->subs.norm);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ਜੇ (rex.nfa_has_zsubexpr)
	copy_sub(&ਮੰਜ਼ਿਲ->subs.synt, &from->subs.synt);
#ਜੇ_ਅੰਤ
    ਮੰਜ਼ਿਲ->ਸਿਰਾ = from->ਸਿਰਾ;
}

    ਸਥਿਰ ਖਾਲੀ
clear_sub(regsub_T *sub)
{
    ਜੇ (REG_MULTI)
	// Use 0xff to set lnum to -1
	vim_memset(sub->list.multi, 0xff,
				  ਆਕਾਰ(ਬਣਤਰ multipos) * rex.nfa_nsubexpr);
    ਨਹੀਂਤਾਂ
	vim_memset(sub->list.ਸਤਰ, 0,
				   ਆਕਾਰ(ਬਣਤਰ linepos) * rex.nfa_nsubexpr);
    sub->ਵਿੱਚ_use = 0;
}

/*
 * Copy the submatches from "from" to "to".
 */
    ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
copy_sub(regsub_T *ਮੰਜ਼ਿਲ, regsub_T *from)
{
    ਮੰਜ਼ਿਲ->ਵਿੱਚ_use = from->ਵਿੱਚ_use;
    ਜੇ (from->ਵਿੱਚ_use <= 0)
	ਵਾਪਸ;

    // Copy the match start and end positions.  addstate() calls this for
    // every state it adds, so it is very hot.  Patterns without capturing
    // groups only use subexpression zero (the whole match), so special-case
    // "in_use == 1" with a direct assignment to avoid the mch_memmove() call.
    ਜੇ (REG_MULTI)
    {
	ਜੇ (from->ਵਿੱਚ_use == 1)
	    ਮੰਜ਼ਿਲ->list.multi[0] = from->list.multi[0];
	ਨਹੀਂਤਾਂ
	    mch_memmove(&ਮੰਜ਼ਿਲ->list.multi[0],
		    &from->list.multi[0],
		    ਆਕਾਰ(ਬਣਤਰ multipos) * from->ਵਿੱਚ_use);
	ਮੰਜ਼ਿਲ->orig_ਸ਼ੁਰੂ_ਖਾਨਾ = from->orig_ਸ਼ੁਰੂ_ਖਾਨਾ;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (from->ਵਿੱਚ_use == 1)
	    ਮੰਜ਼ਿਲ->list.ਸਤਰ[0] = from->list.ਸਤਰ[0];
	ਨਹੀਂਤਾਂ
	    mch_memmove(&ਮੰਜ਼ਿਲ->list.ਸਤਰ[0],
		    &from->list.ਸਤਰ[0],
		    ਆਕਾਰ(ਬਣਤਰ linepos) * from->ਵਿੱਚ_use);
    }
}

/*
 * Like copy_sub() but exclude the main match.
 */
    ਸਥਿਰ ਖਾਲੀ
copy_sub_off(regsub_T *ਮੰਜ਼ਿਲ, regsub_T *from)
{
    ਜੇ (ਮੰਜ਼ਿਲ->ਵਿੱਚ_use < from->ਵਿੱਚ_use)
	ਮੰਜ਼ਿਲ->ਵਿੱਚ_use = from->ਵਿੱਚ_use;
    ਜੇ (from->ਵਿੱਚ_use <= 1)
	ਵਾਪਸ;

    // Copy the match start and end positions.
    ਜੇ (REG_MULTI)
	mch_memmove(&ਮੰਜ਼ਿਲ->list.multi[1],
		&from->list.multi[1],
		ਆਕਾਰ(ਬਣਤਰ multipos) * (from->ਵਿੱਚ_use - 1));
    ਨਹੀਂਤਾਂ
	mch_memmove(&ਮੰਜ਼ਿਲ->list.ਸਤਰ[1],
		&from->list.ਸਤਰ[1],
		ਆਕਾਰ(ਬਣਤਰ linepos) * (from->ਵਿੱਚ_use - 1));
}

/*
 * Like copy_sub() but only do the end of the main match if \ze is present.
 */
    ਸਥਿਰ ਖਾਲੀ
copy_ze_off(regsub_T *ਮੰਜ਼ਿਲ, regsub_T *from)
{
    ਜੇ (!rex.nfa_has_zend)
	ਵਾਪਸ;

    ਜੇ (REG_MULTI)
    {
	ਜੇ (from->list.multi[0].ਸਿਰਾ_lnum >= 0)
	{
	    ਮੰਜ਼ਿਲ->list.multi[0].ਸਿਰਾ_lnum = from->list.multi[0].ਸਿਰਾ_lnum;
	    ਮੰਜ਼ਿਲ->list.multi[0].ਸਿਰਾ_ਖਾਨਾ = from->list.multi[0].ਸਿਰਾ_ਖਾਨਾ;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (from->list.ਸਤਰ[0].ਸਿਰਾ != ਨਿਰਾ)
	    ਮੰਜ਼ਿਲ->list.ਸਤਰ[0].ਸਿਰਾ = from->list.ਸਤਰ[0].ਸਿਰਾ;
    }
}

/*
 * Return TRUE if "sub1" and "sub2" have the same start positions.
 * When using back-references also check the end position.
 */
    ਸਥਿਰ ਪੂਰਨ
sub_equal(regsub_T *sub1, regsub_T *sub2)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		todo;
    linenr_T	s1;
    linenr_T	s2;
    char_u	*sp1;
    char_u	*sp2;

    todo = sub1->ਵਿੱਚ_use > sub2->ਵਿੱਚ_use ? sub1->ਵਿੱਚ_use : sub2->ਵਿੱਚ_use;
    ਜੇ (REG_MULTI)
    {
	ਲਈ (i = 0; i < todo; ++i)
	{
	    ਜੇ (i < sub1->ਵਿੱਚ_use)
		s1 = sub1->list.multi[i].ਸ਼ੁਰੂ_lnum;
	    ਨਹੀਂਤਾਂ
		s1 = -1;
	    ਜੇ (i < sub2->ਵਿੱਚ_use)
		s2 = sub2->list.multi[i].ਸ਼ੁਰੂ_lnum;
	    ਨਹੀਂਤਾਂ
		s2 = -1;
	    ਜੇ (s1 != s2)
		ਵਾਪਸ FALSE;
	    ਜੇ (s1 != -1 && sub1->list.multi[i].ਸ਼ੁਰੂ_ਖਾਨਾ
					     != sub2->list.multi[i].ਸ਼ੁਰੂ_ਖਾਨਾ)
		ਵਾਪਸ FALSE;

	    ਜੇ (rex.nfa_has_backref)
	    {
		ਜੇ (i < sub1->ਵਿੱਚ_use)
		    s1 = sub1->list.multi[i].ਸਿਰਾ_lnum;
		ਨਹੀਂਤਾਂ
		    s1 = -1;
		ਜੇ (i < sub2->ਵਿੱਚ_use)
		    s2 = sub2->list.multi[i].ਸਿਰਾ_lnum;
		ਨਹੀਂਤਾਂ
		    s2 = -1;
		ਜੇ (s1 != s2)
		    ਵਾਪਸ FALSE;
		ਜੇ (s1 != -1 && sub1->list.multi[i].ਸਿਰਾ_ਖਾਨਾ
					       != sub2->list.multi[i].ਸਿਰਾ_ਖਾਨਾ)
		ਵਾਪਸ FALSE;
	    }
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਲਈ (i = 0; i < todo; ++i)
	{
	    ਜੇ (i < sub1->ਵਿੱਚ_use)
		sp1 = sub1->list.ਸਤਰ[i].ਸ਼ੁਰੂ;
	    ਨਹੀਂਤਾਂ
		sp1 = ਨਿਰਾ;
	    ਜੇ (i < sub2->ਵਿੱਚ_use)
		sp2 = sub2->list.ਸਤਰ[i].ਸ਼ੁਰੂ;
	    ਨਹੀਂਤਾਂ
		sp2 = ਨਿਰਾ;
	    ਜੇ (sp1 != sp2)
		ਵਾਪਸ FALSE;
	    ਜੇ (rex.nfa_has_backref)
	    {
		ਜੇ (i < sub1->ਵਿੱਚ_use)
		    sp1 = sub1->list.ਸਤਰ[i].ਸਿਰਾ;
		ਨਹੀਂਤਾਂ
		    sp1 = ਨਿਰਾ;
		ਜੇ (i < sub2->ਵਿੱਚ_use)
		    sp2 = sub2->list.ਸਤਰ[i].ਸਿਰਾ;
		ਨਹੀਂਤਾਂ
		    sp2 = ਨਿਰਾ;
		ਜੇ (sp1 != sp2)
		    ਵਾਪਸ FALSE;
	    }
	}
    }

    ਵਾਪਸ TRUE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
/*
 * Check if we are past the time limit, if there is one.
 */
    ਸਥਿਰ ਪੂਰਨ
nfa_did_ਸਮਾਂ_ਬਾਹਰਲਾ(ਖਾਲੀ)
{
    ਜੇ (*timeout_flag)
    {
	ਜੇ (nfa_timed_ਬਾਹਰਲਾ != ਨਿਰਾ)
	{
# ifdef FEAT_EVAL
	    ਜੇ (!*nfa_timed_ਬਾਹਰਲਾ)
		ch_log(ਨਿਰਾ, "NFA regexp timed out");
# endif
	    *nfa_timed_ਬਾਹਰਲਾ = TRUE;
	}
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
    ਸਥਿਰ ਖਾਲੀ
open_debug_log(ਪੂਰਨ ਨਤੀਜਾ)
{
    log_fd = fopen(NFA_REGEXP_ਲੜੀਵਾਰ_LOG, "a");
    ਜੇ (log_fd == ਨਿਰਾ)
    {
	emsg(_(e_log_open_failed));
	log_fd = stderr;
    }

    fprintf(log_fd, "****************************\n");
    fprintf(log_fd, "FINISHED RUNNING nfa_regmatch() recursively\n");
    fprintf(log_fd, "MATCH = %s\n", ਨਤੀਜਾ == TRUE ? "OK" : ਨਤੀਜਾ == MAYBE
	    ? "MAYBE" : "FALSE");
    fprintf(log_fd, "****************************\n");
}

    ਸਥਿਰ ਖਾਲੀ
report_state(ਅੱਖਰ *action,
	     regsub_T *sub,
	     nfa_state_T *state,
	     ਪੂਰਨ lid,
	     nfa_pim_T *pim)
{
    ਪੂਰਨ ਖਾਨਾ;

    ਜੇ (sub->ਵਿੱਚ_use <= 0)
	ਖਾਨਾ = -1;
    ਨਹੀਂਤਾਂ ਜੇ (REG_MULTI)
	ਖਾਨਾ = sub->list.multi[0].ਸ਼ੁਰੂ_ਖਾਨਾ;
    ਨਹੀਂਤਾਂ
	ਖਾਨਾ = (ਪੂਰਨ)(sub->list.ਸਤਰ[0].ਸ਼ੁਰੂ - rex.ਸਤਰ);
    nfa_ਸੈੱਟ_ਕੋਡ(state->c);
    ਜੇ (log_fd == ਨਿਰਾ)
	open_debug_log(MAYBE);

    fprintf(log_fd, "> %s state %d to list %d. char %d: %s (start col %d)%s\n",
	    action, ਨਿਰਪੇਖ(state->id), lid, state->c, ਕੋਡ, ਖਾਨਾ,
	    pim_info(pim));
}
#ਜੇ_ਅੰਤ

/*
 * Return TRUE if the same state is already in list "l" with the same
 * positions as "subs".
 */
    ਸਥਿਰ ਪੂਰਨ
has_state_with_pos(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim)	// postponed match or NULL
{
    nfa_thread_T	*thread;
    ਪੂਰਨ			i;

    ਲਈ (i = 0; i < l->n; ++i)
    {
	thread = &l->t[i];
	ਜੇ (thread->state->id == state->id
		&& sub_equal(&thread->subs.norm, &subs->norm)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
		&& (!rex.nfa_has_zsubexpr
				|| sub_equal(&thread->subs.synt, &subs->synt))
#ਜੇ_ਅੰਤ
		&& pim_equal(&thread->pim, pim))
	    ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Return TRUE if "one" and "two" are equal.  That includes when both are not
 * set.
 */
    ਸਥਿਰ ਪੂਰਨ
pim_equal(nfa_pim_T *one, nfa_pim_T *two)
{
    ਪੂਰਨ one_ਅਣਵਰਤਿਆ = (one == ਨਿਰਾ || one->ਨਤੀਜਾ == NFA_PIM_ਅਣਵਰਤਿਆ);
    ਪੂਰਨ two_ਅਣਵਰਤਿਆ = (two == ਨਿਰਾ || two->ਨਤੀਜਾ == NFA_PIM_ਅਣਵਰਤਿਆ);

    ਜੇ (one_ਅਣਵਰਤਿਆ)
	// one is unused: equal when two is also unused
	ਵਾਪਸ two_ਅਣਵਰਤਿਆ;
    ਜੇ (two_ਅਣਵਰਤਿਆ)
	// one is used and two is not: not equal
	ਵਾਪਸ FALSE;
    // compare the state id
    ਜੇ (one->state->id != two->state->id)
	ਵਾਪਸ FALSE;
    // compare the position
    ਜੇ (REG_MULTI)
	ਵਾਪਸ one->ਸਿਰਾ.pos.lnum == two->ਸਿਰਾ.pos.lnum
	    && one->ਸਿਰਾ.pos.ਖਾਨਾ == two->ਸਿਰਾ.pos.ਖਾਨਾ;
    ਵਾਪਸ one->ਸਿਰਾ.ptr == two->ਸਿਰਾ.ptr;
}

/*
 * Return TRUE if "state" leads to a NFA_MATCH without advancing the input.
 */
    ਸਥਿਰ ਪੂਰਨ
ਮੇਲ_follows(nfa_state_T *startstate, ਪੂਰਨ ਡੂੰਘਾਈ)
{
    nfa_state_T	    *state = startstate;

    // avoid too much recursion
    ਜੇ (ਡੂੰਘਾਈ > 10)
	ਵਾਪਸ FALSE;

    ਜਦੋਂ (state != ਨਿਰਾ)
    {
	ਚੋਣ (state->c)
	{
	    ਹਾਲਤ NFA_ਮੇਲ:
	    ਹਾਲਤ NFA_MCLOSE:
	    ਹਾਲਤ NFA_ਸਿਰਾ_INVISIBLE:
	    ਹਾਲਤ NFA_ਸਿਰਾ_INVISIBLE_NEG:
	    ਹਾਲਤ NFA_ਸਿਰਾ_ਨਮੂਨਾ:
		ਵਾਪਸ TRUE;

	    ਹਾਲਤ NFA_ਵੰਡੋ:
		ਵਾਪਸ ਮੇਲ_follows(state->ਬਾਹਰਲਾ, ਡੂੰਘਾਈ + 1)
				     || ਮੇਲ_follows(state->out1, ਡੂੰਘਾਈ + 1);

	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE:
	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_ਪਹਿਲਾ:
	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE:
	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_ਪਹਿਲਾ:
	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_NEG:
	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_NEG_ਪਹਿਲਾ:
	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_NEG:
	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_NEG_ਪਹਿਲਾ:
	    ਹਾਲਤ NFA_COMPOSING:
		// skip ahead to next state
		state = state->out1->ਬਾਹਰਲਾ;
		ਜਾਰੀ;

	    ਹਾਲਤ NFA_ANY:
	    ਹਾਲਤ NFA_ANY_COMPOSING:
	    ਹਾਲਤ NFA_IDENT:
	    ਹਾਲਤ NFA_SIDENT:
	    ਹਾਲਤ NFA_KWORD:
	    ਹਾਲਤ NFA_SKWORD:
	    ਹਾਲਤ NFA_FNAME:
	    ਹਾਲਤ NFA_SFNAME:
	    ਹਾਲਤ NFA_PRINT:
	    ਹਾਲਤ NFA_SPRINT:
	    ਹਾਲਤ NFA_WHITE:
	    ਹਾਲਤ NFA_NWHITE:
	    ਹਾਲਤ NFA_DIGIT:
	    ਹਾਲਤ NFA_NDIGIT:
	    ਹਾਲਤ NFA_HEX:
	    ਹਾਲਤ NFA_NHEX:
	    ਹਾਲਤ NFA_OCTAL:
	    ਹਾਲਤ NFA_NOCTAL:
	    ਹਾਲਤ NFA_ਸ਼ਬਦ:
	    ਹਾਲਤ NFA_NWORD:
	    ਹਾਲਤ NFA_ਸਿਰ:
	    ਹਾਲਤ NFA_NHEAD:
	    ਹਾਲਤ NFA_ALPHA:
	    ਹਾਲਤ NFA_NALPHA:
	    ਹਾਲਤ NFA_ਹੇਠਲਾ:
	    ਹਾਲਤ NFA_NLOWER:
	    ਹਾਲਤ NFA_ਉੱਤਲਾ:
	    ਹਾਲਤ NFA_NUPPER:
	    ਹਾਲਤ NFA_ਹੇਠਲਾ_IC:
	    ਹਾਲਤ NFA_NLOWER_IC:
	    ਹਾਲਤ NFA_ਉੱਤਲਾ_IC:
	    ਹਾਲਤ NFA_NUPPER_IC:
	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_COLL:
	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_NEG_COLL:
	    ਹਾਲਤ NFA_NEWL:
		// state will advance input
		ਵਾਪਸ FALSE;

	    ਮੂਲ:
		ਜੇ (state->c > 0)
		    // state will advance input
		    ਵਾਪਸ FALSE;

		// Others: zero-width or possibly zero-width, might still find
		// a match at the same position, keep looking.
		ਰੋਕੋ;
	}
	state = state->ਬਾਹਰਲਾ;
    }
    ਵਾਪਸ FALSE;
}


/*
 * Return TRUE if "state" is already in list "l".
 */
    ਸਥਿਰ ਪੂਰਨ
state_ਵਿੱਚ_list(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs)	// pointers to subexpressions
{
    ਜੇ (state->lastlist[nfa_ll_ਸੂਚਕਾਂਕ] == l->id)
    {
	ਜੇ (!rex.nfa_has_backref || has_state_with_pos(l, state, subs, ਨਿਰਾ))
	    ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

// Offset used for "off" by addstate_here().
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ADDSTATE_ਇੱਥੇ_ਵਿੱਥ 10

/*
 * Add "state" and possibly what follows to state list ".".
 * Returns "subs_arg", possibly copied into temp_subs.
 * Returns NULL when recursiveness is too deep or timed out.
 */
    ਸਥਿਰ regsubs_T *
addstate(
    nfa_list_T		*l,	    // runtime state list
    nfa_state_T		*state,	    // state to update
    regsubs_T		*subs_arg,  // pointers to subexpressions
    nfa_pim_T		*pim,	    // postponed look-behind match
    ਪੂਰਨ			off_arg,    // byte offset, when -1 go to next line
    ਪੂਰਨ			ਡੂੰਘਾਈ)	    // recursion depth
{
    ਪੂਰਨ			subidx;
    ਪੂਰਨ			off = off_arg;
    ਪੂਰਨ			ਜੋੜੋ_ਇੱਥੇ = FALSE;
    ਪੂਰਨ			listindex = 0;
    ਪੂਰਨ			k;
    ਪੂਰਨ			ਮਿਲਿਆ = FALSE;
    nfa_thread_T	*thread;
    ਬਣਤਰ multipos	save_multipos;
    ਪੂਰਨ			save_ਵਿੱਚ_use;
    char_u		*save_ptr;
    ਪੂਰਨ			i;
    regsub_T		*sub;
    regsubs_T		*subs = subs_arg;
    ਸਥਿਰ regsubs_T	ਆਰਜ਼ੀ_subs;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
    ਪੂਰਨ			did_print = FALSE;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
    ਜੇ (nfa_did_ਸਮਾਂ_ਬਾਹਰਲਾ())
	ਵਾਪਸ ਨਿਰਾ;
#ਜੇ_ਅੰਤ

    // This function is called recursively.  When the depth is too much we run
    // out of stack and crash, limit recursiveness here.
    ਜੇ (ਡੂੰਘਾਈ >= 5000 || subs == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (off_arg <= -ADDSTATE_ਇੱਥੇ_ਵਿੱਥ)
    {
	ਜੋੜੋ_ਇੱਥੇ = TRUE;
	off = 0;
	listindex = -(off_arg + ADDSTATE_ਇੱਥੇ_ਵਿੱਥ);
    }

    ਚੋਣ (state->c)
    {
	ਹਾਲਤ NFA_NCLOSE:
	ਹਾਲਤ NFA_MCLOSE:
	ਹਾਲਤ NFA_MCLOSE1:
	ਹਾਲਤ NFA_MCLOSE2:
	ਹਾਲਤ NFA_MCLOSE3:
	ਹਾਲਤ NFA_MCLOSE4:
	ਹਾਲਤ NFA_MCLOSE5:
	ਹਾਲਤ NFA_MCLOSE6:
	ਹਾਲਤ NFA_MCLOSE7:
	ਹਾਲਤ NFA_MCLOSE8:
	ਹਾਲਤ NFA_MCLOSE9:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	ਹਾਲਤ NFA_ZCLOSE:
	ਹਾਲਤ NFA_ZCLOSE1:
	ਹਾਲਤ NFA_ZCLOSE2:
	ਹਾਲਤ NFA_ZCLOSE3:
	ਹਾਲਤ NFA_ZCLOSE4:
	ਹਾਲਤ NFA_ZCLOSE5:
	ਹਾਲਤ NFA_ZCLOSE6:
	ਹਾਲਤ NFA_ZCLOSE7:
	ਹਾਲਤ NFA_ZCLOSE8:
	ਹਾਲਤ NFA_ZCLOSE9:
#ਜੇ_ਅੰਤ
	ਹਾਲਤ NFA_MOPEN:
	ਹਾਲਤ NFA_ZEND:
	ਹਾਲਤ NFA_ਵੰਡੋ:
	ਹਾਲਤ NFA_ਖਾਲੀ_ਥਾਵਾਂ:
	    // These nodes are not added themselves but their "out" and/or
	    // "out1" may be added below.
	    ਰੋਕੋ;

	ਹਾਲਤ NFA_BOL:
	ਹਾਲਤ NFA_BOF:
	    // "^" won't match past end-of-line, don't bother trying.
	    // Except when at the end of the line, or when we are going to the
	    // next line for a look-behind match.
	    ਜੇ (rex.ਨਿਵੇਸ਼ > rex.ਸਤਰ
		    && *rex.ਨਿਵੇਸ਼ != NUL
		    && (nfa_endp == ਨਿਰਾ
			|| !REG_MULTI
			|| rex.lnum == nfa_endp->se_u.pos.lnum))
		ਜਾਓ skip_ਜੋੜੋ;
	    // FALLTHROUGH

	ਹਾਲਤ NFA_MOPEN1:
	ਹਾਲਤ NFA_MOPEN2:
	ਹਾਲਤ NFA_MOPEN3:
	ਹਾਲਤ NFA_MOPEN4:
	ਹਾਲਤ NFA_MOPEN5:
	ਹਾਲਤ NFA_MOPEN6:
	ਹਾਲਤ NFA_MOPEN7:
	ਹਾਲਤ NFA_MOPEN8:
	ਹਾਲਤ NFA_MOPEN9:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	ਹਾਲਤ NFA_ZOPEN:
	ਹਾਲਤ NFA_ZOPEN1:
	ਹਾਲਤ NFA_ZOPEN2:
	ਹਾਲਤ NFA_ZOPEN3:
	ਹਾਲਤ NFA_ZOPEN4:
	ਹਾਲਤ NFA_ZOPEN5:
	ਹਾਲਤ NFA_ZOPEN6:
	ਹਾਲਤ NFA_ZOPEN7:
	ਹਾਲਤ NFA_ZOPEN8:
	ਹਾਲਤ NFA_ZOPEN9:
#ਜੇ_ਅੰਤ
	ਹਾਲਤ NFA_NOPEN:
	ਹਾਲਤ NFA_ZSTART:
	    // These nodes need to be added so that we can bail out when it
	    // was added to this list before at the same position to avoid an
	    // endless loop for "\(\)*"

	ਮੂਲ:
	    ਜੇ (state->lastlist[nfa_ll_ਸੂਚਕਾਂਕ] == l->id && state->c != NFA_SKIP)
	    {
		// This state is already in the list, don't add it again,
		// unless it is an MOPEN that is used for a backreference or
		// when there is a PIM. For NFA_MATCH check the position,
		// lower position is preferred.
		ਜੇ (!rex.nfa_has_backref && pim == ਨਿਰਾ && !l->has_pim
						     && state->c != NFA_ਮੇਲ)
		{
		    // When called from addstate_here() do insert before
		    // existing states.
		    ਜੇ (ਜੋੜੋ_ਇੱਥੇ)
		    {
			ਲਈ (k = 0; k < l->n && k < listindex; ++k)
			    ਜੇ (l->t[k].state->id == state->id)
			    {
				ਮਿਲਿਆ = TRUE;
				ਰੋਕੋ;
			    }
		    }
		    ਜੇ (!ਜੋੜੋ_ਇੱਥੇ || ਮਿਲਿਆ)
		    {
skip_ਜੋੜੋ:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
			nfa_ਸੈੱਟ_ਕੋਡ(state->c);
			fprintf(log_fd, "> Not adding state %d to list %d. char %d: %s pim: %s has_pim: %d found: %d\n",
			    ਨਿਰਪੇਖ(state->id), l->id, state->c, ਕੋਡ,
			    pim == ਨਿਰਾ ? "NULL" : "yes", l->has_pim, ਮਿਲਿਆ);
#ਜੇ_ਅੰਤ
			ਵਾਪਸ subs;
		    }
		}

		// Do not add the state again when it exists with the same
		// positions.
		ਜੇ (has_state_with_pos(l, state, subs, pim))
		    ਜਾਓ skip_ਜੋੜੋ;
	    }

	    // When there are backreferences or PIMs the number of states may
	    // be (a lot) bigger than anticipated.
	    ਜੇ (l->n == l->len)
	    {
		ਪੂਰਨ		newlen = l->len * 3 / 2 + 50;
		size_t		newsize = newlen * ਆਕਾਰ(nfa_thread_T);
		nfa_thread_T	*newt;

		ਜੇ ((ਲੰਮਾ)(newsize >> 10) >= p_mmp)
		{
		    emsg(_(e_ਨਮੂਨਾ_ਵਰਤਦਾ_ਹੋਰ_ਸਮ੍ਰਿਤੀ_than_maxmempattern));
		    ਵਾਪਸ ਨਿਰਾ;
		}
		ਜੇ (subs != &ਆਰਜ਼ੀ_subs)
		{
		    // "subs" may point into the current array, need to make a
		    // copy before it becomes invalid.
		    copy_sub(&ਆਰਜ਼ੀ_subs.norm, &subs->norm);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
		    ਜੇ (rex.nfa_has_zsubexpr)
			copy_sub(&ਆਰਜ਼ੀ_subs.synt, &subs->synt);
#ਜੇ_ਅੰਤ
		    subs = &ਆਰਜ਼ੀ_subs;
		}

		newt = vim_realloc(l->t, newsize);
		ਜੇ (newt == ਨਿਰਾ)
		{
		    // out of memory
		    ਵਾਪਸ ਨਿਰਾ;
		}
		l->t = newt;
		l->len = newlen;
	    }

	    // add the state to the list
	    state->lastlist[nfa_ll_ਸੂਚਕਾਂਕ] = l->id;
	    thread = &l->t[l->n++];
	    thread->state = state;
	    ਜੇ (pim == ਨਿਰਾ)
		thread->pim.ਨਤੀਜਾ = NFA_PIM_ਅਣਵਰਤਿਆ;
	    ਨਹੀਂਤਾਂ
	    {
		copy_pim(&thread->pim, pim);
		l->has_pim = TRUE;
	    }
	    copy_sub(&thread->subs.norm, &subs->norm);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	    ਜੇ (rex.nfa_has_zsubexpr)
		copy_sub(&thread->subs.synt, &subs->synt);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
	    report_state("Adding", &thread->subs.norm, state, l->id, pim);
	    did_print = TRUE;
#ਜੇ_ਅੰਤ
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
    ਜੇ (!did_print)
	report_state("Processing", &subs->norm, state, l->id, pim);
#ਜੇ_ਅੰਤ
    ਚੋਣ (state->c)
    {
	ਹਾਲਤ NFA_ਮੇਲ:
	    ਰੋਕੋ;

	ਹਾਲਤ NFA_ਵੰਡੋ:
	    // order matters here
	    subs = addstate(l, state->ਬਾਹਰਲਾ, subs, pim, off_arg, ਡੂੰਘਾਈ + 1);
	    subs = addstate(l, state->out1, subs, pim, off_arg, ਡੂੰਘਾਈ + 1);
	    ਰੋਕੋ;

	ਹਾਲਤ NFA_ਖਾਲੀ_ਥਾਵਾਂ:
	ਹਾਲਤ NFA_NOPEN:
	ਹਾਲਤ NFA_NCLOSE:
	    subs = addstate(l, state->ਬਾਹਰਲਾ, subs, pim, off_arg, ਡੂੰਘਾਈ + 1);
	    ਰੋਕੋ;

	ਹਾਲਤ NFA_MOPEN:
	ਹਾਲਤ NFA_MOPEN1:
	ਹਾਲਤ NFA_MOPEN2:
	ਹਾਲਤ NFA_MOPEN3:
	ਹਾਲਤ NFA_MOPEN4:
	ਹਾਲਤ NFA_MOPEN5:
	ਹਾਲਤ NFA_MOPEN6:
	ਹਾਲਤ NFA_MOPEN7:
	ਹਾਲਤ NFA_MOPEN8:
	ਹਾਲਤ NFA_MOPEN9:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	ਹਾਲਤ NFA_ZOPEN:
	ਹਾਲਤ NFA_ZOPEN1:
	ਹਾਲਤ NFA_ZOPEN2:
	ਹਾਲਤ NFA_ZOPEN3:
	ਹਾਲਤ NFA_ZOPEN4:
	ਹਾਲਤ NFA_ZOPEN5:
	ਹਾਲਤ NFA_ZOPEN6:
	ਹਾਲਤ NFA_ZOPEN7:
	ਹਾਲਤ NFA_ZOPEN8:
	ਹਾਲਤ NFA_ZOPEN9:
#ਜੇ_ਅੰਤ
	ਹਾਲਤ NFA_ZSTART:
	    ਜੇ (state->c == NFA_ZSTART)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	    ਨਹੀਂਤਾਂ ਜੇ (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
	    {
		subidx = state->c - NFA_ZOPEN;
		sub = &subs->synt;
	    }
#ਜੇ_ਅੰਤ
	    ਨਹੀਂਤਾਂ
	    {
		subidx = state->c - NFA_MOPEN;
		sub = &subs->norm;
	    }

	    // avoid compiler warnings
	    save_ptr = ਨਿਰਾ;
	    CLEAR_ਖੇਤਰ(save_multipos);

	    // Set the position (with "off" added) in the subexpression.  Save
	    // and restore it when it was in use.  Otherwise fill any gap.
	    ਜੇ (REG_MULTI)
	    {
		ਜੇ (subidx < sub->ਵਿੱਚ_use)
		{
		    save_multipos = sub->list.multi[subidx];
		    save_ਵਿੱਚ_use = -1;
		}
		ਨਹੀਂਤਾਂ
		{
		    save_ਵਿੱਚ_use = sub->ਵਿੱਚ_use;
		    ਲਈ (i = sub->ਵਿੱਚ_use; i < subidx; ++i)
		    {
			sub->list.multi[i].ਸ਼ੁਰੂ_lnum = -1;
			sub->list.multi[i].ਸਿਰਾ_lnum = -1;
		    }
		    sub->ਵਿੱਚ_use = subidx + 1;
		}
		ਜੇ (off == -1)
		{
		    sub->list.multi[subidx].ਸ਼ੁਰੂ_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].ਸ਼ੁਰੂ_ਖਾਨਾ = 0;
		}
		ਨਹੀਂਤਾਂ
		{
		    sub->list.multi[subidx].ਸ਼ੁਰੂ_lnum = rex.lnum;
		    sub->list.multi[subidx].ਸ਼ੁਰੂ_ਖਾਨਾ =
					 (colnr_T)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ + off);
		}
		sub->list.multi[subidx].ਸਿਰਾ_lnum = -1;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (subidx < sub->ਵਿੱਚ_use)
		{
		    save_ptr = sub->list.ਸਤਰ[subidx].ਸ਼ੁਰੂ;
		    save_ਵਿੱਚ_use = -1;
		}
		ਨਹੀਂਤਾਂ
		{
		    save_ਵਿੱਚ_use = sub->ਵਿੱਚ_use;
		    ਲਈ (i = sub->ਵਿੱਚ_use; i < subidx; ++i)
		    {
			sub->list.ਸਤਰ[i].ਸ਼ੁਰੂ = ਨਿਰਾ;
			sub->list.ਸਤਰ[i].ਸਿਰਾ = ਨਿਰਾ;
		    }
		    sub->ਵਿੱਚ_use = subidx + 1;
		}
		sub->list.ਸਤਰ[subidx].ਸ਼ੁਰੂ = rex.ਨਿਵੇਸ਼ + off;
	    }

	    subs = addstate(l, state->ਬਾਹਰਲਾ, subs, pim, off_arg, ਡੂੰਘਾਈ + 1);
	    ਜੇ (subs == ਨਿਰਾ)
		ਰੋਕੋ;
	    // "subs" may have changed, need to set "sub" again
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	    ਜੇ (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
		sub = &subs->synt;
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		sub = &subs->norm;

	    ਜੇ (save_ਵਿੱਚ_use == -1)
	    {
		ਜੇ (REG_MULTI)
		    sub->list.multi[subidx] = save_multipos;
		ਨਹੀਂਤਾਂ
		    sub->list.ਸਤਰ[subidx].ਸ਼ੁਰੂ = save_ptr;
	    }
	    ਨਹੀਂਤਾਂ
		sub->ਵਿੱਚ_use = save_ਵਿੱਚ_use;
	    ਰੋਕੋ;

	ਹਾਲਤ NFA_MCLOSE:
	    ਜੇ (rex.nfa_has_zend && (REG_MULTI
			? subs->norm.list.multi[0].ਸਿਰਾ_lnum >= 0
			: subs->norm.list.ਸਤਰ[0].ਸਿਰਾ != ਨਿਰਾ))
	    {
		// Do not overwrite the position set by \ze.
		subs = addstate(l, state->ਬਾਹਰਲਾ, subs, pim, off_arg, ਡੂੰਘਾਈ + 1);
		ਰੋਕੋ;
	    }
	    // FALLTHROUGH
	ਹਾਲਤ NFA_MCLOSE1:
	ਹਾਲਤ NFA_MCLOSE2:
	ਹਾਲਤ NFA_MCLOSE3:
	ਹਾਲਤ NFA_MCLOSE4:
	ਹਾਲਤ NFA_MCLOSE5:
	ਹਾਲਤ NFA_MCLOSE6:
	ਹਾਲਤ NFA_MCLOSE7:
	ਹਾਲਤ NFA_MCLOSE8:
	ਹਾਲਤ NFA_MCLOSE9:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	ਹਾਲਤ NFA_ZCLOSE:
	ਹਾਲਤ NFA_ZCLOSE1:
	ਹਾਲਤ NFA_ZCLOSE2:
	ਹਾਲਤ NFA_ZCLOSE3:
	ਹਾਲਤ NFA_ZCLOSE4:
	ਹਾਲਤ NFA_ZCLOSE5:
	ਹਾਲਤ NFA_ZCLOSE6:
	ਹਾਲਤ NFA_ZCLOSE7:
	ਹਾਲਤ NFA_ZCLOSE8:
	ਹਾਲਤ NFA_ZCLOSE9:
#ਜੇ_ਅੰਤ
	ਹਾਲਤ NFA_ZEND:
	    ਜੇ (state->c == NFA_ZEND)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	    ਨਹੀਂਤਾਂ ਜੇ (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
	    {
		subidx = state->c - NFA_ZCLOSE;
		sub = &subs->synt;
	    }
#ਜੇ_ਅੰਤ
	    ਨਹੀਂਤਾਂ
	    {
		subidx = state->c - NFA_MCLOSE;
		sub = &subs->norm;
	    }

	    // We don't fill in gaps here, there must have been an MOPEN that
	    // has done that.
	    save_ਵਿੱਚ_use = sub->ਵਿੱਚ_use;
	    ਜੇ (sub->ਵਿੱਚ_use <= subidx)
		sub->ਵਿੱਚ_use = subidx + 1;
	    ਜੇ (REG_MULTI)
	    {
		save_multipos = sub->list.multi[subidx];
		ਜੇ (off == -1)
		{
		    sub->list.multi[subidx].ਸਿਰਾ_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].ਸਿਰਾ_ਖਾਨਾ = 0;
		}
		ਨਹੀਂਤਾਂ
		{
		    sub->list.multi[subidx].ਸਿਰਾ_lnum = rex.lnum;
		    sub->list.multi[subidx].ਸਿਰਾ_ਖਾਨਾ =
					  (colnr_T)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ + off);
		}
		// avoid compiler warnings
		save_ptr = ਨਿਰਾ;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		save_ptr = sub->list.ਸਤਰ[subidx].ਸਿਰਾ;
		sub->list.ਸਤਰ[subidx].ਸਿਰਾ = rex.ਨਿਵੇਸ਼ + off;
		// avoid compiler warnings
		CLEAR_ਖੇਤਰ(save_multipos);
	    }

	    subs = addstate(l, state->ਬਾਹਰਲਾ, subs, pim, off_arg, ਡੂੰਘਾਈ + 1);
	    ਜੇ (subs == ਨਿਰਾ)
		ਰੋਕੋ;
	    // "subs" may have changed, need to set "sub" again
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	    ਜੇ (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
		sub = &subs->synt;
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		sub = &subs->norm;

	    ਜੇ (REG_MULTI)
		sub->list.multi[subidx] = save_multipos;
	    ਨਹੀਂਤਾਂ
		sub->list.ਸਤਰ[subidx].ਸਿਰਾ = save_ptr;
	    sub->ਵਿੱਚ_use = save_ਵਿੱਚ_use;
	    ਰੋਕੋ;
    }
    ਵਾਪਸ subs;
}

/*
 * Like addstate(), but the new state(s) are put at position "*ip".
 * Used for zero-width matches, next state to use is the added one.
 * This makes sure the order of states to be tried does not change, which
 * matters for alternatives.
 */
    ਸਥਿਰ regsubs_T *
addstate_ਇੱਥੇ(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim,   // postponed look-behind match
    ਪੂਰਨ			*ip)
{
    ਪੂਰਨ tlen = l->n;
    ਪੂਰਨ ਗਿਣਤੀ_2;
    ਪੂਰਨ listidx = *ip;
    regsubs_T *r;

    // First add the state(s) at the end, so that we know how many there are.
    // Pass the listidx as offset (avoids adding another argument to
    // addstate()).
    r = addstate(l, state, subs, pim, -listidx - ADDSTATE_ਇੱਥੇ_ਵਿੱਥ, 0);
    ਜੇ (r == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // when "*ip" was at the end of the list, nothing to do
    ਜੇ (listidx + 1 == tlen)
	ਵਾਪਸ r;

    // re-order to put the new state at the current position
    ਗਿਣਤੀ_2 = l->n - tlen;
    ਜੇ (ਗਿਣਤੀ_2 == 0)
	ਵਾਪਸ r; // no state got added
    ਜੇ (ਗਿਣਤੀ_2 == 1)
    {
	// overwrite the current state
	l->t[listidx] = l->t[l->n - 1];
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਗਿਣਤੀ_2 > 1)
    {
	ਜੇ (l->n + ਗਿਣਤੀ_2 - 1 >= l->len)
	{
	    // not enough space to move the new states, reallocate the list
	    // and move the states to the right position
	    ਪੂਰਨ		    newlen = l->len * 3 / 2 + 50;
	    size_t	    newsize = newlen * ਆਕਾਰ(nfa_thread_T);
	    nfa_thread_T    *newl;

	    ਜੇ ((ਲੰਮਾ)(newsize >> 10) >= p_mmp)
	    {
		emsg(_(e_ਨਮੂਨਾ_ਵਰਤਦਾ_ਹੋਰ_ਸਮ੍ਰਿਤੀ_than_maxmempattern));
		ਵਾਪਸ ਨਿਰਾ;
	    }
	    newl = alloc(newsize);
	    ਜੇ (newl == ਨਿਰਾ)
		ਵਾਪਸ ਨਿਰਾ;
	    l->len = newlen;
	    mch_memmove(&(newl[0]),
		    &(l->t[0]),
		    ਆਕਾਰ(nfa_thread_T) * listidx);
	    mch_memmove(&(newl[listidx]),
		    &(l->t[l->n - ਗਿਣਤੀ_2]),
		    ਆਕਾਰ(nfa_thread_T) * ਗਿਣਤੀ_2);
	    mch_memmove(&(newl[listidx + ਗਿਣਤੀ_2]),
		    &(l->t[listidx + 1]),
		    ਆਕਾਰ(nfa_thread_T) * (l->n - ਗਿਣਤੀ_2 - listidx - 1));
	    vim_free(l->t);
	    l->t = newl;
	}
	ਨਹੀਂਤਾਂ
	{
	    // make space for new states, then move them from the
	    // end to the current position
	    mch_memmove(&(l->t[listidx + ਗਿਣਤੀ_2]),
		    &(l->t[listidx + 1]),
		    ਆਕਾਰ(nfa_thread_T) * (l->n - listidx - 1));
	    mch_memmove(&(l->t[listidx]),
		    &(l->t[l->n - 1]),
		    ਆਕਾਰ(nfa_thread_T) * ਗਿਣਤੀ_2);
	}
    }
    --l->n;
    *ip = listidx - 1;

    ਵਾਪਸ r;
}

/*
 * Check character class "class" against current character c.
 */
    ਸਥਿਰ ਪੂਰਨ
check_char_class(ਪੂਰਨ class, ਪੂਰਨ c)
{
    ਚੋਣ (class)
    {
	ਹਾਲਤ NFA_CLASS_ALNUM:
	    ਜੇ (c >= 1 && c < 128 && ਅੱਖਰ_ਜਾਂ_ਅੰਕ(c))
		ਵਾਪਸ OK;
	    ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_ALPHA:
	    ਜੇ (c >= 1 && c < 128 && ਅੱਖਰ_ਹੈ(c))
		ਵਾਪਸ OK;
	    ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_BLANK:
	    ਜੇ (c == ' ' || c == '\t')
		ਵਾਪਸ OK;
	    ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_CNTRL:
	    ਜੇ (c >= 1 && c <= 127 && iscntrl(c))
		ਵਾਪਸ OK;
	    ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_DIGIT:
	    ਜੇ (VIM_ISDIGIT(c))
		ਵਾਪਸ OK;
	    ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_ਗ੍ਰਾਫ:
	    ਜੇ (c >= 1 && c <= 127 && isgraph(c))
		ਵਾਪਸ OK;
	    ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_ਹੇਠਲਾ:
	    ਜੇ (MB_ISLOWER(c) && c != 170 && c != 186)
		ਵਾਪਸ OK;
	    ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_PRINT:
	    ਜੇ (vim_isprintc(c))
		ਵਾਪਸ OK;
	    ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_PUNCT:
	    ਜੇ (c >= 1 && c < 128 && ispunct(c))
		ਵਾਪਸ OK;
	    ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_SPACE:
	    ਜੇ ((c >= 9 && c <= 13) || (c == ' '))
		ਵਾਪਸ OK;
	    ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_ਉੱਤਲਾ:
	    ਜੇ (MB_ISUPPER(c))
		ਵਾਪਸ OK;
	    ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_XDIGIT:
	    ਜੇ (vim_isxdigit(c))
		ਵਾਪਸ OK;
	    ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_TAB:
	    ਜੇ (c == '\t')
		ਵਾਪਸ OK;
	    ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_RETURN:
	    ਜੇ (c == '\r')
		ਵਾਪਸ OK;
	    ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_BACKSPACE:
	    ਜੇ (c == '\b')
		ਵਾਪਸ OK;
	    ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_ESCAPE:
	    ਜੇ (c == '\033')
		ਵਾਪਸ OK;
	    ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_IDENT:
	    ਜੇ (vim_isIDc(c))
		ਵਾਪਸ OK;
	    ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_KEYWORD:
	    ਜੇ (reg_iswordc(c))
		ਵਾਪਸ OK;
	    ਰੋਕੋ;
	ਹਾਲਤ NFA_CLASS_FNAME:
	    ਜੇ (vim_isfilec(c))
		ਵਾਪਸ OK;
	    ਰੋਕੋ;

	ਮੂਲ:
	    // should not be here :P
	    siemsg(_(e_nfa_regexp_invalid_character_class_nr), class);
	    ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ FAIL;
}

/*
 * Check for a match with subexpression "subidx".
 * Return TRUE if it matches.
 */
    ਸਥਿਰ ਪੂਰਨ
ਮੇਲ_backref(
    regsub_T	*sub,	    // pointers to subexpressions
    ਪੂਰਨ		subidx,
    ਪੂਰਨ		*bytelen)   // out: length of match in bytes
{
    ਪੂਰਨ		len;

    ਜੇ (sub->ਵਿੱਚ_use <= subidx)
    {
retempty:
	// backref was not set, match an empty string
	*bytelen = 0;
	ਵਾਪਸ TRUE;
    }

    ਜੇ (REG_MULTI)
    {
	ਜੇ (sub->list.multi[subidx].ਸ਼ੁਰੂ_lnum < 0
				       || sub->list.multi[subidx].ਸਿਰਾ_lnum < 0)
	    ਜਾਓ retempty;
	ਜੇ (sub->list.multi[subidx].ਸ਼ੁਰੂ_lnum == rex.lnum
			       && sub->list.multi[subidx].ਸਿਰਾ_lnum == rex.lnum)
	{
	    len = sub->list.multi[subidx].ਸਿਰਾ_ਖਾਨਾ
					  - sub->list.multi[subidx].ਸ਼ੁਰੂ_ਖਾਨਾ;
	    ਜੇ (cstrncmp(rex.ਸਤਰ + sub->list.multi[subidx].ਸ਼ੁਰੂ_ਖਾਨਾ,
							 rex.ਨਿਵੇਸ਼, &len) == 0)
	    {
		*bytelen = len;
		ਵਾਪਸ TRUE;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (ਮੇਲ_with_backref(
			sub->list.multi[subidx].ਸ਼ੁਰੂ_lnum,
			sub->list.multi[subidx].ਸ਼ੁਰੂ_ਖਾਨਾ,
			sub->list.multi[subidx].ਸਿਰਾ_lnum,
			sub->list.multi[subidx].ਸਿਰਾ_ਖਾਨਾ,
			bytelen) == RA_ਮੇਲ)
		ਵਾਪਸ TRUE;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (sub->list.ਸਤਰ[subidx].ਸ਼ੁਰੂ == ਨਿਰਾ
					|| sub->list.ਸਤਰ[subidx].ਸਿਰਾ == ਨਿਰਾ)
	    ਜਾਓ retempty;
	len = (ਪੂਰਨ)(sub->list.ਸਤਰ[subidx].ਸਿਰਾ - sub->list.ਸਤਰ[subidx].ਸ਼ੁਰੂ);
	ਜੇ (cstrncmp(sub->list.ਸਤਰ[subidx].ਸ਼ੁਰੂ, rex.ਨਿਵੇਸ਼, &len) == 0)
	{
	    *bytelen = len;
	    ਵਾਪਸ TRUE;
	}
    }
    ਵਾਪਸ FALSE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL

/*
 * Check for a match with \z subexpression "subidx".
 * Return TRUE if it matches.
 */
    ਸਥਿਰ ਪੂਰਨ
ਮੇਲ_zref(
    ਪੂਰਨ		subidx,
    ਪੂਰਨ		*bytelen)   // out: length of match in bytes
{
    ਪੂਰਨ		len;

    cleanup_zsubexpr();
    ਜੇ (re_extmatch_ਵਿੱਚ == ਨਿਰਾ || re_extmatch_ਵਿੱਚ->matches[subidx] == ਨਿਰਾ)
    {
	// backref was not set, match an empty string
	*bytelen = 0;
	ਵਾਪਸ TRUE;
    }

    len = (ਪੂਰਨ)STRLEN(re_extmatch_ਵਿੱਚ->matches[subidx]);
    ਜੇ (cstrncmp(re_extmatch_ਵਿੱਚ->matches[subidx], rex.ਨਿਵੇਸ਼, &len) == 0)
    {
	*bytelen = len;
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ

/*
 * Save list IDs for all NFA states of "prog" into "list".
 * Also reset the IDs to zero.
 * Only used for the recursive value lastlist[1].
 */
    ਸਥਿਰ ਖਾਲੀ
nfa_save_listids(nfa_regprog_T *prog, ਪੂਰਨ *list)
{
    ਪੂਰਨ		    i;
    nfa_state_T	    *p;

    // Order in the list is reverse, it's a bit faster that way.
    p = &prog->state[0];
    ਲਈ (i = prog->nstate; --i >= 0; )
    {
	list[i] = p->lastlist[1];
	p->lastlist[1] = 0;
	++p;
    }
}

/*
 * Restore list IDs from "list" to all NFA states.
 */
    ਸਥਿਰ ਖਾਲੀ
nfa_restore_listids(nfa_regprog_T *prog, ਪੂਰਨ *list)
{
    ਪੂਰਨ		    i;
    nfa_state_T	    *p;

    p = &prog->state[0];
    ਲਈ (i = prog->nstate; --i >= 0; )
    {
	p->lastlist[1] = list[i];
	++p;
    }
}

    ਸਥਿਰ ਪੂਰਨ
nfa_re_ਅੰਕ_cmp(long_u val, ਪੂਰਨ op, long_u pos)
{
    ਜੇ (op == 1) ਵਾਪਸ pos > val;
    ਜੇ (op == 2) ਵਾਪਸ pos < val;
    ਵਾਪਸ val == pos;
}

ਸਥਿਰ ਪੂਰਨ nfa_regmatch(nfa_regprog_T *prog, nfa_state_T *ਸ਼ੁਰੂ, regsubs_T *submatch, regsubs_T *m);

/*
 * Recursively call nfa_regmatch()
 * "pim" is NULL or contains info about a Postponed Invisible Match (start
 * position).
 */
    ਸਥਿਰ ਪੂਰਨ
recursive_regmatch(
    nfa_state_T	    *state,
    nfa_pim_T	    *pim,
    nfa_regprog_T   *prog,
    regsubs_T	    *submatch,
    regsubs_T	    *m,
    ਪੂਰਨ		    **listids,
    ਪੂਰਨ		    *listids_len)
{
    ਪੂਰਨ		save_reginput_ਖਾਨਾ = (ਪੂਰਨ)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ);
    ਪੂਰਨ		save_reglnum = rex.lnum;
    ਪੂਰਨ		save_nfa_ਮੇਲ = nfa_ਮੇਲ;
    ਪੂਰਨ		save_nfa_listid = rex.nfa_listid;
    save_se_T   *save_nfa_endp = nfa_endp;
    save_se_T   endpos;
    save_se_T   *endposp = ਨਿਰਾ;
    ਪੂਰਨ		ਨਤੀਜਾ;
    ਪੂਰਨ		need_restore = FALSE;

    ਜੇ (pim != ਨਿਰਾ)
    {
	// start at the position where the postponed match was
	ਜੇ (REG_MULTI)
	    rex.ਨਿਵੇਸ਼ = rex.ਸਤਰ + pim->ਸਿਰਾ.pos.ਖਾਨਾ;
	ਨਹੀਂਤਾਂ
	    rex.ਨਿਵੇਸ਼ = pim->ਸਿਰਾ.ptr;
    }

    ਜੇ (state->c == NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE
	    || state->c == NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_ਪਹਿਲਾ
	    || state->c == NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_NEG
	    || state->c == NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_NEG_ਪਹਿਲਾ)
    {
	// The recursive match must end at the current position. When "pim" is
	// not NULL it specifies the current position.
	endposp = &endpos;
	ਜੇ (REG_MULTI)
	{
	    ਜੇ (pim == ਨਿਰਾ)
	    {
		endpos.se_u.pos.ਖਾਨਾ = (ਪੂਰਨ)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ);
		endpos.se_u.pos.lnum = rex.lnum;
	    }
	    ਨਹੀਂਤਾਂ
		endpos.se_u.pos = pim->ਸਿਰਾ.pos;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (pim == ਨਿਰਾ)
		endpos.se_u.ptr = rex.ਨਿਵੇਸ਼;
	    ਨਹੀਂਤਾਂ
		endpos.se_u.ptr = pim->ਸਿਰਾ.ptr;
	}

	// Go back the specified number of bytes, or as far as the
	// start of the previous line, to try matching "\@<=" or
	// not matching "\@<!". This is very inefficient, limit the number of
	// bytes if possible.
	ਜੇ (state->val <= 0)
	{
	    ਜੇ (REG_MULTI)
	    {
		rex.ਸਤਰ = reg_getline(--rex.lnum);
		ਜੇ (rex.ਸਤਰ == ਨਿਰਾ)
		    // can't go before the first line
		    rex.ਸਤਰ = reg_getline(++rex.lnum);
	    }
	    rex.ਨਿਵੇਸ਼ = rex.ਸਤਰ;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (REG_MULTI && (ਪੂਰਨ)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ) < state->val)
	    {
		// Not enough bytes in this line, go to end of
		// previous line.
		rex.ਸਤਰ = reg_getline(--rex.lnum);
		ਜੇ (rex.ਸਤਰ == ਨਿਰਾ)
		{
		    // can't go before the first line
		    rex.ਸਤਰ = reg_getline(++rex.lnum);
		    rex.ਨਿਵੇਸ਼ = rex.ਸਤਰ;
		}
		ਨਹੀਂਤਾਂ
		    rex.ਨਿਵੇਸ਼ = rex.ਸਤਰ + reg_getline_len(rex.lnum);
	    }
	    ਜੇ ((ਪੂਰਨ)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ) >= state->val)
	    {
		rex.ਨਿਵੇਸ਼ -= state->val;
		ਜੇ (has_mbyte)
		    rex.ਨਿਵੇਸ਼ -= mb_ਸਿਰ_off(rex.ਸਤਰ, rex.ਨਿਵੇਸ਼);
	    }
	    ਨਹੀਂਤਾਂ
		rex.ਨਿਵੇਸ਼ = rex.ਸਤਰ;
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
    ਜੇ (log_fd != stderr)
	fclose(log_fd);
    log_fd = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    // Have to clear the lastlist field of the NFA nodes, so that
    // nfa_regmatch() and addstate() can run properly after recursion.
    ਜੇ (nfa_ll_ਸੂਚਕਾਂਕ == 1)
    {
	// Already calling nfa_regmatch() recursively.  Save the lastlist[1]
	// values and clear them.
	ਜੇ (*listids == ਨਿਰਾ || *listids_len < prog->nstate)
	{
	    vim_free(*listids);
	    *listids = ALLOC_MULT(ਪੂਰਨ, prog->nstate);
	    ਜੇ (*listids == ਨਿਰਾ)
	    {
		emsg(_(e_nfa_regexp_could_not_allocate_ਸਮ੍ਰਿਤੀ_for_branch_traversal));
		ਵਾਪਸ 0;
	    }
	    *listids_len = prog->nstate;
	}
	nfa_save_listids(prog, *listids);
	need_restore = TRUE;
	// any value of rex.nfa_listid will do
    }
    ਨਹੀਂਤਾਂ
    {
	// First recursive nfa_regmatch() call, switch to the second lastlist
	// entry.  Make sure rex.nfa_listid is different from a previous
	// recursive call, because some states may still have this ID.
	++nfa_ll_ਸੂਚਕਾਂਕ;
	ਜੇ (rex.nfa_listid <= rex.nfa_alt_listid)
	    rex.nfa_listid = rex.nfa_alt_listid;
    }

    // Call nfa_regmatch() to check if the current concat matches at this
    // position. The concat ends with the node NFA_END_INVISIBLE
    nfa_endp = endposp;
    ਨਤੀਜਾ = nfa_regmatch(prog, state->ਬਾਹਰਲਾ, submatch, m);

    ਜੇ (need_restore)
	nfa_restore_listids(prog, *listids);
    ਨਹੀਂਤਾਂ
    {
	--nfa_ll_ਸੂਚਕਾਂਕ;
	rex.nfa_alt_listid = rex.nfa_listid;
    }

    // restore position in input text
    rex.lnum = save_reglnum;
    ਜੇ (REG_MULTI)
	rex.ਸਤਰ = reg_getline(rex.lnum);
    rex.ਨਿਵੇਸ਼ = rex.ਸਤਰ + save_reginput_ਖਾਨਾ;
    ਜੇ (ਨਤੀਜਾ != NFA_TOO_EXPENSIVE)
    {
	nfa_ਮੇਲ = save_nfa_ਮੇਲ;
	rex.nfa_listid = save_nfa_listid;
    }
    nfa_endp = save_nfa_endp;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
    open_debug_log(ਨਤੀਜਾ);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Estimate the chance of a match with "state" failing.
 * empty match: 0
 * NFA_ANY: 1
 * specific character: 99
 */
    ਸਥਿਰ ਪੂਰਨ
failure_chance(nfa_state_T *state, ਪੂਰਨ ਡੂੰਘਾਈ)
{
    ਪੂਰਨ c = state->c;
    ਪੂਰਨ l, r;

    // detect looping
    ਜੇ (ਡੂੰਘਾਈ > 4)
	ਵਾਪਸ 1;

    ਚੋਣ (c)
    {
	ਹਾਲਤ NFA_ਵੰਡੋ:
	    ਜੇ (state->ਬਾਹਰਲਾ->c == NFA_ਵੰਡੋ || state->out1->c == NFA_ਵੰਡੋ)
		// avoid recursive stuff
		ਵਾਪਸ 1;
	    // two alternatives, use the lowest failure chance
	    l = failure_chance(state->ਬਾਹਰਲਾ, ਡੂੰਘਾਈ + 1);
	    r = failure_chance(state->out1, ਡੂੰਘਾਈ + 1);
	    ਵਾਪਸ l < r ? l : r;

	ਹਾਲਤ NFA_ANY:
	    // matches anything, unlikely to fail
	    ਵਾਪਸ 1;

	ਹਾਲਤ NFA_ਮੇਲ:
	ਹਾਲਤ NFA_MCLOSE:
	ਹਾਲਤ NFA_ANY_COMPOSING:
	    // empty match works always
	    ਵਾਪਸ 0;

	ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE:
	ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_ਪਹਿਲਾ:
	ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_NEG:
	ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_NEG_ਪਹਿਲਾ:
	ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE:
	ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_ਪਹਿਲਾ:
	ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_NEG:
	ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_NEG_ਪਹਿਲਾ:
	ਹਾਲਤ NFA_ਸ਼ੁਰੂ_ਨਮੂਨਾ:
	    // recursive regmatch is expensive, use low failure chance
	    ਵਾਪਸ 5;

	ਹਾਲਤ NFA_BOL:
	ਹਾਲਤ NFA_EOL:
	ਹਾਲਤ NFA_BOF:
	ਹਾਲਤ NFA_EOF:
	ਹਾਲਤ NFA_NEWL:
	    ਵਾਪਸ 99;

	ਹਾਲਤ NFA_BOW:
	ਹਾਲਤ NFA_EOW:
	    ਵਾਪਸ 90;

	ਹਾਲਤ NFA_MOPEN:
	ਹਾਲਤ NFA_MOPEN1:
	ਹਾਲਤ NFA_MOPEN2:
	ਹਾਲਤ NFA_MOPEN3:
	ਹਾਲਤ NFA_MOPEN4:
	ਹਾਲਤ NFA_MOPEN5:
	ਹਾਲਤ NFA_MOPEN6:
	ਹਾਲਤ NFA_MOPEN7:
	ਹਾਲਤ NFA_MOPEN8:
	ਹਾਲਤ NFA_MOPEN9:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	ਹਾਲਤ NFA_ZOPEN:
	ਹਾਲਤ NFA_ZOPEN1:
	ਹਾਲਤ NFA_ZOPEN2:
	ਹਾਲਤ NFA_ZOPEN3:
	ਹਾਲਤ NFA_ZOPEN4:
	ਹਾਲਤ NFA_ZOPEN5:
	ਹਾਲਤ NFA_ZOPEN6:
	ਹਾਲਤ NFA_ZOPEN7:
	ਹਾਲਤ NFA_ZOPEN8:
	ਹਾਲਤ NFA_ZOPEN9:
	ਹਾਲਤ NFA_ZCLOSE:
	ਹਾਲਤ NFA_ZCLOSE1:
	ਹਾਲਤ NFA_ZCLOSE2:
	ਹਾਲਤ NFA_ZCLOSE3:
	ਹਾਲਤ NFA_ZCLOSE4:
	ਹਾਲਤ NFA_ZCLOSE5:
	ਹਾਲਤ NFA_ZCLOSE6:
	ਹਾਲਤ NFA_ZCLOSE7:
	ਹਾਲਤ NFA_ZCLOSE8:
	ਹਾਲਤ NFA_ZCLOSE9:
#ਜੇ_ਅੰਤ
	ਹਾਲਤ NFA_NOPEN:
	ਹਾਲਤ NFA_MCLOSE1:
	ਹਾਲਤ NFA_MCLOSE2:
	ਹਾਲਤ NFA_MCLOSE3:
	ਹਾਲਤ NFA_MCLOSE4:
	ਹਾਲਤ NFA_MCLOSE5:
	ਹਾਲਤ NFA_MCLOSE6:
	ਹਾਲਤ NFA_MCLOSE7:
	ਹਾਲਤ NFA_MCLOSE8:
	ਹਾਲਤ NFA_MCLOSE9:
	ਹਾਲਤ NFA_NCLOSE:
	    ਵਾਪਸ failure_chance(state->ਬਾਹਰਲਾ, ਡੂੰਘਾਈ + 1);

	ਹਾਲਤ NFA_BACKREF1:
	ਹਾਲਤ NFA_BACKREF2:
	ਹਾਲਤ NFA_BACKREF3:
	ਹਾਲਤ NFA_BACKREF4:
	ਹਾਲਤ NFA_BACKREF5:
	ਹਾਲਤ NFA_BACKREF6:
	ਹਾਲਤ NFA_BACKREF7:
	ਹਾਲਤ NFA_BACKREF8:
	ਹਾਲਤ NFA_BACKREF9:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	ਹਾਲਤ NFA_ZREF1:
	ਹਾਲਤ NFA_ZREF2:
	ਹਾਲਤ NFA_ZREF3:
	ਹਾਲਤ NFA_ZREF4:
	ਹਾਲਤ NFA_ZREF5:
	ਹਾਲਤ NFA_ZREF6:
	ਹਾਲਤ NFA_ZREF7:
	ਹਾਲਤ NFA_ZREF8:
	ਹਾਲਤ NFA_ZREF9:
#ਜੇ_ਅੰਤ
	    // backreferences don't match in many places
	    ਵਾਪਸ 94;

	ਹਾਲਤ NFA_LNUM_GT:
	ਹਾਲਤ NFA_LNUM_LT:
	ਹਾਲਤ NFA_ਖਾਨਾ_GT:
	ਹਾਲਤ NFA_ਖਾਨਾ_LT:
	ਹਾਲਤ NFA_VCOL_GT:
	ਹਾਲਤ NFA_VCOL_LT:
	ਹਾਲਤ NFA_MARK_GT:
	ਹਾਲਤ NFA_MARK_LT:
	ਹਾਲਤ NFA_VISUAL:
	    // before/after positions don't match very often
	    ਵਾਪਸ 85;

	ਹਾਲਤ NFA_LNUM:
	    ਵਾਪਸ 90;

	ਹਾਲਤ NFA_CURSOR:
	ਹਾਲਤ NFA_ਖਾਨਾ:
	ਹਾਲਤ NFA_VCOL:
	ਹਾਲਤ NFA_MARK:
	    // specific positions rarely match
	    ਵਾਪਸ 98;

	ਹਾਲਤ NFA_COMPOSING:
	    ਵਾਪਸ 95;

	ਮੂਲ:
	    ਜੇ (c > 0)
		// character match fails often
		ਵਾਪਸ 95;
    }

    // something else, includes character classes
    ਵਾਪਸ 50;
}

/*
 * Skip until the char "c" we know a match must start with.
 */
    ਸਥਿਰ ਪੂਰਨ
skip_ਮੰਜ਼ਿਲ_ਸ਼ੁਰੂ(ਪੂਰਨ c, colnr_T *colp)
{
    char_u *s;

    // Used often, do some work to avoid call overhead.
    ਜੇ (!rex.reg_ic && !has_mbyte)
	s = vim_strbyte(rex.ਸਤਰ + *colp, c);
    ਨਹੀਂਤਾਂ
	s = cstrchr(rex.ਸਤਰ + *colp, c);
    ਜੇ (s == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    *colp = (ਪੂਰਨ)(s - rex.ਸਤਰ);
    ਵਾਪਸ OK;
}

/*
 * Check for a match with match_text.
 * Called after skip_to_start() has found regstart.
 * Returns zero for no match, 1 for a match.
 */
    ਸਥਿਰ ਲੰਮਾ
ਲੱਭੋ_ਮੇਲ_ਪਾਠ(colnr_T *startcol, ਪੂਰਨ regstart, char_u *ਮੇਲ_ਪਾਠ)
{
    colnr_T ਖਾਨਾ = *startcol;
    ਪੂਰਨ	    c1, c2;
    ਪੂਰਨ	    len1, len2;
    ਪੂਰਨ	    ਮੇਲ;

    ਲਈ (;;)
    {
	ਮੇਲ = TRUE;
	// Skip the input character: case-folding can change its byte length.
	len2 = MB_CPTR2LEN(rex.ਸਤਰ + ਖਾਨਾ);
	ਲਈ (len1 = 0; ਮੇਲ_ਪਾਠ[len1] != NUL; len1 += MB_CHAR2LEN(c1))
	{
	    c1 = PTR2CHAR(ਮੇਲ_ਪਾਠ + len1);
	    c2 = PTR2CHAR(rex.ਸਤਰ + ਖਾਨਾ + len2);
	    ਜੇ (c1 != c2 && (!rex.reg_ic || MB_CASEFOLD(c1) != MB_CASEFOLD(c2)))
	    {
		ਮੇਲ = FALSE;
		ਰੋਕੋ;
	    }
	    len2 += enc_utf8 ? utf_ptr2len(rex.ਸਤਰ + ਖਾਨਾ + len2)
							     : MB_CHAR2LEN(c2);
	}
	ਜੇ (ਮੇਲ
		// check that no composing char follows
		&& !(enc_utf8
			  && utf_iscomposing(PTR2CHAR(rex.ਸਤਰ + ਖਾਨਾ + len2))))
	{
	    cleanup_subexpr();
	    ਜੇ (REG_MULTI)
	    {
		rex.reg_startpos[0].lnum = rex.lnum;
		rex.reg_startpos[0].ਖਾਨਾ = ਖਾਨਾ;
		rex.reg_endpos[0].lnum = rex.lnum;
		rex.reg_endpos[0].ਖਾਨਾ = ਖਾਨਾ + len2;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		rex.reg_startp[0] = rex.ਸਤਰ + ਖਾਨਾ;
		rex.reg_endp[0] = rex.ਸਤਰ + ਖਾਨਾ + len2;
	    }
	    *startcol = ਖਾਨਾ;
	    ਵਾਪਸ 1L;
	}

	// Try finding regstart after the current match.
	ਖਾਨਾ += MB_CPTR2LEN(rex.ਸਤਰ + ਖਾਨਾ); // skip the input character
	ਜੇ (skip_ਮੰਜ਼ਿਲ_ਸ਼ੁਰੂ(regstart, &ਖਾਨਾ) == FAIL)
	    ਰੋਕੋ;
    }

    *startcol = ਖਾਨਾ;
    ਵਾਪਸ 0L;
}

/*
 * Check whether the composing characters at the current input position match
 * the NFA composing sub-expression that starts at "sta" (the first state
 * after NFA_COMPOSING).  "curc" is the base character and "clen" its byte
 * length.
 * Returns OK when it matches, FAIL otherwise.
 */
    ਸਥਿਰ ਪੂਰਨ
ਮੇਲ_composing(nfa_state_T *sta, ਪੂਰਨ curc, ਪੂਰਨ clen)
{
    ਪੂਰਨ		mc = curc;
    ਪੂਰਨ		len = 0;
    ਪੂਰਨ		cchars[MAX_MCO];
    ਪੂਰਨ		ccount = 0;
    ਪੂਰਨ		j;

    ਜੇ (utf_iscomposing(sta->c))
	// Only match composing character(s), ignore base character.  Used
	// for ".{composing}" and "{composing}" (no preceding character).
	len += mb_char2len(mc);

    ਜੇ (rex.reg_icombine && len == 0)
	// If \Z was present, then ignore composing characters.  When
	// ignoring the base character this always matches.
	ਵਾਪਸ sta->c == curc ? OK : FAIL;

    // Check base character matches first, unless ignored.
    ਜੇ (len == 0 && mc != sta->c)
	ਵਾਪਸ FAIL;

    ਜੇ (len == 0)
    {
	len += mb_char2len(mc);
	sta = sta->ਬਾਹਰਲਾ;
    }

    // We don't care about the order of composing characters.  Get them into
    // cchars[] first.
    ਜਦੋਂ (len < clen)
    {
	mc = mb_ptr2char(rex.ਨਿਵੇਸ਼ + len);
	cchars[ccount++] = mc;
	len += mb_char2len(mc);
	ਜੇ (ccount == MAX_MCO)
	    ਰੋਕੋ;
    }

    // Check that each composing char in the pattern matches a composing char
    // in the text.  We do not check if all composing chars are matched.
    ਜਦੋਂ (sta->c != NFA_ਸਿਰਾ_COMPOSING)
    {
	ਲਈ (j = 0; j < ccount; ++j)
	    ਜੇ (cchars[j] == sta->c)
		ਰੋਕੋ;
	ਜੇ (j == ccount)
	    ਵਾਪਸ FAIL;
	sta = sta->ਬਾਹਰਲਾ;
    }

    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
nfa_regmatch(
    nfa_regprog_T	*prog,
    nfa_state_T		*ਸ਼ੁਰੂ,
    regsubs_T		*submatch,
    regsubs_T		*m)
{
    ਪੂਰਨ		ਨਤੀਜਾ = FALSE;
    size_t	ਮਾਪ = 0;
    ਪੂਰਨ		flag = 0;
    ਪੂਰਨ		go_ਮੰਜ਼ਿਲ_nextline = FALSE;
    nfa_thread_T *t;
    nfa_list_T	list[2];
    ਪੂਰਨ		listidx;
    nfa_list_T	*thislist;
    nfa_list_T	*nextlist;
    ਪੂਰਨ		*listids = ਨਿਰਾ;
    ਪੂਰਨ		listids_len = 0;
    nfa_state_T *ਜੋੜੋ_state;
    ਪੂਰਨ		ਜੋੜੋ_ਇੱਥੇ;
    ਪੂਰਨ		ਜੋੜੋ_ਗਿਣਤੀ;
    ਪੂਰਨ		ਜੋੜੋ_off = 0;
    ਪੂਰਨ		toplevel = ਸ਼ੁਰੂ->c == NFA_MOPEN;
    regsubs_T	*r;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NFA_REGEXP_DEBUG_LOG
    FILE	*debug;
#ਜੇ_ਅੰਤ

    // Some patterns may take a long time to match, especially when using
    // recursive_regmatch(). Allow interrupting them with CTRL-C.
    fast_breakcheck();
    ਜੇ (got_int)
	ਵਾਪਸ FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
    ਜੇ (nfa_did_ਸਮਾਂ_ਬਾਹਰਲਾ())
	ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NFA_REGEXP_DEBUG_LOG
    debug = fopen(NFA_REGEXP_DEBUG_LOG, "a");
    ਜੇ (debug == ਨਿਰਾ)
    {
	semsg("(NFA) COULD NOT OPEN %s!", NFA_REGEXP_DEBUG_LOG);
	ਵਾਪਸ FALSE;
    }
#ਜੇ_ਅੰਤ
    nfa_ਮੇਲ = FALSE;

    // Allocate memory for the lists of nodes.
    ਮਾਪ = (prog->nstate + 1) * ਆਕਾਰ(nfa_thread_T);

    // Reuse cached list buffers from prog when available (top-level call).
    // Recursive calls must allocate their own buffers.
    ਜੇ (toplevel && prog->listbuf[0] != ਨਿਰਾ)
    {
	list[0].t = (nfa_thread_T *)prog->listbuf[0];
	list[1].t = (nfa_thread_T *)prog->listbuf[1];
	prog->listbuf[0] = ਨਿਰਾ;
	prog->listbuf[1] = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ
    {
	list[0].t = alloc(ਮਾਪ);
	list[1].t = alloc(ਮਾਪ);
    }
    list[0].len = prog->nstate + 1;
    list[1].len = prog->nstate + 1;
    ਜੇ (list[0].t == ਨਿਰਾ || list[1].t == ਨਿਰਾ)
	ਜਾਓ theend;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
    log_fd = fopen(NFA_REGEXP_ਲੜੀਵਾਰ_LOG, "a");
    ਜੇ (log_fd == ਨਿਰਾ)
    {
	emsg(_(e_log_open_failed));
	log_fd = stderr;
    }
    fprintf(log_fd, "**********************************\n");
    nfa_ਸੈੱਟ_ਕੋਡ(ਸ਼ੁਰੂ->c);
    fprintf(log_fd, " RUNNING nfa_regmatch() starting with state %d, code %s\n",
    ਨਿਰਪੇਖ(ਸ਼ੁਰੂ->id), ਕੋਡ);
    fprintf(log_fd, "**********************************\n");
#ਜੇ_ਅੰਤ

    thislist = &list[0];
    thislist->n = 0;
    thislist->has_pim = FALSE;
    nextlist = &list[1];
    nextlist->n = 0;
    nextlist->has_pim = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
    fprintf(log_fd, "(---) STARTSTATE first\n");
#ਜੇ_ਅੰਤ
    thislist->id = rex.nfa_listid + 1;

    // Inline optimized code for addstate(thislist, start, m, 0) if we know
    // it's the first MOPEN.
    ਜੇ (toplevel)
    {
	ਜੇ (REG_MULTI)
	{
	    m->norm.list.multi[0].ਸ਼ੁਰੂ_lnum = rex.lnum;
	    m->norm.list.multi[0].ਸ਼ੁਰੂ_ਖਾਨਾ = (colnr_T)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ);
	    m->norm.orig_ਸ਼ੁਰੂ_ਖਾਨਾ = m->norm.list.multi[0].ਸ਼ੁਰੂ_ਖਾਨਾ;
	}
	ਨਹੀਂਤਾਂ
	    m->norm.list.ਸਤਰ[0].ਸ਼ੁਰੂ = rex.ਨਿਵੇਸ਼;
	m->norm.ਵਿੱਚ_use = 1;
	r = addstate(thislist, ਸ਼ੁਰੂ->ਬਾਹਰਲਾ, m, ਨਿਰਾ, 0, 0);
    }
    ਨਹੀਂਤਾਂ
	r = addstate(thislist, ਸ਼ੁਰੂ, m, ਨਿਰਾ, 0, 0);
    ਜੇ (r == ਨਿਰਾ)
    {
	nfa_ਮੇਲ = NFA_TOO_EXPENSIVE;
	ਜਾਓ theend;
    }

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	ਜੋੜੋ_STATE_IF_ਮੇਲ(state)	\
    ਜੇ (ਨਤੀਜਾ)				\
    {					\
	ਜੋੜੋ_state = state->ਬਾਹਰਲਾ;		\
	ਜੋੜੋ_off = clen;			\
    }

    /*
     * Run for each character.
     */
    ਲਈ (;;)
    {
	ਪੂਰਨ	curc;
	ਪੂਰਨ	clen;

	ਜੇ (has_mbyte)
	{
	    // Fast path for an ASCII byte not followed by a composing
	    // character, matching the check in utfc_ptr2len().  Avoids two
	    // indirect calls for the common case.
	    ਜੇ (rex.ਨਿਵੇਸ਼[0] != NUL && rex.ਨਿਵੇਸ਼[0] < 0x80 && rex.ਨਿਵੇਸ਼[1] < 0x80)
	    {
		curc = rex.ਨਿਵੇਸ਼[0];
		clen = 1;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		curc = (*mb_ptr2char)(rex.ਨਿਵੇਸ਼);
		clen = (*mb_ptr2len)(rex.ਨਿਵੇਸ਼);
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    curc = *rex.ਨਿਵੇਸ਼;
	    clen = 1;
	}
	ਜੇ (curc == NUL)
	{
	    clen = 0;
	    go_ਮੰਜ਼ਿਲ_nextline = FALSE;
	}

	// swap lists
	thislist = &list[flag];
	nextlist = &list[flag ^= 1];
	nextlist->n = 0;	    // clear nextlist
	nextlist->has_pim = FALSE;
	++rex.nfa_listid;
	ਜੇ (prog->re_engine == AUTOMATIC_ENGINE
		&& (rex.nfa_listid >= NFA_MAX_ਹਾਲਤਾਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		    || nfa_fail_for_testing
#ਜੇ_ਅੰਤ
		    ))
	{
	    // too many states, retry with old engine
	    nfa_ਮੇਲ = NFA_TOO_EXPENSIVE;
	    ਜਾਓ theend;
	}

	thislist->id = rex.nfa_listid;
	nextlist->id = rex.nfa_listid + 1;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
	fprintf(log_fd, "------------------------------------------\n");
	fprintf(log_fd, ">>> Reginput is \"%s\"\n", rex.ਨਿਵੇਸ਼);
	fprintf(log_fd, ">>> Advanced one character... Current char is %c (code %d) \n", curc, (ਪੂਰਨ)curc);
	fprintf(log_fd, ">>> Thislist has %d states available: ", thislist->n);
	{
	    ਪੂਰਨ i;

	    ਲਈ (i = 0; i < thislist->n; i++)
		fprintf(log_fd, "%d  ", ਨਿਰਪੇਖ(thislist->t[i].state->id));
	}
	fprintf(log_fd, "\n");
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NFA_REGEXP_DEBUG_LOG
	fprintf(debug, "\n-------------------\n");
#ਜੇ_ਅੰਤ
	/*
	 * If the state lists are empty we can stop.
	 */
	ਜੇ (thislist->n == 0)
	    ਰੋਕੋ;

	// compute nextlist
	ਲਈ (listidx = 0; listidx < thislist->n; ++listidx)
	{
	    // If the list gets very long there probably is something wrong.
	    // At least allow interrupting with CTRL-C.  Only check once in a
	    // while, calling ui_breakcheck() for every state is too slow.
	    ਸਥਿਰ ਪੂਰਨ	breakcheck_ਗਿਣਤੀ = 0;  // using "static" makes it faster
	    ਜੇ (unlikely(++breakcheck_ਗਿਣਤੀ >= 100000))
	    {
		ui_breakcheck();
		breakcheck_ਗਿਣਤੀ = 0;
	    }
	    ਜੇ (got_int)
		ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
	    ਜੇ (nfa_did_ਸਮਾਂ_ਬਾਹਰਲਾ())
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	    t = &thislist->t[listidx];

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NFA_REGEXP_DEBUG_LOG
	    nfa_ਸੈੱਟ_ਕੋਡ(t->state->c);
	    fprintf(debug, "%s, ", ਕੋਡ);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
	    {
		ਪੂਰਨ ਖਾਨਾ;

		ਜੇ (t->subs.norm.ਵਿੱਚ_use <= 0)
		    ਖਾਨਾ = -1;
		ਨਹੀਂਤਾਂ ਜੇ (REG_MULTI)
		    ਖਾਨਾ = t->subs.norm.list.multi[0].ਸ਼ੁਰੂ_ਖਾਨਾ;
		ਨਹੀਂਤਾਂ
		    ਖਾਨਾ = (ਪੂਰਨ)(t->subs.norm.list.ਸਤਰ[0].ਸ਼ੁਰੂ - rex.ਸਤਰ);
		nfa_ਸੈੱਟ_ਕੋਡ(t->state->c);
		fprintf(log_fd, "(%d) char %d %s (start col %d)%s... \n",
			ਨਿਰਪੇਖ(t->state->id), (ਪੂਰਨ)t->state->c, ਕੋਡ, ਖਾਨਾ,
			pim_info(&t->pim));
	    }
#ਜੇ_ਅੰਤ

	    /*
	     * Handle the possible codes of the current state.
	     * The most important is NFA_MATCH.
	     */
	    ਜੋੜੋ_state = ਨਿਰਾ;
	    ਜੋੜੋ_ਇੱਥੇ = FALSE;
	    ਜੋੜੋ_ਗਿਣਤੀ = 0;
	    ਚੋਣ (t->state->c)
	    {
	    ਹਾਲਤ NFA_ਮੇਲ:
	      {
		// If the match is not at the start of the line, ends before a
		// composing characters and rex.reg_icombine is not set, that
		// is not really a match.
		ਜੇ (enc_utf8 && !rex.reg_icombine
			     && rex.ਨਿਵੇਸ਼ != rex.ਸਤਰ && utf_iscomposing(curc))
		    ਰੋਕੋ;

		nfa_ਮੇਲ = TRUE;
		copy_sub(&submatch->norm, &t->subs.norm);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
		ਜੇ (rex.nfa_has_zsubexpr)
		    copy_sub(&submatch->synt, &t->subs.synt);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
		log_subsexpr(&t->subs);
#ਜੇ_ਅੰਤ
		// Found the left-most longest match, do not look at any other
		// states at this position.  When the list of states is going
		// to be empty quit without advancing, so that "rex.input" is
		// correct.
		ਜੇ (nextlist->n == 0)
		    clen = 0;
		ਜਾਓ nextchar;
	      }

	    ਹਾਲਤ NFA_ਸਿਰਾ_INVISIBLE:
	    ਹਾਲਤ NFA_ਸਿਰਾ_INVISIBLE_NEG:
	    ਹਾਲਤ NFA_ਸਿਰਾ_ਨਮੂਨਾ:
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
		ਜੇ (nfa_endp != ਨਿਰਾ)
		{
		    ਜੇ (REG_MULTI)
			fprintf(log_fd, "Current lnum: %d, endp lnum: %d; current col: %d, endp col: %d\n",
				(ਪੂਰਨ)rex.lnum,
				(ਪੂਰਨ)nfa_endp->se_u.pos.lnum,
				(ਪੂਰਨ)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ),
				nfa_endp->se_u.pos.ਖਾਨਾ);
		    ਨਹੀਂਤਾਂ
			fprintf(log_fd, "Current col: %d, endp col: %d\n",
				(ਪੂਰਨ)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ),
				(ਪੂਰਨ)(nfa_endp->se_u.ptr - rex.ਨਿਵੇਸ਼));
		}
#ਜੇ_ਅੰਤ
		// If "nfa_endp" is set it's only a match if it ends at
		// "nfa_endp"
		ਜੇ (nfa_endp != ਨਿਰਾ && (REG_MULTI
			? (rex.lnum != nfa_endp->se_u.pos.lnum
			    || (ਪੂਰਨ)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ)
						!= nfa_endp->se_u.pos.ਖਾਨਾ)
			: rex.ਨਿਵੇਸ਼ != nfa_endp->se_u.ptr))
		    ਰੋਕੋ;

		// do not set submatches for \@!
		ਜੇ (t->state->c != NFA_ਸਿਰਾ_INVISIBLE_NEG)
		{
		    copy_sub(&m->norm, &t->subs.norm);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
		    ਜੇ (rex.nfa_has_zsubexpr)
			copy_sub(&m->synt, &t->subs.synt);
#ਜੇ_ਅੰਤ
		}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
		fprintf(log_fd, "Match found:\n");
		log_subsexpr(m);
#ਜੇ_ਅੰਤ
		nfa_ਮੇਲ = TRUE;
		// See comment above at "goto nextchar".
		ਜੇ (nextlist->n == 0)
		    clen = 0;
		ਜਾਓ nextchar;

	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE:
	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_ਪਹਿਲਾ:
	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_NEG:
	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_NEG_ਪਹਿਲਾ:
	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE:
	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_ਪਹਿਲਾ:
	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_NEG:
	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_NEG_ਪਹਿਲਾ:
		{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
		    fprintf(log_fd, "Failure chance invisible: %d, what follows: %d\n",
			    failure_chance(t->state->ਬਾਹਰਲਾ, 0),
			    failure_chance(t->state->out1->ਬਾਹਰਲਾ, 0));
#ਜੇ_ਅੰਤ
		    // Do it directly if there already is a PIM or when
		    // nfa_postprocess() detected it will work better.
		    ਜੇ (t->pim.ਨਤੀਜਾ != NFA_PIM_ਅਣਵਰਤਿਆ
			 || t->state->c == NFA_ਸ਼ੁਰੂ_INVISIBLE_ਪਹਿਲਾ
			 || t->state->c == NFA_ਸ਼ੁਰੂ_INVISIBLE_NEG_ਪਹਿਲਾ
			 || t->state->c == NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_ਪਹਿਲਾ
			 || t->state->c == NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_NEG_ਪਹਿਲਾ)
		    {
			ਪੂਰਨ ਵਿੱਚ_use = m->norm.ਵਿੱਚ_use;

			// Copy submatch info for the recursive call, opposite
			// of what happens on success below.
			copy_sub_off(&m->norm, &t->subs.norm);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
			ਜੇ (rex.nfa_has_zsubexpr)
			    copy_sub_off(&m->synt, &t->subs.synt);
#ਜੇ_ਅੰਤ

			/*
			 * First try matching the invisible match, then what
			 * follows.
			 */
			ਨਤੀਜਾ = recursive_regmatch(t->state, ਨਿਰਾ, prog,
					  submatch, m, &listids, &listids_len);
			ਜੇ (ਨਤੀਜਾ == NFA_TOO_EXPENSIVE)
			{
			    nfa_ਮੇਲ = ਨਤੀਜਾ;
			    ਜਾਓ theend;
			}

			// for \@! and \@<! it is a match when the result is
			// FALSE
			ਜੇ (ਨਤੀਜਾ != (t->state->c == NFA_ਸ਼ੁਰੂ_INVISIBLE_NEG
			       || t->state->c == NFA_ਸ਼ੁਰੂ_INVISIBLE_NEG_ਪਹਿਲਾ
			       || t->state->c
					   == NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_NEG
			       || t->state->c
				     == NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_NEG_ਪਹਿਲਾ))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&t->subs.norm, &m->norm);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
			    ਜੇ (rex.nfa_has_zsubexpr)
				copy_sub_off(&t->subs.synt, &m->synt);
#ਜੇ_ਅੰਤ
			    // If the pattern has \ze and it matched in the
			    // sub pattern, use it.
			    copy_ze_off(&t->subs.norm, &m->norm);

			    // t->state->out1 is the corresponding
			    // END_INVISIBLE node; Add its out to the current
			    // list (zero-width match).
			    ਜੋੜੋ_ਇੱਥੇ = TRUE;
			    ਜੋੜੋ_state = t->state->out1->ਬਾਹਰਲਾ;
			}
			m->norm.ਵਿੱਚ_use = ਵਿੱਚ_use;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			nfa_pim_T pim;

			/*
			 * First try matching what follows.  Only if a match
			 * is found verify the invisible match matches.  Add a
			 * nfa_pim_T to the following states, it contains info
			 * about the invisible match.
			 */
			pim.state = t->state;
			pim.ਨਤੀਜਾ = NFA_PIM_TODO;
			pim.subs.norm.ਵਿੱਚ_use = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
			pim.subs.synt.ਵਿੱਚ_use = 0;
#ਜੇ_ਅੰਤ
			ਜੇ (REG_MULTI)
			{
			    pim.ਸਿਰਾ.pos.ਖਾਨਾ = (ਪੂਰਨ)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ);
			    pim.ਸਿਰਾ.pos.lnum = rex.lnum;
			}
			ਨਹੀਂਤਾਂ
			    pim.ਸਿਰਾ.ptr = rex.ਨਿਵੇਸ਼;

			// t->state->out1 is the corresponding END_INVISIBLE
			// node; Add its out to the current list (zero-width
			// match).
			ਜੇ (addstate_ਇੱਥੇ(thislist, t->state->out1->ਬਾਹਰਲਾ,
					     &t->subs, &pim, &listidx) == ਨਿਰਾ)
			{
			    nfa_ਮੇਲ = NFA_TOO_EXPENSIVE;
			    ਜਾਓ theend;
			}
		    }
		}
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_ਨਮੂਨਾ:
	      {
		nfa_state_T *skip = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
		ਪੂਰਨ	    skip_lid = 0;
#ਜੇ_ਅੰਤ

		// There is no point in trying to match the pattern if the
		// output state is not going to be added to the list.
		ਜੇ (state_ਵਿੱਚ_list(nextlist, t->state->out1->ਬਾਹਰਲਾ, &t->subs))
		{
		    skip = t->state->out1->ਬਾਹਰਲਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
		    skip_lid = nextlist->id;
#ਜੇ_ਅੰਤ
		}
		ਨਹੀਂਤਾਂ ਜੇ (state_ਵਿੱਚ_list(nextlist,
					  t->state->out1->ਬਾਹਰਲਾ->ਬਾਹਰਲਾ, &t->subs))
		{
		    skip = t->state->out1->ਬਾਹਰਲਾ->ਬਾਹਰਲਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
		    skip_lid = nextlist->id;
#ਜੇ_ਅੰਤ
		}
		ਨਹੀਂਤਾਂ ਜੇ (state_ਵਿੱਚ_list(thislist,
					  t->state->out1->ਬਾਹਰਲਾ->ਬਾਹਰਲਾ, &t->subs))
		{
		    skip = t->state->out1->ਬਾਹਰਲਾ->ਬਾਹਰਲਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
		    skip_lid = thislist->id;
#ਜੇ_ਅੰਤ
		}
		ਜੇ (skip != ਨਿਰਾ)
		{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
		    nfa_ਸੈੱਟ_ਕੋਡ(skip->c);
		    fprintf(log_fd, "> Not trying to match pattern, output state %d is already in list %d. char %d: %s\n",
			    ਨਿਰਪੇਖ(skip->id), skip_lid, skip->c, ਕੋਡ);
#ਜੇ_ਅੰਤ
		    ਰੋਕੋ;
		}
		// Copy submatch info to the recursive call, opposite of what
		// happens afterwards.
		copy_sub_off(&m->norm, &t->subs.norm);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
		ਜੇ (rex.nfa_has_zsubexpr)
		    copy_sub_off(&m->synt, &t->subs.synt);
#ਜੇ_ਅੰਤ

		// First try matching the pattern.
		ਨਤੀਜਾ = recursive_regmatch(t->state, ਨਿਰਾ, prog,
					  submatch, m, &listids, &listids_len);
		ਜੇ (ਨਤੀਜਾ == NFA_TOO_EXPENSIVE)
		{
		    nfa_ਮੇਲ = ਨਤੀਜਾ;
		    ਜਾਓ theend;
		}
		ਜੇ (ਨਤੀਜਾ)
		{
		    ਪੂਰਨ bytelen;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN matches:\n");
		    log_subsexpr(m);
#ਜੇ_ਅੰਤ
		    // Copy submatch info from the recursive call
		    copy_sub_off(&t->subs.norm, &m->norm);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
		    ਜੇ (rex.nfa_has_zsubexpr)
			copy_sub_off(&t->subs.synt, &m->synt);
#ਜੇ_ਅੰਤ
		    // Now we need to skip over the matched text and then
		    // continue with what follows.
		    ਜੇ (REG_MULTI)
			// TODO: multi-line match
			bytelen = m->norm.list.multi[0].ਸਿਰਾ_ਖਾਨਾ
						  - (ਪੂਰਨ)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ);
		    ਨਹੀਂਤਾਂ
			bytelen = (ਪੂਰਨ)(m->norm.list.ਸਤਰ[0].ਸਿਰਾ - rex.ਨਿਵੇਸ਼);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN length: %d\n", bytelen);
#ਜੇ_ਅੰਤ
		    ਜੇ (bytelen == 0)
		    {
			// empty match, output of corresponding
			// NFA_END_PATTERN/NFA_SKIP to be used at current
			// position
			ਜੋੜੋ_ਇੱਥੇ = TRUE;
			ਜੋੜੋ_state = t->state->out1->ਬਾਹਰਲਾ->ਬਾਹਰਲਾ;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (bytelen <= clen)
		    {
			// match current character, output of corresponding
			// NFA_END_PATTERN to be used at next position.
			ਜੋੜੋ_state = t->state->out1->ਬਾਹਰਲਾ->ਬਾਹਰਲਾ;
			ਜੋੜੋ_off = clen;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			ਜੋੜੋ_state = t->state->out1->ਬਾਹਰਲਾ;
			ਜੋੜੋ_off = bytelen;
			ਜੋੜੋ_ਗਿਣਤੀ = bytelen - clen;
		    }
		}
		ਰੋਕੋ;
	      }

	    ਹਾਲਤ NFA_BOL:
		ਜੇ (rex.ਨਿਵੇਸ਼ == rex.ਸਤਰ)
		{
		    ਜੋੜੋ_ਇੱਥੇ = TRUE;
		    ਜੋੜੋ_state = t->state->ਬਾਹਰਲਾ;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_EOL:
		ਜੇ (curc == NUL)
		{
		    ਜੋੜੋ_ਇੱਥੇ = TRUE;
		    ਜੋੜੋ_state = t->state->ਬਾਹਰਲਾ;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_BOW:
		ਨਤੀਜਾ = TRUE;

		ਜੇ (curc == NUL)
		    ਨਤੀਜਾ = FALSE;
		ਨਹੀਂਤਾਂ ਜੇ (has_mbyte)
		{
		    ਪੂਰਨ this_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.ਨਿਵੇਸ਼, rex.reg_buf);
		    ਜੇ (this_class <= 1)
			ਨਤੀਜਾ = FALSE;
		    ਨਹੀਂਤਾਂ ਜੇ (reg_prev_class() == this_class)
			ਨਤੀਜਾ = FALSE;
		}
		ਨਹੀਂਤਾਂ ਜੇ (!vim_iswordc_buf(curc, rex.reg_buf)
			   || (rex.ਨਿਵੇਸ਼ > rex.ਸਤਰ
			       && vim_iswordc_buf(rex.ਨਿਵੇਸ਼[-1], rex.reg_buf)))
		    ਨਤੀਜਾ = FALSE;
		ਜੇ (ਨਤੀਜਾ)
		{
		    ਜੋੜੋ_ਇੱਥੇ = TRUE;
		    ਜੋੜੋ_state = t->state->ਬਾਹਰਲਾ;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_EOW:
		ਨਤੀਜਾ = TRUE;
		ਜੇ (rex.ਨਿਵੇਸ਼ == rex.ਸਤਰ)
		    ਨਤੀਜਾ = FALSE;
		ਨਹੀਂਤਾਂ ਜੇ (has_mbyte)
		{
		    ਪੂਰਨ this_class, prev_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.ਨਿਵੇਸ਼, rex.reg_buf);
		    prev_class = reg_prev_class();
		    ਜੇ (this_class == prev_class
					|| prev_class == 0 || prev_class == 1)
			ਨਤੀਜਾ = FALSE;
		}
		ਨਹੀਂਤਾਂ ਜੇ (!vim_iswordc_buf(rex.ਨਿਵੇਸ਼[-1], rex.reg_buf)
			|| (rex.ਨਿਵੇਸ਼[0] != NUL
					&& vim_iswordc_buf(curc, rex.reg_buf)))
		    ਨਤੀਜਾ = FALSE;
		ਜੇ (ਨਤੀਜਾ)
		{
		    ਜੋੜੋ_ਇੱਥੇ = TRUE;
		    ਜੋੜੋ_state = t->state->ਬਾਹਰਲਾ;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_BOF:
		ਜੇ (rex.lnum == 0 && rex.ਨਿਵੇਸ਼ == rex.ਸਤਰ
				     && (!REG_MULTI || rex.reg_firstlnum == 1))
		{
		    ਜੋੜੋ_ਇੱਥੇ = TRUE;
		    ਜੋੜੋ_state = t->state->ਬਾਹਰਲਾ;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_EOF:
		ਜੇ (rex.lnum == rex.reg_maxline && curc == NUL)
		{
		    ਜੋੜੋ_ਇੱਥੇ = TRUE;
		    ਜੋੜੋ_state = t->state->ਬਾਹਰਲਾ;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_COMPOSING:
	    {
		nfa_state_T *ਸਿਰਾ;

		ਨਤੀਜਾ = ਮੇਲ_composing(t->state->ਬਾਹਰਲਾ, curc, clen);

		ਸਿਰਾ = t->state->out1;	    // NFA_END_COMPOSING
		ਜੋੜੋ_STATE_IF_ਮੇਲ(ਸਿਰਾ);
		ਰੋਕੋ;
	    }

	    ਹਾਲਤ NFA_NEWL:
		ਜੇ (curc == NUL && !rex.reg_ਸਤਰ_lbr && REG_MULTI
						&& rex.lnum <= rex.reg_maxline)
		{
		    go_ਮੰਜ਼ਿਲ_nextline = TRUE;
		    // Pass -1 for the offset, which means taking the position
		    // at the start of the next line.
		    ਜੋੜੋ_state = t->state->ਬਾਹਰਲਾ;
		    ਜੋੜੋ_off = -1;
		}
		ਨਹੀਂਤਾਂ ਜੇ (curc == '\n' && rex.reg_ਸਤਰ_lbr)
		{
		    // match \n as if it is an ordinary character
		    ਜੋੜੋ_state = t->state->ਬਾਹਰਲਾ;
		    ਜੋੜੋ_off = 1;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_COLL:
	    ਹਾਲਤ NFA_ਸ਼ੁਰੂ_NEG_COLL:
	      {
		// What follows is a list of characters, until NFA_END_COLL.
		// One of them must match or none of them must match.
		nfa_state_T	*state;
		ਪੂਰਨ		ਨਤੀਜਾ_if_matched;
		ਪੂਰਨ		c1, c2;

		// Never match EOL. If it's part of the collection it is added
		// as a separate state with an OR.
		ਜੇ (curc == NUL)
		    ਰੋਕੋ;

		state = t->state->ਬਾਹਰਲਾ;
		ਨਤੀਜਾ_if_matched = (t->state->c == NFA_ਸ਼ੁਰੂ_COLL);
		ਲਈ (;;)
		{
		    ਜੇ (state->c == NFA_COMPOSING)
		    {
			nfa_state_T *ਸਿਰਾ;

			ਨਤੀਜਾ = ਮੇਲ_composing(t->state->ਬਾਹਰਲਾ->ਬਾਹਰਲਾ, curc,
									clen);

			ਜੇ (t->state->ਬਾਹਰਲਾ->out1 != ਨਿਰਾ
				&& t->state->ਬਾਹਰਲਾ->out1->c == NFA_ਸਿਰਾ_COMPOSING)
			{
			    ਸਿਰਾ = t->state->ਬਾਹਰਲਾ->out1;
			    ਜੋੜੋ_STATE_IF_ਮੇਲ(ਸਿਰਾ);
			}
			ਰੋਕੋ;
		    }
		    ਜੇ (state->c == NFA_ਸਿਰਾ_COLL)
		    {
			ਨਤੀਜਾ = !ਨਤੀਜਾ_if_matched;
			ਰੋਕੋ;
		    }
		    ਜੇ (state->c == NFA_RANGE_MIN)
		    {
			c1 = state->val;
			state = state->ਬਾਹਰਲਾ; // advance to NFA_RANGE_MAX
			c2 = state->val;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
			fprintf(log_fd, "NFA_RANGE_MIN curc=%d c1=%d c2=%d\n",
				curc, c1, c2);
#ਜੇ_ਅੰਤ
			ਜੇ (curc >= c1 && curc <= c2)
			{
			    ਨਤੀਜਾ = ਨਤੀਜਾ_if_matched;
			    ਰੋਕੋ;
			}
			ਜੇ (rex.reg_ic)
			{
			    ਪੂਰਨ curc_ਨੀਵਾਂ = MB_CASEFOLD(curc);
			    ਪੂਰਨ ਪੂਰਾ = FALSE;

			    ਲਈ ( ; c1 <= c2; ++c1)
				ਜੇ (MB_CASEFOLD(c1) == curc_ਨੀਵਾਂ)
				{
				    ਨਤੀਜਾ = ਨਤੀਜਾ_if_matched;
				    ਪੂਰਾ = TRUE;
				    ਰੋਕੋ;
				}
			    ਜੇ (ਪੂਰਾ)
				ਰੋਕੋ;
			}
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (state->c < 0 ? check_char_class(state->c, curc)
			       : (curc == state->c
				   || (rex.reg_ic && MB_CASEFOLD(curc)
						    == MB_CASEFOLD(state->c))))
		    {
			ਨਤੀਜਾ = ਨਤੀਜਾ_if_matched;
			ਰੋਕੋ;
		    }
		    state = state->ਬਾਹਰਲਾ;
		}
		ਜੇ (ਨਤੀਜਾ)
		{
		    // next state is in out of the NFA_END_COLL, out1 of
		    // START points to the END state
		    ਜੋੜੋ_state = t->state->out1->ਬਾਹਰਲਾ;
		    ਜੋੜੋ_off = clen;
		}
		ਰੋਕੋ;
	      }

	    ਹਾਲਤ NFA_ANY:
		// Any char except '\0', (end of input) does not match.
		ਜੇ (curc > 0)
		{
		    ਜੋੜੋ_state = t->state->ਬਾਹਰਲਾ;
		    ਜੋੜੋ_off = clen;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_ANY_COMPOSING:
		// On a composing character skip over it.  Otherwise do
		// nothing.  Always matches.
		ਜੇ (enc_utf8 && utf_iscomposing(curc))
		{
		    ਜੋੜੋ_off = clen;
		}
		ਨਹੀਂਤਾਂ
		{
		    ਜੋੜੋ_ਇੱਥੇ = TRUE;
		    ਜੋੜੋ_off = 0;
		}
		ਜੋੜੋ_state = t->state->ਬਾਹਰਲਾ;
		ਰੋਕੋ;

	    /*
	     * Character classes like \a for alpha, \d for digit etc.
	     */
	    ਹਾਲਤ NFA_IDENT:	//  \i
		ਨਤੀਜਾ = vim_isIDc(curc);
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_SIDENT:	//  \I
		ਨਤੀਜਾ = !VIM_ISDIGIT(curc) && vim_isIDc(curc);
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_KWORD:	//  \k
		ਨਤੀਜਾ = vim_iswordp_buf(rex.ਨਿਵੇਸ਼, rex.reg_buf);
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_SKWORD:	//  \K
		ਨਤੀਜਾ = !VIM_ISDIGIT(curc)
				     && vim_iswordp_buf(rex.ਨਿਵੇਸ਼, rex.reg_buf);
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_FNAME:	//  \f
		ਨਤੀਜਾ = vim_isfilec(curc);
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_SFNAME:	//  \F
		ਨਤੀਜਾ = !VIM_ISDIGIT(curc) && vim_isfilec(curc);
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_PRINT:	//  \p
		ਨਤੀਜਾ = vim_isprintc(PTR2CHAR(rex.ਨਿਵੇਸ਼));
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_SPRINT:	//  \P
		ਨਤੀਜਾ = !VIM_ISDIGIT(curc) && vim_isprintc(PTR2CHAR(rex.ਨਿਵੇਸ਼));
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_WHITE:	//  \s
		ਨਤੀਜਾ = VIM_ISWHITE(curc);
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_NWHITE:	//  \S
		ਨਤੀਜਾ = curc != NUL && !VIM_ISWHITE(curc);
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_DIGIT:	//  \d
		ਨਤੀਜਾ = ri_digit(curc);
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_NDIGIT:	//  \D
		ਨਤੀਜਾ = curc != NUL && !ri_digit(curc);
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_HEX:	//  \x
		ਨਤੀਜਾ = ri_hex(curc);
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_NHEX:	//  \X
		ਨਤੀਜਾ = curc != NUL && !ri_hex(curc);
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_OCTAL:	//  \o
		ਨਤੀਜਾ = ri_octal(curc);
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_NOCTAL:	//  \O
		ਨਤੀਜਾ = curc != NUL && !ri_octal(curc);
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_ਸ਼ਬਦ:	//  \w
		ਨਤੀਜਾ = ri_ਸ਼ਬਦ(curc);
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_NWORD:	//  \W
		ਨਤੀਜਾ = curc != NUL && !ri_ਸ਼ਬਦ(curc);
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_ਸਿਰ:	//  \h
		ਨਤੀਜਾ = ri_ਸਿਰ(curc);
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_NHEAD:	//  \H
		ਨਤੀਜਾ = curc != NUL && !ri_ਸਿਰ(curc);
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_ALPHA:	//  \a
		ਨਤੀਜਾ = ri_alpha(curc);
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_NALPHA:	//  \A
		ਨਤੀਜਾ = curc != NUL && !ri_alpha(curc);
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_ਹੇਠਲਾ:	//  \l
		ਨਤੀਜਾ = ri_ਹੇਠਲਾ(curc);
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_NLOWER:	//  \L
		ਨਤੀਜਾ = curc != NUL && !ri_ਹੇਠਲਾ(curc);
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_ਉੱਤਲਾ:	//  \u
		ਨਤੀਜਾ = ri_ਉੱਤਲਾ(curc);
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_NUPPER:	// \U
		ਨਤੀਜਾ = curc != NUL && !ri_ਉੱਤਲਾ(curc);
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_ਹੇਠਲਾ_IC:	// [a-z]
		ਨਤੀਜਾ = ri_ਹੇਠਲਾ(curc) || (rex.reg_ic && ri_ਉੱਤਲਾ(curc));
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_NLOWER_IC:	// [^a-z]
		ਨਤੀਜਾ = curc != NUL
			&& !(ri_ਹੇਠਲਾ(curc) || (rex.reg_ic && ri_ਉੱਤਲਾ(curc)));
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_ਉੱਤਲਾ_IC:	// [A-Z]
		ਨਤੀਜਾ = ri_ਉੱਤਲਾ(curc) || (rex.reg_ic && ri_ਹੇਠਲਾ(curc));
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_NUPPER_IC:	// ^[A-Z]
		ਨਤੀਜਾ = curc != NUL
			&& !(ri_ਉੱਤਲਾ(curc) || (rex.reg_ic && ri_ਹੇਠਲਾ(curc)));
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_BACKREF1:
	    ਹਾਲਤ NFA_BACKREF2:
	    ਹਾਲਤ NFA_BACKREF3:
	    ਹਾਲਤ NFA_BACKREF4:
	    ਹਾਲਤ NFA_BACKREF5:
	    ਹਾਲਤ NFA_BACKREF6:
	    ਹਾਲਤ NFA_BACKREF7:
	    ਹਾਲਤ NFA_BACKREF8:
	    ਹਾਲਤ NFA_BACKREF9:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	    ਹਾਲਤ NFA_ZREF1:
	    ਹਾਲਤ NFA_ZREF2:
	    ਹਾਲਤ NFA_ZREF3:
	    ਹਾਲਤ NFA_ZREF4:
	    ਹਾਲਤ NFA_ZREF5:
	    ਹਾਲਤ NFA_ZREF6:
	    ਹਾਲਤ NFA_ZREF7:
	    ਹਾਲਤ NFA_ZREF8:
	    ਹਾਲਤ NFA_ZREF9:
#ਜੇ_ਅੰਤ
		// \1 .. \9  \z1 .. \z9
	      {
		ਪੂਰਨ subidx;
		ਪੂਰਨ bytelen;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
		ਜੇ (t->state->c >= NFA_BACKREF1 && t->state->c <= NFA_BACKREF9)
#ਜੇ_ਅੰਤ
		{
		    subidx = t->state->c - NFA_BACKREF1 + 1;
		    ਨਤੀਜਾ = ਮੇਲ_backref(&t->subs.norm, subidx, &bytelen);
		}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
		ਨਹੀਂਤਾਂ
		{
		    subidx = t->state->c - NFA_ZREF1 + 1;
		    ਨਤੀਜਾ = ਮੇਲ_zref(subidx, &bytelen);
		}
#ਜੇ_ਅੰਤ

		ਜੇ (ਨਤੀਜਾ)
		{
		    ਜੇ (bytelen == 0)
		    {
			// empty match always works, output of NFA_SKIP to be
			// used next
			ਜੋੜੋ_ਇੱਥੇ = TRUE;
			ਜੋੜੋ_state = t->state->ਬਾਹਰਲਾ->ਬਾਹਰਲਾ;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (bytelen <= clen)
		    {
			// match current character, jump ahead to out of
			// NFA_SKIP
			ਜੋੜੋ_state = t->state->ਬਾਹਰਲਾ->ਬਾਹਰਲਾ;
			ਜੋੜੋ_off = clen;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			ਜੋੜੋ_state = t->state->ਬਾਹਰਲਾ;
			ਜੋੜੋ_off = bytelen;
			ਜੋੜੋ_ਗਿਣਤੀ = bytelen - clen;
		    }
		}
		ਰੋਕੋ;
	      }
	    ਹਾਲਤ NFA_SKIP:
	      // character of previous matching \1 .. \9  or \@>
	      ਜੇ (t->ਗਿਣਤੀ_2 - clen <= 0)
	      {
		  // end of match, go to what follows
		  ਜੋੜੋ_state = t->state->ਬਾਹਰਲਾ;
		  ਜੋੜੋ_off = clen;
	      }
	      ਨਹੀਂਤਾਂ
	      {
		  // add state again with decremented count
		  ਜੋੜੋ_state = t->state;
		  ਜੋੜੋ_off = 0;
		  ਜੋੜੋ_ਗਿਣਤੀ = t->ਗਿਣਤੀ_2 - clen;
	      }
	      ਰੋਕੋ;

	    ਹਾਲਤ NFA_LNUM:
	    ਹਾਲਤ NFA_LNUM_GT:
	    ਹਾਲਤ NFA_LNUM_LT:
		ਨਤੀਜਾ = (REG_MULTI &&
			nfa_re_ਅੰਕ_cmp(t->state->val, t->state->c - NFA_LNUM,
			    (long_u)(rex.lnum + rex.reg_firstlnum)));
		ਜੇ (ਨਤੀਜਾ)
		{
		    ਜੋੜੋ_ਇੱਥੇ = TRUE;
		    ਜੋੜੋ_state = t->state->ਬਾਹਰਲਾ;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_ਖਾਨਾ:
	    ਹਾਲਤ NFA_ਖਾਨਾ_GT:
	    ਹਾਲਤ NFA_ਖਾਨਾ_LT:
		ਨਤੀਜਾ = nfa_re_ਅੰਕ_cmp(t->state->val, t->state->c - NFA_ਖਾਨਾ,
			(long_u)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ) + 1);
		ਜੇ (ਨਤੀਜਾ)
		{
		    ਜੋੜੋ_ਇੱਥੇ = TRUE;
		    ਜੋੜੋ_state = t->state->ਬਾਹਰਲਾ;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_VCOL:
	    ਹਾਲਤ NFA_VCOL_GT:
	    ਹਾਲਤ NFA_VCOL_LT:
		{
		    ਪੂਰਨ     op = t->state->c - NFA_VCOL;
		    colnr_T ਖਾਨਾ = (colnr_T)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ);
		    win_T   *wp = rex.reg_win == ਨਿਰਾ ? curwin : rex.reg_win;

		    // Bail out quickly when there can't be a match, avoid the
		    // overhead of win_linetabsize() on long lines.
		    ਜੇ (op != 1 && ਖਾਨਾ > t->state->val
			    * (has_mbyte ? MB_MAXBYTES : 1))
			ਰੋਕੋ;
		    ਨਤੀਜਾ = FALSE;
		    ਜੇ (op == 1 && ਖਾਨਾ - 1 > t->state->val && ਖਾਨਾ > 100)
		    {
			ਪੂਰਨ ts = wp->w_ਬਫਰ->b_p_ts;

			// Guess that a character won't use more columns than
			// 'tabstop', with a minimum of 4.
			ਜੇ (ts < 4)
			    ts = 4;
			ਨਤੀਜਾ = ਖਾਨਾ > t->state->val * ts;
		    }
		    ਜੇ (!ਨਤੀਜਾ)
		    {
			linenr_T    lnum = REG_MULTI
					    ? rex.reg_firstlnum + rex.lnum : 1;
			long_u	    vcol;

			ਜੇ (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ))
			    lnum = 1;
			vcol = (long_u)win_linetabsize(wp, lnum, rex.ਸਤਰ, ਖਾਨਾ);
			ਨਤੀਜਾ = nfa_re_ਅੰਕ_cmp(t->state->val, op, vcol + 1);
		    }
		    ਜੇ (ਨਤੀਜਾ)
		    {
			ਜੋੜੋ_ਇੱਥੇ = TRUE;
			ਜੋੜੋ_state = t->state->ਬਾਹਰਲਾ;
		    }
		}
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_MARK:
	    ਹਾਲਤ NFA_MARK_GT:
	    ਹਾਲਤ NFA_MARK_LT:
	      {
		pos_T	*pos;
		size_t	ਖਾਨਾ = REG_MULTI ? rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ : 0;

		pos = getmark_buf(rex.reg_buf, t->state->val, FALSE);

		// Line may have been freed, get it again.
		ਜੇ (REG_MULTI)
		{
		    rex.ਸਤਰ = reg_getline(rex.lnum);
		    rex.ਨਿਵੇਸ਼ = rex.ਸਤਰ + ਖਾਨਾ;
		}

		// Compare the mark position to the match position, if the mark
		// exists and mark is set in reg_buf.
		ਜੇ (pos != ਨਿਰਾ && pos->lnum > 0)
		{
		    colnr_T pos_ਖਾਨਾ = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->ਖਾਨਾ == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->ਖਾਨਾ;

		    ਨਤੀਜਾ = (pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_ਖਾਨਾ == (colnr_T)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ)
				    ? t->state->c == NFA_MARK
				    : (pos_ਖਾਨਾ < (colnr_T)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ)
					? t->state->c == NFA_MARK_GT
					: t->state->c == NFA_MARK_LT))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? t->state->c == NFA_MARK_GT
				    : t->state->c == NFA_MARK_LT));
		    ਜੇ (ਨਤੀਜਾ)
		    {
			ਜੋੜੋ_ਇੱਥੇ = TRUE;
			ਜੋੜੋ_state = t->state->ਬਾਹਰਲਾ;
		    }
		}
		ਰੋਕੋ;
	      }

	    ਹਾਲਤ NFA_CURSOR:
		ਨਤੀਜਾ = (rex.reg_win != ਨਿਰਾ
			&& (rex.lnum + rex.reg_firstlnum
						 == rex.reg_win->w_cursor.lnum)
			&& ((colnr_T)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ)
						== rex.reg_win->w_cursor.ਖਾਨਾ));
		ਜੇ (ਨਤੀਜਾ)
		{
		    ਜੋੜੋ_ਇੱਥੇ = TRUE;
		    ਜੋੜੋ_state = t->state->ਬਾਹਰਲਾ;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_VISUAL:
		ਨਤੀਜਾ = reg_ਮੇਲ_visual();
		ਜੇ (ਨਤੀਜਾ)
		{
		    ਜੋੜੋ_ਇੱਥੇ = TRUE;
		    ਜੋੜੋ_state = t->state->ਬਾਹਰਲਾ;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ NFA_MOPEN1:
	    ਹਾਲਤ NFA_MOPEN2:
	    ਹਾਲਤ NFA_MOPEN3:
	    ਹਾਲਤ NFA_MOPEN4:
	    ਹਾਲਤ NFA_MOPEN5:
	    ਹਾਲਤ NFA_MOPEN6:
	    ਹਾਲਤ NFA_MOPEN7:
	    ਹਾਲਤ NFA_MOPEN8:
	    ਹਾਲਤ NFA_MOPEN9:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	    ਹਾਲਤ NFA_ZOPEN:
	    ਹਾਲਤ NFA_ZOPEN1:
	    ਹਾਲਤ NFA_ZOPEN2:
	    ਹਾਲਤ NFA_ZOPEN3:
	    ਹਾਲਤ NFA_ZOPEN4:
	    ਹਾਲਤ NFA_ZOPEN5:
	    ਹਾਲਤ NFA_ZOPEN6:
	    ਹਾਲਤ NFA_ZOPEN7:
	    ਹਾਲਤ NFA_ZOPEN8:
	    ਹਾਲਤ NFA_ZOPEN9:
#ਜੇ_ਅੰਤ
	    ਹਾਲਤ NFA_NOPEN:
	    ਹਾਲਤ NFA_ZSTART:
		// These states are only added to be able to bail out when
		// they are added again, nothing is to be done.
		ਰੋਕੋ;

	    ਮੂਲ:	// regular character
	      {
		ਪੂਰਨ c = t->state->c;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
		ਜੇ (c < 0)
		    siemsg("Negative state char: %ld", (ਲੰਮਾ)c);
#ਜੇ_ਅੰਤ
		ਨਤੀਜਾ = (c == curc);

		ਜੇ (!ਨਤੀਜਾ && rex.reg_ic)
		    ਨਤੀਜਾ = MB_CASEFOLD(c) == MB_CASEFOLD(curc);
		// If rex.reg_icombine is not set only skip over the character
		// itself.  When it is set skip over composing characters.
		ਜੇ (ਨਤੀਜਾ && enc_utf8 && !rex.reg_icombine)
		    clen = utf_ptr2len(rex.ਨਿਵੇਸ਼);
		ਜੋੜੋ_STATE_IF_ਮੇਲ(t->state);
		ਰੋਕੋ;
	      }

	    } // switch (t->state->c)

	    ਜੇ (ਜੋੜੋ_state != ਨਿਰਾ)
	    {
		nfa_pim_T *pim;
		nfa_pim_T pim_copy;

		ਜੇ (t->pim.ਨਤੀਜਾ == NFA_PIM_ਅਣਵਰਤਿਆ)
		    pim = ਨਿਰਾ;
		ਨਹੀਂਤਾਂ
		    pim = &t->pim;

		// Handle the postponed invisible match if the match might end
		// without advancing and before the end of the line.
		ਜੇ (pim != ਨਿਰਾ && (clen == 0 || ਮੇਲ_follows(ਜੋੜੋ_state, 0)))
		{
		    ਜੇ (pim->ਨਤੀਜਾ == NFA_PIM_TODO)
		    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
			fprintf(log_fd, "\n");
			fprintf(log_fd, "==================================\n");
			fprintf(log_fd, "Postponed recursive nfa_regmatch()\n");
			fprintf(log_fd, "\n");
#ਜੇ_ਅੰਤ
			ਨਤੀਜਾ = recursive_regmatch(pim->state, pim,
				    prog, submatch, m, &listids, &listids_len);
			pim->ਨਤੀਜਾ = ਨਤੀਜਾ ? NFA_PIM_ਮੇਲ : NFA_PIM_NOMATCH;
			// for \@! and \@<! it is a match when the result is
			// FALSE
			ਜੇ (ਨਤੀਜਾ != (pim->state->c == NFA_ਸ਼ੁਰੂ_INVISIBLE_NEG
			     || pim->state->c == NFA_ਸ਼ੁਰੂ_INVISIBLE_NEG_ਪਹਿਲਾ
			     || pim->state->c
					   == NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_NEG_ਪਹਿਲਾ))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&pim->subs.norm, &m->norm);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
			    ਜੇ (rex.nfa_has_zsubexpr)
				copy_sub_off(&pim->subs.synt, &m->synt);
#ਜੇ_ਅੰਤ
			}
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਨਤੀਜਾ = (pim->ਨਤੀਜਾ == NFA_PIM_ਮੇਲ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
			fprintf(log_fd, "\n");
			fprintf(log_fd, "Using previous recursive nfa_regmatch() result, result == %d\n", pim->ਨਤੀਜਾ);
			fprintf(log_fd, "MATCH = %s\n", ਨਤੀਜਾ == TRUE ? "OK" : "FALSE");
			fprintf(log_fd, "\n");
#ਜੇ_ਅੰਤ
		    }

		    // for \@! and \@<! it is a match when result is FALSE
		    ਜੇ (ਨਤੀਜਾ != (pim->state->c == NFA_ਸ਼ੁਰੂ_INVISIBLE_NEG
			     || pim->state->c == NFA_ਸ਼ੁਰੂ_INVISIBLE_NEG_ਪਹਿਲਾ
			     || pim->state->c
					   == NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_ਸ਼ੁਰੂ_INVISIBLE_BEFORE_NEG_ਪਹਿਲਾ))
		    {
			// Copy submatch info from the recursive call
			copy_sub_off(&t->subs.norm, &pim->subs.norm);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
			ਜੇ (rex.nfa_has_zsubexpr)
			    copy_sub_off(&t->subs.synt, &pim->subs.synt);
#ਜੇ_ਅੰਤ
		    }
		    ਨਹੀਂਤਾਂ
			// look-behind match failed, don't add the state
			ਜਾਰੀ;

		    // Postponed invisible match was handled, don't add it to
		    // following states.
		    pim = ਨਿਰਾ;
		}

		// If "pim" points into l->t it will become invalid when
		// adding the state causes the list to be reallocated.  Make a
		// local copy to avoid that.
		ਜੇ (pim == &t->pim)
		{
		    copy_pim(&pim_copy, pim);
		    pim = &pim_copy;
		}

		ਜੇ (ਜੋੜੋ_ਇੱਥੇ)
		    r = addstate_ਇੱਥੇ(thislist, ਜੋੜੋ_state, &t->subs,
								pim, &listidx);
		ਨਹੀਂਤਾਂ
		{
		    r = addstate(nextlist, ਜੋੜੋ_state, &t->subs, pim, ਜੋੜੋ_off, 0);
		    ਜੇ (ਜੋੜੋ_ਗਿਣਤੀ > 0)
			nextlist->t[nextlist->n - 1].ਗਿਣਤੀ_2 = ਜੋੜੋ_ਗਿਣਤੀ;
		}
		ਜੇ (r == ਨਿਰਾ)
		{
		    nfa_ਮੇਲ = NFA_TOO_EXPENSIVE;
		    ਜਾਓ theend;
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
	ਜੇ (nfa_ਮੇਲ == FALSE
		&& ((toplevel
			&& rex.lnum == 0
			&& clen != 0
			&& (rex.reg_maxcol == 0
			  || (colnr_T)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ) < rex.reg_maxcol))
		    || (nfa_endp != ਨਿਰਾ
			&& (REG_MULTI
			    ? (rex.lnum < nfa_endp->se_u.pos.lnum
			       || (rex.lnum == nfa_endp->se_u.pos.lnum
				   && (ਪੂਰਨ)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ)
						    < nfa_endp->se_u.pos.ਖਾਨਾ))
			    : rex.ਨਿਵੇਸ਼ < nfa_endp->se_u.ptr))))
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
	    fprintf(log_fd, "(---) STARTSTATE\n");
#ਜੇ_ਅੰਤ
	    // Inline optimized code for addstate() if we know the state is
	    // the first MOPEN.
	    ਜੇ (toplevel)
	    {
		ਪੂਰਨ ਜੋੜੋ = TRUE;
		ਪੂਰਨ c;

		ਜੇ (prog->regstart != NUL && clen != 0)
		{
		    ਜੇ (nextlist->n == 0)
		    {
			colnr_T ਖਾਨਾ = (colnr_T)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ) + clen;

			// Nextlist is empty, we can skip ahead to the
			// character that must appear at the start.
			ਜੇ (skip_ਮੰਜ਼ਿਲ_ਸ਼ੁਰੂ(prog->regstart, &ਖਾਨਾ) == FAIL)
			    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
			fprintf(log_fd, "  Skipping ahead %d bytes to regstart\n",
				ਖਾਨਾ - ((colnr_T)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ) + clen));
#ਜੇ_ਅੰਤ
			rex.ਨਿਵੇਸ਼ = rex.ਸਤਰ + ਖਾਨਾ - clen;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// Checking if the required start character matches is
			// cheaper than adding a state that won't match.
			c = PTR2CHAR(rex.ਨਿਵੇਸ਼ + clen);
			ਜੇ (c != prog->regstart && (!rex.reg_ic
			     || MB_CASEFOLD(c) != MB_CASEFOLD(prog->regstart)))
			{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
			    fprintf(log_fd, "  Skipping start state, regstart does not match\n");
#ਜੇ_ਅੰਤ
			    ਜੋੜੋ = FALSE;
			}
		    }
		}

		ਜੇ (ਜੋੜੋ)
		{
		    ਜੇ (REG_MULTI)
		    {
			m->norm.list.multi[0].ਸ਼ੁਰੂ_ਖਾਨਾ =
					 (colnr_T)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ) + clen;
			m->norm.orig_ਸ਼ੁਰੂ_ਖਾਨਾ =
					       m->norm.list.multi[0].ਸ਼ੁਰੂ_ਖਾਨਾ;
		    }
		    ਨਹੀਂਤਾਂ
			m->norm.list.ਸਤਰ[0].ਸ਼ੁਰੂ = rex.ਨਿਵੇਸ਼ + clen;
		    ਜੇ (addstate(nextlist, ਸ਼ੁਰੂ->ਬਾਹਰਲਾ, m, ਨਿਰਾ, clen, 0) == ਨਿਰਾ)
		    {
			nfa_ਮੇਲ = NFA_TOO_EXPENSIVE;
			ਜਾਓ theend;
		    }
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		char_u	    *save_ਸਤਰ = rex.ਸਤਰ;
		char_u	    *save_ਨਿਵੇਸ਼ = rex.ਨਿਵੇਸ਼;
		linenr_T    save_lnum = rex.lnum;

		// At the end of a line the match can only start on the next
		// line, use that position instead of the line break.
		ਜੇ (REG_MULTI && clen == 0 && nfa_endp != ਨਿਰਾ
					 && rex.lnum < nfa_endp->se_u.pos.lnum)
		{
		    char_u  *ਅਗਲਾ_ਸਤਰ = reg_getline(rex.lnum + 1);

		    ਜੇ (ਅਗਲਾ_ਸਤਰ != ਨਿਰਾ)
		    {
			rex.ਸਤਰ = ਅਗਲਾ_ਸਤਰ;
			rex.ਨਿਵੇਸ਼ = rex.ਸਤਰ;
			++rex.lnum;
		    }
		}

		r = addstate(nextlist, ਸ਼ੁਰੂ, m, ਨਿਰਾ, clen, 0);

		rex.ਸਤਰ = save_ਸਤਰ;
		rex.ਨਿਵੇਸ਼ = save_ਨਿਵੇਸ਼;
		rex.lnum = save_lnum;

		ਜੇ (r == ਨਿਰਾ)
		{
		    nfa_ਮੇਲ = NFA_TOO_EXPENSIVE;
		    ਜਾਓ theend;
		}
	    }
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
	fprintf(log_fd, ">>> Thislist had %d states available: ", thislist->n);
	{
	    ਪੂਰਨ i;

	    ਲਈ (i = 0; i < thislist->n; i++)
		fprintf(log_fd, "%d  ", ਨਿਰਪੇਖ(thislist->t[i].state->id));
	}
	fprintf(log_fd, "\n");
#ਜੇ_ਅੰਤ

nextchar:
	// Advance to the next character, or advance to the next line, or
	// finish.
	ਜੇ (clen != 0)
	    rex.ਨਿਵੇਸ਼ += clen;
	ਨਹੀਂਤਾਂ ਜੇ (go_ਮੰਜ਼ਿਲ_nextline || (nfa_endp != ਨਿਰਾ && REG_MULTI
					&& rex.lnum < nfa_endp->se_u.pos.lnum))
	    reg_nextline();
	ਨਹੀਂਤਾਂ
	    ਰੋਕੋ;

	// Allow interrupting with CTRL-C.
	ਸਤਰ_breakcheck();
	ਜੇ (got_int)
	    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
	ਜੇ (nfa_did_ਸਮਾਂ_ਬਾਹਰਲਾ())
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
    ਜੇ (log_fd != stderr)
	fclose(log_fd);
    log_fd = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

theend:
    // Cache list buffers in prog for reuse, or free if prog already has
    // cached buffers (recursive call case).
    ਜੇ (prog->listbuf[0] == ਨਿਰਾ && list[0].t != ਨਿਰਾ && list[1].t != ਨਿਰਾ)
    {
	prog->listbuf[0] = list[0].t;
	prog->listbuf[1] = list[1].t;
    }
    ਨਹੀਂਤਾਂ
    {
	vim_free(list[0].t);
	vim_free(list[1].t);
    }
    vim_free(listids);
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ ਜੋੜੋ_STATE_IF_ਮੇਲ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NFA_REGEXP_DEBUG_LOG
    fclose(debug);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ nfa_ਮੇਲ;
}

/*
 * Try match of "prog" with at rex.line["col"].
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    ਸਥਿਰ ਲੰਮਾ
nfa_regtry(
    nfa_regprog_T   *prog,
    colnr_T	    ਖਾਨਾ,
    ਪੂਰਨ		    *timed_ਬਾਹਰਲਾ ਅਣਵਰਤਿਆ)	// flag set on timeout or NULL
{
    ਪੂਰਨ		i;
    regsubs_T	subs, m;
    nfa_state_T	*ਸ਼ੁਰੂ = prog->ਸ਼ੁਰੂ;
    ਪੂਰਨ		ਨਤੀਜਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
    FILE	*f;
#ਜੇ_ਅੰਤ

    rex.ਨਿਵੇਸ਼ = rex.ਸਤਰ + ਖਾਨਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
    nfa_timed_ਬਾਹਰਲਾ = timed_ਬਾਹਰਲਾ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
    f = fopen(NFA_REGEXP_ਲੜੀਵਾਰ_LOG, "a");
    ਜੇ (f != ਨਿਰਾ)
    {
	fprintf(f, "\n\n\t=======================================================\n");
# ifdef DEBUG
	fprintf(f, "\tRegexp is \"%s\"\n", nfa_regengine.ਵਿਅੰਜਕ);
# endif
	fprintf(f, "\tInput text is \"%s\" \n", rex.ਨਿਵੇਸ਼);
	fprintf(f, "\t=======================================================\n\n");
	nfa_print_state(f, ਸ਼ੁਰੂ);
	fprintf(f, "\n\n");
	fclose(f);
    }
    ਨਹੀਂਤਾਂ
	emsg("Could not open temporary log file for writing");
#ਜੇ_ਅੰਤ

    clear_sub(&subs.norm);
    clear_sub(&m.norm);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    clear_sub(&subs.synt);
    clear_sub(&m.synt);
#ਜੇ_ਅੰਤ

    ਨਤੀਜਾ = nfa_regmatch(prog, ਸ਼ੁਰੂ, &subs, &m);
    ਜੇ (ਨਤੀਜਾ == FALSE)
	ਵਾਪਸ 0;
    ਨਹੀਂਤਾਂ ਜੇ (ਨਤੀਜਾ == NFA_TOO_EXPENSIVE)
	ਵਾਪਸ ਨਤੀਜਾ;

    cleanup_subexpr();
    ਜੇ (REG_MULTI)
    {
	ਲਈ (i = 0; i < subs.norm.ਵਿੱਚ_use; i++)
	{
	    rex.reg_startpos[i].lnum = subs.norm.list.multi[i].ਸ਼ੁਰੂ_lnum;
	    rex.reg_startpos[i].ਖਾਨਾ = subs.norm.list.multi[i].ਸ਼ੁਰੂ_ਖਾਨਾ;

	    rex.reg_endpos[i].lnum = subs.norm.list.multi[i].ਸਿਰਾ_lnum;
	    rex.reg_endpos[i].ਖਾਨਾ = subs.norm.list.multi[i].ਸਿਰਾ_ਖਾਨਾ;
	}
	ਜੇ (rex.reg_mmatch != ਨਿਰਾ)
	    rex.reg_mmatch->rmm_matchcol = subs.norm.orig_ਸ਼ੁਰੂ_ਖਾਨਾ;

	ਜੇ (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].ਖਾਨਾ = ਖਾਨਾ;
	}
	ਜੇ (rex.reg_endpos[0].lnum < 0)
	{
	    // pattern has a \ze but it didn't match, use current end
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].ਖਾਨਾ = (ਪੂਰਨ)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ);
	}
	ਨਹੀਂਤਾਂ
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    ਨਹੀਂਤਾਂ
    {
	ਲਈ (i = 0; i < subs.norm.ਵਿੱਚ_use; i++)
	{
	    rex.reg_startp[i] = subs.norm.list.ਸਤਰ[i].ਸ਼ੁਰੂ;
	    rex.reg_endp[i] = subs.norm.list.ਸਤਰ[i].ਸਿਰਾ;
	}

	ਜੇ (rex.reg_startp[0] == ਨਿਰਾ)
	    rex.reg_startp[0] = rex.ਸਤਰ + ਖਾਨਾ;
	ਜੇ (rex.reg_endp[0] == ਨਿਰਾ)
	    rex.reg_endp[0] = rex.ਨਿਵੇਸ਼;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_ਬਾਹਰਲਾ);
    re_extmatch_ਬਾਹਰਲਾ = ਨਿਰਾ;

    ਜੇ (prog->reghasz == REX_ਸੈੱਟ)
    {
	cleanup_zsubexpr();
	re_extmatch_ਬਾਹਰਲਾ = make_extmatch();
	ਜੇ (re_extmatch_ਬਾਹਰਲਾ == ਨਿਰਾ)
	    ਵਾਪਸ 0;
	// Loop over \z1, \z2, etc.  There is no \z0.
	ਲਈ (i = 1; i < subs.synt.ਵਿੱਚ_use; i++)
	{
	    ਜੇ (REG_MULTI)
	    {
		ਬਣਤਰ multipos *mpos = &subs.synt.list.multi[i];

		// Only accept single line matches that are valid.
		ਜੇ (mpos->ਸ਼ੁਰੂ_lnum >= 0
			&& mpos->ਸ਼ੁਰੂ_lnum == mpos->ਸਿਰਾ_lnum
			&& mpos->ਸਿਰਾ_ਖਾਨਾ >= mpos->ਸ਼ੁਰੂ_ਖਾਨਾ)
		    re_extmatch_ਬਾਹਰਲਾ->matches[i] =
			vim_strnsave(reg_getline(mpos->ਸ਼ੁਰੂ_lnum)
							    + mpos->ਸ਼ੁਰੂ_ਖਾਨਾ,
					     mpos->ਸਿਰਾ_ਖਾਨਾ - mpos->ਸ਼ੁਰੂ_ਖਾਨਾ);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਬਣਤਰ linepos *lpos = &subs.synt.list.ਸਤਰ[i];

		ਜੇ (lpos->ਸ਼ੁਰੂ != ਨਿਰਾ && lpos->ਸਿਰਾ != ਨਿਰਾ)
		    re_extmatch_ਬਾਹਰਲਾ->matches[i] =
			    vim_strnsave(lpos->ਸ਼ੁਰੂ, lpos->ਸਿਰਾ - lpos->ਸ਼ੁਰੂ);
	    }
	}
    }
#ਜੇ_ਅੰਤ

    ਵਾਪਸ 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 *
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    ਸਥਿਰ ਲੰਮਾ
nfa_regexec_both(
    char_u	*ਸਤਰ,
    colnr_T	startcol,	// column to start looking for match
    ਪੂਰਨ		*timed_ਬਾਹਰਲਾ)	// flag set on timeout or NULL
{
    nfa_regprog_T   *prog;
    ਲੰਮਾ	    retval = 0L;
    ਪੂਰਨ		    i;
    colnr_T	    ਖਾਨਾ = startcol;

    ਜੇ (REG_MULTI)
    {
	prog = (nfa_regprog_T *)rex.reg_mmatch->regprog;
	ਸਤਰ = reg_getline((linenr_T)0);    // relative to the cursor
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    ਨਹੀਂਤਾਂ
    {
	prog = (nfa_regprog_T *)rex.reg_ਮੇਲ->regprog;
	rex.reg_startp = rex.reg_ਮੇਲ->startp;
	rex.reg_endp = rex.reg_ਮੇਲ->endp;
    }

    // Be paranoid...
    ਜੇ (prog == ਨਿਰਾ || ਸਤਰ == ਨਿਰਾ)
    {
	iemsg(e_null_ਦਲੀਲ);
	ਜਾਓ theend;
    }

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    ਜੇ (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    ਨਹੀਂਤਾਂ ਜੇ (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    ਜੇ (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    rex.ਸਤਰ = ਸਤਰ;
    rex.lnum = 0;    // relative to line

    rex.nfa_has_zend = prog->has_zend;
    rex.nfa_has_backref = prog->has_backref;
    rex.nfa_nsubexpr = prog->nsubexp;
    rex.nfa_listid = 1;
    rex.nfa_alt_listid = 2;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
    nfa_regengine.ਵਿਅੰਜਕ = prog->ਨਮੂਨਾ;
#ਜੇ_ਅੰਤ

    ਜੇ (prog->reganch && ਖਾਨਾ > 0)
	ਵਾਪਸ 0L;

    rex.need_clear_subexpr = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    ਜੇ (prog->reghasz == REX_ਸੈੱਟ)
    {
	rex.nfa_has_zsubexpr = TRUE;
	rex.need_clear_zsubexpr = TRUE;
    }
    ਨਹੀਂਤਾਂ
    {
	rex.nfa_has_zsubexpr = FALSE;
	rex.need_clear_zsubexpr = FALSE;
    }
#ਜੇ_ਅੰਤ

    ਜੇ (prog->regstart != NUL)
    {
	// Skip ahead until a character we know the match must start with.
	// When there is none there is no match.
	ਜੇ (skip_ਮੰਜ਼ਿਲ_ਸ਼ੁਰੂ(prog->regstart, &ਖਾਨਾ) == FAIL)
	    ਵਾਪਸ 0L;

	// If match_text is set it contains the full text that must match.
	// Nothing else to try. Doesn't handle combining chars well.
	ਜੇ (prog->ਮੇਲ_ਪਾਠ != ਨਿਰਾ && *prog->ਮੇਲ_ਪਾਠ != NUL && !rex.reg_icombine)
	{
	    retval = ਲੱਭੋ_ਮੇਲ_ਪਾਠ(&ਖਾਨਾ, prog->regstart, prog->ਮੇਲ_ਪਾਠ);
	    ਜੇ (REG_MULTI)
		rex.reg_mmatch->rmm_matchcol = ਖਾਨਾ;
	    ਨਹੀਂਤਾਂ
		rex.reg_ਮੇਲ->rm_matchcol = ਖਾਨਾ;
	    ਵਾਪਸ retval;
	}
    }

    // If the start column is past the maximum column: no need to try.
    ਜੇ (rex.reg_maxcol > 0 && ਖਾਨਾ >= rex.reg_maxcol)
	ਜਾਓ theend;

    // Set the "nstate" used by nfa_regcomp() to zero to trigger an error when
    // it's accidentally used during execution.
    nstate = 0;
    ਲਈ (i = 0; i < prog->nstate; ++i)
    {
	prog->state[i].id = i;
	prog->state[i].lastlist[0] = 0;
	prog->state[i].lastlist[1] = 0;
    }

    retval = nfa_regtry(prog, ਖਾਨਾ, timed_ਬਾਹਰਲਾ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
    nfa_regengine.ਵਿਅੰਜਕ = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

theend:
    ਜੇ (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs and
	// \ze are used.
	ਜੇ (REG_MULTI)
	{
	    lpos_T *ਸ਼ੁਰੂ = &rex.reg_mmatch->startpos[0];
	    lpos_T *ਸਿਰਾ = &rex.reg_mmatch->endpos[0];

	    ਜੇ (ਸਿਰਾ->lnum < ਸ਼ੁਰੂ->lnum
			|| (ਸਿਰਾ->lnum == ਸ਼ੁਰੂ->lnum && ਸਿਰਾ->ਖਾਨਾ < ਸ਼ੁਰੂ->ਖਾਨਾ))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (rex.reg_ਮੇਲ->endp[0] < rex.reg_ਮੇਲ->startp[0])
		rex.reg_ਮੇਲ->endp[0] = rex.reg_ਮੇਲ->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_ਮੇਲ->rm_matchcol = ਖਾਨਾ;
	}
    }

    ਵਾਪਸ retval;
}

/*
 * Compile a regular expression into internal code for the NFA matcher.
 * Returns the program in allocated space.  Returns NULL for an error.
 */
    ਸਥਿਰ regprog_T *
nfa_regcomp(char_u *ਵਿਅੰਜਕ, ਪੂਰਨ re_flags)
{
    nfa_regprog_T	*prog = ਨਿਰਾ;
    size_t		prog_ਮਾਪ;
    ਪੂਰਨ			*postfix;

    ਜੇ (ਵਿਅੰਜਕ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
    nfa_regengine.ਵਿਅੰਜਕ = ਵਿਅੰਜਕ;
#ਜੇ_ਅੰਤ
    nfa_re_flags = re_flags;

    init_class_tab();

    ਜੇ (nfa_regcomp_ਸ਼ੁਰੂ(ਵਿਅੰਜਕ, re_flags) == FAIL)
	ਵਾਪਸ ਨਿਰਾ;

    // Build postfix form of the regexp. Needed to build the NFA
    // (and count its size).
    postfix = re2post();
    ਜੇ (postfix == ਨਿਰਾ)
	ਜਾਓ fail;	    // Cascaded (syntax?) error

    /*
     * In order to build the NFA, we parse the input regexp twice:
     * 1. first pass to count size (so we can allocate space)
     * 2. second to emit code
     */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
    {
	FILE *f = fopen(NFA_REGEXP_ਲੜੀਵਾਰ_LOG, "a");

	ਜੇ (f != ਨਿਰਾ)
	{
	    fprintf(f, "\n*****************************\n\n\n\n\tCompiling regexp \"%s\"... hold on !\n", ਵਿਅੰਜਕ);
	    fclose(f);
	}
    }
#ਜੇ_ਅੰਤ

    /*
     * PASS 1
     * Count number of NFA states in "nstate". Do not build the NFA.
     */
    post2nfa(postfix, post_ptr, TRUE);

    // allocate the regprog with space for the compiled regexp
    prog_ਮਾਪ = offsetof(nfa_regprog_T, state) + ਆਕਾਰ(nfa_state_T) * nstate;
    prog = alloc(prog_ਮਾਪ);
    ਜੇ (prog == ਨਿਰਾ)
	ਜਾਓ fail;
    state_ptr = prog->state;
    prog->re_ਵਿੱਚ_use = FALSE;
    prog->listbuf[0] = ਨਿਰਾ;
    prog->listbuf[1] = ਨਿਰਾ;

    /*
     * PASS 2
     * Build the NFA
     */
    prog->ਸ਼ੁਰੂ = post2nfa(postfix, post_ptr, FALSE);
    ਜੇ (prog->ਸ਼ੁਰੂ == ਨਿਰਾ)
	ਜਾਓ fail;

    prog->regflags = regflags;
    prog->engine = &nfa_regengine;
    prog->nstate = nstate;
    prog->has_zend = rex.nfa_has_zend;
    prog->has_backref = rex.nfa_has_backref;
    prog->nsubexp = regnpar;

    nfa_postprocess(prog);

    prog->reganch = nfa_get_reganch(prog->ਸ਼ੁਰੂ, 0);
    prog->regstart = nfa_get_regstart(prog->ਸ਼ੁਰੂ, 0);
    prog->ਮੇਲ_ਪਾਠ = nfa_get_ਮੇਲ_ਪਾਠ(prog->ਸ਼ੁਰੂ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
    nfa_postfix_dump(ਵਿਅੰਜਕ, OK);
    nfa_dump(prog);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    prog->reghasz = re_has_z;
#ਜੇ_ਅੰਤ
    prog->ਨਮੂਨਾ = vim_strsave(ਵਿਅੰਜਕ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
    nfa_regengine.ਵਿਅੰਜਕ = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

ਬਾਹਰਲਾ:
    // The postfix list and fragment stack are reused by the next compilation
    // (and freed in free_regexp_stuff()), but drop ones that grew large for a
    // big pattern to avoid holding much memory.
    ਜੇ (post_ਸ਼ੁਰੂ_len > NFA_POSTFIX_KEEP)
    {
	VIM_CLEAR(post_ਸ਼ੁਰੂ);
	post_ਸ਼ੁਰੂ_len = 0;
    }
    post_ptr = post_ਸਿਰਾ = ਨਿਰਾ;
    ਜੇ (nfa_ਢੇਰ_len > NFA_ਢੇਰ_KEEP)
    {
	VIM_CLEAR(nfa_ਢੇਰ);
	nfa_ਢੇਰ_len = 0;
    }
    state_ptr = ਨਿਰਾ;
    ਵਾਪਸ (regprog_T *)prog;

fail:
    VIM_CLEAR(prog);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_LOG
    nfa_postfix_dump(ਵਿਅੰਜਕ, FAIL);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
    nfa_regengine.ਵਿਅੰਜਕ = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    ਜਾਓ ਬਾਹਰਲਾ;
}

/*
 * Free a compiled regexp program, returned by nfa_regcomp().
 */
    ਸਥਿਰ ਖਾਲੀ
nfa_regfree(regprog_T *prog)
{
    ਜੇ (prog == ਨਿਰਾ)
	ਵਾਪਸ;

    vim_free(((nfa_regprog_T *)prog)->ਮੇਲ_ਪਾਠ);
    vim_free(((nfa_regprog_T *)prog)->ਨਮੂਨਾ);
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
    ਸਥਿਰ ਪੂਰਨ
nfa_regexec_nl(
    regmatch_T	*rmp,
    char_u	*ਸਤਰ,	// string to match against
    colnr_T	ਖਾਨਾ,	// column to start looking for match
    ਪੂਰਨ		ਸਤਰ_lbr)
{
    rex.reg_ਮੇਲ = rmp;
    rex.reg_mmatch = ਨਿਰਾ;
    rex.reg_maxline = 0;
    rex.reg_ਸਤਰ_lbr = ਸਤਰ_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = ਨਿਰਾ;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;
    ਵਾਪਸ nfa_regexec_both(ਸਤਰ, ਖਾਨਾ, ਨਿਰਾ);
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
    ਸਥਿਰ ਲੰਮਾ
nfa_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	ਖਾਨਾ,		// column to start looking for match
    ਪੂਰਨ		*timed_ਬਾਹਰਲਾ)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    ਵਾਪਸ nfa_regexec_both(ਨਿਰਾ, ਖਾਨਾ, timed_ਬਾਹਰਲਾ);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
# undef ENABLE_LOG
#ਜੇ_ਅੰਤ
