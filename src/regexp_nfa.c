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

ಗಣನಾ
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
ಸ್ಥಿರ ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ nfa_classcodes[] = {
    NFA_ANY, NFA_IDENT, NFA_SIDENT, NFA_KWORD,NFA_SKWORD,
    NFA_FNAME, NFA_SFNAME, NFA_PRINT, NFA_SPRINT,
    NFA_WHITE, NFA_NWHITE, NFA_DIGIT, NFA_NDIGIT,
    NFA_HEX, NFA_NHEX, NFA_OCTAL, NFA_NOCTAL,
    NFA_WORD, NFA_NWORD, NFA_HEAD, NFA_NHEAD,
    NFA_ALPHA, NFA_NALPHA, NFA_LOWER, NFA_NLOWER,
    NFA_UPPER, NFA_NUPPER
};

// Variables only used in nfa_regcomp() and descendants.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ nfa_re_flags; // re_flags passed to nfa_regcomp()
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ *post_start;  // holds the postfix form of r.e.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ post_start_len;  // size of allocated post_start (in ints)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ *post_end;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ *post_ptr;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ nfa_reg_parse_depth;	// nesting depth in nfa_reg()

// The postfix list (post_start) and fragment stack (nfa_stack) are reused
// across compilations; a buffer that grew past these sizes for a big pattern
// is freed afterwards instead of being kept, to avoid holding much memory.
#define NFA_POSTFIX_KEEP	10000	// number of ints (~40 Kbyte)
#define NFA_ರಾಶಿ_KEEP		4000	// number of Frag_T (~64 Kbyte)

// Set when the pattern should use the NFA engine.
// E.g. [[:upper:]] only allows 8bit characters for BT engine,
// while NFA engine handles multibyte characters correctly.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ wants_nfa;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ nstate;	// Number of states in the NFA.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ istate;	// Index in the state vector, used in alloc_state()

// If not NULL match must end at this position
ಸ್ಥಿರ save_se_T *nfa_endp = ರಿಕ್ತ;

// 0 for first call to nfa_regmatch(), 1 for recursive call.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ nfa_ll_index = 0;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ realloc_post_list(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ nfa_reg(ಪೂರ್ಣಾಙ್ಕ paren);
#ifdef DEBUG
ಸ್ಥಿರ ಶೂನ್ಯ nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent);
#endif
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ match_follows(nfa_state_T *startstate, ಪೂರ್ಣಾಙ್ಕ depth);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ failure_chance(nfa_state_T *state, ಪೂರ್ಣಾಙ್ಕ depth);

// helper functions used when doing re2post() ... regatom() parsing
#define EMIT(c)	ಕುರು {				\
		    ಯದಿ (post_ptr >= post_end && realloc_post_list() == FAIL) \
			ಪ್ರತ್ಯಾಗಮ FAIL;		\
		    *post_ptr++ = c;		\
		} ಯಾವತ್ (0)

/*
 * Initialize internal variables before NFA compilation.
 * Return OK on success, FAIL otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
nfa_regcomp_start(
    char_u	*ಅಭಿವ್ಯಕ್ತಿ,
    ಪೂರ್ಣಾಙ್ಕ		re_flags)	    // see vim_regcomp()
{
    size_t	postfix_size;
    ಪೂರ್ಣಾಙ್ಕ		nstate_max;

    nstate = 0;
    istate = 0;
    // A reasonable estimation for maximum size
    nstate_max = (ಪೂರ್ಣಾಙ್ಕ)(STRLEN(ಅಭಿವ್ಯಕ್ತಿ) + 1) * 25;

    // Some items blow up in size, such as [A-z].  Add more space for that.
    // When it is still not enough realloc_post_list() will be used.
    nstate_max += 1000;

    // Size for postfix representation of expr.
    postfix_size = ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ) * nstate_max;

    // Reuse the postfix buffer across compilations, only growing it when the
    // estimate exceeds the current size; it is freed in free_regexp_stuff().
    ಯದಿ (post_start == ರಿಕ್ತ || post_start_len < nstate_max)
    {
	vim_free(post_start);
	post_start = alloc(postfix_size);
	ಯದಿ (post_start == ರಿಕ್ತ)
	{
	    post_start_len = 0;
	    ಪ್ರತ್ಯಾಗಮ FAIL;
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
    regcomp_start(ಅಭಿವ್ಯಕ್ತಿ, re_flags);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Figure out if the NFA state list starts with an anchor, must match at start
 * of the line.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
nfa_get_reganch(nfa_state_T *start, ಪೂರ್ಣಾಙ್ಕ depth)
{
    nfa_state_T *p = start;

    ಯದಿ (depth > 4)
	ಪ್ರತ್ಯಾಗಮ 0;

    ಯಾವತ್ (p != ರಿಕ್ತ)
    {
	ವಿಕಲ್ಪ (p->c)
	{
	    ಪಕ್ಷ NFA_BOL:
	    ಪಕ್ಷ NFA_BOF:
		ಪ್ರತ್ಯಾಗಮ 1; // yes!

	    ಪಕ್ಷ NFA_ZSTART:
	    ಪಕ್ಷ NFA_ZEND:
	    ಪಕ್ಷ NFA_CURSOR:
	    ಪಕ್ಷ NFA_VISUAL:

	    ಪಕ್ಷ NFA_MOPEN:
	    ಪಕ್ಷ NFA_MOPEN1:
	    ಪಕ್ಷ NFA_MOPEN2:
	    ಪಕ್ಷ NFA_MOPEN3:
	    ಪಕ್ಷ NFA_MOPEN4:
	    ಪಕ್ಷ NFA_MOPEN5:
	    ಪಕ್ಷ NFA_MOPEN6:
	    ಪಕ್ಷ NFA_MOPEN7:
	    ಪಕ್ಷ NFA_MOPEN8:
	    ಪಕ್ಷ NFA_MOPEN9:
	    ಪಕ್ಷ NFA_NOPEN:
#ifdef FEAT_SYN_HL
	    ಪಕ್ಷ NFA_ZOPEN:
	    ಪಕ್ಷ NFA_ZOPEN1:
	    ಪಕ್ಷ NFA_ZOPEN2:
	    ಪಕ್ಷ NFA_ZOPEN3:
	    ಪಕ್ಷ NFA_ZOPEN4:
	    ಪಕ್ಷ NFA_ZOPEN5:
	    ಪಕ್ಷ NFA_ZOPEN6:
	    ಪಕ್ಷ NFA_ZOPEN7:
	    ಪಕ್ಷ NFA_ZOPEN8:
	    ಪಕ್ಷ NFA_ZOPEN9:
#endif
		p = p->out;
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_SPLIT:
		ಪ್ರತ್ಯಾಗಮ nfa_get_reganch(p->out, depth + 1)
				       && nfa_get_reganch(p->out1, depth + 1);

	    ಸ್ವಭಾವ:
		ಪ್ರತ್ಯಾಗಮ 0; // noooo
	}
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Figure out if the NFA state list starts with a character which must match
 * at start of the match.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
nfa_get_regstart(nfa_state_T *start, ಪೂರ್ಣಾಙ್ಕ depth)
{
    nfa_state_T *p = start;

    ಯದಿ (depth > 4)
	ಪ್ರತ್ಯಾಗಮ 0;

    ಯಾವತ್ (p != ರಿಕ್ತ)
    {
	ವಿಕಲ್ಪ (p->c)
	{
	    // all kinds of zero-width matches
	    ಪಕ್ಷ NFA_BOL:
	    ಪಕ್ಷ NFA_BOF:
	    ಪಕ್ಷ NFA_BOW:
	    ಪಕ್ಷ NFA_EOW:
	    ಪಕ್ಷ NFA_ZSTART:
	    ಪಕ್ಷ NFA_ZEND:
	    ಪಕ್ಷ NFA_CURSOR:
	    ಪಕ್ಷ NFA_VISUAL:
	    ಪಕ್ಷ NFA_LNUM:
	    ಪಕ್ಷ NFA_LNUM_GT:
	    ಪಕ್ಷ NFA_LNUM_LT:
	    ಪಕ್ಷ NFA_COL:
	    ಪಕ್ಷ NFA_COL_GT:
	    ಪಕ್ಷ NFA_COL_LT:
	    ಪಕ್ಷ NFA_VCOL:
	    ಪಕ್ಷ NFA_VCOL_GT:
	    ಪಕ್ಷ NFA_VCOL_LT:
	    ಪಕ್ಷ NFA_MARK:
	    ಪಕ್ಷ NFA_MARK_GT:
	    ಪಕ್ಷ NFA_MARK_LT:

	    ಪಕ್ಷ NFA_MOPEN:
	    ಪಕ್ಷ NFA_MOPEN1:
	    ಪಕ್ಷ NFA_MOPEN2:
	    ಪಕ್ಷ NFA_MOPEN3:
	    ಪಕ್ಷ NFA_MOPEN4:
	    ಪಕ್ಷ NFA_MOPEN5:
	    ಪಕ್ಷ NFA_MOPEN6:
	    ಪಕ್ಷ NFA_MOPEN7:
	    ಪಕ್ಷ NFA_MOPEN8:
	    ಪಕ್ಷ NFA_MOPEN9:
	    ಪಕ್ಷ NFA_NOPEN:
#ifdef FEAT_SYN_HL
	    ಪಕ್ಷ NFA_ZOPEN:
	    ಪಕ್ಷ NFA_ZOPEN1:
	    ಪಕ್ಷ NFA_ZOPEN2:
	    ಪಕ್ಷ NFA_ZOPEN3:
	    ಪಕ್ಷ NFA_ZOPEN4:
	    ಪಕ್ಷ NFA_ZOPEN5:
	    ಪಕ್ಷ NFA_ZOPEN6:
	    ಪಕ್ಷ NFA_ZOPEN7:
	    ಪಕ್ಷ NFA_ZOPEN8:
	    ಪಕ್ಷ NFA_ZOPEN9:
#endif
		p = p->out;
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_SPLIT:
	    {
		ಪೂರ್ಣಾಙ್ಕ c1 = nfa_get_regstart(p->out, depth + 1);
		ಪೂರ್ಣಾಙ್ಕ c2 = nfa_get_regstart(p->out1, depth + 1);

		ಯದಿ (c1 == c2)
		    ಪ್ರತ್ಯಾಗಮ c1; // yes!
		ಪ್ರತ್ಯಾಗಮ 0;
	    }

	    ಸ್ವಭಾವ:
		ಯದಿ (p->c > 0)
		    ಪ್ರತ್ಯಾಗಮ p->c; // yes!
		ಪ್ರತ್ಯಾಗಮ 0;
	}
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Figure out if the NFA state list contains just literal text and nothing
 * else.  If so return a string in allocated memory with what must match after
 * regstart.  Otherwise return NULL.
 */
    ಸ್ಥಿರ char_u *
nfa_get_match_text(nfa_state_T *start)
{
    nfa_state_T *p = start;
    ಪೂರ್ಣಾಙ್ಕ		len = 0;
    char_u	*ret;
    char_u	*s;

    ಯದಿ (p->c != NFA_MOPEN)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ; // just in case
    p = p->out;
    ಯಾವತ್ (p->c > 0)
    {
	len += MB_CHAR2LEN(p->c);
	p = p->out;
    }
    ಯದಿ (p->c != NFA_MCLOSE || p->out->c != NFA_MATCH)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ret = alloc(len);
    ಯದಿ (ret == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    p = start->out->out; // skip first char, it goes into regstart
    s = ret;
    ಯಾವತ್ (p->c > 0)
    {
	ಯದಿ (has_mbyte)
	    s += (*mb_char2bytes)(p->c, s);
	ಅನ್ಯಥಾ
	    *s++ = p->c;
	p = p->out;
    }
    *s = NUL;
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Allocate more space for post_start.  Called when
 * running above the estimated number of states.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
realloc_post_list(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ   nstate_max = (ಪೂರ್ಣಾಙ್ಕ)(post_end - post_start);
    ಪೂರ್ಣಾಙ್ಕ   new_max;
    ಪೂರ್ಣಾಙ್ಕ   *new_start;
    ಪೂರ್ಣಾಙ್ಕ	  *old_start;

    // For weird patterns the number of states can be very high. Increasing by
    // 50% seems a reasonable compromise between memory use and speed.
    new_max = nstate_max * 3 / 2;
    new_start = ALLOC_MULT(ಪೂರ್ಣಾಙ್ಕ, new_max);
    ಯದಿ (new_start == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    mch_memmove(new_start, post_start, nstate_max * ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ));
    old_start = post_start;
    post_start = new_start;
    post_start_len = new_max;
    post_ptr = new_start + (post_ptr - old_start);
    post_end = post_start + new_max;
    vim_free(old_start);
    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
nfa_recognize_char_class(char_u *start, char_u *end, ಪೂರ್ಣಾಙ್ಕ extra_newl)
{
    ಪೂರ್ಣಾಙ್ಕ		newl;
    ಪೂರ್ಣಾಙ್ಕ		config = get_char_class_bits(start, end, &newl);

    ಯದಿ (config < 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (extra_newl == TRUE)
	newl = TRUE;

    ಯದಿ (newl == TRUE)
	extra_newl = NFA_ADD_NL;

    ವಿಕಲ್ಪ (config)
    {
	ಪಕ್ಷ CLASS_o9:
	    ಪ್ರತ್ಯಾಗಮ extra_newl + NFA_DIGIT;
	ಪಕ್ಷ CLASS_not |  CLASS_o9:
	    ಪ್ರತ್ಯಾಗಮ extra_newl + NFA_NDIGIT;
	ಪಕ್ಷ CLASS_af | CLASS_AF | CLASS_o9:
	    ಪ್ರತ್ಯಾಗಮ extra_newl + NFA_HEX;
	ಪಕ್ಷ CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    ಪ್ರತ್ಯಾಗಮ extra_newl + NFA_NHEX;
	ಪಕ್ಷ CLASS_o7:
	    ಪ್ರತ್ಯಾಗಮ extra_newl + NFA_OCTAL;
	ಪಕ್ಷ CLASS_not | CLASS_o7:
	    ಪ್ರತ್ಯಾಗಮ extra_newl + NFA_NOCTAL;
	ಪಕ್ಷ CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    ಪ್ರತ್ಯಾಗಮ extra_newl + NFA_WORD;
	ಪಕ್ಷ CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    ಪ್ರತ್ಯಾಗಮ extra_newl + NFA_NWORD;
	ಪಕ್ಷ CLASS_az | CLASS_AZ | CLASS_underscore:
	    ಪ್ರತ್ಯಾಗಮ extra_newl + NFA_HEAD;
	ಪಕ್ಷ CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    ಪ್ರತ್ಯಾಗಮ extra_newl + NFA_NHEAD;
	ಪಕ್ಷ CLASS_az | CLASS_AZ:
	    ಪ್ರತ್ಯಾಗಮ extra_newl + NFA_ALPHA;
	ಪಕ್ಷ CLASS_not | CLASS_az | CLASS_AZ:
	    ಪ್ರತ್ಯಾಗಮ extra_newl + NFA_NALPHA;
	ಪಕ್ಷ CLASS_az:
	   ಪ್ರತ್ಯಾಗಮ extra_newl + NFA_LOWER_IC;
	ಪಕ್ಷ CLASS_not | CLASS_az:
	    ಪ್ರತ್ಯಾಗಮ extra_newl + NFA_NLOWER_IC;
	ಪಕ್ಷ CLASS_AZ:
	    ಪ್ರತ್ಯಾಗಮ extra_newl + NFA_UPPER_IC;
	ಪಕ್ಷ CLASS_not | CLASS_AZ:
	    ಪ್ರತ್ಯಾಗಮ extra_newl + NFA_NUPPER_IC;
    }
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * Emits bytes in postfix notation: 'a,b,NFA_OR,c,NFA_OR' is
 * equivalent to 'a OR b OR c'
 *
 * NOTE! When changing this function, also update reg_equi_class()
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
nfa_emit_equi_class(ಪೂರ್ಣಾಙ್ಕ c)
{
#define EMIT2(c)    EMIT(c); EMIT(NFA_CONCAT);

    ಯದಿ (enc_utf8 || STRCMP(p_enc, "latin1") == 0
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
	ವಿಕಲ್ಪ (c)
	{
	    ಪಕ್ಷ 'A': ಪಕ್ಷ A_grave: ಪಕ್ಷ A_acute: ಪಕ್ಷ A_circumflex:
	    ಪಕ್ಷ A_virguilla: ಪಕ್ಷ A_diaeresis: ಪಕ್ಷ A_ring:
	    ಪಕ್ಷ 0x100: ಪಕ್ಷ 0x102: ಪಕ್ಷ 0x104: ಪಕ್ಷ 0x1cd:
	    ಪಕ್ಷ 0x1de: ಪಕ್ಷ 0x1e0: ಪಕ್ಷ 0x1fa: ಪಕ್ಷ 0x200:
	    ಪಕ್ಷ 0x202: ಪಕ್ಷ 0x226: ಪಕ್ಷ 0x23a: ಪಕ್ಷ 0x1e00:
	    ಪಕ್ಷ 0x1ea0: ಪಕ್ಷ 0x1ea2: ಪಕ್ಷ 0x1ea4: ಪಕ್ಷ 0x1ea6:
	    ಪಕ್ಷ 0x1ea8: ಪಕ್ಷ 0x1eaa: ಪಕ್ಷ 0x1eac: ಪಕ್ಷ 0x1eae:
	    ಪಕ್ಷ 0x1eb0: ಪಕ್ಷ 0x1eb2: ಪಕ್ಷ 0x1eb4: ಪಕ್ಷ 0x1eb6:
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
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'B': ಪಕ್ಷ 0x181: ಪಕ್ಷ 0x243: ಪಕ್ಷ 0x1e02:
	    ಪಕ್ಷ 0x1e04: ಪಕ್ಷ 0x1e06:
		    EMIT2('B')
		    EMIT2(0x181) EMIT2(0x243) EMIT2(0x1e02)
		    EMIT2(0x1e04) EMIT2(0x1e06)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'C': ಪಕ್ಷ C_cedilla: ಪಕ್ಷ 0x106: ಪಕ್ಷ 0x108:
	    ಪಕ್ಷ 0x10a: ಪಕ್ಷ 0x10c: ಪಕ್ಷ 0x187: ಪಕ್ಷ 0x23b:
	    ಪಕ್ಷ 0x1e08: ಪಕ್ಷ 0xa792:
		    EMIT2('C') EMIT2(C_cedilla)
		    EMIT2(0x106) EMIT2(0x108) EMIT2(0x10a)
		    EMIT2(0x10c) EMIT2(0x187) EMIT2(0x23b)
		    EMIT2(0x1e08) EMIT2(0xa792)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'D': ಪಕ್ಷ 0x10e: ಪಕ್ಷ 0x110: ಪಕ್ಷ 0x18a:
	    ಪಕ್ಷ 0x1e0a: ಪಕ್ಷ 0x1e0c: ಪಕ್ಷ 0x1e0e: ಪಕ್ಷ 0x1e10:
	    ಪಕ್ಷ 0x1e12:
		    EMIT2('D') EMIT2(0x10e) EMIT2(0x110) EMIT2(0x18a)
		    EMIT2(0x1e0a) EMIT2(0x1e0c) EMIT2(0x1e0e)
		    EMIT2(0x1e10) EMIT2(0x1e12)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'E': ಪಕ್ಷ E_grave: ಪಕ್ಷ E_acute: ಪಕ್ಷ E_circumflex:
	    ಪಕ್ಷ E_diaeresis: ಪಕ್ಷ 0x112: ಪಕ್ಷ 0x114: ಪಕ್ಷ 0x116:
	    ಪಕ್ಷ 0x118: ಪಕ್ಷ 0x11a: ಪಕ್ಷ 0x204: ಪಕ್ಷ 0x206:
	    ಪಕ್ಷ 0x228: ಪಕ್ಷ 0x246: ಪಕ್ಷ 0x1e14: ಪಕ್ಷ 0x1e16:
	    ಪಕ್ಷ 0x1e18: ಪಕ್ಷ 0x1e1a: ಪಕ್ಷ 0x1e1c: ಪಕ್ಷ 0x1eb8:
	    ಪಕ್ಷ 0x1eba: ಪಕ್ಷ 0x1ebc: ಪಕ್ಷ 0x1ebe: ಪಕ್ಷ 0x1ec0:
	    ಪಕ್ಷ 0x1ec2: ಪಕ್ಷ 0x1ec4: ಪಕ್ಷ 0x1ec6:
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
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'F': ಪಕ್ಷ 0x191: ಪಕ್ಷ 0x1e1e: ಪಕ್ಷ 0xa798:
		    EMIT2('F') EMIT2(0x191) EMIT2(0x1e1e) EMIT2(0xa798)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'G': ಪಕ್ಷ 0x11c: ಪಕ್ಷ 0x11e: ಪಕ್ಷ 0x120:
	    ಪಕ್ಷ 0x122: ಪಕ್ಷ 0x193: ಪಕ್ಷ 0x1e4: ಪಕ್ಷ 0x1e6:
	    ಪಕ್ಷ 0x1f4: ಪಕ್ಷ 0x1e20: ಪಕ್ಷ 0xa7a0:
		    EMIT2('G') EMIT2(0x11c) EMIT2(0x11e) EMIT2(0x120)
		    EMIT2(0x122) EMIT2(0x193) EMIT2(0x1e4)
		    EMIT2(0x1e6) EMIT2(0x1f4) EMIT2(0x1e20)
		    EMIT2(0xa7a0)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'H': ಪಕ್ಷ 0x124: ಪಕ್ಷ 0x126: ಪಕ್ಷ 0x21e:
	    ಪಕ್ಷ 0x1e22: ಪಕ್ಷ 0x1e24: ಪಕ್ಷ 0x1e26: ಪಕ್ಷ 0x1e28:
	    ಪಕ್ಷ 0x1e2a: ಪಕ್ಷ 0x2c67:
		    EMIT2('H') EMIT2(0x124) EMIT2(0x126) EMIT2(0x21e)
		    EMIT2(0x1e22) EMIT2(0x1e24) EMIT2(0x1e26)
		    EMIT2(0x1e28) EMIT2(0x1e2a) EMIT2(0x2c67)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'I': ಪಕ್ಷ I_grave: ಪಕ್ಷ I_acute: ಪಕ್ಷ I_circumflex:
	    ಪಕ್ಷ I_diaeresis: ಪಕ್ಷ 0x128: ಪಕ್ಷ 0x12a: ಪಕ್ಷ 0x12c:
	    ಪಕ್ಷ 0x12e: ಪಕ್ಷ 0x130: ಪಕ್ಷ 0x197: ಪಕ್ಷ 0x1cf:
	    ಪಕ್ಷ 0x208: ಪಕ್ಷ 0x20a: ಪಕ್ಷ 0x1e2c: ಪಕ್ಷ 0x1e2e:
	    ಪಕ್ಷ 0x1ec8: ಪಕ್ಷ 0x1eca:
		    EMIT2('I') EMIT2(I_grave) EMIT2(I_acute)
		    EMIT2(I_circumflex) EMIT2(I_diaeresis)
		    EMIT2(0x128) EMIT2(0x12a) EMIT2(0x12c)
		    EMIT2(0x12e) EMIT2(0x130) EMIT2(0x197)
		    EMIT2(0x1cf) EMIT2(0x208) EMIT2(0x20a)
		    EMIT2(0x1e2c) EMIT2(0x1e2e) EMIT2(0x1ec8)
		    EMIT2(0x1eca)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'J': ಪಕ್ಷ 0x134: ಪಕ್ಷ 0x248:
		    EMIT2('J') EMIT2(0x134) EMIT2(0x248)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'K': ಪಕ್ಷ 0x136: ಪಕ್ಷ 0x198: ಪಕ್ಷ 0x1e8: ಪಕ್ಷ 0x1e30:
	    ಪಕ್ಷ 0x1e32: ಪಕ್ಷ 0x1e34: ಪಕ್ಷ 0x2c69: ಪಕ್ಷ 0xa740:
		    EMIT2('K') EMIT2(0x136) EMIT2(0x198) EMIT2(0x1e8)
		    EMIT2(0x1e30) EMIT2(0x1e32) EMIT2(0x1e34)
		    EMIT2(0x2c69) EMIT2(0xa740)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'L': ಪಕ್ಷ 0x139: ಪಕ್ಷ 0x13b: ಪಕ್ಷ 0x13d:
	    ಪಕ್ಷ 0x13f: ಪಕ್ಷ 0x141: ಪಕ್ಷ 0x23d: ಪಕ್ಷ 0x1e36:
	    ಪಕ್ಷ 0x1e38: ಪಕ್ಷ 0x1e3a: ಪಕ್ಷ 0x1e3c: ಪಕ್ಷ 0x2c60:
		    EMIT2('L') EMIT2(0x139) EMIT2(0x13b)
		    EMIT2(0x13d) EMIT2(0x13f) EMIT2(0x141)
		    EMIT2(0x23d) EMIT2(0x1e36) EMIT2(0x1e38)
		    EMIT2(0x1e3a) EMIT2(0x1e3c) EMIT2(0x2c60)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'M': ಪಕ್ಷ 0x1e3e: ಪಕ್ಷ 0x1e40: ಪಕ್ಷ 0x1e42:
		    EMIT2('M') EMIT2(0x1e3e) EMIT2(0x1e40)
		    EMIT2(0x1e42)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'N': ಪಕ್ಷ N_virguilla:
	    ಪಕ್ಷ 0x143: ಪಕ್ಷ 0x145: ಪಕ್ಷ 0x147: ಪಕ್ಷ 0x1f8:
	    ಪಕ್ಷ 0x1e44: ಪಕ್ಷ 0x1e46: ಪಕ್ಷ 0x1e48: ಪಕ್ಷ 0x1e4a:
	    ಪಕ್ಷ 0xa7a4:
		    EMIT2('N') EMIT2(N_virguilla)
		    EMIT2(0x143) EMIT2(0x145) EMIT2(0x147)
		    EMIT2(0x1f8) EMIT2(0x1e44) EMIT2(0x1e46)
		    EMIT2(0x1e48) EMIT2(0x1e4a) EMIT2(0xa7a4)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'O': ಪಕ್ಷ O_grave: ಪಕ್ಷ O_acute: ಪಕ್ಷ O_circumflex:
	    ಪಕ್ಷ O_virguilla: ಪಕ್ಷ O_diaeresis: ಪಕ್ಷ O_slash:
	    ಪಕ್ಷ 0x14c: ಪಕ್ಷ 0x14e: ಪಕ್ಷ 0x150: ಪಕ್ಷ 0x19f:
	    ಪಕ್ಷ 0x1a0: ಪಕ್ಷ 0x1d1: ಪಕ್ಷ 0x1ea: ಪಕ್ಷ 0x1ec:
	    ಪಕ್ಷ 0x1fe: ಪಕ್ಷ 0x20c: ಪಕ್ಷ 0x20e: ಪಕ್ಷ 0x22a:
	    ಪಕ್ಷ 0x22c: ಪಕ್ಷ 0x22e: ಪಕ್ಷ 0x230: ಪಕ್ಷ 0x1e4c:
	    ಪಕ್ಷ 0x1e4e: ಪಕ್ಷ 0x1e50: ಪಕ್ಷ 0x1e52: ಪಕ್ಷ 0x1ecc:
	    ಪಕ್ಷ 0x1ece: ಪಕ್ಷ 0x1ed0: ಪಕ್ಷ 0x1ed2: ಪಕ್ಷ 0x1ed4:
	    ಪಕ್ಷ 0x1ed6: ಪಕ್ಷ 0x1ed8: ಪಕ್ಷ 0x1eda: ಪಕ್ಷ 0x1edc:
	    ಪಕ್ಷ 0x1ede: ಪಕ್ಷ 0x1ee0: ಪಕ್ಷ 0x1ee2:
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
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'P': ಪಕ್ಷ 0x1a4: ಪಕ್ಷ 0x1e54: ಪಕ್ಷ 0x1e56: ಪಕ್ಷ 0x2c63:
		    EMIT2('P') EMIT2(0x1a4) EMIT2(0x1e54) EMIT2(0x1e56)
		    EMIT2(0x2c63)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'Q': ಪಕ್ಷ 0x24a:
		    EMIT2('Q') EMIT2(0x24a)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'R': ಪಕ್ಷ 0x154: ಪಕ್ಷ 0x156: ಪಕ್ಷ 0x158: ಪಕ್ಷ 0x210:
	    ಪಕ್ಷ 0x212: ಪಕ್ಷ 0x24c: ಪಕ್ಷ 0x1e58: ಪಕ್ಷ 0x1e5a:
	    ಪಕ್ಷ 0x1e5c: ಪಕ್ಷ 0x1e5e: ಪಕ್ಷ 0x2c64: ಪಕ್ಷ 0xa7a6:
		    EMIT2('R') EMIT2(0x154) EMIT2(0x156) EMIT2(0x158)
		    EMIT2(0x210) EMIT2(0x212) EMIT2(0x24c) EMIT2(0x1e58)
		    EMIT2(0x1e5a) EMIT2(0x1e5c) EMIT2(0x1e5e) EMIT2(0x2c64)
		    EMIT2(0xa7a6)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'S': ಪಕ್ಷ 0x15a: ಪಕ್ಷ 0x15c: ಪಕ್ಷ 0x15e: ಪಕ್ಷ 0x160:
	    ಪಕ್ಷ 0x218: ಪಕ್ಷ 0x1e60: ಪಕ್ಷ 0x1e62: ಪಕ್ಷ 0x1e64:
	    ಪಕ್ಷ 0x1e66: ಪಕ್ಷ 0x1e68: ಪಕ್ಷ 0x2c7e: ಪಕ್ಷ 0xa7a8:
		    EMIT2('S') EMIT2(0x15a) EMIT2(0x15c) EMIT2(0x15e)
		    EMIT2(0x160) EMIT2(0x218) EMIT2(0x1e60) EMIT2(0x1e62)
		    EMIT2(0x1e64) EMIT2(0x1e66) EMIT2(0x1e68) EMIT2(0x2c7e)
		    EMIT2(0xa7a8)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'T': ಪಕ್ಷ 0x162: ಪಕ್ಷ 0x164: ಪಕ್ಷ 0x166: ಪಕ್ಷ 0x1ac:
	    ಪಕ್ಷ 0x1ae: ಪಕ್ಷ 0x21a: ಪಕ್ಷ 0x23e: ಪಕ್ಷ 0x1e6a: ಪಕ್ಷ 0x1e6c:
	    ಪಕ್ಷ 0x1e6e: ಪಕ್ಷ 0x1e70:
		    EMIT2('T') EMIT2(0x162) EMIT2(0x164) EMIT2(0x166)
		    EMIT2(0x1ac) EMIT2(0x1ae) EMIT2(0x23e) EMIT2(0x21a)
		    EMIT2(0x1e6a) EMIT2(0x1e6c) EMIT2(0x1e6e) EMIT2(0x1e70)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'U': ಪಕ್ಷ U_grave: ಪಕ್ಷ U_acute: ಪಕ್ಷ U_diaeresis:
	    ಪಕ್ಷ U_circumflex: ಪಕ್ಷ 0x168: ಪಕ್ಷ 0x16a: ಪಕ್ಷ 0x16c:
	    ಪಕ್ಷ 0x16e: ಪಕ್ಷ 0x170: ಪಕ್ಷ 0x172: ಪಕ್ಷ 0x1af:
	    ಪಕ್ಷ 0x1d3: ಪಕ್ಷ 0x1d5: ಪಕ್ಷ 0x1d7: ಪಕ್ಷ 0x1d9:
	    ಪಕ್ಷ 0x1db: ಪಕ್ಷ 0x214: ಪಕ್ಷ 0x216: ಪಕ್ಷ 0x244:
	    ಪಕ್ಷ 0x1e72: ಪಕ್ಷ 0x1e74: ಪಕ್ಷ 0x1e76: ಪಕ್ಷ 0x1e78:
	    ಪಕ್ಷ 0x1e7a: ಪಕ್ಷ 0x1ee4: ಪಕ್ಷ 0x1ee6: ಪಕ್ಷ 0x1ee8:
	    ಪಕ್ಷ 0x1eea: ಪಕ್ಷ 0x1eec: ಪಕ್ಷ 0x1eee: ಪಕ್ಷ 0x1ef0:
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
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'V': ಪಕ್ಷ 0x1b2: ಪಕ್ಷ 0x1e7c: ಪಕ್ಷ 0x1e7e:
		    EMIT2('V') EMIT2(0x1b2) EMIT2(0x1e7c) EMIT2(0x1e7e)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'W': ಪಕ್ಷ 0x174: ಪಕ್ಷ 0x1e80: ಪಕ್ಷ 0x1e82: ಪಕ್ಷ 0x1e84:
	    ಪಕ್ಷ 0x1e86: ಪಕ್ಷ 0x1e88:
		    EMIT2('W') EMIT2(0x174) EMIT2(0x1e80) EMIT2(0x1e82)
		    EMIT2(0x1e84) EMIT2(0x1e86) EMIT2(0x1e88)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'X': ಪಕ್ಷ 0x1e8a: ಪಕ್ಷ 0x1e8c:
		    EMIT2('X') EMIT2(0x1e8a) EMIT2(0x1e8c)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'Y': ಪಕ್ಷ Y_acute: ಪಕ್ಷ 0x176: ಪಕ್ಷ 0x178:
	    ಪಕ್ಷ 0x1b3: ಪಕ್ಷ 0x232: ಪಕ್ಷ 0x24e: ಪಕ್ಷ 0x1e8e:
	    ಪಕ್ಷ 0x1ef2: ಪಕ್ಷ 0x1ef4: ಪಕ್ಷ 0x1ef6: ಪಕ್ಷ 0x1ef8:
		    EMIT2('Y') EMIT2(Y_acute)
		    EMIT2(0x176) EMIT2(0x178) EMIT2(0x1b3)
		    EMIT2(0x232) EMIT2(0x24e) EMIT2(0x1e8e)
		    EMIT2(0x1ef2) EMIT2(0x1ef4) EMIT2(0x1ef6)
		    EMIT2(0x1ef8)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'Z': ಪಕ್ಷ 0x179: ಪಕ್ಷ 0x17b: ಪಕ್ಷ 0x17d:
	    ಪಕ್ಷ 0x1b5: ಪಕ್ಷ 0x1e90: ಪಕ್ಷ 0x1e92: ಪಕ್ಷ 0x1e94:
	    ಪಕ್ಷ 0x2c6b:
		    EMIT2('Z') EMIT2(0x179) EMIT2(0x17b) EMIT2(0x17d)
		    EMIT2(0x1b5) EMIT2(0x1e90) EMIT2(0x1e92)
		    EMIT2(0x1e94) EMIT2(0x2c6b)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'a': ಪಕ್ಷ a_grave: ಪಕ್ಷ a_acute: ಪಕ್ಷ a_circumflex:
	    ಪಕ್ಷ a_virguilla: ಪಕ್ಷ a_diaeresis: ಪಕ್ಷ a_ring:
	    ಪಕ್ಷ 0x101: ಪಕ್ಷ 0x103: ಪಕ್ಷ 0x105: ಪಕ್ಷ 0x1ce:
	    ಪಕ್ಷ 0x1df: ಪಕ್ಷ 0x1e1: ಪಕ್ಷ 0x1fb: ಪಕ್ಷ 0x201:
	    ಪಕ್ಷ 0x203: ಪಕ್ಷ 0x227: ಪಕ್ಷ 0x1d8f: ಪಕ್ಷ 0x1e01:
	    ಪಕ್ಷ 0x1e9a: ಪಕ್ಷ 0x1ea1: ಪಕ್ಷ 0x1ea3: ಪಕ್ಷ 0x1ea5:
	    ಪಕ್ಷ 0x1ea7: ಪಕ್ಷ 0x1ea9: ಪಕ್ಷ 0x1eab: ಪಕ್ಷ 0x1ead:
	    ಪಕ್ಷ 0x1eaf: ಪಕ್ಷ 0x1eb1: ಪಕ್ಷ 0x1eb3: ಪಕ್ಷ 0x1eb5:
	    ಪಕ್ಷ 0x1eb7: ಪಕ್ಷ 0x2c65:
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
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'b': ಪಕ್ಷ 0x180: ಪಕ್ಷ 0x253: ಪಕ್ಷ 0x1d6c: ಪಕ್ಷ 0x1d80:
	    ಪಕ್ಷ 0x1e03: ಪಕ್ಷ 0x1e05: ಪಕ್ಷ 0x1e07:
		    EMIT2('b') EMIT2(0x180) EMIT2(0x253) EMIT2(0x1d6c)
		    EMIT2(0x1d80) EMIT2(0x1e03) EMIT2(0x1e05) EMIT2(0x1e07)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'c': ಪಕ್ಷ c_cedilla: ಪಕ್ಷ 0x107: ಪಕ್ಷ 0x109: ಪಕ್ಷ 0x10b:
	    ಪಕ್ಷ 0x10d: ಪಕ್ಷ 0x188: ಪಕ್ಷ 0x23c: ಪಕ್ಷ 0x1e09: ಪಕ್ಷ 0xa793:
	    ಪಕ್ಷ 0xa794:
		    EMIT2('c') EMIT2(c_cedilla)
		    EMIT2(0x107) EMIT2(0x109) EMIT2(0x10b)
		    EMIT2(0x10d) EMIT2(0x188) EMIT2(0x23c)
		    EMIT2(0x1e09) EMIT2(0xa793) EMIT2(0xa794)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'd': ಪಕ್ಷ 0x10f: ಪಕ್ಷ 0x111: ಪಕ್ಷ 0x257: ಪಕ್ಷ 0x1d6d:
	    ಪಕ್ಷ 0x1d81: ಪಕ್ಷ 0x1d91: ಪಕ್ಷ 0x1e0b: ಪಕ್ಷ 0x1e0d: ಪಕ್ಷ 0x1e0f:
	    ಪಕ್ಷ 0x1e11: ಪಕ್ಷ 0x1e13:
		    EMIT2('d') EMIT2(0x10f) EMIT2(0x111)
		    EMIT2(0x257) EMIT2(0x1d6d) EMIT2(0x1d81)
		    EMIT2(0x1d91) EMIT2(0x1e0b) EMIT2(0x1e0d)
		    EMIT2(0x1e0f) EMIT2(0x1e11) EMIT2(0x1e13)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'e': ಪಕ್ಷ e_grave: ಪಕ್ಷ e_acute: ಪಕ್ಷ e_circumflex:
	    ಪಕ್ಷ e_diaeresis: ಪಕ್ಷ 0x113: ಪಕ್ಷ 0x115: ಪಕ್ಷ 0x117:
	    ಪಕ್ಷ 0x119: ಪಕ್ಷ 0x11b: ಪಕ್ಷ 0x205: ಪಕ್ಷ 0x207:
	    ಪಕ್ಷ 0x229: ಪಕ್ಷ 0x247: ಪಕ್ಷ 0x1d92: ಪಕ್ಷ 0x1e15:
	    ಪಕ್ಷ 0x1e17: ಪಕ್ಷ 0x1e19: ಪಕ್ಷ 0x1e1b: ಪಕ್ಷ 0x1e1d:
	    ಪಕ್ಷ 0x1eb9: ಪಕ್ಷ 0x1ebb: ಪಕ್ಷ 0x1ebd: ಪಕ್ಷ 0x1ebf:
	    ಪಕ್ಷ 0x1ec1: ಪಕ್ಷ 0x1ec3: ಪಕ್ಷ 0x1ec5: ಪಕ್ಷ 0x1ec7:
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
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'f': ಪಕ್ಷ 0x192: ಪಕ್ಷ 0x1d6e: ಪಕ್ಷ 0x1d82:
	    ಪಕ್ಷ 0x1e1f: ಪಕ್ಷ 0xa799:
		    EMIT2('f') EMIT2(0x192) EMIT2(0x1d6e) EMIT2(0x1d82)
		    EMIT2(0x1e1f) EMIT2(0xa799)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'g': ಪಕ್ಷ 0x11d: ಪಕ್ಷ 0x11f: ಪಕ್ಷ 0x121: ಪಕ್ಷ 0x123:
	    ಪಕ್ಷ 0x1e5: ಪಕ್ಷ 0x1e7: ಪಕ್ಷ 0x1f5: ಪಕ್ಷ 0x260: ಪಕ್ಷ 0x1d83:
	    ಪಕ್ಷ 0x1e21: ಪಕ್ಷ 0xa7a1:
		    EMIT2('g') EMIT2(0x11d) EMIT2(0x11f) EMIT2(0x121)
		    EMIT2(0x123) EMIT2(0x1e5) EMIT2(0x1e7)
		    EMIT2(0x1f5) EMIT2(0x260) EMIT2(0x1d83)
		    EMIT2(0x1e21) EMIT2(0xa7a1)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'h': ಪಕ್ಷ 0x125: ಪಕ್ಷ 0x127: ಪಕ್ಷ 0x21f: ಪಕ್ಷ 0x1e23:
	    ಪಕ್ಷ 0x1e25: ಪಕ್ಷ 0x1e27: ಪಕ್ಷ 0x1e29: ಪಕ್ಷ 0x1e2b:
	    ಪಕ್ಷ 0x1e96: ಪಕ್ಷ 0x2c68: ಪಕ್ಷ 0xa795:
		    EMIT2('h') EMIT2(0x125) EMIT2(0x127) EMIT2(0x21f)
		    EMIT2(0x1e23) EMIT2(0x1e25) EMIT2(0x1e27)
		    EMIT2(0x1e29) EMIT2(0x1e2b) EMIT2(0x1e96)
		    EMIT2(0x2c68) EMIT2(0xa795)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'i': ಪಕ್ಷ i_grave: ಪಕ್ಷ i_acute: ಪಕ್ಷ i_circumflex:
	    ಪಕ್ಷ i_diaeresis: ಪಕ್ಷ 0x129: ಪಕ್ಷ 0x12b: ಪಕ್ಷ 0x12d:
	    ಪಕ್ಷ 0x12f: ಪಕ್ಷ 0x1d0: ಪಕ್ಷ 0x209: ಪಕ್ಷ 0x20b:
	    ಪಕ್ಷ 0x268: ಪಕ್ಷ 0x1d96: ಪಕ್ಷ 0x1e2d: ಪಕ್ಷ 0x1e2f:
	    ಪಕ್ಷ 0x1ec9: ಪಕ್ಷ 0x1ecb:
		    EMIT2('i') EMIT2(i_grave) EMIT2(i_acute)
		    EMIT2(i_circumflex) EMIT2(i_diaeresis)
		    EMIT2(0x129) EMIT2(0x12b) EMIT2(0x12d)
		    EMIT2(0x12f) EMIT2(0x1d0) EMIT2(0x209)
		    EMIT2(0x20b) EMIT2(0x268) EMIT2(0x1d96)
		    EMIT2(0x1e2d) EMIT2(0x1e2f) EMIT2(0x1ec9)
		    EMIT2(0x1ecb)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'j': ಪಕ್ಷ 0x135: ಪಕ್ಷ 0x1f0: ಪಕ್ಷ 0x249:
		    EMIT2('j') EMIT2(0x135) EMIT2(0x1f0) EMIT2(0x249)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'k': ಪಕ್ಷ 0x137: ಪಕ್ಷ 0x199: ಪಕ್ಷ 0x1e9: ಪಕ್ಷ 0x1d84:
	    ಪಕ್ಷ 0x1e31: ಪಕ್ಷ 0x1e33: ಪಕ್ಷ 0x1e35: ಪಕ್ಷ 0x2c6a: ಪಕ್ಷ 0xa741:
		    EMIT2('k') EMIT2(0x137) EMIT2(0x199) EMIT2(0x1e9)
		    EMIT2(0x1d84) EMIT2(0x1e31) EMIT2(0x1e33)
		    EMIT2(0x1e35) EMIT2(0x2c6a) EMIT2(0xa741)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'l': ಪಕ್ಷ 0x13a: ಪಕ್ಷ 0x13c: ಪಕ್ಷ 0x13e: ಪಕ್ಷ 0x140:
	    ಪಕ್ಷ 0x142: ಪಕ್ಷ 0x19a: ಪಕ್ಷ 0x1e37: ಪಕ್ಷ 0x1e39: ಪಕ್ಷ 0x1e3b:
	    ಪಕ್ಷ 0x1e3d: ಪಕ್ಷ 0x2c61:
		    EMIT2('l') EMIT2(0x13a) EMIT2(0x13c)
		    EMIT2(0x13e) EMIT2(0x140) EMIT2(0x142)
		    EMIT2(0x19a) EMIT2(0x1e37) EMIT2(0x1e39)
		    EMIT2(0x1e3b) EMIT2(0x1e3d) EMIT2(0x2c61)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'm': ಪಕ್ಷ 0x1d6f: ಪಕ್ಷ 0x1e3f: ಪಕ್ಷ 0x1e41: ಪಕ್ಷ 0x1e43:
		    EMIT2('m') EMIT2(0x1d6f) EMIT2(0x1e3f)
		    EMIT2(0x1e41) EMIT2(0x1e43)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'n': ಪಕ್ಷ n_virguilla: ಪಕ್ಷ 0x144: ಪಕ್ಷ 0x146: ಪಕ್ಷ 0x148:
	    ಪಕ್ಷ 0x149: ಪಕ್ಷ 0x1f9: ಪಕ್ಷ 0x1d70: ಪಕ್ಷ 0x1d87: ಪಕ್ಷ 0x1e45:
	    ಪಕ್ಷ 0x1e47: ಪಕ್ಷ 0x1e49: ಪಕ್ಷ 0x1e4b: ಪಕ್ಷ 0xa7a5:
		    EMIT2('n') EMIT2(n_virguilla)
		    EMIT2(0x144) EMIT2(0x146) EMIT2(0x148)
		    EMIT2(0x149) EMIT2(0x1f9) EMIT2(0x1d70)
		    EMIT2(0x1d87) EMIT2(0x1e45) EMIT2(0x1e47)
		    EMIT2(0x1e49) EMIT2(0x1e4b) EMIT2(0xa7a5)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'o': ಪಕ್ಷ o_grave: ಪಕ್ಷ o_acute: ಪಕ್ಷ o_circumflex:
	    ಪಕ್ಷ o_virguilla: ಪಕ್ಷ o_diaeresis: ಪಕ್ಷ o_slash:
	    ಪಕ್ಷ 0x14d: ಪಕ್ಷ 0x14f: ಪಕ್ಷ 0x151: ಪಕ್ಷ 0x1a1:
	    ಪಕ್ಷ 0x1d2: ಪಕ್ಷ 0x1eb: ಪಕ್ಷ 0x1ed: ಪಕ್ಷ 0x1ff:
	    ಪಕ್ಷ 0x20d: ಪಕ್ಷ 0x20f: ಪಕ್ಷ 0x22b: ಪಕ್ಷ 0x22d:
	    ಪಕ್ಷ 0x22f: ಪಕ್ಷ 0x231: ಪಕ್ಷ 0x275: ಪಕ್ಷ 0x1e4d:
	    ಪಕ್ಷ 0x1e4f: ಪಕ್ಷ 0x1e51: ಪಕ್ಷ 0x1e53: ಪಕ್ಷ 0x1ecd:
	    ಪಕ್ಷ 0x1ecf: ಪಕ್ಷ 0x1ed1: ಪಕ್ಷ 0x1ed3: ಪಕ್ಷ 0x1ed5:
	    ಪಕ್ಷ 0x1ed7: ಪಕ್ಷ 0x1ed9: ಪಕ್ಷ 0x1edb: ಪಕ್ಷ 0x1edd:
	    ಪಕ್ಷ 0x1edf: ಪಕ್ಷ 0x1ee1: ಪಕ್ಷ 0x1ee3:
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
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'p': ಪಕ್ಷ 0x1a5: ಪಕ್ಷ 0x1d71: ಪಕ್ಷ 0x1d7d: ಪಕ್ಷ 0x1d88:
	    ಪಕ್ಷ 0x1e55: ಪಕ್ಷ 0x1e57:
		    EMIT2('p') EMIT2(0x1a5) EMIT2(0x1d71) EMIT2(0x1d7d)
		    EMIT2(0x1d88) EMIT2(0x1e55) EMIT2(0x1e57)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'q': ಪಕ್ಷ 0x24b: ಪಕ್ಷ 0x2a0:
		    EMIT2('q') EMIT2(0x24b) EMIT2(0x2a0)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'r': ಪಕ್ಷ 0x155: ಪಕ್ಷ 0x157: ಪಕ್ಷ 0x159: ಪಕ್ಷ 0x211:
	    ಪಕ್ಷ 0x213: ಪಕ್ಷ 0x24d: ಪಕ್ಷ 0x27d: ಪಕ್ಷ 0x1d72: ಪಕ್ಷ 0x1d73:
	    ಪಕ್ಷ 0x1d89: ಪಕ್ಷ 0x1e59: ಪಕ್ಷ 0x1e5b: ಪಕ್ಷ 0x1e5d: ಪಕ್ಷ 0x1e5f:
	    ಪಕ್ಷ 0xa7a7:
		    EMIT2('r') EMIT2(0x155) EMIT2(0x157) EMIT2(0x159)
		    EMIT2(0x211) EMIT2(0x213) EMIT2(0x24d) EMIT2(0x27d)
		    EMIT2(0x1d72) EMIT2(0x1d73) EMIT2(0x1d89) EMIT2(0x1e59)
		    EMIT2(0x1e5b) EMIT2(0x1e5d) EMIT2(0x1e5f) EMIT2(0xa7a7)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 's': ಪಕ್ಷ 0x15b: ಪಕ್ಷ 0x15d: ಪಕ್ಷ 0x15f: ಪಕ್ಷ 0x161:
	    ಪಕ್ಷ 0x219: ಪಕ್ಷ 0x23f: ಪಕ್ಷ 0x1d74: ಪಕ್ಷ 0x1d8a: ಪಕ್ಷ 0x1e61:
	    ಪಕ್ಷ 0x1e63: ಪಕ್ಷ 0x1e65: ಪಕ್ಷ 0x1e67: ಪಕ್ಷ 0x1e69: ಪಕ್ಷ 0xa7a9:
		    EMIT2('s') EMIT2(0x15b) EMIT2(0x15d) EMIT2(0x15f)
		    EMIT2(0x161) EMIT2(0x219) EMIT2(0x23f) EMIT2(0x1d74)
		    EMIT2(0x1d8a) EMIT2(0x1e61) EMIT2(0x1e63) EMIT2(0x1e65)
		    EMIT2(0x1e67) EMIT2(0x1e69) EMIT2(0xa7a9)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 't': ಪಕ್ಷ 0x163: ಪಕ್ಷ 0x165: ಪಕ್ಷ 0x167: ಪಕ್ಷ 0x1ab:
	    ಪಕ್ಷ 0x1ad: ಪಕ್ಷ 0x21b: ಪಕ್ಷ 0x288: ಪಕ್ಷ 0x1d75: ಪಕ್ಷ 0x1e6b:
	    ಪಕ್ಷ 0x1e6d: ಪಕ್ಷ 0x1e6f: ಪಕ್ಷ 0x1e71: ಪಕ್ಷ 0x1e97: ಪಕ್ಷ 0x2c66:
		    EMIT2('t') EMIT2(0x163) EMIT2(0x165) EMIT2(0x167)
		    EMIT2(0x1ab) EMIT2(0x1ad) EMIT2(0x21b) EMIT2(0x288)
		    EMIT2(0x1d75) EMIT2(0x1e6b) EMIT2(0x1e6d) EMIT2(0x1e6f)
		    EMIT2(0x1e71) EMIT2(0x1e97) EMIT2(0x2c66)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'u': ಪಕ್ಷ u_grave: ಪಕ್ಷ u_acute: ಪಕ್ಷ u_circumflex:
	    ಪಕ್ಷ u_diaeresis: ಪಕ್ಷ 0x169: ಪಕ್ಷ 0x16b: ಪಕ್ಷ 0x16d:
	    ಪಕ್ಷ 0x16f: ಪಕ್ಷ 0x171: ಪಕ್ಷ 0x173: ಪಕ್ಷ 0x1b0: ಪಕ್ಷ 0x1d4:
	    ಪಕ್ಷ 0x1d6: ಪಕ್ಷ 0x1d8: ಪಕ್ಷ 0x1da: ಪಕ್ಷ 0x1dc: ಪಕ್ಷ 0x215:
	    ಪಕ್ಷ 0x217: ಪಕ್ಷ 0x289: ಪಕ್ಷ 0x1d7e: ಪಕ್ಷ 0x1d99: ಪಕ್ಷ 0x1e73:
	    ಪಕ್ಷ 0x1e75: ಪಕ್ಷ 0x1e77: ಪಕ್ಷ 0x1e79: ಪಕ್ಷ 0x1e7b:
	    ಪಕ್ಷ 0x1ee5: ಪಕ್ಷ 0x1ee7: ಪಕ್ಷ 0x1ee9: ಪಕ್ಷ 0x1eeb:
	    ಪಕ್ಷ 0x1eed: ಪಕ್ಷ 0x1eef: ಪಕ್ಷ 0x1ef1:
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
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'v': ಪಕ್ಷ 0x28b: ಪಕ್ಷ 0x1d8c: ಪಕ್ಷ 0x1e7d: ಪಕ್ಷ 0x1e7f:
		    EMIT2('v') EMIT2(0x28b) EMIT2(0x1d8c) EMIT2(0x1e7d)
		    EMIT2(0x1e7f)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'w': ಪಕ್ಷ 0x175: ಪಕ್ಷ 0x1e81: ಪಕ್ಷ 0x1e83: ಪಕ್ಷ 0x1e85:
	    ಪಕ್ಷ 0x1e87: ಪಕ್ಷ 0x1e89: ಪಕ್ಷ 0x1e98:
		    EMIT2('w') EMIT2(0x175) EMIT2(0x1e81) EMIT2(0x1e83)
		    EMIT2(0x1e85) EMIT2(0x1e87) EMIT2(0x1e89) EMIT2(0x1e98)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'x': ಪಕ್ಷ 0x1e8b: ಪಕ್ಷ 0x1e8d:
		    EMIT2('x') EMIT2(0x1e8b) EMIT2(0x1e8d)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'y': ಪಕ್ಷ y_acute: ಪಕ್ಷ y_diaeresis: ಪಕ್ಷ 0x177:
	    ಪಕ್ಷ 0x1b4: ಪಕ್ಷ 0x233: ಪಕ್ಷ 0x24f: ಪಕ್ಷ 0x1e8f:
	    ಪಕ್ಷ 0x1e99: ಪಕ್ಷ 0x1ef3: ಪಕ್ಷ 0x1ef5: ಪಕ್ಷ 0x1ef7:
	    ಪಕ್ಷ 0x1ef9:
		    EMIT2('y') EMIT2(y_acute) EMIT2(y_diaeresis)
		    EMIT2(0x177) EMIT2(0x1b4) EMIT2(0x233) EMIT2(0x24f)
		    EMIT2(0x1e8f) EMIT2(0x1e99) EMIT2(0x1ef3)
		    EMIT2(0x1ef5) EMIT2(0x1ef7) EMIT2(0x1ef9)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    ಪಕ್ಷ 'z': ಪಕ್ಷ 0x17a: ಪಕ್ಷ 0x17c: ಪಕ್ಷ 0x17e: ಪಕ್ಷ 0x1b6:
	    ಪಕ್ಷ 0x1d76: ಪಕ್ಷ 0x1d8e: ಪಕ್ಷ 0x1e91: ಪಕ್ಷ 0x1e93:
	    ಪಕ್ಷ 0x1e95: ಪಕ್ಷ 0x2c6c:
		    EMIT2('z') EMIT2(0x17a) EMIT2(0x17c) EMIT2(0x17e)
		    EMIT2(0x1b6) EMIT2(0x1d76) EMIT2(0x1d8e) EMIT2(0x1e91)
		    EMIT2(0x1e93) EMIT2(0x1e95) EMIT2(0x2c6c)
		    ಪ್ರತ್ಯಾಗಮ OK;

	    // default: character itself
	}
    }

    EMIT2(c);
    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
nfa_regatom(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		charclass;
    ಪೂರ್ಣಾಙ್ಕ		equiclass;
    ಪೂರ್ಣಾಙ್ಕ		collclass;
    ಪೂರ್ಣಾಙ್ಕ		got_coll_char;
    char_u	*p;
    char_u	*endp;
    char_u	*old_regparse = regparse;
    ಪೂರ್ಣಾಙ್ಕ		extra = 0;
    ಪೂರ್ಣಾಙ್ಕ		emit_range;
    ಪೂರ್ಣಾಙ್ಕ		negated;
    ಪೂರ್ಣಾಙ್ಕ		result;
    ಪೂರ್ಣಾಙ್ಕ		startc = -1;
    ಪೂರ್ಣಾಙ್ಕ		save_prev_at_start = prev_at_start;

    c = getchr();
    ವಿಕಲ್ಪ (c)
    {
	ಪಕ್ಷ NUL:
	    EMSG_RET_FAIL(_(e_nfa_regexp_end_encountered_prematurely));

	ಪಕ್ಷ Magic('^'):
	    EMIT(NFA_BOL);
	    ಭಙ್ಗ;

	ಪಕ್ಷ Magic('$'):
	    EMIT(NFA_EOL);
#ಯದಿ defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#endif
	    ಭಙ್ಗ;

	ಪಕ್ಷ Magic('<'):
	    EMIT(NFA_BOW);
	    ಭಙ್ಗ;

	ಪಕ್ಷ Magic('>'):
	    EMIT(NFA_EOW);
	    ಭಙ್ಗ;

	ಪಕ್ಷ Magic('_'):
	    c = no_Magic(getchr());
	    ಯದಿ (c == NUL)
		EMSG_RET_FAIL(_(e_nfa_regexp_end_encountered_prematurely));

	    ಯದಿ (c == '^')	// "\_^" is start-of-line
	    {
		EMIT(NFA_BOL);
		ಭಙ್ಗ;
	    }
	    ಯದಿ (c == '$')	// "\_$" is end-of-line
	    {
		EMIT(NFA_EOL);
#ಯದಿ defined(FEAT_SYN_HL)
		had_eol = TRUE;
#endif
		ಭಙ್ಗ;
	    }

	    extra = NFA_ADD_NL;

	    // "\_[" is collection plus newline
	    ಯದಿ (c == '[')
		ಗಚ್ಛ collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	/*
	 * Character classes.
	 */
	ಪಕ್ಷ Magic('.'):
	ಪಕ್ಷ Magic('i'):
	ಪಕ್ಷ Magic('I'):
	ಪಕ್ಷ Magic('k'):
	ಪಕ್ಷ Magic('K'):
	ಪಕ್ಷ Magic('f'):
	ಪಕ್ಷ Magic('F'):
	ಪಕ್ಷ Magic('p'):
	ಪಕ್ಷ Magic('P'):
	ಪಕ್ಷ Magic('s'):
	ಪಕ್ಷ Magic('S'):
	ಪಕ್ಷ Magic('d'):
	ಪಕ್ಷ Magic('D'):
	ಪಕ್ಷ Magic('x'):
	ಪಕ್ಷ Magic('X'):
	ಪಕ್ಷ Magic('o'):
	ಪಕ್ಷ Magic('O'):
	ಪಕ್ಷ Magic('w'):
	ಪಕ್ಷ Magic('W'):
	ಪಕ್ಷ Magic('h'):
	ಪಕ್ಷ Magic('H'):
	ಪಕ್ಷ Magic('a'):
	ಪಕ್ಷ Magic('A'):
	ಪಕ್ಷ Magic('l'):
	ಪಕ್ಷ Magic('L'):
	ಪಕ್ಷ Magic('u'):
	ಪಕ್ಷ Magic('U'):
	    p = vim_strchr(classchars, no_Magic(c));
	    ಯದಿ (p == ರಿಕ್ತ)
	    {
		ಯದಿ (extra == NFA_ADD_NL)
		{
		    semsg(_(e_nfa_regexp_invalid_character_class_nr), c);
		    rc_did_emsg = TRUE;
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		siemsg("Unknown character class char: %d", c);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }

	    // When '.' is followed by a composing char ignore the dot, so that
	    // the composing char is matched here.
	    ಯದಿ (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	    {
		old_regparse = regparse;
		c = getchr();
		ಗಚ್ಛ nfa_do_multibyte;
	    }
	    EMIT(nfa_classcodes[p - classchars]);
	    ಯದಿ (extra == NFA_ADD_NL)
	    {
		EMIT(NFA_NEWL);
		EMIT(NFA_OR);
		regflags |= RF_HASNL;
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ Magic('n'):
	    ಯದಿ (reg_string)
		// In a string "\n" matches a newline character.
		EMIT(NL);
	    ಅನ್ಯಥಾ
	    {
		// In buffer text "\n" matches the end of a line.
		EMIT(NFA_NEWL);
		regflags |= RF_HASNL;
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ Magic('('):
	    ಯದಿ (nfa_reg(REG_PAREN) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;	    // cascaded error
	    ಭಙ್ಗ;

	ಪಕ್ಷ Magic('|'):
	ಪಕ್ಷ Magic('&'):
	ಪಕ್ಷ Magic(')'):
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಪಕ್ಷ Magic('='):
	ಪಕ್ಷ Magic('?'):
	ಪಕ್ಷ Magic('+'):
	ಪಕ್ಷ Magic('@'):
	ಪಕ್ಷ Magic('*'):
	ಪಕ್ಷ Magic('{'):
	    // these should follow an atom, not form an atom
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಪಕ್ಷ Magic('~'):
	    {
		char_u	    *lp;

		// Previous substitute pattern.
		// Generated as "\%(pattern\)".
		ಯದಿ (reg_prev_sub == ರಿಕ್ತ)
		{
		    emsg(_(e_no_previous_substitute_regular_expression));
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		ಪ್ರತಿ (lp = reg_prev_sub; *lp != NUL; MB_CPTR_ADV(lp))
		{
		    EMIT(PTR2CHAR(lp));
		    ಯದಿ (lp != reg_prev_sub)
			EMIT(NFA_CONCAT);
		}
		EMIT(NFA_NOPEN);
		ಭಙ್ಗ;
	    }

	ಪಕ್ಷ Magic('1'):
	ಪಕ್ಷ Magic('2'):
	ಪಕ್ಷ Magic('3'):
	ಪಕ್ಷ Magic('4'):
	ಪಕ್ಷ Magic('5'):
	ಪಕ್ಷ Magic('6'):
	ಪಕ್ಷ Magic('7'):
	ಪಕ್ಷ Magic('8'):
	ಪಕ್ಷ Magic('9'):
	    {
		ಪೂರ್ಣಾಙ್ಕ refnum = no_Magic(c) - '1';

		ಯದಿ (!seen_endbrace(refnum + 1))
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		EMIT(NFA_BACKREF1 + refnum);
		rex.nfa_has_backref = TRUE;
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ Magic('z'):
	    c = no_Magic(getchr());
	    ವಿಕಲ್ಪ (c)
	    {
		ಪಕ್ಷ 's':
		    EMIT(NFA_ZSTART);
		    ಯದಿ (re_mult_next("\\zs") == FAIL)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    ಭಙ್ಗ;
		ಪಕ್ಷ 'e':
		    EMIT(NFA_ZEND);
		    rex.nfa_has_zend = TRUE;
		    ಯದಿ (re_mult_next("\\ze") == FAIL)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    ಭಙ್ಗ;
#ifdef FEAT_SYN_HL
		ಪಕ್ಷ '1':
		ಪಕ್ಷ '2':
		ಪಕ್ಷ '3':
		ಪಕ್ಷ '4':
		ಪಕ್ಷ '5':
		ಪಕ್ಷ '6':
		ಪಕ್ಷ '7':
		ಪಕ್ಷ '8':
		ಪಕ್ಷ '9':
		    // \z1...\z9
		    ಯದಿ ((reg_do_extmatch & REX_USE) == 0)
			EMSG_RET_FAIL(_(e_z1_z9_not_allowed_here));
		    EMIT(NFA_ZREF1 + (no_Magic(c) - '1'));
		    // No need to set rex.nfa_has_backref, the sub-matches don't
		    // change when \z1 .. \z9 matches or not.
		    re_has_z = REX_USE;
		    ಭಙ್ಗ;
		ಪಕ್ಷ '(':
		    // \z(
		    ಯದಿ ((reg_do_extmatch & REX_SET) == 0)
			EMSG_RET_FAIL(_(e_z_not_allowed_here));
		    ಯದಿ (nfa_reg(REG_ZPAREN) == FAIL)
			ಪ್ರತ್ಯಾಗಮ FAIL;	    // cascaded error
		    re_has_z = REX_SET;
		    ಭಙ್ಗ;
#endif
		ಸ್ವಭಾವ:
		    semsg(_(e_nfa_regexp_unknown_operator_z_chr), no_Magic(c));
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ Magic('%'):
	    c = no_Magic(getchr());
	    ವಿಕಲ್ಪ (c)
	    {
		// () without a back reference
		ಪಕ್ಷ '(':
		    ಯದಿ (nfa_reg(REG_NPAREN) == FAIL)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    EMIT(NFA_NOPEN);
		    ಭಙ್ಗ;

		ಪಕ್ಷ 'd':   // %d123 decimal
		ಪಕ್ಷ 'o':   // %o123 octal
		ಪಕ್ಷ 'x':   // %xab hex 2
		ಪಕ್ಷ 'u':   // %uabcd hex 4
		ಪಕ್ಷ 'U':   // %U1234abcd hex 8
		    {
			vimlong_T nr;

			ವಿಕಲ್ಪ (c)
			{
			    ಪಕ್ಷ 'd': nr = getdecchrs(); ಭಙ್ಗ;
			    ಪಕ್ಷ 'o': nr = getoctchrs(); ಭಙ್ಗ;
			    ಪಕ್ಷ 'x': nr = gethexchrs(2); ಭಙ್ಗ;
			    ಪಕ್ಷ 'u': nr = gethexchrs(4); ಭಙ್ಗ;
			    ಪಕ್ಷ 'U': nr = gethexchrs(8); ಭಙ್ಗ;
			    ಸ್ವಭಾವ:  nr = -1; ಭಙ್ಗ;
			}

			ಯದಿ (nr < 0 || nr > INT_MAX)
			    EMSG2_RET_FAIL(_(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			// A NUL is stored in the text as NL
			// TODO: what if a composing character follows?
			EMIT(nr == 0 ? 0x0a : (long)nr);
		    }
		    ಭಙ್ಗ;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		ಪಕ್ಷ '^':
		    EMIT(NFA_BOF);
		    ಭಙ್ಗ;

		ಪಕ್ಷ '$':
		    EMIT(NFA_EOF);
		    ಭಙ್ಗ;

		ಪಕ್ಷ '#':
		    ಯದಿ (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_start_of_pattern),
								  regparse[1]);
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    }
		    EMIT(NFA_CURSOR);
		    ಭಙ್ಗ;

		ಪಕ್ಷ 'V':
		    EMIT(NFA_VISUAL);
		    ಭಙ್ಗ;

		ಪಕ್ಷ 'C':
		    EMIT(NFA_ANY_COMPOSING);
		    ಭಙ್ಗ;

		ಪಕ್ಷ '[':
		    {
			ಪೂರ್ಣಾಙ್ಕ	    n;

			// \%[abc]
			ಪ್ರತಿ (n = 0; (c = peekchr()) != ']'; ++n)
			{
			    ಯದಿ (c == NUL)
				EMSG2_RET_FAIL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
			    // recursive call!
			    ಯದಿ (nfa_regatom() == FAIL)
				ಪ್ರತ್ಯಾಗಮ FAIL;
			}
			getchr();  // get the ]
			ಯದಿ (n == 0)
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
			ಭಙ್ಗ;
		    }

		ಸ್ವಭಾವ:
		    {
			long_u	n = 0;
			ಪೂರ್ಣಾಙ್ಕ	cmp = c;
			ಪೂರ್ಣಾಙ್ಕ	cur = FALSE;
			ಪೂರ್ಣಾಙ್ಕ	got_digit = FALSE;

			ಯದಿ (c == '<' || c == '>')
			    c = getchr();
			ಯದಿ (no_Magic(c) == '.')
			{
			    cur = TRUE;
			    c = getchr();
			}
			ಯಾವತ್ (VIM_ISDIGIT(c))
			{
			    long_u tmp;

			    ಯದಿ (cur)
			    {
				semsg(_(e_regexp_ಸಂಖ್ಯೆ_after_dot_pos_ಹುಡುಕು_chr),
								  no_Magic(c));
				ಪ್ರತ್ಯಾಗಮ FAIL;
			    }
			    tmp = n * 10 + (c - '0');

			    ಯದಿ (tmp < n)
			    {
				// overflow.
				emsg(_(e_percent_ಮೌಲ್ಯ_too_large));
				ಪ್ರತ್ಯಾಗಮ FAIL;
			    }
			    n = tmp;
			    c = getchr();
			    got_digit = TRUE;
			}
			ಯದಿ (c == 'l' || c == 'c' || c == 'v')
			{
			    long_u limit = INT_MAX;

			    ಯದಿ (!cur && !got_digit)
			    {
				semsg(_(e_nfa_regexp_missing_ಮೌಲ್ಯ_in_chr),
								  no_Magic(c));
				ಪ್ರತ್ಯಾಗಮ FAIL;
			    }
			    ಯದಿ (c == 'l')
			    {
				ಯದಿ (cur)
				    n = curwin->w_cursor.lnum;
				// \%{n}l  \%{n}<l  \%{n}>l
				EMIT(cmp == '<' ? NFA_LNUM_LT :
				     cmp == '>' ? NFA_LNUM_GT : NFA_LNUM);
				ಯದಿ (save_prev_at_start)
				    at_start = TRUE;
			    }
			    ಅನ್ಯಥಾ ಯದಿ (c == 'c')
			    {
				ಯದಿ (cur)
				{
				    n = curwin->w_cursor.col;
				    n++;
				}
				// \%{n}c  \%{n}<c  \%{n}>c
				EMIT(cmp == '<' ? NFA_COL_LT :
				     cmp == '>' ? NFA_COL_GT : NFA_COL);
			    }
			    ಅನ್ಯಥಾ
			    {
				ಯದಿ (cur)
				{
				    colnr_T vcol = 0;

				    getvvcol(curwin, &curwin->w_cursor,
							 ರಿಕ್ತ, ರಿಕ್ತ, &vcol, 0);
				    n = ++vcol;
				}
				// \%{n}v  \%{n}<v  \%{n}>v
				EMIT(cmp == '<' ? NFA_VCOL_LT :
				     cmp == '>' ? NFA_VCOL_GT : NFA_VCOL);
				limit = INT_MAX / MB_MAXBYTES;
			    }
			    ಯದಿ (n >= limit)
			    {
				emsg(_(e_percent_ಮೌಲ್ಯ_too_large));
				ಪ್ರತ್ಯಾಗಮ FAIL;
			    }
			    EMIT((ಪೂರ್ಣಾಙ್ಕ)n);
			    ಭಙ್ಗ;
			}
			ಅನ್ಯಥಾ ಯದಿ (no_Magic(c) == '\'' && n == 0)
			{
			    // \%'m  \%<'m  \%>'m
			    EMIT(cmp == '<' ? NFA_MARK_LT :
				 cmp == '>' ? NFA_MARK_GT : NFA_MARK);
			    EMIT(getchr());
			    ಭಙ್ಗ;
			}
		    }
		    semsg(_(e_nfa_regexp_unknown_operator_percent_chr),
								  no_Magic(c));
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ Magic('['):
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
	    ಯದಿ (*endp == ']')
	    {
		ಪೂರ್ಣಾಙ್ಕ plen;
		bool range_endpoint;
		/*
		 * Try to reverse engineer character classes. For example,
		 * recognize that [0-9] stands for \d and [A-Za-z_] for \h,
		 * and perform the necessary substitutions in the NFA.
		 */
		result = nfa_recognize_char_class(regparse, endp,
							 extra == NFA_ADD_NL);
		ಯದಿ (result != FAIL)
		{
		    ಯದಿ (result >= NFA_FIRST_NL && result <= NFA_LAST_NL)
		    {
			EMIT(result - NFA_ADD_NL);
			EMIT(NFA_NEWL);
			EMIT(NFA_OR);
		    }
		    ಅನ್ಯಥಾ
			EMIT(result);
		    regparse = endp;
		    MB_PTR_ADV(regparse);
		    ಪ್ರತ್ಯಾಗಮ OK;
		}
		/*
		 * Failed to recognize a character class. Use the simple
		 * version that turns [abc] into 'a' OR 'b' OR 'c'
		 */
		startc = -1;
		negated = FALSE;
		ಯದಿ (*regparse == '^')			// negated range
		{
		    negated = TRUE;
		    MB_PTR_ADV(regparse);
		    EMIT(NFA_START_NEG_COLL);
		}
		ಅನ್ಯಥಾ
		    EMIT(NFA_START_COLL);
		ಯದಿ (*regparse == '-')
		{
		    startc = '-';
		    EMIT(startc);
		    EMIT(NFA_CONCAT);
		    MB_PTR_ADV(regparse);
		}
		// Emit the OR branches for each character in the []
		emit_range = FALSE;
		ಯಾವತ್ (regparse < endp)
		{
		    ಪೂರ್ಣಾಙ್ಕ	    oldstartc = startc;
		    range_endpoint = false;

		    startc = -1;
		    got_coll_char = FALSE;
		    ಯದಿ (*regparse == '[')
		    {
			// Check for [: :], [= =], [. .]
			equiclass = collclass = 0;
			charclass = get_char_class(&regparse);
			ಯದಿ (charclass == CLASS_NONE)
			{
			    equiclass = get_equi_class(&regparse);
			    ಯದಿ (equiclass == 0)
				collclass = get_coll_element(&regparse);
			}

			// Character class like [:alpha:]
			ಯದಿ (charclass != CLASS_NONE)
			{
			    ವಿಕಲ್ಪ (charclass)
			    {
				ಪಕ್ಷ CLASS_ALNUM:
				    EMIT(NFA_CLASS_ALNUM);
				    ಭಙ್ಗ;
				ಪಕ್ಷ CLASS_ALPHA:
				    EMIT(NFA_CLASS_ALPHA);
				    ಭಙ್ಗ;
				ಪಕ್ಷ CLASS_BLANK:
				    EMIT(NFA_CLASS_BLANK);
				    ಭಙ್ಗ;
				ಪಕ್ಷ CLASS_CNTRL:
				    EMIT(NFA_CLASS_CNTRL);
				    ಭಙ್ಗ;
				ಪಕ್ಷ CLASS_DIGIT:
				    EMIT(NFA_CLASS_DIGIT);
				    ಭಙ್ಗ;
				ಪಕ್ಷ CLASS_GRAPH:
				    EMIT(NFA_CLASS_GRAPH);
				    ಭಙ್ಗ;
				ಪಕ್ಷ CLASS_LOWER:
				    wants_nfa = TRUE;
				    EMIT(NFA_CLASS_LOWER);
				    ಭಙ್ಗ;
				ಪಕ್ಷ CLASS_PRINT:
				    EMIT(NFA_CLASS_PRINT);
				    ಭಙ್ಗ;
				ಪಕ್ಷ CLASS_PUNCT:
				    EMIT(NFA_CLASS_PUNCT);
				    ಭಙ್ಗ;
				ಪಕ್ಷ CLASS_SPACE:
				    EMIT(NFA_CLASS_SPACE);
				    ಭಙ್ಗ;
				ಪಕ್ಷ CLASS_UPPER:
				    wants_nfa = TRUE;
				    EMIT(NFA_CLASS_UPPER);
				    ಭಙ್ಗ;
				ಪಕ್ಷ CLASS_XDIGIT:
				    EMIT(NFA_CLASS_XDIGIT);
				    ಭಙ್ಗ;
				ಪಕ್ಷ CLASS_TAB:
				    EMIT(NFA_CLASS_TAB);
				    ಭಙ್ಗ;
				ಪಕ್ಷ CLASS_RETURN:
				    EMIT(NFA_CLASS_RETURN);
				    ಭಙ್ಗ;
				ಪಕ್ಷ CLASS_BACKSPACE:
				    EMIT(NFA_CLASS_BACKSPACE);
				    ಭಙ್ಗ;
				ಪಕ್ಷ CLASS_ESCAPE:
				    EMIT(NFA_CLASS_ESCAPE);
				    ಭಙ್ಗ;
				ಪಕ್ಷ CLASS_IDENT:
				    EMIT(NFA_CLASS_IDENT);
				    ಭಙ್ಗ;
				ಪಕ್ಷ CLASS_KEYWORD:
				    EMIT(NFA_CLASS_KEYWORD);
				    ಭಙ್ಗ;
				ಪಕ್ಷ CLASS_FNAME:
				    EMIT(NFA_CLASS_FNAME);
				    ಭಙ್ಗ;
			    }
			    EMIT(NFA_CONCAT);
			    ಅನುವರ್ತ;
			}
			// Try equivalence class [=a=] and the like
			ಯದಿ (equiclass != 0)
			{
			    result = nfa_emit_equi_class(equiclass);
			    ಯದಿ (result == FAIL)
			    {
				// should never happen
				EMSG_RET_FAIL(_(e_error_building_nfa_with_equivalence_class));
			    }
			    ಅನುವರ್ತ;
			}
			// Try collating class like [. .]
			ಯದಿ (collclass != 0)
			{
			    startc = collclass;	 // allow [.a.]-x as a range
			    // Will emit the proper atom at the end of the
			    // while loop.
			}
		    }
		    // Try a range like 'a-x' or '\t-z'. Also allows '-' as a
		    // start character.
		    ಯದಿ (*regparse == '-' && oldstartc != -1)
		    {
			emit_range = TRUE;
			startc = oldstartc;
			MB_PTR_ADV(regparse);
			ಅನುವರ್ತ;	    // reading the end of the range
		    }

		    // Now handle simple and escaped characters.
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    ಯದಿ (*regparse == '\\'
			    && !reg_cpo_bsl
			    && regparse + 1 <= endp
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != ರಿಕ್ತ
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR, regparse[1])
								      != ರಿಕ್ತ)
			    )
			)
		    {
			MB_PTR_ADV(regparse);

			ಯದಿ (*regparse == 'n')
			    startc = (reg_string || emit_range
					|| regparse[1] == '-') ? NL : NFA_NEWL;
			ಅನ್ಯಥಾ ಯದಿ (*regparse == 'd'
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
				ಯದಿ (startc == INT_MAX)
				    EMSG_RET_FAIL(_(e_unicode_val_too_large));
				got_coll_char = TRUE;
				MB_PTR_BACK(old_regparse, regparse);
			    }
			    ಅನ್ಯಥಾ
			    {
				// \r,\t,\e,\b
				startc = backslash_trans(*regparse);
			    }
		    }

		    // Normal printable char
		    ಯದಿ (startc == -1)
			startc = PTR2CHAR(regparse);

		    // Previous char was '-', so this char is end of range.
		    ಯದಿ (emit_range)
		    {
			ಪೂರ್ಣಾಙ್ಕ	endc = startc;
			range_endpoint = true;

			startc = oldstartc;
			ಯದಿ (startc > endc)
			    EMSG_RET_FAIL(_(e_reverse_range_in_character_class));

			ಯದಿ (endc > startc + 2)
			{
			    // Emit a range instead of the sequence of
			    // individual characters.
			    ಯದಿ (startc == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    ಅನ್ಯಥಾ
				--post_ptr; // remove NFA_CONCAT
			    EMIT(endc);
			    EMIT(NFA_RANGE);
			    EMIT(NFA_CONCAT);
			}
			ಅನ್ಯಥಾ ಯದಿ (has_mbyte && ((*mb_char2len)(startc) > 1
				    || (*mb_char2len)(endc) > 1))
			{
			    // Emit the characters in the range.
			    // "startc" was already emitted, so skip it.
			    //
			    ಪ್ರತಿ (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			ಅನ್ಯಥಾ
			{
			    // Emit the range. "startc" was already emitted, so
			    // skip it.
			    ಪ್ರತಿ (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			emit_range = FALSE;
			startc = -1;
		    }
		    ಅನ್ಯಥಾ
		    {
			// This char (startc) is not part of a range. Just
			// emit it.
			// Normally, simply emit startc. But if we get char
			// code=0 from a collating char, then replace it with
			// 0x0a.
			// This is needed to completely mimic the behaviour of
			// the backtracking engine.
			ಯದಿ (startc == NFA_NEWL)
			{
			    // Line break can't be matched as part of the
			    // collection, add an OR below. But not for negated
			    // range.
			    ಯದಿ (!negated)
				extra = NFA_ADD_NL;
			}
			ಅನ್ಯಥಾ
			{
			    ಯದಿ (got_coll_char == TRUE && startc == 0)
			    {
				EMIT(0x0a);
				EMIT(NFA_CONCAT);
			    }
			    ಅನ್ಯಥಾ
			    {
				EMIT(startc);
				ಯದಿ (!(enc_utf8 && (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse)))))
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
		    ಯದಿ (!range_endpoint && enc_utf8 &&
			    (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse))))
		    {
			ಪೂರ್ಣಾಙ್ಕ i = utf_ptr2len(regparse);

			c = utf_ptr2char(regparse + i);

			// Add composing characters
			ಪ್ರತಿ (;;)
			{
			    ಯದಿ (c == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    ಅನ್ಯಥಾ
				EMIT(c);
			    EMIT(NFA_CONCAT);
			    ಯದಿ ((i += utf_char2len(c)) >= plen)
				ಭಙ್ಗ;
			    c = utf_ptr2char(regparse + i);
			}
			EMIT(NFA_COMPOSING);
			EMIT(NFA_CONCAT);
		    }
		    MB_PTR_ADV(regparse);
		} // while (p < endp)

		MB_PTR_BACK(old_regparse, regparse);
		ಯದಿ (*regparse == '-')	    // if last, '-' is just a char
		{
		    EMIT('-');
		    EMIT(NFA_CONCAT);
		}

		// skip the trailing ]
		regparse = endp;
		MB_PTR_ADV(regparse);

		// Mark end of the collection.
		ಯದಿ (negated == TRUE)
		    EMIT(NFA_END_NEG_COLL);
		ಅನ್ಯಥಾ
		    EMIT(NFA_END_COLL);

		// \_[] also matches \n but it's not negated
		ಯದಿ (extra == NFA_ADD_NL)
		{
		    EMIT(reg_string ? NL : NFA_NEWL);
		    EMIT(NFA_OR);
		}

		ಪ್ರತ್ಯಾಗಮ OK;
	    } // if exists closing ]

	    ಯದಿ (reg_strict)
		EMSG_RET_FAIL(_(e_missing_rsb_after_str_lsb));
	    // FALLTHROUGH

	ಸ್ವಭಾವ:
	    {
		ಪೂರ್ಣಾಙ್ಕ	plen;

nfa_do_multibyte:
		// plen is length of current char with composing chars
		ಯದಿ (enc_utf8 && ((*mb_char2len)(c)
			    != (plen = utfc_ptr2len(old_regparse))
						       || utf_iscomposing(c)))
		{
		    ಪೂರ್ಣಾಙ್ಕ i = 0;

		    // A base character plus composing characters, or just one
		    // or more composing characters.
		    // This requires creating a separate atom as if enclosing
		    // the characters in (), where NFA_COMPOSING is the ( and
		    // NFA_END_COMPOSING is the ). Note that right now we are
		    // building the postfix form, not the NFA itself;
		    // a composing char could be: a, b, c, NFA_COMPOSING
		    // where 'b' and 'c' are chars with codes > 256.
		    ಪ್ರತಿ (;;)
		    {
			EMIT(c);
			ಯದಿ (i > 0)
			    EMIT(NFA_CONCAT);
			ಯದಿ ((i += utf_char2len(c)) >= plen)
			    ಭಙ್ಗ;
			c = utf_ptr2char(old_regparse + i);
		    }
		    EMIT(NFA_COMPOSING);
		    regparse = old_regparse + plen;
		}
		ಅನ್ಯಥಾ
		{
		    c = no_Magic(c);
		    EMIT(c);
		}
		ಪ್ರತ್ಯಾಗಮ OK;
	    }
    }

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
nfa_regpiece(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		op;
    ಪೂರ್ಣಾಙ್ಕ		ret;
    long	minval, maxval;
    ಪೂರ್ಣಾಙ್ಕ		greedy = TRUE;      // Braces are prefixed with '-' ?
    parse_state_T old_state;
    parse_state_T new_state;
    long	c2;
    ಪೂರ್ಣಾಙ್ಕ		old_post_pos;
    ಪೂರ್ಣಾಙ್ಕ		my_post_start;
    ಪೂರ್ಣಾಙ್ಕ		quest;

    // Save the current parse state, so that we can use it if <atom>{m,n} is
    // next.
    save_parse_state(&old_state);

    // store current pos in the postfix form, for \{m,n} involving 0s
    my_post_start = (ಪೂರ್ಣಾಙ್ಕ)(post_ptr - post_start);

    ret = nfa_regatom();
    ಯದಿ (ret == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;	    // cascaded error

    op = peekchr();
    ಯದಿ (re_multi_type(op) == NOT_MULTI)
	ಪ್ರತ್ಯಾಗಮ OK;

    skipchr();
    ವಿಕಲ್ಪ (op)
    {
	ಪಕ್ಷ Magic('*'):
	    EMIT(NFA_STAR);
	    ಭಙ್ಗ;

	ಪಕ್ಷ Magic('+'):
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
	    ಯದಿ (nfa_regatom() == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    EMIT(NFA_STAR);
	    EMIT(NFA_CONCAT);
	    skipchr();		// skip the \+
	    ಭಙ್ಗ;

	ಪಕ್ಷ Magic('@'):
	    c2 = (long)getdecchrs();
	    op = no_Magic(getchr());
	    i = 0;
	    ವಿಕಲ್ಪ(op)
	    {
		ಪಕ್ಷ '=':
		    // \@=
		    i = NFA_PREV_ATOM_NO_WIDTH;
		    ಭಙ್ಗ;
		ಪಕ್ಷ '!':
		    // \@!
		    i = NFA_PREV_ATOM_NO_WIDTH_NEG;
		    ಭಙ್ಗ;
		ಪಕ್ಷ '<':
		    op = no_Magic(getchr());
		    ಯದಿ (op == '=')
			// \@<=
			i = NFA_PREV_ATOM_JUST_BEFORE;
		    ಅನ್ಯಥಾ ಯದಿ (op == '!')
			// \@<!
			i = NFA_PREV_ATOM_JUST_BEFORE_NEG;
		    ಭಙ್ಗ;
		ಪಕ್ಷ '>':
		    // \@>
		    i = NFA_PREV_ATOM_LIKE_PATTERN;
		    ಭಙ್ಗ;
	    }
	    ಯದಿ (i == 0)
	    {
		semsg(_(e_nfa_regexp_unknown_operator_at_chr), op);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    EMIT(i);
	    ಯದಿ (i == NFA_PREV_ATOM_JUST_BEFORE
					|| i == NFA_PREV_ATOM_JUST_BEFORE_NEG)
		EMIT(c2);
	    ಭಙ್ಗ;

	ಪಕ್ಷ Magic('?'):
	ಪಕ್ಷ Magic('='):
	    EMIT(NFA_QUEST);
	    ಭಙ್ಗ;

	ಪಕ್ಷ Magic('{'):
	    // a{2,5} will expand to 'aaa?a?a?'
	    // a{-1,3} will expand to 'aa??a??', where ?? is the nongreedy
	    // version of '?'
	    // \v(ab){2,3} will expand to '(ab)(ab)(ab)?', where all the
	    // parenthesis have the same id

	    greedy = TRUE;
	    c2 = peekchr();
	    ಯದಿ (c2 == '-' || c2 == Magic('-'))
	    {
		skipchr();
		greedy = FALSE;
	    }
	    ಯದಿ (!read_limits(&minval, &maxval))
		EMSG_RET_FAIL(_(e_nfa_regexp_error_reading_repetition_limits));

	    //  <atom>{0,inf}, <atom>{0,} and <atom>{}  are equivalent to
	    //  <atom>*
	    ಯದಿ (minval == 0 && maxval == MAX_LIMIT)
	    {
		ಯದಿ (greedy)		// { { (match the braces)
		    // \{}, \{0,}
		    EMIT(NFA_STAR);
		ಅನ್ಯಥಾ			// { { (match the braces)
		    // \{-}, \{-0,}
		    EMIT(NFA_STAR_NONGREEDY);
		ಭಙ್ಗ;
	    }

	    // Special case: x{0} or x{-0}
	    ಯದಿ (maxval == 0)
	    {
		// Ignore result of previous call to nfa_regatom()
		post_ptr = post_start + my_post_start;
		// NFA_EMPTY is 0-length and works everywhere
		EMIT(NFA_EMPTY);
		ಪ್ರತ್ಯಾಗಮ OK;
	    }

	    // The engine is very inefficient (uses too many states) when the
	    // maximum is much larger than the minimum and when the maximum is
	    // large.  However, when maxval is MAX_LIMIT, it is okay, as this
	    // will emit NFA_STAR.
	    // Bail out if we can use the other engine, but only, when the
	    // pattern does not need the NFA engine like (e.g. [[:upper:]]\{2,\}
	    // does not work with characters > 8 bit with the BT engine)
	    ಯದಿ ((nfa_re_flags & RE_AUTO)
				   && (maxval > 500 || maxval > minval + 200)
				   && (maxval != MAX_LIMIT && minval < 200)
				   && !wants_nfa)
		ಪ್ರತ್ಯಾಗಮ FAIL;

	    // Ignore previous call to nfa_regatom()
	    post_ptr = post_start + my_post_start;
	    // Save parse state after the repeated atom and the \{}
	    save_parse_state(&new_state);

	    quest = (greedy == TRUE? NFA_QUEST : NFA_QUEST_NONGREEDY);
	    ಪ್ರತಿ (i = 0; i < maxval; i++)
	    {
		// Goto beginning of the repeated atom
		restore_parse_state(&old_state);
		old_post_pos = (ಪೂರ್ಣಾಙ್ಕ)(post_ptr - post_start);
		ಯದಿ (nfa_regatom() == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		// after "minval" times, atoms are optional
		ಯದಿ (i + 1 > minval)
		{
		    ಯದಿ (maxval == MAX_LIMIT)
		    {
			ಯದಿ (greedy)
			    EMIT(NFA_STAR);
			ಅನ್ಯಥಾ
			    EMIT(NFA_STAR_NONGREEDY);
		    }
		    ಅನ್ಯಥಾ
			EMIT(quest);
		}
		ಯದಿ (old_post_pos != my_post_start)
		    EMIT(NFA_CONCAT);
		ಯದಿ (i + 1 > minval && maxval == MAX_LIMIT)
		    ಭಙ್ಗ;
	    }

	    // Go to just after the repeated atom and the \{}
	    restore_parse_state(&new_state);
	    curchr = -1;

	    ಭಙ್ಗ;


	ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
    }	// end switch

    ಯದಿ (re_multi_type(peekchr()) != NOT_MULTI)
	// Can't have a multi follow a multi.
	EMSG_RET_FAIL(_(e_nfa_regexp_cant_have_multi_follow_multi));

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
nfa_regconcat(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		cont = TRUE;
    ಪೂರ್ಣಾಙ್ಕ		first = TRUE;

    ಯಾವತ್ (cont)
    {
	ವಿಕಲ್ಪ (peekchr())
	{
	    ಪಕ್ಷ NUL:
	    ಪಕ್ಷ Magic('|'):
	    ಪಕ್ಷ Magic('&'):
	    ಪಕ್ಷ Magic(')'):
		cont = FALSE;
		ಭಙ್ಗ;

	    ಪಕ್ಷ Magic('Z'):
		regflags |= RF_ICOMBINE;
		skipchr_keepstart();
		ಭಙ್ಗ;
	    ಪಕ್ಷ Magic('c'):
		regflags |= RF_ICASE;
		skipchr_keepstart();
		ಭಙ್ಗ;
	    ಪಕ್ಷ Magic('C'):
		regflags |= RF_NOICASE;
		skipchr_keepstart();
		ಭಙ್ಗ;
	    ಪಕ್ಷ Magic('v'):
		reg_magic = MAGIC_ALL;
		skipchr_keepstart();
		curchr = -1;
		ಭಙ್ಗ;
	    ಪಕ್ಷ Magic('m'):
		reg_magic = MAGIC_ON;
		skipchr_keepstart();
		curchr = -1;
		ಭಙ್ಗ;
	    ಪಕ್ಷ Magic('M'):
		reg_magic = MAGIC_OFF;
		skipchr_keepstart();
		curchr = -1;
		ಭಙ್ಗ;
	    ಪಕ್ಷ Magic('V'):
		reg_magic = MAGIC_NONE;
		skipchr_keepstart();
		curchr = -1;
		ಭಙ್ಗ;

	    ಸ್ವಭಾವ:
		ಯದಿ (nfa_regpiece() == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		ಯದಿ (first == FALSE)
		    EMIT(NFA_CONCAT);
		ಅನ್ಯಥಾ
		    first = FALSE;
		ಭಙ್ಗ;
	}
    }

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
nfa_regbranch(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		old_post_pos;

    old_post_pos = (ಪೂರ್ಣಾಙ್ಕ)(post_ptr - post_start);

    // First branch, possibly the only one
    ಯದಿ (nfa_regconcat() == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // Try next concats
    ಯಾವತ್ (peekchr() == Magic('&'))
    {
	skipchr();
	// if concat is empty do emit a node
	ಯದಿ (old_post_pos == (ಪೂರ್ಣಾಙ್ಕ)(post_ptr - post_start))
	    EMIT(NFA_EMPTY);
	EMIT(NFA_NOPEN);
	EMIT(NFA_PREV_ATOM_NO_WIDTH);
	old_post_pos = (ಪೂರ್ಣಾಙ್ಕ)(post_ptr - post_start);
	ಯದಿ (nfa_regconcat() == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	// if concat is empty do emit a node
	ಯದಿ (old_post_pos == (ಪೂರ್ಣಾಙ್ಕ)(post_ptr - post_start))
	    EMIT(NFA_EMPTY);
	EMIT(NFA_CONCAT);
    }

    // if a branch is empty, emit one node for it
    ಯದಿ (old_post_pos == (ಪೂರ್ಣಾಙ್ಕ)(post_ptr - post_start))
	EMIT(NFA_EMPTY);

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
nfa_reg(
    ಪೂರ್ಣಾಙ್ಕ		paren)	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
{
    ಪೂರ್ಣಾಙ್ಕ		parno = 0;
    ಪೂರ್ಣಾಙ್ಕ		status = FAIL;

    ಯದಿ (paren == REG_PAREN)
    {
	ಯದಿ (regnpar >= NSUBEXP) // Too many `('
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_parens));
	parno = regnpar++;
    }
#ifdef FEAT_SYN_HL
    ಅನ್ಯಥಾ ಯದಿ (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	ಯದಿ (regnzpar >= NSUBEXP)
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_z));
	parno = regnzpar++;
    }
#endif

    ಯದಿ (nfa_reg_parse_depth >= REG_MAX_PAREN_DEPTH)
	EMSG_RET_FAIL(_(e_command_too_complex));
    ++nfa_reg_parse_depth;

    ಯದಿ (nfa_regbranch() == FAIL)
	ಗಚ್ಛ theend;	    // cascaded error

    ಯಾವತ್ (peekchr() == Magic('|'))
    {
	skipchr();
	ಯದಿ (nfa_regbranch() == FAIL)
	    ಗಚ್ಛ theend;    // cascaded error
	EMIT(NFA_OR);
    }

    // Check for proper termination.
    ಯದಿ (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
	ಯದಿ (paren == REG_NPAREN)
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	ಅನ್ಯಥಾ
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	rc_did_emsg = TRUE;
	ಗಚ್ಛ theend;
    }
    ಅನ್ಯಥಾ ಯದಿ (paren == REG_NOPAREN && peekchr() != NUL)
    {
	ಯದಿ (peekchr() == Magic(')'))
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	ಅನ್ಯಥಾ
	    emsg(_(e_nfa_regexp_proper_termination_error));
	rc_did_emsg = TRUE;
	ಗಚ್ಛ theend;
    }
    /*
     * Here we set the flag allowing back references to this set of
     * parentheses.
     */
    ಯದಿ (paren == REG_PAREN)
    {
	had_endbrace[parno] = TRUE;     // have seen the close paren
	EMIT(NFA_MOPEN + parno);
    }
#ifdef FEAT_SYN_HL
    ಅನ್ಯಥಾ ಯದಿ (paren == REG_ZPAREN)
	EMIT(NFA_ZOPEN + parno);
#endif

    status = OK;

theend:
    --nfa_reg_parse_depth;
    ಪ್ರತ್ಯಾಗಮ status;
}

#ifdef DEBUG
ಸ್ಥಿರ char_u code[50];

    ಸ್ಥಿರ ಶೂನ್ಯ
nfa_set_code(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪೂರ್ಣಾಙ್ಕ	    addnl = FALSE;

    ಯದಿ (c >= NFA_FIRST_NL && c <= NFA_LAST_NL)
    {
	addnl = TRUE;
	c -= NFA_ADD_NL;
    }

    STRCPY(code, "");
    ವಿಕಲ್ಪ (c)
    {
	ಪಕ್ಷ NFA_MATCH:	    STRCPY(code, "NFA_MATCH "); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_SPLIT:	    STRCPY(code, "NFA_SPLIT "); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CONCAT:    STRCPY(code, "NFA_CONCAT "); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_NEWL:	    STRCPY(code, "NFA_NEWL "); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_ZSTART:    STRCPY(code, "NFA_ZSTART"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_ZEND:	    STRCPY(code, "NFA_ZEND"); ಭಙ್ಗ;

	ಪಕ್ಷ NFA_BACKREF1:  STRCPY(code, "NFA_BACKREF1"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_BACKREF2:  STRCPY(code, "NFA_BACKREF2"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_BACKREF3:  STRCPY(code, "NFA_BACKREF3"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_BACKREF4:  STRCPY(code, "NFA_BACKREF4"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_BACKREF5:  STRCPY(code, "NFA_BACKREF5"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_BACKREF6:  STRCPY(code, "NFA_BACKREF6"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_BACKREF7:  STRCPY(code, "NFA_BACKREF7"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_BACKREF8:  STRCPY(code, "NFA_BACKREF8"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_BACKREF9:  STRCPY(code, "NFA_BACKREF9"); ಭಙ್ಗ;
# ifdef FEAT_SYN_HL
	ಪಕ್ಷ NFA_ZREF1:	    STRCPY(code, "NFA_ZREF1"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_ZREF2:	    STRCPY(code, "NFA_ZREF2"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_ZREF3:	    STRCPY(code, "NFA_ZREF3"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_ZREF4:	    STRCPY(code, "NFA_ZREF4"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_ZREF5:	    STRCPY(code, "NFA_ZREF5"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_ZREF6:	    STRCPY(code, "NFA_ZREF6"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_ZREF7:	    STRCPY(code, "NFA_ZREF7"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_ZREF8:	    STRCPY(code, "NFA_ZREF8"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_ZREF9:	    STRCPY(code, "NFA_ZREF9"); ಭಙ್ಗ;
# endif
	ಪಕ್ಷ NFA_SKIP:	    STRCPY(code, "NFA_SKIP"); ಭಙ್ಗ;

	ಪಕ್ಷ NFA_PREV_ATOM_NO_WIDTH:
			    STRCPY(code, "NFA_PREV_ATOM_NO_WIDTH"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_PREV_ATOM_NO_WIDTH_NEG:
			    STRCPY(code, "NFA_PREV_ATOM_NO_WIDTH_NEG"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_PREV_ATOM_JUST_BEFORE:
			    STRCPY(code, "NFA_PREV_ATOM_JUST_BEFORE"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_PREV_ATOM_JUST_BEFORE_NEG:
			 STRCPY(code, "NFA_PREV_ATOM_JUST_BEFORE_NEG"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_PREV_ATOM_LIKE_PATTERN:
			    STRCPY(code, "NFA_PREV_ATOM_LIKE_PATTERN"); ಭಙ್ಗ;

	ಪಕ್ಷ NFA_NOPEN:		    STRCPY(code, "NFA_NOPEN"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_NCLOSE:	    STRCPY(code, "NFA_NCLOSE"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_START_INVISIBLE:   STRCPY(code, "NFA_START_INVISIBLE"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_START_INVISIBLE_FIRST:
			     STRCPY(code, "NFA_START_INVISIBLE_FIRST"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_START_INVISIBLE_NEG:
			       STRCPY(code, "NFA_START_INVISIBLE_NEG"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_START_INVISIBLE_NEG_FIRST:
			 STRCPY(code, "NFA_START_INVISIBLE_NEG_FIRST"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_START_INVISIBLE_BEFORE:
			    STRCPY(code, "NFA_START_INVISIBLE_BEFORE"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_START_INVISIBLE_BEFORE_FIRST:
		      STRCPY(code, "NFA_START_INVISIBLE_BEFORE_FIRST"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_START_INVISIBLE_BEFORE_NEG:
			STRCPY(code, "NFA_START_INVISIBLE_BEFORE_NEG"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
		  STRCPY(code, "NFA_START_INVISIBLE_BEFORE_NEG_FIRST"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_START_PATTERN:   STRCPY(code, "NFA_START_PATTERN"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_END_INVISIBLE:	    STRCPY(code, "NFA_END_INVISIBLE"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_END_INVISIBLE_NEG: STRCPY(code, "NFA_END_INVISIBLE_NEG"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_END_PATTERN:	    STRCPY(code, "NFA_END_PATTERN"); ಭಙ್ಗ;

	ಪಕ್ಷ NFA_COMPOSING:	    STRCPY(code, "NFA_COMPOSING"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_END_COMPOSING:	    STRCPY(code, "NFA_END_COMPOSING"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_OPT_CHARS:	    STRCPY(code, "NFA_OPT_CHARS"); ಭಙ್ಗ;

	ಪಕ್ಷ NFA_MOPEN:
	ಪಕ್ಷ NFA_MOPEN1:
	ಪಕ್ಷ NFA_MOPEN2:
	ಪಕ್ಷ NFA_MOPEN3:
	ಪಕ್ಷ NFA_MOPEN4:
	ಪಕ್ಷ NFA_MOPEN5:
	ಪಕ್ಷ NFA_MOPEN6:
	ಪಕ್ಷ NFA_MOPEN7:
	ಪಕ್ಷ NFA_MOPEN8:
	ಪಕ್ಷ NFA_MOPEN9:
	    STRCPY(code, "NFA_MOPEN(x)");
	    code[10] = c - NFA_MOPEN + '0';
	    ಭಙ್ಗ;
	ಪಕ್ಷ NFA_MCLOSE:
	ಪಕ್ಷ NFA_MCLOSE1:
	ಪಕ್ಷ NFA_MCLOSE2:
	ಪಕ್ಷ NFA_MCLOSE3:
	ಪಕ್ಷ NFA_MCLOSE4:
	ಪಕ್ಷ NFA_MCLOSE5:
	ಪಕ್ಷ NFA_MCLOSE6:
	ಪಕ್ಷ NFA_MCLOSE7:
	ಪಕ್ಷ NFA_MCLOSE8:
	ಪಕ್ಷ NFA_MCLOSE9:
	    STRCPY(code, "NFA_MCLOSE(x)");
	    code[11] = c - NFA_MCLOSE + '0';
	    ಭಙ್ಗ;
# ifdef FEAT_SYN_HL
	ಪಕ್ಷ NFA_ZOPEN:
	ಪಕ್ಷ NFA_ZOPEN1:
	ಪಕ್ಷ NFA_ZOPEN2:
	ಪಕ್ಷ NFA_ZOPEN3:
	ಪಕ್ಷ NFA_ZOPEN4:
	ಪಕ್ಷ NFA_ZOPEN5:
	ಪಕ್ಷ NFA_ZOPEN6:
	ಪಕ್ಷ NFA_ZOPEN7:
	ಪಕ್ಷ NFA_ZOPEN8:
	ಪಕ್ಷ NFA_ZOPEN9:
	    STRCPY(code, "NFA_ZOPEN(x)");
	    code[10] = c - NFA_ZOPEN + '0';
	    ಭಙ್ಗ;
	ಪಕ್ಷ NFA_ZCLOSE:
	ಪಕ್ಷ NFA_ZCLOSE1:
	ಪಕ್ಷ NFA_ZCLOSE2:
	ಪಕ್ಷ NFA_ZCLOSE3:
	ಪಕ್ಷ NFA_ZCLOSE4:
	ಪಕ್ಷ NFA_ZCLOSE5:
	ಪಕ್ಷ NFA_ZCLOSE6:
	ಪಕ್ಷ NFA_ZCLOSE7:
	ಪಕ್ಷ NFA_ZCLOSE8:
	ಪಕ್ಷ NFA_ZCLOSE9:
	    STRCPY(code, "NFA_ZCLOSE(x)");
	    code[11] = c - NFA_ZCLOSE + '0';
	    ಭಙ್ಗ;
# endif
	ಪಕ್ಷ NFA_EOL:		STRCPY(code, "NFA_EOL "); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_BOL:		STRCPY(code, "NFA_BOL "); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_EOW:		STRCPY(code, "NFA_EOW "); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_BOW:		STRCPY(code, "NFA_BOW "); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_EOF:		STRCPY(code, "NFA_EOF "); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_BOF:		STRCPY(code, "NFA_BOF "); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_LNUM:		STRCPY(code, "NFA_LNUM "); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_LNUM_GT:	STRCPY(code, "NFA_LNUM_GT "); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_LNUM_LT:	STRCPY(code, "NFA_LNUM_LT "); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_COL:		STRCPY(code, "NFA_COL "); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_COL_GT:	STRCPY(code, "NFA_COL_GT "); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_COL_LT:	STRCPY(code, "NFA_COL_LT "); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_VCOL:		STRCPY(code, "NFA_VCOL "); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_VCOL_GT:	STRCPY(code, "NFA_VCOL_GT "); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_VCOL_LT:	STRCPY(code, "NFA_VCOL_LT "); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_MARK:		STRCPY(code, "NFA_MARK "); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_MARK_GT:	STRCPY(code, "NFA_MARK_GT "); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_MARK_LT:	STRCPY(code, "NFA_MARK_LT "); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CURSOR:	STRCPY(code, "NFA_CURSOR "); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_VISUAL:	STRCPY(code, "NFA_VISUAL "); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_ANY_COMPOSING:	STRCPY(code, "NFA_ANY_COMPOSING "); ಭಙ್ಗ;

	ಪಕ್ಷ NFA_STAR:		STRCPY(code, "NFA_STAR "); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_STAR_NONGREEDY: STRCPY(code, "NFA_STAR_NONGREEDY "); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_QUEST:		STRCPY(code, "NFA_QUEST"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_QUEST_NONGREEDY: STRCPY(code, "NFA_QUEST_NON_GREEDY"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_EMPTY:		STRCPY(code, "NFA_EMPTY"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_OR:		STRCPY(code, "NFA_OR"); ಭಙ್ಗ;

	ಪಕ್ಷ NFA_START_COLL:	STRCPY(code, "NFA_START_COLL"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_END_COLL:	STRCPY(code, "NFA_END_COLL"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_START_NEG_COLL: STRCPY(code, "NFA_START_NEG_COLL"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_END_NEG_COLL:	STRCPY(code, "NFA_END_NEG_COLL"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_RANGE:		STRCPY(code, "NFA_RANGE"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_RANGE_MIN:	STRCPY(code, "NFA_RANGE_MIN"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_RANGE_MAX:	STRCPY(code, "NFA_RANGE_MAX"); ಭಙ್ಗ;

	ಪಕ್ಷ NFA_CLASS_ALNUM:	STRCPY(code, "NFA_CLASS_ALNUM"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_ALPHA:	STRCPY(code, "NFA_CLASS_ALPHA"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_BLANK:	STRCPY(code, "NFA_CLASS_BLANK"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_CNTRL:	STRCPY(code, "NFA_CLASS_CNTRL"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_DIGIT:	STRCPY(code, "NFA_CLASS_DIGIT"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_GRAPH:	STRCPY(code, "NFA_CLASS_GRAPH"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_LOWER:	STRCPY(code, "NFA_CLASS_LOWER"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_PRINT:	STRCPY(code, "NFA_CLASS_PRINT"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_PUNCT:	STRCPY(code, "NFA_CLASS_PUNCT"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_SPACE:	STRCPY(code, "NFA_CLASS_SPACE"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_UPPER:	STRCPY(code, "NFA_CLASS_UPPER"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_XDIGIT:	STRCPY(code, "NFA_CLASS_XDIGIT"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_TAB:	STRCPY(code, "NFA_CLASS_TAB"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_RETURN:	STRCPY(code, "NFA_CLASS_RETURN"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_BACKSPACE:   STRCPY(code, "NFA_CLASS_BACKSPACE"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_ESCAPE:	STRCPY(code, "NFA_CLASS_ESCAPE"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_IDENT:	STRCPY(code, "NFA_CLASS_IDENT"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_KEYWORD:	STRCPY(code, "NFA_CLASS_KEYWORD"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_FNAME:	STRCPY(code, "NFA_CLASS_FNAME"); ಭಙ್ಗ;

	ಪಕ್ಷ NFA_ANY:	STRCPY(code, "NFA_ANY"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_IDENT:	STRCPY(code, "NFA_IDENT"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_SIDENT:STRCPY(code, "NFA_SIDENT"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_KWORD:	STRCPY(code, "NFA_KWORD"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_SKWORD:STRCPY(code, "NFA_SKWORD"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_FNAME:	STRCPY(code, "NFA_FNAME"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_SFNAME:STRCPY(code, "NFA_SFNAME"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_PRINT:	STRCPY(code, "NFA_PRINT"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_SPRINT:STRCPY(code, "NFA_SPRINT"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_WHITE:	STRCPY(code, "NFA_WHITE"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_NWHITE:STRCPY(code, "NFA_NWHITE"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_DIGIT:	STRCPY(code, "NFA_DIGIT"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_NDIGIT:STRCPY(code, "NFA_NDIGIT"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_HEX:	STRCPY(code, "NFA_HEX"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_NHEX:	STRCPY(code, "NFA_NHEX"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_OCTAL:	STRCPY(code, "NFA_OCTAL"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_NOCTAL:STRCPY(code, "NFA_NOCTAL"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_WORD:	STRCPY(code, "NFA_WORD"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_NWORD:	STRCPY(code, "NFA_NWORD"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_HEAD:	STRCPY(code, "NFA_HEAD"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_NHEAD:	STRCPY(code, "NFA_NHEAD"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_ALPHA:	STRCPY(code, "NFA_ALPHA"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_NALPHA:STRCPY(code, "NFA_NALPHA"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_LOWER:	STRCPY(code, "NFA_LOWER"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_NLOWER:STRCPY(code, "NFA_NLOWER"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_UPPER:	STRCPY(code, "NFA_UPPER"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_NUPPER:STRCPY(code, "NFA_NUPPER"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_LOWER_IC:  STRCPY(code, "NFA_LOWER_IC"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_NLOWER_IC: STRCPY(code, "NFA_NLOWER_IC"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_UPPER_IC:  STRCPY(code, "NFA_UPPER_IC"); ಭಙ್ಗ;
	ಪಕ್ಷ NFA_NUPPER_IC: STRCPY(code, "NFA_NUPPER_IC"); ಭಙ್ಗ;

	ಸ್ವಭಾವ:
	    STRCPY(code, "CHAR(x)");
	    code[5] = c;
    }

    ಯದಿ (addnl == TRUE)
	STRCAT(code, " + NEWLINE ");

}

# ifdef ENABLE_LOG
ಸ್ಥಿರ FILE *log_fd;
ಸ್ಥಿರ char_u e_log_open_failed[] = N_("Could not open temporary log file for writing, displaying on stderr... ");

/*
 * Print the postfix notation of the current regexp.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
nfa_postfix_dump(char_u *ಅಭಿವ್ಯಕ್ತಿ, ಪೂರ್ಣಾಙ್ಕ retval)
{
    ಪೂರ್ಣಾಙ್ಕ *p;
    FILE *f;

    f = fopen(NFA_REGEXP_DUMP_LOG, "a");
    ಯದಿ (f == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    fprintf(f, "\n-------------------------\n");
    ಯದಿ (retval == FAIL)
	fprintf(f, ">>> NFA engine failed... \n");
    ಅನ್ಯಥಾ ಯದಿ (retval == OK)
	fprintf(f, ">>> NFA engine succeeded !\n");
    fprintf(f, "Regexp: \"%s\"\nPostfix notation (char): \"", ಅಭಿವ್ಯಕ್ತಿ);
    ಪ್ರತಿ (p = post_start; *p && p < post_ptr; p++)
    {
	nfa_set_code(*p);
	fprintf(f, "%s, ", code);
    }
    fprintf(f, "\"\nPostfix notation (int): ");
    ಪ್ರತಿ (p = post_start; *p && p < post_ptr; p++)
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

    ಯದಿ (state == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    fprintf(debugf, "(%2d)", abs(state->id));

    // Output indent
    p = (char_u *)indent->ga_data;
    ಯದಿ (indent->ga_len >= 3)
    {
	ಪೂರ್ಣಾಙ್ಕ	last = indent->ga_len - 3;
	char_u	save[2];

	STRNCPY(save, &p[last], 2);
	memcpy(&p[last], "+-", 2);
	fprintf(debugf, " %s", p);
	STRNCPY(&p[last], save, 2);
    }
    ಅನ್ಯಥಾ
	fprintf(debugf, " %s", p);

    nfa_set_code(state->c);
    fprintf(debugf, "%s (%d) (id=%d) val=%d\n",
		 code,
		 state->c,
		 abs(state->id),
		 state->val);
    ಯದಿ (state->id < 0)
	ಪ್ರತ್ಯಾಗಮ;

    state->id = abs(state->id) * -1;

    // grow indent for state->out
    indent->ga_len -= 1;
    ಯದಿ (state->out1)
	GA_CONCAT_LITERAL(indent, "| ");
    ಅನ್ಯಥಾ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
nfa_dump(nfa_regprog_T *prog)
{
    FILE *debugf = fopen(NFA_REGEXP_DUMP_LOG, "a");

    ಯದಿ (debugf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    nfa_print_state(debugf, prog->start);

    ಯದಿ (prog->reganch)
	fprintf(debugf, "reganch: %d\n", prog->reganch);
    ಯದಿ (prog->regstart != NUL)
	fprintf(debugf, "regstart: %c (decimal: %d)\n",
		prog->regstart, prog->regstart);
    ಯದಿ (prog->match_text != ರಿಕ್ತ)
	fprintf(debugf, "match_text: \"%s\"\n", prog->match_text);

    fclose(debugf);
}
# endif	    // ENABLE_LOG
#endif	    // DEBUG

/*
 * Parse r.e. @expr and convert it into postfix form.
 * Return the postfix string on success, NULL otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ *
re2post(ಶೂನ್ಯ)
{
    ಯದಿ (nfa_reg(REG_NOPAREN) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    EMIT(NFA_MOPEN);
    ಪ್ರತ್ಯಾಗಮ post_start;
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
alloc_state(ಪೂರ್ಣಾಙ್ಕ c, nfa_state_T *out, nfa_state_T *out1)
{
    nfa_state_T *s;

    ಯದಿ (istate >= nstate)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    s = &state_ptr[istate++];

    s->c    = c;
    s->out  = out;
    s->out1 = out1;
    s->val  = 0;

    s->id   = istate;
    s->lastlist[0] = 0;
    s->lastlist[1] = 0;

    ಪ್ರತ್ಯಾಗಮ s;
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
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಙ್ಘ Ptrlist Ptrlist;
ಸಙ್ಘ Ptrlist
{
    Ptrlist	*next;
    nfa_state_T	*s;
};

ಸಂರಚನಾ Frag
{
    nfa_state_T *start;
    Ptrlist	*out;
};
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ Frag Frag_T;

// Reused across compilations by post2nfa(); freed in free_regexp_stuff().
ಸ್ಥಿರ Frag_T *nfa_ರಾಶಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ nfa_ರಾಶಿ_len;

/*
 * Initialize a Frag_T struct and return it.
 */
    ಸ್ಥಿರ Frag_T
frag(nfa_state_T *start, Ptrlist *out)
{
    Frag_T n;

    n.start = start;
    n.out = out;
    ಪ್ರತ್ಯಾಗಮ n;
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
    l->next = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ l;
}

/*
 * Patch the list of states at out to point to start.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
patch(Ptrlist *l, nfa_state_T *s)
{
    Ptrlist *next;

    ಪ್ರತಿ (; l; l = next)
    {
	next = l->next;
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
    ಯಾವತ್ (l1->next)
	l1 = l1->next;
    l1->next = l2;
    ಪ್ರತ್ಯಾಗಮ oldl1;
}

/*
 * Stack used for transforming postfix form into NFA.
 */
ಸ್ಥಿರ Frag_T empty;

    ಸ್ಥಿರ ಶೂನ್ಯ
st_error(ಪೂರ್ಣಾಙ್ಕ *postfix UNUSED, ಪೂರ್ಣಾಙ್ಕ *end UNUSED, ಪೂರ್ಣಾಙ್ಕ *p UNUSED)
{
#ifdef NFA_REGEXP_ERROR_LOG
    FILE *df;
    ಪೂರ್ಣಾಙ್ಕ *p2;

    df = fopen(NFA_REGEXP_ERROR_LOG, "a");
    ಯದಿ (df)
    {
	fprintf(df, "Error popping the stack!\n");
# ifdef DEBUG
	fprintf(df, "Current regexp is \"%s\"\n", nfa_regengine.ಅಭಿವ್ಯಕ್ತಿ);
# endif
	fprintf(df, "Postfix form is: ");
# ifdef DEBUG
	ಪ್ರತಿ (p2 = postfix; p2 < end; p2++)
	{
	    nfa_set_code(*p2);
	    fprintf(df, "%s, ", code);
	}
	nfa_set_code(*p);
	fprintf(df, "\nCurrent position is: ");
	ಪ್ರತಿ (p2 = postfix; p2 <= p; p2 ++)
	{
	    nfa_set_code(*p2);
	    fprintf(df, "%s, ", code);
	}
# ಅನ್ಯಥಾ
	ಪ್ರತಿ (p2 = postfix; p2 < end; p2++)
	    fprintf(df, "%d, ", *p2);
	fprintf(df, "\nCurrent position is: ");
	ಪ್ರತಿ (p2 = postfix; p2 <= p; p2 ++)
	    fprintf(df, "%d, ", *p2);
# endif
	fprintf(df, "\n--------------------------\n");
	fclose(df);
    }
#endif
    emsg(_(e_nfa_regexp_could_not_ತೆಗೆ_ರಾಶಿ));
}

/*
 * Push an item onto the stack.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
st_ತಳ್ಳು(Frag_T s, Frag_T **p, Frag_T *ರಾಶಿ_end)
{
    Frag_T *stackp = *p;

    ಯದಿ (stackp >= ರಾಶಿ_end)
	ಪ್ರತ್ಯಾಗಮ;
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
    ಯದಿ (stackp < ರಾಶಿ)
	ಪ್ರತ್ಯಾಗಮ empty;
    ಪ್ರತ್ಯಾಗಮ **p;
}

/*
 * Estimate the maximum byte length of anything matching "state".
 * When unknown or unlimited return -1.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
nfa_max_width(nfa_state_T *startstate, ಪೂರ್ಣಾಙ್ಕ depth)
{
    ಪೂರ್ಣಾಙ್ಕ		    l, r;
    nfa_state_T	    *state = startstate;
    ಪೂರ್ಣಾಙ್ಕ		    len = 0;

    // detect looping in a NFA_SPLIT
    ಯದಿ (depth > 4)
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯಾವತ್ (state != ರಿಕ್ತ)
    {
	ವಿಕಲ್ಪ (state->c)
	{
	    ಪಕ್ಷ NFA_END_INVISIBLE:
	    ಪಕ್ಷ NFA_END_INVISIBLE_NEG:
		// the end, return what we have
		ಪ್ರತ್ಯಾಗಮ len;

	    ಪಕ್ಷ NFA_SPLIT:
		// two alternatives, use the maximum
		l = nfa_max_width(state->out, depth + 1);
		r = nfa_max_width(state->out1, depth + 1);
		ಯದಿ (l < 0 || r < 0)
		    ಪ್ರತ್ಯಾಗಮ -1;
		ಪ್ರತ್ಯಾಗಮ len + (l > r ? l : r);

	    ಪಕ್ಷ NFA_ANY:
	    ಪಕ್ಷ NFA_START_COLL:
	    ಪಕ್ಷ NFA_START_NEG_COLL:
		// matches some character, including composing chars
		ಯದಿ (enc_utf8)
		    len += MB_MAXBYTES;
		ಅನ್ಯಥಾ ಯದಿ (has_mbyte)
		    len += 2;
		ಅನ್ಯಥಾ
		    ++len;
		ಯದಿ (state->c != NFA_ANY)
		{
		    // Skip over the compiled collection.
		    // malformed NFAs must not crash width estimation.
		    ಯದಿ (state->out1 == ರಿಕ್ತ || state->out1->out == ರಿಕ್ತ)
			ಪ್ರತ್ಯಾಗಮ -1;
		    state = state->out1->out;
		    ಅನುವರ್ತ;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_DIGIT:
	    ಪಕ್ಷ NFA_WHITE:
	    ಪಕ್ಷ NFA_HEX:
	    ಪಕ್ಷ NFA_OCTAL:
		// ascii
		++len;
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_IDENT:
	    ಪಕ್ಷ NFA_SIDENT:
	    ಪಕ್ಷ NFA_KWORD:
	    ಪಕ್ಷ NFA_SKWORD:
	    ಪಕ್ಷ NFA_FNAME:
	    ಪಕ್ಷ NFA_SFNAME:
	    ಪಕ್ಷ NFA_PRINT:
	    ಪಕ್ಷ NFA_SPRINT:
	    ಪಕ್ಷ NFA_NWHITE:
	    ಪಕ್ಷ NFA_NDIGIT:
	    ಪಕ್ಷ NFA_NHEX:
	    ಪಕ್ಷ NFA_NOCTAL:
	    ಪಕ್ಷ NFA_WORD:
	    ಪಕ್ಷ NFA_NWORD:
	    ಪಕ್ಷ NFA_HEAD:
	    ಪಕ್ಷ NFA_NHEAD:
	    ಪಕ್ಷ NFA_ALPHA:
	    ಪಕ್ಷ NFA_NALPHA:
	    ಪಕ್ಷ NFA_LOWER:
	    ಪಕ್ಷ NFA_NLOWER:
	    ಪಕ್ಷ NFA_UPPER:
	    ಪಕ್ಷ NFA_NUPPER:
	    ಪಕ್ಷ NFA_LOWER_IC:
	    ಪಕ್ಷ NFA_NLOWER_IC:
	    ಪಕ್ಷ NFA_UPPER_IC:
	    ಪಕ್ಷ NFA_NUPPER_IC:
	    ಪಕ್ಷ NFA_ANY_COMPOSING:
		// possibly non-ascii
		ಯದಿ (has_mbyte)
		    len += 3;
		ಅನ್ಯಥಾ
		    ++len;
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_START_INVISIBLE:
	    ಪಕ್ಷ NFA_START_INVISIBLE_NEG:
	    ಪಕ್ಷ NFA_START_INVISIBLE_BEFORE:
	    ಪಕ್ಷ NFA_START_INVISIBLE_BEFORE_NEG:
		// zero-width, out1 points to the END state
		state = state->out1->out;
		ಅನುವರ್ತ;

	    ಪಕ್ಷ NFA_BACKREF1:
	    ಪಕ್ಷ NFA_BACKREF2:
	    ಪಕ್ಷ NFA_BACKREF3:
	    ಪಕ್ಷ NFA_BACKREF4:
	    ಪಕ್ಷ NFA_BACKREF5:
	    ಪಕ್ಷ NFA_BACKREF6:
	    ಪಕ್ಷ NFA_BACKREF7:
	    ಪಕ್ಷ NFA_BACKREF8:
	    ಪಕ್ಷ NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	    ಪಕ್ಷ NFA_ZREF1:
	    ಪಕ್ಷ NFA_ZREF2:
	    ಪಕ್ಷ NFA_ZREF3:
	    ಪಕ್ಷ NFA_ZREF4:
	    ಪಕ್ಷ NFA_ZREF5:
	    ಪಕ್ಷ NFA_ZREF6:
	    ಪಕ್ಷ NFA_ZREF7:
	    ಪಕ್ಷ NFA_ZREF8:
	    ಪಕ್ಷ NFA_ZREF9:
#endif
	    ಪಕ್ಷ NFA_NEWL:
	    ಪಕ್ಷ NFA_SKIP:
		// unknown width
		ಪ್ರತ್ಯಾಗಮ -1;

	    ಪಕ್ಷ NFA_BOL:
	    ಪಕ್ಷ NFA_EOL:
	    ಪಕ್ಷ NFA_BOF:
	    ಪಕ್ಷ NFA_EOF:
	    ಪಕ್ಷ NFA_BOW:
	    ಪಕ್ಷ NFA_EOW:
	    ಪಕ್ಷ NFA_MOPEN:
	    ಪಕ್ಷ NFA_MOPEN1:
	    ಪಕ್ಷ NFA_MOPEN2:
	    ಪಕ್ಷ NFA_MOPEN3:
	    ಪಕ್ಷ NFA_MOPEN4:
	    ಪಕ್ಷ NFA_MOPEN5:
	    ಪಕ್ಷ NFA_MOPEN6:
	    ಪಕ್ಷ NFA_MOPEN7:
	    ಪಕ್ಷ NFA_MOPEN8:
	    ಪಕ್ಷ NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	    ಪಕ್ಷ NFA_ZOPEN:
	    ಪಕ್ಷ NFA_ZOPEN1:
	    ಪಕ್ಷ NFA_ZOPEN2:
	    ಪಕ್ಷ NFA_ZOPEN3:
	    ಪಕ್ಷ NFA_ZOPEN4:
	    ಪಕ್ಷ NFA_ZOPEN5:
	    ಪಕ್ಷ NFA_ZOPEN6:
	    ಪಕ್ಷ NFA_ZOPEN7:
	    ಪಕ್ಷ NFA_ZOPEN8:
	    ಪಕ್ಷ NFA_ZOPEN9:
	    ಪಕ್ಷ NFA_ZCLOSE:
	    ಪಕ್ಷ NFA_ZCLOSE1:
	    ಪಕ್ಷ NFA_ZCLOSE2:
	    ಪಕ್ಷ NFA_ZCLOSE3:
	    ಪಕ್ಷ NFA_ZCLOSE4:
	    ಪಕ್ಷ NFA_ZCLOSE5:
	    ಪಕ್ಷ NFA_ZCLOSE6:
	    ಪಕ್ಷ NFA_ZCLOSE7:
	    ಪಕ್ಷ NFA_ZCLOSE8:
	    ಪಕ್ಷ NFA_ZCLOSE9:
#endif
	    ಪಕ್ಷ NFA_MCLOSE:
	    ಪಕ್ಷ NFA_MCLOSE1:
	    ಪಕ್ಷ NFA_MCLOSE2:
	    ಪಕ್ಷ NFA_MCLOSE3:
	    ಪಕ್ಷ NFA_MCLOSE4:
	    ಪಕ್ಷ NFA_MCLOSE5:
	    ಪಕ್ಷ NFA_MCLOSE6:
	    ಪಕ್ಷ NFA_MCLOSE7:
	    ಪಕ್ಷ NFA_MCLOSE8:
	    ಪಕ್ಷ NFA_MCLOSE9:
	    ಪಕ್ಷ NFA_NOPEN:
	    ಪಕ್ಷ NFA_NCLOSE:

	    ಪಕ್ಷ NFA_LNUM_GT:
	    ಪಕ್ಷ NFA_LNUM_LT:
	    ಪಕ್ಷ NFA_COL_GT:
	    ಪಕ್ಷ NFA_COL_LT:
	    ಪಕ್ಷ NFA_VCOL_GT:
	    ಪಕ್ಷ NFA_VCOL_LT:
	    ಪಕ್ಷ NFA_MARK_GT:
	    ಪಕ್ಷ NFA_MARK_LT:
	    ಪಕ್ಷ NFA_VISUAL:
	    ಪಕ್ಷ NFA_LNUM:
	    ಪಕ್ಷ NFA_CURSOR:
	    ಪಕ್ಷ NFA_COL:
	    ಪಕ್ಷ NFA_VCOL:
	    ಪಕ್ಷ NFA_MARK:

	    ಪಕ್ಷ NFA_ZSTART:
	    ಪಕ್ಷ NFA_ZEND:
	    ಪಕ್ಷ NFA_OPT_CHARS:
	    ಪಕ್ಷ NFA_EMPTY:
	    ಪಕ್ಷ NFA_START_PATTERN:
	    ಪಕ್ಷ NFA_END_PATTERN:
	    ಪಕ್ಷ NFA_COMPOSING:
	    ಪಕ್ಷ NFA_END_COMPOSING:
		// zero-width
		ಭಙ್ಗ;

	    ಸ್ವಭಾವ:
		ಯದಿ (state->c < 0)
		    // don't know what this is
		    ಪ್ರತ್ಯಾಗಮ -1;
		// normal character
		len += MB_CHAR2LEN(state->c);
		ಭಙ್ಗ;
	}

	// normal way to continue
	state = state->out;
    }

    // unrecognized, "cannot happen"
    ಪ್ರತ್ಯಾಗಮ -1;
}

/*
 * Convert a postfix form into its equivalent NFA.
 * Return the NFA start state on success, NULL otherwise.
 */
    ಸ್ಥಿರ nfa_state_T *
post2nfa(ಪೂರ್ಣಾಙ್ಕ *postfix, ಪೂರ್ಣಾಙ್ಕ *end, ಪೂರ್ಣಾಙ್ಕ nfa_calc_size)
{
    ಪೂರ್ಣಾಙ್ಕ		*p;
    ಪೂರ್ಣಾಙ್ಕ		mopen;
    ಪೂರ್ಣಾಙ್ಕ		mclose;
    Frag_T	*ರಾಶಿ = ರಿಕ್ತ;
    Frag_T	*stackp = ರಿಕ್ತ;
    Frag_T	*ರಾಶಿ_end = ರಿಕ್ತ;
    Frag_T	e1;
    Frag_T	e2;
    Frag_T	e;
    nfa_state_T	*s;
    nfa_state_T	*s1;
    nfa_state_T	*matchstate;
    nfa_state_T	*ret = ರಿಕ್ತ;

    ಯದಿ (postfix == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

#define ತಳ್ಳು_2(s)	    st_ತಳ್ಳು((s), &stackp, ರಾಶಿ_end)
#define ತೆಗೆ_2()	    st_ತೆಗೆ(&stackp, ರಾಶಿ);		\
		    ಯದಿ (stackp < ರಾಶಿ)			\
		    {					\
			st_error(postfix, end, p);	\
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;			\
		    }

    ಯದಿ (nfa_calc_size == FALSE)
    {
	// Reuse the fragment stack across compilations, growing when needed;
	// it is freed in free_regexp_stuff().
	ಯದಿ (nfa_ರಾಶಿ == ರಿಕ್ತ || nfa_ರಾಶಿ_len < nstate + 1)
	{
	    vim_free(nfa_ರಾಶಿ);
	    nfa_ರಾಶಿ = ALLOC_MULT(Frag_T, nstate + 1);
	    ಯದಿ (nfa_ರಾಶಿ == ರಿಕ್ತ)
	    {
		nfa_ರಾಶಿ_len = 0;
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	    nfa_ರಾಶಿ_len = nstate + 1;
	}
	ರಾಶಿ = nfa_ರಾಶಿ;
	stackp = ರಾಶಿ;
	ರಾಶಿ_end = ರಾಶಿ + (nstate + 1);
    }

    ಪ್ರತಿ (p = postfix; p < end; ++p)
    {
	ವಿಕಲ್ಪ (*p)
	{
	ಪಕ್ಷ NFA_CONCAT:
	    // Concatenation.
	    // Pay attention: this operator does not exist in the r.e. itself
	    // (it is implicit, really).  It is added when r.e. is translated
	    // to postfix form in re2post().
	    ಯದಿ (nfa_calc_size == TRUE)
	    {
		// nstate += 0;
		ಭಙ್ಗ;
	    }
	    e2 = ತೆಗೆ_2();
	    e1 = ತೆಗೆ_2();
	    patch(e1.out, e2.start);
	    ತಳ್ಳು_2(frag(e1.start, e2.out));
	    ಭಙ್ಗ;

	ಪಕ್ಷ NFA_OR:
	    // Alternation
	    ಯದಿ (nfa_calc_size == TRUE)
	    {
		nstate++;
		ಭಙ್ಗ;
	    }
	    e2 = ತೆಗೆ_2();
	    e1 = ತೆಗೆ_2();
	    s = alloc_state(NFA_SPLIT, e1.start, e2.start);
	    ಯದಿ (s == ರಿಕ್ತ)
		ಗಚ್ಛ theend;
	    ತಳ್ಳು_2(frag(s, append(e1.out, e2.out)));
	    ಭಙ್ಗ;

	ಪಕ್ಷ NFA_STAR:
	    // Zero or more, prefer more
	    ಯದಿ (nfa_calc_size == TRUE)
	    {
		nstate++;
		ಭಙ್ಗ;
	    }
	    e = ತೆಗೆ_2();
	    s = alloc_state(NFA_SPLIT, e.start, ರಿಕ್ತ);
	    ಯದಿ (s == ರಿಕ್ತ)
		ಗಚ್ಛ theend;
	    patch(e.out, s);
	    ತಳ್ಳು_2(frag(s, list1(&s->out1)));
	    ಭಙ್ಗ;

	ಪಕ್ಷ NFA_STAR_NONGREEDY:
	    // Zero or more, prefer zero
	    ಯದಿ (nfa_calc_size == TRUE)
	    {
		nstate++;
		ಭಙ್ಗ;
	    }
	    e = ತೆಗೆ_2();
	    s = alloc_state(NFA_SPLIT, ರಿಕ್ತ, e.start);
	    ಯದಿ (s == ರಿಕ್ತ)
		ಗಚ್ಛ theend;
	    patch(e.out, s);
	    ತಳ್ಳು_2(frag(s, list1(&s->out)));
	    ಭಙ್ಗ;

	ಪಕ್ಷ NFA_QUEST:
	    // one or zero atoms=> greedy match
	    ಯದಿ (nfa_calc_size == TRUE)
	    {
		nstate++;
		ಭಙ್ಗ;
	    }
	    e = ತೆಗೆ_2();
	    s = alloc_state(NFA_SPLIT, e.start, ರಿಕ್ತ);
	    ಯದಿ (s == ರಿಕ್ತ)
		ಗಚ್ಛ theend;
	    ತಳ್ಳು_2(frag(s, append(e.out, list1(&s->out1))));
	    ಭಙ್ಗ;

	ಪಕ್ಷ NFA_QUEST_NONGREEDY:
	    // zero or one atoms => non-greedy match
	    ಯದಿ (nfa_calc_size == TRUE)
	    {
		nstate++;
		ಭಙ್ಗ;
	    }
	    e = ತೆಗೆ_2();
	    s = alloc_state(NFA_SPLIT, ರಿಕ್ತ, e.start);
	    ಯದಿ (s == ರಿಕ್ತ)
		ಗಚ್ಛ theend;
	    ತಳ್ಳು_2(frag(s, append(e.out, list1(&s->out))));
	    ಭಙ್ಗ;

	ಪಕ್ಷ NFA_END_COLL:
	ಪಕ್ಷ NFA_END_NEG_COLL:
	    // On the stack is the sequence starting with NFA_START_COLL or
	    // NFA_START_NEG_COLL and all possible characters. Patch it to
	    // add the output to the start.
	    ಯದಿ (nfa_calc_size == TRUE)
	    {
		nstate++;
		ಭಙ್ಗ;
	    }
	    e = ತೆಗೆ_2();
	    s = alloc_state(NFA_END_COLL, ರಿಕ್ತ, ರಿಕ್ತ);
	    ಯದಿ (s == ರಿಕ್ತ)
		ಗಚ್ಛ theend;
	    patch(e.out, s);
	    e.start->out1 = s;
	    ತಳ್ಳು_2(frag(e.start, list1(&s->out)));
	    ಭಙ್ಗ;

	ಪಕ್ಷ NFA_RANGE:
	    // Before this are two characters, the low and high end of a
	    // range.  Turn them into two states with MIN and MAX.
	    ಯದಿ (nfa_calc_size == TRUE)
	    {
		// nstate += 0;
		ಭಙ್ಗ;
	    }
	    e2 = ತೆಗೆ_2();
	    e1 = ತೆಗೆ_2();
	    e2.start->val = e2.start->c;
	    e2.start->c = NFA_RANGE_MAX;
	    e1.start->val = e1.start->c;
	    e1.start->c = NFA_RANGE_MIN;
	    patch(e1.out, e2.start);
	    ತಳ್ಳು_2(frag(e1.start, e2.out));
	    ಭಙ್ಗ;

	ಪಕ್ಷ NFA_EMPTY:
	    // 0-length, used in a repetition with max/min count of 0
	    ಯದಿ (nfa_calc_size == TRUE)
	    {
		nstate++;
		ಭಙ್ಗ;
	    }
	    s = alloc_state(NFA_EMPTY, ರಿಕ್ತ, ರಿಕ್ತ);
	    ಯದಿ (s == ರಿಕ್ತ)
		ಗಚ್ಛ theend;
	    ತಳ್ಳು_2(frag(s, list1(&s->out)));
	    ಭಙ್ಗ;

	ಪಕ್ಷ NFA_OPT_CHARS:
	  {
	    ಪೂರ್ಣಾಙ್ಕ    n;

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
	    ಯದಿ (nfa_calc_size == TRUE)
	    {
		nstate += n;
		ಭಙ್ಗ;
	    }
	    s = ರಿಕ್ತ; // avoid compiler warning
	    e1.out = ರಿಕ್ತ; // stores list with out1's
	    s1 = ರಿಕ್ತ; // previous NFA_SPLIT to connect to
	    ಯಾವತ್ (n-- > 0)
	    {
		e = ತೆಗೆ_2(); // get character
		s = alloc_state(NFA_SPLIT, e.start, ರಿಕ್ತ);
		ಯದಿ (s == ರಿಕ್ತ)
		    ಗಚ್ಛ theend;
		ಯದಿ (e1.out == ರಿಕ್ತ)
		    e1 = e;
		patch(e.out, s1);
		append(e1.out, list1(&s->out1));
		s1 = s;
	    }
	    ತಳ್ಳು_2(frag(s, e1.out));
	    ಭಙ್ಗ;
	  }

	ಪಕ್ಷ NFA_PREV_ATOM_NO_WIDTH:
	ಪಕ್ಷ NFA_PREV_ATOM_NO_WIDTH_NEG:
	ಪಕ್ಷ NFA_PREV_ATOM_JUST_BEFORE:
	ಪಕ್ಷ NFA_PREV_ATOM_JUST_BEFORE_NEG:
	ಪಕ್ಷ NFA_PREV_ATOM_LIKE_PATTERN:
	  {
	    ಪೂರ್ಣಾಙ್ಕ before = (*p == NFA_PREV_ATOM_JUST_BEFORE
				      || *p == NFA_PREV_ATOM_JUST_BEFORE_NEG);
	    ಪೂರ್ಣಾಙ್ಕ pattern = (*p == NFA_PREV_ATOM_LIKE_PATTERN);
	    ಪೂರ್ಣಾಙ್ಕ start_state;
	    ಪೂರ್ಣಾಙ್ಕ end_state;
	    ಪೂರ್ಣಾಙ್ಕ n = 0;
	    nfa_state_T *zend;
	    nfa_state_T *skip;

	    ವಿಕಲ್ಪ (*p)
	    {
		ಪಕ್ಷ NFA_PREV_ATOM_NO_WIDTH:
		    start_state = NFA_START_INVISIBLE;
		    end_state = NFA_END_INVISIBLE;
		    ಭಙ್ಗ;
		ಪಕ್ಷ NFA_PREV_ATOM_NO_WIDTH_NEG:
		    start_state = NFA_START_INVISIBLE_NEG;
		    end_state = NFA_END_INVISIBLE_NEG;
		    ಭಙ್ಗ;
		ಪಕ್ಷ NFA_PREV_ATOM_JUST_BEFORE:
		    start_state = NFA_START_INVISIBLE_BEFORE;
		    end_state = NFA_END_INVISIBLE;
		    ಭಙ್ಗ;
		ಪಕ್ಷ NFA_PREV_ATOM_JUST_BEFORE_NEG:
		    start_state = NFA_START_INVISIBLE_BEFORE_NEG;
		    end_state = NFA_END_INVISIBLE_NEG;
		    ಭಙ್ಗ;
		ಸ್ವಭಾವ: // NFA_PREV_ATOM_LIKE_PATTERN:
		    start_state = NFA_START_PATTERN;
		    end_state = NFA_END_PATTERN;
		    ಭಙ್ಗ;
	    }

	    ಯದಿ (before)
		n = *++p; // get the count

	    // The \@= operator: match the preceding atom with zero width.
	    // The \@! operator: no match for the preceding atom.
	    // The \@<= operator: match for the preceding atom.
	    // The \@<! operator: no match for the preceding atom.
	    // Surrounds the preceding atom with START_INVISIBLE and
	    // END_INVISIBLE, similarly to MOPEN.

	    ಯದಿ (nfa_calc_size == TRUE)
	    {
		nstate += pattern ? 4 : 2;
		ಭಙ್ಗ;
	    }
	    e = ತೆಗೆ_2();
	    s1 = alloc_state(end_state, ರಿಕ್ತ, ರಿಕ್ತ);
	    ಯದಿ (s1 == ರಿಕ್ತ)
		ಗಚ್ಛ theend;

	    s = alloc_state(start_state, e.start, s1);
	    ಯದಿ (s == ರಿಕ್ತ)
		ಗಚ್ಛ theend;
	    ಯದಿ (pattern)
	    {
		// NFA_ZEND -> NFA_END_PATTERN -> NFA_SKIP -> what follows.
		skip = alloc_state(NFA_SKIP, ರಿಕ್ತ, ರಿಕ್ತ);
		ಯದಿ (skip == ರಿಕ್ತ)
		    ಗಚ್ಛ theend;
		zend = alloc_state(NFA_ZEND, s1, ರಿಕ್ತ);
		ಯದಿ (zend == ರಿಕ್ತ)
		    ಗಚ್ಛ theend;
		s1->out= skip;
		patch(e.out, zend);
		ತಳ್ಳು_2(frag(s, list1(&skip->out)));
	    }
	    ಅನ್ಯಥಾ
	    {
		patch(e.out, s1);
		ತಳ್ಳು_2(frag(s, list1(&s1->out)));
		ಯದಿ (before)
		{
		    ಯದಿ (n <= 0)
			// See if we can guess the maximum width, it avoids a
			// lot of pointless tries.
			n = nfa_max_width(e.start, 0);
		    s->val = n; // store the count
		}
	    }
	    ಭಙ್ಗ;
	  }

	ಪಕ್ಷ NFA_COMPOSING:	// char with composing char
#ಯದಿ 0
	    // TODO
	    ಯದಿ (regflags & RF_ICOMBINE)
	    {
		// use the base character only
	    }
#endif
	    // FALLTHROUGH

	ಪಕ್ಷ NFA_MOPEN:	// \( \) Submatch
	ಪಕ್ಷ NFA_MOPEN1:
	ಪಕ್ಷ NFA_MOPEN2:
	ಪಕ್ಷ NFA_MOPEN3:
	ಪಕ್ಷ NFA_MOPEN4:
	ಪಕ್ಷ NFA_MOPEN5:
	ಪಕ್ಷ NFA_MOPEN6:
	ಪಕ್ಷ NFA_MOPEN7:
	ಪಕ್ಷ NFA_MOPEN8:
	ಪಕ್ಷ NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	ಪಕ್ಷ NFA_ZOPEN:	// \z( \) Submatch
	ಪಕ್ಷ NFA_ZOPEN1:
	ಪಕ್ಷ NFA_ZOPEN2:
	ಪಕ್ಷ NFA_ZOPEN3:
	ಪಕ್ಷ NFA_ZOPEN4:
	ಪಕ್ಷ NFA_ZOPEN5:
	ಪಕ್ಷ NFA_ZOPEN6:
	ಪಕ್ಷ NFA_ZOPEN7:
	ಪಕ್ಷ NFA_ZOPEN8:
	ಪಕ್ಷ NFA_ZOPEN9:
#endif
	ಪಕ್ಷ NFA_NOPEN:	// \%( \) "Invisible Submatch"
	    ಯದಿ (nfa_calc_size == TRUE)
	    {
		nstate += 2;
		ಭಙ್ಗ;
	    }

	    mopen = *p;
	    ವಿಕಲ್ಪ (*p)
	    {
		ಪಕ್ಷ NFA_NOPEN: mclose = NFA_NCLOSE; ಭಙ್ಗ;
#ifdef FEAT_SYN_HL
		ಪಕ್ಷ NFA_ZOPEN: mclose = NFA_ZCLOSE; ಭಙ್ಗ;
		ಪಕ್ಷ NFA_ZOPEN1: mclose = NFA_ZCLOSE1; ಭಙ್ಗ;
		ಪಕ್ಷ NFA_ZOPEN2: mclose = NFA_ZCLOSE2; ಭಙ್ಗ;
		ಪಕ್ಷ NFA_ZOPEN3: mclose = NFA_ZCLOSE3; ಭಙ್ಗ;
		ಪಕ್ಷ NFA_ZOPEN4: mclose = NFA_ZCLOSE4; ಭಙ್ಗ;
		ಪಕ್ಷ NFA_ZOPEN5: mclose = NFA_ZCLOSE5; ಭಙ್ಗ;
		ಪಕ್ಷ NFA_ZOPEN6: mclose = NFA_ZCLOSE6; ಭಙ್ಗ;
		ಪಕ್ಷ NFA_ZOPEN7: mclose = NFA_ZCLOSE7; ಭಙ್ಗ;
		ಪಕ್ಷ NFA_ZOPEN8: mclose = NFA_ZCLOSE8; ಭಙ್ಗ;
		ಪಕ್ಷ NFA_ZOPEN9: mclose = NFA_ZCLOSE9; ಭಙ್ಗ;
#endif
		ಪಕ್ಷ NFA_COMPOSING: mclose = NFA_END_COMPOSING; ಭಙ್ಗ;
		ಸ್ವಭಾವ:
		    // NFA_MOPEN, NFA_MOPEN1 .. NFA_MOPEN9
		    mclose = *p + NSUBEXP;
		    ಭಙ್ಗ;
	    }

	    // Allow "NFA_MOPEN" as a valid postfix representation for
	    // the empty regexp "". In this case, the NFA will be
	    // NFA_MOPEN -> NFA_MCLOSE. Note that this also allows
	    // empty groups of parenthesis, and empty mbyte chars
	    ಯದಿ (stackp == ರಾಶಿ)
	    {
		s = alloc_state(mopen, ರಿಕ್ತ, ರಿಕ್ತ);
		ಯದಿ (s == ರಿಕ್ತ)
		    ಗಚ್ಛ theend;
		s1 = alloc_state(mclose, ರಿಕ್ತ, ರಿಕ್ತ);
		ಯದಿ (s1 == ರಿಕ್ತ)
		    ಗಚ್ಛ theend;
		patch(list1(&s->out), s1);
		ತಳ್ಳು_2(frag(s, list1(&s1->out)));
		ಭಙ್ಗ;
	    }

	    // At least one node was emitted before NFA_MOPEN, so
	    // at least one node will be between NFA_MOPEN and NFA_MCLOSE
	    e = ತೆಗೆ_2();
	    s = alloc_state(mopen, e.start, ರಿಕ್ತ);   // `('
	    ಯದಿ (s == ರಿಕ್ತ)
		ಗಚ್ಛ theend;

	    s1 = alloc_state(mclose, ರಿಕ್ತ, ರಿಕ್ತ);   // `)'
	    ಯದಿ (s1 == ರಿಕ್ತ)
		ಗಚ್ಛ theend;
	    patch(e.out, s1);

	    ಯದಿ (mopen == NFA_COMPOSING)
		// COMPOSING->out1 = END_COMPOSING
		patch(list1(&s->out1), s1);

	    ತಳ್ಳು_2(frag(s, list1(&s1->out)));
	    ಭಙ್ಗ;

	ಪಕ್ಷ NFA_BACKREF1:
	ಪಕ್ಷ NFA_BACKREF2:
	ಪಕ್ಷ NFA_BACKREF3:
	ಪಕ್ಷ NFA_BACKREF4:
	ಪಕ್ಷ NFA_BACKREF5:
	ಪಕ್ಷ NFA_BACKREF6:
	ಪಕ್ಷ NFA_BACKREF7:
	ಪಕ್ಷ NFA_BACKREF8:
	ಪಕ್ಷ NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	ಪಕ್ಷ NFA_ZREF1:
	ಪಕ್ಷ NFA_ZREF2:
	ಪಕ್ಷ NFA_ZREF3:
	ಪಕ್ಷ NFA_ZREF4:
	ಪಕ್ಷ NFA_ZREF5:
	ಪಕ್ಷ NFA_ZREF6:
	ಪಕ್ಷ NFA_ZREF7:
	ಪಕ್ಷ NFA_ZREF8:
	ಪಕ್ಷ NFA_ZREF9:
#endif
	    ಯದಿ (nfa_calc_size == TRUE)
	    {
		nstate += 2;
		ಭಙ್ಗ;
	    }
	    s = alloc_state(*p, ರಿಕ್ತ, ರಿಕ್ತ);
	    ಯದಿ (s == ರಿಕ್ತ)
		ಗಚ್ಛ theend;
	    s1 = alloc_state(NFA_SKIP, ರಿಕ್ತ, ರಿಕ್ತ);
	    ಯದಿ (s1 == ರಿಕ್ತ)
		ಗಚ್ಛ theend;
	    patch(list1(&s->out), s1);
	    ತಳ್ಳು_2(frag(s, list1(&s1->out)));
	    ಭಙ್ಗ;

	ಪಕ್ಷ NFA_LNUM:
	ಪಕ್ಷ NFA_LNUM_GT:
	ಪಕ್ಷ NFA_LNUM_LT:
	ಪಕ್ಷ NFA_VCOL:
	ಪಕ್ಷ NFA_VCOL_GT:
	ಪಕ್ಷ NFA_VCOL_LT:
	ಪಕ್ಷ NFA_COL:
	ಪಕ್ಷ NFA_COL_GT:
	ಪಕ್ಷ NFA_COL_LT:
	ಪಕ್ಷ NFA_MARK:
	ಪಕ್ಷ NFA_MARK_GT:
	ಪಕ್ಷ NFA_MARK_LT:
	  {
	    ಪೂರ್ಣಾಙ್ಕ n = *++p; // lnum, col or mark name

	    ಯದಿ (nfa_calc_size == TRUE)
	    {
		nstate += 1;
		ಭಙ್ಗ;
	    }
	    s = alloc_state(p[-1], ರಿಕ್ತ, ರಿಕ್ತ);
	    ಯದಿ (s == ರಿಕ್ತ)
		ಗಚ್ಛ theend;
	    s->val = n;
	    ತಳ್ಳು_2(frag(s, list1(&s->out)));
	    ಭಙ್ಗ;
	  }

	ಪಕ್ಷ NFA_ZSTART:
	ಪಕ್ಷ NFA_ZEND:
	ಸ್ವಭಾವ:
	    // Operands
	    ಯದಿ (nfa_calc_size == TRUE)
	    {
		nstate++;
		ಭಙ್ಗ;
	    }
	    s = alloc_state(*p, ರಿಕ್ತ, ರಿಕ್ತ);
	    ಯದಿ (s == ರಿಕ್ತ)
		ಗಚ್ಛ theend;
	    ತಳ್ಳು_2(frag(s, list1(&s->out)));
	    ಭಙ್ಗ;

	} // switch(*p)

    } // for (p = postfix; *p; ++p)

    ಯದಿ (nfa_calc_size == TRUE)
    {
	nstate++;
	ಗಚ್ಛ theend;	// Return value when counting size is ignored anyway
    }

    e = ತೆಗೆ_2();
    ಯದಿ (stackp != ರಾಶಿ)
	EMSG_RET_NULL(_(e_nfa_regexp_while_converting_from_postfix_to_nfa_too_many_stats_ಎಡ_on_ರಾಶಿ));

    ಯದಿ (istate >= nstate)
	EMSG_RET_NULL(_(e_nfa_regexp_not_enough_space_to_store_whole_nfa));

    matchstate = &state_ptr[istate++]; // the match state
    matchstate->c = NFA_MATCH;
    matchstate->out = matchstate->out1 = ರಿಕ್ತ;
    matchstate->id = 0;

    patch(e.out, matchstate);
    ret = e.start;

theend:
    ಪ್ರತ್ಯಾಗಮ ret;

#undef POP1
#undef PUSH1
#undef POP2
#undef PUSH2
#undef ತೆಗೆ_2
#undef ತಳ್ಳು_2
}

/*
 * After building the NFA program, inspect it to add optimization hints.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
nfa_postprocess(nfa_regprog_T *prog)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    ಪೂರ್ಣಾಙ್ಕ c;

    ಪ್ರತಿ (i = 0; i < prog->nstate; ++i)
    {
	c = prog->state[i].c;
	ಯದಿ (c == NFA_START_INVISIBLE
		|| c == NFA_START_INVISIBLE_NEG
		|| c == NFA_START_INVISIBLE_BEFORE
		|| c == NFA_START_INVISIBLE_BEFORE_NEG)
	{
	    ಪೂರ್ಣಾಙ್ಕ directly;

	    // Do it directly when what follows is possibly the end of the
	    // match.
	    ಯದಿ (match_follows(prog->state[i].out1->out, 0))
		directly = TRUE;
	    ಅನ್ಯಥಾ
	    {
		ಪೂರ್ಣಾಙ್ಕ ch_invisible = failure_chance(prog->state[i].out, 0);
		ಪೂರ್ಣಾಙ್ಕ ch_follows = failure_chance(prog->state[i].out1->out, 0);

		// Postpone when the invisible match is expensive or has a
		// lower chance of failing.
		ಯದಿ (c == NFA_START_INVISIBLE_BEFORE
		     || c == NFA_START_INVISIBLE_BEFORE_NEG)
		{
		    // "before" matches are very expensive when
		    // unbounded, always prefer what follows then,
		    // unless what follows will always match.
		    // Otherwise strongly prefer what follows.
		    ಯದಿ (prog->state[i].val <= 0 && ch_follows > 0)
			directly = FALSE;
		    ಅನ್ಯಥಾ
			directly = ch_follows * 10 < ch_invisible;
		}
		ಅನ್ಯಥಾ
		{
		    // normal invisible, first do the one with the
		    // highest failure chance
		    directly = ch_follows < ch_invisible;
		}
	    }
	    ಯದಿ (directly)
		// switch to the _FIRST state
		++prog->state[i].c;
	}
    }
}

/////////////////////////////////////////////////////////////////
// NFA execution code.
/////////////////////////////////////////////////////////////////

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ಪೂರ್ಣಾಙ್ಕ	    in_use; // number of subexpr with useful info

    // When REG_MULTI is TRUE list.multi is used, otherwise list.line.
    ಸಙ್ಘ
    {
	ಸಂರಚನಾ multipos
	{
	    linenr_T	start_lnum;
	    linenr_T	end_lnum;
	    colnr_T	start_col;
	    colnr_T	end_col;
	} multi[NSUBEXP];
	ಸಂರಚನಾ linepos
	{
	    char_u	*start;
	    char_u	*end;
	} ಸಾಲು[NSUBEXP];
    } list;
    colnr_T	orig_start_col;  // list.multi[0].start_col without \zs
} regsub_T;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    regsub_T	norm; // \( .. \) matches
#ifdef FEAT_SYN_HL
    regsub_T	synt; // \z( .. \) matches
#endif
} regsubs_T;

// nfa_pim_T stores a Postponed Invisible Match.
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ nfa_pim_S nfa_pim_T;
ಸಂರಚನಾ nfa_pim_S
{
    ಪೂರ್ಣಾಙ್ಕ		result;		// NFA_PIM_*, see below
    nfa_state_T	*state;		// the invisible match start state
    regsubs_T	subs;		// submatch info, only party used
    ಸಙ್ಘ
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
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    nfa_state_T	*state;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ;
    nfa_pim_T	pim;		// if pim.result != NFA_PIM_UNUSED: postponed
				// invisible match
    regsubs_T	subs;		// submatch info, only party used
} nfa_thread_T;

// nfa_list_T contains the alternative NFA execution states.
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    nfa_thread_T    *t;		// allocated array of states
    ಪೂರ್ಣಾಙ್ಕ		    n;		// nr of states currently in "t"
    ಪೂರ್ಣಾಙ್ಕ		    len;	// max nr of states in "t"
    ಪೂರ್ಣಾಙ್ಕ		    id;		// ID of the list
    ಪೂರ್ಣಾಙ್ಕ		    has_pim;	// TRUE when any state has a PIM
} nfa_list_T;

#ifdef ENABLE_LOG
ಸ್ಥಿರ ಶೂನ್ಯ log_subexpr(regsub_T *sub);

    ಸ್ಥಿರ ಶೂನ್ಯ
log_subsexpr(regsubs_T *subs)
{
    log_subexpr(&subs->norm);
# ifdef FEAT_SYN_HL
    ಯದಿ (rex.nfa_has_zsubexpr)
	log_subexpr(&subs->synt);
# endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
log_subexpr(regsub_T *sub)
{
    ಪೂರ್ಣಾಙ್ಕ j;

    ಪ್ರತಿ (j = 0; j < sub->in_use; j++)
	ಯದಿ (REG_MULTI)
	    fprintf(log_fd,
		    "*** group %d, start: c=%d, l=%d, end: c=%d, l=%d\n",
		    j,
		    sub->list.multi[j].start_col,
		    (ಪೂರ್ಣಾಙ್ಕ)sub->list.multi[j].start_lnum,
		    sub->list.multi[j].end_col,
		    (ಪೂರ್ಣಾಙ್ಕ)sub->list.multi[j].end_lnum);
	ಅನ್ಯಥಾ
	{
	    ವರ್ಣ *s = (ವರ್ಣ *)sub->list.ಸಾಲು[j].start;
	    ವರ್ಣ *e = (ವರ್ಣ *)sub->list.ಸಾಲು[j].end;

	    fprintf(log_fd, "*** group %d, start: \"%s\", end: \"%s\"\n",
		    j,
		    s == ರಿಕ್ತ ? "NULL" : s,
		    e == ರಿಕ್ತ ? "NULL" : e);
	}
}

    ಸ್ಥಿರ ವರ್ಣ *
pim_info(nfa_pim_T *pim)
{
    ಸ್ಥಿರ ವರ್ಣ buf[30];

    ಯದಿ (pim == ರಿಕ್ತ || pim->result == NFA_PIM_UNUSED)
	buf[0] = NUL;
    ಅನ್ಯಥಾ
    {
	sprintf(buf, " PIM col %d", REG_MULTI ? (ಪೂರ್ಣಾಙ್ಕ)pim->end.pos.col
		: (ಪೂರ್ಣಾಙ್ಕ)(pim->end.ptr - rex.ಒಳಹರಿವು));
    }
    ಪ್ರತ್ಯಾಗಮ buf;
}

#endif

// Used during execution: whether a match has been found.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    nfa_match;
#ifdef FEAT_RELTIME
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	   *nfa_timed_out;
#endif

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ copy_sub(regsub_T *to, regsub_T *from);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pim_equal(nfa_pim_T *one, nfa_pim_T *two);

/*
 * Copy postponed invisible match info from "from" to "to".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
copy_pim(nfa_pim_T *to, nfa_pim_T *from)
{
    to->result = from->result;
    to->state = from->state;
    copy_sub(&to->subs.norm, &from->subs.norm);
#ifdef FEAT_SYN_HL
    ಯದಿ (rex.nfa_has_zsubexpr)
	copy_sub(&to->subs.synt, &from->subs.synt);
#endif
    to->end = from->end;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clear_sub(regsub_T *sub)
{
    ಯದಿ (REG_MULTI)
	// Use 0xff to set lnum to -1
	vim_memset(sub->list.multi, 0xff,
				  ಪರಿಮಾಣ(ಸಂರಚನಾ multipos) * rex.nfa_nsubexpr);
    ಅನ್ಯಥಾ
	vim_memset(sub->list.ಸಾಲು, 0,
				   ಪರಿಮಾಣ(ಸಂರಚನಾ linepos) * rex.nfa_nsubexpr);
    sub->in_use = 0;
}

/*
 * Copy the submatches from "from" to "to".
 */
    ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ
copy_sub(regsub_T *to, regsub_T *from)
{
    to->in_use = from->in_use;
    ಯದಿ (from->in_use <= 0)
	ಪ್ರತ್ಯಾಗಮ;

    // Copy the match start and end positions.  addstate() calls this for
    // every state it adds, so it is very hot.  Patterns without capturing
    // groups only use subexpression zero (the whole match), so special-case
    // "in_use == 1" with a direct assignment to avoid the mch_memmove() call.
    ಯದಿ (REG_MULTI)
    {
	ಯದಿ (from->in_use == 1)
	    to->list.multi[0] = from->list.multi[0];
	ಅನ್ಯಥಾ
	    mch_memmove(&to->list.multi[0],
		    &from->list.multi[0],
		    ಪರಿಮಾಣ(ಸಂರಚನಾ multipos) * from->in_use);
	to->orig_start_col = from->orig_start_col;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (from->in_use == 1)
	    to->list.ಸಾಲು[0] = from->list.ಸಾಲು[0];
	ಅನ್ಯಥಾ
	    mch_memmove(&to->list.ಸಾಲು[0],
		    &from->list.ಸಾಲು[0],
		    ಪರಿಮಾಣ(ಸಂರಚನಾ linepos) * from->in_use);
    }
}

/*
 * Like copy_sub() but exclude the main match.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
copy_sub_off(regsub_T *to, regsub_T *from)
{
    ಯದಿ (to->in_use < from->in_use)
	to->in_use = from->in_use;
    ಯದಿ (from->in_use <= 1)
	ಪ್ರತ್ಯಾಗಮ;

    // Copy the match start and end positions.
    ಯದಿ (REG_MULTI)
	mch_memmove(&to->list.multi[1],
		&from->list.multi[1],
		ಪರಿಮಾಣ(ಸಂರಚನಾ multipos) * (from->in_use - 1));
    ಅನ್ಯಥಾ
	mch_memmove(&to->list.ಸಾಲು[1],
		&from->list.ಸಾಲು[1],
		ಪರಿಮಾಣ(ಸಂರಚನಾ linepos) * (from->in_use - 1));
}

/*
 * Like copy_sub() but only do the end of the main match if \ze is present.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
copy_ze_off(regsub_T *to, regsub_T *from)
{
    ಯದಿ (!rex.nfa_has_zend)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (REG_MULTI)
    {
	ಯದಿ (from->list.multi[0].end_lnum >= 0)
	{
	    to->list.multi[0].end_lnum = from->list.multi[0].end_lnum;
	    to->list.multi[0].end_col = from->list.multi[0].end_col;
	}
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (from->list.ಸಾಲು[0].end != ರಿಕ್ತ)
	    to->list.ಸಾಲು[0].end = from->list.ಸಾಲು[0].end;
    }
}

/*
 * Return TRUE if "sub1" and "sub2" have the same start positions.
 * When using back-references also check the end position.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
sub_equal(regsub_T *sub1, regsub_T *sub2)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		todo;
    linenr_T	s1;
    linenr_T	s2;
    char_u	*sp1;
    char_u	*sp2;

    todo = sub1->in_use > sub2->in_use ? sub1->in_use : sub2->in_use;
    ಯದಿ (REG_MULTI)
    {
	ಪ್ರತಿ (i = 0; i < todo; ++i)
	{
	    ಯದಿ (i < sub1->in_use)
		s1 = sub1->list.multi[i].start_lnum;
	    ಅನ್ಯಥಾ
		s1 = -1;
	    ಯದಿ (i < sub2->in_use)
		s2 = sub2->list.multi[i].start_lnum;
	    ಅನ್ಯಥಾ
		s2 = -1;
	    ಯದಿ (s1 != s2)
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    ಯದಿ (s1 != -1 && sub1->list.multi[i].start_col
					     != sub2->list.multi[i].start_col)
		ಪ್ರತ್ಯಾಗಮ FALSE;

	    ಯದಿ (rex.nfa_has_backref)
	    {
		ಯದಿ (i < sub1->in_use)
		    s1 = sub1->list.multi[i].end_lnum;
		ಅನ್ಯಥಾ
		    s1 = -1;
		ಯದಿ (i < sub2->in_use)
		    s2 = sub2->list.multi[i].end_lnum;
		ಅನ್ಯಥಾ
		    s2 = -1;
		ಯದಿ (s1 != s2)
		    ಪ್ರತ್ಯಾಗಮ FALSE;
		ಯದಿ (s1 != -1 && sub1->list.multi[i].end_col
					       != sub2->list.multi[i].end_col)
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    }
	}
    }
    ಅನ್ಯಥಾ
    {
	ಪ್ರತಿ (i = 0; i < todo; ++i)
	{
	    ಯದಿ (i < sub1->in_use)
		sp1 = sub1->list.ಸಾಲು[i].start;
	    ಅನ್ಯಥಾ
		sp1 = ರಿಕ್ತ;
	    ಯದಿ (i < sub2->in_use)
		sp2 = sub2->list.ಸಾಲು[i].start;
	    ಅನ್ಯಥಾ
		sp2 = ರಿಕ್ತ;
	    ಯದಿ (sp1 != sp2)
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    ಯದಿ (rex.nfa_has_backref)
	    {
		ಯದಿ (i < sub1->in_use)
		    sp1 = sub1->list.ಸಾಲು[i].end;
		ಅನ್ಯಥಾ
		    sp1 = ರಿಕ್ತ;
		ಯದಿ (i < sub2->in_use)
		    sp2 = sub2->list.ಸಾಲು[i].end;
		ಅನ್ಯಥಾ
		    sp2 = ರಿಕ್ತ;
		ಯದಿ (sp1 != sp2)
		    ಪ್ರತ್ಯಾಗಮ FALSE;
	    }
	}
    }

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

#ifdef FEAT_RELTIME
/*
 * Check if we are past the time limit, if there is one.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
nfa_did_time_out(ಶೂನ್ಯ)
{
    ಯದಿ (*timeout_flag)
    {
	ಯದಿ (nfa_timed_out != ರಿಕ್ತ)
	{
# ifdef FEAT_EVAL
	    ಯದಿ (!*nfa_timed_out)
		ch_log(ರಿಕ್ತ, "NFA regexp timed out");
# endif
	    *nfa_timed_out = TRUE;
	}
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}
#endif

#ifdef ENABLE_LOG
    ಸ್ಥಿರ ಶೂನ್ಯ
open_debug_log(ಪೂರ್ಣಾಙ್ಕ result)
{
    log_fd = fopen(NFA_REGEXP_RUN_LOG, "a");
    ಯದಿ (log_fd == ರಿಕ್ತ)
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

    ಸ್ಥಿರ ಶೂನ್ಯ
report_state(ವರ್ಣ *action,
	     regsub_T *sub,
	     nfa_state_T *state,
	     ಪೂರ್ಣಾಙ್ಕ lid,
	     nfa_pim_T *pim)
{
    ಪೂರ್ಣಾಙ್ಕ col;

    ಯದಿ (sub->in_use <= 0)
	col = -1;
    ಅನ್ಯಥಾ ಯದಿ (REG_MULTI)
	col = sub->list.multi[0].start_col;
    ಅನ್ಯಥಾ
	col = (ಪೂರ್ಣಾಙ್ಕ)(sub->list.ಸಾಲು[0].start - rex.ಸಾಲು);
    nfa_set_code(state->c);
    ಯದಿ (log_fd == ರಿಕ್ತ)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
has_state_with_pos(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim)	// postponed match or NULL
{
    nfa_thread_T	*thread;
    ಪೂರ್ಣಾಙ್ಕ			i;

    ಪ್ರತಿ (i = 0; i < l->n; ++i)
    {
	thread = &l->t[i];
	ಯದಿ (thread->state->id == state->id
		&& sub_equal(&thread->subs.norm, &subs->norm)
#ifdef FEAT_SYN_HL
		&& (!rex.nfa_has_zsubexpr
				|| sub_equal(&thread->subs.synt, &subs->synt))
#endif
		&& pim_equal(&thread->pim, pim))
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Return TRUE if "one" and "two" are equal.  That includes when both are not
 * set.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
pim_equal(nfa_pim_T *one, nfa_pim_T *two)
{
    ಪೂರ್ಣಾಙ್ಕ one_unused = (one == ರಿಕ್ತ || one->result == NFA_PIM_UNUSED);
    ಪೂರ್ಣಾಙ್ಕ two_unused = (two == ರಿಕ್ತ || two->result == NFA_PIM_UNUSED);

    ಯದಿ (one_unused)
	// one is unused: equal when two is also unused
	ಪ್ರತ್ಯಾಗಮ two_unused;
    ಯದಿ (two_unused)
	// one is used and two is not: not equal
	ಪ್ರತ್ಯಾಗಮ FALSE;
    // compare the state id
    ಯದಿ (one->state->id != two->state->id)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    // compare the position
    ಯದಿ (REG_MULTI)
	ಪ್ರತ್ಯಾಗಮ one->end.pos.lnum == two->end.pos.lnum
	    && one->end.pos.col == two->end.pos.col;
    ಪ್ರತ್ಯಾಗಮ one->end.ptr == two->end.ptr;
}

/*
 * Return TRUE if "state" leads to a NFA_MATCH without advancing the input.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
match_follows(nfa_state_T *startstate, ಪೂರ್ಣಾಙ್ಕ depth)
{
    nfa_state_T	    *state = startstate;

    // avoid too much recursion
    ಯದಿ (depth > 10)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯಾವತ್ (state != ರಿಕ್ತ)
    {
	ವಿಕಲ್ಪ (state->c)
	{
	    ಪಕ್ಷ NFA_MATCH:
	    ಪಕ್ಷ NFA_MCLOSE:
	    ಪಕ್ಷ NFA_END_INVISIBLE:
	    ಪಕ್ಷ NFA_END_INVISIBLE_NEG:
	    ಪಕ್ಷ NFA_END_PATTERN:
		ಪ್ರತ್ಯಾಗಮ TRUE;

	    ಪಕ್ಷ NFA_SPLIT:
		ಪ್ರತ್ಯಾಗಮ match_follows(state->out, depth + 1)
				     || match_follows(state->out1, depth + 1);

	    ಪಕ್ಷ NFA_START_INVISIBLE:
	    ಪಕ್ಷ NFA_START_INVISIBLE_FIRST:
	    ಪಕ್ಷ NFA_START_INVISIBLE_BEFORE:
	    ಪಕ್ಷ NFA_START_INVISIBLE_BEFORE_FIRST:
	    ಪಕ್ಷ NFA_START_INVISIBLE_NEG:
	    ಪಕ್ಷ NFA_START_INVISIBLE_NEG_FIRST:
	    ಪಕ್ಷ NFA_START_INVISIBLE_BEFORE_NEG:
	    ಪಕ್ಷ NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
	    ಪಕ್ಷ NFA_COMPOSING:
		// skip ahead to next state
		state = state->out1->out;
		ಅನುವರ್ತ;

	    ಪಕ್ಷ NFA_ANY:
	    ಪಕ್ಷ NFA_ANY_COMPOSING:
	    ಪಕ್ಷ NFA_IDENT:
	    ಪಕ್ಷ NFA_SIDENT:
	    ಪಕ್ಷ NFA_KWORD:
	    ಪಕ್ಷ NFA_SKWORD:
	    ಪಕ್ಷ NFA_FNAME:
	    ಪಕ್ಷ NFA_SFNAME:
	    ಪಕ್ಷ NFA_PRINT:
	    ಪಕ್ಷ NFA_SPRINT:
	    ಪಕ್ಷ NFA_WHITE:
	    ಪಕ್ಷ NFA_NWHITE:
	    ಪಕ್ಷ NFA_DIGIT:
	    ಪಕ್ಷ NFA_NDIGIT:
	    ಪಕ್ಷ NFA_HEX:
	    ಪಕ್ಷ NFA_NHEX:
	    ಪಕ್ಷ NFA_OCTAL:
	    ಪಕ್ಷ NFA_NOCTAL:
	    ಪಕ್ಷ NFA_WORD:
	    ಪಕ್ಷ NFA_NWORD:
	    ಪಕ್ಷ NFA_HEAD:
	    ಪಕ್ಷ NFA_NHEAD:
	    ಪಕ್ಷ NFA_ALPHA:
	    ಪಕ್ಷ NFA_NALPHA:
	    ಪಕ್ಷ NFA_LOWER:
	    ಪಕ್ಷ NFA_NLOWER:
	    ಪಕ್ಷ NFA_UPPER:
	    ಪಕ್ಷ NFA_NUPPER:
	    ಪಕ್ಷ NFA_LOWER_IC:
	    ಪಕ್ಷ NFA_NLOWER_IC:
	    ಪಕ್ಷ NFA_UPPER_IC:
	    ಪಕ್ಷ NFA_NUPPER_IC:
	    ಪಕ್ಷ NFA_START_COLL:
	    ಪಕ್ಷ NFA_START_NEG_COLL:
	    ಪಕ್ಷ NFA_NEWL:
		// state will advance input
		ಪ್ರತ್ಯಾಗಮ FALSE;

	    ಸ್ವಭಾವ:
		ಯದಿ (state->c > 0)
		    // state will advance input
		    ಪ್ರತ್ಯಾಗಮ FALSE;

		// Others: zero-width or possibly zero-width, might still find
		// a match at the same position, keep looking.
		ಭಙ್ಗ;
	}
	state = state->out;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}


/*
 * Return TRUE if "state" is already in list "l".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
state_in_list(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs)	// pointers to subexpressions
{
    ಯದಿ (state->lastlist[nfa_ll_index] == l->id)
    {
	ಯದಿ (!rex.nfa_has_backref || has_state_with_pos(l, state, subs, ರಿಕ್ತ))
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

// Offset used for "off" by addstate_here().
#define ADDSTATE_HERE_OFFSET 10

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
    ಪೂರ್ಣಾಙ್ಕ			off_arg,    // byte offset, when -1 go to next line
    ಪೂರ್ಣಾಙ್ಕ			depth)	    // recursion depth
{
    ಪೂರ್ಣಾಙ್ಕ			subidx;
    ಪೂರ್ಣಾಙ್ಕ			off = off_arg;
    ಪೂರ್ಣಾಙ್ಕ			add_here = FALSE;
    ಪೂರ್ಣಾಙ್ಕ			listindex = 0;
    ಪೂರ್ಣಾಙ್ಕ			k;
    ಪೂರ್ಣಾಙ್ಕ			found = FALSE;
    nfa_thread_T	*thread;
    ಸಂರಚನಾ multipos	save_multipos;
    ಪೂರ್ಣಾಙ್ಕ			save_in_use;
    char_u		*save_ptr;
    ಪೂರ್ಣಾಙ್ಕ			i;
    regsub_T		*sub;
    regsubs_T		*subs = subs_arg;
    ಸ್ಥಿರ regsubs_T	temp_subs;
#ifdef ENABLE_LOG
    ಪೂರ್ಣಾಙ್ಕ			did_print = FALSE;
#endif

#ifdef FEAT_RELTIME
    ಯದಿ (nfa_did_time_out())
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
#endif

    // This function is called recursively.  When the depth is too much we run
    // out of stack and crash, limit recursiveness here.
    ಯದಿ (depth >= 5000 || subs == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (off_arg <= -ADDSTATE_HERE_OFFSET)
    {
	add_here = TRUE;
	off = 0;
	listindex = -(off_arg + ADDSTATE_HERE_OFFSET);
    }

    ವಿಕಲ್ಪ (state->c)
    {
	ಪಕ್ಷ NFA_NCLOSE:
	ಪಕ್ಷ NFA_MCLOSE:
	ಪಕ್ಷ NFA_MCLOSE1:
	ಪಕ್ಷ NFA_MCLOSE2:
	ಪಕ್ಷ NFA_MCLOSE3:
	ಪಕ್ಷ NFA_MCLOSE4:
	ಪಕ್ಷ NFA_MCLOSE5:
	ಪಕ್ಷ NFA_MCLOSE6:
	ಪಕ್ಷ NFA_MCLOSE7:
	ಪಕ್ಷ NFA_MCLOSE8:
	ಪಕ್ಷ NFA_MCLOSE9:
#ifdef FEAT_SYN_HL
	ಪಕ್ಷ NFA_ZCLOSE:
	ಪಕ್ಷ NFA_ZCLOSE1:
	ಪಕ್ಷ NFA_ZCLOSE2:
	ಪಕ್ಷ NFA_ZCLOSE3:
	ಪಕ್ಷ NFA_ZCLOSE4:
	ಪಕ್ಷ NFA_ZCLOSE5:
	ಪಕ್ಷ NFA_ZCLOSE6:
	ಪಕ್ಷ NFA_ZCLOSE7:
	ಪಕ್ಷ NFA_ZCLOSE8:
	ಪಕ್ಷ NFA_ZCLOSE9:
#endif
	ಪಕ್ಷ NFA_MOPEN:
	ಪಕ್ಷ NFA_ZEND:
	ಪಕ್ಷ NFA_SPLIT:
	ಪಕ್ಷ NFA_EMPTY:
	    // These nodes are not added themselves but their "out" and/or
	    // "out1" may be added below.
	    ಭಙ್ಗ;

	ಪಕ್ಷ NFA_BOL:
	ಪಕ್ಷ NFA_BOF:
	    // "^" won't match past end-of-line, don't bother trying.
	    // Except when at the end of the line, or when we are going to the
	    // next line for a look-behind match.
	    ಯದಿ (rex.ಒಳಹರಿವು > rex.ಸಾಲು
		    && *rex.ಒಳಹರಿವು != NUL
		    && (nfa_endp == ರಿಕ್ತ
			|| !REG_MULTI
			|| rex.lnum == nfa_endp->se_u.pos.lnum))
		ಗಚ್ಛ skip_add;
	    // FALLTHROUGH

	ಪಕ್ಷ NFA_MOPEN1:
	ಪಕ್ಷ NFA_MOPEN2:
	ಪಕ್ಷ NFA_MOPEN3:
	ಪಕ್ಷ NFA_MOPEN4:
	ಪಕ್ಷ NFA_MOPEN5:
	ಪಕ್ಷ NFA_MOPEN6:
	ಪಕ್ಷ NFA_MOPEN7:
	ಪಕ್ಷ NFA_MOPEN8:
	ಪಕ್ಷ NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	ಪಕ್ಷ NFA_ZOPEN:
	ಪಕ್ಷ NFA_ZOPEN1:
	ಪಕ್ಷ NFA_ZOPEN2:
	ಪಕ್ಷ NFA_ZOPEN3:
	ಪಕ್ಷ NFA_ZOPEN4:
	ಪಕ್ಷ NFA_ZOPEN5:
	ಪಕ್ಷ NFA_ZOPEN6:
	ಪಕ್ಷ NFA_ZOPEN7:
	ಪಕ್ಷ NFA_ZOPEN8:
	ಪಕ್ಷ NFA_ZOPEN9:
#endif
	ಪಕ್ಷ NFA_NOPEN:
	ಪಕ್ಷ NFA_ZSTART:
	    // These nodes need to be added so that we can bail out when it
	    // was added to this list before at the same position to avoid an
	    // endless loop for "\(\)*"

	ಸ್ವಭಾವ:
	    ಯದಿ (state->lastlist[nfa_ll_index] == l->id && state->c != NFA_SKIP)
	    {
		// This state is already in the list, don't add it again,
		// unless it is an MOPEN that is used for a backreference or
		// when there is a PIM. For NFA_MATCH check the position,
		// lower position is preferred.
		ಯದಿ (!rex.nfa_has_backref && pim == ರಿಕ್ತ && !l->has_pim
						     && state->c != NFA_MATCH)
		{
		    // When called from addstate_here() do insert before
		    // existing states.
		    ಯದಿ (add_here)
		    {
			ಪ್ರತಿ (k = 0; k < l->n && k < listindex; ++k)
			    ಯದಿ (l->t[k].state->id == state->id)
			    {
				found = TRUE;
				ಭಙ್ಗ;
			    }
		    }
		    ಯದಿ (!add_here || found)
		    {
skip_add:
#ifdef ENABLE_LOG
			nfa_set_code(state->c);
			fprintf(log_fd, "> Not adding state %d to list %d. char %d: %s pim: %s has_pim: %d found: %d\n",
			    abs(state->id), l->id, state->c, code,
			    pim == ರಿಕ್ತ ? "NULL" : "yes", l->has_pim, found);
#endif
			ಪ್ರತ್ಯಾಗಮ subs;
		    }
		}

		// Do not add the state again when it exists with the same
		// positions.
		ಯದಿ (has_state_with_pos(l, state, subs, pim))
		    ಗಚ್ಛ skip_add;
	    }

	    // When there are backreferences or PIMs the number of states may
	    // be (a lot) bigger than anticipated.
	    ಯದಿ (l->n == l->len)
	    {
		ಪೂರ್ಣಾಙ್ಕ		newlen = l->len * 3 / 2 + 50;
		size_t		newsize = newlen * ಪರಿಮಾಣ(nfa_thread_T);
		nfa_thread_T	*newt;

		ಯದಿ ((long)(newsize >> 10) >= p_mmp)
		{
		    emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		ಯದಿ (subs != &temp_subs)
		{
		    // "subs" may point into the current array, need to make a
		    // copy before it becomes invalid.
		    copy_sub(&temp_subs.norm, &subs->norm);
#ifdef FEAT_SYN_HL
		    ಯದಿ (rex.nfa_has_zsubexpr)
			copy_sub(&temp_subs.synt, &subs->synt);
#endif
		    subs = &temp_subs;
		}

		newt = vim_realloc(l->t, newsize);
		ಯದಿ (newt == ರಿಕ್ತ)
		{
		    // out of memory
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		l->t = newt;
		l->len = newlen;
	    }

	    // add the state to the list
	    state->lastlist[nfa_ll_index] = l->id;
	    thread = &l->t[l->n++];
	    thread->state = state;
	    ಯದಿ (pim == ರಿಕ್ತ)
		thread->pim.result = NFA_PIM_UNUSED;
	    ಅನ್ಯಥಾ
	    {
		copy_pim(&thread->pim, pim);
		l->has_pim = TRUE;
	    }
	    copy_sub(&thread->subs.norm, &subs->norm);
#ifdef FEAT_SYN_HL
	    ಯದಿ (rex.nfa_has_zsubexpr)
		copy_sub(&thread->subs.synt, &subs->synt);
#endif
#ifdef ENABLE_LOG
	    report_state("Adding", &thread->subs.norm, state, l->id, pim);
	    did_print = TRUE;
#endif
    }

#ifdef ENABLE_LOG
    ಯದಿ (!did_print)
	report_state("Processing", &subs->norm, state, l->id, pim);
#endif
    ವಿಕಲ್ಪ (state->c)
    {
	ಪಕ್ಷ NFA_MATCH:
	    ಭಙ್ಗ;

	ಪಕ್ಷ NFA_SPLIT:
	    // order matters here
	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    subs = addstate(l, state->out1, subs, pim, off_arg, depth + 1);
	    ಭಙ್ಗ;

	ಪಕ್ಷ NFA_EMPTY:
	ಪಕ್ಷ NFA_NOPEN:
	ಪಕ್ಷ NFA_NCLOSE:
	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    ಭಙ್ಗ;

	ಪಕ್ಷ NFA_MOPEN:
	ಪಕ್ಷ NFA_MOPEN1:
	ಪಕ್ಷ NFA_MOPEN2:
	ಪಕ್ಷ NFA_MOPEN3:
	ಪಕ್ಷ NFA_MOPEN4:
	ಪಕ್ಷ NFA_MOPEN5:
	ಪಕ್ಷ NFA_MOPEN6:
	ಪಕ್ಷ NFA_MOPEN7:
	ಪಕ್ಷ NFA_MOPEN8:
	ಪಕ್ಷ NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	ಪಕ್ಷ NFA_ZOPEN:
	ಪಕ್ಷ NFA_ZOPEN1:
	ಪಕ್ಷ NFA_ZOPEN2:
	ಪಕ್ಷ NFA_ZOPEN3:
	ಪಕ್ಷ NFA_ZOPEN4:
	ಪಕ್ಷ NFA_ZOPEN5:
	ಪಕ್ಷ NFA_ZOPEN6:
	ಪಕ್ಷ NFA_ZOPEN7:
	ಪಕ್ಷ NFA_ZOPEN8:
	ಪಕ್ಷ NFA_ZOPEN9:
#endif
	ಪಕ್ಷ NFA_ZSTART:
	    ಯದಿ (state->c == NFA_ZSTART)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#ifdef FEAT_SYN_HL
	    ಅನ್ಯಥಾ ಯದಿ (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
	    {
		subidx = state->c - NFA_ZOPEN;
		sub = &subs->synt;
	    }
#endif
	    ಅನ್ಯಥಾ
	    {
		subidx = state->c - NFA_MOPEN;
		sub = &subs->norm;
	    }

	    // avoid compiler warnings
	    save_ptr = ರಿಕ್ತ;
	    CLEAR_FIELD(save_multipos);

	    // Set the position (with "off" added) in the subexpression.  Save
	    // and restore it when it was in use.  Otherwise fill any gap.
	    ಯದಿ (REG_MULTI)
	    {
		ಯದಿ (subidx < sub->in_use)
		{
		    save_multipos = sub->list.multi[subidx];
		    save_in_use = -1;
		}
		ಅನ್ಯಥಾ
		{
		    save_in_use = sub->in_use;
		    ಪ್ರತಿ (i = sub->in_use; i < subidx; ++i)
		    {
			sub->list.multi[i].start_lnum = -1;
			sub->list.multi[i].end_lnum = -1;
		    }
		    sub->in_use = subidx + 1;
		}
		ಯದಿ (off == -1)
		{
		    sub->list.multi[subidx].start_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].start_col = 0;
		}
		ಅನ್ಯಥಾ
		{
		    sub->list.multi[subidx].start_lnum = rex.lnum;
		    sub->list.multi[subidx].start_col =
					 (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು + off);
		}
		sub->list.multi[subidx].end_lnum = -1;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (subidx < sub->in_use)
		{
		    save_ptr = sub->list.ಸಾಲು[subidx].start;
		    save_in_use = -1;
		}
		ಅನ್ಯಥಾ
		{
		    save_in_use = sub->in_use;
		    ಪ್ರತಿ (i = sub->in_use; i < subidx; ++i)
		    {
			sub->list.ಸಾಲು[i].start = ರಿಕ್ತ;
			sub->list.ಸಾಲು[i].end = ರಿಕ್ತ;
		    }
		    sub->in_use = subidx + 1;
		}
		sub->list.ಸಾಲು[subidx].start = rex.ಒಳಹರಿವು + off;
	    }

	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    ಯದಿ (subs == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    // "subs" may have changed, need to set "sub" again
#ifdef FEAT_SYN_HL
	    ಯದಿ (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
		sub = &subs->synt;
	    ಅನ್ಯಥಾ
#endif
		sub = &subs->norm;

	    ಯದಿ (save_in_use == -1)
	    {
		ಯದಿ (REG_MULTI)
		    sub->list.multi[subidx] = save_multipos;
		ಅನ್ಯಥಾ
		    sub->list.ಸಾಲು[subidx].start = save_ptr;
	    }
	    ಅನ್ಯಥಾ
		sub->in_use = save_in_use;
	    ಭಙ್ಗ;

	ಪಕ್ಷ NFA_MCLOSE:
	    ಯದಿ (rex.nfa_has_zend && (REG_MULTI
			? subs->norm.list.multi[0].end_lnum >= 0
			: subs->norm.list.ಸಾಲು[0].end != ರಿಕ್ತ))
	    {
		// Do not overwrite the position set by \ze.
		subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
		ಭಙ್ಗ;
	    }
	    // FALLTHROUGH
	ಪಕ್ಷ NFA_MCLOSE1:
	ಪಕ್ಷ NFA_MCLOSE2:
	ಪಕ್ಷ NFA_MCLOSE3:
	ಪಕ್ಷ NFA_MCLOSE4:
	ಪಕ್ಷ NFA_MCLOSE5:
	ಪಕ್ಷ NFA_MCLOSE6:
	ಪಕ್ಷ NFA_MCLOSE7:
	ಪಕ್ಷ NFA_MCLOSE8:
	ಪಕ್ಷ NFA_MCLOSE9:
#ifdef FEAT_SYN_HL
	ಪಕ್ಷ NFA_ZCLOSE:
	ಪಕ್ಷ NFA_ZCLOSE1:
	ಪಕ್ಷ NFA_ZCLOSE2:
	ಪಕ್ಷ NFA_ZCLOSE3:
	ಪಕ್ಷ NFA_ZCLOSE4:
	ಪಕ್ಷ NFA_ZCLOSE5:
	ಪಕ್ಷ NFA_ZCLOSE6:
	ಪಕ್ಷ NFA_ZCLOSE7:
	ಪಕ್ಷ NFA_ZCLOSE8:
	ಪಕ್ಷ NFA_ZCLOSE9:
#endif
	ಪಕ್ಷ NFA_ZEND:
	    ಯದಿ (state->c == NFA_ZEND)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#ifdef FEAT_SYN_HL
	    ಅನ್ಯಥಾ ಯದಿ (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
	    {
		subidx = state->c - NFA_ZCLOSE;
		sub = &subs->synt;
	    }
#endif
	    ಅನ್ಯಥಾ
	    {
		subidx = state->c - NFA_MCLOSE;
		sub = &subs->norm;
	    }

	    // We don't fill in gaps here, there must have been an MOPEN that
	    // has done that.
	    save_in_use = sub->in_use;
	    ಯದಿ (sub->in_use <= subidx)
		sub->in_use = subidx + 1;
	    ಯದಿ (REG_MULTI)
	    {
		save_multipos = sub->list.multi[subidx];
		ಯದಿ (off == -1)
		{
		    sub->list.multi[subidx].end_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].end_col = 0;
		}
		ಅನ್ಯಥಾ
		{
		    sub->list.multi[subidx].end_lnum = rex.lnum;
		    sub->list.multi[subidx].end_col =
					  (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು + off);
		}
		// avoid compiler warnings
		save_ptr = ರಿಕ್ತ;
	    }
	    ಅನ್ಯಥಾ
	    {
		save_ptr = sub->list.ಸಾಲು[subidx].end;
		sub->list.ಸಾಲು[subidx].end = rex.ಒಳಹರಿವು + off;
		// avoid compiler warnings
		CLEAR_FIELD(save_multipos);
	    }

	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    ಯದಿ (subs == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    // "subs" may have changed, need to set "sub" again
#ifdef FEAT_SYN_HL
	    ಯದಿ (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
		sub = &subs->synt;
	    ಅನ್ಯಥಾ
#endif
		sub = &subs->norm;

	    ಯದಿ (REG_MULTI)
		sub->list.multi[subidx] = save_multipos;
	    ಅನ್ಯಥಾ
		sub->list.ಸಾಲು[subidx].end = save_ptr;
	    sub->in_use = save_in_use;
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ subs;
}

/*
 * Like addstate(), but the new state(s) are put at position "*ip".
 * Used for zero-width matches, next state to use is the added one.
 * This makes sure the order of states to be tried does not change, which
 * matters for alternatives.
 */
    ಸ್ಥಿರ regsubs_T *
addstate_here(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim,   // postponed look-behind match
    ಪೂರ್ಣಾಙ್ಕ			*ip)
{
    ಪೂರ್ಣಾಙ್ಕ tlen = l->n;
    ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ listidx = *ip;
    regsubs_T *r;

    // First add the state(s) at the end, so that we know how many there are.
    // Pass the listidx as offset (avoids adding another argument to
    // addstate()).
    r = addstate(l, state, subs, pim, -listidx - ADDSTATE_HERE_OFFSET, 0);
    ಯದಿ (r == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // when "*ip" was at the end of the list, nothing to do
    ಯದಿ (listidx + 1 == tlen)
	ಪ್ರತ್ಯಾಗಮ r;

    // re-order to put the new state at the current position
    ಎಣಿಕೆ = l->n - tlen;
    ಯದಿ (ಎಣಿಕೆ == 0)
	ಪ್ರತ್ಯಾಗಮ r; // no state got added
    ಯದಿ (ಎಣಿಕೆ == 1)
    {
	// overwrite the current state
	l->t[listidx] = l->t[l->n - 1];
    }
    ಅನ್ಯಥಾ ಯದಿ (ಎಣಿಕೆ > 1)
    {
	ಯದಿ (l->n + ಎಣಿಕೆ - 1 >= l->len)
	{
	    // not enough space to move the new states, reallocate the list
	    // and move the states to the right position
	    ಪೂರ್ಣಾಙ್ಕ		    newlen = l->len * 3 / 2 + 50;
	    size_t	    newsize = newlen * ಪರಿಮಾಣ(nfa_thread_T);
	    nfa_thread_T    *newl;

	    ಯದಿ ((long)(newsize >> 10) >= p_mmp)
	    {
		emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	    newl = alloc(newsize);
	    ಯದಿ (newl == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    l->len = newlen;
	    mch_memmove(&(newl[0]),
		    &(l->t[0]),
		    ಪರಿಮಾಣ(nfa_thread_T) * listidx);
	    mch_memmove(&(newl[listidx]),
		    &(l->t[l->n - ಎಣಿಕೆ]),
		    ಪರಿಮಾಣ(nfa_thread_T) * ಎಣಿಕೆ);
	    mch_memmove(&(newl[listidx + ಎಣಿಕೆ]),
		    &(l->t[listidx + 1]),
		    ಪರಿಮಾಣ(nfa_thread_T) * (l->n - ಎಣಿಕೆ - listidx - 1));
	    vim_free(l->t);
	    l->t = newl;
	}
	ಅನ್ಯಥಾ
	{
	    // make space for new states, then move them from the
	    // end to the current position
	    mch_memmove(&(l->t[listidx + ಎಣಿಕೆ]),
		    &(l->t[listidx + 1]),
		    ಪರಿಮಾಣ(nfa_thread_T) * (l->n - listidx - 1));
	    mch_memmove(&(l->t[listidx]),
		    &(l->t[l->n - 1]),
		    ಪರಿಮಾಣ(nfa_thread_T) * ಎಣಿಕೆ);
	}
    }
    --l->n;
    *ip = listidx - 1;

    ಪ್ರತ್ಯಾಗಮ r;
}

/*
 * Check character class "class" against current character c.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_char_class(ಪೂರ್ಣಾಙ್ಕ class, ಪೂರ್ಣಾಙ್ಕ c)
{
    ವಿಕಲ್ಪ (class)
    {
	ಪಕ್ಷ NFA_CLASS_ALNUM:
	    ಯದಿ (c >= 1 && c < 128 && isalnum(c))
		ಪ್ರತ್ಯಾಗಮ OK;
	    ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_ALPHA:
	    ಯದಿ (c >= 1 && c < 128 && isalpha(c))
		ಪ್ರತ್ಯಾಗಮ OK;
	    ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_BLANK:
	    ಯದಿ (c == ' ' || c == '\t')
		ಪ್ರತ್ಯಾಗಮ OK;
	    ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_CNTRL:
	    ಯದಿ (c >= 1 && c <= 127 && iscntrl(c))
		ಪ್ರತ್ಯಾಗಮ OK;
	    ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_DIGIT:
	    ಯದಿ (VIM_ISDIGIT(c))
		ಪ್ರತ್ಯಾಗಮ OK;
	    ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_GRAPH:
	    ಯದಿ (c >= 1 && c <= 127 && isgraph(c))
		ಪ್ರತ್ಯಾಗಮ OK;
	    ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_LOWER:
	    ಯದಿ (MB_ISLOWER(c) && c != 170 && c != 186)
		ಪ್ರತ್ಯಾಗಮ OK;
	    ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_PRINT:
	    ಯದಿ (vim_isprintc(c))
		ಪ್ರತ್ಯಾಗಮ OK;
	    ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_PUNCT:
	    ಯದಿ (c >= 1 && c < 128 && ispunct(c))
		ಪ್ರತ್ಯಾಗಮ OK;
	    ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_SPACE:
	    ಯದಿ ((c >= 9 && c <= 13) || (c == ' '))
		ಪ್ರತ್ಯಾಗಮ OK;
	    ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_UPPER:
	    ಯದಿ (MB_ISUPPER(c))
		ಪ್ರತ್ಯಾಗಮ OK;
	    ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_XDIGIT:
	    ಯದಿ (vim_isxdigit(c))
		ಪ್ರತ್ಯಾಗಮ OK;
	    ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_TAB:
	    ಯದಿ (c == '\t')
		ಪ್ರತ್ಯಾಗಮ OK;
	    ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_RETURN:
	    ಯದಿ (c == '\r')
		ಪ್ರತ್ಯಾಗಮ OK;
	    ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_BACKSPACE:
	    ಯದಿ (c == '\b')
		ಪ್ರತ್ಯಾಗಮ OK;
	    ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_ESCAPE:
	    ಯದಿ (c == '\033')
		ಪ್ರತ್ಯಾಗಮ OK;
	    ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_IDENT:
	    ಯದಿ (vim_isIDc(c))
		ಪ್ರತ್ಯಾಗಮ OK;
	    ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_KEYWORD:
	    ಯದಿ (reg_iswordc(c))
		ಪ್ರತ್ಯಾಗಮ OK;
	    ಭಙ್ಗ;
	ಪಕ್ಷ NFA_CLASS_FNAME:
	    ಯದಿ (vim_isfilec(c))
		ಪ್ರತ್ಯಾಗಮ OK;
	    ಭಙ್ಗ;

	ಸ್ವಭಾವ:
	    // should not be here :P
	    siemsg(_(e_nfa_regexp_invalid_character_class_nr), class);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Check for a match with subexpression "subidx".
 * Return TRUE if it matches.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
match_backref(
    regsub_T	*sub,	    // pointers to subexpressions
    ಪೂರ್ಣಾಙ್ಕ		subidx,
    ಪೂರ್ಣಾಙ್ಕ		*bytelen)   // out: length of match in bytes
{
    ಪೂರ್ಣಾಙ್ಕ		len;

    ಯದಿ (sub->in_use <= subidx)
    {
retempty:
	// backref was not set, match an empty string
	*bytelen = 0;
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }

    ಯದಿ (REG_MULTI)
    {
	ಯದಿ (sub->list.multi[subidx].start_lnum < 0
				       || sub->list.multi[subidx].end_lnum < 0)
	    ಗಚ್ಛ retempty;
	ಯದಿ (sub->list.multi[subidx].start_lnum == rex.lnum
			       && sub->list.multi[subidx].end_lnum == rex.lnum)
	{
	    len = sub->list.multi[subidx].end_col
					  - sub->list.multi[subidx].start_col;
	    ಯದಿ (cstrncmp(rex.ಸಾಲು + sub->list.multi[subidx].start_col,
							 rex.ಒಳಹರಿವು, &len) == 0)
	    {
		*bytelen = len;
		ಪ್ರತ್ಯಾಗಮ TRUE;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (match_with_backref(
			sub->list.multi[subidx].start_lnum,
			sub->list.multi[subidx].start_col,
			sub->list.multi[subidx].end_lnum,
			sub->list.multi[subidx].end_col,
			bytelen) == RA_MATCH)
		ಪ್ರತ್ಯಾಗಮ TRUE;
	}
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (sub->list.ಸಾಲು[subidx].start == ರಿಕ್ತ
					|| sub->list.ಸಾಲು[subidx].end == ರಿಕ್ತ)
	    ಗಚ್ಛ retempty;
	len = (ಪೂರ್ಣಾಙ್ಕ)(sub->list.ಸಾಲು[subidx].end - sub->list.ಸಾಲು[subidx].start);
	ಯದಿ (cstrncmp(sub->list.ಸಾಲು[subidx].start, rex.ಒಳಹರಿವು, &len) == 0)
	{
	    *bytelen = len;
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	}
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

#ifdef FEAT_SYN_HL

/*
 * Check for a match with \z subexpression "subidx".
 * Return TRUE if it matches.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
match_zref(
    ಪೂರ್ಣಾಙ್ಕ		subidx,
    ಪೂರ್ಣಾಙ್ಕ		*bytelen)   // out: length of match in bytes
{
    ಪೂರ್ಣಾಙ್ಕ		len;

    cleanup_zsubexpr();
    ಯದಿ (re_extmatch_in == ರಿಕ್ತ || re_extmatch_in->matches[subidx] == ರಿಕ್ತ)
    {
	// backref was not set, match an empty string
	*bytelen = 0;
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }

    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(re_extmatch_in->matches[subidx]);
    ಯದಿ (cstrncmp(re_extmatch_in->matches[subidx], rex.ಒಳಹರಿವು, &len) == 0)
    {
	*bytelen = len;
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}
#endif

/*
 * Save list IDs for all NFA states of "prog" into "list".
 * Also reset the IDs to zero.
 * Only used for the recursive value lastlist[1].
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
nfa_save_listids(nfa_regprog_T *prog, ಪೂರ್ಣಾಙ್ಕ *list)
{
    ಪೂರ್ಣಾಙ್ಕ		    i;
    nfa_state_T	    *p;

    // Order in the list is reverse, it's a bit faster that way.
    p = &prog->state[0];
    ಪ್ರತಿ (i = prog->nstate; --i >= 0; )
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
nfa_restore_listids(nfa_regprog_T *prog, ಪೂರ್ಣಾಙ್ಕ *list)
{
    ಪೂರ್ಣಾಙ್ಕ		    i;
    nfa_state_T	    *p;

    p = &prog->state[0];
    ಪ್ರತಿ (i = prog->nstate; --i >= 0; )
    {
	p->lastlist[1] = list[i];
	++p;
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
nfa_re_ಅಂಕೆ_cmp(long_u val, ಪೂರ್ಣಾಙ್ಕ op, long_u pos)
{
    ಯದಿ (op == 1) ಪ್ರತ್ಯಾಗಮ pos > val;
    ಯದಿ (op == 2) ಪ್ರತ್ಯಾಗಮ pos < val;
    ಪ್ರತ್ಯಾಗಮ val == pos;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ nfa_regmatch(nfa_regprog_T *prog, nfa_state_T *start, regsubs_T *submatch, regsubs_T *m);

/*
 * Recursively call nfa_regmatch()
 * "pim" is NULL or contains info about a Postponed Invisible Match (start
 * position).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
recursive_regmatch(
    nfa_state_T	    *state,
    nfa_pim_T	    *pim,
    nfa_regprog_T   *prog,
    regsubs_T	    *submatch,
    regsubs_T	    *m,
    ಪೂರ್ಣಾಙ್ಕ		    **listids,
    ಪೂರ್ಣಾಙ್ಕ		    *listids_len)
{
    ಪೂರ್ಣಾಙ್ಕ		save_reginput_col = (ಪೂರ್ಣಾಙ್ಕ)(rex.ಒಳಹರಿವು - rex.ಸಾಲು);
    ಪೂರ್ಣಾಙ್ಕ		save_reglnum = rex.lnum;
    ಪೂರ್ಣಾಙ್ಕ		save_nfa_match = nfa_match;
    ಪೂರ್ಣಾಙ್ಕ		save_nfa_listid = rex.nfa_listid;
    save_se_T   *save_nfa_endp = nfa_endp;
    save_se_T   endpos;
    save_se_T   *endposp = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		result;
    ಪೂರ್ಣಾಙ್ಕ		need_restore = FALSE;

    ಯದಿ (pim != ರಿಕ್ತ)
    {
	// start at the position where the postponed match was
	ಯದಿ (REG_MULTI)
	    rex.ಒಳಹರಿವು = rex.ಸಾಲು + pim->end.pos.col;
	ಅನ್ಯಥಾ
	    rex.ಒಳಹರಿವು = pim->end.ptr;
    }

    ಯದಿ (state->c == NFA_START_INVISIBLE_BEFORE
	    || state->c == NFA_START_INVISIBLE_BEFORE_FIRST
	    || state->c == NFA_START_INVISIBLE_BEFORE_NEG
	    || state->c == NFA_START_INVISIBLE_BEFORE_NEG_FIRST)
    {
	// The recursive match must end at the current position. When "pim" is
	// not NULL it specifies the current position.
	endposp = &endpos;
	ಯದಿ (REG_MULTI)
	{
	    ಯದಿ (pim == ರಿಕ್ತ)
	    {
		endpos.se_u.pos.col = (ಪೂರ್ಣಾಙ್ಕ)(rex.ಒಳಹರಿವು - rex.ಸಾಲು);
		endpos.se_u.pos.lnum = rex.lnum;
	    }
	    ಅನ್ಯಥಾ
		endpos.se_u.pos = pim->end.pos;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (pim == ರಿಕ್ತ)
		endpos.se_u.ptr = rex.ಒಳಹರಿವು;
	    ಅನ್ಯಥಾ
		endpos.se_u.ptr = pim->end.ptr;
	}

	// Go back the specified number of bytes, or as far as the
	// start of the previous line, to try matching "\@<=" or
	// not matching "\@<!". This is very inefficient, limit the number of
	// bytes if possible.
	ಯದಿ (state->val <= 0)
	{
	    ಯದಿ (REG_MULTI)
	    {
		rex.ಸಾಲು = reg_getline(--rex.lnum);
		ಯದಿ (rex.ಸಾಲು == ರಿಕ್ತ)
		    // can't go before the first line
		    rex.ಸಾಲು = reg_getline(++rex.lnum);
	    }
	    rex.ಒಳಹರಿವು = rex.ಸಾಲು;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (REG_MULTI && (ಪೂರ್ಣಾಙ್ಕ)(rex.ಒಳಹರಿವು - rex.ಸಾಲು) < state->val)
	    {
		// Not enough bytes in this line, go to end of
		// previous line.
		rex.ಸಾಲು = reg_getline(--rex.lnum);
		ಯದಿ (rex.ಸಾಲು == ರಿಕ್ತ)
		{
		    // can't go before the first line
		    rex.ಸಾಲು = reg_getline(++rex.lnum);
		    rex.ಒಳಹರಿವು = rex.ಸಾಲು;
		}
		ಅನ್ಯಥಾ
		    rex.ಒಳಹರಿವು = rex.ಸಾಲು + reg_getline_len(rex.lnum);
	    }
	    ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ)(rex.ಒಳಹರಿವು - rex.ಸಾಲು) >= state->val)
	    {
		rex.ಒಳಹರಿವು -= state->val;
		ಯದಿ (has_mbyte)
		    rex.ಒಳಹರಿವು -= mb_head_off(rex.ಸಾಲು, rex.ಒಳಹರಿವು);
	    }
	    ಅನ್ಯಥಾ
		rex.ಒಳಹರಿವು = rex.ಸಾಲು;
	}
    }

#ifdef ENABLE_LOG
    ಯದಿ (log_fd != stderr)
	fclose(log_fd);
    log_fd = ರಿಕ್ತ;
#endif
    // Have to clear the lastlist field of the NFA nodes, so that
    // nfa_regmatch() and addstate() can run properly after recursion.
    ಯದಿ (nfa_ll_index == 1)
    {
	// Already calling nfa_regmatch() recursively.  Save the lastlist[1]
	// values and clear them.
	ಯದಿ (*listids == ರಿಕ್ತ || *listids_len < prog->nstate)
	{
	    vim_free(*listids);
	    *listids = ALLOC_MULT(ಪೂರ್ಣಾಙ್ಕ, prog->nstate);
	    ಯದಿ (*listids == ರಿಕ್ತ)
	    {
		emsg(_(e_nfa_regexp_could_not_allocate_memory_for_branch_traversal));
		ಪ್ರತ್ಯಾಗಮ 0;
	    }
	    *listids_len = prog->nstate;
	}
	nfa_save_listids(prog, *listids);
	need_restore = TRUE;
	// any value of rex.nfa_listid will do
    }
    ಅನ್ಯಥಾ
    {
	// First recursive nfa_regmatch() call, switch to the second lastlist
	// entry.  Make sure rex.nfa_listid is different from a previous
	// recursive call, because some states may still have this ID.
	++nfa_ll_index;
	ಯದಿ (rex.nfa_listid <= rex.nfa_alt_listid)
	    rex.nfa_listid = rex.nfa_alt_listid;
    }

    // Call nfa_regmatch() to check if the current concat matches at this
    // position. The concat ends with the node NFA_END_INVISIBLE
    nfa_endp = endposp;
    result = nfa_regmatch(prog, state->out, submatch, m);

    ಯದಿ (need_restore)
	nfa_restore_listids(prog, *listids);
    ಅನ್ಯಥಾ
    {
	--nfa_ll_index;
	rex.nfa_alt_listid = rex.nfa_listid;
    }

    // restore position in input text
    rex.lnum = save_reglnum;
    ಯದಿ (REG_MULTI)
	rex.ಸಾಲು = reg_getline(rex.lnum);
    rex.ಒಳಹರಿವು = rex.ಸಾಲು + save_reginput_col;
    ಯದಿ (result != NFA_TOO_EXPENSIVE)
    {
	nfa_match = save_nfa_match;
	rex.nfa_listid = save_nfa_listid;
    }
    nfa_endp = save_nfa_endp;

#ifdef ENABLE_LOG
    open_debug_log(result);
#endif

    ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Estimate the chance of a match with "state" failing.
 * empty match: 0
 * NFA_ANY: 1
 * specific character: 99
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
failure_chance(nfa_state_T *state, ಪೂರ್ಣಾಙ್ಕ depth)
{
    ಪೂರ್ಣಾಙ್ಕ c = state->c;
    ಪೂರ್ಣಾಙ್ಕ l, r;

    // detect looping
    ಯದಿ (depth > 4)
	ಪ್ರತ್ಯಾಗಮ 1;

    ವಿಕಲ್ಪ (c)
    {
	ಪಕ್ಷ NFA_SPLIT:
	    ಯದಿ (state->out->c == NFA_SPLIT || state->out1->c == NFA_SPLIT)
		// avoid recursive stuff
		ಪ್ರತ್ಯಾಗಮ 1;
	    // two alternatives, use the lowest failure chance
	    l = failure_chance(state->out, depth + 1);
	    r = failure_chance(state->out1, depth + 1);
	    ಪ್ರತ್ಯಾಗಮ l < r ? l : r;

	ಪಕ್ಷ NFA_ANY:
	    // matches anything, unlikely to fail
	    ಪ್ರತ್ಯಾಗಮ 1;

	ಪಕ್ಷ NFA_MATCH:
	ಪಕ್ಷ NFA_MCLOSE:
	ಪಕ್ಷ NFA_ANY_COMPOSING:
	    // empty match works always
	    ಪ್ರತ್ಯಾಗಮ 0;

	ಪಕ್ಷ NFA_START_INVISIBLE:
	ಪಕ್ಷ NFA_START_INVISIBLE_FIRST:
	ಪಕ್ಷ NFA_START_INVISIBLE_NEG:
	ಪಕ್ಷ NFA_START_INVISIBLE_NEG_FIRST:
	ಪಕ್ಷ NFA_START_INVISIBLE_BEFORE:
	ಪಕ್ಷ NFA_START_INVISIBLE_BEFORE_FIRST:
	ಪಕ್ಷ NFA_START_INVISIBLE_BEFORE_NEG:
	ಪಕ್ಷ NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
	ಪಕ್ಷ NFA_START_PATTERN:
	    // recursive regmatch is expensive, use low failure chance
	    ಪ್ರತ್ಯಾಗಮ 5;

	ಪಕ್ಷ NFA_BOL:
	ಪಕ್ಷ NFA_EOL:
	ಪಕ್ಷ NFA_BOF:
	ಪಕ್ಷ NFA_EOF:
	ಪಕ್ಷ NFA_NEWL:
	    ಪ್ರತ್ಯಾಗಮ 99;

	ಪಕ್ಷ NFA_BOW:
	ಪಕ್ಷ NFA_EOW:
	    ಪ್ರತ್ಯಾಗಮ 90;

	ಪಕ್ಷ NFA_MOPEN:
	ಪಕ್ಷ NFA_MOPEN1:
	ಪಕ್ಷ NFA_MOPEN2:
	ಪಕ್ಷ NFA_MOPEN3:
	ಪಕ್ಷ NFA_MOPEN4:
	ಪಕ್ಷ NFA_MOPEN5:
	ಪಕ್ಷ NFA_MOPEN6:
	ಪಕ್ಷ NFA_MOPEN7:
	ಪಕ್ಷ NFA_MOPEN8:
	ಪಕ್ಷ NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	ಪಕ್ಷ NFA_ZOPEN:
	ಪಕ್ಷ NFA_ZOPEN1:
	ಪಕ್ಷ NFA_ZOPEN2:
	ಪಕ್ಷ NFA_ZOPEN3:
	ಪಕ್ಷ NFA_ZOPEN4:
	ಪಕ್ಷ NFA_ZOPEN5:
	ಪಕ್ಷ NFA_ZOPEN6:
	ಪಕ್ಷ NFA_ZOPEN7:
	ಪಕ್ಷ NFA_ZOPEN8:
	ಪಕ್ಷ NFA_ZOPEN9:
	ಪಕ್ಷ NFA_ZCLOSE:
	ಪಕ್ಷ NFA_ZCLOSE1:
	ಪಕ್ಷ NFA_ZCLOSE2:
	ಪಕ್ಷ NFA_ZCLOSE3:
	ಪಕ್ಷ NFA_ZCLOSE4:
	ಪಕ್ಷ NFA_ZCLOSE5:
	ಪಕ್ಷ NFA_ZCLOSE6:
	ಪಕ್ಷ NFA_ZCLOSE7:
	ಪಕ್ಷ NFA_ZCLOSE8:
	ಪಕ್ಷ NFA_ZCLOSE9:
#endif
	ಪಕ್ಷ NFA_NOPEN:
	ಪಕ್ಷ NFA_MCLOSE1:
	ಪಕ್ಷ NFA_MCLOSE2:
	ಪಕ್ಷ NFA_MCLOSE3:
	ಪಕ್ಷ NFA_MCLOSE4:
	ಪಕ್ಷ NFA_MCLOSE5:
	ಪಕ್ಷ NFA_MCLOSE6:
	ಪಕ್ಷ NFA_MCLOSE7:
	ಪಕ್ಷ NFA_MCLOSE8:
	ಪಕ್ಷ NFA_MCLOSE9:
	ಪಕ್ಷ NFA_NCLOSE:
	    ಪ್ರತ್ಯಾಗಮ failure_chance(state->out, depth + 1);

	ಪಕ್ಷ NFA_BACKREF1:
	ಪಕ್ಷ NFA_BACKREF2:
	ಪಕ್ಷ NFA_BACKREF3:
	ಪಕ್ಷ NFA_BACKREF4:
	ಪಕ್ಷ NFA_BACKREF5:
	ಪಕ್ಷ NFA_BACKREF6:
	ಪಕ್ಷ NFA_BACKREF7:
	ಪಕ್ಷ NFA_BACKREF8:
	ಪಕ್ಷ NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	ಪಕ್ಷ NFA_ZREF1:
	ಪಕ್ಷ NFA_ZREF2:
	ಪಕ್ಷ NFA_ZREF3:
	ಪಕ್ಷ NFA_ZREF4:
	ಪಕ್ಷ NFA_ZREF5:
	ಪಕ್ಷ NFA_ZREF6:
	ಪಕ್ಷ NFA_ZREF7:
	ಪಕ್ಷ NFA_ZREF8:
	ಪಕ್ಷ NFA_ZREF9:
#endif
	    // backreferences don't match in many places
	    ಪ್ರತ್ಯಾಗಮ 94;

	ಪಕ್ಷ NFA_LNUM_GT:
	ಪಕ್ಷ NFA_LNUM_LT:
	ಪಕ್ಷ NFA_COL_GT:
	ಪಕ್ಷ NFA_COL_LT:
	ಪಕ್ಷ NFA_VCOL_GT:
	ಪಕ್ಷ NFA_VCOL_LT:
	ಪಕ್ಷ NFA_MARK_GT:
	ಪಕ್ಷ NFA_MARK_LT:
	ಪಕ್ಷ NFA_VISUAL:
	    // before/after positions don't match very often
	    ಪ್ರತ್ಯಾಗಮ 85;

	ಪಕ್ಷ NFA_LNUM:
	    ಪ್ರತ್ಯಾಗಮ 90;

	ಪಕ್ಷ NFA_CURSOR:
	ಪಕ್ಷ NFA_COL:
	ಪಕ್ಷ NFA_VCOL:
	ಪಕ್ಷ NFA_MARK:
	    // specific positions rarely match
	    ಪ್ರತ್ಯಾಗಮ 98;

	ಪಕ್ಷ NFA_COMPOSING:
	    ಪ್ರತ್ಯಾಗಮ 95;

	ಸ್ವಭಾವ:
	    ಯದಿ (c > 0)
		// character match fails often
		ಪ್ರತ್ಯಾಗಮ 95;
    }

    // something else, includes character classes
    ಪ್ರತ್ಯಾಗಮ 50;
}

/*
 * Skip until the char "c" we know a match must start with.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
skip_to_start(ಪೂರ್ಣಾಙ್ಕ c, colnr_T *colp)
{
    char_u *s;

    // Used often, do some work to avoid call overhead.
    ಯದಿ (!rex.reg_ic && !has_mbyte)
	s = vim_strbyte(rex.ಸಾಲು + *colp, c);
    ಅನ್ಯಥಾ
	s = cstrchr(rex.ಸಾಲು + *colp, c);
    ಯದಿ (s == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    *colp = (ಪೂರ್ಣಾಙ್ಕ)(s - rex.ಸಾಲು);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check for a match with match_text.
 * Called after skip_to_start() has found regstart.
 * Returns zero for no match, 1 for a match.
 */
    ಸ್ಥಿರ long
find_match_text(colnr_T *startcol, ಪೂರ್ಣಾಙ್ಕ regstart, char_u *match_text)
{
    colnr_T col = *startcol;
    ಪೂರ್ಣಾಙ್ಕ	    c1, c2;
    ಪೂರ್ಣಾಙ್ಕ	    len1, len2;
    ಪೂರ್ಣಾಙ್ಕ	    match;

    ಪ್ರತಿ (;;)
    {
	match = TRUE;
	// Skip the input character: case-folding can change its byte length.
	len2 = MB_CPTR2LEN(rex.ಸಾಲು + col);
	ಪ್ರತಿ (len1 = 0; match_text[len1] != NUL; len1 += MB_CHAR2LEN(c1))
	{
	    c1 = PTR2CHAR(match_text + len1);
	    c2 = PTR2CHAR(rex.ಸಾಲು + col + len2);
	    ಯದಿ (c1 != c2 && (!rex.reg_ic || MB_CASEFOLD(c1) != MB_CASEFOLD(c2)))
	    {
		match = FALSE;
		ಭಙ್ಗ;
	    }
	    len2 += enc_utf8 ? utf_ptr2len(rex.ಸಾಲು + col + len2)
							     : MB_CHAR2LEN(c2);
	}
	ಯದಿ (match
		// check that no composing char follows
		&& !(enc_utf8
			  && utf_iscomposing(PTR2CHAR(rex.ಸಾಲು + col + len2))))
	{
	    cleanup_subexpr();
	    ಯದಿ (REG_MULTI)
	    {
		rex.reg_startpos[0].lnum = rex.lnum;
		rex.reg_startpos[0].col = col;
		rex.reg_endpos[0].lnum = rex.lnum;
		rex.reg_endpos[0].col = col + len2;
	    }
	    ಅನ್ಯಥಾ
	    {
		rex.reg_startp[0] = rex.ಸಾಲು + col;
		rex.reg_endp[0] = rex.ಸಾಲು + col + len2;
	    }
	    *startcol = col;
	    ಪ್ರತ್ಯಾಗಮ 1L;
	}

	// Try finding regstart after the current match.
	col += MB_CPTR2LEN(rex.ಸಾಲು + col); // skip the input character
	ಯದಿ (skip_to_start(regstart, &col) == FAIL)
	    ಭಙ್ಗ;
    }

    *startcol = col;
    ಪ್ರತ್ಯಾಗಮ 0L;
}

/*
 * Check whether the composing characters at the current input position match
 * the NFA composing sub-expression that starts at "sta" (the first state
 * after NFA_COMPOSING).  "curc" is the base character and "clen" its byte
 * length.
 * Returns OK when it matches, FAIL otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
match_composing(nfa_state_T *sta, ಪೂರ್ಣಾಙ್ಕ curc, ಪೂರ್ಣಾಙ್ಕ clen)
{
    ಪೂರ್ಣಾಙ್ಕ		mc = curc;
    ಪೂರ್ಣಾಙ್ಕ		len = 0;
    ಪೂರ್ಣಾಙ್ಕ		cchars[MAX_MCO];
    ಪೂರ್ಣಾಙ್ಕ		ccount = 0;
    ಪೂರ್ಣಾಙ್ಕ		j;

    ಯದಿ (utf_iscomposing(sta->c))
	// Only match composing character(s), ignore base character.  Used
	// for ".{composing}" and "{composing}" (no preceding character).
	len += mb_char2len(mc);

    ಯದಿ (rex.reg_icombine && len == 0)
	// If \Z was present, then ignore composing characters.  When
	// ignoring the base character this always matches.
	ಪ್ರತ್ಯಾಗಮ sta->c == curc ? OK : FAIL;

    // Check base character matches first, unless ignored.
    ಯದಿ (len == 0 && mc != sta->c)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (len == 0)
    {
	len += mb_char2len(mc);
	sta = sta->out;
    }

    // We don't care about the order of composing characters.  Get them into
    // cchars[] first.
    ಯಾವತ್ (len < clen)
    {
	mc = mb_ptr2char(rex.ಒಳಹರಿವು + len);
	cchars[ccount++] = mc;
	len += mb_char2len(mc);
	ಯದಿ (ccount == MAX_MCO)
	    ಭಙ್ಗ;
    }

    // Check that each composing char in the pattern matches a composing char
    // in the text.  We do not check if all composing chars are matched.
    ಯಾವತ್ (sta->c != NFA_END_COMPOSING)
    {
	ಪ್ರತಿ (j = 0; j < ccount; ++j)
	    ಯದಿ (cchars[j] == sta->c)
		ಭಙ್ಗ;
	ಯದಿ (j == ccount)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	sta = sta->out;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
nfa_regmatch(
    nfa_regprog_T	*prog,
    nfa_state_T		*start,
    regsubs_T		*submatch,
    regsubs_T		*m)
{
    ಪೂರ್ಣಾಙ್ಕ		result = FALSE;
    size_t	size = 0;
    ಪೂರ್ಣಾಙ್ಕ		flag = 0;
    ಪೂರ್ಣಾಙ್ಕ		go_to_nextline = FALSE;
    nfa_thread_T *t;
    nfa_list_T	list[2];
    ಪೂರ್ಣಾಙ್ಕ		listidx;
    nfa_list_T	*thislist;
    nfa_list_T	*nextlist;
    ಪೂರ್ಣಾಙ್ಕ		*listids = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		listids_len = 0;
    nfa_state_T *add_state;
    ಪೂರ್ಣಾಙ್ಕ		add_here;
    ಪೂರ್ಣಾಙ್ಕ		add_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		add_off = 0;
    ಪೂರ್ಣಾಙ್ಕ		toplevel = start->c == NFA_MOPEN;
    regsubs_T	*r;
#ifdef NFA_REGEXP_DEBUG_LOG
    FILE	*debug;
#endif

    // Some patterns may take a long time to match, especially when using
    // recursive_regmatch(). Allow interrupting them with CTRL-C.
    fast_breakcheck();
    ಯದಿ (got_int)
	ಪ್ರತ್ಯಾಗಮ FALSE;
#ifdef FEAT_RELTIME
    ಯದಿ (nfa_did_time_out())
	ಪ್ರತ್ಯಾಗಮ FALSE;
#endif

#ifdef NFA_REGEXP_DEBUG_LOG
    debug = fopen(NFA_REGEXP_DEBUG_LOG, "a");
    ಯದಿ (debug == ರಿಕ್ತ)
    {
	semsg("(NFA) COULD NOT OPEN %s!", NFA_REGEXP_DEBUG_LOG);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
#endif
    nfa_match = FALSE;

    // Allocate memory for the lists of nodes.
    size = (prog->nstate + 1) * ಪರಿಮಾಣ(nfa_thread_T);

    // Reuse cached list buffers from prog when available (top-level call).
    // Recursive calls must allocate their own buffers.
    ಯದಿ (toplevel && prog->listbuf[0] != ರಿಕ್ತ)
    {
	list[0].t = (nfa_thread_T *)prog->listbuf[0];
	list[1].t = (nfa_thread_T *)prog->listbuf[1];
	prog->listbuf[0] = ರಿಕ್ತ;
	prog->listbuf[1] = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ
    {
	list[0].t = alloc(size);
	list[1].t = alloc(size);
    }
    list[0].len = prog->nstate + 1;
    list[1].len = prog->nstate + 1;
    ಯದಿ (list[0].t == ರಿಕ್ತ || list[1].t == ರಿಕ್ತ)
	ಗಚ್ಛ theend;

#ifdef ENABLE_LOG
    log_fd = fopen(NFA_REGEXP_RUN_LOG, "a");
    ಯದಿ (log_fd == ರಿಕ್ತ)
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
    ಯದಿ (toplevel)
    {
	ಯದಿ (REG_MULTI)
	{
	    m->norm.list.multi[0].start_lnum = rex.lnum;
	    m->norm.list.multi[0].start_col = (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು);
	    m->norm.orig_start_col = m->norm.list.multi[0].start_col;
	}
	ಅನ್ಯಥಾ
	    m->norm.list.ಸಾಲು[0].start = rex.ಒಳಹರಿವು;
	m->norm.in_use = 1;
	r = addstate(thislist, start->out, m, ರಿಕ್ತ, 0, 0);
    }
    ಅನ್ಯಥಾ
	r = addstate(thislist, start, m, ರಿಕ್ತ, 0, 0);
    ಯದಿ (r == ರಿಕ್ತ)
    {
	nfa_match = NFA_TOO_EXPENSIVE;
	ಗಚ್ಛ theend;
    }

#define	ADD_STATE_IF_MATCH(state)	\
    ಯದಿ (result)				\
    {					\
	add_state = state->out;		\
	add_off = clen;			\
    }

    /*
     * Run for each character.
     */
    ಪ್ರತಿ (;;)
    {
	ಪೂರ್ಣಾಙ್ಕ	curc;
	ಪೂರ್ಣಾಙ್ಕ	clen;

	ಯದಿ (has_mbyte)
	{
	    // Fast path for an ASCII byte not followed by a composing
	    // character, matching the check in utfc_ptr2len().  Avoids two
	    // indirect calls for the common case.
	    ಯದಿ (rex.ಒಳಹರಿವು[0] != NUL && rex.ಒಳಹರಿವು[0] < 0x80 && rex.ಒಳಹರಿವು[1] < 0x80)
	    {
		curc = rex.ಒಳಹರಿವು[0];
		clen = 1;
	    }
	    ಅನ್ಯಥಾ
	    {
		curc = (*mb_ptr2char)(rex.ಒಳಹರಿವು);
		clen = (*mb_ptr2len)(rex.ಒಳಹರಿವು);
	    }
	}
	ಅನ್ಯಥಾ
	{
	    curc = *rex.ಒಳಹರಿವು;
	    clen = 1;
	}
	ಯದಿ (curc == NUL)
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
	ಯದಿ (prog->re_engine == AUTOMATIC_ENGINE
		&& (rex.nfa_listid >= NFA_MAX_STATES
#ifdef FEAT_EVAL
		    || nfa_fail_for_testing
#endif
		    ))
	{
	    // too many states, retry with old engine
	    nfa_match = NFA_TOO_EXPENSIVE;
	    ಗಚ್ಛ theend;
	}

	thislist->id = rex.nfa_listid;
	nextlist->id = rex.nfa_listid + 1;

#ifdef ENABLE_LOG
	fprintf(log_fd, "------------------------------------------\n");
	fprintf(log_fd, ">>> Reginput is \"%s\"\n", rex.ಒಳಹರಿವು);
	fprintf(log_fd, ">>> Advanced one character... Current char is %c (code %d) \n", curc, (ಪೂರ್ಣಾಙ್ಕ)curc);
	fprintf(log_fd, ">>> Thislist has %d states available: ", thislist->n);
	{
	    ಪೂರ್ಣಾಙ್ಕ i;

	    ಪ್ರತಿ (i = 0; i < thislist->n; i++)
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
	ಯದಿ (thislist->n == 0)
	    ಭಙ್ಗ;

	// compute nextlist
	ಪ್ರತಿ (listidx = 0; listidx < thislist->n; ++listidx)
	{
	    // If the list gets very long there probably is something wrong.
	    // At least allow interrupting with CTRL-C.  Only check once in a
	    // while, calling ui_breakcheck() for every state is too slow.
	    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	breakcheck_ಎಣಿಕೆ = 0;  // using "static" makes it faster
	    ಯದಿ (unlikely(++breakcheck_ಎಣಿಕೆ >= 100000))
	    {
		ui_breakcheck();
		breakcheck_ಎಣಿಕೆ = 0;
	    }
	    ಯದಿ (got_int)
		ಭಙ್ಗ;
#ifdef FEAT_RELTIME
	    ಯದಿ (nfa_did_time_out())
		ಭಙ್ಗ;
#endif
	    t = &thislist->t[listidx];

#ifdef NFA_REGEXP_DEBUG_LOG
	    nfa_set_code(t->state->c);
	    fprintf(debug, "%s, ", code);
#endif
#ifdef ENABLE_LOG
	    {
		ಪೂರ್ಣಾಙ್ಕ col;

		ಯದಿ (t->subs.norm.in_use <= 0)
		    col = -1;
		ಅನ್ಯಥಾ ಯದಿ (REG_MULTI)
		    col = t->subs.norm.list.multi[0].start_col;
		ಅನ್ಯಥಾ
		    col = (ಪೂರ್ಣಾಙ್ಕ)(t->subs.norm.list.ಸಾಲು[0].start - rex.ಸಾಲು);
		nfa_set_code(t->state->c);
		fprintf(log_fd, "(%d) char %d %s (start col %d)%s... \n",
			abs(t->state->id), (ಪೂರ್ಣಾಙ್ಕ)t->state->c, code, col,
			pim_info(&t->pim));
	    }
#endif

	    /*
	     * Handle the possible codes of the current state.
	     * The most important is NFA_MATCH.
	     */
	    add_state = ರಿಕ್ತ;
	    add_here = FALSE;
	    add_ಎಣಿಕೆ = 0;
	    ವಿಕಲ್ಪ (t->state->c)
	    {
	    ಪಕ್ಷ NFA_MATCH:
	      {
		// If the match is not at the start of the line, ends before a
		// composing characters and rex.reg_icombine is not set, that
		// is not really a match.
		ಯದಿ (enc_utf8 && !rex.reg_icombine
			     && rex.ಒಳಹರಿವು != rex.ಸಾಲು && utf_iscomposing(curc))
		    ಭಙ್ಗ;

		nfa_match = TRUE;
		copy_sub(&submatch->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		ಯದಿ (rex.nfa_has_zsubexpr)
		    copy_sub(&submatch->synt, &t->subs.synt);
#endif
#ifdef ENABLE_LOG
		log_subsexpr(&t->subs);
#endif
		// Found the left-most longest match, do not look at any other
		// states at this position.  When the list of states is going
		// to be empty quit without advancing, so that "rex.input" is
		// correct.
		ಯದಿ (nextlist->n == 0)
		    clen = 0;
		ಗಚ್ಛ nextchar;
	      }

	    ಪಕ್ಷ NFA_END_INVISIBLE:
	    ಪಕ್ಷ NFA_END_INVISIBLE_NEG:
	    ಪಕ್ಷ NFA_END_PATTERN:
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
		ಯದಿ (nfa_endp != ರಿಕ್ತ)
		{
		    ಯದಿ (REG_MULTI)
			fprintf(log_fd, "Current lnum: %d, endp lnum: %d; current col: %d, endp col: %d\n",
				(ಪೂರ್ಣಾಙ್ಕ)rex.lnum,
				(ಪೂರ್ಣಾಙ್ಕ)nfa_endp->se_u.pos.lnum,
				(ಪೂರ್ಣಾಙ್ಕ)(rex.ಒಳಹರಿವು - rex.ಸಾಲು),
				nfa_endp->se_u.pos.col);
		    ಅನ್ಯಥಾ
			fprintf(log_fd, "Current col: %d, endp col: %d\n",
				(ಪೂರ್ಣಾಙ್ಕ)(rex.ಒಳಹರಿವು - rex.ಸಾಲು),
				(ಪೂರ್ಣಾಙ್ಕ)(nfa_endp->se_u.ptr - rex.ಒಳಹರಿವು));
		}
#endif
		// If "nfa_endp" is set it's only a match if it ends at
		// "nfa_endp"
		ಯದಿ (nfa_endp != ರಿಕ್ತ && (REG_MULTI
			? (rex.lnum != nfa_endp->se_u.pos.lnum
			    || (ಪೂರ್ಣಾಙ್ಕ)(rex.ಒಳಹರಿವು - rex.ಸಾಲು)
						!= nfa_endp->se_u.pos.col)
			: rex.ಒಳಹರಿವು != nfa_endp->se_u.ptr))
		    ಭಙ್ಗ;

		// do not set submatches for \@!
		ಯದಿ (t->state->c != NFA_END_INVISIBLE_NEG)
		{
		    copy_sub(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		    ಯದಿ (rex.nfa_has_zsubexpr)
			copy_sub(&m->synt, &t->subs.synt);
#endif
		}
#ifdef ENABLE_LOG
		fprintf(log_fd, "Match found:\n");
		log_subsexpr(m);
#endif
		nfa_match = TRUE;
		// See comment above at "goto nextchar".
		ಯದಿ (nextlist->n == 0)
		    clen = 0;
		ಗಚ್ಛ nextchar;

	    ಪಕ್ಷ NFA_START_INVISIBLE:
	    ಪಕ್ಷ NFA_START_INVISIBLE_FIRST:
	    ಪಕ್ಷ NFA_START_INVISIBLE_NEG:
	    ಪಕ್ಷ NFA_START_INVISIBLE_NEG_FIRST:
	    ಪಕ್ಷ NFA_START_INVISIBLE_BEFORE:
	    ಪಕ್ಷ NFA_START_INVISIBLE_BEFORE_FIRST:
	    ಪಕ್ಷ NFA_START_INVISIBLE_BEFORE_NEG:
	    ಪಕ್ಷ NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
		{
#ifdef ENABLE_LOG
		    fprintf(log_fd, "Failure chance invisible: %d, what follows: %d\n",
			    failure_chance(t->state->out, 0),
			    failure_chance(t->state->out1->out, 0));
#endif
		    // Do it directly if there already is a PIM or when
		    // nfa_postprocess() detected it will work better.
		    ಯದಿ (t->pim.result != NFA_PIM_UNUSED
			 || t->state->c == NFA_START_INVISIBLE_FIRST
			 || t->state->c == NFA_START_INVISIBLE_NEG_FIRST
			 || t->state->c == NFA_START_INVISIBLE_BEFORE_FIRST
			 || t->state->c == NFA_START_INVISIBLE_BEFORE_NEG_FIRST)
		    {
			ಪೂರ್ಣಾಙ್ಕ in_use = m->norm.in_use;

			// Copy submatch info for the recursive call, opposite
			// of what happens on success below.
			copy_sub_off(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
			ಯದಿ (rex.nfa_has_zsubexpr)
			    copy_sub_off(&m->synt, &t->subs.synt);
#endif

			/*
			 * First try matching the invisible match, then what
			 * follows.
			 */
			result = recursive_regmatch(t->state, ರಿಕ್ತ, prog,
					  submatch, m, &listids, &listids_len);
			ಯದಿ (result == NFA_TOO_EXPENSIVE)
			{
			    nfa_match = result;
			    ಗಚ್ಛ theend;
			}

			// for \@! and \@<! it is a match when the result is
			// FALSE
			ಯದಿ (result != (t->state->c == NFA_START_INVISIBLE_NEG
			       || t->state->c == NFA_START_INVISIBLE_NEG_FIRST
			       || t->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			       || t->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&t->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
			    ಯದಿ (rex.nfa_has_zsubexpr)
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
		    ಅನ್ಯಥಾ
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
			ಯದಿ (REG_MULTI)
			{
			    pim.end.pos.col = (ಪೂರ್ಣಾಙ್ಕ)(rex.ಒಳಹರಿವು - rex.ಸಾಲು);
			    pim.end.pos.lnum = rex.lnum;
			}
			ಅನ್ಯಥಾ
			    pim.end.ptr = rex.ಒಳಹರಿವು;

			// t->state->out1 is the corresponding END_INVISIBLE
			// node; Add its out to the current list (zero-width
			// match).
			ಯದಿ (addstate_here(thislist, t->state->out1->out,
					     &t->subs, &pim, &listidx) == ರಿಕ್ತ)
			{
			    nfa_match = NFA_TOO_EXPENSIVE;
			    ಗಚ್ಛ theend;
			}
		    }
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_START_PATTERN:
	      {
		nfa_state_T *skip = ರಿಕ್ತ;
#ifdef ENABLE_LOG
		ಪೂರ್ಣಾಙ್ಕ	    skip_lid = 0;
#endif

		// There is no point in trying to match the pattern if the
		// output state is not going to be added to the list.
		ಯದಿ (state_in_list(nextlist, t->state->out1->out, &t->subs))
		{
		    skip = t->state->out1->out;
#ifdef ENABLE_LOG
		    skip_lid = nextlist->id;
#endif
		}
		ಅನ್ಯಥಾ ಯದಿ (state_in_list(nextlist,
					  t->state->out1->out->out, &t->subs))
		{
		    skip = t->state->out1->out->out;
#ifdef ENABLE_LOG
		    skip_lid = nextlist->id;
#endif
		}
		ಅನ್ಯಥಾ ಯದಿ (state_in_list(thislist,
					  t->state->out1->out->out, &t->subs))
		{
		    skip = t->state->out1->out->out;
#ifdef ENABLE_LOG
		    skip_lid = thislist->id;
#endif
		}
		ಯದಿ (skip != ರಿಕ್ತ)
		{
#ifdef ENABLE_LOG
		    nfa_set_code(skip->c);
		    fprintf(log_fd, "> Not trying to match pattern, output state %d is already in list %d. char %d: %s\n",
			    abs(skip->id), skip_lid, skip->c, code);
#endif
		    ಭಙ್ಗ;
		}
		// Copy submatch info to the recursive call, opposite of what
		// happens afterwards.
		copy_sub_off(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		ಯದಿ (rex.nfa_has_zsubexpr)
		    copy_sub_off(&m->synt, &t->subs.synt);
#endif

		// First try matching the pattern.
		result = recursive_regmatch(t->state, ರಿಕ್ತ, prog,
					  submatch, m, &listids, &listids_len);
		ಯದಿ (result == NFA_TOO_EXPENSIVE)
		{
		    nfa_match = result;
		    ಗಚ್ಛ theend;
		}
		ಯದಿ (result)
		{
		    ಪೂರ್ಣಾಙ್ಕ bytelen;

#ifdef ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN matches:\n");
		    log_subsexpr(m);
#endif
		    // Copy submatch info from the recursive call
		    copy_sub_off(&t->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
		    ಯದಿ (rex.nfa_has_zsubexpr)
			copy_sub_off(&t->subs.synt, &m->synt);
#endif
		    // Now we need to skip over the matched text and then
		    // continue with what follows.
		    ಯದಿ (REG_MULTI)
			// TODO: multi-line match
			bytelen = m->norm.list.multi[0].end_col
						  - (ಪೂರ್ಣಾಙ್ಕ)(rex.ಒಳಹರಿವು - rex.ಸಾಲು);
		    ಅನ್ಯಥಾ
			bytelen = (ಪೂರ್ಣಾಙ್ಕ)(m->norm.list.ಸಾಲು[0].end - rex.ಒಳಹರಿವು);

#ifdef ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN length: %d\n", bytelen);
#endif
		    ಯದಿ (bytelen == 0)
		    {
			// empty match, output of corresponding
			// NFA_END_PATTERN/NFA_SKIP to be used at current
			// position
			add_here = TRUE;
			add_state = t->state->out1->out->out;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (bytelen <= clen)
		    {
			// match current character, output of corresponding
			// NFA_END_PATTERN to be used at next position.
			add_state = t->state->out1->out->out;
			add_off = clen;
		    }
		    ಅನ್ಯಥಾ
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			add_state = t->state->out1->out;
			add_off = bytelen;
			add_ಎಣಿಕೆ = bytelen - clen;
		    }
		}
		ಭಙ್ಗ;
	      }

	    ಪಕ್ಷ NFA_BOL:
		ಯದಿ (rex.ಒಳಹರಿವು == rex.ಸಾಲು)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_EOL:
		ಯದಿ (curc == NUL)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_BOW:
		result = TRUE;

		ಯದಿ (curc == NUL)
		    result = FALSE;
		ಅನ್ಯಥಾ ಯದಿ (has_mbyte)
		{
		    ಪೂರ್ಣಾಙ್ಕ this_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.ಒಳಹರಿವು, rex.reg_buf);
		    ಯದಿ (this_class <= 1)
			result = FALSE;
		    ಅನ್ಯಥಾ ಯದಿ (reg_prev_class() == this_class)
			result = FALSE;
		}
		ಅನ್ಯಥಾ ಯದಿ (!vim_iswordc_buf(curc, rex.reg_buf)
			   || (rex.ಒಳಹರಿವು > rex.ಸಾಲು
			       && vim_iswordc_buf(rex.ಒಳಹರಿವು[-1], rex.reg_buf)))
		    result = FALSE;
		ಯದಿ (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_EOW:
		result = TRUE;
		ಯದಿ (rex.ಒಳಹರಿವು == rex.ಸಾಲು)
		    result = FALSE;
		ಅನ್ಯಥಾ ಯದಿ (has_mbyte)
		{
		    ಪೂರ್ಣಾಙ್ಕ this_class, prev_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.ಒಳಹರಿವು, rex.reg_buf);
		    prev_class = reg_prev_class();
		    ಯದಿ (this_class == prev_class
					|| prev_class == 0 || prev_class == 1)
			result = FALSE;
		}
		ಅನ್ಯಥಾ ಯದಿ (!vim_iswordc_buf(rex.ಒಳಹರಿವು[-1], rex.reg_buf)
			|| (rex.ಒಳಹರಿವು[0] != NUL
					&& vim_iswordc_buf(curc, rex.reg_buf)))
		    result = FALSE;
		ಯದಿ (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_BOF:
		ಯದಿ (rex.lnum == 0 && rex.ಒಳಹರಿವು == rex.ಸಾಲು
				     && (!REG_MULTI || rex.reg_firstlnum == 1))
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_EOF:
		ಯದಿ (rex.lnum == rex.reg_maxline && curc == NUL)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_COMPOSING:
	    {
		nfa_state_T *end;

		result = match_composing(t->state->out, curc, clen);

		end = t->state->out1;	    // NFA_END_COMPOSING
		ADD_STATE_IF_MATCH(end);
		ಭಙ್ಗ;
	    }

	    ಪಕ್ಷ NFA_NEWL:
		ಯದಿ (curc == NUL && !rex.reg_ಸಾಲು_lbr && REG_MULTI
						&& rex.lnum <= rex.reg_maxline)
		{
		    go_to_nextline = TRUE;
		    // Pass -1 for the offset, which means taking the position
		    // at the start of the next line.
		    add_state = t->state->out;
		    add_off = -1;
		}
		ಅನ್ಯಥಾ ಯದಿ (curc == '\n' && rex.reg_ಸಾಲು_lbr)
		{
		    // match \n as if it is an ordinary character
		    add_state = t->state->out;
		    add_off = 1;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_START_COLL:
	    ಪಕ್ಷ NFA_START_NEG_COLL:
	      {
		// What follows is a list of characters, until NFA_END_COLL.
		// One of them must match or none of them must match.
		nfa_state_T	*state;
		ಪೂರ್ಣಾಙ್ಕ		result_if_matched;
		ಪೂರ್ಣಾಙ್ಕ		c1, c2;

		// Never match EOL. If it's part of the collection it is added
		// as a separate state with an OR.
		ಯದಿ (curc == NUL)
		    ಭಙ್ಗ;

		state = t->state->out;
		result_if_matched = (t->state->c == NFA_START_COLL);
		ಪ್ರತಿ (;;)
		{
		    ಯದಿ (state->c == NFA_COMPOSING)
		    {
			nfa_state_T *end;

			result = match_composing(t->state->out->out, curc,
									clen);

			ಯದಿ (t->state->out->out1 != ರಿಕ್ತ
				&& t->state->out->out1->c == NFA_END_COMPOSING)
			{
			    end = t->state->out->out1;
			    ADD_STATE_IF_MATCH(end);
			}
			ಭಙ್ಗ;
		    }
		    ಯದಿ (state->c == NFA_END_COLL)
		    {
			result = !result_if_matched;
			ಭಙ್ಗ;
		    }
		    ಯದಿ (state->c == NFA_RANGE_MIN)
		    {
			c1 = state->val;
			state = state->out; // advance to NFA_RANGE_MAX
			c2 = state->val;
#ifdef ENABLE_LOG
			fprintf(log_fd, "NFA_RANGE_MIN curc=%d c1=%d c2=%d\n",
				curc, c1, c2);
#endif
			ಯದಿ (curc >= c1 && curc <= c2)
			{
			    result = result_if_matched;
			    ಭಙ್ಗ;
			}
			ಯದಿ (rex.reg_ic)
			{
			    ಪೂರ್ಣಾಙ್ಕ curc_ಕೆಳಗಿನ = MB_CASEFOLD(curc);
			    ಪೂರ್ಣಾಙ್ಕ done = FALSE;

			    ಪ್ರತಿ ( ; c1 <= c2; ++c1)
				ಯದಿ (MB_CASEFOLD(c1) == curc_ಕೆಳಗಿನ)
				{
				    result = result_if_matched;
				    done = TRUE;
				    ಭಙ್ಗ;
				}
			    ಯದಿ (done)
				ಭಙ್ಗ;
			}
		    }
		    ಅನ್ಯಥಾ ಯದಿ (state->c < 0 ? check_char_class(state->c, curc)
			       : (curc == state->c
				   || (rex.reg_ic && MB_CASEFOLD(curc)
						    == MB_CASEFOLD(state->c))))
		    {
			result = result_if_matched;
			ಭಙ್ಗ;
		    }
		    state = state->out;
		}
		ಯದಿ (result)
		{
		    // next state is in out of the NFA_END_COLL, out1 of
		    // START points to the END state
		    add_state = t->state->out1->out;
		    add_off = clen;
		}
		ಭಙ್ಗ;
	      }

	    ಪಕ್ಷ NFA_ANY:
		// Any char except '\0', (end of input) does not match.
		ಯದಿ (curc > 0)
		{
		    add_state = t->state->out;
		    add_off = clen;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_ANY_COMPOSING:
		// On a composing character skip over it.  Otherwise do
		// nothing.  Always matches.
		ಯದಿ (enc_utf8 && utf_iscomposing(curc))
		{
		    add_off = clen;
		}
		ಅನ್ಯಥಾ
		{
		    add_here = TRUE;
		    add_off = 0;
		}
		add_state = t->state->out;
		ಭಙ್ಗ;

	    /*
	     * Character classes like \a for alpha, \d for digit etc.
	     */
	    ಪಕ್ಷ NFA_IDENT:	//  \i
		result = vim_isIDc(curc);
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_SIDENT:	//  \I
		result = !VIM_ISDIGIT(curc) && vim_isIDc(curc);
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_KWORD:	//  \k
		result = vim_iswordp_buf(rex.ಒಳಹರಿವು, rex.reg_buf);
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_SKWORD:	//  \K
		result = !VIM_ISDIGIT(curc)
				     && vim_iswordp_buf(rex.ಒಳಹರಿವು, rex.reg_buf);
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_FNAME:	//  \f
		result = vim_isfilec(curc);
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_SFNAME:	//  \F
		result = !VIM_ISDIGIT(curc) && vim_isfilec(curc);
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_PRINT:	//  \p
		result = vim_isprintc(PTR2CHAR(rex.ಒಳಹರಿವು));
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_SPRINT:	//  \P
		result = !VIM_ISDIGIT(curc) && vim_isprintc(PTR2CHAR(rex.ಒಳಹರಿವು));
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_WHITE:	//  \s
		result = VIM_ISWHITE(curc);
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_NWHITE:	//  \S
		result = curc != NUL && !VIM_ISWHITE(curc);
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_DIGIT:	//  \d
		result = ri_digit(curc);
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_NDIGIT:	//  \D
		result = curc != NUL && !ri_digit(curc);
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_HEX:	//  \x
		result = ri_hex(curc);
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_NHEX:	//  \X
		result = curc != NUL && !ri_hex(curc);
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_OCTAL:	//  \o
		result = ri_octal(curc);
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_NOCTAL:	//  \O
		result = curc != NUL && !ri_octal(curc);
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_WORD:	//  \w
		result = ri_word(curc);
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_NWORD:	//  \W
		result = curc != NUL && !ri_word(curc);
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_HEAD:	//  \h
		result = ri_head(curc);
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_NHEAD:	//  \H
		result = curc != NUL && !ri_head(curc);
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_ALPHA:	//  \a
		result = ri_alpha(curc);
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_NALPHA:	//  \A
		result = curc != NUL && !ri_alpha(curc);
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_LOWER:	//  \l
		result = ri_lower(curc);
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_NLOWER:	//  \L
		result = curc != NUL && !ri_lower(curc);
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_UPPER:	//  \u
		result = ri_upper(curc);
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_NUPPER:	// \U
		result = curc != NUL && !ri_upper(curc);
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_LOWER_IC:	// [a-z]
		result = ri_lower(curc) || (rex.reg_ic && ri_upper(curc));
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_NLOWER_IC:	// [^a-z]
		result = curc != NUL
			&& !(ri_lower(curc) || (rex.reg_ic && ri_upper(curc)));
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_UPPER_IC:	// [A-Z]
		result = ri_upper(curc) || (rex.reg_ic && ri_lower(curc));
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_NUPPER_IC:	// ^[A-Z]
		result = curc != NUL
			&& !(ri_upper(curc) || (rex.reg_ic && ri_lower(curc)));
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_BACKREF1:
	    ಪಕ್ಷ NFA_BACKREF2:
	    ಪಕ್ಷ NFA_BACKREF3:
	    ಪಕ್ಷ NFA_BACKREF4:
	    ಪಕ್ಷ NFA_BACKREF5:
	    ಪಕ್ಷ NFA_BACKREF6:
	    ಪಕ್ಷ NFA_BACKREF7:
	    ಪಕ್ಷ NFA_BACKREF8:
	    ಪಕ್ಷ NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	    ಪಕ್ಷ NFA_ZREF1:
	    ಪಕ್ಷ NFA_ZREF2:
	    ಪಕ್ಷ NFA_ZREF3:
	    ಪಕ್ಷ NFA_ZREF4:
	    ಪಕ್ಷ NFA_ZREF5:
	    ಪಕ್ಷ NFA_ZREF6:
	    ಪಕ್ಷ NFA_ZREF7:
	    ಪಕ್ಷ NFA_ZREF8:
	    ಪಕ್ಷ NFA_ZREF9:
#endif
		// \1 .. \9  \z1 .. \z9
	      {
		ಪೂರ್ಣಾಙ್ಕ subidx;
		ಪೂರ್ಣಾಙ್ಕ bytelen;

#ifdef FEAT_SYN_HL
		ಯದಿ (t->state->c >= NFA_BACKREF1 && t->state->c <= NFA_BACKREF9)
#endif
		{
		    subidx = t->state->c - NFA_BACKREF1 + 1;
		    result = match_backref(&t->subs.norm, subidx, &bytelen);
		}
#ifdef FEAT_SYN_HL
		ಅನ್ಯಥಾ
		{
		    subidx = t->state->c - NFA_ZREF1 + 1;
		    result = match_zref(subidx, &bytelen);
		}
#endif

		ಯದಿ (result)
		{
		    ಯದಿ (bytelen == 0)
		    {
			// empty match always works, output of NFA_SKIP to be
			// used next
			add_here = TRUE;
			add_state = t->state->out->out;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (bytelen <= clen)
		    {
			// match current character, jump ahead to out of
			// NFA_SKIP
			add_state = t->state->out->out;
			add_off = clen;
		    }
		    ಅನ್ಯಥಾ
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			add_state = t->state->out;
			add_off = bytelen;
			add_ಎಣಿಕೆ = bytelen - clen;
		    }
		}
		ಭಙ್ಗ;
	      }
	    ಪಕ್ಷ NFA_SKIP:
	      // character of previous matching \1 .. \9  or \@>
	      ಯದಿ (t->ಎಣಿಕೆ - clen <= 0)
	      {
		  // end of match, go to what follows
		  add_state = t->state->out;
		  add_off = clen;
	      }
	      ಅನ್ಯಥಾ
	      {
		  // add state again with decremented count
		  add_state = t->state;
		  add_off = 0;
		  add_ಎಣಿಕೆ = t->ಎಣಿಕೆ - clen;
	      }
	      ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_LNUM:
	    ಪಕ್ಷ NFA_LNUM_GT:
	    ಪಕ್ಷ NFA_LNUM_LT:
		result = (REG_MULTI &&
			nfa_re_ಅಂಕೆ_cmp(t->state->val, t->state->c - NFA_LNUM,
			    (long_u)(rex.lnum + rex.reg_firstlnum)));
		ಯದಿ (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_COL:
	    ಪಕ್ಷ NFA_COL_GT:
	    ಪಕ್ಷ NFA_COL_LT:
		result = nfa_re_ಅಂಕೆ_cmp(t->state->val, t->state->c - NFA_COL,
			(long_u)(rex.ಒಳಹರಿವು - rex.ಸಾಲು) + 1);
		ಯದಿ (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_VCOL:
	    ಪಕ್ಷ NFA_VCOL_GT:
	    ಪಕ್ಷ NFA_VCOL_LT:
		{
		    ಪೂರ್ಣಾಙ್ಕ     op = t->state->c - NFA_VCOL;
		    colnr_T col = (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು);
		    win_T   *wp = rex.reg_win == ರಿಕ್ತ ? curwin : rex.reg_win;

		    // Bail out quickly when there can't be a match, avoid the
		    // overhead of win_linetabsize() on long lines.
		    ಯದಿ (op != 1 && col > t->state->val
			    * (has_mbyte ? MB_MAXBYTES : 1))
			ಭಙ್ಗ;
		    result = FALSE;
		    ಯದಿ (op == 1 && col - 1 > t->state->val && col > 100)
		    {
			ಪೂರ್ಣಾಙ್ಕ ts = wp->w_buffer->b_p_ts;

			// Guess that a character won't use more columns than
			// 'tabstop', with a minimum of 4.
			ಯದಿ (ts < 4)
			    ts = 4;
			result = col > t->state->val * ts;
		    }
		    ಯದಿ (!result)
		    {
			linenr_T    lnum = REG_MULTI
					    ? rex.reg_firstlnum + rex.lnum : 1;
			long_u	    vcol;

			ಯದಿ (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ))
			    lnum = 1;
			vcol = (long_u)win_linetabsize(wp, lnum, rex.ಸಾಲು, col);
			result = nfa_re_ಅಂಕೆ_cmp(t->state->val, op, vcol + 1);
		    }
		    ಯದಿ (result)
		    {
			add_here = TRUE;
			add_state = t->state->out;
		    }
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_MARK:
	    ಪಕ್ಷ NFA_MARK_GT:
	    ಪಕ್ಷ NFA_MARK_LT:
	      {
		pos_T	*pos;
		size_t	col = REG_MULTI ? rex.ಒಳಹರಿವು - rex.ಸಾಲು : 0;

		pos = getmark_buf(rex.reg_buf, t->state->val, FALSE);

		// Line may have been freed, get it again.
		ಯದಿ (REG_MULTI)
		{
		    rex.ಸಾಲು = reg_getline(rex.lnum);
		    rex.ಒಳಹರಿವು = rex.ಸಾಲು + col;
		}

		// Compare the mark position to the match position, if the mark
		// exists and mark is set in reg_buf.
		ಯದಿ (pos != ರಿಕ್ತ && pos->lnum > 0)
		{
		    colnr_T pos_col = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->col == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->col;

		    result = (pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_col == (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು)
				    ? t->state->c == NFA_MARK
				    : (pos_col < (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು)
					? t->state->c == NFA_MARK_GT
					: t->state->c == NFA_MARK_LT))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? t->state->c == NFA_MARK_GT
				    : t->state->c == NFA_MARK_LT));
		    ಯದಿ (result)
		    {
			add_here = TRUE;
			add_state = t->state->out;
		    }
		}
		ಭಙ್ಗ;
	      }

	    ಪಕ್ಷ NFA_CURSOR:
		result = (rex.reg_win != ರಿಕ್ತ
			&& (rex.lnum + rex.reg_firstlnum
						 == rex.reg_win->w_cursor.lnum)
			&& ((colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು)
						== rex.reg_win->w_cursor.col));
		ಯದಿ (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_VISUAL:
		result = reg_match_visual();
		ಯದಿ (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ NFA_MOPEN1:
	    ಪಕ್ಷ NFA_MOPEN2:
	    ಪಕ್ಷ NFA_MOPEN3:
	    ಪಕ್ಷ NFA_MOPEN4:
	    ಪಕ್ಷ NFA_MOPEN5:
	    ಪಕ್ಷ NFA_MOPEN6:
	    ಪಕ್ಷ NFA_MOPEN7:
	    ಪಕ್ಷ NFA_MOPEN8:
	    ಪಕ್ಷ NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	    ಪಕ್ಷ NFA_ZOPEN:
	    ಪಕ್ಷ NFA_ZOPEN1:
	    ಪಕ್ಷ NFA_ZOPEN2:
	    ಪಕ್ಷ NFA_ZOPEN3:
	    ಪಕ್ಷ NFA_ZOPEN4:
	    ಪಕ್ಷ NFA_ZOPEN5:
	    ಪಕ್ಷ NFA_ZOPEN6:
	    ಪಕ್ಷ NFA_ZOPEN7:
	    ಪಕ್ಷ NFA_ZOPEN8:
	    ಪಕ್ಷ NFA_ZOPEN9:
#endif
	    ಪಕ್ಷ NFA_NOPEN:
	    ಪಕ್ಷ NFA_ZSTART:
		// These states are only added to be able to bail out when
		// they are added again, nothing is to be done.
		ಭಙ್ಗ;

	    ಸ್ವಭಾವ:	// regular character
	      {
		ಪೂರ್ಣಾಙ್ಕ c = t->state->c;

#ifdef DEBUG
		ಯದಿ (c < 0)
		    siemsg("Negative state char: %ld", (long)c);
#endif
		result = (c == curc);

		ಯದಿ (!result && rex.reg_ic)
		    result = MB_CASEFOLD(c) == MB_CASEFOLD(curc);
		// If rex.reg_icombine is not set only skip over the character
		// itself.  When it is set skip over composing characters.
		ಯದಿ (result && enc_utf8 && !rex.reg_icombine)
		    clen = utf_ptr2len(rex.ಒಳಹರಿವು);
		ADD_STATE_IF_MATCH(t->state);
		ಭಙ್ಗ;
	      }

	    } // switch (t->state->c)

	    ಯದಿ (add_state != ರಿಕ್ತ)
	    {
		nfa_pim_T *pim;
		nfa_pim_T pim_copy;

		ಯದಿ (t->pim.result == NFA_PIM_UNUSED)
		    pim = ರಿಕ್ತ;
		ಅನ್ಯಥಾ
		    pim = &t->pim;

		// Handle the postponed invisible match if the match might end
		// without advancing and before the end of the line.
		ಯದಿ (pim != ರಿಕ್ತ && (clen == 0 || match_follows(add_state, 0)))
		{
		    ಯದಿ (pim->result == NFA_PIM_TODO)
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
			ಯದಿ (result != (pim->state->c == NFA_START_INVISIBLE_NEG
			     || pim->state->c == NFA_START_INVISIBLE_NEG_FIRST
			     || pim->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&pim->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
			    ಯದಿ (rex.nfa_has_zsubexpr)
				copy_sub_off(&pim->subs.synt, &m->synt);
#endif
			}
		    }
		    ಅನ್ಯಥಾ
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
		    ಯದಿ (result != (pim->state->c == NFA_START_INVISIBLE_NEG
			     || pim->state->c == NFA_START_INVISIBLE_NEG_FIRST
			     || pim->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
		    {
			// Copy submatch info from the recursive call
			copy_sub_off(&t->subs.norm, &pim->subs.norm);
#ifdef FEAT_SYN_HL
			ಯದಿ (rex.nfa_has_zsubexpr)
			    copy_sub_off(&t->subs.synt, &pim->subs.synt);
#endif
		    }
		    ಅನ್ಯಥಾ
			// look-behind match failed, don't add the state
			ಅನುವರ್ತ;

		    // Postponed invisible match was handled, don't add it to
		    // following states.
		    pim = ರಿಕ್ತ;
		}

		// If "pim" points into l->t it will become invalid when
		// adding the state causes the list to be reallocated.  Make a
		// local copy to avoid that.
		ಯದಿ (pim == &t->pim)
		{
		    copy_pim(&pim_copy, pim);
		    pim = &pim_copy;
		}

		ಯದಿ (add_here)
		    r = addstate_here(thislist, add_state, &t->subs,
								pim, &listidx);
		ಅನ್ಯಥಾ
		{
		    r = addstate(nextlist, add_state, &t->subs, pim, add_off, 0);
		    ಯದಿ (add_ಎಣಿಕೆ > 0)
			nextlist->t[nextlist->n - 1].ಎಣಿಕೆ = add_ಎಣಿಕೆ;
		}
		ಯದಿ (r == ರಿಕ್ತ)
		{
		    nfa_match = NFA_TOO_EXPENSIVE;
		    ಗಚ್ಛ theend;
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
	ಯದಿ (nfa_match == FALSE
		&& ((toplevel
			&& rex.lnum == 0
			&& clen != 0
			&& (rex.reg_maxcol == 0
			  || (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು) < rex.reg_maxcol))
		    || (nfa_endp != ರಿಕ್ತ
			&& (REG_MULTI
			    ? (rex.lnum < nfa_endp->se_u.pos.lnum
			       || (rex.lnum == nfa_endp->se_u.pos.lnum
				   && (ಪೂರ್ಣಾಙ್ಕ)(rex.ಒಳಹರಿವು - rex.ಸಾಲು)
						    < nfa_endp->se_u.pos.col))
			    : rex.ಒಳಹರಿವು < nfa_endp->se_u.ptr))))
	{
#ifdef ENABLE_LOG
	    fprintf(log_fd, "(---) STARTSTATE\n");
#endif
	    // Inline optimized code for addstate() if we know the state is
	    // the first MOPEN.
	    ಯದಿ (toplevel)
	    {
		ಪೂರ್ಣಾಙ್ಕ add = TRUE;
		ಪೂರ್ಣಾಙ್ಕ c;

		ಯದಿ (prog->regstart != NUL && clen != 0)
		{
		    ಯದಿ (nextlist->n == 0)
		    {
			colnr_T col = (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು) + clen;

			// Nextlist is empty, we can skip ahead to the
			// character that must appear at the start.
			ಯದಿ (skip_to_start(prog->regstart, &col) == FAIL)
			    ಭಙ್ಗ;
#ifdef ENABLE_LOG
			fprintf(log_fd, "  Skipping ahead %d bytes to regstart\n",
				col - ((colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು) + clen));
#endif
			rex.ಒಳಹರಿವು = rex.ಸಾಲು + col - clen;
		    }
		    ಅನ್ಯಥಾ
		    {
			// Checking if the required start character matches is
			// cheaper than adding a state that won't match.
			c = PTR2CHAR(rex.ಒಳಹರಿವು + clen);
			ಯದಿ (c != prog->regstart && (!rex.reg_ic
			     || MB_CASEFOLD(c) != MB_CASEFOLD(prog->regstart)))
			{
#ifdef ENABLE_LOG
			    fprintf(log_fd, "  Skipping start state, regstart does not match\n");
#endif
			    add = FALSE;
			}
		    }
		}

		ಯದಿ (add)
		{
		    ಯದಿ (REG_MULTI)
		    {
			m->norm.list.multi[0].start_col =
					 (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು) + clen;
			m->norm.orig_start_col =
					       m->norm.list.multi[0].start_col;
		    }
		    ಅನ್ಯಥಾ
			m->norm.list.ಸಾಲು[0].start = rex.ಒಳಹರಿವು + clen;
		    ಯದಿ (addstate(nextlist, start->out, m, ರಿಕ್ತ, clen, 0) == ರಿಕ್ತ)
		    {
			nfa_match = NFA_TOO_EXPENSIVE;
			ಗಚ್ಛ theend;
		    }
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		char_u	    *save_ಸಾಲು = rex.ಸಾಲು;
		char_u	    *save_ಒಳಹರಿವು = rex.ಒಳಹರಿವು;
		linenr_T    save_lnum = rex.lnum;

		// At the end of a line the match can only start on the next
		// line, use that position instead of the line break.
		ಯದಿ (REG_MULTI && clen == 0 && nfa_endp != ರಿಕ್ತ
					 && rex.lnum < nfa_endp->se_u.pos.lnum)
		{
		    char_u  *next_ಸಾಲು = reg_getline(rex.lnum + 1);

		    ಯದಿ (next_ಸಾಲು != ರಿಕ್ತ)
		    {
			rex.ಸಾಲು = next_ಸಾಲು;
			rex.ಒಳಹರಿವು = rex.ಸಾಲು;
			++rex.lnum;
		    }
		}

		r = addstate(nextlist, start, m, ರಿಕ್ತ, clen, 0);

		rex.ಸಾಲು = save_ಸಾಲು;
		rex.ಒಳಹರಿವು = save_ಒಳಹರಿವು;
		rex.lnum = save_lnum;

		ಯದಿ (r == ರಿಕ್ತ)
		{
		    nfa_match = NFA_TOO_EXPENSIVE;
		    ಗಚ್ಛ theend;
		}
	    }
	}

#ifdef ENABLE_LOG
	fprintf(log_fd, ">>> Thislist had %d states available: ", thislist->n);
	{
	    ಪೂರ್ಣಾಙ್ಕ i;

	    ಪ್ರತಿ (i = 0; i < thislist->n; i++)
		fprintf(log_fd, "%d  ", abs(thislist->t[i].state->id));
	}
	fprintf(log_fd, "\n");
#endif

nextchar:
	// Advance to the next character, or advance to the next line, or
	// finish.
	ಯದಿ (clen != 0)
	    rex.ಒಳಹರಿವು += clen;
	ಅನ್ಯಥಾ ಯದಿ (go_to_nextline || (nfa_endp != ರಿಕ್ತ && REG_MULTI
					&& rex.lnum < nfa_endp->se_u.pos.lnum))
	    reg_nextline();
	ಅನ್ಯಥಾ
	    ಭಙ್ಗ;

	// Allow interrupting with CTRL-C.
	ಸಾಲು_breakcheck();
	ಯದಿ (got_int)
	    ಭಙ್ಗ;
#ifdef FEAT_RELTIME
	ಯದಿ (nfa_did_time_out())
	    ಭಙ್ಗ;
#endif
    }

#ifdef ENABLE_LOG
    ಯದಿ (log_fd != stderr)
	fclose(log_fd);
    log_fd = ರಿಕ್ತ;
#endif

theend:
    // Cache list buffers in prog for reuse, or free if prog already has
    // cached buffers (recursive call case).
    ಯದಿ (prog->listbuf[0] == ರಿಕ್ತ && list[0].t != ರಿಕ್ತ && list[1].t != ರಿಕ್ತ)
    {
	prog->listbuf[0] = list[0].t;
	prog->listbuf[1] = list[1].t;
    }
    ಅನ್ಯಥಾ
    {
	vim_free(list[0].t);
	vim_free(list[1].t);
    }
    vim_free(listids);
#undef ADD_STATE_IF_MATCH
#ifdef NFA_REGEXP_DEBUG_LOG
    fclose(debug);
#endif

    ಪ್ರತ್ಯಾಗಮ nfa_match;
}

/*
 * Try match of "prog" with at rex.line["col"].
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    ಸ್ಥಿರ long
nfa_regtry(
    nfa_regprog_T   *prog,
    colnr_T	    col,
    ಪೂರ್ಣಾಙ್ಕ		    *timed_out UNUSED)	// flag set on timeout or NULL
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    regsubs_T	subs, m;
    nfa_state_T	*start = prog->start;
    ಪೂರ್ಣಾಙ್ಕ		result;
#ifdef ENABLE_LOG
    FILE	*f;
#endif

    rex.ಒಳಹರಿವು = rex.ಸಾಲು + col;
#ifdef FEAT_RELTIME
    nfa_timed_out = timed_out;
#endif

#ifdef ENABLE_LOG
    f = fopen(NFA_REGEXP_RUN_LOG, "a");
    ಯದಿ (f != ರಿಕ್ತ)
    {
	fprintf(f, "\n\n\t=======================================================\n");
# ifdef DEBUG
	fprintf(f, "\tRegexp is \"%s\"\n", nfa_regengine.ಅಭಿವ್ಯಕ್ತಿ);
# endif
	fprintf(f, "\tInput text is \"%s\" \n", rex.ಒಳಹರಿವು);
	fprintf(f, "\t=======================================================\n\n");
	nfa_print_state(f, start);
	fprintf(f, "\n\n");
	fclose(f);
    }
    ಅನ್ಯಥಾ
	emsg("Could not open temporary log file for writing");
#endif

    clear_sub(&subs.norm);
    clear_sub(&m.norm);
#ifdef FEAT_SYN_HL
    clear_sub(&subs.synt);
    clear_sub(&m.synt);
#endif

    result = nfa_regmatch(prog, start, &subs, &m);
    ಯದಿ (result == FALSE)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಅನ್ಯಥಾ ಯದಿ (result == NFA_TOO_EXPENSIVE)
	ಪ್ರತ್ಯಾಗಮ result;

    cleanup_subexpr();
    ಯದಿ (REG_MULTI)
    {
	ಪ್ರತಿ (i = 0; i < subs.norm.in_use; i++)
	{
	    rex.reg_startpos[i].lnum = subs.norm.list.multi[i].start_lnum;
	    rex.reg_startpos[i].col = subs.norm.list.multi[i].start_col;

	    rex.reg_endpos[i].lnum = subs.norm.list.multi[i].end_lnum;
	    rex.reg_endpos[i].col = subs.norm.list.multi[i].end_col;
	}
	ಯದಿ (rex.reg_mmatch != ರಿಕ್ತ)
	    rex.reg_mmatch->rmm_matchcol = subs.norm.orig_start_col;

	ಯದಿ (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].col = col;
	}
	ಯದಿ (rex.reg_endpos[0].lnum < 0)
	{
	    // pattern has a \ze but it didn't match, use current end
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].col = (ಪೂರ್ಣಾಙ್ಕ)(rex.ಒಳಹರಿವು - rex.ಸಾಲು);
	}
	ಅನ್ಯಥಾ
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    ಅನ್ಯಥಾ
    {
	ಪ್ರತಿ (i = 0; i < subs.norm.in_use; i++)
	{
	    rex.reg_startp[i] = subs.norm.list.ಸಾಲು[i].start;
	    rex.reg_endp[i] = subs.norm.list.ಸಾಲು[i].end;
	}

	ಯದಿ (rex.reg_startp[0] == ರಿಕ್ತ)
	    rex.reg_startp[0] = rex.ಸಾಲು + col;
	ಯದಿ (rex.reg_endp[0] == ರಿಕ್ತ)
	    rex.reg_endp[0] = rex.ಒಳಹರಿವು;
    }

#ifdef FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = ರಿಕ್ತ;

    ಯದಿ (prog->reghasz == REX_SET)
    {
	cleanup_zsubexpr();
	re_extmatch_out = make_extmatch();
	ಯದಿ (re_extmatch_out == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ 0;
	// Loop over \z1, \z2, etc.  There is no \z0.
	ಪ್ರತಿ (i = 1; i < subs.synt.in_use; i++)
	{
	    ಯದಿ (REG_MULTI)
	    {
		ಸಂರಚನಾ multipos *mpos = &subs.synt.list.multi[i];

		// Only accept single line matches that are valid.
		ಯದಿ (mpos->start_lnum >= 0
			&& mpos->start_lnum == mpos->end_lnum
			&& mpos->end_col >= mpos->start_col)
		    re_extmatch_out->matches[i] =
			vim_strnsave(reg_getline(mpos->start_lnum)
							    + mpos->start_col,
					     mpos->end_col - mpos->start_col);
	    }
	    ಅನ್ಯಥಾ
	    {
		ಸಂರಚನಾ linepos *lpos = &subs.synt.list.ಸಾಲು[i];

		ಯದಿ (lpos->start != ರಿಕ್ತ && lpos->end != ರಿಕ್ತ)
		    re_extmatch_out->matches[i] =
			    vim_strnsave(lpos->start, lpos->end - lpos->start);
	    }
	}
    }
#endif

    ಪ್ರತ್ಯಾಗಮ 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 *
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    ಸ್ಥಿರ long
nfa_regexec_both(
    char_u	*ಸಾಲು,
    colnr_T	startcol,	// column to start looking for match
    ಪೂರ್ಣಾಙ್ಕ		*timed_out)	// flag set on timeout or NULL
{
    nfa_regprog_T   *prog;
    long	    retval = 0L;
    ಪೂರ್ಣಾಙ್ಕ		    i;
    colnr_T	    col = startcol;

    ಯದಿ (REG_MULTI)
    {
	prog = (nfa_regprog_T *)rex.reg_mmatch->regprog;
	ಸಾಲು = reg_getline((linenr_T)0);    // relative to the cursor
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    ಅನ್ಯಥಾ
    {
	prog = (nfa_regprog_T *)rex.reg_match->regprog;
	rex.reg_startp = rex.reg_match->startp;
	rex.reg_endp = rex.reg_match->endp;
    }

    // Be paranoid...
    ಯದಿ (prog == ರಿಕ್ತ || ಸಾಲು == ರಿಕ್ತ)
    {
	iemsg(e_null_argument);
	ಗಚ್ಛ theend;
    }

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    ಯದಿ (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    ಅನ್ಯಥಾ ಯದಿ (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    ಯದಿ (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    rex.ಸಾಲು = ಸಾಲು;
    rex.lnum = 0;    // relative to line

    rex.nfa_has_zend = prog->has_zend;
    rex.nfa_has_backref = prog->has_backref;
    rex.nfa_nsubexpr = prog->nsubexp;
    rex.nfa_listid = 1;
    rex.nfa_alt_listid = 2;
#ifdef DEBUG
    nfa_regengine.ಅಭಿವ್ಯಕ್ತಿ = prog->pattern;
#endif

    ಯದಿ (prog->reganch && col > 0)
	ಪ್ರತ್ಯಾಗಮ 0L;

    rex.need_clear_subexpr = TRUE;
#ifdef FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    ಯದಿ (prog->reghasz == REX_SET)
    {
	rex.nfa_has_zsubexpr = TRUE;
	rex.need_clear_zsubexpr = TRUE;
    }
    ಅನ್ಯಥಾ
    {
	rex.nfa_has_zsubexpr = FALSE;
	rex.need_clear_zsubexpr = FALSE;
    }
#endif

    ಯದಿ (prog->regstart != NUL)
    {
	// Skip ahead until a character we know the match must start with.
	// When there is none there is no match.
	ಯದಿ (skip_to_start(prog->regstart, &col) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ 0L;

	// If match_text is set it contains the full text that must match.
	// Nothing else to try. Doesn't handle combining chars well.
	ಯದಿ (prog->match_text != ರಿಕ್ತ && *prog->match_text != NUL && !rex.reg_icombine)
	{
	    retval = find_match_text(&col, prog->regstart, prog->match_text);
	    ಯದಿ (REG_MULTI)
		rex.reg_mmatch->rmm_matchcol = col;
	    ಅನ್ಯಥಾ
		rex.reg_match->rm_matchcol = col;
	    ಪ್ರತ್ಯಾಗಮ retval;
	}
    }

    // If the start column is past the maximum column: no need to try.
    ಯದಿ (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	ಗಚ್ಛ theend;

    // Set the "nstate" used by nfa_regcomp() to zero to trigger an error when
    // it's accidentally used during execution.
    nstate = 0;
    ಪ್ರತಿ (i = 0; i < prog->nstate; ++i)
    {
	prog->state[i].id = i;
	prog->state[i].lastlist[0] = 0;
	prog->state[i].lastlist[1] = 0;
    }

    retval = nfa_regtry(prog, col, timed_out);

#ifdef DEBUG
    nfa_regengine.ಅಭಿವ್ಯಕ್ತಿ = ರಿಕ್ತ;
#endif

theend:
    ಯದಿ (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs and
	// \ze are used.
	ಯದಿ (REG_MULTI)
	{
	    lpos_T *start = &rex.reg_mmatch->startpos[0];
	    lpos_T *end = &rex.reg_mmatch->endpos[0];

	    ಯದಿ (end->lnum < start->lnum
			|| (end->lnum == start->lnum && end->col < start->col))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (rex.reg_match->endp[0] < rex.reg_match->startp[0])
		rex.reg_match->endp[0] = rex.reg_match->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_match->rm_matchcol = col;
	}
    }

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Compile a regular expression into internal code for the NFA matcher.
 * Returns the program in allocated space.  Returns NULL for an error.
 */
    ಸ್ಥಿರ regprog_T *
nfa_regcomp(char_u *ಅಭಿವ್ಯಕ್ತಿ, ಪೂರ್ಣಾಙ್ಕ re_flags)
{
    nfa_regprog_T	*prog = ರಿಕ್ತ;
    size_t		prog_size;
    ಪೂರ್ಣಾಙ್ಕ			*postfix;

    ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

#ifdef DEBUG
    nfa_regengine.ಅಭಿವ್ಯಕ್ತಿ = ಅಭಿವ್ಯಕ್ತಿ;
#endif
    nfa_re_flags = re_flags;

    init_class_tab();

    ಯದಿ (nfa_regcomp_start(ಅಭಿವ್ಯಕ್ತಿ, re_flags) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Build postfix form of the regexp. Needed to build the NFA
    // (and count its size).
    postfix = re2post();
    ಯದಿ (postfix == ರಿಕ್ತ)
	ಗಚ್ಛ fail;	    // Cascaded (syntax?) error

    /*
     * In order to build the NFA, we parse the input regexp twice:
     * 1. first pass to count size (so we can allocate space)
     * 2. second to emit code
     */
#ifdef ENABLE_LOG
    {
	FILE *f = fopen(NFA_REGEXP_RUN_LOG, "a");

	ಯದಿ (f != ರಿಕ್ತ)
	{
	    fprintf(f, "\n*****************************\n\n\n\n\tCompiling regexp \"%s\"... hold on !\n", ಅಭಿವ್ಯಕ್ತಿ);
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
    prog_size = offsetof(nfa_regprog_T, state) + ಪರಿಮಾಣ(nfa_state_T) * nstate;
    prog = alloc(prog_size);
    ಯದಿ (prog == ರಿಕ್ತ)
	ಗಚ್ಛ fail;
    state_ptr = prog->state;
    prog->re_in_use = FALSE;
    prog->listbuf[0] = ರಿಕ್ತ;
    prog->listbuf[1] = ರಿಕ್ತ;

    /*
     * PASS 2
     * Build the NFA
     */
    prog->start = post2nfa(postfix, post_ptr, FALSE);
    ಯದಿ (prog->start == ರಿಕ್ತ)
	ಗಚ್ಛ fail;

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
    nfa_postfix_dump(ಅಭಿವ್ಯಕ್ತಿ, OK);
    nfa_dump(prog);
#endif
#ifdef FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    prog->reghasz = re_has_z;
#endif
    prog->pattern = vim_strsave(ಅಭಿವ್ಯಕ್ತಿ);
#ifdef DEBUG
    nfa_regengine.ಅಭಿವ್ಯಕ್ತಿ = ರಿಕ್ತ;
#endif

out:
    // The postfix list and fragment stack are reused by the next compilation
    // (and freed in free_regexp_stuff()), but drop ones that grew large for a
    // big pattern to avoid holding much memory.
    ಯದಿ (post_start_len > NFA_POSTFIX_KEEP)
    {
	VIM_CLEAR(post_start);
	post_start_len = 0;
    }
    post_ptr = post_end = ರಿಕ್ತ;
    ಯದಿ (nfa_ರಾಶಿ_len > NFA_ರಾಶಿ_KEEP)
    {
	VIM_CLEAR(nfa_ರಾಶಿ);
	nfa_ರಾಶಿ_len = 0;
    }
    state_ptr = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ (regprog_T *)prog;

fail:
    VIM_CLEAR(prog);
#ifdef ENABLE_LOG
    nfa_postfix_dump(ಅಭಿವ್ಯಕ್ತಿ, FAIL);
#endif
#ifdef DEBUG
    nfa_regengine.ಅಭಿವ್ಯಕ್ತಿ = ರಿಕ್ತ;
#endif
    ಗಚ್ಛ out;
}

/*
 * Free a compiled regexp program, returned by nfa_regcomp().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
nfa_regfree(regprog_T *prog)
{
    ಯದಿ (prog == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
nfa_regexec_nl(
    regmatch_T	*rmp,
    char_u	*ಸಾಲು,	// string to match against
    colnr_T	col,	// column to start looking for match
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು_lbr)
{
    rex.reg_match = rmp;
    rex.reg_mmatch = ರಿಕ್ತ;
    rex.reg_maxline = 0;
    rex.reg_ಸಾಲು_lbr = ಸಾಲು_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = ರಿಕ್ತ;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;
    ಪ್ರತ್ಯಾಗಮ nfa_regexec_both(ಸಾಲು, col, ರಿಕ್ತ);
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
    ಸ್ಥಿರ long
nfa_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	col,		// column to start looking for match
    ಪೂರ್ಣಾಙ್ಕ		*timed_out)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    ಪ್ರತ್ಯಾಗಮ nfa_regexec_both(ರಿಕ್ತ, col, timed_out);
}

#ifdef DEBUG
# undef ENABLE_LOG
#endif
