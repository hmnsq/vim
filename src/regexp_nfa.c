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

ગણના
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
સ્થિર અચલ પૂર્ણાંક nfa_classcodes[] = {
    NFA_ANY, NFA_IDENT, NFA_SIDENT, NFA_KWORD,NFA_SKWORD,
    NFA_FNAME, NFA_SFNAME, NFA_PRINT, NFA_SPRINT,
    NFA_WHITE, NFA_NWHITE, NFA_DIGIT, NFA_NDIGIT,
    NFA_HEX, NFA_NHEX, NFA_OCTAL, NFA_NOCTAL,
    NFA_WORD, NFA_NWORD, NFA_HEAD, NFA_NHEAD,
    NFA_ALPHA, NFA_NALPHA, NFA_LOWER, NFA_NLOWER,
    NFA_UPPER, NFA_NUPPER
};

// Variables only used in nfa_regcomp() and descendants.
સ્થિર પૂર્ણાંક nfa_re_flags; // re_flags passed to nfa_regcomp()
સ્થિર પૂર્ણાંક *post_start;  // holds the postfix form of r.e.
સ્થિર પૂર્ણાંક post_start_len;  // size of allocated post_start (in ints)
સ્થિર પૂર્ણાંક *post_end;
સ્થિર પૂર્ણાંક *post_ptr;
સ્થિર પૂર્ણાંક nfa_reg_parse_depth;	// nesting depth in nfa_reg()

// The postfix list (post_start) and fragment stack (nfa_stack) are reused
// across compilations; a buffer that grew past these sizes for a big pattern
// is freed afterwards instead of being kept, to avoid holding much memory.
#define NFA_POSTFIX_KEEP	10000	// number of ints (~40 Kbyte)
#define NFA_ઢગલો_KEEP		4000	// number of Frag_T (~64 Kbyte)

// Set when the pattern should use the NFA engine.
// E.g. [[:upper:]] only allows 8bit characters for BT engine,
// while NFA engine handles multibyte characters correctly.
સ્થિર પૂર્ણાંક wants_nfa;

સ્થિર પૂર્ણાંક nstate;	// Number of states in the NFA.
સ્થિર પૂર્ણાંક istate;	// Index in the state vector, used in alloc_state()

// If not NULL match must end at this position
સ્થિર save_se_T *nfa_endp = ખાલી;

// 0 for first call to nfa_regmatch(), 1 for recursive call.
સ્થિર પૂર્ણાંક nfa_ll_index = 0;

સ્થિર પૂર્ણાંક realloc_post_list(શૂન્ય);
સ્થિર પૂર્ણાંક nfa_reg(પૂર્ણાંક paren);
#ifdef DEBUG
સ્થિર શૂન્ય nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent);
#endif
સ્થિર પૂર્ણાંક match_follows(nfa_state_T *startstate, પૂર્ણાંક depth);
સ્થિર પૂર્ણાંક failure_chance(nfa_state_T *state, પૂર્ણાંક depth);

// helper functions used when doing re2post() ... regatom() parsing
#define EMIT(c)	કરો {				\
		    જો (post_ptr >= post_end && realloc_post_list() == FAIL) \
			પાછા FAIL;		\
		    *post_ptr++ = c;		\
		} જ્યારે (0)

/*
 * Initialize internal variables before NFA compilation.
 * Return OK on success, FAIL otherwise.
 */
    સ્થિર પૂર્ણાંક
nfa_regcomp_start(
    char_u	*પદાવલિ,
    પૂર્ણાંક		re_flags)	    // see vim_regcomp()
{
    size_t	postfix_size;
    પૂર્ણાંક		nstate_max;

    nstate = 0;
    istate = 0;
    // A reasonable estimation for maximum size
    nstate_max = (પૂર્ણાંક)(STRLEN(પદાવલિ) + 1) * 25;

    // Some items blow up in size, such as [A-z].  Add more space for that.
    // When it is still not enough realloc_post_list() will be used.
    nstate_max += 1000;

    // Size for postfix representation of expr.
    postfix_size = કદ(પૂર્ણાંક) * nstate_max;

    // Reuse the postfix buffer across compilations, only growing it when the
    // estimate exceeds the current size; it is freed in free_regexp_stuff().
    જો (post_start == ખાલી || post_start_len < nstate_max)
    {
	vim_free(post_start);
	post_start = alloc(postfix_size);
	જો (post_start == ખાલી)
	{
	    post_start_len = 0;
	    પાછા FAIL;
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
    regcomp_start(પદાવલિ, re_flags);

    પાછા OK;
}

/*
 * Figure out if the NFA state list starts with an anchor, must match at start
 * of the line.
 */
    સ્થિર પૂર્ણાંક
nfa_get_reganch(nfa_state_T *start, પૂર્ણાંક depth)
{
    nfa_state_T *p = start;

    જો (depth > 4)
	પાછા 0;

    જ્યારે (p != ખાલી)
    {
	પસંદગી (p->c)
	{
	    સ્થિતિ NFA_BOL:
	    સ્થિતિ NFA_BOF:
		પાછા 1; // yes!

	    સ્થિતિ NFA_ZSTART:
	    સ્થિતિ NFA_ZEND:
	    સ્થિતિ NFA_CURSOR:
	    સ્થિતિ NFA_VISUAL:

	    સ્થિતિ NFA_MOPEN:
	    સ્થિતિ NFA_MOPEN1:
	    સ્થિતિ NFA_MOPEN2:
	    સ્થિતિ NFA_MOPEN3:
	    સ્થિતિ NFA_MOPEN4:
	    સ્થિતિ NFA_MOPEN5:
	    સ્થિતિ NFA_MOPEN6:
	    સ્થિતિ NFA_MOPEN7:
	    સ્થિતિ NFA_MOPEN8:
	    સ્થિતિ NFA_MOPEN9:
	    સ્થિતિ NFA_NOPEN:
#ifdef FEAT_SYN_HL
	    સ્થિતિ NFA_ZOPEN:
	    સ્થિતિ NFA_ZOPEN1:
	    સ્થિતિ NFA_ZOPEN2:
	    સ્થિતિ NFA_ZOPEN3:
	    સ્થિતિ NFA_ZOPEN4:
	    સ્થિતિ NFA_ZOPEN5:
	    સ્થિતિ NFA_ZOPEN6:
	    સ્થિતિ NFA_ZOPEN7:
	    સ્થિતિ NFA_ZOPEN8:
	    સ્થિતિ NFA_ZOPEN9:
#endif
		p = p->out;
		તોડો;

	    સ્થિતિ NFA_SPLIT:
		પાછા nfa_get_reganch(p->out, depth + 1)
				       && nfa_get_reganch(p->out1, depth + 1);

	    મૂળભૂત:
		પાછા 0; // noooo
	}
    }
    પાછા 0;
}

/*
 * Figure out if the NFA state list starts with a character which must match
 * at start of the match.
 */
    સ્થિર પૂર્ણાંક
nfa_get_regstart(nfa_state_T *start, પૂર્ણાંક depth)
{
    nfa_state_T *p = start;

    જો (depth > 4)
	પાછા 0;

    જ્યારે (p != ખાલી)
    {
	પસંદગી (p->c)
	{
	    // all kinds of zero-width matches
	    સ્થિતિ NFA_BOL:
	    સ્થિતિ NFA_BOF:
	    સ્થિતિ NFA_BOW:
	    સ્થિતિ NFA_EOW:
	    સ્થિતિ NFA_ZSTART:
	    સ્થિતિ NFA_ZEND:
	    સ્થિતિ NFA_CURSOR:
	    સ્થિતિ NFA_VISUAL:
	    સ્થિતિ NFA_LNUM:
	    સ્થિતિ NFA_LNUM_GT:
	    સ્થિતિ NFA_LNUM_LT:
	    સ્થિતિ NFA_COL:
	    સ્થિતિ NFA_COL_GT:
	    સ્થિતિ NFA_COL_LT:
	    સ્થિતિ NFA_VCOL:
	    સ્થિતિ NFA_VCOL_GT:
	    સ્થિતિ NFA_VCOL_LT:
	    સ્થિતિ NFA_MARK:
	    સ્થિતિ NFA_MARK_GT:
	    સ્થિતિ NFA_MARK_LT:

	    સ્થિતિ NFA_MOPEN:
	    સ્થિતિ NFA_MOPEN1:
	    સ્થિતિ NFA_MOPEN2:
	    સ્થિતિ NFA_MOPEN3:
	    સ્થિતિ NFA_MOPEN4:
	    સ્થિતિ NFA_MOPEN5:
	    સ્થિતિ NFA_MOPEN6:
	    સ્થિતિ NFA_MOPEN7:
	    સ્થિતિ NFA_MOPEN8:
	    સ્થિતિ NFA_MOPEN9:
	    સ્થિતિ NFA_NOPEN:
#ifdef FEAT_SYN_HL
	    સ્થિતિ NFA_ZOPEN:
	    સ્થિતિ NFA_ZOPEN1:
	    સ્થિતિ NFA_ZOPEN2:
	    સ્થિતિ NFA_ZOPEN3:
	    સ્થિતિ NFA_ZOPEN4:
	    સ્થિતિ NFA_ZOPEN5:
	    સ્થિતિ NFA_ZOPEN6:
	    સ્થિતિ NFA_ZOPEN7:
	    સ્થિતિ NFA_ZOPEN8:
	    સ્થિતિ NFA_ZOPEN9:
#endif
		p = p->out;
		તોડો;

	    સ્થિતિ NFA_SPLIT:
	    {
		પૂર્ણાંક c1 = nfa_get_regstart(p->out, depth + 1);
		પૂર્ણાંક c2 = nfa_get_regstart(p->out1, depth + 1);

		જો (c1 == c2)
		    પાછા c1; // yes!
		પાછા 0;
	    }

	    મૂળભૂત:
		જો (p->c > 0)
		    પાછા p->c; // yes!
		પાછા 0;
	}
    }
    પાછા 0;
}

/*
 * Figure out if the NFA state list contains just literal text and nothing
 * else.  If so return a string in allocated memory with what must match after
 * regstart.  Otherwise return NULL.
 */
    સ્થિર char_u *
nfa_get_match_text(nfa_state_T *start)
{
    nfa_state_T *p = start;
    પૂર્ણાંક		len = 0;
    char_u	*ret;
    char_u	*s;

    જો (p->c != NFA_MOPEN)
	પાછા ખાલી; // just in case
    p = p->out;
    જ્યારે (p->c > 0)
    {
	len += MB_CHAR2LEN(p->c);
	p = p->out;
    }
    જો (p->c != NFA_MCLOSE || p->out->c != NFA_MATCH)
	પાછા ખાલી;

    ret = alloc(len);
    જો (ret == ખાલી)
	પાછા ખાલી;

    p = start->out->out; // skip first char, it goes into regstart
    s = ret;
    જ્યારે (p->c > 0)
    {
	જો (has_mbyte)
	    s += (*mb_char2bytes)(p->c, s);
	નહીંતર
	    *s++ = p->c;
	p = p->out;
    }
    *s = NUL;
    પાછા ret;
}

/*
 * Allocate more space for post_start.  Called when
 * running above the estimated number of states.
 */
    સ્થિર પૂર્ણાંક
realloc_post_list(શૂન્ય)
{
    પૂર્ણાંક   nstate_max = (પૂર્ણાંક)(post_end - post_start);
    પૂર્ણાંક   new_max;
    પૂર્ણાંક   *new_start;
    પૂર્ણાંક	  *old_start;

    // For weird patterns the number of states can be very high. Increasing by
    // 50% seems a reasonable compromise between memory use and speed.
    new_max = nstate_max * 3 / 2;
    new_start = ALLOC_MULT(પૂર્ણાંક, new_max);
    જો (new_start == ખાલી)
	પાછા FAIL;
    mch_memmove(new_start, post_start, nstate_max * કદ(પૂર્ણાંક));
    old_start = post_start;
    post_start = new_start;
    post_start_len = new_max;
    post_ptr = new_start + (post_ptr - old_start);
    post_end = post_start + new_max;
    vim_free(old_start);
    પાછા OK;
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
    સ્થિર પૂર્ણાંક
nfa_recognize_char_class(char_u *start, char_u *end, પૂર્ણાંક extra_newl)
{
    પૂર્ણાંક		newl;
    પૂર્ણાંક		config = get_char_class_bits(start, end, &newl);

    જો (config < 0)
	પાછા FAIL;

    જો (extra_newl == TRUE)
	newl = TRUE;

    જો (newl == TRUE)
	extra_newl = NFA_ADD_NL;

    પસંદગી (config)
    {
	સ્થિતિ CLASS_o9:
	    પાછા extra_newl + NFA_DIGIT;
	સ્થિતિ CLASS_not |  CLASS_o9:
	    પાછા extra_newl + NFA_NDIGIT;
	સ્થિતિ CLASS_af | CLASS_AF | CLASS_o9:
	    પાછા extra_newl + NFA_HEX;
	સ્થિતિ CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    પાછા extra_newl + NFA_NHEX;
	સ્થિતિ CLASS_o7:
	    પાછા extra_newl + NFA_OCTAL;
	સ્થિતિ CLASS_not | CLASS_o7:
	    પાછા extra_newl + NFA_NOCTAL;
	સ્થિતિ CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    પાછા extra_newl + NFA_WORD;
	સ્થિતિ CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    પાછા extra_newl + NFA_NWORD;
	સ્થિતિ CLASS_az | CLASS_AZ | CLASS_underscore:
	    પાછા extra_newl + NFA_HEAD;
	સ્થિતિ CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    પાછા extra_newl + NFA_NHEAD;
	સ્થિતિ CLASS_az | CLASS_AZ:
	    પાછા extra_newl + NFA_ALPHA;
	સ્થિતિ CLASS_not | CLASS_az | CLASS_AZ:
	    પાછા extra_newl + NFA_NALPHA;
	સ્થિતિ CLASS_az:
	   પાછા extra_newl + NFA_LOWER_IC;
	સ્થિતિ CLASS_not | CLASS_az:
	    પાછા extra_newl + NFA_NLOWER_IC;
	સ્થિતિ CLASS_AZ:
	    પાછા extra_newl + NFA_UPPER_IC;
	સ્થિતિ CLASS_not | CLASS_AZ:
	    પાછા extra_newl + NFA_NUPPER_IC;
    }
    પાછા FAIL;
}

/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * Emits bytes in postfix notation: 'a,b,NFA_OR,c,NFA_OR' is
 * equivalent to 'a OR b OR c'
 *
 * NOTE! When changing this function, also update reg_equi_class()
 */
    સ્થિર પૂર્ણાંક
nfa_emit_equi_class(પૂર્ણાંક c)
{
#define EMIT2(c)    EMIT(c); EMIT(NFA_CONCAT);

    જો (enc_utf8 || STRCMP(p_enc, "latin1") == 0
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
	પસંદગી (c)
	{
	    સ્થિતિ 'A': સ્થિતિ A_grave: સ્થિતિ A_acute: સ્થિતિ A_circumflex:
	    સ્થિતિ A_virguilla: સ્થિતિ A_diaeresis: સ્થિતિ A_ring:
	    સ્થિતિ 0x100: સ્થિતિ 0x102: સ્થિતિ 0x104: સ્થિતિ 0x1cd:
	    સ્થિતિ 0x1de: સ્થિતિ 0x1e0: સ્થિતિ 0x1fa: સ્થિતિ 0x200:
	    સ્થિતિ 0x202: સ્થિતિ 0x226: સ્થિતિ 0x23a: સ્થિતિ 0x1e00:
	    સ્થિતિ 0x1ea0: સ્થિતિ 0x1ea2: સ્થિતિ 0x1ea4: સ્થિતિ 0x1ea6:
	    સ્થિતિ 0x1ea8: સ્થિતિ 0x1eaa: સ્થિતિ 0x1eac: સ્થિતિ 0x1eae:
	    સ્થિતિ 0x1eb0: સ્થિતિ 0x1eb2: સ્થિતિ 0x1eb4: સ્થિતિ 0x1eb6:
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
		    પાછા OK;

	    સ્થિતિ 'B': સ્થિતિ 0x181: સ્થિતિ 0x243: સ્થિતિ 0x1e02:
	    સ્થિતિ 0x1e04: સ્થિતિ 0x1e06:
		    EMIT2('B')
		    EMIT2(0x181) EMIT2(0x243) EMIT2(0x1e02)
		    EMIT2(0x1e04) EMIT2(0x1e06)
		    પાછા OK;

	    સ્થિતિ 'C': સ્થિતિ C_cedilla: સ્થિતિ 0x106: સ્થિતિ 0x108:
	    સ્થિતિ 0x10a: સ્થિતિ 0x10c: સ્થિતિ 0x187: સ્થિતિ 0x23b:
	    સ્થિતિ 0x1e08: સ્થિતિ 0xa792:
		    EMIT2('C') EMIT2(C_cedilla)
		    EMIT2(0x106) EMIT2(0x108) EMIT2(0x10a)
		    EMIT2(0x10c) EMIT2(0x187) EMIT2(0x23b)
		    EMIT2(0x1e08) EMIT2(0xa792)
		    પાછા OK;

	    સ્થિતિ 'D': સ્થિતિ 0x10e: સ્થિતિ 0x110: સ્થિતિ 0x18a:
	    સ્થિતિ 0x1e0a: સ્થિતિ 0x1e0c: સ્થિતિ 0x1e0e: સ્થિતિ 0x1e10:
	    સ્થિતિ 0x1e12:
		    EMIT2('D') EMIT2(0x10e) EMIT2(0x110) EMIT2(0x18a)
		    EMIT2(0x1e0a) EMIT2(0x1e0c) EMIT2(0x1e0e)
		    EMIT2(0x1e10) EMIT2(0x1e12)
		    પાછા OK;

	    સ્થિતિ 'E': સ્થિતિ E_grave: સ્થિતિ E_acute: સ્થિતિ E_circumflex:
	    સ્થિતિ E_diaeresis: સ્થિતિ 0x112: સ્થિતિ 0x114: સ્થિતિ 0x116:
	    સ્થિતિ 0x118: સ્થિતિ 0x11a: સ્થિતિ 0x204: સ્થિતિ 0x206:
	    સ્થિતિ 0x228: સ્થિતિ 0x246: સ્થિતિ 0x1e14: સ્થિતિ 0x1e16:
	    સ્થિતિ 0x1e18: સ્થિતિ 0x1e1a: સ્થિતિ 0x1e1c: સ્થિતિ 0x1eb8:
	    સ્થિતિ 0x1eba: સ્થિતિ 0x1ebc: સ્થિતિ 0x1ebe: સ્થિતિ 0x1ec0:
	    સ્થિતિ 0x1ec2: સ્થિતિ 0x1ec4: સ્થિતિ 0x1ec6:
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
		    પાછા OK;

	    સ્થિતિ 'F': સ્થિતિ 0x191: સ્થિતિ 0x1e1e: સ્થિતિ 0xa798:
		    EMIT2('F') EMIT2(0x191) EMIT2(0x1e1e) EMIT2(0xa798)
		    પાછા OK;

	    સ્થિતિ 'G': સ્થિતિ 0x11c: સ્થિતિ 0x11e: સ્થિતિ 0x120:
	    સ્થિતિ 0x122: સ્થિતિ 0x193: સ્થિતિ 0x1e4: સ્થિતિ 0x1e6:
	    સ્થિતિ 0x1f4: સ્થિતિ 0x1e20: સ્થિતિ 0xa7a0:
		    EMIT2('G') EMIT2(0x11c) EMIT2(0x11e) EMIT2(0x120)
		    EMIT2(0x122) EMIT2(0x193) EMIT2(0x1e4)
		    EMIT2(0x1e6) EMIT2(0x1f4) EMIT2(0x1e20)
		    EMIT2(0xa7a0)
		    પાછા OK;

	    સ્થિતિ 'H': સ્થિતિ 0x124: સ્થિતિ 0x126: સ્થિતિ 0x21e:
	    સ્થિતિ 0x1e22: સ્થિતિ 0x1e24: સ્થિતિ 0x1e26: સ્થિતિ 0x1e28:
	    સ્થિતિ 0x1e2a: સ્થિતિ 0x2c67:
		    EMIT2('H') EMIT2(0x124) EMIT2(0x126) EMIT2(0x21e)
		    EMIT2(0x1e22) EMIT2(0x1e24) EMIT2(0x1e26)
		    EMIT2(0x1e28) EMIT2(0x1e2a) EMIT2(0x2c67)
		    પાછા OK;

	    સ્થિતિ 'I': સ્થિતિ I_grave: સ્થિતિ I_acute: સ્થિતિ I_circumflex:
	    સ્થિતિ I_diaeresis: સ્થિતિ 0x128: સ્થિતિ 0x12a: સ્થિતિ 0x12c:
	    સ્થિતિ 0x12e: સ્થિતિ 0x130: સ્થિતિ 0x197: સ્થિતિ 0x1cf:
	    સ્થિતિ 0x208: સ્થિતિ 0x20a: સ્થિતિ 0x1e2c: સ્થિતિ 0x1e2e:
	    સ્થિતિ 0x1ec8: સ્થિતિ 0x1eca:
		    EMIT2('I') EMIT2(I_grave) EMIT2(I_acute)
		    EMIT2(I_circumflex) EMIT2(I_diaeresis)
		    EMIT2(0x128) EMIT2(0x12a) EMIT2(0x12c)
		    EMIT2(0x12e) EMIT2(0x130) EMIT2(0x197)
		    EMIT2(0x1cf) EMIT2(0x208) EMIT2(0x20a)
		    EMIT2(0x1e2c) EMIT2(0x1e2e) EMIT2(0x1ec8)
		    EMIT2(0x1eca)
		    પાછા OK;

	    સ્થિતિ 'J': સ્થિતિ 0x134: સ્થિતિ 0x248:
		    EMIT2('J') EMIT2(0x134) EMIT2(0x248)
		    પાછા OK;

	    સ્થિતિ 'K': સ્થિતિ 0x136: સ્થિતિ 0x198: સ્થિતિ 0x1e8: સ્થિતિ 0x1e30:
	    સ્થિતિ 0x1e32: સ્થિતિ 0x1e34: સ્થિતિ 0x2c69: સ્થિતિ 0xa740:
		    EMIT2('K') EMIT2(0x136) EMIT2(0x198) EMIT2(0x1e8)
		    EMIT2(0x1e30) EMIT2(0x1e32) EMIT2(0x1e34)
		    EMIT2(0x2c69) EMIT2(0xa740)
		    પાછા OK;

	    સ્થિતિ 'L': સ્થિતિ 0x139: સ્થિતિ 0x13b: સ્થિતિ 0x13d:
	    સ્થિતિ 0x13f: સ્થિતિ 0x141: સ્થિતિ 0x23d: સ્થિતિ 0x1e36:
	    સ્થિતિ 0x1e38: સ્થિતિ 0x1e3a: સ્થિતિ 0x1e3c: સ્થિતિ 0x2c60:
		    EMIT2('L') EMIT2(0x139) EMIT2(0x13b)
		    EMIT2(0x13d) EMIT2(0x13f) EMIT2(0x141)
		    EMIT2(0x23d) EMIT2(0x1e36) EMIT2(0x1e38)
		    EMIT2(0x1e3a) EMIT2(0x1e3c) EMIT2(0x2c60)
		    પાછા OK;

	    સ્થિતિ 'M': સ્થિતિ 0x1e3e: સ્થિતિ 0x1e40: સ્થિતિ 0x1e42:
		    EMIT2('M') EMIT2(0x1e3e) EMIT2(0x1e40)
		    EMIT2(0x1e42)
		    પાછા OK;

	    સ્થિતિ 'N': સ્થિતિ N_virguilla:
	    સ્થિતિ 0x143: સ્થિતિ 0x145: સ્થિતિ 0x147: સ્થિતિ 0x1f8:
	    સ્થિતિ 0x1e44: સ્થિતિ 0x1e46: સ્થિતિ 0x1e48: સ્થિતિ 0x1e4a:
	    સ્થિતિ 0xa7a4:
		    EMIT2('N') EMIT2(N_virguilla)
		    EMIT2(0x143) EMIT2(0x145) EMIT2(0x147)
		    EMIT2(0x1f8) EMIT2(0x1e44) EMIT2(0x1e46)
		    EMIT2(0x1e48) EMIT2(0x1e4a) EMIT2(0xa7a4)
		    પાછા OK;

	    સ્થિતિ 'O': સ્થિતિ O_grave: સ્થિતિ O_acute: સ્થિતિ O_circumflex:
	    સ્થિતિ O_virguilla: સ્થિતિ O_diaeresis: સ્થિતિ O_slash:
	    સ્થિતિ 0x14c: સ્થિતિ 0x14e: સ્થિતિ 0x150: સ્થિતિ 0x19f:
	    સ્થિતિ 0x1a0: સ્થિતિ 0x1d1: સ્થિતિ 0x1ea: સ્થિતિ 0x1ec:
	    સ્થિતિ 0x1fe: સ્થિતિ 0x20c: સ્થિતિ 0x20e: સ્થિતિ 0x22a:
	    સ્થિતિ 0x22c: સ્થિતિ 0x22e: સ્થિતિ 0x230: સ્થિતિ 0x1e4c:
	    સ્થિતિ 0x1e4e: સ્થિતિ 0x1e50: સ્થિતિ 0x1e52: સ્થિતિ 0x1ecc:
	    સ્થિતિ 0x1ece: સ્થિતિ 0x1ed0: સ્થિતિ 0x1ed2: સ્થિતિ 0x1ed4:
	    સ્થિતિ 0x1ed6: સ્થિતિ 0x1ed8: સ્થિતિ 0x1eda: સ્થિતિ 0x1edc:
	    સ્થિતિ 0x1ede: સ્થિતિ 0x1ee0: સ્થિતિ 0x1ee2:
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
		    પાછા OK;

	    સ્થિતિ 'P': સ્થિતિ 0x1a4: સ્થિતિ 0x1e54: સ્થિતિ 0x1e56: સ્થિતિ 0x2c63:
		    EMIT2('P') EMIT2(0x1a4) EMIT2(0x1e54) EMIT2(0x1e56)
		    EMIT2(0x2c63)
		    પાછા OK;

	    સ્થિતિ 'Q': સ્થિતિ 0x24a:
		    EMIT2('Q') EMIT2(0x24a)
		    પાછા OK;

	    સ્થિતિ 'R': સ્થિતિ 0x154: સ્થિતિ 0x156: સ્થિતિ 0x158: સ્થિતિ 0x210:
	    સ્થિતિ 0x212: સ્થિતિ 0x24c: સ્થિતિ 0x1e58: સ્થિતિ 0x1e5a:
	    સ્થિતિ 0x1e5c: સ્થિતિ 0x1e5e: સ્થિતિ 0x2c64: સ્થિતિ 0xa7a6:
		    EMIT2('R') EMIT2(0x154) EMIT2(0x156) EMIT2(0x158)
		    EMIT2(0x210) EMIT2(0x212) EMIT2(0x24c) EMIT2(0x1e58)
		    EMIT2(0x1e5a) EMIT2(0x1e5c) EMIT2(0x1e5e) EMIT2(0x2c64)
		    EMIT2(0xa7a6)
		    પાછા OK;

	    સ્થિતિ 'S': સ્થિતિ 0x15a: સ્થિતિ 0x15c: સ્થિતિ 0x15e: સ્થિતિ 0x160:
	    સ્થિતિ 0x218: સ્થિતિ 0x1e60: સ્થિતિ 0x1e62: સ્થિતિ 0x1e64:
	    સ્થિતિ 0x1e66: સ્થિતિ 0x1e68: સ્થિતિ 0x2c7e: સ્થિતિ 0xa7a8:
		    EMIT2('S') EMIT2(0x15a) EMIT2(0x15c) EMIT2(0x15e)
		    EMIT2(0x160) EMIT2(0x218) EMIT2(0x1e60) EMIT2(0x1e62)
		    EMIT2(0x1e64) EMIT2(0x1e66) EMIT2(0x1e68) EMIT2(0x2c7e)
		    EMIT2(0xa7a8)
		    પાછા OK;

	    સ્થિતિ 'T': સ્થિતિ 0x162: સ્થિતિ 0x164: સ્થિતિ 0x166: સ્થિતિ 0x1ac:
	    સ્થિતિ 0x1ae: સ્થિતિ 0x21a: સ્થિતિ 0x23e: સ્થિતિ 0x1e6a: સ્થિતિ 0x1e6c:
	    સ્થિતિ 0x1e6e: સ્થિતિ 0x1e70:
		    EMIT2('T') EMIT2(0x162) EMIT2(0x164) EMIT2(0x166)
		    EMIT2(0x1ac) EMIT2(0x1ae) EMIT2(0x23e) EMIT2(0x21a)
		    EMIT2(0x1e6a) EMIT2(0x1e6c) EMIT2(0x1e6e) EMIT2(0x1e70)
		    પાછા OK;

	    સ્થિતિ 'U': સ્થિતિ U_grave: સ્થિતિ U_acute: સ્થિતિ U_diaeresis:
	    સ્થિતિ U_circumflex: સ્થિતિ 0x168: સ્થિતિ 0x16a: સ્થિતિ 0x16c:
	    સ્થિતિ 0x16e: સ્થિતિ 0x170: સ્થિતિ 0x172: સ્થિતિ 0x1af:
	    સ્થિતિ 0x1d3: સ્થિતિ 0x1d5: સ્થિતિ 0x1d7: સ્થિતિ 0x1d9:
	    સ્થિતિ 0x1db: સ્થિતિ 0x214: સ્થિતિ 0x216: સ્થિતિ 0x244:
	    સ્થિતિ 0x1e72: સ્થિતિ 0x1e74: સ્થિતિ 0x1e76: સ્થિતિ 0x1e78:
	    સ્થિતિ 0x1e7a: સ્થિતિ 0x1ee4: સ્થિતિ 0x1ee6: સ્થિતિ 0x1ee8:
	    સ્થિતિ 0x1eea: સ્થિતિ 0x1eec: સ્થિતિ 0x1eee: સ્થિતિ 0x1ef0:
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
		    પાછા OK;

	    સ્થિતિ 'V': સ્થિતિ 0x1b2: સ્થિતિ 0x1e7c: સ્થિતિ 0x1e7e:
		    EMIT2('V') EMIT2(0x1b2) EMIT2(0x1e7c) EMIT2(0x1e7e)
		    પાછા OK;

	    સ્થિતિ 'W': સ્થિતિ 0x174: સ્થિતિ 0x1e80: સ્થિતિ 0x1e82: સ્થિતિ 0x1e84:
	    સ્થિતિ 0x1e86: સ્થિતિ 0x1e88:
		    EMIT2('W') EMIT2(0x174) EMIT2(0x1e80) EMIT2(0x1e82)
		    EMIT2(0x1e84) EMIT2(0x1e86) EMIT2(0x1e88)
		    પાછા OK;

	    સ્થિતિ 'X': સ્થિતિ 0x1e8a: સ્થિતિ 0x1e8c:
		    EMIT2('X') EMIT2(0x1e8a) EMIT2(0x1e8c)
		    પાછા OK;

	    સ્થિતિ 'Y': સ્થિતિ Y_acute: સ્થિતિ 0x176: સ્થિતિ 0x178:
	    સ્થિતિ 0x1b3: સ્થિતિ 0x232: સ્થિતિ 0x24e: સ્થિતિ 0x1e8e:
	    સ્થિતિ 0x1ef2: સ્થિતિ 0x1ef4: સ્થિતિ 0x1ef6: સ્થિતિ 0x1ef8:
		    EMIT2('Y') EMIT2(Y_acute)
		    EMIT2(0x176) EMIT2(0x178) EMIT2(0x1b3)
		    EMIT2(0x232) EMIT2(0x24e) EMIT2(0x1e8e)
		    EMIT2(0x1ef2) EMIT2(0x1ef4) EMIT2(0x1ef6)
		    EMIT2(0x1ef8)
		    પાછા OK;

	    સ્થિતિ 'Z': સ્થિતિ 0x179: સ્થિતિ 0x17b: સ્થિતિ 0x17d:
	    સ્થિતિ 0x1b5: સ્થિતિ 0x1e90: સ્થિતિ 0x1e92: સ્થિતિ 0x1e94:
	    સ્થિતિ 0x2c6b:
		    EMIT2('Z') EMIT2(0x179) EMIT2(0x17b) EMIT2(0x17d)
		    EMIT2(0x1b5) EMIT2(0x1e90) EMIT2(0x1e92)
		    EMIT2(0x1e94) EMIT2(0x2c6b)
		    પાછા OK;

	    સ્થિતિ 'a': સ્થિતિ a_grave: સ્થિતિ a_acute: સ્થિતિ a_circumflex:
	    સ્થિતિ a_virguilla: સ્થિતિ a_diaeresis: સ્થિતિ a_ring:
	    સ્થિતિ 0x101: સ્થિતિ 0x103: સ્થિતિ 0x105: સ્થિતિ 0x1ce:
	    સ્થિતિ 0x1df: સ્થિતિ 0x1e1: સ્થિતિ 0x1fb: સ્થિતિ 0x201:
	    સ્થિતિ 0x203: સ્થિતિ 0x227: સ્થિતિ 0x1d8f: સ્થિતિ 0x1e01:
	    સ્થિતિ 0x1e9a: સ્થિતિ 0x1ea1: સ્થિતિ 0x1ea3: સ્થિતિ 0x1ea5:
	    સ્થિતિ 0x1ea7: સ્થિતિ 0x1ea9: સ્થિતિ 0x1eab: સ્થિતિ 0x1ead:
	    સ્થિતિ 0x1eaf: સ્થિતિ 0x1eb1: સ્થિતિ 0x1eb3: સ્થિતિ 0x1eb5:
	    સ્થિતિ 0x1eb7: સ્થિતિ 0x2c65:
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
		    પાછા OK;

	    સ્થિતિ 'b': સ્થિતિ 0x180: સ્થિતિ 0x253: સ્થિતિ 0x1d6c: સ્થિતિ 0x1d80:
	    સ્થિતિ 0x1e03: સ્થિતિ 0x1e05: સ્થિતિ 0x1e07:
		    EMIT2('b') EMIT2(0x180) EMIT2(0x253) EMIT2(0x1d6c)
		    EMIT2(0x1d80) EMIT2(0x1e03) EMIT2(0x1e05) EMIT2(0x1e07)
		    પાછા OK;

	    સ્થિતિ 'c': સ્થિતિ c_cedilla: સ્થિતિ 0x107: સ્થિતિ 0x109: સ્થિતિ 0x10b:
	    સ્થિતિ 0x10d: સ્થિતિ 0x188: સ્થિતિ 0x23c: સ્થિતિ 0x1e09: સ્થિતિ 0xa793:
	    સ્થિતિ 0xa794:
		    EMIT2('c') EMIT2(c_cedilla)
		    EMIT2(0x107) EMIT2(0x109) EMIT2(0x10b)
		    EMIT2(0x10d) EMIT2(0x188) EMIT2(0x23c)
		    EMIT2(0x1e09) EMIT2(0xa793) EMIT2(0xa794)
		    પાછા OK;

	    સ્થિતિ 'd': સ્થિતિ 0x10f: સ્થિતિ 0x111: સ્થિતિ 0x257: સ્થિતિ 0x1d6d:
	    સ્થિતિ 0x1d81: સ્થિતિ 0x1d91: સ્થિતિ 0x1e0b: સ્થિતિ 0x1e0d: સ્થિતિ 0x1e0f:
	    સ્થિતિ 0x1e11: સ્થિતિ 0x1e13:
		    EMIT2('d') EMIT2(0x10f) EMIT2(0x111)
		    EMIT2(0x257) EMIT2(0x1d6d) EMIT2(0x1d81)
		    EMIT2(0x1d91) EMIT2(0x1e0b) EMIT2(0x1e0d)
		    EMIT2(0x1e0f) EMIT2(0x1e11) EMIT2(0x1e13)
		    પાછા OK;

	    સ્થિતિ 'e': સ્થિતિ e_grave: સ્થિતિ e_acute: સ્થિતિ e_circumflex:
	    સ્થિતિ e_diaeresis: સ્થિતિ 0x113: સ્થિતિ 0x115: સ્થિતિ 0x117:
	    સ્થિતિ 0x119: સ્થિતિ 0x11b: સ્થિતિ 0x205: સ્થિતિ 0x207:
	    સ્થિતિ 0x229: સ્થિતિ 0x247: સ્થિતિ 0x1d92: સ્થિતિ 0x1e15:
	    સ્થિતિ 0x1e17: સ્થિતિ 0x1e19: સ્થિતિ 0x1e1b: સ્થિતિ 0x1e1d:
	    સ્થિતિ 0x1eb9: સ્થિતિ 0x1ebb: સ્થિતિ 0x1ebd: સ્થિતિ 0x1ebf:
	    સ્થિતિ 0x1ec1: સ્થિતિ 0x1ec3: સ્થિતિ 0x1ec5: સ્થિતિ 0x1ec7:
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
		    પાછા OK;

	    સ્થિતિ 'f': સ્થિતિ 0x192: સ્થિતિ 0x1d6e: સ્થિતિ 0x1d82:
	    સ્થિતિ 0x1e1f: સ્થિતિ 0xa799:
		    EMIT2('f') EMIT2(0x192) EMIT2(0x1d6e) EMIT2(0x1d82)
		    EMIT2(0x1e1f) EMIT2(0xa799)
		    પાછા OK;

	    સ્થિતિ 'g': સ્થિતિ 0x11d: સ્થિતિ 0x11f: સ્થિતિ 0x121: સ્થિતિ 0x123:
	    સ્થિતિ 0x1e5: સ્થિતિ 0x1e7: સ્થિતિ 0x1f5: સ્થિતિ 0x260: સ્થિતિ 0x1d83:
	    સ્થિતિ 0x1e21: સ્થિતિ 0xa7a1:
		    EMIT2('g') EMIT2(0x11d) EMIT2(0x11f) EMIT2(0x121)
		    EMIT2(0x123) EMIT2(0x1e5) EMIT2(0x1e7)
		    EMIT2(0x1f5) EMIT2(0x260) EMIT2(0x1d83)
		    EMIT2(0x1e21) EMIT2(0xa7a1)
		    પાછા OK;

	    સ્થિતિ 'h': સ્થિતિ 0x125: સ્થિતિ 0x127: સ્થિતિ 0x21f: સ્થિતિ 0x1e23:
	    સ્થિતિ 0x1e25: સ્થિતિ 0x1e27: સ્થિતિ 0x1e29: સ્થિતિ 0x1e2b:
	    સ્થિતિ 0x1e96: સ્થિતિ 0x2c68: સ્થિતિ 0xa795:
		    EMIT2('h') EMIT2(0x125) EMIT2(0x127) EMIT2(0x21f)
		    EMIT2(0x1e23) EMIT2(0x1e25) EMIT2(0x1e27)
		    EMIT2(0x1e29) EMIT2(0x1e2b) EMIT2(0x1e96)
		    EMIT2(0x2c68) EMIT2(0xa795)
		    પાછા OK;

	    સ્થિતિ 'i': સ્થિતિ i_grave: સ્થિતિ i_acute: સ્થિતિ i_circumflex:
	    સ્થિતિ i_diaeresis: સ્થિતિ 0x129: સ્થિતિ 0x12b: સ્થિતિ 0x12d:
	    સ્થિતિ 0x12f: સ્થિતિ 0x1d0: સ્થિતિ 0x209: સ્થિતિ 0x20b:
	    સ્થિતિ 0x268: સ્થિતિ 0x1d96: સ્થિતિ 0x1e2d: સ્થિતિ 0x1e2f:
	    સ્થિતિ 0x1ec9: સ્થિતિ 0x1ecb:
		    EMIT2('i') EMIT2(i_grave) EMIT2(i_acute)
		    EMIT2(i_circumflex) EMIT2(i_diaeresis)
		    EMIT2(0x129) EMIT2(0x12b) EMIT2(0x12d)
		    EMIT2(0x12f) EMIT2(0x1d0) EMIT2(0x209)
		    EMIT2(0x20b) EMIT2(0x268) EMIT2(0x1d96)
		    EMIT2(0x1e2d) EMIT2(0x1e2f) EMIT2(0x1ec9)
		    EMIT2(0x1ecb)
		    પાછા OK;

	    સ્થિતિ 'j': સ્થિતિ 0x135: સ્થિતિ 0x1f0: સ્થિતિ 0x249:
		    EMIT2('j') EMIT2(0x135) EMIT2(0x1f0) EMIT2(0x249)
		    પાછા OK;

	    સ્થિતિ 'k': સ્થિતિ 0x137: સ્થિતિ 0x199: સ્થિતિ 0x1e9: સ્થિતિ 0x1d84:
	    સ્થિતિ 0x1e31: સ્થિતિ 0x1e33: સ્થિતિ 0x1e35: સ્થિતિ 0x2c6a: સ્થિતિ 0xa741:
		    EMIT2('k') EMIT2(0x137) EMIT2(0x199) EMIT2(0x1e9)
		    EMIT2(0x1d84) EMIT2(0x1e31) EMIT2(0x1e33)
		    EMIT2(0x1e35) EMIT2(0x2c6a) EMIT2(0xa741)
		    પાછા OK;

	    સ્થિતિ 'l': સ્થિતિ 0x13a: સ્થિતિ 0x13c: સ્થિતિ 0x13e: સ્થિતિ 0x140:
	    સ્થિતિ 0x142: સ્થિતિ 0x19a: સ્થિતિ 0x1e37: સ્થિતિ 0x1e39: સ્થિતિ 0x1e3b:
	    સ્થિતિ 0x1e3d: સ્થિતિ 0x2c61:
		    EMIT2('l') EMIT2(0x13a) EMIT2(0x13c)
		    EMIT2(0x13e) EMIT2(0x140) EMIT2(0x142)
		    EMIT2(0x19a) EMIT2(0x1e37) EMIT2(0x1e39)
		    EMIT2(0x1e3b) EMIT2(0x1e3d) EMIT2(0x2c61)
		    પાછા OK;

	    સ્થિતિ 'm': સ્થિતિ 0x1d6f: સ્થિતિ 0x1e3f: સ્થિતિ 0x1e41: સ્થિતિ 0x1e43:
		    EMIT2('m') EMIT2(0x1d6f) EMIT2(0x1e3f)
		    EMIT2(0x1e41) EMIT2(0x1e43)
		    પાછા OK;

	    સ્થિતિ 'n': સ્થિતિ n_virguilla: સ્થિતિ 0x144: સ્થિતિ 0x146: સ્થિતિ 0x148:
	    સ્થિતિ 0x149: સ્થિતિ 0x1f9: સ્થિતિ 0x1d70: સ્થિતિ 0x1d87: સ્થિતિ 0x1e45:
	    સ્થિતિ 0x1e47: સ્થિતિ 0x1e49: સ્થિતિ 0x1e4b: સ્થિતિ 0xa7a5:
		    EMIT2('n') EMIT2(n_virguilla)
		    EMIT2(0x144) EMIT2(0x146) EMIT2(0x148)
		    EMIT2(0x149) EMIT2(0x1f9) EMIT2(0x1d70)
		    EMIT2(0x1d87) EMIT2(0x1e45) EMIT2(0x1e47)
		    EMIT2(0x1e49) EMIT2(0x1e4b) EMIT2(0xa7a5)
		    પાછા OK;

	    સ્થિતિ 'o': સ્થિતિ o_grave: સ્થિતિ o_acute: સ્થિતિ o_circumflex:
	    સ્થિતિ o_virguilla: સ્થિતિ o_diaeresis: સ્થિતિ o_slash:
	    સ્થિતિ 0x14d: સ્થિતિ 0x14f: સ્થિતિ 0x151: સ્થિતિ 0x1a1:
	    સ્થિતિ 0x1d2: સ્થિતિ 0x1eb: સ્થિતિ 0x1ed: સ્થિતિ 0x1ff:
	    સ્થિતિ 0x20d: સ્થિતિ 0x20f: સ્થિતિ 0x22b: સ્થિતિ 0x22d:
	    સ્થિતિ 0x22f: સ્થિતિ 0x231: સ્થિતિ 0x275: સ્થિતિ 0x1e4d:
	    સ્થિતિ 0x1e4f: સ્થિતિ 0x1e51: સ્થિતિ 0x1e53: સ્થિતિ 0x1ecd:
	    સ્થિતિ 0x1ecf: સ્થિતિ 0x1ed1: સ્થિતિ 0x1ed3: સ્થિતિ 0x1ed5:
	    સ્થિતિ 0x1ed7: સ્થિતિ 0x1ed9: સ્થિતિ 0x1edb: સ્થિતિ 0x1edd:
	    સ્થિતિ 0x1edf: સ્થિતિ 0x1ee1: સ્થિતિ 0x1ee3:
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
		    પાછા OK;

	    સ્થિતિ 'p': સ્થિતિ 0x1a5: સ્થિતિ 0x1d71: સ્થિતિ 0x1d7d: સ્થિતિ 0x1d88:
	    સ્થિતિ 0x1e55: સ્થિતિ 0x1e57:
		    EMIT2('p') EMIT2(0x1a5) EMIT2(0x1d71) EMIT2(0x1d7d)
		    EMIT2(0x1d88) EMIT2(0x1e55) EMIT2(0x1e57)
		    પાછા OK;

	    સ્થિતિ 'q': સ્થિતિ 0x24b: સ્થિતિ 0x2a0:
		    EMIT2('q') EMIT2(0x24b) EMIT2(0x2a0)
		    પાછા OK;

	    સ્થિતિ 'r': સ્થિતિ 0x155: સ્થિતિ 0x157: સ્થિતિ 0x159: સ્થિતિ 0x211:
	    સ્થિતિ 0x213: સ્થિતિ 0x24d: સ્થિતિ 0x27d: સ્થિતિ 0x1d72: સ્થિતિ 0x1d73:
	    સ્થિતિ 0x1d89: સ્થિતિ 0x1e59: સ્થિતિ 0x1e5b: સ્થિતિ 0x1e5d: સ્થિતિ 0x1e5f:
	    સ્થિતિ 0xa7a7:
		    EMIT2('r') EMIT2(0x155) EMIT2(0x157) EMIT2(0x159)
		    EMIT2(0x211) EMIT2(0x213) EMIT2(0x24d) EMIT2(0x27d)
		    EMIT2(0x1d72) EMIT2(0x1d73) EMIT2(0x1d89) EMIT2(0x1e59)
		    EMIT2(0x1e5b) EMIT2(0x1e5d) EMIT2(0x1e5f) EMIT2(0xa7a7)
		    પાછા OK;

	    સ્થિતિ 's': સ્થિતિ 0x15b: સ્થિતિ 0x15d: સ્થિતિ 0x15f: સ્થિતિ 0x161:
	    સ્થિતિ 0x219: સ્થિતિ 0x23f: સ્થિતિ 0x1d74: સ્થિતિ 0x1d8a: સ્થિતિ 0x1e61:
	    સ્થિતિ 0x1e63: સ્થિતિ 0x1e65: સ્થિતિ 0x1e67: સ્થિતિ 0x1e69: સ્થિતિ 0xa7a9:
		    EMIT2('s') EMIT2(0x15b) EMIT2(0x15d) EMIT2(0x15f)
		    EMIT2(0x161) EMIT2(0x219) EMIT2(0x23f) EMIT2(0x1d74)
		    EMIT2(0x1d8a) EMIT2(0x1e61) EMIT2(0x1e63) EMIT2(0x1e65)
		    EMIT2(0x1e67) EMIT2(0x1e69) EMIT2(0xa7a9)
		    પાછા OK;

	    સ્થિતિ 't': સ્થિતિ 0x163: સ્થિતિ 0x165: સ્થિતિ 0x167: સ્થિતિ 0x1ab:
	    સ્થિતિ 0x1ad: સ્થિતિ 0x21b: સ્થિતિ 0x288: સ્થિતિ 0x1d75: સ્થિતિ 0x1e6b:
	    સ્થિતિ 0x1e6d: સ્થિતિ 0x1e6f: સ્થિતિ 0x1e71: સ્થિતિ 0x1e97: સ્થિતિ 0x2c66:
		    EMIT2('t') EMIT2(0x163) EMIT2(0x165) EMIT2(0x167)
		    EMIT2(0x1ab) EMIT2(0x1ad) EMIT2(0x21b) EMIT2(0x288)
		    EMIT2(0x1d75) EMIT2(0x1e6b) EMIT2(0x1e6d) EMIT2(0x1e6f)
		    EMIT2(0x1e71) EMIT2(0x1e97) EMIT2(0x2c66)
		    પાછા OK;

	    સ્થિતિ 'u': સ્થિતિ u_grave: સ્થિતિ u_acute: સ્થિતિ u_circumflex:
	    સ્થિતિ u_diaeresis: સ્થિતિ 0x169: સ્થિતિ 0x16b: સ્થિતિ 0x16d:
	    સ્થિતિ 0x16f: સ્થિતિ 0x171: સ્થિતિ 0x173: સ્થિતિ 0x1b0: સ્થિતિ 0x1d4:
	    સ્થિતિ 0x1d6: સ્થિતિ 0x1d8: સ્થિતિ 0x1da: સ્થિતિ 0x1dc: સ્થિતિ 0x215:
	    સ્થિતિ 0x217: સ્થિતિ 0x289: સ્થિતિ 0x1d7e: સ્થિતિ 0x1d99: સ્થિતિ 0x1e73:
	    સ્થિતિ 0x1e75: સ્થિતિ 0x1e77: સ્થિતિ 0x1e79: સ્થિતિ 0x1e7b:
	    સ્થિતિ 0x1ee5: સ્થિતિ 0x1ee7: સ્થિતિ 0x1ee9: સ્થિતિ 0x1eeb:
	    સ્થિતિ 0x1eed: સ્થિતિ 0x1eef: સ્થિતિ 0x1ef1:
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
		    પાછા OK;

	    સ્થિતિ 'v': સ્થિતિ 0x28b: સ્થિતિ 0x1d8c: સ્થિતિ 0x1e7d: સ્થિતિ 0x1e7f:
		    EMIT2('v') EMIT2(0x28b) EMIT2(0x1d8c) EMIT2(0x1e7d)
		    EMIT2(0x1e7f)
		    પાછા OK;

	    સ્થિતિ 'w': સ્થિતિ 0x175: સ્થિતિ 0x1e81: સ્થિતિ 0x1e83: સ્થિતિ 0x1e85:
	    સ્થિતિ 0x1e87: સ્થિતિ 0x1e89: સ્થિતિ 0x1e98:
		    EMIT2('w') EMIT2(0x175) EMIT2(0x1e81) EMIT2(0x1e83)
		    EMIT2(0x1e85) EMIT2(0x1e87) EMIT2(0x1e89) EMIT2(0x1e98)
		    પાછા OK;

	    સ્થિતિ 'x': સ્થિતિ 0x1e8b: સ્થિતિ 0x1e8d:
		    EMIT2('x') EMIT2(0x1e8b) EMIT2(0x1e8d)
		    પાછા OK;

	    સ્થિતિ 'y': સ્થિતિ y_acute: સ્થિતિ y_diaeresis: સ્થિતિ 0x177:
	    સ્થિતિ 0x1b4: સ્થિતિ 0x233: સ્થિતિ 0x24f: સ્થિતિ 0x1e8f:
	    સ્થિતિ 0x1e99: સ્થિતિ 0x1ef3: સ્થિતિ 0x1ef5: સ્થિતિ 0x1ef7:
	    સ્થિતિ 0x1ef9:
		    EMIT2('y') EMIT2(y_acute) EMIT2(y_diaeresis)
		    EMIT2(0x177) EMIT2(0x1b4) EMIT2(0x233) EMIT2(0x24f)
		    EMIT2(0x1e8f) EMIT2(0x1e99) EMIT2(0x1ef3)
		    EMIT2(0x1ef5) EMIT2(0x1ef7) EMIT2(0x1ef9)
		    પાછા OK;

	    સ્થિતિ 'z': સ્થિતિ 0x17a: સ્થિતિ 0x17c: સ્થિતિ 0x17e: સ્થિતિ 0x1b6:
	    સ્થિતિ 0x1d76: સ્થિતિ 0x1d8e: સ્થિતિ 0x1e91: સ્થિતિ 0x1e93:
	    સ્થિતિ 0x1e95: સ્થિતિ 0x2c6c:
		    EMIT2('z') EMIT2(0x17a) EMIT2(0x17c) EMIT2(0x17e)
		    EMIT2(0x1b6) EMIT2(0x1d76) EMIT2(0x1d8e) EMIT2(0x1e91)
		    EMIT2(0x1e93) EMIT2(0x1e95) EMIT2(0x2c6c)
		    પાછા OK;

	    // default: character itself
	}
    }

    EMIT2(c);
    પાછા OK;
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
    સ્થિર પૂર્ણાંક
nfa_regatom(શૂન્ય)
{
    પૂર્ણાંક		c;
    પૂર્ણાંક		charclass;
    પૂર્ણાંક		equiclass;
    પૂર્ણાંક		collclass;
    પૂર્ણાંક		got_coll_char;
    char_u	*p;
    char_u	*endp;
    char_u	*old_regparse = regparse;
    પૂર્ણાંક		extra = 0;
    પૂર્ણાંક		emit_range;
    પૂર્ણાંક		negated;
    પૂર્ણાંક		result;
    પૂર્ણાંક		startc = -1;
    પૂર્ણાંક		save_prev_at_start = prev_at_start;

    c = getchr();
    પસંદગી (c)
    {
	સ્થિતિ NUL:
	    EMSG_RET_FAIL(_(e_nfa_regexp_end_encountered_prematurely));

	સ્થિતિ Magic('^'):
	    EMIT(NFA_BOL);
	    તોડો;

	સ્થિતિ Magic('$'):
	    EMIT(NFA_EOL);
#જો defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#endif
	    તોડો;

	સ્થિતિ Magic('<'):
	    EMIT(NFA_BOW);
	    તોડો;

	સ્થિતિ Magic('>'):
	    EMIT(NFA_EOW);
	    તોડો;

	સ્થિતિ Magic('_'):
	    c = no_Magic(getchr());
	    જો (c == NUL)
		EMSG_RET_FAIL(_(e_nfa_regexp_end_encountered_prematurely));

	    જો (c == '^')	// "\_^" is start-of-line
	    {
		EMIT(NFA_BOL);
		તોડો;
	    }
	    જો (c == '$')	// "\_$" is end-of-line
	    {
		EMIT(NFA_EOL);
#જો defined(FEAT_SYN_HL)
		had_eol = TRUE;
#endif
		તોડો;
	    }

	    extra = NFA_ADD_NL;

	    // "\_[" is collection plus newline
	    જો (c == '[')
		જાઓ collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	/*
	 * Character classes.
	 */
	સ્થિતિ Magic('.'):
	સ્થિતિ Magic('i'):
	સ્થિતિ Magic('I'):
	સ્થિતિ Magic('k'):
	સ્થિતિ Magic('K'):
	સ્થિતિ Magic('f'):
	સ્થિતિ Magic('F'):
	સ્થિતિ Magic('p'):
	સ્થિતિ Magic('P'):
	સ્થિતિ Magic('s'):
	સ્થિતિ Magic('S'):
	સ્થિતિ Magic('d'):
	સ્થિતિ Magic('D'):
	સ્થિતિ Magic('x'):
	સ્થિતિ Magic('X'):
	સ્થિતિ Magic('o'):
	સ્થિતિ Magic('O'):
	સ્થિતિ Magic('w'):
	સ્થિતિ Magic('W'):
	સ્થિતિ Magic('h'):
	સ્થિતિ Magic('H'):
	સ્થિતિ Magic('a'):
	સ્થિતિ Magic('A'):
	સ્થિતિ Magic('l'):
	સ્થિતિ Magic('L'):
	સ્થિતિ Magic('u'):
	સ્થિતિ Magic('U'):
	    p = vim_strchr(classchars, no_Magic(c));
	    જો (p == ખાલી)
	    {
		જો (extra == NFA_ADD_NL)
		{
		    semsg(_(e_nfa_regexp_invalid_character_class_nr), c);
		    rc_did_emsg = TRUE;
		    પાછા FAIL;
		}
		siemsg("Unknown character class char: %d", c);
		પાછા FAIL;
	    }

	    // When '.' is followed by a composing char ignore the dot, so that
	    // the composing char is matched here.
	    જો (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	    {
		old_regparse = regparse;
		c = getchr();
		જાઓ nfa_do_multibyte;
	    }
	    EMIT(nfa_classcodes[p - classchars]);
	    જો (extra == NFA_ADD_NL)
	    {
		EMIT(NFA_NEWL);
		EMIT(NFA_OR);
		regflags |= RF_HASNL;
	    }
	    તોડો;

	સ્થિતિ Magic('n'):
	    જો (reg_string)
		// In a string "\n" matches a newline character.
		EMIT(NL);
	    નહીંતર
	    {
		// In buffer text "\n" matches the end of a line.
		EMIT(NFA_NEWL);
		regflags |= RF_HASNL;
	    }
	    તોડો;

	સ્થિતિ Magic('('):
	    જો (nfa_reg(REG_PAREN) == FAIL)
		પાછા FAIL;	    // cascaded error
	    તોડો;

	સ્થિતિ Magic('|'):
	સ્થિતિ Magic('&'):
	સ્થિતિ Magic(')'):
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    પાછા FAIL;

	સ્થિતિ Magic('='):
	સ્થિતિ Magic('?'):
	સ્થિતિ Magic('+'):
	સ્થિતિ Magic('@'):
	સ્થિતિ Magic('*'):
	સ્થિતિ Magic('{'):
	    // these should follow an atom, not form an atom
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    પાછા FAIL;

	સ્થિતિ Magic('~'):
	    {
		char_u	    *lp;

		// Previous substitute pattern.
		// Generated as "\%(pattern\)".
		જો (reg_prev_sub == ખાલી)
		{
		    emsg(_(e_no_previous_substitute_regular_expression));
		    પાછા FAIL;
		}
		માટે (lp = reg_prev_sub; *lp != NUL; MB_CPTR_ADV(lp))
		{
		    EMIT(PTR2CHAR(lp));
		    જો (lp != reg_prev_sub)
			EMIT(NFA_CONCAT);
		}
		EMIT(NFA_NOPEN);
		તોડો;
	    }

	સ્થિતિ Magic('1'):
	સ્થિતિ Magic('2'):
	સ્થિતિ Magic('3'):
	સ્થિતિ Magic('4'):
	સ્થિતિ Magic('5'):
	સ્થિતિ Magic('6'):
	સ્થિતિ Magic('7'):
	સ્થિતિ Magic('8'):
	સ્થિતિ Magic('9'):
	    {
		પૂર્ણાંક refnum = no_Magic(c) - '1';

		જો (!seen_endbrace(refnum + 1))
		    પાછા FAIL;
		EMIT(NFA_BACKREF1 + refnum);
		rex.nfa_has_backref = TRUE;
	    }
	    તોડો;

	સ્થિતિ Magic('z'):
	    c = no_Magic(getchr());
	    પસંદગી (c)
	    {
		સ્થિતિ 's':
		    EMIT(NFA_ZSTART);
		    જો (re_mult_next("\\zs") == FAIL)
			પાછા FAIL;
		    તોડો;
		સ્થિતિ 'e':
		    EMIT(NFA_ZEND);
		    rex.nfa_has_zend = TRUE;
		    જો (re_mult_next("\\ze") == FAIL)
			પાછા FAIL;
		    તોડો;
#ifdef FEAT_SYN_HL
		સ્થિતિ '1':
		સ્થિતિ '2':
		સ્થિતિ '3':
		સ્થિતિ '4':
		સ્થિતિ '5':
		સ્થિતિ '6':
		સ્થિતિ '7':
		સ્થિતિ '8':
		સ્થિતિ '9':
		    // \z1...\z9
		    જો ((reg_do_extmatch & REX_USE) == 0)
			EMSG_RET_FAIL(_(e_z1_z9_not_allowed_here));
		    EMIT(NFA_ZREF1 + (no_Magic(c) - '1'));
		    // No need to set rex.nfa_has_backref, the sub-matches don't
		    // change when \z1 .. \z9 matches or not.
		    re_has_z = REX_USE;
		    તોડો;
		સ્થિતિ '(':
		    // \z(
		    જો ((reg_do_extmatch & REX_SET) == 0)
			EMSG_RET_FAIL(_(e_z_not_allowed_here));
		    જો (nfa_reg(REG_ZPAREN) == FAIL)
			પાછા FAIL;	    // cascaded error
		    re_has_z = REX_SET;
		    તોડો;
#endif
		મૂળભૂત:
		    semsg(_(e_nfa_regexp_unknown_operator_z_chr), no_Magic(c));
		    પાછા FAIL;
	    }
	    તોડો;

	સ્થિતિ Magic('%'):
	    c = no_Magic(getchr());
	    પસંદગી (c)
	    {
		// () without a back reference
		સ્થિતિ '(':
		    જો (nfa_reg(REG_NPAREN) == FAIL)
			પાછા FAIL;
		    EMIT(NFA_NOPEN);
		    તોડો;

		સ્થિતિ 'd':   // %d123 decimal
		સ્થિતિ 'o':   // %o123 octal
		સ્થિતિ 'x':   // %xab hex 2
		સ્થિતિ 'u':   // %uabcd hex 4
		સ્થિતિ 'U':   // %U1234abcd hex 8
		    {
			vimlong_T nr;

			પસંદગી (c)
			{
			    સ્થિતિ 'd': nr = getdecchrs(); તોડો;
			    સ્થિતિ 'o': nr = getoctchrs(); તોડો;
			    સ્થિતિ 'x': nr = gethexchrs(2); તોડો;
			    સ્થિતિ 'u': nr = gethexchrs(4); તોડો;
			    સ્થિતિ 'U': nr = gethexchrs(8); તોડો;
			    મૂળભૂત:  nr = -1; તોડો;
			}

			જો (nr < 0 || nr > INT_MAX)
			    EMSG2_RET_FAIL(_(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			// A NUL is stored in the text as NL
			// TODO: what if a composing character follows?
			EMIT(nr == 0 ? 0x0a : (long)nr);
		    }
		    તોડો;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		સ્થિતિ '^':
		    EMIT(NFA_BOF);
		    તોડો;

		સ્થિતિ '$':
		    EMIT(NFA_EOF);
		    તોડો;

		સ્થિતિ '#':
		    જો (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_start_of_pattern),
								  regparse[1]);
			પાછા FAIL;
		    }
		    EMIT(NFA_CURSOR);
		    તોડો;

		સ્થિતિ 'V':
		    EMIT(NFA_VISUAL);
		    તોડો;

		સ્થિતિ 'C':
		    EMIT(NFA_ANY_COMPOSING);
		    તોડો;

		સ્થિતિ '[':
		    {
			પૂર્ણાંક	    n;

			// \%[abc]
			માટે (n = 0; (c = peekchr()) != ']'; ++n)
			{
			    જો (c == NUL)
				EMSG2_RET_FAIL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
			    // recursive call!
			    જો (nfa_regatom() == FAIL)
				પાછા FAIL;
			}
			getchr();  // get the ]
			જો (n == 0)
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
			તોડો;
		    }

		મૂળભૂત:
		    {
			long_u	n = 0;
			પૂર્ણાંક	cmp = c;
			પૂર્ણાંક	cur = FALSE;
			પૂર્ણાંક	got_digit = FALSE;

			જો (c == '<' || c == '>')
			    c = getchr();
			જો (no_Magic(c) == '.')
			{
			    cur = TRUE;
			    c = getchr();
			}
			જ્યારે (VIM_ISDIGIT(c))
			{
			    long_u tmp;

			    જો (cur)
			    {
				semsg(_(e_regexp_સંખ્યા_after_dot_pos_શોધ_chr),
								  no_Magic(c));
				પાછા FAIL;
			    }
			    tmp = n * 10 + (c - '0');

			    જો (tmp < n)
			    {
				// overflow.
				emsg(_(e_percent_કિંમત_too_large));
				પાછા FAIL;
			    }
			    n = tmp;
			    c = getchr();
			    got_digit = TRUE;
			}
			જો (c == 'l' || c == 'c' || c == 'v')
			{
			    long_u limit = INT_MAX;

			    જો (!cur && !got_digit)
			    {
				semsg(_(e_nfa_regexp_missing_કિંમત_in_chr),
								  no_Magic(c));
				પાછા FAIL;
			    }
			    જો (c == 'l')
			    {
				જો (cur)
				    n = curwin->w_cursor.lnum;
				// \%{n}l  \%{n}<l  \%{n}>l
				EMIT(cmp == '<' ? NFA_LNUM_LT :
				     cmp == '>' ? NFA_LNUM_GT : NFA_LNUM);
				જો (save_prev_at_start)
				    at_start = TRUE;
			    }
			    નહીંતર જો (c == 'c')
			    {
				જો (cur)
				{
				    n = curwin->w_cursor.col;
				    n++;
				}
				// \%{n}c  \%{n}<c  \%{n}>c
				EMIT(cmp == '<' ? NFA_COL_LT :
				     cmp == '>' ? NFA_COL_GT : NFA_COL);
			    }
			    નહીંતર
			    {
				જો (cur)
				{
				    colnr_T vcol = 0;

				    getvvcol(curwin, &curwin->w_cursor,
							 ખાલી, ખાલી, &vcol, 0);
				    n = ++vcol;
				}
				// \%{n}v  \%{n}<v  \%{n}>v
				EMIT(cmp == '<' ? NFA_VCOL_LT :
				     cmp == '>' ? NFA_VCOL_GT : NFA_VCOL);
				limit = INT_MAX / MB_MAXBYTES;
			    }
			    જો (n >= limit)
			    {
				emsg(_(e_percent_કિંમત_too_large));
				પાછા FAIL;
			    }
			    EMIT((પૂર્ણાંક)n);
			    તોડો;
			}
			નહીંતર જો (no_Magic(c) == '\'' && n == 0)
			{
			    // \%'m  \%<'m  \%>'m
			    EMIT(cmp == '<' ? NFA_MARK_LT :
				 cmp == '>' ? NFA_MARK_GT : NFA_MARK);
			    EMIT(getchr());
			    તોડો;
			}
		    }
		    semsg(_(e_nfa_regexp_unknown_operator_percent_chr),
								  no_Magic(c));
		    પાછા FAIL;
	    }
	    તોડો;

	સ્થિતિ Magic('['):
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
	    જો (*endp == ']')
	    {
		પૂર્ણાંક plen;
		bool range_endpoint;
		/*
		 * Try to reverse engineer character classes. For example,
		 * recognize that [0-9] stands for \d and [A-Za-z_] for \h,
		 * and perform the necessary substitutions in the NFA.
		 */
		result = nfa_recognize_char_class(regparse, endp,
							 extra == NFA_ADD_NL);
		જો (result != FAIL)
		{
		    જો (result >= NFA_FIRST_NL && result <= NFA_LAST_NL)
		    {
			EMIT(result - NFA_ADD_NL);
			EMIT(NFA_NEWL);
			EMIT(NFA_OR);
		    }
		    નહીંતર
			EMIT(result);
		    regparse = endp;
		    MB_PTR_ADV(regparse);
		    પાછા OK;
		}
		/*
		 * Failed to recognize a character class. Use the simple
		 * version that turns [abc] into 'a' OR 'b' OR 'c'
		 */
		startc = -1;
		negated = FALSE;
		જો (*regparse == '^')			// negated range
		{
		    negated = TRUE;
		    MB_PTR_ADV(regparse);
		    EMIT(NFA_START_NEG_COLL);
		}
		નહીંતર
		    EMIT(NFA_START_COLL);
		જો (*regparse == '-')
		{
		    startc = '-';
		    EMIT(startc);
		    EMIT(NFA_CONCAT);
		    MB_PTR_ADV(regparse);
		}
		// Emit the OR branches for each character in the []
		emit_range = FALSE;
		જ્યારે (regparse < endp)
		{
		    પૂર્ણાંક	    oldstartc = startc;
		    range_endpoint = false;

		    startc = -1;
		    got_coll_char = FALSE;
		    જો (*regparse == '[')
		    {
			// Check for [: :], [= =], [. .]
			equiclass = collclass = 0;
			charclass = get_char_class(&regparse);
			જો (charclass == CLASS_NONE)
			{
			    equiclass = get_equi_class(&regparse);
			    જો (equiclass == 0)
				collclass = get_coll_element(&regparse);
			}

			// Character class like [:alpha:]
			જો (charclass != CLASS_NONE)
			{
			    પસંદગી (charclass)
			    {
				સ્થિતિ CLASS_ALNUM:
				    EMIT(NFA_CLASS_ALNUM);
				    તોડો;
				સ્થિતિ CLASS_ALPHA:
				    EMIT(NFA_CLASS_ALPHA);
				    તોડો;
				સ્થિતિ CLASS_BLANK:
				    EMIT(NFA_CLASS_BLANK);
				    તોડો;
				સ્થિતિ CLASS_CNTRL:
				    EMIT(NFA_CLASS_CNTRL);
				    તોડો;
				સ્થિતિ CLASS_DIGIT:
				    EMIT(NFA_CLASS_DIGIT);
				    તોડો;
				સ્થિતિ CLASS_GRAPH:
				    EMIT(NFA_CLASS_GRAPH);
				    તોડો;
				સ્થિતિ CLASS_LOWER:
				    wants_nfa = TRUE;
				    EMIT(NFA_CLASS_LOWER);
				    તોડો;
				સ્થિતિ CLASS_PRINT:
				    EMIT(NFA_CLASS_PRINT);
				    તોડો;
				સ્થિતિ CLASS_PUNCT:
				    EMIT(NFA_CLASS_PUNCT);
				    તોડો;
				સ્થિતિ CLASS_SPACE:
				    EMIT(NFA_CLASS_SPACE);
				    તોડો;
				સ્થિતિ CLASS_UPPER:
				    wants_nfa = TRUE;
				    EMIT(NFA_CLASS_UPPER);
				    તોડો;
				સ્થિતિ CLASS_XDIGIT:
				    EMIT(NFA_CLASS_XDIGIT);
				    તોડો;
				સ્થિતિ CLASS_TAB:
				    EMIT(NFA_CLASS_TAB);
				    તોડો;
				સ્થિતિ CLASS_RETURN:
				    EMIT(NFA_CLASS_RETURN);
				    તોડો;
				સ્થિતિ CLASS_BACKSPACE:
				    EMIT(NFA_CLASS_BACKSPACE);
				    તોડો;
				સ્થિતિ CLASS_ESCAPE:
				    EMIT(NFA_CLASS_ESCAPE);
				    તોડો;
				સ્થિતિ CLASS_IDENT:
				    EMIT(NFA_CLASS_IDENT);
				    તોડો;
				સ્થિતિ CLASS_KEYWORD:
				    EMIT(NFA_CLASS_KEYWORD);
				    તોડો;
				સ્થિતિ CLASS_FNAME:
				    EMIT(NFA_CLASS_FNAME);
				    તોડો;
			    }
			    EMIT(NFA_CONCAT);
			    ચાલુ;
			}
			// Try equivalence class [=a=] and the like
			જો (equiclass != 0)
			{
			    result = nfa_emit_equi_class(equiclass);
			    જો (result == FAIL)
			    {
				// should never happen
				EMSG_RET_FAIL(_(e_error_building_nfa_with_equivalence_class));
			    }
			    ચાલુ;
			}
			// Try collating class like [. .]
			જો (collclass != 0)
			{
			    startc = collclass;	 // allow [.a.]-x as a range
			    // Will emit the proper atom at the end of the
			    // while loop.
			}
		    }
		    // Try a range like 'a-x' or '\t-z'. Also allows '-' as a
		    // start character.
		    જો (*regparse == '-' && oldstartc != -1)
		    {
			emit_range = TRUE;
			startc = oldstartc;
			MB_PTR_ADV(regparse);
			ચાલુ;	    // reading the end of the range
		    }

		    // Now handle simple and escaped characters.
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    જો (*regparse == '\\'
			    && !reg_cpo_bsl
			    && regparse + 1 <= endp
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != ખાલી
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR, regparse[1])
								      != ખાલી)
			    )
			)
		    {
			MB_PTR_ADV(regparse);

			જો (*regparse == 'n')
			    startc = (reg_string || emit_range
					|| regparse[1] == '-') ? NL : NFA_NEWL;
			નહીંતર જો (*regparse == 'd'
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
				જો (startc == INT_MAX)
				    EMSG_RET_FAIL(_(e_unicode_val_too_large));
				got_coll_char = TRUE;
				MB_PTR_BACK(old_regparse, regparse);
			    }
			    નહીંતર
			    {
				// \r,\t,\e,\b
				startc = backslash_trans(*regparse);
			    }
		    }

		    // Normal printable char
		    જો (startc == -1)
			startc = PTR2CHAR(regparse);

		    // Previous char was '-', so this char is end of range.
		    જો (emit_range)
		    {
			પૂર્ણાંક	endc = startc;
			range_endpoint = true;

			startc = oldstartc;
			જો (startc > endc)
			    EMSG_RET_FAIL(_(e_reverse_range_in_character_class));

			જો (endc > startc + 2)
			{
			    // Emit a range instead of the sequence of
			    // individual characters.
			    જો (startc == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    નહીંતર
				--post_ptr; // remove NFA_CONCAT
			    EMIT(endc);
			    EMIT(NFA_RANGE);
			    EMIT(NFA_CONCAT);
			}
			નહીંતર જો (has_mbyte && ((*mb_char2len)(startc) > 1
				    || (*mb_char2len)(endc) > 1))
			{
			    // Emit the characters in the range.
			    // "startc" was already emitted, so skip it.
			    //
			    માટે (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			નહીંતર
			{
			    // Emit the range. "startc" was already emitted, so
			    // skip it.
			    માટે (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			emit_range = FALSE;
			startc = -1;
		    }
		    નહીંતર
		    {
			// This char (startc) is not part of a range. Just
			// emit it.
			// Normally, simply emit startc. But if we get char
			// code=0 from a collating char, then replace it with
			// 0x0a.
			// This is needed to completely mimic the behaviour of
			// the backtracking engine.
			જો (startc == NFA_NEWL)
			{
			    // Line break can't be matched as part of the
			    // collection, add an OR below. But not for negated
			    // range.
			    જો (!negated)
				extra = NFA_ADD_NL;
			}
			નહીંતર
			{
			    જો (got_coll_char == TRUE && startc == 0)
			    {
				EMIT(0x0a);
				EMIT(NFA_CONCAT);
			    }
			    નહીંતર
			    {
				EMIT(startc);
				જો (!(enc_utf8 && (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse)))))
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
		    જો (!range_endpoint && enc_utf8 &&
			    (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse))))
		    {
			પૂર્ણાંક i = utf_ptr2len(regparse);

			c = utf_ptr2char(regparse + i);

			// Add composing characters
			માટે (;;)
			{
			    જો (c == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    નહીંતર
				EMIT(c);
			    EMIT(NFA_CONCAT);
			    જો ((i += utf_char2len(c)) >= plen)
				તોડો;
			    c = utf_ptr2char(regparse + i);
			}
			EMIT(NFA_COMPOSING);
			EMIT(NFA_CONCAT);
		    }
		    MB_PTR_ADV(regparse);
		} // while (p < endp)

		MB_PTR_BACK(old_regparse, regparse);
		જો (*regparse == '-')	    // if last, '-' is just a char
		{
		    EMIT('-');
		    EMIT(NFA_CONCAT);
		}

		// skip the trailing ]
		regparse = endp;
		MB_PTR_ADV(regparse);

		// Mark end of the collection.
		જો (negated == TRUE)
		    EMIT(NFA_END_NEG_COLL);
		નહીંતર
		    EMIT(NFA_END_COLL);

		// \_[] also matches \n but it's not negated
		જો (extra == NFA_ADD_NL)
		{
		    EMIT(reg_string ? NL : NFA_NEWL);
		    EMIT(NFA_OR);
		}

		પાછા OK;
	    } // if exists closing ]

	    જો (reg_strict)
		EMSG_RET_FAIL(_(e_missing_rsb_after_str_lsb));
	    // FALLTHROUGH

	મૂળભૂત:
	    {
		પૂર્ણાંક	plen;

nfa_do_multibyte:
		// plen is length of current char with composing chars
		જો (enc_utf8 && ((*mb_char2len)(c)
			    != (plen = utfc_ptr2len(old_regparse))
						       || utf_iscomposing(c)))
		{
		    પૂર્ણાંક i = 0;

		    // A base character plus composing characters, or just one
		    // or more composing characters.
		    // This requires creating a separate atom as if enclosing
		    // the characters in (), where NFA_COMPOSING is the ( and
		    // NFA_END_COMPOSING is the ). Note that right now we are
		    // building the postfix form, not the NFA itself;
		    // a composing char could be: a, b, c, NFA_COMPOSING
		    // where 'b' and 'c' are chars with codes > 256.
		    માટે (;;)
		    {
			EMIT(c);
			જો (i > 0)
			    EMIT(NFA_CONCAT);
			જો ((i += utf_char2len(c)) >= plen)
			    તોડો;
			c = utf_ptr2char(old_regparse + i);
		    }
		    EMIT(NFA_COMPOSING);
		    regparse = old_regparse + plen;
		}
		નહીંતર
		{
		    c = no_Magic(c);
		    EMIT(c);
		}
		પાછા OK;
	    }
    }

    પાછા OK;
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
    સ્થિર પૂર્ણાંક
nfa_regpiece(શૂન્ય)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		op;
    પૂર્ણાંક		ret;
    long	minval, maxval;
    પૂર્ણાંક		greedy = TRUE;      // Braces are prefixed with '-' ?
    parse_state_T old_state;
    parse_state_T new_state;
    long	c2;
    પૂર્ણાંક		old_post_pos;
    પૂર્ણાંક		my_post_start;
    પૂર્ણાંક		quest;

    // Save the current parse state, so that we can use it if <atom>{m,n} is
    // next.
    save_parse_state(&old_state);

    // store current pos in the postfix form, for \{m,n} involving 0s
    my_post_start = (પૂર્ણાંક)(post_ptr - post_start);

    ret = nfa_regatom();
    જો (ret == FAIL)
	પાછા FAIL;	    // cascaded error

    op = peekchr();
    જો (re_multi_type(op) == NOT_MULTI)
	પાછા OK;

    skipchr();
    પસંદગી (op)
    {
	સ્થિતિ Magic('*'):
	    EMIT(NFA_STAR);
	    તોડો;

	સ્થિતિ Magic('+'):
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
	    જો (nfa_regatom() == FAIL)
		પાછા FAIL;
	    EMIT(NFA_STAR);
	    EMIT(NFA_CONCAT);
	    skipchr();		// skip the \+
	    તોડો;

	સ્થિતિ Magic('@'):
	    c2 = (long)getdecchrs();
	    op = no_Magic(getchr());
	    i = 0;
	    પસંદગી(op)
	    {
		સ્થિતિ '=':
		    // \@=
		    i = NFA_PREV_ATOM_NO_WIDTH;
		    તોડો;
		સ્થિતિ '!':
		    // \@!
		    i = NFA_PREV_ATOM_NO_WIDTH_NEG;
		    તોડો;
		સ્થિતિ '<':
		    op = no_Magic(getchr());
		    જો (op == '=')
			// \@<=
			i = NFA_PREV_ATOM_JUST_BEFORE;
		    નહીંતર જો (op == '!')
			// \@<!
			i = NFA_PREV_ATOM_JUST_BEFORE_NEG;
		    તોડો;
		સ્થિતિ '>':
		    // \@>
		    i = NFA_PREV_ATOM_LIKE_PATTERN;
		    તોડો;
	    }
	    જો (i == 0)
	    {
		semsg(_(e_nfa_regexp_unknown_operator_at_chr), op);
		પાછા FAIL;
	    }
	    EMIT(i);
	    જો (i == NFA_PREV_ATOM_JUST_BEFORE
					|| i == NFA_PREV_ATOM_JUST_BEFORE_NEG)
		EMIT(c2);
	    તોડો;

	સ્થિતિ Magic('?'):
	સ્થિતિ Magic('='):
	    EMIT(NFA_QUEST);
	    તોડો;

	સ્થિતિ Magic('{'):
	    // a{2,5} will expand to 'aaa?a?a?'
	    // a{-1,3} will expand to 'aa??a??', where ?? is the nongreedy
	    // version of '?'
	    // \v(ab){2,3} will expand to '(ab)(ab)(ab)?', where all the
	    // parenthesis have the same id

	    greedy = TRUE;
	    c2 = peekchr();
	    જો (c2 == '-' || c2 == Magic('-'))
	    {
		skipchr();
		greedy = FALSE;
	    }
	    જો (!read_limits(&minval, &maxval))
		EMSG_RET_FAIL(_(e_nfa_regexp_error_reading_repetition_limits));

	    //  <atom>{0,inf}, <atom>{0,} and <atom>{}  are equivalent to
	    //  <atom>*
	    જો (minval == 0 && maxval == MAX_LIMIT)
	    {
		જો (greedy)		// { { (match the braces)
		    // \{}, \{0,}
		    EMIT(NFA_STAR);
		નહીંતર			// { { (match the braces)
		    // \{-}, \{-0,}
		    EMIT(NFA_STAR_NONGREEDY);
		તોડો;
	    }

	    // Special case: x{0} or x{-0}
	    જો (maxval == 0)
	    {
		// Ignore result of previous call to nfa_regatom()
		post_ptr = post_start + my_post_start;
		// NFA_EMPTY is 0-length and works everywhere
		EMIT(NFA_EMPTY);
		પાછા OK;
	    }

	    // The engine is very inefficient (uses too many states) when the
	    // maximum is much larger than the minimum and when the maximum is
	    // large.  However, when maxval is MAX_LIMIT, it is okay, as this
	    // will emit NFA_STAR.
	    // Bail out if we can use the other engine, but only, when the
	    // pattern does not need the NFA engine like (e.g. [[:upper:]]\{2,\}
	    // does not work with characters > 8 bit with the BT engine)
	    જો ((nfa_re_flags & RE_AUTO)
				   && (maxval > 500 || maxval > minval + 200)
				   && (maxval != MAX_LIMIT && minval < 200)
				   && !wants_nfa)
		પાછા FAIL;

	    // Ignore previous call to nfa_regatom()
	    post_ptr = post_start + my_post_start;
	    // Save parse state after the repeated atom and the \{}
	    save_parse_state(&new_state);

	    quest = (greedy == TRUE? NFA_QUEST : NFA_QUEST_NONGREEDY);
	    માટે (i = 0; i < maxval; i++)
	    {
		// Goto beginning of the repeated atom
		restore_parse_state(&old_state);
		old_post_pos = (પૂર્ણાંક)(post_ptr - post_start);
		જો (nfa_regatom() == FAIL)
		    પાછા FAIL;
		// after "minval" times, atoms are optional
		જો (i + 1 > minval)
		{
		    જો (maxval == MAX_LIMIT)
		    {
			જો (greedy)
			    EMIT(NFA_STAR);
			નહીંતર
			    EMIT(NFA_STAR_NONGREEDY);
		    }
		    નહીંતર
			EMIT(quest);
		}
		જો (old_post_pos != my_post_start)
		    EMIT(NFA_CONCAT);
		જો (i + 1 > minval && maxval == MAX_LIMIT)
		    તોડો;
	    }

	    // Go to just after the repeated atom and the \{}
	    restore_parse_state(&new_state);
	    curchr = -1;

	    તોડો;


	મૂળભૂત:
	    તોડો;
    }	// end switch

    જો (re_multi_type(peekchr()) != NOT_MULTI)
	// Can't have a multi follow a multi.
	EMSG_RET_FAIL(_(e_nfa_regexp_cant_have_multi_follow_multi));

    પાછા OK;
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
    સ્થિર પૂર્ણાંક
nfa_regconcat(શૂન્ય)
{
    પૂર્ણાંક		cont = TRUE;
    પૂર્ણાંક		first = TRUE;

    જ્યારે (cont)
    {
	પસંદગી (peekchr())
	{
	    સ્થિતિ NUL:
	    સ્થિતિ Magic('|'):
	    સ્થિતિ Magic('&'):
	    સ્થિતિ Magic(')'):
		cont = FALSE;
		તોડો;

	    સ્થિતિ Magic('Z'):
		regflags |= RF_ICOMBINE;
		skipchr_keepstart();
		તોડો;
	    સ્થિતિ Magic('c'):
		regflags |= RF_ICASE;
		skipchr_keepstart();
		તોડો;
	    સ્થિતિ Magic('C'):
		regflags |= RF_NOICASE;
		skipchr_keepstart();
		તોડો;
	    સ્થિતિ Magic('v'):
		reg_magic = MAGIC_ALL;
		skipchr_keepstart();
		curchr = -1;
		તોડો;
	    સ્થિતિ Magic('m'):
		reg_magic = MAGIC_ON;
		skipchr_keepstart();
		curchr = -1;
		તોડો;
	    સ્થિતિ Magic('M'):
		reg_magic = MAGIC_OFF;
		skipchr_keepstart();
		curchr = -1;
		તોડો;
	    સ્થિતિ Magic('V'):
		reg_magic = MAGIC_NONE;
		skipchr_keepstart();
		curchr = -1;
		તોડો;

	    મૂળભૂત:
		જો (nfa_regpiece() == FAIL)
		    પાછા FAIL;
		જો (first == FALSE)
		    EMIT(NFA_CONCAT);
		નહીંતર
		    first = FALSE;
		તોડો;
	}
    }

    પાછા OK;
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
    સ્થિર પૂર્ણાંક
nfa_regbranch(શૂન્ય)
{
    પૂર્ણાંક		old_post_pos;

    old_post_pos = (પૂર્ણાંક)(post_ptr - post_start);

    // First branch, possibly the only one
    જો (nfa_regconcat() == FAIL)
	પાછા FAIL;

    // Try next concats
    જ્યારે (peekchr() == Magic('&'))
    {
	skipchr();
	// if concat is empty do emit a node
	જો (old_post_pos == (પૂર્ણાંક)(post_ptr - post_start))
	    EMIT(NFA_EMPTY);
	EMIT(NFA_NOPEN);
	EMIT(NFA_PREV_ATOM_NO_WIDTH);
	old_post_pos = (પૂર્ણાંક)(post_ptr - post_start);
	જો (nfa_regconcat() == FAIL)
	    પાછા FAIL;
	// if concat is empty do emit a node
	જો (old_post_pos == (પૂર્ણાંક)(post_ptr - post_start))
	    EMIT(NFA_EMPTY);
	EMIT(NFA_CONCAT);
    }

    // if a branch is empty, emit one node for it
    જો (old_post_pos == (પૂર્ણાંક)(post_ptr - post_start))
	EMIT(NFA_EMPTY);

    પાછા OK;
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
    સ્થિર પૂર્ણાંક
nfa_reg(
    પૂર્ણાંક		paren)	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
{
    પૂર્ણાંક		parno = 0;
    પૂર્ણાંક		status = FAIL;

    જો (paren == REG_PAREN)
    {
	જો (regnpar >= NSUBEXP) // Too many `('
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_parens));
	parno = regnpar++;
    }
#ifdef FEAT_SYN_HL
    નહીંતર જો (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	જો (regnzpar >= NSUBEXP)
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_z));
	parno = regnzpar++;
    }
#endif

    જો (nfa_reg_parse_depth >= REG_MAX_PAREN_DEPTH)
	EMSG_RET_FAIL(_(e_command_too_complex));
    ++nfa_reg_parse_depth;

    જો (nfa_regbranch() == FAIL)
	જાઓ theend;	    // cascaded error

    જ્યારે (peekchr() == Magic('|'))
    {
	skipchr();
	જો (nfa_regbranch() == FAIL)
	    જાઓ theend;    // cascaded error
	EMIT(NFA_OR);
    }

    // Check for proper termination.
    જો (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
	જો (paren == REG_NPAREN)
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	નહીંતર
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	rc_did_emsg = TRUE;
	જાઓ theend;
    }
    નહીંતર જો (paren == REG_NOPAREN && peekchr() != NUL)
    {
	જો (peekchr() == Magic(')'))
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	નહીંતર
	    emsg(_(e_nfa_regexp_proper_termination_error));
	rc_did_emsg = TRUE;
	જાઓ theend;
    }
    /*
     * Here we set the flag allowing back references to this set of
     * parentheses.
     */
    જો (paren == REG_PAREN)
    {
	had_endbrace[parno] = TRUE;     // have seen the close paren
	EMIT(NFA_MOPEN + parno);
    }
#ifdef FEAT_SYN_HL
    નહીંતર જો (paren == REG_ZPAREN)
	EMIT(NFA_ZOPEN + parno);
#endif

    status = OK;

theend:
    --nfa_reg_parse_depth;
    પાછા status;
}

#ifdef DEBUG
સ્થિર char_u code[50];

    સ્થિર શૂન્ય
nfa_set_code(પૂર્ણાંક c)
{
    પૂર્ણાંક	    addnl = FALSE;

    જો (c >= NFA_FIRST_NL && c <= NFA_LAST_NL)
    {
	addnl = TRUE;
	c -= NFA_ADD_NL;
    }

    STRCPY(code, "");
    પસંદગી (c)
    {
	સ્થિતિ NFA_MATCH:	    STRCPY(code, "NFA_MATCH "); તોડો;
	સ્થિતિ NFA_SPLIT:	    STRCPY(code, "NFA_SPLIT "); તોડો;
	સ્થિતિ NFA_CONCAT:    STRCPY(code, "NFA_CONCAT "); તોડો;
	સ્થિતિ NFA_NEWL:	    STRCPY(code, "NFA_NEWL "); તોડો;
	સ્થિતિ NFA_ZSTART:    STRCPY(code, "NFA_ZSTART"); તોડો;
	સ્થિતિ NFA_ZEND:	    STRCPY(code, "NFA_ZEND"); તોડો;

	સ્થિતિ NFA_BACKREF1:  STRCPY(code, "NFA_BACKREF1"); તોડો;
	સ્થિતિ NFA_BACKREF2:  STRCPY(code, "NFA_BACKREF2"); તોડો;
	સ્થિતિ NFA_BACKREF3:  STRCPY(code, "NFA_BACKREF3"); તોડો;
	સ્થિતિ NFA_BACKREF4:  STRCPY(code, "NFA_BACKREF4"); તોડો;
	સ્થિતિ NFA_BACKREF5:  STRCPY(code, "NFA_BACKREF5"); તોડો;
	સ્થિતિ NFA_BACKREF6:  STRCPY(code, "NFA_BACKREF6"); તોડો;
	સ્થિતિ NFA_BACKREF7:  STRCPY(code, "NFA_BACKREF7"); તોડો;
	સ્થિતિ NFA_BACKREF8:  STRCPY(code, "NFA_BACKREF8"); તોડો;
	સ્થિતિ NFA_BACKREF9:  STRCPY(code, "NFA_BACKREF9"); તોડો;
# ifdef FEAT_SYN_HL
	સ્થિતિ NFA_ZREF1:	    STRCPY(code, "NFA_ZREF1"); તોડો;
	સ્થિતિ NFA_ZREF2:	    STRCPY(code, "NFA_ZREF2"); તોડો;
	સ્થિતિ NFA_ZREF3:	    STRCPY(code, "NFA_ZREF3"); તોડો;
	સ્થિતિ NFA_ZREF4:	    STRCPY(code, "NFA_ZREF4"); તોડો;
	સ્થિતિ NFA_ZREF5:	    STRCPY(code, "NFA_ZREF5"); તોડો;
	સ્થિતિ NFA_ZREF6:	    STRCPY(code, "NFA_ZREF6"); તોડો;
	સ્થિતિ NFA_ZREF7:	    STRCPY(code, "NFA_ZREF7"); તોડો;
	સ્થિતિ NFA_ZREF8:	    STRCPY(code, "NFA_ZREF8"); તોડો;
	સ્થિતિ NFA_ZREF9:	    STRCPY(code, "NFA_ZREF9"); તોડો;
# endif
	સ્થિતિ NFA_SKIP:	    STRCPY(code, "NFA_SKIP"); તોડો;

	સ્થિતિ NFA_PREV_ATOM_NO_WIDTH:
			    STRCPY(code, "NFA_PREV_ATOM_NO_WIDTH"); તોડો;
	સ્થિતિ NFA_PREV_ATOM_NO_WIDTH_NEG:
			    STRCPY(code, "NFA_PREV_ATOM_NO_WIDTH_NEG"); તોડો;
	સ્થિતિ NFA_PREV_ATOM_JUST_BEFORE:
			    STRCPY(code, "NFA_PREV_ATOM_JUST_BEFORE"); તોડો;
	સ્થિતિ NFA_PREV_ATOM_JUST_BEFORE_NEG:
			 STRCPY(code, "NFA_PREV_ATOM_JUST_BEFORE_NEG"); તોડો;
	સ્થિતિ NFA_PREV_ATOM_LIKE_PATTERN:
			    STRCPY(code, "NFA_PREV_ATOM_LIKE_PATTERN"); તોડો;

	સ્થિતિ NFA_NOPEN:		    STRCPY(code, "NFA_NOPEN"); તોડો;
	સ્થિતિ NFA_NCLOSE:	    STRCPY(code, "NFA_NCLOSE"); તોડો;
	સ્થિતિ NFA_START_INVISIBLE:   STRCPY(code, "NFA_START_INVISIBLE"); તોડો;
	સ્થિતિ NFA_START_INVISIBLE_FIRST:
			     STRCPY(code, "NFA_START_INVISIBLE_FIRST"); તોડો;
	સ્થિતિ NFA_START_INVISIBLE_NEG:
			       STRCPY(code, "NFA_START_INVISIBLE_NEG"); તોડો;
	સ્થિતિ NFA_START_INVISIBLE_NEG_FIRST:
			 STRCPY(code, "NFA_START_INVISIBLE_NEG_FIRST"); તોડો;
	સ્થિતિ NFA_START_INVISIBLE_BEFORE:
			    STRCPY(code, "NFA_START_INVISIBLE_BEFORE"); તોડો;
	સ્થિતિ NFA_START_INVISIBLE_BEFORE_FIRST:
		      STRCPY(code, "NFA_START_INVISIBLE_BEFORE_FIRST"); તોડો;
	સ્થિતિ NFA_START_INVISIBLE_BEFORE_NEG:
			STRCPY(code, "NFA_START_INVISIBLE_BEFORE_NEG"); તોડો;
	સ્થિતિ NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
		  STRCPY(code, "NFA_START_INVISIBLE_BEFORE_NEG_FIRST"); તોડો;
	સ્થિતિ NFA_START_PATTERN:   STRCPY(code, "NFA_START_PATTERN"); તોડો;
	સ્થિતિ NFA_END_INVISIBLE:	    STRCPY(code, "NFA_END_INVISIBLE"); તોડો;
	સ્થિતિ NFA_END_INVISIBLE_NEG: STRCPY(code, "NFA_END_INVISIBLE_NEG"); તોડો;
	સ્થિતિ NFA_END_PATTERN:	    STRCPY(code, "NFA_END_PATTERN"); તોડો;

	સ્થિતિ NFA_COMPOSING:	    STRCPY(code, "NFA_COMPOSING"); તોડો;
	સ્થિતિ NFA_END_COMPOSING:	    STRCPY(code, "NFA_END_COMPOSING"); તોડો;
	સ્થિતિ NFA_OPT_CHARS:	    STRCPY(code, "NFA_OPT_CHARS"); તોડો;

	સ્થિતિ NFA_MOPEN:
	સ્થિતિ NFA_MOPEN1:
	સ્થિતિ NFA_MOPEN2:
	સ્થિતિ NFA_MOPEN3:
	સ્થિતિ NFA_MOPEN4:
	સ્થિતિ NFA_MOPEN5:
	સ્થિતિ NFA_MOPEN6:
	સ્થિતિ NFA_MOPEN7:
	સ્થિતિ NFA_MOPEN8:
	સ્થિતિ NFA_MOPEN9:
	    STRCPY(code, "NFA_MOPEN(x)");
	    code[10] = c - NFA_MOPEN + '0';
	    તોડો;
	સ્થિતિ NFA_MCLOSE:
	સ્થિતિ NFA_MCLOSE1:
	સ્થિતિ NFA_MCLOSE2:
	સ્થિતિ NFA_MCLOSE3:
	સ્થિતિ NFA_MCLOSE4:
	સ્થિતિ NFA_MCLOSE5:
	સ્થિતિ NFA_MCLOSE6:
	સ્થિતિ NFA_MCLOSE7:
	સ્થિતિ NFA_MCLOSE8:
	સ્થિતિ NFA_MCLOSE9:
	    STRCPY(code, "NFA_MCLOSE(x)");
	    code[11] = c - NFA_MCLOSE + '0';
	    તોડો;
# ifdef FEAT_SYN_HL
	સ્થિતિ NFA_ZOPEN:
	સ્થિતિ NFA_ZOPEN1:
	સ્થિતિ NFA_ZOPEN2:
	સ્થિતિ NFA_ZOPEN3:
	સ્થિતિ NFA_ZOPEN4:
	સ્થિતિ NFA_ZOPEN5:
	સ્થિતિ NFA_ZOPEN6:
	સ્થિતિ NFA_ZOPEN7:
	સ્થિતિ NFA_ZOPEN8:
	સ્થિતિ NFA_ZOPEN9:
	    STRCPY(code, "NFA_ZOPEN(x)");
	    code[10] = c - NFA_ZOPEN + '0';
	    તોડો;
	સ્થિતિ NFA_ZCLOSE:
	સ્થિતિ NFA_ZCLOSE1:
	સ્થિતિ NFA_ZCLOSE2:
	સ્થિતિ NFA_ZCLOSE3:
	સ્થિતિ NFA_ZCLOSE4:
	સ્થિતિ NFA_ZCLOSE5:
	સ્થિતિ NFA_ZCLOSE6:
	સ્થિતિ NFA_ZCLOSE7:
	સ્થિતિ NFA_ZCLOSE8:
	સ્થિતિ NFA_ZCLOSE9:
	    STRCPY(code, "NFA_ZCLOSE(x)");
	    code[11] = c - NFA_ZCLOSE + '0';
	    તોડો;
# endif
	સ્થિતિ NFA_EOL:		STRCPY(code, "NFA_EOL "); તોડો;
	સ્થિતિ NFA_BOL:		STRCPY(code, "NFA_BOL "); તોડો;
	સ્થિતિ NFA_EOW:		STRCPY(code, "NFA_EOW "); તોડો;
	સ્થિતિ NFA_BOW:		STRCPY(code, "NFA_BOW "); તોડો;
	સ્થિતિ NFA_EOF:		STRCPY(code, "NFA_EOF "); તોડો;
	સ્થિતિ NFA_BOF:		STRCPY(code, "NFA_BOF "); તોડો;
	સ્થિતિ NFA_LNUM:		STRCPY(code, "NFA_LNUM "); તોડો;
	સ્થિતિ NFA_LNUM_GT:	STRCPY(code, "NFA_LNUM_GT "); તોડો;
	સ્થિતિ NFA_LNUM_LT:	STRCPY(code, "NFA_LNUM_LT "); તોડો;
	સ્થિતિ NFA_COL:		STRCPY(code, "NFA_COL "); તોડો;
	સ્થિતિ NFA_COL_GT:	STRCPY(code, "NFA_COL_GT "); તોડો;
	સ્થિતિ NFA_COL_LT:	STRCPY(code, "NFA_COL_LT "); તોડો;
	સ્થિતિ NFA_VCOL:		STRCPY(code, "NFA_VCOL "); તોડો;
	સ્થિતિ NFA_VCOL_GT:	STRCPY(code, "NFA_VCOL_GT "); તોડો;
	સ્થિતિ NFA_VCOL_LT:	STRCPY(code, "NFA_VCOL_LT "); તોડો;
	સ્થિતિ NFA_MARK:		STRCPY(code, "NFA_MARK "); તોડો;
	સ્થિતિ NFA_MARK_GT:	STRCPY(code, "NFA_MARK_GT "); તોડો;
	સ્થિતિ NFA_MARK_LT:	STRCPY(code, "NFA_MARK_LT "); તોડો;
	સ્થિતિ NFA_CURSOR:	STRCPY(code, "NFA_CURSOR "); તોડો;
	સ્થિતિ NFA_VISUAL:	STRCPY(code, "NFA_VISUAL "); તોડો;
	સ્થિતિ NFA_ANY_COMPOSING:	STRCPY(code, "NFA_ANY_COMPOSING "); તોડો;

	સ્થિતિ NFA_STAR:		STRCPY(code, "NFA_STAR "); તોડો;
	સ્થિતિ NFA_STAR_NONGREEDY: STRCPY(code, "NFA_STAR_NONGREEDY "); તોડો;
	સ્થિતિ NFA_QUEST:		STRCPY(code, "NFA_QUEST"); તોડો;
	સ્થિતિ NFA_QUEST_NONGREEDY: STRCPY(code, "NFA_QUEST_NON_GREEDY"); તોડો;
	સ્થિતિ NFA_EMPTY:		STRCPY(code, "NFA_EMPTY"); તોડો;
	સ્થિતિ NFA_OR:		STRCPY(code, "NFA_OR"); તોડો;

	સ્થિતિ NFA_START_COLL:	STRCPY(code, "NFA_START_COLL"); તોડો;
	સ્થિતિ NFA_END_COLL:	STRCPY(code, "NFA_END_COLL"); તોડો;
	સ્થિતિ NFA_START_NEG_COLL: STRCPY(code, "NFA_START_NEG_COLL"); તોડો;
	સ્થિતિ NFA_END_NEG_COLL:	STRCPY(code, "NFA_END_NEG_COLL"); તોડો;
	સ્થિતિ NFA_RANGE:		STRCPY(code, "NFA_RANGE"); તોડો;
	સ્થિતિ NFA_RANGE_MIN:	STRCPY(code, "NFA_RANGE_MIN"); તોડો;
	સ્થિતિ NFA_RANGE_MAX:	STRCPY(code, "NFA_RANGE_MAX"); તોડો;

	સ્થિતિ NFA_CLASS_ALNUM:	STRCPY(code, "NFA_CLASS_ALNUM"); તોડો;
	સ્થિતિ NFA_CLASS_ALPHA:	STRCPY(code, "NFA_CLASS_ALPHA"); તોડો;
	સ્થિતિ NFA_CLASS_BLANK:	STRCPY(code, "NFA_CLASS_BLANK"); તોડો;
	સ્થિતિ NFA_CLASS_CNTRL:	STRCPY(code, "NFA_CLASS_CNTRL"); તોડો;
	સ્થિતિ NFA_CLASS_DIGIT:	STRCPY(code, "NFA_CLASS_DIGIT"); તોડો;
	સ્થિતિ NFA_CLASS_GRAPH:	STRCPY(code, "NFA_CLASS_GRAPH"); તોડો;
	સ્થિતિ NFA_CLASS_LOWER:	STRCPY(code, "NFA_CLASS_LOWER"); તોડો;
	સ્થિતિ NFA_CLASS_PRINT:	STRCPY(code, "NFA_CLASS_PRINT"); તોડો;
	સ્થિતિ NFA_CLASS_PUNCT:	STRCPY(code, "NFA_CLASS_PUNCT"); તોડો;
	સ્થિતિ NFA_CLASS_SPACE:	STRCPY(code, "NFA_CLASS_SPACE"); તોડો;
	સ્થિતિ NFA_CLASS_UPPER:	STRCPY(code, "NFA_CLASS_UPPER"); તોડો;
	સ્થિતિ NFA_CLASS_XDIGIT:	STRCPY(code, "NFA_CLASS_XDIGIT"); તોડો;
	સ્થિતિ NFA_CLASS_TAB:	STRCPY(code, "NFA_CLASS_TAB"); તોડો;
	સ્થિતિ NFA_CLASS_RETURN:	STRCPY(code, "NFA_CLASS_RETURN"); તોડો;
	સ્થિતિ NFA_CLASS_BACKSPACE:   STRCPY(code, "NFA_CLASS_BACKSPACE"); તોડો;
	સ્થિતિ NFA_CLASS_ESCAPE:	STRCPY(code, "NFA_CLASS_ESCAPE"); તોડો;
	સ્થિતિ NFA_CLASS_IDENT:	STRCPY(code, "NFA_CLASS_IDENT"); તોડો;
	સ્થિતિ NFA_CLASS_KEYWORD:	STRCPY(code, "NFA_CLASS_KEYWORD"); તોડો;
	સ્થિતિ NFA_CLASS_FNAME:	STRCPY(code, "NFA_CLASS_FNAME"); તોડો;

	સ્થિતિ NFA_ANY:	STRCPY(code, "NFA_ANY"); તોડો;
	સ્થિતિ NFA_IDENT:	STRCPY(code, "NFA_IDENT"); તોડો;
	સ્થિતિ NFA_SIDENT:STRCPY(code, "NFA_SIDENT"); તોડો;
	સ્થિતિ NFA_KWORD:	STRCPY(code, "NFA_KWORD"); તોડો;
	સ્થિતિ NFA_SKWORD:STRCPY(code, "NFA_SKWORD"); તોડો;
	સ્થિતિ NFA_FNAME:	STRCPY(code, "NFA_FNAME"); તોડો;
	સ્થિતિ NFA_SFNAME:STRCPY(code, "NFA_SFNAME"); તોડો;
	સ્થિતિ NFA_PRINT:	STRCPY(code, "NFA_PRINT"); તોડો;
	સ્થિતિ NFA_SPRINT:STRCPY(code, "NFA_SPRINT"); તોડો;
	સ્થિતિ NFA_WHITE:	STRCPY(code, "NFA_WHITE"); તોડો;
	સ્થિતિ NFA_NWHITE:STRCPY(code, "NFA_NWHITE"); તોડો;
	સ્થિતિ NFA_DIGIT:	STRCPY(code, "NFA_DIGIT"); તોડો;
	સ્થિતિ NFA_NDIGIT:STRCPY(code, "NFA_NDIGIT"); તોડો;
	સ્થિતિ NFA_HEX:	STRCPY(code, "NFA_HEX"); તોડો;
	સ્થિતિ NFA_NHEX:	STRCPY(code, "NFA_NHEX"); તોડો;
	સ્થિતિ NFA_OCTAL:	STRCPY(code, "NFA_OCTAL"); તોડો;
	સ્થિતિ NFA_NOCTAL:STRCPY(code, "NFA_NOCTAL"); તોડો;
	સ્થિતિ NFA_WORD:	STRCPY(code, "NFA_WORD"); તોડો;
	સ્થિતિ NFA_NWORD:	STRCPY(code, "NFA_NWORD"); તોડો;
	સ્થિતિ NFA_HEAD:	STRCPY(code, "NFA_HEAD"); તોડો;
	સ્થિતિ NFA_NHEAD:	STRCPY(code, "NFA_NHEAD"); તોડો;
	સ્થિતિ NFA_ALPHA:	STRCPY(code, "NFA_ALPHA"); તોડો;
	સ્થિતિ NFA_NALPHA:STRCPY(code, "NFA_NALPHA"); તોડો;
	સ્થિતિ NFA_LOWER:	STRCPY(code, "NFA_LOWER"); તોડો;
	સ્થિતિ NFA_NLOWER:STRCPY(code, "NFA_NLOWER"); તોડો;
	સ્થિતિ NFA_UPPER:	STRCPY(code, "NFA_UPPER"); તોડો;
	સ્થિતિ NFA_NUPPER:STRCPY(code, "NFA_NUPPER"); તોડો;
	સ્થિતિ NFA_LOWER_IC:  STRCPY(code, "NFA_LOWER_IC"); તોડો;
	સ્થિતિ NFA_NLOWER_IC: STRCPY(code, "NFA_NLOWER_IC"); તોડો;
	સ્થિતિ NFA_UPPER_IC:  STRCPY(code, "NFA_UPPER_IC"); તોડો;
	સ્થિતિ NFA_NUPPER_IC: STRCPY(code, "NFA_NUPPER_IC"); તોડો;

	મૂળભૂત:
	    STRCPY(code, "CHAR(x)");
	    code[5] = c;
    }

    જો (addnl == TRUE)
	STRCAT(code, " + NEWLINE ");

}

# ifdef ENABLE_LOG
સ્થિર FILE *log_fd;
સ્થિર char_u e_log_open_failed[] = N_("Could not open temporary log file for writing, displaying on stderr... ");

/*
 * Print the postfix notation of the current regexp.
 */
    સ્થિર શૂન્ય
nfa_postfix_dump(char_u *પદાવલિ, પૂર્ણાંક retval)
{
    પૂર્ણાંક *p;
    FILE *f;

    f = fopen(NFA_REGEXP_DUMP_LOG, "a");
    જો (f == ખાલી)
	પાછા;

    fprintf(f, "\n-------------------------\n");
    જો (retval == FAIL)
	fprintf(f, ">>> NFA engine failed... \n");
    નહીંતર જો (retval == OK)
	fprintf(f, ">>> NFA engine succeeded !\n");
    fprintf(f, "Regexp: \"%s\"\nPostfix notation (char): \"", પદાવલિ);
    માટે (p = post_start; *p && p < post_ptr; p++)
    {
	nfa_set_code(*p);
	fprintf(f, "%s, ", code);
    }
    fprintf(f, "\"\nPostfix notation (int): ");
    માટે (p = post_start; *p && p < post_ptr; p++)
	fprintf(f, "%d ", *p);
    fprintf(f, "\n\n");
    fclose(f);
}

/*
 * Print the NFA starting with a root node "state".
 */
    સ્થિર શૂન્ય
nfa_print_state(FILE *debugf, nfa_state_T *state)
{
    garray_T indent;

    ga_init2(&indent, 1, 64);
    ga_append(&indent, '\0');
    nfa_print_state2(debugf, state, &indent);
    ga_clear(&indent);
}

    સ્થિર શૂન્ય
nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent)
{
    char_u  *p;

    જો (state == ખાલી)
	પાછા;

    fprintf(debugf, "(%2d)", abs(state->id));

    // Output indent
    p = (char_u *)indent->ga_data;
    જો (indent->ga_len >= 3)
    {
	પૂર્ણાંક	last = indent->ga_len - 3;
	char_u	save[2];

	STRNCPY(save, &p[last], 2);
	memcpy(&p[last], "+-", 2);
	fprintf(debugf, " %s", p);
	STRNCPY(&p[last], save, 2);
    }
    નહીંતર
	fprintf(debugf, " %s", p);

    nfa_set_code(state->c);
    fprintf(debugf, "%s (%d) (id=%d) val=%d\n",
		 code,
		 state->c,
		 abs(state->id),
		 state->val);
    જો (state->id < 0)
	પાછા;

    state->id = abs(state->id) * -1;

    // grow indent for state->out
    indent->ga_len -= 1;
    જો (state->out1)
	GA_CONCAT_LITERAL(indent, "| ");
    નહીંતર
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
    સ્થિર શૂન્ય
nfa_dump(nfa_regprog_T *prog)
{
    FILE *debugf = fopen(NFA_REGEXP_DUMP_LOG, "a");

    જો (debugf == ખાલી)
	પાછા;

    nfa_print_state(debugf, prog->start);

    જો (prog->reganch)
	fprintf(debugf, "reganch: %d\n", prog->reganch);
    જો (prog->regstart != NUL)
	fprintf(debugf, "regstart: %c (decimal: %d)\n",
		prog->regstart, prog->regstart);
    જો (prog->match_text != ખાલી)
	fprintf(debugf, "match_text: \"%s\"\n", prog->match_text);

    fclose(debugf);
}
# endif	    // ENABLE_LOG
#endif	    // DEBUG

/*
 * Parse r.e. @expr and convert it into postfix form.
 * Return the postfix string on success, NULL otherwise.
 */
    સ્થિર પૂર્ણાંક *
re2post(શૂન્ય)
{
    જો (nfa_reg(REG_NOPAREN) == FAIL)
	પાછા ખાલી;
    EMIT(NFA_MOPEN);
    પાછા post_start;
}

// NB. Some of the code below is inspired by Russ's.

/*
 * Represents an NFA state plus zero or one or two arrows exiting.
 * if c == MATCH, no arrows out; matching state.
 * If c == SPLIT, unlabeled arrows to out and out1 (if != NULL).
 * If c < 256, labeled arrow with character c to out.
 */

સ્થિર nfa_state_T	*state_ptr; // points to nfa_prog->state

/*
 * Allocate and initialize nfa_state_T.
 */
    સ્થિર nfa_state_T *
alloc_state(પૂર્ણાંક c, nfa_state_T *out, nfa_state_T *out1)
{
    nfa_state_T *s;

    જો (istate >= nstate)
	પાછા ખાલી;

    s = &state_ptr[istate++];

    s->c    = c;
    s->out  = out;
    s->out1 = out1;
    s->val  = 0;

    s->id   = istate;
    s->lastlist[0] = 0;
    s->lastlist[1] = 0;

    પાછા s;
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
પ્રકારવ્યાખ્યા સંઘ Ptrlist Ptrlist;
સંઘ Ptrlist
{
    Ptrlist	*next;
    nfa_state_T	*s;
};

સંરચના Frag
{
    nfa_state_T *start;
    Ptrlist	*out;
};
પ્રકારવ્યાખ્યા સંરચના Frag Frag_T;

// Reused across compilations by post2nfa(); freed in free_regexp_stuff().
સ્થિર Frag_T *nfa_ઢગલો;
સ્થિર પૂર્ણાંક nfa_ઢગલો_len;

/*
 * Initialize a Frag_T struct and return it.
 */
    સ્થિર Frag_T
frag(nfa_state_T *start, Ptrlist *out)
{
    Frag_T n;

    n.start = start;
    n.out = out;
    પાછા n;
}

/*
 * Create singleton list containing just outp.
 */
    સ્થિર Ptrlist *
list1(
    nfa_state_T	**outp)
{
    Ptrlist *l;

    l = (Ptrlist *)outp;
    l->next = ખાલી;
    પાછા l;
}

/*
 * Patch the list of states at out to point to start.
 */
    સ્થિર શૂન્ય
patch(Ptrlist *l, nfa_state_T *s)
{
    Ptrlist *next;

    માટે (; l; l = next)
    {
	next = l->next;
	l->s = s;
    }
}


/*
 * Join the two lists l1 and l2, returning the combination.
 */
    સ્થિર Ptrlist *
append(Ptrlist *l1, Ptrlist *l2)
{
    Ptrlist *oldl1;

    oldl1 = l1;
    જ્યારે (l1->next)
	l1 = l1->next;
    l1->next = l2;
    પાછા oldl1;
}

/*
 * Stack used for transforming postfix form into NFA.
 */
સ્થિર Frag_T empty;

    સ્થિર શૂન્ય
st_error(પૂર્ણાંક *postfix UNUSED, પૂર્ણાંક *end UNUSED, પૂર્ણાંક *p UNUSED)
{
#ifdef NFA_REGEXP_ERROR_LOG
    FILE *df;
    પૂર્ણાંક *p2;

    df = fopen(NFA_REGEXP_ERROR_LOG, "a");
    જો (df)
    {
	fprintf(df, "Error popping the stack!\n");
# ifdef DEBUG
	fprintf(df, "Current regexp is \"%s\"\n", nfa_regengine.પદાવલિ);
# endif
	fprintf(df, "Postfix form is: ");
# ifdef DEBUG
	માટે (p2 = postfix; p2 < end; p2++)
	{
	    nfa_set_code(*p2);
	    fprintf(df, "%s, ", code);
	}
	nfa_set_code(*p);
	fprintf(df, "\nCurrent position is: ");
	માટે (p2 = postfix; p2 <= p; p2 ++)
	{
	    nfa_set_code(*p2);
	    fprintf(df, "%s, ", code);
	}
# નહીંતર
	માટે (p2 = postfix; p2 < end; p2++)
	    fprintf(df, "%d, ", *p2);
	fprintf(df, "\nCurrent position is: ");
	માટે (p2 = postfix; p2 <= p; p2 ++)
	    fprintf(df, "%d, ", *p2);
# endif
	fprintf(df, "\n--------------------------\n");
	fclose(df);
    }
#endif
    emsg(_(e_nfa_regexp_could_not_કાઢો_ઢગલો));
}

/*
 * Push an item onto the stack.
 */
    સ્થિર શૂન્ય
st_ધકેલો(Frag_T s, Frag_T **p, Frag_T *ઢગલો_end)
{
    Frag_T *stackp = *p;

    જો (stackp >= ઢગલો_end)
	પાછા;
    *stackp = s;
    *p = *p + 1;
}

/*
 * Pop an item from the stack.
 */
    સ્થિર Frag_T
st_કાઢો(Frag_T **p, Frag_T *ઢગલો)
{
    Frag_T *stackp;

    *p = *p - 1;
    stackp = *p;
    જો (stackp < ઢગલો)
	પાછા empty;
    પાછા **p;
}

/*
 * Estimate the maximum byte length of anything matching "state".
 * When unknown or unlimited return -1.
 */
    સ્થિર પૂર્ણાંક
nfa_max_width(nfa_state_T *startstate, પૂર્ણાંક depth)
{
    પૂર્ણાંક		    l, r;
    nfa_state_T	    *state = startstate;
    પૂર્ણાંક		    len = 0;

    // detect looping in a NFA_SPLIT
    જો (depth > 4)
	પાછા -1;

    જ્યારે (state != ખાલી)
    {
	પસંદગી (state->c)
	{
	    સ્થિતિ NFA_END_INVISIBLE:
	    સ્થિતિ NFA_END_INVISIBLE_NEG:
		// the end, return what we have
		પાછા len;

	    સ્થિતિ NFA_SPLIT:
		// two alternatives, use the maximum
		l = nfa_max_width(state->out, depth + 1);
		r = nfa_max_width(state->out1, depth + 1);
		જો (l < 0 || r < 0)
		    પાછા -1;
		પાછા len + (l > r ? l : r);

	    સ્થિતિ NFA_ANY:
	    સ્થિતિ NFA_START_COLL:
	    સ્થિતિ NFA_START_NEG_COLL:
		// matches some character, including composing chars
		જો (enc_utf8)
		    len += MB_MAXBYTES;
		નહીંતર જો (has_mbyte)
		    len += 2;
		નહીંતર
		    ++len;
		જો (state->c != NFA_ANY)
		{
		    // Skip over the compiled collection.
		    // malformed NFAs must not crash width estimation.
		    જો (state->out1 == ખાલી || state->out1->out == ખાલી)
			પાછા -1;
		    state = state->out1->out;
		    ચાલુ;
		}
		તોડો;

	    સ્થિતિ NFA_DIGIT:
	    સ્થિતિ NFA_WHITE:
	    સ્થિતિ NFA_HEX:
	    સ્થિતિ NFA_OCTAL:
		// ascii
		++len;
		તોડો;

	    સ્થિતિ NFA_IDENT:
	    સ્થિતિ NFA_SIDENT:
	    સ્થિતિ NFA_KWORD:
	    સ્થિતિ NFA_SKWORD:
	    સ્થિતિ NFA_FNAME:
	    સ્થિતિ NFA_SFNAME:
	    સ્થિતિ NFA_PRINT:
	    સ્થિતિ NFA_SPRINT:
	    સ્થિતિ NFA_NWHITE:
	    સ્થિતિ NFA_NDIGIT:
	    સ્થિતિ NFA_NHEX:
	    સ્થિતિ NFA_NOCTAL:
	    સ્થિતિ NFA_WORD:
	    સ્થિતિ NFA_NWORD:
	    સ્થિતિ NFA_HEAD:
	    સ્થિતિ NFA_NHEAD:
	    સ્થિતિ NFA_ALPHA:
	    સ્થિતિ NFA_NALPHA:
	    સ્થિતિ NFA_LOWER:
	    સ્થિતિ NFA_NLOWER:
	    સ્થિતિ NFA_UPPER:
	    સ્થિતિ NFA_NUPPER:
	    સ્થિતિ NFA_LOWER_IC:
	    સ્થિતિ NFA_NLOWER_IC:
	    સ્થિતિ NFA_UPPER_IC:
	    સ્થિતિ NFA_NUPPER_IC:
	    સ્થિતિ NFA_ANY_COMPOSING:
		// possibly non-ascii
		જો (has_mbyte)
		    len += 3;
		નહીંતર
		    ++len;
		તોડો;

	    સ્થિતિ NFA_START_INVISIBLE:
	    સ્થિતિ NFA_START_INVISIBLE_NEG:
	    સ્થિતિ NFA_START_INVISIBLE_BEFORE:
	    સ્થિતિ NFA_START_INVISIBLE_BEFORE_NEG:
		// zero-width, out1 points to the END state
		state = state->out1->out;
		ચાલુ;

	    સ્થિતિ NFA_BACKREF1:
	    સ્થિતિ NFA_BACKREF2:
	    સ્થિતિ NFA_BACKREF3:
	    સ્થિતિ NFA_BACKREF4:
	    સ્થિતિ NFA_BACKREF5:
	    સ્થિતિ NFA_BACKREF6:
	    સ્થિતિ NFA_BACKREF7:
	    સ્થિતિ NFA_BACKREF8:
	    સ્થિતિ NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	    સ્થિતિ NFA_ZREF1:
	    સ્થિતિ NFA_ZREF2:
	    સ્થિતિ NFA_ZREF3:
	    સ્થિતિ NFA_ZREF4:
	    સ્થિતિ NFA_ZREF5:
	    સ્થિતિ NFA_ZREF6:
	    સ્થિતિ NFA_ZREF7:
	    સ્થિતિ NFA_ZREF8:
	    સ્થિતિ NFA_ZREF9:
#endif
	    સ્થિતિ NFA_NEWL:
	    સ્થિતિ NFA_SKIP:
		// unknown width
		પાછા -1;

	    સ્થિતિ NFA_BOL:
	    સ્થિતિ NFA_EOL:
	    સ્થિતિ NFA_BOF:
	    સ્થિતિ NFA_EOF:
	    સ્થિતિ NFA_BOW:
	    સ્થિતિ NFA_EOW:
	    સ્થિતિ NFA_MOPEN:
	    સ્થિતિ NFA_MOPEN1:
	    સ્થિતિ NFA_MOPEN2:
	    સ્થિતિ NFA_MOPEN3:
	    સ્થિતિ NFA_MOPEN4:
	    સ્થિતિ NFA_MOPEN5:
	    સ્થિતિ NFA_MOPEN6:
	    સ્થિતિ NFA_MOPEN7:
	    સ્થિતિ NFA_MOPEN8:
	    સ્થિતિ NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	    સ્થિતિ NFA_ZOPEN:
	    સ્થિતિ NFA_ZOPEN1:
	    સ્થિતિ NFA_ZOPEN2:
	    સ્થિતિ NFA_ZOPEN3:
	    સ્થિતિ NFA_ZOPEN4:
	    સ્થિતિ NFA_ZOPEN5:
	    સ્થિતિ NFA_ZOPEN6:
	    સ્થિતિ NFA_ZOPEN7:
	    સ્થિતિ NFA_ZOPEN8:
	    સ્થિતિ NFA_ZOPEN9:
	    સ્થિતિ NFA_ZCLOSE:
	    સ્થિતિ NFA_ZCLOSE1:
	    સ્થિતિ NFA_ZCLOSE2:
	    સ્થિતિ NFA_ZCLOSE3:
	    સ્થિતિ NFA_ZCLOSE4:
	    સ્થિતિ NFA_ZCLOSE5:
	    સ્થિતિ NFA_ZCLOSE6:
	    સ્થિતિ NFA_ZCLOSE7:
	    સ્થિતિ NFA_ZCLOSE8:
	    સ્થિતિ NFA_ZCLOSE9:
#endif
	    સ્થિતિ NFA_MCLOSE:
	    સ્થિતિ NFA_MCLOSE1:
	    સ્થિતિ NFA_MCLOSE2:
	    સ્થિતિ NFA_MCLOSE3:
	    સ્થિતિ NFA_MCLOSE4:
	    સ્થિતિ NFA_MCLOSE5:
	    સ્થિતિ NFA_MCLOSE6:
	    સ્થિતિ NFA_MCLOSE7:
	    સ્થિતિ NFA_MCLOSE8:
	    સ્થિતિ NFA_MCLOSE9:
	    સ્થિતિ NFA_NOPEN:
	    સ્થિતિ NFA_NCLOSE:

	    સ્થિતિ NFA_LNUM_GT:
	    સ્થિતિ NFA_LNUM_LT:
	    સ્થિતિ NFA_COL_GT:
	    સ્થિતિ NFA_COL_LT:
	    સ્થિતિ NFA_VCOL_GT:
	    સ્થિતિ NFA_VCOL_LT:
	    સ્થિતિ NFA_MARK_GT:
	    સ્થિતિ NFA_MARK_LT:
	    સ્થિતિ NFA_VISUAL:
	    સ્થિતિ NFA_LNUM:
	    સ્થિતિ NFA_CURSOR:
	    સ્થિતિ NFA_COL:
	    સ્થિતિ NFA_VCOL:
	    સ્થિતિ NFA_MARK:

	    સ્થિતિ NFA_ZSTART:
	    સ્થિતિ NFA_ZEND:
	    સ્થિતિ NFA_OPT_CHARS:
	    સ્થિતિ NFA_EMPTY:
	    સ્થિતિ NFA_START_PATTERN:
	    સ્થિતિ NFA_END_PATTERN:
	    સ્થિતિ NFA_COMPOSING:
	    સ્થિતિ NFA_END_COMPOSING:
		// zero-width
		તોડો;

	    મૂળભૂત:
		જો (state->c < 0)
		    // don't know what this is
		    પાછા -1;
		// normal character
		len += MB_CHAR2LEN(state->c);
		તોડો;
	}

	// normal way to continue
	state = state->out;
    }

    // unrecognized, "cannot happen"
    પાછા -1;
}

/*
 * Convert a postfix form into its equivalent NFA.
 * Return the NFA start state on success, NULL otherwise.
 */
    સ્થિર nfa_state_T *
post2nfa(પૂર્ણાંક *postfix, પૂર્ણાંક *end, પૂર્ણાંક nfa_calc_size)
{
    પૂર્ણાંક		*p;
    પૂર્ણાંક		mopen;
    પૂર્ણાંક		mclose;
    Frag_T	*ઢગલો = ખાલી;
    Frag_T	*stackp = ખાલી;
    Frag_T	*ઢગલો_end = ખાલી;
    Frag_T	e1;
    Frag_T	e2;
    Frag_T	e;
    nfa_state_T	*s;
    nfa_state_T	*s1;
    nfa_state_T	*matchstate;
    nfa_state_T	*ret = ખાલી;

    જો (postfix == ખાલી)
	પાછા ખાલી;

#define ધકેલો_2(s)	    st_ધકેલો((s), &stackp, ઢગલો_end)
#define કાઢો_2()	    st_કાઢો(&stackp, ઢગલો);		\
		    જો (stackp < ઢગલો)			\
		    {					\
			st_error(postfix, end, p);	\
			પાછા ખાલી;			\
		    }

    જો (nfa_calc_size == FALSE)
    {
	// Reuse the fragment stack across compilations, growing when needed;
	// it is freed in free_regexp_stuff().
	જો (nfa_ઢગલો == ખાલી || nfa_ઢગલો_len < nstate + 1)
	{
	    vim_free(nfa_ઢગલો);
	    nfa_ઢગલો = ALLOC_MULT(Frag_T, nstate + 1);
	    જો (nfa_ઢગલો == ખાલી)
	    {
		nfa_ઢગલો_len = 0;
		પાછા ખાલી;
	    }
	    nfa_ઢગલો_len = nstate + 1;
	}
	ઢગલો = nfa_ઢગલો;
	stackp = ઢગલો;
	ઢગલો_end = ઢગલો + (nstate + 1);
    }

    માટે (p = postfix; p < end; ++p)
    {
	પસંદગી (*p)
	{
	સ્થિતિ NFA_CONCAT:
	    // Concatenation.
	    // Pay attention: this operator does not exist in the r.e. itself
	    // (it is implicit, really).  It is added when r.e. is translated
	    // to postfix form in re2post().
	    જો (nfa_calc_size == TRUE)
	    {
		// nstate += 0;
		તોડો;
	    }
	    e2 = કાઢો_2();
	    e1 = કાઢો_2();
	    patch(e1.out, e2.start);
	    ધકેલો_2(frag(e1.start, e2.out));
	    તોડો;

	સ્થિતિ NFA_OR:
	    // Alternation
	    જો (nfa_calc_size == TRUE)
	    {
		nstate++;
		તોડો;
	    }
	    e2 = કાઢો_2();
	    e1 = કાઢો_2();
	    s = alloc_state(NFA_SPLIT, e1.start, e2.start);
	    જો (s == ખાલી)
		જાઓ theend;
	    ધકેલો_2(frag(s, append(e1.out, e2.out)));
	    તોડો;

	સ્થિતિ NFA_STAR:
	    // Zero or more, prefer more
	    જો (nfa_calc_size == TRUE)
	    {
		nstate++;
		તોડો;
	    }
	    e = કાઢો_2();
	    s = alloc_state(NFA_SPLIT, e.start, ખાલી);
	    જો (s == ખાલી)
		જાઓ theend;
	    patch(e.out, s);
	    ધકેલો_2(frag(s, list1(&s->out1)));
	    તોડો;

	સ્થિતિ NFA_STAR_NONGREEDY:
	    // Zero or more, prefer zero
	    જો (nfa_calc_size == TRUE)
	    {
		nstate++;
		તોડો;
	    }
	    e = કાઢો_2();
	    s = alloc_state(NFA_SPLIT, ખાલી, e.start);
	    જો (s == ખાલી)
		જાઓ theend;
	    patch(e.out, s);
	    ધકેલો_2(frag(s, list1(&s->out)));
	    તોડો;

	સ્થિતિ NFA_QUEST:
	    // one or zero atoms=> greedy match
	    જો (nfa_calc_size == TRUE)
	    {
		nstate++;
		તોડો;
	    }
	    e = કાઢો_2();
	    s = alloc_state(NFA_SPLIT, e.start, ખાલી);
	    જો (s == ખાલી)
		જાઓ theend;
	    ધકેલો_2(frag(s, append(e.out, list1(&s->out1))));
	    તોડો;

	સ્થિતિ NFA_QUEST_NONGREEDY:
	    // zero or one atoms => non-greedy match
	    જો (nfa_calc_size == TRUE)
	    {
		nstate++;
		તોડો;
	    }
	    e = કાઢો_2();
	    s = alloc_state(NFA_SPLIT, ખાલી, e.start);
	    જો (s == ખાલી)
		જાઓ theend;
	    ધકેલો_2(frag(s, append(e.out, list1(&s->out))));
	    તોડો;

	સ્થિતિ NFA_END_COLL:
	સ્થિતિ NFA_END_NEG_COLL:
	    // On the stack is the sequence starting with NFA_START_COLL or
	    // NFA_START_NEG_COLL and all possible characters. Patch it to
	    // add the output to the start.
	    જો (nfa_calc_size == TRUE)
	    {
		nstate++;
		તોડો;
	    }
	    e = કાઢો_2();
	    s = alloc_state(NFA_END_COLL, ખાલી, ખાલી);
	    જો (s == ખાલી)
		જાઓ theend;
	    patch(e.out, s);
	    e.start->out1 = s;
	    ધકેલો_2(frag(e.start, list1(&s->out)));
	    તોડો;

	સ્થિતિ NFA_RANGE:
	    // Before this are two characters, the low and high end of a
	    // range.  Turn them into two states with MIN and MAX.
	    જો (nfa_calc_size == TRUE)
	    {
		// nstate += 0;
		તોડો;
	    }
	    e2 = કાઢો_2();
	    e1 = કાઢો_2();
	    e2.start->val = e2.start->c;
	    e2.start->c = NFA_RANGE_MAX;
	    e1.start->val = e1.start->c;
	    e1.start->c = NFA_RANGE_MIN;
	    patch(e1.out, e2.start);
	    ધકેલો_2(frag(e1.start, e2.out));
	    તોડો;

	સ્થિતિ NFA_EMPTY:
	    // 0-length, used in a repetition with max/min count of 0
	    જો (nfa_calc_size == TRUE)
	    {
		nstate++;
		તોડો;
	    }
	    s = alloc_state(NFA_EMPTY, ખાલી, ખાલી);
	    જો (s == ખાલી)
		જાઓ theend;
	    ધકેલો_2(frag(s, list1(&s->out)));
	    તોડો;

	સ્થિતિ NFA_OPT_CHARS:
	  {
	    પૂર્ણાંક    n;

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
	    જો (nfa_calc_size == TRUE)
	    {
		nstate += n;
		તોડો;
	    }
	    s = ખાલી; // avoid compiler warning
	    e1.out = ખાલી; // stores list with out1's
	    s1 = ખાલી; // previous NFA_SPLIT to connect to
	    જ્યારે (n-- > 0)
	    {
		e = કાઢો_2(); // get character
		s = alloc_state(NFA_SPLIT, e.start, ખાલી);
		જો (s == ખાલી)
		    જાઓ theend;
		જો (e1.out == ખાલી)
		    e1 = e;
		patch(e.out, s1);
		append(e1.out, list1(&s->out1));
		s1 = s;
	    }
	    ધકેલો_2(frag(s, e1.out));
	    તોડો;
	  }

	સ્થિતિ NFA_PREV_ATOM_NO_WIDTH:
	સ્થિતિ NFA_PREV_ATOM_NO_WIDTH_NEG:
	સ્થિતિ NFA_PREV_ATOM_JUST_BEFORE:
	સ્થિતિ NFA_PREV_ATOM_JUST_BEFORE_NEG:
	સ્થિતિ NFA_PREV_ATOM_LIKE_PATTERN:
	  {
	    પૂર્ણાંક before = (*p == NFA_PREV_ATOM_JUST_BEFORE
				      || *p == NFA_PREV_ATOM_JUST_BEFORE_NEG);
	    પૂર્ણાંક pattern = (*p == NFA_PREV_ATOM_LIKE_PATTERN);
	    પૂર્ણાંક start_state;
	    પૂર્ણાંક end_state;
	    પૂર્ણાંક n = 0;
	    nfa_state_T *zend;
	    nfa_state_T *skip;

	    પસંદગી (*p)
	    {
		સ્થિતિ NFA_PREV_ATOM_NO_WIDTH:
		    start_state = NFA_START_INVISIBLE;
		    end_state = NFA_END_INVISIBLE;
		    તોડો;
		સ્થિતિ NFA_PREV_ATOM_NO_WIDTH_NEG:
		    start_state = NFA_START_INVISIBLE_NEG;
		    end_state = NFA_END_INVISIBLE_NEG;
		    તોડો;
		સ્થિતિ NFA_PREV_ATOM_JUST_BEFORE:
		    start_state = NFA_START_INVISIBLE_BEFORE;
		    end_state = NFA_END_INVISIBLE;
		    તોડો;
		સ્થિતિ NFA_PREV_ATOM_JUST_BEFORE_NEG:
		    start_state = NFA_START_INVISIBLE_BEFORE_NEG;
		    end_state = NFA_END_INVISIBLE_NEG;
		    તોડો;
		મૂળભૂત: // NFA_PREV_ATOM_LIKE_PATTERN:
		    start_state = NFA_START_PATTERN;
		    end_state = NFA_END_PATTERN;
		    તોડો;
	    }

	    જો (before)
		n = *++p; // get the count

	    // The \@= operator: match the preceding atom with zero width.
	    // The \@! operator: no match for the preceding atom.
	    // The \@<= operator: match for the preceding atom.
	    // The \@<! operator: no match for the preceding atom.
	    // Surrounds the preceding atom with START_INVISIBLE and
	    // END_INVISIBLE, similarly to MOPEN.

	    જો (nfa_calc_size == TRUE)
	    {
		nstate += pattern ? 4 : 2;
		તોડો;
	    }
	    e = કાઢો_2();
	    s1 = alloc_state(end_state, ખાલી, ખાલી);
	    જો (s1 == ખાલી)
		જાઓ theend;

	    s = alloc_state(start_state, e.start, s1);
	    જો (s == ખાલી)
		જાઓ theend;
	    જો (pattern)
	    {
		// NFA_ZEND -> NFA_END_PATTERN -> NFA_SKIP -> what follows.
		skip = alloc_state(NFA_SKIP, ખાલી, ખાલી);
		જો (skip == ખાલી)
		    જાઓ theend;
		zend = alloc_state(NFA_ZEND, s1, ખાલી);
		જો (zend == ખાલી)
		    જાઓ theend;
		s1->out= skip;
		patch(e.out, zend);
		ધકેલો_2(frag(s, list1(&skip->out)));
	    }
	    નહીંતર
	    {
		patch(e.out, s1);
		ધકેલો_2(frag(s, list1(&s1->out)));
		જો (before)
		{
		    જો (n <= 0)
			// See if we can guess the maximum width, it avoids a
			// lot of pointless tries.
			n = nfa_max_width(e.start, 0);
		    s->val = n; // store the count
		}
	    }
	    તોડો;
	  }

	સ્થિતિ NFA_COMPOSING:	// char with composing char
#જો 0
	    // TODO
	    જો (regflags & RF_ICOMBINE)
	    {
		// use the base character only
	    }
#endif
	    // FALLTHROUGH

	સ્થિતિ NFA_MOPEN:	// \( \) Submatch
	સ્થિતિ NFA_MOPEN1:
	સ્થિતિ NFA_MOPEN2:
	સ્થિતિ NFA_MOPEN3:
	સ્થિતિ NFA_MOPEN4:
	સ્થિતિ NFA_MOPEN5:
	સ્થિતિ NFA_MOPEN6:
	સ્થિતિ NFA_MOPEN7:
	સ્થિતિ NFA_MOPEN8:
	સ્થિતિ NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	સ્થિતિ NFA_ZOPEN:	// \z( \) Submatch
	સ્થિતિ NFA_ZOPEN1:
	સ્થિતિ NFA_ZOPEN2:
	સ્થિતિ NFA_ZOPEN3:
	સ્થિતિ NFA_ZOPEN4:
	સ્થિતિ NFA_ZOPEN5:
	સ્થિતિ NFA_ZOPEN6:
	સ્થિતિ NFA_ZOPEN7:
	સ્થિતિ NFA_ZOPEN8:
	સ્થિતિ NFA_ZOPEN9:
#endif
	સ્થિતિ NFA_NOPEN:	// \%( \) "Invisible Submatch"
	    જો (nfa_calc_size == TRUE)
	    {
		nstate += 2;
		તોડો;
	    }

	    mopen = *p;
	    પસંદગી (*p)
	    {
		સ્થિતિ NFA_NOPEN: mclose = NFA_NCLOSE; તોડો;
#ifdef FEAT_SYN_HL
		સ્થિતિ NFA_ZOPEN: mclose = NFA_ZCLOSE; તોડો;
		સ્થિતિ NFA_ZOPEN1: mclose = NFA_ZCLOSE1; તોડો;
		સ્થિતિ NFA_ZOPEN2: mclose = NFA_ZCLOSE2; તોડો;
		સ્થિતિ NFA_ZOPEN3: mclose = NFA_ZCLOSE3; તોડો;
		સ્થિતિ NFA_ZOPEN4: mclose = NFA_ZCLOSE4; તોડો;
		સ્થિતિ NFA_ZOPEN5: mclose = NFA_ZCLOSE5; તોડો;
		સ્થિતિ NFA_ZOPEN6: mclose = NFA_ZCLOSE6; તોડો;
		સ્થિતિ NFA_ZOPEN7: mclose = NFA_ZCLOSE7; તોડો;
		સ્થિતિ NFA_ZOPEN8: mclose = NFA_ZCLOSE8; તોડો;
		સ્થિતિ NFA_ZOPEN9: mclose = NFA_ZCLOSE9; તોડો;
#endif
		સ્થિતિ NFA_COMPOSING: mclose = NFA_END_COMPOSING; તોડો;
		મૂળભૂત:
		    // NFA_MOPEN, NFA_MOPEN1 .. NFA_MOPEN9
		    mclose = *p + NSUBEXP;
		    તોડો;
	    }

	    // Allow "NFA_MOPEN" as a valid postfix representation for
	    // the empty regexp "". In this case, the NFA will be
	    // NFA_MOPEN -> NFA_MCLOSE. Note that this also allows
	    // empty groups of parenthesis, and empty mbyte chars
	    જો (stackp == ઢગલો)
	    {
		s = alloc_state(mopen, ખાલી, ખાલી);
		જો (s == ખાલી)
		    જાઓ theend;
		s1 = alloc_state(mclose, ખાલી, ખાલી);
		જો (s1 == ખાલી)
		    જાઓ theend;
		patch(list1(&s->out), s1);
		ધકેલો_2(frag(s, list1(&s1->out)));
		તોડો;
	    }

	    // At least one node was emitted before NFA_MOPEN, so
	    // at least one node will be between NFA_MOPEN and NFA_MCLOSE
	    e = કાઢો_2();
	    s = alloc_state(mopen, e.start, ખાલી);   // `('
	    જો (s == ખાલી)
		જાઓ theend;

	    s1 = alloc_state(mclose, ખાલી, ખાલી);   // `)'
	    જો (s1 == ખાલી)
		જાઓ theend;
	    patch(e.out, s1);

	    જો (mopen == NFA_COMPOSING)
		// COMPOSING->out1 = END_COMPOSING
		patch(list1(&s->out1), s1);

	    ધકેલો_2(frag(s, list1(&s1->out)));
	    તોડો;

	સ્થિતિ NFA_BACKREF1:
	સ્થિતિ NFA_BACKREF2:
	સ્થિતિ NFA_BACKREF3:
	સ્થિતિ NFA_BACKREF4:
	સ્થિતિ NFA_BACKREF5:
	સ્થિતિ NFA_BACKREF6:
	સ્થિતિ NFA_BACKREF7:
	સ્થિતિ NFA_BACKREF8:
	સ્થિતિ NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	સ્થિતિ NFA_ZREF1:
	સ્થિતિ NFA_ZREF2:
	સ્થિતિ NFA_ZREF3:
	સ્થિતિ NFA_ZREF4:
	સ્થિતિ NFA_ZREF5:
	સ્થિતિ NFA_ZREF6:
	સ્થિતિ NFA_ZREF7:
	સ્થિતિ NFA_ZREF8:
	સ્થિતિ NFA_ZREF9:
#endif
	    જો (nfa_calc_size == TRUE)
	    {
		nstate += 2;
		તોડો;
	    }
	    s = alloc_state(*p, ખાલી, ખાલી);
	    જો (s == ખાલી)
		જાઓ theend;
	    s1 = alloc_state(NFA_SKIP, ખાલી, ખાલી);
	    જો (s1 == ખાલી)
		જાઓ theend;
	    patch(list1(&s->out), s1);
	    ધકેલો_2(frag(s, list1(&s1->out)));
	    તોડો;

	સ્થિતિ NFA_LNUM:
	સ્થિતિ NFA_LNUM_GT:
	સ્થિતિ NFA_LNUM_LT:
	સ્થિતિ NFA_VCOL:
	સ્થિતિ NFA_VCOL_GT:
	સ્થિતિ NFA_VCOL_LT:
	સ્થિતિ NFA_COL:
	સ્થિતિ NFA_COL_GT:
	સ્થિતિ NFA_COL_LT:
	સ્થિતિ NFA_MARK:
	સ્થિતિ NFA_MARK_GT:
	સ્થિતિ NFA_MARK_LT:
	  {
	    પૂર્ણાંક n = *++p; // lnum, col or mark name

	    જો (nfa_calc_size == TRUE)
	    {
		nstate += 1;
		તોડો;
	    }
	    s = alloc_state(p[-1], ખાલી, ખાલી);
	    જો (s == ખાલી)
		જાઓ theend;
	    s->val = n;
	    ધકેલો_2(frag(s, list1(&s->out)));
	    તોડો;
	  }

	સ્થિતિ NFA_ZSTART:
	સ્થિતિ NFA_ZEND:
	મૂળભૂત:
	    // Operands
	    જો (nfa_calc_size == TRUE)
	    {
		nstate++;
		તોડો;
	    }
	    s = alloc_state(*p, ખાલી, ખાલી);
	    જો (s == ખાલી)
		જાઓ theend;
	    ધકેલો_2(frag(s, list1(&s->out)));
	    તોડો;

	} // switch(*p)

    } // for (p = postfix; *p; ++p)

    જો (nfa_calc_size == TRUE)
    {
	nstate++;
	જાઓ theend;	// Return value when counting size is ignored anyway
    }

    e = કાઢો_2();
    જો (stackp != ઢગલો)
	EMSG_RET_NULL(_(e_nfa_regexp_while_converting_from_postfix_to_nfa_too_many_stats_ડાબું_on_ઢગલો));

    જો (istate >= nstate)
	EMSG_RET_NULL(_(e_nfa_regexp_not_enough_space_to_store_whole_nfa));

    matchstate = &state_ptr[istate++]; // the match state
    matchstate->c = NFA_MATCH;
    matchstate->out = matchstate->out1 = ખાલી;
    matchstate->id = 0;

    patch(e.out, matchstate);
    ret = e.start;

theend:
    પાછા ret;

#undef POP1
#undef PUSH1
#undef POP2
#undef PUSH2
#undef કાઢો_2
#undef ધકેલો_2
}

/*
 * After building the NFA program, inspect it to add optimization hints.
 */
    સ્થિર શૂન્ય
nfa_postprocess(nfa_regprog_T *prog)
{
    પૂર્ણાંક i;
    પૂર્ણાંક c;

    માટે (i = 0; i < prog->nstate; ++i)
    {
	c = prog->state[i].c;
	જો (c == NFA_START_INVISIBLE
		|| c == NFA_START_INVISIBLE_NEG
		|| c == NFA_START_INVISIBLE_BEFORE
		|| c == NFA_START_INVISIBLE_BEFORE_NEG)
	{
	    પૂર્ણાંક directly;

	    // Do it directly when what follows is possibly the end of the
	    // match.
	    જો (match_follows(prog->state[i].out1->out, 0))
		directly = TRUE;
	    નહીંતર
	    {
		પૂર્ણાંક ch_invisible = failure_chance(prog->state[i].out, 0);
		પૂર્ણાંક ch_follows = failure_chance(prog->state[i].out1->out, 0);

		// Postpone when the invisible match is expensive or has a
		// lower chance of failing.
		જો (c == NFA_START_INVISIBLE_BEFORE
		     || c == NFA_START_INVISIBLE_BEFORE_NEG)
		{
		    // "before" matches are very expensive when
		    // unbounded, always prefer what follows then,
		    // unless what follows will always match.
		    // Otherwise strongly prefer what follows.
		    જો (prog->state[i].val <= 0 && ch_follows > 0)
			directly = FALSE;
		    નહીંતર
			directly = ch_follows * 10 < ch_invisible;
		}
		નહીંતર
		{
		    // normal invisible, first do the one with the
		    // highest failure chance
		    directly = ch_follows < ch_invisible;
		}
	    }
	    જો (directly)
		// switch to the _FIRST state
		++prog->state[i].c;
	}
    }
}

/////////////////////////////////////////////////////////////////
// NFA execution code.
/////////////////////////////////////////////////////////////////

પ્રકારવ્યાખ્યા સંરચના
{
    પૂર્ણાંક	    in_use; // number of subexpr with useful info

    // When REG_MULTI is TRUE list.multi is used, otherwise list.line.
    સંઘ
    {
	સંરચના multipos
	{
	    linenr_T	start_lnum;
	    linenr_T	end_lnum;
	    colnr_T	start_col;
	    colnr_T	end_col;
	} multi[NSUBEXP];
	સંરચના linepos
	{
	    char_u	*start;
	    char_u	*end;
	} લીટી[NSUBEXP];
    } list;
    colnr_T	orig_start_col;  // list.multi[0].start_col without \zs
} regsub_T;

પ્રકારવ્યાખ્યા સંરચના
{
    regsub_T	norm; // \( .. \) matches
#ifdef FEAT_SYN_HL
    regsub_T	synt; // \z( .. \) matches
#endif
} regsubs_T;

// nfa_pim_T stores a Postponed Invisible Match.
પ્રકારવ્યાખ્યા સંરચના nfa_pim_S nfa_pim_T;
સંરચના nfa_pim_S
{
    પૂર્ણાંક		result;		// NFA_PIM_*, see below
    nfa_state_T	*state;		// the invisible match start state
    regsubs_T	subs;		// submatch info, only party used
    સંઘ
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
પ્રકારવ્યાખ્યા સંરચના
{
    nfa_state_T	*state;
    પૂર્ણાંક		ગણતરી;
    nfa_pim_T	pim;		// if pim.result != NFA_PIM_UNUSED: postponed
				// invisible match
    regsubs_T	subs;		// submatch info, only party used
} nfa_thread_T;

// nfa_list_T contains the alternative NFA execution states.
પ્રકારવ્યાખ્યા સંરચના
{
    nfa_thread_T    *t;		// allocated array of states
    પૂર્ણાંક		    n;		// nr of states currently in "t"
    પૂર્ણાંક		    len;	// max nr of states in "t"
    પૂર્ણાંક		    id;		// ID of the list
    પૂર્ણાંક		    has_pim;	// TRUE when any state has a PIM
} nfa_list_T;

#ifdef ENABLE_LOG
સ્થિર શૂન્ય log_subexpr(regsub_T *sub);

    સ્થિર શૂન્ય
log_subsexpr(regsubs_T *subs)
{
    log_subexpr(&subs->norm);
# ifdef FEAT_SYN_HL
    જો (rex.nfa_has_zsubexpr)
	log_subexpr(&subs->synt);
# endif
}

    સ્થિર શૂન્ય
log_subexpr(regsub_T *sub)
{
    પૂર્ણાંક j;

    માટે (j = 0; j < sub->in_use; j++)
	જો (REG_MULTI)
	    fprintf(log_fd,
		    "*** group %d, start: c=%d, l=%d, end: c=%d, l=%d\n",
		    j,
		    sub->list.multi[j].start_col,
		    (પૂર્ણાંક)sub->list.multi[j].start_lnum,
		    sub->list.multi[j].end_col,
		    (પૂર્ણાંક)sub->list.multi[j].end_lnum);
	નહીંતર
	{
	    અક્ષર *s = (અક્ષર *)sub->list.લીટી[j].start;
	    અક્ષર *e = (અક્ષર *)sub->list.લીટી[j].end;

	    fprintf(log_fd, "*** group %d, start: \"%s\", end: \"%s\"\n",
		    j,
		    s == ખાલી ? "NULL" : s,
		    e == ખાલી ? "NULL" : e);
	}
}

    સ્થિર અક્ષર *
pim_info(nfa_pim_T *pim)
{
    સ્થિર અક્ષર buf[30];

    જો (pim == ખાલી || pim->result == NFA_PIM_UNUSED)
	buf[0] = NUL;
    નહીંતર
    {
	sprintf(buf, " PIM col %d", REG_MULTI ? (પૂર્ણાંક)pim->end.pos.col
		: (પૂર્ણાંક)(pim->end.ptr - rex.આગત));
    }
    પાછા buf;
}

#endif

// Used during execution: whether a match has been found.
સ્થિર પૂર્ણાંક	    nfa_match;
#ifdef FEAT_RELTIME
સ્થિર પૂર્ણાંક	   *nfa_timed_out;
#endif

સ્થિર અંતઃરેખ શૂન્ય copy_sub(regsub_T *to, regsub_T *from);
સ્થિર પૂર્ણાંક pim_equal(nfa_pim_T *one, nfa_pim_T *two);

/*
 * Copy postponed invisible match info from "from" to "to".
 */
    સ્થિર શૂન્ય
copy_pim(nfa_pim_T *to, nfa_pim_T *from)
{
    to->result = from->result;
    to->state = from->state;
    copy_sub(&to->subs.norm, &from->subs.norm);
#ifdef FEAT_SYN_HL
    જો (rex.nfa_has_zsubexpr)
	copy_sub(&to->subs.synt, &from->subs.synt);
#endif
    to->end = from->end;
}

    સ્થિર શૂન્ય
clear_sub(regsub_T *sub)
{
    જો (REG_MULTI)
	// Use 0xff to set lnum to -1
	vim_memset(sub->list.multi, 0xff,
				  કદ(સંરચના multipos) * rex.nfa_nsubexpr);
    નહીંતર
	vim_memset(sub->list.લીટી, 0,
				   કદ(સંરચના linepos) * rex.nfa_nsubexpr);
    sub->in_use = 0;
}

/*
 * Copy the submatches from "from" to "to".
 */
    સ્થિર અંતઃરેખ શૂન્ય
copy_sub(regsub_T *to, regsub_T *from)
{
    to->in_use = from->in_use;
    જો (from->in_use <= 0)
	પાછા;

    // Copy the match start and end positions.  addstate() calls this for
    // every state it adds, so it is very hot.  Patterns without capturing
    // groups only use subexpression zero (the whole match), so special-case
    // "in_use == 1" with a direct assignment to avoid the mch_memmove() call.
    જો (REG_MULTI)
    {
	જો (from->in_use == 1)
	    to->list.multi[0] = from->list.multi[0];
	નહીંતર
	    mch_memmove(&to->list.multi[0],
		    &from->list.multi[0],
		    કદ(સંરચના multipos) * from->in_use);
	to->orig_start_col = from->orig_start_col;
    }
    નહીંતર
    {
	જો (from->in_use == 1)
	    to->list.લીટી[0] = from->list.લીટી[0];
	નહીંતર
	    mch_memmove(&to->list.લીટી[0],
		    &from->list.લીટી[0],
		    કદ(સંરચના linepos) * from->in_use);
    }
}

/*
 * Like copy_sub() but exclude the main match.
 */
    સ્થિર શૂન્ય
copy_sub_off(regsub_T *to, regsub_T *from)
{
    જો (to->in_use < from->in_use)
	to->in_use = from->in_use;
    જો (from->in_use <= 1)
	પાછા;

    // Copy the match start and end positions.
    જો (REG_MULTI)
	mch_memmove(&to->list.multi[1],
		&from->list.multi[1],
		કદ(સંરચના multipos) * (from->in_use - 1));
    નહીંતર
	mch_memmove(&to->list.લીટી[1],
		&from->list.લીટી[1],
		કદ(સંરચના linepos) * (from->in_use - 1));
}

/*
 * Like copy_sub() but only do the end of the main match if \ze is present.
 */
    સ્થિર શૂન્ય
copy_ze_off(regsub_T *to, regsub_T *from)
{
    જો (!rex.nfa_has_zend)
	પાછા;

    જો (REG_MULTI)
    {
	જો (from->list.multi[0].end_lnum >= 0)
	{
	    to->list.multi[0].end_lnum = from->list.multi[0].end_lnum;
	    to->list.multi[0].end_col = from->list.multi[0].end_col;
	}
    }
    નહીંતર
    {
	જો (from->list.લીટી[0].end != ખાલી)
	    to->list.લીટી[0].end = from->list.લીટી[0].end;
    }
}

/*
 * Return TRUE if "sub1" and "sub2" have the same start positions.
 * When using back-references also check the end position.
 */
    સ્થિર પૂર્ણાંક
sub_equal(regsub_T *sub1, regsub_T *sub2)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		todo;
    linenr_T	s1;
    linenr_T	s2;
    char_u	*sp1;
    char_u	*sp2;

    todo = sub1->in_use > sub2->in_use ? sub1->in_use : sub2->in_use;
    જો (REG_MULTI)
    {
	માટે (i = 0; i < todo; ++i)
	{
	    જો (i < sub1->in_use)
		s1 = sub1->list.multi[i].start_lnum;
	    નહીંતર
		s1 = -1;
	    જો (i < sub2->in_use)
		s2 = sub2->list.multi[i].start_lnum;
	    નહીંતર
		s2 = -1;
	    જો (s1 != s2)
		પાછા FALSE;
	    જો (s1 != -1 && sub1->list.multi[i].start_col
					     != sub2->list.multi[i].start_col)
		પાછા FALSE;

	    જો (rex.nfa_has_backref)
	    {
		જો (i < sub1->in_use)
		    s1 = sub1->list.multi[i].end_lnum;
		નહીંતર
		    s1 = -1;
		જો (i < sub2->in_use)
		    s2 = sub2->list.multi[i].end_lnum;
		નહીંતર
		    s2 = -1;
		જો (s1 != s2)
		    પાછા FALSE;
		જો (s1 != -1 && sub1->list.multi[i].end_col
					       != sub2->list.multi[i].end_col)
		પાછા FALSE;
	    }
	}
    }
    નહીંતર
    {
	માટે (i = 0; i < todo; ++i)
	{
	    જો (i < sub1->in_use)
		sp1 = sub1->list.લીટી[i].start;
	    નહીંતર
		sp1 = ખાલી;
	    જો (i < sub2->in_use)
		sp2 = sub2->list.લીટી[i].start;
	    નહીંતર
		sp2 = ખાલી;
	    જો (sp1 != sp2)
		પાછા FALSE;
	    જો (rex.nfa_has_backref)
	    {
		જો (i < sub1->in_use)
		    sp1 = sub1->list.લીટી[i].end;
		નહીંતર
		    sp1 = ખાલી;
		જો (i < sub2->in_use)
		    sp2 = sub2->list.લીટી[i].end;
		નહીંતર
		    sp2 = ખાલી;
		જો (sp1 != sp2)
		    પાછા FALSE;
	    }
	}
    }

    પાછા TRUE;
}

#ifdef FEAT_RELTIME
/*
 * Check if we are past the time limit, if there is one.
 */
    સ્થિર પૂર્ણાંક
nfa_did_time_out(શૂન્ય)
{
    જો (*timeout_flag)
    {
	જો (nfa_timed_out != ખાલી)
	{
# ifdef FEAT_EVAL
	    જો (!*nfa_timed_out)
		ch_log(ખાલી, "NFA regexp timed out");
# endif
	    *nfa_timed_out = TRUE;
	}
	પાછા TRUE;
    }
    પાછા FALSE;
}
#endif

#ifdef ENABLE_LOG
    સ્થિર શૂન્ય
open_debug_log(પૂર્ણાંક result)
{
    log_fd = fopen(NFA_REGEXP_RUN_LOG, "a");
    જો (log_fd == ખાલી)
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

    સ્થિર શૂન્ય
report_state(અક્ષર *action,
	     regsub_T *sub,
	     nfa_state_T *state,
	     પૂર્ણાંક lid,
	     nfa_pim_T *pim)
{
    પૂર્ણાંક col;

    જો (sub->in_use <= 0)
	col = -1;
    નહીંતર જો (REG_MULTI)
	col = sub->list.multi[0].start_col;
    નહીંતર
	col = (પૂર્ણાંક)(sub->list.લીટી[0].start - rex.લીટી);
    nfa_set_code(state->c);
    જો (log_fd == ખાલી)
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
    સ્થિર પૂર્ણાંક
has_state_with_pos(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim)	// postponed match or NULL
{
    nfa_thread_T	*thread;
    પૂર્ણાંક			i;

    માટે (i = 0; i < l->n; ++i)
    {
	thread = &l->t[i];
	જો (thread->state->id == state->id
		&& sub_equal(&thread->subs.norm, &subs->norm)
#ifdef FEAT_SYN_HL
		&& (!rex.nfa_has_zsubexpr
				|| sub_equal(&thread->subs.synt, &subs->synt))
#endif
		&& pim_equal(&thread->pim, pim))
	    પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Return TRUE if "one" and "two" are equal.  That includes when both are not
 * set.
 */
    સ્થિર પૂર્ણાંક
pim_equal(nfa_pim_T *one, nfa_pim_T *two)
{
    પૂર્ણાંક one_unused = (one == ખાલી || one->result == NFA_PIM_UNUSED);
    પૂર્ણાંક two_unused = (two == ખાલી || two->result == NFA_PIM_UNUSED);

    જો (one_unused)
	// one is unused: equal when two is also unused
	પાછા two_unused;
    જો (two_unused)
	// one is used and two is not: not equal
	પાછા FALSE;
    // compare the state id
    જો (one->state->id != two->state->id)
	પાછા FALSE;
    // compare the position
    જો (REG_MULTI)
	પાછા one->end.pos.lnum == two->end.pos.lnum
	    && one->end.pos.col == two->end.pos.col;
    પાછા one->end.ptr == two->end.ptr;
}

/*
 * Return TRUE if "state" leads to a NFA_MATCH without advancing the input.
 */
    સ્થિર પૂર્ણાંક
match_follows(nfa_state_T *startstate, પૂર્ણાંક depth)
{
    nfa_state_T	    *state = startstate;

    // avoid too much recursion
    જો (depth > 10)
	પાછા FALSE;

    જ્યારે (state != ખાલી)
    {
	પસંદગી (state->c)
	{
	    સ્થિતિ NFA_MATCH:
	    સ્થિતિ NFA_MCLOSE:
	    સ્થિતિ NFA_END_INVISIBLE:
	    સ્થિતિ NFA_END_INVISIBLE_NEG:
	    સ્થિતિ NFA_END_PATTERN:
		પાછા TRUE;

	    સ્થિતિ NFA_SPLIT:
		પાછા match_follows(state->out, depth + 1)
				     || match_follows(state->out1, depth + 1);

	    સ્થિતિ NFA_START_INVISIBLE:
	    સ્થિતિ NFA_START_INVISIBLE_FIRST:
	    સ્થિતિ NFA_START_INVISIBLE_BEFORE:
	    સ્થિતિ NFA_START_INVISIBLE_BEFORE_FIRST:
	    સ્થિતિ NFA_START_INVISIBLE_NEG:
	    સ્થિતિ NFA_START_INVISIBLE_NEG_FIRST:
	    સ્થિતિ NFA_START_INVISIBLE_BEFORE_NEG:
	    સ્થિતિ NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
	    સ્થિતિ NFA_COMPOSING:
		// skip ahead to next state
		state = state->out1->out;
		ચાલુ;

	    સ્થિતિ NFA_ANY:
	    સ્થિતિ NFA_ANY_COMPOSING:
	    સ્થિતિ NFA_IDENT:
	    સ્થિતિ NFA_SIDENT:
	    સ્થિતિ NFA_KWORD:
	    સ્થિતિ NFA_SKWORD:
	    સ્થિતિ NFA_FNAME:
	    સ્થિતિ NFA_SFNAME:
	    સ્થિતિ NFA_PRINT:
	    સ્થિતિ NFA_SPRINT:
	    સ્થિતિ NFA_WHITE:
	    સ્થિતિ NFA_NWHITE:
	    સ્થિતિ NFA_DIGIT:
	    સ્થિતિ NFA_NDIGIT:
	    સ્થિતિ NFA_HEX:
	    સ્થિતિ NFA_NHEX:
	    સ્થિતિ NFA_OCTAL:
	    સ્થિતિ NFA_NOCTAL:
	    સ્થિતિ NFA_WORD:
	    સ્થિતિ NFA_NWORD:
	    સ્થિતિ NFA_HEAD:
	    સ્થિતિ NFA_NHEAD:
	    સ્થિતિ NFA_ALPHA:
	    સ્થિતિ NFA_NALPHA:
	    સ્થિતિ NFA_LOWER:
	    સ્થિતિ NFA_NLOWER:
	    સ્થિતિ NFA_UPPER:
	    સ્થિતિ NFA_NUPPER:
	    સ્થિતિ NFA_LOWER_IC:
	    સ્થિતિ NFA_NLOWER_IC:
	    સ્થિતિ NFA_UPPER_IC:
	    સ્થિતિ NFA_NUPPER_IC:
	    સ્થિતિ NFA_START_COLL:
	    સ્થિતિ NFA_START_NEG_COLL:
	    સ્થિતિ NFA_NEWL:
		// state will advance input
		પાછા FALSE;

	    મૂળભૂત:
		જો (state->c > 0)
		    // state will advance input
		    પાછા FALSE;

		// Others: zero-width or possibly zero-width, might still find
		// a match at the same position, keep looking.
		તોડો;
	}
	state = state->out;
    }
    પાછા FALSE;
}


/*
 * Return TRUE if "state" is already in list "l".
 */
    સ્થિર પૂર્ણાંક
state_in_list(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs)	// pointers to subexpressions
{
    જો (state->lastlist[nfa_ll_index] == l->id)
    {
	જો (!rex.nfa_has_backref || has_state_with_pos(l, state, subs, ખાલી))
	    પાછા TRUE;
    }
    પાછા FALSE;
}

// Offset used for "off" by addstate_here().
#define ADDSTATE_HERE_OFFSET 10

/*
 * Add "state" and possibly what follows to state list ".".
 * Returns "subs_arg", possibly copied into temp_subs.
 * Returns NULL when recursiveness is too deep or timed out.
 */
    સ્થિર regsubs_T *
addstate(
    nfa_list_T		*l,	    // runtime state list
    nfa_state_T		*state,	    // state to update
    regsubs_T		*subs_arg,  // pointers to subexpressions
    nfa_pim_T		*pim,	    // postponed look-behind match
    પૂર્ણાંક			off_arg,    // byte offset, when -1 go to next line
    પૂર્ણાંક			depth)	    // recursion depth
{
    પૂર્ણાંક			subidx;
    પૂર્ણાંક			off = off_arg;
    પૂર્ણાંક			add_here = FALSE;
    પૂર્ણાંક			listindex = 0;
    પૂર્ણાંક			k;
    પૂર્ણાંક			found = FALSE;
    nfa_thread_T	*thread;
    સંરચના multipos	save_multipos;
    પૂર્ણાંક			save_in_use;
    char_u		*save_ptr;
    પૂર્ણાંક			i;
    regsub_T		*sub;
    regsubs_T		*subs = subs_arg;
    સ્થિર regsubs_T	temp_subs;
#ifdef ENABLE_LOG
    પૂર્ણાંક			did_print = FALSE;
#endif

#ifdef FEAT_RELTIME
    જો (nfa_did_time_out())
	પાછા ખાલી;
#endif

    // This function is called recursively.  When the depth is too much we run
    // out of stack and crash, limit recursiveness here.
    જો (depth >= 5000 || subs == ખાલી)
	પાછા ખાલી;

    જો (off_arg <= -ADDSTATE_HERE_OFFSET)
    {
	add_here = TRUE;
	off = 0;
	listindex = -(off_arg + ADDSTATE_HERE_OFFSET);
    }

    પસંદગી (state->c)
    {
	સ્થિતિ NFA_NCLOSE:
	સ્થિતિ NFA_MCLOSE:
	સ્થિતિ NFA_MCLOSE1:
	સ્થિતિ NFA_MCLOSE2:
	સ્થિતિ NFA_MCLOSE3:
	સ્થિતિ NFA_MCLOSE4:
	સ્થિતિ NFA_MCLOSE5:
	સ્થિતિ NFA_MCLOSE6:
	સ્થિતિ NFA_MCLOSE7:
	સ્થિતિ NFA_MCLOSE8:
	સ્થિતિ NFA_MCLOSE9:
#ifdef FEAT_SYN_HL
	સ્થિતિ NFA_ZCLOSE:
	સ્થિતિ NFA_ZCLOSE1:
	સ્થિતિ NFA_ZCLOSE2:
	સ્થિતિ NFA_ZCLOSE3:
	સ્થિતિ NFA_ZCLOSE4:
	સ્થિતિ NFA_ZCLOSE5:
	સ્થિતિ NFA_ZCLOSE6:
	સ્થિતિ NFA_ZCLOSE7:
	સ્થિતિ NFA_ZCLOSE8:
	સ્થિતિ NFA_ZCLOSE9:
#endif
	સ્થિતિ NFA_MOPEN:
	સ્થિતિ NFA_ZEND:
	સ્થિતિ NFA_SPLIT:
	સ્થિતિ NFA_EMPTY:
	    // These nodes are not added themselves but their "out" and/or
	    // "out1" may be added below.
	    તોડો;

	સ્થિતિ NFA_BOL:
	સ્થિતિ NFA_BOF:
	    // "^" won't match past end-of-line, don't bother trying.
	    // Except when at the end of the line, or when we are going to the
	    // next line for a look-behind match.
	    જો (rex.આગત > rex.લીટી
		    && *rex.આગત != NUL
		    && (nfa_endp == ખાલી
			|| !REG_MULTI
			|| rex.lnum == nfa_endp->se_u.pos.lnum))
		જાઓ skip_add;
	    // FALLTHROUGH

	સ્થિતિ NFA_MOPEN1:
	સ્થિતિ NFA_MOPEN2:
	સ્થિતિ NFA_MOPEN3:
	સ્થિતિ NFA_MOPEN4:
	સ્થિતિ NFA_MOPEN5:
	સ્થિતિ NFA_MOPEN6:
	સ્થિતિ NFA_MOPEN7:
	સ્થિતિ NFA_MOPEN8:
	સ્થિતિ NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	સ્થિતિ NFA_ZOPEN:
	સ્થિતિ NFA_ZOPEN1:
	સ્થિતિ NFA_ZOPEN2:
	સ્થિતિ NFA_ZOPEN3:
	સ્થિતિ NFA_ZOPEN4:
	સ્થિતિ NFA_ZOPEN5:
	સ્થિતિ NFA_ZOPEN6:
	સ્થિતિ NFA_ZOPEN7:
	સ્થિતિ NFA_ZOPEN8:
	સ્થિતિ NFA_ZOPEN9:
#endif
	સ્થિતિ NFA_NOPEN:
	સ્થિતિ NFA_ZSTART:
	    // These nodes need to be added so that we can bail out when it
	    // was added to this list before at the same position to avoid an
	    // endless loop for "\(\)*"

	મૂળભૂત:
	    જો (state->lastlist[nfa_ll_index] == l->id && state->c != NFA_SKIP)
	    {
		// This state is already in the list, don't add it again,
		// unless it is an MOPEN that is used for a backreference or
		// when there is a PIM. For NFA_MATCH check the position,
		// lower position is preferred.
		જો (!rex.nfa_has_backref && pim == ખાલી && !l->has_pim
						     && state->c != NFA_MATCH)
		{
		    // When called from addstate_here() do insert before
		    // existing states.
		    જો (add_here)
		    {
			માટે (k = 0; k < l->n && k < listindex; ++k)
			    જો (l->t[k].state->id == state->id)
			    {
				found = TRUE;
				તોડો;
			    }
		    }
		    જો (!add_here || found)
		    {
skip_add:
#ifdef ENABLE_LOG
			nfa_set_code(state->c);
			fprintf(log_fd, "> Not adding state %d to list %d. char %d: %s pim: %s has_pim: %d found: %d\n",
			    abs(state->id), l->id, state->c, code,
			    pim == ખાલી ? "NULL" : "yes", l->has_pim, found);
#endif
			પાછા subs;
		    }
		}

		// Do not add the state again when it exists with the same
		// positions.
		જો (has_state_with_pos(l, state, subs, pim))
		    જાઓ skip_add;
	    }

	    // When there are backreferences or PIMs the number of states may
	    // be (a lot) bigger than anticipated.
	    જો (l->n == l->len)
	    {
		પૂર્ણાંક		newlen = l->len * 3 / 2 + 50;
		size_t		newsize = newlen * કદ(nfa_thread_T);
		nfa_thread_T	*newt;

		જો ((long)(newsize >> 10) >= p_mmp)
		{
		    emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		    પાછા ખાલી;
		}
		જો (subs != &temp_subs)
		{
		    // "subs" may point into the current array, need to make a
		    // copy before it becomes invalid.
		    copy_sub(&temp_subs.norm, &subs->norm);
#ifdef FEAT_SYN_HL
		    જો (rex.nfa_has_zsubexpr)
			copy_sub(&temp_subs.synt, &subs->synt);
#endif
		    subs = &temp_subs;
		}

		newt = vim_realloc(l->t, newsize);
		જો (newt == ખાલી)
		{
		    // out of memory
		    પાછા ખાલી;
		}
		l->t = newt;
		l->len = newlen;
	    }

	    // add the state to the list
	    state->lastlist[nfa_ll_index] = l->id;
	    thread = &l->t[l->n++];
	    thread->state = state;
	    જો (pim == ખાલી)
		thread->pim.result = NFA_PIM_UNUSED;
	    નહીંતર
	    {
		copy_pim(&thread->pim, pim);
		l->has_pim = TRUE;
	    }
	    copy_sub(&thread->subs.norm, &subs->norm);
#ifdef FEAT_SYN_HL
	    જો (rex.nfa_has_zsubexpr)
		copy_sub(&thread->subs.synt, &subs->synt);
#endif
#ifdef ENABLE_LOG
	    report_state("Adding", &thread->subs.norm, state, l->id, pim);
	    did_print = TRUE;
#endif
    }

#ifdef ENABLE_LOG
    જો (!did_print)
	report_state("Processing", &subs->norm, state, l->id, pim);
#endif
    પસંદગી (state->c)
    {
	સ્થિતિ NFA_MATCH:
	    તોડો;

	સ્થિતિ NFA_SPLIT:
	    // order matters here
	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    subs = addstate(l, state->out1, subs, pim, off_arg, depth + 1);
	    તોડો;

	સ્થિતિ NFA_EMPTY:
	સ્થિતિ NFA_NOPEN:
	સ્થિતિ NFA_NCLOSE:
	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    તોડો;

	સ્થિતિ NFA_MOPEN:
	સ્થિતિ NFA_MOPEN1:
	સ્થિતિ NFA_MOPEN2:
	સ્થિતિ NFA_MOPEN3:
	સ્થિતિ NFA_MOPEN4:
	સ્થિતિ NFA_MOPEN5:
	સ્થિતિ NFA_MOPEN6:
	સ્થિતિ NFA_MOPEN7:
	સ્થિતિ NFA_MOPEN8:
	સ્થિતિ NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	સ્થિતિ NFA_ZOPEN:
	સ્થિતિ NFA_ZOPEN1:
	સ્થિતિ NFA_ZOPEN2:
	સ્થિતિ NFA_ZOPEN3:
	સ્થિતિ NFA_ZOPEN4:
	સ્થિતિ NFA_ZOPEN5:
	સ્થિતિ NFA_ZOPEN6:
	સ્થિતિ NFA_ZOPEN7:
	સ્થિતિ NFA_ZOPEN8:
	સ્થિતિ NFA_ZOPEN9:
#endif
	સ્થિતિ NFA_ZSTART:
	    જો (state->c == NFA_ZSTART)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#ifdef FEAT_SYN_HL
	    નહીંતર જો (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
	    {
		subidx = state->c - NFA_ZOPEN;
		sub = &subs->synt;
	    }
#endif
	    નહીંતર
	    {
		subidx = state->c - NFA_MOPEN;
		sub = &subs->norm;
	    }

	    // avoid compiler warnings
	    save_ptr = ખાલી;
	    CLEAR_FIELD(save_multipos);

	    // Set the position (with "off" added) in the subexpression.  Save
	    // and restore it when it was in use.  Otherwise fill any gap.
	    જો (REG_MULTI)
	    {
		જો (subidx < sub->in_use)
		{
		    save_multipos = sub->list.multi[subidx];
		    save_in_use = -1;
		}
		નહીંતર
		{
		    save_in_use = sub->in_use;
		    માટે (i = sub->in_use; i < subidx; ++i)
		    {
			sub->list.multi[i].start_lnum = -1;
			sub->list.multi[i].end_lnum = -1;
		    }
		    sub->in_use = subidx + 1;
		}
		જો (off == -1)
		{
		    sub->list.multi[subidx].start_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].start_col = 0;
		}
		નહીંતર
		{
		    sub->list.multi[subidx].start_lnum = rex.lnum;
		    sub->list.multi[subidx].start_col =
					 (colnr_T)(rex.આગત - rex.લીટી + off);
		}
		sub->list.multi[subidx].end_lnum = -1;
	    }
	    નહીંતર
	    {
		જો (subidx < sub->in_use)
		{
		    save_ptr = sub->list.લીટી[subidx].start;
		    save_in_use = -1;
		}
		નહીંતર
		{
		    save_in_use = sub->in_use;
		    માટે (i = sub->in_use; i < subidx; ++i)
		    {
			sub->list.લીટી[i].start = ખાલી;
			sub->list.લીટી[i].end = ખાલી;
		    }
		    sub->in_use = subidx + 1;
		}
		sub->list.લીટી[subidx].start = rex.આગત + off;
	    }

	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    જો (subs == ખાલી)
		તોડો;
	    // "subs" may have changed, need to set "sub" again
#ifdef FEAT_SYN_HL
	    જો (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
		sub = &subs->synt;
	    નહીંતર
#endif
		sub = &subs->norm;

	    જો (save_in_use == -1)
	    {
		જો (REG_MULTI)
		    sub->list.multi[subidx] = save_multipos;
		નહીંતર
		    sub->list.લીટી[subidx].start = save_ptr;
	    }
	    નહીંતર
		sub->in_use = save_in_use;
	    તોડો;

	સ્થિતિ NFA_MCLOSE:
	    જો (rex.nfa_has_zend && (REG_MULTI
			? subs->norm.list.multi[0].end_lnum >= 0
			: subs->norm.list.લીટી[0].end != ખાલી))
	    {
		// Do not overwrite the position set by \ze.
		subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
		તોડો;
	    }
	    // FALLTHROUGH
	સ્થિતિ NFA_MCLOSE1:
	સ્થિતિ NFA_MCLOSE2:
	સ્થિતિ NFA_MCLOSE3:
	સ્થિતિ NFA_MCLOSE4:
	સ્થિતિ NFA_MCLOSE5:
	સ્થિતિ NFA_MCLOSE6:
	સ્થિતિ NFA_MCLOSE7:
	સ્થિતિ NFA_MCLOSE8:
	સ્થિતિ NFA_MCLOSE9:
#ifdef FEAT_SYN_HL
	સ્થિતિ NFA_ZCLOSE:
	સ્થિતિ NFA_ZCLOSE1:
	સ્થિતિ NFA_ZCLOSE2:
	સ્થિતિ NFA_ZCLOSE3:
	સ્થિતિ NFA_ZCLOSE4:
	સ્થિતિ NFA_ZCLOSE5:
	સ્થિતિ NFA_ZCLOSE6:
	સ્થિતિ NFA_ZCLOSE7:
	સ્થિતિ NFA_ZCLOSE8:
	સ્થિતિ NFA_ZCLOSE9:
#endif
	સ્થિતિ NFA_ZEND:
	    જો (state->c == NFA_ZEND)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#ifdef FEAT_SYN_HL
	    નહીંતર જો (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
	    {
		subidx = state->c - NFA_ZCLOSE;
		sub = &subs->synt;
	    }
#endif
	    નહીંતર
	    {
		subidx = state->c - NFA_MCLOSE;
		sub = &subs->norm;
	    }

	    // We don't fill in gaps here, there must have been an MOPEN that
	    // has done that.
	    save_in_use = sub->in_use;
	    જો (sub->in_use <= subidx)
		sub->in_use = subidx + 1;
	    જો (REG_MULTI)
	    {
		save_multipos = sub->list.multi[subidx];
		જો (off == -1)
		{
		    sub->list.multi[subidx].end_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].end_col = 0;
		}
		નહીંતર
		{
		    sub->list.multi[subidx].end_lnum = rex.lnum;
		    sub->list.multi[subidx].end_col =
					  (colnr_T)(rex.આગત - rex.લીટી + off);
		}
		// avoid compiler warnings
		save_ptr = ખાલી;
	    }
	    નહીંતર
	    {
		save_ptr = sub->list.લીટી[subidx].end;
		sub->list.લીટી[subidx].end = rex.આગત + off;
		// avoid compiler warnings
		CLEAR_FIELD(save_multipos);
	    }

	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    જો (subs == ખાલી)
		તોડો;
	    // "subs" may have changed, need to set "sub" again
#ifdef FEAT_SYN_HL
	    જો (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
		sub = &subs->synt;
	    નહીંતર
#endif
		sub = &subs->norm;

	    જો (REG_MULTI)
		sub->list.multi[subidx] = save_multipos;
	    નહીંતર
		sub->list.લીટી[subidx].end = save_ptr;
	    sub->in_use = save_in_use;
	    તોડો;
    }
    પાછા subs;
}

/*
 * Like addstate(), but the new state(s) are put at position "*ip".
 * Used for zero-width matches, next state to use is the added one.
 * This makes sure the order of states to be tried does not change, which
 * matters for alternatives.
 */
    સ્થિર regsubs_T *
addstate_here(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim,   // postponed look-behind match
    પૂર્ણાંક			*ip)
{
    પૂર્ણાંક tlen = l->n;
    પૂર્ણાંક ગણતરી;
    પૂર્ણાંક listidx = *ip;
    regsubs_T *r;

    // First add the state(s) at the end, so that we know how many there are.
    // Pass the listidx as offset (avoids adding another argument to
    // addstate()).
    r = addstate(l, state, subs, pim, -listidx - ADDSTATE_HERE_OFFSET, 0);
    જો (r == ખાલી)
	પાછા ખાલી;

    // when "*ip" was at the end of the list, nothing to do
    જો (listidx + 1 == tlen)
	પાછા r;

    // re-order to put the new state at the current position
    ગણતરી = l->n - tlen;
    જો (ગણતરી == 0)
	પાછા r; // no state got added
    જો (ગણતરી == 1)
    {
	// overwrite the current state
	l->t[listidx] = l->t[l->n - 1];
    }
    નહીંતર જો (ગણતરી > 1)
    {
	જો (l->n + ગણતરી - 1 >= l->len)
	{
	    // not enough space to move the new states, reallocate the list
	    // and move the states to the right position
	    પૂર્ણાંક		    newlen = l->len * 3 / 2 + 50;
	    size_t	    newsize = newlen * કદ(nfa_thread_T);
	    nfa_thread_T    *newl;

	    જો ((long)(newsize >> 10) >= p_mmp)
	    {
		emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		પાછા ખાલી;
	    }
	    newl = alloc(newsize);
	    જો (newl == ખાલી)
		પાછા ખાલી;
	    l->len = newlen;
	    mch_memmove(&(newl[0]),
		    &(l->t[0]),
		    કદ(nfa_thread_T) * listidx);
	    mch_memmove(&(newl[listidx]),
		    &(l->t[l->n - ગણતરી]),
		    કદ(nfa_thread_T) * ગણતરી);
	    mch_memmove(&(newl[listidx + ગણતરી]),
		    &(l->t[listidx + 1]),
		    કદ(nfa_thread_T) * (l->n - ગણતરી - listidx - 1));
	    vim_free(l->t);
	    l->t = newl;
	}
	નહીંતર
	{
	    // make space for new states, then move them from the
	    // end to the current position
	    mch_memmove(&(l->t[listidx + ગણતરી]),
		    &(l->t[listidx + 1]),
		    કદ(nfa_thread_T) * (l->n - listidx - 1));
	    mch_memmove(&(l->t[listidx]),
		    &(l->t[l->n - 1]),
		    કદ(nfa_thread_T) * ગણતરી);
	}
    }
    --l->n;
    *ip = listidx - 1;

    પાછા r;
}

/*
 * Check character class "class" against current character c.
 */
    સ્થિર પૂર્ણાંક
check_char_class(પૂર્ણાંક class, પૂર્ણાંક c)
{
    પસંદગી (class)
    {
	સ્થિતિ NFA_CLASS_ALNUM:
	    જો (c >= 1 && c < 128 && isalnum(c))
		પાછા OK;
	    તોડો;
	સ્થિતિ NFA_CLASS_ALPHA:
	    જો (c >= 1 && c < 128 && isalpha(c))
		પાછા OK;
	    તોડો;
	સ્થિતિ NFA_CLASS_BLANK:
	    જો (c == ' ' || c == '\t')
		પાછા OK;
	    તોડો;
	સ્થિતિ NFA_CLASS_CNTRL:
	    જો (c >= 1 && c <= 127 && iscntrl(c))
		પાછા OK;
	    તોડો;
	સ્થિતિ NFA_CLASS_DIGIT:
	    જો (VIM_ISDIGIT(c))
		પાછા OK;
	    તોડો;
	સ્થિતિ NFA_CLASS_GRAPH:
	    જો (c >= 1 && c <= 127 && isgraph(c))
		પાછા OK;
	    તોડો;
	સ્થિતિ NFA_CLASS_LOWER:
	    જો (MB_ISLOWER(c) && c != 170 && c != 186)
		પાછા OK;
	    તોડો;
	સ્થિતિ NFA_CLASS_PRINT:
	    જો (vim_isprintc(c))
		પાછા OK;
	    તોડો;
	સ્થિતિ NFA_CLASS_PUNCT:
	    જો (c >= 1 && c < 128 && ispunct(c))
		પાછા OK;
	    તોડો;
	સ્થિતિ NFA_CLASS_SPACE:
	    જો ((c >= 9 && c <= 13) || (c == ' '))
		પાછા OK;
	    તોડો;
	સ્થિતિ NFA_CLASS_UPPER:
	    જો (MB_ISUPPER(c))
		પાછા OK;
	    તોડો;
	સ્થિતિ NFA_CLASS_XDIGIT:
	    જો (vim_isxdigit(c))
		પાછા OK;
	    તોડો;
	સ્થિતિ NFA_CLASS_TAB:
	    જો (c == '\t')
		પાછા OK;
	    તોડો;
	સ્થિતિ NFA_CLASS_RETURN:
	    જો (c == '\r')
		પાછા OK;
	    તોડો;
	સ્થિતિ NFA_CLASS_BACKSPACE:
	    જો (c == '\b')
		પાછા OK;
	    તોડો;
	સ્થિતિ NFA_CLASS_ESCAPE:
	    જો (c == '\033')
		પાછા OK;
	    તોડો;
	સ્થિતિ NFA_CLASS_IDENT:
	    જો (vim_isIDc(c))
		પાછા OK;
	    તોડો;
	સ્થિતિ NFA_CLASS_KEYWORD:
	    જો (reg_iswordc(c))
		પાછા OK;
	    તોડો;
	સ્થિતિ NFA_CLASS_FNAME:
	    જો (vim_isfilec(c))
		પાછા OK;
	    તોડો;

	મૂળભૂત:
	    // should not be here :P
	    siemsg(_(e_nfa_regexp_invalid_character_class_nr), class);
	    પાછા FAIL;
    }
    પાછા FAIL;
}

/*
 * Check for a match with subexpression "subidx".
 * Return TRUE if it matches.
 */
    સ્થિર પૂર્ણાંક
match_backref(
    regsub_T	*sub,	    // pointers to subexpressions
    પૂર્ણાંક		subidx,
    પૂર્ણાંક		*bytelen)   // out: length of match in bytes
{
    પૂર્ણાંક		len;

    જો (sub->in_use <= subidx)
    {
retempty:
	// backref was not set, match an empty string
	*bytelen = 0;
	પાછા TRUE;
    }

    જો (REG_MULTI)
    {
	જો (sub->list.multi[subidx].start_lnum < 0
				       || sub->list.multi[subidx].end_lnum < 0)
	    જાઓ retempty;
	જો (sub->list.multi[subidx].start_lnum == rex.lnum
			       && sub->list.multi[subidx].end_lnum == rex.lnum)
	{
	    len = sub->list.multi[subidx].end_col
					  - sub->list.multi[subidx].start_col;
	    જો (cstrncmp(rex.લીટી + sub->list.multi[subidx].start_col,
							 rex.આગત, &len) == 0)
	    {
		*bytelen = len;
		પાછા TRUE;
	    }
	}
	નહીંતર
	{
	    જો (match_with_backref(
			sub->list.multi[subidx].start_lnum,
			sub->list.multi[subidx].start_col,
			sub->list.multi[subidx].end_lnum,
			sub->list.multi[subidx].end_col,
			bytelen) == RA_MATCH)
		પાછા TRUE;
	}
    }
    નહીંતર
    {
	જો (sub->list.લીટી[subidx].start == ખાલી
					|| sub->list.લીટી[subidx].end == ખાલી)
	    જાઓ retempty;
	len = (પૂર્ણાંક)(sub->list.લીટી[subidx].end - sub->list.લીટી[subidx].start);
	જો (cstrncmp(sub->list.લીટી[subidx].start, rex.આગત, &len) == 0)
	{
	    *bytelen = len;
	    પાછા TRUE;
	}
    }
    પાછા FALSE;
}

#ifdef FEAT_SYN_HL

/*
 * Check for a match with \z subexpression "subidx".
 * Return TRUE if it matches.
 */
    સ્થિર પૂર્ણાંક
match_zref(
    પૂર્ણાંક		subidx,
    પૂર્ણાંક		*bytelen)   // out: length of match in bytes
{
    પૂર્ણાંક		len;

    cleanup_zsubexpr();
    જો (re_extmatch_in == ખાલી || re_extmatch_in->matches[subidx] == ખાલી)
    {
	// backref was not set, match an empty string
	*bytelen = 0;
	પાછા TRUE;
    }

    len = (પૂર્ણાંક)STRLEN(re_extmatch_in->matches[subidx]);
    જો (cstrncmp(re_extmatch_in->matches[subidx], rex.આગત, &len) == 0)
    {
	*bytelen = len;
	પાછા TRUE;
    }
    પાછા FALSE;
}
#endif

/*
 * Save list IDs for all NFA states of "prog" into "list".
 * Also reset the IDs to zero.
 * Only used for the recursive value lastlist[1].
 */
    સ્થિર શૂન્ય
nfa_save_listids(nfa_regprog_T *prog, પૂર્ણાંક *list)
{
    પૂર્ણાંક		    i;
    nfa_state_T	    *p;

    // Order in the list is reverse, it's a bit faster that way.
    p = &prog->state[0];
    માટે (i = prog->nstate; --i >= 0; )
    {
	list[i] = p->lastlist[1];
	p->lastlist[1] = 0;
	++p;
    }
}

/*
 * Restore list IDs from "list" to all NFA states.
 */
    સ્થિર શૂન્ય
nfa_restore_listids(nfa_regprog_T *prog, પૂર્ણાંક *list)
{
    પૂર્ણાંક		    i;
    nfa_state_T	    *p;

    p = &prog->state[0];
    માટે (i = prog->nstate; --i >= 0; )
    {
	p->lastlist[1] = list[i];
	++p;
    }
}

    સ્થિર પૂર્ણાંક
nfa_re_અંક_cmp(long_u val, પૂર્ણાંક op, long_u pos)
{
    જો (op == 1) પાછા pos > val;
    જો (op == 2) પાછા pos < val;
    પાછા val == pos;
}

સ્થિર પૂર્ણાંક nfa_regmatch(nfa_regprog_T *prog, nfa_state_T *start, regsubs_T *submatch, regsubs_T *m);

/*
 * Recursively call nfa_regmatch()
 * "pim" is NULL or contains info about a Postponed Invisible Match (start
 * position).
 */
    સ્થિર પૂર્ણાંક
recursive_regmatch(
    nfa_state_T	    *state,
    nfa_pim_T	    *pim,
    nfa_regprog_T   *prog,
    regsubs_T	    *submatch,
    regsubs_T	    *m,
    પૂર્ણાંક		    **listids,
    પૂર્ણાંક		    *listids_len)
{
    પૂર્ણાંક		save_reginput_col = (પૂર્ણાંક)(rex.આગત - rex.લીટી);
    પૂર્ણાંક		save_reglnum = rex.lnum;
    પૂર્ણાંક		save_nfa_match = nfa_match;
    પૂર્ણાંક		save_nfa_listid = rex.nfa_listid;
    save_se_T   *save_nfa_endp = nfa_endp;
    save_se_T   endpos;
    save_se_T   *endposp = ખાલી;
    પૂર્ણાંક		result;
    પૂર્ણાંક		need_restore = FALSE;

    જો (pim != ખાલી)
    {
	// start at the position where the postponed match was
	જો (REG_MULTI)
	    rex.આગત = rex.લીટી + pim->end.pos.col;
	નહીંતર
	    rex.આગત = pim->end.ptr;
    }

    જો (state->c == NFA_START_INVISIBLE_BEFORE
	    || state->c == NFA_START_INVISIBLE_BEFORE_FIRST
	    || state->c == NFA_START_INVISIBLE_BEFORE_NEG
	    || state->c == NFA_START_INVISIBLE_BEFORE_NEG_FIRST)
    {
	// The recursive match must end at the current position. When "pim" is
	// not NULL it specifies the current position.
	endposp = &endpos;
	જો (REG_MULTI)
	{
	    જો (pim == ખાલી)
	    {
		endpos.se_u.pos.col = (પૂર્ણાંક)(rex.આગત - rex.લીટી);
		endpos.se_u.pos.lnum = rex.lnum;
	    }
	    નહીંતર
		endpos.se_u.pos = pim->end.pos;
	}
	નહીંતર
	{
	    જો (pim == ખાલી)
		endpos.se_u.ptr = rex.આગત;
	    નહીંતર
		endpos.se_u.ptr = pim->end.ptr;
	}

	// Go back the specified number of bytes, or as far as the
	// start of the previous line, to try matching "\@<=" or
	// not matching "\@<!". This is very inefficient, limit the number of
	// bytes if possible.
	જો (state->val <= 0)
	{
	    જો (REG_MULTI)
	    {
		rex.લીટી = reg_getline(--rex.lnum);
		જો (rex.લીટી == ખાલી)
		    // can't go before the first line
		    rex.લીટી = reg_getline(++rex.lnum);
	    }
	    rex.આગત = rex.લીટી;
	}
	નહીંતર
	{
	    જો (REG_MULTI && (પૂર્ણાંક)(rex.આગત - rex.લીટી) < state->val)
	    {
		// Not enough bytes in this line, go to end of
		// previous line.
		rex.લીટી = reg_getline(--rex.lnum);
		જો (rex.લીટી == ખાલી)
		{
		    // can't go before the first line
		    rex.લીટી = reg_getline(++rex.lnum);
		    rex.આગત = rex.લીટી;
		}
		નહીંતર
		    rex.આગત = rex.લીટી + reg_getline_len(rex.lnum);
	    }
	    જો ((પૂર્ણાંક)(rex.આગત - rex.લીટી) >= state->val)
	    {
		rex.આગત -= state->val;
		જો (has_mbyte)
		    rex.આગત -= mb_head_off(rex.લીટી, rex.આગત);
	    }
	    નહીંતર
		rex.આગત = rex.લીટી;
	}
    }

#ifdef ENABLE_LOG
    જો (log_fd != stderr)
	fclose(log_fd);
    log_fd = ખાલી;
#endif
    // Have to clear the lastlist field of the NFA nodes, so that
    // nfa_regmatch() and addstate() can run properly after recursion.
    જો (nfa_ll_index == 1)
    {
	// Already calling nfa_regmatch() recursively.  Save the lastlist[1]
	// values and clear them.
	જો (*listids == ખાલી || *listids_len < prog->nstate)
	{
	    vim_free(*listids);
	    *listids = ALLOC_MULT(પૂર્ણાંક, prog->nstate);
	    જો (*listids == ખાલી)
	    {
		emsg(_(e_nfa_regexp_could_not_allocate_memory_for_branch_traversal));
		પાછા 0;
	    }
	    *listids_len = prog->nstate;
	}
	nfa_save_listids(prog, *listids);
	need_restore = TRUE;
	// any value of rex.nfa_listid will do
    }
    નહીંતર
    {
	// First recursive nfa_regmatch() call, switch to the second lastlist
	// entry.  Make sure rex.nfa_listid is different from a previous
	// recursive call, because some states may still have this ID.
	++nfa_ll_index;
	જો (rex.nfa_listid <= rex.nfa_alt_listid)
	    rex.nfa_listid = rex.nfa_alt_listid;
    }

    // Call nfa_regmatch() to check if the current concat matches at this
    // position. The concat ends with the node NFA_END_INVISIBLE
    nfa_endp = endposp;
    result = nfa_regmatch(prog, state->out, submatch, m);

    જો (need_restore)
	nfa_restore_listids(prog, *listids);
    નહીંતર
    {
	--nfa_ll_index;
	rex.nfa_alt_listid = rex.nfa_listid;
    }

    // restore position in input text
    rex.lnum = save_reglnum;
    જો (REG_MULTI)
	rex.લીટી = reg_getline(rex.lnum);
    rex.આગત = rex.લીટી + save_reginput_col;
    જો (result != NFA_TOO_EXPENSIVE)
    {
	nfa_match = save_nfa_match;
	rex.nfa_listid = save_nfa_listid;
    }
    nfa_endp = save_nfa_endp;

#ifdef ENABLE_LOG
    open_debug_log(result);
#endif

    પાછા result;
}

/*
 * Estimate the chance of a match with "state" failing.
 * empty match: 0
 * NFA_ANY: 1
 * specific character: 99
 */
    સ્થિર પૂર્ણાંક
failure_chance(nfa_state_T *state, પૂર્ણાંક depth)
{
    પૂર્ણાંક c = state->c;
    પૂર્ણાંક l, r;

    // detect looping
    જો (depth > 4)
	પાછા 1;

    પસંદગી (c)
    {
	સ્થિતિ NFA_SPLIT:
	    જો (state->out->c == NFA_SPLIT || state->out1->c == NFA_SPLIT)
		// avoid recursive stuff
		પાછા 1;
	    // two alternatives, use the lowest failure chance
	    l = failure_chance(state->out, depth + 1);
	    r = failure_chance(state->out1, depth + 1);
	    પાછા l < r ? l : r;

	સ્થિતિ NFA_ANY:
	    // matches anything, unlikely to fail
	    પાછા 1;

	સ્થિતિ NFA_MATCH:
	સ્થિતિ NFA_MCLOSE:
	સ્થિતિ NFA_ANY_COMPOSING:
	    // empty match works always
	    પાછા 0;

	સ્થિતિ NFA_START_INVISIBLE:
	સ્થિતિ NFA_START_INVISIBLE_FIRST:
	સ્થિતિ NFA_START_INVISIBLE_NEG:
	સ્થિતિ NFA_START_INVISIBLE_NEG_FIRST:
	સ્થિતિ NFA_START_INVISIBLE_BEFORE:
	સ્થિતિ NFA_START_INVISIBLE_BEFORE_FIRST:
	સ્થિતિ NFA_START_INVISIBLE_BEFORE_NEG:
	સ્થિતિ NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
	સ્થિતિ NFA_START_PATTERN:
	    // recursive regmatch is expensive, use low failure chance
	    પાછા 5;

	સ્થિતિ NFA_BOL:
	સ્થિતિ NFA_EOL:
	સ્થિતિ NFA_BOF:
	સ્થિતિ NFA_EOF:
	સ્થિતિ NFA_NEWL:
	    પાછા 99;

	સ્થિતિ NFA_BOW:
	સ્થિતિ NFA_EOW:
	    પાછા 90;

	સ્થિતિ NFA_MOPEN:
	સ્થિતિ NFA_MOPEN1:
	સ્થિતિ NFA_MOPEN2:
	સ્થિતિ NFA_MOPEN3:
	સ્થિતિ NFA_MOPEN4:
	સ્થિતિ NFA_MOPEN5:
	સ્થિતિ NFA_MOPEN6:
	સ્થિતિ NFA_MOPEN7:
	સ્થિતિ NFA_MOPEN8:
	સ્થિતિ NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	સ્થિતિ NFA_ZOPEN:
	સ્થિતિ NFA_ZOPEN1:
	સ્થિતિ NFA_ZOPEN2:
	સ્થિતિ NFA_ZOPEN3:
	સ્થિતિ NFA_ZOPEN4:
	સ્થિતિ NFA_ZOPEN5:
	સ્થિતિ NFA_ZOPEN6:
	સ્થિતિ NFA_ZOPEN7:
	સ્થિતિ NFA_ZOPEN8:
	સ્થિતિ NFA_ZOPEN9:
	સ્થિતિ NFA_ZCLOSE:
	સ્થિતિ NFA_ZCLOSE1:
	સ્થિતિ NFA_ZCLOSE2:
	સ્થિતિ NFA_ZCLOSE3:
	સ્થિતિ NFA_ZCLOSE4:
	સ્થિતિ NFA_ZCLOSE5:
	સ્થિતિ NFA_ZCLOSE6:
	સ્થિતિ NFA_ZCLOSE7:
	સ્થિતિ NFA_ZCLOSE8:
	સ્થિતિ NFA_ZCLOSE9:
#endif
	સ્થિતિ NFA_NOPEN:
	સ્થિતિ NFA_MCLOSE1:
	સ્થિતિ NFA_MCLOSE2:
	સ્થિતિ NFA_MCLOSE3:
	સ્થિતિ NFA_MCLOSE4:
	સ્થિતિ NFA_MCLOSE5:
	સ્થિતિ NFA_MCLOSE6:
	સ્થિતિ NFA_MCLOSE7:
	સ્થિતિ NFA_MCLOSE8:
	સ્થિતિ NFA_MCLOSE9:
	સ્થિતિ NFA_NCLOSE:
	    પાછા failure_chance(state->out, depth + 1);

	સ્થિતિ NFA_BACKREF1:
	સ્થિતિ NFA_BACKREF2:
	સ્થિતિ NFA_BACKREF3:
	સ્થિતિ NFA_BACKREF4:
	સ્થિતિ NFA_BACKREF5:
	સ્થિતિ NFA_BACKREF6:
	સ્થિતિ NFA_BACKREF7:
	સ્થિતિ NFA_BACKREF8:
	સ્થિતિ NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	સ્થિતિ NFA_ZREF1:
	સ્થિતિ NFA_ZREF2:
	સ્થિતિ NFA_ZREF3:
	સ્થિતિ NFA_ZREF4:
	સ્થિતિ NFA_ZREF5:
	સ્થિતિ NFA_ZREF6:
	સ્થિતિ NFA_ZREF7:
	સ્થિતિ NFA_ZREF8:
	સ્થિતિ NFA_ZREF9:
#endif
	    // backreferences don't match in many places
	    પાછા 94;

	સ્થિતિ NFA_LNUM_GT:
	સ્થિતિ NFA_LNUM_LT:
	સ્થિતિ NFA_COL_GT:
	સ્થિતિ NFA_COL_LT:
	સ્થિતિ NFA_VCOL_GT:
	સ્થિતિ NFA_VCOL_LT:
	સ્થિતિ NFA_MARK_GT:
	સ્થિતિ NFA_MARK_LT:
	સ્થિતિ NFA_VISUAL:
	    // before/after positions don't match very often
	    પાછા 85;

	સ્થિતિ NFA_LNUM:
	    પાછા 90;

	સ્થિતિ NFA_CURSOR:
	સ્થિતિ NFA_COL:
	સ્થિતિ NFA_VCOL:
	સ્થિતિ NFA_MARK:
	    // specific positions rarely match
	    પાછા 98;

	સ્થિતિ NFA_COMPOSING:
	    પાછા 95;

	મૂળભૂત:
	    જો (c > 0)
		// character match fails often
		પાછા 95;
    }

    // something else, includes character classes
    પાછા 50;
}

/*
 * Skip until the char "c" we know a match must start with.
 */
    સ્થિર પૂર્ણાંક
skip_to_start(પૂર્ણાંક c, colnr_T *colp)
{
    char_u *s;

    // Used often, do some work to avoid call overhead.
    જો (!rex.reg_ic && !has_mbyte)
	s = vim_strbyte(rex.લીટી + *colp, c);
    નહીંતર
	s = cstrchr(rex.લીટી + *colp, c);
    જો (s == ખાલી)
	પાછા FAIL;
    *colp = (પૂર્ણાંક)(s - rex.લીટી);
    પાછા OK;
}

/*
 * Check for a match with match_text.
 * Called after skip_to_start() has found regstart.
 * Returns zero for no match, 1 for a match.
 */
    સ્થિર long
find_match_text(colnr_T *startcol, પૂર્ણાંક regstart, char_u *match_text)
{
    colnr_T col = *startcol;
    પૂર્ણાંક	    c1, c2;
    પૂર્ણાંક	    len1, len2;
    પૂર્ણાંક	    match;

    માટે (;;)
    {
	match = TRUE;
	// Skip the input character: case-folding can change its byte length.
	len2 = MB_CPTR2LEN(rex.લીટી + col);
	માટે (len1 = 0; match_text[len1] != NUL; len1 += MB_CHAR2LEN(c1))
	{
	    c1 = PTR2CHAR(match_text + len1);
	    c2 = PTR2CHAR(rex.લીટી + col + len2);
	    જો (c1 != c2 && (!rex.reg_ic || MB_CASEFOLD(c1) != MB_CASEFOLD(c2)))
	    {
		match = FALSE;
		તોડો;
	    }
	    len2 += enc_utf8 ? utf_ptr2len(rex.લીટી + col + len2)
							     : MB_CHAR2LEN(c2);
	}
	જો (match
		// check that no composing char follows
		&& !(enc_utf8
			  && utf_iscomposing(PTR2CHAR(rex.લીટી + col + len2))))
	{
	    cleanup_subexpr();
	    જો (REG_MULTI)
	    {
		rex.reg_startpos[0].lnum = rex.lnum;
		rex.reg_startpos[0].col = col;
		rex.reg_endpos[0].lnum = rex.lnum;
		rex.reg_endpos[0].col = col + len2;
	    }
	    નહીંતર
	    {
		rex.reg_startp[0] = rex.લીટી + col;
		rex.reg_endp[0] = rex.લીટી + col + len2;
	    }
	    *startcol = col;
	    પાછા 1L;
	}

	// Try finding regstart after the current match.
	col += MB_CPTR2LEN(rex.લીટી + col); // skip the input character
	જો (skip_to_start(regstart, &col) == FAIL)
	    તોડો;
    }

    *startcol = col;
    પાછા 0L;
}

/*
 * Check whether the composing characters at the current input position match
 * the NFA composing sub-expression that starts at "sta" (the first state
 * after NFA_COMPOSING).  "curc" is the base character and "clen" its byte
 * length.
 * Returns OK when it matches, FAIL otherwise.
 */
    સ્થિર પૂર્ણાંક
match_composing(nfa_state_T *sta, પૂર્ણાંક curc, પૂર્ણાંક clen)
{
    પૂર્ણાંક		mc = curc;
    પૂર્ણાંક		len = 0;
    પૂર્ણાંક		cchars[MAX_MCO];
    પૂર્ણાંક		ccount = 0;
    પૂર્ણાંક		j;

    જો (utf_iscomposing(sta->c))
	// Only match composing character(s), ignore base character.  Used
	// for ".{composing}" and "{composing}" (no preceding character).
	len += mb_char2len(mc);

    જો (rex.reg_icombine && len == 0)
	// If \Z was present, then ignore composing characters.  When
	// ignoring the base character this always matches.
	પાછા sta->c == curc ? OK : FAIL;

    // Check base character matches first, unless ignored.
    જો (len == 0 && mc != sta->c)
	પાછા FAIL;

    જો (len == 0)
    {
	len += mb_char2len(mc);
	sta = sta->out;
    }

    // We don't care about the order of composing characters.  Get them into
    // cchars[] first.
    જ્યારે (len < clen)
    {
	mc = mb_ptr2char(rex.આગત + len);
	cchars[ccount++] = mc;
	len += mb_char2len(mc);
	જો (ccount == MAX_MCO)
	    તોડો;
    }

    // Check that each composing char in the pattern matches a composing char
    // in the text.  We do not check if all composing chars are matched.
    જ્યારે (sta->c != NFA_END_COMPOSING)
    {
	માટે (j = 0; j < ccount; ++j)
	    જો (cchars[j] == sta->c)
		તોડો;
	જો (j == ccount)
	    પાછા FAIL;
	sta = sta->out;
    }

    પાછા OK;
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
    સ્થિર પૂર્ણાંક
nfa_regmatch(
    nfa_regprog_T	*prog,
    nfa_state_T		*start,
    regsubs_T		*submatch,
    regsubs_T		*m)
{
    પૂર્ણાંક		result = FALSE;
    size_t	size = 0;
    પૂર્ણાંક		flag = 0;
    પૂર્ણાંક		go_to_nextline = FALSE;
    nfa_thread_T *t;
    nfa_list_T	list[2];
    પૂર્ણાંક		listidx;
    nfa_list_T	*thislist;
    nfa_list_T	*nextlist;
    પૂર્ણાંક		*listids = ખાલી;
    પૂર્ણાંક		listids_len = 0;
    nfa_state_T *add_state;
    પૂર્ણાંક		add_here;
    પૂર્ણાંક		add_ગણતરી;
    પૂર્ણાંક		add_off = 0;
    પૂર્ણાંક		toplevel = start->c == NFA_MOPEN;
    regsubs_T	*r;
#ifdef NFA_REGEXP_DEBUG_LOG
    FILE	*debug;
#endif

    // Some patterns may take a long time to match, especially when using
    // recursive_regmatch(). Allow interrupting them with CTRL-C.
    fast_breakcheck();
    જો (got_int)
	પાછા FALSE;
#ifdef FEAT_RELTIME
    જો (nfa_did_time_out())
	પાછા FALSE;
#endif

#ifdef NFA_REGEXP_DEBUG_LOG
    debug = fopen(NFA_REGEXP_DEBUG_LOG, "a");
    જો (debug == ખાલી)
    {
	semsg("(NFA) COULD NOT OPEN %s!", NFA_REGEXP_DEBUG_LOG);
	પાછા FALSE;
    }
#endif
    nfa_match = FALSE;

    // Allocate memory for the lists of nodes.
    size = (prog->nstate + 1) * કદ(nfa_thread_T);

    // Reuse cached list buffers from prog when available (top-level call).
    // Recursive calls must allocate their own buffers.
    જો (toplevel && prog->listbuf[0] != ખાલી)
    {
	list[0].t = (nfa_thread_T *)prog->listbuf[0];
	list[1].t = (nfa_thread_T *)prog->listbuf[1];
	prog->listbuf[0] = ખાલી;
	prog->listbuf[1] = ખાલી;
    }
    નહીંતર
    {
	list[0].t = alloc(size);
	list[1].t = alloc(size);
    }
    list[0].len = prog->nstate + 1;
    list[1].len = prog->nstate + 1;
    જો (list[0].t == ખાલી || list[1].t == ખાલી)
	જાઓ theend;

#ifdef ENABLE_LOG
    log_fd = fopen(NFA_REGEXP_RUN_LOG, "a");
    જો (log_fd == ખાલી)
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
    જો (toplevel)
    {
	જો (REG_MULTI)
	{
	    m->norm.list.multi[0].start_lnum = rex.lnum;
	    m->norm.list.multi[0].start_col = (colnr_T)(rex.આગત - rex.લીટી);
	    m->norm.orig_start_col = m->norm.list.multi[0].start_col;
	}
	નહીંતર
	    m->norm.list.લીટી[0].start = rex.આગત;
	m->norm.in_use = 1;
	r = addstate(thislist, start->out, m, ખાલી, 0, 0);
    }
    નહીંતર
	r = addstate(thislist, start, m, ખાલી, 0, 0);
    જો (r == ખાલી)
    {
	nfa_match = NFA_TOO_EXPENSIVE;
	જાઓ theend;
    }

#define	ADD_STATE_IF_MATCH(state)	\
    જો (result)				\
    {					\
	add_state = state->out;		\
	add_off = clen;			\
    }

    /*
     * Run for each character.
     */
    માટે (;;)
    {
	પૂર્ણાંક	curc;
	પૂર્ણાંક	clen;

	જો (has_mbyte)
	{
	    // Fast path for an ASCII byte not followed by a composing
	    // character, matching the check in utfc_ptr2len().  Avoids two
	    // indirect calls for the common case.
	    જો (rex.આગત[0] != NUL && rex.આગત[0] < 0x80 && rex.આગત[1] < 0x80)
	    {
		curc = rex.આગત[0];
		clen = 1;
	    }
	    નહીંતર
	    {
		curc = (*mb_ptr2char)(rex.આગત);
		clen = (*mb_ptr2len)(rex.આગત);
	    }
	}
	નહીંતર
	{
	    curc = *rex.આગત;
	    clen = 1;
	}
	જો (curc == NUL)
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
	જો (prog->re_engine == AUTOMATIC_ENGINE
		&& (rex.nfa_listid >= NFA_MAX_STATES
#ifdef FEAT_EVAL
		    || nfa_fail_for_testing
#endif
		    ))
	{
	    // too many states, retry with old engine
	    nfa_match = NFA_TOO_EXPENSIVE;
	    જાઓ theend;
	}

	thislist->id = rex.nfa_listid;
	nextlist->id = rex.nfa_listid + 1;

#ifdef ENABLE_LOG
	fprintf(log_fd, "------------------------------------------\n");
	fprintf(log_fd, ">>> Reginput is \"%s\"\n", rex.આગત);
	fprintf(log_fd, ">>> Advanced one character... Current char is %c (code %d) \n", curc, (પૂર્ણાંક)curc);
	fprintf(log_fd, ">>> Thislist has %d states available: ", thislist->n);
	{
	    પૂર્ણાંક i;

	    માટે (i = 0; i < thislist->n; i++)
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
	જો (thislist->n == 0)
	    તોડો;

	// compute nextlist
	માટે (listidx = 0; listidx < thislist->n; ++listidx)
	{
	    // If the list gets very long there probably is something wrong.
	    // At least allow interrupting with CTRL-C.  Only check once in a
	    // while, calling ui_breakcheck() for every state is too slow.
	    સ્થિર પૂર્ણાંક	breakcheck_ગણતરી = 0;  // using "static" makes it faster
	    જો (unlikely(++breakcheck_ગણતરી >= 100000))
	    {
		ui_breakcheck();
		breakcheck_ગણતરી = 0;
	    }
	    જો (got_int)
		તોડો;
#ifdef FEAT_RELTIME
	    જો (nfa_did_time_out())
		તોડો;
#endif
	    t = &thislist->t[listidx];

#ifdef NFA_REGEXP_DEBUG_LOG
	    nfa_set_code(t->state->c);
	    fprintf(debug, "%s, ", code);
#endif
#ifdef ENABLE_LOG
	    {
		પૂર્ણાંક col;

		જો (t->subs.norm.in_use <= 0)
		    col = -1;
		નહીંતર જો (REG_MULTI)
		    col = t->subs.norm.list.multi[0].start_col;
		નહીંતર
		    col = (પૂર્ણાંક)(t->subs.norm.list.લીટી[0].start - rex.લીટી);
		nfa_set_code(t->state->c);
		fprintf(log_fd, "(%d) char %d %s (start col %d)%s... \n",
			abs(t->state->id), (પૂર્ણાંક)t->state->c, code, col,
			pim_info(&t->pim));
	    }
#endif

	    /*
	     * Handle the possible codes of the current state.
	     * The most important is NFA_MATCH.
	     */
	    add_state = ખાલી;
	    add_here = FALSE;
	    add_ગણતરી = 0;
	    પસંદગી (t->state->c)
	    {
	    સ્થિતિ NFA_MATCH:
	      {
		// If the match is not at the start of the line, ends before a
		// composing characters and rex.reg_icombine is not set, that
		// is not really a match.
		જો (enc_utf8 && !rex.reg_icombine
			     && rex.આગત != rex.લીટી && utf_iscomposing(curc))
		    તોડો;

		nfa_match = TRUE;
		copy_sub(&submatch->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		જો (rex.nfa_has_zsubexpr)
		    copy_sub(&submatch->synt, &t->subs.synt);
#endif
#ifdef ENABLE_LOG
		log_subsexpr(&t->subs);
#endif
		// Found the left-most longest match, do not look at any other
		// states at this position.  When the list of states is going
		// to be empty quit without advancing, so that "rex.input" is
		// correct.
		જો (nextlist->n == 0)
		    clen = 0;
		જાઓ nextchar;
	      }

	    સ્થિતિ NFA_END_INVISIBLE:
	    સ્થિતિ NFA_END_INVISIBLE_NEG:
	    સ્થિતિ NFA_END_PATTERN:
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
		જો (nfa_endp != ખાલી)
		{
		    જો (REG_MULTI)
			fprintf(log_fd, "Current lnum: %d, endp lnum: %d; current col: %d, endp col: %d\n",
				(પૂર્ણાંક)rex.lnum,
				(પૂર્ણાંક)nfa_endp->se_u.pos.lnum,
				(પૂર્ણાંક)(rex.આગત - rex.લીટી),
				nfa_endp->se_u.pos.col);
		    નહીંતર
			fprintf(log_fd, "Current col: %d, endp col: %d\n",
				(પૂર્ણાંક)(rex.આગત - rex.લીટી),
				(પૂર્ણાંક)(nfa_endp->se_u.ptr - rex.આગત));
		}
#endif
		// If "nfa_endp" is set it's only a match if it ends at
		// "nfa_endp"
		જો (nfa_endp != ખાલી && (REG_MULTI
			? (rex.lnum != nfa_endp->se_u.pos.lnum
			    || (પૂર્ણાંક)(rex.આગત - rex.લીટી)
						!= nfa_endp->se_u.pos.col)
			: rex.આગત != nfa_endp->se_u.ptr))
		    તોડો;

		// do not set submatches for \@!
		જો (t->state->c != NFA_END_INVISIBLE_NEG)
		{
		    copy_sub(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		    જો (rex.nfa_has_zsubexpr)
			copy_sub(&m->synt, &t->subs.synt);
#endif
		}
#ifdef ENABLE_LOG
		fprintf(log_fd, "Match found:\n");
		log_subsexpr(m);
#endif
		nfa_match = TRUE;
		// See comment above at "goto nextchar".
		જો (nextlist->n == 0)
		    clen = 0;
		જાઓ nextchar;

	    સ્થિતિ NFA_START_INVISIBLE:
	    સ્થિતિ NFA_START_INVISIBLE_FIRST:
	    સ્થિતિ NFA_START_INVISIBLE_NEG:
	    સ્થિતિ NFA_START_INVISIBLE_NEG_FIRST:
	    સ્થિતિ NFA_START_INVISIBLE_BEFORE:
	    સ્થિતિ NFA_START_INVISIBLE_BEFORE_FIRST:
	    સ્થિતિ NFA_START_INVISIBLE_BEFORE_NEG:
	    સ્થિતિ NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
		{
#ifdef ENABLE_LOG
		    fprintf(log_fd, "Failure chance invisible: %d, what follows: %d\n",
			    failure_chance(t->state->out, 0),
			    failure_chance(t->state->out1->out, 0));
#endif
		    // Do it directly if there already is a PIM or when
		    // nfa_postprocess() detected it will work better.
		    જો (t->pim.result != NFA_PIM_UNUSED
			 || t->state->c == NFA_START_INVISIBLE_FIRST
			 || t->state->c == NFA_START_INVISIBLE_NEG_FIRST
			 || t->state->c == NFA_START_INVISIBLE_BEFORE_FIRST
			 || t->state->c == NFA_START_INVISIBLE_BEFORE_NEG_FIRST)
		    {
			પૂર્ણાંક in_use = m->norm.in_use;

			// Copy submatch info for the recursive call, opposite
			// of what happens on success below.
			copy_sub_off(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
			જો (rex.nfa_has_zsubexpr)
			    copy_sub_off(&m->synt, &t->subs.synt);
#endif

			/*
			 * First try matching the invisible match, then what
			 * follows.
			 */
			result = recursive_regmatch(t->state, ખાલી, prog,
					  submatch, m, &listids, &listids_len);
			જો (result == NFA_TOO_EXPENSIVE)
			{
			    nfa_match = result;
			    જાઓ theend;
			}

			// for \@! and \@<! it is a match when the result is
			// FALSE
			જો (result != (t->state->c == NFA_START_INVISIBLE_NEG
			       || t->state->c == NFA_START_INVISIBLE_NEG_FIRST
			       || t->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			       || t->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&t->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
			    જો (rex.nfa_has_zsubexpr)
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
		    નહીંતર
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
			જો (REG_MULTI)
			{
			    pim.end.pos.col = (પૂર્ણાંક)(rex.આગત - rex.લીટી);
			    pim.end.pos.lnum = rex.lnum;
			}
			નહીંતર
			    pim.end.ptr = rex.આગત;

			// t->state->out1 is the corresponding END_INVISIBLE
			// node; Add its out to the current list (zero-width
			// match).
			જો (addstate_here(thislist, t->state->out1->out,
					     &t->subs, &pim, &listidx) == ખાલી)
			{
			    nfa_match = NFA_TOO_EXPENSIVE;
			    જાઓ theend;
			}
		    }
		}
		તોડો;

	    સ્થિતિ NFA_START_PATTERN:
	      {
		nfa_state_T *skip = ખાલી;
#ifdef ENABLE_LOG
		પૂર્ણાંક	    skip_lid = 0;
#endif

		// There is no point in trying to match the pattern if the
		// output state is not going to be added to the list.
		જો (state_in_list(nextlist, t->state->out1->out, &t->subs))
		{
		    skip = t->state->out1->out;
#ifdef ENABLE_LOG
		    skip_lid = nextlist->id;
#endif
		}
		નહીંતર જો (state_in_list(nextlist,
					  t->state->out1->out->out, &t->subs))
		{
		    skip = t->state->out1->out->out;
#ifdef ENABLE_LOG
		    skip_lid = nextlist->id;
#endif
		}
		નહીંતર જો (state_in_list(thislist,
					  t->state->out1->out->out, &t->subs))
		{
		    skip = t->state->out1->out->out;
#ifdef ENABLE_LOG
		    skip_lid = thislist->id;
#endif
		}
		જો (skip != ખાલી)
		{
#ifdef ENABLE_LOG
		    nfa_set_code(skip->c);
		    fprintf(log_fd, "> Not trying to match pattern, output state %d is already in list %d. char %d: %s\n",
			    abs(skip->id), skip_lid, skip->c, code);
#endif
		    તોડો;
		}
		// Copy submatch info to the recursive call, opposite of what
		// happens afterwards.
		copy_sub_off(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		જો (rex.nfa_has_zsubexpr)
		    copy_sub_off(&m->synt, &t->subs.synt);
#endif

		// First try matching the pattern.
		result = recursive_regmatch(t->state, ખાલી, prog,
					  submatch, m, &listids, &listids_len);
		જો (result == NFA_TOO_EXPENSIVE)
		{
		    nfa_match = result;
		    જાઓ theend;
		}
		જો (result)
		{
		    પૂર્ણાંક bytelen;

#ifdef ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN matches:\n");
		    log_subsexpr(m);
#endif
		    // Copy submatch info from the recursive call
		    copy_sub_off(&t->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
		    જો (rex.nfa_has_zsubexpr)
			copy_sub_off(&t->subs.synt, &m->synt);
#endif
		    // Now we need to skip over the matched text and then
		    // continue with what follows.
		    જો (REG_MULTI)
			// TODO: multi-line match
			bytelen = m->norm.list.multi[0].end_col
						  - (પૂર્ણાંક)(rex.આગત - rex.લીટી);
		    નહીંતર
			bytelen = (પૂર્ણાંક)(m->norm.list.લીટી[0].end - rex.આગત);

#ifdef ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN length: %d\n", bytelen);
#endif
		    જો (bytelen == 0)
		    {
			// empty match, output of corresponding
			// NFA_END_PATTERN/NFA_SKIP to be used at current
			// position
			add_here = TRUE;
			add_state = t->state->out1->out->out;
		    }
		    નહીંતર જો (bytelen <= clen)
		    {
			// match current character, output of corresponding
			// NFA_END_PATTERN to be used at next position.
			add_state = t->state->out1->out->out;
			add_off = clen;
		    }
		    નહીંતર
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			add_state = t->state->out1->out;
			add_off = bytelen;
			add_ગણતરી = bytelen - clen;
		    }
		}
		તોડો;
	      }

	    સ્થિતિ NFA_BOL:
		જો (rex.આગત == rex.લીટી)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		તોડો;

	    સ્થિતિ NFA_EOL:
		જો (curc == NUL)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		તોડો;

	    સ્થિતિ NFA_BOW:
		result = TRUE;

		જો (curc == NUL)
		    result = FALSE;
		નહીંતર જો (has_mbyte)
		{
		    પૂર્ણાંક this_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.આગત, rex.reg_buf);
		    જો (this_class <= 1)
			result = FALSE;
		    નહીંતર જો (reg_prev_class() == this_class)
			result = FALSE;
		}
		નહીંતર જો (!vim_iswordc_buf(curc, rex.reg_buf)
			   || (rex.આગત > rex.લીટી
			       && vim_iswordc_buf(rex.આગત[-1], rex.reg_buf)))
		    result = FALSE;
		જો (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		તોડો;

	    સ્થિતિ NFA_EOW:
		result = TRUE;
		જો (rex.આગત == rex.લીટી)
		    result = FALSE;
		નહીંતર જો (has_mbyte)
		{
		    પૂર્ણાંક this_class, prev_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.આગત, rex.reg_buf);
		    prev_class = reg_prev_class();
		    જો (this_class == prev_class
					|| prev_class == 0 || prev_class == 1)
			result = FALSE;
		}
		નહીંતર જો (!vim_iswordc_buf(rex.આગત[-1], rex.reg_buf)
			|| (rex.આગત[0] != NUL
					&& vim_iswordc_buf(curc, rex.reg_buf)))
		    result = FALSE;
		જો (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		તોડો;

	    સ્થિતિ NFA_BOF:
		જો (rex.lnum == 0 && rex.આગત == rex.લીટી
				     && (!REG_MULTI || rex.reg_firstlnum == 1))
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		તોડો;

	    સ્થિતિ NFA_EOF:
		જો (rex.lnum == rex.reg_maxline && curc == NUL)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		તોડો;

	    સ્થિતિ NFA_COMPOSING:
	    {
		nfa_state_T *end;

		result = match_composing(t->state->out, curc, clen);

		end = t->state->out1;	    // NFA_END_COMPOSING
		ADD_STATE_IF_MATCH(end);
		તોડો;
	    }

	    સ્થિતિ NFA_NEWL:
		જો (curc == NUL && !rex.reg_લીટી_lbr && REG_MULTI
						&& rex.lnum <= rex.reg_maxline)
		{
		    go_to_nextline = TRUE;
		    // Pass -1 for the offset, which means taking the position
		    // at the start of the next line.
		    add_state = t->state->out;
		    add_off = -1;
		}
		નહીંતર જો (curc == '\n' && rex.reg_લીટી_lbr)
		{
		    // match \n as if it is an ordinary character
		    add_state = t->state->out;
		    add_off = 1;
		}
		તોડો;

	    સ્થિતિ NFA_START_COLL:
	    સ્થિતિ NFA_START_NEG_COLL:
	      {
		// What follows is a list of characters, until NFA_END_COLL.
		// One of them must match or none of them must match.
		nfa_state_T	*state;
		પૂર્ણાંક		result_if_matched;
		પૂર્ણાંક		c1, c2;

		// Never match EOL. If it's part of the collection it is added
		// as a separate state with an OR.
		જો (curc == NUL)
		    તોડો;

		state = t->state->out;
		result_if_matched = (t->state->c == NFA_START_COLL);
		માટે (;;)
		{
		    જો (state->c == NFA_COMPOSING)
		    {
			nfa_state_T *end;

			result = match_composing(t->state->out->out, curc,
									clen);

			જો (t->state->out->out1 != ખાલી
				&& t->state->out->out1->c == NFA_END_COMPOSING)
			{
			    end = t->state->out->out1;
			    ADD_STATE_IF_MATCH(end);
			}
			તોડો;
		    }
		    જો (state->c == NFA_END_COLL)
		    {
			result = !result_if_matched;
			તોડો;
		    }
		    જો (state->c == NFA_RANGE_MIN)
		    {
			c1 = state->val;
			state = state->out; // advance to NFA_RANGE_MAX
			c2 = state->val;
#ifdef ENABLE_LOG
			fprintf(log_fd, "NFA_RANGE_MIN curc=%d c1=%d c2=%d\n",
				curc, c1, c2);
#endif
			જો (curc >= c1 && curc <= c2)
			{
			    result = result_if_matched;
			    તોડો;
			}
			જો (rex.reg_ic)
			{
			    પૂર્ણાંક curc_નીચલું = MB_CASEFOLD(curc);
			    પૂર્ણાંક done = FALSE;

			    માટે ( ; c1 <= c2; ++c1)
				જો (MB_CASEFOLD(c1) == curc_નીચલું)
				{
				    result = result_if_matched;
				    done = TRUE;
				    તોડો;
				}
			    જો (done)
				તોડો;
			}
		    }
		    નહીંતર જો (state->c < 0 ? check_char_class(state->c, curc)
			       : (curc == state->c
				   || (rex.reg_ic && MB_CASEFOLD(curc)
						    == MB_CASEFOLD(state->c))))
		    {
			result = result_if_matched;
			તોડો;
		    }
		    state = state->out;
		}
		જો (result)
		{
		    // next state is in out of the NFA_END_COLL, out1 of
		    // START points to the END state
		    add_state = t->state->out1->out;
		    add_off = clen;
		}
		તોડો;
	      }

	    સ્થિતિ NFA_ANY:
		// Any char except '\0', (end of input) does not match.
		જો (curc > 0)
		{
		    add_state = t->state->out;
		    add_off = clen;
		}
		તોડો;

	    સ્થિતિ NFA_ANY_COMPOSING:
		// On a composing character skip over it.  Otherwise do
		// nothing.  Always matches.
		જો (enc_utf8 && utf_iscomposing(curc))
		{
		    add_off = clen;
		}
		નહીંતર
		{
		    add_here = TRUE;
		    add_off = 0;
		}
		add_state = t->state->out;
		તોડો;

	    /*
	     * Character classes like \a for alpha, \d for digit etc.
	     */
	    સ્થિતિ NFA_IDENT:	//  \i
		result = vim_isIDc(curc);
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_SIDENT:	//  \I
		result = !VIM_ISDIGIT(curc) && vim_isIDc(curc);
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_KWORD:	//  \k
		result = vim_iswordp_buf(rex.આગત, rex.reg_buf);
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_SKWORD:	//  \K
		result = !VIM_ISDIGIT(curc)
				     && vim_iswordp_buf(rex.આગત, rex.reg_buf);
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_FNAME:	//  \f
		result = vim_isfilec(curc);
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_SFNAME:	//  \F
		result = !VIM_ISDIGIT(curc) && vim_isfilec(curc);
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_PRINT:	//  \p
		result = vim_isprintc(PTR2CHAR(rex.આગત));
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_SPRINT:	//  \P
		result = !VIM_ISDIGIT(curc) && vim_isprintc(PTR2CHAR(rex.આગત));
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_WHITE:	//  \s
		result = VIM_ISWHITE(curc);
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_NWHITE:	//  \S
		result = curc != NUL && !VIM_ISWHITE(curc);
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_DIGIT:	//  \d
		result = ri_digit(curc);
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_NDIGIT:	//  \D
		result = curc != NUL && !ri_digit(curc);
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_HEX:	//  \x
		result = ri_hex(curc);
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_NHEX:	//  \X
		result = curc != NUL && !ri_hex(curc);
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_OCTAL:	//  \o
		result = ri_octal(curc);
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_NOCTAL:	//  \O
		result = curc != NUL && !ri_octal(curc);
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_WORD:	//  \w
		result = ri_word(curc);
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_NWORD:	//  \W
		result = curc != NUL && !ri_word(curc);
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_HEAD:	//  \h
		result = ri_head(curc);
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_NHEAD:	//  \H
		result = curc != NUL && !ri_head(curc);
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_ALPHA:	//  \a
		result = ri_alpha(curc);
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_NALPHA:	//  \A
		result = curc != NUL && !ri_alpha(curc);
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_LOWER:	//  \l
		result = ri_lower(curc);
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_NLOWER:	//  \L
		result = curc != NUL && !ri_lower(curc);
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_UPPER:	//  \u
		result = ri_upper(curc);
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_NUPPER:	// \U
		result = curc != NUL && !ri_upper(curc);
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_LOWER_IC:	// [a-z]
		result = ri_lower(curc) || (rex.reg_ic && ri_upper(curc));
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_NLOWER_IC:	// [^a-z]
		result = curc != NUL
			&& !(ri_lower(curc) || (rex.reg_ic && ri_upper(curc)));
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_UPPER_IC:	// [A-Z]
		result = ri_upper(curc) || (rex.reg_ic && ri_lower(curc));
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_NUPPER_IC:	// ^[A-Z]
		result = curc != NUL
			&& !(ri_upper(curc) || (rex.reg_ic && ri_lower(curc)));
		ADD_STATE_IF_MATCH(t->state);
		તોડો;

	    સ્થિતિ NFA_BACKREF1:
	    સ્થિતિ NFA_BACKREF2:
	    સ્થિતિ NFA_BACKREF3:
	    સ્થિતિ NFA_BACKREF4:
	    સ્થિતિ NFA_BACKREF5:
	    સ્થિતિ NFA_BACKREF6:
	    સ્થિતિ NFA_BACKREF7:
	    સ્થિતિ NFA_BACKREF8:
	    સ્થિતિ NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	    સ્થિતિ NFA_ZREF1:
	    સ્થિતિ NFA_ZREF2:
	    સ્થિતિ NFA_ZREF3:
	    સ્થિતિ NFA_ZREF4:
	    સ્થિતિ NFA_ZREF5:
	    સ્થિતિ NFA_ZREF6:
	    સ્થિતિ NFA_ZREF7:
	    સ્થિતિ NFA_ZREF8:
	    સ્થિતિ NFA_ZREF9:
#endif
		// \1 .. \9  \z1 .. \z9
	      {
		પૂર્ણાંક subidx;
		પૂર્ણાંક bytelen;

#ifdef FEAT_SYN_HL
		જો (t->state->c >= NFA_BACKREF1 && t->state->c <= NFA_BACKREF9)
#endif
		{
		    subidx = t->state->c - NFA_BACKREF1 + 1;
		    result = match_backref(&t->subs.norm, subidx, &bytelen);
		}
#ifdef FEAT_SYN_HL
		નહીંતર
		{
		    subidx = t->state->c - NFA_ZREF1 + 1;
		    result = match_zref(subidx, &bytelen);
		}
#endif

		જો (result)
		{
		    જો (bytelen == 0)
		    {
			// empty match always works, output of NFA_SKIP to be
			// used next
			add_here = TRUE;
			add_state = t->state->out->out;
		    }
		    નહીંતર જો (bytelen <= clen)
		    {
			// match current character, jump ahead to out of
			// NFA_SKIP
			add_state = t->state->out->out;
			add_off = clen;
		    }
		    નહીંતર
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			add_state = t->state->out;
			add_off = bytelen;
			add_ગણતરી = bytelen - clen;
		    }
		}
		તોડો;
	      }
	    સ્થિતિ NFA_SKIP:
	      // character of previous matching \1 .. \9  or \@>
	      જો (t->ગણતરી - clen <= 0)
	      {
		  // end of match, go to what follows
		  add_state = t->state->out;
		  add_off = clen;
	      }
	      નહીંતર
	      {
		  // add state again with decremented count
		  add_state = t->state;
		  add_off = 0;
		  add_ગણતરી = t->ગણતરી - clen;
	      }
	      તોડો;

	    સ્થિતિ NFA_LNUM:
	    સ્થિતિ NFA_LNUM_GT:
	    સ્થિતિ NFA_LNUM_LT:
		result = (REG_MULTI &&
			nfa_re_અંક_cmp(t->state->val, t->state->c - NFA_LNUM,
			    (long_u)(rex.lnum + rex.reg_firstlnum)));
		જો (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		તોડો;

	    સ્થિતિ NFA_COL:
	    સ્થિતિ NFA_COL_GT:
	    સ્થિતિ NFA_COL_LT:
		result = nfa_re_અંક_cmp(t->state->val, t->state->c - NFA_COL,
			(long_u)(rex.આગત - rex.લીટી) + 1);
		જો (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		તોડો;

	    સ્થિતિ NFA_VCOL:
	    સ્થિતિ NFA_VCOL_GT:
	    સ્થિતિ NFA_VCOL_LT:
		{
		    પૂર્ણાંક     op = t->state->c - NFA_VCOL;
		    colnr_T col = (colnr_T)(rex.આગત - rex.લીટી);
		    win_T   *wp = rex.reg_win == ખાલી ? curwin : rex.reg_win;

		    // Bail out quickly when there can't be a match, avoid the
		    // overhead of win_linetabsize() on long lines.
		    જો (op != 1 && col > t->state->val
			    * (has_mbyte ? MB_MAXBYTES : 1))
			તોડો;
		    result = FALSE;
		    જો (op == 1 && col - 1 > t->state->val && col > 100)
		    {
			પૂર્ણાંક ts = wp->w_buffer->b_p_ts;

			// Guess that a character won't use more columns than
			// 'tabstop', with a minimum of 4.
			જો (ts < 4)
			    ts = 4;
			result = col > t->state->val * ts;
		    }
		    જો (!result)
		    {
			linenr_T    lnum = REG_MULTI
					    ? rex.reg_firstlnum + rex.lnum : 1;
			long_u	    vcol;

			જો (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_buffer->b_ml.ml_લીટી_ગણતરી))
			    lnum = 1;
			vcol = (long_u)win_linetabsize(wp, lnum, rex.લીટી, col);
			result = nfa_re_અંક_cmp(t->state->val, op, vcol + 1);
		    }
		    જો (result)
		    {
			add_here = TRUE;
			add_state = t->state->out;
		    }
		}
		તોડો;

	    સ્થિતિ NFA_MARK:
	    સ્થિતિ NFA_MARK_GT:
	    સ્થિતિ NFA_MARK_LT:
	      {
		pos_T	*pos;
		size_t	col = REG_MULTI ? rex.આગત - rex.લીટી : 0;

		pos = getmark_buf(rex.reg_buf, t->state->val, FALSE);

		// Line may have been freed, get it again.
		જો (REG_MULTI)
		{
		    rex.લીટી = reg_getline(rex.lnum);
		    rex.આગત = rex.લીટી + col;
		}

		// Compare the mark position to the match position, if the mark
		// exists and mark is set in reg_buf.
		જો (pos != ખાલી && pos->lnum > 0)
		{
		    colnr_T pos_col = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->col == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->col;

		    result = (pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_col == (colnr_T)(rex.આગત - rex.લીટી)
				    ? t->state->c == NFA_MARK
				    : (pos_col < (colnr_T)(rex.આગત - rex.લીટી)
					? t->state->c == NFA_MARK_GT
					: t->state->c == NFA_MARK_LT))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? t->state->c == NFA_MARK_GT
				    : t->state->c == NFA_MARK_LT));
		    જો (result)
		    {
			add_here = TRUE;
			add_state = t->state->out;
		    }
		}
		તોડો;
	      }

	    સ્થિતિ NFA_CURSOR:
		result = (rex.reg_win != ખાલી
			&& (rex.lnum + rex.reg_firstlnum
						 == rex.reg_win->w_cursor.lnum)
			&& ((colnr_T)(rex.આગત - rex.લીટી)
						== rex.reg_win->w_cursor.col));
		જો (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		તોડો;

	    સ્થિતિ NFA_VISUAL:
		result = reg_match_visual();
		જો (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		તોડો;

	    સ્થિતિ NFA_MOPEN1:
	    સ્થિતિ NFA_MOPEN2:
	    સ્થિતિ NFA_MOPEN3:
	    સ્થિતિ NFA_MOPEN4:
	    સ્થિતિ NFA_MOPEN5:
	    સ્થિતિ NFA_MOPEN6:
	    સ્થિતિ NFA_MOPEN7:
	    સ્થિતિ NFA_MOPEN8:
	    સ્થિતિ NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	    સ્થિતિ NFA_ZOPEN:
	    સ્થિતિ NFA_ZOPEN1:
	    સ્થિતિ NFA_ZOPEN2:
	    સ્થિતિ NFA_ZOPEN3:
	    સ્થિતિ NFA_ZOPEN4:
	    સ્થિતિ NFA_ZOPEN5:
	    સ્થિતિ NFA_ZOPEN6:
	    સ્થિતિ NFA_ZOPEN7:
	    સ્થિતિ NFA_ZOPEN8:
	    સ્થિતિ NFA_ZOPEN9:
#endif
	    સ્થિતિ NFA_NOPEN:
	    સ્થિતિ NFA_ZSTART:
		// These states are only added to be able to bail out when
		// they are added again, nothing is to be done.
		તોડો;

	    મૂળભૂત:	// regular character
	      {
		પૂર્ણાંક c = t->state->c;

#ifdef DEBUG
		જો (c < 0)
		    siemsg("Negative state char: %ld", (long)c);
#endif
		result = (c == curc);

		જો (!result && rex.reg_ic)
		    result = MB_CASEFOLD(c) == MB_CASEFOLD(curc);
		// If rex.reg_icombine is not set only skip over the character
		// itself.  When it is set skip over composing characters.
		જો (result && enc_utf8 && !rex.reg_icombine)
		    clen = utf_ptr2len(rex.આગત);
		ADD_STATE_IF_MATCH(t->state);
		તોડો;
	      }

	    } // switch (t->state->c)

	    જો (add_state != ખાલી)
	    {
		nfa_pim_T *pim;
		nfa_pim_T pim_copy;

		જો (t->pim.result == NFA_PIM_UNUSED)
		    pim = ખાલી;
		નહીંતર
		    pim = &t->pim;

		// Handle the postponed invisible match if the match might end
		// without advancing and before the end of the line.
		જો (pim != ખાલી && (clen == 0 || match_follows(add_state, 0)))
		{
		    જો (pim->result == NFA_PIM_TODO)
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
			જો (result != (pim->state->c == NFA_START_INVISIBLE_NEG
			     || pim->state->c == NFA_START_INVISIBLE_NEG_FIRST
			     || pim->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&pim->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
			    જો (rex.nfa_has_zsubexpr)
				copy_sub_off(&pim->subs.synt, &m->synt);
#endif
			}
		    }
		    નહીંતર
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
		    જો (result != (pim->state->c == NFA_START_INVISIBLE_NEG
			     || pim->state->c == NFA_START_INVISIBLE_NEG_FIRST
			     || pim->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
		    {
			// Copy submatch info from the recursive call
			copy_sub_off(&t->subs.norm, &pim->subs.norm);
#ifdef FEAT_SYN_HL
			જો (rex.nfa_has_zsubexpr)
			    copy_sub_off(&t->subs.synt, &pim->subs.synt);
#endif
		    }
		    નહીંતર
			// look-behind match failed, don't add the state
			ચાલુ;

		    // Postponed invisible match was handled, don't add it to
		    // following states.
		    pim = ખાલી;
		}

		// If "pim" points into l->t it will become invalid when
		// adding the state causes the list to be reallocated.  Make a
		// local copy to avoid that.
		જો (pim == &t->pim)
		{
		    copy_pim(&pim_copy, pim);
		    pim = &pim_copy;
		}

		જો (add_here)
		    r = addstate_here(thislist, add_state, &t->subs,
								pim, &listidx);
		નહીંતર
		{
		    r = addstate(nextlist, add_state, &t->subs, pim, add_off, 0);
		    જો (add_ગણતરી > 0)
			nextlist->t[nextlist->n - 1].ગણતરી = add_ગણતરી;
		}
		જો (r == ખાલી)
		{
		    nfa_match = NFA_TOO_EXPENSIVE;
		    જાઓ theend;
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
	જો (nfa_match == FALSE
		&& ((toplevel
			&& rex.lnum == 0
			&& clen != 0
			&& (rex.reg_maxcol == 0
			  || (colnr_T)(rex.આગત - rex.લીટી) < rex.reg_maxcol))
		    || (nfa_endp != ખાલી
			&& (REG_MULTI
			    ? (rex.lnum < nfa_endp->se_u.pos.lnum
			       || (rex.lnum == nfa_endp->se_u.pos.lnum
				   && (પૂર્ણાંક)(rex.આગત - rex.લીટી)
						    < nfa_endp->se_u.pos.col))
			    : rex.આગત < nfa_endp->se_u.ptr))))
	{
#ifdef ENABLE_LOG
	    fprintf(log_fd, "(---) STARTSTATE\n");
#endif
	    // Inline optimized code for addstate() if we know the state is
	    // the first MOPEN.
	    જો (toplevel)
	    {
		પૂર્ણાંક add = TRUE;
		પૂર્ણાંક c;

		જો (prog->regstart != NUL && clen != 0)
		{
		    જો (nextlist->n == 0)
		    {
			colnr_T col = (colnr_T)(rex.આગત - rex.લીટી) + clen;

			// Nextlist is empty, we can skip ahead to the
			// character that must appear at the start.
			જો (skip_to_start(prog->regstart, &col) == FAIL)
			    તોડો;
#ifdef ENABLE_LOG
			fprintf(log_fd, "  Skipping ahead %d bytes to regstart\n",
				col - ((colnr_T)(rex.આગત - rex.લીટી) + clen));
#endif
			rex.આગત = rex.લીટી + col - clen;
		    }
		    નહીંતર
		    {
			// Checking if the required start character matches is
			// cheaper than adding a state that won't match.
			c = PTR2CHAR(rex.આગત + clen);
			જો (c != prog->regstart && (!rex.reg_ic
			     || MB_CASEFOLD(c) != MB_CASEFOLD(prog->regstart)))
			{
#ifdef ENABLE_LOG
			    fprintf(log_fd, "  Skipping start state, regstart does not match\n");
#endif
			    add = FALSE;
			}
		    }
		}

		જો (add)
		{
		    જો (REG_MULTI)
		    {
			m->norm.list.multi[0].start_col =
					 (colnr_T)(rex.આગત - rex.લીટી) + clen;
			m->norm.orig_start_col =
					       m->norm.list.multi[0].start_col;
		    }
		    નહીંતર
			m->norm.list.લીટી[0].start = rex.આગત + clen;
		    જો (addstate(nextlist, start->out, m, ખાલી, clen, 0) == ખાલી)
		    {
			nfa_match = NFA_TOO_EXPENSIVE;
			જાઓ theend;
		    }
		}
	    }
	    નહીંતર
	    {
		char_u	    *save_લીટી = rex.લીટી;
		char_u	    *save_આગત = rex.આગત;
		linenr_T    save_lnum = rex.lnum;

		// At the end of a line the match can only start on the next
		// line, use that position instead of the line break.
		જો (REG_MULTI && clen == 0 && nfa_endp != ખાલી
					 && rex.lnum < nfa_endp->se_u.pos.lnum)
		{
		    char_u  *next_લીટી = reg_getline(rex.lnum + 1);

		    જો (next_લીટી != ખાલી)
		    {
			rex.લીટી = next_લીટી;
			rex.આગત = rex.લીટી;
			++rex.lnum;
		    }
		}

		r = addstate(nextlist, start, m, ખાલી, clen, 0);

		rex.લીટી = save_લીટી;
		rex.આગત = save_આગત;
		rex.lnum = save_lnum;

		જો (r == ખાલી)
		{
		    nfa_match = NFA_TOO_EXPENSIVE;
		    જાઓ theend;
		}
	    }
	}

#ifdef ENABLE_LOG
	fprintf(log_fd, ">>> Thislist had %d states available: ", thislist->n);
	{
	    પૂર્ણાંક i;

	    માટે (i = 0; i < thislist->n; i++)
		fprintf(log_fd, "%d  ", abs(thislist->t[i].state->id));
	}
	fprintf(log_fd, "\n");
#endif

nextchar:
	// Advance to the next character, or advance to the next line, or
	// finish.
	જો (clen != 0)
	    rex.આગત += clen;
	નહીંતર જો (go_to_nextline || (nfa_endp != ખાલી && REG_MULTI
					&& rex.lnum < nfa_endp->se_u.pos.lnum))
	    reg_nextline();
	નહીંતર
	    તોડો;

	// Allow interrupting with CTRL-C.
	લીટી_breakcheck();
	જો (got_int)
	    તોડો;
#ifdef FEAT_RELTIME
	જો (nfa_did_time_out())
	    તોડો;
#endif
    }

#ifdef ENABLE_LOG
    જો (log_fd != stderr)
	fclose(log_fd);
    log_fd = ખાલી;
#endif

theend:
    // Cache list buffers in prog for reuse, or free if prog already has
    // cached buffers (recursive call case).
    જો (prog->listbuf[0] == ખાલી && list[0].t != ખાલી && list[1].t != ખાલી)
    {
	prog->listbuf[0] = list[0].t;
	prog->listbuf[1] = list[1].t;
    }
    નહીંતર
    {
	vim_free(list[0].t);
	vim_free(list[1].t);
    }
    vim_free(listids);
#undef ADD_STATE_IF_MATCH
#ifdef NFA_REGEXP_DEBUG_LOG
    fclose(debug);
#endif

    પાછા nfa_match;
}

/*
 * Try match of "prog" with at rex.line["col"].
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    સ્થિર long
nfa_regtry(
    nfa_regprog_T   *prog,
    colnr_T	    col,
    પૂર્ણાંક		    *timed_out UNUSED)	// flag set on timeout or NULL
{
    પૂર્ણાંક		i;
    regsubs_T	subs, m;
    nfa_state_T	*start = prog->start;
    પૂર્ણાંક		result;
#ifdef ENABLE_LOG
    FILE	*f;
#endif

    rex.આગત = rex.લીટી + col;
#ifdef FEAT_RELTIME
    nfa_timed_out = timed_out;
#endif

#ifdef ENABLE_LOG
    f = fopen(NFA_REGEXP_RUN_LOG, "a");
    જો (f != ખાલી)
    {
	fprintf(f, "\n\n\t=======================================================\n");
# ifdef DEBUG
	fprintf(f, "\tRegexp is \"%s\"\n", nfa_regengine.પદાવલિ);
# endif
	fprintf(f, "\tInput text is \"%s\" \n", rex.આગત);
	fprintf(f, "\t=======================================================\n\n");
	nfa_print_state(f, start);
	fprintf(f, "\n\n");
	fclose(f);
    }
    નહીંતર
	emsg("Could not open temporary log file for writing");
#endif

    clear_sub(&subs.norm);
    clear_sub(&m.norm);
#ifdef FEAT_SYN_HL
    clear_sub(&subs.synt);
    clear_sub(&m.synt);
#endif

    result = nfa_regmatch(prog, start, &subs, &m);
    જો (result == FALSE)
	પાછા 0;
    નહીંતર જો (result == NFA_TOO_EXPENSIVE)
	પાછા result;

    cleanup_subexpr();
    જો (REG_MULTI)
    {
	માટે (i = 0; i < subs.norm.in_use; i++)
	{
	    rex.reg_startpos[i].lnum = subs.norm.list.multi[i].start_lnum;
	    rex.reg_startpos[i].col = subs.norm.list.multi[i].start_col;

	    rex.reg_endpos[i].lnum = subs.norm.list.multi[i].end_lnum;
	    rex.reg_endpos[i].col = subs.norm.list.multi[i].end_col;
	}
	જો (rex.reg_mmatch != ખાલી)
	    rex.reg_mmatch->rmm_matchcol = subs.norm.orig_start_col;

	જો (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].col = col;
	}
	જો (rex.reg_endpos[0].lnum < 0)
	{
	    // pattern has a \ze but it didn't match, use current end
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].col = (પૂર્ણાંક)(rex.આગત - rex.લીટી);
	}
	નહીંતર
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    નહીંતર
    {
	માટે (i = 0; i < subs.norm.in_use; i++)
	{
	    rex.reg_startp[i] = subs.norm.list.લીટી[i].start;
	    rex.reg_endp[i] = subs.norm.list.લીટી[i].end;
	}

	જો (rex.reg_startp[0] == ખાલી)
	    rex.reg_startp[0] = rex.લીટી + col;
	જો (rex.reg_endp[0] == ખાલી)
	    rex.reg_endp[0] = rex.આગત;
    }

#ifdef FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = ખાલી;

    જો (prog->reghasz == REX_SET)
    {
	cleanup_zsubexpr();
	re_extmatch_out = make_extmatch();
	જો (re_extmatch_out == ખાલી)
	    પાછા 0;
	// Loop over \z1, \z2, etc.  There is no \z0.
	માટે (i = 1; i < subs.synt.in_use; i++)
	{
	    જો (REG_MULTI)
	    {
		સંરચના multipos *mpos = &subs.synt.list.multi[i];

		// Only accept single line matches that are valid.
		જો (mpos->start_lnum >= 0
			&& mpos->start_lnum == mpos->end_lnum
			&& mpos->end_col >= mpos->start_col)
		    re_extmatch_out->matches[i] =
			vim_strnsave(reg_getline(mpos->start_lnum)
							    + mpos->start_col,
					     mpos->end_col - mpos->start_col);
	    }
	    નહીંતર
	    {
		સંરચના linepos *lpos = &subs.synt.list.લીટી[i];

		જો (lpos->start != ખાલી && lpos->end != ખાલી)
		    re_extmatch_out->matches[i] =
			    vim_strnsave(lpos->start, lpos->end - lpos->start);
	    }
	}
    }
#endif

    પાછા 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 *
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    સ્થિર long
nfa_regexec_both(
    char_u	*લીટી,
    colnr_T	startcol,	// column to start looking for match
    પૂર્ણાંક		*timed_out)	// flag set on timeout or NULL
{
    nfa_regprog_T   *prog;
    long	    retval = 0L;
    પૂર્ણાંક		    i;
    colnr_T	    col = startcol;

    જો (REG_MULTI)
    {
	prog = (nfa_regprog_T *)rex.reg_mmatch->regprog;
	લીટી = reg_getline((linenr_T)0);    // relative to the cursor
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    નહીંતર
    {
	prog = (nfa_regprog_T *)rex.reg_match->regprog;
	rex.reg_startp = rex.reg_match->startp;
	rex.reg_endp = rex.reg_match->endp;
    }

    // Be paranoid...
    જો (prog == ખાલી || લીટી == ખાલી)
    {
	iemsg(e_null_argument);
	જાઓ theend;
    }

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    જો (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    નહીંતર જો (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    જો (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    rex.લીટી = લીટી;
    rex.lnum = 0;    // relative to line

    rex.nfa_has_zend = prog->has_zend;
    rex.nfa_has_backref = prog->has_backref;
    rex.nfa_nsubexpr = prog->nsubexp;
    rex.nfa_listid = 1;
    rex.nfa_alt_listid = 2;
#ifdef DEBUG
    nfa_regengine.પદાવલિ = prog->pattern;
#endif

    જો (prog->reganch && col > 0)
	પાછા 0L;

    rex.need_clear_subexpr = TRUE;
#ifdef FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    જો (prog->reghasz == REX_SET)
    {
	rex.nfa_has_zsubexpr = TRUE;
	rex.need_clear_zsubexpr = TRUE;
    }
    નહીંતર
    {
	rex.nfa_has_zsubexpr = FALSE;
	rex.need_clear_zsubexpr = FALSE;
    }
#endif

    જો (prog->regstart != NUL)
    {
	// Skip ahead until a character we know the match must start with.
	// When there is none there is no match.
	જો (skip_to_start(prog->regstart, &col) == FAIL)
	    પાછા 0L;

	// If match_text is set it contains the full text that must match.
	// Nothing else to try. Doesn't handle combining chars well.
	જો (prog->match_text != ખાલી && *prog->match_text != NUL && !rex.reg_icombine)
	{
	    retval = find_match_text(&col, prog->regstart, prog->match_text);
	    જો (REG_MULTI)
		rex.reg_mmatch->rmm_matchcol = col;
	    નહીંતર
		rex.reg_match->rm_matchcol = col;
	    પાછા retval;
	}
    }

    // If the start column is past the maximum column: no need to try.
    જો (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	જાઓ theend;

    // Set the "nstate" used by nfa_regcomp() to zero to trigger an error when
    // it's accidentally used during execution.
    nstate = 0;
    માટે (i = 0; i < prog->nstate; ++i)
    {
	prog->state[i].id = i;
	prog->state[i].lastlist[0] = 0;
	prog->state[i].lastlist[1] = 0;
    }

    retval = nfa_regtry(prog, col, timed_out);

#ifdef DEBUG
    nfa_regengine.પદાવલિ = ખાલી;
#endif

theend:
    જો (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs and
	// \ze are used.
	જો (REG_MULTI)
	{
	    lpos_T *start = &rex.reg_mmatch->startpos[0];
	    lpos_T *end = &rex.reg_mmatch->endpos[0];

	    જો (end->lnum < start->lnum
			|| (end->lnum == start->lnum && end->col < start->col))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];
	}
	નહીંતર
	{
	    જો (rex.reg_match->endp[0] < rex.reg_match->startp[0])
		rex.reg_match->endp[0] = rex.reg_match->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_match->rm_matchcol = col;
	}
    }

    પાછા retval;
}

/*
 * Compile a regular expression into internal code for the NFA matcher.
 * Returns the program in allocated space.  Returns NULL for an error.
 */
    સ્થિર regprog_T *
nfa_regcomp(char_u *પદાવલિ, પૂર્ણાંક re_flags)
{
    nfa_regprog_T	*prog = ખાલી;
    size_t		prog_size;
    પૂર્ણાંક			*postfix;

    જો (પદાવલિ == ખાલી)
	પાછા ખાલી;

#ifdef DEBUG
    nfa_regengine.પદાવલિ = પદાવલિ;
#endif
    nfa_re_flags = re_flags;

    init_class_tab();

    જો (nfa_regcomp_start(પદાવલિ, re_flags) == FAIL)
	પાછા ખાલી;

    // Build postfix form of the regexp. Needed to build the NFA
    // (and count its size).
    postfix = re2post();
    જો (postfix == ખાલી)
	જાઓ fail;	    // Cascaded (syntax?) error

    /*
     * In order to build the NFA, we parse the input regexp twice:
     * 1. first pass to count size (so we can allocate space)
     * 2. second to emit code
     */
#ifdef ENABLE_LOG
    {
	FILE *f = fopen(NFA_REGEXP_RUN_LOG, "a");

	જો (f != ખાલી)
	{
	    fprintf(f, "\n*****************************\n\n\n\n\tCompiling regexp \"%s\"... hold on !\n", પદાવલિ);
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
    prog_size = offsetof(nfa_regprog_T, state) + કદ(nfa_state_T) * nstate;
    prog = alloc(prog_size);
    જો (prog == ખાલી)
	જાઓ fail;
    state_ptr = prog->state;
    prog->re_in_use = FALSE;
    prog->listbuf[0] = ખાલી;
    prog->listbuf[1] = ખાલી;

    /*
     * PASS 2
     * Build the NFA
     */
    prog->start = post2nfa(postfix, post_ptr, FALSE);
    જો (prog->start == ખાલી)
	જાઓ fail;

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
    nfa_postfix_dump(પદાવલિ, OK);
    nfa_dump(prog);
#endif
#ifdef FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    prog->reghasz = re_has_z;
#endif
    prog->pattern = vim_strsave(પદાવલિ);
#ifdef DEBUG
    nfa_regengine.પદાવલિ = ખાલી;
#endif

out:
    // The postfix list and fragment stack are reused by the next compilation
    // (and freed in free_regexp_stuff()), but drop ones that grew large for a
    // big pattern to avoid holding much memory.
    જો (post_start_len > NFA_POSTFIX_KEEP)
    {
	VIM_CLEAR(post_start);
	post_start_len = 0;
    }
    post_ptr = post_end = ખાલી;
    જો (nfa_ઢગલો_len > NFA_ઢગલો_KEEP)
    {
	VIM_CLEAR(nfa_ઢગલો);
	nfa_ઢગલો_len = 0;
    }
    state_ptr = ખાલી;
    પાછા (regprog_T *)prog;

fail:
    VIM_CLEAR(prog);
#ifdef ENABLE_LOG
    nfa_postfix_dump(પદાવલિ, FAIL);
#endif
#ifdef DEBUG
    nfa_regengine.પદાવલિ = ખાલી;
#endif
    જાઓ out;
}

/*
 * Free a compiled regexp program, returned by nfa_regcomp().
 */
    સ્થિર શૂન્ય
nfa_regfree(regprog_T *prog)
{
    જો (prog == ખાલી)
	પાછા;

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
    સ્થિર પૂર્ણાંક
nfa_regexec_nl(
    regmatch_T	*rmp,
    char_u	*લીટી,	// string to match against
    colnr_T	col,	// column to start looking for match
    પૂર્ણાંક		લીટી_lbr)
{
    rex.reg_match = rmp;
    rex.reg_mmatch = ખાલી;
    rex.reg_maxline = 0;
    rex.reg_લીટી_lbr = લીટી_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = ખાલી;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;
    પાછા nfa_regexec_both(લીટી, col, ખાલી);
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
    સ્થિર long
nfa_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	col,		// column to start looking for match
    પૂર્ણાંક		*timed_out)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    પાછા nfa_regexec_both(ખાલી, col, timed_out);
}

#ifdef DEBUG
# undef ENABLE_LOG
#endif
