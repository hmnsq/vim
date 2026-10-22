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
#ifdef DEBUG
# define NFA_REGEXP_ERROR_LOG	"nfa_regexp_error.log"
# define ENABLE_LOG
# define NFA_REGEXP_DUMP_LOG	"nfa_regexp_dump.log"
# define NFA_REGEXP_RUN_LOG	"nfa_regexp_run.log"
# define NFA_REGEXP_DEBUG_LOG	"nfa_regexp_debug.log"
#endif

// Added to NFA_ANY - NFA_NUPPER_IC to include a NL.
#define NFA_ADD_NL		31

ଗଣନା
{
    NFA_SPLIT = -1024,
    NFA_MATCH,
    NFA_EMPTY,			    // matches 0-length

    NFA_START_COLL,		    // [abc] start
    NFA_END_COLL,		    // [abc] end
    NFA_START_NEG_COLL,		    // [^abc] start
    NFA_END_NEG_COLL,		    // [^abc] end (postfix only)
    NFA_RANGE,			    // range of the two previous items
				    // (postfix only)
    NFA_RANGE_MIN,		    // low end of a range
    NFA_RANGE_MAX,		    // high end of a range

    NFA_CONCAT,			    // concatenate two previous items (postfix
				    // only)
    NFA_OR,			    // \| (postfix only)
    NFA_STAR,			    // greedy * (postfix only)
    NFA_STAR_NONGREEDY,		    // non-greedy * (postfix only)
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
    NFA_START_INVISIBLE,
    NFA_START_INVISIBLE_FIRST,
    NFA_START_INVISIBLE_NEG,
    NFA_START_INVISIBLE_NEG_FIRST,
    NFA_START_INVISIBLE_BEFORE,
    NFA_START_INVISIBLE_BEFORE_FIRST,
    NFA_START_INVISIBLE_BEFORE_NEG,
    NFA_START_INVISIBLE_BEFORE_NEG_FIRST,
    NFA_START_PATTERN,
    NFA_END_INVISIBLE,
    NFA_END_INVISIBLE_NEG,
    NFA_END_PATTERN,
    NFA_COMPOSING,		    // Next nodes in NFA are part of the
				    // composing multibyte char
    NFA_END_COMPOSING,		    // End of a composing char in the NFA
    NFA_ANY_COMPOSING,		    // \%C: Any composing characters.
    NFA_OPT_CHARS,		    // \%[abc]

    // The following are used only in the postfix form, not in the NFA
    NFA_PREV_ATOM_NO_WIDTH,	    // Used for \@=
    NFA_PREV_ATOM_NO_WIDTH_NEG,	    // Used for \@!
    NFA_PREV_ATOM_JUST_BEFORE,	    // Used for \@<=
    NFA_PREV_ATOM_JUST_BEFORE_NEG,  // Used for \@<!
    NFA_PREV_ATOM_LIKE_PATTERN,	    // Used for \@>

    NFA_BACKREF1,		    // \1
    NFA_BACKREF2,		    // \2
    NFA_BACKREF3,		    // \3
    NFA_BACKREF4,		    // \4
    NFA_BACKREF5,		    // \5
    NFA_BACKREF6,		    // \6
    NFA_BACKREF7,		    // \7
    NFA_BACKREF8,		    // \8
    NFA_BACKREF9,		    // \9
#ifdef FEAT_SYN_HL
    NFA_ZREF1,			    // \z1
    NFA_ZREF2,			    // \z2
    NFA_ZREF3,			    // \z3
    NFA_ZREF4,			    // \z4
    NFA_ZREF5,			    // \z5
    NFA_ZREF6,			    // \z6
    NFA_ZREF7,			    // \z7
    NFA_ZREF8,			    // \z8
    NFA_ZREF9,			    // \z9
#endif
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

#ifdef FEAT_SYN_HL
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
#endif

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
    NFA_WORD,		//	Match word char
    NFA_NWORD,		//	Match non-word char
    NFA_HEAD,		//	Match head char
    NFA_NHEAD,		//	Match non-head char
    NFA_ALPHA,		//	Match alpha char
    NFA_NALPHA,		//	Match non-alpha char
    NFA_LOWER,		//	Match lowercase char
    NFA_NLOWER,		//	Match non-lowercase char
    NFA_UPPER,		//	Match uppercase char
    NFA_NUPPER,		//	Match non-uppercase char
    NFA_LOWER_IC,	//	Match [a-z]
    NFA_NLOWER_IC,	//	Match [^a-z]
    NFA_UPPER_IC,	//	Match [A-Z]
    NFA_NUPPER_IC,	//	Match [^A-Z]

    NFA_FIRST_NL = NFA_ANY + NFA_ADD_NL,
    NFA_LAST_NL = NFA_NUPPER_IC + NFA_ADD_NL,

    NFA_CURSOR,		//	Match cursor pos
    NFA_LNUM,		//	Match line number
    NFA_LNUM_GT,	//	Match > line number
    NFA_LNUM_LT,	//	Match < line number
    NFA_COL,		//	Match cursor column
    NFA_COL_GT,		//	Match > cursor column
    NFA_COL_LT,		//	Match < cursor column
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
    NFA_CLASS_GRAPH,
    NFA_CLASS_LOWER,
    NFA_CLASS_PRINT,
    NFA_CLASS_PUNCT,
    NFA_CLASS_SPACE,
    NFA_CLASS_UPPER,
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
ସ୍ଥିର ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା nfa_classcodes[] = {
    NFA_ANY, NFA_IDENT, NFA_SIDENT, NFA_KWORD,NFA_SKWORD,
    NFA_FNAME, NFA_SFNAME, NFA_PRINT, NFA_SPRINT,
    NFA_WHITE, NFA_NWHITE, NFA_DIGIT, NFA_NDIGIT,
    NFA_HEX, NFA_NHEX, NFA_OCTAL, NFA_NOCTAL,
    NFA_WORD, NFA_NWORD, NFA_HEAD, NFA_NHEAD,
    NFA_ALPHA, NFA_NALPHA, NFA_LOWER, NFA_NLOWER,
    NFA_UPPER, NFA_NUPPER
};

// Variables only used in nfa_regcomp() and descendants.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା nfa_re_flags; // re_flags passed to nfa_regcomp()
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା *post_start;  // holds the postfix form of r.e.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା post_start_len;  // size of allocated post_start (in ints)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା *post_end;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା *post_ptr;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା nfa_reg_parse_depth;	// nesting depth in nfa_reg()

// The postfix list (post_start) and fragment stack (nfa_stack) are reused
// across compilations; a buffer that grew past these sizes for a big pattern
// is freed afterwards instead of being kept, to avoid holding much memory.
#define NFA_POSTFIX_KEEP	10000	// number of ints (~40 Kbyte)
#define NFA_ଗଦା_KEEP		4000	// number of Frag_T (~64 Kbyte)

// Set when the pattern should use the NFA engine.
// E.g. [[:upper:]] only allows 8bit characters for BT engine,
// while NFA engine handles multibyte characters correctly.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା wants_nfa;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା nstate;	// Number of states in the NFA.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା istate;	// Index in the state vector, used in alloc_state()

// If not NULL match must end at this position
ସ୍ଥିର save_se_T *nfa_endp = ଖାଲି;

// 0 for first call to nfa_regmatch(), 1 for recursive call.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା nfa_ll_index = 0;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା realloc_post_list(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା nfa_reg(ପୂର୍ଣ୍ଣସଂଖ୍ୟା paren);
#ifdef DEBUG
ସ୍ଥିର ଶୂନ୍ୟ nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent);
#endif
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା match_follows(nfa_state_T *startstate, ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା failure_chance(nfa_state_T *state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth);

// helper functions used when doing re2post() ... regatom() parsing
#define EMIT(c)	କର {				\
		    ଯଦି (post_ptr >= post_end && realloc_post_list() == FAIL) \
			ଫେରନ୍ତୁ FAIL;		\
		    *post_ptr++ = c;		\
		} ଯେତେବେଳେ (0)

/*
 * Initialize internal variables before NFA compilation.
 * Return OK on success, FAIL otherwise.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
nfa_regcomp_start(
    char_u	*ପଦସମଷ୍ଟି,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		re_flags)	    // see vim_regcomp()
{
    size_t	postfix_size;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nstate_max;

    nstate = 0;
    istate = 0;
    // A reasonable estimation for maximum size
    nstate_max = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(STRLEN(ପଦସମଷ୍ଟି) + 1) * 25;

    // Some items blow up in size, such as [A-z].  Add more space for that.
    // When it is still not enough realloc_post_list() will be used.
    nstate_max += 1000;

    // Size for postfix representation of expr.
    postfix_size = ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) * nstate_max;

    // Reuse the postfix buffer across compilations, only growing it when the
    // estimate exceeds the current size; it is freed in free_regexp_stuff().
    ଯଦି (post_start == ଖାଲି || post_start_len < nstate_max)
    {
	vim_free(post_start);
	post_start = alloc(postfix_size);
	ଯଦି (post_start == ଖାଲି)
	{
	    post_start_len = 0;
	    ଫେରନ୍ତୁ FAIL;
	}
	post_start_len = nstate_max;
    }
    post_ptr = post_start;
    post_end = post_start + post_start_len;
    wants_nfa = FALSE;
    rex.nfa_has_zend = FALSE;
    rex.nfa_has_backref = FALSE;
    nfa_reg_parse_depth = 0;

    // shared with BT engine
    regcomp_start(ପଦସମଷ୍ଟି, re_flags);

    ଫେରନ୍ତୁ OK;
}

/*
 * Figure out if the NFA state list starts with an anchor, must match at start
 * of the line.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
nfa_get_reganch(nfa_state_T *start, ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth)
{
    nfa_state_T *p = start;

    ଯଦି (depth > 4)
	ଫେରନ୍ତୁ 0;

    ଯେତେବେଳେ (p != ଖାଲି)
    {
	ବାଛ (p->c)
	{
	    ସ୍ଥିତି NFA_BOL:
	    ସ୍ଥିତି NFA_BOF:
		ଫେରନ୍ତୁ 1; // yes!

	    ସ୍ଥିତି NFA_ZSTART:
	    ସ୍ଥିତି NFA_ZEND:
	    ସ୍ଥିତି NFA_CURSOR:
	    ସ୍ଥିତି NFA_VISUAL:

	    ସ୍ଥିତି NFA_MOPEN:
	    ସ୍ଥିତି NFA_MOPEN1:
	    ସ୍ଥିତି NFA_MOPEN2:
	    ସ୍ଥିତି NFA_MOPEN3:
	    ସ୍ଥିତି NFA_MOPEN4:
	    ସ୍ଥିତି NFA_MOPEN5:
	    ସ୍ଥିତି NFA_MOPEN6:
	    ସ୍ଥିତି NFA_MOPEN7:
	    ସ୍ଥିତି NFA_MOPEN8:
	    ସ୍ଥିତି NFA_MOPEN9:
	    ସ୍ଥିତି NFA_NOPEN:
#ifdef FEAT_SYN_HL
	    ସ୍ଥିତି NFA_ZOPEN:
	    ସ୍ଥିତି NFA_ZOPEN1:
	    ସ୍ଥିତି NFA_ZOPEN2:
	    ସ୍ଥିତି NFA_ZOPEN3:
	    ସ୍ଥିତି NFA_ZOPEN4:
	    ସ୍ଥିତି NFA_ZOPEN5:
	    ସ୍ଥିତି NFA_ZOPEN6:
	    ସ୍ଥିତି NFA_ZOPEN7:
	    ସ୍ଥିତି NFA_ZOPEN8:
	    ସ୍ଥିତି NFA_ZOPEN9:
#endif
		p = p->out;
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_SPLIT:
		ଫେରନ୍ତୁ nfa_get_reganch(p->out, depth + 1)
				       && nfa_get_reganch(p->out1, depth + 1);

	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଫେରନ୍ତୁ 0; // noooo
	}
    }
    ଫେରନ୍ତୁ 0;
}

/*
 * Figure out if the NFA state list starts with a character which must match
 * at start of the match.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
nfa_get_regstart(nfa_state_T *start, ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth)
{
    nfa_state_T *p = start;

    ଯଦି (depth > 4)
	ଫେରନ୍ତୁ 0;

    ଯେତେବେଳେ (p != ଖାଲି)
    {
	ବାଛ (p->c)
	{
	    // all kinds of zero-width matches
	    ସ୍ଥିତି NFA_BOL:
	    ସ୍ଥିତି NFA_BOF:
	    ସ୍ଥିତି NFA_BOW:
	    ସ୍ଥିତି NFA_EOW:
	    ସ୍ଥିତି NFA_ZSTART:
	    ସ୍ଥିତି NFA_ZEND:
	    ସ୍ଥିତି NFA_CURSOR:
	    ସ୍ଥିତି NFA_VISUAL:
	    ସ୍ଥିତି NFA_LNUM:
	    ସ୍ଥିତି NFA_LNUM_GT:
	    ସ୍ଥିତି NFA_LNUM_LT:
	    ସ୍ଥିତି NFA_COL:
	    ସ୍ଥିତି NFA_COL_GT:
	    ସ୍ଥିତି NFA_COL_LT:
	    ସ୍ଥିତି NFA_VCOL:
	    ସ୍ଥିତି NFA_VCOL_GT:
	    ସ୍ଥିତି NFA_VCOL_LT:
	    ସ୍ଥିତି NFA_MARK:
	    ସ୍ଥିତି NFA_MARK_GT:
	    ସ୍ଥିତି NFA_MARK_LT:

	    ସ୍ଥିତି NFA_MOPEN:
	    ସ୍ଥିତି NFA_MOPEN1:
	    ସ୍ଥିତି NFA_MOPEN2:
	    ସ୍ଥିତି NFA_MOPEN3:
	    ସ୍ଥିତି NFA_MOPEN4:
	    ସ୍ଥିତି NFA_MOPEN5:
	    ସ୍ଥିତି NFA_MOPEN6:
	    ସ୍ଥିତି NFA_MOPEN7:
	    ସ୍ଥିତି NFA_MOPEN8:
	    ସ୍ଥିତି NFA_MOPEN9:
	    ସ୍ଥିତି NFA_NOPEN:
#ifdef FEAT_SYN_HL
	    ସ୍ଥିତି NFA_ZOPEN:
	    ସ୍ଥିତି NFA_ZOPEN1:
	    ସ୍ଥିତି NFA_ZOPEN2:
	    ସ୍ଥିତି NFA_ZOPEN3:
	    ସ୍ଥିତି NFA_ZOPEN4:
	    ସ୍ଥିତି NFA_ZOPEN5:
	    ସ୍ଥିତି NFA_ZOPEN6:
	    ସ୍ଥିତି NFA_ZOPEN7:
	    ସ୍ଥିତି NFA_ZOPEN8:
	    ସ୍ଥିତି NFA_ZOPEN9:
#endif
		p = p->out;
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_SPLIT:
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା c1 = nfa_get_regstart(p->out, depth + 1);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା c2 = nfa_get_regstart(p->out1, depth + 1);

		ଯଦି (c1 == c2)
		    ଫେରନ୍ତୁ c1; // yes!
		ଫେରନ୍ତୁ 0;
	    }

	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଯଦି (p->c > 0)
		    ଫେରନ୍ତୁ p->c; // yes!
		ଫେରନ୍ତୁ 0;
	}
    }
    ଫେରନ୍ତୁ 0;
}

/*
 * Figure out if the NFA state list contains just literal text and nothing
 * else.  If so return a string in allocated memory with what must match after
 * regstart.  Otherwise return NULL.
 */
    ସ୍ଥିର char_u *
nfa_get_match_text(nfa_state_T *start)
{
    nfa_state_T *p = start;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 0;
    char_u	*ret;
    char_u	*s;

    ଯଦି (p->c != NFA_MOPEN)
	ଫେରନ୍ତୁ ଖାଲି; // just in case
    p = p->out;
    ଯେତେବେଳେ (p->c > 0)
    {
	len += MB_CHAR2LEN(p->c);
	p = p->out;
    }
    ଯଦି (p->c != NFA_MCLOSE || p->out->c != NFA_MATCH)
	ଫେରନ୍ତୁ ଖାଲି;

    ret = alloc(len);
    ଯଦି (ret == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    p = start->out->out; // skip first char, it goes into regstart
    s = ret;
    ଯେତେବେଳେ (p->c > 0)
    {
	ଯଦି (has_mbyte)
	    s += (*mb_char2bytes)(p->c, s);
	ଅନ୍ୟଥା
	    *s++ = p->c;
	p = p->out;
    }
    *s = NUL;
    ଫେରନ୍ତୁ ret;
}

/*
 * Allocate more space for post_start.  Called when
 * running above the estimated number of states.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
realloc_post_list(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା   nstate_max = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(post_end - post_start);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା   new_max;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା   *new_start;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  *old_start;

    // For weird patterns the number of states can be very high. Increasing by
    // 50% seems a reasonable compromise between memory use and speed.
    new_max = nstate_max * 3 / 2;
    new_start = ALLOC_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, new_max);
    ଯଦି (new_start == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    mch_memmove(new_start, post_start, nstate_max * ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା));
    old_start = post_start;
    post_start = new_start;
    post_start_len = new_max;
    post_ptr = new_start + (post_ptr - old_start);
    post_end = post_start + new_max;
    vim_free(old_start);
    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
nfa_recognize_char_class(char_u *start, char_u *end, ପୂର୍ଣ୍ଣସଂଖ୍ୟା extra_newl)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		newl;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		config = get_char_class_bits(start, end, &newl);

    ଯଦି (config < 0)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (extra_newl == TRUE)
	newl = TRUE;

    ଯଦି (newl == TRUE)
	extra_newl = NFA_ADD_NL;

    ବାଛ (config)
    {
	ସ୍ଥିତି CLASS_o9:
	    ଫେରନ୍ତୁ extra_newl + NFA_DIGIT;
	ସ୍ଥିତି CLASS_not |  CLASS_o9:
	    ଫେରନ୍ତୁ extra_newl + NFA_NDIGIT;
	ସ୍ଥିତି CLASS_af | CLASS_AF | CLASS_o9:
	    ଫେରନ୍ତୁ extra_newl + NFA_HEX;
	ସ୍ଥିତି CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    ଫେରନ୍ତୁ extra_newl + NFA_NHEX;
	ସ୍ଥିତି CLASS_o7:
	    ଫେରନ୍ତୁ extra_newl + NFA_OCTAL;
	ସ୍ଥିତି CLASS_not | CLASS_o7:
	    ଫେରନ୍ତୁ extra_newl + NFA_NOCTAL;
	ସ୍ଥିତି CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    ଫେରନ୍ତୁ extra_newl + NFA_WORD;
	ସ୍ଥିତି CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    ଫେରନ୍ତୁ extra_newl + NFA_NWORD;
	ସ୍ଥିତି CLASS_az | CLASS_AZ | CLASS_underscore:
	    ଫେରନ୍ତୁ extra_newl + NFA_HEAD;
	ସ୍ଥିତି CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    ଫେରନ୍ତୁ extra_newl + NFA_NHEAD;
	ସ୍ଥିତି CLASS_az | CLASS_AZ:
	    ଫେରନ୍ତୁ extra_newl + NFA_ALPHA;
	ସ୍ଥିତି CLASS_not | CLASS_az | CLASS_AZ:
	    ଫେରନ୍ତୁ extra_newl + NFA_NALPHA;
	ସ୍ଥିତି CLASS_az:
	   ଫେରନ୍ତୁ extra_newl + NFA_LOWER_IC;
	ସ୍ଥିତି CLASS_not | CLASS_az:
	    ଫେରନ୍ତୁ extra_newl + NFA_NLOWER_IC;
	ସ୍ଥିତି CLASS_AZ:
	    ଫେରନ୍ତୁ extra_newl + NFA_UPPER_IC;
	ସ୍ଥିତି CLASS_not | CLASS_AZ:
	    ଫେରନ୍ତୁ extra_newl + NFA_NUPPER_IC;
    }
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * Emits bytes in postfix notation: 'a,b,NFA_OR,c,NFA_OR' is
 * equivalent to 'a OR b OR c'
 *
 * NOTE! When changing this function, also update reg_equi_class()
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
nfa_emit_equi_class(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
#define EMIT2(c)    EMIT(c); EMIT(NFA_CONCAT);

    ଯଦି (enc_utf8 || STRCMP(p_enc, "latin1") == 0
					 || STRCMP(p_enc, "iso-8859-15") == 0)
    {
#define A_grave 0xc0
#define A_acute 0xc1
#define A_circumflex 0xc2
#define A_virguilla 0xc3
#define A_diaeresis 0xc4
#define A_ring 0xc5
#define C_cedilla 0xc7
#define E_grave 0xc8
#define E_acute 0xc9
#define E_circumflex 0xca
#define E_diaeresis 0xcb
#define I_grave 0xcc
#define I_acute 0xcd
#define I_circumflex 0xce
#define I_diaeresis 0xcf
#define N_virguilla 0xd1
#define O_grave 0xd2
#define O_acute 0xd3
#define O_circumflex 0xd4
#define O_virguilla 0xd5
#define O_diaeresis 0xd6
#define O_slash 0xd8
#define U_grave 0xd9
#define U_acute 0xda
#define U_circumflex 0xdb
#define U_diaeresis 0xdc
#define Y_acute 0xdd
#define a_grave 0xe0
#define a_acute 0xe1
#define a_circumflex 0xe2
#define a_virguilla 0xe3
#define a_diaeresis 0xe4
#define a_ring 0xe5
#define c_cedilla 0xe7
#define e_grave 0xe8
#define e_acute 0xe9
#define e_circumflex 0xea
#define e_diaeresis 0xeb
#define i_grave 0xec
#define i_acute 0xed
#define i_circumflex 0xee
#define i_diaeresis 0xef
#define n_virguilla 0xf1
#define o_grave 0xf2
#define o_acute 0xf3
#define o_circumflex 0xf4
#define o_virguilla 0xf5
#define o_diaeresis 0xf6
#define o_slash 0xf8
#define u_grave 0xf9
#define u_acute 0xfa
#define u_circumflex 0xfb
#define u_diaeresis 0xfc
#define y_acute 0xfd
#define y_diaeresis 0xff
	ବାଛ (c)
	{
	    ସ୍ଥିତି 'A': ସ୍ଥିତି A_grave: ସ୍ଥିତି A_acute: ସ୍ଥିତି A_circumflex:
	    ସ୍ଥିତି A_virguilla: ସ୍ଥିତି A_diaeresis: ସ୍ଥିତି A_ring:
	    ସ୍ଥିତି 0x100: ସ୍ଥିତି 0x102: ସ୍ଥିତି 0x104: ସ୍ଥିତି 0x1cd:
	    ସ୍ଥିତି 0x1de: ସ୍ଥିତି 0x1e0: ସ୍ଥିତି 0x1fa: ସ୍ଥିତି 0x200:
	    ସ୍ଥିତି 0x202: ସ୍ଥିତି 0x226: ସ୍ଥିତି 0x23a: ସ୍ଥିତି 0x1e00:
	    ସ୍ଥିତି 0x1ea0: ସ୍ଥିତି 0x1ea2: ସ୍ଥିତି 0x1ea4: ସ୍ଥିତି 0x1ea6:
	    ସ୍ଥିତି 0x1ea8: ସ୍ଥିତି 0x1eaa: ସ୍ଥିତି 0x1eac: ସ୍ଥିତି 0x1eae:
	    ସ୍ଥିତି 0x1eb0: ସ୍ଥିତି 0x1eb2: ସ୍ଥିତି 0x1eb4: ସ୍ଥିତି 0x1eb6:
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
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'B': ସ୍ଥିତି 0x181: ସ୍ଥିତି 0x243: ସ୍ଥିତି 0x1e02:
	    ସ୍ଥିତି 0x1e04: ସ୍ଥିତି 0x1e06:
		    EMIT2('B')
		    EMIT2(0x181) EMIT2(0x243) EMIT2(0x1e02)
		    EMIT2(0x1e04) EMIT2(0x1e06)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'C': ସ୍ଥିତି C_cedilla: ସ୍ଥିତି 0x106: ସ୍ଥିତି 0x108:
	    ସ୍ଥିତି 0x10a: ସ୍ଥିତି 0x10c: ସ୍ଥିତି 0x187: ସ୍ଥିତି 0x23b:
	    ସ୍ଥିତି 0x1e08: ସ୍ଥିତି 0xa792:
		    EMIT2('C') EMIT2(C_cedilla)
		    EMIT2(0x106) EMIT2(0x108) EMIT2(0x10a)
		    EMIT2(0x10c) EMIT2(0x187) EMIT2(0x23b)
		    EMIT2(0x1e08) EMIT2(0xa792)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'D': ସ୍ଥିତି 0x10e: ସ୍ଥିତି 0x110: ସ୍ଥିତି 0x18a:
	    ସ୍ଥିତି 0x1e0a: ସ୍ଥିତି 0x1e0c: ସ୍ଥିତି 0x1e0e: ସ୍ଥିତି 0x1e10:
	    ସ୍ଥିତି 0x1e12:
		    EMIT2('D') EMIT2(0x10e) EMIT2(0x110) EMIT2(0x18a)
		    EMIT2(0x1e0a) EMIT2(0x1e0c) EMIT2(0x1e0e)
		    EMIT2(0x1e10) EMIT2(0x1e12)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'E': ସ୍ଥିତି E_grave: ସ୍ଥିତି E_acute: ସ୍ଥିତି E_circumflex:
	    ସ୍ଥିତି E_diaeresis: ସ୍ଥିତି 0x112: ସ୍ଥିତି 0x114: ସ୍ଥିତି 0x116:
	    ସ୍ଥିତି 0x118: ସ୍ଥିତି 0x11a: ସ୍ଥିତି 0x204: ସ୍ଥିତି 0x206:
	    ସ୍ଥିତି 0x228: ସ୍ଥିତି 0x246: ସ୍ଥିତି 0x1e14: ସ୍ଥିତି 0x1e16:
	    ସ୍ଥିତି 0x1e18: ସ୍ଥିତି 0x1e1a: ସ୍ଥିତି 0x1e1c: ସ୍ଥିତି 0x1eb8:
	    ସ୍ଥିତି 0x1eba: ସ୍ଥିତି 0x1ebc: ସ୍ଥିତି 0x1ebe: ସ୍ଥିତି 0x1ec0:
	    ସ୍ଥିତି 0x1ec2: ସ୍ଥିତି 0x1ec4: ସ୍ଥିତି 0x1ec6:
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
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'F': ସ୍ଥିତି 0x191: ସ୍ଥିତି 0x1e1e: ସ୍ଥିତି 0xa798:
		    EMIT2('F') EMIT2(0x191) EMIT2(0x1e1e) EMIT2(0xa798)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'G': ସ୍ଥିତି 0x11c: ସ୍ଥିତି 0x11e: ସ୍ଥିତି 0x120:
	    ସ୍ଥିତି 0x122: ସ୍ଥିତି 0x193: ସ୍ଥିତି 0x1e4: ସ୍ଥିତି 0x1e6:
	    ସ୍ଥିତି 0x1f4: ସ୍ଥିତି 0x1e20: ସ୍ଥିତି 0xa7a0:
		    EMIT2('G') EMIT2(0x11c) EMIT2(0x11e) EMIT2(0x120)
		    EMIT2(0x122) EMIT2(0x193) EMIT2(0x1e4)
		    EMIT2(0x1e6) EMIT2(0x1f4) EMIT2(0x1e20)
		    EMIT2(0xa7a0)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'H': ସ୍ଥିତି 0x124: ସ୍ଥିତି 0x126: ସ୍ଥିତି 0x21e:
	    ସ୍ଥିତି 0x1e22: ସ୍ଥିତି 0x1e24: ସ୍ଥିତି 0x1e26: ସ୍ଥିତି 0x1e28:
	    ସ୍ଥିତି 0x1e2a: ସ୍ଥିତି 0x2c67:
		    EMIT2('H') EMIT2(0x124) EMIT2(0x126) EMIT2(0x21e)
		    EMIT2(0x1e22) EMIT2(0x1e24) EMIT2(0x1e26)
		    EMIT2(0x1e28) EMIT2(0x1e2a) EMIT2(0x2c67)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'I': ସ୍ଥିତି I_grave: ସ୍ଥିତି I_acute: ସ୍ଥିତି I_circumflex:
	    ସ୍ଥିତି I_diaeresis: ସ୍ଥିତି 0x128: ସ୍ଥିତି 0x12a: ସ୍ଥିତି 0x12c:
	    ସ୍ଥିତି 0x12e: ସ୍ଥିତି 0x130: ସ୍ଥିତି 0x197: ସ୍ଥିତି 0x1cf:
	    ସ୍ଥିତି 0x208: ସ୍ଥିତି 0x20a: ସ୍ଥିତି 0x1e2c: ସ୍ଥିତି 0x1e2e:
	    ସ୍ଥିତି 0x1ec8: ସ୍ଥିତି 0x1eca:
		    EMIT2('I') EMIT2(I_grave) EMIT2(I_acute)
		    EMIT2(I_circumflex) EMIT2(I_diaeresis)
		    EMIT2(0x128) EMIT2(0x12a) EMIT2(0x12c)
		    EMIT2(0x12e) EMIT2(0x130) EMIT2(0x197)
		    EMIT2(0x1cf) EMIT2(0x208) EMIT2(0x20a)
		    EMIT2(0x1e2c) EMIT2(0x1e2e) EMIT2(0x1ec8)
		    EMIT2(0x1eca)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'J': ସ୍ଥିତି 0x134: ସ୍ଥିତି 0x248:
		    EMIT2('J') EMIT2(0x134) EMIT2(0x248)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'K': ସ୍ଥିତି 0x136: ସ୍ଥିତି 0x198: ସ୍ଥିତି 0x1e8: ସ୍ଥିତି 0x1e30:
	    ସ୍ଥିତି 0x1e32: ସ୍ଥିତି 0x1e34: ସ୍ଥିତି 0x2c69: ସ୍ଥିତି 0xa740:
		    EMIT2('K') EMIT2(0x136) EMIT2(0x198) EMIT2(0x1e8)
		    EMIT2(0x1e30) EMIT2(0x1e32) EMIT2(0x1e34)
		    EMIT2(0x2c69) EMIT2(0xa740)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'L': ସ୍ଥିତି 0x139: ସ୍ଥିତି 0x13b: ସ୍ଥିତି 0x13d:
	    ସ୍ଥିତି 0x13f: ସ୍ଥିତି 0x141: ସ୍ଥିତି 0x23d: ସ୍ଥିତି 0x1e36:
	    ସ୍ଥିତି 0x1e38: ସ୍ଥିତି 0x1e3a: ସ୍ଥିତି 0x1e3c: ସ୍ଥିତି 0x2c60:
		    EMIT2('L') EMIT2(0x139) EMIT2(0x13b)
		    EMIT2(0x13d) EMIT2(0x13f) EMIT2(0x141)
		    EMIT2(0x23d) EMIT2(0x1e36) EMIT2(0x1e38)
		    EMIT2(0x1e3a) EMIT2(0x1e3c) EMIT2(0x2c60)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'M': ସ୍ଥିତି 0x1e3e: ସ୍ଥିତି 0x1e40: ସ୍ଥିତି 0x1e42:
		    EMIT2('M') EMIT2(0x1e3e) EMIT2(0x1e40)
		    EMIT2(0x1e42)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'N': ସ୍ଥିତି N_virguilla:
	    ସ୍ଥିତି 0x143: ସ୍ଥିତି 0x145: ସ୍ଥିତି 0x147: ସ୍ଥିତି 0x1f8:
	    ସ୍ଥିତି 0x1e44: ସ୍ଥିତି 0x1e46: ସ୍ଥିତି 0x1e48: ସ୍ଥିତି 0x1e4a:
	    ସ୍ଥିତି 0xa7a4:
		    EMIT2('N') EMIT2(N_virguilla)
		    EMIT2(0x143) EMIT2(0x145) EMIT2(0x147)
		    EMIT2(0x1f8) EMIT2(0x1e44) EMIT2(0x1e46)
		    EMIT2(0x1e48) EMIT2(0x1e4a) EMIT2(0xa7a4)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'O': ସ୍ଥିତି O_grave: ସ୍ଥିତି O_acute: ସ୍ଥିତି O_circumflex:
	    ସ୍ଥିତି O_virguilla: ସ୍ଥିତି O_diaeresis: ସ୍ଥିତି O_slash:
	    ସ୍ଥିତି 0x14c: ସ୍ଥିତି 0x14e: ସ୍ଥିତି 0x150: ସ୍ଥିତି 0x19f:
	    ସ୍ଥିତି 0x1a0: ସ୍ଥିତି 0x1d1: ସ୍ଥିତି 0x1ea: ସ୍ଥିତି 0x1ec:
	    ସ୍ଥିତି 0x1fe: ସ୍ଥିତି 0x20c: ସ୍ଥିତି 0x20e: ସ୍ଥିତି 0x22a:
	    ସ୍ଥିତି 0x22c: ସ୍ଥିତି 0x22e: ସ୍ଥିତି 0x230: ସ୍ଥିତି 0x1e4c:
	    ସ୍ଥିତି 0x1e4e: ସ୍ଥିତି 0x1e50: ସ୍ଥିତି 0x1e52: ସ୍ଥିତି 0x1ecc:
	    ସ୍ଥିତି 0x1ece: ସ୍ଥିତି 0x1ed0: ସ୍ଥିତି 0x1ed2: ସ୍ଥିତି 0x1ed4:
	    ସ୍ଥିତି 0x1ed6: ସ୍ଥିତି 0x1ed8: ସ୍ଥିତି 0x1eda: ସ୍ଥିତି 0x1edc:
	    ସ୍ଥିତି 0x1ede: ସ୍ଥିତି 0x1ee0: ସ୍ଥିତି 0x1ee2:
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
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'P': ସ୍ଥିତି 0x1a4: ସ୍ଥିତି 0x1e54: ସ୍ଥିତି 0x1e56: ସ୍ଥିତି 0x2c63:
		    EMIT2('P') EMIT2(0x1a4) EMIT2(0x1e54) EMIT2(0x1e56)
		    EMIT2(0x2c63)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'Q': ସ୍ଥିତି 0x24a:
		    EMIT2('Q') EMIT2(0x24a)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'R': ସ୍ଥିତି 0x154: ସ୍ଥିତି 0x156: ସ୍ଥିତି 0x158: ସ୍ଥିତି 0x210:
	    ସ୍ଥିତି 0x212: ସ୍ଥିତି 0x24c: ସ୍ଥିତି 0x1e58: ସ୍ଥିତି 0x1e5a:
	    ସ୍ଥିତି 0x1e5c: ସ୍ଥିତି 0x1e5e: ସ୍ଥିତି 0x2c64: ସ୍ଥିତି 0xa7a6:
		    EMIT2('R') EMIT2(0x154) EMIT2(0x156) EMIT2(0x158)
		    EMIT2(0x210) EMIT2(0x212) EMIT2(0x24c) EMIT2(0x1e58)
		    EMIT2(0x1e5a) EMIT2(0x1e5c) EMIT2(0x1e5e) EMIT2(0x2c64)
		    EMIT2(0xa7a6)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'S': ସ୍ଥିତି 0x15a: ସ୍ଥିତି 0x15c: ସ୍ଥିତି 0x15e: ସ୍ଥିତି 0x160:
	    ସ୍ଥିତି 0x218: ସ୍ଥିତି 0x1e60: ସ୍ଥିତି 0x1e62: ସ୍ଥିତି 0x1e64:
	    ସ୍ଥିତି 0x1e66: ସ୍ଥିତି 0x1e68: ସ୍ଥିତି 0x2c7e: ସ୍ଥିତି 0xa7a8:
		    EMIT2('S') EMIT2(0x15a) EMIT2(0x15c) EMIT2(0x15e)
		    EMIT2(0x160) EMIT2(0x218) EMIT2(0x1e60) EMIT2(0x1e62)
		    EMIT2(0x1e64) EMIT2(0x1e66) EMIT2(0x1e68) EMIT2(0x2c7e)
		    EMIT2(0xa7a8)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'T': ସ୍ଥିତି 0x162: ସ୍ଥିତି 0x164: ସ୍ଥିତି 0x166: ସ୍ଥିତି 0x1ac:
	    ସ୍ଥିତି 0x1ae: ସ୍ଥିତି 0x21a: ସ୍ଥିତି 0x23e: ସ୍ଥିତି 0x1e6a: ସ୍ଥିତି 0x1e6c:
	    ସ୍ଥିତି 0x1e6e: ସ୍ଥିତି 0x1e70:
		    EMIT2('T') EMIT2(0x162) EMIT2(0x164) EMIT2(0x166)
		    EMIT2(0x1ac) EMIT2(0x1ae) EMIT2(0x23e) EMIT2(0x21a)
		    EMIT2(0x1e6a) EMIT2(0x1e6c) EMIT2(0x1e6e) EMIT2(0x1e70)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'U': ସ୍ଥିତି U_grave: ସ୍ଥିତି U_acute: ସ୍ଥିତି U_diaeresis:
	    ସ୍ଥିତି U_circumflex: ସ୍ଥିତି 0x168: ସ୍ଥିତି 0x16a: ସ୍ଥିତି 0x16c:
	    ସ୍ଥିତି 0x16e: ସ୍ଥିତି 0x170: ସ୍ଥିତି 0x172: ସ୍ଥିତି 0x1af:
	    ସ୍ଥିତି 0x1d3: ସ୍ଥିତି 0x1d5: ସ୍ଥିତି 0x1d7: ସ୍ଥିତି 0x1d9:
	    ସ୍ଥିତି 0x1db: ସ୍ଥିତି 0x214: ସ୍ଥିତି 0x216: ସ୍ଥିତି 0x244:
	    ସ୍ଥିତି 0x1e72: ସ୍ଥିତି 0x1e74: ସ୍ଥିତି 0x1e76: ସ୍ଥିତି 0x1e78:
	    ସ୍ଥିତି 0x1e7a: ସ୍ଥିତି 0x1ee4: ସ୍ଥିତି 0x1ee6: ସ୍ଥିତି 0x1ee8:
	    ସ୍ଥିତି 0x1eea: ସ୍ଥିତି 0x1eec: ସ୍ଥିତି 0x1eee: ସ୍ଥିତି 0x1ef0:
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
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'V': ସ୍ଥିତି 0x1b2: ସ୍ଥିତି 0x1e7c: ସ୍ଥିତି 0x1e7e:
		    EMIT2('V') EMIT2(0x1b2) EMIT2(0x1e7c) EMIT2(0x1e7e)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'W': ସ୍ଥିତି 0x174: ସ୍ଥିତି 0x1e80: ସ୍ଥିତି 0x1e82: ସ୍ଥିତି 0x1e84:
	    ସ୍ଥିତି 0x1e86: ସ୍ଥିତି 0x1e88:
		    EMIT2('W') EMIT2(0x174) EMIT2(0x1e80) EMIT2(0x1e82)
		    EMIT2(0x1e84) EMIT2(0x1e86) EMIT2(0x1e88)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'X': ସ୍ଥିତି 0x1e8a: ସ୍ଥିତି 0x1e8c:
		    EMIT2('X') EMIT2(0x1e8a) EMIT2(0x1e8c)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'Y': ସ୍ଥିତି Y_acute: ସ୍ଥିତି 0x176: ସ୍ଥିତି 0x178:
	    ସ୍ଥିତି 0x1b3: ସ୍ଥିତି 0x232: ସ୍ଥିତି 0x24e: ସ୍ଥିତି 0x1e8e:
	    ସ୍ଥିତି 0x1ef2: ସ୍ଥିତି 0x1ef4: ସ୍ଥିତି 0x1ef6: ସ୍ଥିତି 0x1ef8:
		    EMIT2('Y') EMIT2(Y_acute)
		    EMIT2(0x176) EMIT2(0x178) EMIT2(0x1b3)
		    EMIT2(0x232) EMIT2(0x24e) EMIT2(0x1e8e)
		    EMIT2(0x1ef2) EMIT2(0x1ef4) EMIT2(0x1ef6)
		    EMIT2(0x1ef8)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'Z': ସ୍ଥିତି 0x179: ସ୍ଥିତି 0x17b: ସ୍ଥିତି 0x17d:
	    ସ୍ଥିତି 0x1b5: ସ୍ଥିତି 0x1e90: ସ୍ଥିତି 0x1e92: ସ୍ଥିତି 0x1e94:
	    ସ୍ଥିତି 0x2c6b:
		    EMIT2('Z') EMIT2(0x179) EMIT2(0x17b) EMIT2(0x17d)
		    EMIT2(0x1b5) EMIT2(0x1e90) EMIT2(0x1e92)
		    EMIT2(0x1e94) EMIT2(0x2c6b)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'a': ସ୍ଥିତି a_grave: ସ୍ଥିତି a_acute: ସ୍ଥିତି a_circumflex:
	    ସ୍ଥିତି a_virguilla: ସ୍ଥିତି a_diaeresis: ସ୍ଥିତି a_ring:
	    ସ୍ଥିତି 0x101: ସ୍ଥିତି 0x103: ସ୍ଥିତି 0x105: ସ୍ଥିତି 0x1ce:
	    ସ୍ଥିତି 0x1df: ସ୍ଥିତି 0x1e1: ସ୍ଥିତି 0x1fb: ସ୍ଥିତି 0x201:
	    ସ୍ଥିତି 0x203: ସ୍ଥିତି 0x227: ସ୍ଥିତି 0x1d8f: ସ୍ଥିତି 0x1e01:
	    ସ୍ଥିତି 0x1e9a: ସ୍ଥିତି 0x1ea1: ସ୍ଥିତି 0x1ea3: ସ୍ଥିତି 0x1ea5:
	    ସ୍ଥିତି 0x1ea7: ସ୍ଥିତି 0x1ea9: ସ୍ଥିତି 0x1eab: ସ୍ଥିତି 0x1ead:
	    ସ୍ଥିତି 0x1eaf: ସ୍ଥିତି 0x1eb1: ସ୍ଥିତି 0x1eb3: ସ୍ଥିତି 0x1eb5:
	    ସ୍ଥିତି 0x1eb7: ସ୍ଥିତି 0x2c65:
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
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'b': ସ୍ଥିତି 0x180: ସ୍ଥିତି 0x253: ସ୍ଥିତି 0x1d6c: ସ୍ଥିତି 0x1d80:
	    ସ୍ଥିତି 0x1e03: ସ୍ଥିତି 0x1e05: ସ୍ଥିତି 0x1e07:
		    EMIT2('b') EMIT2(0x180) EMIT2(0x253) EMIT2(0x1d6c)
		    EMIT2(0x1d80) EMIT2(0x1e03) EMIT2(0x1e05) EMIT2(0x1e07)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'c': ସ୍ଥିତି c_cedilla: ସ୍ଥିତି 0x107: ସ୍ଥିତି 0x109: ସ୍ଥିତି 0x10b:
	    ସ୍ଥିତି 0x10d: ସ୍ଥିତି 0x188: ସ୍ଥିତି 0x23c: ସ୍ଥିତି 0x1e09: ସ୍ଥିତି 0xa793:
	    ସ୍ଥିତି 0xa794:
		    EMIT2('c') EMIT2(c_cedilla)
		    EMIT2(0x107) EMIT2(0x109) EMIT2(0x10b)
		    EMIT2(0x10d) EMIT2(0x188) EMIT2(0x23c)
		    EMIT2(0x1e09) EMIT2(0xa793) EMIT2(0xa794)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'd': ସ୍ଥିତି 0x10f: ସ୍ଥିତି 0x111: ସ୍ଥିତି 0x257: ସ୍ଥିତି 0x1d6d:
	    ସ୍ଥିତି 0x1d81: ସ୍ଥିତି 0x1d91: ସ୍ଥିତି 0x1e0b: ସ୍ଥିତି 0x1e0d: ସ୍ଥିତି 0x1e0f:
	    ସ୍ଥିତି 0x1e11: ସ୍ଥିତି 0x1e13:
		    EMIT2('d') EMIT2(0x10f) EMIT2(0x111)
		    EMIT2(0x257) EMIT2(0x1d6d) EMIT2(0x1d81)
		    EMIT2(0x1d91) EMIT2(0x1e0b) EMIT2(0x1e0d)
		    EMIT2(0x1e0f) EMIT2(0x1e11) EMIT2(0x1e13)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'e': ସ୍ଥିତି e_grave: ସ୍ଥିତି e_acute: ସ୍ଥିତି e_circumflex:
	    ସ୍ଥିତି e_diaeresis: ସ୍ଥିତି 0x113: ସ୍ଥିତି 0x115: ସ୍ଥିତି 0x117:
	    ସ୍ଥିତି 0x119: ସ୍ଥିତି 0x11b: ସ୍ଥିତି 0x205: ସ୍ଥିତି 0x207:
	    ସ୍ଥିତି 0x229: ସ୍ଥିତି 0x247: ସ୍ଥିତି 0x1d92: ସ୍ଥିତି 0x1e15:
	    ସ୍ଥିତି 0x1e17: ସ୍ଥିତି 0x1e19: ସ୍ଥିତି 0x1e1b: ସ୍ଥିତି 0x1e1d:
	    ସ୍ଥିତି 0x1eb9: ସ୍ଥିତି 0x1ebb: ସ୍ଥିତି 0x1ebd: ସ୍ଥିତି 0x1ebf:
	    ସ୍ଥିତି 0x1ec1: ସ୍ଥିତି 0x1ec3: ସ୍ଥିତି 0x1ec5: ସ୍ଥିତି 0x1ec7:
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
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'f': ସ୍ଥିତି 0x192: ସ୍ଥିତି 0x1d6e: ସ୍ଥିତି 0x1d82:
	    ସ୍ଥିତି 0x1e1f: ସ୍ଥିତି 0xa799:
		    EMIT2('f') EMIT2(0x192) EMIT2(0x1d6e) EMIT2(0x1d82)
		    EMIT2(0x1e1f) EMIT2(0xa799)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'g': ସ୍ଥିତି 0x11d: ସ୍ଥିତି 0x11f: ସ୍ଥିତି 0x121: ସ୍ଥିତି 0x123:
	    ସ୍ଥିତି 0x1e5: ସ୍ଥିତି 0x1e7: ସ୍ଥିତି 0x1f5: ସ୍ଥିତି 0x260: ସ୍ଥିତି 0x1d83:
	    ସ୍ଥିତି 0x1e21: ସ୍ଥିତି 0xa7a1:
		    EMIT2('g') EMIT2(0x11d) EMIT2(0x11f) EMIT2(0x121)
		    EMIT2(0x123) EMIT2(0x1e5) EMIT2(0x1e7)
		    EMIT2(0x1f5) EMIT2(0x260) EMIT2(0x1d83)
		    EMIT2(0x1e21) EMIT2(0xa7a1)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'h': ସ୍ଥିତି 0x125: ସ୍ଥିତି 0x127: ସ୍ଥିତି 0x21f: ସ୍ଥିତି 0x1e23:
	    ସ୍ଥିତି 0x1e25: ସ୍ଥିତି 0x1e27: ସ୍ଥିତି 0x1e29: ସ୍ଥିତି 0x1e2b:
	    ସ୍ଥିତି 0x1e96: ସ୍ଥିତି 0x2c68: ସ୍ଥିତି 0xa795:
		    EMIT2('h') EMIT2(0x125) EMIT2(0x127) EMIT2(0x21f)
		    EMIT2(0x1e23) EMIT2(0x1e25) EMIT2(0x1e27)
		    EMIT2(0x1e29) EMIT2(0x1e2b) EMIT2(0x1e96)
		    EMIT2(0x2c68) EMIT2(0xa795)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'i': ସ୍ଥିତି i_grave: ସ୍ଥିତି i_acute: ସ୍ଥିତି i_circumflex:
	    ସ୍ଥିତି i_diaeresis: ସ୍ଥିତି 0x129: ସ୍ଥିତି 0x12b: ସ୍ଥିତି 0x12d:
	    ସ୍ଥିତି 0x12f: ସ୍ଥିତି 0x1d0: ସ୍ଥିତି 0x209: ସ୍ଥିତି 0x20b:
	    ସ୍ଥିତି 0x268: ସ୍ଥିତି 0x1d96: ସ୍ଥିତି 0x1e2d: ସ୍ଥିତି 0x1e2f:
	    ସ୍ଥିତି 0x1ec9: ସ୍ଥିତି 0x1ecb:
		    EMIT2('i') EMIT2(i_grave) EMIT2(i_acute)
		    EMIT2(i_circumflex) EMIT2(i_diaeresis)
		    EMIT2(0x129) EMIT2(0x12b) EMIT2(0x12d)
		    EMIT2(0x12f) EMIT2(0x1d0) EMIT2(0x209)
		    EMIT2(0x20b) EMIT2(0x268) EMIT2(0x1d96)
		    EMIT2(0x1e2d) EMIT2(0x1e2f) EMIT2(0x1ec9)
		    EMIT2(0x1ecb)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'j': ସ୍ଥିତି 0x135: ସ୍ଥିତି 0x1f0: ସ୍ଥିତି 0x249:
		    EMIT2('j') EMIT2(0x135) EMIT2(0x1f0) EMIT2(0x249)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'k': ସ୍ଥିତି 0x137: ସ୍ଥିତି 0x199: ସ୍ଥିତି 0x1e9: ସ୍ଥିତି 0x1d84:
	    ସ୍ଥିତି 0x1e31: ସ୍ଥିତି 0x1e33: ସ୍ଥିତି 0x1e35: ସ୍ଥିତି 0x2c6a: ସ୍ଥିତି 0xa741:
		    EMIT2('k') EMIT2(0x137) EMIT2(0x199) EMIT2(0x1e9)
		    EMIT2(0x1d84) EMIT2(0x1e31) EMIT2(0x1e33)
		    EMIT2(0x1e35) EMIT2(0x2c6a) EMIT2(0xa741)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'l': ସ୍ଥିତି 0x13a: ସ୍ଥିତି 0x13c: ସ୍ଥିତି 0x13e: ସ୍ଥିତି 0x140:
	    ସ୍ଥିତି 0x142: ସ୍ଥିତି 0x19a: ସ୍ଥିତି 0x1e37: ସ୍ଥିତି 0x1e39: ସ୍ଥିତି 0x1e3b:
	    ସ୍ଥିତି 0x1e3d: ସ୍ଥିତି 0x2c61:
		    EMIT2('l') EMIT2(0x13a) EMIT2(0x13c)
		    EMIT2(0x13e) EMIT2(0x140) EMIT2(0x142)
		    EMIT2(0x19a) EMIT2(0x1e37) EMIT2(0x1e39)
		    EMIT2(0x1e3b) EMIT2(0x1e3d) EMIT2(0x2c61)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'm': ସ୍ଥିତି 0x1d6f: ସ୍ଥିତି 0x1e3f: ସ୍ଥିତି 0x1e41: ସ୍ଥିତି 0x1e43:
		    EMIT2('m') EMIT2(0x1d6f) EMIT2(0x1e3f)
		    EMIT2(0x1e41) EMIT2(0x1e43)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'n': ସ୍ଥିତି n_virguilla: ସ୍ଥିତି 0x144: ସ୍ଥିତି 0x146: ସ୍ଥିତି 0x148:
	    ସ୍ଥିତି 0x149: ସ୍ଥିତି 0x1f9: ସ୍ଥିତି 0x1d70: ସ୍ଥିତି 0x1d87: ସ୍ଥିତି 0x1e45:
	    ସ୍ଥିତି 0x1e47: ସ୍ଥିତି 0x1e49: ସ୍ଥିତି 0x1e4b: ସ୍ଥିତି 0xa7a5:
		    EMIT2('n') EMIT2(n_virguilla)
		    EMIT2(0x144) EMIT2(0x146) EMIT2(0x148)
		    EMIT2(0x149) EMIT2(0x1f9) EMIT2(0x1d70)
		    EMIT2(0x1d87) EMIT2(0x1e45) EMIT2(0x1e47)
		    EMIT2(0x1e49) EMIT2(0x1e4b) EMIT2(0xa7a5)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'o': ସ୍ଥିତି o_grave: ସ୍ଥିତି o_acute: ସ୍ଥିତି o_circumflex:
	    ସ୍ଥିତି o_virguilla: ସ୍ଥିତି o_diaeresis: ସ୍ଥିତି o_slash:
	    ସ୍ଥିତି 0x14d: ସ୍ଥିତି 0x14f: ସ୍ଥିତି 0x151: ସ୍ଥିତି 0x1a1:
	    ସ୍ଥିତି 0x1d2: ସ୍ଥିତି 0x1eb: ସ୍ଥିତି 0x1ed: ସ୍ଥିତି 0x1ff:
	    ସ୍ଥିତି 0x20d: ସ୍ଥିତି 0x20f: ସ୍ଥିତି 0x22b: ସ୍ଥିତି 0x22d:
	    ସ୍ଥିତି 0x22f: ସ୍ଥିତି 0x231: ସ୍ଥିତି 0x275: ସ୍ଥିତି 0x1e4d:
	    ସ୍ଥିତି 0x1e4f: ସ୍ଥିତି 0x1e51: ସ୍ଥିତି 0x1e53: ସ୍ଥିତି 0x1ecd:
	    ସ୍ଥିତି 0x1ecf: ସ୍ଥିତି 0x1ed1: ସ୍ଥିତି 0x1ed3: ସ୍ଥିତି 0x1ed5:
	    ସ୍ଥିତି 0x1ed7: ସ୍ଥିତି 0x1ed9: ସ୍ଥିତି 0x1edb: ସ୍ଥିତି 0x1edd:
	    ସ୍ଥିତି 0x1edf: ସ୍ଥିତି 0x1ee1: ସ୍ଥିତି 0x1ee3:
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
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'p': ସ୍ଥିତି 0x1a5: ସ୍ଥିତି 0x1d71: ସ୍ଥିତି 0x1d7d: ସ୍ଥିତି 0x1d88:
	    ସ୍ଥିତି 0x1e55: ସ୍ଥିତି 0x1e57:
		    EMIT2('p') EMIT2(0x1a5) EMIT2(0x1d71) EMIT2(0x1d7d)
		    EMIT2(0x1d88) EMIT2(0x1e55) EMIT2(0x1e57)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'q': ସ୍ଥିତି 0x24b: ସ୍ଥିତି 0x2a0:
		    EMIT2('q') EMIT2(0x24b) EMIT2(0x2a0)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'r': ସ୍ଥିତି 0x155: ସ୍ଥିତି 0x157: ସ୍ଥିତି 0x159: ସ୍ଥିତି 0x211:
	    ସ୍ଥିତି 0x213: ସ୍ଥିତି 0x24d: ସ୍ଥିତି 0x27d: ସ୍ଥିତି 0x1d72: ସ୍ଥିତି 0x1d73:
	    ସ୍ଥିତି 0x1d89: ସ୍ଥିତି 0x1e59: ସ୍ଥିତି 0x1e5b: ସ୍ଥିତି 0x1e5d: ସ୍ଥିତି 0x1e5f:
	    ସ୍ଥିତି 0xa7a7:
		    EMIT2('r') EMIT2(0x155) EMIT2(0x157) EMIT2(0x159)
		    EMIT2(0x211) EMIT2(0x213) EMIT2(0x24d) EMIT2(0x27d)
		    EMIT2(0x1d72) EMIT2(0x1d73) EMIT2(0x1d89) EMIT2(0x1e59)
		    EMIT2(0x1e5b) EMIT2(0x1e5d) EMIT2(0x1e5f) EMIT2(0xa7a7)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 's': ସ୍ଥିତି 0x15b: ସ୍ଥିତି 0x15d: ସ୍ଥିତି 0x15f: ସ୍ଥିତି 0x161:
	    ସ୍ଥିତି 0x219: ସ୍ଥିତି 0x23f: ସ୍ଥିତି 0x1d74: ସ୍ଥିତି 0x1d8a: ସ୍ଥିତି 0x1e61:
	    ସ୍ଥିତି 0x1e63: ସ୍ଥିତି 0x1e65: ସ୍ଥିତି 0x1e67: ସ୍ଥିତି 0x1e69: ସ୍ଥିତି 0xa7a9:
		    EMIT2('s') EMIT2(0x15b) EMIT2(0x15d) EMIT2(0x15f)
		    EMIT2(0x161) EMIT2(0x219) EMIT2(0x23f) EMIT2(0x1d74)
		    EMIT2(0x1d8a) EMIT2(0x1e61) EMIT2(0x1e63) EMIT2(0x1e65)
		    EMIT2(0x1e67) EMIT2(0x1e69) EMIT2(0xa7a9)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 't': ସ୍ଥିତି 0x163: ସ୍ଥିତି 0x165: ସ୍ଥିତି 0x167: ସ୍ଥିତି 0x1ab:
	    ସ୍ଥିତି 0x1ad: ସ୍ଥିତି 0x21b: ସ୍ଥିତି 0x288: ସ୍ଥିତି 0x1d75: ସ୍ଥିତି 0x1e6b:
	    ସ୍ଥିତି 0x1e6d: ସ୍ଥିତି 0x1e6f: ସ୍ଥିତି 0x1e71: ସ୍ଥିତି 0x1e97: ସ୍ଥିତି 0x2c66:
		    EMIT2('t') EMIT2(0x163) EMIT2(0x165) EMIT2(0x167)
		    EMIT2(0x1ab) EMIT2(0x1ad) EMIT2(0x21b) EMIT2(0x288)
		    EMIT2(0x1d75) EMIT2(0x1e6b) EMIT2(0x1e6d) EMIT2(0x1e6f)
		    EMIT2(0x1e71) EMIT2(0x1e97) EMIT2(0x2c66)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'u': ସ୍ଥିତି u_grave: ସ୍ଥିତି u_acute: ସ୍ଥିତି u_circumflex:
	    ସ୍ଥିତି u_diaeresis: ସ୍ଥିତି 0x169: ସ୍ଥିତି 0x16b: ସ୍ଥିତି 0x16d:
	    ସ୍ଥିତି 0x16f: ସ୍ଥିତି 0x171: ସ୍ଥିତି 0x173: ସ୍ଥିତି 0x1b0: ସ୍ଥିତି 0x1d4:
	    ସ୍ଥିତି 0x1d6: ସ୍ଥିତି 0x1d8: ସ୍ଥିତି 0x1da: ସ୍ଥିତି 0x1dc: ସ୍ଥିତି 0x215:
	    ସ୍ଥିତି 0x217: ସ୍ଥିତି 0x289: ସ୍ଥିତି 0x1d7e: ସ୍ଥିତି 0x1d99: ସ୍ଥିତି 0x1e73:
	    ସ୍ଥିତି 0x1e75: ସ୍ଥିତି 0x1e77: ସ୍ଥିତି 0x1e79: ସ୍ଥିତି 0x1e7b:
	    ସ୍ଥିତି 0x1ee5: ସ୍ଥିତି 0x1ee7: ସ୍ଥିତି 0x1ee9: ସ୍ଥିତି 0x1eeb:
	    ସ୍ଥିତି 0x1eed: ସ୍ଥିତି 0x1eef: ସ୍ଥିତି 0x1ef1:
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
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'v': ସ୍ଥିତି 0x28b: ସ୍ଥିତି 0x1d8c: ସ୍ଥିତି 0x1e7d: ସ୍ଥିତି 0x1e7f:
		    EMIT2('v') EMIT2(0x28b) EMIT2(0x1d8c) EMIT2(0x1e7d)
		    EMIT2(0x1e7f)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'w': ସ୍ଥିତି 0x175: ସ୍ଥିତି 0x1e81: ସ୍ଥିତି 0x1e83: ସ୍ଥିତି 0x1e85:
	    ସ୍ଥିତି 0x1e87: ସ୍ଥିତି 0x1e89: ସ୍ଥିତି 0x1e98:
		    EMIT2('w') EMIT2(0x175) EMIT2(0x1e81) EMIT2(0x1e83)
		    EMIT2(0x1e85) EMIT2(0x1e87) EMIT2(0x1e89) EMIT2(0x1e98)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'x': ସ୍ଥିତି 0x1e8b: ସ୍ଥିତି 0x1e8d:
		    EMIT2('x') EMIT2(0x1e8b) EMIT2(0x1e8d)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'y': ସ୍ଥିତି y_acute: ସ୍ଥିତି y_diaeresis: ସ୍ଥିତି 0x177:
	    ସ୍ଥିତି 0x1b4: ସ୍ଥିତି 0x233: ସ୍ଥିତି 0x24f: ସ୍ଥିତି 0x1e8f:
	    ସ୍ଥିତି 0x1e99: ସ୍ଥିତି 0x1ef3: ସ୍ଥିତି 0x1ef5: ସ୍ଥିତି 0x1ef7:
	    ସ୍ଥିତି 0x1ef9:
		    EMIT2('y') EMIT2(y_acute) EMIT2(y_diaeresis)
		    EMIT2(0x177) EMIT2(0x1b4) EMIT2(0x233) EMIT2(0x24f)
		    EMIT2(0x1e8f) EMIT2(0x1e99) EMIT2(0x1ef3)
		    EMIT2(0x1ef5) EMIT2(0x1ef7) EMIT2(0x1ef9)
		    ଫେରନ୍ତୁ OK;

	    ସ୍ଥିତି 'z': ସ୍ଥିତି 0x17a: ସ୍ଥିତି 0x17c: ସ୍ଥିତି 0x17e: ସ୍ଥିତି 0x1b6:
	    ସ୍ଥିତି 0x1d76: ସ୍ଥିତି 0x1d8e: ସ୍ଥିତି 0x1e91: ସ୍ଥିତି 0x1e93:
	    ସ୍ଥିତି 0x1e95: ସ୍ଥିତି 0x2c6c:
		    EMIT2('z') EMIT2(0x17a) EMIT2(0x17c) EMIT2(0x17e)
		    EMIT2(0x1b6) EMIT2(0x1d76) EMIT2(0x1d8e) EMIT2(0x1e91)
		    EMIT2(0x1e93) EMIT2(0x1e95) EMIT2(0x2c6c)
		    ଫେରନ୍ତୁ OK;

	    // default: character itself
	}
    }

    EMIT2(c);
    ଫେରନ୍ତୁ OK;
#undef EMIT2
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
nfa_regatom(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		charclass;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		equiclass;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		collclass;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		got_coll_char;
    char_u	*p;
    char_u	*endp;
    char_u	*old_regparse = regparse;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		extra = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		emit_range;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		negated;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		startc = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_prev_at_start = prev_at_start;

    c = getchr();
    ବାଛ (c)
    {
	ସ୍ଥିତି NUL:
	    EMSG_RET_FAIL(_(e_nfa_regexp_end_encountered_prematurely));

	ସ୍ଥିତି Magic('^'):
	    EMIT(NFA_BOL);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି Magic('$'):
	    EMIT(NFA_EOL);
#ଯଦି defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#endif
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି Magic('<'):
	    EMIT(NFA_BOW);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି Magic('>'):
	    EMIT(NFA_EOW);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି Magic('_'):
	    c = no_Magic(getchr());
	    ଯଦି (c == NUL)
		EMSG_RET_FAIL(_(e_nfa_regexp_end_encountered_prematurely));

	    ଯଦି (c == '^')	// "\_^" is start-of-line
	    {
		EMIT(NFA_BOL);
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (c == '$')	// "\_$" is end-of-line
	    {
		EMIT(NFA_EOL);
#ଯଦି defined(FEAT_SYN_HL)
		had_eol = TRUE;
#endif
		ଭାଙ୍ଗ;
	    }

	    extra = NFA_ADD_NL;

	    // "\_[" is collection plus newline
	    ଯଦି (c == '[')
		ଯାଅ collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	/*
	 * Character classes.
	 */
	ସ୍ଥିତି Magic('.'):
	ସ୍ଥିତି Magic('i'):
	ସ୍ଥିତି Magic('I'):
	ସ୍ଥିତି Magic('k'):
	ସ୍ଥିତି Magic('K'):
	ସ୍ଥିତି Magic('f'):
	ସ୍ଥିତି Magic('F'):
	ସ୍ଥିତି Magic('p'):
	ସ୍ଥିତି Magic('P'):
	ସ୍ଥିତି Magic('s'):
	ସ୍ଥିତି Magic('S'):
	ସ୍ଥିତି Magic('d'):
	ସ୍ଥିତି Magic('D'):
	ସ୍ଥିତି Magic('x'):
	ସ୍ଥିତି Magic('X'):
	ସ୍ଥିତି Magic('o'):
	ସ୍ଥିତି Magic('O'):
	ସ୍ଥିତି Magic('w'):
	ସ୍ଥିତି Magic('W'):
	ସ୍ଥିତି Magic('h'):
	ସ୍ଥିତି Magic('H'):
	ସ୍ଥିତି Magic('a'):
	ସ୍ଥିତି Magic('A'):
	ସ୍ଥିତି Magic('l'):
	ସ୍ଥିତି Magic('L'):
	ସ୍ଥିତି Magic('u'):
	ସ୍ଥିତି Magic('U'):
	    p = vim_strchr(classchars, no_Magic(c));
	    ଯଦି (p == ଖାଲି)
	    {
		ଯଦି (extra == NFA_ADD_NL)
		{
		    semsg(_(e_nfa_regexp_invalid_character_class_nr), c);
		    rc_did_emsg = TRUE;
		    ଫେରନ୍ତୁ FAIL;
		}
		siemsg("Unknown character class char: %d", c);
		ଫେରନ୍ତୁ FAIL;
	    }

	    // When '.' is followed by a composing char ignore the dot, so that
	    // the composing char is matched here.
	    ଯଦି (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	    {
		old_regparse = regparse;
		c = getchr();
		ଯାଅ nfa_do_multibyte;
	    }
	    EMIT(nfa_classcodes[p - classchars]);
	    ଯଦି (extra == NFA_ADD_NL)
	    {
		EMIT(NFA_NEWL);
		EMIT(NFA_OR);
		regflags |= RF_HASNL;
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି Magic('n'):
	    ଯଦି (reg_string)
		// In a string "\n" matches a newline character.
		EMIT(NL);
	    ଅନ୍ୟଥା
	    {
		// In buffer text "\n" matches the end of a line.
		EMIT(NFA_NEWL);
		regflags |= RF_HASNL;
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି Magic('('):
	    ଯଦି (nfa_reg(REG_PAREN) == FAIL)
		ଫେରନ୍ତୁ FAIL;	    // cascaded error
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି Magic('|'):
	ସ୍ଥିତି Magic('&'):
	ସ୍ଥିତି Magic(')'):
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    ଫେରନ୍ତୁ FAIL;

	ସ୍ଥିତି Magic('='):
	ସ୍ଥିତି Magic('?'):
	ସ୍ଥିତି Magic('+'):
	ସ୍ଥିତି Magic('@'):
	ସ୍ଥିତି Magic('*'):
	ସ୍ଥିତି Magic('{'):
	    // these should follow an atom, not form an atom
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    ଫେରନ୍ତୁ FAIL;

	ସ୍ଥିତି Magic('~'):
	    {
		char_u	    *lp;

		// Previous substitute pattern.
		// Generated as "\%(pattern\)".
		ଯଦି (reg_prev_sub == ଖାଲି)
		{
		    emsg(_(e_no_previous_substitute_regular_expression));
		    ଫେରନ୍ତୁ FAIL;
		}
		ପାଇଁ (lp = reg_prev_sub; *lp != NUL; MB_CPTR_ADV(lp))
		{
		    EMIT(PTR2CHAR(lp));
		    ଯଦି (lp != reg_prev_sub)
			EMIT(NFA_CONCAT);
		}
		EMIT(NFA_NOPEN);
		ଭାଙ୍ଗ;
	    }

	ସ୍ଥିତି Magic('1'):
	ସ୍ଥିତି Magic('2'):
	ସ୍ଥିତି Magic('3'):
	ସ୍ଥିତି Magic('4'):
	ସ୍ଥିତି Magic('5'):
	ସ୍ଥିତି Magic('6'):
	ସ୍ଥିତି Magic('7'):
	ସ୍ଥିତି Magic('8'):
	ସ୍ଥିତି Magic('9'):
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା refnum = no_Magic(c) - '1';

		ଯଦି (!seen_endbrace(refnum + 1))
		    ଫେରନ୍ତୁ FAIL;
		EMIT(NFA_BACKREF1 + refnum);
		rex.nfa_has_backref = TRUE;
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି Magic('z'):
	    c = no_Magic(getchr());
	    ବାଛ (c)
	    {
		ସ୍ଥିତି 's':
		    EMIT(NFA_ZSTART);
		    ଯଦି (re_mult_next("\\zs") == FAIL)
			ଫେରନ୍ତୁ FAIL;
		    ଭାଙ୍ଗ;
		ସ୍ଥିତି 'e':
		    EMIT(NFA_ZEND);
		    rex.nfa_has_zend = TRUE;
		    ଯଦି (re_mult_next("\\ze") == FAIL)
			ଫେରନ୍ତୁ FAIL;
		    ଭାଙ୍ଗ;
#ifdef FEAT_SYN_HL
		ସ୍ଥିତି '1':
		ସ୍ଥିତି '2':
		ସ୍ଥିତି '3':
		ସ୍ଥିତି '4':
		ସ୍ଥିତି '5':
		ସ୍ଥିତି '6':
		ସ୍ଥିତି '7':
		ସ୍ଥିତି '8':
		ସ୍ଥିତି '9':
		    // \z1...\z9
		    ଯଦି ((reg_do_extmatch & REX_USE) == 0)
			EMSG_RET_FAIL(_(e_z1_z9_not_allowed_here));
		    EMIT(NFA_ZREF1 + (no_Magic(c) - '1'));
		    // No need to set rex.nfa_has_backref, the sub-matches don't
		    // change when \z1 .. \z9 matches or not.
		    re_has_z = REX_USE;
		    ଭାଙ୍ଗ;
		ସ୍ଥିତି '(':
		    // \z(
		    ଯଦି ((reg_do_extmatch & REX_SET) == 0)
			EMSG_RET_FAIL(_(e_z_not_allowed_here));
		    ଯଦି (nfa_reg(REG_ZPAREN) == FAIL)
			ଫେରନ୍ତୁ FAIL;	    // cascaded error
		    re_has_z = REX_SET;
		    ଭାଙ୍ଗ;
#endif
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		    semsg(_(e_nfa_regexp_unknown_operator_z_chr), no_Magic(c));
		    ଫେରନ୍ତୁ FAIL;
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି Magic('%'):
	    c = no_Magic(getchr());
	    ବାଛ (c)
	    {
		// () without a back reference
		ସ୍ଥିତି '(':
		    ଯଦି (nfa_reg(REG_NPAREN) == FAIL)
			ଫେରନ୍ତୁ FAIL;
		    EMIT(NFA_NOPEN);
		    ଭାଙ୍ଗ;

		ସ୍ଥିତି 'd':   // %d123 decimal
		ସ୍ଥିତି 'o':   // %o123 octal
		ସ୍ଥିତି 'x':   // %xab hex 2
		ସ୍ଥିତି 'u':   // %uabcd hex 4
		ସ୍ଥିତି 'U':   // %U1234abcd hex 8
		    {
			vimlong_T nr;

			ବାଛ (c)
			{
			    ସ୍ଥିତି 'd': nr = getdecchrs(); ଭାଙ୍ଗ;
			    ସ୍ଥିତି 'o': nr = getoctchrs(); ଭାଙ୍ଗ;
			    ସ୍ଥିତି 'x': nr = gethexchrs(2); ଭାଙ୍ଗ;
			    ସ୍ଥିତି 'u': nr = gethexchrs(4); ଭାଙ୍ଗ;
			    ସ୍ଥିତି 'U': nr = gethexchrs(8); ଭାଙ୍ଗ;
			    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  nr = -1; ଭାଙ୍ଗ;
			}

			ଯଦି (nr < 0 || nr > INT_MAX)
			    EMSG2_RET_FAIL(_(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			// A NUL is stored in the text as NL
			// TODO: what if a composing character follows?
			EMIT(nr == 0 ? 0x0a : (long)nr);
		    }
		    ଭାଙ୍ଗ;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		ସ୍ଥିତି '^':
		    EMIT(NFA_BOF);
		    ଭାଙ୍ଗ;

		ସ୍ଥିତି '$':
		    EMIT(NFA_EOF);
		    ଭାଙ୍ଗ;

		ସ୍ଥିତି '#':
		    ଯଦି (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_start_of_pattern),
								  regparse[1]);
			ଫେରନ୍ତୁ FAIL;
		    }
		    EMIT(NFA_CURSOR);
		    ଭାଙ୍ଗ;

		ସ୍ଥିତି 'V':
		    EMIT(NFA_VISUAL);
		    ଭାଙ୍ଗ;

		ସ୍ଥିତି 'C':
		    EMIT(NFA_ANY_COMPOSING);
		    ଭାଙ୍ଗ;

		ସ୍ଥିତି '[':
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    n;

			// \%[abc]
			ପାଇଁ (n = 0; (c = peekchr()) != ']'; ++n)
			{
			    ଯଦି (c == NUL)
				EMSG2_RET_FAIL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
			    // recursive call!
			    ଯଦି (nfa_regatom() == FAIL)
				ଫେରନ୍ତୁ FAIL;
			}
			getchr();  // get the ]
			ଯଦି (n == 0)
			    EMSG2_RET_FAIL(_(e_empty_str_brackets),
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
			ଭାଙ୍ଗ;
		    }

		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		    {
			long_u	n = 0;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cmp = c;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cur = FALSE;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା	got_digit = FALSE;

			ଯଦି (c == '<' || c == '>')
			    c = getchr();
			ଯଦି (no_Magic(c) == '.')
			{
			    cur = TRUE;
			    c = getchr();
			}
			ଯେତେବେଳେ (VIM_ISDIGIT(c))
			{
			    long_u tmp;

			    ଯଦି (cur)
			    {
				semsg(_(e_regexp_ସଂଖ୍ୟା_after_dot_pos_ସନ୍ଧାନ_chr),
								  no_Magic(c));
				ଫେରନ୍ତୁ FAIL;
			    }
			    tmp = n * 10 + (c - '0');

			    ଯଦି (tmp < n)
			    {
				// overflow.
				emsg(_(e_percent_ମୂଲ୍ୟ_too_large));
				ଫେରନ୍ତୁ FAIL;
			    }
			    n = tmp;
			    c = getchr();
			    got_digit = TRUE;
			}
			ଯଦି (c == 'l' || c == 'c' || c == 'v')
			{
			    long_u limit = INT_MAX;

			    ଯଦି (!cur && !got_digit)
			    {
				semsg(_(e_nfa_regexp_missing_ମୂଲ୍ୟ_in_chr),
								  no_Magic(c));
				ଫେରନ୍ତୁ FAIL;
			    }
			    ଯଦି (c == 'l')
			    {
				ଯଦି (cur)
				    n = curwin->w_cursor.lnum;
				// \%{n}l  \%{n}<l  \%{n}>l
				EMIT(cmp == '<' ? NFA_LNUM_LT :
				     cmp == '>' ? NFA_LNUM_GT : NFA_LNUM);
				ଯଦି (save_prev_at_start)
				    at_start = TRUE;
			    }
			    ଅନ୍ୟଥା ଯଦି (c == 'c')
			    {
				ଯଦି (cur)
				{
				    n = curwin->w_cursor.col;
				    n++;
				}
				// \%{n}c  \%{n}<c  \%{n}>c
				EMIT(cmp == '<' ? NFA_COL_LT :
				     cmp == '>' ? NFA_COL_GT : NFA_COL);
			    }
			    ଅନ୍ୟଥା
			    {
				ଯଦି (cur)
				{
				    colnr_T vcol = 0;

				    getvvcol(curwin, &curwin->w_cursor,
							 ଖାଲି, ଖାଲି, &vcol, 0);
				    n = ++vcol;
				}
				// \%{n}v  \%{n}<v  \%{n}>v
				EMIT(cmp == '<' ? NFA_VCOL_LT :
				     cmp == '>' ? NFA_VCOL_GT : NFA_VCOL);
				limit = INT_MAX / MB_MAXBYTES;
			    }
			    ଯଦି (n >= limit)
			    {
				emsg(_(e_percent_ମୂଲ୍ୟ_too_large));
				ଫେରନ୍ତୁ FAIL;
			    }
			    EMIT((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n);
			    ଭାଙ୍ଗ;
			}
			ଅନ୍ୟଥା ଯଦି (no_Magic(c) == '\'' && n == 0)
			{
			    // \%'m  \%<'m  \%>'m
			    EMIT(cmp == '<' ? NFA_MARK_LT :
				 cmp == '>' ? NFA_MARK_GT : NFA_MARK);
			    EMIT(getchr());
			    ଭାଙ୍ଗ;
			}
		    }
		    semsg(_(e_nfa_regexp_unknown_operator_percent_chr),
								  no_Magic(c));
		    ଫେରନ୍ତୁ FAIL;
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି Magic('['):
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
	    ଯଦି (*endp == ']')
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା plen;
		bool range_endpoint;
		/*
		 * Try to reverse engineer character classes. For example,
		 * recognize that [0-9] stands for \d and [A-Za-z_] for \h,
		 * and perform the necessary substitutions in the NFA.
		 */
		result = nfa_recognize_char_class(regparse, endp,
							 extra == NFA_ADD_NL);
		ଯଦି (result != FAIL)
		{
		    ଯଦି (result >= NFA_FIRST_NL && result <= NFA_LAST_NL)
		    {
			EMIT(result - NFA_ADD_NL);
			EMIT(NFA_NEWL);
			EMIT(NFA_OR);
		    }
		    ଅନ୍ୟଥା
			EMIT(result);
		    regparse = endp;
		    MB_PTR_ADV(regparse);
		    ଫେରନ୍ତୁ OK;
		}
		/*
		 * Failed to recognize a character class. Use the simple
		 * version that turns [abc] into 'a' OR 'b' OR 'c'
		 */
		startc = -1;
		negated = FALSE;
		ଯଦି (*regparse == '^')			// negated range
		{
		    negated = TRUE;
		    MB_PTR_ADV(regparse);
		    EMIT(NFA_START_NEG_COLL);
		}
		ଅନ୍ୟଥା
		    EMIT(NFA_START_COLL);
		ଯଦି (*regparse == '-')
		{
		    startc = '-';
		    EMIT(startc);
		    EMIT(NFA_CONCAT);
		    MB_PTR_ADV(regparse);
		}
		// Emit the OR branches for each character in the []
		emit_range = FALSE;
		ଯେତେବେଳେ (regparse < endp)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    oldstartc = startc;
		    range_endpoint = false;

		    startc = -1;
		    got_coll_char = FALSE;
		    ଯଦି (*regparse == '[')
		    {
			// Check for [: :], [= =], [. .]
			equiclass = collclass = 0;
			charclass = get_char_class(&regparse);
			ଯଦି (charclass == CLASS_NONE)
			{
			    equiclass = get_equi_class(&regparse);
			    ଯଦି (equiclass == 0)
				collclass = get_coll_element(&regparse);
			}

			// Character class like [:alpha:]
			ଯଦି (charclass != CLASS_NONE)
			{
			    ବାଛ (charclass)
			    {
				ସ୍ଥିତି CLASS_ALNUM:
				    EMIT(NFA_CLASS_ALNUM);
				    ଭାଙ୍ଗ;
				ସ୍ଥିତି CLASS_ALPHA:
				    EMIT(NFA_CLASS_ALPHA);
				    ଭାଙ୍ଗ;
				ସ୍ଥିତି CLASS_BLANK:
				    EMIT(NFA_CLASS_BLANK);
				    ଭାଙ୍ଗ;
				ସ୍ଥିତି CLASS_CNTRL:
				    EMIT(NFA_CLASS_CNTRL);
				    ଭାଙ୍ଗ;
				ସ୍ଥିତି CLASS_DIGIT:
				    EMIT(NFA_CLASS_DIGIT);
				    ଭାଙ୍ଗ;
				ସ୍ଥିତି CLASS_GRAPH:
				    EMIT(NFA_CLASS_GRAPH);
				    ଭାଙ୍ଗ;
				ସ୍ଥିତି CLASS_LOWER:
				    wants_nfa = TRUE;
				    EMIT(NFA_CLASS_LOWER);
				    ଭାଙ୍ଗ;
				ସ୍ଥିତି CLASS_PRINT:
				    EMIT(NFA_CLASS_PRINT);
				    ଭାଙ୍ଗ;
				ସ୍ଥିତି CLASS_PUNCT:
				    EMIT(NFA_CLASS_PUNCT);
				    ଭାଙ୍ଗ;
				ସ୍ଥିତି CLASS_SPACE:
				    EMIT(NFA_CLASS_SPACE);
				    ଭାଙ୍ଗ;
				ସ୍ଥିତି CLASS_UPPER:
				    wants_nfa = TRUE;
				    EMIT(NFA_CLASS_UPPER);
				    ଭାଙ୍ଗ;
				ସ୍ଥିତି CLASS_XDIGIT:
				    EMIT(NFA_CLASS_XDIGIT);
				    ଭାଙ୍ଗ;
				ସ୍ଥିତି CLASS_TAB:
				    EMIT(NFA_CLASS_TAB);
				    ଭାଙ୍ଗ;
				ସ୍ଥିତି CLASS_RETURN:
				    EMIT(NFA_CLASS_RETURN);
				    ଭାଙ୍ଗ;
				ସ୍ଥିତି CLASS_BACKSPACE:
				    EMIT(NFA_CLASS_BACKSPACE);
				    ଭାଙ୍ଗ;
				ସ୍ଥିତି CLASS_ESCAPE:
				    EMIT(NFA_CLASS_ESCAPE);
				    ଭାଙ୍ଗ;
				ସ୍ଥିତି CLASS_IDENT:
				    EMIT(NFA_CLASS_IDENT);
				    ଭାଙ୍ଗ;
				ସ୍ଥିତି CLASS_KEYWORD:
				    EMIT(NFA_CLASS_KEYWORD);
				    ଭାଙ୍ଗ;
				ସ୍ଥିତି CLASS_FNAME:
				    EMIT(NFA_CLASS_FNAME);
				    ଭାଙ୍ଗ;
			    }
			    EMIT(NFA_CONCAT);
			    ଜାରି;
			}
			// Try equivalence class [=a=] and the like
			ଯଦି (equiclass != 0)
			{
			    result = nfa_emit_equi_class(equiclass);
			    ଯଦି (result == FAIL)
			    {
				// should never happen
				EMSG_RET_FAIL(_(e_error_building_nfa_with_equivalence_class));
			    }
			    ଜାରି;
			}
			// Try collating class like [. .]
			ଯଦି (collclass != 0)
			{
			    startc = collclass;	 // allow [.a.]-x as a range
			    // Will emit the proper atom at the end of the
			    // while loop.
			}
		    }
		    // Try a range like 'a-x' or '\t-z'. Also allows '-' as a
		    // start character.
		    ଯଦି (*regparse == '-' && oldstartc != -1)
		    {
			emit_range = TRUE;
			startc = oldstartc;
			MB_PTR_ADV(regparse);
			ଜାରି;	    // reading the end of the range
		    }

		    // Now handle simple and escaped characters.
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    ଯଦି (*regparse == '\\'
			    && !reg_cpo_bsl
			    && regparse + 1 <= endp
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != ଖାଲି
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR, regparse[1])
								      != ଖାଲି)
			    )
			)
		    {
			MB_PTR_ADV(regparse);

			ଯଦି (*regparse == 'n')
			    startc = (reg_string || emit_range
					|| regparse[1] == '-') ? NL : NFA_NEWL;
			ଅନ୍ୟଥା ଯଦି (*regparse == 'd'
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
				ଯଦି (startc == INT_MAX)
				    EMSG_RET_FAIL(_(e_unicode_val_too_large));
				got_coll_char = TRUE;
				MB_PTR_BACK(old_regparse, regparse);
			    }
			    ଅନ୍ୟଥା
			    {
				// \r,\t,\e,\b
				startc = backslash_trans(*regparse);
			    }
		    }

		    // Normal printable char
		    ଯଦି (startc == -1)
			startc = PTR2CHAR(regparse);

		    // Previous char was '-', so this char is end of range.
		    ଯଦି (emit_range)
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା	endc = startc;
			range_endpoint = true;

			startc = oldstartc;
			ଯଦି (startc > endc)
			    EMSG_RET_FAIL(_(e_reverse_range_in_character_class));

			ଯଦି (endc > startc + 2)
			{
			    // Emit a range instead of the sequence of
			    // individual characters.
			    ଯଦି (startc == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    ଅନ୍ୟଥା
				--post_ptr; // remove NFA_CONCAT
			    EMIT(endc);
			    EMIT(NFA_RANGE);
			    EMIT(NFA_CONCAT);
			}
			ଅନ୍ୟଥା ଯଦି (has_mbyte && ((*mb_char2len)(startc) > 1
				    || (*mb_char2len)(endc) > 1))
			{
			    // Emit the characters in the range.
			    // "startc" was already emitted, so skip it.
			    //
			    ପାଇଁ (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			ଅନ୍ୟଥା
			{
			    // Emit the range. "startc" was already emitted, so
			    // skip it.
			    ପାଇଁ (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			emit_range = FALSE;
			startc = -1;
		    }
		    ଅନ୍ୟଥା
		    {
			// This char (startc) is not part of a range. Just
			// emit it.
			// Normally, simply emit startc. But if we get char
			// code=0 from a collating char, then replace it with
			// 0x0a.
			// This is needed to completely mimic the behaviour of
			// the backtracking engine.
			ଯଦି (startc == NFA_NEWL)
			{
			    // Line break can't be matched as part of the
			    // collection, add an OR below. But not for negated
			    // range.
			    ଯଦି (!negated)
				extra = NFA_ADD_NL;
			}
			ଅନ୍ୟଥା
			{
			    ଯଦି (got_coll_char == TRUE && startc == 0)
			    {
				EMIT(0x0a);
				EMIT(NFA_CONCAT);
			    }
			    ଅନ୍ୟଥା
			    {
				EMIT(startc);
				ଯଦି (!(enc_utf8 && (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse)))))
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
		    ଯଦି (!range_endpoint && enc_utf8 &&
			    (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse))))
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = utf_ptr2len(regparse);

			c = utf_ptr2char(regparse + i);

			// Add composing characters
			ପାଇଁ (;;)
			{
			    ଯଦି (c == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    ଅନ୍ୟଥା
				EMIT(c);
			    EMIT(NFA_CONCAT);
			    ଯଦି ((i += utf_char2len(c)) >= plen)
				ଭାଙ୍ଗ;
			    c = utf_ptr2char(regparse + i);
			}
			EMIT(NFA_COMPOSING);
			EMIT(NFA_CONCAT);
		    }
		    MB_PTR_ADV(regparse);
		} // while (p < endp)

		MB_PTR_BACK(old_regparse, regparse);
		ଯଦି (*regparse == '-')	    // if last, '-' is just a char
		{
		    EMIT('-');
		    EMIT(NFA_CONCAT);
		}

		// skip the trailing ]
		regparse = endp;
		MB_PTR_ADV(regparse);

		// Mark end of the collection.
		ଯଦି (negated == TRUE)
		    EMIT(NFA_END_NEG_COLL);
		ଅନ୍ୟଥା
		    EMIT(NFA_END_COLL);

		// \_[] also matches \n but it's not negated
		ଯଦି (extra == NFA_ADD_NL)
		{
		    EMIT(reg_string ? NL : NFA_NEWL);
		    EMIT(NFA_OR);
		}

		ଫେରନ୍ତୁ OK;
	    } // if exists closing ]

	    ଯଦି (reg_strict)
		EMSG_RET_FAIL(_(e_missing_rsb_after_str_lsb));
	    // FALLTHROUGH

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	plen;

nfa_do_multibyte:
		// plen is length of current char with composing chars
		ଯଦି (enc_utf8 && ((*mb_char2len)(c)
			    != (plen = utfc_ptr2len(old_regparse))
						       || utf_iscomposing(c)))
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;

		    // A base character plus composing characters, or just one
		    // or more composing characters.
		    // This requires creating a separate atom as if enclosing
		    // the characters in (), where NFA_COMPOSING is the ( and
		    // NFA_END_COMPOSING is the ). Note that right now we are
		    // building the postfix form, not the NFA itself;
		    // a composing char could be: a, b, c, NFA_COMPOSING
		    // where 'b' and 'c' are chars with codes > 256.
		    ପାଇଁ (;;)
		    {
			EMIT(c);
			ଯଦି (i > 0)
			    EMIT(NFA_CONCAT);
			ଯଦି ((i += utf_char2len(c)) >= plen)
			    ଭାଙ୍ଗ;
			c = utf_ptr2char(old_regparse + i);
		    }
		    EMIT(NFA_COMPOSING);
		    regparse = old_regparse + plen;
		}
		ଅନ୍ୟଥା
		{
		    c = no_Magic(c);
		    EMIT(c);
		}
		ଫେରନ୍ତୁ OK;
	    }
    }

    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
nfa_regpiece(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		op;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;
    long	minval, maxval;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		greedy = TRUE;      // Braces are prefixed with '-' ?
    parse_state_T old_state;
    parse_state_T new_state;
    long	c2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_post_pos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		my_post_start;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		quest;

    // Save the current parse state, so that we can use it if <atom>{m,n} is
    // next.
    save_parse_state(&old_state);

    // store current pos in the postfix form, for \{m,n} involving 0s
    my_post_start = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(post_ptr - post_start);

    ret = nfa_regatom();
    ଯଦି (ret == FAIL)
	ଫେରନ୍ତୁ FAIL;	    // cascaded error

    op = peekchr();
    ଯଦି (re_multi_type(op) == NOT_MULTI)
	ଫେରନ୍ତୁ OK;

    skipchr();
    ବାଛ (op)
    {
	ସ୍ଥିତି Magic('*'):
	    EMIT(NFA_STAR);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି Magic('+'):
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
	    ଯଦି (nfa_regatom() == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    EMIT(NFA_STAR);
	    EMIT(NFA_CONCAT);
	    skipchr();		// skip the \+
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି Magic('@'):
	    c2 = (long)getdecchrs();
	    op = no_Magic(getchr());
	    i = 0;
	    ବାଛ(op)
	    {
		ସ୍ଥିତି '=':
		    // \@=
		    i = NFA_PREV_ATOM_NO_WIDTH;
		    ଭାଙ୍ଗ;
		ସ୍ଥିତି '!':
		    // \@!
		    i = NFA_PREV_ATOM_NO_WIDTH_NEG;
		    ଭାଙ୍ଗ;
		ସ୍ଥିତି '<':
		    op = no_Magic(getchr());
		    ଯଦି (op == '=')
			// \@<=
			i = NFA_PREV_ATOM_JUST_BEFORE;
		    ଅନ୍ୟଥା ଯଦି (op == '!')
			// \@<!
			i = NFA_PREV_ATOM_JUST_BEFORE_NEG;
		    ଭାଙ୍ଗ;
		ସ୍ଥିତି '>':
		    // \@>
		    i = NFA_PREV_ATOM_LIKE_PATTERN;
		    ଭାଙ୍ଗ;
	    }
	    ଯଦି (i == 0)
	    {
		semsg(_(e_nfa_regexp_unknown_operator_at_chr), op);
		ଫେରନ୍ତୁ FAIL;
	    }
	    EMIT(i);
	    ଯଦି (i == NFA_PREV_ATOM_JUST_BEFORE
					|| i == NFA_PREV_ATOM_JUST_BEFORE_NEG)
		EMIT(c2);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି Magic('?'):
	ସ୍ଥିତି Magic('='):
	    EMIT(NFA_QUEST);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି Magic('{'):
	    // a{2,5} will expand to 'aaa?a?a?'
	    // a{-1,3} will expand to 'aa??a??', where ?? is the nongreedy
	    // version of '?'
	    // \v(ab){2,3} will expand to '(ab)(ab)(ab)?', where all the
	    // parenthesis have the same id

	    greedy = TRUE;
	    c2 = peekchr();
	    ଯଦି (c2 == '-' || c2 == Magic('-'))
	    {
		skipchr();
		greedy = FALSE;
	    }
	    ଯଦି (!read_limits(&minval, &maxval))
		EMSG_RET_FAIL(_(e_nfa_regexp_error_reading_repetition_limits));

	    //  <atom>{0,inf}, <atom>{0,} and <atom>{}  are equivalent to
	    //  <atom>*
	    ଯଦି (minval == 0 && maxval == MAX_LIMIT)
	    {
		ଯଦି (greedy)		// { { (match the braces)
		    // \{}, \{0,}
		    EMIT(NFA_STAR);
		ଅନ୍ୟଥା			// { { (match the braces)
		    // \{-}, \{-0,}
		    EMIT(NFA_STAR_NONGREEDY);
		ଭାଙ୍ଗ;
	    }

	    // Special case: x{0} or x{-0}
	    ଯଦି (maxval == 0)
	    {
		// Ignore result of previous call to nfa_regatom()
		post_ptr = post_start + my_post_start;
		// NFA_EMPTY is 0-length and works everywhere
		EMIT(NFA_EMPTY);
		ଫେରନ୍ତୁ OK;
	    }

	    // The engine is very inefficient (uses too many states) when the
	    // maximum is much larger than the minimum and when the maximum is
	    // large.  However, when maxval is MAX_LIMIT, it is okay, as this
	    // will emit NFA_STAR.
	    // Bail out if we can use the other engine, but only, when the
	    // pattern does not need the NFA engine like (e.g. [[:upper:]]\{2,\}
	    // does not work with characters > 8 bit with the BT engine)
	    ଯଦି ((nfa_re_flags & RE_AUTO)
				   && (maxval > 500 || maxval > minval + 200)
				   && (maxval != MAX_LIMIT && minval < 200)
				   && !wants_nfa)
		ଫେରନ୍ତୁ FAIL;

	    // Ignore previous call to nfa_regatom()
	    post_ptr = post_start + my_post_start;
	    // Save parse state after the repeated atom and the \{}
	    save_parse_state(&new_state);

	    quest = (greedy == TRUE? NFA_QUEST : NFA_QUEST_NONGREEDY);
	    ପାଇଁ (i = 0; i < maxval; i++)
	    {
		// Goto beginning of the repeated atom
		restore_parse_state(&old_state);
		old_post_pos = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(post_ptr - post_start);
		ଯଦି (nfa_regatom() == FAIL)
		    ଫେରନ୍ତୁ FAIL;
		// after "minval" times, atoms are optional
		ଯଦି (i + 1 > minval)
		{
		    ଯଦି (maxval == MAX_LIMIT)
		    {
			ଯଦି (greedy)
			    EMIT(NFA_STAR);
			ଅନ୍ୟଥା
			    EMIT(NFA_STAR_NONGREEDY);
		    }
		    ଅନ୍ୟଥା
			EMIT(quest);
		}
		ଯଦି (old_post_pos != my_post_start)
		    EMIT(NFA_CONCAT);
		ଯଦି (i + 1 > minval && maxval == MAX_LIMIT)
		    ଭାଙ୍ଗ;
	    }

	    // Go to just after the repeated atom and the \{}
	    restore_parse_state(&new_state);
	    curchr = -1;

	    ଭାଙ୍ଗ;


	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଭାଙ୍ଗ;
    }	// end switch

    ଯଦି (re_multi_type(peekchr()) != NOT_MULTI)
	// Can't have a multi follow a multi.
	EMSG_RET_FAIL(_(e_nfa_regexp_cant_have_multi_follow_multi));

    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
nfa_regconcat(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cont = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first = TRUE;

    ଯେତେବେଳେ (cont)
    {
	ବାଛ (peekchr())
	{
	    ସ୍ଥିତି NUL:
	    ସ୍ଥିତି Magic('|'):
	    ସ୍ଥିତି Magic('&'):
	    ସ୍ଥିତି Magic(')'):
		cont = FALSE;
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି Magic('Z'):
		regflags |= RF_ICOMBINE;
		skipchr_keepstart();
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି Magic('c'):
		regflags |= RF_ICASE;
		skipchr_keepstart();
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି Magic('C'):
		regflags |= RF_NOICASE;
		skipchr_keepstart();
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି Magic('v'):
		reg_magic = MAGIC_ALL;
		skipchr_keepstart();
		curchr = -1;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି Magic('m'):
		reg_magic = MAGIC_ON;
		skipchr_keepstart();
		curchr = -1;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି Magic('M'):
		reg_magic = MAGIC_OFF;
		skipchr_keepstart();
		curchr = -1;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି Magic('V'):
		reg_magic = MAGIC_NONE;
		skipchr_keepstart();
		curchr = -1;
		ଭାଙ୍ଗ;

	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଯଦି (nfa_regpiece() == FAIL)
		    ଫେରନ୍ତୁ FAIL;
		ଯଦି (first == FALSE)
		    EMIT(NFA_CONCAT);
		ଅନ୍ୟଥା
		    first = FALSE;
		ଭାଙ୍ଗ;
	}
    }

    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
nfa_regbranch(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_post_pos;

    old_post_pos = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(post_ptr - post_start);

    // First branch, possibly the only one
    ଯଦି (nfa_regconcat() == FAIL)
	ଫେରନ୍ତୁ FAIL;

    // Try next concats
    ଯେତେବେଳେ (peekchr() == Magic('&'))
    {
	skipchr();
	// if concat is empty do emit a node
	ଯଦି (old_post_pos == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(post_ptr - post_start))
	    EMIT(NFA_EMPTY);
	EMIT(NFA_NOPEN);
	EMIT(NFA_PREV_ATOM_NO_WIDTH);
	old_post_pos = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(post_ptr - post_start);
	ଯଦି (nfa_regconcat() == FAIL)
	    ଫେରନ୍ତୁ FAIL;
	// if concat is empty do emit a node
	ଯଦି (old_post_pos == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(post_ptr - post_start))
	    EMIT(NFA_EMPTY);
	EMIT(NFA_CONCAT);
    }

    // if a branch is empty, emit one node for it
    ଯଦି (old_post_pos == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(post_ptr - post_start))
	EMIT(NFA_EMPTY);

    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
nfa_reg(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		paren)	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		parno = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status = FAIL;

    ଯଦି (paren == REG_PAREN)
    {
	ଯଦି (regnpar >= NSUBEXP) // Too many `('
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_parens));
	parno = regnpar++;
    }
#ifdef FEAT_SYN_HL
    ଅନ୍ୟଥା ଯଦି (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	ଯଦି (regnzpar >= NSUBEXP)
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_z));
	parno = regnzpar++;
    }
#endif

    ଯଦି (nfa_reg_parse_depth >= REG_MAX_PAREN_DEPTH)
	EMSG_RET_FAIL(_(e_command_too_complex));
    ++nfa_reg_parse_depth;

    ଯଦି (nfa_regbranch() == FAIL)
	ଯାଅ theend;	    // cascaded error

    ଯେତେବେଳେ (peekchr() == Magic('|'))
    {
	skipchr();
	ଯଦି (nfa_regbranch() == FAIL)
	    ଯାଅ theend;    // cascaded error
	EMIT(NFA_OR);
    }

    // Check for proper termination.
    ଯଦି (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
	ଯଦି (paren == REG_NPAREN)
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	ଅନ୍ୟଥା
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	rc_did_emsg = TRUE;
	ଯାଅ theend;
    }
    ଅନ୍ୟଥା ଯଦି (paren == REG_NOPAREN && peekchr() != NUL)
    {
	ଯଦି (peekchr() == Magic(')'))
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	ଅନ୍ୟଥା
	    emsg(_(e_nfa_regexp_proper_termination_error));
	rc_did_emsg = TRUE;
	ଯାଅ theend;
    }
    /*
     * Here we set the flag allowing back references to this set of
     * parentheses.
     */
    ଯଦି (paren == REG_PAREN)
    {
	had_endbrace[parno] = TRUE;     // have seen the close paren
	EMIT(NFA_MOPEN + parno);
    }
#ifdef FEAT_SYN_HL
    ଅନ୍ୟଥା ଯଦି (paren == REG_ZPAREN)
	EMIT(NFA_ZOPEN + parno);
#endif

    status = OK;

theend:
    --nfa_reg_parse_depth;
    ଫେରନ୍ତୁ status;
}

#ifdef DEBUG
ସ୍ଥିର char_u code[50];

    ସ୍ଥିର ଶୂନ୍ୟ
nfa_set_code(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    addnl = FALSE;

    ଯଦି (c >= NFA_FIRST_NL && c <= NFA_LAST_NL)
    {
	addnl = TRUE;
	c -= NFA_ADD_NL;
    }

    STRCPY(code, "");
    ବାଛ (c)
    {
	ସ୍ଥିତି NFA_MATCH:	    STRCPY(code, "NFA_MATCH "); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_SPLIT:	    STRCPY(code, "NFA_SPLIT "); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CONCAT:    STRCPY(code, "NFA_CONCAT "); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_NEWL:	    STRCPY(code, "NFA_NEWL "); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_ZSTART:    STRCPY(code, "NFA_ZSTART"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_ZEND:	    STRCPY(code, "NFA_ZEND"); ଭାଙ୍ଗ;

	ସ୍ଥିତି NFA_BACKREF1:  STRCPY(code, "NFA_BACKREF1"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_BACKREF2:  STRCPY(code, "NFA_BACKREF2"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_BACKREF3:  STRCPY(code, "NFA_BACKREF3"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_BACKREF4:  STRCPY(code, "NFA_BACKREF4"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_BACKREF5:  STRCPY(code, "NFA_BACKREF5"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_BACKREF6:  STRCPY(code, "NFA_BACKREF6"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_BACKREF7:  STRCPY(code, "NFA_BACKREF7"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_BACKREF8:  STRCPY(code, "NFA_BACKREF8"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_BACKREF9:  STRCPY(code, "NFA_BACKREF9"); ଭାଙ୍ଗ;
# ifdef FEAT_SYN_HL
	ସ୍ଥିତି NFA_ZREF1:	    STRCPY(code, "NFA_ZREF1"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_ZREF2:	    STRCPY(code, "NFA_ZREF2"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_ZREF3:	    STRCPY(code, "NFA_ZREF3"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_ZREF4:	    STRCPY(code, "NFA_ZREF4"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_ZREF5:	    STRCPY(code, "NFA_ZREF5"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_ZREF6:	    STRCPY(code, "NFA_ZREF6"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_ZREF7:	    STRCPY(code, "NFA_ZREF7"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_ZREF8:	    STRCPY(code, "NFA_ZREF8"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_ZREF9:	    STRCPY(code, "NFA_ZREF9"); ଭାଙ୍ଗ;
# endif
	ସ୍ଥିତି NFA_SKIP:	    STRCPY(code, "NFA_SKIP"); ଭାଙ୍ଗ;

	ସ୍ଥିତି NFA_PREV_ATOM_NO_WIDTH:
			    STRCPY(code, "NFA_PREV_ATOM_NO_WIDTH"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_PREV_ATOM_NO_WIDTH_NEG:
			    STRCPY(code, "NFA_PREV_ATOM_NO_WIDTH_NEG"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_PREV_ATOM_JUST_BEFORE:
			    STRCPY(code, "NFA_PREV_ATOM_JUST_BEFORE"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_PREV_ATOM_JUST_BEFORE_NEG:
			 STRCPY(code, "NFA_PREV_ATOM_JUST_BEFORE_NEG"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_PREV_ATOM_LIKE_PATTERN:
			    STRCPY(code, "NFA_PREV_ATOM_LIKE_PATTERN"); ଭାଙ୍ଗ;

	ସ୍ଥିତି NFA_NOPEN:		    STRCPY(code, "NFA_NOPEN"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_NCLOSE:	    STRCPY(code, "NFA_NCLOSE"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_START_INVISIBLE:   STRCPY(code, "NFA_START_INVISIBLE"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_START_INVISIBLE_FIRST:
			     STRCPY(code, "NFA_START_INVISIBLE_FIRST"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_START_INVISIBLE_NEG:
			       STRCPY(code, "NFA_START_INVISIBLE_NEG"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_START_INVISIBLE_NEG_FIRST:
			 STRCPY(code, "NFA_START_INVISIBLE_NEG_FIRST"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_START_INVISIBLE_BEFORE:
			    STRCPY(code, "NFA_START_INVISIBLE_BEFORE"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_START_INVISIBLE_BEFORE_FIRST:
		      STRCPY(code, "NFA_START_INVISIBLE_BEFORE_FIRST"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_START_INVISIBLE_BEFORE_NEG:
			STRCPY(code, "NFA_START_INVISIBLE_BEFORE_NEG"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
		  STRCPY(code, "NFA_START_INVISIBLE_BEFORE_NEG_FIRST"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_START_PATTERN:   STRCPY(code, "NFA_START_PATTERN"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_END_INVISIBLE:	    STRCPY(code, "NFA_END_INVISIBLE"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_END_INVISIBLE_NEG: STRCPY(code, "NFA_END_INVISIBLE_NEG"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_END_PATTERN:	    STRCPY(code, "NFA_END_PATTERN"); ଭାଙ୍ଗ;

	ସ୍ଥିତି NFA_COMPOSING:	    STRCPY(code, "NFA_COMPOSING"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_END_COMPOSING:	    STRCPY(code, "NFA_END_COMPOSING"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_OPT_CHARS:	    STRCPY(code, "NFA_OPT_CHARS"); ଭାଙ୍ଗ;

	ସ୍ଥିତି NFA_MOPEN:
	ସ୍ଥିତି NFA_MOPEN1:
	ସ୍ଥିତି NFA_MOPEN2:
	ସ୍ଥିତି NFA_MOPEN3:
	ସ୍ଥିତି NFA_MOPEN4:
	ସ୍ଥିତି NFA_MOPEN5:
	ସ୍ଥିତି NFA_MOPEN6:
	ସ୍ଥିତି NFA_MOPEN7:
	ସ୍ଥିତି NFA_MOPEN8:
	ସ୍ଥିତି NFA_MOPEN9:
	    STRCPY(code, "NFA_MOPEN(x)");
	    code[10] = c - NFA_MOPEN + '0';
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_MCLOSE:
	ସ୍ଥିତି NFA_MCLOSE1:
	ସ୍ଥିତି NFA_MCLOSE2:
	ସ୍ଥିତି NFA_MCLOSE3:
	ସ୍ଥିତି NFA_MCLOSE4:
	ସ୍ଥିତି NFA_MCLOSE5:
	ସ୍ଥିତି NFA_MCLOSE6:
	ସ୍ଥିତି NFA_MCLOSE7:
	ସ୍ଥିତି NFA_MCLOSE8:
	ସ୍ଥିତି NFA_MCLOSE9:
	    STRCPY(code, "NFA_MCLOSE(x)");
	    code[11] = c - NFA_MCLOSE + '0';
	    ଭାଙ୍ଗ;
# ifdef FEAT_SYN_HL
	ସ୍ଥିତି NFA_ZOPEN:
	ସ୍ଥିତି NFA_ZOPEN1:
	ସ୍ଥିତି NFA_ZOPEN2:
	ସ୍ଥିତି NFA_ZOPEN3:
	ସ୍ଥିତି NFA_ZOPEN4:
	ସ୍ଥିତି NFA_ZOPEN5:
	ସ୍ଥିତି NFA_ZOPEN6:
	ସ୍ଥିତି NFA_ZOPEN7:
	ସ୍ଥିତି NFA_ZOPEN8:
	ସ୍ଥିତି NFA_ZOPEN9:
	    STRCPY(code, "NFA_ZOPEN(x)");
	    code[10] = c - NFA_ZOPEN + '0';
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_ZCLOSE:
	ସ୍ଥିତି NFA_ZCLOSE1:
	ସ୍ଥିତି NFA_ZCLOSE2:
	ସ୍ଥିତି NFA_ZCLOSE3:
	ସ୍ଥିତି NFA_ZCLOSE4:
	ସ୍ଥିତି NFA_ZCLOSE5:
	ସ୍ଥିତି NFA_ZCLOSE6:
	ସ୍ଥିତି NFA_ZCLOSE7:
	ସ୍ଥିତି NFA_ZCLOSE8:
	ସ୍ଥିତି NFA_ZCLOSE9:
	    STRCPY(code, "NFA_ZCLOSE(x)");
	    code[11] = c - NFA_ZCLOSE + '0';
	    ଭାଙ୍ଗ;
# endif
	ସ୍ଥିତି NFA_EOL:		STRCPY(code, "NFA_EOL "); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_BOL:		STRCPY(code, "NFA_BOL "); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_EOW:		STRCPY(code, "NFA_EOW "); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_BOW:		STRCPY(code, "NFA_BOW "); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_EOF:		STRCPY(code, "NFA_EOF "); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_BOF:		STRCPY(code, "NFA_BOF "); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_LNUM:		STRCPY(code, "NFA_LNUM "); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_LNUM_GT:	STRCPY(code, "NFA_LNUM_GT "); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_LNUM_LT:	STRCPY(code, "NFA_LNUM_LT "); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_COL:		STRCPY(code, "NFA_COL "); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_COL_GT:	STRCPY(code, "NFA_COL_GT "); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_COL_LT:	STRCPY(code, "NFA_COL_LT "); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_VCOL:		STRCPY(code, "NFA_VCOL "); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_VCOL_GT:	STRCPY(code, "NFA_VCOL_GT "); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_VCOL_LT:	STRCPY(code, "NFA_VCOL_LT "); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_MARK:		STRCPY(code, "NFA_MARK "); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_MARK_GT:	STRCPY(code, "NFA_MARK_GT "); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_MARK_LT:	STRCPY(code, "NFA_MARK_LT "); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CURSOR:	STRCPY(code, "NFA_CURSOR "); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_VISUAL:	STRCPY(code, "NFA_VISUAL "); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_ANY_COMPOSING:	STRCPY(code, "NFA_ANY_COMPOSING "); ଭାଙ୍ଗ;

	ସ୍ଥିତି NFA_STAR:		STRCPY(code, "NFA_STAR "); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_STAR_NONGREEDY: STRCPY(code, "NFA_STAR_NONGREEDY "); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_QUEST:		STRCPY(code, "NFA_QUEST"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_QUEST_NONGREEDY: STRCPY(code, "NFA_QUEST_NON_GREEDY"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_EMPTY:		STRCPY(code, "NFA_EMPTY"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_OR:		STRCPY(code, "NFA_OR"); ଭାଙ୍ଗ;

	ସ୍ଥିତି NFA_START_COLL:	STRCPY(code, "NFA_START_COLL"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_END_COLL:	STRCPY(code, "NFA_END_COLL"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_START_NEG_COLL: STRCPY(code, "NFA_START_NEG_COLL"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_END_NEG_COLL:	STRCPY(code, "NFA_END_NEG_COLL"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_RANGE:		STRCPY(code, "NFA_RANGE"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_RANGE_MIN:	STRCPY(code, "NFA_RANGE_MIN"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_RANGE_MAX:	STRCPY(code, "NFA_RANGE_MAX"); ଭାଙ୍ଗ;

	ସ୍ଥିତି NFA_CLASS_ALNUM:	STRCPY(code, "NFA_CLASS_ALNUM"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_ALPHA:	STRCPY(code, "NFA_CLASS_ALPHA"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_BLANK:	STRCPY(code, "NFA_CLASS_BLANK"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_CNTRL:	STRCPY(code, "NFA_CLASS_CNTRL"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_DIGIT:	STRCPY(code, "NFA_CLASS_DIGIT"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_GRAPH:	STRCPY(code, "NFA_CLASS_GRAPH"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_LOWER:	STRCPY(code, "NFA_CLASS_LOWER"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_PRINT:	STRCPY(code, "NFA_CLASS_PRINT"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_PUNCT:	STRCPY(code, "NFA_CLASS_PUNCT"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_SPACE:	STRCPY(code, "NFA_CLASS_SPACE"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_UPPER:	STRCPY(code, "NFA_CLASS_UPPER"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_XDIGIT:	STRCPY(code, "NFA_CLASS_XDIGIT"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_TAB:	STRCPY(code, "NFA_CLASS_TAB"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_RETURN:	STRCPY(code, "NFA_CLASS_RETURN"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_BACKSPACE:   STRCPY(code, "NFA_CLASS_BACKSPACE"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_ESCAPE:	STRCPY(code, "NFA_CLASS_ESCAPE"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_IDENT:	STRCPY(code, "NFA_CLASS_IDENT"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_KEYWORD:	STRCPY(code, "NFA_CLASS_KEYWORD"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_FNAME:	STRCPY(code, "NFA_CLASS_FNAME"); ଭାଙ୍ଗ;

	ସ୍ଥିତି NFA_ANY:	STRCPY(code, "NFA_ANY"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_IDENT:	STRCPY(code, "NFA_IDENT"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_SIDENT:STRCPY(code, "NFA_SIDENT"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_KWORD:	STRCPY(code, "NFA_KWORD"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_SKWORD:STRCPY(code, "NFA_SKWORD"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_FNAME:	STRCPY(code, "NFA_FNAME"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_SFNAME:STRCPY(code, "NFA_SFNAME"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_PRINT:	STRCPY(code, "NFA_PRINT"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_SPRINT:STRCPY(code, "NFA_SPRINT"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_WHITE:	STRCPY(code, "NFA_WHITE"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_NWHITE:STRCPY(code, "NFA_NWHITE"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_DIGIT:	STRCPY(code, "NFA_DIGIT"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_NDIGIT:STRCPY(code, "NFA_NDIGIT"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_HEX:	STRCPY(code, "NFA_HEX"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_NHEX:	STRCPY(code, "NFA_NHEX"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_OCTAL:	STRCPY(code, "NFA_OCTAL"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_NOCTAL:STRCPY(code, "NFA_NOCTAL"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_WORD:	STRCPY(code, "NFA_WORD"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_NWORD:	STRCPY(code, "NFA_NWORD"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_HEAD:	STRCPY(code, "NFA_HEAD"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_NHEAD:	STRCPY(code, "NFA_NHEAD"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_ALPHA:	STRCPY(code, "NFA_ALPHA"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_NALPHA:STRCPY(code, "NFA_NALPHA"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_LOWER:	STRCPY(code, "NFA_LOWER"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_NLOWER:STRCPY(code, "NFA_NLOWER"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_UPPER:	STRCPY(code, "NFA_UPPER"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_NUPPER:STRCPY(code, "NFA_NUPPER"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_LOWER_IC:  STRCPY(code, "NFA_LOWER_IC"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_NLOWER_IC: STRCPY(code, "NFA_NLOWER_IC"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_UPPER_IC:  STRCPY(code, "NFA_UPPER_IC"); ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_NUPPER_IC: STRCPY(code, "NFA_NUPPER_IC"); ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    STRCPY(code, "CHAR(x)");
	    code[5] = c;
    }

    ଯଦି (addnl == TRUE)
	STRCAT(code, " + NEWLINE ");

}

# ifdef ENABLE_LOG
ସ୍ଥିର FILE *log_fd;
ସ୍ଥିର char_u e_log_open_failed[] = N_("Could not open temporary log file for writing, displaying on stderr... ");

/*
 * Print the postfix notation of the current regexp.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
nfa_postfix_dump(char_u *ପଦସମଷ୍ଟି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା retval)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *p;
    FILE *f;

    f = fopen(NFA_REGEXP_DUMP_LOG, "a");
    ଯଦି (f == ଖାଲି)
	ଫେରନ୍ତୁ;

    fprintf(f, "\n-------------------------\n");
    ଯଦି (retval == FAIL)
	fprintf(f, ">>> NFA engine failed... \n");
    ଅନ୍ୟଥା ଯଦି (retval == OK)
	fprintf(f, ">>> NFA engine succeeded !\n");
    fprintf(f, "Regexp: \"%s\"\nPostfix notation (char): \"", ପଦସମଷ୍ଟି);
    ପାଇଁ (p = post_start; *p && p < post_ptr; p++)
    {
	nfa_set_code(*p);
	fprintf(f, "%s, ", code);
    }
    fprintf(f, "\"\nPostfix notation (int): ");
    ପାଇଁ (p = post_start; *p && p < post_ptr; p++)
	fprintf(f, "%d ", *p);
    fprintf(f, "\n\n");
    fclose(f);
}

/*
 * Print the NFA starting with a root node "state".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
nfa_print_state(FILE *debugf, nfa_state_T *state)
{
    garray_T indent;

    ga_init2(&indent, 1, 64);
    ga_append(&indent, '\0');
    nfa_print_state2(debugf, state, &indent);
    ga_clear(&indent);
}

    ସ୍ଥିର ଶୂନ୍ୟ
nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent)
{
    char_u  *p;

    ଯଦି (state == ଖାଲି)
	ଫେରନ୍ତୁ;

    fprintf(debugf, "(%2d)", abs(state->id));

    // Output indent
    p = (char_u *)indent->ga_data;
    ଯଦି (indent->ga_len >= 3)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	last = indent->ga_len - 3;
	char_u	save[2];

	STRNCPY(save, &p[last], 2);
	memcpy(&p[last], "+-", 2);
	fprintf(debugf, " %s", p);
	STRNCPY(&p[last], save, 2);
    }
    ଅନ୍ୟଥା
	fprintf(debugf, " %s", p);

    nfa_set_code(state->c);
    fprintf(debugf, "%s (%d) (id=%d) val=%d\n",
		 code,
		 state->c,
		 abs(state->id),
		 state->val);
    ଯଦି (state->id < 0)
	ଫେରନ୍ତୁ;

    state->id = abs(state->id) * -1;

    // grow indent for state->out
    indent->ga_len -= 1;
    ଯଦି (state->out1)
	GA_CONCAT_LITERAL(indent, "| ");
    ଅନ୍ୟଥା
	GA_CONCAT_LITERAL(indent, "  ");
    ga_append(indent, NUL);

    nfa_print_state2(debugf, state->out, indent);

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
    ସ୍ଥିର ଶୂନ୍ୟ
nfa_dump(nfa_regprog_T *prog)
{
    FILE *debugf = fopen(NFA_REGEXP_DUMP_LOG, "a");

    ଯଦି (debugf == ଖାଲି)
	ଫେରନ୍ତୁ;

    nfa_print_state(debugf, prog->start);

    ଯଦି (prog->reganch)
	fprintf(debugf, "reganch: %d\n", prog->reganch);
    ଯଦି (prog->regstart != NUL)
	fprintf(debugf, "regstart: %c (decimal: %d)\n",
		prog->regstart, prog->regstart);
    ଯଦି (prog->match_text != ଖାଲି)
	fprintf(debugf, "match_text: \"%s\"\n", prog->match_text);

    fclose(debugf);
}
# endif	    // ENABLE_LOG
#endif	    // DEBUG

/*
 * Parse r.e. @expr and convert it into postfix form.
 * Return the postfix string on success, NULL otherwise.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା *
re2post(ଶୂନ୍ୟ)
{
    ଯଦି (nfa_reg(REG_NOPAREN) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;
    EMIT(NFA_MOPEN);
    ଫେରନ୍ତୁ post_start;
}

// NB. Some of the code below is inspired by Russ's.

/*
 * Represents an NFA state plus zero or one or two arrows exiting.
 * if c == MATCH, no arrows out; matching state.
 * If c == SPLIT, unlabeled arrows to out and out1 (if != NULL).
 * If c < 256, labeled arrow with character c to out.
 */

ସ୍ଥିର nfa_state_T	*state_ptr; // points to nfa_prog->state

/*
 * Allocate and initialize nfa_state_T.
 */
    ସ୍ଥିର nfa_state_T *
alloc_state(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, nfa_state_T *out, nfa_state_T *out1)
{
    nfa_state_T *s;

    ଯଦି (istate >= nstate)
	ଫେରନ୍ତୁ ଖାଲି;

    s = &state_ptr[istate++];

    s->c    = c;
    s->out  = out;
    s->out1 = out1;
    s->val  = 0;

    s->id   = istate;
    s->lastlist[0] = 0;
    s->lastlist[1] = 0;

    ଫେରନ୍ତୁ s;
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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂଘ Ptrlist Ptrlist;
ସଂଘ Ptrlist
{
    Ptrlist	*next;
    nfa_state_T	*s;
};

ସଂରଚନା Frag
{
    nfa_state_T *start;
    Ptrlist	*out;
};
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା Frag Frag_T;

// Reused across compilations by post2nfa(); freed in free_regexp_stuff().
ସ୍ଥିର Frag_T *nfa_ଗଦା;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା nfa_ଗଦା_len;

/*
 * Initialize a Frag_T struct and return it.
 */
    ସ୍ଥିର Frag_T
frag(nfa_state_T *start, Ptrlist *out)
{
    Frag_T n;

    n.start = start;
    n.out = out;
    ଫେରନ୍ତୁ n;
}

/*
 * Create singleton list containing just outp.
 */
    ସ୍ଥିର Ptrlist *
list1(
    nfa_state_T	**outp)
{
    Ptrlist *l;

    l = (Ptrlist *)outp;
    l->next = ଖାଲି;
    ଫେରନ୍ତୁ l;
}

/*
 * Patch the list of states at out to point to start.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
patch(Ptrlist *l, nfa_state_T *s)
{
    Ptrlist *next;

    ପାଇଁ (; l; l = next)
    {
	next = l->next;
	l->s = s;
    }
}


/*
 * Join the two lists l1 and l2, returning the combination.
 */
    ସ୍ଥିର Ptrlist *
append(Ptrlist *l1, Ptrlist *l2)
{
    Ptrlist *oldl1;

    oldl1 = l1;
    ଯେତେବେଳେ (l1->next)
	l1 = l1->next;
    l1->next = l2;
    ଫେରନ୍ତୁ oldl1;
}

/*
 * Stack used for transforming postfix form into NFA.
 */
ସ୍ଥିର Frag_T empty;

    ସ୍ଥିର ଶୂନ୍ୟ
st_error(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *postfix UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *end UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *p UNUSED)
{
#ifdef NFA_REGEXP_ERROR_LOG
    FILE *df;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *p2;

    df = fopen(NFA_REGEXP_ERROR_LOG, "a");
    ଯଦି (df)
    {
	fprintf(df, "Error popping the stack!\n");
# ifdef DEBUG
	fprintf(df, "Current regexp is \"%s\"\n", nfa_regengine.ପଦସମଷ୍ଟି);
# endif
	fprintf(df, "Postfix form is: ");
# ifdef DEBUG
	ପାଇଁ (p2 = postfix; p2 < end; p2++)
	{
	    nfa_set_code(*p2);
	    fprintf(df, "%s, ", code);
	}
	nfa_set_code(*p);
	fprintf(df, "\nCurrent position is: ");
	ପାଇଁ (p2 = postfix; p2 <= p; p2 ++)
	{
	    nfa_set_code(*p2);
	    fprintf(df, "%s, ", code);
	}
# ଅନ୍ୟଥା
	ପାଇଁ (p2 = postfix; p2 < end; p2++)
	    fprintf(df, "%d, ", *p2);
	fprintf(df, "\nCurrent position is: ");
	ପାଇଁ (p2 = postfix; p2 <= p; p2 ++)
	    fprintf(df, "%d, ", *p2);
# endif
	fprintf(df, "\n--------------------------\n");
	fclose(df);
    }
#endif
    emsg(_(e_nfa_regexp_could_not_କାଢ଼_ଗଦା));
}

/*
 * Push an item onto the stack.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
st_ଠେଲ(Frag_T s, Frag_T **p, Frag_T *ଗଦା_end)
{
    Frag_T *stackp = *p;

    ଯଦି (stackp >= ଗଦା_end)
	ଫେରନ୍ତୁ;
    *stackp = s;
    *p = *p + 1;
}

/*
 * Pop an item from the stack.
 */
    ସ୍ଥିର Frag_T
st_କାଢ଼(Frag_T **p, Frag_T *ଗଦା)
{
    Frag_T *stackp;

    *p = *p - 1;
    stackp = *p;
    ଯଦି (stackp < ଗଦା)
	ଫେରନ୍ତୁ empty;
    ଫେରନ୍ତୁ **p;
}

/*
 * Estimate the maximum byte length of anything matching "state".
 * When unknown or unlimited return -1.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
nfa_max_width(nfa_state_T *startstate, ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    l, r;
    nfa_state_T	    *state = startstate;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    len = 0;

    // detect looping in a NFA_SPLIT
    ଯଦି (depth > 4)
	ଫେରନ୍ତୁ -1;

    ଯେତେବେଳେ (state != ଖାଲି)
    {
	ବାଛ (state->c)
	{
	    ସ୍ଥିତି NFA_END_INVISIBLE:
	    ସ୍ଥିତି NFA_END_INVISIBLE_NEG:
		// the end, return what we have
		ଫେରନ୍ତୁ len;

	    ସ୍ଥିତି NFA_SPLIT:
		// two alternatives, use the maximum
		l = nfa_max_width(state->out, depth + 1);
		r = nfa_max_width(state->out1, depth + 1);
		ଯଦି (l < 0 || r < 0)
		    ଫେରନ୍ତୁ -1;
		ଫେରନ୍ତୁ len + (l > r ? l : r);

	    ସ୍ଥିତି NFA_ANY:
	    ସ୍ଥିତି NFA_START_COLL:
	    ସ୍ଥିତି NFA_START_NEG_COLL:
		// matches some character, including composing chars
		ଯଦି (enc_utf8)
		    len += MB_MAXBYTES;
		ଅନ୍ୟଥା ଯଦି (has_mbyte)
		    len += 2;
		ଅନ୍ୟଥା
		    ++len;
		ଯଦି (state->c != NFA_ANY)
		{
		    // Skip over the compiled collection.
		    // malformed NFAs must not crash width estimation.
		    ଯଦି (state->out1 == ଖାଲି || state->out1->out == ଖାଲି)
			ଫେରନ୍ତୁ -1;
		    state = state->out1->out;
		    ଜାରି;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_DIGIT:
	    ସ୍ଥିତି NFA_WHITE:
	    ସ୍ଥିତି NFA_HEX:
	    ସ୍ଥିତି NFA_OCTAL:
		// ascii
		++len;
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_IDENT:
	    ସ୍ଥିତି NFA_SIDENT:
	    ସ୍ଥିତି NFA_KWORD:
	    ସ୍ଥିତି NFA_SKWORD:
	    ସ୍ଥିତି NFA_FNAME:
	    ସ୍ଥିତି NFA_SFNAME:
	    ସ୍ଥିତି NFA_PRINT:
	    ସ୍ଥିତି NFA_SPRINT:
	    ସ୍ଥିତି NFA_NWHITE:
	    ସ୍ଥିତି NFA_NDIGIT:
	    ସ୍ଥିତି NFA_NHEX:
	    ସ୍ଥିତି NFA_NOCTAL:
	    ସ୍ଥିତି NFA_WORD:
	    ସ୍ଥିତି NFA_NWORD:
	    ସ୍ଥିତି NFA_HEAD:
	    ସ୍ଥିତି NFA_NHEAD:
	    ସ୍ଥିତି NFA_ALPHA:
	    ସ୍ଥିତି NFA_NALPHA:
	    ସ୍ଥିତି NFA_LOWER:
	    ସ୍ଥିତି NFA_NLOWER:
	    ସ୍ଥିତି NFA_UPPER:
	    ସ୍ଥିତି NFA_NUPPER:
	    ସ୍ଥିତି NFA_LOWER_IC:
	    ସ୍ଥିତି NFA_NLOWER_IC:
	    ସ୍ଥିତି NFA_UPPER_IC:
	    ସ୍ଥିତି NFA_NUPPER_IC:
	    ସ୍ଥିତି NFA_ANY_COMPOSING:
		// possibly non-ascii
		ଯଦି (has_mbyte)
		    len += 3;
		ଅନ୍ୟଥା
		    ++len;
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_START_INVISIBLE:
	    ସ୍ଥିତି NFA_START_INVISIBLE_NEG:
	    ସ୍ଥିତି NFA_START_INVISIBLE_BEFORE:
	    ସ୍ଥିତି NFA_START_INVISIBLE_BEFORE_NEG:
		// zero-width, out1 points to the END state
		state = state->out1->out;
		ଜାରି;

	    ସ୍ଥିତି NFA_BACKREF1:
	    ସ୍ଥିତି NFA_BACKREF2:
	    ସ୍ଥିତି NFA_BACKREF3:
	    ସ୍ଥିତି NFA_BACKREF4:
	    ସ୍ଥିତି NFA_BACKREF5:
	    ସ୍ଥିତି NFA_BACKREF6:
	    ସ୍ଥିତି NFA_BACKREF7:
	    ସ୍ଥିତି NFA_BACKREF8:
	    ସ୍ଥିତି NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	    ସ୍ଥିତି NFA_ZREF1:
	    ସ୍ଥିତି NFA_ZREF2:
	    ସ୍ଥିତି NFA_ZREF3:
	    ସ୍ଥିତି NFA_ZREF4:
	    ସ୍ଥିତି NFA_ZREF5:
	    ସ୍ଥିତି NFA_ZREF6:
	    ସ୍ଥିତି NFA_ZREF7:
	    ସ୍ଥିତି NFA_ZREF8:
	    ସ୍ଥିତି NFA_ZREF9:
#endif
	    ସ୍ଥିତି NFA_NEWL:
	    ସ୍ଥିତି NFA_SKIP:
		// unknown width
		ଫେରନ୍ତୁ -1;

	    ସ୍ଥିତି NFA_BOL:
	    ସ୍ଥିତି NFA_EOL:
	    ସ୍ଥିତି NFA_BOF:
	    ସ୍ଥିତି NFA_EOF:
	    ସ୍ଥିତି NFA_BOW:
	    ସ୍ଥିତି NFA_EOW:
	    ସ୍ଥିତି NFA_MOPEN:
	    ସ୍ଥିତି NFA_MOPEN1:
	    ସ୍ଥିତି NFA_MOPEN2:
	    ସ୍ଥିତି NFA_MOPEN3:
	    ସ୍ଥିତି NFA_MOPEN4:
	    ସ୍ଥିତି NFA_MOPEN5:
	    ସ୍ଥିତି NFA_MOPEN6:
	    ସ୍ଥିତି NFA_MOPEN7:
	    ସ୍ଥିତି NFA_MOPEN8:
	    ସ୍ଥିତି NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	    ସ୍ଥିତି NFA_ZOPEN:
	    ସ୍ଥିତି NFA_ZOPEN1:
	    ସ୍ଥିତି NFA_ZOPEN2:
	    ସ୍ଥିତି NFA_ZOPEN3:
	    ସ୍ଥିତି NFA_ZOPEN4:
	    ସ୍ଥିତି NFA_ZOPEN5:
	    ସ୍ଥିତି NFA_ZOPEN6:
	    ସ୍ଥିତି NFA_ZOPEN7:
	    ସ୍ଥିତି NFA_ZOPEN8:
	    ସ୍ଥିତି NFA_ZOPEN9:
	    ସ୍ଥିତି NFA_ZCLOSE:
	    ସ୍ଥିତି NFA_ZCLOSE1:
	    ସ୍ଥିତି NFA_ZCLOSE2:
	    ସ୍ଥିତି NFA_ZCLOSE3:
	    ସ୍ଥିତି NFA_ZCLOSE4:
	    ସ୍ଥିତି NFA_ZCLOSE5:
	    ସ୍ଥିତି NFA_ZCLOSE6:
	    ସ୍ଥିତି NFA_ZCLOSE7:
	    ସ୍ଥିତି NFA_ZCLOSE8:
	    ସ୍ଥିତି NFA_ZCLOSE9:
#endif
	    ସ୍ଥିତି NFA_MCLOSE:
	    ସ୍ଥିତି NFA_MCLOSE1:
	    ସ୍ଥିତି NFA_MCLOSE2:
	    ସ୍ଥିତି NFA_MCLOSE3:
	    ସ୍ଥିତି NFA_MCLOSE4:
	    ସ୍ଥିତି NFA_MCLOSE5:
	    ସ୍ଥିତି NFA_MCLOSE6:
	    ସ୍ଥିତି NFA_MCLOSE7:
	    ସ୍ଥିତି NFA_MCLOSE8:
	    ସ୍ଥିତି NFA_MCLOSE9:
	    ସ୍ଥିତି NFA_NOPEN:
	    ସ୍ଥିତି NFA_NCLOSE:

	    ସ୍ଥିତି NFA_LNUM_GT:
	    ସ୍ଥିତି NFA_LNUM_LT:
	    ସ୍ଥିତି NFA_COL_GT:
	    ସ୍ଥିତି NFA_COL_LT:
	    ସ୍ଥିତି NFA_VCOL_GT:
	    ସ୍ଥିତି NFA_VCOL_LT:
	    ସ୍ଥିତି NFA_MARK_GT:
	    ସ୍ଥିତି NFA_MARK_LT:
	    ସ୍ଥିତି NFA_VISUAL:
	    ସ୍ଥିତି NFA_LNUM:
	    ସ୍ଥିତି NFA_CURSOR:
	    ସ୍ଥିତି NFA_COL:
	    ସ୍ଥିତି NFA_VCOL:
	    ସ୍ଥିତି NFA_MARK:

	    ସ୍ଥିତି NFA_ZSTART:
	    ସ୍ଥିତି NFA_ZEND:
	    ସ୍ଥିତି NFA_OPT_CHARS:
	    ସ୍ଥିତି NFA_EMPTY:
	    ସ୍ଥିତି NFA_START_PATTERN:
	    ସ୍ଥିତି NFA_END_PATTERN:
	    ସ୍ଥିତି NFA_COMPOSING:
	    ସ୍ଥିତି NFA_END_COMPOSING:
		// zero-width
		ଭାଙ୍ଗ;

	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଯଦି (state->c < 0)
		    // don't know what this is
		    ଫେରନ୍ତୁ -1;
		// normal character
		len += MB_CHAR2LEN(state->c);
		ଭାଙ୍ଗ;
	}

	// normal way to continue
	state = state->out;
    }

    // unrecognized, "cannot happen"
    ଫେରନ୍ତୁ -1;
}

/*
 * Convert a postfix form into its equivalent NFA.
 * Return the NFA start state on success, NULL otherwise.
 */
    ସ୍ଥିର nfa_state_T *
post2nfa(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *postfix, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *end, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nfa_calc_size)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mopen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mclose;
    Frag_T	*ଗଦା = ଖାଲି;
    Frag_T	*stackp = ଖାଲି;
    Frag_T	*ଗଦା_end = ଖାଲି;
    Frag_T	e1;
    Frag_T	e2;
    Frag_T	e;
    nfa_state_T	*s;
    nfa_state_T	*s1;
    nfa_state_T	*matchstate;
    nfa_state_T	*ret = ଖାଲି;

    ଯଦି (postfix == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

#define ଠେଲ_2(s)	    st_ଠେଲ((s), &stackp, ଗଦା_end)
#define କାଢ଼_2()	    st_କାଢ଼(&stackp, ଗଦା);		\
		    ଯଦି (stackp < ଗଦା)			\
		    {					\
			st_error(postfix, end, p);	\
			ଫେରନ୍ତୁ ଖାଲି;			\
		    }

    ଯଦି (nfa_calc_size == FALSE)
    {
	// Reuse the fragment stack across compilations, growing when needed;
	// it is freed in free_regexp_stuff().
	ଯଦି (nfa_ଗଦା == ଖାଲି || nfa_ଗଦା_len < nstate + 1)
	{
	    vim_free(nfa_ଗଦା);
	    nfa_ଗଦା = ALLOC_MULT(Frag_T, nstate + 1);
	    ଯଦି (nfa_ଗଦା == ଖାଲି)
	    {
		nfa_ଗଦା_len = 0;
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    nfa_ଗଦା_len = nstate + 1;
	}
	ଗଦା = nfa_ଗଦା;
	stackp = ଗଦା;
	ଗଦା_end = ଗଦା + (nstate + 1);
    }

    ପାଇଁ (p = postfix; p < end; ++p)
    {
	ବାଛ (*p)
	{
	ସ୍ଥିତି NFA_CONCAT:
	    // Concatenation.
	    // Pay attention: this operator does not exist in the r.e. itself
	    // (it is implicit, really).  It is added when r.e. is translated
	    // to postfix form in re2post().
	    ଯଦି (nfa_calc_size == TRUE)
	    {
		// nstate += 0;
		ଭାଙ୍ଗ;
	    }
	    e2 = କାଢ଼_2();
	    e1 = କାଢ଼_2();
	    patch(e1.out, e2.start);
	    ଠେଲ_2(frag(e1.start, e2.out));
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି NFA_OR:
	    // Alternation
	    ଯଦି (nfa_calc_size == TRUE)
	    {
		nstate++;
		ଭାଙ୍ଗ;
	    }
	    e2 = କାଢ଼_2();
	    e1 = କାଢ଼_2();
	    s = alloc_state(NFA_SPLIT, e1.start, e2.start);
	    ଯଦି (s == ଖାଲି)
		ଯାଅ theend;
	    ଠେଲ_2(frag(s, append(e1.out, e2.out)));
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି NFA_STAR:
	    // Zero or more, prefer more
	    ଯଦି (nfa_calc_size == TRUE)
	    {
		nstate++;
		ଭାଙ୍ଗ;
	    }
	    e = କାଢ଼_2();
	    s = alloc_state(NFA_SPLIT, e.start, ଖାଲି);
	    ଯଦି (s == ଖାଲି)
		ଯାଅ theend;
	    patch(e.out, s);
	    ଠେଲ_2(frag(s, list1(&s->out1)));
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି NFA_STAR_NONGREEDY:
	    // Zero or more, prefer zero
	    ଯଦି (nfa_calc_size == TRUE)
	    {
		nstate++;
		ଭାଙ୍ଗ;
	    }
	    e = କାଢ଼_2();
	    s = alloc_state(NFA_SPLIT, ଖାଲି, e.start);
	    ଯଦି (s == ଖାଲି)
		ଯାଅ theend;
	    patch(e.out, s);
	    ଠେଲ_2(frag(s, list1(&s->out)));
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି NFA_QUEST:
	    // one or zero atoms=> greedy match
	    ଯଦି (nfa_calc_size == TRUE)
	    {
		nstate++;
		ଭାଙ୍ଗ;
	    }
	    e = କାଢ଼_2();
	    s = alloc_state(NFA_SPLIT, e.start, ଖାଲି);
	    ଯଦି (s == ଖାଲି)
		ଯାଅ theend;
	    ଠେଲ_2(frag(s, append(e.out, list1(&s->out1))));
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି NFA_QUEST_NONGREEDY:
	    // zero or one atoms => non-greedy match
	    ଯଦି (nfa_calc_size == TRUE)
	    {
		nstate++;
		ଭାଙ୍ଗ;
	    }
	    e = କାଢ଼_2();
	    s = alloc_state(NFA_SPLIT, ଖାଲି, e.start);
	    ଯଦି (s == ଖାଲି)
		ଯାଅ theend;
	    ଠେଲ_2(frag(s, append(e.out, list1(&s->out))));
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି NFA_END_COLL:
	ସ୍ଥିତି NFA_END_NEG_COLL:
	    // On the stack is the sequence starting with NFA_START_COLL or
	    // NFA_START_NEG_COLL and all possible characters. Patch it to
	    // add the output to the start.
	    ଯଦି (nfa_calc_size == TRUE)
	    {
		nstate++;
		ଭାଙ୍ଗ;
	    }
	    e = କାଢ଼_2();
	    s = alloc_state(NFA_END_COLL, ଖାଲି, ଖାଲି);
	    ଯଦି (s == ଖାଲି)
		ଯାଅ theend;
	    patch(e.out, s);
	    e.start->out1 = s;
	    ଠେଲ_2(frag(e.start, list1(&s->out)));
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି NFA_RANGE:
	    // Before this are two characters, the low and high end of a
	    // range.  Turn them into two states with MIN and MAX.
	    ଯଦି (nfa_calc_size == TRUE)
	    {
		// nstate += 0;
		ଭାଙ୍ଗ;
	    }
	    e2 = କାଢ଼_2();
	    e1 = କାଢ଼_2();
	    e2.start->val = e2.start->c;
	    e2.start->c = NFA_RANGE_MAX;
	    e1.start->val = e1.start->c;
	    e1.start->c = NFA_RANGE_MIN;
	    patch(e1.out, e2.start);
	    ଠେଲ_2(frag(e1.start, e2.out));
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି NFA_EMPTY:
	    // 0-length, used in a repetition with max/min count of 0
	    ଯଦି (nfa_calc_size == TRUE)
	    {
		nstate++;
		ଭାଙ୍ଗ;
	    }
	    s = alloc_state(NFA_EMPTY, ଖାଲି, ଖାଲି);
	    ଯଦି (s == ଖାଲି)
		ଯାଅ theend;
	    ଠେଲ_2(frag(s, list1(&s->out)));
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି NFA_OPT_CHARS:
	  {
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା    n;

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
	    ଯଦି (nfa_calc_size == TRUE)
	    {
		nstate += n;
		ଭାଙ୍ଗ;
	    }
	    s = ଖାଲି; // avoid compiler warning
	    e1.out = ଖାଲି; // stores list with out1's
	    s1 = ଖାଲି; // previous NFA_SPLIT to connect to
	    ଯେତେବେଳେ (n-- > 0)
	    {
		e = କାଢ଼_2(); // get character
		s = alloc_state(NFA_SPLIT, e.start, ଖାଲି);
		ଯଦି (s == ଖାଲି)
		    ଯାଅ theend;
		ଯଦି (e1.out == ଖାଲି)
		    e1 = e;
		patch(e.out, s1);
		append(e1.out, list1(&s->out1));
		s1 = s;
	    }
	    ଠେଲ_2(frag(s, e1.out));
	    ଭାଙ୍ଗ;
	  }

	ସ୍ଥିତି NFA_PREV_ATOM_NO_WIDTH:
	ସ୍ଥିତି NFA_PREV_ATOM_NO_WIDTH_NEG:
	ସ୍ଥିତି NFA_PREV_ATOM_JUST_BEFORE:
	ସ୍ଥିତି NFA_PREV_ATOM_JUST_BEFORE_NEG:
	ସ୍ଥିତି NFA_PREV_ATOM_LIKE_PATTERN:
	  {
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା before = (*p == NFA_PREV_ATOM_JUST_BEFORE
				      || *p == NFA_PREV_ATOM_JUST_BEFORE_NEG);
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pattern = (*p == NFA_PREV_ATOM_LIKE_PATTERN);
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା start_state;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_state;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = 0;
	    nfa_state_T *zend;
	    nfa_state_T *skip;

	    ବାଛ (*p)
	    {
		ସ୍ଥିତି NFA_PREV_ATOM_NO_WIDTH:
		    start_state = NFA_START_INVISIBLE;
		    end_state = NFA_END_INVISIBLE;
		    ଭାଙ୍ଗ;
		ସ୍ଥିତି NFA_PREV_ATOM_NO_WIDTH_NEG:
		    start_state = NFA_START_INVISIBLE_NEG;
		    end_state = NFA_END_INVISIBLE_NEG;
		    ଭାଙ୍ଗ;
		ସ୍ଥିତି NFA_PREV_ATOM_JUST_BEFORE:
		    start_state = NFA_START_INVISIBLE_BEFORE;
		    end_state = NFA_END_INVISIBLE;
		    ଭାଙ୍ଗ;
		ସ୍ଥିତି NFA_PREV_ATOM_JUST_BEFORE_NEG:
		    start_state = NFA_START_INVISIBLE_BEFORE_NEG;
		    end_state = NFA_END_INVISIBLE_NEG;
		    ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ: // NFA_PREV_ATOM_LIKE_PATTERN:
		    start_state = NFA_START_PATTERN;
		    end_state = NFA_END_PATTERN;
		    ଭାଙ୍ଗ;
	    }

	    ଯଦି (before)
		n = *++p; // get the count

	    // The \@= operator: match the preceding atom with zero width.
	    // The \@! operator: no match for the preceding atom.
	    // The \@<= operator: match for the preceding atom.
	    // The \@<! operator: no match for the preceding atom.
	    // Surrounds the preceding atom with START_INVISIBLE and
	    // END_INVISIBLE, similarly to MOPEN.

	    ଯଦି (nfa_calc_size == TRUE)
	    {
		nstate += pattern ? 4 : 2;
		ଭାଙ୍ଗ;
	    }
	    e = କାଢ଼_2();
	    s1 = alloc_state(end_state, ଖାଲି, ଖାଲି);
	    ଯଦି (s1 == ଖାଲି)
		ଯାଅ theend;

	    s = alloc_state(start_state, e.start, s1);
	    ଯଦି (s == ଖାଲି)
		ଯାଅ theend;
	    ଯଦି (pattern)
	    {
		// NFA_ZEND -> NFA_END_PATTERN -> NFA_SKIP -> what follows.
		skip = alloc_state(NFA_SKIP, ଖାଲି, ଖାଲି);
		ଯଦି (skip == ଖାଲି)
		    ଯାଅ theend;
		zend = alloc_state(NFA_ZEND, s1, ଖାଲି);
		ଯଦି (zend == ଖାଲି)
		    ଯାଅ theend;
		s1->out= skip;
		patch(e.out, zend);
		ଠେଲ_2(frag(s, list1(&skip->out)));
	    }
	    ଅନ୍ୟଥା
	    {
		patch(e.out, s1);
		ଠେଲ_2(frag(s, list1(&s1->out)));
		ଯଦି (before)
		{
		    ଯଦି (n <= 0)
			// See if we can guess the maximum width, it avoids a
			// lot of pointless tries.
			n = nfa_max_width(e.start, 0);
		    s->val = n; // store the count
		}
	    }
	    ଭାଙ୍ଗ;
	  }

	ସ୍ଥିତି NFA_COMPOSING:	// char with composing char
#ଯଦି 0
	    // TODO
	    ଯଦି (regflags & RF_ICOMBINE)
	    {
		// use the base character only
	    }
#endif
	    // FALLTHROUGH

	ସ୍ଥିତି NFA_MOPEN:	// \( \) Submatch
	ସ୍ଥିତି NFA_MOPEN1:
	ସ୍ଥିତି NFA_MOPEN2:
	ସ୍ଥିତି NFA_MOPEN3:
	ସ୍ଥିତି NFA_MOPEN4:
	ସ୍ଥିତି NFA_MOPEN5:
	ସ୍ଥିତି NFA_MOPEN6:
	ସ୍ଥିତି NFA_MOPEN7:
	ସ୍ଥିତି NFA_MOPEN8:
	ସ୍ଥିତି NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	ସ୍ଥିତି NFA_ZOPEN:	// \z( \) Submatch
	ସ୍ଥିତି NFA_ZOPEN1:
	ସ୍ଥିତି NFA_ZOPEN2:
	ସ୍ଥିତି NFA_ZOPEN3:
	ସ୍ଥିତି NFA_ZOPEN4:
	ସ୍ଥିତି NFA_ZOPEN5:
	ସ୍ଥିତି NFA_ZOPEN6:
	ସ୍ଥିତି NFA_ZOPEN7:
	ସ୍ଥିତି NFA_ZOPEN8:
	ସ୍ଥିତି NFA_ZOPEN9:
#endif
	ସ୍ଥିତି NFA_NOPEN:	// \%( \) "Invisible Submatch"
	    ଯଦି (nfa_calc_size == TRUE)
	    {
		nstate += 2;
		ଭାଙ୍ଗ;
	    }

	    mopen = *p;
	    ବାଛ (*p)
	    {
		ସ୍ଥିତି NFA_NOPEN: mclose = NFA_NCLOSE; ଭାଙ୍ଗ;
#ifdef FEAT_SYN_HL
		ସ୍ଥିତି NFA_ZOPEN: mclose = NFA_ZCLOSE; ଭାଙ୍ଗ;
		ସ୍ଥିତି NFA_ZOPEN1: mclose = NFA_ZCLOSE1; ଭାଙ୍ଗ;
		ସ୍ଥିତି NFA_ZOPEN2: mclose = NFA_ZCLOSE2; ଭାଙ୍ଗ;
		ସ୍ଥିତି NFA_ZOPEN3: mclose = NFA_ZCLOSE3; ଭାଙ୍ଗ;
		ସ୍ଥିତି NFA_ZOPEN4: mclose = NFA_ZCLOSE4; ଭାଙ୍ଗ;
		ସ୍ଥିତି NFA_ZOPEN5: mclose = NFA_ZCLOSE5; ଭାଙ୍ଗ;
		ସ୍ଥିତି NFA_ZOPEN6: mclose = NFA_ZCLOSE6; ଭାଙ୍ଗ;
		ସ୍ଥିତି NFA_ZOPEN7: mclose = NFA_ZCLOSE7; ଭାଙ୍ଗ;
		ସ୍ଥିତି NFA_ZOPEN8: mclose = NFA_ZCLOSE8; ଭାଙ୍ଗ;
		ସ୍ଥିତି NFA_ZOPEN9: mclose = NFA_ZCLOSE9; ଭାଙ୍ଗ;
#endif
		ସ୍ଥିତି NFA_COMPOSING: mclose = NFA_END_COMPOSING; ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		    // NFA_MOPEN, NFA_MOPEN1 .. NFA_MOPEN9
		    mclose = *p + NSUBEXP;
		    ଭାଙ୍ଗ;
	    }

	    // Allow "NFA_MOPEN" as a valid postfix representation for
	    // the empty regexp "". In this case, the NFA will be
	    // NFA_MOPEN -> NFA_MCLOSE. Note that this also allows
	    // empty groups of parenthesis, and empty mbyte chars
	    ଯଦି (stackp == ଗଦା)
	    {
		s = alloc_state(mopen, ଖାଲି, ଖାଲି);
		ଯଦି (s == ଖାଲି)
		    ଯାଅ theend;
		s1 = alloc_state(mclose, ଖାଲି, ଖାଲି);
		ଯଦି (s1 == ଖାଲି)
		    ଯାଅ theend;
		patch(list1(&s->out), s1);
		ଠେଲ_2(frag(s, list1(&s1->out)));
		ଭାଙ୍ଗ;
	    }

	    // At least one node was emitted before NFA_MOPEN, so
	    // at least one node will be between NFA_MOPEN and NFA_MCLOSE
	    e = କାଢ଼_2();
	    s = alloc_state(mopen, e.start, ଖାଲି);   // `('
	    ଯଦି (s == ଖାଲି)
		ଯାଅ theend;

	    s1 = alloc_state(mclose, ଖାଲି, ଖାଲି);   // `)'
	    ଯଦି (s1 == ଖାଲି)
		ଯାଅ theend;
	    patch(e.out, s1);

	    ଯଦି (mopen == NFA_COMPOSING)
		// COMPOSING->out1 = END_COMPOSING
		patch(list1(&s->out1), s1);

	    ଠେଲ_2(frag(s, list1(&s1->out)));
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି NFA_BACKREF1:
	ସ୍ଥିତି NFA_BACKREF2:
	ସ୍ଥିତି NFA_BACKREF3:
	ସ୍ଥିତି NFA_BACKREF4:
	ସ୍ଥିତି NFA_BACKREF5:
	ସ୍ଥିତି NFA_BACKREF6:
	ସ୍ଥିତି NFA_BACKREF7:
	ସ୍ଥିତି NFA_BACKREF8:
	ସ୍ଥିତି NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	ସ୍ଥିତି NFA_ZREF1:
	ସ୍ଥିତି NFA_ZREF2:
	ସ୍ଥିତି NFA_ZREF3:
	ସ୍ଥିତି NFA_ZREF4:
	ସ୍ଥିତି NFA_ZREF5:
	ସ୍ଥିତି NFA_ZREF6:
	ସ୍ଥିତି NFA_ZREF7:
	ସ୍ଥିତି NFA_ZREF8:
	ସ୍ଥିତି NFA_ZREF9:
#endif
	    ଯଦି (nfa_calc_size == TRUE)
	    {
		nstate += 2;
		ଭାଙ୍ଗ;
	    }
	    s = alloc_state(*p, ଖାଲି, ଖାଲି);
	    ଯଦି (s == ଖାଲି)
		ଯାଅ theend;
	    s1 = alloc_state(NFA_SKIP, ଖାଲି, ଖାଲି);
	    ଯଦି (s1 == ଖାଲି)
		ଯାଅ theend;
	    patch(list1(&s->out), s1);
	    ଠେଲ_2(frag(s, list1(&s1->out)));
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି NFA_LNUM:
	ସ୍ଥିତି NFA_LNUM_GT:
	ସ୍ଥିତି NFA_LNUM_LT:
	ସ୍ଥିତି NFA_VCOL:
	ସ୍ଥିତି NFA_VCOL_GT:
	ସ୍ଥିତି NFA_VCOL_LT:
	ସ୍ଥିତି NFA_COL:
	ସ୍ଥିତି NFA_COL_GT:
	ସ୍ଥିତି NFA_COL_LT:
	ସ୍ଥିତି NFA_MARK:
	ସ୍ଥିତି NFA_MARK_GT:
	ସ୍ଥିତି NFA_MARK_LT:
	  {
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = *++p; // lnum, col or mark name

	    ଯଦି (nfa_calc_size == TRUE)
	    {
		nstate += 1;
		ଭାଙ୍ଗ;
	    }
	    s = alloc_state(p[-1], ଖାଲି, ଖାଲି);
	    ଯଦି (s == ଖାଲି)
		ଯାଅ theend;
	    s->val = n;
	    ଠେଲ_2(frag(s, list1(&s->out)));
	    ଭାଙ୍ଗ;
	  }

	ସ୍ଥିତି NFA_ZSTART:
	ସ୍ଥିତି NFA_ZEND:
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    // Operands
	    ଯଦି (nfa_calc_size == TRUE)
	    {
		nstate++;
		ଭାଙ୍ଗ;
	    }
	    s = alloc_state(*p, ଖାଲି, ଖାଲି);
	    ଯଦି (s == ଖାଲି)
		ଯାଅ theend;
	    ଠେଲ_2(frag(s, list1(&s->out)));
	    ଭାଙ୍ଗ;

	} // switch(*p)

    } // for (p = postfix; *p; ++p)

    ଯଦି (nfa_calc_size == TRUE)
    {
	nstate++;
	ଯାଅ theend;	// Return value when counting size is ignored anyway
    }

    e = କାଢ଼_2();
    ଯଦି (stackp != ଗଦା)
	EMSG_RET_NULL(_(e_nfa_regexp_while_converting_from_postfix_to_nfa_too_many_stats_ବାମ_on_ଗଦା));

    ଯଦି (istate >= nstate)
	EMSG_RET_NULL(_(e_nfa_regexp_not_enough_space_to_store_whole_nfa));

    matchstate = &state_ptr[istate++]; // the match state
    matchstate->c = NFA_MATCH;
    matchstate->out = matchstate->out1 = ଖାଲି;
    matchstate->id = 0;

    patch(e.out, matchstate);
    ret = e.start;

theend:
    ଫେରନ୍ତୁ ret;

#undef POP1
#undef PUSH1
#undef POP2
#undef PUSH2
#undef କାଢ଼_2
#undef ଠେଲ_2
}

/*
 * After building the NFA program, inspect it to add optimization hints.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
nfa_postprocess(nfa_regprog_T *prog)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;

    ପାଇଁ (i = 0; i < prog->nstate; ++i)
    {
	c = prog->state[i].c;
	ଯଦି (c == NFA_START_INVISIBLE
		|| c == NFA_START_INVISIBLE_NEG
		|| c == NFA_START_INVISIBLE_BEFORE
		|| c == NFA_START_INVISIBLE_BEFORE_NEG)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା directly;

	    // Do it directly when what follows is possibly the end of the
	    // match.
	    ଯଦି (match_follows(prog->state[i].out1->out, 0))
		directly = TRUE;
	    ଅନ୍ୟଥା
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ch_invisible = failure_chance(prog->state[i].out, 0);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ch_follows = failure_chance(prog->state[i].out1->out, 0);

		// Postpone when the invisible match is expensive or has a
		// lower chance of failing.
		ଯଦି (c == NFA_START_INVISIBLE_BEFORE
		     || c == NFA_START_INVISIBLE_BEFORE_NEG)
		{
		    // "before" matches are very expensive when
		    // unbounded, always prefer what follows then,
		    // unless what follows will always match.
		    // Otherwise strongly prefer what follows.
		    ଯଦି (prog->state[i].val <= 0 && ch_follows > 0)
			directly = FALSE;
		    ଅନ୍ୟଥା
			directly = ch_follows * 10 < ch_invisible;
		}
		ଅନ୍ୟଥା
		{
		    // normal invisible, first do the one with the
		    // highest failure chance
		    directly = ch_follows < ch_invisible;
		}
	    }
	    ଯଦି (directly)
		// switch to the _FIRST state
		++prog->state[i].c;
	}
    }
}

/////////////////////////////////////////////////////////////////
// NFA execution code.
/////////////////////////////////////////////////////////////////

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    in_use; // number of subexpr with useful info

    // When REG_MULTI is TRUE list.multi is used, otherwise list.line.
    ସଂଘ
    {
	ସଂରଚନା multipos
	{
	    linenr_T	start_lnum;
	    linenr_T	end_lnum;
	    colnr_T	start_col;
	    colnr_T	end_col;
	} multi[NSUBEXP];
	ସଂରଚନା linepos
	{
	    char_u	*start;
	    char_u	*end;
	} ଧାଡ଼ି[NSUBEXP];
    } list;
    colnr_T	orig_start_col;  // list.multi[0].start_col without \zs
} regsub_T;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    regsub_T	norm; // \( .. \) matches
#ifdef FEAT_SYN_HL
    regsub_T	synt; // \z( .. \) matches
#endif
} regsubs_T;

// nfa_pim_T stores a Postponed Invisible Match.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା nfa_pim_S nfa_pim_T;
ସଂରଚନା nfa_pim_S
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result;		// NFA_PIM_*, see below
    nfa_state_T	*state;		// the invisible match start state
    regsubs_T	subs;		// submatch info, only party used
    ସଂଘ
    {
	lpos_T	pos;
	char_u	*ptr;
    } end;			// where the match must end
};

// Values for done in nfa_pim_T.
#define NFA_PIM_UNUSED   0	// pim not used
#define NFA_PIM_TODO     1	// pim not done yet
#define NFA_PIM_MATCH    2	// pim executed, matches
#define NFA_PIM_NOMATCH  3	// pim executed, no match


// nfa_thread_T contains execution information of a NFA state
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    nfa_state_T	*state;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2;
    nfa_pim_T	pim;		// if pim.result != NFA_PIM_UNUSED: postponed
				// invisible match
    regsubs_T	subs;		// submatch info, only party used
} nfa_thread_T;

// nfa_list_T contains the alternative NFA execution states.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    nfa_thread_T    *t;		// allocated array of states
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    n;		// nr of states currently in "t"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    len;	// max nr of states in "t"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    id;		// ID of the list
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    has_pim;	// TRUE when any state has a PIM
} nfa_list_T;

#ifdef ENABLE_LOG
ସ୍ଥିର ଶୂନ୍ୟ log_subexpr(regsub_T *sub);

    ସ୍ଥିର ଶୂନ୍ୟ
log_subsexpr(regsubs_T *subs)
{
    log_subexpr(&subs->norm);
# ifdef FEAT_SYN_HL
    ଯଦି (rex.nfa_has_zsubexpr)
	log_subexpr(&subs->synt);
# endif
}

    ସ୍ଥିର ଶୂନ୍ୟ
log_subexpr(regsub_T *sub)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା j;

    ପାଇଁ (j = 0; j < sub->in_use; j++)
	ଯଦି (REG_MULTI)
	    fprintf(log_fd,
		    "*** group %d, start: c=%d, l=%d, end: c=%d, l=%d\n",
		    j,
		    sub->list.multi[j].start_col,
		    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)sub->list.multi[j].start_lnum,
		    sub->list.multi[j].end_col,
		    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)sub->list.multi[j].end_lnum);
	ଅନ୍ୟଥା
	{
	    ଅକ୍ଷର *s = (ଅକ୍ଷର *)sub->list.ଧାଡ଼ି[j].start;
	    ଅକ୍ଷର *e = (ଅକ୍ଷର *)sub->list.ଧାଡ଼ି[j].end;

	    fprintf(log_fd, "*** group %d, start: \"%s\", end: \"%s\"\n",
		    j,
		    s == ଖାଲି ? "NULL" : s,
		    e == ଖାଲି ? "NULL" : e);
	}
}

    ସ୍ଥିର ଅକ୍ଷର *
pim_info(nfa_pim_T *pim)
{
    ସ୍ଥିର ଅକ୍ଷର buf[30];

    ଯଦି (pim == ଖାଲି || pim->result == NFA_PIM_UNUSED)
	buf[0] = NUL;
    ଅନ୍ୟଥା
    {
	sprintf(buf, " PIM col %d", REG_MULTI ? (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)pim->end.pos.col
		: (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(pim->end.ptr - rex.ନିବେଶ));
    }
    ଫେରନ୍ତୁ buf;
}

#endif

// Used during execution: whether a match has been found.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    nfa_match;
#ifdef FEAT_RELTIME
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	   *nfa_timed_out;
#endif

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ copy_sub(regsub_T *to, regsub_T *from);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pim_equal(nfa_pim_T *one, nfa_pim_T *two);

/*
 * Copy postponed invisible match info from "from" to "to".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
copy_pim(nfa_pim_T *to, nfa_pim_T *from)
{
    to->result = from->result;
    to->state = from->state;
    copy_sub(&to->subs.norm, &from->subs.norm);
#ifdef FEAT_SYN_HL
    ଯଦି (rex.nfa_has_zsubexpr)
	copy_sub(&to->subs.synt, &from->subs.synt);
#endif
    to->end = from->end;
}

    ସ୍ଥିର ଶୂନ୍ୟ
clear_sub(regsub_T *sub)
{
    ଯଦି (REG_MULTI)
	// Use 0xff to set lnum to -1
	vim_memset(sub->list.multi, 0xff,
				  ଆକାର(ସଂରଚନା multipos) * rex.nfa_nsubexpr);
    ଅନ୍ୟଥା
	vim_memset(sub->list.ଧାଡ଼ି, 0,
				   ଆକାର(ସଂରଚନା linepos) * rex.nfa_nsubexpr);
    sub->in_use = 0;
}

/*
 * Copy the submatches from "from" to "to".
 */
    ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ
copy_sub(regsub_T *to, regsub_T *from)
{
    to->in_use = from->in_use;
    ଯଦି (from->in_use <= 0)
	ଫେରନ୍ତୁ;

    // Copy the match start and end positions.  addstate() calls this for
    // every state it adds, so it is very hot.  Patterns without capturing
    // groups only use subexpression zero (the whole match), so special-case
    // "in_use == 1" with a direct assignment to avoid the mch_memmove() call.
    ଯଦି (REG_MULTI)
    {
	ଯଦି (from->in_use == 1)
	    to->list.multi[0] = from->list.multi[0];
	ଅନ୍ୟଥା
	    mch_memmove(&to->list.multi[0],
		    &from->list.multi[0],
		    ଆକାର(ସଂରଚନା multipos) * from->in_use);
	to->orig_start_col = from->orig_start_col;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (from->in_use == 1)
	    to->list.ଧାଡ଼ି[0] = from->list.ଧାଡ଼ି[0];
	ଅନ୍ୟଥା
	    mch_memmove(&to->list.ଧାଡ଼ି[0],
		    &from->list.ଧାଡ଼ି[0],
		    ଆକାର(ସଂରଚନା linepos) * from->in_use);
    }
}

/*
 * Like copy_sub() but exclude the main match.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
copy_sub_off(regsub_T *to, regsub_T *from)
{
    ଯଦି (to->in_use < from->in_use)
	to->in_use = from->in_use;
    ଯଦି (from->in_use <= 1)
	ଫେରନ୍ତୁ;

    // Copy the match start and end positions.
    ଯଦି (REG_MULTI)
	mch_memmove(&to->list.multi[1],
		&from->list.multi[1],
		ଆକାର(ସଂରଚନା multipos) * (from->in_use - 1));
    ଅନ୍ୟଥା
	mch_memmove(&to->list.ଧାଡ଼ି[1],
		&from->list.ଧାଡ଼ି[1],
		ଆକାର(ସଂରଚନା linepos) * (from->in_use - 1));
}

/*
 * Like copy_sub() but only do the end of the main match if \ze is present.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
copy_ze_off(regsub_T *to, regsub_T *from)
{
    ଯଦି (!rex.nfa_has_zend)
	ଫେରନ୍ତୁ;

    ଯଦି (REG_MULTI)
    {
	ଯଦି (from->list.multi[0].end_lnum >= 0)
	{
	    to->list.multi[0].end_lnum = from->list.multi[0].end_lnum;
	    to->list.multi[0].end_col = from->list.multi[0].end_col;
	}
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (from->list.ଧାଡ଼ି[0].end != ଖାଲି)
	    to->list.ଧାଡ଼ି[0].end = from->list.ଧାଡ଼ି[0].end;
    }
}

/*
 * Return TRUE if "sub1" and "sub2" have the same start positions.
 * When using back-references also check the end position.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sub_equal(regsub_T *sub1, regsub_T *sub2)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;
    linenr_T	s1;
    linenr_T	s2;
    char_u	*sp1;
    char_u	*sp2;

    todo = sub1->in_use > sub2->in_use ? sub1->in_use : sub2->in_use;
    ଯଦି (REG_MULTI)
    {
	ପାଇଁ (i = 0; i < todo; ++i)
	{
	    ଯଦି (i < sub1->in_use)
		s1 = sub1->list.multi[i].start_lnum;
	    ଅନ୍ୟଥା
		s1 = -1;
	    ଯଦି (i < sub2->in_use)
		s2 = sub2->list.multi[i].start_lnum;
	    ଅନ୍ୟଥା
		s2 = -1;
	    ଯଦି (s1 != s2)
		ଫେରନ୍ତୁ FALSE;
	    ଯଦି (s1 != -1 && sub1->list.multi[i].start_col
					     != sub2->list.multi[i].start_col)
		ଫେରନ୍ତୁ FALSE;

	    ଯଦି (rex.nfa_has_backref)
	    {
		ଯଦି (i < sub1->in_use)
		    s1 = sub1->list.multi[i].end_lnum;
		ଅନ୍ୟଥା
		    s1 = -1;
		ଯଦି (i < sub2->in_use)
		    s2 = sub2->list.multi[i].end_lnum;
		ଅନ୍ୟଥା
		    s2 = -1;
		ଯଦି (s1 != s2)
		    ଫେରନ୍ତୁ FALSE;
		ଯଦି (s1 != -1 && sub1->list.multi[i].end_col
					       != sub2->list.multi[i].end_col)
		ଫେରନ୍ତୁ FALSE;
	    }
	}
    }
    ଅନ୍ୟଥା
    {
	ପାଇଁ (i = 0; i < todo; ++i)
	{
	    ଯଦି (i < sub1->in_use)
		sp1 = sub1->list.ଧାଡ଼ି[i].start;
	    ଅନ୍ୟଥା
		sp1 = ଖାଲି;
	    ଯଦି (i < sub2->in_use)
		sp2 = sub2->list.ଧାଡ଼ି[i].start;
	    ଅନ୍ୟଥା
		sp2 = ଖାଲି;
	    ଯଦି (sp1 != sp2)
		ଫେରନ୍ତୁ FALSE;
	    ଯଦି (rex.nfa_has_backref)
	    {
		ଯଦି (i < sub1->in_use)
		    sp1 = sub1->list.ଧାଡ଼ି[i].end;
		ଅନ୍ୟଥା
		    sp1 = ଖାଲି;
		ଯଦି (i < sub2->in_use)
		    sp2 = sub2->list.ଧାଡ଼ି[i].end;
		ଅନ୍ୟଥା
		    sp2 = ଖାଲି;
		ଯଦି (sp1 != sp2)
		    ଫେରନ୍ତୁ FALSE;
	    }
	}
    }

    ଫେରନ୍ତୁ TRUE;
}

#ifdef FEAT_RELTIME
/*
 * Check if we are past the time limit, if there is one.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
nfa_did_time_out(ଶୂନ୍ୟ)
{
    ଯଦି (*timeout_flag)
    {
	ଯଦି (nfa_timed_out != ଖାଲି)
	{
# ifdef FEAT_EVAL
	    ଯଦି (!*nfa_timed_out)
		ch_log(ଖାଲି, "NFA regexp timed out");
# endif
	    *nfa_timed_out = TRUE;
	}
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}
#endif

#ifdef ENABLE_LOG
    ସ୍ଥିର ଶୂନ୍ୟ
open_debug_log(ପୂର୍ଣ୍ଣସଂଖ୍ୟା result)
{
    log_fd = fopen(NFA_REGEXP_RUN_LOG, "a");
    ଯଦି (log_fd == ଖାଲି)
    {
	emsg(_(e_log_open_failed));
	log_fd = stderr;
    }

    fprintf(log_fd, "****************************\n");
    fprintf(log_fd, "FINISHED RUNNING nfa_regmatch() recursively\n");
    fprintf(log_fd, "MATCH = %s\n", result == TRUE ? "OK" : result == MAYBE
	    ? "MAYBE" : "FALSE");
    fprintf(log_fd, "****************************\n");
}

    ସ୍ଥିର ଶୂନ୍ୟ
report_state(ଅକ୍ଷର *action,
	     regsub_T *sub,
	     nfa_state_T *state,
	     ପୂର୍ଣ୍ଣସଂଖ୍ୟା lid,
	     nfa_pim_T *pim)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା col;

    ଯଦି (sub->in_use <= 0)
	col = -1;
    ଅନ୍ୟଥା ଯଦି (REG_MULTI)
	col = sub->list.multi[0].start_col;
    ଅନ୍ୟଥା
	col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(sub->list.ଧାଡ଼ି[0].start - rex.ଧାଡ଼ି);
    nfa_set_code(state->c);
    ଯଦି (log_fd == ଖାଲି)
	open_debug_log(MAYBE);

    fprintf(log_fd, "> %s state %d to list %d. char %d: %s (start col %d)%s\n",
	    action, abs(state->id), lid, state->c, code, col,
	    pim_info(pim));
}
#endif

/*
 * Return TRUE if the same state is already in list "l" with the same
 * positions as "subs".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_state_with_pos(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim)	// postponed match or NULL
{
    nfa_thread_T	*thread;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			i;

    ପାଇଁ (i = 0; i < l->n; ++i)
    {
	thread = &l->t[i];
	ଯଦି (thread->state->id == state->id
		&& sub_equal(&thread->subs.norm, &subs->norm)
#ifdef FEAT_SYN_HL
		&& (!rex.nfa_has_zsubexpr
				|| sub_equal(&thread->subs.synt, &subs->synt))
#endif
		&& pim_equal(&thread->pim, pim))
	    ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Return TRUE if "one" and "two" are equal.  That includes when both are not
 * set.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
pim_equal(nfa_pim_T *one, nfa_pim_T *two)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା one_unused = (one == ଖାଲି || one->result == NFA_PIM_UNUSED);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା two_unused = (two == ଖାଲି || two->result == NFA_PIM_UNUSED);

    ଯଦି (one_unused)
	// one is unused: equal when two is also unused
	ଫେରନ୍ତୁ two_unused;
    ଯଦି (two_unused)
	// one is used and two is not: not equal
	ଫେରନ୍ତୁ FALSE;
    // compare the state id
    ଯଦି (one->state->id != two->state->id)
	ଫେରନ୍ତୁ FALSE;
    // compare the position
    ଯଦି (REG_MULTI)
	ଫେରନ୍ତୁ one->end.pos.lnum == two->end.pos.lnum
	    && one->end.pos.col == two->end.pos.col;
    ଫେରନ୍ତୁ one->end.ptr == two->end.ptr;
}

/*
 * Return TRUE if "state" leads to a NFA_MATCH without advancing the input.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
match_follows(nfa_state_T *startstate, ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth)
{
    nfa_state_T	    *state = startstate;

    // avoid too much recursion
    ଯଦି (depth > 10)
	ଫେରନ୍ତୁ FALSE;

    ଯେତେବେଳେ (state != ଖାଲି)
    {
	ବାଛ (state->c)
	{
	    ସ୍ଥିତି NFA_MATCH:
	    ସ୍ଥିତି NFA_MCLOSE:
	    ସ୍ଥିତି NFA_END_INVISIBLE:
	    ସ୍ଥିତି NFA_END_INVISIBLE_NEG:
	    ସ୍ଥିତି NFA_END_PATTERN:
		ଫେରନ୍ତୁ TRUE;

	    ସ୍ଥିତି NFA_SPLIT:
		ଫେରନ୍ତୁ match_follows(state->out, depth + 1)
				     || match_follows(state->out1, depth + 1);

	    ସ୍ଥିତି NFA_START_INVISIBLE:
	    ସ୍ଥିତି NFA_START_INVISIBLE_FIRST:
	    ସ୍ଥିତି NFA_START_INVISIBLE_BEFORE:
	    ସ୍ଥିତି NFA_START_INVISIBLE_BEFORE_FIRST:
	    ସ୍ଥିତି NFA_START_INVISIBLE_NEG:
	    ସ୍ଥିତି NFA_START_INVISIBLE_NEG_FIRST:
	    ସ୍ଥିତି NFA_START_INVISIBLE_BEFORE_NEG:
	    ସ୍ଥିତି NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
	    ସ୍ଥିତି NFA_COMPOSING:
		// skip ahead to next state
		state = state->out1->out;
		ଜାରି;

	    ସ୍ଥିତି NFA_ANY:
	    ସ୍ଥିତି NFA_ANY_COMPOSING:
	    ସ୍ଥିତି NFA_IDENT:
	    ସ୍ଥିତି NFA_SIDENT:
	    ସ୍ଥିତି NFA_KWORD:
	    ସ୍ଥିତି NFA_SKWORD:
	    ସ୍ଥିତି NFA_FNAME:
	    ସ୍ଥିତି NFA_SFNAME:
	    ସ୍ଥିତି NFA_PRINT:
	    ସ୍ଥିତି NFA_SPRINT:
	    ସ୍ଥିତି NFA_WHITE:
	    ସ୍ଥିତି NFA_NWHITE:
	    ସ୍ଥିତି NFA_DIGIT:
	    ସ୍ଥିତି NFA_NDIGIT:
	    ସ୍ଥିତି NFA_HEX:
	    ସ୍ଥିତି NFA_NHEX:
	    ସ୍ଥିତି NFA_OCTAL:
	    ସ୍ଥିତି NFA_NOCTAL:
	    ସ୍ଥିତି NFA_WORD:
	    ସ୍ଥିତି NFA_NWORD:
	    ସ୍ଥିତି NFA_HEAD:
	    ସ୍ଥିତି NFA_NHEAD:
	    ସ୍ଥିତି NFA_ALPHA:
	    ସ୍ଥିତି NFA_NALPHA:
	    ସ୍ଥିତି NFA_LOWER:
	    ସ୍ଥିତି NFA_NLOWER:
	    ସ୍ଥିତି NFA_UPPER:
	    ସ୍ଥିତି NFA_NUPPER:
	    ସ୍ଥିତି NFA_LOWER_IC:
	    ସ୍ଥିତି NFA_NLOWER_IC:
	    ସ୍ଥିତି NFA_UPPER_IC:
	    ସ୍ଥିତି NFA_NUPPER_IC:
	    ସ୍ଥିତି NFA_START_COLL:
	    ସ୍ଥିତି NFA_START_NEG_COLL:
	    ସ୍ଥିତି NFA_NEWL:
		// state will advance input
		ଫେରନ୍ତୁ FALSE;

	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଯଦି (state->c > 0)
		    // state will advance input
		    ଫେରନ୍ତୁ FALSE;

		// Others: zero-width or possibly zero-width, might still find
		// a match at the same position, keep looking.
		ଭାଙ୍ଗ;
	}
	state = state->out;
    }
    ଫେରନ୍ତୁ FALSE;
}


/*
 * Return TRUE if "state" is already in list "l".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
state_in_list(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs)	// pointers to subexpressions
{
    ଯଦି (state->lastlist[nfa_ll_index] == l->id)
    {
	ଯଦି (!rex.nfa_has_backref || has_state_with_pos(l, state, subs, ଖାଲି))
	    ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

// Offset used for "off" by addstate_here().
#define ADDSTATE_HERE_OFFSET 10

/*
 * Add "state" and possibly what follows to state list ".".
 * Returns "subs_arg", possibly copied into temp_subs.
 * Returns NULL when recursiveness is too deep or timed out.
 */
    ସ୍ଥିର regsubs_T *
addstate(
    nfa_list_T		*l,	    // runtime state list
    nfa_state_T		*state,	    // state to update
    regsubs_T		*subs_arg,  // pointers to subexpressions
    nfa_pim_T		*pim,	    // postponed look-behind match
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			off_arg,    // byte offset, when -1 go to next line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			depth)	    // recursion depth
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			subidx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			off = off_arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			add_here = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			listindex = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			k;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			found = FALSE;
    nfa_thread_T	*thread;
    ସଂରଚନା multipos	save_multipos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			save_in_use;
    char_u		*save_ptr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			i;
    regsub_T		*sub;
    regsubs_T		*subs = subs_arg;
    ସ୍ଥିର regsubs_T	temp_subs;
#ifdef ENABLE_LOG
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			did_print = FALSE;
#endif

#ifdef FEAT_RELTIME
    ଯଦି (nfa_did_time_out())
	ଫେରନ୍ତୁ ଖାଲି;
#endif

    // This function is called recursively.  When the depth is too much we run
    // out of stack and crash, limit recursiveness here.
    ଯଦି (depth >= 5000 || subs == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (off_arg <= -ADDSTATE_HERE_OFFSET)
    {
	add_here = TRUE;
	off = 0;
	listindex = -(off_arg + ADDSTATE_HERE_OFFSET);
    }

    ବାଛ (state->c)
    {
	ସ୍ଥିତି NFA_NCLOSE:
	ସ୍ଥିତି NFA_MCLOSE:
	ସ୍ଥିତି NFA_MCLOSE1:
	ସ୍ଥିତି NFA_MCLOSE2:
	ସ୍ଥିତି NFA_MCLOSE3:
	ସ୍ଥିତି NFA_MCLOSE4:
	ସ୍ଥିତି NFA_MCLOSE5:
	ସ୍ଥିତି NFA_MCLOSE6:
	ସ୍ଥିତି NFA_MCLOSE7:
	ସ୍ଥିତି NFA_MCLOSE8:
	ସ୍ଥିତି NFA_MCLOSE9:
#ifdef FEAT_SYN_HL
	ସ୍ଥିତି NFA_ZCLOSE:
	ସ୍ଥିତି NFA_ZCLOSE1:
	ସ୍ଥିତି NFA_ZCLOSE2:
	ସ୍ଥିତି NFA_ZCLOSE3:
	ସ୍ଥିତି NFA_ZCLOSE4:
	ସ୍ଥିତି NFA_ZCLOSE5:
	ସ୍ଥିତି NFA_ZCLOSE6:
	ସ୍ଥିତି NFA_ZCLOSE7:
	ସ୍ଥିତି NFA_ZCLOSE8:
	ସ୍ଥିତି NFA_ZCLOSE9:
#endif
	ସ୍ଥିତି NFA_MOPEN:
	ସ୍ଥିତି NFA_ZEND:
	ସ୍ଥିତି NFA_SPLIT:
	ସ୍ଥିତି NFA_EMPTY:
	    // These nodes are not added themselves but their "out" and/or
	    // "out1" may be added below.
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି NFA_BOL:
	ସ୍ଥିତି NFA_BOF:
	    // "^" won't match past end-of-line, don't bother trying.
	    // Except when at the end of the line, or when we are going to the
	    // next line for a look-behind match.
	    ଯଦି (rex.ନିବେଶ > rex.ଧାଡ଼ି
		    && *rex.ନିବେଶ != NUL
		    && (nfa_endp == ଖାଲି
			|| !REG_MULTI
			|| rex.lnum == nfa_endp->se_u.pos.lnum))
		ଯାଅ skip_add;
	    // FALLTHROUGH

	ସ୍ଥିତି NFA_MOPEN1:
	ସ୍ଥିତି NFA_MOPEN2:
	ସ୍ଥିତି NFA_MOPEN3:
	ସ୍ଥିତି NFA_MOPEN4:
	ସ୍ଥିତି NFA_MOPEN5:
	ସ୍ଥିତି NFA_MOPEN6:
	ସ୍ଥିତି NFA_MOPEN7:
	ସ୍ଥିତି NFA_MOPEN8:
	ସ୍ଥିତି NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	ସ୍ଥିତି NFA_ZOPEN:
	ସ୍ଥିତି NFA_ZOPEN1:
	ସ୍ଥିତି NFA_ZOPEN2:
	ସ୍ଥିତି NFA_ZOPEN3:
	ସ୍ଥିତି NFA_ZOPEN4:
	ସ୍ଥିତି NFA_ZOPEN5:
	ସ୍ଥିତି NFA_ZOPEN6:
	ସ୍ଥିତି NFA_ZOPEN7:
	ସ୍ଥିତି NFA_ZOPEN8:
	ସ୍ଥିତି NFA_ZOPEN9:
#endif
	ସ୍ଥିତି NFA_NOPEN:
	ସ୍ଥିତି NFA_ZSTART:
	    // These nodes need to be added so that we can bail out when it
	    // was added to this list before at the same position to avoid an
	    // endless loop for "\(\)*"

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଯଦି (state->lastlist[nfa_ll_index] == l->id && state->c != NFA_SKIP)
	    {
		// This state is already in the list, don't add it again,
		// unless it is an MOPEN that is used for a backreference or
		// when there is a PIM. For NFA_MATCH check the position,
		// lower position is preferred.
		ଯଦି (!rex.nfa_has_backref && pim == ଖାଲି && !l->has_pim
						     && state->c != NFA_MATCH)
		{
		    // When called from addstate_here() do insert before
		    // existing states.
		    ଯଦି (add_here)
		    {
			ପାଇଁ (k = 0; k < l->n && k < listindex; ++k)
			    ଯଦି (l->t[k].state->id == state->id)
			    {
				found = TRUE;
				ଭାଙ୍ଗ;
			    }
		    }
		    ଯଦି (!add_here || found)
		    {
skip_add:
#ifdef ENABLE_LOG
			nfa_set_code(state->c);
			fprintf(log_fd, "> Not adding state %d to list %d. char %d: %s pim: %s has_pim: %d found: %d\n",
			    abs(state->id), l->id, state->c, code,
			    pim == ଖାଲି ? "NULL" : "yes", l->has_pim, found);
#endif
			ଫେରନ୍ତୁ subs;
		    }
		}

		// Do not add the state again when it exists with the same
		// positions.
		ଯଦି (has_state_with_pos(l, state, subs, pim))
		    ଯାଅ skip_add;
	    }

	    // When there are backreferences or PIMs the number of states may
	    // be (a lot) bigger than anticipated.
	    ଯଦି (l->n == l->len)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା		newlen = l->len * 3 / 2 + 50;
		size_t		newsize = newlen * ଆକାର(nfa_thread_T);
		nfa_thread_T	*newt;

		ଯଦି ((long)(newsize >> 10) >= p_mmp)
		{
		    emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		    ଫେରନ୍ତୁ ଖାଲି;
		}
		ଯଦି (subs != &temp_subs)
		{
		    // "subs" may point into the current array, need to make a
		    // copy before it becomes invalid.
		    copy_sub(&temp_subs.norm, &subs->norm);
#ifdef FEAT_SYN_HL
		    ଯଦି (rex.nfa_has_zsubexpr)
			copy_sub(&temp_subs.synt, &subs->synt);
#endif
		    subs = &temp_subs;
		}

		newt = vim_realloc(l->t, newsize);
		ଯଦି (newt == ଖାଲି)
		{
		    // out of memory
		    ଫେରନ୍ତୁ ଖାଲି;
		}
		l->t = newt;
		l->len = newlen;
	    }

	    // add the state to the list
	    state->lastlist[nfa_ll_index] = l->id;
	    thread = &l->t[l->n++];
	    thread->state = state;
	    ଯଦି (pim == ଖାଲି)
		thread->pim.result = NFA_PIM_UNUSED;
	    ଅନ୍ୟଥା
	    {
		copy_pim(&thread->pim, pim);
		l->has_pim = TRUE;
	    }
	    copy_sub(&thread->subs.norm, &subs->norm);
#ifdef FEAT_SYN_HL
	    ଯଦି (rex.nfa_has_zsubexpr)
		copy_sub(&thread->subs.synt, &subs->synt);
#endif
#ifdef ENABLE_LOG
	    report_state("Adding", &thread->subs.norm, state, l->id, pim);
	    did_print = TRUE;
#endif
    }

#ifdef ENABLE_LOG
    ଯଦି (!did_print)
	report_state("Processing", &subs->norm, state, l->id, pim);
#endif
    ବାଛ (state->c)
    {
	ସ୍ଥିତି NFA_MATCH:
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି NFA_SPLIT:
	    // order matters here
	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    subs = addstate(l, state->out1, subs, pim, off_arg, depth + 1);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି NFA_EMPTY:
	ସ୍ଥିତି NFA_NOPEN:
	ସ୍ଥିତି NFA_NCLOSE:
	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି NFA_MOPEN:
	ସ୍ଥିତି NFA_MOPEN1:
	ସ୍ଥିତି NFA_MOPEN2:
	ସ୍ଥିତି NFA_MOPEN3:
	ସ୍ଥିତି NFA_MOPEN4:
	ସ୍ଥିତି NFA_MOPEN5:
	ସ୍ଥିତି NFA_MOPEN6:
	ସ୍ଥିତି NFA_MOPEN7:
	ସ୍ଥିତି NFA_MOPEN8:
	ସ୍ଥିତି NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	ସ୍ଥିତି NFA_ZOPEN:
	ସ୍ଥିତି NFA_ZOPEN1:
	ସ୍ଥିତି NFA_ZOPEN2:
	ସ୍ଥିତି NFA_ZOPEN3:
	ସ୍ଥିତି NFA_ZOPEN4:
	ସ୍ଥିତି NFA_ZOPEN5:
	ସ୍ଥିତି NFA_ZOPEN6:
	ସ୍ଥିତି NFA_ZOPEN7:
	ସ୍ଥିତି NFA_ZOPEN8:
	ସ୍ଥିତି NFA_ZOPEN9:
#endif
	ସ୍ଥିତି NFA_ZSTART:
	    ଯଦି (state->c == NFA_ZSTART)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#ifdef FEAT_SYN_HL
	    ଅନ୍ୟଥା ଯଦି (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
	    {
		subidx = state->c - NFA_ZOPEN;
		sub = &subs->synt;
	    }
#endif
	    ଅନ୍ୟଥା
	    {
		subidx = state->c - NFA_MOPEN;
		sub = &subs->norm;
	    }

	    // avoid compiler warnings
	    save_ptr = ଖାଲି;
	    CLEAR_FIELD(save_multipos);

	    // Set the position (with "off" added) in the subexpression.  Save
	    // and restore it when it was in use.  Otherwise fill any gap.
	    ଯଦି (REG_MULTI)
	    {
		ଯଦି (subidx < sub->in_use)
		{
		    save_multipos = sub->list.multi[subidx];
		    save_in_use = -1;
		}
		ଅନ୍ୟଥା
		{
		    save_in_use = sub->in_use;
		    ପାଇଁ (i = sub->in_use; i < subidx; ++i)
		    {
			sub->list.multi[i].start_lnum = -1;
			sub->list.multi[i].end_lnum = -1;
		    }
		    sub->in_use = subidx + 1;
		}
		ଯଦି (off == -1)
		{
		    sub->list.multi[subidx].start_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].start_col = 0;
		}
		ଅନ୍ୟଥା
		{
		    sub->list.multi[subidx].start_lnum = rex.lnum;
		    sub->list.multi[subidx].start_col =
					 (colnr_T)(rex.ନିବେଶ - rex.ଧାଡ଼ି + off);
		}
		sub->list.multi[subidx].end_lnum = -1;
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (subidx < sub->in_use)
		{
		    save_ptr = sub->list.ଧାଡ଼ି[subidx].start;
		    save_in_use = -1;
		}
		ଅନ୍ୟଥା
		{
		    save_in_use = sub->in_use;
		    ପାଇଁ (i = sub->in_use; i < subidx; ++i)
		    {
			sub->list.ଧାଡ଼ି[i].start = ଖାଲି;
			sub->list.ଧାଡ଼ି[i].end = ଖାଲି;
		    }
		    sub->in_use = subidx + 1;
		}
		sub->list.ଧାଡ଼ି[subidx].start = rex.ନିବେଶ + off;
	    }

	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    ଯଦି (subs == ଖାଲି)
		ଭାଙ୍ଗ;
	    // "subs" may have changed, need to set "sub" again
#ifdef FEAT_SYN_HL
	    ଯଦି (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
		sub = &subs->synt;
	    ଅନ୍ୟଥା
#endif
		sub = &subs->norm;

	    ଯଦି (save_in_use == -1)
	    {
		ଯଦି (REG_MULTI)
		    sub->list.multi[subidx] = save_multipos;
		ଅନ୍ୟଥା
		    sub->list.ଧାଡ଼ି[subidx].start = save_ptr;
	    }
	    ଅନ୍ୟଥା
		sub->in_use = save_in_use;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି NFA_MCLOSE:
	    ଯଦି (rex.nfa_has_zend && (REG_MULTI
			? subs->norm.list.multi[0].end_lnum >= 0
			: subs->norm.list.ଧାଡ଼ି[0].end != ଖାଲି))
	    {
		// Do not overwrite the position set by \ze.
		subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
		ଭାଙ୍ଗ;
	    }
	    // FALLTHROUGH
	ସ୍ଥିତି NFA_MCLOSE1:
	ସ୍ଥିତି NFA_MCLOSE2:
	ସ୍ଥିତି NFA_MCLOSE3:
	ସ୍ଥିତି NFA_MCLOSE4:
	ସ୍ଥିତି NFA_MCLOSE5:
	ସ୍ଥିତି NFA_MCLOSE6:
	ସ୍ଥିତି NFA_MCLOSE7:
	ସ୍ଥିତି NFA_MCLOSE8:
	ସ୍ଥିତି NFA_MCLOSE9:
#ifdef FEAT_SYN_HL
	ସ୍ଥିତି NFA_ZCLOSE:
	ସ୍ଥିତି NFA_ZCLOSE1:
	ସ୍ଥିତି NFA_ZCLOSE2:
	ସ୍ଥିତି NFA_ZCLOSE3:
	ସ୍ଥିତି NFA_ZCLOSE4:
	ସ୍ଥିତି NFA_ZCLOSE5:
	ସ୍ଥିତି NFA_ZCLOSE6:
	ସ୍ଥିତି NFA_ZCLOSE7:
	ସ୍ଥିତି NFA_ZCLOSE8:
	ସ୍ଥିତି NFA_ZCLOSE9:
#endif
	ସ୍ଥିତି NFA_ZEND:
	    ଯଦି (state->c == NFA_ZEND)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#ifdef FEAT_SYN_HL
	    ଅନ୍ୟଥା ଯଦି (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
	    {
		subidx = state->c - NFA_ZCLOSE;
		sub = &subs->synt;
	    }
#endif
	    ଅନ୍ୟଥା
	    {
		subidx = state->c - NFA_MCLOSE;
		sub = &subs->norm;
	    }

	    // We don't fill in gaps here, there must have been an MOPEN that
	    // has done that.
	    save_in_use = sub->in_use;
	    ଯଦି (sub->in_use <= subidx)
		sub->in_use = subidx + 1;
	    ଯଦି (REG_MULTI)
	    {
		save_multipos = sub->list.multi[subidx];
		ଯଦି (off == -1)
		{
		    sub->list.multi[subidx].end_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].end_col = 0;
		}
		ଅନ୍ୟଥା
		{
		    sub->list.multi[subidx].end_lnum = rex.lnum;
		    sub->list.multi[subidx].end_col =
					  (colnr_T)(rex.ନିବେଶ - rex.ଧାଡ଼ି + off);
		}
		// avoid compiler warnings
		save_ptr = ଖାଲି;
	    }
	    ଅନ୍ୟଥା
	    {
		save_ptr = sub->list.ଧାଡ଼ି[subidx].end;
		sub->list.ଧାଡ଼ି[subidx].end = rex.ନିବେଶ + off;
		// avoid compiler warnings
		CLEAR_FIELD(save_multipos);
	    }

	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    ଯଦି (subs == ଖାଲି)
		ଭାଙ୍ଗ;
	    // "subs" may have changed, need to set "sub" again
#ifdef FEAT_SYN_HL
	    ଯଦି (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
		sub = &subs->synt;
	    ଅନ୍ୟଥା
#endif
		sub = &subs->norm;

	    ଯଦି (REG_MULTI)
		sub->list.multi[subidx] = save_multipos;
	    ଅନ୍ୟଥା
		sub->list.ଧାଡ଼ି[subidx].end = save_ptr;
	    sub->in_use = save_in_use;
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ subs;
}

/*
 * Like addstate(), but the new state(s) are put at position "*ip".
 * Used for zero-width matches, next state to use is the added one.
 * This makes sure the order of states to be tried does not change, which
 * matters for alternatives.
 */
    ସ୍ଥିର regsubs_T *
addstate_here(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim,   // postponed look-behind match
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			*ip)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା tlen = l->n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା listidx = *ip;
    regsubs_T *r;

    // First add the state(s) at the end, so that we know how many there are.
    // Pass the listidx as offset (avoids adding another argument to
    // addstate()).
    r = addstate(l, state, subs, pim, -listidx - ADDSTATE_HERE_OFFSET, 0);
    ଯଦି (r == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // when "*ip" was at the end of the list, nothing to do
    ଯଦି (listidx + 1 == tlen)
	ଫେରନ୍ତୁ r;

    // re-order to put the new state at the current position
    ଗଣନା_2 = l->n - tlen;
    ଯଦି (ଗଣନା_2 == 0)
	ଫେରନ୍ତୁ r; // no state got added
    ଯଦି (ଗଣନା_2 == 1)
    {
	// overwrite the current state
	l->t[listidx] = l->t[l->n - 1];
    }
    ଅନ୍ୟଥା ଯଦି (ଗଣନା_2 > 1)
    {
	ଯଦି (l->n + ଗଣନା_2 - 1 >= l->len)
	{
	    // not enough space to move the new states, reallocate the list
	    // and move the states to the right position
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    newlen = l->len * 3 / 2 + 50;
	    size_t	    newsize = newlen * ଆକାର(nfa_thread_T);
	    nfa_thread_T    *newl;

	    ଯଦି ((long)(newsize >> 10) >= p_mmp)
	    {
		emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    newl = alloc(newsize);
	    ଯଦି (newl == ଖାଲି)
		ଫେରନ୍ତୁ ଖାଲି;
	    l->len = newlen;
	    mch_memmove(&(newl[0]),
		    &(l->t[0]),
		    ଆକାର(nfa_thread_T) * listidx);
	    mch_memmove(&(newl[listidx]),
		    &(l->t[l->n - ଗଣନା_2]),
		    ଆକାର(nfa_thread_T) * ଗଣନା_2);
	    mch_memmove(&(newl[listidx + ଗଣନା_2]),
		    &(l->t[listidx + 1]),
		    ଆକାର(nfa_thread_T) * (l->n - ଗଣନା_2 - listidx - 1));
	    vim_free(l->t);
	    l->t = newl;
	}
	ଅନ୍ୟଥା
	{
	    // make space for new states, then move them from the
	    // end to the current position
	    mch_memmove(&(l->t[listidx + ଗଣନା_2]),
		    &(l->t[listidx + 1]),
		    ଆକାର(nfa_thread_T) * (l->n - listidx - 1));
	    mch_memmove(&(l->t[listidx]),
		    &(l->t[l->n - 1]),
		    ଆକାର(nfa_thread_T) * ଗଣନା_2);
	}
    }
    --l->n;
    *ip = listidx - 1;

    ଫେରନ୍ତୁ r;
}

/*
 * Check character class "class" against current character c.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_char_class(ପୂର୍ଣ୍ଣସଂଖ୍ୟା class, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ବାଛ (class)
    {
	ସ୍ଥିତି NFA_CLASS_ALNUM:
	    ଯଦି (c >= 1 && c < 128 && isalnum(c))
		ଫେରନ୍ତୁ OK;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_ALPHA:
	    ଯଦି (c >= 1 && c < 128 && isalpha(c))
		ଫେରନ୍ତୁ OK;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_BLANK:
	    ଯଦି (c == ' ' || c == '\t')
		ଫେରନ୍ତୁ OK;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_CNTRL:
	    ଯଦି (c >= 1 && c <= 127 && iscntrl(c))
		ଫେରନ୍ତୁ OK;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_DIGIT:
	    ଯଦି (VIM_ISDIGIT(c))
		ଫେରନ୍ତୁ OK;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_GRAPH:
	    ଯଦି (c >= 1 && c <= 127 && isgraph(c))
		ଫେରନ୍ତୁ OK;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_LOWER:
	    ଯଦି (MB_ISLOWER(c) && c != 170 && c != 186)
		ଫେରନ୍ତୁ OK;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_PRINT:
	    ଯଦି (vim_isprintc(c))
		ଫେରନ୍ତୁ OK;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_PUNCT:
	    ଯଦି (c >= 1 && c < 128 && ispunct(c))
		ଫେରନ୍ତୁ OK;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_SPACE:
	    ଯଦି ((c >= 9 && c <= 13) || (c == ' '))
		ଫେରନ୍ତୁ OK;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_UPPER:
	    ଯଦି (MB_ISUPPER(c))
		ଫେରନ୍ତୁ OK;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_XDIGIT:
	    ଯଦି (vim_isxdigit(c))
		ଫେରନ୍ତୁ OK;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_TAB:
	    ଯଦି (c == '\t')
		ଫେରନ୍ତୁ OK;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_RETURN:
	    ଯଦି (c == '\r')
		ଫେରନ୍ତୁ OK;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_BACKSPACE:
	    ଯଦି (c == '\b')
		ଫେରନ୍ତୁ OK;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_ESCAPE:
	    ଯଦି (c == '\033')
		ଫେରନ୍ତୁ OK;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_IDENT:
	    ଯଦି (vim_isIDc(c))
		ଫେରନ୍ତୁ OK;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_KEYWORD:
	    ଯଦି (reg_iswordc(c))
		ଫେରନ୍ତୁ OK;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି NFA_CLASS_FNAME:
	    ଯଦି (vim_isfilec(c))
		ଫେରନ୍ତୁ OK;
	    ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    // should not be here :P
	    siemsg(_(e_nfa_regexp_invalid_character_class_nr), class);
	    ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Check for a match with subexpression "subidx".
 * Return TRUE if it matches.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
match_backref(
    regsub_T	*sub,	    // pointers to subexpressions
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		subidx,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*bytelen)   // out: length of match in bytes
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    ଯଦି (sub->in_use <= subidx)
    {
retempty:
	// backref was not set, match an empty string
	*bytelen = 0;
	ଫେରନ୍ତୁ TRUE;
    }

    ଯଦି (REG_MULTI)
    {
	ଯଦି (sub->list.multi[subidx].start_lnum < 0
				       || sub->list.multi[subidx].end_lnum < 0)
	    ଯାଅ retempty;
	ଯଦି (sub->list.multi[subidx].start_lnum == rex.lnum
			       && sub->list.multi[subidx].end_lnum == rex.lnum)
	{
	    len = sub->list.multi[subidx].end_col
					  - sub->list.multi[subidx].start_col;
	    ଯଦି (cstrncmp(rex.ଧାଡ଼ି + sub->list.multi[subidx].start_col,
							 rex.ନିବେଶ, &len) == 0)
	    {
		*bytelen = len;
		ଫେରନ୍ତୁ TRUE;
	    }
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (match_with_backref(
			sub->list.multi[subidx].start_lnum,
			sub->list.multi[subidx].start_col,
			sub->list.multi[subidx].end_lnum,
			sub->list.multi[subidx].end_col,
			bytelen) == RA_MATCH)
		ଫେରନ୍ତୁ TRUE;
	}
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (sub->list.ଧାଡ଼ି[subidx].start == ଖାଲି
					|| sub->list.ଧାଡ଼ି[subidx].end == ଖାଲି)
	    ଯାଅ retempty;
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(sub->list.ଧାଡ଼ି[subidx].end - sub->list.ଧାଡ଼ି[subidx].start);
	ଯଦି (cstrncmp(sub->list.ଧାଡ଼ି[subidx].start, rex.ନିବେଶ, &len) == 0)
	{
	    *bytelen = len;
	    ଫେରନ୍ତୁ TRUE;
	}
    }
    ଫେରନ୍ତୁ FALSE;
}

#ifdef FEAT_SYN_HL

/*
 * Check for a match with \z subexpression "subidx".
 * Return TRUE if it matches.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
match_zref(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		subidx,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*bytelen)   // out: length of match in bytes
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    cleanup_zsubexpr();
    ଯଦି (re_extmatch_in == ଖାଲି || re_extmatch_in->matches[subidx] == ଖାଲି)
    {
	// backref was not set, match an empty string
	*bytelen = 0;
	ଫେରନ୍ତୁ TRUE;
    }

    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(re_extmatch_in->matches[subidx]);
    ଯଦି (cstrncmp(re_extmatch_in->matches[subidx], rex.ନିବେଶ, &len) == 0)
    {
	*bytelen = len;
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}
#endif

/*
 * Save list IDs for all NFA states of "prog" into "list".
 * Also reset the IDs to zero.
 * Only used for the recursive value lastlist[1].
 */
    ସ୍ଥିର ଶୂନ୍ୟ
nfa_save_listids(nfa_regprog_T *prog, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *list)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    i;
    nfa_state_T	    *p;

    // Order in the list is reverse, it's a bit faster that way.
    p = &prog->state[0];
    ପାଇଁ (i = prog->nstate; --i >= 0; )
    {
	list[i] = p->lastlist[1];
	p->lastlist[1] = 0;
	++p;
    }
}

/*
 * Restore list IDs from "list" to all NFA states.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
nfa_restore_listids(nfa_regprog_T *prog, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *list)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    i;
    nfa_state_T	    *p;

    p = &prog->state[0];
    ପାଇଁ (i = prog->nstate; --i >= 0; )
    {
	p->lastlist[1] = list[i];
	++p;
    }
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
nfa_re_ଅଙ୍କ_cmp(long_u val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା op, long_u pos)
{
    ଯଦି (op == 1) ଫେରନ୍ତୁ pos > val;
    ଯଦି (op == 2) ଫେରନ୍ତୁ pos < val;
    ଫେରନ୍ତୁ val == pos;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା nfa_regmatch(nfa_regprog_T *prog, nfa_state_T *start, regsubs_T *submatch, regsubs_T *m);

/*
 * Recursively call nfa_regmatch()
 * "pim" is NULL or contains info about a Postponed Invisible Match (start
 * position).
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
recursive_regmatch(
    nfa_state_T	    *state,
    nfa_pim_T	    *pim,
    nfa_regprog_T   *prog,
    regsubs_T	    *submatch,
    regsubs_T	    *m,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    **listids,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    *listids_len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_reginput_col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(rex.ନିବେଶ - rex.ଧାଡ଼ି);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_reglnum = rex.lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_nfa_match = nfa_match;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_nfa_listid = rex.nfa_listid;
    save_se_T   *save_nfa_endp = nfa_endp;
    save_se_T   endpos;
    save_se_T   *endposp = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		need_restore = FALSE;

    ଯଦି (pim != ଖାଲି)
    {
	// start at the position where the postponed match was
	ଯଦି (REG_MULTI)
	    rex.ନିବେଶ = rex.ଧାଡ଼ି + pim->end.pos.col;
	ଅନ୍ୟଥା
	    rex.ନିବେଶ = pim->end.ptr;
    }

    ଯଦି (state->c == NFA_START_INVISIBLE_BEFORE
	    || state->c == NFA_START_INVISIBLE_BEFORE_FIRST
	    || state->c == NFA_START_INVISIBLE_BEFORE_NEG
	    || state->c == NFA_START_INVISIBLE_BEFORE_NEG_FIRST)
    {
	// The recursive match must end at the current position. When "pim" is
	// not NULL it specifies the current position.
	endposp = &endpos;
	ଯଦି (REG_MULTI)
	{
	    ଯଦି (pim == ଖାଲି)
	    {
		endpos.se_u.pos.col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(rex.ନିବେଶ - rex.ଧାଡ଼ି);
		endpos.se_u.pos.lnum = rex.lnum;
	    }
	    ଅନ୍ୟଥା
		endpos.se_u.pos = pim->end.pos;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (pim == ଖାଲି)
		endpos.se_u.ptr = rex.ନିବେଶ;
	    ଅନ୍ୟଥା
		endpos.se_u.ptr = pim->end.ptr;
	}

	// Go back the specified number of bytes, or as far as the
	// start of the previous line, to try matching "\@<=" or
	// not matching "\@<!". This is very inefficient, limit the number of
	// bytes if possible.
	ଯଦି (state->val <= 0)
	{
	    ଯଦି (REG_MULTI)
	    {
		rex.ଧାଡ଼ି = reg_getline(--rex.lnum);
		ଯଦି (rex.ଧାଡ଼ି == ଖାଲି)
		    // can't go before the first line
		    rex.ଧାଡ଼ି = reg_getline(++rex.lnum);
	    }
	    rex.ନିବେଶ = rex.ଧାଡ଼ି;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (REG_MULTI && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(rex.ନିବେଶ - rex.ଧାଡ଼ି) < state->val)
	    {
		// Not enough bytes in this line, go to end of
		// previous line.
		rex.ଧାଡ଼ି = reg_getline(--rex.lnum);
		ଯଦି (rex.ଧାଡ଼ି == ଖାଲି)
		{
		    // can't go before the first line
		    rex.ଧାଡ଼ି = reg_getline(++rex.lnum);
		    rex.ନିବେଶ = rex.ଧାଡ଼ି;
		}
		ଅନ୍ୟଥା
		    rex.ନିବେଶ = rex.ଧାଡ଼ି + reg_getline_len(rex.lnum);
	    }
	    ଯଦି ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(rex.ନିବେଶ - rex.ଧାଡ଼ି) >= state->val)
	    {
		rex.ନିବେଶ -= state->val;
		ଯଦି (has_mbyte)
		    rex.ନିବେଶ -= mb_head_off(rex.ଧାଡ଼ି, rex.ନିବେଶ);
	    }
	    ଅନ୍ୟଥା
		rex.ନିବେଶ = rex.ଧାଡ଼ି;
	}
    }

#ifdef ENABLE_LOG
    ଯଦି (log_fd != stderr)
	fclose(log_fd);
    log_fd = ଖାଲି;
#endif
    // Have to clear the lastlist field of the NFA nodes, so that
    // nfa_regmatch() and addstate() can run properly after recursion.
    ଯଦି (nfa_ll_index == 1)
    {
	// Already calling nfa_regmatch() recursively.  Save the lastlist[1]
	// values and clear them.
	ଯଦି (*listids == ଖାଲି || *listids_len < prog->nstate)
	{
	    vim_free(*listids);
	    *listids = ALLOC_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, prog->nstate);
	    ଯଦି (*listids == ଖାଲି)
	    {
		emsg(_(e_nfa_regexp_could_not_allocate_memory_for_branch_traversal));
		ଫେରନ୍ତୁ 0;
	    }
	    *listids_len = prog->nstate;
	}
	nfa_save_listids(prog, *listids);
	need_restore = TRUE;
	// any value of rex.nfa_listid will do
    }
    ଅନ୍ୟଥା
    {
	// First recursive nfa_regmatch() call, switch to the second lastlist
	// entry.  Make sure rex.nfa_listid is different from a previous
	// recursive call, because some states may still have this ID.
	++nfa_ll_index;
	ଯଦି (rex.nfa_listid <= rex.nfa_alt_listid)
	    rex.nfa_listid = rex.nfa_alt_listid;
    }

    // Call nfa_regmatch() to check if the current concat matches at this
    // position. The concat ends with the node NFA_END_INVISIBLE
    nfa_endp = endposp;
    result = nfa_regmatch(prog, state->out, submatch, m);

    ଯଦି (need_restore)
	nfa_restore_listids(prog, *listids);
    ଅନ୍ୟଥା
    {
	--nfa_ll_index;
	rex.nfa_alt_listid = rex.nfa_listid;
    }

    // restore position in input text
    rex.lnum = save_reglnum;
    ଯଦି (REG_MULTI)
	rex.ଧାଡ଼ି = reg_getline(rex.lnum);
    rex.ନିବେଶ = rex.ଧାଡ଼ି + save_reginput_col;
    ଯଦି (result != NFA_TOO_EXPENSIVE)
    {
	nfa_match = save_nfa_match;
	rex.nfa_listid = save_nfa_listid;
    }
    nfa_endp = save_nfa_endp;

#ifdef ENABLE_LOG
    open_debug_log(result);
#endif

    ଫେରନ୍ତୁ result;
}

/*
 * Estimate the chance of a match with "state" failing.
 * empty match: 0
 * NFA_ANY: 1
 * specific character: 99
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
failure_chance(nfa_state_T *state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = state->c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା l, r;

    // detect looping
    ଯଦି (depth > 4)
	ଫେରନ୍ତୁ 1;

    ବାଛ (c)
    {
	ସ୍ଥିତି NFA_SPLIT:
	    ଯଦି (state->out->c == NFA_SPLIT || state->out1->c == NFA_SPLIT)
		// avoid recursive stuff
		ଫେରନ୍ତୁ 1;
	    // two alternatives, use the lowest failure chance
	    l = failure_chance(state->out, depth + 1);
	    r = failure_chance(state->out1, depth + 1);
	    ଫେରନ୍ତୁ l < r ? l : r;

	ସ୍ଥିତି NFA_ANY:
	    // matches anything, unlikely to fail
	    ଫେରନ୍ତୁ 1;

	ସ୍ଥିତି NFA_MATCH:
	ସ୍ଥିତି NFA_MCLOSE:
	ସ୍ଥିତି NFA_ANY_COMPOSING:
	    // empty match works always
	    ଫେରନ୍ତୁ 0;

	ସ୍ଥିତି NFA_START_INVISIBLE:
	ସ୍ଥିତି NFA_START_INVISIBLE_FIRST:
	ସ୍ଥିତି NFA_START_INVISIBLE_NEG:
	ସ୍ଥିତି NFA_START_INVISIBLE_NEG_FIRST:
	ସ୍ଥିତି NFA_START_INVISIBLE_BEFORE:
	ସ୍ଥିତି NFA_START_INVISIBLE_BEFORE_FIRST:
	ସ୍ଥିତି NFA_START_INVISIBLE_BEFORE_NEG:
	ସ୍ଥିତି NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
	ସ୍ଥିତି NFA_START_PATTERN:
	    // recursive regmatch is expensive, use low failure chance
	    ଫେରନ୍ତୁ 5;

	ସ୍ଥିତି NFA_BOL:
	ସ୍ଥିତି NFA_EOL:
	ସ୍ଥିତି NFA_BOF:
	ସ୍ଥିତି NFA_EOF:
	ସ୍ଥିତି NFA_NEWL:
	    ଫେରନ୍ତୁ 99;

	ସ୍ଥିତି NFA_BOW:
	ସ୍ଥିତି NFA_EOW:
	    ଫେରନ୍ତୁ 90;

	ସ୍ଥିତି NFA_MOPEN:
	ସ୍ଥିତି NFA_MOPEN1:
	ସ୍ଥିତି NFA_MOPEN2:
	ସ୍ଥିତି NFA_MOPEN3:
	ସ୍ଥିତି NFA_MOPEN4:
	ସ୍ଥିତି NFA_MOPEN5:
	ସ୍ଥିତି NFA_MOPEN6:
	ସ୍ଥିତି NFA_MOPEN7:
	ସ୍ଥିତି NFA_MOPEN8:
	ସ୍ଥିତି NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	ସ୍ଥିତି NFA_ZOPEN:
	ସ୍ଥିତି NFA_ZOPEN1:
	ସ୍ଥିତି NFA_ZOPEN2:
	ସ୍ଥିତି NFA_ZOPEN3:
	ସ୍ଥିତି NFA_ZOPEN4:
	ସ୍ଥିତି NFA_ZOPEN5:
	ସ୍ଥିତି NFA_ZOPEN6:
	ସ୍ଥିତି NFA_ZOPEN7:
	ସ୍ଥିତି NFA_ZOPEN8:
	ସ୍ଥିତି NFA_ZOPEN9:
	ସ୍ଥିତି NFA_ZCLOSE:
	ସ୍ଥିତି NFA_ZCLOSE1:
	ସ୍ଥିତି NFA_ZCLOSE2:
	ସ୍ଥିତି NFA_ZCLOSE3:
	ସ୍ଥିତି NFA_ZCLOSE4:
	ସ୍ଥିତି NFA_ZCLOSE5:
	ସ୍ଥିତି NFA_ZCLOSE6:
	ସ୍ଥିତି NFA_ZCLOSE7:
	ସ୍ଥିତି NFA_ZCLOSE8:
	ସ୍ଥିତି NFA_ZCLOSE9:
#endif
	ସ୍ଥିତି NFA_NOPEN:
	ସ୍ଥିତି NFA_MCLOSE1:
	ସ୍ଥିତି NFA_MCLOSE2:
	ସ୍ଥିତି NFA_MCLOSE3:
	ସ୍ଥିତି NFA_MCLOSE4:
	ସ୍ଥିତି NFA_MCLOSE5:
	ସ୍ଥିତି NFA_MCLOSE6:
	ସ୍ଥିତି NFA_MCLOSE7:
	ସ୍ଥିତି NFA_MCLOSE8:
	ସ୍ଥିତି NFA_MCLOSE9:
	ସ୍ଥିତି NFA_NCLOSE:
	    ଫେରନ୍ତୁ failure_chance(state->out, depth + 1);

	ସ୍ଥିତି NFA_BACKREF1:
	ସ୍ଥିତି NFA_BACKREF2:
	ସ୍ଥିତି NFA_BACKREF3:
	ସ୍ଥିତି NFA_BACKREF4:
	ସ୍ଥିତି NFA_BACKREF5:
	ସ୍ଥିତି NFA_BACKREF6:
	ସ୍ଥିତି NFA_BACKREF7:
	ସ୍ଥିତି NFA_BACKREF8:
	ସ୍ଥିତି NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	ସ୍ଥିତି NFA_ZREF1:
	ସ୍ଥିତି NFA_ZREF2:
	ସ୍ଥିତି NFA_ZREF3:
	ସ୍ଥିତି NFA_ZREF4:
	ସ୍ଥିତି NFA_ZREF5:
	ସ୍ଥିତି NFA_ZREF6:
	ସ୍ଥିତି NFA_ZREF7:
	ସ୍ଥିତି NFA_ZREF8:
	ସ୍ଥିତି NFA_ZREF9:
#endif
	    // backreferences don't match in many places
	    ଫେରନ୍ତୁ 94;

	ସ୍ଥିତି NFA_LNUM_GT:
	ସ୍ଥିତି NFA_LNUM_LT:
	ସ୍ଥିତି NFA_COL_GT:
	ସ୍ଥିତି NFA_COL_LT:
	ସ୍ଥିତି NFA_VCOL_GT:
	ସ୍ଥିତି NFA_VCOL_LT:
	ସ୍ଥିତି NFA_MARK_GT:
	ସ୍ଥିତି NFA_MARK_LT:
	ସ୍ଥିତି NFA_VISUAL:
	    // before/after positions don't match very often
	    ଫେରନ୍ତୁ 85;

	ସ୍ଥିତି NFA_LNUM:
	    ଫେରନ୍ତୁ 90;

	ସ୍ଥିତି NFA_CURSOR:
	ସ୍ଥିତି NFA_COL:
	ସ୍ଥିତି NFA_VCOL:
	ସ୍ଥିତି NFA_MARK:
	    // specific positions rarely match
	    ଫେରନ୍ତୁ 98;

	ସ୍ଥିତି NFA_COMPOSING:
	    ଫେରନ୍ତୁ 95;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଯଦି (c > 0)
		// character match fails often
		ଫେରନ୍ତୁ 95;
    }

    // something else, includes character classes
    ଫେରନ୍ତୁ 50;
}

/*
 * Skip until the char "c" we know a match must start with.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
skip_to_start(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, colnr_T *colp)
{
    char_u *s;

    // Used often, do some work to avoid call overhead.
    ଯଦି (!rex.reg_ic && !has_mbyte)
	s = vim_strbyte(rex.ଧାଡ଼ି + *colp, c);
    ଅନ୍ୟଥା
	s = cstrchr(rex.ଧାଡ଼ି + *colp, c);
    ଯଦି (s == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    *colp = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(s - rex.ଧାଡ଼ି);
    ଫେରନ୍ତୁ OK;
}

/*
 * Check for a match with match_text.
 * Called after skip_to_start() has found regstart.
 * Returns zero for no match, 1 for a match.
 */
    ସ୍ଥିର long
find_match_text(colnr_T *startcol, ପୂର୍ଣ୍ଣସଂଖ୍ୟା regstart, char_u *match_text)
{
    colnr_T col = *startcol;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c1, c2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len1, len2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    match;

    ପାଇଁ (;;)
    {
	match = TRUE;
	// Skip the input character: case-folding can change its byte length.
	len2 = MB_CPTR2LEN(rex.ଧାଡ଼ି + col);
	ପାଇଁ (len1 = 0; match_text[len1] != NUL; len1 += MB_CHAR2LEN(c1))
	{
	    c1 = PTR2CHAR(match_text + len1);
	    c2 = PTR2CHAR(rex.ଧାଡ଼ି + col + len2);
	    ଯଦି (c1 != c2 && (!rex.reg_ic || MB_CASEFOLD(c1) != MB_CASEFOLD(c2)))
	    {
		match = FALSE;
		ଭାଙ୍ଗ;
	    }
	    len2 += enc_utf8 ? utf_ptr2len(rex.ଧାଡ଼ି + col + len2)
							     : MB_CHAR2LEN(c2);
	}
	ଯଦି (match
		// check that no composing char follows
		&& !(enc_utf8
			  && utf_iscomposing(PTR2CHAR(rex.ଧାଡ଼ି + col + len2))))
	{
	    cleanup_subexpr();
	    ଯଦି (REG_MULTI)
	    {
		rex.reg_startpos[0].lnum = rex.lnum;
		rex.reg_startpos[0].col = col;
		rex.reg_endpos[0].lnum = rex.lnum;
		rex.reg_endpos[0].col = col + len2;
	    }
	    ଅନ୍ୟଥା
	    {
		rex.reg_startp[0] = rex.ଧାଡ଼ି + col;
		rex.reg_endp[0] = rex.ଧାଡ଼ି + col + len2;
	    }
	    *startcol = col;
	    ଫେରନ୍ତୁ 1L;
	}

	// Try finding regstart after the current match.
	col += MB_CPTR2LEN(rex.ଧାଡ଼ି + col); // skip the input character
	ଯଦି (skip_to_start(regstart, &col) == FAIL)
	    ଭାଙ୍ଗ;
    }

    *startcol = col;
    ଫେରନ୍ତୁ 0L;
}

/*
 * Check whether the composing characters at the current input position match
 * the NFA composing sub-expression that starts at "sta" (the first state
 * after NFA_COMPOSING).  "curc" is the base character and "clen" its byte
 * length.
 * Returns OK when it matches, FAIL otherwise.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
match_composing(nfa_state_T *sta, ପୂର୍ଣ୍ଣସଂଖ୍ୟା curc, ପୂର୍ଣ୍ଣସଂଖ୍ୟା clen)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mc = curc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cchars[MAX_MCO];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ccount = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		j;

    ଯଦି (utf_iscomposing(sta->c))
	// Only match composing character(s), ignore base character.  Used
	// for ".{composing}" and "{composing}" (no preceding character).
	len += mb_char2len(mc);

    ଯଦି (rex.reg_icombine && len == 0)
	// If \Z was present, then ignore composing characters.  When
	// ignoring the base character this always matches.
	ଫେରନ୍ତୁ sta->c == curc ? OK : FAIL;

    // Check base character matches first, unless ignored.
    ଯଦି (len == 0 && mc != sta->c)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (len == 0)
    {
	len += mb_char2len(mc);
	sta = sta->out;
    }

    // We don't care about the order of composing characters.  Get them into
    // cchars[] first.
    ଯେତେବେଳେ (len < clen)
    {
	mc = mb_ptr2char(rex.ନିବେଶ + len);
	cchars[ccount++] = mc;
	len += mb_char2len(mc);
	ଯଦି (ccount == MAX_MCO)
	    ଭାଙ୍ଗ;
    }

    // Check that each composing char in the pattern matches a composing char
    // in the text.  We do not check if all composing chars are matched.
    ଯେତେବେଳେ (sta->c != NFA_END_COMPOSING)
    {
	ପାଇଁ (j = 0; j < ccount; ++j)
	    ଯଦି (cchars[j] == sta->c)
		ଭାଙ୍ଗ;
	ଯଦି (j == ccount)
	    ଫେରନ୍ତୁ FAIL;
	sta = sta->out;
    }

    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
nfa_regmatch(
    nfa_regprog_T	*prog,
    nfa_state_T		*start,
    regsubs_T		*submatch,
    regsubs_T		*m)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result = FALSE;
    size_t	size = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flag = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		go_to_nextline = FALSE;
    nfa_thread_T *t;
    nfa_list_T	list[2];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		listidx;
    nfa_list_T	*thislist;
    nfa_list_T	*nextlist;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*listids = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		listids_len = 0;
    nfa_state_T *add_state;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		add_here;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		add_ଗଣନା;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		add_off = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		toplevel = start->c == NFA_MOPEN;
    regsubs_T	*r;
#ifdef NFA_REGEXP_DEBUG_LOG
    FILE	*debug;
#endif

    // Some patterns may take a long time to match, especially when using
    // recursive_regmatch(). Allow interrupting them with CTRL-C.
    fast_breakcheck();
    ଯଦି (got_int)
	ଫେରନ୍ତୁ FALSE;
#ifdef FEAT_RELTIME
    ଯଦି (nfa_did_time_out())
	ଫେରନ୍ତୁ FALSE;
#endif

#ifdef NFA_REGEXP_DEBUG_LOG
    debug = fopen(NFA_REGEXP_DEBUG_LOG, "a");
    ଯଦି (debug == ଖାଲି)
    {
	semsg("(NFA) COULD NOT OPEN %s!", NFA_REGEXP_DEBUG_LOG);
	ଫେରନ୍ତୁ FALSE;
    }
#endif
    nfa_match = FALSE;

    // Allocate memory for the lists of nodes.
    size = (prog->nstate + 1) * ଆକାର(nfa_thread_T);

    // Reuse cached list buffers from prog when available (top-level call).
    // Recursive calls must allocate their own buffers.
    ଯଦି (toplevel && prog->listbuf[0] != ଖାଲି)
    {
	list[0].t = (nfa_thread_T *)prog->listbuf[0];
	list[1].t = (nfa_thread_T *)prog->listbuf[1];
	prog->listbuf[0] = ଖାଲି;
	prog->listbuf[1] = ଖାଲି;
    }
    ଅନ୍ୟଥା
    {
	list[0].t = alloc(size);
	list[1].t = alloc(size);
    }
    list[0].len = prog->nstate + 1;
    list[1].len = prog->nstate + 1;
    ଯଦି (list[0].t == ଖାଲି || list[1].t == ଖାଲି)
	ଯାଅ theend;

#ifdef ENABLE_LOG
    log_fd = fopen(NFA_REGEXP_RUN_LOG, "a");
    ଯଦି (log_fd == ଖାଲି)
    {
	emsg(_(e_log_open_failed));
	log_fd = stderr;
    }
    fprintf(log_fd, "**********************************\n");
    nfa_set_code(start->c);
    fprintf(log_fd, " RUNNING nfa_regmatch() starting with state %d, code %s\n",
    abs(start->id), code);
    fprintf(log_fd, "**********************************\n");
#endif

    thislist = &list[0];
    thislist->n = 0;
    thislist->has_pim = FALSE;
    nextlist = &list[1];
    nextlist->n = 0;
    nextlist->has_pim = FALSE;
#ifdef ENABLE_LOG
    fprintf(log_fd, "(---) STARTSTATE first\n");
#endif
    thislist->id = rex.nfa_listid + 1;

    // Inline optimized code for addstate(thislist, start, m, 0) if we know
    // it's the first MOPEN.
    ଯଦି (toplevel)
    {
	ଯଦି (REG_MULTI)
	{
	    m->norm.list.multi[0].start_lnum = rex.lnum;
	    m->norm.list.multi[0].start_col = (colnr_T)(rex.ନିବେଶ - rex.ଧାଡ଼ି);
	    m->norm.orig_start_col = m->norm.list.multi[0].start_col;
	}
	ଅନ୍ୟଥା
	    m->norm.list.ଧାଡ଼ି[0].start = rex.ନିବେଶ;
	m->norm.in_use = 1;
	r = addstate(thislist, start->out, m, ଖାଲି, 0, 0);
    }
    ଅନ୍ୟଥା
	r = addstate(thislist, start, m, ଖାଲି, 0, 0);
    ଯଦି (r == ଖାଲି)
    {
	nfa_match = NFA_TOO_EXPENSIVE;
	ଯାଅ theend;
    }

#define	ADD_STATE_IF_MATCH(state)	\
    ଯଦି (result)				\
    {					\
	add_state = state->out;		\
	add_off = clen;			\
    }

    /*
     * Run for each character.
     */
    ପାଇଁ (;;)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	curc;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	clen;

	ଯଦି (has_mbyte)
	{
	    // Fast path for an ASCII byte not followed by a composing
	    // character, matching the check in utfc_ptr2len().  Avoids two
	    // indirect calls for the common case.
	    ଯଦି (rex.ନିବେଶ[0] != NUL && rex.ନିବେଶ[0] < 0x80 && rex.ନିବେଶ[1] < 0x80)
	    {
		curc = rex.ନିବେଶ[0];
		clen = 1;
	    }
	    ଅନ୍ୟଥା
	    {
		curc = (*mb_ptr2char)(rex.ନିବେଶ);
		clen = (*mb_ptr2len)(rex.ନିବେଶ);
	    }
	}
	ଅନ୍ୟଥା
	{
	    curc = *rex.ନିବେଶ;
	    clen = 1;
	}
	ଯଦି (curc == NUL)
	{
	    clen = 0;
	    go_to_nextline = FALSE;
	}

	// swap lists
	thislist = &list[flag];
	nextlist = &list[flag ^= 1];
	nextlist->n = 0;	    // clear nextlist
	nextlist->has_pim = FALSE;
	++rex.nfa_listid;
	ଯଦି (prog->re_engine == AUTOMATIC_ENGINE
		&& (rex.nfa_listid >= NFA_MAX_STATES
#ifdef FEAT_EVAL
		    || nfa_fail_for_testing
#endif
		    ))
	{
	    // too many states, retry with old engine
	    nfa_match = NFA_TOO_EXPENSIVE;
	    ଯାଅ theend;
	}

	thislist->id = rex.nfa_listid;
	nextlist->id = rex.nfa_listid + 1;

#ifdef ENABLE_LOG
	fprintf(log_fd, "------------------------------------------\n");
	fprintf(log_fd, ">>> Reginput is \"%s\"\n", rex.ନିବେଶ);
	fprintf(log_fd, ">>> Advanced one character... Current char is %c (code %d) \n", curc, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curc);
	fprintf(log_fd, ">>> Thislist has %d states available: ", thislist->n);
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	    ପାଇଁ (i = 0; i < thislist->n; i++)
		fprintf(log_fd, "%d  ", abs(thislist->t[i].state->id));
	}
	fprintf(log_fd, "\n");
#endif

#ifdef NFA_REGEXP_DEBUG_LOG
	fprintf(debug, "\n-------------------\n");
#endif
	/*
	 * If the state lists are empty we can stop.
	 */
	ଯଦି (thislist->n == 0)
	    ଭାଙ୍ଗ;

	// compute nextlist
	ପାଇଁ (listidx = 0; listidx < thislist->n; ++listidx)
	{
	    // If the list gets very long there probably is something wrong.
	    // At least allow interrupting with CTRL-C.  Only check once in a
	    // while, calling ui_breakcheck() for every state is too slow.
	    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	breakcheck_ଗଣନା = 0;  // using "static" makes it faster
	    ଯଦି (unlikely(++breakcheck_ଗଣନା >= 100000))
	    {
		ui_breakcheck();
		breakcheck_ଗଣନା = 0;
	    }
	    ଯଦି (got_int)
		ଭାଙ୍ଗ;
#ifdef FEAT_RELTIME
	    ଯଦି (nfa_did_time_out())
		ଭାଙ୍ଗ;
#endif
	    t = &thislist->t[listidx];

#ifdef NFA_REGEXP_DEBUG_LOG
	    nfa_set_code(t->state->c);
	    fprintf(debug, "%s, ", code);
#endif
#ifdef ENABLE_LOG
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା col;

		ଯଦି (t->subs.norm.in_use <= 0)
		    col = -1;
		ଅନ୍ୟଥା ଯଦି (REG_MULTI)
		    col = t->subs.norm.list.multi[0].start_col;
		ଅନ୍ୟଥା
		    col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(t->subs.norm.list.ଧାଡ଼ି[0].start - rex.ଧାଡ଼ି);
		nfa_set_code(t->state->c);
		fprintf(log_fd, "(%d) char %d %s (start col %d)%s... \n",
			abs(t->state->id), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)t->state->c, code, col,
			pim_info(&t->pim));
	    }
#endif

	    /*
	     * Handle the possible codes of the current state.
	     * The most important is NFA_MATCH.
	     */
	    add_state = ଖାଲି;
	    add_here = FALSE;
	    add_ଗଣନା = 0;
	    ବାଛ (t->state->c)
	    {
	    ସ୍ଥିତି NFA_MATCH:
	      {
		// If the match is not at the start of the line, ends before a
		// composing characters and rex.reg_icombine is not set, that
		// is not really a match.
		ଯଦି (enc_utf8 && !rex.reg_icombine
			     && rex.ନିବେଶ != rex.ଧାଡ଼ି && utf_iscomposing(curc))
		    ଭାଙ୍ଗ;

		nfa_match = TRUE;
		copy_sub(&submatch->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		ଯଦି (rex.nfa_has_zsubexpr)
		    copy_sub(&submatch->synt, &t->subs.synt);
#endif
#ifdef ENABLE_LOG
		log_subsexpr(&t->subs);
#endif
		// Found the left-most longest match, do not look at any other
		// states at this position.  When the list of states is going
		// to be empty quit without advancing, so that "rex.input" is
		// correct.
		ଯଦି (nextlist->n == 0)
		    clen = 0;
		ଯାଅ nextchar;
	      }

	    ସ୍ଥିତି NFA_END_INVISIBLE:
	    ସ୍ଥିତି NFA_END_INVISIBLE_NEG:
	    ସ୍ଥିତି NFA_END_PATTERN:
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
#ifdef ENABLE_LOG
		ଯଦି (nfa_endp != ଖାଲି)
		{
		    ଯଦି (REG_MULTI)
			fprintf(log_fd, "Current lnum: %d, endp lnum: %d; current col: %d, endp col: %d\n",
				(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)rex.lnum,
				(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)nfa_endp->se_u.pos.lnum,
				(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(rex.ନିବେଶ - rex.ଧାଡ଼ି),
				nfa_endp->se_u.pos.col);
		    ଅନ୍ୟଥା
			fprintf(log_fd, "Current col: %d, endp col: %d\n",
				(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(rex.ନିବେଶ - rex.ଧାଡ଼ି),
				(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(nfa_endp->se_u.ptr - rex.ନିବେଶ));
		}
#endif
		// If "nfa_endp" is set it's only a match if it ends at
		// "nfa_endp"
		ଯଦି (nfa_endp != ଖାଲି && (REG_MULTI
			? (rex.lnum != nfa_endp->se_u.pos.lnum
			    || (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(rex.ନିବେଶ - rex.ଧାଡ଼ି)
						!= nfa_endp->se_u.pos.col)
			: rex.ନିବେଶ != nfa_endp->se_u.ptr))
		    ଭାଙ୍ଗ;

		// do not set submatches for \@!
		ଯଦି (t->state->c != NFA_END_INVISIBLE_NEG)
		{
		    copy_sub(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		    ଯଦି (rex.nfa_has_zsubexpr)
			copy_sub(&m->synt, &t->subs.synt);
#endif
		}
#ifdef ENABLE_LOG
		fprintf(log_fd, "Match found:\n");
		log_subsexpr(m);
#endif
		nfa_match = TRUE;
		// See comment above at "goto nextchar".
		ଯଦି (nextlist->n == 0)
		    clen = 0;
		ଯାଅ nextchar;

	    ସ୍ଥିତି NFA_START_INVISIBLE:
	    ସ୍ଥିତି NFA_START_INVISIBLE_FIRST:
	    ସ୍ଥିତି NFA_START_INVISIBLE_NEG:
	    ସ୍ଥିତି NFA_START_INVISIBLE_NEG_FIRST:
	    ସ୍ଥିତି NFA_START_INVISIBLE_BEFORE:
	    ସ୍ଥିତି NFA_START_INVISIBLE_BEFORE_FIRST:
	    ସ୍ଥିତି NFA_START_INVISIBLE_BEFORE_NEG:
	    ସ୍ଥିତି NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
		{
#ifdef ENABLE_LOG
		    fprintf(log_fd, "Failure chance invisible: %d, what follows: %d\n",
			    failure_chance(t->state->out, 0),
			    failure_chance(t->state->out1->out, 0));
#endif
		    // Do it directly if there already is a PIM or when
		    // nfa_postprocess() detected it will work better.
		    ଯଦି (t->pim.result != NFA_PIM_UNUSED
			 || t->state->c == NFA_START_INVISIBLE_FIRST
			 || t->state->c == NFA_START_INVISIBLE_NEG_FIRST
			 || t->state->c == NFA_START_INVISIBLE_BEFORE_FIRST
			 || t->state->c == NFA_START_INVISIBLE_BEFORE_NEG_FIRST)
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_use = m->norm.in_use;

			// Copy submatch info for the recursive call, opposite
			// of what happens on success below.
			copy_sub_off(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
			ଯଦି (rex.nfa_has_zsubexpr)
			    copy_sub_off(&m->synt, &t->subs.synt);
#endif

			/*
			 * First try matching the invisible match, then what
			 * follows.
			 */
			result = recursive_regmatch(t->state, ଖାଲି, prog,
					  submatch, m, &listids, &listids_len);
			ଯଦି (result == NFA_TOO_EXPENSIVE)
			{
			    nfa_match = result;
			    ଯାଅ theend;
			}

			// for \@! and \@<! it is a match when the result is
			// FALSE
			ଯଦି (result != (t->state->c == NFA_START_INVISIBLE_NEG
			       || t->state->c == NFA_START_INVISIBLE_NEG_FIRST
			       || t->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			       || t->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&t->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
			    ଯଦି (rex.nfa_has_zsubexpr)
				copy_sub_off(&t->subs.synt, &m->synt);
#endif
			    // If the pattern has \ze and it matched in the
			    // sub pattern, use it.
			    copy_ze_off(&t->subs.norm, &m->norm);

			    // t->state->out1 is the corresponding
			    // END_INVISIBLE node; Add its out to the current
			    // list (zero-width match).
			    add_here = TRUE;
			    add_state = t->state->out1->out;
			}
			m->norm.in_use = in_use;
		    }
		    ଅନ୍ୟଥା
		    {
			nfa_pim_T pim;

			/*
			 * First try matching what follows.  Only if a match
			 * is found verify the invisible match matches.  Add a
			 * nfa_pim_T to the following states, it contains info
			 * about the invisible match.
			 */
			pim.state = t->state;
			pim.result = NFA_PIM_TODO;
			pim.subs.norm.in_use = 0;
#ifdef FEAT_SYN_HL
			pim.subs.synt.in_use = 0;
#endif
			ଯଦି (REG_MULTI)
			{
			    pim.end.pos.col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(rex.ନିବେଶ - rex.ଧାଡ଼ି);
			    pim.end.pos.lnum = rex.lnum;
			}
			ଅନ୍ୟଥା
			    pim.end.ptr = rex.ନିବେଶ;

			// t->state->out1 is the corresponding END_INVISIBLE
			// node; Add its out to the current list (zero-width
			// match).
			ଯଦି (addstate_here(thislist, t->state->out1->out,
					     &t->subs, &pim, &listidx) == ଖାଲି)
			{
			    nfa_match = NFA_TOO_EXPENSIVE;
			    ଯାଅ theend;
			}
		    }
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_START_PATTERN:
	      {
		nfa_state_T *skip = ଖାଲି;
#ifdef ENABLE_LOG
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    skip_lid = 0;
#endif

		// There is no point in trying to match the pattern if the
		// output state is not going to be added to the list.
		ଯଦି (state_in_list(nextlist, t->state->out1->out, &t->subs))
		{
		    skip = t->state->out1->out;
#ifdef ENABLE_LOG
		    skip_lid = nextlist->id;
#endif
		}
		ଅନ୍ୟଥା ଯଦି (state_in_list(nextlist,
					  t->state->out1->out->out, &t->subs))
		{
		    skip = t->state->out1->out->out;
#ifdef ENABLE_LOG
		    skip_lid = nextlist->id;
#endif
		}
		ଅନ୍ୟଥା ଯଦି (state_in_list(thislist,
					  t->state->out1->out->out, &t->subs))
		{
		    skip = t->state->out1->out->out;
#ifdef ENABLE_LOG
		    skip_lid = thislist->id;
#endif
		}
		ଯଦି (skip != ଖାଲି)
		{
#ifdef ENABLE_LOG
		    nfa_set_code(skip->c);
		    fprintf(log_fd, "> Not trying to match pattern, output state %d is already in list %d. char %d: %s\n",
			    abs(skip->id), skip_lid, skip->c, code);
#endif
		    ଭାଙ୍ଗ;
		}
		// Copy submatch info to the recursive call, opposite of what
		// happens afterwards.
		copy_sub_off(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		ଯଦି (rex.nfa_has_zsubexpr)
		    copy_sub_off(&m->synt, &t->subs.synt);
#endif

		// First try matching the pattern.
		result = recursive_regmatch(t->state, ଖାଲି, prog,
					  submatch, m, &listids, &listids_len);
		ଯଦି (result == NFA_TOO_EXPENSIVE)
		{
		    nfa_match = result;
		    ଯାଅ theend;
		}
		ଯଦି (result)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା bytelen;

#ifdef ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN matches:\n");
		    log_subsexpr(m);
#endif
		    // Copy submatch info from the recursive call
		    copy_sub_off(&t->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
		    ଯଦି (rex.nfa_has_zsubexpr)
			copy_sub_off(&t->subs.synt, &m->synt);
#endif
		    // Now we need to skip over the matched text and then
		    // continue with what follows.
		    ଯଦି (REG_MULTI)
			// TODO: multi-line match
			bytelen = m->norm.list.multi[0].end_col
						  - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(rex.ନିବେଶ - rex.ଧାଡ଼ି);
		    ଅନ୍ୟଥା
			bytelen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(m->norm.list.ଧାଡ଼ି[0].end - rex.ନିବେଶ);

#ifdef ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN length: %d\n", bytelen);
#endif
		    ଯଦି (bytelen == 0)
		    {
			// empty match, output of corresponding
			// NFA_END_PATTERN/NFA_SKIP to be used at current
			// position
			add_here = TRUE;
			add_state = t->state->out1->out->out;
		    }
		    ଅନ୍ୟଥା ଯଦି (bytelen <= clen)
		    {
			// match current character, output of corresponding
			// NFA_END_PATTERN to be used at next position.
			add_state = t->state->out1->out->out;
			add_off = clen;
		    }
		    ଅନ୍ୟଥା
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			add_state = t->state->out1->out;
			add_off = bytelen;
			add_ଗଣନା = bytelen - clen;
		    }
		}
		ଭାଙ୍ଗ;
	      }

	    ସ୍ଥିତି NFA_BOL:
		ଯଦି (rex.ନିବେଶ == rex.ଧାଡ଼ି)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_EOL:
		ଯଦି (curc == NUL)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_BOW:
		result = TRUE;

		ଯଦି (curc == NUL)
		    result = FALSE;
		ଅନ୍ୟଥା ଯଦି (has_mbyte)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା this_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.ନିବେଶ, rex.reg_buf);
		    ଯଦି (this_class <= 1)
			result = FALSE;
		    ଅନ୍ୟଥା ଯଦି (reg_prev_class() == this_class)
			result = FALSE;
		}
		ଅନ୍ୟଥା ଯଦି (!vim_iswordc_buf(curc, rex.reg_buf)
			   || (rex.ନିବେଶ > rex.ଧାଡ଼ି
			       && vim_iswordc_buf(rex.ନିବେଶ[-1], rex.reg_buf)))
		    result = FALSE;
		ଯଦି (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_EOW:
		result = TRUE;
		ଯଦି (rex.ନିବେଶ == rex.ଧାଡ଼ି)
		    result = FALSE;
		ଅନ୍ୟଥା ଯଦି (has_mbyte)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା this_class, prev_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.ନିବେଶ, rex.reg_buf);
		    prev_class = reg_prev_class();
		    ଯଦି (this_class == prev_class
					|| prev_class == 0 || prev_class == 1)
			result = FALSE;
		}
		ଅନ୍ୟଥା ଯଦି (!vim_iswordc_buf(rex.ନିବେଶ[-1], rex.reg_buf)
			|| (rex.ନିବେଶ[0] != NUL
					&& vim_iswordc_buf(curc, rex.reg_buf)))
		    result = FALSE;
		ଯଦି (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_BOF:
		ଯଦି (rex.lnum == 0 && rex.ନିବେଶ == rex.ଧାଡ଼ି
				     && (!REG_MULTI || rex.reg_firstlnum == 1))
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_EOF:
		ଯଦି (rex.lnum == rex.reg_maxline && curc == NUL)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_COMPOSING:
	    {
		nfa_state_T *end;

		result = match_composing(t->state->out, curc, clen);

		end = t->state->out1;	    // NFA_END_COMPOSING
		ADD_STATE_IF_MATCH(end);
		ଭାଙ୍ଗ;
	    }

	    ସ୍ଥିତି NFA_NEWL:
		ଯଦି (curc == NUL && !rex.reg_ଧାଡ଼ି_lbr && REG_MULTI
						&& rex.lnum <= rex.reg_maxline)
		{
		    go_to_nextline = TRUE;
		    // Pass -1 for the offset, which means taking the position
		    // at the start of the next line.
		    add_state = t->state->out;
		    add_off = -1;
		}
		ଅନ୍ୟଥା ଯଦି (curc == '\n' && rex.reg_ଧାଡ଼ି_lbr)
		{
		    // match \n as if it is an ordinary character
		    add_state = t->state->out;
		    add_off = 1;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_START_COLL:
	    ସ୍ଥିତି NFA_START_NEG_COLL:
	      {
		// What follows is a list of characters, until NFA_END_COLL.
		// One of them must match or none of them must match.
		nfa_state_T	*state;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result_if_matched;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c1, c2;

		// Never match EOL. If it's part of the collection it is added
		// as a separate state with an OR.
		ଯଦି (curc == NUL)
		    ଭାଙ୍ଗ;

		state = t->state->out;
		result_if_matched = (t->state->c == NFA_START_COLL);
		ପାଇଁ (;;)
		{
		    ଯଦି (state->c == NFA_COMPOSING)
		    {
			nfa_state_T *end;

			result = match_composing(t->state->out->out, curc,
									clen);

			ଯଦି (t->state->out->out1 != ଖାଲି
				&& t->state->out->out1->c == NFA_END_COMPOSING)
			{
			    end = t->state->out->out1;
			    ADD_STATE_IF_MATCH(end);
			}
			ଭାଙ୍ଗ;
		    }
		    ଯଦି (state->c == NFA_END_COLL)
		    {
			result = !result_if_matched;
			ଭାଙ୍ଗ;
		    }
		    ଯଦି (state->c == NFA_RANGE_MIN)
		    {
			c1 = state->val;
			state = state->out; // advance to NFA_RANGE_MAX
			c2 = state->val;
#ifdef ENABLE_LOG
			fprintf(log_fd, "NFA_RANGE_MIN curc=%d c1=%d c2=%d\n",
				curc, c1, c2);
#endif
			ଯଦି (curc >= c1 && curc <= c2)
			{
			    result = result_if_matched;
			    ଭାଙ୍ଗ;
			}
			ଯଦି (rex.reg_ic)
			{
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା curc_ତଳ = MB_CASEFOLD(curc);
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା done = FALSE;

			    ପାଇଁ ( ; c1 <= c2; ++c1)
				ଯଦି (MB_CASEFOLD(c1) == curc_ତଳ)
				{
				    result = result_if_matched;
				    done = TRUE;
				    ଭାଙ୍ଗ;
				}
			    ଯଦି (done)
				ଭାଙ୍ଗ;
			}
		    }
		    ଅନ୍ୟଥା ଯଦି (state->c < 0 ? check_char_class(state->c, curc)
			       : (curc == state->c
				   || (rex.reg_ic && MB_CASEFOLD(curc)
						    == MB_CASEFOLD(state->c))))
		    {
			result = result_if_matched;
			ଭାଙ୍ଗ;
		    }
		    state = state->out;
		}
		ଯଦି (result)
		{
		    // next state is in out of the NFA_END_COLL, out1 of
		    // START points to the END state
		    add_state = t->state->out1->out;
		    add_off = clen;
		}
		ଭାଙ୍ଗ;
	      }

	    ସ୍ଥିତି NFA_ANY:
		// Any char except '\0', (end of input) does not match.
		ଯଦି (curc > 0)
		{
		    add_state = t->state->out;
		    add_off = clen;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_ANY_COMPOSING:
		// On a composing character skip over it.  Otherwise do
		// nothing.  Always matches.
		ଯଦି (enc_utf8 && utf_iscomposing(curc))
		{
		    add_off = clen;
		}
		ଅନ୍ୟଥା
		{
		    add_here = TRUE;
		    add_off = 0;
		}
		add_state = t->state->out;
		ଭାଙ୍ଗ;

	    /*
	     * Character classes like \a for alpha, \d for digit etc.
	     */
	    ସ୍ଥିତି NFA_IDENT:	//  \i
		result = vim_isIDc(curc);
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_SIDENT:	//  \I
		result = !VIM_ISDIGIT(curc) && vim_isIDc(curc);
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_KWORD:	//  \k
		result = vim_iswordp_buf(rex.ନିବେଶ, rex.reg_buf);
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_SKWORD:	//  \K
		result = !VIM_ISDIGIT(curc)
				     && vim_iswordp_buf(rex.ନିବେଶ, rex.reg_buf);
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_FNAME:	//  \f
		result = vim_isfilec(curc);
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_SFNAME:	//  \F
		result = !VIM_ISDIGIT(curc) && vim_isfilec(curc);
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_PRINT:	//  \p
		result = vim_isprintc(PTR2CHAR(rex.ନିବେଶ));
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_SPRINT:	//  \P
		result = !VIM_ISDIGIT(curc) && vim_isprintc(PTR2CHAR(rex.ନିବେଶ));
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_WHITE:	//  \s
		result = VIM_ISWHITE(curc);
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_NWHITE:	//  \S
		result = curc != NUL && !VIM_ISWHITE(curc);
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_DIGIT:	//  \d
		result = ri_digit(curc);
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_NDIGIT:	//  \D
		result = curc != NUL && !ri_digit(curc);
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_HEX:	//  \x
		result = ri_hex(curc);
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_NHEX:	//  \X
		result = curc != NUL && !ri_hex(curc);
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_OCTAL:	//  \o
		result = ri_octal(curc);
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_NOCTAL:	//  \O
		result = curc != NUL && !ri_octal(curc);
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_WORD:	//  \w
		result = ri_word(curc);
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_NWORD:	//  \W
		result = curc != NUL && !ri_word(curc);
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_HEAD:	//  \h
		result = ri_head(curc);
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_NHEAD:	//  \H
		result = curc != NUL && !ri_head(curc);
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_ALPHA:	//  \a
		result = ri_alpha(curc);
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_NALPHA:	//  \A
		result = curc != NUL && !ri_alpha(curc);
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_LOWER:	//  \l
		result = ri_lower(curc);
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_NLOWER:	//  \L
		result = curc != NUL && !ri_lower(curc);
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_UPPER:	//  \u
		result = ri_upper(curc);
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_NUPPER:	// \U
		result = curc != NUL && !ri_upper(curc);
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_LOWER_IC:	// [a-z]
		result = ri_lower(curc) || (rex.reg_ic && ri_upper(curc));
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_NLOWER_IC:	// [^a-z]
		result = curc != NUL
			&& !(ri_lower(curc) || (rex.reg_ic && ri_upper(curc)));
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_UPPER_IC:	// [A-Z]
		result = ri_upper(curc) || (rex.reg_ic && ri_lower(curc));
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_NUPPER_IC:	// ^[A-Z]
		result = curc != NUL
			&& !(ri_upper(curc) || (rex.reg_ic && ri_lower(curc)));
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_BACKREF1:
	    ସ୍ଥିତି NFA_BACKREF2:
	    ସ୍ଥିତି NFA_BACKREF3:
	    ସ୍ଥିତି NFA_BACKREF4:
	    ସ୍ଥିତି NFA_BACKREF5:
	    ସ୍ଥିତି NFA_BACKREF6:
	    ସ୍ଥିତି NFA_BACKREF7:
	    ସ୍ଥିତି NFA_BACKREF8:
	    ସ୍ଥିତି NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	    ସ୍ଥିତି NFA_ZREF1:
	    ସ୍ଥିତି NFA_ZREF2:
	    ସ୍ଥିତି NFA_ZREF3:
	    ସ୍ଥିତି NFA_ZREF4:
	    ସ୍ଥିତି NFA_ZREF5:
	    ସ୍ଥିତି NFA_ZREF6:
	    ସ୍ଥିତି NFA_ZREF7:
	    ସ୍ଥିତି NFA_ZREF8:
	    ସ୍ଥିତି NFA_ZREF9:
#endif
		// \1 .. \9  \z1 .. \z9
	      {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା subidx;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା bytelen;

#ifdef FEAT_SYN_HL
		ଯଦି (t->state->c >= NFA_BACKREF1 && t->state->c <= NFA_BACKREF9)
#endif
		{
		    subidx = t->state->c - NFA_BACKREF1 + 1;
		    result = match_backref(&t->subs.norm, subidx, &bytelen);
		}
#ifdef FEAT_SYN_HL
		ଅନ୍ୟଥା
		{
		    subidx = t->state->c - NFA_ZREF1 + 1;
		    result = match_zref(subidx, &bytelen);
		}
#endif

		ଯଦି (result)
		{
		    ଯଦି (bytelen == 0)
		    {
			// empty match always works, output of NFA_SKIP to be
			// used next
			add_here = TRUE;
			add_state = t->state->out->out;
		    }
		    ଅନ୍ୟଥା ଯଦି (bytelen <= clen)
		    {
			// match current character, jump ahead to out of
			// NFA_SKIP
			add_state = t->state->out->out;
			add_off = clen;
		    }
		    ଅନ୍ୟଥା
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			add_state = t->state->out;
			add_off = bytelen;
			add_ଗଣନା = bytelen - clen;
		    }
		}
		ଭାଙ୍ଗ;
	      }
	    ସ୍ଥିତି NFA_SKIP:
	      // character of previous matching \1 .. \9  or \@>
	      ଯଦି (t->ଗଣନା_2 - clen <= 0)
	      {
		  // end of match, go to what follows
		  add_state = t->state->out;
		  add_off = clen;
	      }
	      ଅନ୍ୟଥା
	      {
		  // add state again with decremented count
		  add_state = t->state;
		  add_off = 0;
		  add_ଗଣନା = t->ଗଣନା_2 - clen;
	      }
	      ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_LNUM:
	    ସ୍ଥିତି NFA_LNUM_GT:
	    ସ୍ଥିତି NFA_LNUM_LT:
		result = (REG_MULTI &&
			nfa_re_ଅଙ୍କ_cmp(t->state->val, t->state->c - NFA_LNUM,
			    (long_u)(rex.lnum + rex.reg_firstlnum)));
		ଯଦି (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_COL:
	    ସ୍ଥିତି NFA_COL_GT:
	    ସ୍ଥିତି NFA_COL_LT:
		result = nfa_re_ଅଙ୍କ_cmp(t->state->val, t->state->c - NFA_COL,
			(long_u)(rex.ନିବେଶ - rex.ଧାଡ଼ି) + 1);
		ଯଦି (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_VCOL:
	    ସ୍ଥିତି NFA_VCOL_GT:
	    ସ୍ଥିତି NFA_VCOL_LT:
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     op = t->state->c - NFA_VCOL;
		    colnr_T col = (colnr_T)(rex.ନିବେଶ - rex.ଧାଡ଼ି);
		    win_T   *wp = rex.reg_win == ଖାଲି ? curwin : rex.reg_win;

		    // Bail out quickly when there can't be a match, avoid the
		    // overhead of win_linetabsize() on long lines.
		    ଯଦି (op != 1 && col > t->state->val
			    * (has_mbyte ? MB_MAXBYTES : 1))
			ଭାଙ୍ଗ;
		    result = FALSE;
		    ଯଦି (op == 1 && col - 1 > t->state->val && col > 100)
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା ts = wp->w_buffer->b_p_ts;

			// Guess that a character won't use more columns than
			// 'tabstop', with a minimum of 4.
			ଯଦି (ts < 4)
			    ts = 4;
			result = col > t->state->val * ts;
		    }
		    ଯଦି (!result)
		    {
			linenr_T    lnum = REG_MULTI
					    ? rex.reg_firstlnum + rex.lnum : 1;
			long_u	    vcol;

			ଯଦି (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା))
			    lnum = 1;
			vcol = (long_u)win_linetabsize(wp, lnum, rex.ଧାଡ଼ି, col);
			result = nfa_re_ଅଙ୍କ_cmp(t->state->val, op, vcol + 1);
		    }
		    ଯଦି (result)
		    {
			add_here = TRUE;
			add_state = t->state->out;
		    }
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_MARK:
	    ସ୍ଥିତି NFA_MARK_GT:
	    ସ୍ଥିତି NFA_MARK_LT:
	      {
		pos_T	*pos;
		size_t	col = REG_MULTI ? rex.ନିବେଶ - rex.ଧାଡ଼ି : 0;

		pos = getmark_buf(rex.reg_buf, t->state->val, FALSE);

		// Line may have been freed, get it again.
		ଯଦି (REG_MULTI)
		{
		    rex.ଧାଡ଼ି = reg_getline(rex.lnum);
		    rex.ନିବେଶ = rex.ଧାଡ଼ି + col;
		}

		// Compare the mark position to the match position, if the mark
		// exists and mark is set in reg_buf.
		ଯଦି (pos != ଖାଲି && pos->lnum > 0)
		{
		    colnr_T pos_col = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->col == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->col;

		    result = (pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_col == (colnr_T)(rex.ନିବେଶ - rex.ଧାଡ଼ି)
				    ? t->state->c == NFA_MARK
				    : (pos_col < (colnr_T)(rex.ନିବେଶ - rex.ଧାଡ଼ି)
					? t->state->c == NFA_MARK_GT
					: t->state->c == NFA_MARK_LT))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? t->state->c == NFA_MARK_GT
				    : t->state->c == NFA_MARK_LT));
		    ଯଦି (result)
		    {
			add_here = TRUE;
			add_state = t->state->out;
		    }
		}
		ଭାଙ୍ଗ;
	      }

	    ସ୍ଥିତି NFA_CURSOR:
		result = (rex.reg_win != ଖାଲି
			&& (rex.lnum + rex.reg_firstlnum
						 == rex.reg_win->w_cursor.lnum)
			&& ((colnr_T)(rex.ନିବେଶ - rex.ଧାଡ଼ି)
						== rex.reg_win->w_cursor.col));
		ଯଦି (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_VISUAL:
		result = reg_match_visual();
		ଯଦି (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି NFA_MOPEN1:
	    ସ୍ଥିତି NFA_MOPEN2:
	    ସ୍ଥିତି NFA_MOPEN3:
	    ସ୍ଥିତି NFA_MOPEN4:
	    ସ୍ଥିତି NFA_MOPEN5:
	    ସ୍ଥିତି NFA_MOPEN6:
	    ସ୍ଥିତି NFA_MOPEN7:
	    ସ୍ଥିତି NFA_MOPEN8:
	    ସ୍ଥିତି NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	    ସ୍ଥିତି NFA_ZOPEN:
	    ସ୍ଥିତି NFA_ZOPEN1:
	    ସ୍ଥିତି NFA_ZOPEN2:
	    ସ୍ଥିତି NFA_ZOPEN3:
	    ସ୍ଥିତି NFA_ZOPEN4:
	    ସ୍ଥିତି NFA_ZOPEN5:
	    ସ୍ଥିତି NFA_ZOPEN6:
	    ସ୍ଥିତି NFA_ZOPEN7:
	    ସ୍ଥିତି NFA_ZOPEN8:
	    ସ୍ଥିତି NFA_ZOPEN9:
#endif
	    ସ୍ଥିତି NFA_NOPEN:
	    ସ୍ଥିତି NFA_ZSTART:
		// These states are only added to be able to bail out when
		// they are added again, nothing is to be done.
		ଭାଙ୍ଗ;

	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:	// regular character
	      {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = t->state->c;

#ifdef DEBUG
		ଯଦି (c < 0)
		    siemsg("Negative state char: %ld", (long)c);
#endif
		result = (c == curc);

		ଯଦି (!result && rex.reg_ic)
		    result = MB_CASEFOLD(c) == MB_CASEFOLD(curc);
		// If rex.reg_icombine is not set only skip over the character
		// itself.  When it is set skip over composing characters.
		ଯଦି (result && enc_utf8 && !rex.reg_icombine)
		    clen = utf_ptr2len(rex.ନିବେଶ);
		ADD_STATE_IF_MATCH(t->state);
		ଭାଙ୍ଗ;
	      }

	    } // switch (t->state->c)

	    ଯଦି (add_state != ଖାଲି)
	    {
		nfa_pim_T *pim;
		nfa_pim_T pim_copy;

		ଯଦି (t->pim.result == NFA_PIM_UNUSED)
		    pim = ଖାଲି;
		ଅନ୍ୟଥା
		    pim = &t->pim;

		// Handle the postponed invisible match if the match might end
		// without advancing and before the end of the line.
		ଯଦି (pim != ଖାଲି && (clen == 0 || match_follows(add_state, 0)))
		{
		    ଯଦି (pim->result == NFA_PIM_TODO)
		    {
#ifdef ENABLE_LOG
			fprintf(log_fd, "\n");
			fprintf(log_fd, "==================================\n");
			fprintf(log_fd, "Postponed recursive nfa_regmatch()\n");
			fprintf(log_fd, "\n");
#endif
			result = recursive_regmatch(pim->state, pim,
				    prog, submatch, m, &listids, &listids_len);
			pim->result = result ? NFA_PIM_MATCH : NFA_PIM_NOMATCH;
			// for \@! and \@<! it is a match when the result is
			// FALSE
			ଯଦି (result != (pim->state->c == NFA_START_INVISIBLE_NEG
			     || pim->state->c == NFA_START_INVISIBLE_NEG_FIRST
			     || pim->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&pim->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
			    ଯଦି (rex.nfa_has_zsubexpr)
				copy_sub_off(&pim->subs.synt, &m->synt);
#endif
			}
		    }
		    ଅନ୍ୟଥା
		    {
			result = (pim->result == NFA_PIM_MATCH);
#ifdef ENABLE_LOG
			fprintf(log_fd, "\n");
			fprintf(log_fd, "Using previous recursive nfa_regmatch() result, result == %d\n", pim->result);
			fprintf(log_fd, "MATCH = %s\n", result == TRUE ? "OK" : "FALSE");
			fprintf(log_fd, "\n");
#endif
		    }

		    // for \@! and \@<! it is a match when result is FALSE
		    ଯଦି (result != (pim->state->c == NFA_START_INVISIBLE_NEG
			     || pim->state->c == NFA_START_INVISIBLE_NEG_FIRST
			     || pim->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
		    {
			// Copy submatch info from the recursive call
			copy_sub_off(&t->subs.norm, &pim->subs.norm);
#ifdef FEAT_SYN_HL
			ଯଦି (rex.nfa_has_zsubexpr)
			    copy_sub_off(&t->subs.synt, &pim->subs.synt);
#endif
		    }
		    ଅନ୍ୟଥା
			// look-behind match failed, don't add the state
			ଜାରି;

		    // Postponed invisible match was handled, don't add it to
		    // following states.
		    pim = ଖାଲି;
		}

		// If "pim" points into l->t it will become invalid when
		// adding the state causes the list to be reallocated.  Make a
		// local copy to avoid that.
		ଯଦି (pim == &t->pim)
		{
		    copy_pim(&pim_copy, pim);
		    pim = &pim_copy;
		}

		ଯଦି (add_here)
		    r = addstate_here(thislist, add_state, &t->subs,
								pim, &listidx);
		ଅନ୍ୟଥା
		{
		    r = addstate(nextlist, add_state, &t->subs, pim, add_off, 0);
		    ଯଦି (add_ଗଣନା > 0)
			nextlist->t[nextlist->n - 1].ଗଣନା_2 = add_ଗଣନା;
		}
		ଯଦି (r == ଖାଲି)
		{
		    nfa_match = NFA_TOO_EXPENSIVE;
		    ଯାଅ theend;
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
	ଯଦି (nfa_match == FALSE
		&& ((toplevel
			&& rex.lnum == 0
			&& clen != 0
			&& (rex.reg_maxcol == 0
			  || (colnr_T)(rex.ନିବେଶ - rex.ଧାଡ଼ି) < rex.reg_maxcol))
		    || (nfa_endp != ଖାଲି
			&& (REG_MULTI
			    ? (rex.lnum < nfa_endp->se_u.pos.lnum
			       || (rex.lnum == nfa_endp->se_u.pos.lnum
				   && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(rex.ନିବେଶ - rex.ଧାଡ଼ି)
						    < nfa_endp->se_u.pos.col))
			    : rex.ନିବେଶ < nfa_endp->se_u.ptr))))
	{
#ifdef ENABLE_LOG
	    fprintf(log_fd, "(---) STARTSTATE\n");
#endif
	    // Inline optimized code for addstate() if we know the state is
	    // the first MOPEN.
	    ଯଦି (toplevel)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା add = TRUE;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;

		ଯଦି (prog->regstart != NUL && clen != 0)
		{
		    ଯଦି (nextlist->n == 0)
		    {
			colnr_T col = (colnr_T)(rex.ନିବେଶ - rex.ଧାଡ଼ି) + clen;

			// Nextlist is empty, we can skip ahead to the
			// character that must appear at the start.
			ଯଦି (skip_to_start(prog->regstart, &col) == FAIL)
			    ଭାଙ୍ଗ;
#ifdef ENABLE_LOG
			fprintf(log_fd, "  Skipping ahead %d bytes to regstart\n",
				col - ((colnr_T)(rex.ନିବେଶ - rex.ଧାଡ଼ି) + clen));
#endif
			rex.ନିବେଶ = rex.ଧାଡ଼ି + col - clen;
		    }
		    ଅନ୍ୟଥା
		    {
			// Checking if the required start character matches is
			// cheaper than adding a state that won't match.
			c = PTR2CHAR(rex.ନିବେଶ + clen);
			ଯଦି (c != prog->regstart && (!rex.reg_ic
			     || MB_CASEFOLD(c) != MB_CASEFOLD(prog->regstart)))
			{
#ifdef ENABLE_LOG
			    fprintf(log_fd, "  Skipping start state, regstart does not match\n");
#endif
			    add = FALSE;
			}
		    }
		}

		ଯଦି (add)
		{
		    ଯଦି (REG_MULTI)
		    {
			m->norm.list.multi[0].start_col =
					 (colnr_T)(rex.ନିବେଶ - rex.ଧାଡ଼ି) + clen;
			m->norm.orig_start_col =
					       m->norm.list.multi[0].start_col;
		    }
		    ଅନ୍ୟଥା
			m->norm.list.ଧାଡ଼ି[0].start = rex.ନିବେଶ + clen;
		    ଯଦି (addstate(nextlist, start->out, m, ଖାଲି, clen, 0) == ଖାଲି)
		    {
			nfa_match = NFA_TOO_EXPENSIVE;
			ଯାଅ theend;
		    }
		}
	    }
	    ଅନ୍ୟଥା
	    {
		char_u	    *save_ଧାଡ଼ି = rex.ଧାଡ଼ି;
		char_u	    *save_ନିବେଶ = rex.ନିବେଶ;
		linenr_T    save_lnum = rex.lnum;

		// At the end of a line the match can only start on the next
		// line, use that position instead of the line break.
		ଯଦି (REG_MULTI && clen == 0 && nfa_endp != ଖାଲି
					 && rex.lnum < nfa_endp->se_u.pos.lnum)
		{
		    char_u  *next_ଧାଡ଼ି = reg_getline(rex.lnum + 1);

		    ଯଦି (next_ଧାଡ଼ି != ଖାଲି)
		    {
			rex.ଧାଡ଼ି = next_ଧାଡ଼ି;
			rex.ନିବେଶ = rex.ଧାଡ଼ି;
			++rex.lnum;
		    }
		}

		r = addstate(nextlist, start, m, ଖାଲି, clen, 0);

		rex.ଧାଡ଼ି = save_ଧାଡ଼ି;
		rex.ନିବେଶ = save_ନିବେଶ;
		rex.lnum = save_lnum;

		ଯଦି (r == ଖାଲି)
		{
		    nfa_match = NFA_TOO_EXPENSIVE;
		    ଯାଅ theend;
		}
	    }
	}

#ifdef ENABLE_LOG
	fprintf(log_fd, ">>> Thislist had %d states available: ", thislist->n);
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	    ପାଇଁ (i = 0; i < thislist->n; i++)
		fprintf(log_fd, "%d  ", abs(thislist->t[i].state->id));
	}
	fprintf(log_fd, "\n");
#endif

nextchar:
	// Advance to the next character, or advance to the next line, or
	// finish.
	ଯଦି (clen != 0)
	    rex.ନିବେଶ += clen;
	ଅନ୍ୟଥା ଯଦି (go_to_nextline || (nfa_endp != ଖାଲି && REG_MULTI
					&& rex.lnum < nfa_endp->se_u.pos.lnum))
	    reg_nextline();
	ଅନ୍ୟଥା
	    ଭାଙ୍ଗ;

	// Allow interrupting with CTRL-C.
	ଧାଡ଼ି_breakcheck();
	ଯଦି (got_int)
	    ଭାଙ୍ଗ;
#ifdef FEAT_RELTIME
	ଯଦି (nfa_did_time_out())
	    ଭାଙ୍ଗ;
#endif
    }

#ifdef ENABLE_LOG
    ଯଦି (log_fd != stderr)
	fclose(log_fd);
    log_fd = ଖାଲି;
#endif

theend:
    // Cache list buffers in prog for reuse, or free if prog already has
    // cached buffers (recursive call case).
    ଯଦି (prog->listbuf[0] == ଖାଲି && list[0].t != ଖାଲି && list[1].t != ଖାଲି)
    {
	prog->listbuf[0] = list[0].t;
	prog->listbuf[1] = list[1].t;
    }
    ଅନ୍ୟଥା
    {
	vim_free(list[0].t);
	vim_free(list[1].t);
    }
    vim_free(listids);
#undef ADD_STATE_IF_MATCH
#ifdef NFA_REGEXP_DEBUG_LOG
    fclose(debug);
#endif

    ଫେରନ୍ତୁ nfa_match;
}

/*
 * Try match of "prog" with at rex.line["col"].
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    ସ୍ଥିର long
nfa_regtry(
    nfa_regprog_T   *prog,
    colnr_T	    col,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    *timed_out UNUSED)	// flag set on timeout or NULL
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    regsubs_T	subs, m;
    nfa_state_T	*start = prog->start;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result;
#ifdef ENABLE_LOG
    FILE	*f;
#endif

    rex.ନିବେଶ = rex.ଧାଡ଼ି + col;
#ifdef FEAT_RELTIME
    nfa_timed_out = timed_out;
#endif

#ifdef ENABLE_LOG
    f = fopen(NFA_REGEXP_RUN_LOG, "a");
    ଯଦି (f != ଖାଲି)
    {
	fprintf(f, "\n\n\t=======================================================\n");
# ifdef DEBUG
	fprintf(f, "\tRegexp is \"%s\"\n", nfa_regengine.ପଦସମଷ୍ଟି);
# endif
	fprintf(f, "\tInput text is \"%s\" \n", rex.ନିବେଶ);
	fprintf(f, "\t=======================================================\n\n");
	nfa_print_state(f, start);
	fprintf(f, "\n\n");
	fclose(f);
    }
    ଅନ୍ୟଥା
	emsg("Could not open temporary log file for writing");
#endif

    clear_sub(&subs.norm);
    clear_sub(&m.norm);
#ifdef FEAT_SYN_HL
    clear_sub(&subs.synt);
    clear_sub(&m.synt);
#endif

    result = nfa_regmatch(prog, start, &subs, &m);
    ଯଦି (result == FALSE)
	ଫେରନ୍ତୁ 0;
    ଅନ୍ୟଥା ଯଦି (result == NFA_TOO_EXPENSIVE)
	ଫେରନ୍ତୁ result;

    cleanup_subexpr();
    ଯଦି (REG_MULTI)
    {
	ପାଇଁ (i = 0; i < subs.norm.in_use; i++)
	{
	    rex.reg_startpos[i].lnum = subs.norm.list.multi[i].start_lnum;
	    rex.reg_startpos[i].col = subs.norm.list.multi[i].start_col;

	    rex.reg_endpos[i].lnum = subs.norm.list.multi[i].end_lnum;
	    rex.reg_endpos[i].col = subs.norm.list.multi[i].end_col;
	}
	ଯଦି (rex.reg_mmatch != ଖାଲି)
	    rex.reg_mmatch->rmm_matchcol = subs.norm.orig_start_col;

	ଯଦି (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].col = col;
	}
	ଯଦି (rex.reg_endpos[0].lnum < 0)
	{
	    // pattern has a \ze but it didn't match, use current end
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(rex.ନିବେଶ - rex.ଧାଡ଼ି);
	}
	ଅନ୍ୟଥା
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    ଅନ୍ୟଥା
    {
	ପାଇଁ (i = 0; i < subs.norm.in_use; i++)
	{
	    rex.reg_startp[i] = subs.norm.list.ଧାଡ଼ି[i].start;
	    rex.reg_endp[i] = subs.norm.list.ଧାଡ଼ି[i].end;
	}

	ଯଦି (rex.reg_startp[0] == ଖାଲି)
	    rex.reg_startp[0] = rex.ଧାଡ଼ି + col;
	ଯଦି (rex.reg_endp[0] == ଖାଲି)
	    rex.reg_endp[0] = rex.ନିବେଶ;
    }

#ifdef FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = ଖାଲି;

    ଯଦି (prog->reghasz == REX_SET)
    {
	cleanup_zsubexpr();
	re_extmatch_out = make_extmatch();
	ଯଦି (re_extmatch_out == ଖାଲି)
	    ଫେରନ୍ତୁ 0;
	// Loop over \z1, \z2, etc.  There is no \z0.
	ପାଇଁ (i = 1; i < subs.synt.in_use; i++)
	{
	    ଯଦି (REG_MULTI)
	    {
		ସଂରଚନା multipos *mpos = &subs.synt.list.multi[i];

		// Only accept single line matches that are valid.
		ଯଦି (mpos->start_lnum >= 0
			&& mpos->start_lnum == mpos->end_lnum
			&& mpos->end_col >= mpos->start_col)
		    re_extmatch_out->matches[i] =
			vim_strnsave(reg_getline(mpos->start_lnum)
							    + mpos->start_col,
					     mpos->end_col - mpos->start_col);
	    }
	    ଅନ୍ୟଥା
	    {
		ସଂରଚନା linepos *lpos = &subs.synt.list.ଧାଡ଼ି[i];

		ଯଦି (lpos->start != ଖାଲି && lpos->end != ଖାଲି)
		    re_extmatch_out->matches[i] =
			    vim_strnsave(lpos->start, lpos->end - lpos->start);
	    }
	}
    }
#endif

    ଫେରନ୍ତୁ 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 *
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    ସ୍ଥିର long
nfa_regexec_both(
    char_u	*ଧାଡ଼ି,
    colnr_T	startcol,	// column to start looking for match
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*timed_out)	// flag set on timeout or NULL
{
    nfa_regprog_T   *prog;
    long	    retval = 0L;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    i;
    colnr_T	    col = startcol;

    ଯଦି (REG_MULTI)
    {
	prog = (nfa_regprog_T *)rex.reg_mmatch->regprog;
	ଧାଡ଼ି = reg_getline((linenr_T)0);    // relative to the cursor
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    ଅନ୍ୟଥା
    {
	prog = (nfa_regprog_T *)rex.reg_match->regprog;
	rex.reg_startp = rex.reg_match->startp;
	rex.reg_endp = rex.reg_match->endp;
    }

    // Be paranoid...
    ଯଦି (prog == ଖାଲି || ଧାଡ଼ି == ଖାଲି)
    {
	iemsg(e_null_argument);
	ଯାଅ theend;
    }

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    ଯଦି (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    ଅନ୍ୟଥା ଯଦି (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    ଯଦି (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    rex.ଧାଡ଼ି = ଧାଡ଼ି;
    rex.lnum = 0;    // relative to line

    rex.nfa_has_zend = prog->has_zend;
    rex.nfa_has_backref = prog->has_backref;
    rex.nfa_nsubexpr = prog->nsubexp;
    rex.nfa_listid = 1;
    rex.nfa_alt_listid = 2;
#ifdef DEBUG
    nfa_regengine.ପଦସମଷ୍ଟି = prog->pattern;
#endif

    ଯଦି (prog->reganch && col > 0)
	ଫେରନ୍ତୁ 0L;

    rex.need_clear_subexpr = TRUE;
#ifdef FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    ଯଦି (prog->reghasz == REX_SET)
    {
	rex.nfa_has_zsubexpr = TRUE;
	rex.need_clear_zsubexpr = TRUE;
    }
    ଅନ୍ୟଥା
    {
	rex.nfa_has_zsubexpr = FALSE;
	rex.need_clear_zsubexpr = FALSE;
    }
#endif

    ଯଦି (prog->regstart != NUL)
    {
	// Skip ahead until a character we know the match must start with.
	// When there is none there is no match.
	ଯଦି (skip_to_start(prog->regstart, &col) == FAIL)
	    ଫେରନ୍ତୁ 0L;

	// If match_text is set it contains the full text that must match.
	// Nothing else to try. Doesn't handle combining chars well.
	ଯଦି (prog->match_text != ଖାଲି && *prog->match_text != NUL && !rex.reg_icombine)
	{
	    retval = find_match_text(&col, prog->regstart, prog->match_text);
	    ଯଦି (REG_MULTI)
		rex.reg_mmatch->rmm_matchcol = col;
	    ଅନ୍ୟଥା
		rex.reg_match->rm_matchcol = col;
	    ଫେରନ୍ତୁ retval;
	}
    }

    // If the start column is past the maximum column: no need to try.
    ଯଦି (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	ଯାଅ theend;

    // Set the "nstate" used by nfa_regcomp() to zero to trigger an error when
    // it's accidentally used during execution.
    nstate = 0;
    ପାଇଁ (i = 0; i < prog->nstate; ++i)
    {
	prog->state[i].id = i;
	prog->state[i].lastlist[0] = 0;
	prog->state[i].lastlist[1] = 0;
    }

    retval = nfa_regtry(prog, col, timed_out);

#ifdef DEBUG
    nfa_regengine.ପଦସମଷ୍ଟି = ଖାଲି;
#endif

theend:
    ଯଦି (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs and
	// \ze are used.
	ଯଦି (REG_MULTI)
	{
	    lpos_T *start = &rex.reg_mmatch->startpos[0];
	    lpos_T *end = &rex.reg_mmatch->endpos[0];

	    ଯଦି (end->lnum < start->lnum
			|| (end->lnum == start->lnum && end->col < start->col))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (rex.reg_match->endp[0] < rex.reg_match->startp[0])
		rex.reg_match->endp[0] = rex.reg_match->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_match->rm_matchcol = col;
	}
    }

    ଫେରନ୍ତୁ retval;
}

/*
 * Compile a regular expression into internal code for the NFA matcher.
 * Returns the program in allocated space.  Returns NULL for an error.
 */
    ସ୍ଥିର regprog_T *
nfa_regcomp(char_u *ପଦସମଷ୍ଟି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା re_flags)
{
    nfa_regprog_T	*prog = ଖାଲି;
    size_t		prog_size;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			*postfix;

    ଯଦି (ପଦସମଷ୍ଟି == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

#ifdef DEBUG
    nfa_regengine.ପଦସମଷ୍ଟି = ପଦସମଷ୍ଟି;
#endif
    nfa_re_flags = re_flags;

    init_class_tab();

    ଯଦି (nfa_regcomp_start(ପଦସମଷ୍ଟି, re_flags) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;

    // Build postfix form of the regexp. Needed to build the NFA
    // (and count its size).
    postfix = re2post();
    ଯଦି (postfix == ଖାଲି)
	ଯାଅ fail;	    // Cascaded (syntax?) error

    /*
     * In order to build the NFA, we parse the input regexp twice:
     * 1. first pass to count size (so we can allocate space)
     * 2. second to emit code
     */
#ifdef ENABLE_LOG
    {
	FILE *f = fopen(NFA_REGEXP_RUN_LOG, "a");

	ଯଦି (f != ଖାଲି)
	{
	    fprintf(f, "\n*****************************\n\n\n\n\tCompiling regexp \"%s\"... hold on !\n", ପଦସମଷ୍ଟି);
	    fclose(f);
	}
    }
#endif

    /*
     * PASS 1
     * Count number of NFA states in "nstate". Do not build the NFA.
     */
    post2nfa(postfix, post_ptr, TRUE);

    // allocate the regprog with space for the compiled regexp
    prog_size = offsetof(nfa_regprog_T, state) + ଆକାର(nfa_state_T) * nstate;
    prog = alloc(prog_size);
    ଯଦି (prog == ଖାଲି)
	ଯାଅ fail;
    state_ptr = prog->state;
    prog->re_in_use = FALSE;
    prog->listbuf[0] = ଖାଲି;
    prog->listbuf[1] = ଖାଲି;

    /*
     * PASS 2
     * Build the NFA
     */
    prog->start = post2nfa(postfix, post_ptr, FALSE);
    ଯଦି (prog->start == ଖାଲି)
	ଯାଅ fail;

    prog->regflags = regflags;
    prog->engine = &nfa_regengine;
    prog->nstate = nstate;
    prog->has_zend = rex.nfa_has_zend;
    prog->has_backref = rex.nfa_has_backref;
    prog->nsubexp = regnpar;

    nfa_postprocess(prog);

    prog->reganch = nfa_get_reganch(prog->start, 0);
    prog->regstart = nfa_get_regstart(prog->start, 0);
    prog->match_text = nfa_get_match_text(prog->start);

#ifdef ENABLE_LOG
    nfa_postfix_dump(ପଦସମଷ୍ଟି, OK);
    nfa_dump(prog);
#endif
#ifdef FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    prog->reghasz = re_has_z;
#endif
    prog->pattern = vim_strsave(ପଦସମଷ୍ଟି);
#ifdef DEBUG
    nfa_regengine.ପଦସମଷ୍ଟି = ଖାଲି;
#endif

out:
    // The postfix list and fragment stack are reused by the next compilation
    // (and freed in free_regexp_stuff()), but drop ones that grew large for a
    // big pattern to avoid holding much memory.
    ଯଦି (post_start_len > NFA_POSTFIX_KEEP)
    {
	VIM_CLEAR(post_start);
	post_start_len = 0;
    }
    post_ptr = post_end = ଖାଲି;
    ଯଦି (nfa_ଗଦା_len > NFA_ଗଦା_KEEP)
    {
	VIM_CLEAR(nfa_ଗଦା);
	nfa_ଗଦା_len = 0;
    }
    state_ptr = ଖାଲି;
    ଫେରନ୍ତୁ (regprog_T *)prog;

fail:
    VIM_CLEAR(prog);
#ifdef ENABLE_LOG
    nfa_postfix_dump(ପଦସମଷ୍ଟି, FAIL);
#endif
#ifdef DEBUG
    nfa_regengine.ପଦସମଷ୍ଟି = ଖାଲି;
#endif
    ଯାଅ out;
}

/*
 * Free a compiled regexp program, returned by nfa_regcomp().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
nfa_regfree(regprog_T *prog)
{
    ଯଦି (prog == ଖାଲି)
	ଫେରନ୍ତୁ;

    vim_free(((nfa_regprog_T *)prog)->match_text);
    vim_free(((nfa_regprog_T *)prog)->pattern);
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
nfa_regexec_nl(
    regmatch_T	*rmp,
    char_u	*ଧାଡ଼ି,	// string to match against
    colnr_T	col,	// column to start looking for match
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଧାଡ଼ି_lbr)
{
    rex.reg_match = rmp;
    rex.reg_mmatch = ଖାଲି;
    rex.reg_maxline = 0;
    rex.reg_ଧାଡ଼ି_lbr = ଧାଡ଼ି_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = ଖାଲି;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;
    ଫେରନ୍ତୁ nfa_regexec_both(ଧାଡ଼ି, col, ଖାଲି);
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
    ସ୍ଥିର long
nfa_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	col,		// column to start looking for match
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*timed_out)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    ଫେରନ୍ତୁ nfa_regexec_both(ଖାଲି, col, timed_out);
}

#ifdef DEBUG
# undef ENABLE_LOG
#endif
