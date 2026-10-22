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

গণনা
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
স্থির ধ্রুৱ পূর্ণাঙ্ক nfa_classcodes[] = {
    NFA_ANY, NFA_IDENT, NFA_SIDENT, NFA_KWORD,NFA_SKWORD,
    NFA_FNAME, NFA_SFNAME, NFA_PRINT, NFA_SPRINT,
    NFA_WHITE, NFA_NWHITE, NFA_DIGIT, NFA_NDIGIT,
    NFA_HEX, NFA_NHEX, NFA_OCTAL, NFA_NOCTAL,
    NFA_WORD, NFA_NWORD, NFA_HEAD, NFA_NHEAD,
    NFA_ALPHA, NFA_NALPHA, NFA_LOWER, NFA_NLOWER,
    NFA_UPPER, NFA_NUPPER
};

// Variables only used in nfa_regcomp() and descendants.
স্থির পূর্ণাঙ্ক nfa_re_flags; // re_flags passed to nfa_regcomp()
স্থির পূর্ণাঙ্ক *post_start;  // holds the postfix form of r.e.
স্থির পূর্ণাঙ্ক post_start_len;  // size of allocated post_start (in ints)
স্থির পূর্ণাঙ্ক *post_end;
স্থির পূর্ণাঙ্ক *post_ptr;
স্থির পূর্ণাঙ্ক nfa_reg_parse_depth;	// nesting depth in nfa_reg()

// The postfix list (post_start) and fragment stack (nfa_stack) are reused
// across compilations; a buffer that grew past these sizes for a big pattern
// is freed afterwards instead of being kept, to avoid holding much memory.
#define NFA_POSTFIX_KEEP	10000	// number of ints (~40 Kbyte)
#define NFA_স্তূপ_KEEP		4000	// number of Frag_T (~64 Kbyte)

// Set when the pattern should use the NFA engine.
// E.g. [[:upper:]] only allows 8bit characters for BT engine,
// while NFA engine handles multibyte characters correctly.
স্থির পূর্ণাঙ্ক wants_nfa;

স্থির পূর্ণাঙ্ক nstate;	// Number of states in the NFA.
স্থির পূর্ণাঙ্ক istate;	// Index in the state vector, used in alloc_state()

// If not NULL match must end at this position
স্থির save_se_T *nfa_endp = রিক্ত;

// 0 for first call to nfa_regmatch(), 1 for recursive call.
স্থির পূর্ণাঙ্ক nfa_ll_index = 0;

স্থির পূর্ণাঙ্ক realloc_post_list(শূন্য);
স্থির পূর্ণাঙ্ক nfa_reg(পূর্ণাঙ্ক paren);
#ifdef DEBUG
স্থির শূন্য nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent);
#endif
স্থির পূর্ণাঙ্ক match_follows(nfa_state_T *startstate, পূর্ণাঙ্ক depth);
স্থির পূর্ণাঙ্ক failure_chance(nfa_state_T *state, পূর্ণাঙ্ক depth);

// helper functions used when doing re2post() ... regatom() parsing
#define EMIT(c)	কৰা {				\
		    যদি (post_ptr >= post_end && realloc_post_list() == FAIL) \
			ঘূৰাই FAIL;		\
		    *post_ptr++ = c;		\
		} যেতিয়ালৈ (0)

/*
 * Initialize internal variables before NFA compilation.
 * Return OK on success, FAIL otherwise.
 */
    স্থির পূর্ণাঙ্ক
nfa_regcomp_start(
    char_u	*ৰাশিমালা,
    পূর্ণাঙ্ক		re_flags)	    // see vim_regcomp()
{
    size_t	postfix_size;
    পূর্ণাঙ্ক		nstate_max;

    nstate = 0;
    istate = 0;
    // A reasonable estimation for maximum size
    nstate_max = (পূর্ণাঙ্ক)(STRLEN(ৰাশিমালা) + 1) * 25;

    // Some items blow up in size, such as [A-z].  Add more space for that.
    // When it is still not enough realloc_post_list() will be used.
    nstate_max += 1000;

    // Size for postfix representation of expr.
    postfix_size = পরিমাণ(পূর্ণাঙ্ক) * nstate_max;

    // Reuse the postfix buffer across compilations, only growing it when the
    // estimate exceeds the current size; it is freed in free_regexp_stuff().
    যদি (post_start == রিক্ত || post_start_len < nstate_max)
    {
	vim_free(post_start);
	post_start = alloc(postfix_size);
	যদি (post_start == রিক্ত)
	{
	    post_start_len = 0;
	    ঘূৰাই FAIL;
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
    regcomp_start(ৰাশিমালা, re_flags);

    ঘূৰাই OK;
}

/*
 * Figure out if the NFA state list starts with an anchor, must match at start
 * of the line.
 */
    স্থির পূর্ণাঙ্ক
nfa_get_reganch(nfa_state_T *start, পূর্ণাঙ্ক depth)
{
    nfa_state_T *p = start;

    যদি (depth > 4)
	ঘূৰাই 0;

    যেতিয়ালৈ (p != রিক্ত)
    {
	ৱিকল্প (p->c)
	{
	    পক্ষ NFA_BOL:
	    পক্ষ NFA_BOF:
		ঘূৰাই 1; // yes!

	    পক্ষ NFA_ZSTART:
	    পক্ষ NFA_ZEND:
	    পক্ষ NFA_CURSOR:
	    পক্ষ NFA_VISUAL:

	    পক্ষ NFA_MOPEN:
	    পক্ষ NFA_MOPEN1:
	    পক্ষ NFA_MOPEN2:
	    পক্ষ NFA_MOPEN3:
	    পক্ষ NFA_MOPEN4:
	    পক্ষ NFA_MOPEN5:
	    পক্ষ NFA_MOPEN6:
	    পক্ষ NFA_MOPEN7:
	    পক্ষ NFA_MOPEN8:
	    পক্ষ NFA_MOPEN9:
	    পক্ষ NFA_NOPEN:
#ifdef FEAT_SYN_HL
	    পক্ষ NFA_ZOPEN:
	    পক্ষ NFA_ZOPEN1:
	    পক্ষ NFA_ZOPEN2:
	    পক্ষ NFA_ZOPEN3:
	    পক্ষ NFA_ZOPEN4:
	    পক্ষ NFA_ZOPEN5:
	    পক্ষ NFA_ZOPEN6:
	    পক্ষ NFA_ZOPEN7:
	    পক্ষ NFA_ZOPEN8:
	    পক্ষ NFA_ZOPEN9:
#endif
		p = p->out;
		ভঙ্গ;

	    পক্ষ NFA_SPLIT:
		ঘূৰাই nfa_get_reganch(p->out, depth + 1)
				       && nfa_get_reganch(p->out1, depth + 1);

	    স্ৱভাৱ:
		ঘূৰাই 0; // noooo
	}
    }
    ঘূৰাই 0;
}

/*
 * Figure out if the NFA state list starts with a character which must match
 * at start of the match.
 */
    স্থির পূর্ণাঙ্ক
nfa_get_regstart(nfa_state_T *start, পূর্ণাঙ্ক depth)
{
    nfa_state_T *p = start;

    যদি (depth > 4)
	ঘূৰাই 0;

    যেতিয়ালৈ (p != রিক্ত)
    {
	ৱিকল্প (p->c)
	{
	    // all kinds of zero-width matches
	    পক্ষ NFA_BOL:
	    পক্ষ NFA_BOF:
	    পক্ষ NFA_BOW:
	    পক্ষ NFA_EOW:
	    পক্ষ NFA_ZSTART:
	    পক্ষ NFA_ZEND:
	    পক্ষ NFA_CURSOR:
	    পক্ষ NFA_VISUAL:
	    পক্ষ NFA_LNUM:
	    পক্ষ NFA_LNUM_GT:
	    পক্ষ NFA_LNUM_LT:
	    পক্ষ NFA_COL:
	    পক্ষ NFA_COL_GT:
	    পক্ষ NFA_COL_LT:
	    পক্ষ NFA_VCOL:
	    পক্ষ NFA_VCOL_GT:
	    পক্ষ NFA_VCOL_LT:
	    পক্ষ NFA_MARK:
	    পক্ষ NFA_MARK_GT:
	    পক্ষ NFA_MARK_LT:

	    পক্ষ NFA_MOPEN:
	    পক্ষ NFA_MOPEN1:
	    পক্ষ NFA_MOPEN2:
	    পক্ষ NFA_MOPEN3:
	    পক্ষ NFA_MOPEN4:
	    পক্ষ NFA_MOPEN5:
	    পক্ষ NFA_MOPEN6:
	    পক্ষ NFA_MOPEN7:
	    পক্ষ NFA_MOPEN8:
	    পক্ষ NFA_MOPEN9:
	    পক্ষ NFA_NOPEN:
#ifdef FEAT_SYN_HL
	    পক্ষ NFA_ZOPEN:
	    পক্ষ NFA_ZOPEN1:
	    পক্ষ NFA_ZOPEN2:
	    পক্ষ NFA_ZOPEN3:
	    পক্ষ NFA_ZOPEN4:
	    পক্ষ NFA_ZOPEN5:
	    পক্ষ NFA_ZOPEN6:
	    পক্ষ NFA_ZOPEN7:
	    পক্ষ NFA_ZOPEN8:
	    পক্ষ NFA_ZOPEN9:
#endif
		p = p->out;
		ভঙ্গ;

	    পক্ষ NFA_SPLIT:
	    {
		পূর্ণাঙ্ক c1 = nfa_get_regstart(p->out, depth + 1);
		পূর্ণাঙ্ক c2 = nfa_get_regstart(p->out1, depth + 1);

		যদি (c1 == c2)
		    ঘূৰাই c1; // yes!
		ঘূৰাই 0;
	    }

	    স্ৱভাৱ:
		যদি (p->c > 0)
		    ঘূৰাই p->c; // yes!
		ঘূৰাই 0;
	}
    }
    ঘূৰাই 0;
}

/*
 * Figure out if the NFA state list contains just literal text and nothing
 * else.  If so return a string in allocated memory with what must match after
 * regstart.  Otherwise return NULL.
 */
    স্থির char_u *
nfa_get_match_text(nfa_state_T *start)
{
    nfa_state_T *p = start;
    পূর্ণাঙ্ক		len = 0;
    char_u	*ret;
    char_u	*s;

    যদি (p->c != NFA_MOPEN)
	ঘূৰাই রিক্ত; // just in case
    p = p->out;
    যেতিয়ালৈ (p->c > 0)
    {
	len += MB_CHAR2LEN(p->c);
	p = p->out;
    }
    যদি (p->c != NFA_MCLOSE || p->out->c != NFA_MATCH)
	ঘূৰাই রিক্ত;

    ret = alloc(len);
    যদি (ret == রিক্ত)
	ঘূৰাই রিক্ত;

    p = start->out->out; // skip first char, it goes into regstart
    s = ret;
    যেতিয়ালৈ (p->c > 0)
    {
	যদি (has_mbyte)
	    s += (*mb_char2bytes)(p->c, s);
	অন্যথা
	    *s++ = p->c;
	p = p->out;
    }
    *s = NUL;
    ঘূৰাই ret;
}

/*
 * Allocate more space for post_start.  Called when
 * running above the estimated number of states.
 */
    স্থির পূর্ণাঙ্ক
realloc_post_list(শূন্য)
{
    পূর্ণাঙ্ক   nstate_max = (পূর্ণাঙ্ক)(post_end - post_start);
    পূর্ণাঙ্ক   new_max;
    পূর্ণাঙ্ক   *new_start;
    পূর্ণাঙ্ক	  *old_start;

    // For weird patterns the number of states can be very high. Increasing by
    // 50% seems a reasonable compromise between memory use and speed.
    new_max = nstate_max * 3 / 2;
    new_start = ALLOC_MULT(পূর্ণাঙ্ক, new_max);
    যদি (new_start == রিক্ত)
	ঘূৰাই FAIL;
    mch_memmove(new_start, post_start, nstate_max * পরিমাণ(পূর্ণাঙ্ক));
    old_start = post_start;
    post_start = new_start;
    post_start_len = new_max;
    post_ptr = new_start + (post_ptr - old_start);
    post_end = post_start + new_max;
    vim_free(old_start);
    ঘূৰাই OK;
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
    স্থির পূর্ণাঙ্ক
nfa_recognize_char_class(char_u *start, char_u *end, পূর্ণাঙ্ক extra_newl)
{
    পূর্ণাঙ্ক		newl;
    পূর্ণাঙ্ক		config = get_char_class_bits(start, end, &newl);

    যদি (config < 0)
	ঘূৰাই FAIL;

    যদি (extra_newl == TRUE)
	newl = TRUE;

    যদি (newl == TRUE)
	extra_newl = NFA_ADD_NL;

    ৱিকল্প (config)
    {
	পক্ষ CLASS_o9:
	    ঘূৰাই extra_newl + NFA_DIGIT;
	পক্ষ CLASS_not |  CLASS_o9:
	    ঘূৰাই extra_newl + NFA_NDIGIT;
	পক্ষ CLASS_af | CLASS_AF | CLASS_o9:
	    ঘূৰাই extra_newl + NFA_HEX;
	পক্ষ CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    ঘূৰাই extra_newl + NFA_NHEX;
	পক্ষ CLASS_o7:
	    ঘূৰাই extra_newl + NFA_OCTAL;
	পক্ষ CLASS_not | CLASS_o7:
	    ঘূৰাই extra_newl + NFA_NOCTAL;
	পক্ষ CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    ঘূৰাই extra_newl + NFA_WORD;
	পক্ষ CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    ঘূৰাই extra_newl + NFA_NWORD;
	পক্ষ CLASS_az | CLASS_AZ | CLASS_underscore:
	    ঘূৰাই extra_newl + NFA_HEAD;
	পক্ষ CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    ঘূৰাই extra_newl + NFA_NHEAD;
	পক্ষ CLASS_az | CLASS_AZ:
	    ঘূৰাই extra_newl + NFA_ALPHA;
	পক্ষ CLASS_not | CLASS_az | CLASS_AZ:
	    ঘূৰাই extra_newl + NFA_NALPHA;
	পক্ষ CLASS_az:
	   ঘূৰাই extra_newl + NFA_LOWER_IC;
	পক্ষ CLASS_not | CLASS_az:
	    ঘূৰাই extra_newl + NFA_NLOWER_IC;
	পক্ষ CLASS_AZ:
	    ঘূৰাই extra_newl + NFA_UPPER_IC;
	পক্ষ CLASS_not | CLASS_AZ:
	    ঘূৰাই extra_newl + NFA_NUPPER_IC;
    }
    ঘূৰাই FAIL;
}

/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * Emits bytes in postfix notation: 'a,b,NFA_OR,c,NFA_OR' is
 * equivalent to 'a OR b OR c'
 *
 * NOTE! When changing this function, also update reg_equi_class()
 */
    স্থির পূর্ণাঙ্ক
nfa_emit_equi_class(পূর্ণাঙ্ক c)
{
#define EMIT2(c)    EMIT(c); EMIT(NFA_CONCAT);

    যদি (enc_utf8 || STRCMP(p_enc, "latin1") == 0
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
	ৱিকল্প (c)
	{
	    পক্ষ 'A': পক্ষ A_grave: পক্ষ A_acute: পক্ষ A_circumflex:
	    পক্ষ A_virguilla: পক্ষ A_diaeresis: পক্ষ A_ring:
	    পক্ষ 0x100: পক্ষ 0x102: পক্ষ 0x104: পক্ষ 0x1cd:
	    পক্ষ 0x1de: পক্ষ 0x1e0: পক্ষ 0x1fa: পক্ষ 0x200:
	    পক্ষ 0x202: পক্ষ 0x226: পক্ষ 0x23a: পক্ষ 0x1e00:
	    পক্ষ 0x1ea0: পক্ষ 0x1ea2: পক্ষ 0x1ea4: পক্ষ 0x1ea6:
	    পক্ষ 0x1ea8: পক্ষ 0x1eaa: পক্ষ 0x1eac: পক্ষ 0x1eae:
	    পক্ষ 0x1eb0: পক্ষ 0x1eb2: পক্ষ 0x1eb4: পক্ষ 0x1eb6:
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
		    ঘূৰাই OK;

	    পক্ষ 'B': পক্ষ 0x181: পক্ষ 0x243: পক্ষ 0x1e02:
	    পক্ষ 0x1e04: পক্ষ 0x1e06:
		    EMIT2('B')
		    EMIT2(0x181) EMIT2(0x243) EMIT2(0x1e02)
		    EMIT2(0x1e04) EMIT2(0x1e06)
		    ঘূৰাই OK;

	    পক্ষ 'C': পক্ষ C_cedilla: পক্ষ 0x106: পক্ষ 0x108:
	    পক্ষ 0x10a: পক্ষ 0x10c: পক্ষ 0x187: পক্ষ 0x23b:
	    পক্ষ 0x1e08: পক্ষ 0xa792:
		    EMIT2('C') EMIT2(C_cedilla)
		    EMIT2(0x106) EMIT2(0x108) EMIT2(0x10a)
		    EMIT2(0x10c) EMIT2(0x187) EMIT2(0x23b)
		    EMIT2(0x1e08) EMIT2(0xa792)
		    ঘূৰাই OK;

	    পক্ষ 'D': পক্ষ 0x10e: পক্ষ 0x110: পক্ষ 0x18a:
	    পক্ষ 0x1e0a: পক্ষ 0x1e0c: পক্ষ 0x1e0e: পক্ষ 0x1e10:
	    পক্ষ 0x1e12:
		    EMIT2('D') EMIT2(0x10e) EMIT2(0x110) EMIT2(0x18a)
		    EMIT2(0x1e0a) EMIT2(0x1e0c) EMIT2(0x1e0e)
		    EMIT2(0x1e10) EMIT2(0x1e12)
		    ঘূৰাই OK;

	    পক্ষ 'E': পক্ষ E_grave: পক্ষ E_acute: পক্ষ E_circumflex:
	    পক্ষ E_diaeresis: পক্ষ 0x112: পক্ষ 0x114: পক্ষ 0x116:
	    পক্ষ 0x118: পক্ষ 0x11a: পক্ষ 0x204: পক্ষ 0x206:
	    পক্ষ 0x228: পক্ষ 0x246: পক্ষ 0x1e14: পক্ষ 0x1e16:
	    পক্ষ 0x1e18: পক্ষ 0x1e1a: পক্ষ 0x1e1c: পক্ষ 0x1eb8:
	    পক্ষ 0x1eba: পক্ষ 0x1ebc: পক্ষ 0x1ebe: পক্ষ 0x1ec0:
	    পক্ষ 0x1ec2: পক্ষ 0x1ec4: পক্ষ 0x1ec6:
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
		    ঘূৰাই OK;

	    পক্ষ 'F': পক্ষ 0x191: পক্ষ 0x1e1e: পক্ষ 0xa798:
		    EMIT2('F') EMIT2(0x191) EMIT2(0x1e1e) EMIT2(0xa798)
		    ঘূৰাই OK;

	    পক্ষ 'G': পক্ষ 0x11c: পক্ষ 0x11e: পক্ষ 0x120:
	    পক্ষ 0x122: পক্ষ 0x193: পক্ষ 0x1e4: পক্ষ 0x1e6:
	    পক্ষ 0x1f4: পক্ষ 0x1e20: পক্ষ 0xa7a0:
		    EMIT2('G') EMIT2(0x11c) EMIT2(0x11e) EMIT2(0x120)
		    EMIT2(0x122) EMIT2(0x193) EMIT2(0x1e4)
		    EMIT2(0x1e6) EMIT2(0x1f4) EMIT2(0x1e20)
		    EMIT2(0xa7a0)
		    ঘূৰাই OK;

	    পক্ষ 'H': পক্ষ 0x124: পক্ষ 0x126: পক্ষ 0x21e:
	    পক্ষ 0x1e22: পক্ষ 0x1e24: পক্ষ 0x1e26: পক্ষ 0x1e28:
	    পক্ষ 0x1e2a: পক্ষ 0x2c67:
		    EMIT2('H') EMIT2(0x124) EMIT2(0x126) EMIT2(0x21e)
		    EMIT2(0x1e22) EMIT2(0x1e24) EMIT2(0x1e26)
		    EMIT2(0x1e28) EMIT2(0x1e2a) EMIT2(0x2c67)
		    ঘূৰাই OK;

	    পক্ষ 'I': পক্ষ I_grave: পক্ষ I_acute: পক্ষ I_circumflex:
	    পক্ষ I_diaeresis: পক্ষ 0x128: পক্ষ 0x12a: পক্ষ 0x12c:
	    পক্ষ 0x12e: পক্ষ 0x130: পক্ষ 0x197: পক্ষ 0x1cf:
	    পক্ষ 0x208: পক্ষ 0x20a: পক্ষ 0x1e2c: পক্ষ 0x1e2e:
	    পক্ষ 0x1ec8: পক্ষ 0x1eca:
		    EMIT2('I') EMIT2(I_grave) EMIT2(I_acute)
		    EMIT2(I_circumflex) EMIT2(I_diaeresis)
		    EMIT2(0x128) EMIT2(0x12a) EMIT2(0x12c)
		    EMIT2(0x12e) EMIT2(0x130) EMIT2(0x197)
		    EMIT2(0x1cf) EMIT2(0x208) EMIT2(0x20a)
		    EMIT2(0x1e2c) EMIT2(0x1e2e) EMIT2(0x1ec8)
		    EMIT2(0x1eca)
		    ঘূৰাই OK;

	    পক্ষ 'J': পক্ষ 0x134: পক্ষ 0x248:
		    EMIT2('J') EMIT2(0x134) EMIT2(0x248)
		    ঘূৰাই OK;

	    পক্ষ 'K': পক্ষ 0x136: পক্ষ 0x198: পক্ষ 0x1e8: পক্ষ 0x1e30:
	    পক্ষ 0x1e32: পক্ষ 0x1e34: পক্ষ 0x2c69: পক্ষ 0xa740:
		    EMIT2('K') EMIT2(0x136) EMIT2(0x198) EMIT2(0x1e8)
		    EMIT2(0x1e30) EMIT2(0x1e32) EMIT2(0x1e34)
		    EMIT2(0x2c69) EMIT2(0xa740)
		    ঘূৰাই OK;

	    পক্ষ 'L': পক্ষ 0x139: পক্ষ 0x13b: পক্ষ 0x13d:
	    পক্ষ 0x13f: পক্ষ 0x141: পক্ষ 0x23d: পক্ষ 0x1e36:
	    পক্ষ 0x1e38: পক্ষ 0x1e3a: পক্ষ 0x1e3c: পক্ষ 0x2c60:
		    EMIT2('L') EMIT2(0x139) EMIT2(0x13b)
		    EMIT2(0x13d) EMIT2(0x13f) EMIT2(0x141)
		    EMIT2(0x23d) EMIT2(0x1e36) EMIT2(0x1e38)
		    EMIT2(0x1e3a) EMIT2(0x1e3c) EMIT2(0x2c60)
		    ঘূৰাই OK;

	    পক্ষ 'M': পক্ষ 0x1e3e: পক্ষ 0x1e40: পক্ষ 0x1e42:
		    EMIT2('M') EMIT2(0x1e3e) EMIT2(0x1e40)
		    EMIT2(0x1e42)
		    ঘূৰাই OK;

	    পক্ষ 'N': পক্ষ N_virguilla:
	    পক্ষ 0x143: পক্ষ 0x145: পক্ষ 0x147: পক্ষ 0x1f8:
	    পক্ষ 0x1e44: পক্ষ 0x1e46: পক্ষ 0x1e48: পক্ষ 0x1e4a:
	    পক্ষ 0xa7a4:
		    EMIT2('N') EMIT2(N_virguilla)
		    EMIT2(0x143) EMIT2(0x145) EMIT2(0x147)
		    EMIT2(0x1f8) EMIT2(0x1e44) EMIT2(0x1e46)
		    EMIT2(0x1e48) EMIT2(0x1e4a) EMIT2(0xa7a4)
		    ঘূৰাই OK;

	    পক্ষ 'O': পক্ষ O_grave: পক্ষ O_acute: পক্ষ O_circumflex:
	    পক্ষ O_virguilla: পক্ষ O_diaeresis: পক্ষ O_slash:
	    পক্ষ 0x14c: পক্ষ 0x14e: পক্ষ 0x150: পক্ষ 0x19f:
	    পক্ষ 0x1a0: পক্ষ 0x1d1: পক্ষ 0x1ea: পক্ষ 0x1ec:
	    পক্ষ 0x1fe: পক্ষ 0x20c: পক্ষ 0x20e: পক্ষ 0x22a:
	    পক্ষ 0x22c: পক্ষ 0x22e: পক্ষ 0x230: পক্ষ 0x1e4c:
	    পক্ষ 0x1e4e: পক্ষ 0x1e50: পক্ষ 0x1e52: পক্ষ 0x1ecc:
	    পক্ষ 0x1ece: পক্ষ 0x1ed0: পক্ষ 0x1ed2: পক্ষ 0x1ed4:
	    পক্ষ 0x1ed6: পক্ষ 0x1ed8: পক্ষ 0x1eda: পক্ষ 0x1edc:
	    পক্ষ 0x1ede: পক্ষ 0x1ee0: পক্ষ 0x1ee2:
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
		    ঘূৰাই OK;

	    পক্ষ 'P': পক্ষ 0x1a4: পক্ষ 0x1e54: পক্ষ 0x1e56: পক্ষ 0x2c63:
		    EMIT2('P') EMIT2(0x1a4) EMIT2(0x1e54) EMIT2(0x1e56)
		    EMIT2(0x2c63)
		    ঘূৰাই OK;

	    পক্ষ 'Q': পক্ষ 0x24a:
		    EMIT2('Q') EMIT2(0x24a)
		    ঘূৰাই OK;

	    পক্ষ 'R': পক্ষ 0x154: পক্ষ 0x156: পক্ষ 0x158: পক্ষ 0x210:
	    পক্ষ 0x212: পক্ষ 0x24c: পক্ষ 0x1e58: পক্ষ 0x1e5a:
	    পক্ষ 0x1e5c: পক্ষ 0x1e5e: পক্ষ 0x2c64: পক্ষ 0xa7a6:
		    EMIT2('R') EMIT2(0x154) EMIT2(0x156) EMIT2(0x158)
		    EMIT2(0x210) EMIT2(0x212) EMIT2(0x24c) EMIT2(0x1e58)
		    EMIT2(0x1e5a) EMIT2(0x1e5c) EMIT2(0x1e5e) EMIT2(0x2c64)
		    EMIT2(0xa7a6)
		    ঘূৰাই OK;

	    পক্ষ 'S': পক্ষ 0x15a: পক্ষ 0x15c: পক্ষ 0x15e: পক্ষ 0x160:
	    পক্ষ 0x218: পক্ষ 0x1e60: পক্ষ 0x1e62: পক্ষ 0x1e64:
	    পক্ষ 0x1e66: পক্ষ 0x1e68: পক্ষ 0x2c7e: পক্ষ 0xa7a8:
		    EMIT2('S') EMIT2(0x15a) EMIT2(0x15c) EMIT2(0x15e)
		    EMIT2(0x160) EMIT2(0x218) EMIT2(0x1e60) EMIT2(0x1e62)
		    EMIT2(0x1e64) EMIT2(0x1e66) EMIT2(0x1e68) EMIT2(0x2c7e)
		    EMIT2(0xa7a8)
		    ঘূৰাই OK;

	    পক্ষ 'T': পক্ষ 0x162: পক্ষ 0x164: পক্ষ 0x166: পক্ষ 0x1ac:
	    পক্ষ 0x1ae: পক্ষ 0x21a: পক্ষ 0x23e: পক্ষ 0x1e6a: পক্ষ 0x1e6c:
	    পক্ষ 0x1e6e: পক্ষ 0x1e70:
		    EMIT2('T') EMIT2(0x162) EMIT2(0x164) EMIT2(0x166)
		    EMIT2(0x1ac) EMIT2(0x1ae) EMIT2(0x23e) EMIT2(0x21a)
		    EMIT2(0x1e6a) EMIT2(0x1e6c) EMIT2(0x1e6e) EMIT2(0x1e70)
		    ঘূৰাই OK;

	    পক্ষ 'U': পক্ষ U_grave: পক্ষ U_acute: পক্ষ U_diaeresis:
	    পক্ষ U_circumflex: পক্ষ 0x168: পক্ষ 0x16a: পক্ষ 0x16c:
	    পক্ষ 0x16e: পক্ষ 0x170: পক্ষ 0x172: পক্ষ 0x1af:
	    পক্ষ 0x1d3: পক্ষ 0x1d5: পক্ষ 0x1d7: পক্ষ 0x1d9:
	    পক্ষ 0x1db: পক্ষ 0x214: পক্ষ 0x216: পক্ষ 0x244:
	    পক্ষ 0x1e72: পক্ষ 0x1e74: পক্ষ 0x1e76: পক্ষ 0x1e78:
	    পক্ষ 0x1e7a: পক্ষ 0x1ee4: পক্ষ 0x1ee6: পক্ষ 0x1ee8:
	    পক্ষ 0x1eea: পক্ষ 0x1eec: পক্ষ 0x1eee: পক্ষ 0x1ef0:
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
		    ঘূৰাই OK;

	    পক্ষ 'V': পক্ষ 0x1b2: পক্ষ 0x1e7c: পক্ষ 0x1e7e:
		    EMIT2('V') EMIT2(0x1b2) EMIT2(0x1e7c) EMIT2(0x1e7e)
		    ঘূৰাই OK;

	    পক্ষ 'W': পক্ষ 0x174: পক্ষ 0x1e80: পক্ষ 0x1e82: পক্ষ 0x1e84:
	    পক্ষ 0x1e86: পক্ষ 0x1e88:
		    EMIT2('W') EMIT2(0x174) EMIT2(0x1e80) EMIT2(0x1e82)
		    EMIT2(0x1e84) EMIT2(0x1e86) EMIT2(0x1e88)
		    ঘূৰাই OK;

	    পক্ষ 'X': পক্ষ 0x1e8a: পক্ষ 0x1e8c:
		    EMIT2('X') EMIT2(0x1e8a) EMIT2(0x1e8c)
		    ঘূৰাই OK;

	    পক্ষ 'Y': পক্ষ Y_acute: পক্ষ 0x176: পক্ষ 0x178:
	    পক্ষ 0x1b3: পক্ষ 0x232: পক্ষ 0x24e: পক্ষ 0x1e8e:
	    পক্ষ 0x1ef2: পক্ষ 0x1ef4: পক্ষ 0x1ef6: পক্ষ 0x1ef8:
		    EMIT2('Y') EMIT2(Y_acute)
		    EMIT2(0x176) EMIT2(0x178) EMIT2(0x1b3)
		    EMIT2(0x232) EMIT2(0x24e) EMIT2(0x1e8e)
		    EMIT2(0x1ef2) EMIT2(0x1ef4) EMIT2(0x1ef6)
		    EMIT2(0x1ef8)
		    ঘূৰাই OK;

	    পক্ষ 'Z': পক্ষ 0x179: পক্ষ 0x17b: পক্ষ 0x17d:
	    পক্ষ 0x1b5: পক্ষ 0x1e90: পক্ষ 0x1e92: পক্ষ 0x1e94:
	    পক্ষ 0x2c6b:
		    EMIT2('Z') EMIT2(0x179) EMIT2(0x17b) EMIT2(0x17d)
		    EMIT2(0x1b5) EMIT2(0x1e90) EMIT2(0x1e92)
		    EMIT2(0x1e94) EMIT2(0x2c6b)
		    ঘূৰাই OK;

	    পক্ষ 'a': পক্ষ a_grave: পক্ষ a_acute: পক্ষ a_circumflex:
	    পক্ষ a_virguilla: পক্ষ a_diaeresis: পক্ষ a_ring:
	    পক্ষ 0x101: পক্ষ 0x103: পক্ষ 0x105: পক্ষ 0x1ce:
	    পক্ষ 0x1df: পক্ষ 0x1e1: পক্ষ 0x1fb: পক্ষ 0x201:
	    পক্ষ 0x203: পক্ষ 0x227: পক্ষ 0x1d8f: পক্ষ 0x1e01:
	    পক্ষ 0x1e9a: পক্ষ 0x1ea1: পক্ষ 0x1ea3: পক্ষ 0x1ea5:
	    পক্ষ 0x1ea7: পক্ষ 0x1ea9: পক্ষ 0x1eab: পক্ষ 0x1ead:
	    পক্ষ 0x1eaf: পক্ষ 0x1eb1: পক্ষ 0x1eb3: পক্ষ 0x1eb5:
	    পক্ষ 0x1eb7: পক্ষ 0x2c65:
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
		    ঘূৰাই OK;

	    পক্ষ 'b': পক্ষ 0x180: পক্ষ 0x253: পক্ষ 0x1d6c: পক্ষ 0x1d80:
	    পক্ষ 0x1e03: পক্ষ 0x1e05: পক্ষ 0x1e07:
		    EMIT2('b') EMIT2(0x180) EMIT2(0x253) EMIT2(0x1d6c)
		    EMIT2(0x1d80) EMIT2(0x1e03) EMIT2(0x1e05) EMIT2(0x1e07)
		    ঘূৰাই OK;

	    পক্ষ 'c': পক্ষ c_cedilla: পক্ষ 0x107: পক্ষ 0x109: পক্ষ 0x10b:
	    পক্ষ 0x10d: পক্ষ 0x188: পক্ষ 0x23c: পক্ষ 0x1e09: পক্ষ 0xa793:
	    পক্ষ 0xa794:
		    EMIT2('c') EMIT2(c_cedilla)
		    EMIT2(0x107) EMIT2(0x109) EMIT2(0x10b)
		    EMIT2(0x10d) EMIT2(0x188) EMIT2(0x23c)
		    EMIT2(0x1e09) EMIT2(0xa793) EMIT2(0xa794)
		    ঘূৰাই OK;

	    পক্ষ 'd': পক্ষ 0x10f: পক্ষ 0x111: পক্ষ 0x257: পক্ষ 0x1d6d:
	    পক্ষ 0x1d81: পক্ষ 0x1d91: পক্ষ 0x1e0b: পক্ষ 0x1e0d: পক্ষ 0x1e0f:
	    পক্ষ 0x1e11: পক্ষ 0x1e13:
		    EMIT2('d') EMIT2(0x10f) EMIT2(0x111)
		    EMIT2(0x257) EMIT2(0x1d6d) EMIT2(0x1d81)
		    EMIT2(0x1d91) EMIT2(0x1e0b) EMIT2(0x1e0d)
		    EMIT2(0x1e0f) EMIT2(0x1e11) EMIT2(0x1e13)
		    ঘূৰাই OK;

	    পক্ষ 'e': পক্ষ e_grave: পক্ষ e_acute: পক্ষ e_circumflex:
	    পক্ষ e_diaeresis: পক্ষ 0x113: পক্ষ 0x115: পক্ষ 0x117:
	    পক্ষ 0x119: পক্ষ 0x11b: পক্ষ 0x205: পক্ষ 0x207:
	    পক্ষ 0x229: পক্ষ 0x247: পক্ষ 0x1d92: পক্ষ 0x1e15:
	    পক্ষ 0x1e17: পক্ষ 0x1e19: পক্ষ 0x1e1b: পক্ষ 0x1e1d:
	    পক্ষ 0x1eb9: পক্ষ 0x1ebb: পক্ষ 0x1ebd: পক্ষ 0x1ebf:
	    পক্ষ 0x1ec1: পক্ষ 0x1ec3: পক্ষ 0x1ec5: পক্ষ 0x1ec7:
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
		    ঘূৰাই OK;

	    পক্ষ 'f': পক্ষ 0x192: পক্ষ 0x1d6e: পক্ষ 0x1d82:
	    পক্ষ 0x1e1f: পক্ষ 0xa799:
		    EMIT2('f') EMIT2(0x192) EMIT2(0x1d6e) EMIT2(0x1d82)
		    EMIT2(0x1e1f) EMIT2(0xa799)
		    ঘূৰাই OK;

	    পক্ষ 'g': পক্ষ 0x11d: পক্ষ 0x11f: পক্ষ 0x121: পক্ষ 0x123:
	    পক্ষ 0x1e5: পক্ষ 0x1e7: পক্ষ 0x1f5: পক্ষ 0x260: পক্ষ 0x1d83:
	    পক্ষ 0x1e21: পক্ষ 0xa7a1:
		    EMIT2('g') EMIT2(0x11d) EMIT2(0x11f) EMIT2(0x121)
		    EMIT2(0x123) EMIT2(0x1e5) EMIT2(0x1e7)
		    EMIT2(0x1f5) EMIT2(0x260) EMIT2(0x1d83)
		    EMIT2(0x1e21) EMIT2(0xa7a1)
		    ঘূৰাই OK;

	    পক্ষ 'h': পক্ষ 0x125: পক্ষ 0x127: পক্ষ 0x21f: পক্ষ 0x1e23:
	    পক্ষ 0x1e25: পক্ষ 0x1e27: পক্ষ 0x1e29: পক্ষ 0x1e2b:
	    পক্ষ 0x1e96: পক্ষ 0x2c68: পক্ষ 0xa795:
		    EMIT2('h') EMIT2(0x125) EMIT2(0x127) EMIT2(0x21f)
		    EMIT2(0x1e23) EMIT2(0x1e25) EMIT2(0x1e27)
		    EMIT2(0x1e29) EMIT2(0x1e2b) EMIT2(0x1e96)
		    EMIT2(0x2c68) EMIT2(0xa795)
		    ঘূৰাই OK;

	    পক্ষ 'i': পক্ষ i_grave: পক্ষ i_acute: পক্ষ i_circumflex:
	    পক্ষ i_diaeresis: পক্ষ 0x129: পক্ষ 0x12b: পক্ষ 0x12d:
	    পক্ষ 0x12f: পক্ষ 0x1d0: পক্ষ 0x209: পক্ষ 0x20b:
	    পক্ষ 0x268: পক্ষ 0x1d96: পক্ষ 0x1e2d: পক্ষ 0x1e2f:
	    পক্ষ 0x1ec9: পক্ষ 0x1ecb:
		    EMIT2('i') EMIT2(i_grave) EMIT2(i_acute)
		    EMIT2(i_circumflex) EMIT2(i_diaeresis)
		    EMIT2(0x129) EMIT2(0x12b) EMIT2(0x12d)
		    EMIT2(0x12f) EMIT2(0x1d0) EMIT2(0x209)
		    EMIT2(0x20b) EMIT2(0x268) EMIT2(0x1d96)
		    EMIT2(0x1e2d) EMIT2(0x1e2f) EMIT2(0x1ec9)
		    EMIT2(0x1ecb)
		    ঘূৰাই OK;

	    পক্ষ 'j': পক্ষ 0x135: পক্ষ 0x1f0: পক্ষ 0x249:
		    EMIT2('j') EMIT2(0x135) EMIT2(0x1f0) EMIT2(0x249)
		    ঘূৰাই OK;

	    পক্ষ 'k': পক্ষ 0x137: পক্ষ 0x199: পক্ষ 0x1e9: পক্ষ 0x1d84:
	    পক্ষ 0x1e31: পক্ষ 0x1e33: পক্ষ 0x1e35: পক্ষ 0x2c6a: পক্ষ 0xa741:
		    EMIT2('k') EMIT2(0x137) EMIT2(0x199) EMIT2(0x1e9)
		    EMIT2(0x1d84) EMIT2(0x1e31) EMIT2(0x1e33)
		    EMIT2(0x1e35) EMIT2(0x2c6a) EMIT2(0xa741)
		    ঘূৰাই OK;

	    পক্ষ 'l': পক্ষ 0x13a: পক্ষ 0x13c: পক্ষ 0x13e: পক্ষ 0x140:
	    পক্ষ 0x142: পক্ষ 0x19a: পক্ষ 0x1e37: পক্ষ 0x1e39: পক্ষ 0x1e3b:
	    পক্ষ 0x1e3d: পক্ষ 0x2c61:
		    EMIT2('l') EMIT2(0x13a) EMIT2(0x13c)
		    EMIT2(0x13e) EMIT2(0x140) EMIT2(0x142)
		    EMIT2(0x19a) EMIT2(0x1e37) EMIT2(0x1e39)
		    EMIT2(0x1e3b) EMIT2(0x1e3d) EMIT2(0x2c61)
		    ঘূৰাই OK;

	    পক্ষ 'm': পক্ষ 0x1d6f: পক্ষ 0x1e3f: পক্ষ 0x1e41: পক্ষ 0x1e43:
		    EMIT2('m') EMIT2(0x1d6f) EMIT2(0x1e3f)
		    EMIT2(0x1e41) EMIT2(0x1e43)
		    ঘূৰাই OK;

	    পক্ষ 'n': পক্ষ n_virguilla: পক্ষ 0x144: পক্ষ 0x146: পক্ষ 0x148:
	    পক্ষ 0x149: পক্ষ 0x1f9: পক্ষ 0x1d70: পক্ষ 0x1d87: পক্ষ 0x1e45:
	    পক্ষ 0x1e47: পক্ষ 0x1e49: পক্ষ 0x1e4b: পক্ষ 0xa7a5:
		    EMIT2('n') EMIT2(n_virguilla)
		    EMIT2(0x144) EMIT2(0x146) EMIT2(0x148)
		    EMIT2(0x149) EMIT2(0x1f9) EMIT2(0x1d70)
		    EMIT2(0x1d87) EMIT2(0x1e45) EMIT2(0x1e47)
		    EMIT2(0x1e49) EMIT2(0x1e4b) EMIT2(0xa7a5)
		    ঘূৰাই OK;

	    পক্ষ 'o': পক্ষ o_grave: পক্ষ o_acute: পক্ষ o_circumflex:
	    পক্ষ o_virguilla: পক্ষ o_diaeresis: পক্ষ o_slash:
	    পক্ষ 0x14d: পক্ষ 0x14f: পক্ষ 0x151: পক্ষ 0x1a1:
	    পক্ষ 0x1d2: পক্ষ 0x1eb: পক্ষ 0x1ed: পক্ষ 0x1ff:
	    পক্ষ 0x20d: পক্ষ 0x20f: পক্ষ 0x22b: পক্ষ 0x22d:
	    পক্ষ 0x22f: পক্ষ 0x231: পক্ষ 0x275: পক্ষ 0x1e4d:
	    পক্ষ 0x1e4f: পক্ষ 0x1e51: পক্ষ 0x1e53: পক্ষ 0x1ecd:
	    পক্ষ 0x1ecf: পক্ষ 0x1ed1: পক্ষ 0x1ed3: পক্ষ 0x1ed5:
	    পক্ষ 0x1ed7: পক্ষ 0x1ed9: পক্ষ 0x1edb: পক্ষ 0x1edd:
	    পক্ষ 0x1edf: পক্ষ 0x1ee1: পক্ষ 0x1ee3:
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
		    ঘূৰাই OK;

	    পক্ষ 'p': পক্ষ 0x1a5: পক্ষ 0x1d71: পক্ষ 0x1d7d: পক্ষ 0x1d88:
	    পক্ষ 0x1e55: পক্ষ 0x1e57:
		    EMIT2('p') EMIT2(0x1a5) EMIT2(0x1d71) EMIT2(0x1d7d)
		    EMIT2(0x1d88) EMIT2(0x1e55) EMIT2(0x1e57)
		    ঘূৰাই OK;

	    পক্ষ 'q': পক্ষ 0x24b: পক্ষ 0x2a0:
		    EMIT2('q') EMIT2(0x24b) EMIT2(0x2a0)
		    ঘূৰাই OK;

	    পক্ষ 'r': পক্ষ 0x155: পক্ষ 0x157: পক্ষ 0x159: পক্ষ 0x211:
	    পক্ষ 0x213: পক্ষ 0x24d: পক্ষ 0x27d: পক্ষ 0x1d72: পক্ষ 0x1d73:
	    পক্ষ 0x1d89: পক্ষ 0x1e59: পক্ষ 0x1e5b: পক্ষ 0x1e5d: পক্ষ 0x1e5f:
	    পক্ষ 0xa7a7:
		    EMIT2('r') EMIT2(0x155) EMIT2(0x157) EMIT2(0x159)
		    EMIT2(0x211) EMIT2(0x213) EMIT2(0x24d) EMIT2(0x27d)
		    EMIT2(0x1d72) EMIT2(0x1d73) EMIT2(0x1d89) EMIT2(0x1e59)
		    EMIT2(0x1e5b) EMIT2(0x1e5d) EMIT2(0x1e5f) EMIT2(0xa7a7)
		    ঘূৰাই OK;

	    পক্ষ 's': পক্ষ 0x15b: পক্ষ 0x15d: পক্ষ 0x15f: পক্ষ 0x161:
	    পক্ষ 0x219: পক্ষ 0x23f: পক্ষ 0x1d74: পক্ষ 0x1d8a: পক্ষ 0x1e61:
	    পক্ষ 0x1e63: পক্ষ 0x1e65: পক্ষ 0x1e67: পক্ষ 0x1e69: পক্ষ 0xa7a9:
		    EMIT2('s') EMIT2(0x15b) EMIT2(0x15d) EMIT2(0x15f)
		    EMIT2(0x161) EMIT2(0x219) EMIT2(0x23f) EMIT2(0x1d74)
		    EMIT2(0x1d8a) EMIT2(0x1e61) EMIT2(0x1e63) EMIT2(0x1e65)
		    EMIT2(0x1e67) EMIT2(0x1e69) EMIT2(0xa7a9)
		    ঘূৰাই OK;

	    পক্ষ 't': পক্ষ 0x163: পক্ষ 0x165: পক্ষ 0x167: পক্ষ 0x1ab:
	    পক্ষ 0x1ad: পক্ষ 0x21b: পক্ষ 0x288: পক্ষ 0x1d75: পক্ষ 0x1e6b:
	    পক্ষ 0x1e6d: পক্ষ 0x1e6f: পক্ষ 0x1e71: পক্ষ 0x1e97: পক্ষ 0x2c66:
		    EMIT2('t') EMIT2(0x163) EMIT2(0x165) EMIT2(0x167)
		    EMIT2(0x1ab) EMIT2(0x1ad) EMIT2(0x21b) EMIT2(0x288)
		    EMIT2(0x1d75) EMIT2(0x1e6b) EMIT2(0x1e6d) EMIT2(0x1e6f)
		    EMIT2(0x1e71) EMIT2(0x1e97) EMIT2(0x2c66)
		    ঘূৰাই OK;

	    পক্ষ 'u': পক্ষ u_grave: পক্ষ u_acute: পক্ষ u_circumflex:
	    পক্ষ u_diaeresis: পক্ষ 0x169: পক্ষ 0x16b: পক্ষ 0x16d:
	    পক্ষ 0x16f: পক্ষ 0x171: পক্ষ 0x173: পক্ষ 0x1b0: পক্ষ 0x1d4:
	    পক্ষ 0x1d6: পক্ষ 0x1d8: পক্ষ 0x1da: পক্ষ 0x1dc: পক্ষ 0x215:
	    পক্ষ 0x217: পক্ষ 0x289: পক্ষ 0x1d7e: পক্ষ 0x1d99: পক্ষ 0x1e73:
	    পক্ষ 0x1e75: পক্ষ 0x1e77: পক্ষ 0x1e79: পক্ষ 0x1e7b:
	    পক্ষ 0x1ee5: পক্ষ 0x1ee7: পক্ষ 0x1ee9: পক্ষ 0x1eeb:
	    পক্ষ 0x1eed: পক্ষ 0x1eef: পক্ষ 0x1ef1:
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
		    ঘূৰাই OK;

	    পক্ষ 'v': পক্ষ 0x28b: পক্ষ 0x1d8c: পক্ষ 0x1e7d: পক্ষ 0x1e7f:
		    EMIT2('v') EMIT2(0x28b) EMIT2(0x1d8c) EMIT2(0x1e7d)
		    EMIT2(0x1e7f)
		    ঘূৰাই OK;

	    পক্ষ 'w': পক্ষ 0x175: পক্ষ 0x1e81: পক্ষ 0x1e83: পক্ষ 0x1e85:
	    পক্ষ 0x1e87: পক্ষ 0x1e89: পক্ষ 0x1e98:
		    EMIT2('w') EMIT2(0x175) EMIT2(0x1e81) EMIT2(0x1e83)
		    EMIT2(0x1e85) EMIT2(0x1e87) EMIT2(0x1e89) EMIT2(0x1e98)
		    ঘূৰাই OK;

	    পক্ষ 'x': পক্ষ 0x1e8b: পক্ষ 0x1e8d:
		    EMIT2('x') EMIT2(0x1e8b) EMIT2(0x1e8d)
		    ঘূৰাই OK;

	    পক্ষ 'y': পক্ষ y_acute: পক্ষ y_diaeresis: পক্ষ 0x177:
	    পক্ষ 0x1b4: পক্ষ 0x233: পক্ষ 0x24f: পক্ষ 0x1e8f:
	    পক্ষ 0x1e99: পক্ষ 0x1ef3: পক্ষ 0x1ef5: পক্ষ 0x1ef7:
	    পক্ষ 0x1ef9:
		    EMIT2('y') EMIT2(y_acute) EMIT2(y_diaeresis)
		    EMIT2(0x177) EMIT2(0x1b4) EMIT2(0x233) EMIT2(0x24f)
		    EMIT2(0x1e8f) EMIT2(0x1e99) EMIT2(0x1ef3)
		    EMIT2(0x1ef5) EMIT2(0x1ef7) EMIT2(0x1ef9)
		    ঘূৰাই OK;

	    পক্ষ 'z': পক্ষ 0x17a: পক্ষ 0x17c: পক্ষ 0x17e: পক্ষ 0x1b6:
	    পক্ষ 0x1d76: পক্ষ 0x1d8e: পক্ষ 0x1e91: পক্ষ 0x1e93:
	    পক্ষ 0x1e95: পক্ষ 0x2c6c:
		    EMIT2('z') EMIT2(0x17a) EMIT2(0x17c) EMIT2(0x17e)
		    EMIT2(0x1b6) EMIT2(0x1d76) EMIT2(0x1d8e) EMIT2(0x1e91)
		    EMIT2(0x1e93) EMIT2(0x1e95) EMIT2(0x2c6c)
		    ঘূৰাই OK;

	    // default: character itself
	}
    }

    EMIT2(c);
    ঘূৰাই OK;
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
    স্থির পূর্ণাঙ্ক
nfa_regatom(শূন্য)
{
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		charclass;
    পূর্ণাঙ্ক		equiclass;
    পূর্ণাঙ্ক		collclass;
    পূর্ণাঙ্ক		got_coll_char;
    char_u	*p;
    char_u	*endp;
    char_u	*old_regparse = regparse;
    পূর্ণাঙ্ক		extra = 0;
    পূর্ণাঙ্ক		emit_range;
    পূর্ণাঙ্ক		negated;
    পূর্ণাঙ্ক		result;
    পূর্ণাঙ্ক		startc = -1;
    পূর্ণাঙ্ক		save_prev_at_start = prev_at_start;

    c = getchr();
    ৱিকল্প (c)
    {
	পক্ষ NUL:
	    EMSG_RET_FAIL(_(e_nfa_regexp_end_encountered_prematurely));

	পক্ষ Magic('^'):
	    EMIT(NFA_BOL);
	    ভঙ্গ;

	পক্ষ Magic('$'):
	    EMIT(NFA_EOL);
#যদি defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#endif
	    ভঙ্গ;

	পক্ষ Magic('<'):
	    EMIT(NFA_BOW);
	    ভঙ্গ;

	পক্ষ Magic('>'):
	    EMIT(NFA_EOW);
	    ভঙ্গ;

	পক্ষ Magic('_'):
	    c = no_Magic(getchr());
	    যদি (c == NUL)
		EMSG_RET_FAIL(_(e_nfa_regexp_end_encountered_prematurely));

	    যদি (c == '^')	// "\_^" is start-of-line
	    {
		EMIT(NFA_BOL);
		ভঙ্গ;
	    }
	    যদি (c == '$')	// "\_$" is end-of-line
	    {
		EMIT(NFA_EOL);
#যদি defined(FEAT_SYN_HL)
		had_eol = TRUE;
#endif
		ভঙ্গ;
	    }

	    extra = NFA_ADD_NL;

	    // "\_[" is collection plus newline
	    যদি (c == '[')
		গচ্ছ collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	/*
	 * Character classes.
	 */
	পক্ষ Magic('.'):
	পক্ষ Magic('i'):
	পক্ষ Magic('I'):
	পক্ষ Magic('k'):
	পক্ষ Magic('K'):
	পক্ষ Magic('f'):
	পক্ষ Magic('F'):
	পক্ষ Magic('p'):
	পক্ষ Magic('P'):
	পক্ষ Magic('s'):
	পক্ষ Magic('S'):
	পক্ষ Magic('d'):
	পক্ষ Magic('D'):
	পক্ষ Magic('x'):
	পক্ষ Magic('X'):
	পক্ষ Magic('o'):
	পক্ষ Magic('O'):
	পক্ষ Magic('w'):
	পক্ষ Magic('W'):
	পক্ষ Magic('h'):
	পক্ষ Magic('H'):
	পক্ষ Magic('a'):
	পক্ষ Magic('A'):
	পক্ষ Magic('l'):
	পক্ষ Magic('L'):
	পক্ষ Magic('u'):
	পক্ষ Magic('U'):
	    p = vim_strchr(classchars, no_Magic(c));
	    যদি (p == রিক্ত)
	    {
		যদি (extra == NFA_ADD_NL)
		{
		    semsg(_(e_nfa_regexp_invalid_character_class_nr), c);
		    rc_did_emsg = TRUE;
		    ঘূৰাই FAIL;
		}
		siemsg("Unknown character class char: %d", c);
		ঘূৰাই FAIL;
	    }

	    // When '.' is followed by a composing char ignore the dot, so that
	    // the composing char is matched here.
	    যদি (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	    {
		old_regparse = regparse;
		c = getchr();
		গচ্ছ nfa_do_multibyte;
	    }
	    EMIT(nfa_classcodes[p - classchars]);
	    যদি (extra == NFA_ADD_NL)
	    {
		EMIT(NFA_NEWL);
		EMIT(NFA_OR);
		regflags |= RF_HASNL;
	    }
	    ভঙ্গ;

	পক্ষ Magic('n'):
	    যদি (reg_string)
		// In a string "\n" matches a newline character.
		EMIT(NL);
	    অন্যথা
	    {
		// In buffer text "\n" matches the end of a line.
		EMIT(NFA_NEWL);
		regflags |= RF_HASNL;
	    }
	    ভঙ্গ;

	পক্ষ Magic('('):
	    যদি (nfa_reg(REG_PAREN) == FAIL)
		ঘূৰাই FAIL;	    // cascaded error
	    ভঙ্গ;

	পক্ষ Magic('|'):
	পক্ষ Magic('&'):
	পক্ষ Magic(')'):
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    ঘূৰাই FAIL;

	পক্ষ Magic('='):
	পক্ষ Magic('?'):
	পক্ষ Magic('+'):
	পক্ষ Magic('@'):
	পক্ষ Magic('*'):
	পক্ষ Magic('{'):
	    // these should follow an atom, not form an atom
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    ঘূৰাই FAIL;

	পক্ষ Magic('~'):
	    {
		char_u	    *lp;

		// Previous substitute pattern.
		// Generated as "\%(pattern\)".
		যদি (reg_prev_sub == রিক্ত)
		{
		    emsg(_(e_no_previous_substitute_regular_expression));
		    ঘূৰাই FAIL;
		}
		বাবে (lp = reg_prev_sub; *lp != NUL; MB_CPTR_ADV(lp))
		{
		    EMIT(PTR2CHAR(lp));
		    যদি (lp != reg_prev_sub)
			EMIT(NFA_CONCAT);
		}
		EMIT(NFA_NOPEN);
		ভঙ্গ;
	    }

	পক্ষ Magic('1'):
	পক্ষ Magic('2'):
	পক্ষ Magic('3'):
	পক্ষ Magic('4'):
	পক্ষ Magic('5'):
	পক্ষ Magic('6'):
	পক্ষ Magic('7'):
	পক্ষ Magic('8'):
	পক্ষ Magic('9'):
	    {
		পূর্ণাঙ্ক refnum = no_Magic(c) - '1';

		যদি (!seen_endbrace(refnum + 1))
		    ঘূৰাই FAIL;
		EMIT(NFA_BACKREF1 + refnum);
		rex.nfa_has_backref = TRUE;
	    }
	    ভঙ্গ;

	পক্ষ Magic('z'):
	    c = no_Magic(getchr());
	    ৱিকল্প (c)
	    {
		পক্ষ 's':
		    EMIT(NFA_ZSTART);
		    যদি (re_mult_next("\\zs") == FAIL)
			ঘূৰাই FAIL;
		    ভঙ্গ;
		পক্ষ 'e':
		    EMIT(NFA_ZEND);
		    rex.nfa_has_zend = TRUE;
		    যদি (re_mult_next("\\ze") == FAIL)
			ঘূৰাই FAIL;
		    ভঙ্গ;
#ifdef FEAT_SYN_HL
		পক্ষ '1':
		পক্ষ '2':
		পক্ষ '3':
		পক্ষ '4':
		পক্ষ '5':
		পক্ষ '6':
		পক্ষ '7':
		পক্ষ '8':
		পক্ষ '9':
		    // \z1...\z9
		    যদি ((reg_do_extmatch & REX_USE) == 0)
			EMSG_RET_FAIL(_(e_z1_z9_not_allowed_here));
		    EMIT(NFA_ZREF1 + (no_Magic(c) - '1'));
		    // No need to set rex.nfa_has_backref, the sub-matches don't
		    // change when \z1 .. \z9 matches or not.
		    re_has_z = REX_USE;
		    ভঙ্গ;
		পক্ষ '(':
		    // \z(
		    যদি ((reg_do_extmatch & REX_SET) == 0)
			EMSG_RET_FAIL(_(e_z_not_allowed_here));
		    যদি (nfa_reg(REG_ZPAREN) == FAIL)
			ঘূৰাই FAIL;	    // cascaded error
		    re_has_z = REX_SET;
		    ভঙ্গ;
#endif
		স্ৱভাৱ:
		    semsg(_(e_nfa_regexp_unknown_operator_z_chr), no_Magic(c));
		    ঘূৰাই FAIL;
	    }
	    ভঙ্গ;

	পক্ষ Magic('%'):
	    c = no_Magic(getchr());
	    ৱিকল্প (c)
	    {
		// () without a back reference
		পক্ষ '(':
		    যদি (nfa_reg(REG_NPAREN) == FAIL)
			ঘূৰাই FAIL;
		    EMIT(NFA_NOPEN);
		    ভঙ্গ;

		পক্ষ 'd':   // %d123 decimal
		পক্ষ 'o':   // %o123 octal
		পক্ষ 'x':   // %xab hex 2
		পক্ষ 'u':   // %uabcd hex 4
		পক্ষ 'U':   // %U1234abcd hex 8
		    {
			vimlong_T nr;

			ৱিকল্প (c)
			{
			    পক্ষ 'd': nr = getdecchrs(); ভঙ্গ;
			    পক্ষ 'o': nr = getoctchrs(); ভঙ্গ;
			    পক্ষ 'x': nr = gethexchrs(2); ভঙ্গ;
			    পক্ষ 'u': nr = gethexchrs(4); ভঙ্গ;
			    পক্ষ 'U': nr = gethexchrs(8); ভঙ্গ;
			    স্ৱভাৱ:  nr = -1; ভঙ্গ;
			}

			যদি (nr < 0 || nr > INT_MAX)
			    EMSG2_RET_FAIL(_(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			// A NUL is stored in the text as NL
			// TODO: what if a composing character follows?
			EMIT(nr == 0 ? 0x0a : (long)nr);
		    }
		    ভঙ্গ;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		পক্ষ '^':
		    EMIT(NFA_BOF);
		    ভঙ্গ;

		পক্ষ '$':
		    EMIT(NFA_EOF);
		    ভঙ্গ;

		পক্ষ '#':
		    যদি (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_start_of_pattern),
								  regparse[1]);
			ঘূৰাই FAIL;
		    }
		    EMIT(NFA_CURSOR);
		    ভঙ্গ;

		পক্ষ 'V':
		    EMIT(NFA_VISUAL);
		    ভঙ্গ;

		পক্ষ 'C':
		    EMIT(NFA_ANY_COMPOSING);
		    ভঙ্গ;

		পক্ষ '[':
		    {
			পূর্ণাঙ্ক	    n;

			// \%[abc]
			বাবে (n = 0; (c = peekchr()) != ']'; ++n)
			{
			    যদি (c == NUL)
				EMSG2_RET_FAIL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
			    // recursive call!
			    যদি (nfa_regatom() == FAIL)
				ঘূৰাই FAIL;
			}
			getchr();  // get the ]
			যদি (n == 0)
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
			ভঙ্গ;
		    }

		স্ৱভাৱ:
		    {
			long_u	n = 0;
			পূর্ণাঙ্ক	cmp = c;
			পূর্ণাঙ্ক	cur = FALSE;
			পূর্ণাঙ্ক	got_digit = FALSE;

			যদি (c == '<' || c == '>')
			    c = getchr();
			যদি (no_Magic(c) == '.')
			{
			    cur = TRUE;
			    c = getchr();
			}
			যেতিয়ালৈ (VIM_ISDIGIT(c))
			{
			    long_u tmp;

			    যদি (cur)
			    {
				semsg(_(e_regexp_সংখ্যা_after_dot_pos_অনুসন্ধান_chr),
								  no_Magic(c));
				ঘূৰাই FAIL;
			    }
			    tmp = n * 10 + (c - '0');

			    যদি (tmp < n)
			    {
				// overflow.
				emsg(_(e_percent_মান_too_large));
				ঘূৰাই FAIL;
			    }
			    n = tmp;
			    c = getchr();
			    got_digit = TRUE;
			}
			যদি (c == 'l' || c == 'c' || c == 'v')
			{
			    long_u limit = INT_MAX;

			    যদি (!cur && !got_digit)
			    {
				semsg(_(e_nfa_regexp_missing_মান_in_chr),
								  no_Magic(c));
				ঘূৰাই FAIL;
			    }
			    যদি (c == 'l')
			    {
				যদি (cur)
				    n = curwin->w_cursor.lnum;
				// \%{n}l  \%{n}<l  \%{n}>l
				EMIT(cmp == '<' ? NFA_LNUM_LT :
				     cmp == '>' ? NFA_LNUM_GT : NFA_LNUM);
				যদি (save_prev_at_start)
				    at_start = TRUE;
			    }
			    অন্যথা যদি (c == 'c')
			    {
				যদি (cur)
				{
				    n = curwin->w_cursor.col;
				    n++;
				}
				// \%{n}c  \%{n}<c  \%{n}>c
				EMIT(cmp == '<' ? NFA_COL_LT :
				     cmp == '>' ? NFA_COL_GT : NFA_COL);
			    }
			    অন্যথা
			    {
				যদি (cur)
				{
				    colnr_T vcol = 0;

				    getvvcol(curwin, &curwin->w_cursor,
							 রিক্ত, রিক্ত, &vcol, 0);
				    n = ++vcol;
				}
				// \%{n}v  \%{n}<v  \%{n}>v
				EMIT(cmp == '<' ? NFA_VCOL_LT :
				     cmp == '>' ? NFA_VCOL_GT : NFA_VCOL);
				limit = INT_MAX / MB_MAXBYTES;
			    }
			    যদি (n >= limit)
			    {
				emsg(_(e_percent_মান_too_large));
				ঘূৰাই FAIL;
			    }
			    EMIT((পূর্ণাঙ্ক)n);
			    ভঙ্গ;
			}
			অন্যথা যদি (no_Magic(c) == '\'' && n == 0)
			{
			    // \%'m  \%<'m  \%>'m
			    EMIT(cmp == '<' ? NFA_MARK_LT :
				 cmp == '>' ? NFA_MARK_GT : NFA_MARK);
			    EMIT(getchr());
			    ভঙ্গ;
			}
		    }
		    semsg(_(e_nfa_regexp_unknown_operator_percent_chr),
								  no_Magic(c));
		    ঘূৰাই FAIL;
	    }
	    ভঙ্গ;

	পক্ষ Magic('['):
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
	    যদি (*endp == ']')
	    {
		পূর্ণাঙ্ক plen;
		bool range_endpoint;
		/*
		 * Try to reverse engineer character classes. For example,
		 * recognize that [0-9] stands for \d and [A-Za-z_] for \h,
		 * and perform the necessary substitutions in the NFA.
		 */
		result = nfa_recognize_char_class(regparse, endp,
							 extra == NFA_ADD_NL);
		যদি (result != FAIL)
		{
		    যদি (result >= NFA_FIRST_NL && result <= NFA_LAST_NL)
		    {
			EMIT(result - NFA_ADD_NL);
			EMIT(NFA_NEWL);
			EMIT(NFA_OR);
		    }
		    অন্যথা
			EMIT(result);
		    regparse = endp;
		    MB_PTR_ADV(regparse);
		    ঘূৰাই OK;
		}
		/*
		 * Failed to recognize a character class. Use the simple
		 * version that turns [abc] into 'a' OR 'b' OR 'c'
		 */
		startc = -1;
		negated = FALSE;
		যদি (*regparse == '^')			// negated range
		{
		    negated = TRUE;
		    MB_PTR_ADV(regparse);
		    EMIT(NFA_START_NEG_COLL);
		}
		অন্যথা
		    EMIT(NFA_START_COLL);
		যদি (*regparse == '-')
		{
		    startc = '-';
		    EMIT(startc);
		    EMIT(NFA_CONCAT);
		    MB_PTR_ADV(regparse);
		}
		// Emit the OR branches for each character in the []
		emit_range = FALSE;
		যেতিয়ালৈ (regparse < endp)
		{
		    পূর্ণাঙ্ক	    oldstartc = startc;
		    range_endpoint = false;

		    startc = -1;
		    got_coll_char = FALSE;
		    যদি (*regparse == '[')
		    {
			// Check for [: :], [= =], [. .]
			equiclass = collclass = 0;
			charclass = get_char_class(&regparse);
			যদি (charclass == CLASS_NONE)
			{
			    equiclass = get_equi_class(&regparse);
			    যদি (equiclass == 0)
				collclass = get_coll_element(&regparse);
			}

			// Character class like [:alpha:]
			যদি (charclass != CLASS_NONE)
			{
			    ৱিকল্প (charclass)
			    {
				পক্ষ CLASS_ALNUM:
				    EMIT(NFA_CLASS_ALNUM);
				    ভঙ্গ;
				পক্ষ CLASS_ALPHA:
				    EMIT(NFA_CLASS_ALPHA);
				    ভঙ্গ;
				পক্ষ CLASS_BLANK:
				    EMIT(NFA_CLASS_BLANK);
				    ভঙ্গ;
				পক্ষ CLASS_CNTRL:
				    EMIT(NFA_CLASS_CNTRL);
				    ভঙ্গ;
				পক্ষ CLASS_DIGIT:
				    EMIT(NFA_CLASS_DIGIT);
				    ভঙ্গ;
				পক্ষ CLASS_GRAPH:
				    EMIT(NFA_CLASS_GRAPH);
				    ভঙ্গ;
				পক্ষ CLASS_LOWER:
				    wants_nfa = TRUE;
				    EMIT(NFA_CLASS_LOWER);
				    ভঙ্গ;
				পক্ষ CLASS_PRINT:
				    EMIT(NFA_CLASS_PRINT);
				    ভঙ্গ;
				পক্ষ CLASS_PUNCT:
				    EMIT(NFA_CLASS_PUNCT);
				    ভঙ্গ;
				পক্ষ CLASS_SPACE:
				    EMIT(NFA_CLASS_SPACE);
				    ভঙ্গ;
				পক্ষ CLASS_UPPER:
				    wants_nfa = TRUE;
				    EMIT(NFA_CLASS_UPPER);
				    ভঙ্গ;
				পক্ষ CLASS_XDIGIT:
				    EMIT(NFA_CLASS_XDIGIT);
				    ভঙ্গ;
				পক্ষ CLASS_TAB:
				    EMIT(NFA_CLASS_TAB);
				    ভঙ্গ;
				পক্ষ CLASS_RETURN:
				    EMIT(NFA_CLASS_RETURN);
				    ভঙ্গ;
				পক্ষ CLASS_BACKSPACE:
				    EMIT(NFA_CLASS_BACKSPACE);
				    ভঙ্গ;
				পক্ষ CLASS_ESCAPE:
				    EMIT(NFA_CLASS_ESCAPE);
				    ভঙ্গ;
				পক্ষ CLASS_IDENT:
				    EMIT(NFA_CLASS_IDENT);
				    ভঙ্গ;
				পক্ষ CLASS_KEYWORD:
				    EMIT(NFA_CLASS_KEYWORD);
				    ভঙ্গ;
				পক্ষ CLASS_FNAME:
				    EMIT(NFA_CLASS_FNAME);
				    ভঙ্গ;
			    }
			    EMIT(NFA_CONCAT);
			    অনুৱর্ত;
			}
			// Try equivalence class [=a=] and the like
			যদি (equiclass != 0)
			{
			    result = nfa_emit_equi_class(equiclass);
			    যদি (result == FAIL)
			    {
				// should never happen
				EMSG_RET_FAIL(_(e_error_building_nfa_with_equivalence_class));
			    }
			    অনুৱর্ত;
			}
			// Try collating class like [. .]
			যদি (collclass != 0)
			{
			    startc = collclass;	 // allow [.a.]-x as a range
			    // Will emit the proper atom at the end of the
			    // while loop.
			}
		    }
		    // Try a range like 'a-x' or '\t-z'. Also allows '-' as a
		    // start character.
		    যদি (*regparse == '-' && oldstartc != -1)
		    {
			emit_range = TRUE;
			startc = oldstartc;
			MB_PTR_ADV(regparse);
			অনুৱর্ত;	    // reading the end of the range
		    }

		    // Now handle simple and escaped characters.
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    যদি (*regparse == '\\'
			    && !reg_cpo_bsl
			    && regparse + 1 <= endp
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != রিক্ত
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR, regparse[1])
								      != রিক্ত)
			    )
			)
		    {
			MB_PTR_ADV(regparse);

			যদি (*regparse == 'n')
			    startc = (reg_string || emit_range
					|| regparse[1] == '-') ? NL : NFA_NEWL;
			অন্যথা যদি (*regparse == 'd'
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
				যদি (startc == INT_MAX)
				    EMSG_RET_FAIL(_(e_unicode_val_too_large));
				got_coll_char = TRUE;
				MB_PTR_BACK(old_regparse, regparse);
			    }
			    অন্যথা
			    {
				// \r,\t,\e,\b
				startc = backslash_trans(*regparse);
			    }
		    }

		    // Normal printable char
		    যদি (startc == -1)
			startc = PTR2CHAR(regparse);

		    // Previous char was '-', so this char is end of range.
		    যদি (emit_range)
		    {
			পূর্ণাঙ্ক	endc = startc;
			range_endpoint = true;

			startc = oldstartc;
			যদি (startc > endc)
			    EMSG_RET_FAIL(_(e_reverse_range_in_character_class));

			যদি (endc > startc + 2)
			{
			    // Emit a range instead of the sequence of
			    // individual characters.
			    যদি (startc == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    অন্যথা
				--post_ptr; // remove NFA_CONCAT
			    EMIT(endc);
			    EMIT(NFA_RANGE);
			    EMIT(NFA_CONCAT);
			}
			অন্যথা যদি (has_mbyte && ((*mb_char2len)(startc) > 1
				    || (*mb_char2len)(endc) > 1))
			{
			    // Emit the characters in the range.
			    // "startc" was already emitted, so skip it.
			    //
			    বাবে (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			অন্যথা
			{
			    // Emit the range. "startc" was already emitted, so
			    // skip it.
			    বাবে (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			emit_range = FALSE;
			startc = -1;
		    }
		    অন্যথা
		    {
			// This char (startc) is not part of a range. Just
			// emit it.
			// Normally, simply emit startc. But if we get char
			// code=0 from a collating char, then replace it with
			// 0x0a.
			// This is needed to completely mimic the behaviour of
			// the backtracking engine.
			যদি (startc == NFA_NEWL)
			{
			    // Line break can't be matched as part of the
			    // collection, add an OR below. But not for negated
			    // range.
			    যদি (!negated)
				extra = NFA_ADD_NL;
			}
			অন্যথা
			{
			    যদি (got_coll_char == TRUE && startc == 0)
			    {
				EMIT(0x0a);
				EMIT(NFA_CONCAT);
			    }
			    অন্যথা
			    {
				EMIT(startc);
				যদি (!(enc_utf8 && (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse)))))
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
		    যদি (!range_endpoint && enc_utf8 &&
			    (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse))))
		    {
			পূর্ণাঙ্ক i = utf_ptr2len(regparse);

			c = utf_ptr2char(regparse + i);

			// Add composing characters
			বাবে (;;)
			{
			    যদি (c == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    অন্যথা
				EMIT(c);
			    EMIT(NFA_CONCAT);
			    যদি ((i += utf_char2len(c)) >= plen)
				ভঙ্গ;
			    c = utf_ptr2char(regparse + i);
			}
			EMIT(NFA_COMPOSING);
			EMIT(NFA_CONCAT);
		    }
		    MB_PTR_ADV(regparse);
		} // while (p < endp)

		MB_PTR_BACK(old_regparse, regparse);
		যদি (*regparse == '-')	    // if last, '-' is just a char
		{
		    EMIT('-');
		    EMIT(NFA_CONCAT);
		}

		// skip the trailing ]
		regparse = endp;
		MB_PTR_ADV(regparse);

		// Mark end of the collection.
		যদি (negated == TRUE)
		    EMIT(NFA_END_NEG_COLL);
		অন্যথা
		    EMIT(NFA_END_COLL);

		// \_[] also matches \n but it's not negated
		যদি (extra == NFA_ADD_NL)
		{
		    EMIT(reg_string ? NL : NFA_NEWL);
		    EMIT(NFA_OR);
		}

		ঘূৰাই OK;
	    } // if exists closing ]

	    যদি (reg_strict)
		EMSG_RET_FAIL(_(e_missing_rsb_after_str_lsb));
	    // FALLTHROUGH

	স্ৱভাৱ:
	    {
		পূর্ণাঙ্ক	plen;

nfa_do_multibyte:
		// plen is length of current char with composing chars
		যদি (enc_utf8 && ((*mb_char2len)(c)
			    != (plen = utfc_ptr2len(old_regparse))
						       || utf_iscomposing(c)))
		{
		    পূর্ণাঙ্ক i = 0;

		    // A base character plus composing characters, or just one
		    // or more composing characters.
		    // This requires creating a separate atom as if enclosing
		    // the characters in (), where NFA_COMPOSING is the ( and
		    // NFA_END_COMPOSING is the ). Note that right now we are
		    // building the postfix form, not the NFA itself;
		    // a composing char could be: a, b, c, NFA_COMPOSING
		    // where 'b' and 'c' are chars with codes > 256.
		    বাবে (;;)
		    {
			EMIT(c);
			যদি (i > 0)
			    EMIT(NFA_CONCAT);
			যদি ((i += utf_char2len(c)) >= plen)
			    ভঙ্গ;
			c = utf_ptr2char(old_regparse + i);
		    }
		    EMIT(NFA_COMPOSING);
		    regparse = old_regparse + plen;
		}
		অন্যথা
		{
		    c = no_Magic(c);
		    EMIT(c);
		}
		ঘূৰাই OK;
	    }
    }

    ঘূৰাই OK;
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
    স্থির পূর্ণাঙ্ক
nfa_regpiece(শূন্য)
{
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		op;
    পূর্ণাঙ্ক		ret;
    long	minval, maxval;
    পূর্ণাঙ্ক		greedy = TRUE;      // Braces are prefixed with '-' ?
    parse_state_T old_state;
    parse_state_T new_state;
    long	c2;
    পূর্ণাঙ্ক		old_post_pos;
    পূর্ণাঙ্ক		my_post_start;
    পূর্ণাঙ্ক		quest;

    // Save the current parse state, so that we can use it if <atom>{m,n} is
    // next.
    save_parse_state(&old_state);

    // store current pos in the postfix form, for \{m,n} involving 0s
    my_post_start = (পূর্ণাঙ্ক)(post_ptr - post_start);

    ret = nfa_regatom();
    যদি (ret == FAIL)
	ঘূৰাই FAIL;	    // cascaded error

    op = peekchr();
    যদি (re_multi_type(op) == NOT_MULTI)
	ঘূৰাই OK;

    skipchr();
    ৱিকল্প (op)
    {
	পক্ষ Magic('*'):
	    EMIT(NFA_STAR);
	    ভঙ্গ;

	পক্ষ Magic('+'):
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
	    যদি (nfa_regatom() == FAIL)
		ঘূৰাই FAIL;
	    EMIT(NFA_STAR);
	    EMIT(NFA_CONCAT);
	    skipchr();		// skip the \+
	    ভঙ্গ;

	পক্ষ Magic('@'):
	    c2 = (long)getdecchrs();
	    op = no_Magic(getchr());
	    i = 0;
	    ৱিকল্প(op)
	    {
		পক্ষ '=':
		    // \@=
		    i = NFA_PREV_ATOM_NO_WIDTH;
		    ভঙ্গ;
		পক্ষ '!':
		    // \@!
		    i = NFA_PREV_ATOM_NO_WIDTH_NEG;
		    ভঙ্গ;
		পক্ষ '<':
		    op = no_Magic(getchr());
		    যদি (op == '=')
			// \@<=
			i = NFA_PREV_ATOM_JUST_BEFORE;
		    অন্যথা যদি (op == '!')
			// \@<!
			i = NFA_PREV_ATOM_JUST_BEFORE_NEG;
		    ভঙ্গ;
		পক্ষ '>':
		    // \@>
		    i = NFA_PREV_ATOM_LIKE_PATTERN;
		    ভঙ্গ;
	    }
	    যদি (i == 0)
	    {
		semsg(_(e_nfa_regexp_unknown_operator_at_chr), op);
		ঘূৰাই FAIL;
	    }
	    EMIT(i);
	    যদি (i == NFA_PREV_ATOM_JUST_BEFORE
					|| i == NFA_PREV_ATOM_JUST_BEFORE_NEG)
		EMIT(c2);
	    ভঙ্গ;

	পক্ষ Magic('?'):
	পক্ষ Magic('='):
	    EMIT(NFA_QUEST);
	    ভঙ্গ;

	পক্ষ Magic('{'):
	    // a{2,5} will expand to 'aaa?a?a?'
	    // a{-1,3} will expand to 'aa??a??', where ?? is the nongreedy
	    // version of '?'
	    // \v(ab){2,3} will expand to '(ab)(ab)(ab)?', where all the
	    // parenthesis have the same id

	    greedy = TRUE;
	    c2 = peekchr();
	    যদি (c2 == '-' || c2 == Magic('-'))
	    {
		skipchr();
		greedy = FALSE;
	    }
	    যদি (!read_limits(&minval, &maxval))
		EMSG_RET_FAIL(_(e_nfa_regexp_error_reading_repetition_limits));

	    //  <atom>{0,inf}, <atom>{0,} and <atom>{}  are equivalent to
	    //  <atom>*
	    যদি (minval == 0 && maxval == MAX_LIMIT)
	    {
		যদি (greedy)		// { { (match the braces)
		    // \{}, \{0,}
		    EMIT(NFA_STAR);
		অন্যথা			// { { (match the braces)
		    // \{-}, \{-0,}
		    EMIT(NFA_STAR_NONGREEDY);
		ভঙ্গ;
	    }

	    // Special case: x{0} or x{-0}
	    যদি (maxval == 0)
	    {
		// Ignore result of previous call to nfa_regatom()
		post_ptr = post_start + my_post_start;
		// NFA_EMPTY is 0-length and works everywhere
		EMIT(NFA_EMPTY);
		ঘূৰাই OK;
	    }

	    // The engine is very inefficient (uses too many states) when the
	    // maximum is much larger than the minimum and when the maximum is
	    // large.  However, when maxval is MAX_LIMIT, it is okay, as this
	    // will emit NFA_STAR.
	    // Bail out if we can use the other engine, but only, when the
	    // pattern does not need the NFA engine like (e.g. [[:upper:]]\{2,\}
	    // does not work with characters > 8 bit with the BT engine)
	    যদি ((nfa_re_flags & RE_AUTO)
				   && (maxval > 500 || maxval > minval + 200)
				   && (maxval != MAX_LIMIT && minval < 200)
				   && !wants_nfa)
		ঘূৰাই FAIL;

	    // Ignore previous call to nfa_regatom()
	    post_ptr = post_start + my_post_start;
	    // Save parse state after the repeated atom and the \{}
	    save_parse_state(&new_state);

	    quest = (greedy == TRUE? NFA_QUEST : NFA_QUEST_NONGREEDY);
	    বাবে (i = 0; i < maxval; i++)
	    {
		// Goto beginning of the repeated atom
		restore_parse_state(&old_state);
		old_post_pos = (পূর্ণাঙ্ক)(post_ptr - post_start);
		যদি (nfa_regatom() == FAIL)
		    ঘূৰাই FAIL;
		// after "minval" times, atoms are optional
		যদি (i + 1 > minval)
		{
		    যদি (maxval == MAX_LIMIT)
		    {
			যদি (greedy)
			    EMIT(NFA_STAR);
			অন্যথা
			    EMIT(NFA_STAR_NONGREEDY);
		    }
		    অন্যথা
			EMIT(quest);
		}
		যদি (old_post_pos != my_post_start)
		    EMIT(NFA_CONCAT);
		যদি (i + 1 > minval && maxval == MAX_LIMIT)
		    ভঙ্গ;
	    }

	    // Go to just after the repeated atom and the \{}
	    restore_parse_state(&new_state);
	    curchr = -1;

	    ভঙ্গ;


	স্ৱভাৱ:
	    ভঙ্গ;
    }	// end switch

    যদি (re_multi_type(peekchr()) != NOT_MULTI)
	// Can't have a multi follow a multi.
	EMSG_RET_FAIL(_(e_nfa_regexp_cant_have_multi_follow_multi));

    ঘূৰাই OK;
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
    স্থির পূর্ণাঙ্ক
nfa_regconcat(শূন্য)
{
    পূর্ণাঙ্ক		cont = TRUE;
    পূর্ণাঙ্ক		first = TRUE;

    যেতিয়ালৈ (cont)
    {
	ৱিকল্প (peekchr())
	{
	    পক্ষ NUL:
	    পক্ষ Magic('|'):
	    পক্ষ Magic('&'):
	    পক্ষ Magic(')'):
		cont = FALSE;
		ভঙ্গ;

	    পক্ষ Magic('Z'):
		regflags |= RF_ICOMBINE;
		skipchr_keepstart();
		ভঙ্গ;
	    পক্ষ Magic('c'):
		regflags |= RF_ICASE;
		skipchr_keepstart();
		ভঙ্গ;
	    পক্ষ Magic('C'):
		regflags |= RF_NOICASE;
		skipchr_keepstart();
		ভঙ্গ;
	    পক্ষ Magic('v'):
		reg_magic = MAGIC_ALL;
		skipchr_keepstart();
		curchr = -1;
		ভঙ্গ;
	    পক্ষ Magic('m'):
		reg_magic = MAGIC_ON;
		skipchr_keepstart();
		curchr = -1;
		ভঙ্গ;
	    পক্ষ Magic('M'):
		reg_magic = MAGIC_OFF;
		skipchr_keepstart();
		curchr = -1;
		ভঙ্গ;
	    পক্ষ Magic('V'):
		reg_magic = MAGIC_NONE;
		skipchr_keepstart();
		curchr = -1;
		ভঙ্গ;

	    স্ৱভাৱ:
		যদি (nfa_regpiece() == FAIL)
		    ঘূৰাই FAIL;
		যদি (first == FALSE)
		    EMIT(NFA_CONCAT);
		অন্যথা
		    first = FALSE;
		ভঙ্গ;
	}
    }

    ঘূৰাই OK;
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
    স্থির পূর্ণাঙ্ক
nfa_regbranch(শূন্য)
{
    পূর্ণাঙ্ক		old_post_pos;

    old_post_pos = (পূর্ণাঙ্ক)(post_ptr - post_start);

    // First branch, possibly the only one
    যদি (nfa_regconcat() == FAIL)
	ঘূৰাই FAIL;

    // Try next concats
    যেতিয়ালৈ (peekchr() == Magic('&'))
    {
	skipchr();
	// if concat is empty do emit a node
	যদি (old_post_pos == (পূর্ণাঙ্ক)(post_ptr - post_start))
	    EMIT(NFA_EMPTY);
	EMIT(NFA_NOPEN);
	EMIT(NFA_PREV_ATOM_NO_WIDTH);
	old_post_pos = (পূর্ণাঙ্ক)(post_ptr - post_start);
	যদি (nfa_regconcat() == FAIL)
	    ঘূৰাই FAIL;
	// if concat is empty do emit a node
	যদি (old_post_pos == (পূর্ণাঙ্ক)(post_ptr - post_start))
	    EMIT(NFA_EMPTY);
	EMIT(NFA_CONCAT);
    }

    // if a branch is empty, emit one node for it
    যদি (old_post_pos == (পূর্ণাঙ্ক)(post_ptr - post_start))
	EMIT(NFA_EMPTY);

    ঘূৰাই OK;
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
    স্থির পূর্ণাঙ্ক
nfa_reg(
    পূর্ণাঙ্ক		paren)	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
{
    পূর্ণাঙ্ক		parno = 0;
    পূর্ণাঙ্ক		status = FAIL;

    যদি (paren == REG_PAREN)
    {
	যদি (regnpar >= NSUBEXP) // Too many `('
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_parens));
	parno = regnpar++;
    }
#ifdef FEAT_SYN_HL
    অন্যথা যদি (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	যদি (regnzpar >= NSUBEXP)
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_z));
	parno = regnzpar++;
    }
#endif

    যদি (nfa_reg_parse_depth >= REG_MAX_PAREN_DEPTH)
	EMSG_RET_FAIL(_(e_command_too_complex));
    ++nfa_reg_parse_depth;

    যদি (nfa_regbranch() == FAIL)
	গচ্ছ theend;	    // cascaded error

    যেতিয়ালৈ (peekchr() == Magic('|'))
    {
	skipchr();
	যদি (nfa_regbranch() == FAIL)
	    গচ্ছ theend;    // cascaded error
	EMIT(NFA_OR);
    }

    // Check for proper termination.
    যদি (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
	যদি (paren == REG_NPAREN)
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	অন্যথা
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	rc_did_emsg = TRUE;
	গচ্ছ theend;
    }
    অন্যথা যদি (paren == REG_NOPAREN && peekchr() != NUL)
    {
	যদি (peekchr() == Magic(')'))
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	অন্যথা
	    emsg(_(e_nfa_regexp_proper_termination_error));
	rc_did_emsg = TRUE;
	গচ্ছ theend;
    }
    /*
     * Here we set the flag allowing back references to this set of
     * parentheses.
     */
    যদি (paren == REG_PAREN)
    {
	had_endbrace[parno] = TRUE;     // have seen the close paren
	EMIT(NFA_MOPEN + parno);
    }
#ifdef FEAT_SYN_HL
    অন্যথা যদি (paren == REG_ZPAREN)
	EMIT(NFA_ZOPEN + parno);
#endif

    status = OK;

theend:
    --nfa_reg_parse_depth;
    ঘূৰাই status;
}

#ifdef DEBUG
স্থির char_u code[50];

    স্থির শূন্য
nfa_set_code(পূর্ণাঙ্ক c)
{
    পূর্ণাঙ্ক	    addnl = FALSE;

    যদি (c >= NFA_FIRST_NL && c <= NFA_LAST_NL)
    {
	addnl = TRUE;
	c -= NFA_ADD_NL;
    }

    STRCPY(code, "");
    ৱিকল্প (c)
    {
	পক্ষ NFA_MATCH:	    STRCPY(code, "NFA_MATCH "); ভঙ্গ;
	পক্ষ NFA_SPLIT:	    STRCPY(code, "NFA_SPLIT "); ভঙ্গ;
	পক্ষ NFA_CONCAT:    STRCPY(code, "NFA_CONCAT "); ভঙ্গ;
	পক্ষ NFA_NEWL:	    STRCPY(code, "NFA_NEWL "); ভঙ্গ;
	পক্ষ NFA_ZSTART:    STRCPY(code, "NFA_ZSTART"); ভঙ্গ;
	পক্ষ NFA_ZEND:	    STRCPY(code, "NFA_ZEND"); ভঙ্গ;

	পক্ষ NFA_BACKREF1:  STRCPY(code, "NFA_BACKREF1"); ভঙ্গ;
	পক্ষ NFA_BACKREF2:  STRCPY(code, "NFA_BACKREF2"); ভঙ্গ;
	পক্ষ NFA_BACKREF3:  STRCPY(code, "NFA_BACKREF3"); ভঙ্গ;
	পক্ষ NFA_BACKREF4:  STRCPY(code, "NFA_BACKREF4"); ভঙ্গ;
	পক্ষ NFA_BACKREF5:  STRCPY(code, "NFA_BACKREF5"); ভঙ্গ;
	পক্ষ NFA_BACKREF6:  STRCPY(code, "NFA_BACKREF6"); ভঙ্গ;
	পক্ষ NFA_BACKREF7:  STRCPY(code, "NFA_BACKREF7"); ভঙ্গ;
	পক্ষ NFA_BACKREF8:  STRCPY(code, "NFA_BACKREF8"); ভঙ্গ;
	পক্ষ NFA_BACKREF9:  STRCPY(code, "NFA_BACKREF9"); ভঙ্গ;
# ifdef FEAT_SYN_HL
	পক্ষ NFA_ZREF1:	    STRCPY(code, "NFA_ZREF1"); ভঙ্গ;
	পক্ষ NFA_ZREF2:	    STRCPY(code, "NFA_ZREF2"); ভঙ্গ;
	পক্ষ NFA_ZREF3:	    STRCPY(code, "NFA_ZREF3"); ভঙ্গ;
	পক্ষ NFA_ZREF4:	    STRCPY(code, "NFA_ZREF4"); ভঙ্গ;
	পক্ষ NFA_ZREF5:	    STRCPY(code, "NFA_ZREF5"); ভঙ্গ;
	পক্ষ NFA_ZREF6:	    STRCPY(code, "NFA_ZREF6"); ভঙ্গ;
	পক্ষ NFA_ZREF7:	    STRCPY(code, "NFA_ZREF7"); ভঙ্গ;
	পক্ষ NFA_ZREF8:	    STRCPY(code, "NFA_ZREF8"); ভঙ্গ;
	পক্ষ NFA_ZREF9:	    STRCPY(code, "NFA_ZREF9"); ভঙ্গ;
# endif
	পক্ষ NFA_SKIP:	    STRCPY(code, "NFA_SKIP"); ভঙ্গ;

	পক্ষ NFA_PREV_ATOM_NO_WIDTH:
			    STRCPY(code, "NFA_PREV_ATOM_NO_WIDTH"); ভঙ্গ;
	পক্ষ NFA_PREV_ATOM_NO_WIDTH_NEG:
			    STRCPY(code, "NFA_PREV_ATOM_NO_WIDTH_NEG"); ভঙ্গ;
	পক্ষ NFA_PREV_ATOM_JUST_BEFORE:
			    STRCPY(code, "NFA_PREV_ATOM_JUST_BEFORE"); ভঙ্গ;
	পক্ষ NFA_PREV_ATOM_JUST_BEFORE_NEG:
			 STRCPY(code, "NFA_PREV_ATOM_JUST_BEFORE_NEG"); ভঙ্গ;
	পক্ষ NFA_PREV_ATOM_LIKE_PATTERN:
			    STRCPY(code, "NFA_PREV_ATOM_LIKE_PATTERN"); ভঙ্গ;

	পক্ষ NFA_NOPEN:		    STRCPY(code, "NFA_NOPEN"); ভঙ্গ;
	পক্ষ NFA_NCLOSE:	    STRCPY(code, "NFA_NCLOSE"); ভঙ্গ;
	পক্ষ NFA_START_INVISIBLE:   STRCPY(code, "NFA_START_INVISIBLE"); ভঙ্গ;
	পক্ষ NFA_START_INVISIBLE_FIRST:
			     STRCPY(code, "NFA_START_INVISIBLE_FIRST"); ভঙ্গ;
	পক্ষ NFA_START_INVISIBLE_NEG:
			       STRCPY(code, "NFA_START_INVISIBLE_NEG"); ভঙ্গ;
	পক্ষ NFA_START_INVISIBLE_NEG_FIRST:
			 STRCPY(code, "NFA_START_INVISIBLE_NEG_FIRST"); ভঙ্গ;
	পক্ষ NFA_START_INVISIBLE_BEFORE:
			    STRCPY(code, "NFA_START_INVISIBLE_BEFORE"); ভঙ্গ;
	পক্ষ NFA_START_INVISIBLE_BEFORE_FIRST:
		      STRCPY(code, "NFA_START_INVISIBLE_BEFORE_FIRST"); ভঙ্গ;
	পক্ষ NFA_START_INVISIBLE_BEFORE_NEG:
			STRCPY(code, "NFA_START_INVISIBLE_BEFORE_NEG"); ভঙ্গ;
	পক্ষ NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
		  STRCPY(code, "NFA_START_INVISIBLE_BEFORE_NEG_FIRST"); ভঙ্গ;
	পক্ষ NFA_START_PATTERN:   STRCPY(code, "NFA_START_PATTERN"); ভঙ্গ;
	পক্ষ NFA_END_INVISIBLE:	    STRCPY(code, "NFA_END_INVISIBLE"); ভঙ্গ;
	পক্ষ NFA_END_INVISIBLE_NEG: STRCPY(code, "NFA_END_INVISIBLE_NEG"); ভঙ্গ;
	পক্ষ NFA_END_PATTERN:	    STRCPY(code, "NFA_END_PATTERN"); ভঙ্গ;

	পক্ষ NFA_COMPOSING:	    STRCPY(code, "NFA_COMPOSING"); ভঙ্গ;
	পক্ষ NFA_END_COMPOSING:	    STRCPY(code, "NFA_END_COMPOSING"); ভঙ্গ;
	পক্ষ NFA_OPT_CHARS:	    STRCPY(code, "NFA_OPT_CHARS"); ভঙ্গ;

	পক্ষ NFA_MOPEN:
	পক্ষ NFA_MOPEN1:
	পক্ষ NFA_MOPEN2:
	পক্ষ NFA_MOPEN3:
	পক্ষ NFA_MOPEN4:
	পক্ষ NFA_MOPEN5:
	পক্ষ NFA_MOPEN6:
	পক্ষ NFA_MOPEN7:
	পক্ষ NFA_MOPEN8:
	পক্ষ NFA_MOPEN9:
	    STRCPY(code, "NFA_MOPEN(x)");
	    code[10] = c - NFA_MOPEN + '0';
	    ভঙ্গ;
	পক্ষ NFA_MCLOSE:
	পক্ষ NFA_MCLOSE1:
	পক্ষ NFA_MCLOSE2:
	পক্ষ NFA_MCLOSE3:
	পক্ষ NFA_MCLOSE4:
	পক্ষ NFA_MCLOSE5:
	পক্ষ NFA_MCLOSE6:
	পক্ষ NFA_MCLOSE7:
	পক্ষ NFA_MCLOSE8:
	পক্ষ NFA_MCLOSE9:
	    STRCPY(code, "NFA_MCLOSE(x)");
	    code[11] = c - NFA_MCLOSE + '0';
	    ভঙ্গ;
# ifdef FEAT_SYN_HL
	পক্ষ NFA_ZOPEN:
	পক্ষ NFA_ZOPEN1:
	পক্ষ NFA_ZOPEN2:
	পক্ষ NFA_ZOPEN3:
	পক্ষ NFA_ZOPEN4:
	পক্ষ NFA_ZOPEN5:
	পক্ষ NFA_ZOPEN6:
	পক্ষ NFA_ZOPEN7:
	পক্ষ NFA_ZOPEN8:
	পক্ষ NFA_ZOPEN9:
	    STRCPY(code, "NFA_ZOPEN(x)");
	    code[10] = c - NFA_ZOPEN + '0';
	    ভঙ্গ;
	পক্ষ NFA_ZCLOSE:
	পক্ষ NFA_ZCLOSE1:
	পক্ষ NFA_ZCLOSE2:
	পক্ষ NFA_ZCLOSE3:
	পক্ষ NFA_ZCLOSE4:
	পক্ষ NFA_ZCLOSE5:
	পক্ষ NFA_ZCLOSE6:
	পক্ষ NFA_ZCLOSE7:
	পক্ষ NFA_ZCLOSE8:
	পক্ষ NFA_ZCLOSE9:
	    STRCPY(code, "NFA_ZCLOSE(x)");
	    code[11] = c - NFA_ZCLOSE + '0';
	    ভঙ্গ;
# endif
	পক্ষ NFA_EOL:		STRCPY(code, "NFA_EOL "); ভঙ্গ;
	পক্ষ NFA_BOL:		STRCPY(code, "NFA_BOL "); ভঙ্গ;
	পক্ষ NFA_EOW:		STRCPY(code, "NFA_EOW "); ভঙ্গ;
	পক্ষ NFA_BOW:		STRCPY(code, "NFA_BOW "); ভঙ্গ;
	পক্ষ NFA_EOF:		STRCPY(code, "NFA_EOF "); ভঙ্গ;
	পক্ষ NFA_BOF:		STRCPY(code, "NFA_BOF "); ভঙ্গ;
	পক্ষ NFA_LNUM:		STRCPY(code, "NFA_LNUM "); ভঙ্গ;
	পক্ষ NFA_LNUM_GT:	STRCPY(code, "NFA_LNUM_GT "); ভঙ্গ;
	পক্ষ NFA_LNUM_LT:	STRCPY(code, "NFA_LNUM_LT "); ভঙ্গ;
	পক্ষ NFA_COL:		STRCPY(code, "NFA_COL "); ভঙ্গ;
	পক্ষ NFA_COL_GT:	STRCPY(code, "NFA_COL_GT "); ভঙ্গ;
	পক্ষ NFA_COL_LT:	STRCPY(code, "NFA_COL_LT "); ভঙ্গ;
	পক্ষ NFA_VCOL:		STRCPY(code, "NFA_VCOL "); ভঙ্গ;
	পক্ষ NFA_VCOL_GT:	STRCPY(code, "NFA_VCOL_GT "); ভঙ্গ;
	পক্ষ NFA_VCOL_LT:	STRCPY(code, "NFA_VCOL_LT "); ভঙ্গ;
	পক্ষ NFA_MARK:		STRCPY(code, "NFA_MARK "); ভঙ্গ;
	পক্ষ NFA_MARK_GT:	STRCPY(code, "NFA_MARK_GT "); ভঙ্গ;
	পক্ষ NFA_MARK_LT:	STRCPY(code, "NFA_MARK_LT "); ভঙ্গ;
	পক্ষ NFA_CURSOR:	STRCPY(code, "NFA_CURSOR "); ভঙ্গ;
	পক্ষ NFA_VISUAL:	STRCPY(code, "NFA_VISUAL "); ভঙ্গ;
	পক্ষ NFA_ANY_COMPOSING:	STRCPY(code, "NFA_ANY_COMPOSING "); ভঙ্গ;

	পক্ষ NFA_STAR:		STRCPY(code, "NFA_STAR "); ভঙ্গ;
	পক্ষ NFA_STAR_NONGREEDY: STRCPY(code, "NFA_STAR_NONGREEDY "); ভঙ্গ;
	পক্ষ NFA_QUEST:		STRCPY(code, "NFA_QUEST"); ভঙ্গ;
	পক্ষ NFA_QUEST_NONGREEDY: STRCPY(code, "NFA_QUEST_NON_GREEDY"); ভঙ্গ;
	পক্ষ NFA_EMPTY:		STRCPY(code, "NFA_EMPTY"); ভঙ্গ;
	পক্ষ NFA_OR:		STRCPY(code, "NFA_OR"); ভঙ্গ;

	পক্ষ NFA_START_COLL:	STRCPY(code, "NFA_START_COLL"); ভঙ্গ;
	পক্ষ NFA_END_COLL:	STRCPY(code, "NFA_END_COLL"); ভঙ্গ;
	পক্ষ NFA_START_NEG_COLL: STRCPY(code, "NFA_START_NEG_COLL"); ভঙ্গ;
	পক্ষ NFA_END_NEG_COLL:	STRCPY(code, "NFA_END_NEG_COLL"); ভঙ্গ;
	পক্ষ NFA_RANGE:		STRCPY(code, "NFA_RANGE"); ভঙ্গ;
	পক্ষ NFA_RANGE_MIN:	STRCPY(code, "NFA_RANGE_MIN"); ভঙ্গ;
	পক্ষ NFA_RANGE_MAX:	STRCPY(code, "NFA_RANGE_MAX"); ভঙ্গ;

	পক্ষ NFA_CLASS_ALNUM:	STRCPY(code, "NFA_CLASS_ALNUM"); ভঙ্গ;
	পক্ষ NFA_CLASS_ALPHA:	STRCPY(code, "NFA_CLASS_ALPHA"); ভঙ্গ;
	পক্ষ NFA_CLASS_BLANK:	STRCPY(code, "NFA_CLASS_BLANK"); ভঙ্গ;
	পক্ষ NFA_CLASS_CNTRL:	STRCPY(code, "NFA_CLASS_CNTRL"); ভঙ্গ;
	পক্ষ NFA_CLASS_DIGIT:	STRCPY(code, "NFA_CLASS_DIGIT"); ভঙ্গ;
	পক্ষ NFA_CLASS_GRAPH:	STRCPY(code, "NFA_CLASS_GRAPH"); ভঙ্গ;
	পক্ষ NFA_CLASS_LOWER:	STRCPY(code, "NFA_CLASS_LOWER"); ভঙ্গ;
	পক্ষ NFA_CLASS_PRINT:	STRCPY(code, "NFA_CLASS_PRINT"); ভঙ্গ;
	পক্ষ NFA_CLASS_PUNCT:	STRCPY(code, "NFA_CLASS_PUNCT"); ভঙ্গ;
	পক্ষ NFA_CLASS_SPACE:	STRCPY(code, "NFA_CLASS_SPACE"); ভঙ্গ;
	পক্ষ NFA_CLASS_UPPER:	STRCPY(code, "NFA_CLASS_UPPER"); ভঙ্গ;
	পক্ষ NFA_CLASS_XDIGIT:	STRCPY(code, "NFA_CLASS_XDIGIT"); ভঙ্গ;
	পক্ষ NFA_CLASS_TAB:	STRCPY(code, "NFA_CLASS_TAB"); ভঙ্গ;
	পক্ষ NFA_CLASS_RETURN:	STRCPY(code, "NFA_CLASS_RETURN"); ভঙ্গ;
	পক্ষ NFA_CLASS_BACKSPACE:   STRCPY(code, "NFA_CLASS_BACKSPACE"); ভঙ্গ;
	পক্ষ NFA_CLASS_ESCAPE:	STRCPY(code, "NFA_CLASS_ESCAPE"); ভঙ্গ;
	পক্ষ NFA_CLASS_IDENT:	STRCPY(code, "NFA_CLASS_IDENT"); ভঙ্গ;
	পক্ষ NFA_CLASS_KEYWORD:	STRCPY(code, "NFA_CLASS_KEYWORD"); ভঙ্গ;
	পক্ষ NFA_CLASS_FNAME:	STRCPY(code, "NFA_CLASS_FNAME"); ভঙ্গ;

	পক্ষ NFA_ANY:	STRCPY(code, "NFA_ANY"); ভঙ্গ;
	পক্ষ NFA_IDENT:	STRCPY(code, "NFA_IDENT"); ভঙ্গ;
	পক্ষ NFA_SIDENT:STRCPY(code, "NFA_SIDENT"); ভঙ্গ;
	পক্ষ NFA_KWORD:	STRCPY(code, "NFA_KWORD"); ভঙ্গ;
	পক্ষ NFA_SKWORD:STRCPY(code, "NFA_SKWORD"); ভঙ্গ;
	পক্ষ NFA_FNAME:	STRCPY(code, "NFA_FNAME"); ভঙ্গ;
	পক্ষ NFA_SFNAME:STRCPY(code, "NFA_SFNAME"); ভঙ্গ;
	পক্ষ NFA_PRINT:	STRCPY(code, "NFA_PRINT"); ভঙ্গ;
	পক্ষ NFA_SPRINT:STRCPY(code, "NFA_SPRINT"); ভঙ্গ;
	পক্ষ NFA_WHITE:	STRCPY(code, "NFA_WHITE"); ভঙ্গ;
	পক্ষ NFA_NWHITE:STRCPY(code, "NFA_NWHITE"); ভঙ্গ;
	পক্ষ NFA_DIGIT:	STRCPY(code, "NFA_DIGIT"); ভঙ্গ;
	পক্ষ NFA_NDIGIT:STRCPY(code, "NFA_NDIGIT"); ভঙ্গ;
	পক্ষ NFA_HEX:	STRCPY(code, "NFA_HEX"); ভঙ্গ;
	পক্ষ NFA_NHEX:	STRCPY(code, "NFA_NHEX"); ভঙ্গ;
	পক্ষ NFA_OCTAL:	STRCPY(code, "NFA_OCTAL"); ভঙ্গ;
	পক্ষ NFA_NOCTAL:STRCPY(code, "NFA_NOCTAL"); ভঙ্গ;
	পক্ষ NFA_WORD:	STRCPY(code, "NFA_WORD"); ভঙ্গ;
	পক্ষ NFA_NWORD:	STRCPY(code, "NFA_NWORD"); ভঙ্গ;
	পক্ষ NFA_HEAD:	STRCPY(code, "NFA_HEAD"); ভঙ্গ;
	পক্ষ NFA_NHEAD:	STRCPY(code, "NFA_NHEAD"); ভঙ্গ;
	পক্ষ NFA_ALPHA:	STRCPY(code, "NFA_ALPHA"); ভঙ্গ;
	পক্ষ NFA_NALPHA:STRCPY(code, "NFA_NALPHA"); ভঙ্গ;
	পক্ষ NFA_LOWER:	STRCPY(code, "NFA_LOWER"); ভঙ্গ;
	পক্ষ NFA_NLOWER:STRCPY(code, "NFA_NLOWER"); ভঙ্গ;
	পক্ষ NFA_UPPER:	STRCPY(code, "NFA_UPPER"); ভঙ্গ;
	পক্ষ NFA_NUPPER:STRCPY(code, "NFA_NUPPER"); ভঙ্গ;
	পক্ষ NFA_LOWER_IC:  STRCPY(code, "NFA_LOWER_IC"); ভঙ্গ;
	পক্ষ NFA_NLOWER_IC: STRCPY(code, "NFA_NLOWER_IC"); ভঙ্গ;
	পক্ষ NFA_UPPER_IC:  STRCPY(code, "NFA_UPPER_IC"); ভঙ্গ;
	পক্ষ NFA_NUPPER_IC: STRCPY(code, "NFA_NUPPER_IC"); ভঙ্গ;

	স্ৱভাৱ:
	    STRCPY(code, "CHAR(x)");
	    code[5] = c;
    }

    যদি (addnl == TRUE)
	STRCAT(code, " + NEWLINE ");

}

# ifdef ENABLE_LOG
স্থির FILE *log_fd;
স্থির char_u e_log_open_failed[] = N_("Could not open temporary log file for writing, displaying on stderr... ");

/*
 * Print the postfix notation of the current regexp.
 */
    স্থির শূন্য
nfa_postfix_dump(char_u *ৰাশিমালা, পূর্ণাঙ্ক retval)
{
    পূর্ণাঙ্ক *p;
    FILE *f;

    f = fopen(NFA_REGEXP_DUMP_LOG, "a");
    যদি (f == রিক্ত)
	ঘূৰাই;

    fprintf(f, "\n-------------------------\n");
    যদি (retval == FAIL)
	fprintf(f, ">>> NFA engine failed... \n");
    অন্যথা যদি (retval == OK)
	fprintf(f, ">>> NFA engine succeeded !\n");
    fprintf(f, "Regexp: \"%s\"\nPostfix notation (char): \"", ৰাশিমালা);
    বাবে (p = post_start; *p && p < post_ptr; p++)
    {
	nfa_set_code(*p);
	fprintf(f, "%s, ", code);
    }
    fprintf(f, "\"\nPostfix notation (int): ");
    বাবে (p = post_start; *p && p < post_ptr; p++)
	fprintf(f, "%d ", *p);
    fprintf(f, "\n\n");
    fclose(f);
}

/*
 * Print the NFA starting with a root node "state".
 */
    স্থির শূন্য
nfa_print_state(FILE *debugf, nfa_state_T *state)
{
    garray_T indent;

    ga_init2(&indent, 1, 64);
    ga_append(&indent, '\0');
    nfa_print_state2(debugf, state, &indent);
    ga_clear(&indent);
}

    স্থির শূন্য
nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent)
{
    char_u  *p;

    যদি (state == রিক্ত)
	ঘূৰাই;

    fprintf(debugf, "(%2d)", abs(state->id));

    // Output indent
    p = (char_u *)indent->ga_data;
    যদি (indent->ga_len >= 3)
    {
	পূর্ণাঙ্ক	last = indent->ga_len - 3;
	char_u	save[2];

	STRNCPY(save, &p[last], 2);
	memcpy(&p[last], "+-", 2);
	fprintf(debugf, " %s", p);
	STRNCPY(&p[last], save, 2);
    }
    অন্যথা
	fprintf(debugf, " %s", p);

    nfa_set_code(state->c);
    fprintf(debugf, "%s (%d) (id=%d) val=%d\n",
		 code,
		 state->c,
		 abs(state->id),
		 state->val);
    যদি (state->id < 0)
	ঘূৰাই;

    state->id = abs(state->id) * -1;

    // grow indent for state->out
    indent->ga_len -= 1;
    যদি (state->out1)
	GA_CONCAT_LITERAL(indent, "| ");
    অন্যথা
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
    স্থির শূন্য
nfa_dump(nfa_regprog_T *prog)
{
    FILE *debugf = fopen(NFA_REGEXP_DUMP_LOG, "a");

    যদি (debugf == রিক্ত)
	ঘূৰাই;

    nfa_print_state(debugf, prog->start);

    যদি (prog->reganch)
	fprintf(debugf, "reganch: %d\n", prog->reganch);
    যদি (prog->regstart != NUL)
	fprintf(debugf, "regstart: %c (decimal: %d)\n",
		prog->regstart, prog->regstart);
    যদি (prog->match_text != রিক্ত)
	fprintf(debugf, "match_text: \"%s\"\n", prog->match_text);

    fclose(debugf);
}
# endif	    // ENABLE_LOG
#endif	    // DEBUG

/*
 * Parse r.e. @expr and convert it into postfix form.
 * Return the postfix string on success, NULL otherwise.
 */
    স্থির পূর্ণাঙ্ক *
re2post(শূন্য)
{
    যদি (nfa_reg(REG_NOPAREN) == FAIL)
	ঘূৰাই রিক্ত;
    EMIT(NFA_MOPEN);
    ঘূৰাই post_start;
}

// NB. Some of the code below is inspired by Russ's.

/*
 * Represents an NFA state plus zero or one or two arrows exiting.
 * if c == MATCH, no arrows out; matching state.
 * If c == SPLIT, unlabeled arrows to out and out1 (if != NULL).
 * If c < 256, labeled arrow with character c to out.
 */

স্থির nfa_state_T	*state_ptr; // points to nfa_prog->state

/*
 * Allocate and initialize nfa_state_T.
 */
    স্থির nfa_state_T *
alloc_state(পূর্ণাঙ্ক c, nfa_state_T *out, nfa_state_T *out1)
{
    nfa_state_T *s;

    যদি (istate >= nstate)
	ঘূৰাই রিক্ত;

    s = &state_ptr[istate++];

    s->c    = c;
    s->out  = out;
    s->out1 = out1;
    s->val  = 0;

    s->id   = istate;
    s->lastlist[0] = 0;
    s->lastlist[1] = 0;

    ঘূৰাই s;
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
প্রকারনির্দেশ সঙ্ঘ Ptrlist Ptrlist;
সঙ্ঘ Ptrlist
{
    Ptrlist	*next;
    nfa_state_T	*s;
};

সংরচনা Frag
{
    nfa_state_T *start;
    Ptrlist	*out;
};
প্রকারনির্দেশ সংরচনা Frag Frag_T;

// Reused across compilations by post2nfa(); freed in free_regexp_stuff().
স্থির Frag_T *nfa_স্তূপ;
স্থির পূর্ণাঙ্ক nfa_স্তূপ_len;

/*
 * Initialize a Frag_T struct and return it.
 */
    স্থির Frag_T
frag(nfa_state_T *start, Ptrlist *out)
{
    Frag_T n;

    n.start = start;
    n.out = out;
    ঘূৰাই n;
}

/*
 * Create singleton list containing just outp.
 */
    স্থির Ptrlist *
list1(
    nfa_state_T	**outp)
{
    Ptrlist *l;

    l = (Ptrlist *)outp;
    l->next = রিক্ত;
    ঘূৰাই l;
}

/*
 * Patch the list of states at out to point to start.
 */
    স্থির শূন্য
patch(Ptrlist *l, nfa_state_T *s)
{
    Ptrlist *next;

    বাবে (; l; l = next)
    {
	next = l->next;
	l->s = s;
    }
}


/*
 * Join the two lists l1 and l2, returning the combination.
 */
    স্থির Ptrlist *
append(Ptrlist *l1, Ptrlist *l2)
{
    Ptrlist *oldl1;

    oldl1 = l1;
    যেতিয়ালৈ (l1->next)
	l1 = l1->next;
    l1->next = l2;
    ঘূৰাই oldl1;
}

/*
 * Stack used for transforming postfix form into NFA.
 */
স্থির Frag_T empty;

    স্থির শূন্য
st_error(পূর্ণাঙ্ক *postfix UNUSED, পূর্ণাঙ্ক *end UNUSED, পূর্ণাঙ্ক *p UNUSED)
{
#ifdef NFA_REGEXP_ERROR_LOG
    FILE *df;
    পূর্ণাঙ্ক *p2;

    df = fopen(NFA_REGEXP_ERROR_LOG, "a");
    যদি (df)
    {
	fprintf(df, "Error popping the stack!\n");
# ifdef DEBUG
	fprintf(df, "Current regexp is \"%s\"\n", nfa_regengine.ৰাশিমালা);
# endif
	fprintf(df, "Postfix form is: ");
# ifdef DEBUG
	বাবে (p2 = postfix; p2 < end; p2++)
	{
	    nfa_set_code(*p2);
	    fprintf(df, "%s, ", code);
	}
	nfa_set_code(*p);
	fprintf(df, "\nCurrent position is: ");
	বাবে (p2 = postfix; p2 <= p; p2 ++)
	{
	    nfa_set_code(*p2);
	    fprintf(df, "%s, ", code);
	}
# অন্যথা
	বাবে (p2 = postfix; p2 < end; p2++)
	    fprintf(df, "%d, ", *p2);
	fprintf(df, "\nCurrent position is: ");
	বাবে (p2 = postfix; p2 <= p; p2 ++)
	    fprintf(df, "%d, ", *p2);
# endif
	fprintf(df, "\n--------------------------\n");
	fclose(df);
    }
#endif
    emsg(_(e_nfa_regexp_could_not_বেৰ_কৰো_স্তূপ));
}

/*
 * Push an item onto the stack.
 */
    স্থির শূন্য
st_ঠেলো(Frag_T s, Frag_T **p, Frag_T *স্তূপ_end)
{
    Frag_T *stackp = *p;

    যদি (stackp >= স্তূপ_end)
	ঘূৰাই;
    *stackp = s;
    *p = *p + 1;
}

/*
 * Pop an item from the stack.
 */
    স্থির Frag_T
st_বেৰ_কৰো(Frag_T **p, Frag_T *স্তূপ)
{
    Frag_T *stackp;

    *p = *p - 1;
    stackp = *p;
    যদি (stackp < স্তূপ)
	ঘূৰাই empty;
    ঘূৰাই **p;
}

/*
 * Estimate the maximum byte length of anything matching "state".
 * When unknown or unlimited return -1.
 */
    স্থির পূর্ণাঙ্ক
nfa_max_width(nfa_state_T *startstate, পূর্ণাঙ্ক depth)
{
    পূর্ণাঙ্ক		    l, r;
    nfa_state_T	    *state = startstate;
    পূর্ণাঙ্ক		    len = 0;

    // detect looping in a NFA_SPLIT
    যদি (depth > 4)
	ঘূৰাই -1;

    যেতিয়ালৈ (state != রিক্ত)
    {
	ৱিকল্প (state->c)
	{
	    পক্ষ NFA_END_INVISIBLE:
	    পক্ষ NFA_END_INVISIBLE_NEG:
		// the end, return what we have
		ঘূৰাই len;

	    পক্ষ NFA_SPLIT:
		// two alternatives, use the maximum
		l = nfa_max_width(state->out, depth + 1);
		r = nfa_max_width(state->out1, depth + 1);
		যদি (l < 0 || r < 0)
		    ঘূৰাই -1;
		ঘূৰাই len + (l > r ? l : r);

	    পক্ষ NFA_ANY:
	    পক্ষ NFA_START_COLL:
	    পক্ষ NFA_START_NEG_COLL:
		// matches some character, including composing chars
		যদি (enc_utf8)
		    len += MB_MAXBYTES;
		অন্যথা যদি (has_mbyte)
		    len += 2;
		অন্যথা
		    ++len;
		যদি (state->c != NFA_ANY)
		{
		    // Skip over the compiled collection.
		    // malformed NFAs must not crash width estimation.
		    যদি (state->out1 == রিক্ত || state->out1->out == রিক্ত)
			ঘূৰাই -1;
		    state = state->out1->out;
		    অনুৱর্ত;
		}
		ভঙ্গ;

	    পক্ষ NFA_DIGIT:
	    পক্ষ NFA_WHITE:
	    পক্ষ NFA_HEX:
	    পক্ষ NFA_OCTAL:
		// ascii
		++len;
		ভঙ্গ;

	    পক্ষ NFA_IDENT:
	    পক্ষ NFA_SIDENT:
	    পক্ষ NFA_KWORD:
	    পক্ষ NFA_SKWORD:
	    পক্ষ NFA_FNAME:
	    পক্ষ NFA_SFNAME:
	    পক্ষ NFA_PRINT:
	    পক্ষ NFA_SPRINT:
	    পক্ষ NFA_NWHITE:
	    পক্ষ NFA_NDIGIT:
	    পক্ষ NFA_NHEX:
	    পক্ষ NFA_NOCTAL:
	    পক্ষ NFA_WORD:
	    পক্ষ NFA_NWORD:
	    পক্ষ NFA_HEAD:
	    পক্ষ NFA_NHEAD:
	    পক্ষ NFA_ALPHA:
	    পক্ষ NFA_NALPHA:
	    পক্ষ NFA_LOWER:
	    পক্ষ NFA_NLOWER:
	    পক্ষ NFA_UPPER:
	    পক্ষ NFA_NUPPER:
	    পক্ষ NFA_LOWER_IC:
	    পক্ষ NFA_NLOWER_IC:
	    পক্ষ NFA_UPPER_IC:
	    পক্ষ NFA_NUPPER_IC:
	    পক্ষ NFA_ANY_COMPOSING:
		// possibly non-ascii
		যদি (has_mbyte)
		    len += 3;
		অন্যথা
		    ++len;
		ভঙ্গ;

	    পক্ষ NFA_START_INVISIBLE:
	    পক্ষ NFA_START_INVISIBLE_NEG:
	    পক্ষ NFA_START_INVISIBLE_BEFORE:
	    পক্ষ NFA_START_INVISIBLE_BEFORE_NEG:
		// zero-width, out1 points to the END state
		state = state->out1->out;
		অনুৱর্ত;

	    পক্ষ NFA_BACKREF1:
	    পক্ষ NFA_BACKREF2:
	    পক্ষ NFA_BACKREF3:
	    পক্ষ NFA_BACKREF4:
	    পক্ষ NFA_BACKREF5:
	    পক্ষ NFA_BACKREF6:
	    পক্ষ NFA_BACKREF7:
	    পক্ষ NFA_BACKREF8:
	    পক্ষ NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	    পক্ষ NFA_ZREF1:
	    পক্ষ NFA_ZREF2:
	    পক্ষ NFA_ZREF3:
	    পক্ষ NFA_ZREF4:
	    পক্ষ NFA_ZREF5:
	    পক্ষ NFA_ZREF6:
	    পক্ষ NFA_ZREF7:
	    পক্ষ NFA_ZREF8:
	    পক্ষ NFA_ZREF9:
#endif
	    পক্ষ NFA_NEWL:
	    পক্ষ NFA_SKIP:
		// unknown width
		ঘূৰাই -1;

	    পক্ষ NFA_BOL:
	    পক্ষ NFA_EOL:
	    পক্ষ NFA_BOF:
	    পক্ষ NFA_EOF:
	    পক্ষ NFA_BOW:
	    পক্ষ NFA_EOW:
	    পক্ষ NFA_MOPEN:
	    পক্ষ NFA_MOPEN1:
	    পক্ষ NFA_MOPEN2:
	    পক্ষ NFA_MOPEN3:
	    পক্ষ NFA_MOPEN4:
	    পক্ষ NFA_MOPEN5:
	    পক্ষ NFA_MOPEN6:
	    পক্ষ NFA_MOPEN7:
	    পক্ষ NFA_MOPEN8:
	    পক্ষ NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	    পক্ষ NFA_ZOPEN:
	    পক্ষ NFA_ZOPEN1:
	    পক্ষ NFA_ZOPEN2:
	    পক্ষ NFA_ZOPEN3:
	    পক্ষ NFA_ZOPEN4:
	    পক্ষ NFA_ZOPEN5:
	    পক্ষ NFA_ZOPEN6:
	    পক্ষ NFA_ZOPEN7:
	    পক্ষ NFA_ZOPEN8:
	    পক্ষ NFA_ZOPEN9:
	    পক্ষ NFA_ZCLOSE:
	    পক্ষ NFA_ZCLOSE1:
	    পক্ষ NFA_ZCLOSE2:
	    পক্ষ NFA_ZCLOSE3:
	    পক্ষ NFA_ZCLOSE4:
	    পক্ষ NFA_ZCLOSE5:
	    পক্ষ NFA_ZCLOSE6:
	    পক্ষ NFA_ZCLOSE7:
	    পক্ষ NFA_ZCLOSE8:
	    পক্ষ NFA_ZCLOSE9:
#endif
	    পক্ষ NFA_MCLOSE:
	    পক্ষ NFA_MCLOSE1:
	    পক্ষ NFA_MCLOSE2:
	    পক্ষ NFA_MCLOSE3:
	    পক্ষ NFA_MCLOSE4:
	    পক্ষ NFA_MCLOSE5:
	    পক্ষ NFA_MCLOSE6:
	    পক্ষ NFA_MCLOSE7:
	    পক্ষ NFA_MCLOSE8:
	    পক্ষ NFA_MCLOSE9:
	    পক্ষ NFA_NOPEN:
	    পক্ষ NFA_NCLOSE:

	    পক্ষ NFA_LNUM_GT:
	    পক্ষ NFA_LNUM_LT:
	    পক্ষ NFA_COL_GT:
	    পক্ষ NFA_COL_LT:
	    পক্ষ NFA_VCOL_GT:
	    পক্ষ NFA_VCOL_LT:
	    পক্ষ NFA_MARK_GT:
	    পক্ষ NFA_MARK_LT:
	    পক্ষ NFA_VISUAL:
	    পক্ষ NFA_LNUM:
	    পক্ষ NFA_CURSOR:
	    পক্ষ NFA_COL:
	    পক্ষ NFA_VCOL:
	    পক্ষ NFA_MARK:

	    পক্ষ NFA_ZSTART:
	    পক্ষ NFA_ZEND:
	    পক্ষ NFA_OPT_CHARS:
	    পক্ষ NFA_EMPTY:
	    পক্ষ NFA_START_PATTERN:
	    পক্ষ NFA_END_PATTERN:
	    পক্ষ NFA_COMPOSING:
	    পক্ষ NFA_END_COMPOSING:
		// zero-width
		ভঙ্গ;

	    স্ৱভাৱ:
		যদি (state->c < 0)
		    // don't know what this is
		    ঘূৰাই -1;
		// normal character
		len += MB_CHAR2LEN(state->c);
		ভঙ্গ;
	}

	// normal way to continue
	state = state->out;
    }

    // unrecognized, "cannot happen"
    ঘূৰাই -1;
}

/*
 * Convert a postfix form into its equivalent NFA.
 * Return the NFA start state on success, NULL otherwise.
 */
    স্থির nfa_state_T *
post2nfa(পূর্ণাঙ্ক *postfix, পূর্ণাঙ্ক *end, পূর্ণাঙ্ক nfa_calc_size)
{
    পূর্ণাঙ্ক		*p;
    পূর্ণাঙ্ক		mopen;
    পূর্ণাঙ্ক		mclose;
    Frag_T	*স্তূপ = রিক্ত;
    Frag_T	*stackp = রিক্ত;
    Frag_T	*স্তূপ_end = রিক্ত;
    Frag_T	e1;
    Frag_T	e2;
    Frag_T	e;
    nfa_state_T	*s;
    nfa_state_T	*s1;
    nfa_state_T	*matchstate;
    nfa_state_T	*ret = রিক্ত;

    যদি (postfix == রিক্ত)
	ঘূৰাই রিক্ত;

#define ঠেলো_2(s)	    st_ঠেলো((s), &stackp, স্তূপ_end)
#define বেৰ_কৰো_2()	    st_বেৰ_কৰো(&stackp, স্তূপ);		\
		    যদি (stackp < স্তূপ)			\
		    {					\
			st_error(postfix, end, p);	\
			ঘূৰাই রিক্ত;			\
		    }

    যদি (nfa_calc_size == FALSE)
    {
	// Reuse the fragment stack across compilations, growing when needed;
	// it is freed in free_regexp_stuff().
	যদি (nfa_স্তূপ == রিক্ত || nfa_স্তূপ_len < nstate + 1)
	{
	    vim_free(nfa_স্তূপ);
	    nfa_স্তূপ = ALLOC_MULT(Frag_T, nstate + 1);
	    যদি (nfa_স্তূপ == রিক্ত)
	    {
		nfa_স্তূপ_len = 0;
		ঘূৰাই রিক্ত;
	    }
	    nfa_স্তূপ_len = nstate + 1;
	}
	স্তূপ = nfa_স্তূপ;
	stackp = স্তূপ;
	স্তূপ_end = স্তূপ + (nstate + 1);
    }

    বাবে (p = postfix; p < end; ++p)
    {
	ৱিকল্প (*p)
	{
	পক্ষ NFA_CONCAT:
	    // Concatenation.
	    // Pay attention: this operator does not exist in the r.e. itself
	    // (it is implicit, really).  It is added when r.e. is translated
	    // to postfix form in re2post().
	    যদি (nfa_calc_size == TRUE)
	    {
		// nstate += 0;
		ভঙ্গ;
	    }
	    e2 = বেৰ_কৰো_2();
	    e1 = বেৰ_কৰো_2();
	    patch(e1.out, e2.start);
	    ঠেলো_2(frag(e1.start, e2.out));
	    ভঙ্গ;

	পক্ষ NFA_OR:
	    // Alternation
	    যদি (nfa_calc_size == TRUE)
	    {
		nstate++;
		ভঙ্গ;
	    }
	    e2 = বেৰ_কৰো_2();
	    e1 = বেৰ_কৰো_2();
	    s = alloc_state(NFA_SPLIT, e1.start, e2.start);
	    যদি (s == রিক্ত)
		গচ্ছ theend;
	    ঠেলো_2(frag(s, append(e1.out, e2.out)));
	    ভঙ্গ;

	পক্ষ NFA_STAR:
	    // Zero or more, prefer more
	    যদি (nfa_calc_size == TRUE)
	    {
		nstate++;
		ভঙ্গ;
	    }
	    e = বেৰ_কৰো_2();
	    s = alloc_state(NFA_SPLIT, e.start, রিক্ত);
	    যদি (s == রিক্ত)
		গচ্ছ theend;
	    patch(e.out, s);
	    ঠেলো_2(frag(s, list1(&s->out1)));
	    ভঙ্গ;

	পক্ষ NFA_STAR_NONGREEDY:
	    // Zero or more, prefer zero
	    যদি (nfa_calc_size == TRUE)
	    {
		nstate++;
		ভঙ্গ;
	    }
	    e = বেৰ_কৰো_2();
	    s = alloc_state(NFA_SPLIT, রিক্ত, e.start);
	    যদি (s == রিক্ত)
		গচ্ছ theend;
	    patch(e.out, s);
	    ঠেলো_2(frag(s, list1(&s->out)));
	    ভঙ্গ;

	পক্ষ NFA_QUEST:
	    // one or zero atoms=> greedy match
	    যদি (nfa_calc_size == TRUE)
	    {
		nstate++;
		ভঙ্গ;
	    }
	    e = বেৰ_কৰো_2();
	    s = alloc_state(NFA_SPLIT, e.start, রিক্ত);
	    যদি (s == রিক্ত)
		গচ্ছ theend;
	    ঠেলো_2(frag(s, append(e.out, list1(&s->out1))));
	    ভঙ্গ;

	পক্ষ NFA_QUEST_NONGREEDY:
	    // zero or one atoms => non-greedy match
	    যদি (nfa_calc_size == TRUE)
	    {
		nstate++;
		ভঙ্গ;
	    }
	    e = বেৰ_কৰো_2();
	    s = alloc_state(NFA_SPLIT, রিক্ত, e.start);
	    যদি (s == রিক্ত)
		গচ্ছ theend;
	    ঠেলো_2(frag(s, append(e.out, list1(&s->out))));
	    ভঙ্গ;

	পক্ষ NFA_END_COLL:
	পক্ষ NFA_END_NEG_COLL:
	    // On the stack is the sequence starting with NFA_START_COLL or
	    // NFA_START_NEG_COLL and all possible characters. Patch it to
	    // add the output to the start.
	    যদি (nfa_calc_size == TRUE)
	    {
		nstate++;
		ভঙ্গ;
	    }
	    e = বেৰ_কৰো_2();
	    s = alloc_state(NFA_END_COLL, রিক্ত, রিক্ত);
	    যদি (s == রিক্ত)
		গচ্ছ theend;
	    patch(e.out, s);
	    e.start->out1 = s;
	    ঠেলো_2(frag(e.start, list1(&s->out)));
	    ভঙ্গ;

	পক্ষ NFA_RANGE:
	    // Before this are two characters, the low and high end of a
	    // range.  Turn them into two states with MIN and MAX.
	    যদি (nfa_calc_size == TRUE)
	    {
		// nstate += 0;
		ভঙ্গ;
	    }
	    e2 = বেৰ_কৰো_2();
	    e1 = বেৰ_কৰো_2();
	    e2.start->val = e2.start->c;
	    e2.start->c = NFA_RANGE_MAX;
	    e1.start->val = e1.start->c;
	    e1.start->c = NFA_RANGE_MIN;
	    patch(e1.out, e2.start);
	    ঠেলো_2(frag(e1.start, e2.out));
	    ভঙ্গ;

	পক্ষ NFA_EMPTY:
	    // 0-length, used in a repetition with max/min count of 0
	    যদি (nfa_calc_size == TRUE)
	    {
		nstate++;
		ভঙ্গ;
	    }
	    s = alloc_state(NFA_EMPTY, রিক্ত, রিক্ত);
	    যদি (s == রিক্ত)
		গচ্ছ theend;
	    ঠেলো_2(frag(s, list1(&s->out)));
	    ভঙ্গ;

	পক্ষ NFA_OPT_CHARS:
	  {
	    পূর্ণাঙ্ক    n;

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
	    যদি (nfa_calc_size == TRUE)
	    {
		nstate += n;
		ভঙ্গ;
	    }
	    s = রিক্ত; // avoid compiler warning
	    e1.out = রিক্ত; // stores list with out1's
	    s1 = রিক্ত; // previous NFA_SPLIT to connect to
	    যেতিয়ালৈ (n-- > 0)
	    {
		e = বেৰ_কৰো_2(); // get character
		s = alloc_state(NFA_SPLIT, e.start, রিক্ত);
		যদি (s == রিক্ত)
		    গচ্ছ theend;
		যদি (e1.out == রিক্ত)
		    e1 = e;
		patch(e.out, s1);
		append(e1.out, list1(&s->out1));
		s1 = s;
	    }
	    ঠেলো_2(frag(s, e1.out));
	    ভঙ্গ;
	  }

	পক্ষ NFA_PREV_ATOM_NO_WIDTH:
	পক্ষ NFA_PREV_ATOM_NO_WIDTH_NEG:
	পক্ষ NFA_PREV_ATOM_JUST_BEFORE:
	পক্ষ NFA_PREV_ATOM_JUST_BEFORE_NEG:
	পক্ষ NFA_PREV_ATOM_LIKE_PATTERN:
	  {
	    পূর্ণাঙ্ক before = (*p == NFA_PREV_ATOM_JUST_BEFORE
				      || *p == NFA_PREV_ATOM_JUST_BEFORE_NEG);
	    পূর্ণাঙ্ক pattern = (*p == NFA_PREV_ATOM_LIKE_PATTERN);
	    পূর্ণাঙ্ক start_state;
	    পূর্ণাঙ্ক end_state;
	    পূর্ণাঙ্ক n = 0;
	    nfa_state_T *zend;
	    nfa_state_T *skip;

	    ৱিকল্প (*p)
	    {
		পক্ষ NFA_PREV_ATOM_NO_WIDTH:
		    start_state = NFA_START_INVISIBLE;
		    end_state = NFA_END_INVISIBLE;
		    ভঙ্গ;
		পক্ষ NFA_PREV_ATOM_NO_WIDTH_NEG:
		    start_state = NFA_START_INVISIBLE_NEG;
		    end_state = NFA_END_INVISIBLE_NEG;
		    ভঙ্গ;
		পক্ষ NFA_PREV_ATOM_JUST_BEFORE:
		    start_state = NFA_START_INVISIBLE_BEFORE;
		    end_state = NFA_END_INVISIBLE;
		    ভঙ্গ;
		পক্ষ NFA_PREV_ATOM_JUST_BEFORE_NEG:
		    start_state = NFA_START_INVISIBLE_BEFORE_NEG;
		    end_state = NFA_END_INVISIBLE_NEG;
		    ভঙ্গ;
		স্ৱভাৱ: // NFA_PREV_ATOM_LIKE_PATTERN:
		    start_state = NFA_START_PATTERN;
		    end_state = NFA_END_PATTERN;
		    ভঙ্গ;
	    }

	    যদি (before)
		n = *++p; // get the count

	    // The \@= operator: match the preceding atom with zero width.
	    // The \@! operator: no match for the preceding atom.
	    // The \@<= operator: match for the preceding atom.
	    // The \@<! operator: no match for the preceding atom.
	    // Surrounds the preceding atom with START_INVISIBLE and
	    // END_INVISIBLE, similarly to MOPEN.

	    যদি (nfa_calc_size == TRUE)
	    {
		nstate += pattern ? 4 : 2;
		ভঙ্গ;
	    }
	    e = বেৰ_কৰো_2();
	    s1 = alloc_state(end_state, রিক্ত, রিক্ত);
	    যদি (s1 == রিক্ত)
		গচ্ছ theend;

	    s = alloc_state(start_state, e.start, s1);
	    যদি (s == রিক্ত)
		গচ্ছ theend;
	    যদি (pattern)
	    {
		// NFA_ZEND -> NFA_END_PATTERN -> NFA_SKIP -> what follows.
		skip = alloc_state(NFA_SKIP, রিক্ত, রিক্ত);
		যদি (skip == রিক্ত)
		    গচ্ছ theend;
		zend = alloc_state(NFA_ZEND, s1, রিক্ত);
		যদি (zend == রিক্ত)
		    গচ্ছ theend;
		s1->out= skip;
		patch(e.out, zend);
		ঠেলো_2(frag(s, list1(&skip->out)));
	    }
	    অন্যথা
	    {
		patch(e.out, s1);
		ঠেলো_2(frag(s, list1(&s1->out)));
		যদি (before)
		{
		    যদি (n <= 0)
			// See if we can guess the maximum width, it avoids a
			// lot of pointless tries.
			n = nfa_max_width(e.start, 0);
		    s->val = n; // store the count
		}
	    }
	    ভঙ্গ;
	  }

	পক্ষ NFA_COMPOSING:	// char with composing char
#যদি 0
	    // TODO
	    যদি (regflags & RF_ICOMBINE)
	    {
		// use the base character only
	    }
#endif
	    // FALLTHROUGH

	পক্ষ NFA_MOPEN:	// \( \) Submatch
	পক্ষ NFA_MOPEN1:
	পক্ষ NFA_MOPEN2:
	পক্ষ NFA_MOPEN3:
	পক্ষ NFA_MOPEN4:
	পক্ষ NFA_MOPEN5:
	পক্ষ NFA_MOPEN6:
	পক্ষ NFA_MOPEN7:
	পক্ষ NFA_MOPEN8:
	পক্ষ NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	পক্ষ NFA_ZOPEN:	// \z( \) Submatch
	পক্ষ NFA_ZOPEN1:
	পক্ষ NFA_ZOPEN2:
	পক্ষ NFA_ZOPEN3:
	পক্ষ NFA_ZOPEN4:
	পক্ষ NFA_ZOPEN5:
	পক্ষ NFA_ZOPEN6:
	পক্ষ NFA_ZOPEN7:
	পক্ষ NFA_ZOPEN8:
	পক্ষ NFA_ZOPEN9:
#endif
	পক্ষ NFA_NOPEN:	// \%( \) "Invisible Submatch"
	    যদি (nfa_calc_size == TRUE)
	    {
		nstate += 2;
		ভঙ্গ;
	    }

	    mopen = *p;
	    ৱিকল্প (*p)
	    {
		পক্ষ NFA_NOPEN: mclose = NFA_NCLOSE; ভঙ্গ;
#ifdef FEAT_SYN_HL
		পক্ষ NFA_ZOPEN: mclose = NFA_ZCLOSE; ভঙ্গ;
		পক্ষ NFA_ZOPEN1: mclose = NFA_ZCLOSE1; ভঙ্গ;
		পক্ষ NFA_ZOPEN2: mclose = NFA_ZCLOSE2; ভঙ্গ;
		পক্ষ NFA_ZOPEN3: mclose = NFA_ZCLOSE3; ভঙ্গ;
		পক্ষ NFA_ZOPEN4: mclose = NFA_ZCLOSE4; ভঙ্গ;
		পক্ষ NFA_ZOPEN5: mclose = NFA_ZCLOSE5; ভঙ্গ;
		পক্ষ NFA_ZOPEN6: mclose = NFA_ZCLOSE6; ভঙ্গ;
		পক্ষ NFA_ZOPEN7: mclose = NFA_ZCLOSE7; ভঙ্গ;
		পক্ষ NFA_ZOPEN8: mclose = NFA_ZCLOSE8; ভঙ্গ;
		পক্ষ NFA_ZOPEN9: mclose = NFA_ZCLOSE9; ভঙ্গ;
#endif
		পক্ষ NFA_COMPOSING: mclose = NFA_END_COMPOSING; ভঙ্গ;
		স্ৱভাৱ:
		    // NFA_MOPEN, NFA_MOPEN1 .. NFA_MOPEN9
		    mclose = *p + NSUBEXP;
		    ভঙ্গ;
	    }

	    // Allow "NFA_MOPEN" as a valid postfix representation for
	    // the empty regexp "". In this case, the NFA will be
	    // NFA_MOPEN -> NFA_MCLOSE. Note that this also allows
	    // empty groups of parenthesis, and empty mbyte chars
	    যদি (stackp == স্তূপ)
	    {
		s = alloc_state(mopen, রিক্ত, রিক্ত);
		যদি (s == রিক্ত)
		    গচ্ছ theend;
		s1 = alloc_state(mclose, রিক্ত, রিক্ত);
		যদি (s1 == রিক্ত)
		    গচ্ছ theend;
		patch(list1(&s->out), s1);
		ঠেলো_2(frag(s, list1(&s1->out)));
		ভঙ্গ;
	    }

	    // At least one node was emitted before NFA_MOPEN, so
	    // at least one node will be between NFA_MOPEN and NFA_MCLOSE
	    e = বেৰ_কৰো_2();
	    s = alloc_state(mopen, e.start, রিক্ত);   // `('
	    যদি (s == রিক্ত)
		গচ্ছ theend;

	    s1 = alloc_state(mclose, রিক্ত, রিক্ত);   // `)'
	    যদি (s1 == রিক্ত)
		গচ্ছ theend;
	    patch(e.out, s1);

	    যদি (mopen == NFA_COMPOSING)
		// COMPOSING->out1 = END_COMPOSING
		patch(list1(&s->out1), s1);

	    ঠেলো_2(frag(s, list1(&s1->out)));
	    ভঙ্গ;

	পক্ষ NFA_BACKREF1:
	পক্ষ NFA_BACKREF2:
	পক্ষ NFA_BACKREF3:
	পক্ষ NFA_BACKREF4:
	পক্ষ NFA_BACKREF5:
	পক্ষ NFA_BACKREF6:
	পক্ষ NFA_BACKREF7:
	পক্ষ NFA_BACKREF8:
	পক্ষ NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	পক্ষ NFA_ZREF1:
	পক্ষ NFA_ZREF2:
	পক্ষ NFA_ZREF3:
	পক্ষ NFA_ZREF4:
	পক্ষ NFA_ZREF5:
	পক্ষ NFA_ZREF6:
	পক্ষ NFA_ZREF7:
	পক্ষ NFA_ZREF8:
	পক্ষ NFA_ZREF9:
#endif
	    যদি (nfa_calc_size == TRUE)
	    {
		nstate += 2;
		ভঙ্গ;
	    }
	    s = alloc_state(*p, রিক্ত, রিক্ত);
	    যদি (s == রিক্ত)
		গচ্ছ theend;
	    s1 = alloc_state(NFA_SKIP, রিক্ত, রিক্ত);
	    যদি (s1 == রিক্ত)
		গচ্ছ theend;
	    patch(list1(&s->out), s1);
	    ঠেলো_2(frag(s, list1(&s1->out)));
	    ভঙ্গ;

	পক্ষ NFA_LNUM:
	পক্ষ NFA_LNUM_GT:
	পক্ষ NFA_LNUM_LT:
	পক্ষ NFA_VCOL:
	পক্ষ NFA_VCOL_GT:
	পক্ষ NFA_VCOL_LT:
	পক্ষ NFA_COL:
	পক্ষ NFA_COL_GT:
	পক্ষ NFA_COL_LT:
	পক্ষ NFA_MARK:
	পক্ষ NFA_MARK_GT:
	পক্ষ NFA_MARK_LT:
	  {
	    পূর্ণাঙ্ক n = *++p; // lnum, col or mark name

	    যদি (nfa_calc_size == TRUE)
	    {
		nstate += 1;
		ভঙ্গ;
	    }
	    s = alloc_state(p[-1], রিক্ত, রিক্ত);
	    যদি (s == রিক্ত)
		গচ্ছ theend;
	    s->val = n;
	    ঠেলো_2(frag(s, list1(&s->out)));
	    ভঙ্গ;
	  }

	পক্ষ NFA_ZSTART:
	পক্ষ NFA_ZEND:
	স্ৱভাৱ:
	    // Operands
	    যদি (nfa_calc_size == TRUE)
	    {
		nstate++;
		ভঙ্গ;
	    }
	    s = alloc_state(*p, রিক্ত, রিক্ত);
	    যদি (s == রিক্ত)
		গচ্ছ theend;
	    ঠেলো_2(frag(s, list1(&s->out)));
	    ভঙ্গ;

	} // switch(*p)

    } // for (p = postfix; *p; ++p)

    যদি (nfa_calc_size == TRUE)
    {
	nstate++;
	গচ্ছ theend;	// Return value when counting size is ignored anyway
    }

    e = বেৰ_কৰো_2();
    যদি (stackp != স্তূপ)
	EMSG_RET_NULL(_(e_nfa_regexp_while_converting_from_postfix_to_nfa_too_many_stats_বাম_on_স্তূপ));

    যদি (istate >= nstate)
	EMSG_RET_NULL(_(e_nfa_regexp_not_enough_space_to_store_whole_nfa));

    matchstate = &state_ptr[istate++]; // the match state
    matchstate->c = NFA_MATCH;
    matchstate->out = matchstate->out1 = রিক্ত;
    matchstate->id = 0;

    patch(e.out, matchstate);
    ret = e.start;

theend:
    ঘূৰাই ret;

#undef POP1
#undef PUSH1
#undef POP2
#undef PUSH2
#undef বেৰ_কৰো_2
#undef ঠেলো_2
}

/*
 * After building the NFA program, inspect it to add optimization hints.
 */
    স্থির শূন্য
nfa_postprocess(nfa_regprog_T *prog)
{
    পূর্ণাঙ্ক i;
    পূর্ণাঙ্ক c;

    বাবে (i = 0; i < prog->nstate; ++i)
    {
	c = prog->state[i].c;
	যদি (c == NFA_START_INVISIBLE
		|| c == NFA_START_INVISIBLE_NEG
		|| c == NFA_START_INVISIBLE_BEFORE
		|| c == NFA_START_INVISIBLE_BEFORE_NEG)
	{
	    পূর্ণাঙ্ক directly;

	    // Do it directly when what follows is possibly the end of the
	    // match.
	    যদি (match_follows(prog->state[i].out1->out, 0))
		directly = TRUE;
	    অন্যথা
	    {
		পূর্ণাঙ্ক ch_invisible = failure_chance(prog->state[i].out, 0);
		পূর্ণাঙ্ক ch_follows = failure_chance(prog->state[i].out1->out, 0);

		// Postpone when the invisible match is expensive or has a
		// lower chance of failing.
		যদি (c == NFA_START_INVISIBLE_BEFORE
		     || c == NFA_START_INVISIBLE_BEFORE_NEG)
		{
		    // "before" matches are very expensive when
		    // unbounded, always prefer what follows then,
		    // unless what follows will always match.
		    // Otherwise strongly prefer what follows.
		    যদি (prog->state[i].val <= 0 && ch_follows > 0)
			directly = FALSE;
		    অন্যথা
			directly = ch_follows * 10 < ch_invisible;
		}
		অন্যথা
		{
		    // normal invisible, first do the one with the
		    // highest failure chance
		    directly = ch_follows < ch_invisible;
		}
	    }
	    যদি (directly)
		// switch to the _FIRST state
		++prog->state[i].c;
	}
    }
}

/////////////////////////////////////////////////////////////////
// NFA execution code.
/////////////////////////////////////////////////////////////////

প্রকারনির্দেশ সংরচনা
{
    পূর্ণাঙ্ক	    in_use; // number of subexpr with useful info

    // When REG_MULTI is TRUE list.multi is used, otherwise list.line.
    সঙ্ঘ
    {
	সংরচনা multipos
	{
	    linenr_T	start_lnum;
	    linenr_T	end_lnum;
	    colnr_T	start_col;
	    colnr_T	end_col;
	} multi[NSUBEXP];
	সংরচনা linepos
	{
	    char_u	*start;
	    char_u	*end;
	} সাৰি[NSUBEXP];
    } list;
    colnr_T	orig_start_col;  // list.multi[0].start_col without \zs
} regsub_T;

প্রকারনির্দেশ সংরচনা
{
    regsub_T	norm; // \( .. \) matches
#ifdef FEAT_SYN_HL
    regsub_T	synt; // \z( .. \) matches
#endif
} regsubs_T;

// nfa_pim_T stores a Postponed Invisible Match.
প্রকারনির্দেশ সংরচনা nfa_pim_S nfa_pim_T;
সংরচনা nfa_pim_S
{
    পূর্ণাঙ্ক		result;		// NFA_PIM_*, see below
    nfa_state_T	*state;		// the invisible match start state
    regsubs_T	subs;		// submatch info, only party used
    সঙ্ঘ
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
প্রকারনির্দেশ সংরচনা
{
    nfa_state_T	*state;
    পূর্ণাঙ্ক		গুনতি;
    nfa_pim_T	pim;		// if pim.result != NFA_PIM_UNUSED: postponed
				// invisible match
    regsubs_T	subs;		// submatch info, only party used
} nfa_thread_T;

// nfa_list_T contains the alternative NFA execution states.
প্রকারনির্দেশ সংরচনা
{
    nfa_thread_T    *t;		// allocated array of states
    পূর্ণাঙ্ক		    n;		// nr of states currently in "t"
    পূর্ণাঙ্ক		    len;	// max nr of states in "t"
    পূর্ণাঙ্ক		    id;		// ID of the list
    পূর্ণাঙ্ক		    has_pim;	// TRUE when any state has a PIM
} nfa_list_T;

#ifdef ENABLE_LOG
স্থির শূন্য log_subexpr(regsub_T *sub);

    স্থির শূন্য
log_subsexpr(regsubs_T *subs)
{
    log_subexpr(&subs->norm);
# ifdef FEAT_SYN_HL
    যদি (rex.nfa_has_zsubexpr)
	log_subexpr(&subs->synt);
# endif
}

    স্থির শূন্য
log_subexpr(regsub_T *sub)
{
    পূর্ণাঙ্ক j;

    বাবে (j = 0; j < sub->in_use; j++)
	যদি (REG_MULTI)
	    fprintf(log_fd,
		    "*** group %d, start: c=%d, l=%d, end: c=%d, l=%d\n",
		    j,
		    sub->list.multi[j].start_col,
		    (পূর্ণাঙ্ক)sub->list.multi[j].start_lnum,
		    sub->list.multi[j].end_col,
		    (পূর্ণাঙ্ক)sub->list.multi[j].end_lnum);
	অন্যথা
	{
	    ৱর্ণ *s = (ৱর্ণ *)sub->list.সাৰি[j].start;
	    ৱর্ণ *e = (ৱর্ণ *)sub->list.সাৰি[j].end;

	    fprintf(log_fd, "*** group %d, start: \"%s\", end: \"%s\"\n",
		    j,
		    s == রিক্ত ? "NULL" : s,
		    e == রিক্ত ? "NULL" : e);
	}
}

    স্থির ৱর্ণ *
pim_info(nfa_pim_T *pim)
{
    স্থির ৱর্ণ buf[30];

    যদি (pim == রিক্ত || pim->result == NFA_PIM_UNUSED)
	buf[0] = NUL;
    অন্যথা
    {
	sprintf(buf, " PIM col %d", REG_MULTI ? (পূর্ণাঙ্ক)pim->end.pos.col
		: (পূর্ণাঙ্ক)(pim->end.ptr - rex.প্ৰবেশ));
    }
    ঘূৰাই buf;
}

#endif

// Used during execution: whether a match has been found.
স্থির পূর্ণাঙ্ক	    nfa_match;
#ifdef FEAT_RELTIME
স্থির পূর্ণাঙ্ক	   *nfa_timed_out;
#endif

স্থির অন্তঃস্থ শূন্য copy_sub(regsub_T *to, regsub_T *from);
স্থির পূর্ণাঙ্ক pim_equal(nfa_pim_T *one, nfa_pim_T *two);

/*
 * Copy postponed invisible match info from "from" to "to".
 */
    স্থির শূন্য
copy_pim(nfa_pim_T *to, nfa_pim_T *from)
{
    to->result = from->result;
    to->state = from->state;
    copy_sub(&to->subs.norm, &from->subs.norm);
#ifdef FEAT_SYN_HL
    যদি (rex.nfa_has_zsubexpr)
	copy_sub(&to->subs.synt, &from->subs.synt);
#endif
    to->end = from->end;
}

    স্থির শূন্য
clear_sub(regsub_T *sub)
{
    যদি (REG_MULTI)
	// Use 0xff to set lnum to -1
	vim_memset(sub->list.multi, 0xff,
				  পরিমাণ(সংরচনা multipos) * rex.nfa_nsubexpr);
    অন্যথা
	vim_memset(sub->list.সাৰি, 0,
				   পরিমাণ(সংরচনা linepos) * rex.nfa_nsubexpr);
    sub->in_use = 0;
}

/*
 * Copy the submatches from "from" to "to".
 */
    স্থির অন্তঃস্থ শূন্য
copy_sub(regsub_T *to, regsub_T *from)
{
    to->in_use = from->in_use;
    যদি (from->in_use <= 0)
	ঘূৰাই;

    // Copy the match start and end positions.  addstate() calls this for
    // every state it adds, so it is very hot.  Patterns without capturing
    // groups only use subexpression zero (the whole match), so special-case
    // "in_use == 1" with a direct assignment to avoid the mch_memmove() call.
    যদি (REG_MULTI)
    {
	যদি (from->in_use == 1)
	    to->list.multi[0] = from->list.multi[0];
	অন্যথা
	    mch_memmove(&to->list.multi[0],
		    &from->list.multi[0],
		    পরিমাণ(সংরচনা multipos) * from->in_use);
	to->orig_start_col = from->orig_start_col;
    }
    অন্যথা
    {
	যদি (from->in_use == 1)
	    to->list.সাৰি[0] = from->list.সাৰি[0];
	অন্যথা
	    mch_memmove(&to->list.সাৰি[0],
		    &from->list.সাৰি[0],
		    পরিমাণ(সংরচনা linepos) * from->in_use);
    }
}

/*
 * Like copy_sub() but exclude the main match.
 */
    স্থির শূন্য
copy_sub_off(regsub_T *to, regsub_T *from)
{
    যদি (to->in_use < from->in_use)
	to->in_use = from->in_use;
    যদি (from->in_use <= 1)
	ঘূৰাই;

    // Copy the match start and end positions.
    যদি (REG_MULTI)
	mch_memmove(&to->list.multi[1],
		&from->list.multi[1],
		পরিমাণ(সংরচনা multipos) * (from->in_use - 1));
    অন্যথা
	mch_memmove(&to->list.সাৰি[1],
		&from->list.সাৰি[1],
		পরিমাণ(সংরচনা linepos) * (from->in_use - 1));
}

/*
 * Like copy_sub() but only do the end of the main match if \ze is present.
 */
    স্থির শূন্য
copy_ze_off(regsub_T *to, regsub_T *from)
{
    যদি (!rex.nfa_has_zend)
	ঘূৰাই;

    যদি (REG_MULTI)
    {
	যদি (from->list.multi[0].end_lnum >= 0)
	{
	    to->list.multi[0].end_lnum = from->list.multi[0].end_lnum;
	    to->list.multi[0].end_col = from->list.multi[0].end_col;
	}
    }
    অন্যথা
    {
	যদি (from->list.সাৰি[0].end != রিক্ত)
	    to->list.সাৰি[0].end = from->list.সাৰি[0].end;
    }
}

/*
 * Return TRUE if "sub1" and "sub2" have the same start positions.
 * When using back-references also check the end position.
 */
    স্থির পূর্ণাঙ্ক
sub_equal(regsub_T *sub1, regsub_T *sub2)
{
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		todo;
    linenr_T	s1;
    linenr_T	s2;
    char_u	*sp1;
    char_u	*sp2;

    todo = sub1->in_use > sub2->in_use ? sub1->in_use : sub2->in_use;
    যদি (REG_MULTI)
    {
	বাবে (i = 0; i < todo; ++i)
	{
	    যদি (i < sub1->in_use)
		s1 = sub1->list.multi[i].start_lnum;
	    অন্যথা
		s1 = -1;
	    যদি (i < sub2->in_use)
		s2 = sub2->list.multi[i].start_lnum;
	    অন্যথা
		s2 = -1;
	    যদি (s1 != s2)
		ঘূৰাই FALSE;
	    যদি (s1 != -1 && sub1->list.multi[i].start_col
					     != sub2->list.multi[i].start_col)
		ঘূৰাই FALSE;

	    যদি (rex.nfa_has_backref)
	    {
		যদি (i < sub1->in_use)
		    s1 = sub1->list.multi[i].end_lnum;
		অন্যথা
		    s1 = -1;
		যদি (i < sub2->in_use)
		    s2 = sub2->list.multi[i].end_lnum;
		অন্যথা
		    s2 = -1;
		যদি (s1 != s2)
		    ঘূৰাই FALSE;
		যদি (s1 != -1 && sub1->list.multi[i].end_col
					       != sub2->list.multi[i].end_col)
		ঘূৰাই FALSE;
	    }
	}
    }
    অন্যথা
    {
	বাবে (i = 0; i < todo; ++i)
	{
	    যদি (i < sub1->in_use)
		sp1 = sub1->list.সাৰি[i].start;
	    অন্যথা
		sp1 = রিক্ত;
	    যদি (i < sub2->in_use)
		sp2 = sub2->list.সাৰি[i].start;
	    অন্যথা
		sp2 = রিক্ত;
	    যদি (sp1 != sp2)
		ঘূৰাই FALSE;
	    যদি (rex.nfa_has_backref)
	    {
		যদি (i < sub1->in_use)
		    sp1 = sub1->list.সাৰি[i].end;
		অন্যথা
		    sp1 = রিক্ত;
		যদি (i < sub2->in_use)
		    sp2 = sub2->list.সাৰি[i].end;
		অন্যথা
		    sp2 = রিক্ত;
		যদি (sp1 != sp2)
		    ঘূৰাই FALSE;
	    }
	}
    }

    ঘূৰাই TRUE;
}

#ifdef FEAT_RELTIME
/*
 * Check if we are past the time limit, if there is one.
 */
    স্থির পূর্ণাঙ্ক
nfa_did_time_out(শূন্য)
{
    যদি (*timeout_flag)
    {
	যদি (nfa_timed_out != রিক্ত)
	{
# ifdef FEAT_EVAL
	    যদি (!*nfa_timed_out)
		ch_log(রিক্ত, "NFA regexp timed out");
# endif
	    *nfa_timed_out = TRUE;
	}
	ঘূৰাই TRUE;
    }
    ঘূৰাই FALSE;
}
#endif

#ifdef ENABLE_LOG
    স্থির শূন্য
open_debug_log(পূর্ণাঙ্ক result)
{
    log_fd = fopen(NFA_REGEXP_RUN_LOG, "a");
    যদি (log_fd == রিক্ত)
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

    স্থির শূন্য
report_state(ৱর্ণ *action,
	     regsub_T *sub,
	     nfa_state_T *state,
	     পূর্ণাঙ্ক lid,
	     nfa_pim_T *pim)
{
    পূর্ণাঙ্ক col;

    যদি (sub->in_use <= 0)
	col = -1;
    অন্যথা যদি (REG_MULTI)
	col = sub->list.multi[0].start_col;
    অন্যথা
	col = (পূর্ণাঙ্ক)(sub->list.সাৰি[0].start - rex.সাৰি);
    nfa_set_code(state->c);
    যদি (log_fd == রিক্ত)
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
    স্থির পূর্ণাঙ্ক
has_state_with_pos(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim)	// postponed match or NULL
{
    nfa_thread_T	*thread;
    পূর্ণাঙ্ক			i;

    বাবে (i = 0; i < l->n; ++i)
    {
	thread = &l->t[i];
	যদি (thread->state->id == state->id
		&& sub_equal(&thread->subs.norm, &subs->norm)
#ifdef FEAT_SYN_HL
		&& (!rex.nfa_has_zsubexpr
				|| sub_equal(&thread->subs.synt, &subs->synt))
#endif
		&& pim_equal(&thread->pim, pim))
	    ঘূৰাই TRUE;
    }
    ঘূৰাই FALSE;
}

/*
 * Return TRUE if "one" and "two" are equal.  That includes when both are not
 * set.
 */
    স্থির পূর্ণাঙ্ক
pim_equal(nfa_pim_T *one, nfa_pim_T *two)
{
    পূর্ণাঙ্ক one_unused = (one == রিক্ত || one->result == NFA_PIM_UNUSED);
    পূর্ণাঙ্ক two_unused = (two == রিক্ত || two->result == NFA_PIM_UNUSED);

    যদি (one_unused)
	// one is unused: equal when two is also unused
	ঘূৰাই two_unused;
    যদি (two_unused)
	// one is used and two is not: not equal
	ঘূৰাই FALSE;
    // compare the state id
    যদি (one->state->id != two->state->id)
	ঘূৰাই FALSE;
    // compare the position
    যদি (REG_MULTI)
	ঘূৰাই one->end.pos.lnum == two->end.pos.lnum
	    && one->end.pos.col == two->end.pos.col;
    ঘূৰাই one->end.ptr == two->end.ptr;
}

/*
 * Return TRUE if "state" leads to a NFA_MATCH without advancing the input.
 */
    স্থির পূর্ণাঙ্ক
match_follows(nfa_state_T *startstate, পূর্ণাঙ্ক depth)
{
    nfa_state_T	    *state = startstate;

    // avoid too much recursion
    যদি (depth > 10)
	ঘূৰাই FALSE;

    যেতিয়ালৈ (state != রিক্ত)
    {
	ৱিকল্প (state->c)
	{
	    পক্ষ NFA_MATCH:
	    পক্ষ NFA_MCLOSE:
	    পক্ষ NFA_END_INVISIBLE:
	    পক্ষ NFA_END_INVISIBLE_NEG:
	    পক্ষ NFA_END_PATTERN:
		ঘূৰাই TRUE;

	    পক্ষ NFA_SPLIT:
		ঘূৰাই match_follows(state->out, depth + 1)
				     || match_follows(state->out1, depth + 1);

	    পক্ষ NFA_START_INVISIBLE:
	    পক্ষ NFA_START_INVISIBLE_FIRST:
	    পক্ষ NFA_START_INVISIBLE_BEFORE:
	    পক্ষ NFA_START_INVISIBLE_BEFORE_FIRST:
	    পক্ষ NFA_START_INVISIBLE_NEG:
	    পক্ষ NFA_START_INVISIBLE_NEG_FIRST:
	    পক্ষ NFA_START_INVISIBLE_BEFORE_NEG:
	    পক্ষ NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
	    পক্ষ NFA_COMPOSING:
		// skip ahead to next state
		state = state->out1->out;
		অনুৱর্ত;

	    পক্ষ NFA_ANY:
	    পক্ষ NFA_ANY_COMPOSING:
	    পক্ষ NFA_IDENT:
	    পক্ষ NFA_SIDENT:
	    পক্ষ NFA_KWORD:
	    পক্ষ NFA_SKWORD:
	    পক্ষ NFA_FNAME:
	    পক্ষ NFA_SFNAME:
	    পক্ষ NFA_PRINT:
	    পক্ষ NFA_SPRINT:
	    পক্ষ NFA_WHITE:
	    পক্ষ NFA_NWHITE:
	    পক্ষ NFA_DIGIT:
	    পক্ষ NFA_NDIGIT:
	    পক্ষ NFA_HEX:
	    পক্ষ NFA_NHEX:
	    পক্ষ NFA_OCTAL:
	    পক্ষ NFA_NOCTAL:
	    পক্ষ NFA_WORD:
	    পক্ষ NFA_NWORD:
	    পক্ষ NFA_HEAD:
	    পক্ষ NFA_NHEAD:
	    পক্ষ NFA_ALPHA:
	    পক্ষ NFA_NALPHA:
	    পক্ষ NFA_LOWER:
	    পক্ষ NFA_NLOWER:
	    পক্ষ NFA_UPPER:
	    পক্ষ NFA_NUPPER:
	    পক্ষ NFA_LOWER_IC:
	    পক্ষ NFA_NLOWER_IC:
	    পক্ষ NFA_UPPER_IC:
	    পক্ষ NFA_NUPPER_IC:
	    পক্ষ NFA_START_COLL:
	    পক্ষ NFA_START_NEG_COLL:
	    পক্ষ NFA_NEWL:
		// state will advance input
		ঘূৰাই FALSE;

	    স্ৱভাৱ:
		যদি (state->c > 0)
		    // state will advance input
		    ঘূৰাই FALSE;

		// Others: zero-width or possibly zero-width, might still find
		// a match at the same position, keep looking.
		ভঙ্গ;
	}
	state = state->out;
    }
    ঘূৰাই FALSE;
}


/*
 * Return TRUE if "state" is already in list "l".
 */
    স্থির পূর্ণাঙ্ক
state_in_list(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs)	// pointers to subexpressions
{
    যদি (state->lastlist[nfa_ll_index] == l->id)
    {
	যদি (!rex.nfa_has_backref || has_state_with_pos(l, state, subs, রিক্ত))
	    ঘূৰাই TRUE;
    }
    ঘূৰাই FALSE;
}

// Offset used for "off" by addstate_here().
#define ADDSTATE_HERE_OFFSET 10

/*
 * Add "state" and possibly what follows to state list ".".
 * Returns "subs_arg", possibly copied into temp_subs.
 * Returns NULL when recursiveness is too deep or timed out.
 */
    স্থির regsubs_T *
addstate(
    nfa_list_T		*l,	    // runtime state list
    nfa_state_T		*state,	    // state to update
    regsubs_T		*subs_arg,  // pointers to subexpressions
    nfa_pim_T		*pim,	    // postponed look-behind match
    পূর্ণাঙ্ক			off_arg,    // byte offset, when -1 go to next line
    পূর্ণাঙ্ক			depth)	    // recursion depth
{
    পূর্ণাঙ্ক			subidx;
    পূর্ণাঙ্ক			off = off_arg;
    পূর্ণাঙ্ক			add_here = FALSE;
    পূর্ণাঙ্ক			listindex = 0;
    পূর্ণাঙ্ক			k;
    পূর্ণাঙ্ক			found = FALSE;
    nfa_thread_T	*thread;
    সংরচনা multipos	save_multipos;
    পূর্ণাঙ্ক			save_in_use;
    char_u		*save_ptr;
    পূর্ণাঙ্ক			i;
    regsub_T		*sub;
    regsubs_T		*subs = subs_arg;
    স্থির regsubs_T	temp_subs;
#ifdef ENABLE_LOG
    পূর্ণাঙ্ক			did_print = FALSE;
#endif

#ifdef FEAT_RELTIME
    যদি (nfa_did_time_out())
	ঘূৰাই রিক্ত;
#endif

    // This function is called recursively.  When the depth is too much we run
    // out of stack and crash, limit recursiveness here.
    যদি (depth >= 5000 || subs == রিক্ত)
	ঘূৰাই রিক্ত;

    যদি (off_arg <= -ADDSTATE_HERE_OFFSET)
    {
	add_here = TRUE;
	off = 0;
	listindex = -(off_arg + ADDSTATE_HERE_OFFSET);
    }

    ৱিকল্প (state->c)
    {
	পক্ষ NFA_NCLOSE:
	পক্ষ NFA_MCLOSE:
	পক্ষ NFA_MCLOSE1:
	পক্ষ NFA_MCLOSE2:
	পক্ষ NFA_MCLOSE3:
	পক্ষ NFA_MCLOSE4:
	পক্ষ NFA_MCLOSE5:
	পক্ষ NFA_MCLOSE6:
	পক্ষ NFA_MCLOSE7:
	পক্ষ NFA_MCLOSE8:
	পক্ষ NFA_MCLOSE9:
#ifdef FEAT_SYN_HL
	পক্ষ NFA_ZCLOSE:
	পক্ষ NFA_ZCLOSE1:
	পক্ষ NFA_ZCLOSE2:
	পক্ষ NFA_ZCLOSE3:
	পক্ষ NFA_ZCLOSE4:
	পক্ষ NFA_ZCLOSE5:
	পক্ষ NFA_ZCLOSE6:
	পক্ষ NFA_ZCLOSE7:
	পক্ষ NFA_ZCLOSE8:
	পক্ষ NFA_ZCLOSE9:
#endif
	পক্ষ NFA_MOPEN:
	পক্ষ NFA_ZEND:
	পক্ষ NFA_SPLIT:
	পক্ষ NFA_EMPTY:
	    // These nodes are not added themselves but their "out" and/or
	    // "out1" may be added below.
	    ভঙ্গ;

	পক্ষ NFA_BOL:
	পক্ষ NFA_BOF:
	    // "^" won't match past end-of-line, don't bother trying.
	    // Except when at the end of the line, or when we are going to the
	    // next line for a look-behind match.
	    যদি (rex.প্ৰবেশ > rex.সাৰি
		    && *rex.প্ৰবেশ != NUL
		    && (nfa_endp == রিক্ত
			|| !REG_MULTI
			|| rex.lnum == nfa_endp->se_u.pos.lnum))
		গচ্ছ skip_add;
	    // FALLTHROUGH

	পক্ষ NFA_MOPEN1:
	পক্ষ NFA_MOPEN2:
	পক্ষ NFA_MOPEN3:
	পক্ষ NFA_MOPEN4:
	পক্ষ NFA_MOPEN5:
	পক্ষ NFA_MOPEN6:
	পক্ষ NFA_MOPEN7:
	পক্ষ NFA_MOPEN8:
	পক্ষ NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	পক্ষ NFA_ZOPEN:
	পক্ষ NFA_ZOPEN1:
	পক্ষ NFA_ZOPEN2:
	পক্ষ NFA_ZOPEN3:
	পক্ষ NFA_ZOPEN4:
	পক্ষ NFA_ZOPEN5:
	পক্ষ NFA_ZOPEN6:
	পক্ষ NFA_ZOPEN7:
	পক্ষ NFA_ZOPEN8:
	পক্ষ NFA_ZOPEN9:
#endif
	পক্ষ NFA_NOPEN:
	পক্ষ NFA_ZSTART:
	    // These nodes need to be added so that we can bail out when it
	    // was added to this list before at the same position to avoid an
	    // endless loop for "\(\)*"

	স্ৱভাৱ:
	    যদি (state->lastlist[nfa_ll_index] == l->id && state->c != NFA_SKIP)
	    {
		// This state is already in the list, don't add it again,
		// unless it is an MOPEN that is used for a backreference or
		// when there is a PIM. For NFA_MATCH check the position,
		// lower position is preferred.
		যদি (!rex.nfa_has_backref && pim == রিক্ত && !l->has_pim
						     && state->c != NFA_MATCH)
		{
		    // When called from addstate_here() do insert before
		    // existing states.
		    যদি (add_here)
		    {
			বাবে (k = 0; k < l->n && k < listindex; ++k)
			    যদি (l->t[k].state->id == state->id)
			    {
				found = TRUE;
				ভঙ্গ;
			    }
		    }
		    যদি (!add_here || found)
		    {
skip_add:
#ifdef ENABLE_LOG
			nfa_set_code(state->c);
			fprintf(log_fd, "> Not adding state %d to list %d. char %d: %s pim: %s has_pim: %d found: %d\n",
			    abs(state->id), l->id, state->c, code,
			    pim == রিক্ত ? "NULL" : "yes", l->has_pim, found);
#endif
			ঘূৰাই subs;
		    }
		}

		// Do not add the state again when it exists with the same
		// positions.
		যদি (has_state_with_pos(l, state, subs, pim))
		    গচ্ছ skip_add;
	    }

	    // When there are backreferences or PIMs the number of states may
	    // be (a lot) bigger than anticipated.
	    যদি (l->n == l->len)
	    {
		পূর্ণাঙ্ক		newlen = l->len * 3 / 2 + 50;
		size_t		newsize = newlen * পরিমাণ(nfa_thread_T);
		nfa_thread_T	*newt;

		যদি ((long)(newsize >> 10) >= p_mmp)
		{
		    emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		    ঘূৰাই রিক্ত;
		}
		যদি (subs != &temp_subs)
		{
		    // "subs" may point into the current array, need to make a
		    // copy before it becomes invalid.
		    copy_sub(&temp_subs.norm, &subs->norm);
#ifdef FEAT_SYN_HL
		    যদি (rex.nfa_has_zsubexpr)
			copy_sub(&temp_subs.synt, &subs->synt);
#endif
		    subs = &temp_subs;
		}

		newt = vim_realloc(l->t, newsize);
		যদি (newt == রিক্ত)
		{
		    // out of memory
		    ঘূৰাই রিক্ত;
		}
		l->t = newt;
		l->len = newlen;
	    }

	    // add the state to the list
	    state->lastlist[nfa_ll_index] = l->id;
	    thread = &l->t[l->n++];
	    thread->state = state;
	    যদি (pim == রিক্ত)
		thread->pim.result = NFA_PIM_UNUSED;
	    অন্যথা
	    {
		copy_pim(&thread->pim, pim);
		l->has_pim = TRUE;
	    }
	    copy_sub(&thread->subs.norm, &subs->norm);
#ifdef FEAT_SYN_HL
	    যদি (rex.nfa_has_zsubexpr)
		copy_sub(&thread->subs.synt, &subs->synt);
#endif
#ifdef ENABLE_LOG
	    report_state("Adding", &thread->subs.norm, state, l->id, pim);
	    did_print = TRUE;
#endif
    }

#ifdef ENABLE_LOG
    যদি (!did_print)
	report_state("Processing", &subs->norm, state, l->id, pim);
#endif
    ৱিকল্প (state->c)
    {
	পক্ষ NFA_MATCH:
	    ভঙ্গ;

	পক্ষ NFA_SPLIT:
	    // order matters here
	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    subs = addstate(l, state->out1, subs, pim, off_arg, depth + 1);
	    ভঙ্গ;

	পক্ষ NFA_EMPTY:
	পক্ষ NFA_NOPEN:
	পক্ষ NFA_NCLOSE:
	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    ভঙ্গ;

	পক্ষ NFA_MOPEN:
	পক্ষ NFA_MOPEN1:
	পক্ষ NFA_MOPEN2:
	পক্ষ NFA_MOPEN3:
	পক্ষ NFA_MOPEN4:
	পক্ষ NFA_MOPEN5:
	পক্ষ NFA_MOPEN6:
	পক্ষ NFA_MOPEN7:
	পক্ষ NFA_MOPEN8:
	পক্ষ NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	পক্ষ NFA_ZOPEN:
	পক্ষ NFA_ZOPEN1:
	পক্ষ NFA_ZOPEN2:
	পক্ষ NFA_ZOPEN3:
	পক্ষ NFA_ZOPEN4:
	পক্ষ NFA_ZOPEN5:
	পক্ষ NFA_ZOPEN6:
	পক্ষ NFA_ZOPEN7:
	পক্ষ NFA_ZOPEN8:
	পক্ষ NFA_ZOPEN9:
#endif
	পক্ষ NFA_ZSTART:
	    যদি (state->c == NFA_ZSTART)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#ifdef FEAT_SYN_HL
	    অন্যথা যদি (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
	    {
		subidx = state->c - NFA_ZOPEN;
		sub = &subs->synt;
	    }
#endif
	    অন্যথা
	    {
		subidx = state->c - NFA_MOPEN;
		sub = &subs->norm;
	    }

	    // avoid compiler warnings
	    save_ptr = রিক্ত;
	    CLEAR_FIELD(save_multipos);

	    // Set the position (with "off" added) in the subexpression.  Save
	    // and restore it when it was in use.  Otherwise fill any gap.
	    যদি (REG_MULTI)
	    {
		যদি (subidx < sub->in_use)
		{
		    save_multipos = sub->list.multi[subidx];
		    save_in_use = -1;
		}
		অন্যথা
		{
		    save_in_use = sub->in_use;
		    বাবে (i = sub->in_use; i < subidx; ++i)
		    {
			sub->list.multi[i].start_lnum = -1;
			sub->list.multi[i].end_lnum = -1;
		    }
		    sub->in_use = subidx + 1;
		}
		যদি (off == -1)
		{
		    sub->list.multi[subidx].start_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].start_col = 0;
		}
		অন্যথা
		{
		    sub->list.multi[subidx].start_lnum = rex.lnum;
		    sub->list.multi[subidx].start_col =
					 (colnr_T)(rex.প্ৰবেশ - rex.সাৰি + off);
		}
		sub->list.multi[subidx].end_lnum = -1;
	    }
	    অন্যথা
	    {
		যদি (subidx < sub->in_use)
		{
		    save_ptr = sub->list.সাৰি[subidx].start;
		    save_in_use = -1;
		}
		অন্যথা
		{
		    save_in_use = sub->in_use;
		    বাবে (i = sub->in_use; i < subidx; ++i)
		    {
			sub->list.সাৰি[i].start = রিক্ত;
			sub->list.সাৰি[i].end = রিক্ত;
		    }
		    sub->in_use = subidx + 1;
		}
		sub->list.সাৰি[subidx].start = rex.প্ৰবেশ + off;
	    }

	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    যদি (subs == রিক্ত)
		ভঙ্গ;
	    // "subs" may have changed, need to set "sub" again
#ifdef FEAT_SYN_HL
	    যদি (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
		sub = &subs->synt;
	    অন্যথা
#endif
		sub = &subs->norm;

	    যদি (save_in_use == -1)
	    {
		যদি (REG_MULTI)
		    sub->list.multi[subidx] = save_multipos;
		অন্যথা
		    sub->list.সাৰি[subidx].start = save_ptr;
	    }
	    অন্যথা
		sub->in_use = save_in_use;
	    ভঙ্গ;

	পক্ষ NFA_MCLOSE:
	    যদি (rex.nfa_has_zend && (REG_MULTI
			? subs->norm.list.multi[0].end_lnum >= 0
			: subs->norm.list.সাৰি[0].end != রিক্ত))
	    {
		// Do not overwrite the position set by \ze.
		subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
		ভঙ্গ;
	    }
	    // FALLTHROUGH
	পক্ষ NFA_MCLOSE1:
	পক্ষ NFA_MCLOSE2:
	পক্ষ NFA_MCLOSE3:
	পক্ষ NFA_MCLOSE4:
	পক্ষ NFA_MCLOSE5:
	পক্ষ NFA_MCLOSE6:
	পক্ষ NFA_MCLOSE7:
	পক্ষ NFA_MCLOSE8:
	পক্ষ NFA_MCLOSE9:
#ifdef FEAT_SYN_HL
	পক্ষ NFA_ZCLOSE:
	পক্ষ NFA_ZCLOSE1:
	পক্ষ NFA_ZCLOSE2:
	পক্ষ NFA_ZCLOSE3:
	পক্ষ NFA_ZCLOSE4:
	পক্ষ NFA_ZCLOSE5:
	পক্ষ NFA_ZCLOSE6:
	পক্ষ NFA_ZCLOSE7:
	পক্ষ NFA_ZCLOSE8:
	পক্ষ NFA_ZCLOSE9:
#endif
	পক্ষ NFA_ZEND:
	    যদি (state->c == NFA_ZEND)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#ifdef FEAT_SYN_HL
	    অন্যথা যদি (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
	    {
		subidx = state->c - NFA_ZCLOSE;
		sub = &subs->synt;
	    }
#endif
	    অন্যথা
	    {
		subidx = state->c - NFA_MCLOSE;
		sub = &subs->norm;
	    }

	    // We don't fill in gaps here, there must have been an MOPEN that
	    // has done that.
	    save_in_use = sub->in_use;
	    যদি (sub->in_use <= subidx)
		sub->in_use = subidx + 1;
	    যদি (REG_MULTI)
	    {
		save_multipos = sub->list.multi[subidx];
		যদি (off == -1)
		{
		    sub->list.multi[subidx].end_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].end_col = 0;
		}
		অন্যথা
		{
		    sub->list.multi[subidx].end_lnum = rex.lnum;
		    sub->list.multi[subidx].end_col =
					  (colnr_T)(rex.প্ৰবেশ - rex.সাৰি + off);
		}
		// avoid compiler warnings
		save_ptr = রিক্ত;
	    }
	    অন্যথা
	    {
		save_ptr = sub->list.সাৰি[subidx].end;
		sub->list.সাৰি[subidx].end = rex.প্ৰবেশ + off;
		// avoid compiler warnings
		CLEAR_FIELD(save_multipos);
	    }

	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    যদি (subs == রিক্ত)
		ভঙ্গ;
	    // "subs" may have changed, need to set "sub" again
#ifdef FEAT_SYN_HL
	    যদি (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
		sub = &subs->synt;
	    অন্যথা
#endif
		sub = &subs->norm;

	    যদি (REG_MULTI)
		sub->list.multi[subidx] = save_multipos;
	    অন্যথা
		sub->list.সাৰি[subidx].end = save_ptr;
	    sub->in_use = save_in_use;
	    ভঙ্গ;
    }
    ঘূৰাই subs;
}

/*
 * Like addstate(), but the new state(s) are put at position "*ip".
 * Used for zero-width matches, next state to use is the added one.
 * This makes sure the order of states to be tried does not change, which
 * matters for alternatives.
 */
    স্থির regsubs_T *
addstate_here(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim,   // postponed look-behind match
    পূর্ণাঙ্ক			*ip)
{
    পূর্ণাঙ্ক tlen = l->n;
    পূর্ণাঙ্ক গুনতি;
    পূর্ণাঙ্ক listidx = *ip;
    regsubs_T *r;

    // First add the state(s) at the end, so that we know how many there are.
    // Pass the listidx as offset (avoids adding another argument to
    // addstate()).
    r = addstate(l, state, subs, pim, -listidx - ADDSTATE_HERE_OFFSET, 0);
    যদি (r == রিক্ত)
	ঘূৰাই রিক্ত;

    // when "*ip" was at the end of the list, nothing to do
    যদি (listidx + 1 == tlen)
	ঘূৰাই r;

    // re-order to put the new state at the current position
    গুনতি = l->n - tlen;
    যদি (গুনতি == 0)
	ঘূৰাই r; // no state got added
    যদি (গুনতি == 1)
    {
	// overwrite the current state
	l->t[listidx] = l->t[l->n - 1];
    }
    অন্যথা যদি (গুনতি > 1)
    {
	যদি (l->n + গুনতি - 1 >= l->len)
	{
	    // not enough space to move the new states, reallocate the list
	    // and move the states to the right position
	    পূর্ণাঙ্ক		    newlen = l->len * 3 / 2 + 50;
	    size_t	    newsize = newlen * পরিমাণ(nfa_thread_T);
	    nfa_thread_T    *newl;

	    যদি ((long)(newsize >> 10) >= p_mmp)
	    {
		emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		ঘূৰাই রিক্ত;
	    }
	    newl = alloc(newsize);
	    যদি (newl == রিক্ত)
		ঘূৰাই রিক্ত;
	    l->len = newlen;
	    mch_memmove(&(newl[0]),
		    &(l->t[0]),
		    পরিমাণ(nfa_thread_T) * listidx);
	    mch_memmove(&(newl[listidx]),
		    &(l->t[l->n - গুনতি]),
		    পরিমাণ(nfa_thread_T) * গুনতি);
	    mch_memmove(&(newl[listidx + গুনতি]),
		    &(l->t[listidx + 1]),
		    পরিমাণ(nfa_thread_T) * (l->n - গুনতি - listidx - 1));
	    vim_free(l->t);
	    l->t = newl;
	}
	অন্যথা
	{
	    // make space for new states, then move them from the
	    // end to the current position
	    mch_memmove(&(l->t[listidx + গুনতি]),
		    &(l->t[listidx + 1]),
		    পরিমাণ(nfa_thread_T) * (l->n - listidx - 1));
	    mch_memmove(&(l->t[listidx]),
		    &(l->t[l->n - 1]),
		    পরিমাণ(nfa_thread_T) * গুনতি);
	}
    }
    --l->n;
    *ip = listidx - 1;

    ঘূৰাই r;
}

/*
 * Check character class "class" against current character c.
 */
    স্থির পূর্ণাঙ্ক
check_char_class(পূর্ণাঙ্ক class, পূর্ণাঙ্ক c)
{
    ৱিকল্প (class)
    {
	পক্ষ NFA_CLASS_ALNUM:
	    যদি (c >= 1 && c < 128 && isalnum(c))
		ঘূৰাই OK;
	    ভঙ্গ;
	পক্ষ NFA_CLASS_ALPHA:
	    যদি (c >= 1 && c < 128 && isalpha(c))
		ঘূৰাই OK;
	    ভঙ্গ;
	পক্ষ NFA_CLASS_BLANK:
	    যদি (c == ' ' || c == '\t')
		ঘূৰাই OK;
	    ভঙ্গ;
	পক্ষ NFA_CLASS_CNTRL:
	    যদি (c >= 1 && c <= 127 && iscntrl(c))
		ঘূৰাই OK;
	    ভঙ্গ;
	পক্ষ NFA_CLASS_DIGIT:
	    যদি (VIM_ISDIGIT(c))
		ঘূৰাই OK;
	    ভঙ্গ;
	পক্ষ NFA_CLASS_GRAPH:
	    যদি (c >= 1 && c <= 127 && isgraph(c))
		ঘূৰাই OK;
	    ভঙ্গ;
	পক্ষ NFA_CLASS_LOWER:
	    যদি (MB_ISLOWER(c) && c != 170 && c != 186)
		ঘূৰাই OK;
	    ভঙ্গ;
	পক্ষ NFA_CLASS_PRINT:
	    যদি (vim_isprintc(c))
		ঘূৰাই OK;
	    ভঙ্গ;
	পক্ষ NFA_CLASS_PUNCT:
	    যদি (c >= 1 && c < 128 && ispunct(c))
		ঘূৰাই OK;
	    ভঙ্গ;
	পক্ষ NFA_CLASS_SPACE:
	    যদি ((c >= 9 && c <= 13) || (c == ' '))
		ঘূৰাই OK;
	    ভঙ্গ;
	পক্ষ NFA_CLASS_UPPER:
	    যদি (MB_ISUPPER(c))
		ঘূৰাই OK;
	    ভঙ্গ;
	পক্ষ NFA_CLASS_XDIGIT:
	    যদি (vim_isxdigit(c))
		ঘূৰাই OK;
	    ভঙ্গ;
	পক্ষ NFA_CLASS_TAB:
	    যদি (c == '\t')
		ঘূৰাই OK;
	    ভঙ্গ;
	পক্ষ NFA_CLASS_RETURN:
	    যদি (c == '\r')
		ঘূৰাই OK;
	    ভঙ্গ;
	পক্ষ NFA_CLASS_BACKSPACE:
	    যদি (c == '\b')
		ঘূৰাই OK;
	    ভঙ্গ;
	পক্ষ NFA_CLASS_ESCAPE:
	    যদি (c == '\033')
		ঘূৰাই OK;
	    ভঙ্গ;
	পক্ষ NFA_CLASS_IDENT:
	    যদি (vim_isIDc(c))
		ঘূৰাই OK;
	    ভঙ্গ;
	পক্ষ NFA_CLASS_KEYWORD:
	    যদি (reg_iswordc(c))
		ঘূৰাই OK;
	    ভঙ্গ;
	পক্ষ NFA_CLASS_FNAME:
	    যদি (vim_isfilec(c))
		ঘূৰাই OK;
	    ভঙ্গ;

	স্ৱভাৱ:
	    // should not be here :P
	    siemsg(_(e_nfa_regexp_invalid_character_class_nr), class);
	    ঘূৰাই FAIL;
    }
    ঘূৰাই FAIL;
}

/*
 * Check for a match with subexpression "subidx".
 * Return TRUE if it matches.
 */
    স্থির পূর্ণাঙ্ক
match_backref(
    regsub_T	*sub,	    // pointers to subexpressions
    পূর্ণাঙ্ক		subidx,
    পূর্ণাঙ্ক		*bytelen)   // out: length of match in bytes
{
    পূর্ণাঙ্ক		len;

    যদি (sub->in_use <= subidx)
    {
retempty:
	// backref was not set, match an empty string
	*bytelen = 0;
	ঘূৰাই TRUE;
    }

    যদি (REG_MULTI)
    {
	যদি (sub->list.multi[subidx].start_lnum < 0
				       || sub->list.multi[subidx].end_lnum < 0)
	    গচ্ছ retempty;
	যদি (sub->list.multi[subidx].start_lnum == rex.lnum
			       && sub->list.multi[subidx].end_lnum == rex.lnum)
	{
	    len = sub->list.multi[subidx].end_col
					  - sub->list.multi[subidx].start_col;
	    যদি (cstrncmp(rex.সাৰি + sub->list.multi[subidx].start_col,
							 rex.প্ৰবেশ, &len) == 0)
	    {
		*bytelen = len;
		ঘূৰাই TRUE;
	    }
	}
	অন্যথা
	{
	    যদি (match_with_backref(
			sub->list.multi[subidx].start_lnum,
			sub->list.multi[subidx].start_col,
			sub->list.multi[subidx].end_lnum,
			sub->list.multi[subidx].end_col,
			bytelen) == RA_MATCH)
		ঘূৰাই TRUE;
	}
    }
    অন্যথা
    {
	যদি (sub->list.সাৰি[subidx].start == রিক্ত
					|| sub->list.সাৰি[subidx].end == রিক্ত)
	    গচ্ছ retempty;
	len = (পূর্ণাঙ্ক)(sub->list.সাৰি[subidx].end - sub->list.সাৰি[subidx].start);
	যদি (cstrncmp(sub->list.সাৰি[subidx].start, rex.প্ৰবেশ, &len) == 0)
	{
	    *bytelen = len;
	    ঘূৰাই TRUE;
	}
    }
    ঘূৰাই FALSE;
}

#ifdef FEAT_SYN_HL

/*
 * Check for a match with \z subexpression "subidx".
 * Return TRUE if it matches.
 */
    স্থির পূর্ণাঙ্ক
match_zref(
    পূর্ণাঙ্ক		subidx,
    পূর্ণাঙ্ক		*bytelen)   // out: length of match in bytes
{
    পূর্ণাঙ্ক		len;

    cleanup_zsubexpr();
    যদি (re_extmatch_in == রিক্ত || re_extmatch_in->matches[subidx] == রিক্ত)
    {
	// backref was not set, match an empty string
	*bytelen = 0;
	ঘূৰাই TRUE;
    }

    len = (পূর্ণাঙ্ক)STRLEN(re_extmatch_in->matches[subidx]);
    যদি (cstrncmp(re_extmatch_in->matches[subidx], rex.প্ৰবেশ, &len) == 0)
    {
	*bytelen = len;
	ঘূৰাই TRUE;
    }
    ঘূৰাই FALSE;
}
#endif

/*
 * Save list IDs for all NFA states of "prog" into "list".
 * Also reset the IDs to zero.
 * Only used for the recursive value lastlist[1].
 */
    স্থির শূন্য
nfa_save_listids(nfa_regprog_T *prog, পূর্ণাঙ্ক *list)
{
    পূর্ণাঙ্ক		    i;
    nfa_state_T	    *p;

    // Order in the list is reverse, it's a bit faster that way.
    p = &prog->state[0];
    বাবে (i = prog->nstate; --i >= 0; )
    {
	list[i] = p->lastlist[1];
	p->lastlist[1] = 0;
	++p;
    }
}

/*
 * Restore list IDs from "list" to all NFA states.
 */
    স্থির শূন্য
nfa_restore_listids(nfa_regprog_T *prog, পূর্ণাঙ্ক *list)
{
    পূর্ণাঙ্ক		    i;
    nfa_state_T	    *p;

    p = &prog->state[0];
    বাবে (i = prog->nstate; --i >= 0; )
    {
	p->lastlist[1] = list[i];
	++p;
    }
}

    স্থির পূর্ণাঙ্ক
nfa_re_অঙ্ক_cmp(long_u val, পূর্ণাঙ্ক op, long_u pos)
{
    যদি (op == 1) ঘূৰাই pos > val;
    যদি (op == 2) ঘূৰাই pos < val;
    ঘূৰাই val == pos;
}

স্থির পূর্ণাঙ্ক nfa_regmatch(nfa_regprog_T *prog, nfa_state_T *start, regsubs_T *submatch, regsubs_T *m);

/*
 * Recursively call nfa_regmatch()
 * "pim" is NULL or contains info about a Postponed Invisible Match (start
 * position).
 */
    স্থির পূর্ণাঙ্ক
recursive_regmatch(
    nfa_state_T	    *state,
    nfa_pim_T	    *pim,
    nfa_regprog_T   *prog,
    regsubs_T	    *submatch,
    regsubs_T	    *m,
    পূর্ণাঙ্ক		    **listids,
    পূর্ণাঙ্ক		    *listids_len)
{
    পূর্ণাঙ্ক		save_reginput_col = (পূর্ণাঙ্ক)(rex.প্ৰবেশ - rex.সাৰি);
    পূর্ণাঙ্ক		save_reglnum = rex.lnum;
    পূর্ণাঙ্ক		save_nfa_match = nfa_match;
    পূর্ণাঙ্ক		save_nfa_listid = rex.nfa_listid;
    save_se_T   *save_nfa_endp = nfa_endp;
    save_se_T   endpos;
    save_se_T   *endposp = রিক্ত;
    পূর্ণাঙ্ক		result;
    পূর্ণাঙ্ক		need_restore = FALSE;

    যদি (pim != রিক্ত)
    {
	// start at the position where the postponed match was
	যদি (REG_MULTI)
	    rex.প্ৰবেশ = rex.সাৰি + pim->end.pos.col;
	অন্যথা
	    rex.প্ৰবেশ = pim->end.ptr;
    }

    যদি (state->c == NFA_START_INVISIBLE_BEFORE
	    || state->c == NFA_START_INVISIBLE_BEFORE_FIRST
	    || state->c == NFA_START_INVISIBLE_BEFORE_NEG
	    || state->c == NFA_START_INVISIBLE_BEFORE_NEG_FIRST)
    {
	// The recursive match must end at the current position. When "pim" is
	// not NULL it specifies the current position.
	endposp = &endpos;
	যদি (REG_MULTI)
	{
	    যদি (pim == রিক্ত)
	    {
		endpos.se_u.pos.col = (পূর্ণাঙ্ক)(rex.প্ৰবেশ - rex.সাৰি);
		endpos.se_u.pos.lnum = rex.lnum;
	    }
	    অন্যথা
		endpos.se_u.pos = pim->end.pos;
	}
	অন্যথা
	{
	    যদি (pim == রিক্ত)
		endpos.se_u.ptr = rex.প্ৰবেশ;
	    অন্যথা
		endpos.se_u.ptr = pim->end.ptr;
	}

	// Go back the specified number of bytes, or as far as the
	// start of the previous line, to try matching "\@<=" or
	// not matching "\@<!". This is very inefficient, limit the number of
	// bytes if possible.
	যদি (state->val <= 0)
	{
	    যদি (REG_MULTI)
	    {
		rex.সাৰি = reg_getline(--rex.lnum);
		যদি (rex.সাৰি == রিক্ত)
		    // can't go before the first line
		    rex.সাৰি = reg_getline(++rex.lnum);
	    }
	    rex.প্ৰবেশ = rex.সাৰি;
	}
	অন্যথা
	{
	    যদি (REG_MULTI && (পূর্ণাঙ্ক)(rex.প্ৰবেশ - rex.সাৰি) < state->val)
	    {
		// Not enough bytes in this line, go to end of
		// previous line.
		rex.সাৰি = reg_getline(--rex.lnum);
		যদি (rex.সাৰি == রিক্ত)
		{
		    // can't go before the first line
		    rex.সাৰি = reg_getline(++rex.lnum);
		    rex.প্ৰবেশ = rex.সাৰি;
		}
		অন্যথা
		    rex.প্ৰবেশ = rex.সাৰি + reg_getline_len(rex.lnum);
	    }
	    যদি ((পূর্ণাঙ্ক)(rex.প্ৰবেশ - rex.সাৰি) >= state->val)
	    {
		rex.প্ৰবেশ -= state->val;
		যদি (has_mbyte)
		    rex.প্ৰবেশ -= mb_head_off(rex.সাৰি, rex.প্ৰবেশ);
	    }
	    অন্যথা
		rex.প্ৰবেশ = rex.সাৰি;
	}
    }

#ifdef ENABLE_LOG
    যদি (log_fd != stderr)
	fclose(log_fd);
    log_fd = রিক্ত;
#endif
    // Have to clear the lastlist field of the NFA nodes, so that
    // nfa_regmatch() and addstate() can run properly after recursion.
    যদি (nfa_ll_index == 1)
    {
	// Already calling nfa_regmatch() recursively.  Save the lastlist[1]
	// values and clear them.
	যদি (*listids == রিক্ত || *listids_len < prog->nstate)
	{
	    vim_free(*listids);
	    *listids = ALLOC_MULT(পূর্ণাঙ্ক, prog->nstate);
	    যদি (*listids == রিক্ত)
	    {
		emsg(_(e_nfa_regexp_could_not_allocate_memory_for_branch_traversal));
		ঘূৰাই 0;
	    }
	    *listids_len = prog->nstate;
	}
	nfa_save_listids(prog, *listids);
	need_restore = TRUE;
	// any value of rex.nfa_listid will do
    }
    অন্যথা
    {
	// First recursive nfa_regmatch() call, switch to the second lastlist
	// entry.  Make sure rex.nfa_listid is different from a previous
	// recursive call, because some states may still have this ID.
	++nfa_ll_index;
	যদি (rex.nfa_listid <= rex.nfa_alt_listid)
	    rex.nfa_listid = rex.nfa_alt_listid;
    }

    // Call nfa_regmatch() to check if the current concat matches at this
    // position. The concat ends with the node NFA_END_INVISIBLE
    nfa_endp = endposp;
    result = nfa_regmatch(prog, state->out, submatch, m);

    যদি (need_restore)
	nfa_restore_listids(prog, *listids);
    অন্যথা
    {
	--nfa_ll_index;
	rex.nfa_alt_listid = rex.nfa_listid;
    }

    // restore position in input text
    rex.lnum = save_reglnum;
    যদি (REG_MULTI)
	rex.সাৰি = reg_getline(rex.lnum);
    rex.প্ৰবেশ = rex.সাৰি + save_reginput_col;
    যদি (result != NFA_TOO_EXPENSIVE)
    {
	nfa_match = save_nfa_match;
	rex.nfa_listid = save_nfa_listid;
    }
    nfa_endp = save_nfa_endp;

#ifdef ENABLE_LOG
    open_debug_log(result);
#endif

    ঘূৰাই result;
}

/*
 * Estimate the chance of a match with "state" failing.
 * empty match: 0
 * NFA_ANY: 1
 * specific character: 99
 */
    স্থির পূর্ণাঙ্ক
failure_chance(nfa_state_T *state, পূর্ণাঙ্ক depth)
{
    পূর্ণাঙ্ক c = state->c;
    পূর্ণাঙ্ক l, r;

    // detect looping
    যদি (depth > 4)
	ঘূৰাই 1;

    ৱিকল্প (c)
    {
	পক্ষ NFA_SPLIT:
	    যদি (state->out->c == NFA_SPLIT || state->out1->c == NFA_SPLIT)
		// avoid recursive stuff
		ঘূৰাই 1;
	    // two alternatives, use the lowest failure chance
	    l = failure_chance(state->out, depth + 1);
	    r = failure_chance(state->out1, depth + 1);
	    ঘূৰাই l < r ? l : r;

	পক্ষ NFA_ANY:
	    // matches anything, unlikely to fail
	    ঘূৰাই 1;

	পক্ষ NFA_MATCH:
	পক্ষ NFA_MCLOSE:
	পক্ষ NFA_ANY_COMPOSING:
	    // empty match works always
	    ঘূৰাই 0;

	পক্ষ NFA_START_INVISIBLE:
	পক্ষ NFA_START_INVISIBLE_FIRST:
	পক্ষ NFA_START_INVISIBLE_NEG:
	পক্ষ NFA_START_INVISIBLE_NEG_FIRST:
	পক্ষ NFA_START_INVISIBLE_BEFORE:
	পক্ষ NFA_START_INVISIBLE_BEFORE_FIRST:
	পক্ষ NFA_START_INVISIBLE_BEFORE_NEG:
	পক্ষ NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
	পক্ষ NFA_START_PATTERN:
	    // recursive regmatch is expensive, use low failure chance
	    ঘূৰাই 5;

	পক্ষ NFA_BOL:
	পক্ষ NFA_EOL:
	পক্ষ NFA_BOF:
	পক্ষ NFA_EOF:
	পক্ষ NFA_NEWL:
	    ঘূৰাই 99;

	পক্ষ NFA_BOW:
	পক্ষ NFA_EOW:
	    ঘূৰাই 90;

	পক্ষ NFA_MOPEN:
	পক্ষ NFA_MOPEN1:
	পক্ষ NFA_MOPEN2:
	পক্ষ NFA_MOPEN3:
	পক্ষ NFA_MOPEN4:
	পক্ষ NFA_MOPEN5:
	পক্ষ NFA_MOPEN6:
	পক্ষ NFA_MOPEN7:
	পক্ষ NFA_MOPEN8:
	পক্ষ NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	পক্ষ NFA_ZOPEN:
	পক্ষ NFA_ZOPEN1:
	পক্ষ NFA_ZOPEN2:
	পক্ষ NFA_ZOPEN3:
	পক্ষ NFA_ZOPEN4:
	পক্ষ NFA_ZOPEN5:
	পক্ষ NFA_ZOPEN6:
	পক্ষ NFA_ZOPEN7:
	পক্ষ NFA_ZOPEN8:
	পক্ষ NFA_ZOPEN9:
	পক্ষ NFA_ZCLOSE:
	পক্ষ NFA_ZCLOSE1:
	পক্ষ NFA_ZCLOSE2:
	পক্ষ NFA_ZCLOSE3:
	পক্ষ NFA_ZCLOSE4:
	পক্ষ NFA_ZCLOSE5:
	পক্ষ NFA_ZCLOSE6:
	পক্ষ NFA_ZCLOSE7:
	পক্ষ NFA_ZCLOSE8:
	পক্ষ NFA_ZCLOSE9:
#endif
	পক্ষ NFA_NOPEN:
	পক্ষ NFA_MCLOSE1:
	পক্ষ NFA_MCLOSE2:
	পক্ষ NFA_MCLOSE3:
	পক্ষ NFA_MCLOSE4:
	পক্ষ NFA_MCLOSE5:
	পক্ষ NFA_MCLOSE6:
	পক্ষ NFA_MCLOSE7:
	পক্ষ NFA_MCLOSE8:
	পক্ষ NFA_MCLOSE9:
	পক্ষ NFA_NCLOSE:
	    ঘূৰাই failure_chance(state->out, depth + 1);

	পক্ষ NFA_BACKREF1:
	পক্ষ NFA_BACKREF2:
	পক্ষ NFA_BACKREF3:
	পক্ষ NFA_BACKREF4:
	পক্ষ NFA_BACKREF5:
	পক্ষ NFA_BACKREF6:
	পক্ষ NFA_BACKREF7:
	পক্ষ NFA_BACKREF8:
	পক্ষ NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	পক্ষ NFA_ZREF1:
	পক্ষ NFA_ZREF2:
	পক্ষ NFA_ZREF3:
	পক্ষ NFA_ZREF4:
	পক্ষ NFA_ZREF5:
	পক্ষ NFA_ZREF6:
	পক্ষ NFA_ZREF7:
	পক্ষ NFA_ZREF8:
	পক্ষ NFA_ZREF9:
#endif
	    // backreferences don't match in many places
	    ঘূৰাই 94;

	পক্ষ NFA_LNUM_GT:
	পক্ষ NFA_LNUM_LT:
	পক্ষ NFA_COL_GT:
	পক্ষ NFA_COL_LT:
	পক্ষ NFA_VCOL_GT:
	পক্ষ NFA_VCOL_LT:
	পক্ষ NFA_MARK_GT:
	পক্ষ NFA_MARK_LT:
	পক্ষ NFA_VISUAL:
	    // before/after positions don't match very often
	    ঘূৰাই 85;

	পক্ষ NFA_LNUM:
	    ঘূৰাই 90;

	পক্ষ NFA_CURSOR:
	পক্ষ NFA_COL:
	পক্ষ NFA_VCOL:
	পক্ষ NFA_MARK:
	    // specific positions rarely match
	    ঘূৰাই 98;

	পক্ষ NFA_COMPOSING:
	    ঘূৰাই 95;

	স্ৱভাৱ:
	    যদি (c > 0)
		// character match fails often
		ঘূৰাই 95;
    }

    // something else, includes character classes
    ঘূৰাই 50;
}

/*
 * Skip until the char "c" we know a match must start with.
 */
    স্থির পূর্ণাঙ্ক
skip_to_start(পূর্ণাঙ্ক c, colnr_T *colp)
{
    char_u *s;

    // Used often, do some work to avoid call overhead.
    যদি (!rex.reg_ic && !has_mbyte)
	s = vim_strbyte(rex.সাৰি + *colp, c);
    অন্যথা
	s = cstrchr(rex.সাৰি + *colp, c);
    যদি (s == রিক্ত)
	ঘূৰাই FAIL;
    *colp = (পূর্ণাঙ্ক)(s - rex.সাৰি);
    ঘূৰাই OK;
}

/*
 * Check for a match with match_text.
 * Called after skip_to_start() has found regstart.
 * Returns zero for no match, 1 for a match.
 */
    স্থির long
find_match_text(colnr_T *startcol, পূর্ণাঙ্ক regstart, char_u *match_text)
{
    colnr_T col = *startcol;
    পূর্ণাঙ্ক	    c1, c2;
    পূর্ণাঙ্ক	    len1, len2;
    পূর্ণাঙ্ক	    match;

    বাবে (;;)
    {
	match = TRUE;
	// Skip the input character: case-folding can change its byte length.
	len2 = MB_CPTR2LEN(rex.সাৰি + col);
	বাবে (len1 = 0; match_text[len1] != NUL; len1 += MB_CHAR2LEN(c1))
	{
	    c1 = PTR2CHAR(match_text + len1);
	    c2 = PTR2CHAR(rex.সাৰি + col + len2);
	    যদি (c1 != c2 && (!rex.reg_ic || MB_CASEFOLD(c1) != MB_CASEFOLD(c2)))
	    {
		match = FALSE;
		ভঙ্গ;
	    }
	    len2 += enc_utf8 ? utf_ptr2len(rex.সাৰি + col + len2)
							     : MB_CHAR2LEN(c2);
	}
	যদি (match
		// check that no composing char follows
		&& !(enc_utf8
			  && utf_iscomposing(PTR2CHAR(rex.সাৰি + col + len2))))
	{
	    cleanup_subexpr();
	    যদি (REG_MULTI)
	    {
		rex.reg_startpos[0].lnum = rex.lnum;
		rex.reg_startpos[0].col = col;
		rex.reg_endpos[0].lnum = rex.lnum;
		rex.reg_endpos[0].col = col + len2;
	    }
	    অন্যথা
	    {
		rex.reg_startp[0] = rex.সাৰি + col;
		rex.reg_endp[0] = rex.সাৰি + col + len2;
	    }
	    *startcol = col;
	    ঘূৰাই 1L;
	}

	// Try finding regstart after the current match.
	col += MB_CPTR2LEN(rex.সাৰি + col); // skip the input character
	যদি (skip_to_start(regstart, &col) == FAIL)
	    ভঙ্গ;
    }

    *startcol = col;
    ঘূৰাই 0L;
}

/*
 * Check whether the composing characters at the current input position match
 * the NFA composing sub-expression that starts at "sta" (the first state
 * after NFA_COMPOSING).  "curc" is the base character and "clen" its byte
 * length.
 * Returns OK when it matches, FAIL otherwise.
 */
    স্থির পূর্ণাঙ্ক
match_composing(nfa_state_T *sta, পূর্ণাঙ্ক curc, পূর্ণাঙ্ক clen)
{
    পূর্ণাঙ্ক		mc = curc;
    পূর্ণাঙ্ক		len = 0;
    পূর্ণাঙ্ক		cchars[MAX_MCO];
    পূর্ণাঙ্ক		ccount = 0;
    পূর্ণাঙ্ক		j;

    যদি (utf_iscomposing(sta->c))
	// Only match composing character(s), ignore base character.  Used
	// for ".{composing}" and "{composing}" (no preceding character).
	len += mb_char2len(mc);

    যদি (rex.reg_icombine && len == 0)
	// If \Z was present, then ignore composing characters.  When
	// ignoring the base character this always matches.
	ঘূৰাই sta->c == curc ? OK : FAIL;

    // Check base character matches first, unless ignored.
    যদি (len == 0 && mc != sta->c)
	ঘূৰাই FAIL;

    যদি (len == 0)
    {
	len += mb_char2len(mc);
	sta = sta->out;
    }

    // We don't care about the order of composing characters.  Get them into
    // cchars[] first.
    যেতিয়ালৈ (len < clen)
    {
	mc = mb_ptr2char(rex.প্ৰবেশ + len);
	cchars[ccount++] = mc;
	len += mb_char2len(mc);
	যদি (ccount == MAX_MCO)
	    ভঙ্গ;
    }

    // Check that each composing char in the pattern matches a composing char
    // in the text.  We do not check if all composing chars are matched.
    যেতিয়ালৈ (sta->c != NFA_END_COMPOSING)
    {
	বাবে (j = 0; j < ccount; ++j)
	    যদি (cchars[j] == sta->c)
		ভঙ্গ;
	যদি (j == ccount)
	    ঘূৰাই FAIL;
	sta = sta->out;
    }

    ঘূৰাই OK;
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
    স্থির পূর্ণাঙ্ক
nfa_regmatch(
    nfa_regprog_T	*prog,
    nfa_state_T		*start,
    regsubs_T		*submatch,
    regsubs_T		*m)
{
    পূর্ণাঙ্ক		result = FALSE;
    size_t	size = 0;
    পূর্ণাঙ্ক		flag = 0;
    পূর্ণাঙ্ক		go_to_nextline = FALSE;
    nfa_thread_T *t;
    nfa_list_T	list[2];
    পূর্ণাঙ্ক		listidx;
    nfa_list_T	*thislist;
    nfa_list_T	*nextlist;
    পূর্ণাঙ্ক		*listids = রিক্ত;
    পূর্ণাঙ্ক		listids_len = 0;
    nfa_state_T *add_state;
    পূর্ণাঙ্ক		add_here;
    পূর্ণাঙ্ক		add_গুনতি;
    পূর্ণাঙ্ক		add_off = 0;
    পূর্ণাঙ্ক		toplevel = start->c == NFA_MOPEN;
    regsubs_T	*r;
#ifdef NFA_REGEXP_DEBUG_LOG
    FILE	*debug;
#endif

    // Some patterns may take a long time to match, especially when using
    // recursive_regmatch(). Allow interrupting them with CTRL-C.
    fast_breakcheck();
    যদি (got_int)
	ঘূৰাই FALSE;
#ifdef FEAT_RELTIME
    যদি (nfa_did_time_out())
	ঘূৰাই FALSE;
#endif

#ifdef NFA_REGEXP_DEBUG_LOG
    debug = fopen(NFA_REGEXP_DEBUG_LOG, "a");
    যদি (debug == রিক্ত)
    {
	semsg("(NFA) COULD NOT OPEN %s!", NFA_REGEXP_DEBUG_LOG);
	ঘূৰাই FALSE;
    }
#endif
    nfa_match = FALSE;

    // Allocate memory for the lists of nodes.
    size = (prog->nstate + 1) * পরিমাণ(nfa_thread_T);

    // Reuse cached list buffers from prog when available (top-level call).
    // Recursive calls must allocate their own buffers.
    যদি (toplevel && prog->listbuf[0] != রিক্ত)
    {
	list[0].t = (nfa_thread_T *)prog->listbuf[0];
	list[1].t = (nfa_thread_T *)prog->listbuf[1];
	prog->listbuf[0] = রিক্ত;
	prog->listbuf[1] = রিক্ত;
    }
    অন্যথা
    {
	list[0].t = alloc(size);
	list[1].t = alloc(size);
    }
    list[0].len = prog->nstate + 1;
    list[1].len = prog->nstate + 1;
    যদি (list[0].t == রিক্ত || list[1].t == রিক্ত)
	গচ্ছ theend;

#ifdef ENABLE_LOG
    log_fd = fopen(NFA_REGEXP_RUN_LOG, "a");
    যদি (log_fd == রিক্ত)
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
    যদি (toplevel)
    {
	যদি (REG_MULTI)
	{
	    m->norm.list.multi[0].start_lnum = rex.lnum;
	    m->norm.list.multi[0].start_col = (colnr_T)(rex.প্ৰবেশ - rex.সাৰি);
	    m->norm.orig_start_col = m->norm.list.multi[0].start_col;
	}
	অন্যথা
	    m->norm.list.সাৰি[0].start = rex.প্ৰবেশ;
	m->norm.in_use = 1;
	r = addstate(thislist, start->out, m, রিক্ত, 0, 0);
    }
    অন্যথা
	r = addstate(thislist, start, m, রিক্ত, 0, 0);
    যদি (r == রিক্ত)
    {
	nfa_match = NFA_TOO_EXPENSIVE;
	গচ্ছ theend;
    }

#define	ADD_STATE_IF_MATCH(state)	\
    যদি (result)				\
    {					\
	add_state = state->out;		\
	add_off = clen;			\
    }

    /*
     * Run for each character.
     */
    বাবে (;;)
    {
	পূর্ণাঙ্ক	curc;
	পূর্ণাঙ্ক	clen;

	যদি (has_mbyte)
	{
	    // Fast path for an ASCII byte not followed by a composing
	    // character, matching the check in utfc_ptr2len().  Avoids two
	    // indirect calls for the common case.
	    যদি (rex.প্ৰবেশ[0] != NUL && rex.প্ৰবেশ[0] < 0x80 && rex.প্ৰবেশ[1] < 0x80)
	    {
		curc = rex.প্ৰবেশ[0];
		clen = 1;
	    }
	    অন্যথা
	    {
		curc = (*mb_ptr2char)(rex.প্ৰবেশ);
		clen = (*mb_ptr2len)(rex.প্ৰবেশ);
	    }
	}
	অন্যথা
	{
	    curc = *rex.প্ৰবেশ;
	    clen = 1;
	}
	যদি (curc == NUL)
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
	যদি (prog->re_engine == AUTOMATIC_ENGINE
		&& (rex.nfa_listid >= NFA_MAX_STATES
#ifdef FEAT_EVAL
		    || nfa_fail_for_testing
#endif
		    ))
	{
	    // too many states, retry with old engine
	    nfa_match = NFA_TOO_EXPENSIVE;
	    গচ্ছ theend;
	}

	thislist->id = rex.nfa_listid;
	nextlist->id = rex.nfa_listid + 1;

#ifdef ENABLE_LOG
	fprintf(log_fd, "------------------------------------------\n");
	fprintf(log_fd, ">>> Reginput is \"%s\"\n", rex.প্ৰবেশ);
	fprintf(log_fd, ">>> Advanced one character... Current char is %c (code %d) \n", curc, (পূর্ণাঙ্ক)curc);
	fprintf(log_fd, ">>> Thislist has %d states available: ", thislist->n);
	{
	    পূর্ণাঙ্ক i;

	    বাবে (i = 0; i < thislist->n; i++)
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
	যদি (thislist->n == 0)
	    ভঙ্গ;

	// compute nextlist
	বাবে (listidx = 0; listidx < thislist->n; ++listidx)
	{
	    // If the list gets very long there probably is something wrong.
	    // At least allow interrupting with CTRL-C.  Only check once in a
	    // while, calling ui_breakcheck() for every state is too slow.
	    স্থির পূর্ণাঙ্ক	breakcheck_গুনতি = 0;  // using "static" makes it faster
	    যদি (unlikely(++breakcheck_গুনতি >= 100000))
	    {
		ui_breakcheck();
		breakcheck_গুনতি = 0;
	    }
	    যদি (got_int)
		ভঙ্গ;
#ifdef FEAT_RELTIME
	    যদি (nfa_did_time_out())
		ভঙ্গ;
#endif
	    t = &thislist->t[listidx];

#ifdef NFA_REGEXP_DEBUG_LOG
	    nfa_set_code(t->state->c);
	    fprintf(debug, "%s, ", code);
#endif
#ifdef ENABLE_LOG
	    {
		পূর্ণাঙ্ক col;

		যদি (t->subs.norm.in_use <= 0)
		    col = -1;
		অন্যথা যদি (REG_MULTI)
		    col = t->subs.norm.list.multi[0].start_col;
		অন্যথা
		    col = (পূর্ণাঙ্ক)(t->subs.norm.list.সাৰি[0].start - rex.সাৰি);
		nfa_set_code(t->state->c);
		fprintf(log_fd, "(%d) char %d %s (start col %d)%s... \n",
			abs(t->state->id), (পূর্ণাঙ্ক)t->state->c, code, col,
			pim_info(&t->pim));
	    }
#endif

	    /*
	     * Handle the possible codes of the current state.
	     * The most important is NFA_MATCH.
	     */
	    add_state = রিক্ত;
	    add_here = FALSE;
	    add_গুনতি = 0;
	    ৱিকল্প (t->state->c)
	    {
	    পক্ষ NFA_MATCH:
	      {
		// If the match is not at the start of the line, ends before a
		// composing characters and rex.reg_icombine is not set, that
		// is not really a match.
		যদি (enc_utf8 && !rex.reg_icombine
			     && rex.প্ৰবেশ != rex.সাৰি && utf_iscomposing(curc))
		    ভঙ্গ;

		nfa_match = TRUE;
		copy_sub(&submatch->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		যদি (rex.nfa_has_zsubexpr)
		    copy_sub(&submatch->synt, &t->subs.synt);
#endif
#ifdef ENABLE_LOG
		log_subsexpr(&t->subs);
#endif
		// Found the left-most longest match, do not look at any other
		// states at this position.  When the list of states is going
		// to be empty quit without advancing, so that "rex.input" is
		// correct.
		যদি (nextlist->n == 0)
		    clen = 0;
		গচ্ছ nextchar;
	      }

	    পক্ষ NFA_END_INVISIBLE:
	    পক্ষ NFA_END_INVISIBLE_NEG:
	    পক্ষ NFA_END_PATTERN:
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
		যদি (nfa_endp != রিক্ত)
		{
		    যদি (REG_MULTI)
			fprintf(log_fd, "Current lnum: %d, endp lnum: %d; current col: %d, endp col: %d\n",
				(পূর্ণাঙ্ক)rex.lnum,
				(পূর্ণাঙ্ক)nfa_endp->se_u.pos.lnum,
				(পূর্ণাঙ্ক)(rex.প্ৰবেশ - rex.সাৰি),
				nfa_endp->se_u.pos.col);
		    অন্যথা
			fprintf(log_fd, "Current col: %d, endp col: %d\n",
				(পূর্ণাঙ্ক)(rex.প্ৰবেশ - rex.সাৰি),
				(পূর্ণাঙ্ক)(nfa_endp->se_u.ptr - rex.প্ৰবেশ));
		}
#endif
		// If "nfa_endp" is set it's only a match if it ends at
		// "nfa_endp"
		যদি (nfa_endp != রিক্ত && (REG_MULTI
			? (rex.lnum != nfa_endp->se_u.pos.lnum
			    || (পূর্ণাঙ্ক)(rex.প্ৰবেশ - rex.সাৰি)
						!= nfa_endp->se_u.pos.col)
			: rex.প্ৰবেশ != nfa_endp->se_u.ptr))
		    ভঙ্গ;

		// do not set submatches for \@!
		যদি (t->state->c != NFA_END_INVISIBLE_NEG)
		{
		    copy_sub(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		    যদি (rex.nfa_has_zsubexpr)
			copy_sub(&m->synt, &t->subs.synt);
#endif
		}
#ifdef ENABLE_LOG
		fprintf(log_fd, "Match found:\n");
		log_subsexpr(m);
#endif
		nfa_match = TRUE;
		// See comment above at "goto nextchar".
		যদি (nextlist->n == 0)
		    clen = 0;
		গচ্ছ nextchar;

	    পক্ষ NFA_START_INVISIBLE:
	    পক্ষ NFA_START_INVISIBLE_FIRST:
	    পক্ষ NFA_START_INVISIBLE_NEG:
	    পক্ষ NFA_START_INVISIBLE_NEG_FIRST:
	    পক্ষ NFA_START_INVISIBLE_BEFORE:
	    পক্ষ NFA_START_INVISIBLE_BEFORE_FIRST:
	    পক্ষ NFA_START_INVISIBLE_BEFORE_NEG:
	    পক্ষ NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
		{
#ifdef ENABLE_LOG
		    fprintf(log_fd, "Failure chance invisible: %d, what follows: %d\n",
			    failure_chance(t->state->out, 0),
			    failure_chance(t->state->out1->out, 0));
#endif
		    // Do it directly if there already is a PIM or when
		    // nfa_postprocess() detected it will work better.
		    যদি (t->pim.result != NFA_PIM_UNUSED
			 || t->state->c == NFA_START_INVISIBLE_FIRST
			 || t->state->c == NFA_START_INVISIBLE_NEG_FIRST
			 || t->state->c == NFA_START_INVISIBLE_BEFORE_FIRST
			 || t->state->c == NFA_START_INVISIBLE_BEFORE_NEG_FIRST)
		    {
			পূর্ণাঙ্ক in_use = m->norm.in_use;

			// Copy submatch info for the recursive call, opposite
			// of what happens on success below.
			copy_sub_off(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
			যদি (rex.nfa_has_zsubexpr)
			    copy_sub_off(&m->synt, &t->subs.synt);
#endif

			/*
			 * First try matching the invisible match, then what
			 * follows.
			 */
			result = recursive_regmatch(t->state, রিক্ত, prog,
					  submatch, m, &listids, &listids_len);
			যদি (result == NFA_TOO_EXPENSIVE)
			{
			    nfa_match = result;
			    গচ্ছ theend;
			}

			// for \@! and \@<! it is a match when the result is
			// FALSE
			যদি (result != (t->state->c == NFA_START_INVISIBLE_NEG
			       || t->state->c == NFA_START_INVISIBLE_NEG_FIRST
			       || t->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			       || t->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&t->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
			    যদি (rex.nfa_has_zsubexpr)
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
		    অন্যথা
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
			যদি (REG_MULTI)
			{
			    pim.end.pos.col = (পূর্ণাঙ্ক)(rex.প্ৰবেশ - rex.সাৰি);
			    pim.end.pos.lnum = rex.lnum;
			}
			অন্যথা
			    pim.end.ptr = rex.প্ৰবেশ;

			// t->state->out1 is the corresponding END_INVISIBLE
			// node; Add its out to the current list (zero-width
			// match).
			যদি (addstate_here(thislist, t->state->out1->out,
					     &t->subs, &pim, &listidx) == রিক্ত)
			{
			    nfa_match = NFA_TOO_EXPENSIVE;
			    গচ্ছ theend;
			}
		    }
		}
		ভঙ্গ;

	    পক্ষ NFA_START_PATTERN:
	      {
		nfa_state_T *skip = রিক্ত;
#ifdef ENABLE_LOG
		পূর্ণাঙ্ক	    skip_lid = 0;
#endif

		// There is no point in trying to match the pattern if the
		// output state is not going to be added to the list.
		যদি (state_in_list(nextlist, t->state->out1->out, &t->subs))
		{
		    skip = t->state->out1->out;
#ifdef ENABLE_LOG
		    skip_lid = nextlist->id;
#endif
		}
		অন্যথা যদি (state_in_list(nextlist,
					  t->state->out1->out->out, &t->subs))
		{
		    skip = t->state->out1->out->out;
#ifdef ENABLE_LOG
		    skip_lid = nextlist->id;
#endif
		}
		অন্যথা যদি (state_in_list(thislist,
					  t->state->out1->out->out, &t->subs))
		{
		    skip = t->state->out1->out->out;
#ifdef ENABLE_LOG
		    skip_lid = thislist->id;
#endif
		}
		যদি (skip != রিক্ত)
		{
#ifdef ENABLE_LOG
		    nfa_set_code(skip->c);
		    fprintf(log_fd, "> Not trying to match pattern, output state %d is already in list %d. char %d: %s\n",
			    abs(skip->id), skip_lid, skip->c, code);
#endif
		    ভঙ্গ;
		}
		// Copy submatch info to the recursive call, opposite of what
		// happens afterwards.
		copy_sub_off(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		যদি (rex.nfa_has_zsubexpr)
		    copy_sub_off(&m->synt, &t->subs.synt);
#endif

		// First try matching the pattern.
		result = recursive_regmatch(t->state, রিক্ত, prog,
					  submatch, m, &listids, &listids_len);
		যদি (result == NFA_TOO_EXPENSIVE)
		{
		    nfa_match = result;
		    গচ্ছ theend;
		}
		যদি (result)
		{
		    পূর্ণাঙ্ক bytelen;

#ifdef ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN matches:\n");
		    log_subsexpr(m);
#endif
		    // Copy submatch info from the recursive call
		    copy_sub_off(&t->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
		    যদি (rex.nfa_has_zsubexpr)
			copy_sub_off(&t->subs.synt, &m->synt);
#endif
		    // Now we need to skip over the matched text and then
		    // continue with what follows.
		    যদি (REG_MULTI)
			// TODO: multi-line match
			bytelen = m->norm.list.multi[0].end_col
						  - (পূর্ণাঙ্ক)(rex.প্ৰবেশ - rex.সাৰি);
		    অন্যথা
			bytelen = (পূর্ণাঙ্ক)(m->norm.list.সাৰি[0].end - rex.প্ৰবেশ);

#ifdef ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN length: %d\n", bytelen);
#endif
		    যদি (bytelen == 0)
		    {
			// empty match, output of corresponding
			// NFA_END_PATTERN/NFA_SKIP to be used at current
			// position
			add_here = TRUE;
			add_state = t->state->out1->out->out;
		    }
		    অন্যথা যদি (bytelen <= clen)
		    {
			// match current character, output of corresponding
			// NFA_END_PATTERN to be used at next position.
			add_state = t->state->out1->out->out;
			add_off = clen;
		    }
		    অন্যথা
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			add_state = t->state->out1->out;
			add_off = bytelen;
			add_গুনতি = bytelen - clen;
		    }
		}
		ভঙ্গ;
	      }

	    পক্ষ NFA_BOL:
		যদি (rex.প্ৰবেশ == rex.সাৰি)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ভঙ্গ;

	    পক্ষ NFA_EOL:
		যদি (curc == NUL)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ভঙ্গ;

	    পক্ষ NFA_BOW:
		result = TRUE;

		যদি (curc == NUL)
		    result = FALSE;
		অন্যথা যদি (has_mbyte)
		{
		    পূর্ণাঙ্ক this_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.প্ৰবেশ, rex.reg_buf);
		    যদি (this_class <= 1)
			result = FALSE;
		    অন্যথা যদি (reg_prev_class() == this_class)
			result = FALSE;
		}
		অন্যথা যদি (!vim_iswordc_buf(curc, rex.reg_buf)
			   || (rex.প্ৰবেশ > rex.সাৰি
			       && vim_iswordc_buf(rex.প্ৰবেশ[-1], rex.reg_buf)))
		    result = FALSE;
		যদি (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ভঙ্গ;

	    পক্ষ NFA_EOW:
		result = TRUE;
		যদি (rex.প্ৰবেশ == rex.সাৰি)
		    result = FALSE;
		অন্যথা যদি (has_mbyte)
		{
		    পূর্ণাঙ্ক this_class, prev_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.প্ৰবেশ, rex.reg_buf);
		    prev_class = reg_prev_class();
		    যদি (this_class == prev_class
					|| prev_class == 0 || prev_class == 1)
			result = FALSE;
		}
		অন্যথা যদি (!vim_iswordc_buf(rex.প্ৰবেশ[-1], rex.reg_buf)
			|| (rex.প্ৰবেশ[0] != NUL
					&& vim_iswordc_buf(curc, rex.reg_buf)))
		    result = FALSE;
		যদি (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ভঙ্গ;

	    পক্ষ NFA_BOF:
		যদি (rex.lnum == 0 && rex.প্ৰবেশ == rex.সাৰি
				     && (!REG_MULTI || rex.reg_firstlnum == 1))
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ভঙ্গ;

	    পক্ষ NFA_EOF:
		যদি (rex.lnum == rex.reg_maxline && curc == NUL)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ভঙ্গ;

	    পক্ষ NFA_COMPOSING:
	    {
		nfa_state_T *end;

		result = match_composing(t->state->out, curc, clen);

		end = t->state->out1;	    // NFA_END_COMPOSING
		ADD_STATE_IF_MATCH(end);
		ভঙ্গ;
	    }

	    পক্ষ NFA_NEWL:
		যদি (curc == NUL && !rex.reg_সাৰি_lbr && REG_MULTI
						&& rex.lnum <= rex.reg_maxline)
		{
		    go_to_nextline = TRUE;
		    // Pass -1 for the offset, which means taking the position
		    // at the start of the next line.
		    add_state = t->state->out;
		    add_off = -1;
		}
		অন্যথা যদি (curc == '\n' && rex.reg_সাৰি_lbr)
		{
		    // match \n as if it is an ordinary character
		    add_state = t->state->out;
		    add_off = 1;
		}
		ভঙ্গ;

	    পক্ষ NFA_START_COLL:
	    পক্ষ NFA_START_NEG_COLL:
	      {
		// What follows is a list of characters, until NFA_END_COLL.
		// One of them must match or none of them must match.
		nfa_state_T	*state;
		পূর্ণাঙ্ক		result_if_matched;
		পূর্ণাঙ্ক		c1, c2;

		// Never match EOL. If it's part of the collection it is added
		// as a separate state with an OR.
		যদি (curc == NUL)
		    ভঙ্গ;

		state = t->state->out;
		result_if_matched = (t->state->c == NFA_START_COLL);
		বাবে (;;)
		{
		    যদি (state->c == NFA_COMPOSING)
		    {
			nfa_state_T *end;

			result = match_composing(t->state->out->out, curc,
									clen);

			যদি (t->state->out->out1 != রিক্ত
				&& t->state->out->out1->c == NFA_END_COMPOSING)
			{
			    end = t->state->out->out1;
			    ADD_STATE_IF_MATCH(end);
			}
			ভঙ্গ;
		    }
		    যদি (state->c == NFA_END_COLL)
		    {
			result = !result_if_matched;
			ভঙ্গ;
		    }
		    যদি (state->c == NFA_RANGE_MIN)
		    {
			c1 = state->val;
			state = state->out; // advance to NFA_RANGE_MAX
			c2 = state->val;
#ifdef ENABLE_LOG
			fprintf(log_fd, "NFA_RANGE_MIN curc=%d c1=%d c2=%d\n",
				curc, c1, c2);
#endif
			যদি (curc >= c1 && curc <= c2)
			{
			    result = result_if_matched;
			    ভঙ্গ;
			}
			যদি (rex.reg_ic)
			{
			    পূর্ণাঙ্ক curc_নিচেৰ = MB_CASEFOLD(curc);
			    পূর্ণাঙ্ক done = FALSE;

			    বাবে ( ; c1 <= c2; ++c1)
				যদি (MB_CASEFOLD(c1) == curc_নিচেৰ)
				{
				    result = result_if_matched;
				    done = TRUE;
				    ভঙ্গ;
				}
			    যদি (done)
				ভঙ্গ;
			}
		    }
		    অন্যথা যদি (state->c < 0 ? check_char_class(state->c, curc)
			       : (curc == state->c
				   || (rex.reg_ic && MB_CASEFOLD(curc)
						    == MB_CASEFOLD(state->c))))
		    {
			result = result_if_matched;
			ভঙ্গ;
		    }
		    state = state->out;
		}
		যদি (result)
		{
		    // next state is in out of the NFA_END_COLL, out1 of
		    // START points to the END state
		    add_state = t->state->out1->out;
		    add_off = clen;
		}
		ভঙ্গ;
	      }

	    পক্ষ NFA_ANY:
		// Any char except '\0', (end of input) does not match.
		যদি (curc > 0)
		{
		    add_state = t->state->out;
		    add_off = clen;
		}
		ভঙ্গ;

	    পক্ষ NFA_ANY_COMPOSING:
		// On a composing character skip over it.  Otherwise do
		// nothing.  Always matches.
		যদি (enc_utf8 && utf_iscomposing(curc))
		{
		    add_off = clen;
		}
		অন্যথা
		{
		    add_here = TRUE;
		    add_off = 0;
		}
		add_state = t->state->out;
		ভঙ্গ;

	    /*
	     * Character classes like \a for alpha, \d for digit etc.
	     */
	    পক্ষ NFA_IDENT:	//  \i
		result = vim_isIDc(curc);
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_SIDENT:	//  \I
		result = !VIM_ISDIGIT(curc) && vim_isIDc(curc);
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_KWORD:	//  \k
		result = vim_iswordp_buf(rex.প্ৰবেশ, rex.reg_buf);
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_SKWORD:	//  \K
		result = !VIM_ISDIGIT(curc)
				     && vim_iswordp_buf(rex.প্ৰবেশ, rex.reg_buf);
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_FNAME:	//  \f
		result = vim_isfilec(curc);
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_SFNAME:	//  \F
		result = !VIM_ISDIGIT(curc) && vim_isfilec(curc);
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_PRINT:	//  \p
		result = vim_isprintc(PTR2CHAR(rex.প্ৰবেশ));
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_SPRINT:	//  \P
		result = !VIM_ISDIGIT(curc) && vim_isprintc(PTR2CHAR(rex.প্ৰবেশ));
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_WHITE:	//  \s
		result = VIM_ISWHITE(curc);
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_NWHITE:	//  \S
		result = curc != NUL && !VIM_ISWHITE(curc);
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_DIGIT:	//  \d
		result = ri_digit(curc);
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_NDIGIT:	//  \D
		result = curc != NUL && !ri_digit(curc);
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_HEX:	//  \x
		result = ri_hex(curc);
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_NHEX:	//  \X
		result = curc != NUL && !ri_hex(curc);
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_OCTAL:	//  \o
		result = ri_octal(curc);
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_NOCTAL:	//  \O
		result = curc != NUL && !ri_octal(curc);
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_WORD:	//  \w
		result = ri_word(curc);
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_NWORD:	//  \W
		result = curc != NUL && !ri_word(curc);
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_HEAD:	//  \h
		result = ri_head(curc);
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_NHEAD:	//  \H
		result = curc != NUL && !ri_head(curc);
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_ALPHA:	//  \a
		result = ri_alpha(curc);
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_NALPHA:	//  \A
		result = curc != NUL && !ri_alpha(curc);
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_LOWER:	//  \l
		result = ri_lower(curc);
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_NLOWER:	//  \L
		result = curc != NUL && !ri_lower(curc);
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_UPPER:	//  \u
		result = ri_upper(curc);
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_NUPPER:	// \U
		result = curc != NUL && !ri_upper(curc);
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_LOWER_IC:	// [a-z]
		result = ri_lower(curc) || (rex.reg_ic && ri_upper(curc));
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_NLOWER_IC:	// [^a-z]
		result = curc != NUL
			&& !(ri_lower(curc) || (rex.reg_ic && ri_upper(curc)));
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_UPPER_IC:	// [A-Z]
		result = ri_upper(curc) || (rex.reg_ic && ri_lower(curc));
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_NUPPER_IC:	// ^[A-Z]
		result = curc != NUL
			&& !(ri_upper(curc) || (rex.reg_ic && ri_lower(curc)));
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;

	    পক্ষ NFA_BACKREF1:
	    পক্ষ NFA_BACKREF2:
	    পক্ষ NFA_BACKREF3:
	    পক্ষ NFA_BACKREF4:
	    পক্ষ NFA_BACKREF5:
	    পক্ষ NFA_BACKREF6:
	    পক্ষ NFA_BACKREF7:
	    পক্ষ NFA_BACKREF8:
	    পক্ষ NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	    পক্ষ NFA_ZREF1:
	    পক্ষ NFA_ZREF2:
	    পক্ষ NFA_ZREF3:
	    পক্ষ NFA_ZREF4:
	    পক্ষ NFA_ZREF5:
	    পক্ষ NFA_ZREF6:
	    পক্ষ NFA_ZREF7:
	    পক্ষ NFA_ZREF8:
	    পক্ষ NFA_ZREF9:
#endif
		// \1 .. \9  \z1 .. \z9
	      {
		পূর্ণাঙ্ক subidx;
		পূর্ণাঙ্ক bytelen;

#ifdef FEAT_SYN_HL
		যদি (t->state->c >= NFA_BACKREF1 && t->state->c <= NFA_BACKREF9)
#endif
		{
		    subidx = t->state->c - NFA_BACKREF1 + 1;
		    result = match_backref(&t->subs.norm, subidx, &bytelen);
		}
#ifdef FEAT_SYN_HL
		অন্যথা
		{
		    subidx = t->state->c - NFA_ZREF1 + 1;
		    result = match_zref(subidx, &bytelen);
		}
#endif

		যদি (result)
		{
		    যদি (bytelen == 0)
		    {
			// empty match always works, output of NFA_SKIP to be
			// used next
			add_here = TRUE;
			add_state = t->state->out->out;
		    }
		    অন্যথা যদি (bytelen <= clen)
		    {
			// match current character, jump ahead to out of
			// NFA_SKIP
			add_state = t->state->out->out;
			add_off = clen;
		    }
		    অন্যথা
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			add_state = t->state->out;
			add_off = bytelen;
			add_গুনতি = bytelen - clen;
		    }
		}
		ভঙ্গ;
	      }
	    পক্ষ NFA_SKIP:
	      // character of previous matching \1 .. \9  or \@>
	      যদি (t->গুনতি - clen <= 0)
	      {
		  // end of match, go to what follows
		  add_state = t->state->out;
		  add_off = clen;
	      }
	      অন্যথা
	      {
		  // add state again with decremented count
		  add_state = t->state;
		  add_off = 0;
		  add_গুনতি = t->গুনতি - clen;
	      }
	      ভঙ্গ;

	    পক্ষ NFA_LNUM:
	    পক্ষ NFA_LNUM_GT:
	    পক্ষ NFA_LNUM_LT:
		result = (REG_MULTI &&
			nfa_re_অঙ্ক_cmp(t->state->val, t->state->c - NFA_LNUM,
			    (long_u)(rex.lnum + rex.reg_firstlnum)));
		যদি (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ভঙ্গ;

	    পক্ষ NFA_COL:
	    পক্ষ NFA_COL_GT:
	    পক্ষ NFA_COL_LT:
		result = nfa_re_অঙ্ক_cmp(t->state->val, t->state->c - NFA_COL,
			(long_u)(rex.প্ৰবেশ - rex.সাৰি) + 1);
		যদি (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ভঙ্গ;

	    পক্ষ NFA_VCOL:
	    পক্ষ NFA_VCOL_GT:
	    পক্ষ NFA_VCOL_LT:
		{
		    পূর্ণাঙ্ক     op = t->state->c - NFA_VCOL;
		    colnr_T col = (colnr_T)(rex.প্ৰবেশ - rex.সাৰি);
		    win_T   *wp = rex.reg_win == রিক্ত ? curwin : rex.reg_win;

		    // Bail out quickly when there can't be a match, avoid the
		    // overhead of win_linetabsize() on long lines.
		    যদি (op != 1 && col > t->state->val
			    * (has_mbyte ? MB_MAXBYTES : 1))
			ভঙ্গ;
		    result = FALSE;
		    যদি (op == 1 && col - 1 > t->state->val && col > 100)
		    {
			পূর্ণাঙ্ক ts = wp->w_buffer->b_p_ts;

			// Guess that a character won't use more columns than
			// 'tabstop', with a minimum of 4.
			যদি (ts < 4)
			    ts = 4;
			result = col > t->state->val * ts;
		    }
		    যদি (!result)
		    {
			linenr_T    lnum = REG_MULTI
					    ? rex.reg_firstlnum + rex.lnum : 1;
			long_u	    vcol;

			যদি (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_buffer->b_ml.ml_সাৰি_গুনতি))
			    lnum = 1;
			vcol = (long_u)win_linetabsize(wp, lnum, rex.সাৰি, col);
			result = nfa_re_অঙ্ক_cmp(t->state->val, op, vcol + 1);
		    }
		    যদি (result)
		    {
			add_here = TRUE;
			add_state = t->state->out;
		    }
		}
		ভঙ্গ;

	    পক্ষ NFA_MARK:
	    পক্ষ NFA_MARK_GT:
	    পক্ষ NFA_MARK_LT:
	      {
		pos_T	*pos;
		size_t	col = REG_MULTI ? rex.প্ৰবেশ - rex.সাৰি : 0;

		pos = getmark_buf(rex.reg_buf, t->state->val, FALSE);

		// Line may have been freed, get it again.
		যদি (REG_MULTI)
		{
		    rex.সাৰি = reg_getline(rex.lnum);
		    rex.প্ৰবেশ = rex.সাৰি + col;
		}

		// Compare the mark position to the match position, if the mark
		// exists and mark is set in reg_buf.
		যদি (pos != রিক্ত && pos->lnum > 0)
		{
		    colnr_T pos_col = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->col == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->col;

		    result = (pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_col == (colnr_T)(rex.প্ৰবেশ - rex.সাৰি)
				    ? t->state->c == NFA_MARK
				    : (pos_col < (colnr_T)(rex.প্ৰবেশ - rex.সাৰি)
					? t->state->c == NFA_MARK_GT
					: t->state->c == NFA_MARK_LT))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? t->state->c == NFA_MARK_GT
				    : t->state->c == NFA_MARK_LT));
		    যদি (result)
		    {
			add_here = TRUE;
			add_state = t->state->out;
		    }
		}
		ভঙ্গ;
	      }

	    পক্ষ NFA_CURSOR:
		result = (rex.reg_win != রিক্ত
			&& (rex.lnum + rex.reg_firstlnum
						 == rex.reg_win->w_cursor.lnum)
			&& ((colnr_T)(rex.প্ৰবেশ - rex.সাৰি)
						== rex.reg_win->w_cursor.col));
		যদি (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ভঙ্গ;

	    পক্ষ NFA_VISUAL:
		result = reg_match_visual();
		যদি (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		ভঙ্গ;

	    পক্ষ NFA_MOPEN1:
	    পক্ষ NFA_MOPEN2:
	    পক্ষ NFA_MOPEN3:
	    পক্ষ NFA_MOPEN4:
	    পক্ষ NFA_MOPEN5:
	    পক্ষ NFA_MOPEN6:
	    পক্ষ NFA_MOPEN7:
	    পক্ষ NFA_MOPEN8:
	    পক্ষ NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	    পক্ষ NFA_ZOPEN:
	    পক্ষ NFA_ZOPEN1:
	    পক্ষ NFA_ZOPEN2:
	    পক্ষ NFA_ZOPEN3:
	    পক্ষ NFA_ZOPEN4:
	    পক্ষ NFA_ZOPEN5:
	    পক্ষ NFA_ZOPEN6:
	    পক্ষ NFA_ZOPEN7:
	    পক্ষ NFA_ZOPEN8:
	    পক্ষ NFA_ZOPEN9:
#endif
	    পক্ষ NFA_NOPEN:
	    পক্ষ NFA_ZSTART:
		// These states are only added to be able to bail out when
		// they are added again, nothing is to be done.
		ভঙ্গ;

	    স্ৱভাৱ:	// regular character
	      {
		পূর্ণাঙ্ক c = t->state->c;

#ifdef DEBUG
		যদি (c < 0)
		    siemsg("Negative state char: %ld", (long)c);
#endif
		result = (c == curc);

		যদি (!result && rex.reg_ic)
		    result = MB_CASEFOLD(c) == MB_CASEFOLD(curc);
		// If rex.reg_icombine is not set only skip over the character
		// itself.  When it is set skip over composing characters.
		যদি (result && enc_utf8 && !rex.reg_icombine)
		    clen = utf_ptr2len(rex.প্ৰবেশ);
		ADD_STATE_IF_MATCH(t->state);
		ভঙ্গ;
	      }

	    } // switch (t->state->c)

	    যদি (add_state != রিক্ত)
	    {
		nfa_pim_T *pim;
		nfa_pim_T pim_copy;

		যদি (t->pim.result == NFA_PIM_UNUSED)
		    pim = রিক্ত;
		অন্যথা
		    pim = &t->pim;

		// Handle the postponed invisible match if the match might end
		// without advancing and before the end of the line.
		যদি (pim != রিক্ত && (clen == 0 || match_follows(add_state, 0)))
		{
		    যদি (pim->result == NFA_PIM_TODO)
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
			যদি (result != (pim->state->c == NFA_START_INVISIBLE_NEG
			     || pim->state->c == NFA_START_INVISIBLE_NEG_FIRST
			     || pim->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&pim->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
			    যদি (rex.nfa_has_zsubexpr)
				copy_sub_off(&pim->subs.synt, &m->synt);
#endif
			}
		    }
		    অন্যথা
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
		    যদি (result != (pim->state->c == NFA_START_INVISIBLE_NEG
			     || pim->state->c == NFA_START_INVISIBLE_NEG_FIRST
			     || pim->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
		    {
			// Copy submatch info from the recursive call
			copy_sub_off(&t->subs.norm, &pim->subs.norm);
#ifdef FEAT_SYN_HL
			যদি (rex.nfa_has_zsubexpr)
			    copy_sub_off(&t->subs.synt, &pim->subs.synt);
#endif
		    }
		    অন্যথা
			// look-behind match failed, don't add the state
			অনুৱর্ত;

		    // Postponed invisible match was handled, don't add it to
		    // following states.
		    pim = রিক্ত;
		}

		// If "pim" points into l->t it will become invalid when
		// adding the state causes the list to be reallocated.  Make a
		// local copy to avoid that.
		যদি (pim == &t->pim)
		{
		    copy_pim(&pim_copy, pim);
		    pim = &pim_copy;
		}

		যদি (add_here)
		    r = addstate_here(thislist, add_state, &t->subs,
								pim, &listidx);
		অন্যথা
		{
		    r = addstate(nextlist, add_state, &t->subs, pim, add_off, 0);
		    যদি (add_গুনতি > 0)
			nextlist->t[nextlist->n - 1].গুনতি = add_গুনতি;
		}
		যদি (r == রিক্ত)
		{
		    nfa_match = NFA_TOO_EXPENSIVE;
		    গচ্ছ theend;
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
	যদি (nfa_match == FALSE
		&& ((toplevel
			&& rex.lnum == 0
			&& clen != 0
			&& (rex.reg_maxcol == 0
			  || (colnr_T)(rex.প্ৰবেশ - rex.সাৰি) < rex.reg_maxcol))
		    || (nfa_endp != রিক্ত
			&& (REG_MULTI
			    ? (rex.lnum < nfa_endp->se_u.pos.lnum
			       || (rex.lnum == nfa_endp->se_u.pos.lnum
				   && (পূর্ণাঙ্ক)(rex.প্ৰবেশ - rex.সাৰি)
						    < nfa_endp->se_u.pos.col))
			    : rex.প্ৰবেশ < nfa_endp->se_u.ptr))))
	{
#ifdef ENABLE_LOG
	    fprintf(log_fd, "(---) STARTSTATE\n");
#endif
	    // Inline optimized code for addstate() if we know the state is
	    // the first MOPEN.
	    যদি (toplevel)
	    {
		পূর্ণাঙ্ক add = TRUE;
		পূর্ণাঙ্ক c;

		যদি (prog->regstart != NUL && clen != 0)
		{
		    যদি (nextlist->n == 0)
		    {
			colnr_T col = (colnr_T)(rex.প্ৰবেশ - rex.সাৰি) + clen;

			// Nextlist is empty, we can skip ahead to the
			// character that must appear at the start.
			যদি (skip_to_start(prog->regstart, &col) == FAIL)
			    ভঙ্গ;
#ifdef ENABLE_LOG
			fprintf(log_fd, "  Skipping ahead %d bytes to regstart\n",
				col - ((colnr_T)(rex.প্ৰবেশ - rex.সাৰি) + clen));
#endif
			rex.প্ৰবেশ = rex.সাৰি + col - clen;
		    }
		    অন্যথা
		    {
			// Checking if the required start character matches is
			// cheaper than adding a state that won't match.
			c = PTR2CHAR(rex.প্ৰবেশ + clen);
			যদি (c != prog->regstart && (!rex.reg_ic
			     || MB_CASEFOLD(c) != MB_CASEFOLD(prog->regstart)))
			{
#ifdef ENABLE_LOG
			    fprintf(log_fd, "  Skipping start state, regstart does not match\n");
#endif
			    add = FALSE;
			}
		    }
		}

		যদি (add)
		{
		    যদি (REG_MULTI)
		    {
			m->norm.list.multi[0].start_col =
					 (colnr_T)(rex.প্ৰবেশ - rex.সাৰি) + clen;
			m->norm.orig_start_col =
					       m->norm.list.multi[0].start_col;
		    }
		    অন্যথা
			m->norm.list.সাৰি[0].start = rex.প্ৰবেশ + clen;
		    যদি (addstate(nextlist, start->out, m, রিক্ত, clen, 0) == রিক্ত)
		    {
			nfa_match = NFA_TOO_EXPENSIVE;
			গচ্ছ theend;
		    }
		}
	    }
	    অন্যথা
	    {
		char_u	    *save_সাৰি = rex.সাৰি;
		char_u	    *save_প্ৰবেশ = rex.প্ৰবেশ;
		linenr_T    save_lnum = rex.lnum;

		// At the end of a line the match can only start on the next
		// line, use that position instead of the line break.
		যদি (REG_MULTI && clen == 0 && nfa_endp != রিক্ত
					 && rex.lnum < nfa_endp->se_u.pos.lnum)
		{
		    char_u  *next_সাৰি = reg_getline(rex.lnum + 1);

		    যদি (next_সাৰি != রিক্ত)
		    {
			rex.সাৰি = next_সাৰি;
			rex.প্ৰবেশ = rex.সাৰি;
			++rex.lnum;
		    }
		}

		r = addstate(nextlist, start, m, রিক্ত, clen, 0);

		rex.সাৰি = save_সাৰি;
		rex.প্ৰবেশ = save_প্ৰবেশ;
		rex.lnum = save_lnum;

		যদি (r == রিক্ত)
		{
		    nfa_match = NFA_TOO_EXPENSIVE;
		    গচ্ছ theend;
		}
	    }
	}

#ifdef ENABLE_LOG
	fprintf(log_fd, ">>> Thislist had %d states available: ", thislist->n);
	{
	    পূর্ণাঙ্ক i;

	    বাবে (i = 0; i < thislist->n; i++)
		fprintf(log_fd, "%d  ", abs(thislist->t[i].state->id));
	}
	fprintf(log_fd, "\n");
#endif

nextchar:
	// Advance to the next character, or advance to the next line, or
	// finish.
	যদি (clen != 0)
	    rex.প্ৰবেশ += clen;
	অন্যথা যদি (go_to_nextline || (nfa_endp != রিক্ত && REG_MULTI
					&& rex.lnum < nfa_endp->se_u.pos.lnum))
	    reg_nextline();
	অন্যথা
	    ভঙ্গ;

	// Allow interrupting with CTRL-C.
	সাৰি_breakcheck();
	যদি (got_int)
	    ভঙ্গ;
#ifdef FEAT_RELTIME
	যদি (nfa_did_time_out())
	    ভঙ্গ;
#endif
    }

#ifdef ENABLE_LOG
    যদি (log_fd != stderr)
	fclose(log_fd);
    log_fd = রিক্ত;
#endif

theend:
    // Cache list buffers in prog for reuse, or free if prog already has
    // cached buffers (recursive call case).
    যদি (prog->listbuf[0] == রিক্ত && list[0].t != রিক্ত && list[1].t != রিক্ত)
    {
	prog->listbuf[0] = list[0].t;
	prog->listbuf[1] = list[1].t;
    }
    অন্যথা
    {
	vim_free(list[0].t);
	vim_free(list[1].t);
    }
    vim_free(listids);
#undef ADD_STATE_IF_MATCH
#ifdef NFA_REGEXP_DEBUG_LOG
    fclose(debug);
#endif

    ঘূৰাই nfa_match;
}

/*
 * Try match of "prog" with at rex.line["col"].
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    স্থির long
nfa_regtry(
    nfa_regprog_T   *prog,
    colnr_T	    col,
    পূর্ণাঙ্ক		    *timed_out UNUSED)	// flag set on timeout or NULL
{
    পূর্ণাঙ্ক		i;
    regsubs_T	subs, m;
    nfa_state_T	*start = prog->start;
    পূর্ণাঙ্ক		result;
#ifdef ENABLE_LOG
    FILE	*f;
#endif

    rex.প্ৰবেশ = rex.সাৰি + col;
#ifdef FEAT_RELTIME
    nfa_timed_out = timed_out;
#endif

#ifdef ENABLE_LOG
    f = fopen(NFA_REGEXP_RUN_LOG, "a");
    যদি (f != রিক্ত)
    {
	fprintf(f, "\n\n\t=======================================================\n");
# ifdef DEBUG
	fprintf(f, "\tRegexp is \"%s\"\n", nfa_regengine.ৰাশিমালা);
# endif
	fprintf(f, "\tInput text is \"%s\" \n", rex.প্ৰবেশ);
	fprintf(f, "\t=======================================================\n\n");
	nfa_print_state(f, start);
	fprintf(f, "\n\n");
	fclose(f);
    }
    অন্যথা
	emsg("Could not open temporary log file for writing");
#endif

    clear_sub(&subs.norm);
    clear_sub(&m.norm);
#ifdef FEAT_SYN_HL
    clear_sub(&subs.synt);
    clear_sub(&m.synt);
#endif

    result = nfa_regmatch(prog, start, &subs, &m);
    যদি (result == FALSE)
	ঘূৰাই 0;
    অন্যথা যদি (result == NFA_TOO_EXPENSIVE)
	ঘূৰাই result;

    cleanup_subexpr();
    যদি (REG_MULTI)
    {
	বাবে (i = 0; i < subs.norm.in_use; i++)
	{
	    rex.reg_startpos[i].lnum = subs.norm.list.multi[i].start_lnum;
	    rex.reg_startpos[i].col = subs.norm.list.multi[i].start_col;

	    rex.reg_endpos[i].lnum = subs.norm.list.multi[i].end_lnum;
	    rex.reg_endpos[i].col = subs.norm.list.multi[i].end_col;
	}
	যদি (rex.reg_mmatch != রিক্ত)
	    rex.reg_mmatch->rmm_matchcol = subs.norm.orig_start_col;

	যদি (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].col = col;
	}
	যদি (rex.reg_endpos[0].lnum < 0)
	{
	    // pattern has a \ze but it didn't match, use current end
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].col = (পূর্ণাঙ্ক)(rex.প্ৰবেশ - rex.সাৰি);
	}
	অন্যথা
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    অন্যথা
    {
	বাবে (i = 0; i < subs.norm.in_use; i++)
	{
	    rex.reg_startp[i] = subs.norm.list.সাৰি[i].start;
	    rex.reg_endp[i] = subs.norm.list.সাৰি[i].end;
	}

	যদি (rex.reg_startp[0] == রিক্ত)
	    rex.reg_startp[0] = rex.সাৰি + col;
	যদি (rex.reg_endp[0] == রিক্ত)
	    rex.reg_endp[0] = rex.প্ৰবেশ;
    }

#ifdef FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = রিক্ত;

    যদি (prog->reghasz == REX_SET)
    {
	cleanup_zsubexpr();
	re_extmatch_out = make_extmatch();
	যদি (re_extmatch_out == রিক্ত)
	    ঘূৰাই 0;
	// Loop over \z1, \z2, etc.  There is no \z0.
	বাবে (i = 1; i < subs.synt.in_use; i++)
	{
	    যদি (REG_MULTI)
	    {
		সংরচনা multipos *mpos = &subs.synt.list.multi[i];

		// Only accept single line matches that are valid.
		যদি (mpos->start_lnum >= 0
			&& mpos->start_lnum == mpos->end_lnum
			&& mpos->end_col >= mpos->start_col)
		    re_extmatch_out->matches[i] =
			vim_strnsave(reg_getline(mpos->start_lnum)
							    + mpos->start_col,
					     mpos->end_col - mpos->start_col);
	    }
	    অন্যথা
	    {
		সংরচনা linepos *lpos = &subs.synt.list.সাৰি[i];

		যদি (lpos->start != রিক্ত && lpos->end != রিক্ত)
		    re_extmatch_out->matches[i] =
			    vim_strnsave(lpos->start, lpos->end - lpos->start);
	    }
	}
    }
#endif

    ঘূৰাই 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 *
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    স্থির long
nfa_regexec_both(
    char_u	*সাৰি,
    colnr_T	startcol,	// column to start looking for match
    পূর্ণাঙ্ক		*timed_out)	// flag set on timeout or NULL
{
    nfa_regprog_T   *prog;
    long	    retval = 0L;
    পূর্ণাঙ্ক		    i;
    colnr_T	    col = startcol;

    যদি (REG_MULTI)
    {
	prog = (nfa_regprog_T *)rex.reg_mmatch->regprog;
	সাৰি = reg_getline((linenr_T)0);    // relative to the cursor
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    অন্যথা
    {
	prog = (nfa_regprog_T *)rex.reg_match->regprog;
	rex.reg_startp = rex.reg_match->startp;
	rex.reg_endp = rex.reg_match->endp;
    }

    // Be paranoid...
    যদি (prog == রিক্ত || সাৰি == রিক্ত)
    {
	iemsg(e_null_argument);
	গচ্ছ theend;
    }

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    যদি (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    অন্যথা যদি (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    যদি (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    rex.সাৰি = সাৰি;
    rex.lnum = 0;    // relative to line

    rex.nfa_has_zend = prog->has_zend;
    rex.nfa_has_backref = prog->has_backref;
    rex.nfa_nsubexpr = prog->nsubexp;
    rex.nfa_listid = 1;
    rex.nfa_alt_listid = 2;
#ifdef DEBUG
    nfa_regengine.ৰাশিমালা = prog->pattern;
#endif

    যদি (prog->reganch && col > 0)
	ঘূৰাই 0L;

    rex.need_clear_subexpr = TRUE;
#ifdef FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    যদি (prog->reghasz == REX_SET)
    {
	rex.nfa_has_zsubexpr = TRUE;
	rex.need_clear_zsubexpr = TRUE;
    }
    অন্যথা
    {
	rex.nfa_has_zsubexpr = FALSE;
	rex.need_clear_zsubexpr = FALSE;
    }
#endif

    যদি (prog->regstart != NUL)
    {
	// Skip ahead until a character we know the match must start with.
	// When there is none there is no match.
	যদি (skip_to_start(prog->regstart, &col) == FAIL)
	    ঘূৰাই 0L;

	// If match_text is set it contains the full text that must match.
	// Nothing else to try. Doesn't handle combining chars well.
	যদি (prog->match_text != রিক্ত && *prog->match_text != NUL && !rex.reg_icombine)
	{
	    retval = find_match_text(&col, prog->regstart, prog->match_text);
	    যদি (REG_MULTI)
		rex.reg_mmatch->rmm_matchcol = col;
	    অন্যথা
		rex.reg_match->rm_matchcol = col;
	    ঘূৰাই retval;
	}
    }

    // If the start column is past the maximum column: no need to try.
    যদি (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	গচ্ছ theend;

    // Set the "nstate" used by nfa_regcomp() to zero to trigger an error when
    // it's accidentally used during execution.
    nstate = 0;
    বাবে (i = 0; i < prog->nstate; ++i)
    {
	prog->state[i].id = i;
	prog->state[i].lastlist[0] = 0;
	prog->state[i].lastlist[1] = 0;
    }

    retval = nfa_regtry(prog, col, timed_out);

#ifdef DEBUG
    nfa_regengine.ৰাশিমালা = রিক্ত;
#endif

theend:
    যদি (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs and
	// \ze are used.
	যদি (REG_MULTI)
	{
	    lpos_T *start = &rex.reg_mmatch->startpos[0];
	    lpos_T *end = &rex.reg_mmatch->endpos[0];

	    যদি (end->lnum < start->lnum
			|| (end->lnum == start->lnum && end->col < start->col))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];
	}
	অন্যথা
	{
	    যদি (rex.reg_match->endp[0] < rex.reg_match->startp[0])
		rex.reg_match->endp[0] = rex.reg_match->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_match->rm_matchcol = col;
	}
    }

    ঘূৰাই retval;
}

/*
 * Compile a regular expression into internal code for the NFA matcher.
 * Returns the program in allocated space.  Returns NULL for an error.
 */
    স্থির regprog_T *
nfa_regcomp(char_u *ৰাশিমালা, পূর্ণাঙ্ক re_flags)
{
    nfa_regprog_T	*prog = রিক্ত;
    size_t		prog_size;
    পূর্ণাঙ্ক			*postfix;

    যদি (ৰাশিমালা == রিক্ত)
	ঘূৰাই রিক্ত;

#ifdef DEBUG
    nfa_regengine.ৰাশিমালা = ৰাশিমালা;
#endif
    nfa_re_flags = re_flags;

    init_class_tab();

    যদি (nfa_regcomp_start(ৰাশিমালা, re_flags) == FAIL)
	ঘূৰাই রিক্ত;

    // Build postfix form of the regexp. Needed to build the NFA
    // (and count its size).
    postfix = re2post();
    যদি (postfix == রিক্ত)
	গচ্ছ fail;	    // Cascaded (syntax?) error

    /*
     * In order to build the NFA, we parse the input regexp twice:
     * 1. first pass to count size (so we can allocate space)
     * 2. second to emit code
     */
#ifdef ENABLE_LOG
    {
	FILE *f = fopen(NFA_REGEXP_RUN_LOG, "a");

	যদি (f != রিক্ত)
	{
	    fprintf(f, "\n*****************************\n\n\n\n\tCompiling regexp \"%s\"... hold on !\n", ৰাশিমালা);
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
    prog_size = offsetof(nfa_regprog_T, state) + পরিমাণ(nfa_state_T) * nstate;
    prog = alloc(prog_size);
    যদি (prog == রিক্ত)
	গচ্ছ fail;
    state_ptr = prog->state;
    prog->re_in_use = FALSE;
    prog->listbuf[0] = রিক্ত;
    prog->listbuf[1] = রিক্ত;

    /*
     * PASS 2
     * Build the NFA
     */
    prog->start = post2nfa(postfix, post_ptr, FALSE);
    যদি (prog->start == রিক্ত)
	গচ্ছ fail;

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
    nfa_postfix_dump(ৰাশিমালা, OK);
    nfa_dump(prog);
#endif
#ifdef FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    prog->reghasz = re_has_z;
#endif
    prog->pattern = vim_strsave(ৰাশিমালা);
#ifdef DEBUG
    nfa_regengine.ৰাশিমালা = রিক্ত;
#endif

out:
    // The postfix list and fragment stack are reused by the next compilation
    // (and freed in free_regexp_stuff()), but drop ones that grew large for a
    // big pattern to avoid holding much memory.
    যদি (post_start_len > NFA_POSTFIX_KEEP)
    {
	VIM_CLEAR(post_start);
	post_start_len = 0;
    }
    post_ptr = post_end = রিক্ত;
    যদি (nfa_স্তূপ_len > NFA_স্তূপ_KEEP)
    {
	VIM_CLEAR(nfa_স্তূপ);
	nfa_স্তূপ_len = 0;
    }
    state_ptr = রিক্ত;
    ঘূৰাই (regprog_T *)prog;

fail:
    VIM_CLEAR(prog);
#ifdef ENABLE_LOG
    nfa_postfix_dump(ৰাশিমালা, FAIL);
#endif
#ifdef DEBUG
    nfa_regengine.ৰাশিমালা = রিক্ত;
#endif
    গচ্ছ out;
}

/*
 * Free a compiled regexp program, returned by nfa_regcomp().
 */
    স্থির শূন্য
nfa_regfree(regprog_T *prog)
{
    যদি (prog == রিক্ত)
	ঘূৰাই;

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
    স্থির পূর্ণাঙ্ক
nfa_regexec_nl(
    regmatch_T	*rmp,
    char_u	*সাৰি,	// string to match against
    colnr_T	col,	// column to start looking for match
    পূর্ণাঙ্ক		সাৰি_lbr)
{
    rex.reg_match = rmp;
    rex.reg_mmatch = রিক্ত;
    rex.reg_maxline = 0;
    rex.reg_সাৰি_lbr = সাৰি_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = রিক্ত;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;
    ঘূৰাই nfa_regexec_both(সাৰি, col, রিক্ত);
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
    স্থির long
nfa_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	col,		// column to start looking for match
    পূর্ণাঙ্ক		*timed_out)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    ঘূৰাই nfa_regexec_both(রিক্ত, col, timed_out);
}

#ifdef DEBUG
# undef ENABLE_LOG
#endif
