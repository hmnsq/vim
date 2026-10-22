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
    NFA_পূর্ব_ATOM_NO_WIDTH,	    // Used for \@=
    NFA_পূর্ব_ATOM_NO_WIDTH_NEG,	    // Used for \@!
    NFA_পূর্ব_ATOM_JUST_BEFORE,	    // Used for \@<=
    NFA_পূর্ব_ATOM_JUST_BEFORE_NEG,  // Used for \@<!
    NFA_পূর্ব_ATOM_LIKE_PATTERN,	    // Used for \@>

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
    NFA_শীর্ষ,		//	Match head char
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
স্থির ধ্রুবক পূর্ণসংখ্যা nfa_classcodes[] = {
    NFA_ANY, NFA_IDENT, NFA_SIDENT, NFA_KWORD,NFA_SKWORD,
    NFA_FNAME, NFA_SFNAME, NFA_PRINT, NFA_SPRINT,
    NFA_WHITE, NFA_NWHITE, NFA_DIGIT, NFA_NDIGIT,
    NFA_HEX, NFA_NHEX, NFA_OCTAL, NFA_NOCTAL,
    NFA_WORD, NFA_NWORD, NFA_শীর্ষ, NFA_NHEAD,
    NFA_ALPHA, NFA_NALPHA, NFA_LOWER, NFA_NLOWER,
    NFA_UPPER, NFA_NUPPER
};

// Variables only used in nfa_regcomp() and descendants.
স্থির পূর্ণসংখ্যা nfa_re_flags; // re_flags passed to nfa_regcomp()
স্থির পূর্ণসংখ্যা *post_start;  // holds the postfix form of r.e.
স্থির পূর্ণসংখ্যা post_start_len;  // size of allocated post_start (in ints)
স্থির পূর্ণসংখ্যা *post_end;
স্থির পূর্ণসংখ্যা *post_ptr;
স্থির পূর্ণসংখ্যা nfa_reg_parse_depth;	// nesting depth in nfa_reg()

// The postfix list (post_start) and fragment stack (nfa_stack) are reused
// across compilations; a buffer that grew past these sizes for a big pattern
// is freed afterwards instead of being kept, to avoid holding much memory.
#define NFA_POSTFIX_KEEP	10000	// number of ints (~40 Kbyte)
#define NFA_স্তূপ_KEEP		4000	// number of Frag_T (~64 Kbyte)

// Set when the pattern should use the NFA engine.
// E.g. [[:upper:]] only allows 8bit characters for BT engine,
// while NFA engine handles multibyte characters correctly.
স্থির পূর্ণসংখ্যা wants_nfa;

স্থির পূর্ণসংখ্যা nstate;	// Number of states in the NFA.
স্থির পূর্ণসংখ্যা istate;	// Index in the state vector, used in alloc_state()

// If not NULL match must end at this position
স্থির save_se_T *nfa_endp = ফাঁকা;

// 0 for first call to nfa_regmatch(), 1 for recursive call.
স্থির পূর্ণসংখ্যা nfa_ll_সূচক = 0;

স্থির পূর্ণসংখ্যা realloc_post_list(শূন্য);
স্থির পূর্ণসংখ্যা nfa_reg(পূর্ণসংখ্যা paren);
#ifdef DEBUG
স্থির শূন্য nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent);
#endif
স্থির পূর্ণসংখ্যা match_follows(nfa_state_T *startstate, পূর্ণসংখ্যা depth);
স্থির পূর্ণসংখ্যা failure_chance(nfa_state_T *state, পূর্ণসংখ্যা depth);

// helper functions used when doing re2post() ... regatom() parsing
#define EMIT(c)	করো {				\
		    যদি (post_ptr >= post_end && realloc_post_list() == FAIL) \
			ফেরত FAIL;		\
		    *post_ptr++ = c;		\
		} যখন (0)

/*
 * Initialize internal variables before NFA compilation.
 * Return OK on success, FAIL otherwise.
 */
    স্থির পূর্ণসংখ্যা
nfa_regcomp_start(
    char_u	*রাশিমালা,
    পূর্ণসংখ্যা		re_flags)	    // see vim_regcomp()
{
    size_t	postfix_আকার;
    পূর্ণসংখ্যা		nstate_max;

    nstate = 0;
    istate = 0;
    // A reasonable estimation for maximum size
    nstate_max = (পূর্ণসংখ্যা)(STRLEN(রাশিমালা) + 1) * 25;

    // Some items blow up in size, such as [A-z].  Add more space for that.
    // When it is still not enough realloc_post_list() will be used.
    nstate_max += 1000;

    // Size for postfix representation of expr.
    postfix_আকার = আকার(পূর্ণসংখ্যা) * nstate_max;

    // Reuse the postfix buffer across compilations, only growing it when the
    // estimate exceeds the current size; it is freed in free_regexp_stuff().
    যদি (post_start == ফাঁকা || post_start_len < nstate_max)
    {
	vim_free(post_start);
	post_start = alloc(postfix_আকার);
	যদি (post_start == ফাঁকা)
	{
	    post_start_len = 0;
	    ফেরত FAIL;
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
    regcomp_start(রাশিমালা, re_flags);

    ফেরত OK;
}

/*
 * Figure out if the NFA state list starts with an anchor, must match at start
 * of the line.
 */
    স্থির পূর্ণসংখ্যা
nfa_get_reganch(nfa_state_T *start, পূর্ণসংখ্যা depth)
{
    nfa_state_T *p = start;

    যদি (depth > 4)
	ফেরত 0;

    যখন (p != ফাঁকা)
    {
	নির্বাচন (p->c)
	{
	    ক্ষেত্র NFA_BOL:
	    ক্ষেত্র NFA_BOF:
		ফেরত 1; // yes!

	    ক্ষেত্র NFA_ZSTART:
	    ক্ষেত্র NFA_ZEND:
	    ক্ষেত্র NFA_CURSOR:
	    ক্ষেত্র NFA_VISUAL:

	    ক্ষেত্র NFA_MOPEN:
	    ক্ষেত্র NFA_MOPEN1:
	    ক্ষেত্র NFA_MOPEN2:
	    ক্ষেত্র NFA_MOPEN3:
	    ক্ষেত্র NFA_MOPEN4:
	    ক্ষেত্র NFA_MOPEN5:
	    ক্ষেত্র NFA_MOPEN6:
	    ক্ষেত্র NFA_MOPEN7:
	    ক্ষেত্র NFA_MOPEN8:
	    ক্ষেত্র NFA_MOPEN9:
	    ক্ষেত্র NFA_NOPEN:
#ifdef FEAT_SYN_HL
	    ক্ষেত্র NFA_ZOPEN:
	    ক্ষেত্র NFA_ZOPEN1:
	    ক্ষেত্র NFA_ZOPEN2:
	    ক্ষেত্র NFA_ZOPEN3:
	    ক্ষেত্র NFA_ZOPEN4:
	    ক্ষেত্র NFA_ZOPEN5:
	    ক্ষেত্র NFA_ZOPEN6:
	    ক্ষেত্র NFA_ZOPEN7:
	    ক্ষেত্র NFA_ZOPEN8:
	    ক্ষেত্র NFA_ZOPEN9:
#endif
		p = p->out;
		বিরতি;

	    ক্ষেত্র NFA_SPLIT:
		ফেরত nfa_get_reganch(p->out, depth + 1)
				       && nfa_get_reganch(p->out1, depth + 1);

	    পূর্বনির্ধারিত:
		ফেরত 0; // noooo
	}
    }
    ফেরত 0;
}

/*
 * Figure out if the NFA state list starts with a character which must match
 * at start of the match.
 */
    স্থির পূর্ণসংখ্যা
nfa_get_regstart(nfa_state_T *start, পূর্ণসংখ্যা depth)
{
    nfa_state_T *p = start;

    যদি (depth > 4)
	ফেরত 0;

    যখন (p != ফাঁকা)
    {
	নির্বাচন (p->c)
	{
	    // all kinds of zero-width matches
	    ক্ষেত্র NFA_BOL:
	    ক্ষেত্র NFA_BOF:
	    ক্ষেত্র NFA_BOW:
	    ক্ষেত্র NFA_EOW:
	    ক্ষেত্র NFA_ZSTART:
	    ক্ষেত্র NFA_ZEND:
	    ক্ষেত্র NFA_CURSOR:
	    ক্ষেত্র NFA_VISUAL:
	    ক্ষেত্র NFA_LNUM:
	    ক্ষেত্র NFA_LNUM_GT:
	    ক্ষেত্র NFA_LNUM_LT:
	    ক্ষেত্র NFA_COL:
	    ক্ষেত্র NFA_COL_GT:
	    ক্ষেত্র NFA_COL_LT:
	    ক্ষেত্র NFA_VCOL:
	    ক্ষেত্র NFA_VCOL_GT:
	    ক্ষেত্র NFA_VCOL_LT:
	    ক্ষেত্র NFA_MARK:
	    ক্ষেত্র NFA_MARK_GT:
	    ক্ষেত্র NFA_MARK_LT:

	    ক্ষেত্র NFA_MOPEN:
	    ক্ষেত্র NFA_MOPEN1:
	    ক্ষেত্র NFA_MOPEN2:
	    ক্ষেত্র NFA_MOPEN3:
	    ক্ষেত্র NFA_MOPEN4:
	    ক্ষেত্র NFA_MOPEN5:
	    ক্ষেত্র NFA_MOPEN6:
	    ক্ষেত্র NFA_MOPEN7:
	    ক্ষেত্র NFA_MOPEN8:
	    ক্ষেত্র NFA_MOPEN9:
	    ক্ষেত্র NFA_NOPEN:
#ifdef FEAT_SYN_HL
	    ক্ষেত্র NFA_ZOPEN:
	    ক্ষেত্র NFA_ZOPEN1:
	    ক্ষেত্র NFA_ZOPEN2:
	    ক্ষেত্র NFA_ZOPEN3:
	    ক্ষেত্র NFA_ZOPEN4:
	    ক্ষেত্র NFA_ZOPEN5:
	    ক্ষেত্র NFA_ZOPEN6:
	    ক্ষেত্র NFA_ZOPEN7:
	    ক্ষেত্র NFA_ZOPEN8:
	    ক্ষেত্র NFA_ZOPEN9:
#endif
		p = p->out;
		বিরতি;

	    ক্ষেত্র NFA_SPLIT:
	    {
		পূর্ণসংখ্যা c1 = nfa_get_regstart(p->out, depth + 1);
		পূর্ণসংখ্যা c2 = nfa_get_regstart(p->out1, depth + 1);

		যদি (c1 == c2)
		    ফেরত c1; // yes!
		ফেরত 0;
	    }

	    পূর্বনির্ধারিত:
		যদি (p->c > 0)
		    ফেরত p->c; // yes!
		ফেরত 0;
	}
    }
    ফেরত 0;
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
    পূর্ণসংখ্যা		len = 0;
    char_u	*ret;
    char_u	*s;

    যদি (p->c != NFA_MOPEN)
	ফেরত ফাঁকা; // just in case
    p = p->out;
    যখন (p->c > 0)
    {
	len += MB_CHAR2LEN(p->c);
	p = p->out;
    }
    যদি (p->c != NFA_MCLOSE || p->out->c != NFA_MATCH)
	ফেরত ফাঁকা;

    ret = alloc(len);
    যদি (ret == ফাঁকা)
	ফেরত ফাঁকা;

    p = start->out->out; // skip first char, it goes into regstart
    s = ret;
    যখন (p->c > 0)
    {
	যদি (has_mbyte)
	    s += (*mb_char2bytes)(p->c, s);
	অন্যথা
	    *s++ = p->c;
	p = p->out;
    }
    *s = NUL;
    ফেরত ret;
}

/*
 * Allocate more space for post_start.  Called when
 * running above the estimated number of states.
 */
    স্থির পূর্ণসংখ্যা
realloc_post_list(শূন্য)
{
    পূর্ণসংখ্যা   nstate_max = (পূর্ণসংখ্যা)(post_end - post_start);
    পূর্ণসংখ্যা   new_max;
    পূর্ণসংখ্যা   *new_start;
    পূর্ণসংখ্যা	  *old_start;

    // For weird patterns the number of states can be very high. Increasing by
    // 50% seems a reasonable compromise between memory use and speed.
    new_max = nstate_max * 3 / 2;
    new_start = ALLOC_MULT(পূর্ণসংখ্যা, new_max);
    যদি (new_start == ফাঁকা)
	ফেরত FAIL;
    mch_memmove(new_start, post_start, nstate_max * আকার(পূর্ণসংখ্যা));
    old_start = post_start;
    post_start = new_start;
    post_start_len = new_max;
    post_ptr = new_start + (post_ptr - old_start);
    post_end = post_start + new_max;
    vim_free(old_start);
    ফেরত OK;
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
    স্থির পূর্ণসংখ্যা
nfa_recognize_char_class(char_u *start, char_u *end, পূর্ণসংখ্যা extra_newl)
{
    পূর্ণসংখ্যা		newl;
    পূর্ণসংখ্যা		config = get_char_class_bits(start, end, &newl);

    যদি (config < 0)
	ফেরত FAIL;

    যদি (extra_newl == TRUE)
	newl = TRUE;

    যদি (newl == TRUE)
	extra_newl = NFA_ADD_NL;

    নির্বাচন (config)
    {
	ক্ষেত্র CLASS_o9:
	    ফেরত extra_newl + NFA_DIGIT;
	ক্ষেত্র CLASS_not |  CLASS_o9:
	    ফেরত extra_newl + NFA_NDIGIT;
	ক্ষেত্র CLASS_af | CLASS_AF | CLASS_o9:
	    ফেরত extra_newl + NFA_HEX;
	ক্ষেত্র CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    ফেরত extra_newl + NFA_NHEX;
	ক্ষেত্র CLASS_o7:
	    ফেরত extra_newl + NFA_OCTAL;
	ক্ষেত্র CLASS_not | CLASS_o7:
	    ফেরত extra_newl + NFA_NOCTAL;
	ক্ষেত্র CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    ফেরত extra_newl + NFA_WORD;
	ক্ষেত্র CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    ফেরত extra_newl + NFA_NWORD;
	ক্ষেত্র CLASS_az | CLASS_AZ | CLASS_underscore:
	    ফেরত extra_newl + NFA_শীর্ষ;
	ক্ষেত্র CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    ফেরত extra_newl + NFA_NHEAD;
	ক্ষেত্র CLASS_az | CLASS_AZ:
	    ফেরত extra_newl + NFA_ALPHA;
	ক্ষেত্র CLASS_not | CLASS_az | CLASS_AZ:
	    ফেরত extra_newl + NFA_NALPHA;
	ক্ষেত্র CLASS_az:
	   ফেরত extra_newl + NFA_LOWER_IC;
	ক্ষেত্র CLASS_not | CLASS_az:
	    ফেরত extra_newl + NFA_NLOWER_IC;
	ক্ষেত্র CLASS_AZ:
	    ফেরত extra_newl + NFA_UPPER_IC;
	ক্ষেত্র CLASS_not | CLASS_AZ:
	    ফেরত extra_newl + NFA_NUPPER_IC;
    }
    ফেরত FAIL;
}

/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * Emits bytes in postfix notation: 'a,b,NFA_OR,c,NFA_OR' is
 * equivalent to 'a OR b OR c'
 *
 * NOTE! When changing this function, also update reg_equi_class()
 */
    স্থির পূর্ণসংখ্যা
nfa_emit_equi_class(পূর্ণসংখ্যা c)
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
	নির্বাচন (c)
	{
	    ক্ষেত্র 'A': ক্ষেত্র A_grave: ক্ষেত্র A_acute: ক্ষেত্র A_circumflex:
	    ক্ষেত্র A_virguilla: ক্ষেত্র A_diaeresis: ক্ষেত্র A_ring:
	    ক্ষেত্র 0x100: ক্ষেত্র 0x102: ক্ষেত্র 0x104: ক্ষেত্র 0x1cd:
	    ক্ষেত্র 0x1de: ক্ষেত্র 0x1e0: ক্ষেত্র 0x1fa: ক্ষেত্র 0x200:
	    ক্ষেত্র 0x202: ক্ষেত্র 0x226: ক্ষেত্র 0x23a: ক্ষেত্র 0x1e00:
	    ক্ষেত্র 0x1ea0: ক্ষেত্র 0x1ea2: ক্ষেত্র 0x1ea4: ক্ষেত্র 0x1ea6:
	    ক্ষেত্র 0x1ea8: ক্ষেত্র 0x1eaa: ক্ষেত্র 0x1eac: ক্ষেত্র 0x1eae:
	    ক্ষেত্র 0x1eb0: ক্ষেত্র 0x1eb2: ক্ষেত্র 0x1eb4: ক্ষেত্র 0x1eb6:
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
		    ফেরত OK;

	    ক্ষেত্র 'B': ক্ষেত্র 0x181: ক্ষেত্র 0x243: ক্ষেত্র 0x1e02:
	    ক্ষেত্র 0x1e04: ক্ষেত্র 0x1e06:
		    EMIT2('B')
		    EMIT2(0x181) EMIT2(0x243) EMIT2(0x1e02)
		    EMIT2(0x1e04) EMIT2(0x1e06)
		    ফেরত OK;

	    ক্ষেত্র 'C': ক্ষেত্র C_cedilla: ক্ষেত্র 0x106: ক্ষেত্র 0x108:
	    ক্ষেত্র 0x10a: ক্ষেত্র 0x10c: ক্ষেত্র 0x187: ক্ষেত্র 0x23b:
	    ক্ষেত্র 0x1e08: ক্ষেত্র 0xa792:
		    EMIT2('C') EMIT2(C_cedilla)
		    EMIT2(0x106) EMIT2(0x108) EMIT2(0x10a)
		    EMIT2(0x10c) EMIT2(0x187) EMIT2(0x23b)
		    EMIT2(0x1e08) EMIT2(0xa792)
		    ফেরত OK;

	    ক্ষেত্র 'D': ক্ষেত্র 0x10e: ক্ষেত্র 0x110: ক্ষেত্র 0x18a:
	    ক্ষেত্র 0x1e0a: ক্ষেত্র 0x1e0c: ক্ষেত্র 0x1e0e: ক্ষেত্র 0x1e10:
	    ক্ষেত্র 0x1e12:
		    EMIT2('D') EMIT2(0x10e) EMIT2(0x110) EMIT2(0x18a)
		    EMIT2(0x1e0a) EMIT2(0x1e0c) EMIT2(0x1e0e)
		    EMIT2(0x1e10) EMIT2(0x1e12)
		    ফেরত OK;

	    ক্ষেত্র 'E': ক্ষেত্র E_grave: ক্ষেত্র E_acute: ক্ষেত্র E_circumflex:
	    ক্ষেত্র E_diaeresis: ক্ষেত্র 0x112: ক্ষেত্র 0x114: ক্ষেত্র 0x116:
	    ক্ষেত্র 0x118: ক্ষেত্র 0x11a: ক্ষেত্র 0x204: ক্ষেত্র 0x206:
	    ক্ষেত্র 0x228: ক্ষেত্র 0x246: ক্ষেত্র 0x1e14: ক্ষেত্র 0x1e16:
	    ক্ষেত্র 0x1e18: ক্ষেত্র 0x1e1a: ক্ষেত্র 0x1e1c: ক্ষেত্র 0x1eb8:
	    ক্ষেত্র 0x1eba: ক্ষেত্র 0x1ebc: ক্ষেত্র 0x1ebe: ক্ষেত্র 0x1ec0:
	    ক্ষেত্র 0x1ec2: ক্ষেত্র 0x1ec4: ক্ষেত্র 0x1ec6:
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
		    ফেরত OK;

	    ক্ষেত্র 'F': ক্ষেত্র 0x191: ক্ষেত্র 0x1e1e: ক্ষেত্র 0xa798:
		    EMIT2('F') EMIT2(0x191) EMIT2(0x1e1e) EMIT2(0xa798)
		    ফেরত OK;

	    ক্ষেত্র 'G': ক্ষেত্র 0x11c: ক্ষেত্র 0x11e: ক্ষেত্র 0x120:
	    ক্ষেত্র 0x122: ক্ষেত্র 0x193: ক্ষেত্র 0x1e4: ক্ষেত্র 0x1e6:
	    ক্ষেত্র 0x1f4: ক্ষেত্র 0x1e20: ক্ষেত্র 0xa7a0:
		    EMIT2('G') EMIT2(0x11c) EMIT2(0x11e) EMIT2(0x120)
		    EMIT2(0x122) EMIT2(0x193) EMIT2(0x1e4)
		    EMIT2(0x1e6) EMIT2(0x1f4) EMIT2(0x1e20)
		    EMIT2(0xa7a0)
		    ফেরত OK;

	    ক্ষেত্র 'H': ক্ষেত্র 0x124: ক্ষেত্র 0x126: ক্ষেত্র 0x21e:
	    ক্ষেত্র 0x1e22: ক্ষেত্র 0x1e24: ক্ষেত্র 0x1e26: ক্ষেত্র 0x1e28:
	    ক্ষেত্র 0x1e2a: ক্ষেত্র 0x2c67:
		    EMIT2('H') EMIT2(0x124) EMIT2(0x126) EMIT2(0x21e)
		    EMIT2(0x1e22) EMIT2(0x1e24) EMIT2(0x1e26)
		    EMIT2(0x1e28) EMIT2(0x1e2a) EMIT2(0x2c67)
		    ফেরত OK;

	    ক্ষেত্র 'I': ক্ষেত্র I_grave: ক্ষেত্র I_acute: ক্ষেত্র I_circumflex:
	    ক্ষেত্র I_diaeresis: ক্ষেত্র 0x128: ক্ষেত্র 0x12a: ক্ষেত্র 0x12c:
	    ক্ষেত্র 0x12e: ক্ষেত্র 0x130: ক্ষেত্র 0x197: ক্ষেত্র 0x1cf:
	    ক্ষেত্র 0x208: ক্ষেত্র 0x20a: ক্ষেত্র 0x1e2c: ক্ষেত্র 0x1e2e:
	    ক্ষেত্র 0x1ec8: ক্ষেত্র 0x1eca:
		    EMIT2('I') EMIT2(I_grave) EMIT2(I_acute)
		    EMIT2(I_circumflex) EMIT2(I_diaeresis)
		    EMIT2(0x128) EMIT2(0x12a) EMIT2(0x12c)
		    EMIT2(0x12e) EMIT2(0x130) EMIT2(0x197)
		    EMIT2(0x1cf) EMIT2(0x208) EMIT2(0x20a)
		    EMIT2(0x1e2c) EMIT2(0x1e2e) EMIT2(0x1ec8)
		    EMIT2(0x1eca)
		    ফেরত OK;

	    ক্ষেত্র 'J': ক্ষেত্র 0x134: ক্ষেত্র 0x248:
		    EMIT2('J') EMIT2(0x134) EMIT2(0x248)
		    ফেরত OK;

	    ক্ষেত্র 'K': ক্ষেত্র 0x136: ক্ষেত্র 0x198: ক্ষেত্র 0x1e8: ক্ষেত্র 0x1e30:
	    ক্ষেত্র 0x1e32: ক্ষেত্র 0x1e34: ক্ষেত্র 0x2c69: ক্ষেত্র 0xa740:
		    EMIT2('K') EMIT2(0x136) EMIT2(0x198) EMIT2(0x1e8)
		    EMIT2(0x1e30) EMIT2(0x1e32) EMIT2(0x1e34)
		    EMIT2(0x2c69) EMIT2(0xa740)
		    ফেরত OK;

	    ক্ষেত্র 'L': ক্ষেত্র 0x139: ক্ষেত্র 0x13b: ক্ষেত্র 0x13d:
	    ক্ষেত্র 0x13f: ক্ষেত্র 0x141: ক্ষেত্র 0x23d: ক্ষেত্র 0x1e36:
	    ক্ষেত্র 0x1e38: ক্ষেত্র 0x1e3a: ক্ষেত্র 0x1e3c: ক্ষেত্র 0x2c60:
		    EMIT2('L') EMIT2(0x139) EMIT2(0x13b)
		    EMIT2(0x13d) EMIT2(0x13f) EMIT2(0x141)
		    EMIT2(0x23d) EMIT2(0x1e36) EMIT2(0x1e38)
		    EMIT2(0x1e3a) EMIT2(0x1e3c) EMIT2(0x2c60)
		    ফেরত OK;

	    ক্ষেত্র 'M': ক্ষেত্র 0x1e3e: ক্ষেত্র 0x1e40: ক্ষেত্র 0x1e42:
		    EMIT2('M') EMIT2(0x1e3e) EMIT2(0x1e40)
		    EMIT2(0x1e42)
		    ফেরত OK;

	    ক্ষেত্র 'N': ক্ষেত্র N_virguilla:
	    ক্ষেত্র 0x143: ক্ষেত্র 0x145: ক্ষেত্র 0x147: ক্ষেত্র 0x1f8:
	    ক্ষেত্র 0x1e44: ক্ষেত্র 0x1e46: ক্ষেত্র 0x1e48: ক্ষেত্র 0x1e4a:
	    ক্ষেত্র 0xa7a4:
		    EMIT2('N') EMIT2(N_virguilla)
		    EMIT2(0x143) EMIT2(0x145) EMIT2(0x147)
		    EMIT2(0x1f8) EMIT2(0x1e44) EMIT2(0x1e46)
		    EMIT2(0x1e48) EMIT2(0x1e4a) EMIT2(0xa7a4)
		    ফেরত OK;

	    ক্ষেত্র 'O': ক্ষেত্র O_grave: ক্ষেত্র O_acute: ক্ষেত্র O_circumflex:
	    ক্ষেত্র O_virguilla: ক্ষেত্র O_diaeresis: ক্ষেত্র O_slash:
	    ক্ষেত্র 0x14c: ক্ষেত্র 0x14e: ক্ষেত্র 0x150: ক্ষেত্র 0x19f:
	    ক্ষেত্র 0x1a0: ক্ষেত্র 0x1d1: ক্ষেত্র 0x1ea: ক্ষেত্র 0x1ec:
	    ক্ষেত্র 0x1fe: ক্ষেত্র 0x20c: ক্ষেত্র 0x20e: ক্ষেত্র 0x22a:
	    ক্ষেত্র 0x22c: ক্ষেত্র 0x22e: ক্ষেত্র 0x230: ক্ষেত্র 0x1e4c:
	    ক্ষেত্র 0x1e4e: ক্ষেত্র 0x1e50: ক্ষেত্র 0x1e52: ক্ষেত্র 0x1ecc:
	    ক্ষেত্র 0x1ece: ক্ষেত্র 0x1ed0: ক্ষেত্র 0x1ed2: ক্ষেত্র 0x1ed4:
	    ক্ষেত্র 0x1ed6: ক্ষেত্র 0x1ed8: ক্ষেত্র 0x1eda: ক্ষেত্র 0x1edc:
	    ক্ষেত্র 0x1ede: ক্ষেত্র 0x1ee0: ক্ষেত্র 0x1ee2:
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
		    ফেরত OK;

	    ক্ষেত্র 'P': ক্ষেত্র 0x1a4: ক্ষেত্র 0x1e54: ক্ষেত্র 0x1e56: ক্ষেত্র 0x2c63:
		    EMIT2('P') EMIT2(0x1a4) EMIT2(0x1e54) EMIT2(0x1e56)
		    EMIT2(0x2c63)
		    ফেরত OK;

	    ক্ষেত্র 'Q': ক্ষেত্র 0x24a:
		    EMIT2('Q') EMIT2(0x24a)
		    ফেরত OK;

	    ক্ষেত্র 'R': ক্ষেত্র 0x154: ক্ষেত্র 0x156: ক্ষেত্র 0x158: ক্ষেত্র 0x210:
	    ক্ষেত্র 0x212: ক্ষেত্র 0x24c: ক্ষেত্র 0x1e58: ক্ষেত্র 0x1e5a:
	    ক্ষেত্র 0x1e5c: ক্ষেত্র 0x1e5e: ক্ষেত্র 0x2c64: ক্ষেত্র 0xa7a6:
		    EMIT2('R') EMIT2(0x154) EMIT2(0x156) EMIT2(0x158)
		    EMIT2(0x210) EMIT2(0x212) EMIT2(0x24c) EMIT2(0x1e58)
		    EMIT2(0x1e5a) EMIT2(0x1e5c) EMIT2(0x1e5e) EMIT2(0x2c64)
		    EMIT2(0xa7a6)
		    ফেরত OK;

	    ক্ষেত্র 'S': ক্ষেত্র 0x15a: ক্ষেত্র 0x15c: ক্ষেত্র 0x15e: ক্ষেত্র 0x160:
	    ক্ষেত্র 0x218: ক্ষেত্র 0x1e60: ক্ষেত্র 0x1e62: ক্ষেত্র 0x1e64:
	    ক্ষেত্র 0x1e66: ক্ষেত্র 0x1e68: ক্ষেত্র 0x2c7e: ক্ষেত্র 0xa7a8:
		    EMIT2('S') EMIT2(0x15a) EMIT2(0x15c) EMIT2(0x15e)
		    EMIT2(0x160) EMIT2(0x218) EMIT2(0x1e60) EMIT2(0x1e62)
		    EMIT2(0x1e64) EMIT2(0x1e66) EMIT2(0x1e68) EMIT2(0x2c7e)
		    EMIT2(0xa7a8)
		    ফেরত OK;

	    ক্ষেত্র 'T': ক্ষেত্র 0x162: ক্ষেত্র 0x164: ক্ষেত্র 0x166: ক্ষেত্র 0x1ac:
	    ক্ষেত্র 0x1ae: ক্ষেত্র 0x21a: ক্ষেত্র 0x23e: ক্ষেত্র 0x1e6a: ক্ষেত্র 0x1e6c:
	    ক্ষেত্র 0x1e6e: ক্ষেত্র 0x1e70:
		    EMIT2('T') EMIT2(0x162) EMIT2(0x164) EMIT2(0x166)
		    EMIT2(0x1ac) EMIT2(0x1ae) EMIT2(0x23e) EMIT2(0x21a)
		    EMIT2(0x1e6a) EMIT2(0x1e6c) EMIT2(0x1e6e) EMIT2(0x1e70)
		    ফেরত OK;

	    ক্ষেত্র 'U': ক্ষেত্র U_grave: ক্ষেত্র U_acute: ক্ষেত্র U_diaeresis:
	    ক্ষেত্র U_circumflex: ক্ষেত্র 0x168: ক্ষেত্র 0x16a: ক্ষেত্র 0x16c:
	    ক্ষেত্র 0x16e: ক্ষেত্র 0x170: ক্ষেত্র 0x172: ক্ষেত্র 0x1af:
	    ক্ষেত্র 0x1d3: ক্ষেত্র 0x1d5: ক্ষেত্র 0x1d7: ক্ষেত্র 0x1d9:
	    ক্ষেত্র 0x1db: ক্ষেত্র 0x214: ক্ষেত্র 0x216: ক্ষেত্র 0x244:
	    ক্ষেত্র 0x1e72: ক্ষেত্র 0x1e74: ক্ষেত্র 0x1e76: ক্ষেত্র 0x1e78:
	    ক্ষেত্র 0x1e7a: ক্ষেত্র 0x1ee4: ক্ষেত্র 0x1ee6: ক্ষেত্র 0x1ee8:
	    ক্ষেত্র 0x1eea: ক্ষেত্র 0x1eec: ক্ষেত্র 0x1eee: ক্ষেত্র 0x1ef0:
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
		    ফেরত OK;

	    ক্ষেত্র 'V': ক্ষেত্র 0x1b2: ক্ষেত্র 0x1e7c: ক্ষেত্র 0x1e7e:
		    EMIT2('V') EMIT2(0x1b2) EMIT2(0x1e7c) EMIT2(0x1e7e)
		    ফেরত OK;

	    ক্ষেত্র 'W': ক্ষেত্র 0x174: ক্ষেত্র 0x1e80: ক্ষেত্র 0x1e82: ক্ষেত্র 0x1e84:
	    ক্ষেত্র 0x1e86: ক্ষেত্র 0x1e88:
		    EMIT2('W') EMIT2(0x174) EMIT2(0x1e80) EMIT2(0x1e82)
		    EMIT2(0x1e84) EMIT2(0x1e86) EMIT2(0x1e88)
		    ফেরত OK;

	    ক্ষেত্র 'X': ক্ষেত্র 0x1e8a: ক্ষেত্র 0x1e8c:
		    EMIT2('X') EMIT2(0x1e8a) EMIT2(0x1e8c)
		    ফেরত OK;

	    ক্ষেত্র 'Y': ক্ষেত্র Y_acute: ক্ষেত্র 0x176: ক্ষেত্র 0x178:
	    ক্ষেত্র 0x1b3: ক্ষেত্র 0x232: ক্ষেত্র 0x24e: ক্ষেত্র 0x1e8e:
	    ক্ষেত্র 0x1ef2: ক্ষেত্র 0x1ef4: ক্ষেত্র 0x1ef6: ক্ষেত্র 0x1ef8:
		    EMIT2('Y') EMIT2(Y_acute)
		    EMIT2(0x176) EMIT2(0x178) EMIT2(0x1b3)
		    EMIT2(0x232) EMIT2(0x24e) EMIT2(0x1e8e)
		    EMIT2(0x1ef2) EMIT2(0x1ef4) EMIT2(0x1ef6)
		    EMIT2(0x1ef8)
		    ফেরত OK;

	    ক্ষেত্র 'Z': ক্ষেত্র 0x179: ক্ষেত্র 0x17b: ক্ষেত্র 0x17d:
	    ক্ষেত্র 0x1b5: ক্ষেত্র 0x1e90: ক্ষেত্র 0x1e92: ক্ষেত্র 0x1e94:
	    ক্ষেত্র 0x2c6b:
		    EMIT2('Z') EMIT2(0x179) EMIT2(0x17b) EMIT2(0x17d)
		    EMIT2(0x1b5) EMIT2(0x1e90) EMIT2(0x1e92)
		    EMIT2(0x1e94) EMIT2(0x2c6b)
		    ফেরত OK;

	    ক্ষেত্র 'a': ক্ষেত্র a_grave: ক্ষেত্র a_acute: ক্ষেত্র a_circumflex:
	    ক্ষেত্র a_virguilla: ক্ষেত্র a_diaeresis: ক্ষেত্র a_ring:
	    ক্ষেত্র 0x101: ক্ষেত্র 0x103: ক্ষেত্র 0x105: ক্ষেত্র 0x1ce:
	    ক্ষেত্র 0x1df: ক্ষেত্র 0x1e1: ক্ষেত্র 0x1fb: ক্ষেত্র 0x201:
	    ক্ষেত্র 0x203: ক্ষেত্র 0x227: ক্ষেত্র 0x1d8f: ক্ষেত্র 0x1e01:
	    ক্ষেত্র 0x1e9a: ক্ষেত্র 0x1ea1: ক্ষেত্র 0x1ea3: ক্ষেত্র 0x1ea5:
	    ক্ষেত্র 0x1ea7: ক্ষেত্র 0x1ea9: ক্ষেত্র 0x1eab: ক্ষেত্র 0x1ead:
	    ক্ষেত্র 0x1eaf: ক্ষেত্র 0x1eb1: ক্ষেত্র 0x1eb3: ক্ষেত্র 0x1eb5:
	    ক্ষেত্র 0x1eb7: ক্ষেত্র 0x2c65:
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
		    ফেরত OK;

	    ক্ষেত্র 'b': ক্ষেত্র 0x180: ক্ষেত্র 0x253: ক্ষেত্র 0x1d6c: ক্ষেত্র 0x1d80:
	    ক্ষেত্র 0x1e03: ক্ষেত্র 0x1e05: ক্ষেত্র 0x1e07:
		    EMIT2('b') EMIT2(0x180) EMIT2(0x253) EMIT2(0x1d6c)
		    EMIT2(0x1d80) EMIT2(0x1e03) EMIT2(0x1e05) EMIT2(0x1e07)
		    ফেরত OK;

	    ক্ষেত্র 'c': ক্ষেত্র c_cedilla: ক্ষেত্র 0x107: ক্ষেত্র 0x109: ক্ষেত্র 0x10b:
	    ক্ষেত্র 0x10d: ক্ষেত্র 0x188: ক্ষেত্র 0x23c: ক্ষেত্র 0x1e09: ক্ষেত্র 0xa793:
	    ক্ষেত্র 0xa794:
		    EMIT2('c') EMIT2(c_cedilla)
		    EMIT2(0x107) EMIT2(0x109) EMIT2(0x10b)
		    EMIT2(0x10d) EMIT2(0x188) EMIT2(0x23c)
		    EMIT2(0x1e09) EMIT2(0xa793) EMIT2(0xa794)
		    ফেরত OK;

	    ক্ষেত্র 'd': ক্ষেত্র 0x10f: ক্ষেত্র 0x111: ক্ষেত্র 0x257: ক্ষেত্র 0x1d6d:
	    ক্ষেত্র 0x1d81: ক্ষেত্র 0x1d91: ক্ষেত্র 0x1e0b: ক্ষেত্র 0x1e0d: ক্ষেত্র 0x1e0f:
	    ক্ষেত্র 0x1e11: ক্ষেত্র 0x1e13:
		    EMIT2('d') EMIT2(0x10f) EMIT2(0x111)
		    EMIT2(0x257) EMIT2(0x1d6d) EMIT2(0x1d81)
		    EMIT2(0x1d91) EMIT2(0x1e0b) EMIT2(0x1e0d)
		    EMIT2(0x1e0f) EMIT2(0x1e11) EMIT2(0x1e13)
		    ফেরত OK;

	    ক্ষেত্র 'e': ক্ষেত্র e_grave: ক্ষেত্র e_acute: ক্ষেত্র e_circumflex:
	    ক্ষেত্র e_diaeresis: ক্ষেত্র 0x113: ক্ষেত্র 0x115: ক্ষেত্র 0x117:
	    ক্ষেত্র 0x119: ক্ষেত্র 0x11b: ক্ষেত্র 0x205: ক্ষেত্র 0x207:
	    ক্ষেত্র 0x229: ক্ষেত্র 0x247: ক্ষেত্র 0x1d92: ক্ষেত্র 0x1e15:
	    ক্ষেত্র 0x1e17: ক্ষেত্র 0x1e19: ক্ষেত্র 0x1e1b: ক্ষেত্র 0x1e1d:
	    ক্ষেত্র 0x1eb9: ক্ষেত্র 0x1ebb: ক্ষেত্র 0x1ebd: ক্ষেত্র 0x1ebf:
	    ক্ষেত্র 0x1ec1: ক্ষেত্র 0x1ec3: ক্ষেত্র 0x1ec5: ক্ষেত্র 0x1ec7:
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
		    ফেরত OK;

	    ক্ষেত্র 'f': ক্ষেত্র 0x192: ক্ষেত্র 0x1d6e: ক্ষেত্র 0x1d82:
	    ক্ষেত্র 0x1e1f: ক্ষেত্র 0xa799:
		    EMIT2('f') EMIT2(0x192) EMIT2(0x1d6e) EMIT2(0x1d82)
		    EMIT2(0x1e1f) EMIT2(0xa799)
		    ফেরত OK;

	    ক্ষেত্র 'g': ক্ষেত্র 0x11d: ক্ষেত্র 0x11f: ক্ষেত্র 0x121: ক্ষেত্র 0x123:
	    ক্ষেত্র 0x1e5: ক্ষেত্র 0x1e7: ক্ষেত্র 0x1f5: ক্ষেত্র 0x260: ক্ষেত্র 0x1d83:
	    ক্ষেত্র 0x1e21: ক্ষেত্র 0xa7a1:
		    EMIT2('g') EMIT2(0x11d) EMIT2(0x11f) EMIT2(0x121)
		    EMIT2(0x123) EMIT2(0x1e5) EMIT2(0x1e7)
		    EMIT2(0x1f5) EMIT2(0x260) EMIT2(0x1d83)
		    EMIT2(0x1e21) EMIT2(0xa7a1)
		    ফেরত OK;

	    ক্ষেত্র 'h': ক্ষেত্র 0x125: ক্ষেত্র 0x127: ক্ষেত্র 0x21f: ক্ষেত্র 0x1e23:
	    ক্ষেত্র 0x1e25: ক্ষেত্র 0x1e27: ক্ষেত্র 0x1e29: ক্ষেত্র 0x1e2b:
	    ক্ষেত্র 0x1e96: ক্ষেত্র 0x2c68: ক্ষেত্র 0xa795:
		    EMIT2('h') EMIT2(0x125) EMIT2(0x127) EMIT2(0x21f)
		    EMIT2(0x1e23) EMIT2(0x1e25) EMIT2(0x1e27)
		    EMIT2(0x1e29) EMIT2(0x1e2b) EMIT2(0x1e96)
		    EMIT2(0x2c68) EMIT2(0xa795)
		    ফেরত OK;

	    ক্ষেত্র 'i': ক্ষেত্র i_grave: ক্ষেত্র i_acute: ক্ষেত্র i_circumflex:
	    ক্ষেত্র i_diaeresis: ক্ষেত্র 0x129: ক্ষেত্র 0x12b: ক্ষেত্র 0x12d:
	    ক্ষেত্র 0x12f: ক্ষেত্র 0x1d0: ক্ষেত্র 0x209: ক্ষেত্র 0x20b:
	    ক্ষেত্র 0x268: ক্ষেত্র 0x1d96: ক্ষেত্র 0x1e2d: ক্ষেত্র 0x1e2f:
	    ক্ষেত্র 0x1ec9: ক্ষেত্র 0x1ecb:
		    EMIT2('i') EMIT2(i_grave) EMIT2(i_acute)
		    EMIT2(i_circumflex) EMIT2(i_diaeresis)
		    EMIT2(0x129) EMIT2(0x12b) EMIT2(0x12d)
		    EMIT2(0x12f) EMIT2(0x1d0) EMIT2(0x209)
		    EMIT2(0x20b) EMIT2(0x268) EMIT2(0x1d96)
		    EMIT2(0x1e2d) EMIT2(0x1e2f) EMIT2(0x1ec9)
		    EMIT2(0x1ecb)
		    ফেরত OK;

	    ক্ষেত্র 'j': ক্ষেত্র 0x135: ক্ষেত্র 0x1f0: ক্ষেত্র 0x249:
		    EMIT2('j') EMIT2(0x135) EMIT2(0x1f0) EMIT2(0x249)
		    ফেরত OK;

	    ক্ষেত্র 'k': ক্ষেত্র 0x137: ক্ষেত্র 0x199: ক্ষেত্র 0x1e9: ক্ষেত্র 0x1d84:
	    ক্ষেত্র 0x1e31: ক্ষেত্র 0x1e33: ক্ষেত্র 0x1e35: ক্ষেত্র 0x2c6a: ক্ষেত্র 0xa741:
		    EMIT2('k') EMIT2(0x137) EMIT2(0x199) EMIT2(0x1e9)
		    EMIT2(0x1d84) EMIT2(0x1e31) EMIT2(0x1e33)
		    EMIT2(0x1e35) EMIT2(0x2c6a) EMIT2(0xa741)
		    ফেরত OK;

	    ক্ষেত্র 'l': ক্ষেত্র 0x13a: ক্ষেত্র 0x13c: ক্ষেত্র 0x13e: ক্ষেত্র 0x140:
	    ক্ষেত্র 0x142: ক্ষেত্র 0x19a: ক্ষেত্র 0x1e37: ক্ষেত্র 0x1e39: ক্ষেত্র 0x1e3b:
	    ক্ষেত্র 0x1e3d: ক্ষেত্র 0x2c61:
		    EMIT2('l') EMIT2(0x13a) EMIT2(0x13c)
		    EMIT2(0x13e) EMIT2(0x140) EMIT2(0x142)
		    EMIT2(0x19a) EMIT2(0x1e37) EMIT2(0x1e39)
		    EMIT2(0x1e3b) EMIT2(0x1e3d) EMIT2(0x2c61)
		    ফেরত OK;

	    ক্ষেত্র 'm': ক্ষেত্র 0x1d6f: ক্ষেত্র 0x1e3f: ক্ষেত্র 0x1e41: ক্ষেত্র 0x1e43:
		    EMIT2('m') EMIT2(0x1d6f) EMIT2(0x1e3f)
		    EMIT2(0x1e41) EMIT2(0x1e43)
		    ফেরত OK;

	    ক্ষেত্র 'n': ক্ষেত্র n_virguilla: ক্ষেত্র 0x144: ক্ষেত্র 0x146: ক্ষেত্র 0x148:
	    ক্ষেত্র 0x149: ক্ষেত্র 0x1f9: ক্ষেত্র 0x1d70: ক্ষেত্র 0x1d87: ক্ষেত্র 0x1e45:
	    ক্ষেত্র 0x1e47: ক্ষেত্র 0x1e49: ক্ষেত্র 0x1e4b: ক্ষেত্র 0xa7a5:
		    EMIT2('n') EMIT2(n_virguilla)
		    EMIT2(0x144) EMIT2(0x146) EMIT2(0x148)
		    EMIT2(0x149) EMIT2(0x1f9) EMIT2(0x1d70)
		    EMIT2(0x1d87) EMIT2(0x1e45) EMIT2(0x1e47)
		    EMIT2(0x1e49) EMIT2(0x1e4b) EMIT2(0xa7a5)
		    ফেরত OK;

	    ক্ষেত্র 'o': ক্ষেত্র o_grave: ক্ষেত্র o_acute: ক্ষেত্র o_circumflex:
	    ক্ষেত্র o_virguilla: ক্ষেত্র o_diaeresis: ক্ষেত্র o_slash:
	    ক্ষেত্র 0x14d: ক্ষেত্র 0x14f: ক্ষেত্র 0x151: ক্ষেত্র 0x1a1:
	    ক্ষেত্র 0x1d2: ক্ষেত্র 0x1eb: ক্ষেত্র 0x1ed: ক্ষেত্র 0x1ff:
	    ক্ষেত্র 0x20d: ক্ষেত্র 0x20f: ক্ষেত্র 0x22b: ক্ষেত্র 0x22d:
	    ক্ষেত্র 0x22f: ক্ষেত্র 0x231: ক্ষেত্র 0x275: ক্ষেত্র 0x1e4d:
	    ক্ষেত্র 0x1e4f: ক্ষেত্র 0x1e51: ক্ষেত্র 0x1e53: ক্ষেত্র 0x1ecd:
	    ক্ষেত্র 0x1ecf: ক্ষেত্র 0x1ed1: ক্ষেত্র 0x1ed3: ক্ষেত্র 0x1ed5:
	    ক্ষেত্র 0x1ed7: ক্ষেত্র 0x1ed9: ক্ষেত্র 0x1edb: ক্ষেত্র 0x1edd:
	    ক্ষেত্র 0x1edf: ক্ষেত্র 0x1ee1: ক্ষেত্র 0x1ee3:
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
		    ফেরত OK;

	    ক্ষেত্র 'p': ক্ষেত্র 0x1a5: ক্ষেত্র 0x1d71: ক্ষেত্র 0x1d7d: ক্ষেত্র 0x1d88:
	    ক্ষেত্র 0x1e55: ক্ষেত্র 0x1e57:
		    EMIT2('p') EMIT2(0x1a5) EMIT2(0x1d71) EMIT2(0x1d7d)
		    EMIT2(0x1d88) EMIT2(0x1e55) EMIT2(0x1e57)
		    ফেরত OK;

	    ক্ষেত্র 'q': ক্ষেত্র 0x24b: ক্ষেত্র 0x2a0:
		    EMIT2('q') EMIT2(0x24b) EMIT2(0x2a0)
		    ফেরত OK;

	    ক্ষেত্র 'r': ক্ষেত্র 0x155: ক্ষেত্র 0x157: ক্ষেত্র 0x159: ক্ষেত্র 0x211:
	    ক্ষেত্র 0x213: ক্ষেত্র 0x24d: ক্ষেত্র 0x27d: ক্ষেত্র 0x1d72: ক্ষেত্র 0x1d73:
	    ক্ষেত্র 0x1d89: ক্ষেত্র 0x1e59: ক্ষেত্র 0x1e5b: ক্ষেত্র 0x1e5d: ক্ষেত্র 0x1e5f:
	    ক্ষেত্র 0xa7a7:
		    EMIT2('r') EMIT2(0x155) EMIT2(0x157) EMIT2(0x159)
		    EMIT2(0x211) EMIT2(0x213) EMIT2(0x24d) EMIT2(0x27d)
		    EMIT2(0x1d72) EMIT2(0x1d73) EMIT2(0x1d89) EMIT2(0x1e59)
		    EMIT2(0x1e5b) EMIT2(0x1e5d) EMIT2(0x1e5f) EMIT2(0xa7a7)
		    ফেরত OK;

	    ক্ষেত্র 's': ক্ষেত্র 0x15b: ক্ষেত্র 0x15d: ক্ষেত্র 0x15f: ক্ষেত্র 0x161:
	    ক্ষেত্র 0x219: ক্ষেত্র 0x23f: ক্ষেত্র 0x1d74: ক্ষেত্র 0x1d8a: ক্ষেত্র 0x1e61:
	    ক্ষেত্র 0x1e63: ক্ষেত্র 0x1e65: ক্ষেত্র 0x1e67: ক্ষেত্র 0x1e69: ক্ষেত্র 0xa7a9:
		    EMIT2('s') EMIT2(0x15b) EMIT2(0x15d) EMIT2(0x15f)
		    EMIT2(0x161) EMIT2(0x219) EMIT2(0x23f) EMIT2(0x1d74)
		    EMIT2(0x1d8a) EMIT2(0x1e61) EMIT2(0x1e63) EMIT2(0x1e65)
		    EMIT2(0x1e67) EMIT2(0x1e69) EMIT2(0xa7a9)
		    ফেরত OK;

	    ক্ষেত্র 't': ক্ষেত্র 0x163: ক্ষেত্র 0x165: ক্ষেত্র 0x167: ক্ষেত্র 0x1ab:
	    ক্ষেত্র 0x1ad: ক্ষেত্র 0x21b: ক্ষেত্র 0x288: ক্ষেত্র 0x1d75: ক্ষেত্র 0x1e6b:
	    ক্ষেত্র 0x1e6d: ক্ষেত্র 0x1e6f: ক্ষেত্র 0x1e71: ক্ষেত্র 0x1e97: ক্ষেত্র 0x2c66:
		    EMIT2('t') EMIT2(0x163) EMIT2(0x165) EMIT2(0x167)
		    EMIT2(0x1ab) EMIT2(0x1ad) EMIT2(0x21b) EMIT2(0x288)
		    EMIT2(0x1d75) EMIT2(0x1e6b) EMIT2(0x1e6d) EMIT2(0x1e6f)
		    EMIT2(0x1e71) EMIT2(0x1e97) EMIT2(0x2c66)
		    ফেরত OK;

	    ক্ষেত্র 'u': ক্ষেত্র u_grave: ক্ষেত্র u_acute: ক্ষেত্র u_circumflex:
	    ক্ষেত্র u_diaeresis: ক্ষেত্র 0x169: ক্ষেত্র 0x16b: ক্ষেত্র 0x16d:
	    ক্ষেত্র 0x16f: ক্ষেত্র 0x171: ক্ষেত্র 0x173: ক্ষেত্র 0x1b0: ক্ষেত্র 0x1d4:
	    ক্ষেত্র 0x1d6: ক্ষেত্র 0x1d8: ক্ষেত্র 0x1da: ক্ষেত্র 0x1dc: ক্ষেত্র 0x215:
	    ক্ষেত্র 0x217: ক্ষেত্র 0x289: ক্ষেত্র 0x1d7e: ক্ষেত্র 0x1d99: ক্ষেত্র 0x1e73:
	    ক্ষেত্র 0x1e75: ক্ষেত্র 0x1e77: ক্ষেত্র 0x1e79: ক্ষেত্র 0x1e7b:
	    ক্ষেত্র 0x1ee5: ক্ষেত্র 0x1ee7: ক্ষেত্র 0x1ee9: ক্ষেত্র 0x1eeb:
	    ক্ষেত্র 0x1eed: ক্ষেত্র 0x1eef: ক্ষেত্র 0x1ef1:
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
		    ফেরত OK;

	    ক্ষেত্র 'v': ক্ষেত্র 0x28b: ক্ষেত্র 0x1d8c: ক্ষেত্র 0x1e7d: ক্ষেত্র 0x1e7f:
		    EMIT2('v') EMIT2(0x28b) EMIT2(0x1d8c) EMIT2(0x1e7d)
		    EMIT2(0x1e7f)
		    ফেরত OK;

	    ক্ষেত্র 'w': ক্ষেত্র 0x175: ক্ষেত্র 0x1e81: ক্ষেত্র 0x1e83: ক্ষেত্র 0x1e85:
	    ক্ষেত্র 0x1e87: ক্ষেত্র 0x1e89: ক্ষেত্র 0x1e98:
		    EMIT2('w') EMIT2(0x175) EMIT2(0x1e81) EMIT2(0x1e83)
		    EMIT2(0x1e85) EMIT2(0x1e87) EMIT2(0x1e89) EMIT2(0x1e98)
		    ফেরত OK;

	    ক্ষেত্র 'x': ক্ষেত্র 0x1e8b: ক্ষেত্র 0x1e8d:
		    EMIT2('x') EMIT2(0x1e8b) EMIT2(0x1e8d)
		    ফেরত OK;

	    ক্ষেত্র 'y': ক্ষেত্র y_acute: ক্ষেত্র y_diaeresis: ক্ষেত্র 0x177:
	    ক্ষেত্র 0x1b4: ক্ষেত্র 0x233: ক্ষেত্র 0x24f: ক্ষেত্র 0x1e8f:
	    ক্ষেত্র 0x1e99: ক্ষেত্র 0x1ef3: ক্ষেত্র 0x1ef5: ক্ষেত্র 0x1ef7:
	    ক্ষেত্র 0x1ef9:
		    EMIT2('y') EMIT2(y_acute) EMIT2(y_diaeresis)
		    EMIT2(0x177) EMIT2(0x1b4) EMIT2(0x233) EMIT2(0x24f)
		    EMIT2(0x1e8f) EMIT2(0x1e99) EMIT2(0x1ef3)
		    EMIT2(0x1ef5) EMIT2(0x1ef7) EMIT2(0x1ef9)
		    ফেরত OK;

	    ক্ষেত্র 'z': ক্ষেত্র 0x17a: ক্ষেত্র 0x17c: ক্ষেত্র 0x17e: ক্ষেত্র 0x1b6:
	    ক্ষেত্র 0x1d76: ক্ষেত্র 0x1d8e: ক্ষেত্র 0x1e91: ক্ষেত্র 0x1e93:
	    ক্ষেত্র 0x1e95: ক্ষেত্র 0x2c6c:
		    EMIT2('z') EMIT2(0x17a) EMIT2(0x17c) EMIT2(0x17e)
		    EMIT2(0x1b6) EMIT2(0x1d76) EMIT2(0x1d8e) EMIT2(0x1e91)
		    EMIT2(0x1e93) EMIT2(0x1e95) EMIT2(0x2c6c)
		    ফেরত OK;

	    // default: character itself
	}
    }

    EMIT2(c);
    ফেরত OK;
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
    স্থির পূর্ণসংখ্যা
nfa_regatom(শূন্য)
{
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		charclass;
    পূর্ণসংখ্যা		equiclass;
    পূর্ণসংখ্যা		collclass;
    পূর্ণসংখ্যা		got_coll_char;
    char_u	*p;
    char_u	*endp;
    char_u	*old_regparse = regparse;
    পূর্ণসংখ্যা		extra = 0;
    পূর্ণসংখ্যা		emit_range;
    পূর্ণসংখ্যা		negated;
    পূর্ণসংখ্যা		ফলাফল;
    পূর্ণসংখ্যা		startc = -1;
    পূর্ণসংখ্যা		save_পূর্ব_at_start = পূর্ব_at_start;

    c = getchr();
    নির্বাচন (c)
    {
	ক্ষেত্র NUL:
	    EMSG_RET_FAIL(_(e_nfa_regexp_end_encountered_prematurely));

	ক্ষেত্র Magic('^'):
	    EMIT(NFA_BOL);
	    বিরতি;

	ক্ষেত্র Magic('$'):
	    EMIT(NFA_EOL);
#যদি defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#endif
	    বিরতি;

	ক্ষেত্র Magic('<'):
	    EMIT(NFA_BOW);
	    বিরতি;

	ক্ষেত্র Magic('>'):
	    EMIT(NFA_EOW);
	    বিরতি;

	ক্ষেত্র Magic('_'):
	    c = no_Magic(getchr());
	    যদি (c == NUL)
		EMSG_RET_FAIL(_(e_nfa_regexp_end_encountered_prematurely));

	    যদি (c == '^')	// "\_^" is start-of-line
	    {
		EMIT(NFA_BOL);
		বিরতি;
	    }
	    যদি (c == '$')	// "\_$" is end-of-line
	    {
		EMIT(NFA_EOL);
#যদি defined(FEAT_SYN_HL)
		had_eol = TRUE;
#endif
		বিরতি;
	    }

	    extra = NFA_ADD_NL;

	    // "\_[" is collection plus newline
	    যদি (c == '[')
		যাও collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	/*
	 * Character classes.
	 */
	ক্ষেত্র Magic('.'):
	ক্ষেত্র Magic('i'):
	ক্ষেত্র Magic('I'):
	ক্ষেত্র Magic('k'):
	ক্ষেত্র Magic('K'):
	ক্ষেত্র Magic('f'):
	ক্ষেত্র Magic('F'):
	ক্ষেত্র Magic('p'):
	ক্ষেত্র Magic('P'):
	ক্ষেত্র Magic('s'):
	ক্ষেত্র Magic('S'):
	ক্ষেত্র Magic('d'):
	ক্ষেত্র Magic('D'):
	ক্ষেত্র Magic('x'):
	ক্ষেত্র Magic('X'):
	ক্ষেত্র Magic('o'):
	ক্ষেত্র Magic('O'):
	ক্ষেত্র Magic('w'):
	ক্ষেত্র Magic('W'):
	ক্ষেত্র Magic('h'):
	ক্ষেত্র Magic('H'):
	ক্ষেত্র Magic('a'):
	ক্ষেত্র Magic('A'):
	ক্ষেত্র Magic('l'):
	ক্ষেত্র Magic('L'):
	ক্ষেত্র Magic('u'):
	ক্ষেত্র Magic('U'):
	    p = vim_strchr(classchars, no_Magic(c));
	    যদি (p == ফাঁকা)
	    {
		যদি (extra == NFA_ADD_NL)
		{
		    semsg(_(e_nfa_regexp_invalid_character_class_nr), c);
		    rc_did_emsg = TRUE;
		    ফেরত FAIL;
		}
		siemsg("Unknown character class char: %d", c);
		ফেরত FAIL;
	    }

	    // When '.' is followed by a composing char ignore the dot, so that
	    // the composing char is matched here.
	    যদি (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	    {
		old_regparse = regparse;
		c = getchr();
		যাও nfa_do_multibyte;
	    }
	    EMIT(nfa_classcodes[p - classchars]);
	    যদি (extra == NFA_ADD_NL)
	    {
		EMIT(NFA_NEWL);
		EMIT(NFA_OR);
		regflags |= RF_HASNL;
	    }
	    বিরতি;

	ক্ষেত্র Magic('n'):
	    যদি (reg_string)
		// In a string "\n" matches a newline character.
		EMIT(NL);
	    অন্যথা
	    {
		// In buffer text "\n" matches the end of a line.
		EMIT(NFA_NEWL);
		regflags |= RF_HASNL;
	    }
	    বিরতি;

	ক্ষেত্র Magic('('):
	    যদি (nfa_reg(REG_PAREN) == FAIL)
		ফেরত FAIL;	    // cascaded error
	    বিরতি;

	ক্ষেত্র Magic('|'):
	ক্ষেত্র Magic('&'):
	ক্ষেত্র Magic(')'):
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    ফেরত FAIL;

	ক্ষেত্র Magic('='):
	ক্ষেত্র Magic('?'):
	ক্ষেত্র Magic('+'):
	ক্ষেত্র Magic('@'):
	ক্ষেত্র Magic('*'):
	ক্ষেত্র Magic('{'):
	    // these should follow an atom, not form an atom
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    ফেরত FAIL;

	ক্ষেত্র Magic('~'):
	    {
		char_u	    *lp;

		// Previous substitute pattern.
		// Generated as "\%(pattern\)".
		যদি (reg_পূর্ব_sub == ফাঁকা)
		{
		    emsg(_(e_no_previous_substitute_regular_expression));
		    ফেরত FAIL;
		}
		জন্য (lp = reg_পূর্ব_sub; *lp != NUL; MB_CPTR_ADV(lp))
		{
		    EMIT(PTR2CHAR(lp));
		    যদি (lp != reg_পূর্ব_sub)
			EMIT(NFA_CONCAT);
		}
		EMIT(NFA_NOPEN);
		বিরতি;
	    }

	ক্ষেত্র Magic('1'):
	ক্ষেত্র Magic('2'):
	ক্ষেত্র Magic('3'):
	ক্ষেত্র Magic('4'):
	ক্ষেত্র Magic('5'):
	ক্ষেত্র Magic('6'):
	ক্ষেত্র Magic('7'):
	ক্ষেত্র Magic('8'):
	ক্ষেত্র Magic('9'):
	    {
		পূর্ণসংখ্যা refnum = no_Magic(c) - '1';

		যদি (!seen_endbrace(refnum + 1))
		    ফেরত FAIL;
		EMIT(NFA_BACKREF1 + refnum);
		rex.nfa_has_backref = TRUE;
	    }
	    বিরতি;

	ক্ষেত্র Magic('z'):
	    c = no_Magic(getchr());
	    নির্বাচন (c)
	    {
		ক্ষেত্র 's':
		    EMIT(NFA_ZSTART);
		    যদি (re_mult_পরবর্তী("\\zs") == FAIL)
			ফেরত FAIL;
		    বিরতি;
		ক্ষেত্র 'e':
		    EMIT(NFA_ZEND);
		    rex.nfa_has_zend = TRUE;
		    যদি (re_mult_পরবর্তী("\\ze") == FAIL)
			ফেরত FAIL;
		    বিরতি;
#ifdef FEAT_SYN_HL
		ক্ষেত্র '1':
		ক্ষেত্র '2':
		ক্ষেত্র '3':
		ক্ষেত্র '4':
		ক্ষেত্র '5':
		ক্ষেত্র '6':
		ক্ষেত্র '7':
		ক্ষেত্র '8':
		ক্ষেত্র '9':
		    // \z1...\z9
		    যদি ((reg_do_extmatch & REX_USE) == 0)
			EMSG_RET_FAIL(_(e_z1_z9_not_allowed_here));
		    EMIT(NFA_ZREF1 + (no_Magic(c) - '1'));
		    // No need to set rex.nfa_has_backref, the sub-matches don't
		    // change when \z1 .. \z9 matches or not.
		    re_has_z = REX_USE;
		    বিরতি;
		ক্ষেত্র '(':
		    // \z(
		    যদি ((reg_do_extmatch & REX_SET) == 0)
			EMSG_RET_FAIL(_(e_z_not_allowed_here));
		    যদি (nfa_reg(REG_ZPAREN) == FAIL)
			ফেরত FAIL;	    // cascaded error
		    re_has_z = REX_SET;
		    বিরতি;
#endif
		পূর্বনির্ধারিত:
		    semsg(_(e_nfa_regexp_unknown_operator_z_chr), no_Magic(c));
		    ফেরত FAIL;
	    }
	    বিরতি;

	ক্ষেত্র Magic('%'):
	    c = no_Magic(getchr());
	    নির্বাচন (c)
	    {
		// () without a back reference
		ক্ষেত্র '(':
		    যদি (nfa_reg(REG_NPAREN) == FAIL)
			ফেরত FAIL;
		    EMIT(NFA_NOPEN);
		    বিরতি;

		ক্ষেত্র 'd':   // %d123 decimal
		ক্ষেত্র 'o':   // %o123 octal
		ক্ষেত্র 'x':   // %xab hex 2
		ক্ষেত্র 'u':   // %uabcd hex 4
		ক্ষেত্র 'U':   // %U1234abcd hex 8
		    {
			vimlong_T nr;

			নির্বাচন (c)
			{
			    ক্ষেত্র 'd': nr = getdecchrs(); বিরতি;
			    ক্ষেত্র 'o': nr = getoctchrs(); বিরতি;
			    ক্ষেত্র 'x': nr = gethexchrs(2); বিরতি;
			    ক্ষেত্র 'u': nr = gethexchrs(4); বিরতি;
			    ক্ষেত্র 'U': nr = gethexchrs(8); বিরতি;
			    পূর্বনির্ধারিত:  nr = -1; বিরতি;
			}

			যদি (nr < 0 || nr > INT_MAX)
			    EMSG2_RET_FAIL(_(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			// A NUL is stored in the text as NL
			// TODO: what if a composing character follows?
			EMIT(nr == 0 ? 0x0a : (long)nr);
		    }
		    বিরতি;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		ক্ষেত্র '^':
		    EMIT(NFA_BOF);
		    বিরতি;

		ক্ষেত্র '$':
		    EMIT(NFA_EOF);
		    বিরতি;

		ক্ষেত্র '#':
		    যদি (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_start_of_pattern),
								  regparse[1]);
			ফেরত FAIL;
		    }
		    EMIT(NFA_CURSOR);
		    বিরতি;

		ক্ষেত্র 'V':
		    EMIT(NFA_VISUAL);
		    বিরতি;

		ক্ষেত্র 'C':
		    EMIT(NFA_ANY_COMPOSING);
		    বিরতি;

		ক্ষেত্র '[':
		    {
			পূর্ণসংখ্যা	    n;

			// \%[abc]
			জন্য (n = 0; (c = peekchr()) != ']'; ++n)
			{
			    যদি (c == NUL)
				EMSG2_RET_FAIL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
			    // recursive call!
			    যদি (nfa_regatom() == FAIL)
				ফেরত FAIL;
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
			বিরতি;
		    }

		পূর্বনির্ধারিত:
		    {
			long_u	n = 0;
			পূর্ণসংখ্যা	cmp = c;
			পূর্ণসংখ্যা	cur = FALSE;
			পূর্ণসংখ্যা	got_digit = FALSE;

			যদি (c == '<' || c == '>')
			    c = getchr();
			যদি (no_Magic(c) == '.')
			{
			    cur = TRUE;
			    c = getchr();
			}
			যখন (VIM_ISDIGIT(c))
			{
			    long_u tmp;

			    যদি (cur)
			    {
				semsg(_(e_regexp_সংখ্যা_after_dot_pos_অনুসন্ধান_chr),
								  no_Magic(c));
				ফেরত FAIL;
			    }
			    tmp = n * 10 + (c - '0');

			    যদি (tmp < n)
			    {
				// overflow.
				emsg(_(e_percent_মান_too_large));
				ফেরত FAIL;
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
				ফেরত FAIL;
			    }
			    যদি (c == 'l')
			    {
				যদি (cur)
				    n = curwin->w_cursor.lnum;
				// \%{n}l  \%{n}<l  \%{n}>l
				EMIT(cmp == '<' ? NFA_LNUM_LT :
				     cmp == '>' ? NFA_LNUM_GT : NFA_LNUM);
				যদি (save_পূর্ব_at_start)
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
							 ফাঁকা, ফাঁকা, &vcol, 0);
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
				ফেরত FAIL;
			    }
			    EMIT((পূর্ণসংখ্যা)n);
			    বিরতি;
			}
			অন্যথা যদি (no_Magic(c) == '\'' && n == 0)
			{
			    // \%'m  \%<'m  \%>'m
			    EMIT(cmp == '<' ? NFA_MARK_LT :
				 cmp == '>' ? NFA_MARK_GT : NFA_MARK);
			    EMIT(getchr());
			    বিরতি;
			}
		    }
		    semsg(_(e_nfa_regexp_unknown_operator_percent_chr),
								  no_Magic(c));
		    ফেরত FAIL;
	    }
	    বিরতি;

	ক্ষেত্র Magic('['):
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
		পূর্ণসংখ্যা plen;
		bool range_endpoint;
		/*
		 * Try to reverse engineer character classes. For example,
		 * recognize that [0-9] stands for \d and [A-Za-z_] for \h,
		 * and perform the necessary substitutions in the NFA.
		 */
		ফলাফল = nfa_recognize_char_class(regparse, endp,
							 extra == NFA_ADD_NL);
		যদি (ফলাফল != FAIL)
		{
		    যদি (ফলাফল >= NFA_FIRST_NL && ফলাফল <= NFA_LAST_NL)
		    {
			EMIT(ফলাফল - NFA_ADD_NL);
			EMIT(NFA_NEWL);
			EMIT(NFA_OR);
		    }
		    অন্যথা
			EMIT(ফলাফল);
		    regparse = endp;
		    MB_PTR_ADV(regparse);
		    ফেরত OK;
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
		যখন (regparse < endp)
		{
		    পূর্ণসংখ্যা	    oldstartc = startc;
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
			    নির্বাচন (charclass)
			    {
				ক্ষেত্র CLASS_ALNUM:
				    EMIT(NFA_CLASS_ALNUM);
				    বিরতি;
				ক্ষেত্র CLASS_ALPHA:
				    EMIT(NFA_CLASS_ALPHA);
				    বিরতি;
				ক্ষেত্র CLASS_BLANK:
				    EMIT(NFA_CLASS_BLANK);
				    বিরতি;
				ক্ষেত্র CLASS_CNTRL:
				    EMIT(NFA_CLASS_CNTRL);
				    বিরতি;
				ক্ষেত্র CLASS_DIGIT:
				    EMIT(NFA_CLASS_DIGIT);
				    বিরতি;
				ক্ষেত্র CLASS_GRAPH:
				    EMIT(NFA_CLASS_GRAPH);
				    বিরতি;
				ক্ষেত্র CLASS_LOWER:
				    wants_nfa = TRUE;
				    EMIT(NFA_CLASS_LOWER);
				    বিরতি;
				ক্ষেত্র CLASS_PRINT:
				    EMIT(NFA_CLASS_PRINT);
				    বিরতি;
				ক্ষেত্র CLASS_PUNCT:
				    EMIT(NFA_CLASS_PUNCT);
				    বিরতি;
				ক্ষেত্র CLASS_SPACE:
				    EMIT(NFA_CLASS_SPACE);
				    বিরতি;
				ক্ষেত্র CLASS_UPPER:
				    wants_nfa = TRUE;
				    EMIT(NFA_CLASS_UPPER);
				    বিরতি;
				ক্ষেত্র CLASS_XDIGIT:
				    EMIT(NFA_CLASS_XDIGIT);
				    বিরতি;
				ক্ষেত্র CLASS_TAB:
				    EMIT(NFA_CLASS_TAB);
				    বিরতি;
				ক্ষেত্র CLASS_RETURN:
				    EMIT(NFA_CLASS_RETURN);
				    বিরতি;
				ক্ষেত্র CLASS_BACKSPACE:
				    EMIT(NFA_CLASS_BACKSPACE);
				    বিরতি;
				ক্ষেত্র CLASS_ESCAPE:
				    EMIT(NFA_CLASS_ESCAPE);
				    বিরতি;
				ক্ষেত্র CLASS_IDENT:
				    EMIT(NFA_CLASS_IDENT);
				    বিরতি;
				ক্ষেত্র CLASS_KEYWORD:
				    EMIT(NFA_CLASS_KEYWORD);
				    বিরতি;
				ক্ষেত্র CLASS_FNAME:
				    EMIT(NFA_CLASS_FNAME);
				    বিরতি;
			    }
			    EMIT(NFA_CONCAT);
			    চালু;
			}
			// Try equivalence class [=a=] and the like
			যদি (equiclass != 0)
			{
			    ফলাফল = nfa_emit_equi_class(equiclass);
			    যদি (ফলাফল == FAIL)
			    {
				// should never happen
				EMSG_RET_FAIL(_(e_error_building_nfa_with_equivalence_class));
			    }
			    চালু;
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
			চালু;	    // reading the end of the range
		    }

		    // Now handle simple and escaped characters.
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    যদি (*regparse == '\\'
			    && !reg_cpo_bsl
			    && regparse + 1 <= endp
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != ফাঁকা
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR, regparse[1])
								      != ফাঁকা)
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
			পূর্ণসংখ্যা	endc = startc;
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
			    জন্য (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			অন্যথা
			{
			    // Emit the range. "startc" was already emitted, so
			    // skip it.
			    জন্য (c = startc + 1; c <= endc; c++)
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
			পূর্ণসংখ্যা i = utf_ptr2len(regparse);

			c = utf_ptr2char(regparse + i);

			// Add composing characters
			জন্য (;;)
			{
			    যদি (c == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    অন্যথা
				EMIT(c);
			    EMIT(NFA_CONCAT);
			    যদি ((i += utf_char2len(c)) >= plen)
				বিরতি;
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

		ফেরত OK;
	    } // if exists closing ]

	    যদি (reg_strict)
		EMSG_RET_FAIL(_(e_missing_rsb_after_str_lsb));
	    // FALLTHROUGH

	পূর্বনির্ধারিত:
	    {
		পূর্ণসংখ্যা	plen;

nfa_do_multibyte:
		// plen is length of current char with composing chars
		যদি (enc_utf8 && ((*mb_char2len)(c)
			    != (plen = utfc_ptr2len(old_regparse))
						       || utf_iscomposing(c)))
		{
		    পূর্ণসংখ্যা i = 0;

		    // A base character plus composing characters, or just one
		    // or more composing characters.
		    // This requires creating a separate atom as if enclosing
		    // the characters in (), where NFA_COMPOSING is the ( and
		    // NFA_END_COMPOSING is the ). Note that right now we are
		    // building the postfix form, not the NFA itself;
		    // a composing char could be: a, b, c, NFA_COMPOSING
		    // where 'b' and 'c' are chars with codes > 256.
		    জন্য (;;)
		    {
			EMIT(c);
			যদি (i > 0)
			    EMIT(NFA_CONCAT);
			যদি ((i += utf_char2len(c)) >= plen)
			    বিরতি;
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
		ফেরত OK;
	    }
    }

    ফেরত OK;
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
    স্থির পূর্ণসংখ্যা
nfa_regpiece(শূন্য)
{
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		op;
    পূর্ণসংখ্যা		ret;
    long	minval, maxval;
    পূর্ণসংখ্যা		greedy = TRUE;      // Braces are prefixed with '-' ?
    parse_state_T old_state;
    parse_state_T new_state;
    long	c2;
    পূর্ণসংখ্যা		old_post_pos;
    পূর্ণসংখ্যা		my_post_start;
    পূর্ণসংখ্যা		quest;

    // Save the current parse state, so that we can use it if <atom>{m,n} is
    // next.
    save_parse_state(&old_state);

    // store current pos in the postfix form, for \{m,n} involving 0s
    my_post_start = (পূর্ণসংখ্যা)(post_ptr - post_start);

    ret = nfa_regatom();
    যদি (ret == FAIL)
	ফেরত FAIL;	    // cascaded error

    op = peekchr();
    যদি (re_multi_type(op) == NOT_MULTI)
	ফেরত OK;

    skipchr();
    নির্বাচন (op)
    {
	ক্ষেত্র Magic('*'):
	    EMIT(NFA_STAR);
	    বিরতি;

	ক্ষেত্র Magic('+'):
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
		ফেরত FAIL;
	    EMIT(NFA_STAR);
	    EMIT(NFA_CONCAT);
	    skipchr();		// skip the \+
	    বিরতি;

	ক্ষেত্র Magic('@'):
	    c2 = (long)getdecchrs();
	    op = no_Magic(getchr());
	    i = 0;
	    নির্বাচন(op)
	    {
		ক্ষেত্র '=':
		    // \@=
		    i = NFA_পূর্ব_ATOM_NO_WIDTH;
		    বিরতি;
		ক্ষেত্র '!':
		    // \@!
		    i = NFA_পূর্ব_ATOM_NO_WIDTH_NEG;
		    বিরতি;
		ক্ষেত্র '<':
		    op = no_Magic(getchr());
		    যদি (op == '=')
			// \@<=
			i = NFA_পূর্ব_ATOM_JUST_BEFORE;
		    অন্যথা যদি (op == '!')
			// \@<!
			i = NFA_পূর্ব_ATOM_JUST_BEFORE_NEG;
		    বিরতি;
		ক্ষেত্র '>':
		    // \@>
		    i = NFA_পূর্ব_ATOM_LIKE_PATTERN;
		    বিরতি;
	    }
	    যদি (i == 0)
	    {
		semsg(_(e_nfa_regexp_unknown_operator_at_chr), op);
		ফেরত FAIL;
	    }
	    EMIT(i);
	    যদি (i == NFA_পূর্ব_ATOM_JUST_BEFORE
					|| i == NFA_পূর্ব_ATOM_JUST_BEFORE_NEG)
		EMIT(c2);
	    বিরতি;

	ক্ষেত্র Magic('?'):
	ক্ষেত্র Magic('='):
	    EMIT(NFA_QUEST);
	    বিরতি;

	ক্ষেত্র Magic('{'):
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
		বিরতি;
	    }

	    // Special case: x{0} or x{-0}
	    যদি (maxval == 0)
	    {
		// Ignore result of previous call to nfa_regatom()
		post_ptr = post_start + my_post_start;
		// NFA_EMPTY is 0-length and works everywhere
		EMIT(NFA_EMPTY);
		ফেরত OK;
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
		ফেরত FAIL;

	    // Ignore previous call to nfa_regatom()
	    post_ptr = post_start + my_post_start;
	    // Save parse state after the repeated atom and the \{}
	    save_parse_state(&new_state);

	    quest = (greedy == TRUE? NFA_QUEST : NFA_QUEST_NONGREEDY);
	    জন্য (i = 0; i < maxval; i++)
	    {
		// Goto beginning of the repeated atom
		restore_parse_state(&old_state);
		old_post_pos = (পূর্ণসংখ্যা)(post_ptr - post_start);
		যদি (nfa_regatom() == FAIL)
		    ফেরত FAIL;
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
		    বিরতি;
	    }

	    // Go to just after the repeated atom and the \{}
	    restore_parse_state(&new_state);
	    curchr = -1;

	    বিরতি;


	পূর্বনির্ধারিত:
	    বিরতি;
    }	// end switch

    যদি (re_multi_type(peekchr()) != NOT_MULTI)
	// Can't have a multi follow a multi.
	EMSG_RET_FAIL(_(e_nfa_regexp_cant_have_multi_follow_multi));

    ফেরত OK;
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
    স্থির পূর্ণসংখ্যা
nfa_regconcat(শূন্য)
{
    পূর্ণসংখ্যা		cont = TRUE;
    পূর্ণসংখ্যা		first = TRUE;

    যখন (cont)
    {
	নির্বাচন (peekchr())
	{
	    ক্ষেত্র NUL:
	    ক্ষেত্র Magic('|'):
	    ক্ষেত্র Magic('&'):
	    ক্ষেত্র Magic(')'):
		cont = FALSE;
		বিরতি;

	    ক্ষেত্র Magic('Z'):
		regflags |= RF_ICOMBINE;
		skipchr_keepstart();
		বিরতি;
	    ক্ষেত্র Magic('c'):
		regflags |= RF_ICASE;
		skipchr_keepstart();
		বিরতি;
	    ক্ষেত্র Magic('C'):
		regflags |= RF_NOICASE;
		skipchr_keepstart();
		বিরতি;
	    ক্ষেত্র Magic('v'):
		reg_magic = MAGIC_ALL;
		skipchr_keepstart();
		curchr = -1;
		বিরতি;
	    ক্ষেত্র Magic('m'):
		reg_magic = MAGIC_ON;
		skipchr_keepstart();
		curchr = -1;
		বিরতি;
	    ক্ষেত্র Magic('M'):
		reg_magic = MAGIC_OFF;
		skipchr_keepstart();
		curchr = -1;
		বিরতি;
	    ক্ষেত্র Magic('V'):
		reg_magic = MAGIC_NONE;
		skipchr_keepstart();
		curchr = -1;
		বিরতি;

	    পূর্বনির্ধারিত:
		যদি (nfa_regpiece() == FAIL)
		    ফেরত FAIL;
		যদি (first == FALSE)
		    EMIT(NFA_CONCAT);
		অন্যথা
		    first = FALSE;
		বিরতি;
	}
    }

    ফেরত OK;
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
    স্থির পূর্ণসংখ্যা
nfa_regbranch(শূন্য)
{
    পূর্ণসংখ্যা		old_post_pos;

    old_post_pos = (পূর্ণসংখ্যা)(post_ptr - post_start);

    // First branch, possibly the only one
    যদি (nfa_regconcat() == FAIL)
	ফেরত FAIL;

    // Try next concats
    যখন (peekchr() == Magic('&'))
    {
	skipchr();
	// if concat is empty do emit a node
	যদি (old_post_pos == (পূর্ণসংখ্যা)(post_ptr - post_start))
	    EMIT(NFA_EMPTY);
	EMIT(NFA_NOPEN);
	EMIT(NFA_পূর্ব_ATOM_NO_WIDTH);
	old_post_pos = (পূর্ণসংখ্যা)(post_ptr - post_start);
	যদি (nfa_regconcat() == FAIL)
	    ফেরত FAIL;
	// if concat is empty do emit a node
	যদি (old_post_pos == (পূর্ণসংখ্যা)(post_ptr - post_start))
	    EMIT(NFA_EMPTY);
	EMIT(NFA_CONCAT);
    }

    // if a branch is empty, emit one node for it
    যদি (old_post_pos == (পূর্ণসংখ্যা)(post_ptr - post_start))
	EMIT(NFA_EMPTY);

    ফেরত OK;
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
    স্থির পূর্ণসংখ্যা
nfa_reg(
    পূর্ণসংখ্যা		paren)	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
{
    পূর্ণসংখ্যা		parno = 0;
    পূর্ণসংখ্যা		status = FAIL;

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
	যাও theend;	    // cascaded error

    যখন (peekchr() == Magic('|'))
    {
	skipchr();
	যদি (nfa_regbranch() == FAIL)
	    যাও theend;    // cascaded error
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
	যাও theend;
    }
    অন্যথা যদি (paren == REG_NOPAREN && peekchr() != NUL)
    {
	যদি (peekchr() == Magic(')'))
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	অন্যথা
	    emsg(_(e_nfa_regexp_proper_termination_error));
	rc_did_emsg = TRUE;
	যাও theend;
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
    ফেরত status;
}

#ifdef DEBUG
স্থির char_u code[50];

    স্থির শূন্য
nfa_set_code(পূর্ণসংখ্যা c)
{
    পূর্ণসংখ্যা	    addnl = FALSE;

    যদি (c >= NFA_FIRST_NL && c <= NFA_LAST_NL)
    {
	addnl = TRUE;
	c -= NFA_ADD_NL;
    }

    STRCPY(code, "");
    নির্বাচন (c)
    {
	ক্ষেত্র NFA_MATCH:	    STRCPY(code, "NFA_MATCH "); বিরতি;
	ক্ষেত্র NFA_SPLIT:	    STRCPY(code, "NFA_SPLIT "); বিরতি;
	ক্ষেত্র NFA_CONCAT:    STRCPY(code, "NFA_CONCAT "); বিরতি;
	ক্ষেত্র NFA_NEWL:	    STRCPY(code, "NFA_NEWL "); বিরতি;
	ক্ষেত্র NFA_ZSTART:    STRCPY(code, "NFA_ZSTART"); বিরতি;
	ক্ষেত্র NFA_ZEND:	    STRCPY(code, "NFA_ZEND"); বিরতি;

	ক্ষেত্র NFA_BACKREF1:  STRCPY(code, "NFA_BACKREF1"); বিরতি;
	ক্ষেত্র NFA_BACKREF2:  STRCPY(code, "NFA_BACKREF2"); বিরতি;
	ক্ষেত্র NFA_BACKREF3:  STRCPY(code, "NFA_BACKREF3"); বিরতি;
	ক্ষেত্র NFA_BACKREF4:  STRCPY(code, "NFA_BACKREF4"); বিরতি;
	ক্ষেত্র NFA_BACKREF5:  STRCPY(code, "NFA_BACKREF5"); বিরতি;
	ক্ষেত্র NFA_BACKREF6:  STRCPY(code, "NFA_BACKREF6"); বিরতি;
	ক্ষেত্র NFA_BACKREF7:  STRCPY(code, "NFA_BACKREF7"); বিরতি;
	ক্ষেত্র NFA_BACKREF8:  STRCPY(code, "NFA_BACKREF8"); বিরতি;
	ক্ষেত্র NFA_BACKREF9:  STRCPY(code, "NFA_BACKREF9"); বিরতি;
# ifdef FEAT_SYN_HL
	ক্ষেত্র NFA_ZREF1:	    STRCPY(code, "NFA_ZREF1"); বিরতি;
	ক্ষেত্র NFA_ZREF2:	    STRCPY(code, "NFA_ZREF2"); বিরতি;
	ক্ষেত্র NFA_ZREF3:	    STRCPY(code, "NFA_ZREF3"); বিরতি;
	ক্ষেত্র NFA_ZREF4:	    STRCPY(code, "NFA_ZREF4"); বিরতি;
	ক্ষেত্র NFA_ZREF5:	    STRCPY(code, "NFA_ZREF5"); বিরতি;
	ক্ষেত্র NFA_ZREF6:	    STRCPY(code, "NFA_ZREF6"); বিরতি;
	ক্ষেত্র NFA_ZREF7:	    STRCPY(code, "NFA_ZREF7"); বিরতি;
	ক্ষেত্র NFA_ZREF8:	    STRCPY(code, "NFA_ZREF8"); বিরতি;
	ক্ষেত্র NFA_ZREF9:	    STRCPY(code, "NFA_ZREF9"); বিরতি;
# endif
	ক্ষেত্র NFA_SKIP:	    STRCPY(code, "NFA_SKIP"); বিরতি;

	ক্ষেত্র NFA_পূর্ব_ATOM_NO_WIDTH:
			    STRCPY(code, "NFA_PREV_ATOM_NO_WIDTH"); বিরতি;
	ক্ষেত্র NFA_পূর্ব_ATOM_NO_WIDTH_NEG:
			    STRCPY(code, "NFA_PREV_ATOM_NO_WIDTH_NEG"); বিরতি;
	ক্ষেত্র NFA_পূর্ব_ATOM_JUST_BEFORE:
			    STRCPY(code, "NFA_PREV_ATOM_JUST_BEFORE"); বিরতি;
	ক্ষেত্র NFA_পূর্ব_ATOM_JUST_BEFORE_NEG:
			 STRCPY(code, "NFA_PREV_ATOM_JUST_BEFORE_NEG"); বিরতি;
	ক্ষেত্র NFA_পূর্ব_ATOM_LIKE_PATTERN:
			    STRCPY(code, "NFA_PREV_ATOM_LIKE_PATTERN"); বিরতি;

	ক্ষেত্র NFA_NOPEN:		    STRCPY(code, "NFA_NOPEN"); বিরতি;
	ক্ষেত্র NFA_NCLOSE:	    STRCPY(code, "NFA_NCLOSE"); বিরতি;
	ক্ষেত্র NFA_START_INVISIBLE:   STRCPY(code, "NFA_START_INVISIBLE"); বিরতি;
	ক্ষেত্র NFA_START_INVISIBLE_FIRST:
			     STRCPY(code, "NFA_START_INVISIBLE_FIRST"); বিরতি;
	ক্ষেত্র NFA_START_INVISIBLE_NEG:
			       STRCPY(code, "NFA_START_INVISIBLE_NEG"); বিরতি;
	ক্ষেত্র NFA_START_INVISIBLE_NEG_FIRST:
			 STRCPY(code, "NFA_START_INVISIBLE_NEG_FIRST"); বিরতি;
	ক্ষেত্র NFA_START_INVISIBLE_BEFORE:
			    STRCPY(code, "NFA_START_INVISIBLE_BEFORE"); বিরতি;
	ক্ষেত্র NFA_START_INVISIBLE_BEFORE_FIRST:
		      STRCPY(code, "NFA_START_INVISIBLE_BEFORE_FIRST"); বিরতি;
	ক্ষেত্র NFA_START_INVISIBLE_BEFORE_NEG:
			STRCPY(code, "NFA_START_INVISIBLE_BEFORE_NEG"); বিরতি;
	ক্ষেত্র NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
		  STRCPY(code, "NFA_START_INVISIBLE_BEFORE_NEG_FIRST"); বিরতি;
	ক্ষেত্র NFA_START_PATTERN:   STRCPY(code, "NFA_START_PATTERN"); বিরতি;
	ক্ষেত্র NFA_END_INVISIBLE:	    STRCPY(code, "NFA_END_INVISIBLE"); বিরতি;
	ক্ষেত্র NFA_END_INVISIBLE_NEG: STRCPY(code, "NFA_END_INVISIBLE_NEG"); বিরতি;
	ক্ষেত্র NFA_END_PATTERN:	    STRCPY(code, "NFA_END_PATTERN"); বিরতি;

	ক্ষেত্র NFA_COMPOSING:	    STRCPY(code, "NFA_COMPOSING"); বিরতি;
	ক্ষেত্র NFA_END_COMPOSING:	    STRCPY(code, "NFA_END_COMPOSING"); বিরতি;
	ক্ষেত্র NFA_OPT_CHARS:	    STRCPY(code, "NFA_OPT_CHARS"); বিরতি;

	ক্ষেত্র NFA_MOPEN:
	ক্ষেত্র NFA_MOPEN1:
	ক্ষেত্র NFA_MOPEN2:
	ক্ষেত্র NFA_MOPEN3:
	ক্ষেত্র NFA_MOPEN4:
	ক্ষেত্র NFA_MOPEN5:
	ক্ষেত্র NFA_MOPEN6:
	ক্ষেত্র NFA_MOPEN7:
	ক্ষেত্র NFA_MOPEN8:
	ক্ষেত্র NFA_MOPEN9:
	    STRCPY(code, "NFA_MOPEN(x)");
	    code[10] = c - NFA_MOPEN + '0';
	    বিরতি;
	ক্ষেত্র NFA_MCLOSE:
	ক্ষেত্র NFA_MCLOSE1:
	ক্ষেত্র NFA_MCLOSE2:
	ক্ষেত্র NFA_MCLOSE3:
	ক্ষেত্র NFA_MCLOSE4:
	ক্ষেত্র NFA_MCLOSE5:
	ক্ষেত্র NFA_MCLOSE6:
	ক্ষেত্র NFA_MCLOSE7:
	ক্ষেত্র NFA_MCLOSE8:
	ক্ষেত্র NFA_MCLOSE9:
	    STRCPY(code, "NFA_MCLOSE(x)");
	    code[11] = c - NFA_MCLOSE + '0';
	    বিরতি;
# ifdef FEAT_SYN_HL
	ক্ষেত্র NFA_ZOPEN:
	ক্ষেত্র NFA_ZOPEN1:
	ক্ষেত্র NFA_ZOPEN2:
	ক্ষেত্র NFA_ZOPEN3:
	ক্ষেত্র NFA_ZOPEN4:
	ক্ষেত্র NFA_ZOPEN5:
	ক্ষেত্র NFA_ZOPEN6:
	ক্ষেত্র NFA_ZOPEN7:
	ক্ষেত্র NFA_ZOPEN8:
	ক্ষেত্র NFA_ZOPEN9:
	    STRCPY(code, "NFA_ZOPEN(x)");
	    code[10] = c - NFA_ZOPEN + '0';
	    বিরতি;
	ক্ষেত্র NFA_ZCLOSE:
	ক্ষেত্র NFA_ZCLOSE1:
	ক্ষেত্র NFA_ZCLOSE2:
	ক্ষেত্র NFA_ZCLOSE3:
	ক্ষেত্র NFA_ZCLOSE4:
	ক্ষেত্র NFA_ZCLOSE5:
	ক্ষেত্র NFA_ZCLOSE6:
	ক্ষেত্র NFA_ZCLOSE7:
	ক্ষেত্র NFA_ZCLOSE8:
	ক্ষেত্র NFA_ZCLOSE9:
	    STRCPY(code, "NFA_ZCLOSE(x)");
	    code[11] = c - NFA_ZCLOSE + '0';
	    বিরতি;
# endif
	ক্ষেত্র NFA_EOL:		STRCPY(code, "NFA_EOL "); বিরতি;
	ক্ষেত্র NFA_BOL:		STRCPY(code, "NFA_BOL "); বিরতি;
	ক্ষেত্র NFA_EOW:		STRCPY(code, "NFA_EOW "); বিরতি;
	ক্ষেত্র NFA_BOW:		STRCPY(code, "NFA_BOW "); বিরতি;
	ক্ষেত্র NFA_EOF:		STRCPY(code, "NFA_EOF "); বিরতি;
	ক্ষেত্র NFA_BOF:		STRCPY(code, "NFA_BOF "); বিরতি;
	ক্ষেত্র NFA_LNUM:		STRCPY(code, "NFA_LNUM "); বিরতি;
	ক্ষেত্র NFA_LNUM_GT:	STRCPY(code, "NFA_LNUM_GT "); বিরতি;
	ক্ষেত্র NFA_LNUM_LT:	STRCPY(code, "NFA_LNUM_LT "); বিরতি;
	ক্ষেত্র NFA_COL:		STRCPY(code, "NFA_COL "); বিরতি;
	ক্ষেত্র NFA_COL_GT:	STRCPY(code, "NFA_COL_GT "); বিরতি;
	ক্ষেত্র NFA_COL_LT:	STRCPY(code, "NFA_COL_LT "); বিরতি;
	ক্ষেত্র NFA_VCOL:		STRCPY(code, "NFA_VCOL "); বিরতি;
	ক্ষেত্র NFA_VCOL_GT:	STRCPY(code, "NFA_VCOL_GT "); বিরতি;
	ক্ষেত্র NFA_VCOL_LT:	STRCPY(code, "NFA_VCOL_LT "); বিরতি;
	ক্ষেত্র NFA_MARK:		STRCPY(code, "NFA_MARK "); বিরতি;
	ক্ষেত্র NFA_MARK_GT:	STRCPY(code, "NFA_MARK_GT "); বিরতি;
	ক্ষেত্র NFA_MARK_LT:	STRCPY(code, "NFA_MARK_LT "); বিরতি;
	ক্ষেত্র NFA_CURSOR:	STRCPY(code, "NFA_CURSOR "); বিরতি;
	ক্ষেত্র NFA_VISUAL:	STRCPY(code, "NFA_VISUAL "); বিরতি;
	ক্ষেত্র NFA_ANY_COMPOSING:	STRCPY(code, "NFA_ANY_COMPOSING "); বিরতি;

	ক্ষেত্র NFA_STAR:		STRCPY(code, "NFA_STAR "); বিরতি;
	ক্ষেত্র NFA_STAR_NONGREEDY: STRCPY(code, "NFA_STAR_NONGREEDY "); বিরতি;
	ক্ষেত্র NFA_QUEST:		STRCPY(code, "NFA_QUEST"); বিরতি;
	ক্ষেত্র NFA_QUEST_NONGREEDY: STRCPY(code, "NFA_QUEST_NON_GREEDY"); বিরতি;
	ক্ষেত্র NFA_EMPTY:		STRCPY(code, "NFA_EMPTY"); বিরতি;
	ক্ষেত্র NFA_OR:		STRCPY(code, "NFA_OR"); বিরতি;

	ক্ষেত্র NFA_START_COLL:	STRCPY(code, "NFA_START_COLL"); বিরতি;
	ক্ষেত্র NFA_END_COLL:	STRCPY(code, "NFA_END_COLL"); বিরতি;
	ক্ষেত্র NFA_START_NEG_COLL: STRCPY(code, "NFA_START_NEG_COLL"); বিরতি;
	ক্ষেত্র NFA_END_NEG_COLL:	STRCPY(code, "NFA_END_NEG_COLL"); বিরতি;
	ক্ষেত্র NFA_RANGE:		STRCPY(code, "NFA_RANGE"); বিরতি;
	ক্ষেত্র NFA_RANGE_MIN:	STRCPY(code, "NFA_RANGE_MIN"); বিরতি;
	ক্ষেত্র NFA_RANGE_MAX:	STRCPY(code, "NFA_RANGE_MAX"); বিরতি;

	ক্ষেত্র NFA_CLASS_ALNUM:	STRCPY(code, "NFA_CLASS_ALNUM"); বিরতি;
	ক্ষেত্র NFA_CLASS_ALPHA:	STRCPY(code, "NFA_CLASS_ALPHA"); বিরতি;
	ক্ষেত্র NFA_CLASS_BLANK:	STRCPY(code, "NFA_CLASS_BLANK"); বিরতি;
	ক্ষেত্র NFA_CLASS_CNTRL:	STRCPY(code, "NFA_CLASS_CNTRL"); বিরতি;
	ক্ষেত্র NFA_CLASS_DIGIT:	STRCPY(code, "NFA_CLASS_DIGIT"); বিরতি;
	ক্ষেত্র NFA_CLASS_GRAPH:	STRCPY(code, "NFA_CLASS_GRAPH"); বিরতি;
	ক্ষেত্র NFA_CLASS_LOWER:	STRCPY(code, "NFA_CLASS_LOWER"); বিরতি;
	ক্ষেত্র NFA_CLASS_PRINT:	STRCPY(code, "NFA_CLASS_PRINT"); বিরতি;
	ক্ষেত্র NFA_CLASS_PUNCT:	STRCPY(code, "NFA_CLASS_PUNCT"); বিরতি;
	ক্ষেত্র NFA_CLASS_SPACE:	STRCPY(code, "NFA_CLASS_SPACE"); বিরতি;
	ক্ষেত্র NFA_CLASS_UPPER:	STRCPY(code, "NFA_CLASS_UPPER"); বিরতি;
	ক্ষেত্র NFA_CLASS_XDIGIT:	STRCPY(code, "NFA_CLASS_XDIGIT"); বিরতি;
	ক্ষেত্র NFA_CLASS_TAB:	STRCPY(code, "NFA_CLASS_TAB"); বিরতি;
	ক্ষেত্র NFA_CLASS_RETURN:	STRCPY(code, "NFA_CLASS_RETURN"); বিরতি;
	ক্ষেত্র NFA_CLASS_BACKSPACE:   STRCPY(code, "NFA_CLASS_BACKSPACE"); বিরতি;
	ক্ষেত্র NFA_CLASS_ESCAPE:	STRCPY(code, "NFA_CLASS_ESCAPE"); বিরতি;
	ক্ষেত্র NFA_CLASS_IDENT:	STRCPY(code, "NFA_CLASS_IDENT"); বিরতি;
	ক্ষেত্র NFA_CLASS_KEYWORD:	STRCPY(code, "NFA_CLASS_KEYWORD"); বিরতি;
	ক্ষেত্র NFA_CLASS_FNAME:	STRCPY(code, "NFA_CLASS_FNAME"); বিরতি;

	ক্ষেত্র NFA_ANY:	STRCPY(code, "NFA_ANY"); বিরতি;
	ক্ষেত্র NFA_IDENT:	STRCPY(code, "NFA_IDENT"); বিরতি;
	ক্ষেত্র NFA_SIDENT:STRCPY(code, "NFA_SIDENT"); বিরতি;
	ক্ষেত্র NFA_KWORD:	STRCPY(code, "NFA_KWORD"); বিরতি;
	ক্ষেত্র NFA_SKWORD:STRCPY(code, "NFA_SKWORD"); বিরতি;
	ক্ষেত্র NFA_FNAME:	STRCPY(code, "NFA_FNAME"); বিরতি;
	ক্ষেত্র NFA_SFNAME:STRCPY(code, "NFA_SFNAME"); বিরতি;
	ক্ষেত্র NFA_PRINT:	STRCPY(code, "NFA_PRINT"); বিরতি;
	ক্ষেত্র NFA_SPRINT:STRCPY(code, "NFA_SPRINT"); বিরতি;
	ক্ষেত্র NFA_WHITE:	STRCPY(code, "NFA_WHITE"); বিরতি;
	ক্ষেত্র NFA_NWHITE:STRCPY(code, "NFA_NWHITE"); বিরতি;
	ক্ষেত্র NFA_DIGIT:	STRCPY(code, "NFA_DIGIT"); বিরতি;
	ক্ষেত্র NFA_NDIGIT:STRCPY(code, "NFA_NDIGIT"); বিরতি;
	ক্ষেত্র NFA_HEX:	STRCPY(code, "NFA_HEX"); বিরতি;
	ক্ষেত্র NFA_NHEX:	STRCPY(code, "NFA_NHEX"); বিরতি;
	ক্ষেত্র NFA_OCTAL:	STRCPY(code, "NFA_OCTAL"); বিরতি;
	ক্ষেত্র NFA_NOCTAL:STRCPY(code, "NFA_NOCTAL"); বিরতি;
	ক্ষেত্র NFA_WORD:	STRCPY(code, "NFA_WORD"); বিরতি;
	ক্ষেত্র NFA_NWORD:	STRCPY(code, "NFA_NWORD"); বিরতি;
	ক্ষেত্র NFA_শীর্ষ:	STRCPY(code, "NFA_HEAD"); বিরতি;
	ক্ষেত্র NFA_NHEAD:	STRCPY(code, "NFA_NHEAD"); বিরতি;
	ক্ষেত্র NFA_ALPHA:	STRCPY(code, "NFA_ALPHA"); বিরতি;
	ক্ষেত্র NFA_NALPHA:STRCPY(code, "NFA_NALPHA"); বিরতি;
	ক্ষেত্র NFA_LOWER:	STRCPY(code, "NFA_LOWER"); বিরতি;
	ক্ষেত্র NFA_NLOWER:STRCPY(code, "NFA_NLOWER"); বিরতি;
	ক্ষেত্র NFA_UPPER:	STRCPY(code, "NFA_UPPER"); বিরতি;
	ক্ষেত্র NFA_NUPPER:STRCPY(code, "NFA_NUPPER"); বিরতি;
	ক্ষেত্র NFA_LOWER_IC:  STRCPY(code, "NFA_LOWER_IC"); বিরতি;
	ক্ষেত্র NFA_NLOWER_IC: STRCPY(code, "NFA_NLOWER_IC"); বিরতি;
	ক্ষেত্র NFA_UPPER_IC:  STRCPY(code, "NFA_UPPER_IC"); বিরতি;
	ক্ষেত্র NFA_NUPPER_IC: STRCPY(code, "NFA_NUPPER_IC"); বিরতি;

	পূর্বনির্ধারিত:
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
nfa_postfix_dump(char_u *রাশিমালা, পূর্ণসংখ্যা retval)
{
    পূর্ণসংখ্যা *p;
    FILE *f;

    f = fopen(NFA_REGEXP_DUMP_LOG, "a");
    যদি (f == ফাঁকা)
	ফেরত;

    fprintf(f, "\n-------------------------\n");
    যদি (retval == FAIL)
	fprintf(f, ">>> NFA engine failed... \n");
    অন্যথা যদি (retval == OK)
	fprintf(f, ">>> NFA engine succeeded !\n");
    fprintf(f, "Regexp: \"%s\"\nPostfix notation (char): \"", রাশিমালা);
    জন্য (p = post_start; *p && p < post_ptr; p++)
    {
	nfa_set_code(*p);
	fprintf(f, "%s, ", code);
    }
    fprintf(f, "\"\nPostfix notation (int): ");
    জন্য (p = post_start; *p && p < post_ptr; p++)
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

    যদি (state == ফাঁকা)
	ফেরত;

    fprintf(debugf, "(%2d)", abs(state->id));

    // Output indent
    p = (char_u *)indent->ga_তথ্য;
    যদি (indent->ga_len >= 3)
    {
	পূর্ণসংখ্যা	last = indent->ga_len - 3;
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
	ফেরত;

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

    যদি (debugf == ফাঁকা)
	ফেরত;

    nfa_print_state(debugf, prog->start);

    যদি (prog->reganch)
	fprintf(debugf, "reganch: %d\n", prog->reganch);
    যদি (prog->regstart != NUL)
	fprintf(debugf, "regstart: %c (decimal: %d)\n",
		prog->regstart, prog->regstart);
    যদি (prog->match_text != ফাঁকা)
	fprintf(debugf, "match_text: \"%s\"\n", prog->match_text);

    fclose(debugf);
}
# endif	    // ENABLE_LOG
#endif	    // DEBUG

/*
 * Parse r.e. @expr and convert it into postfix form.
 * Return the postfix string on success, NULL otherwise.
 */
    স্থির পূর্ণসংখ্যা *
re2post(শূন্য)
{
    যদি (nfa_reg(REG_NOPAREN) == FAIL)
	ফেরত ফাঁকা;
    EMIT(NFA_MOPEN);
    ফেরত post_start;
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
alloc_state(পূর্ণসংখ্যা c, nfa_state_T *out, nfa_state_T *out1)
{
    nfa_state_T *s;

    যদি (istate >= nstate)
	ফেরত ফাঁকা;

    s = &state_ptr[istate++];

    s->c    = c;
    s->out  = out;
    s->out1 = out1;
    s->val  = 0;

    s->id   = istate;
    s->lastlist[0] = 0;
    s->lastlist[1] = 0;

    ফেরত s;
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
প্রকারলেখ সংঘ Ptrlist Ptrlist;
সংঘ Ptrlist
{
    Ptrlist	*পরবর্তী;
    nfa_state_T	*s;
};

কাঠামো Frag
{
    nfa_state_T *start;
    Ptrlist	*out;
};
প্রকারলেখ কাঠামো Frag Frag_T;

// Reused across compilations by post2nfa(); freed in free_regexp_stuff().
স্থির Frag_T *nfa_স্তূপ;
স্থির পূর্ণসংখ্যা nfa_স্তূপ_len;

/*
 * Initialize a Frag_T struct and return it.
 */
    স্থির Frag_T
frag(nfa_state_T *start, Ptrlist *out)
{
    Frag_T n;

    n.start = start;
    n.out = out;
    ফেরত n;
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
    l->পরবর্তী = ফাঁকা;
    ফেরত l;
}

/*
 * Patch the list of states at out to point to start.
 */
    স্থির শূন্য
patch(Ptrlist *l, nfa_state_T *s)
{
    Ptrlist *পরবর্তী;

    জন্য (; l; l = পরবর্তী)
    {
	পরবর্তী = l->পরবর্তী;
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
    যখন (l1->পরবর্তী)
	l1 = l1->পরবর্তী;
    l1->পরবর্তী = l2;
    ফেরত oldl1;
}

/*
 * Stack used for transforming postfix form into NFA.
 */
স্থির Frag_T empty;

    স্থির শূন্য
st_error(পূর্ণসংখ্যা *postfix UNUSED, পূর্ণসংখ্যা *end UNUSED, পূর্ণসংখ্যা *p UNUSED)
{
#ifdef NFA_REGEXP_ERROR_LOG
    FILE *df;
    পূর্ণসংখ্যা *p2;

    df = fopen(NFA_REGEXP_ERROR_LOG, "a");
    যদি (df)
    {
	fprintf(df, "Error popping the stack!\n");
# ifdef DEBUG
	fprintf(df, "Current regexp is \"%s\"\n", nfa_regengine.রাশিমালা);
# endif
	fprintf(df, "Postfix form is: ");
# ifdef DEBUG
	জন্য (p2 = postfix; p2 < end; p2++)
	{
	    nfa_set_code(*p2);
	    fprintf(df, "%s, ", code);
	}
	nfa_set_code(*p);
	fprintf(df, "\nCurrent position is: ");
	জন্য (p2 = postfix; p2 <= p; p2 ++)
	{
	    nfa_set_code(*p2);
	    fprintf(df, "%s, ", code);
	}
# অন্যথা
	জন্য (p2 = postfix; p2 < end; p2++)
	    fprintf(df, "%d, ", *p2);
	fprintf(df, "\nCurrent position is: ");
	জন্য (p2 = postfix; p2 <= p; p2 ++)
	    fprintf(df, "%d, ", *p2);
# endif
	fprintf(df, "\n--------------------------\n");
	fclose(df);
    }
#endif
    emsg(_(e_nfa_regexp_could_not_বের_করো_স্তূপ));
}

/*
 * Push an item onto the stack.
 */
    স্থির শূন্য
st_ঠেলো(Frag_T s, Frag_T **p, Frag_T *স্তূপ_end)
{
    Frag_T *stackp = *p;

    যদি (stackp >= স্তূপ_end)
	ফেরত;
    *stackp = s;
    *p = *p + 1;
}

/*
 * Pop an item from the stack.
 */
    স্থির Frag_T
st_বের_করো(Frag_T **p, Frag_T *স্তূপ)
{
    Frag_T *stackp;

    *p = *p - 1;
    stackp = *p;
    যদি (stackp < স্তূপ)
	ফেরত empty;
    ফেরত **p;
}

/*
 * Estimate the maximum byte length of anything matching "state".
 * When unknown or unlimited return -1.
 */
    স্থির পূর্ণসংখ্যা
nfa_max_width(nfa_state_T *startstate, পূর্ণসংখ্যা depth)
{
    পূর্ণসংখ্যা		    l, r;
    nfa_state_T	    *state = startstate;
    পূর্ণসংখ্যা		    len = 0;

    // detect looping in a NFA_SPLIT
    যদি (depth > 4)
	ফেরত -1;

    যখন (state != ফাঁকা)
    {
	নির্বাচন (state->c)
	{
	    ক্ষেত্র NFA_END_INVISIBLE:
	    ক্ষেত্র NFA_END_INVISIBLE_NEG:
		// the end, return what we have
		ফেরত len;

	    ক্ষেত্র NFA_SPLIT:
		// two alternatives, use the maximum
		l = nfa_max_width(state->out, depth + 1);
		r = nfa_max_width(state->out1, depth + 1);
		যদি (l < 0 || r < 0)
		    ফেরত -1;
		ফেরত len + (l > r ? l : r);

	    ক্ষেত্র NFA_ANY:
	    ক্ষেত্র NFA_START_COLL:
	    ক্ষেত্র NFA_START_NEG_COLL:
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
		    যদি (state->out1 == ফাঁকা || state->out1->out == ফাঁকা)
			ফেরত -1;
		    state = state->out1->out;
		    চালু;
		}
		বিরতি;

	    ক্ষেত্র NFA_DIGIT:
	    ক্ষেত্র NFA_WHITE:
	    ক্ষেত্র NFA_HEX:
	    ক্ষেত্র NFA_OCTAL:
		// ascii
		++len;
		বিরতি;

	    ক্ষেত্র NFA_IDENT:
	    ক্ষেত্র NFA_SIDENT:
	    ক্ষেত্র NFA_KWORD:
	    ক্ষেত্র NFA_SKWORD:
	    ক্ষেত্র NFA_FNAME:
	    ক্ষেত্র NFA_SFNAME:
	    ক্ষেত্র NFA_PRINT:
	    ক্ষেত্র NFA_SPRINT:
	    ক্ষেত্র NFA_NWHITE:
	    ক্ষেত্র NFA_NDIGIT:
	    ক্ষেত্র NFA_NHEX:
	    ক্ষেত্র NFA_NOCTAL:
	    ক্ষেত্র NFA_WORD:
	    ক্ষেত্র NFA_NWORD:
	    ক্ষেত্র NFA_শীর্ষ:
	    ক্ষেত্র NFA_NHEAD:
	    ক্ষেত্র NFA_ALPHA:
	    ক্ষেত্র NFA_NALPHA:
	    ক্ষেত্র NFA_LOWER:
	    ক্ষেত্র NFA_NLOWER:
	    ক্ষেত্র NFA_UPPER:
	    ক্ষেত্র NFA_NUPPER:
	    ক্ষেত্র NFA_LOWER_IC:
	    ক্ষেত্র NFA_NLOWER_IC:
	    ক্ষেত্র NFA_UPPER_IC:
	    ক্ষেত্র NFA_NUPPER_IC:
	    ক্ষেত্র NFA_ANY_COMPOSING:
		// possibly non-ascii
		যদি (has_mbyte)
		    len += 3;
		অন্যথা
		    ++len;
		বিরতি;

	    ক্ষেত্র NFA_START_INVISIBLE:
	    ক্ষেত্র NFA_START_INVISIBLE_NEG:
	    ক্ষেত্র NFA_START_INVISIBLE_BEFORE:
	    ক্ষেত্র NFA_START_INVISIBLE_BEFORE_NEG:
		// zero-width, out1 points to the END state
		state = state->out1->out;
		চালু;

	    ক্ষেত্র NFA_BACKREF1:
	    ক্ষেত্র NFA_BACKREF2:
	    ক্ষেত্র NFA_BACKREF3:
	    ক্ষেত্র NFA_BACKREF4:
	    ক্ষেত্র NFA_BACKREF5:
	    ক্ষেত্র NFA_BACKREF6:
	    ক্ষেত্র NFA_BACKREF7:
	    ক্ষেত্র NFA_BACKREF8:
	    ক্ষেত্র NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	    ক্ষেত্র NFA_ZREF1:
	    ক্ষেত্র NFA_ZREF2:
	    ক্ষেত্র NFA_ZREF3:
	    ক্ষেত্র NFA_ZREF4:
	    ক্ষেত্র NFA_ZREF5:
	    ক্ষেত্র NFA_ZREF6:
	    ক্ষেত্র NFA_ZREF7:
	    ক্ষেত্র NFA_ZREF8:
	    ক্ষেত্র NFA_ZREF9:
#endif
	    ক্ষেত্র NFA_NEWL:
	    ক্ষেত্র NFA_SKIP:
		// unknown width
		ফেরত -1;

	    ক্ষেত্র NFA_BOL:
	    ক্ষেত্র NFA_EOL:
	    ক্ষেত্র NFA_BOF:
	    ক্ষেত্র NFA_EOF:
	    ক্ষেত্র NFA_BOW:
	    ক্ষেত্র NFA_EOW:
	    ক্ষেত্র NFA_MOPEN:
	    ক্ষেত্র NFA_MOPEN1:
	    ক্ষেত্র NFA_MOPEN2:
	    ক্ষেত্র NFA_MOPEN3:
	    ক্ষেত্র NFA_MOPEN4:
	    ক্ষেত্র NFA_MOPEN5:
	    ক্ষেত্র NFA_MOPEN6:
	    ক্ষেত্র NFA_MOPEN7:
	    ক্ষেত্র NFA_MOPEN8:
	    ক্ষেত্র NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	    ক্ষেত্র NFA_ZOPEN:
	    ক্ষেত্র NFA_ZOPEN1:
	    ক্ষেত্র NFA_ZOPEN2:
	    ক্ষেত্র NFA_ZOPEN3:
	    ক্ষেত্র NFA_ZOPEN4:
	    ক্ষেত্র NFA_ZOPEN5:
	    ক্ষেত্র NFA_ZOPEN6:
	    ক্ষেত্র NFA_ZOPEN7:
	    ক্ষেত্র NFA_ZOPEN8:
	    ক্ষেত্র NFA_ZOPEN9:
	    ক্ষেত্র NFA_ZCLOSE:
	    ক্ষেত্র NFA_ZCLOSE1:
	    ক্ষেত্র NFA_ZCLOSE2:
	    ক্ষেত্র NFA_ZCLOSE3:
	    ক্ষেত্র NFA_ZCLOSE4:
	    ক্ষেত্র NFA_ZCLOSE5:
	    ক্ষেত্র NFA_ZCLOSE6:
	    ক্ষেত্র NFA_ZCLOSE7:
	    ক্ষেত্র NFA_ZCLOSE8:
	    ক্ষেত্র NFA_ZCLOSE9:
#endif
	    ক্ষেত্র NFA_MCLOSE:
	    ক্ষেত্র NFA_MCLOSE1:
	    ক্ষেত্র NFA_MCLOSE2:
	    ক্ষেত্র NFA_MCLOSE3:
	    ক্ষেত্র NFA_MCLOSE4:
	    ক্ষেত্র NFA_MCLOSE5:
	    ক্ষেত্র NFA_MCLOSE6:
	    ক্ষেত্র NFA_MCLOSE7:
	    ক্ষেত্র NFA_MCLOSE8:
	    ক্ষেত্র NFA_MCLOSE9:
	    ক্ষেত্র NFA_NOPEN:
	    ক্ষেত্র NFA_NCLOSE:

	    ক্ষেত্র NFA_LNUM_GT:
	    ক্ষেত্র NFA_LNUM_LT:
	    ক্ষেত্র NFA_COL_GT:
	    ক্ষেত্র NFA_COL_LT:
	    ক্ষেত্র NFA_VCOL_GT:
	    ক্ষেত্র NFA_VCOL_LT:
	    ক্ষেত্র NFA_MARK_GT:
	    ক্ষেত্র NFA_MARK_LT:
	    ক্ষেত্র NFA_VISUAL:
	    ক্ষেত্র NFA_LNUM:
	    ক্ষেত্র NFA_CURSOR:
	    ক্ষেত্র NFA_COL:
	    ক্ষেত্র NFA_VCOL:
	    ক্ষেত্র NFA_MARK:

	    ক্ষেত্র NFA_ZSTART:
	    ক্ষেত্র NFA_ZEND:
	    ক্ষেত্র NFA_OPT_CHARS:
	    ক্ষেত্র NFA_EMPTY:
	    ক্ষেত্র NFA_START_PATTERN:
	    ক্ষেত্র NFA_END_PATTERN:
	    ক্ষেত্র NFA_COMPOSING:
	    ক্ষেত্র NFA_END_COMPOSING:
		// zero-width
		বিরতি;

	    পূর্বনির্ধারিত:
		যদি (state->c < 0)
		    // don't know what this is
		    ফেরত -1;
		// normal character
		len += MB_CHAR2LEN(state->c);
		বিরতি;
	}

	// normal way to continue
	state = state->out;
    }

    // unrecognized, "cannot happen"
    ফেরত -1;
}

/*
 * Convert a postfix form into its equivalent NFA.
 * Return the NFA start state on success, NULL otherwise.
 */
    স্থির nfa_state_T *
post2nfa(পূর্ণসংখ্যা *postfix, পূর্ণসংখ্যা *end, পূর্ণসংখ্যা nfa_calc_আকার)
{
    পূর্ণসংখ্যা		*p;
    পূর্ণসংখ্যা		mopen;
    পূর্ণসংখ্যা		mclose;
    Frag_T	*স্তূপ = ফাঁকা;
    Frag_T	*stackp = ফাঁকা;
    Frag_T	*স্তূপ_end = ফাঁকা;
    Frag_T	e1;
    Frag_T	e2;
    Frag_T	e;
    nfa_state_T	*s;
    nfa_state_T	*s1;
    nfa_state_T	*matchstate;
    nfa_state_T	*ret = ফাঁকা;

    যদি (postfix == ফাঁকা)
	ফেরত ফাঁকা;

#define ঠেলো_2(s)	    st_ঠেলো((s), &stackp, স্তূপ_end)
#define বের_করো_2()	    st_বের_করো(&stackp, স্তূপ);		\
		    যদি (stackp < স্তূপ)			\
		    {					\
			st_error(postfix, end, p);	\
			ফেরত ফাঁকা;			\
		    }

    যদি (nfa_calc_আকার == FALSE)
    {
	// Reuse the fragment stack across compilations, growing when needed;
	// it is freed in free_regexp_stuff().
	যদি (nfa_স্তূপ == ফাঁকা || nfa_স্তূপ_len < nstate + 1)
	{
	    vim_free(nfa_স্তূপ);
	    nfa_স্তূপ = ALLOC_MULT(Frag_T, nstate + 1);
	    যদি (nfa_স্তূপ == ফাঁকা)
	    {
		nfa_স্তূপ_len = 0;
		ফেরত ফাঁকা;
	    }
	    nfa_স্তূপ_len = nstate + 1;
	}
	স্তূপ = nfa_স্তূপ;
	stackp = স্তূপ;
	স্তূপ_end = স্তূপ + (nstate + 1);
    }

    জন্য (p = postfix; p < end; ++p)
    {
	নির্বাচন (*p)
	{
	ক্ষেত্র NFA_CONCAT:
	    // Concatenation.
	    // Pay attention: this operator does not exist in the r.e. itself
	    // (it is implicit, really).  It is added when r.e. is translated
	    // to postfix form in re2post().
	    যদি (nfa_calc_আকার == TRUE)
	    {
		// nstate += 0;
		বিরতি;
	    }
	    e2 = বের_করো_2();
	    e1 = বের_করো_2();
	    patch(e1.out, e2.start);
	    ঠেলো_2(frag(e1.start, e2.out));
	    বিরতি;

	ক্ষেত্র NFA_OR:
	    // Alternation
	    যদি (nfa_calc_আকার == TRUE)
	    {
		nstate++;
		বিরতি;
	    }
	    e2 = বের_করো_2();
	    e1 = বের_করো_2();
	    s = alloc_state(NFA_SPLIT, e1.start, e2.start);
	    যদি (s == ফাঁকা)
		যাও theend;
	    ঠেলো_2(frag(s, append(e1.out, e2.out)));
	    বিরতি;

	ক্ষেত্র NFA_STAR:
	    // Zero or more, prefer more
	    যদি (nfa_calc_আকার == TRUE)
	    {
		nstate++;
		বিরতি;
	    }
	    e = বের_করো_2();
	    s = alloc_state(NFA_SPLIT, e.start, ফাঁকা);
	    যদি (s == ফাঁকা)
		যাও theend;
	    patch(e.out, s);
	    ঠেলো_2(frag(s, list1(&s->out1)));
	    বিরতি;

	ক্ষেত্র NFA_STAR_NONGREEDY:
	    // Zero or more, prefer zero
	    যদি (nfa_calc_আকার == TRUE)
	    {
		nstate++;
		বিরতি;
	    }
	    e = বের_করো_2();
	    s = alloc_state(NFA_SPLIT, ফাঁকা, e.start);
	    যদি (s == ফাঁকা)
		যাও theend;
	    patch(e.out, s);
	    ঠেলো_2(frag(s, list1(&s->out)));
	    বিরতি;

	ক্ষেত্র NFA_QUEST:
	    // one or zero atoms=> greedy match
	    যদি (nfa_calc_আকার == TRUE)
	    {
		nstate++;
		বিরতি;
	    }
	    e = বের_করো_2();
	    s = alloc_state(NFA_SPLIT, e.start, ফাঁকা);
	    যদি (s == ফাঁকা)
		যাও theend;
	    ঠেলো_2(frag(s, append(e.out, list1(&s->out1))));
	    বিরতি;

	ক্ষেত্র NFA_QUEST_NONGREEDY:
	    // zero or one atoms => non-greedy match
	    যদি (nfa_calc_আকার == TRUE)
	    {
		nstate++;
		বিরতি;
	    }
	    e = বের_করো_2();
	    s = alloc_state(NFA_SPLIT, ফাঁকা, e.start);
	    যদি (s == ফাঁকা)
		যাও theend;
	    ঠেলো_2(frag(s, append(e.out, list1(&s->out))));
	    বিরতি;

	ক্ষেত্র NFA_END_COLL:
	ক্ষেত্র NFA_END_NEG_COLL:
	    // On the stack is the sequence starting with NFA_START_COLL or
	    // NFA_START_NEG_COLL and all possible characters. Patch it to
	    // add the output to the start.
	    যদি (nfa_calc_আকার == TRUE)
	    {
		nstate++;
		বিরতি;
	    }
	    e = বের_করো_2();
	    s = alloc_state(NFA_END_COLL, ফাঁকা, ফাঁকা);
	    যদি (s == ফাঁকা)
		যাও theend;
	    patch(e.out, s);
	    e.start->out1 = s;
	    ঠেলো_2(frag(e.start, list1(&s->out)));
	    বিরতি;

	ক্ষেত্র NFA_RANGE:
	    // Before this are two characters, the low and high end of a
	    // range.  Turn them into two states with MIN and MAX.
	    যদি (nfa_calc_আকার == TRUE)
	    {
		// nstate += 0;
		বিরতি;
	    }
	    e2 = বের_করো_2();
	    e1 = বের_করো_2();
	    e2.start->val = e2.start->c;
	    e2.start->c = NFA_RANGE_MAX;
	    e1.start->val = e1.start->c;
	    e1.start->c = NFA_RANGE_MIN;
	    patch(e1.out, e2.start);
	    ঠেলো_2(frag(e1.start, e2.out));
	    বিরতি;

	ক্ষেত্র NFA_EMPTY:
	    // 0-length, used in a repetition with max/min count of 0
	    যদি (nfa_calc_আকার == TRUE)
	    {
		nstate++;
		বিরতি;
	    }
	    s = alloc_state(NFA_EMPTY, ফাঁকা, ফাঁকা);
	    যদি (s == ফাঁকা)
		যাও theend;
	    ঠেলো_2(frag(s, list1(&s->out)));
	    বিরতি;

	ক্ষেত্র NFA_OPT_CHARS:
	  {
	    পূর্ণসংখ্যা    n;

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
	    যদি (nfa_calc_আকার == TRUE)
	    {
		nstate += n;
		বিরতি;
	    }
	    s = ফাঁকা; // avoid compiler warning
	    e1.out = ফাঁকা; // stores list with out1's
	    s1 = ফাঁকা; // previous NFA_SPLIT to connect to
	    যখন (n-- > 0)
	    {
		e = বের_করো_2(); // get character
		s = alloc_state(NFA_SPLIT, e.start, ফাঁকা);
		যদি (s == ফাঁকা)
		    যাও theend;
		যদি (e1.out == ফাঁকা)
		    e1 = e;
		patch(e.out, s1);
		append(e1.out, list1(&s->out1));
		s1 = s;
	    }
	    ঠেলো_2(frag(s, e1.out));
	    বিরতি;
	  }

	ক্ষেত্র NFA_পূর্ব_ATOM_NO_WIDTH:
	ক্ষেত্র NFA_পূর্ব_ATOM_NO_WIDTH_NEG:
	ক্ষেত্র NFA_পূর্ব_ATOM_JUST_BEFORE:
	ক্ষেত্র NFA_পূর্ব_ATOM_JUST_BEFORE_NEG:
	ক্ষেত্র NFA_পূর্ব_ATOM_LIKE_PATTERN:
	  {
	    পূর্ণসংখ্যা before = (*p == NFA_পূর্ব_ATOM_JUST_BEFORE
				      || *p == NFA_পূর্ব_ATOM_JUST_BEFORE_NEG);
	    পূর্ণসংখ্যা pattern = (*p == NFA_পূর্ব_ATOM_LIKE_PATTERN);
	    পূর্ণসংখ্যা start_state;
	    পূর্ণসংখ্যা end_state;
	    পূর্ণসংখ্যা n = 0;
	    nfa_state_T *zend;
	    nfa_state_T *skip;

	    নির্বাচন (*p)
	    {
		ক্ষেত্র NFA_পূর্ব_ATOM_NO_WIDTH:
		    start_state = NFA_START_INVISIBLE;
		    end_state = NFA_END_INVISIBLE;
		    বিরতি;
		ক্ষেত্র NFA_পূর্ব_ATOM_NO_WIDTH_NEG:
		    start_state = NFA_START_INVISIBLE_NEG;
		    end_state = NFA_END_INVISIBLE_NEG;
		    বিরতি;
		ক্ষেত্র NFA_পূর্ব_ATOM_JUST_BEFORE:
		    start_state = NFA_START_INVISIBLE_BEFORE;
		    end_state = NFA_END_INVISIBLE;
		    বিরতি;
		ক্ষেত্র NFA_পূর্ব_ATOM_JUST_BEFORE_NEG:
		    start_state = NFA_START_INVISIBLE_BEFORE_NEG;
		    end_state = NFA_END_INVISIBLE_NEG;
		    বিরতি;
		পূর্বনির্ধারিত: // NFA_PREV_ATOM_LIKE_PATTERN:
		    start_state = NFA_START_PATTERN;
		    end_state = NFA_END_PATTERN;
		    বিরতি;
	    }

	    যদি (before)
		n = *++p; // get the count

	    // The \@= operator: match the preceding atom with zero width.
	    // The \@! operator: no match for the preceding atom.
	    // The \@<= operator: match for the preceding atom.
	    // The \@<! operator: no match for the preceding atom.
	    // Surrounds the preceding atom with START_INVISIBLE and
	    // END_INVISIBLE, similarly to MOPEN.

	    যদি (nfa_calc_আকার == TRUE)
	    {
		nstate += pattern ? 4 : 2;
		বিরতি;
	    }
	    e = বের_করো_2();
	    s1 = alloc_state(end_state, ফাঁকা, ফাঁকা);
	    যদি (s1 == ফাঁকা)
		যাও theend;

	    s = alloc_state(start_state, e.start, s1);
	    যদি (s == ফাঁকা)
		যাও theend;
	    যদি (pattern)
	    {
		// NFA_ZEND -> NFA_END_PATTERN -> NFA_SKIP -> what follows.
		skip = alloc_state(NFA_SKIP, ফাঁকা, ফাঁকা);
		যদি (skip == ফাঁকা)
		    যাও theend;
		zend = alloc_state(NFA_ZEND, s1, ফাঁকা);
		যদি (zend == ফাঁকা)
		    যাও theend;
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
	    বিরতি;
	  }

	ক্ষেত্র NFA_COMPOSING:	// char with composing char
#যদি 0
	    // TODO
	    যদি (regflags & RF_ICOMBINE)
	    {
		// use the base character only
	    }
#endif
	    // FALLTHROUGH

	ক্ষেত্র NFA_MOPEN:	// \( \) Submatch
	ক্ষেত্র NFA_MOPEN1:
	ক্ষেত্র NFA_MOPEN2:
	ক্ষেত্র NFA_MOPEN3:
	ক্ষেত্র NFA_MOPEN4:
	ক্ষেত্র NFA_MOPEN5:
	ক্ষেত্র NFA_MOPEN6:
	ক্ষেত্র NFA_MOPEN7:
	ক্ষেত্র NFA_MOPEN8:
	ক্ষেত্র NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	ক্ষেত্র NFA_ZOPEN:	// \z( \) Submatch
	ক্ষেত্র NFA_ZOPEN1:
	ক্ষেত্র NFA_ZOPEN2:
	ক্ষেত্র NFA_ZOPEN3:
	ক্ষেত্র NFA_ZOPEN4:
	ক্ষেত্র NFA_ZOPEN5:
	ক্ষেত্র NFA_ZOPEN6:
	ক্ষেত্র NFA_ZOPEN7:
	ক্ষেত্র NFA_ZOPEN8:
	ক্ষেত্র NFA_ZOPEN9:
#endif
	ক্ষেত্র NFA_NOPEN:	// \%( \) "Invisible Submatch"
	    যদি (nfa_calc_আকার == TRUE)
	    {
		nstate += 2;
		বিরতি;
	    }

	    mopen = *p;
	    নির্বাচন (*p)
	    {
		ক্ষেত্র NFA_NOPEN: mclose = NFA_NCLOSE; বিরতি;
#ifdef FEAT_SYN_HL
		ক্ষেত্র NFA_ZOPEN: mclose = NFA_ZCLOSE; বিরতি;
		ক্ষেত্র NFA_ZOPEN1: mclose = NFA_ZCLOSE1; বিরতি;
		ক্ষেত্র NFA_ZOPEN2: mclose = NFA_ZCLOSE2; বিরতি;
		ক্ষেত্র NFA_ZOPEN3: mclose = NFA_ZCLOSE3; বিরতি;
		ক্ষেত্র NFA_ZOPEN4: mclose = NFA_ZCLOSE4; বিরতি;
		ক্ষেত্র NFA_ZOPEN5: mclose = NFA_ZCLOSE5; বিরতি;
		ক্ষেত্র NFA_ZOPEN6: mclose = NFA_ZCLOSE6; বিরতি;
		ক্ষেত্র NFA_ZOPEN7: mclose = NFA_ZCLOSE7; বিরতি;
		ক্ষেত্র NFA_ZOPEN8: mclose = NFA_ZCLOSE8; বিরতি;
		ক্ষেত্র NFA_ZOPEN9: mclose = NFA_ZCLOSE9; বিরতি;
#endif
		ক্ষেত্র NFA_COMPOSING: mclose = NFA_END_COMPOSING; বিরতি;
		পূর্বনির্ধারিত:
		    // NFA_MOPEN, NFA_MOPEN1 .. NFA_MOPEN9
		    mclose = *p + NSUBEXP;
		    বিরতি;
	    }

	    // Allow "NFA_MOPEN" as a valid postfix representation for
	    // the empty regexp "". In this case, the NFA will be
	    // NFA_MOPEN -> NFA_MCLOSE. Note that this also allows
	    // empty groups of parenthesis, and empty mbyte chars
	    যদি (stackp == স্তূপ)
	    {
		s = alloc_state(mopen, ফাঁকা, ফাঁকা);
		যদি (s == ফাঁকা)
		    যাও theend;
		s1 = alloc_state(mclose, ফাঁকা, ফাঁকা);
		যদি (s1 == ফাঁকা)
		    যাও theend;
		patch(list1(&s->out), s1);
		ঠেলো_2(frag(s, list1(&s1->out)));
		বিরতি;
	    }

	    // At least one node was emitted before NFA_MOPEN, so
	    // at least one node will be between NFA_MOPEN and NFA_MCLOSE
	    e = বের_করো_2();
	    s = alloc_state(mopen, e.start, ফাঁকা);   // `('
	    যদি (s == ফাঁকা)
		যাও theend;

	    s1 = alloc_state(mclose, ফাঁকা, ফাঁকা);   // `)'
	    যদি (s1 == ফাঁকা)
		যাও theend;
	    patch(e.out, s1);

	    যদি (mopen == NFA_COMPOSING)
		// COMPOSING->out1 = END_COMPOSING
		patch(list1(&s->out1), s1);

	    ঠেলো_2(frag(s, list1(&s1->out)));
	    বিরতি;

	ক্ষেত্র NFA_BACKREF1:
	ক্ষেত্র NFA_BACKREF2:
	ক্ষেত্র NFA_BACKREF3:
	ক্ষেত্র NFA_BACKREF4:
	ক্ষেত্র NFA_BACKREF5:
	ক্ষেত্র NFA_BACKREF6:
	ক্ষেত্র NFA_BACKREF7:
	ক্ষেত্র NFA_BACKREF8:
	ক্ষেত্র NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	ক্ষেত্র NFA_ZREF1:
	ক্ষেত্র NFA_ZREF2:
	ক্ষেত্র NFA_ZREF3:
	ক্ষেত্র NFA_ZREF4:
	ক্ষেত্র NFA_ZREF5:
	ক্ষেত্র NFA_ZREF6:
	ক্ষেত্র NFA_ZREF7:
	ক্ষেত্র NFA_ZREF8:
	ক্ষেত্র NFA_ZREF9:
#endif
	    যদি (nfa_calc_আকার == TRUE)
	    {
		nstate += 2;
		বিরতি;
	    }
	    s = alloc_state(*p, ফাঁকা, ফাঁকা);
	    যদি (s == ফাঁকা)
		যাও theend;
	    s1 = alloc_state(NFA_SKIP, ফাঁকা, ফাঁকা);
	    যদি (s1 == ফাঁকা)
		যাও theend;
	    patch(list1(&s->out), s1);
	    ঠেলো_2(frag(s, list1(&s1->out)));
	    বিরতি;

	ক্ষেত্র NFA_LNUM:
	ক্ষেত্র NFA_LNUM_GT:
	ক্ষেত্র NFA_LNUM_LT:
	ক্ষেত্র NFA_VCOL:
	ক্ষেত্র NFA_VCOL_GT:
	ক্ষেত্র NFA_VCOL_LT:
	ক্ষেত্র NFA_COL:
	ক্ষেত্র NFA_COL_GT:
	ক্ষেত্র NFA_COL_LT:
	ক্ষেত্র NFA_MARK:
	ক্ষেত্র NFA_MARK_GT:
	ক্ষেত্র NFA_MARK_LT:
	  {
	    পূর্ণসংখ্যা n = *++p; // lnum, col or mark name

	    যদি (nfa_calc_আকার == TRUE)
	    {
		nstate += 1;
		বিরতি;
	    }
	    s = alloc_state(p[-1], ফাঁকা, ফাঁকা);
	    যদি (s == ফাঁকা)
		যাও theend;
	    s->val = n;
	    ঠেলো_2(frag(s, list1(&s->out)));
	    বিরতি;
	  }

	ক্ষেত্র NFA_ZSTART:
	ক্ষেত্র NFA_ZEND:
	পূর্বনির্ধারিত:
	    // Operands
	    যদি (nfa_calc_আকার == TRUE)
	    {
		nstate++;
		বিরতি;
	    }
	    s = alloc_state(*p, ফাঁকা, ফাঁকা);
	    যদি (s == ফাঁকা)
		যাও theend;
	    ঠেলো_2(frag(s, list1(&s->out)));
	    বিরতি;

	} // switch(*p)

    } // for (p = postfix; *p; ++p)

    যদি (nfa_calc_আকার == TRUE)
    {
	nstate++;
	যাও theend;	// Return value when counting size is ignored anyway
    }

    e = বের_করো_2();
    যদি (stackp != স্তূপ)
	EMSG_RET_NULL(_(e_nfa_regexp_while_converting_from_postfix_to_nfa_too_many_stats_বাম_on_স্তূপ));

    যদি (istate >= nstate)
	EMSG_RET_NULL(_(e_nfa_regexp_not_enough_space_to_store_whole_nfa));

    matchstate = &state_ptr[istate++]; // the match state
    matchstate->c = NFA_MATCH;
    matchstate->out = matchstate->out1 = ফাঁকা;
    matchstate->id = 0;

    patch(e.out, matchstate);
    ret = e.start;

theend:
    ফেরত ret;

#undef POP1
#undef PUSH1
#undef POP2
#undef PUSH2
#undef বের_করো_2
#undef ঠেলো_2
}

/*
 * After building the NFA program, inspect it to add optimization hints.
 */
    স্থির শূন্য
nfa_postprocess(nfa_regprog_T *prog)
{
    পূর্ণসংখ্যা i;
    পূর্ণসংখ্যা c;

    জন্য (i = 0; i < prog->nstate; ++i)
    {
	c = prog->state[i].c;
	যদি (c == NFA_START_INVISIBLE
		|| c == NFA_START_INVISIBLE_NEG
		|| c == NFA_START_INVISIBLE_BEFORE
		|| c == NFA_START_INVISIBLE_BEFORE_NEG)
	{
	    পূর্ণসংখ্যা directly;

	    // Do it directly when what follows is possibly the end of the
	    // match.
	    যদি (match_follows(prog->state[i].out1->out, 0))
		directly = TRUE;
	    অন্যথা
	    {
		পূর্ণসংখ্যা ch_invisible = failure_chance(prog->state[i].out, 0);
		পূর্ণসংখ্যা ch_follows = failure_chance(prog->state[i].out1->out, 0);

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

প্রকারলেখ কাঠামো
{
    পূর্ণসংখ্যা	    in_use; // number of subexpr with useful info

    // When REG_MULTI is TRUE list.multi is used, otherwise list.line.
    সংঘ
    {
	কাঠামো multipos
	{
	    linenr_T	start_lnum;
	    linenr_T	end_lnum;
	    colnr_T	start_col;
	    colnr_T	end_col;
	} multi[NSUBEXP];
	কাঠামো linepos
	{
	    char_u	*start;
	    char_u	*end;
	} সারি[NSUBEXP];
    } list;
    colnr_T	orig_start_col;  // list.multi[0].start_col without \zs
} regsub_T;

প্রকারলেখ কাঠামো
{
    regsub_T	norm; // \( .. \) matches
#ifdef FEAT_SYN_HL
    regsub_T	synt; // \z( .. \) matches
#endif
} regsubs_T;

// nfa_pim_T stores a Postponed Invisible Match.
প্রকারলেখ কাঠামো nfa_pim_S nfa_pim_T;
কাঠামো nfa_pim_S
{
    পূর্ণসংখ্যা		ফলাফল;		// NFA_PIM_*, see below
    nfa_state_T	*state;		// the invisible match start state
    regsubs_T	subs;		// submatch info, only party used
    সংঘ
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
প্রকারলেখ কাঠামো
{
    nfa_state_T	*state;
    পূর্ণসংখ্যা		গুনতি;
    nfa_pim_T	pim;		// if pim.result != NFA_PIM_UNUSED: postponed
				// invisible match
    regsubs_T	subs;		// submatch info, only party used
} nfa_thread_T;

// nfa_list_T contains the alternative NFA execution states.
প্রকারলেখ কাঠামো
{
    nfa_thread_T    *t;		// allocated array of states
    পূর্ণসংখ্যা		    n;		// nr of states currently in "t"
    পূর্ণসংখ্যা		    len;	// max nr of states in "t"
    পূর্ণসংখ্যা		    id;		// ID of the list
    পূর্ণসংখ্যা		    has_pim;	// TRUE when any state has a PIM
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
    পূর্ণসংখ্যা j;

    জন্য (j = 0; j < sub->in_use; j++)
	যদি (REG_MULTI)
	    fprintf(log_fd,
		    "*** group %d, start: c=%d, l=%d, end: c=%d, l=%d\n",
		    j,
		    sub->list.multi[j].start_col,
		    (পূর্ণসংখ্যা)sub->list.multi[j].start_lnum,
		    sub->list.multi[j].end_col,
		    (পূর্ণসংখ্যা)sub->list.multi[j].end_lnum);
	অন্যথা
	{
	    অক্ষর *s = (অক্ষর *)sub->list.সারি[j].start;
	    অক্ষর *e = (অক্ষর *)sub->list.সারি[j].end;

	    fprintf(log_fd, "*** group %d, start: \"%s\", end: \"%s\"\n",
		    j,
		    s == ফাঁকা ? "NULL" : s,
		    e == ফাঁকা ? "NULL" : e);
	}
}

    স্থির অক্ষর *
pim_info(nfa_pim_T *pim)
{
    স্থির অক্ষর buf[30];

    যদি (pim == ফাঁকা || pim->ফলাফল == NFA_PIM_UNUSED)
	buf[0] = NUL;
    অন্যথা
    {
	sprintf(buf, " PIM col %d", REG_MULTI ? (পূর্ণসংখ্যা)pim->end.pos.col
		: (পূর্ণসংখ্যা)(pim->end.ptr - rex.প্রবেশ));
    }
    ফেরত buf;
}

#endif

// Used during execution: whether a match has been found.
স্থির পূর্ণসংখ্যা	    nfa_match;
#ifdef FEAT_RELTIME
স্থির পূর্ণসংখ্যা	   *nfa_timed_out;
#endif

স্থির অন্তঃরেখ শূন্য copy_sub(regsub_T *to, regsub_T *from);
স্থির পূর্ণসংখ্যা pim_equal(nfa_pim_T *one, nfa_pim_T *two);

/*
 * Copy postponed invisible match info from "from" to "to".
 */
    স্থির শূন্য
copy_pim(nfa_pim_T *to, nfa_pim_T *from)
{
    to->ফলাফল = from->ফলাফল;
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
				  আকার(কাঠামো multipos) * rex.nfa_nsubexpr);
    অন্যথা
	vim_memset(sub->list.সারি, 0,
				   আকার(কাঠামো linepos) * rex.nfa_nsubexpr);
    sub->in_use = 0;
}

/*
 * Copy the submatches from "from" to "to".
 */
    স্থির অন্তঃরেখ শূন্য
copy_sub(regsub_T *to, regsub_T *from)
{
    to->in_use = from->in_use;
    যদি (from->in_use <= 0)
	ফেরত;

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
		    আকার(কাঠামো multipos) * from->in_use);
	to->orig_start_col = from->orig_start_col;
    }
    অন্যথা
    {
	যদি (from->in_use == 1)
	    to->list.সারি[0] = from->list.সারি[0];
	অন্যথা
	    mch_memmove(&to->list.সারি[0],
		    &from->list.সারি[0],
		    আকার(কাঠামো linepos) * from->in_use);
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
	ফেরত;

    // Copy the match start and end positions.
    যদি (REG_MULTI)
	mch_memmove(&to->list.multi[1],
		&from->list.multi[1],
		আকার(কাঠামো multipos) * (from->in_use - 1));
    অন্যথা
	mch_memmove(&to->list.সারি[1],
		&from->list.সারি[1],
		আকার(কাঠামো linepos) * (from->in_use - 1));
}

/*
 * Like copy_sub() but only do the end of the main match if \ze is present.
 */
    স্থির শূন্য
copy_ze_off(regsub_T *to, regsub_T *from)
{
    যদি (!rex.nfa_has_zend)
	ফেরত;

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
	যদি (from->list.সারি[0].end != ফাঁকা)
	    to->list.সারি[0].end = from->list.সারি[0].end;
    }
}

/*
 * Return TRUE if "sub1" and "sub2" have the same start positions.
 * When using back-references also check the end position.
 */
    স্থির পূর্ণসংখ্যা
sub_equal(regsub_T *sub1, regsub_T *sub2)
{
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		todo;
    linenr_T	s1;
    linenr_T	s2;
    char_u	*sp1;
    char_u	*sp2;

    todo = sub1->in_use > sub2->in_use ? sub1->in_use : sub2->in_use;
    যদি (REG_MULTI)
    {
	জন্য (i = 0; i < todo; ++i)
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
		ফেরত FALSE;
	    যদি (s1 != -1 && sub1->list.multi[i].start_col
					     != sub2->list.multi[i].start_col)
		ফেরত FALSE;

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
		    ফেরত FALSE;
		যদি (s1 != -1 && sub1->list.multi[i].end_col
					       != sub2->list.multi[i].end_col)
		ফেরত FALSE;
	    }
	}
    }
    অন্যথা
    {
	জন্য (i = 0; i < todo; ++i)
	{
	    যদি (i < sub1->in_use)
		sp1 = sub1->list.সারি[i].start;
	    অন্যথা
		sp1 = ফাঁকা;
	    যদি (i < sub2->in_use)
		sp2 = sub2->list.সারি[i].start;
	    অন্যথা
		sp2 = ফাঁকা;
	    যদি (sp1 != sp2)
		ফেরত FALSE;
	    যদি (rex.nfa_has_backref)
	    {
		যদি (i < sub1->in_use)
		    sp1 = sub1->list.সারি[i].end;
		অন্যথা
		    sp1 = ফাঁকা;
		যদি (i < sub2->in_use)
		    sp2 = sub2->list.সারি[i].end;
		অন্যথা
		    sp2 = ফাঁকা;
		যদি (sp1 != sp2)
		    ফেরত FALSE;
	    }
	}
    }

    ফেরত TRUE;
}

#ifdef FEAT_RELTIME
/*
 * Check if we are past the time limit, if there is one.
 */
    স্থির পূর্ণসংখ্যা
nfa_did_time_out(শূন্য)
{
    যদি (*timeout_flag)
    {
	যদি (nfa_timed_out != ফাঁকা)
	{
# ifdef FEAT_EVAL
	    যদি (!*nfa_timed_out)
		ch_log(ফাঁকা, "NFA regexp timed out");
# endif
	    *nfa_timed_out = TRUE;
	}
	ফেরত TRUE;
    }
    ফেরত FALSE;
}
#endif

#ifdef ENABLE_LOG
    স্থির শূন্য
open_debug_log(পূর্ণসংখ্যা ফলাফল)
{
    log_fd = fopen(NFA_REGEXP_RUN_LOG, "a");
    যদি (log_fd == ফাঁকা)
    {
	emsg(_(e_log_open_failed));
	log_fd = stderr;
    }

    fprintf(log_fd, "****************************\n");
    fprintf(log_fd, "FINISHED RUNNING nfa_regmatch() recursively\n");
    fprintf(log_fd, "MATCH = %s\n", ফলাফল == TRUE ? "OK" : ফলাফল == MAYBE
	    ? "MAYBE" : "FALSE");
    fprintf(log_fd, "****************************\n");
}

    স্থির শূন্য
report_state(অক্ষর *action,
	     regsub_T *sub,
	     nfa_state_T *state,
	     পূর্ণসংখ্যা lid,
	     nfa_pim_T *pim)
{
    পূর্ণসংখ্যা col;

    যদি (sub->in_use <= 0)
	col = -1;
    অন্যথা যদি (REG_MULTI)
	col = sub->list.multi[0].start_col;
    অন্যথা
	col = (পূর্ণসংখ্যা)(sub->list.সারি[0].start - rex.সারি);
    nfa_set_code(state->c);
    যদি (log_fd == ফাঁকা)
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
    স্থির পূর্ণসংখ্যা
has_state_with_pos(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim)	// postponed match or NULL
{
    nfa_thread_T	*thread;
    পূর্ণসংখ্যা			i;

    জন্য (i = 0; i < l->n; ++i)
    {
	thread = &l->t[i];
	যদি (thread->state->id == state->id
		&& sub_equal(&thread->subs.norm, &subs->norm)
#ifdef FEAT_SYN_HL
		&& (!rex.nfa_has_zsubexpr
				|| sub_equal(&thread->subs.synt, &subs->synt))
#endif
		&& pim_equal(&thread->pim, pim))
	    ফেরত TRUE;
    }
    ফেরত FALSE;
}

/*
 * Return TRUE if "one" and "two" are equal.  That includes when both are not
 * set.
 */
    স্থির পূর্ণসংখ্যা
pim_equal(nfa_pim_T *one, nfa_pim_T *two)
{
    পূর্ণসংখ্যা one_unused = (one == ফাঁকা || one->ফলাফল == NFA_PIM_UNUSED);
    পূর্ণসংখ্যা two_unused = (two == ফাঁকা || two->ফলাফল == NFA_PIM_UNUSED);

    যদি (one_unused)
	// one is unused: equal when two is also unused
	ফেরত two_unused;
    যদি (two_unused)
	// one is used and two is not: not equal
	ফেরত FALSE;
    // compare the state id
    যদি (one->state->id != two->state->id)
	ফেরত FALSE;
    // compare the position
    যদি (REG_MULTI)
	ফেরত one->end.pos.lnum == two->end.pos.lnum
	    && one->end.pos.col == two->end.pos.col;
    ফেরত one->end.ptr == two->end.ptr;
}

/*
 * Return TRUE if "state" leads to a NFA_MATCH without advancing the input.
 */
    স্থির পূর্ণসংখ্যা
match_follows(nfa_state_T *startstate, পূর্ণসংখ্যা depth)
{
    nfa_state_T	    *state = startstate;

    // avoid too much recursion
    যদি (depth > 10)
	ফেরত FALSE;

    যখন (state != ফাঁকা)
    {
	নির্বাচন (state->c)
	{
	    ক্ষেত্র NFA_MATCH:
	    ক্ষেত্র NFA_MCLOSE:
	    ক্ষেত্র NFA_END_INVISIBLE:
	    ক্ষেত্র NFA_END_INVISIBLE_NEG:
	    ক্ষেত্র NFA_END_PATTERN:
		ফেরত TRUE;

	    ক্ষেত্র NFA_SPLIT:
		ফেরত match_follows(state->out, depth + 1)
				     || match_follows(state->out1, depth + 1);

	    ক্ষেত্র NFA_START_INVISIBLE:
	    ক্ষেত্র NFA_START_INVISIBLE_FIRST:
	    ক্ষেত্র NFA_START_INVISIBLE_BEFORE:
	    ক্ষেত্র NFA_START_INVISIBLE_BEFORE_FIRST:
	    ক্ষেত্র NFA_START_INVISIBLE_NEG:
	    ক্ষেত্র NFA_START_INVISIBLE_NEG_FIRST:
	    ক্ষেত্র NFA_START_INVISIBLE_BEFORE_NEG:
	    ক্ষেত্র NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
	    ক্ষেত্র NFA_COMPOSING:
		// skip ahead to next state
		state = state->out1->out;
		চালু;

	    ক্ষেত্র NFA_ANY:
	    ক্ষেত্র NFA_ANY_COMPOSING:
	    ক্ষেত্র NFA_IDENT:
	    ক্ষেত্র NFA_SIDENT:
	    ক্ষেত্র NFA_KWORD:
	    ক্ষেত্র NFA_SKWORD:
	    ক্ষেত্র NFA_FNAME:
	    ক্ষেত্র NFA_SFNAME:
	    ক্ষেত্র NFA_PRINT:
	    ক্ষেত্র NFA_SPRINT:
	    ক্ষেত্র NFA_WHITE:
	    ক্ষেত্র NFA_NWHITE:
	    ক্ষেত্র NFA_DIGIT:
	    ক্ষেত্র NFA_NDIGIT:
	    ক্ষেত্র NFA_HEX:
	    ক্ষেত্র NFA_NHEX:
	    ক্ষেত্র NFA_OCTAL:
	    ক্ষেত্র NFA_NOCTAL:
	    ক্ষেত্র NFA_WORD:
	    ক্ষেত্র NFA_NWORD:
	    ক্ষেত্র NFA_শীর্ষ:
	    ক্ষেত্র NFA_NHEAD:
	    ক্ষেত্র NFA_ALPHA:
	    ক্ষেত্র NFA_NALPHA:
	    ক্ষেত্র NFA_LOWER:
	    ক্ষেত্র NFA_NLOWER:
	    ক্ষেত্র NFA_UPPER:
	    ক্ষেত্র NFA_NUPPER:
	    ক্ষেত্র NFA_LOWER_IC:
	    ক্ষেত্র NFA_NLOWER_IC:
	    ক্ষেত্র NFA_UPPER_IC:
	    ক্ষেত্র NFA_NUPPER_IC:
	    ক্ষেত্র NFA_START_COLL:
	    ক্ষেত্র NFA_START_NEG_COLL:
	    ক্ষেত্র NFA_NEWL:
		// state will advance input
		ফেরত FALSE;

	    পূর্বনির্ধারিত:
		যদি (state->c > 0)
		    // state will advance input
		    ফেরত FALSE;

		// Others: zero-width or possibly zero-width, might still find
		// a match at the same position, keep looking.
		বিরতি;
	}
	state = state->out;
    }
    ফেরত FALSE;
}


/*
 * Return TRUE if "state" is already in list "l".
 */
    স্থির পূর্ণসংখ্যা
state_in_list(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs)	// pointers to subexpressions
{
    যদি (state->lastlist[nfa_ll_সূচক] == l->id)
    {
	যদি (!rex.nfa_has_backref || has_state_with_pos(l, state, subs, ফাঁকা))
	    ফেরত TRUE;
    }
    ফেরত FALSE;
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
    পূর্ণসংখ্যা			off_arg,    // byte offset, when -1 go to next line
    পূর্ণসংখ্যা			depth)	    // recursion depth
{
    পূর্ণসংখ্যা			subidx;
    পূর্ণসংখ্যা			off = off_arg;
    পূর্ণসংখ্যা			add_here = FALSE;
    পূর্ণসংখ্যা			listindex = 0;
    পূর্ণসংখ্যা			k;
    পূর্ণসংখ্যা			found = FALSE;
    nfa_thread_T	*thread;
    কাঠামো multipos	save_multipos;
    পূর্ণসংখ্যা			save_in_use;
    char_u		*save_ptr;
    পূর্ণসংখ্যা			i;
    regsub_T		*sub;
    regsubs_T		*subs = subs_arg;
    স্থির regsubs_T	অস্থায়ী_subs;
#ifdef ENABLE_LOG
    পূর্ণসংখ্যা			did_print = FALSE;
#endif

#ifdef FEAT_RELTIME
    যদি (nfa_did_time_out())
	ফেরত ফাঁকা;
#endif

    // This function is called recursively.  When the depth is too much we run
    // out of stack and crash, limit recursiveness here.
    যদি (depth >= 5000 || subs == ফাঁকা)
	ফেরত ফাঁকা;

    যদি (off_arg <= -ADDSTATE_HERE_OFFSET)
    {
	add_here = TRUE;
	off = 0;
	listindex = -(off_arg + ADDSTATE_HERE_OFFSET);
    }

    নির্বাচন (state->c)
    {
	ক্ষেত্র NFA_NCLOSE:
	ক্ষেত্র NFA_MCLOSE:
	ক্ষেত্র NFA_MCLOSE1:
	ক্ষেত্র NFA_MCLOSE2:
	ক্ষেত্র NFA_MCLOSE3:
	ক্ষেত্র NFA_MCLOSE4:
	ক্ষেত্র NFA_MCLOSE5:
	ক্ষেত্র NFA_MCLOSE6:
	ক্ষেত্র NFA_MCLOSE7:
	ক্ষেত্র NFA_MCLOSE8:
	ক্ষেত্র NFA_MCLOSE9:
#ifdef FEAT_SYN_HL
	ক্ষেত্র NFA_ZCLOSE:
	ক্ষেত্র NFA_ZCLOSE1:
	ক্ষেত্র NFA_ZCLOSE2:
	ক্ষেত্র NFA_ZCLOSE3:
	ক্ষেত্র NFA_ZCLOSE4:
	ক্ষেত্র NFA_ZCLOSE5:
	ক্ষেত্র NFA_ZCLOSE6:
	ক্ষেত্র NFA_ZCLOSE7:
	ক্ষেত্র NFA_ZCLOSE8:
	ক্ষেত্র NFA_ZCLOSE9:
#endif
	ক্ষেত্র NFA_MOPEN:
	ক্ষেত্র NFA_ZEND:
	ক্ষেত্র NFA_SPLIT:
	ক্ষেত্র NFA_EMPTY:
	    // These nodes are not added themselves but their "out" and/or
	    // "out1" may be added below.
	    বিরতি;

	ক্ষেত্র NFA_BOL:
	ক্ষেত্র NFA_BOF:
	    // "^" won't match past end-of-line, don't bother trying.
	    // Except when at the end of the line, or when we are going to the
	    // next line for a look-behind match.
	    যদি (rex.প্রবেশ > rex.সারি
		    && *rex.প্রবেশ != NUL
		    && (nfa_endp == ফাঁকা
			|| !REG_MULTI
			|| rex.lnum == nfa_endp->se_u.pos.lnum))
		যাও skip_add;
	    // FALLTHROUGH

	ক্ষেত্র NFA_MOPEN1:
	ক্ষেত্র NFA_MOPEN2:
	ক্ষেত্র NFA_MOPEN3:
	ক্ষেত্র NFA_MOPEN4:
	ক্ষেত্র NFA_MOPEN5:
	ক্ষেত্র NFA_MOPEN6:
	ক্ষেত্র NFA_MOPEN7:
	ক্ষেত্র NFA_MOPEN8:
	ক্ষেত্র NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	ক্ষেত্র NFA_ZOPEN:
	ক্ষেত্র NFA_ZOPEN1:
	ক্ষেত্র NFA_ZOPEN2:
	ক্ষেত্র NFA_ZOPEN3:
	ক্ষেত্র NFA_ZOPEN4:
	ক্ষেত্র NFA_ZOPEN5:
	ক্ষেত্র NFA_ZOPEN6:
	ক্ষেত্র NFA_ZOPEN7:
	ক্ষেত্র NFA_ZOPEN8:
	ক্ষেত্র NFA_ZOPEN9:
#endif
	ক্ষেত্র NFA_NOPEN:
	ক্ষেত্র NFA_ZSTART:
	    // These nodes need to be added so that we can bail out when it
	    // was added to this list before at the same position to avoid an
	    // endless loop for "\(\)*"

	পূর্বনির্ধারিত:
	    যদি (state->lastlist[nfa_ll_সূচক] == l->id && state->c != NFA_SKIP)
	    {
		// This state is already in the list, don't add it again,
		// unless it is an MOPEN that is used for a backreference or
		// when there is a PIM. For NFA_MATCH check the position,
		// lower position is preferred.
		যদি (!rex.nfa_has_backref && pim == ফাঁকা && !l->has_pim
						     && state->c != NFA_MATCH)
		{
		    // When called from addstate_here() do insert before
		    // existing states.
		    যদি (add_here)
		    {
			জন্য (k = 0; k < l->n && k < listindex; ++k)
			    যদি (l->t[k].state->id == state->id)
			    {
				found = TRUE;
				বিরতি;
			    }
		    }
		    যদি (!add_here || found)
		    {
skip_add:
#ifdef ENABLE_LOG
			nfa_set_code(state->c);
			fprintf(log_fd, "> Not adding state %d to list %d. char %d: %s pim: %s has_pim: %d found: %d\n",
			    abs(state->id), l->id, state->c, code,
			    pim == ফাঁকা ? "NULL" : "yes", l->has_pim, found);
#endif
			ফেরত subs;
		    }
		}

		// Do not add the state again when it exists with the same
		// positions.
		যদি (has_state_with_pos(l, state, subs, pim))
		    যাও skip_add;
	    }

	    // When there are backreferences or PIMs the number of states may
	    // be (a lot) bigger than anticipated.
	    যদি (l->n == l->len)
	    {
		পূর্ণসংখ্যা		newlen = l->len * 3 / 2 + 50;
		size_t		newsize = newlen * আকার(nfa_thread_T);
		nfa_thread_T	*newt;

		যদি ((long)(newsize >> 10) >= p_mmp)
		{
		    emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		    ফেরত ফাঁকা;
		}
		যদি (subs != &অস্থায়ী_subs)
		{
		    // "subs" may point into the current array, need to make a
		    // copy before it becomes invalid.
		    copy_sub(&অস্থায়ী_subs.norm, &subs->norm);
#ifdef FEAT_SYN_HL
		    যদি (rex.nfa_has_zsubexpr)
			copy_sub(&অস্থায়ী_subs.synt, &subs->synt);
#endif
		    subs = &অস্থায়ী_subs;
		}

		newt = vim_realloc(l->t, newsize);
		যদি (newt == ফাঁকা)
		{
		    // out of memory
		    ফেরত ফাঁকা;
		}
		l->t = newt;
		l->len = newlen;
	    }

	    // add the state to the list
	    state->lastlist[nfa_ll_সূচক] = l->id;
	    thread = &l->t[l->n++];
	    thread->state = state;
	    যদি (pim == ফাঁকা)
		thread->pim.ফলাফল = NFA_PIM_UNUSED;
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
    নির্বাচন (state->c)
    {
	ক্ষেত্র NFA_MATCH:
	    বিরতি;

	ক্ষেত্র NFA_SPLIT:
	    // order matters here
	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    subs = addstate(l, state->out1, subs, pim, off_arg, depth + 1);
	    বিরতি;

	ক্ষেত্র NFA_EMPTY:
	ক্ষেত্র NFA_NOPEN:
	ক্ষেত্র NFA_NCLOSE:
	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    বিরতি;

	ক্ষেত্র NFA_MOPEN:
	ক্ষেত্র NFA_MOPEN1:
	ক্ষেত্র NFA_MOPEN2:
	ক্ষেত্র NFA_MOPEN3:
	ক্ষেত্র NFA_MOPEN4:
	ক্ষেত্র NFA_MOPEN5:
	ক্ষেত্র NFA_MOPEN6:
	ক্ষেত্র NFA_MOPEN7:
	ক্ষেত্র NFA_MOPEN8:
	ক্ষেত্র NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	ক্ষেত্র NFA_ZOPEN:
	ক্ষেত্র NFA_ZOPEN1:
	ক্ষেত্র NFA_ZOPEN2:
	ক্ষেত্র NFA_ZOPEN3:
	ক্ষেত্র NFA_ZOPEN4:
	ক্ষেত্র NFA_ZOPEN5:
	ক্ষেত্র NFA_ZOPEN6:
	ক্ষেত্র NFA_ZOPEN7:
	ক্ষেত্র NFA_ZOPEN8:
	ক্ষেত্র NFA_ZOPEN9:
#endif
	ক্ষেত্র NFA_ZSTART:
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
	    save_ptr = ফাঁকা;
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
		    জন্য (i = sub->in_use; i < subidx; ++i)
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
					 (colnr_T)(rex.প্রবেশ - rex.সারি + off);
		}
		sub->list.multi[subidx].end_lnum = -1;
	    }
	    অন্যথা
	    {
		যদি (subidx < sub->in_use)
		{
		    save_ptr = sub->list.সারি[subidx].start;
		    save_in_use = -1;
		}
		অন্যথা
		{
		    save_in_use = sub->in_use;
		    জন্য (i = sub->in_use; i < subidx; ++i)
		    {
			sub->list.সারি[i].start = ফাঁকা;
			sub->list.সারি[i].end = ফাঁকা;
		    }
		    sub->in_use = subidx + 1;
		}
		sub->list.সারি[subidx].start = rex.প্রবেশ + off;
	    }

	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    যদি (subs == ফাঁকা)
		বিরতি;
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
		    sub->list.সারি[subidx].start = save_ptr;
	    }
	    অন্যথা
		sub->in_use = save_in_use;
	    বিরতি;

	ক্ষেত্র NFA_MCLOSE:
	    যদি (rex.nfa_has_zend && (REG_MULTI
			? subs->norm.list.multi[0].end_lnum >= 0
			: subs->norm.list.সারি[0].end != ফাঁকা))
	    {
		// Do not overwrite the position set by \ze.
		subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
		বিরতি;
	    }
	    // FALLTHROUGH
	ক্ষেত্র NFA_MCLOSE1:
	ক্ষেত্র NFA_MCLOSE2:
	ক্ষেত্র NFA_MCLOSE3:
	ক্ষেত্র NFA_MCLOSE4:
	ক্ষেত্র NFA_MCLOSE5:
	ক্ষেত্র NFA_MCLOSE6:
	ক্ষেত্র NFA_MCLOSE7:
	ক্ষেত্র NFA_MCLOSE8:
	ক্ষেত্র NFA_MCLOSE9:
#ifdef FEAT_SYN_HL
	ক্ষেত্র NFA_ZCLOSE:
	ক্ষেত্র NFA_ZCLOSE1:
	ক্ষেত্র NFA_ZCLOSE2:
	ক্ষেত্র NFA_ZCLOSE3:
	ক্ষেত্র NFA_ZCLOSE4:
	ক্ষেত্র NFA_ZCLOSE5:
	ক্ষেত্র NFA_ZCLOSE6:
	ক্ষেত্র NFA_ZCLOSE7:
	ক্ষেত্র NFA_ZCLOSE8:
	ক্ষেত্র NFA_ZCLOSE9:
#endif
	ক্ষেত্র NFA_ZEND:
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
					  (colnr_T)(rex.প্রবেশ - rex.সারি + off);
		}
		// avoid compiler warnings
		save_ptr = ফাঁকা;
	    }
	    অন্যথা
	    {
		save_ptr = sub->list.সারি[subidx].end;
		sub->list.সারি[subidx].end = rex.প্রবেশ + off;
		// avoid compiler warnings
		CLEAR_FIELD(save_multipos);
	    }

	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    যদি (subs == ফাঁকা)
		বিরতি;
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
		sub->list.সারি[subidx].end = save_ptr;
	    sub->in_use = save_in_use;
	    বিরতি;
    }
    ফেরত subs;
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
    পূর্ণসংখ্যা			*ip)
{
    পূর্ণসংখ্যা tlen = l->n;
    পূর্ণসংখ্যা গুনতি;
    পূর্ণসংখ্যা listidx = *ip;
    regsubs_T *r;

    // First add the state(s) at the end, so that we know how many there are.
    // Pass the listidx as offset (avoids adding another argument to
    // addstate()).
    r = addstate(l, state, subs, pim, -listidx - ADDSTATE_HERE_OFFSET, 0);
    যদি (r == ফাঁকা)
	ফেরত ফাঁকা;

    // when "*ip" was at the end of the list, nothing to do
    যদি (listidx + 1 == tlen)
	ফেরত r;

    // re-order to put the new state at the current position
    গুনতি = l->n - tlen;
    যদি (গুনতি == 0)
	ফেরত r; // no state got added
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
	    পূর্ণসংখ্যা		    newlen = l->len * 3 / 2 + 50;
	    size_t	    newsize = newlen * আকার(nfa_thread_T);
	    nfa_thread_T    *newl;

	    যদি ((long)(newsize >> 10) >= p_mmp)
	    {
		emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		ফেরত ফাঁকা;
	    }
	    newl = alloc(newsize);
	    যদি (newl == ফাঁকা)
		ফেরত ফাঁকা;
	    l->len = newlen;
	    mch_memmove(&(newl[0]),
		    &(l->t[0]),
		    আকার(nfa_thread_T) * listidx);
	    mch_memmove(&(newl[listidx]),
		    &(l->t[l->n - গুনতি]),
		    আকার(nfa_thread_T) * গুনতি);
	    mch_memmove(&(newl[listidx + গুনতি]),
		    &(l->t[listidx + 1]),
		    আকার(nfa_thread_T) * (l->n - গুনতি - listidx - 1));
	    vim_free(l->t);
	    l->t = newl;
	}
	অন্যথা
	{
	    // make space for new states, then move them from the
	    // end to the current position
	    mch_memmove(&(l->t[listidx + গুনতি]),
		    &(l->t[listidx + 1]),
		    আকার(nfa_thread_T) * (l->n - listidx - 1));
	    mch_memmove(&(l->t[listidx]),
		    &(l->t[l->n - 1]),
		    আকার(nfa_thread_T) * গুনতি);
	}
    }
    --l->n;
    *ip = listidx - 1;

    ফেরত r;
}

/*
 * Check character class "class" against current character c.
 */
    স্থির পূর্ণসংখ্যা
check_char_class(পূর্ণসংখ্যা class, পূর্ণসংখ্যা c)
{
    নির্বাচন (class)
    {
	ক্ষেত্র NFA_CLASS_ALNUM:
	    যদি (c >= 1 && c < 128 && isalnum(c))
		ফেরত OK;
	    বিরতি;
	ক্ষেত্র NFA_CLASS_ALPHA:
	    যদি (c >= 1 && c < 128 && isalpha(c))
		ফেরত OK;
	    বিরতি;
	ক্ষেত্র NFA_CLASS_BLANK:
	    যদি (c == ' ' || c == '\t')
		ফেরত OK;
	    বিরতি;
	ক্ষেত্র NFA_CLASS_CNTRL:
	    যদি (c >= 1 && c <= 127 && iscntrl(c))
		ফেরত OK;
	    বিরতি;
	ক্ষেত্র NFA_CLASS_DIGIT:
	    যদি (VIM_ISDIGIT(c))
		ফেরত OK;
	    বিরতি;
	ক্ষেত্র NFA_CLASS_GRAPH:
	    যদি (c >= 1 && c <= 127 && isgraph(c))
		ফেরত OK;
	    বিরতি;
	ক্ষেত্র NFA_CLASS_LOWER:
	    যদি (MB_ISLOWER(c) && c != 170 && c != 186)
		ফেরত OK;
	    বিরতি;
	ক্ষেত্র NFA_CLASS_PRINT:
	    যদি (vim_isprintc(c))
		ফেরত OK;
	    বিরতি;
	ক্ষেত্র NFA_CLASS_PUNCT:
	    যদি (c >= 1 && c < 128 && ispunct(c))
		ফেরত OK;
	    বিরতি;
	ক্ষেত্র NFA_CLASS_SPACE:
	    যদি ((c >= 9 && c <= 13) || (c == ' '))
		ফেরত OK;
	    বিরতি;
	ক্ষেত্র NFA_CLASS_UPPER:
	    যদি (MB_ISUPPER(c))
		ফেরত OK;
	    বিরতি;
	ক্ষেত্র NFA_CLASS_XDIGIT:
	    যদি (vim_isxdigit(c))
		ফেরত OK;
	    বিরতি;
	ক্ষেত্র NFA_CLASS_TAB:
	    যদি (c == '\t')
		ফেরত OK;
	    বিরতি;
	ক্ষেত্র NFA_CLASS_RETURN:
	    যদি (c == '\r')
		ফেরত OK;
	    বিরতি;
	ক্ষেত্র NFA_CLASS_BACKSPACE:
	    যদি (c == '\b')
		ফেরত OK;
	    বিরতি;
	ক্ষেত্র NFA_CLASS_ESCAPE:
	    যদি (c == '\033')
		ফেরত OK;
	    বিরতি;
	ক্ষেত্র NFA_CLASS_IDENT:
	    যদি (vim_isIDc(c))
		ফেরত OK;
	    বিরতি;
	ক্ষেত্র NFA_CLASS_KEYWORD:
	    যদি (reg_iswordc(c))
		ফেরত OK;
	    বিরতি;
	ক্ষেত্র NFA_CLASS_FNAME:
	    যদি (vim_isfilec(c))
		ফেরত OK;
	    বিরতি;

	পূর্বনির্ধারিত:
	    // should not be here :P
	    siemsg(_(e_nfa_regexp_invalid_character_class_nr), class);
	    ফেরত FAIL;
    }
    ফেরত FAIL;
}

/*
 * Check for a match with subexpression "subidx".
 * Return TRUE if it matches.
 */
    স্থির পূর্ণসংখ্যা
match_backref(
    regsub_T	*sub,	    // pointers to subexpressions
    পূর্ণসংখ্যা		subidx,
    পূর্ণসংখ্যা		*bytelen)   // out: length of match in bytes
{
    পূর্ণসংখ্যা		len;

    যদি (sub->in_use <= subidx)
    {
retempty:
	// backref was not set, match an empty string
	*bytelen = 0;
	ফেরত TRUE;
    }

    যদি (REG_MULTI)
    {
	যদি (sub->list.multi[subidx].start_lnum < 0
				       || sub->list.multi[subidx].end_lnum < 0)
	    যাও retempty;
	যদি (sub->list.multi[subidx].start_lnum == rex.lnum
			       && sub->list.multi[subidx].end_lnum == rex.lnum)
	{
	    len = sub->list.multi[subidx].end_col
					  - sub->list.multi[subidx].start_col;
	    যদি (cstrncmp(rex.সারি + sub->list.multi[subidx].start_col,
							 rex.প্রবেশ, &len) == 0)
	    {
		*bytelen = len;
		ফেরত TRUE;
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
		ফেরত TRUE;
	}
    }
    অন্যথা
    {
	যদি (sub->list.সারি[subidx].start == ফাঁকা
					|| sub->list.সারি[subidx].end == ফাঁকা)
	    যাও retempty;
	len = (পূর্ণসংখ্যা)(sub->list.সারি[subidx].end - sub->list.সারি[subidx].start);
	যদি (cstrncmp(sub->list.সারি[subidx].start, rex.প্রবেশ, &len) == 0)
	{
	    *bytelen = len;
	    ফেরত TRUE;
	}
    }
    ফেরত FALSE;
}

#ifdef FEAT_SYN_HL

/*
 * Check for a match with \z subexpression "subidx".
 * Return TRUE if it matches.
 */
    স্থির পূর্ণসংখ্যা
match_zref(
    পূর্ণসংখ্যা		subidx,
    পূর্ণসংখ্যা		*bytelen)   // out: length of match in bytes
{
    পূর্ণসংখ্যা		len;

    cleanup_zsubexpr();
    যদি (re_extmatch_in == ফাঁকা || re_extmatch_in->matches[subidx] == ফাঁকা)
    {
	// backref was not set, match an empty string
	*bytelen = 0;
	ফেরত TRUE;
    }

    len = (পূর্ণসংখ্যা)STRLEN(re_extmatch_in->matches[subidx]);
    যদি (cstrncmp(re_extmatch_in->matches[subidx], rex.প্রবেশ, &len) == 0)
    {
	*bytelen = len;
	ফেরত TRUE;
    }
    ফেরত FALSE;
}
#endif

/*
 * Save list IDs for all NFA states of "prog" into "list".
 * Also reset the IDs to zero.
 * Only used for the recursive value lastlist[1].
 */
    স্থির শূন্য
nfa_save_listids(nfa_regprog_T *prog, পূর্ণসংখ্যা *list)
{
    পূর্ণসংখ্যা		    i;
    nfa_state_T	    *p;

    // Order in the list is reverse, it's a bit faster that way.
    p = &prog->state[0];
    জন্য (i = prog->nstate; --i >= 0; )
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
nfa_restore_listids(nfa_regprog_T *prog, পূর্ণসংখ্যা *list)
{
    পূর্ণসংখ্যা		    i;
    nfa_state_T	    *p;

    p = &prog->state[0];
    জন্য (i = prog->nstate; --i >= 0; )
    {
	p->lastlist[1] = list[i];
	++p;
    }
}

    স্থির পূর্ণসংখ্যা
nfa_re_অঙ্ক_cmp(long_u val, পূর্ণসংখ্যা op, long_u pos)
{
    যদি (op == 1) ফেরত pos > val;
    যদি (op == 2) ফেরত pos < val;
    ফেরত val == pos;
}

স্থির পূর্ণসংখ্যা nfa_regmatch(nfa_regprog_T *prog, nfa_state_T *start, regsubs_T *submatch, regsubs_T *m);

/*
 * Recursively call nfa_regmatch()
 * "pim" is NULL or contains info about a Postponed Invisible Match (start
 * position).
 */
    স্থির পূর্ণসংখ্যা
recursive_regmatch(
    nfa_state_T	    *state,
    nfa_pim_T	    *pim,
    nfa_regprog_T   *prog,
    regsubs_T	    *submatch,
    regsubs_T	    *m,
    পূর্ণসংখ্যা		    **listids,
    পূর্ণসংখ্যা		    *listids_len)
{
    পূর্ণসংখ্যা		save_reginput_col = (পূর্ণসংখ্যা)(rex.প্রবেশ - rex.সারি);
    পূর্ণসংখ্যা		save_reglnum = rex.lnum;
    পূর্ণসংখ্যা		save_nfa_match = nfa_match;
    পূর্ণসংখ্যা		save_nfa_listid = rex.nfa_listid;
    save_se_T   *save_nfa_endp = nfa_endp;
    save_se_T   endpos;
    save_se_T   *endposp = ফাঁকা;
    পূর্ণসংখ্যা		ফলাফল;
    পূর্ণসংখ্যা		need_restore = FALSE;

    যদি (pim != ফাঁকা)
    {
	// start at the position where the postponed match was
	যদি (REG_MULTI)
	    rex.প্রবেশ = rex.সারি + pim->end.pos.col;
	অন্যথা
	    rex.প্রবেশ = pim->end.ptr;
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
	    যদি (pim == ফাঁকা)
	    {
		endpos.se_u.pos.col = (পূর্ণসংখ্যা)(rex.প্রবেশ - rex.সারি);
		endpos.se_u.pos.lnum = rex.lnum;
	    }
	    অন্যথা
		endpos.se_u.pos = pim->end.pos;
	}
	অন্যথা
	{
	    যদি (pim == ফাঁকা)
		endpos.se_u.ptr = rex.প্রবেশ;
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
		rex.সারি = reg_getline(--rex.lnum);
		যদি (rex.সারি == ফাঁকা)
		    // can't go before the first line
		    rex.সারি = reg_getline(++rex.lnum);
	    }
	    rex.প্রবেশ = rex.সারি;
	}
	অন্যথা
	{
	    যদি (REG_MULTI && (পূর্ণসংখ্যা)(rex.প্রবেশ - rex.সারি) < state->val)
	    {
		// Not enough bytes in this line, go to end of
		// previous line.
		rex.সারি = reg_getline(--rex.lnum);
		যদি (rex.সারি == ফাঁকা)
		{
		    // can't go before the first line
		    rex.সারি = reg_getline(++rex.lnum);
		    rex.প্রবেশ = rex.সারি;
		}
		অন্যথা
		    rex.প্রবেশ = rex.সারি + reg_getline_len(rex.lnum);
	    }
	    যদি ((পূর্ণসংখ্যা)(rex.প্রবেশ - rex.সারি) >= state->val)
	    {
		rex.প্রবেশ -= state->val;
		যদি (has_mbyte)
		    rex.প্রবেশ -= mb_শীর্ষ_off(rex.সারি, rex.প্রবেশ);
	    }
	    অন্যথা
		rex.প্রবেশ = rex.সারি;
	}
    }

#ifdef ENABLE_LOG
    যদি (log_fd != stderr)
	fclose(log_fd);
    log_fd = ফাঁকা;
#endif
    // Have to clear the lastlist field of the NFA nodes, so that
    // nfa_regmatch() and addstate() can run properly after recursion.
    যদি (nfa_ll_সূচক == 1)
    {
	// Already calling nfa_regmatch() recursively.  Save the lastlist[1]
	// values and clear them.
	যদি (*listids == ফাঁকা || *listids_len < prog->nstate)
	{
	    vim_free(*listids);
	    *listids = ALLOC_MULT(পূর্ণসংখ্যা, prog->nstate);
	    যদি (*listids == ফাঁকা)
	    {
		emsg(_(e_nfa_regexp_could_not_allocate_memory_for_branch_traversal));
		ফেরত 0;
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
	++nfa_ll_সূচক;
	যদি (rex.nfa_listid <= rex.nfa_alt_listid)
	    rex.nfa_listid = rex.nfa_alt_listid;
    }

    // Call nfa_regmatch() to check if the current concat matches at this
    // position. The concat ends with the node NFA_END_INVISIBLE
    nfa_endp = endposp;
    ফলাফল = nfa_regmatch(prog, state->out, submatch, m);

    যদি (need_restore)
	nfa_restore_listids(prog, *listids);
    অন্যথা
    {
	--nfa_ll_সূচক;
	rex.nfa_alt_listid = rex.nfa_listid;
    }

    // restore position in input text
    rex.lnum = save_reglnum;
    যদি (REG_MULTI)
	rex.সারি = reg_getline(rex.lnum);
    rex.প্রবেশ = rex.সারি + save_reginput_col;
    যদি (ফলাফল != NFA_TOO_EXPENSIVE)
    {
	nfa_match = save_nfa_match;
	rex.nfa_listid = save_nfa_listid;
    }
    nfa_endp = save_nfa_endp;

#ifdef ENABLE_LOG
    open_debug_log(ফলাফল);
#endif

    ফেরত ফলাফল;
}

/*
 * Estimate the chance of a match with "state" failing.
 * empty match: 0
 * NFA_ANY: 1
 * specific character: 99
 */
    স্থির পূর্ণসংখ্যা
failure_chance(nfa_state_T *state, পূর্ণসংখ্যা depth)
{
    পূর্ণসংখ্যা c = state->c;
    পূর্ণসংখ্যা l, r;

    // detect looping
    যদি (depth > 4)
	ফেরত 1;

    নির্বাচন (c)
    {
	ক্ষেত্র NFA_SPLIT:
	    যদি (state->out->c == NFA_SPLIT || state->out1->c == NFA_SPLIT)
		// avoid recursive stuff
		ফেরত 1;
	    // two alternatives, use the lowest failure chance
	    l = failure_chance(state->out, depth + 1);
	    r = failure_chance(state->out1, depth + 1);
	    ফেরত l < r ? l : r;

	ক্ষেত্র NFA_ANY:
	    // matches anything, unlikely to fail
	    ফেরত 1;

	ক্ষেত্র NFA_MATCH:
	ক্ষেত্র NFA_MCLOSE:
	ক্ষেত্র NFA_ANY_COMPOSING:
	    // empty match works always
	    ফেরত 0;

	ক্ষেত্র NFA_START_INVISIBLE:
	ক্ষেত্র NFA_START_INVISIBLE_FIRST:
	ক্ষেত্র NFA_START_INVISIBLE_NEG:
	ক্ষেত্র NFA_START_INVISIBLE_NEG_FIRST:
	ক্ষেত্র NFA_START_INVISIBLE_BEFORE:
	ক্ষেত্র NFA_START_INVISIBLE_BEFORE_FIRST:
	ক্ষেত্র NFA_START_INVISIBLE_BEFORE_NEG:
	ক্ষেত্র NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
	ক্ষেত্র NFA_START_PATTERN:
	    // recursive regmatch is expensive, use low failure chance
	    ফেরত 5;

	ক্ষেত্র NFA_BOL:
	ক্ষেত্র NFA_EOL:
	ক্ষেত্র NFA_BOF:
	ক্ষেত্র NFA_EOF:
	ক্ষেত্র NFA_NEWL:
	    ফেরত 99;

	ক্ষেত্র NFA_BOW:
	ক্ষেত্র NFA_EOW:
	    ফেরত 90;

	ক্ষেত্র NFA_MOPEN:
	ক্ষেত্র NFA_MOPEN1:
	ক্ষেত্র NFA_MOPEN2:
	ক্ষেত্র NFA_MOPEN3:
	ক্ষেত্র NFA_MOPEN4:
	ক্ষেত্র NFA_MOPEN5:
	ক্ষেত্র NFA_MOPEN6:
	ক্ষেত্র NFA_MOPEN7:
	ক্ষেত্র NFA_MOPEN8:
	ক্ষেত্র NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	ক্ষেত্র NFA_ZOPEN:
	ক্ষেত্র NFA_ZOPEN1:
	ক্ষেত্র NFA_ZOPEN2:
	ক্ষেত্র NFA_ZOPEN3:
	ক্ষেত্র NFA_ZOPEN4:
	ক্ষেত্র NFA_ZOPEN5:
	ক্ষেত্র NFA_ZOPEN6:
	ক্ষেত্র NFA_ZOPEN7:
	ক্ষেত্র NFA_ZOPEN8:
	ক্ষেত্র NFA_ZOPEN9:
	ক্ষেত্র NFA_ZCLOSE:
	ক্ষেত্র NFA_ZCLOSE1:
	ক্ষেত্র NFA_ZCLOSE2:
	ক্ষেত্র NFA_ZCLOSE3:
	ক্ষেত্র NFA_ZCLOSE4:
	ক্ষেত্র NFA_ZCLOSE5:
	ক্ষেত্র NFA_ZCLOSE6:
	ক্ষেত্র NFA_ZCLOSE7:
	ক্ষেত্র NFA_ZCLOSE8:
	ক্ষেত্র NFA_ZCLOSE9:
#endif
	ক্ষেত্র NFA_NOPEN:
	ক্ষেত্র NFA_MCLOSE1:
	ক্ষেত্র NFA_MCLOSE2:
	ক্ষেত্র NFA_MCLOSE3:
	ক্ষেত্র NFA_MCLOSE4:
	ক্ষেত্র NFA_MCLOSE5:
	ক্ষেত্র NFA_MCLOSE6:
	ক্ষেত্র NFA_MCLOSE7:
	ক্ষেত্র NFA_MCLOSE8:
	ক্ষেত্র NFA_MCLOSE9:
	ক্ষেত্র NFA_NCLOSE:
	    ফেরত failure_chance(state->out, depth + 1);

	ক্ষেত্র NFA_BACKREF1:
	ক্ষেত্র NFA_BACKREF2:
	ক্ষেত্র NFA_BACKREF3:
	ক্ষেত্র NFA_BACKREF4:
	ক্ষেত্র NFA_BACKREF5:
	ক্ষেত্র NFA_BACKREF6:
	ক্ষেত্র NFA_BACKREF7:
	ক্ষেত্র NFA_BACKREF8:
	ক্ষেত্র NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	ক্ষেত্র NFA_ZREF1:
	ক্ষেত্র NFA_ZREF2:
	ক্ষেত্র NFA_ZREF3:
	ক্ষেত্র NFA_ZREF4:
	ক্ষেত্র NFA_ZREF5:
	ক্ষেত্র NFA_ZREF6:
	ক্ষেত্র NFA_ZREF7:
	ক্ষেত্র NFA_ZREF8:
	ক্ষেত্র NFA_ZREF9:
#endif
	    // backreferences don't match in many places
	    ফেরত 94;

	ক্ষেত্র NFA_LNUM_GT:
	ক্ষেত্র NFA_LNUM_LT:
	ক্ষেত্র NFA_COL_GT:
	ক্ষেত্র NFA_COL_LT:
	ক্ষেত্র NFA_VCOL_GT:
	ক্ষেত্র NFA_VCOL_LT:
	ক্ষেত্র NFA_MARK_GT:
	ক্ষেত্র NFA_MARK_LT:
	ক্ষেত্র NFA_VISUAL:
	    // before/after positions don't match very often
	    ফেরত 85;

	ক্ষেত্র NFA_LNUM:
	    ফেরত 90;

	ক্ষেত্র NFA_CURSOR:
	ক্ষেত্র NFA_COL:
	ক্ষেত্র NFA_VCOL:
	ক্ষেত্র NFA_MARK:
	    // specific positions rarely match
	    ফেরত 98;

	ক্ষেত্র NFA_COMPOSING:
	    ফেরত 95;

	পূর্বনির্ধারিত:
	    যদি (c > 0)
		// character match fails often
		ফেরত 95;
    }

    // something else, includes character classes
    ফেরত 50;
}

/*
 * Skip until the char "c" we know a match must start with.
 */
    স্থির পূর্ণসংখ্যা
skip_to_start(পূর্ণসংখ্যা c, colnr_T *colp)
{
    char_u *s;

    // Used often, do some work to avoid call overhead.
    যদি (!rex.reg_ic && !has_mbyte)
	s = vim_strbyte(rex.সারি + *colp, c);
    অন্যথা
	s = cstrchr(rex.সারি + *colp, c);
    যদি (s == ফাঁকা)
	ফেরত FAIL;
    *colp = (পূর্ণসংখ্যা)(s - rex.সারি);
    ফেরত OK;
}

/*
 * Check for a match with match_text.
 * Called after skip_to_start() has found regstart.
 * Returns zero for no match, 1 for a match.
 */
    স্থির long
find_match_text(colnr_T *startcol, পূর্ণসংখ্যা regstart, char_u *match_text)
{
    colnr_T col = *startcol;
    পূর্ণসংখ্যা	    c1, c2;
    পূর্ণসংখ্যা	    len1, len2;
    পূর্ণসংখ্যা	    match;

    জন্য (;;)
    {
	match = TRUE;
	// Skip the input character: case-folding can change its byte length.
	len2 = MB_CPTR2LEN(rex.সারি + col);
	জন্য (len1 = 0; match_text[len1] != NUL; len1 += MB_CHAR2LEN(c1))
	{
	    c1 = PTR2CHAR(match_text + len1);
	    c2 = PTR2CHAR(rex.সারি + col + len2);
	    যদি (c1 != c2 && (!rex.reg_ic || MB_CASEFOLD(c1) != MB_CASEFOLD(c2)))
	    {
		match = FALSE;
		বিরতি;
	    }
	    len2 += enc_utf8 ? utf_ptr2len(rex.সারি + col + len2)
							     : MB_CHAR2LEN(c2);
	}
	যদি (match
		// check that no composing char follows
		&& !(enc_utf8
			  && utf_iscomposing(PTR2CHAR(rex.সারি + col + len2))))
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
		rex.reg_startp[0] = rex.সারি + col;
		rex.reg_endp[0] = rex.সারি + col + len2;
	    }
	    *startcol = col;
	    ফেরত 1L;
	}

	// Try finding regstart after the current match.
	col += MB_CPTR2LEN(rex.সারি + col); // skip the input character
	যদি (skip_to_start(regstart, &col) == FAIL)
	    বিরতি;
    }

    *startcol = col;
    ফেরত 0L;
}

/*
 * Check whether the composing characters at the current input position match
 * the NFA composing sub-expression that starts at "sta" (the first state
 * after NFA_COMPOSING).  "curc" is the base character and "clen" its byte
 * length.
 * Returns OK when it matches, FAIL otherwise.
 */
    স্থির পূর্ণসংখ্যা
match_composing(nfa_state_T *sta, পূর্ণসংখ্যা curc, পূর্ণসংখ্যা clen)
{
    পূর্ণসংখ্যা		mc = curc;
    পূর্ণসংখ্যা		len = 0;
    পূর্ণসংখ্যা		cchars[MAX_MCO];
    পূর্ণসংখ্যা		ccount = 0;
    পূর্ণসংখ্যা		j;

    যদি (utf_iscomposing(sta->c))
	// Only match composing character(s), ignore base character.  Used
	// for ".{composing}" and "{composing}" (no preceding character).
	len += mb_char2len(mc);

    যদি (rex.reg_icombine && len == 0)
	// If \Z was present, then ignore composing characters.  When
	// ignoring the base character this always matches.
	ফেরত sta->c == curc ? OK : FAIL;

    // Check base character matches first, unless ignored.
    যদি (len == 0 && mc != sta->c)
	ফেরত FAIL;

    যদি (len == 0)
    {
	len += mb_char2len(mc);
	sta = sta->out;
    }

    // We don't care about the order of composing characters.  Get them into
    // cchars[] first.
    যখন (len < clen)
    {
	mc = mb_ptr2char(rex.প্রবেশ + len);
	cchars[ccount++] = mc;
	len += mb_char2len(mc);
	যদি (ccount == MAX_MCO)
	    বিরতি;
    }

    // Check that each composing char in the pattern matches a composing char
    // in the text.  We do not check if all composing chars are matched.
    যখন (sta->c != NFA_END_COMPOSING)
    {
	জন্য (j = 0; j < ccount; ++j)
	    যদি (cchars[j] == sta->c)
		বিরতি;
	যদি (j == ccount)
	    ফেরত FAIL;
	sta = sta->out;
    }

    ফেরত OK;
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
    স্থির পূর্ণসংখ্যা
nfa_regmatch(
    nfa_regprog_T	*prog,
    nfa_state_T		*start,
    regsubs_T		*submatch,
    regsubs_T		*m)
{
    পূর্ণসংখ্যা		ফলাফল = FALSE;
    size_t	আকার_2 = 0;
    পূর্ণসংখ্যা		flag = 0;
    পূর্ণসংখ্যা		go_to_nextline = FALSE;
    nfa_thread_T *t;
    nfa_list_T	list[2];
    পূর্ণসংখ্যা		listidx;
    nfa_list_T	*thislist;
    nfa_list_T	*nextlist;
    পূর্ণসংখ্যা		*listids = ফাঁকা;
    পূর্ণসংখ্যা		listids_len = 0;
    nfa_state_T *add_state;
    পূর্ণসংখ্যা		add_here;
    পূর্ণসংখ্যা		add_গুনতি;
    পূর্ণসংখ্যা		add_off = 0;
    পূর্ণসংখ্যা		toplevel = start->c == NFA_MOPEN;
    regsubs_T	*r;
#ifdef NFA_REGEXP_DEBUG_LOG
    FILE	*debug;
#endif

    // Some patterns may take a long time to match, especially when using
    // recursive_regmatch(). Allow interrupting them with CTRL-C.
    fast_breakcheck();
    যদি (got_int)
	ফেরত FALSE;
#ifdef FEAT_RELTIME
    যদি (nfa_did_time_out())
	ফেরত FALSE;
#endif

#ifdef NFA_REGEXP_DEBUG_LOG
    debug = fopen(NFA_REGEXP_DEBUG_LOG, "a");
    যদি (debug == ফাঁকা)
    {
	semsg("(NFA) COULD NOT OPEN %s!", NFA_REGEXP_DEBUG_LOG);
	ফেরত FALSE;
    }
#endif
    nfa_match = FALSE;

    // Allocate memory for the lists of nodes.
    আকার_2 = (prog->nstate + 1) * আকার(nfa_thread_T);

    // Reuse cached list buffers from prog when available (top-level call).
    // Recursive calls must allocate their own buffers.
    যদি (toplevel && prog->listbuf[0] != ফাঁকা)
    {
	list[0].t = (nfa_thread_T *)prog->listbuf[0];
	list[1].t = (nfa_thread_T *)prog->listbuf[1];
	prog->listbuf[0] = ফাঁকা;
	prog->listbuf[1] = ফাঁকা;
    }
    অন্যথা
    {
	list[0].t = alloc(আকার_2);
	list[1].t = alloc(আকার_2);
    }
    list[0].len = prog->nstate + 1;
    list[1].len = prog->nstate + 1;
    যদি (list[0].t == ফাঁকা || list[1].t == ফাঁকা)
	যাও theend;

#ifdef ENABLE_LOG
    log_fd = fopen(NFA_REGEXP_RUN_LOG, "a");
    যদি (log_fd == ফাঁকা)
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
	    m->norm.list.multi[0].start_col = (colnr_T)(rex.প্রবেশ - rex.সারি);
	    m->norm.orig_start_col = m->norm.list.multi[0].start_col;
	}
	অন্যথা
	    m->norm.list.সারি[0].start = rex.প্রবেশ;
	m->norm.in_use = 1;
	r = addstate(thislist, start->out, m, ফাঁকা, 0, 0);
    }
    অন্যথা
	r = addstate(thislist, start, m, ফাঁকা, 0, 0);
    যদি (r == ফাঁকা)
    {
	nfa_match = NFA_TOO_EXPENSIVE;
	যাও theend;
    }

#define	ADD_STATE_IF_MATCH(state)	\
    যদি (ফলাফল)				\
    {					\
	add_state = state->out;		\
	add_off = clen;			\
    }

    /*
     * Run for each character.
     */
    জন্য (;;)
    {
	পূর্ণসংখ্যা	curc;
	পূর্ণসংখ্যা	clen;

	যদি (has_mbyte)
	{
	    // Fast path for an ASCII byte not followed by a composing
	    // character, matching the check in utfc_ptr2len().  Avoids two
	    // indirect calls for the common case.
	    যদি (rex.প্রবেশ[0] != NUL && rex.প্রবেশ[0] < 0x80 && rex.প্রবেশ[1] < 0x80)
	    {
		curc = rex.প্রবেশ[0];
		clen = 1;
	    }
	    অন্যথা
	    {
		curc = (*mb_ptr2char)(rex.প্রবেশ);
		clen = (*mb_ptr2len)(rex.প্রবেশ);
	    }
	}
	অন্যথা
	{
	    curc = *rex.প্রবেশ;
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
	    যাও theend;
	}

	thislist->id = rex.nfa_listid;
	nextlist->id = rex.nfa_listid + 1;

#ifdef ENABLE_LOG
	fprintf(log_fd, "------------------------------------------\n");
	fprintf(log_fd, ">>> Reginput is \"%s\"\n", rex.প্রবেশ);
	fprintf(log_fd, ">>> Advanced one character... Current char is %c (code %d) \n", curc, (পূর্ণসংখ্যা)curc);
	fprintf(log_fd, ">>> Thislist has %d states available: ", thislist->n);
	{
	    পূর্ণসংখ্যা i;

	    জন্য (i = 0; i < thislist->n; i++)
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
	    বিরতি;

	// compute nextlist
	জন্য (listidx = 0; listidx < thislist->n; ++listidx)
	{
	    // If the list gets very long there probably is something wrong.
	    // At least allow interrupting with CTRL-C.  Only check once in a
	    // while, calling ui_breakcheck() for every state is too slow.
	    স্থির পূর্ণসংখ্যা	breakcheck_গুনতি = 0;  // using "static" makes it faster
	    যদি (unlikely(++breakcheck_গুনতি >= 100000))
	    {
		ui_breakcheck();
		breakcheck_গুনতি = 0;
	    }
	    যদি (got_int)
		বিরতি;
#ifdef FEAT_RELTIME
	    যদি (nfa_did_time_out())
		বিরতি;
#endif
	    t = &thislist->t[listidx];

#ifdef NFA_REGEXP_DEBUG_LOG
	    nfa_set_code(t->state->c);
	    fprintf(debug, "%s, ", code);
#endif
#ifdef ENABLE_LOG
	    {
		পূর্ণসংখ্যা col;

		যদি (t->subs.norm.in_use <= 0)
		    col = -1;
		অন্যথা যদি (REG_MULTI)
		    col = t->subs.norm.list.multi[0].start_col;
		অন্যথা
		    col = (পূর্ণসংখ্যা)(t->subs.norm.list.সারি[0].start - rex.সারি);
		nfa_set_code(t->state->c);
		fprintf(log_fd, "(%d) char %d %s (start col %d)%s... \n",
			abs(t->state->id), (পূর্ণসংখ্যা)t->state->c, code, col,
			pim_info(&t->pim));
	    }
#endif

	    /*
	     * Handle the possible codes of the current state.
	     * The most important is NFA_MATCH.
	     */
	    add_state = ফাঁকা;
	    add_here = FALSE;
	    add_গুনতি = 0;
	    নির্বাচন (t->state->c)
	    {
	    ক্ষেত্র NFA_MATCH:
	      {
		// If the match is not at the start of the line, ends before a
		// composing characters and rex.reg_icombine is not set, that
		// is not really a match.
		যদি (enc_utf8 && !rex.reg_icombine
			     && rex.প্রবেশ != rex.সারি && utf_iscomposing(curc))
		    বিরতি;

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
		যাও nextchar;
	      }

	    ক্ষেত্র NFA_END_INVISIBLE:
	    ক্ষেত্র NFA_END_INVISIBLE_NEG:
	    ক্ষেত্র NFA_END_PATTERN:
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
		যদি (nfa_endp != ফাঁকা)
		{
		    যদি (REG_MULTI)
			fprintf(log_fd, "Current lnum: %d, endp lnum: %d; current col: %d, endp col: %d\n",
				(পূর্ণসংখ্যা)rex.lnum,
				(পূর্ণসংখ্যা)nfa_endp->se_u.pos.lnum,
				(পূর্ণসংখ্যা)(rex.প্রবেশ - rex.সারি),
				nfa_endp->se_u.pos.col);
		    অন্যথা
			fprintf(log_fd, "Current col: %d, endp col: %d\n",
				(পূর্ণসংখ্যা)(rex.প্রবেশ - rex.সারি),
				(পূর্ণসংখ্যা)(nfa_endp->se_u.ptr - rex.প্রবেশ));
		}
#endif
		// If "nfa_endp" is set it's only a match if it ends at
		// "nfa_endp"
		যদি (nfa_endp != ফাঁকা && (REG_MULTI
			? (rex.lnum != nfa_endp->se_u.pos.lnum
			    || (পূর্ণসংখ্যা)(rex.প্রবেশ - rex.সারি)
						!= nfa_endp->se_u.pos.col)
			: rex.প্রবেশ != nfa_endp->se_u.ptr))
		    বিরতি;

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
		যাও nextchar;

	    ক্ষেত্র NFA_START_INVISIBLE:
	    ক্ষেত্র NFA_START_INVISIBLE_FIRST:
	    ক্ষেত্র NFA_START_INVISIBLE_NEG:
	    ক্ষেত্র NFA_START_INVISIBLE_NEG_FIRST:
	    ক্ষেত্র NFA_START_INVISIBLE_BEFORE:
	    ক্ষেত্র NFA_START_INVISIBLE_BEFORE_FIRST:
	    ক্ষেত্র NFA_START_INVISIBLE_BEFORE_NEG:
	    ক্ষেত্র NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
		{
#ifdef ENABLE_LOG
		    fprintf(log_fd, "Failure chance invisible: %d, what follows: %d\n",
			    failure_chance(t->state->out, 0),
			    failure_chance(t->state->out1->out, 0));
#endif
		    // Do it directly if there already is a PIM or when
		    // nfa_postprocess() detected it will work better.
		    যদি (t->pim.ফলাফল != NFA_PIM_UNUSED
			 || t->state->c == NFA_START_INVISIBLE_FIRST
			 || t->state->c == NFA_START_INVISIBLE_NEG_FIRST
			 || t->state->c == NFA_START_INVISIBLE_BEFORE_FIRST
			 || t->state->c == NFA_START_INVISIBLE_BEFORE_NEG_FIRST)
		    {
			পূর্ণসংখ্যা in_use = m->norm.in_use;

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
			ফলাফল = recursive_regmatch(t->state, ফাঁকা, prog,
					  submatch, m, &listids, &listids_len);
			যদি (ফলাফল == NFA_TOO_EXPENSIVE)
			{
			    nfa_match = ফলাফল;
			    যাও theend;
			}

			// for \@! and \@<! it is a match when the result is
			// FALSE
			যদি (ফলাফল != (t->state->c == NFA_START_INVISIBLE_NEG
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
			pim.ফলাফল = NFA_PIM_TODO;
			pim.subs.norm.in_use = 0;
#ifdef FEAT_SYN_HL
			pim.subs.synt.in_use = 0;
#endif
			যদি (REG_MULTI)
			{
			    pim.end.pos.col = (পূর্ণসংখ্যা)(rex.প্রবেশ - rex.সারি);
			    pim.end.pos.lnum = rex.lnum;
			}
			অন্যথা
			    pim.end.ptr = rex.প্রবেশ;

			// t->state->out1 is the corresponding END_INVISIBLE
			// node; Add its out to the current list (zero-width
			// match).
			যদি (addstate_here(thislist, t->state->out1->out,
					     &t->subs, &pim, &listidx) == ফাঁকা)
			{
			    nfa_match = NFA_TOO_EXPENSIVE;
			    যাও theend;
			}
		    }
		}
		বিরতি;

	    ক্ষেত্র NFA_START_PATTERN:
	      {
		nfa_state_T *skip = ফাঁকা;
#ifdef ENABLE_LOG
		পূর্ণসংখ্যা	    skip_lid = 0;
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
		যদি (skip != ফাঁকা)
		{
#ifdef ENABLE_LOG
		    nfa_set_code(skip->c);
		    fprintf(log_fd, "> Not trying to match pattern, output state %d is already in list %d. char %d: %s\n",
			    abs(skip->id), skip_lid, skip->c, code);
#endif
		    বিরতি;
		}
		// Copy submatch info to the recursive call, opposite of what
		// happens afterwards.
		copy_sub_off(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		যদি (rex.nfa_has_zsubexpr)
		    copy_sub_off(&m->synt, &t->subs.synt);
#endif

		// First try matching the pattern.
		ফলাফল = recursive_regmatch(t->state, ফাঁকা, prog,
					  submatch, m, &listids, &listids_len);
		যদি (ফলাফল == NFA_TOO_EXPENSIVE)
		{
		    nfa_match = ফলাফল;
		    যাও theend;
		}
		যদি (ফলাফল)
		{
		    পূর্ণসংখ্যা bytelen;

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
						  - (পূর্ণসংখ্যা)(rex.প্রবেশ - rex.সারি);
		    অন্যথা
			bytelen = (পূর্ণসংখ্যা)(m->norm.list.সারি[0].end - rex.প্রবেশ);

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
		বিরতি;
	      }

	    ক্ষেত্র NFA_BOL:
		যদি (rex.প্রবেশ == rex.সারি)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		বিরতি;

	    ক্ষেত্র NFA_EOL:
		যদি (curc == NUL)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		বিরতি;

	    ক্ষেত্র NFA_BOW:
		ফলাফল = TRUE;

		যদি (curc == NUL)
		    ফলাফল = FALSE;
		অন্যথা যদি (has_mbyte)
		{
		    পূর্ণসংখ্যা this_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.প্রবেশ, rex.reg_buf);
		    যদি (this_class <= 1)
			ফলাফল = FALSE;
		    অন্যথা যদি (reg_পূর্ব_class() == this_class)
			ফলাফল = FALSE;
		}
		অন্যথা যদি (!vim_iswordc_buf(curc, rex.reg_buf)
			   || (rex.প্রবেশ > rex.সারি
			       && vim_iswordc_buf(rex.প্রবেশ[-1], rex.reg_buf)))
		    ফলাফল = FALSE;
		যদি (ফলাফল)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		বিরতি;

	    ক্ষেত্র NFA_EOW:
		ফলাফল = TRUE;
		যদি (rex.প্রবেশ == rex.সারি)
		    ফলাফল = FALSE;
		অন্যথা যদি (has_mbyte)
		{
		    পূর্ণসংখ্যা this_class, পূর্ব_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.প্রবেশ, rex.reg_buf);
		    পূর্ব_class = reg_পূর্ব_class();
		    যদি (this_class == পূর্ব_class
					|| পূর্ব_class == 0 || পূর্ব_class == 1)
			ফলাফল = FALSE;
		}
		অন্যথা যদি (!vim_iswordc_buf(rex.প্রবেশ[-1], rex.reg_buf)
			|| (rex.প্রবেশ[0] != NUL
					&& vim_iswordc_buf(curc, rex.reg_buf)))
		    ফলাফল = FALSE;
		যদি (ফলাফল)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		বিরতি;

	    ক্ষেত্র NFA_BOF:
		যদি (rex.lnum == 0 && rex.প্রবেশ == rex.সারি
				     && (!REG_MULTI || rex.reg_firstlnum == 1))
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		বিরতি;

	    ক্ষেত্র NFA_EOF:
		যদি (rex.lnum == rex.reg_maxline && curc == NUL)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		বিরতি;

	    ক্ষেত্র NFA_COMPOSING:
	    {
		nfa_state_T *end;

		ফলাফল = match_composing(t->state->out, curc, clen);

		end = t->state->out1;	    // NFA_END_COMPOSING
		ADD_STATE_IF_MATCH(end);
		বিরতি;
	    }

	    ক্ষেত্র NFA_NEWL:
		যদি (curc == NUL && !rex.reg_সারি_lbr && REG_MULTI
						&& rex.lnum <= rex.reg_maxline)
		{
		    go_to_nextline = TRUE;
		    // Pass -1 for the offset, which means taking the position
		    // at the start of the next line.
		    add_state = t->state->out;
		    add_off = -1;
		}
		অন্যথা যদি (curc == '\n' && rex.reg_সারি_lbr)
		{
		    // match \n as if it is an ordinary character
		    add_state = t->state->out;
		    add_off = 1;
		}
		বিরতি;

	    ক্ষেত্র NFA_START_COLL:
	    ক্ষেত্র NFA_START_NEG_COLL:
	      {
		// What follows is a list of characters, until NFA_END_COLL.
		// One of them must match or none of them must match.
		nfa_state_T	*state;
		পূর্ণসংখ্যা		ফলাফল_if_matched;
		পূর্ণসংখ্যা		c1, c2;

		// Never match EOL. If it's part of the collection it is added
		// as a separate state with an OR.
		যদি (curc == NUL)
		    বিরতি;

		state = t->state->out;
		ফলাফল_if_matched = (t->state->c == NFA_START_COLL);
		জন্য (;;)
		{
		    যদি (state->c == NFA_COMPOSING)
		    {
			nfa_state_T *end;

			ফলাফল = match_composing(t->state->out->out, curc,
									clen);

			যদি (t->state->out->out1 != ফাঁকা
				&& t->state->out->out1->c == NFA_END_COMPOSING)
			{
			    end = t->state->out->out1;
			    ADD_STATE_IF_MATCH(end);
			}
			বিরতি;
		    }
		    যদি (state->c == NFA_END_COLL)
		    {
			ফলাফল = !ফলাফল_if_matched;
			বিরতি;
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
			    ফলাফল = ফলাফল_if_matched;
			    বিরতি;
			}
			যদি (rex.reg_ic)
			{
			    পূর্ণসংখ্যা curc_নিচের = MB_CASEFOLD(curc);
			    পূর্ণসংখ্যা done = FALSE;

			    জন্য ( ; c1 <= c2; ++c1)
				যদি (MB_CASEFOLD(c1) == curc_নিচের)
				{
				    ফলাফল = ফলাফল_if_matched;
				    done = TRUE;
				    বিরতি;
				}
			    যদি (done)
				বিরতি;
			}
		    }
		    অন্যথা যদি (state->c < 0 ? check_char_class(state->c, curc)
			       : (curc == state->c
				   || (rex.reg_ic && MB_CASEFOLD(curc)
						    == MB_CASEFOLD(state->c))))
		    {
			ফলাফল = ফলাফল_if_matched;
			বিরতি;
		    }
		    state = state->out;
		}
		যদি (ফলাফল)
		{
		    // next state is in out of the NFA_END_COLL, out1 of
		    // START points to the END state
		    add_state = t->state->out1->out;
		    add_off = clen;
		}
		বিরতি;
	      }

	    ক্ষেত্র NFA_ANY:
		// Any char except '\0', (end of input) does not match.
		যদি (curc > 0)
		{
		    add_state = t->state->out;
		    add_off = clen;
		}
		বিরতি;

	    ক্ষেত্র NFA_ANY_COMPOSING:
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
		বিরতি;

	    /*
	     * Character classes like \a for alpha, \d for digit etc.
	     */
	    ক্ষেত্র NFA_IDENT:	//  \i
		ফলাফল = vim_isIDc(curc);
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_SIDENT:	//  \I
		ফলাফল = !VIM_ISDIGIT(curc) && vim_isIDc(curc);
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_KWORD:	//  \k
		ফলাফল = vim_iswordp_buf(rex.প্রবেশ, rex.reg_buf);
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_SKWORD:	//  \K
		ফলাফল = !VIM_ISDIGIT(curc)
				     && vim_iswordp_buf(rex.প্রবেশ, rex.reg_buf);
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_FNAME:	//  \f
		ফলাফল = vim_isfilec(curc);
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_SFNAME:	//  \F
		ফলাফল = !VIM_ISDIGIT(curc) && vim_isfilec(curc);
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_PRINT:	//  \p
		ফলাফল = vim_isprintc(PTR2CHAR(rex.প্রবেশ));
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_SPRINT:	//  \P
		ফলাফল = !VIM_ISDIGIT(curc) && vim_isprintc(PTR2CHAR(rex.প্রবেশ));
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_WHITE:	//  \s
		ফলাফল = VIM_ISWHITE(curc);
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_NWHITE:	//  \S
		ফলাফল = curc != NUL && !VIM_ISWHITE(curc);
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_DIGIT:	//  \d
		ফলাফল = ri_digit(curc);
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_NDIGIT:	//  \D
		ফলাফল = curc != NUL && !ri_digit(curc);
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_HEX:	//  \x
		ফলাফল = ri_hex(curc);
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_NHEX:	//  \X
		ফলাফল = curc != NUL && !ri_hex(curc);
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_OCTAL:	//  \o
		ফলাফল = ri_octal(curc);
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_NOCTAL:	//  \O
		ফলাফল = curc != NUL && !ri_octal(curc);
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_WORD:	//  \w
		ফলাফল = ri_word(curc);
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_NWORD:	//  \W
		ফলাফল = curc != NUL && !ri_word(curc);
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_শীর্ষ:	//  \h
		ফলাফল = ri_শীর্ষ(curc);
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_NHEAD:	//  \H
		ফলাফল = curc != NUL && !ri_শীর্ষ(curc);
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_ALPHA:	//  \a
		ফলাফল = ri_alpha(curc);
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_NALPHA:	//  \A
		ফলাফল = curc != NUL && !ri_alpha(curc);
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_LOWER:	//  \l
		ফলাফল = ri_lower(curc);
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_NLOWER:	//  \L
		ফলাফল = curc != NUL && !ri_lower(curc);
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_UPPER:	//  \u
		ফলাফল = ri_upper(curc);
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_NUPPER:	// \U
		ফলাফল = curc != NUL && !ri_upper(curc);
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_LOWER_IC:	// [a-z]
		ফলাফল = ri_lower(curc) || (rex.reg_ic && ri_upper(curc));
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_NLOWER_IC:	// [^a-z]
		ফলাফল = curc != NUL
			&& !(ri_lower(curc) || (rex.reg_ic && ri_upper(curc)));
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_UPPER_IC:	// [A-Z]
		ফলাফল = ri_upper(curc) || (rex.reg_ic && ri_lower(curc));
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_NUPPER_IC:	// ^[A-Z]
		ফলাফল = curc != NUL
			&& !(ri_upper(curc) || (rex.reg_ic && ri_lower(curc)));
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;

	    ক্ষেত্র NFA_BACKREF1:
	    ক্ষেত্র NFA_BACKREF2:
	    ক্ষেত্র NFA_BACKREF3:
	    ক্ষেত্র NFA_BACKREF4:
	    ক্ষেত্র NFA_BACKREF5:
	    ক্ষেত্র NFA_BACKREF6:
	    ক্ষেত্র NFA_BACKREF7:
	    ক্ষেত্র NFA_BACKREF8:
	    ক্ষেত্র NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	    ক্ষেত্র NFA_ZREF1:
	    ক্ষেত্র NFA_ZREF2:
	    ক্ষেত্র NFA_ZREF3:
	    ক্ষেত্র NFA_ZREF4:
	    ক্ষেত্র NFA_ZREF5:
	    ক্ষেত্র NFA_ZREF6:
	    ক্ষেত্র NFA_ZREF7:
	    ক্ষেত্র NFA_ZREF8:
	    ক্ষেত্র NFA_ZREF9:
#endif
		// \1 .. \9  \z1 .. \z9
	      {
		পূর্ণসংখ্যা subidx;
		পূর্ণসংখ্যা bytelen;

#ifdef FEAT_SYN_HL
		যদি (t->state->c >= NFA_BACKREF1 && t->state->c <= NFA_BACKREF9)
#endif
		{
		    subidx = t->state->c - NFA_BACKREF1 + 1;
		    ফলাফল = match_backref(&t->subs.norm, subidx, &bytelen);
		}
#ifdef FEAT_SYN_HL
		অন্যথা
		{
		    subidx = t->state->c - NFA_ZREF1 + 1;
		    ফলাফল = match_zref(subidx, &bytelen);
		}
#endif

		যদি (ফলাফল)
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
		বিরতি;
	      }
	    ক্ষেত্র NFA_SKIP:
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
	      বিরতি;

	    ক্ষেত্র NFA_LNUM:
	    ক্ষেত্র NFA_LNUM_GT:
	    ক্ষেত্র NFA_LNUM_LT:
		ফলাফল = (REG_MULTI &&
			nfa_re_অঙ্ক_cmp(t->state->val, t->state->c - NFA_LNUM,
			    (long_u)(rex.lnum + rex.reg_firstlnum)));
		যদি (ফলাফল)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		বিরতি;

	    ক্ষেত্র NFA_COL:
	    ক্ষেত্র NFA_COL_GT:
	    ক্ষেত্র NFA_COL_LT:
		ফলাফল = nfa_re_অঙ্ক_cmp(t->state->val, t->state->c - NFA_COL,
			(long_u)(rex.প্রবেশ - rex.সারি) + 1);
		যদি (ফলাফল)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		বিরতি;

	    ক্ষেত্র NFA_VCOL:
	    ক্ষেত্র NFA_VCOL_GT:
	    ক্ষেত্র NFA_VCOL_LT:
		{
		    পূর্ণসংখ্যা     op = t->state->c - NFA_VCOL;
		    colnr_T col = (colnr_T)(rex.প্রবেশ - rex.সারি);
		    win_T   *wp = rex.reg_win == ফাঁকা ? curwin : rex.reg_win;

		    // Bail out quickly when there can't be a match, avoid the
		    // overhead of win_linetabsize() on long lines.
		    যদি (op != 1 && col > t->state->val
			    * (has_mbyte ? MB_MAXBYTES : 1))
			বিরতি;
		    ফলাফল = FALSE;
		    যদি (op == 1 && col - 1 > t->state->val && col > 100)
		    {
			পূর্ণসংখ্যা ts = wp->w_বাফার->b_p_ts;

			// Guess that a character won't use more columns than
			// 'tabstop', with a minimum of 4.
			যদি (ts < 4)
			    ts = 4;
			ফলাফল = col > t->state->val * ts;
		    }
		    যদি (!ফলাফল)
		    {
			linenr_T    lnum = REG_MULTI
					    ? rex.reg_firstlnum + rex.lnum : 1;
			long_u	    vcol;

			যদি (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_বাফার->b_ml.ml_সারি_গুনতি))
			    lnum = 1;
			vcol = (long_u)win_linetabsize(wp, lnum, rex.সারি, col);
			ফলাফল = nfa_re_অঙ্ক_cmp(t->state->val, op, vcol + 1);
		    }
		    যদি (ফলাফল)
		    {
			add_here = TRUE;
			add_state = t->state->out;
		    }
		}
		বিরতি;

	    ক্ষেত্র NFA_MARK:
	    ক্ষেত্র NFA_MARK_GT:
	    ক্ষেত্র NFA_MARK_LT:
	      {
		pos_T	*pos;
		size_t	col = REG_MULTI ? rex.প্রবেশ - rex.সারি : 0;

		pos = getmark_buf(rex.reg_buf, t->state->val, FALSE);

		// Line may have been freed, get it again.
		যদি (REG_MULTI)
		{
		    rex.সারি = reg_getline(rex.lnum);
		    rex.প্রবেশ = rex.সারি + col;
		}

		// Compare the mark position to the match position, if the mark
		// exists and mark is set in reg_buf.
		যদি (pos != ফাঁকা && pos->lnum > 0)
		{
		    colnr_T pos_col = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->col == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->col;

		    ফলাফল = (pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_col == (colnr_T)(rex.প্রবেশ - rex.সারি)
				    ? t->state->c == NFA_MARK
				    : (pos_col < (colnr_T)(rex.প্রবেশ - rex.সারি)
					? t->state->c == NFA_MARK_GT
					: t->state->c == NFA_MARK_LT))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? t->state->c == NFA_MARK_GT
				    : t->state->c == NFA_MARK_LT));
		    যদি (ফলাফল)
		    {
			add_here = TRUE;
			add_state = t->state->out;
		    }
		}
		বিরতি;
	      }

	    ক্ষেত্র NFA_CURSOR:
		ফলাফল = (rex.reg_win != ফাঁকা
			&& (rex.lnum + rex.reg_firstlnum
						 == rex.reg_win->w_cursor.lnum)
			&& ((colnr_T)(rex.প্রবেশ - rex.সারি)
						== rex.reg_win->w_cursor.col));
		যদি (ফলাফল)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		বিরতি;

	    ক্ষেত্র NFA_VISUAL:
		ফলাফল = reg_match_visual();
		যদি (ফলাফল)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		বিরতি;

	    ক্ষেত্র NFA_MOPEN1:
	    ক্ষেত্র NFA_MOPEN2:
	    ক্ষেত্র NFA_MOPEN3:
	    ক্ষেত্র NFA_MOPEN4:
	    ক্ষেত্র NFA_MOPEN5:
	    ক্ষেত্র NFA_MOPEN6:
	    ক্ষেত্র NFA_MOPEN7:
	    ক্ষেত্র NFA_MOPEN8:
	    ক্ষেত্র NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	    ক্ষেত্র NFA_ZOPEN:
	    ক্ষেত্র NFA_ZOPEN1:
	    ক্ষেত্র NFA_ZOPEN2:
	    ক্ষেত্র NFA_ZOPEN3:
	    ক্ষেত্র NFA_ZOPEN4:
	    ক্ষেত্র NFA_ZOPEN5:
	    ক্ষেত্র NFA_ZOPEN6:
	    ক্ষেত্র NFA_ZOPEN7:
	    ক্ষেত্র NFA_ZOPEN8:
	    ক্ষেত্র NFA_ZOPEN9:
#endif
	    ক্ষেত্র NFA_NOPEN:
	    ক্ষেত্র NFA_ZSTART:
		// These states are only added to be able to bail out when
		// they are added again, nothing is to be done.
		বিরতি;

	    পূর্বনির্ধারিত:	// regular character
	      {
		পূর্ণসংখ্যা c = t->state->c;

#ifdef DEBUG
		যদি (c < 0)
		    siemsg("Negative state char: %ld", (long)c);
#endif
		ফলাফল = (c == curc);

		যদি (!ফলাফল && rex.reg_ic)
		    ফলাফল = MB_CASEFOLD(c) == MB_CASEFOLD(curc);
		// If rex.reg_icombine is not set only skip over the character
		// itself.  When it is set skip over composing characters.
		যদি (ফলাফল && enc_utf8 && !rex.reg_icombine)
		    clen = utf_ptr2len(rex.প্রবেশ);
		ADD_STATE_IF_MATCH(t->state);
		বিরতি;
	      }

	    } // switch (t->state->c)

	    যদি (add_state != ফাঁকা)
	    {
		nfa_pim_T *pim;
		nfa_pim_T pim_copy;

		যদি (t->pim.ফলাফল == NFA_PIM_UNUSED)
		    pim = ফাঁকা;
		অন্যথা
		    pim = &t->pim;

		// Handle the postponed invisible match if the match might end
		// without advancing and before the end of the line.
		যদি (pim != ফাঁকা && (clen == 0 || match_follows(add_state, 0)))
		{
		    যদি (pim->ফলাফল == NFA_PIM_TODO)
		    {
#ifdef ENABLE_LOG
			fprintf(log_fd, "\n");
			fprintf(log_fd, "==================================\n");
			fprintf(log_fd, "Postponed recursive nfa_regmatch()\n");
			fprintf(log_fd, "\n");
#endif
			ফলাফল = recursive_regmatch(pim->state, pim,
				    prog, submatch, m, &listids, &listids_len);
			pim->ফলাফল = ফলাফল ? NFA_PIM_MATCH : NFA_PIM_NOMATCH;
			// for \@! and \@<! it is a match when the result is
			// FALSE
			যদি (ফলাফল != (pim->state->c == NFA_START_INVISIBLE_NEG
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
			ফলাফল = (pim->ফলাফল == NFA_PIM_MATCH);
#ifdef ENABLE_LOG
			fprintf(log_fd, "\n");
			fprintf(log_fd, "Using previous recursive nfa_regmatch() result, result == %d\n", pim->ফলাফল);
			fprintf(log_fd, "MATCH = %s\n", ফলাফল == TRUE ? "OK" : "FALSE");
			fprintf(log_fd, "\n");
#endif
		    }

		    // for \@! and \@<! it is a match when result is FALSE
		    যদি (ফলাফল != (pim->state->c == NFA_START_INVISIBLE_NEG
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
			চালু;

		    // Postponed invisible match was handled, don't add it to
		    // following states.
		    pim = ফাঁকা;
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
		যদি (r == ফাঁকা)
		{
		    nfa_match = NFA_TOO_EXPENSIVE;
		    যাও theend;
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
			  || (colnr_T)(rex.প্রবেশ - rex.সারি) < rex.reg_maxcol))
		    || (nfa_endp != ফাঁকা
			&& (REG_MULTI
			    ? (rex.lnum < nfa_endp->se_u.pos.lnum
			       || (rex.lnum == nfa_endp->se_u.pos.lnum
				   && (পূর্ণসংখ্যা)(rex.প্রবেশ - rex.সারি)
						    < nfa_endp->se_u.pos.col))
			    : rex.প্রবেশ < nfa_endp->se_u.ptr))))
	{
#ifdef ENABLE_LOG
	    fprintf(log_fd, "(---) STARTSTATE\n");
#endif
	    // Inline optimized code for addstate() if we know the state is
	    // the first MOPEN.
	    যদি (toplevel)
	    {
		পূর্ণসংখ্যা add = TRUE;
		পূর্ণসংখ্যা c;

		যদি (prog->regstart != NUL && clen != 0)
		{
		    যদি (nextlist->n == 0)
		    {
			colnr_T col = (colnr_T)(rex.প্রবেশ - rex.সারি) + clen;

			// Nextlist is empty, we can skip ahead to the
			// character that must appear at the start.
			যদি (skip_to_start(prog->regstart, &col) == FAIL)
			    বিরতি;
#ifdef ENABLE_LOG
			fprintf(log_fd, "  Skipping ahead %d bytes to regstart\n",
				col - ((colnr_T)(rex.প্রবেশ - rex.সারি) + clen));
#endif
			rex.প্রবেশ = rex.সারি + col - clen;
		    }
		    অন্যথা
		    {
			// Checking if the required start character matches is
			// cheaper than adding a state that won't match.
			c = PTR2CHAR(rex.প্রবেশ + clen);
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
					 (colnr_T)(rex.প্রবেশ - rex.সারি) + clen;
			m->norm.orig_start_col =
					       m->norm.list.multi[0].start_col;
		    }
		    অন্যথা
			m->norm.list.সারি[0].start = rex.প্রবেশ + clen;
		    যদি (addstate(nextlist, start->out, m, ফাঁকা, clen, 0) == ফাঁকা)
		    {
			nfa_match = NFA_TOO_EXPENSIVE;
			যাও theend;
		    }
		}
	    }
	    অন্যথা
	    {
		char_u	    *save_সারি = rex.সারি;
		char_u	    *save_প্রবেশ = rex.প্রবেশ;
		linenr_T    save_lnum = rex.lnum;

		// At the end of a line the match can only start on the next
		// line, use that position instead of the line break.
		যদি (REG_MULTI && clen == 0 && nfa_endp != ফাঁকা
					 && rex.lnum < nfa_endp->se_u.pos.lnum)
		{
		    char_u  *পরবর্তী_সারি = reg_getline(rex.lnum + 1);

		    যদি (পরবর্তী_সারি != ফাঁকা)
		    {
			rex.সারি = পরবর্তী_সারি;
			rex.প্রবেশ = rex.সারি;
			++rex.lnum;
		    }
		}

		r = addstate(nextlist, start, m, ফাঁকা, clen, 0);

		rex.সারি = save_সারি;
		rex.প্রবেশ = save_প্রবেশ;
		rex.lnum = save_lnum;

		যদি (r == ফাঁকা)
		{
		    nfa_match = NFA_TOO_EXPENSIVE;
		    যাও theend;
		}
	    }
	}

#ifdef ENABLE_LOG
	fprintf(log_fd, ">>> Thislist had %d states available: ", thislist->n);
	{
	    পূর্ণসংখ্যা i;

	    জন্য (i = 0; i < thislist->n; i++)
		fprintf(log_fd, "%d  ", abs(thislist->t[i].state->id));
	}
	fprintf(log_fd, "\n");
#endif

nextchar:
	// Advance to the next character, or advance to the next line, or
	// finish.
	যদি (clen != 0)
	    rex.প্রবেশ += clen;
	অন্যথা যদি (go_to_nextline || (nfa_endp != ফাঁকা && REG_MULTI
					&& rex.lnum < nfa_endp->se_u.pos.lnum))
	    reg_nextline();
	অন্যথা
	    বিরতি;

	// Allow interrupting with CTRL-C.
	সারি_breakcheck();
	যদি (got_int)
	    বিরতি;
#ifdef FEAT_RELTIME
	যদি (nfa_did_time_out())
	    বিরতি;
#endif
    }

#ifdef ENABLE_LOG
    যদি (log_fd != stderr)
	fclose(log_fd);
    log_fd = ফাঁকা;
#endif

theend:
    // Cache list buffers in prog for reuse, or free if prog already has
    // cached buffers (recursive call case).
    যদি (prog->listbuf[0] == ফাঁকা && list[0].t != ফাঁকা && list[1].t != ফাঁকা)
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

    ফেরত nfa_match;
}

/*
 * Try match of "prog" with at rex.line["col"].
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    স্থির long
nfa_regtry(
    nfa_regprog_T   *prog,
    colnr_T	    col,
    পূর্ণসংখ্যা		    *timed_out UNUSED)	// flag set on timeout or NULL
{
    পূর্ণসংখ্যা		i;
    regsubs_T	subs, m;
    nfa_state_T	*start = prog->start;
    পূর্ণসংখ্যা		ফলাফল;
#ifdef ENABLE_LOG
    FILE	*f;
#endif

    rex.প্রবেশ = rex.সারি + col;
#ifdef FEAT_RELTIME
    nfa_timed_out = timed_out;
#endif

#ifdef ENABLE_LOG
    f = fopen(NFA_REGEXP_RUN_LOG, "a");
    যদি (f != ফাঁকা)
    {
	fprintf(f, "\n\n\t=======================================================\n");
# ifdef DEBUG
	fprintf(f, "\tRegexp is \"%s\"\n", nfa_regengine.রাশিমালা);
# endif
	fprintf(f, "\tInput text is \"%s\" \n", rex.প্রবেশ);
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

    ফলাফল = nfa_regmatch(prog, start, &subs, &m);
    যদি (ফলাফল == FALSE)
	ফেরত 0;
    অন্যথা যদি (ফলাফল == NFA_TOO_EXPENSIVE)
	ফেরত ফলাফল;

    cleanup_subexpr();
    যদি (REG_MULTI)
    {
	জন্য (i = 0; i < subs.norm.in_use; i++)
	{
	    rex.reg_startpos[i].lnum = subs.norm.list.multi[i].start_lnum;
	    rex.reg_startpos[i].col = subs.norm.list.multi[i].start_col;

	    rex.reg_endpos[i].lnum = subs.norm.list.multi[i].end_lnum;
	    rex.reg_endpos[i].col = subs.norm.list.multi[i].end_col;
	}
	যদি (rex.reg_mmatch != ফাঁকা)
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
	    rex.reg_endpos[0].col = (পূর্ণসংখ্যা)(rex.প্রবেশ - rex.সারি);
	}
	অন্যথা
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    অন্যথা
    {
	জন্য (i = 0; i < subs.norm.in_use; i++)
	{
	    rex.reg_startp[i] = subs.norm.list.সারি[i].start;
	    rex.reg_endp[i] = subs.norm.list.সারি[i].end;
	}

	যদি (rex.reg_startp[0] == ফাঁকা)
	    rex.reg_startp[0] = rex.সারি + col;
	যদি (rex.reg_endp[0] == ফাঁকা)
	    rex.reg_endp[0] = rex.প্রবেশ;
    }

#ifdef FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = ফাঁকা;

    যদি (prog->reghasz == REX_SET)
    {
	cleanup_zsubexpr();
	re_extmatch_out = make_extmatch();
	যদি (re_extmatch_out == ফাঁকা)
	    ফেরত 0;
	// Loop over \z1, \z2, etc.  There is no \z0.
	জন্য (i = 1; i < subs.synt.in_use; i++)
	{
	    যদি (REG_MULTI)
	    {
		কাঠামো multipos *mpos = &subs.synt.list.multi[i];

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
		কাঠামো linepos *lpos = &subs.synt.list.সারি[i];

		যদি (lpos->start != ফাঁকা && lpos->end != ফাঁকা)
		    re_extmatch_out->matches[i] =
			    vim_strnsave(lpos->start, lpos->end - lpos->start);
	    }
	}
    }
#endif

    ফেরত 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 *
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    স্থির long
nfa_regexec_both(
    char_u	*সারি,
    colnr_T	startcol,	// column to start looking for match
    পূর্ণসংখ্যা		*timed_out)	// flag set on timeout or NULL
{
    nfa_regprog_T   *prog;
    long	    retval = 0L;
    পূর্ণসংখ্যা		    i;
    colnr_T	    col = startcol;

    যদি (REG_MULTI)
    {
	prog = (nfa_regprog_T *)rex.reg_mmatch->regprog;
	সারি = reg_getline((linenr_T)0);    // relative to the cursor
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
    যদি (prog == ফাঁকা || সারি == ফাঁকা)
    {
	iemsg(e_null_argument);
	যাও theend;
    }

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    যদি (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    অন্যথা যদি (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    যদি (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    rex.সারি = সারি;
    rex.lnum = 0;    // relative to line

    rex.nfa_has_zend = prog->has_zend;
    rex.nfa_has_backref = prog->has_backref;
    rex.nfa_nsubexpr = prog->nsubexp;
    rex.nfa_listid = 1;
    rex.nfa_alt_listid = 2;
#ifdef DEBUG
    nfa_regengine.রাশিমালা = prog->pattern;
#endif

    যদি (prog->reganch && col > 0)
	ফেরত 0L;

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
	    ফেরত 0L;

	// If match_text is set it contains the full text that must match.
	// Nothing else to try. Doesn't handle combining chars well.
	যদি (prog->match_text != ফাঁকা && *prog->match_text != NUL && !rex.reg_icombine)
	{
	    retval = find_match_text(&col, prog->regstart, prog->match_text);
	    যদি (REG_MULTI)
		rex.reg_mmatch->rmm_matchcol = col;
	    অন্যথা
		rex.reg_match->rm_matchcol = col;
	    ফেরত retval;
	}
    }

    // If the start column is past the maximum column: no need to try.
    যদি (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	যাও theend;

    // Set the "nstate" used by nfa_regcomp() to zero to trigger an error when
    // it's accidentally used during execution.
    nstate = 0;
    জন্য (i = 0; i < prog->nstate; ++i)
    {
	prog->state[i].id = i;
	prog->state[i].lastlist[0] = 0;
	prog->state[i].lastlist[1] = 0;
    }

    retval = nfa_regtry(prog, col, timed_out);

#ifdef DEBUG
    nfa_regengine.রাশিমালা = ফাঁকা;
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

    ফেরত retval;
}

/*
 * Compile a regular expression into internal code for the NFA matcher.
 * Returns the program in allocated space.  Returns NULL for an error.
 */
    স্থির regprog_T *
nfa_regcomp(char_u *রাশিমালা, পূর্ণসংখ্যা re_flags)
{
    nfa_regprog_T	*prog = ফাঁকা;
    size_t		prog_আকার;
    পূর্ণসংখ্যা			*postfix;

    যদি (রাশিমালা == ফাঁকা)
	ফেরত ফাঁকা;

#ifdef DEBUG
    nfa_regengine.রাশিমালা = রাশিমালা;
#endif
    nfa_re_flags = re_flags;

    init_class_tab();

    যদি (nfa_regcomp_start(রাশিমালা, re_flags) == FAIL)
	ফেরত ফাঁকা;

    // Build postfix form of the regexp. Needed to build the NFA
    // (and count its size).
    postfix = re2post();
    যদি (postfix == ফাঁকা)
	যাও fail;	    // Cascaded (syntax?) error

    /*
     * In order to build the NFA, we parse the input regexp twice:
     * 1. first pass to count size (so we can allocate space)
     * 2. second to emit code
     */
#ifdef ENABLE_LOG
    {
	FILE *f = fopen(NFA_REGEXP_RUN_LOG, "a");

	যদি (f != ফাঁকা)
	{
	    fprintf(f, "\n*****************************\n\n\n\n\tCompiling regexp \"%s\"... hold on !\n", রাশিমালা);
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
    prog_আকার = offsetof(nfa_regprog_T, state) + আকার(nfa_state_T) * nstate;
    prog = alloc(prog_আকার);
    যদি (prog == ফাঁকা)
	যাও fail;
    state_ptr = prog->state;
    prog->re_in_use = FALSE;
    prog->listbuf[0] = ফাঁকা;
    prog->listbuf[1] = ফাঁকা;

    /*
     * PASS 2
     * Build the NFA
     */
    prog->start = post2nfa(postfix, post_ptr, FALSE);
    যদি (prog->start == ফাঁকা)
	যাও fail;

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
    nfa_postfix_dump(রাশিমালা, OK);
    nfa_dump(prog);
#endif
#ifdef FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    prog->reghasz = re_has_z;
#endif
    prog->pattern = vim_strsave(রাশিমালা);
#ifdef DEBUG
    nfa_regengine.রাশিমালা = ফাঁকা;
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
    post_ptr = post_end = ফাঁকা;
    যদি (nfa_স্তূপ_len > NFA_স্তূপ_KEEP)
    {
	VIM_CLEAR(nfa_স্তূপ);
	nfa_স্তূপ_len = 0;
    }
    state_ptr = ফাঁকা;
    ফেরত (regprog_T *)prog;

fail:
    VIM_CLEAR(prog);
#ifdef ENABLE_LOG
    nfa_postfix_dump(রাশিমালা, FAIL);
#endif
#ifdef DEBUG
    nfa_regengine.রাশিমালা = ফাঁকা;
#endif
    যাও out;
}

/*
 * Free a compiled regexp program, returned by nfa_regcomp().
 */
    স্থির শূন্য
nfa_regfree(regprog_T *prog)
{
    যদি (prog == ফাঁকা)
	ফেরত;

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
    স্থির পূর্ণসংখ্যা
nfa_regexec_nl(
    regmatch_T	*rmp,
    char_u	*সারি,	// string to match against
    colnr_T	col,	// column to start looking for match
    পূর্ণসংখ্যা		সারি_lbr)
{
    rex.reg_match = rmp;
    rex.reg_mmatch = ফাঁকা;
    rex.reg_maxline = 0;
    rex.reg_সারি_lbr = সারি_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = ফাঁকা;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;
    ফেরত nfa_regexec_both(সারি, col, ফাঁকা);
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
    পূর্ণসংখ্যা		*timed_out)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    ফেরত nfa_regexec_both(ফাঁকা, col, timed_out);
}

#ifdef DEBUG
# undef ENABLE_LOG
#endif
