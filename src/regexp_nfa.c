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

गणना
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
स्थिर ध्रुव पूर्णाङ्क nfa_classcodes[] = {
    NFA_ANY, NFA_IDENT, NFA_SIDENT, NFA_KWORD,NFA_SKWORD,
    NFA_FNAME, NFA_SFNAME, NFA_PRINT, NFA_SPRINT,
    NFA_WHITE, NFA_NWHITE, NFA_DIGIT, NFA_NDIGIT,
    NFA_HEX, NFA_NHEX, NFA_OCTAL, NFA_NOCTAL,
    NFA_WORD, NFA_NWORD, NFA_HEAD, NFA_NHEAD,
    NFA_ALPHA, NFA_NALPHA, NFA_LOWER, NFA_NLOWER,
    NFA_UPPER, NFA_NUPPER
};

// Variables only used in nfa_regcomp() and descendants.
स्थिर पूर्णाङ्क nfa_re_flags; // re_flags passed to nfa_regcomp()
स्थिर पूर्णाङ्क *post_start;  // holds the postfix form of r.e.
स्थिर पूर्णाङ्क post_start_len;  // size of allocated post_start (in ints)
स्थिर पूर्णाङ्क *post_end;
स्थिर पूर्णाङ्क *post_ptr;
स्थिर पूर्णाङ्क nfa_reg_parse_depth;	// nesting depth in nfa_reg()

// The postfix list (post_start) and fragment stack (nfa_stack) are reused
// across compilations; a buffer that grew past these sizes for a big pattern
// is freed afterwards instead of being kept, to avoid holding much memory.
#define NFA_POSTFIX_KEEP	10000	// number of ints (~40 Kbyte)
#define NFA_चळत_KEEP		4000	// number of Frag_T (~64 Kbyte)

// Set when the pattern should use the NFA engine.
// E.g. [[:upper:]] only allows 8bit characters for BT engine,
// while NFA engine handles multibyte characters correctly.
स्थिर पूर्णाङ्क wants_nfa;

स्थिर पूर्णाङ्क nstate;	// Number of states in the NFA.
स्थिर पूर्णाङ्क istate;	// Index in the state vector, used in alloc_state()

// If not NULL match must end at this position
स्थिर save_se_T *nfa_endp = रिक्त;

// 0 for first call to nfa_regmatch(), 1 for recursive call.
स्थिर पूर्णाङ्क nfa_ll_index = 0;

स्थिर पूर्णाङ्क realloc_post_list(शून्य);
स्थिर पूर्णाङ्क nfa_reg(पूर्णाङ्क paren);
#ifdef DEBUG
स्थिर शून्य nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent);
#endif
स्थिर पूर्णाङ्क match_follows(nfa_state_T *startstate, पूर्णाङ्क depth);
स्थिर पूर्णाङ्क failure_chance(nfa_state_T *state, पूर्णाङ्क depth);

// helper functions used when doing re2post() ... regatom() parsing
#define EMIT(c)	कुरु {				\
		    यदि (post_ptr >= post_end && realloc_post_list() == FAIL) \
			प्रत्यागम FAIL;		\
		    *post_ptr++ = c;		\
		} यावत् (0)

/*
 * Initialize internal variables before NFA compilation.
 * Return OK on success, FAIL otherwise.
 */
    स्थिर पूर्णाङ्क
nfa_regcomp_start(
    char_u	*अभिव्यक्ती,
    पूर्णाङ्क		re_flags)	    // see vim_regcomp()
{
    size_t	postfix_size;
    पूर्णाङ्क		nstate_max;

    nstate = 0;
    istate = 0;
    // A reasonable estimation for maximum size
    nstate_max = (पूर्णाङ्क)(STRLEN(अभिव्यक्ती) + 1) * 25;

    // Some items blow up in size, such as [A-z].  Add more space for that.
    // When it is still not enough realloc_post_list() will be used.
    nstate_max += 1000;

    // Size for postfix representation of expr.
    postfix_size = परिमाण(पूर्णाङ्क) * nstate_max;

    // Reuse the postfix buffer across compilations, only growing it when the
    // estimate exceeds the current size; it is freed in free_regexp_stuff().
    यदि (post_start == रिक्त || post_start_len < nstate_max)
    {
	vim_free(post_start);
	post_start = alloc(postfix_size);
	यदि (post_start == रिक्त)
	{
	    post_start_len = 0;
	    प्रत्यागम FAIL;
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
    regcomp_start(अभिव्यक्ती, re_flags);

    प्रत्यागम OK;
}

/*
 * Figure out if the NFA state list starts with an anchor, must match at start
 * of the line.
 */
    स्थिर पूर्णाङ्क
nfa_get_reganch(nfa_state_T *start, पूर्णाङ्क depth)
{
    nfa_state_T *p = start;

    यदि (depth > 4)
	प्रत्यागम 0;

    यावत् (p != रिक्त)
    {
	विकल्प (p->c)
	{
	    पक्ष NFA_BOL:
	    पक्ष NFA_BOF:
		प्रत्यागम 1; // yes!

	    पक्ष NFA_ZSTART:
	    पक्ष NFA_ZEND:
	    पक्ष NFA_CURSOR:
	    पक्ष NFA_VISUAL:

	    पक्ष NFA_MOPEN:
	    पक्ष NFA_MOPEN1:
	    पक्ष NFA_MOPEN2:
	    पक्ष NFA_MOPEN3:
	    पक्ष NFA_MOPEN4:
	    पक्ष NFA_MOPEN5:
	    पक्ष NFA_MOPEN6:
	    पक्ष NFA_MOPEN7:
	    पक्ष NFA_MOPEN8:
	    पक्ष NFA_MOPEN9:
	    पक्ष NFA_NOPEN:
#ifdef FEAT_SYN_HL
	    पक्ष NFA_ZOPEN:
	    पक्ष NFA_ZOPEN1:
	    पक्ष NFA_ZOPEN2:
	    पक्ष NFA_ZOPEN3:
	    पक्ष NFA_ZOPEN4:
	    पक्ष NFA_ZOPEN5:
	    पक्ष NFA_ZOPEN6:
	    पक्ष NFA_ZOPEN7:
	    पक्ष NFA_ZOPEN8:
	    पक्ष NFA_ZOPEN9:
#endif
		p = p->out;
		भङ्ग;

	    पक्ष NFA_SPLIT:
		प्रत्यागम nfa_get_reganch(p->out, depth + 1)
				       && nfa_get_reganch(p->out1, depth + 1);

	    स्वभाव:
		प्रत्यागम 0; // noooo
	}
    }
    प्रत्यागम 0;
}

/*
 * Figure out if the NFA state list starts with a character which must match
 * at start of the match.
 */
    स्थिर पूर्णाङ्क
nfa_get_regstart(nfa_state_T *start, पूर्णाङ्क depth)
{
    nfa_state_T *p = start;

    यदि (depth > 4)
	प्रत्यागम 0;

    यावत् (p != रिक्त)
    {
	विकल्प (p->c)
	{
	    // all kinds of zero-width matches
	    पक्ष NFA_BOL:
	    पक्ष NFA_BOF:
	    पक्ष NFA_BOW:
	    पक्ष NFA_EOW:
	    पक्ष NFA_ZSTART:
	    पक्ष NFA_ZEND:
	    पक्ष NFA_CURSOR:
	    पक्ष NFA_VISUAL:
	    पक्ष NFA_LNUM:
	    पक्ष NFA_LNUM_GT:
	    पक्ष NFA_LNUM_LT:
	    पक्ष NFA_COL:
	    पक्ष NFA_COL_GT:
	    पक्ष NFA_COL_LT:
	    पक्ष NFA_VCOL:
	    पक्ष NFA_VCOL_GT:
	    पक्ष NFA_VCOL_LT:
	    पक्ष NFA_MARK:
	    पक्ष NFA_MARK_GT:
	    पक्ष NFA_MARK_LT:

	    पक्ष NFA_MOPEN:
	    पक्ष NFA_MOPEN1:
	    पक्ष NFA_MOPEN2:
	    पक्ष NFA_MOPEN3:
	    पक्ष NFA_MOPEN4:
	    पक्ष NFA_MOPEN5:
	    पक्ष NFA_MOPEN6:
	    पक्ष NFA_MOPEN7:
	    पक्ष NFA_MOPEN8:
	    पक्ष NFA_MOPEN9:
	    पक्ष NFA_NOPEN:
#ifdef FEAT_SYN_HL
	    पक्ष NFA_ZOPEN:
	    पक्ष NFA_ZOPEN1:
	    पक्ष NFA_ZOPEN2:
	    पक्ष NFA_ZOPEN3:
	    पक्ष NFA_ZOPEN4:
	    पक्ष NFA_ZOPEN5:
	    पक्ष NFA_ZOPEN6:
	    पक्ष NFA_ZOPEN7:
	    पक्ष NFA_ZOPEN8:
	    पक्ष NFA_ZOPEN9:
#endif
		p = p->out;
		भङ्ग;

	    पक्ष NFA_SPLIT:
	    {
		पूर्णाङ्क c1 = nfa_get_regstart(p->out, depth + 1);
		पूर्णाङ्क c2 = nfa_get_regstart(p->out1, depth + 1);

		यदि (c1 == c2)
		    प्रत्यागम c1; // yes!
		प्रत्यागम 0;
	    }

	    स्वभाव:
		यदि (p->c > 0)
		    प्रत्यागम p->c; // yes!
		प्रत्यागम 0;
	}
    }
    प्रत्यागम 0;
}

/*
 * Figure out if the NFA state list contains just literal text and nothing
 * else.  If so return a string in allocated memory with what must match after
 * regstart.  Otherwise return NULL.
 */
    स्थिर char_u *
nfa_get_match_text(nfa_state_T *start)
{
    nfa_state_T *p = start;
    पूर्णाङ्क		len = 0;
    char_u	*ret;
    char_u	*s;

    यदि (p->c != NFA_MOPEN)
	प्रत्यागम रिक्त; // just in case
    p = p->out;
    यावत् (p->c > 0)
    {
	len += MB_CHAR2LEN(p->c);
	p = p->out;
    }
    यदि (p->c != NFA_MCLOSE || p->out->c != NFA_MATCH)
	प्रत्यागम रिक्त;

    ret = alloc(len);
    यदि (ret == रिक्त)
	प्रत्यागम रिक्त;

    p = start->out->out; // skip first char, it goes into regstart
    s = ret;
    यावत् (p->c > 0)
    {
	यदि (has_mbyte)
	    s += (*mb_char2bytes)(p->c, s);
	अन्यथा
	    *s++ = p->c;
	p = p->out;
    }
    *s = NUL;
    प्रत्यागम ret;
}

/*
 * Allocate more space for post_start.  Called when
 * running above the estimated number of states.
 */
    स्थिर पूर्णाङ्क
realloc_post_list(शून्य)
{
    पूर्णाङ्क   nstate_max = (पूर्णाङ्क)(post_end - post_start);
    पूर्णाङ्क   new_max;
    पूर्णाङ्क   *new_start;
    पूर्णाङ्क	  *old_start;

    // For weird patterns the number of states can be very high. Increasing by
    // 50% seems a reasonable compromise between memory use and speed.
    new_max = nstate_max * 3 / 2;
    new_start = ALLOC_MULT(पूर्णाङ्क, new_max);
    यदि (new_start == रिक्त)
	प्रत्यागम FAIL;
    mch_memmove(new_start, post_start, nstate_max * परिमाण(पूर्णाङ्क));
    old_start = post_start;
    post_start = new_start;
    post_start_len = new_max;
    post_ptr = new_start + (post_ptr - old_start);
    post_end = post_start + new_max;
    vim_free(old_start);
    प्रत्यागम OK;
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
    स्थिर पूर्णाङ्क
nfa_recognize_char_class(char_u *start, char_u *end, पूर्णाङ्क extra_newl)
{
    पूर्णाङ्क		newl;
    पूर्णाङ्क		config = get_char_class_bits(start, end, &newl);

    यदि (config < 0)
	प्रत्यागम FAIL;

    यदि (extra_newl == TRUE)
	newl = TRUE;

    यदि (newl == TRUE)
	extra_newl = NFA_ADD_NL;

    विकल्प (config)
    {
	पक्ष CLASS_o9:
	    प्रत्यागम extra_newl + NFA_DIGIT;
	पक्ष CLASS_not |  CLASS_o9:
	    प्रत्यागम extra_newl + NFA_NDIGIT;
	पक्ष CLASS_af | CLASS_AF | CLASS_o9:
	    प्रत्यागम extra_newl + NFA_HEX;
	पक्ष CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    प्रत्यागम extra_newl + NFA_NHEX;
	पक्ष CLASS_o7:
	    प्रत्यागम extra_newl + NFA_OCTAL;
	पक्ष CLASS_not | CLASS_o7:
	    प्रत्यागम extra_newl + NFA_NOCTAL;
	पक्ष CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    प्रत्यागम extra_newl + NFA_WORD;
	पक्ष CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    प्रत्यागम extra_newl + NFA_NWORD;
	पक्ष CLASS_az | CLASS_AZ | CLASS_underscore:
	    प्रत्यागम extra_newl + NFA_HEAD;
	पक्ष CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    प्रत्यागम extra_newl + NFA_NHEAD;
	पक्ष CLASS_az | CLASS_AZ:
	    प्रत्यागम extra_newl + NFA_ALPHA;
	पक्ष CLASS_not | CLASS_az | CLASS_AZ:
	    प्रत्यागम extra_newl + NFA_NALPHA;
	पक्ष CLASS_az:
	   प्रत्यागम extra_newl + NFA_LOWER_IC;
	पक्ष CLASS_not | CLASS_az:
	    प्रत्यागम extra_newl + NFA_NLOWER_IC;
	पक्ष CLASS_AZ:
	    प्रत्यागम extra_newl + NFA_UPPER_IC;
	पक्ष CLASS_not | CLASS_AZ:
	    प्रत्यागम extra_newl + NFA_NUPPER_IC;
    }
    प्रत्यागम FAIL;
}

/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * Emits bytes in postfix notation: 'a,b,NFA_OR,c,NFA_OR' is
 * equivalent to 'a OR b OR c'
 *
 * NOTE! When changing this function, also update reg_equi_class()
 */
    स्थिर पूर्णाङ्क
nfa_emit_equi_class(पूर्णाङ्क c)
{
#define EMIT2(c)    EMIT(c); EMIT(NFA_CONCAT);

    यदि (enc_utf8 || STRCMP(p_enc, "latin1") == 0
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
	विकल्प (c)
	{
	    पक्ष 'A': पक्ष A_grave: पक्ष A_acute: पक्ष A_circumflex:
	    पक्ष A_virguilla: पक्ष A_diaeresis: पक्ष A_ring:
	    पक्ष 0x100: पक्ष 0x102: पक्ष 0x104: पक्ष 0x1cd:
	    पक्ष 0x1de: पक्ष 0x1e0: पक्ष 0x1fa: पक्ष 0x200:
	    पक्ष 0x202: पक्ष 0x226: पक्ष 0x23a: पक्ष 0x1e00:
	    पक्ष 0x1ea0: पक्ष 0x1ea2: पक्ष 0x1ea4: पक्ष 0x1ea6:
	    पक्ष 0x1ea8: पक्ष 0x1eaa: पक्ष 0x1eac: पक्ष 0x1eae:
	    पक्ष 0x1eb0: पक्ष 0x1eb2: पक्ष 0x1eb4: पक्ष 0x1eb6:
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
		    प्रत्यागम OK;

	    पक्ष 'B': पक्ष 0x181: पक्ष 0x243: पक्ष 0x1e02:
	    पक्ष 0x1e04: पक्ष 0x1e06:
		    EMIT2('B')
		    EMIT2(0x181) EMIT2(0x243) EMIT2(0x1e02)
		    EMIT2(0x1e04) EMIT2(0x1e06)
		    प्रत्यागम OK;

	    पक्ष 'C': पक्ष C_cedilla: पक्ष 0x106: पक्ष 0x108:
	    पक्ष 0x10a: पक्ष 0x10c: पक्ष 0x187: पक्ष 0x23b:
	    पक्ष 0x1e08: पक्ष 0xa792:
		    EMIT2('C') EMIT2(C_cedilla)
		    EMIT2(0x106) EMIT2(0x108) EMIT2(0x10a)
		    EMIT2(0x10c) EMIT2(0x187) EMIT2(0x23b)
		    EMIT2(0x1e08) EMIT2(0xa792)
		    प्रत्यागम OK;

	    पक्ष 'D': पक्ष 0x10e: पक्ष 0x110: पक्ष 0x18a:
	    पक्ष 0x1e0a: पक्ष 0x1e0c: पक्ष 0x1e0e: पक्ष 0x1e10:
	    पक्ष 0x1e12:
		    EMIT2('D') EMIT2(0x10e) EMIT2(0x110) EMIT2(0x18a)
		    EMIT2(0x1e0a) EMIT2(0x1e0c) EMIT2(0x1e0e)
		    EMIT2(0x1e10) EMIT2(0x1e12)
		    प्रत्यागम OK;

	    पक्ष 'E': पक्ष E_grave: पक्ष E_acute: पक्ष E_circumflex:
	    पक्ष E_diaeresis: पक्ष 0x112: पक्ष 0x114: पक्ष 0x116:
	    पक्ष 0x118: पक्ष 0x11a: पक्ष 0x204: पक्ष 0x206:
	    पक्ष 0x228: पक्ष 0x246: पक्ष 0x1e14: पक्ष 0x1e16:
	    पक्ष 0x1e18: पक्ष 0x1e1a: पक्ष 0x1e1c: पक्ष 0x1eb8:
	    पक्ष 0x1eba: पक्ष 0x1ebc: पक्ष 0x1ebe: पक्ष 0x1ec0:
	    पक्ष 0x1ec2: पक्ष 0x1ec4: पक्ष 0x1ec6:
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
		    प्रत्यागम OK;

	    पक्ष 'F': पक्ष 0x191: पक्ष 0x1e1e: पक्ष 0xa798:
		    EMIT2('F') EMIT2(0x191) EMIT2(0x1e1e) EMIT2(0xa798)
		    प्रत्यागम OK;

	    पक्ष 'G': पक्ष 0x11c: पक्ष 0x11e: पक्ष 0x120:
	    पक्ष 0x122: पक्ष 0x193: पक्ष 0x1e4: पक्ष 0x1e6:
	    पक्ष 0x1f4: पक्ष 0x1e20: पक्ष 0xa7a0:
		    EMIT2('G') EMIT2(0x11c) EMIT2(0x11e) EMIT2(0x120)
		    EMIT2(0x122) EMIT2(0x193) EMIT2(0x1e4)
		    EMIT2(0x1e6) EMIT2(0x1f4) EMIT2(0x1e20)
		    EMIT2(0xa7a0)
		    प्रत्यागम OK;

	    पक्ष 'H': पक्ष 0x124: पक्ष 0x126: पक्ष 0x21e:
	    पक्ष 0x1e22: पक्ष 0x1e24: पक्ष 0x1e26: पक्ष 0x1e28:
	    पक्ष 0x1e2a: पक्ष 0x2c67:
		    EMIT2('H') EMIT2(0x124) EMIT2(0x126) EMIT2(0x21e)
		    EMIT2(0x1e22) EMIT2(0x1e24) EMIT2(0x1e26)
		    EMIT2(0x1e28) EMIT2(0x1e2a) EMIT2(0x2c67)
		    प्रत्यागम OK;

	    पक्ष 'I': पक्ष I_grave: पक्ष I_acute: पक्ष I_circumflex:
	    पक्ष I_diaeresis: पक्ष 0x128: पक्ष 0x12a: पक्ष 0x12c:
	    पक्ष 0x12e: पक्ष 0x130: पक्ष 0x197: पक्ष 0x1cf:
	    पक्ष 0x208: पक्ष 0x20a: पक्ष 0x1e2c: पक्ष 0x1e2e:
	    पक्ष 0x1ec8: पक्ष 0x1eca:
		    EMIT2('I') EMIT2(I_grave) EMIT2(I_acute)
		    EMIT2(I_circumflex) EMIT2(I_diaeresis)
		    EMIT2(0x128) EMIT2(0x12a) EMIT2(0x12c)
		    EMIT2(0x12e) EMIT2(0x130) EMIT2(0x197)
		    EMIT2(0x1cf) EMIT2(0x208) EMIT2(0x20a)
		    EMIT2(0x1e2c) EMIT2(0x1e2e) EMIT2(0x1ec8)
		    EMIT2(0x1eca)
		    प्रत्यागम OK;

	    पक्ष 'J': पक्ष 0x134: पक्ष 0x248:
		    EMIT2('J') EMIT2(0x134) EMIT2(0x248)
		    प्रत्यागम OK;

	    पक्ष 'K': पक्ष 0x136: पक्ष 0x198: पक्ष 0x1e8: पक्ष 0x1e30:
	    पक्ष 0x1e32: पक्ष 0x1e34: पक्ष 0x2c69: पक्ष 0xa740:
		    EMIT2('K') EMIT2(0x136) EMIT2(0x198) EMIT2(0x1e8)
		    EMIT2(0x1e30) EMIT2(0x1e32) EMIT2(0x1e34)
		    EMIT2(0x2c69) EMIT2(0xa740)
		    प्रत्यागम OK;

	    पक्ष 'L': पक्ष 0x139: पक्ष 0x13b: पक्ष 0x13d:
	    पक्ष 0x13f: पक्ष 0x141: पक्ष 0x23d: पक्ष 0x1e36:
	    पक्ष 0x1e38: पक्ष 0x1e3a: पक्ष 0x1e3c: पक्ष 0x2c60:
		    EMIT2('L') EMIT2(0x139) EMIT2(0x13b)
		    EMIT2(0x13d) EMIT2(0x13f) EMIT2(0x141)
		    EMIT2(0x23d) EMIT2(0x1e36) EMIT2(0x1e38)
		    EMIT2(0x1e3a) EMIT2(0x1e3c) EMIT2(0x2c60)
		    प्रत्यागम OK;

	    पक्ष 'M': पक्ष 0x1e3e: पक्ष 0x1e40: पक्ष 0x1e42:
		    EMIT2('M') EMIT2(0x1e3e) EMIT2(0x1e40)
		    EMIT2(0x1e42)
		    प्रत्यागम OK;

	    पक्ष 'N': पक्ष N_virguilla:
	    पक्ष 0x143: पक्ष 0x145: पक्ष 0x147: पक्ष 0x1f8:
	    पक्ष 0x1e44: पक्ष 0x1e46: पक्ष 0x1e48: पक्ष 0x1e4a:
	    पक्ष 0xa7a4:
		    EMIT2('N') EMIT2(N_virguilla)
		    EMIT2(0x143) EMIT2(0x145) EMIT2(0x147)
		    EMIT2(0x1f8) EMIT2(0x1e44) EMIT2(0x1e46)
		    EMIT2(0x1e48) EMIT2(0x1e4a) EMIT2(0xa7a4)
		    प्रत्यागम OK;

	    पक्ष 'O': पक्ष O_grave: पक्ष O_acute: पक्ष O_circumflex:
	    पक्ष O_virguilla: पक्ष O_diaeresis: पक्ष O_slash:
	    पक्ष 0x14c: पक्ष 0x14e: पक्ष 0x150: पक्ष 0x19f:
	    पक्ष 0x1a0: पक्ष 0x1d1: पक्ष 0x1ea: पक्ष 0x1ec:
	    पक्ष 0x1fe: पक्ष 0x20c: पक्ष 0x20e: पक्ष 0x22a:
	    पक्ष 0x22c: पक्ष 0x22e: पक्ष 0x230: पक्ष 0x1e4c:
	    पक्ष 0x1e4e: पक्ष 0x1e50: पक्ष 0x1e52: पक्ष 0x1ecc:
	    पक्ष 0x1ece: पक्ष 0x1ed0: पक्ष 0x1ed2: पक्ष 0x1ed4:
	    पक्ष 0x1ed6: पक्ष 0x1ed8: पक्ष 0x1eda: पक्ष 0x1edc:
	    पक्ष 0x1ede: पक्ष 0x1ee0: पक्ष 0x1ee2:
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
		    प्रत्यागम OK;

	    पक्ष 'P': पक्ष 0x1a4: पक्ष 0x1e54: पक्ष 0x1e56: पक्ष 0x2c63:
		    EMIT2('P') EMIT2(0x1a4) EMIT2(0x1e54) EMIT2(0x1e56)
		    EMIT2(0x2c63)
		    प्रत्यागम OK;

	    पक्ष 'Q': पक्ष 0x24a:
		    EMIT2('Q') EMIT2(0x24a)
		    प्रत्यागम OK;

	    पक्ष 'R': पक्ष 0x154: पक्ष 0x156: पक्ष 0x158: पक्ष 0x210:
	    पक्ष 0x212: पक्ष 0x24c: पक्ष 0x1e58: पक्ष 0x1e5a:
	    पक्ष 0x1e5c: पक्ष 0x1e5e: पक्ष 0x2c64: पक्ष 0xa7a6:
		    EMIT2('R') EMIT2(0x154) EMIT2(0x156) EMIT2(0x158)
		    EMIT2(0x210) EMIT2(0x212) EMIT2(0x24c) EMIT2(0x1e58)
		    EMIT2(0x1e5a) EMIT2(0x1e5c) EMIT2(0x1e5e) EMIT2(0x2c64)
		    EMIT2(0xa7a6)
		    प्रत्यागम OK;

	    पक्ष 'S': पक्ष 0x15a: पक्ष 0x15c: पक्ष 0x15e: पक्ष 0x160:
	    पक्ष 0x218: पक्ष 0x1e60: पक्ष 0x1e62: पक्ष 0x1e64:
	    पक्ष 0x1e66: पक्ष 0x1e68: पक्ष 0x2c7e: पक्ष 0xa7a8:
		    EMIT2('S') EMIT2(0x15a) EMIT2(0x15c) EMIT2(0x15e)
		    EMIT2(0x160) EMIT2(0x218) EMIT2(0x1e60) EMIT2(0x1e62)
		    EMIT2(0x1e64) EMIT2(0x1e66) EMIT2(0x1e68) EMIT2(0x2c7e)
		    EMIT2(0xa7a8)
		    प्रत्यागम OK;

	    पक्ष 'T': पक्ष 0x162: पक्ष 0x164: पक्ष 0x166: पक्ष 0x1ac:
	    पक्ष 0x1ae: पक्ष 0x21a: पक्ष 0x23e: पक्ष 0x1e6a: पक्ष 0x1e6c:
	    पक्ष 0x1e6e: पक्ष 0x1e70:
		    EMIT2('T') EMIT2(0x162) EMIT2(0x164) EMIT2(0x166)
		    EMIT2(0x1ac) EMIT2(0x1ae) EMIT2(0x23e) EMIT2(0x21a)
		    EMIT2(0x1e6a) EMIT2(0x1e6c) EMIT2(0x1e6e) EMIT2(0x1e70)
		    प्रत्यागम OK;

	    पक्ष 'U': पक्ष U_grave: पक्ष U_acute: पक्ष U_diaeresis:
	    पक्ष U_circumflex: पक्ष 0x168: पक्ष 0x16a: पक्ष 0x16c:
	    पक्ष 0x16e: पक्ष 0x170: पक्ष 0x172: पक्ष 0x1af:
	    पक्ष 0x1d3: पक्ष 0x1d5: पक्ष 0x1d7: पक्ष 0x1d9:
	    पक्ष 0x1db: पक्ष 0x214: पक्ष 0x216: पक्ष 0x244:
	    पक्ष 0x1e72: पक्ष 0x1e74: पक्ष 0x1e76: पक्ष 0x1e78:
	    पक्ष 0x1e7a: पक्ष 0x1ee4: पक्ष 0x1ee6: पक्ष 0x1ee8:
	    पक्ष 0x1eea: पक्ष 0x1eec: पक्ष 0x1eee: पक्ष 0x1ef0:
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
		    प्रत्यागम OK;

	    पक्ष 'V': पक्ष 0x1b2: पक्ष 0x1e7c: पक्ष 0x1e7e:
		    EMIT2('V') EMIT2(0x1b2) EMIT2(0x1e7c) EMIT2(0x1e7e)
		    प्रत्यागम OK;

	    पक्ष 'W': पक्ष 0x174: पक्ष 0x1e80: पक्ष 0x1e82: पक्ष 0x1e84:
	    पक्ष 0x1e86: पक्ष 0x1e88:
		    EMIT2('W') EMIT2(0x174) EMIT2(0x1e80) EMIT2(0x1e82)
		    EMIT2(0x1e84) EMIT2(0x1e86) EMIT2(0x1e88)
		    प्रत्यागम OK;

	    पक्ष 'X': पक्ष 0x1e8a: पक्ष 0x1e8c:
		    EMIT2('X') EMIT2(0x1e8a) EMIT2(0x1e8c)
		    प्रत्यागम OK;

	    पक्ष 'Y': पक्ष Y_acute: पक्ष 0x176: पक्ष 0x178:
	    पक्ष 0x1b3: पक्ष 0x232: पक्ष 0x24e: पक्ष 0x1e8e:
	    पक्ष 0x1ef2: पक्ष 0x1ef4: पक्ष 0x1ef6: पक्ष 0x1ef8:
		    EMIT2('Y') EMIT2(Y_acute)
		    EMIT2(0x176) EMIT2(0x178) EMIT2(0x1b3)
		    EMIT2(0x232) EMIT2(0x24e) EMIT2(0x1e8e)
		    EMIT2(0x1ef2) EMIT2(0x1ef4) EMIT2(0x1ef6)
		    EMIT2(0x1ef8)
		    प्रत्यागम OK;

	    पक्ष 'Z': पक्ष 0x179: पक्ष 0x17b: पक्ष 0x17d:
	    पक्ष 0x1b5: पक्ष 0x1e90: पक्ष 0x1e92: पक्ष 0x1e94:
	    पक्ष 0x2c6b:
		    EMIT2('Z') EMIT2(0x179) EMIT2(0x17b) EMIT2(0x17d)
		    EMIT2(0x1b5) EMIT2(0x1e90) EMIT2(0x1e92)
		    EMIT2(0x1e94) EMIT2(0x2c6b)
		    प्रत्यागम OK;

	    पक्ष 'a': पक्ष a_grave: पक्ष a_acute: पक्ष a_circumflex:
	    पक्ष a_virguilla: पक्ष a_diaeresis: पक्ष a_ring:
	    पक्ष 0x101: पक्ष 0x103: पक्ष 0x105: पक्ष 0x1ce:
	    पक्ष 0x1df: पक्ष 0x1e1: पक्ष 0x1fb: पक्ष 0x201:
	    पक्ष 0x203: पक्ष 0x227: पक्ष 0x1d8f: पक्ष 0x1e01:
	    पक्ष 0x1e9a: पक्ष 0x1ea1: पक्ष 0x1ea3: पक्ष 0x1ea5:
	    पक्ष 0x1ea7: पक्ष 0x1ea9: पक्ष 0x1eab: पक्ष 0x1ead:
	    पक्ष 0x1eaf: पक्ष 0x1eb1: पक्ष 0x1eb3: पक्ष 0x1eb5:
	    पक्ष 0x1eb7: पक्ष 0x2c65:
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
		    प्रत्यागम OK;

	    पक्ष 'b': पक्ष 0x180: पक्ष 0x253: पक्ष 0x1d6c: पक्ष 0x1d80:
	    पक्ष 0x1e03: पक्ष 0x1e05: पक्ष 0x1e07:
		    EMIT2('b') EMIT2(0x180) EMIT2(0x253) EMIT2(0x1d6c)
		    EMIT2(0x1d80) EMIT2(0x1e03) EMIT2(0x1e05) EMIT2(0x1e07)
		    प्रत्यागम OK;

	    पक्ष 'c': पक्ष c_cedilla: पक्ष 0x107: पक्ष 0x109: पक्ष 0x10b:
	    पक्ष 0x10d: पक्ष 0x188: पक्ष 0x23c: पक्ष 0x1e09: पक्ष 0xa793:
	    पक्ष 0xa794:
		    EMIT2('c') EMIT2(c_cedilla)
		    EMIT2(0x107) EMIT2(0x109) EMIT2(0x10b)
		    EMIT2(0x10d) EMIT2(0x188) EMIT2(0x23c)
		    EMIT2(0x1e09) EMIT2(0xa793) EMIT2(0xa794)
		    प्रत्यागम OK;

	    पक्ष 'd': पक्ष 0x10f: पक्ष 0x111: पक्ष 0x257: पक्ष 0x1d6d:
	    पक्ष 0x1d81: पक्ष 0x1d91: पक्ष 0x1e0b: पक्ष 0x1e0d: पक्ष 0x1e0f:
	    पक्ष 0x1e11: पक्ष 0x1e13:
		    EMIT2('d') EMIT2(0x10f) EMIT2(0x111)
		    EMIT2(0x257) EMIT2(0x1d6d) EMIT2(0x1d81)
		    EMIT2(0x1d91) EMIT2(0x1e0b) EMIT2(0x1e0d)
		    EMIT2(0x1e0f) EMIT2(0x1e11) EMIT2(0x1e13)
		    प्रत्यागम OK;

	    पक्ष 'e': पक्ष e_grave: पक्ष e_acute: पक्ष e_circumflex:
	    पक्ष e_diaeresis: पक्ष 0x113: पक्ष 0x115: पक्ष 0x117:
	    पक्ष 0x119: पक्ष 0x11b: पक्ष 0x205: पक्ष 0x207:
	    पक्ष 0x229: पक्ष 0x247: पक्ष 0x1d92: पक्ष 0x1e15:
	    पक्ष 0x1e17: पक्ष 0x1e19: पक्ष 0x1e1b: पक्ष 0x1e1d:
	    पक्ष 0x1eb9: पक्ष 0x1ebb: पक्ष 0x1ebd: पक्ष 0x1ebf:
	    पक्ष 0x1ec1: पक्ष 0x1ec3: पक्ष 0x1ec5: पक्ष 0x1ec7:
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
		    प्रत्यागम OK;

	    पक्ष 'f': पक्ष 0x192: पक्ष 0x1d6e: पक्ष 0x1d82:
	    पक्ष 0x1e1f: पक्ष 0xa799:
		    EMIT2('f') EMIT2(0x192) EMIT2(0x1d6e) EMIT2(0x1d82)
		    EMIT2(0x1e1f) EMIT2(0xa799)
		    प्रत्यागम OK;

	    पक्ष 'g': पक्ष 0x11d: पक्ष 0x11f: पक्ष 0x121: पक्ष 0x123:
	    पक्ष 0x1e5: पक्ष 0x1e7: पक्ष 0x1f5: पक्ष 0x260: पक्ष 0x1d83:
	    पक्ष 0x1e21: पक्ष 0xa7a1:
		    EMIT2('g') EMIT2(0x11d) EMIT2(0x11f) EMIT2(0x121)
		    EMIT2(0x123) EMIT2(0x1e5) EMIT2(0x1e7)
		    EMIT2(0x1f5) EMIT2(0x260) EMIT2(0x1d83)
		    EMIT2(0x1e21) EMIT2(0xa7a1)
		    प्रत्यागम OK;

	    पक्ष 'h': पक्ष 0x125: पक्ष 0x127: पक्ष 0x21f: पक्ष 0x1e23:
	    पक्ष 0x1e25: पक्ष 0x1e27: पक्ष 0x1e29: पक्ष 0x1e2b:
	    पक्ष 0x1e96: पक्ष 0x2c68: पक्ष 0xa795:
		    EMIT2('h') EMIT2(0x125) EMIT2(0x127) EMIT2(0x21f)
		    EMIT2(0x1e23) EMIT2(0x1e25) EMIT2(0x1e27)
		    EMIT2(0x1e29) EMIT2(0x1e2b) EMIT2(0x1e96)
		    EMIT2(0x2c68) EMIT2(0xa795)
		    प्रत्यागम OK;

	    पक्ष 'i': पक्ष i_grave: पक्ष i_acute: पक्ष i_circumflex:
	    पक्ष i_diaeresis: पक्ष 0x129: पक्ष 0x12b: पक्ष 0x12d:
	    पक्ष 0x12f: पक्ष 0x1d0: पक्ष 0x209: पक्ष 0x20b:
	    पक्ष 0x268: पक्ष 0x1d96: पक्ष 0x1e2d: पक्ष 0x1e2f:
	    पक्ष 0x1ec9: पक्ष 0x1ecb:
		    EMIT2('i') EMIT2(i_grave) EMIT2(i_acute)
		    EMIT2(i_circumflex) EMIT2(i_diaeresis)
		    EMIT2(0x129) EMIT2(0x12b) EMIT2(0x12d)
		    EMIT2(0x12f) EMIT2(0x1d0) EMIT2(0x209)
		    EMIT2(0x20b) EMIT2(0x268) EMIT2(0x1d96)
		    EMIT2(0x1e2d) EMIT2(0x1e2f) EMIT2(0x1ec9)
		    EMIT2(0x1ecb)
		    प्रत्यागम OK;

	    पक्ष 'j': पक्ष 0x135: पक्ष 0x1f0: पक्ष 0x249:
		    EMIT2('j') EMIT2(0x135) EMIT2(0x1f0) EMIT2(0x249)
		    प्रत्यागम OK;

	    पक्ष 'k': पक्ष 0x137: पक्ष 0x199: पक्ष 0x1e9: पक्ष 0x1d84:
	    पक्ष 0x1e31: पक्ष 0x1e33: पक्ष 0x1e35: पक्ष 0x2c6a: पक्ष 0xa741:
		    EMIT2('k') EMIT2(0x137) EMIT2(0x199) EMIT2(0x1e9)
		    EMIT2(0x1d84) EMIT2(0x1e31) EMIT2(0x1e33)
		    EMIT2(0x1e35) EMIT2(0x2c6a) EMIT2(0xa741)
		    प्रत्यागम OK;

	    पक्ष 'l': पक्ष 0x13a: पक्ष 0x13c: पक्ष 0x13e: पक्ष 0x140:
	    पक्ष 0x142: पक्ष 0x19a: पक्ष 0x1e37: पक्ष 0x1e39: पक्ष 0x1e3b:
	    पक्ष 0x1e3d: पक्ष 0x2c61:
		    EMIT2('l') EMIT2(0x13a) EMIT2(0x13c)
		    EMIT2(0x13e) EMIT2(0x140) EMIT2(0x142)
		    EMIT2(0x19a) EMIT2(0x1e37) EMIT2(0x1e39)
		    EMIT2(0x1e3b) EMIT2(0x1e3d) EMIT2(0x2c61)
		    प्रत्यागम OK;

	    पक्ष 'm': पक्ष 0x1d6f: पक्ष 0x1e3f: पक्ष 0x1e41: पक्ष 0x1e43:
		    EMIT2('m') EMIT2(0x1d6f) EMIT2(0x1e3f)
		    EMIT2(0x1e41) EMIT2(0x1e43)
		    प्रत्यागम OK;

	    पक्ष 'n': पक्ष n_virguilla: पक्ष 0x144: पक्ष 0x146: पक्ष 0x148:
	    पक्ष 0x149: पक्ष 0x1f9: पक्ष 0x1d70: पक्ष 0x1d87: पक्ष 0x1e45:
	    पक्ष 0x1e47: पक्ष 0x1e49: पक्ष 0x1e4b: पक्ष 0xa7a5:
		    EMIT2('n') EMIT2(n_virguilla)
		    EMIT2(0x144) EMIT2(0x146) EMIT2(0x148)
		    EMIT2(0x149) EMIT2(0x1f9) EMIT2(0x1d70)
		    EMIT2(0x1d87) EMIT2(0x1e45) EMIT2(0x1e47)
		    EMIT2(0x1e49) EMIT2(0x1e4b) EMIT2(0xa7a5)
		    प्रत्यागम OK;

	    पक्ष 'o': पक्ष o_grave: पक्ष o_acute: पक्ष o_circumflex:
	    पक्ष o_virguilla: पक्ष o_diaeresis: पक्ष o_slash:
	    पक्ष 0x14d: पक्ष 0x14f: पक्ष 0x151: पक्ष 0x1a1:
	    पक्ष 0x1d2: पक्ष 0x1eb: पक्ष 0x1ed: पक्ष 0x1ff:
	    पक्ष 0x20d: पक्ष 0x20f: पक्ष 0x22b: पक्ष 0x22d:
	    पक्ष 0x22f: पक्ष 0x231: पक्ष 0x275: पक्ष 0x1e4d:
	    पक्ष 0x1e4f: पक्ष 0x1e51: पक्ष 0x1e53: पक्ष 0x1ecd:
	    पक्ष 0x1ecf: पक्ष 0x1ed1: पक्ष 0x1ed3: पक्ष 0x1ed5:
	    पक्ष 0x1ed7: पक्ष 0x1ed9: पक्ष 0x1edb: पक्ष 0x1edd:
	    पक्ष 0x1edf: पक्ष 0x1ee1: पक्ष 0x1ee3:
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
		    प्रत्यागम OK;

	    पक्ष 'p': पक्ष 0x1a5: पक्ष 0x1d71: पक्ष 0x1d7d: पक्ष 0x1d88:
	    पक्ष 0x1e55: पक्ष 0x1e57:
		    EMIT2('p') EMIT2(0x1a5) EMIT2(0x1d71) EMIT2(0x1d7d)
		    EMIT2(0x1d88) EMIT2(0x1e55) EMIT2(0x1e57)
		    प्रत्यागम OK;

	    पक्ष 'q': पक्ष 0x24b: पक्ष 0x2a0:
		    EMIT2('q') EMIT2(0x24b) EMIT2(0x2a0)
		    प्रत्यागम OK;

	    पक्ष 'r': पक्ष 0x155: पक्ष 0x157: पक्ष 0x159: पक्ष 0x211:
	    पक्ष 0x213: पक्ष 0x24d: पक्ष 0x27d: पक्ष 0x1d72: पक्ष 0x1d73:
	    पक्ष 0x1d89: पक्ष 0x1e59: पक्ष 0x1e5b: पक्ष 0x1e5d: पक्ष 0x1e5f:
	    पक्ष 0xa7a7:
		    EMIT2('r') EMIT2(0x155) EMIT2(0x157) EMIT2(0x159)
		    EMIT2(0x211) EMIT2(0x213) EMIT2(0x24d) EMIT2(0x27d)
		    EMIT2(0x1d72) EMIT2(0x1d73) EMIT2(0x1d89) EMIT2(0x1e59)
		    EMIT2(0x1e5b) EMIT2(0x1e5d) EMIT2(0x1e5f) EMIT2(0xa7a7)
		    प्रत्यागम OK;

	    पक्ष 's': पक्ष 0x15b: पक्ष 0x15d: पक्ष 0x15f: पक्ष 0x161:
	    पक्ष 0x219: पक्ष 0x23f: पक्ष 0x1d74: पक्ष 0x1d8a: पक्ष 0x1e61:
	    पक्ष 0x1e63: पक्ष 0x1e65: पक्ष 0x1e67: पक्ष 0x1e69: पक्ष 0xa7a9:
		    EMIT2('s') EMIT2(0x15b) EMIT2(0x15d) EMIT2(0x15f)
		    EMIT2(0x161) EMIT2(0x219) EMIT2(0x23f) EMIT2(0x1d74)
		    EMIT2(0x1d8a) EMIT2(0x1e61) EMIT2(0x1e63) EMIT2(0x1e65)
		    EMIT2(0x1e67) EMIT2(0x1e69) EMIT2(0xa7a9)
		    प्रत्यागम OK;

	    पक्ष 't': पक्ष 0x163: पक्ष 0x165: पक्ष 0x167: पक्ष 0x1ab:
	    पक्ष 0x1ad: पक्ष 0x21b: पक्ष 0x288: पक्ष 0x1d75: पक्ष 0x1e6b:
	    पक्ष 0x1e6d: पक्ष 0x1e6f: पक्ष 0x1e71: पक्ष 0x1e97: पक्ष 0x2c66:
		    EMIT2('t') EMIT2(0x163) EMIT2(0x165) EMIT2(0x167)
		    EMIT2(0x1ab) EMIT2(0x1ad) EMIT2(0x21b) EMIT2(0x288)
		    EMIT2(0x1d75) EMIT2(0x1e6b) EMIT2(0x1e6d) EMIT2(0x1e6f)
		    EMIT2(0x1e71) EMIT2(0x1e97) EMIT2(0x2c66)
		    प्रत्यागम OK;

	    पक्ष 'u': पक्ष u_grave: पक्ष u_acute: पक्ष u_circumflex:
	    पक्ष u_diaeresis: पक्ष 0x169: पक्ष 0x16b: पक्ष 0x16d:
	    पक्ष 0x16f: पक्ष 0x171: पक्ष 0x173: पक्ष 0x1b0: पक्ष 0x1d4:
	    पक्ष 0x1d6: पक्ष 0x1d8: पक्ष 0x1da: पक्ष 0x1dc: पक्ष 0x215:
	    पक्ष 0x217: पक्ष 0x289: पक्ष 0x1d7e: पक्ष 0x1d99: पक्ष 0x1e73:
	    पक्ष 0x1e75: पक्ष 0x1e77: पक्ष 0x1e79: पक्ष 0x1e7b:
	    पक्ष 0x1ee5: पक्ष 0x1ee7: पक्ष 0x1ee9: पक्ष 0x1eeb:
	    पक्ष 0x1eed: पक्ष 0x1eef: पक्ष 0x1ef1:
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
		    प्रत्यागम OK;

	    पक्ष 'v': पक्ष 0x28b: पक्ष 0x1d8c: पक्ष 0x1e7d: पक्ष 0x1e7f:
		    EMIT2('v') EMIT2(0x28b) EMIT2(0x1d8c) EMIT2(0x1e7d)
		    EMIT2(0x1e7f)
		    प्रत्यागम OK;

	    पक्ष 'w': पक्ष 0x175: पक्ष 0x1e81: पक्ष 0x1e83: पक्ष 0x1e85:
	    पक्ष 0x1e87: पक्ष 0x1e89: पक्ष 0x1e98:
		    EMIT2('w') EMIT2(0x175) EMIT2(0x1e81) EMIT2(0x1e83)
		    EMIT2(0x1e85) EMIT2(0x1e87) EMIT2(0x1e89) EMIT2(0x1e98)
		    प्रत्यागम OK;

	    पक्ष 'x': पक्ष 0x1e8b: पक्ष 0x1e8d:
		    EMIT2('x') EMIT2(0x1e8b) EMIT2(0x1e8d)
		    प्रत्यागम OK;

	    पक्ष 'y': पक्ष y_acute: पक्ष y_diaeresis: पक्ष 0x177:
	    पक्ष 0x1b4: पक्ष 0x233: पक्ष 0x24f: पक्ष 0x1e8f:
	    पक्ष 0x1e99: पक्ष 0x1ef3: पक्ष 0x1ef5: पक्ष 0x1ef7:
	    पक्ष 0x1ef9:
		    EMIT2('y') EMIT2(y_acute) EMIT2(y_diaeresis)
		    EMIT2(0x177) EMIT2(0x1b4) EMIT2(0x233) EMIT2(0x24f)
		    EMIT2(0x1e8f) EMIT2(0x1e99) EMIT2(0x1ef3)
		    EMIT2(0x1ef5) EMIT2(0x1ef7) EMIT2(0x1ef9)
		    प्रत्यागम OK;

	    पक्ष 'z': पक्ष 0x17a: पक्ष 0x17c: पक्ष 0x17e: पक्ष 0x1b6:
	    पक्ष 0x1d76: पक्ष 0x1d8e: पक्ष 0x1e91: पक्ष 0x1e93:
	    पक्ष 0x1e95: पक्ष 0x2c6c:
		    EMIT2('z') EMIT2(0x17a) EMIT2(0x17c) EMIT2(0x17e)
		    EMIT2(0x1b6) EMIT2(0x1d76) EMIT2(0x1d8e) EMIT2(0x1e91)
		    EMIT2(0x1e93) EMIT2(0x1e95) EMIT2(0x2c6c)
		    प्रत्यागम OK;

	    // default: character itself
	}
    }

    EMIT2(c);
    प्रत्यागम OK;
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
    स्थिर पूर्णाङ्क
nfa_regatom(शून्य)
{
    पूर्णाङ्क		c;
    पूर्णाङ्क		charclass;
    पूर्णाङ्क		equiclass;
    पूर्णाङ्क		collclass;
    पूर्णाङ्क		got_coll_char;
    char_u	*p;
    char_u	*endp;
    char_u	*old_regparse = regparse;
    पूर्णाङ्क		extra = 0;
    पूर्णाङ्क		emit_range;
    पूर्णाङ्क		negated;
    पूर्णाङ्क		result;
    पूर्णाङ्क		startc = -1;
    पूर्णाङ्क		save_prev_at_start = prev_at_start;

    c = getchr();
    विकल्प (c)
    {
	पक्ष NUL:
	    EMSG_RET_FAIL(_(e_nfa_regexp_end_encountered_prematurely));

	पक्ष Magic('^'):
	    EMIT(NFA_BOL);
	    भङ्ग;

	पक्ष Magic('$'):
	    EMIT(NFA_EOL);
#यदि defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#endif
	    भङ्ग;

	पक्ष Magic('<'):
	    EMIT(NFA_BOW);
	    भङ्ग;

	पक्ष Magic('>'):
	    EMIT(NFA_EOW);
	    भङ्ग;

	पक्ष Magic('_'):
	    c = no_Magic(getchr());
	    यदि (c == NUL)
		EMSG_RET_FAIL(_(e_nfa_regexp_end_encountered_prematurely));

	    यदि (c == '^')	// "\_^" is start-of-line
	    {
		EMIT(NFA_BOL);
		भङ्ग;
	    }
	    यदि (c == '$')	// "\_$" is end-of-line
	    {
		EMIT(NFA_EOL);
#यदि defined(FEAT_SYN_HL)
		had_eol = TRUE;
#endif
		भङ्ग;
	    }

	    extra = NFA_ADD_NL;

	    // "\_[" is collection plus newline
	    यदि (c == '[')
		गच्छ collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	/*
	 * Character classes.
	 */
	पक्ष Magic('.'):
	पक्ष Magic('i'):
	पक्ष Magic('I'):
	पक्ष Magic('k'):
	पक्ष Magic('K'):
	पक्ष Magic('f'):
	पक्ष Magic('F'):
	पक्ष Magic('p'):
	पक्ष Magic('P'):
	पक्ष Magic('s'):
	पक्ष Magic('S'):
	पक्ष Magic('d'):
	पक्ष Magic('D'):
	पक्ष Magic('x'):
	पक्ष Magic('X'):
	पक्ष Magic('o'):
	पक्ष Magic('O'):
	पक्ष Magic('w'):
	पक्ष Magic('W'):
	पक्ष Magic('h'):
	पक्ष Magic('H'):
	पक्ष Magic('a'):
	पक्ष Magic('A'):
	पक्ष Magic('l'):
	पक्ष Magic('L'):
	पक्ष Magic('u'):
	पक्ष Magic('U'):
	    p = vim_strchr(classchars, no_Magic(c));
	    यदि (p == रिक्त)
	    {
		यदि (extra == NFA_ADD_NL)
		{
		    semsg(_(e_nfa_regexp_invalid_character_class_nr), c);
		    rc_did_emsg = TRUE;
		    प्रत्यागम FAIL;
		}
		siemsg("Unknown character class char: %d", c);
		प्रत्यागम FAIL;
	    }

	    // When '.' is followed by a composing char ignore the dot, so that
	    // the composing char is matched here.
	    यदि (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	    {
		old_regparse = regparse;
		c = getchr();
		गच्छ nfa_do_multibyte;
	    }
	    EMIT(nfa_classcodes[p - classchars]);
	    यदि (extra == NFA_ADD_NL)
	    {
		EMIT(NFA_NEWL);
		EMIT(NFA_OR);
		regflags |= RF_HASNL;
	    }
	    भङ्ग;

	पक्ष Magic('n'):
	    यदि (reg_string)
		// In a string "\n" matches a newline character.
		EMIT(NL);
	    अन्यथा
	    {
		// In buffer text "\n" matches the end of a line.
		EMIT(NFA_NEWL);
		regflags |= RF_HASNL;
	    }
	    भङ्ग;

	पक्ष Magic('('):
	    यदि (nfa_reg(REG_PAREN) == FAIL)
		प्रत्यागम FAIL;	    // cascaded error
	    भङ्ग;

	पक्ष Magic('|'):
	पक्ष Magic('&'):
	पक्ष Magic(')'):
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    प्रत्यागम FAIL;

	पक्ष Magic('='):
	पक्ष Magic('?'):
	पक्ष Magic('+'):
	पक्ष Magic('@'):
	पक्ष Magic('*'):
	पक्ष Magic('{'):
	    // these should follow an atom, not form an atom
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    प्रत्यागम FAIL;

	पक्ष Magic('~'):
	    {
		char_u	    *lp;

		// Previous substitute pattern.
		// Generated as "\%(pattern\)".
		यदि (reg_prev_sub == रिक्त)
		{
		    emsg(_(e_no_previous_substitute_regular_expression));
		    प्रत्यागम FAIL;
		}
		प्रति (lp = reg_prev_sub; *lp != NUL; MB_CPTR_ADV(lp))
		{
		    EMIT(PTR2CHAR(lp));
		    यदि (lp != reg_prev_sub)
			EMIT(NFA_CONCAT);
		}
		EMIT(NFA_NOPEN);
		भङ्ग;
	    }

	पक्ष Magic('1'):
	पक्ष Magic('2'):
	पक्ष Magic('3'):
	पक्ष Magic('4'):
	पक्ष Magic('5'):
	पक्ष Magic('6'):
	पक्ष Magic('7'):
	पक्ष Magic('8'):
	पक्ष Magic('9'):
	    {
		पूर्णाङ्क refnum = no_Magic(c) - '1';

		यदि (!seen_endbrace(refnum + 1))
		    प्रत्यागम FAIL;
		EMIT(NFA_BACKREF1 + refnum);
		rex.nfa_has_backref = TRUE;
	    }
	    भङ्ग;

	पक्ष Magic('z'):
	    c = no_Magic(getchr());
	    विकल्प (c)
	    {
		पक्ष 's':
		    EMIT(NFA_ZSTART);
		    यदि (re_mult_next("\\zs") == FAIL)
			प्रत्यागम FAIL;
		    भङ्ग;
		पक्ष 'e':
		    EMIT(NFA_ZEND);
		    rex.nfa_has_zend = TRUE;
		    यदि (re_mult_next("\\ze") == FAIL)
			प्रत्यागम FAIL;
		    भङ्ग;
#ifdef FEAT_SYN_HL
		पक्ष '1':
		पक्ष '2':
		पक्ष '3':
		पक्ष '4':
		पक्ष '5':
		पक्ष '6':
		पक्ष '7':
		पक्ष '8':
		पक्ष '9':
		    // \z1...\z9
		    यदि ((reg_do_extmatch & REX_USE) == 0)
			EMSG_RET_FAIL(_(e_z1_z9_not_allowed_here));
		    EMIT(NFA_ZREF1 + (no_Magic(c) - '1'));
		    // No need to set rex.nfa_has_backref, the sub-matches don't
		    // change when \z1 .. \z9 matches or not.
		    re_has_z = REX_USE;
		    भङ्ग;
		पक्ष '(':
		    // \z(
		    यदि ((reg_do_extmatch & REX_SET) == 0)
			EMSG_RET_FAIL(_(e_z_not_allowed_here));
		    यदि (nfa_reg(REG_ZPAREN) == FAIL)
			प्रत्यागम FAIL;	    // cascaded error
		    re_has_z = REX_SET;
		    भङ्ग;
#endif
		स्वभाव:
		    semsg(_(e_nfa_regexp_unknown_operator_z_chr), no_Magic(c));
		    प्रत्यागम FAIL;
	    }
	    भङ्ग;

	पक्ष Magic('%'):
	    c = no_Magic(getchr());
	    विकल्प (c)
	    {
		// () without a back reference
		पक्ष '(':
		    यदि (nfa_reg(REG_NPAREN) == FAIL)
			प्रत्यागम FAIL;
		    EMIT(NFA_NOPEN);
		    भङ्ग;

		पक्ष 'd':   // %d123 decimal
		पक्ष 'o':   // %o123 octal
		पक्ष 'x':   // %xab hex 2
		पक्ष 'u':   // %uabcd hex 4
		पक्ष 'U':   // %U1234abcd hex 8
		    {
			vimlong_T nr;

			विकल्प (c)
			{
			    पक्ष 'd': nr = getdecchrs(); भङ्ग;
			    पक्ष 'o': nr = getoctchrs(); भङ्ग;
			    पक्ष 'x': nr = gethexchrs(2); भङ्ग;
			    पक्ष 'u': nr = gethexchrs(4); भङ्ग;
			    पक्ष 'U': nr = gethexchrs(8); भङ्ग;
			    स्वभाव:  nr = -1; भङ्ग;
			}

			यदि (nr < 0 || nr > INT_MAX)
			    EMSG2_RET_FAIL(_(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			// A NUL is stored in the text as NL
			// TODO: what if a composing character follows?
			EMIT(nr == 0 ? 0x0a : (long)nr);
		    }
		    भङ्ग;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		पक्ष '^':
		    EMIT(NFA_BOF);
		    भङ्ग;

		पक्ष '$':
		    EMIT(NFA_EOF);
		    भङ्ग;

		पक्ष '#':
		    यदि (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_start_of_pattern),
								  regparse[1]);
			प्रत्यागम FAIL;
		    }
		    EMIT(NFA_CURSOR);
		    भङ्ग;

		पक्ष 'V':
		    EMIT(NFA_VISUAL);
		    भङ्ग;

		पक्ष 'C':
		    EMIT(NFA_ANY_COMPOSING);
		    भङ्ग;

		पक्ष '[':
		    {
			पूर्णाङ्क	    n;

			// \%[abc]
			प्रति (n = 0; (c = peekchr()) != ']'; ++n)
			{
			    यदि (c == NUL)
				EMSG2_RET_FAIL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
			    // recursive call!
			    यदि (nfa_regatom() == FAIL)
				प्रत्यागम FAIL;
			}
			getchr();  // get the ]
			यदि (n == 0)
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
			भङ्ग;
		    }

		स्वभाव:
		    {
			long_u	n = 0;
			पूर्णाङ्क	cmp = c;
			पूर्णाङ्क	cur = FALSE;
			पूर्णाङ्क	got_digit = FALSE;

			यदि (c == '<' || c == '>')
			    c = getchr();
			यदि (no_Magic(c) == '.')
			{
			    cur = TRUE;
			    c = getchr();
			}
			यावत् (VIM_ISDIGIT(c))
			{
			    long_u tmp;

			    यदि (cur)
			    {
				semsg(_(e_regexp_संख्या_after_dot_pos_शोध_chr),
								  no_Magic(c));
				प्रत्यागम FAIL;
			    }
			    tmp = n * 10 + (c - '0');

			    यदि (tmp < n)
			    {
				// overflow.
				emsg(_(e_percent_मूल्य_too_large));
				प्रत्यागम FAIL;
			    }
			    n = tmp;
			    c = getchr();
			    got_digit = TRUE;
			}
			यदि (c == 'l' || c == 'c' || c == 'v')
			{
			    long_u limit = INT_MAX;

			    यदि (!cur && !got_digit)
			    {
				semsg(_(e_nfa_regexp_missing_मूल्य_in_chr),
								  no_Magic(c));
				प्रत्यागम FAIL;
			    }
			    यदि (c == 'l')
			    {
				यदि (cur)
				    n = curwin->w_cursor.lnum;
				// \%{n}l  \%{n}<l  \%{n}>l
				EMIT(cmp == '<' ? NFA_LNUM_LT :
				     cmp == '>' ? NFA_LNUM_GT : NFA_LNUM);
				यदि (save_prev_at_start)
				    at_start = TRUE;
			    }
			    अन्यथा यदि (c == 'c')
			    {
				यदि (cur)
				{
				    n = curwin->w_cursor.col;
				    n++;
				}
				// \%{n}c  \%{n}<c  \%{n}>c
				EMIT(cmp == '<' ? NFA_COL_LT :
				     cmp == '>' ? NFA_COL_GT : NFA_COL);
			    }
			    अन्यथा
			    {
				यदि (cur)
				{
				    colnr_T vcol = 0;

				    getvvcol(curwin, &curwin->w_cursor,
							 रिक्त, रिक्त, &vcol, 0);
				    n = ++vcol;
				}
				// \%{n}v  \%{n}<v  \%{n}>v
				EMIT(cmp == '<' ? NFA_VCOL_LT :
				     cmp == '>' ? NFA_VCOL_GT : NFA_VCOL);
				limit = INT_MAX / MB_MAXBYTES;
			    }
			    यदि (n >= limit)
			    {
				emsg(_(e_percent_मूल्य_too_large));
				प्रत्यागम FAIL;
			    }
			    EMIT((पूर्णाङ्क)n);
			    भङ्ग;
			}
			अन्यथा यदि (no_Magic(c) == '\'' && n == 0)
			{
			    // \%'m  \%<'m  \%>'m
			    EMIT(cmp == '<' ? NFA_MARK_LT :
				 cmp == '>' ? NFA_MARK_GT : NFA_MARK);
			    EMIT(getchr());
			    भङ्ग;
			}
		    }
		    semsg(_(e_nfa_regexp_unknown_operator_percent_chr),
								  no_Magic(c));
		    प्रत्यागम FAIL;
	    }
	    भङ्ग;

	पक्ष Magic('['):
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
	    यदि (*endp == ']')
	    {
		पूर्णाङ्क plen;
		bool range_endpoint;
		/*
		 * Try to reverse engineer character classes. For example,
		 * recognize that [0-9] stands for \d and [A-Za-z_] for \h,
		 * and perform the necessary substitutions in the NFA.
		 */
		result = nfa_recognize_char_class(regparse, endp,
							 extra == NFA_ADD_NL);
		यदि (result != FAIL)
		{
		    यदि (result >= NFA_FIRST_NL && result <= NFA_LAST_NL)
		    {
			EMIT(result - NFA_ADD_NL);
			EMIT(NFA_NEWL);
			EMIT(NFA_OR);
		    }
		    अन्यथा
			EMIT(result);
		    regparse = endp;
		    MB_PTR_ADV(regparse);
		    प्रत्यागम OK;
		}
		/*
		 * Failed to recognize a character class. Use the simple
		 * version that turns [abc] into 'a' OR 'b' OR 'c'
		 */
		startc = -1;
		negated = FALSE;
		यदि (*regparse == '^')			// negated range
		{
		    negated = TRUE;
		    MB_PTR_ADV(regparse);
		    EMIT(NFA_START_NEG_COLL);
		}
		अन्यथा
		    EMIT(NFA_START_COLL);
		यदि (*regparse == '-')
		{
		    startc = '-';
		    EMIT(startc);
		    EMIT(NFA_CONCAT);
		    MB_PTR_ADV(regparse);
		}
		// Emit the OR branches for each character in the []
		emit_range = FALSE;
		यावत् (regparse < endp)
		{
		    पूर्णाङ्क	    oldstartc = startc;
		    range_endpoint = false;

		    startc = -1;
		    got_coll_char = FALSE;
		    यदि (*regparse == '[')
		    {
			// Check for [: :], [= =], [. .]
			equiclass = collclass = 0;
			charclass = get_char_class(&regparse);
			यदि (charclass == CLASS_NONE)
			{
			    equiclass = get_equi_class(&regparse);
			    यदि (equiclass == 0)
				collclass = get_coll_element(&regparse);
			}

			// Character class like [:alpha:]
			यदि (charclass != CLASS_NONE)
			{
			    विकल्प (charclass)
			    {
				पक्ष CLASS_ALNUM:
				    EMIT(NFA_CLASS_ALNUM);
				    भङ्ग;
				पक्ष CLASS_ALPHA:
				    EMIT(NFA_CLASS_ALPHA);
				    भङ्ग;
				पक्ष CLASS_BLANK:
				    EMIT(NFA_CLASS_BLANK);
				    भङ्ग;
				पक्ष CLASS_CNTRL:
				    EMIT(NFA_CLASS_CNTRL);
				    भङ्ग;
				पक्ष CLASS_DIGIT:
				    EMIT(NFA_CLASS_DIGIT);
				    भङ्ग;
				पक्ष CLASS_GRAPH:
				    EMIT(NFA_CLASS_GRAPH);
				    भङ्ग;
				पक्ष CLASS_LOWER:
				    wants_nfa = TRUE;
				    EMIT(NFA_CLASS_LOWER);
				    भङ्ग;
				पक्ष CLASS_PRINT:
				    EMIT(NFA_CLASS_PRINT);
				    भङ्ग;
				पक्ष CLASS_PUNCT:
				    EMIT(NFA_CLASS_PUNCT);
				    भङ्ग;
				पक्ष CLASS_SPACE:
				    EMIT(NFA_CLASS_SPACE);
				    भङ्ग;
				पक्ष CLASS_UPPER:
				    wants_nfa = TRUE;
				    EMIT(NFA_CLASS_UPPER);
				    भङ्ग;
				पक्ष CLASS_XDIGIT:
				    EMIT(NFA_CLASS_XDIGIT);
				    भङ्ग;
				पक्ष CLASS_TAB:
				    EMIT(NFA_CLASS_TAB);
				    भङ्ग;
				पक्ष CLASS_RETURN:
				    EMIT(NFA_CLASS_RETURN);
				    भङ्ग;
				पक्ष CLASS_BACKSPACE:
				    EMIT(NFA_CLASS_BACKSPACE);
				    भङ्ग;
				पक्ष CLASS_ESCAPE:
				    EMIT(NFA_CLASS_ESCAPE);
				    भङ्ग;
				पक्ष CLASS_IDENT:
				    EMIT(NFA_CLASS_IDENT);
				    भङ्ग;
				पक्ष CLASS_KEYWORD:
				    EMIT(NFA_CLASS_KEYWORD);
				    भङ्ग;
				पक्ष CLASS_FNAME:
				    EMIT(NFA_CLASS_FNAME);
				    भङ्ग;
			    }
			    EMIT(NFA_CONCAT);
			    अनुवर्त;
			}
			// Try equivalence class [=a=] and the like
			यदि (equiclass != 0)
			{
			    result = nfa_emit_equi_class(equiclass);
			    यदि (result == FAIL)
			    {
				// should never happen
				EMSG_RET_FAIL(_(e_error_building_nfa_with_equivalence_class));
			    }
			    अनुवर्त;
			}
			// Try collating class like [. .]
			यदि (collclass != 0)
			{
			    startc = collclass;	 // allow [.a.]-x as a range
			    // Will emit the proper atom at the end of the
			    // while loop.
			}
		    }
		    // Try a range like 'a-x' or '\t-z'. Also allows '-' as a
		    // start character.
		    यदि (*regparse == '-' && oldstartc != -1)
		    {
			emit_range = TRUE;
			startc = oldstartc;
			MB_PTR_ADV(regparse);
			अनुवर्त;	    // reading the end of the range
		    }

		    // Now handle simple and escaped characters.
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    यदि (*regparse == '\\'
			    && !reg_cpo_bsl
			    && regparse + 1 <= endp
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != रिक्त
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR, regparse[1])
								      != रिक्त)
			    )
			)
		    {
			MB_PTR_ADV(regparse);

			यदि (*regparse == 'n')
			    startc = (reg_string || emit_range
					|| regparse[1] == '-') ? NL : NFA_NEWL;
			अन्यथा यदि (*regparse == 'd'
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
				यदि (startc == INT_MAX)
				    EMSG_RET_FAIL(_(e_unicode_val_too_large));
				got_coll_char = TRUE;
				MB_PTR_BACK(old_regparse, regparse);
			    }
			    अन्यथा
			    {
				// \r,\t,\e,\b
				startc = backslash_trans(*regparse);
			    }
		    }

		    // Normal printable char
		    यदि (startc == -1)
			startc = PTR2CHAR(regparse);

		    // Previous char was '-', so this char is end of range.
		    यदि (emit_range)
		    {
			पूर्णाङ्क	endc = startc;
			range_endpoint = true;

			startc = oldstartc;
			यदि (startc > endc)
			    EMSG_RET_FAIL(_(e_reverse_range_in_character_class));

			यदि (endc > startc + 2)
			{
			    // Emit a range instead of the sequence of
			    // individual characters.
			    यदि (startc == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    अन्यथा
				--post_ptr; // remove NFA_CONCAT
			    EMIT(endc);
			    EMIT(NFA_RANGE);
			    EMIT(NFA_CONCAT);
			}
			अन्यथा यदि (has_mbyte && ((*mb_char2len)(startc) > 1
				    || (*mb_char2len)(endc) > 1))
			{
			    // Emit the characters in the range.
			    // "startc" was already emitted, so skip it.
			    //
			    प्रति (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			अन्यथा
			{
			    // Emit the range. "startc" was already emitted, so
			    // skip it.
			    प्रति (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			emit_range = FALSE;
			startc = -1;
		    }
		    अन्यथा
		    {
			// This char (startc) is not part of a range. Just
			// emit it.
			// Normally, simply emit startc. But if we get char
			// code=0 from a collating char, then replace it with
			// 0x0a.
			// This is needed to completely mimic the behaviour of
			// the backtracking engine.
			यदि (startc == NFA_NEWL)
			{
			    // Line break can't be matched as part of the
			    // collection, add an OR below. But not for negated
			    // range.
			    यदि (!negated)
				extra = NFA_ADD_NL;
			}
			अन्यथा
			{
			    यदि (got_coll_char == TRUE && startc == 0)
			    {
				EMIT(0x0a);
				EMIT(NFA_CONCAT);
			    }
			    अन्यथा
			    {
				EMIT(startc);
				यदि (!(enc_utf8 && (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse)))))
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
		    यदि (!range_endpoint && enc_utf8 &&
			    (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse))))
		    {
			पूर्णाङ्क i = utf_ptr2len(regparse);

			c = utf_ptr2char(regparse + i);

			// Add composing characters
			प्रति (;;)
			{
			    यदि (c == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    अन्यथा
				EMIT(c);
			    EMIT(NFA_CONCAT);
			    यदि ((i += utf_char2len(c)) >= plen)
				भङ्ग;
			    c = utf_ptr2char(regparse + i);
			}
			EMIT(NFA_COMPOSING);
			EMIT(NFA_CONCAT);
		    }
		    MB_PTR_ADV(regparse);
		} // while (p < endp)

		MB_PTR_BACK(old_regparse, regparse);
		यदि (*regparse == '-')	    // if last, '-' is just a char
		{
		    EMIT('-');
		    EMIT(NFA_CONCAT);
		}

		// skip the trailing ]
		regparse = endp;
		MB_PTR_ADV(regparse);

		// Mark end of the collection.
		यदि (negated == TRUE)
		    EMIT(NFA_END_NEG_COLL);
		अन्यथा
		    EMIT(NFA_END_COLL);

		// \_[] also matches \n but it's not negated
		यदि (extra == NFA_ADD_NL)
		{
		    EMIT(reg_string ? NL : NFA_NEWL);
		    EMIT(NFA_OR);
		}

		प्रत्यागम OK;
	    } // if exists closing ]

	    यदि (reg_strict)
		EMSG_RET_FAIL(_(e_missing_rsb_after_str_lsb));
	    // FALLTHROUGH

	स्वभाव:
	    {
		पूर्णाङ्क	plen;

nfa_do_multibyte:
		// plen is length of current char with composing chars
		यदि (enc_utf8 && ((*mb_char2len)(c)
			    != (plen = utfc_ptr2len(old_regparse))
						       || utf_iscomposing(c)))
		{
		    पूर्णाङ्क i = 0;

		    // A base character plus composing characters, or just one
		    // or more composing characters.
		    // This requires creating a separate atom as if enclosing
		    // the characters in (), where NFA_COMPOSING is the ( and
		    // NFA_END_COMPOSING is the ). Note that right now we are
		    // building the postfix form, not the NFA itself;
		    // a composing char could be: a, b, c, NFA_COMPOSING
		    // where 'b' and 'c' are chars with codes > 256.
		    प्रति (;;)
		    {
			EMIT(c);
			यदि (i > 0)
			    EMIT(NFA_CONCAT);
			यदि ((i += utf_char2len(c)) >= plen)
			    भङ्ग;
			c = utf_ptr2char(old_regparse + i);
		    }
		    EMIT(NFA_COMPOSING);
		    regparse = old_regparse + plen;
		}
		अन्यथा
		{
		    c = no_Magic(c);
		    EMIT(c);
		}
		प्रत्यागम OK;
	    }
    }

    प्रत्यागम OK;
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
    स्थिर पूर्णाङ्क
nfa_regpiece(शून्य)
{
    पूर्णाङ्क		i;
    पूर्णाङ्क		op;
    पूर्णाङ्क		ret;
    long	minval, maxval;
    पूर्णाङ्क		greedy = TRUE;      // Braces are prefixed with '-' ?
    parse_state_T old_state;
    parse_state_T new_state;
    long	c2;
    पूर्णाङ्क		old_post_pos;
    पूर्णाङ्क		my_post_start;
    पूर्णाङ्क		quest;

    // Save the current parse state, so that we can use it if <atom>{m,n} is
    // next.
    save_parse_state(&old_state);

    // store current pos in the postfix form, for \{m,n} involving 0s
    my_post_start = (पूर्णाङ्क)(post_ptr - post_start);

    ret = nfa_regatom();
    यदि (ret == FAIL)
	प्रत्यागम FAIL;	    // cascaded error

    op = peekchr();
    यदि (re_multi_type(op) == NOT_MULTI)
	प्रत्यागम OK;

    skipchr();
    विकल्प (op)
    {
	पक्ष Magic('*'):
	    EMIT(NFA_STAR);
	    भङ्ग;

	पक्ष Magic('+'):
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
	    यदि (nfa_regatom() == FAIL)
		प्रत्यागम FAIL;
	    EMIT(NFA_STAR);
	    EMIT(NFA_CONCAT);
	    skipchr();		// skip the \+
	    भङ्ग;

	पक्ष Magic('@'):
	    c2 = (long)getdecchrs();
	    op = no_Magic(getchr());
	    i = 0;
	    विकल्प(op)
	    {
		पक्ष '=':
		    // \@=
		    i = NFA_PREV_ATOM_NO_WIDTH;
		    भङ्ग;
		पक्ष '!':
		    // \@!
		    i = NFA_PREV_ATOM_NO_WIDTH_NEG;
		    भङ्ग;
		पक्ष '<':
		    op = no_Magic(getchr());
		    यदि (op == '=')
			// \@<=
			i = NFA_PREV_ATOM_JUST_BEFORE;
		    अन्यथा यदि (op == '!')
			// \@<!
			i = NFA_PREV_ATOM_JUST_BEFORE_NEG;
		    भङ्ग;
		पक्ष '>':
		    // \@>
		    i = NFA_PREV_ATOM_LIKE_PATTERN;
		    भङ्ग;
	    }
	    यदि (i == 0)
	    {
		semsg(_(e_nfa_regexp_unknown_operator_at_chr), op);
		प्रत्यागम FAIL;
	    }
	    EMIT(i);
	    यदि (i == NFA_PREV_ATOM_JUST_BEFORE
					|| i == NFA_PREV_ATOM_JUST_BEFORE_NEG)
		EMIT(c2);
	    भङ्ग;

	पक्ष Magic('?'):
	पक्ष Magic('='):
	    EMIT(NFA_QUEST);
	    भङ्ग;

	पक्ष Magic('{'):
	    // a{2,5} will expand to 'aaa?a?a?'
	    // a{-1,3} will expand to 'aa??a??', where ?? is the nongreedy
	    // version of '?'
	    // \v(ab){2,3} will expand to '(ab)(ab)(ab)?', where all the
	    // parenthesis have the same id

	    greedy = TRUE;
	    c2 = peekchr();
	    यदि (c2 == '-' || c2 == Magic('-'))
	    {
		skipchr();
		greedy = FALSE;
	    }
	    यदि (!read_limits(&minval, &maxval))
		EMSG_RET_FAIL(_(e_nfa_regexp_error_reading_repetition_limits));

	    //  <atom>{0,inf}, <atom>{0,} and <atom>{}  are equivalent to
	    //  <atom>*
	    यदि (minval == 0 && maxval == MAX_LIMIT)
	    {
		यदि (greedy)		// { { (match the braces)
		    // \{}, \{0,}
		    EMIT(NFA_STAR);
		अन्यथा			// { { (match the braces)
		    // \{-}, \{-0,}
		    EMIT(NFA_STAR_NONGREEDY);
		भङ्ग;
	    }

	    // Special case: x{0} or x{-0}
	    यदि (maxval == 0)
	    {
		// Ignore result of previous call to nfa_regatom()
		post_ptr = post_start + my_post_start;
		// NFA_EMPTY is 0-length and works everywhere
		EMIT(NFA_EMPTY);
		प्रत्यागम OK;
	    }

	    // The engine is very inefficient (uses too many states) when the
	    // maximum is much larger than the minimum and when the maximum is
	    // large.  However, when maxval is MAX_LIMIT, it is okay, as this
	    // will emit NFA_STAR.
	    // Bail out if we can use the other engine, but only, when the
	    // pattern does not need the NFA engine like (e.g. [[:upper:]]\{2,\}
	    // does not work with characters > 8 bit with the BT engine)
	    यदि ((nfa_re_flags & RE_AUTO)
				   && (maxval > 500 || maxval > minval + 200)
				   && (maxval != MAX_LIMIT && minval < 200)
				   && !wants_nfa)
		प्रत्यागम FAIL;

	    // Ignore previous call to nfa_regatom()
	    post_ptr = post_start + my_post_start;
	    // Save parse state after the repeated atom and the \{}
	    save_parse_state(&new_state);

	    quest = (greedy == TRUE? NFA_QUEST : NFA_QUEST_NONGREEDY);
	    प्रति (i = 0; i < maxval; i++)
	    {
		// Goto beginning of the repeated atom
		restore_parse_state(&old_state);
		old_post_pos = (पूर्णाङ्क)(post_ptr - post_start);
		यदि (nfa_regatom() == FAIL)
		    प्रत्यागम FAIL;
		// after "minval" times, atoms are optional
		यदि (i + 1 > minval)
		{
		    यदि (maxval == MAX_LIMIT)
		    {
			यदि (greedy)
			    EMIT(NFA_STAR);
			अन्यथा
			    EMIT(NFA_STAR_NONGREEDY);
		    }
		    अन्यथा
			EMIT(quest);
		}
		यदि (old_post_pos != my_post_start)
		    EMIT(NFA_CONCAT);
		यदि (i + 1 > minval && maxval == MAX_LIMIT)
		    भङ्ग;
	    }

	    // Go to just after the repeated atom and the \{}
	    restore_parse_state(&new_state);
	    curchr = -1;

	    भङ्ग;


	स्वभाव:
	    भङ्ग;
    }	// end switch

    यदि (re_multi_type(peekchr()) != NOT_MULTI)
	// Can't have a multi follow a multi.
	EMSG_RET_FAIL(_(e_nfa_regexp_cant_have_multi_follow_multi));

    प्रत्यागम OK;
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
    स्थिर पूर्णाङ्क
nfa_regconcat(शून्य)
{
    पूर्णाङ्क		cont = TRUE;
    पूर्णाङ्क		first = TRUE;

    यावत् (cont)
    {
	विकल्प (peekchr())
	{
	    पक्ष NUL:
	    पक्ष Magic('|'):
	    पक्ष Magic('&'):
	    पक्ष Magic(')'):
		cont = FALSE;
		भङ्ग;

	    पक्ष Magic('Z'):
		regflags |= RF_ICOMBINE;
		skipchr_keepstart();
		भङ्ग;
	    पक्ष Magic('c'):
		regflags |= RF_ICASE;
		skipchr_keepstart();
		भङ्ग;
	    पक्ष Magic('C'):
		regflags |= RF_NOICASE;
		skipchr_keepstart();
		भङ्ग;
	    पक्ष Magic('v'):
		reg_magic = MAGIC_ALL;
		skipchr_keepstart();
		curchr = -1;
		भङ्ग;
	    पक्ष Magic('m'):
		reg_magic = MAGIC_ON;
		skipchr_keepstart();
		curchr = -1;
		भङ्ग;
	    पक्ष Magic('M'):
		reg_magic = MAGIC_OFF;
		skipchr_keepstart();
		curchr = -1;
		भङ्ग;
	    पक्ष Magic('V'):
		reg_magic = MAGIC_NONE;
		skipchr_keepstart();
		curchr = -1;
		भङ्ग;

	    स्वभाव:
		यदि (nfa_regpiece() == FAIL)
		    प्रत्यागम FAIL;
		यदि (first == FALSE)
		    EMIT(NFA_CONCAT);
		अन्यथा
		    first = FALSE;
		भङ्ग;
	}
    }

    प्रत्यागम OK;
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
    स्थिर पूर्णाङ्क
nfa_regbranch(शून्य)
{
    पूर्णाङ्क		old_post_pos;

    old_post_pos = (पूर्णाङ्क)(post_ptr - post_start);

    // First branch, possibly the only one
    यदि (nfa_regconcat() == FAIL)
	प्रत्यागम FAIL;

    // Try next concats
    यावत् (peekchr() == Magic('&'))
    {
	skipchr();
	// if concat is empty do emit a node
	यदि (old_post_pos == (पूर्णाङ्क)(post_ptr - post_start))
	    EMIT(NFA_EMPTY);
	EMIT(NFA_NOPEN);
	EMIT(NFA_PREV_ATOM_NO_WIDTH);
	old_post_pos = (पूर्णाङ्क)(post_ptr - post_start);
	यदि (nfa_regconcat() == FAIL)
	    प्रत्यागम FAIL;
	// if concat is empty do emit a node
	यदि (old_post_pos == (पूर्णाङ्क)(post_ptr - post_start))
	    EMIT(NFA_EMPTY);
	EMIT(NFA_CONCAT);
    }

    // if a branch is empty, emit one node for it
    यदि (old_post_pos == (पूर्णाङ्क)(post_ptr - post_start))
	EMIT(NFA_EMPTY);

    प्रत्यागम OK;
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
    स्थिर पूर्णाङ्क
nfa_reg(
    पूर्णाङ्क		paren)	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
{
    पूर्णाङ्क		parno = 0;
    पूर्णाङ्क		status = FAIL;

    यदि (paren == REG_PAREN)
    {
	यदि (regnpar >= NSUBEXP) // Too many `('
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_parens));
	parno = regnpar++;
    }
#ifdef FEAT_SYN_HL
    अन्यथा यदि (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	यदि (regnzpar >= NSUBEXP)
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_z));
	parno = regnzpar++;
    }
#endif

    यदि (nfa_reg_parse_depth >= REG_MAX_PAREN_DEPTH)
	EMSG_RET_FAIL(_(e_command_too_complex));
    ++nfa_reg_parse_depth;

    यदि (nfa_regbranch() == FAIL)
	गच्छ theend;	    // cascaded error

    यावत् (peekchr() == Magic('|'))
    {
	skipchr();
	यदि (nfa_regbranch() == FAIL)
	    गच्छ theend;    // cascaded error
	EMIT(NFA_OR);
    }

    // Check for proper termination.
    यदि (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
	यदि (paren == REG_NPAREN)
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	अन्यथा
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	rc_did_emsg = TRUE;
	गच्छ theend;
    }
    अन्यथा यदि (paren == REG_NOPAREN && peekchr() != NUL)
    {
	यदि (peekchr() == Magic(')'))
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	अन्यथा
	    emsg(_(e_nfa_regexp_proper_termination_error));
	rc_did_emsg = TRUE;
	गच्छ theend;
    }
    /*
     * Here we set the flag allowing back references to this set of
     * parentheses.
     */
    यदि (paren == REG_PAREN)
    {
	had_endbrace[parno] = TRUE;     // have seen the close paren
	EMIT(NFA_MOPEN + parno);
    }
#ifdef FEAT_SYN_HL
    अन्यथा यदि (paren == REG_ZPAREN)
	EMIT(NFA_ZOPEN + parno);
#endif

    status = OK;

theend:
    --nfa_reg_parse_depth;
    प्रत्यागम status;
}

#ifdef DEBUG
स्थिर char_u code[50];

    स्थिर शून्य
nfa_set_code(पूर्णाङ्क c)
{
    पूर्णाङ्क	    addnl = FALSE;

    यदि (c >= NFA_FIRST_NL && c <= NFA_LAST_NL)
    {
	addnl = TRUE;
	c -= NFA_ADD_NL;
    }

    STRCPY(code, "");
    विकल्प (c)
    {
	पक्ष NFA_MATCH:	    STRCPY(code, "NFA_MATCH "); भङ्ग;
	पक्ष NFA_SPLIT:	    STRCPY(code, "NFA_SPLIT "); भङ्ग;
	पक्ष NFA_CONCAT:    STRCPY(code, "NFA_CONCAT "); भङ्ग;
	पक्ष NFA_NEWL:	    STRCPY(code, "NFA_NEWL "); भङ्ग;
	पक्ष NFA_ZSTART:    STRCPY(code, "NFA_ZSTART"); भङ्ग;
	पक्ष NFA_ZEND:	    STRCPY(code, "NFA_ZEND"); भङ्ग;

	पक्ष NFA_BACKREF1:  STRCPY(code, "NFA_BACKREF1"); भङ्ग;
	पक्ष NFA_BACKREF2:  STRCPY(code, "NFA_BACKREF2"); भङ्ग;
	पक्ष NFA_BACKREF3:  STRCPY(code, "NFA_BACKREF3"); भङ्ग;
	पक्ष NFA_BACKREF4:  STRCPY(code, "NFA_BACKREF4"); भङ्ग;
	पक्ष NFA_BACKREF5:  STRCPY(code, "NFA_BACKREF5"); भङ्ग;
	पक्ष NFA_BACKREF6:  STRCPY(code, "NFA_BACKREF6"); भङ्ग;
	पक्ष NFA_BACKREF7:  STRCPY(code, "NFA_BACKREF7"); भङ्ग;
	पक्ष NFA_BACKREF8:  STRCPY(code, "NFA_BACKREF8"); भङ्ग;
	पक्ष NFA_BACKREF9:  STRCPY(code, "NFA_BACKREF9"); भङ्ग;
# ifdef FEAT_SYN_HL
	पक्ष NFA_ZREF1:	    STRCPY(code, "NFA_ZREF1"); भङ्ग;
	पक्ष NFA_ZREF2:	    STRCPY(code, "NFA_ZREF2"); भङ्ग;
	पक्ष NFA_ZREF3:	    STRCPY(code, "NFA_ZREF3"); भङ्ग;
	पक्ष NFA_ZREF4:	    STRCPY(code, "NFA_ZREF4"); भङ्ग;
	पक्ष NFA_ZREF5:	    STRCPY(code, "NFA_ZREF5"); भङ्ग;
	पक्ष NFA_ZREF6:	    STRCPY(code, "NFA_ZREF6"); भङ्ग;
	पक्ष NFA_ZREF7:	    STRCPY(code, "NFA_ZREF7"); भङ्ग;
	पक्ष NFA_ZREF8:	    STRCPY(code, "NFA_ZREF8"); भङ्ग;
	पक्ष NFA_ZREF9:	    STRCPY(code, "NFA_ZREF9"); भङ्ग;
# endif
	पक्ष NFA_SKIP:	    STRCPY(code, "NFA_SKIP"); भङ्ग;

	पक्ष NFA_PREV_ATOM_NO_WIDTH:
			    STRCPY(code, "NFA_PREV_ATOM_NO_WIDTH"); भङ्ग;
	पक्ष NFA_PREV_ATOM_NO_WIDTH_NEG:
			    STRCPY(code, "NFA_PREV_ATOM_NO_WIDTH_NEG"); भङ्ग;
	पक्ष NFA_PREV_ATOM_JUST_BEFORE:
			    STRCPY(code, "NFA_PREV_ATOM_JUST_BEFORE"); भङ्ग;
	पक्ष NFA_PREV_ATOM_JUST_BEFORE_NEG:
			 STRCPY(code, "NFA_PREV_ATOM_JUST_BEFORE_NEG"); भङ्ग;
	पक्ष NFA_PREV_ATOM_LIKE_PATTERN:
			    STRCPY(code, "NFA_PREV_ATOM_LIKE_PATTERN"); भङ्ग;

	पक्ष NFA_NOPEN:		    STRCPY(code, "NFA_NOPEN"); भङ्ग;
	पक्ष NFA_NCLOSE:	    STRCPY(code, "NFA_NCLOSE"); भङ्ग;
	पक्ष NFA_START_INVISIBLE:   STRCPY(code, "NFA_START_INVISIBLE"); भङ्ग;
	पक्ष NFA_START_INVISIBLE_FIRST:
			     STRCPY(code, "NFA_START_INVISIBLE_FIRST"); भङ्ग;
	पक्ष NFA_START_INVISIBLE_NEG:
			       STRCPY(code, "NFA_START_INVISIBLE_NEG"); भङ्ग;
	पक्ष NFA_START_INVISIBLE_NEG_FIRST:
			 STRCPY(code, "NFA_START_INVISIBLE_NEG_FIRST"); भङ्ग;
	पक्ष NFA_START_INVISIBLE_BEFORE:
			    STRCPY(code, "NFA_START_INVISIBLE_BEFORE"); भङ्ग;
	पक्ष NFA_START_INVISIBLE_BEFORE_FIRST:
		      STRCPY(code, "NFA_START_INVISIBLE_BEFORE_FIRST"); भङ्ग;
	पक्ष NFA_START_INVISIBLE_BEFORE_NEG:
			STRCPY(code, "NFA_START_INVISIBLE_BEFORE_NEG"); भङ्ग;
	पक्ष NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
		  STRCPY(code, "NFA_START_INVISIBLE_BEFORE_NEG_FIRST"); भङ्ग;
	पक्ष NFA_START_PATTERN:   STRCPY(code, "NFA_START_PATTERN"); भङ्ग;
	पक्ष NFA_END_INVISIBLE:	    STRCPY(code, "NFA_END_INVISIBLE"); भङ्ग;
	पक्ष NFA_END_INVISIBLE_NEG: STRCPY(code, "NFA_END_INVISIBLE_NEG"); भङ्ग;
	पक्ष NFA_END_PATTERN:	    STRCPY(code, "NFA_END_PATTERN"); भङ्ग;

	पक्ष NFA_COMPOSING:	    STRCPY(code, "NFA_COMPOSING"); भङ्ग;
	पक्ष NFA_END_COMPOSING:	    STRCPY(code, "NFA_END_COMPOSING"); भङ्ग;
	पक्ष NFA_OPT_CHARS:	    STRCPY(code, "NFA_OPT_CHARS"); भङ्ग;

	पक्ष NFA_MOPEN:
	पक्ष NFA_MOPEN1:
	पक्ष NFA_MOPEN2:
	पक्ष NFA_MOPEN3:
	पक्ष NFA_MOPEN4:
	पक्ष NFA_MOPEN5:
	पक्ष NFA_MOPEN6:
	पक्ष NFA_MOPEN7:
	पक्ष NFA_MOPEN8:
	पक्ष NFA_MOPEN9:
	    STRCPY(code, "NFA_MOPEN(x)");
	    code[10] = c - NFA_MOPEN + '0';
	    भङ्ग;
	पक्ष NFA_MCLOSE:
	पक्ष NFA_MCLOSE1:
	पक्ष NFA_MCLOSE2:
	पक्ष NFA_MCLOSE3:
	पक्ष NFA_MCLOSE4:
	पक्ष NFA_MCLOSE5:
	पक्ष NFA_MCLOSE6:
	पक्ष NFA_MCLOSE7:
	पक्ष NFA_MCLOSE8:
	पक्ष NFA_MCLOSE9:
	    STRCPY(code, "NFA_MCLOSE(x)");
	    code[11] = c - NFA_MCLOSE + '0';
	    भङ्ग;
# ifdef FEAT_SYN_HL
	पक्ष NFA_ZOPEN:
	पक्ष NFA_ZOPEN1:
	पक्ष NFA_ZOPEN2:
	पक्ष NFA_ZOPEN3:
	पक्ष NFA_ZOPEN4:
	पक्ष NFA_ZOPEN5:
	पक्ष NFA_ZOPEN6:
	पक्ष NFA_ZOPEN7:
	पक्ष NFA_ZOPEN8:
	पक्ष NFA_ZOPEN9:
	    STRCPY(code, "NFA_ZOPEN(x)");
	    code[10] = c - NFA_ZOPEN + '0';
	    भङ्ग;
	पक्ष NFA_ZCLOSE:
	पक्ष NFA_ZCLOSE1:
	पक्ष NFA_ZCLOSE2:
	पक्ष NFA_ZCLOSE3:
	पक्ष NFA_ZCLOSE4:
	पक्ष NFA_ZCLOSE5:
	पक्ष NFA_ZCLOSE6:
	पक्ष NFA_ZCLOSE7:
	पक्ष NFA_ZCLOSE8:
	पक्ष NFA_ZCLOSE9:
	    STRCPY(code, "NFA_ZCLOSE(x)");
	    code[11] = c - NFA_ZCLOSE + '0';
	    भङ्ग;
# endif
	पक्ष NFA_EOL:		STRCPY(code, "NFA_EOL "); भङ्ग;
	पक्ष NFA_BOL:		STRCPY(code, "NFA_BOL "); भङ्ग;
	पक्ष NFA_EOW:		STRCPY(code, "NFA_EOW "); भङ्ग;
	पक्ष NFA_BOW:		STRCPY(code, "NFA_BOW "); भङ्ग;
	पक्ष NFA_EOF:		STRCPY(code, "NFA_EOF "); भङ्ग;
	पक्ष NFA_BOF:		STRCPY(code, "NFA_BOF "); भङ्ग;
	पक्ष NFA_LNUM:		STRCPY(code, "NFA_LNUM "); भङ्ग;
	पक्ष NFA_LNUM_GT:	STRCPY(code, "NFA_LNUM_GT "); भङ्ग;
	पक्ष NFA_LNUM_LT:	STRCPY(code, "NFA_LNUM_LT "); भङ्ग;
	पक्ष NFA_COL:		STRCPY(code, "NFA_COL "); भङ्ग;
	पक्ष NFA_COL_GT:	STRCPY(code, "NFA_COL_GT "); भङ्ग;
	पक्ष NFA_COL_LT:	STRCPY(code, "NFA_COL_LT "); भङ्ग;
	पक्ष NFA_VCOL:		STRCPY(code, "NFA_VCOL "); भङ्ग;
	पक्ष NFA_VCOL_GT:	STRCPY(code, "NFA_VCOL_GT "); भङ्ग;
	पक्ष NFA_VCOL_LT:	STRCPY(code, "NFA_VCOL_LT "); भङ्ग;
	पक्ष NFA_MARK:		STRCPY(code, "NFA_MARK "); भङ्ग;
	पक्ष NFA_MARK_GT:	STRCPY(code, "NFA_MARK_GT "); भङ्ग;
	पक्ष NFA_MARK_LT:	STRCPY(code, "NFA_MARK_LT "); भङ्ग;
	पक्ष NFA_CURSOR:	STRCPY(code, "NFA_CURSOR "); भङ्ग;
	पक्ष NFA_VISUAL:	STRCPY(code, "NFA_VISUAL "); भङ्ग;
	पक्ष NFA_ANY_COMPOSING:	STRCPY(code, "NFA_ANY_COMPOSING "); भङ्ग;

	पक्ष NFA_STAR:		STRCPY(code, "NFA_STAR "); भङ्ग;
	पक्ष NFA_STAR_NONGREEDY: STRCPY(code, "NFA_STAR_NONGREEDY "); भङ्ग;
	पक्ष NFA_QUEST:		STRCPY(code, "NFA_QUEST"); भङ्ग;
	पक्ष NFA_QUEST_NONGREEDY: STRCPY(code, "NFA_QUEST_NON_GREEDY"); भङ्ग;
	पक्ष NFA_EMPTY:		STRCPY(code, "NFA_EMPTY"); भङ्ग;
	पक्ष NFA_OR:		STRCPY(code, "NFA_OR"); भङ्ग;

	पक्ष NFA_START_COLL:	STRCPY(code, "NFA_START_COLL"); भङ्ग;
	पक्ष NFA_END_COLL:	STRCPY(code, "NFA_END_COLL"); भङ्ग;
	पक्ष NFA_START_NEG_COLL: STRCPY(code, "NFA_START_NEG_COLL"); भङ्ग;
	पक्ष NFA_END_NEG_COLL:	STRCPY(code, "NFA_END_NEG_COLL"); भङ्ग;
	पक्ष NFA_RANGE:		STRCPY(code, "NFA_RANGE"); भङ्ग;
	पक्ष NFA_RANGE_MIN:	STRCPY(code, "NFA_RANGE_MIN"); भङ्ग;
	पक्ष NFA_RANGE_MAX:	STRCPY(code, "NFA_RANGE_MAX"); भङ्ग;

	पक्ष NFA_CLASS_ALNUM:	STRCPY(code, "NFA_CLASS_ALNUM"); भङ्ग;
	पक्ष NFA_CLASS_ALPHA:	STRCPY(code, "NFA_CLASS_ALPHA"); भङ्ग;
	पक्ष NFA_CLASS_BLANK:	STRCPY(code, "NFA_CLASS_BLANK"); भङ्ग;
	पक्ष NFA_CLASS_CNTRL:	STRCPY(code, "NFA_CLASS_CNTRL"); भङ्ग;
	पक्ष NFA_CLASS_DIGIT:	STRCPY(code, "NFA_CLASS_DIGIT"); भङ्ग;
	पक्ष NFA_CLASS_GRAPH:	STRCPY(code, "NFA_CLASS_GRAPH"); भङ्ग;
	पक्ष NFA_CLASS_LOWER:	STRCPY(code, "NFA_CLASS_LOWER"); भङ्ग;
	पक्ष NFA_CLASS_PRINT:	STRCPY(code, "NFA_CLASS_PRINT"); भङ्ग;
	पक्ष NFA_CLASS_PUNCT:	STRCPY(code, "NFA_CLASS_PUNCT"); भङ्ग;
	पक्ष NFA_CLASS_SPACE:	STRCPY(code, "NFA_CLASS_SPACE"); भङ्ग;
	पक्ष NFA_CLASS_UPPER:	STRCPY(code, "NFA_CLASS_UPPER"); भङ्ग;
	पक्ष NFA_CLASS_XDIGIT:	STRCPY(code, "NFA_CLASS_XDIGIT"); भङ्ग;
	पक्ष NFA_CLASS_TAB:	STRCPY(code, "NFA_CLASS_TAB"); भङ्ग;
	पक्ष NFA_CLASS_RETURN:	STRCPY(code, "NFA_CLASS_RETURN"); भङ्ग;
	पक्ष NFA_CLASS_BACKSPACE:   STRCPY(code, "NFA_CLASS_BACKSPACE"); भङ्ग;
	पक्ष NFA_CLASS_ESCAPE:	STRCPY(code, "NFA_CLASS_ESCAPE"); भङ्ग;
	पक्ष NFA_CLASS_IDENT:	STRCPY(code, "NFA_CLASS_IDENT"); भङ्ग;
	पक्ष NFA_CLASS_KEYWORD:	STRCPY(code, "NFA_CLASS_KEYWORD"); भङ्ग;
	पक्ष NFA_CLASS_FNAME:	STRCPY(code, "NFA_CLASS_FNAME"); भङ्ग;

	पक्ष NFA_ANY:	STRCPY(code, "NFA_ANY"); भङ्ग;
	पक्ष NFA_IDENT:	STRCPY(code, "NFA_IDENT"); भङ्ग;
	पक्ष NFA_SIDENT:STRCPY(code, "NFA_SIDENT"); भङ्ग;
	पक्ष NFA_KWORD:	STRCPY(code, "NFA_KWORD"); भङ्ग;
	पक्ष NFA_SKWORD:STRCPY(code, "NFA_SKWORD"); भङ्ग;
	पक्ष NFA_FNAME:	STRCPY(code, "NFA_FNAME"); भङ्ग;
	पक्ष NFA_SFNAME:STRCPY(code, "NFA_SFNAME"); भङ्ग;
	पक्ष NFA_PRINT:	STRCPY(code, "NFA_PRINT"); भङ्ग;
	पक्ष NFA_SPRINT:STRCPY(code, "NFA_SPRINT"); भङ्ग;
	पक्ष NFA_WHITE:	STRCPY(code, "NFA_WHITE"); भङ्ग;
	पक्ष NFA_NWHITE:STRCPY(code, "NFA_NWHITE"); भङ्ग;
	पक्ष NFA_DIGIT:	STRCPY(code, "NFA_DIGIT"); भङ्ग;
	पक्ष NFA_NDIGIT:STRCPY(code, "NFA_NDIGIT"); भङ्ग;
	पक्ष NFA_HEX:	STRCPY(code, "NFA_HEX"); भङ्ग;
	पक्ष NFA_NHEX:	STRCPY(code, "NFA_NHEX"); भङ्ग;
	पक्ष NFA_OCTAL:	STRCPY(code, "NFA_OCTAL"); भङ्ग;
	पक्ष NFA_NOCTAL:STRCPY(code, "NFA_NOCTAL"); भङ्ग;
	पक्ष NFA_WORD:	STRCPY(code, "NFA_WORD"); भङ्ग;
	पक्ष NFA_NWORD:	STRCPY(code, "NFA_NWORD"); भङ्ग;
	पक्ष NFA_HEAD:	STRCPY(code, "NFA_HEAD"); भङ्ग;
	पक्ष NFA_NHEAD:	STRCPY(code, "NFA_NHEAD"); भङ्ग;
	पक्ष NFA_ALPHA:	STRCPY(code, "NFA_ALPHA"); भङ्ग;
	पक्ष NFA_NALPHA:STRCPY(code, "NFA_NALPHA"); भङ्ग;
	पक्ष NFA_LOWER:	STRCPY(code, "NFA_LOWER"); भङ्ग;
	पक्ष NFA_NLOWER:STRCPY(code, "NFA_NLOWER"); भङ्ग;
	पक्ष NFA_UPPER:	STRCPY(code, "NFA_UPPER"); भङ्ग;
	पक्ष NFA_NUPPER:STRCPY(code, "NFA_NUPPER"); भङ्ग;
	पक्ष NFA_LOWER_IC:  STRCPY(code, "NFA_LOWER_IC"); भङ्ग;
	पक्ष NFA_NLOWER_IC: STRCPY(code, "NFA_NLOWER_IC"); भङ्ग;
	पक्ष NFA_UPPER_IC:  STRCPY(code, "NFA_UPPER_IC"); भङ्ग;
	पक्ष NFA_NUPPER_IC: STRCPY(code, "NFA_NUPPER_IC"); भङ्ग;

	स्वभाव:
	    STRCPY(code, "CHAR(x)");
	    code[5] = c;
    }

    यदि (addnl == TRUE)
	STRCAT(code, " + NEWLINE ");

}

# ifdef ENABLE_LOG
स्थिर FILE *log_fd;
स्थिर char_u e_log_open_failed[] = N_("Could not open temporary log file for writing, displaying on stderr... ");

/*
 * Print the postfix notation of the current regexp.
 */
    स्थिर शून्य
nfa_postfix_dump(char_u *अभिव्यक्ती, पूर्णाङ्क retval)
{
    पूर्णाङ्क *p;
    FILE *f;

    f = fopen(NFA_REGEXP_DUMP_LOG, "a");
    यदि (f == रिक्त)
	प्रत्यागम;

    fprintf(f, "\n-------------------------\n");
    यदि (retval == FAIL)
	fprintf(f, ">>> NFA engine failed... \n");
    अन्यथा यदि (retval == OK)
	fprintf(f, ">>> NFA engine succeeded !\n");
    fprintf(f, "Regexp: \"%s\"\nPostfix notation (char): \"", अभिव्यक्ती);
    प्रति (p = post_start; *p && p < post_ptr; p++)
    {
	nfa_set_code(*p);
	fprintf(f, "%s, ", code);
    }
    fprintf(f, "\"\nPostfix notation (int): ");
    प्रति (p = post_start; *p && p < post_ptr; p++)
	fprintf(f, "%d ", *p);
    fprintf(f, "\n\n");
    fclose(f);
}

/*
 * Print the NFA starting with a root node "state".
 */
    स्थिर शून्य
nfa_print_state(FILE *debugf, nfa_state_T *state)
{
    garray_T indent;

    ga_init2(&indent, 1, 64);
    ga_append(&indent, '\0');
    nfa_print_state2(debugf, state, &indent);
    ga_clear(&indent);
}

    स्थिर शून्य
nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent)
{
    char_u  *p;

    यदि (state == रिक्त)
	प्रत्यागम;

    fprintf(debugf, "(%2d)", abs(state->id));

    // Output indent
    p = (char_u *)indent->ga_data;
    यदि (indent->ga_len >= 3)
    {
	पूर्णाङ्क	last = indent->ga_len - 3;
	char_u	save[2];

	STRNCPY(save, &p[last], 2);
	memcpy(&p[last], "+-", 2);
	fprintf(debugf, " %s", p);
	STRNCPY(&p[last], save, 2);
    }
    अन्यथा
	fprintf(debugf, " %s", p);

    nfa_set_code(state->c);
    fprintf(debugf, "%s (%d) (id=%d) val=%d\n",
		 code,
		 state->c,
		 abs(state->id),
		 state->val);
    यदि (state->id < 0)
	प्रत्यागम;

    state->id = abs(state->id) * -1;

    // grow indent for state->out
    indent->ga_len -= 1;
    यदि (state->out1)
	GA_CONCAT_LITERAL(indent, "| ");
    अन्यथा
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
    स्थिर शून्य
nfa_dump(nfa_regprog_T *prog)
{
    FILE *debugf = fopen(NFA_REGEXP_DUMP_LOG, "a");

    यदि (debugf == रिक्त)
	प्रत्यागम;

    nfa_print_state(debugf, prog->start);

    यदि (prog->reganch)
	fprintf(debugf, "reganch: %d\n", prog->reganch);
    यदि (prog->regstart != NUL)
	fprintf(debugf, "regstart: %c (decimal: %d)\n",
		prog->regstart, prog->regstart);
    यदि (prog->match_text != रिक्त)
	fprintf(debugf, "match_text: \"%s\"\n", prog->match_text);

    fclose(debugf);
}
# endif	    // ENABLE_LOG
#endif	    // DEBUG

/*
 * Parse r.e. @expr and convert it into postfix form.
 * Return the postfix string on success, NULL otherwise.
 */
    स्थिर पूर्णाङ्क *
re2post(शून्य)
{
    यदि (nfa_reg(REG_NOPAREN) == FAIL)
	प्रत्यागम रिक्त;
    EMIT(NFA_MOPEN);
    प्रत्यागम post_start;
}

// NB. Some of the code below is inspired by Russ's.

/*
 * Represents an NFA state plus zero or one or two arrows exiting.
 * if c == MATCH, no arrows out; matching state.
 * If c == SPLIT, unlabeled arrows to out and out1 (if != NULL).
 * If c < 256, labeled arrow with character c to out.
 */

स्थिर nfa_state_T	*state_ptr; // points to nfa_prog->state

/*
 * Allocate and initialize nfa_state_T.
 */
    स्थिर nfa_state_T *
alloc_state(पूर्णाङ्क c, nfa_state_T *out, nfa_state_T *out1)
{
    nfa_state_T *s;

    यदि (istate >= nstate)
	प्रत्यागम रिक्त;

    s = &state_ptr[istate++];

    s->c    = c;
    s->out  = out;
    s->out1 = out1;
    s->val  = 0;

    s->id   = istate;
    s->lastlist[0] = 0;
    s->lastlist[1] = 0;

    प्रत्यागम s;
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
प्रकारनिर्देश सङ्घ Ptrlist Ptrlist;
सङ्घ Ptrlist
{
    Ptrlist	*next;
    nfa_state_T	*s;
};

संरचना Frag
{
    nfa_state_T *start;
    Ptrlist	*out;
};
प्रकारनिर्देश संरचना Frag Frag_T;

// Reused across compilations by post2nfa(); freed in free_regexp_stuff().
स्थिर Frag_T *nfa_चळत;
स्थिर पूर्णाङ्क nfa_चळत_len;

/*
 * Initialize a Frag_T struct and return it.
 */
    स्थिर Frag_T
frag(nfa_state_T *start, Ptrlist *out)
{
    Frag_T n;

    n.start = start;
    n.out = out;
    प्रत्यागम n;
}

/*
 * Create singleton list containing just outp.
 */
    स्थिर Ptrlist *
list1(
    nfa_state_T	**outp)
{
    Ptrlist *l;

    l = (Ptrlist *)outp;
    l->next = रिक्त;
    प्रत्यागम l;
}

/*
 * Patch the list of states at out to point to start.
 */
    स्थिर शून्य
patch(Ptrlist *l, nfa_state_T *s)
{
    Ptrlist *next;

    प्रति (; l; l = next)
    {
	next = l->next;
	l->s = s;
    }
}


/*
 * Join the two lists l1 and l2, returning the combination.
 */
    स्थिर Ptrlist *
append(Ptrlist *l1, Ptrlist *l2)
{
    Ptrlist *oldl1;

    oldl1 = l1;
    यावत् (l1->next)
	l1 = l1->next;
    l1->next = l2;
    प्रत्यागम oldl1;
}

/*
 * Stack used for transforming postfix form into NFA.
 */
स्थिर Frag_T empty;

    स्थिर शून्य
st_error(पूर्णाङ्क *postfix UNUSED, पूर्णाङ्क *end UNUSED, पूर्णाङ्क *p UNUSED)
{
#ifdef NFA_REGEXP_ERROR_LOG
    FILE *df;
    पूर्णाङ्क *p2;

    df = fopen(NFA_REGEXP_ERROR_LOG, "a");
    यदि (df)
    {
	fprintf(df, "Error popping the stack!\n");
# ifdef DEBUG
	fprintf(df, "Current regexp is \"%s\"\n", nfa_regengine.अभिव्यक्ती);
# endif
	fprintf(df, "Postfix form is: ");
# ifdef DEBUG
	प्रति (p2 = postfix; p2 < end; p2++)
	{
	    nfa_set_code(*p2);
	    fprintf(df, "%s, ", code);
	}
	nfa_set_code(*p);
	fprintf(df, "\nCurrent position is: ");
	प्रति (p2 = postfix; p2 <= p; p2 ++)
	{
	    nfa_set_code(*p2);
	    fprintf(df, "%s, ", code);
	}
# अन्यथा
	प्रति (p2 = postfix; p2 < end; p2++)
	    fprintf(df, "%d, ", *p2);
	fprintf(df, "\nCurrent position is: ");
	प्रति (p2 = postfix; p2 <= p; p2 ++)
	    fprintf(df, "%d, ", *p2);
# endif
	fprintf(df, "\n--------------------------\n");
	fclose(df);
    }
#endif
    emsg(_(e_nfa_regexp_could_not_काढा_चळत));
}

/*
 * Push an item onto the stack.
 */
    स्थिर शून्य
st_ढकला(Frag_T s, Frag_T **p, Frag_T *चळत_end)
{
    Frag_T *stackp = *p;

    यदि (stackp >= चळत_end)
	प्रत्यागम;
    *stackp = s;
    *p = *p + 1;
}

/*
 * Pop an item from the stack.
 */
    स्थिर Frag_T
st_काढा(Frag_T **p, Frag_T *चळत)
{
    Frag_T *stackp;

    *p = *p - 1;
    stackp = *p;
    यदि (stackp < चळत)
	प्रत्यागम empty;
    प्रत्यागम **p;
}

/*
 * Estimate the maximum byte length of anything matching "state".
 * When unknown or unlimited return -1.
 */
    स्थिर पूर्णाङ्क
nfa_max_width(nfa_state_T *startstate, पूर्णाङ्क depth)
{
    पूर्णाङ्क		    l, r;
    nfa_state_T	    *state = startstate;
    पूर्णाङ्क		    len = 0;

    // detect looping in a NFA_SPLIT
    यदि (depth > 4)
	प्रत्यागम -1;

    यावत् (state != रिक्त)
    {
	विकल्प (state->c)
	{
	    पक्ष NFA_END_INVISIBLE:
	    पक्ष NFA_END_INVISIBLE_NEG:
		// the end, return what we have
		प्रत्यागम len;

	    पक्ष NFA_SPLIT:
		// two alternatives, use the maximum
		l = nfa_max_width(state->out, depth + 1);
		r = nfa_max_width(state->out1, depth + 1);
		यदि (l < 0 || r < 0)
		    प्रत्यागम -1;
		प्रत्यागम len + (l > r ? l : r);

	    पक्ष NFA_ANY:
	    पक्ष NFA_START_COLL:
	    पक्ष NFA_START_NEG_COLL:
		// matches some character, including composing chars
		यदि (enc_utf8)
		    len += MB_MAXBYTES;
		अन्यथा यदि (has_mbyte)
		    len += 2;
		अन्यथा
		    ++len;
		यदि (state->c != NFA_ANY)
		{
		    // Skip over the compiled collection.
		    // malformed NFAs must not crash width estimation.
		    यदि (state->out1 == रिक्त || state->out1->out == रिक्त)
			प्रत्यागम -1;
		    state = state->out1->out;
		    अनुवर्त;
		}
		भङ्ग;

	    पक्ष NFA_DIGIT:
	    पक्ष NFA_WHITE:
	    पक्ष NFA_HEX:
	    पक्ष NFA_OCTAL:
		// ascii
		++len;
		भङ्ग;

	    पक्ष NFA_IDENT:
	    पक्ष NFA_SIDENT:
	    पक्ष NFA_KWORD:
	    पक्ष NFA_SKWORD:
	    पक्ष NFA_FNAME:
	    पक्ष NFA_SFNAME:
	    पक्ष NFA_PRINT:
	    पक्ष NFA_SPRINT:
	    पक्ष NFA_NWHITE:
	    पक्ष NFA_NDIGIT:
	    पक्ष NFA_NHEX:
	    पक्ष NFA_NOCTAL:
	    पक्ष NFA_WORD:
	    पक्ष NFA_NWORD:
	    पक्ष NFA_HEAD:
	    पक्ष NFA_NHEAD:
	    पक्ष NFA_ALPHA:
	    पक्ष NFA_NALPHA:
	    पक्ष NFA_LOWER:
	    पक्ष NFA_NLOWER:
	    पक्ष NFA_UPPER:
	    पक्ष NFA_NUPPER:
	    पक्ष NFA_LOWER_IC:
	    पक्ष NFA_NLOWER_IC:
	    पक्ष NFA_UPPER_IC:
	    पक्ष NFA_NUPPER_IC:
	    पक्ष NFA_ANY_COMPOSING:
		// possibly non-ascii
		यदि (has_mbyte)
		    len += 3;
		अन्यथा
		    ++len;
		भङ्ग;

	    पक्ष NFA_START_INVISIBLE:
	    पक्ष NFA_START_INVISIBLE_NEG:
	    पक्ष NFA_START_INVISIBLE_BEFORE:
	    पक्ष NFA_START_INVISIBLE_BEFORE_NEG:
		// zero-width, out1 points to the END state
		state = state->out1->out;
		अनुवर्त;

	    पक्ष NFA_BACKREF1:
	    पक्ष NFA_BACKREF2:
	    पक्ष NFA_BACKREF3:
	    पक्ष NFA_BACKREF4:
	    पक्ष NFA_BACKREF5:
	    पक्ष NFA_BACKREF6:
	    पक्ष NFA_BACKREF7:
	    पक्ष NFA_BACKREF8:
	    पक्ष NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	    पक्ष NFA_ZREF1:
	    पक्ष NFA_ZREF2:
	    पक्ष NFA_ZREF3:
	    पक्ष NFA_ZREF4:
	    पक्ष NFA_ZREF5:
	    पक्ष NFA_ZREF6:
	    पक्ष NFA_ZREF7:
	    पक्ष NFA_ZREF8:
	    पक्ष NFA_ZREF9:
#endif
	    पक्ष NFA_NEWL:
	    पक्ष NFA_SKIP:
		// unknown width
		प्रत्यागम -1;

	    पक्ष NFA_BOL:
	    पक्ष NFA_EOL:
	    पक्ष NFA_BOF:
	    पक्ष NFA_EOF:
	    पक्ष NFA_BOW:
	    पक्ष NFA_EOW:
	    पक्ष NFA_MOPEN:
	    पक्ष NFA_MOPEN1:
	    पक्ष NFA_MOPEN2:
	    पक्ष NFA_MOPEN3:
	    पक्ष NFA_MOPEN4:
	    पक्ष NFA_MOPEN5:
	    पक्ष NFA_MOPEN6:
	    पक्ष NFA_MOPEN7:
	    पक्ष NFA_MOPEN8:
	    पक्ष NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	    पक्ष NFA_ZOPEN:
	    पक्ष NFA_ZOPEN1:
	    पक्ष NFA_ZOPEN2:
	    पक्ष NFA_ZOPEN3:
	    पक्ष NFA_ZOPEN4:
	    पक्ष NFA_ZOPEN5:
	    पक्ष NFA_ZOPEN6:
	    पक्ष NFA_ZOPEN7:
	    पक्ष NFA_ZOPEN8:
	    पक्ष NFA_ZOPEN9:
	    पक्ष NFA_ZCLOSE:
	    पक्ष NFA_ZCLOSE1:
	    पक्ष NFA_ZCLOSE2:
	    पक्ष NFA_ZCLOSE3:
	    पक्ष NFA_ZCLOSE4:
	    पक्ष NFA_ZCLOSE5:
	    पक्ष NFA_ZCLOSE6:
	    पक्ष NFA_ZCLOSE7:
	    पक्ष NFA_ZCLOSE8:
	    पक्ष NFA_ZCLOSE9:
#endif
	    पक्ष NFA_MCLOSE:
	    पक्ष NFA_MCLOSE1:
	    पक्ष NFA_MCLOSE2:
	    पक्ष NFA_MCLOSE3:
	    पक्ष NFA_MCLOSE4:
	    पक्ष NFA_MCLOSE5:
	    पक्ष NFA_MCLOSE6:
	    पक्ष NFA_MCLOSE7:
	    पक्ष NFA_MCLOSE8:
	    पक्ष NFA_MCLOSE9:
	    पक्ष NFA_NOPEN:
	    पक्ष NFA_NCLOSE:

	    पक्ष NFA_LNUM_GT:
	    पक्ष NFA_LNUM_LT:
	    पक्ष NFA_COL_GT:
	    पक्ष NFA_COL_LT:
	    पक्ष NFA_VCOL_GT:
	    पक्ष NFA_VCOL_LT:
	    पक्ष NFA_MARK_GT:
	    पक्ष NFA_MARK_LT:
	    पक्ष NFA_VISUAL:
	    पक्ष NFA_LNUM:
	    पक्ष NFA_CURSOR:
	    पक्ष NFA_COL:
	    पक्ष NFA_VCOL:
	    पक्ष NFA_MARK:

	    पक्ष NFA_ZSTART:
	    पक्ष NFA_ZEND:
	    पक्ष NFA_OPT_CHARS:
	    पक्ष NFA_EMPTY:
	    पक्ष NFA_START_PATTERN:
	    पक्ष NFA_END_PATTERN:
	    पक्ष NFA_COMPOSING:
	    पक्ष NFA_END_COMPOSING:
		// zero-width
		भङ्ग;

	    स्वभाव:
		यदि (state->c < 0)
		    // don't know what this is
		    प्रत्यागम -1;
		// normal character
		len += MB_CHAR2LEN(state->c);
		भङ्ग;
	}

	// normal way to continue
	state = state->out;
    }

    // unrecognized, "cannot happen"
    प्रत्यागम -1;
}

/*
 * Convert a postfix form into its equivalent NFA.
 * Return the NFA start state on success, NULL otherwise.
 */
    स्थिर nfa_state_T *
post2nfa(पूर्णाङ्क *postfix, पूर्णाङ्क *end, पूर्णाङ्क nfa_calc_size)
{
    पूर्णाङ्क		*p;
    पूर्णाङ्क		mopen;
    पूर्णाङ्क		mclose;
    Frag_T	*चळत = रिक्त;
    Frag_T	*stackp = रिक्त;
    Frag_T	*चळत_end = रिक्त;
    Frag_T	e1;
    Frag_T	e2;
    Frag_T	e;
    nfa_state_T	*s;
    nfa_state_T	*s1;
    nfa_state_T	*matchstate;
    nfa_state_T	*ret = रिक्त;

    यदि (postfix == रिक्त)
	प्रत्यागम रिक्त;

#define ढकला_2(s)	    st_ढकला((s), &stackp, चळत_end)
#define काढा_2()	    st_काढा(&stackp, चळत);		\
		    यदि (stackp < चळत)			\
		    {					\
			st_error(postfix, end, p);	\
			प्रत्यागम रिक्त;			\
		    }

    यदि (nfa_calc_size == FALSE)
    {
	// Reuse the fragment stack across compilations, growing when needed;
	// it is freed in free_regexp_stuff().
	यदि (nfa_चळत == रिक्त || nfa_चळत_len < nstate + 1)
	{
	    vim_free(nfa_चळत);
	    nfa_चळत = ALLOC_MULT(Frag_T, nstate + 1);
	    यदि (nfa_चळत == रिक्त)
	    {
		nfa_चळत_len = 0;
		प्रत्यागम रिक्त;
	    }
	    nfa_चळत_len = nstate + 1;
	}
	चळत = nfa_चळत;
	stackp = चळत;
	चळत_end = चळत + (nstate + 1);
    }

    प्रति (p = postfix; p < end; ++p)
    {
	विकल्प (*p)
	{
	पक्ष NFA_CONCAT:
	    // Concatenation.
	    // Pay attention: this operator does not exist in the r.e. itself
	    // (it is implicit, really).  It is added when r.e. is translated
	    // to postfix form in re2post().
	    यदि (nfa_calc_size == TRUE)
	    {
		// nstate += 0;
		भङ्ग;
	    }
	    e2 = काढा_2();
	    e1 = काढा_2();
	    patch(e1.out, e2.start);
	    ढकला_2(frag(e1.start, e2.out));
	    भङ्ग;

	पक्ष NFA_OR:
	    // Alternation
	    यदि (nfa_calc_size == TRUE)
	    {
		nstate++;
		भङ्ग;
	    }
	    e2 = काढा_2();
	    e1 = काढा_2();
	    s = alloc_state(NFA_SPLIT, e1.start, e2.start);
	    यदि (s == रिक्त)
		गच्छ theend;
	    ढकला_2(frag(s, append(e1.out, e2.out)));
	    भङ्ग;

	पक्ष NFA_STAR:
	    // Zero or more, prefer more
	    यदि (nfa_calc_size == TRUE)
	    {
		nstate++;
		भङ्ग;
	    }
	    e = काढा_2();
	    s = alloc_state(NFA_SPLIT, e.start, रिक्त);
	    यदि (s == रिक्त)
		गच्छ theend;
	    patch(e.out, s);
	    ढकला_2(frag(s, list1(&s->out1)));
	    भङ्ग;

	पक्ष NFA_STAR_NONGREEDY:
	    // Zero or more, prefer zero
	    यदि (nfa_calc_size == TRUE)
	    {
		nstate++;
		भङ्ग;
	    }
	    e = काढा_2();
	    s = alloc_state(NFA_SPLIT, रिक्त, e.start);
	    यदि (s == रिक्त)
		गच्छ theend;
	    patch(e.out, s);
	    ढकला_2(frag(s, list1(&s->out)));
	    भङ्ग;

	पक्ष NFA_QUEST:
	    // one or zero atoms=> greedy match
	    यदि (nfa_calc_size == TRUE)
	    {
		nstate++;
		भङ्ग;
	    }
	    e = काढा_2();
	    s = alloc_state(NFA_SPLIT, e.start, रिक्त);
	    यदि (s == रिक्त)
		गच्छ theend;
	    ढकला_2(frag(s, append(e.out, list1(&s->out1))));
	    भङ्ग;

	पक्ष NFA_QUEST_NONGREEDY:
	    // zero or one atoms => non-greedy match
	    यदि (nfa_calc_size == TRUE)
	    {
		nstate++;
		भङ्ग;
	    }
	    e = काढा_2();
	    s = alloc_state(NFA_SPLIT, रिक्त, e.start);
	    यदि (s == रिक्त)
		गच्छ theend;
	    ढकला_2(frag(s, append(e.out, list1(&s->out))));
	    भङ्ग;

	पक्ष NFA_END_COLL:
	पक्ष NFA_END_NEG_COLL:
	    // On the stack is the sequence starting with NFA_START_COLL or
	    // NFA_START_NEG_COLL and all possible characters. Patch it to
	    // add the output to the start.
	    यदि (nfa_calc_size == TRUE)
	    {
		nstate++;
		भङ्ग;
	    }
	    e = काढा_2();
	    s = alloc_state(NFA_END_COLL, रिक्त, रिक्त);
	    यदि (s == रिक्त)
		गच्छ theend;
	    patch(e.out, s);
	    e.start->out1 = s;
	    ढकला_2(frag(e.start, list1(&s->out)));
	    भङ्ग;

	पक्ष NFA_RANGE:
	    // Before this are two characters, the low and high end of a
	    // range.  Turn them into two states with MIN and MAX.
	    यदि (nfa_calc_size == TRUE)
	    {
		// nstate += 0;
		भङ्ग;
	    }
	    e2 = काढा_2();
	    e1 = काढा_2();
	    e2.start->val = e2.start->c;
	    e2.start->c = NFA_RANGE_MAX;
	    e1.start->val = e1.start->c;
	    e1.start->c = NFA_RANGE_MIN;
	    patch(e1.out, e2.start);
	    ढकला_2(frag(e1.start, e2.out));
	    भङ्ग;

	पक्ष NFA_EMPTY:
	    // 0-length, used in a repetition with max/min count of 0
	    यदि (nfa_calc_size == TRUE)
	    {
		nstate++;
		भङ्ग;
	    }
	    s = alloc_state(NFA_EMPTY, रिक्त, रिक्त);
	    यदि (s == रिक्त)
		गच्छ theend;
	    ढकला_2(frag(s, list1(&s->out)));
	    भङ्ग;

	पक्ष NFA_OPT_CHARS:
	  {
	    पूर्णाङ्क    n;

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
	    यदि (nfa_calc_size == TRUE)
	    {
		nstate += n;
		भङ्ग;
	    }
	    s = रिक्त; // avoid compiler warning
	    e1.out = रिक्त; // stores list with out1's
	    s1 = रिक्त; // previous NFA_SPLIT to connect to
	    यावत् (n-- > 0)
	    {
		e = काढा_2(); // get character
		s = alloc_state(NFA_SPLIT, e.start, रिक्त);
		यदि (s == रिक्त)
		    गच्छ theend;
		यदि (e1.out == रिक्त)
		    e1 = e;
		patch(e.out, s1);
		append(e1.out, list1(&s->out1));
		s1 = s;
	    }
	    ढकला_2(frag(s, e1.out));
	    भङ्ग;
	  }

	पक्ष NFA_PREV_ATOM_NO_WIDTH:
	पक्ष NFA_PREV_ATOM_NO_WIDTH_NEG:
	पक्ष NFA_PREV_ATOM_JUST_BEFORE:
	पक्ष NFA_PREV_ATOM_JUST_BEFORE_NEG:
	पक्ष NFA_PREV_ATOM_LIKE_PATTERN:
	  {
	    पूर्णाङ्क before = (*p == NFA_PREV_ATOM_JUST_BEFORE
				      || *p == NFA_PREV_ATOM_JUST_BEFORE_NEG);
	    पूर्णाङ्क pattern = (*p == NFA_PREV_ATOM_LIKE_PATTERN);
	    पूर्णाङ्क start_state;
	    पूर्णाङ्क end_state;
	    पूर्णाङ्क n = 0;
	    nfa_state_T *zend;
	    nfa_state_T *skip;

	    विकल्प (*p)
	    {
		पक्ष NFA_PREV_ATOM_NO_WIDTH:
		    start_state = NFA_START_INVISIBLE;
		    end_state = NFA_END_INVISIBLE;
		    भङ्ग;
		पक्ष NFA_PREV_ATOM_NO_WIDTH_NEG:
		    start_state = NFA_START_INVISIBLE_NEG;
		    end_state = NFA_END_INVISIBLE_NEG;
		    भङ्ग;
		पक्ष NFA_PREV_ATOM_JUST_BEFORE:
		    start_state = NFA_START_INVISIBLE_BEFORE;
		    end_state = NFA_END_INVISIBLE;
		    भङ्ग;
		पक्ष NFA_PREV_ATOM_JUST_BEFORE_NEG:
		    start_state = NFA_START_INVISIBLE_BEFORE_NEG;
		    end_state = NFA_END_INVISIBLE_NEG;
		    भङ्ग;
		स्वभाव: // NFA_PREV_ATOM_LIKE_PATTERN:
		    start_state = NFA_START_PATTERN;
		    end_state = NFA_END_PATTERN;
		    भङ्ग;
	    }

	    यदि (before)
		n = *++p; // get the count

	    // The \@= operator: match the preceding atom with zero width.
	    // The \@! operator: no match for the preceding atom.
	    // The \@<= operator: match for the preceding atom.
	    // The \@<! operator: no match for the preceding atom.
	    // Surrounds the preceding atom with START_INVISIBLE and
	    // END_INVISIBLE, similarly to MOPEN.

	    यदि (nfa_calc_size == TRUE)
	    {
		nstate += pattern ? 4 : 2;
		भङ्ग;
	    }
	    e = काढा_2();
	    s1 = alloc_state(end_state, रिक्त, रिक्त);
	    यदि (s1 == रिक्त)
		गच्छ theend;

	    s = alloc_state(start_state, e.start, s1);
	    यदि (s == रिक्त)
		गच्छ theend;
	    यदि (pattern)
	    {
		// NFA_ZEND -> NFA_END_PATTERN -> NFA_SKIP -> what follows.
		skip = alloc_state(NFA_SKIP, रिक्त, रिक्त);
		यदि (skip == रिक्त)
		    गच्छ theend;
		zend = alloc_state(NFA_ZEND, s1, रिक्त);
		यदि (zend == रिक्त)
		    गच्छ theend;
		s1->out= skip;
		patch(e.out, zend);
		ढकला_2(frag(s, list1(&skip->out)));
	    }
	    अन्यथा
	    {
		patch(e.out, s1);
		ढकला_2(frag(s, list1(&s1->out)));
		यदि (before)
		{
		    यदि (n <= 0)
			// See if we can guess the maximum width, it avoids a
			// lot of pointless tries.
			n = nfa_max_width(e.start, 0);
		    s->val = n; // store the count
		}
	    }
	    भङ्ग;
	  }

	पक्ष NFA_COMPOSING:	// char with composing char
#यदि 0
	    // TODO
	    यदि (regflags & RF_ICOMBINE)
	    {
		// use the base character only
	    }
#endif
	    // FALLTHROUGH

	पक्ष NFA_MOPEN:	// \( \) Submatch
	पक्ष NFA_MOPEN1:
	पक्ष NFA_MOPEN2:
	पक्ष NFA_MOPEN3:
	पक्ष NFA_MOPEN4:
	पक्ष NFA_MOPEN5:
	पक्ष NFA_MOPEN6:
	पक्ष NFA_MOPEN7:
	पक्ष NFA_MOPEN8:
	पक्ष NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	पक्ष NFA_ZOPEN:	// \z( \) Submatch
	पक्ष NFA_ZOPEN1:
	पक्ष NFA_ZOPEN2:
	पक्ष NFA_ZOPEN3:
	पक्ष NFA_ZOPEN4:
	पक्ष NFA_ZOPEN5:
	पक्ष NFA_ZOPEN6:
	पक्ष NFA_ZOPEN7:
	पक्ष NFA_ZOPEN8:
	पक्ष NFA_ZOPEN9:
#endif
	पक्ष NFA_NOPEN:	// \%( \) "Invisible Submatch"
	    यदि (nfa_calc_size == TRUE)
	    {
		nstate += 2;
		भङ्ग;
	    }

	    mopen = *p;
	    विकल्प (*p)
	    {
		पक्ष NFA_NOPEN: mclose = NFA_NCLOSE; भङ्ग;
#ifdef FEAT_SYN_HL
		पक्ष NFA_ZOPEN: mclose = NFA_ZCLOSE; भङ्ग;
		पक्ष NFA_ZOPEN1: mclose = NFA_ZCLOSE1; भङ्ग;
		पक्ष NFA_ZOPEN2: mclose = NFA_ZCLOSE2; भङ्ग;
		पक्ष NFA_ZOPEN3: mclose = NFA_ZCLOSE3; भङ्ग;
		पक्ष NFA_ZOPEN4: mclose = NFA_ZCLOSE4; भङ्ग;
		पक्ष NFA_ZOPEN5: mclose = NFA_ZCLOSE5; भङ्ग;
		पक्ष NFA_ZOPEN6: mclose = NFA_ZCLOSE6; भङ्ग;
		पक्ष NFA_ZOPEN7: mclose = NFA_ZCLOSE7; भङ्ग;
		पक्ष NFA_ZOPEN8: mclose = NFA_ZCLOSE8; भङ्ग;
		पक्ष NFA_ZOPEN9: mclose = NFA_ZCLOSE9; भङ्ग;
#endif
		पक्ष NFA_COMPOSING: mclose = NFA_END_COMPOSING; भङ्ग;
		स्वभाव:
		    // NFA_MOPEN, NFA_MOPEN1 .. NFA_MOPEN9
		    mclose = *p + NSUBEXP;
		    भङ्ग;
	    }

	    // Allow "NFA_MOPEN" as a valid postfix representation for
	    // the empty regexp "". In this case, the NFA will be
	    // NFA_MOPEN -> NFA_MCLOSE. Note that this also allows
	    // empty groups of parenthesis, and empty mbyte chars
	    यदि (stackp == चळत)
	    {
		s = alloc_state(mopen, रिक्त, रिक्त);
		यदि (s == रिक्त)
		    गच्छ theend;
		s1 = alloc_state(mclose, रिक्त, रिक्त);
		यदि (s1 == रिक्त)
		    गच्छ theend;
		patch(list1(&s->out), s1);
		ढकला_2(frag(s, list1(&s1->out)));
		भङ्ग;
	    }

	    // At least one node was emitted before NFA_MOPEN, so
	    // at least one node will be between NFA_MOPEN and NFA_MCLOSE
	    e = काढा_2();
	    s = alloc_state(mopen, e.start, रिक्त);   // `('
	    यदि (s == रिक्त)
		गच्छ theend;

	    s1 = alloc_state(mclose, रिक्त, रिक्त);   // `)'
	    यदि (s1 == रिक्त)
		गच्छ theend;
	    patch(e.out, s1);

	    यदि (mopen == NFA_COMPOSING)
		// COMPOSING->out1 = END_COMPOSING
		patch(list1(&s->out1), s1);

	    ढकला_2(frag(s, list1(&s1->out)));
	    भङ्ग;

	पक्ष NFA_BACKREF1:
	पक्ष NFA_BACKREF2:
	पक्ष NFA_BACKREF3:
	पक्ष NFA_BACKREF4:
	पक्ष NFA_BACKREF5:
	पक्ष NFA_BACKREF6:
	पक्ष NFA_BACKREF7:
	पक्ष NFA_BACKREF8:
	पक्ष NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	पक्ष NFA_ZREF1:
	पक्ष NFA_ZREF2:
	पक्ष NFA_ZREF3:
	पक्ष NFA_ZREF4:
	पक्ष NFA_ZREF5:
	पक्ष NFA_ZREF6:
	पक्ष NFA_ZREF7:
	पक्ष NFA_ZREF8:
	पक्ष NFA_ZREF9:
#endif
	    यदि (nfa_calc_size == TRUE)
	    {
		nstate += 2;
		भङ्ग;
	    }
	    s = alloc_state(*p, रिक्त, रिक्त);
	    यदि (s == रिक्त)
		गच्छ theend;
	    s1 = alloc_state(NFA_SKIP, रिक्त, रिक्त);
	    यदि (s1 == रिक्त)
		गच्छ theend;
	    patch(list1(&s->out), s1);
	    ढकला_2(frag(s, list1(&s1->out)));
	    भङ्ग;

	पक्ष NFA_LNUM:
	पक्ष NFA_LNUM_GT:
	पक्ष NFA_LNUM_LT:
	पक्ष NFA_VCOL:
	पक्ष NFA_VCOL_GT:
	पक्ष NFA_VCOL_LT:
	पक्ष NFA_COL:
	पक्ष NFA_COL_GT:
	पक्ष NFA_COL_LT:
	पक्ष NFA_MARK:
	पक्ष NFA_MARK_GT:
	पक्ष NFA_MARK_LT:
	  {
	    पूर्णाङ्क n = *++p; // lnum, col or mark name

	    यदि (nfa_calc_size == TRUE)
	    {
		nstate += 1;
		भङ्ग;
	    }
	    s = alloc_state(p[-1], रिक्त, रिक्त);
	    यदि (s == रिक्त)
		गच्छ theend;
	    s->val = n;
	    ढकला_2(frag(s, list1(&s->out)));
	    भङ्ग;
	  }

	पक्ष NFA_ZSTART:
	पक्ष NFA_ZEND:
	स्वभाव:
	    // Operands
	    यदि (nfa_calc_size == TRUE)
	    {
		nstate++;
		भङ्ग;
	    }
	    s = alloc_state(*p, रिक्त, रिक्त);
	    यदि (s == रिक्त)
		गच्छ theend;
	    ढकला_2(frag(s, list1(&s->out)));
	    भङ्ग;

	} // switch(*p)

    } // for (p = postfix; *p; ++p)

    यदि (nfa_calc_size == TRUE)
    {
	nstate++;
	गच्छ theend;	// Return value when counting size is ignored anyway
    }

    e = काढा_2();
    यदि (stackp != चळत)
	EMSG_RET_NULL(_(e_nfa_regexp_while_converting_from_postfix_to_nfa_too_many_stats_डावा_on_चळत));

    यदि (istate >= nstate)
	EMSG_RET_NULL(_(e_nfa_regexp_not_enough_space_to_store_whole_nfa));

    matchstate = &state_ptr[istate++]; // the match state
    matchstate->c = NFA_MATCH;
    matchstate->out = matchstate->out1 = रिक्त;
    matchstate->id = 0;

    patch(e.out, matchstate);
    ret = e.start;

theend:
    प्रत्यागम ret;

#undef POP1
#undef PUSH1
#undef POP2
#undef PUSH2
#undef काढा_2
#undef ढकला_2
}

/*
 * After building the NFA program, inspect it to add optimization hints.
 */
    स्थिर शून्य
nfa_postprocess(nfa_regprog_T *prog)
{
    पूर्णाङ्क i;
    पूर्णाङ्क c;

    प्रति (i = 0; i < prog->nstate; ++i)
    {
	c = prog->state[i].c;
	यदि (c == NFA_START_INVISIBLE
		|| c == NFA_START_INVISIBLE_NEG
		|| c == NFA_START_INVISIBLE_BEFORE
		|| c == NFA_START_INVISIBLE_BEFORE_NEG)
	{
	    पूर्णाङ्क directly;

	    // Do it directly when what follows is possibly the end of the
	    // match.
	    यदि (match_follows(prog->state[i].out1->out, 0))
		directly = TRUE;
	    अन्यथा
	    {
		पूर्णाङ्क ch_invisible = failure_chance(prog->state[i].out, 0);
		पूर्णाङ्क ch_follows = failure_chance(prog->state[i].out1->out, 0);

		// Postpone when the invisible match is expensive or has a
		// lower chance of failing.
		यदि (c == NFA_START_INVISIBLE_BEFORE
		     || c == NFA_START_INVISIBLE_BEFORE_NEG)
		{
		    // "before" matches are very expensive when
		    // unbounded, always prefer what follows then,
		    // unless what follows will always match.
		    // Otherwise strongly prefer what follows.
		    यदि (prog->state[i].val <= 0 && ch_follows > 0)
			directly = FALSE;
		    अन्यथा
			directly = ch_follows * 10 < ch_invisible;
		}
		अन्यथा
		{
		    // normal invisible, first do the one with the
		    // highest failure chance
		    directly = ch_follows < ch_invisible;
		}
	    }
	    यदि (directly)
		// switch to the _FIRST state
		++prog->state[i].c;
	}
    }
}

/////////////////////////////////////////////////////////////////
// NFA execution code.
/////////////////////////////////////////////////////////////////

प्रकारनिर्देश संरचना
{
    पूर्णाङ्क	    in_use; // number of subexpr with useful info

    // When REG_MULTI is TRUE list.multi is used, otherwise list.line.
    सङ्घ
    {
	संरचना multipos
	{
	    linenr_T	start_lnum;
	    linenr_T	end_lnum;
	    colnr_T	start_col;
	    colnr_T	end_col;
	} multi[NSUBEXP];
	संरचना linepos
	{
	    char_u	*start;
	    char_u	*end;
	} ओळ[NSUBEXP];
    } list;
    colnr_T	orig_start_col;  // list.multi[0].start_col without \zs
} regsub_T;

प्रकारनिर्देश संरचना
{
    regsub_T	norm; // \( .. \) matches
#ifdef FEAT_SYN_HL
    regsub_T	synt; // \z( .. \) matches
#endif
} regsubs_T;

// nfa_pim_T stores a Postponed Invisible Match.
प्रकारनिर्देश संरचना nfa_pim_S nfa_pim_T;
संरचना nfa_pim_S
{
    पूर्णाङ्क		result;		// NFA_PIM_*, see below
    nfa_state_T	*state;		// the invisible match start state
    regsubs_T	subs;		// submatch info, only party used
    सङ्घ
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
प्रकारनिर्देश संरचना
{
    nfa_state_T	*state;
    पूर्णाङ्क		मोजणी;
    nfa_pim_T	pim;		// if pim.result != NFA_PIM_UNUSED: postponed
				// invisible match
    regsubs_T	subs;		// submatch info, only party used
} nfa_thread_T;

// nfa_list_T contains the alternative NFA execution states.
प्रकारनिर्देश संरचना
{
    nfa_thread_T    *t;		// allocated array of states
    पूर्णाङ्क		    n;		// nr of states currently in "t"
    पूर्णाङ्क		    len;	// max nr of states in "t"
    पूर्णाङ्क		    id;		// ID of the list
    पूर्णाङ्क		    has_pim;	// TRUE when any state has a PIM
} nfa_list_T;

#ifdef ENABLE_LOG
स्थिर शून्य log_subexpr(regsub_T *sub);

    स्थिर शून्य
log_subsexpr(regsubs_T *subs)
{
    log_subexpr(&subs->norm);
# ifdef FEAT_SYN_HL
    यदि (rex.nfa_has_zsubexpr)
	log_subexpr(&subs->synt);
# endif
}

    स्थिर शून्य
log_subexpr(regsub_T *sub)
{
    पूर्णाङ्क j;

    प्रति (j = 0; j < sub->in_use; j++)
	यदि (REG_MULTI)
	    fprintf(log_fd,
		    "*** group %d, start: c=%d, l=%d, end: c=%d, l=%d\n",
		    j,
		    sub->list.multi[j].start_col,
		    (पूर्णाङ्क)sub->list.multi[j].start_lnum,
		    sub->list.multi[j].end_col,
		    (पूर्णाङ्क)sub->list.multi[j].end_lnum);
	अन्यथा
	{
	    वर्ण *s = (वर्ण *)sub->list.ओळ[j].start;
	    वर्ण *e = (वर्ण *)sub->list.ओळ[j].end;

	    fprintf(log_fd, "*** group %d, start: \"%s\", end: \"%s\"\n",
		    j,
		    s == रिक्त ? "NULL" : s,
		    e == रिक्त ? "NULL" : e);
	}
}

    स्थिर वर्ण *
pim_info(nfa_pim_T *pim)
{
    स्थिर वर्ण buf[30];

    यदि (pim == रिक्त || pim->result == NFA_PIM_UNUSED)
	buf[0] = NUL;
    अन्यथा
    {
	sprintf(buf, " PIM col %d", REG_MULTI ? (पूर्णाङ्क)pim->end.pos.col
		: (पूर्णाङ्क)(pim->end.ptr - rex.आदान));
    }
    प्रत्यागम buf;
}

#endif

// Used during execution: whether a match has been found.
स्थिर पूर्णाङ्क	    nfa_match;
#ifdef FEAT_RELTIME
स्थिर पूर्णाङ्क	   *nfa_timed_out;
#endif

स्थिर अन्तःस्थ शून्य copy_sub(regsub_T *to, regsub_T *from);
स्थिर पूर्णाङ्क pim_equal(nfa_pim_T *one, nfa_pim_T *two);

/*
 * Copy postponed invisible match info from "from" to "to".
 */
    स्थिर शून्य
copy_pim(nfa_pim_T *to, nfa_pim_T *from)
{
    to->result = from->result;
    to->state = from->state;
    copy_sub(&to->subs.norm, &from->subs.norm);
#ifdef FEAT_SYN_HL
    यदि (rex.nfa_has_zsubexpr)
	copy_sub(&to->subs.synt, &from->subs.synt);
#endif
    to->end = from->end;
}

    स्थिर शून्य
clear_sub(regsub_T *sub)
{
    यदि (REG_MULTI)
	// Use 0xff to set lnum to -1
	vim_memset(sub->list.multi, 0xff,
				  परिमाण(संरचना multipos) * rex.nfa_nsubexpr);
    अन्यथा
	vim_memset(sub->list.ओळ, 0,
				   परिमाण(संरचना linepos) * rex.nfa_nsubexpr);
    sub->in_use = 0;
}

/*
 * Copy the submatches from "from" to "to".
 */
    स्थिर अन्तःस्थ शून्य
copy_sub(regsub_T *to, regsub_T *from)
{
    to->in_use = from->in_use;
    यदि (from->in_use <= 0)
	प्रत्यागम;

    // Copy the match start and end positions.  addstate() calls this for
    // every state it adds, so it is very hot.  Patterns without capturing
    // groups only use subexpression zero (the whole match), so special-case
    // "in_use == 1" with a direct assignment to avoid the mch_memmove() call.
    यदि (REG_MULTI)
    {
	यदि (from->in_use == 1)
	    to->list.multi[0] = from->list.multi[0];
	अन्यथा
	    mch_memmove(&to->list.multi[0],
		    &from->list.multi[0],
		    परिमाण(संरचना multipos) * from->in_use);
	to->orig_start_col = from->orig_start_col;
    }
    अन्यथा
    {
	यदि (from->in_use == 1)
	    to->list.ओळ[0] = from->list.ओळ[0];
	अन्यथा
	    mch_memmove(&to->list.ओळ[0],
		    &from->list.ओळ[0],
		    परिमाण(संरचना linepos) * from->in_use);
    }
}

/*
 * Like copy_sub() but exclude the main match.
 */
    स्थिर शून्य
copy_sub_off(regsub_T *to, regsub_T *from)
{
    यदि (to->in_use < from->in_use)
	to->in_use = from->in_use;
    यदि (from->in_use <= 1)
	प्रत्यागम;

    // Copy the match start and end positions.
    यदि (REG_MULTI)
	mch_memmove(&to->list.multi[1],
		&from->list.multi[1],
		परिमाण(संरचना multipos) * (from->in_use - 1));
    अन्यथा
	mch_memmove(&to->list.ओळ[1],
		&from->list.ओळ[1],
		परिमाण(संरचना linepos) * (from->in_use - 1));
}

/*
 * Like copy_sub() but only do the end of the main match if \ze is present.
 */
    स्थिर शून्य
copy_ze_off(regsub_T *to, regsub_T *from)
{
    यदि (!rex.nfa_has_zend)
	प्रत्यागम;

    यदि (REG_MULTI)
    {
	यदि (from->list.multi[0].end_lnum >= 0)
	{
	    to->list.multi[0].end_lnum = from->list.multi[0].end_lnum;
	    to->list.multi[0].end_col = from->list.multi[0].end_col;
	}
    }
    अन्यथा
    {
	यदि (from->list.ओळ[0].end != रिक्त)
	    to->list.ओळ[0].end = from->list.ओळ[0].end;
    }
}

/*
 * Return TRUE if "sub1" and "sub2" have the same start positions.
 * When using back-references also check the end position.
 */
    स्थिर पूर्णाङ्क
sub_equal(regsub_T *sub1, regsub_T *sub2)
{
    पूर्णाङ्क		i;
    पूर्णाङ्क		todo;
    linenr_T	s1;
    linenr_T	s2;
    char_u	*sp1;
    char_u	*sp2;

    todo = sub1->in_use > sub2->in_use ? sub1->in_use : sub2->in_use;
    यदि (REG_MULTI)
    {
	प्रति (i = 0; i < todo; ++i)
	{
	    यदि (i < sub1->in_use)
		s1 = sub1->list.multi[i].start_lnum;
	    अन्यथा
		s1 = -1;
	    यदि (i < sub2->in_use)
		s2 = sub2->list.multi[i].start_lnum;
	    अन्यथा
		s2 = -1;
	    यदि (s1 != s2)
		प्रत्यागम FALSE;
	    यदि (s1 != -1 && sub1->list.multi[i].start_col
					     != sub2->list.multi[i].start_col)
		प्रत्यागम FALSE;

	    यदि (rex.nfa_has_backref)
	    {
		यदि (i < sub1->in_use)
		    s1 = sub1->list.multi[i].end_lnum;
		अन्यथा
		    s1 = -1;
		यदि (i < sub2->in_use)
		    s2 = sub2->list.multi[i].end_lnum;
		अन्यथा
		    s2 = -1;
		यदि (s1 != s2)
		    प्रत्यागम FALSE;
		यदि (s1 != -1 && sub1->list.multi[i].end_col
					       != sub2->list.multi[i].end_col)
		प्रत्यागम FALSE;
	    }
	}
    }
    अन्यथा
    {
	प्रति (i = 0; i < todo; ++i)
	{
	    यदि (i < sub1->in_use)
		sp1 = sub1->list.ओळ[i].start;
	    अन्यथा
		sp1 = रिक्त;
	    यदि (i < sub2->in_use)
		sp2 = sub2->list.ओळ[i].start;
	    अन्यथा
		sp2 = रिक्त;
	    यदि (sp1 != sp2)
		प्रत्यागम FALSE;
	    यदि (rex.nfa_has_backref)
	    {
		यदि (i < sub1->in_use)
		    sp1 = sub1->list.ओळ[i].end;
		अन्यथा
		    sp1 = रिक्त;
		यदि (i < sub2->in_use)
		    sp2 = sub2->list.ओळ[i].end;
		अन्यथा
		    sp2 = रिक्त;
		यदि (sp1 != sp2)
		    प्रत्यागम FALSE;
	    }
	}
    }

    प्रत्यागम TRUE;
}

#ifdef FEAT_RELTIME
/*
 * Check if we are past the time limit, if there is one.
 */
    स्थिर पूर्णाङ्क
nfa_did_time_out(शून्य)
{
    यदि (*timeout_flag)
    {
	यदि (nfa_timed_out != रिक्त)
	{
# ifdef FEAT_EVAL
	    यदि (!*nfa_timed_out)
		ch_log(रिक्त, "NFA regexp timed out");
# endif
	    *nfa_timed_out = TRUE;
	}
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}
#endif

#ifdef ENABLE_LOG
    स्थिर शून्य
open_debug_log(पूर्णाङ्क result)
{
    log_fd = fopen(NFA_REGEXP_RUN_LOG, "a");
    यदि (log_fd == रिक्त)
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

    स्थिर शून्य
report_state(वर्ण *action,
	     regsub_T *sub,
	     nfa_state_T *state,
	     पूर्णाङ्क lid,
	     nfa_pim_T *pim)
{
    पूर्णाङ्क col;

    यदि (sub->in_use <= 0)
	col = -1;
    अन्यथा यदि (REG_MULTI)
	col = sub->list.multi[0].start_col;
    अन्यथा
	col = (पूर्णाङ्क)(sub->list.ओळ[0].start - rex.ओळ);
    nfa_set_code(state->c);
    यदि (log_fd == रिक्त)
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
    स्थिर पूर्णाङ्क
has_state_with_pos(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim)	// postponed match or NULL
{
    nfa_thread_T	*thread;
    पूर्णाङ्क			i;

    प्रति (i = 0; i < l->n; ++i)
    {
	thread = &l->t[i];
	यदि (thread->state->id == state->id
		&& sub_equal(&thread->subs.norm, &subs->norm)
#ifdef FEAT_SYN_HL
		&& (!rex.nfa_has_zsubexpr
				|| sub_equal(&thread->subs.synt, &subs->synt))
#endif
		&& pim_equal(&thread->pim, pim))
	    प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

/*
 * Return TRUE if "one" and "two" are equal.  That includes when both are not
 * set.
 */
    स्थिर पूर्णाङ्क
pim_equal(nfa_pim_T *one, nfa_pim_T *two)
{
    पूर्णाङ्क one_unused = (one == रिक्त || one->result == NFA_PIM_UNUSED);
    पूर्णाङ्क two_unused = (two == रिक्त || two->result == NFA_PIM_UNUSED);

    यदि (one_unused)
	// one is unused: equal when two is also unused
	प्रत्यागम two_unused;
    यदि (two_unused)
	// one is used and two is not: not equal
	प्रत्यागम FALSE;
    // compare the state id
    यदि (one->state->id != two->state->id)
	प्रत्यागम FALSE;
    // compare the position
    यदि (REG_MULTI)
	प्रत्यागम one->end.pos.lnum == two->end.pos.lnum
	    && one->end.pos.col == two->end.pos.col;
    प्रत्यागम one->end.ptr == two->end.ptr;
}

/*
 * Return TRUE if "state" leads to a NFA_MATCH without advancing the input.
 */
    स्थिर पूर्णाङ्क
match_follows(nfa_state_T *startstate, पूर्णाङ्क depth)
{
    nfa_state_T	    *state = startstate;

    // avoid too much recursion
    यदि (depth > 10)
	प्रत्यागम FALSE;

    यावत् (state != रिक्त)
    {
	विकल्प (state->c)
	{
	    पक्ष NFA_MATCH:
	    पक्ष NFA_MCLOSE:
	    पक्ष NFA_END_INVISIBLE:
	    पक्ष NFA_END_INVISIBLE_NEG:
	    पक्ष NFA_END_PATTERN:
		प्रत्यागम TRUE;

	    पक्ष NFA_SPLIT:
		प्रत्यागम match_follows(state->out, depth + 1)
				     || match_follows(state->out1, depth + 1);

	    पक्ष NFA_START_INVISIBLE:
	    पक्ष NFA_START_INVISIBLE_FIRST:
	    पक्ष NFA_START_INVISIBLE_BEFORE:
	    पक्ष NFA_START_INVISIBLE_BEFORE_FIRST:
	    पक्ष NFA_START_INVISIBLE_NEG:
	    पक्ष NFA_START_INVISIBLE_NEG_FIRST:
	    पक्ष NFA_START_INVISIBLE_BEFORE_NEG:
	    पक्ष NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
	    पक्ष NFA_COMPOSING:
		// skip ahead to next state
		state = state->out1->out;
		अनुवर्त;

	    पक्ष NFA_ANY:
	    पक्ष NFA_ANY_COMPOSING:
	    पक्ष NFA_IDENT:
	    पक्ष NFA_SIDENT:
	    पक्ष NFA_KWORD:
	    पक्ष NFA_SKWORD:
	    पक्ष NFA_FNAME:
	    पक्ष NFA_SFNAME:
	    पक्ष NFA_PRINT:
	    पक्ष NFA_SPRINT:
	    पक्ष NFA_WHITE:
	    पक्ष NFA_NWHITE:
	    पक्ष NFA_DIGIT:
	    पक्ष NFA_NDIGIT:
	    पक्ष NFA_HEX:
	    पक्ष NFA_NHEX:
	    पक्ष NFA_OCTAL:
	    पक्ष NFA_NOCTAL:
	    पक्ष NFA_WORD:
	    पक्ष NFA_NWORD:
	    पक्ष NFA_HEAD:
	    पक्ष NFA_NHEAD:
	    पक्ष NFA_ALPHA:
	    पक्ष NFA_NALPHA:
	    पक्ष NFA_LOWER:
	    पक्ष NFA_NLOWER:
	    पक्ष NFA_UPPER:
	    पक्ष NFA_NUPPER:
	    पक्ष NFA_LOWER_IC:
	    पक्ष NFA_NLOWER_IC:
	    पक्ष NFA_UPPER_IC:
	    पक्ष NFA_NUPPER_IC:
	    पक्ष NFA_START_COLL:
	    पक्ष NFA_START_NEG_COLL:
	    पक्ष NFA_NEWL:
		// state will advance input
		प्रत्यागम FALSE;

	    स्वभाव:
		यदि (state->c > 0)
		    // state will advance input
		    प्रत्यागम FALSE;

		// Others: zero-width or possibly zero-width, might still find
		// a match at the same position, keep looking.
		भङ्ग;
	}
	state = state->out;
    }
    प्रत्यागम FALSE;
}


/*
 * Return TRUE if "state" is already in list "l".
 */
    स्थिर पूर्णाङ्क
state_in_list(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs)	// pointers to subexpressions
{
    यदि (state->lastlist[nfa_ll_index] == l->id)
    {
	यदि (!rex.nfa_has_backref || has_state_with_pos(l, state, subs, रिक्त))
	    प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

// Offset used for "off" by addstate_here().
#define ADDSTATE_HERE_OFFSET 10

/*
 * Add "state" and possibly what follows to state list ".".
 * Returns "subs_arg", possibly copied into temp_subs.
 * Returns NULL when recursiveness is too deep or timed out.
 */
    स्थिर regsubs_T *
addstate(
    nfa_list_T		*l,	    // runtime state list
    nfa_state_T		*state,	    // state to update
    regsubs_T		*subs_arg,  // pointers to subexpressions
    nfa_pim_T		*pim,	    // postponed look-behind match
    पूर्णाङ्क			off_arg,    // byte offset, when -1 go to next line
    पूर्णाङ्क			depth)	    // recursion depth
{
    पूर्णाङ्क			subidx;
    पूर्णाङ्क			off = off_arg;
    पूर्णाङ्क			add_here = FALSE;
    पूर्णाङ्क			listindex = 0;
    पूर्णाङ्क			k;
    पूर्णाङ्क			found = FALSE;
    nfa_thread_T	*thread;
    संरचना multipos	save_multipos;
    पूर्णाङ्क			save_in_use;
    char_u		*save_ptr;
    पूर्णाङ्क			i;
    regsub_T		*sub;
    regsubs_T		*subs = subs_arg;
    स्थिर regsubs_T	temp_subs;
#ifdef ENABLE_LOG
    पूर्णाङ्क			did_print = FALSE;
#endif

#ifdef FEAT_RELTIME
    यदि (nfa_did_time_out())
	प्रत्यागम रिक्त;
#endif

    // This function is called recursively.  When the depth is too much we run
    // out of stack and crash, limit recursiveness here.
    यदि (depth >= 5000 || subs == रिक्त)
	प्रत्यागम रिक्त;

    यदि (off_arg <= -ADDSTATE_HERE_OFFSET)
    {
	add_here = TRUE;
	off = 0;
	listindex = -(off_arg + ADDSTATE_HERE_OFFSET);
    }

    विकल्प (state->c)
    {
	पक्ष NFA_NCLOSE:
	पक्ष NFA_MCLOSE:
	पक्ष NFA_MCLOSE1:
	पक्ष NFA_MCLOSE2:
	पक्ष NFA_MCLOSE3:
	पक्ष NFA_MCLOSE4:
	पक्ष NFA_MCLOSE5:
	पक्ष NFA_MCLOSE6:
	पक्ष NFA_MCLOSE7:
	पक्ष NFA_MCLOSE8:
	पक्ष NFA_MCLOSE9:
#ifdef FEAT_SYN_HL
	पक्ष NFA_ZCLOSE:
	पक्ष NFA_ZCLOSE1:
	पक्ष NFA_ZCLOSE2:
	पक्ष NFA_ZCLOSE3:
	पक्ष NFA_ZCLOSE4:
	पक्ष NFA_ZCLOSE5:
	पक्ष NFA_ZCLOSE6:
	पक्ष NFA_ZCLOSE7:
	पक्ष NFA_ZCLOSE8:
	पक्ष NFA_ZCLOSE9:
#endif
	पक्ष NFA_MOPEN:
	पक्ष NFA_ZEND:
	पक्ष NFA_SPLIT:
	पक्ष NFA_EMPTY:
	    // These nodes are not added themselves but their "out" and/or
	    // "out1" may be added below.
	    भङ्ग;

	पक्ष NFA_BOL:
	पक्ष NFA_BOF:
	    // "^" won't match past end-of-line, don't bother trying.
	    // Except when at the end of the line, or when we are going to the
	    // next line for a look-behind match.
	    यदि (rex.आदान > rex.ओळ
		    && *rex.आदान != NUL
		    && (nfa_endp == रिक्त
			|| !REG_MULTI
			|| rex.lnum == nfa_endp->se_u.pos.lnum))
		गच्छ skip_add;
	    // FALLTHROUGH

	पक्ष NFA_MOPEN1:
	पक्ष NFA_MOPEN2:
	पक्ष NFA_MOPEN3:
	पक्ष NFA_MOPEN4:
	पक्ष NFA_MOPEN5:
	पक्ष NFA_MOPEN6:
	पक्ष NFA_MOPEN7:
	पक्ष NFA_MOPEN8:
	पक्ष NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	पक्ष NFA_ZOPEN:
	पक्ष NFA_ZOPEN1:
	पक्ष NFA_ZOPEN2:
	पक्ष NFA_ZOPEN3:
	पक्ष NFA_ZOPEN4:
	पक्ष NFA_ZOPEN5:
	पक्ष NFA_ZOPEN6:
	पक्ष NFA_ZOPEN7:
	पक्ष NFA_ZOPEN8:
	पक्ष NFA_ZOPEN9:
#endif
	पक्ष NFA_NOPEN:
	पक्ष NFA_ZSTART:
	    // These nodes need to be added so that we can bail out when it
	    // was added to this list before at the same position to avoid an
	    // endless loop for "\(\)*"

	स्वभाव:
	    यदि (state->lastlist[nfa_ll_index] == l->id && state->c != NFA_SKIP)
	    {
		// This state is already in the list, don't add it again,
		// unless it is an MOPEN that is used for a backreference or
		// when there is a PIM. For NFA_MATCH check the position,
		// lower position is preferred.
		यदि (!rex.nfa_has_backref && pim == रिक्त && !l->has_pim
						     && state->c != NFA_MATCH)
		{
		    // When called from addstate_here() do insert before
		    // existing states.
		    यदि (add_here)
		    {
			प्रति (k = 0; k < l->n && k < listindex; ++k)
			    यदि (l->t[k].state->id == state->id)
			    {
				found = TRUE;
				भङ्ग;
			    }
		    }
		    यदि (!add_here || found)
		    {
skip_add:
#ifdef ENABLE_LOG
			nfa_set_code(state->c);
			fprintf(log_fd, "> Not adding state %d to list %d. char %d: %s pim: %s has_pim: %d found: %d\n",
			    abs(state->id), l->id, state->c, code,
			    pim == रिक्त ? "NULL" : "yes", l->has_pim, found);
#endif
			प्रत्यागम subs;
		    }
		}

		// Do not add the state again when it exists with the same
		// positions.
		यदि (has_state_with_pos(l, state, subs, pim))
		    गच्छ skip_add;
	    }

	    // When there are backreferences or PIMs the number of states may
	    // be (a lot) bigger than anticipated.
	    यदि (l->n == l->len)
	    {
		पूर्णाङ्क		newlen = l->len * 3 / 2 + 50;
		size_t		newsize = newlen * परिमाण(nfa_thread_T);
		nfa_thread_T	*newt;

		यदि ((long)(newsize >> 10) >= p_mmp)
		{
		    emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		    प्रत्यागम रिक्त;
		}
		यदि (subs != &temp_subs)
		{
		    // "subs" may point into the current array, need to make a
		    // copy before it becomes invalid.
		    copy_sub(&temp_subs.norm, &subs->norm);
#ifdef FEAT_SYN_HL
		    यदि (rex.nfa_has_zsubexpr)
			copy_sub(&temp_subs.synt, &subs->synt);
#endif
		    subs = &temp_subs;
		}

		newt = vim_realloc(l->t, newsize);
		यदि (newt == रिक्त)
		{
		    // out of memory
		    प्रत्यागम रिक्त;
		}
		l->t = newt;
		l->len = newlen;
	    }

	    // add the state to the list
	    state->lastlist[nfa_ll_index] = l->id;
	    thread = &l->t[l->n++];
	    thread->state = state;
	    यदि (pim == रिक्त)
		thread->pim.result = NFA_PIM_UNUSED;
	    अन्यथा
	    {
		copy_pim(&thread->pim, pim);
		l->has_pim = TRUE;
	    }
	    copy_sub(&thread->subs.norm, &subs->norm);
#ifdef FEAT_SYN_HL
	    यदि (rex.nfa_has_zsubexpr)
		copy_sub(&thread->subs.synt, &subs->synt);
#endif
#ifdef ENABLE_LOG
	    report_state("Adding", &thread->subs.norm, state, l->id, pim);
	    did_print = TRUE;
#endif
    }

#ifdef ENABLE_LOG
    यदि (!did_print)
	report_state("Processing", &subs->norm, state, l->id, pim);
#endif
    विकल्प (state->c)
    {
	पक्ष NFA_MATCH:
	    भङ्ग;

	पक्ष NFA_SPLIT:
	    // order matters here
	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    subs = addstate(l, state->out1, subs, pim, off_arg, depth + 1);
	    भङ्ग;

	पक्ष NFA_EMPTY:
	पक्ष NFA_NOPEN:
	पक्ष NFA_NCLOSE:
	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    भङ्ग;

	पक्ष NFA_MOPEN:
	पक्ष NFA_MOPEN1:
	पक्ष NFA_MOPEN2:
	पक्ष NFA_MOPEN3:
	पक्ष NFA_MOPEN4:
	पक्ष NFA_MOPEN5:
	पक्ष NFA_MOPEN6:
	पक्ष NFA_MOPEN7:
	पक्ष NFA_MOPEN8:
	पक्ष NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	पक्ष NFA_ZOPEN:
	पक्ष NFA_ZOPEN1:
	पक्ष NFA_ZOPEN2:
	पक्ष NFA_ZOPEN3:
	पक्ष NFA_ZOPEN4:
	पक्ष NFA_ZOPEN5:
	पक्ष NFA_ZOPEN6:
	पक्ष NFA_ZOPEN7:
	पक्ष NFA_ZOPEN8:
	पक्ष NFA_ZOPEN9:
#endif
	पक्ष NFA_ZSTART:
	    यदि (state->c == NFA_ZSTART)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#ifdef FEAT_SYN_HL
	    अन्यथा यदि (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
	    {
		subidx = state->c - NFA_ZOPEN;
		sub = &subs->synt;
	    }
#endif
	    अन्यथा
	    {
		subidx = state->c - NFA_MOPEN;
		sub = &subs->norm;
	    }

	    // avoid compiler warnings
	    save_ptr = रिक्त;
	    CLEAR_FIELD(save_multipos);

	    // Set the position (with "off" added) in the subexpression.  Save
	    // and restore it when it was in use.  Otherwise fill any gap.
	    यदि (REG_MULTI)
	    {
		यदि (subidx < sub->in_use)
		{
		    save_multipos = sub->list.multi[subidx];
		    save_in_use = -1;
		}
		अन्यथा
		{
		    save_in_use = sub->in_use;
		    प्रति (i = sub->in_use; i < subidx; ++i)
		    {
			sub->list.multi[i].start_lnum = -1;
			sub->list.multi[i].end_lnum = -1;
		    }
		    sub->in_use = subidx + 1;
		}
		यदि (off == -1)
		{
		    sub->list.multi[subidx].start_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].start_col = 0;
		}
		अन्यथा
		{
		    sub->list.multi[subidx].start_lnum = rex.lnum;
		    sub->list.multi[subidx].start_col =
					 (colnr_T)(rex.आदान - rex.ओळ + off);
		}
		sub->list.multi[subidx].end_lnum = -1;
	    }
	    अन्यथा
	    {
		यदि (subidx < sub->in_use)
		{
		    save_ptr = sub->list.ओळ[subidx].start;
		    save_in_use = -1;
		}
		अन्यथा
		{
		    save_in_use = sub->in_use;
		    प्रति (i = sub->in_use; i < subidx; ++i)
		    {
			sub->list.ओळ[i].start = रिक्त;
			sub->list.ओळ[i].end = रिक्त;
		    }
		    sub->in_use = subidx + 1;
		}
		sub->list.ओळ[subidx].start = rex.आदान + off;
	    }

	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    यदि (subs == रिक्त)
		भङ्ग;
	    // "subs" may have changed, need to set "sub" again
#ifdef FEAT_SYN_HL
	    यदि (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
		sub = &subs->synt;
	    अन्यथा
#endif
		sub = &subs->norm;

	    यदि (save_in_use == -1)
	    {
		यदि (REG_MULTI)
		    sub->list.multi[subidx] = save_multipos;
		अन्यथा
		    sub->list.ओळ[subidx].start = save_ptr;
	    }
	    अन्यथा
		sub->in_use = save_in_use;
	    भङ्ग;

	पक्ष NFA_MCLOSE:
	    यदि (rex.nfa_has_zend && (REG_MULTI
			? subs->norm.list.multi[0].end_lnum >= 0
			: subs->norm.list.ओळ[0].end != रिक्त))
	    {
		// Do not overwrite the position set by \ze.
		subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
		भङ्ग;
	    }
	    // FALLTHROUGH
	पक्ष NFA_MCLOSE1:
	पक्ष NFA_MCLOSE2:
	पक्ष NFA_MCLOSE3:
	पक्ष NFA_MCLOSE4:
	पक्ष NFA_MCLOSE5:
	पक्ष NFA_MCLOSE6:
	पक्ष NFA_MCLOSE7:
	पक्ष NFA_MCLOSE8:
	पक्ष NFA_MCLOSE9:
#ifdef FEAT_SYN_HL
	पक्ष NFA_ZCLOSE:
	पक्ष NFA_ZCLOSE1:
	पक्ष NFA_ZCLOSE2:
	पक्ष NFA_ZCLOSE3:
	पक्ष NFA_ZCLOSE4:
	पक्ष NFA_ZCLOSE5:
	पक्ष NFA_ZCLOSE6:
	पक्ष NFA_ZCLOSE7:
	पक्ष NFA_ZCLOSE8:
	पक्ष NFA_ZCLOSE9:
#endif
	पक्ष NFA_ZEND:
	    यदि (state->c == NFA_ZEND)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#ifdef FEAT_SYN_HL
	    अन्यथा यदि (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
	    {
		subidx = state->c - NFA_ZCLOSE;
		sub = &subs->synt;
	    }
#endif
	    अन्यथा
	    {
		subidx = state->c - NFA_MCLOSE;
		sub = &subs->norm;
	    }

	    // We don't fill in gaps here, there must have been an MOPEN that
	    // has done that.
	    save_in_use = sub->in_use;
	    यदि (sub->in_use <= subidx)
		sub->in_use = subidx + 1;
	    यदि (REG_MULTI)
	    {
		save_multipos = sub->list.multi[subidx];
		यदि (off == -1)
		{
		    sub->list.multi[subidx].end_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].end_col = 0;
		}
		अन्यथा
		{
		    sub->list.multi[subidx].end_lnum = rex.lnum;
		    sub->list.multi[subidx].end_col =
					  (colnr_T)(rex.आदान - rex.ओळ + off);
		}
		// avoid compiler warnings
		save_ptr = रिक्त;
	    }
	    अन्यथा
	    {
		save_ptr = sub->list.ओळ[subidx].end;
		sub->list.ओळ[subidx].end = rex.आदान + off;
		// avoid compiler warnings
		CLEAR_FIELD(save_multipos);
	    }

	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    यदि (subs == रिक्त)
		भङ्ग;
	    // "subs" may have changed, need to set "sub" again
#ifdef FEAT_SYN_HL
	    यदि (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
		sub = &subs->synt;
	    अन्यथा
#endif
		sub = &subs->norm;

	    यदि (REG_MULTI)
		sub->list.multi[subidx] = save_multipos;
	    अन्यथा
		sub->list.ओळ[subidx].end = save_ptr;
	    sub->in_use = save_in_use;
	    भङ्ग;
    }
    प्रत्यागम subs;
}

/*
 * Like addstate(), but the new state(s) are put at position "*ip".
 * Used for zero-width matches, next state to use is the added one.
 * This makes sure the order of states to be tried does not change, which
 * matters for alternatives.
 */
    स्थिर regsubs_T *
addstate_here(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim,   // postponed look-behind match
    पूर्णाङ्क			*ip)
{
    पूर्णाङ्क tlen = l->n;
    पूर्णाङ्क मोजणी;
    पूर्णाङ्क listidx = *ip;
    regsubs_T *r;

    // First add the state(s) at the end, so that we know how many there are.
    // Pass the listidx as offset (avoids adding another argument to
    // addstate()).
    r = addstate(l, state, subs, pim, -listidx - ADDSTATE_HERE_OFFSET, 0);
    यदि (r == रिक्त)
	प्रत्यागम रिक्त;

    // when "*ip" was at the end of the list, nothing to do
    यदि (listidx + 1 == tlen)
	प्रत्यागम r;

    // re-order to put the new state at the current position
    मोजणी = l->n - tlen;
    यदि (मोजणी == 0)
	प्रत्यागम r; // no state got added
    यदि (मोजणी == 1)
    {
	// overwrite the current state
	l->t[listidx] = l->t[l->n - 1];
    }
    अन्यथा यदि (मोजणी > 1)
    {
	यदि (l->n + मोजणी - 1 >= l->len)
	{
	    // not enough space to move the new states, reallocate the list
	    // and move the states to the right position
	    पूर्णाङ्क		    newlen = l->len * 3 / 2 + 50;
	    size_t	    newsize = newlen * परिमाण(nfa_thread_T);
	    nfa_thread_T    *newl;

	    यदि ((long)(newsize >> 10) >= p_mmp)
	    {
		emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		प्रत्यागम रिक्त;
	    }
	    newl = alloc(newsize);
	    यदि (newl == रिक्त)
		प्रत्यागम रिक्त;
	    l->len = newlen;
	    mch_memmove(&(newl[0]),
		    &(l->t[0]),
		    परिमाण(nfa_thread_T) * listidx);
	    mch_memmove(&(newl[listidx]),
		    &(l->t[l->n - मोजणी]),
		    परिमाण(nfa_thread_T) * मोजणी);
	    mch_memmove(&(newl[listidx + मोजणी]),
		    &(l->t[listidx + 1]),
		    परिमाण(nfa_thread_T) * (l->n - मोजणी - listidx - 1));
	    vim_free(l->t);
	    l->t = newl;
	}
	अन्यथा
	{
	    // make space for new states, then move them from the
	    // end to the current position
	    mch_memmove(&(l->t[listidx + मोजणी]),
		    &(l->t[listidx + 1]),
		    परिमाण(nfa_thread_T) * (l->n - listidx - 1));
	    mch_memmove(&(l->t[listidx]),
		    &(l->t[l->n - 1]),
		    परिमाण(nfa_thread_T) * मोजणी);
	}
    }
    --l->n;
    *ip = listidx - 1;

    प्रत्यागम r;
}

/*
 * Check character class "class" against current character c.
 */
    स्थिर पूर्णाङ्क
check_char_class(पूर्णाङ्क class, पूर्णाङ्क c)
{
    विकल्प (class)
    {
	पक्ष NFA_CLASS_ALNUM:
	    यदि (c >= 1 && c < 128 && isalnum(c))
		प्रत्यागम OK;
	    भङ्ग;
	पक्ष NFA_CLASS_ALPHA:
	    यदि (c >= 1 && c < 128 && isalpha(c))
		प्रत्यागम OK;
	    भङ्ग;
	पक्ष NFA_CLASS_BLANK:
	    यदि (c == ' ' || c == '\t')
		प्रत्यागम OK;
	    भङ्ग;
	पक्ष NFA_CLASS_CNTRL:
	    यदि (c >= 1 && c <= 127 && iscntrl(c))
		प्रत्यागम OK;
	    भङ्ग;
	पक्ष NFA_CLASS_DIGIT:
	    यदि (VIM_ISDIGIT(c))
		प्रत्यागम OK;
	    भङ्ग;
	पक्ष NFA_CLASS_GRAPH:
	    यदि (c >= 1 && c <= 127 && isgraph(c))
		प्रत्यागम OK;
	    भङ्ग;
	पक्ष NFA_CLASS_LOWER:
	    यदि (MB_ISLOWER(c) && c != 170 && c != 186)
		प्रत्यागम OK;
	    भङ्ग;
	पक्ष NFA_CLASS_PRINT:
	    यदि (vim_isprintc(c))
		प्रत्यागम OK;
	    भङ्ग;
	पक्ष NFA_CLASS_PUNCT:
	    यदि (c >= 1 && c < 128 && ispunct(c))
		प्रत्यागम OK;
	    भङ्ग;
	पक्ष NFA_CLASS_SPACE:
	    यदि ((c >= 9 && c <= 13) || (c == ' '))
		प्रत्यागम OK;
	    भङ्ग;
	पक्ष NFA_CLASS_UPPER:
	    यदि (MB_ISUPPER(c))
		प्रत्यागम OK;
	    भङ्ग;
	पक्ष NFA_CLASS_XDIGIT:
	    यदि (vim_isxdigit(c))
		प्रत्यागम OK;
	    भङ्ग;
	पक्ष NFA_CLASS_TAB:
	    यदि (c == '\t')
		प्रत्यागम OK;
	    भङ्ग;
	पक्ष NFA_CLASS_RETURN:
	    यदि (c == '\r')
		प्रत्यागम OK;
	    भङ्ग;
	पक्ष NFA_CLASS_BACKSPACE:
	    यदि (c == '\b')
		प्रत्यागम OK;
	    भङ्ग;
	पक्ष NFA_CLASS_ESCAPE:
	    यदि (c == '\033')
		प्रत्यागम OK;
	    भङ्ग;
	पक्ष NFA_CLASS_IDENT:
	    यदि (vim_isIDc(c))
		प्रत्यागम OK;
	    भङ्ग;
	पक्ष NFA_CLASS_KEYWORD:
	    यदि (reg_iswordc(c))
		प्रत्यागम OK;
	    भङ्ग;
	पक्ष NFA_CLASS_FNAME:
	    यदि (vim_isfilec(c))
		प्रत्यागम OK;
	    भङ्ग;

	स्वभाव:
	    // should not be here :P
	    siemsg(_(e_nfa_regexp_invalid_character_class_nr), class);
	    प्रत्यागम FAIL;
    }
    प्रत्यागम FAIL;
}

/*
 * Check for a match with subexpression "subidx".
 * Return TRUE if it matches.
 */
    स्थिर पूर्णाङ्क
match_backref(
    regsub_T	*sub,	    // pointers to subexpressions
    पूर्णाङ्क		subidx,
    पूर्णाङ्क		*bytelen)   // out: length of match in bytes
{
    पूर्णाङ्क		len;

    यदि (sub->in_use <= subidx)
    {
retempty:
	// backref was not set, match an empty string
	*bytelen = 0;
	प्रत्यागम TRUE;
    }

    यदि (REG_MULTI)
    {
	यदि (sub->list.multi[subidx].start_lnum < 0
				       || sub->list.multi[subidx].end_lnum < 0)
	    गच्छ retempty;
	यदि (sub->list.multi[subidx].start_lnum == rex.lnum
			       && sub->list.multi[subidx].end_lnum == rex.lnum)
	{
	    len = sub->list.multi[subidx].end_col
					  - sub->list.multi[subidx].start_col;
	    यदि (cstrncmp(rex.ओळ + sub->list.multi[subidx].start_col,
							 rex.आदान, &len) == 0)
	    {
		*bytelen = len;
		प्रत्यागम TRUE;
	    }
	}
	अन्यथा
	{
	    यदि (match_with_backref(
			sub->list.multi[subidx].start_lnum,
			sub->list.multi[subidx].start_col,
			sub->list.multi[subidx].end_lnum,
			sub->list.multi[subidx].end_col,
			bytelen) == RA_MATCH)
		प्रत्यागम TRUE;
	}
    }
    अन्यथा
    {
	यदि (sub->list.ओळ[subidx].start == रिक्त
					|| sub->list.ओळ[subidx].end == रिक्त)
	    गच्छ retempty;
	len = (पूर्णाङ्क)(sub->list.ओळ[subidx].end - sub->list.ओळ[subidx].start);
	यदि (cstrncmp(sub->list.ओळ[subidx].start, rex.आदान, &len) == 0)
	{
	    *bytelen = len;
	    प्रत्यागम TRUE;
	}
    }
    प्रत्यागम FALSE;
}

#ifdef FEAT_SYN_HL

/*
 * Check for a match with \z subexpression "subidx".
 * Return TRUE if it matches.
 */
    स्थिर पूर्णाङ्क
match_zref(
    पूर्णाङ्क		subidx,
    पूर्णाङ्क		*bytelen)   // out: length of match in bytes
{
    पूर्णाङ्क		len;

    cleanup_zsubexpr();
    यदि (re_extmatch_in == रिक्त || re_extmatch_in->matches[subidx] == रिक्त)
    {
	// backref was not set, match an empty string
	*bytelen = 0;
	प्रत्यागम TRUE;
    }

    len = (पूर्णाङ्क)STRLEN(re_extmatch_in->matches[subidx]);
    यदि (cstrncmp(re_extmatch_in->matches[subidx], rex.आदान, &len) == 0)
    {
	*bytelen = len;
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}
#endif

/*
 * Save list IDs for all NFA states of "prog" into "list".
 * Also reset the IDs to zero.
 * Only used for the recursive value lastlist[1].
 */
    स्थिर शून्य
nfa_save_listids(nfa_regprog_T *prog, पूर्णाङ्क *list)
{
    पूर्णाङ्क		    i;
    nfa_state_T	    *p;

    // Order in the list is reverse, it's a bit faster that way.
    p = &prog->state[0];
    प्रति (i = prog->nstate; --i >= 0; )
    {
	list[i] = p->lastlist[1];
	p->lastlist[1] = 0;
	++p;
    }
}

/*
 * Restore list IDs from "list" to all NFA states.
 */
    स्थिर शून्य
nfa_restore_listids(nfa_regprog_T *prog, पूर्णाङ्क *list)
{
    पूर्णाङ्क		    i;
    nfa_state_T	    *p;

    p = &prog->state[0];
    प्रति (i = prog->nstate; --i >= 0; )
    {
	p->lastlist[1] = list[i];
	++p;
    }
}

    स्थिर पूर्णाङ्क
nfa_re_अंक_cmp(long_u val, पूर्णाङ्क op, long_u pos)
{
    यदि (op == 1) प्रत्यागम pos > val;
    यदि (op == 2) प्रत्यागम pos < val;
    प्रत्यागम val == pos;
}

स्थिर पूर्णाङ्क nfa_regmatch(nfa_regprog_T *prog, nfa_state_T *start, regsubs_T *submatch, regsubs_T *m);

/*
 * Recursively call nfa_regmatch()
 * "pim" is NULL or contains info about a Postponed Invisible Match (start
 * position).
 */
    स्थिर पूर्णाङ्क
recursive_regmatch(
    nfa_state_T	    *state,
    nfa_pim_T	    *pim,
    nfa_regprog_T   *prog,
    regsubs_T	    *submatch,
    regsubs_T	    *m,
    पूर्णाङ्क		    **listids,
    पूर्णाङ्क		    *listids_len)
{
    पूर्णाङ्क		save_reginput_col = (पूर्णाङ्क)(rex.आदान - rex.ओळ);
    पूर्णाङ्क		save_reglnum = rex.lnum;
    पूर्णाङ्क		save_nfa_match = nfa_match;
    पूर्णाङ्क		save_nfa_listid = rex.nfa_listid;
    save_se_T   *save_nfa_endp = nfa_endp;
    save_se_T   endpos;
    save_se_T   *endposp = रिक्त;
    पूर्णाङ्क		result;
    पूर्णाङ्क		need_restore = FALSE;

    यदि (pim != रिक्त)
    {
	// start at the position where the postponed match was
	यदि (REG_MULTI)
	    rex.आदान = rex.ओळ + pim->end.pos.col;
	अन्यथा
	    rex.आदान = pim->end.ptr;
    }

    यदि (state->c == NFA_START_INVISIBLE_BEFORE
	    || state->c == NFA_START_INVISIBLE_BEFORE_FIRST
	    || state->c == NFA_START_INVISIBLE_BEFORE_NEG
	    || state->c == NFA_START_INVISIBLE_BEFORE_NEG_FIRST)
    {
	// The recursive match must end at the current position. When "pim" is
	// not NULL it specifies the current position.
	endposp = &endpos;
	यदि (REG_MULTI)
	{
	    यदि (pim == रिक्त)
	    {
		endpos.se_u.pos.col = (पूर्णाङ्क)(rex.आदान - rex.ओळ);
		endpos.se_u.pos.lnum = rex.lnum;
	    }
	    अन्यथा
		endpos.se_u.pos = pim->end.pos;
	}
	अन्यथा
	{
	    यदि (pim == रिक्त)
		endpos.se_u.ptr = rex.आदान;
	    अन्यथा
		endpos.se_u.ptr = pim->end.ptr;
	}

	// Go back the specified number of bytes, or as far as the
	// start of the previous line, to try matching "\@<=" or
	// not matching "\@<!". This is very inefficient, limit the number of
	// bytes if possible.
	यदि (state->val <= 0)
	{
	    यदि (REG_MULTI)
	    {
		rex.ओळ = reg_getline(--rex.lnum);
		यदि (rex.ओळ == रिक्त)
		    // can't go before the first line
		    rex.ओळ = reg_getline(++rex.lnum);
	    }
	    rex.आदान = rex.ओळ;
	}
	अन्यथा
	{
	    यदि (REG_MULTI && (पूर्णाङ्क)(rex.आदान - rex.ओळ) < state->val)
	    {
		// Not enough bytes in this line, go to end of
		// previous line.
		rex.ओळ = reg_getline(--rex.lnum);
		यदि (rex.ओळ == रिक्त)
		{
		    // can't go before the first line
		    rex.ओळ = reg_getline(++rex.lnum);
		    rex.आदान = rex.ओळ;
		}
		अन्यथा
		    rex.आदान = rex.ओळ + reg_getline_len(rex.lnum);
	    }
	    यदि ((पूर्णाङ्क)(rex.आदान - rex.ओळ) >= state->val)
	    {
		rex.आदान -= state->val;
		यदि (has_mbyte)
		    rex.आदान -= mb_head_off(rex.ओळ, rex.आदान);
	    }
	    अन्यथा
		rex.आदान = rex.ओळ;
	}
    }

#ifdef ENABLE_LOG
    यदि (log_fd != stderr)
	fclose(log_fd);
    log_fd = रिक्त;
#endif
    // Have to clear the lastlist field of the NFA nodes, so that
    // nfa_regmatch() and addstate() can run properly after recursion.
    यदि (nfa_ll_index == 1)
    {
	// Already calling nfa_regmatch() recursively.  Save the lastlist[1]
	// values and clear them.
	यदि (*listids == रिक्त || *listids_len < prog->nstate)
	{
	    vim_free(*listids);
	    *listids = ALLOC_MULT(पूर्णाङ्क, prog->nstate);
	    यदि (*listids == रिक्त)
	    {
		emsg(_(e_nfa_regexp_could_not_allocate_memory_for_branch_traversal));
		प्रत्यागम 0;
	    }
	    *listids_len = prog->nstate;
	}
	nfa_save_listids(prog, *listids);
	need_restore = TRUE;
	// any value of rex.nfa_listid will do
    }
    अन्यथा
    {
	// First recursive nfa_regmatch() call, switch to the second lastlist
	// entry.  Make sure rex.nfa_listid is different from a previous
	// recursive call, because some states may still have this ID.
	++nfa_ll_index;
	यदि (rex.nfa_listid <= rex.nfa_alt_listid)
	    rex.nfa_listid = rex.nfa_alt_listid;
    }

    // Call nfa_regmatch() to check if the current concat matches at this
    // position. The concat ends with the node NFA_END_INVISIBLE
    nfa_endp = endposp;
    result = nfa_regmatch(prog, state->out, submatch, m);

    यदि (need_restore)
	nfa_restore_listids(prog, *listids);
    अन्यथा
    {
	--nfa_ll_index;
	rex.nfa_alt_listid = rex.nfa_listid;
    }

    // restore position in input text
    rex.lnum = save_reglnum;
    यदि (REG_MULTI)
	rex.ओळ = reg_getline(rex.lnum);
    rex.आदान = rex.ओळ + save_reginput_col;
    यदि (result != NFA_TOO_EXPENSIVE)
    {
	nfa_match = save_nfa_match;
	rex.nfa_listid = save_nfa_listid;
    }
    nfa_endp = save_nfa_endp;

#ifdef ENABLE_LOG
    open_debug_log(result);
#endif

    प्रत्यागम result;
}

/*
 * Estimate the chance of a match with "state" failing.
 * empty match: 0
 * NFA_ANY: 1
 * specific character: 99
 */
    स्थिर पूर्णाङ्क
failure_chance(nfa_state_T *state, पूर्णाङ्क depth)
{
    पूर्णाङ्क c = state->c;
    पूर्णाङ्क l, r;

    // detect looping
    यदि (depth > 4)
	प्रत्यागम 1;

    विकल्प (c)
    {
	पक्ष NFA_SPLIT:
	    यदि (state->out->c == NFA_SPLIT || state->out1->c == NFA_SPLIT)
		// avoid recursive stuff
		प्रत्यागम 1;
	    // two alternatives, use the lowest failure chance
	    l = failure_chance(state->out, depth + 1);
	    r = failure_chance(state->out1, depth + 1);
	    प्रत्यागम l < r ? l : r;

	पक्ष NFA_ANY:
	    // matches anything, unlikely to fail
	    प्रत्यागम 1;

	पक्ष NFA_MATCH:
	पक्ष NFA_MCLOSE:
	पक्ष NFA_ANY_COMPOSING:
	    // empty match works always
	    प्रत्यागम 0;

	पक्ष NFA_START_INVISIBLE:
	पक्ष NFA_START_INVISIBLE_FIRST:
	पक्ष NFA_START_INVISIBLE_NEG:
	पक्ष NFA_START_INVISIBLE_NEG_FIRST:
	पक्ष NFA_START_INVISIBLE_BEFORE:
	पक्ष NFA_START_INVISIBLE_BEFORE_FIRST:
	पक्ष NFA_START_INVISIBLE_BEFORE_NEG:
	पक्ष NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
	पक्ष NFA_START_PATTERN:
	    // recursive regmatch is expensive, use low failure chance
	    प्रत्यागम 5;

	पक्ष NFA_BOL:
	पक्ष NFA_EOL:
	पक्ष NFA_BOF:
	पक्ष NFA_EOF:
	पक्ष NFA_NEWL:
	    प्रत्यागम 99;

	पक्ष NFA_BOW:
	पक्ष NFA_EOW:
	    प्रत्यागम 90;

	पक्ष NFA_MOPEN:
	पक्ष NFA_MOPEN1:
	पक्ष NFA_MOPEN2:
	पक्ष NFA_MOPEN3:
	पक्ष NFA_MOPEN4:
	पक्ष NFA_MOPEN5:
	पक्ष NFA_MOPEN6:
	पक्ष NFA_MOPEN7:
	पक्ष NFA_MOPEN8:
	पक्ष NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	पक्ष NFA_ZOPEN:
	पक्ष NFA_ZOPEN1:
	पक्ष NFA_ZOPEN2:
	पक्ष NFA_ZOPEN3:
	पक्ष NFA_ZOPEN4:
	पक्ष NFA_ZOPEN5:
	पक्ष NFA_ZOPEN6:
	पक्ष NFA_ZOPEN7:
	पक्ष NFA_ZOPEN8:
	पक्ष NFA_ZOPEN9:
	पक्ष NFA_ZCLOSE:
	पक्ष NFA_ZCLOSE1:
	पक्ष NFA_ZCLOSE2:
	पक्ष NFA_ZCLOSE3:
	पक्ष NFA_ZCLOSE4:
	पक्ष NFA_ZCLOSE5:
	पक्ष NFA_ZCLOSE6:
	पक्ष NFA_ZCLOSE7:
	पक्ष NFA_ZCLOSE8:
	पक्ष NFA_ZCLOSE9:
#endif
	पक्ष NFA_NOPEN:
	पक्ष NFA_MCLOSE1:
	पक्ष NFA_MCLOSE2:
	पक्ष NFA_MCLOSE3:
	पक्ष NFA_MCLOSE4:
	पक्ष NFA_MCLOSE5:
	पक्ष NFA_MCLOSE6:
	पक्ष NFA_MCLOSE7:
	पक्ष NFA_MCLOSE8:
	पक्ष NFA_MCLOSE9:
	पक्ष NFA_NCLOSE:
	    प्रत्यागम failure_chance(state->out, depth + 1);

	पक्ष NFA_BACKREF1:
	पक्ष NFA_BACKREF2:
	पक्ष NFA_BACKREF3:
	पक्ष NFA_BACKREF4:
	पक्ष NFA_BACKREF5:
	पक्ष NFA_BACKREF6:
	पक्ष NFA_BACKREF7:
	पक्ष NFA_BACKREF8:
	पक्ष NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	पक्ष NFA_ZREF1:
	पक्ष NFA_ZREF2:
	पक्ष NFA_ZREF3:
	पक्ष NFA_ZREF4:
	पक्ष NFA_ZREF5:
	पक्ष NFA_ZREF6:
	पक्ष NFA_ZREF7:
	पक्ष NFA_ZREF8:
	पक्ष NFA_ZREF9:
#endif
	    // backreferences don't match in many places
	    प्रत्यागम 94;

	पक्ष NFA_LNUM_GT:
	पक्ष NFA_LNUM_LT:
	पक्ष NFA_COL_GT:
	पक्ष NFA_COL_LT:
	पक्ष NFA_VCOL_GT:
	पक्ष NFA_VCOL_LT:
	पक्ष NFA_MARK_GT:
	पक्ष NFA_MARK_LT:
	पक्ष NFA_VISUAL:
	    // before/after positions don't match very often
	    प्रत्यागम 85;

	पक्ष NFA_LNUM:
	    प्रत्यागम 90;

	पक्ष NFA_CURSOR:
	पक्ष NFA_COL:
	पक्ष NFA_VCOL:
	पक्ष NFA_MARK:
	    // specific positions rarely match
	    प्रत्यागम 98;

	पक्ष NFA_COMPOSING:
	    प्रत्यागम 95;

	स्वभाव:
	    यदि (c > 0)
		// character match fails often
		प्रत्यागम 95;
    }

    // something else, includes character classes
    प्रत्यागम 50;
}

/*
 * Skip until the char "c" we know a match must start with.
 */
    स्थिर पूर्णाङ्क
skip_to_start(पूर्णाङ्क c, colnr_T *colp)
{
    char_u *s;

    // Used often, do some work to avoid call overhead.
    यदि (!rex.reg_ic && !has_mbyte)
	s = vim_strbyte(rex.ओळ + *colp, c);
    अन्यथा
	s = cstrchr(rex.ओळ + *colp, c);
    यदि (s == रिक्त)
	प्रत्यागम FAIL;
    *colp = (पूर्णाङ्क)(s - rex.ओळ);
    प्रत्यागम OK;
}

/*
 * Check for a match with match_text.
 * Called after skip_to_start() has found regstart.
 * Returns zero for no match, 1 for a match.
 */
    स्थिर long
find_match_text(colnr_T *startcol, पूर्णाङ्क regstart, char_u *match_text)
{
    colnr_T col = *startcol;
    पूर्णाङ्क	    c1, c2;
    पूर्णाङ्क	    len1, len2;
    पूर्णाङ्क	    match;

    प्रति (;;)
    {
	match = TRUE;
	// Skip the input character: case-folding can change its byte length.
	len2 = MB_CPTR2LEN(rex.ओळ + col);
	प्रति (len1 = 0; match_text[len1] != NUL; len1 += MB_CHAR2LEN(c1))
	{
	    c1 = PTR2CHAR(match_text + len1);
	    c2 = PTR2CHAR(rex.ओळ + col + len2);
	    यदि (c1 != c2 && (!rex.reg_ic || MB_CASEFOLD(c1) != MB_CASEFOLD(c2)))
	    {
		match = FALSE;
		भङ्ग;
	    }
	    len2 += enc_utf8 ? utf_ptr2len(rex.ओळ + col + len2)
							     : MB_CHAR2LEN(c2);
	}
	यदि (match
		// check that no composing char follows
		&& !(enc_utf8
			  && utf_iscomposing(PTR2CHAR(rex.ओळ + col + len2))))
	{
	    cleanup_subexpr();
	    यदि (REG_MULTI)
	    {
		rex.reg_startpos[0].lnum = rex.lnum;
		rex.reg_startpos[0].col = col;
		rex.reg_endpos[0].lnum = rex.lnum;
		rex.reg_endpos[0].col = col + len2;
	    }
	    अन्यथा
	    {
		rex.reg_startp[0] = rex.ओळ + col;
		rex.reg_endp[0] = rex.ओळ + col + len2;
	    }
	    *startcol = col;
	    प्रत्यागम 1L;
	}

	// Try finding regstart after the current match.
	col += MB_CPTR2LEN(rex.ओळ + col); // skip the input character
	यदि (skip_to_start(regstart, &col) == FAIL)
	    भङ्ग;
    }

    *startcol = col;
    प्रत्यागम 0L;
}

/*
 * Check whether the composing characters at the current input position match
 * the NFA composing sub-expression that starts at "sta" (the first state
 * after NFA_COMPOSING).  "curc" is the base character and "clen" its byte
 * length.
 * Returns OK when it matches, FAIL otherwise.
 */
    स्थिर पूर्णाङ्क
match_composing(nfa_state_T *sta, पूर्णाङ्क curc, पूर्णाङ्क clen)
{
    पूर्णाङ्क		mc = curc;
    पूर्णाङ्क		len = 0;
    पूर्णाङ्क		cchars[MAX_MCO];
    पूर्णाङ्क		ccount = 0;
    पूर्णाङ्क		j;

    यदि (utf_iscomposing(sta->c))
	// Only match composing character(s), ignore base character.  Used
	// for ".{composing}" and "{composing}" (no preceding character).
	len += mb_char2len(mc);

    यदि (rex.reg_icombine && len == 0)
	// If \Z was present, then ignore composing characters.  When
	// ignoring the base character this always matches.
	प्रत्यागम sta->c == curc ? OK : FAIL;

    // Check base character matches first, unless ignored.
    यदि (len == 0 && mc != sta->c)
	प्रत्यागम FAIL;

    यदि (len == 0)
    {
	len += mb_char2len(mc);
	sta = sta->out;
    }

    // We don't care about the order of composing characters.  Get them into
    // cchars[] first.
    यावत् (len < clen)
    {
	mc = mb_ptr2char(rex.आदान + len);
	cchars[ccount++] = mc;
	len += mb_char2len(mc);
	यदि (ccount == MAX_MCO)
	    भङ्ग;
    }

    // Check that each composing char in the pattern matches a composing char
    // in the text.  We do not check if all composing chars are matched.
    यावत् (sta->c != NFA_END_COMPOSING)
    {
	प्रति (j = 0; j < ccount; ++j)
	    यदि (cchars[j] == sta->c)
		भङ्ग;
	यदि (j == ccount)
	    प्रत्यागम FAIL;
	sta = sta->out;
    }

    प्रत्यागम OK;
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
    स्थिर पूर्णाङ्क
nfa_regmatch(
    nfa_regprog_T	*prog,
    nfa_state_T		*start,
    regsubs_T		*submatch,
    regsubs_T		*m)
{
    पूर्णाङ्क		result = FALSE;
    size_t	size = 0;
    पूर्णाङ्क		flag = 0;
    पूर्णाङ्क		go_to_nextline = FALSE;
    nfa_thread_T *t;
    nfa_list_T	list[2];
    पूर्णाङ्क		listidx;
    nfa_list_T	*thislist;
    nfa_list_T	*nextlist;
    पूर्णाङ्क		*listids = रिक्त;
    पूर्णाङ्क		listids_len = 0;
    nfa_state_T *add_state;
    पूर्णाङ्क		add_here;
    पूर्णाङ्क		add_मोजणी;
    पूर्णाङ्क		add_off = 0;
    पूर्णाङ्क		toplevel = start->c == NFA_MOPEN;
    regsubs_T	*r;
#ifdef NFA_REGEXP_DEBUG_LOG
    FILE	*debug;
#endif

    // Some patterns may take a long time to match, especially when using
    // recursive_regmatch(). Allow interrupting them with CTRL-C.
    fast_breakcheck();
    यदि (got_int)
	प्रत्यागम FALSE;
#ifdef FEAT_RELTIME
    यदि (nfa_did_time_out())
	प्रत्यागम FALSE;
#endif

#ifdef NFA_REGEXP_DEBUG_LOG
    debug = fopen(NFA_REGEXP_DEBUG_LOG, "a");
    यदि (debug == रिक्त)
    {
	semsg("(NFA) COULD NOT OPEN %s!", NFA_REGEXP_DEBUG_LOG);
	प्रत्यागम FALSE;
    }
#endif
    nfa_match = FALSE;

    // Allocate memory for the lists of nodes.
    size = (prog->nstate + 1) * परिमाण(nfa_thread_T);

    // Reuse cached list buffers from prog when available (top-level call).
    // Recursive calls must allocate their own buffers.
    यदि (toplevel && prog->listbuf[0] != रिक्त)
    {
	list[0].t = (nfa_thread_T *)prog->listbuf[0];
	list[1].t = (nfa_thread_T *)prog->listbuf[1];
	prog->listbuf[0] = रिक्त;
	prog->listbuf[1] = रिक्त;
    }
    अन्यथा
    {
	list[0].t = alloc(size);
	list[1].t = alloc(size);
    }
    list[0].len = prog->nstate + 1;
    list[1].len = prog->nstate + 1;
    यदि (list[0].t == रिक्त || list[1].t == रिक्त)
	गच्छ theend;

#ifdef ENABLE_LOG
    log_fd = fopen(NFA_REGEXP_RUN_LOG, "a");
    यदि (log_fd == रिक्त)
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
    यदि (toplevel)
    {
	यदि (REG_MULTI)
	{
	    m->norm.list.multi[0].start_lnum = rex.lnum;
	    m->norm.list.multi[0].start_col = (colnr_T)(rex.आदान - rex.ओळ);
	    m->norm.orig_start_col = m->norm.list.multi[0].start_col;
	}
	अन्यथा
	    m->norm.list.ओळ[0].start = rex.आदान;
	m->norm.in_use = 1;
	r = addstate(thislist, start->out, m, रिक्त, 0, 0);
    }
    अन्यथा
	r = addstate(thislist, start, m, रिक्त, 0, 0);
    यदि (r == रिक्त)
    {
	nfa_match = NFA_TOO_EXPENSIVE;
	गच्छ theend;
    }

#define	ADD_STATE_IF_MATCH(state)	\
    यदि (result)				\
    {					\
	add_state = state->out;		\
	add_off = clen;			\
    }

    /*
     * Run for each character.
     */
    प्रति (;;)
    {
	पूर्णाङ्क	curc;
	पूर्णाङ्क	clen;

	यदि (has_mbyte)
	{
	    // Fast path for an ASCII byte not followed by a composing
	    // character, matching the check in utfc_ptr2len().  Avoids two
	    // indirect calls for the common case.
	    यदि (rex.आदान[0] != NUL && rex.आदान[0] < 0x80 && rex.आदान[1] < 0x80)
	    {
		curc = rex.आदान[0];
		clen = 1;
	    }
	    अन्यथा
	    {
		curc = (*mb_ptr2char)(rex.आदान);
		clen = (*mb_ptr2len)(rex.आदान);
	    }
	}
	अन्यथा
	{
	    curc = *rex.आदान;
	    clen = 1;
	}
	यदि (curc == NUL)
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
	यदि (prog->re_engine == AUTOMATIC_ENGINE
		&& (rex.nfa_listid >= NFA_MAX_STATES
#ifdef FEAT_EVAL
		    || nfa_fail_for_testing
#endif
		    ))
	{
	    // too many states, retry with old engine
	    nfa_match = NFA_TOO_EXPENSIVE;
	    गच्छ theend;
	}

	thislist->id = rex.nfa_listid;
	nextlist->id = rex.nfa_listid + 1;

#ifdef ENABLE_LOG
	fprintf(log_fd, "------------------------------------------\n");
	fprintf(log_fd, ">>> Reginput is \"%s\"\n", rex.आदान);
	fprintf(log_fd, ">>> Advanced one character... Current char is %c (code %d) \n", curc, (पूर्णाङ्क)curc);
	fprintf(log_fd, ">>> Thislist has %d states available: ", thislist->n);
	{
	    पूर्णाङ्क i;

	    प्रति (i = 0; i < thislist->n; i++)
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
	यदि (thislist->n == 0)
	    भङ्ग;

	// compute nextlist
	प्रति (listidx = 0; listidx < thislist->n; ++listidx)
	{
	    // If the list gets very long there probably is something wrong.
	    // At least allow interrupting with CTRL-C.  Only check once in a
	    // while, calling ui_breakcheck() for every state is too slow.
	    स्थिर पूर्णाङ्क	breakcheck_मोजणी = 0;  // using "static" makes it faster
	    यदि (unlikely(++breakcheck_मोजणी >= 100000))
	    {
		ui_breakcheck();
		breakcheck_मोजणी = 0;
	    }
	    यदि (got_int)
		भङ्ग;
#ifdef FEAT_RELTIME
	    यदि (nfa_did_time_out())
		भङ्ग;
#endif
	    t = &thislist->t[listidx];

#ifdef NFA_REGEXP_DEBUG_LOG
	    nfa_set_code(t->state->c);
	    fprintf(debug, "%s, ", code);
#endif
#ifdef ENABLE_LOG
	    {
		पूर्णाङ्क col;

		यदि (t->subs.norm.in_use <= 0)
		    col = -1;
		अन्यथा यदि (REG_MULTI)
		    col = t->subs.norm.list.multi[0].start_col;
		अन्यथा
		    col = (पूर्णाङ्क)(t->subs.norm.list.ओळ[0].start - rex.ओळ);
		nfa_set_code(t->state->c);
		fprintf(log_fd, "(%d) char %d %s (start col %d)%s... \n",
			abs(t->state->id), (पूर्णाङ्क)t->state->c, code, col,
			pim_info(&t->pim));
	    }
#endif

	    /*
	     * Handle the possible codes of the current state.
	     * The most important is NFA_MATCH.
	     */
	    add_state = रिक्त;
	    add_here = FALSE;
	    add_मोजणी = 0;
	    विकल्प (t->state->c)
	    {
	    पक्ष NFA_MATCH:
	      {
		// If the match is not at the start of the line, ends before a
		// composing characters and rex.reg_icombine is not set, that
		// is not really a match.
		यदि (enc_utf8 && !rex.reg_icombine
			     && rex.आदान != rex.ओळ && utf_iscomposing(curc))
		    भङ्ग;

		nfa_match = TRUE;
		copy_sub(&submatch->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		यदि (rex.nfa_has_zsubexpr)
		    copy_sub(&submatch->synt, &t->subs.synt);
#endif
#ifdef ENABLE_LOG
		log_subsexpr(&t->subs);
#endif
		// Found the left-most longest match, do not look at any other
		// states at this position.  When the list of states is going
		// to be empty quit without advancing, so that "rex.input" is
		// correct.
		यदि (nextlist->n == 0)
		    clen = 0;
		गच्छ nextchar;
	      }

	    पक्ष NFA_END_INVISIBLE:
	    पक्ष NFA_END_INVISIBLE_NEG:
	    पक्ष NFA_END_PATTERN:
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
		यदि (nfa_endp != रिक्त)
		{
		    यदि (REG_MULTI)
			fprintf(log_fd, "Current lnum: %d, endp lnum: %d; current col: %d, endp col: %d\n",
				(पूर्णाङ्क)rex.lnum,
				(पूर्णाङ्क)nfa_endp->se_u.pos.lnum,
				(पूर्णाङ्क)(rex.आदान - rex.ओळ),
				nfa_endp->se_u.pos.col);
		    अन्यथा
			fprintf(log_fd, "Current col: %d, endp col: %d\n",
				(पूर्णाङ्क)(rex.आदान - rex.ओळ),
				(पूर्णाङ्क)(nfa_endp->se_u.ptr - rex.आदान));
		}
#endif
		// If "nfa_endp" is set it's only a match if it ends at
		// "nfa_endp"
		यदि (nfa_endp != रिक्त && (REG_MULTI
			? (rex.lnum != nfa_endp->se_u.pos.lnum
			    || (पूर्णाङ्क)(rex.आदान - rex.ओळ)
						!= nfa_endp->se_u.pos.col)
			: rex.आदान != nfa_endp->se_u.ptr))
		    भङ्ग;

		// do not set submatches for \@!
		यदि (t->state->c != NFA_END_INVISIBLE_NEG)
		{
		    copy_sub(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		    यदि (rex.nfa_has_zsubexpr)
			copy_sub(&m->synt, &t->subs.synt);
#endif
		}
#ifdef ENABLE_LOG
		fprintf(log_fd, "Match found:\n");
		log_subsexpr(m);
#endif
		nfa_match = TRUE;
		// See comment above at "goto nextchar".
		यदि (nextlist->n == 0)
		    clen = 0;
		गच्छ nextchar;

	    पक्ष NFA_START_INVISIBLE:
	    पक्ष NFA_START_INVISIBLE_FIRST:
	    पक्ष NFA_START_INVISIBLE_NEG:
	    पक्ष NFA_START_INVISIBLE_NEG_FIRST:
	    पक्ष NFA_START_INVISIBLE_BEFORE:
	    पक्ष NFA_START_INVISIBLE_BEFORE_FIRST:
	    पक्ष NFA_START_INVISIBLE_BEFORE_NEG:
	    पक्ष NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
		{
#ifdef ENABLE_LOG
		    fprintf(log_fd, "Failure chance invisible: %d, what follows: %d\n",
			    failure_chance(t->state->out, 0),
			    failure_chance(t->state->out1->out, 0));
#endif
		    // Do it directly if there already is a PIM or when
		    // nfa_postprocess() detected it will work better.
		    यदि (t->pim.result != NFA_PIM_UNUSED
			 || t->state->c == NFA_START_INVISIBLE_FIRST
			 || t->state->c == NFA_START_INVISIBLE_NEG_FIRST
			 || t->state->c == NFA_START_INVISIBLE_BEFORE_FIRST
			 || t->state->c == NFA_START_INVISIBLE_BEFORE_NEG_FIRST)
		    {
			पूर्णाङ्क in_use = m->norm.in_use;

			// Copy submatch info for the recursive call, opposite
			// of what happens on success below.
			copy_sub_off(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
			यदि (rex.nfa_has_zsubexpr)
			    copy_sub_off(&m->synt, &t->subs.synt);
#endif

			/*
			 * First try matching the invisible match, then what
			 * follows.
			 */
			result = recursive_regmatch(t->state, रिक्त, prog,
					  submatch, m, &listids, &listids_len);
			यदि (result == NFA_TOO_EXPENSIVE)
			{
			    nfa_match = result;
			    गच्छ theend;
			}

			// for \@! and \@<! it is a match when the result is
			// FALSE
			यदि (result != (t->state->c == NFA_START_INVISIBLE_NEG
			       || t->state->c == NFA_START_INVISIBLE_NEG_FIRST
			       || t->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			       || t->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&t->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
			    यदि (rex.nfa_has_zsubexpr)
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
		    अन्यथा
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
			यदि (REG_MULTI)
			{
			    pim.end.pos.col = (पूर्णाङ्क)(rex.आदान - rex.ओळ);
			    pim.end.pos.lnum = rex.lnum;
			}
			अन्यथा
			    pim.end.ptr = rex.आदान;

			// t->state->out1 is the corresponding END_INVISIBLE
			// node; Add its out to the current list (zero-width
			// match).
			यदि (addstate_here(thislist, t->state->out1->out,
					     &t->subs, &pim, &listidx) == रिक्त)
			{
			    nfa_match = NFA_TOO_EXPENSIVE;
			    गच्छ theend;
			}
		    }
		}
		भङ्ग;

	    पक्ष NFA_START_PATTERN:
	      {
		nfa_state_T *skip = रिक्त;
#ifdef ENABLE_LOG
		पूर्णाङ्क	    skip_lid = 0;
#endif

		// There is no point in trying to match the pattern if the
		// output state is not going to be added to the list.
		यदि (state_in_list(nextlist, t->state->out1->out, &t->subs))
		{
		    skip = t->state->out1->out;
#ifdef ENABLE_LOG
		    skip_lid = nextlist->id;
#endif
		}
		अन्यथा यदि (state_in_list(nextlist,
					  t->state->out1->out->out, &t->subs))
		{
		    skip = t->state->out1->out->out;
#ifdef ENABLE_LOG
		    skip_lid = nextlist->id;
#endif
		}
		अन्यथा यदि (state_in_list(thislist,
					  t->state->out1->out->out, &t->subs))
		{
		    skip = t->state->out1->out->out;
#ifdef ENABLE_LOG
		    skip_lid = thislist->id;
#endif
		}
		यदि (skip != रिक्त)
		{
#ifdef ENABLE_LOG
		    nfa_set_code(skip->c);
		    fprintf(log_fd, "> Not trying to match pattern, output state %d is already in list %d. char %d: %s\n",
			    abs(skip->id), skip_lid, skip->c, code);
#endif
		    भङ्ग;
		}
		// Copy submatch info to the recursive call, opposite of what
		// happens afterwards.
		copy_sub_off(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		यदि (rex.nfa_has_zsubexpr)
		    copy_sub_off(&m->synt, &t->subs.synt);
#endif

		// First try matching the pattern.
		result = recursive_regmatch(t->state, रिक्त, prog,
					  submatch, m, &listids, &listids_len);
		यदि (result == NFA_TOO_EXPENSIVE)
		{
		    nfa_match = result;
		    गच्छ theend;
		}
		यदि (result)
		{
		    पूर्णाङ्क bytelen;

#ifdef ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN matches:\n");
		    log_subsexpr(m);
#endif
		    // Copy submatch info from the recursive call
		    copy_sub_off(&t->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
		    यदि (rex.nfa_has_zsubexpr)
			copy_sub_off(&t->subs.synt, &m->synt);
#endif
		    // Now we need to skip over the matched text and then
		    // continue with what follows.
		    यदि (REG_MULTI)
			// TODO: multi-line match
			bytelen = m->norm.list.multi[0].end_col
						  - (पूर्णाङ्क)(rex.आदान - rex.ओळ);
		    अन्यथा
			bytelen = (पूर्णाङ्क)(m->norm.list.ओळ[0].end - rex.आदान);

#ifdef ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN length: %d\n", bytelen);
#endif
		    यदि (bytelen == 0)
		    {
			// empty match, output of corresponding
			// NFA_END_PATTERN/NFA_SKIP to be used at current
			// position
			add_here = TRUE;
			add_state = t->state->out1->out->out;
		    }
		    अन्यथा यदि (bytelen <= clen)
		    {
			// match current character, output of corresponding
			// NFA_END_PATTERN to be used at next position.
			add_state = t->state->out1->out->out;
			add_off = clen;
		    }
		    अन्यथा
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			add_state = t->state->out1->out;
			add_off = bytelen;
			add_मोजणी = bytelen - clen;
		    }
		}
		भङ्ग;
	      }

	    पक्ष NFA_BOL:
		यदि (rex.आदान == rex.ओळ)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		भङ्ग;

	    पक्ष NFA_EOL:
		यदि (curc == NUL)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		भङ्ग;

	    पक्ष NFA_BOW:
		result = TRUE;

		यदि (curc == NUL)
		    result = FALSE;
		अन्यथा यदि (has_mbyte)
		{
		    पूर्णाङ्क this_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.आदान, rex.reg_buf);
		    यदि (this_class <= 1)
			result = FALSE;
		    अन्यथा यदि (reg_prev_class() == this_class)
			result = FALSE;
		}
		अन्यथा यदि (!vim_iswordc_buf(curc, rex.reg_buf)
			   || (rex.आदान > rex.ओळ
			       && vim_iswordc_buf(rex.आदान[-1], rex.reg_buf)))
		    result = FALSE;
		यदि (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		भङ्ग;

	    पक्ष NFA_EOW:
		result = TRUE;
		यदि (rex.आदान == rex.ओळ)
		    result = FALSE;
		अन्यथा यदि (has_mbyte)
		{
		    पूर्णाङ्क this_class, prev_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.आदान, rex.reg_buf);
		    prev_class = reg_prev_class();
		    यदि (this_class == prev_class
					|| prev_class == 0 || prev_class == 1)
			result = FALSE;
		}
		अन्यथा यदि (!vim_iswordc_buf(rex.आदान[-1], rex.reg_buf)
			|| (rex.आदान[0] != NUL
					&& vim_iswordc_buf(curc, rex.reg_buf)))
		    result = FALSE;
		यदि (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		भङ्ग;

	    पक्ष NFA_BOF:
		यदि (rex.lnum == 0 && rex.आदान == rex.ओळ
				     && (!REG_MULTI || rex.reg_firstlnum == 1))
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		भङ्ग;

	    पक्ष NFA_EOF:
		यदि (rex.lnum == rex.reg_maxline && curc == NUL)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		भङ्ग;

	    पक्ष NFA_COMPOSING:
	    {
		nfa_state_T *end;

		result = match_composing(t->state->out, curc, clen);

		end = t->state->out1;	    // NFA_END_COMPOSING
		ADD_STATE_IF_MATCH(end);
		भङ्ग;
	    }

	    पक्ष NFA_NEWL:
		यदि (curc == NUL && !rex.reg_ओळ_lbr && REG_MULTI
						&& rex.lnum <= rex.reg_maxline)
		{
		    go_to_nextline = TRUE;
		    // Pass -1 for the offset, which means taking the position
		    // at the start of the next line.
		    add_state = t->state->out;
		    add_off = -1;
		}
		अन्यथा यदि (curc == '\n' && rex.reg_ओळ_lbr)
		{
		    // match \n as if it is an ordinary character
		    add_state = t->state->out;
		    add_off = 1;
		}
		भङ्ग;

	    पक्ष NFA_START_COLL:
	    पक्ष NFA_START_NEG_COLL:
	      {
		// What follows is a list of characters, until NFA_END_COLL.
		// One of them must match or none of them must match.
		nfa_state_T	*state;
		पूर्णाङ्क		result_if_matched;
		पूर्णाङ्क		c1, c2;

		// Never match EOL. If it's part of the collection it is added
		// as a separate state with an OR.
		यदि (curc == NUL)
		    भङ्ग;

		state = t->state->out;
		result_if_matched = (t->state->c == NFA_START_COLL);
		प्रति (;;)
		{
		    यदि (state->c == NFA_COMPOSING)
		    {
			nfa_state_T *end;

			result = match_composing(t->state->out->out, curc,
									clen);

			यदि (t->state->out->out1 != रिक्त
				&& t->state->out->out1->c == NFA_END_COMPOSING)
			{
			    end = t->state->out->out1;
			    ADD_STATE_IF_MATCH(end);
			}
			भङ्ग;
		    }
		    यदि (state->c == NFA_END_COLL)
		    {
			result = !result_if_matched;
			भङ्ग;
		    }
		    यदि (state->c == NFA_RANGE_MIN)
		    {
			c1 = state->val;
			state = state->out; // advance to NFA_RANGE_MAX
			c2 = state->val;
#ifdef ENABLE_LOG
			fprintf(log_fd, "NFA_RANGE_MIN curc=%d c1=%d c2=%d\n",
				curc, c1, c2);
#endif
			यदि (curc >= c1 && curc <= c2)
			{
			    result = result_if_matched;
			    भङ्ग;
			}
			यदि (rex.reg_ic)
			{
			    पूर्णाङ्क curc_खालचा = MB_CASEFOLD(curc);
			    पूर्णाङ्क done = FALSE;

			    प्रति ( ; c1 <= c2; ++c1)
				यदि (MB_CASEFOLD(c1) == curc_खालचा)
				{
				    result = result_if_matched;
				    done = TRUE;
				    भङ्ग;
				}
			    यदि (done)
				भङ्ग;
			}
		    }
		    अन्यथा यदि (state->c < 0 ? check_char_class(state->c, curc)
			       : (curc == state->c
				   || (rex.reg_ic && MB_CASEFOLD(curc)
						    == MB_CASEFOLD(state->c))))
		    {
			result = result_if_matched;
			भङ्ग;
		    }
		    state = state->out;
		}
		यदि (result)
		{
		    // next state is in out of the NFA_END_COLL, out1 of
		    // START points to the END state
		    add_state = t->state->out1->out;
		    add_off = clen;
		}
		भङ्ग;
	      }

	    पक्ष NFA_ANY:
		// Any char except '\0', (end of input) does not match.
		यदि (curc > 0)
		{
		    add_state = t->state->out;
		    add_off = clen;
		}
		भङ्ग;

	    पक्ष NFA_ANY_COMPOSING:
		// On a composing character skip over it.  Otherwise do
		// nothing.  Always matches.
		यदि (enc_utf8 && utf_iscomposing(curc))
		{
		    add_off = clen;
		}
		अन्यथा
		{
		    add_here = TRUE;
		    add_off = 0;
		}
		add_state = t->state->out;
		भङ्ग;

	    /*
	     * Character classes like \a for alpha, \d for digit etc.
	     */
	    पक्ष NFA_IDENT:	//  \i
		result = vim_isIDc(curc);
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_SIDENT:	//  \I
		result = !VIM_ISDIGIT(curc) && vim_isIDc(curc);
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_KWORD:	//  \k
		result = vim_iswordp_buf(rex.आदान, rex.reg_buf);
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_SKWORD:	//  \K
		result = !VIM_ISDIGIT(curc)
				     && vim_iswordp_buf(rex.आदान, rex.reg_buf);
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_FNAME:	//  \f
		result = vim_isfilec(curc);
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_SFNAME:	//  \F
		result = !VIM_ISDIGIT(curc) && vim_isfilec(curc);
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_PRINT:	//  \p
		result = vim_isprintc(PTR2CHAR(rex.आदान));
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_SPRINT:	//  \P
		result = !VIM_ISDIGIT(curc) && vim_isprintc(PTR2CHAR(rex.आदान));
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_WHITE:	//  \s
		result = VIM_ISWHITE(curc);
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_NWHITE:	//  \S
		result = curc != NUL && !VIM_ISWHITE(curc);
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_DIGIT:	//  \d
		result = ri_digit(curc);
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_NDIGIT:	//  \D
		result = curc != NUL && !ri_digit(curc);
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_HEX:	//  \x
		result = ri_hex(curc);
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_NHEX:	//  \X
		result = curc != NUL && !ri_hex(curc);
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_OCTAL:	//  \o
		result = ri_octal(curc);
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_NOCTAL:	//  \O
		result = curc != NUL && !ri_octal(curc);
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_WORD:	//  \w
		result = ri_word(curc);
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_NWORD:	//  \W
		result = curc != NUL && !ri_word(curc);
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_HEAD:	//  \h
		result = ri_head(curc);
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_NHEAD:	//  \H
		result = curc != NUL && !ri_head(curc);
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_ALPHA:	//  \a
		result = ri_alpha(curc);
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_NALPHA:	//  \A
		result = curc != NUL && !ri_alpha(curc);
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_LOWER:	//  \l
		result = ri_lower(curc);
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_NLOWER:	//  \L
		result = curc != NUL && !ri_lower(curc);
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_UPPER:	//  \u
		result = ri_upper(curc);
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_NUPPER:	// \U
		result = curc != NUL && !ri_upper(curc);
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_LOWER_IC:	// [a-z]
		result = ri_lower(curc) || (rex.reg_ic && ri_upper(curc));
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_NLOWER_IC:	// [^a-z]
		result = curc != NUL
			&& !(ri_lower(curc) || (rex.reg_ic && ri_upper(curc)));
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_UPPER_IC:	// [A-Z]
		result = ri_upper(curc) || (rex.reg_ic && ri_lower(curc));
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_NUPPER_IC:	// ^[A-Z]
		result = curc != NUL
			&& !(ri_upper(curc) || (rex.reg_ic && ri_lower(curc)));
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;

	    पक्ष NFA_BACKREF1:
	    पक्ष NFA_BACKREF2:
	    पक्ष NFA_BACKREF3:
	    पक्ष NFA_BACKREF4:
	    पक्ष NFA_BACKREF5:
	    पक्ष NFA_BACKREF6:
	    पक्ष NFA_BACKREF7:
	    पक्ष NFA_BACKREF8:
	    पक्ष NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	    पक्ष NFA_ZREF1:
	    पक्ष NFA_ZREF2:
	    पक्ष NFA_ZREF3:
	    पक्ष NFA_ZREF4:
	    पक्ष NFA_ZREF5:
	    पक्ष NFA_ZREF6:
	    पक्ष NFA_ZREF7:
	    पक्ष NFA_ZREF8:
	    पक्ष NFA_ZREF9:
#endif
		// \1 .. \9  \z1 .. \z9
	      {
		पूर्णाङ्क subidx;
		पूर्णाङ्क bytelen;

#ifdef FEAT_SYN_HL
		यदि (t->state->c >= NFA_BACKREF1 && t->state->c <= NFA_BACKREF9)
#endif
		{
		    subidx = t->state->c - NFA_BACKREF1 + 1;
		    result = match_backref(&t->subs.norm, subidx, &bytelen);
		}
#ifdef FEAT_SYN_HL
		अन्यथा
		{
		    subidx = t->state->c - NFA_ZREF1 + 1;
		    result = match_zref(subidx, &bytelen);
		}
#endif

		यदि (result)
		{
		    यदि (bytelen == 0)
		    {
			// empty match always works, output of NFA_SKIP to be
			// used next
			add_here = TRUE;
			add_state = t->state->out->out;
		    }
		    अन्यथा यदि (bytelen <= clen)
		    {
			// match current character, jump ahead to out of
			// NFA_SKIP
			add_state = t->state->out->out;
			add_off = clen;
		    }
		    अन्यथा
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			add_state = t->state->out;
			add_off = bytelen;
			add_मोजणी = bytelen - clen;
		    }
		}
		भङ्ग;
	      }
	    पक्ष NFA_SKIP:
	      // character of previous matching \1 .. \9  or \@>
	      यदि (t->मोजणी - clen <= 0)
	      {
		  // end of match, go to what follows
		  add_state = t->state->out;
		  add_off = clen;
	      }
	      अन्यथा
	      {
		  // add state again with decremented count
		  add_state = t->state;
		  add_off = 0;
		  add_मोजणी = t->मोजणी - clen;
	      }
	      भङ्ग;

	    पक्ष NFA_LNUM:
	    पक्ष NFA_LNUM_GT:
	    पक्ष NFA_LNUM_LT:
		result = (REG_MULTI &&
			nfa_re_अंक_cmp(t->state->val, t->state->c - NFA_LNUM,
			    (long_u)(rex.lnum + rex.reg_firstlnum)));
		यदि (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		भङ्ग;

	    पक्ष NFA_COL:
	    पक्ष NFA_COL_GT:
	    पक्ष NFA_COL_LT:
		result = nfa_re_अंक_cmp(t->state->val, t->state->c - NFA_COL,
			(long_u)(rex.आदान - rex.ओळ) + 1);
		यदि (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		भङ्ग;

	    पक्ष NFA_VCOL:
	    पक्ष NFA_VCOL_GT:
	    पक्ष NFA_VCOL_LT:
		{
		    पूर्णाङ्क     op = t->state->c - NFA_VCOL;
		    colnr_T col = (colnr_T)(rex.आदान - rex.ओळ);
		    win_T   *wp = rex.reg_win == रिक्त ? curwin : rex.reg_win;

		    // Bail out quickly when there can't be a match, avoid the
		    // overhead of win_linetabsize() on long lines.
		    यदि (op != 1 && col > t->state->val
			    * (has_mbyte ? MB_MAXBYTES : 1))
			भङ्ग;
		    result = FALSE;
		    यदि (op == 1 && col - 1 > t->state->val && col > 100)
		    {
			पूर्णाङ्क ts = wp->w_buffer->b_p_ts;

			// Guess that a character won't use more columns than
			// 'tabstop', with a minimum of 4.
			यदि (ts < 4)
			    ts = 4;
			result = col > t->state->val * ts;
		    }
		    यदि (!result)
		    {
			linenr_T    lnum = REG_MULTI
					    ? rex.reg_firstlnum + rex.lnum : 1;
			long_u	    vcol;

			यदि (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_buffer->b_ml.ml_ओळ_मोजणी))
			    lnum = 1;
			vcol = (long_u)win_linetabsize(wp, lnum, rex.ओळ, col);
			result = nfa_re_अंक_cmp(t->state->val, op, vcol + 1);
		    }
		    यदि (result)
		    {
			add_here = TRUE;
			add_state = t->state->out;
		    }
		}
		भङ्ग;

	    पक्ष NFA_MARK:
	    पक्ष NFA_MARK_GT:
	    पक्ष NFA_MARK_LT:
	      {
		pos_T	*pos;
		size_t	col = REG_MULTI ? rex.आदान - rex.ओळ : 0;

		pos = getmark_buf(rex.reg_buf, t->state->val, FALSE);

		// Line may have been freed, get it again.
		यदि (REG_MULTI)
		{
		    rex.ओळ = reg_getline(rex.lnum);
		    rex.आदान = rex.ओळ + col;
		}

		// Compare the mark position to the match position, if the mark
		// exists and mark is set in reg_buf.
		यदि (pos != रिक्त && pos->lnum > 0)
		{
		    colnr_T pos_col = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->col == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->col;

		    result = (pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_col == (colnr_T)(rex.आदान - rex.ओळ)
				    ? t->state->c == NFA_MARK
				    : (pos_col < (colnr_T)(rex.आदान - rex.ओळ)
					? t->state->c == NFA_MARK_GT
					: t->state->c == NFA_MARK_LT))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? t->state->c == NFA_MARK_GT
				    : t->state->c == NFA_MARK_LT));
		    यदि (result)
		    {
			add_here = TRUE;
			add_state = t->state->out;
		    }
		}
		भङ्ग;
	      }

	    पक्ष NFA_CURSOR:
		result = (rex.reg_win != रिक्त
			&& (rex.lnum + rex.reg_firstlnum
						 == rex.reg_win->w_cursor.lnum)
			&& ((colnr_T)(rex.आदान - rex.ओळ)
						== rex.reg_win->w_cursor.col));
		यदि (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		भङ्ग;

	    पक्ष NFA_VISUAL:
		result = reg_match_visual();
		यदि (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		भङ्ग;

	    पक्ष NFA_MOPEN1:
	    पक्ष NFA_MOPEN2:
	    पक्ष NFA_MOPEN3:
	    पक्ष NFA_MOPEN4:
	    पक्ष NFA_MOPEN5:
	    पक्ष NFA_MOPEN6:
	    पक्ष NFA_MOPEN7:
	    पक्ष NFA_MOPEN8:
	    पक्ष NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	    पक्ष NFA_ZOPEN:
	    पक्ष NFA_ZOPEN1:
	    पक्ष NFA_ZOPEN2:
	    पक्ष NFA_ZOPEN3:
	    पक्ष NFA_ZOPEN4:
	    पक्ष NFA_ZOPEN5:
	    पक्ष NFA_ZOPEN6:
	    पक्ष NFA_ZOPEN7:
	    पक्ष NFA_ZOPEN8:
	    पक्ष NFA_ZOPEN9:
#endif
	    पक्ष NFA_NOPEN:
	    पक्ष NFA_ZSTART:
		// These states are only added to be able to bail out when
		// they are added again, nothing is to be done.
		भङ्ग;

	    स्वभाव:	// regular character
	      {
		पूर्णाङ्क c = t->state->c;

#ifdef DEBUG
		यदि (c < 0)
		    siemsg("Negative state char: %ld", (long)c);
#endif
		result = (c == curc);

		यदि (!result && rex.reg_ic)
		    result = MB_CASEFOLD(c) == MB_CASEFOLD(curc);
		// If rex.reg_icombine is not set only skip over the character
		// itself.  When it is set skip over composing characters.
		यदि (result && enc_utf8 && !rex.reg_icombine)
		    clen = utf_ptr2len(rex.आदान);
		ADD_STATE_IF_MATCH(t->state);
		भङ्ग;
	      }

	    } // switch (t->state->c)

	    यदि (add_state != रिक्त)
	    {
		nfa_pim_T *pim;
		nfa_pim_T pim_copy;

		यदि (t->pim.result == NFA_PIM_UNUSED)
		    pim = रिक्त;
		अन्यथा
		    pim = &t->pim;

		// Handle the postponed invisible match if the match might end
		// without advancing and before the end of the line.
		यदि (pim != रिक्त && (clen == 0 || match_follows(add_state, 0)))
		{
		    यदि (pim->result == NFA_PIM_TODO)
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
			यदि (result != (pim->state->c == NFA_START_INVISIBLE_NEG
			     || pim->state->c == NFA_START_INVISIBLE_NEG_FIRST
			     || pim->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&pim->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
			    यदि (rex.nfa_has_zsubexpr)
				copy_sub_off(&pim->subs.synt, &m->synt);
#endif
			}
		    }
		    अन्यथा
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
		    यदि (result != (pim->state->c == NFA_START_INVISIBLE_NEG
			     || pim->state->c == NFA_START_INVISIBLE_NEG_FIRST
			     || pim->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
		    {
			// Copy submatch info from the recursive call
			copy_sub_off(&t->subs.norm, &pim->subs.norm);
#ifdef FEAT_SYN_HL
			यदि (rex.nfa_has_zsubexpr)
			    copy_sub_off(&t->subs.synt, &pim->subs.synt);
#endif
		    }
		    अन्यथा
			// look-behind match failed, don't add the state
			अनुवर्त;

		    // Postponed invisible match was handled, don't add it to
		    // following states.
		    pim = रिक्त;
		}

		// If "pim" points into l->t it will become invalid when
		// adding the state causes the list to be reallocated.  Make a
		// local copy to avoid that.
		यदि (pim == &t->pim)
		{
		    copy_pim(&pim_copy, pim);
		    pim = &pim_copy;
		}

		यदि (add_here)
		    r = addstate_here(thislist, add_state, &t->subs,
								pim, &listidx);
		अन्यथा
		{
		    r = addstate(nextlist, add_state, &t->subs, pim, add_off, 0);
		    यदि (add_मोजणी > 0)
			nextlist->t[nextlist->n - 1].मोजणी = add_मोजणी;
		}
		यदि (r == रिक्त)
		{
		    nfa_match = NFA_TOO_EXPENSIVE;
		    गच्छ theend;
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
	यदि (nfa_match == FALSE
		&& ((toplevel
			&& rex.lnum == 0
			&& clen != 0
			&& (rex.reg_maxcol == 0
			  || (colnr_T)(rex.आदान - rex.ओळ) < rex.reg_maxcol))
		    || (nfa_endp != रिक्त
			&& (REG_MULTI
			    ? (rex.lnum < nfa_endp->se_u.pos.lnum
			       || (rex.lnum == nfa_endp->se_u.pos.lnum
				   && (पूर्णाङ्क)(rex.आदान - rex.ओळ)
						    < nfa_endp->se_u.pos.col))
			    : rex.आदान < nfa_endp->se_u.ptr))))
	{
#ifdef ENABLE_LOG
	    fprintf(log_fd, "(---) STARTSTATE\n");
#endif
	    // Inline optimized code for addstate() if we know the state is
	    // the first MOPEN.
	    यदि (toplevel)
	    {
		पूर्णाङ्क add = TRUE;
		पूर्णाङ्क c;

		यदि (prog->regstart != NUL && clen != 0)
		{
		    यदि (nextlist->n == 0)
		    {
			colnr_T col = (colnr_T)(rex.आदान - rex.ओळ) + clen;

			// Nextlist is empty, we can skip ahead to the
			// character that must appear at the start.
			यदि (skip_to_start(prog->regstart, &col) == FAIL)
			    भङ्ग;
#ifdef ENABLE_LOG
			fprintf(log_fd, "  Skipping ahead %d bytes to regstart\n",
				col - ((colnr_T)(rex.आदान - rex.ओळ) + clen));
#endif
			rex.आदान = rex.ओळ + col - clen;
		    }
		    अन्यथा
		    {
			// Checking if the required start character matches is
			// cheaper than adding a state that won't match.
			c = PTR2CHAR(rex.आदान + clen);
			यदि (c != prog->regstart && (!rex.reg_ic
			     || MB_CASEFOLD(c) != MB_CASEFOLD(prog->regstart)))
			{
#ifdef ENABLE_LOG
			    fprintf(log_fd, "  Skipping start state, regstart does not match\n");
#endif
			    add = FALSE;
			}
		    }
		}

		यदि (add)
		{
		    यदि (REG_MULTI)
		    {
			m->norm.list.multi[0].start_col =
					 (colnr_T)(rex.आदान - rex.ओळ) + clen;
			m->norm.orig_start_col =
					       m->norm.list.multi[0].start_col;
		    }
		    अन्यथा
			m->norm.list.ओळ[0].start = rex.आदान + clen;
		    यदि (addstate(nextlist, start->out, m, रिक्त, clen, 0) == रिक्त)
		    {
			nfa_match = NFA_TOO_EXPENSIVE;
			गच्छ theend;
		    }
		}
	    }
	    अन्यथा
	    {
		char_u	    *save_ओळ = rex.ओळ;
		char_u	    *save_आदान = rex.आदान;
		linenr_T    save_lnum = rex.lnum;

		// At the end of a line the match can only start on the next
		// line, use that position instead of the line break.
		यदि (REG_MULTI && clen == 0 && nfa_endp != रिक्त
					 && rex.lnum < nfa_endp->se_u.pos.lnum)
		{
		    char_u  *next_ओळ = reg_getline(rex.lnum + 1);

		    यदि (next_ओळ != रिक्त)
		    {
			rex.ओळ = next_ओळ;
			rex.आदान = rex.ओळ;
			++rex.lnum;
		    }
		}

		r = addstate(nextlist, start, m, रिक्त, clen, 0);

		rex.ओळ = save_ओळ;
		rex.आदान = save_आदान;
		rex.lnum = save_lnum;

		यदि (r == रिक्त)
		{
		    nfa_match = NFA_TOO_EXPENSIVE;
		    गच्छ theend;
		}
	    }
	}

#ifdef ENABLE_LOG
	fprintf(log_fd, ">>> Thislist had %d states available: ", thislist->n);
	{
	    पूर्णाङ्क i;

	    प्रति (i = 0; i < thislist->n; i++)
		fprintf(log_fd, "%d  ", abs(thislist->t[i].state->id));
	}
	fprintf(log_fd, "\n");
#endif

nextchar:
	// Advance to the next character, or advance to the next line, or
	// finish.
	यदि (clen != 0)
	    rex.आदान += clen;
	अन्यथा यदि (go_to_nextline || (nfa_endp != रिक्त && REG_MULTI
					&& rex.lnum < nfa_endp->se_u.pos.lnum))
	    reg_nextline();
	अन्यथा
	    भङ्ग;

	// Allow interrupting with CTRL-C.
	ओळ_breakcheck();
	यदि (got_int)
	    भङ्ग;
#ifdef FEAT_RELTIME
	यदि (nfa_did_time_out())
	    भङ्ग;
#endif
    }

#ifdef ENABLE_LOG
    यदि (log_fd != stderr)
	fclose(log_fd);
    log_fd = रिक्त;
#endif

theend:
    // Cache list buffers in prog for reuse, or free if prog already has
    // cached buffers (recursive call case).
    यदि (prog->listbuf[0] == रिक्त && list[0].t != रिक्त && list[1].t != रिक्त)
    {
	prog->listbuf[0] = list[0].t;
	prog->listbuf[1] = list[1].t;
    }
    अन्यथा
    {
	vim_free(list[0].t);
	vim_free(list[1].t);
    }
    vim_free(listids);
#undef ADD_STATE_IF_MATCH
#ifdef NFA_REGEXP_DEBUG_LOG
    fclose(debug);
#endif

    प्रत्यागम nfa_match;
}

/*
 * Try match of "prog" with at rex.line["col"].
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    स्थिर long
nfa_regtry(
    nfa_regprog_T   *prog,
    colnr_T	    col,
    पूर्णाङ्क		    *timed_out UNUSED)	// flag set on timeout or NULL
{
    पूर्णाङ्क		i;
    regsubs_T	subs, m;
    nfa_state_T	*start = prog->start;
    पूर्णाङ्क		result;
#ifdef ENABLE_LOG
    FILE	*f;
#endif

    rex.आदान = rex.ओळ + col;
#ifdef FEAT_RELTIME
    nfa_timed_out = timed_out;
#endif

#ifdef ENABLE_LOG
    f = fopen(NFA_REGEXP_RUN_LOG, "a");
    यदि (f != रिक्त)
    {
	fprintf(f, "\n\n\t=======================================================\n");
# ifdef DEBUG
	fprintf(f, "\tRegexp is \"%s\"\n", nfa_regengine.अभिव्यक्ती);
# endif
	fprintf(f, "\tInput text is \"%s\" \n", rex.आदान);
	fprintf(f, "\t=======================================================\n\n");
	nfa_print_state(f, start);
	fprintf(f, "\n\n");
	fclose(f);
    }
    अन्यथा
	emsg("Could not open temporary log file for writing");
#endif

    clear_sub(&subs.norm);
    clear_sub(&m.norm);
#ifdef FEAT_SYN_HL
    clear_sub(&subs.synt);
    clear_sub(&m.synt);
#endif

    result = nfa_regmatch(prog, start, &subs, &m);
    यदि (result == FALSE)
	प्रत्यागम 0;
    अन्यथा यदि (result == NFA_TOO_EXPENSIVE)
	प्रत्यागम result;

    cleanup_subexpr();
    यदि (REG_MULTI)
    {
	प्रति (i = 0; i < subs.norm.in_use; i++)
	{
	    rex.reg_startpos[i].lnum = subs.norm.list.multi[i].start_lnum;
	    rex.reg_startpos[i].col = subs.norm.list.multi[i].start_col;

	    rex.reg_endpos[i].lnum = subs.norm.list.multi[i].end_lnum;
	    rex.reg_endpos[i].col = subs.norm.list.multi[i].end_col;
	}
	यदि (rex.reg_mmatch != रिक्त)
	    rex.reg_mmatch->rmm_matchcol = subs.norm.orig_start_col;

	यदि (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].col = col;
	}
	यदि (rex.reg_endpos[0].lnum < 0)
	{
	    // pattern has a \ze but it didn't match, use current end
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].col = (पूर्णाङ्क)(rex.आदान - rex.ओळ);
	}
	अन्यथा
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    अन्यथा
    {
	प्रति (i = 0; i < subs.norm.in_use; i++)
	{
	    rex.reg_startp[i] = subs.norm.list.ओळ[i].start;
	    rex.reg_endp[i] = subs.norm.list.ओळ[i].end;
	}

	यदि (rex.reg_startp[0] == रिक्त)
	    rex.reg_startp[0] = rex.ओळ + col;
	यदि (rex.reg_endp[0] == रिक्त)
	    rex.reg_endp[0] = rex.आदान;
    }

#ifdef FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = रिक्त;

    यदि (prog->reghasz == REX_SET)
    {
	cleanup_zsubexpr();
	re_extmatch_out = make_extmatch();
	यदि (re_extmatch_out == रिक्त)
	    प्रत्यागम 0;
	// Loop over \z1, \z2, etc.  There is no \z0.
	प्रति (i = 1; i < subs.synt.in_use; i++)
	{
	    यदि (REG_MULTI)
	    {
		संरचना multipos *mpos = &subs.synt.list.multi[i];

		// Only accept single line matches that are valid.
		यदि (mpos->start_lnum >= 0
			&& mpos->start_lnum == mpos->end_lnum
			&& mpos->end_col >= mpos->start_col)
		    re_extmatch_out->matches[i] =
			vim_strnsave(reg_getline(mpos->start_lnum)
							    + mpos->start_col,
					     mpos->end_col - mpos->start_col);
	    }
	    अन्यथा
	    {
		संरचना linepos *lpos = &subs.synt.list.ओळ[i];

		यदि (lpos->start != रिक्त && lpos->end != रिक्त)
		    re_extmatch_out->matches[i] =
			    vim_strnsave(lpos->start, lpos->end - lpos->start);
	    }
	}
    }
#endif

    प्रत्यागम 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 *
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    स्थिर long
nfa_regexec_both(
    char_u	*ओळ,
    colnr_T	startcol,	// column to start looking for match
    पूर्णाङ्क		*timed_out)	// flag set on timeout or NULL
{
    nfa_regprog_T   *prog;
    long	    retval = 0L;
    पूर्णाङ्क		    i;
    colnr_T	    col = startcol;

    यदि (REG_MULTI)
    {
	prog = (nfa_regprog_T *)rex.reg_mmatch->regprog;
	ओळ = reg_getline((linenr_T)0);    // relative to the cursor
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    अन्यथा
    {
	prog = (nfa_regprog_T *)rex.reg_match->regprog;
	rex.reg_startp = rex.reg_match->startp;
	rex.reg_endp = rex.reg_match->endp;
    }

    // Be paranoid...
    यदि (prog == रिक्त || ओळ == रिक्त)
    {
	iemsg(e_null_argument);
	गच्छ theend;
    }

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    यदि (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    अन्यथा यदि (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    यदि (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    rex.ओळ = ओळ;
    rex.lnum = 0;    // relative to line

    rex.nfa_has_zend = prog->has_zend;
    rex.nfa_has_backref = prog->has_backref;
    rex.nfa_nsubexpr = prog->nsubexp;
    rex.nfa_listid = 1;
    rex.nfa_alt_listid = 2;
#ifdef DEBUG
    nfa_regengine.अभिव्यक्ती = prog->pattern;
#endif

    यदि (prog->reganch && col > 0)
	प्रत्यागम 0L;

    rex.need_clear_subexpr = TRUE;
#ifdef FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    यदि (prog->reghasz == REX_SET)
    {
	rex.nfa_has_zsubexpr = TRUE;
	rex.need_clear_zsubexpr = TRUE;
    }
    अन्यथा
    {
	rex.nfa_has_zsubexpr = FALSE;
	rex.need_clear_zsubexpr = FALSE;
    }
#endif

    यदि (prog->regstart != NUL)
    {
	// Skip ahead until a character we know the match must start with.
	// When there is none there is no match.
	यदि (skip_to_start(prog->regstart, &col) == FAIL)
	    प्रत्यागम 0L;

	// If match_text is set it contains the full text that must match.
	// Nothing else to try. Doesn't handle combining chars well.
	यदि (prog->match_text != रिक्त && *prog->match_text != NUL && !rex.reg_icombine)
	{
	    retval = find_match_text(&col, prog->regstart, prog->match_text);
	    यदि (REG_MULTI)
		rex.reg_mmatch->rmm_matchcol = col;
	    अन्यथा
		rex.reg_match->rm_matchcol = col;
	    प्रत्यागम retval;
	}
    }

    // If the start column is past the maximum column: no need to try.
    यदि (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	गच्छ theend;

    // Set the "nstate" used by nfa_regcomp() to zero to trigger an error when
    // it's accidentally used during execution.
    nstate = 0;
    प्रति (i = 0; i < prog->nstate; ++i)
    {
	prog->state[i].id = i;
	prog->state[i].lastlist[0] = 0;
	prog->state[i].lastlist[1] = 0;
    }

    retval = nfa_regtry(prog, col, timed_out);

#ifdef DEBUG
    nfa_regengine.अभिव्यक्ती = रिक्त;
#endif

theend:
    यदि (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs and
	// \ze are used.
	यदि (REG_MULTI)
	{
	    lpos_T *start = &rex.reg_mmatch->startpos[0];
	    lpos_T *end = &rex.reg_mmatch->endpos[0];

	    यदि (end->lnum < start->lnum
			|| (end->lnum == start->lnum && end->col < start->col))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];
	}
	अन्यथा
	{
	    यदि (rex.reg_match->endp[0] < rex.reg_match->startp[0])
		rex.reg_match->endp[0] = rex.reg_match->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_match->rm_matchcol = col;
	}
    }

    प्रत्यागम retval;
}

/*
 * Compile a regular expression into internal code for the NFA matcher.
 * Returns the program in allocated space.  Returns NULL for an error.
 */
    स्थिर regprog_T *
nfa_regcomp(char_u *अभिव्यक्ती, पूर्णाङ्क re_flags)
{
    nfa_regprog_T	*prog = रिक्त;
    size_t		prog_size;
    पूर्णाङ्क			*postfix;

    यदि (अभिव्यक्ती == रिक्त)
	प्रत्यागम रिक्त;

#ifdef DEBUG
    nfa_regengine.अभिव्यक्ती = अभिव्यक्ती;
#endif
    nfa_re_flags = re_flags;

    init_class_tab();

    यदि (nfa_regcomp_start(अभिव्यक्ती, re_flags) == FAIL)
	प्रत्यागम रिक्त;

    // Build postfix form of the regexp. Needed to build the NFA
    // (and count its size).
    postfix = re2post();
    यदि (postfix == रिक्त)
	गच्छ fail;	    // Cascaded (syntax?) error

    /*
     * In order to build the NFA, we parse the input regexp twice:
     * 1. first pass to count size (so we can allocate space)
     * 2. second to emit code
     */
#ifdef ENABLE_LOG
    {
	FILE *f = fopen(NFA_REGEXP_RUN_LOG, "a");

	यदि (f != रिक्त)
	{
	    fprintf(f, "\n*****************************\n\n\n\n\tCompiling regexp \"%s\"... hold on !\n", अभिव्यक्ती);
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
    prog_size = offsetof(nfa_regprog_T, state) + परिमाण(nfa_state_T) * nstate;
    prog = alloc(prog_size);
    यदि (prog == रिक्त)
	गच्छ fail;
    state_ptr = prog->state;
    prog->re_in_use = FALSE;
    prog->listbuf[0] = रिक्त;
    prog->listbuf[1] = रिक्त;

    /*
     * PASS 2
     * Build the NFA
     */
    prog->start = post2nfa(postfix, post_ptr, FALSE);
    यदि (prog->start == रिक्त)
	गच्छ fail;

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
    nfa_postfix_dump(अभिव्यक्ती, OK);
    nfa_dump(prog);
#endif
#ifdef FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    prog->reghasz = re_has_z;
#endif
    prog->pattern = vim_strsave(अभिव्यक्ती);
#ifdef DEBUG
    nfa_regengine.अभिव्यक्ती = रिक्त;
#endif

out:
    // The postfix list and fragment stack are reused by the next compilation
    // (and freed in free_regexp_stuff()), but drop ones that grew large for a
    // big pattern to avoid holding much memory.
    यदि (post_start_len > NFA_POSTFIX_KEEP)
    {
	VIM_CLEAR(post_start);
	post_start_len = 0;
    }
    post_ptr = post_end = रिक्त;
    यदि (nfa_चळत_len > NFA_चळत_KEEP)
    {
	VIM_CLEAR(nfa_चळत);
	nfa_चळत_len = 0;
    }
    state_ptr = रिक्त;
    प्रत्यागम (regprog_T *)prog;

fail:
    VIM_CLEAR(prog);
#ifdef ENABLE_LOG
    nfa_postfix_dump(अभिव्यक्ती, FAIL);
#endif
#ifdef DEBUG
    nfa_regengine.अभिव्यक्ती = रिक्त;
#endif
    गच्छ out;
}

/*
 * Free a compiled regexp program, returned by nfa_regcomp().
 */
    स्थिर शून्य
nfa_regfree(regprog_T *prog)
{
    यदि (prog == रिक्त)
	प्रत्यागम;

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
    स्थिर पूर्णाङ्क
nfa_regexec_nl(
    regmatch_T	*rmp,
    char_u	*ओळ,	// string to match against
    colnr_T	col,	// column to start looking for match
    पूर्णाङ्क		ओळ_lbr)
{
    rex.reg_match = rmp;
    rex.reg_mmatch = रिक्त;
    rex.reg_maxline = 0;
    rex.reg_ओळ_lbr = ओळ_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = रिक्त;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;
    प्रत्यागम nfa_regexec_both(ओळ, col, रिक्त);
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
    स्थिर long
nfa_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	col,		// column to start looking for match
    पूर्णाङ्क		*timed_out)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    प्रत्यागम nfa_regexec_both(रिक्त, col, timed_out);
}

#ifdef DEBUG
# undef ENABLE_LOG
#endif
