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

شمارش
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
ایستا ثابت صحیح nfa_classcodes[] = {
    NFA_ANY, NFA_IDENT, NFA_SIDENT, NFA_KWORD,NFA_SKWORD,
    NFA_FNAME, NFA_SFNAME, NFA_PRINT, NFA_SPRINT,
    NFA_WHITE, NFA_NWHITE, NFA_DIGIT, NFA_NDIGIT,
    NFA_HEX, NFA_NHEX, NFA_OCTAL, NFA_NOCTAL,
    NFA_WORD, NFA_NWORD, NFA_HEAD, NFA_NHEAD,
    NFA_ALPHA, NFA_NALPHA, NFA_LOWER, NFA_NLOWER,
    NFA_UPPER, NFA_NUPPER
};

// Variables only used in nfa_regcomp() and descendants.
ایستا صحیح nfa_re_flags; // re_flags passed to nfa_regcomp()
ایستا صحیح *post_start;  // holds the postfix form of r.e.
ایستا صحیح post_start_len;  // size of allocated post_start (in ints)
ایستا صحیح *post_end;
ایستا صحیح *post_ptr;
ایستا صحیح nfa_reg_parse_depth;	// nesting depth in nfa_reg()

// The postfix list (post_start) and fragment stack (nfa_stack) are reused
// across compilations; a buffer that grew past these sizes for a big pattern
// is freed afterwards instead of being kept, to avoid holding much memory.
#define NFA_POSTFIX_KEEP	10000	// number of ints (~40 Kbyte)
#define NFA_پشته_KEEP		4000	// number of Frag_T (~64 Kbyte)

// Set when the pattern should use the NFA engine.
// E.g. [[:upper:]] only allows 8bit characters for BT engine,
// while NFA engine handles multibyte characters correctly.
ایستا صحیح wants_nfa;

ایستا صحیح nstate;	// Number of states in the NFA.
ایستا صحیح istate;	// Index in the state vector, used in alloc_state()

// If not NULL match must end at this position
ایستا save_se_T *nfa_endp = پوچ;

// 0 for first call to nfa_regmatch(), 1 for recursive call.
ایستا صحیح nfa_ll_index = 0;

ایستا صحیح realloc_post_list(تهی);
ایستا صحیح nfa_reg(صحیح paren);
#ifdef DEBUG
ایستا تهی nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent);
#endif
ایستا صحیح match_follows(nfa_state_T *startstate, صحیح depth);
ایستا صحیح failure_chance(nfa_state_T *state, صحیح depth);

// helper functions used when doing re2post() ... regatom() parsing
#define EMIT(c)	انجامبده {				\
		    اگر (post_ptr >= post_end && realloc_post_list() == FAIL) \
			بازگشت FAIL;		\
		    *post_ptr++ = c;		\
		} وقتی (0)

/*
 * Initialize internal variables before NFA compilation.
 * Return OK on success, FAIL otherwise.
 */
    ایستا صحیح
nfa_regcomp_start(
    char_u	*عبارت,
    صحیح		re_flags)	    // see vim_regcomp()
{
    size_t	postfix_size;
    صحیح		nstate_max;

    nstate = 0;
    istate = 0;
    // A reasonable estimation for maximum size
    nstate_max = (صحیح)(STRLEN(عبارت) + 1) * 25;

    // Some items blow up in size, such as [A-z].  Add more space for that.
    // When it is still not enough realloc_post_list() will be used.
    nstate_max += 1000;

    // Size for postfix representation of expr.
    postfix_size = اندازه(صحیح) * nstate_max;

    // Reuse the postfix buffer across compilations, only growing it when the
    // estimate exceeds the current size; it is freed in free_regexp_stuff().
    اگر (post_start == پوچ || post_start_len < nstate_max)
    {
	vim_free(post_start);
	post_start = alloc(postfix_size);
	اگر (post_start == پوچ)
	{
	    post_start_len = 0;
	    بازگشت FAIL;
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
    regcomp_start(عبارت, re_flags);

    بازگشت OK;
}

/*
 * Figure out if the NFA state list starts with an anchor, must match at start
 * of the line.
 */
    ایستا صحیح
nfa_get_reganch(nfa_state_T *start, صحیح depth)
{
    nfa_state_T *p = start;

    اگر (depth > 4)
	بازگشت 0;

    وقتی (p != پوچ)
    {
	انتخاب (p->c)
	{
	    حالت NFA_BOL:
	    حالت NFA_BOF:
		بازگشت 1; // yes!

	    حالت NFA_ZSTART:
	    حالت NFA_ZEND:
	    حالت NFA_CURSOR:
	    حالت NFA_VISUAL:

	    حالت NFA_MOPEN:
	    حالت NFA_MOPEN1:
	    حالت NFA_MOPEN2:
	    حالت NFA_MOPEN3:
	    حالت NFA_MOPEN4:
	    حالت NFA_MOPEN5:
	    حالت NFA_MOPEN6:
	    حالت NFA_MOPEN7:
	    حالت NFA_MOPEN8:
	    حالت NFA_MOPEN9:
	    حالت NFA_NOPEN:
#ifdef FEAT_SYN_HL
	    حالت NFA_ZOPEN:
	    حالت NFA_ZOPEN1:
	    حالت NFA_ZOPEN2:
	    حالت NFA_ZOPEN3:
	    حالت NFA_ZOPEN4:
	    حالت NFA_ZOPEN5:
	    حالت NFA_ZOPEN6:
	    حالت NFA_ZOPEN7:
	    حالت NFA_ZOPEN8:
	    حالت NFA_ZOPEN9:
#endif
		p = p->out;
		بشکن;

	    حالت NFA_SPLIT:
		بازگشت nfa_get_reganch(p->out, depth + 1)
				       && nfa_get_reganch(p->out1, depth + 1);

	    پیشفرض:
		بازگشت 0; // noooo
	}
    }
    بازگشت 0;
}

/*
 * Figure out if the NFA state list starts with a character which must match
 * at start of the match.
 */
    ایستا صحیح
nfa_get_regstart(nfa_state_T *start, صحیح depth)
{
    nfa_state_T *p = start;

    اگر (depth > 4)
	بازگشت 0;

    وقتی (p != پوچ)
    {
	انتخاب (p->c)
	{
	    // all kinds of zero-width matches
	    حالت NFA_BOL:
	    حالت NFA_BOF:
	    حالت NFA_BOW:
	    حالت NFA_EOW:
	    حالت NFA_ZSTART:
	    حالت NFA_ZEND:
	    حالت NFA_CURSOR:
	    حالت NFA_VISUAL:
	    حالت NFA_LNUM:
	    حالت NFA_LNUM_GT:
	    حالت NFA_LNUM_LT:
	    حالت NFA_COL:
	    حالت NFA_COL_GT:
	    حالت NFA_COL_LT:
	    حالت NFA_VCOL:
	    حالت NFA_VCOL_GT:
	    حالت NFA_VCOL_LT:
	    حالت NFA_MARK:
	    حالت NFA_MARK_GT:
	    حالت NFA_MARK_LT:

	    حالت NFA_MOPEN:
	    حالت NFA_MOPEN1:
	    حالت NFA_MOPEN2:
	    حالت NFA_MOPEN3:
	    حالت NFA_MOPEN4:
	    حالت NFA_MOPEN5:
	    حالت NFA_MOPEN6:
	    حالت NFA_MOPEN7:
	    حالت NFA_MOPEN8:
	    حالت NFA_MOPEN9:
	    حالت NFA_NOPEN:
#ifdef FEAT_SYN_HL
	    حالت NFA_ZOPEN:
	    حالت NFA_ZOPEN1:
	    حالت NFA_ZOPEN2:
	    حالت NFA_ZOPEN3:
	    حالت NFA_ZOPEN4:
	    حالت NFA_ZOPEN5:
	    حالت NFA_ZOPEN6:
	    حالت NFA_ZOPEN7:
	    حالت NFA_ZOPEN8:
	    حالت NFA_ZOPEN9:
#endif
		p = p->out;
		بشکن;

	    حالت NFA_SPLIT:
	    {
		صحیح c1 = nfa_get_regstart(p->out, depth + 1);
		صحیح c2 = nfa_get_regstart(p->out1, depth + 1);

		اگر (c1 == c2)
		    بازگشت c1; // yes!
		بازگشت 0;
	    }

	    پیشفرض:
		اگر (p->c > 0)
		    بازگشت p->c; // yes!
		بازگشت 0;
	}
    }
    بازگشت 0;
}

/*
 * Figure out if the NFA state list contains just literal text and nothing
 * else.  If so return a string in allocated memory with what must match after
 * regstart.  Otherwise return NULL.
 */
    ایستا char_u *
nfa_get_match_text(nfa_state_T *start)
{
    nfa_state_T *p = start;
    صحیح		len = 0;
    char_u	*ret;
    char_u	*s;

    اگر (p->c != NFA_MOPEN)
	بازگشت پوچ; // just in case
    p = p->out;
    وقتی (p->c > 0)
    {
	len += MB_CHAR2LEN(p->c);
	p = p->out;
    }
    اگر (p->c != NFA_MCLOSE || p->out->c != NFA_MATCH)
	بازگشت پوچ;

    ret = alloc(len);
    اگر (ret == پوچ)
	بازگشت پوچ;

    p = start->out->out; // skip first char, it goes into regstart
    s = ret;
    وقتی (p->c > 0)
    {
	اگر (has_mbyte)
	    s += (*mb_char2bytes)(p->c, s);
	وگرنه
	    *s++ = p->c;
	p = p->out;
    }
    *s = NUL;
    بازگشت ret;
}

/*
 * Allocate more space for post_start.  Called when
 * running above the estimated number of states.
 */
    ایستا صحیح
realloc_post_list(تهی)
{
    صحیح   nstate_max = (صحیح)(post_end - post_start);
    صحیح   new_max;
    صحیح   *new_start;
    صحیح	  *old_start;

    // For weird patterns the number of states can be very high. Increasing by
    // 50% seems a reasonable compromise between memory use and speed.
    new_max = nstate_max * 3 / 2;
    new_start = ALLOC_MULT(صحیح, new_max);
    اگر (new_start == پوچ)
	بازگشت FAIL;
    mch_memmove(new_start, post_start, nstate_max * اندازه(صحیح));
    old_start = post_start;
    post_start = new_start;
    post_start_len = new_max;
    post_ptr = new_start + (post_ptr - old_start);
    post_end = post_start + new_max;
    vim_free(old_start);
    بازگشت OK;
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
    ایستا صحیح
nfa_recognize_char_class(char_u *start, char_u *end, صحیح extra_newl)
{
    صحیح		newl;
    صحیح		config = get_char_class_bits(start, end, &newl);

    اگر (config < 0)
	بازگشت FAIL;

    اگر (extra_newl == TRUE)
	newl = TRUE;

    اگر (newl == TRUE)
	extra_newl = NFA_ADD_NL;

    انتخاب (config)
    {
	حالت CLASS_o9:
	    بازگشت extra_newl + NFA_DIGIT;
	حالت CLASS_not |  CLASS_o9:
	    بازگشت extra_newl + NFA_NDIGIT;
	حالت CLASS_af | CLASS_AF | CLASS_o9:
	    بازگشت extra_newl + NFA_HEX;
	حالت CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    بازگشت extra_newl + NFA_NHEX;
	حالت CLASS_o7:
	    بازگشت extra_newl + NFA_OCTAL;
	حالت CLASS_not | CLASS_o7:
	    بازگشت extra_newl + NFA_NOCTAL;
	حالت CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    بازگشت extra_newl + NFA_WORD;
	حالت CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    بازگشت extra_newl + NFA_NWORD;
	حالت CLASS_az | CLASS_AZ | CLASS_underscore:
	    بازگشت extra_newl + NFA_HEAD;
	حالت CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    بازگشت extra_newl + NFA_NHEAD;
	حالت CLASS_az | CLASS_AZ:
	    بازگشت extra_newl + NFA_ALPHA;
	حالت CLASS_not | CLASS_az | CLASS_AZ:
	    بازگشت extra_newl + NFA_NALPHA;
	حالت CLASS_az:
	   بازگشت extra_newl + NFA_LOWER_IC;
	حالت CLASS_not | CLASS_az:
	    بازگشت extra_newl + NFA_NLOWER_IC;
	حالت CLASS_AZ:
	    بازگشت extra_newl + NFA_UPPER_IC;
	حالت CLASS_not | CLASS_AZ:
	    بازگشت extra_newl + NFA_NUPPER_IC;
    }
    بازگشت FAIL;
}

/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * Emits bytes in postfix notation: 'a,b,NFA_OR,c,NFA_OR' is
 * equivalent to 'a OR b OR c'
 *
 * NOTE! When changing this function, also update reg_equi_class()
 */
    ایستا صحیح
nfa_emit_equi_class(صحیح c)
{
#define EMIT2(c)    EMIT(c); EMIT(NFA_CONCAT);

    اگر (enc_utf8 || STRCMP(p_enc, "latin1") == 0
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
	انتخاب (c)
	{
	    حالت 'A': حالت A_grave: حالت A_acute: حالت A_circumflex:
	    حالت A_virguilla: حالت A_diaeresis: حالت A_ring:
	    حالت 0x100: حالت 0x102: حالت 0x104: حالت 0x1cd:
	    حالت 0x1de: حالت 0x1e0: حالت 0x1fa: حالت 0x200:
	    حالت 0x202: حالت 0x226: حالت 0x23a: حالت 0x1e00:
	    حالت 0x1ea0: حالت 0x1ea2: حالت 0x1ea4: حالت 0x1ea6:
	    حالت 0x1ea8: حالت 0x1eaa: حالت 0x1eac: حالت 0x1eae:
	    حالت 0x1eb0: حالت 0x1eb2: حالت 0x1eb4: حالت 0x1eb6:
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
		    بازگشت OK;

	    حالت 'B': حالت 0x181: حالت 0x243: حالت 0x1e02:
	    حالت 0x1e04: حالت 0x1e06:
		    EMIT2('B')
		    EMIT2(0x181) EMIT2(0x243) EMIT2(0x1e02)
		    EMIT2(0x1e04) EMIT2(0x1e06)
		    بازگشت OK;

	    حالت 'C': حالت C_cedilla: حالت 0x106: حالت 0x108:
	    حالت 0x10a: حالت 0x10c: حالت 0x187: حالت 0x23b:
	    حالت 0x1e08: حالت 0xa792:
		    EMIT2('C') EMIT2(C_cedilla)
		    EMIT2(0x106) EMIT2(0x108) EMIT2(0x10a)
		    EMIT2(0x10c) EMIT2(0x187) EMIT2(0x23b)
		    EMIT2(0x1e08) EMIT2(0xa792)
		    بازگشت OK;

	    حالت 'D': حالت 0x10e: حالت 0x110: حالت 0x18a:
	    حالت 0x1e0a: حالت 0x1e0c: حالت 0x1e0e: حالت 0x1e10:
	    حالت 0x1e12:
		    EMIT2('D') EMIT2(0x10e) EMIT2(0x110) EMIT2(0x18a)
		    EMIT2(0x1e0a) EMIT2(0x1e0c) EMIT2(0x1e0e)
		    EMIT2(0x1e10) EMIT2(0x1e12)
		    بازگشت OK;

	    حالت 'E': حالت E_grave: حالت E_acute: حالت E_circumflex:
	    حالت E_diaeresis: حالت 0x112: حالت 0x114: حالت 0x116:
	    حالت 0x118: حالت 0x11a: حالت 0x204: حالت 0x206:
	    حالت 0x228: حالت 0x246: حالت 0x1e14: حالت 0x1e16:
	    حالت 0x1e18: حالت 0x1e1a: حالت 0x1e1c: حالت 0x1eb8:
	    حالت 0x1eba: حالت 0x1ebc: حالت 0x1ebe: حالت 0x1ec0:
	    حالت 0x1ec2: حالت 0x1ec4: حالت 0x1ec6:
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
		    بازگشت OK;

	    حالت 'F': حالت 0x191: حالت 0x1e1e: حالت 0xa798:
		    EMIT2('F') EMIT2(0x191) EMIT2(0x1e1e) EMIT2(0xa798)
		    بازگشت OK;

	    حالت 'G': حالت 0x11c: حالت 0x11e: حالت 0x120:
	    حالت 0x122: حالت 0x193: حالت 0x1e4: حالت 0x1e6:
	    حالت 0x1f4: حالت 0x1e20: حالت 0xa7a0:
		    EMIT2('G') EMIT2(0x11c) EMIT2(0x11e) EMIT2(0x120)
		    EMIT2(0x122) EMIT2(0x193) EMIT2(0x1e4)
		    EMIT2(0x1e6) EMIT2(0x1f4) EMIT2(0x1e20)
		    EMIT2(0xa7a0)
		    بازگشت OK;

	    حالت 'H': حالت 0x124: حالت 0x126: حالت 0x21e:
	    حالت 0x1e22: حالت 0x1e24: حالت 0x1e26: حالت 0x1e28:
	    حالت 0x1e2a: حالت 0x2c67:
		    EMIT2('H') EMIT2(0x124) EMIT2(0x126) EMIT2(0x21e)
		    EMIT2(0x1e22) EMIT2(0x1e24) EMIT2(0x1e26)
		    EMIT2(0x1e28) EMIT2(0x1e2a) EMIT2(0x2c67)
		    بازگشت OK;

	    حالت 'I': حالت I_grave: حالت I_acute: حالت I_circumflex:
	    حالت I_diaeresis: حالت 0x128: حالت 0x12a: حالت 0x12c:
	    حالت 0x12e: حالت 0x130: حالت 0x197: حالت 0x1cf:
	    حالت 0x208: حالت 0x20a: حالت 0x1e2c: حالت 0x1e2e:
	    حالت 0x1ec8: حالت 0x1eca:
		    EMIT2('I') EMIT2(I_grave) EMIT2(I_acute)
		    EMIT2(I_circumflex) EMIT2(I_diaeresis)
		    EMIT2(0x128) EMIT2(0x12a) EMIT2(0x12c)
		    EMIT2(0x12e) EMIT2(0x130) EMIT2(0x197)
		    EMIT2(0x1cf) EMIT2(0x208) EMIT2(0x20a)
		    EMIT2(0x1e2c) EMIT2(0x1e2e) EMIT2(0x1ec8)
		    EMIT2(0x1eca)
		    بازگشت OK;

	    حالت 'J': حالت 0x134: حالت 0x248:
		    EMIT2('J') EMIT2(0x134) EMIT2(0x248)
		    بازگشت OK;

	    حالت 'K': حالت 0x136: حالت 0x198: حالت 0x1e8: حالت 0x1e30:
	    حالت 0x1e32: حالت 0x1e34: حالت 0x2c69: حالت 0xa740:
		    EMIT2('K') EMIT2(0x136) EMIT2(0x198) EMIT2(0x1e8)
		    EMIT2(0x1e30) EMIT2(0x1e32) EMIT2(0x1e34)
		    EMIT2(0x2c69) EMIT2(0xa740)
		    بازگشت OK;

	    حالت 'L': حالت 0x139: حالت 0x13b: حالت 0x13d:
	    حالت 0x13f: حالت 0x141: حالت 0x23d: حالت 0x1e36:
	    حالت 0x1e38: حالت 0x1e3a: حالت 0x1e3c: حالت 0x2c60:
		    EMIT2('L') EMIT2(0x139) EMIT2(0x13b)
		    EMIT2(0x13d) EMIT2(0x13f) EMIT2(0x141)
		    EMIT2(0x23d) EMIT2(0x1e36) EMIT2(0x1e38)
		    EMIT2(0x1e3a) EMIT2(0x1e3c) EMIT2(0x2c60)
		    بازگشت OK;

	    حالت 'M': حالت 0x1e3e: حالت 0x1e40: حالت 0x1e42:
		    EMIT2('M') EMIT2(0x1e3e) EMIT2(0x1e40)
		    EMIT2(0x1e42)
		    بازگشت OK;

	    حالت 'N': حالت N_virguilla:
	    حالت 0x143: حالت 0x145: حالت 0x147: حالت 0x1f8:
	    حالت 0x1e44: حالت 0x1e46: حالت 0x1e48: حالت 0x1e4a:
	    حالت 0xa7a4:
		    EMIT2('N') EMIT2(N_virguilla)
		    EMIT2(0x143) EMIT2(0x145) EMIT2(0x147)
		    EMIT2(0x1f8) EMIT2(0x1e44) EMIT2(0x1e46)
		    EMIT2(0x1e48) EMIT2(0x1e4a) EMIT2(0xa7a4)
		    بازگشت OK;

	    حالت 'O': حالت O_grave: حالت O_acute: حالت O_circumflex:
	    حالت O_virguilla: حالت O_diaeresis: حالت O_slash:
	    حالت 0x14c: حالت 0x14e: حالت 0x150: حالت 0x19f:
	    حالت 0x1a0: حالت 0x1d1: حالت 0x1ea: حالت 0x1ec:
	    حالت 0x1fe: حالت 0x20c: حالت 0x20e: حالت 0x22a:
	    حالت 0x22c: حالت 0x22e: حالت 0x230: حالت 0x1e4c:
	    حالت 0x1e4e: حالت 0x1e50: حالت 0x1e52: حالت 0x1ecc:
	    حالت 0x1ece: حالت 0x1ed0: حالت 0x1ed2: حالت 0x1ed4:
	    حالت 0x1ed6: حالت 0x1ed8: حالت 0x1eda: حالت 0x1edc:
	    حالت 0x1ede: حالت 0x1ee0: حالت 0x1ee2:
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
		    بازگشت OK;

	    حالت 'P': حالت 0x1a4: حالت 0x1e54: حالت 0x1e56: حالت 0x2c63:
		    EMIT2('P') EMIT2(0x1a4) EMIT2(0x1e54) EMIT2(0x1e56)
		    EMIT2(0x2c63)
		    بازگشت OK;

	    حالت 'Q': حالت 0x24a:
		    EMIT2('Q') EMIT2(0x24a)
		    بازگشت OK;

	    حالت 'R': حالت 0x154: حالت 0x156: حالت 0x158: حالت 0x210:
	    حالت 0x212: حالت 0x24c: حالت 0x1e58: حالت 0x1e5a:
	    حالت 0x1e5c: حالت 0x1e5e: حالت 0x2c64: حالت 0xa7a6:
		    EMIT2('R') EMIT2(0x154) EMIT2(0x156) EMIT2(0x158)
		    EMIT2(0x210) EMIT2(0x212) EMIT2(0x24c) EMIT2(0x1e58)
		    EMIT2(0x1e5a) EMIT2(0x1e5c) EMIT2(0x1e5e) EMIT2(0x2c64)
		    EMIT2(0xa7a6)
		    بازگشت OK;

	    حالت 'S': حالت 0x15a: حالت 0x15c: حالت 0x15e: حالت 0x160:
	    حالت 0x218: حالت 0x1e60: حالت 0x1e62: حالت 0x1e64:
	    حالت 0x1e66: حالت 0x1e68: حالت 0x2c7e: حالت 0xa7a8:
		    EMIT2('S') EMIT2(0x15a) EMIT2(0x15c) EMIT2(0x15e)
		    EMIT2(0x160) EMIT2(0x218) EMIT2(0x1e60) EMIT2(0x1e62)
		    EMIT2(0x1e64) EMIT2(0x1e66) EMIT2(0x1e68) EMIT2(0x2c7e)
		    EMIT2(0xa7a8)
		    بازگشت OK;

	    حالت 'T': حالت 0x162: حالت 0x164: حالت 0x166: حالت 0x1ac:
	    حالت 0x1ae: حالت 0x21a: حالت 0x23e: حالت 0x1e6a: حالت 0x1e6c:
	    حالت 0x1e6e: حالت 0x1e70:
		    EMIT2('T') EMIT2(0x162) EMIT2(0x164) EMIT2(0x166)
		    EMIT2(0x1ac) EMIT2(0x1ae) EMIT2(0x23e) EMIT2(0x21a)
		    EMIT2(0x1e6a) EMIT2(0x1e6c) EMIT2(0x1e6e) EMIT2(0x1e70)
		    بازگشت OK;

	    حالت 'U': حالت U_grave: حالت U_acute: حالت U_diaeresis:
	    حالت U_circumflex: حالت 0x168: حالت 0x16a: حالت 0x16c:
	    حالت 0x16e: حالت 0x170: حالت 0x172: حالت 0x1af:
	    حالت 0x1d3: حالت 0x1d5: حالت 0x1d7: حالت 0x1d9:
	    حالت 0x1db: حالت 0x214: حالت 0x216: حالت 0x244:
	    حالت 0x1e72: حالت 0x1e74: حالت 0x1e76: حالت 0x1e78:
	    حالت 0x1e7a: حالت 0x1ee4: حالت 0x1ee6: حالت 0x1ee8:
	    حالت 0x1eea: حالت 0x1eec: حالت 0x1eee: حالت 0x1ef0:
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
		    بازگشت OK;

	    حالت 'V': حالت 0x1b2: حالت 0x1e7c: حالت 0x1e7e:
		    EMIT2('V') EMIT2(0x1b2) EMIT2(0x1e7c) EMIT2(0x1e7e)
		    بازگشت OK;

	    حالت 'W': حالت 0x174: حالت 0x1e80: حالت 0x1e82: حالت 0x1e84:
	    حالت 0x1e86: حالت 0x1e88:
		    EMIT2('W') EMIT2(0x174) EMIT2(0x1e80) EMIT2(0x1e82)
		    EMIT2(0x1e84) EMIT2(0x1e86) EMIT2(0x1e88)
		    بازگشت OK;

	    حالت 'X': حالت 0x1e8a: حالت 0x1e8c:
		    EMIT2('X') EMIT2(0x1e8a) EMIT2(0x1e8c)
		    بازگشت OK;

	    حالت 'Y': حالت Y_acute: حالت 0x176: حالت 0x178:
	    حالت 0x1b3: حالت 0x232: حالت 0x24e: حالت 0x1e8e:
	    حالت 0x1ef2: حالت 0x1ef4: حالت 0x1ef6: حالت 0x1ef8:
		    EMIT2('Y') EMIT2(Y_acute)
		    EMIT2(0x176) EMIT2(0x178) EMIT2(0x1b3)
		    EMIT2(0x232) EMIT2(0x24e) EMIT2(0x1e8e)
		    EMIT2(0x1ef2) EMIT2(0x1ef4) EMIT2(0x1ef6)
		    EMIT2(0x1ef8)
		    بازگشت OK;

	    حالت 'Z': حالت 0x179: حالت 0x17b: حالت 0x17d:
	    حالت 0x1b5: حالت 0x1e90: حالت 0x1e92: حالت 0x1e94:
	    حالت 0x2c6b:
		    EMIT2('Z') EMIT2(0x179) EMIT2(0x17b) EMIT2(0x17d)
		    EMIT2(0x1b5) EMIT2(0x1e90) EMIT2(0x1e92)
		    EMIT2(0x1e94) EMIT2(0x2c6b)
		    بازگشت OK;

	    حالت 'a': حالت a_grave: حالت a_acute: حالت a_circumflex:
	    حالت a_virguilla: حالت a_diaeresis: حالت a_ring:
	    حالت 0x101: حالت 0x103: حالت 0x105: حالت 0x1ce:
	    حالت 0x1df: حالت 0x1e1: حالت 0x1fb: حالت 0x201:
	    حالت 0x203: حالت 0x227: حالت 0x1d8f: حالت 0x1e01:
	    حالت 0x1e9a: حالت 0x1ea1: حالت 0x1ea3: حالت 0x1ea5:
	    حالت 0x1ea7: حالت 0x1ea9: حالت 0x1eab: حالت 0x1ead:
	    حالت 0x1eaf: حالت 0x1eb1: حالت 0x1eb3: حالت 0x1eb5:
	    حالت 0x1eb7: حالت 0x2c65:
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
		    بازگشت OK;

	    حالت 'b': حالت 0x180: حالت 0x253: حالت 0x1d6c: حالت 0x1d80:
	    حالت 0x1e03: حالت 0x1e05: حالت 0x1e07:
		    EMIT2('b') EMIT2(0x180) EMIT2(0x253) EMIT2(0x1d6c)
		    EMIT2(0x1d80) EMIT2(0x1e03) EMIT2(0x1e05) EMIT2(0x1e07)
		    بازگشت OK;

	    حالت 'c': حالت c_cedilla: حالت 0x107: حالت 0x109: حالت 0x10b:
	    حالت 0x10d: حالت 0x188: حالت 0x23c: حالت 0x1e09: حالت 0xa793:
	    حالت 0xa794:
		    EMIT2('c') EMIT2(c_cedilla)
		    EMIT2(0x107) EMIT2(0x109) EMIT2(0x10b)
		    EMIT2(0x10d) EMIT2(0x188) EMIT2(0x23c)
		    EMIT2(0x1e09) EMIT2(0xa793) EMIT2(0xa794)
		    بازگشت OK;

	    حالت 'd': حالت 0x10f: حالت 0x111: حالت 0x257: حالت 0x1d6d:
	    حالت 0x1d81: حالت 0x1d91: حالت 0x1e0b: حالت 0x1e0d: حالت 0x1e0f:
	    حالت 0x1e11: حالت 0x1e13:
		    EMIT2('d') EMIT2(0x10f) EMIT2(0x111)
		    EMIT2(0x257) EMIT2(0x1d6d) EMIT2(0x1d81)
		    EMIT2(0x1d91) EMIT2(0x1e0b) EMIT2(0x1e0d)
		    EMIT2(0x1e0f) EMIT2(0x1e11) EMIT2(0x1e13)
		    بازگشت OK;

	    حالت 'e': حالت e_grave: حالت e_acute: حالت e_circumflex:
	    حالت e_diaeresis: حالت 0x113: حالت 0x115: حالت 0x117:
	    حالت 0x119: حالت 0x11b: حالت 0x205: حالت 0x207:
	    حالت 0x229: حالت 0x247: حالت 0x1d92: حالت 0x1e15:
	    حالت 0x1e17: حالت 0x1e19: حالت 0x1e1b: حالت 0x1e1d:
	    حالت 0x1eb9: حالت 0x1ebb: حالت 0x1ebd: حالت 0x1ebf:
	    حالت 0x1ec1: حالت 0x1ec3: حالت 0x1ec5: حالت 0x1ec7:
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
		    بازگشت OK;

	    حالت 'f': حالت 0x192: حالت 0x1d6e: حالت 0x1d82:
	    حالت 0x1e1f: حالت 0xa799:
		    EMIT2('f') EMIT2(0x192) EMIT2(0x1d6e) EMIT2(0x1d82)
		    EMIT2(0x1e1f) EMIT2(0xa799)
		    بازگشت OK;

	    حالت 'g': حالت 0x11d: حالت 0x11f: حالت 0x121: حالت 0x123:
	    حالت 0x1e5: حالت 0x1e7: حالت 0x1f5: حالت 0x260: حالت 0x1d83:
	    حالت 0x1e21: حالت 0xa7a1:
		    EMIT2('g') EMIT2(0x11d) EMIT2(0x11f) EMIT2(0x121)
		    EMIT2(0x123) EMIT2(0x1e5) EMIT2(0x1e7)
		    EMIT2(0x1f5) EMIT2(0x260) EMIT2(0x1d83)
		    EMIT2(0x1e21) EMIT2(0xa7a1)
		    بازگشت OK;

	    حالت 'h': حالت 0x125: حالت 0x127: حالت 0x21f: حالت 0x1e23:
	    حالت 0x1e25: حالت 0x1e27: حالت 0x1e29: حالت 0x1e2b:
	    حالت 0x1e96: حالت 0x2c68: حالت 0xa795:
		    EMIT2('h') EMIT2(0x125) EMIT2(0x127) EMIT2(0x21f)
		    EMIT2(0x1e23) EMIT2(0x1e25) EMIT2(0x1e27)
		    EMIT2(0x1e29) EMIT2(0x1e2b) EMIT2(0x1e96)
		    EMIT2(0x2c68) EMIT2(0xa795)
		    بازگشت OK;

	    حالت 'i': حالت i_grave: حالت i_acute: حالت i_circumflex:
	    حالت i_diaeresis: حالت 0x129: حالت 0x12b: حالت 0x12d:
	    حالت 0x12f: حالت 0x1d0: حالت 0x209: حالت 0x20b:
	    حالت 0x268: حالت 0x1d96: حالت 0x1e2d: حالت 0x1e2f:
	    حالت 0x1ec9: حالت 0x1ecb:
		    EMIT2('i') EMIT2(i_grave) EMIT2(i_acute)
		    EMIT2(i_circumflex) EMIT2(i_diaeresis)
		    EMIT2(0x129) EMIT2(0x12b) EMIT2(0x12d)
		    EMIT2(0x12f) EMIT2(0x1d0) EMIT2(0x209)
		    EMIT2(0x20b) EMIT2(0x268) EMIT2(0x1d96)
		    EMIT2(0x1e2d) EMIT2(0x1e2f) EMIT2(0x1ec9)
		    EMIT2(0x1ecb)
		    بازگشت OK;

	    حالت 'j': حالت 0x135: حالت 0x1f0: حالت 0x249:
		    EMIT2('j') EMIT2(0x135) EMIT2(0x1f0) EMIT2(0x249)
		    بازگشت OK;

	    حالت 'k': حالت 0x137: حالت 0x199: حالت 0x1e9: حالت 0x1d84:
	    حالت 0x1e31: حالت 0x1e33: حالت 0x1e35: حالت 0x2c6a: حالت 0xa741:
		    EMIT2('k') EMIT2(0x137) EMIT2(0x199) EMIT2(0x1e9)
		    EMIT2(0x1d84) EMIT2(0x1e31) EMIT2(0x1e33)
		    EMIT2(0x1e35) EMIT2(0x2c6a) EMIT2(0xa741)
		    بازگشت OK;

	    حالت 'l': حالت 0x13a: حالت 0x13c: حالت 0x13e: حالت 0x140:
	    حالت 0x142: حالت 0x19a: حالت 0x1e37: حالت 0x1e39: حالت 0x1e3b:
	    حالت 0x1e3d: حالت 0x2c61:
		    EMIT2('l') EMIT2(0x13a) EMIT2(0x13c)
		    EMIT2(0x13e) EMIT2(0x140) EMIT2(0x142)
		    EMIT2(0x19a) EMIT2(0x1e37) EMIT2(0x1e39)
		    EMIT2(0x1e3b) EMIT2(0x1e3d) EMIT2(0x2c61)
		    بازگشت OK;

	    حالت 'm': حالت 0x1d6f: حالت 0x1e3f: حالت 0x1e41: حالت 0x1e43:
		    EMIT2('m') EMIT2(0x1d6f) EMIT2(0x1e3f)
		    EMIT2(0x1e41) EMIT2(0x1e43)
		    بازگشت OK;

	    حالت 'n': حالت n_virguilla: حالت 0x144: حالت 0x146: حالت 0x148:
	    حالت 0x149: حالت 0x1f9: حالت 0x1d70: حالت 0x1d87: حالت 0x1e45:
	    حالت 0x1e47: حالت 0x1e49: حالت 0x1e4b: حالت 0xa7a5:
		    EMIT2('n') EMIT2(n_virguilla)
		    EMIT2(0x144) EMIT2(0x146) EMIT2(0x148)
		    EMIT2(0x149) EMIT2(0x1f9) EMIT2(0x1d70)
		    EMIT2(0x1d87) EMIT2(0x1e45) EMIT2(0x1e47)
		    EMIT2(0x1e49) EMIT2(0x1e4b) EMIT2(0xa7a5)
		    بازگشت OK;

	    حالت 'o': حالت o_grave: حالت o_acute: حالت o_circumflex:
	    حالت o_virguilla: حالت o_diaeresis: حالت o_slash:
	    حالت 0x14d: حالت 0x14f: حالت 0x151: حالت 0x1a1:
	    حالت 0x1d2: حالت 0x1eb: حالت 0x1ed: حالت 0x1ff:
	    حالت 0x20d: حالت 0x20f: حالت 0x22b: حالت 0x22d:
	    حالت 0x22f: حالت 0x231: حالت 0x275: حالت 0x1e4d:
	    حالت 0x1e4f: حالت 0x1e51: حالت 0x1e53: حالت 0x1ecd:
	    حالت 0x1ecf: حالت 0x1ed1: حالت 0x1ed3: حالت 0x1ed5:
	    حالت 0x1ed7: حالت 0x1ed9: حالت 0x1edb: حالت 0x1edd:
	    حالت 0x1edf: حالت 0x1ee1: حالت 0x1ee3:
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
		    بازگشت OK;

	    حالت 'p': حالت 0x1a5: حالت 0x1d71: حالت 0x1d7d: حالت 0x1d88:
	    حالت 0x1e55: حالت 0x1e57:
		    EMIT2('p') EMIT2(0x1a5) EMIT2(0x1d71) EMIT2(0x1d7d)
		    EMIT2(0x1d88) EMIT2(0x1e55) EMIT2(0x1e57)
		    بازگشت OK;

	    حالت 'q': حالت 0x24b: حالت 0x2a0:
		    EMIT2('q') EMIT2(0x24b) EMIT2(0x2a0)
		    بازگشت OK;

	    حالت 'r': حالت 0x155: حالت 0x157: حالت 0x159: حالت 0x211:
	    حالت 0x213: حالت 0x24d: حالت 0x27d: حالت 0x1d72: حالت 0x1d73:
	    حالت 0x1d89: حالت 0x1e59: حالت 0x1e5b: حالت 0x1e5d: حالت 0x1e5f:
	    حالت 0xa7a7:
		    EMIT2('r') EMIT2(0x155) EMIT2(0x157) EMIT2(0x159)
		    EMIT2(0x211) EMIT2(0x213) EMIT2(0x24d) EMIT2(0x27d)
		    EMIT2(0x1d72) EMIT2(0x1d73) EMIT2(0x1d89) EMIT2(0x1e59)
		    EMIT2(0x1e5b) EMIT2(0x1e5d) EMIT2(0x1e5f) EMIT2(0xa7a7)
		    بازگشت OK;

	    حالت 's': حالت 0x15b: حالت 0x15d: حالت 0x15f: حالت 0x161:
	    حالت 0x219: حالت 0x23f: حالت 0x1d74: حالت 0x1d8a: حالت 0x1e61:
	    حالت 0x1e63: حالت 0x1e65: حالت 0x1e67: حالت 0x1e69: حالت 0xa7a9:
		    EMIT2('s') EMIT2(0x15b) EMIT2(0x15d) EMIT2(0x15f)
		    EMIT2(0x161) EMIT2(0x219) EMIT2(0x23f) EMIT2(0x1d74)
		    EMIT2(0x1d8a) EMIT2(0x1e61) EMIT2(0x1e63) EMIT2(0x1e65)
		    EMIT2(0x1e67) EMIT2(0x1e69) EMIT2(0xa7a9)
		    بازگشت OK;

	    حالت 't': حالت 0x163: حالت 0x165: حالت 0x167: حالت 0x1ab:
	    حالت 0x1ad: حالت 0x21b: حالت 0x288: حالت 0x1d75: حالت 0x1e6b:
	    حالت 0x1e6d: حالت 0x1e6f: حالت 0x1e71: حالت 0x1e97: حالت 0x2c66:
		    EMIT2('t') EMIT2(0x163) EMIT2(0x165) EMIT2(0x167)
		    EMIT2(0x1ab) EMIT2(0x1ad) EMIT2(0x21b) EMIT2(0x288)
		    EMIT2(0x1d75) EMIT2(0x1e6b) EMIT2(0x1e6d) EMIT2(0x1e6f)
		    EMIT2(0x1e71) EMIT2(0x1e97) EMIT2(0x2c66)
		    بازگشت OK;

	    حالت 'u': حالت u_grave: حالت u_acute: حالت u_circumflex:
	    حالت u_diaeresis: حالت 0x169: حالت 0x16b: حالت 0x16d:
	    حالت 0x16f: حالت 0x171: حالت 0x173: حالت 0x1b0: حالت 0x1d4:
	    حالت 0x1d6: حالت 0x1d8: حالت 0x1da: حالت 0x1dc: حالت 0x215:
	    حالت 0x217: حالت 0x289: حالت 0x1d7e: حالت 0x1d99: حالت 0x1e73:
	    حالت 0x1e75: حالت 0x1e77: حالت 0x1e79: حالت 0x1e7b:
	    حالت 0x1ee5: حالت 0x1ee7: حالت 0x1ee9: حالت 0x1eeb:
	    حالت 0x1eed: حالت 0x1eef: حالت 0x1ef1:
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
		    بازگشت OK;

	    حالت 'v': حالت 0x28b: حالت 0x1d8c: حالت 0x1e7d: حالت 0x1e7f:
		    EMIT2('v') EMIT2(0x28b) EMIT2(0x1d8c) EMIT2(0x1e7d)
		    EMIT2(0x1e7f)
		    بازگشت OK;

	    حالت 'w': حالت 0x175: حالت 0x1e81: حالت 0x1e83: حالت 0x1e85:
	    حالت 0x1e87: حالت 0x1e89: حالت 0x1e98:
		    EMIT2('w') EMIT2(0x175) EMIT2(0x1e81) EMIT2(0x1e83)
		    EMIT2(0x1e85) EMIT2(0x1e87) EMIT2(0x1e89) EMIT2(0x1e98)
		    بازگشت OK;

	    حالت 'x': حالت 0x1e8b: حالت 0x1e8d:
		    EMIT2('x') EMIT2(0x1e8b) EMIT2(0x1e8d)
		    بازگشت OK;

	    حالت 'y': حالت y_acute: حالت y_diaeresis: حالت 0x177:
	    حالت 0x1b4: حالت 0x233: حالت 0x24f: حالت 0x1e8f:
	    حالت 0x1e99: حالت 0x1ef3: حالت 0x1ef5: حالت 0x1ef7:
	    حالت 0x1ef9:
		    EMIT2('y') EMIT2(y_acute) EMIT2(y_diaeresis)
		    EMIT2(0x177) EMIT2(0x1b4) EMIT2(0x233) EMIT2(0x24f)
		    EMIT2(0x1e8f) EMIT2(0x1e99) EMIT2(0x1ef3)
		    EMIT2(0x1ef5) EMIT2(0x1ef7) EMIT2(0x1ef9)
		    بازگشت OK;

	    حالت 'z': حالت 0x17a: حالت 0x17c: حالت 0x17e: حالت 0x1b6:
	    حالت 0x1d76: حالت 0x1d8e: حالت 0x1e91: حالت 0x1e93:
	    حالت 0x1e95: حالت 0x2c6c:
		    EMIT2('z') EMIT2(0x17a) EMIT2(0x17c) EMIT2(0x17e)
		    EMIT2(0x1b6) EMIT2(0x1d76) EMIT2(0x1d8e) EMIT2(0x1e91)
		    EMIT2(0x1e93) EMIT2(0x1e95) EMIT2(0x2c6c)
		    بازگشت OK;

	    // default: character itself
	}
    }

    EMIT2(c);
    بازگشت OK;
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
    ایستا صحیح
nfa_regatom(تهی)
{
    صحیح		c;
    صحیح		charclass;
    صحیح		equiclass;
    صحیح		collclass;
    صحیح		got_coll_char;
    char_u	*p;
    char_u	*endp;
    char_u	*old_regparse = regparse;
    صحیح		extra = 0;
    صحیح		emit_range;
    صحیح		negated;
    صحیح		result;
    صحیح		startc = -1;
    صحیح		save_prev_at_start = prev_at_start;

    c = getchr();
    انتخاب (c)
    {
	حالت NUL:
	    EMSG_RET_FAIL(_(e_nfa_regexp_end_encountered_prematurely));

	حالت Magic('^'):
	    EMIT(NFA_BOL);
	    بشکن;

	حالت Magic('$'):
	    EMIT(NFA_EOL);
#اگر defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#endif
	    بشکن;

	حالت Magic('<'):
	    EMIT(NFA_BOW);
	    بشکن;

	حالت Magic('>'):
	    EMIT(NFA_EOW);
	    بشکن;

	حالت Magic('_'):
	    c = no_Magic(getchr());
	    اگر (c == NUL)
		EMSG_RET_FAIL(_(e_nfa_regexp_end_encountered_prematurely));

	    اگر (c == '^')	// "\_^" is start-of-line
	    {
		EMIT(NFA_BOL);
		بشکن;
	    }
	    اگر (c == '$')	// "\_$" is end-of-line
	    {
		EMIT(NFA_EOL);
#اگر defined(FEAT_SYN_HL)
		had_eol = TRUE;
#endif
		بشکن;
	    }

	    extra = NFA_ADD_NL;

	    // "\_[" is collection plus newline
	    اگر (c == '[')
		برو collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	/*
	 * Character classes.
	 */
	حالت Magic('.'):
	حالت Magic('i'):
	حالت Magic('I'):
	حالت Magic('k'):
	حالت Magic('K'):
	حالت Magic('f'):
	حالت Magic('F'):
	حالت Magic('p'):
	حالت Magic('P'):
	حالت Magic('s'):
	حالت Magic('S'):
	حالت Magic('d'):
	حالت Magic('D'):
	حالت Magic('x'):
	حالت Magic('X'):
	حالت Magic('o'):
	حالت Magic('O'):
	حالت Magic('w'):
	حالت Magic('W'):
	حالت Magic('h'):
	حالت Magic('H'):
	حالت Magic('a'):
	حالت Magic('A'):
	حالت Magic('l'):
	حالت Magic('L'):
	حالت Magic('u'):
	حالت Magic('U'):
	    p = vim_strchr(classchars, no_Magic(c));
	    اگر (p == پوچ)
	    {
		اگر (extra == NFA_ADD_NL)
		{
		    semsg(_(e_nfa_regexp_invalid_character_class_nr), c);
		    rc_did_emsg = TRUE;
		    بازگشت FAIL;
		}
		siemsg("Unknown character class char: %d", c);
		بازگشت FAIL;
	    }

	    // When '.' is followed by a composing char ignore the dot, so that
	    // the composing char is matched here.
	    اگر (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	    {
		old_regparse = regparse;
		c = getchr();
		برو nfa_do_multibyte;
	    }
	    EMIT(nfa_classcodes[p - classchars]);
	    اگر (extra == NFA_ADD_NL)
	    {
		EMIT(NFA_NEWL);
		EMIT(NFA_OR);
		regflags |= RF_HASNL;
	    }
	    بشکن;

	حالت Magic('n'):
	    اگر (reg_string)
		// In a string "\n" matches a newline character.
		EMIT(NL);
	    وگرنه
	    {
		// In buffer text "\n" matches the end of a line.
		EMIT(NFA_NEWL);
		regflags |= RF_HASNL;
	    }
	    بشکن;

	حالت Magic('('):
	    اگر (nfa_reg(REG_PAREN) == FAIL)
		بازگشت FAIL;	    // cascaded error
	    بشکن;

	حالت Magic('|'):
	حالت Magic('&'):
	حالت Magic(')'):
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    بازگشت FAIL;

	حالت Magic('='):
	حالت Magic('?'):
	حالت Magic('+'):
	حالت Magic('@'):
	حالت Magic('*'):
	حالت Magic('{'):
	    // these should follow an atom, not form an atom
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    بازگشت FAIL;

	حالت Magic('~'):
	    {
		char_u	    *lp;

		// Previous substitute pattern.
		// Generated as "\%(pattern\)".
		اگر (reg_prev_sub == پوچ)
		{
		    emsg(_(e_no_previous_substitute_regular_expression));
		    بازگشت FAIL;
		}
		برای (lp = reg_prev_sub; *lp != NUL; MB_CPTR_ADV(lp))
		{
		    EMIT(PTR2CHAR(lp));
		    اگر (lp != reg_prev_sub)
			EMIT(NFA_CONCAT);
		}
		EMIT(NFA_NOPEN);
		بشکن;
	    }

	حالت Magic('1'):
	حالت Magic('2'):
	حالت Magic('3'):
	حالت Magic('4'):
	حالت Magic('5'):
	حالت Magic('6'):
	حالت Magic('7'):
	حالت Magic('8'):
	حالت Magic('9'):
	    {
		صحیح refnum = no_Magic(c) - '1';

		اگر (!seen_endbrace(refnum + 1))
		    بازگشت FAIL;
		EMIT(NFA_BACKREF1 + refnum);
		rex.nfa_has_backref = TRUE;
	    }
	    بشکن;

	حالت Magic('z'):
	    c = no_Magic(getchr());
	    انتخاب (c)
	    {
		حالت 's':
		    EMIT(NFA_ZSTART);
		    اگر (re_mult_next("\\zs") == FAIL)
			بازگشت FAIL;
		    بشکن;
		حالت 'e':
		    EMIT(NFA_ZEND);
		    rex.nfa_has_zend = TRUE;
		    اگر (re_mult_next("\\ze") == FAIL)
			بازگشت FAIL;
		    بشکن;
#ifdef FEAT_SYN_HL
		حالت '1':
		حالت '2':
		حالت '3':
		حالت '4':
		حالت '5':
		حالت '6':
		حالت '7':
		حالت '8':
		حالت '9':
		    // \z1...\z9
		    اگر ((reg_do_extmatch & REX_USE) == 0)
			EMSG_RET_FAIL(_(e_z1_z9_not_allowed_here));
		    EMIT(NFA_ZREF1 + (no_Magic(c) - '1'));
		    // No need to set rex.nfa_has_backref, the sub-matches don't
		    // change when \z1 .. \z9 matches or not.
		    re_has_z = REX_USE;
		    بشکن;
		حالت '(':
		    // \z(
		    اگر ((reg_do_extmatch & REX_SET) == 0)
			EMSG_RET_FAIL(_(e_z_not_allowed_here));
		    اگر (nfa_reg(REG_ZPAREN) == FAIL)
			بازگشت FAIL;	    // cascaded error
		    re_has_z = REX_SET;
		    بشکن;
#endif
		پیشفرض:
		    semsg(_(e_nfa_regexp_unknown_operator_z_chr), no_Magic(c));
		    بازگشت FAIL;
	    }
	    بشکن;

	حالت Magic('%'):
	    c = no_Magic(getchr());
	    انتخاب (c)
	    {
		// () without a back reference
		حالت '(':
		    اگر (nfa_reg(REG_NPAREN) == FAIL)
			بازگشت FAIL;
		    EMIT(NFA_NOPEN);
		    بشکن;

		حالت 'd':   // %d123 decimal
		حالت 'o':   // %o123 octal
		حالت 'x':   // %xab hex 2
		حالت 'u':   // %uabcd hex 4
		حالت 'U':   // %U1234abcd hex 8
		    {
			vimlong_T nr;

			انتخاب (c)
			{
			    حالت 'd': nr = getdecchrs(); بشکن;
			    حالت 'o': nr = getoctchrs(); بشکن;
			    حالت 'x': nr = gethexchrs(2); بشکن;
			    حالت 'u': nr = gethexchrs(4); بشکن;
			    حالت 'U': nr = gethexchrs(8); بشکن;
			    پیشفرض:  nr = -1; بشکن;
			}

			اگر (nr < 0 || nr > INT_MAX)
			    EMSG2_RET_FAIL(_(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			// A NUL is stored in the text as NL
			// TODO: what if a composing character follows?
			EMIT(nr == 0 ? 0x0a : (long)nr);
		    }
		    بشکن;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		حالت '^':
		    EMIT(NFA_BOF);
		    بشکن;

		حالت '$':
		    EMIT(NFA_EOF);
		    بشکن;

		حالت '#':
		    اگر (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_start_of_pattern),
								  regparse[1]);
			بازگشت FAIL;
		    }
		    EMIT(NFA_CURSOR);
		    بشکن;

		حالت 'V':
		    EMIT(NFA_VISUAL);
		    بشکن;

		حالت 'C':
		    EMIT(NFA_ANY_COMPOSING);
		    بشکن;

		حالت '[':
		    {
			صحیح	    n;

			// \%[abc]
			برای (n = 0; (c = peekchr()) != ']'; ++n)
			{
			    اگر (c == NUL)
				EMSG2_RET_FAIL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
			    // recursive call!
			    اگر (nfa_regatom() == FAIL)
				بازگشت FAIL;
			}
			getchr();  // get the ]
			اگر (n == 0)
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
			بشکن;
		    }

		پیشفرض:
		    {
			long_u	n = 0;
			صحیح	cmp = c;
			صحیح	cur = FALSE;
			صحیح	got_digit = FALSE;

			اگر (c == '<' || c == '>')
			    c = getchr();
			اگر (no_Magic(c) == '.')
			{
			    cur = TRUE;
			    c = getchr();
			}
			وقتی (VIM_ISDIGIT(c))
			{
			    long_u tmp;

			    اگر (cur)
			    {
				semsg(_(e_regexp_عدد_after_dot_pos_جستجو_chr),
								  no_Magic(c));
				بازگشت FAIL;
			    }
			    tmp = n * 10 + (c - '0');

			    اگر (tmp < n)
			    {
				// overflow.
				emsg(_(e_percent_مقدار_too_large));
				بازگشت FAIL;
			    }
			    n = tmp;
			    c = getchr();
			    got_digit = TRUE;
			}
			اگر (c == 'l' || c == 'c' || c == 'v')
			{
			    long_u limit = INT_MAX;

			    اگر (!cur && !got_digit)
			    {
				semsg(_(e_nfa_regexp_missing_مقدار_in_chr),
								  no_Magic(c));
				بازگشت FAIL;
			    }
			    اگر (c == 'l')
			    {
				اگر (cur)
				    n = curwin->w_cursor.lnum;
				// \%{n}l  \%{n}<l  \%{n}>l
				EMIT(cmp == '<' ? NFA_LNUM_LT :
				     cmp == '>' ? NFA_LNUM_GT : NFA_LNUM);
				اگر (save_prev_at_start)
				    at_start = TRUE;
			    }
			    وگرنه اگر (c == 'c')
			    {
				اگر (cur)
				{
				    n = curwin->w_cursor.col;
				    n++;
				}
				// \%{n}c  \%{n}<c  \%{n}>c
				EMIT(cmp == '<' ? NFA_COL_LT :
				     cmp == '>' ? NFA_COL_GT : NFA_COL);
			    }
			    وگرنه
			    {
				اگر (cur)
				{
				    colnr_T vcol = 0;

				    getvvcol(curwin, &curwin->w_cursor,
							 پوچ, پوچ, &vcol, 0);
				    n = ++vcol;
				}
				// \%{n}v  \%{n}<v  \%{n}>v
				EMIT(cmp == '<' ? NFA_VCOL_LT :
				     cmp == '>' ? NFA_VCOL_GT : NFA_VCOL);
				limit = INT_MAX / MB_MAXBYTES;
			    }
			    اگر (n >= limit)
			    {
				emsg(_(e_percent_مقدار_too_large));
				بازگشت FAIL;
			    }
			    EMIT((صحیح)n);
			    بشکن;
			}
			وگرنه اگر (no_Magic(c) == '\'' && n == 0)
			{
			    // \%'m  \%<'m  \%>'m
			    EMIT(cmp == '<' ? NFA_MARK_LT :
				 cmp == '>' ? NFA_MARK_GT : NFA_MARK);
			    EMIT(getchr());
			    بشکن;
			}
		    }
		    semsg(_(e_nfa_regexp_unknown_operator_percent_chr),
								  no_Magic(c));
		    بازگشت FAIL;
	    }
	    بشکن;

	حالت Magic('['):
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
	    اگر (*endp == ']')
	    {
		صحیح plen;
		bool range_endpoint;
		/*
		 * Try to reverse engineer character classes. For example,
		 * recognize that [0-9] stands for \d and [A-Za-z_] for \h,
		 * and perform the necessary substitutions in the NFA.
		 */
		result = nfa_recognize_char_class(regparse, endp,
							 extra == NFA_ADD_NL);
		اگر (result != FAIL)
		{
		    اگر (result >= NFA_FIRST_NL && result <= NFA_LAST_NL)
		    {
			EMIT(result - NFA_ADD_NL);
			EMIT(NFA_NEWL);
			EMIT(NFA_OR);
		    }
		    وگرنه
			EMIT(result);
		    regparse = endp;
		    MB_PTR_ADV(regparse);
		    بازگشت OK;
		}
		/*
		 * Failed to recognize a character class. Use the simple
		 * version that turns [abc] into 'a' OR 'b' OR 'c'
		 */
		startc = -1;
		negated = FALSE;
		اگر (*regparse == '^')			// negated range
		{
		    negated = TRUE;
		    MB_PTR_ADV(regparse);
		    EMIT(NFA_START_NEG_COLL);
		}
		وگرنه
		    EMIT(NFA_START_COLL);
		اگر (*regparse == '-')
		{
		    startc = '-';
		    EMIT(startc);
		    EMIT(NFA_CONCAT);
		    MB_PTR_ADV(regparse);
		}
		// Emit the OR branches for each character in the []
		emit_range = FALSE;
		وقتی (regparse < endp)
		{
		    صحیح	    oldstartc = startc;
		    range_endpoint = false;

		    startc = -1;
		    got_coll_char = FALSE;
		    اگر (*regparse == '[')
		    {
			// Check for [: :], [= =], [. .]
			equiclass = collclass = 0;
			charclass = get_char_class(&regparse);
			اگر (charclass == CLASS_NONE)
			{
			    equiclass = get_equi_class(&regparse);
			    اگر (equiclass == 0)
				collclass = get_coll_element(&regparse);
			}

			// Character class like [:alpha:]
			اگر (charclass != CLASS_NONE)
			{
			    انتخاب (charclass)
			    {
				حالت CLASS_ALNUM:
				    EMIT(NFA_CLASS_ALNUM);
				    بشکن;
				حالت CLASS_ALPHA:
				    EMIT(NFA_CLASS_ALPHA);
				    بشکن;
				حالت CLASS_BLANK:
				    EMIT(NFA_CLASS_BLANK);
				    بشکن;
				حالت CLASS_CNTRL:
				    EMIT(NFA_CLASS_CNTRL);
				    بشکن;
				حالت CLASS_DIGIT:
				    EMIT(NFA_CLASS_DIGIT);
				    بشکن;
				حالت CLASS_GRAPH:
				    EMIT(NFA_CLASS_GRAPH);
				    بشکن;
				حالت CLASS_LOWER:
				    wants_nfa = TRUE;
				    EMIT(NFA_CLASS_LOWER);
				    بشکن;
				حالت CLASS_PRINT:
				    EMIT(NFA_CLASS_PRINT);
				    بشکن;
				حالت CLASS_PUNCT:
				    EMIT(NFA_CLASS_PUNCT);
				    بشکن;
				حالت CLASS_SPACE:
				    EMIT(NFA_CLASS_SPACE);
				    بشکن;
				حالت CLASS_UPPER:
				    wants_nfa = TRUE;
				    EMIT(NFA_CLASS_UPPER);
				    بشکن;
				حالت CLASS_XDIGIT:
				    EMIT(NFA_CLASS_XDIGIT);
				    بشکن;
				حالت CLASS_TAB:
				    EMIT(NFA_CLASS_TAB);
				    بشکن;
				حالت CLASS_RETURN:
				    EMIT(NFA_CLASS_RETURN);
				    بشکن;
				حالت CLASS_BACKSPACE:
				    EMIT(NFA_CLASS_BACKSPACE);
				    بشکن;
				حالت CLASS_ESCAPE:
				    EMIT(NFA_CLASS_ESCAPE);
				    بشکن;
				حالت CLASS_IDENT:
				    EMIT(NFA_CLASS_IDENT);
				    بشکن;
				حالت CLASS_KEYWORD:
				    EMIT(NFA_CLASS_KEYWORD);
				    بشکن;
				حالت CLASS_FNAME:
				    EMIT(NFA_CLASS_FNAME);
				    بشکن;
			    }
			    EMIT(NFA_CONCAT);
			    ادامه;
			}
			// Try equivalence class [=a=] and the like
			اگر (equiclass != 0)
			{
			    result = nfa_emit_equi_class(equiclass);
			    اگر (result == FAIL)
			    {
				// should never happen
				EMSG_RET_FAIL(_(e_error_building_nfa_with_equivalence_class));
			    }
			    ادامه;
			}
			// Try collating class like [. .]
			اگر (collclass != 0)
			{
			    startc = collclass;	 // allow [.a.]-x as a range
			    // Will emit the proper atom at the end of the
			    // while loop.
			}
		    }
		    // Try a range like 'a-x' or '\t-z'. Also allows '-' as a
		    // start character.
		    اگر (*regparse == '-' && oldstartc != -1)
		    {
			emit_range = TRUE;
			startc = oldstartc;
			MB_PTR_ADV(regparse);
			ادامه;	    // reading the end of the range
		    }

		    // Now handle simple and escaped characters.
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    اگر (*regparse == '\\'
			    && !reg_cpo_bsl
			    && regparse + 1 <= endp
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != پوچ
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR, regparse[1])
								      != پوچ)
			    )
			)
		    {
			MB_PTR_ADV(regparse);

			اگر (*regparse == 'n')
			    startc = (reg_string || emit_range
					|| regparse[1] == '-') ? NL : NFA_NEWL;
			وگرنه اگر (*regparse == 'd'
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
				اگر (startc == INT_MAX)
				    EMSG_RET_FAIL(_(e_unicode_val_too_large));
				got_coll_char = TRUE;
				MB_PTR_BACK(old_regparse, regparse);
			    }
			    وگرنه
			    {
				// \r,\t,\e,\b
				startc = backslash_trans(*regparse);
			    }
		    }

		    // Normal printable char
		    اگر (startc == -1)
			startc = PTR2CHAR(regparse);

		    // Previous char was '-', so this char is end of range.
		    اگر (emit_range)
		    {
			صحیح	endc = startc;
			range_endpoint = true;

			startc = oldstartc;
			اگر (startc > endc)
			    EMSG_RET_FAIL(_(e_reverse_range_in_character_class));

			اگر (endc > startc + 2)
			{
			    // Emit a range instead of the sequence of
			    // individual characters.
			    اگر (startc == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    وگرنه
				--post_ptr; // remove NFA_CONCAT
			    EMIT(endc);
			    EMIT(NFA_RANGE);
			    EMIT(NFA_CONCAT);
			}
			وگرنه اگر (has_mbyte && ((*mb_char2len)(startc) > 1
				    || (*mb_char2len)(endc) > 1))
			{
			    // Emit the characters in the range.
			    // "startc" was already emitted, so skip it.
			    //
			    برای (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			وگرنه
			{
			    // Emit the range. "startc" was already emitted, so
			    // skip it.
			    برای (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			emit_range = FALSE;
			startc = -1;
		    }
		    وگرنه
		    {
			// This char (startc) is not part of a range. Just
			// emit it.
			// Normally, simply emit startc. But if we get char
			// code=0 from a collating char, then replace it with
			// 0x0a.
			// This is needed to completely mimic the behaviour of
			// the backtracking engine.
			اگر (startc == NFA_NEWL)
			{
			    // Line break can't be matched as part of the
			    // collection, add an OR below. But not for negated
			    // range.
			    اگر (!negated)
				extra = NFA_ADD_NL;
			}
			وگرنه
			{
			    اگر (got_coll_char == TRUE && startc == 0)
			    {
				EMIT(0x0a);
				EMIT(NFA_CONCAT);
			    }
			    وگرنه
			    {
				EMIT(startc);
				اگر (!(enc_utf8 && (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse)))))
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
		    اگر (!range_endpoint && enc_utf8 &&
			    (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse))))
		    {
			صحیح i = utf_ptr2len(regparse);

			c = utf_ptr2char(regparse + i);

			// Add composing characters
			برای (;;)
			{
			    اگر (c == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    وگرنه
				EMIT(c);
			    EMIT(NFA_CONCAT);
			    اگر ((i += utf_char2len(c)) >= plen)
				بشکن;
			    c = utf_ptr2char(regparse + i);
			}
			EMIT(NFA_COMPOSING);
			EMIT(NFA_CONCAT);
		    }
		    MB_PTR_ADV(regparse);
		} // while (p < endp)

		MB_PTR_BACK(old_regparse, regparse);
		اگر (*regparse == '-')	    // if last, '-' is just a char
		{
		    EMIT('-');
		    EMIT(NFA_CONCAT);
		}

		// skip the trailing ]
		regparse = endp;
		MB_PTR_ADV(regparse);

		// Mark end of the collection.
		اگر (negated == TRUE)
		    EMIT(NFA_END_NEG_COLL);
		وگرنه
		    EMIT(NFA_END_COLL);

		// \_[] also matches \n but it's not negated
		اگر (extra == NFA_ADD_NL)
		{
		    EMIT(reg_string ? NL : NFA_NEWL);
		    EMIT(NFA_OR);
		}

		بازگشت OK;
	    } // if exists closing ]

	    اگر (reg_strict)
		EMSG_RET_FAIL(_(e_missing_rsb_after_str_lsb));
	    // FALLTHROUGH

	پیشفرض:
	    {
		صحیح	plen;

nfa_do_multibyte:
		// plen is length of current char with composing chars
		اگر (enc_utf8 && ((*mb_char2len)(c)
			    != (plen = utfc_ptr2len(old_regparse))
						       || utf_iscomposing(c)))
		{
		    صحیح i = 0;

		    // A base character plus composing characters, or just one
		    // or more composing characters.
		    // This requires creating a separate atom as if enclosing
		    // the characters in (), where NFA_COMPOSING is the ( and
		    // NFA_END_COMPOSING is the ). Note that right now we are
		    // building the postfix form, not the NFA itself;
		    // a composing char could be: a, b, c, NFA_COMPOSING
		    // where 'b' and 'c' are chars with codes > 256.
		    برای (;;)
		    {
			EMIT(c);
			اگر (i > 0)
			    EMIT(NFA_CONCAT);
			اگر ((i += utf_char2len(c)) >= plen)
			    بشکن;
			c = utf_ptr2char(old_regparse + i);
		    }
		    EMIT(NFA_COMPOSING);
		    regparse = old_regparse + plen;
		}
		وگرنه
		{
		    c = no_Magic(c);
		    EMIT(c);
		}
		بازگشت OK;
	    }
    }

    بازگشت OK;
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
    ایستا صحیح
nfa_regpiece(تهی)
{
    صحیح		i;
    صحیح		op;
    صحیح		ret;
    long	minval, maxval;
    صحیح		greedy = TRUE;      // Braces are prefixed with '-' ?
    parse_state_T old_state;
    parse_state_T new_state;
    long	c2;
    صحیح		old_post_pos;
    صحیح		my_post_start;
    صحیح		quest;

    // Save the current parse state, so that we can use it if <atom>{m,n} is
    // next.
    save_parse_state(&old_state);

    // store current pos in the postfix form, for \{m,n} involving 0s
    my_post_start = (صحیح)(post_ptr - post_start);

    ret = nfa_regatom();
    اگر (ret == FAIL)
	بازگشت FAIL;	    // cascaded error

    op = peekchr();
    اگر (re_multi_type(op) == NOT_MULTI)
	بازگشت OK;

    skipchr();
    انتخاب (op)
    {
	حالت Magic('*'):
	    EMIT(NFA_STAR);
	    بشکن;

	حالت Magic('+'):
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
	    اگر (nfa_regatom() == FAIL)
		بازگشت FAIL;
	    EMIT(NFA_STAR);
	    EMIT(NFA_CONCAT);
	    skipchr();		// skip the \+
	    بشکن;

	حالت Magic('@'):
	    c2 = (long)getdecchrs();
	    op = no_Magic(getchr());
	    i = 0;
	    انتخاب(op)
	    {
		حالت '=':
		    // \@=
		    i = NFA_PREV_ATOM_NO_WIDTH;
		    بشکن;
		حالت '!':
		    // \@!
		    i = NFA_PREV_ATOM_NO_WIDTH_NEG;
		    بشکن;
		حالت '<':
		    op = no_Magic(getchr());
		    اگر (op == '=')
			// \@<=
			i = NFA_PREV_ATOM_JUST_BEFORE;
		    وگرنه اگر (op == '!')
			// \@<!
			i = NFA_PREV_ATOM_JUST_BEFORE_NEG;
		    بشکن;
		حالت '>':
		    // \@>
		    i = NFA_PREV_ATOM_LIKE_PATTERN;
		    بشکن;
	    }
	    اگر (i == 0)
	    {
		semsg(_(e_nfa_regexp_unknown_operator_at_chr), op);
		بازگشت FAIL;
	    }
	    EMIT(i);
	    اگر (i == NFA_PREV_ATOM_JUST_BEFORE
					|| i == NFA_PREV_ATOM_JUST_BEFORE_NEG)
		EMIT(c2);
	    بشکن;

	حالت Magic('?'):
	حالت Magic('='):
	    EMIT(NFA_QUEST);
	    بشکن;

	حالت Magic('{'):
	    // a{2,5} will expand to 'aaa?a?a?'
	    // a{-1,3} will expand to 'aa??a??', where ?? is the nongreedy
	    // version of '?'
	    // \v(ab){2,3} will expand to '(ab)(ab)(ab)?', where all the
	    // parenthesis have the same id

	    greedy = TRUE;
	    c2 = peekchr();
	    اگر (c2 == '-' || c2 == Magic('-'))
	    {
		skipchr();
		greedy = FALSE;
	    }
	    اگر (!read_limits(&minval, &maxval))
		EMSG_RET_FAIL(_(e_nfa_regexp_error_reading_repetition_limits));

	    //  <atom>{0,inf}, <atom>{0,} and <atom>{}  are equivalent to
	    //  <atom>*
	    اگر (minval == 0 && maxval == MAX_LIMIT)
	    {
		اگر (greedy)		// { { (match the braces)
		    // \{}, \{0,}
		    EMIT(NFA_STAR);
		وگرنه			// { { (match the braces)
		    // \{-}, \{-0,}
		    EMIT(NFA_STAR_NONGREEDY);
		بشکن;
	    }

	    // Special case: x{0} or x{-0}
	    اگر (maxval == 0)
	    {
		// Ignore result of previous call to nfa_regatom()
		post_ptr = post_start + my_post_start;
		// NFA_EMPTY is 0-length and works everywhere
		EMIT(NFA_EMPTY);
		بازگشت OK;
	    }

	    // The engine is very inefficient (uses too many states) when the
	    // maximum is much larger than the minimum and when the maximum is
	    // large.  However, when maxval is MAX_LIMIT, it is okay, as this
	    // will emit NFA_STAR.
	    // Bail out if we can use the other engine, but only, when the
	    // pattern does not need the NFA engine like (e.g. [[:upper:]]\{2,\}
	    // does not work with characters > 8 bit with the BT engine)
	    اگر ((nfa_re_flags & RE_AUTO)
				   && (maxval > 500 || maxval > minval + 200)
				   && (maxval != MAX_LIMIT && minval < 200)
				   && !wants_nfa)
		بازگشت FAIL;

	    // Ignore previous call to nfa_regatom()
	    post_ptr = post_start + my_post_start;
	    // Save parse state after the repeated atom and the \{}
	    save_parse_state(&new_state);

	    quest = (greedy == TRUE? NFA_QUEST : NFA_QUEST_NONGREEDY);
	    برای (i = 0; i < maxval; i++)
	    {
		// Goto beginning of the repeated atom
		restore_parse_state(&old_state);
		old_post_pos = (صحیح)(post_ptr - post_start);
		اگر (nfa_regatom() == FAIL)
		    بازگشت FAIL;
		// after "minval" times, atoms are optional
		اگر (i + 1 > minval)
		{
		    اگر (maxval == MAX_LIMIT)
		    {
			اگر (greedy)
			    EMIT(NFA_STAR);
			وگرنه
			    EMIT(NFA_STAR_NONGREEDY);
		    }
		    وگرنه
			EMIT(quest);
		}
		اگر (old_post_pos != my_post_start)
		    EMIT(NFA_CONCAT);
		اگر (i + 1 > minval && maxval == MAX_LIMIT)
		    بشکن;
	    }

	    // Go to just after the repeated atom and the \{}
	    restore_parse_state(&new_state);
	    curchr = -1;

	    بشکن;


	پیشفرض:
	    بشکن;
    }	// end switch

    اگر (re_multi_type(peekchr()) != NOT_MULTI)
	// Can't have a multi follow a multi.
	EMSG_RET_FAIL(_(e_nfa_regexp_cant_have_multi_follow_multi));

    بازگشت OK;
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
    ایستا صحیح
nfa_regconcat(تهی)
{
    صحیح		cont = TRUE;
    صحیح		first = TRUE;

    وقتی (cont)
    {
	انتخاب (peekchr())
	{
	    حالت NUL:
	    حالت Magic('|'):
	    حالت Magic('&'):
	    حالت Magic(')'):
		cont = FALSE;
		بشکن;

	    حالت Magic('Z'):
		regflags |= RF_ICOMBINE;
		skipchr_keepstart();
		بشکن;
	    حالت Magic('c'):
		regflags |= RF_ICASE;
		skipchr_keepstart();
		بشکن;
	    حالت Magic('C'):
		regflags |= RF_NOICASE;
		skipchr_keepstart();
		بشکن;
	    حالت Magic('v'):
		reg_magic = MAGIC_ALL;
		skipchr_keepstart();
		curchr = -1;
		بشکن;
	    حالت Magic('m'):
		reg_magic = MAGIC_ON;
		skipchr_keepstart();
		curchr = -1;
		بشکن;
	    حالت Magic('M'):
		reg_magic = MAGIC_OFF;
		skipchr_keepstart();
		curchr = -1;
		بشکن;
	    حالت Magic('V'):
		reg_magic = MAGIC_NONE;
		skipchr_keepstart();
		curchr = -1;
		بشکن;

	    پیشفرض:
		اگر (nfa_regpiece() == FAIL)
		    بازگشت FAIL;
		اگر (first == FALSE)
		    EMIT(NFA_CONCAT);
		وگرنه
		    first = FALSE;
		بشکن;
	}
    }

    بازگشت OK;
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
    ایستا صحیح
nfa_regbranch(تهی)
{
    صحیح		old_post_pos;

    old_post_pos = (صحیح)(post_ptr - post_start);

    // First branch, possibly the only one
    اگر (nfa_regconcat() == FAIL)
	بازگشت FAIL;

    // Try next concats
    وقتی (peekchr() == Magic('&'))
    {
	skipchr();
	// if concat is empty do emit a node
	اگر (old_post_pos == (صحیح)(post_ptr - post_start))
	    EMIT(NFA_EMPTY);
	EMIT(NFA_NOPEN);
	EMIT(NFA_PREV_ATOM_NO_WIDTH);
	old_post_pos = (صحیح)(post_ptr - post_start);
	اگر (nfa_regconcat() == FAIL)
	    بازگشت FAIL;
	// if concat is empty do emit a node
	اگر (old_post_pos == (صحیح)(post_ptr - post_start))
	    EMIT(NFA_EMPTY);
	EMIT(NFA_CONCAT);
    }

    // if a branch is empty, emit one node for it
    اگر (old_post_pos == (صحیح)(post_ptr - post_start))
	EMIT(NFA_EMPTY);

    بازگشت OK;
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
    ایستا صحیح
nfa_reg(
    صحیح		paren)	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
{
    صحیح		parno = 0;
    صحیح		status = FAIL;

    اگر (paren == REG_PAREN)
    {
	اگر (regnpar >= NSUBEXP) // Too many `('
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_parens));
	parno = regnpar++;
    }
#ifdef FEAT_SYN_HL
    وگرنه اگر (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	اگر (regnzpar >= NSUBEXP)
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_z));
	parno = regnzpar++;
    }
#endif

    اگر (nfa_reg_parse_depth >= REG_MAX_PAREN_DEPTH)
	EMSG_RET_FAIL(_(e_command_too_complex));
    ++nfa_reg_parse_depth;

    اگر (nfa_regbranch() == FAIL)
	برو theend;	    // cascaded error

    وقتی (peekchr() == Magic('|'))
    {
	skipchr();
	اگر (nfa_regbranch() == FAIL)
	    برو theend;    // cascaded error
	EMIT(NFA_OR);
    }

    // Check for proper termination.
    اگر (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
	اگر (paren == REG_NPAREN)
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	وگرنه
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	rc_did_emsg = TRUE;
	برو theend;
    }
    وگرنه اگر (paren == REG_NOPAREN && peekchr() != NUL)
    {
	اگر (peekchr() == Magic(')'))
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	وگرنه
	    emsg(_(e_nfa_regexp_proper_termination_error));
	rc_did_emsg = TRUE;
	برو theend;
    }
    /*
     * Here we set the flag allowing back references to this set of
     * parentheses.
     */
    اگر (paren == REG_PAREN)
    {
	had_endbrace[parno] = TRUE;     // have seen the close paren
	EMIT(NFA_MOPEN + parno);
    }
#ifdef FEAT_SYN_HL
    وگرنه اگر (paren == REG_ZPAREN)
	EMIT(NFA_ZOPEN + parno);
#endif

    status = OK;

theend:
    --nfa_reg_parse_depth;
    بازگشت status;
}

#ifdef DEBUG
ایستا char_u code[50];

    ایستا تهی
nfa_set_code(صحیح c)
{
    صحیح	    addnl = FALSE;

    اگر (c >= NFA_FIRST_NL && c <= NFA_LAST_NL)
    {
	addnl = TRUE;
	c -= NFA_ADD_NL;
    }

    STRCPY(code, "");
    انتخاب (c)
    {
	حالت NFA_MATCH:	    STRCPY(code, "NFA_MATCH "); بشکن;
	حالت NFA_SPLIT:	    STRCPY(code, "NFA_SPLIT "); بشکن;
	حالت NFA_CONCAT:    STRCPY(code, "NFA_CONCAT "); بشکن;
	حالت NFA_NEWL:	    STRCPY(code, "NFA_NEWL "); بشکن;
	حالت NFA_ZSTART:    STRCPY(code, "NFA_ZSTART"); بشکن;
	حالت NFA_ZEND:	    STRCPY(code, "NFA_ZEND"); بشکن;

	حالت NFA_BACKREF1:  STRCPY(code, "NFA_BACKREF1"); بشکن;
	حالت NFA_BACKREF2:  STRCPY(code, "NFA_BACKREF2"); بشکن;
	حالت NFA_BACKREF3:  STRCPY(code, "NFA_BACKREF3"); بشکن;
	حالت NFA_BACKREF4:  STRCPY(code, "NFA_BACKREF4"); بشکن;
	حالت NFA_BACKREF5:  STRCPY(code, "NFA_BACKREF5"); بشکن;
	حالت NFA_BACKREF6:  STRCPY(code, "NFA_BACKREF6"); بشکن;
	حالت NFA_BACKREF7:  STRCPY(code, "NFA_BACKREF7"); بشکن;
	حالت NFA_BACKREF8:  STRCPY(code, "NFA_BACKREF8"); بشکن;
	حالت NFA_BACKREF9:  STRCPY(code, "NFA_BACKREF9"); بشکن;
# ifdef FEAT_SYN_HL
	حالت NFA_ZREF1:	    STRCPY(code, "NFA_ZREF1"); بشکن;
	حالت NFA_ZREF2:	    STRCPY(code, "NFA_ZREF2"); بشکن;
	حالت NFA_ZREF3:	    STRCPY(code, "NFA_ZREF3"); بشکن;
	حالت NFA_ZREF4:	    STRCPY(code, "NFA_ZREF4"); بشکن;
	حالت NFA_ZREF5:	    STRCPY(code, "NFA_ZREF5"); بشکن;
	حالت NFA_ZREF6:	    STRCPY(code, "NFA_ZREF6"); بشکن;
	حالت NFA_ZREF7:	    STRCPY(code, "NFA_ZREF7"); بشکن;
	حالت NFA_ZREF8:	    STRCPY(code, "NFA_ZREF8"); بشکن;
	حالت NFA_ZREF9:	    STRCPY(code, "NFA_ZREF9"); بشکن;
# endif
	حالت NFA_SKIP:	    STRCPY(code, "NFA_SKIP"); بشکن;

	حالت NFA_PREV_ATOM_NO_WIDTH:
			    STRCPY(code, "NFA_PREV_ATOM_NO_WIDTH"); بشکن;
	حالت NFA_PREV_ATOM_NO_WIDTH_NEG:
			    STRCPY(code, "NFA_PREV_ATOM_NO_WIDTH_NEG"); بشکن;
	حالت NFA_PREV_ATOM_JUST_BEFORE:
			    STRCPY(code, "NFA_PREV_ATOM_JUST_BEFORE"); بشکن;
	حالت NFA_PREV_ATOM_JUST_BEFORE_NEG:
			 STRCPY(code, "NFA_PREV_ATOM_JUST_BEFORE_NEG"); بشکن;
	حالت NFA_PREV_ATOM_LIKE_PATTERN:
			    STRCPY(code, "NFA_PREV_ATOM_LIKE_PATTERN"); بشکن;

	حالت NFA_NOPEN:		    STRCPY(code, "NFA_NOPEN"); بشکن;
	حالت NFA_NCLOSE:	    STRCPY(code, "NFA_NCLOSE"); بشکن;
	حالت NFA_START_INVISIBLE:   STRCPY(code, "NFA_START_INVISIBLE"); بشکن;
	حالت NFA_START_INVISIBLE_FIRST:
			     STRCPY(code, "NFA_START_INVISIBLE_FIRST"); بشکن;
	حالت NFA_START_INVISIBLE_NEG:
			       STRCPY(code, "NFA_START_INVISIBLE_NEG"); بشکن;
	حالت NFA_START_INVISIBLE_NEG_FIRST:
			 STRCPY(code, "NFA_START_INVISIBLE_NEG_FIRST"); بشکن;
	حالت NFA_START_INVISIBLE_BEFORE:
			    STRCPY(code, "NFA_START_INVISIBLE_BEFORE"); بشکن;
	حالت NFA_START_INVISIBLE_BEFORE_FIRST:
		      STRCPY(code, "NFA_START_INVISIBLE_BEFORE_FIRST"); بشکن;
	حالت NFA_START_INVISIBLE_BEFORE_NEG:
			STRCPY(code, "NFA_START_INVISIBLE_BEFORE_NEG"); بشکن;
	حالت NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
		  STRCPY(code, "NFA_START_INVISIBLE_BEFORE_NEG_FIRST"); بشکن;
	حالت NFA_START_PATTERN:   STRCPY(code, "NFA_START_PATTERN"); بشکن;
	حالت NFA_END_INVISIBLE:	    STRCPY(code, "NFA_END_INVISIBLE"); بشکن;
	حالت NFA_END_INVISIBLE_NEG: STRCPY(code, "NFA_END_INVISIBLE_NEG"); بشکن;
	حالت NFA_END_PATTERN:	    STRCPY(code, "NFA_END_PATTERN"); بشکن;

	حالت NFA_COMPOSING:	    STRCPY(code, "NFA_COMPOSING"); بشکن;
	حالت NFA_END_COMPOSING:	    STRCPY(code, "NFA_END_COMPOSING"); بشکن;
	حالت NFA_OPT_CHARS:	    STRCPY(code, "NFA_OPT_CHARS"); بشکن;

	حالت NFA_MOPEN:
	حالت NFA_MOPEN1:
	حالت NFA_MOPEN2:
	حالت NFA_MOPEN3:
	حالت NFA_MOPEN4:
	حالت NFA_MOPEN5:
	حالت NFA_MOPEN6:
	حالت NFA_MOPEN7:
	حالت NFA_MOPEN8:
	حالت NFA_MOPEN9:
	    STRCPY(code, "NFA_MOPEN(x)");
	    code[10] = c - NFA_MOPEN + '0';
	    بشکن;
	حالت NFA_MCLOSE:
	حالت NFA_MCLOSE1:
	حالت NFA_MCLOSE2:
	حالت NFA_MCLOSE3:
	حالت NFA_MCLOSE4:
	حالت NFA_MCLOSE5:
	حالت NFA_MCLOSE6:
	حالت NFA_MCLOSE7:
	حالت NFA_MCLOSE8:
	حالت NFA_MCLOSE9:
	    STRCPY(code, "NFA_MCLOSE(x)");
	    code[11] = c - NFA_MCLOSE + '0';
	    بشکن;
# ifdef FEAT_SYN_HL
	حالت NFA_ZOPEN:
	حالت NFA_ZOPEN1:
	حالت NFA_ZOPEN2:
	حالت NFA_ZOPEN3:
	حالت NFA_ZOPEN4:
	حالت NFA_ZOPEN5:
	حالت NFA_ZOPEN6:
	حالت NFA_ZOPEN7:
	حالت NFA_ZOPEN8:
	حالت NFA_ZOPEN9:
	    STRCPY(code, "NFA_ZOPEN(x)");
	    code[10] = c - NFA_ZOPEN + '0';
	    بشکن;
	حالت NFA_ZCLOSE:
	حالت NFA_ZCLOSE1:
	حالت NFA_ZCLOSE2:
	حالت NFA_ZCLOSE3:
	حالت NFA_ZCLOSE4:
	حالت NFA_ZCLOSE5:
	حالت NFA_ZCLOSE6:
	حالت NFA_ZCLOSE7:
	حالت NFA_ZCLOSE8:
	حالت NFA_ZCLOSE9:
	    STRCPY(code, "NFA_ZCLOSE(x)");
	    code[11] = c - NFA_ZCLOSE + '0';
	    بشکن;
# endif
	حالت NFA_EOL:		STRCPY(code, "NFA_EOL "); بشکن;
	حالت NFA_BOL:		STRCPY(code, "NFA_BOL "); بشکن;
	حالت NFA_EOW:		STRCPY(code, "NFA_EOW "); بشکن;
	حالت NFA_BOW:		STRCPY(code, "NFA_BOW "); بشکن;
	حالت NFA_EOF:		STRCPY(code, "NFA_EOF "); بشکن;
	حالت NFA_BOF:		STRCPY(code, "NFA_BOF "); بشکن;
	حالت NFA_LNUM:		STRCPY(code, "NFA_LNUM "); بشکن;
	حالت NFA_LNUM_GT:	STRCPY(code, "NFA_LNUM_GT "); بشکن;
	حالت NFA_LNUM_LT:	STRCPY(code, "NFA_LNUM_LT "); بشکن;
	حالت NFA_COL:		STRCPY(code, "NFA_COL "); بشکن;
	حالت NFA_COL_GT:	STRCPY(code, "NFA_COL_GT "); بشکن;
	حالت NFA_COL_LT:	STRCPY(code, "NFA_COL_LT "); بشکن;
	حالت NFA_VCOL:		STRCPY(code, "NFA_VCOL "); بشکن;
	حالت NFA_VCOL_GT:	STRCPY(code, "NFA_VCOL_GT "); بشکن;
	حالت NFA_VCOL_LT:	STRCPY(code, "NFA_VCOL_LT "); بشکن;
	حالت NFA_MARK:		STRCPY(code, "NFA_MARK "); بشکن;
	حالت NFA_MARK_GT:	STRCPY(code, "NFA_MARK_GT "); بشکن;
	حالت NFA_MARK_LT:	STRCPY(code, "NFA_MARK_LT "); بشکن;
	حالت NFA_CURSOR:	STRCPY(code, "NFA_CURSOR "); بشکن;
	حالت NFA_VISUAL:	STRCPY(code, "NFA_VISUAL "); بشکن;
	حالت NFA_ANY_COMPOSING:	STRCPY(code, "NFA_ANY_COMPOSING "); بشکن;

	حالت NFA_STAR:		STRCPY(code, "NFA_STAR "); بشکن;
	حالت NFA_STAR_NONGREEDY: STRCPY(code, "NFA_STAR_NONGREEDY "); بشکن;
	حالت NFA_QUEST:		STRCPY(code, "NFA_QUEST"); بشکن;
	حالت NFA_QUEST_NONGREEDY: STRCPY(code, "NFA_QUEST_NON_GREEDY"); بشکن;
	حالت NFA_EMPTY:		STRCPY(code, "NFA_EMPTY"); بشکن;
	حالت NFA_OR:		STRCPY(code, "NFA_OR"); بشکن;

	حالت NFA_START_COLL:	STRCPY(code, "NFA_START_COLL"); بشکن;
	حالت NFA_END_COLL:	STRCPY(code, "NFA_END_COLL"); بشکن;
	حالت NFA_START_NEG_COLL: STRCPY(code, "NFA_START_NEG_COLL"); بشکن;
	حالت NFA_END_NEG_COLL:	STRCPY(code, "NFA_END_NEG_COLL"); بشکن;
	حالت NFA_RANGE:		STRCPY(code, "NFA_RANGE"); بشکن;
	حالت NFA_RANGE_MIN:	STRCPY(code, "NFA_RANGE_MIN"); بشکن;
	حالت NFA_RANGE_MAX:	STRCPY(code, "NFA_RANGE_MAX"); بشکن;

	حالت NFA_CLASS_ALNUM:	STRCPY(code, "NFA_CLASS_ALNUM"); بشکن;
	حالت NFA_CLASS_ALPHA:	STRCPY(code, "NFA_CLASS_ALPHA"); بشکن;
	حالت NFA_CLASS_BLANK:	STRCPY(code, "NFA_CLASS_BLANK"); بشکن;
	حالت NFA_CLASS_CNTRL:	STRCPY(code, "NFA_CLASS_CNTRL"); بشکن;
	حالت NFA_CLASS_DIGIT:	STRCPY(code, "NFA_CLASS_DIGIT"); بشکن;
	حالت NFA_CLASS_GRAPH:	STRCPY(code, "NFA_CLASS_GRAPH"); بشکن;
	حالت NFA_CLASS_LOWER:	STRCPY(code, "NFA_CLASS_LOWER"); بشکن;
	حالت NFA_CLASS_PRINT:	STRCPY(code, "NFA_CLASS_PRINT"); بشکن;
	حالت NFA_CLASS_PUNCT:	STRCPY(code, "NFA_CLASS_PUNCT"); بشکن;
	حالت NFA_CLASS_SPACE:	STRCPY(code, "NFA_CLASS_SPACE"); بشکن;
	حالت NFA_CLASS_UPPER:	STRCPY(code, "NFA_CLASS_UPPER"); بشکن;
	حالت NFA_CLASS_XDIGIT:	STRCPY(code, "NFA_CLASS_XDIGIT"); بشکن;
	حالت NFA_CLASS_TAB:	STRCPY(code, "NFA_CLASS_TAB"); بشکن;
	حالت NFA_CLASS_RETURN:	STRCPY(code, "NFA_CLASS_RETURN"); بشکن;
	حالت NFA_CLASS_BACKSPACE:   STRCPY(code, "NFA_CLASS_BACKSPACE"); بشکن;
	حالت NFA_CLASS_ESCAPE:	STRCPY(code, "NFA_CLASS_ESCAPE"); بشکن;
	حالت NFA_CLASS_IDENT:	STRCPY(code, "NFA_CLASS_IDENT"); بشکن;
	حالت NFA_CLASS_KEYWORD:	STRCPY(code, "NFA_CLASS_KEYWORD"); بشکن;
	حالت NFA_CLASS_FNAME:	STRCPY(code, "NFA_CLASS_FNAME"); بشکن;

	حالت NFA_ANY:	STRCPY(code, "NFA_ANY"); بشکن;
	حالت NFA_IDENT:	STRCPY(code, "NFA_IDENT"); بشکن;
	حالت NFA_SIDENT:STRCPY(code, "NFA_SIDENT"); بشکن;
	حالت NFA_KWORD:	STRCPY(code, "NFA_KWORD"); بشکن;
	حالت NFA_SKWORD:STRCPY(code, "NFA_SKWORD"); بشکن;
	حالت NFA_FNAME:	STRCPY(code, "NFA_FNAME"); بشکن;
	حالت NFA_SFNAME:STRCPY(code, "NFA_SFNAME"); بشکن;
	حالت NFA_PRINT:	STRCPY(code, "NFA_PRINT"); بشکن;
	حالت NFA_SPRINT:STRCPY(code, "NFA_SPRINT"); بشکن;
	حالت NFA_WHITE:	STRCPY(code, "NFA_WHITE"); بشکن;
	حالت NFA_NWHITE:STRCPY(code, "NFA_NWHITE"); بشکن;
	حالت NFA_DIGIT:	STRCPY(code, "NFA_DIGIT"); بشکن;
	حالت NFA_NDIGIT:STRCPY(code, "NFA_NDIGIT"); بشکن;
	حالت NFA_HEX:	STRCPY(code, "NFA_HEX"); بشکن;
	حالت NFA_NHEX:	STRCPY(code, "NFA_NHEX"); بشکن;
	حالت NFA_OCTAL:	STRCPY(code, "NFA_OCTAL"); بشکن;
	حالت NFA_NOCTAL:STRCPY(code, "NFA_NOCTAL"); بشکن;
	حالت NFA_WORD:	STRCPY(code, "NFA_WORD"); بشکن;
	حالت NFA_NWORD:	STRCPY(code, "NFA_NWORD"); بشکن;
	حالت NFA_HEAD:	STRCPY(code, "NFA_HEAD"); بشکن;
	حالت NFA_NHEAD:	STRCPY(code, "NFA_NHEAD"); بشکن;
	حالت NFA_ALPHA:	STRCPY(code, "NFA_ALPHA"); بشکن;
	حالت NFA_NALPHA:STRCPY(code, "NFA_NALPHA"); بشکن;
	حالت NFA_LOWER:	STRCPY(code, "NFA_LOWER"); بشکن;
	حالت NFA_NLOWER:STRCPY(code, "NFA_NLOWER"); بشکن;
	حالت NFA_UPPER:	STRCPY(code, "NFA_UPPER"); بشکن;
	حالت NFA_NUPPER:STRCPY(code, "NFA_NUPPER"); بشکن;
	حالت NFA_LOWER_IC:  STRCPY(code, "NFA_LOWER_IC"); بشکن;
	حالت NFA_NLOWER_IC: STRCPY(code, "NFA_NLOWER_IC"); بشکن;
	حالت NFA_UPPER_IC:  STRCPY(code, "NFA_UPPER_IC"); بشکن;
	حالت NFA_NUPPER_IC: STRCPY(code, "NFA_NUPPER_IC"); بشکن;

	پیشفرض:
	    STRCPY(code, "CHAR(x)");
	    code[5] = c;
    }

    اگر (addnl == TRUE)
	STRCAT(code, " + NEWLINE ");

}

# ifdef ENABLE_LOG
ایستا FILE *log_fd;
ایستا char_u e_log_open_failed[] = N_("Could not open temporary log file for writing, displaying on stderr... ");

/*
 * Print the postfix notation of the current regexp.
 */
    ایستا تهی
nfa_postfix_dump(char_u *عبارت, صحیح retval)
{
    صحیح *p;
    FILE *f;

    f = fopen(NFA_REGEXP_DUMP_LOG, "a");
    اگر (f == پوچ)
	بازگشت;

    fprintf(f, "\n-------------------------\n");
    اگر (retval == FAIL)
	fprintf(f, ">>> NFA engine failed... \n");
    وگرنه اگر (retval == OK)
	fprintf(f, ">>> NFA engine succeeded !\n");
    fprintf(f, "Regexp: \"%s\"\nPostfix notation (char): \"", عبارت);
    برای (p = post_start; *p && p < post_ptr; p++)
    {
	nfa_set_code(*p);
	fprintf(f, "%s, ", code);
    }
    fprintf(f, "\"\nPostfix notation (int): ");
    برای (p = post_start; *p && p < post_ptr; p++)
	fprintf(f, "%d ", *p);
    fprintf(f, "\n\n");
    fclose(f);
}

/*
 * Print the NFA starting with a root node "state".
 */
    ایستا تهی
nfa_print_state(FILE *debugf, nfa_state_T *state)
{
    garray_T indent;

    ga_init2(&indent, 1, 64);
    ga_append(&indent, '\0');
    nfa_print_state2(debugf, state, &indent);
    ga_clear(&indent);
}

    ایستا تهی
nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent)
{
    char_u  *p;

    اگر (state == پوچ)
	بازگشت;

    fprintf(debugf, "(%2d)", abs(state->id));

    // Output indent
    p = (char_u *)indent->ga_data;
    اگر (indent->ga_len >= 3)
    {
	صحیح	last = indent->ga_len - 3;
	char_u	save[2];

	STRNCPY(save, &p[last], 2);
	memcpy(&p[last], "+-", 2);
	fprintf(debugf, " %s", p);
	STRNCPY(&p[last], save, 2);
    }
    وگرنه
	fprintf(debugf, " %s", p);

    nfa_set_code(state->c);
    fprintf(debugf, "%s (%d) (id=%d) val=%d\n",
		 code,
		 state->c,
		 abs(state->id),
		 state->val);
    اگر (state->id < 0)
	بازگشت;

    state->id = abs(state->id) * -1;

    // grow indent for state->out
    indent->ga_len -= 1;
    اگر (state->out1)
	GA_CONCAT_LITERAL(indent, "| ");
    وگرنه
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
    ایستا تهی
nfa_dump(nfa_regprog_T *prog)
{
    FILE *debugf = fopen(NFA_REGEXP_DUMP_LOG, "a");

    اگر (debugf == پوچ)
	بازگشت;

    nfa_print_state(debugf, prog->start);

    اگر (prog->reganch)
	fprintf(debugf, "reganch: %d\n", prog->reganch);
    اگر (prog->regstart != NUL)
	fprintf(debugf, "regstart: %c (decimal: %d)\n",
		prog->regstart, prog->regstart);
    اگر (prog->match_text != پوچ)
	fprintf(debugf, "match_text: \"%s\"\n", prog->match_text);

    fclose(debugf);
}
# endif	    // ENABLE_LOG
#endif	    // DEBUG

/*
 * Parse r.e. @expr and convert it into postfix form.
 * Return the postfix string on success, NULL otherwise.
 */
    ایستا صحیح *
re2post(تهی)
{
    اگر (nfa_reg(REG_NOPAREN) == FAIL)
	بازگشت پوچ;
    EMIT(NFA_MOPEN);
    بازگشت post_start;
}

// NB. Some of the code below is inspired by Russ's.

/*
 * Represents an NFA state plus zero or one or two arrows exiting.
 * if c == MATCH, no arrows out; matching state.
 * If c == SPLIT, unlabeled arrows to out and out1 (if != NULL).
 * If c < 256, labeled arrow with character c to out.
 */

ایستا nfa_state_T	*state_ptr; // points to nfa_prog->state

/*
 * Allocate and initialize nfa_state_T.
 */
    ایستا nfa_state_T *
alloc_state(صحیح c, nfa_state_T *out, nfa_state_T *out1)
{
    nfa_state_T *s;

    اگر (istate >= nstate)
	بازگشت پوچ;

    s = &state_ptr[istate++];

    s->c    = c;
    s->out  = out;
    s->out1 = out1;
    s->val  = 0;

    s->id   = istate;
    s->lastlist[0] = 0;
    s->lastlist[1] = 0;

    بازگشت s;
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
تعریفنوع اجتماع Ptrlist Ptrlist;
اجتماع Ptrlist
{
    Ptrlist	*next;
    nfa_state_T	*s;
};

ساختار Frag
{
    nfa_state_T *start;
    Ptrlist	*out;
};
تعریفنوع ساختار Frag Frag_T;

// Reused across compilations by post2nfa(); freed in free_regexp_stuff().
ایستا Frag_T *nfa_پشته;
ایستا صحیح nfa_پشته_len;

/*
 * Initialize a Frag_T struct and return it.
 */
    ایستا Frag_T
frag(nfa_state_T *start, Ptrlist *out)
{
    Frag_T n;

    n.start = start;
    n.out = out;
    بازگشت n;
}

/*
 * Create singleton list containing just outp.
 */
    ایستا Ptrlist *
list1(
    nfa_state_T	**outp)
{
    Ptrlist *l;

    l = (Ptrlist *)outp;
    l->next = پوچ;
    بازگشت l;
}

/*
 * Patch the list of states at out to point to start.
 */
    ایستا تهی
patch(Ptrlist *l, nfa_state_T *s)
{
    Ptrlist *next;

    برای (; l; l = next)
    {
	next = l->next;
	l->s = s;
    }
}


/*
 * Join the two lists l1 and l2, returning the combination.
 */
    ایستا Ptrlist *
append(Ptrlist *l1, Ptrlist *l2)
{
    Ptrlist *oldl1;

    oldl1 = l1;
    وقتی (l1->next)
	l1 = l1->next;
    l1->next = l2;
    بازگشت oldl1;
}

/*
 * Stack used for transforming postfix form into NFA.
 */
ایستا Frag_T empty;

    ایستا تهی
st_error(صحیح *postfix UNUSED, صحیح *end UNUSED, صحیح *p UNUSED)
{
#ifdef NFA_REGEXP_ERROR_LOG
    FILE *df;
    صحیح *p2;

    df = fopen(NFA_REGEXP_ERROR_LOG, "a");
    اگر (df)
    {
	fprintf(df, "Error popping the stack!\n");
# ifdef DEBUG
	fprintf(df, "Current regexp is \"%s\"\n", nfa_regengine.عبارت);
# endif
	fprintf(df, "Postfix form is: ");
# ifdef DEBUG
	برای (p2 = postfix; p2 < end; p2++)
	{
	    nfa_set_code(*p2);
	    fprintf(df, "%s, ", code);
	}
	nfa_set_code(*p);
	fprintf(df, "\nCurrent position is: ");
	برای (p2 = postfix; p2 <= p; p2 ++)
	{
	    nfa_set_code(*p2);
	    fprintf(df, "%s, ", code);
	}
# وگرنه
	برای (p2 = postfix; p2 < end; p2++)
	    fprintf(df, "%d, ", *p2);
	fprintf(df, "\nCurrent position is: ");
	برای (p2 = postfix; p2 <= p; p2 ++)
	    fprintf(df, "%d, ", *p2);
# endif
	fprintf(df, "\n--------------------------\n");
	fclose(df);
    }
#endif
    emsg(_(e_nfa_regexp_could_not_برداشت_پشته));
}

/*
 * Push an item onto the stack.
 */
    ایستا تهی
st_افزودن(Frag_T s, Frag_T **p, Frag_T *پشته_end)
{
    Frag_T *stackp = *p;

    اگر (stackp >= پشته_end)
	بازگشت;
    *stackp = s;
    *p = *p + 1;
}

/*
 * Pop an item from the stack.
 */
    ایستا Frag_T
st_برداشت(Frag_T **p, Frag_T *پشته)
{
    Frag_T *stackp;

    *p = *p - 1;
    stackp = *p;
    اگر (stackp < پشته)
	بازگشت empty;
    بازگشت **p;
}

/*
 * Estimate the maximum byte length of anything matching "state".
 * When unknown or unlimited return -1.
 */
    ایستا صحیح
nfa_max_width(nfa_state_T *startstate, صحیح depth)
{
    صحیح		    l, r;
    nfa_state_T	    *state = startstate;
    صحیح		    len = 0;

    // detect looping in a NFA_SPLIT
    اگر (depth > 4)
	بازگشت -1;

    وقتی (state != پوچ)
    {
	انتخاب (state->c)
	{
	    حالت NFA_END_INVISIBLE:
	    حالت NFA_END_INVISIBLE_NEG:
		// the end, return what we have
		بازگشت len;

	    حالت NFA_SPLIT:
		// two alternatives, use the maximum
		l = nfa_max_width(state->out, depth + 1);
		r = nfa_max_width(state->out1, depth + 1);
		اگر (l < 0 || r < 0)
		    بازگشت -1;
		بازگشت len + (l > r ? l : r);

	    حالت NFA_ANY:
	    حالت NFA_START_COLL:
	    حالت NFA_START_NEG_COLL:
		// matches some character, including composing chars
		اگر (enc_utf8)
		    len += MB_MAXBYTES;
		وگرنه اگر (has_mbyte)
		    len += 2;
		وگرنه
		    ++len;
		اگر (state->c != NFA_ANY)
		{
		    // Skip over the compiled collection.
		    // malformed NFAs must not crash width estimation.
		    اگر (state->out1 == پوچ || state->out1->out == پوچ)
			بازگشت -1;
		    state = state->out1->out;
		    ادامه;
		}
		بشکن;

	    حالت NFA_DIGIT:
	    حالت NFA_WHITE:
	    حالت NFA_HEX:
	    حالت NFA_OCTAL:
		// ascii
		++len;
		بشکن;

	    حالت NFA_IDENT:
	    حالت NFA_SIDENT:
	    حالت NFA_KWORD:
	    حالت NFA_SKWORD:
	    حالت NFA_FNAME:
	    حالت NFA_SFNAME:
	    حالت NFA_PRINT:
	    حالت NFA_SPRINT:
	    حالت NFA_NWHITE:
	    حالت NFA_NDIGIT:
	    حالت NFA_NHEX:
	    حالت NFA_NOCTAL:
	    حالت NFA_WORD:
	    حالت NFA_NWORD:
	    حالت NFA_HEAD:
	    حالت NFA_NHEAD:
	    حالت NFA_ALPHA:
	    حالت NFA_NALPHA:
	    حالت NFA_LOWER:
	    حالت NFA_NLOWER:
	    حالت NFA_UPPER:
	    حالت NFA_NUPPER:
	    حالت NFA_LOWER_IC:
	    حالت NFA_NLOWER_IC:
	    حالت NFA_UPPER_IC:
	    حالت NFA_NUPPER_IC:
	    حالت NFA_ANY_COMPOSING:
		// possibly non-ascii
		اگر (has_mbyte)
		    len += 3;
		وگرنه
		    ++len;
		بشکن;

	    حالت NFA_START_INVISIBLE:
	    حالت NFA_START_INVISIBLE_NEG:
	    حالت NFA_START_INVISIBLE_BEFORE:
	    حالت NFA_START_INVISIBLE_BEFORE_NEG:
		// zero-width, out1 points to the END state
		state = state->out1->out;
		ادامه;

	    حالت NFA_BACKREF1:
	    حالت NFA_BACKREF2:
	    حالت NFA_BACKREF3:
	    حالت NFA_BACKREF4:
	    حالت NFA_BACKREF5:
	    حالت NFA_BACKREF6:
	    حالت NFA_BACKREF7:
	    حالت NFA_BACKREF8:
	    حالت NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	    حالت NFA_ZREF1:
	    حالت NFA_ZREF2:
	    حالت NFA_ZREF3:
	    حالت NFA_ZREF4:
	    حالت NFA_ZREF5:
	    حالت NFA_ZREF6:
	    حالت NFA_ZREF7:
	    حالت NFA_ZREF8:
	    حالت NFA_ZREF9:
#endif
	    حالت NFA_NEWL:
	    حالت NFA_SKIP:
		// unknown width
		بازگشت -1;

	    حالت NFA_BOL:
	    حالت NFA_EOL:
	    حالت NFA_BOF:
	    حالت NFA_EOF:
	    حالت NFA_BOW:
	    حالت NFA_EOW:
	    حالت NFA_MOPEN:
	    حالت NFA_MOPEN1:
	    حالت NFA_MOPEN2:
	    حالت NFA_MOPEN3:
	    حالت NFA_MOPEN4:
	    حالت NFA_MOPEN5:
	    حالت NFA_MOPEN6:
	    حالت NFA_MOPEN7:
	    حالت NFA_MOPEN8:
	    حالت NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	    حالت NFA_ZOPEN:
	    حالت NFA_ZOPEN1:
	    حالت NFA_ZOPEN2:
	    حالت NFA_ZOPEN3:
	    حالت NFA_ZOPEN4:
	    حالت NFA_ZOPEN5:
	    حالت NFA_ZOPEN6:
	    حالت NFA_ZOPEN7:
	    حالت NFA_ZOPEN8:
	    حالت NFA_ZOPEN9:
	    حالت NFA_ZCLOSE:
	    حالت NFA_ZCLOSE1:
	    حالت NFA_ZCLOSE2:
	    حالت NFA_ZCLOSE3:
	    حالت NFA_ZCLOSE4:
	    حالت NFA_ZCLOSE5:
	    حالت NFA_ZCLOSE6:
	    حالت NFA_ZCLOSE7:
	    حالت NFA_ZCLOSE8:
	    حالت NFA_ZCLOSE9:
#endif
	    حالت NFA_MCLOSE:
	    حالت NFA_MCLOSE1:
	    حالت NFA_MCLOSE2:
	    حالت NFA_MCLOSE3:
	    حالت NFA_MCLOSE4:
	    حالت NFA_MCLOSE5:
	    حالت NFA_MCLOSE6:
	    حالت NFA_MCLOSE7:
	    حالت NFA_MCLOSE8:
	    حالت NFA_MCLOSE9:
	    حالت NFA_NOPEN:
	    حالت NFA_NCLOSE:

	    حالت NFA_LNUM_GT:
	    حالت NFA_LNUM_LT:
	    حالت NFA_COL_GT:
	    حالت NFA_COL_LT:
	    حالت NFA_VCOL_GT:
	    حالت NFA_VCOL_LT:
	    حالت NFA_MARK_GT:
	    حالت NFA_MARK_LT:
	    حالت NFA_VISUAL:
	    حالت NFA_LNUM:
	    حالت NFA_CURSOR:
	    حالت NFA_COL:
	    حالت NFA_VCOL:
	    حالت NFA_MARK:

	    حالت NFA_ZSTART:
	    حالت NFA_ZEND:
	    حالت NFA_OPT_CHARS:
	    حالت NFA_EMPTY:
	    حالت NFA_START_PATTERN:
	    حالت NFA_END_PATTERN:
	    حالت NFA_COMPOSING:
	    حالت NFA_END_COMPOSING:
		// zero-width
		بشکن;

	    پیشفرض:
		اگر (state->c < 0)
		    // don't know what this is
		    بازگشت -1;
		// normal character
		len += MB_CHAR2LEN(state->c);
		بشکن;
	}

	// normal way to continue
	state = state->out;
    }

    // unrecognized, "cannot happen"
    بازگشت -1;
}

/*
 * Convert a postfix form into its equivalent NFA.
 * Return the NFA start state on success, NULL otherwise.
 */
    ایستا nfa_state_T *
post2nfa(صحیح *postfix, صحیح *end, صحیح nfa_calc_size)
{
    صحیح		*p;
    صحیح		mopen;
    صحیح		mclose;
    Frag_T	*پشته = پوچ;
    Frag_T	*stackp = پوچ;
    Frag_T	*پشته_end = پوچ;
    Frag_T	e1;
    Frag_T	e2;
    Frag_T	e;
    nfa_state_T	*s;
    nfa_state_T	*s1;
    nfa_state_T	*matchstate;
    nfa_state_T	*ret = پوچ;

    اگر (postfix == پوچ)
	بازگشت پوچ;

#define افزودن_2(s)	    st_افزودن((s), &stackp, پشته_end)
#define برداشت_2()	    st_برداشت(&stackp, پشته);		\
		    اگر (stackp < پشته)			\
		    {					\
			st_error(postfix, end, p);	\
			بازگشت پوچ;			\
		    }

    اگر (nfa_calc_size == FALSE)
    {
	// Reuse the fragment stack across compilations, growing when needed;
	// it is freed in free_regexp_stuff().
	اگر (nfa_پشته == پوچ || nfa_پشته_len < nstate + 1)
	{
	    vim_free(nfa_پشته);
	    nfa_پشته = ALLOC_MULT(Frag_T, nstate + 1);
	    اگر (nfa_پشته == پوچ)
	    {
		nfa_پشته_len = 0;
		بازگشت پوچ;
	    }
	    nfa_پشته_len = nstate + 1;
	}
	پشته = nfa_پشته;
	stackp = پشته;
	پشته_end = پشته + (nstate + 1);
    }

    برای (p = postfix; p < end; ++p)
    {
	انتخاب (*p)
	{
	حالت NFA_CONCAT:
	    // Concatenation.
	    // Pay attention: this operator does not exist in the r.e. itself
	    // (it is implicit, really).  It is added when r.e. is translated
	    // to postfix form in re2post().
	    اگر (nfa_calc_size == TRUE)
	    {
		// nstate += 0;
		بشکن;
	    }
	    e2 = برداشت_2();
	    e1 = برداشت_2();
	    patch(e1.out, e2.start);
	    افزودن_2(frag(e1.start, e2.out));
	    بشکن;

	حالت NFA_OR:
	    // Alternation
	    اگر (nfa_calc_size == TRUE)
	    {
		nstate++;
		بشکن;
	    }
	    e2 = برداشت_2();
	    e1 = برداشت_2();
	    s = alloc_state(NFA_SPLIT, e1.start, e2.start);
	    اگر (s == پوچ)
		برو theend;
	    افزودن_2(frag(s, append(e1.out, e2.out)));
	    بشکن;

	حالت NFA_STAR:
	    // Zero or more, prefer more
	    اگر (nfa_calc_size == TRUE)
	    {
		nstate++;
		بشکن;
	    }
	    e = برداشت_2();
	    s = alloc_state(NFA_SPLIT, e.start, پوچ);
	    اگر (s == پوچ)
		برو theend;
	    patch(e.out, s);
	    افزودن_2(frag(s, list1(&s->out1)));
	    بشکن;

	حالت NFA_STAR_NONGREEDY:
	    // Zero or more, prefer zero
	    اگر (nfa_calc_size == TRUE)
	    {
		nstate++;
		بشکن;
	    }
	    e = برداشت_2();
	    s = alloc_state(NFA_SPLIT, پوچ, e.start);
	    اگر (s == پوچ)
		برو theend;
	    patch(e.out, s);
	    افزودن_2(frag(s, list1(&s->out)));
	    بشکن;

	حالت NFA_QUEST:
	    // one or zero atoms=> greedy match
	    اگر (nfa_calc_size == TRUE)
	    {
		nstate++;
		بشکن;
	    }
	    e = برداشت_2();
	    s = alloc_state(NFA_SPLIT, e.start, پوچ);
	    اگر (s == پوچ)
		برو theend;
	    افزودن_2(frag(s, append(e.out, list1(&s->out1))));
	    بشکن;

	حالت NFA_QUEST_NONGREEDY:
	    // zero or one atoms => non-greedy match
	    اگر (nfa_calc_size == TRUE)
	    {
		nstate++;
		بشکن;
	    }
	    e = برداشت_2();
	    s = alloc_state(NFA_SPLIT, پوچ, e.start);
	    اگر (s == پوچ)
		برو theend;
	    افزودن_2(frag(s, append(e.out, list1(&s->out))));
	    بشکن;

	حالت NFA_END_COLL:
	حالت NFA_END_NEG_COLL:
	    // On the stack is the sequence starting with NFA_START_COLL or
	    // NFA_START_NEG_COLL and all possible characters. Patch it to
	    // add the output to the start.
	    اگر (nfa_calc_size == TRUE)
	    {
		nstate++;
		بشکن;
	    }
	    e = برداشت_2();
	    s = alloc_state(NFA_END_COLL, پوچ, پوچ);
	    اگر (s == پوچ)
		برو theend;
	    patch(e.out, s);
	    e.start->out1 = s;
	    افزودن_2(frag(e.start, list1(&s->out)));
	    بشکن;

	حالت NFA_RANGE:
	    // Before this are two characters, the low and high end of a
	    // range.  Turn them into two states with MIN and MAX.
	    اگر (nfa_calc_size == TRUE)
	    {
		// nstate += 0;
		بشکن;
	    }
	    e2 = برداشت_2();
	    e1 = برداشت_2();
	    e2.start->val = e2.start->c;
	    e2.start->c = NFA_RANGE_MAX;
	    e1.start->val = e1.start->c;
	    e1.start->c = NFA_RANGE_MIN;
	    patch(e1.out, e2.start);
	    افزودن_2(frag(e1.start, e2.out));
	    بشکن;

	حالت NFA_EMPTY:
	    // 0-length, used in a repetition with max/min count of 0
	    اگر (nfa_calc_size == TRUE)
	    {
		nstate++;
		بشکن;
	    }
	    s = alloc_state(NFA_EMPTY, پوچ, پوچ);
	    اگر (s == پوچ)
		برو theend;
	    افزودن_2(frag(s, list1(&s->out)));
	    بشکن;

	حالت NFA_OPT_CHARS:
	  {
	    صحیح    n;

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
	    اگر (nfa_calc_size == TRUE)
	    {
		nstate += n;
		بشکن;
	    }
	    s = پوچ; // avoid compiler warning
	    e1.out = پوچ; // stores list with out1's
	    s1 = پوچ; // previous NFA_SPLIT to connect to
	    وقتی (n-- > 0)
	    {
		e = برداشت_2(); // get character
		s = alloc_state(NFA_SPLIT, e.start, پوچ);
		اگر (s == پوچ)
		    برو theend;
		اگر (e1.out == پوچ)
		    e1 = e;
		patch(e.out, s1);
		append(e1.out, list1(&s->out1));
		s1 = s;
	    }
	    افزودن_2(frag(s, e1.out));
	    بشکن;
	  }

	حالت NFA_PREV_ATOM_NO_WIDTH:
	حالت NFA_PREV_ATOM_NO_WIDTH_NEG:
	حالت NFA_PREV_ATOM_JUST_BEFORE:
	حالت NFA_PREV_ATOM_JUST_BEFORE_NEG:
	حالت NFA_PREV_ATOM_LIKE_PATTERN:
	  {
	    صحیح before = (*p == NFA_PREV_ATOM_JUST_BEFORE
				      || *p == NFA_PREV_ATOM_JUST_BEFORE_NEG);
	    صحیح pattern = (*p == NFA_PREV_ATOM_LIKE_PATTERN);
	    صحیح start_state;
	    صحیح end_state;
	    صحیح n = 0;
	    nfa_state_T *zend;
	    nfa_state_T *skip;

	    انتخاب (*p)
	    {
		حالت NFA_PREV_ATOM_NO_WIDTH:
		    start_state = NFA_START_INVISIBLE;
		    end_state = NFA_END_INVISIBLE;
		    بشکن;
		حالت NFA_PREV_ATOM_NO_WIDTH_NEG:
		    start_state = NFA_START_INVISIBLE_NEG;
		    end_state = NFA_END_INVISIBLE_NEG;
		    بشکن;
		حالت NFA_PREV_ATOM_JUST_BEFORE:
		    start_state = NFA_START_INVISIBLE_BEFORE;
		    end_state = NFA_END_INVISIBLE;
		    بشکن;
		حالت NFA_PREV_ATOM_JUST_BEFORE_NEG:
		    start_state = NFA_START_INVISIBLE_BEFORE_NEG;
		    end_state = NFA_END_INVISIBLE_NEG;
		    بشکن;
		پیشفرض: // NFA_PREV_ATOM_LIKE_PATTERN:
		    start_state = NFA_START_PATTERN;
		    end_state = NFA_END_PATTERN;
		    بشکن;
	    }

	    اگر (before)
		n = *++p; // get the count

	    // The \@= operator: match the preceding atom with zero width.
	    // The \@! operator: no match for the preceding atom.
	    // The \@<= operator: match for the preceding atom.
	    // The \@<! operator: no match for the preceding atom.
	    // Surrounds the preceding atom with START_INVISIBLE and
	    // END_INVISIBLE, similarly to MOPEN.

	    اگر (nfa_calc_size == TRUE)
	    {
		nstate += pattern ? 4 : 2;
		بشکن;
	    }
	    e = برداشت_2();
	    s1 = alloc_state(end_state, پوچ, پوچ);
	    اگر (s1 == پوچ)
		برو theend;

	    s = alloc_state(start_state, e.start, s1);
	    اگر (s == پوچ)
		برو theend;
	    اگر (pattern)
	    {
		// NFA_ZEND -> NFA_END_PATTERN -> NFA_SKIP -> what follows.
		skip = alloc_state(NFA_SKIP, پوچ, پوچ);
		اگر (skip == پوچ)
		    برو theend;
		zend = alloc_state(NFA_ZEND, s1, پوچ);
		اگر (zend == پوچ)
		    برو theend;
		s1->out= skip;
		patch(e.out, zend);
		افزودن_2(frag(s, list1(&skip->out)));
	    }
	    وگرنه
	    {
		patch(e.out, s1);
		افزودن_2(frag(s, list1(&s1->out)));
		اگر (before)
		{
		    اگر (n <= 0)
			// See if we can guess the maximum width, it avoids a
			// lot of pointless tries.
			n = nfa_max_width(e.start, 0);
		    s->val = n; // store the count
		}
	    }
	    بشکن;
	  }

	حالت NFA_COMPOSING:	// char with composing char
#اگر 0
	    // TODO
	    اگر (regflags & RF_ICOMBINE)
	    {
		// use the base character only
	    }
#endif
	    // FALLTHROUGH

	حالت NFA_MOPEN:	// \( \) Submatch
	حالت NFA_MOPEN1:
	حالت NFA_MOPEN2:
	حالت NFA_MOPEN3:
	حالت NFA_MOPEN4:
	حالت NFA_MOPEN5:
	حالت NFA_MOPEN6:
	حالت NFA_MOPEN7:
	حالت NFA_MOPEN8:
	حالت NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	حالت NFA_ZOPEN:	// \z( \) Submatch
	حالت NFA_ZOPEN1:
	حالت NFA_ZOPEN2:
	حالت NFA_ZOPEN3:
	حالت NFA_ZOPEN4:
	حالت NFA_ZOPEN5:
	حالت NFA_ZOPEN6:
	حالت NFA_ZOPEN7:
	حالت NFA_ZOPEN8:
	حالت NFA_ZOPEN9:
#endif
	حالت NFA_NOPEN:	// \%( \) "Invisible Submatch"
	    اگر (nfa_calc_size == TRUE)
	    {
		nstate += 2;
		بشکن;
	    }

	    mopen = *p;
	    انتخاب (*p)
	    {
		حالت NFA_NOPEN: mclose = NFA_NCLOSE; بشکن;
#ifdef FEAT_SYN_HL
		حالت NFA_ZOPEN: mclose = NFA_ZCLOSE; بشکن;
		حالت NFA_ZOPEN1: mclose = NFA_ZCLOSE1; بشکن;
		حالت NFA_ZOPEN2: mclose = NFA_ZCLOSE2; بشکن;
		حالت NFA_ZOPEN3: mclose = NFA_ZCLOSE3; بشکن;
		حالت NFA_ZOPEN4: mclose = NFA_ZCLOSE4; بشکن;
		حالت NFA_ZOPEN5: mclose = NFA_ZCLOSE5; بشکن;
		حالت NFA_ZOPEN6: mclose = NFA_ZCLOSE6; بشکن;
		حالت NFA_ZOPEN7: mclose = NFA_ZCLOSE7; بشکن;
		حالت NFA_ZOPEN8: mclose = NFA_ZCLOSE8; بشکن;
		حالت NFA_ZOPEN9: mclose = NFA_ZCLOSE9; بشکن;
#endif
		حالت NFA_COMPOSING: mclose = NFA_END_COMPOSING; بشکن;
		پیشفرض:
		    // NFA_MOPEN, NFA_MOPEN1 .. NFA_MOPEN9
		    mclose = *p + NSUBEXP;
		    بشکن;
	    }

	    // Allow "NFA_MOPEN" as a valid postfix representation for
	    // the empty regexp "". In this case, the NFA will be
	    // NFA_MOPEN -> NFA_MCLOSE. Note that this also allows
	    // empty groups of parenthesis, and empty mbyte chars
	    اگر (stackp == پشته)
	    {
		s = alloc_state(mopen, پوچ, پوچ);
		اگر (s == پوچ)
		    برو theend;
		s1 = alloc_state(mclose, پوچ, پوچ);
		اگر (s1 == پوچ)
		    برو theend;
		patch(list1(&s->out), s1);
		افزودن_2(frag(s, list1(&s1->out)));
		بشکن;
	    }

	    // At least one node was emitted before NFA_MOPEN, so
	    // at least one node will be between NFA_MOPEN and NFA_MCLOSE
	    e = برداشت_2();
	    s = alloc_state(mopen, e.start, پوچ);   // `('
	    اگر (s == پوچ)
		برو theend;

	    s1 = alloc_state(mclose, پوچ, پوچ);   // `)'
	    اگر (s1 == پوچ)
		برو theend;
	    patch(e.out, s1);

	    اگر (mopen == NFA_COMPOSING)
		// COMPOSING->out1 = END_COMPOSING
		patch(list1(&s->out1), s1);

	    افزودن_2(frag(s, list1(&s1->out)));
	    بشکن;

	حالت NFA_BACKREF1:
	حالت NFA_BACKREF2:
	حالت NFA_BACKREF3:
	حالت NFA_BACKREF4:
	حالت NFA_BACKREF5:
	حالت NFA_BACKREF6:
	حالت NFA_BACKREF7:
	حالت NFA_BACKREF8:
	حالت NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	حالت NFA_ZREF1:
	حالت NFA_ZREF2:
	حالت NFA_ZREF3:
	حالت NFA_ZREF4:
	حالت NFA_ZREF5:
	حالت NFA_ZREF6:
	حالت NFA_ZREF7:
	حالت NFA_ZREF8:
	حالت NFA_ZREF9:
#endif
	    اگر (nfa_calc_size == TRUE)
	    {
		nstate += 2;
		بشکن;
	    }
	    s = alloc_state(*p, پوچ, پوچ);
	    اگر (s == پوچ)
		برو theend;
	    s1 = alloc_state(NFA_SKIP, پوچ, پوچ);
	    اگر (s1 == پوچ)
		برو theend;
	    patch(list1(&s->out), s1);
	    افزودن_2(frag(s, list1(&s1->out)));
	    بشکن;

	حالت NFA_LNUM:
	حالت NFA_LNUM_GT:
	حالت NFA_LNUM_LT:
	حالت NFA_VCOL:
	حالت NFA_VCOL_GT:
	حالت NFA_VCOL_LT:
	حالت NFA_COL:
	حالت NFA_COL_GT:
	حالت NFA_COL_LT:
	حالت NFA_MARK:
	حالت NFA_MARK_GT:
	حالت NFA_MARK_LT:
	  {
	    صحیح n = *++p; // lnum, col or mark name

	    اگر (nfa_calc_size == TRUE)
	    {
		nstate += 1;
		بشکن;
	    }
	    s = alloc_state(p[-1], پوچ, پوچ);
	    اگر (s == پوچ)
		برو theend;
	    s->val = n;
	    افزودن_2(frag(s, list1(&s->out)));
	    بشکن;
	  }

	حالت NFA_ZSTART:
	حالت NFA_ZEND:
	پیشفرض:
	    // Operands
	    اگر (nfa_calc_size == TRUE)
	    {
		nstate++;
		بشکن;
	    }
	    s = alloc_state(*p, پوچ, پوچ);
	    اگر (s == پوچ)
		برو theend;
	    افزودن_2(frag(s, list1(&s->out)));
	    بشکن;

	} // switch(*p)

    } // for (p = postfix; *p; ++p)

    اگر (nfa_calc_size == TRUE)
    {
	nstate++;
	برو theend;	// Return value when counting size is ignored anyway
    }

    e = برداشت_2();
    اگر (stackp != پشته)
	EMSG_RET_NULL(_(e_nfa_regexp_while_converting_from_postfix_to_nfa_too_many_stats_چپ_on_پشته));

    اگر (istate >= nstate)
	EMSG_RET_NULL(_(e_nfa_regexp_not_enough_space_to_store_whole_nfa));

    matchstate = &state_ptr[istate++]; // the match state
    matchstate->c = NFA_MATCH;
    matchstate->out = matchstate->out1 = پوچ;
    matchstate->id = 0;

    patch(e.out, matchstate);
    ret = e.start;

theend:
    بازگشت ret;

#undef POP1
#undef PUSH1
#undef POP2
#undef PUSH2
#undef برداشت_2
#undef افزودن_2
}

/*
 * After building the NFA program, inspect it to add optimization hints.
 */
    ایستا تهی
nfa_postprocess(nfa_regprog_T *prog)
{
    صحیح i;
    صحیح c;

    برای (i = 0; i < prog->nstate; ++i)
    {
	c = prog->state[i].c;
	اگر (c == NFA_START_INVISIBLE
		|| c == NFA_START_INVISIBLE_NEG
		|| c == NFA_START_INVISIBLE_BEFORE
		|| c == NFA_START_INVISIBLE_BEFORE_NEG)
	{
	    صحیح directly;

	    // Do it directly when what follows is possibly the end of the
	    // match.
	    اگر (match_follows(prog->state[i].out1->out, 0))
		directly = TRUE;
	    وگرنه
	    {
		صحیح ch_invisible = failure_chance(prog->state[i].out, 0);
		صحیح ch_follows = failure_chance(prog->state[i].out1->out, 0);

		// Postpone when the invisible match is expensive or has a
		// lower chance of failing.
		اگر (c == NFA_START_INVISIBLE_BEFORE
		     || c == NFA_START_INVISIBLE_BEFORE_NEG)
		{
		    // "before" matches are very expensive when
		    // unbounded, always prefer what follows then,
		    // unless what follows will always match.
		    // Otherwise strongly prefer what follows.
		    اگر (prog->state[i].val <= 0 && ch_follows > 0)
			directly = FALSE;
		    وگرنه
			directly = ch_follows * 10 < ch_invisible;
		}
		وگرنه
		{
		    // normal invisible, first do the one with the
		    // highest failure chance
		    directly = ch_follows < ch_invisible;
		}
	    }
	    اگر (directly)
		// switch to the _FIRST state
		++prog->state[i].c;
	}
    }
}

/////////////////////////////////////////////////////////////////
// NFA execution code.
/////////////////////////////////////////////////////////////////

تعریفنوع ساختار
{
    صحیح	    in_use; // number of subexpr with useful info

    // When REG_MULTI is TRUE list.multi is used, otherwise list.line.
    اجتماع
    {
	ساختار multipos
	{
	    linenr_T	start_lnum;
	    linenr_T	end_lnum;
	    colnr_T	start_col;
	    colnr_T	end_col;
	} multi[NSUBEXP];
	ساختار linepos
	{
	    char_u	*start;
	    char_u	*end;
	} سطر[NSUBEXP];
    } list;
    colnr_T	orig_start_col;  // list.multi[0].start_col without \zs
} regsub_T;

تعریفنوع ساختار
{
    regsub_T	norm; // \( .. \) matches
#ifdef FEAT_SYN_HL
    regsub_T	synt; // \z( .. \) matches
#endif
} regsubs_T;

// nfa_pim_T stores a Postponed Invisible Match.
تعریفنوع ساختار nfa_pim_S nfa_pim_T;
ساختار nfa_pim_S
{
    صحیح		result;		// NFA_PIM_*, see below
    nfa_state_T	*state;		// the invisible match start state
    regsubs_T	subs;		// submatch info, only party used
    اجتماع
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
تعریفنوع ساختار
{
    nfa_state_T	*state;
    صحیح		شمار;
    nfa_pim_T	pim;		// if pim.result != NFA_PIM_UNUSED: postponed
				// invisible match
    regsubs_T	subs;		// submatch info, only party used
} nfa_thread_T;

// nfa_list_T contains the alternative NFA execution states.
تعریفنوع ساختار
{
    nfa_thread_T    *t;		// allocated array of states
    صحیح		    n;		// nr of states currently in "t"
    صحیح		    len;	// max nr of states in "t"
    صحیح		    id;		// ID of the list
    صحیح		    has_pim;	// TRUE when any state has a PIM
} nfa_list_T;

#ifdef ENABLE_LOG
ایستا تهی log_subexpr(regsub_T *sub);

    ایستا تهی
log_subsexpr(regsubs_T *subs)
{
    log_subexpr(&subs->norm);
# ifdef FEAT_SYN_HL
    اگر (rex.nfa_has_zsubexpr)
	log_subexpr(&subs->synt);
# endif
}

    ایستا تهی
log_subexpr(regsub_T *sub)
{
    صحیح j;

    برای (j = 0; j < sub->in_use; j++)
	اگر (REG_MULTI)
	    fprintf(log_fd,
		    "*** group %d, start: c=%d, l=%d, end: c=%d, l=%d\n",
		    j,
		    sub->list.multi[j].start_col,
		    (صحیح)sub->list.multi[j].start_lnum,
		    sub->list.multi[j].end_col,
		    (صحیح)sub->list.multi[j].end_lnum);
	وگرنه
	{
	    نویسه *s = (نویسه *)sub->list.سطر[j].start;
	    نویسه *e = (نویسه *)sub->list.سطر[j].end;

	    fprintf(log_fd, "*** group %d, start: \"%s\", end: \"%s\"\n",
		    j,
		    s == پوچ ? "NULL" : s,
		    e == پوچ ? "NULL" : e);
	}
}

    ایستا نویسه *
pim_info(nfa_pim_T *pim)
{
    ایستا نویسه buf[30];

    اگر (pim == پوچ || pim->result == NFA_PIM_UNUSED)
	buf[0] = NUL;
    وگرنه
    {
	sprintf(buf, " PIM col %d", REG_MULTI ? (صحیح)pim->end.pos.col
		: (صحیح)(pim->end.ptr - rex.درون_داد));
    }
    بازگشت buf;
}

#endif

// Used during execution: whether a match has been found.
ایستا صحیح	    nfa_match;
#ifdef FEAT_RELTIME
ایستا صحیح	   *nfa_timed_out;
#endif

ایستا درونخطی تهی copy_sub(regsub_T *to, regsub_T *from);
ایستا صحیح pim_equal(nfa_pim_T *one, nfa_pim_T *two);

/*
 * Copy postponed invisible match info from "from" to "to".
 */
    ایستا تهی
copy_pim(nfa_pim_T *to, nfa_pim_T *from)
{
    to->result = from->result;
    to->state = from->state;
    copy_sub(&to->subs.norm, &from->subs.norm);
#ifdef FEAT_SYN_HL
    اگر (rex.nfa_has_zsubexpr)
	copy_sub(&to->subs.synt, &from->subs.synt);
#endif
    to->end = from->end;
}

    ایستا تهی
clear_sub(regsub_T *sub)
{
    اگر (REG_MULTI)
	// Use 0xff to set lnum to -1
	vim_memset(sub->list.multi, 0xff,
				  اندازه(ساختار multipos) * rex.nfa_nsubexpr);
    وگرنه
	vim_memset(sub->list.سطر, 0,
				   اندازه(ساختار linepos) * rex.nfa_nsubexpr);
    sub->in_use = 0;
}

/*
 * Copy the submatches from "from" to "to".
 */
    ایستا درونخطی تهی
copy_sub(regsub_T *to, regsub_T *from)
{
    to->in_use = from->in_use;
    اگر (from->in_use <= 0)
	بازگشت;

    // Copy the match start and end positions.  addstate() calls this for
    // every state it adds, so it is very hot.  Patterns without capturing
    // groups only use subexpression zero (the whole match), so special-case
    // "in_use == 1" with a direct assignment to avoid the mch_memmove() call.
    اگر (REG_MULTI)
    {
	اگر (from->in_use == 1)
	    to->list.multi[0] = from->list.multi[0];
	وگرنه
	    mch_memmove(&to->list.multi[0],
		    &from->list.multi[0],
		    اندازه(ساختار multipos) * from->in_use);
	to->orig_start_col = from->orig_start_col;
    }
    وگرنه
    {
	اگر (from->in_use == 1)
	    to->list.سطر[0] = from->list.سطر[0];
	وگرنه
	    mch_memmove(&to->list.سطر[0],
		    &from->list.سطر[0],
		    اندازه(ساختار linepos) * from->in_use);
    }
}

/*
 * Like copy_sub() but exclude the main match.
 */
    ایستا تهی
copy_sub_off(regsub_T *to, regsub_T *from)
{
    اگر (to->in_use < from->in_use)
	to->in_use = from->in_use;
    اگر (from->in_use <= 1)
	بازگشت;

    // Copy the match start and end positions.
    اگر (REG_MULTI)
	mch_memmove(&to->list.multi[1],
		&from->list.multi[1],
		اندازه(ساختار multipos) * (from->in_use - 1));
    وگرنه
	mch_memmove(&to->list.سطر[1],
		&from->list.سطر[1],
		اندازه(ساختار linepos) * (from->in_use - 1));
}

/*
 * Like copy_sub() but only do the end of the main match if \ze is present.
 */
    ایستا تهی
copy_ze_off(regsub_T *to, regsub_T *from)
{
    اگر (!rex.nfa_has_zend)
	بازگشت;

    اگر (REG_MULTI)
    {
	اگر (from->list.multi[0].end_lnum >= 0)
	{
	    to->list.multi[0].end_lnum = from->list.multi[0].end_lnum;
	    to->list.multi[0].end_col = from->list.multi[0].end_col;
	}
    }
    وگرنه
    {
	اگر (from->list.سطر[0].end != پوچ)
	    to->list.سطر[0].end = from->list.سطر[0].end;
    }
}

/*
 * Return TRUE if "sub1" and "sub2" have the same start positions.
 * When using back-references also check the end position.
 */
    ایستا صحیح
sub_equal(regsub_T *sub1, regsub_T *sub2)
{
    صحیح		i;
    صحیح		todo;
    linenr_T	s1;
    linenr_T	s2;
    char_u	*sp1;
    char_u	*sp2;

    todo = sub1->in_use > sub2->in_use ? sub1->in_use : sub2->in_use;
    اگر (REG_MULTI)
    {
	برای (i = 0; i < todo; ++i)
	{
	    اگر (i < sub1->in_use)
		s1 = sub1->list.multi[i].start_lnum;
	    وگرنه
		s1 = -1;
	    اگر (i < sub2->in_use)
		s2 = sub2->list.multi[i].start_lnum;
	    وگرنه
		s2 = -1;
	    اگر (s1 != s2)
		بازگشت FALSE;
	    اگر (s1 != -1 && sub1->list.multi[i].start_col
					     != sub2->list.multi[i].start_col)
		بازگشت FALSE;

	    اگر (rex.nfa_has_backref)
	    {
		اگر (i < sub1->in_use)
		    s1 = sub1->list.multi[i].end_lnum;
		وگرنه
		    s1 = -1;
		اگر (i < sub2->in_use)
		    s2 = sub2->list.multi[i].end_lnum;
		وگرنه
		    s2 = -1;
		اگر (s1 != s2)
		    بازگشت FALSE;
		اگر (s1 != -1 && sub1->list.multi[i].end_col
					       != sub2->list.multi[i].end_col)
		بازگشت FALSE;
	    }
	}
    }
    وگرنه
    {
	برای (i = 0; i < todo; ++i)
	{
	    اگر (i < sub1->in_use)
		sp1 = sub1->list.سطر[i].start;
	    وگرنه
		sp1 = پوچ;
	    اگر (i < sub2->in_use)
		sp2 = sub2->list.سطر[i].start;
	    وگرنه
		sp2 = پوچ;
	    اگر (sp1 != sp2)
		بازگشت FALSE;
	    اگر (rex.nfa_has_backref)
	    {
		اگر (i < sub1->in_use)
		    sp1 = sub1->list.سطر[i].end;
		وگرنه
		    sp1 = پوچ;
		اگر (i < sub2->in_use)
		    sp2 = sub2->list.سطر[i].end;
		وگرنه
		    sp2 = پوچ;
		اگر (sp1 != sp2)
		    بازگشت FALSE;
	    }
	}
    }

    بازگشت TRUE;
}

#ifdef FEAT_RELTIME
/*
 * Check if we are past the time limit, if there is one.
 */
    ایستا صحیح
nfa_did_time_out(تهی)
{
    اگر (*timeout_flag)
    {
	اگر (nfa_timed_out != پوچ)
	{
# ifdef FEAT_EVAL
	    اگر (!*nfa_timed_out)
		ch_log(پوچ, "NFA regexp timed out");
# endif
	    *nfa_timed_out = TRUE;
	}
	بازگشت TRUE;
    }
    بازگشت FALSE;
}
#endif

#ifdef ENABLE_LOG
    ایستا تهی
open_debug_log(صحیح result)
{
    log_fd = fopen(NFA_REGEXP_RUN_LOG, "a");
    اگر (log_fd == پوچ)
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

    ایستا تهی
report_state(نویسه *action,
	     regsub_T *sub,
	     nfa_state_T *state,
	     صحیح lid,
	     nfa_pim_T *pim)
{
    صحیح col;

    اگر (sub->in_use <= 0)
	col = -1;
    وگرنه اگر (REG_MULTI)
	col = sub->list.multi[0].start_col;
    وگرنه
	col = (صحیح)(sub->list.سطر[0].start - rex.سطر);
    nfa_set_code(state->c);
    اگر (log_fd == پوچ)
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
    ایستا صحیح
has_state_with_pos(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim)	// postponed match or NULL
{
    nfa_thread_T	*thread;
    صحیح			i;

    برای (i = 0; i < l->n; ++i)
    {
	thread = &l->t[i];
	اگر (thread->state->id == state->id
		&& sub_equal(&thread->subs.norm, &subs->norm)
#ifdef FEAT_SYN_HL
		&& (!rex.nfa_has_zsubexpr
				|| sub_equal(&thread->subs.synt, &subs->synt))
#endif
		&& pim_equal(&thread->pim, pim))
	    بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * Return TRUE if "one" and "two" are equal.  That includes when both are not
 * set.
 */
    ایستا صحیح
pim_equal(nfa_pim_T *one, nfa_pim_T *two)
{
    صحیح one_unused = (one == پوچ || one->result == NFA_PIM_UNUSED);
    صحیح two_unused = (two == پوچ || two->result == NFA_PIM_UNUSED);

    اگر (one_unused)
	// one is unused: equal when two is also unused
	بازگشت two_unused;
    اگر (two_unused)
	// one is used and two is not: not equal
	بازگشت FALSE;
    // compare the state id
    اگر (one->state->id != two->state->id)
	بازگشت FALSE;
    // compare the position
    اگر (REG_MULTI)
	بازگشت one->end.pos.lnum == two->end.pos.lnum
	    && one->end.pos.col == two->end.pos.col;
    بازگشت one->end.ptr == two->end.ptr;
}

/*
 * Return TRUE if "state" leads to a NFA_MATCH without advancing the input.
 */
    ایستا صحیح
match_follows(nfa_state_T *startstate, صحیح depth)
{
    nfa_state_T	    *state = startstate;

    // avoid too much recursion
    اگر (depth > 10)
	بازگشت FALSE;

    وقتی (state != پوچ)
    {
	انتخاب (state->c)
	{
	    حالت NFA_MATCH:
	    حالت NFA_MCLOSE:
	    حالت NFA_END_INVISIBLE:
	    حالت NFA_END_INVISIBLE_NEG:
	    حالت NFA_END_PATTERN:
		بازگشت TRUE;

	    حالت NFA_SPLIT:
		بازگشت match_follows(state->out, depth + 1)
				     || match_follows(state->out1, depth + 1);

	    حالت NFA_START_INVISIBLE:
	    حالت NFA_START_INVISIBLE_FIRST:
	    حالت NFA_START_INVISIBLE_BEFORE:
	    حالت NFA_START_INVISIBLE_BEFORE_FIRST:
	    حالت NFA_START_INVISIBLE_NEG:
	    حالت NFA_START_INVISIBLE_NEG_FIRST:
	    حالت NFA_START_INVISIBLE_BEFORE_NEG:
	    حالت NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
	    حالت NFA_COMPOSING:
		// skip ahead to next state
		state = state->out1->out;
		ادامه;

	    حالت NFA_ANY:
	    حالت NFA_ANY_COMPOSING:
	    حالت NFA_IDENT:
	    حالت NFA_SIDENT:
	    حالت NFA_KWORD:
	    حالت NFA_SKWORD:
	    حالت NFA_FNAME:
	    حالت NFA_SFNAME:
	    حالت NFA_PRINT:
	    حالت NFA_SPRINT:
	    حالت NFA_WHITE:
	    حالت NFA_NWHITE:
	    حالت NFA_DIGIT:
	    حالت NFA_NDIGIT:
	    حالت NFA_HEX:
	    حالت NFA_NHEX:
	    حالت NFA_OCTAL:
	    حالت NFA_NOCTAL:
	    حالت NFA_WORD:
	    حالت NFA_NWORD:
	    حالت NFA_HEAD:
	    حالت NFA_NHEAD:
	    حالت NFA_ALPHA:
	    حالت NFA_NALPHA:
	    حالت NFA_LOWER:
	    حالت NFA_NLOWER:
	    حالت NFA_UPPER:
	    حالت NFA_NUPPER:
	    حالت NFA_LOWER_IC:
	    حالت NFA_NLOWER_IC:
	    حالت NFA_UPPER_IC:
	    حالت NFA_NUPPER_IC:
	    حالت NFA_START_COLL:
	    حالت NFA_START_NEG_COLL:
	    حالت NFA_NEWL:
		// state will advance input
		بازگشت FALSE;

	    پیشفرض:
		اگر (state->c > 0)
		    // state will advance input
		    بازگشت FALSE;

		// Others: zero-width or possibly zero-width, might still find
		// a match at the same position, keep looking.
		بشکن;
	}
	state = state->out;
    }
    بازگشت FALSE;
}


/*
 * Return TRUE if "state" is already in list "l".
 */
    ایستا صحیح
state_in_list(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs)	// pointers to subexpressions
{
    اگر (state->lastlist[nfa_ll_index] == l->id)
    {
	اگر (!rex.nfa_has_backref || has_state_with_pos(l, state, subs, پوچ))
	    بازگشت TRUE;
    }
    بازگشت FALSE;
}

// Offset used for "off" by addstate_here().
#define ADDSTATE_HERE_OFFSET 10

/*
 * Add "state" and possibly what follows to state list ".".
 * Returns "subs_arg", possibly copied into temp_subs.
 * Returns NULL when recursiveness is too deep or timed out.
 */
    ایستا regsubs_T *
addstate(
    nfa_list_T		*l,	    // runtime state list
    nfa_state_T		*state,	    // state to update
    regsubs_T		*subs_arg,  // pointers to subexpressions
    nfa_pim_T		*pim,	    // postponed look-behind match
    صحیح			off_arg,    // byte offset, when -1 go to next line
    صحیح			depth)	    // recursion depth
{
    صحیح			subidx;
    صحیح			off = off_arg;
    صحیح			add_here = FALSE;
    صحیح			listindex = 0;
    صحیح			k;
    صحیح			found = FALSE;
    nfa_thread_T	*thread;
    ساختار multipos	save_multipos;
    صحیح			save_in_use;
    char_u		*save_ptr;
    صحیح			i;
    regsub_T		*sub;
    regsubs_T		*subs = subs_arg;
    ایستا regsubs_T	temp_subs;
#ifdef ENABLE_LOG
    صحیح			did_print = FALSE;
#endif

#ifdef FEAT_RELTIME
    اگر (nfa_did_time_out())
	بازگشت پوچ;
#endif

    // This function is called recursively.  When the depth is too much we run
    // out of stack and crash, limit recursiveness here.
    اگر (depth >= 5000 || subs == پوچ)
	بازگشت پوچ;

    اگر (off_arg <= -ADDSTATE_HERE_OFFSET)
    {
	add_here = TRUE;
	off = 0;
	listindex = -(off_arg + ADDSTATE_HERE_OFFSET);
    }

    انتخاب (state->c)
    {
	حالت NFA_NCLOSE:
	حالت NFA_MCLOSE:
	حالت NFA_MCLOSE1:
	حالت NFA_MCLOSE2:
	حالت NFA_MCLOSE3:
	حالت NFA_MCLOSE4:
	حالت NFA_MCLOSE5:
	حالت NFA_MCLOSE6:
	حالت NFA_MCLOSE7:
	حالت NFA_MCLOSE8:
	حالت NFA_MCLOSE9:
#ifdef FEAT_SYN_HL
	حالت NFA_ZCLOSE:
	حالت NFA_ZCLOSE1:
	حالت NFA_ZCLOSE2:
	حالت NFA_ZCLOSE3:
	حالت NFA_ZCLOSE4:
	حالت NFA_ZCLOSE5:
	حالت NFA_ZCLOSE6:
	حالت NFA_ZCLOSE7:
	حالت NFA_ZCLOSE8:
	حالت NFA_ZCLOSE9:
#endif
	حالت NFA_MOPEN:
	حالت NFA_ZEND:
	حالت NFA_SPLIT:
	حالت NFA_EMPTY:
	    // These nodes are not added themselves but their "out" and/or
	    // "out1" may be added below.
	    بشکن;

	حالت NFA_BOL:
	حالت NFA_BOF:
	    // "^" won't match past end-of-line, don't bother trying.
	    // Except when at the end of the line, or when we are going to the
	    // next line for a look-behind match.
	    اگر (rex.درون_داد > rex.سطر
		    && *rex.درون_داد != NUL
		    && (nfa_endp == پوچ
			|| !REG_MULTI
			|| rex.lnum == nfa_endp->se_u.pos.lnum))
		برو skip_add;
	    // FALLTHROUGH

	حالت NFA_MOPEN1:
	حالت NFA_MOPEN2:
	حالت NFA_MOPEN3:
	حالت NFA_MOPEN4:
	حالت NFA_MOPEN5:
	حالت NFA_MOPEN6:
	حالت NFA_MOPEN7:
	حالت NFA_MOPEN8:
	حالت NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	حالت NFA_ZOPEN:
	حالت NFA_ZOPEN1:
	حالت NFA_ZOPEN2:
	حالت NFA_ZOPEN3:
	حالت NFA_ZOPEN4:
	حالت NFA_ZOPEN5:
	حالت NFA_ZOPEN6:
	حالت NFA_ZOPEN7:
	حالت NFA_ZOPEN8:
	حالت NFA_ZOPEN9:
#endif
	حالت NFA_NOPEN:
	حالت NFA_ZSTART:
	    // These nodes need to be added so that we can bail out when it
	    // was added to this list before at the same position to avoid an
	    // endless loop for "\(\)*"

	پیشفرض:
	    اگر (state->lastlist[nfa_ll_index] == l->id && state->c != NFA_SKIP)
	    {
		// This state is already in the list, don't add it again,
		// unless it is an MOPEN that is used for a backreference or
		// when there is a PIM. For NFA_MATCH check the position,
		// lower position is preferred.
		اگر (!rex.nfa_has_backref && pim == پوچ && !l->has_pim
						     && state->c != NFA_MATCH)
		{
		    // When called from addstate_here() do insert before
		    // existing states.
		    اگر (add_here)
		    {
			برای (k = 0; k < l->n && k < listindex; ++k)
			    اگر (l->t[k].state->id == state->id)
			    {
				found = TRUE;
				بشکن;
			    }
		    }
		    اگر (!add_here || found)
		    {
skip_add:
#ifdef ENABLE_LOG
			nfa_set_code(state->c);
			fprintf(log_fd, "> Not adding state %d to list %d. char %d: %s pim: %s has_pim: %d found: %d\n",
			    abs(state->id), l->id, state->c, code,
			    pim == پوچ ? "NULL" : "yes", l->has_pim, found);
#endif
			بازگشت subs;
		    }
		}

		// Do not add the state again when it exists with the same
		// positions.
		اگر (has_state_with_pos(l, state, subs, pim))
		    برو skip_add;
	    }

	    // When there are backreferences or PIMs the number of states may
	    // be (a lot) bigger than anticipated.
	    اگر (l->n == l->len)
	    {
		صحیح		newlen = l->len * 3 / 2 + 50;
		size_t		newsize = newlen * اندازه(nfa_thread_T);
		nfa_thread_T	*newt;

		اگر ((long)(newsize >> 10) >= p_mmp)
		{
		    emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		    بازگشت پوچ;
		}
		اگر (subs != &temp_subs)
		{
		    // "subs" may point into the current array, need to make a
		    // copy before it becomes invalid.
		    copy_sub(&temp_subs.norm, &subs->norm);
#ifdef FEAT_SYN_HL
		    اگر (rex.nfa_has_zsubexpr)
			copy_sub(&temp_subs.synt, &subs->synt);
#endif
		    subs = &temp_subs;
		}

		newt = vim_realloc(l->t, newsize);
		اگر (newt == پوچ)
		{
		    // out of memory
		    بازگشت پوچ;
		}
		l->t = newt;
		l->len = newlen;
	    }

	    // add the state to the list
	    state->lastlist[nfa_ll_index] = l->id;
	    thread = &l->t[l->n++];
	    thread->state = state;
	    اگر (pim == پوچ)
		thread->pim.result = NFA_PIM_UNUSED;
	    وگرنه
	    {
		copy_pim(&thread->pim, pim);
		l->has_pim = TRUE;
	    }
	    copy_sub(&thread->subs.norm, &subs->norm);
#ifdef FEAT_SYN_HL
	    اگر (rex.nfa_has_zsubexpr)
		copy_sub(&thread->subs.synt, &subs->synt);
#endif
#ifdef ENABLE_LOG
	    report_state("Adding", &thread->subs.norm, state, l->id, pim);
	    did_print = TRUE;
#endif
    }

#ifdef ENABLE_LOG
    اگر (!did_print)
	report_state("Processing", &subs->norm, state, l->id, pim);
#endif
    انتخاب (state->c)
    {
	حالت NFA_MATCH:
	    بشکن;

	حالت NFA_SPLIT:
	    // order matters here
	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    subs = addstate(l, state->out1, subs, pim, off_arg, depth + 1);
	    بشکن;

	حالت NFA_EMPTY:
	حالت NFA_NOPEN:
	حالت NFA_NCLOSE:
	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    بشکن;

	حالت NFA_MOPEN:
	حالت NFA_MOPEN1:
	حالت NFA_MOPEN2:
	حالت NFA_MOPEN3:
	حالت NFA_MOPEN4:
	حالت NFA_MOPEN5:
	حالت NFA_MOPEN6:
	حالت NFA_MOPEN7:
	حالت NFA_MOPEN8:
	حالت NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	حالت NFA_ZOPEN:
	حالت NFA_ZOPEN1:
	حالت NFA_ZOPEN2:
	حالت NFA_ZOPEN3:
	حالت NFA_ZOPEN4:
	حالت NFA_ZOPEN5:
	حالت NFA_ZOPEN6:
	حالت NFA_ZOPEN7:
	حالت NFA_ZOPEN8:
	حالت NFA_ZOPEN9:
#endif
	حالت NFA_ZSTART:
	    اگر (state->c == NFA_ZSTART)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#ifdef FEAT_SYN_HL
	    وگرنه اگر (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
	    {
		subidx = state->c - NFA_ZOPEN;
		sub = &subs->synt;
	    }
#endif
	    وگرنه
	    {
		subidx = state->c - NFA_MOPEN;
		sub = &subs->norm;
	    }

	    // avoid compiler warnings
	    save_ptr = پوچ;
	    CLEAR_FIELD(save_multipos);

	    // Set the position (with "off" added) in the subexpression.  Save
	    // and restore it when it was in use.  Otherwise fill any gap.
	    اگر (REG_MULTI)
	    {
		اگر (subidx < sub->in_use)
		{
		    save_multipos = sub->list.multi[subidx];
		    save_in_use = -1;
		}
		وگرنه
		{
		    save_in_use = sub->in_use;
		    برای (i = sub->in_use; i < subidx; ++i)
		    {
			sub->list.multi[i].start_lnum = -1;
			sub->list.multi[i].end_lnum = -1;
		    }
		    sub->in_use = subidx + 1;
		}
		اگر (off == -1)
		{
		    sub->list.multi[subidx].start_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].start_col = 0;
		}
		وگرنه
		{
		    sub->list.multi[subidx].start_lnum = rex.lnum;
		    sub->list.multi[subidx].start_col =
					 (colnr_T)(rex.درون_داد - rex.سطر + off);
		}
		sub->list.multi[subidx].end_lnum = -1;
	    }
	    وگرنه
	    {
		اگر (subidx < sub->in_use)
		{
		    save_ptr = sub->list.سطر[subidx].start;
		    save_in_use = -1;
		}
		وگرنه
		{
		    save_in_use = sub->in_use;
		    برای (i = sub->in_use; i < subidx; ++i)
		    {
			sub->list.سطر[i].start = پوچ;
			sub->list.سطر[i].end = پوچ;
		    }
		    sub->in_use = subidx + 1;
		}
		sub->list.سطر[subidx].start = rex.درون_داد + off;
	    }

	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    اگر (subs == پوچ)
		بشکن;
	    // "subs" may have changed, need to set "sub" again
#ifdef FEAT_SYN_HL
	    اگر (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
		sub = &subs->synt;
	    وگرنه
#endif
		sub = &subs->norm;

	    اگر (save_in_use == -1)
	    {
		اگر (REG_MULTI)
		    sub->list.multi[subidx] = save_multipos;
		وگرنه
		    sub->list.سطر[subidx].start = save_ptr;
	    }
	    وگرنه
		sub->in_use = save_in_use;
	    بشکن;

	حالت NFA_MCLOSE:
	    اگر (rex.nfa_has_zend && (REG_MULTI
			? subs->norm.list.multi[0].end_lnum >= 0
			: subs->norm.list.سطر[0].end != پوچ))
	    {
		// Do not overwrite the position set by \ze.
		subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
		بشکن;
	    }
	    // FALLTHROUGH
	حالت NFA_MCLOSE1:
	حالت NFA_MCLOSE2:
	حالت NFA_MCLOSE3:
	حالت NFA_MCLOSE4:
	حالت NFA_MCLOSE5:
	حالت NFA_MCLOSE6:
	حالت NFA_MCLOSE7:
	حالت NFA_MCLOSE8:
	حالت NFA_MCLOSE9:
#ifdef FEAT_SYN_HL
	حالت NFA_ZCLOSE:
	حالت NFA_ZCLOSE1:
	حالت NFA_ZCLOSE2:
	حالت NFA_ZCLOSE3:
	حالت NFA_ZCLOSE4:
	حالت NFA_ZCLOSE5:
	حالت NFA_ZCLOSE6:
	حالت NFA_ZCLOSE7:
	حالت NFA_ZCLOSE8:
	حالت NFA_ZCLOSE9:
#endif
	حالت NFA_ZEND:
	    اگر (state->c == NFA_ZEND)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#ifdef FEAT_SYN_HL
	    وگرنه اگر (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
	    {
		subidx = state->c - NFA_ZCLOSE;
		sub = &subs->synt;
	    }
#endif
	    وگرنه
	    {
		subidx = state->c - NFA_MCLOSE;
		sub = &subs->norm;
	    }

	    // We don't fill in gaps here, there must have been an MOPEN that
	    // has done that.
	    save_in_use = sub->in_use;
	    اگر (sub->in_use <= subidx)
		sub->in_use = subidx + 1;
	    اگر (REG_MULTI)
	    {
		save_multipos = sub->list.multi[subidx];
		اگر (off == -1)
		{
		    sub->list.multi[subidx].end_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].end_col = 0;
		}
		وگرنه
		{
		    sub->list.multi[subidx].end_lnum = rex.lnum;
		    sub->list.multi[subidx].end_col =
					  (colnr_T)(rex.درون_داد - rex.سطر + off);
		}
		// avoid compiler warnings
		save_ptr = پوچ;
	    }
	    وگرنه
	    {
		save_ptr = sub->list.سطر[subidx].end;
		sub->list.سطر[subidx].end = rex.درون_داد + off;
		// avoid compiler warnings
		CLEAR_FIELD(save_multipos);
	    }

	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    اگر (subs == پوچ)
		بشکن;
	    // "subs" may have changed, need to set "sub" again
#ifdef FEAT_SYN_HL
	    اگر (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
		sub = &subs->synt;
	    وگرنه
#endif
		sub = &subs->norm;

	    اگر (REG_MULTI)
		sub->list.multi[subidx] = save_multipos;
	    وگرنه
		sub->list.سطر[subidx].end = save_ptr;
	    sub->in_use = save_in_use;
	    بشکن;
    }
    بازگشت subs;
}

/*
 * Like addstate(), but the new state(s) are put at position "*ip".
 * Used for zero-width matches, next state to use is the added one.
 * This makes sure the order of states to be tried does not change, which
 * matters for alternatives.
 */
    ایستا regsubs_T *
addstate_here(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim,   // postponed look-behind match
    صحیح			*ip)
{
    صحیح tlen = l->n;
    صحیح شمار;
    صحیح listidx = *ip;
    regsubs_T *r;

    // First add the state(s) at the end, so that we know how many there are.
    // Pass the listidx as offset (avoids adding another argument to
    // addstate()).
    r = addstate(l, state, subs, pim, -listidx - ADDSTATE_HERE_OFFSET, 0);
    اگر (r == پوچ)
	بازگشت پوچ;

    // when "*ip" was at the end of the list, nothing to do
    اگر (listidx + 1 == tlen)
	بازگشت r;

    // re-order to put the new state at the current position
    شمار = l->n - tlen;
    اگر (شمار == 0)
	بازگشت r; // no state got added
    اگر (شمار == 1)
    {
	// overwrite the current state
	l->t[listidx] = l->t[l->n - 1];
    }
    وگرنه اگر (شمار > 1)
    {
	اگر (l->n + شمار - 1 >= l->len)
	{
	    // not enough space to move the new states, reallocate the list
	    // and move the states to the right position
	    صحیح		    newlen = l->len * 3 / 2 + 50;
	    size_t	    newsize = newlen * اندازه(nfa_thread_T);
	    nfa_thread_T    *newl;

	    اگر ((long)(newsize >> 10) >= p_mmp)
	    {
		emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		بازگشت پوچ;
	    }
	    newl = alloc(newsize);
	    اگر (newl == پوچ)
		بازگشت پوچ;
	    l->len = newlen;
	    mch_memmove(&(newl[0]),
		    &(l->t[0]),
		    اندازه(nfa_thread_T) * listidx);
	    mch_memmove(&(newl[listidx]),
		    &(l->t[l->n - شمار]),
		    اندازه(nfa_thread_T) * شمار);
	    mch_memmove(&(newl[listidx + شمار]),
		    &(l->t[listidx + 1]),
		    اندازه(nfa_thread_T) * (l->n - شمار - listidx - 1));
	    vim_free(l->t);
	    l->t = newl;
	}
	وگرنه
	{
	    // make space for new states, then move them from the
	    // end to the current position
	    mch_memmove(&(l->t[listidx + شمار]),
		    &(l->t[listidx + 1]),
		    اندازه(nfa_thread_T) * (l->n - listidx - 1));
	    mch_memmove(&(l->t[listidx]),
		    &(l->t[l->n - 1]),
		    اندازه(nfa_thread_T) * شمار);
	}
    }
    --l->n;
    *ip = listidx - 1;

    بازگشت r;
}

/*
 * Check character class "class" against current character c.
 */
    ایستا صحیح
check_char_class(صحیح class, صحیح c)
{
    انتخاب (class)
    {
	حالت NFA_CLASS_ALNUM:
	    اگر (c >= 1 && c < 128 && isalnum(c))
		بازگشت OK;
	    بشکن;
	حالت NFA_CLASS_ALPHA:
	    اگر (c >= 1 && c < 128 && isalpha(c))
		بازگشت OK;
	    بشکن;
	حالت NFA_CLASS_BLANK:
	    اگر (c == ' ' || c == '\t')
		بازگشت OK;
	    بشکن;
	حالت NFA_CLASS_CNTRL:
	    اگر (c >= 1 && c <= 127 && iscntrl(c))
		بازگشت OK;
	    بشکن;
	حالت NFA_CLASS_DIGIT:
	    اگر (VIM_ISDIGIT(c))
		بازگشت OK;
	    بشکن;
	حالت NFA_CLASS_GRAPH:
	    اگر (c >= 1 && c <= 127 && isgraph(c))
		بازگشت OK;
	    بشکن;
	حالت NFA_CLASS_LOWER:
	    اگر (MB_ISLOWER(c) && c != 170 && c != 186)
		بازگشت OK;
	    بشکن;
	حالت NFA_CLASS_PRINT:
	    اگر (vim_isprintc(c))
		بازگشت OK;
	    بشکن;
	حالت NFA_CLASS_PUNCT:
	    اگر (c >= 1 && c < 128 && ispunct(c))
		بازگشت OK;
	    بشکن;
	حالت NFA_CLASS_SPACE:
	    اگر ((c >= 9 && c <= 13) || (c == ' '))
		بازگشت OK;
	    بشکن;
	حالت NFA_CLASS_UPPER:
	    اگر (MB_ISUPPER(c))
		بازگشت OK;
	    بشکن;
	حالت NFA_CLASS_XDIGIT:
	    اگر (vim_isxdigit(c))
		بازگشت OK;
	    بشکن;
	حالت NFA_CLASS_TAB:
	    اگر (c == '\t')
		بازگشت OK;
	    بشکن;
	حالت NFA_CLASS_RETURN:
	    اگر (c == '\r')
		بازگشت OK;
	    بشکن;
	حالت NFA_CLASS_BACKSPACE:
	    اگر (c == '\b')
		بازگشت OK;
	    بشکن;
	حالت NFA_CLASS_ESCAPE:
	    اگر (c == '\033')
		بازگشت OK;
	    بشکن;
	حالت NFA_CLASS_IDENT:
	    اگر (vim_isIDc(c))
		بازگشت OK;
	    بشکن;
	حالت NFA_CLASS_KEYWORD:
	    اگر (reg_iswordc(c))
		بازگشت OK;
	    بشکن;
	حالت NFA_CLASS_FNAME:
	    اگر (vim_isfilec(c))
		بازگشت OK;
	    بشکن;

	پیشفرض:
	    // should not be here :P
	    siemsg(_(e_nfa_regexp_invalid_character_class_nr), class);
	    بازگشت FAIL;
    }
    بازگشت FAIL;
}

/*
 * Check for a match with subexpression "subidx".
 * Return TRUE if it matches.
 */
    ایستا صحیح
match_backref(
    regsub_T	*sub,	    // pointers to subexpressions
    صحیح		subidx,
    صحیح		*bytelen)   // out: length of match in bytes
{
    صحیح		len;

    اگر (sub->in_use <= subidx)
    {
retempty:
	// backref was not set, match an empty string
	*bytelen = 0;
	بازگشت TRUE;
    }

    اگر (REG_MULTI)
    {
	اگر (sub->list.multi[subidx].start_lnum < 0
				       || sub->list.multi[subidx].end_lnum < 0)
	    برو retempty;
	اگر (sub->list.multi[subidx].start_lnum == rex.lnum
			       && sub->list.multi[subidx].end_lnum == rex.lnum)
	{
	    len = sub->list.multi[subidx].end_col
					  - sub->list.multi[subidx].start_col;
	    اگر (cstrncmp(rex.سطر + sub->list.multi[subidx].start_col,
							 rex.درون_داد, &len) == 0)
	    {
		*bytelen = len;
		بازگشت TRUE;
	    }
	}
	وگرنه
	{
	    اگر (match_with_backref(
			sub->list.multi[subidx].start_lnum,
			sub->list.multi[subidx].start_col,
			sub->list.multi[subidx].end_lnum,
			sub->list.multi[subidx].end_col,
			bytelen) == RA_MATCH)
		بازگشت TRUE;
	}
    }
    وگرنه
    {
	اگر (sub->list.سطر[subidx].start == پوچ
					|| sub->list.سطر[subidx].end == پوچ)
	    برو retempty;
	len = (صحیح)(sub->list.سطر[subidx].end - sub->list.سطر[subidx].start);
	اگر (cstrncmp(sub->list.سطر[subidx].start, rex.درون_داد, &len) == 0)
	{
	    *bytelen = len;
	    بازگشت TRUE;
	}
    }
    بازگشت FALSE;
}

#ifdef FEAT_SYN_HL

/*
 * Check for a match with \z subexpression "subidx".
 * Return TRUE if it matches.
 */
    ایستا صحیح
match_zref(
    صحیح		subidx,
    صحیح		*bytelen)   // out: length of match in bytes
{
    صحیح		len;

    cleanup_zsubexpr();
    اگر (re_extmatch_in == پوچ || re_extmatch_in->matches[subidx] == پوچ)
    {
	// backref was not set, match an empty string
	*bytelen = 0;
	بازگشت TRUE;
    }

    len = (صحیح)STRLEN(re_extmatch_in->matches[subidx]);
    اگر (cstrncmp(re_extmatch_in->matches[subidx], rex.درون_داد, &len) == 0)
    {
	*bytelen = len;
	بازگشت TRUE;
    }
    بازگشت FALSE;
}
#endif

/*
 * Save list IDs for all NFA states of "prog" into "list".
 * Also reset the IDs to zero.
 * Only used for the recursive value lastlist[1].
 */
    ایستا تهی
nfa_save_listids(nfa_regprog_T *prog, صحیح *list)
{
    صحیح		    i;
    nfa_state_T	    *p;

    // Order in the list is reverse, it's a bit faster that way.
    p = &prog->state[0];
    برای (i = prog->nstate; --i >= 0; )
    {
	list[i] = p->lastlist[1];
	p->lastlist[1] = 0;
	++p;
    }
}

/*
 * Restore list IDs from "list" to all NFA states.
 */
    ایستا تهی
nfa_restore_listids(nfa_regprog_T *prog, صحیح *list)
{
    صحیح		    i;
    nfa_state_T	    *p;

    p = &prog->state[0];
    برای (i = prog->nstate; --i >= 0; )
    {
	p->lastlist[1] = list[i];
	++p;
    }
}

    ایستا صحیح
nfa_re_رقم_cmp(long_u val, صحیح op, long_u pos)
{
    اگر (op == 1) بازگشت pos > val;
    اگر (op == 2) بازگشت pos < val;
    بازگشت val == pos;
}

ایستا صحیح nfa_regmatch(nfa_regprog_T *prog, nfa_state_T *start, regsubs_T *submatch, regsubs_T *m);

/*
 * Recursively call nfa_regmatch()
 * "pim" is NULL or contains info about a Postponed Invisible Match (start
 * position).
 */
    ایستا صحیح
recursive_regmatch(
    nfa_state_T	    *state,
    nfa_pim_T	    *pim,
    nfa_regprog_T   *prog,
    regsubs_T	    *submatch,
    regsubs_T	    *m,
    صحیح		    **listids,
    صحیح		    *listids_len)
{
    صحیح		save_reginput_col = (صحیح)(rex.درون_داد - rex.سطر);
    صحیح		save_reglnum = rex.lnum;
    صحیح		save_nfa_match = nfa_match;
    صحیح		save_nfa_listid = rex.nfa_listid;
    save_se_T   *save_nfa_endp = nfa_endp;
    save_se_T   endpos;
    save_se_T   *endposp = پوچ;
    صحیح		result;
    صحیح		need_restore = FALSE;

    اگر (pim != پوچ)
    {
	// start at the position where the postponed match was
	اگر (REG_MULTI)
	    rex.درون_داد = rex.سطر + pim->end.pos.col;
	وگرنه
	    rex.درون_داد = pim->end.ptr;
    }

    اگر (state->c == NFA_START_INVISIBLE_BEFORE
	    || state->c == NFA_START_INVISIBLE_BEFORE_FIRST
	    || state->c == NFA_START_INVISIBLE_BEFORE_NEG
	    || state->c == NFA_START_INVISIBLE_BEFORE_NEG_FIRST)
    {
	// The recursive match must end at the current position. When "pim" is
	// not NULL it specifies the current position.
	endposp = &endpos;
	اگر (REG_MULTI)
	{
	    اگر (pim == پوچ)
	    {
		endpos.se_u.pos.col = (صحیح)(rex.درون_داد - rex.سطر);
		endpos.se_u.pos.lnum = rex.lnum;
	    }
	    وگرنه
		endpos.se_u.pos = pim->end.pos;
	}
	وگرنه
	{
	    اگر (pim == پوچ)
		endpos.se_u.ptr = rex.درون_داد;
	    وگرنه
		endpos.se_u.ptr = pim->end.ptr;
	}

	// Go back the specified number of bytes, or as far as the
	// start of the previous line, to try matching "\@<=" or
	// not matching "\@<!". This is very inefficient, limit the number of
	// bytes if possible.
	اگر (state->val <= 0)
	{
	    اگر (REG_MULTI)
	    {
		rex.سطر = reg_getline(--rex.lnum);
		اگر (rex.سطر == پوچ)
		    // can't go before the first line
		    rex.سطر = reg_getline(++rex.lnum);
	    }
	    rex.درون_داد = rex.سطر;
	}
	وگرنه
	{
	    اگر (REG_MULTI && (صحیح)(rex.درون_داد - rex.سطر) < state->val)
	    {
		// Not enough bytes in this line, go to end of
		// previous line.
		rex.سطر = reg_getline(--rex.lnum);
		اگر (rex.سطر == پوچ)
		{
		    // can't go before the first line
		    rex.سطر = reg_getline(++rex.lnum);
		    rex.درون_داد = rex.سطر;
		}
		وگرنه
		    rex.درون_داد = rex.سطر + reg_getline_len(rex.lnum);
	    }
	    اگر ((صحیح)(rex.درون_داد - rex.سطر) >= state->val)
	    {
		rex.درون_داد -= state->val;
		اگر (has_mbyte)
		    rex.درون_داد -= mb_head_off(rex.سطر, rex.درون_داد);
	    }
	    وگرنه
		rex.درون_داد = rex.سطر;
	}
    }

#ifdef ENABLE_LOG
    اگر (log_fd != stderr)
	fclose(log_fd);
    log_fd = پوچ;
#endif
    // Have to clear the lastlist field of the NFA nodes, so that
    // nfa_regmatch() and addstate() can run properly after recursion.
    اگر (nfa_ll_index == 1)
    {
	// Already calling nfa_regmatch() recursively.  Save the lastlist[1]
	// values and clear them.
	اگر (*listids == پوچ || *listids_len < prog->nstate)
	{
	    vim_free(*listids);
	    *listids = ALLOC_MULT(صحیح, prog->nstate);
	    اگر (*listids == پوچ)
	    {
		emsg(_(e_nfa_regexp_could_not_allocate_memory_for_branch_traversal));
		بازگشت 0;
	    }
	    *listids_len = prog->nstate;
	}
	nfa_save_listids(prog, *listids);
	need_restore = TRUE;
	// any value of rex.nfa_listid will do
    }
    وگرنه
    {
	// First recursive nfa_regmatch() call, switch to the second lastlist
	// entry.  Make sure rex.nfa_listid is different from a previous
	// recursive call, because some states may still have this ID.
	++nfa_ll_index;
	اگر (rex.nfa_listid <= rex.nfa_alt_listid)
	    rex.nfa_listid = rex.nfa_alt_listid;
    }

    // Call nfa_regmatch() to check if the current concat matches at this
    // position. The concat ends with the node NFA_END_INVISIBLE
    nfa_endp = endposp;
    result = nfa_regmatch(prog, state->out, submatch, m);

    اگر (need_restore)
	nfa_restore_listids(prog, *listids);
    وگرنه
    {
	--nfa_ll_index;
	rex.nfa_alt_listid = rex.nfa_listid;
    }

    // restore position in input text
    rex.lnum = save_reglnum;
    اگر (REG_MULTI)
	rex.سطر = reg_getline(rex.lnum);
    rex.درون_داد = rex.سطر + save_reginput_col;
    اگر (result != NFA_TOO_EXPENSIVE)
    {
	nfa_match = save_nfa_match;
	rex.nfa_listid = save_nfa_listid;
    }
    nfa_endp = save_nfa_endp;

#ifdef ENABLE_LOG
    open_debug_log(result);
#endif

    بازگشت result;
}

/*
 * Estimate the chance of a match with "state" failing.
 * empty match: 0
 * NFA_ANY: 1
 * specific character: 99
 */
    ایستا صحیح
failure_chance(nfa_state_T *state, صحیح depth)
{
    صحیح c = state->c;
    صحیح l, r;

    // detect looping
    اگر (depth > 4)
	بازگشت 1;

    انتخاب (c)
    {
	حالت NFA_SPLIT:
	    اگر (state->out->c == NFA_SPLIT || state->out1->c == NFA_SPLIT)
		// avoid recursive stuff
		بازگشت 1;
	    // two alternatives, use the lowest failure chance
	    l = failure_chance(state->out, depth + 1);
	    r = failure_chance(state->out1, depth + 1);
	    بازگشت l < r ? l : r;

	حالت NFA_ANY:
	    // matches anything, unlikely to fail
	    بازگشت 1;

	حالت NFA_MATCH:
	حالت NFA_MCLOSE:
	حالت NFA_ANY_COMPOSING:
	    // empty match works always
	    بازگشت 0;

	حالت NFA_START_INVISIBLE:
	حالت NFA_START_INVISIBLE_FIRST:
	حالت NFA_START_INVISIBLE_NEG:
	حالت NFA_START_INVISIBLE_NEG_FIRST:
	حالت NFA_START_INVISIBLE_BEFORE:
	حالت NFA_START_INVISIBLE_BEFORE_FIRST:
	حالت NFA_START_INVISIBLE_BEFORE_NEG:
	حالت NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
	حالت NFA_START_PATTERN:
	    // recursive regmatch is expensive, use low failure chance
	    بازگشت 5;

	حالت NFA_BOL:
	حالت NFA_EOL:
	حالت NFA_BOF:
	حالت NFA_EOF:
	حالت NFA_NEWL:
	    بازگشت 99;

	حالت NFA_BOW:
	حالت NFA_EOW:
	    بازگشت 90;

	حالت NFA_MOPEN:
	حالت NFA_MOPEN1:
	حالت NFA_MOPEN2:
	حالت NFA_MOPEN3:
	حالت NFA_MOPEN4:
	حالت NFA_MOPEN5:
	حالت NFA_MOPEN6:
	حالت NFA_MOPEN7:
	حالت NFA_MOPEN8:
	حالت NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	حالت NFA_ZOPEN:
	حالت NFA_ZOPEN1:
	حالت NFA_ZOPEN2:
	حالت NFA_ZOPEN3:
	حالت NFA_ZOPEN4:
	حالت NFA_ZOPEN5:
	حالت NFA_ZOPEN6:
	حالت NFA_ZOPEN7:
	حالت NFA_ZOPEN8:
	حالت NFA_ZOPEN9:
	حالت NFA_ZCLOSE:
	حالت NFA_ZCLOSE1:
	حالت NFA_ZCLOSE2:
	حالت NFA_ZCLOSE3:
	حالت NFA_ZCLOSE4:
	حالت NFA_ZCLOSE5:
	حالت NFA_ZCLOSE6:
	حالت NFA_ZCLOSE7:
	حالت NFA_ZCLOSE8:
	حالت NFA_ZCLOSE9:
#endif
	حالت NFA_NOPEN:
	حالت NFA_MCLOSE1:
	حالت NFA_MCLOSE2:
	حالت NFA_MCLOSE3:
	حالت NFA_MCLOSE4:
	حالت NFA_MCLOSE5:
	حالت NFA_MCLOSE6:
	حالت NFA_MCLOSE7:
	حالت NFA_MCLOSE8:
	حالت NFA_MCLOSE9:
	حالت NFA_NCLOSE:
	    بازگشت failure_chance(state->out, depth + 1);

	حالت NFA_BACKREF1:
	حالت NFA_BACKREF2:
	حالت NFA_BACKREF3:
	حالت NFA_BACKREF4:
	حالت NFA_BACKREF5:
	حالت NFA_BACKREF6:
	حالت NFA_BACKREF7:
	حالت NFA_BACKREF8:
	حالت NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	حالت NFA_ZREF1:
	حالت NFA_ZREF2:
	حالت NFA_ZREF3:
	حالت NFA_ZREF4:
	حالت NFA_ZREF5:
	حالت NFA_ZREF6:
	حالت NFA_ZREF7:
	حالت NFA_ZREF8:
	حالت NFA_ZREF9:
#endif
	    // backreferences don't match in many places
	    بازگشت 94;

	حالت NFA_LNUM_GT:
	حالت NFA_LNUM_LT:
	حالت NFA_COL_GT:
	حالت NFA_COL_LT:
	حالت NFA_VCOL_GT:
	حالت NFA_VCOL_LT:
	حالت NFA_MARK_GT:
	حالت NFA_MARK_LT:
	حالت NFA_VISUAL:
	    // before/after positions don't match very often
	    بازگشت 85;

	حالت NFA_LNUM:
	    بازگشت 90;

	حالت NFA_CURSOR:
	حالت NFA_COL:
	حالت NFA_VCOL:
	حالت NFA_MARK:
	    // specific positions rarely match
	    بازگشت 98;

	حالت NFA_COMPOSING:
	    بازگشت 95;

	پیشفرض:
	    اگر (c > 0)
		// character match fails often
		بازگشت 95;
    }

    // something else, includes character classes
    بازگشت 50;
}

/*
 * Skip until the char "c" we know a match must start with.
 */
    ایستا صحیح
skip_to_start(صحیح c, colnr_T *colp)
{
    char_u *s;

    // Used often, do some work to avoid call overhead.
    اگر (!rex.reg_ic && !has_mbyte)
	s = vim_strbyte(rex.سطر + *colp, c);
    وگرنه
	s = cstrchr(rex.سطر + *colp, c);
    اگر (s == پوچ)
	بازگشت FAIL;
    *colp = (صحیح)(s - rex.سطر);
    بازگشت OK;
}

/*
 * Check for a match with match_text.
 * Called after skip_to_start() has found regstart.
 * Returns zero for no match, 1 for a match.
 */
    ایستا long
find_match_text(colnr_T *startcol, صحیح regstart, char_u *match_text)
{
    colnr_T col = *startcol;
    صحیح	    c1, c2;
    صحیح	    len1, len2;
    صحیح	    match;

    برای (;;)
    {
	match = TRUE;
	// Skip the input character: case-folding can change its byte length.
	len2 = MB_CPTR2LEN(rex.سطر + col);
	برای (len1 = 0; match_text[len1] != NUL; len1 += MB_CHAR2LEN(c1))
	{
	    c1 = PTR2CHAR(match_text + len1);
	    c2 = PTR2CHAR(rex.سطر + col + len2);
	    اگر (c1 != c2 && (!rex.reg_ic || MB_CASEFOLD(c1) != MB_CASEFOLD(c2)))
	    {
		match = FALSE;
		بشکن;
	    }
	    len2 += enc_utf8 ? utf_ptr2len(rex.سطر + col + len2)
							     : MB_CHAR2LEN(c2);
	}
	اگر (match
		// check that no composing char follows
		&& !(enc_utf8
			  && utf_iscomposing(PTR2CHAR(rex.سطر + col + len2))))
	{
	    cleanup_subexpr();
	    اگر (REG_MULTI)
	    {
		rex.reg_startpos[0].lnum = rex.lnum;
		rex.reg_startpos[0].col = col;
		rex.reg_endpos[0].lnum = rex.lnum;
		rex.reg_endpos[0].col = col + len2;
	    }
	    وگرنه
	    {
		rex.reg_startp[0] = rex.سطر + col;
		rex.reg_endp[0] = rex.سطر + col + len2;
	    }
	    *startcol = col;
	    بازگشت 1L;
	}

	// Try finding regstart after the current match.
	col += MB_CPTR2LEN(rex.سطر + col); // skip the input character
	اگر (skip_to_start(regstart, &col) == FAIL)
	    بشکن;
    }

    *startcol = col;
    بازگشت 0L;
}

/*
 * Check whether the composing characters at the current input position match
 * the NFA composing sub-expression that starts at "sta" (the first state
 * after NFA_COMPOSING).  "curc" is the base character and "clen" its byte
 * length.
 * Returns OK when it matches, FAIL otherwise.
 */
    ایستا صحیح
match_composing(nfa_state_T *sta, صحیح curc, صحیح clen)
{
    صحیح		mc = curc;
    صحیح		len = 0;
    صحیح		cchars[MAX_MCO];
    صحیح		ccount = 0;
    صحیح		j;

    اگر (utf_iscomposing(sta->c))
	// Only match composing character(s), ignore base character.  Used
	// for ".{composing}" and "{composing}" (no preceding character).
	len += mb_char2len(mc);

    اگر (rex.reg_icombine && len == 0)
	// If \Z was present, then ignore composing characters.  When
	// ignoring the base character this always matches.
	بازگشت sta->c == curc ? OK : FAIL;

    // Check base character matches first, unless ignored.
    اگر (len == 0 && mc != sta->c)
	بازگشت FAIL;

    اگر (len == 0)
    {
	len += mb_char2len(mc);
	sta = sta->out;
    }

    // We don't care about the order of composing characters.  Get them into
    // cchars[] first.
    وقتی (len < clen)
    {
	mc = mb_ptr2char(rex.درون_داد + len);
	cchars[ccount++] = mc;
	len += mb_char2len(mc);
	اگر (ccount == MAX_MCO)
	    بشکن;
    }

    // Check that each composing char in the pattern matches a composing char
    // in the text.  We do not check if all composing chars are matched.
    وقتی (sta->c != NFA_END_COMPOSING)
    {
	برای (j = 0; j < ccount; ++j)
	    اگر (cchars[j] == sta->c)
		بشکن;
	اگر (j == ccount)
	    بازگشت FAIL;
	sta = sta->out;
    }

    بازگشت OK;
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
    ایستا صحیح
nfa_regmatch(
    nfa_regprog_T	*prog,
    nfa_state_T		*start,
    regsubs_T		*submatch,
    regsubs_T		*m)
{
    صحیح		result = FALSE;
    size_t	size = 0;
    صحیح		flag = 0;
    صحیح		go_to_nextline = FALSE;
    nfa_thread_T *t;
    nfa_list_T	list[2];
    صحیح		listidx;
    nfa_list_T	*thislist;
    nfa_list_T	*nextlist;
    صحیح		*listids = پوچ;
    صحیح		listids_len = 0;
    nfa_state_T *add_state;
    صحیح		add_here;
    صحیح		add_شمار;
    صحیح		add_off = 0;
    صحیح		toplevel = start->c == NFA_MOPEN;
    regsubs_T	*r;
#ifdef NFA_REGEXP_DEBUG_LOG
    FILE	*debug;
#endif

    // Some patterns may take a long time to match, especially when using
    // recursive_regmatch(). Allow interrupting them with CTRL-C.
    fast_breakcheck();
    اگر (got_int)
	بازگشت FALSE;
#ifdef FEAT_RELTIME
    اگر (nfa_did_time_out())
	بازگشت FALSE;
#endif

#ifdef NFA_REGEXP_DEBUG_LOG
    debug = fopen(NFA_REGEXP_DEBUG_LOG, "a");
    اگر (debug == پوچ)
    {
	semsg("(NFA) COULD NOT OPEN %s!", NFA_REGEXP_DEBUG_LOG);
	بازگشت FALSE;
    }
#endif
    nfa_match = FALSE;

    // Allocate memory for the lists of nodes.
    size = (prog->nstate + 1) * اندازه(nfa_thread_T);

    // Reuse cached list buffers from prog when available (top-level call).
    // Recursive calls must allocate their own buffers.
    اگر (toplevel && prog->listbuf[0] != پوچ)
    {
	list[0].t = (nfa_thread_T *)prog->listbuf[0];
	list[1].t = (nfa_thread_T *)prog->listbuf[1];
	prog->listbuf[0] = پوچ;
	prog->listbuf[1] = پوچ;
    }
    وگرنه
    {
	list[0].t = alloc(size);
	list[1].t = alloc(size);
    }
    list[0].len = prog->nstate + 1;
    list[1].len = prog->nstate + 1;
    اگر (list[0].t == پوچ || list[1].t == پوچ)
	برو theend;

#ifdef ENABLE_LOG
    log_fd = fopen(NFA_REGEXP_RUN_LOG, "a");
    اگر (log_fd == پوچ)
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
    اگر (toplevel)
    {
	اگر (REG_MULTI)
	{
	    m->norm.list.multi[0].start_lnum = rex.lnum;
	    m->norm.list.multi[0].start_col = (colnr_T)(rex.درون_داد - rex.سطر);
	    m->norm.orig_start_col = m->norm.list.multi[0].start_col;
	}
	وگرنه
	    m->norm.list.سطر[0].start = rex.درون_داد;
	m->norm.in_use = 1;
	r = addstate(thislist, start->out, m, پوچ, 0, 0);
    }
    وگرنه
	r = addstate(thislist, start, m, پوچ, 0, 0);
    اگر (r == پوچ)
    {
	nfa_match = NFA_TOO_EXPENSIVE;
	برو theend;
    }

#define	ADD_STATE_IF_MATCH(state)	\
    اگر (result)				\
    {					\
	add_state = state->out;		\
	add_off = clen;			\
    }

    /*
     * Run for each character.
     */
    برای (;;)
    {
	صحیح	curc;
	صحیح	clen;

	اگر (has_mbyte)
	{
	    // Fast path for an ASCII byte not followed by a composing
	    // character, matching the check in utfc_ptr2len().  Avoids two
	    // indirect calls for the common case.
	    اگر (rex.درون_داد[0] != NUL && rex.درون_داد[0] < 0x80 && rex.درون_داد[1] < 0x80)
	    {
		curc = rex.درون_داد[0];
		clen = 1;
	    }
	    وگرنه
	    {
		curc = (*mb_ptr2char)(rex.درون_داد);
		clen = (*mb_ptr2len)(rex.درون_داد);
	    }
	}
	وگرنه
	{
	    curc = *rex.درون_داد;
	    clen = 1;
	}
	اگر (curc == NUL)
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
	اگر (prog->re_engine == AUTOMATIC_ENGINE
		&& (rex.nfa_listid >= NFA_MAX_STATES
#ifdef FEAT_EVAL
		    || nfa_fail_for_testing
#endif
		    ))
	{
	    // too many states, retry with old engine
	    nfa_match = NFA_TOO_EXPENSIVE;
	    برو theend;
	}

	thislist->id = rex.nfa_listid;
	nextlist->id = rex.nfa_listid + 1;

#ifdef ENABLE_LOG
	fprintf(log_fd, "------------------------------------------\n");
	fprintf(log_fd, ">>> Reginput is \"%s\"\n", rex.درون_داد);
	fprintf(log_fd, ">>> Advanced one character... Current char is %c (code %d) \n", curc, (صحیح)curc);
	fprintf(log_fd, ">>> Thislist has %d states available: ", thislist->n);
	{
	    صحیح i;

	    برای (i = 0; i < thislist->n; i++)
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
	اگر (thislist->n == 0)
	    بشکن;

	// compute nextlist
	برای (listidx = 0; listidx < thislist->n; ++listidx)
	{
	    // If the list gets very long there probably is something wrong.
	    // At least allow interrupting with CTRL-C.  Only check once in a
	    // while, calling ui_breakcheck() for every state is too slow.
	    ایستا صحیح	breakcheck_شمار = 0;  // using "static" makes it faster
	    اگر (unlikely(++breakcheck_شمار >= 100000))
	    {
		ui_breakcheck();
		breakcheck_شمار = 0;
	    }
	    اگر (got_int)
		بشکن;
#ifdef FEAT_RELTIME
	    اگر (nfa_did_time_out())
		بشکن;
#endif
	    t = &thislist->t[listidx];

#ifdef NFA_REGEXP_DEBUG_LOG
	    nfa_set_code(t->state->c);
	    fprintf(debug, "%s, ", code);
#endif
#ifdef ENABLE_LOG
	    {
		صحیح col;

		اگر (t->subs.norm.in_use <= 0)
		    col = -1;
		وگرنه اگر (REG_MULTI)
		    col = t->subs.norm.list.multi[0].start_col;
		وگرنه
		    col = (صحیح)(t->subs.norm.list.سطر[0].start - rex.سطر);
		nfa_set_code(t->state->c);
		fprintf(log_fd, "(%d) char %d %s (start col %d)%s... \n",
			abs(t->state->id), (صحیح)t->state->c, code, col,
			pim_info(&t->pim));
	    }
#endif

	    /*
	     * Handle the possible codes of the current state.
	     * The most important is NFA_MATCH.
	     */
	    add_state = پوچ;
	    add_here = FALSE;
	    add_شمار = 0;
	    انتخاب (t->state->c)
	    {
	    حالت NFA_MATCH:
	      {
		// If the match is not at the start of the line, ends before a
		// composing characters and rex.reg_icombine is not set, that
		// is not really a match.
		اگر (enc_utf8 && !rex.reg_icombine
			     && rex.درون_داد != rex.سطر && utf_iscomposing(curc))
		    بشکن;

		nfa_match = TRUE;
		copy_sub(&submatch->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		اگر (rex.nfa_has_zsubexpr)
		    copy_sub(&submatch->synt, &t->subs.synt);
#endif
#ifdef ENABLE_LOG
		log_subsexpr(&t->subs);
#endif
		// Found the left-most longest match, do not look at any other
		// states at this position.  When the list of states is going
		// to be empty quit without advancing, so that "rex.input" is
		// correct.
		اگر (nextlist->n == 0)
		    clen = 0;
		برو nextchar;
	      }

	    حالت NFA_END_INVISIBLE:
	    حالت NFA_END_INVISIBLE_NEG:
	    حالت NFA_END_PATTERN:
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
		اگر (nfa_endp != پوچ)
		{
		    اگر (REG_MULTI)
			fprintf(log_fd, "Current lnum: %d, endp lnum: %d; current col: %d, endp col: %d\n",
				(صحیح)rex.lnum,
				(صحیح)nfa_endp->se_u.pos.lnum,
				(صحیح)(rex.درون_داد - rex.سطر),
				nfa_endp->se_u.pos.col);
		    وگرنه
			fprintf(log_fd, "Current col: %d, endp col: %d\n",
				(صحیح)(rex.درون_داد - rex.سطر),
				(صحیح)(nfa_endp->se_u.ptr - rex.درون_داد));
		}
#endif
		// If "nfa_endp" is set it's only a match if it ends at
		// "nfa_endp"
		اگر (nfa_endp != پوچ && (REG_MULTI
			? (rex.lnum != nfa_endp->se_u.pos.lnum
			    || (صحیح)(rex.درون_داد - rex.سطر)
						!= nfa_endp->se_u.pos.col)
			: rex.درون_داد != nfa_endp->se_u.ptr))
		    بشکن;

		// do not set submatches for \@!
		اگر (t->state->c != NFA_END_INVISIBLE_NEG)
		{
		    copy_sub(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		    اگر (rex.nfa_has_zsubexpr)
			copy_sub(&m->synt, &t->subs.synt);
#endif
		}
#ifdef ENABLE_LOG
		fprintf(log_fd, "Match found:\n");
		log_subsexpr(m);
#endif
		nfa_match = TRUE;
		// See comment above at "goto nextchar".
		اگر (nextlist->n == 0)
		    clen = 0;
		برو nextchar;

	    حالت NFA_START_INVISIBLE:
	    حالت NFA_START_INVISIBLE_FIRST:
	    حالت NFA_START_INVISIBLE_NEG:
	    حالت NFA_START_INVISIBLE_NEG_FIRST:
	    حالت NFA_START_INVISIBLE_BEFORE:
	    حالت NFA_START_INVISIBLE_BEFORE_FIRST:
	    حالت NFA_START_INVISIBLE_BEFORE_NEG:
	    حالت NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
		{
#ifdef ENABLE_LOG
		    fprintf(log_fd, "Failure chance invisible: %d, what follows: %d\n",
			    failure_chance(t->state->out, 0),
			    failure_chance(t->state->out1->out, 0));
#endif
		    // Do it directly if there already is a PIM or when
		    // nfa_postprocess() detected it will work better.
		    اگر (t->pim.result != NFA_PIM_UNUSED
			 || t->state->c == NFA_START_INVISIBLE_FIRST
			 || t->state->c == NFA_START_INVISIBLE_NEG_FIRST
			 || t->state->c == NFA_START_INVISIBLE_BEFORE_FIRST
			 || t->state->c == NFA_START_INVISIBLE_BEFORE_NEG_FIRST)
		    {
			صحیح in_use = m->norm.in_use;

			// Copy submatch info for the recursive call, opposite
			// of what happens on success below.
			copy_sub_off(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
			اگر (rex.nfa_has_zsubexpr)
			    copy_sub_off(&m->synt, &t->subs.synt);
#endif

			/*
			 * First try matching the invisible match, then what
			 * follows.
			 */
			result = recursive_regmatch(t->state, پوچ, prog,
					  submatch, m, &listids, &listids_len);
			اگر (result == NFA_TOO_EXPENSIVE)
			{
			    nfa_match = result;
			    برو theend;
			}

			// for \@! and \@<! it is a match when the result is
			// FALSE
			اگر (result != (t->state->c == NFA_START_INVISIBLE_NEG
			       || t->state->c == NFA_START_INVISIBLE_NEG_FIRST
			       || t->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			       || t->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&t->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
			    اگر (rex.nfa_has_zsubexpr)
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
		    وگرنه
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
			اگر (REG_MULTI)
			{
			    pim.end.pos.col = (صحیح)(rex.درون_داد - rex.سطر);
			    pim.end.pos.lnum = rex.lnum;
			}
			وگرنه
			    pim.end.ptr = rex.درون_داد;

			// t->state->out1 is the corresponding END_INVISIBLE
			// node; Add its out to the current list (zero-width
			// match).
			اگر (addstate_here(thislist, t->state->out1->out,
					     &t->subs, &pim, &listidx) == پوچ)
			{
			    nfa_match = NFA_TOO_EXPENSIVE;
			    برو theend;
			}
		    }
		}
		بشکن;

	    حالت NFA_START_PATTERN:
	      {
		nfa_state_T *skip = پوچ;
#ifdef ENABLE_LOG
		صحیح	    skip_lid = 0;
#endif

		// There is no point in trying to match the pattern if the
		// output state is not going to be added to the list.
		اگر (state_in_list(nextlist, t->state->out1->out, &t->subs))
		{
		    skip = t->state->out1->out;
#ifdef ENABLE_LOG
		    skip_lid = nextlist->id;
#endif
		}
		وگرنه اگر (state_in_list(nextlist,
					  t->state->out1->out->out, &t->subs))
		{
		    skip = t->state->out1->out->out;
#ifdef ENABLE_LOG
		    skip_lid = nextlist->id;
#endif
		}
		وگرنه اگر (state_in_list(thislist,
					  t->state->out1->out->out, &t->subs))
		{
		    skip = t->state->out1->out->out;
#ifdef ENABLE_LOG
		    skip_lid = thislist->id;
#endif
		}
		اگر (skip != پوچ)
		{
#ifdef ENABLE_LOG
		    nfa_set_code(skip->c);
		    fprintf(log_fd, "> Not trying to match pattern, output state %d is already in list %d. char %d: %s\n",
			    abs(skip->id), skip_lid, skip->c, code);
#endif
		    بشکن;
		}
		// Copy submatch info to the recursive call, opposite of what
		// happens afterwards.
		copy_sub_off(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		اگر (rex.nfa_has_zsubexpr)
		    copy_sub_off(&m->synt, &t->subs.synt);
#endif

		// First try matching the pattern.
		result = recursive_regmatch(t->state, پوچ, prog,
					  submatch, m, &listids, &listids_len);
		اگر (result == NFA_TOO_EXPENSIVE)
		{
		    nfa_match = result;
		    برو theend;
		}
		اگر (result)
		{
		    صحیح bytelen;

#ifdef ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN matches:\n");
		    log_subsexpr(m);
#endif
		    // Copy submatch info from the recursive call
		    copy_sub_off(&t->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
		    اگر (rex.nfa_has_zsubexpr)
			copy_sub_off(&t->subs.synt, &m->synt);
#endif
		    // Now we need to skip over the matched text and then
		    // continue with what follows.
		    اگر (REG_MULTI)
			// TODO: multi-line match
			bytelen = m->norm.list.multi[0].end_col
						  - (صحیح)(rex.درون_داد - rex.سطر);
		    وگرنه
			bytelen = (صحیح)(m->norm.list.سطر[0].end - rex.درون_داد);

#ifdef ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN length: %d\n", bytelen);
#endif
		    اگر (bytelen == 0)
		    {
			// empty match, output of corresponding
			// NFA_END_PATTERN/NFA_SKIP to be used at current
			// position
			add_here = TRUE;
			add_state = t->state->out1->out->out;
		    }
		    وگرنه اگر (bytelen <= clen)
		    {
			// match current character, output of corresponding
			// NFA_END_PATTERN to be used at next position.
			add_state = t->state->out1->out->out;
			add_off = clen;
		    }
		    وگرنه
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			add_state = t->state->out1->out;
			add_off = bytelen;
			add_شمار = bytelen - clen;
		    }
		}
		بشکن;
	      }

	    حالت NFA_BOL:
		اگر (rex.درون_داد == rex.سطر)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		بشکن;

	    حالت NFA_EOL:
		اگر (curc == NUL)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		بشکن;

	    حالت NFA_BOW:
		result = TRUE;

		اگر (curc == NUL)
		    result = FALSE;
		وگرنه اگر (has_mbyte)
		{
		    صحیح this_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.درون_داد, rex.reg_buf);
		    اگر (this_class <= 1)
			result = FALSE;
		    وگرنه اگر (reg_prev_class() == this_class)
			result = FALSE;
		}
		وگرنه اگر (!vim_iswordc_buf(curc, rex.reg_buf)
			   || (rex.درون_داد > rex.سطر
			       && vim_iswordc_buf(rex.درون_داد[-1], rex.reg_buf)))
		    result = FALSE;
		اگر (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		بشکن;

	    حالت NFA_EOW:
		result = TRUE;
		اگر (rex.درون_داد == rex.سطر)
		    result = FALSE;
		وگرنه اگر (has_mbyte)
		{
		    صحیح this_class, prev_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.درون_داد, rex.reg_buf);
		    prev_class = reg_prev_class();
		    اگر (this_class == prev_class
					|| prev_class == 0 || prev_class == 1)
			result = FALSE;
		}
		وگرنه اگر (!vim_iswordc_buf(rex.درون_داد[-1], rex.reg_buf)
			|| (rex.درون_داد[0] != NUL
					&& vim_iswordc_buf(curc, rex.reg_buf)))
		    result = FALSE;
		اگر (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		بشکن;

	    حالت NFA_BOF:
		اگر (rex.lnum == 0 && rex.درون_داد == rex.سطر
				     && (!REG_MULTI || rex.reg_firstlnum == 1))
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		بشکن;

	    حالت NFA_EOF:
		اگر (rex.lnum == rex.reg_maxline && curc == NUL)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		بشکن;

	    حالت NFA_COMPOSING:
	    {
		nfa_state_T *end;

		result = match_composing(t->state->out, curc, clen);

		end = t->state->out1;	    // NFA_END_COMPOSING
		ADD_STATE_IF_MATCH(end);
		بشکن;
	    }

	    حالت NFA_NEWL:
		اگر (curc == NUL && !rex.reg_سطر_lbr && REG_MULTI
						&& rex.lnum <= rex.reg_maxline)
		{
		    go_to_nextline = TRUE;
		    // Pass -1 for the offset, which means taking the position
		    // at the start of the next line.
		    add_state = t->state->out;
		    add_off = -1;
		}
		وگرنه اگر (curc == '\n' && rex.reg_سطر_lbr)
		{
		    // match \n as if it is an ordinary character
		    add_state = t->state->out;
		    add_off = 1;
		}
		بشکن;

	    حالت NFA_START_COLL:
	    حالت NFA_START_NEG_COLL:
	      {
		// What follows is a list of characters, until NFA_END_COLL.
		// One of them must match or none of them must match.
		nfa_state_T	*state;
		صحیح		result_if_matched;
		صحیح		c1, c2;

		// Never match EOL. If it's part of the collection it is added
		// as a separate state with an OR.
		اگر (curc == NUL)
		    بشکن;

		state = t->state->out;
		result_if_matched = (t->state->c == NFA_START_COLL);
		برای (;;)
		{
		    اگر (state->c == NFA_COMPOSING)
		    {
			nfa_state_T *end;

			result = match_composing(t->state->out->out, curc,
									clen);

			اگر (t->state->out->out1 != پوچ
				&& t->state->out->out1->c == NFA_END_COMPOSING)
			{
			    end = t->state->out->out1;
			    ADD_STATE_IF_MATCH(end);
			}
			بشکن;
		    }
		    اگر (state->c == NFA_END_COLL)
		    {
			result = !result_if_matched;
			بشکن;
		    }
		    اگر (state->c == NFA_RANGE_MIN)
		    {
			c1 = state->val;
			state = state->out; // advance to NFA_RANGE_MAX
			c2 = state->val;
#ifdef ENABLE_LOG
			fprintf(log_fd, "NFA_RANGE_MIN curc=%d c1=%d c2=%d\n",
				curc, c1, c2);
#endif
			اگر (curc >= c1 && curc <= c2)
			{
			    result = result_if_matched;
			    بشکن;
			}
			اگر (rex.reg_ic)
			{
			    صحیح curc_کم = MB_CASEFOLD(curc);
			    صحیح done = FALSE;

			    برای ( ; c1 <= c2; ++c1)
				اگر (MB_CASEFOLD(c1) == curc_کم)
				{
				    result = result_if_matched;
				    done = TRUE;
				    بشکن;
				}
			    اگر (done)
				بشکن;
			}
		    }
		    وگرنه اگر (state->c < 0 ? check_char_class(state->c, curc)
			       : (curc == state->c
				   || (rex.reg_ic && MB_CASEFOLD(curc)
						    == MB_CASEFOLD(state->c))))
		    {
			result = result_if_matched;
			بشکن;
		    }
		    state = state->out;
		}
		اگر (result)
		{
		    // next state is in out of the NFA_END_COLL, out1 of
		    // START points to the END state
		    add_state = t->state->out1->out;
		    add_off = clen;
		}
		بشکن;
	      }

	    حالت NFA_ANY:
		// Any char except '\0', (end of input) does not match.
		اگر (curc > 0)
		{
		    add_state = t->state->out;
		    add_off = clen;
		}
		بشکن;

	    حالت NFA_ANY_COMPOSING:
		// On a composing character skip over it.  Otherwise do
		// nothing.  Always matches.
		اگر (enc_utf8 && utf_iscomposing(curc))
		{
		    add_off = clen;
		}
		وگرنه
		{
		    add_here = TRUE;
		    add_off = 0;
		}
		add_state = t->state->out;
		بشکن;

	    /*
	     * Character classes like \a for alpha, \d for digit etc.
	     */
	    حالت NFA_IDENT:	//  \i
		result = vim_isIDc(curc);
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_SIDENT:	//  \I
		result = !VIM_ISDIGIT(curc) && vim_isIDc(curc);
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_KWORD:	//  \k
		result = vim_iswordp_buf(rex.درون_داد, rex.reg_buf);
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_SKWORD:	//  \K
		result = !VIM_ISDIGIT(curc)
				     && vim_iswordp_buf(rex.درون_داد, rex.reg_buf);
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_FNAME:	//  \f
		result = vim_isfilec(curc);
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_SFNAME:	//  \F
		result = !VIM_ISDIGIT(curc) && vim_isfilec(curc);
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_PRINT:	//  \p
		result = vim_isprintc(PTR2CHAR(rex.درون_داد));
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_SPRINT:	//  \P
		result = !VIM_ISDIGIT(curc) && vim_isprintc(PTR2CHAR(rex.درون_داد));
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_WHITE:	//  \s
		result = VIM_ISWHITE(curc);
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_NWHITE:	//  \S
		result = curc != NUL && !VIM_ISWHITE(curc);
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_DIGIT:	//  \d
		result = ri_digit(curc);
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_NDIGIT:	//  \D
		result = curc != NUL && !ri_digit(curc);
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_HEX:	//  \x
		result = ri_hex(curc);
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_NHEX:	//  \X
		result = curc != NUL && !ri_hex(curc);
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_OCTAL:	//  \o
		result = ri_octal(curc);
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_NOCTAL:	//  \O
		result = curc != NUL && !ri_octal(curc);
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_WORD:	//  \w
		result = ri_word(curc);
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_NWORD:	//  \W
		result = curc != NUL && !ri_word(curc);
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_HEAD:	//  \h
		result = ri_head(curc);
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_NHEAD:	//  \H
		result = curc != NUL && !ri_head(curc);
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_ALPHA:	//  \a
		result = ri_alpha(curc);
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_NALPHA:	//  \A
		result = curc != NUL && !ri_alpha(curc);
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_LOWER:	//  \l
		result = ri_lower(curc);
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_NLOWER:	//  \L
		result = curc != NUL && !ri_lower(curc);
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_UPPER:	//  \u
		result = ri_upper(curc);
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_NUPPER:	// \U
		result = curc != NUL && !ri_upper(curc);
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_LOWER_IC:	// [a-z]
		result = ri_lower(curc) || (rex.reg_ic && ri_upper(curc));
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_NLOWER_IC:	// [^a-z]
		result = curc != NUL
			&& !(ri_lower(curc) || (rex.reg_ic && ri_upper(curc)));
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_UPPER_IC:	// [A-Z]
		result = ri_upper(curc) || (rex.reg_ic && ri_lower(curc));
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_NUPPER_IC:	// ^[A-Z]
		result = curc != NUL
			&& !(ri_upper(curc) || (rex.reg_ic && ri_lower(curc)));
		ADD_STATE_IF_MATCH(t->state);
		بشکن;

	    حالت NFA_BACKREF1:
	    حالت NFA_BACKREF2:
	    حالت NFA_BACKREF3:
	    حالت NFA_BACKREF4:
	    حالت NFA_BACKREF5:
	    حالت NFA_BACKREF6:
	    حالت NFA_BACKREF7:
	    حالت NFA_BACKREF8:
	    حالت NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	    حالت NFA_ZREF1:
	    حالت NFA_ZREF2:
	    حالت NFA_ZREF3:
	    حالت NFA_ZREF4:
	    حالت NFA_ZREF5:
	    حالت NFA_ZREF6:
	    حالت NFA_ZREF7:
	    حالت NFA_ZREF8:
	    حالت NFA_ZREF9:
#endif
		// \1 .. \9  \z1 .. \z9
	      {
		صحیح subidx;
		صحیح bytelen;

#ifdef FEAT_SYN_HL
		اگر (t->state->c >= NFA_BACKREF1 && t->state->c <= NFA_BACKREF9)
#endif
		{
		    subidx = t->state->c - NFA_BACKREF1 + 1;
		    result = match_backref(&t->subs.norm, subidx, &bytelen);
		}
#ifdef FEAT_SYN_HL
		وگرنه
		{
		    subidx = t->state->c - NFA_ZREF1 + 1;
		    result = match_zref(subidx, &bytelen);
		}
#endif

		اگر (result)
		{
		    اگر (bytelen == 0)
		    {
			// empty match always works, output of NFA_SKIP to be
			// used next
			add_here = TRUE;
			add_state = t->state->out->out;
		    }
		    وگرنه اگر (bytelen <= clen)
		    {
			// match current character, jump ahead to out of
			// NFA_SKIP
			add_state = t->state->out->out;
			add_off = clen;
		    }
		    وگرنه
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			add_state = t->state->out;
			add_off = bytelen;
			add_شمار = bytelen - clen;
		    }
		}
		بشکن;
	      }
	    حالت NFA_SKIP:
	      // character of previous matching \1 .. \9  or \@>
	      اگر (t->شمار - clen <= 0)
	      {
		  // end of match, go to what follows
		  add_state = t->state->out;
		  add_off = clen;
	      }
	      وگرنه
	      {
		  // add state again with decremented count
		  add_state = t->state;
		  add_off = 0;
		  add_شمار = t->شمار - clen;
	      }
	      بشکن;

	    حالت NFA_LNUM:
	    حالت NFA_LNUM_GT:
	    حالت NFA_LNUM_LT:
		result = (REG_MULTI &&
			nfa_re_رقم_cmp(t->state->val, t->state->c - NFA_LNUM,
			    (long_u)(rex.lnum + rex.reg_firstlnum)));
		اگر (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		بشکن;

	    حالت NFA_COL:
	    حالت NFA_COL_GT:
	    حالت NFA_COL_LT:
		result = nfa_re_رقم_cmp(t->state->val, t->state->c - NFA_COL,
			(long_u)(rex.درون_داد - rex.سطر) + 1);
		اگر (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		بشکن;

	    حالت NFA_VCOL:
	    حالت NFA_VCOL_GT:
	    حالت NFA_VCOL_LT:
		{
		    صحیح     op = t->state->c - NFA_VCOL;
		    colnr_T col = (colnr_T)(rex.درون_داد - rex.سطر);
		    win_T   *wp = rex.reg_win == پوچ ? curwin : rex.reg_win;

		    // Bail out quickly when there can't be a match, avoid the
		    // overhead of win_linetabsize() on long lines.
		    اگر (op != 1 && col > t->state->val
			    * (has_mbyte ? MB_MAXBYTES : 1))
			بشکن;
		    result = FALSE;
		    اگر (op == 1 && col - 1 > t->state->val && col > 100)
		    {
			صحیح ts = wp->w_buffer->b_p_ts;

			// Guess that a character won't use more columns than
			// 'tabstop', with a minimum of 4.
			اگر (ts < 4)
			    ts = 4;
			result = col > t->state->val * ts;
		    }
		    اگر (!result)
		    {
			linenr_T    lnum = REG_MULTI
					    ? rex.reg_firstlnum + rex.lnum : 1;
			long_u	    vcol;

			اگر (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_buffer->b_ml.ml_سطر_شمار))
			    lnum = 1;
			vcol = (long_u)win_linetabsize(wp, lnum, rex.سطر, col);
			result = nfa_re_رقم_cmp(t->state->val, op, vcol + 1);
		    }
		    اگر (result)
		    {
			add_here = TRUE;
			add_state = t->state->out;
		    }
		}
		بشکن;

	    حالت NFA_MARK:
	    حالت NFA_MARK_GT:
	    حالت NFA_MARK_LT:
	      {
		pos_T	*pos;
		size_t	col = REG_MULTI ? rex.درون_داد - rex.سطر : 0;

		pos = getmark_buf(rex.reg_buf, t->state->val, FALSE);

		// Line may have been freed, get it again.
		اگر (REG_MULTI)
		{
		    rex.سطر = reg_getline(rex.lnum);
		    rex.درون_داد = rex.سطر + col;
		}

		// Compare the mark position to the match position, if the mark
		// exists and mark is set in reg_buf.
		اگر (pos != پوچ && pos->lnum > 0)
		{
		    colnr_T pos_col = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->col == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->col;

		    result = (pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_col == (colnr_T)(rex.درون_داد - rex.سطر)
				    ? t->state->c == NFA_MARK
				    : (pos_col < (colnr_T)(rex.درون_داد - rex.سطر)
					? t->state->c == NFA_MARK_GT
					: t->state->c == NFA_MARK_LT))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? t->state->c == NFA_MARK_GT
				    : t->state->c == NFA_MARK_LT));
		    اگر (result)
		    {
			add_here = TRUE;
			add_state = t->state->out;
		    }
		}
		بشکن;
	      }

	    حالت NFA_CURSOR:
		result = (rex.reg_win != پوچ
			&& (rex.lnum + rex.reg_firstlnum
						 == rex.reg_win->w_cursor.lnum)
			&& ((colnr_T)(rex.درون_داد - rex.سطر)
						== rex.reg_win->w_cursor.col));
		اگر (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		بشکن;

	    حالت NFA_VISUAL:
		result = reg_match_visual();
		اگر (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		بشکن;

	    حالت NFA_MOPEN1:
	    حالت NFA_MOPEN2:
	    حالت NFA_MOPEN3:
	    حالت NFA_MOPEN4:
	    حالت NFA_MOPEN5:
	    حالت NFA_MOPEN6:
	    حالت NFA_MOPEN7:
	    حالت NFA_MOPEN8:
	    حالت NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	    حالت NFA_ZOPEN:
	    حالت NFA_ZOPEN1:
	    حالت NFA_ZOPEN2:
	    حالت NFA_ZOPEN3:
	    حالت NFA_ZOPEN4:
	    حالت NFA_ZOPEN5:
	    حالت NFA_ZOPEN6:
	    حالت NFA_ZOPEN7:
	    حالت NFA_ZOPEN8:
	    حالت NFA_ZOPEN9:
#endif
	    حالت NFA_NOPEN:
	    حالت NFA_ZSTART:
		// These states are only added to be able to bail out when
		// they are added again, nothing is to be done.
		بشکن;

	    پیشفرض:	// regular character
	      {
		صحیح c = t->state->c;

#ifdef DEBUG
		اگر (c < 0)
		    siemsg("Negative state char: %ld", (long)c);
#endif
		result = (c == curc);

		اگر (!result && rex.reg_ic)
		    result = MB_CASEFOLD(c) == MB_CASEFOLD(curc);
		// If rex.reg_icombine is not set only skip over the character
		// itself.  When it is set skip over composing characters.
		اگر (result && enc_utf8 && !rex.reg_icombine)
		    clen = utf_ptr2len(rex.درون_داد);
		ADD_STATE_IF_MATCH(t->state);
		بشکن;
	      }

	    } // switch (t->state->c)

	    اگر (add_state != پوچ)
	    {
		nfa_pim_T *pim;
		nfa_pim_T pim_copy;

		اگر (t->pim.result == NFA_PIM_UNUSED)
		    pim = پوچ;
		وگرنه
		    pim = &t->pim;

		// Handle the postponed invisible match if the match might end
		// without advancing and before the end of the line.
		اگر (pim != پوچ && (clen == 0 || match_follows(add_state, 0)))
		{
		    اگر (pim->result == NFA_PIM_TODO)
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
			اگر (result != (pim->state->c == NFA_START_INVISIBLE_NEG
			     || pim->state->c == NFA_START_INVISIBLE_NEG_FIRST
			     || pim->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&pim->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
			    اگر (rex.nfa_has_zsubexpr)
				copy_sub_off(&pim->subs.synt, &m->synt);
#endif
			}
		    }
		    وگرنه
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
		    اگر (result != (pim->state->c == NFA_START_INVISIBLE_NEG
			     || pim->state->c == NFA_START_INVISIBLE_NEG_FIRST
			     || pim->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
		    {
			// Copy submatch info from the recursive call
			copy_sub_off(&t->subs.norm, &pim->subs.norm);
#ifdef FEAT_SYN_HL
			اگر (rex.nfa_has_zsubexpr)
			    copy_sub_off(&t->subs.synt, &pim->subs.synt);
#endif
		    }
		    وگرنه
			// look-behind match failed, don't add the state
			ادامه;

		    // Postponed invisible match was handled, don't add it to
		    // following states.
		    pim = پوچ;
		}

		// If "pim" points into l->t it will become invalid when
		// adding the state causes the list to be reallocated.  Make a
		// local copy to avoid that.
		اگر (pim == &t->pim)
		{
		    copy_pim(&pim_copy, pim);
		    pim = &pim_copy;
		}

		اگر (add_here)
		    r = addstate_here(thislist, add_state, &t->subs,
								pim, &listidx);
		وگرنه
		{
		    r = addstate(nextlist, add_state, &t->subs, pim, add_off, 0);
		    اگر (add_شمار > 0)
			nextlist->t[nextlist->n - 1].شمار = add_شمار;
		}
		اگر (r == پوچ)
		{
		    nfa_match = NFA_TOO_EXPENSIVE;
		    برو theend;
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
	اگر (nfa_match == FALSE
		&& ((toplevel
			&& rex.lnum == 0
			&& clen != 0
			&& (rex.reg_maxcol == 0
			  || (colnr_T)(rex.درون_داد - rex.سطر) < rex.reg_maxcol))
		    || (nfa_endp != پوچ
			&& (REG_MULTI
			    ? (rex.lnum < nfa_endp->se_u.pos.lnum
			       || (rex.lnum == nfa_endp->se_u.pos.lnum
				   && (صحیح)(rex.درون_داد - rex.سطر)
						    < nfa_endp->se_u.pos.col))
			    : rex.درون_داد < nfa_endp->se_u.ptr))))
	{
#ifdef ENABLE_LOG
	    fprintf(log_fd, "(---) STARTSTATE\n");
#endif
	    // Inline optimized code for addstate() if we know the state is
	    // the first MOPEN.
	    اگر (toplevel)
	    {
		صحیح add = TRUE;
		صحیح c;

		اگر (prog->regstart != NUL && clen != 0)
		{
		    اگر (nextlist->n == 0)
		    {
			colnr_T col = (colnr_T)(rex.درون_داد - rex.سطر) + clen;

			// Nextlist is empty, we can skip ahead to the
			// character that must appear at the start.
			اگر (skip_to_start(prog->regstart, &col) == FAIL)
			    بشکن;
#ifdef ENABLE_LOG
			fprintf(log_fd, "  Skipping ahead %d bytes to regstart\n",
				col - ((colnr_T)(rex.درون_داد - rex.سطر) + clen));
#endif
			rex.درون_داد = rex.سطر + col - clen;
		    }
		    وگرنه
		    {
			// Checking if the required start character matches is
			// cheaper than adding a state that won't match.
			c = PTR2CHAR(rex.درون_داد + clen);
			اگر (c != prog->regstart && (!rex.reg_ic
			     || MB_CASEFOLD(c) != MB_CASEFOLD(prog->regstart)))
			{
#ifdef ENABLE_LOG
			    fprintf(log_fd, "  Skipping start state, regstart does not match\n");
#endif
			    add = FALSE;
			}
		    }
		}

		اگر (add)
		{
		    اگر (REG_MULTI)
		    {
			m->norm.list.multi[0].start_col =
					 (colnr_T)(rex.درون_داد - rex.سطر) + clen;
			m->norm.orig_start_col =
					       m->norm.list.multi[0].start_col;
		    }
		    وگرنه
			m->norm.list.سطر[0].start = rex.درون_داد + clen;
		    اگر (addstate(nextlist, start->out, m, پوچ, clen, 0) == پوچ)
		    {
			nfa_match = NFA_TOO_EXPENSIVE;
			برو theend;
		    }
		}
	    }
	    وگرنه
	    {
		char_u	    *save_سطر = rex.سطر;
		char_u	    *save_درون_داد = rex.درون_داد;
		linenr_T    save_lnum = rex.lnum;

		// At the end of a line the match can only start on the next
		// line, use that position instead of the line break.
		اگر (REG_MULTI && clen == 0 && nfa_endp != پوچ
					 && rex.lnum < nfa_endp->se_u.pos.lnum)
		{
		    char_u  *next_سطر = reg_getline(rex.lnum + 1);

		    اگر (next_سطر != پوچ)
		    {
			rex.سطر = next_سطر;
			rex.درون_داد = rex.سطر;
			++rex.lnum;
		    }
		}

		r = addstate(nextlist, start, m, پوچ, clen, 0);

		rex.سطر = save_سطر;
		rex.درون_داد = save_درون_داد;
		rex.lnum = save_lnum;

		اگر (r == پوچ)
		{
		    nfa_match = NFA_TOO_EXPENSIVE;
		    برو theend;
		}
	    }
	}

#ifdef ENABLE_LOG
	fprintf(log_fd, ">>> Thislist had %d states available: ", thislist->n);
	{
	    صحیح i;

	    برای (i = 0; i < thislist->n; i++)
		fprintf(log_fd, "%d  ", abs(thislist->t[i].state->id));
	}
	fprintf(log_fd, "\n");
#endif

nextchar:
	// Advance to the next character, or advance to the next line, or
	// finish.
	اگر (clen != 0)
	    rex.درون_داد += clen;
	وگرنه اگر (go_to_nextline || (nfa_endp != پوچ && REG_MULTI
					&& rex.lnum < nfa_endp->se_u.pos.lnum))
	    reg_nextline();
	وگرنه
	    بشکن;

	// Allow interrupting with CTRL-C.
	سطر_breakcheck();
	اگر (got_int)
	    بشکن;
#ifdef FEAT_RELTIME
	اگر (nfa_did_time_out())
	    بشکن;
#endif
    }

#ifdef ENABLE_LOG
    اگر (log_fd != stderr)
	fclose(log_fd);
    log_fd = پوچ;
#endif

theend:
    // Cache list buffers in prog for reuse, or free if prog already has
    // cached buffers (recursive call case).
    اگر (prog->listbuf[0] == پوچ && list[0].t != پوچ && list[1].t != پوچ)
    {
	prog->listbuf[0] = list[0].t;
	prog->listbuf[1] = list[1].t;
    }
    وگرنه
    {
	vim_free(list[0].t);
	vim_free(list[1].t);
    }
    vim_free(listids);
#undef ADD_STATE_IF_MATCH
#ifdef NFA_REGEXP_DEBUG_LOG
    fclose(debug);
#endif

    بازگشت nfa_match;
}

/*
 * Try match of "prog" with at rex.line["col"].
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    ایستا long
nfa_regtry(
    nfa_regprog_T   *prog,
    colnr_T	    col,
    صحیح		    *timed_out UNUSED)	// flag set on timeout or NULL
{
    صحیح		i;
    regsubs_T	subs, m;
    nfa_state_T	*start = prog->start;
    صحیح		result;
#ifdef ENABLE_LOG
    FILE	*f;
#endif

    rex.درون_داد = rex.سطر + col;
#ifdef FEAT_RELTIME
    nfa_timed_out = timed_out;
#endif

#ifdef ENABLE_LOG
    f = fopen(NFA_REGEXP_RUN_LOG, "a");
    اگر (f != پوچ)
    {
	fprintf(f, "\n\n\t=======================================================\n");
# ifdef DEBUG
	fprintf(f, "\tRegexp is \"%s\"\n", nfa_regengine.عبارت);
# endif
	fprintf(f, "\tInput text is \"%s\" \n", rex.درون_داد);
	fprintf(f, "\t=======================================================\n\n");
	nfa_print_state(f, start);
	fprintf(f, "\n\n");
	fclose(f);
    }
    وگرنه
	emsg("Could not open temporary log file for writing");
#endif

    clear_sub(&subs.norm);
    clear_sub(&m.norm);
#ifdef FEAT_SYN_HL
    clear_sub(&subs.synt);
    clear_sub(&m.synt);
#endif

    result = nfa_regmatch(prog, start, &subs, &m);
    اگر (result == FALSE)
	بازگشت 0;
    وگرنه اگر (result == NFA_TOO_EXPENSIVE)
	بازگشت result;

    cleanup_subexpr();
    اگر (REG_MULTI)
    {
	برای (i = 0; i < subs.norm.in_use; i++)
	{
	    rex.reg_startpos[i].lnum = subs.norm.list.multi[i].start_lnum;
	    rex.reg_startpos[i].col = subs.norm.list.multi[i].start_col;

	    rex.reg_endpos[i].lnum = subs.norm.list.multi[i].end_lnum;
	    rex.reg_endpos[i].col = subs.norm.list.multi[i].end_col;
	}
	اگر (rex.reg_mmatch != پوچ)
	    rex.reg_mmatch->rmm_matchcol = subs.norm.orig_start_col;

	اگر (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].col = col;
	}
	اگر (rex.reg_endpos[0].lnum < 0)
	{
	    // pattern has a \ze but it didn't match, use current end
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].col = (صحیح)(rex.درون_داد - rex.سطر);
	}
	وگرنه
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    وگرنه
    {
	برای (i = 0; i < subs.norm.in_use; i++)
	{
	    rex.reg_startp[i] = subs.norm.list.سطر[i].start;
	    rex.reg_endp[i] = subs.norm.list.سطر[i].end;
	}

	اگر (rex.reg_startp[0] == پوچ)
	    rex.reg_startp[0] = rex.سطر + col;
	اگر (rex.reg_endp[0] == پوچ)
	    rex.reg_endp[0] = rex.درون_داد;
    }

#ifdef FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = پوچ;

    اگر (prog->reghasz == REX_SET)
    {
	cleanup_zsubexpr();
	re_extmatch_out = make_extmatch();
	اگر (re_extmatch_out == پوچ)
	    بازگشت 0;
	// Loop over \z1, \z2, etc.  There is no \z0.
	برای (i = 1; i < subs.synt.in_use; i++)
	{
	    اگر (REG_MULTI)
	    {
		ساختار multipos *mpos = &subs.synt.list.multi[i];

		// Only accept single line matches that are valid.
		اگر (mpos->start_lnum >= 0
			&& mpos->start_lnum == mpos->end_lnum
			&& mpos->end_col >= mpos->start_col)
		    re_extmatch_out->matches[i] =
			vim_strnsave(reg_getline(mpos->start_lnum)
							    + mpos->start_col,
					     mpos->end_col - mpos->start_col);
	    }
	    وگرنه
	    {
		ساختار linepos *lpos = &subs.synt.list.سطر[i];

		اگر (lpos->start != پوچ && lpos->end != پوچ)
		    re_extmatch_out->matches[i] =
			    vim_strnsave(lpos->start, lpos->end - lpos->start);
	    }
	}
    }
#endif

    بازگشت 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 *
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    ایستا long
nfa_regexec_both(
    char_u	*سطر,
    colnr_T	startcol,	// column to start looking for match
    صحیح		*timed_out)	// flag set on timeout or NULL
{
    nfa_regprog_T   *prog;
    long	    retval = 0L;
    صحیح		    i;
    colnr_T	    col = startcol;

    اگر (REG_MULTI)
    {
	prog = (nfa_regprog_T *)rex.reg_mmatch->regprog;
	سطر = reg_getline((linenr_T)0);    // relative to the cursor
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    وگرنه
    {
	prog = (nfa_regprog_T *)rex.reg_match->regprog;
	rex.reg_startp = rex.reg_match->startp;
	rex.reg_endp = rex.reg_match->endp;
    }

    // Be paranoid...
    اگر (prog == پوچ || سطر == پوچ)
    {
	iemsg(e_null_argument);
	برو theend;
    }

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    اگر (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    وگرنه اگر (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    اگر (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    rex.سطر = سطر;
    rex.lnum = 0;    // relative to line

    rex.nfa_has_zend = prog->has_zend;
    rex.nfa_has_backref = prog->has_backref;
    rex.nfa_nsubexpr = prog->nsubexp;
    rex.nfa_listid = 1;
    rex.nfa_alt_listid = 2;
#ifdef DEBUG
    nfa_regengine.عبارت = prog->pattern;
#endif

    اگر (prog->reganch && col > 0)
	بازگشت 0L;

    rex.need_clear_subexpr = TRUE;
#ifdef FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    اگر (prog->reghasz == REX_SET)
    {
	rex.nfa_has_zsubexpr = TRUE;
	rex.need_clear_zsubexpr = TRUE;
    }
    وگرنه
    {
	rex.nfa_has_zsubexpr = FALSE;
	rex.need_clear_zsubexpr = FALSE;
    }
#endif

    اگر (prog->regstart != NUL)
    {
	// Skip ahead until a character we know the match must start with.
	// When there is none there is no match.
	اگر (skip_to_start(prog->regstart, &col) == FAIL)
	    بازگشت 0L;

	// If match_text is set it contains the full text that must match.
	// Nothing else to try. Doesn't handle combining chars well.
	اگر (prog->match_text != پوچ && *prog->match_text != NUL && !rex.reg_icombine)
	{
	    retval = find_match_text(&col, prog->regstart, prog->match_text);
	    اگر (REG_MULTI)
		rex.reg_mmatch->rmm_matchcol = col;
	    وگرنه
		rex.reg_match->rm_matchcol = col;
	    بازگشت retval;
	}
    }

    // If the start column is past the maximum column: no need to try.
    اگر (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	برو theend;

    // Set the "nstate" used by nfa_regcomp() to zero to trigger an error when
    // it's accidentally used during execution.
    nstate = 0;
    برای (i = 0; i < prog->nstate; ++i)
    {
	prog->state[i].id = i;
	prog->state[i].lastlist[0] = 0;
	prog->state[i].lastlist[1] = 0;
    }

    retval = nfa_regtry(prog, col, timed_out);

#ifdef DEBUG
    nfa_regengine.عبارت = پوچ;
#endif

theend:
    اگر (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs and
	// \ze are used.
	اگر (REG_MULTI)
	{
	    lpos_T *start = &rex.reg_mmatch->startpos[0];
	    lpos_T *end = &rex.reg_mmatch->endpos[0];

	    اگر (end->lnum < start->lnum
			|| (end->lnum == start->lnum && end->col < start->col))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];
	}
	وگرنه
	{
	    اگر (rex.reg_match->endp[0] < rex.reg_match->startp[0])
		rex.reg_match->endp[0] = rex.reg_match->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_match->rm_matchcol = col;
	}
    }

    بازگشت retval;
}

/*
 * Compile a regular expression into internal code for the NFA matcher.
 * Returns the program in allocated space.  Returns NULL for an error.
 */
    ایستا regprog_T *
nfa_regcomp(char_u *عبارت, صحیح re_flags)
{
    nfa_regprog_T	*prog = پوچ;
    size_t		prog_size;
    صحیح			*postfix;

    اگر (عبارت == پوچ)
	بازگشت پوچ;

#ifdef DEBUG
    nfa_regengine.عبارت = عبارت;
#endif
    nfa_re_flags = re_flags;

    init_class_tab();

    اگر (nfa_regcomp_start(عبارت, re_flags) == FAIL)
	بازگشت پوچ;

    // Build postfix form of the regexp. Needed to build the NFA
    // (and count its size).
    postfix = re2post();
    اگر (postfix == پوچ)
	برو fail;	    // Cascaded (syntax?) error

    /*
     * In order to build the NFA, we parse the input regexp twice:
     * 1. first pass to count size (so we can allocate space)
     * 2. second to emit code
     */
#ifdef ENABLE_LOG
    {
	FILE *f = fopen(NFA_REGEXP_RUN_LOG, "a");

	اگر (f != پوچ)
	{
	    fprintf(f, "\n*****************************\n\n\n\n\tCompiling regexp \"%s\"... hold on !\n", عبارت);
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
    prog_size = offsetof(nfa_regprog_T, state) + اندازه(nfa_state_T) * nstate;
    prog = alloc(prog_size);
    اگر (prog == پوچ)
	برو fail;
    state_ptr = prog->state;
    prog->re_in_use = FALSE;
    prog->listbuf[0] = پوچ;
    prog->listbuf[1] = پوچ;

    /*
     * PASS 2
     * Build the NFA
     */
    prog->start = post2nfa(postfix, post_ptr, FALSE);
    اگر (prog->start == پوچ)
	برو fail;

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
    nfa_postfix_dump(عبارت, OK);
    nfa_dump(prog);
#endif
#ifdef FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    prog->reghasz = re_has_z;
#endif
    prog->pattern = vim_strsave(عبارت);
#ifdef DEBUG
    nfa_regengine.عبارت = پوچ;
#endif

out:
    // The postfix list and fragment stack are reused by the next compilation
    // (and freed in free_regexp_stuff()), but drop ones that grew large for a
    // big pattern to avoid holding much memory.
    اگر (post_start_len > NFA_POSTFIX_KEEP)
    {
	VIM_CLEAR(post_start);
	post_start_len = 0;
    }
    post_ptr = post_end = پوچ;
    اگر (nfa_پشته_len > NFA_پشته_KEEP)
    {
	VIM_CLEAR(nfa_پشته);
	nfa_پشته_len = 0;
    }
    state_ptr = پوچ;
    بازگشت (regprog_T *)prog;

fail:
    VIM_CLEAR(prog);
#ifdef ENABLE_LOG
    nfa_postfix_dump(عبارت, FAIL);
#endif
#ifdef DEBUG
    nfa_regengine.عبارت = پوچ;
#endif
    برو out;
}

/*
 * Free a compiled regexp program, returned by nfa_regcomp().
 */
    ایستا تهی
nfa_regfree(regprog_T *prog)
{
    اگر (prog == پوچ)
	بازگشت;

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
    ایستا صحیح
nfa_regexec_nl(
    regmatch_T	*rmp,
    char_u	*سطر,	// string to match against
    colnr_T	col,	// column to start looking for match
    صحیح		سطر_lbr)
{
    rex.reg_match = rmp;
    rex.reg_mmatch = پوچ;
    rex.reg_maxline = 0;
    rex.reg_سطر_lbr = سطر_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = پوچ;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;
    بازگشت nfa_regexec_both(سطر, col, پوچ);
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
    ایستا long
nfa_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	col,		// column to start looking for match
    صحیح		*timed_out)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    بازگشت nfa_regexec_both(پوچ, col, timed_out);
}

#ifdef DEBUG
# undef ENABLE_LOG
#endif
