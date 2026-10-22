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

எண்ணிக்கை
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
நிலையான மாறிலி முழு nfa_classcodes[] = {
    NFA_ANY, NFA_IDENT, NFA_SIDENT, NFA_KWORD,NFA_SKWORD,
    NFA_FNAME, NFA_SFNAME, NFA_PRINT, NFA_SPRINT,
    NFA_WHITE, NFA_NWHITE, NFA_DIGIT, NFA_NDIGIT,
    NFA_HEX, NFA_NHEX, NFA_OCTAL, NFA_NOCTAL,
    NFA_WORD, NFA_NWORD, NFA_HEAD, NFA_NHEAD,
    NFA_ALPHA, NFA_NALPHA, NFA_LOWER, NFA_NLOWER,
    NFA_UPPER, NFA_NUPPER
};

// Variables only used in nfa_regcomp() and descendants.
நிலையான முழு nfa_re_flags; // re_flags passed to nfa_regcomp()
நிலையான முழு *post_start;  // holds the postfix form of r.e.
நிலையான முழு post_start_len;  // size of allocated post_start (in ints)
நிலையான முழு *post_end;
நிலையான முழு *post_ptr;
நிலையான முழு nfa_reg_parse_depth;	// nesting depth in nfa_reg()

// The postfix list (post_start) and fragment stack (nfa_stack) are reused
// across compilations; a buffer that grew past these sizes for a big pattern
// is freed afterwards instead of being kept, to avoid holding much memory.
#define NFA_POSTFIX_KEEP	10000	// number of ints (~40 Kbyte)
#define NFA_அடுக்கு_KEEP		4000	// number of Frag_T (~64 Kbyte)

// Set when the pattern should use the NFA engine.
// E.g. [[:upper:]] only allows 8bit characters for BT engine,
// while NFA engine handles multibyte characters correctly.
நிலையான முழு wants_nfa;

நிலையான முழு nstate;	// Number of states in the NFA.
நிலையான முழு istate;	// Index in the state vector, used in alloc_state()

// If not NULL match must end at this position
நிலையான save_se_T *nfa_endp = வெற்று;

// 0 for first call to nfa_regmatch(), 1 for recursive call.
நிலையான முழு nfa_ll_index = 0;

நிலையான முழு realloc_post_list(வெறுமை);
நிலையான முழு nfa_reg(முழு paren);
#ifdef DEBUG
நிலையான வெறுமை nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent);
#endif
நிலையான முழு match_follows(nfa_state_T *startstate, முழு depth);
நிலையான முழு failure_chance(nfa_state_T *state, முழு depth);

// helper functions used when doing re2post() ... regatom() parsing
#define EMIT(c)	செய் {				\
		    எனில் (post_ptr >= post_end && realloc_post_list() == FAIL) \
			திரும்பு FAIL;		\
		    *post_ptr++ = c;		\
		} போது (0)

/*
 * Initialize internal variables before NFA compilation.
 * Return OK on success, FAIL otherwise.
 */
    நிலையான முழு
nfa_regcomp_start(
    char_u	*கோவை,
    முழு		re_flags)	    // see vim_regcomp()
{
    size_t	postfix_size;
    முழு		nstate_max;

    nstate = 0;
    istate = 0;
    // A reasonable estimation for maximum size
    nstate_max = (முழு)(STRLEN(கோவை) + 1) * 25;

    // Some items blow up in size, such as [A-z].  Add more space for that.
    // When it is still not enough realloc_post_list() will be used.
    nstate_max += 1000;

    // Size for postfix representation of expr.
    postfix_size = அளவு(முழு) * nstate_max;

    // Reuse the postfix buffer across compilations, only growing it when the
    // estimate exceeds the current size; it is freed in free_regexp_stuff().
    எனில் (post_start == வெற்று || post_start_len < nstate_max)
    {
	vim_free(post_start);
	post_start = alloc(postfix_size);
	எனில் (post_start == வெற்று)
	{
	    post_start_len = 0;
	    திரும்பு FAIL;
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
    regcomp_start(கோவை, re_flags);

    திரும்பு OK;
}

/*
 * Figure out if the NFA state list starts with an anchor, must match at start
 * of the line.
 */
    நிலையான முழு
nfa_get_reganch(nfa_state_T *start, முழு depth)
{
    nfa_state_T *p = start;

    எனில் (depth > 4)
	திரும்பு 0;

    போது (p != வெற்று)
    {
	தேர்வு (p->c)
	{
	    நிலை NFA_BOL:
	    நிலை NFA_BOF:
		திரும்பு 1; // yes!

	    நிலை NFA_ZSTART:
	    நிலை NFA_ZEND:
	    நிலை NFA_CURSOR:
	    நிலை NFA_VISUAL:

	    நிலை NFA_MOPEN:
	    நிலை NFA_MOPEN1:
	    நிலை NFA_MOPEN2:
	    நிலை NFA_MOPEN3:
	    நிலை NFA_MOPEN4:
	    நிலை NFA_MOPEN5:
	    நிலை NFA_MOPEN6:
	    நிலை NFA_MOPEN7:
	    நிலை NFA_MOPEN8:
	    நிலை NFA_MOPEN9:
	    நிலை NFA_NOPEN:
#ifdef FEAT_SYN_HL
	    நிலை NFA_ZOPEN:
	    நிலை NFA_ZOPEN1:
	    நிலை NFA_ZOPEN2:
	    நிலை NFA_ZOPEN3:
	    நிலை NFA_ZOPEN4:
	    நிலை NFA_ZOPEN5:
	    நிலை NFA_ZOPEN6:
	    நிலை NFA_ZOPEN7:
	    நிலை NFA_ZOPEN8:
	    நிலை NFA_ZOPEN9:
#endif
		p = p->out;
		நிறுத்து;

	    நிலை NFA_SPLIT:
		திரும்பு nfa_get_reganch(p->out, depth + 1)
				       && nfa_get_reganch(p->out1, depth + 1);

	    இயல்பு:
		திரும்பு 0; // noooo
	}
    }
    திரும்பு 0;
}

/*
 * Figure out if the NFA state list starts with a character which must match
 * at start of the match.
 */
    நிலையான முழு
nfa_get_regstart(nfa_state_T *start, முழு depth)
{
    nfa_state_T *p = start;

    எனில் (depth > 4)
	திரும்பு 0;

    போது (p != வெற்று)
    {
	தேர்வு (p->c)
	{
	    // all kinds of zero-width matches
	    நிலை NFA_BOL:
	    நிலை NFA_BOF:
	    நிலை NFA_BOW:
	    நிலை NFA_EOW:
	    நிலை NFA_ZSTART:
	    நிலை NFA_ZEND:
	    நிலை NFA_CURSOR:
	    நிலை NFA_VISUAL:
	    நிலை NFA_LNUM:
	    நிலை NFA_LNUM_GT:
	    நிலை NFA_LNUM_LT:
	    நிலை NFA_COL:
	    நிலை NFA_COL_GT:
	    நிலை NFA_COL_LT:
	    நிலை NFA_VCOL:
	    நிலை NFA_VCOL_GT:
	    நிலை NFA_VCOL_LT:
	    நிலை NFA_MARK:
	    நிலை NFA_MARK_GT:
	    நிலை NFA_MARK_LT:

	    நிலை NFA_MOPEN:
	    நிலை NFA_MOPEN1:
	    நிலை NFA_MOPEN2:
	    நிலை NFA_MOPEN3:
	    நிலை NFA_MOPEN4:
	    நிலை NFA_MOPEN5:
	    நிலை NFA_MOPEN6:
	    நிலை NFA_MOPEN7:
	    நிலை NFA_MOPEN8:
	    நிலை NFA_MOPEN9:
	    நிலை NFA_NOPEN:
#ifdef FEAT_SYN_HL
	    நிலை NFA_ZOPEN:
	    நிலை NFA_ZOPEN1:
	    நிலை NFA_ZOPEN2:
	    நிலை NFA_ZOPEN3:
	    நிலை NFA_ZOPEN4:
	    நிலை NFA_ZOPEN5:
	    நிலை NFA_ZOPEN6:
	    நிலை NFA_ZOPEN7:
	    நிலை NFA_ZOPEN8:
	    நிலை NFA_ZOPEN9:
#endif
		p = p->out;
		நிறுத்து;

	    நிலை NFA_SPLIT:
	    {
		முழு c1 = nfa_get_regstart(p->out, depth + 1);
		முழு c2 = nfa_get_regstart(p->out1, depth + 1);

		எனில் (c1 == c2)
		    திரும்பு c1; // yes!
		திரும்பு 0;
	    }

	    இயல்பு:
		எனில் (p->c > 0)
		    திரும்பு p->c; // yes!
		திரும்பு 0;
	}
    }
    திரும்பு 0;
}

/*
 * Figure out if the NFA state list contains just literal text and nothing
 * else.  If so return a string in allocated memory with what must match after
 * regstart.  Otherwise return NULL.
 */
    நிலையான char_u *
nfa_get_match_text(nfa_state_T *start)
{
    nfa_state_T *p = start;
    முழு		len = 0;
    char_u	*ret;
    char_u	*s;

    எனில் (p->c != NFA_MOPEN)
	திரும்பு வெற்று; // just in case
    p = p->out;
    போது (p->c > 0)
    {
	len += MB_CHAR2LEN(p->c);
	p = p->out;
    }
    எனில் (p->c != NFA_MCLOSE || p->out->c != NFA_MATCH)
	திரும்பு வெற்று;

    ret = alloc(len);
    எனில் (ret == வெற்று)
	திரும்பு வெற்று;

    p = start->out->out; // skip first char, it goes into regstart
    s = ret;
    போது (p->c > 0)
    {
	எனில் (has_mbyte)
	    s += (*mb_char2bytes)(p->c, s);
	இல்லையெனில்
	    *s++ = p->c;
	p = p->out;
    }
    *s = NUL;
    திரும்பு ret;
}

/*
 * Allocate more space for post_start.  Called when
 * running above the estimated number of states.
 */
    நிலையான முழு
realloc_post_list(வெறுமை)
{
    முழு   nstate_max = (முழு)(post_end - post_start);
    முழு   new_max;
    முழு   *new_start;
    முழு	  *old_start;

    // For weird patterns the number of states can be very high. Increasing by
    // 50% seems a reasonable compromise between memory use and speed.
    new_max = nstate_max * 3 / 2;
    new_start = ALLOC_MULT(முழு, new_max);
    எனில் (new_start == வெற்று)
	திரும்பு FAIL;
    mch_memmove(new_start, post_start, nstate_max * அளவு(முழு));
    old_start = post_start;
    post_start = new_start;
    post_start_len = new_max;
    post_ptr = new_start + (post_ptr - old_start);
    post_end = post_start + new_max;
    vim_free(old_start);
    திரும்பு OK;
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
    நிலையான முழு
nfa_recognize_char_class(char_u *start, char_u *end, முழு extra_newl)
{
    முழு		newl;
    முழு		config = get_char_class_bits(start, end, &newl);

    எனில் (config < 0)
	திரும்பு FAIL;

    எனில் (extra_newl == TRUE)
	newl = TRUE;

    எனில் (newl == TRUE)
	extra_newl = NFA_ADD_NL;

    தேர்வு (config)
    {
	நிலை CLASS_o9:
	    திரும்பு extra_newl + NFA_DIGIT;
	நிலை CLASS_not |  CLASS_o9:
	    திரும்பு extra_newl + NFA_NDIGIT;
	நிலை CLASS_af | CLASS_AF | CLASS_o9:
	    திரும்பு extra_newl + NFA_HEX;
	நிலை CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    திரும்பு extra_newl + NFA_NHEX;
	நிலை CLASS_o7:
	    திரும்பு extra_newl + NFA_OCTAL;
	நிலை CLASS_not | CLASS_o7:
	    திரும்பு extra_newl + NFA_NOCTAL;
	நிலை CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    திரும்பு extra_newl + NFA_WORD;
	நிலை CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    திரும்பு extra_newl + NFA_NWORD;
	நிலை CLASS_az | CLASS_AZ | CLASS_underscore:
	    திரும்பு extra_newl + NFA_HEAD;
	நிலை CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    திரும்பு extra_newl + NFA_NHEAD;
	நிலை CLASS_az | CLASS_AZ:
	    திரும்பு extra_newl + NFA_ALPHA;
	நிலை CLASS_not | CLASS_az | CLASS_AZ:
	    திரும்பு extra_newl + NFA_NALPHA;
	நிலை CLASS_az:
	   திரும்பு extra_newl + NFA_LOWER_IC;
	நிலை CLASS_not | CLASS_az:
	    திரும்பு extra_newl + NFA_NLOWER_IC;
	நிலை CLASS_AZ:
	    திரும்பு extra_newl + NFA_UPPER_IC;
	நிலை CLASS_not | CLASS_AZ:
	    திரும்பு extra_newl + NFA_NUPPER_IC;
    }
    திரும்பு FAIL;
}

/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * Emits bytes in postfix notation: 'a,b,NFA_OR,c,NFA_OR' is
 * equivalent to 'a OR b OR c'
 *
 * NOTE! When changing this function, also update reg_equi_class()
 */
    நிலையான முழு
nfa_emit_equi_class(முழு c)
{
#define EMIT2(c)    EMIT(c); EMIT(NFA_CONCAT);

    எனில் (enc_utf8 || STRCMP(p_enc, "latin1") == 0
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
	தேர்வு (c)
	{
	    நிலை 'A': நிலை A_grave: நிலை A_acute: நிலை A_circumflex:
	    நிலை A_virguilla: நிலை A_diaeresis: நிலை A_ring:
	    நிலை 0x100: நிலை 0x102: நிலை 0x104: நிலை 0x1cd:
	    நிலை 0x1de: நிலை 0x1e0: நிலை 0x1fa: நிலை 0x200:
	    நிலை 0x202: நிலை 0x226: நிலை 0x23a: நிலை 0x1e00:
	    நிலை 0x1ea0: நிலை 0x1ea2: நிலை 0x1ea4: நிலை 0x1ea6:
	    நிலை 0x1ea8: நிலை 0x1eaa: நிலை 0x1eac: நிலை 0x1eae:
	    நிலை 0x1eb0: நிலை 0x1eb2: நிலை 0x1eb4: நிலை 0x1eb6:
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
		    திரும்பு OK;

	    நிலை 'B': நிலை 0x181: நிலை 0x243: நிலை 0x1e02:
	    நிலை 0x1e04: நிலை 0x1e06:
		    EMIT2('B')
		    EMIT2(0x181) EMIT2(0x243) EMIT2(0x1e02)
		    EMIT2(0x1e04) EMIT2(0x1e06)
		    திரும்பு OK;

	    நிலை 'C': நிலை C_cedilla: நிலை 0x106: நிலை 0x108:
	    நிலை 0x10a: நிலை 0x10c: நிலை 0x187: நிலை 0x23b:
	    நிலை 0x1e08: நிலை 0xa792:
		    EMIT2('C') EMIT2(C_cedilla)
		    EMIT2(0x106) EMIT2(0x108) EMIT2(0x10a)
		    EMIT2(0x10c) EMIT2(0x187) EMIT2(0x23b)
		    EMIT2(0x1e08) EMIT2(0xa792)
		    திரும்பு OK;

	    நிலை 'D': நிலை 0x10e: நிலை 0x110: நிலை 0x18a:
	    நிலை 0x1e0a: நிலை 0x1e0c: நிலை 0x1e0e: நிலை 0x1e10:
	    நிலை 0x1e12:
		    EMIT2('D') EMIT2(0x10e) EMIT2(0x110) EMIT2(0x18a)
		    EMIT2(0x1e0a) EMIT2(0x1e0c) EMIT2(0x1e0e)
		    EMIT2(0x1e10) EMIT2(0x1e12)
		    திரும்பு OK;

	    நிலை 'E': நிலை E_grave: நிலை E_acute: நிலை E_circumflex:
	    நிலை E_diaeresis: நிலை 0x112: நிலை 0x114: நிலை 0x116:
	    நிலை 0x118: நிலை 0x11a: நிலை 0x204: நிலை 0x206:
	    நிலை 0x228: நிலை 0x246: நிலை 0x1e14: நிலை 0x1e16:
	    நிலை 0x1e18: நிலை 0x1e1a: நிலை 0x1e1c: நிலை 0x1eb8:
	    நிலை 0x1eba: நிலை 0x1ebc: நிலை 0x1ebe: நிலை 0x1ec0:
	    நிலை 0x1ec2: நிலை 0x1ec4: நிலை 0x1ec6:
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
		    திரும்பு OK;

	    நிலை 'F': நிலை 0x191: நிலை 0x1e1e: நிலை 0xa798:
		    EMIT2('F') EMIT2(0x191) EMIT2(0x1e1e) EMIT2(0xa798)
		    திரும்பு OK;

	    நிலை 'G': நிலை 0x11c: நிலை 0x11e: நிலை 0x120:
	    நிலை 0x122: நிலை 0x193: நிலை 0x1e4: நிலை 0x1e6:
	    நிலை 0x1f4: நிலை 0x1e20: நிலை 0xa7a0:
		    EMIT2('G') EMIT2(0x11c) EMIT2(0x11e) EMIT2(0x120)
		    EMIT2(0x122) EMIT2(0x193) EMIT2(0x1e4)
		    EMIT2(0x1e6) EMIT2(0x1f4) EMIT2(0x1e20)
		    EMIT2(0xa7a0)
		    திரும்பு OK;

	    நிலை 'H': நிலை 0x124: நிலை 0x126: நிலை 0x21e:
	    நிலை 0x1e22: நிலை 0x1e24: நிலை 0x1e26: நிலை 0x1e28:
	    நிலை 0x1e2a: நிலை 0x2c67:
		    EMIT2('H') EMIT2(0x124) EMIT2(0x126) EMIT2(0x21e)
		    EMIT2(0x1e22) EMIT2(0x1e24) EMIT2(0x1e26)
		    EMIT2(0x1e28) EMIT2(0x1e2a) EMIT2(0x2c67)
		    திரும்பு OK;

	    நிலை 'I': நிலை I_grave: நிலை I_acute: நிலை I_circumflex:
	    நிலை I_diaeresis: நிலை 0x128: நிலை 0x12a: நிலை 0x12c:
	    நிலை 0x12e: நிலை 0x130: நிலை 0x197: நிலை 0x1cf:
	    நிலை 0x208: நிலை 0x20a: நிலை 0x1e2c: நிலை 0x1e2e:
	    நிலை 0x1ec8: நிலை 0x1eca:
		    EMIT2('I') EMIT2(I_grave) EMIT2(I_acute)
		    EMIT2(I_circumflex) EMIT2(I_diaeresis)
		    EMIT2(0x128) EMIT2(0x12a) EMIT2(0x12c)
		    EMIT2(0x12e) EMIT2(0x130) EMIT2(0x197)
		    EMIT2(0x1cf) EMIT2(0x208) EMIT2(0x20a)
		    EMIT2(0x1e2c) EMIT2(0x1e2e) EMIT2(0x1ec8)
		    EMIT2(0x1eca)
		    திரும்பு OK;

	    நிலை 'J': நிலை 0x134: நிலை 0x248:
		    EMIT2('J') EMIT2(0x134) EMIT2(0x248)
		    திரும்பு OK;

	    நிலை 'K': நிலை 0x136: நிலை 0x198: நிலை 0x1e8: நிலை 0x1e30:
	    நிலை 0x1e32: நிலை 0x1e34: நிலை 0x2c69: நிலை 0xa740:
		    EMIT2('K') EMIT2(0x136) EMIT2(0x198) EMIT2(0x1e8)
		    EMIT2(0x1e30) EMIT2(0x1e32) EMIT2(0x1e34)
		    EMIT2(0x2c69) EMIT2(0xa740)
		    திரும்பு OK;

	    நிலை 'L': நிலை 0x139: நிலை 0x13b: நிலை 0x13d:
	    நிலை 0x13f: நிலை 0x141: நிலை 0x23d: நிலை 0x1e36:
	    நிலை 0x1e38: நிலை 0x1e3a: நிலை 0x1e3c: நிலை 0x2c60:
		    EMIT2('L') EMIT2(0x139) EMIT2(0x13b)
		    EMIT2(0x13d) EMIT2(0x13f) EMIT2(0x141)
		    EMIT2(0x23d) EMIT2(0x1e36) EMIT2(0x1e38)
		    EMIT2(0x1e3a) EMIT2(0x1e3c) EMIT2(0x2c60)
		    திரும்பு OK;

	    நிலை 'M': நிலை 0x1e3e: நிலை 0x1e40: நிலை 0x1e42:
		    EMIT2('M') EMIT2(0x1e3e) EMIT2(0x1e40)
		    EMIT2(0x1e42)
		    திரும்பு OK;

	    நிலை 'N': நிலை N_virguilla:
	    நிலை 0x143: நிலை 0x145: நிலை 0x147: நிலை 0x1f8:
	    நிலை 0x1e44: நிலை 0x1e46: நிலை 0x1e48: நிலை 0x1e4a:
	    நிலை 0xa7a4:
		    EMIT2('N') EMIT2(N_virguilla)
		    EMIT2(0x143) EMIT2(0x145) EMIT2(0x147)
		    EMIT2(0x1f8) EMIT2(0x1e44) EMIT2(0x1e46)
		    EMIT2(0x1e48) EMIT2(0x1e4a) EMIT2(0xa7a4)
		    திரும்பு OK;

	    நிலை 'O': நிலை O_grave: நிலை O_acute: நிலை O_circumflex:
	    நிலை O_virguilla: நிலை O_diaeresis: நிலை O_slash:
	    நிலை 0x14c: நிலை 0x14e: நிலை 0x150: நிலை 0x19f:
	    நிலை 0x1a0: நிலை 0x1d1: நிலை 0x1ea: நிலை 0x1ec:
	    நிலை 0x1fe: நிலை 0x20c: நிலை 0x20e: நிலை 0x22a:
	    நிலை 0x22c: நிலை 0x22e: நிலை 0x230: நிலை 0x1e4c:
	    நிலை 0x1e4e: நிலை 0x1e50: நிலை 0x1e52: நிலை 0x1ecc:
	    நிலை 0x1ece: நிலை 0x1ed0: நிலை 0x1ed2: நிலை 0x1ed4:
	    நிலை 0x1ed6: நிலை 0x1ed8: நிலை 0x1eda: நிலை 0x1edc:
	    நிலை 0x1ede: நிலை 0x1ee0: நிலை 0x1ee2:
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
		    திரும்பு OK;

	    நிலை 'P': நிலை 0x1a4: நிலை 0x1e54: நிலை 0x1e56: நிலை 0x2c63:
		    EMIT2('P') EMIT2(0x1a4) EMIT2(0x1e54) EMIT2(0x1e56)
		    EMIT2(0x2c63)
		    திரும்பு OK;

	    நிலை 'Q': நிலை 0x24a:
		    EMIT2('Q') EMIT2(0x24a)
		    திரும்பு OK;

	    நிலை 'R': நிலை 0x154: நிலை 0x156: நிலை 0x158: நிலை 0x210:
	    நிலை 0x212: நிலை 0x24c: நிலை 0x1e58: நிலை 0x1e5a:
	    நிலை 0x1e5c: நிலை 0x1e5e: நிலை 0x2c64: நிலை 0xa7a6:
		    EMIT2('R') EMIT2(0x154) EMIT2(0x156) EMIT2(0x158)
		    EMIT2(0x210) EMIT2(0x212) EMIT2(0x24c) EMIT2(0x1e58)
		    EMIT2(0x1e5a) EMIT2(0x1e5c) EMIT2(0x1e5e) EMIT2(0x2c64)
		    EMIT2(0xa7a6)
		    திரும்பு OK;

	    நிலை 'S': நிலை 0x15a: நிலை 0x15c: நிலை 0x15e: நிலை 0x160:
	    நிலை 0x218: நிலை 0x1e60: நிலை 0x1e62: நிலை 0x1e64:
	    நிலை 0x1e66: நிலை 0x1e68: நிலை 0x2c7e: நிலை 0xa7a8:
		    EMIT2('S') EMIT2(0x15a) EMIT2(0x15c) EMIT2(0x15e)
		    EMIT2(0x160) EMIT2(0x218) EMIT2(0x1e60) EMIT2(0x1e62)
		    EMIT2(0x1e64) EMIT2(0x1e66) EMIT2(0x1e68) EMIT2(0x2c7e)
		    EMIT2(0xa7a8)
		    திரும்பு OK;

	    நிலை 'T': நிலை 0x162: நிலை 0x164: நிலை 0x166: நிலை 0x1ac:
	    நிலை 0x1ae: நிலை 0x21a: நிலை 0x23e: நிலை 0x1e6a: நிலை 0x1e6c:
	    நிலை 0x1e6e: நிலை 0x1e70:
		    EMIT2('T') EMIT2(0x162) EMIT2(0x164) EMIT2(0x166)
		    EMIT2(0x1ac) EMIT2(0x1ae) EMIT2(0x23e) EMIT2(0x21a)
		    EMIT2(0x1e6a) EMIT2(0x1e6c) EMIT2(0x1e6e) EMIT2(0x1e70)
		    திரும்பு OK;

	    நிலை 'U': நிலை U_grave: நிலை U_acute: நிலை U_diaeresis:
	    நிலை U_circumflex: நிலை 0x168: நிலை 0x16a: நிலை 0x16c:
	    நிலை 0x16e: நிலை 0x170: நிலை 0x172: நிலை 0x1af:
	    நிலை 0x1d3: நிலை 0x1d5: நிலை 0x1d7: நிலை 0x1d9:
	    நிலை 0x1db: நிலை 0x214: நிலை 0x216: நிலை 0x244:
	    நிலை 0x1e72: நிலை 0x1e74: நிலை 0x1e76: நிலை 0x1e78:
	    நிலை 0x1e7a: நிலை 0x1ee4: நிலை 0x1ee6: நிலை 0x1ee8:
	    நிலை 0x1eea: நிலை 0x1eec: நிலை 0x1eee: நிலை 0x1ef0:
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
		    திரும்பு OK;

	    நிலை 'V': நிலை 0x1b2: நிலை 0x1e7c: நிலை 0x1e7e:
		    EMIT2('V') EMIT2(0x1b2) EMIT2(0x1e7c) EMIT2(0x1e7e)
		    திரும்பு OK;

	    நிலை 'W': நிலை 0x174: நிலை 0x1e80: நிலை 0x1e82: நிலை 0x1e84:
	    நிலை 0x1e86: நிலை 0x1e88:
		    EMIT2('W') EMIT2(0x174) EMIT2(0x1e80) EMIT2(0x1e82)
		    EMIT2(0x1e84) EMIT2(0x1e86) EMIT2(0x1e88)
		    திரும்பு OK;

	    நிலை 'X': நிலை 0x1e8a: நிலை 0x1e8c:
		    EMIT2('X') EMIT2(0x1e8a) EMIT2(0x1e8c)
		    திரும்பு OK;

	    நிலை 'Y': நிலை Y_acute: நிலை 0x176: நிலை 0x178:
	    நிலை 0x1b3: நிலை 0x232: நிலை 0x24e: நிலை 0x1e8e:
	    நிலை 0x1ef2: நிலை 0x1ef4: நிலை 0x1ef6: நிலை 0x1ef8:
		    EMIT2('Y') EMIT2(Y_acute)
		    EMIT2(0x176) EMIT2(0x178) EMIT2(0x1b3)
		    EMIT2(0x232) EMIT2(0x24e) EMIT2(0x1e8e)
		    EMIT2(0x1ef2) EMIT2(0x1ef4) EMIT2(0x1ef6)
		    EMIT2(0x1ef8)
		    திரும்பு OK;

	    நிலை 'Z': நிலை 0x179: நிலை 0x17b: நிலை 0x17d:
	    நிலை 0x1b5: நிலை 0x1e90: நிலை 0x1e92: நிலை 0x1e94:
	    நிலை 0x2c6b:
		    EMIT2('Z') EMIT2(0x179) EMIT2(0x17b) EMIT2(0x17d)
		    EMIT2(0x1b5) EMIT2(0x1e90) EMIT2(0x1e92)
		    EMIT2(0x1e94) EMIT2(0x2c6b)
		    திரும்பு OK;

	    நிலை 'a': நிலை a_grave: நிலை a_acute: நிலை a_circumflex:
	    நிலை a_virguilla: நிலை a_diaeresis: நிலை a_ring:
	    நிலை 0x101: நிலை 0x103: நிலை 0x105: நிலை 0x1ce:
	    நிலை 0x1df: நிலை 0x1e1: நிலை 0x1fb: நிலை 0x201:
	    நிலை 0x203: நிலை 0x227: நிலை 0x1d8f: நிலை 0x1e01:
	    நிலை 0x1e9a: நிலை 0x1ea1: நிலை 0x1ea3: நிலை 0x1ea5:
	    நிலை 0x1ea7: நிலை 0x1ea9: நிலை 0x1eab: நிலை 0x1ead:
	    நிலை 0x1eaf: நிலை 0x1eb1: நிலை 0x1eb3: நிலை 0x1eb5:
	    நிலை 0x1eb7: நிலை 0x2c65:
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
		    திரும்பு OK;

	    நிலை 'b': நிலை 0x180: நிலை 0x253: நிலை 0x1d6c: நிலை 0x1d80:
	    நிலை 0x1e03: நிலை 0x1e05: நிலை 0x1e07:
		    EMIT2('b') EMIT2(0x180) EMIT2(0x253) EMIT2(0x1d6c)
		    EMIT2(0x1d80) EMIT2(0x1e03) EMIT2(0x1e05) EMIT2(0x1e07)
		    திரும்பு OK;

	    நிலை 'c': நிலை c_cedilla: நிலை 0x107: நிலை 0x109: நிலை 0x10b:
	    நிலை 0x10d: நிலை 0x188: நிலை 0x23c: நிலை 0x1e09: நிலை 0xa793:
	    நிலை 0xa794:
		    EMIT2('c') EMIT2(c_cedilla)
		    EMIT2(0x107) EMIT2(0x109) EMIT2(0x10b)
		    EMIT2(0x10d) EMIT2(0x188) EMIT2(0x23c)
		    EMIT2(0x1e09) EMIT2(0xa793) EMIT2(0xa794)
		    திரும்பு OK;

	    நிலை 'd': நிலை 0x10f: நிலை 0x111: நிலை 0x257: நிலை 0x1d6d:
	    நிலை 0x1d81: நிலை 0x1d91: நிலை 0x1e0b: நிலை 0x1e0d: நிலை 0x1e0f:
	    நிலை 0x1e11: நிலை 0x1e13:
		    EMIT2('d') EMIT2(0x10f) EMIT2(0x111)
		    EMIT2(0x257) EMIT2(0x1d6d) EMIT2(0x1d81)
		    EMIT2(0x1d91) EMIT2(0x1e0b) EMIT2(0x1e0d)
		    EMIT2(0x1e0f) EMIT2(0x1e11) EMIT2(0x1e13)
		    திரும்பு OK;

	    நிலை 'e': நிலை e_grave: நிலை e_acute: நிலை e_circumflex:
	    நிலை e_diaeresis: நிலை 0x113: நிலை 0x115: நிலை 0x117:
	    நிலை 0x119: நிலை 0x11b: நிலை 0x205: நிலை 0x207:
	    நிலை 0x229: நிலை 0x247: நிலை 0x1d92: நிலை 0x1e15:
	    நிலை 0x1e17: நிலை 0x1e19: நிலை 0x1e1b: நிலை 0x1e1d:
	    நிலை 0x1eb9: நிலை 0x1ebb: நிலை 0x1ebd: நிலை 0x1ebf:
	    நிலை 0x1ec1: நிலை 0x1ec3: நிலை 0x1ec5: நிலை 0x1ec7:
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
		    திரும்பு OK;

	    நிலை 'f': நிலை 0x192: நிலை 0x1d6e: நிலை 0x1d82:
	    நிலை 0x1e1f: நிலை 0xa799:
		    EMIT2('f') EMIT2(0x192) EMIT2(0x1d6e) EMIT2(0x1d82)
		    EMIT2(0x1e1f) EMIT2(0xa799)
		    திரும்பு OK;

	    நிலை 'g': நிலை 0x11d: நிலை 0x11f: நிலை 0x121: நிலை 0x123:
	    நிலை 0x1e5: நிலை 0x1e7: நிலை 0x1f5: நிலை 0x260: நிலை 0x1d83:
	    நிலை 0x1e21: நிலை 0xa7a1:
		    EMIT2('g') EMIT2(0x11d) EMIT2(0x11f) EMIT2(0x121)
		    EMIT2(0x123) EMIT2(0x1e5) EMIT2(0x1e7)
		    EMIT2(0x1f5) EMIT2(0x260) EMIT2(0x1d83)
		    EMIT2(0x1e21) EMIT2(0xa7a1)
		    திரும்பு OK;

	    நிலை 'h': நிலை 0x125: நிலை 0x127: நிலை 0x21f: நிலை 0x1e23:
	    நிலை 0x1e25: நிலை 0x1e27: நிலை 0x1e29: நிலை 0x1e2b:
	    நிலை 0x1e96: நிலை 0x2c68: நிலை 0xa795:
		    EMIT2('h') EMIT2(0x125) EMIT2(0x127) EMIT2(0x21f)
		    EMIT2(0x1e23) EMIT2(0x1e25) EMIT2(0x1e27)
		    EMIT2(0x1e29) EMIT2(0x1e2b) EMIT2(0x1e96)
		    EMIT2(0x2c68) EMIT2(0xa795)
		    திரும்பு OK;

	    நிலை 'i': நிலை i_grave: நிலை i_acute: நிலை i_circumflex:
	    நிலை i_diaeresis: நிலை 0x129: நிலை 0x12b: நிலை 0x12d:
	    நிலை 0x12f: நிலை 0x1d0: நிலை 0x209: நிலை 0x20b:
	    நிலை 0x268: நிலை 0x1d96: நிலை 0x1e2d: நிலை 0x1e2f:
	    நிலை 0x1ec9: நிலை 0x1ecb:
		    EMIT2('i') EMIT2(i_grave) EMIT2(i_acute)
		    EMIT2(i_circumflex) EMIT2(i_diaeresis)
		    EMIT2(0x129) EMIT2(0x12b) EMIT2(0x12d)
		    EMIT2(0x12f) EMIT2(0x1d0) EMIT2(0x209)
		    EMIT2(0x20b) EMIT2(0x268) EMIT2(0x1d96)
		    EMIT2(0x1e2d) EMIT2(0x1e2f) EMIT2(0x1ec9)
		    EMIT2(0x1ecb)
		    திரும்பு OK;

	    நிலை 'j': நிலை 0x135: நிலை 0x1f0: நிலை 0x249:
		    EMIT2('j') EMIT2(0x135) EMIT2(0x1f0) EMIT2(0x249)
		    திரும்பு OK;

	    நிலை 'k': நிலை 0x137: நிலை 0x199: நிலை 0x1e9: நிலை 0x1d84:
	    நிலை 0x1e31: நிலை 0x1e33: நிலை 0x1e35: நிலை 0x2c6a: நிலை 0xa741:
		    EMIT2('k') EMIT2(0x137) EMIT2(0x199) EMIT2(0x1e9)
		    EMIT2(0x1d84) EMIT2(0x1e31) EMIT2(0x1e33)
		    EMIT2(0x1e35) EMIT2(0x2c6a) EMIT2(0xa741)
		    திரும்பு OK;

	    நிலை 'l': நிலை 0x13a: நிலை 0x13c: நிலை 0x13e: நிலை 0x140:
	    நிலை 0x142: நிலை 0x19a: நிலை 0x1e37: நிலை 0x1e39: நிலை 0x1e3b:
	    நிலை 0x1e3d: நிலை 0x2c61:
		    EMIT2('l') EMIT2(0x13a) EMIT2(0x13c)
		    EMIT2(0x13e) EMIT2(0x140) EMIT2(0x142)
		    EMIT2(0x19a) EMIT2(0x1e37) EMIT2(0x1e39)
		    EMIT2(0x1e3b) EMIT2(0x1e3d) EMIT2(0x2c61)
		    திரும்பு OK;

	    நிலை 'm': நிலை 0x1d6f: நிலை 0x1e3f: நிலை 0x1e41: நிலை 0x1e43:
		    EMIT2('m') EMIT2(0x1d6f) EMIT2(0x1e3f)
		    EMIT2(0x1e41) EMIT2(0x1e43)
		    திரும்பு OK;

	    நிலை 'n': நிலை n_virguilla: நிலை 0x144: நிலை 0x146: நிலை 0x148:
	    நிலை 0x149: நிலை 0x1f9: நிலை 0x1d70: நிலை 0x1d87: நிலை 0x1e45:
	    நிலை 0x1e47: நிலை 0x1e49: நிலை 0x1e4b: நிலை 0xa7a5:
		    EMIT2('n') EMIT2(n_virguilla)
		    EMIT2(0x144) EMIT2(0x146) EMIT2(0x148)
		    EMIT2(0x149) EMIT2(0x1f9) EMIT2(0x1d70)
		    EMIT2(0x1d87) EMIT2(0x1e45) EMIT2(0x1e47)
		    EMIT2(0x1e49) EMIT2(0x1e4b) EMIT2(0xa7a5)
		    திரும்பு OK;

	    நிலை 'o': நிலை o_grave: நிலை o_acute: நிலை o_circumflex:
	    நிலை o_virguilla: நிலை o_diaeresis: நிலை o_slash:
	    நிலை 0x14d: நிலை 0x14f: நிலை 0x151: நிலை 0x1a1:
	    நிலை 0x1d2: நிலை 0x1eb: நிலை 0x1ed: நிலை 0x1ff:
	    நிலை 0x20d: நிலை 0x20f: நிலை 0x22b: நிலை 0x22d:
	    நிலை 0x22f: நிலை 0x231: நிலை 0x275: நிலை 0x1e4d:
	    நிலை 0x1e4f: நிலை 0x1e51: நிலை 0x1e53: நிலை 0x1ecd:
	    நிலை 0x1ecf: நிலை 0x1ed1: நிலை 0x1ed3: நிலை 0x1ed5:
	    நிலை 0x1ed7: நிலை 0x1ed9: நிலை 0x1edb: நிலை 0x1edd:
	    நிலை 0x1edf: நிலை 0x1ee1: நிலை 0x1ee3:
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
		    திரும்பு OK;

	    நிலை 'p': நிலை 0x1a5: நிலை 0x1d71: நிலை 0x1d7d: நிலை 0x1d88:
	    நிலை 0x1e55: நிலை 0x1e57:
		    EMIT2('p') EMIT2(0x1a5) EMIT2(0x1d71) EMIT2(0x1d7d)
		    EMIT2(0x1d88) EMIT2(0x1e55) EMIT2(0x1e57)
		    திரும்பு OK;

	    நிலை 'q': நிலை 0x24b: நிலை 0x2a0:
		    EMIT2('q') EMIT2(0x24b) EMIT2(0x2a0)
		    திரும்பு OK;

	    நிலை 'r': நிலை 0x155: நிலை 0x157: நிலை 0x159: நிலை 0x211:
	    நிலை 0x213: நிலை 0x24d: நிலை 0x27d: நிலை 0x1d72: நிலை 0x1d73:
	    நிலை 0x1d89: நிலை 0x1e59: நிலை 0x1e5b: நிலை 0x1e5d: நிலை 0x1e5f:
	    நிலை 0xa7a7:
		    EMIT2('r') EMIT2(0x155) EMIT2(0x157) EMIT2(0x159)
		    EMIT2(0x211) EMIT2(0x213) EMIT2(0x24d) EMIT2(0x27d)
		    EMIT2(0x1d72) EMIT2(0x1d73) EMIT2(0x1d89) EMIT2(0x1e59)
		    EMIT2(0x1e5b) EMIT2(0x1e5d) EMIT2(0x1e5f) EMIT2(0xa7a7)
		    திரும்பு OK;

	    நிலை 's': நிலை 0x15b: நிலை 0x15d: நிலை 0x15f: நிலை 0x161:
	    நிலை 0x219: நிலை 0x23f: நிலை 0x1d74: நிலை 0x1d8a: நிலை 0x1e61:
	    நிலை 0x1e63: நிலை 0x1e65: நிலை 0x1e67: நிலை 0x1e69: நிலை 0xa7a9:
		    EMIT2('s') EMIT2(0x15b) EMIT2(0x15d) EMIT2(0x15f)
		    EMIT2(0x161) EMIT2(0x219) EMIT2(0x23f) EMIT2(0x1d74)
		    EMIT2(0x1d8a) EMIT2(0x1e61) EMIT2(0x1e63) EMIT2(0x1e65)
		    EMIT2(0x1e67) EMIT2(0x1e69) EMIT2(0xa7a9)
		    திரும்பு OK;

	    நிலை 't': நிலை 0x163: நிலை 0x165: நிலை 0x167: நிலை 0x1ab:
	    நிலை 0x1ad: நிலை 0x21b: நிலை 0x288: நிலை 0x1d75: நிலை 0x1e6b:
	    நிலை 0x1e6d: நிலை 0x1e6f: நிலை 0x1e71: நிலை 0x1e97: நிலை 0x2c66:
		    EMIT2('t') EMIT2(0x163) EMIT2(0x165) EMIT2(0x167)
		    EMIT2(0x1ab) EMIT2(0x1ad) EMIT2(0x21b) EMIT2(0x288)
		    EMIT2(0x1d75) EMIT2(0x1e6b) EMIT2(0x1e6d) EMIT2(0x1e6f)
		    EMIT2(0x1e71) EMIT2(0x1e97) EMIT2(0x2c66)
		    திரும்பு OK;

	    நிலை 'u': நிலை u_grave: நிலை u_acute: நிலை u_circumflex:
	    நிலை u_diaeresis: நிலை 0x169: நிலை 0x16b: நிலை 0x16d:
	    நிலை 0x16f: நிலை 0x171: நிலை 0x173: நிலை 0x1b0: நிலை 0x1d4:
	    நிலை 0x1d6: நிலை 0x1d8: நிலை 0x1da: நிலை 0x1dc: நிலை 0x215:
	    நிலை 0x217: நிலை 0x289: நிலை 0x1d7e: நிலை 0x1d99: நிலை 0x1e73:
	    நிலை 0x1e75: நிலை 0x1e77: நிலை 0x1e79: நிலை 0x1e7b:
	    நிலை 0x1ee5: நிலை 0x1ee7: நிலை 0x1ee9: நிலை 0x1eeb:
	    நிலை 0x1eed: நிலை 0x1eef: நிலை 0x1ef1:
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
		    திரும்பு OK;

	    நிலை 'v': நிலை 0x28b: நிலை 0x1d8c: நிலை 0x1e7d: நிலை 0x1e7f:
		    EMIT2('v') EMIT2(0x28b) EMIT2(0x1d8c) EMIT2(0x1e7d)
		    EMIT2(0x1e7f)
		    திரும்பு OK;

	    நிலை 'w': நிலை 0x175: நிலை 0x1e81: நிலை 0x1e83: நிலை 0x1e85:
	    நிலை 0x1e87: நிலை 0x1e89: நிலை 0x1e98:
		    EMIT2('w') EMIT2(0x175) EMIT2(0x1e81) EMIT2(0x1e83)
		    EMIT2(0x1e85) EMIT2(0x1e87) EMIT2(0x1e89) EMIT2(0x1e98)
		    திரும்பு OK;

	    நிலை 'x': நிலை 0x1e8b: நிலை 0x1e8d:
		    EMIT2('x') EMIT2(0x1e8b) EMIT2(0x1e8d)
		    திரும்பு OK;

	    நிலை 'y': நிலை y_acute: நிலை y_diaeresis: நிலை 0x177:
	    நிலை 0x1b4: நிலை 0x233: நிலை 0x24f: நிலை 0x1e8f:
	    நிலை 0x1e99: நிலை 0x1ef3: நிலை 0x1ef5: நிலை 0x1ef7:
	    நிலை 0x1ef9:
		    EMIT2('y') EMIT2(y_acute) EMIT2(y_diaeresis)
		    EMIT2(0x177) EMIT2(0x1b4) EMIT2(0x233) EMIT2(0x24f)
		    EMIT2(0x1e8f) EMIT2(0x1e99) EMIT2(0x1ef3)
		    EMIT2(0x1ef5) EMIT2(0x1ef7) EMIT2(0x1ef9)
		    திரும்பு OK;

	    நிலை 'z': நிலை 0x17a: நிலை 0x17c: நிலை 0x17e: நிலை 0x1b6:
	    நிலை 0x1d76: நிலை 0x1d8e: நிலை 0x1e91: நிலை 0x1e93:
	    நிலை 0x1e95: நிலை 0x2c6c:
		    EMIT2('z') EMIT2(0x17a) EMIT2(0x17c) EMIT2(0x17e)
		    EMIT2(0x1b6) EMIT2(0x1d76) EMIT2(0x1d8e) EMIT2(0x1e91)
		    EMIT2(0x1e93) EMIT2(0x1e95) EMIT2(0x2c6c)
		    திரும்பு OK;

	    // default: character itself
	}
    }

    EMIT2(c);
    திரும்பு OK;
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
    நிலையான முழு
nfa_regatom(வெறுமை)
{
    முழு		c;
    முழு		charclass;
    முழு		equiclass;
    முழு		collclass;
    முழு		got_coll_char;
    char_u	*p;
    char_u	*endp;
    char_u	*old_regparse = regparse;
    முழு		extra = 0;
    முழு		emit_range;
    முழு		negated;
    முழு		result;
    முழு		startc = -1;
    முழு		save_prev_at_start = prev_at_start;

    c = getchr();
    தேர்வு (c)
    {
	நிலை NUL:
	    EMSG_RET_FAIL(_(e_nfa_regexp_end_encountered_prematurely));

	நிலை Magic('^'):
	    EMIT(NFA_BOL);
	    நிறுத்து;

	நிலை Magic('$'):
	    EMIT(NFA_EOL);
#எனில் defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#endif
	    நிறுத்து;

	நிலை Magic('<'):
	    EMIT(NFA_BOW);
	    நிறுத்து;

	நிலை Magic('>'):
	    EMIT(NFA_EOW);
	    நிறுத்து;

	நிலை Magic('_'):
	    c = no_Magic(getchr());
	    எனில் (c == NUL)
		EMSG_RET_FAIL(_(e_nfa_regexp_end_encountered_prematurely));

	    எனில் (c == '^')	// "\_^" is start-of-line
	    {
		EMIT(NFA_BOL);
		நிறுத்து;
	    }
	    எனில் (c == '$')	// "\_$" is end-of-line
	    {
		EMIT(NFA_EOL);
#எனில் defined(FEAT_SYN_HL)
		had_eol = TRUE;
#endif
		நிறுத்து;
	    }

	    extra = NFA_ADD_NL;

	    // "\_[" is collection plus newline
	    எனில் (c == '[')
		செல் collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	/*
	 * Character classes.
	 */
	நிலை Magic('.'):
	நிலை Magic('i'):
	நிலை Magic('I'):
	நிலை Magic('k'):
	நிலை Magic('K'):
	நிலை Magic('f'):
	நிலை Magic('F'):
	நிலை Magic('p'):
	நிலை Magic('P'):
	நிலை Magic('s'):
	நிலை Magic('S'):
	நிலை Magic('d'):
	நிலை Magic('D'):
	நிலை Magic('x'):
	நிலை Magic('X'):
	நிலை Magic('o'):
	நிலை Magic('O'):
	நிலை Magic('w'):
	நிலை Magic('W'):
	நிலை Magic('h'):
	நிலை Magic('H'):
	நிலை Magic('a'):
	நிலை Magic('A'):
	நிலை Magic('l'):
	நிலை Magic('L'):
	நிலை Magic('u'):
	நிலை Magic('U'):
	    p = vim_strchr(classchars, no_Magic(c));
	    எனில் (p == வெற்று)
	    {
		எனில் (extra == NFA_ADD_NL)
		{
		    semsg(_(e_nfa_regexp_invalid_character_class_nr), c);
		    rc_did_emsg = TRUE;
		    திரும்பு FAIL;
		}
		siemsg("Unknown character class char: %d", c);
		திரும்பு FAIL;
	    }

	    // When '.' is followed by a composing char ignore the dot, so that
	    // the composing char is matched here.
	    எனில் (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	    {
		old_regparse = regparse;
		c = getchr();
		செல் nfa_do_multibyte;
	    }
	    EMIT(nfa_classcodes[p - classchars]);
	    எனில் (extra == NFA_ADD_NL)
	    {
		EMIT(NFA_NEWL);
		EMIT(NFA_OR);
		regflags |= RF_HASNL;
	    }
	    நிறுத்து;

	நிலை Magic('n'):
	    எனில் (reg_string)
		// In a string "\n" matches a newline character.
		EMIT(NL);
	    இல்லையெனில்
	    {
		// In buffer text "\n" matches the end of a line.
		EMIT(NFA_NEWL);
		regflags |= RF_HASNL;
	    }
	    நிறுத்து;

	நிலை Magic('('):
	    எனில் (nfa_reg(REG_PAREN) == FAIL)
		திரும்பு FAIL;	    // cascaded error
	    நிறுத்து;

	நிலை Magic('|'):
	நிலை Magic('&'):
	நிலை Magic(')'):
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    திரும்பு FAIL;

	நிலை Magic('='):
	நிலை Magic('?'):
	நிலை Magic('+'):
	நிலை Magic('@'):
	நிலை Magic('*'):
	நிலை Magic('{'):
	    // these should follow an atom, not form an atom
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    திரும்பு FAIL;

	நிலை Magic('~'):
	    {
		char_u	    *lp;

		// Previous substitute pattern.
		// Generated as "\%(pattern\)".
		எனில் (reg_prev_sub == வெற்று)
		{
		    emsg(_(e_no_previous_substitute_regular_expression));
		    திரும்பு FAIL;
		}
		க்கு (lp = reg_prev_sub; *lp != NUL; MB_CPTR_ADV(lp))
		{
		    EMIT(PTR2CHAR(lp));
		    எனில் (lp != reg_prev_sub)
			EMIT(NFA_CONCAT);
		}
		EMIT(NFA_NOPEN);
		நிறுத்து;
	    }

	நிலை Magic('1'):
	நிலை Magic('2'):
	நிலை Magic('3'):
	நிலை Magic('4'):
	நிலை Magic('5'):
	நிலை Magic('6'):
	நிலை Magic('7'):
	நிலை Magic('8'):
	நிலை Magic('9'):
	    {
		முழு refnum = no_Magic(c) - '1';

		எனில் (!seen_endbrace(refnum + 1))
		    திரும்பு FAIL;
		EMIT(NFA_BACKREF1 + refnum);
		rex.nfa_has_backref = TRUE;
	    }
	    நிறுத்து;

	நிலை Magic('z'):
	    c = no_Magic(getchr());
	    தேர்வு (c)
	    {
		நிலை 's':
		    EMIT(NFA_ZSTART);
		    எனில் (re_mult_next("\\zs") == FAIL)
			திரும்பு FAIL;
		    நிறுத்து;
		நிலை 'e':
		    EMIT(NFA_ZEND);
		    rex.nfa_has_zend = TRUE;
		    எனில் (re_mult_next("\\ze") == FAIL)
			திரும்பு FAIL;
		    நிறுத்து;
#ifdef FEAT_SYN_HL
		நிலை '1':
		நிலை '2':
		நிலை '3':
		நிலை '4':
		நிலை '5':
		நிலை '6':
		நிலை '7':
		நிலை '8':
		நிலை '9':
		    // \z1...\z9
		    எனில் ((reg_do_extmatch & REX_USE) == 0)
			EMSG_RET_FAIL(_(e_z1_z9_not_allowed_here));
		    EMIT(NFA_ZREF1 + (no_Magic(c) - '1'));
		    // No need to set rex.nfa_has_backref, the sub-matches don't
		    // change when \z1 .. \z9 matches or not.
		    re_has_z = REX_USE;
		    நிறுத்து;
		நிலை '(':
		    // \z(
		    எனில் ((reg_do_extmatch & REX_SET) == 0)
			EMSG_RET_FAIL(_(e_z_not_allowed_here));
		    எனில் (nfa_reg(REG_ZPAREN) == FAIL)
			திரும்பு FAIL;	    // cascaded error
		    re_has_z = REX_SET;
		    நிறுத்து;
#endif
		இயல்பு:
		    semsg(_(e_nfa_regexp_unknown_operator_z_chr), no_Magic(c));
		    திரும்பு FAIL;
	    }
	    நிறுத்து;

	நிலை Magic('%'):
	    c = no_Magic(getchr());
	    தேர்வு (c)
	    {
		// () without a back reference
		நிலை '(':
		    எனில் (nfa_reg(REG_NPAREN) == FAIL)
			திரும்பு FAIL;
		    EMIT(NFA_NOPEN);
		    நிறுத்து;

		நிலை 'd':   // %d123 decimal
		நிலை 'o':   // %o123 octal
		நிலை 'x':   // %xab hex 2
		நிலை 'u':   // %uabcd hex 4
		நிலை 'U':   // %U1234abcd hex 8
		    {
			vimlong_T nr;

			தேர்வு (c)
			{
			    நிலை 'd': nr = getdecchrs(); நிறுத்து;
			    நிலை 'o': nr = getoctchrs(); நிறுத்து;
			    நிலை 'x': nr = gethexchrs(2); நிறுத்து;
			    நிலை 'u': nr = gethexchrs(4); நிறுத்து;
			    நிலை 'U': nr = gethexchrs(8); நிறுத்து;
			    இயல்பு:  nr = -1; நிறுத்து;
			}

			எனில் (nr < 0 || nr > INT_MAX)
			    EMSG2_RET_FAIL(_(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			// A NUL is stored in the text as NL
			// TODO: what if a composing character follows?
			EMIT(nr == 0 ? 0x0a : (long)nr);
		    }
		    நிறுத்து;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		நிலை '^':
		    EMIT(NFA_BOF);
		    நிறுத்து;

		நிலை '$':
		    EMIT(NFA_EOF);
		    நிறுத்து;

		நிலை '#':
		    எனில் (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_start_of_pattern),
								  regparse[1]);
			திரும்பு FAIL;
		    }
		    EMIT(NFA_CURSOR);
		    நிறுத்து;

		நிலை 'V':
		    EMIT(NFA_VISUAL);
		    நிறுத்து;

		நிலை 'C':
		    EMIT(NFA_ANY_COMPOSING);
		    நிறுத்து;

		நிலை '[':
		    {
			முழு	    n;

			// \%[abc]
			க்கு (n = 0; (c = peekchr()) != ']'; ++n)
			{
			    எனில் (c == NUL)
				EMSG2_RET_FAIL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
			    // recursive call!
			    எனில் (nfa_regatom() == FAIL)
				திரும்பு FAIL;
			}
			getchr();  // get the ]
			எனில் (n == 0)
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
			நிறுத்து;
		    }

		இயல்பு:
		    {
			long_u	n = 0;
			முழு	cmp = c;
			முழு	cur = FALSE;
			முழு	got_digit = FALSE;

			எனில் (c == '<' || c == '>')
			    c = getchr();
			எனில் (no_Magic(c) == '.')
			{
			    cur = TRUE;
			    c = getchr();
			}
			போது (VIM_ISDIGIT(c))
			{
			    long_u tmp;

			    எனில் (cur)
			    {
				semsg(_(e_regexp_எண்_after_dot_pos_தேடல்_chr),
								  no_Magic(c));
				திரும்பு FAIL;
			    }
			    tmp = n * 10 + (c - '0');

			    எனில் (tmp < n)
			    {
				// overflow.
				emsg(_(e_percent_மதிப்பு_too_large));
				திரும்பு FAIL;
			    }
			    n = tmp;
			    c = getchr();
			    got_digit = TRUE;
			}
			எனில் (c == 'l' || c == 'c' || c == 'v')
			{
			    long_u limit = INT_MAX;

			    எனில் (!cur && !got_digit)
			    {
				semsg(_(e_nfa_regexp_missing_மதிப்பு_in_chr),
								  no_Magic(c));
				திரும்பு FAIL;
			    }
			    எனில் (c == 'l')
			    {
				எனில் (cur)
				    n = curwin->w_cursor.lnum;
				// \%{n}l  \%{n}<l  \%{n}>l
				EMIT(cmp == '<' ? NFA_LNUM_LT :
				     cmp == '>' ? NFA_LNUM_GT : NFA_LNUM);
				எனில் (save_prev_at_start)
				    at_start = TRUE;
			    }
			    இல்லையெனில் எனில் (c == 'c')
			    {
				எனில் (cur)
				{
				    n = curwin->w_cursor.col;
				    n++;
				}
				// \%{n}c  \%{n}<c  \%{n}>c
				EMIT(cmp == '<' ? NFA_COL_LT :
				     cmp == '>' ? NFA_COL_GT : NFA_COL);
			    }
			    இல்லையெனில்
			    {
				எனில் (cur)
				{
				    colnr_T vcol = 0;

				    getvvcol(curwin, &curwin->w_cursor,
							 வெற்று, வெற்று, &vcol, 0);
				    n = ++vcol;
				}
				// \%{n}v  \%{n}<v  \%{n}>v
				EMIT(cmp == '<' ? NFA_VCOL_LT :
				     cmp == '>' ? NFA_VCOL_GT : NFA_VCOL);
				limit = INT_MAX / MB_MAXBYTES;
			    }
			    எனில் (n >= limit)
			    {
				emsg(_(e_percent_மதிப்பு_too_large));
				திரும்பு FAIL;
			    }
			    EMIT((முழு)n);
			    நிறுத்து;
			}
			இல்லையெனில் எனில் (no_Magic(c) == '\'' && n == 0)
			{
			    // \%'m  \%<'m  \%>'m
			    EMIT(cmp == '<' ? NFA_MARK_LT :
				 cmp == '>' ? NFA_MARK_GT : NFA_MARK);
			    EMIT(getchr());
			    நிறுத்து;
			}
		    }
		    semsg(_(e_nfa_regexp_unknown_operator_percent_chr),
								  no_Magic(c));
		    திரும்பு FAIL;
	    }
	    நிறுத்து;

	நிலை Magic('['):
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
	    எனில் (*endp == ']')
	    {
		முழு plen;
		bool range_endpoint;
		/*
		 * Try to reverse engineer character classes. For example,
		 * recognize that [0-9] stands for \d and [A-Za-z_] for \h,
		 * and perform the necessary substitutions in the NFA.
		 */
		result = nfa_recognize_char_class(regparse, endp,
							 extra == NFA_ADD_NL);
		எனில் (result != FAIL)
		{
		    எனில் (result >= NFA_FIRST_NL && result <= NFA_LAST_NL)
		    {
			EMIT(result - NFA_ADD_NL);
			EMIT(NFA_NEWL);
			EMIT(NFA_OR);
		    }
		    இல்லையெனில்
			EMIT(result);
		    regparse = endp;
		    MB_PTR_ADV(regparse);
		    திரும்பு OK;
		}
		/*
		 * Failed to recognize a character class. Use the simple
		 * version that turns [abc] into 'a' OR 'b' OR 'c'
		 */
		startc = -1;
		negated = FALSE;
		எனில் (*regparse == '^')			// negated range
		{
		    negated = TRUE;
		    MB_PTR_ADV(regparse);
		    EMIT(NFA_START_NEG_COLL);
		}
		இல்லையெனில்
		    EMIT(NFA_START_COLL);
		எனில் (*regparse == '-')
		{
		    startc = '-';
		    EMIT(startc);
		    EMIT(NFA_CONCAT);
		    MB_PTR_ADV(regparse);
		}
		// Emit the OR branches for each character in the []
		emit_range = FALSE;
		போது (regparse < endp)
		{
		    முழு	    oldstartc = startc;
		    range_endpoint = false;

		    startc = -1;
		    got_coll_char = FALSE;
		    எனில் (*regparse == '[')
		    {
			// Check for [: :], [= =], [. .]
			equiclass = collclass = 0;
			charclass = get_char_class(&regparse);
			எனில் (charclass == CLASS_NONE)
			{
			    equiclass = get_equi_class(&regparse);
			    எனில் (equiclass == 0)
				collclass = get_coll_element(&regparse);
			}

			// Character class like [:alpha:]
			எனில் (charclass != CLASS_NONE)
			{
			    தேர்வு (charclass)
			    {
				நிலை CLASS_ALNUM:
				    EMIT(NFA_CLASS_ALNUM);
				    நிறுத்து;
				நிலை CLASS_ALPHA:
				    EMIT(NFA_CLASS_ALPHA);
				    நிறுத்து;
				நிலை CLASS_BLANK:
				    EMIT(NFA_CLASS_BLANK);
				    நிறுத்து;
				நிலை CLASS_CNTRL:
				    EMIT(NFA_CLASS_CNTRL);
				    நிறுத்து;
				நிலை CLASS_DIGIT:
				    EMIT(NFA_CLASS_DIGIT);
				    நிறுத்து;
				நிலை CLASS_GRAPH:
				    EMIT(NFA_CLASS_GRAPH);
				    நிறுத்து;
				நிலை CLASS_LOWER:
				    wants_nfa = TRUE;
				    EMIT(NFA_CLASS_LOWER);
				    நிறுத்து;
				நிலை CLASS_PRINT:
				    EMIT(NFA_CLASS_PRINT);
				    நிறுத்து;
				நிலை CLASS_PUNCT:
				    EMIT(NFA_CLASS_PUNCT);
				    நிறுத்து;
				நிலை CLASS_SPACE:
				    EMIT(NFA_CLASS_SPACE);
				    நிறுத்து;
				நிலை CLASS_UPPER:
				    wants_nfa = TRUE;
				    EMIT(NFA_CLASS_UPPER);
				    நிறுத்து;
				நிலை CLASS_XDIGIT:
				    EMIT(NFA_CLASS_XDIGIT);
				    நிறுத்து;
				நிலை CLASS_TAB:
				    EMIT(NFA_CLASS_TAB);
				    நிறுத்து;
				நிலை CLASS_RETURN:
				    EMIT(NFA_CLASS_RETURN);
				    நிறுத்து;
				நிலை CLASS_BACKSPACE:
				    EMIT(NFA_CLASS_BACKSPACE);
				    நிறுத்து;
				நிலை CLASS_ESCAPE:
				    EMIT(NFA_CLASS_ESCAPE);
				    நிறுத்து;
				நிலை CLASS_IDENT:
				    EMIT(NFA_CLASS_IDENT);
				    நிறுத்து;
				நிலை CLASS_KEYWORD:
				    EMIT(NFA_CLASS_KEYWORD);
				    நிறுத்து;
				நிலை CLASS_FNAME:
				    EMIT(NFA_CLASS_FNAME);
				    நிறுத்து;
			    }
			    EMIT(NFA_CONCAT);
			    தொடர்;
			}
			// Try equivalence class [=a=] and the like
			எனில் (equiclass != 0)
			{
			    result = nfa_emit_equi_class(equiclass);
			    எனில் (result == FAIL)
			    {
				// should never happen
				EMSG_RET_FAIL(_(e_error_building_nfa_with_equivalence_class));
			    }
			    தொடர்;
			}
			// Try collating class like [. .]
			எனில் (collclass != 0)
			{
			    startc = collclass;	 // allow [.a.]-x as a range
			    // Will emit the proper atom at the end of the
			    // while loop.
			}
		    }
		    // Try a range like 'a-x' or '\t-z'. Also allows '-' as a
		    // start character.
		    எனில் (*regparse == '-' && oldstartc != -1)
		    {
			emit_range = TRUE;
			startc = oldstartc;
			MB_PTR_ADV(regparse);
			தொடர்;	    // reading the end of the range
		    }

		    // Now handle simple and escaped characters.
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    எனில் (*regparse == '\\'
			    && !reg_cpo_bsl
			    && regparse + 1 <= endp
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != வெற்று
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR, regparse[1])
								      != வெற்று)
			    )
			)
		    {
			MB_PTR_ADV(regparse);

			எனில் (*regparse == 'n')
			    startc = (reg_string || emit_range
					|| regparse[1] == '-') ? NL : NFA_NEWL;
			இல்லையெனில் எனில் (*regparse == 'd'
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
				எனில் (startc == INT_MAX)
				    EMSG_RET_FAIL(_(e_unicode_val_too_large));
				got_coll_char = TRUE;
				MB_PTR_BACK(old_regparse, regparse);
			    }
			    இல்லையெனில்
			    {
				// \r,\t,\e,\b
				startc = backslash_trans(*regparse);
			    }
		    }

		    // Normal printable char
		    எனில் (startc == -1)
			startc = PTR2CHAR(regparse);

		    // Previous char was '-', so this char is end of range.
		    எனில் (emit_range)
		    {
			முழு	endc = startc;
			range_endpoint = true;

			startc = oldstartc;
			எனில் (startc > endc)
			    EMSG_RET_FAIL(_(e_reverse_range_in_character_class));

			எனில் (endc > startc + 2)
			{
			    // Emit a range instead of the sequence of
			    // individual characters.
			    எனில் (startc == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    இல்லையெனில்
				--post_ptr; // remove NFA_CONCAT
			    EMIT(endc);
			    EMIT(NFA_RANGE);
			    EMIT(NFA_CONCAT);
			}
			இல்லையெனில் எனில் (has_mbyte && ((*mb_char2len)(startc) > 1
				    || (*mb_char2len)(endc) > 1))
			{
			    // Emit the characters in the range.
			    // "startc" was already emitted, so skip it.
			    //
			    க்கு (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			இல்லையெனில்
			{
			    // Emit the range. "startc" was already emitted, so
			    // skip it.
			    க்கு (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			emit_range = FALSE;
			startc = -1;
		    }
		    இல்லையெனில்
		    {
			// This char (startc) is not part of a range. Just
			// emit it.
			// Normally, simply emit startc. But if we get char
			// code=0 from a collating char, then replace it with
			// 0x0a.
			// This is needed to completely mimic the behaviour of
			// the backtracking engine.
			எனில் (startc == NFA_NEWL)
			{
			    // Line break can't be matched as part of the
			    // collection, add an OR below. But not for negated
			    // range.
			    எனில் (!negated)
				extra = NFA_ADD_NL;
			}
			இல்லையெனில்
			{
			    எனில் (got_coll_char == TRUE && startc == 0)
			    {
				EMIT(0x0a);
				EMIT(NFA_CONCAT);
			    }
			    இல்லையெனில்
			    {
				EMIT(startc);
				எனில் (!(enc_utf8 && (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse)))))
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
		    எனில் (!range_endpoint && enc_utf8 &&
			    (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse))))
		    {
			முழு i = utf_ptr2len(regparse);

			c = utf_ptr2char(regparse + i);

			// Add composing characters
			க்கு (;;)
			{
			    எனில் (c == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    இல்லையெனில்
				EMIT(c);
			    EMIT(NFA_CONCAT);
			    எனில் ((i += utf_char2len(c)) >= plen)
				நிறுத்து;
			    c = utf_ptr2char(regparse + i);
			}
			EMIT(NFA_COMPOSING);
			EMIT(NFA_CONCAT);
		    }
		    MB_PTR_ADV(regparse);
		} // while (p < endp)

		MB_PTR_BACK(old_regparse, regparse);
		எனில் (*regparse == '-')	    // if last, '-' is just a char
		{
		    EMIT('-');
		    EMIT(NFA_CONCAT);
		}

		// skip the trailing ]
		regparse = endp;
		MB_PTR_ADV(regparse);

		// Mark end of the collection.
		எனில் (negated == TRUE)
		    EMIT(NFA_END_NEG_COLL);
		இல்லையெனில்
		    EMIT(NFA_END_COLL);

		// \_[] also matches \n but it's not negated
		எனில் (extra == NFA_ADD_NL)
		{
		    EMIT(reg_string ? NL : NFA_NEWL);
		    EMIT(NFA_OR);
		}

		திரும்பு OK;
	    } // if exists closing ]

	    எனில் (reg_strict)
		EMSG_RET_FAIL(_(e_missing_rsb_after_str_lsb));
	    // FALLTHROUGH

	இயல்பு:
	    {
		முழு	plen;

nfa_do_multibyte:
		// plen is length of current char with composing chars
		எனில் (enc_utf8 && ((*mb_char2len)(c)
			    != (plen = utfc_ptr2len(old_regparse))
						       || utf_iscomposing(c)))
		{
		    முழு i = 0;

		    // A base character plus composing characters, or just one
		    // or more composing characters.
		    // This requires creating a separate atom as if enclosing
		    // the characters in (), where NFA_COMPOSING is the ( and
		    // NFA_END_COMPOSING is the ). Note that right now we are
		    // building the postfix form, not the NFA itself;
		    // a composing char could be: a, b, c, NFA_COMPOSING
		    // where 'b' and 'c' are chars with codes > 256.
		    க்கு (;;)
		    {
			EMIT(c);
			எனில் (i > 0)
			    EMIT(NFA_CONCAT);
			எனில் ((i += utf_char2len(c)) >= plen)
			    நிறுத்து;
			c = utf_ptr2char(old_regparse + i);
		    }
		    EMIT(NFA_COMPOSING);
		    regparse = old_regparse + plen;
		}
		இல்லையெனில்
		{
		    c = no_Magic(c);
		    EMIT(c);
		}
		திரும்பு OK;
	    }
    }

    திரும்பு OK;
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
    நிலையான முழு
nfa_regpiece(வெறுமை)
{
    முழு		i;
    முழு		op;
    முழு		ret;
    long	minval, maxval;
    முழு		greedy = TRUE;      // Braces are prefixed with '-' ?
    parse_state_T old_state;
    parse_state_T new_state;
    long	c2;
    முழு		old_post_pos;
    முழு		my_post_start;
    முழு		quest;

    // Save the current parse state, so that we can use it if <atom>{m,n} is
    // next.
    save_parse_state(&old_state);

    // store current pos in the postfix form, for \{m,n} involving 0s
    my_post_start = (முழு)(post_ptr - post_start);

    ret = nfa_regatom();
    எனில் (ret == FAIL)
	திரும்பு FAIL;	    // cascaded error

    op = peekchr();
    எனில் (re_multi_type(op) == NOT_MULTI)
	திரும்பு OK;

    skipchr();
    தேர்வு (op)
    {
	நிலை Magic('*'):
	    EMIT(NFA_STAR);
	    நிறுத்து;

	நிலை Magic('+'):
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
	    எனில் (nfa_regatom() == FAIL)
		திரும்பு FAIL;
	    EMIT(NFA_STAR);
	    EMIT(NFA_CONCAT);
	    skipchr();		// skip the \+
	    நிறுத்து;

	நிலை Magic('@'):
	    c2 = (long)getdecchrs();
	    op = no_Magic(getchr());
	    i = 0;
	    தேர்வு(op)
	    {
		நிலை '=':
		    // \@=
		    i = NFA_PREV_ATOM_NO_WIDTH;
		    நிறுத்து;
		நிலை '!':
		    // \@!
		    i = NFA_PREV_ATOM_NO_WIDTH_NEG;
		    நிறுத்து;
		நிலை '<':
		    op = no_Magic(getchr());
		    எனில் (op == '=')
			// \@<=
			i = NFA_PREV_ATOM_JUST_BEFORE;
		    இல்லையெனில் எனில் (op == '!')
			// \@<!
			i = NFA_PREV_ATOM_JUST_BEFORE_NEG;
		    நிறுத்து;
		நிலை '>':
		    // \@>
		    i = NFA_PREV_ATOM_LIKE_PATTERN;
		    நிறுத்து;
	    }
	    எனில் (i == 0)
	    {
		semsg(_(e_nfa_regexp_unknown_operator_at_chr), op);
		திரும்பு FAIL;
	    }
	    EMIT(i);
	    எனில் (i == NFA_PREV_ATOM_JUST_BEFORE
					|| i == NFA_PREV_ATOM_JUST_BEFORE_NEG)
		EMIT(c2);
	    நிறுத்து;

	நிலை Magic('?'):
	நிலை Magic('='):
	    EMIT(NFA_QUEST);
	    நிறுத்து;

	நிலை Magic('{'):
	    // a{2,5} will expand to 'aaa?a?a?'
	    // a{-1,3} will expand to 'aa??a??', where ?? is the nongreedy
	    // version of '?'
	    // \v(ab){2,3} will expand to '(ab)(ab)(ab)?', where all the
	    // parenthesis have the same id

	    greedy = TRUE;
	    c2 = peekchr();
	    எனில் (c2 == '-' || c2 == Magic('-'))
	    {
		skipchr();
		greedy = FALSE;
	    }
	    எனில் (!read_limits(&minval, &maxval))
		EMSG_RET_FAIL(_(e_nfa_regexp_error_reading_repetition_limits));

	    //  <atom>{0,inf}, <atom>{0,} and <atom>{}  are equivalent to
	    //  <atom>*
	    எனில் (minval == 0 && maxval == MAX_LIMIT)
	    {
		எனில் (greedy)		// { { (match the braces)
		    // \{}, \{0,}
		    EMIT(NFA_STAR);
		இல்லையெனில்			// { { (match the braces)
		    // \{-}, \{-0,}
		    EMIT(NFA_STAR_NONGREEDY);
		நிறுத்து;
	    }

	    // Special case: x{0} or x{-0}
	    எனில் (maxval == 0)
	    {
		// Ignore result of previous call to nfa_regatom()
		post_ptr = post_start + my_post_start;
		// NFA_EMPTY is 0-length and works everywhere
		EMIT(NFA_EMPTY);
		திரும்பு OK;
	    }

	    // The engine is very inefficient (uses too many states) when the
	    // maximum is much larger than the minimum and when the maximum is
	    // large.  However, when maxval is MAX_LIMIT, it is okay, as this
	    // will emit NFA_STAR.
	    // Bail out if we can use the other engine, but only, when the
	    // pattern does not need the NFA engine like (e.g. [[:upper:]]\{2,\}
	    // does not work with characters > 8 bit with the BT engine)
	    எனில் ((nfa_re_flags & RE_AUTO)
				   && (maxval > 500 || maxval > minval + 200)
				   && (maxval != MAX_LIMIT && minval < 200)
				   && !wants_nfa)
		திரும்பு FAIL;

	    // Ignore previous call to nfa_regatom()
	    post_ptr = post_start + my_post_start;
	    // Save parse state after the repeated atom and the \{}
	    save_parse_state(&new_state);

	    quest = (greedy == TRUE? NFA_QUEST : NFA_QUEST_NONGREEDY);
	    க்கு (i = 0; i < maxval; i++)
	    {
		// Goto beginning of the repeated atom
		restore_parse_state(&old_state);
		old_post_pos = (முழு)(post_ptr - post_start);
		எனில் (nfa_regatom() == FAIL)
		    திரும்பு FAIL;
		// after "minval" times, atoms are optional
		எனில் (i + 1 > minval)
		{
		    எனில் (maxval == MAX_LIMIT)
		    {
			எனில் (greedy)
			    EMIT(NFA_STAR);
			இல்லையெனில்
			    EMIT(NFA_STAR_NONGREEDY);
		    }
		    இல்லையெனில்
			EMIT(quest);
		}
		எனில் (old_post_pos != my_post_start)
		    EMIT(NFA_CONCAT);
		எனில் (i + 1 > minval && maxval == MAX_LIMIT)
		    நிறுத்து;
	    }

	    // Go to just after the repeated atom and the \{}
	    restore_parse_state(&new_state);
	    curchr = -1;

	    நிறுத்து;


	இயல்பு:
	    நிறுத்து;
    }	// end switch

    எனில் (re_multi_type(peekchr()) != NOT_MULTI)
	// Can't have a multi follow a multi.
	EMSG_RET_FAIL(_(e_nfa_regexp_cant_have_multi_follow_multi));

    திரும்பு OK;
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
    நிலையான முழு
nfa_regconcat(வெறுமை)
{
    முழு		cont = TRUE;
    முழு		first = TRUE;

    போது (cont)
    {
	தேர்வு (peekchr())
	{
	    நிலை NUL:
	    நிலை Magic('|'):
	    நிலை Magic('&'):
	    நிலை Magic(')'):
		cont = FALSE;
		நிறுத்து;

	    நிலை Magic('Z'):
		regflags |= RF_ICOMBINE;
		skipchr_keepstart();
		நிறுத்து;
	    நிலை Magic('c'):
		regflags |= RF_ICASE;
		skipchr_keepstart();
		நிறுத்து;
	    நிலை Magic('C'):
		regflags |= RF_NOICASE;
		skipchr_keepstart();
		நிறுத்து;
	    நிலை Magic('v'):
		reg_magic = MAGIC_ALL;
		skipchr_keepstart();
		curchr = -1;
		நிறுத்து;
	    நிலை Magic('m'):
		reg_magic = MAGIC_ON;
		skipchr_keepstart();
		curchr = -1;
		நிறுத்து;
	    நிலை Magic('M'):
		reg_magic = MAGIC_OFF;
		skipchr_keepstart();
		curchr = -1;
		நிறுத்து;
	    நிலை Magic('V'):
		reg_magic = MAGIC_NONE;
		skipchr_keepstart();
		curchr = -1;
		நிறுத்து;

	    இயல்பு:
		எனில் (nfa_regpiece() == FAIL)
		    திரும்பு FAIL;
		எனில் (first == FALSE)
		    EMIT(NFA_CONCAT);
		இல்லையெனில்
		    first = FALSE;
		நிறுத்து;
	}
    }

    திரும்பு OK;
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
    நிலையான முழு
nfa_regbranch(வெறுமை)
{
    முழு		old_post_pos;

    old_post_pos = (முழு)(post_ptr - post_start);

    // First branch, possibly the only one
    எனில் (nfa_regconcat() == FAIL)
	திரும்பு FAIL;

    // Try next concats
    போது (peekchr() == Magic('&'))
    {
	skipchr();
	// if concat is empty do emit a node
	எனில் (old_post_pos == (முழு)(post_ptr - post_start))
	    EMIT(NFA_EMPTY);
	EMIT(NFA_NOPEN);
	EMIT(NFA_PREV_ATOM_NO_WIDTH);
	old_post_pos = (முழு)(post_ptr - post_start);
	எனில் (nfa_regconcat() == FAIL)
	    திரும்பு FAIL;
	// if concat is empty do emit a node
	எனில் (old_post_pos == (முழு)(post_ptr - post_start))
	    EMIT(NFA_EMPTY);
	EMIT(NFA_CONCAT);
    }

    // if a branch is empty, emit one node for it
    எனில் (old_post_pos == (முழு)(post_ptr - post_start))
	EMIT(NFA_EMPTY);

    திரும்பு OK;
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
    நிலையான முழு
nfa_reg(
    முழு		paren)	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
{
    முழு		parno = 0;
    முழு		status = FAIL;

    எனில் (paren == REG_PAREN)
    {
	எனில் (regnpar >= NSUBEXP) // Too many `('
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_parens));
	parno = regnpar++;
    }
#ifdef FEAT_SYN_HL
    இல்லையெனில் எனில் (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	எனில் (regnzpar >= NSUBEXP)
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_z));
	parno = regnzpar++;
    }
#endif

    எனில் (nfa_reg_parse_depth >= REG_MAX_PAREN_DEPTH)
	EMSG_RET_FAIL(_(e_command_too_complex));
    ++nfa_reg_parse_depth;

    எனில் (nfa_regbranch() == FAIL)
	செல் theend;	    // cascaded error

    போது (peekchr() == Magic('|'))
    {
	skipchr();
	எனில் (nfa_regbranch() == FAIL)
	    செல் theend;    // cascaded error
	EMIT(NFA_OR);
    }

    // Check for proper termination.
    எனில் (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
	எனில் (paren == REG_NPAREN)
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	இல்லையெனில்
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	rc_did_emsg = TRUE;
	செல் theend;
    }
    இல்லையெனில் எனில் (paren == REG_NOPAREN && peekchr() != NUL)
    {
	எனில் (peekchr() == Magic(')'))
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	இல்லையெனில்
	    emsg(_(e_nfa_regexp_proper_termination_error));
	rc_did_emsg = TRUE;
	செல் theend;
    }
    /*
     * Here we set the flag allowing back references to this set of
     * parentheses.
     */
    எனில் (paren == REG_PAREN)
    {
	had_endbrace[parno] = TRUE;     // have seen the close paren
	EMIT(NFA_MOPEN + parno);
    }
#ifdef FEAT_SYN_HL
    இல்லையெனில் எனில் (paren == REG_ZPAREN)
	EMIT(NFA_ZOPEN + parno);
#endif

    status = OK;

theend:
    --nfa_reg_parse_depth;
    திரும்பு status;
}

#ifdef DEBUG
நிலையான char_u code[50];

    நிலையான வெறுமை
nfa_set_code(முழு c)
{
    முழு	    addnl = FALSE;

    எனில் (c >= NFA_FIRST_NL && c <= NFA_LAST_NL)
    {
	addnl = TRUE;
	c -= NFA_ADD_NL;
    }

    STRCPY(code, "");
    தேர்வு (c)
    {
	நிலை NFA_MATCH:	    STRCPY(code, "NFA_MATCH "); நிறுத்து;
	நிலை NFA_SPLIT:	    STRCPY(code, "NFA_SPLIT "); நிறுத்து;
	நிலை NFA_CONCAT:    STRCPY(code, "NFA_CONCAT "); நிறுத்து;
	நிலை NFA_NEWL:	    STRCPY(code, "NFA_NEWL "); நிறுத்து;
	நிலை NFA_ZSTART:    STRCPY(code, "NFA_ZSTART"); நிறுத்து;
	நிலை NFA_ZEND:	    STRCPY(code, "NFA_ZEND"); நிறுத்து;

	நிலை NFA_BACKREF1:  STRCPY(code, "NFA_BACKREF1"); நிறுத்து;
	நிலை NFA_BACKREF2:  STRCPY(code, "NFA_BACKREF2"); நிறுத்து;
	நிலை NFA_BACKREF3:  STRCPY(code, "NFA_BACKREF3"); நிறுத்து;
	நிலை NFA_BACKREF4:  STRCPY(code, "NFA_BACKREF4"); நிறுத்து;
	நிலை NFA_BACKREF5:  STRCPY(code, "NFA_BACKREF5"); நிறுத்து;
	நிலை NFA_BACKREF6:  STRCPY(code, "NFA_BACKREF6"); நிறுத்து;
	நிலை NFA_BACKREF7:  STRCPY(code, "NFA_BACKREF7"); நிறுத்து;
	நிலை NFA_BACKREF8:  STRCPY(code, "NFA_BACKREF8"); நிறுத்து;
	நிலை NFA_BACKREF9:  STRCPY(code, "NFA_BACKREF9"); நிறுத்து;
# ifdef FEAT_SYN_HL
	நிலை NFA_ZREF1:	    STRCPY(code, "NFA_ZREF1"); நிறுத்து;
	நிலை NFA_ZREF2:	    STRCPY(code, "NFA_ZREF2"); நிறுத்து;
	நிலை NFA_ZREF3:	    STRCPY(code, "NFA_ZREF3"); நிறுத்து;
	நிலை NFA_ZREF4:	    STRCPY(code, "NFA_ZREF4"); நிறுத்து;
	நிலை NFA_ZREF5:	    STRCPY(code, "NFA_ZREF5"); நிறுத்து;
	நிலை NFA_ZREF6:	    STRCPY(code, "NFA_ZREF6"); நிறுத்து;
	நிலை NFA_ZREF7:	    STRCPY(code, "NFA_ZREF7"); நிறுத்து;
	நிலை NFA_ZREF8:	    STRCPY(code, "NFA_ZREF8"); நிறுத்து;
	நிலை NFA_ZREF9:	    STRCPY(code, "NFA_ZREF9"); நிறுத்து;
# endif
	நிலை NFA_SKIP:	    STRCPY(code, "NFA_SKIP"); நிறுத்து;

	நிலை NFA_PREV_ATOM_NO_WIDTH:
			    STRCPY(code, "NFA_PREV_ATOM_NO_WIDTH"); நிறுத்து;
	நிலை NFA_PREV_ATOM_NO_WIDTH_NEG:
			    STRCPY(code, "NFA_PREV_ATOM_NO_WIDTH_NEG"); நிறுத்து;
	நிலை NFA_PREV_ATOM_JUST_BEFORE:
			    STRCPY(code, "NFA_PREV_ATOM_JUST_BEFORE"); நிறுத்து;
	நிலை NFA_PREV_ATOM_JUST_BEFORE_NEG:
			 STRCPY(code, "NFA_PREV_ATOM_JUST_BEFORE_NEG"); நிறுத்து;
	நிலை NFA_PREV_ATOM_LIKE_PATTERN:
			    STRCPY(code, "NFA_PREV_ATOM_LIKE_PATTERN"); நிறுத்து;

	நிலை NFA_NOPEN:		    STRCPY(code, "NFA_NOPEN"); நிறுத்து;
	நிலை NFA_NCLOSE:	    STRCPY(code, "NFA_NCLOSE"); நிறுத்து;
	நிலை NFA_START_INVISIBLE:   STRCPY(code, "NFA_START_INVISIBLE"); நிறுத்து;
	நிலை NFA_START_INVISIBLE_FIRST:
			     STRCPY(code, "NFA_START_INVISIBLE_FIRST"); நிறுத்து;
	நிலை NFA_START_INVISIBLE_NEG:
			       STRCPY(code, "NFA_START_INVISIBLE_NEG"); நிறுத்து;
	நிலை NFA_START_INVISIBLE_NEG_FIRST:
			 STRCPY(code, "NFA_START_INVISIBLE_NEG_FIRST"); நிறுத்து;
	நிலை NFA_START_INVISIBLE_BEFORE:
			    STRCPY(code, "NFA_START_INVISIBLE_BEFORE"); நிறுத்து;
	நிலை NFA_START_INVISIBLE_BEFORE_FIRST:
		      STRCPY(code, "NFA_START_INVISIBLE_BEFORE_FIRST"); நிறுத்து;
	நிலை NFA_START_INVISIBLE_BEFORE_NEG:
			STRCPY(code, "NFA_START_INVISIBLE_BEFORE_NEG"); நிறுத்து;
	நிலை NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
		  STRCPY(code, "NFA_START_INVISIBLE_BEFORE_NEG_FIRST"); நிறுத்து;
	நிலை NFA_START_PATTERN:   STRCPY(code, "NFA_START_PATTERN"); நிறுத்து;
	நிலை NFA_END_INVISIBLE:	    STRCPY(code, "NFA_END_INVISIBLE"); நிறுத்து;
	நிலை NFA_END_INVISIBLE_NEG: STRCPY(code, "NFA_END_INVISIBLE_NEG"); நிறுத்து;
	நிலை NFA_END_PATTERN:	    STRCPY(code, "NFA_END_PATTERN"); நிறுத்து;

	நிலை NFA_COMPOSING:	    STRCPY(code, "NFA_COMPOSING"); நிறுத்து;
	நிலை NFA_END_COMPOSING:	    STRCPY(code, "NFA_END_COMPOSING"); நிறுத்து;
	நிலை NFA_OPT_CHARS:	    STRCPY(code, "NFA_OPT_CHARS"); நிறுத்து;

	நிலை NFA_MOPEN:
	நிலை NFA_MOPEN1:
	நிலை NFA_MOPEN2:
	நிலை NFA_MOPEN3:
	நிலை NFA_MOPEN4:
	நிலை NFA_MOPEN5:
	நிலை NFA_MOPEN6:
	நிலை NFA_MOPEN7:
	நிலை NFA_MOPEN8:
	நிலை NFA_MOPEN9:
	    STRCPY(code, "NFA_MOPEN(x)");
	    code[10] = c - NFA_MOPEN + '0';
	    நிறுத்து;
	நிலை NFA_MCLOSE:
	நிலை NFA_MCLOSE1:
	நிலை NFA_MCLOSE2:
	நிலை NFA_MCLOSE3:
	நிலை NFA_MCLOSE4:
	நிலை NFA_MCLOSE5:
	நிலை NFA_MCLOSE6:
	நிலை NFA_MCLOSE7:
	நிலை NFA_MCLOSE8:
	நிலை NFA_MCLOSE9:
	    STRCPY(code, "NFA_MCLOSE(x)");
	    code[11] = c - NFA_MCLOSE + '0';
	    நிறுத்து;
# ifdef FEAT_SYN_HL
	நிலை NFA_ZOPEN:
	நிலை NFA_ZOPEN1:
	நிலை NFA_ZOPEN2:
	நிலை NFA_ZOPEN3:
	நிலை NFA_ZOPEN4:
	நிலை NFA_ZOPEN5:
	நிலை NFA_ZOPEN6:
	நிலை NFA_ZOPEN7:
	நிலை NFA_ZOPEN8:
	நிலை NFA_ZOPEN9:
	    STRCPY(code, "NFA_ZOPEN(x)");
	    code[10] = c - NFA_ZOPEN + '0';
	    நிறுத்து;
	நிலை NFA_ZCLOSE:
	நிலை NFA_ZCLOSE1:
	நிலை NFA_ZCLOSE2:
	நிலை NFA_ZCLOSE3:
	நிலை NFA_ZCLOSE4:
	நிலை NFA_ZCLOSE5:
	நிலை NFA_ZCLOSE6:
	நிலை NFA_ZCLOSE7:
	நிலை NFA_ZCLOSE8:
	நிலை NFA_ZCLOSE9:
	    STRCPY(code, "NFA_ZCLOSE(x)");
	    code[11] = c - NFA_ZCLOSE + '0';
	    நிறுத்து;
# endif
	நிலை NFA_EOL:		STRCPY(code, "NFA_EOL "); நிறுத்து;
	நிலை NFA_BOL:		STRCPY(code, "NFA_BOL "); நிறுத்து;
	நிலை NFA_EOW:		STRCPY(code, "NFA_EOW "); நிறுத்து;
	நிலை NFA_BOW:		STRCPY(code, "NFA_BOW "); நிறுத்து;
	நிலை NFA_EOF:		STRCPY(code, "NFA_EOF "); நிறுத்து;
	நிலை NFA_BOF:		STRCPY(code, "NFA_BOF "); நிறுத்து;
	நிலை NFA_LNUM:		STRCPY(code, "NFA_LNUM "); நிறுத்து;
	நிலை NFA_LNUM_GT:	STRCPY(code, "NFA_LNUM_GT "); நிறுத்து;
	நிலை NFA_LNUM_LT:	STRCPY(code, "NFA_LNUM_LT "); நிறுத்து;
	நிலை NFA_COL:		STRCPY(code, "NFA_COL "); நிறுத்து;
	நிலை NFA_COL_GT:	STRCPY(code, "NFA_COL_GT "); நிறுத்து;
	நிலை NFA_COL_LT:	STRCPY(code, "NFA_COL_LT "); நிறுத்து;
	நிலை NFA_VCOL:		STRCPY(code, "NFA_VCOL "); நிறுத்து;
	நிலை NFA_VCOL_GT:	STRCPY(code, "NFA_VCOL_GT "); நிறுத்து;
	நிலை NFA_VCOL_LT:	STRCPY(code, "NFA_VCOL_LT "); நிறுத்து;
	நிலை NFA_MARK:		STRCPY(code, "NFA_MARK "); நிறுத்து;
	நிலை NFA_MARK_GT:	STRCPY(code, "NFA_MARK_GT "); நிறுத்து;
	நிலை NFA_MARK_LT:	STRCPY(code, "NFA_MARK_LT "); நிறுத்து;
	நிலை NFA_CURSOR:	STRCPY(code, "NFA_CURSOR "); நிறுத்து;
	நிலை NFA_VISUAL:	STRCPY(code, "NFA_VISUAL "); நிறுத்து;
	நிலை NFA_ANY_COMPOSING:	STRCPY(code, "NFA_ANY_COMPOSING "); நிறுத்து;

	நிலை NFA_STAR:		STRCPY(code, "NFA_STAR "); நிறுத்து;
	நிலை NFA_STAR_NONGREEDY: STRCPY(code, "NFA_STAR_NONGREEDY "); நிறுத்து;
	நிலை NFA_QUEST:		STRCPY(code, "NFA_QUEST"); நிறுத்து;
	நிலை NFA_QUEST_NONGREEDY: STRCPY(code, "NFA_QUEST_NON_GREEDY"); நிறுத்து;
	நிலை NFA_EMPTY:		STRCPY(code, "NFA_EMPTY"); நிறுத்து;
	நிலை NFA_OR:		STRCPY(code, "NFA_OR"); நிறுத்து;

	நிலை NFA_START_COLL:	STRCPY(code, "NFA_START_COLL"); நிறுத்து;
	நிலை NFA_END_COLL:	STRCPY(code, "NFA_END_COLL"); நிறுத்து;
	நிலை NFA_START_NEG_COLL: STRCPY(code, "NFA_START_NEG_COLL"); நிறுத்து;
	நிலை NFA_END_NEG_COLL:	STRCPY(code, "NFA_END_NEG_COLL"); நிறுத்து;
	நிலை NFA_RANGE:		STRCPY(code, "NFA_RANGE"); நிறுத்து;
	நிலை NFA_RANGE_MIN:	STRCPY(code, "NFA_RANGE_MIN"); நிறுத்து;
	நிலை NFA_RANGE_MAX:	STRCPY(code, "NFA_RANGE_MAX"); நிறுத்து;

	நிலை NFA_CLASS_ALNUM:	STRCPY(code, "NFA_CLASS_ALNUM"); நிறுத்து;
	நிலை NFA_CLASS_ALPHA:	STRCPY(code, "NFA_CLASS_ALPHA"); நிறுத்து;
	நிலை NFA_CLASS_BLANK:	STRCPY(code, "NFA_CLASS_BLANK"); நிறுத்து;
	நிலை NFA_CLASS_CNTRL:	STRCPY(code, "NFA_CLASS_CNTRL"); நிறுத்து;
	நிலை NFA_CLASS_DIGIT:	STRCPY(code, "NFA_CLASS_DIGIT"); நிறுத்து;
	நிலை NFA_CLASS_GRAPH:	STRCPY(code, "NFA_CLASS_GRAPH"); நிறுத்து;
	நிலை NFA_CLASS_LOWER:	STRCPY(code, "NFA_CLASS_LOWER"); நிறுத்து;
	நிலை NFA_CLASS_PRINT:	STRCPY(code, "NFA_CLASS_PRINT"); நிறுத்து;
	நிலை NFA_CLASS_PUNCT:	STRCPY(code, "NFA_CLASS_PUNCT"); நிறுத்து;
	நிலை NFA_CLASS_SPACE:	STRCPY(code, "NFA_CLASS_SPACE"); நிறுத்து;
	நிலை NFA_CLASS_UPPER:	STRCPY(code, "NFA_CLASS_UPPER"); நிறுத்து;
	நிலை NFA_CLASS_XDIGIT:	STRCPY(code, "NFA_CLASS_XDIGIT"); நிறுத்து;
	நிலை NFA_CLASS_TAB:	STRCPY(code, "NFA_CLASS_TAB"); நிறுத்து;
	நிலை NFA_CLASS_RETURN:	STRCPY(code, "NFA_CLASS_RETURN"); நிறுத்து;
	நிலை NFA_CLASS_BACKSPACE:   STRCPY(code, "NFA_CLASS_BACKSPACE"); நிறுத்து;
	நிலை NFA_CLASS_ESCAPE:	STRCPY(code, "NFA_CLASS_ESCAPE"); நிறுத்து;
	நிலை NFA_CLASS_IDENT:	STRCPY(code, "NFA_CLASS_IDENT"); நிறுத்து;
	நிலை NFA_CLASS_KEYWORD:	STRCPY(code, "NFA_CLASS_KEYWORD"); நிறுத்து;
	நிலை NFA_CLASS_FNAME:	STRCPY(code, "NFA_CLASS_FNAME"); நிறுத்து;

	நிலை NFA_ANY:	STRCPY(code, "NFA_ANY"); நிறுத்து;
	நிலை NFA_IDENT:	STRCPY(code, "NFA_IDENT"); நிறுத்து;
	நிலை NFA_SIDENT:STRCPY(code, "NFA_SIDENT"); நிறுத்து;
	நிலை NFA_KWORD:	STRCPY(code, "NFA_KWORD"); நிறுத்து;
	நிலை NFA_SKWORD:STRCPY(code, "NFA_SKWORD"); நிறுத்து;
	நிலை NFA_FNAME:	STRCPY(code, "NFA_FNAME"); நிறுத்து;
	நிலை NFA_SFNAME:STRCPY(code, "NFA_SFNAME"); நிறுத்து;
	நிலை NFA_PRINT:	STRCPY(code, "NFA_PRINT"); நிறுத்து;
	நிலை NFA_SPRINT:STRCPY(code, "NFA_SPRINT"); நிறுத்து;
	நிலை NFA_WHITE:	STRCPY(code, "NFA_WHITE"); நிறுத்து;
	நிலை NFA_NWHITE:STRCPY(code, "NFA_NWHITE"); நிறுத்து;
	நிலை NFA_DIGIT:	STRCPY(code, "NFA_DIGIT"); நிறுத்து;
	நிலை NFA_NDIGIT:STRCPY(code, "NFA_NDIGIT"); நிறுத்து;
	நிலை NFA_HEX:	STRCPY(code, "NFA_HEX"); நிறுத்து;
	நிலை NFA_NHEX:	STRCPY(code, "NFA_NHEX"); நிறுத்து;
	நிலை NFA_OCTAL:	STRCPY(code, "NFA_OCTAL"); நிறுத்து;
	நிலை NFA_NOCTAL:STRCPY(code, "NFA_NOCTAL"); நிறுத்து;
	நிலை NFA_WORD:	STRCPY(code, "NFA_WORD"); நிறுத்து;
	நிலை NFA_NWORD:	STRCPY(code, "NFA_NWORD"); நிறுத்து;
	நிலை NFA_HEAD:	STRCPY(code, "NFA_HEAD"); நிறுத்து;
	நிலை NFA_NHEAD:	STRCPY(code, "NFA_NHEAD"); நிறுத்து;
	நிலை NFA_ALPHA:	STRCPY(code, "NFA_ALPHA"); நிறுத்து;
	நிலை NFA_NALPHA:STRCPY(code, "NFA_NALPHA"); நிறுத்து;
	நிலை NFA_LOWER:	STRCPY(code, "NFA_LOWER"); நிறுத்து;
	நிலை NFA_NLOWER:STRCPY(code, "NFA_NLOWER"); நிறுத்து;
	நிலை NFA_UPPER:	STRCPY(code, "NFA_UPPER"); நிறுத்து;
	நிலை NFA_NUPPER:STRCPY(code, "NFA_NUPPER"); நிறுத்து;
	நிலை NFA_LOWER_IC:  STRCPY(code, "NFA_LOWER_IC"); நிறுத்து;
	நிலை NFA_NLOWER_IC: STRCPY(code, "NFA_NLOWER_IC"); நிறுத்து;
	நிலை NFA_UPPER_IC:  STRCPY(code, "NFA_UPPER_IC"); நிறுத்து;
	நிலை NFA_NUPPER_IC: STRCPY(code, "NFA_NUPPER_IC"); நிறுத்து;

	இயல்பு:
	    STRCPY(code, "CHAR(x)");
	    code[5] = c;
    }

    எனில் (addnl == TRUE)
	STRCAT(code, " + NEWLINE ");

}

# ifdef ENABLE_LOG
நிலையான FILE *log_fd;
நிலையான char_u e_log_open_failed[] = N_("Could not open temporary log file for writing, displaying on stderr... ");

/*
 * Print the postfix notation of the current regexp.
 */
    நிலையான வெறுமை
nfa_postfix_dump(char_u *கோவை, முழு retval)
{
    முழு *p;
    FILE *f;

    f = fopen(NFA_REGEXP_DUMP_LOG, "a");
    எனில் (f == வெற்று)
	திரும்பு;

    fprintf(f, "\n-------------------------\n");
    எனில் (retval == FAIL)
	fprintf(f, ">>> NFA engine failed... \n");
    இல்லையெனில் எனில் (retval == OK)
	fprintf(f, ">>> NFA engine succeeded !\n");
    fprintf(f, "Regexp: \"%s\"\nPostfix notation (char): \"", கோவை);
    க்கு (p = post_start; *p && p < post_ptr; p++)
    {
	nfa_set_code(*p);
	fprintf(f, "%s, ", code);
    }
    fprintf(f, "\"\nPostfix notation (int): ");
    க்கு (p = post_start; *p && p < post_ptr; p++)
	fprintf(f, "%d ", *p);
    fprintf(f, "\n\n");
    fclose(f);
}

/*
 * Print the NFA starting with a root node "state".
 */
    நிலையான வெறுமை
nfa_print_state(FILE *debugf, nfa_state_T *state)
{
    garray_T indent;

    ga_init2(&indent, 1, 64);
    ga_append(&indent, '\0');
    nfa_print_state2(debugf, state, &indent);
    ga_clear(&indent);
}

    நிலையான வெறுமை
nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent)
{
    char_u  *p;

    எனில் (state == வெற்று)
	திரும்பு;

    fprintf(debugf, "(%2d)", abs(state->id));

    // Output indent
    p = (char_u *)indent->ga_data;
    எனில் (indent->ga_len >= 3)
    {
	முழு	last = indent->ga_len - 3;
	char_u	save[2];

	STRNCPY(save, &p[last], 2);
	memcpy(&p[last], "+-", 2);
	fprintf(debugf, " %s", p);
	STRNCPY(&p[last], save, 2);
    }
    இல்லையெனில்
	fprintf(debugf, " %s", p);

    nfa_set_code(state->c);
    fprintf(debugf, "%s (%d) (id=%d) val=%d\n",
		 code,
		 state->c,
		 abs(state->id),
		 state->val);
    எனில் (state->id < 0)
	திரும்பு;

    state->id = abs(state->id) * -1;

    // grow indent for state->out
    indent->ga_len -= 1;
    எனில் (state->out1)
	GA_CONCAT_LITERAL(indent, "| ");
    இல்லையெனில்
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
    நிலையான வெறுமை
nfa_dump(nfa_regprog_T *prog)
{
    FILE *debugf = fopen(NFA_REGEXP_DUMP_LOG, "a");

    எனில் (debugf == வெற்று)
	திரும்பு;

    nfa_print_state(debugf, prog->start);

    எனில் (prog->reganch)
	fprintf(debugf, "reganch: %d\n", prog->reganch);
    எனில் (prog->regstart != NUL)
	fprintf(debugf, "regstart: %c (decimal: %d)\n",
		prog->regstart, prog->regstart);
    எனில் (prog->match_text != வெற்று)
	fprintf(debugf, "match_text: \"%s\"\n", prog->match_text);

    fclose(debugf);
}
# endif	    // ENABLE_LOG
#endif	    // DEBUG

/*
 * Parse r.e. @expr and convert it into postfix form.
 * Return the postfix string on success, NULL otherwise.
 */
    நிலையான முழு *
re2post(வெறுமை)
{
    எனில் (nfa_reg(REG_NOPAREN) == FAIL)
	திரும்பு வெற்று;
    EMIT(NFA_MOPEN);
    திரும்பு post_start;
}

// NB. Some of the code below is inspired by Russ's.

/*
 * Represents an NFA state plus zero or one or two arrows exiting.
 * if c == MATCH, no arrows out; matching state.
 * If c == SPLIT, unlabeled arrows to out and out1 (if != NULL).
 * If c < 256, labeled arrow with character c to out.
 */

நிலையான nfa_state_T	*state_ptr; // points to nfa_prog->state

/*
 * Allocate and initialize nfa_state_T.
 */
    நிலையான nfa_state_T *
alloc_state(முழு c, nfa_state_T *out, nfa_state_T *out1)
{
    nfa_state_T *s;

    எனில் (istate >= nstate)
	திரும்பு வெற்று;

    s = &state_ptr[istate++];

    s->c    = c;
    s->out  = out;
    s->out1 = out1;
    s->val  = 0;

    s->id   = istate;
    s->lastlist[0] = 0;
    s->lastlist[1] = 0;

    திரும்பு s;
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
வகைவரை ஒன்றிப்பு Ptrlist Ptrlist;
ஒன்றிப்பு Ptrlist
{
    Ptrlist	*next;
    nfa_state_T	*s;
};

கட்டமைப்பு Frag
{
    nfa_state_T *start;
    Ptrlist	*out;
};
வகைவரை கட்டமைப்பு Frag Frag_T;

// Reused across compilations by post2nfa(); freed in free_regexp_stuff().
நிலையான Frag_T *nfa_அடுக்கு;
நிலையான முழு nfa_அடுக்கு_len;

/*
 * Initialize a Frag_T struct and return it.
 */
    நிலையான Frag_T
frag(nfa_state_T *start, Ptrlist *out)
{
    Frag_T n;

    n.start = start;
    n.out = out;
    திரும்பு n;
}

/*
 * Create singleton list containing just outp.
 */
    நிலையான Ptrlist *
list1(
    nfa_state_T	**outp)
{
    Ptrlist *l;

    l = (Ptrlist *)outp;
    l->next = வெற்று;
    திரும்பு l;
}

/*
 * Patch the list of states at out to point to start.
 */
    நிலையான வெறுமை
patch(Ptrlist *l, nfa_state_T *s)
{
    Ptrlist *next;

    க்கு (; l; l = next)
    {
	next = l->next;
	l->s = s;
    }
}


/*
 * Join the two lists l1 and l2, returning the combination.
 */
    நிலையான Ptrlist *
append(Ptrlist *l1, Ptrlist *l2)
{
    Ptrlist *oldl1;

    oldl1 = l1;
    போது (l1->next)
	l1 = l1->next;
    l1->next = l2;
    திரும்பு oldl1;
}

/*
 * Stack used for transforming postfix form into NFA.
 */
நிலையான Frag_T empty;

    நிலையான வெறுமை
st_error(முழு *postfix UNUSED, முழு *end UNUSED, முழு *p UNUSED)
{
#ifdef NFA_REGEXP_ERROR_LOG
    FILE *df;
    முழு *p2;

    df = fopen(NFA_REGEXP_ERROR_LOG, "a");
    எனில் (df)
    {
	fprintf(df, "Error popping the stack!\n");
# ifdef DEBUG
	fprintf(df, "Current regexp is \"%s\"\n", nfa_regengine.கோவை);
# endif
	fprintf(df, "Postfix form is: ");
# ifdef DEBUG
	க்கு (p2 = postfix; p2 < end; p2++)
	{
	    nfa_set_code(*p2);
	    fprintf(df, "%s, ", code);
	}
	nfa_set_code(*p);
	fprintf(df, "\nCurrent position is: ");
	க்கு (p2 = postfix; p2 <= p; p2 ++)
	{
	    nfa_set_code(*p2);
	    fprintf(df, "%s, ", code);
	}
# இல்லையெனில்
	க்கு (p2 = postfix; p2 < end; p2++)
	    fprintf(df, "%d, ", *p2);
	fprintf(df, "\nCurrent position is: ");
	க்கு (p2 = postfix; p2 <= p; p2 ++)
	    fprintf(df, "%d, ", *p2);
# endif
	fprintf(df, "\n--------------------------\n");
	fclose(df);
    }
#endif
    emsg(_(e_nfa_regexp_could_not_எடு_அடுக்கு));
}

/*
 * Push an item onto the stack.
 */
    நிலையான வெறுமை
st_தள்ளு(Frag_T s, Frag_T **p, Frag_T *அடுக்கு_end)
{
    Frag_T *stackp = *p;

    எனில் (stackp >= அடுக்கு_end)
	திரும்பு;
    *stackp = s;
    *p = *p + 1;
}

/*
 * Pop an item from the stack.
 */
    நிலையான Frag_T
st_எடு(Frag_T **p, Frag_T *அடுக்கு)
{
    Frag_T *stackp;

    *p = *p - 1;
    stackp = *p;
    எனில் (stackp < அடுக்கு)
	திரும்பு empty;
    திரும்பு **p;
}

/*
 * Estimate the maximum byte length of anything matching "state".
 * When unknown or unlimited return -1.
 */
    நிலையான முழு
nfa_max_width(nfa_state_T *startstate, முழு depth)
{
    முழு		    l, r;
    nfa_state_T	    *state = startstate;
    முழு		    len = 0;

    // detect looping in a NFA_SPLIT
    எனில் (depth > 4)
	திரும்பு -1;

    போது (state != வெற்று)
    {
	தேர்வு (state->c)
	{
	    நிலை NFA_END_INVISIBLE:
	    நிலை NFA_END_INVISIBLE_NEG:
		// the end, return what we have
		திரும்பு len;

	    நிலை NFA_SPLIT:
		// two alternatives, use the maximum
		l = nfa_max_width(state->out, depth + 1);
		r = nfa_max_width(state->out1, depth + 1);
		எனில் (l < 0 || r < 0)
		    திரும்பு -1;
		திரும்பு len + (l > r ? l : r);

	    நிலை NFA_ANY:
	    நிலை NFA_START_COLL:
	    நிலை NFA_START_NEG_COLL:
		// matches some character, including composing chars
		எனில் (enc_utf8)
		    len += MB_MAXBYTES;
		இல்லையெனில் எனில் (has_mbyte)
		    len += 2;
		இல்லையெனில்
		    ++len;
		எனில் (state->c != NFA_ANY)
		{
		    // Skip over the compiled collection.
		    // malformed NFAs must not crash width estimation.
		    எனில் (state->out1 == வெற்று || state->out1->out == வெற்று)
			திரும்பு -1;
		    state = state->out1->out;
		    தொடர்;
		}
		நிறுத்து;

	    நிலை NFA_DIGIT:
	    நிலை NFA_WHITE:
	    நிலை NFA_HEX:
	    நிலை NFA_OCTAL:
		// ascii
		++len;
		நிறுத்து;

	    நிலை NFA_IDENT:
	    நிலை NFA_SIDENT:
	    நிலை NFA_KWORD:
	    நிலை NFA_SKWORD:
	    நிலை NFA_FNAME:
	    நிலை NFA_SFNAME:
	    நிலை NFA_PRINT:
	    நிலை NFA_SPRINT:
	    நிலை NFA_NWHITE:
	    நிலை NFA_NDIGIT:
	    நிலை NFA_NHEX:
	    நிலை NFA_NOCTAL:
	    நிலை NFA_WORD:
	    நிலை NFA_NWORD:
	    நிலை NFA_HEAD:
	    நிலை NFA_NHEAD:
	    நிலை NFA_ALPHA:
	    நிலை NFA_NALPHA:
	    நிலை NFA_LOWER:
	    நிலை NFA_NLOWER:
	    நிலை NFA_UPPER:
	    நிலை NFA_NUPPER:
	    நிலை NFA_LOWER_IC:
	    நிலை NFA_NLOWER_IC:
	    நிலை NFA_UPPER_IC:
	    நிலை NFA_NUPPER_IC:
	    நிலை NFA_ANY_COMPOSING:
		// possibly non-ascii
		எனில் (has_mbyte)
		    len += 3;
		இல்லையெனில்
		    ++len;
		நிறுத்து;

	    நிலை NFA_START_INVISIBLE:
	    நிலை NFA_START_INVISIBLE_NEG:
	    நிலை NFA_START_INVISIBLE_BEFORE:
	    நிலை NFA_START_INVISIBLE_BEFORE_NEG:
		// zero-width, out1 points to the END state
		state = state->out1->out;
		தொடர்;

	    நிலை NFA_BACKREF1:
	    நிலை NFA_BACKREF2:
	    நிலை NFA_BACKREF3:
	    நிலை NFA_BACKREF4:
	    நிலை NFA_BACKREF5:
	    நிலை NFA_BACKREF6:
	    நிலை NFA_BACKREF7:
	    நிலை NFA_BACKREF8:
	    நிலை NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	    நிலை NFA_ZREF1:
	    நிலை NFA_ZREF2:
	    நிலை NFA_ZREF3:
	    நிலை NFA_ZREF4:
	    நிலை NFA_ZREF5:
	    நிலை NFA_ZREF6:
	    நிலை NFA_ZREF7:
	    நிலை NFA_ZREF8:
	    நிலை NFA_ZREF9:
#endif
	    நிலை NFA_NEWL:
	    நிலை NFA_SKIP:
		// unknown width
		திரும்பு -1;

	    நிலை NFA_BOL:
	    நிலை NFA_EOL:
	    நிலை NFA_BOF:
	    நிலை NFA_EOF:
	    நிலை NFA_BOW:
	    நிலை NFA_EOW:
	    நிலை NFA_MOPEN:
	    நிலை NFA_MOPEN1:
	    நிலை NFA_MOPEN2:
	    நிலை NFA_MOPEN3:
	    நிலை NFA_MOPEN4:
	    நிலை NFA_MOPEN5:
	    நிலை NFA_MOPEN6:
	    நிலை NFA_MOPEN7:
	    நிலை NFA_MOPEN8:
	    நிலை NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	    நிலை NFA_ZOPEN:
	    நிலை NFA_ZOPEN1:
	    நிலை NFA_ZOPEN2:
	    நிலை NFA_ZOPEN3:
	    நிலை NFA_ZOPEN4:
	    நிலை NFA_ZOPEN5:
	    நிலை NFA_ZOPEN6:
	    நிலை NFA_ZOPEN7:
	    நிலை NFA_ZOPEN8:
	    நிலை NFA_ZOPEN9:
	    நிலை NFA_ZCLOSE:
	    நிலை NFA_ZCLOSE1:
	    நிலை NFA_ZCLOSE2:
	    நிலை NFA_ZCLOSE3:
	    நிலை NFA_ZCLOSE4:
	    நிலை NFA_ZCLOSE5:
	    நிலை NFA_ZCLOSE6:
	    நிலை NFA_ZCLOSE7:
	    நிலை NFA_ZCLOSE8:
	    நிலை NFA_ZCLOSE9:
#endif
	    நிலை NFA_MCLOSE:
	    நிலை NFA_MCLOSE1:
	    நிலை NFA_MCLOSE2:
	    நிலை NFA_MCLOSE3:
	    நிலை NFA_MCLOSE4:
	    நிலை NFA_MCLOSE5:
	    நிலை NFA_MCLOSE6:
	    நிலை NFA_MCLOSE7:
	    நிலை NFA_MCLOSE8:
	    நிலை NFA_MCLOSE9:
	    நிலை NFA_NOPEN:
	    நிலை NFA_NCLOSE:

	    நிலை NFA_LNUM_GT:
	    நிலை NFA_LNUM_LT:
	    நிலை NFA_COL_GT:
	    நிலை NFA_COL_LT:
	    நிலை NFA_VCOL_GT:
	    நிலை NFA_VCOL_LT:
	    நிலை NFA_MARK_GT:
	    நிலை NFA_MARK_LT:
	    நிலை NFA_VISUAL:
	    நிலை NFA_LNUM:
	    நிலை NFA_CURSOR:
	    நிலை NFA_COL:
	    நிலை NFA_VCOL:
	    நிலை NFA_MARK:

	    நிலை NFA_ZSTART:
	    நிலை NFA_ZEND:
	    நிலை NFA_OPT_CHARS:
	    நிலை NFA_EMPTY:
	    நிலை NFA_START_PATTERN:
	    நிலை NFA_END_PATTERN:
	    நிலை NFA_COMPOSING:
	    நிலை NFA_END_COMPOSING:
		// zero-width
		நிறுத்து;

	    இயல்பு:
		எனில் (state->c < 0)
		    // don't know what this is
		    திரும்பு -1;
		// normal character
		len += MB_CHAR2LEN(state->c);
		நிறுத்து;
	}

	// normal way to continue
	state = state->out;
    }

    // unrecognized, "cannot happen"
    திரும்பு -1;
}

/*
 * Convert a postfix form into its equivalent NFA.
 * Return the NFA start state on success, NULL otherwise.
 */
    நிலையான nfa_state_T *
post2nfa(முழு *postfix, முழு *end, முழு nfa_calc_size)
{
    முழு		*p;
    முழு		mopen;
    முழு		mclose;
    Frag_T	*அடுக்கு = வெற்று;
    Frag_T	*stackp = வெற்று;
    Frag_T	*அடுக்கு_end = வெற்று;
    Frag_T	e1;
    Frag_T	e2;
    Frag_T	e;
    nfa_state_T	*s;
    nfa_state_T	*s1;
    nfa_state_T	*matchstate;
    nfa_state_T	*ret = வெற்று;

    எனில் (postfix == வெற்று)
	திரும்பு வெற்று;

#define தள்ளு_2(s)	    st_தள்ளு((s), &stackp, அடுக்கு_end)
#define எடு_2()	    st_எடு(&stackp, அடுக்கு);		\
		    எனில் (stackp < அடுக்கு)			\
		    {					\
			st_error(postfix, end, p);	\
			திரும்பு வெற்று;			\
		    }

    எனில் (nfa_calc_size == FALSE)
    {
	// Reuse the fragment stack across compilations, growing when needed;
	// it is freed in free_regexp_stuff().
	எனில் (nfa_அடுக்கு == வெற்று || nfa_அடுக்கு_len < nstate + 1)
	{
	    vim_free(nfa_அடுக்கு);
	    nfa_அடுக்கு = ALLOC_MULT(Frag_T, nstate + 1);
	    எனில் (nfa_அடுக்கு == வெற்று)
	    {
		nfa_அடுக்கு_len = 0;
		திரும்பு வெற்று;
	    }
	    nfa_அடுக்கு_len = nstate + 1;
	}
	அடுக்கு = nfa_அடுக்கு;
	stackp = அடுக்கு;
	அடுக்கு_end = அடுக்கு + (nstate + 1);
    }

    க்கு (p = postfix; p < end; ++p)
    {
	தேர்வு (*p)
	{
	நிலை NFA_CONCAT:
	    // Concatenation.
	    // Pay attention: this operator does not exist in the r.e. itself
	    // (it is implicit, really).  It is added when r.e. is translated
	    // to postfix form in re2post().
	    எனில் (nfa_calc_size == TRUE)
	    {
		// nstate += 0;
		நிறுத்து;
	    }
	    e2 = எடு_2();
	    e1 = எடு_2();
	    patch(e1.out, e2.start);
	    தள்ளு_2(frag(e1.start, e2.out));
	    நிறுத்து;

	நிலை NFA_OR:
	    // Alternation
	    எனில் (nfa_calc_size == TRUE)
	    {
		nstate++;
		நிறுத்து;
	    }
	    e2 = எடு_2();
	    e1 = எடு_2();
	    s = alloc_state(NFA_SPLIT, e1.start, e2.start);
	    எனில் (s == வெற்று)
		செல் theend;
	    தள்ளு_2(frag(s, append(e1.out, e2.out)));
	    நிறுத்து;

	நிலை NFA_STAR:
	    // Zero or more, prefer more
	    எனில் (nfa_calc_size == TRUE)
	    {
		nstate++;
		நிறுத்து;
	    }
	    e = எடு_2();
	    s = alloc_state(NFA_SPLIT, e.start, வெற்று);
	    எனில் (s == வெற்று)
		செல் theend;
	    patch(e.out, s);
	    தள்ளு_2(frag(s, list1(&s->out1)));
	    நிறுத்து;

	நிலை NFA_STAR_NONGREEDY:
	    // Zero or more, prefer zero
	    எனில் (nfa_calc_size == TRUE)
	    {
		nstate++;
		நிறுத்து;
	    }
	    e = எடு_2();
	    s = alloc_state(NFA_SPLIT, வெற்று, e.start);
	    எனில் (s == வெற்று)
		செல் theend;
	    patch(e.out, s);
	    தள்ளு_2(frag(s, list1(&s->out)));
	    நிறுத்து;

	நிலை NFA_QUEST:
	    // one or zero atoms=> greedy match
	    எனில் (nfa_calc_size == TRUE)
	    {
		nstate++;
		நிறுத்து;
	    }
	    e = எடு_2();
	    s = alloc_state(NFA_SPLIT, e.start, வெற்று);
	    எனில் (s == வெற்று)
		செல் theend;
	    தள்ளு_2(frag(s, append(e.out, list1(&s->out1))));
	    நிறுத்து;

	நிலை NFA_QUEST_NONGREEDY:
	    // zero or one atoms => non-greedy match
	    எனில் (nfa_calc_size == TRUE)
	    {
		nstate++;
		நிறுத்து;
	    }
	    e = எடு_2();
	    s = alloc_state(NFA_SPLIT, வெற்று, e.start);
	    எனில் (s == வெற்று)
		செல் theend;
	    தள்ளு_2(frag(s, append(e.out, list1(&s->out))));
	    நிறுத்து;

	நிலை NFA_END_COLL:
	நிலை NFA_END_NEG_COLL:
	    // On the stack is the sequence starting with NFA_START_COLL or
	    // NFA_START_NEG_COLL and all possible characters. Patch it to
	    // add the output to the start.
	    எனில் (nfa_calc_size == TRUE)
	    {
		nstate++;
		நிறுத்து;
	    }
	    e = எடு_2();
	    s = alloc_state(NFA_END_COLL, வெற்று, வெற்று);
	    எனில் (s == வெற்று)
		செல் theend;
	    patch(e.out, s);
	    e.start->out1 = s;
	    தள்ளு_2(frag(e.start, list1(&s->out)));
	    நிறுத்து;

	நிலை NFA_RANGE:
	    // Before this are two characters, the low and high end of a
	    // range.  Turn them into two states with MIN and MAX.
	    எனில் (nfa_calc_size == TRUE)
	    {
		// nstate += 0;
		நிறுத்து;
	    }
	    e2 = எடு_2();
	    e1 = எடு_2();
	    e2.start->val = e2.start->c;
	    e2.start->c = NFA_RANGE_MAX;
	    e1.start->val = e1.start->c;
	    e1.start->c = NFA_RANGE_MIN;
	    patch(e1.out, e2.start);
	    தள்ளு_2(frag(e1.start, e2.out));
	    நிறுத்து;

	நிலை NFA_EMPTY:
	    // 0-length, used in a repetition with max/min count of 0
	    எனில் (nfa_calc_size == TRUE)
	    {
		nstate++;
		நிறுத்து;
	    }
	    s = alloc_state(NFA_EMPTY, வெற்று, வெற்று);
	    எனில் (s == வெற்று)
		செல் theend;
	    தள்ளு_2(frag(s, list1(&s->out)));
	    நிறுத்து;

	நிலை NFA_OPT_CHARS:
	  {
	    முழு    n;

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
	    எனில் (nfa_calc_size == TRUE)
	    {
		nstate += n;
		நிறுத்து;
	    }
	    s = வெற்று; // avoid compiler warning
	    e1.out = வெற்று; // stores list with out1's
	    s1 = வெற்று; // previous NFA_SPLIT to connect to
	    போது (n-- > 0)
	    {
		e = எடு_2(); // get character
		s = alloc_state(NFA_SPLIT, e.start, வெற்று);
		எனில் (s == வெற்று)
		    செல் theend;
		எனில் (e1.out == வெற்று)
		    e1 = e;
		patch(e.out, s1);
		append(e1.out, list1(&s->out1));
		s1 = s;
	    }
	    தள்ளு_2(frag(s, e1.out));
	    நிறுத்து;
	  }

	நிலை NFA_PREV_ATOM_NO_WIDTH:
	நிலை NFA_PREV_ATOM_NO_WIDTH_NEG:
	நிலை NFA_PREV_ATOM_JUST_BEFORE:
	நிலை NFA_PREV_ATOM_JUST_BEFORE_NEG:
	நிலை NFA_PREV_ATOM_LIKE_PATTERN:
	  {
	    முழு before = (*p == NFA_PREV_ATOM_JUST_BEFORE
				      || *p == NFA_PREV_ATOM_JUST_BEFORE_NEG);
	    முழு pattern = (*p == NFA_PREV_ATOM_LIKE_PATTERN);
	    முழு start_state;
	    முழு end_state;
	    முழு n = 0;
	    nfa_state_T *zend;
	    nfa_state_T *skip;

	    தேர்வு (*p)
	    {
		நிலை NFA_PREV_ATOM_NO_WIDTH:
		    start_state = NFA_START_INVISIBLE;
		    end_state = NFA_END_INVISIBLE;
		    நிறுத்து;
		நிலை NFA_PREV_ATOM_NO_WIDTH_NEG:
		    start_state = NFA_START_INVISIBLE_NEG;
		    end_state = NFA_END_INVISIBLE_NEG;
		    நிறுத்து;
		நிலை NFA_PREV_ATOM_JUST_BEFORE:
		    start_state = NFA_START_INVISIBLE_BEFORE;
		    end_state = NFA_END_INVISIBLE;
		    நிறுத்து;
		நிலை NFA_PREV_ATOM_JUST_BEFORE_NEG:
		    start_state = NFA_START_INVISIBLE_BEFORE_NEG;
		    end_state = NFA_END_INVISIBLE_NEG;
		    நிறுத்து;
		இயல்பு: // NFA_PREV_ATOM_LIKE_PATTERN:
		    start_state = NFA_START_PATTERN;
		    end_state = NFA_END_PATTERN;
		    நிறுத்து;
	    }

	    எனில் (before)
		n = *++p; // get the count

	    // The \@= operator: match the preceding atom with zero width.
	    // The \@! operator: no match for the preceding atom.
	    // The \@<= operator: match for the preceding atom.
	    // The \@<! operator: no match for the preceding atom.
	    // Surrounds the preceding atom with START_INVISIBLE and
	    // END_INVISIBLE, similarly to MOPEN.

	    எனில் (nfa_calc_size == TRUE)
	    {
		nstate += pattern ? 4 : 2;
		நிறுத்து;
	    }
	    e = எடு_2();
	    s1 = alloc_state(end_state, வெற்று, வெற்று);
	    எனில் (s1 == வெற்று)
		செல் theend;

	    s = alloc_state(start_state, e.start, s1);
	    எனில் (s == வெற்று)
		செல் theend;
	    எனில் (pattern)
	    {
		// NFA_ZEND -> NFA_END_PATTERN -> NFA_SKIP -> what follows.
		skip = alloc_state(NFA_SKIP, வெற்று, வெற்று);
		எனில் (skip == வெற்று)
		    செல் theend;
		zend = alloc_state(NFA_ZEND, s1, வெற்று);
		எனில் (zend == வெற்று)
		    செல் theend;
		s1->out= skip;
		patch(e.out, zend);
		தள்ளு_2(frag(s, list1(&skip->out)));
	    }
	    இல்லையெனில்
	    {
		patch(e.out, s1);
		தள்ளு_2(frag(s, list1(&s1->out)));
		எனில் (before)
		{
		    எனில் (n <= 0)
			// See if we can guess the maximum width, it avoids a
			// lot of pointless tries.
			n = nfa_max_width(e.start, 0);
		    s->val = n; // store the count
		}
	    }
	    நிறுத்து;
	  }

	நிலை NFA_COMPOSING:	// char with composing char
#எனில் 0
	    // TODO
	    எனில் (regflags & RF_ICOMBINE)
	    {
		// use the base character only
	    }
#endif
	    // FALLTHROUGH

	நிலை NFA_MOPEN:	// \( \) Submatch
	நிலை NFA_MOPEN1:
	நிலை NFA_MOPEN2:
	நிலை NFA_MOPEN3:
	நிலை NFA_MOPEN4:
	நிலை NFA_MOPEN5:
	நிலை NFA_MOPEN6:
	நிலை NFA_MOPEN7:
	நிலை NFA_MOPEN8:
	நிலை NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	நிலை NFA_ZOPEN:	// \z( \) Submatch
	நிலை NFA_ZOPEN1:
	நிலை NFA_ZOPEN2:
	நிலை NFA_ZOPEN3:
	நிலை NFA_ZOPEN4:
	நிலை NFA_ZOPEN5:
	நிலை NFA_ZOPEN6:
	நிலை NFA_ZOPEN7:
	நிலை NFA_ZOPEN8:
	நிலை NFA_ZOPEN9:
#endif
	நிலை NFA_NOPEN:	// \%( \) "Invisible Submatch"
	    எனில் (nfa_calc_size == TRUE)
	    {
		nstate += 2;
		நிறுத்து;
	    }

	    mopen = *p;
	    தேர்வு (*p)
	    {
		நிலை NFA_NOPEN: mclose = NFA_NCLOSE; நிறுத்து;
#ifdef FEAT_SYN_HL
		நிலை NFA_ZOPEN: mclose = NFA_ZCLOSE; நிறுத்து;
		நிலை NFA_ZOPEN1: mclose = NFA_ZCLOSE1; நிறுத்து;
		நிலை NFA_ZOPEN2: mclose = NFA_ZCLOSE2; நிறுத்து;
		நிலை NFA_ZOPEN3: mclose = NFA_ZCLOSE3; நிறுத்து;
		நிலை NFA_ZOPEN4: mclose = NFA_ZCLOSE4; நிறுத்து;
		நிலை NFA_ZOPEN5: mclose = NFA_ZCLOSE5; நிறுத்து;
		நிலை NFA_ZOPEN6: mclose = NFA_ZCLOSE6; நிறுத்து;
		நிலை NFA_ZOPEN7: mclose = NFA_ZCLOSE7; நிறுத்து;
		நிலை NFA_ZOPEN8: mclose = NFA_ZCLOSE8; நிறுத்து;
		நிலை NFA_ZOPEN9: mclose = NFA_ZCLOSE9; நிறுத்து;
#endif
		நிலை NFA_COMPOSING: mclose = NFA_END_COMPOSING; நிறுத்து;
		இயல்பு:
		    // NFA_MOPEN, NFA_MOPEN1 .. NFA_MOPEN9
		    mclose = *p + NSUBEXP;
		    நிறுத்து;
	    }

	    // Allow "NFA_MOPEN" as a valid postfix representation for
	    // the empty regexp "". In this case, the NFA will be
	    // NFA_MOPEN -> NFA_MCLOSE. Note that this also allows
	    // empty groups of parenthesis, and empty mbyte chars
	    எனில் (stackp == அடுக்கு)
	    {
		s = alloc_state(mopen, வெற்று, வெற்று);
		எனில் (s == வெற்று)
		    செல் theend;
		s1 = alloc_state(mclose, வெற்று, வெற்று);
		எனில் (s1 == வெற்று)
		    செல் theend;
		patch(list1(&s->out), s1);
		தள்ளு_2(frag(s, list1(&s1->out)));
		நிறுத்து;
	    }

	    // At least one node was emitted before NFA_MOPEN, so
	    // at least one node will be between NFA_MOPEN and NFA_MCLOSE
	    e = எடு_2();
	    s = alloc_state(mopen, e.start, வெற்று);   // `('
	    எனில் (s == வெற்று)
		செல் theend;

	    s1 = alloc_state(mclose, வெற்று, வெற்று);   // `)'
	    எனில் (s1 == வெற்று)
		செல் theend;
	    patch(e.out, s1);

	    எனில் (mopen == NFA_COMPOSING)
		// COMPOSING->out1 = END_COMPOSING
		patch(list1(&s->out1), s1);

	    தள்ளு_2(frag(s, list1(&s1->out)));
	    நிறுத்து;

	நிலை NFA_BACKREF1:
	நிலை NFA_BACKREF2:
	நிலை NFA_BACKREF3:
	நிலை NFA_BACKREF4:
	நிலை NFA_BACKREF5:
	நிலை NFA_BACKREF6:
	நிலை NFA_BACKREF7:
	நிலை NFA_BACKREF8:
	நிலை NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	நிலை NFA_ZREF1:
	நிலை NFA_ZREF2:
	நிலை NFA_ZREF3:
	நிலை NFA_ZREF4:
	நிலை NFA_ZREF5:
	நிலை NFA_ZREF6:
	நிலை NFA_ZREF7:
	நிலை NFA_ZREF8:
	நிலை NFA_ZREF9:
#endif
	    எனில் (nfa_calc_size == TRUE)
	    {
		nstate += 2;
		நிறுத்து;
	    }
	    s = alloc_state(*p, வெற்று, வெற்று);
	    எனில் (s == வெற்று)
		செல் theend;
	    s1 = alloc_state(NFA_SKIP, வெற்று, வெற்று);
	    எனில் (s1 == வெற்று)
		செல் theend;
	    patch(list1(&s->out), s1);
	    தள்ளு_2(frag(s, list1(&s1->out)));
	    நிறுத்து;

	நிலை NFA_LNUM:
	நிலை NFA_LNUM_GT:
	நிலை NFA_LNUM_LT:
	நிலை NFA_VCOL:
	நிலை NFA_VCOL_GT:
	நிலை NFA_VCOL_LT:
	நிலை NFA_COL:
	நிலை NFA_COL_GT:
	நிலை NFA_COL_LT:
	நிலை NFA_MARK:
	நிலை NFA_MARK_GT:
	நிலை NFA_MARK_LT:
	  {
	    முழு n = *++p; // lnum, col or mark name

	    எனில் (nfa_calc_size == TRUE)
	    {
		nstate += 1;
		நிறுத்து;
	    }
	    s = alloc_state(p[-1], வெற்று, வெற்று);
	    எனில் (s == வெற்று)
		செல் theend;
	    s->val = n;
	    தள்ளு_2(frag(s, list1(&s->out)));
	    நிறுத்து;
	  }

	நிலை NFA_ZSTART:
	நிலை NFA_ZEND:
	இயல்பு:
	    // Operands
	    எனில் (nfa_calc_size == TRUE)
	    {
		nstate++;
		நிறுத்து;
	    }
	    s = alloc_state(*p, வெற்று, வெற்று);
	    எனில் (s == வெற்று)
		செல் theend;
	    தள்ளு_2(frag(s, list1(&s->out)));
	    நிறுத்து;

	} // switch(*p)

    } // for (p = postfix; *p; ++p)

    எனில் (nfa_calc_size == TRUE)
    {
	nstate++;
	செல் theend;	// Return value when counting size is ignored anyway
    }

    e = எடு_2();
    எனில் (stackp != அடுக்கு)
	EMSG_RET_NULL(_(e_nfa_regexp_while_converting_from_postfix_to_nfa_too_many_stats_இடது_on_அடுக்கு));

    எனில் (istate >= nstate)
	EMSG_RET_NULL(_(e_nfa_regexp_not_enough_space_to_store_whole_nfa));

    matchstate = &state_ptr[istate++]; // the match state
    matchstate->c = NFA_MATCH;
    matchstate->out = matchstate->out1 = வெற்று;
    matchstate->id = 0;

    patch(e.out, matchstate);
    ret = e.start;

theend:
    திரும்பு ret;

#undef POP1
#undef PUSH1
#undef POP2
#undef PUSH2
#undef எடு_2
#undef தள்ளு_2
}

/*
 * After building the NFA program, inspect it to add optimization hints.
 */
    நிலையான வெறுமை
nfa_postprocess(nfa_regprog_T *prog)
{
    முழு i;
    முழு c;

    க்கு (i = 0; i < prog->nstate; ++i)
    {
	c = prog->state[i].c;
	எனில் (c == NFA_START_INVISIBLE
		|| c == NFA_START_INVISIBLE_NEG
		|| c == NFA_START_INVISIBLE_BEFORE
		|| c == NFA_START_INVISIBLE_BEFORE_NEG)
	{
	    முழு directly;

	    // Do it directly when what follows is possibly the end of the
	    // match.
	    எனில் (match_follows(prog->state[i].out1->out, 0))
		directly = TRUE;
	    இல்லையெனில்
	    {
		முழு ch_invisible = failure_chance(prog->state[i].out, 0);
		முழு ch_follows = failure_chance(prog->state[i].out1->out, 0);

		// Postpone when the invisible match is expensive or has a
		// lower chance of failing.
		எனில் (c == NFA_START_INVISIBLE_BEFORE
		     || c == NFA_START_INVISIBLE_BEFORE_NEG)
		{
		    // "before" matches are very expensive when
		    // unbounded, always prefer what follows then,
		    // unless what follows will always match.
		    // Otherwise strongly prefer what follows.
		    எனில் (prog->state[i].val <= 0 && ch_follows > 0)
			directly = FALSE;
		    இல்லையெனில்
			directly = ch_follows * 10 < ch_invisible;
		}
		இல்லையெனில்
		{
		    // normal invisible, first do the one with the
		    // highest failure chance
		    directly = ch_follows < ch_invisible;
		}
	    }
	    எனில் (directly)
		// switch to the _FIRST state
		++prog->state[i].c;
	}
    }
}

/////////////////////////////////////////////////////////////////
// NFA execution code.
/////////////////////////////////////////////////////////////////

வகைவரை கட்டமைப்பு
{
    முழு	    in_use; // number of subexpr with useful info

    // When REG_MULTI is TRUE list.multi is used, otherwise list.line.
    ஒன்றிப்பு
    {
	கட்டமைப்பு multipos
	{
	    linenr_T	start_lnum;
	    linenr_T	end_lnum;
	    colnr_T	start_col;
	    colnr_T	end_col;
	} multi[NSUBEXP];
	கட்டமைப்பு linepos
	{
	    char_u	*start;
	    char_u	*end;
	} வரி[NSUBEXP];
    } list;
    colnr_T	orig_start_col;  // list.multi[0].start_col without \zs
} regsub_T;

வகைவரை கட்டமைப்பு
{
    regsub_T	norm; // \( .. \) matches
#ifdef FEAT_SYN_HL
    regsub_T	synt; // \z( .. \) matches
#endif
} regsubs_T;

// nfa_pim_T stores a Postponed Invisible Match.
வகைவரை கட்டமைப்பு nfa_pim_S nfa_pim_T;
கட்டமைப்பு nfa_pim_S
{
    முழு		result;		// NFA_PIM_*, see below
    nfa_state_T	*state;		// the invisible match start state
    regsubs_T	subs;		// submatch info, only party used
    ஒன்றிப்பு
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
வகைவரை கட்டமைப்பு
{
    nfa_state_T	*state;
    முழு		மொத்தம்;
    nfa_pim_T	pim;		// if pim.result != NFA_PIM_UNUSED: postponed
				// invisible match
    regsubs_T	subs;		// submatch info, only party used
} nfa_thread_T;

// nfa_list_T contains the alternative NFA execution states.
வகைவரை கட்டமைப்பு
{
    nfa_thread_T    *t;		// allocated array of states
    முழு		    n;		// nr of states currently in "t"
    முழு		    len;	// max nr of states in "t"
    முழு		    id;		// ID of the list
    முழு		    has_pim;	// TRUE when any state has a PIM
} nfa_list_T;

#ifdef ENABLE_LOG
நிலையான வெறுமை log_subexpr(regsub_T *sub);

    நிலையான வெறுமை
log_subsexpr(regsubs_T *subs)
{
    log_subexpr(&subs->norm);
# ifdef FEAT_SYN_HL
    எனில் (rex.nfa_has_zsubexpr)
	log_subexpr(&subs->synt);
# endif
}

    நிலையான வெறுமை
log_subexpr(regsub_T *sub)
{
    முழு j;

    க்கு (j = 0; j < sub->in_use; j++)
	எனில் (REG_MULTI)
	    fprintf(log_fd,
		    "*** group %d, start: c=%d, l=%d, end: c=%d, l=%d\n",
		    j,
		    sub->list.multi[j].start_col,
		    (முழு)sub->list.multi[j].start_lnum,
		    sub->list.multi[j].end_col,
		    (முழு)sub->list.multi[j].end_lnum);
	இல்லையெனில்
	{
	    எழுத்து *s = (எழுத்து *)sub->list.வரி[j].start;
	    எழுத்து *e = (எழுத்து *)sub->list.வரி[j].end;

	    fprintf(log_fd, "*** group %d, start: \"%s\", end: \"%s\"\n",
		    j,
		    s == வெற்று ? "NULL" : s,
		    e == வெற்று ? "NULL" : e);
	}
}

    நிலையான எழுத்து *
pim_info(nfa_pim_T *pim)
{
    நிலையான எழுத்து buf[30];

    எனில் (pim == வெற்று || pim->result == NFA_PIM_UNUSED)
	buf[0] = NUL;
    இல்லையெனில்
    {
	sprintf(buf, " PIM col %d", REG_MULTI ? (முழு)pim->end.pos.col
		: (முழு)(pim->end.ptr - rex.உள்ளீடு));
    }
    திரும்பு buf;
}

#endif

// Used during execution: whether a match has been found.
நிலையான முழு	    nfa_match;
#ifdef FEAT_RELTIME
நிலையான முழு	   *nfa_timed_out;
#endif

நிலையான உள்வரி வெறுமை copy_sub(regsub_T *to, regsub_T *from);
நிலையான முழு pim_equal(nfa_pim_T *one, nfa_pim_T *two);

/*
 * Copy postponed invisible match info from "from" to "to".
 */
    நிலையான வெறுமை
copy_pim(nfa_pim_T *to, nfa_pim_T *from)
{
    to->result = from->result;
    to->state = from->state;
    copy_sub(&to->subs.norm, &from->subs.norm);
#ifdef FEAT_SYN_HL
    எனில் (rex.nfa_has_zsubexpr)
	copy_sub(&to->subs.synt, &from->subs.synt);
#endif
    to->end = from->end;
}

    நிலையான வெறுமை
clear_sub(regsub_T *sub)
{
    எனில் (REG_MULTI)
	// Use 0xff to set lnum to -1
	vim_memset(sub->list.multi, 0xff,
				  அளவு(கட்டமைப்பு multipos) * rex.nfa_nsubexpr);
    இல்லையெனில்
	vim_memset(sub->list.வரி, 0,
				   அளவு(கட்டமைப்பு linepos) * rex.nfa_nsubexpr);
    sub->in_use = 0;
}

/*
 * Copy the submatches from "from" to "to".
 */
    நிலையான உள்வரி வெறுமை
copy_sub(regsub_T *to, regsub_T *from)
{
    to->in_use = from->in_use;
    எனில் (from->in_use <= 0)
	திரும்பு;

    // Copy the match start and end positions.  addstate() calls this for
    // every state it adds, so it is very hot.  Patterns without capturing
    // groups only use subexpression zero (the whole match), so special-case
    // "in_use == 1" with a direct assignment to avoid the mch_memmove() call.
    எனில் (REG_MULTI)
    {
	எனில் (from->in_use == 1)
	    to->list.multi[0] = from->list.multi[0];
	இல்லையெனில்
	    mch_memmove(&to->list.multi[0],
		    &from->list.multi[0],
		    அளவு(கட்டமைப்பு multipos) * from->in_use);
	to->orig_start_col = from->orig_start_col;
    }
    இல்லையெனில்
    {
	எனில் (from->in_use == 1)
	    to->list.வரி[0] = from->list.வரி[0];
	இல்லையெனில்
	    mch_memmove(&to->list.வரி[0],
		    &from->list.வரி[0],
		    அளவு(கட்டமைப்பு linepos) * from->in_use);
    }
}

/*
 * Like copy_sub() but exclude the main match.
 */
    நிலையான வெறுமை
copy_sub_off(regsub_T *to, regsub_T *from)
{
    எனில் (to->in_use < from->in_use)
	to->in_use = from->in_use;
    எனில் (from->in_use <= 1)
	திரும்பு;

    // Copy the match start and end positions.
    எனில் (REG_MULTI)
	mch_memmove(&to->list.multi[1],
		&from->list.multi[1],
		அளவு(கட்டமைப்பு multipos) * (from->in_use - 1));
    இல்லையெனில்
	mch_memmove(&to->list.வரி[1],
		&from->list.வரி[1],
		அளவு(கட்டமைப்பு linepos) * (from->in_use - 1));
}

/*
 * Like copy_sub() but only do the end of the main match if \ze is present.
 */
    நிலையான வெறுமை
copy_ze_off(regsub_T *to, regsub_T *from)
{
    எனில் (!rex.nfa_has_zend)
	திரும்பு;

    எனில் (REG_MULTI)
    {
	எனில் (from->list.multi[0].end_lnum >= 0)
	{
	    to->list.multi[0].end_lnum = from->list.multi[0].end_lnum;
	    to->list.multi[0].end_col = from->list.multi[0].end_col;
	}
    }
    இல்லையெனில்
    {
	எனில் (from->list.வரி[0].end != வெற்று)
	    to->list.வரி[0].end = from->list.வரி[0].end;
    }
}

/*
 * Return TRUE if "sub1" and "sub2" have the same start positions.
 * When using back-references also check the end position.
 */
    நிலையான முழு
sub_equal(regsub_T *sub1, regsub_T *sub2)
{
    முழு		i;
    முழு		todo;
    linenr_T	s1;
    linenr_T	s2;
    char_u	*sp1;
    char_u	*sp2;

    todo = sub1->in_use > sub2->in_use ? sub1->in_use : sub2->in_use;
    எனில் (REG_MULTI)
    {
	க்கு (i = 0; i < todo; ++i)
	{
	    எனில் (i < sub1->in_use)
		s1 = sub1->list.multi[i].start_lnum;
	    இல்லையெனில்
		s1 = -1;
	    எனில் (i < sub2->in_use)
		s2 = sub2->list.multi[i].start_lnum;
	    இல்லையெனில்
		s2 = -1;
	    எனில் (s1 != s2)
		திரும்பு FALSE;
	    எனில் (s1 != -1 && sub1->list.multi[i].start_col
					     != sub2->list.multi[i].start_col)
		திரும்பு FALSE;

	    எனில் (rex.nfa_has_backref)
	    {
		எனில் (i < sub1->in_use)
		    s1 = sub1->list.multi[i].end_lnum;
		இல்லையெனில்
		    s1 = -1;
		எனில் (i < sub2->in_use)
		    s2 = sub2->list.multi[i].end_lnum;
		இல்லையெனில்
		    s2 = -1;
		எனில் (s1 != s2)
		    திரும்பு FALSE;
		எனில் (s1 != -1 && sub1->list.multi[i].end_col
					       != sub2->list.multi[i].end_col)
		திரும்பு FALSE;
	    }
	}
    }
    இல்லையெனில்
    {
	க்கு (i = 0; i < todo; ++i)
	{
	    எனில் (i < sub1->in_use)
		sp1 = sub1->list.வரி[i].start;
	    இல்லையெனில்
		sp1 = வெற்று;
	    எனில் (i < sub2->in_use)
		sp2 = sub2->list.வரி[i].start;
	    இல்லையெனில்
		sp2 = வெற்று;
	    எனில் (sp1 != sp2)
		திரும்பு FALSE;
	    எனில் (rex.nfa_has_backref)
	    {
		எனில் (i < sub1->in_use)
		    sp1 = sub1->list.வரி[i].end;
		இல்லையெனில்
		    sp1 = வெற்று;
		எனில் (i < sub2->in_use)
		    sp2 = sub2->list.வரி[i].end;
		இல்லையெனில்
		    sp2 = வெற்று;
		எனில் (sp1 != sp2)
		    திரும்பு FALSE;
	    }
	}
    }

    திரும்பு TRUE;
}

#ifdef FEAT_RELTIME
/*
 * Check if we are past the time limit, if there is one.
 */
    நிலையான முழு
nfa_did_time_out(வெறுமை)
{
    எனில் (*timeout_flag)
    {
	எனில் (nfa_timed_out != வெற்று)
	{
# ifdef FEAT_EVAL
	    எனில் (!*nfa_timed_out)
		ch_log(வெற்று, "NFA regexp timed out");
# endif
	    *nfa_timed_out = TRUE;
	}
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}
#endif

#ifdef ENABLE_LOG
    நிலையான வெறுமை
open_debug_log(முழு result)
{
    log_fd = fopen(NFA_REGEXP_RUN_LOG, "a");
    எனில் (log_fd == வெற்று)
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

    நிலையான வெறுமை
report_state(எழுத்து *action,
	     regsub_T *sub,
	     nfa_state_T *state,
	     முழு lid,
	     nfa_pim_T *pim)
{
    முழு col;

    எனில் (sub->in_use <= 0)
	col = -1;
    இல்லையெனில் எனில் (REG_MULTI)
	col = sub->list.multi[0].start_col;
    இல்லையெனில்
	col = (முழு)(sub->list.வரி[0].start - rex.வரி);
    nfa_set_code(state->c);
    எனில் (log_fd == வெற்று)
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
    நிலையான முழு
has_state_with_pos(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim)	// postponed match or NULL
{
    nfa_thread_T	*thread;
    முழு			i;

    க்கு (i = 0; i < l->n; ++i)
    {
	thread = &l->t[i];
	எனில் (thread->state->id == state->id
		&& sub_equal(&thread->subs.norm, &subs->norm)
#ifdef FEAT_SYN_HL
		&& (!rex.nfa_has_zsubexpr
				|| sub_equal(&thread->subs.synt, &subs->synt))
#endif
		&& pim_equal(&thread->pim, pim))
	    திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Return TRUE if "one" and "two" are equal.  That includes when both are not
 * set.
 */
    நிலையான முழு
pim_equal(nfa_pim_T *one, nfa_pim_T *two)
{
    முழு one_unused = (one == வெற்று || one->result == NFA_PIM_UNUSED);
    முழு two_unused = (two == வெற்று || two->result == NFA_PIM_UNUSED);

    எனில் (one_unused)
	// one is unused: equal when two is also unused
	திரும்பு two_unused;
    எனில் (two_unused)
	// one is used and two is not: not equal
	திரும்பு FALSE;
    // compare the state id
    எனில் (one->state->id != two->state->id)
	திரும்பு FALSE;
    // compare the position
    எனில் (REG_MULTI)
	திரும்பு one->end.pos.lnum == two->end.pos.lnum
	    && one->end.pos.col == two->end.pos.col;
    திரும்பு one->end.ptr == two->end.ptr;
}

/*
 * Return TRUE if "state" leads to a NFA_MATCH without advancing the input.
 */
    நிலையான முழு
match_follows(nfa_state_T *startstate, முழு depth)
{
    nfa_state_T	    *state = startstate;

    // avoid too much recursion
    எனில் (depth > 10)
	திரும்பு FALSE;

    போது (state != வெற்று)
    {
	தேர்வு (state->c)
	{
	    நிலை NFA_MATCH:
	    நிலை NFA_MCLOSE:
	    நிலை NFA_END_INVISIBLE:
	    நிலை NFA_END_INVISIBLE_NEG:
	    நிலை NFA_END_PATTERN:
		திரும்பு TRUE;

	    நிலை NFA_SPLIT:
		திரும்பு match_follows(state->out, depth + 1)
				     || match_follows(state->out1, depth + 1);

	    நிலை NFA_START_INVISIBLE:
	    நிலை NFA_START_INVISIBLE_FIRST:
	    நிலை NFA_START_INVISIBLE_BEFORE:
	    நிலை NFA_START_INVISIBLE_BEFORE_FIRST:
	    நிலை NFA_START_INVISIBLE_NEG:
	    நிலை NFA_START_INVISIBLE_NEG_FIRST:
	    நிலை NFA_START_INVISIBLE_BEFORE_NEG:
	    நிலை NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
	    நிலை NFA_COMPOSING:
		// skip ahead to next state
		state = state->out1->out;
		தொடர்;

	    நிலை NFA_ANY:
	    நிலை NFA_ANY_COMPOSING:
	    நிலை NFA_IDENT:
	    நிலை NFA_SIDENT:
	    நிலை NFA_KWORD:
	    நிலை NFA_SKWORD:
	    நிலை NFA_FNAME:
	    நிலை NFA_SFNAME:
	    நிலை NFA_PRINT:
	    நிலை NFA_SPRINT:
	    நிலை NFA_WHITE:
	    நிலை NFA_NWHITE:
	    நிலை NFA_DIGIT:
	    நிலை NFA_NDIGIT:
	    நிலை NFA_HEX:
	    நிலை NFA_NHEX:
	    நிலை NFA_OCTAL:
	    நிலை NFA_NOCTAL:
	    நிலை NFA_WORD:
	    நிலை NFA_NWORD:
	    நிலை NFA_HEAD:
	    நிலை NFA_NHEAD:
	    நிலை NFA_ALPHA:
	    நிலை NFA_NALPHA:
	    நிலை NFA_LOWER:
	    நிலை NFA_NLOWER:
	    நிலை NFA_UPPER:
	    நிலை NFA_NUPPER:
	    நிலை NFA_LOWER_IC:
	    நிலை NFA_NLOWER_IC:
	    நிலை NFA_UPPER_IC:
	    நிலை NFA_NUPPER_IC:
	    நிலை NFA_START_COLL:
	    நிலை NFA_START_NEG_COLL:
	    நிலை NFA_NEWL:
		// state will advance input
		திரும்பு FALSE;

	    இயல்பு:
		எனில் (state->c > 0)
		    // state will advance input
		    திரும்பு FALSE;

		// Others: zero-width or possibly zero-width, might still find
		// a match at the same position, keep looking.
		நிறுத்து;
	}
	state = state->out;
    }
    திரும்பு FALSE;
}


/*
 * Return TRUE if "state" is already in list "l".
 */
    நிலையான முழு
state_in_list(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs)	// pointers to subexpressions
{
    எனில் (state->lastlist[nfa_ll_index] == l->id)
    {
	எனில் (!rex.nfa_has_backref || has_state_with_pos(l, state, subs, வெற்று))
	    திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

// Offset used for "off" by addstate_here().
#define ADDSTATE_HERE_OFFSET 10

/*
 * Add "state" and possibly what follows to state list ".".
 * Returns "subs_arg", possibly copied into temp_subs.
 * Returns NULL when recursiveness is too deep or timed out.
 */
    நிலையான regsubs_T *
addstate(
    nfa_list_T		*l,	    // runtime state list
    nfa_state_T		*state,	    // state to update
    regsubs_T		*subs_arg,  // pointers to subexpressions
    nfa_pim_T		*pim,	    // postponed look-behind match
    முழு			off_arg,    // byte offset, when -1 go to next line
    முழு			depth)	    // recursion depth
{
    முழு			subidx;
    முழு			off = off_arg;
    முழு			add_here = FALSE;
    முழு			listindex = 0;
    முழு			k;
    முழு			found = FALSE;
    nfa_thread_T	*thread;
    கட்டமைப்பு multipos	save_multipos;
    முழு			save_in_use;
    char_u		*save_ptr;
    முழு			i;
    regsub_T		*sub;
    regsubs_T		*subs = subs_arg;
    நிலையான regsubs_T	temp_subs;
#ifdef ENABLE_LOG
    முழு			did_print = FALSE;
#endif

#ifdef FEAT_RELTIME
    எனில் (nfa_did_time_out())
	திரும்பு வெற்று;
#endif

    // This function is called recursively.  When the depth is too much we run
    // out of stack and crash, limit recursiveness here.
    எனில் (depth >= 5000 || subs == வெற்று)
	திரும்பு வெற்று;

    எனில் (off_arg <= -ADDSTATE_HERE_OFFSET)
    {
	add_here = TRUE;
	off = 0;
	listindex = -(off_arg + ADDSTATE_HERE_OFFSET);
    }

    தேர்வு (state->c)
    {
	நிலை NFA_NCLOSE:
	நிலை NFA_MCLOSE:
	நிலை NFA_MCLOSE1:
	நிலை NFA_MCLOSE2:
	நிலை NFA_MCLOSE3:
	நிலை NFA_MCLOSE4:
	நிலை NFA_MCLOSE5:
	நிலை NFA_MCLOSE6:
	நிலை NFA_MCLOSE7:
	நிலை NFA_MCLOSE8:
	நிலை NFA_MCLOSE9:
#ifdef FEAT_SYN_HL
	நிலை NFA_ZCLOSE:
	நிலை NFA_ZCLOSE1:
	நிலை NFA_ZCLOSE2:
	நிலை NFA_ZCLOSE3:
	நிலை NFA_ZCLOSE4:
	நிலை NFA_ZCLOSE5:
	நிலை NFA_ZCLOSE6:
	நிலை NFA_ZCLOSE7:
	நிலை NFA_ZCLOSE8:
	நிலை NFA_ZCLOSE9:
#endif
	நிலை NFA_MOPEN:
	நிலை NFA_ZEND:
	நிலை NFA_SPLIT:
	நிலை NFA_EMPTY:
	    // These nodes are not added themselves but their "out" and/or
	    // "out1" may be added below.
	    நிறுத்து;

	நிலை NFA_BOL:
	நிலை NFA_BOF:
	    // "^" won't match past end-of-line, don't bother trying.
	    // Except when at the end of the line, or when we are going to the
	    // next line for a look-behind match.
	    எனில் (rex.உள்ளீடு > rex.வரி
		    && *rex.உள்ளீடு != NUL
		    && (nfa_endp == வெற்று
			|| !REG_MULTI
			|| rex.lnum == nfa_endp->se_u.pos.lnum))
		செல் skip_add;
	    // FALLTHROUGH

	நிலை NFA_MOPEN1:
	நிலை NFA_MOPEN2:
	நிலை NFA_MOPEN3:
	நிலை NFA_MOPEN4:
	நிலை NFA_MOPEN5:
	நிலை NFA_MOPEN6:
	நிலை NFA_MOPEN7:
	நிலை NFA_MOPEN8:
	நிலை NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	நிலை NFA_ZOPEN:
	நிலை NFA_ZOPEN1:
	நிலை NFA_ZOPEN2:
	நிலை NFA_ZOPEN3:
	நிலை NFA_ZOPEN4:
	நிலை NFA_ZOPEN5:
	நிலை NFA_ZOPEN6:
	நிலை NFA_ZOPEN7:
	நிலை NFA_ZOPEN8:
	நிலை NFA_ZOPEN9:
#endif
	நிலை NFA_NOPEN:
	நிலை NFA_ZSTART:
	    // These nodes need to be added so that we can bail out when it
	    // was added to this list before at the same position to avoid an
	    // endless loop for "\(\)*"

	இயல்பு:
	    எனில் (state->lastlist[nfa_ll_index] == l->id && state->c != NFA_SKIP)
	    {
		// This state is already in the list, don't add it again,
		// unless it is an MOPEN that is used for a backreference or
		// when there is a PIM. For NFA_MATCH check the position,
		// lower position is preferred.
		எனில் (!rex.nfa_has_backref && pim == வெற்று && !l->has_pim
						     && state->c != NFA_MATCH)
		{
		    // When called from addstate_here() do insert before
		    // existing states.
		    எனில் (add_here)
		    {
			க்கு (k = 0; k < l->n && k < listindex; ++k)
			    எனில் (l->t[k].state->id == state->id)
			    {
				found = TRUE;
				நிறுத்து;
			    }
		    }
		    எனில் (!add_here || found)
		    {
skip_add:
#ifdef ENABLE_LOG
			nfa_set_code(state->c);
			fprintf(log_fd, "> Not adding state %d to list %d. char %d: %s pim: %s has_pim: %d found: %d\n",
			    abs(state->id), l->id, state->c, code,
			    pim == வெற்று ? "NULL" : "yes", l->has_pim, found);
#endif
			திரும்பு subs;
		    }
		}

		// Do not add the state again when it exists with the same
		// positions.
		எனில் (has_state_with_pos(l, state, subs, pim))
		    செல் skip_add;
	    }

	    // When there are backreferences or PIMs the number of states may
	    // be (a lot) bigger than anticipated.
	    எனில் (l->n == l->len)
	    {
		முழு		newlen = l->len * 3 / 2 + 50;
		size_t		newsize = newlen * அளவு(nfa_thread_T);
		nfa_thread_T	*newt;

		எனில் ((long)(newsize >> 10) >= p_mmp)
		{
		    emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		    திரும்பு வெற்று;
		}
		எனில் (subs != &temp_subs)
		{
		    // "subs" may point into the current array, need to make a
		    // copy before it becomes invalid.
		    copy_sub(&temp_subs.norm, &subs->norm);
#ifdef FEAT_SYN_HL
		    எனில் (rex.nfa_has_zsubexpr)
			copy_sub(&temp_subs.synt, &subs->synt);
#endif
		    subs = &temp_subs;
		}

		newt = vim_realloc(l->t, newsize);
		எனில் (newt == வெற்று)
		{
		    // out of memory
		    திரும்பு வெற்று;
		}
		l->t = newt;
		l->len = newlen;
	    }

	    // add the state to the list
	    state->lastlist[nfa_ll_index] = l->id;
	    thread = &l->t[l->n++];
	    thread->state = state;
	    எனில் (pim == வெற்று)
		thread->pim.result = NFA_PIM_UNUSED;
	    இல்லையெனில்
	    {
		copy_pim(&thread->pim, pim);
		l->has_pim = TRUE;
	    }
	    copy_sub(&thread->subs.norm, &subs->norm);
#ifdef FEAT_SYN_HL
	    எனில் (rex.nfa_has_zsubexpr)
		copy_sub(&thread->subs.synt, &subs->synt);
#endif
#ifdef ENABLE_LOG
	    report_state("Adding", &thread->subs.norm, state, l->id, pim);
	    did_print = TRUE;
#endif
    }

#ifdef ENABLE_LOG
    எனில் (!did_print)
	report_state("Processing", &subs->norm, state, l->id, pim);
#endif
    தேர்வு (state->c)
    {
	நிலை NFA_MATCH:
	    நிறுத்து;

	நிலை NFA_SPLIT:
	    // order matters here
	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    subs = addstate(l, state->out1, subs, pim, off_arg, depth + 1);
	    நிறுத்து;

	நிலை NFA_EMPTY:
	நிலை NFA_NOPEN:
	நிலை NFA_NCLOSE:
	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    நிறுத்து;

	நிலை NFA_MOPEN:
	நிலை NFA_MOPEN1:
	நிலை NFA_MOPEN2:
	நிலை NFA_MOPEN3:
	நிலை NFA_MOPEN4:
	நிலை NFA_MOPEN5:
	நிலை NFA_MOPEN6:
	நிலை NFA_MOPEN7:
	நிலை NFA_MOPEN8:
	நிலை NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	நிலை NFA_ZOPEN:
	நிலை NFA_ZOPEN1:
	நிலை NFA_ZOPEN2:
	நிலை NFA_ZOPEN3:
	நிலை NFA_ZOPEN4:
	நிலை NFA_ZOPEN5:
	நிலை NFA_ZOPEN6:
	நிலை NFA_ZOPEN7:
	நிலை NFA_ZOPEN8:
	நிலை NFA_ZOPEN9:
#endif
	நிலை NFA_ZSTART:
	    எனில் (state->c == NFA_ZSTART)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#ifdef FEAT_SYN_HL
	    இல்லையெனில் எனில் (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
	    {
		subidx = state->c - NFA_ZOPEN;
		sub = &subs->synt;
	    }
#endif
	    இல்லையெனில்
	    {
		subidx = state->c - NFA_MOPEN;
		sub = &subs->norm;
	    }

	    // avoid compiler warnings
	    save_ptr = வெற்று;
	    CLEAR_FIELD(save_multipos);

	    // Set the position (with "off" added) in the subexpression.  Save
	    // and restore it when it was in use.  Otherwise fill any gap.
	    எனில் (REG_MULTI)
	    {
		எனில் (subidx < sub->in_use)
		{
		    save_multipos = sub->list.multi[subidx];
		    save_in_use = -1;
		}
		இல்லையெனில்
		{
		    save_in_use = sub->in_use;
		    க்கு (i = sub->in_use; i < subidx; ++i)
		    {
			sub->list.multi[i].start_lnum = -1;
			sub->list.multi[i].end_lnum = -1;
		    }
		    sub->in_use = subidx + 1;
		}
		எனில் (off == -1)
		{
		    sub->list.multi[subidx].start_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].start_col = 0;
		}
		இல்லையெனில்
		{
		    sub->list.multi[subidx].start_lnum = rex.lnum;
		    sub->list.multi[subidx].start_col =
					 (colnr_T)(rex.உள்ளீடு - rex.வரி + off);
		}
		sub->list.multi[subidx].end_lnum = -1;
	    }
	    இல்லையெனில்
	    {
		எனில் (subidx < sub->in_use)
		{
		    save_ptr = sub->list.வரி[subidx].start;
		    save_in_use = -1;
		}
		இல்லையெனில்
		{
		    save_in_use = sub->in_use;
		    க்கு (i = sub->in_use; i < subidx; ++i)
		    {
			sub->list.வரி[i].start = வெற்று;
			sub->list.வரி[i].end = வெற்று;
		    }
		    sub->in_use = subidx + 1;
		}
		sub->list.வரி[subidx].start = rex.உள்ளீடு + off;
	    }

	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    எனில் (subs == வெற்று)
		நிறுத்து;
	    // "subs" may have changed, need to set "sub" again
#ifdef FEAT_SYN_HL
	    எனில் (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
		sub = &subs->synt;
	    இல்லையெனில்
#endif
		sub = &subs->norm;

	    எனில் (save_in_use == -1)
	    {
		எனில் (REG_MULTI)
		    sub->list.multi[subidx] = save_multipos;
		இல்லையெனில்
		    sub->list.வரி[subidx].start = save_ptr;
	    }
	    இல்லையெனில்
		sub->in_use = save_in_use;
	    நிறுத்து;

	நிலை NFA_MCLOSE:
	    எனில் (rex.nfa_has_zend && (REG_MULTI
			? subs->norm.list.multi[0].end_lnum >= 0
			: subs->norm.list.வரி[0].end != வெற்று))
	    {
		// Do not overwrite the position set by \ze.
		subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
		நிறுத்து;
	    }
	    // FALLTHROUGH
	நிலை NFA_MCLOSE1:
	நிலை NFA_MCLOSE2:
	நிலை NFA_MCLOSE3:
	நிலை NFA_MCLOSE4:
	நிலை NFA_MCLOSE5:
	நிலை NFA_MCLOSE6:
	நிலை NFA_MCLOSE7:
	நிலை NFA_MCLOSE8:
	நிலை NFA_MCLOSE9:
#ifdef FEAT_SYN_HL
	நிலை NFA_ZCLOSE:
	நிலை NFA_ZCLOSE1:
	நிலை NFA_ZCLOSE2:
	நிலை NFA_ZCLOSE3:
	நிலை NFA_ZCLOSE4:
	நிலை NFA_ZCLOSE5:
	நிலை NFA_ZCLOSE6:
	நிலை NFA_ZCLOSE7:
	நிலை NFA_ZCLOSE8:
	நிலை NFA_ZCLOSE9:
#endif
	நிலை NFA_ZEND:
	    எனில் (state->c == NFA_ZEND)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#ifdef FEAT_SYN_HL
	    இல்லையெனில் எனில் (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
	    {
		subidx = state->c - NFA_ZCLOSE;
		sub = &subs->synt;
	    }
#endif
	    இல்லையெனில்
	    {
		subidx = state->c - NFA_MCLOSE;
		sub = &subs->norm;
	    }

	    // We don't fill in gaps here, there must have been an MOPEN that
	    // has done that.
	    save_in_use = sub->in_use;
	    எனில் (sub->in_use <= subidx)
		sub->in_use = subidx + 1;
	    எனில் (REG_MULTI)
	    {
		save_multipos = sub->list.multi[subidx];
		எனில் (off == -1)
		{
		    sub->list.multi[subidx].end_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].end_col = 0;
		}
		இல்லையெனில்
		{
		    sub->list.multi[subidx].end_lnum = rex.lnum;
		    sub->list.multi[subidx].end_col =
					  (colnr_T)(rex.உள்ளீடு - rex.வரி + off);
		}
		// avoid compiler warnings
		save_ptr = வெற்று;
	    }
	    இல்லையெனில்
	    {
		save_ptr = sub->list.வரி[subidx].end;
		sub->list.வரி[subidx].end = rex.உள்ளீடு + off;
		// avoid compiler warnings
		CLEAR_FIELD(save_multipos);
	    }

	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    எனில் (subs == வெற்று)
		நிறுத்து;
	    // "subs" may have changed, need to set "sub" again
#ifdef FEAT_SYN_HL
	    எனில் (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
		sub = &subs->synt;
	    இல்லையெனில்
#endif
		sub = &subs->norm;

	    எனில் (REG_MULTI)
		sub->list.multi[subidx] = save_multipos;
	    இல்லையெனில்
		sub->list.வரி[subidx].end = save_ptr;
	    sub->in_use = save_in_use;
	    நிறுத்து;
    }
    திரும்பு subs;
}

/*
 * Like addstate(), but the new state(s) are put at position "*ip".
 * Used for zero-width matches, next state to use is the added one.
 * This makes sure the order of states to be tried does not change, which
 * matters for alternatives.
 */
    நிலையான regsubs_T *
addstate_here(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim,   // postponed look-behind match
    முழு			*ip)
{
    முழு tlen = l->n;
    முழு மொத்தம்;
    முழு listidx = *ip;
    regsubs_T *r;

    // First add the state(s) at the end, so that we know how many there are.
    // Pass the listidx as offset (avoids adding another argument to
    // addstate()).
    r = addstate(l, state, subs, pim, -listidx - ADDSTATE_HERE_OFFSET, 0);
    எனில் (r == வெற்று)
	திரும்பு வெற்று;

    // when "*ip" was at the end of the list, nothing to do
    எனில் (listidx + 1 == tlen)
	திரும்பு r;

    // re-order to put the new state at the current position
    மொத்தம் = l->n - tlen;
    எனில் (மொத்தம் == 0)
	திரும்பு r; // no state got added
    எனில் (மொத்தம் == 1)
    {
	// overwrite the current state
	l->t[listidx] = l->t[l->n - 1];
    }
    இல்லையெனில் எனில் (மொத்தம் > 1)
    {
	எனில் (l->n + மொத்தம் - 1 >= l->len)
	{
	    // not enough space to move the new states, reallocate the list
	    // and move the states to the right position
	    முழு		    newlen = l->len * 3 / 2 + 50;
	    size_t	    newsize = newlen * அளவு(nfa_thread_T);
	    nfa_thread_T    *newl;

	    எனில் ((long)(newsize >> 10) >= p_mmp)
	    {
		emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		திரும்பு வெற்று;
	    }
	    newl = alloc(newsize);
	    எனில் (newl == வெற்று)
		திரும்பு வெற்று;
	    l->len = newlen;
	    mch_memmove(&(newl[0]),
		    &(l->t[0]),
		    அளவு(nfa_thread_T) * listidx);
	    mch_memmove(&(newl[listidx]),
		    &(l->t[l->n - மொத்தம்]),
		    அளவு(nfa_thread_T) * மொத்தம்);
	    mch_memmove(&(newl[listidx + மொத்தம்]),
		    &(l->t[listidx + 1]),
		    அளவு(nfa_thread_T) * (l->n - மொத்தம் - listidx - 1));
	    vim_free(l->t);
	    l->t = newl;
	}
	இல்லையெனில்
	{
	    // make space for new states, then move them from the
	    // end to the current position
	    mch_memmove(&(l->t[listidx + மொத்தம்]),
		    &(l->t[listidx + 1]),
		    அளவு(nfa_thread_T) * (l->n - listidx - 1));
	    mch_memmove(&(l->t[listidx]),
		    &(l->t[l->n - 1]),
		    அளவு(nfa_thread_T) * மொத்தம்);
	}
    }
    --l->n;
    *ip = listidx - 1;

    திரும்பு r;
}

/*
 * Check character class "class" against current character c.
 */
    நிலையான முழு
check_char_class(முழு class, முழு c)
{
    தேர்வு (class)
    {
	நிலை NFA_CLASS_ALNUM:
	    எனில் (c >= 1 && c < 128 && isalnum(c))
		திரும்பு OK;
	    நிறுத்து;
	நிலை NFA_CLASS_ALPHA:
	    எனில் (c >= 1 && c < 128 && isalpha(c))
		திரும்பு OK;
	    நிறுத்து;
	நிலை NFA_CLASS_BLANK:
	    எனில் (c == ' ' || c == '\t')
		திரும்பு OK;
	    நிறுத்து;
	நிலை NFA_CLASS_CNTRL:
	    எனில் (c >= 1 && c <= 127 && iscntrl(c))
		திரும்பு OK;
	    நிறுத்து;
	நிலை NFA_CLASS_DIGIT:
	    எனில் (VIM_ISDIGIT(c))
		திரும்பு OK;
	    நிறுத்து;
	நிலை NFA_CLASS_GRAPH:
	    எனில் (c >= 1 && c <= 127 && isgraph(c))
		திரும்பு OK;
	    நிறுத்து;
	நிலை NFA_CLASS_LOWER:
	    எனில் (MB_ISLOWER(c) && c != 170 && c != 186)
		திரும்பு OK;
	    நிறுத்து;
	நிலை NFA_CLASS_PRINT:
	    எனில் (vim_isprintc(c))
		திரும்பு OK;
	    நிறுத்து;
	நிலை NFA_CLASS_PUNCT:
	    எனில் (c >= 1 && c < 128 && ispunct(c))
		திரும்பு OK;
	    நிறுத்து;
	நிலை NFA_CLASS_SPACE:
	    எனில் ((c >= 9 && c <= 13) || (c == ' '))
		திரும்பு OK;
	    நிறுத்து;
	நிலை NFA_CLASS_UPPER:
	    எனில் (MB_ISUPPER(c))
		திரும்பு OK;
	    நிறுத்து;
	நிலை NFA_CLASS_XDIGIT:
	    எனில் (vim_isxdigit(c))
		திரும்பு OK;
	    நிறுத்து;
	நிலை NFA_CLASS_TAB:
	    எனில் (c == '\t')
		திரும்பு OK;
	    நிறுத்து;
	நிலை NFA_CLASS_RETURN:
	    எனில் (c == '\r')
		திரும்பு OK;
	    நிறுத்து;
	நிலை NFA_CLASS_BACKSPACE:
	    எனில் (c == '\b')
		திரும்பு OK;
	    நிறுத்து;
	நிலை NFA_CLASS_ESCAPE:
	    எனில் (c == '\033')
		திரும்பு OK;
	    நிறுத்து;
	நிலை NFA_CLASS_IDENT:
	    எனில் (vim_isIDc(c))
		திரும்பு OK;
	    நிறுத்து;
	நிலை NFA_CLASS_KEYWORD:
	    எனில் (reg_iswordc(c))
		திரும்பு OK;
	    நிறுத்து;
	நிலை NFA_CLASS_FNAME:
	    எனில் (vim_isfilec(c))
		திரும்பு OK;
	    நிறுத்து;

	இயல்பு:
	    // should not be here :P
	    siemsg(_(e_nfa_regexp_invalid_character_class_nr), class);
	    திரும்பு FAIL;
    }
    திரும்பு FAIL;
}

/*
 * Check for a match with subexpression "subidx".
 * Return TRUE if it matches.
 */
    நிலையான முழு
match_backref(
    regsub_T	*sub,	    // pointers to subexpressions
    முழு		subidx,
    முழு		*bytelen)   // out: length of match in bytes
{
    முழு		len;

    எனில் (sub->in_use <= subidx)
    {
retempty:
	// backref was not set, match an empty string
	*bytelen = 0;
	திரும்பு TRUE;
    }

    எனில் (REG_MULTI)
    {
	எனில் (sub->list.multi[subidx].start_lnum < 0
				       || sub->list.multi[subidx].end_lnum < 0)
	    செல் retempty;
	எனில் (sub->list.multi[subidx].start_lnum == rex.lnum
			       && sub->list.multi[subidx].end_lnum == rex.lnum)
	{
	    len = sub->list.multi[subidx].end_col
					  - sub->list.multi[subidx].start_col;
	    எனில் (cstrncmp(rex.வரி + sub->list.multi[subidx].start_col,
							 rex.உள்ளீடு, &len) == 0)
	    {
		*bytelen = len;
		திரும்பு TRUE;
	    }
	}
	இல்லையெனில்
	{
	    எனில் (match_with_backref(
			sub->list.multi[subidx].start_lnum,
			sub->list.multi[subidx].start_col,
			sub->list.multi[subidx].end_lnum,
			sub->list.multi[subidx].end_col,
			bytelen) == RA_MATCH)
		திரும்பு TRUE;
	}
    }
    இல்லையெனில்
    {
	எனில் (sub->list.வரி[subidx].start == வெற்று
					|| sub->list.வரி[subidx].end == வெற்று)
	    செல் retempty;
	len = (முழு)(sub->list.வரி[subidx].end - sub->list.வரி[subidx].start);
	எனில் (cstrncmp(sub->list.வரி[subidx].start, rex.உள்ளீடு, &len) == 0)
	{
	    *bytelen = len;
	    திரும்பு TRUE;
	}
    }
    திரும்பு FALSE;
}

#ifdef FEAT_SYN_HL

/*
 * Check for a match with \z subexpression "subidx".
 * Return TRUE if it matches.
 */
    நிலையான முழு
match_zref(
    முழு		subidx,
    முழு		*bytelen)   // out: length of match in bytes
{
    முழு		len;

    cleanup_zsubexpr();
    எனில் (re_extmatch_in == வெற்று || re_extmatch_in->matches[subidx] == வெற்று)
    {
	// backref was not set, match an empty string
	*bytelen = 0;
	திரும்பு TRUE;
    }

    len = (முழு)STRLEN(re_extmatch_in->matches[subidx]);
    எனில் (cstrncmp(re_extmatch_in->matches[subidx], rex.உள்ளீடு, &len) == 0)
    {
	*bytelen = len;
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}
#endif

/*
 * Save list IDs for all NFA states of "prog" into "list".
 * Also reset the IDs to zero.
 * Only used for the recursive value lastlist[1].
 */
    நிலையான வெறுமை
nfa_save_listids(nfa_regprog_T *prog, முழு *list)
{
    முழு		    i;
    nfa_state_T	    *p;

    // Order in the list is reverse, it's a bit faster that way.
    p = &prog->state[0];
    க்கு (i = prog->nstate; --i >= 0; )
    {
	list[i] = p->lastlist[1];
	p->lastlist[1] = 0;
	++p;
    }
}

/*
 * Restore list IDs from "list" to all NFA states.
 */
    நிலையான வெறுமை
nfa_restore_listids(nfa_regprog_T *prog, முழு *list)
{
    முழு		    i;
    nfa_state_T	    *p;

    p = &prog->state[0];
    க்கு (i = prog->nstate; --i >= 0; )
    {
	p->lastlist[1] = list[i];
	++p;
    }
}

    நிலையான முழு
nfa_re_இலக்கம்_cmp(long_u val, முழு op, long_u pos)
{
    எனில் (op == 1) திரும்பு pos > val;
    எனில் (op == 2) திரும்பு pos < val;
    திரும்பு val == pos;
}

நிலையான முழு nfa_regmatch(nfa_regprog_T *prog, nfa_state_T *start, regsubs_T *submatch, regsubs_T *m);

/*
 * Recursively call nfa_regmatch()
 * "pim" is NULL or contains info about a Postponed Invisible Match (start
 * position).
 */
    நிலையான முழு
recursive_regmatch(
    nfa_state_T	    *state,
    nfa_pim_T	    *pim,
    nfa_regprog_T   *prog,
    regsubs_T	    *submatch,
    regsubs_T	    *m,
    முழு		    **listids,
    முழு		    *listids_len)
{
    முழு		save_reginput_col = (முழு)(rex.உள்ளீடு - rex.வரி);
    முழு		save_reglnum = rex.lnum;
    முழு		save_nfa_match = nfa_match;
    முழு		save_nfa_listid = rex.nfa_listid;
    save_se_T   *save_nfa_endp = nfa_endp;
    save_se_T   endpos;
    save_se_T   *endposp = வெற்று;
    முழு		result;
    முழு		need_restore = FALSE;

    எனில் (pim != வெற்று)
    {
	// start at the position where the postponed match was
	எனில் (REG_MULTI)
	    rex.உள்ளீடு = rex.வரி + pim->end.pos.col;
	இல்லையெனில்
	    rex.உள்ளீடு = pim->end.ptr;
    }

    எனில் (state->c == NFA_START_INVISIBLE_BEFORE
	    || state->c == NFA_START_INVISIBLE_BEFORE_FIRST
	    || state->c == NFA_START_INVISIBLE_BEFORE_NEG
	    || state->c == NFA_START_INVISIBLE_BEFORE_NEG_FIRST)
    {
	// The recursive match must end at the current position. When "pim" is
	// not NULL it specifies the current position.
	endposp = &endpos;
	எனில் (REG_MULTI)
	{
	    எனில் (pim == வெற்று)
	    {
		endpos.se_u.pos.col = (முழு)(rex.உள்ளீடு - rex.வரி);
		endpos.se_u.pos.lnum = rex.lnum;
	    }
	    இல்லையெனில்
		endpos.se_u.pos = pim->end.pos;
	}
	இல்லையெனில்
	{
	    எனில் (pim == வெற்று)
		endpos.se_u.ptr = rex.உள்ளீடு;
	    இல்லையெனில்
		endpos.se_u.ptr = pim->end.ptr;
	}

	// Go back the specified number of bytes, or as far as the
	// start of the previous line, to try matching "\@<=" or
	// not matching "\@<!". This is very inefficient, limit the number of
	// bytes if possible.
	எனில் (state->val <= 0)
	{
	    எனில் (REG_MULTI)
	    {
		rex.வரி = reg_getline(--rex.lnum);
		எனில் (rex.வரி == வெற்று)
		    // can't go before the first line
		    rex.வரி = reg_getline(++rex.lnum);
	    }
	    rex.உள்ளீடு = rex.வரி;
	}
	இல்லையெனில்
	{
	    எனில் (REG_MULTI && (முழு)(rex.உள்ளீடு - rex.வரி) < state->val)
	    {
		// Not enough bytes in this line, go to end of
		// previous line.
		rex.வரி = reg_getline(--rex.lnum);
		எனில் (rex.வரி == வெற்று)
		{
		    // can't go before the first line
		    rex.வரி = reg_getline(++rex.lnum);
		    rex.உள்ளீடு = rex.வரி;
		}
		இல்லையெனில்
		    rex.உள்ளீடு = rex.வரி + reg_getline_len(rex.lnum);
	    }
	    எனில் ((முழு)(rex.உள்ளீடு - rex.வரி) >= state->val)
	    {
		rex.உள்ளீடு -= state->val;
		எனில் (has_mbyte)
		    rex.உள்ளீடு -= mb_head_off(rex.வரி, rex.உள்ளீடு);
	    }
	    இல்லையெனில்
		rex.உள்ளீடு = rex.வரி;
	}
    }

#ifdef ENABLE_LOG
    எனில் (log_fd != stderr)
	fclose(log_fd);
    log_fd = வெற்று;
#endif
    // Have to clear the lastlist field of the NFA nodes, so that
    // nfa_regmatch() and addstate() can run properly after recursion.
    எனில் (nfa_ll_index == 1)
    {
	// Already calling nfa_regmatch() recursively.  Save the lastlist[1]
	// values and clear them.
	எனில் (*listids == வெற்று || *listids_len < prog->nstate)
	{
	    vim_free(*listids);
	    *listids = ALLOC_MULT(முழு, prog->nstate);
	    எனில் (*listids == வெற்று)
	    {
		emsg(_(e_nfa_regexp_could_not_allocate_memory_for_branch_traversal));
		திரும்பு 0;
	    }
	    *listids_len = prog->nstate;
	}
	nfa_save_listids(prog, *listids);
	need_restore = TRUE;
	// any value of rex.nfa_listid will do
    }
    இல்லையெனில்
    {
	// First recursive nfa_regmatch() call, switch to the second lastlist
	// entry.  Make sure rex.nfa_listid is different from a previous
	// recursive call, because some states may still have this ID.
	++nfa_ll_index;
	எனில் (rex.nfa_listid <= rex.nfa_alt_listid)
	    rex.nfa_listid = rex.nfa_alt_listid;
    }

    // Call nfa_regmatch() to check if the current concat matches at this
    // position. The concat ends with the node NFA_END_INVISIBLE
    nfa_endp = endposp;
    result = nfa_regmatch(prog, state->out, submatch, m);

    எனில் (need_restore)
	nfa_restore_listids(prog, *listids);
    இல்லையெனில்
    {
	--nfa_ll_index;
	rex.nfa_alt_listid = rex.nfa_listid;
    }

    // restore position in input text
    rex.lnum = save_reglnum;
    எனில் (REG_MULTI)
	rex.வரி = reg_getline(rex.lnum);
    rex.உள்ளீடு = rex.வரி + save_reginput_col;
    எனில் (result != NFA_TOO_EXPENSIVE)
    {
	nfa_match = save_nfa_match;
	rex.nfa_listid = save_nfa_listid;
    }
    nfa_endp = save_nfa_endp;

#ifdef ENABLE_LOG
    open_debug_log(result);
#endif

    திரும்பு result;
}

/*
 * Estimate the chance of a match with "state" failing.
 * empty match: 0
 * NFA_ANY: 1
 * specific character: 99
 */
    நிலையான முழு
failure_chance(nfa_state_T *state, முழு depth)
{
    முழு c = state->c;
    முழு l, r;

    // detect looping
    எனில் (depth > 4)
	திரும்பு 1;

    தேர்வு (c)
    {
	நிலை NFA_SPLIT:
	    எனில் (state->out->c == NFA_SPLIT || state->out1->c == NFA_SPLIT)
		// avoid recursive stuff
		திரும்பு 1;
	    // two alternatives, use the lowest failure chance
	    l = failure_chance(state->out, depth + 1);
	    r = failure_chance(state->out1, depth + 1);
	    திரும்பு l < r ? l : r;

	நிலை NFA_ANY:
	    // matches anything, unlikely to fail
	    திரும்பு 1;

	நிலை NFA_MATCH:
	நிலை NFA_MCLOSE:
	நிலை NFA_ANY_COMPOSING:
	    // empty match works always
	    திரும்பு 0;

	நிலை NFA_START_INVISIBLE:
	நிலை NFA_START_INVISIBLE_FIRST:
	நிலை NFA_START_INVISIBLE_NEG:
	நிலை NFA_START_INVISIBLE_NEG_FIRST:
	நிலை NFA_START_INVISIBLE_BEFORE:
	நிலை NFA_START_INVISIBLE_BEFORE_FIRST:
	நிலை NFA_START_INVISIBLE_BEFORE_NEG:
	நிலை NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
	நிலை NFA_START_PATTERN:
	    // recursive regmatch is expensive, use low failure chance
	    திரும்பு 5;

	நிலை NFA_BOL:
	நிலை NFA_EOL:
	நிலை NFA_BOF:
	நிலை NFA_EOF:
	நிலை NFA_NEWL:
	    திரும்பு 99;

	நிலை NFA_BOW:
	நிலை NFA_EOW:
	    திரும்பு 90;

	நிலை NFA_MOPEN:
	நிலை NFA_MOPEN1:
	நிலை NFA_MOPEN2:
	நிலை NFA_MOPEN3:
	நிலை NFA_MOPEN4:
	நிலை NFA_MOPEN5:
	நிலை NFA_MOPEN6:
	நிலை NFA_MOPEN7:
	நிலை NFA_MOPEN8:
	நிலை NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	நிலை NFA_ZOPEN:
	நிலை NFA_ZOPEN1:
	நிலை NFA_ZOPEN2:
	நிலை NFA_ZOPEN3:
	நிலை NFA_ZOPEN4:
	நிலை NFA_ZOPEN5:
	நிலை NFA_ZOPEN6:
	நிலை NFA_ZOPEN7:
	நிலை NFA_ZOPEN8:
	நிலை NFA_ZOPEN9:
	நிலை NFA_ZCLOSE:
	நிலை NFA_ZCLOSE1:
	நிலை NFA_ZCLOSE2:
	நிலை NFA_ZCLOSE3:
	நிலை NFA_ZCLOSE4:
	நிலை NFA_ZCLOSE5:
	நிலை NFA_ZCLOSE6:
	நிலை NFA_ZCLOSE7:
	நிலை NFA_ZCLOSE8:
	நிலை NFA_ZCLOSE9:
#endif
	நிலை NFA_NOPEN:
	நிலை NFA_MCLOSE1:
	நிலை NFA_MCLOSE2:
	நிலை NFA_MCLOSE3:
	நிலை NFA_MCLOSE4:
	நிலை NFA_MCLOSE5:
	நிலை NFA_MCLOSE6:
	நிலை NFA_MCLOSE7:
	நிலை NFA_MCLOSE8:
	நிலை NFA_MCLOSE9:
	நிலை NFA_NCLOSE:
	    திரும்பு failure_chance(state->out, depth + 1);

	நிலை NFA_BACKREF1:
	நிலை NFA_BACKREF2:
	நிலை NFA_BACKREF3:
	நிலை NFA_BACKREF4:
	நிலை NFA_BACKREF5:
	நிலை NFA_BACKREF6:
	நிலை NFA_BACKREF7:
	நிலை NFA_BACKREF8:
	நிலை NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	நிலை NFA_ZREF1:
	நிலை NFA_ZREF2:
	நிலை NFA_ZREF3:
	நிலை NFA_ZREF4:
	நிலை NFA_ZREF5:
	நிலை NFA_ZREF6:
	நிலை NFA_ZREF7:
	நிலை NFA_ZREF8:
	நிலை NFA_ZREF9:
#endif
	    // backreferences don't match in many places
	    திரும்பு 94;

	நிலை NFA_LNUM_GT:
	நிலை NFA_LNUM_LT:
	நிலை NFA_COL_GT:
	நிலை NFA_COL_LT:
	நிலை NFA_VCOL_GT:
	நிலை NFA_VCOL_LT:
	நிலை NFA_MARK_GT:
	நிலை NFA_MARK_LT:
	நிலை NFA_VISUAL:
	    // before/after positions don't match very often
	    திரும்பு 85;

	நிலை NFA_LNUM:
	    திரும்பு 90;

	நிலை NFA_CURSOR:
	நிலை NFA_COL:
	நிலை NFA_VCOL:
	நிலை NFA_MARK:
	    // specific positions rarely match
	    திரும்பு 98;

	நிலை NFA_COMPOSING:
	    திரும்பு 95;

	இயல்பு:
	    எனில் (c > 0)
		// character match fails often
		திரும்பு 95;
    }

    // something else, includes character classes
    திரும்பு 50;
}

/*
 * Skip until the char "c" we know a match must start with.
 */
    நிலையான முழு
skip_to_start(முழு c, colnr_T *colp)
{
    char_u *s;

    // Used often, do some work to avoid call overhead.
    எனில் (!rex.reg_ic && !has_mbyte)
	s = vim_strbyte(rex.வரி + *colp, c);
    இல்லையெனில்
	s = cstrchr(rex.வரி + *colp, c);
    எனில் (s == வெற்று)
	திரும்பு FAIL;
    *colp = (முழு)(s - rex.வரி);
    திரும்பு OK;
}

/*
 * Check for a match with match_text.
 * Called after skip_to_start() has found regstart.
 * Returns zero for no match, 1 for a match.
 */
    நிலையான long
find_match_text(colnr_T *startcol, முழு regstart, char_u *match_text)
{
    colnr_T col = *startcol;
    முழு	    c1, c2;
    முழு	    len1, len2;
    முழு	    match;

    க்கு (;;)
    {
	match = TRUE;
	// Skip the input character: case-folding can change its byte length.
	len2 = MB_CPTR2LEN(rex.வரி + col);
	க்கு (len1 = 0; match_text[len1] != NUL; len1 += MB_CHAR2LEN(c1))
	{
	    c1 = PTR2CHAR(match_text + len1);
	    c2 = PTR2CHAR(rex.வரி + col + len2);
	    எனில் (c1 != c2 && (!rex.reg_ic || MB_CASEFOLD(c1) != MB_CASEFOLD(c2)))
	    {
		match = FALSE;
		நிறுத்து;
	    }
	    len2 += enc_utf8 ? utf_ptr2len(rex.வரி + col + len2)
							     : MB_CHAR2LEN(c2);
	}
	எனில் (match
		// check that no composing char follows
		&& !(enc_utf8
			  && utf_iscomposing(PTR2CHAR(rex.வரி + col + len2))))
	{
	    cleanup_subexpr();
	    எனில் (REG_MULTI)
	    {
		rex.reg_startpos[0].lnum = rex.lnum;
		rex.reg_startpos[0].col = col;
		rex.reg_endpos[0].lnum = rex.lnum;
		rex.reg_endpos[0].col = col + len2;
	    }
	    இல்லையெனில்
	    {
		rex.reg_startp[0] = rex.வரி + col;
		rex.reg_endp[0] = rex.வரி + col + len2;
	    }
	    *startcol = col;
	    திரும்பு 1L;
	}

	// Try finding regstart after the current match.
	col += MB_CPTR2LEN(rex.வரி + col); // skip the input character
	எனில் (skip_to_start(regstart, &col) == FAIL)
	    நிறுத்து;
    }

    *startcol = col;
    திரும்பு 0L;
}

/*
 * Check whether the composing characters at the current input position match
 * the NFA composing sub-expression that starts at "sta" (the first state
 * after NFA_COMPOSING).  "curc" is the base character and "clen" its byte
 * length.
 * Returns OK when it matches, FAIL otherwise.
 */
    நிலையான முழு
match_composing(nfa_state_T *sta, முழு curc, முழு clen)
{
    முழு		mc = curc;
    முழு		len = 0;
    முழு		cchars[MAX_MCO];
    முழு		ccount = 0;
    முழு		j;

    எனில் (utf_iscomposing(sta->c))
	// Only match composing character(s), ignore base character.  Used
	// for ".{composing}" and "{composing}" (no preceding character).
	len += mb_char2len(mc);

    எனில் (rex.reg_icombine && len == 0)
	// If \Z was present, then ignore composing characters.  When
	// ignoring the base character this always matches.
	திரும்பு sta->c == curc ? OK : FAIL;

    // Check base character matches first, unless ignored.
    எனில் (len == 0 && mc != sta->c)
	திரும்பு FAIL;

    எனில் (len == 0)
    {
	len += mb_char2len(mc);
	sta = sta->out;
    }

    // We don't care about the order of composing characters.  Get them into
    // cchars[] first.
    போது (len < clen)
    {
	mc = mb_ptr2char(rex.உள்ளீடு + len);
	cchars[ccount++] = mc;
	len += mb_char2len(mc);
	எனில் (ccount == MAX_MCO)
	    நிறுத்து;
    }

    // Check that each composing char in the pattern matches a composing char
    // in the text.  We do not check if all composing chars are matched.
    போது (sta->c != NFA_END_COMPOSING)
    {
	க்கு (j = 0; j < ccount; ++j)
	    எனில் (cchars[j] == sta->c)
		நிறுத்து;
	எனில் (j == ccount)
	    திரும்பு FAIL;
	sta = sta->out;
    }

    திரும்பு OK;
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
    நிலையான முழு
nfa_regmatch(
    nfa_regprog_T	*prog,
    nfa_state_T		*start,
    regsubs_T		*submatch,
    regsubs_T		*m)
{
    முழு		result = FALSE;
    size_t	size = 0;
    முழு		flag = 0;
    முழு		go_to_nextline = FALSE;
    nfa_thread_T *t;
    nfa_list_T	list[2];
    முழு		listidx;
    nfa_list_T	*thislist;
    nfa_list_T	*nextlist;
    முழு		*listids = வெற்று;
    முழு		listids_len = 0;
    nfa_state_T *add_state;
    முழு		add_here;
    முழு		add_மொத்தம்;
    முழு		add_off = 0;
    முழு		toplevel = start->c == NFA_MOPEN;
    regsubs_T	*r;
#ifdef NFA_REGEXP_DEBUG_LOG
    FILE	*debug;
#endif

    // Some patterns may take a long time to match, especially when using
    // recursive_regmatch(). Allow interrupting them with CTRL-C.
    fast_breakcheck();
    எனில் (got_int)
	திரும்பு FALSE;
#ifdef FEAT_RELTIME
    எனில் (nfa_did_time_out())
	திரும்பு FALSE;
#endif

#ifdef NFA_REGEXP_DEBUG_LOG
    debug = fopen(NFA_REGEXP_DEBUG_LOG, "a");
    எனில் (debug == வெற்று)
    {
	semsg("(NFA) COULD NOT OPEN %s!", NFA_REGEXP_DEBUG_LOG);
	திரும்பு FALSE;
    }
#endif
    nfa_match = FALSE;

    // Allocate memory for the lists of nodes.
    size = (prog->nstate + 1) * அளவு(nfa_thread_T);

    // Reuse cached list buffers from prog when available (top-level call).
    // Recursive calls must allocate their own buffers.
    எனில் (toplevel && prog->listbuf[0] != வெற்று)
    {
	list[0].t = (nfa_thread_T *)prog->listbuf[0];
	list[1].t = (nfa_thread_T *)prog->listbuf[1];
	prog->listbuf[0] = வெற்று;
	prog->listbuf[1] = வெற்று;
    }
    இல்லையெனில்
    {
	list[0].t = alloc(size);
	list[1].t = alloc(size);
    }
    list[0].len = prog->nstate + 1;
    list[1].len = prog->nstate + 1;
    எனில் (list[0].t == வெற்று || list[1].t == வெற்று)
	செல் theend;

#ifdef ENABLE_LOG
    log_fd = fopen(NFA_REGEXP_RUN_LOG, "a");
    எனில் (log_fd == வெற்று)
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
    எனில் (toplevel)
    {
	எனில் (REG_MULTI)
	{
	    m->norm.list.multi[0].start_lnum = rex.lnum;
	    m->norm.list.multi[0].start_col = (colnr_T)(rex.உள்ளீடு - rex.வரி);
	    m->norm.orig_start_col = m->norm.list.multi[0].start_col;
	}
	இல்லையெனில்
	    m->norm.list.வரி[0].start = rex.உள்ளீடு;
	m->norm.in_use = 1;
	r = addstate(thislist, start->out, m, வெற்று, 0, 0);
    }
    இல்லையெனில்
	r = addstate(thislist, start, m, வெற்று, 0, 0);
    எனில் (r == வெற்று)
    {
	nfa_match = NFA_TOO_EXPENSIVE;
	செல் theend;
    }

#define	ADD_STATE_IF_MATCH(state)	\
    எனில் (result)				\
    {					\
	add_state = state->out;		\
	add_off = clen;			\
    }

    /*
     * Run for each character.
     */
    க்கு (;;)
    {
	முழு	curc;
	முழு	clen;

	எனில் (has_mbyte)
	{
	    // Fast path for an ASCII byte not followed by a composing
	    // character, matching the check in utfc_ptr2len().  Avoids two
	    // indirect calls for the common case.
	    எனில் (rex.உள்ளீடு[0] != NUL && rex.உள்ளீடு[0] < 0x80 && rex.உள்ளீடு[1] < 0x80)
	    {
		curc = rex.உள்ளீடு[0];
		clen = 1;
	    }
	    இல்லையெனில்
	    {
		curc = (*mb_ptr2char)(rex.உள்ளீடு);
		clen = (*mb_ptr2len)(rex.உள்ளீடு);
	    }
	}
	இல்லையெனில்
	{
	    curc = *rex.உள்ளீடு;
	    clen = 1;
	}
	எனில் (curc == NUL)
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
	எனில் (prog->re_engine == AUTOMATIC_ENGINE
		&& (rex.nfa_listid >= NFA_MAX_STATES
#ifdef FEAT_EVAL
		    || nfa_fail_for_testing
#endif
		    ))
	{
	    // too many states, retry with old engine
	    nfa_match = NFA_TOO_EXPENSIVE;
	    செல் theend;
	}

	thislist->id = rex.nfa_listid;
	nextlist->id = rex.nfa_listid + 1;

#ifdef ENABLE_LOG
	fprintf(log_fd, "------------------------------------------\n");
	fprintf(log_fd, ">>> Reginput is \"%s\"\n", rex.உள்ளீடு);
	fprintf(log_fd, ">>> Advanced one character... Current char is %c (code %d) \n", curc, (முழு)curc);
	fprintf(log_fd, ">>> Thislist has %d states available: ", thislist->n);
	{
	    முழு i;

	    க்கு (i = 0; i < thislist->n; i++)
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
	எனில் (thislist->n == 0)
	    நிறுத்து;

	// compute nextlist
	க்கு (listidx = 0; listidx < thislist->n; ++listidx)
	{
	    // If the list gets very long there probably is something wrong.
	    // At least allow interrupting with CTRL-C.  Only check once in a
	    // while, calling ui_breakcheck() for every state is too slow.
	    நிலையான முழு	breakcheck_மொத்தம் = 0;  // using "static" makes it faster
	    எனில் (unlikely(++breakcheck_மொத்தம் >= 100000))
	    {
		ui_breakcheck();
		breakcheck_மொத்தம் = 0;
	    }
	    எனில் (got_int)
		நிறுத்து;
#ifdef FEAT_RELTIME
	    எனில் (nfa_did_time_out())
		நிறுத்து;
#endif
	    t = &thislist->t[listidx];

#ifdef NFA_REGEXP_DEBUG_LOG
	    nfa_set_code(t->state->c);
	    fprintf(debug, "%s, ", code);
#endif
#ifdef ENABLE_LOG
	    {
		முழு col;

		எனில் (t->subs.norm.in_use <= 0)
		    col = -1;
		இல்லையெனில் எனில் (REG_MULTI)
		    col = t->subs.norm.list.multi[0].start_col;
		இல்லையெனில்
		    col = (முழு)(t->subs.norm.list.வரி[0].start - rex.வரி);
		nfa_set_code(t->state->c);
		fprintf(log_fd, "(%d) char %d %s (start col %d)%s... \n",
			abs(t->state->id), (முழு)t->state->c, code, col,
			pim_info(&t->pim));
	    }
#endif

	    /*
	     * Handle the possible codes of the current state.
	     * The most important is NFA_MATCH.
	     */
	    add_state = வெற்று;
	    add_here = FALSE;
	    add_மொத்தம் = 0;
	    தேர்வு (t->state->c)
	    {
	    நிலை NFA_MATCH:
	      {
		// If the match is not at the start of the line, ends before a
		// composing characters and rex.reg_icombine is not set, that
		// is not really a match.
		எனில் (enc_utf8 && !rex.reg_icombine
			     && rex.உள்ளீடு != rex.வரி && utf_iscomposing(curc))
		    நிறுத்து;

		nfa_match = TRUE;
		copy_sub(&submatch->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		எனில் (rex.nfa_has_zsubexpr)
		    copy_sub(&submatch->synt, &t->subs.synt);
#endif
#ifdef ENABLE_LOG
		log_subsexpr(&t->subs);
#endif
		// Found the left-most longest match, do not look at any other
		// states at this position.  When the list of states is going
		// to be empty quit without advancing, so that "rex.input" is
		// correct.
		எனில் (nextlist->n == 0)
		    clen = 0;
		செல் nextchar;
	      }

	    நிலை NFA_END_INVISIBLE:
	    நிலை NFA_END_INVISIBLE_NEG:
	    நிலை NFA_END_PATTERN:
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
		எனில் (nfa_endp != வெற்று)
		{
		    எனில் (REG_MULTI)
			fprintf(log_fd, "Current lnum: %d, endp lnum: %d; current col: %d, endp col: %d\n",
				(முழு)rex.lnum,
				(முழு)nfa_endp->se_u.pos.lnum,
				(முழு)(rex.உள்ளீடு - rex.வரி),
				nfa_endp->se_u.pos.col);
		    இல்லையெனில்
			fprintf(log_fd, "Current col: %d, endp col: %d\n",
				(முழு)(rex.உள்ளீடு - rex.வரி),
				(முழு)(nfa_endp->se_u.ptr - rex.உள்ளீடு));
		}
#endif
		// If "nfa_endp" is set it's only a match if it ends at
		// "nfa_endp"
		எனில் (nfa_endp != வெற்று && (REG_MULTI
			? (rex.lnum != nfa_endp->se_u.pos.lnum
			    || (முழு)(rex.உள்ளீடு - rex.வரி)
						!= nfa_endp->se_u.pos.col)
			: rex.உள்ளீடு != nfa_endp->se_u.ptr))
		    நிறுத்து;

		// do not set submatches for \@!
		எனில் (t->state->c != NFA_END_INVISIBLE_NEG)
		{
		    copy_sub(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		    எனில் (rex.nfa_has_zsubexpr)
			copy_sub(&m->synt, &t->subs.synt);
#endif
		}
#ifdef ENABLE_LOG
		fprintf(log_fd, "Match found:\n");
		log_subsexpr(m);
#endif
		nfa_match = TRUE;
		// See comment above at "goto nextchar".
		எனில் (nextlist->n == 0)
		    clen = 0;
		செல் nextchar;

	    நிலை NFA_START_INVISIBLE:
	    நிலை NFA_START_INVISIBLE_FIRST:
	    நிலை NFA_START_INVISIBLE_NEG:
	    நிலை NFA_START_INVISIBLE_NEG_FIRST:
	    நிலை NFA_START_INVISIBLE_BEFORE:
	    நிலை NFA_START_INVISIBLE_BEFORE_FIRST:
	    நிலை NFA_START_INVISIBLE_BEFORE_NEG:
	    நிலை NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
		{
#ifdef ENABLE_LOG
		    fprintf(log_fd, "Failure chance invisible: %d, what follows: %d\n",
			    failure_chance(t->state->out, 0),
			    failure_chance(t->state->out1->out, 0));
#endif
		    // Do it directly if there already is a PIM or when
		    // nfa_postprocess() detected it will work better.
		    எனில் (t->pim.result != NFA_PIM_UNUSED
			 || t->state->c == NFA_START_INVISIBLE_FIRST
			 || t->state->c == NFA_START_INVISIBLE_NEG_FIRST
			 || t->state->c == NFA_START_INVISIBLE_BEFORE_FIRST
			 || t->state->c == NFA_START_INVISIBLE_BEFORE_NEG_FIRST)
		    {
			முழு in_use = m->norm.in_use;

			// Copy submatch info for the recursive call, opposite
			// of what happens on success below.
			copy_sub_off(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
			எனில் (rex.nfa_has_zsubexpr)
			    copy_sub_off(&m->synt, &t->subs.synt);
#endif

			/*
			 * First try matching the invisible match, then what
			 * follows.
			 */
			result = recursive_regmatch(t->state, வெற்று, prog,
					  submatch, m, &listids, &listids_len);
			எனில் (result == NFA_TOO_EXPENSIVE)
			{
			    nfa_match = result;
			    செல் theend;
			}

			// for \@! and \@<! it is a match when the result is
			// FALSE
			எனில் (result != (t->state->c == NFA_START_INVISIBLE_NEG
			       || t->state->c == NFA_START_INVISIBLE_NEG_FIRST
			       || t->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			       || t->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&t->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
			    எனில் (rex.nfa_has_zsubexpr)
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
		    இல்லையெனில்
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
			எனில் (REG_MULTI)
			{
			    pim.end.pos.col = (முழு)(rex.உள்ளீடு - rex.வரி);
			    pim.end.pos.lnum = rex.lnum;
			}
			இல்லையெனில்
			    pim.end.ptr = rex.உள்ளீடு;

			// t->state->out1 is the corresponding END_INVISIBLE
			// node; Add its out to the current list (zero-width
			// match).
			எனில் (addstate_here(thislist, t->state->out1->out,
					     &t->subs, &pim, &listidx) == வெற்று)
			{
			    nfa_match = NFA_TOO_EXPENSIVE;
			    செல் theend;
			}
		    }
		}
		நிறுத்து;

	    நிலை NFA_START_PATTERN:
	      {
		nfa_state_T *skip = வெற்று;
#ifdef ENABLE_LOG
		முழு	    skip_lid = 0;
#endif

		// There is no point in trying to match the pattern if the
		// output state is not going to be added to the list.
		எனில் (state_in_list(nextlist, t->state->out1->out, &t->subs))
		{
		    skip = t->state->out1->out;
#ifdef ENABLE_LOG
		    skip_lid = nextlist->id;
#endif
		}
		இல்லையெனில் எனில் (state_in_list(nextlist,
					  t->state->out1->out->out, &t->subs))
		{
		    skip = t->state->out1->out->out;
#ifdef ENABLE_LOG
		    skip_lid = nextlist->id;
#endif
		}
		இல்லையெனில் எனில் (state_in_list(thislist,
					  t->state->out1->out->out, &t->subs))
		{
		    skip = t->state->out1->out->out;
#ifdef ENABLE_LOG
		    skip_lid = thislist->id;
#endif
		}
		எனில் (skip != வெற்று)
		{
#ifdef ENABLE_LOG
		    nfa_set_code(skip->c);
		    fprintf(log_fd, "> Not trying to match pattern, output state %d is already in list %d. char %d: %s\n",
			    abs(skip->id), skip_lid, skip->c, code);
#endif
		    நிறுத்து;
		}
		// Copy submatch info to the recursive call, opposite of what
		// happens afterwards.
		copy_sub_off(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		எனில் (rex.nfa_has_zsubexpr)
		    copy_sub_off(&m->synt, &t->subs.synt);
#endif

		// First try matching the pattern.
		result = recursive_regmatch(t->state, வெற்று, prog,
					  submatch, m, &listids, &listids_len);
		எனில் (result == NFA_TOO_EXPENSIVE)
		{
		    nfa_match = result;
		    செல் theend;
		}
		எனில் (result)
		{
		    முழு bytelen;

#ifdef ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN matches:\n");
		    log_subsexpr(m);
#endif
		    // Copy submatch info from the recursive call
		    copy_sub_off(&t->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
		    எனில் (rex.nfa_has_zsubexpr)
			copy_sub_off(&t->subs.synt, &m->synt);
#endif
		    // Now we need to skip over the matched text and then
		    // continue with what follows.
		    எனில் (REG_MULTI)
			// TODO: multi-line match
			bytelen = m->norm.list.multi[0].end_col
						  - (முழு)(rex.உள்ளீடு - rex.வரி);
		    இல்லையெனில்
			bytelen = (முழு)(m->norm.list.வரி[0].end - rex.உள்ளீடு);

#ifdef ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN length: %d\n", bytelen);
#endif
		    எனில் (bytelen == 0)
		    {
			// empty match, output of corresponding
			// NFA_END_PATTERN/NFA_SKIP to be used at current
			// position
			add_here = TRUE;
			add_state = t->state->out1->out->out;
		    }
		    இல்லையெனில் எனில் (bytelen <= clen)
		    {
			// match current character, output of corresponding
			// NFA_END_PATTERN to be used at next position.
			add_state = t->state->out1->out->out;
			add_off = clen;
		    }
		    இல்லையெனில்
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			add_state = t->state->out1->out;
			add_off = bytelen;
			add_மொத்தம் = bytelen - clen;
		    }
		}
		நிறுத்து;
	      }

	    நிலை NFA_BOL:
		எனில் (rex.உள்ளீடு == rex.வரி)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		நிறுத்து;

	    நிலை NFA_EOL:
		எனில் (curc == NUL)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		நிறுத்து;

	    நிலை NFA_BOW:
		result = TRUE;

		எனில் (curc == NUL)
		    result = FALSE;
		இல்லையெனில் எனில் (has_mbyte)
		{
		    முழு this_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.உள்ளீடு, rex.reg_buf);
		    எனில் (this_class <= 1)
			result = FALSE;
		    இல்லையெனில் எனில் (reg_prev_class() == this_class)
			result = FALSE;
		}
		இல்லையெனில் எனில் (!vim_iswordc_buf(curc, rex.reg_buf)
			   || (rex.உள்ளீடு > rex.வரி
			       && vim_iswordc_buf(rex.உள்ளீடு[-1], rex.reg_buf)))
		    result = FALSE;
		எனில் (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		நிறுத்து;

	    நிலை NFA_EOW:
		result = TRUE;
		எனில் (rex.உள்ளீடு == rex.வரி)
		    result = FALSE;
		இல்லையெனில் எனில் (has_mbyte)
		{
		    முழு this_class, prev_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.உள்ளீடு, rex.reg_buf);
		    prev_class = reg_prev_class();
		    எனில் (this_class == prev_class
					|| prev_class == 0 || prev_class == 1)
			result = FALSE;
		}
		இல்லையெனில் எனில் (!vim_iswordc_buf(rex.உள்ளீடு[-1], rex.reg_buf)
			|| (rex.உள்ளீடு[0] != NUL
					&& vim_iswordc_buf(curc, rex.reg_buf)))
		    result = FALSE;
		எனில் (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		நிறுத்து;

	    நிலை NFA_BOF:
		எனில் (rex.lnum == 0 && rex.உள்ளீடு == rex.வரி
				     && (!REG_MULTI || rex.reg_firstlnum == 1))
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		நிறுத்து;

	    நிலை NFA_EOF:
		எனில் (rex.lnum == rex.reg_maxline && curc == NUL)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		நிறுத்து;

	    நிலை NFA_COMPOSING:
	    {
		nfa_state_T *end;

		result = match_composing(t->state->out, curc, clen);

		end = t->state->out1;	    // NFA_END_COMPOSING
		ADD_STATE_IF_MATCH(end);
		நிறுத்து;
	    }

	    நிலை NFA_NEWL:
		எனில் (curc == NUL && !rex.reg_வரி_lbr && REG_MULTI
						&& rex.lnum <= rex.reg_maxline)
		{
		    go_to_nextline = TRUE;
		    // Pass -1 for the offset, which means taking the position
		    // at the start of the next line.
		    add_state = t->state->out;
		    add_off = -1;
		}
		இல்லையெனில் எனில் (curc == '\n' && rex.reg_வரி_lbr)
		{
		    // match \n as if it is an ordinary character
		    add_state = t->state->out;
		    add_off = 1;
		}
		நிறுத்து;

	    நிலை NFA_START_COLL:
	    நிலை NFA_START_NEG_COLL:
	      {
		// What follows is a list of characters, until NFA_END_COLL.
		// One of them must match or none of them must match.
		nfa_state_T	*state;
		முழு		result_if_matched;
		முழு		c1, c2;

		// Never match EOL. If it's part of the collection it is added
		// as a separate state with an OR.
		எனில் (curc == NUL)
		    நிறுத்து;

		state = t->state->out;
		result_if_matched = (t->state->c == NFA_START_COLL);
		க்கு (;;)
		{
		    எனில் (state->c == NFA_COMPOSING)
		    {
			nfa_state_T *end;

			result = match_composing(t->state->out->out, curc,
									clen);

			எனில் (t->state->out->out1 != வெற்று
				&& t->state->out->out1->c == NFA_END_COMPOSING)
			{
			    end = t->state->out->out1;
			    ADD_STATE_IF_MATCH(end);
			}
			நிறுத்து;
		    }
		    எனில் (state->c == NFA_END_COLL)
		    {
			result = !result_if_matched;
			நிறுத்து;
		    }
		    எனில் (state->c == NFA_RANGE_MIN)
		    {
			c1 = state->val;
			state = state->out; // advance to NFA_RANGE_MAX
			c2 = state->val;
#ifdef ENABLE_LOG
			fprintf(log_fd, "NFA_RANGE_MIN curc=%d c1=%d c2=%d\n",
				curc, c1, c2);
#endif
			எனில் (curc >= c1 && curc <= c2)
			{
			    result = result_if_matched;
			    நிறுத்து;
			}
			எனில் (rex.reg_ic)
			{
			    முழு curc_கீழ் = MB_CASEFOLD(curc);
			    முழு done = FALSE;

			    க்கு ( ; c1 <= c2; ++c1)
				எனில் (MB_CASEFOLD(c1) == curc_கீழ்)
				{
				    result = result_if_matched;
				    done = TRUE;
				    நிறுத்து;
				}
			    எனில் (done)
				நிறுத்து;
			}
		    }
		    இல்லையெனில் எனில் (state->c < 0 ? check_char_class(state->c, curc)
			       : (curc == state->c
				   || (rex.reg_ic && MB_CASEFOLD(curc)
						    == MB_CASEFOLD(state->c))))
		    {
			result = result_if_matched;
			நிறுத்து;
		    }
		    state = state->out;
		}
		எனில் (result)
		{
		    // next state is in out of the NFA_END_COLL, out1 of
		    // START points to the END state
		    add_state = t->state->out1->out;
		    add_off = clen;
		}
		நிறுத்து;
	      }

	    நிலை NFA_ANY:
		// Any char except '\0', (end of input) does not match.
		எனில் (curc > 0)
		{
		    add_state = t->state->out;
		    add_off = clen;
		}
		நிறுத்து;

	    நிலை NFA_ANY_COMPOSING:
		// On a composing character skip over it.  Otherwise do
		// nothing.  Always matches.
		எனில் (enc_utf8 && utf_iscomposing(curc))
		{
		    add_off = clen;
		}
		இல்லையெனில்
		{
		    add_here = TRUE;
		    add_off = 0;
		}
		add_state = t->state->out;
		நிறுத்து;

	    /*
	     * Character classes like \a for alpha, \d for digit etc.
	     */
	    நிலை NFA_IDENT:	//  \i
		result = vim_isIDc(curc);
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_SIDENT:	//  \I
		result = !VIM_ISDIGIT(curc) && vim_isIDc(curc);
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_KWORD:	//  \k
		result = vim_iswordp_buf(rex.உள்ளீடு, rex.reg_buf);
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_SKWORD:	//  \K
		result = !VIM_ISDIGIT(curc)
				     && vim_iswordp_buf(rex.உள்ளீடு, rex.reg_buf);
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_FNAME:	//  \f
		result = vim_isfilec(curc);
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_SFNAME:	//  \F
		result = !VIM_ISDIGIT(curc) && vim_isfilec(curc);
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_PRINT:	//  \p
		result = vim_isprintc(PTR2CHAR(rex.உள்ளீடு));
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_SPRINT:	//  \P
		result = !VIM_ISDIGIT(curc) && vim_isprintc(PTR2CHAR(rex.உள்ளீடு));
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_WHITE:	//  \s
		result = VIM_ISWHITE(curc);
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_NWHITE:	//  \S
		result = curc != NUL && !VIM_ISWHITE(curc);
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_DIGIT:	//  \d
		result = ri_digit(curc);
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_NDIGIT:	//  \D
		result = curc != NUL && !ri_digit(curc);
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_HEX:	//  \x
		result = ri_hex(curc);
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_NHEX:	//  \X
		result = curc != NUL && !ri_hex(curc);
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_OCTAL:	//  \o
		result = ri_octal(curc);
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_NOCTAL:	//  \O
		result = curc != NUL && !ri_octal(curc);
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_WORD:	//  \w
		result = ri_word(curc);
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_NWORD:	//  \W
		result = curc != NUL && !ri_word(curc);
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_HEAD:	//  \h
		result = ri_head(curc);
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_NHEAD:	//  \H
		result = curc != NUL && !ri_head(curc);
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_ALPHA:	//  \a
		result = ri_alpha(curc);
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_NALPHA:	//  \A
		result = curc != NUL && !ri_alpha(curc);
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_LOWER:	//  \l
		result = ri_lower(curc);
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_NLOWER:	//  \L
		result = curc != NUL && !ri_lower(curc);
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_UPPER:	//  \u
		result = ri_upper(curc);
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_NUPPER:	// \U
		result = curc != NUL && !ri_upper(curc);
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_LOWER_IC:	// [a-z]
		result = ri_lower(curc) || (rex.reg_ic && ri_upper(curc));
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_NLOWER_IC:	// [^a-z]
		result = curc != NUL
			&& !(ri_lower(curc) || (rex.reg_ic && ri_upper(curc)));
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_UPPER_IC:	// [A-Z]
		result = ri_upper(curc) || (rex.reg_ic && ri_lower(curc));
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_NUPPER_IC:	// ^[A-Z]
		result = curc != NUL
			&& !(ri_upper(curc) || (rex.reg_ic && ri_lower(curc)));
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;

	    நிலை NFA_BACKREF1:
	    நிலை NFA_BACKREF2:
	    நிலை NFA_BACKREF3:
	    நிலை NFA_BACKREF4:
	    நிலை NFA_BACKREF5:
	    நிலை NFA_BACKREF6:
	    நிலை NFA_BACKREF7:
	    நிலை NFA_BACKREF8:
	    நிலை NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	    நிலை NFA_ZREF1:
	    நிலை NFA_ZREF2:
	    நிலை NFA_ZREF3:
	    நிலை NFA_ZREF4:
	    நிலை NFA_ZREF5:
	    நிலை NFA_ZREF6:
	    நிலை NFA_ZREF7:
	    நிலை NFA_ZREF8:
	    நிலை NFA_ZREF9:
#endif
		// \1 .. \9  \z1 .. \z9
	      {
		முழு subidx;
		முழு bytelen;

#ifdef FEAT_SYN_HL
		எனில் (t->state->c >= NFA_BACKREF1 && t->state->c <= NFA_BACKREF9)
#endif
		{
		    subidx = t->state->c - NFA_BACKREF1 + 1;
		    result = match_backref(&t->subs.norm, subidx, &bytelen);
		}
#ifdef FEAT_SYN_HL
		இல்லையெனில்
		{
		    subidx = t->state->c - NFA_ZREF1 + 1;
		    result = match_zref(subidx, &bytelen);
		}
#endif

		எனில் (result)
		{
		    எனில் (bytelen == 0)
		    {
			// empty match always works, output of NFA_SKIP to be
			// used next
			add_here = TRUE;
			add_state = t->state->out->out;
		    }
		    இல்லையெனில் எனில் (bytelen <= clen)
		    {
			// match current character, jump ahead to out of
			// NFA_SKIP
			add_state = t->state->out->out;
			add_off = clen;
		    }
		    இல்லையெனில்
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			add_state = t->state->out;
			add_off = bytelen;
			add_மொத்தம் = bytelen - clen;
		    }
		}
		நிறுத்து;
	      }
	    நிலை NFA_SKIP:
	      // character of previous matching \1 .. \9  or \@>
	      எனில் (t->மொத்தம் - clen <= 0)
	      {
		  // end of match, go to what follows
		  add_state = t->state->out;
		  add_off = clen;
	      }
	      இல்லையெனில்
	      {
		  // add state again with decremented count
		  add_state = t->state;
		  add_off = 0;
		  add_மொத்தம் = t->மொத்தம் - clen;
	      }
	      நிறுத்து;

	    நிலை NFA_LNUM:
	    நிலை NFA_LNUM_GT:
	    நிலை NFA_LNUM_LT:
		result = (REG_MULTI &&
			nfa_re_இலக்கம்_cmp(t->state->val, t->state->c - NFA_LNUM,
			    (long_u)(rex.lnum + rex.reg_firstlnum)));
		எனில் (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		நிறுத்து;

	    நிலை NFA_COL:
	    நிலை NFA_COL_GT:
	    நிலை NFA_COL_LT:
		result = nfa_re_இலக்கம்_cmp(t->state->val, t->state->c - NFA_COL,
			(long_u)(rex.உள்ளீடு - rex.வரி) + 1);
		எனில் (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		நிறுத்து;

	    நிலை NFA_VCOL:
	    நிலை NFA_VCOL_GT:
	    நிலை NFA_VCOL_LT:
		{
		    முழு     op = t->state->c - NFA_VCOL;
		    colnr_T col = (colnr_T)(rex.உள்ளீடு - rex.வரி);
		    win_T   *wp = rex.reg_win == வெற்று ? curwin : rex.reg_win;

		    // Bail out quickly when there can't be a match, avoid the
		    // overhead of win_linetabsize() on long lines.
		    எனில் (op != 1 && col > t->state->val
			    * (has_mbyte ? MB_MAXBYTES : 1))
			நிறுத்து;
		    result = FALSE;
		    எனில் (op == 1 && col - 1 > t->state->val && col > 100)
		    {
			முழு ts = wp->w_buffer->b_p_ts;

			// Guess that a character won't use more columns than
			// 'tabstop', with a minimum of 4.
			எனில் (ts < 4)
			    ts = 4;
			result = col > t->state->val * ts;
		    }
		    எனில் (!result)
		    {
			linenr_T    lnum = REG_MULTI
					    ? rex.reg_firstlnum + rex.lnum : 1;
			long_u	    vcol;

			எனில் (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_buffer->b_ml.ml_வரி_மொத்தம்))
			    lnum = 1;
			vcol = (long_u)win_linetabsize(wp, lnum, rex.வரி, col);
			result = nfa_re_இலக்கம்_cmp(t->state->val, op, vcol + 1);
		    }
		    எனில் (result)
		    {
			add_here = TRUE;
			add_state = t->state->out;
		    }
		}
		நிறுத்து;

	    நிலை NFA_MARK:
	    நிலை NFA_MARK_GT:
	    நிலை NFA_MARK_LT:
	      {
		pos_T	*pos;
		size_t	col = REG_MULTI ? rex.உள்ளீடு - rex.வரி : 0;

		pos = getmark_buf(rex.reg_buf, t->state->val, FALSE);

		// Line may have been freed, get it again.
		எனில் (REG_MULTI)
		{
		    rex.வரி = reg_getline(rex.lnum);
		    rex.உள்ளீடு = rex.வரி + col;
		}

		// Compare the mark position to the match position, if the mark
		// exists and mark is set in reg_buf.
		எனில் (pos != வெற்று && pos->lnum > 0)
		{
		    colnr_T pos_col = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->col == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->col;

		    result = (pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_col == (colnr_T)(rex.உள்ளீடு - rex.வரி)
				    ? t->state->c == NFA_MARK
				    : (pos_col < (colnr_T)(rex.உள்ளீடு - rex.வரி)
					? t->state->c == NFA_MARK_GT
					: t->state->c == NFA_MARK_LT))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? t->state->c == NFA_MARK_GT
				    : t->state->c == NFA_MARK_LT));
		    எனில் (result)
		    {
			add_here = TRUE;
			add_state = t->state->out;
		    }
		}
		நிறுத்து;
	      }

	    நிலை NFA_CURSOR:
		result = (rex.reg_win != வெற்று
			&& (rex.lnum + rex.reg_firstlnum
						 == rex.reg_win->w_cursor.lnum)
			&& ((colnr_T)(rex.உள்ளீடு - rex.வரி)
						== rex.reg_win->w_cursor.col));
		எனில் (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		நிறுத்து;

	    நிலை NFA_VISUAL:
		result = reg_match_visual();
		எனில் (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		நிறுத்து;

	    நிலை NFA_MOPEN1:
	    நிலை NFA_MOPEN2:
	    நிலை NFA_MOPEN3:
	    நிலை NFA_MOPEN4:
	    நிலை NFA_MOPEN5:
	    நிலை NFA_MOPEN6:
	    நிலை NFA_MOPEN7:
	    நிலை NFA_MOPEN8:
	    நிலை NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	    நிலை NFA_ZOPEN:
	    நிலை NFA_ZOPEN1:
	    நிலை NFA_ZOPEN2:
	    நிலை NFA_ZOPEN3:
	    நிலை NFA_ZOPEN4:
	    நிலை NFA_ZOPEN5:
	    நிலை NFA_ZOPEN6:
	    நிலை NFA_ZOPEN7:
	    நிலை NFA_ZOPEN8:
	    நிலை NFA_ZOPEN9:
#endif
	    நிலை NFA_NOPEN:
	    நிலை NFA_ZSTART:
		// These states are only added to be able to bail out when
		// they are added again, nothing is to be done.
		நிறுத்து;

	    இயல்பு:	// regular character
	      {
		முழு c = t->state->c;

#ifdef DEBUG
		எனில் (c < 0)
		    siemsg("Negative state char: %ld", (long)c);
#endif
		result = (c == curc);

		எனில் (!result && rex.reg_ic)
		    result = MB_CASEFOLD(c) == MB_CASEFOLD(curc);
		// If rex.reg_icombine is not set only skip over the character
		// itself.  When it is set skip over composing characters.
		எனில் (result && enc_utf8 && !rex.reg_icombine)
		    clen = utf_ptr2len(rex.உள்ளீடு);
		ADD_STATE_IF_MATCH(t->state);
		நிறுத்து;
	      }

	    } // switch (t->state->c)

	    எனில் (add_state != வெற்று)
	    {
		nfa_pim_T *pim;
		nfa_pim_T pim_copy;

		எனில் (t->pim.result == NFA_PIM_UNUSED)
		    pim = வெற்று;
		இல்லையெனில்
		    pim = &t->pim;

		// Handle the postponed invisible match if the match might end
		// without advancing and before the end of the line.
		எனில் (pim != வெற்று && (clen == 0 || match_follows(add_state, 0)))
		{
		    எனில் (pim->result == NFA_PIM_TODO)
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
			எனில் (result != (pim->state->c == NFA_START_INVISIBLE_NEG
			     || pim->state->c == NFA_START_INVISIBLE_NEG_FIRST
			     || pim->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&pim->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
			    எனில் (rex.nfa_has_zsubexpr)
				copy_sub_off(&pim->subs.synt, &m->synt);
#endif
			}
		    }
		    இல்லையெனில்
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
		    எனில் (result != (pim->state->c == NFA_START_INVISIBLE_NEG
			     || pim->state->c == NFA_START_INVISIBLE_NEG_FIRST
			     || pim->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
		    {
			// Copy submatch info from the recursive call
			copy_sub_off(&t->subs.norm, &pim->subs.norm);
#ifdef FEAT_SYN_HL
			எனில் (rex.nfa_has_zsubexpr)
			    copy_sub_off(&t->subs.synt, &pim->subs.synt);
#endif
		    }
		    இல்லையெனில்
			// look-behind match failed, don't add the state
			தொடர்;

		    // Postponed invisible match was handled, don't add it to
		    // following states.
		    pim = வெற்று;
		}

		// If "pim" points into l->t it will become invalid when
		// adding the state causes the list to be reallocated.  Make a
		// local copy to avoid that.
		எனில் (pim == &t->pim)
		{
		    copy_pim(&pim_copy, pim);
		    pim = &pim_copy;
		}

		எனில் (add_here)
		    r = addstate_here(thislist, add_state, &t->subs,
								pim, &listidx);
		இல்லையெனில்
		{
		    r = addstate(nextlist, add_state, &t->subs, pim, add_off, 0);
		    எனில் (add_மொத்தம் > 0)
			nextlist->t[nextlist->n - 1].மொத்தம் = add_மொத்தம்;
		}
		எனில் (r == வெற்று)
		{
		    nfa_match = NFA_TOO_EXPENSIVE;
		    செல் theend;
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
	எனில் (nfa_match == FALSE
		&& ((toplevel
			&& rex.lnum == 0
			&& clen != 0
			&& (rex.reg_maxcol == 0
			  || (colnr_T)(rex.உள்ளீடு - rex.வரி) < rex.reg_maxcol))
		    || (nfa_endp != வெற்று
			&& (REG_MULTI
			    ? (rex.lnum < nfa_endp->se_u.pos.lnum
			       || (rex.lnum == nfa_endp->se_u.pos.lnum
				   && (முழு)(rex.உள்ளீடு - rex.வரி)
						    < nfa_endp->se_u.pos.col))
			    : rex.உள்ளீடு < nfa_endp->se_u.ptr))))
	{
#ifdef ENABLE_LOG
	    fprintf(log_fd, "(---) STARTSTATE\n");
#endif
	    // Inline optimized code for addstate() if we know the state is
	    // the first MOPEN.
	    எனில் (toplevel)
	    {
		முழு add = TRUE;
		முழு c;

		எனில் (prog->regstart != NUL && clen != 0)
		{
		    எனில் (nextlist->n == 0)
		    {
			colnr_T col = (colnr_T)(rex.உள்ளீடு - rex.வரி) + clen;

			// Nextlist is empty, we can skip ahead to the
			// character that must appear at the start.
			எனில் (skip_to_start(prog->regstart, &col) == FAIL)
			    நிறுத்து;
#ifdef ENABLE_LOG
			fprintf(log_fd, "  Skipping ahead %d bytes to regstart\n",
				col - ((colnr_T)(rex.உள்ளீடு - rex.வரி) + clen));
#endif
			rex.உள்ளீடு = rex.வரி + col - clen;
		    }
		    இல்லையெனில்
		    {
			// Checking if the required start character matches is
			// cheaper than adding a state that won't match.
			c = PTR2CHAR(rex.உள்ளீடு + clen);
			எனில் (c != prog->regstart && (!rex.reg_ic
			     || MB_CASEFOLD(c) != MB_CASEFOLD(prog->regstart)))
			{
#ifdef ENABLE_LOG
			    fprintf(log_fd, "  Skipping start state, regstart does not match\n");
#endif
			    add = FALSE;
			}
		    }
		}

		எனில் (add)
		{
		    எனில் (REG_MULTI)
		    {
			m->norm.list.multi[0].start_col =
					 (colnr_T)(rex.உள்ளீடு - rex.வரி) + clen;
			m->norm.orig_start_col =
					       m->norm.list.multi[0].start_col;
		    }
		    இல்லையெனில்
			m->norm.list.வரி[0].start = rex.உள்ளீடு + clen;
		    எனில் (addstate(nextlist, start->out, m, வெற்று, clen, 0) == வெற்று)
		    {
			nfa_match = NFA_TOO_EXPENSIVE;
			செல் theend;
		    }
		}
	    }
	    இல்லையெனில்
	    {
		char_u	    *save_வரி = rex.வரி;
		char_u	    *save_உள்ளீடு = rex.உள்ளீடு;
		linenr_T    save_lnum = rex.lnum;

		// At the end of a line the match can only start on the next
		// line, use that position instead of the line break.
		எனில் (REG_MULTI && clen == 0 && nfa_endp != வெற்று
					 && rex.lnum < nfa_endp->se_u.pos.lnum)
		{
		    char_u  *next_வரி = reg_getline(rex.lnum + 1);

		    எனில் (next_வரி != வெற்று)
		    {
			rex.வரி = next_வரி;
			rex.உள்ளீடு = rex.வரி;
			++rex.lnum;
		    }
		}

		r = addstate(nextlist, start, m, வெற்று, clen, 0);

		rex.வரி = save_வரி;
		rex.உள்ளீடு = save_உள்ளீடு;
		rex.lnum = save_lnum;

		எனில் (r == வெற்று)
		{
		    nfa_match = NFA_TOO_EXPENSIVE;
		    செல் theend;
		}
	    }
	}

#ifdef ENABLE_LOG
	fprintf(log_fd, ">>> Thislist had %d states available: ", thislist->n);
	{
	    முழு i;

	    க்கு (i = 0; i < thislist->n; i++)
		fprintf(log_fd, "%d  ", abs(thislist->t[i].state->id));
	}
	fprintf(log_fd, "\n");
#endif

nextchar:
	// Advance to the next character, or advance to the next line, or
	// finish.
	எனில் (clen != 0)
	    rex.உள்ளீடு += clen;
	இல்லையெனில் எனில் (go_to_nextline || (nfa_endp != வெற்று && REG_MULTI
					&& rex.lnum < nfa_endp->se_u.pos.lnum))
	    reg_nextline();
	இல்லையெனில்
	    நிறுத்து;

	// Allow interrupting with CTRL-C.
	வரி_breakcheck();
	எனில் (got_int)
	    நிறுத்து;
#ifdef FEAT_RELTIME
	எனில் (nfa_did_time_out())
	    நிறுத்து;
#endif
    }

#ifdef ENABLE_LOG
    எனில் (log_fd != stderr)
	fclose(log_fd);
    log_fd = வெற்று;
#endif

theend:
    // Cache list buffers in prog for reuse, or free if prog already has
    // cached buffers (recursive call case).
    எனில் (prog->listbuf[0] == வெற்று && list[0].t != வெற்று && list[1].t != வெற்று)
    {
	prog->listbuf[0] = list[0].t;
	prog->listbuf[1] = list[1].t;
    }
    இல்லையெனில்
    {
	vim_free(list[0].t);
	vim_free(list[1].t);
    }
    vim_free(listids);
#undef ADD_STATE_IF_MATCH
#ifdef NFA_REGEXP_DEBUG_LOG
    fclose(debug);
#endif

    திரும்பு nfa_match;
}

/*
 * Try match of "prog" with at rex.line["col"].
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    நிலையான long
nfa_regtry(
    nfa_regprog_T   *prog,
    colnr_T	    col,
    முழு		    *timed_out UNUSED)	// flag set on timeout or NULL
{
    முழு		i;
    regsubs_T	subs, m;
    nfa_state_T	*start = prog->start;
    முழு		result;
#ifdef ENABLE_LOG
    FILE	*f;
#endif

    rex.உள்ளீடு = rex.வரி + col;
#ifdef FEAT_RELTIME
    nfa_timed_out = timed_out;
#endif

#ifdef ENABLE_LOG
    f = fopen(NFA_REGEXP_RUN_LOG, "a");
    எனில் (f != வெற்று)
    {
	fprintf(f, "\n\n\t=======================================================\n");
# ifdef DEBUG
	fprintf(f, "\tRegexp is \"%s\"\n", nfa_regengine.கோவை);
# endif
	fprintf(f, "\tInput text is \"%s\" \n", rex.உள்ளீடு);
	fprintf(f, "\t=======================================================\n\n");
	nfa_print_state(f, start);
	fprintf(f, "\n\n");
	fclose(f);
    }
    இல்லையெனில்
	emsg("Could not open temporary log file for writing");
#endif

    clear_sub(&subs.norm);
    clear_sub(&m.norm);
#ifdef FEAT_SYN_HL
    clear_sub(&subs.synt);
    clear_sub(&m.synt);
#endif

    result = nfa_regmatch(prog, start, &subs, &m);
    எனில் (result == FALSE)
	திரும்பு 0;
    இல்லையெனில் எனில் (result == NFA_TOO_EXPENSIVE)
	திரும்பு result;

    cleanup_subexpr();
    எனில் (REG_MULTI)
    {
	க்கு (i = 0; i < subs.norm.in_use; i++)
	{
	    rex.reg_startpos[i].lnum = subs.norm.list.multi[i].start_lnum;
	    rex.reg_startpos[i].col = subs.norm.list.multi[i].start_col;

	    rex.reg_endpos[i].lnum = subs.norm.list.multi[i].end_lnum;
	    rex.reg_endpos[i].col = subs.norm.list.multi[i].end_col;
	}
	எனில் (rex.reg_mmatch != வெற்று)
	    rex.reg_mmatch->rmm_matchcol = subs.norm.orig_start_col;

	எனில் (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].col = col;
	}
	எனில் (rex.reg_endpos[0].lnum < 0)
	{
	    // pattern has a \ze but it didn't match, use current end
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].col = (முழு)(rex.உள்ளீடு - rex.வரி);
	}
	இல்லையெனில்
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    இல்லையெனில்
    {
	க்கு (i = 0; i < subs.norm.in_use; i++)
	{
	    rex.reg_startp[i] = subs.norm.list.வரி[i].start;
	    rex.reg_endp[i] = subs.norm.list.வரி[i].end;
	}

	எனில் (rex.reg_startp[0] == வெற்று)
	    rex.reg_startp[0] = rex.வரி + col;
	எனில் (rex.reg_endp[0] == வெற்று)
	    rex.reg_endp[0] = rex.உள்ளீடு;
    }

#ifdef FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = வெற்று;

    எனில் (prog->reghasz == REX_SET)
    {
	cleanup_zsubexpr();
	re_extmatch_out = make_extmatch();
	எனில் (re_extmatch_out == வெற்று)
	    திரும்பு 0;
	// Loop over \z1, \z2, etc.  There is no \z0.
	க்கு (i = 1; i < subs.synt.in_use; i++)
	{
	    எனில் (REG_MULTI)
	    {
		கட்டமைப்பு multipos *mpos = &subs.synt.list.multi[i];

		// Only accept single line matches that are valid.
		எனில் (mpos->start_lnum >= 0
			&& mpos->start_lnum == mpos->end_lnum
			&& mpos->end_col >= mpos->start_col)
		    re_extmatch_out->matches[i] =
			vim_strnsave(reg_getline(mpos->start_lnum)
							    + mpos->start_col,
					     mpos->end_col - mpos->start_col);
	    }
	    இல்லையெனில்
	    {
		கட்டமைப்பு linepos *lpos = &subs.synt.list.வரி[i];

		எனில் (lpos->start != வெற்று && lpos->end != வெற்று)
		    re_extmatch_out->matches[i] =
			    vim_strnsave(lpos->start, lpos->end - lpos->start);
	    }
	}
    }
#endif

    திரும்பு 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 *
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    நிலையான long
nfa_regexec_both(
    char_u	*வரி,
    colnr_T	startcol,	// column to start looking for match
    முழு		*timed_out)	// flag set on timeout or NULL
{
    nfa_regprog_T   *prog;
    long	    retval = 0L;
    முழு		    i;
    colnr_T	    col = startcol;

    எனில் (REG_MULTI)
    {
	prog = (nfa_regprog_T *)rex.reg_mmatch->regprog;
	வரி = reg_getline((linenr_T)0);    // relative to the cursor
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    இல்லையெனில்
    {
	prog = (nfa_regprog_T *)rex.reg_match->regprog;
	rex.reg_startp = rex.reg_match->startp;
	rex.reg_endp = rex.reg_match->endp;
    }

    // Be paranoid...
    எனில் (prog == வெற்று || வரி == வெற்று)
    {
	iemsg(e_null_argument);
	செல் theend;
    }

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    எனில் (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    இல்லையெனில் எனில் (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    எனில் (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    rex.வரி = வரி;
    rex.lnum = 0;    // relative to line

    rex.nfa_has_zend = prog->has_zend;
    rex.nfa_has_backref = prog->has_backref;
    rex.nfa_nsubexpr = prog->nsubexp;
    rex.nfa_listid = 1;
    rex.nfa_alt_listid = 2;
#ifdef DEBUG
    nfa_regengine.கோவை = prog->pattern;
#endif

    எனில் (prog->reganch && col > 0)
	திரும்பு 0L;

    rex.need_clear_subexpr = TRUE;
#ifdef FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    எனில் (prog->reghasz == REX_SET)
    {
	rex.nfa_has_zsubexpr = TRUE;
	rex.need_clear_zsubexpr = TRUE;
    }
    இல்லையெனில்
    {
	rex.nfa_has_zsubexpr = FALSE;
	rex.need_clear_zsubexpr = FALSE;
    }
#endif

    எனில் (prog->regstart != NUL)
    {
	// Skip ahead until a character we know the match must start with.
	// When there is none there is no match.
	எனில் (skip_to_start(prog->regstart, &col) == FAIL)
	    திரும்பு 0L;

	// If match_text is set it contains the full text that must match.
	// Nothing else to try. Doesn't handle combining chars well.
	எனில் (prog->match_text != வெற்று && *prog->match_text != NUL && !rex.reg_icombine)
	{
	    retval = find_match_text(&col, prog->regstart, prog->match_text);
	    எனில் (REG_MULTI)
		rex.reg_mmatch->rmm_matchcol = col;
	    இல்லையெனில்
		rex.reg_match->rm_matchcol = col;
	    திரும்பு retval;
	}
    }

    // If the start column is past the maximum column: no need to try.
    எனில் (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	செல் theend;

    // Set the "nstate" used by nfa_regcomp() to zero to trigger an error when
    // it's accidentally used during execution.
    nstate = 0;
    க்கு (i = 0; i < prog->nstate; ++i)
    {
	prog->state[i].id = i;
	prog->state[i].lastlist[0] = 0;
	prog->state[i].lastlist[1] = 0;
    }

    retval = nfa_regtry(prog, col, timed_out);

#ifdef DEBUG
    nfa_regengine.கோவை = வெற்று;
#endif

theend:
    எனில் (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs and
	// \ze are used.
	எனில் (REG_MULTI)
	{
	    lpos_T *start = &rex.reg_mmatch->startpos[0];
	    lpos_T *end = &rex.reg_mmatch->endpos[0];

	    எனில் (end->lnum < start->lnum
			|| (end->lnum == start->lnum && end->col < start->col))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];
	}
	இல்லையெனில்
	{
	    எனில் (rex.reg_match->endp[0] < rex.reg_match->startp[0])
		rex.reg_match->endp[0] = rex.reg_match->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_match->rm_matchcol = col;
	}
    }

    திரும்பு retval;
}

/*
 * Compile a regular expression into internal code for the NFA matcher.
 * Returns the program in allocated space.  Returns NULL for an error.
 */
    நிலையான regprog_T *
nfa_regcomp(char_u *கோவை, முழு re_flags)
{
    nfa_regprog_T	*prog = வெற்று;
    size_t		prog_size;
    முழு			*postfix;

    எனில் (கோவை == வெற்று)
	திரும்பு வெற்று;

#ifdef DEBUG
    nfa_regengine.கோவை = கோவை;
#endif
    nfa_re_flags = re_flags;

    init_class_tab();

    எனில் (nfa_regcomp_start(கோவை, re_flags) == FAIL)
	திரும்பு வெற்று;

    // Build postfix form of the regexp. Needed to build the NFA
    // (and count its size).
    postfix = re2post();
    எனில் (postfix == வெற்று)
	செல் fail;	    // Cascaded (syntax?) error

    /*
     * In order to build the NFA, we parse the input regexp twice:
     * 1. first pass to count size (so we can allocate space)
     * 2. second to emit code
     */
#ifdef ENABLE_LOG
    {
	FILE *f = fopen(NFA_REGEXP_RUN_LOG, "a");

	எனில் (f != வெற்று)
	{
	    fprintf(f, "\n*****************************\n\n\n\n\tCompiling regexp \"%s\"... hold on !\n", கோவை);
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
    prog_size = offsetof(nfa_regprog_T, state) + அளவு(nfa_state_T) * nstate;
    prog = alloc(prog_size);
    எனில் (prog == வெற்று)
	செல் fail;
    state_ptr = prog->state;
    prog->re_in_use = FALSE;
    prog->listbuf[0] = வெற்று;
    prog->listbuf[1] = வெற்று;

    /*
     * PASS 2
     * Build the NFA
     */
    prog->start = post2nfa(postfix, post_ptr, FALSE);
    எனில் (prog->start == வெற்று)
	செல் fail;

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
    nfa_postfix_dump(கோவை, OK);
    nfa_dump(prog);
#endif
#ifdef FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    prog->reghasz = re_has_z;
#endif
    prog->pattern = vim_strsave(கோவை);
#ifdef DEBUG
    nfa_regengine.கோவை = வெற்று;
#endif

out:
    // The postfix list and fragment stack are reused by the next compilation
    // (and freed in free_regexp_stuff()), but drop ones that grew large for a
    // big pattern to avoid holding much memory.
    எனில் (post_start_len > NFA_POSTFIX_KEEP)
    {
	VIM_CLEAR(post_start);
	post_start_len = 0;
    }
    post_ptr = post_end = வெற்று;
    எனில் (nfa_அடுக்கு_len > NFA_அடுக்கு_KEEP)
    {
	VIM_CLEAR(nfa_அடுக்கு);
	nfa_அடுக்கு_len = 0;
    }
    state_ptr = வெற்று;
    திரும்பு (regprog_T *)prog;

fail:
    VIM_CLEAR(prog);
#ifdef ENABLE_LOG
    nfa_postfix_dump(கோவை, FAIL);
#endif
#ifdef DEBUG
    nfa_regengine.கோவை = வெற்று;
#endif
    செல் out;
}

/*
 * Free a compiled regexp program, returned by nfa_regcomp().
 */
    நிலையான வெறுமை
nfa_regfree(regprog_T *prog)
{
    எனில் (prog == வெற்று)
	திரும்பு;

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
    நிலையான முழு
nfa_regexec_nl(
    regmatch_T	*rmp,
    char_u	*வரி,	// string to match against
    colnr_T	col,	// column to start looking for match
    முழு		வரி_lbr)
{
    rex.reg_match = rmp;
    rex.reg_mmatch = வெற்று;
    rex.reg_maxline = 0;
    rex.reg_வரி_lbr = வரி_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = வெற்று;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;
    திரும்பு nfa_regexec_both(வரி, col, வெற்று);
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
    நிலையான long
nfa_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	col,		// column to start looking for match
    முழு		*timed_out)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    திரும்பு nfa_regexec_both(வெற்று, col, timed_out);
}

#ifdef DEBUG
# undef ENABLE_LOG
#endif
