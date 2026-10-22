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

ga_nanaa
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
static const จำนวนเต็ม nfa_classcodes[] = {
    NFA_ANY, NFA_IDENT, NFA_SIDENT, NFA_KWORD,NFA_SKWORD,
    NFA_FNAME, NFA_SFNAME, NFA_PRINT, NFA_SPRINT,
    NFA_WHITE, NFA_NWHITE, NFA_DIGIT, NFA_NDIGIT,
    NFA_HEX, NFA_NHEX, NFA_OCTAL, NFA_NOCTAL,
    NFA_WORD, NFA_NWORD, NFA_HEAD, NFA_NHEAD,
    NFA_ALPHA, NFA_NALPHA, NFA_LOWER, NFA_NLOWER,
    NFA_UPPER, NFA_NUPPER
};

// Variables only used in nfa_regcomp() and descendants.
static จำนวนเต็ม nfa_re_flags; // re_flags passed to nfa_regcomp()
static จำนวนเต็ม *post_start;  // holds the postfix form of r.e.
static จำนวนเต็ม post_start_len;  // size of allocated post_start (in ints)
static จำนวนเต็ม *post_end;
static จำนวนเต็ม *post_ptr;
static จำนวนเต็ม nfa_reg_parse_depth;	// nesting depth in nfa_reg()

// The postfix list (post_start) and fragment stack (nfa_stack) are reused
// across compilations; a buffer that grew past these sizes for a big pattern
// is freed afterwards instead of being kept, to avoid holding much memory.
#define NFA_POSTFIX_KEEP	10000	// number of ints (~40 Kbyte)
#define NFA_STACK_KEEP		4000	// number of Frag_T (~64 Kbyte)

// Set when the pattern should use the NFA engine.
// E.g. [[:upper:]] only allows 8bit characters for BT engine,
// while NFA engine handles multibyte characters correctly.
static จำนวนเต็ม wants_nfa;

static จำนวนเต็ม nstate;	// Number of states in the NFA.
static จำนวนเต็ม istate;	// Index in the state vector, used in alloc_state()

// If not NULL match must end at this position
static save_se_T *nfa_endp = NULL;

// 0 for first call to nfa_regmatch(), 1 for recursive call.
static จำนวนเต็ม nfa_ll_index = 0;

static จำนวนเต็ม realloc_post_list(void);
static จำนวนเต็ม nfa_reg(จำนวนเต็ม paren);
#ifdef DEBUG
static void nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent);
#endif
static จำนวนเต็ม match_follows(nfa_state_T *startstate, จำนวนเต็ม depth);
static จำนวนเต็ม failure_chance(nfa_state_T *state, จำนวนเต็ม depth);

// helper functions used when doing re2post() ... regatom() parsing
#define EMIT(c)	do {				\
		    ถ้า (post_ptr >= post_end && realloc_post_list() == FAIL) \
			คืน FAIL;		\
		    *post_ptr++ = c;		\
		} ขณะ (0)

/*
 * Initialize internal variables before NFA compilation.
 * Return OK on success, FAIL otherwise.
 */
    static จำนวนเต็ม
nfa_regcomp_start(
    char_u	*expr,
    จำนวนเต็ม		re_flags)	    // see vim_regcomp()
{
    size_t	postfix_size;
    จำนวนเต็ม		nstate_max;

    nstate = 0;
    istate = 0;
    // A reasonable estimation for maximum size
    nstate_max = (จำนวนเต็ม)(STRLEN(expr) + 1) * 25;

    // Some items blow up in size, such as [A-z].  Add more space for that.
    // When it is still not enough realloc_post_list() will be used.
    nstate_max += 1000;

    // Size for postfix representation of expr.
    postfix_size = parimaa_na(จำนวนเต็ม) * nstate_max;

    // Reuse the postfix buffer across compilations, only growing it when the
    // estimate exceeds the current size; it is freed in free_regexp_stuff().
    ถ้า (post_start == NULL || post_start_len < nstate_max)
    {
	vim_free(post_start);
	post_start = alloc(postfix_size);
	ถ้า (post_start == NULL)
	{
	    post_start_len = 0;
	    คืน FAIL;
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
    regcomp_start(expr, re_flags);

    คืน OK;
}

/*
 * Figure out if the NFA state list starts with an anchor, must match at start
 * of the line.
 */
    static จำนวนเต็ม
nfa_get_reganch(nfa_state_T *start, จำนวนเต็ม depth)
{
    nfa_state_T *p = start;

    ถ้า (depth > 4)
	คืน 0;

    ขณะ (p != NULL)
    {
	switch (p->c)
	{
	    pak_sa NFA_BOL:
	    pak_sa NFA_BOF:
		คืน 1; // yes!

	    pak_sa NFA_ZSTART:
	    pak_sa NFA_ZEND:
	    pak_sa NFA_CURSOR:
	    pak_sa NFA_VISUAL:

	    pak_sa NFA_MOPEN:
	    pak_sa NFA_MOPEN1:
	    pak_sa NFA_MOPEN2:
	    pak_sa NFA_MOPEN3:
	    pak_sa NFA_MOPEN4:
	    pak_sa NFA_MOPEN5:
	    pak_sa NFA_MOPEN6:
	    pak_sa NFA_MOPEN7:
	    pak_sa NFA_MOPEN8:
	    pak_sa NFA_MOPEN9:
	    pak_sa NFA_NOPEN:
#ifdef FEAT_SYN_HL
	    pak_sa NFA_ZOPEN:
	    pak_sa NFA_ZOPEN1:
	    pak_sa NFA_ZOPEN2:
	    pak_sa NFA_ZOPEN3:
	    pak_sa NFA_ZOPEN4:
	    pak_sa NFA_ZOPEN5:
	    pak_sa NFA_ZOPEN6:
	    pak_sa NFA_ZOPEN7:
	    pak_sa NFA_ZOPEN8:
	    pak_sa NFA_ZOPEN9:
#endif
		p = p->out;
		bha_nga;

	    pak_sa NFA_SPLIT:
		คืน nfa_get_reganch(p->out, depth + 1)
				       && nfa_get_reganch(p->out1, depth + 1);

	    default:
		คืน 0; // noooo
	}
    }
    คืน 0;
}

/*
 * Figure out if the NFA state list starts with a character which must match
 * at start of the match.
 */
    static จำนวนเต็ม
nfa_get_regstart(nfa_state_T *start, จำนวนเต็ม depth)
{
    nfa_state_T *p = start;

    ถ้า (depth > 4)
	คืน 0;

    ขณะ (p != NULL)
    {
	switch (p->c)
	{
	    // all kinds of zero-width matches
	    pak_sa NFA_BOL:
	    pak_sa NFA_BOF:
	    pak_sa NFA_BOW:
	    pak_sa NFA_EOW:
	    pak_sa NFA_ZSTART:
	    pak_sa NFA_ZEND:
	    pak_sa NFA_CURSOR:
	    pak_sa NFA_VISUAL:
	    pak_sa NFA_LNUM:
	    pak_sa NFA_LNUM_GT:
	    pak_sa NFA_LNUM_LT:
	    pak_sa NFA_COL:
	    pak_sa NFA_COL_GT:
	    pak_sa NFA_COL_LT:
	    pak_sa NFA_VCOL:
	    pak_sa NFA_VCOL_GT:
	    pak_sa NFA_VCOL_LT:
	    pak_sa NFA_MARK:
	    pak_sa NFA_MARK_GT:
	    pak_sa NFA_MARK_LT:

	    pak_sa NFA_MOPEN:
	    pak_sa NFA_MOPEN1:
	    pak_sa NFA_MOPEN2:
	    pak_sa NFA_MOPEN3:
	    pak_sa NFA_MOPEN4:
	    pak_sa NFA_MOPEN5:
	    pak_sa NFA_MOPEN6:
	    pak_sa NFA_MOPEN7:
	    pak_sa NFA_MOPEN8:
	    pak_sa NFA_MOPEN9:
	    pak_sa NFA_NOPEN:
#ifdef FEAT_SYN_HL
	    pak_sa NFA_ZOPEN:
	    pak_sa NFA_ZOPEN1:
	    pak_sa NFA_ZOPEN2:
	    pak_sa NFA_ZOPEN3:
	    pak_sa NFA_ZOPEN4:
	    pak_sa NFA_ZOPEN5:
	    pak_sa NFA_ZOPEN6:
	    pak_sa NFA_ZOPEN7:
	    pak_sa NFA_ZOPEN8:
	    pak_sa NFA_ZOPEN9:
#endif
		p = p->out;
		bha_nga;

	    pak_sa NFA_SPLIT:
	    {
		จำนวนเต็ม c1 = nfa_get_regstart(p->out, depth + 1);
		จำนวนเต็ม c2 = nfa_get_regstart(p->out1, depth + 1);

		ถ้า (c1 == c2)
		    คืน c1; // yes!
		คืน 0;
	    }

	    default:
		ถ้า (p->c > 0)
		    คืน p->c; // yes!
		คืน 0;
	}
    }
    คืน 0;
}

/*
 * Figure out if the NFA state list contains just literal text and nothing
 * else.  If so return a string in allocated memory with what must match after
 * regstart.  Otherwise return NULL.
 */
    static char_u *
nfa_get_match_text(nfa_state_T *start)
{
    nfa_state_T *p = start;
    จำนวนเต็ม		len = 0;
    char_u	*ret;
    char_u	*s;

    ถ้า (p->c != NFA_MOPEN)
	คืน NULL; // just in case
    p = p->out;
    ขณะ (p->c > 0)
    {
	len += MB_CHAR2LEN(p->c);
	p = p->out;
    }
    ถ้า (p->c != NFA_MCLOSE || p->out->c != NFA_MATCH)
	คืน NULL;

    ret = alloc(len);
    ถ้า (ret == NULL)
	คืน NULL;

    p = start->out->out; // skip first char, it goes into regstart
    s = ret;
    ขณะ (p->c > 0)
    {
	ถ้า (has_mbyte)
	    s += (*mb_char2bytes)(p->c, s);
	มิฉะนั้น
	    *s++ = p->c;
	p = p->out;
    }
    *s = NUL;
    คืน ret;
}

/*
 * Allocate more space for post_start.  Called when
 * running above the estimated number of states.
 */
    static จำนวนเต็ม
realloc_post_list(void)
{
    จำนวนเต็ม   nstate_max = (จำนวนเต็ม)(post_end - post_start);
    จำนวนเต็ม   new_max;
    จำนวนเต็ม   *new_start;
    จำนวนเต็ม	  *old_start;

    // For weird patterns the number of states can be very high. Increasing by
    // 50% seems a reasonable compromise between memory use and speed.
    new_max = nstate_max * 3 / 2;
    new_start = ALLOC_MULT(จำนวนเต็ม, new_max);
    ถ้า (new_start == NULL)
	คืน FAIL;
    mch_memmove(new_start, post_start, nstate_max * parimaa_na(จำนวนเต็ม));
    old_start = post_start;
    post_start = new_start;
    post_start_len = new_max;
    post_ptr = new_start + (post_ptr - old_start);
    post_end = post_start + new_max;
    vim_free(old_start);
    คืน OK;
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
    static จำนวนเต็ม
nfa_recognize_char_class(char_u *start, char_u *end, จำนวนเต็ม extra_newl)
{
    จำนวนเต็ม		newl;
    จำนวนเต็ม		config = get_char_class_bits(start, end, &newl);

    ถ้า (config < 0)
	คืน FAIL;

    ถ้า (extra_newl == TRUE)
	newl = TRUE;

    ถ้า (newl == TRUE)
	extra_newl = NFA_ADD_NL;

    switch (config)
    {
	pak_sa CLASS_o9:
	    คืน extra_newl + NFA_DIGIT;
	pak_sa CLASS_not |  CLASS_o9:
	    คืน extra_newl + NFA_NDIGIT;
	pak_sa CLASS_af | CLASS_AF | CLASS_o9:
	    คืน extra_newl + NFA_HEX;
	pak_sa CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    คืน extra_newl + NFA_NHEX;
	pak_sa CLASS_o7:
	    คืน extra_newl + NFA_OCTAL;
	pak_sa CLASS_not | CLASS_o7:
	    คืน extra_newl + NFA_NOCTAL;
	pak_sa CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    คืน extra_newl + NFA_WORD;
	pak_sa CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    คืน extra_newl + NFA_NWORD;
	pak_sa CLASS_az | CLASS_AZ | CLASS_underscore:
	    คืน extra_newl + NFA_HEAD;
	pak_sa CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    คืน extra_newl + NFA_NHEAD;
	pak_sa CLASS_az | CLASS_AZ:
	    คืน extra_newl + NFA_ALPHA;
	pak_sa CLASS_not | CLASS_az | CLASS_AZ:
	    คืน extra_newl + NFA_NALPHA;
	pak_sa CLASS_az:
	   คืน extra_newl + NFA_LOWER_IC;
	pak_sa CLASS_not | CLASS_az:
	    คืน extra_newl + NFA_NLOWER_IC;
	pak_sa CLASS_AZ:
	    คืน extra_newl + NFA_UPPER_IC;
	pak_sa CLASS_not | CLASS_AZ:
	    คืน extra_newl + NFA_NUPPER_IC;
    }
    คืน FAIL;
}

/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * Emits bytes in postfix notation: 'a,b,NFA_OR,c,NFA_OR' is
 * equivalent to 'a OR b OR c'
 *
 * NOTE! When changing this function, also update reg_equi_class()
 */
    static จำนวนเต็ม
nfa_emit_equi_class(จำนวนเต็ม c)
{
#define EMIT2(c)    EMIT(c); EMIT(NFA_CONCAT);

    ถ้า (enc_utf8 || STRCMP(p_enc, "latin1") == 0
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
	switch (c)
	{
	    pak_sa 'A': pak_sa A_grave: pak_sa A_acute: pak_sa A_circumflex:
	    pak_sa A_virguilla: pak_sa A_diaeresis: pak_sa A_ring:
	    pak_sa 0x100: pak_sa 0x102: pak_sa 0x104: pak_sa 0x1cd:
	    pak_sa 0x1de: pak_sa 0x1e0: pak_sa 0x1fa: pak_sa 0x200:
	    pak_sa 0x202: pak_sa 0x226: pak_sa 0x23a: pak_sa 0x1e00:
	    pak_sa 0x1ea0: pak_sa 0x1ea2: pak_sa 0x1ea4: pak_sa 0x1ea6:
	    pak_sa 0x1ea8: pak_sa 0x1eaa: pak_sa 0x1eac: pak_sa 0x1eae:
	    pak_sa 0x1eb0: pak_sa 0x1eb2: pak_sa 0x1eb4: pak_sa 0x1eb6:
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
		    คืน OK;

	    pak_sa 'B': pak_sa 0x181: pak_sa 0x243: pak_sa 0x1e02:
	    pak_sa 0x1e04: pak_sa 0x1e06:
		    EMIT2('B')
		    EMIT2(0x181) EMIT2(0x243) EMIT2(0x1e02)
		    EMIT2(0x1e04) EMIT2(0x1e06)
		    คืน OK;

	    pak_sa 'C': pak_sa C_cedilla: pak_sa 0x106: pak_sa 0x108:
	    pak_sa 0x10a: pak_sa 0x10c: pak_sa 0x187: pak_sa 0x23b:
	    pak_sa 0x1e08: pak_sa 0xa792:
		    EMIT2('C') EMIT2(C_cedilla)
		    EMIT2(0x106) EMIT2(0x108) EMIT2(0x10a)
		    EMIT2(0x10c) EMIT2(0x187) EMIT2(0x23b)
		    EMIT2(0x1e08) EMIT2(0xa792)
		    คืน OK;

	    pak_sa 'D': pak_sa 0x10e: pak_sa 0x110: pak_sa 0x18a:
	    pak_sa 0x1e0a: pak_sa 0x1e0c: pak_sa 0x1e0e: pak_sa 0x1e10:
	    pak_sa 0x1e12:
		    EMIT2('D') EMIT2(0x10e) EMIT2(0x110) EMIT2(0x18a)
		    EMIT2(0x1e0a) EMIT2(0x1e0c) EMIT2(0x1e0e)
		    EMIT2(0x1e10) EMIT2(0x1e12)
		    คืน OK;

	    pak_sa 'E': pak_sa E_grave: pak_sa E_acute: pak_sa E_circumflex:
	    pak_sa E_diaeresis: pak_sa 0x112: pak_sa 0x114: pak_sa 0x116:
	    pak_sa 0x118: pak_sa 0x11a: pak_sa 0x204: pak_sa 0x206:
	    pak_sa 0x228: pak_sa 0x246: pak_sa 0x1e14: pak_sa 0x1e16:
	    pak_sa 0x1e18: pak_sa 0x1e1a: pak_sa 0x1e1c: pak_sa 0x1eb8:
	    pak_sa 0x1eba: pak_sa 0x1ebc: pak_sa 0x1ebe: pak_sa 0x1ec0:
	    pak_sa 0x1ec2: pak_sa 0x1ec4: pak_sa 0x1ec6:
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
		    คืน OK;

	    pak_sa 'F': pak_sa 0x191: pak_sa 0x1e1e: pak_sa 0xa798:
		    EMIT2('F') EMIT2(0x191) EMIT2(0x1e1e) EMIT2(0xa798)
		    คืน OK;

	    pak_sa 'G': pak_sa 0x11c: pak_sa 0x11e: pak_sa 0x120:
	    pak_sa 0x122: pak_sa 0x193: pak_sa 0x1e4: pak_sa 0x1e6:
	    pak_sa 0x1f4: pak_sa 0x1e20: pak_sa 0xa7a0:
		    EMIT2('G') EMIT2(0x11c) EMIT2(0x11e) EMIT2(0x120)
		    EMIT2(0x122) EMIT2(0x193) EMIT2(0x1e4)
		    EMIT2(0x1e6) EMIT2(0x1f4) EMIT2(0x1e20)
		    EMIT2(0xa7a0)
		    คืน OK;

	    pak_sa 'H': pak_sa 0x124: pak_sa 0x126: pak_sa 0x21e:
	    pak_sa 0x1e22: pak_sa 0x1e24: pak_sa 0x1e26: pak_sa 0x1e28:
	    pak_sa 0x1e2a: pak_sa 0x2c67:
		    EMIT2('H') EMIT2(0x124) EMIT2(0x126) EMIT2(0x21e)
		    EMIT2(0x1e22) EMIT2(0x1e24) EMIT2(0x1e26)
		    EMIT2(0x1e28) EMIT2(0x1e2a) EMIT2(0x2c67)
		    คืน OK;

	    pak_sa 'I': pak_sa I_grave: pak_sa I_acute: pak_sa I_circumflex:
	    pak_sa I_diaeresis: pak_sa 0x128: pak_sa 0x12a: pak_sa 0x12c:
	    pak_sa 0x12e: pak_sa 0x130: pak_sa 0x197: pak_sa 0x1cf:
	    pak_sa 0x208: pak_sa 0x20a: pak_sa 0x1e2c: pak_sa 0x1e2e:
	    pak_sa 0x1ec8: pak_sa 0x1eca:
		    EMIT2('I') EMIT2(I_grave) EMIT2(I_acute)
		    EMIT2(I_circumflex) EMIT2(I_diaeresis)
		    EMIT2(0x128) EMIT2(0x12a) EMIT2(0x12c)
		    EMIT2(0x12e) EMIT2(0x130) EMIT2(0x197)
		    EMIT2(0x1cf) EMIT2(0x208) EMIT2(0x20a)
		    EMIT2(0x1e2c) EMIT2(0x1e2e) EMIT2(0x1ec8)
		    EMIT2(0x1eca)
		    คืน OK;

	    pak_sa 'J': pak_sa 0x134: pak_sa 0x248:
		    EMIT2('J') EMIT2(0x134) EMIT2(0x248)
		    คืน OK;

	    pak_sa 'K': pak_sa 0x136: pak_sa 0x198: pak_sa 0x1e8: pak_sa 0x1e30:
	    pak_sa 0x1e32: pak_sa 0x1e34: pak_sa 0x2c69: pak_sa 0xa740:
		    EMIT2('K') EMIT2(0x136) EMIT2(0x198) EMIT2(0x1e8)
		    EMIT2(0x1e30) EMIT2(0x1e32) EMIT2(0x1e34)
		    EMIT2(0x2c69) EMIT2(0xa740)
		    คืน OK;

	    pak_sa 'L': pak_sa 0x139: pak_sa 0x13b: pak_sa 0x13d:
	    pak_sa 0x13f: pak_sa 0x141: pak_sa 0x23d: pak_sa 0x1e36:
	    pak_sa 0x1e38: pak_sa 0x1e3a: pak_sa 0x1e3c: pak_sa 0x2c60:
		    EMIT2('L') EMIT2(0x139) EMIT2(0x13b)
		    EMIT2(0x13d) EMIT2(0x13f) EMIT2(0x141)
		    EMIT2(0x23d) EMIT2(0x1e36) EMIT2(0x1e38)
		    EMIT2(0x1e3a) EMIT2(0x1e3c) EMIT2(0x2c60)
		    คืน OK;

	    pak_sa 'M': pak_sa 0x1e3e: pak_sa 0x1e40: pak_sa 0x1e42:
		    EMIT2('M') EMIT2(0x1e3e) EMIT2(0x1e40)
		    EMIT2(0x1e42)
		    คืน OK;

	    pak_sa 'N': pak_sa N_virguilla:
	    pak_sa 0x143: pak_sa 0x145: pak_sa 0x147: pak_sa 0x1f8:
	    pak_sa 0x1e44: pak_sa 0x1e46: pak_sa 0x1e48: pak_sa 0x1e4a:
	    pak_sa 0xa7a4:
		    EMIT2('N') EMIT2(N_virguilla)
		    EMIT2(0x143) EMIT2(0x145) EMIT2(0x147)
		    EMIT2(0x1f8) EMIT2(0x1e44) EMIT2(0x1e46)
		    EMIT2(0x1e48) EMIT2(0x1e4a) EMIT2(0xa7a4)
		    คืน OK;

	    pak_sa 'O': pak_sa O_grave: pak_sa O_acute: pak_sa O_circumflex:
	    pak_sa O_virguilla: pak_sa O_diaeresis: pak_sa O_slash:
	    pak_sa 0x14c: pak_sa 0x14e: pak_sa 0x150: pak_sa 0x19f:
	    pak_sa 0x1a0: pak_sa 0x1d1: pak_sa 0x1ea: pak_sa 0x1ec:
	    pak_sa 0x1fe: pak_sa 0x20c: pak_sa 0x20e: pak_sa 0x22a:
	    pak_sa 0x22c: pak_sa 0x22e: pak_sa 0x230: pak_sa 0x1e4c:
	    pak_sa 0x1e4e: pak_sa 0x1e50: pak_sa 0x1e52: pak_sa 0x1ecc:
	    pak_sa 0x1ece: pak_sa 0x1ed0: pak_sa 0x1ed2: pak_sa 0x1ed4:
	    pak_sa 0x1ed6: pak_sa 0x1ed8: pak_sa 0x1eda: pak_sa 0x1edc:
	    pak_sa 0x1ede: pak_sa 0x1ee0: pak_sa 0x1ee2:
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
		    คืน OK;

	    pak_sa 'P': pak_sa 0x1a4: pak_sa 0x1e54: pak_sa 0x1e56: pak_sa 0x2c63:
		    EMIT2('P') EMIT2(0x1a4) EMIT2(0x1e54) EMIT2(0x1e56)
		    EMIT2(0x2c63)
		    คืน OK;

	    pak_sa 'Q': pak_sa 0x24a:
		    EMIT2('Q') EMIT2(0x24a)
		    คืน OK;

	    pak_sa 'R': pak_sa 0x154: pak_sa 0x156: pak_sa 0x158: pak_sa 0x210:
	    pak_sa 0x212: pak_sa 0x24c: pak_sa 0x1e58: pak_sa 0x1e5a:
	    pak_sa 0x1e5c: pak_sa 0x1e5e: pak_sa 0x2c64: pak_sa 0xa7a6:
		    EMIT2('R') EMIT2(0x154) EMIT2(0x156) EMIT2(0x158)
		    EMIT2(0x210) EMIT2(0x212) EMIT2(0x24c) EMIT2(0x1e58)
		    EMIT2(0x1e5a) EMIT2(0x1e5c) EMIT2(0x1e5e) EMIT2(0x2c64)
		    EMIT2(0xa7a6)
		    คืน OK;

	    pak_sa 'S': pak_sa 0x15a: pak_sa 0x15c: pak_sa 0x15e: pak_sa 0x160:
	    pak_sa 0x218: pak_sa 0x1e60: pak_sa 0x1e62: pak_sa 0x1e64:
	    pak_sa 0x1e66: pak_sa 0x1e68: pak_sa 0x2c7e: pak_sa 0xa7a8:
		    EMIT2('S') EMIT2(0x15a) EMIT2(0x15c) EMIT2(0x15e)
		    EMIT2(0x160) EMIT2(0x218) EMIT2(0x1e60) EMIT2(0x1e62)
		    EMIT2(0x1e64) EMIT2(0x1e66) EMIT2(0x1e68) EMIT2(0x2c7e)
		    EMIT2(0xa7a8)
		    คืน OK;

	    pak_sa 'T': pak_sa 0x162: pak_sa 0x164: pak_sa 0x166: pak_sa 0x1ac:
	    pak_sa 0x1ae: pak_sa 0x21a: pak_sa 0x23e: pak_sa 0x1e6a: pak_sa 0x1e6c:
	    pak_sa 0x1e6e: pak_sa 0x1e70:
		    EMIT2('T') EMIT2(0x162) EMIT2(0x164) EMIT2(0x166)
		    EMIT2(0x1ac) EMIT2(0x1ae) EMIT2(0x23e) EMIT2(0x21a)
		    EMIT2(0x1e6a) EMIT2(0x1e6c) EMIT2(0x1e6e) EMIT2(0x1e70)
		    คืน OK;

	    pak_sa 'U': pak_sa U_grave: pak_sa U_acute: pak_sa U_diaeresis:
	    pak_sa U_circumflex: pak_sa 0x168: pak_sa 0x16a: pak_sa 0x16c:
	    pak_sa 0x16e: pak_sa 0x170: pak_sa 0x172: pak_sa 0x1af:
	    pak_sa 0x1d3: pak_sa 0x1d5: pak_sa 0x1d7: pak_sa 0x1d9:
	    pak_sa 0x1db: pak_sa 0x214: pak_sa 0x216: pak_sa 0x244:
	    pak_sa 0x1e72: pak_sa 0x1e74: pak_sa 0x1e76: pak_sa 0x1e78:
	    pak_sa 0x1e7a: pak_sa 0x1ee4: pak_sa 0x1ee6: pak_sa 0x1ee8:
	    pak_sa 0x1eea: pak_sa 0x1eec: pak_sa 0x1eee: pak_sa 0x1ef0:
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
		    คืน OK;

	    pak_sa 'V': pak_sa 0x1b2: pak_sa 0x1e7c: pak_sa 0x1e7e:
		    EMIT2('V') EMIT2(0x1b2) EMIT2(0x1e7c) EMIT2(0x1e7e)
		    คืน OK;

	    pak_sa 'W': pak_sa 0x174: pak_sa 0x1e80: pak_sa 0x1e82: pak_sa 0x1e84:
	    pak_sa 0x1e86: pak_sa 0x1e88:
		    EMIT2('W') EMIT2(0x174) EMIT2(0x1e80) EMIT2(0x1e82)
		    EMIT2(0x1e84) EMIT2(0x1e86) EMIT2(0x1e88)
		    คืน OK;

	    pak_sa 'X': pak_sa 0x1e8a: pak_sa 0x1e8c:
		    EMIT2('X') EMIT2(0x1e8a) EMIT2(0x1e8c)
		    คืน OK;

	    pak_sa 'Y': pak_sa Y_acute: pak_sa 0x176: pak_sa 0x178:
	    pak_sa 0x1b3: pak_sa 0x232: pak_sa 0x24e: pak_sa 0x1e8e:
	    pak_sa 0x1ef2: pak_sa 0x1ef4: pak_sa 0x1ef6: pak_sa 0x1ef8:
		    EMIT2('Y') EMIT2(Y_acute)
		    EMIT2(0x176) EMIT2(0x178) EMIT2(0x1b3)
		    EMIT2(0x232) EMIT2(0x24e) EMIT2(0x1e8e)
		    EMIT2(0x1ef2) EMIT2(0x1ef4) EMIT2(0x1ef6)
		    EMIT2(0x1ef8)
		    คืน OK;

	    pak_sa 'Z': pak_sa 0x179: pak_sa 0x17b: pak_sa 0x17d:
	    pak_sa 0x1b5: pak_sa 0x1e90: pak_sa 0x1e92: pak_sa 0x1e94:
	    pak_sa 0x2c6b:
		    EMIT2('Z') EMIT2(0x179) EMIT2(0x17b) EMIT2(0x17d)
		    EMIT2(0x1b5) EMIT2(0x1e90) EMIT2(0x1e92)
		    EMIT2(0x1e94) EMIT2(0x2c6b)
		    คืน OK;

	    pak_sa 'a': pak_sa a_grave: pak_sa a_acute: pak_sa a_circumflex:
	    pak_sa a_virguilla: pak_sa a_diaeresis: pak_sa a_ring:
	    pak_sa 0x101: pak_sa 0x103: pak_sa 0x105: pak_sa 0x1ce:
	    pak_sa 0x1df: pak_sa 0x1e1: pak_sa 0x1fb: pak_sa 0x201:
	    pak_sa 0x203: pak_sa 0x227: pak_sa 0x1d8f: pak_sa 0x1e01:
	    pak_sa 0x1e9a: pak_sa 0x1ea1: pak_sa 0x1ea3: pak_sa 0x1ea5:
	    pak_sa 0x1ea7: pak_sa 0x1ea9: pak_sa 0x1eab: pak_sa 0x1ead:
	    pak_sa 0x1eaf: pak_sa 0x1eb1: pak_sa 0x1eb3: pak_sa 0x1eb5:
	    pak_sa 0x1eb7: pak_sa 0x2c65:
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
		    คืน OK;

	    pak_sa 'b': pak_sa 0x180: pak_sa 0x253: pak_sa 0x1d6c: pak_sa 0x1d80:
	    pak_sa 0x1e03: pak_sa 0x1e05: pak_sa 0x1e07:
		    EMIT2('b') EMIT2(0x180) EMIT2(0x253) EMIT2(0x1d6c)
		    EMIT2(0x1d80) EMIT2(0x1e03) EMIT2(0x1e05) EMIT2(0x1e07)
		    คืน OK;

	    pak_sa 'c': pak_sa c_cedilla: pak_sa 0x107: pak_sa 0x109: pak_sa 0x10b:
	    pak_sa 0x10d: pak_sa 0x188: pak_sa 0x23c: pak_sa 0x1e09: pak_sa 0xa793:
	    pak_sa 0xa794:
		    EMIT2('c') EMIT2(c_cedilla)
		    EMIT2(0x107) EMIT2(0x109) EMIT2(0x10b)
		    EMIT2(0x10d) EMIT2(0x188) EMIT2(0x23c)
		    EMIT2(0x1e09) EMIT2(0xa793) EMIT2(0xa794)
		    คืน OK;

	    pak_sa 'd': pak_sa 0x10f: pak_sa 0x111: pak_sa 0x257: pak_sa 0x1d6d:
	    pak_sa 0x1d81: pak_sa 0x1d91: pak_sa 0x1e0b: pak_sa 0x1e0d: pak_sa 0x1e0f:
	    pak_sa 0x1e11: pak_sa 0x1e13:
		    EMIT2('d') EMIT2(0x10f) EMIT2(0x111)
		    EMIT2(0x257) EMIT2(0x1d6d) EMIT2(0x1d81)
		    EMIT2(0x1d91) EMIT2(0x1e0b) EMIT2(0x1e0d)
		    EMIT2(0x1e0f) EMIT2(0x1e11) EMIT2(0x1e13)
		    คืน OK;

	    pak_sa 'e': pak_sa e_grave: pak_sa e_acute: pak_sa e_circumflex:
	    pak_sa e_diaeresis: pak_sa 0x113: pak_sa 0x115: pak_sa 0x117:
	    pak_sa 0x119: pak_sa 0x11b: pak_sa 0x205: pak_sa 0x207:
	    pak_sa 0x229: pak_sa 0x247: pak_sa 0x1d92: pak_sa 0x1e15:
	    pak_sa 0x1e17: pak_sa 0x1e19: pak_sa 0x1e1b: pak_sa 0x1e1d:
	    pak_sa 0x1eb9: pak_sa 0x1ebb: pak_sa 0x1ebd: pak_sa 0x1ebf:
	    pak_sa 0x1ec1: pak_sa 0x1ec3: pak_sa 0x1ec5: pak_sa 0x1ec7:
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
		    คืน OK;

	    pak_sa 'f': pak_sa 0x192: pak_sa 0x1d6e: pak_sa 0x1d82:
	    pak_sa 0x1e1f: pak_sa 0xa799:
		    EMIT2('f') EMIT2(0x192) EMIT2(0x1d6e) EMIT2(0x1d82)
		    EMIT2(0x1e1f) EMIT2(0xa799)
		    คืน OK;

	    pak_sa 'g': pak_sa 0x11d: pak_sa 0x11f: pak_sa 0x121: pak_sa 0x123:
	    pak_sa 0x1e5: pak_sa 0x1e7: pak_sa 0x1f5: pak_sa 0x260: pak_sa 0x1d83:
	    pak_sa 0x1e21: pak_sa 0xa7a1:
		    EMIT2('g') EMIT2(0x11d) EMIT2(0x11f) EMIT2(0x121)
		    EMIT2(0x123) EMIT2(0x1e5) EMIT2(0x1e7)
		    EMIT2(0x1f5) EMIT2(0x260) EMIT2(0x1d83)
		    EMIT2(0x1e21) EMIT2(0xa7a1)
		    คืน OK;

	    pak_sa 'h': pak_sa 0x125: pak_sa 0x127: pak_sa 0x21f: pak_sa 0x1e23:
	    pak_sa 0x1e25: pak_sa 0x1e27: pak_sa 0x1e29: pak_sa 0x1e2b:
	    pak_sa 0x1e96: pak_sa 0x2c68: pak_sa 0xa795:
		    EMIT2('h') EMIT2(0x125) EMIT2(0x127) EMIT2(0x21f)
		    EMIT2(0x1e23) EMIT2(0x1e25) EMIT2(0x1e27)
		    EMIT2(0x1e29) EMIT2(0x1e2b) EMIT2(0x1e96)
		    EMIT2(0x2c68) EMIT2(0xa795)
		    คืน OK;

	    pak_sa 'i': pak_sa i_grave: pak_sa i_acute: pak_sa i_circumflex:
	    pak_sa i_diaeresis: pak_sa 0x129: pak_sa 0x12b: pak_sa 0x12d:
	    pak_sa 0x12f: pak_sa 0x1d0: pak_sa 0x209: pak_sa 0x20b:
	    pak_sa 0x268: pak_sa 0x1d96: pak_sa 0x1e2d: pak_sa 0x1e2f:
	    pak_sa 0x1ec9: pak_sa 0x1ecb:
		    EMIT2('i') EMIT2(i_grave) EMIT2(i_acute)
		    EMIT2(i_circumflex) EMIT2(i_diaeresis)
		    EMIT2(0x129) EMIT2(0x12b) EMIT2(0x12d)
		    EMIT2(0x12f) EMIT2(0x1d0) EMIT2(0x209)
		    EMIT2(0x20b) EMIT2(0x268) EMIT2(0x1d96)
		    EMIT2(0x1e2d) EMIT2(0x1e2f) EMIT2(0x1ec9)
		    EMIT2(0x1ecb)
		    คืน OK;

	    pak_sa 'j': pak_sa 0x135: pak_sa 0x1f0: pak_sa 0x249:
		    EMIT2('j') EMIT2(0x135) EMIT2(0x1f0) EMIT2(0x249)
		    คืน OK;

	    pak_sa 'k': pak_sa 0x137: pak_sa 0x199: pak_sa 0x1e9: pak_sa 0x1d84:
	    pak_sa 0x1e31: pak_sa 0x1e33: pak_sa 0x1e35: pak_sa 0x2c6a: pak_sa 0xa741:
		    EMIT2('k') EMIT2(0x137) EMIT2(0x199) EMIT2(0x1e9)
		    EMIT2(0x1d84) EMIT2(0x1e31) EMIT2(0x1e33)
		    EMIT2(0x1e35) EMIT2(0x2c6a) EMIT2(0xa741)
		    คืน OK;

	    pak_sa 'l': pak_sa 0x13a: pak_sa 0x13c: pak_sa 0x13e: pak_sa 0x140:
	    pak_sa 0x142: pak_sa 0x19a: pak_sa 0x1e37: pak_sa 0x1e39: pak_sa 0x1e3b:
	    pak_sa 0x1e3d: pak_sa 0x2c61:
		    EMIT2('l') EMIT2(0x13a) EMIT2(0x13c)
		    EMIT2(0x13e) EMIT2(0x140) EMIT2(0x142)
		    EMIT2(0x19a) EMIT2(0x1e37) EMIT2(0x1e39)
		    EMIT2(0x1e3b) EMIT2(0x1e3d) EMIT2(0x2c61)
		    คืน OK;

	    pak_sa 'm': pak_sa 0x1d6f: pak_sa 0x1e3f: pak_sa 0x1e41: pak_sa 0x1e43:
		    EMIT2('m') EMIT2(0x1d6f) EMIT2(0x1e3f)
		    EMIT2(0x1e41) EMIT2(0x1e43)
		    คืน OK;

	    pak_sa 'n': pak_sa n_virguilla: pak_sa 0x144: pak_sa 0x146: pak_sa 0x148:
	    pak_sa 0x149: pak_sa 0x1f9: pak_sa 0x1d70: pak_sa 0x1d87: pak_sa 0x1e45:
	    pak_sa 0x1e47: pak_sa 0x1e49: pak_sa 0x1e4b: pak_sa 0xa7a5:
		    EMIT2('n') EMIT2(n_virguilla)
		    EMIT2(0x144) EMIT2(0x146) EMIT2(0x148)
		    EMIT2(0x149) EMIT2(0x1f9) EMIT2(0x1d70)
		    EMIT2(0x1d87) EMIT2(0x1e45) EMIT2(0x1e47)
		    EMIT2(0x1e49) EMIT2(0x1e4b) EMIT2(0xa7a5)
		    คืน OK;

	    pak_sa 'o': pak_sa o_grave: pak_sa o_acute: pak_sa o_circumflex:
	    pak_sa o_virguilla: pak_sa o_diaeresis: pak_sa o_slash:
	    pak_sa 0x14d: pak_sa 0x14f: pak_sa 0x151: pak_sa 0x1a1:
	    pak_sa 0x1d2: pak_sa 0x1eb: pak_sa 0x1ed: pak_sa 0x1ff:
	    pak_sa 0x20d: pak_sa 0x20f: pak_sa 0x22b: pak_sa 0x22d:
	    pak_sa 0x22f: pak_sa 0x231: pak_sa 0x275: pak_sa 0x1e4d:
	    pak_sa 0x1e4f: pak_sa 0x1e51: pak_sa 0x1e53: pak_sa 0x1ecd:
	    pak_sa 0x1ecf: pak_sa 0x1ed1: pak_sa 0x1ed3: pak_sa 0x1ed5:
	    pak_sa 0x1ed7: pak_sa 0x1ed9: pak_sa 0x1edb: pak_sa 0x1edd:
	    pak_sa 0x1edf: pak_sa 0x1ee1: pak_sa 0x1ee3:
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
		    คืน OK;

	    pak_sa 'p': pak_sa 0x1a5: pak_sa 0x1d71: pak_sa 0x1d7d: pak_sa 0x1d88:
	    pak_sa 0x1e55: pak_sa 0x1e57:
		    EMIT2('p') EMIT2(0x1a5) EMIT2(0x1d71) EMIT2(0x1d7d)
		    EMIT2(0x1d88) EMIT2(0x1e55) EMIT2(0x1e57)
		    คืน OK;

	    pak_sa 'q': pak_sa 0x24b: pak_sa 0x2a0:
		    EMIT2('q') EMIT2(0x24b) EMIT2(0x2a0)
		    คืน OK;

	    pak_sa 'r': pak_sa 0x155: pak_sa 0x157: pak_sa 0x159: pak_sa 0x211:
	    pak_sa 0x213: pak_sa 0x24d: pak_sa 0x27d: pak_sa 0x1d72: pak_sa 0x1d73:
	    pak_sa 0x1d89: pak_sa 0x1e59: pak_sa 0x1e5b: pak_sa 0x1e5d: pak_sa 0x1e5f:
	    pak_sa 0xa7a7:
		    EMIT2('r') EMIT2(0x155) EMIT2(0x157) EMIT2(0x159)
		    EMIT2(0x211) EMIT2(0x213) EMIT2(0x24d) EMIT2(0x27d)
		    EMIT2(0x1d72) EMIT2(0x1d73) EMIT2(0x1d89) EMIT2(0x1e59)
		    EMIT2(0x1e5b) EMIT2(0x1e5d) EMIT2(0x1e5f) EMIT2(0xa7a7)
		    คืน OK;

	    pak_sa 's': pak_sa 0x15b: pak_sa 0x15d: pak_sa 0x15f: pak_sa 0x161:
	    pak_sa 0x219: pak_sa 0x23f: pak_sa 0x1d74: pak_sa 0x1d8a: pak_sa 0x1e61:
	    pak_sa 0x1e63: pak_sa 0x1e65: pak_sa 0x1e67: pak_sa 0x1e69: pak_sa 0xa7a9:
		    EMIT2('s') EMIT2(0x15b) EMIT2(0x15d) EMIT2(0x15f)
		    EMIT2(0x161) EMIT2(0x219) EMIT2(0x23f) EMIT2(0x1d74)
		    EMIT2(0x1d8a) EMIT2(0x1e61) EMIT2(0x1e63) EMIT2(0x1e65)
		    EMIT2(0x1e67) EMIT2(0x1e69) EMIT2(0xa7a9)
		    คืน OK;

	    pak_sa 't': pak_sa 0x163: pak_sa 0x165: pak_sa 0x167: pak_sa 0x1ab:
	    pak_sa 0x1ad: pak_sa 0x21b: pak_sa 0x288: pak_sa 0x1d75: pak_sa 0x1e6b:
	    pak_sa 0x1e6d: pak_sa 0x1e6f: pak_sa 0x1e71: pak_sa 0x1e97: pak_sa 0x2c66:
		    EMIT2('t') EMIT2(0x163) EMIT2(0x165) EMIT2(0x167)
		    EMIT2(0x1ab) EMIT2(0x1ad) EMIT2(0x21b) EMIT2(0x288)
		    EMIT2(0x1d75) EMIT2(0x1e6b) EMIT2(0x1e6d) EMIT2(0x1e6f)
		    EMIT2(0x1e71) EMIT2(0x1e97) EMIT2(0x2c66)
		    คืน OK;

	    pak_sa 'u': pak_sa u_grave: pak_sa u_acute: pak_sa u_circumflex:
	    pak_sa u_diaeresis: pak_sa 0x169: pak_sa 0x16b: pak_sa 0x16d:
	    pak_sa 0x16f: pak_sa 0x171: pak_sa 0x173: pak_sa 0x1b0: pak_sa 0x1d4:
	    pak_sa 0x1d6: pak_sa 0x1d8: pak_sa 0x1da: pak_sa 0x1dc: pak_sa 0x215:
	    pak_sa 0x217: pak_sa 0x289: pak_sa 0x1d7e: pak_sa 0x1d99: pak_sa 0x1e73:
	    pak_sa 0x1e75: pak_sa 0x1e77: pak_sa 0x1e79: pak_sa 0x1e7b:
	    pak_sa 0x1ee5: pak_sa 0x1ee7: pak_sa 0x1ee9: pak_sa 0x1eeb:
	    pak_sa 0x1eed: pak_sa 0x1eef: pak_sa 0x1ef1:
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
		    คืน OK;

	    pak_sa 'v': pak_sa 0x28b: pak_sa 0x1d8c: pak_sa 0x1e7d: pak_sa 0x1e7f:
		    EMIT2('v') EMIT2(0x28b) EMIT2(0x1d8c) EMIT2(0x1e7d)
		    EMIT2(0x1e7f)
		    คืน OK;

	    pak_sa 'w': pak_sa 0x175: pak_sa 0x1e81: pak_sa 0x1e83: pak_sa 0x1e85:
	    pak_sa 0x1e87: pak_sa 0x1e89: pak_sa 0x1e98:
		    EMIT2('w') EMIT2(0x175) EMIT2(0x1e81) EMIT2(0x1e83)
		    EMIT2(0x1e85) EMIT2(0x1e87) EMIT2(0x1e89) EMIT2(0x1e98)
		    คืน OK;

	    pak_sa 'x': pak_sa 0x1e8b: pak_sa 0x1e8d:
		    EMIT2('x') EMIT2(0x1e8b) EMIT2(0x1e8d)
		    คืน OK;

	    pak_sa 'y': pak_sa y_acute: pak_sa y_diaeresis: pak_sa 0x177:
	    pak_sa 0x1b4: pak_sa 0x233: pak_sa 0x24f: pak_sa 0x1e8f:
	    pak_sa 0x1e99: pak_sa 0x1ef3: pak_sa 0x1ef5: pak_sa 0x1ef7:
	    pak_sa 0x1ef9:
		    EMIT2('y') EMIT2(y_acute) EMIT2(y_diaeresis)
		    EMIT2(0x177) EMIT2(0x1b4) EMIT2(0x233) EMIT2(0x24f)
		    EMIT2(0x1e8f) EMIT2(0x1e99) EMIT2(0x1ef3)
		    EMIT2(0x1ef5) EMIT2(0x1ef7) EMIT2(0x1ef9)
		    คืน OK;

	    pak_sa 'z': pak_sa 0x17a: pak_sa 0x17c: pak_sa 0x17e: pak_sa 0x1b6:
	    pak_sa 0x1d76: pak_sa 0x1d8e: pak_sa 0x1e91: pak_sa 0x1e93:
	    pak_sa 0x1e95: pak_sa 0x2c6c:
		    EMIT2('z') EMIT2(0x17a) EMIT2(0x17c) EMIT2(0x17e)
		    EMIT2(0x1b6) EMIT2(0x1d76) EMIT2(0x1d8e) EMIT2(0x1e91)
		    EMIT2(0x1e93) EMIT2(0x1e95) EMIT2(0x2c6c)
		    คืน OK;

	    // default: character itself
	}
    }

    EMIT2(c);
    คืน OK;
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
    static จำนวนเต็ม
nfa_regatom(void)
{
    จำนวนเต็ม		c;
    จำนวนเต็ม		charclass;
    จำนวนเต็ม		equiclass;
    จำนวนเต็ม		collclass;
    จำนวนเต็ม		got_coll_char;
    char_u	*p;
    char_u	*endp;
    char_u	*old_regparse = regparse;
    จำนวนเต็ม		extra = 0;
    จำนวนเต็ม		emit_range;
    จำนวนเต็ม		negated;
    จำนวนเต็ม		result;
    จำนวนเต็ม		startc = -1;
    จำนวนเต็ม		save_prev_at_start = prev_at_start;

    c = getchr();
    switch (c)
    {
	pak_sa NUL:
	    EMSG_RET_FAIL(_(e_nfa_regexp_end_encountered_prematurely));

	pak_sa Magic('^'):
	    EMIT(NFA_BOL);
	    bha_nga;

	pak_sa Magic('$'):
	    EMIT(NFA_EOL);
#ถ้า defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#endif
	    bha_nga;

	pak_sa Magic('<'):
	    EMIT(NFA_BOW);
	    bha_nga;

	pak_sa Magic('>'):
	    EMIT(NFA_EOW);
	    bha_nga;

	pak_sa Magic('_'):
	    c = no_Magic(getchr());
	    ถ้า (c == NUL)
		EMSG_RET_FAIL(_(e_nfa_regexp_end_encountered_prematurely));

	    ถ้า (c == '^')	// "\_^" is start-of-line
	    {
		EMIT(NFA_BOL);
		bha_nga;
	    }
	    ถ้า (c == '$')	// "\_$" is end-of-line
	    {
		EMIT(NFA_EOL);
#ถ้า defined(FEAT_SYN_HL)
		had_eol = TRUE;
#endif
		bha_nga;
	    }

	    extra = NFA_ADD_NL;

	    // "\_[" is collection plus newline
	    ถ้า (c == '[')
		goto collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	/*
	 * Character classes.
	 */
	pak_sa Magic('.'):
	pak_sa Magic('i'):
	pak_sa Magic('I'):
	pak_sa Magic('k'):
	pak_sa Magic('K'):
	pak_sa Magic('f'):
	pak_sa Magic('F'):
	pak_sa Magic('p'):
	pak_sa Magic('P'):
	pak_sa Magic('s'):
	pak_sa Magic('S'):
	pak_sa Magic('d'):
	pak_sa Magic('D'):
	pak_sa Magic('x'):
	pak_sa Magic('X'):
	pak_sa Magic('o'):
	pak_sa Magic('O'):
	pak_sa Magic('w'):
	pak_sa Magic('W'):
	pak_sa Magic('h'):
	pak_sa Magic('H'):
	pak_sa Magic('a'):
	pak_sa Magic('A'):
	pak_sa Magic('l'):
	pak_sa Magic('L'):
	pak_sa Magic('u'):
	pak_sa Magic('U'):
	    p = vim_strchr(classchars, no_Magic(c));
	    ถ้า (p == NULL)
	    {
		ถ้า (extra == NFA_ADD_NL)
		{
		    semsg(_(e_nfa_regexp_invalid_character_class_nr), c);
		    rc_did_emsg = TRUE;
		    คืน FAIL;
		}
		siemsg("Unknown character class char: %d", c);
		คืน FAIL;
	    }

	    // When '.' is followed by a composing char ignore the dot, so that
	    // the composing char is matched here.
	    ถ้า (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	    {
		old_regparse = regparse;
		c = getchr();
		goto nfa_do_multibyte;
	    }
	    EMIT(nfa_classcodes[p - classchars]);
	    ถ้า (extra == NFA_ADD_NL)
	    {
		EMIT(NFA_NEWL);
		EMIT(NFA_OR);
		regflags |= RF_HASNL;
	    }
	    bha_nga;

	pak_sa Magic('n'):
	    ถ้า (reg_string)
		// In a string "\n" matches a newline character.
		EMIT(NL);
	    มิฉะนั้น
	    {
		// In buffer text "\n" matches the end of a line.
		EMIT(NFA_NEWL);
		regflags |= RF_HASNL;
	    }
	    bha_nga;

	pak_sa Magic('('):
	    ถ้า (nfa_reg(REG_PAREN) == FAIL)
		คืน FAIL;	    // cascaded error
	    bha_nga;

	pak_sa Magic('|'):
	pak_sa Magic('&'):
	pak_sa Magic(')'):
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    คืน FAIL;

	pak_sa Magic('='):
	pak_sa Magic('?'):
	pak_sa Magic('+'):
	pak_sa Magic('@'):
	pak_sa Magic('*'):
	pak_sa Magic('{'):
	    // these should follow an atom, not form an atom
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    คืน FAIL;

	pak_sa Magic('~'):
	    {
		char_u	    *lp;

		// Previous substitute pattern.
		// Generated as "\%(pattern\)".
		ถ้า (reg_prev_sub == NULL)
		{
		    emsg(_(e_no_previous_substitute_regular_expression));
		    คืน FAIL;
		}
		สำหรับ (lp = reg_prev_sub; *lp != NUL; MB_CPTR_ADV(lp))
		{
		    EMIT(PTR2CHAR(lp));
		    ถ้า (lp != reg_prev_sub)
			EMIT(NFA_CONCAT);
		}
		EMIT(NFA_NOPEN);
		bha_nga;
	    }

	pak_sa Magic('1'):
	pak_sa Magic('2'):
	pak_sa Magic('3'):
	pak_sa Magic('4'):
	pak_sa Magic('5'):
	pak_sa Magic('6'):
	pak_sa Magic('7'):
	pak_sa Magic('8'):
	pak_sa Magic('9'):
	    {
		จำนวนเต็ม refnum = no_Magic(c) - '1';

		ถ้า (!seen_endbrace(refnum + 1))
		    คืน FAIL;
		EMIT(NFA_BACKREF1 + refnum);
		rex.nfa_has_backref = TRUE;
	    }
	    bha_nga;

	pak_sa Magic('z'):
	    c = no_Magic(getchr());
	    switch (c)
	    {
		pak_sa 's':
		    EMIT(NFA_ZSTART);
		    ถ้า (re_mult_next("\\zs") == FAIL)
			คืน FAIL;
		    bha_nga;
		pak_sa 'e':
		    EMIT(NFA_ZEND);
		    rex.nfa_has_zend = TRUE;
		    ถ้า (re_mult_next("\\ze") == FAIL)
			คืน FAIL;
		    bha_nga;
#ifdef FEAT_SYN_HL
		pak_sa '1':
		pak_sa '2':
		pak_sa '3':
		pak_sa '4':
		pak_sa '5':
		pak_sa '6':
		pak_sa '7':
		pak_sa '8':
		pak_sa '9':
		    // \z1...\z9
		    ถ้า ((reg_do_extmatch & REX_USE) == 0)
			EMSG_RET_FAIL(_(e_z1_z9_not_allowed_here));
		    EMIT(NFA_ZREF1 + (no_Magic(c) - '1'));
		    // No need to set rex.nfa_has_backref, the sub-matches don't
		    // change when \z1 .. \z9 matches or not.
		    re_has_z = REX_USE;
		    bha_nga;
		pak_sa '(':
		    // \z(
		    ถ้า ((reg_do_extmatch & REX_SET) == 0)
			EMSG_RET_FAIL(_(e_z_not_allowed_here));
		    ถ้า (nfa_reg(REG_ZPAREN) == FAIL)
			คืน FAIL;	    // cascaded error
		    re_has_z = REX_SET;
		    bha_nga;
#endif
		default:
		    semsg(_(e_nfa_regexp_unknown_operator_z_chr), no_Magic(c));
		    คืน FAIL;
	    }
	    bha_nga;

	pak_sa Magic('%'):
	    c = no_Magic(getchr());
	    switch (c)
	    {
		// () without a back reference
		pak_sa '(':
		    ถ้า (nfa_reg(REG_NPAREN) == FAIL)
			คืน FAIL;
		    EMIT(NFA_NOPEN);
		    bha_nga;

		pak_sa 'd':   // %d123 decimal
		pak_sa 'o':   // %o123 octal
		pak_sa 'x':   // %xab hex 2
		pak_sa 'u':   // %uabcd hex 4
		pak_sa 'U':   // %U1234abcd hex 8
		    {
			vimlong_T nr;

			switch (c)
			{
			    pak_sa 'd': nr = getdecchrs(); bha_nga;
			    pak_sa 'o': nr = getoctchrs(); bha_nga;
			    pak_sa 'x': nr = gethexchrs(2); bha_nga;
			    pak_sa 'u': nr = gethexchrs(4); bha_nga;
			    pak_sa 'U': nr = gethexchrs(8); bha_nga;
			    default:  nr = -1; bha_nga;
			}

			ถ้า (nr < 0 || nr > INT_MAX)
			    EMSG2_RET_FAIL(_(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			// A NUL is stored in the text as NL
			// TODO: what if a composing character follows?
			EMIT(nr == 0 ? 0x0a : (long)nr);
		    }
		    bha_nga;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		pak_sa '^':
		    EMIT(NFA_BOF);
		    bha_nga;

		pak_sa '$':
		    EMIT(NFA_EOF);
		    bha_nga;

		pak_sa '#':
		    ถ้า (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_start_of_pattern),
								  regparse[1]);
			คืน FAIL;
		    }
		    EMIT(NFA_CURSOR);
		    bha_nga;

		pak_sa 'V':
		    EMIT(NFA_VISUAL);
		    bha_nga;

		pak_sa 'C':
		    EMIT(NFA_ANY_COMPOSING);
		    bha_nga;

		pak_sa '[':
		    {
			จำนวนเต็ม	    n;

			// \%[abc]
			สำหรับ (n = 0; (c = peekchr()) != ']'; ++n)
			{
			    ถ้า (c == NUL)
				EMSG2_RET_FAIL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
			    // recursive call!
			    ถ้า (nfa_regatom() == FAIL)
				คืน FAIL;
			}
			getchr();  // get the ]
			ถ้า (n == 0)
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
			bha_nga;
		    }

		default:
		    {
			long_u	n = 0;
			จำนวนเต็ม	cmp = c;
			จำนวนเต็ม	cur = FALSE;
			จำนวนเต็ม	got_digit = FALSE;

			ถ้า (c == '<' || c == '>')
			    c = getchr();
			ถ้า (no_Magic(c) == '.')
			{
			    cur = TRUE;
			    c = getchr();
			}
			ขณะ (VIM_ISDIGIT(c))
			{
			    long_u tmp;

			    ถ้า (cur)
			    {
				semsg(_(e_regexp_number_after_dot_pos_search_chr),
								  no_Magic(c));
				คืน FAIL;
			    }
			    tmp = n * 10 + (c - '0');

			    ถ้า (tmp < n)
			    {
				// overflow.
				emsg(_(e_percent_value_too_large));
				คืน FAIL;
			    }
			    n = tmp;
			    c = getchr();
			    got_digit = TRUE;
			}
			ถ้า (c == 'l' || c == 'c' || c == 'v')
			{
			    long_u limit = INT_MAX;

			    ถ้า (!cur && !got_digit)
			    {
				semsg(_(e_nfa_regexp_missing_value_in_chr),
								  no_Magic(c));
				คืน FAIL;
			    }
			    ถ้า (c == 'l')
			    {
				ถ้า (cur)
				    n = curwin->w_cursor.lnum;
				// \%{n}l  \%{n}<l  \%{n}>l
				EMIT(cmp == '<' ? NFA_LNUM_LT :
				     cmp == '>' ? NFA_LNUM_GT : NFA_LNUM);
				ถ้า (save_prev_at_start)
				    at_start = TRUE;
			    }
			    มิฉะนั้น ถ้า (c == 'c')
			    {
				ถ้า (cur)
				{
				    n = curwin->w_cursor.col;
				    n++;
				}
				// \%{n}c  \%{n}<c  \%{n}>c
				EMIT(cmp == '<' ? NFA_COL_LT :
				     cmp == '>' ? NFA_COL_GT : NFA_COL);
			    }
			    มิฉะนั้น
			    {
				ถ้า (cur)
				{
				    colnr_T vcol = 0;

				    getvvcol(curwin, &curwin->w_cursor,
							 NULL, NULL, &vcol, 0);
				    n = ++vcol;
				}
				// \%{n}v  \%{n}<v  \%{n}>v
				EMIT(cmp == '<' ? NFA_VCOL_LT :
				     cmp == '>' ? NFA_VCOL_GT : NFA_VCOL);
				limit = INT_MAX / MB_MAXBYTES;
			    }
			    ถ้า (n >= limit)
			    {
				emsg(_(e_percent_value_too_large));
				คืน FAIL;
			    }
			    EMIT((จำนวนเต็ม)n);
			    bha_nga;
			}
			มิฉะนั้น ถ้า (no_Magic(c) == '\'' && n == 0)
			{
			    // \%'m  \%<'m  \%>'m
			    EMIT(cmp == '<' ? NFA_MARK_LT :
				 cmp == '>' ? NFA_MARK_GT : NFA_MARK);
			    EMIT(getchr());
			    bha_nga;
			}
		    }
		    semsg(_(e_nfa_regexp_unknown_operator_percent_chr),
								  no_Magic(c));
		    คืน FAIL;
	    }
	    bha_nga;

	pak_sa Magic('['):
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
	    ถ้า (*endp == ']')
	    {
		จำนวนเต็ม plen;
		bool range_endpoint;
		/*
		 * Try to reverse engineer character classes. For example,
		 * recognize that [0-9] stands for \d and [A-Za-z_] for \h,
		 * and perform the necessary substitutions in the NFA.
		 */
		result = nfa_recognize_char_class(regparse, endp,
							 extra == NFA_ADD_NL);
		ถ้า (result != FAIL)
		{
		    ถ้า (result >= NFA_FIRST_NL && result <= NFA_LAST_NL)
		    {
			EMIT(result - NFA_ADD_NL);
			EMIT(NFA_NEWL);
			EMIT(NFA_OR);
		    }
		    มิฉะนั้น
			EMIT(result);
		    regparse = endp;
		    MB_PTR_ADV(regparse);
		    คืน OK;
		}
		/*
		 * Failed to recognize a character class. Use the simple
		 * version that turns [abc] into 'a' OR 'b' OR 'c'
		 */
		startc = -1;
		negated = FALSE;
		ถ้า (*regparse == '^')			// negated range
		{
		    negated = TRUE;
		    MB_PTR_ADV(regparse);
		    EMIT(NFA_START_NEG_COLL);
		}
		มิฉะนั้น
		    EMIT(NFA_START_COLL);
		ถ้า (*regparse == '-')
		{
		    startc = '-';
		    EMIT(startc);
		    EMIT(NFA_CONCAT);
		    MB_PTR_ADV(regparse);
		}
		// Emit the OR branches for each character in the []
		emit_range = FALSE;
		ขณะ (regparse < endp)
		{
		    จำนวนเต็ม	    oldstartc = startc;
		    range_endpoint = false;

		    startc = -1;
		    got_coll_char = FALSE;
		    ถ้า (*regparse == '[')
		    {
			// Check for [: :], [= =], [. .]
			equiclass = collclass = 0;
			charclass = get_char_class(&regparse);
			ถ้า (charclass == CLASS_NONE)
			{
			    equiclass = get_equi_class(&regparse);
			    ถ้า (equiclass == 0)
				collclass = get_coll_element(&regparse);
			}

			// Character class like [:alpha:]
			ถ้า (charclass != CLASS_NONE)
			{
			    switch (charclass)
			    {
				pak_sa CLASS_ALNUM:
				    EMIT(NFA_CLASS_ALNUM);
				    bha_nga;
				pak_sa CLASS_ALPHA:
				    EMIT(NFA_CLASS_ALPHA);
				    bha_nga;
				pak_sa CLASS_BLANK:
				    EMIT(NFA_CLASS_BLANK);
				    bha_nga;
				pak_sa CLASS_CNTRL:
				    EMIT(NFA_CLASS_CNTRL);
				    bha_nga;
				pak_sa CLASS_DIGIT:
				    EMIT(NFA_CLASS_DIGIT);
				    bha_nga;
				pak_sa CLASS_GRAPH:
				    EMIT(NFA_CLASS_GRAPH);
				    bha_nga;
				pak_sa CLASS_LOWER:
				    wants_nfa = TRUE;
				    EMIT(NFA_CLASS_LOWER);
				    bha_nga;
				pak_sa CLASS_PRINT:
				    EMIT(NFA_CLASS_PRINT);
				    bha_nga;
				pak_sa CLASS_PUNCT:
				    EMIT(NFA_CLASS_PUNCT);
				    bha_nga;
				pak_sa CLASS_SPACE:
				    EMIT(NFA_CLASS_SPACE);
				    bha_nga;
				pak_sa CLASS_UPPER:
				    wants_nfa = TRUE;
				    EMIT(NFA_CLASS_UPPER);
				    bha_nga;
				pak_sa CLASS_XDIGIT:
				    EMIT(NFA_CLASS_XDIGIT);
				    bha_nga;
				pak_sa CLASS_TAB:
				    EMIT(NFA_CLASS_TAB);
				    bha_nga;
				pak_sa CLASS_RETURN:
				    EMIT(NFA_CLASS_RETURN);
				    bha_nga;
				pak_sa CLASS_BACKSPACE:
				    EMIT(NFA_CLASS_BACKSPACE);
				    bha_nga;
				pak_sa CLASS_ESCAPE:
				    EMIT(NFA_CLASS_ESCAPE);
				    bha_nga;
				pak_sa CLASS_IDENT:
				    EMIT(NFA_CLASS_IDENT);
				    bha_nga;
				pak_sa CLASS_KEYWORD:
				    EMIT(NFA_CLASS_KEYWORD);
				    bha_nga;
				pak_sa CLASS_FNAME:
				    EMIT(NFA_CLASS_FNAME);
				    bha_nga;
			    }
			    EMIT(NFA_CONCAT);
			    continue;
			}
			// Try equivalence class [=a=] and the like
			ถ้า (equiclass != 0)
			{
			    result = nfa_emit_equi_class(equiclass);
			    ถ้า (result == FAIL)
			    {
				// should never happen
				EMSG_RET_FAIL(_(e_error_building_nfa_with_equivalence_class));
			    }
			    continue;
			}
			// Try collating class like [. .]
			ถ้า (collclass != 0)
			{
			    startc = collclass;	 // allow [.a.]-x as a range
			    // Will emit the proper atom at the end of the
			    // while loop.
			}
		    }
		    // Try a range like 'a-x' or '\t-z'. Also allows '-' as a
		    // start character.
		    ถ้า (*regparse == '-' && oldstartc != -1)
		    {
			emit_range = TRUE;
			startc = oldstartc;
			MB_PTR_ADV(regparse);
			continue;	    // reading the end of the range
		    }

		    // Now handle simple and escaped characters.
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    ถ้า (*regparse == '\\'
			    && !reg_cpo_bsl
			    && regparse + 1 <= endp
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != NULL
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR, regparse[1])
								      != NULL)
			    )
			)
		    {
			MB_PTR_ADV(regparse);

			ถ้า (*regparse == 'n')
			    startc = (reg_string || emit_range
					|| regparse[1] == '-') ? NL : NFA_NEWL;
			มิฉะนั้น ถ้า (*regparse == 'd'
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
				ถ้า (startc == INT_MAX)
				    EMSG_RET_FAIL(_(e_unicode_val_too_large));
				got_coll_char = TRUE;
				MB_PTR_BACK(old_regparse, regparse);
			    }
			    มิฉะนั้น
			    {
				// \r,\t,\e,\b
				startc = backslash_trans(*regparse);
			    }
		    }

		    // Normal printable char
		    ถ้า (startc == -1)
			startc = PTR2CHAR(regparse);

		    // Previous char was '-', so this char is end of range.
		    ถ้า (emit_range)
		    {
			จำนวนเต็ม	endc = startc;
			range_endpoint = true;

			startc = oldstartc;
			ถ้า (startc > endc)
			    EMSG_RET_FAIL(_(e_reverse_range_in_character_class));

			ถ้า (endc > startc + 2)
			{
			    // Emit a range instead of the sequence of
			    // individual characters.
			    ถ้า (startc == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    มิฉะนั้น
				--post_ptr; // remove NFA_CONCAT
			    EMIT(endc);
			    EMIT(NFA_RANGE);
			    EMIT(NFA_CONCAT);
			}
			มิฉะนั้น ถ้า (has_mbyte && ((*mb_char2len)(startc) > 1
				    || (*mb_char2len)(endc) > 1))
			{
			    // Emit the characters in the range.
			    // "startc" was already emitted, so skip it.
			    //
			    สำหรับ (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			มิฉะนั้น
			{
			    // Emit the range. "startc" was already emitted, so
			    // skip it.
			    สำหรับ (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			emit_range = FALSE;
			startc = -1;
		    }
		    มิฉะนั้น
		    {
			// This char (startc) is not part of a range. Just
			// emit it.
			// Normally, simply emit startc. But if we get char
			// code=0 from a collating char, then replace it with
			// 0x0a.
			// This is needed to completely mimic the behaviour of
			// the backtracking engine.
			ถ้า (startc == NFA_NEWL)
			{
			    // Line break can't be matched as part of the
			    // collection, add an OR below. But not for negated
			    // range.
			    ถ้า (!negated)
				extra = NFA_ADD_NL;
			}
			มิฉะนั้น
			{
			    ถ้า (got_coll_char == TRUE && startc == 0)
			    {
				EMIT(0x0a);
				EMIT(NFA_CONCAT);
			    }
			    มิฉะนั้น
			    {
				EMIT(startc);
				ถ้า (!(enc_utf8 && (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse)))))
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
		    ถ้า (!range_endpoint && enc_utf8 &&
			    (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse))))
		    {
			จำนวนเต็ม i = utf_ptr2len(regparse);

			c = utf_ptr2char(regparse + i);

			// Add composing characters
			สำหรับ (;;)
			{
			    ถ้า (c == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    มิฉะนั้น
				EMIT(c);
			    EMIT(NFA_CONCAT);
			    ถ้า ((i += utf_char2len(c)) >= plen)
				bha_nga;
			    c = utf_ptr2char(regparse + i);
			}
			EMIT(NFA_COMPOSING);
			EMIT(NFA_CONCAT);
		    }
		    MB_PTR_ADV(regparse);
		} // while (p < endp)

		MB_PTR_BACK(old_regparse, regparse);
		ถ้า (*regparse == '-')	    // if last, '-' is just a char
		{
		    EMIT('-');
		    EMIT(NFA_CONCAT);
		}

		// skip the trailing ]
		regparse = endp;
		MB_PTR_ADV(regparse);

		// Mark end of the collection.
		ถ้า (negated == TRUE)
		    EMIT(NFA_END_NEG_COLL);
		มิฉะนั้น
		    EMIT(NFA_END_COLL);

		// \_[] also matches \n but it's not negated
		ถ้า (extra == NFA_ADD_NL)
		{
		    EMIT(reg_string ? NL : NFA_NEWL);
		    EMIT(NFA_OR);
		}

		คืน OK;
	    } // if exists closing ]

	    ถ้า (reg_strict)
		EMSG_RET_FAIL(_(e_missing_rsb_after_str_lsb));
	    // FALLTHROUGH

	default:
	    {
		จำนวนเต็ม	plen;

nfa_do_multibyte:
		// plen is length of current char with composing chars
		ถ้า (enc_utf8 && ((*mb_char2len)(c)
			    != (plen = utfc_ptr2len(old_regparse))
						       || utf_iscomposing(c)))
		{
		    จำนวนเต็ม i = 0;

		    // A base character plus composing characters, or just one
		    // or more composing characters.
		    // This requires creating a separate atom as if enclosing
		    // the characters in (), where NFA_COMPOSING is the ( and
		    // NFA_END_COMPOSING is the ). Note that right now we are
		    // building the postfix form, not the NFA itself;
		    // a composing char could be: a, b, c, NFA_COMPOSING
		    // where 'b' and 'c' are chars with codes > 256.
		    สำหรับ (;;)
		    {
			EMIT(c);
			ถ้า (i > 0)
			    EMIT(NFA_CONCAT);
			ถ้า ((i += utf_char2len(c)) >= plen)
			    bha_nga;
			c = utf_ptr2char(old_regparse + i);
		    }
		    EMIT(NFA_COMPOSING);
		    regparse = old_regparse + plen;
		}
		มิฉะนั้น
		{
		    c = no_Magic(c);
		    EMIT(c);
		}
		คืน OK;
	    }
    }

    คืน OK;
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
    static จำนวนเต็ม
nfa_regpiece(void)
{
    จำนวนเต็ม		i;
    จำนวนเต็ม		op;
    จำนวนเต็ม		ret;
    long	minval, maxval;
    จำนวนเต็ม		greedy = TRUE;      // Braces are prefixed with '-' ?
    parse_state_T old_state;
    parse_state_T new_state;
    long	c2;
    จำนวนเต็ม		old_post_pos;
    จำนวนเต็ม		my_post_start;
    จำนวนเต็ม		quest;

    // Save the current parse state, so that we can use it if <atom>{m,n} is
    // next.
    save_parse_state(&old_state);

    // store current pos in the postfix form, for \{m,n} involving 0s
    my_post_start = (จำนวนเต็ม)(post_ptr - post_start);

    ret = nfa_regatom();
    ถ้า (ret == FAIL)
	คืน FAIL;	    // cascaded error

    op = peekchr();
    ถ้า (re_multi_type(op) == NOT_MULTI)
	คืน OK;

    skipchr();
    switch (op)
    {
	pak_sa Magic('*'):
	    EMIT(NFA_STAR);
	    bha_nga;

	pak_sa Magic('+'):
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
	    ถ้า (nfa_regatom() == FAIL)
		คืน FAIL;
	    EMIT(NFA_STAR);
	    EMIT(NFA_CONCAT);
	    skipchr();		// skip the \+
	    bha_nga;

	pak_sa Magic('@'):
	    c2 = (long)getdecchrs();
	    op = no_Magic(getchr());
	    i = 0;
	    switch(op)
	    {
		pak_sa '=':
		    // \@=
		    i = NFA_PREV_ATOM_NO_WIDTH;
		    bha_nga;
		pak_sa '!':
		    // \@!
		    i = NFA_PREV_ATOM_NO_WIDTH_NEG;
		    bha_nga;
		pak_sa '<':
		    op = no_Magic(getchr());
		    ถ้า (op == '=')
			// \@<=
			i = NFA_PREV_ATOM_JUST_BEFORE;
		    มิฉะนั้น ถ้า (op == '!')
			// \@<!
			i = NFA_PREV_ATOM_JUST_BEFORE_NEG;
		    bha_nga;
		pak_sa '>':
		    // \@>
		    i = NFA_PREV_ATOM_LIKE_PATTERN;
		    bha_nga;
	    }
	    ถ้า (i == 0)
	    {
		semsg(_(e_nfa_regexp_unknown_operator_at_chr), op);
		คืน FAIL;
	    }
	    EMIT(i);
	    ถ้า (i == NFA_PREV_ATOM_JUST_BEFORE
					|| i == NFA_PREV_ATOM_JUST_BEFORE_NEG)
		EMIT(c2);
	    bha_nga;

	pak_sa Magic('?'):
	pak_sa Magic('='):
	    EMIT(NFA_QUEST);
	    bha_nga;

	pak_sa Magic('{'):
	    // a{2,5} will expand to 'aaa?a?a?'
	    // a{-1,3} will expand to 'aa??a??', where ?? is the nongreedy
	    // version of '?'
	    // \v(ab){2,3} will expand to '(ab)(ab)(ab)?', where all the
	    // parenthesis have the same id

	    greedy = TRUE;
	    c2 = peekchr();
	    ถ้า (c2 == '-' || c2 == Magic('-'))
	    {
		skipchr();
		greedy = FALSE;
	    }
	    ถ้า (!read_limits(&minval, &maxval))
		EMSG_RET_FAIL(_(e_nfa_regexp_error_reading_repetition_limits));

	    //  <atom>{0,inf}, <atom>{0,} and <atom>{}  are equivalent to
	    //  <atom>*
	    ถ้า (minval == 0 && maxval == MAX_LIMIT)
	    {
		ถ้า (greedy)		// { { (match the braces)
		    // \{}, \{0,}
		    EMIT(NFA_STAR);
		มิฉะนั้น			// { { (match the braces)
		    // \{-}, \{-0,}
		    EMIT(NFA_STAR_NONGREEDY);
		bha_nga;
	    }

	    // Special case: x{0} or x{-0}
	    ถ้า (maxval == 0)
	    {
		// Ignore result of previous call to nfa_regatom()
		post_ptr = post_start + my_post_start;
		// NFA_EMPTY is 0-length and works everywhere
		EMIT(NFA_EMPTY);
		คืน OK;
	    }

	    // The engine is very inefficient (uses too many states) when the
	    // maximum is much larger than the minimum and when the maximum is
	    // large.  However, when maxval is MAX_LIMIT, it is okay, as this
	    // will emit NFA_STAR.
	    // Bail out if we can use the other engine, but only, when the
	    // pattern does not need the NFA engine like (e.g. [[:upper:]]\{2,\}
	    // does not work with characters > 8 bit with the BT engine)
	    ถ้า ((nfa_re_flags & RE_AUTO)
				   && (maxval > 500 || maxval > minval + 200)
				   && (maxval != MAX_LIMIT && minval < 200)
				   && !wants_nfa)
		คืน FAIL;

	    // Ignore previous call to nfa_regatom()
	    post_ptr = post_start + my_post_start;
	    // Save parse state after the repeated atom and the \{}
	    save_parse_state(&new_state);

	    quest = (greedy == TRUE? NFA_QUEST : NFA_QUEST_NONGREEDY);
	    สำหรับ (i = 0; i < maxval; i++)
	    {
		// Goto beginning of the repeated atom
		restore_parse_state(&old_state);
		old_post_pos = (จำนวนเต็ม)(post_ptr - post_start);
		ถ้า (nfa_regatom() == FAIL)
		    คืน FAIL;
		// after "minval" times, atoms are optional
		ถ้า (i + 1 > minval)
		{
		    ถ้า (maxval == MAX_LIMIT)
		    {
			ถ้า (greedy)
			    EMIT(NFA_STAR);
			มิฉะนั้น
			    EMIT(NFA_STAR_NONGREEDY);
		    }
		    มิฉะนั้น
			EMIT(quest);
		}
		ถ้า (old_post_pos != my_post_start)
		    EMIT(NFA_CONCAT);
		ถ้า (i + 1 > minval && maxval == MAX_LIMIT)
		    bha_nga;
	    }

	    // Go to just after the repeated atom and the \{}
	    restore_parse_state(&new_state);
	    curchr = -1;

	    bha_nga;


	default:
	    bha_nga;
    }	// end switch

    ถ้า (re_multi_type(peekchr()) != NOT_MULTI)
	// Can't have a multi follow a multi.
	EMSG_RET_FAIL(_(e_nfa_regexp_cant_have_multi_follow_multi));

    คืน OK;
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
    static จำนวนเต็ม
nfa_regconcat(void)
{
    จำนวนเต็ม		cont = TRUE;
    จำนวนเต็ม		first = TRUE;

    ขณะ (cont)
    {
	switch (peekchr())
	{
	    pak_sa NUL:
	    pak_sa Magic('|'):
	    pak_sa Magic('&'):
	    pak_sa Magic(')'):
		cont = FALSE;
		bha_nga;

	    pak_sa Magic('Z'):
		regflags |= RF_ICOMBINE;
		skipchr_keepstart();
		bha_nga;
	    pak_sa Magic('c'):
		regflags |= RF_ICASE;
		skipchr_keepstart();
		bha_nga;
	    pak_sa Magic('C'):
		regflags |= RF_NOICASE;
		skipchr_keepstart();
		bha_nga;
	    pak_sa Magic('v'):
		reg_magic = MAGIC_ALL;
		skipchr_keepstart();
		curchr = -1;
		bha_nga;
	    pak_sa Magic('m'):
		reg_magic = MAGIC_ON;
		skipchr_keepstart();
		curchr = -1;
		bha_nga;
	    pak_sa Magic('M'):
		reg_magic = MAGIC_OFF;
		skipchr_keepstart();
		curchr = -1;
		bha_nga;
	    pak_sa Magic('V'):
		reg_magic = MAGIC_NONE;
		skipchr_keepstart();
		curchr = -1;
		bha_nga;

	    default:
		ถ้า (nfa_regpiece() == FAIL)
		    คืน FAIL;
		ถ้า (first == FALSE)
		    EMIT(NFA_CONCAT);
		มิฉะนั้น
		    first = FALSE;
		bha_nga;
	}
    }

    คืน OK;
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
    static จำนวนเต็ม
nfa_regbranch(void)
{
    จำนวนเต็ม		old_post_pos;

    old_post_pos = (จำนวนเต็ม)(post_ptr - post_start);

    // First branch, possibly the only one
    ถ้า (nfa_regconcat() == FAIL)
	คืน FAIL;

    // Try next concats
    ขณะ (peekchr() == Magic('&'))
    {
	skipchr();
	// if concat is empty do emit a node
	ถ้า (old_post_pos == (จำนวนเต็ม)(post_ptr - post_start))
	    EMIT(NFA_EMPTY);
	EMIT(NFA_NOPEN);
	EMIT(NFA_PREV_ATOM_NO_WIDTH);
	old_post_pos = (จำนวนเต็ม)(post_ptr - post_start);
	ถ้า (nfa_regconcat() == FAIL)
	    คืน FAIL;
	// if concat is empty do emit a node
	ถ้า (old_post_pos == (จำนวนเต็ม)(post_ptr - post_start))
	    EMIT(NFA_EMPTY);
	EMIT(NFA_CONCAT);
    }

    // if a branch is empty, emit one node for it
    ถ้า (old_post_pos == (จำนวนเต็ม)(post_ptr - post_start))
	EMIT(NFA_EMPTY);

    คืน OK;
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
    static จำนวนเต็ม
nfa_reg(
    จำนวนเต็ม		paren)	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
{
    จำนวนเต็ม		parno = 0;
    จำนวนเต็ม		status = FAIL;

    ถ้า (paren == REG_PAREN)
    {
	ถ้า (regnpar >= NSUBEXP) // Too many `('
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_parens));
	parno = regnpar++;
    }
#ifdef FEAT_SYN_HL
    มิฉะนั้น ถ้า (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	ถ้า (regnzpar >= NSUBEXP)
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_z));
	parno = regnzpar++;
    }
#endif

    ถ้า (nfa_reg_parse_depth >= REG_MAX_PAREN_DEPTH)
	EMSG_RET_FAIL(_(e_command_too_complex));
    ++nfa_reg_parse_depth;

    ถ้า (nfa_regbranch() == FAIL)
	goto theend;	    // cascaded error

    ขณะ (peekchr() == Magic('|'))
    {
	skipchr();
	ถ้า (nfa_regbranch() == FAIL)
	    goto theend;    // cascaded error
	EMIT(NFA_OR);
    }

    // Check for proper termination.
    ถ้า (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
	ถ้า (paren == REG_NPAREN)
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	มิฉะนั้น
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	rc_did_emsg = TRUE;
	goto theend;
    }
    มิฉะนั้น ถ้า (paren == REG_NOPAREN && peekchr() != NUL)
    {
	ถ้า (peekchr() == Magic(')'))
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	มิฉะนั้น
	    emsg(_(e_nfa_regexp_proper_termination_error));
	rc_did_emsg = TRUE;
	goto theend;
    }
    /*
     * Here we set the flag allowing back references to this set of
     * parentheses.
     */
    ถ้า (paren == REG_PAREN)
    {
	had_endbrace[parno] = TRUE;     // have seen the close paren
	EMIT(NFA_MOPEN + parno);
    }
#ifdef FEAT_SYN_HL
    มิฉะนั้น ถ้า (paren == REG_ZPAREN)
	EMIT(NFA_ZOPEN + parno);
#endif

    status = OK;

theend:
    --nfa_reg_parse_depth;
    คืน status;
}

#ifdef DEBUG
static char_u code[50];

    static void
nfa_set_code(จำนวนเต็ม c)
{
    จำนวนเต็ม	    addnl = FALSE;

    ถ้า (c >= NFA_FIRST_NL && c <= NFA_LAST_NL)
    {
	addnl = TRUE;
	c -= NFA_ADD_NL;
    }

    STRCPY(code, "");
    switch (c)
    {
	pak_sa NFA_MATCH:	    STRCPY(code, "NFA_MATCH "); bha_nga;
	pak_sa NFA_SPLIT:	    STRCPY(code, "NFA_SPLIT "); bha_nga;
	pak_sa NFA_CONCAT:    STRCPY(code, "NFA_CONCAT "); bha_nga;
	pak_sa NFA_NEWL:	    STRCPY(code, "NFA_NEWL "); bha_nga;
	pak_sa NFA_ZSTART:    STRCPY(code, "NFA_ZSTART"); bha_nga;
	pak_sa NFA_ZEND:	    STRCPY(code, "NFA_ZEND"); bha_nga;

	pak_sa NFA_BACKREF1:  STRCPY(code, "NFA_BACKREF1"); bha_nga;
	pak_sa NFA_BACKREF2:  STRCPY(code, "NFA_BACKREF2"); bha_nga;
	pak_sa NFA_BACKREF3:  STRCPY(code, "NFA_BACKREF3"); bha_nga;
	pak_sa NFA_BACKREF4:  STRCPY(code, "NFA_BACKREF4"); bha_nga;
	pak_sa NFA_BACKREF5:  STRCPY(code, "NFA_BACKREF5"); bha_nga;
	pak_sa NFA_BACKREF6:  STRCPY(code, "NFA_BACKREF6"); bha_nga;
	pak_sa NFA_BACKREF7:  STRCPY(code, "NFA_BACKREF7"); bha_nga;
	pak_sa NFA_BACKREF8:  STRCPY(code, "NFA_BACKREF8"); bha_nga;
	pak_sa NFA_BACKREF9:  STRCPY(code, "NFA_BACKREF9"); bha_nga;
# ifdef FEAT_SYN_HL
	pak_sa NFA_ZREF1:	    STRCPY(code, "NFA_ZREF1"); bha_nga;
	pak_sa NFA_ZREF2:	    STRCPY(code, "NFA_ZREF2"); bha_nga;
	pak_sa NFA_ZREF3:	    STRCPY(code, "NFA_ZREF3"); bha_nga;
	pak_sa NFA_ZREF4:	    STRCPY(code, "NFA_ZREF4"); bha_nga;
	pak_sa NFA_ZREF5:	    STRCPY(code, "NFA_ZREF5"); bha_nga;
	pak_sa NFA_ZREF6:	    STRCPY(code, "NFA_ZREF6"); bha_nga;
	pak_sa NFA_ZREF7:	    STRCPY(code, "NFA_ZREF7"); bha_nga;
	pak_sa NFA_ZREF8:	    STRCPY(code, "NFA_ZREF8"); bha_nga;
	pak_sa NFA_ZREF9:	    STRCPY(code, "NFA_ZREF9"); bha_nga;
# endif
	pak_sa NFA_SKIP:	    STRCPY(code, "NFA_SKIP"); bha_nga;

	pak_sa NFA_PREV_ATOM_NO_WIDTH:
			    STRCPY(code, "NFA_PREV_ATOM_NO_WIDTH"); bha_nga;
	pak_sa NFA_PREV_ATOM_NO_WIDTH_NEG:
			    STRCPY(code, "NFA_PREV_ATOM_NO_WIDTH_NEG"); bha_nga;
	pak_sa NFA_PREV_ATOM_JUST_BEFORE:
			    STRCPY(code, "NFA_PREV_ATOM_JUST_BEFORE"); bha_nga;
	pak_sa NFA_PREV_ATOM_JUST_BEFORE_NEG:
			 STRCPY(code, "NFA_PREV_ATOM_JUST_BEFORE_NEG"); bha_nga;
	pak_sa NFA_PREV_ATOM_LIKE_PATTERN:
			    STRCPY(code, "NFA_PREV_ATOM_LIKE_PATTERN"); bha_nga;

	pak_sa NFA_NOPEN:		    STRCPY(code, "NFA_NOPEN"); bha_nga;
	pak_sa NFA_NCLOSE:	    STRCPY(code, "NFA_NCLOSE"); bha_nga;
	pak_sa NFA_START_INVISIBLE:   STRCPY(code, "NFA_START_INVISIBLE"); bha_nga;
	pak_sa NFA_START_INVISIBLE_FIRST:
			     STRCPY(code, "NFA_START_INVISIBLE_FIRST"); bha_nga;
	pak_sa NFA_START_INVISIBLE_NEG:
			       STRCPY(code, "NFA_START_INVISIBLE_NEG"); bha_nga;
	pak_sa NFA_START_INVISIBLE_NEG_FIRST:
			 STRCPY(code, "NFA_START_INVISIBLE_NEG_FIRST"); bha_nga;
	pak_sa NFA_START_INVISIBLE_BEFORE:
			    STRCPY(code, "NFA_START_INVISIBLE_BEFORE"); bha_nga;
	pak_sa NFA_START_INVISIBLE_BEFORE_FIRST:
		      STRCPY(code, "NFA_START_INVISIBLE_BEFORE_FIRST"); bha_nga;
	pak_sa NFA_START_INVISIBLE_BEFORE_NEG:
			STRCPY(code, "NFA_START_INVISIBLE_BEFORE_NEG"); bha_nga;
	pak_sa NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
		  STRCPY(code, "NFA_START_INVISIBLE_BEFORE_NEG_FIRST"); bha_nga;
	pak_sa NFA_START_PATTERN:   STRCPY(code, "NFA_START_PATTERN"); bha_nga;
	pak_sa NFA_END_INVISIBLE:	    STRCPY(code, "NFA_END_INVISIBLE"); bha_nga;
	pak_sa NFA_END_INVISIBLE_NEG: STRCPY(code, "NFA_END_INVISIBLE_NEG"); bha_nga;
	pak_sa NFA_END_PATTERN:	    STRCPY(code, "NFA_END_PATTERN"); bha_nga;

	pak_sa NFA_COMPOSING:	    STRCPY(code, "NFA_COMPOSING"); bha_nga;
	pak_sa NFA_END_COMPOSING:	    STRCPY(code, "NFA_END_COMPOSING"); bha_nga;
	pak_sa NFA_OPT_CHARS:	    STRCPY(code, "NFA_OPT_CHARS"); bha_nga;

	pak_sa NFA_MOPEN:
	pak_sa NFA_MOPEN1:
	pak_sa NFA_MOPEN2:
	pak_sa NFA_MOPEN3:
	pak_sa NFA_MOPEN4:
	pak_sa NFA_MOPEN5:
	pak_sa NFA_MOPEN6:
	pak_sa NFA_MOPEN7:
	pak_sa NFA_MOPEN8:
	pak_sa NFA_MOPEN9:
	    STRCPY(code, "NFA_MOPEN(x)");
	    code[10] = c - NFA_MOPEN + '0';
	    bha_nga;
	pak_sa NFA_MCLOSE:
	pak_sa NFA_MCLOSE1:
	pak_sa NFA_MCLOSE2:
	pak_sa NFA_MCLOSE3:
	pak_sa NFA_MCLOSE4:
	pak_sa NFA_MCLOSE5:
	pak_sa NFA_MCLOSE6:
	pak_sa NFA_MCLOSE7:
	pak_sa NFA_MCLOSE8:
	pak_sa NFA_MCLOSE9:
	    STRCPY(code, "NFA_MCLOSE(x)");
	    code[11] = c - NFA_MCLOSE + '0';
	    bha_nga;
# ifdef FEAT_SYN_HL
	pak_sa NFA_ZOPEN:
	pak_sa NFA_ZOPEN1:
	pak_sa NFA_ZOPEN2:
	pak_sa NFA_ZOPEN3:
	pak_sa NFA_ZOPEN4:
	pak_sa NFA_ZOPEN5:
	pak_sa NFA_ZOPEN6:
	pak_sa NFA_ZOPEN7:
	pak_sa NFA_ZOPEN8:
	pak_sa NFA_ZOPEN9:
	    STRCPY(code, "NFA_ZOPEN(x)");
	    code[10] = c - NFA_ZOPEN + '0';
	    bha_nga;
	pak_sa NFA_ZCLOSE:
	pak_sa NFA_ZCLOSE1:
	pak_sa NFA_ZCLOSE2:
	pak_sa NFA_ZCLOSE3:
	pak_sa NFA_ZCLOSE4:
	pak_sa NFA_ZCLOSE5:
	pak_sa NFA_ZCLOSE6:
	pak_sa NFA_ZCLOSE7:
	pak_sa NFA_ZCLOSE8:
	pak_sa NFA_ZCLOSE9:
	    STRCPY(code, "NFA_ZCLOSE(x)");
	    code[11] = c - NFA_ZCLOSE + '0';
	    bha_nga;
# endif
	pak_sa NFA_EOL:		STRCPY(code, "NFA_EOL "); bha_nga;
	pak_sa NFA_BOL:		STRCPY(code, "NFA_BOL "); bha_nga;
	pak_sa NFA_EOW:		STRCPY(code, "NFA_EOW "); bha_nga;
	pak_sa NFA_BOW:		STRCPY(code, "NFA_BOW "); bha_nga;
	pak_sa NFA_EOF:		STRCPY(code, "NFA_EOF "); bha_nga;
	pak_sa NFA_BOF:		STRCPY(code, "NFA_BOF "); bha_nga;
	pak_sa NFA_LNUM:		STRCPY(code, "NFA_LNUM "); bha_nga;
	pak_sa NFA_LNUM_GT:	STRCPY(code, "NFA_LNUM_GT "); bha_nga;
	pak_sa NFA_LNUM_LT:	STRCPY(code, "NFA_LNUM_LT "); bha_nga;
	pak_sa NFA_COL:		STRCPY(code, "NFA_COL "); bha_nga;
	pak_sa NFA_COL_GT:	STRCPY(code, "NFA_COL_GT "); bha_nga;
	pak_sa NFA_COL_LT:	STRCPY(code, "NFA_COL_LT "); bha_nga;
	pak_sa NFA_VCOL:		STRCPY(code, "NFA_VCOL "); bha_nga;
	pak_sa NFA_VCOL_GT:	STRCPY(code, "NFA_VCOL_GT "); bha_nga;
	pak_sa NFA_VCOL_LT:	STRCPY(code, "NFA_VCOL_LT "); bha_nga;
	pak_sa NFA_MARK:		STRCPY(code, "NFA_MARK "); bha_nga;
	pak_sa NFA_MARK_GT:	STRCPY(code, "NFA_MARK_GT "); bha_nga;
	pak_sa NFA_MARK_LT:	STRCPY(code, "NFA_MARK_LT "); bha_nga;
	pak_sa NFA_CURSOR:	STRCPY(code, "NFA_CURSOR "); bha_nga;
	pak_sa NFA_VISUAL:	STRCPY(code, "NFA_VISUAL "); bha_nga;
	pak_sa NFA_ANY_COMPOSING:	STRCPY(code, "NFA_ANY_COMPOSING "); bha_nga;

	pak_sa NFA_STAR:		STRCPY(code, "NFA_STAR "); bha_nga;
	pak_sa NFA_STAR_NONGREEDY: STRCPY(code, "NFA_STAR_NONGREEDY "); bha_nga;
	pak_sa NFA_QUEST:		STRCPY(code, "NFA_QUEST"); bha_nga;
	pak_sa NFA_QUEST_NONGREEDY: STRCPY(code, "NFA_QUEST_NON_GREEDY"); bha_nga;
	pak_sa NFA_EMPTY:		STRCPY(code, "NFA_EMPTY"); bha_nga;
	pak_sa NFA_OR:		STRCPY(code, "NFA_OR"); bha_nga;

	pak_sa NFA_START_COLL:	STRCPY(code, "NFA_START_COLL"); bha_nga;
	pak_sa NFA_END_COLL:	STRCPY(code, "NFA_END_COLL"); bha_nga;
	pak_sa NFA_START_NEG_COLL: STRCPY(code, "NFA_START_NEG_COLL"); bha_nga;
	pak_sa NFA_END_NEG_COLL:	STRCPY(code, "NFA_END_NEG_COLL"); bha_nga;
	pak_sa NFA_RANGE:		STRCPY(code, "NFA_RANGE"); bha_nga;
	pak_sa NFA_RANGE_MIN:	STRCPY(code, "NFA_RANGE_MIN"); bha_nga;
	pak_sa NFA_RANGE_MAX:	STRCPY(code, "NFA_RANGE_MAX"); bha_nga;

	pak_sa NFA_CLASS_ALNUM:	STRCPY(code, "NFA_CLASS_ALNUM"); bha_nga;
	pak_sa NFA_CLASS_ALPHA:	STRCPY(code, "NFA_CLASS_ALPHA"); bha_nga;
	pak_sa NFA_CLASS_BLANK:	STRCPY(code, "NFA_CLASS_BLANK"); bha_nga;
	pak_sa NFA_CLASS_CNTRL:	STRCPY(code, "NFA_CLASS_CNTRL"); bha_nga;
	pak_sa NFA_CLASS_DIGIT:	STRCPY(code, "NFA_CLASS_DIGIT"); bha_nga;
	pak_sa NFA_CLASS_GRAPH:	STRCPY(code, "NFA_CLASS_GRAPH"); bha_nga;
	pak_sa NFA_CLASS_LOWER:	STRCPY(code, "NFA_CLASS_LOWER"); bha_nga;
	pak_sa NFA_CLASS_PRINT:	STRCPY(code, "NFA_CLASS_PRINT"); bha_nga;
	pak_sa NFA_CLASS_PUNCT:	STRCPY(code, "NFA_CLASS_PUNCT"); bha_nga;
	pak_sa NFA_CLASS_SPACE:	STRCPY(code, "NFA_CLASS_SPACE"); bha_nga;
	pak_sa NFA_CLASS_UPPER:	STRCPY(code, "NFA_CLASS_UPPER"); bha_nga;
	pak_sa NFA_CLASS_XDIGIT:	STRCPY(code, "NFA_CLASS_XDIGIT"); bha_nga;
	pak_sa NFA_CLASS_TAB:	STRCPY(code, "NFA_CLASS_TAB"); bha_nga;
	pak_sa NFA_CLASS_RETURN:	STRCPY(code, "NFA_CLASS_RETURN"); bha_nga;
	pak_sa NFA_CLASS_BACKSPACE:   STRCPY(code, "NFA_CLASS_BACKSPACE"); bha_nga;
	pak_sa NFA_CLASS_ESCAPE:	STRCPY(code, "NFA_CLASS_ESCAPE"); bha_nga;
	pak_sa NFA_CLASS_IDENT:	STRCPY(code, "NFA_CLASS_IDENT"); bha_nga;
	pak_sa NFA_CLASS_KEYWORD:	STRCPY(code, "NFA_CLASS_KEYWORD"); bha_nga;
	pak_sa NFA_CLASS_FNAME:	STRCPY(code, "NFA_CLASS_FNAME"); bha_nga;

	pak_sa NFA_ANY:	STRCPY(code, "NFA_ANY"); bha_nga;
	pak_sa NFA_IDENT:	STRCPY(code, "NFA_IDENT"); bha_nga;
	pak_sa NFA_SIDENT:STRCPY(code, "NFA_SIDENT"); bha_nga;
	pak_sa NFA_KWORD:	STRCPY(code, "NFA_KWORD"); bha_nga;
	pak_sa NFA_SKWORD:STRCPY(code, "NFA_SKWORD"); bha_nga;
	pak_sa NFA_FNAME:	STRCPY(code, "NFA_FNAME"); bha_nga;
	pak_sa NFA_SFNAME:STRCPY(code, "NFA_SFNAME"); bha_nga;
	pak_sa NFA_PRINT:	STRCPY(code, "NFA_PRINT"); bha_nga;
	pak_sa NFA_SPRINT:STRCPY(code, "NFA_SPRINT"); bha_nga;
	pak_sa NFA_WHITE:	STRCPY(code, "NFA_WHITE"); bha_nga;
	pak_sa NFA_NWHITE:STRCPY(code, "NFA_NWHITE"); bha_nga;
	pak_sa NFA_DIGIT:	STRCPY(code, "NFA_DIGIT"); bha_nga;
	pak_sa NFA_NDIGIT:STRCPY(code, "NFA_NDIGIT"); bha_nga;
	pak_sa NFA_HEX:	STRCPY(code, "NFA_HEX"); bha_nga;
	pak_sa NFA_NHEX:	STRCPY(code, "NFA_NHEX"); bha_nga;
	pak_sa NFA_OCTAL:	STRCPY(code, "NFA_OCTAL"); bha_nga;
	pak_sa NFA_NOCTAL:STRCPY(code, "NFA_NOCTAL"); bha_nga;
	pak_sa NFA_WORD:	STRCPY(code, "NFA_WORD"); bha_nga;
	pak_sa NFA_NWORD:	STRCPY(code, "NFA_NWORD"); bha_nga;
	pak_sa NFA_HEAD:	STRCPY(code, "NFA_HEAD"); bha_nga;
	pak_sa NFA_NHEAD:	STRCPY(code, "NFA_NHEAD"); bha_nga;
	pak_sa NFA_ALPHA:	STRCPY(code, "NFA_ALPHA"); bha_nga;
	pak_sa NFA_NALPHA:STRCPY(code, "NFA_NALPHA"); bha_nga;
	pak_sa NFA_LOWER:	STRCPY(code, "NFA_LOWER"); bha_nga;
	pak_sa NFA_NLOWER:STRCPY(code, "NFA_NLOWER"); bha_nga;
	pak_sa NFA_UPPER:	STRCPY(code, "NFA_UPPER"); bha_nga;
	pak_sa NFA_NUPPER:STRCPY(code, "NFA_NUPPER"); bha_nga;
	pak_sa NFA_LOWER_IC:  STRCPY(code, "NFA_LOWER_IC"); bha_nga;
	pak_sa NFA_NLOWER_IC: STRCPY(code, "NFA_NLOWER_IC"); bha_nga;
	pak_sa NFA_UPPER_IC:  STRCPY(code, "NFA_UPPER_IC"); bha_nga;
	pak_sa NFA_NUPPER_IC: STRCPY(code, "NFA_NUPPER_IC"); bha_nga;

	default:
	    STRCPY(code, "CHAR(x)");
	    code[5] = c;
    }

    ถ้า (addnl == TRUE)
	STRCAT(code, " + NEWLINE ");

}

# ifdef ENABLE_LOG
static FILE *log_fd;
static char_u e_log_open_failed[] = N_("Could not open temporary log file for writing, displaying on stderr... ");

/*
 * Print the postfix notation of the current regexp.
 */
    static void
nfa_postfix_dump(char_u *expr, จำนวนเต็ม retval)
{
    จำนวนเต็ม *p;
    FILE *f;

    f = fopen(NFA_REGEXP_DUMP_LOG, "a");
    ถ้า (f == NULL)
	คืน;

    fprintf(f, "\n-------------------------\n");
    ถ้า (retval == FAIL)
	fprintf(f, ">>> NFA engine failed... \n");
    มิฉะนั้น ถ้า (retval == OK)
	fprintf(f, ">>> NFA engine succeeded !\n");
    fprintf(f, "Regexp: \"%s\"\nPostfix notation (char): \"", expr);
    สำหรับ (p = post_start; *p && p < post_ptr; p++)
    {
	nfa_set_code(*p);
	fprintf(f, "%s, ", code);
    }
    fprintf(f, "\"\nPostfix notation (int): ");
    สำหรับ (p = post_start; *p && p < post_ptr; p++)
	fprintf(f, "%d ", *p);
    fprintf(f, "\n\n");
    fclose(f);
}

/*
 * Print the NFA starting with a root node "state".
 */
    static void
nfa_print_state(FILE *debugf, nfa_state_T *state)
{
    garray_T indent;

    ga_init2(&indent, 1, 64);
    ga_append(&indent, '\0');
    nfa_print_state2(debugf, state, &indent);
    ga_clear(&indent);
}

    static void
nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent)
{
    char_u  *p;

    ถ้า (state == NULL)
	คืน;

    fprintf(debugf, "(%2d)", abs(state->id));

    // Output indent
    p = (char_u *)indent->ga_data;
    ถ้า (indent->ga_len >= 3)
    {
	จำนวนเต็ม	last = indent->ga_len - 3;
	char_u	save[2];

	STRNCPY(save, &p[last], 2);
	memcpy(&p[last], "+-", 2);
	fprintf(debugf, " %s", p);
	STRNCPY(&p[last], save, 2);
    }
    มิฉะนั้น
	fprintf(debugf, " %s", p);

    nfa_set_code(state->c);
    fprintf(debugf, "%s (%d) (id=%d) val=%d\n",
		 code,
		 state->c,
		 abs(state->id),
		 state->val);
    ถ้า (state->id < 0)
	คืน;

    state->id = abs(state->id) * -1;

    // grow indent for state->out
    indent->ga_len -= 1;
    ถ้า (state->out1)
	GA_CONCAT_LITERAL(indent, "| ");
    มิฉะนั้น
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
    static void
nfa_dump(nfa_regprog_T *prog)
{
    FILE *debugf = fopen(NFA_REGEXP_DUMP_LOG, "a");

    ถ้า (debugf == NULL)
	คืน;

    nfa_print_state(debugf, prog->start);

    ถ้า (prog->reganch)
	fprintf(debugf, "reganch: %d\n", prog->reganch);
    ถ้า (prog->regstart != NUL)
	fprintf(debugf, "regstart: %c (decimal: %d)\n",
		prog->regstart, prog->regstart);
    ถ้า (prog->match_text != NULL)
	fprintf(debugf, "match_text: \"%s\"\n", prog->match_text);

    fclose(debugf);
}
# endif	    // ENABLE_LOG
#endif	    // DEBUG

/*
 * Parse r.e. @expr and convert it into postfix form.
 * Return the postfix string on success, NULL otherwise.
 */
    static จำนวนเต็ม *
re2post(void)
{
    ถ้า (nfa_reg(REG_NOPAREN) == FAIL)
	คืน NULL;
    EMIT(NFA_MOPEN);
    คืน post_start;
}

// NB. Some of the code below is inspired by Russ's.

/*
 * Represents an NFA state plus zero or one or two arrows exiting.
 * if c == MATCH, no arrows out; matching state.
 * If c == SPLIT, unlabeled arrows to out and out1 (if != NULL).
 * If c < 256, labeled arrow with character c to out.
 */

static nfa_state_T	*state_ptr; // points to nfa_prog->state

/*
 * Allocate and initialize nfa_state_T.
 */
    static nfa_state_T *
alloc_state(จำนวนเต็ม c, nfa_state_T *out, nfa_state_T *out1)
{
    nfa_state_T *s;

    ถ้า (istate >= nstate)
	คืน NULL;

    s = &state_ptr[istate++];

    s->c    = c;
    s->out  = out;
    s->out1 = out1;
    s->val  = 0;

    s->id   = istate;
    s->lastlist[0] = 0;
    s->lastlist[1] = 0;

    คืน s;
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
typedef sa_ngha Ptrlist Ptrlist;
sa_ngha Ptrlist
{
    Ptrlist	*next;
    nfa_state_T	*s;
};

sa_nracanaa Frag
{
    nfa_state_T *start;
    Ptrlist	*out;
};
typedef sa_nracanaa Frag Frag_T;

// Reused across compilations by post2nfa(); freed in free_regexp_stuff().
static Frag_T *nfa_stack;
static จำนวนเต็ม nfa_stack_len;

/*
 * Initialize a Frag_T struct and return it.
 */
    static Frag_T
frag(nfa_state_T *start, Ptrlist *out)
{
    Frag_T n;

    n.start = start;
    n.out = out;
    คืน n;
}

/*
 * Create singleton list containing just outp.
 */
    static Ptrlist *
list1(
    nfa_state_T	**outp)
{
    Ptrlist *l;

    l = (Ptrlist *)outp;
    l->next = NULL;
    คืน l;
}

/*
 * Patch the list of states at out to point to start.
 */
    static void
patch(Ptrlist *l, nfa_state_T *s)
{
    Ptrlist *next;

    สำหรับ (; l; l = next)
    {
	next = l->next;
	l->s = s;
    }
}


/*
 * Join the two lists l1 and l2, returning the combination.
 */
    static Ptrlist *
append(Ptrlist *l1, Ptrlist *l2)
{
    Ptrlist *oldl1;

    oldl1 = l1;
    ขณะ (l1->next)
	l1 = l1->next;
    l1->next = l2;
    คืน oldl1;
}

/*
 * Stack used for transforming postfix form into NFA.
 */
static Frag_T empty;

    static void
st_error(จำนวนเต็ม *postfix UNUSED, จำนวนเต็ม *end UNUSED, จำนวนเต็ม *p UNUSED)
{
#ifdef NFA_REGEXP_ERROR_LOG
    FILE *df;
    จำนวนเต็ม *p2;

    df = fopen(NFA_REGEXP_ERROR_LOG, "a");
    ถ้า (df)
    {
	fprintf(df, "Error popping the stack!\n");
# ifdef DEBUG
	fprintf(df, "Current regexp is \"%s\"\n", nfa_regengine.expr);
# endif
	fprintf(df, "Postfix form is: ");
# ifdef DEBUG
	สำหรับ (p2 = postfix; p2 < end; p2++)
	{
	    nfa_set_code(*p2);
	    fprintf(df, "%s, ", code);
	}
	nfa_set_code(*p);
	fprintf(df, "\nCurrent position is: ");
	สำหรับ (p2 = postfix; p2 <= p; p2 ++)
	{
	    nfa_set_code(*p2);
	    fprintf(df, "%s, ", code);
	}
# มิฉะนั้น
	สำหรับ (p2 = postfix; p2 < end; p2++)
	    fprintf(df, "%d, ", *p2);
	fprintf(df, "\nCurrent position is: ");
	สำหรับ (p2 = postfix; p2 <= p; p2 ++)
	    fprintf(df, "%d, ", *p2);
# endif
	fprintf(df, "\n--------------------------\n");
	fclose(df);
    }
#endif
    emsg(_(e_nfa_regexp_could_not_pop_stack));
}

/*
 * Push an item onto the stack.
 */
    static void
st_push(Frag_T s, Frag_T **p, Frag_T *stack_end)
{
    Frag_T *stackp = *p;

    ถ้า (stackp >= stack_end)
	คืน;
    *stackp = s;
    *p = *p + 1;
}

/*
 * Pop an item from the stack.
 */
    static Frag_T
st_pop(Frag_T **p, Frag_T *stack)
{
    Frag_T *stackp;

    *p = *p - 1;
    stackp = *p;
    ถ้า (stackp < stack)
	คืน empty;
    คืน **p;
}

/*
 * Estimate the maximum byte length of anything matching "state".
 * When unknown or unlimited return -1.
 */
    static จำนวนเต็ม
nfa_max_width(nfa_state_T *startstate, จำนวนเต็ม depth)
{
    จำนวนเต็ม		    l, r;
    nfa_state_T	    *state = startstate;
    จำนวนเต็ม		    len = 0;

    // detect looping in a NFA_SPLIT
    ถ้า (depth > 4)
	คืน -1;

    ขณะ (state != NULL)
    {
	switch (state->c)
	{
	    pak_sa NFA_END_INVISIBLE:
	    pak_sa NFA_END_INVISIBLE_NEG:
		// the end, return what we have
		คืน len;

	    pak_sa NFA_SPLIT:
		// two alternatives, use the maximum
		l = nfa_max_width(state->out, depth + 1);
		r = nfa_max_width(state->out1, depth + 1);
		ถ้า (l < 0 || r < 0)
		    คืน -1;
		คืน len + (l > r ? l : r);

	    pak_sa NFA_ANY:
	    pak_sa NFA_START_COLL:
	    pak_sa NFA_START_NEG_COLL:
		// matches some character, including composing chars
		ถ้า (enc_utf8)
		    len += MB_MAXBYTES;
		มิฉะนั้น ถ้า (has_mbyte)
		    len += 2;
		มิฉะนั้น
		    ++len;
		ถ้า (state->c != NFA_ANY)
		{
		    // Skip over the compiled collection.
		    // malformed NFAs must not crash width estimation.
		    ถ้า (state->out1 == NULL || state->out1->out == NULL)
			คืน -1;
		    state = state->out1->out;
		    continue;
		}
		bha_nga;

	    pak_sa NFA_DIGIT:
	    pak_sa NFA_WHITE:
	    pak_sa NFA_HEX:
	    pak_sa NFA_OCTAL:
		// ascii
		++len;
		bha_nga;

	    pak_sa NFA_IDENT:
	    pak_sa NFA_SIDENT:
	    pak_sa NFA_KWORD:
	    pak_sa NFA_SKWORD:
	    pak_sa NFA_FNAME:
	    pak_sa NFA_SFNAME:
	    pak_sa NFA_PRINT:
	    pak_sa NFA_SPRINT:
	    pak_sa NFA_NWHITE:
	    pak_sa NFA_NDIGIT:
	    pak_sa NFA_NHEX:
	    pak_sa NFA_NOCTAL:
	    pak_sa NFA_WORD:
	    pak_sa NFA_NWORD:
	    pak_sa NFA_HEAD:
	    pak_sa NFA_NHEAD:
	    pak_sa NFA_ALPHA:
	    pak_sa NFA_NALPHA:
	    pak_sa NFA_LOWER:
	    pak_sa NFA_NLOWER:
	    pak_sa NFA_UPPER:
	    pak_sa NFA_NUPPER:
	    pak_sa NFA_LOWER_IC:
	    pak_sa NFA_NLOWER_IC:
	    pak_sa NFA_UPPER_IC:
	    pak_sa NFA_NUPPER_IC:
	    pak_sa NFA_ANY_COMPOSING:
		// possibly non-ascii
		ถ้า (has_mbyte)
		    len += 3;
		มิฉะนั้น
		    ++len;
		bha_nga;

	    pak_sa NFA_START_INVISIBLE:
	    pak_sa NFA_START_INVISIBLE_NEG:
	    pak_sa NFA_START_INVISIBLE_BEFORE:
	    pak_sa NFA_START_INVISIBLE_BEFORE_NEG:
		// zero-width, out1 points to the END state
		state = state->out1->out;
		continue;

	    pak_sa NFA_BACKREF1:
	    pak_sa NFA_BACKREF2:
	    pak_sa NFA_BACKREF3:
	    pak_sa NFA_BACKREF4:
	    pak_sa NFA_BACKREF5:
	    pak_sa NFA_BACKREF6:
	    pak_sa NFA_BACKREF7:
	    pak_sa NFA_BACKREF8:
	    pak_sa NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	    pak_sa NFA_ZREF1:
	    pak_sa NFA_ZREF2:
	    pak_sa NFA_ZREF3:
	    pak_sa NFA_ZREF4:
	    pak_sa NFA_ZREF5:
	    pak_sa NFA_ZREF6:
	    pak_sa NFA_ZREF7:
	    pak_sa NFA_ZREF8:
	    pak_sa NFA_ZREF9:
#endif
	    pak_sa NFA_NEWL:
	    pak_sa NFA_SKIP:
		// unknown width
		คืน -1;

	    pak_sa NFA_BOL:
	    pak_sa NFA_EOL:
	    pak_sa NFA_BOF:
	    pak_sa NFA_EOF:
	    pak_sa NFA_BOW:
	    pak_sa NFA_EOW:
	    pak_sa NFA_MOPEN:
	    pak_sa NFA_MOPEN1:
	    pak_sa NFA_MOPEN2:
	    pak_sa NFA_MOPEN3:
	    pak_sa NFA_MOPEN4:
	    pak_sa NFA_MOPEN5:
	    pak_sa NFA_MOPEN6:
	    pak_sa NFA_MOPEN7:
	    pak_sa NFA_MOPEN8:
	    pak_sa NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	    pak_sa NFA_ZOPEN:
	    pak_sa NFA_ZOPEN1:
	    pak_sa NFA_ZOPEN2:
	    pak_sa NFA_ZOPEN3:
	    pak_sa NFA_ZOPEN4:
	    pak_sa NFA_ZOPEN5:
	    pak_sa NFA_ZOPEN6:
	    pak_sa NFA_ZOPEN7:
	    pak_sa NFA_ZOPEN8:
	    pak_sa NFA_ZOPEN9:
	    pak_sa NFA_ZCLOSE:
	    pak_sa NFA_ZCLOSE1:
	    pak_sa NFA_ZCLOSE2:
	    pak_sa NFA_ZCLOSE3:
	    pak_sa NFA_ZCLOSE4:
	    pak_sa NFA_ZCLOSE5:
	    pak_sa NFA_ZCLOSE6:
	    pak_sa NFA_ZCLOSE7:
	    pak_sa NFA_ZCLOSE8:
	    pak_sa NFA_ZCLOSE9:
#endif
	    pak_sa NFA_MCLOSE:
	    pak_sa NFA_MCLOSE1:
	    pak_sa NFA_MCLOSE2:
	    pak_sa NFA_MCLOSE3:
	    pak_sa NFA_MCLOSE4:
	    pak_sa NFA_MCLOSE5:
	    pak_sa NFA_MCLOSE6:
	    pak_sa NFA_MCLOSE7:
	    pak_sa NFA_MCLOSE8:
	    pak_sa NFA_MCLOSE9:
	    pak_sa NFA_NOPEN:
	    pak_sa NFA_NCLOSE:

	    pak_sa NFA_LNUM_GT:
	    pak_sa NFA_LNUM_LT:
	    pak_sa NFA_COL_GT:
	    pak_sa NFA_COL_LT:
	    pak_sa NFA_VCOL_GT:
	    pak_sa NFA_VCOL_LT:
	    pak_sa NFA_MARK_GT:
	    pak_sa NFA_MARK_LT:
	    pak_sa NFA_VISUAL:
	    pak_sa NFA_LNUM:
	    pak_sa NFA_CURSOR:
	    pak_sa NFA_COL:
	    pak_sa NFA_VCOL:
	    pak_sa NFA_MARK:

	    pak_sa NFA_ZSTART:
	    pak_sa NFA_ZEND:
	    pak_sa NFA_OPT_CHARS:
	    pak_sa NFA_EMPTY:
	    pak_sa NFA_START_PATTERN:
	    pak_sa NFA_END_PATTERN:
	    pak_sa NFA_COMPOSING:
	    pak_sa NFA_END_COMPOSING:
		// zero-width
		bha_nga;

	    default:
		ถ้า (state->c < 0)
		    // don't know what this is
		    คืน -1;
		// normal character
		len += MB_CHAR2LEN(state->c);
		bha_nga;
	}

	// normal way to continue
	state = state->out;
    }

    // unrecognized, "cannot happen"
    คืน -1;
}

/*
 * Convert a postfix form into its equivalent NFA.
 * Return the NFA start state on success, NULL otherwise.
 */
    static nfa_state_T *
post2nfa(จำนวนเต็ม *postfix, จำนวนเต็ม *end, จำนวนเต็ม nfa_calc_size)
{
    จำนวนเต็ม		*p;
    จำนวนเต็ม		mopen;
    จำนวนเต็ม		mclose;
    Frag_T	*stack = NULL;
    Frag_T	*stackp = NULL;
    Frag_T	*stack_end = NULL;
    Frag_T	e1;
    Frag_T	e2;
    Frag_T	e;
    nfa_state_T	*s;
    nfa_state_T	*s1;
    nfa_state_T	*matchstate;
    nfa_state_T	*ret = NULL;

    ถ้า (postfix == NULL)
	คืน NULL;

#define PUSH(s)	    st_push((s), &stackp, stack_end)
#define POP()	    st_pop(&stackp, stack);		\
		    ถ้า (stackp < stack)			\
		    {					\
			st_error(postfix, end, p);	\
			คืน NULL;			\
		    }

    ถ้า (nfa_calc_size == FALSE)
    {
	// Reuse the fragment stack across compilations, growing when needed;
	// it is freed in free_regexp_stuff().
	ถ้า (nfa_stack == NULL || nfa_stack_len < nstate + 1)
	{
	    vim_free(nfa_stack);
	    nfa_stack = ALLOC_MULT(Frag_T, nstate + 1);
	    ถ้า (nfa_stack == NULL)
	    {
		nfa_stack_len = 0;
		คืน NULL;
	    }
	    nfa_stack_len = nstate + 1;
	}
	stack = nfa_stack;
	stackp = stack;
	stack_end = stack + (nstate + 1);
    }

    สำหรับ (p = postfix; p < end; ++p)
    {
	switch (*p)
	{
	pak_sa NFA_CONCAT:
	    // Concatenation.
	    // Pay attention: this operator does not exist in the r.e. itself
	    // (it is implicit, really).  It is added when r.e. is translated
	    // to postfix form in re2post().
	    ถ้า (nfa_calc_size == TRUE)
	    {
		// nstate += 0;
		bha_nga;
	    }
	    e2 = POP();
	    e1 = POP();
	    patch(e1.out, e2.start);
	    PUSH(frag(e1.start, e2.out));
	    bha_nga;

	pak_sa NFA_OR:
	    // Alternation
	    ถ้า (nfa_calc_size == TRUE)
	    {
		nstate++;
		bha_nga;
	    }
	    e2 = POP();
	    e1 = POP();
	    s = alloc_state(NFA_SPLIT, e1.start, e2.start);
	    ถ้า (s == NULL)
		goto theend;
	    PUSH(frag(s, append(e1.out, e2.out)));
	    bha_nga;

	pak_sa NFA_STAR:
	    // Zero or more, prefer more
	    ถ้า (nfa_calc_size == TRUE)
	    {
		nstate++;
		bha_nga;
	    }
	    e = POP();
	    s = alloc_state(NFA_SPLIT, e.start, NULL);
	    ถ้า (s == NULL)
		goto theend;
	    patch(e.out, s);
	    PUSH(frag(s, list1(&s->out1)));
	    bha_nga;

	pak_sa NFA_STAR_NONGREEDY:
	    // Zero or more, prefer zero
	    ถ้า (nfa_calc_size == TRUE)
	    {
		nstate++;
		bha_nga;
	    }
	    e = POP();
	    s = alloc_state(NFA_SPLIT, NULL, e.start);
	    ถ้า (s == NULL)
		goto theend;
	    patch(e.out, s);
	    PUSH(frag(s, list1(&s->out)));
	    bha_nga;

	pak_sa NFA_QUEST:
	    // one or zero atoms=> greedy match
	    ถ้า (nfa_calc_size == TRUE)
	    {
		nstate++;
		bha_nga;
	    }
	    e = POP();
	    s = alloc_state(NFA_SPLIT, e.start, NULL);
	    ถ้า (s == NULL)
		goto theend;
	    PUSH(frag(s, append(e.out, list1(&s->out1))));
	    bha_nga;

	pak_sa NFA_QUEST_NONGREEDY:
	    // zero or one atoms => non-greedy match
	    ถ้า (nfa_calc_size == TRUE)
	    {
		nstate++;
		bha_nga;
	    }
	    e = POP();
	    s = alloc_state(NFA_SPLIT, NULL, e.start);
	    ถ้า (s == NULL)
		goto theend;
	    PUSH(frag(s, append(e.out, list1(&s->out))));
	    bha_nga;

	pak_sa NFA_END_COLL:
	pak_sa NFA_END_NEG_COLL:
	    // On the stack is the sequence starting with NFA_START_COLL or
	    // NFA_START_NEG_COLL and all possible characters. Patch it to
	    // add the output to the start.
	    ถ้า (nfa_calc_size == TRUE)
	    {
		nstate++;
		bha_nga;
	    }
	    e = POP();
	    s = alloc_state(NFA_END_COLL, NULL, NULL);
	    ถ้า (s == NULL)
		goto theend;
	    patch(e.out, s);
	    e.start->out1 = s;
	    PUSH(frag(e.start, list1(&s->out)));
	    bha_nga;

	pak_sa NFA_RANGE:
	    // Before this are two characters, the low and high end of a
	    // range.  Turn them into two states with MIN and MAX.
	    ถ้า (nfa_calc_size == TRUE)
	    {
		// nstate += 0;
		bha_nga;
	    }
	    e2 = POP();
	    e1 = POP();
	    e2.start->val = e2.start->c;
	    e2.start->c = NFA_RANGE_MAX;
	    e1.start->val = e1.start->c;
	    e1.start->c = NFA_RANGE_MIN;
	    patch(e1.out, e2.start);
	    PUSH(frag(e1.start, e2.out));
	    bha_nga;

	pak_sa NFA_EMPTY:
	    // 0-length, used in a repetition with max/min count of 0
	    ถ้า (nfa_calc_size == TRUE)
	    {
		nstate++;
		bha_nga;
	    }
	    s = alloc_state(NFA_EMPTY, NULL, NULL);
	    ถ้า (s == NULL)
		goto theend;
	    PUSH(frag(s, list1(&s->out)));
	    bha_nga;

	pak_sa NFA_OPT_CHARS:
	  {
	    จำนวนเต็ม    n;

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
	    ถ้า (nfa_calc_size == TRUE)
	    {
		nstate += n;
		bha_nga;
	    }
	    s = NULL; // avoid compiler warning
	    e1.out = NULL; // stores list with out1's
	    s1 = NULL; // previous NFA_SPLIT to connect to
	    ขณะ (n-- > 0)
	    {
		e = POP(); // get character
		s = alloc_state(NFA_SPLIT, e.start, NULL);
		ถ้า (s == NULL)
		    goto theend;
		ถ้า (e1.out == NULL)
		    e1 = e;
		patch(e.out, s1);
		append(e1.out, list1(&s->out1));
		s1 = s;
	    }
	    PUSH(frag(s, e1.out));
	    bha_nga;
	  }

	pak_sa NFA_PREV_ATOM_NO_WIDTH:
	pak_sa NFA_PREV_ATOM_NO_WIDTH_NEG:
	pak_sa NFA_PREV_ATOM_JUST_BEFORE:
	pak_sa NFA_PREV_ATOM_JUST_BEFORE_NEG:
	pak_sa NFA_PREV_ATOM_LIKE_PATTERN:
	  {
	    จำนวนเต็ม before = (*p == NFA_PREV_ATOM_JUST_BEFORE
				      || *p == NFA_PREV_ATOM_JUST_BEFORE_NEG);
	    จำนวนเต็ม pattern = (*p == NFA_PREV_ATOM_LIKE_PATTERN);
	    จำนวนเต็ม start_state;
	    จำนวนเต็ม end_state;
	    จำนวนเต็ม n = 0;
	    nfa_state_T *zend;
	    nfa_state_T *skip;

	    switch (*p)
	    {
		pak_sa NFA_PREV_ATOM_NO_WIDTH:
		    start_state = NFA_START_INVISIBLE;
		    end_state = NFA_END_INVISIBLE;
		    bha_nga;
		pak_sa NFA_PREV_ATOM_NO_WIDTH_NEG:
		    start_state = NFA_START_INVISIBLE_NEG;
		    end_state = NFA_END_INVISIBLE_NEG;
		    bha_nga;
		pak_sa NFA_PREV_ATOM_JUST_BEFORE:
		    start_state = NFA_START_INVISIBLE_BEFORE;
		    end_state = NFA_END_INVISIBLE;
		    bha_nga;
		pak_sa NFA_PREV_ATOM_JUST_BEFORE_NEG:
		    start_state = NFA_START_INVISIBLE_BEFORE_NEG;
		    end_state = NFA_END_INVISIBLE_NEG;
		    bha_nga;
		default: // NFA_PREV_ATOM_LIKE_PATTERN:
		    start_state = NFA_START_PATTERN;
		    end_state = NFA_END_PATTERN;
		    bha_nga;
	    }

	    ถ้า (before)
		n = *++p; // get the count

	    // The \@= operator: match the preceding atom with zero width.
	    // The \@! operator: no match for the preceding atom.
	    // The \@<= operator: match for the preceding atom.
	    // The \@<! operator: no match for the preceding atom.
	    // Surrounds the preceding atom with START_INVISIBLE and
	    // END_INVISIBLE, similarly to MOPEN.

	    ถ้า (nfa_calc_size == TRUE)
	    {
		nstate += pattern ? 4 : 2;
		bha_nga;
	    }
	    e = POP();
	    s1 = alloc_state(end_state, NULL, NULL);
	    ถ้า (s1 == NULL)
		goto theend;

	    s = alloc_state(start_state, e.start, s1);
	    ถ้า (s == NULL)
		goto theend;
	    ถ้า (pattern)
	    {
		// NFA_ZEND -> NFA_END_PATTERN -> NFA_SKIP -> what follows.
		skip = alloc_state(NFA_SKIP, NULL, NULL);
		ถ้า (skip == NULL)
		    goto theend;
		zend = alloc_state(NFA_ZEND, s1, NULL);
		ถ้า (zend == NULL)
		    goto theend;
		s1->out= skip;
		patch(e.out, zend);
		PUSH(frag(s, list1(&skip->out)));
	    }
	    มิฉะนั้น
	    {
		patch(e.out, s1);
		PUSH(frag(s, list1(&s1->out)));
		ถ้า (before)
		{
		    ถ้า (n <= 0)
			// See if we can guess the maximum width, it avoids a
			// lot of pointless tries.
			n = nfa_max_width(e.start, 0);
		    s->val = n; // store the count
		}
	    }
	    bha_nga;
	  }

	pak_sa NFA_COMPOSING:	// char with composing char
#ถ้า 0
	    // TODO
	    ถ้า (regflags & RF_ICOMBINE)
	    {
		// use the base character only
	    }
#endif
	    // FALLTHROUGH

	pak_sa NFA_MOPEN:	// \( \) Submatch
	pak_sa NFA_MOPEN1:
	pak_sa NFA_MOPEN2:
	pak_sa NFA_MOPEN3:
	pak_sa NFA_MOPEN4:
	pak_sa NFA_MOPEN5:
	pak_sa NFA_MOPEN6:
	pak_sa NFA_MOPEN7:
	pak_sa NFA_MOPEN8:
	pak_sa NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	pak_sa NFA_ZOPEN:	// \z( \) Submatch
	pak_sa NFA_ZOPEN1:
	pak_sa NFA_ZOPEN2:
	pak_sa NFA_ZOPEN3:
	pak_sa NFA_ZOPEN4:
	pak_sa NFA_ZOPEN5:
	pak_sa NFA_ZOPEN6:
	pak_sa NFA_ZOPEN7:
	pak_sa NFA_ZOPEN8:
	pak_sa NFA_ZOPEN9:
#endif
	pak_sa NFA_NOPEN:	// \%( \) "Invisible Submatch"
	    ถ้า (nfa_calc_size == TRUE)
	    {
		nstate += 2;
		bha_nga;
	    }

	    mopen = *p;
	    switch (*p)
	    {
		pak_sa NFA_NOPEN: mclose = NFA_NCLOSE; bha_nga;
#ifdef FEAT_SYN_HL
		pak_sa NFA_ZOPEN: mclose = NFA_ZCLOSE; bha_nga;
		pak_sa NFA_ZOPEN1: mclose = NFA_ZCLOSE1; bha_nga;
		pak_sa NFA_ZOPEN2: mclose = NFA_ZCLOSE2; bha_nga;
		pak_sa NFA_ZOPEN3: mclose = NFA_ZCLOSE3; bha_nga;
		pak_sa NFA_ZOPEN4: mclose = NFA_ZCLOSE4; bha_nga;
		pak_sa NFA_ZOPEN5: mclose = NFA_ZCLOSE5; bha_nga;
		pak_sa NFA_ZOPEN6: mclose = NFA_ZCLOSE6; bha_nga;
		pak_sa NFA_ZOPEN7: mclose = NFA_ZCLOSE7; bha_nga;
		pak_sa NFA_ZOPEN8: mclose = NFA_ZCLOSE8; bha_nga;
		pak_sa NFA_ZOPEN9: mclose = NFA_ZCLOSE9; bha_nga;
#endif
		pak_sa NFA_COMPOSING: mclose = NFA_END_COMPOSING; bha_nga;
		default:
		    // NFA_MOPEN, NFA_MOPEN1 .. NFA_MOPEN9
		    mclose = *p + NSUBEXP;
		    bha_nga;
	    }

	    // Allow "NFA_MOPEN" as a valid postfix representation for
	    // the empty regexp "". In this case, the NFA will be
	    // NFA_MOPEN -> NFA_MCLOSE. Note that this also allows
	    // empty groups of parenthesis, and empty mbyte chars
	    ถ้า (stackp == stack)
	    {
		s = alloc_state(mopen, NULL, NULL);
		ถ้า (s == NULL)
		    goto theend;
		s1 = alloc_state(mclose, NULL, NULL);
		ถ้า (s1 == NULL)
		    goto theend;
		patch(list1(&s->out), s1);
		PUSH(frag(s, list1(&s1->out)));
		bha_nga;
	    }

	    // At least one node was emitted before NFA_MOPEN, so
	    // at least one node will be between NFA_MOPEN and NFA_MCLOSE
	    e = POP();
	    s = alloc_state(mopen, e.start, NULL);   // `('
	    ถ้า (s == NULL)
		goto theend;

	    s1 = alloc_state(mclose, NULL, NULL);   // `)'
	    ถ้า (s1 == NULL)
		goto theend;
	    patch(e.out, s1);

	    ถ้า (mopen == NFA_COMPOSING)
		// COMPOSING->out1 = END_COMPOSING
		patch(list1(&s->out1), s1);

	    PUSH(frag(s, list1(&s1->out)));
	    bha_nga;

	pak_sa NFA_BACKREF1:
	pak_sa NFA_BACKREF2:
	pak_sa NFA_BACKREF3:
	pak_sa NFA_BACKREF4:
	pak_sa NFA_BACKREF5:
	pak_sa NFA_BACKREF6:
	pak_sa NFA_BACKREF7:
	pak_sa NFA_BACKREF8:
	pak_sa NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	pak_sa NFA_ZREF1:
	pak_sa NFA_ZREF2:
	pak_sa NFA_ZREF3:
	pak_sa NFA_ZREF4:
	pak_sa NFA_ZREF5:
	pak_sa NFA_ZREF6:
	pak_sa NFA_ZREF7:
	pak_sa NFA_ZREF8:
	pak_sa NFA_ZREF9:
#endif
	    ถ้า (nfa_calc_size == TRUE)
	    {
		nstate += 2;
		bha_nga;
	    }
	    s = alloc_state(*p, NULL, NULL);
	    ถ้า (s == NULL)
		goto theend;
	    s1 = alloc_state(NFA_SKIP, NULL, NULL);
	    ถ้า (s1 == NULL)
		goto theend;
	    patch(list1(&s->out), s1);
	    PUSH(frag(s, list1(&s1->out)));
	    bha_nga;

	pak_sa NFA_LNUM:
	pak_sa NFA_LNUM_GT:
	pak_sa NFA_LNUM_LT:
	pak_sa NFA_VCOL:
	pak_sa NFA_VCOL_GT:
	pak_sa NFA_VCOL_LT:
	pak_sa NFA_COL:
	pak_sa NFA_COL_GT:
	pak_sa NFA_COL_LT:
	pak_sa NFA_MARK:
	pak_sa NFA_MARK_GT:
	pak_sa NFA_MARK_LT:
	  {
	    จำนวนเต็ม n = *++p; // lnum, col or mark name

	    ถ้า (nfa_calc_size == TRUE)
	    {
		nstate += 1;
		bha_nga;
	    }
	    s = alloc_state(p[-1], NULL, NULL);
	    ถ้า (s == NULL)
		goto theend;
	    s->val = n;
	    PUSH(frag(s, list1(&s->out)));
	    bha_nga;
	  }

	pak_sa NFA_ZSTART:
	pak_sa NFA_ZEND:
	default:
	    // Operands
	    ถ้า (nfa_calc_size == TRUE)
	    {
		nstate++;
		bha_nga;
	    }
	    s = alloc_state(*p, NULL, NULL);
	    ถ้า (s == NULL)
		goto theend;
	    PUSH(frag(s, list1(&s->out)));
	    bha_nga;

	} // switch(*p)

    } // for (p = postfix; *p; ++p)

    ถ้า (nfa_calc_size == TRUE)
    {
	nstate++;
	goto theend;	// Return value when counting size is ignored anyway
    }

    e = POP();
    ถ้า (stackp != stack)
	EMSG_RET_NULL(_(e_nfa_regexp_while_converting_from_postfix_to_nfa_too_many_stats_left_on_stack));

    ถ้า (istate >= nstate)
	EMSG_RET_NULL(_(e_nfa_regexp_not_enough_space_to_store_whole_nfa));

    matchstate = &state_ptr[istate++]; // the match state
    matchstate->c = NFA_MATCH;
    matchstate->out = matchstate->out1 = NULL;
    matchstate->id = 0;

    patch(e.out, matchstate);
    ret = e.start;

theend:
    คืน ret;

#undef POP1
#undef PUSH1
#undef POP2
#undef PUSH2
#undef POP
#undef PUSH
}

/*
 * After building the NFA program, inspect it to add optimization hints.
 */
    static void
nfa_postprocess(nfa_regprog_T *prog)
{
    จำนวนเต็ม i;
    จำนวนเต็ม c;

    สำหรับ (i = 0; i < prog->nstate; ++i)
    {
	c = prog->state[i].c;
	ถ้า (c == NFA_START_INVISIBLE
		|| c == NFA_START_INVISIBLE_NEG
		|| c == NFA_START_INVISIBLE_BEFORE
		|| c == NFA_START_INVISIBLE_BEFORE_NEG)
	{
	    จำนวนเต็ม directly;

	    // Do it directly when what follows is possibly the end of the
	    // match.
	    ถ้า (match_follows(prog->state[i].out1->out, 0))
		directly = TRUE;
	    มิฉะนั้น
	    {
		จำนวนเต็ม ch_invisible = failure_chance(prog->state[i].out, 0);
		จำนวนเต็ม ch_follows = failure_chance(prog->state[i].out1->out, 0);

		// Postpone when the invisible match is expensive or has a
		// lower chance of failing.
		ถ้า (c == NFA_START_INVISIBLE_BEFORE
		     || c == NFA_START_INVISIBLE_BEFORE_NEG)
		{
		    // "before" matches are very expensive when
		    // unbounded, always prefer what follows then,
		    // unless what follows will always match.
		    // Otherwise strongly prefer what follows.
		    ถ้า (prog->state[i].val <= 0 && ch_follows > 0)
			directly = FALSE;
		    มิฉะนั้น
			directly = ch_follows * 10 < ch_invisible;
		}
		มิฉะนั้น
		{
		    // normal invisible, first do the one with the
		    // highest failure chance
		    directly = ch_follows < ch_invisible;
		}
	    }
	    ถ้า (directly)
		// switch to the _FIRST state
		++prog->state[i].c;
	}
    }
}

/////////////////////////////////////////////////////////////////
// NFA execution code.
/////////////////////////////////////////////////////////////////

typedef sa_nracanaa
{
    จำนวนเต็ม	    in_use; // number of subexpr with useful info

    // When REG_MULTI is TRUE list.multi is used, otherwise list.line.
    sa_ngha
    {
	sa_nracanaa multipos
	{
	    linenr_T	start_lnum;
	    linenr_T	end_lnum;
	    colnr_T	start_col;
	    colnr_T	end_col;
	} multi[NSUBEXP];
	sa_nracanaa linepos
	{
	    char_u	*start;
	    char_u	*end;
	} line[NSUBEXP];
    } list;
    colnr_T	orig_start_col;  // list.multi[0].start_col without \zs
} regsub_T;

typedef sa_nracanaa
{
    regsub_T	norm; // \( .. \) matches
#ifdef FEAT_SYN_HL
    regsub_T	synt; // \z( .. \) matches
#endif
} regsubs_T;

// nfa_pim_T stores a Postponed Invisible Match.
typedef sa_nracanaa nfa_pim_S nfa_pim_T;
sa_nracanaa nfa_pim_S
{
    จำนวนเต็ม		result;		// NFA_PIM_*, see below
    nfa_state_T	*state;		// the invisible match start state
    regsubs_T	subs;		// submatch info, only party used
    sa_ngha
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
typedef sa_nracanaa
{
    nfa_state_T	*state;
    จำนวนเต็ม		count;
    nfa_pim_T	pim;		// if pim.result != NFA_PIM_UNUSED: postponed
				// invisible match
    regsubs_T	subs;		// submatch info, only party used
} nfa_thread_T;

// nfa_list_T contains the alternative NFA execution states.
typedef sa_nracanaa
{
    nfa_thread_T    *t;		// allocated array of states
    จำนวนเต็ม		    n;		// nr of states currently in "t"
    จำนวนเต็ม		    len;	// max nr of states in "t"
    จำนวนเต็ม		    id;		// ID of the list
    จำนวนเต็ม		    has_pim;	// TRUE when any state has a PIM
} nfa_list_T;

#ifdef ENABLE_LOG
static void log_subexpr(regsub_T *sub);

    static void
log_subsexpr(regsubs_T *subs)
{
    log_subexpr(&subs->norm);
# ifdef FEAT_SYN_HL
    ถ้า (rex.nfa_has_zsubexpr)
	log_subexpr(&subs->synt);
# endif
}

    static void
log_subexpr(regsub_T *sub)
{
    จำนวนเต็ม j;

    สำหรับ (j = 0; j < sub->in_use; j++)
	ถ้า (REG_MULTI)
	    fprintf(log_fd,
		    "*** group %d, start: c=%d, l=%d, end: c=%d, l=%d\n",
		    j,
		    sub->list.multi[j].start_col,
		    (จำนวนเต็ม)sub->list.multi[j].start_lnum,
		    sub->list.multi[j].end_col,
		    (จำนวนเต็ม)sub->list.multi[j].end_lnum);
	มิฉะนั้น
	{
	    var_na *s = (var_na *)sub->list.line[j].start;
	    var_na *e = (var_na *)sub->list.line[j].end;

	    fprintf(log_fd, "*** group %d, start: \"%s\", end: \"%s\"\n",
		    j,
		    s == NULL ? "NULL" : s,
		    e == NULL ? "NULL" : e);
	}
}

    static var_na *
pim_info(nfa_pim_T *pim)
{
    static var_na buf[30];

    ถ้า (pim == NULL || pim->result == NFA_PIM_UNUSED)
	buf[0] = NUL;
    มิฉะนั้น
    {
	sprintf(buf, " PIM col %d", REG_MULTI ? (จำนวนเต็ม)pim->end.pos.col
		: (จำนวนเต็ม)(pim->end.ptr - rex.input));
    }
    คืน buf;
}

#endif

// Used during execution: whether a match has been found.
static จำนวนเต็ม	    nfa_match;
#ifdef FEAT_RELTIME
static จำนวนเต็ม	   *nfa_timed_out;
#endif

static anta_hstha void copy_sub(regsub_T *to, regsub_T *from);
static จำนวนเต็ม pim_equal(nfa_pim_T *one, nfa_pim_T *two);

/*
 * Copy postponed invisible match info from "from" to "to".
 */
    static void
copy_pim(nfa_pim_T *to, nfa_pim_T *from)
{
    to->result = from->result;
    to->state = from->state;
    copy_sub(&to->subs.norm, &from->subs.norm);
#ifdef FEAT_SYN_HL
    ถ้า (rex.nfa_has_zsubexpr)
	copy_sub(&to->subs.synt, &from->subs.synt);
#endif
    to->end = from->end;
}

    static void
clear_sub(regsub_T *sub)
{
    ถ้า (REG_MULTI)
	// Use 0xff to set lnum to -1
	vim_memset(sub->list.multi, 0xff,
				  parimaa_na(sa_nracanaa multipos) * rex.nfa_nsubexpr);
    มิฉะนั้น
	vim_memset(sub->list.line, 0,
				   parimaa_na(sa_nracanaa linepos) * rex.nfa_nsubexpr);
    sub->in_use = 0;
}

/*
 * Copy the submatches from "from" to "to".
 */
    static anta_hstha void
copy_sub(regsub_T *to, regsub_T *from)
{
    to->in_use = from->in_use;
    ถ้า (from->in_use <= 0)
	คืน;

    // Copy the match start and end positions.  addstate() calls this for
    // every state it adds, so it is very hot.  Patterns without capturing
    // groups only use subexpression zero (the whole match), so special-case
    // "in_use == 1" with a direct assignment to avoid the mch_memmove() call.
    ถ้า (REG_MULTI)
    {
	ถ้า (from->in_use == 1)
	    to->list.multi[0] = from->list.multi[0];
	มิฉะนั้น
	    mch_memmove(&to->list.multi[0],
		    &from->list.multi[0],
		    parimaa_na(sa_nracanaa multipos) * from->in_use);
	to->orig_start_col = from->orig_start_col;
    }
    มิฉะนั้น
    {
	ถ้า (from->in_use == 1)
	    to->list.line[0] = from->list.line[0];
	มิฉะนั้น
	    mch_memmove(&to->list.line[0],
		    &from->list.line[0],
		    parimaa_na(sa_nracanaa linepos) * from->in_use);
    }
}

/*
 * Like copy_sub() but exclude the main match.
 */
    static void
copy_sub_off(regsub_T *to, regsub_T *from)
{
    ถ้า (to->in_use < from->in_use)
	to->in_use = from->in_use;
    ถ้า (from->in_use <= 1)
	คืน;

    // Copy the match start and end positions.
    ถ้า (REG_MULTI)
	mch_memmove(&to->list.multi[1],
		&from->list.multi[1],
		parimaa_na(sa_nracanaa multipos) * (from->in_use - 1));
    มิฉะนั้น
	mch_memmove(&to->list.line[1],
		&from->list.line[1],
		parimaa_na(sa_nracanaa linepos) * (from->in_use - 1));
}

/*
 * Like copy_sub() but only do the end of the main match if \ze is present.
 */
    static void
copy_ze_off(regsub_T *to, regsub_T *from)
{
    ถ้า (!rex.nfa_has_zend)
	คืน;

    ถ้า (REG_MULTI)
    {
	ถ้า (from->list.multi[0].end_lnum >= 0)
	{
	    to->list.multi[0].end_lnum = from->list.multi[0].end_lnum;
	    to->list.multi[0].end_col = from->list.multi[0].end_col;
	}
    }
    มิฉะนั้น
    {
	ถ้า (from->list.line[0].end != NULL)
	    to->list.line[0].end = from->list.line[0].end;
    }
}

/*
 * Return TRUE if "sub1" and "sub2" have the same start positions.
 * When using back-references also check the end position.
 */
    static จำนวนเต็ม
sub_equal(regsub_T *sub1, regsub_T *sub2)
{
    จำนวนเต็ม		i;
    จำนวนเต็ม		todo;
    linenr_T	s1;
    linenr_T	s2;
    char_u	*sp1;
    char_u	*sp2;

    todo = sub1->in_use > sub2->in_use ? sub1->in_use : sub2->in_use;
    ถ้า (REG_MULTI)
    {
	สำหรับ (i = 0; i < todo; ++i)
	{
	    ถ้า (i < sub1->in_use)
		s1 = sub1->list.multi[i].start_lnum;
	    มิฉะนั้น
		s1 = -1;
	    ถ้า (i < sub2->in_use)
		s2 = sub2->list.multi[i].start_lnum;
	    มิฉะนั้น
		s2 = -1;
	    ถ้า (s1 != s2)
		คืน FALSE;
	    ถ้า (s1 != -1 && sub1->list.multi[i].start_col
					     != sub2->list.multi[i].start_col)
		คืน FALSE;

	    ถ้า (rex.nfa_has_backref)
	    {
		ถ้า (i < sub1->in_use)
		    s1 = sub1->list.multi[i].end_lnum;
		มิฉะนั้น
		    s1 = -1;
		ถ้า (i < sub2->in_use)
		    s2 = sub2->list.multi[i].end_lnum;
		มิฉะนั้น
		    s2 = -1;
		ถ้า (s1 != s2)
		    คืน FALSE;
		ถ้า (s1 != -1 && sub1->list.multi[i].end_col
					       != sub2->list.multi[i].end_col)
		คืน FALSE;
	    }
	}
    }
    มิฉะนั้น
    {
	สำหรับ (i = 0; i < todo; ++i)
	{
	    ถ้า (i < sub1->in_use)
		sp1 = sub1->list.line[i].start;
	    มิฉะนั้น
		sp1 = NULL;
	    ถ้า (i < sub2->in_use)
		sp2 = sub2->list.line[i].start;
	    มิฉะนั้น
		sp2 = NULL;
	    ถ้า (sp1 != sp2)
		คืน FALSE;
	    ถ้า (rex.nfa_has_backref)
	    {
		ถ้า (i < sub1->in_use)
		    sp1 = sub1->list.line[i].end;
		มิฉะนั้น
		    sp1 = NULL;
		ถ้า (i < sub2->in_use)
		    sp2 = sub2->list.line[i].end;
		มิฉะนั้น
		    sp2 = NULL;
		ถ้า (sp1 != sp2)
		    คืน FALSE;
	    }
	}
    }

    คืน TRUE;
}

#ifdef FEAT_RELTIME
/*
 * Check if we are past the time limit, if there is one.
 */
    static จำนวนเต็ม
nfa_did_time_out(void)
{
    ถ้า (*timeout_flag)
    {
	ถ้า (nfa_timed_out != NULL)
	{
# ifdef FEAT_EVAL
	    ถ้า (!*nfa_timed_out)
		ch_log(NULL, "NFA regexp timed out");
# endif
	    *nfa_timed_out = TRUE;
	}
	คืน TRUE;
    }
    คืน FALSE;
}
#endif

#ifdef ENABLE_LOG
    static void
open_debug_log(จำนวนเต็ม result)
{
    log_fd = fopen(NFA_REGEXP_RUN_LOG, "a");
    ถ้า (log_fd == NULL)
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

    static void
report_state(var_na *action,
	     regsub_T *sub,
	     nfa_state_T *state,
	     จำนวนเต็ม lid,
	     nfa_pim_T *pim)
{
    จำนวนเต็ม col;

    ถ้า (sub->in_use <= 0)
	col = -1;
    มิฉะนั้น ถ้า (REG_MULTI)
	col = sub->list.multi[0].start_col;
    มิฉะนั้น
	col = (จำนวนเต็ม)(sub->list.line[0].start - rex.line);
    nfa_set_code(state->c);
    ถ้า (log_fd == NULL)
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
    static จำนวนเต็ม
has_state_with_pos(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim)	// postponed match or NULL
{
    nfa_thread_T	*thread;
    จำนวนเต็ม			i;

    สำหรับ (i = 0; i < l->n; ++i)
    {
	thread = &l->t[i];
	ถ้า (thread->state->id == state->id
		&& sub_equal(&thread->subs.norm, &subs->norm)
#ifdef FEAT_SYN_HL
		&& (!rex.nfa_has_zsubexpr
				|| sub_equal(&thread->subs.synt, &subs->synt))
#endif
		&& pim_equal(&thread->pim, pim))
	    คืน TRUE;
    }
    คืน FALSE;
}

/*
 * Return TRUE if "one" and "two" are equal.  That includes when both are not
 * set.
 */
    static จำนวนเต็ม
pim_equal(nfa_pim_T *one, nfa_pim_T *two)
{
    จำนวนเต็ม one_unused = (one == NULL || one->result == NFA_PIM_UNUSED);
    จำนวนเต็ม two_unused = (two == NULL || two->result == NFA_PIM_UNUSED);

    ถ้า (one_unused)
	// one is unused: equal when two is also unused
	คืน two_unused;
    ถ้า (two_unused)
	// one is used and two is not: not equal
	คืน FALSE;
    // compare the state id
    ถ้า (one->state->id != two->state->id)
	คืน FALSE;
    // compare the position
    ถ้า (REG_MULTI)
	คืน one->end.pos.lnum == two->end.pos.lnum
	    && one->end.pos.col == two->end.pos.col;
    คืน one->end.ptr == two->end.ptr;
}

/*
 * Return TRUE if "state" leads to a NFA_MATCH without advancing the input.
 */
    static จำนวนเต็ม
match_follows(nfa_state_T *startstate, จำนวนเต็ม depth)
{
    nfa_state_T	    *state = startstate;

    // avoid too much recursion
    ถ้า (depth > 10)
	คืน FALSE;

    ขณะ (state != NULL)
    {
	switch (state->c)
	{
	    pak_sa NFA_MATCH:
	    pak_sa NFA_MCLOSE:
	    pak_sa NFA_END_INVISIBLE:
	    pak_sa NFA_END_INVISIBLE_NEG:
	    pak_sa NFA_END_PATTERN:
		คืน TRUE;

	    pak_sa NFA_SPLIT:
		คืน match_follows(state->out, depth + 1)
				     || match_follows(state->out1, depth + 1);

	    pak_sa NFA_START_INVISIBLE:
	    pak_sa NFA_START_INVISIBLE_FIRST:
	    pak_sa NFA_START_INVISIBLE_BEFORE:
	    pak_sa NFA_START_INVISIBLE_BEFORE_FIRST:
	    pak_sa NFA_START_INVISIBLE_NEG:
	    pak_sa NFA_START_INVISIBLE_NEG_FIRST:
	    pak_sa NFA_START_INVISIBLE_BEFORE_NEG:
	    pak_sa NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
	    pak_sa NFA_COMPOSING:
		// skip ahead to next state
		state = state->out1->out;
		continue;

	    pak_sa NFA_ANY:
	    pak_sa NFA_ANY_COMPOSING:
	    pak_sa NFA_IDENT:
	    pak_sa NFA_SIDENT:
	    pak_sa NFA_KWORD:
	    pak_sa NFA_SKWORD:
	    pak_sa NFA_FNAME:
	    pak_sa NFA_SFNAME:
	    pak_sa NFA_PRINT:
	    pak_sa NFA_SPRINT:
	    pak_sa NFA_WHITE:
	    pak_sa NFA_NWHITE:
	    pak_sa NFA_DIGIT:
	    pak_sa NFA_NDIGIT:
	    pak_sa NFA_HEX:
	    pak_sa NFA_NHEX:
	    pak_sa NFA_OCTAL:
	    pak_sa NFA_NOCTAL:
	    pak_sa NFA_WORD:
	    pak_sa NFA_NWORD:
	    pak_sa NFA_HEAD:
	    pak_sa NFA_NHEAD:
	    pak_sa NFA_ALPHA:
	    pak_sa NFA_NALPHA:
	    pak_sa NFA_LOWER:
	    pak_sa NFA_NLOWER:
	    pak_sa NFA_UPPER:
	    pak_sa NFA_NUPPER:
	    pak_sa NFA_LOWER_IC:
	    pak_sa NFA_NLOWER_IC:
	    pak_sa NFA_UPPER_IC:
	    pak_sa NFA_NUPPER_IC:
	    pak_sa NFA_START_COLL:
	    pak_sa NFA_START_NEG_COLL:
	    pak_sa NFA_NEWL:
		// state will advance input
		คืน FALSE;

	    default:
		ถ้า (state->c > 0)
		    // state will advance input
		    คืน FALSE;

		// Others: zero-width or possibly zero-width, might still find
		// a match at the same position, keep looking.
		bha_nga;
	}
	state = state->out;
    }
    คืน FALSE;
}


/*
 * Return TRUE if "state" is already in list "l".
 */
    static จำนวนเต็ม
state_in_list(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs)	// pointers to subexpressions
{
    ถ้า (state->lastlist[nfa_ll_index] == l->id)
    {
	ถ้า (!rex.nfa_has_backref || has_state_with_pos(l, state, subs, NULL))
	    คืน TRUE;
    }
    คืน FALSE;
}

// Offset used for "off" by addstate_here().
#define ADDSTATE_HERE_OFFSET 10

/*
 * Add "state" and possibly what follows to state list ".".
 * Returns "subs_arg", possibly copied into temp_subs.
 * Returns NULL when recursiveness is too deep or timed out.
 */
    static regsubs_T *
addstate(
    nfa_list_T		*l,	    // runtime state list
    nfa_state_T		*state,	    // state to update
    regsubs_T		*subs_arg,  // pointers to subexpressions
    nfa_pim_T		*pim,	    // postponed look-behind match
    จำนวนเต็ม			off_arg,    // byte offset, when -1 go to next line
    จำนวนเต็ม			depth)	    // recursion depth
{
    จำนวนเต็ม			subidx;
    จำนวนเต็ม			off = off_arg;
    จำนวนเต็ม			add_here = FALSE;
    จำนวนเต็ม			listindex = 0;
    จำนวนเต็ม			k;
    จำนวนเต็ม			found = FALSE;
    nfa_thread_T	*thread;
    sa_nracanaa multipos	save_multipos;
    จำนวนเต็ม			save_in_use;
    char_u		*save_ptr;
    จำนวนเต็ม			i;
    regsub_T		*sub;
    regsubs_T		*subs = subs_arg;
    static regsubs_T	temp_subs;
#ifdef ENABLE_LOG
    จำนวนเต็ม			did_print = FALSE;
#endif

#ifdef FEAT_RELTIME
    ถ้า (nfa_did_time_out())
	คืน NULL;
#endif

    // This function is called recursively.  When the depth is too much we run
    // out of stack and crash, limit recursiveness here.
    ถ้า (depth >= 5000 || subs == NULL)
	คืน NULL;

    ถ้า (off_arg <= -ADDSTATE_HERE_OFFSET)
    {
	add_here = TRUE;
	off = 0;
	listindex = -(off_arg + ADDSTATE_HERE_OFFSET);
    }

    switch (state->c)
    {
	pak_sa NFA_NCLOSE:
	pak_sa NFA_MCLOSE:
	pak_sa NFA_MCLOSE1:
	pak_sa NFA_MCLOSE2:
	pak_sa NFA_MCLOSE3:
	pak_sa NFA_MCLOSE4:
	pak_sa NFA_MCLOSE5:
	pak_sa NFA_MCLOSE6:
	pak_sa NFA_MCLOSE7:
	pak_sa NFA_MCLOSE8:
	pak_sa NFA_MCLOSE9:
#ifdef FEAT_SYN_HL
	pak_sa NFA_ZCLOSE:
	pak_sa NFA_ZCLOSE1:
	pak_sa NFA_ZCLOSE2:
	pak_sa NFA_ZCLOSE3:
	pak_sa NFA_ZCLOSE4:
	pak_sa NFA_ZCLOSE5:
	pak_sa NFA_ZCLOSE6:
	pak_sa NFA_ZCLOSE7:
	pak_sa NFA_ZCLOSE8:
	pak_sa NFA_ZCLOSE9:
#endif
	pak_sa NFA_MOPEN:
	pak_sa NFA_ZEND:
	pak_sa NFA_SPLIT:
	pak_sa NFA_EMPTY:
	    // These nodes are not added themselves but their "out" and/or
	    // "out1" may be added below.
	    bha_nga;

	pak_sa NFA_BOL:
	pak_sa NFA_BOF:
	    // "^" won't match past end-of-line, don't bother trying.
	    // Except when at the end of the line, or when we are going to the
	    // next line for a look-behind match.
	    ถ้า (rex.input > rex.line
		    && *rex.input != NUL
		    && (nfa_endp == NULL
			|| !REG_MULTI
			|| rex.lnum == nfa_endp->se_u.pos.lnum))
		goto skip_add;
	    // FALLTHROUGH

	pak_sa NFA_MOPEN1:
	pak_sa NFA_MOPEN2:
	pak_sa NFA_MOPEN3:
	pak_sa NFA_MOPEN4:
	pak_sa NFA_MOPEN5:
	pak_sa NFA_MOPEN6:
	pak_sa NFA_MOPEN7:
	pak_sa NFA_MOPEN8:
	pak_sa NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	pak_sa NFA_ZOPEN:
	pak_sa NFA_ZOPEN1:
	pak_sa NFA_ZOPEN2:
	pak_sa NFA_ZOPEN3:
	pak_sa NFA_ZOPEN4:
	pak_sa NFA_ZOPEN5:
	pak_sa NFA_ZOPEN6:
	pak_sa NFA_ZOPEN7:
	pak_sa NFA_ZOPEN8:
	pak_sa NFA_ZOPEN9:
#endif
	pak_sa NFA_NOPEN:
	pak_sa NFA_ZSTART:
	    // These nodes need to be added so that we can bail out when it
	    // was added to this list before at the same position to avoid an
	    // endless loop for "\(\)*"

	default:
	    ถ้า (state->lastlist[nfa_ll_index] == l->id && state->c != NFA_SKIP)
	    {
		// This state is already in the list, don't add it again,
		// unless it is an MOPEN that is used for a backreference or
		// when there is a PIM. For NFA_MATCH check the position,
		// lower position is preferred.
		ถ้า (!rex.nfa_has_backref && pim == NULL && !l->has_pim
						     && state->c != NFA_MATCH)
		{
		    // When called from addstate_here() do insert before
		    // existing states.
		    ถ้า (add_here)
		    {
			สำหรับ (k = 0; k < l->n && k < listindex; ++k)
			    ถ้า (l->t[k].state->id == state->id)
			    {
				found = TRUE;
				bha_nga;
			    }
		    }
		    ถ้า (!add_here || found)
		    {
skip_add:
#ifdef ENABLE_LOG
			nfa_set_code(state->c);
			fprintf(log_fd, "> Not adding state %d to list %d. char %d: %s pim: %s has_pim: %d found: %d\n",
			    abs(state->id), l->id, state->c, code,
			    pim == NULL ? "NULL" : "yes", l->has_pim, found);
#endif
			คืน subs;
		    }
		}

		// Do not add the state again when it exists with the same
		// positions.
		ถ้า (has_state_with_pos(l, state, subs, pim))
		    goto skip_add;
	    }

	    // When there are backreferences or PIMs the number of states may
	    // be (a lot) bigger than anticipated.
	    ถ้า (l->n == l->len)
	    {
		จำนวนเต็ม		newlen = l->len * 3 / 2 + 50;
		size_t		newsize = newlen * parimaa_na(nfa_thread_T);
		nfa_thread_T	*newt;

		ถ้า ((long)(newsize >> 10) >= p_mmp)
		{
		    emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		    คืน NULL;
		}
		ถ้า (subs != &temp_subs)
		{
		    // "subs" may point into the current array, need to make a
		    // copy before it becomes invalid.
		    copy_sub(&temp_subs.norm, &subs->norm);
#ifdef FEAT_SYN_HL
		    ถ้า (rex.nfa_has_zsubexpr)
			copy_sub(&temp_subs.synt, &subs->synt);
#endif
		    subs = &temp_subs;
		}

		newt = vim_realloc(l->t, newsize);
		ถ้า (newt == NULL)
		{
		    // out of memory
		    คืน NULL;
		}
		l->t = newt;
		l->len = newlen;
	    }

	    // add the state to the list
	    state->lastlist[nfa_ll_index] = l->id;
	    thread = &l->t[l->n++];
	    thread->state = state;
	    ถ้า (pim == NULL)
		thread->pim.result = NFA_PIM_UNUSED;
	    มิฉะนั้น
	    {
		copy_pim(&thread->pim, pim);
		l->has_pim = TRUE;
	    }
	    copy_sub(&thread->subs.norm, &subs->norm);
#ifdef FEAT_SYN_HL
	    ถ้า (rex.nfa_has_zsubexpr)
		copy_sub(&thread->subs.synt, &subs->synt);
#endif
#ifdef ENABLE_LOG
	    report_state("Adding", &thread->subs.norm, state, l->id, pim);
	    did_print = TRUE;
#endif
    }

#ifdef ENABLE_LOG
    ถ้า (!did_print)
	report_state("Processing", &subs->norm, state, l->id, pim);
#endif
    switch (state->c)
    {
	pak_sa NFA_MATCH:
	    bha_nga;

	pak_sa NFA_SPLIT:
	    // order matters here
	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    subs = addstate(l, state->out1, subs, pim, off_arg, depth + 1);
	    bha_nga;

	pak_sa NFA_EMPTY:
	pak_sa NFA_NOPEN:
	pak_sa NFA_NCLOSE:
	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    bha_nga;

	pak_sa NFA_MOPEN:
	pak_sa NFA_MOPEN1:
	pak_sa NFA_MOPEN2:
	pak_sa NFA_MOPEN3:
	pak_sa NFA_MOPEN4:
	pak_sa NFA_MOPEN5:
	pak_sa NFA_MOPEN6:
	pak_sa NFA_MOPEN7:
	pak_sa NFA_MOPEN8:
	pak_sa NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	pak_sa NFA_ZOPEN:
	pak_sa NFA_ZOPEN1:
	pak_sa NFA_ZOPEN2:
	pak_sa NFA_ZOPEN3:
	pak_sa NFA_ZOPEN4:
	pak_sa NFA_ZOPEN5:
	pak_sa NFA_ZOPEN6:
	pak_sa NFA_ZOPEN7:
	pak_sa NFA_ZOPEN8:
	pak_sa NFA_ZOPEN9:
#endif
	pak_sa NFA_ZSTART:
	    ถ้า (state->c == NFA_ZSTART)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#ifdef FEAT_SYN_HL
	    มิฉะนั้น ถ้า (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
	    {
		subidx = state->c - NFA_ZOPEN;
		sub = &subs->synt;
	    }
#endif
	    มิฉะนั้น
	    {
		subidx = state->c - NFA_MOPEN;
		sub = &subs->norm;
	    }

	    // avoid compiler warnings
	    save_ptr = NULL;
	    CLEAR_FIELD(save_multipos);

	    // Set the position (with "off" added) in the subexpression.  Save
	    // and restore it when it was in use.  Otherwise fill any gap.
	    ถ้า (REG_MULTI)
	    {
		ถ้า (subidx < sub->in_use)
		{
		    save_multipos = sub->list.multi[subidx];
		    save_in_use = -1;
		}
		มิฉะนั้น
		{
		    save_in_use = sub->in_use;
		    สำหรับ (i = sub->in_use; i < subidx; ++i)
		    {
			sub->list.multi[i].start_lnum = -1;
			sub->list.multi[i].end_lnum = -1;
		    }
		    sub->in_use = subidx + 1;
		}
		ถ้า (off == -1)
		{
		    sub->list.multi[subidx].start_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].start_col = 0;
		}
		มิฉะนั้น
		{
		    sub->list.multi[subidx].start_lnum = rex.lnum;
		    sub->list.multi[subidx].start_col =
					 (colnr_T)(rex.input - rex.line + off);
		}
		sub->list.multi[subidx].end_lnum = -1;
	    }
	    มิฉะนั้น
	    {
		ถ้า (subidx < sub->in_use)
		{
		    save_ptr = sub->list.line[subidx].start;
		    save_in_use = -1;
		}
		มิฉะนั้น
		{
		    save_in_use = sub->in_use;
		    สำหรับ (i = sub->in_use; i < subidx; ++i)
		    {
			sub->list.line[i].start = NULL;
			sub->list.line[i].end = NULL;
		    }
		    sub->in_use = subidx + 1;
		}
		sub->list.line[subidx].start = rex.input + off;
	    }

	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    ถ้า (subs == NULL)
		bha_nga;
	    // "subs" may have changed, need to set "sub" again
#ifdef FEAT_SYN_HL
	    ถ้า (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
		sub = &subs->synt;
	    มิฉะนั้น
#endif
		sub = &subs->norm;

	    ถ้า (save_in_use == -1)
	    {
		ถ้า (REG_MULTI)
		    sub->list.multi[subidx] = save_multipos;
		มิฉะนั้น
		    sub->list.line[subidx].start = save_ptr;
	    }
	    มิฉะนั้น
		sub->in_use = save_in_use;
	    bha_nga;

	pak_sa NFA_MCLOSE:
	    ถ้า (rex.nfa_has_zend && (REG_MULTI
			? subs->norm.list.multi[0].end_lnum >= 0
			: subs->norm.list.line[0].end != NULL))
	    {
		// Do not overwrite the position set by \ze.
		subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
		bha_nga;
	    }
	    // FALLTHROUGH
	pak_sa NFA_MCLOSE1:
	pak_sa NFA_MCLOSE2:
	pak_sa NFA_MCLOSE3:
	pak_sa NFA_MCLOSE4:
	pak_sa NFA_MCLOSE5:
	pak_sa NFA_MCLOSE6:
	pak_sa NFA_MCLOSE7:
	pak_sa NFA_MCLOSE8:
	pak_sa NFA_MCLOSE9:
#ifdef FEAT_SYN_HL
	pak_sa NFA_ZCLOSE:
	pak_sa NFA_ZCLOSE1:
	pak_sa NFA_ZCLOSE2:
	pak_sa NFA_ZCLOSE3:
	pak_sa NFA_ZCLOSE4:
	pak_sa NFA_ZCLOSE5:
	pak_sa NFA_ZCLOSE6:
	pak_sa NFA_ZCLOSE7:
	pak_sa NFA_ZCLOSE8:
	pak_sa NFA_ZCLOSE9:
#endif
	pak_sa NFA_ZEND:
	    ถ้า (state->c == NFA_ZEND)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#ifdef FEAT_SYN_HL
	    มิฉะนั้น ถ้า (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
	    {
		subidx = state->c - NFA_ZCLOSE;
		sub = &subs->synt;
	    }
#endif
	    มิฉะนั้น
	    {
		subidx = state->c - NFA_MCLOSE;
		sub = &subs->norm;
	    }

	    // We don't fill in gaps here, there must have been an MOPEN that
	    // has done that.
	    save_in_use = sub->in_use;
	    ถ้า (sub->in_use <= subidx)
		sub->in_use = subidx + 1;
	    ถ้า (REG_MULTI)
	    {
		save_multipos = sub->list.multi[subidx];
		ถ้า (off == -1)
		{
		    sub->list.multi[subidx].end_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].end_col = 0;
		}
		มิฉะนั้น
		{
		    sub->list.multi[subidx].end_lnum = rex.lnum;
		    sub->list.multi[subidx].end_col =
					  (colnr_T)(rex.input - rex.line + off);
		}
		// avoid compiler warnings
		save_ptr = NULL;
	    }
	    มิฉะนั้น
	    {
		save_ptr = sub->list.line[subidx].end;
		sub->list.line[subidx].end = rex.input + off;
		// avoid compiler warnings
		CLEAR_FIELD(save_multipos);
	    }

	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    ถ้า (subs == NULL)
		bha_nga;
	    // "subs" may have changed, need to set "sub" again
#ifdef FEAT_SYN_HL
	    ถ้า (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
		sub = &subs->synt;
	    มิฉะนั้น
#endif
		sub = &subs->norm;

	    ถ้า (REG_MULTI)
		sub->list.multi[subidx] = save_multipos;
	    มิฉะนั้น
		sub->list.line[subidx].end = save_ptr;
	    sub->in_use = save_in_use;
	    bha_nga;
    }
    คืน subs;
}

/*
 * Like addstate(), but the new state(s) are put at position "*ip".
 * Used for zero-width matches, next state to use is the added one.
 * This makes sure the order of states to be tried does not change, which
 * matters for alternatives.
 */
    static regsubs_T *
addstate_here(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim,   // postponed look-behind match
    จำนวนเต็ม			*ip)
{
    จำนวนเต็ม tlen = l->n;
    จำนวนเต็ม count;
    จำนวนเต็ม listidx = *ip;
    regsubs_T *r;

    // First add the state(s) at the end, so that we know how many there are.
    // Pass the listidx as offset (avoids adding another argument to
    // addstate()).
    r = addstate(l, state, subs, pim, -listidx - ADDSTATE_HERE_OFFSET, 0);
    ถ้า (r == NULL)
	คืน NULL;

    // when "*ip" was at the end of the list, nothing to do
    ถ้า (listidx + 1 == tlen)
	คืน r;

    // re-order to put the new state at the current position
    count = l->n - tlen;
    ถ้า (count == 0)
	คืน r; // no state got added
    ถ้า (count == 1)
    {
	// overwrite the current state
	l->t[listidx] = l->t[l->n - 1];
    }
    มิฉะนั้น ถ้า (count > 1)
    {
	ถ้า (l->n + count - 1 >= l->len)
	{
	    // not enough space to move the new states, reallocate the list
	    // and move the states to the right position
	    จำนวนเต็ม		    newlen = l->len * 3 / 2 + 50;
	    size_t	    newsize = newlen * parimaa_na(nfa_thread_T);
	    nfa_thread_T    *newl;

	    ถ้า ((long)(newsize >> 10) >= p_mmp)
	    {
		emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		คืน NULL;
	    }
	    newl = alloc(newsize);
	    ถ้า (newl == NULL)
		คืน NULL;
	    l->len = newlen;
	    mch_memmove(&(newl[0]),
		    &(l->t[0]),
		    parimaa_na(nfa_thread_T) * listidx);
	    mch_memmove(&(newl[listidx]),
		    &(l->t[l->n - count]),
		    parimaa_na(nfa_thread_T) * count);
	    mch_memmove(&(newl[listidx + count]),
		    &(l->t[listidx + 1]),
		    parimaa_na(nfa_thread_T) * (l->n - count - listidx - 1));
	    vim_free(l->t);
	    l->t = newl;
	}
	มิฉะนั้น
	{
	    // make space for new states, then move them from the
	    // end to the current position
	    mch_memmove(&(l->t[listidx + count]),
		    &(l->t[listidx + 1]),
		    parimaa_na(nfa_thread_T) * (l->n - listidx - 1));
	    mch_memmove(&(l->t[listidx]),
		    &(l->t[l->n - 1]),
		    parimaa_na(nfa_thread_T) * count);
	}
    }
    --l->n;
    *ip = listidx - 1;

    คืน r;
}

/*
 * Check character class "class" against current character c.
 */
    static จำนวนเต็ม
check_char_class(จำนวนเต็ม class, จำนวนเต็ม c)
{
    switch (class)
    {
	pak_sa NFA_CLASS_ALNUM:
	    ถ้า (c >= 1 && c < 128 && isalnum(c))
		คืน OK;
	    bha_nga;
	pak_sa NFA_CLASS_ALPHA:
	    ถ้า (c >= 1 && c < 128 && isalpha(c))
		คืน OK;
	    bha_nga;
	pak_sa NFA_CLASS_BLANK:
	    ถ้า (c == ' ' || c == '\t')
		คืน OK;
	    bha_nga;
	pak_sa NFA_CLASS_CNTRL:
	    ถ้า (c >= 1 && c <= 127 && iscntrl(c))
		คืน OK;
	    bha_nga;
	pak_sa NFA_CLASS_DIGIT:
	    ถ้า (VIM_ISDIGIT(c))
		คืน OK;
	    bha_nga;
	pak_sa NFA_CLASS_GRAPH:
	    ถ้า (c >= 1 && c <= 127 && isgraph(c))
		คืน OK;
	    bha_nga;
	pak_sa NFA_CLASS_LOWER:
	    ถ้า (MB_ISLOWER(c) && c != 170 && c != 186)
		คืน OK;
	    bha_nga;
	pak_sa NFA_CLASS_PRINT:
	    ถ้า (vim_isprintc(c))
		คืน OK;
	    bha_nga;
	pak_sa NFA_CLASS_PUNCT:
	    ถ้า (c >= 1 && c < 128 && ispunct(c))
		คืน OK;
	    bha_nga;
	pak_sa NFA_CLASS_SPACE:
	    ถ้า ((c >= 9 && c <= 13) || (c == ' '))
		คืน OK;
	    bha_nga;
	pak_sa NFA_CLASS_UPPER:
	    ถ้า (MB_ISUPPER(c))
		คืน OK;
	    bha_nga;
	pak_sa NFA_CLASS_XDIGIT:
	    ถ้า (vim_isxdigit(c))
		คืน OK;
	    bha_nga;
	pak_sa NFA_CLASS_TAB:
	    ถ้า (c == '\t')
		คืน OK;
	    bha_nga;
	pak_sa NFA_CLASS_RETURN:
	    ถ้า (c == '\r')
		คืน OK;
	    bha_nga;
	pak_sa NFA_CLASS_BACKSPACE:
	    ถ้า (c == '\b')
		คืน OK;
	    bha_nga;
	pak_sa NFA_CLASS_ESCAPE:
	    ถ้า (c == '\033')
		คืน OK;
	    bha_nga;
	pak_sa NFA_CLASS_IDENT:
	    ถ้า (vim_isIDc(c))
		คืน OK;
	    bha_nga;
	pak_sa NFA_CLASS_KEYWORD:
	    ถ้า (reg_iswordc(c))
		คืน OK;
	    bha_nga;
	pak_sa NFA_CLASS_FNAME:
	    ถ้า (vim_isfilec(c))
		คืน OK;
	    bha_nga;

	default:
	    // should not be here :P
	    siemsg(_(e_nfa_regexp_invalid_character_class_nr), class);
	    คืน FAIL;
    }
    คืน FAIL;
}

/*
 * Check for a match with subexpression "subidx".
 * Return TRUE if it matches.
 */
    static จำนวนเต็ม
match_backref(
    regsub_T	*sub,	    // pointers to subexpressions
    จำนวนเต็ม		subidx,
    จำนวนเต็ม		*bytelen)   // out: length of match in bytes
{
    จำนวนเต็ม		len;

    ถ้า (sub->in_use <= subidx)
    {
retempty:
	// backref was not set, match an empty string
	*bytelen = 0;
	คืน TRUE;
    }

    ถ้า (REG_MULTI)
    {
	ถ้า (sub->list.multi[subidx].start_lnum < 0
				       || sub->list.multi[subidx].end_lnum < 0)
	    goto retempty;
	ถ้า (sub->list.multi[subidx].start_lnum == rex.lnum
			       && sub->list.multi[subidx].end_lnum == rex.lnum)
	{
	    len = sub->list.multi[subidx].end_col
					  - sub->list.multi[subidx].start_col;
	    ถ้า (cstrncmp(rex.line + sub->list.multi[subidx].start_col,
							 rex.input, &len) == 0)
	    {
		*bytelen = len;
		คืน TRUE;
	    }
	}
	มิฉะนั้น
	{
	    ถ้า (match_with_backref(
			sub->list.multi[subidx].start_lnum,
			sub->list.multi[subidx].start_col,
			sub->list.multi[subidx].end_lnum,
			sub->list.multi[subidx].end_col,
			bytelen) == RA_MATCH)
		คืน TRUE;
	}
    }
    มิฉะนั้น
    {
	ถ้า (sub->list.line[subidx].start == NULL
					|| sub->list.line[subidx].end == NULL)
	    goto retempty;
	len = (จำนวนเต็ม)(sub->list.line[subidx].end - sub->list.line[subidx].start);
	ถ้า (cstrncmp(sub->list.line[subidx].start, rex.input, &len) == 0)
	{
	    *bytelen = len;
	    คืน TRUE;
	}
    }
    คืน FALSE;
}

#ifdef FEAT_SYN_HL

/*
 * Check for a match with \z subexpression "subidx".
 * Return TRUE if it matches.
 */
    static จำนวนเต็ม
match_zref(
    จำนวนเต็ม		subidx,
    จำนวนเต็ม		*bytelen)   // out: length of match in bytes
{
    จำนวนเต็ม		len;

    cleanup_zsubexpr();
    ถ้า (re_extmatch_in == NULL || re_extmatch_in->matches[subidx] == NULL)
    {
	// backref was not set, match an empty string
	*bytelen = 0;
	คืน TRUE;
    }

    len = (จำนวนเต็ม)STRLEN(re_extmatch_in->matches[subidx]);
    ถ้า (cstrncmp(re_extmatch_in->matches[subidx], rex.input, &len) == 0)
    {
	*bytelen = len;
	คืน TRUE;
    }
    คืน FALSE;
}
#endif

/*
 * Save list IDs for all NFA states of "prog" into "list".
 * Also reset the IDs to zero.
 * Only used for the recursive value lastlist[1].
 */
    static void
nfa_save_listids(nfa_regprog_T *prog, จำนวนเต็ม *list)
{
    จำนวนเต็ม		    i;
    nfa_state_T	    *p;

    // Order in the list is reverse, it's a bit faster that way.
    p = &prog->state[0];
    สำหรับ (i = prog->nstate; --i >= 0; )
    {
	list[i] = p->lastlist[1];
	p->lastlist[1] = 0;
	++p;
    }
}

/*
 * Restore list IDs from "list" to all NFA states.
 */
    static void
nfa_restore_listids(nfa_regprog_T *prog, จำนวนเต็ม *list)
{
    จำนวนเต็ม		    i;
    nfa_state_T	    *p;

    p = &prog->state[0];
    สำหรับ (i = prog->nstate; --i >= 0; )
    {
	p->lastlist[1] = list[i];
	++p;
    }
}

    static จำนวนเต็ม
nfa_re_num_cmp(long_u val, จำนวนเต็ม op, long_u pos)
{
    ถ้า (op == 1) คืน pos > val;
    ถ้า (op == 2) คืน pos < val;
    คืน val == pos;
}

static จำนวนเต็ม nfa_regmatch(nfa_regprog_T *prog, nfa_state_T *start, regsubs_T *submatch, regsubs_T *m);

/*
 * Recursively call nfa_regmatch()
 * "pim" is NULL or contains info about a Postponed Invisible Match (start
 * position).
 */
    static จำนวนเต็ม
recursive_regmatch(
    nfa_state_T	    *state,
    nfa_pim_T	    *pim,
    nfa_regprog_T   *prog,
    regsubs_T	    *submatch,
    regsubs_T	    *m,
    จำนวนเต็ม		    **listids,
    จำนวนเต็ม		    *listids_len)
{
    จำนวนเต็ม		save_reginput_col = (จำนวนเต็ม)(rex.input - rex.line);
    จำนวนเต็ม		save_reglnum = rex.lnum;
    จำนวนเต็ม		save_nfa_match = nfa_match;
    จำนวนเต็ม		save_nfa_listid = rex.nfa_listid;
    save_se_T   *save_nfa_endp = nfa_endp;
    save_se_T   endpos;
    save_se_T   *endposp = NULL;
    จำนวนเต็ม		result;
    จำนวนเต็ม		need_restore = FALSE;

    ถ้า (pim != NULL)
    {
	// start at the position where the postponed match was
	ถ้า (REG_MULTI)
	    rex.input = rex.line + pim->end.pos.col;
	มิฉะนั้น
	    rex.input = pim->end.ptr;
    }

    ถ้า (state->c == NFA_START_INVISIBLE_BEFORE
	    || state->c == NFA_START_INVISIBLE_BEFORE_FIRST
	    || state->c == NFA_START_INVISIBLE_BEFORE_NEG
	    || state->c == NFA_START_INVISIBLE_BEFORE_NEG_FIRST)
    {
	// The recursive match must end at the current position. When "pim" is
	// not NULL it specifies the current position.
	endposp = &endpos;
	ถ้า (REG_MULTI)
	{
	    ถ้า (pim == NULL)
	    {
		endpos.se_u.pos.col = (จำนวนเต็ม)(rex.input - rex.line);
		endpos.se_u.pos.lnum = rex.lnum;
	    }
	    มิฉะนั้น
		endpos.se_u.pos = pim->end.pos;
	}
	มิฉะนั้น
	{
	    ถ้า (pim == NULL)
		endpos.se_u.ptr = rex.input;
	    มิฉะนั้น
		endpos.se_u.ptr = pim->end.ptr;
	}

	// Go back the specified number of bytes, or as far as the
	// start of the previous line, to try matching "\@<=" or
	// not matching "\@<!". This is very inefficient, limit the number of
	// bytes if possible.
	ถ้า (state->val <= 0)
	{
	    ถ้า (REG_MULTI)
	    {
		rex.line = reg_getline(--rex.lnum);
		ถ้า (rex.line == NULL)
		    // can't go before the first line
		    rex.line = reg_getline(++rex.lnum);
	    }
	    rex.input = rex.line;
	}
	มิฉะนั้น
	{
	    ถ้า (REG_MULTI && (จำนวนเต็ม)(rex.input - rex.line) < state->val)
	    {
		// Not enough bytes in this line, go to end of
		// previous line.
		rex.line = reg_getline(--rex.lnum);
		ถ้า (rex.line == NULL)
		{
		    // can't go before the first line
		    rex.line = reg_getline(++rex.lnum);
		    rex.input = rex.line;
		}
		มิฉะนั้น
		    rex.input = rex.line + reg_getline_len(rex.lnum);
	    }
	    ถ้า ((จำนวนเต็ม)(rex.input - rex.line) >= state->val)
	    {
		rex.input -= state->val;
		ถ้า (has_mbyte)
		    rex.input -= mb_head_off(rex.line, rex.input);
	    }
	    มิฉะนั้น
		rex.input = rex.line;
	}
    }

#ifdef ENABLE_LOG
    ถ้า (log_fd != stderr)
	fclose(log_fd);
    log_fd = NULL;
#endif
    // Have to clear the lastlist field of the NFA nodes, so that
    // nfa_regmatch() and addstate() can run properly after recursion.
    ถ้า (nfa_ll_index == 1)
    {
	// Already calling nfa_regmatch() recursively.  Save the lastlist[1]
	// values and clear them.
	ถ้า (*listids == NULL || *listids_len < prog->nstate)
	{
	    vim_free(*listids);
	    *listids = ALLOC_MULT(จำนวนเต็ม, prog->nstate);
	    ถ้า (*listids == NULL)
	    {
		emsg(_(e_nfa_regexp_could_not_allocate_memory_for_branch_traversal));
		คืน 0;
	    }
	    *listids_len = prog->nstate;
	}
	nfa_save_listids(prog, *listids);
	need_restore = TRUE;
	// any value of rex.nfa_listid will do
    }
    มิฉะนั้น
    {
	// First recursive nfa_regmatch() call, switch to the second lastlist
	// entry.  Make sure rex.nfa_listid is different from a previous
	// recursive call, because some states may still have this ID.
	++nfa_ll_index;
	ถ้า (rex.nfa_listid <= rex.nfa_alt_listid)
	    rex.nfa_listid = rex.nfa_alt_listid;
    }

    // Call nfa_regmatch() to check if the current concat matches at this
    // position. The concat ends with the node NFA_END_INVISIBLE
    nfa_endp = endposp;
    result = nfa_regmatch(prog, state->out, submatch, m);

    ถ้า (need_restore)
	nfa_restore_listids(prog, *listids);
    มิฉะนั้น
    {
	--nfa_ll_index;
	rex.nfa_alt_listid = rex.nfa_listid;
    }

    // restore position in input text
    rex.lnum = save_reglnum;
    ถ้า (REG_MULTI)
	rex.line = reg_getline(rex.lnum);
    rex.input = rex.line + save_reginput_col;
    ถ้า (result != NFA_TOO_EXPENSIVE)
    {
	nfa_match = save_nfa_match;
	rex.nfa_listid = save_nfa_listid;
    }
    nfa_endp = save_nfa_endp;

#ifdef ENABLE_LOG
    open_debug_log(result);
#endif

    คืน result;
}

/*
 * Estimate the chance of a match with "state" failing.
 * empty match: 0
 * NFA_ANY: 1
 * specific character: 99
 */
    static จำนวนเต็ม
failure_chance(nfa_state_T *state, จำนวนเต็ม depth)
{
    จำนวนเต็ม c = state->c;
    จำนวนเต็ม l, r;

    // detect looping
    ถ้า (depth > 4)
	คืน 1;

    switch (c)
    {
	pak_sa NFA_SPLIT:
	    ถ้า (state->out->c == NFA_SPLIT || state->out1->c == NFA_SPLIT)
		// avoid recursive stuff
		คืน 1;
	    // two alternatives, use the lowest failure chance
	    l = failure_chance(state->out, depth + 1);
	    r = failure_chance(state->out1, depth + 1);
	    คืน l < r ? l : r;

	pak_sa NFA_ANY:
	    // matches anything, unlikely to fail
	    คืน 1;

	pak_sa NFA_MATCH:
	pak_sa NFA_MCLOSE:
	pak_sa NFA_ANY_COMPOSING:
	    // empty match works always
	    คืน 0;

	pak_sa NFA_START_INVISIBLE:
	pak_sa NFA_START_INVISIBLE_FIRST:
	pak_sa NFA_START_INVISIBLE_NEG:
	pak_sa NFA_START_INVISIBLE_NEG_FIRST:
	pak_sa NFA_START_INVISIBLE_BEFORE:
	pak_sa NFA_START_INVISIBLE_BEFORE_FIRST:
	pak_sa NFA_START_INVISIBLE_BEFORE_NEG:
	pak_sa NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
	pak_sa NFA_START_PATTERN:
	    // recursive regmatch is expensive, use low failure chance
	    คืน 5;

	pak_sa NFA_BOL:
	pak_sa NFA_EOL:
	pak_sa NFA_BOF:
	pak_sa NFA_EOF:
	pak_sa NFA_NEWL:
	    คืน 99;

	pak_sa NFA_BOW:
	pak_sa NFA_EOW:
	    คืน 90;

	pak_sa NFA_MOPEN:
	pak_sa NFA_MOPEN1:
	pak_sa NFA_MOPEN2:
	pak_sa NFA_MOPEN3:
	pak_sa NFA_MOPEN4:
	pak_sa NFA_MOPEN5:
	pak_sa NFA_MOPEN6:
	pak_sa NFA_MOPEN7:
	pak_sa NFA_MOPEN8:
	pak_sa NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	pak_sa NFA_ZOPEN:
	pak_sa NFA_ZOPEN1:
	pak_sa NFA_ZOPEN2:
	pak_sa NFA_ZOPEN3:
	pak_sa NFA_ZOPEN4:
	pak_sa NFA_ZOPEN5:
	pak_sa NFA_ZOPEN6:
	pak_sa NFA_ZOPEN7:
	pak_sa NFA_ZOPEN8:
	pak_sa NFA_ZOPEN9:
	pak_sa NFA_ZCLOSE:
	pak_sa NFA_ZCLOSE1:
	pak_sa NFA_ZCLOSE2:
	pak_sa NFA_ZCLOSE3:
	pak_sa NFA_ZCLOSE4:
	pak_sa NFA_ZCLOSE5:
	pak_sa NFA_ZCLOSE6:
	pak_sa NFA_ZCLOSE7:
	pak_sa NFA_ZCLOSE8:
	pak_sa NFA_ZCLOSE9:
#endif
	pak_sa NFA_NOPEN:
	pak_sa NFA_MCLOSE1:
	pak_sa NFA_MCLOSE2:
	pak_sa NFA_MCLOSE3:
	pak_sa NFA_MCLOSE4:
	pak_sa NFA_MCLOSE5:
	pak_sa NFA_MCLOSE6:
	pak_sa NFA_MCLOSE7:
	pak_sa NFA_MCLOSE8:
	pak_sa NFA_MCLOSE9:
	pak_sa NFA_NCLOSE:
	    คืน failure_chance(state->out, depth + 1);

	pak_sa NFA_BACKREF1:
	pak_sa NFA_BACKREF2:
	pak_sa NFA_BACKREF3:
	pak_sa NFA_BACKREF4:
	pak_sa NFA_BACKREF5:
	pak_sa NFA_BACKREF6:
	pak_sa NFA_BACKREF7:
	pak_sa NFA_BACKREF8:
	pak_sa NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	pak_sa NFA_ZREF1:
	pak_sa NFA_ZREF2:
	pak_sa NFA_ZREF3:
	pak_sa NFA_ZREF4:
	pak_sa NFA_ZREF5:
	pak_sa NFA_ZREF6:
	pak_sa NFA_ZREF7:
	pak_sa NFA_ZREF8:
	pak_sa NFA_ZREF9:
#endif
	    // backreferences don't match in many places
	    คืน 94;

	pak_sa NFA_LNUM_GT:
	pak_sa NFA_LNUM_LT:
	pak_sa NFA_COL_GT:
	pak_sa NFA_COL_LT:
	pak_sa NFA_VCOL_GT:
	pak_sa NFA_VCOL_LT:
	pak_sa NFA_MARK_GT:
	pak_sa NFA_MARK_LT:
	pak_sa NFA_VISUAL:
	    // before/after positions don't match very often
	    คืน 85;

	pak_sa NFA_LNUM:
	    คืน 90;

	pak_sa NFA_CURSOR:
	pak_sa NFA_COL:
	pak_sa NFA_VCOL:
	pak_sa NFA_MARK:
	    // specific positions rarely match
	    คืน 98;

	pak_sa NFA_COMPOSING:
	    คืน 95;

	default:
	    ถ้า (c > 0)
		// character match fails often
		คืน 95;
    }

    // something else, includes character classes
    คืน 50;
}

/*
 * Skip until the char "c" we know a match must start with.
 */
    static จำนวนเต็ม
skip_to_start(จำนวนเต็ม c, colnr_T *colp)
{
    char_u *s;

    // Used often, do some work to avoid call overhead.
    ถ้า (!rex.reg_ic && !has_mbyte)
	s = vim_strbyte(rex.line + *colp, c);
    มิฉะนั้น
	s = cstrchr(rex.line + *colp, c);
    ถ้า (s == NULL)
	คืน FAIL;
    *colp = (จำนวนเต็ม)(s - rex.line);
    คืน OK;
}

/*
 * Check for a match with match_text.
 * Called after skip_to_start() has found regstart.
 * Returns zero for no match, 1 for a match.
 */
    static long
find_match_text(colnr_T *startcol, จำนวนเต็ม regstart, char_u *match_text)
{
    colnr_T col = *startcol;
    จำนวนเต็ม	    c1, c2;
    จำนวนเต็ม	    len1, len2;
    จำนวนเต็ม	    match;

    สำหรับ (;;)
    {
	match = TRUE;
	// Skip the input character: case-folding can change its byte length.
	len2 = MB_CPTR2LEN(rex.line + col);
	สำหรับ (len1 = 0; match_text[len1] != NUL; len1 += MB_CHAR2LEN(c1))
	{
	    c1 = PTR2CHAR(match_text + len1);
	    c2 = PTR2CHAR(rex.line + col + len2);
	    ถ้า (c1 != c2 && (!rex.reg_ic || MB_CASEFOLD(c1) != MB_CASEFOLD(c2)))
	    {
		match = FALSE;
		bha_nga;
	    }
	    len2 += enc_utf8 ? utf_ptr2len(rex.line + col + len2)
							     : MB_CHAR2LEN(c2);
	}
	ถ้า (match
		// check that no composing char follows
		&& !(enc_utf8
			  && utf_iscomposing(PTR2CHAR(rex.line + col + len2))))
	{
	    cleanup_subexpr();
	    ถ้า (REG_MULTI)
	    {
		rex.reg_startpos[0].lnum = rex.lnum;
		rex.reg_startpos[0].col = col;
		rex.reg_endpos[0].lnum = rex.lnum;
		rex.reg_endpos[0].col = col + len2;
	    }
	    มิฉะนั้น
	    {
		rex.reg_startp[0] = rex.line + col;
		rex.reg_endp[0] = rex.line + col + len2;
	    }
	    *startcol = col;
	    คืน 1L;
	}

	// Try finding regstart after the current match.
	col += MB_CPTR2LEN(rex.line + col); // skip the input character
	ถ้า (skip_to_start(regstart, &col) == FAIL)
	    bha_nga;
    }

    *startcol = col;
    คืน 0L;
}

/*
 * Check whether the composing characters at the current input position match
 * the NFA composing sub-expression that starts at "sta" (the first state
 * after NFA_COMPOSING).  "curc" is the base character and "clen" its byte
 * length.
 * Returns OK when it matches, FAIL otherwise.
 */
    static จำนวนเต็ม
match_composing(nfa_state_T *sta, จำนวนเต็ม curc, จำนวนเต็ม clen)
{
    จำนวนเต็ม		mc = curc;
    จำนวนเต็ม		len = 0;
    จำนวนเต็ม		cchars[MAX_MCO];
    จำนวนเต็ม		ccount = 0;
    จำนวนเต็ม		j;

    ถ้า (utf_iscomposing(sta->c))
	// Only match composing character(s), ignore base character.  Used
	// for ".{composing}" and "{composing}" (no preceding character).
	len += mb_char2len(mc);

    ถ้า (rex.reg_icombine && len == 0)
	// If \Z was present, then ignore composing characters.  When
	// ignoring the base character this always matches.
	คืน sta->c == curc ? OK : FAIL;

    // Check base character matches first, unless ignored.
    ถ้า (len == 0 && mc != sta->c)
	คืน FAIL;

    ถ้า (len == 0)
    {
	len += mb_char2len(mc);
	sta = sta->out;
    }

    // We don't care about the order of composing characters.  Get them into
    // cchars[] first.
    ขณะ (len < clen)
    {
	mc = mb_ptr2char(rex.input + len);
	cchars[ccount++] = mc;
	len += mb_char2len(mc);
	ถ้า (ccount == MAX_MCO)
	    bha_nga;
    }

    // Check that each composing char in the pattern matches a composing char
    // in the text.  We do not check if all composing chars are matched.
    ขณะ (sta->c != NFA_END_COMPOSING)
    {
	สำหรับ (j = 0; j < ccount; ++j)
	    ถ้า (cchars[j] == sta->c)
		bha_nga;
	ถ้า (j == ccount)
	    คืน FAIL;
	sta = sta->out;
    }

    คืน OK;
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
    static จำนวนเต็ม
nfa_regmatch(
    nfa_regprog_T	*prog,
    nfa_state_T		*start,
    regsubs_T		*submatch,
    regsubs_T		*m)
{
    จำนวนเต็ม		result = FALSE;
    size_t	size = 0;
    จำนวนเต็ม		flag = 0;
    จำนวนเต็ม		go_to_nextline = FALSE;
    nfa_thread_T *t;
    nfa_list_T	list[2];
    จำนวนเต็ม		listidx;
    nfa_list_T	*thislist;
    nfa_list_T	*nextlist;
    จำนวนเต็ม		*listids = NULL;
    จำนวนเต็ม		listids_len = 0;
    nfa_state_T *add_state;
    จำนวนเต็ม		add_here;
    จำนวนเต็ม		add_count;
    จำนวนเต็ม		add_off = 0;
    จำนวนเต็ม		toplevel = start->c == NFA_MOPEN;
    regsubs_T	*r;
#ifdef NFA_REGEXP_DEBUG_LOG
    FILE	*debug;
#endif

    // Some patterns may take a long time to match, especially when using
    // recursive_regmatch(). Allow interrupting them with CTRL-C.
    fast_breakcheck();
    ถ้า (got_int)
	คืน FALSE;
#ifdef FEAT_RELTIME
    ถ้า (nfa_did_time_out())
	คืน FALSE;
#endif

#ifdef NFA_REGEXP_DEBUG_LOG
    debug = fopen(NFA_REGEXP_DEBUG_LOG, "a");
    ถ้า (debug == NULL)
    {
	semsg("(NFA) COULD NOT OPEN %s!", NFA_REGEXP_DEBUG_LOG);
	คืน FALSE;
    }
#endif
    nfa_match = FALSE;

    // Allocate memory for the lists of nodes.
    size = (prog->nstate + 1) * parimaa_na(nfa_thread_T);

    // Reuse cached list buffers from prog when available (top-level call).
    // Recursive calls must allocate their own buffers.
    ถ้า (toplevel && prog->listbuf[0] != NULL)
    {
	list[0].t = (nfa_thread_T *)prog->listbuf[0];
	list[1].t = (nfa_thread_T *)prog->listbuf[1];
	prog->listbuf[0] = NULL;
	prog->listbuf[1] = NULL;
    }
    มิฉะนั้น
    {
	list[0].t = alloc(size);
	list[1].t = alloc(size);
    }
    list[0].len = prog->nstate + 1;
    list[1].len = prog->nstate + 1;
    ถ้า (list[0].t == NULL || list[1].t == NULL)
	goto theend;

#ifdef ENABLE_LOG
    log_fd = fopen(NFA_REGEXP_RUN_LOG, "a");
    ถ้า (log_fd == NULL)
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
    ถ้า (toplevel)
    {
	ถ้า (REG_MULTI)
	{
	    m->norm.list.multi[0].start_lnum = rex.lnum;
	    m->norm.list.multi[0].start_col = (colnr_T)(rex.input - rex.line);
	    m->norm.orig_start_col = m->norm.list.multi[0].start_col;
	}
	มิฉะนั้น
	    m->norm.list.line[0].start = rex.input;
	m->norm.in_use = 1;
	r = addstate(thislist, start->out, m, NULL, 0, 0);
    }
    มิฉะนั้น
	r = addstate(thislist, start, m, NULL, 0, 0);
    ถ้า (r == NULL)
    {
	nfa_match = NFA_TOO_EXPENSIVE;
	goto theend;
    }

#define	ADD_STATE_IF_MATCH(state)	\
    ถ้า (result)				\
    {					\
	add_state = state->out;		\
	add_off = clen;			\
    }

    /*
     * Run for each character.
     */
    สำหรับ (;;)
    {
	จำนวนเต็ม	curc;
	จำนวนเต็ม	clen;

	ถ้า (has_mbyte)
	{
	    // Fast path for an ASCII byte not followed by a composing
	    // character, matching the check in utfc_ptr2len().  Avoids two
	    // indirect calls for the common case.
	    ถ้า (rex.input[0] != NUL && rex.input[0] < 0x80 && rex.input[1] < 0x80)
	    {
		curc = rex.input[0];
		clen = 1;
	    }
	    มิฉะนั้น
	    {
		curc = (*mb_ptr2char)(rex.input);
		clen = (*mb_ptr2len)(rex.input);
	    }
	}
	มิฉะนั้น
	{
	    curc = *rex.input;
	    clen = 1;
	}
	ถ้า (curc == NUL)
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
	ถ้า (prog->re_engine == AUTOMATIC_ENGINE
		&& (rex.nfa_listid >= NFA_MAX_STATES
#ifdef FEAT_EVAL
		    || nfa_fail_for_testing
#endif
		    ))
	{
	    // too many states, retry with old engine
	    nfa_match = NFA_TOO_EXPENSIVE;
	    goto theend;
	}

	thislist->id = rex.nfa_listid;
	nextlist->id = rex.nfa_listid + 1;

#ifdef ENABLE_LOG
	fprintf(log_fd, "------------------------------------------\n");
	fprintf(log_fd, ">>> Reginput is \"%s\"\n", rex.input);
	fprintf(log_fd, ">>> Advanced one character... Current char is %c (code %d) \n", curc, (จำนวนเต็ม)curc);
	fprintf(log_fd, ">>> Thislist has %d states available: ", thislist->n);
	{
	    จำนวนเต็ม i;

	    สำหรับ (i = 0; i < thislist->n; i++)
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
	ถ้า (thislist->n == 0)
	    bha_nga;

	// compute nextlist
	สำหรับ (listidx = 0; listidx < thislist->n; ++listidx)
	{
	    // If the list gets very long there probably is something wrong.
	    // At least allow interrupting with CTRL-C.  Only check once in a
	    // while, calling ui_breakcheck() for every state is too slow.
	    static จำนวนเต็ม	breakcheck_count = 0;  // using "static" makes it faster
	    ถ้า (unlikely(++breakcheck_count >= 100000))
	    {
		ui_breakcheck();
		breakcheck_count = 0;
	    }
	    ถ้า (got_int)
		bha_nga;
#ifdef FEAT_RELTIME
	    ถ้า (nfa_did_time_out())
		bha_nga;
#endif
	    t = &thislist->t[listidx];

#ifdef NFA_REGEXP_DEBUG_LOG
	    nfa_set_code(t->state->c);
	    fprintf(debug, "%s, ", code);
#endif
#ifdef ENABLE_LOG
	    {
		จำนวนเต็ม col;

		ถ้า (t->subs.norm.in_use <= 0)
		    col = -1;
		มิฉะนั้น ถ้า (REG_MULTI)
		    col = t->subs.norm.list.multi[0].start_col;
		มิฉะนั้น
		    col = (จำนวนเต็ม)(t->subs.norm.list.line[0].start - rex.line);
		nfa_set_code(t->state->c);
		fprintf(log_fd, "(%d) char %d %s (start col %d)%s... \n",
			abs(t->state->id), (จำนวนเต็ม)t->state->c, code, col,
			pim_info(&t->pim));
	    }
#endif

	    /*
	     * Handle the possible codes of the current state.
	     * The most important is NFA_MATCH.
	     */
	    add_state = NULL;
	    add_here = FALSE;
	    add_count = 0;
	    switch (t->state->c)
	    {
	    pak_sa NFA_MATCH:
	      {
		// If the match is not at the start of the line, ends before a
		// composing characters and rex.reg_icombine is not set, that
		// is not really a match.
		ถ้า (enc_utf8 && !rex.reg_icombine
			     && rex.input != rex.line && utf_iscomposing(curc))
		    bha_nga;

		nfa_match = TRUE;
		copy_sub(&submatch->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		ถ้า (rex.nfa_has_zsubexpr)
		    copy_sub(&submatch->synt, &t->subs.synt);
#endif
#ifdef ENABLE_LOG
		log_subsexpr(&t->subs);
#endif
		// Found the left-most longest match, do not look at any other
		// states at this position.  When the list of states is going
		// to be empty quit without advancing, so that "rex.input" is
		// correct.
		ถ้า (nextlist->n == 0)
		    clen = 0;
		goto nextchar;
	      }

	    pak_sa NFA_END_INVISIBLE:
	    pak_sa NFA_END_INVISIBLE_NEG:
	    pak_sa NFA_END_PATTERN:
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
		ถ้า (nfa_endp != NULL)
		{
		    ถ้า (REG_MULTI)
			fprintf(log_fd, "Current lnum: %d, endp lnum: %d; current col: %d, endp col: %d\n",
				(จำนวนเต็ม)rex.lnum,
				(จำนวนเต็ม)nfa_endp->se_u.pos.lnum,
				(จำนวนเต็ม)(rex.input - rex.line),
				nfa_endp->se_u.pos.col);
		    มิฉะนั้น
			fprintf(log_fd, "Current col: %d, endp col: %d\n",
				(จำนวนเต็ม)(rex.input - rex.line),
				(จำนวนเต็ม)(nfa_endp->se_u.ptr - rex.input));
		}
#endif
		// If "nfa_endp" is set it's only a match if it ends at
		// "nfa_endp"
		ถ้า (nfa_endp != NULL && (REG_MULTI
			? (rex.lnum != nfa_endp->se_u.pos.lnum
			    || (จำนวนเต็ม)(rex.input - rex.line)
						!= nfa_endp->se_u.pos.col)
			: rex.input != nfa_endp->se_u.ptr))
		    bha_nga;

		// do not set submatches for \@!
		ถ้า (t->state->c != NFA_END_INVISIBLE_NEG)
		{
		    copy_sub(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		    ถ้า (rex.nfa_has_zsubexpr)
			copy_sub(&m->synt, &t->subs.synt);
#endif
		}
#ifdef ENABLE_LOG
		fprintf(log_fd, "Match found:\n");
		log_subsexpr(m);
#endif
		nfa_match = TRUE;
		// See comment above at "goto nextchar".
		ถ้า (nextlist->n == 0)
		    clen = 0;
		goto nextchar;

	    pak_sa NFA_START_INVISIBLE:
	    pak_sa NFA_START_INVISIBLE_FIRST:
	    pak_sa NFA_START_INVISIBLE_NEG:
	    pak_sa NFA_START_INVISIBLE_NEG_FIRST:
	    pak_sa NFA_START_INVISIBLE_BEFORE:
	    pak_sa NFA_START_INVISIBLE_BEFORE_FIRST:
	    pak_sa NFA_START_INVISIBLE_BEFORE_NEG:
	    pak_sa NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
		{
#ifdef ENABLE_LOG
		    fprintf(log_fd, "Failure chance invisible: %d, what follows: %d\n",
			    failure_chance(t->state->out, 0),
			    failure_chance(t->state->out1->out, 0));
#endif
		    // Do it directly if there already is a PIM or when
		    // nfa_postprocess() detected it will work better.
		    ถ้า (t->pim.result != NFA_PIM_UNUSED
			 || t->state->c == NFA_START_INVISIBLE_FIRST
			 || t->state->c == NFA_START_INVISIBLE_NEG_FIRST
			 || t->state->c == NFA_START_INVISIBLE_BEFORE_FIRST
			 || t->state->c == NFA_START_INVISIBLE_BEFORE_NEG_FIRST)
		    {
			จำนวนเต็ม in_use = m->norm.in_use;

			// Copy submatch info for the recursive call, opposite
			// of what happens on success below.
			copy_sub_off(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
			ถ้า (rex.nfa_has_zsubexpr)
			    copy_sub_off(&m->synt, &t->subs.synt);
#endif

			/*
			 * First try matching the invisible match, then what
			 * follows.
			 */
			result = recursive_regmatch(t->state, NULL, prog,
					  submatch, m, &listids, &listids_len);
			ถ้า (result == NFA_TOO_EXPENSIVE)
			{
			    nfa_match = result;
			    goto theend;
			}

			// for \@! and \@<! it is a match when the result is
			// FALSE
			ถ้า (result != (t->state->c == NFA_START_INVISIBLE_NEG
			       || t->state->c == NFA_START_INVISIBLE_NEG_FIRST
			       || t->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			       || t->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&t->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
			    ถ้า (rex.nfa_has_zsubexpr)
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
		    มิฉะนั้น
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
			ถ้า (REG_MULTI)
			{
			    pim.end.pos.col = (จำนวนเต็ม)(rex.input - rex.line);
			    pim.end.pos.lnum = rex.lnum;
			}
			มิฉะนั้น
			    pim.end.ptr = rex.input;

			// t->state->out1 is the corresponding END_INVISIBLE
			// node; Add its out to the current list (zero-width
			// match).
			ถ้า (addstate_here(thislist, t->state->out1->out,
					     &t->subs, &pim, &listidx) == NULL)
			{
			    nfa_match = NFA_TOO_EXPENSIVE;
			    goto theend;
			}
		    }
		}
		bha_nga;

	    pak_sa NFA_START_PATTERN:
	      {
		nfa_state_T *skip = NULL;
#ifdef ENABLE_LOG
		จำนวนเต็ม	    skip_lid = 0;
#endif

		// There is no point in trying to match the pattern if the
		// output state is not going to be added to the list.
		ถ้า (state_in_list(nextlist, t->state->out1->out, &t->subs))
		{
		    skip = t->state->out1->out;
#ifdef ENABLE_LOG
		    skip_lid = nextlist->id;
#endif
		}
		มิฉะนั้น ถ้า (state_in_list(nextlist,
					  t->state->out1->out->out, &t->subs))
		{
		    skip = t->state->out1->out->out;
#ifdef ENABLE_LOG
		    skip_lid = nextlist->id;
#endif
		}
		มิฉะนั้น ถ้า (state_in_list(thislist,
					  t->state->out1->out->out, &t->subs))
		{
		    skip = t->state->out1->out->out;
#ifdef ENABLE_LOG
		    skip_lid = thislist->id;
#endif
		}
		ถ้า (skip != NULL)
		{
#ifdef ENABLE_LOG
		    nfa_set_code(skip->c);
		    fprintf(log_fd, "> Not trying to match pattern, output state %d is already in list %d. char %d: %s\n",
			    abs(skip->id), skip_lid, skip->c, code);
#endif
		    bha_nga;
		}
		// Copy submatch info to the recursive call, opposite of what
		// happens afterwards.
		copy_sub_off(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		ถ้า (rex.nfa_has_zsubexpr)
		    copy_sub_off(&m->synt, &t->subs.synt);
#endif

		// First try matching the pattern.
		result = recursive_regmatch(t->state, NULL, prog,
					  submatch, m, &listids, &listids_len);
		ถ้า (result == NFA_TOO_EXPENSIVE)
		{
		    nfa_match = result;
		    goto theend;
		}
		ถ้า (result)
		{
		    จำนวนเต็ม bytelen;

#ifdef ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN matches:\n");
		    log_subsexpr(m);
#endif
		    // Copy submatch info from the recursive call
		    copy_sub_off(&t->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
		    ถ้า (rex.nfa_has_zsubexpr)
			copy_sub_off(&t->subs.synt, &m->synt);
#endif
		    // Now we need to skip over the matched text and then
		    // continue with what follows.
		    ถ้า (REG_MULTI)
			// TODO: multi-line match
			bytelen = m->norm.list.multi[0].end_col
						  - (จำนวนเต็ม)(rex.input - rex.line);
		    มิฉะนั้น
			bytelen = (จำนวนเต็ม)(m->norm.list.line[0].end - rex.input);

#ifdef ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN length: %d\n", bytelen);
#endif
		    ถ้า (bytelen == 0)
		    {
			// empty match, output of corresponding
			// NFA_END_PATTERN/NFA_SKIP to be used at current
			// position
			add_here = TRUE;
			add_state = t->state->out1->out->out;
		    }
		    มิฉะนั้น ถ้า (bytelen <= clen)
		    {
			// match current character, output of corresponding
			// NFA_END_PATTERN to be used at next position.
			add_state = t->state->out1->out->out;
			add_off = clen;
		    }
		    มิฉะนั้น
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			add_state = t->state->out1->out;
			add_off = bytelen;
			add_count = bytelen - clen;
		    }
		}
		bha_nga;
	      }

	    pak_sa NFA_BOL:
		ถ้า (rex.input == rex.line)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		bha_nga;

	    pak_sa NFA_EOL:
		ถ้า (curc == NUL)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		bha_nga;

	    pak_sa NFA_BOW:
		result = TRUE;

		ถ้า (curc == NUL)
		    result = FALSE;
		มิฉะนั้น ถ้า (has_mbyte)
		{
		    จำนวนเต็ม this_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.input, rex.reg_buf);
		    ถ้า (this_class <= 1)
			result = FALSE;
		    มิฉะนั้น ถ้า (reg_prev_class() == this_class)
			result = FALSE;
		}
		มิฉะนั้น ถ้า (!vim_iswordc_buf(curc, rex.reg_buf)
			   || (rex.input > rex.line
			       && vim_iswordc_buf(rex.input[-1], rex.reg_buf)))
		    result = FALSE;
		ถ้า (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		bha_nga;

	    pak_sa NFA_EOW:
		result = TRUE;
		ถ้า (rex.input == rex.line)
		    result = FALSE;
		มิฉะนั้น ถ้า (has_mbyte)
		{
		    จำนวนเต็ม this_class, prev_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.input, rex.reg_buf);
		    prev_class = reg_prev_class();
		    ถ้า (this_class == prev_class
					|| prev_class == 0 || prev_class == 1)
			result = FALSE;
		}
		มิฉะนั้น ถ้า (!vim_iswordc_buf(rex.input[-1], rex.reg_buf)
			|| (rex.input[0] != NUL
					&& vim_iswordc_buf(curc, rex.reg_buf)))
		    result = FALSE;
		ถ้า (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		bha_nga;

	    pak_sa NFA_BOF:
		ถ้า (rex.lnum == 0 && rex.input == rex.line
				     && (!REG_MULTI || rex.reg_firstlnum == 1))
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		bha_nga;

	    pak_sa NFA_EOF:
		ถ้า (rex.lnum == rex.reg_maxline && curc == NUL)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		bha_nga;

	    pak_sa NFA_COMPOSING:
	    {
		nfa_state_T *end;

		result = match_composing(t->state->out, curc, clen);

		end = t->state->out1;	    // NFA_END_COMPOSING
		ADD_STATE_IF_MATCH(end);
		bha_nga;
	    }

	    pak_sa NFA_NEWL:
		ถ้า (curc == NUL && !rex.reg_line_lbr && REG_MULTI
						&& rex.lnum <= rex.reg_maxline)
		{
		    go_to_nextline = TRUE;
		    // Pass -1 for the offset, which means taking the position
		    // at the start of the next line.
		    add_state = t->state->out;
		    add_off = -1;
		}
		มิฉะนั้น ถ้า (curc == '\n' && rex.reg_line_lbr)
		{
		    // match \n as if it is an ordinary character
		    add_state = t->state->out;
		    add_off = 1;
		}
		bha_nga;

	    pak_sa NFA_START_COLL:
	    pak_sa NFA_START_NEG_COLL:
	      {
		// What follows is a list of characters, until NFA_END_COLL.
		// One of them must match or none of them must match.
		nfa_state_T	*state;
		จำนวนเต็ม		result_if_matched;
		จำนวนเต็ม		c1, c2;

		// Never match EOL. If it's part of the collection it is added
		// as a separate state with an OR.
		ถ้า (curc == NUL)
		    bha_nga;

		state = t->state->out;
		result_if_matched = (t->state->c == NFA_START_COLL);
		สำหรับ (;;)
		{
		    ถ้า (state->c == NFA_COMPOSING)
		    {
			nfa_state_T *end;

			result = match_composing(t->state->out->out, curc,
									clen);

			ถ้า (t->state->out->out1 != NULL
				&& t->state->out->out1->c == NFA_END_COMPOSING)
			{
			    end = t->state->out->out1;
			    ADD_STATE_IF_MATCH(end);
			}
			bha_nga;
		    }
		    ถ้า (state->c == NFA_END_COLL)
		    {
			result = !result_if_matched;
			bha_nga;
		    }
		    ถ้า (state->c == NFA_RANGE_MIN)
		    {
			c1 = state->val;
			state = state->out; // advance to NFA_RANGE_MAX
			c2 = state->val;
#ifdef ENABLE_LOG
			fprintf(log_fd, "NFA_RANGE_MIN curc=%d c1=%d c2=%d\n",
				curc, c1, c2);
#endif
			ถ้า (curc >= c1 && curc <= c2)
			{
			    result = result_if_matched;
			    bha_nga;
			}
			ถ้า (rex.reg_ic)
			{
			    จำนวนเต็ม curc_low = MB_CASEFOLD(curc);
			    จำนวนเต็ม done = FALSE;

			    สำหรับ ( ; c1 <= c2; ++c1)
				ถ้า (MB_CASEFOLD(c1) == curc_low)
				{
				    result = result_if_matched;
				    done = TRUE;
				    bha_nga;
				}
			    ถ้า (done)
				bha_nga;
			}
		    }
		    มิฉะนั้น ถ้า (state->c < 0 ? check_char_class(state->c, curc)
			       : (curc == state->c
				   || (rex.reg_ic && MB_CASEFOLD(curc)
						    == MB_CASEFOLD(state->c))))
		    {
			result = result_if_matched;
			bha_nga;
		    }
		    state = state->out;
		}
		ถ้า (result)
		{
		    // next state is in out of the NFA_END_COLL, out1 of
		    // START points to the END state
		    add_state = t->state->out1->out;
		    add_off = clen;
		}
		bha_nga;
	      }

	    pak_sa NFA_ANY:
		// Any char except '\0', (end of input) does not match.
		ถ้า (curc > 0)
		{
		    add_state = t->state->out;
		    add_off = clen;
		}
		bha_nga;

	    pak_sa NFA_ANY_COMPOSING:
		// On a composing character skip over it.  Otherwise do
		// nothing.  Always matches.
		ถ้า (enc_utf8 && utf_iscomposing(curc))
		{
		    add_off = clen;
		}
		มิฉะนั้น
		{
		    add_here = TRUE;
		    add_off = 0;
		}
		add_state = t->state->out;
		bha_nga;

	    /*
	     * Character classes like \a for alpha, \d for digit etc.
	     */
	    pak_sa NFA_IDENT:	//  \i
		result = vim_isIDc(curc);
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_SIDENT:	//  \I
		result = !VIM_ISDIGIT(curc) && vim_isIDc(curc);
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_KWORD:	//  \k
		result = vim_iswordp_buf(rex.input, rex.reg_buf);
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_SKWORD:	//  \K
		result = !VIM_ISDIGIT(curc)
				     && vim_iswordp_buf(rex.input, rex.reg_buf);
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_FNAME:	//  \f
		result = vim_isfilec(curc);
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_SFNAME:	//  \F
		result = !VIM_ISDIGIT(curc) && vim_isfilec(curc);
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_PRINT:	//  \p
		result = vim_isprintc(PTR2CHAR(rex.input));
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_SPRINT:	//  \P
		result = !VIM_ISDIGIT(curc) && vim_isprintc(PTR2CHAR(rex.input));
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_WHITE:	//  \s
		result = VIM_ISWHITE(curc);
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_NWHITE:	//  \S
		result = curc != NUL && !VIM_ISWHITE(curc);
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_DIGIT:	//  \d
		result = ri_digit(curc);
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_NDIGIT:	//  \D
		result = curc != NUL && !ri_digit(curc);
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_HEX:	//  \x
		result = ri_hex(curc);
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_NHEX:	//  \X
		result = curc != NUL && !ri_hex(curc);
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_OCTAL:	//  \o
		result = ri_octal(curc);
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_NOCTAL:	//  \O
		result = curc != NUL && !ri_octal(curc);
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_WORD:	//  \w
		result = ri_word(curc);
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_NWORD:	//  \W
		result = curc != NUL && !ri_word(curc);
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_HEAD:	//  \h
		result = ri_head(curc);
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_NHEAD:	//  \H
		result = curc != NUL && !ri_head(curc);
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_ALPHA:	//  \a
		result = ri_alpha(curc);
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_NALPHA:	//  \A
		result = curc != NUL && !ri_alpha(curc);
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_LOWER:	//  \l
		result = ri_lower(curc);
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_NLOWER:	//  \L
		result = curc != NUL && !ri_lower(curc);
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_UPPER:	//  \u
		result = ri_upper(curc);
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_NUPPER:	// \U
		result = curc != NUL && !ri_upper(curc);
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_LOWER_IC:	// [a-z]
		result = ri_lower(curc) || (rex.reg_ic && ri_upper(curc));
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_NLOWER_IC:	// [^a-z]
		result = curc != NUL
			&& !(ri_lower(curc) || (rex.reg_ic && ri_upper(curc)));
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_UPPER_IC:	// [A-Z]
		result = ri_upper(curc) || (rex.reg_ic && ri_lower(curc));
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_NUPPER_IC:	// ^[A-Z]
		result = curc != NUL
			&& !(ri_upper(curc) || (rex.reg_ic && ri_lower(curc)));
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;

	    pak_sa NFA_BACKREF1:
	    pak_sa NFA_BACKREF2:
	    pak_sa NFA_BACKREF3:
	    pak_sa NFA_BACKREF4:
	    pak_sa NFA_BACKREF5:
	    pak_sa NFA_BACKREF6:
	    pak_sa NFA_BACKREF7:
	    pak_sa NFA_BACKREF8:
	    pak_sa NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	    pak_sa NFA_ZREF1:
	    pak_sa NFA_ZREF2:
	    pak_sa NFA_ZREF3:
	    pak_sa NFA_ZREF4:
	    pak_sa NFA_ZREF5:
	    pak_sa NFA_ZREF6:
	    pak_sa NFA_ZREF7:
	    pak_sa NFA_ZREF8:
	    pak_sa NFA_ZREF9:
#endif
		// \1 .. \9  \z1 .. \z9
	      {
		จำนวนเต็ม subidx;
		จำนวนเต็ม bytelen;

#ifdef FEAT_SYN_HL
		ถ้า (t->state->c >= NFA_BACKREF1 && t->state->c <= NFA_BACKREF9)
#endif
		{
		    subidx = t->state->c - NFA_BACKREF1 + 1;
		    result = match_backref(&t->subs.norm, subidx, &bytelen);
		}
#ifdef FEAT_SYN_HL
		มิฉะนั้น
		{
		    subidx = t->state->c - NFA_ZREF1 + 1;
		    result = match_zref(subidx, &bytelen);
		}
#endif

		ถ้า (result)
		{
		    ถ้า (bytelen == 0)
		    {
			// empty match always works, output of NFA_SKIP to be
			// used next
			add_here = TRUE;
			add_state = t->state->out->out;
		    }
		    มิฉะนั้น ถ้า (bytelen <= clen)
		    {
			// match current character, jump ahead to out of
			// NFA_SKIP
			add_state = t->state->out->out;
			add_off = clen;
		    }
		    มิฉะนั้น
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			add_state = t->state->out;
			add_off = bytelen;
			add_count = bytelen - clen;
		    }
		}
		bha_nga;
	      }
	    pak_sa NFA_SKIP:
	      // character of previous matching \1 .. \9  or \@>
	      ถ้า (t->count - clen <= 0)
	      {
		  // end of match, go to what follows
		  add_state = t->state->out;
		  add_off = clen;
	      }
	      มิฉะนั้น
	      {
		  // add state again with decremented count
		  add_state = t->state;
		  add_off = 0;
		  add_count = t->count - clen;
	      }
	      bha_nga;

	    pak_sa NFA_LNUM:
	    pak_sa NFA_LNUM_GT:
	    pak_sa NFA_LNUM_LT:
		result = (REG_MULTI &&
			nfa_re_num_cmp(t->state->val, t->state->c - NFA_LNUM,
			    (long_u)(rex.lnum + rex.reg_firstlnum)));
		ถ้า (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		bha_nga;

	    pak_sa NFA_COL:
	    pak_sa NFA_COL_GT:
	    pak_sa NFA_COL_LT:
		result = nfa_re_num_cmp(t->state->val, t->state->c - NFA_COL,
			(long_u)(rex.input - rex.line) + 1);
		ถ้า (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		bha_nga;

	    pak_sa NFA_VCOL:
	    pak_sa NFA_VCOL_GT:
	    pak_sa NFA_VCOL_LT:
		{
		    จำนวนเต็ม     op = t->state->c - NFA_VCOL;
		    colnr_T col = (colnr_T)(rex.input - rex.line);
		    win_T   *wp = rex.reg_win == NULL ? curwin : rex.reg_win;

		    // Bail out quickly when there can't be a match, avoid the
		    // overhead of win_linetabsize() on long lines.
		    ถ้า (op != 1 && col > t->state->val
			    * (has_mbyte ? MB_MAXBYTES : 1))
			bha_nga;
		    result = FALSE;
		    ถ้า (op == 1 && col - 1 > t->state->val && col > 100)
		    {
			จำนวนเต็ม ts = wp->w_buffer->b_p_ts;

			// Guess that a character won't use more columns than
			// 'tabstop', with a minimum of 4.
			ถ้า (ts < 4)
			    ts = 4;
			result = col > t->state->val * ts;
		    }
		    ถ้า (!result)
		    {
			linenr_T    lnum = REG_MULTI
					    ? rex.reg_firstlnum + rex.lnum : 1;
			long_u	    vcol;

			ถ้า (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_buffer->b_ml.ml_line_count))
			    lnum = 1;
			vcol = (long_u)win_linetabsize(wp, lnum, rex.line, col);
			result = nfa_re_num_cmp(t->state->val, op, vcol + 1);
		    }
		    ถ้า (result)
		    {
			add_here = TRUE;
			add_state = t->state->out;
		    }
		}
		bha_nga;

	    pak_sa NFA_MARK:
	    pak_sa NFA_MARK_GT:
	    pak_sa NFA_MARK_LT:
	      {
		pos_T	*pos;
		size_t	col = REG_MULTI ? rex.input - rex.line : 0;

		pos = getmark_buf(rex.reg_buf, t->state->val, FALSE);

		// Line may have been freed, get it again.
		ถ้า (REG_MULTI)
		{
		    rex.line = reg_getline(rex.lnum);
		    rex.input = rex.line + col;
		}

		// Compare the mark position to the match position, if the mark
		// exists and mark is set in reg_buf.
		ถ้า (pos != NULL && pos->lnum > 0)
		{
		    colnr_T pos_col = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->col == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->col;

		    result = (pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_col == (colnr_T)(rex.input - rex.line)
				    ? t->state->c == NFA_MARK
				    : (pos_col < (colnr_T)(rex.input - rex.line)
					? t->state->c == NFA_MARK_GT
					: t->state->c == NFA_MARK_LT))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? t->state->c == NFA_MARK_GT
				    : t->state->c == NFA_MARK_LT));
		    ถ้า (result)
		    {
			add_here = TRUE;
			add_state = t->state->out;
		    }
		}
		bha_nga;
	      }

	    pak_sa NFA_CURSOR:
		result = (rex.reg_win != NULL
			&& (rex.lnum + rex.reg_firstlnum
						 == rex.reg_win->w_cursor.lnum)
			&& ((colnr_T)(rex.input - rex.line)
						== rex.reg_win->w_cursor.col));
		ถ้า (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		bha_nga;

	    pak_sa NFA_VISUAL:
		result = reg_match_visual();
		ถ้า (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		bha_nga;

	    pak_sa NFA_MOPEN1:
	    pak_sa NFA_MOPEN2:
	    pak_sa NFA_MOPEN3:
	    pak_sa NFA_MOPEN4:
	    pak_sa NFA_MOPEN5:
	    pak_sa NFA_MOPEN6:
	    pak_sa NFA_MOPEN7:
	    pak_sa NFA_MOPEN8:
	    pak_sa NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	    pak_sa NFA_ZOPEN:
	    pak_sa NFA_ZOPEN1:
	    pak_sa NFA_ZOPEN2:
	    pak_sa NFA_ZOPEN3:
	    pak_sa NFA_ZOPEN4:
	    pak_sa NFA_ZOPEN5:
	    pak_sa NFA_ZOPEN6:
	    pak_sa NFA_ZOPEN7:
	    pak_sa NFA_ZOPEN8:
	    pak_sa NFA_ZOPEN9:
#endif
	    pak_sa NFA_NOPEN:
	    pak_sa NFA_ZSTART:
		// These states are only added to be able to bail out when
		// they are added again, nothing is to be done.
		bha_nga;

	    default:	// regular character
	      {
		จำนวนเต็ม c = t->state->c;

#ifdef DEBUG
		ถ้า (c < 0)
		    siemsg("Negative state char: %ld", (long)c);
#endif
		result = (c == curc);

		ถ้า (!result && rex.reg_ic)
		    result = MB_CASEFOLD(c) == MB_CASEFOLD(curc);
		// If rex.reg_icombine is not set only skip over the character
		// itself.  When it is set skip over composing characters.
		ถ้า (result && enc_utf8 && !rex.reg_icombine)
		    clen = utf_ptr2len(rex.input);
		ADD_STATE_IF_MATCH(t->state);
		bha_nga;
	      }

	    } // switch (t->state->c)

	    ถ้า (add_state != NULL)
	    {
		nfa_pim_T *pim;
		nfa_pim_T pim_copy;

		ถ้า (t->pim.result == NFA_PIM_UNUSED)
		    pim = NULL;
		มิฉะนั้น
		    pim = &t->pim;

		// Handle the postponed invisible match if the match might end
		// without advancing and before the end of the line.
		ถ้า (pim != NULL && (clen == 0 || match_follows(add_state, 0)))
		{
		    ถ้า (pim->result == NFA_PIM_TODO)
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
			ถ้า (result != (pim->state->c == NFA_START_INVISIBLE_NEG
			     || pim->state->c == NFA_START_INVISIBLE_NEG_FIRST
			     || pim->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&pim->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
			    ถ้า (rex.nfa_has_zsubexpr)
				copy_sub_off(&pim->subs.synt, &m->synt);
#endif
			}
		    }
		    มิฉะนั้น
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
		    ถ้า (result != (pim->state->c == NFA_START_INVISIBLE_NEG
			     || pim->state->c == NFA_START_INVISIBLE_NEG_FIRST
			     || pim->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
		    {
			// Copy submatch info from the recursive call
			copy_sub_off(&t->subs.norm, &pim->subs.norm);
#ifdef FEAT_SYN_HL
			ถ้า (rex.nfa_has_zsubexpr)
			    copy_sub_off(&t->subs.synt, &pim->subs.synt);
#endif
		    }
		    มิฉะนั้น
			// look-behind match failed, don't add the state
			continue;

		    // Postponed invisible match was handled, don't add it to
		    // following states.
		    pim = NULL;
		}

		// If "pim" points into l->t it will become invalid when
		// adding the state causes the list to be reallocated.  Make a
		// local copy to avoid that.
		ถ้า (pim == &t->pim)
		{
		    copy_pim(&pim_copy, pim);
		    pim = &pim_copy;
		}

		ถ้า (add_here)
		    r = addstate_here(thislist, add_state, &t->subs,
								pim, &listidx);
		มิฉะนั้น
		{
		    r = addstate(nextlist, add_state, &t->subs, pim, add_off, 0);
		    ถ้า (add_count > 0)
			nextlist->t[nextlist->n - 1].count = add_count;
		}
		ถ้า (r == NULL)
		{
		    nfa_match = NFA_TOO_EXPENSIVE;
		    goto theend;
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
	ถ้า (nfa_match == FALSE
		&& ((toplevel
			&& rex.lnum == 0
			&& clen != 0
			&& (rex.reg_maxcol == 0
			  || (colnr_T)(rex.input - rex.line) < rex.reg_maxcol))
		    || (nfa_endp != NULL
			&& (REG_MULTI
			    ? (rex.lnum < nfa_endp->se_u.pos.lnum
			       || (rex.lnum == nfa_endp->se_u.pos.lnum
				   && (จำนวนเต็ม)(rex.input - rex.line)
						    < nfa_endp->se_u.pos.col))
			    : rex.input < nfa_endp->se_u.ptr))))
	{
#ifdef ENABLE_LOG
	    fprintf(log_fd, "(---) STARTSTATE\n");
#endif
	    // Inline optimized code for addstate() if we know the state is
	    // the first MOPEN.
	    ถ้า (toplevel)
	    {
		จำนวนเต็ม add = TRUE;
		จำนวนเต็ม c;

		ถ้า (prog->regstart != NUL && clen != 0)
		{
		    ถ้า (nextlist->n == 0)
		    {
			colnr_T col = (colnr_T)(rex.input - rex.line) + clen;

			// Nextlist is empty, we can skip ahead to the
			// character that must appear at the start.
			ถ้า (skip_to_start(prog->regstart, &col) == FAIL)
			    bha_nga;
#ifdef ENABLE_LOG
			fprintf(log_fd, "  Skipping ahead %d bytes to regstart\n",
				col - ((colnr_T)(rex.input - rex.line) + clen));
#endif
			rex.input = rex.line + col - clen;
		    }
		    มิฉะนั้น
		    {
			// Checking if the required start character matches is
			// cheaper than adding a state that won't match.
			c = PTR2CHAR(rex.input + clen);
			ถ้า (c != prog->regstart && (!rex.reg_ic
			     || MB_CASEFOLD(c) != MB_CASEFOLD(prog->regstart)))
			{
#ifdef ENABLE_LOG
			    fprintf(log_fd, "  Skipping start state, regstart does not match\n");
#endif
			    add = FALSE;
			}
		    }
		}

		ถ้า (add)
		{
		    ถ้า (REG_MULTI)
		    {
			m->norm.list.multi[0].start_col =
					 (colnr_T)(rex.input - rex.line) + clen;
			m->norm.orig_start_col =
					       m->norm.list.multi[0].start_col;
		    }
		    มิฉะนั้น
			m->norm.list.line[0].start = rex.input + clen;
		    ถ้า (addstate(nextlist, start->out, m, NULL, clen, 0) == NULL)
		    {
			nfa_match = NFA_TOO_EXPENSIVE;
			goto theend;
		    }
		}
	    }
	    มิฉะนั้น
	    {
		char_u	    *save_line = rex.line;
		char_u	    *save_input = rex.input;
		linenr_T    save_lnum = rex.lnum;

		// At the end of a line the match can only start on the next
		// line, use that position instead of the line break.
		ถ้า (REG_MULTI && clen == 0 && nfa_endp != NULL
					 && rex.lnum < nfa_endp->se_u.pos.lnum)
		{
		    char_u  *next_line = reg_getline(rex.lnum + 1);

		    ถ้า (next_line != NULL)
		    {
			rex.line = next_line;
			rex.input = rex.line;
			++rex.lnum;
		    }
		}

		r = addstate(nextlist, start, m, NULL, clen, 0);

		rex.line = save_line;
		rex.input = save_input;
		rex.lnum = save_lnum;

		ถ้า (r == NULL)
		{
		    nfa_match = NFA_TOO_EXPENSIVE;
		    goto theend;
		}
	    }
	}

#ifdef ENABLE_LOG
	fprintf(log_fd, ">>> Thislist had %d states available: ", thislist->n);
	{
	    จำนวนเต็ม i;

	    สำหรับ (i = 0; i < thislist->n; i++)
		fprintf(log_fd, "%d  ", abs(thislist->t[i].state->id));
	}
	fprintf(log_fd, "\n");
#endif

nextchar:
	// Advance to the next character, or advance to the next line, or
	// finish.
	ถ้า (clen != 0)
	    rex.input += clen;
	มิฉะนั้น ถ้า (go_to_nextline || (nfa_endp != NULL && REG_MULTI
					&& rex.lnum < nfa_endp->se_u.pos.lnum))
	    reg_nextline();
	มิฉะนั้น
	    bha_nga;

	// Allow interrupting with CTRL-C.
	line_breakcheck();
	ถ้า (got_int)
	    bha_nga;
#ifdef FEAT_RELTIME
	ถ้า (nfa_did_time_out())
	    bha_nga;
#endif
    }

#ifdef ENABLE_LOG
    ถ้า (log_fd != stderr)
	fclose(log_fd);
    log_fd = NULL;
#endif

theend:
    // Cache list buffers in prog for reuse, or free if prog already has
    // cached buffers (recursive call case).
    ถ้า (prog->listbuf[0] == NULL && list[0].t != NULL && list[1].t != NULL)
    {
	prog->listbuf[0] = list[0].t;
	prog->listbuf[1] = list[1].t;
    }
    มิฉะนั้น
    {
	vim_free(list[0].t);
	vim_free(list[1].t);
    }
    vim_free(listids);
#undef ADD_STATE_IF_MATCH
#ifdef NFA_REGEXP_DEBUG_LOG
    fclose(debug);
#endif

    คืน nfa_match;
}

/*
 * Try match of "prog" with at rex.line["col"].
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    static long
nfa_regtry(
    nfa_regprog_T   *prog,
    colnr_T	    col,
    จำนวนเต็ม		    *timed_out UNUSED)	// flag set on timeout or NULL
{
    จำนวนเต็ม		i;
    regsubs_T	subs, m;
    nfa_state_T	*start = prog->start;
    จำนวนเต็ม		result;
#ifdef ENABLE_LOG
    FILE	*f;
#endif

    rex.input = rex.line + col;
#ifdef FEAT_RELTIME
    nfa_timed_out = timed_out;
#endif

#ifdef ENABLE_LOG
    f = fopen(NFA_REGEXP_RUN_LOG, "a");
    ถ้า (f != NULL)
    {
	fprintf(f, "\n\n\t=======================================================\n");
# ifdef DEBUG
	fprintf(f, "\tRegexp is \"%s\"\n", nfa_regengine.expr);
# endif
	fprintf(f, "\tInput text is \"%s\" \n", rex.input);
	fprintf(f, "\t=======================================================\n\n");
	nfa_print_state(f, start);
	fprintf(f, "\n\n");
	fclose(f);
    }
    มิฉะนั้น
	emsg("Could not open temporary log file for writing");
#endif

    clear_sub(&subs.norm);
    clear_sub(&m.norm);
#ifdef FEAT_SYN_HL
    clear_sub(&subs.synt);
    clear_sub(&m.synt);
#endif

    result = nfa_regmatch(prog, start, &subs, &m);
    ถ้า (result == FALSE)
	คืน 0;
    มิฉะนั้น ถ้า (result == NFA_TOO_EXPENSIVE)
	คืน result;

    cleanup_subexpr();
    ถ้า (REG_MULTI)
    {
	สำหรับ (i = 0; i < subs.norm.in_use; i++)
	{
	    rex.reg_startpos[i].lnum = subs.norm.list.multi[i].start_lnum;
	    rex.reg_startpos[i].col = subs.norm.list.multi[i].start_col;

	    rex.reg_endpos[i].lnum = subs.norm.list.multi[i].end_lnum;
	    rex.reg_endpos[i].col = subs.norm.list.multi[i].end_col;
	}
	ถ้า (rex.reg_mmatch != NULL)
	    rex.reg_mmatch->rmm_matchcol = subs.norm.orig_start_col;

	ถ้า (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].col = col;
	}
	ถ้า (rex.reg_endpos[0].lnum < 0)
	{
	    // pattern has a \ze but it didn't match, use current end
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].col = (จำนวนเต็ม)(rex.input - rex.line);
	}
	มิฉะนั้น
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    มิฉะนั้น
    {
	สำหรับ (i = 0; i < subs.norm.in_use; i++)
	{
	    rex.reg_startp[i] = subs.norm.list.line[i].start;
	    rex.reg_endp[i] = subs.norm.list.line[i].end;
	}

	ถ้า (rex.reg_startp[0] == NULL)
	    rex.reg_startp[0] = rex.line + col;
	ถ้า (rex.reg_endp[0] == NULL)
	    rex.reg_endp[0] = rex.input;
    }

#ifdef FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = NULL;

    ถ้า (prog->reghasz == REX_SET)
    {
	cleanup_zsubexpr();
	re_extmatch_out = make_extmatch();
	ถ้า (re_extmatch_out == NULL)
	    คืน 0;
	// Loop over \z1, \z2, etc.  There is no \z0.
	สำหรับ (i = 1; i < subs.synt.in_use; i++)
	{
	    ถ้า (REG_MULTI)
	    {
		sa_nracanaa multipos *mpos = &subs.synt.list.multi[i];

		// Only accept single line matches that are valid.
		ถ้า (mpos->start_lnum >= 0
			&& mpos->start_lnum == mpos->end_lnum
			&& mpos->end_col >= mpos->start_col)
		    re_extmatch_out->matches[i] =
			vim_strnsave(reg_getline(mpos->start_lnum)
							    + mpos->start_col,
					     mpos->end_col - mpos->start_col);
	    }
	    มิฉะนั้น
	    {
		sa_nracanaa linepos *lpos = &subs.synt.list.line[i];

		ถ้า (lpos->start != NULL && lpos->end != NULL)
		    re_extmatch_out->matches[i] =
			    vim_strnsave(lpos->start, lpos->end - lpos->start);
	    }
	}
    }
#endif

    คืน 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 *
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    static long
nfa_regexec_both(
    char_u	*line,
    colnr_T	startcol,	// column to start looking for match
    จำนวนเต็ม		*timed_out)	// flag set on timeout or NULL
{
    nfa_regprog_T   *prog;
    long	    retval = 0L;
    จำนวนเต็ม		    i;
    colnr_T	    col = startcol;

    ถ้า (REG_MULTI)
    {
	prog = (nfa_regprog_T *)rex.reg_mmatch->regprog;
	line = reg_getline((linenr_T)0);    // relative to the cursor
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    มิฉะนั้น
    {
	prog = (nfa_regprog_T *)rex.reg_match->regprog;
	rex.reg_startp = rex.reg_match->startp;
	rex.reg_endp = rex.reg_match->endp;
    }

    // Be paranoid...
    ถ้า (prog == NULL || line == NULL)
    {
	iemsg(e_null_argument);
	goto theend;
    }

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    ถ้า (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    มิฉะนั้น ถ้า (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    ถ้า (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    rex.line = line;
    rex.lnum = 0;    // relative to line

    rex.nfa_has_zend = prog->has_zend;
    rex.nfa_has_backref = prog->has_backref;
    rex.nfa_nsubexpr = prog->nsubexp;
    rex.nfa_listid = 1;
    rex.nfa_alt_listid = 2;
#ifdef DEBUG
    nfa_regengine.expr = prog->pattern;
#endif

    ถ้า (prog->reganch && col > 0)
	คืน 0L;

    rex.need_clear_subexpr = TRUE;
#ifdef FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    ถ้า (prog->reghasz == REX_SET)
    {
	rex.nfa_has_zsubexpr = TRUE;
	rex.need_clear_zsubexpr = TRUE;
    }
    มิฉะนั้น
    {
	rex.nfa_has_zsubexpr = FALSE;
	rex.need_clear_zsubexpr = FALSE;
    }
#endif

    ถ้า (prog->regstart != NUL)
    {
	// Skip ahead until a character we know the match must start with.
	// When there is none there is no match.
	ถ้า (skip_to_start(prog->regstart, &col) == FAIL)
	    คืน 0L;

	// If match_text is set it contains the full text that must match.
	// Nothing else to try. Doesn't handle combining chars well.
	ถ้า (prog->match_text != NULL && *prog->match_text != NUL && !rex.reg_icombine)
	{
	    retval = find_match_text(&col, prog->regstart, prog->match_text);
	    ถ้า (REG_MULTI)
		rex.reg_mmatch->rmm_matchcol = col;
	    มิฉะนั้น
		rex.reg_match->rm_matchcol = col;
	    คืน retval;
	}
    }

    // If the start column is past the maximum column: no need to try.
    ถ้า (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	goto theend;

    // Set the "nstate" used by nfa_regcomp() to zero to trigger an error when
    // it's accidentally used during execution.
    nstate = 0;
    สำหรับ (i = 0; i < prog->nstate; ++i)
    {
	prog->state[i].id = i;
	prog->state[i].lastlist[0] = 0;
	prog->state[i].lastlist[1] = 0;
    }

    retval = nfa_regtry(prog, col, timed_out);

#ifdef DEBUG
    nfa_regengine.expr = NULL;
#endif

theend:
    ถ้า (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs and
	// \ze are used.
	ถ้า (REG_MULTI)
	{
	    lpos_T *start = &rex.reg_mmatch->startpos[0];
	    lpos_T *end = &rex.reg_mmatch->endpos[0];

	    ถ้า (end->lnum < start->lnum
			|| (end->lnum == start->lnum && end->col < start->col))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];
	}
	มิฉะนั้น
	{
	    ถ้า (rex.reg_match->endp[0] < rex.reg_match->startp[0])
		rex.reg_match->endp[0] = rex.reg_match->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_match->rm_matchcol = col;
	}
    }

    คืน retval;
}

/*
 * Compile a regular expression into internal code for the NFA matcher.
 * Returns the program in allocated space.  Returns NULL for an error.
 */
    static regprog_T *
nfa_regcomp(char_u *expr, จำนวนเต็ม re_flags)
{
    nfa_regprog_T	*prog = NULL;
    size_t		prog_size;
    จำนวนเต็ม			*postfix;

    ถ้า (expr == NULL)
	คืน NULL;

#ifdef DEBUG
    nfa_regengine.expr = expr;
#endif
    nfa_re_flags = re_flags;

    init_class_tab();

    ถ้า (nfa_regcomp_start(expr, re_flags) == FAIL)
	คืน NULL;

    // Build postfix form of the regexp. Needed to build the NFA
    // (and count its size).
    postfix = re2post();
    ถ้า (postfix == NULL)
	goto fail;	    // Cascaded (syntax?) error

    /*
     * In order to build the NFA, we parse the input regexp twice:
     * 1. first pass to count size (so we can allocate space)
     * 2. second to emit code
     */
#ifdef ENABLE_LOG
    {
	FILE *f = fopen(NFA_REGEXP_RUN_LOG, "a");

	ถ้า (f != NULL)
	{
	    fprintf(f, "\n*****************************\n\n\n\n\tCompiling regexp \"%s\"... hold on !\n", expr);
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
    prog_size = offsetof(nfa_regprog_T, state) + parimaa_na(nfa_state_T) * nstate;
    prog = alloc(prog_size);
    ถ้า (prog == NULL)
	goto fail;
    state_ptr = prog->state;
    prog->re_in_use = FALSE;
    prog->listbuf[0] = NULL;
    prog->listbuf[1] = NULL;

    /*
     * PASS 2
     * Build the NFA
     */
    prog->start = post2nfa(postfix, post_ptr, FALSE);
    ถ้า (prog->start == NULL)
	goto fail;

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
    nfa_postfix_dump(expr, OK);
    nfa_dump(prog);
#endif
#ifdef FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    prog->reghasz = re_has_z;
#endif
    prog->pattern = vim_strsave(expr);
#ifdef DEBUG
    nfa_regengine.expr = NULL;
#endif

out:
    // The postfix list and fragment stack are reused by the next compilation
    // (and freed in free_regexp_stuff()), but drop ones that grew large for a
    // big pattern to avoid holding much memory.
    ถ้า (post_start_len > NFA_POSTFIX_KEEP)
    {
	VIM_CLEAR(post_start);
	post_start_len = 0;
    }
    post_ptr = post_end = NULL;
    ถ้า (nfa_stack_len > NFA_STACK_KEEP)
    {
	VIM_CLEAR(nfa_stack);
	nfa_stack_len = 0;
    }
    state_ptr = NULL;
    คืน (regprog_T *)prog;

fail:
    VIM_CLEAR(prog);
#ifdef ENABLE_LOG
    nfa_postfix_dump(expr, FAIL);
#endif
#ifdef DEBUG
    nfa_regengine.expr = NULL;
#endif
    goto out;
}

/*
 * Free a compiled regexp program, returned by nfa_regcomp().
 */
    static void
nfa_regfree(regprog_T *prog)
{
    ถ้า (prog == NULL)
	คืน;

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
    static จำนวนเต็ม
nfa_regexec_nl(
    regmatch_T	*rmp,
    char_u	*line,	// string to match against
    colnr_T	col,	// column to start looking for match
    จำนวนเต็ม		line_lbr)
{
    rex.reg_match = rmp;
    rex.reg_mmatch = NULL;
    rex.reg_maxline = 0;
    rex.reg_line_lbr = line_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = NULL;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;
    คืน nfa_regexec_both(line, col, NULL);
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
    static long
nfa_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	col,		// column to start looking for match
    จำนวนเต็ม		*timed_out)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    คืน nfa_regexec_both(NULL, col, timed_out);
}

#ifdef DEBUG
# undef ENABLE_LOG
#endif
