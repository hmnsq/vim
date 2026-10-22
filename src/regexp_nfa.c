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

എണ്ണൽ
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
സ്ഥിതം സ്ഥിരം പൂർണ്ണസംഖ്യ nfa_classcodes[] = {
    NFA_ANY, NFA_IDENT, NFA_SIDENT, NFA_KWORD,NFA_SKWORD,
    NFA_FNAME, NFA_SFNAME, NFA_PRINT, NFA_SPRINT,
    NFA_WHITE, NFA_NWHITE, NFA_DIGIT, NFA_NDIGIT,
    NFA_HEX, NFA_NHEX, NFA_OCTAL, NFA_NOCTAL,
    NFA_WORD, NFA_NWORD, NFA_HEAD, NFA_NHEAD,
    NFA_ALPHA, NFA_NALPHA, NFA_LOWER, NFA_NLOWER,
    NFA_UPPER, NFA_NUPPER
};

// Variables only used in nfa_regcomp() and descendants.
സ്ഥിതം പൂർണ്ണസംഖ്യ nfa_re_flags; // re_flags passed to nfa_regcomp()
സ്ഥിതം പൂർണ്ണസംഖ്യ *post_start;  // holds the postfix form of r.e.
സ്ഥിതം പൂർണ്ണസംഖ്യ post_start_len;  // size of allocated post_start (in ints)
സ്ഥിതം പൂർണ്ണസംഖ്യ *post_end;
സ്ഥിതം പൂർണ്ണസംഖ്യ *post_ptr;
സ്ഥിതം പൂർണ്ണസംഖ്യ nfa_reg_parse_depth;	// nesting depth in nfa_reg()

// The postfix list (post_start) and fragment stack (nfa_stack) are reused
// across compilations; a buffer that grew past these sizes for a big pattern
// is freed afterwards instead of being kept, to avoid holding much memory.
#define NFA_POSTFIX_KEEP	10000	// number of ints (~40 Kbyte)
#define NFA_അട്ടി_KEEP		4000	// number of Frag_T (~64 Kbyte)

// Set when the pattern should use the NFA engine.
// E.g. [[:upper:]] only allows 8bit characters for BT engine,
// while NFA engine handles multibyte characters correctly.
സ്ഥിതം പൂർണ്ണസംഖ്യ wants_nfa;

സ്ഥിതം പൂർണ്ണസംഖ്യ nstate;	// Number of states in the NFA.
സ്ഥിതം പൂർണ്ണസംഖ്യ istate;	// Index in the state vector, used in alloc_state()

// If not NULL match must end at this position
സ്ഥിതം save_se_T *nfa_endp = ശൂന്യ;

// 0 for first call to nfa_regmatch(), 1 for recursive call.
സ്ഥിതം പൂർണ്ണസംഖ്യ nfa_ll_index = 0;

സ്ഥിതം പൂർണ്ണസംഖ്യ realloc_post_list(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ nfa_reg(പൂർണ്ണസംഖ്യ paren);
#ifdef DEBUG
സ്ഥിതം ശൂന്യം nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent);
#endif
സ്ഥിതം പൂർണ്ണസംഖ്യ match_follows(nfa_state_T *startstate, പൂർണ്ണസംഖ്യ depth);
സ്ഥിതം പൂർണ്ണസംഖ്യ failure_chance(nfa_state_T *state, പൂർണ്ണസംഖ്യ depth);

// helper functions used when doing re2post() ... regatom() parsing
#define EMIT(c)	ചെയ്യുക {				\
		    എങ്കിൽ (post_ptr >= post_end && realloc_post_list() == FAIL) \
			തിരികെ FAIL;		\
		    *post_ptr++ = c;		\
		} ആകുമ്പോൾ (0)

/*
 * Initialize internal variables before NFA compilation.
 * Return OK on success, FAIL otherwise.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
nfa_regcomp_start(
    char_u	*വ്യഞ്ജകം,
    പൂർണ്ണസംഖ്യ		re_flags)	    // see vim_regcomp()
{
    size_t	postfix_size;
    പൂർണ്ണസംഖ്യ		nstate_max;

    nstate = 0;
    istate = 0;
    // A reasonable estimation for maximum size
    nstate_max = (പൂർണ്ണസംഖ്യ)(STRLEN(വ്യഞ്ജകം) + 1) * 25;

    // Some items blow up in size, such as [A-z].  Add more space for that.
    // When it is still not enough realloc_post_list() will be used.
    nstate_max += 1000;

    // Size for postfix representation of expr.
    postfix_size = വലുപ്പം(പൂർണ്ണസംഖ്യ) * nstate_max;

    // Reuse the postfix buffer across compilations, only growing it when the
    // estimate exceeds the current size; it is freed in free_regexp_stuff().
    എങ്കിൽ (post_start == ശൂന്യ || post_start_len < nstate_max)
    {
	vim_free(post_start);
	post_start = alloc(postfix_size);
	എങ്കിൽ (post_start == ശൂന്യ)
	{
	    post_start_len = 0;
	    തിരികെ FAIL;
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
    regcomp_start(വ്യഞ്ജകം, re_flags);

    തിരികെ OK;
}

/*
 * Figure out if the NFA state list starts with an anchor, must match at start
 * of the line.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
nfa_get_reganch(nfa_state_T *start, പൂർണ്ണസംഖ്യ depth)
{
    nfa_state_T *p = start;

    എങ്കിൽ (depth > 4)
	തിരികെ 0;

    ആകുമ്പോൾ (p != ശൂന്യ)
    {
	തിരഞ്ഞെടുപ്പ് (p->c)
	{
	    അവസ്ഥ NFA_BOL:
	    അവസ്ഥ NFA_BOF:
		തിരികെ 1; // yes!

	    അവസ്ഥ NFA_ZSTART:
	    അവസ്ഥ NFA_ZEND:
	    അവസ്ഥ NFA_CURSOR:
	    അവസ്ഥ NFA_VISUAL:

	    അവസ്ഥ NFA_MOPEN:
	    അവസ്ഥ NFA_MOPEN1:
	    അവസ്ഥ NFA_MOPEN2:
	    അവസ്ഥ NFA_MOPEN3:
	    അവസ്ഥ NFA_MOPEN4:
	    അവസ്ഥ NFA_MOPEN5:
	    അവസ്ഥ NFA_MOPEN6:
	    അവസ്ഥ NFA_MOPEN7:
	    അവസ്ഥ NFA_MOPEN8:
	    അവസ്ഥ NFA_MOPEN9:
	    അവസ്ഥ NFA_NOPEN:
#ifdef FEAT_SYN_HL
	    അവസ്ഥ NFA_ZOPEN:
	    അവസ്ഥ NFA_ZOPEN1:
	    അവസ്ഥ NFA_ZOPEN2:
	    അവസ്ഥ NFA_ZOPEN3:
	    അവസ്ഥ NFA_ZOPEN4:
	    അവസ്ഥ NFA_ZOPEN5:
	    അവസ്ഥ NFA_ZOPEN6:
	    അവസ്ഥ NFA_ZOPEN7:
	    അവസ്ഥ NFA_ZOPEN8:
	    അവസ്ഥ NFA_ZOPEN9:
#endif
		p = p->out;
		നിർത്തുക;

	    അവസ്ഥ NFA_SPLIT:
		തിരികെ nfa_get_reganch(p->out, depth + 1)
				       && nfa_get_reganch(p->out1, depth + 1);

	    സ്ഥിരസ്ഥിതി:
		തിരികെ 0; // noooo
	}
    }
    തിരികെ 0;
}

/*
 * Figure out if the NFA state list starts with a character which must match
 * at start of the match.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
nfa_get_regstart(nfa_state_T *start, പൂർണ്ണസംഖ്യ depth)
{
    nfa_state_T *p = start;

    എങ്കിൽ (depth > 4)
	തിരികെ 0;

    ആകുമ്പോൾ (p != ശൂന്യ)
    {
	തിരഞ്ഞെടുപ്പ് (p->c)
	{
	    // all kinds of zero-width matches
	    അവസ്ഥ NFA_BOL:
	    അവസ്ഥ NFA_BOF:
	    അവസ്ഥ NFA_BOW:
	    അവസ്ഥ NFA_EOW:
	    അവസ്ഥ NFA_ZSTART:
	    അവസ്ഥ NFA_ZEND:
	    അവസ്ഥ NFA_CURSOR:
	    അവസ്ഥ NFA_VISUAL:
	    അവസ്ഥ NFA_LNUM:
	    അവസ്ഥ NFA_LNUM_GT:
	    അവസ്ഥ NFA_LNUM_LT:
	    അവസ്ഥ NFA_COL:
	    അവസ്ഥ NFA_COL_GT:
	    അവസ്ഥ NFA_COL_LT:
	    അവസ്ഥ NFA_VCOL:
	    അവസ്ഥ NFA_VCOL_GT:
	    അവസ്ഥ NFA_VCOL_LT:
	    അവസ്ഥ NFA_MARK:
	    അവസ്ഥ NFA_MARK_GT:
	    അവസ്ഥ NFA_MARK_LT:

	    അവസ്ഥ NFA_MOPEN:
	    അവസ്ഥ NFA_MOPEN1:
	    അവസ്ഥ NFA_MOPEN2:
	    അവസ്ഥ NFA_MOPEN3:
	    അവസ്ഥ NFA_MOPEN4:
	    അവസ്ഥ NFA_MOPEN5:
	    അവസ്ഥ NFA_MOPEN6:
	    അവസ്ഥ NFA_MOPEN7:
	    അവസ്ഥ NFA_MOPEN8:
	    അവസ്ഥ NFA_MOPEN9:
	    അവസ്ഥ NFA_NOPEN:
#ifdef FEAT_SYN_HL
	    അവസ്ഥ NFA_ZOPEN:
	    അവസ്ഥ NFA_ZOPEN1:
	    അവസ്ഥ NFA_ZOPEN2:
	    അവസ്ഥ NFA_ZOPEN3:
	    അവസ്ഥ NFA_ZOPEN4:
	    അവസ്ഥ NFA_ZOPEN5:
	    അവസ്ഥ NFA_ZOPEN6:
	    അവസ്ഥ NFA_ZOPEN7:
	    അവസ്ഥ NFA_ZOPEN8:
	    അവസ്ഥ NFA_ZOPEN9:
#endif
		p = p->out;
		നിർത്തുക;

	    അവസ്ഥ NFA_SPLIT:
	    {
		പൂർണ്ണസംഖ്യ c1 = nfa_get_regstart(p->out, depth + 1);
		പൂർണ്ണസംഖ്യ c2 = nfa_get_regstart(p->out1, depth + 1);

		എങ്കിൽ (c1 == c2)
		    തിരികെ c1; // yes!
		തിരികെ 0;
	    }

	    സ്ഥിരസ്ഥിതി:
		എങ്കിൽ (p->c > 0)
		    തിരികെ p->c; // yes!
		തിരികെ 0;
	}
    }
    തിരികെ 0;
}

/*
 * Figure out if the NFA state list contains just literal text and nothing
 * else.  If so return a string in allocated memory with what must match after
 * regstart.  Otherwise return NULL.
 */
    സ്ഥിതം char_u *
nfa_get_match_text(nfa_state_T *start)
{
    nfa_state_T *p = start;
    പൂർണ്ണസംഖ്യ		len = 0;
    char_u	*ret;
    char_u	*s;

    എങ്കിൽ (p->c != NFA_MOPEN)
	തിരികെ ശൂന്യ; // just in case
    p = p->out;
    ആകുമ്പോൾ (p->c > 0)
    {
	len += MB_CHAR2LEN(p->c);
	p = p->out;
    }
    എങ്കിൽ (p->c != NFA_MCLOSE || p->out->c != NFA_MATCH)
	തിരികെ ശൂന്യ;

    ret = alloc(len);
    എങ്കിൽ (ret == ശൂന്യ)
	തിരികെ ശൂന്യ;

    p = start->out->out; // skip first char, it goes into regstart
    s = ret;
    ആകുമ്പോൾ (p->c > 0)
    {
	എങ്കിൽ (has_mbyte)
	    s += (*mb_char2bytes)(p->c, s);
	അല്ലെങ്കിൽ
	    *s++ = p->c;
	p = p->out;
    }
    *s = NUL;
    തിരികെ ret;
}

/*
 * Allocate more space for post_start.  Called when
 * running above the estimated number of states.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
realloc_post_list(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ   nstate_max = (പൂർണ്ണസംഖ്യ)(post_end - post_start);
    പൂർണ്ണസംഖ്യ   new_max;
    പൂർണ്ണസംഖ്യ   *new_start;
    പൂർണ്ണസംഖ്യ	  *old_start;

    // For weird patterns the number of states can be very high. Increasing by
    // 50% seems a reasonable compromise between memory use and speed.
    new_max = nstate_max * 3 / 2;
    new_start = ALLOC_MULT(പൂർണ്ണസംഖ്യ, new_max);
    എങ്കിൽ (new_start == ശൂന്യ)
	തിരികെ FAIL;
    mch_memmove(new_start, post_start, nstate_max * വലുപ്പം(പൂർണ്ണസംഖ്യ));
    old_start = post_start;
    post_start = new_start;
    post_start_len = new_max;
    post_ptr = new_start + (post_ptr - old_start);
    post_end = post_start + new_max;
    vim_free(old_start);
    തിരികെ OK;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
nfa_recognize_char_class(char_u *start, char_u *end, പൂർണ്ണസംഖ്യ extra_newl)
{
    പൂർണ്ണസംഖ്യ		newl;
    പൂർണ്ണസംഖ്യ		config = get_char_class_bits(start, end, &newl);

    എങ്കിൽ (config < 0)
	തിരികെ FAIL;

    എങ്കിൽ (extra_newl == TRUE)
	newl = TRUE;

    എങ്കിൽ (newl == TRUE)
	extra_newl = NFA_ADD_NL;

    തിരഞ്ഞെടുപ്പ് (config)
    {
	അവസ്ഥ CLASS_o9:
	    തിരികെ extra_newl + NFA_DIGIT;
	അവസ്ഥ CLASS_not |  CLASS_o9:
	    തിരികെ extra_newl + NFA_NDIGIT;
	അവസ്ഥ CLASS_af | CLASS_AF | CLASS_o9:
	    തിരികെ extra_newl + NFA_HEX;
	അവസ്ഥ CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    തിരികെ extra_newl + NFA_NHEX;
	അവസ്ഥ CLASS_o7:
	    തിരികെ extra_newl + NFA_OCTAL;
	അവസ്ഥ CLASS_not | CLASS_o7:
	    തിരികെ extra_newl + NFA_NOCTAL;
	അവസ്ഥ CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    തിരികെ extra_newl + NFA_WORD;
	അവസ്ഥ CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    തിരികെ extra_newl + NFA_NWORD;
	അവസ്ഥ CLASS_az | CLASS_AZ | CLASS_underscore:
	    തിരികെ extra_newl + NFA_HEAD;
	അവസ്ഥ CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    തിരികെ extra_newl + NFA_NHEAD;
	അവസ്ഥ CLASS_az | CLASS_AZ:
	    തിരികെ extra_newl + NFA_ALPHA;
	അവസ്ഥ CLASS_not | CLASS_az | CLASS_AZ:
	    തിരികെ extra_newl + NFA_NALPHA;
	അവസ്ഥ CLASS_az:
	   തിരികെ extra_newl + NFA_LOWER_IC;
	അവസ്ഥ CLASS_not | CLASS_az:
	    തിരികെ extra_newl + NFA_NLOWER_IC;
	അവസ്ഥ CLASS_AZ:
	    തിരികെ extra_newl + NFA_UPPER_IC;
	അവസ്ഥ CLASS_not | CLASS_AZ:
	    തിരികെ extra_newl + NFA_NUPPER_IC;
    }
    തിരികെ FAIL;
}

/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * Emits bytes in postfix notation: 'a,b,NFA_OR,c,NFA_OR' is
 * equivalent to 'a OR b OR c'
 *
 * NOTE! When changing this function, also update reg_equi_class()
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
nfa_emit_equi_class(പൂർണ്ണസംഖ്യ c)
{
#define EMIT2(c)    EMIT(c); EMIT(NFA_CONCAT);

    എങ്കിൽ (enc_utf8 || STRCMP(p_enc, "latin1") == 0
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
	തിരഞ്ഞെടുപ്പ് (c)
	{
	    അവസ്ഥ 'A': അവസ്ഥ A_grave: അവസ്ഥ A_acute: അവസ്ഥ A_circumflex:
	    അവസ്ഥ A_virguilla: അവസ്ഥ A_diaeresis: അവസ്ഥ A_ring:
	    അവസ്ഥ 0x100: അവസ്ഥ 0x102: അവസ്ഥ 0x104: അവസ്ഥ 0x1cd:
	    അവസ്ഥ 0x1de: അവസ്ഥ 0x1e0: അവസ്ഥ 0x1fa: അവസ്ഥ 0x200:
	    അവസ്ഥ 0x202: അവസ്ഥ 0x226: അവസ്ഥ 0x23a: അവസ്ഥ 0x1e00:
	    അവസ്ഥ 0x1ea0: അവസ്ഥ 0x1ea2: അവസ്ഥ 0x1ea4: അവസ്ഥ 0x1ea6:
	    അവസ്ഥ 0x1ea8: അവസ്ഥ 0x1eaa: അവസ്ഥ 0x1eac: അവസ്ഥ 0x1eae:
	    അവസ്ഥ 0x1eb0: അവസ്ഥ 0x1eb2: അവസ്ഥ 0x1eb4: അവസ്ഥ 0x1eb6:
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
		    തിരികെ OK;

	    അവസ്ഥ 'B': അവസ്ഥ 0x181: അവസ്ഥ 0x243: അവസ്ഥ 0x1e02:
	    അവസ്ഥ 0x1e04: അവസ്ഥ 0x1e06:
		    EMIT2('B')
		    EMIT2(0x181) EMIT2(0x243) EMIT2(0x1e02)
		    EMIT2(0x1e04) EMIT2(0x1e06)
		    തിരികെ OK;

	    അവസ്ഥ 'C': അവസ്ഥ C_cedilla: അവസ്ഥ 0x106: അവസ്ഥ 0x108:
	    അവസ്ഥ 0x10a: അവസ്ഥ 0x10c: അവസ്ഥ 0x187: അവസ്ഥ 0x23b:
	    അവസ്ഥ 0x1e08: അവസ്ഥ 0xa792:
		    EMIT2('C') EMIT2(C_cedilla)
		    EMIT2(0x106) EMIT2(0x108) EMIT2(0x10a)
		    EMIT2(0x10c) EMIT2(0x187) EMIT2(0x23b)
		    EMIT2(0x1e08) EMIT2(0xa792)
		    തിരികെ OK;

	    അവസ്ഥ 'D': അവസ്ഥ 0x10e: അവസ്ഥ 0x110: അവസ്ഥ 0x18a:
	    അവസ്ഥ 0x1e0a: അവസ്ഥ 0x1e0c: അവസ്ഥ 0x1e0e: അവസ്ഥ 0x1e10:
	    അവസ്ഥ 0x1e12:
		    EMIT2('D') EMIT2(0x10e) EMIT2(0x110) EMIT2(0x18a)
		    EMIT2(0x1e0a) EMIT2(0x1e0c) EMIT2(0x1e0e)
		    EMIT2(0x1e10) EMIT2(0x1e12)
		    തിരികെ OK;

	    അവസ്ഥ 'E': അവസ്ഥ E_grave: അവസ്ഥ E_acute: അവസ്ഥ E_circumflex:
	    അവസ്ഥ E_diaeresis: അവസ്ഥ 0x112: അവസ്ഥ 0x114: അവസ്ഥ 0x116:
	    അവസ്ഥ 0x118: അവസ്ഥ 0x11a: അവസ്ഥ 0x204: അവസ്ഥ 0x206:
	    അവസ്ഥ 0x228: അവസ്ഥ 0x246: അവസ്ഥ 0x1e14: അവസ്ഥ 0x1e16:
	    അവസ്ഥ 0x1e18: അവസ്ഥ 0x1e1a: അവസ്ഥ 0x1e1c: അവസ്ഥ 0x1eb8:
	    അവസ്ഥ 0x1eba: അവസ്ഥ 0x1ebc: അവസ്ഥ 0x1ebe: അവസ്ഥ 0x1ec0:
	    അവസ്ഥ 0x1ec2: അവസ്ഥ 0x1ec4: അവസ്ഥ 0x1ec6:
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
		    തിരികെ OK;

	    അവസ്ഥ 'F': അവസ്ഥ 0x191: അവസ്ഥ 0x1e1e: അവസ്ഥ 0xa798:
		    EMIT2('F') EMIT2(0x191) EMIT2(0x1e1e) EMIT2(0xa798)
		    തിരികെ OK;

	    അവസ്ഥ 'G': അവസ്ഥ 0x11c: അവസ്ഥ 0x11e: അവസ്ഥ 0x120:
	    അവസ്ഥ 0x122: അവസ്ഥ 0x193: അവസ്ഥ 0x1e4: അവസ്ഥ 0x1e6:
	    അവസ്ഥ 0x1f4: അവസ്ഥ 0x1e20: അവസ്ഥ 0xa7a0:
		    EMIT2('G') EMIT2(0x11c) EMIT2(0x11e) EMIT2(0x120)
		    EMIT2(0x122) EMIT2(0x193) EMIT2(0x1e4)
		    EMIT2(0x1e6) EMIT2(0x1f4) EMIT2(0x1e20)
		    EMIT2(0xa7a0)
		    തിരികെ OK;

	    അവസ്ഥ 'H': അവസ്ഥ 0x124: അവസ്ഥ 0x126: അവസ്ഥ 0x21e:
	    അവസ്ഥ 0x1e22: അവസ്ഥ 0x1e24: അവസ്ഥ 0x1e26: അവസ്ഥ 0x1e28:
	    അവസ്ഥ 0x1e2a: അവസ്ഥ 0x2c67:
		    EMIT2('H') EMIT2(0x124) EMIT2(0x126) EMIT2(0x21e)
		    EMIT2(0x1e22) EMIT2(0x1e24) EMIT2(0x1e26)
		    EMIT2(0x1e28) EMIT2(0x1e2a) EMIT2(0x2c67)
		    തിരികെ OK;

	    അവസ്ഥ 'I': അവസ്ഥ I_grave: അവസ്ഥ I_acute: അവസ്ഥ I_circumflex:
	    അവസ്ഥ I_diaeresis: അവസ്ഥ 0x128: അവസ്ഥ 0x12a: അവസ്ഥ 0x12c:
	    അവസ്ഥ 0x12e: അവസ്ഥ 0x130: അവസ്ഥ 0x197: അവസ്ഥ 0x1cf:
	    അവസ്ഥ 0x208: അവസ്ഥ 0x20a: അവസ്ഥ 0x1e2c: അവസ്ഥ 0x1e2e:
	    അവസ്ഥ 0x1ec8: അവസ്ഥ 0x1eca:
		    EMIT2('I') EMIT2(I_grave) EMIT2(I_acute)
		    EMIT2(I_circumflex) EMIT2(I_diaeresis)
		    EMIT2(0x128) EMIT2(0x12a) EMIT2(0x12c)
		    EMIT2(0x12e) EMIT2(0x130) EMIT2(0x197)
		    EMIT2(0x1cf) EMIT2(0x208) EMIT2(0x20a)
		    EMIT2(0x1e2c) EMIT2(0x1e2e) EMIT2(0x1ec8)
		    EMIT2(0x1eca)
		    തിരികെ OK;

	    അവസ്ഥ 'J': അവസ്ഥ 0x134: അവസ്ഥ 0x248:
		    EMIT2('J') EMIT2(0x134) EMIT2(0x248)
		    തിരികെ OK;

	    അവസ്ഥ 'K': അവസ്ഥ 0x136: അവസ്ഥ 0x198: അവസ്ഥ 0x1e8: അവസ്ഥ 0x1e30:
	    അവസ്ഥ 0x1e32: അവസ്ഥ 0x1e34: അവസ്ഥ 0x2c69: അവസ്ഥ 0xa740:
		    EMIT2('K') EMIT2(0x136) EMIT2(0x198) EMIT2(0x1e8)
		    EMIT2(0x1e30) EMIT2(0x1e32) EMIT2(0x1e34)
		    EMIT2(0x2c69) EMIT2(0xa740)
		    തിരികെ OK;

	    അവസ്ഥ 'L': അവസ്ഥ 0x139: അവസ്ഥ 0x13b: അവസ്ഥ 0x13d:
	    അവസ്ഥ 0x13f: അവസ്ഥ 0x141: അവസ്ഥ 0x23d: അവസ്ഥ 0x1e36:
	    അവസ്ഥ 0x1e38: അവസ്ഥ 0x1e3a: അവസ്ഥ 0x1e3c: അവസ്ഥ 0x2c60:
		    EMIT2('L') EMIT2(0x139) EMIT2(0x13b)
		    EMIT2(0x13d) EMIT2(0x13f) EMIT2(0x141)
		    EMIT2(0x23d) EMIT2(0x1e36) EMIT2(0x1e38)
		    EMIT2(0x1e3a) EMIT2(0x1e3c) EMIT2(0x2c60)
		    തിരികെ OK;

	    അവസ്ഥ 'M': അവസ്ഥ 0x1e3e: അവസ്ഥ 0x1e40: അവസ്ഥ 0x1e42:
		    EMIT2('M') EMIT2(0x1e3e) EMIT2(0x1e40)
		    EMIT2(0x1e42)
		    തിരികെ OK;

	    അവസ്ഥ 'N': അവസ്ഥ N_virguilla:
	    അവസ്ഥ 0x143: അവസ്ഥ 0x145: അവസ്ഥ 0x147: അവസ്ഥ 0x1f8:
	    അവസ്ഥ 0x1e44: അവസ്ഥ 0x1e46: അവസ്ഥ 0x1e48: അവസ്ഥ 0x1e4a:
	    അവസ്ഥ 0xa7a4:
		    EMIT2('N') EMIT2(N_virguilla)
		    EMIT2(0x143) EMIT2(0x145) EMIT2(0x147)
		    EMIT2(0x1f8) EMIT2(0x1e44) EMIT2(0x1e46)
		    EMIT2(0x1e48) EMIT2(0x1e4a) EMIT2(0xa7a4)
		    തിരികെ OK;

	    അവസ്ഥ 'O': അവസ്ഥ O_grave: അവസ്ഥ O_acute: അവസ്ഥ O_circumflex:
	    അവസ്ഥ O_virguilla: അവസ്ഥ O_diaeresis: അവസ്ഥ O_slash:
	    അവസ്ഥ 0x14c: അവസ്ഥ 0x14e: അവസ്ഥ 0x150: അവസ്ഥ 0x19f:
	    അവസ്ഥ 0x1a0: അവസ്ഥ 0x1d1: അവസ്ഥ 0x1ea: അവസ്ഥ 0x1ec:
	    അവസ്ഥ 0x1fe: അവസ്ഥ 0x20c: അവസ്ഥ 0x20e: അവസ്ഥ 0x22a:
	    അവസ്ഥ 0x22c: അവസ്ഥ 0x22e: അവസ്ഥ 0x230: അവസ്ഥ 0x1e4c:
	    അവസ്ഥ 0x1e4e: അവസ്ഥ 0x1e50: അവസ്ഥ 0x1e52: അവസ്ഥ 0x1ecc:
	    അവസ്ഥ 0x1ece: അവസ്ഥ 0x1ed0: അവസ്ഥ 0x1ed2: അവസ്ഥ 0x1ed4:
	    അവസ്ഥ 0x1ed6: അവസ്ഥ 0x1ed8: അവസ്ഥ 0x1eda: അവസ്ഥ 0x1edc:
	    അവസ്ഥ 0x1ede: അവസ്ഥ 0x1ee0: അവസ്ഥ 0x1ee2:
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
		    തിരികെ OK;

	    അവസ്ഥ 'P': അവസ്ഥ 0x1a4: അവസ്ഥ 0x1e54: അവസ്ഥ 0x1e56: അവസ്ഥ 0x2c63:
		    EMIT2('P') EMIT2(0x1a4) EMIT2(0x1e54) EMIT2(0x1e56)
		    EMIT2(0x2c63)
		    തിരികെ OK;

	    അവസ്ഥ 'Q': അവസ്ഥ 0x24a:
		    EMIT2('Q') EMIT2(0x24a)
		    തിരികെ OK;

	    അവസ്ഥ 'R': അവസ്ഥ 0x154: അവസ്ഥ 0x156: അവസ്ഥ 0x158: അവസ്ഥ 0x210:
	    അവസ്ഥ 0x212: അവസ്ഥ 0x24c: അവസ്ഥ 0x1e58: അവസ്ഥ 0x1e5a:
	    അവസ്ഥ 0x1e5c: അവസ്ഥ 0x1e5e: അവസ്ഥ 0x2c64: അവസ്ഥ 0xa7a6:
		    EMIT2('R') EMIT2(0x154) EMIT2(0x156) EMIT2(0x158)
		    EMIT2(0x210) EMIT2(0x212) EMIT2(0x24c) EMIT2(0x1e58)
		    EMIT2(0x1e5a) EMIT2(0x1e5c) EMIT2(0x1e5e) EMIT2(0x2c64)
		    EMIT2(0xa7a6)
		    തിരികെ OK;

	    അവസ്ഥ 'S': അവസ്ഥ 0x15a: അവസ്ഥ 0x15c: അവസ്ഥ 0x15e: അവസ്ഥ 0x160:
	    അവസ്ഥ 0x218: അവസ്ഥ 0x1e60: അവസ്ഥ 0x1e62: അവസ്ഥ 0x1e64:
	    അവസ്ഥ 0x1e66: അവസ്ഥ 0x1e68: അവസ്ഥ 0x2c7e: അവസ്ഥ 0xa7a8:
		    EMIT2('S') EMIT2(0x15a) EMIT2(0x15c) EMIT2(0x15e)
		    EMIT2(0x160) EMIT2(0x218) EMIT2(0x1e60) EMIT2(0x1e62)
		    EMIT2(0x1e64) EMIT2(0x1e66) EMIT2(0x1e68) EMIT2(0x2c7e)
		    EMIT2(0xa7a8)
		    തിരികെ OK;

	    അവസ്ഥ 'T': അവസ്ഥ 0x162: അവസ്ഥ 0x164: അവസ്ഥ 0x166: അവസ്ഥ 0x1ac:
	    അവസ്ഥ 0x1ae: അവസ്ഥ 0x21a: അവസ്ഥ 0x23e: അവസ്ഥ 0x1e6a: അവസ്ഥ 0x1e6c:
	    അവസ്ഥ 0x1e6e: അവസ്ഥ 0x1e70:
		    EMIT2('T') EMIT2(0x162) EMIT2(0x164) EMIT2(0x166)
		    EMIT2(0x1ac) EMIT2(0x1ae) EMIT2(0x23e) EMIT2(0x21a)
		    EMIT2(0x1e6a) EMIT2(0x1e6c) EMIT2(0x1e6e) EMIT2(0x1e70)
		    തിരികെ OK;

	    അവസ്ഥ 'U': അവസ്ഥ U_grave: അവസ്ഥ U_acute: അവസ്ഥ U_diaeresis:
	    അവസ്ഥ U_circumflex: അവസ്ഥ 0x168: അവസ്ഥ 0x16a: അവസ്ഥ 0x16c:
	    അവസ്ഥ 0x16e: അവസ്ഥ 0x170: അവസ്ഥ 0x172: അവസ്ഥ 0x1af:
	    അവസ്ഥ 0x1d3: അവസ്ഥ 0x1d5: അവസ്ഥ 0x1d7: അവസ്ഥ 0x1d9:
	    അവസ്ഥ 0x1db: അവസ്ഥ 0x214: അവസ്ഥ 0x216: അവസ്ഥ 0x244:
	    അവസ്ഥ 0x1e72: അവസ്ഥ 0x1e74: അവസ്ഥ 0x1e76: അവസ്ഥ 0x1e78:
	    അവസ്ഥ 0x1e7a: അവസ്ഥ 0x1ee4: അവസ്ഥ 0x1ee6: അവസ്ഥ 0x1ee8:
	    അവസ്ഥ 0x1eea: അവസ്ഥ 0x1eec: അവസ്ഥ 0x1eee: അവസ്ഥ 0x1ef0:
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
		    തിരികെ OK;

	    അവസ്ഥ 'V': അവസ്ഥ 0x1b2: അവസ്ഥ 0x1e7c: അവസ്ഥ 0x1e7e:
		    EMIT2('V') EMIT2(0x1b2) EMIT2(0x1e7c) EMIT2(0x1e7e)
		    തിരികെ OK;

	    അവസ്ഥ 'W': അവസ്ഥ 0x174: അവസ്ഥ 0x1e80: അവസ്ഥ 0x1e82: അവസ്ഥ 0x1e84:
	    അവസ്ഥ 0x1e86: അവസ്ഥ 0x1e88:
		    EMIT2('W') EMIT2(0x174) EMIT2(0x1e80) EMIT2(0x1e82)
		    EMIT2(0x1e84) EMIT2(0x1e86) EMIT2(0x1e88)
		    തിരികെ OK;

	    അവസ്ഥ 'X': അവസ്ഥ 0x1e8a: അവസ്ഥ 0x1e8c:
		    EMIT2('X') EMIT2(0x1e8a) EMIT2(0x1e8c)
		    തിരികെ OK;

	    അവസ്ഥ 'Y': അവസ്ഥ Y_acute: അവസ്ഥ 0x176: അവസ്ഥ 0x178:
	    അവസ്ഥ 0x1b3: അവസ്ഥ 0x232: അവസ്ഥ 0x24e: അവസ്ഥ 0x1e8e:
	    അവസ്ഥ 0x1ef2: അവസ്ഥ 0x1ef4: അവസ്ഥ 0x1ef6: അവസ്ഥ 0x1ef8:
		    EMIT2('Y') EMIT2(Y_acute)
		    EMIT2(0x176) EMIT2(0x178) EMIT2(0x1b3)
		    EMIT2(0x232) EMIT2(0x24e) EMIT2(0x1e8e)
		    EMIT2(0x1ef2) EMIT2(0x1ef4) EMIT2(0x1ef6)
		    EMIT2(0x1ef8)
		    തിരികെ OK;

	    അവസ്ഥ 'Z': അവസ്ഥ 0x179: അവസ്ഥ 0x17b: അവസ്ഥ 0x17d:
	    അവസ്ഥ 0x1b5: അവസ്ഥ 0x1e90: അവസ്ഥ 0x1e92: അവസ്ഥ 0x1e94:
	    അവസ്ഥ 0x2c6b:
		    EMIT2('Z') EMIT2(0x179) EMIT2(0x17b) EMIT2(0x17d)
		    EMIT2(0x1b5) EMIT2(0x1e90) EMIT2(0x1e92)
		    EMIT2(0x1e94) EMIT2(0x2c6b)
		    തിരികെ OK;

	    അവസ്ഥ 'a': അവസ്ഥ a_grave: അവസ്ഥ a_acute: അവസ്ഥ a_circumflex:
	    അവസ്ഥ a_virguilla: അവസ്ഥ a_diaeresis: അവസ്ഥ a_ring:
	    അവസ്ഥ 0x101: അവസ്ഥ 0x103: അവസ്ഥ 0x105: അവസ്ഥ 0x1ce:
	    അവസ്ഥ 0x1df: അവസ്ഥ 0x1e1: അവസ്ഥ 0x1fb: അവസ്ഥ 0x201:
	    അവസ്ഥ 0x203: അവസ്ഥ 0x227: അവസ്ഥ 0x1d8f: അവസ്ഥ 0x1e01:
	    അവസ്ഥ 0x1e9a: അവസ്ഥ 0x1ea1: അവസ്ഥ 0x1ea3: അവസ്ഥ 0x1ea5:
	    അവസ്ഥ 0x1ea7: അവസ്ഥ 0x1ea9: അവസ്ഥ 0x1eab: അവസ്ഥ 0x1ead:
	    അവസ്ഥ 0x1eaf: അവസ്ഥ 0x1eb1: അവസ്ഥ 0x1eb3: അവസ്ഥ 0x1eb5:
	    അവസ്ഥ 0x1eb7: അവസ്ഥ 0x2c65:
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
		    തിരികെ OK;

	    അവസ്ഥ 'b': അവസ്ഥ 0x180: അവസ്ഥ 0x253: അവസ്ഥ 0x1d6c: അവസ്ഥ 0x1d80:
	    അവസ്ഥ 0x1e03: അവസ്ഥ 0x1e05: അവസ്ഥ 0x1e07:
		    EMIT2('b') EMIT2(0x180) EMIT2(0x253) EMIT2(0x1d6c)
		    EMIT2(0x1d80) EMIT2(0x1e03) EMIT2(0x1e05) EMIT2(0x1e07)
		    തിരികെ OK;

	    അവസ്ഥ 'c': അവസ്ഥ c_cedilla: അവസ്ഥ 0x107: അവസ്ഥ 0x109: അവസ്ഥ 0x10b:
	    അവസ്ഥ 0x10d: അവസ്ഥ 0x188: അവസ്ഥ 0x23c: അവസ്ഥ 0x1e09: അവസ്ഥ 0xa793:
	    അവസ്ഥ 0xa794:
		    EMIT2('c') EMIT2(c_cedilla)
		    EMIT2(0x107) EMIT2(0x109) EMIT2(0x10b)
		    EMIT2(0x10d) EMIT2(0x188) EMIT2(0x23c)
		    EMIT2(0x1e09) EMIT2(0xa793) EMIT2(0xa794)
		    തിരികെ OK;

	    അവസ്ഥ 'd': അവസ്ഥ 0x10f: അവസ്ഥ 0x111: അവസ്ഥ 0x257: അവസ്ഥ 0x1d6d:
	    അവസ്ഥ 0x1d81: അവസ്ഥ 0x1d91: അവസ്ഥ 0x1e0b: അവസ്ഥ 0x1e0d: അവസ്ഥ 0x1e0f:
	    അവസ്ഥ 0x1e11: അവസ്ഥ 0x1e13:
		    EMIT2('d') EMIT2(0x10f) EMIT2(0x111)
		    EMIT2(0x257) EMIT2(0x1d6d) EMIT2(0x1d81)
		    EMIT2(0x1d91) EMIT2(0x1e0b) EMIT2(0x1e0d)
		    EMIT2(0x1e0f) EMIT2(0x1e11) EMIT2(0x1e13)
		    തിരികെ OK;

	    അവസ്ഥ 'e': അവസ്ഥ e_grave: അവസ്ഥ e_acute: അവസ്ഥ e_circumflex:
	    അവസ്ഥ e_diaeresis: അവസ്ഥ 0x113: അവസ്ഥ 0x115: അവസ്ഥ 0x117:
	    അവസ്ഥ 0x119: അവസ്ഥ 0x11b: അവസ്ഥ 0x205: അവസ്ഥ 0x207:
	    അവസ്ഥ 0x229: അവസ്ഥ 0x247: അവസ്ഥ 0x1d92: അവസ്ഥ 0x1e15:
	    അവസ്ഥ 0x1e17: അവസ്ഥ 0x1e19: അവസ്ഥ 0x1e1b: അവസ്ഥ 0x1e1d:
	    അവസ്ഥ 0x1eb9: അവസ്ഥ 0x1ebb: അവസ്ഥ 0x1ebd: അവസ്ഥ 0x1ebf:
	    അവസ്ഥ 0x1ec1: അവസ്ഥ 0x1ec3: അവസ്ഥ 0x1ec5: അവസ്ഥ 0x1ec7:
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
		    തിരികെ OK;

	    അവസ്ഥ 'f': അവസ്ഥ 0x192: അവസ്ഥ 0x1d6e: അവസ്ഥ 0x1d82:
	    അവസ്ഥ 0x1e1f: അവസ്ഥ 0xa799:
		    EMIT2('f') EMIT2(0x192) EMIT2(0x1d6e) EMIT2(0x1d82)
		    EMIT2(0x1e1f) EMIT2(0xa799)
		    തിരികെ OK;

	    അവസ്ഥ 'g': അവസ്ഥ 0x11d: അവസ്ഥ 0x11f: അവസ്ഥ 0x121: അവസ്ഥ 0x123:
	    അവസ്ഥ 0x1e5: അവസ്ഥ 0x1e7: അവസ്ഥ 0x1f5: അവസ്ഥ 0x260: അവസ്ഥ 0x1d83:
	    അവസ്ഥ 0x1e21: അവസ്ഥ 0xa7a1:
		    EMIT2('g') EMIT2(0x11d) EMIT2(0x11f) EMIT2(0x121)
		    EMIT2(0x123) EMIT2(0x1e5) EMIT2(0x1e7)
		    EMIT2(0x1f5) EMIT2(0x260) EMIT2(0x1d83)
		    EMIT2(0x1e21) EMIT2(0xa7a1)
		    തിരികെ OK;

	    അവസ്ഥ 'h': അവസ്ഥ 0x125: അവസ്ഥ 0x127: അവസ്ഥ 0x21f: അവസ്ഥ 0x1e23:
	    അവസ്ഥ 0x1e25: അവസ്ഥ 0x1e27: അവസ്ഥ 0x1e29: അവസ്ഥ 0x1e2b:
	    അവസ്ഥ 0x1e96: അവസ്ഥ 0x2c68: അവസ്ഥ 0xa795:
		    EMIT2('h') EMIT2(0x125) EMIT2(0x127) EMIT2(0x21f)
		    EMIT2(0x1e23) EMIT2(0x1e25) EMIT2(0x1e27)
		    EMIT2(0x1e29) EMIT2(0x1e2b) EMIT2(0x1e96)
		    EMIT2(0x2c68) EMIT2(0xa795)
		    തിരികെ OK;

	    അവസ്ഥ 'i': അവസ്ഥ i_grave: അവസ്ഥ i_acute: അവസ്ഥ i_circumflex:
	    അവസ്ഥ i_diaeresis: അവസ്ഥ 0x129: അവസ്ഥ 0x12b: അവസ്ഥ 0x12d:
	    അവസ്ഥ 0x12f: അവസ്ഥ 0x1d0: അവസ്ഥ 0x209: അവസ്ഥ 0x20b:
	    അവസ്ഥ 0x268: അവസ്ഥ 0x1d96: അവസ്ഥ 0x1e2d: അവസ്ഥ 0x1e2f:
	    അവസ്ഥ 0x1ec9: അവസ്ഥ 0x1ecb:
		    EMIT2('i') EMIT2(i_grave) EMIT2(i_acute)
		    EMIT2(i_circumflex) EMIT2(i_diaeresis)
		    EMIT2(0x129) EMIT2(0x12b) EMIT2(0x12d)
		    EMIT2(0x12f) EMIT2(0x1d0) EMIT2(0x209)
		    EMIT2(0x20b) EMIT2(0x268) EMIT2(0x1d96)
		    EMIT2(0x1e2d) EMIT2(0x1e2f) EMIT2(0x1ec9)
		    EMIT2(0x1ecb)
		    തിരികെ OK;

	    അവസ്ഥ 'j': അവസ്ഥ 0x135: അവസ്ഥ 0x1f0: അവസ്ഥ 0x249:
		    EMIT2('j') EMIT2(0x135) EMIT2(0x1f0) EMIT2(0x249)
		    തിരികെ OK;

	    അവസ്ഥ 'k': അവസ്ഥ 0x137: അവസ്ഥ 0x199: അവസ്ഥ 0x1e9: അവസ്ഥ 0x1d84:
	    അവസ്ഥ 0x1e31: അവസ്ഥ 0x1e33: അവസ്ഥ 0x1e35: അവസ്ഥ 0x2c6a: അവസ്ഥ 0xa741:
		    EMIT2('k') EMIT2(0x137) EMIT2(0x199) EMIT2(0x1e9)
		    EMIT2(0x1d84) EMIT2(0x1e31) EMIT2(0x1e33)
		    EMIT2(0x1e35) EMIT2(0x2c6a) EMIT2(0xa741)
		    തിരികെ OK;

	    അവസ്ഥ 'l': അവസ്ഥ 0x13a: അവസ്ഥ 0x13c: അവസ്ഥ 0x13e: അവസ്ഥ 0x140:
	    അവസ്ഥ 0x142: അവസ്ഥ 0x19a: അവസ്ഥ 0x1e37: അവസ്ഥ 0x1e39: അവസ്ഥ 0x1e3b:
	    അവസ്ഥ 0x1e3d: അവസ്ഥ 0x2c61:
		    EMIT2('l') EMIT2(0x13a) EMIT2(0x13c)
		    EMIT2(0x13e) EMIT2(0x140) EMIT2(0x142)
		    EMIT2(0x19a) EMIT2(0x1e37) EMIT2(0x1e39)
		    EMIT2(0x1e3b) EMIT2(0x1e3d) EMIT2(0x2c61)
		    തിരികെ OK;

	    അവസ്ഥ 'm': അവസ്ഥ 0x1d6f: അവസ്ഥ 0x1e3f: അവസ്ഥ 0x1e41: അവസ്ഥ 0x1e43:
		    EMIT2('m') EMIT2(0x1d6f) EMIT2(0x1e3f)
		    EMIT2(0x1e41) EMIT2(0x1e43)
		    തിരികെ OK;

	    അവസ്ഥ 'n': അവസ്ഥ n_virguilla: അവസ്ഥ 0x144: അവസ്ഥ 0x146: അവസ്ഥ 0x148:
	    അവസ്ഥ 0x149: അവസ്ഥ 0x1f9: അവസ്ഥ 0x1d70: അവസ്ഥ 0x1d87: അവസ്ഥ 0x1e45:
	    അവസ്ഥ 0x1e47: അവസ്ഥ 0x1e49: അവസ്ഥ 0x1e4b: അവസ്ഥ 0xa7a5:
		    EMIT2('n') EMIT2(n_virguilla)
		    EMIT2(0x144) EMIT2(0x146) EMIT2(0x148)
		    EMIT2(0x149) EMIT2(0x1f9) EMIT2(0x1d70)
		    EMIT2(0x1d87) EMIT2(0x1e45) EMIT2(0x1e47)
		    EMIT2(0x1e49) EMIT2(0x1e4b) EMIT2(0xa7a5)
		    തിരികെ OK;

	    അവസ്ഥ 'o': അവസ്ഥ o_grave: അവസ്ഥ o_acute: അവസ്ഥ o_circumflex:
	    അവസ്ഥ o_virguilla: അവസ്ഥ o_diaeresis: അവസ്ഥ o_slash:
	    അവസ്ഥ 0x14d: അവസ്ഥ 0x14f: അവസ്ഥ 0x151: അവസ്ഥ 0x1a1:
	    അവസ്ഥ 0x1d2: അവസ്ഥ 0x1eb: അവസ്ഥ 0x1ed: അവസ്ഥ 0x1ff:
	    അവസ്ഥ 0x20d: അവസ്ഥ 0x20f: അവസ്ഥ 0x22b: അവസ്ഥ 0x22d:
	    അവസ്ഥ 0x22f: അവസ്ഥ 0x231: അവസ്ഥ 0x275: അവസ്ഥ 0x1e4d:
	    അവസ്ഥ 0x1e4f: അവസ്ഥ 0x1e51: അവസ്ഥ 0x1e53: അവസ്ഥ 0x1ecd:
	    അവസ്ഥ 0x1ecf: അവസ്ഥ 0x1ed1: അവസ്ഥ 0x1ed3: അവസ്ഥ 0x1ed5:
	    അവസ്ഥ 0x1ed7: അവസ്ഥ 0x1ed9: അവസ്ഥ 0x1edb: അവസ്ഥ 0x1edd:
	    അവസ്ഥ 0x1edf: അവസ്ഥ 0x1ee1: അവസ്ഥ 0x1ee3:
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
		    തിരികെ OK;

	    അവസ്ഥ 'p': അവസ്ഥ 0x1a5: അവസ്ഥ 0x1d71: അവസ്ഥ 0x1d7d: അവസ്ഥ 0x1d88:
	    അവസ്ഥ 0x1e55: അവസ്ഥ 0x1e57:
		    EMIT2('p') EMIT2(0x1a5) EMIT2(0x1d71) EMIT2(0x1d7d)
		    EMIT2(0x1d88) EMIT2(0x1e55) EMIT2(0x1e57)
		    തിരികെ OK;

	    അവസ്ഥ 'q': അവസ്ഥ 0x24b: അവസ്ഥ 0x2a0:
		    EMIT2('q') EMIT2(0x24b) EMIT2(0x2a0)
		    തിരികെ OK;

	    അവസ്ഥ 'r': അവസ്ഥ 0x155: അവസ്ഥ 0x157: അവസ്ഥ 0x159: അവസ്ഥ 0x211:
	    അവസ്ഥ 0x213: അവസ്ഥ 0x24d: അവസ്ഥ 0x27d: അവസ്ഥ 0x1d72: അവസ്ഥ 0x1d73:
	    അവസ്ഥ 0x1d89: അവസ്ഥ 0x1e59: അവസ്ഥ 0x1e5b: അവസ്ഥ 0x1e5d: അവസ്ഥ 0x1e5f:
	    അവസ്ഥ 0xa7a7:
		    EMIT2('r') EMIT2(0x155) EMIT2(0x157) EMIT2(0x159)
		    EMIT2(0x211) EMIT2(0x213) EMIT2(0x24d) EMIT2(0x27d)
		    EMIT2(0x1d72) EMIT2(0x1d73) EMIT2(0x1d89) EMIT2(0x1e59)
		    EMIT2(0x1e5b) EMIT2(0x1e5d) EMIT2(0x1e5f) EMIT2(0xa7a7)
		    തിരികെ OK;

	    അവസ്ഥ 's': അവസ്ഥ 0x15b: അവസ്ഥ 0x15d: അവസ്ഥ 0x15f: അവസ്ഥ 0x161:
	    അവസ്ഥ 0x219: അവസ്ഥ 0x23f: അവസ്ഥ 0x1d74: അവസ്ഥ 0x1d8a: അവസ്ഥ 0x1e61:
	    അവസ്ഥ 0x1e63: അവസ്ഥ 0x1e65: അവസ്ഥ 0x1e67: അവസ്ഥ 0x1e69: അവസ്ഥ 0xa7a9:
		    EMIT2('s') EMIT2(0x15b) EMIT2(0x15d) EMIT2(0x15f)
		    EMIT2(0x161) EMIT2(0x219) EMIT2(0x23f) EMIT2(0x1d74)
		    EMIT2(0x1d8a) EMIT2(0x1e61) EMIT2(0x1e63) EMIT2(0x1e65)
		    EMIT2(0x1e67) EMIT2(0x1e69) EMIT2(0xa7a9)
		    തിരികെ OK;

	    അവസ്ഥ 't': അവസ്ഥ 0x163: അവസ്ഥ 0x165: അവസ്ഥ 0x167: അവസ്ഥ 0x1ab:
	    അവസ്ഥ 0x1ad: അവസ്ഥ 0x21b: അവസ്ഥ 0x288: അവസ്ഥ 0x1d75: അവസ്ഥ 0x1e6b:
	    അവസ്ഥ 0x1e6d: അവസ്ഥ 0x1e6f: അവസ്ഥ 0x1e71: അവസ്ഥ 0x1e97: അവസ്ഥ 0x2c66:
		    EMIT2('t') EMIT2(0x163) EMIT2(0x165) EMIT2(0x167)
		    EMIT2(0x1ab) EMIT2(0x1ad) EMIT2(0x21b) EMIT2(0x288)
		    EMIT2(0x1d75) EMIT2(0x1e6b) EMIT2(0x1e6d) EMIT2(0x1e6f)
		    EMIT2(0x1e71) EMIT2(0x1e97) EMIT2(0x2c66)
		    തിരികെ OK;

	    അവസ്ഥ 'u': അവസ്ഥ u_grave: അവസ്ഥ u_acute: അവസ്ഥ u_circumflex:
	    അവസ്ഥ u_diaeresis: അവസ്ഥ 0x169: അവസ്ഥ 0x16b: അവസ്ഥ 0x16d:
	    അവസ്ഥ 0x16f: അവസ്ഥ 0x171: അവസ്ഥ 0x173: അവസ്ഥ 0x1b0: അവസ്ഥ 0x1d4:
	    അവസ്ഥ 0x1d6: അവസ്ഥ 0x1d8: അവസ്ഥ 0x1da: അവസ്ഥ 0x1dc: അവസ്ഥ 0x215:
	    അവസ്ഥ 0x217: അവസ്ഥ 0x289: അവസ്ഥ 0x1d7e: അവസ്ഥ 0x1d99: അവസ്ഥ 0x1e73:
	    അവസ്ഥ 0x1e75: അവസ്ഥ 0x1e77: അവസ്ഥ 0x1e79: അവസ്ഥ 0x1e7b:
	    അവസ്ഥ 0x1ee5: അവസ്ഥ 0x1ee7: അവസ്ഥ 0x1ee9: അവസ്ഥ 0x1eeb:
	    അവസ്ഥ 0x1eed: അവസ്ഥ 0x1eef: അവസ്ഥ 0x1ef1:
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
		    തിരികെ OK;

	    അവസ്ഥ 'v': അവസ്ഥ 0x28b: അവസ്ഥ 0x1d8c: അവസ്ഥ 0x1e7d: അവസ്ഥ 0x1e7f:
		    EMIT2('v') EMIT2(0x28b) EMIT2(0x1d8c) EMIT2(0x1e7d)
		    EMIT2(0x1e7f)
		    തിരികെ OK;

	    അവസ്ഥ 'w': അവസ്ഥ 0x175: അവസ്ഥ 0x1e81: അവസ്ഥ 0x1e83: അവസ്ഥ 0x1e85:
	    അവസ്ഥ 0x1e87: അവസ്ഥ 0x1e89: അവസ്ഥ 0x1e98:
		    EMIT2('w') EMIT2(0x175) EMIT2(0x1e81) EMIT2(0x1e83)
		    EMIT2(0x1e85) EMIT2(0x1e87) EMIT2(0x1e89) EMIT2(0x1e98)
		    തിരികെ OK;

	    അവസ്ഥ 'x': അവസ്ഥ 0x1e8b: അവസ്ഥ 0x1e8d:
		    EMIT2('x') EMIT2(0x1e8b) EMIT2(0x1e8d)
		    തിരികെ OK;

	    അവസ്ഥ 'y': അവസ്ഥ y_acute: അവസ്ഥ y_diaeresis: അവസ്ഥ 0x177:
	    അവസ്ഥ 0x1b4: അവസ്ഥ 0x233: അവസ്ഥ 0x24f: അവസ്ഥ 0x1e8f:
	    അവസ്ഥ 0x1e99: അവസ്ഥ 0x1ef3: അവസ്ഥ 0x1ef5: അവസ്ഥ 0x1ef7:
	    അവസ്ഥ 0x1ef9:
		    EMIT2('y') EMIT2(y_acute) EMIT2(y_diaeresis)
		    EMIT2(0x177) EMIT2(0x1b4) EMIT2(0x233) EMIT2(0x24f)
		    EMIT2(0x1e8f) EMIT2(0x1e99) EMIT2(0x1ef3)
		    EMIT2(0x1ef5) EMIT2(0x1ef7) EMIT2(0x1ef9)
		    തിരികെ OK;

	    അവസ്ഥ 'z': അവസ്ഥ 0x17a: അവസ്ഥ 0x17c: അവസ്ഥ 0x17e: അവസ്ഥ 0x1b6:
	    അവസ്ഥ 0x1d76: അവസ്ഥ 0x1d8e: അവസ്ഥ 0x1e91: അവസ്ഥ 0x1e93:
	    അവസ്ഥ 0x1e95: അവസ്ഥ 0x2c6c:
		    EMIT2('z') EMIT2(0x17a) EMIT2(0x17c) EMIT2(0x17e)
		    EMIT2(0x1b6) EMIT2(0x1d76) EMIT2(0x1d8e) EMIT2(0x1e91)
		    EMIT2(0x1e93) EMIT2(0x1e95) EMIT2(0x2c6c)
		    തിരികെ OK;

	    // default: character itself
	}
    }

    EMIT2(c);
    തിരികെ OK;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
nfa_regatom(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		charclass;
    പൂർണ്ണസംഖ്യ		equiclass;
    പൂർണ്ണസംഖ്യ		collclass;
    പൂർണ്ണസംഖ്യ		got_coll_char;
    char_u	*p;
    char_u	*endp;
    char_u	*old_regparse = regparse;
    പൂർണ്ണസംഖ്യ		extra = 0;
    പൂർണ്ണസംഖ്യ		emit_range;
    പൂർണ്ണസംഖ്യ		negated;
    പൂർണ്ണസംഖ്യ		result;
    പൂർണ്ണസംഖ്യ		startc = -1;
    പൂർണ്ണസംഖ്യ		save_prev_at_start = prev_at_start;

    c = getchr();
    തിരഞ്ഞെടുപ്പ് (c)
    {
	അവസ്ഥ NUL:
	    EMSG_RET_FAIL(_(e_nfa_regexp_end_encountered_prematurely));

	അവസ്ഥ Magic('^'):
	    EMIT(NFA_BOL);
	    നിർത്തുക;

	അവസ്ഥ Magic('$'):
	    EMIT(NFA_EOL);
#എങ്കിൽ defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#endif
	    നിർത്തുക;

	അവസ്ഥ Magic('<'):
	    EMIT(NFA_BOW);
	    നിർത്തുക;

	അവസ്ഥ Magic('>'):
	    EMIT(NFA_EOW);
	    നിർത്തുക;

	അവസ്ഥ Magic('_'):
	    c = no_Magic(getchr());
	    എങ്കിൽ (c == NUL)
		EMSG_RET_FAIL(_(e_nfa_regexp_end_encountered_prematurely));

	    എങ്കിൽ (c == '^')	// "\_^" is start-of-line
	    {
		EMIT(NFA_BOL);
		നിർത്തുക;
	    }
	    എങ്കിൽ (c == '$')	// "\_$" is end-of-line
	    {
		EMIT(NFA_EOL);
#എങ്കിൽ defined(FEAT_SYN_HL)
		had_eol = TRUE;
#endif
		നിർത്തുക;
	    }

	    extra = NFA_ADD_NL;

	    // "\_[" is collection plus newline
	    എങ്കിൽ (c == '[')
		പോകുക collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	/*
	 * Character classes.
	 */
	അവസ്ഥ Magic('.'):
	അവസ്ഥ Magic('i'):
	അവസ്ഥ Magic('I'):
	അവസ്ഥ Magic('k'):
	അവസ്ഥ Magic('K'):
	അവസ്ഥ Magic('f'):
	അവസ്ഥ Magic('F'):
	അവസ്ഥ Magic('p'):
	അവസ്ഥ Magic('P'):
	അവസ്ഥ Magic('s'):
	അവസ്ഥ Magic('S'):
	അവസ്ഥ Magic('d'):
	അവസ്ഥ Magic('D'):
	അവസ്ഥ Magic('x'):
	അവസ്ഥ Magic('X'):
	അവസ്ഥ Magic('o'):
	അവസ്ഥ Magic('O'):
	അവസ്ഥ Magic('w'):
	അവസ്ഥ Magic('W'):
	അവസ്ഥ Magic('h'):
	അവസ്ഥ Magic('H'):
	അവസ്ഥ Magic('a'):
	അവസ്ഥ Magic('A'):
	അവസ്ഥ Magic('l'):
	അവസ്ഥ Magic('L'):
	അവസ്ഥ Magic('u'):
	അവസ്ഥ Magic('U'):
	    p = vim_strchr(classchars, no_Magic(c));
	    എങ്കിൽ (p == ശൂന്യ)
	    {
		എങ്കിൽ (extra == NFA_ADD_NL)
		{
		    semsg(_(e_nfa_regexp_invalid_character_class_nr), c);
		    rc_did_emsg = TRUE;
		    തിരികെ FAIL;
		}
		siemsg("Unknown character class char: %d", c);
		തിരികെ FAIL;
	    }

	    // When '.' is followed by a composing char ignore the dot, so that
	    // the composing char is matched here.
	    എങ്കിൽ (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	    {
		old_regparse = regparse;
		c = getchr();
		പോകുക nfa_do_multibyte;
	    }
	    EMIT(nfa_classcodes[p - classchars]);
	    എങ്കിൽ (extra == NFA_ADD_NL)
	    {
		EMIT(NFA_NEWL);
		EMIT(NFA_OR);
		regflags |= RF_HASNL;
	    }
	    നിർത്തുക;

	അവസ്ഥ Magic('n'):
	    എങ്കിൽ (reg_string)
		// In a string "\n" matches a newline character.
		EMIT(NL);
	    അല്ലെങ്കിൽ
	    {
		// In buffer text "\n" matches the end of a line.
		EMIT(NFA_NEWL);
		regflags |= RF_HASNL;
	    }
	    നിർത്തുക;

	അവസ്ഥ Magic('('):
	    എങ്കിൽ (nfa_reg(REG_PAREN) == FAIL)
		തിരികെ FAIL;	    // cascaded error
	    നിർത്തുക;

	അവസ്ഥ Magic('|'):
	അവസ്ഥ Magic('&'):
	അവസ്ഥ Magic(')'):
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    തിരികെ FAIL;

	അവസ്ഥ Magic('='):
	അവസ്ഥ Magic('?'):
	അവസ്ഥ Magic('+'):
	അവസ്ഥ Magic('@'):
	അവസ്ഥ Magic('*'):
	അവസ്ഥ Magic('{'):
	    // these should follow an atom, not form an atom
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    തിരികെ FAIL;

	അവസ്ഥ Magic('~'):
	    {
		char_u	    *lp;

		// Previous substitute pattern.
		// Generated as "\%(pattern\)".
		എങ്കിൽ (reg_prev_sub == ശൂന്യ)
		{
		    emsg(_(e_no_previous_substitute_regular_expression));
		    തിരികെ FAIL;
		}
		വേണ്ടി (lp = reg_prev_sub; *lp != NUL; MB_CPTR_ADV(lp))
		{
		    EMIT(PTR2CHAR(lp));
		    എങ്കിൽ (lp != reg_prev_sub)
			EMIT(NFA_CONCAT);
		}
		EMIT(NFA_NOPEN);
		നിർത്തുക;
	    }

	അവസ്ഥ Magic('1'):
	അവസ്ഥ Magic('2'):
	അവസ്ഥ Magic('3'):
	അവസ്ഥ Magic('4'):
	അവസ്ഥ Magic('5'):
	അവസ്ഥ Magic('6'):
	അവസ്ഥ Magic('7'):
	അവസ്ഥ Magic('8'):
	അവസ്ഥ Magic('9'):
	    {
		പൂർണ്ണസംഖ്യ refnum = no_Magic(c) - '1';

		എങ്കിൽ (!seen_endbrace(refnum + 1))
		    തിരികെ FAIL;
		EMIT(NFA_BACKREF1 + refnum);
		rex.nfa_has_backref = TRUE;
	    }
	    നിർത്തുക;

	അവസ്ഥ Magic('z'):
	    c = no_Magic(getchr());
	    തിരഞ്ഞെടുപ്പ് (c)
	    {
		അവസ്ഥ 's':
		    EMIT(NFA_ZSTART);
		    എങ്കിൽ (re_mult_next("\\zs") == FAIL)
			തിരികെ FAIL;
		    നിർത്തുക;
		അവസ്ഥ 'e':
		    EMIT(NFA_ZEND);
		    rex.nfa_has_zend = TRUE;
		    എങ്കിൽ (re_mult_next("\\ze") == FAIL)
			തിരികെ FAIL;
		    നിർത്തുക;
#ifdef FEAT_SYN_HL
		അവസ്ഥ '1':
		അവസ്ഥ '2':
		അവസ്ഥ '3':
		അവസ്ഥ '4':
		അവസ്ഥ '5':
		അവസ്ഥ '6':
		അവസ്ഥ '7':
		അവസ്ഥ '8':
		അവസ്ഥ '9':
		    // \z1...\z9
		    എങ്കിൽ ((reg_do_extmatch & REX_USE) == 0)
			EMSG_RET_FAIL(_(e_z1_z9_not_allowed_here));
		    EMIT(NFA_ZREF1 + (no_Magic(c) - '1'));
		    // No need to set rex.nfa_has_backref, the sub-matches don't
		    // change when \z1 .. \z9 matches or not.
		    re_has_z = REX_USE;
		    നിർത്തുക;
		അവസ്ഥ '(':
		    // \z(
		    എങ്കിൽ ((reg_do_extmatch & REX_SET) == 0)
			EMSG_RET_FAIL(_(e_z_not_allowed_here));
		    എങ്കിൽ (nfa_reg(REG_ZPAREN) == FAIL)
			തിരികെ FAIL;	    // cascaded error
		    re_has_z = REX_SET;
		    നിർത്തുക;
#endif
		സ്ഥിരസ്ഥിതി:
		    semsg(_(e_nfa_regexp_unknown_operator_z_chr), no_Magic(c));
		    തിരികെ FAIL;
	    }
	    നിർത്തുക;

	അവസ്ഥ Magic('%'):
	    c = no_Magic(getchr());
	    തിരഞ്ഞെടുപ്പ് (c)
	    {
		// () without a back reference
		അവസ്ഥ '(':
		    എങ്കിൽ (nfa_reg(REG_NPAREN) == FAIL)
			തിരികെ FAIL;
		    EMIT(NFA_NOPEN);
		    നിർത്തുക;

		അവസ്ഥ 'd':   // %d123 decimal
		അവസ്ഥ 'o':   // %o123 octal
		അവസ്ഥ 'x':   // %xab hex 2
		അവസ്ഥ 'u':   // %uabcd hex 4
		അവസ്ഥ 'U':   // %U1234abcd hex 8
		    {
			vimlong_T nr;

			തിരഞ്ഞെടുപ്പ് (c)
			{
			    അവസ്ഥ 'd': nr = getdecchrs(); നിർത്തുക;
			    അവസ്ഥ 'o': nr = getoctchrs(); നിർത്തുക;
			    അവസ്ഥ 'x': nr = gethexchrs(2); നിർത്തുക;
			    അവസ്ഥ 'u': nr = gethexchrs(4); നിർത്തുക;
			    അവസ്ഥ 'U': nr = gethexchrs(8); നിർത്തുക;
			    സ്ഥിരസ്ഥിതി:  nr = -1; നിർത്തുക;
			}

			എങ്കിൽ (nr < 0 || nr > INT_MAX)
			    EMSG2_RET_FAIL(_(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			// A NUL is stored in the text as NL
			// TODO: what if a composing character follows?
			EMIT(nr == 0 ? 0x0a : (long)nr);
		    }
		    നിർത്തുക;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		അവസ്ഥ '^':
		    EMIT(NFA_BOF);
		    നിർത്തുക;

		അവസ്ഥ '$':
		    EMIT(NFA_EOF);
		    നിർത്തുക;

		അവസ്ഥ '#':
		    എങ്കിൽ (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_start_of_pattern),
								  regparse[1]);
			തിരികെ FAIL;
		    }
		    EMIT(NFA_CURSOR);
		    നിർത്തുക;

		അവസ്ഥ 'V':
		    EMIT(NFA_VISUAL);
		    നിർത്തുക;

		അവസ്ഥ 'C':
		    EMIT(NFA_ANY_COMPOSING);
		    നിർത്തുക;

		അവസ്ഥ '[':
		    {
			പൂർണ്ണസംഖ്യ	    n;

			// \%[abc]
			വേണ്ടി (n = 0; (c = peekchr()) != ']'; ++n)
			{
			    എങ്കിൽ (c == NUL)
				EMSG2_RET_FAIL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
			    // recursive call!
			    എങ്കിൽ (nfa_regatom() == FAIL)
				തിരികെ FAIL;
			}
			getchr();  // get the ]
			എങ്കിൽ (n == 0)
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
			നിർത്തുക;
		    }

		സ്ഥിരസ്ഥിതി:
		    {
			long_u	n = 0;
			പൂർണ്ണസംഖ്യ	cmp = c;
			പൂർണ്ണസംഖ്യ	cur = FALSE;
			പൂർണ്ണസംഖ്യ	got_digit = FALSE;

			എങ്കിൽ (c == '<' || c == '>')
			    c = getchr();
			എങ്കിൽ (no_Magic(c) == '.')
			{
			    cur = TRUE;
			    c = getchr();
			}
			ആകുമ്പോൾ (VIM_ISDIGIT(c))
			{
			    long_u tmp;

			    എങ്കിൽ (cur)
			    {
				semsg(_(e_regexp_സംഖ്യ_after_dot_pos_തിരയൽ_chr),
								  no_Magic(c));
				തിരികെ FAIL;
			    }
			    tmp = n * 10 + (c - '0');

			    എങ്കിൽ (tmp < n)
			    {
				// overflow.
				emsg(_(e_percent_മൂല്യം_too_large));
				തിരികെ FAIL;
			    }
			    n = tmp;
			    c = getchr();
			    got_digit = TRUE;
			}
			എങ്കിൽ (c == 'l' || c == 'c' || c == 'v')
			{
			    long_u limit = INT_MAX;

			    എങ്കിൽ (!cur && !got_digit)
			    {
				semsg(_(e_nfa_regexp_missing_മൂല്യം_in_chr),
								  no_Magic(c));
				തിരികെ FAIL;
			    }
			    എങ്കിൽ (c == 'l')
			    {
				എങ്കിൽ (cur)
				    n = curwin->w_cursor.lnum;
				// \%{n}l  \%{n}<l  \%{n}>l
				EMIT(cmp == '<' ? NFA_LNUM_LT :
				     cmp == '>' ? NFA_LNUM_GT : NFA_LNUM);
				എങ്കിൽ (save_prev_at_start)
				    at_start = TRUE;
			    }
			    അല്ലെങ്കിൽ എങ്കിൽ (c == 'c')
			    {
				എങ്കിൽ (cur)
				{
				    n = curwin->w_cursor.col;
				    n++;
				}
				// \%{n}c  \%{n}<c  \%{n}>c
				EMIT(cmp == '<' ? NFA_COL_LT :
				     cmp == '>' ? NFA_COL_GT : NFA_COL);
			    }
			    അല്ലെങ്കിൽ
			    {
				എങ്കിൽ (cur)
				{
				    colnr_T vcol = 0;

				    getvvcol(curwin, &curwin->w_cursor,
							 ശൂന്യ, ശൂന്യ, &vcol, 0);
				    n = ++vcol;
				}
				// \%{n}v  \%{n}<v  \%{n}>v
				EMIT(cmp == '<' ? NFA_VCOL_LT :
				     cmp == '>' ? NFA_VCOL_GT : NFA_VCOL);
				limit = INT_MAX / MB_MAXBYTES;
			    }
			    എങ്കിൽ (n >= limit)
			    {
				emsg(_(e_percent_മൂല്യം_too_large));
				തിരികെ FAIL;
			    }
			    EMIT((പൂർണ്ണസംഖ്യ)n);
			    നിർത്തുക;
			}
			അല്ലെങ്കിൽ എങ്കിൽ (no_Magic(c) == '\'' && n == 0)
			{
			    // \%'m  \%<'m  \%>'m
			    EMIT(cmp == '<' ? NFA_MARK_LT :
				 cmp == '>' ? NFA_MARK_GT : NFA_MARK);
			    EMIT(getchr());
			    നിർത്തുക;
			}
		    }
		    semsg(_(e_nfa_regexp_unknown_operator_percent_chr),
								  no_Magic(c));
		    തിരികെ FAIL;
	    }
	    നിർത്തുക;

	അവസ്ഥ Magic('['):
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
	    എങ്കിൽ (*endp == ']')
	    {
		പൂർണ്ണസംഖ്യ plen;
		bool range_endpoint;
		/*
		 * Try to reverse engineer character classes. For example,
		 * recognize that [0-9] stands for \d and [A-Za-z_] for \h,
		 * and perform the necessary substitutions in the NFA.
		 */
		result = nfa_recognize_char_class(regparse, endp,
							 extra == NFA_ADD_NL);
		എങ്കിൽ (result != FAIL)
		{
		    എങ്കിൽ (result >= NFA_FIRST_NL && result <= NFA_LAST_NL)
		    {
			EMIT(result - NFA_ADD_NL);
			EMIT(NFA_NEWL);
			EMIT(NFA_OR);
		    }
		    അല്ലെങ്കിൽ
			EMIT(result);
		    regparse = endp;
		    MB_PTR_ADV(regparse);
		    തിരികെ OK;
		}
		/*
		 * Failed to recognize a character class. Use the simple
		 * version that turns [abc] into 'a' OR 'b' OR 'c'
		 */
		startc = -1;
		negated = FALSE;
		എങ്കിൽ (*regparse == '^')			// negated range
		{
		    negated = TRUE;
		    MB_PTR_ADV(regparse);
		    EMIT(NFA_START_NEG_COLL);
		}
		അല്ലെങ്കിൽ
		    EMIT(NFA_START_COLL);
		എങ്കിൽ (*regparse == '-')
		{
		    startc = '-';
		    EMIT(startc);
		    EMIT(NFA_CONCAT);
		    MB_PTR_ADV(regparse);
		}
		// Emit the OR branches for each character in the []
		emit_range = FALSE;
		ആകുമ്പോൾ (regparse < endp)
		{
		    പൂർണ്ണസംഖ്യ	    oldstartc = startc;
		    range_endpoint = false;

		    startc = -1;
		    got_coll_char = FALSE;
		    എങ്കിൽ (*regparse == '[')
		    {
			// Check for [: :], [= =], [. .]
			equiclass = collclass = 0;
			charclass = get_char_class(&regparse);
			എങ്കിൽ (charclass == CLASS_NONE)
			{
			    equiclass = get_equi_class(&regparse);
			    എങ്കിൽ (equiclass == 0)
				collclass = get_coll_element(&regparse);
			}

			// Character class like [:alpha:]
			എങ്കിൽ (charclass != CLASS_NONE)
			{
			    തിരഞ്ഞെടുപ്പ് (charclass)
			    {
				അവസ്ഥ CLASS_ALNUM:
				    EMIT(NFA_CLASS_ALNUM);
				    നിർത്തുക;
				അവസ്ഥ CLASS_ALPHA:
				    EMIT(NFA_CLASS_ALPHA);
				    നിർത്തുക;
				അവസ്ഥ CLASS_BLANK:
				    EMIT(NFA_CLASS_BLANK);
				    നിർത്തുക;
				അവസ്ഥ CLASS_CNTRL:
				    EMIT(NFA_CLASS_CNTRL);
				    നിർത്തുക;
				അവസ്ഥ CLASS_DIGIT:
				    EMIT(NFA_CLASS_DIGIT);
				    നിർത്തുക;
				അവസ്ഥ CLASS_GRAPH:
				    EMIT(NFA_CLASS_GRAPH);
				    നിർത്തുക;
				അവസ്ഥ CLASS_LOWER:
				    wants_nfa = TRUE;
				    EMIT(NFA_CLASS_LOWER);
				    നിർത്തുക;
				അവസ്ഥ CLASS_PRINT:
				    EMIT(NFA_CLASS_PRINT);
				    നിർത്തുക;
				അവസ്ഥ CLASS_PUNCT:
				    EMIT(NFA_CLASS_PUNCT);
				    നിർത്തുക;
				അവസ്ഥ CLASS_SPACE:
				    EMIT(NFA_CLASS_SPACE);
				    നിർത്തുക;
				അവസ്ഥ CLASS_UPPER:
				    wants_nfa = TRUE;
				    EMIT(NFA_CLASS_UPPER);
				    നിർത്തുക;
				അവസ്ഥ CLASS_XDIGIT:
				    EMIT(NFA_CLASS_XDIGIT);
				    നിർത്തുക;
				അവസ്ഥ CLASS_TAB:
				    EMIT(NFA_CLASS_TAB);
				    നിർത്തുക;
				അവസ്ഥ CLASS_RETURN:
				    EMIT(NFA_CLASS_RETURN);
				    നിർത്തുക;
				അവസ്ഥ CLASS_BACKSPACE:
				    EMIT(NFA_CLASS_BACKSPACE);
				    നിർത്തുക;
				അവസ്ഥ CLASS_ESCAPE:
				    EMIT(NFA_CLASS_ESCAPE);
				    നിർത്തുക;
				അവസ്ഥ CLASS_IDENT:
				    EMIT(NFA_CLASS_IDENT);
				    നിർത്തുക;
				അവസ്ഥ CLASS_KEYWORD:
				    EMIT(NFA_CLASS_KEYWORD);
				    നിർത്തുക;
				അവസ്ഥ CLASS_FNAME:
				    EMIT(NFA_CLASS_FNAME);
				    നിർത്തുക;
			    }
			    EMIT(NFA_CONCAT);
			    തുടരുക;
			}
			// Try equivalence class [=a=] and the like
			എങ്കിൽ (equiclass != 0)
			{
			    result = nfa_emit_equi_class(equiclass);
			    എങ്കിൽ (result == FAIL)
			    {
				// should never happen
				EMSG_RET_FAIL(_(e_error_building_nfa_with_equivalence_class));
			    }
			    തുടരുക;
			}
			// Try collating class like [. .]
			എങ്കിൽ (collclass != 0)
			{
			    startc = collclass;	 // allow [.a.]-x as a range
			    // Will emit the proper atom at the end of the
			    // while loop.
			}
		    }
		    // Try a range like 'a-x' or '\t-z'. Also allows '-' as a
		    // start character.
		    എങ്കിൽ (*regparse == '-' && oldstartc != -1)
		    {
			emit_range = TRUE;
			startc = oldstartc;
			MB_PTR_ADV(regparse);
			തുടരുക;	    // reading the end of the range
		    }

		    // Now handle simple and escaped characters.
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    എങ്കിൽ (*regparse == '\\'
			    && !reg_cpo_bsl
			    && regparse + 1 <= endp
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != ശൂന്യ
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR, regparse[1])
								      != ശൂന്യ)
			    )
			)
		    {
			MB_PTR_ADV(regparse);

			എങ്കിൽ (*regparse == 'n')
			    startc = (reg_string || emit_range
					|| regparse[1] == '-') ? NL : NFA_NEWL;
			അല്ലെങ്കിൽ എങ്കിൽ (*regparse == 'd'
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
				എങ്കിൽ (startc == INT_MAX)
				    EMSG_RET_FAIL(_(e_unicode_val_too_large));
				got_coll_char = TRUE;
				MB_PTR_BACK(old_regparse, regparse);
			    }
			    അല്ലെങ്കിൽ
			    {
				// \r,\t,\e,\b
				startc = backslash_trans(*regparse);
			    }
		    }

		    // Normal printable char
		    എങ്കിൽ (startc == -1)
			startc = PTR2CHAR(regparse);

		    // Previous char was '-', so this char is end of range.
		    എങ്കിൽ (emit_range)
		    {
			പൂർണ്ണസംഖ്യ	endc = startc;
			range_endpoint = true;

			startc = oldstartc;
			എങ്കിൽ (startc > endc)
			    EMSG_RET_FAIL(_(e_reverse_range_in_character_class));

			എങ്കിൽ (endc > startc + 2)
			{
			    // Emit a range instead of the sequence of
			    // individual characters.
			    എങ്കിൽ (startc == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    അല്ലെങ്കിൽ
				--post_ptr; // remove NFA_CONCAT
			    EMIT(endc);
			    EMIT(NFA_RANGE);
			    EMIT(NFA_CONCAT);
			}
			അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte && ((*mb_char2len)(startc) > 1
				    || (*mb_char2len)(endc) > 1))
			{
			    // Emit the characters in the range.
			    // "startc" was already emitted, so skip it.
			    //
			    വേണ്ടി (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			അല്ലെങ്കിൽ
			{
			    // Emit the range. "startc" was already emitted, so
			    // skip it.
			    വേണ്ടി (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			emit_range = FALSE;
			startc = -1;
		    }
		    അല്ലെങ്കിൽ
		    {
			// This char (startc) is not part of a range. Just
			// emit it.
			// Normally, simply emit startc. But if we get char
			// code=0 from a collating char, then replace it with
			// 0x0a.
			// This is needed to completely mimic the behaviour of
			// the backtracking engine.
			എങ്കിൽ (startc == NFA_NEWL)
			{
			    // Line break can't be matched as part of the
			    // collection, add an OR below. But not for negated
			    // range.
			    എങ്കിൽ (!negated)
				extra = NFA_ADD_NL;
			}
			അല്ലെങ്കിൽ
			{
			    എങ്കിൽ (got_coll_char == TRUE && startc == 0)
			    {
				EMIT(0x0a);
				EMIT(NFA_CONCAT);
			    }
			    അല്ലെങ്കിൽ
			    {
				EMIT(startc);
				എങ്കിൽ (!(enc_utf8 && (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse)))))
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
		    എങ്കിൽ (!range_endpoint && enc_utf8 &&
			    (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse))))
		    {
			പൂർണ്ണസംഖ്യ i = utf_ptr2len(regparse);

			c = utf_ptr2char(regparse + i);

			// Add composing characters
			വേണ്ടി (;;)
			{
			    എങ്കിൽ (c == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    അല്ലെങ്കിൽ
				EMIT(c);
			    EMIT(NFA_CONCAT);
			    എങ്കിൽ ((i += utf_char2len(c)) >= plen)
				നിർത്തുക;
			    c = utf_ptr2char(regparse + i);
			}
			EMIT(NFA_COMPOSING);
			EMIT(NFA_CONCAT);
		    }
		    MB_PTR_ADV(regparse);
		} // while (p < endp)

		MB_PTR_BACK(old_regparse, regparse);
		എങ്കിൽ (*regparse == '-')	    // if last, '-' is just a char
		{
		    EMIT('-');
		    EMIT(NFA_CONCAT);
		}

		// skip the trailing ]
		regparse = endp;
		MB_PTR_ADV(regparse);

		// Mark end of the collection.
		എങ്കിൽ (negated == TRUE)
		    EMIT(NFA_END_NEG_COLL);
		അല്ലെങ്കിൽ
		    EMIT(NFA_END_COLL);

		// \_[] also matches \n but it's not negated
		എങ്കിൽ (extra == NFA_ADD_NL)
		{
		    EMIT(reg_string ? NL : NFA_NEWL);
		    EMIT(NFA_OR);
		}

		തിരികെ OK;
	    } // if exists closing ]

	    എങ്കിൽ (reg_strict)
		EMSG_RET_FAIL(_(e_missing_rsb_after_str_lsb));
	    // FALLTHROUGH

	സ്ഥിരസ്ഥിതി:
	    {
		പൂർണ്ണസംഖ്യ	plen;

nfa_do_multibyte:
		// plen is length of current char with composing chars
		എങ്കിൽ (enc_utf8 && ((*mb_char2len)(c)
			    != (plen = utfc_ptr2len(old_regparse))
						       || utf_iscomposing(c)))
		{
		    പൂർണ്ണസംഖ്യ i = 0;

		    // A base character plus composing characters, or just one
		    // or more composing characters.
		    // This requires creating a separate atom as if enclosing
		    // the characters in (), where NFA_COMPOSING is the ( and
		    // NFA_END_COMPOSING is the ). Note that right now we are
		    // building the postfix form, not the NFA itself;
		    // a composing char could be: a, b, c, NFA_COMPOSING
		    // where 'b' and 'c' are chars with codes > 256.
		    വേണ്ടി (;;)
		    {
			EMIT(c);
			എങ്കിൽ (i > 0)
			    EMIT(NFA_CONCAT);
			എങ്കിൽ ((i += utf_char2len(c)) >= plen)
			    നിർത്തുക;
			c = utf_ptr2char(old_regparse + i);
		    }
		    EMIT(NFA_COMPOSING);
		    regparse = old_regparse + plen;
		}
		അല്ലെങ്കിൽ
		{
		    c = no_Magic(c);
		    EMIT(c);
		}
		തിരികെ OK;
	    }
    }

    തിരികെ OK;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
nfa_regpiece(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		op;
    പൂർണ്ണസംഖ്യ		ret;
    long	minval, maxval;
    പൂർണ്ണസംഖ്യ		greedy = TRUE;      // Braces are prefixed with '-' ?
    parse_state_T old_state;
    parse_state_T new_state;
    long	c2;
    പൂർണ്ണസംഖ്യ		old_post_pos;
    പൂർണ്ണസംഖ്യ		my_post_start;
    പൂർണ്ണസംഖ്യ		quest;

    // Save the current parse state, so that we can use it if <atom>{m,n} is
    // next.
    save_parse_state(&old_state);

    // store current pos in the postfix form, for \{m,n} involving 0s
    my_post_start = (പൂർണ്ണസംഖ്യ)(post_ptr - post_start);

    ret = nfa_regatom();
    എങ്കിൽ (ret == FAIL)
	തിരികെ FAIL;	    // cascaded error

    op = peekchr();
    എങ്കിൽ (re_multi_type(op) == NOT_MULTI)
	തിരികെ OK;

    skipchr();
    തിരഞ്ഞെടുപ്പ് (op)
    {
	അവസ്ഥ Magic('*'):
	    EMIT(NFA_STAR);
	    നിർത്തുക;

	അവസ്ഥ Magic('+'):
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
	    എങ്കിൽ (nfa_regatom() == FAIL)
		തിരികെ FAIL;
	    EMIT(NFA_STAR);
	    EMIT(NFA_CONCAT);
	    skipchr();		// skip the \+
	    നിർത്തുക;

	അവസ്ഥ Magic('@'):
	    c2 = (long)getdecchrs();
	    op = no_Magic(getchr());
	    i = 0;
	    തിരഞ്ഞെടുപ്പ്(op)
	    {
		അവസ്ഥ '=':
		    // \@=
		    i = NFA_PREV_ATOM_NO_WIDTH;
		    നിർത്തുക;
		അവസ്ഥ '!':
		    // \@!
		    i = NFA_PREV_ATOM_NO_WIDTH_NEG;
		    നിർത്തുക;
		അവസ്ഥ '<':
		    op = no_Magic(getchr());
		    എങ്കിൽ (op == '=')
			// \@<=
			i = NFA_PREV_ATOM_JUST_BEFORE;
		    അല്ലെങ്കിൽ എങ്കിൽ (op == '!')
			// \@<!
			i = NFA_PREV_ATOM_JUST_BEFORE_NEG;
		    നിർത്തുക;
		അവസ്ഥ '>':
		    // \@>
		    i = NFA_PREV_ATOM_LIKE_PATTERN;
		    നിർത്തുക;
	    }
	    എങ്കിൽ (i == 0)
	    {
		semsg(_(e_nfa_regexp_unknown_operator_at_chr), op);
		തിരികെ FAIL;
	    }
	    EMIT(i);
	    എങ്കിൽ (i == NFA_PREV_ATOM_JUST_BEFORE
					|| i == NFA_PREV_ATOM_JUST_BEFORE_NEG)
		EMIT(c2);
	    നിർത്തുക;

	അവസ്ഥ Magic('?'):
	അവസ്ഥ Magic('='):
	    EMIT(NFA_QUEST);
	    നിർത്തുക;

	അവസ്ഥ Magic('{'):
	    // a{2,5} will expand to 'aaa?a?a?'
	    // a{-1,3} will expand to 'aa??a??', where ?? is the nongreedy
	    // version of '?'
	    // \v(ab){2,3} will expand to '(ab)(ab)(ab)?', where all the
	    // parenthesis have the same id

	    greedy = TRUE;
	    c2 = peekchr();
	    എങ്കിൽ (c2 == '-' || c2 == Magic('-'))
	    {
		skipchr();
		greedy = FALSE;
	    }
	    എങ്കിൽ (!read_limits(&minval, &maxval))
		EMSG_RET_FAIL(_(e_nfa_regexp_error_reading_repetition_limits));

	    //  <atom>{0,inf}, <atom>{0,} and <atom>{}  are equivalent to
	    //  <atom>*
	    എങ്കിൽ (minval == 0 && maxval == MAX_LIMIT)
	    {
		എങ്കിൽ (greedy)		// { { (match the braces)
		    // \{}, \{0,}
		    EMIT(NFA_STAR);
		അല്ലെങ്കിൽ			// { { (match the braces)
		    // \{-}, \{-0,}
		    EMIT(NFA_STAR_NONGREEDY);
		നിർത്തുക;
	    }

	    // Special case: x{0} or x{-0}
	    എങ്കിൽ (maxval == 0)
	    {
		// Ignore result of previous call to nfa_regatom()
		post_ptr = post_start + my_post_start;
		// NFA_EMPTY is 0-length and works everywhere
		EMIT(NFA_EMPTY);
		തിരികെ OK;
	    }

	    // The engine is very inefficient (uses too many states) when the
	    // maximum is much larger than the minimum and when the maximum is
	    // large.  However, when maxval is MAX_LIMIT, it is okay, as this
	    // will emit NFA_STAR.
	    // Bail out if we can use the other engine, but only, when the
	    // pattern does not need the NFA engine like (e.g. [[:upper:]]\{2,\}
	    // does not work with characters > 8 bit with the BT engine)
	    എങ്കിൽ ((nfa_re_flags & RE_AUTO)
				   && (maxval > 500 || maxval > minval + 200)
				   && (maxval != MAX_LIMIT && minval < 200)
				   && !wants_nfa)
		തിരികെ FAIL;

	    // Ignore previous call to nfa_regatom()
	    post_ptr = post_start + my_post_start;
	    // Save parse state after the repeated atom and the \{}
	    save_parse_state(&new_state);

	    quest = (greedy == TRUE? NFA_QUEST : NFA_QUEST_NONGREEDY);
	    വേണ്ടി (i = 0; i < maxval; i++)
	    {
		// Goto beginning of the repeated atom
		restore_parse_state(&old_state);
		old_post_pos = (പൂർണ്ണസംഖ്യ)(post_ptr - post_start);
		എങ്കിൽ (nfa_regatom() == FAIL)
		    തിരികെ FAIL;
		// after "minval" times, atoms are optional
		എങ്കിൽ (i + 1 > minval)
		{
		    എങ്കിൽ (maxval == MAX_LIMIT)
		    {
			എങ്കിൽ (greedy)
			    EMIT(NFA_STAR);
			അല്ലെങ്കിൽ
			    EMIT(NFA_STAR_NONGREEDY);
		    }
		    അല്ലെങ്കിൽ
			EMIT(quest);
		}
		എങ്കിൽ (old_post_pos != my_post_start)
		    EMIT(NFA_CONCAT);
		എങ്കിൽ (i + 1 > minval && maxval == MAX_LIMIT)
		    നിർത്തുക;
	    }

	    // Go to just after the repeated atom and the \{}
	    restore_parse_state(&new_state);
	    curchr = -1;

	    നിർത്തുക;


	സ്ഥിരസ്ഥിതി:
	    നിർത്തുക;
    }	// end switch

    എങ്കിൽ (re_multi_type(peekchr()) != NOT_MULTI)
	// Can't have a multi follow a multi.
	EMSG_RET_FAIL(_(e_nfa_regexp_cant_have_multi_follow_multi));

    തിരികെ OK;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
nfa_regconcat(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		cont = TRUE;
    പൂർണ്ണസംഖ്യ		first = TRUE;

    ആകുമ്പോൾ (cont)
    {
	തിരഞ്ഞെടുപ്പ് (peekchr())
	{
	    അവസ്ഥ NUL:
	    അവസ്ഥ Magic('|'):
	    അവസ്ഥ Magic('&'):
	    അവസ്ഥ Magic(')'):
		cont = FALSE;
		നിർത്തുക;

	    അവസ്ഥ Magic('Z'):
		regflags |= RF_ICOMBINE;
		skipchr_keepstart();
		നിർത്തുക;
	    അവസ്ഥ Magic('c'):
		regflags |= RF_ICASE;
		skipchr_keepstart();
		നിർത്തുക;
	    അവസ്ഥ Magic('C'):
		regflags |= RF_NOICASE;
		skipchr_keepstart();
		നിർത്തുക;
	    അവസ്ഥ Magic('v'):
		reg_magic = MAGIC_ALL;
		skipchr_keepstart();
		curchr = -1;
		നിർത്തുക;
	    അവസ്ഥ Magic('m'):
		reg_magic = MAGIC_ON;
		skipchr_keepstart();
		curchr = -1;
		നിർത്തുക;
	    അവസ്ഥ Magic('M'):
		reg_magic = MAGIC_OFF;
		skipchr_keepstart();
		curchr = -1;
		നിർത്തുക;
	    അവസ്ഥ Magic('V'):
		reg_magic = MAGIC_NONE;
		skipchr_keepstart();
		curchr = -1;
		നിർത്തുക;

	    സ്ഥിരസ്ഥിതി:
		എങ്കിൽ (nfa_regpiece() == FAIL)
		    തിരികെ FAIL;
		എങ്കിൽ (first == FALSE)
		    EMIT(NFA_CONCAT);
		അല്ലെങ്കിൽ
		    first = FALSE;
		നിർത്തുക;
	}
    }

    തിരികെ OK;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
nfa_regbranch(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		old_post_pos;

    old_post_pos = (പൂർണ്ണസംഖ്യ)(post_ptr - post_start);

    // First branch, possibly the only one
    എങ്കിൽ (nfa_regconcat() == FAIL)
	തിരികെ FAIL;

    // Try next concats
    ആകുമ്പോൾ (peekchr() == Magic('&'))
    {
	skipchr();
	// if concat is empty do emit a node
	എങ്കിൽ (old_post_pos == (പൂർണ്ണസംഖ്യ)(post_ptr - post_start))
	    EMIT(NFA_EMPTY);
	EMIT(NFA_NOPEN);
	EMIT(NFA_PREV_ATOM_NO_WIDTH);
	old_post_pos = (പൂർണ്ണസംഖ്യ)(post_ptr - post_start);
	എങ്കിൽ (nfa_regconcat() == FAIL)
	    തിരികെ FAIL;
	// if concat is empty do emit a node
	എങ്കിൽ (old_post_pos == (പൂർണ്ണസംഖ്യ)(post_ptr - post_start))
	    EMIT(NFA_EMPTY);
	EMIT(NFA_CONCAT);
    }

    // if a branch is empty, emit one node for it
    എങ്കിൽ (old_post_pos == (പൂർണ്ണസംഖ്യ)(post_ptr - post_start))
	EMIT(NFA_EMPTY);

    തിരികെ OK;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
nfa_reg(
    പൂർണ്ണസംഖ്യ		paren)	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
{
    പൂർണ്ണസംഖ്യ		parno = 0;
    പൂർണ്ണസംഖ്യ		status = FAIL;

    എങ്കിൽ (paren == REG_PAREN)
    {
	എങ്കിൽ (regnpar >= NSUBEXP) // Too many `('
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_parens));
	parno = regnpar++;
    }
#ifdef FEAT_SYN_HL
    അല്ലെങ്കിൽ എങ്കിൽ (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	എങ്കിൽ (regnzpar >= NSUBEXP)
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_z));
	parno = regnzpar++;
    }
#endif

    എങ്കിൽ (nfa_reg_parse_depth >= REG_MAX_PAREN_DEPTH)
	EMSG_RET_FAIL(_(e_command_too_complex));
    ++nfa_reg_parse_depth;

    എങ്കിൽ (nfa_regbranch() == FAIL)
	പോകുക theend;	    // cascaded error

    ആകുമ്പോൾ (peekchr() == Magic('|'))
    {
	skipchr();
	എങ്കിൽ (nfa_regbranch() == FAIL)
	    പോകുക theend;    // cascaded error
	EMIT(NFA_OR);
    }

    // Check for proper termination.
    എങ്കിൽ (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
	എങ്കിൽ (paren == REG_NPAREN)
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	അല്ലെങ്കിൽ
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	rc_did_emsg = TRUE;
	പോകുക theend;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (paren == REG_NOPAREN && peekchr() != NUL)
    {
	എങ്കിൽ (peekchr() == Magic(')'))
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	അല്ലെങ്കിൽ
	    emsg(_(e_nfa_regexp_proper_termination_error));
	rc_did_emsg = TRUE;
	പോകുക theend;
    }
    /*
     * Here we set the flag allowing back references to this set of
     * parentheses.
     */
    എങ്കിൽ (paren == REG_PAREN)
    {
	had_endbrace[parno] = TRUE;     // have seen the close paren
	EMIT(NFA_MOPEN + parno);
    }
#ifdef FEAT_SYN_HL
    അല്ലെങ്കിൽ എങ്കിൽ (paren == REG_ZPAREN)
	EMIT(NFA_ZOPEN + parno);
#endif

    status = OK;

theend:
    --nfa_reg_parse_depth;
    തിരികെ status;
}

#ifdef DEBUG
സ്ഥിതം char_u code[50];

    സ്ഥിതം ശൂന്യം
nfa_set_code(പൂർണ്ണസംഖ്യ c)
{
    പൂർണ്ണസംഖ്യ	    addnl = FALSE;

    എങ്കിൽ (c >= NFA_FIRST_NL && c <= NFA_LAST_NL)
    {
	addnl = TRUE;
	c -= NFA_ADD_NL;
    }

    STRCPY(code, "");
    തിരഞ്ഞെടുപ്പ് (c)
    {
	അവസ്ഥ NFA_MATCH:	    STRCPY(code, "NFA_MATCH "); നിർത്തുക;
	അവസ്ഥ NFA_SPLIT:	    STRCPY(code, "NFA_SPLIT "); നിർത്തുക;
	അവസ്ഥ NFA_CONCAT:    STRCPY(code, "NFA_CONCAT "); നിർത്തുക;
	അവസ്ഥ NFA_NEWL:	    STRCPY(code, "NFA_NEWL "); നിർത്തുക;
	അവസ്ഥ NFA_ZSTART:    STRCPY(code, "NFA_ZSTART"); നിർത്തുക;
	അവസ്ഥ NFA_ZEND:	    STRCPY(code, "NFA_ZEND"); നിർത്തുക;

	അവസ്ഥ NFA_BACKREF1:  STRCPY(code, "NFA_BACKREF1"); നിർത്തുക;
	അവസ്ഥ NFA_BACKREF2:  STRCPY(code, "NFA_BACKREF2"); നിർത്തുക;
	അവസ്ഥ NFA_BACKREF3:  STRCPY(code, "NFA_BACKREF3"); നിർത്തുക;
	അവസ്ഥ NFA_BACKREF4:  STRCPY(code, "NFA_BACKREF4"); നിർത്തുക;
	അവസ്ഥ NFA_BACKREF5:  STRCPY(code, "NFA_BACKREF5"); നിർത്തുക;
	അവസ്ഥ NFA_BACKREF6:  STRCPY(code, "NFA_BACKREF6"); നിർത്തുക;
	അവസ്ഥ NFA_BACKREF7:  STRCPY(code, "NFA_BACKREF7"); നിർത്തുക;
	അവസ്ഥ NFA_BACKREF8:  STRCPY(code, "NFA_BACKREF8"); നിർത്തുക;
	അവസ്ഥ NFA_BACKREF9:  STRCPY(code, "NFA_BACKREF9"); നിർത്തുക;
# ifdef FEAT_SYN_HL
	അവസ്ഥ NFA_ZREF1:	    STRCPY(code, "NFA_ZREF1"); നിർത്തുക;
	അവസ്ഥ NFA_ZREF2:	    STRCPY(code, "NFA_ZREF2"); നിർത്തുക;
	അവസ്ഥ NFA_ZREF3:	    STRCPY(code, "NFA_ZREF3"); നിർത്തുക;
	അവസ്ഥ NFA_ZREF4:	    STRCPY(code, "NFA_ZREF4"); നിർത്തുക;
	അവസ്ഥ NFA_ZREF5:	    STRCPY(code, "NFA_ZREF5"); നിർത്തുക;
	അവസ്ഥ NFA_ZREF6:	    STRCPY(code, "NFA_ZREF6"); നിർത്തുക;
	അവസ്ഥ NFA_ZREF7:	    STRCPY(code, "NFA_ZREF7"); നിർത്തുക;
	അവസ്ഥ NFA_ZREF8:	    STRCPY(code, "NFA_ZREF8"); നിർത്തുക;
	അവസ്ഥ NFA_ZREF9:	    STRCPY(code, "NFA_ZREF9"); നിർത്തുക;
# endif
	അവസ്ഥ NFA_SKIP:	    STRCPY(code, "NFA_SKIP"); നിർത്തുക;

	അവസ്ഥ NFA_PREV_ATOM_NO_WIDTH:
			    STRCPY(code, "NFA_PREV_ATOM_NO_WIDTH"); നിർത്തുക;
	അവസ്ഥ NFA_PREV_ATOM_NO_WIDTH_NEG:
			    STRCPY(code, "NFA_PREV_ATOM_NO_WIDTH_NEG"); നിർത്തുക;
	അവസ്ഥ NFA_PREV_ATOM_JUST_BEFORE:
			    STRCPY(code, "NFA_PREV_ATOM_JUST_BEFORE"); നിർത്തുക;
	അവസ്ഥ NFA_PREV_ATOM_JUST_BEFORE_NEG:
			 STRCPY(code, "NFA_PREV_ATOM_JUST_BEFORE_NEG"); നിർത്തുക;
	അവസ്ഥ NFA_PREV_ATOM_LIKE_PATTERN:
			    STRCPY(code, "NFA_PREV_ATOM_LIKE_PATTERN"); നിർത്തുക;

	അവസ്ഥ NFA_NOPEN:		    STRCPY(code, "NFA_NOPEN"); നിർത്തുക;
	അവസ്ഥ NFA_NCLOSE:	    STRCPY(code, "NFA_NCLOSE"); നിർത്തുക;
	അവസ്ഥ NFA_START_INVISIBLE:   STRCPY(code, "NFA_START_INVISIBLE"); നിർത്തുക;
	അവസ്ഥ NFA_START_INVISIBLE_FIRST:
			     STRCPY(code, "NFA_START_INVISIBLE_FIRST"); നിർത്തുക;
	അവസ്ഥ NFA_START_INVISIBLE_NEG:
			       STRCPY(code, "NFA_START_INVISIBLE_NEG"); നിർത്തുക;
	അവസ്ഥ NFA_START_INVISIBLE_NEG_FIRST:
			 STRCPY(code, "NFA_START_INVISIBLE_NEG_FIRST"); നിർത്തുക;
	അവസ്ഥ NFA_START_INVISIBLE_BEFORE:
			    STRCPY(code, "NFA_START_INVISIBLE_BEFORE"); നിർത്തുക;
	അവസ്ഥ NFA_START_INVISIBLE_BEFORE_FIRST:
		      STRCPY(code, "NFA_START_INVISIBLE_BEFORE_FIRST"); നിർത്തുക;
	അവസ്ഥ NFA_START_INVISIBLE_BEFORE_NEG:
			STRCPY(code, "NFA_START_INVISIBLE_BEFORE_NEG"); നിർത്തുക;
	അവസ്ഥ NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
		  STRCPY(code, "NFA_START_INVISIBLE_BEFORE_NEG_FIRST"); നിർത്തുക;
	അവസ്ഥ NFA_START_PATTERN:   STRCPY(code, "NFA_START_PATTERN"); നിർത്തുക;
	അവസ്ഥ NFA_END_INVISIBLE:	    STRCPY(code, "NFA_END_INVISIBLE"); നിർത്തുക;
	അവസ്ഥ NFA_END_INVISIBLE_NEG: STRCPY(code, "NFA_END_INVISIBLE_NEG"); നിർത്തുക;
	അവസ്ഥ NFA_END_PATTERN:	    STRCPY(code, "NFA_END_PATTERN"); നിർത്തുക;

	അവസ്ഥ NFA_COMPOSING:	    STRCPY(code, "NFA_COMPOSING"); നിർത്തുക;
	അവസ്ഥ NFA_END_COMPOSING:	    STRCPY(code, "NFA_END_COMPOSING"); നിർത്തുക;
	അവസ്ഥ NFA_OPT_CHARS:	    STRCPY(code, "NFA_OPT_CHARS"); നിർത്തുക;

	അവസ്ഥ NFA_MOPEN:
	അവസ്ഥ NFA_MOPEN1:
	അവസ്ഥ NFA_MOPEN2:
	അവസ്ഥ NFA_MOPEN3:
	അവസ്ഥ NFA_MOPEN4:
	അവസ്ഥ NFA_MOPEN5:
	അവസ്ഥ NFA_MOPEN6:
	അവസ്ഥ NFA_MOPEN7:
	അവസ്ഥ NFA_MOPEN8:
	അവസ്ഥ NFA_MOPEN9:
	    STRCPY(code, "NFA_MOPEN(x)");
	    code[10] = c - NFA_MOPEN + '0';
	    നിർത്തുക;
	അവസ്ഥ NFA_MCLOSE:
	അവസ്ഥ NFA_MCLOSE1:
	അവസ്ഥ NFA_MCLOSE2:
	അവസ്ഥ NFA_MCLOSE3:
	അവസ്ഥ NFA_MCLOSE4:
	അവസ്ഥ NFA_MCLOSE5:
	അവസ്ഥ NFA_MCLOSE6:
	അവസ്ഥ NFA_MCLOSE7:
	അവസ്ഥ NFA_MCLOSE8:
	അവസ്ഥ NFA_MCLOSE9:
	    STRCPY(code, "NFA_MCLOSE(x)");
	    code[11] = c - NFA_MCLOSE + '0';
	    നിർത്തുക;
# ifdef FEAT_SYN_HL
	അവസ്ഥ NFA_ZOPEN:
	അവസ്ഥ NFA_ZOPEN1:
	അവസ്ഥ NFA_ZOPEN2:
	അവസ്ഥ NFA_ZOPEN3:
	അവസ്ഥ NFA_ZOPEN4:
	അവസ്ഥ NFA_ZOPEN5:
	അവസ്ഥ NFA_ZOPEN6:
	അവസ്ഥ NFA_ZOPEN7:
	അവസ്ഥ NFA_ZOPEN8:
	അവസ്ഥ NFA_ZOPEN9:
	    STRCPY(code, "NFA_ZOPEN(x)");
	    code[10] = c - NFA_ZOPEN + '0';
	    നിർത്തുക;
	അവസ്ഥ NFA_ZCLOSE:
	അവസ്ഥ NFA_ZCLOSE1:
	അവസ്ഥ NFA_ZCLOSE2:
	അവസ്ഥ NFA_ZCLOSE3:
	അവസ്ഥ NFA_ZCLOSE4:
	അവസ്ഥ NFA_ZCLOSE5:
	അവസ്ഥ NFA_ZCLOSE6:
	അവസ്ഥ NFA_ZCLOSE7:
	അവസ്ഥ NFA_ZCLOSE8:
	അവസ്ഥ NFA_ZCLOSE9:
	    STRCPY(code, "NFA_ZCLOSE(x)");
	    code[11] = c - NFA_ZCLOSE + '0';
	    നിർത്തുക;
# endif
	അവസ്ഥ NFA_EOL:		STRCPY(code, "NFA_EOL "); നിർത്തുക;
	അവസ്ഥ NFA_BOL:		STRCPY(code, "NFA_BOL "); നിർത്തുക;
	അവസ്ഥ NFA_EOW:		STRCPY(code, "NFA_EOW "); നിർത്തുക;
	അവസ്ഥ NFA_BOW:		STRCPY(code, "NFA_BOW "); നിർത്തുക;
	അവസ്ഥ NFA_EOF:		STRCPY(code, "NFA_EOF "); നിർത്തുക;
	അവസ്ഥ NFA_BOF:		STRCPY(code, "NFA_BOF "); നിർത്തുക;
	അവസ്ഥ NFA_LNUM:		STRCPY(code, "NFA_LNUM "); നിർത്തുക;
	അവസ്ഥ NFA_LNUM_GT:	STRCPY(code, "NFA_LNUM_GT "); നിർത്തുക;
	അവസ്ഥ NFA_LNUM_LT:	STRCPY(code, "NFA_LNUM_LT "); നിർത്തുക;
	അവസ്ഥ NFA_COL:		STRCPY(code, "NFA_COL "); നിർത്തുക;
	അവസ്ഥ NFA_COL_GT:	STRCPY(code, "NFA_COL_GT "); നിർത്തുക;
	അവസ്ഥ NFA_COL_LT:	STRCPY(code, "NFA_COL_LT "); നിർത്തുക;
	അവസ്ഥ NFA_VCOL:		STRCPY(code, "NFA_VCOL "); നിർത്തുക;
	അവസ്ഥ NFA_VCOL_GT:	STRCPY(code, "NFA_VCOL_GT "); നിർത്തുക;
	അവസ്ഥ NFA_VCOL_LT:	STRCPY(code, "NFA_VCOL_LT "); നിർത്തുക;
	അവസ്ഥ NFA_MARK:		STRCPY(code, "NFA_MARK "); നിർത്തുക;
	അവസ്ഥ NFA_MARK_GT:	STRCPY(code, "NFA_MARK_GT "); നിർത്തുക;
	അവസ്ഥ NFA_MARK_LT:	STRCPY(code, "NFA_MARK_LT "); നിർത്തുക;
	അവസ്ഥ NFA_CURSOR:	STRCPY(code, "NFA_CURSOR "); നിർത്തുക;
	അവസ്ഥ NFA_VISUAL:	STRCPY(code, "NFA_VISUAL "); നിർത്തുക;
	അവസ്ഥ NFA_ANY_COMPOSING:	STRCPY(code, "NFA_ANY_COMPOSING "); നിർത്തുക;

	അവസ്ഥ NFA_STAR:		STRCPY(code, "NFA_STAR "); നിർത്തുക;
	അവസ്ഥ NFA_STAR_NONGREEDY: STRCPY(code, "NFA_STAR_NONGREEDY "); നിർത്തുക;
	അവസ്ഥ NFA_QUEST:		STRCPY(code, "NFA_QUEST"); നിർത്തുക;
	അവസ്ഥ NFA_QUEST_NONGREEDY: STRCPY(code, "NFA_QUEST_NON_GREEDY"); നിർത്തുക;
	അവസ്ഥ NFA_EMPTY:		STRCPY(code, "NFA_EMPTY"); നിർത്തുക;
	അവസ്ഥ NFA_OR:		STRCPY(code, "NFA_OR"); നിർത്തുക;

	അവസ്ഥ NFA_START_COLL:	STRCPY(code, "NFA_START_COLL"); നിർത്തുക;
	അവസ്ഥ NFA_END_COLL:	STRCPY(code, "NFA_END_COLL"); നിർത്തുക;
	അവസ്ഥ NFA_START_NEG_COLL: STRCPY(code, "NFA_START_NEG_COLL"); നിർത്തുക;
	അവസ്ഥ NFA_END_NEG_COLL:	STRCPY(code, "NFA_END_NEG_COLL"); നിർത്തുക;
	അവസ്ഥ NFA_RANGE:		STRCPY(code, "NFA_RANGE"); നിർത്തുക;
	അവസ്ഥ NFA_RANGE_MIN:	STRCPY(code, "NFA_RANGE_MIN"); നിർത്തുക;
	അവസ്ഥ NFA_RANGE_MAX:	STRCPY(code, "NFA_RANGE_MAX"); നിർത്തുക;

	അവസ്ഥ NFA_CLASS_ALNUM:	STRCPY(code, "NFA_CLASS_ALNUM"); നിർത്തുക;
	അവസ്ഥ NFA_CLASS_ALPHA:	STRCPY(code, "NFA_CLASS_ALPHA"); നിർത്തുക;
	അവസ്ഥ NFA_CLASS_BLANK:	STRCPY(code, "NFA_CLASS_BLANK"); നിർത്തുക;
	അവസ്ഥ NFA_CLASS_CNTRL:	STRCPY(code, "NFA_CLASS_CNTRL"); നിർത്തുക;
	അവസ്ഥ NFA_CLASS_DIGIT:	STRCPY(code, "NFA_CLASS_DIGIT"); നിർത്തുക;
	അവസ്ഥ NFA_CLASS_GRAPH:	STRCPY(code, "NFA_CLASS_GRAPH"); നിർത്തുക;
	അവസ്ഥ NFA_CLASS_LOWER:	STRCPY(code, "NFA_CLASS_LOWER"); നിർത്തുക;
	അവസ്ഥ NFA_CLASS_PRINT:	STRCPY(code, "NFA_CLASS_PRINT"); നിർത്തുക;
	അവസ്ഥ NFA_CLASS_PUNCT:	STRCPY(code, "NFA_CLASS_PUNCT"); നിർത്തുക;
	അവസ്ഥ NFA_CLASS_SPACE:	STRCPY(code, "NFA_CLASS_SPACE"); നിർത്തുക;
	അവസ്ഥ NFA_CLASS_UPPER:	STRCPY(code, "NFA_CLASS_UPPER"); നിർത്തുക;
	അവസ്ഥ NFA_CLASS_XDIGIT:	STRCPY(code, "NFA_CLASS_XDIGIT"); നിർത്തുക;
	അവസ്ഥ NFA_CLASS_TAB:	STRCPY(code, "NFA_CLASS_TAB"); നിർത്തുക;
	അവസ്ഥ NFA_CLASS_RETURN:	STRCPY(code, "NFA_CLASS_RETURN"); നിർത്തുക;
	അവസ്ഥ NFA_CLASS_BACKSPACE:   STRCPY(code, "NFA_CLASS_BACKSPACE"); നിർത്തുക;
	അവസ്ഥ NFA_CLASS_ESCAPE:	STRCPY(code, "NFA_CLASS_ESCAPE"); നിർത്തുക;
	അവസ്ഥ NFA_CLASS_IDENT:	STRCPY(code, "NFA_CLASS_IDENT"); നിർത്തുക;
	അവസ്ഥ NFA_CLASS_KEYWORD:	STRCPY(code, "NFA_CLASS_KEYWORD"); നിർത്തുക;
	അവസ്ഥ NFA_CLASS_FNAME:	STRCPY(code, "NFA_CLASS_FNAME"); നിർത്തുക;

	അവസ്ഥ NFA_ANY:	STRCPY(code, "NFA_ANY"); നിർത്തുക;
	അവസ്ഥ NFA_IDENT:	STRCPY(code, "NFA_IDENT"); നിർത്തുക;
	അവസ്ഥ NFA_SIDENT:STRCPY(code, "NFA_SIDENT"); നിർത്തുക;
	അവസ്ഥ NFA_KWORD:	STRCPY(code, "NFA_KWORD"); നിർത്തുക;
	അവസ്ഥ NFA_SKWORD:STRCPY(code, "NFA_SKWORD"); നിർത്തുക;
	അവസ്ഥ NFA_FNAME:	STRCPY(code, "NFA_FNAME"); നിർത്തുക;
	അവസ്ഥ NFA_SFNAME:STRCPY(code, "NFA_SFNAME"); നിർത്തുക;
	അവസ്ഥ NFA_PRINT:	STRCPY(code, "NFA_PRINT"); നിർത്തുക;
	അവസ്ഥ NFA_SPRINT:STRCPY(code, "NFA_SPRINT"); നിർത്തുക;
	അവസ്ഥ NFA_WHITE:	STRCPY(code, "NFA_WHITE"); നിർത്തുക;
	അവസ്ഥ NFA_NWHITE:STRCPY(code, "NFA_NWHITE"); നിർത്തുക;
	അവസ്ഥ NFA_DIGIT:	STRCPY(code, "NFA_DIGIT"); നിർത്തുക;
	അവസ്ഥ NFA_NDIGIT:STRCPY(code, "NFA_NDIGIT"); നിർത്തുക;
	അവസ്ഥ NFA_HEX:	STRCPY(code, "NFA_HEX"); നിർത്തുക;
	അവസ്ഥ NFA_NHEX:	STRCPY(code, "NFA_NHEX"); നിർത്തുക;
	അവസ്ഥ NFA_OCTAL:	STRCPY(code, "NFA_OCTAL"); നിർത്തുക;
	അവസ്ഥ NFA_NOCTAL:STRCPY(code, "NFA_NOCTAL"); നിർത്തുക;
	അവസ്ഥ NFA_WORD:	STRCPY(code, "NFA_WORD"); നിർത്തുക;
	അവസ്ഥ NFA_NWORD:	STRCPY(code, "NFA_NWORD"); നിർത്തുക;
	അവസ്ഥ NFA_HEAD:	STRCPY(code, "NFA_HEAD"); നിർത്തുക;
	അവസ്ഥ NFA_NHEAD:	STRCPY(code, "NFA_NHEAD"); നിർത്തുക;
	അവസ്ഥ NFA_ALPHA:	STRCPY(code, "NFA_ALPHA"); നിർത്തുക;
	അവസ്ഥ NFA_NALPHA:STRCPY(code, "NFA_NALPHA"); നിർത്തുക;
	അവസ്ഥ NFA_LOWER:	STRCPY(code, "NFA_LOWER"); നിർത്തുക;
	അവസ്ഥ NFA_NLOWER:STRCPY(code, "NFA_NLOWER"); നിർത്തുക;
	അവസ്ഥ NFA_UPPER:	STRCPY(code, "NFA_UPPER"); നിർത്തുക;
	അവസ്ഥ NFA_NUPPER:STRCPY(code, "NFA_NUPPER"); നിർത്തുക;
	അവസ്ഥ NFA_LOWER_IC:  STRCPY(code, "NFA_LOWER_IC"); നിർത്തുക;
	അവസ്ഥ NFA_NLOWER_IC: STRCPY(code, "NFA_NLOWER_IC"); നിർത്തുക;
	അവസ്ഥ NFA_UPPER_IC:  STRCPY(code, "NFA_UPPER_IC"); നിർത്തുക;
	അവസ്ഥ NFA_NUPPER_IC: STRCPY(code, "NFA_NUPPER_IC"); നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
	    STRCPY(code, "CHAR(x)");
	    code[5] = c;
    }

    എങ്കിൽ (addnl == TRUE)
	STRCAT(code, " + NEWLINE ");

}

# ifdef ENABLE_LOG
സ്ഥിതം FILE *log_fd;
സ്ഥിതം char_u e_log_open_failed[] = N_("Could not open temporary log file for writing, displaying on stderr... ");

/*
 * Print the postfix notation of the current regexp.
 */
    സ്ഥിതം ശൂന്യം
nfa_postfix_dump(char_u *വ്യഞ്ജകം, പൂർണ്ണസംഖ്യ retval)
{
    പൂർണ്ണസംഖ്യ *p;
    FILE *f;

    f = fopen(NFA_REGEXP_DUMP_LOG, "a");
    എങ്കിൽ (f == ശൂന്യ)
	തിരികെ;

    fprintf(f, "\n-------------------------\n");
    എങ്കിൽ (retval == FAIL)
	fprintf(f, ">>> NFA engine failed... \n");
    അല്ലെങ്കിൽ എങ്കിൽ (retval == OK)
	fprintf(f, ">>> NFA engine succeeded !\n");
    fprintf(f, "Regexp: \"%s\"\nPostfix notation (char): \"", വ്യഞ്ജകം);
    വേണ്ടി (p = post_start; *p && p < post_ptr; p++)
    {
	nfa_set_code(*p);
	fprintf(f, "%s, ", code);
    }
    fprintf(f, "\"\nPostfix notation (int): ");
    വേണ്ടി (p = post_start; *p && p < post_ptr; p++)
	fprintf(f, "%d ", *p);
    fprintf(f, "\n\n");
    fclose(f);
}

/*
 * Print the NFA starting with a root node "state".
 */
    സ്ഥിതം ശൂന്യം
nfa_print_state(FILE *debugf, nfa_state_T *state)
{
    garray_T indent;

    ga_init2(&indent, 1, 64);
    ga_append(&indent, '\0');
    nfa_print_state2(debugf, state, &indent);
    ga_clear(&indent);
}

    സ്ഥിതം ശൂന്യം
nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent)
{
    char_u  *p;

    എങ്കിൽ (state == ശൂന്യ)
	തിരികെ;

    fprintf(debugf, "(%2d)", abs(state->id));

    // Output indent
    p = (char_u *)indent->ga_data;
    എങ്കിൽ (indent->ga_len >= 3)
    {
	പൂർണ്ണസംഖ്യ	last = indent->ga_len - 3;
	char_u	save[2];

	STRNCPY(save, &p[last], 2);
	memcpy(&p[last], "+-", 2);
	fprintf(debugf, " %s", p);
	STRNCPY(&p[last], save, 2);
    }
    അല്ലെങ്കിൽ
	fprintf(debugf, " %s", p);

    nfa_set_code(state->c);
    fprintf(debugf, "%s (%d) (id=%d) val=%d\n",
		 code,
		 state->c,
		 abs(state->id),
		 state->val);
    എങ്കിൽ (state->id < 0)
	തിരികെ;

    state->id = abs(state->id) * -1;

    // grow indent for state->out
    indent->ga_len -= 1;
    എങ്കിൽ (state->out1)
	GA_CONCAT_LITERAL(indent, "| ");
    അല്ലെങ്കിൽ
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
    സ്ഥിതം ശൂന്യം
nfa_dump(nfa_regprog_T *prog)
{
    FILE *debugf = fopen(NFA_REGEXP_DUMP_LOG, "a");

    എങ്കിൽ (debugf == ശൂന്യ)
	തിരികെ;

    nfa_print_state(debugf, prog->start);

    എങ്കിൽ (prog->reganch)
	fprintf(debugf, "reganch: %d\n", prog->reganch);
    എങ്കിൽ (prog->regstart != NUL)
	fprintf(debugf, "regstart: %c (decimal: %d)\n",
		prog->regstart, prog->regstart);
    എങ്കിൽ (prog->match_text != ശൂന്യ)
	fprintf(debugf, "match_text: \"%s\"\n", prog->match_text);

    fclose(debugf);
}
# endif	    // ENABLE_LOG
#endif	    // DEBUG

/*
 * Parse r.e. @expr and convert it into postfix form.
 * Return the postfix string on success, NULL otherwise.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ *
re2post(ശൂന്യം)
{
    എങ്കിൽ (nfa_reg(REG_NOPAREN) == FAIL)
	തിരികെ ശൂന്യ;
    EMIT(NFA_MOPEN);
    തിരികെ post_start;
}

// NB. Some of the code below is inspired by Russ's.

/*
 * Represents an NFA state plus zero or one or two arrows exiting.
 * if c == MATCH, no arrows out; matching state.
 * If c == SPLIT, unlabeled arrows to out and out1 (if != NULL).
 * If c < 256, labeled arrow with character c to out.
 */

സ്ഥിതം nfa_state_T	*state_ptr; // points to nfa_prog->state

/*
 * Allocate and initialize nfa_state_T.
 */
    സ്ഥിതം nfa_state_T *
alloc_state(പൂർണ്ണസംഖ്യ c, nfa_state_T *out, nfa_state_T *out1)
{
    nfa_state_T *s;

    എങ്കിൽ (istate >= nstate)
	തിരികെ ശൂന്യ;

    s = &state_ptr[istate++];

    s->c    = c;
    s->out  = out;
    s->out1 = out1;
    s->val  = 0;

    s->id   = istate;
    s->lastlist[0] = 0;
    s->lastlist[1] = 0;

    തിരികെ s;
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
തരനിർവചനം സംഘം Ptrlist Ptrlist;
സംഘം Ptrlist
{
    Ptrlist	*next;
    nfa_state_T	*s;
};

ഘടന Frag
{
    nfa_state_T *start;
    Ptrlist	*out;
};
തരനിർവചനം ഘടന Frag Frag_T;

// Reused across compilations by post2nfa(); freed in free_regexp_stuff().
സ്ഥിതം Frag_T *nfa_അട്ടി;
സ്ഥിതം പൂർണ്ണസംഖ്യ nfa_അട്ടി_len;

/*
 * Initialize a Frag_T struct and return it.
 */
    സ്ഥിതം Frag_T
frag(nfa_state_T *start, Ptrlist *out)
{
    Frag_T n;

    n.start = start;
    n.out = out;
    തിരികെ n;
}

/*
 * Create singleton list containing just outp.
 */
    സ്ഥിതം Ptrlist *
list1(
    nfa_state_T	**outp)
{
    Ptrlist *l;

    l = (Ptrlist *)outp;
    l->next = ശൂന്യ;
    തിരികെ l;
}

/*
 * Patch the list of states at out to point to start.
 */
    സ്ഥിതം ശൂന്യം
patch(Ptrlist *l, nfa_state_T *s)
{
    Ptrlist *next;

    വേണ്ടി (; l; l = next)
    {
	next = l->next;
	l->s = s;
    }
}


/*
 * Join the two lists l1 and l2, returning the combination.
 */
    സ്ഥിതം Ptrlist *
append(Ptrlist *l1, Ptrlist *l2)
{
    Ptrlist *oldl1;

    oldl1 = l1;
    ആകുമ്പോൾ (l1->next)
	l1 = l1->next;
    l1->next = l2;
    തിരികെ oldl1;
}

/*
 * Stack used for transforming postfix form into NFA.
 */
സ്ഥിതം Frag_T empty;

    സ്ഥിതം ശൂന്യം
st_error(പൂർണ്ണസംഖ്യ *postfix UNUSED, പൂർണ്ണസംഖ്യ *end UNUSED, പൂർണ്ണസംഖ്യ *p UNUSED)
{
#ifdef NFA_REGEXP_ERROR_LOG
    FILE *df;
    പൂർണ്ണസംഖ്യ *p2;

    df = fopen(NFA_REGEXP_ERROR_LOG, "a");
    എങ്കിൽ (df)
    {
	fprintf(df, "Error popping the stack!\n");
# ifdef DEBUG
	fprintf(df, "Current regexp is \"%s\"\n", nfa_regengine.വ്യഞ്ജകം);
# endif
	fprintf(df, "Postfix form is: ");
# ifdef DEBUG
	വേണ്ടി (p2 = postfix; p2 < end; p2++)
	{
	    nfa_set_code(*p2);
	    fprintf(df, "%s, ", code);
	}
	nfa_set_code(*p);
	fprintf(df, "\nCurrent position is: ");
	വേണ്ടി (p2 = postfix; p2 <= p; p2 ++)
	{
	    nfa_set_code(*p2);
	    fprintf(df, "%s, ", code);
	}
# അല്ലെങ്കിൽ
	വേണ്ടി (p2 = postfix; p2 < end; p2++)
	    fprintf(df, "%d, ", *p2);
	fprintf(df, "\nCurrent position is: ");
	വേണ്ടി (p2 = postfix; p2 <= p; p2 ++)
	    fprintf(df, "%d, ", *p2);
# endif
	fprintf(df, "\n--------------------------\n");
	fclose(df);
    }
#endif
    emsg(_(e_nfa_regexp_could_not_എടുക്കുക_അട്ടി));
}

/*
 * Push an item onto the stack.
 */
    സ്ഥിതം ശൂന്യം
st_തള്ളുക(Frag_T s, Frag_T **p, Frag_T *അട്ടി_end)
{
    Frag_T *stackp = *p;

    എങ്കിൽ (stackp >= അട്ടി_end)
	തിരികെ;
    *stackp = s;
    *p = *p + 1;
}

/*
 * Pop an item from the stack.
 */
    സ്ഥിതം Frag_T
st_എടുക്കുക(Frag_T **p, Frag_T *അട്ടി)
{
    Frag_T *stackp;

    *p = *p - 1;
    stackp = *p;
    എങ്കിൽ (stackp < അട്ടി)
	തിരികെ empty;
    തിരികെ **p;
}

/*
 * Estimate the maximum byte length of anything matching "state".
 * When unknown or unlimited return -1.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
nfa_max_width(nfa_state_T *startstate, പൂർണ്ണസംഖ്യ depth)
{
    പൂർണ്ണസംഖ്യ		    l, r;
    nfa_state_T	    *state = startstate;
    പൂർണ്ണസംഖ്യ		    len = 0;

    // detect looping in a NFA_SPLIT
    എങ്കിൽ (depth > 4)
	തിരികെ -1;

    ആകുമ്പോൾ (state != ശൂന്യ)
    {
	തിരഞ്ഞെടുപ്പ് (state->c)
	{
	    അവസ്ഥ NFA_END_INVISIBLE:
	    അവസ്ഥ NFA_END_INVISIBLE_NEG:
		// the end, return what we have
		തിരികെ len;

	    അവസ്ഥ NFA_SPLIT:
		// two alternatives, use the maximum
		l = nfa_max_width(state->out, depth + 1);
		r = nfa_max_width(state->out1, depth + 1);
		എങ്കിൽ (l < 0 || r < 0)
		    തിരികെ -1;
		തിരികെ len + (l > r ? l : r);

	    അവസ്ഥ NFA_ANY:
	    അവസ്ഥ NFA_START_COLL:
	    അവസ്ഥ NFA_START_NEG_COLL:
		// matches some character, including composing chars
		എങ്കിൽ (enc_utf8)
		    len += MB_MAXBYTES;
		അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte)
		    len += 2;
		അല്ലെങ്കിൽ
		    ++len;
		എങ്കിൽ (state->c != NFA_ANY)
		{
		    // Skip over the compiled collection.
		    // malformed NFAs must not crash width estimation.
		    എങ്കിൽ (state->out1 == ശൂന്യ || state->out1->out == ശൂന്യ)
			തിരികെ -1;
		    state = state->out1->out;
		    തുടരുക;
		}
		നിർത്തുക;

	    അവസ്ഥ NFA_DIGIT:
	    അവസ്ഥ NFA_WHITE:
	    അവസ്ഥ NFA_HEX:
	    അവസ്ഥ NFA_OCTAL:
		// ascii
		++len;
		നിർത്തുക;

	    അവസ്ഥ NFA_IDENT:
	    അവസ്ഥ NFA_SIDENT:
	    അവസ്ഥ NFA_KWORD:
	    അവസ്ഥ NFA_SKWORD:
	    അവസ്ഥ NFA_FNAME:
	    അവസ്ഥ NFA_SFNAME:
	    അവസ്ഥ NFA_PRINT:
	    അവസ്ഥ NFA_SPRINT:
	    അവസ്ഥ NFA_NWHITE:
	    അവസ്ഥ NFA_NDIGIT:
	    അവസ്ഥ NFA_NHEX:
	    അവസ്ഥ NFA_NOCTAL:
	    അവസ്ഥ NFA_WORD:
	    അവസ്ഥ NFA_NWORD:
	    അവസ്ഥ NFA_HEAD:
	    അവസ്ഥ NFA_NHEAD:
	    അവസ്ഥ NFA_ALPHA:
	    അവസ്ഥ NFA_NALPHA:
	    അവസ്ഥ NFA_LOWER:
	    അവസ്ഥ NFA_NLOWER:
	    അവസ്ഥ NFA_UPPER:
	    അവസ്ഥ NFA_NUPPER:
	    അവസ്ഥ NFA_LOWER_IC:
	    അവസ്ഥ NFA_NLOWER_IC:
	    അവസ്ഥ NFA_UPPER_IC:
	    അവസ്ഥ NFA_NUPPER_IC:
	    അവസ്ഥ NFA_ANY_COMPOSING:
		// possibly non-ascii
		എങ്കിൽ (has_mbyte)
		    len += 3;
		അല്ലെങ്കിൽ
		    ++len;
		നിർത്തുക;

	    അവസ്ഥ NFA_START_INVISIBLE:
	    അവസ്ഥ NFA_START_INVISIBLE_NEG:
	    അവസ്ഥ NFA_START_INVISIBLE_BEFORE:
	    അവസ്ഥ NFA_START_INVISIBLE_BEFORE_NEG:
		// zero-width, out1 points to the END state
		state = state->out1->out;
		തുടരുക;

	    അവസ്ഥ NFA_BACKREF1:
	    അവസ്ഥ NFA_BACKREF2:
	    അവസ്ഥ NFA_BACKREF3:
	    അവസ്ഥ NFA_BACKREF4:
	    അവസ്ഥ NFA_BACKREF5:
	    അവസ്ഥ NFA_BACKREF6:
	    അവസ്ഥ NFA_BACKREF7:
	    അവസ്ഥ NFA_BACKREF8:
	    അവസ്ഥ NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	    അവസ്ഥ NFA_ZREF1:
	    അവസ്ഥ NFA_ZREF2:
	    അവസ്ഥ NFA_ZREF3:
	    അവസ്ഥ NFA_ZREF4:
	    അവസ്ഥ NFA_ZREF5:
	    അവസ്ഥ NFA_ZREF6:
	    അവസ്ഥ NFA_ZREF7:
	    അവസ്ഥ NFA_ZREF8:
	    അവസ്ഥ NFA_ZREF9:
#endif
	    അവസ്ഥ NFA_NEWL:
	    അവസ്ഥ NFA_SKIP:
		// unknown width
		തിരികെ -1;

	    അവസ്ഥ NFA_BOL:
	    അവസ്ഥ NFA_EOL:
	    അവസ്ഥ NFA_BOF:
	    അവസ്ഥ NFA_EOF:
	    അവസ്ഥ NFA_BOW:
	    അവസ്ഥ NFA_EOW:
	    അവസ്ഥ NFA_MOPEN:
	    അവസ്ഥ NFA_MOPEN1:
	    അവസ്ഥ NFA_MOPEN2:
	    അവസ്ഥ NFA_MOPEN3:
	    അവസ്ഥ NFA_MOPEN4:
	    അവസ്ഥ NFA_MOPEN5:
	    അവസ്ഥ NFA_MOPEN6:
	    അവസ്ഥ NFA_MOPEN7:
	    അവസ്ഥ NFA_MOPEN8:
	    അവസ്ഥ NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	    അവസ്ഥ NFA_ZOPEN:
	    അവസ്ഥ NFA_ZOPEN1:
	    അവസ്ഥ NFA_ZOPEN2:
	    അവസ്ഥ NFA_ZOPEN3:
	    അവസ്ഥ NFA_ZOPEN4:
	    അവസ്ഥ NFA_ZOPEN5:
	    അവസ്ഥ NFA_ZOPEN6:
	    അവസ്ഥ NFA_ZOPEN7:
	    അവസ്ഥ NFA_ZOPEN8:
	    അവസ്ഥ NFA_ZOPEN9:
	    അവസ്ഥ NFA_ZCLOSE:
	    അവസ്ഥ NFA_ZCLOSE1:
	    അവസ്ഥ NFA_ZCLOSE2:
	    അവസ്ഥ NFA_ZCLOSE3:
	    അവസ്ഥ NFA_ZCLOSE4:
	    അവസ്ഥ NFA_ZCLOSE5:
	    അവസ്ഥ NFA_ZCLOSE6:
	    അവസ്ഥ NFA_ZCLOSE7:
	    അവസ്ഥ NFA_ZCLOSE8:
	    അവസ്ഥ NFA_ZCLOSE9:
#endif
	    അവസ്ഥ NFA_MCLOSE:
	    അവസ്ഥ NFA_MCLOSE1:
	    അവസ്ഥ NFA_MCLOSE2:
	    അവസ്ഥ NFA_MCLOSE3:
	    അവസ്ഥ NFA_MCLOSE4:
	    അവസ്ഥ NFA_MCLOSE5:
	    അവസ്ഥ NFA_MCLOSE6:
	    അവസ്ഥ NFA_MCLOSE7:
	    അവസ്ഥ NFA_MCLOSE8:
	    അവസ്ഥ NFA_MCLOSE9:
	    അവസ്ഥ NFA_NOPEN:
	    അവസ്ഥ NFA_NCLOSE:

	    അവസ്ഥ NFA_LNUM_GT:
	    അവസ്ഥ NFA_LNUM_LT:
	    അവസ്ഥ NFA_COL_GT:
	    അവസ്ഥ NFA_COL_LT:
	    അവസ്ഥ NFA_VCOL_GT:
	    അവസ്ഥ NFA_VCOL_LT:
	    അവസ്ഥ NFA_MARK_GT:
	    അവസ്ഥ NFA_MARK_LT:
	    അവസ്ഥ NFA_VISUAL:
	    അവസ്ഥ NFA_LNUM:
	    അവസ്ഥ NFA_CURSOR:
	    അവസ്ഥ NFA_COL:
	    അവസ്ഥ NFA_VCOL:
	    അവസ്ഥ NFA_MARK:

	    അവസ്ഥ NFA_ZSTART:
	    അവസ്ഥ NFA_ZEND:
	    അവസ്ഥ NFA_OPT_CHARS:
	    അവസ്ഥ NFA_EMPTY:
	    അവസ്ഥ NFA_START_PATTERN:
	    അവസ്ഥ NFA_END_PATTERN:
	    അവസ്ഥ NFA_COMPOSING:
	    അവസ്ഥ NFA_END_COMPOSING:
		// zero-width
		നിർത്തുക;

	    സ്ഥിരസ്ഥിതി:
		എങ്കിൽ (state->c < 0)
		    // don't know what this is
		    തിരികെ -1;
		// normal character
		len += MB_CHAR2LEN(state->c);
		നിർത്തുക;
	}

	// normal way to continue
	state = state->out;
    }

    // unrecognized, "cannot happen"
    തിരികെ -1;
}

/*
 * Convert a postfix form into its equivalent NFA.
 * Return the NFA start state on success, NULL otherwise.
 */
    സ്ഥിതം nfa_state_T *
post2nfa(പൂർണ്ണസംഖ്യ *postfix, പൂർണ്ണസംഖ്യ *end, പൂർണ്ണസംഖ്യ nfa_calc_size)
{
    പൂർണ്ണസംഖ്യ		*p;
    പൂർണ്ണസംഖ്യ		mopen;
    പൂർണ്ണസംഖ്യ		mclose;
    Frag_T	*അട്ടി = ശൂന്യ;
    Frag_T	*stackp = ശൂന്യ;
    Frag_T	*അട്ടി_end = ശൂന്യ;
    Frag_T	e1;
    Frag_T	e2;
    Frag_T	e;
    nfa_state_T	*s;
    nfa_state_T	*s1;
    nfa_state_T	*matchstate;
    nfa_state_T	*ret = ശൂന്യ;

    എങ്കിൽ (postfix == ശൂന്യ)
	തിരികെ ശൂന്യ;

#define തള്ളുക_2(s)	    st_തള്ളുക((s), &stackp, അട്ടി_end)
#define എടുക്കുക_2()	    st_എടുക്കുക(&stackp, അട്ടി);		\
		    എങ്കിൽ (stackp < അട്ടി)			\
		    {					\
			st_error(postfix, end, p);	\
			തിരികെ ശൂന്യ;			\
		    }

    എങ്കിൽ (nfa_calc_size == FALSE)
    {
	// Reuse the fragment stack across compilations, growing when needed;
	// it is freed in free_regexp_stuff().
	എങ്കിൽ (nfa_അട്ടി == ശൂന്യ || nfa_അട്ടി_len < nstate + 1)
	{
	    vim_free(nfa_അട്ടി);
	    nfa_അട്ടി = ALLOC_MULT(Frag_T, nstate + 1);
	    എങ്കിൽ (nfa_അട്ടി == ശൂന്യ)
	    {
		nfa_അട്ടി_len = 0;
		തിരികെ ശൂന്യ;
	    }
	    nfa_അട്ടി_len = nstate + 1;
	}
	അട്ടി = nfa_അട്ടി;
	stackp = അട്ടി;
	അട്ടി_end = അട്ടി + (nstate + 1);
    }

    വേണ്ടി (p = postfix; p < end; ++p)
    {
	തിരഞ്ഞെടുപ്പ് (*p)
	{
	അവസ്ഥ NFA_CONCAT:
	    // Concatenation.
	    // Pay attention: this operator does not exist in the r.e. itself
	    // (it is implicit, really).  It is added when r.e. is translated
	    // to postfix form in re2post().
	    എങ്കിൽ (nfa_calc_size == TRUE)
	    {
		// nstate += 0;
		നിർത്തുക;
	    }
	    e2 = എടുക്കുക_2();
	    e1 = എടുക്കുക_2();
	    patch(e1.out, e2.start);
	    തള്ളുക_2(frag(e1.start, e2.out));
	    നിർത്തുക;

	അവസ്ഥ NFA_OR:
	    // Alternation
	    എങ്കിൽ (nfa_calc_size == TRUE)
	    {
		nstate++;
		നിർത്തുക;
	    }
	    e2 = എടുക്കുക_2();
	    e1 = എടുക്കുക_2();
	    s = alloc_state(NFA_SPLIT, e1.start, e2.start);
	    എങ്കിൽ (s == ശൂന്യ)
		പോകുക theend;
	    തള്ളുക_2(frag(s, append(e1.out, e2.out)));
	    നിർത്തുക;

	അവസ്ഥ NFA_STAR:
	    // Zero or more, prefer more
	    എങ്കിൽ (nfa_calc_size == TRUE)
	    {
		nstate++;
		നിർത്തുക;
	    }
	    e = എടുക്കുക_2();
	    s = alloc_state(NFA_SPLIT, e.start, ശൂന്യ);
	    എങ്കിൽ (s == ശൂന്യ)
		പോകുക theend;
	    patch(e.out, s);
	    തള്ളുക_2(frag(s, list1(&s->out1)));
	    നിർത്തുക;

	അവസ്ഥ NFA_STAR_NONGREEDY:
	    // Zero or more, prefer zero
	    എങ്കിൽ (nfa_calc_size == TRUE)
	    {
		nstate++;
		നിർത്തുക;
	    }
	    e = എടുക്കുക_2();
	    s = alloc_state(NFA_SPLIT, ശൂന്യ, e.start);
	    എങ്കിൽ (s == ശൂന്യ)
		പോകുക theend;
	    patch(e.out, s);
	    തള്ളുക_2(frag(s, list1(&s->out)));
	    നിർത്തുക;

	അവസ്ഥ NFA_QUEST:
	    // one or zero atoms=> greedy match
	    എങ്കിൽ (nfa_calc_size == TRUE)
	    {
		nstate++;
		നിർത്തുക;
	    }
	    e = എടുക്കുക_2();
	    s = alloc_state(NFA_SPLIT, e.start, ശൂന്യ);
	    എങ്കിൽ (s == ശൂന്യ)
		പോകുക theend;
	    തള്ളുക_2(frag(s, append(e.out, list1(&s->out1))));
	    നിർത്തുക;

	അവസ്ഥ NFA_QUEST_NONGREEDY:
	    // zero or one atoms => non-greedy match
	    എങ്കിൽ (nfa_calc_size == TRUE)
	    {
		nstate++;
		നിർത്തുക;
	    }
	    e = എടുക്കുക_2();
	    s = alloc_state(NFA_SPLIT, ശൂന്യ, e.start);
	    എങ്കിൽ (s == ശൂന്യ)
		പോകുക theend;
	    തള്ളുക_2(frag(s, append(e.out, list1(&s->out))));
	    നിർത്തുക;

	അവസ്ഥ NFA_END_COLL:
	അവസ്ഥ NFA_END_NEG_COLL:
	    // On the stack is the sequence starting with NFA_START_COLL or
	    // NFA_START_NEG_COLL and all possible characters. Patch it to
	    // add the output to the start.
	    എങ്കിൽ (nfa_calc_size == TRUE)
	    {
		nstate++;
		നിർത്തുക;
	    }
	    e = എടുക്കുക_2();
	    s = alloc_state(NFA_END_COLL, ശൂന്യ, ശൂന്യ);
	    എങ്കിൽ (s == ശൂന്യ)
		പോകുക theend;
	    patch(e.out, s);
	    e.start->out1 = s;
	    തള്ളുക_2(frag(e.start, list1(&s->out)));
	    നിർത്തുക;

	അവസ്ഥ NFA_RANGE:
	    // Before this are two characters, the low and high end of a
	    // range.  Turn them into two states with MIN and MAX.
	    എങ്കിൽ (nfa_calc_size == TRUE)
	    {
		// nstate += 0;
		നിർത്തുക;
	    }
	    e2 = എടുക്കുക_2();
	    e1 = എടുക്കുക_2();
	    e2.start->val = e2.start->c;
	    e2.start->c = NFA_RANGE_MAX;
	    e1.start->val = e1.start->c;
	    e1.start->c = NFA_RANGE_MIN;
	    patch(e1.out, e2.start);
	    തള്ളുക_2(frag(e1.start, e2.out));
	    നിർത്തുക;

	അവസ്ഥ NFA_EMPTY:
	    // 0-length, used in a repetition with max/min count of 0
	    എങ്കിൽ (nfa_calc_size == TRUE)
	    {
		nstate++;
		നിർത്തുക;
	    }
	    s = alloc_state(NFA_EMPTY, ശൂന്യ, ശൂന്യ);
	    എങ്കിൽ (s == ശൂന്യ)
		പോകുക theend;
	    തള്ളുക_2(frag(s, list1(&s->out)));
	    നിർത്തുക;

	അവസ്ഥ NFA_OPT_CHARS:
	  {
	    പൂർണ്ണസംഖ്യ    n;

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
	    എങ്കിൽ (nfa_calc_size == TRUE)
	    {
		nstate += n;
		നിർത്തുക;
	    }
	    s = ശൂന്യ; // avoid compiler warning
	    e1.out = ശൂന്യ; // stores list with out1's
	    s1 = ശൂന്യ; // previous NFA_SPLIT to connect to
	    ആകുമ്പോൾ (n-- > 0)
	    {
		e = എടുക്കുക_2(); // get character
		s = alloc_state(NFA_SPLIT, e.start, ശൂന്യ);
		എങ്കിൽ (s == ശൂന്യ)
		    പോകുക theend;
		എങ്കിൽ (e1.out == ശൂന്യ)
		    e1 = e;
		patch(e.out, s1);
		append(e1.out, list1(&s->out1));
		s1 = s;
	    }
	    തള്ളുക_2(frag(s, e1.out));
	    നിർത്തുക;
	  }

	അവസ്ഥ NFA_PREV_ATOM_NO_WIDTH:
	അവസ്ഥ NFA_PREV_ATOM_NO_WIDTH_NEG:
	അവസ്ഥ NFA_PREV_ATOM_JUST_BEFORE:
	അവസ്ഥ NFA_PREV_ATOM_JUST_BEFORE_NEG:
	അവസ്ഥ NFA_PREV_ATOM_LIKE_PATTERN:
	  {
	    പൂർണ്ണസംഖ്യ before = (*p == NFA_PREV_ATOM_JUST_BEFORE
				      || *p == NFA_PREV_ATOM_JUST_BEFORE_NEG);
	    പൂർണ്ണസംഖ്യ pattern = (*p == NFA_PREV_ATOM_LIKE_PATTERN);
	    പൂർണ്ണസംഖ്യ start_state;
	    പൂർണ്ണസംഖ്യ end_state;
	    പൂർണ്ണസംഖ്യ n = 0;
	    nfa_state_T *zend;
	    nfa_state_T *skip;

	    തിരഞ്ഞെടുപ്പ് (*p)
	    {
		അവസ്ഥ NFA_PREV_ATOM_NO_WIDTH:
		    start_state = NFA_START_INVISIBLE;
		    end_state = NFA_END_INVISIBLE;
		    നിർത്തുക;
		അവസ്ഥ NFA_PREV_ATOM_NO_WIDTH_NEG:
		    start_state = NFA_START_INVISIBLE_NEG;
		    end_state = NFA_END_INVISIBLE_NEG;
		    നിർത്തുക;
		അവസ്ഥ NFA_PREV_ATOM_JUST_BEFORE:
		    start_state = NFA_START_INVISIBLE_BEFORE;
		    end_state = NFA_END_INVISIBLE;
		    നിർത്തുക;
		അവസ്ഥ NFA_PREV_ATOM_JUST_BEFORE_NEG:
		    start_state = NFA_START_INVISIBLE_BEFORE_NEG;
		    end_state = NFA_END_INVISIBLE_NEG;
		    നിർത്തുക;
		സ്ഥിരസ്ഥിതി: // NFA_PREV_ATOM_LIKE_PATTERN:
		    start_state = NFA_START_PATTERN;
		    end_state = NFA_END_PATTERN;
		    നിർത്തുക;
	    }

	    എങ്കിൽ (before)
		n = *++p; // get the count

	    // The \@= operator: match the preceding atom with zero width.
	    // The \@! operator: no match for the preceding atom.
	    // The \@<= operator: match for the preceding atom.
	    // The \@<! operator: no match for the preceding atom.
	    // Surrounds the preceding atom with START_INVISIBLE and
	    // END_INVISIBLE, similarly to MOPEN.

	    എങ്കിൽ (nfa_calc_size == TRUE)
	    {
		nstate += pattern ? 4 : 2;
		നിർത്തുക;
	    }
	    e = എടുക്കുക_2();
	    s1 = alloc_state(end_state, ശൂന്യ, ശൂന്യ);
	    എങ്കിൽ (s1 == ശൂന്യ)
		പോകുക theend;

	    s = alloc_state(start_state, e.start, s1);
	    എങ്കിൽ (s == ശൂന്യ)
		പോകുക theend;
	    എങ്കിൽ (pattern)
	    {
		// NFA_ZEND -> NFA_END_PATTERN -> NFA_SKIP -> what follows.
		skip = alloc_state(NFA_SKIP, ശൂന്യ, ശൂന്യ);
		എങ്കിൽ (skip == ശൂന്യ)
		    പോകുക theend;
		zend = alloc_state(NFA_ZEND, s1, ശൂന്യ);
		എങ്കിൽ (zend == ശൂന്യ)
		    പോകുക theend;
		s1->out= skip;
		patch(e.out, zend);
		തള്ളുക_2(frag(s, list1(&skip->out)));
	    }
	    അല്ലെങ്കിൽ
	    {
		patch(e.out, s1);
		തള്ളുക_2(frag(s, list1(&s1->out)));
		എങ്കിൽ (before)
		{
		    എങ്കിൽ (n <= 0)
			// See if we can guess the maximum width, it avoids a
			// lot of pointless tries.
			n = nfa_max_width(e.start, 0);
		    s->val = n; // store the count
		}
	    }
	    നിർത്തുക;
	  }

	അവസ്ഥ NFA_COMPOSING:	// char with composing char
#എങ്കിൽ 0
	    // TODO
	    എങ്കിൽ (regflags & RF_ICOMBINE)
	    {
		// use the base character only
	    }
#endif
	    // FALLTHROUGH

	അവസ്ഥ NFA_MOPEN:	// \( \) Submatch
	അവസ്ഥ NFA_MOPEN1:
	അവസ്ഥ NFA_MOPEN2:
	അവസ്ഥ NFA_MOPEN3:
	അവസ്ഥ NFA_MOPEN4:
	അവസ്ഥ NFA_MOPEN5:
	അവസ്ഥ NFA_MOPEN6:
	അവസ്ഥ NFA_MOPEN7:
	അവസ്ഥ NFA_MOPEN8:
	അവസ്ഥ NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	അവസ്ഥ NFA_ZOPEN:	// \z( \) Submatch
	അവസ്ഥ NFA_ZOPEN1:
	അവസ്ഥ NFA_ZOPEN2:
	അവസ്ഥ NFA_ZOPEN3:
	അവസ്ഥ NFA_ZOPEN4:
	അവസ്ഥ NFA_ZOPEN5:
	അവസ്ഥ NFA_ZOPEN6:
	അവസ്ഥ NFA_ZOPEN7:
	അവസ്ഥ NFA_ZOPEN8:
	അവസ്ഥ NFA_ZOPEN9:
#endif
	അവസ്ഥ NFA_NOPEN:	// \%( \) "Invisible Submatch"
	    എങ്കിൽ (nfa_calc_size == TRUE)
	    {
		nstate += 2;
		നിർത്തുക;
	    }

	    mopen = *p;
	    തിരഞ്ഞെടുപ്പ് (*p)
	    {
		അവസ്ഥ NFA_NOPEN: mclose = NFA_NCLOSE; നിർത്തുക;
#ifdef FEAT_SYN_HL
		അവസ്ഥ NFA_ZOPEN: mclose = NFA_ZCLOSE; നിർത്തുക;
		അവസ്ഥ NFA_ZOPEN1: mclose = NFA_ZCLOSE1; നിർത്തുക;
		അവസ്ഥ NFA_ZOPEN2: mclose = NFA_ZCLOSE2; നിർത്തുക;
		അവസ്ഥ NFA_ZOPEN3: mclose = NFA_ZCLOSE3; നിർത്തുക;
		അവസ്ഥ NFA_ZOPEN4: mclose = NFA_ZCLOSE4; നിർത്തുക;
		അവസ്ഥ NFA_ZOPEN5: mclose = NFA_ZCLOSE5; നിർത്തുക;
		അവസ്ഥ NFA_ZOPEN6: mclose = NFA_ZCLOSE6; നിർത്തുക;
		അവസ്ഥ NFA_ZOPEN7: mclose = NFA_ZCLOSE7; നിർത്തുക;
		അവസ്ഥ NFA_ZOPEN8: mclose = NFA_ZCLOSE8; നിർത്തുക;
		അവസ്ഥ NFA_ZOPEN9: mclose = NFA_ZCLOSE9; നിർത്തുക;
#endif
		അവസ്ഥ NFA_COMPOSING: mclose = NFA_END_COMPOSING; നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		    // NFA_MOPEN, NFA_MOPEN1 .. NFA_MOPEN9
		    mclose = *p + NSUBEXP;
		    നിർത്തുക;
	    }

	    // Allow "NFA_MOPEN" as a valid postfix representation for
	    // the empty regexp "". In this case, the NFA will be
	    // NFA_MOPEN -> NFA_MCLOSE. Note that this also allows
	    // empty groups of parenthesis, and empty mbyte chars
	    എങ്കിൽ (stackp == അട്ടി)
	    {
		s = alloc_state(mopen, ശൂന്യ, ശൂന്യ);
		എങ്കിൽ (s == ശൂന്യ)
		    പോകുക theend;
		s1 = alloc_state(mclose, ശൂന്യ, ശൂന്യ);
		എങ്കിൽ (s1 == ശൂന്യ)
		    പോകുക theend;
		patch(list1(&s->out), s1);
		തള്ളുക_2(frag(s, list1(&s1->out)));
		നിർത്തുക;
	    }

	    // At least one node was emitted before NFA_MOPEN, so
	    // at least one node will be between NFA_MOPEN and NFA_MCLOSE
	    e = എടുക്കുക_2();
	    s = alloc_state(mopen, e.start, ശൂന്യ);   // `('
	    എങ്കിൽ (s == ശൂന്യ)
		പോകുക theend;

	    s1 = alloc_state(mclose, ശൂന്യ, ശൂന്യ);   // `)'
	    എങ്കിൽ (s1 == ശൂന്യ)
		പോകുക theend;
	    patch(e.out, s1);

	    എങ്കിൽ (mopen == NFA_COMPOSING)
		// COMPOSING->out1 = END_COMPOSING
		patch(list1(&s->out1), s1);

	    തള്ളുക_2(frag(s, list1(&s1->out)));
	    നിർത്തുക;

	അവസ്ഥ NFA_BACKREF1:
	അവസ്ഥ NFA_BACKREF2:
	അവസ്ഥ NFA_BACKREF3:
	അവസ്ഥ NFA_BACKREF4:
	അവസ്ഥ NFA_BACKREF5:
	അവസ്ഥ NFA_BACKREF6:
	അവസ്ഥ NFA_BACKREF7:
	അവസ്ഥ NFA_BACKREF8:
	അവസ്ഥ NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	അവസ്ഥ NFA_ZREF1:
	അവസ്ഥ NFA_ZREF2:
	അവസ്ഥ NFA_ZREF3:
	അവസ്ഥ NFA_ZREF4:
	അവസ്ഥ NFA_ZREF5:
	അവസ്ഥ NFA_ZREF6:
	അവസ്ഥ NFA_ZREF7:
	അവസ്ഥ NFA_ZREF8:
	അവസ്ഥ NFA_ZREF9:
#endif
	    എങ്കിൽ (nfa_calc_size == TRUE)
	    {
		nstate += 2;
		നിർത്തുക;
	    }
	    s = alloc_state(*p, ശൂന്യ, ശൂന്യ);
	    എങ്കിൽ (s == ശൂന്യ)
		പോകുക theend;
	    s1 = alloc_state(NFA_SKIP, ശൂന്യ, ശൂന്യ);
	    എങ്കിൽ (s1 == ശൂന്യ)
		പോകുക theend;
	    patch(list1(&s->out), s1);
	    തള്ളുക_2(frag(s, list1(&s1->out)));
	    നിർത്തുക;

	അവസ്ഥ NFA_LNUM:
	അവസ്ഥ NFA_LNUM_GT:
	അവസ്ഥ NFA_LNUM_LT:
	അവസ്ഥ NFA_VCOL:
	അവസ്ഥ NFA_VCOL_GT:
	അവസ്ഥ NFA_VCOL_LT:
	അവസ്ഥ NFA_COL:
	അവസ്ഥ NFA_COL_GT:
	അവസ്ഥ NFA_COL_LT:
	അവസ്ഥ NFA_MARK:
	അവസ്ഥ NFA_MARK_GT:
	അവസ്ഥ NFA_MARK_LT:
	  {
	    പൂർണ്ണസംഖ്യ n = *++p; // lnum, col or mark name

	    എങ്കിൽ (nfa_calc_size == TRUE)
	    {
		nstate += 1;
		നിർത്തുക;
	    }
	    s = alloc_state(p[-1], ശൂന്യ, ശൂന്യ);
	    എങ്കിൽ (s == ശൂന്യ)
		പോകുക theend;
	    s->val = n;
	    തള്ളുക_2(frag(s, list1(&s->out)));
	    നിർത്തുക;
	  }

	അവസ്ഥ NFA_ZSTART:
	അവസ്ഥ NFA_ZEND:
	സ്ഥിരസ്ഥിതി:
	    // Operands
	    എങ്കിൽ (nfa_calc_size == TRUE)
	    {
		nstate++;
		നിർത്തുക;
	    }
	    s = alloc_state(*p, ശൂന്യ, ശൂന്യ);
	    എങ്കിൽ (s == ശൂന്യ)
		പോകുക theend;
	    തള്ളുക_2(frag(s, list1(&s->out)));
	    നിർത്തുക;

	} // switch(*p)

    } // for (p = postfix; *p; ++p)

    എങ്കിൽ (nfa_calc_size == TRUE)
    {
	nstate++;
	പോകുക theend;	// Return value when counting size is ignored anyway
    }

    e = എടുക്കുക_2();
    എങ്കിൽ (stackp != അട്ടി)
	EMSG_RET_NULL(_(e_nfa_regexp_while_converting_from_postfix_to_nfa_too_many_stats_ഇടത്_on_അട്ടി));

    എങ്കിൽ (istate >= nstate)
	EMSG_RET_NULL(_(e_nfa_regexp_not_enough_space_to_store_whole_nfa));

    matchstate = &state_ptr[istate++]; // the match state
    matchstate->c = NFA_MATCH;
    matchstate->out = matchstate->out1 = ശൂന്യ;
    matchstate->id = 0;

    patch(e.out, matchstate);
    ret = e.start;

theend:
    തിരികെ ret;

#undef POP1
#undef PUSH1
#undef POP2
#undef PUSH2
#undef എടുക്കുക_2
#undef തള്ളുക_2
}

/*
 * After building the NFA program, inspect it to add optimization hints.
 */
    സ്ഥിതം ശൂന്യം
nfa_postprocess(nfa_regprog_T *prog)
{
    പൂർണ്ണസംഖ്യ i;
    പൂർണ്ണസംഖ്യ c;

    വേണ്ടി (i = 0; i < prog->nstate; ++i)
    {
	c = prog->state[i].c;
	എങ്കിൽ (c == NFA_START_INVISIBLE
		|| c == NFA_START_INVISIBLE_NEG
		|| c == NFA_START_INVISIBLE_BEFORE
		|| c == NFA_START_INVISIBLE_BEFORE_NEG)
	{
	    പൂർണ്ണസംഖ്യ directly;

	    // Do it directly when what follows is possibly the end of the
	    // match.
	    എങ്കിൽ (match_follows(prog->state[i].out1->out, 0))
		directly = TRUE;
	    അല്ലെങ്കിൽ
	    {
		പൂർണ്ണസംഖ്യ ch_invisible = failure_chance(prog->state[i].out, 0);
		പൂർണ്ണസംഖ്യ ch_follows = failure_chance(prog->state[i].out1->out, 0);

		// Postpone when the invisible match is expensive or has a
		// lower chance of failing.
		എങ്കിൽ (c == NFA_START_INVISIBLE_BEFORE
		     || c == NFA_START_INVISIBLE_BEFORE_NEG)
		{
		    // "before" matches are very expensive when
		    // unbounded, always prefer what follows then,
		    // unless what follows will always match.
		    // Otherwise strongly prefer what follows.
		    എങ്കിൽ (prog->state[i].val <= 0 && ch_follows > 0)
			directly = FALSE;
		    അല്ലെങ്കിൽ
			directly = ch_follows * 10 < ch_invisible;
		}
		അല്ലെങ്കിൽ
		{
		    // normal invisible, first do the one with the
		    // highest failure chance
		    directly = ch_follows < ch_invisible;
		}
	    }
	    എങ്കിൽ (directly)
		// switch to the _FIRST state
		++prog->state[i].c;
	}
    }
}

/////////////////////////////////////////////////////////////////
// NFA execution code.
/////////////////////////////////////////////////////////////////

തരനിർവചനം ഘടന
{
    പൂർണ്ണസംഖ്യ	    in_use; // number of subexpr with useful info

    // When REG_MULTI is TRUE list.multi is used, otherwise list.line.
    സംഘം
    {
	ഘടന multipos
	{
	    linenr_T	start_lnum;
	    linenr_T	end_lnum;
	    colnr_T	start_col;
	    colnr_T	end_col;
	} multi[NSUBEXP];
	ഘടന linepos
	{
	    char_u	*start;
	    char_u	*end;
	} വരി[NSUBEXP];
    } list;
    colnr_T	orig_start_col;  // list.multi[0].start_col without \zs
} regsub_T;

തരനിർവചനം ഘടന
{
    regsub_T	norm; // \( .. \) matches
#ifdef FEAT_SYN_HL
    regsub_T	synt; // \z( .. \) matches
#endif
} regsubs_T;

// nfa_pim_T stores a Postponed Invisible Match.
തരനിർവചനം ഘടന nfa_pim_S nfa_pim_T;
ഘടന nfa_pim_S
{
    പൂർണ്ണസംഖ്യ		result;		// NFA_PIM_*, see below
    nfa_state_T	*state;		// the invisible match start state
    regsubs_T	subs;		// submatch info, only party used
    സംഘം
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
തരനിർവചനം ഘടന
{
    nfa_state_T	*state;
    പൂർണ്ണസംഖ്യ		എണ്ണം;
    nfa_pim_T	pim;		// if pim.result != NFA_PIM_UNUSED: postponed
				// invisible match
    regsubs_T	subs;		// submatch info, only party used
} nfa_thread_T;

// nfa_list_T contains the alternative NFA execution states.
തരനിർവചനം ഘടന
{
    nfa_thread_T    *t;		// allocated array of states
    പൂർണ്ണസംഖ്യ		    n;		// nr of states currently in "t"
    പൂർണ്ണസംഖ്യ		    len;	// max nr of states in "t"
    പൂർണ്ണസംഖ്യ		    id;		// ID of the list
    പൂർണ്ണസംഖ്യ		    has_pim;	// TRUE when any state has a PIM
} nfa_list_T;

#ifdef ENABLE_LOG
സ്ഥിതം ശൂന്യം log_subexpr(regsub_T *sub);

    സ്ഥിതം ശൂന്യം
log_subsexpr(regsubs_T *subs)
{
    log_subexpr(&subs->norm);
# ifdef FEAT_SYN_HL
    എങ്കിൽ (rex.nfa_has_zsubexpr)
	log_subexpr(&subs->synt);
# endif
}

    സ്ഥിതം ശൂന്യം
log_subexpr(regsub_T *sub)
{
    പൂർണ്ണസംഖ്യ j;

    വേണ്ടി (j = 0; j < sub->in_use; j++)
	എങ്കിൽ (REG_MULTI)
	    fprintf(log_fd,
		    "*** group %d, start: c=%d, l=%d, end: c=%d, l=%d\n",
		    j,
		    sub->list.multi[j].start_col,
		    (പൂർണ്ണസംഖ്യ)sub->list.multi[j].start_lnum,
		    sub->list.multi[j].end_col,
		    (പൂർണ്ണസംഖ്യ)sub->list.multi[j].end_lnum);
	അല്ലെങ്കിൽ
	{
	    അക്ഷരം *s = (അക്ഷരം *)sub->list.വരി[j].start;
	    അക്ഷരം *e = (അക്ഷരം *)sub->list.വരി[j].end;

	    fprintf(log_fd, "*** group %d, start: \"%s\", end: \"%s\"\n",
		    j,
		    s == ശൂന്യ ? "NULL" : s,
		    e == ശൂന്യ ? "NULL" : e);
	}
}

    സ്ഥിതം അക്ഷരം *
pim_info(nfa_pim_T *pim)
{
    സ്ഥിതം അക്ഷരം buf[30];

    എങ്കിൽ (pim == ശൂന്യ || pim->result == NFA_PIM_UNUSED)
	buf[0] = NUL;
    അല്ലെങ്കിൽ
    {
	sprintf(buf, " PIM col %d", REG_MULTI ? (പൂർണ്ണസംഖ്യ)pim->end.pos.col
		: (പൂർണ്ണസംഖ്യ)(pim->end.ptr - rex.നിവേശം));
    }
    തിരികെ buf;
}

#endif

// Used during execution: whether a match has been found.
സ്ഥിതം പൂർണ്ണസംഖ്യ	    nfa_match;
#ifdef FEAT_RELTIME
സ്ഥിതം പൂർണ്ണസംഖ്യ	   *nfa_timed_out;
#endif

സ്ഥിതം വരിയിൽ ശൂന്യം copy_sub(regsub_T *to, regsub_T *from);
സ്ഥിതം പൂർണ്ണസംഖ്യ pim_equal(nfa_pim_T *one, nfa_pim_T *two);

/*
 * Copy postponed invisible match info from "from" to "to".
 */
    സ്ഥിതം ശൂന്യം
copy_pim(nfa_pim_T *to, nfa_pim_T *from)
{
    to->result = from->result;
    to->state = from->state;
    copy_sub(&to->subs.norm, &from->subs.norm);
#ifdef FEAT_SYN_HL
    എങ്കിൽ (rex.nfa_has_zsubexpr)
	copy_sub(&to->subs.synt, &from->subs.synt);
#endif
    to->end = from->end;
}

    സ്ഥിതം ശൂന്യം
clear_sub(regsub_T *sub)
{
    എങ്കിൽ (REG_MULTI)
	// Use 0xff to set lnum to -1
	vim_memset(sub->list.multi, 0xff,
				  വലുപ്പം(ഘടന multipos) * rex.nfa_nsubexpr);
    അല്ലെങ്കിൽ
	vim_memset(sub->list.വരി, 0,
				   വലുപ്പം(ഘടന linepos) * rex.nfa_nsubexpr);
    sub->in_use = 0;
}

/*
 * Copy the submatches from "from" to "to".
 */
    സ്ഥിതം വരിയിൽ ശൂന്യം
copy_sub(regsub_T *to, regsub_T *from)
{
    to->in_use = from->in_use;
    എങ്കിൽ (from->in_use <= 0)
	തിരികെ;

    // Copy the match start and end positions.  addstate() calls this for
    // every state it adds, so it is very hot.  Patterns without capturing
    // groups only use subexpression zero (the whole match), so special-case
    // "in_use == 1" with a direct assignment to avoid the mch_memmove() call.
    എങ്കിൽ (REG_MULTI)
    {
	എങ്കിൽ (from->in_use == 1)
	    to->list.multi[0] = from->list.multi[0];
	അല്ലെങ്കിൽ
	    mch_memmove(&to->list.multi[0],
		    &from->list.multi[0],
		    വലുപ്പം(ഘടന multipos) * from->in_use);
	to->orig_start_col = from->orig_start_col;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (from->in_use == 1)
	    to->list.വരി[0] = from->list.വരി[0];
	അല്ലെങ്കിൽ
	    mch_memmove(&to->list.വരി[0],
		    &from->list.വരി[0],
		    വലുപ്പം(ഘടന linepos) * from->in_use);
    }
}

/*
 * Like copy_sub() but exclude the main match.
 */
    സ്ഥിതം ശൂന്യം
copy_sub_off(regsub_T *to, regsub_T *from)
{
    എങ്കിൽ (to->in_use < from->in_use)
	to->in_use = from->in_use;
    എങ്കിൽ (from->in_use <= 1)
	തിരികെ;

    // Copy the match start and end positions.
    എങ്കിൽ (REG_MULTI)
	mch_memmove(&to->list.multi[1],
		&from->list.multi[1],
		വലുപ്പം(ഘടന multipos) * (from->in_use - 1));
    അല്ലെങ്കിൽ
	mch_memmove(&to->list.വരി[1],
		&from->list.വരി[1],
		വലുപ്പം(ഘടന linepos) * (from->in_use - 1));
}

/*
 * Like copy_sub() but only do the end of the main match if \ze is present.
 */
    സ്ഥിതം ശൂന്യം
copy_ze_off(regsub_T *to, regsub_T *from)
{
    എങ്കിൽ (!rex.nfa_has_zend)
	തിരികെ;

    എങ്കിൽ (REG_MULTI)
    {
	എങ്കിൽ (from->list.multi[0].end_lnum >= 0)
	{
	    to->list.multi[0].end_lnum = from->list.multi[0].end_lnum;
	    to->list.multi[0].end_col = from->list.multi[0].end_col;
	}
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (from->list.വരി[0].end != ശൂന്യ)
	    to->list.വരി[0].end = from->list.വരി[0].end;
    }
}

/*
 * Return TRUE if "sub1" and "sub2" have the same start positions.
 * When using back-references also check the end position.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
sub_equal(regsub_T *sub1, regsub_T *sub2)
{
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		todo;
    linenr_T	s1;
    linenr_T	s2;
    char_u	*sp1;
    char_u	*sp2;

    todo = sub1->in_use > sub2->in_use ? sub1->in_use : sub2->in_use;
    എങ്കിൽ (REG_MULTI)
    {
	വേണ്ടി (i = 0; i < todo; ++i)
	{
	    എങ്കിൽ (i < sub1->in_use)
		s1 = sub1->list.multi[i].start_lnum;
	    അല്ലെങ്കിൽ
		s1 = -1;
	    എങ്കിൽ (i < sub2->in_use)
		s2 = sub2->list.multi[i].start_lnum;
	    അല്ലെങ്കിൽ
		s2 = -1;
	    എങ്കിൽ (s1 != s2)
		തിരികെ FALSE;
	    എങ്കിൽ (s1 != -1 && sub1->list.multi[i].start_col
					     != sub2->list.multi[i].start_col)
		തിരികെ FALSE;

	    എങ്കിൽ (rex.nfa_has_backref)
	    {
		എങ്കിൽ (i < sub1->in_use)
		    s1 = sub1->list.multi[i].end_lnum;
		അല്ലെങ്കിൽ
		    s1 = -1;
		എങ്കിൽ (i < sub2->in_use)
		    s2 = sub2->list.multi[i].end_lnum;
		അല്ലെങ്കിൽ
		    s2 = -1;
		എങ്കിൽ (s1 != s2)
		    തിരികെ FALSE;
		എങ്കിൽ (s1 != -1 && sub1->list.multi[i].end_col
					       != sub2->list.multi[i].end_col)
		തിരികെ FALSE;
	    }
	}
    }
    അല്ലെങ്കിൽ
    {
	വേണ്ടി (i = 0; i < todo; ++i)
	{
	    എങ്കിൽ (i < sub1->in_use)
		sp1 = sub1->list.വരി[i].start;
	    അല്ലെങ്കിൽ
		sp1 = ശൂന്യ;
	    എങ്കിൽ (i < sub2->in_use)
		sp2 = sub2->list.വരി[i].start;
	    അല്ലെങ്കിൽ
		sp2 = ശൂന്യ;
	    എങ്കിൽ (sp1 != sp2)
		തിരികെ FALSE;
	    എങ്കിൽ (rex.nfa_has_backref)
	    {
		എങ്കിൽ (i < sub1->in_use)
		    sp1 = sub1->list.വരി[i].end;
		അല്ലെങ്കിൽ
		    sp1 = ശൂന്യ;
		എങ്കിൽ (i < sub2->in_use)
		    sp2 = sub2->list.വരി[i].end;
		അല്ലെങ്കിൽ
		    sp2 = ശൂന്യ;
		എങ്കിൽ (sp1 != sp2)
		    തിരികെ FALSE;
	    }
	}
    }

    തിരികെ TRUE;
}

#ifdef FEAT_RELTIME
/*
 * Check if we are past the time limit, if there is one.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
nfa_did_time_out(ശൂന്യം)
{
    എങ്കിൽ (*timeout_flag)
    {
	എങ്കിൽ (nfa_timed_out != ശൂന്യ)
	{
# ifdef FEAT_EVAL
	    എങ്കിൽ (!*nfa_timed_out)
		ch_log(ശൂന്യ, "NFA regexp timed out");
# endif
	    *nfa_timed_out = TRUE;
	}
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}
#endif

#ifdef ENABLE_LOG
    സ്ഥിതം ശൂന്യം
open_debug_log(പൂർണ്ണസംഖ്യ result)
{
    log_fd = fopen(NFA_REGEXP_RUN_LOG, "a");
    എങ്കിൽ (log_fd == ശൂന്യ)
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

    സ്ഥിതം ശൂന്യം
report_state(അക്ഷരം *action,
	     regsub_T *sub,
	     nfa_state_T *state,
	     പൂർണ്ണസംഖ്യ lid,
	     nfa_pim_T *pim)
{
    പൂർണ്ണസംഖ്യ col;

    എങ്കിൽ (sub->in_use <= 0)
	col = -1;
    അല്ലെങ്കിൽ എങ്കിൽ (REG_MULTI)
	col = sub->list.multi[0].start_col;
    അല്ലെങ്കിൽ
	col = (പൂർണ്ണസംഖ്യ)(sub->list.വരി[0].start - rex.വരി);
    nfa_set_code(state->c);
    എങ്കിൽ (log_fd == ശൂന്യ)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
has_state_with_pos(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim)	// postponed match or NULL
{
    nfa_thread_T	*thread;
    പൂർണ്ണസംഖ്യ			i;

    വേണ്ടി (i = 0; i < l->n; ++i)
    {
	thread = &l->t[i];
	എങ്കിൽ (thread->state->id == state->id
		&& sub_equal(&thread->subs.norm, &subs->norm)
#ifdef FEAT_SYN_HL
		&& (!rex.nfa_has_zsubexpr
				|| sub_equal(&thread->subs.synt, &subs->synt))
#endif
		&& pim_equal(&thread->pim, pim))
	    തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Return TRUE if "one" and "two" are equal.  That includes when both are not
 * set.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
pim_equal(nfa_pim_T *one, nfa_pim_T *two)
{
    പൂർണ്ണസംഖ്യ one_unused = (one == ശൂന്യ || one->result == NFA_PIM_UNUSED);
    പൂർണ്ണസംഖ്യ two_unused = (two == ശൂന്യ || two->result == NFA_PIM_UNUSED);

    എങ്കിൽ (one_unused)
	// one is unused: equal when two is also unused
	തിരികെ two_unused;
    എങ്കിൽ (two_unused)
	// one is used and two is not: not equal
	തിരികെ FALSE;
    // compare the state id
    എങ്കിൽ (one->state->id != two->state->id)
	തിരികെ FALSE;
    // compare the position
    എങ്കിൽ (REG_MULTI)
	തിരികെ one->end.pos.lnum == two->end.pos.lnum
	    && one->end.pos.col == two->end.pos.col;
    തിരികെ one->end.ptr == two->end.ptr;
}

/*
 * Return TRUE if "state" leads to a NFA_MATCH without advancing the input.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
match_follows(nfa_state_T *startstate, പൂർണ്ണസംഖ്യ depth)
{
    nfa_state_T	    *state = startstate;

    // avoid too much recursion
    എങ്കിൽ (depth > 10)
	തിരികെ FALSE;

    ആകുമ്പോൾ (state != ശൂന്യ)
    {
	തിരഞ്ഞെടുപ്പ് (state->c)
	{
	    അവസ്ഥ NFA_MATCH:
	    അവസ്ഥ NFA_MCLOSE:
	    അവസ്ഥ NFA_END_INVISIBLE:
	    അവസ്ഥ NFA_END_INVISIBLE_NEG:
	    അവസ്ഥ NFA_END_PATTERN:
		തിരികെ TRUE;

	    അവസ്ഥ NFA_SPLIT:
		തിരികെ match_follows(state->out, depth + 1)
				     || match_follows(state->out1, depth + 1);

	    അവസ്ഥ NFA_START_INVISIBLE:
	    അവസ്ഥ NFA_START_INVISIBLE_FIRST:
	    അവസ്ഥ NFA_START_INVISIBLE_BEFORE:
	    അവസ്ഥ NFA_START_INVISIBLE_BEFORE_FIRST:
	    അവസ്ഥ NFA_START_INVISIBLE_NEG:
	    അവസ്ഥ NFA_START_INVISIBLE_NEG_FIRST:
	    അവസ്ഥ NFA_START_INVISIBLE_BEFORE_NEG:
	    അവസ്ഥ NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
	    അവസ്ഥ NFA_COMPOSING:
		// skip ahead to next state
		state = state->out1->out;
		തുടരുക;

	    അവസ്ഥ NFA_ANY:
	    അവസ്ഥ NFA_ANY_COMPOSING:
	    അവസ്ഥ NFA_IDENT:
	    അവസ്ഥ NFA_SIDENT:
	    അവസ്ഥ NFA_KWORD:
	    അവസ്ഥ NFA_SKWORD:
	    അവസ്ഥ NFA_FNAME:
	    അവസ്ഥ NFA_SFNAME:
	    അവസ്ഥ NFA_PRINT:
	    അവസ്ഥ NFA_SPRINT:
	    അവസ്ഥ NFA_WHITE:
	    അവസ്ഥ NFA_NWHITE:
	    അവസ്ഥ NFA_DIGIT:
	    അവസ്ഥ NFA_NDIGIT:
	    അവസ്ഥ NFA_HEX:
	    അവസ്ഥ NFA_NHEX:
	    അവസ്ഥ NFA_OCTAL:
	    അവസ്ഥ NFA_NOCTAL:
	    അവസ്ഥ NFA_WORD:
	    അവസ്ഥ NFA_NWORD:
	    അവസ്ഥ NFA_HEAD:
	    അവസ്ഥ NFA_NHEAD:
	    അവസ്ഥ NFA_ALPHA:
	    അവസ്ഥ NFA_NALPHA:
	    അവസ്ഥ NFA_LOWER:
	    അവസ്ഥ NFA_NLOWER:
	    അവസ്ഥ NFA_UPPER:
	    അവസ്ഥ NFA_NUPPER:
	    അവസ്ഥ NFA_LOWER_IC:
	    അവസ്ഥ NFA_NLOWER_IC:
	    അവസ്ഥ NFA_UPPER_IC:
	    അവസ്ഥ NFA_NUPPER_IC:
	    അവസ്ഥ NFA_START_COLL:
	    അവസ്ഥ NFA_START_NEG_COLL:
	    അവസ്ഥ NFA_NEWL:
		// state will advance input
		തിരികെ FALSE;

	    സ്ഥിരസ്ഥിതി:
		എങ്കിൽ (state->c > 0)
		    // state will advance input
		    തിരികെ FALSE;

		// Others: zero-width or possibly zero-width, might still find
		// a match at the same position, keep looking.
		നിർത്തുക;
	}
	state = state->out;
    }
    തിരികെ FALSE;
}


/*
 * Return TRUE if "state" is already in list "l".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
state_in_list(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs)	// pointers to subexpressions
{
    എങ്കിൽ (state->lastlist[nfa_ll_index] == l->id)
    {
	എങ്കിൽ (!rex.nfa_has_backref || has_state_with_pos(l, state, subs, ശൂന്യ))
	    തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

// Offset used for "off" by addstate_here().
#define ADDSTATE_HERE_OFFSET 10

/*
 * Add "state" and possibly what follows to state list ".".
 * Returns "subs_arg", possibly copied into temp_subs.
 * Returns NULL when recursiveness is too deep or timed out.
 */
    സ്ഥിതം regsubs_T *
addstate(
    nfa_list_T		*l,	    // runtime state list
    nfa_state_T		*state,	    // state to update
    regsubs_T		*subs_arg,  // pointers to subexpressions
    nfa_pim_T		*pim,	    // postponed look-behind match
    പൂർണ്ണസംഖ്യ			off_arg,    // byte offset, when -1 go to next line
    പൂർണ്ണസംഖ്യ			depth)	    // recursion depth
{
    പൂർണ്ണസംഖ്യ			subidx;
    പൂർണ്ണസംഖ്യ			off = off_arg;
    പൂർണ്ണസംഖ്യ			add_here = FALSE;
    പൂർണ്ണസംഖ്യ			listindex = 0;
    പൂർണ്ണസംഖ്യ			k;
    പൂർണ്ണസംഖ്യ			found = FALSE;
    nfa_thread_T	*thread;
    ഘടന multipos	save_multipos;
    പൂർണ്ണസംഖ്യ			save_in_use;
    char_u		*save_ptr;
    പൂർണ്ണസംഖ്യ			i;
    regsub_T		*sub;
    regsubs_T		*subs = subs_arg;
    സ്ഥിതം regsubs_T	temp_subs;
#ifdef ENABLE_LOG
    പൂർണ്ണസംഖ്യ			did_print = FALSE;
#endif

#ifdef FEAT_RELTIME
    എങ്കിൽ (nfa_did_time_out())
	തിരികെ ശൂന്യ;
#endif

    // This function is called recursively.  When the depth is too much we run
    // out of stack and crash, limit recursiveness here.
    എങ്കിൽ (depth >= 5000 || subs == ശൂന്യ)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (off_arg <= -ADDSTATE_HERE_OFFSET)
    {
	add_here = TRUE;
	off = 0;
	listindex = -(off_arg + ADDSTATE_HERE_OFFSET);
    }

    തിരഞ്ഞെടുപ്പ് (state->c)
    {
	അവസ്ഥ NFA_NCLOSE:
	അവസ്ഥ NFA_MCLOSE:
	അവസ്ഥ NFA_MCLOSE1:
	അവസ്ഥ NFA_MCLOSE2:
	അവസ്ഥ NFA_MCLOSE3:
	അവസ്ഥ NFA_MCLOSE4:
	അവസ്ഥ NFA_MCLOSE5:
	അവസ്ഥ NFA_MCLOSE6:
	അവസ്ഥ NFA_MCLOSE7:
	അവസ്ഥ NFA_MCLOSE8:
	അവസ്ഥ NFA_MCLOSE9:
#ifdef FEAT_SYN_HL
	അവസ്ഥ NFA_ZCLOSE:
	അവസ്ഥ NFA_ZCLOSE1:
	അവസ്ഥ NFA_ZCLOSE2:
	അവസ്ഥ NFA_ZCLOSE3:
	അവസ്ഥ NFA_ZCLOSE4:
	അവസ്ഥ NFA_ZCLOSE5:
	അവസ്ഥ NFA_ZCLOSE6:
	അവസ്ഥ NFA_ZCLOSE7:
	അവസ്ഥ NFA_ZCLOSE8:
	അവസ്ഥ NFA_ZCLOSE9:
#endif
	അവസ്ഥ NFA_MOPEN:
	അവസ്ഥ NFA_ZEND:
	അവസ്ഥ NFA_SPLIT:
	അവസ്ഥ NFA_EMPTY:
	    // These nodes are not added themselves but their "out" and/or
	    // "out1" may be added below.
	    നിർത്തുക;

	അവസ്ഥ NFA_BOL:
	അവസ്ഥ NFA_BOF:
	    // "^" won't match past end-of-line, don't bother trying.
	    // Except when at the end of the line, or when we are going to the
	    // next line for a look-behind match.
	    എങ്കിൽ (rex.നിവേശം > rex.വരി
		    && *rex.നിവേശം != NUL
		    && (nfa_endp == ശൂന്യ
			|| !REG_MULTI
			|| rex.lnum == nfa_endp->se_u.pos.lnum))
		പോകുക skip_add;
	    // FALLTHROUGH

	അവസ്ഥ NFA_MOPEN1:
	അവസ്ഥ NFA_MOPEN2:
	അവസ്ഥ NFA_MOPEN3:
	അവസ്ഥ NFA_MOPEN4:
	അവസ്ഥ NFA_MOPEN5:
	അവസ്ഥ NFA_MOPEN6:
	അവസ്ഥ NFA_MOPEN7:
	അവസ്ഥ NFA_MOPEN8:
	അവസ്ഥ NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	അവസ്ഥ NFA_ZOPEN:
	അവസ്ഥ NFA_ZOPEN1:
	അവസ്ഥ NFA_ZOPEN2:
	അവസ്ഥ NFA_ZOPEN3:
	അവസ്ഥ NFA_ZOPEN4:
	അവസ്ഥ NFA_ZOPEN5:
	അവസ്ഥ NFA_ZOPEN6:
	അവസ്ഥ NFA_ZOPEN7:
	അവസ്ഥ NFA_ZOPEN8:
	അവസ്ഥ NFA_ZOPEN9:
#endif
	അവസ്ഥ NFA_NOPEN:
	അവസ്ഥ NFA_ZSTART:
	    // These nodes need to be added so that we can bail out when it
	    // was added to this list before at the same position to avoid an
	    // endless loop for "\(\)*"

	സ്ഥിരസ്ഥിതി:
	    എങ്കിൽ (state->lastlist[nfa_ll_index] == l->id && state->c != NFA_SKIP)
	    {
		// This state is already in the list, don't add it again,
		// unless it is an MOPEN that is used for a backreference or
		// when there is a PIM. For NFA_MATCH check the position,
		// lower position is preferred.
		എങ്കിൽ (!rex.nfa_has_backref && pim == ശൂന്യ && !l->has_pim
						     && state->c != NFA_MATCH)
		{
		    // When called from addstate_here() do insert before
		    // existing states.
		    എങ്കിൽ (add_here)
		    {
			വേണ്ടി (k = 0; k < l->n && k < listindex; ++k)
			    എങ്കിൽ (l->t[k].state->id == state->id)
			    {
				found = TRUE;
				നിർത്തുക;
			    }
		    }
		    എങ്കിൽ (!add_here || found)
		    {
skip_add:
#ifdef ENABLE_LOG
			nfa_set_code(state->c);
			fprintf(log_fd, "> Not adding state %d to list %d. char %d: %s pim: %s has_pim: %d found: %d\n",
			    abs(state->id), l->id, state->c, code,
			    pim == ശൂന്യ ? "NULL" : "yes", l->has_pim, found);
#endif
			തിരികെ subs;
		    }
		}

		// Do not add the state again when it exists with the same
		// positions.
		എങ്കിൽ (has_state_with_pos(l, state, subs, pim))
		    പോകുക skip_add;
	    }

	    // When there are backreferences or PIMs the number of states may
	    // be (a lot) bigger than anticipated.
	    എങ്കിൽ (l->n == l->len)
	    {
		പൂർണ്ണസംഖ്യ		newlen = l->len * 3 / 2 + 50;
		size_t		newsize = newlen * വലുപ്പം(nfa_thread_T);
		nfa_thread_T	*newt;

		എങ്കിൽ ((long)(newsize >> 10) >= p_mmp)
		{
		    emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		    തിരികെ ശൂന്യ;
		}
		എങ്കിൽ (subs != &temp_subs)
		{
		    // "subs" may point into the current array, need to make a
		    // copy before it becomes invalid.
		    copy_sub(&temp_subs.norm, &subs->norm);
#ifdef FEAT_SYN_HL
		    എങ്കിൽ (rex.nfa_has_zsubexpr)
			copy_sub(&temp_subs.synt, &subs->synt);
#endif
		    subs = &temp_subs;
		}

		newt = vim_realloc(l->t, newsize);
		എങ്കിൽ (newt == ശൂന്യ)
		{
		    // out of memory
		    തിരികെ ശൂന്യ;
		}
		l->t = newt;
		l->len = newlen;
	    }

	    // add the state to the list
	    state->lastlist[nfa_ll_index] = l->id;
	    thread = &l->t[l->n++];
	    thread->state = state;
	    എങ്കിൽ (pim == ശൂന്യ)
		thread->pim.result = NFA_PIM_UNUSED;
	    അല്ലെങ്കിൽ
	    {
		copy_pim(&thread->pim, pim);
		l->has_pim = TRUE;
	    }
	    copy_sub(&thread->subs.norm, &subs->norm);
#ifdef FEAT_SYN_HL
	    എങ്കിൽ (rex.nfa_has_zsubexpr)
		copy_sub(&thread->subs.synt, &subs->synt);
#endif
#ifdef ENABLE_LOG
	    report_state("Adding", &thread->subs.norm, state, l->id, pim);
	    did_print = TRUE;
#endif
    }

#ifdef ENABLE_LOG
    എങ്കിൽ (!did_print)
	report_state("Processing", &subs->norm, state, l->id, pim);
#endif
    തിരഞ്ഞെടുപ്പ് (state->c)
    {
	അവസ്ഥ NFA_MATCH:
	    നിർത്തുക;

	അവസ്ഥ NFA_SPLIT:
	    // order matters here
	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    subs = addstate(l, state->out1, subs, pim, off_arg, depth + 1);
	    നിർത്തുക;

	അവസ്ഥ NFA_EMPTY:
	അവസ്ഥ NFA_NOPEN:
	അവസ്ഥ NFA_NCLOSE:
	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    നിർത്തുക;

	അവസ്ഥ NFA_MOPEN:
	അവസ്ഥ NFA_MOPEN1:
	അവസ്ഥ NFA_MOPEN2:
	അവസ്ഥ NFA_MOPEN3:
	അവസ്ഥ NFA_MOPEN4:
	അവസ്ഥ NFA_MOPEN5:
	അവസ്ഥ NFA_MOPEN6:
	അവസ്ഥ NFA_MOPEN7:
	അവസ്ഥ NFA_MOPEN8:
	അവസ്ഥ NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	അവസ്ഥ NFA_ZOPEN:
	അവസ്ഥ NFA_ZOPEN1:
	അവസ്ഥ NFA_ZOPEN2:
	അവസ്ഥ NFA_ZOPEN3:
	അവസ്ഥ NFA_ZOPEN4:
	അവസ്ഥ NFA_ZOPEN5:
	അവസ്ഥ NFA_ZOPEN6:
	അവസ്ഥ NFA_ZOPEN7:
	അവസ്ഥ NFA_ZOPEN8:
	അവസ്ഥ NFA_ZOPEN9:
#endif
	അവസ്ഥ NFA_ZSTART:
	    എങ്കിൽ (state->c == NFA_ZSTART)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#ifdef FEAT_SYN_HL
	    അല്ലെങ്കിൽ എങ്കിൽ (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
	    {
		subidx = state->c - NFA_ZOPEN;
		sub = &subs->synt;
	    }
#endif
	    അല്ലെങ്കിൽ
	    {
		subidx = state->c - NFA_MOPEN;
		sub = &subs->norm;
	    }

	    // avoid compiler warnings
	    save_ptr = ശൂന്യ;
	    CLEAR_FIELD(save_multipos);

	    // Set the position (with "off" added) in the subexpression.  Save
	    // and restore it when it was in use.  Otherwise fill any gap.
	    എങ്കിൽ (REG_MULTI)
	    {
		എങ്കിൽ (subidx < sub->in_use)
		{
		    save_multipos = sub->list.multi[subidx];
		    save_in_use = -1;
		}
		അല്ലെങ്കിൽ
		{
		    save_in_use = sub->in_use;
		    വേണ്ടി (i = sub->in_use; i < subidx; ++i)
		    {
			sub->list.multi[i].start_lnum = -1;
			sub->list.multi[i].end_lnum = -1;
		    }
		    sub->in_use = subidx + 1;
		}
		എങ്കിൽ (off == -1)
		{
		    sub->list.multi[subidx].start_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].start_col = 0;
		}
		അല്ലെങ്കിൽ
		{
		    sub->list.multi[subidx].start_lnum = rex.lnum;
		    sub->list.multi[subidx].start_col =
					 (colnr_T)(rex.നിവേശം - rex.വരി + off);
		}
		sub->list.multi[subidx].end_lnum = -1;
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (subidx < sub->in_use)
		{
		    save_ptr = sub->list.വരി[subidx].start;
		    save_in_use = -1;
		}
		അല്ലെങ്കിൽ
		{
		    save_in_use = sub->in_use;
		    വേണ്ടി (i = sub->in_use; i < subidx; ++i)
		    {
			sub->list.വരി[i].start = ശൂന്യ;
			sub->list.വരി[i].end = ശൂന്യ;
		    }
		    sub->in_use = subidx + 1;
		}
		sub->list.വരി[subidx].start = rex.നിവേശം + off;
	    }

	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    എങ്കിൽ (subs == ശൂന്യ)
		നിർത്തുക;
	    // "subs" may have changed, need to set "sub" again
#ifdef FEAT_SYN_HL
	    എങ്കിൽ (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
		sub = &subs->synt;
	    അല്ലെങ്കിൽ
#endif
		sub = &subs->norm;

	    എങ്കിൽ (save_in_use == -1)
	    {
		എങ്കിൽ (REG_MULTI)
		    sub->list.multi[subidx] = save_multipos;
		അല്ലെങ്കിൽ
		    sub->list.വരി[subidx].start = save_ptr;
	    }
	    അല്ലെങ്കിൽ
		sub->in_use = save_in_use;
	    നിർത്തുക;

	അവസ്ഥ NFA_MCLOSE:
	    എങ്കിൽ (rex.nfa_has_zend && (REG_MULTI
			? subs->norm.list.multi[0].end_lnum >= 0
			: subs->norm.list.വരി[0].end != ശൂന്യ))
	    {
		// Do not overwrite the position set by \ze.
		subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
		നിർത്തുക;
	    }
	    // FALLTHROUGH
	അവസ്ഥ NFA_MCLOSE1:
	അവസ്ഥ NFA_MCLOSE2:
	അവസ്ഥ NFA_MCLOSE3:
	അവസ്ഥ NFA_MCLOSE4:
	അവസ്ഥ NFA_MCLOSE5:
	അവസ്ഥ NFA_MCLOSE6:
	അവസ്ഥ NFA_MCLOSE7:
	അവസ്ഥ NFA_MCLOSE8:
	അവസ്ഥ NFA_MCLOSE9:
#ifdef FEAT_SYN_HL
	അവസ്ഥ NFA_ZCLOSE:
	അവസ്ഥ NFA_ZCLOSE1:
	അവസ്ഥ NFA_ZCLOSE2:
	അവസ്ഥ NFA_ZCLOSE3:
	അവസ്ഥ NFA_ZCLOSE4:
	അവസ്ഥ NFA_ZCLOSE5:
	അവസ്ഥ NFA_ZCLOSE6:
	അവസ്ഥ NFA_ZCLOSE7:
	അവസ്ഥ NFA_ZCLOSE8:
	അവസ്ഥ NFA_ZCLOSE9:
#endif
	അവസ്ഥ NFA_ZEND:
	    എങ്കിൽ (state->c == NFA_ZEND)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#ifdef FEAT_SYN_HL
	    അല്ലെങ്കിൽ എങ്കിൽ (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
	    {
		subidx = state->c - NFA_ZCLOSE;
		sub = &subs->synt;
	    }
#endif
	    അല്ലെങ്കിൽ
	    {
		subidx = state->c - NFA_MCLOSE;
		sub = &subs->norm;
	    }

	    // We don't fill in gaps here, there must have been an MOPEN that
	    // has done that.
	    save_in_use = sub->in_use;
	    എങ്കിൽ (sub->in_use <= subidx)
		sub->in_use = subidx + 1;
	    എങ്കിൽ (REG_MULTI)
	    {
		save_multipos = sub->list.multi[subidx];
		എങ്കിൽ (off == -1)
		{
		    sub->list.multi[subidx].end_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].end_col = 0;
		}
		അല്ലെങ്കിൽ
		{
		    sub->list.multi[subidx].end_lnum = rex.lnum;
		    sub->list.multi[subidx].end_col =
					  (colnr_T)(rex.നിവേശം - rex.വരി + off);
		}
		// avoid compiler warnings
		save_ptr = ശൂന്യ;
	    }
	    അല്ലെങ്കിൽ
	    {
		save_ptr = sub->list.വരി[subidx].end;
		sub->list.വരി[subidx].end = rex.നിവേശം + off;
		// avoid compiler warnings
		CLEAR_FIELD(save_multipos);
	    }

	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    എങ്കിൽ (subs == ശൂന്യ)
		നിർത്തുക;
	    // "subs" may have changed, need to set "sub" again
#ifdef FEAT_SYN_HL
	    എങ്കിൽ (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
		sub = &subs->synt;
	    അല്ലെങ്കിൽ
#endif
		sub = &subs->norm;

	    എങ്കിൽ (REG_MULTI)
		sub->list.multi[subidx] = save_multipos;
	    അല്ലെങ്കിൽ
		sub->list.വരി[subidx].end = save_ptr;
	    sub->in_use = save_in_use;
	    നിർത്തുക;
    }
    തിരികെ subs;
}

/*
 * Like addstate(), but the new state(s) are put at position "*ip".
 * Used for zero-width matches, next state to use is the added one.
 * This makes sure the order of states to be tried does not change, which
 * matters for alternatives.
 */
    സ്ഥിതം regsubs_T *
addstate_here(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim,   // postponed look-behind match
    പൂർണ്ണസംഖ്യ			*ip)
{
    പൂർണ്ണസംഖ്യ tlen = l->n;
    പൂർണ്ണസംഖ്യ എണ്ണം;
    പൂർണ്ണസംഖ്യ listidx = *ip;
    regsubs_T *r;

    // First add the state(s) at the end, so that we know how many there are.
    // Pass the listidx as offset (avoids adding another argument to
    // addstate()).
    r = addstate(l, state, subs, pim, -listidx - ADDSTATE_HERE_OFFSET, 0);
    എങ്കിൽ (r == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // when "*ip" was at the end of the list, nothing to do
    എങ്കിൽ (listidx + 1 == tlen)
	തിരികെ r;

    // re-order to put the new state at the current position
    എണ്ണം = l->n - tlen;
    എങ്കിൽ (എണ്ണം == 0)
	തിരികെ r; // no state got added
    എങ്കിൽ (എണ്ണം == 1)
    {
	// overwrite the current state
	l->t[listidx] = l->t[l->n - 1];
    }
    അല്ലെങ്കിൽ എങ്കിൽ (എണ്ണം > 1)
    {
	എങ്കിൽ (l->n + എണ്ണം - 1 >= l->len)
	{
	    // not enough space to move the new states, reallocate the list
	    // and move the states to the right position
	    പൂർണ്ണസംഖ്യ		    newlen = l->len * 3 / 2 + 50;
	    size_t	    newsize = newlen * വലുപ്പം(nfa_thread_T);
	    nfa_thread_T    *newl;

	    എങ്കിൽ ((long)(newsize >> 10) >= p_mmp)
	    {
		emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		തിരികെ ശൂന്യ;
	    }
	    newl = alloc(newsize);
	    എങ്കിൽ (newl == ശൂന്യ)
		തിരികെ ശൂന്യ;
	    l->len = newlen;
	    mch_memmove(&(newl[0]),
		    &(l->t[0]),
		    വലുപ്പം(nfa_thread_T) * listidx);
	    mch_memmove(&(newl[listidx]),
		    &(l->t[l->n - എണ്ണം]),
		    വലുപ്പം(nfa_thread_T) * എണ്ണം);
	    mch_memmove(&(newl[listidx + എണ്ണം]),
		    &(l->t[listidx + 1]),
		    വലുപ്പം(nfa_thread_T) * (l->n - എണ്ണം - listidx - 1));
	    vim_free(l->t);
	    l->t = newl;
	}
	അല്ലെങ്കിൽ
	{
	    // make space for new states, then move them from the
	    // end to the current position
	    mch_memmove(&(l->t[listidx + എണ്ണം]),
		    &(l->t[listidx + 1]),
		    വലുപ്പം(nfa_thread_T) * (l->n - listidx - 1));
	    mch_memmove(&(l->t[listidx]),
		    &(l->t[l->n - 1]),
		    വലുപ്പം(nfa_thread_T) * എണ്ണം);
	}
    }
    --l->n;
    *ip = listidx - 1;

    തിരികെ r;
}

/*
 * Check character class "class" against current character c.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_char_class(പൂർണ്ണസംഖ്യ class, പൂർണ്ണസംഖ്യ c)
{
    തിരഞ്ഞെടുപ്പ് (class)
    {
	അവസ്ഥ NFA_CLASS_ALNUM:
	    എങ്കിൽ (c >= 1 && c < 128 && isalnum(c))
		തിരികെ OK;
	    നിർത്തുക;
	അവസ്ഥ NFA_CLASS_ALPHA:
	    എങ്കിൽ (c >= 1 && c < 128 && isalpha(c))
		തിരികെ OK;
	    നിർത്തുക;
	അവസ്ഥ NFA_CLASS_BLANK:
	    എങ്കിൽ (c == ' ' || c == '\t')
		തിരികെ OK;
	    നിർത്തുക;
	അവസ്ഥ NFA_CLASS_CNTRL:
	    എങ്കിൽ (c >= 1 && c <= 127 && iscntrl(c))
		തിരികെ OK;
	    നിർത്തുക;
	അവസ്ഥ NFA_CLASS_DIGIT:
	    എങ്കിൽ (VIM_ISDIGIT(c))
		തിരികെ OK;
	    നിർത്തുക;
	അവസ്ഥ NFA_CLASS_GRAPH:
	    എങ്കിൽ (c >= 1 && c <= 127 && isgraph(c))
		തിരികെ OK;
	    നിർത്തുക;
	അവസ്ഥ NFA_CLASS_LOWER:
	    എങ്കിൽ (MB_ISLOWER(c) && c != 170 && c != 186)
		തിരികെ OK;
	    നിർത്തുക;
	അവസ്ഥ NFA_CLASS_PRINT:
	    എങ്കിൽ (vim_isprintc(c))
		തിരികെ OK;
	    നിർത്തുക;
	അവസ്ഥ NFA_CLASS_PUNCT:
	    എങ്കിൽ (c >= 1 && c < 128 && ispunct(c))
		തിരികെ OK;
	    നിർത്തുക;
	അവസ്ഥ NFA_CLASS_SPACE:
	    എങ്കിൽ ((c >= 9 && c <= 13) || (c == ' '))
		തിരികെ OK;
	    നിർത്തുക;
	അവസ്ഥ NFA_CLASS_UPPER:
	    എങ്കിൽ (MB_ISUPPER(c))
		തിരികെ OK;
	    നിർത്തുക;
	അവസ്ഥ NFA_CLASS_XDIGIT:
	    എങ്കിൽ (vim_isxdigit(c))
		തിരികെ OK;
	    നിർത്തുക;
	അവസ്ഥ NFA_CLASS_TAB:
	    എങ്കിൽ (c == '\t')
		തിരികെ OK;
	    നിർത്തുക;
	അവസ്ഥ NFA_CLASS_RETURN:
	    എങ്കിൽ (c == '\r')
		തിരികെ OK;
	    നിർത്തുക;
	അവസ്ഥ NFA_CLASS_BACKSPACE:
	    എങ്കിൽ (c == '\b')
		തിരികെ OK;
	    നിർത്തുക;
	അവസ്ഥ NFA_CLASS_ESCAPE:
	    എങ്കിൽ (c == '\033')
		തിരികെ OK;
	    നിർത്തുക;
	അവസ്ഥ NFA_CLASS_IDENT:
	    എങ്കിൽ (vim_isIDc(c))
		തിരികെ OK;
	    നിർത്തുക;
	അവസ്ഥ NFA_CLASS_KEYWORD:
	    എങ്കിൽ (reg_iswordc(c))
		തിരികെ OK;
	    നിർത്തുക;
	അവസ്ഥ NFA_CLASS_FNAME:
	    എങ്കിൽ (vim_isfilec(c))
		തിരികെ OK;
	    നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
	    // should not be here :P
	    siemsg(_(e_nfa_regexp_invalid_character_class_nr), class);
	    തിരികെ FAIL;
    }
    തിരികെ FAIL;
}

/*
 * Check for a match with subexpression "subidx".
 * Return TRUE if it matches.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
match_backref(
    regsub_T	*sub,	    // pointers to subexpressions
    പൂർണ്ണസംഖ്യ		subidx,
    പൂർണ്ണസംഖ്യ		*bytelen)   // out: length of match in bytes
{
    പൂർണ്ണസംഖ്യ		len;

    എങ്കിൽ (sub->in_use <= subidx)
    {
retempty:
	// backref was not set, match an empty string
	*bytelen = 0;
	തിരികെ TRUE;
    }

    എങ്കിൽ (REG_MULTI)
    {
	എങ്കിൽ (sub->list.multi[subidx].start_lnum < 0
				       || sub->list.multi[subidx].end_lnum < 0)
	    പോകുക retempty;
	എങ്കിൽ (sub->list.multi[subidx].start_lnum == rex.lnum
			       && sub->list.multi[subidx].end_lnum == rex.lnum)
	{
	    len = sub->list.multi[subidx].end_col
					  - sub->list.multi[subidx].start_col;
	    എങ്കിൽ (cstrncmp(rex.വരി + sub->list.multi[subidx].start_col,
							 rex.നിവേശം, &len) == 0)
	    {
		*bytelen = len;
		തിരികെ TRUE;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (match_with_backref(
			sub->list.multi[subidx].start_lnum,
			sub->list.multi[subidx].start_col,
			sub->list.multi[subidx].end_lnum,
			sub->list.multi[subidx].end_col,
			bytelen) == RA_MATCH)
		തിരികെ TRUE;
	}
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (sub->list.വരി[subidx].start == ശൂന്യ
					|| sub->list.വരി[subidx].end == ശൂന്യ)
	    പോകുക retempty;
	len = (പൂർണ്ണസംഖ്യ)(sub->list.വരി[subidx].end - sub->list.വരി[subidx].start);
	എങ്കിൽ (cstrncmp(sub->list.വരി[subidx].start, rex.നിവേശം, &len) == 0)
	{
	    *bytelen = len;
	    തിരികെ TRUE;
	}
    }
    തിരികെ FALSE;
}

#ifdef FEAT_SYN_HL

/*
 * Check for a match with \z subexpression "subidx".
 * Return TRUE if it matches.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
match_zref(
    പൂർണ്ണസംഖ്യ		subidx,
    പൂർണ്ണസംഖ്യ		*bytelen)   // out: length of match in bytes
{
    പൂർണ്ണസംഖ്യ		len;

    cleanup_zsubexpr();
    എങ്കിൽ (re_extmatch_in == ശൂന്യ || re_extmatch_in->matches[subidx] == ശൂന്യ)
    {
	// backref was not set, match an empty string
	*bytelen = 0;
	തിരികെ TRUE;
    }

    len = (പൂർണ്ണസംഖ്യ)STRLEN(re_extmatch_in->matches[subidx]);
    എങ്കിൽ (cstrncmp(re_extmatch_in->matches[subidx], rex.നിവേശം, &len) == 0)
    {
	*bytelen = len;
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}
#endif

/*
 * Save list IDs for all NFA states of "prog" into "list".
 * Also reset the IDs to zero.
 * Only used for the recursive value lastlist[1].
 */
    സ്ഥിതം ശൂന്യം
nfa_save_listids(nfa_regprog_T *prog, പൂർണ്ണസംഖ്യ *list)
{
    പൂർണ്ണസംഖ്യ		    i;
    nfa_state_T	    *p;

    // Order in the list is reverse, it's a bit faster that way.
    p = &prog->state[0];
    വേണ്ടി (i = prog->nstate; --i >= 0; )
    {
	list[i] = p->lastlist[1];
	p->lastlist[1] = 0;
	++p;
    }
}

/*
 * Restore list IDs from "list" to all NFA states.
 */
    സ്ഥിതം ശൂന്യം
nfa_restore_listids(nfa_regprog_T *prog, പൂർണ്ണസംഖ്യ *list)
{
    പൂർണ്ണസംഖ്യ		    i;
    nfa_state_T	    *p;

    p = &prog->state[0];
    വേണ്ടി (i = prog->nstate; --i >= 0; )
    {
	p->lastlist[1] = list[i];
	++p;
    }
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
nfa_re_അക്കം_cmp(long_u val, പൂർണ്ണസംഖ്യ op, long_u pos)
{
    എങ്കിൽ (op == 1) തിരികെ pos > val;
    എങ്കിൽ (op == 2) തിരികെ pos < val;
    തിരികെ val == pos;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ nfa_regmatch(nfa_regprog_T *prog, nfa_state_T *start, regsubs_T *submatch, regsubs_T *m);

/*
 * Recursively call nfa_regmatch()
 * "pim" is NULL or contains info about a Postponed Invisible Match (start
 * position).
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
recursive_regmatch(
    nfa_state_T	    *state,
    nfa_pim_T	    *pim,
    nfa_regprog_T   *prog,
    regsubs_T	    *submatch,
    regsubs_T	    *m,
    പൂർണ്ണസംഖ്യ		    **listids,
    പൂർണ്ണസംഖ്യ		    *listids_len)
{
    പൂർണ്ണസംഖ്യ		save_reginput_col = (പൂർണ്ണസംഖ്യ)(rex.നിവേശം - rex.വരി);
    പൂർണ്ണസംഖ്യ		save_reglnum = rex.lnum;
    പൂർണ്ണസംഖ്യ		save_nfa_match = nfa_match;
    പൂർണ്ണസംഖ്യ		save_nfa_listid = rex.nfa_listid;
    save_se_T   *save_nfa_endp = nfa_endp;
    save_se_T   endpos;
    save_se_T   *endposp = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		result;
    പൂർണ്ണസംഖ്യ		need_restore = FALSE;

    എങ്കിൽ (pim != ശൂന്യ)
    {
	// start at the position where the postponed match was
	എങ്കിൽ (REG_MULTI)
	    rex.നിവേശം = rex.വരി + pim->end.pos.col;
	അല്ലെങ്കിൽ
	    rex.നിവേശം = pim->end.ptr;
    }

    എങ്കിൽ (state->c == NFA_START_INVISIBLE_BEFORE
	    || state->c == NFA_START_INVISIBLE_BEFORE_FIRST
	    || state->c == NFA_START_INVISIBLE_BEFORE_NEG
	    || state->c == NFA_START_INVISIBLE_BEFORE_NEG_FIRST)
    {
	// The recursive match must end at the current position. When "pim" is
	// not NULL it specifies the current position.
	endposp = &endpos;
	എങ്കിൽ (REG_MULTI)
	{
	    എങ്കിൽ (pim == ശൂന്യ)
	    {
		endpos.se_u.pos.col = (പൂർണ്ണസംഖ്യ)(rex.നിവേശം - rex.വരി);
		endpos.se_u.pos.lnum = rex.lnum;
	    }
	    അല്ലെങ്കിൽ
		endpos.se_u.pos = pim->end.pos;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (pim == ശൂന്യ)
		endpos.se_u.ptr = rex.നിവേശം;
	    അല്ലെങ്കിൽ
		endpos.se_u.ptr = pim->end.ptr;
	}

	// Go back the specified number of bytes, or as far as the
	// start of the previous line, to try matching "\@<=" or
	// not matching "\@<!". This is very inefficient, limit the number of
	// bytes if possible.
	എങ്കിൽ (state->val <= 0)
	{
	    എങ്കിൽ (REG_MULTI)
	    {
		rex.വരി = reg_getline(--rex.lnum);
		എങ്കിൽ (rex.വരി == ശൂന്യ)
		    // can't go before the first line
		    rex.വരി = reg_getline(++rex.lnum);
	    }
	    rex.നിവേശം = rex.വരി;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (REG_MULTI && (പൂർണ്ണസംഖ്യ)(rex.നിവേശം - rex.വരി) < state->val)
	    {
		// Not enough bytes in this line, go to end of
		// previous line.
		rex.വരി = reg_getline(--rex.lnum);
		എങ്കിൽ (rex.വരി == ശൂന്യ)
		{
		    // can't go before the first line
		    rex.വരി = reg_getline(++rex.lnum);
		    rex.നിവേശം = rex.വരി;
		}
		അല്ലെങ്കിൽ
		    rex.നിവേശം = rex.വരി + reg_getline_len(rex.lnum);
	    }
	    എങ്കിൽ ((പൂർണ്ണസംഖ്യ)(rex.നിവേശം - rex.വരി) >= state->val)
	    {
		rex.നിവേശം -= state->val;
		എങ്കിൽ (has_mbyte)
		    rex.നിവേശം -= mb_head_off(rex.വരി, rex.നിവേശം);
	    }
	    അല്ലെങ്കിൽ
		rex.നിവേശം = rex.വരി;
	}
    }

#ifdef ENABLE_LOG
    എങ്കിൽ (log_fd != stderr)
	fclose(log_fd);
    log_fd = ശൂന്യ;
#endif
    // Have to clear the lastlist field of the NFA nodes, so that
    // nfa_regmatch() and addstate() can run properly after recursion.
    എങ്കിൽ (nfa_ll_index == 1)
    {
	// Already calling nfa_regmatch() recursively.  Save the lastlist[1]
	// values and clear them.
	എങ്കിൽ (*listids == ശൂന്യ || *listids_len < prog->nstate)
	{
	    vim_free(*listids);
	    *listids = ALLOC_MULT(പൂർണ്ണസംഖ്യ, prog->nstate);
	    എങ്കിൽ (*listids == ശൂന്യ)
	    {
		emsg(_(e_nfa_regexp_could_not_allocate_memory_for_branch_traversal));
		തിരികെ 0;
	    }
	    *listids_len = prog->nstate;
	}
	nfa_save_listids(prog, *listids);
	need_restore = TRUE;
	// any value of rex.nfa_listid will do
    }
    അല്ലെങ്കിൽ
    {
	// First recursive nfa_regmatch() call, switch to the second lastlist
	// entry.  Make sure rex.nfa_listid is different from a previous
	// recursive call, because some states may still have this ID.
	++nfa_ll_index;
	എങ്കിൽ (rex.nfa_listid <= rex.nfa_alt_listid)
	    rex.nfa_listid = rex.nfa_alt_listid;
    }

    // Call nfa_regmatch() to check if the current concat matches at this
    // position. The concat ends with the node NFA_END_INVISIBLE
    nfa_endp = endposp;
    result = nfa_regmatch(prog, state->out, submatch, m);

    എങ്കിൽ (need_restore)
	nfa_restore_listids(prog, *listids);
    അല്ലെങ്കിൽ
    {
	--nfa_ll_index;
	rex.nfa_alt_listid = rex.nfa_listid;
    }

    // restore position in input text
    rex.lnum = save_reglnum;
    എങ്കിൽ (REG_MULTI)
	rex.വരി = reg_getline(rex.lnum);
    rex.നിവേശം = rex.വരി + save_reginput_col;
    എങ്കിൽ (result != NFA_TOO_EXPENSIVE)
    {
	nfa_match = save_nfa_match;
	rex.nfa_listid = save_nfa_listid;
    }
    nfa_endp = save_nfa_endp;

#ifdef ENABLE_LOG
    open_debug_log(result);
#endif

    തിരികെ result;
}

/*
 * Estimate the chance of a match with "state" failing.
 * empty match: 0
 * NFA_ANY: 1
 * specific character: 99
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
failure_chance(nfa_state_T *state, പൂർണ്ണസംഖ്യ depth)
{
    പൂർണ്ണസംഖ്യ c = state->c;
    പൂർണ്ണസംഖ്യ l, r;

    // detect looping
    എങ്കിൽ (depth > 4)
	തിരികെ 1;

    തിരഞ്ഞെടുപ്പ് (c)
    {
	അവസ്ഥ NFA_SPLIT:
	    എങ്കിൽ (state->out->c == NFA_SPLIT || state->out1->c == NFA_SPLIT)
		// avoid recursive stuff
		തിരികെ 1;
	    // two alternatives, use the lowest failure chance
	    l = failure_chance(state->out, depth + 1);
	    r = failure_chance(state->out1, depth + 1);
	    തിരികെ l < r ? l : r;

	അവസ്ഥ NFA_ANY:
	    // matches anything, unlikely to fail
	    തിരികെ 1;

	അവസ്ഥ NFA_MATCH:
	അവസ്ഥ NFA_MCLOSE:
	അവസ്ഥ NFA_ANY_COMPOSING:
	    // empty match works always
	    തിരികെ 0;

	അവസ്ഥ NFA_START_INVISIBLE:
	അവസ്ഥ NFA_START_INVISIBLE_FIRST:
	അവസ്ഥ NFA_START_INVISIBLE_NEG:
	അവസ്ഥ NFA_START_INVISIBLE_NEG_FIRST:
	അവസ്ഥ NFA_START_INVISIBLE_BEFORE:
	അവസ്ഥ NFA_START_INVISIBLE_BEFORE_FIRST:
	അവസ്ഥ NFA_START_INVISIBLE_BEFORE_NEG:
	അവസ്ഥ NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
	അവസ്ഥ NFA_START_PATTERN:
	    // recursive regmatch is expensive, use low failure chance
	    തിരികെ 5;

	അവസ്ഥ NFA_BOL:
	അവസ്ഥ NFA_EOL:
	അവസ്ഥ NFA_BOF:
	അവസ്ഥ NFA_EOF:
	അവസ്ഥ NFA_NEWL:
	    തിരികെ 99;

	അവസ്ഥ NFA_BOW:
	അവസ്ഥ NFA_EOW:
	    തിരികെ 90;

	അവസ്ഥ NFA_MOPEN:
	അവസ്ഥ NFA_MOPEN1:
	അവസ്ഥ NFA_MOPEN2:
	അവസ്ഥ NFA_MOPEN3:
	അവസ്ഥ NFA_MOPEN4:
	അവസ്ഥ NFA_MOPEN5:
	അവസ്ഥ NFA_MOPEN6:
	അവസ്ഥ NFA_MOPEN7:
	അവസ്ഥ NFA_MOPEN8:
	അവസ്ഥ NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	അവസ്ഥ NFA_ZOPEN:
	അവസ്ഥ NFA_ZOPEN1:
	അവസ്ഥ NFA_ZOPEN2:
	അവസ്ഥ NFA_ZOPEN3:
	അവസ്ഥ NFA_ZOPEN4:
	അവസ്ഥ NFA_ZOPEN5:
	അവസ്ഥ NFA_ZOPEN6:
	അവസ്ഥ NFA_ZOPEN7:
	അവസ്ഥ NFA_ZOPEN8:
	അവസ്ഥ NFA_ZOPEN9:
	അവസ്ഥ NFA_ZCLOSE:
	അവസ്ഥ NFA_ZCLOSE1:
	അവസ്ഥ NFA_ZCLOSE2:
	അവസ്ഥ NFA_ZCLOSE3:
	അവസ്ഥ NFA_ZCLOSE4:
	അവസ്ഥ NFA_ZCLOSE5:
	അവസ്ഥ NFA_ZCLOSE6:
	അവസ്ഥ NFA_ZCLOSE7:
	അവസ്ഥ NFA_ZCLOSE8:
	അവസ്ഥ NFA_ZCLOSE9:
#endif
	അവസ്ഥ NFA_NOPEN:
	അവസ്ഥ NFA_MCLOSE1:
	അവസ്ഥ NFA_MCLOSE2:
	അവസ്ഥ NFA_MCLOSE3:
	അവസ്ഥ NFA_MCLOSE4:
	അവസ്ഥ NFA_MCLOSE5:
	അവസ്ഥ NFA_MCLOSE6:
	അവസ്ഥ NFA_MCLOSE7:
	അവസ്ഥ NFA_MCLOSE8:
	അവസ്ഥ NFA_MCLOSE9:
	അവസ്ഥ NFA_NCLOSE:
	    തിരികെ failure_chance(state->out, depth + 1);

	അവസ്ഥ NFA_BACKREF1:
	അവസ്ഥ NFA_BACKREF2:
	അവസ്ഥ NFA_BACKREF3:
	അവസ്ഥ NFA_BACKREF4:
	അവസ്ഥ NFA_BACKREF5:
	അവസ്ഥ NFA_BACKREF6:
	അവസ്ഥ NFA_BACKREF7:
	അവസ്ഥ NFA_BACKREF8:
	അവസ്ഥ NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	അവസ്ഥ NFA_ZREF1:
	അവസ്ഥ NFA_ZREF2:
	അവസ്ഥ NFA_ZREF3:
	അവസ്ഥ NFA_ZREF4:
	അവസ്ഥ NFA_ZREF5:
	അവസ്ഥ NFA_ZREF6:
	അവസ്ഥ NFA_ZREF7:
	അവസ്ഥ NFA_ZREF8:
	അവസ്ഥ NFA_ZREF9:
#endif
	    // backreferences don't match in many places
	    തിരികെ 94;

	അവസ്ഥ NFA_LNUM_GT:
	അവസ്ഥ NFA_LNUM_LT:
	അവസ്ഥ NFA_COL_GT:
	അവസ്ഥ NFA_COL_LT:
	അവസ്ഥ NFA_VCOL_GT:
	അവസ്ഥ NFA_VCOL_LT:
	അവസ്ഥ NFA_MARK_GT:
	അവസ്ഥ NFA_MARK_LT:
	അവസ്ഥ NFA_VISUAL:
	    // before/after positions don't match very often
	    തിരികെ 85;

	അവസ്ഥ NFA_LNUM:
	    തിരികെ 90;

	അവസ്ഥ NFA_CURSOR:
	അവസ്ഥ NFA_COL:
	അവസ്ഥ NFA_VCOL:
	അവസ്ഥ NFA_MARK:
	    // specific positions rarely match
	    തിരികെ 98;

	അവസ്ഥ NFA_COMPOSING:
	    തിരികെ 95;

	സ്ഥിരസ്ഥിതി:
	    എങ്കിൽ (c > 0)
		// character match fails often
		തിരികെ 95;
    }

    // something else, includes character classes
    തിരികെ 50;
}

/*
 * Skip until the char "c" we know a match must start with.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
skip_to_start(പൂർണ്ണസംഖ്യ c, colnr_T *colp)
{
    char_u *s;

    // Used often, do some work to avoid call overhead.
    എങ്കിൽ (!rex.reg_ic && !has_mbyte)
	s = vim_strbyte(rex.വരി + *colp, c);
    അല്ലെങ്കിൽ
	s = cstrchr(rex.വരി + *colp, c);
    എങ്കിൽ (s == ശൂന്യ)
	തിരികെ FAIL;
    *colp = (പൂർണ്ണസംഖ്യ)(s - rex.വരി);
    തിരികെ OK;
}

/*
 * Check for a match with match_text.
 * Called after skip_to_start() has found regstart.
 * Returns zero for no match, 1 for a match.
 */
    സ്ഥിതം long
find_match_text(colnr_T *startcol, പൂർണ്ണസംഖ്യ regstart, char_u *match_text)
{
    colnr_T col = *startcol;
    പൂർണ്ണസംഖ്യ	    c1, c2;
    പൂർണ്ണസംഖ്യ	    len1, len2;
    പൂർണ്ണസംഖ്യ	    match;

    വേണ്ടി (;;)
    {
	match = TRUE;
	// Skip the input character: case-folding can change its byte length.
	len2 = MB_CPTR2LEN(rex.വരി + col);
	വേണ്ടി (len1 = 0; match_text[len1] != NUL; len1 += MB_CHAR2LEN(c1))
	{
	    c1 = PTR2CHAR(match_text + len1);
	    c2 = PTR2CHAR(rex.വരി + col + len2);
	    എങ്കിൽ (c1 != c2 && (!rex.reg_ic || MB_CASEFOLD(c1) != MB_CASEFOLD(c2)))
	    {
		match = FALSE;
		നിർത്തുക;
	    }
	    len2 += enc_utf8 ? utf_ptr2len(rex.വരി + col + len2)
							     : MB_CHAR2LEN(c2);
	}
	എങ്കിൽ (match
		// check that no composing char follows
		&& !(enc_utf8
			  && utf_iscomposing(PTR2CHAR(rex.വരി + col + len2))))
	{
	    cleanup_subexpr();
	    എങ്കിൽ (REG_MULTI)
	    {
		rex.reg_startpos[0].lnum = rex.lnum;
		rex.reg_startpos[0].col = col;
		rex.reg_endpos[0].lnum = rex.lnum;
		rex.reg_endpos[0].col = col + len2;
	    }
	    അല്ലെങ്കിൽ
	    {
		rex.reg_startp[0] = rex.വരി + col;
		rex.reg_endp[0] = rex.വരി + col + len2;
	    }
	    *startcol = col;
	    തിരികെ 1L;
	}

	// Try finding regstart after the current match.
	col += MB_CPTR2LEN(rex.വരി + col); // skip the input character
	എങ്കിൽ (skip_to_start(regstart, &col) == FAIL)
	    നിർത്തുക;
    }

    *startcol = col;
    തിരികെ 0L;
}

/*
 * Check whether the composing characters at the current input position match
 * the NFA composing sub-expression that starts at "sta" (the first state
 * after NFA_COMPOSING).  "curc" is the base character and "clen" its byte
 * length.
 * Returns OK when it matches, FAIL otherwise.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
match_composing(nfa_state_T *sta, പൂർണ്ണസംഖ്യ curc, പൂർണ്ണസംഖ്യ clen)
{
    പൂർണ്ണസംഖ്യ		mc = curc;
    പൂർണ്ണസംഖ്യ		len = 0;
    പൂർണ്ണസംഖ്യ		cchars[MAX_MCO];
    പൂർണ്ണസംഖ്യ		ccount = 0;
    പൂർണ്ണസംഖ്യ		j;

    എങ്കിൽ (utf_iscomposing(sta->c))
	// Only match composing character(s), ignore base character.  Used
	// for ".{composing}" and "{composing}" (no preceding character).
	len += mb_char2len(mc);

    എങ്കിൽ (rex.reg_icombine && len == 0)
	// If \Z was present, then ignore composing characters.  When
	// ignoring the base character this always matches.
	തിരികെ sta->c == curc ? OK : FAIL;

    // Check base character matches first, unless ignored.
    എങ്കിൽ (len == 0 && mc != sta->c)
	തിരികെ FAIL;

    എങ്കിൽ (len == 0)
    {
	len += mb_char2len(mc);
	sta = sta->out;
    }

    // We don't care about the order of composing characters.  Get them into
    // cchars[] first.
    ആകുമ്പോൾ (len < clen)
    {
	mc = mb_ptr2char(rex.നിവേശം + len);
	cchars[ccount++] = mc;
	len += mb_char2len(mc);
	എങ്കിൽ (ccount == MAX_MCO)
	    നിർത്തുക;
    }

    // Check that each composing char in the pattern matches a composing char
    // in the text.  We do not check if all composing chars are matched.
    ആകുമ്പോൾ (sta->c != NFA_END_COMPOSING)
    {
	വേണ്ടി (j = 0; j < ccount; ++j)
	    എങ്കിൽ (cchars[j] == sta->c)
		നിർത്തുക;
	എങ്കിൽ (j == ccount)
	    തിരികെ FAIL;
	sta = sta->out;
    }

    തിരികെ OK;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
nfa_regmatch(
    nfa_regprog_T	*prog,
    nfa_state_T		*start,
    regsubs_T		*submatch,
    regsubs_T		*m)
{
    പൂർണ്ണസംഖ്യ		result = FALSE;
    size_t	size = 0;
    പൂർണ്ണസംഖ്യ		flag = 0;
    പൂർണ്ണസംഖ്യ		go_to_nextline = FALSE;
    nfa_thread_T *t;
    nfa_list_T	list[2];
    പൂർണ്ണസംഖ്യ		listidx;
    nfa_list_T	*thislist;
    nfa_list_T	*nextlist;
    പൂർണ്ണസംഖ്യ		*listids = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		listids_len = 0;
    nfa_state_T *add_state;
    പൂർണ്ണസംഖ്യ		add_here;
    പൂർണ്ണസംഖ്യ		add_എണ്ണം;
    പൂർണ്ണസംഖ്യ		add_off = 0;
    പൂർണ്ണസംഖ്യ		toplevel = start->c == NFA_MOPEN;
    regsubs_T	*r;
#ifdef NFA_REGEXP_DEBUG_LOG
    FILE	*debug;
#endif

    // Some patterns may take a long time to match, especially when using
    // recursive_regmatch(). Allow interrupting them with CTRL-C.
    fast_breakcheck();
    എങ്കിൽ (got_int)
	തിരികെ FALSE;
#ifdef FEAT_RELTIME
    എങ്കിൽ (nfa_did_time_out())
	തിരികെ FALSE;
#endif

#ifdef NFA_REGEXP_DEBUG_LOG
    debug = fopen(NFA_REGEXP_DEBUG_LOG, "a");
    എങ്കിൽ (debug == ശൂന്യ)
    {
	semsg("(NFA) COULD NOT OPEN %s!", NFA_REGEXP_DEBUG_LOG);
	തിരികെ FALSE;
    }
#endif
    nfa_match = FALSE;

    // Allocate memory for the lists of nodes.
    size = (prog->nstate + 1) * വലുപ്പം(nfa_thread_T);

    // Reuse cached list buffers from prog when available (top-level call).
    // Recursive calls must allocate their own buffers.
    എങ്കിൽ (toplevel && prog->listbuf[0] != ശൂന്യ)
    {
	list[0].t = (nfa_thread_T *)prog->listbuf[0];
	list[1].t = (nfa_thread_T *)prog->listbuf[1];
	prog->listbuf[0] = ശൂന്യ;
	prog->listbuf[1] = ശൂന്യ;
    }
    അല്ലെങ്കിൽ
    {
	list[0].t = alloc(size);
	list[1].t = alloc(size);
    }
    list[0].len = prog->nstate + 1;
    list[1].len = prog->nstate + 1;
    എങ്കിൽ (list[0].t == ശൂന്യ || list[1].t == ശൂന്യ)
	പോകുക theend;

#ifdef ENABLE_LOG
    log_fd = fopen(NFA_REGEXP_RUN_LOG, "a");
    എങ്കിൽ (log_fd == ശൂന്യ)
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
    എങ്കിൽ (toplevel)
    {
	എങ്കിൽ (REG_MULTI)
	{
	    m->norm.list.multi[0].start_lnum = rex.lnum;
	    m->norm.list.multi[0].start_col = (colnr_T)(rex.നിവേശം - rex.വരി);
	    m->norm.orig_start_col = m->norm.list.multi[0].start_col;
	}
	അല്ലെങ്കിൽ
	    m->norm.list.വരി[0].start = rex.നിവേശം;
	m->norm.in_use = 1;
	r = addstate(thislist, start->out, m, ശൂന്യ, 0, 0);
    }
    അല്ലെങ്കിൽ
	r = addstate(thislist, start, m, ശൂന്യ, 0, 0);
    എങ്കിൽ (r == ശൂന്യ)
    {
	nfa_match = NFA_TOO_EXPENSIVE;
	പോകുക theend;
    }

#define	ADD_STATE_IF_MATCH(state)	\
    എങ്കിൽ (result)				\
    {					\
	add_state = state->out;		\
	add_off = clen;			\
    }

    /*
     * Run for each character.
     */
    വേണ്ടി (;;)
    {
	പൂർണ്ണസംഖ്യ	curc;
	പൂർണ്ണസംഖ്യ	clen;

	എങ്കിൽ (has_mbyte)
	{
	    // Fast path for an ASCII byte not followed by a composing
	    // character, matching the check in utfc_ptr2len().  Avoids two
	    // indirect calls for the common case.
	    എങ്കിൽ (rex.നിവേശം[0] != NUL && rex.നിവേശം[0] < 0x80 && rex.നിവേശം[1] < 0x80)
	    {
		curc = rex.നിവേശം[0];
		clen = 1;
	    }
	    അല്ലെങ്കിൽ
	    {
		curc = (*mb_ptr2char)(rex.നിവേശം);
		clen = (*mb_ptr2len)(rex.നിവേശം);
	    }
	}
	അല്ലെങ്കിൽ
	{
	    curc = *rex.നിവേശം;
	    clen = 1;
	}
	എങ്കിൽ (curc == NUL)
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
	എങ്കിൽ (prog->re_engine == AUTOMATIC_ENGINE
		&& (rex.nfa_listid >= NFA_MAX_STATES
#ifdef FEAT_EVAL
		    || nfa_fail_for_testing
#endif
		    ))
	{
	    // too many states, retry with old engine
	    nfa_match = NFA_TOO_EXPENSIVE;
	    പോകുക theend;
	}

	thislist->id = rex.nfa_listid;
	nextlist->id = rex.nfa_listid + 1;

#ifdef ENABLE_LOG
	fprintf(log_fd, "------------------------------------------\n");
	fprintf(log_fd, ">>> Reginput is \"%s\"\n", rex.നിവേശം);
	fprintf(log_fd, ">>> Advanced one character... Current char is %c (code %d) \n", curc, (പൂർണ്ണസംഖ്യ)curc);
	fprintf(log_fd, ">>> Thislist has %d states available: ", thislist->n);
	{
	    പൂർണ്ണസംഖ്യ i;

	    വേണ്ടി (i = 0; i < thislist->n; i++)
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
	എങ്കിൽ (thislist->n == 0)
	    നിർത്തുക;

	// compute nextlist
	വേണ്ടി (listidx = 0; listidx < thislist->n; ++listidx)
	{
	    // If the list gets very long there probably is something wrong.
	    // At least allow interrupting with CTRL-C.  Only check once in a
	    // while, calling ui_breakcheck() for every state is too slow.
	    സ്ഥിതം പൂർണ്ണസംഖ്യ	breakcheck_എണ്ണം = 0;  // using "static" makes it faster
	    എങ്കിൽ (unlikely(++breakcheck_എണ്ണം >= 100000))
	    {
		ui_breakcheck();
		breakcheck_എണ്ണം = 0;
	    }
	    എങ്കിൽ (got_int)
		നിർത്തുക;
#ifdef FEAT_RELTIME
	    എങ്കിൽ (nfa_did_time_out())
		നിർത്തുക;
#endif
	    t = &thislist->t[listidx];

#ifdef NFA_REGEXP_DEBUG_LOG
	    nfa_set_code(t->state->c);
	    fprintf(debug, "%s, ", code);
#endif
#ifdef ENABLE_LOG
	    {
		പൂർണ്ണസംഖ്യ col;

		എങ്കിൽ (t->subs.norm.in_use <= 0)
		    col = -1;
		അല്ലെങ്കിൽ എങ്കിൽ (REG_MULTI)
		    col = t->subs.norm.list.multi[0].start_col;
		അല്ലെങ്കിൽ
		    col = (പൂർണ്ണസംഖ്യ)(t->subs.norm.list.വരി[0].start - rex.വരി);
		nfa_set_code(t->state->c);
		fprintf(log_fd, "(%d) char %d %s (start col %d)%s... \n",
			abs(t->state->id), (പൂർണ്ണസംഖ്യ)t->state->c, code, col,
			pim_info(&t->pim));
	    }
#endif

	    /*
	     * Handle the possible codes of the current state.
	     * The most important is NFA_MATCH.
	     */
	    add_state = ശൂന്യ;
	    add_here = FALSE;
	    add_എണ്ണം = 0;
	    തിരഞ്ഞെടുപ്പ് (t->state->c)
	    {
	    അവസ്ഥ NFA_MATCH:
	      {
		// If the match is not at the start of the line, ends before a
		// composing characters and rex.reg_icombine is not set, that
		// is not really a match.
		എങ്കിൽ (enc_utf8 && !rex.reg_icombine
			     && rex.നിവേശം != rex.വരി && utf_iscomposing(curc))
		    നിർത്തുക;

		nfa_match = TRUE;
		copy_sub(&submatch->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		എങ്കിൽ (rex.nfa_has_zsubexpr)
		    copy_sub(&submatch->synt, &t->subs.synt);
#endif
#ifdef ENABLE_LOG
		log_subsexpr(&t->subs);
#endif
		// Found the left-most longest match, do not look at any other
		// states at this position.  When the list of states is going
		// to be empty quit without advancing, so that "rex.input" is
		// correct.
		എങ്കിൽ (nextlist->n == 0)
		    clen = 0;
		പോകുക nextchar;
	      }

	    അവസ്ഥ NFA_END_INVISIBLE:
	    അവസ്ഥ NFA_END_INVISIBLE_NEG:
	    അവസ്ഥ NFA_END_PATTERN:
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
		എങ്കിൽ (nfa_endp != ശൂന്യ)
		{
		    എങ്കിൽ (REG_MULTI)
			fprintf(log_fd, "Current lnum: %d, endp lnum: %d; current col: %d, endp col: %d\n",
				(പൂർണ്ണസംഖ്യ)rex.lnum,
				(പൂർണ്ണസംഖ്യ)nfa_endp->se_u.pos.lnum,
				(പൂർണ്ണസംഖ്യ)(rex.നിവേശം - rex.വരി),
				nfa_endp->se_u.pos.col);
		    അല്ലെങ്കിൽ
			fprintf(log_fd, "Current col: %d, endp col: %d\n",
				(പൂർണ്ണസംഖ്യ)(rex.നിവേശം - rex.വരി),
				(പൂർണ്ണസംഖ്യ)(nfa_endp->se_u.ptr - rex.നിവേശം));
		}
#endif
		// If "nfa_endp" is set it's only a match if it ends at
		// "nfa_endp"
		എങ്കിൽ (nfa_endp != ശൂന്യ && (REG_MULTI
			? (rex.lnum != nfa_endp->se_u.pos.lnum
			    || (പൂർണ്ണസംഖ്യ)(rex.നിവേശം - rex.വരി)
						!= nfa_endp->se_u.pos.col)
			: rex.നിവേശം != nfa_endp->se_u.ptr))
		    നിർത്തുക;

		// do not set submatches for \@!
		എങ്കിൽ (t->state->c != NFA_END_INVISIBLE_NEG)
		{
		    copy_sub(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		    എങ്കിൽ (rex.nfa_has_zsubexpr)
			copy_sub(&m->synt, &t->subs.synt);
#endif
		}
#ifdef ENABLE_LOG
		fprintf(log_fd, "Match found:\n");
		log_subsexpr(m);
#endif
		nfa_match = TRUE;
		// See comment above at "goto nextchar".
		എങ്കിൽ (nextlist->n == 0)
		    clen = 0;
		പോകുക nextchar;

	    അവസ്ഥ NFA_START_INVISIBLE:
	    അവസ്ഥ NFA_START_INVISIBLE_FIRST:
	    അവസ്ഥ NFA_START_INVISIBLE_NEG:
	    അവസ്ഥ NFA_START_INVISIBLE_NEG_FIRST:
	    അവസ്ഥ NFA_START_INVISIBLE_BEFORE:
	    അവസ്ഥ NFA_START_INVISIBLE_BEFORE_FIRST:
	    അവസ്ഥ NFA_START_INVISIBLE_BEFORE_NEG:
	    അവസ്ഥ NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
		{
#ifdef ENABLE_LOG
		    fprintf(log_fd, "Failure chance invisible: %d, what follows: %d\n",
			    failure_chance(t->state->out, 0),
			    failure_chance(t->state->out1->out, 0));
#endif
		    // Do it directly if there already is a PIM or when
		    // nfa_postprocess() detected it will work better.
		    എങ്കിൽ (t->pim.result != NFA_PIM_UNUSED
			 || t->state->c == NFA_START_INVISIBLE_FIRST
			 || t->state->c == NFA_START_INVISIBLE_NEG_FIRST
			 || t->state->c == NFA_START_INVISIBLE_BEFORE_FIRST
			 || t->state->c == NFA_START_INVISIBLE_BEFORE_NEG_FIRST)
		    {
			പൂർണ്ണസംഖ്യ in_use = m->norm.in_use;

			// Copy submatch info for the recursive call, opposite
			// of what happens on success below.
			copy_sub_off(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
			എങ്കിൽ (rex.nfa_has_zsubexpr)
			    copy_sub_off(&m->synt, &t->subs.synt);
#endif

			/*
			 * First try matching the invisible match, then what
			 * follows.
			 */
			result = recursive_regmatch(t->state, ശൂന്യ, prog,
					  submatch, m, &listids, &listids_len);
			എങ്കിൽ (result == NFA_TOO_EXPENSIVE)
			{
			    nfa_match = result;
			    പോകുക theend;
			}

			// for \@! and \@<! it is a match when the result is
			// FALSE
			എങ്കിൽ (result != (t->state->c == NFA_START_INVISIBLE_NEG
			       || t->state->c == NFA_START_INVISIBLE_NEG_FIRST
			       || t->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			       || t->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&t->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
			    എങ്കിൽ (rex.nfa_has_zsubexpr)
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
		    അല്ലെങ്കിൽ
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
			എങ്കിൽ (REG_MULTI)
			{
			    pim.end.pos.col = (പൂർണ്ണസംഖ്യ)(rex.നിവേശം - rex.വരി);
			    pim.end.pos.lnum = rex.lnum;
			}
			അല്ലെങ്കിൽ
			    pim.end.ptr = rex.നിവേശം;

			// t->state->out1 is the corresponding END_INVISIBLE
			// node; Add its out to the current list (zero-width
			// match).
			എങ്കിൽ (addstate_here(thislist, t->state->out1->out,
					     &t->subs, &pim, &listidx) == ശൂന്യ)
			{
			    nfa_match = NFA_TOO_EXPENSIVE;
			    പോകുക theend;
			}
		    }
		}
		നിർത്തുക;

	    അവസ്ഥ NFA_START_PATTERN:
	      {
		nfa_state_T *skip = ശൂന്യ;
#ifdef ENABLE_LOG
		പൂർണ്ണസംഖ്യ	    skip_lid = 0;
#endif

		// There is no point in trying to match the pattern if the
		// output state is not going to be added to the list.
		എങ്കിൽ (state_in_list(nextlist, t->state->out1->out, &t->subs))
		{
		    skip = t->state->out1->out;
#ifdef ENABLE_LOG
		    skip_lid = nextlist->id;
#endif
		}
		അല്ലെങ്കിൽ എങ്കിൽ (state_in_list(nextlist,
					  t->state->out1->out->out, &t->subs))
		{
		    skip = t->state->out1->out->out;
#ifdef ENABLE_LOG
		    skip_lid = nextlist->id;
#endif
		}
		അല്ലെങ്കിൽ എങ്കിൽ (state_in_list(thislist,
					  t->state->out1->out->out, &t->subs))
		{
		    skip = t->state->out1->out->out;
#ifdef ENABLE_LOG
		    skip_lid = thislist->id;
#endif
		}
		എങ്കിൽ (skip != ശൂന്യ)
		{
#ifdef ENABLE_LOG
		    nfa_set_code(skip->c);
		    fprintf(log_fd, "> Not trying to match pattern, output state %d is already in list %d. char %d: %s\n",
			    abs(skip->id), skip_lid, skip->c, code);
#endif
		    നിർത്തുക;
		}
		// Copy submatch info to the recursive call, opposite of what
		// happens afterwards.
		copy_sub_off(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		എങ്കിൽ (rex.nfa_has_zsubexpr)
		    copy_sub_off(&m->synt, &t->subs.synt);
#endif

		// First try matching the pattern.
		result = recursive_regmatch(t->state, ശൂന്യ, prog,
					  submatch, m, &listids, &listids_len);
		എങ്കിൽ (result == NFA_TOO_EXPENSIVE)
		{
		    nfa_match = result;
		    പോകുക theend;
		}
		എങ്കിൽ (result)
		{
		    പൂർണ്ണസംഖ്യ bytelen;

#ifdef ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN matches:\n");
		    log_subsexpr(m);
#endif
		    // Copy submatch info from the recursive call
		    copy_sub_off(&t->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
		    എങ്കിൽ (rex.nfa_has_zsubexpr)
			copy_sub_off(&t->subs.synt, &m->synt);
#endif
		    // Now we need to skip over the matched text and then
		    // continue with what follows.
		    എങ്കിൽ (REG_MULTI)
			// TODO: multi-line match
			bytelen = m->norm.list.multi[0].end_col
						  - (പൂർണ്ണസംഖ്യ)(rex.നിവേശം - rex.വരി);
		    അല്ലെങ്കിൽ
			bytelen = (പൂർണ്ണസംഖ്യ)(m->norm.list.വരി[0].end - rex.നിവേശം);

#ifdef ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN length: %d\n", bytelen);
#endif
		    എങ്കിൽ (bytelen == 0)
		    {
			// empty match, output of corresponding
			// NFA_END_PATTERN/NFA_SKIP to be used at current
			// position
			add_here = TRUE;
			add_state = t->state->out1->out->out;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (bytelen <= clen)
		    {
			// match current character, output of corresponding
			// NFA_END_PATTERN to be used at next position.
			add_state = t->state->out1->out->out;
			add_off = clen;
		    }
		    അല്ലെങ്കിൽ
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			add_state = t->state->out1->out;
			add_off = bytelen;
			add_എണ്ണം = bytelen - clen;
		    }
		}
		നിർത്തുക;
	      }

	    അവസ്ഥ NFA_BOL:
		എങ്കിൽ (rex.നിവേശം == rex.വരി)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		നിർത്തുക;

	    അവസ്ഥ NFA_EOL:
		എങ്കിൽ (curc == NUL)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		നിർത്തുക;

	    അവസ്ഥ NFA_BOW:
		result = TRUE;

		എങ്കിൽ (curc == NUL)
		    result = FALSE;
		അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte)
		{
		    പൂർണ്ണസംഖ്യ this_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.നിവേശം, rex.reg_buf);
		    എങ്കിൽ (this_class <= 1)
			result = FALSE;
		    അല്ലെങ്കിൽ എങ്കിൽ (reg_prev_class() == this_class)
			result = FALSE;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (!vim_iswordc_buf(curc, rex.reg_buf)
			   || (rex.നിവേശം > rex.വരി
			       && vim_iswordc_buf(rex.നിവേശം[-1], rex.reg_buf)))
		    result = FALSE;
		എങ്കിൽ (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		നിർത്തുക;

	    അവസ്ഥ NFA_EOW:
		result = TRUE;
		എങ്കിൽ (rex.നിവേശം == rex.വരി)
		    result = FALSE;
		അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte)
		{
		    പൂർണ്ണസംഖ്യ this_class, prev_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.നിവേശം, rex.reg_buf);
		    prev_class = reg_prev_class();
		    എങ്കിൽ (this_class == prev_class
					|| prev_class == 0 || prev_class == 1)
			result = FALSE;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (!vim_iswordc_buf(rex.നിവേശം[-1], rex.reg_buf)
			|| (rex.നിവേശം[0] != NUL
					&& vim_iswordc_buf(curc, rex.reg_buf)))
		    result = FALSE;
		എങ്കിൽ (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		നിർത്തുക;

	    അവസ്ഥ NFA_BOF:
		എങ്കിൽ (rex.lnum == 0 && rex.നിവേശം == rex.വരി
				     && (!REG_MULTI || rex.reg_firstlnum == 1))
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		നിർത്തുക;

	    അവസ്ഥ NFA_EOF:
		എങ്കിൽ (rex.lnum == rex.reg_maxline && curc == NUL)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		നിർത്തുക;

	    അവസ്ഥ NFA_COMPOSING:
	    {
		nfa_state_T *end;

		result = match_composing(t->state->out, curc, clen);

		end = t->state->out1;	    // NFA_END_COMPOSING
		ADD_STATE_IF_MATCH(end);
		നിർത്തുക;
	    }

	    അവസ്ഥ NFA_NEWL:
		എങ്കിൽ (curc == NUL && !rex.reg_വരി_lbr && REG_MULTI
						&& rex.lnum <= rex.reg_maxline)
		{
		    go_to_nextline = TRUE;
		    // Pass -1 for the offset, which means taking the position
		    // at the start of the next line.
		    add_state = t->state->out;
		    add_off = -1;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (curc == '\n' && rex.reg_വരി_lbr)
		{
		    // match \n as if it is an ordinary character
		    add_state = t->state->out;
		    add_off = 1;
		}
		നിർത്തുക;

	    അവസ്ഥ NFA_START_COLL:
	    അവസ്ഥ NFA_START_NEG_COLL:
	      {
		// What follows is a list of characters, until NFA_END_COLL.
		// One of them must match or none of them must match.
		nfa_state_T	*state;
		പൂർണ്ണസംഖ്യ		result_if_matched;
		പൂർണ്ണസംഖ്യ		c1, c2;

		// Never match EOL. If it's part of the collection it is added
		// as a separate state with an OR.
		എങ്കിൽ (curc == NUL)
		    നിർത്തുക;

		state = t->state->out;
		result_if_matched = (t->state->c == NFA_START_COLL);
		വേണ്ടി (;;)
		{
		    എങ്കിൽ (state->c == NFA_COMPOSING)
		    {
			nfa_state_T *end;

			result = match_composing(t->state->out->out, curc,
									clen);

			എങ്കിൽ (t->state->out->out1 != ശൂന്യ
				&& t->state->out->out1->c == NFA_END_COMPOSING)
			{
			    end = t->state->out->out1;
			    ADD_STATE_IF_MATCH(end);
			}
			നിർത്തുക;
		    }
		    എങ്കിൽ (state->c == NFA_END_COLL)
		    {
			result = !result_if_matched;
			നിർത്തുക;
		    }
		    എങ്കിൽ (state->c == NFA_RANGE_MIN)
		    {
			c1 = state->val;
			state = state->out; // advance to NFA_RANGE_MAX
			c2 = state->val;
#ifdef ENABLE_LOG
			fprintf(log_fd, "NFA_RANGE_MIN curc=%d c1=%d c2=%d\n",
				curc, c1, c2);
#endif
			എങ്കിൽ (curc >= c1 && curc <= c2)
			{
			    result = result_if_matched;
			    നിർത്തുക;
			}
			എങ്കിൽ (rex.reg_ic)
			{
			    പൂർണ്ണസംഖ്യ curc_താഴെയുള്ള = MB_CASEFOLD(curc);
			    പൂർണ്ണസംഖ്യ done = FALSE;

			    വേണ്ടി ( ; c1 <= c2; ++c1)
				എങ്കിൽ (MB_CASEFOLD(c1) == curc_താഴെയുള്ള)
				{
				    result = result_if_matched;
				    done = TRUE;
				    നിർത്തുക;
				}
			    എങ്കിൽ (done)
				നിർത്തുക;
			}
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (state->c < 0 ? check_char_class(state->c, curc)
			       : (curc == state->c
				   || (rex.reg_ic && MB_CASEFOLD(curc)
						    == MB_CASEFOLD(state->c))))
		    {
			result = result_if_matched;
			നിർത്തുക;
		    }
		    state = state->out;
		}
		എങ്കിൽ (result)
		{
		    // next state is in out of the NFA_END_COLL, out1 of
		    // START points to the END state
		    add_state = t->state->out1->out;
		    add_off = clen;
		}
		നിർത്തുക;
	      }

	    അവസ്ഥ NFA_ANY:
		// Any char except '\0', (end of input) does not match.
		എങ്കിൽ (curc > 0)
		{
		    add_state = t->state->out;
		    add_off = clen;
		}
		നിർത്തുക;

	    അവസ്ഥ NFA_ANY_COMPOSING:
		// On a composing character skip over it.  Otherwise do
		// nothing.  Always matches.
		എങ്കിൽ (enc_utf8 && utf_iscomposing(curc))
		{
		    add_off = clen;
		}
		അല്ലെങ്കിൽ
		{
		    add_here = TRUE;
		    add_off = 0;
		}
		add_state = t->state->out;
		നിർത്തുക;

	    /*
	     * Character classes like \a for alpha, \d for digit etc.
	     */
	    അവസ്ഥ NFA_IDENT:	//  \i
		result = vim_isIDc(curc);
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_SIDENT:	//  \I
		result = !VIM_ISDIGIT(curc) && vim_isIDc(curc);
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_KWORD:	//  \k
		result = vim_iswordp_buf(rex.നിവേശം, rex.reg_buf);
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_SKWORD:	//  \K
		result = !VIM_ISDIGIT(curc)
				     && vim_iswordp_buf(rex.നിവേശം, rex.reg_buf);
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_FNAME:	//  \f
		result = vim_isfilec(curc);
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_SFNAME:	//  \F
		result = !VIM_ISDIGIT(curc) && vim_isfilec(curc);
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_PRINT:	//  \p
		result = vim_isprintc(PTR2CHAR(rex.നിവേശം));
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_SPRINT:	//  \P
		result = !VIM_ISDIGIT(curc) && vim_isprintc(PTR2CHAR(rex.നിവേശം));
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_WHITE:	//  \s
		result = VIM_ISWHITE(curc);
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_NWHITE:	//  \S
		result = curc != NUL && !VIM_ISWHITE(curc);
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_DIGIT:	//  \d
		result = ri_digit(curc);
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_NDIGIT:	//  \D
		result = curc != NUL && !ri_digit(curc);
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_HEX:	//  \x
		result = ri_hex(curc);
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_NHEX:	//  \X
		result = curc != NUL && !ri_hex(curc);
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_OCTAL:	//  \o
		result = ri_octal(curc);
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_NOCTAL:	//  \O
		result = curc != NUL && !ri_octal(curc);
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_WORD:	//  \w
		result = ri_word(curc);
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_NWORD:	//  \W
		result = curc != NUL && !ri_word(curc);
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_HEAD:	//  \h
		result = ri_head(curc);
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_NHEAD:	//  \H
		result = curc != NUL && !ri_head(curc);
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_ALPHA:	//  \a
		result = ri_alpha(curc);
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_NALPHA:	//  \A
		result = curc != NUL && !ri_alpha(curc);
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_LOWER:	//  \l
		result = ri_lower(curc);
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_NLOWER:	//  \L
		result = curc != NUL && !ri_lower(curc);
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_UPPER:	//  \u
		result = ri_upper(curc);
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_NUPPER:	// \U
		result = curc != NUL && !ri_upper(curc);
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_LOWER_IC:	// [a-z]
		result = ri_lower(curc) || (rex.reg_ic && ri_upper(curc));
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_NLOWER_IC:	// [^a-z]
		result = curc != NUL
			&& !(ri_lower(curc) || (rex.reg_ic && ri_upper(curc)));
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_UPPER_IC:	// [A-Z]
		result = ri_upper(curc) || (rex.reg_ic && ri_lower(curc));
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_NUPPER_IC:	// ^[A-Z]
		result = curc != NUL
			&& !(ri_upper(curc) || (rex.reg_ic && ri_lower(curc)));
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;

	    അവസ്ഥ NFA_BACKREF1:
	    അവസ്ഥ NFA_BACKREF2:
	    അവസ്ഥ NFA_BACKREF3:
	    അവസ്ഥ NFA_BACKREF4:
	    അവസ്ഥ NFA_BACKREF5:
	    അവസ്ഥ NFA_BACKREF6:
	    അവസ്ഥ NFA_BACKREF7:
	    അവസ്ഥ NFA_BACKREF8:
	    അവസ്ഥ NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	    അവസ്ഥ NFA_ZREF1:
	    അവസ്ഥ NFA_ZREF2:
	    അവസ്ഥ NFA_ZREF3:
	    അവസ്ഥ NFA_ZREF4:
	    അവസ്ഥ NFA_ZREF5:
	    അവസ്ഥ NFA_ZREF6:
	    അവസ്ഥ NFA_ZREF7:
	    അവസ്ഥ NFA_ZREF8:
	    അവസ്ഥ NFA_ZREF9:
#endif
		// \1 .. \9  \z1 .. \z9
	      {
		പൂർണ്ണസംഖ്യ subidx;
		പൂർണ്ണസംഖ്യ bytelen;

#ifdef FEAT_SYN_HL
		എങ്കിൽ (t->state->c >= NFA_BACKREF1 && t->state->c <= NFA_BACKREF9)
#endif
		{
		    subidx = t->state->c - NFA_BACKREF1 + 1;
		    result = match_backref(&t->subs.norm, subidx, &bytelen);
		}
#ifdef FEAT_SYN_HL
		അല്ലെങ്കിൽ
		{
		    subidx = t->state->c - NFA_ZREF1 + 1;
		    result = match_zref(subidx, &bytelen);
		}
#endif

		എങ്കിൽ (result)
		{
		    എങ്കിൽ (bytelen == 0)
		    {
			// empty match always works, output of NFA_SKIP to be
			// used next
			add_here = TRUE;
			add_state = t->state->out->out;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (bytelen <= clen)
		    {
			// match current character, jump ahead to out of
			// NFA_SKIP
			add_state = t->state->out->out;
			add_off = clen;
		    }
		    അല്ലെങ്കിൽ
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			add_state = t->state->out;
			add_off = bytelen;
			add_എണ്ണം = bytelen - clen;
		    }
		}
		നിർത്തുക;
	      }
	    അവസ്ഥ NFA_SKIP:
	      // character of previous matching \1 .. \9  or \@>
	      എങ്കിൽ (t->എണ്ണം - clen <= 0)
	      {
		  // end of match, go to what follows
		  add_state = t->state->out;
		  add_off = clen;
	      }
	      അല്ലെങ്കിൽ
	      {
		  // add state again with decremented count
		  add_state = t->state;
		  add_off = 0;
		  add_എണ്ണം = t->എണ്ണം - clen;
	      }
	      നിർത്തുക;

	    അവസ്ഥ NFA_LNUM:
	    അവസ്ഥ NFA_LNUM_GT:
	    അവസ്ഥ NFA_LNUM_LT:
		result = (REG_MULTI &&
			nfa_re_അക്കം_cmp(t->state->val, t->state->c - NFA_LNUM,
			    (long_u)(rex.lnum + rex.reg_firstlnum)));
		എങ്കിൽ (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		നിർത്തുക;

	    അവസ്ഥ NFA_COL:
	    അവസ്ഥ NFA_COL_GT:
	    അവസ്ഥ NFA_COL_LT:
		result = nfa_re_അക്കം_cmp(t->state->val, t->state->c - NFA_COL,
			(long_u)(rex.നിവേശം - rex.വരി) + 1);
		എങ്കിൽ (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		നിർത്തുക;

	    അവസ്ഥ NFA_VCOL:
	    അവസ്ഥ NFA_VCOL_GT:
	    അവസ്ഥ NFA_VCOL_LT:
		{
		    പൂർണ്ണസംഖ്യ     op = t->state->c - NFA_VCOL;
		    colnr_T col = (colnr_T)(rex.നിവേശം - rex.വരി);
		    win_T   *wp = rex.reg_win == ശൂന്യ ? curwin : rex.reg_win;

		    // Bail out quickly when there can't be a match, avoid the
		    // overhead of win_linetabsize() on long lines.
		    എങ്കിൽ (op != 1 && col > t->state->val
			    * (has_mbyte ? MB_MAXBYTES : 1))
			നിർത്തുക;
		    result = FALSE;
		    എങ്കിൽ (op == 1 && col - 1 > t->state->val && col > 100)
		    {
			പൂർണ്ണസംഖ്യ ts = wp->w_buffer->b_p_ts;

			// Guess that a character won't use more columns than
			// 'tabstop', with a minimum of 4.
			എങ്കിൽ (ts < 4)
			    ts = 4;
			result = col > t->state->val * ts;
		    }
		    എങ്കിൽ (!result)
		    {
			linenr_T    lnum = REG_MULTI
					    ? rex.reg_firstlnum + rex.lnum : 1;
			long_u	    vcol;

			എങ്കിൽ (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_buffer->b_ml.ml_വരി_എണ്ണം))
			    lnum = 1;
			vcol = (long_u)win_linetabsize(wp, lnum, rex.വരി, col);
			result = nfa_re_അക്കം_cmp(t->state->val, op, vcol + 1);
		    }
		    എങ്കിൽ (result)
		    {
			add_here = TRUE;
			add_state = t->state->out;
		    }
		}
		നിർത്തുക;

	    അവസ്ഥ NFA_MARK:
	    അവസ്ഥ NFA_MARK_GT:
	    അവസ്ഥ NFA_MARK_LT:
	      {
		pos_T	*pos;
		size_t	col = REG_MULTI ? rex.നിവേശം - rex.വരി : 0;

		pos = getmark_buf(rex.reg_buf, t->state->val, FALSE);

		// Line may have been freed, get it again.
		എങ്കിൽ (REG_MULTI)
		{
		    rex.വരി = reg_getline(rex.lnum);
		    rex.നിവേശം = rex.വരി + col;
		}

		// Compare the mark position to the match position, if the mark
		// exists and mark is set in reg_buf.
		എങ്കിൽ (pos != ശൂന്യ && pos->lnum > 0)
		{
		    colnr_T pos_col = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->col == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->col;

		    result = (pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_col == (colnr_T)(rex.നിവേശം - rex.വരി)
				    ? t->state->c == NFA_MARK
				    : (pos_col < (colnr_T)(rex.നിവേശം - rex.വരി)
					? t->state->c == NFA_MARK_GT
					: t->state->c == NFA_MARK_LT))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? t->state->c == NFA_MARK_GT
				    : t->state->c == NFA_MARK_LT));
		    എങ്കിൽ (result)
		    {
			add_here = TRUE;
			add_state = t->state->out;
		    }
		}
		നിർത്തുക;
	      }

	    അവസ്ഥ NFA_CURSOR:
		result = (rex.reg_win != ശൂന്യ
			&& (rex.lnum + rex.reg_firstlnum
						 == rex.reg_win->w_cursor.lnum)
			&& ((colnr_T)(rex.നിവേശം - rex.വരി)
						== rex.reg_win->w_cursor.col));
		എങ്കിൽ (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		നിർത്തുക;

	    അവസ്ഥ NFA_VISUAL:
		result = reg_match_visual();
		എങ്കിൽ (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		നിർത്തുക;

	    അവസ്ഥ NFA_MOPEN1:
	    അവസ്ഥ NFA_MOPEN2:
	    അവസ്ഥ NFA_MOPEN3:
	    അവസ്ഥ NFA_MOPEN4:
	    അവസ്ഥ NFA_MOPEN5:
	    അവസ്ഥ NFA_MOPEN6:
	    അവസ്ഥ NFA_MOPEN7:
	    അവസ്ഥ NFA_MOPEN8:
	    അവസ്ഥ NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	    അവസ്ഥ NFA_ZOPEN:
	    അവസ്ഥ NFA_ZOPEN1:
	    അവസ്ഥ NFA_ZOPEN2:
	    അവസ്ഥ NFA_ZOPEN3:
	    അവസ്ഥ NFA_ZOPEN4:
	    അവസ്ഥ NFA_ZOPEN5:
	    അവസ്ഥ NFA_ZOPEN6:
	    അവസ്ഥ NFA_ZOPEN7:
	    അവസ്ഥ NFA_ZOPEN8:
	    അവസ്ഥ NFA_ZOPEN9:
#endif
	    അവസ്ഥ NFA_NOPEN:
	    അവസ്ഥ NFA_ZSTART:
		// These states are only added to be able to bail out when
		// they are added again, nothing is to be done.
		നിർത്തുക;

	    സ്ഥിരസ്ഥിതി:	// regular character
	      {
		പൂർണ്ണസംഖ്യ c = t->state->c;

#ifdef DEBUG
		എങ്കിൽ (c < 0)
		    siemsg("Negative state char: %ld", (long)c);
#endif
		result = (c == curc);

		എങ്കിൽ (!result && rex.reg_ic)
		    result = MB_CASEFOLD(c) == MB_CASEFOLD(curc);
		// If rex.reg_icombine is not set only skip over the character
		// itself.  When it is set skip over composing characters.
		എങ്കിൽ (result && enc_utf8 && !rex.reg_icombine)
		    clen = utf_ptr2len(rex.നിവേശം);
		ADD_STATE_IF_MATCH(t->state);
		നിർത്തുക;
	      }

	    } // switch (t->state->c)

	    എങ്കിൽ (add_state != ശൂന്യ)
	    {
		nfa_pim_T *pim;
		nfa_pim_T pim_copy;

		എങ്കിൽ (t->pim.result == NFA_PIM_UNUSED)
		    pim = ശൂന്യ;
		അല്ലെങ്കിൽ
		    pim = &t->pim;

		// Handle the postponed invisible match if the match might end
		// without advancing and before the end of the line.
		എങ്കിൽ (pim != ശൂന്യ && (clen == 0 || match_follows(add_state, 0)))
		{
		    എങ്കിൽ (pim->result == NFA_PIM_TODO)
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
			എങ്കിൽ (result != (pim->state->c == NFA_START_INVISIBLE_NEG
			     || pim->state->c == NFA_START_INVISIBLE_NEG_FIRST
			     || pim->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&pim->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
			    എങ്കിൽ (rex.nfa_has_zsubexpr)
				copy_sub_off(&pim->subs.synt, &m->synt);
#endif
			}
		    }
		    അല്ലെങ്കിൽ
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
		    എങ്കിൽ (result != (pim->state->c == NFA_START_INVISIBLE_NEG
			     || pim->state->c == NFA_START_INVISIBLE_NEG_FIRST
			     || pim->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
		    {
			// Copy submatch info from the recursive call
			copy_sub_off(&t->subs.norm, &pim->subs.norm);
#ifdef FEAT_SYN_HL
			എങ്കിൽ (rex.nfa_has_zsubexpr)
			    copy_sub_off(&t->subs.synt, &pim->subs.synt);
#endif
		    }
		    അല്ലെങ്കിൽ
			// look-behind match failed, don't add the state
			തുടരുക;

		    // Postponed invisible match was handled, don't add it to
		    // following states.
		    pim = ശൂന്യ;
		}

		// If "pim" points into l->t it will become invalid when
		// adding the state causes the list to be reallocated.  Make a
		// local copy to avoid that.
		എങ്കിൽ (pim == &t->pim)
		{
		    copy_pim(&pim_copy, pim);
		    pim = &pim_copy;
		}

		എങ്കിൽ (add_here)
		    r = addstate_here(thislist, add_state, &t->subs,
								pim, &listidx);
		അല്ലെങ്കിൽ
		{
		    r = addstate(nextlist, add_state, &t->subs, pim, add_off, 0);
		    എങ്കിൽ (add_എണ്ണം > 0)
			nextlist->t[nextlist->n - 1].എണ്ണം = add_എണ്ണം;
		}
		എങ്കിൽ (r == ശൂന്യ)
		{
		    nfa_match = NFA_TOO_EXPENSIVE;
		    പോകുക theend;
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
	എങ്കിൽ (nfa_match == FALSE
		&& ((toplevel
			&& rex.lnum == 0
			&& clen != 0
			&& (rex.reg_maxcol == 0
			  || (colnr_T)(rex.നിവേശം - rex.വരി) < rex.reg_maxcol))
		    || (nfa_endp != ശൂന്യ
			&& (REG_MULTI
			    ? (rex.lnum < nfa_endp->se_u.pos.lnum
			       || (rex.lnum == nfa_endp->se_u.pos.lnum
				   && (പൂർണ്ണസംഖ്യ)(rex.നിവേശം - rex.വരി)
						    < nfa_endp->se_u.pos.col))
			    : rex.നിവേശം < nfa_endp->se_u.ptr))))
	{
#ifdef ENABLE_LOG
	    fprintf(log_fd, "(---) STARTSTATE\n");
#endif
	    // Inline optimized code for addstate() if we know the state is
	    // the first MOPEN.
	    എങ്കിൽ (toplevel)
	    {
		പൂർണ്ണസംഖ്യ add = TRUE;
		പൂർണ്ണസംഖ്യ c;

		എങ്കിൽ (prog->regstart != NUL && clen != 0)
		{
		    എങ്കിൽ (nextlist->n == 0)
		    {
			colnr_T col = (colnr_T)(rex.നിവേശം - rex.വരി) + clen;

			// Nextlist is empty, we can skip ahead to the
			// character that must appear at the start.
			എങ്കിൽ (skip_to_start(prog->regstart, &col) == FAIL)
			    നിർത്തുക;
#ifdef ENABLE_LOG
			fprintf(log_fd, "  Skipping ahead %d bytes to regstart\n",
				col - ((colnr_T)(rex.നിവേശം - rex.വരി) + clen));
#endif
			rex.നിവേശം = rex.വരി + col - clen;
		    }
		    അല്ലെങ്കിൽ
		    {
			// Checking if the required start character matches is
			// cheaper than adding a state that won't match.
			c = PTR2CHAR(rex.നിവേശം + clen);
			എങ്കിൽ (c != prog->regstart && (!rex.reg_ic
			     || MB_CASEFOLD(c) != MB_CASEFOLD(prog->regstart)))
			{
#ifdef ENABLE_LOG
			    fprintf(log_fd, "  Skipping start state, regstart does not match\n");
#endif
			    add = FALSE;
			}
		    }
		}

		എങ്കിൽ (add)
		{
		    എങ്കിൽ (REG_MULTI)
		    {
			m->norm.list.multi[0].start_col =
					 (colnr_T)(rex.നിവേശം - rex.വരി) + clen;
			m->norm.orig_start_col =
					       m->norm.list.multi[0].start_col;
		    }
		    അല്ലെങ്കിൽ
			m->norm.list.വരി[0].start = rex.നിവേശം + clen;
		    എങ്കിൽ (addstate(nextlist, start->out, m, ശൂന്യ, clen, 0) == ശൂന്യ)
		    {
			nfa_match = NFA_TOO_EXPENSIVE;
			പോകുക theend;
		    }
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		char_u	    *save_വരി = rex.വരി;
		char_u	    *save_നിവേശം = rex.നിവേശം;
		linenr_T    save_lnum = rex.lnum;

		// At the end of a line the match can only start on the next
		// line, use that position instead of the line break.
		എങ്കിൽ (REG_MULTI && clen == 0 && nfa_endp != ശൂന്യ
					 && rex.lnum < nfa_endp->se_u.pos.lnum)
		{
		    char_u  *next_വരി = reg_getline(rex.lnum + 1);

		    എങ്കിൽ (next_വരി != ശൂന്യ)
		    {
			rex.വരി = next_വരി;
			rex.നിവേശം = rex.വരി;
			++rex.lnum;
		    }
		}

		r = addstate(nextlist, start, m, ശൂന്യ, clen, 0);

		rex.വരി = save_വരി;
		rex.നിവേശം = save_നിവേശം;
		rex.lnum = save_lnum;

		എങ്കിൽ (r == ശൂന്യ)
		{
		    nfa_match = NFA_TOO_EXPENSIVE;
		    പോകുക theend;
		}
	    }
	}

#ifdef ENABLE_LOG
	fprintf(log_fd, ">>> Thislist had %d states available: ", thislist->n);
	{
	    പൂർണ്ണസംഖ്യ i;

	    വേണ്ടി (i = 0; i < thislist->n; i++)
		fprintf(log_fd, "%d  ", abs(thislist->t[i].state->id));
	}
	fprintf(log_fd, "\n");
#endif

nextchar:
	// Advance to the next character, or advance to the next line, or
	// finish.
	എങ്കിൽ (clen != 0)
	    rex.നിവേശം += clen;
	അല്ലെങ്കിൽ എങ്കിൽ (go_to_nextline || (nfa_endp != ശൂന്യ && REG_MULTI
					&& rex.lnum < nfa_endp->se_u.pos.lnum))
	    reg_nextline();
	അല്ലെങ്കിൽ
	    നിർത്തുക;

	// Allow interrupting with CTRL-C.
	വരി_breakcheck();
	എങ്കിൽ (got_int)
	    നിർത്തുക;
#ifdef FEAT_RELTIME
	എങ്കിൽ (nfa_did_time_out())
	    നിർത്തുക;
#endif
    }

#ifdef ENABLE_LOG
    എങ്കിൽ (log_fd != stderr)
	fclose(log_fd);
    log_fd = ശൂന്യ;
#endif

theend:
    // Cache list buffers in prog for reuse, or free if prog already has
    // cached buffers (recursive call case).
    എങ്കിൽ (prog->listbuf[0] == ശൂന്യ && list[0].t != ശൂന്യ && list[1].t != ശൂന്യ)
    {
	prog->listbuf[0] = list[0].t;
	prog->listbuf[1] = list[1].t;
    }
    അല്ലെങ്കിൽ
    {
	vim_free(list[0].t);
	vim_free(list[1].t);
    }
    vim_free(listids);
#undef ADD_STATE_IF_MATCH
#ifdef NFA_REGEXP_DEBUG_LOG
    fclose(debug);
#endif

    തിരികെ nfa_match;
}

/*
 * Try match of "prog" with at rex.line["col"].
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    സ്ഥിതം long
nfa_regtry(
    nfa_regprog_T   *prog,
    colnr_T	    col,
    പൂർണ്ണസംഖ്യ		    *timed_out UNUSED)	// flag set on timeout or NULL
{
    പൂർണ്ണസംഖ്യ		i;
    regsubs_T	subs, m;
    nfa_state_T	*start = prog->start;
    പൂർണ്ണസംഖ്യ		result;
#ifdef ENABLE_LOG
    FILE	*f;
#endif

    rex.നിവേശം = rex.വരി + col;
#ifdef FEAT_RELTIME
    nfa_timed_out = timed_out;
#endif

#ifdef ENABLE_LOG
    f = fopen(NFA_REGEXP_RUN_LOG, "a");
    എങ്കിൽ (f != ശൂന്യ)
    {
	fprintf(f, "\n\n\t=======================================================\n");
# ifdef DEBUG
	fprintf(f, "\tRegexp is \"%s\"\n", nfa_regengine.വ്യഞ്ജകം);
# endif
	fprintf(f, "\tInput text is \"%s\" \n", rex.നിവേശം);
	fprintf(f, "\t=======================================================\n\n");
	nfa_print_state(f, start);
	fprintf(f, "\n\n");
	fclose(f);
    }
    അല്ലെങ്കിൽ
	emsg("Could not open temporary log file for writing");
#endif

    clear_sub(&subs.norm);
    clear_sub(&m.norm);
#ifdef FEAT_SYN_HL
    clear_sub(&subs.synt);
    clear_sub(&m.synt);
#endif

    result = nfa_regmatch(prog, start, &subs, &m);
    എങ്കിൽ (result == FALSE)
	തിരികെ 0;
    അല്ലെങ്കിൽ എങ്കിൽ (result == NFA_TOO_EXPENSIVE)
	തിരികെ result;

    cleanup_subexpr();
    എങ്കിൽ (REG_MULTI)
    {
	വേണ്ടി (i = 0; i < subs.norm.in_use; i++)
	{
	    rex.reg_startpos[i].lnum = subs.norm.list.multi[i].start_lnum;
	    rex.reg_startpos[i].col = subs.norm.list.multi[i].start_col;

	    rex.reg_endpos[i].lnum = subs.norm.list.multi[i].end_lnum;
	    rex.reg_endpos[i].col = subs.norm.list.multi[i].end_col;
	}
	എങ്കിൽ (rex.reg_mmatch != ശൂന്യ)
	    rex.reg_mmatch->rmm_matchcol = subs.norm.orig_start_col;

	എങ്കിൽ (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].col = col;
	}
	എങ്കിൽ (rex.reg_endpos[0].lnum < 0)
	{
	    // pattern has a \ze but it didn't match, use current end
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].col = (പൂർണ്ണസംഖ്യ)(rex.നിവേശം - rex.വരി);
	}
	അല്ലെങ്കിൽ
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    അല്ലെങ്കിൽ
    {
	വേണ്ടി (i = 0; i < subs.norm.in_use; i++)
	{
	    rex.reg_startp[i] = subs.norm.list.വരി[i].start;
	    rex.reg_endp[i] = subs.norm.list.വരി[i].end;
	}

	എങ്കിൽ (rex.reg_startp[0] == ശൂന്യ)
	    rex.reg_startp[0] = rex.വരി + col;
	എങ്കിൽ (rex.reg_endp[0] == ശൂന്യ)
	    rex.reg_endp[0] = rex.നിവേശം;
    }

#ifdef FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = ശൂന്യ;

    എങ്കിൽ (prog->reghasz == REX_SET)
    {
	cleanup_zsubexpr();
	re_extmatch_out = make_extmatch();
	എങ്കിൽ (re_extmatch_out == ശൂന്യ)
	    തിരികെ 0;
	// Loop over \z1, \z2, etc.  There is no \z0.
	വേണ്ടി (i = 1; i < subs.synt.in_use; i++)
	{
	    എങ്കിൽ (REG_MULTI)
	    {
		ഘടന multipos *mpos = &subs.synt.list.multi[i];

		// Only accept single line matches that are valid.
		എങ്കിൽ (mpos->start_lnum >= 0
			&& mpos->start_lnum == mpos->end_lnum
			&& mpos->end_col >= mpos->start_col)
		    re_extmatch_out->matches[i] =
			vim_strnsave(reg_getline(mpos->start_lnum)
							    + mpos->start_col,
					     mpos->end_col - mpos->start_col);
	    }
	    അല്ലെങ്കിൽ
	    {
		ഘടന linepos *lpos = &subs.synt.list.വരി[i];

		എങ്കിൽ (lpos->start != ശൂന്യ && lpos->end != ശൂന്യ)
		    re_extmatch_out->matches[i] =
			    vim_strnsave(lpos->start, lpos->end - lpos->start);
	    }
	}
    }
#endif

    തിരികെ 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 *
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    സ്ഥിതം long
nfa_regexec_both(
    char_u	*വരി,
    colnr_T	startcol,	// column to start looking for match
    പൂർണ്ണസംഖ്യ		*timed_out)	// flag set on timeout or NULL
{
    nfa_regprog_T   *prog;
    long	    retval = 0L;
    പൂർണ്ണസംഖ്യ		    i;
    colnr_T	    col = startcol;

    എങ്കിൽ (REG_MULTI)
    {
	prog = (nfa_regprog_T *)rex.reg_mmatch->regprog;
	വരി = reg_getline((linenr_T)0);    // relative to the cursor
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    അല്ലെങ്കിൽ
    {
	prog = (nfa_regprog_T *)rex.reg_match->regprog;
	rex.reg_startp = rex.reg_match->startp;
	rex.reg_endp = rex.reg_match->endp;
    }

    // Be paranoid...
    എങ്കിൽ (prog == ശൂന്യ || വരി == ശൂന്യ)
    {
	iemsg(e_null_argument);
	പോകുക theend;
    }

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    എങ്കിൽ (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    അല്ലെങ്കിൽ എങ്കിൽ (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    എങ്കിൽ (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    rex.വരി = വരി;
    rex.lnum = 0;    // relative to line

    rex.nfa_has_zend = prog->has_zend;
    rex.nfa_has_backref = prog->has_backref;
    rex.nfa_nsubexpr = prog->nsubexp;
    rex.nfa_listid = 1;
    rex.nfa_alt_listid = 2;
#ifdef DEBUG
    nfa_regengine.വ്യഞ്ജകം = prog->pattern;
#endif

    എങ്കിൽ (prog->reganch && col > 0)
	തിരികെ 0L;

    rex.need_clear_subexpr = TRUE;
#ifdef FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    എങ്കിൽ (prog->reghasz == REX_SET)
    {
	rex.nfa_has_zsubexpr = TRUE;
	rex.need_clear_zsubexpr = TRUE;
    }
    അല്ലെങ്കിൽ
    {
	rex.nfa_has_zsubexpr = FALSE;
	rex.need_clear_zsubexpr = FALSE;
    }
#endif

    എങ്കിൽ (prog->regstart != NUL)
    {
	// Skip ahead until a character we know the match must start with.
	// When there is none there is no match.
	എങ്കിൽ (skip_to_start(prog->regstart, &col) == FAIL)
	    തിരികെ 0L;

	// If match_text is set it contains the full text that must match.
	// Nothing else to try. Doesn't handle combining chars well.
	എങ്കിൽ (prog->match_text != ശൂന്യ && *prog->match_text != NUL && !rex.reg_icombine)
	{
	    retval = find_match_text(&col, prog->regstart, prog->match_text);
	    എങ്കിൽ (REG_MULTI)
		rex.reg_mmatch->rmm_matchcol = col;
	    അല്ലെങ്കിൽ
		rex.reg_match->rm_matchcol = col;
	    തിരികെ retval;
	}
    }

    // If the start column is past the maximum column: no need to try.
    എങ്കിൽ (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	പോകുക theend;

    // Set the "nstate" used by nfa_regcomp() to zero to trigger an error when
    // it's accidentally used during execution.
    nstate = 0;
    വേണ്ടി (i = 0; i < prog->nstate; ++i)
    {
	prog->state[i].id = i;
	prog->state[i].lastlist[0] = 0;
	prog->state[i].lastlist[1] = 0;
    }

    retval = nfa_regtry(prog, col, timed_out);

#ifdef DEBUG
    nfa_regengine.വ്യഞ്ജകം = ശൂന്യ;
#endif

theend:
    എങ്കിൽ (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs and
	// \ze are used.
	എങ്കിൽ (REG_MULTI)
	{
	    lpos_T *start = &rex.reg_mmatch->startpos[0];
	    lpos_T *end = &rex.reg_mmatch->endpos[0];

	    എങ്കിൽ (end->lnum < start->lnum
			|| (end->lnum == start->lnum && end->col < start->col))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (rex.reg_match->endp[0] < rex.reg_match->startp[0])
		rex.reg_match->endp[0] = rex.reg_match->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_match->rm_matchcol = col;
	}
    }

    തിരികെ retval;
}

/*
 * Compile a regular expression into internal code for the NFA matcher.
 * Returns the program in allocated space.  Returns NULL for an error.
 */
    സ്ഥിതം regprog_T *
nfa_regcomp(char_u *വ്യഞ്ജകം, പൂർണ്ണസംഖ്യ re_flags)
{
    nfa_regprog_T	*prog = ശൂന്യ;
    size_t		prog_size;
    പൂർണ്ണസംഖ്യ			*postfix;

    എങ്കിൽ (വ്യഞ്ജകം == ശൂന്യ)
	തിരികെ ശൂന്യ;

#ifdef DEBUG
    nfa_regengine.വ്യഞ്ജകം = വ്യഞ്ജകം;
#endif
    nfa_re_flags = re_flags;

    init_class_tab();

    എങ്കിൽ (nfa_regcomp_start(വ്യഞ്ജകം, re_flags) == FAIL)
	തിരികെ ശൂന്യ;

    // Build postfix form of the regexp. Needed to build the NFA
    // (and count its size).
    postfix = re2post();
    എങ്കിൽ (postfix == ശൂന്യ)
	പോകുക fail;	    // Cascaded (syntax?) error

    /*
     * In order to build the NFA, we parse the input regexp twice:
     * 1. first pass to count size (so we can allocate space)
     * 2. second to emit code
     */
#ifdef ENABLE_LOG
    {
	FILE *f = fopen(NFA_REGEXP_RUN_LOG, "a");

	എങ്കിൽ (f != ശൂന്യ)
	{
	    fprintf(f, "\n*****************************\n\n\n\n\tCompiling regexp \"%s\"... hold on !\n", വ്യഞ്ജകം);
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
    prog_size = offsetof(nfa_regprog_T, state) + വലുപ്പം(nfa_state_T) * nstate;
    prog = alloc(prog_size);
    എങ്കിൽ (prog == ശൂന്യ)
	പോകുക fail;
    state_ptr = prog->state;
    prog->re_in_use = FALSE;
    prog->listbuf[0] = ശൂന്യ;
    prog->listbuf[1] = ശൂന്യ;

    /*
     * PASS 2
     * Build the NFA
     */
    prog->start = post2nfa(postfix, post_ptr, FALSE);
    എങ്കിൽ (prog->start == ശൂന്യ)
	പോകുക fail;

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
    nfa_postfix_dump(വ്യഞ്ജകം, OK);
    nfa_dump(prog);
#endif
#ifdef FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    prog->reghasz = re_has_z;
#endif
    prog->pattern = vim_strsave(വ്യഞ്ജകം);
#ifdef DEBUG
    nfa_regengine.വ്യഞ്ജകം = ശൂന്യ;
#endif

out:
    // The postfix list and fragment stack are reused by the next compilation
    // (and freed in free_regexp_stuff()), but drop ones that grew large for a
    // big pattern to avoid holding much memory.
    എങ്കിൽ (post_start_len > NFA_POSTFIX_KEEP)
    {
	VIM_CLEAR(post_start);
	post_start_len = 0;
    }
    post_ptr = post_end = ശൂന്യ;
    എങ്കിൽ (nfa_അട്ടി_len > NFA_അട്ടി_KEEP)
    {
	VIM_CLEAR(nfa_അട്ടി);
	nfa_അട്ടി_len = 0;
    }
    state_ptr = ശൂന്യ;
    തിരികെ (regprog_T *)prog;

fail:
    VIM_CLEAR(prog);
#ifdef ENABLE_LOG
    nfa_postfix_dump(വ്യഞ്ജകം, FAIL);
#endif
#ifdef DEBUG
    nfa_regengine.വ്യഞ്ജകം = ശൂന്യ;
#endif
    പോകുക out;
}

/*
 * Free a compiled regexp program, returned by nfa_regcomp().
 */
    സ്ഥിതം ശൂന്യം
nfa_regfree(regprog_T *prog)
{
    എങ്കിൽ (prog == ശൂന്യ)
	തിരികെ;

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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
nfa_regexec_nl(
    regmatch_T	*rmp,
    char_u	*വരി,	// string to match against
    colnr_T	col,	// column to start looking for match
    പൂർണ്ണസംഖ്യ		വരി_lbr)
{
    rex.reg_match = rmp;
    rex.reg_mmatch = ശൂന്യ;
    rex.reg_maxline = 0;
    rex.reg_വരി_lbr = വരി_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = ശൂന്യ;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;
    തിരികെ nfa_regexec_both(വരി, col, ശൂന്യ);
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
    സ്ഥിതം long
nfa_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	col,		// column to start looking for match
    പൂർണ്ണസംഖ്യ		*timed_out)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    തിരികെ nfa_regexec_both(ശൂന്യ, col, timed_out);
}

#ifdef DEBUG
# undef ENABLE_LOG
#endif
