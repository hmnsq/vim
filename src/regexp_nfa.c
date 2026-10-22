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

تعداد
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
ساكن ثابت صحيح nfa_classcodes[] = {
    NFA_ANY, NFA_IDENT, NFA_SIDENT, NFA_KWORD,NFA_SKWORD,
    NFA_FNAME, NFA_SFNAME, NFA_PRINT, NFA_SPRINT,
    NFA_WHITE, NFA_NWHITE, NFA_DIGIT, NFA_NDIGIT,
    NFA_HEX, NFA_NHEX, NFA_OCTAL, NFA_NOCTAL,
    NFA_WORD, NFA_NWORD, NFA_HEAD, NFA_NHEAD,
    NFA_ALPHA, NFA_NALPHA, NFA_LOWER, NFA_NLOWER,
    NFA_UPPER, NFA_NUPPER
};

// Variables only used in nfa_regcomp() and descendants.
ساكن صحيح nfa_re_flags; // re_flags passed to nfa_regcomp()
ساكن صحيح *post_start;  // holds the postfix form of r.e.
ساكن صحيح post_start_len;  // size of allocated post_start (in ints)
ساكن صحيح *post_end;
ساكن صحيح *post_ptr;
ساكن صحيح nfa_reg_parse_depth;	// nesting depth in nfa_reg()

// The postfix list (post_start) and fragment stack (nfa_stack) are reused
// across compilations; a buffer that grew past these sizes for a big pattern
// is freed afterwards instead of being kept, to avoid holding much memory.
#define NFA_POSTFIX_KEEP	10000	// number of ints (~40 Kbyte)
#define NFA_STACK_KEEP		4000	// number of Frag_T (~64 Kbyte)

// Set when the pattern should use the NFA engine.
// E.g. [[:upper:]] only allows 8bit characters for BT engine,
// while NFA engine handles multibyte characters correctly.
ساكن صحيح wants_nfa;

ساكن صحيح nstate;	// Number of states in the NFA.
ساكن صحيح istate;	// Index in the state vector, used in alloc_state()

// If not NULL match must end at this position
ساكن save_se_T *nfa_endp = عدم;

// 0 for first call to nfa_regmatch(), 1 for recursive call.
ساكن صحيح nfa_ll_index = 0;

ساكن صحيح realloc_post_list(فراغ);
ساكن صحيح nfa_reg(صحيح paren);
#ifdef DEBUG
ساكن فراغ nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent);
#endif
ساكن صحيح match_follows(nfa_state_T *startstate, صحيح depth);
ساكن صحيح failure_chance(nfa_state_T *state, صحيح depth);

// helper functions used when doing re2post() ... regatom() parsing
#define EMIT(c)	עשה {				\
		    אם (post_ptr >= post_end && realloc_post_list() == FAIL) \
			החזר FAIL;		\
		    *post_ptr++ = c;		\
		} כלעוד (0)

/*
 * Initialize internal variables before NFA compilation.
 * Return OK on success, FAIL otherwise.
 */
    ساكن صحيح
nfa_regcomp_start(
    char_u	*expr,
    صحيح		re_flags)	    // see vim_regcomp()
{
    size_t	postfix_size;
    صحيح		nstate_max;

    nstate = 0;
    istate = 0;
    // A reasonable estimation for maximum size
    nstate_max = (صحيح)(STRLEN(expr) + 1) * 25;

    // Some items blow up in size, such as [A-z].  Add more space for that.
    // When it is still not enough realloc_post_list() will be used.
    nstate_max += 1000;

    // Size for postfix representation of expr.
    postfix_size = حجم(صحيح) * nstate_max;

    // Reuse the postfix buffer across compilations, only growing it when the
    // estimate exceeds the current size; it is freed in free_regexp_stuff().
    אם (post_start == عدم || post_start_len < nstate_max)
    {
	vim_free(post_start);
	post_start = alloc(postfix_size);
	אם (post_start == عدم)
	{
	    post_start_len = 0;
	    החזר FAIL;
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

    החזר OK;
}

/*
 * Figure out if the NFA state list starts with an anchor, must match at start
 * of the line.
 */
    ساكن صحيح
nfa_get_reganch(nfa_state_T *start, صحيح depth)
{
    nfa_state_T *p = start;

    אם (depth > 4)
	החזר 0;

    כלעוד (p != عدم)
    {
	بدل (p->c)
	{
	    حالة NFA_BOL:
	    حالة NFA_BOF:
		החזר 1; // yes!

	    حالة NFA_ZSTART:
	    حالة NFA_ZEND:
	    حالة NFA_CURSOR:
	    حالة NFA_VISUAL:

	    حالة NFA_MOPEN:
	    حالة NFA_MOPEN1:
	    حالة NFA_MOPEN2:
	    حالة NFA_MOPEN3:
	    حالة NFA_MOPEN4:
	    حالة NFA_MOPEN5:
	    حالة NFA_MOPEN6:
	    حالة NFA_MOPEN7:
	    حالة NFA_MOPEN8:
	    حالة NFA_MOPEN9:
	    حالة NFA_NOPEN:
#ifdef FEAT_SYN_HL
	    حالة NFA_ZOPEN:
	    حالة NFA_ZOPEN1:
	    حالة NFA_ZOPEN2:
	    حالة NFA_ZOPEN3:
	    حالة NFA_ZOPEN4:
	    حالة NFA_ZOPEN5:
	    حالة NFA_ZOPEN6:
	    حالة NFA_ZOPEN7:
	    حالة NFA_ZOPEN8:
	    حالة NFA_ZOPEN9:
#endif
		p = p->out;
		اكسر;

	    حالة NFA_SPLIT:
		החזר nfa_get_reganch(p->out, depth + 1)
				       && nfa_get_reganch(p->out1, depth + 1);

	    افتراضي:
		החזר 0; // noooo
	}
    }
    החזר 0;
}

/*
 * Figure out if the NFA state list starts with a character which must match
 * at start of the match.
 */
    ساكن صحيح
nfa_get_regstart(nfa_state_T *start, صحيح depth)
{
    nfa_state_T *p = start;

    אם (depth > 4)
	החזר 0;

    כלעוד (p != عدم)
    {
	بدل (p->c)
	{
	    // all kinds of zero-width matches
	    حالة NFA_BOL:
	    حالة NFA_BOF:
	    حالة NFA_BOW:
	    حالة NFA_EOW:
	    حالة NFA_ZSTART:
	    حالة NFA_ZEND:
	    حالة NFA_CURSOR:
	    حالة NFA_VISUAL:
	    حالة NFA_LNUM:
	    حالة NFA_LNUM_GT:
	    حالة NFA_LNUM_LT:
	    حالة NFA_COL:
	    حالة NFA_COL_GT:
	    حالة NFA_COL_LT:
	    حالة NFA_VCOL:
	    حالة NFA_VCOL_GT:
	    حالة NFA_VCOL_LT:
	    حالة NFA_MARK:
	    حالة NFA_MARK_GT:
	    حالة NFA_MARK_LT:

	    حالة NFA_MOPEN:
	    حالة NFA_MOPEN1:
	    حالة NFA_MOPEN2:
	    حالة NFA_MOPEN3:
	    حالة NFA_MOPEN4:
	    حالة NFA_MOPEN5:
	    حالة NFA_MOPEN6:
	    حالة NFA_MOPEN7:
	    حالة NFA_MOPEN8:
	    حالة NFA_MOPEN9:
	    حالة NFA_NOPEN:
#ifdef FEAT_SYN_HL
	    حالة NFA_ZOPEN:
	    حالة NFA_ZOPEN1:
	    حالة NFA_ZOPEN2:
	    حالة NFA_ZOPEN3:
	    حالة NFA_ZOPEN4:
	    حالة NFA_ZOPEN5:
	    حالة NFA_ZOPEN6:
	    حالة NFA_ZOPEN7:
	    حالة NFA_ZOPEN8:
	    حالة NFA_ZOPEN9:
#endif
		p = p->out;
		اكسر;

	    حالة NFA_SPLIT:
	    {
		صحيح c1 = nfa_get_regstart(p->out, depth + 1);
		صحيح c2 = nfa_get_regstart(p->out1, depth + 1);

		אם (c1 == c2)
		    החזר c1; // yes!
		החזר 0;
	    }

	    افتراضي:
		אם (p->c > 0)
		    החזר p->c; // yes!
		החזר 0;
	}
    }
    החזר 0;
}

/*
 * Figure out if the NFA state list contains just literal text and nothing
 * else.  If so return a string in allocated memory with what must match after
 * regstart.  Otherwise return NULL.
 */
    ساكن char_u *
nfa_get_match_text(nfa_state_T *start)
{
    nfa_state_T *p = start;
    صحيح		len = 0;
    char_u	*ret;
    char_u	*s;

    אם (p->c != NFA_MOPEN)
	החזר عدم; // just in case
    p = p->out;
    כלעוד (p->c > 0)
    {
	len += MB_CHAR2LEN(p->c);
	p = p->out;
    }
    אם (p->c != NFA_MCLOSE || p->out->c != NFA_MATCH)
	החזר عدم;

    ret = alloc(len);
    אם (ret == عدم)
	החזר عدم;

    p = start->out->out; // skip first char, it goes into regstart
    s = ret;
    כלעוד (p->c > 0)
    {
	אם (has_mbyte)
	    s += (*mb_char2bytes)(p->c, s);
	אחרת
	    *s++ = p->c;
	p = p->out;
    }
    *s = NUL;
    החזר ret;
}

/*
 * Allocate more space for post_start.  Called when
 * running above the estimated number of states.
 */
    ساكن صحيح
realloc_post_list(فراغ)
{
    صحيح   nstate_max = (صحيح)(post_end - post_start);
    صحيح   new_max;
    صحيح   *new_start;
    صحيح	  *old_start;

    // For weird patterns the number of states can be very high. Increasing by
    // 50% seems a reasonable compromise between memory use and speed.
    new_max = nstate_max * 3 / 2;
    new_start = ALLOC_MULT(صحيح, new_max);
    אם (new_start == عدم)
	החזר FAIL;
    mch_memmove(new_start, post_start, nstate_max * حجم(صحيح));
    old_start = post_start;
    post_start = new_start;
    post_start_len = new_max;
    post_ptr = new_start + (post_ptr - old_start);
    post_end = post_start + new_max;
    vim_free(old_start);
    החזר OK;
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
    ساكن صحيح
nfa_recognize_char_class(char_u *start, char_u *end, صحيح extra_newl)
{
    صحيح		newl;
    صحيح		config = get_char_class_bits(start, end, &newl);

    אם (config < 0)
	החזר FAIL;

    אם (extra_newl == TRUE)
	newl = TRUE;

    אם (newl == TRUE)
	extra_newl = NFA_ADD_NL;

    بدل (config)
    {
	حالة CLASS_o9:
	    החזר extra_newl + NFA_DIGIT;
	حالة CLASS_not |  CLASS_o9:
	    החזר extra_newl + NFA_NDIGIT;
	حالة CLASS_af | CLASS_AF | CLASS_o9:
	    החזר extra_newl + NFA_HEX;
	حالة CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    החזר extra_newl + NFA_NHEX;
	حالة CLASS_o7:
	    החזר extra_newl + NFA_OCTAL;
	حالة CLASS_not | CLASS_o7:
	    החזר extra_newl + NFA_NOCTAL;
	حالة CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    החזר extra_newl + NFA_WORD;
	حالة CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    החזר extra_newl + NFA_NWORD;
	حالة CLASS_az | CLASS_AZ | CLASS_underscore:
	    החזר extra_newl + NFA_HEAD;
	حالة CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    החזר extra_newl + NFA_NHEAD;
	حالة CLASS_az | CLASS_AZ:
	    החזר extra_newl + NFA_ALPHA;
	حالة CLASS_not | CLASS_az | CLASS_AZ:
	    החזר extra_newl + NFA_NALPHA;
	حالة CLASS_az:
	   החזר extra_newl + NFA_LOWER_IC;
	حالة CLASS_not | CLASS_az:
	    החזר extra_newl + NFA_NLOWER_IC;
	حالة CLASS_AZ:
	    החזר extra_newl + NFA_UPPER_IC;
	حالة CLASS_not | CLASS_AZ:
	    החזר extra_newl + NFA_NUPPER_IC;
    }
    החזר FAIL;
}

/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * Emits bytes in postfix notation: 'a,b,NFA_OR,c,NFA_OR' is
 * equivalent to 'a OR b OR c'
 *
 * NOTE! When changing this function, also update reg_equi_class()
 */
    ساكن صحيح
nfa_emit_equi_class(صحيح c)
{
#define EMIT2(c)    EMIT(c); EMIT(NFA_CONCAT);

    אם (enc_utf8 || STRCMP(p_enc, "latin1") == 0
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
	بدل (c)
	{
	    حالة 'A': حالة A_grave: حالة A_acute: حالة A_circumflex:
	    حالة A_virguilla: حالة A_diaeresis: حالة A_ring:
	    حالة 0x100: حالة 0x102: حالة 0x104: حالة 0x1cd:
	    حالة 0x1de: حالة 0x1e0: حالة 0x1fa: حالة 0x200:
	    حالة 0x202: حالة 0x226: حالة 0x23a: حالة 0x1e00:
	    حالة 0x1ea0: حالة 0x1ea2: حالة 0x1ea4: حالة 0x1ea6:
	    حالة 0x1ea8: حالة 0x1eaa: حالة 0x1eac: حالة 0x1eae:
	    حالة 0x1eb0: حالة 0x1eb2: حالة 0x1eb4: حالة 0x1eb6:
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
		    החזר OK;

	    حالة 'B': حالة 0x181: حالة 0x243: حالة 0x1e02:
	    حالة 0x1e04: حالة 0x1e06:
		    EMIT2('B')
		    EMIT2(0x181) EMIT2(0x243) EMIT2(0x1e02)
		    EMIT2(0x1e04) EMIT2(0x1e06)
		    החזר OK;

	    حالة 'C': حالة C_cedilla: حالة 0x106: حالة 0x108:
	    حالة 0x10a: حالة 0x10c: حالة 0x187: حالة 0x23b:
	    حالة 0x1e08: حالة 0xa792:
		    EMIT2('C') EMIT2(C_cedilla)
		    EMIT2(0x106) EMIT2(0x108) EMIT2(0x10a)
		    EMIT2(0x10c) EMIT2(0x187) EMIT2(0x23b)
		    EMIT2(0x1e08) EMIT2(0xa792)
		    החזר OK;

	    حالة 'D': حالة 0x10e: حالة 0x110: حالة 0x18a:
	    حالة 0x1e0a: حالة 0x1e0c: حالة 0x1e0e: حالة 0x1e10:
	    حالة 0x1e12:
		    EMIT2('D') EMIT2(0x10e) EMIT2(0x110) EMIT2(0x18a)
		    EMIT2(0x1e0a) EMIT2(0x1e0c) EMIT2(0x1e0e)
		    EMIT2(0x1e10) EMIT2(0x1e12)
		    החזר OK;

	    حالة 'E': حالة E_grave: حالة E_acute: حالة E_circumflex:
	    حالة E_diaeresis: حالة 0x112: حالة 0x114: حالة 0x116:
	    حالة 0x118: حالة 0x11a: حالة 0x204: حالة 0x206:
	    حالة 0x228: حالة 0x246: حالة 0x1e14: حالة 0x1e16:
	    حالة 0x1e18: حالة 0x1e1a: حالة 0x1e1c: حالة 0x1eb8:
	    حالة 0x1eba: حالة 0x1ebc: حالة 0x1ebe: حالة 0x1ec0:
	    حالة 0x1ec2: حالة 0x1ec4: حالة 0x1ec6:
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
		    החזר OK;

	    حالة 'F': حالة 0x191: حالة 0x1e1e: حالة 0xa798:
		    EMIT2('F') EMIT2(0x191) EMIT2(0x1e1e) EMIT2(0xa798)
		    החזר OK;

	    حالة 'G': حالة 0x11c: حالة 0x11e: حالة 0x120:
	    حالة 0x122: حالة 0x193: حالة 0x1e4: حالة 0x1e6:
	    حالة 0x1f4: حالة 0x1e20: حالة 0xa7a0:
		    EMIT2('G') EMIT2(0x11c) EMIT2(0x11e) EMIT2(0x120)
		    EMIT2(0x122) EMIT2(0x193) EMIT2(0x1e4)
		    EMIT2(0x1e6) EMIT2(0x1f4) EMIT2(0x1e20)
		    EMIT2(0xa7a0)
		    החזר OK;

	    حالة 'H': حالة 0x124: حالة 0x126: حالة 0x21e:
	    حالة 0x1e22: حالة 0x1e24: حالة 0x1e26: حالة 0x1e28:
	    حالة 0x1e2a: حالة 0x2c67:
		    EMIT2('H') EMIT2(0x124) EMIT2(0x126) EMIT2(0x21e)
		    EMIT2(0x1e22) EMIT2(0x1e24) EMIT2(0x1e26)
		    EMIT2(0x1e28) EMIT2(0x1e2a) EMIT2(0x2c67)
		    החזר OK;

	    حالة 'I': حالة I_grave: حالة I_acute: حالة I_circumflex:
	    حالة I_diaeresis: حالة 0x128: حالة 0x12a: حالة 0x12c:
	    حالة 0x12e: حالة 0x130: حالة 0x197: حالة 0x1cf:
	    حالة 0x208: حالة 0x20a: حالة 0x1e2c: حالة 0x1e2e:
	    حالة 0x1ec8: حالة 0x1eca:
		    EMIT2('I') EMIT2(I_grave) EMIT2(I_acute)
		    EMIT2(I_circumflex) EMIT2(I_diaeresis)
		    EMIT2(0x128) EMIT2(0x12a) EMIT2(0x12c)
		    EMIT2(0x12e) EMIT2(0x130) EMIT2(0x197)
		    EMIT2(0x1cf) EMIT2(0x208) EMIT2(0x20a)
		    EMIT2(0x1e2c) EMIT2(0x1e2e) EMIT2(0x1ec8)
		    EMIT2(0x1eca)
		    החזר OK;

	    حالة 'J': حالة 0x134: حالة 0x248:
		    EMIT2('J') EMIT2(0x134) EMIT2(0x248)
		    החזר OK;

	    حالة 'K': حالة 0x136: حالة 0x198: حالة 0x1e8: حالة 0x1e30:
	    حالة 0x1e32: حالة 0x1e34: حالة 0x2c69: حالة 0xa740:
		    EMIT2('K') EMIT2(0x136) EMIT2(0x198) EMIT2(0x1e8)
		    EMIT2(0x1e30) EMIT2(0x1e32) EMIT2(0x1e34)
		    EMIT2(0x2c69) EMIT2(0xa740)
		    החזר OK;

	    حالة 'L': حالة 0x139: حالة 0x13b: حالة 0x13d:
	    حالة 0x13f: حالة 0x141: حالة 0x23d: حالة 0x1e36:
	    حالة 0x1e38: حالة 0x1e3a: حالة 0x1e3c: حالة 0x2c60:
		    EMIT2('L') EMIT2(0x139) EMIT2(0x13b)
		    EMIT2(0x13d) EMIT2(0x13f) EMIT2(0x141)
		    EMIT2(0x23d) EMIT2(0x1e36) EMIT2(0x1e38)
		    EMIT2(0x1e3a) EMIT2(0x1e3c) EMIT2(0x2c60)
		    החזר OK;

	    حالة 'M': حالة 0x1e3e: حالة 0x1e40: حالة 0x1e42:
		    EMIT2('M') EMIT2(0x1e3e) EMIT2(0x1e40)
		    EMIT2(0x1e42)
		    החזר OK;

	    حالة 'N': حالة N_virguilla:
	    حالة 0x143: حالة 0x145: حالة 0x147: حالة 0x1f8:
	    حالة 0x1e44: حالة 0x1e46: حالة 0x1e48: حالة 0x1e4a:
	    حالة 0xa7a4:
		    EMIT2('N') EMIT2(N_virguilla)
		    EMIT2(0x143) EMIT2(0x145) EMIT2(0x147)
		    EMIT2(0x1f8) EMIT2(0x1e44) EMIT2(0x1e46)
		    EMIT2(0x1e48) EMIT2(0x1e4a) EMIT2(0xa7a4)
		    החזר OK;

	    حالة 'O': حالة O_grave: حالة O_acute: حالة O_circumflex:
	    حالة O_virguilla: حالة O_diaeresis: حالة O_slash:
	    حالة 0x14c: حالة 0x14e: حالة 0x150: حالة 0x19f:
	    حالة 0x1a0: حالة 0x1d1: حالة 0x1ea: حالة 0x1ec:
	    حالة 0x1fe: حالة 0x20c: حالة 0x20e: حالة 0x22a:
	    حالة 0x22c: حالة 0x22e: حالة 0x230: حالة 0x1e4c:
	    حالة 0x1e4e: حالة 0x1e50: حالة 0x1e52: حالة 0x1ecc:
	    حالة 0x1ece: حالة 0x1ed0: حالة 0x1ed2: حالة 0x1ed4:
	    حالة 0x1ed6: حالة 0x1ed8: حالة 0x1eda: حالة 0x1edc:
	    حالة 0x1ede: حالة 0x1ee0: حالة 0x1ee2:
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
		    החזר OK;

	    حالة 'P': حالة 0x1a4: حالة 0x1e54: حالة 0x1e56: حالة 0x2c63:
		    EMIT2('P') EMIT2(0x1a4) EMIT2(0x1e54) EMIT2(0x1e56)
		    EMIT2(0x2c63)
		    החזר OK;

	    حالة 'Q': حالة 0x24a:
		    EMIT2('Q') EMIT2(0x24a)
		    החזר OK;

	    حالة 'R': حالة 0x154: حالة 0x156: حالة 0x158: حالة 0x210:
	    حالة 0x212: حالة 0x24c: حالة 0x1e58: حالة 0x1e5a:
	    حالة 0x1e5c: حالة 0x1e5e: حالة 0x2c64: حالة 0xa7a6:
		    EMIT2('R') EMIT2(0x154) EMIT2(0x156) EMIT2(0x158)
		    EMIT2(0x210) EMIT2(0x212) EMIT2(0x24c) EMIT2(0x1e58)
		    EMIT2(0x1e5a) EMIT2(0x1e5c) EMIT2(0x1e5e) EMIT2(0x2c64)
		    EMIT2(0xa7a6)
		    החזר OK;

	    حالة 'S': حالة 0x15a: حالة 0x15c: حالة 0x15e: حالة 0x160:
	    حالة 0x218: حالة 0x1e60: حالة 0x1e62: حالة 0x1e64:
	    حالة 0x1e66: حالة 0x1e68: حالة 0x2c7e: حالة 0xa7a8:
		    EMIT2('S') EMIT2(0x15a) EMIT2(0x15c) EMIT2(0x15e)
		    EMIT2(0x160) EMIT2(0x218) EMIT2(0x1e60) EMIT2(0x1e62)
		    EMIT2(0x1e64) EMIT2(0x1e66) EMIT2(0x1e68) EMIT2(0x2c7e)
		    EMIT2(0xa7a8)
		    החזר OK;

	    حالة 'T': حالة 0x162: حالة 0x164: حالة 0x166: حالة 0x1ac:
	    حالة 0x1ae: حالة 0x21a: حالة 0x23e: حالة 0x1e6a: حالة 0x1e6c:
	    حالة 0x1e6e: حالة 0x1e70:
		    EMIT2('T') EMIT2(0x162) EMIT2(0x164) EMIT2(0x166)
		    EMIT2(0x1ac) EMIT2(0x1ae) EMIT2(0x23e) EMIT2(0x21a)
		    EMIT2(0x1e6a) EMIT2(0x1e6c) EMIT2(0x1e6e) EMIT2(0x1e70)
		    החזר OK;

	    حالة 'U': حالة U_grave: حالة U_acute: حالة U_diaeresis:
	    حالة U_circumflex: حالة 0x168: حالة 0x16a: حالة 0x16c:
	    حالة 0x16e: حالة 0x170: حالة 0x172: حالة 0x1af:
	    حالة 0x1d3: حالة 0x1d5: حالة 0x1d7: حالة 0x1d9:
	    حالة 0x1db: حالة 0x214: حالة 0x216: حالة 0x244:
	    حالة 0x1e72: حالة 0x1e74: حالة 0x1e76: حالة 0x1e78:
	    حالة 0x1e7a: حالة 0x1ee4: حالة 0x1ee6: حالة 0x1ee8:
	    حالة 0x1eea: حالة 0x1eec: حالة 0x1eee: حالة 0x1ef0:
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
		    החזר OK;

	    حالة 'V': حالة 0x1b2: حالة 0x1e7c: حالة 0x1e7e:
		    EMIT2('V') EMIT2(0x1b2) EMIT2(0x1e7c) EMIT2(0x1e7e)
		    החזר OK;

	    حالة 'W': حالة 0x174: حالة 0x1e80: حالة 0x1e82: حالة 0x1e84:
	    حالة 0x1e86: حالة 0x1e88:
		    EMIT2('W') EMIT2(0x174) EMIT2(0x1e80) EMIT2(0x1e82)
		    EMIT2(0x1e84) EMIT2(0x1e86) EMIT2(0x1e88)
		    החזר OK;

	    حالة 'X': حالة 0x1e8a: حالة 0x1e8c:
		    EMIT2('X') EMIT2(0x1e8a) EMIT2(0x1e8c)
		    החזר OK;

	    حالة 'Y': حالة Y_acute: حالة 0x176: حالة 0x178:
	    حالة 0x1b3: حالة 0x232: حالة 0x24e: حالة 0x1e8e:
	    حالة 0x1ef2: حالة 0x1ef4: حالة 0x1ef6: حالة 0x1ef8:
		    EMIT2('Y') EMIT2(Y_acute)
		    EMIT2(0x176) EMIT2(0x178) EMIT2(0x1b3)
		    EMIT2(0x232) EMIT2(0x24e) EMIT2(0x1e8e)
		    EMIT2(0x1ef2) EMIT2(0x1ef4) EMIT2(0x1ef6)
		    EMIT2(0x1ef8)
		    החזר OK;

	    حالة 'Z': حالة 0x179: حالة 0x17b: حالة 0x17d:
	    حالة 0x1b5: حالة 0x1e90: حالة 0x1e92: حالة 0x1e94:
	    حالة 0x2c6b:
		    EMIT2('Z') EMIT2(0x179) EMIT2(0x17b) EMIT2(0x17d)
		    EMIT2(0x1b5) EMIT2(0x1e90) EMIT2(0x1e92)
		    EMIT2(0x1e94) EMIT2(0x2c6b)
		    החזר OK;

	    حالة 'a': حالة a_grave: حالة a_acute: حالة a_circumflex:
	    حالة a_virguilla: حالة a_diaeresis: حالة a_ring:
	    حالة 0x101: حالة 0x103: حالة 0x105: حالة 0x1ce:
	    حالة 0x1df: حالة 0x1e1: حالة 0x1fb: حالة 0x201:
	    حالة 0x203: حالة 0x227: حالة 0x1d8f: حالة 0x1e01:
	    حالة 0x1e9a: حالة 0x1ea1: حالة 0x1ea3: حالة 0x1ea5:
	    حالة 0x1ea7: حالة 0x1ea9: حالة 0x1eab: حالة 0x1ead:
	    حالة 0x1eaf: حالة 0x1eb1: حالة 0x1eb3: حالة 0x1eb5:
	    حالة 0x1eb7: حالة 0x2c65:
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
		    החזר OK;

	    حالة 'b': حالة 0x180: حالة 0x253: حالة 0x1d6c: حالة 0x1d80:
	    حالة 0x1e03: حالة 0x1e05: حالة 0x1e07:
		    EMIT2('b') EMIT2(0x180) EMIT2(0x253) EMIT2(0x1d6c)
		    EMIT2(0x1d80) EMIT2(0x1e03) EMIT2(0x1e05) EMIT2(0x1e07)
		    החזר OK;

	    حالة 'c': حالة c_cedilla: حالة 0x107: حالة 0x109: حالة 0x10b:
	    حالة 0x10d: حالة 0x188: حالة 0x23c: حالة 0x1e09: حالة 0xa793:
	    حالة 0xa794:
		    EMIT2('c') EMIT2(c_cedilla)
		    EMIT2(0x107) EMIT2(0x109) EMIT2(0x10b)
		    EMIT2(0x10d) EMIT2(0x188) EMIT2(0x23c)
		    EMIT2(0x1e09) EMIT2(0xa793) EMIT2(0xa794)
		    החזר OK;

	    حالة 'd': حالة 0x10f: حالة 0x111: حالة 0x257: حالة 0x1d6d:
	    حالة 0x1d81: حالة 0x1d91: حالة 0x1e0b: حالة 0x1e0d: حالة 0x1e0f:
	    حالة 0x1e11: حالة 0x1e13:
		    EMIT2('d') EMIT2(0x10f) EMIT2(0x111)
		    EMIT2(0x257) EMIT2(0x1d6d) EMIT2(0x1d81)
		    EMIT2(0x1d91) EMIT2(0x1e0b) EMIT2(0x1e0d)
		    EMIT2(0x1e0f) EMIT2(0x1e11) EMIT2(0x1e13)
		    החזר OK;

	    حالة 'e': حالة e_grave: حالة e_acute: حالة e_circumflex:
	    حالة e_diaeresis: حالة 0x113: حالة 0x115: حالة 0x117:
	    حالة 0x119: حالة 0x11b: حالة 0x205: حالة 0x207:
	    حالة 0x229: حالة 0x247: حالة 0x1d92: حالة 0x1e15:
	    حالة 0x1e17: حالة 0x1e19: حالة 0x1e1b: حالة 0x1e1d:
	    حالة 0x1eb9: حالة 0x1ebb: حالة 0x1ebd: حالة 0x1ebf:
	    حالة 0x1ec1: حالة 0x1ec3: حالة 0x1ec5: حالة 0x1ec7:
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
		    החזר OK;

	    حالة 'f': حالة 0x192: حالة 0x1d6e: حالة 0x1d82:
	    حالة 0x1e1f: حالة 0xa799:
		    EMIT2('f') EMIT2(0x192) EMIT2(0x1d6e) EMIT2(0x1d82)
		    EMIT2(0x1e1f) EMIT2(0xa799)
		    החזר OK;

	    حالة 'g': حالة 0x11d: حالة 0x11f: حالة 0x121: حالة 0x123:
	    حالة 0x1e5: حالة 0x1e7: حالة 0x1f5: حالة 0x260: حالة 0x1d83:
	    حالة 0x1e21: حالة 0xa7a1:
		    EMIT2('g') EMIT2(0x11d) EMIT2(0x11f) EMIT2(0x121)
		    EMIT2(0x123) EMIT2(0x1e5) EMIT2(0x1e7)
		    EMIT2(0x1f5) EMIT2(0x260) EMIT2(0x1d83)
		    EMIT2(0x1e21) EMIT2(0xa7a1)
		    החזר OK;

	    حالة 'h': حالة 0x125: حالة 0x127: حالة 0x21f: حالة 0x1e23:
	    حالة 0x1e25: حالة 0x1e27: حالة 0x1e29: حالة 0x1e2b:
	    حالة 0x1e96: حالة 0x2c68: حالة 0xa795:
		    EMIT2('h') EMIT2(0x125) EMIT2(0x127) EMIT2(0x21f)
		    EMIT2(0x1e23) EMIT2(0x1e25) EMIT2(0x1e27)
		    EMIT2(0x1e29) EMIT2(0x1e2b) EMIT2(0x1e96)
		    EMIT2(0x2c68) EMIT2(0xa795)
		    החזר OK;

	    حالة 'i': حالة i_grave: حالة i_acute: حالة i_circumflex:
	    حالة i_diaeresis: حالة 0x129: حالة 0x12b: حالة 0x12d:
	    حالة 0x12f: حالة 0x1d0: حالة 0x209: حالة 0x20b:
	    حالة 0x268: حالة 0x1d96: حالة 0x1e2d: حالة 0x1e2f:
	    حالة 0x1ec9: حالة 0x1ecb:
		    EMIT2('i') EMIT2(i_grave) EMIT2(i_acute)
		    EMIT2(i_circumflex) EMIT2(i_diaeresis)
		    EMIT2(0x129) EMIT2(0x12b) EMIT2(0x12d)
		    EMIT2(0x12f) EMIT2(0x1d0) EMIT2(0x209)
		    EMIT2(0x20b) EMIT2(0x268) EMIT2(0x1d96)
		    EMIT2(0x1e2d) EMIT2(0x1e2f) EMIT2(0x1ec9)
		    EMIT2(0x1ecb)
		    החזר OK;

	    حالة 'j': حالة 0x135: حالة 0x1f0: حالة 0x249:
		    EMIT2('j') EMIT2(0x135) EMIT2(0x1f0) EMIT2(0x249)
		    החזר OK;

	    حالة 'k': حالة 0x137: حالة 0x199: حالة 0x1e9: حالة 0x1d84:
	    حالة 0x1e31: حالة 0x1e33: حالة 0x1e35: حالة 0x2c6a: حالة 0xa741:
		    EMIT2('k') EMIT2(0x137) EMIT2(0x199) EMIT2(0x1e9)
		    EMIT2(0x1d84) EMIT2(0x1e31) EMIT2(0x1e33)
		    EMIT2(0x1e35) EMIT2(0x2c6a) EMIT2(0xa741)
		    החזר OK;

	    حالة 'l': حالة 0x13a: حالة 0x13c: حالة 0x13e: حالة 0x140:
	    حالة 0x142: حالة 0x19a: حالة 0x1e37: حالة 0x1e39: حالة 0x1e3b:
	    حالة 0x1e3d: حالة 0x2c61:
		    EMIT2('l') EMIT2(0x13a) EMIT2(0x13c)
		    EMIT2(0x13e) EMIT2(0x140) EMIT2(0x142)
		    EMIT2(0x19a) EMIT2(0x1e37) EMIT2(0x1e39)
		    EMIT2(0x1e3b) EMIT2(0x1e3d) EMIT2(0x2c61)
		    החזר OK;

	    حالة 'm': حالة 0x1d6f: حالة 0x1e3f: حالة 0x1e41: حالة 0x1e43:
		    EMIT2('m') EMIT2(0x1d6f) EMIT2(0x1e3f)
		    EMIT2(0x1e41) EMIT2(0x1e43)
		    החזר OK;

	    حالة 'n': حالة n_virguilla: حالة 0x144: حالة 0x146: حالة 0x148:
	    حالة 0x149: حالة 0x1f9: حالة 0x1d70: حالة 0x1d87: حالة 0x1e45:
	    حالة 0x1e47: حالة 0x1e49: حالة 0x1e4b: حالة 0xa7a5:
		    EMIT2('n') EMIT2(n_virguilla)
		    EMIT2(0x144) EMIT2(0x146) EMIT2(0x148)
		    EMIT2(0x149) EMIT2(0x1f9) EMIT2(0x1d70)
		    EMIT2(0x1d87) EMIT2(0x1e45) EMIT2(0x1e47)
		    EMIT2(0x1e49) EMIT2(0x1e4b) EMIT2(0xa7a5)
		    החזר OK;

	    حالة 'o': حالة o_grave: حالة o_acute: حالة o_circumflex:
	    حالة o_virguilla: حالة o_diaeresis: حالة o_slash:
	    حالة 0x14d: حالة 0x14f: حالة 0x151: حالة 0x1a1:
	    حالة 0x1d2: حالة 0x1eb: حالة 0x1ed: حالة 0x1ff:
	    حالة 0x20d: حالة 0x20f: حالة 0x22b: حالة 0x22d:
	    حالة 0x22f: حالة 0x231: حالة 0x275: حالة 0x1e4d:
	    حالة 0x1e4f: حالة 0x1e51: حالة 0x1e53: حالة 0x1ecd:
	    حالة 0x1ecf: حالة 0x1ed1: حالة 0x1ed3: حالة 0x1ed5:
	    حالة 0x1ed7: حالة 0x1ed9: حالة 0x1edb: حالة 0x1edd:
	    حالة 0x1edf: حالة 0x1ee1: حالة 0x1ee3:
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
		    החזר OK;

	    حالة 'p': حالة 0x1a5: حالة 0x1d71: حالة 0x1d7d: حالة 0x1d88:
	    حالة 0x1e55: حالة 0x1e57:
		    EMIT2('p') EMIT2(0x1a5) EMIT2(0x1d71) EMIT2(0x1d7d)
		    EMIT2(0x1d88) EMIT2(0x1e55) EMIT2(0x1e57)
		    החזר OK;

	    حالة 'q': حالة 0x24b: حالة 0x2a0:
		    EMIT2('q') EMIT2(0x24b) EMIT2(0x2a0)
		    החזר OK;

	    حالة 'r': حالة 0x155: حالة 0x157: حالة 0x159: حالة 0x211:
	    حالة 0x213: حالة 0x24d: حالة 0x27d: حالة 0x1d72: حالة 0x1d73:
	    حالة 0x1d89: حالة 0x1e59: حالة 0x1e5b: حالة 0x1e5d: حالة 0x1e5f:
	    حالة 0xa7a7:
		    EMIT2('r') EMIT2(0x155) EMIT2(0x157) EMIT2(0x159)
		    EMIT2(0x211) EMIT2(0x213) EMIT2(0x24d) EMIT2(0x27d)
		    EMIT2(0x1d72) EMIT2(0x1d73) EMIT2(0x1d89) EMIT2(0x1e59)
		    EMIT2(0x1e5b) EMIT2(0x1e5d) EMIT2(0x1e5f) EMIT2(0xa7a7)
		    החזר OK;

	    حالة 's': حالة 0x15b: حالة 0x15d: حالة 0x15f: حالة 0x161:
	    حالة 0x219: حالة 0x23f: حالة 0x1d74: حالة 0x1d8a: حالة 0x1e61:
	    حالة 0x1e63: حالة 0x1e65: حالة 0x1e67: حالة 0x1e69: حالة 0xa7a9:
		    EMIT2('s') EMIT2(0x15b) EMIT2(0x15d) EMIT2(0x15f)
		    EMIT2(0x161) EMIT2(0x219) EMIT2(0x23f) EMIT2(0x1d74)
		    EMIT2(0x1d8a) EMIT2(0x1e61) EMIT2(0x1e63) EMIT2(0x1e65)
		    EMIT2(0x1e67) EMIT2(0x1e69) EMIT2(0xa7a9)
		    החזר OK;

	    حالة 't': حالة 0x163: حالة 0x165: حالة 0x167: حالة 0x1ab:
	    حالة 0x1ad: حالة 0x21b: حالة 0x288: حالة 0x1d75: حالة 0x1e6b:
	    حالة 0x1e6d: حالة 0x1e6f: حالة 0x1e71: حالة 0x1e97: حالة 0x2c66:
		    EMIT2('t') EMIT2(0x163) EMIT2(0x165) EMIT2(0x167)
		    EMIT2(0x1ab) EMIT2(0x1ad) EMIT2(0x21b) EMIT2(0x288)
		    EMIT2(0x1d75) EMIT2(0x1e6b) EMIT2(0x1e6d) EMIT2(0x1e6f)
		    EMIT2(0x1e71) EMIT2(0x1e97) EMIT2(0x2c66)
		    החזר OK;

	    حالة 'u': حالة u_grave: حالة u_acute: حالة u_circumflex:
	    حالة u_diaeresis: حالة 0x169: حالة 0x16b: حالة 0x16d:
	    حالة 0x16f: حالة 0x171: حالة 0x173: حالة 0x1b0: حالة 0x1d4:
	    حالة 0x1d6: حالة 0x1d8: حالة 0x1da: حالة 0x1dc: حالة 0x215:
	    حالة 0x217: حالة 0x289: حالة 0x1d7e: حالة 0x1d99: حالة 0x1e73:
	    حالة 0x1e75: حالة 0x1e77: حالة 0x1e79: حالة 0x1e7b:
	    حالة 0x1ee5: حالة 0x1ee7: حالة 0x1ee9: حالة 0x1eeb:
	    حالة 0x1eed: حالة 0x1eef: حالة 0x1ef1:
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
		    החזר OK;

	    حالة 'v': حالة 0x28b: حالة 0x1d8c: حالة 0x1e7d: حالة 0x1e7f:
		    EMIT2('v') EMIT2(0x28b) EMIT2(0x1d8c) EMIT2(0x1e7d)
		    EMIT2(0x1e7f)
		    החזר OK;

	    حالة 'w': حالة 0x175: حالة 0x1e81: حالة 0x1e83: حالة 0x1e85:
	    حالة 0x1e87: حالة 0x1e89: حالة 0x1e98:
		    EMIT2('w') EMIT2(0x175) EMIT2(0x1e81) EMIT2(0x1e83)
		    EMIT2(0x1e85) EMIT2(0x1e87) EMIT2(0x1e89) EMIT2(0x1e98)
		    החזר OK;

	    حالة 'x': حالة 0x1e8b: حالة 0x1e8d:
		    EMIT2('x') EMIT2(0x1e8b) EMIT2(0x1e8d)
		    החזר OK;

	    حالة 'y': حالة y_acute: حالة y_diaeresis: حالة 0x177:
	    حالة 0x1b4: حالة 0x233: حالة 0x24f: حالة 0x1e8f:
	    حالة 0x1e99: حالة 0x1ef3: حالة 0x1ef5: حالة 0x1ef7:
	    حالة 0x1ef9:
		    EMIT2('y') EMIT2(y_acute) EMIT2(y_diaeresis)
		    EMIT2(0x177) EMIT2(0x1b4) EMIT2(0x233) EMIT2(0x24f)
		    EMIT2(0x1e8f) EMIT2(0x1e99) EMIT2(0x1ef3)
		    EMIT2(0x1ef5) EMIT2(0x1ef7) EMIT2(0x1ef9)
		    החזר OK;

	    حالة 'z': حالة 0x17a: حالة 0x17c: حالة 0x17e: حالة 0x1b6:
	    حالة 0x1d76: حالة 0x1d8e: حالة 0x1e91: حالة 0x1e93:
	    حالة 0x1e95: حالة 0x2c6c:
		    EMIT2('z') EMIT2(0x17a) EMIT2(0x17c) EMIT2(0x17e)
		    EMIT2(0x1b6) EMIT2(0x1d76) EMIT2(0x1d8e) EMIT2(0x1e91)
		    EMIT2(0x1e93) EMIT2(0x1e95) EMIT2(0x2c6c)
		    החזר OK;

	    // default: character itself
	}
    }

    EMIT2(c);
    החזר OK;
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
    ساكن صحيح
nfa_regatom(فراغ)
{
    صحيح		c;
    صحيح		charclass;
    صحيح		equiclass;
    صحيح		collclass;
    صحيح		got_coll_char;
    char_u	*p;
    char_u	*endp;
    char_u	*old_regparse = regparse;
    صحيح		extra = 0;
    صحيح		emit_range;
    صحيح		negated;
    صحيح		result;
    صحيح		startc = -1;
    صحيح		save_prev_at_start = prev_at_start;

    c = getchr();
    بدل (c)
    {
	حالة NUL:
	    EMSG_RET_FAIL(_(e_nfa_regexp_end_encountered_prematurely));

	حالة Magic('^'):
	    EMIT(NFA_BOL);
	    اكسر;

	حالة Magic('$'):
	    EMIT(NFA_EOL);
#אם defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#endif
	    اكسر;

	حالة Magic('<'):
	    EMIT(NFA_BOW);
	    اكسر;

	حالة Magic('>'):
	    EMIT(NFA_EOW);
	    اكسر;

	حالة Magic('_'):
	    c = no_Magic(getchr());
	    אם (c == NUL)
		EMSG_RET_FAIL(_(e_nfa_regexp_end_encountered_prematurely));

	    אם (c == '^')	// "\_^" is start-of-line
	    {
		EMIT(NFA_BOL);
		اكسر;
	    }
	    אם (c == '$')	// "\_$" is end-of-line
	    {
		EMIT(NFA_EOL);
#אם defined(FEAT_SYN_HL)
		had_eol = TRUE;
#endif
		اكسر;
	    }

	    extra = NFA_ADD_NL;

	    // "\_[" is collection plus newline
	    אם (c == '[')
		اذهب collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	/*
	 * Character classes.
	 */
	حالة Magic('.'):
	حالة Magic('i'):
	حالة Magic('I'):
	حالة Magic('k'):
	حالة Magic('K'):
	حالة Magic('f'):
	حالة Magic('F'):
	حالة Magic('p'):
	حالة Magic('P'):
	حالة Magic('s'):
	حالة Magic('S'):
	حالة Magic('d'):
	حالة Magic('D'):
	حالة Magic('x'):
	حالة Magic('X'):
	حالة Magic('o'):
	حالة Magic('O'):
	حالة Magic('w'):
	حالة Magic('W'):
	حالة Magic('h'):
	حالة Magic('H'):
	حالة Magic('a'):
	حالة Magic('A'):
	حالة Magic('l'):
	حالة Magic('L'):
	حالة Magic('u'):
	حالة Magic('U'):
	    p = vim_strchr(classchars, no_Magic(c));
	    אם (p == عدم)
	    {
		אם (extra == NFA_ADD_NL)
		{
		    semsg(_(e_nfa_regexp_invalid_character_class_nr), c);
		    rc_did_emsg = TRUE;
		    החזר FAIL;
		}
		siemsg("Unknown character class char: %d", c);
		החזר FAIL;
	    }

	    // When '.' is followed by a composing char ignore the dot, so that
	    // the composing char is matched here.
	    אם (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	    {
		old_regparse = regparse;
		c = getchr();
		اذهب nfa_do_multibyte;
	    }
	    EMIT(nfa_classcodes[p - classchars]);
	    אם (extra == NFA_ADD_NL)
	    {
		EMIT(NFA_NEWL);
		EMIT(NFA_OR);
		regflags |= RF_HASNL;
	    }
	    اكسر;

	حالة Magic('n'):
	    אם (reg_string)
		// In a string "\n" matches a newline character.
		EMIT(NL);
	    אחרת
	    {
		// In buffer text "\n" matches the end of a line.
		EMIT(NFA_NEWL);
		regflags |= RF_HASNL;
	    }
	    اكسر;

	حالة Magic('('):
	    אם (nfa_reg(REG_PAREN) == FAIL)
		החזר FAIL;	    // cascaded error
	    اكسر;

	حالة Magic('|'):
	حالة Magic('&'):
	حالة Magic(')'):
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    החזר FAIL;

	حالة Magic('='):
	حالة Magic('?'):
	حالة Magic('+'):
	حالة Magic('@'):
	حالة Magic('*'):
	حالة Magic('{'):
	    // these should follow an atom, not form an atom
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    החזר FAIL;

	حالة Magic('~'):
	    {
		char_u	    *lp;

		// Previous substitute pattern.
		// Generated as "\%(pattern\)".
		אם (reg_prev_sub == عدم)
		{
		    emsg(_(e_no_previous_substitute_regular_expression));
		    החזר FAIL;
		}
		עבור (lp = reg_prev_sub; *lp != NUL; MB_CPTR_ADV(lp))
		{
		    EMIT(PTR2CHAR(lp));
		    אם (lp != reg_prev_sub)
			EMIT(NFA_CONCAT);
		}
		EMIT(NFA_NOPEN);
		اكسر;
	    }

	حالة Magic('1'):
	حالة Magic('2'):
	حالة Magic('3'):
	حالة Magic('4'):
	حالة Magic('5'):
	حالة Magic('6'):
	حالة Magic('7'):
	حالة Magic('8'):
	حالة Magic('9'):
	    {
		صحيح refnum = no_Magic(c) - '1';

		אם (!seen_endbrace(refnum + 1))
		    החזר FAIL;
		EMIT(NFA_BACKREF1 + refnum);
		rex.nfa_has_backref = TRUE;
	    }
	    اكسر;

	حالة Magic('z'):
	    c = no_Magic(getchr());
	    بدل (c)
	    {
		حالة 's':
		    EMIT(NFA_ZSTART);
		    אם (re_mult_next("\\zs") == FAIL)
			החזר FAIL;
		    اكسر;
		حالة 'e':
		    EMIT(NFA_ZEND);
		    rex.nfa_has_zend = TRUE;
		    אם (re_mult_next("\\ze") == FAIL)
			החזר FAIL;
		    اكسر;
#ifdef FEAT_SYN_HL
		حالة '1':
		حالة '2':
		حالة '3':
		حالة '4':
		حالة '5':
		حالة '6':
		حالة '7':
		حالة '8':
		حالة '9':
		    // \z1...\z9
		    אם ((reg_do_extmatch & REX_USE) == 0)
			EMSG_RET_FAIL(_(e_z1_z9_not_allowed_here));
		    EMIT(NFA_ZREF1 + (no_Magic(c) - '1'));
		    // No need to set rex.nfa_has_backref, the sub-matches don't
		    // change when \z1 .. \z9 matches or not.
		    re_has_z = REX_USE;
		    اكسر;
		حالة '(':
		    // \z(
		    אם ((reg_do_extmatch & REX_SET) == 0)
			EMSG_RET_FAIL(_(e_z_not_allowed_here));
		    אם (nfa_reg(REG_ZPAREN) == FAIL)
			החזר FAIL;	    // cascaded error
		    re_has_z = REX_SET;
		    اكسر;
#endif
		افتراضي:
		    semsg(_(e_nfa_regexp_unknown_operator_z_chr), no_Magic(c));
		    החזר FAIL;
	    }
	    اكسر;

	حالة Magic('%'):
	    c = no_Magic(getchr());
	    بدل (c)
	    {
		// () without a back reference
		حالة '(':
		    אם (nfa_reg(REG_NPAREN) == FAIL)
			החזר FAIL;
		    EMIT(NFA_NOPEN);
		    اكسر;

		حالة 'd':   // %d123 decimal
		حالة 'o':   // %o123 octal
		حالة 'x':   // %xab hex 2
		حالة 'u':   // %uabcd hex 4
		حالة 'U':   // %U1234abcd hex 8
		    {
			vimlong_T nr;

			بدل (c)
			{
			    حالة 'd': nr = getdecchrs(); اكسر;
			    حالة 'o': nr = getoctchrs(); اكسر;
			    حالة 'x': nr = gethexchrs(2); اكسر;
			    حالة 'u': nr = gethexchrs(4); اكسر;
			    حالة 'U': nr = gethexchrs(8); اكسر;
			    افتراضي:  nr = -1; اكسر;
			}

			אם (nr < 0 || nr > INT_MAX)
			    EMSG2_RET_FAIL(_(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			// A NUL is stored in the text as NL
			// TODO: what if a composing character follows?
			EMIT(nr == 0 ? 0x0a : (long)nr);
		    }
		    اكسر;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		حالة '^':
		    EMIT(NFA_BOF);
		    اكسر;

		حالة '$':
		    EMIT(NFA_EOF);
		    اكسر;

		حالة '#':
		    אם (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_start_of_pattern),
								  regparse[1]);
			החזר FAIL;
		    }
		    EMIT(NFA_CURSOR);
		    اكسر;

		حالة 'V':
		    EMIT(NFA_VISUAL);
		    اكسر;

		حالة 'C':
		    EMIT(NFA_ANY_COMPOSING);
		    اكسر;

		حالة '[':
		    {
			صحيح	    n;

			// \%[abc]
			עבור (n = 0; (c = peekchr()) != ']'; ++n)
			{
			    אם (c == NUL)
				EMSG2_RET_FAIL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
			    // recursive call!
			    אם (nfa_regatom() == FAIL)
				החזר FAIL;
			}
			getchr();  // get the ]
			אם (n == 0)
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
			اكسر;
		    }

		افتراضي:
		    {
			long_u	n = 0;
			صحيح	cmp = c;
			صحيح	cur = FALSE;
			صحيح	got_digit = FALSE;

			אם (c == '<' || c == '>')
			    c = getchr();
			אם (no_Magic(c) == '.')
			{
			    cur = TRUE;
			    c = getchr();
			}
			כלעוד (VIM_ISDIGIT(c))
			{
			    long_u tmp;

			    אם (cur)
			    {
				semsg(_(e_regexp_number_after_dot_pos_search_chr),
								  no_Magic(c));
				החזר FAIL;
			    }
			    tmp = n * 10 + (c - '0');

			    אם (tmp < n)
			    {
				// overflow.
				emsg(_(e_percent_value_too_large));
				החזר FAIL;
			    }
			    n = tmp;
			    c = getchr();
			    got_digit = TRUE;
			}
			אם (c == 'l' || c == 'c' || c == 'v')
			{
			    long_u limit = INT_MAX;

			    אם (!cur && !got_digit)
			    {
				semsg(_(e_nfa_regexp_missing_value_in_chr),
								  no_Magic(c));
				החזר FAIL;
			    }
			    אם (c == 'l')
			    {
				אם (cur)
				    n = curwin->w_cursor.lnum;
				// \%{n}l  \%{n}<l  \%{n}>l
				EMIT(cmp == '<' ? NFA_LNUM_LT :
				     cmp == '>' ? NFA_LNUM_GT : NFA_LNUM);
				אם (save_prev_at_start)
				    at_start = TRUE;
			    }
			    אחרת אם (c == 'c')
			    {
				אם (cur)
				{
				    n = curwin->w_cursor.col;
				    n++;
				}
				// \%{n}c  \%{n}<c  \%{n}>c
				EMIT(cmp == '<' ? NFA_COL_LT :
				     cmp == '>' ? NFA_COL_GT : NFA_COL);
			    }
			    אחרת
			    {
				אם (cur)
				{
				    colnr_T vcol = 0;

				    getvvcol(curwin, &curwin->w_cursor,
							 عدم, عدم, &vcol, 0);
				    n = ++vcol;
				}
				// \%{n}v  \%{n}<v  \%{n}>v
				EMIT(cmp == '<' ? NFA_VCOL_LT :
				     cmp == '>' ? NFA_VCOL_GT : NFA_VCOL);
				limit = INT_MAX / MB_MAXBYTES;
			    }
			    אם (n >= limit)
			    {
				emsg(_(e_percent_value_too_large));
				החזר FAIL;
			    }
			    EMIT((صحيح)n);
			    اكسر;
			}
			אחרת אם (no_Magic(c) == '\'' && n == 0)
			{
			    // \%'m  \%<'m  \%>'m
			    EMIT(cmp == '<' ? NFA_MARK_LT :
				 cmp == '>' ? NFA_MARK_GT : NFA_MARK);
			    EMIT(getchr());
			    اكسر;
			}
		    }
		    semsg(_(e_nfa_regexp_unknown_operator_percent_chr),
								  no_Magic(c));
		    החזר FAIL;
	    }
	    اكسر;

	حالة Magic('['):
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
	    אם (*endp == ']')
	    {
		صحيح plen;
		bool range_endpoint;
		/*
		 * Try to reverse engineer character classes. For example,
		 * recognize that [0-9] stands for \d and [A-Za-z_] for \h,
		 * and perform the necessary substitutions in the NFA.
		 */
		result = nfa_recognize_char_class(regparse, endp,
							 extra == NFA_ADD_NL);
		אם (result != FAIL)
		{
		    אם (result >= NFA_FIRST_NL && result <= NFA_LAST_NL)
		    {
			EMIT(result - NFA_ADD_NL);
			EMIT(NFA_NEWL);
			EMIT(NFA_OR);
		    }
		    אחרת
			EMIT(result);
		    regparse = endp;
		    MB_PTR_ADV(regparse);
		    החזר OK;
		}
		/*
		 * Failed to recognize a character class. Use the simple
		 * version that turns [abc] into 'a' OR 'b' OR 'c'
		 */
		startc = -1;
		negated = FALSE;
		אם (*regparse == '^')			// negated range
		{
		    negated = TRUE;
		    MB_PTR_ADV(regparse);
		    EMIT(NFA_START_NEG_COLL);
		}
		אחרת
		    EMIT(NFA_START_COLL);
		אם (*regparse == '-')
		{
		    startc = '-';
		    EMIT(startc);
		    EMIT(NFA_CONCAT);
		    MB_PTR_ADV(regparse);
		}
		// Emit the OR branches for each character in the []
		emit_range = FALSE;
		כלעוד (regparse < endp)
		{
		    صحيح	    oldstartc = startc;
		    range_endpoint = false;

		    startc = -1;
		    got_coll_char = FALSE;
		    אם (*regparse == '[')
		    {
			// Check for [: :], [= =], [. .]
			equiclass = collclass = 0;
			charclass = get_char_class(&regparse);
			אם (charclass == CLASS_NONE)
			{
			    equiclass = get_equi_class(&regparse);
			    אם (equiclass == 0)
				collclass = get_coll_element(&regparse);
			}

			// Character class like [:alpha:]
			אם (charclass != CLASS_NONE)
			{
			    بدل (charclass)
			    {
				حالة CLASS_ALNUM:
				    EMIT(NFA_CLASS_ALNUM);
				    اكسر;
				حالة CLASS_ALPHA:
				    EMIT(NFA_CLASS_ALPHA);
				    اكسر;
				حالة CLASS_BLANK:
				    EMIT(NFA_CLASS_BLANK);
				    اكسر;
				حالة CLASS_CNTRL:
				    EMIT(NFA_CLASS_CNTRL);
				    اكسر;
				حالة CLASS_DIGIT:
				    EMIT(NFA_CLASS_DIGIT);
				    اكسر;
				حالة CLASS_GRAPH:
				    EMIT(NFA_CLASS_GRAPH);
				    اكسر;
				حالة CLASS_LOWER:
				    wants_nfa = TRUE;
				    EMIT(NFA_CLASS_LOWER);
				    اكسر;
				حالة CLASS_PRINT:
				    EMIT(NFA_CLASS_PRINT);
				    اكسر;
				حالة CLASS_PUNCT:
				    EMIT(NFA_CLASS_PUNCT);
				    اكسر;
				حالة CLASS_SPACE:
				    EMIT(NFA_CLASS_SPACE);
				    اكسر;
				حالة CLASS_UPPER:
				    wants_nfa = TRUE;
				    EMIT(NFA_CLASS_UPPER);
				    اكسر;
				حالة CLASS_XDIGIT:
				    EMIT(NFA_CLASS_XDIGIT);
				    اكسر;
				حالة CLASS_TAB:
				    EMIT(NFA_CLASS_TAB);
				    اكسر;
				حالة CLASS_RETURN:
				    EMIT(NFA_CLASS_RETURN);
				    اكسر;
				حالة CLASS_BACKSPACE:
				    EMIT(NFA_CLASS_BACKSPACE);
				    اكسر;
				حالة CLASS_ESCAPE:
				    EMIT(NFA_CLASS_ESCAPE);
				    اكسر;
				حالة CLASS_IDENT:
				    EMIT(NFA_CLASS_IDENT);
				    اكسر;
				حالة CLASS_KEYWORD:
				    EMIT(NFA_CLASS_KEYWORD);
				    اكسر;
				حالة CLASS_FNAME:
				    EMIT(NFA_CLASS_FNAME);
				    اكسر;
			    }
			    EMIT(NFA_CONCAT);
			    استمر;
			}
			// Try equivalence class [=a=] and the like
			אם (equiclass != 0)
			{
			    result = nfa_emit_equi_class(equiclass);
			    אם (result == FAIL)
			    {
				// should never happen
				EMSG_RET_FAIL(_(e_error_building_nfa_with_equivalence_class));
			    }
			    استمر;
			}
			// Try collating class like [. .]
			אם (collclass != 0)
			{
			    startc = collclass;	 // allow [.a.]-x as a range
			    // Will emit the proper atom at the end of the
			    // while loop.
			}
		    }
		    // Try a range like 'a-x' or '\t-z'. Also allows '-' as a
		    // start character.
		    אם (*regparse == '-' && oldstartc != -1)
		    {
			emit_range = TRUE;
			startc = oldstartc;
			MB_PTR_ADV(regparse);
			استمر;	    // reading the end of the range
		    }

		    // Now handle simple and escaped characters.
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    אם (*regparse == '\\'
			    && !reg_cpo_bsl
			    && regparse + 1 <= endp
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != عدم
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR, regparse[1])
								      != عدم)
			    )
			)
		    {
			MB_PTR_ADV(regparse);

			אם (*regparse == 'n')
			    startc = (reg_string || emit_range
					|| regparse[1] == '-') ? NL : NFA_NEWL;
			אחרת אם (*regparse == 'd'
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
				אם (startc == INT_MAX)
				    EMSG_RET_FAIL(_(e_unicode_val_too_large));
				got_coll_char = TRUE;
				MB_PTR_BACK(old_regparse, regparse);
			    }
			    אחרת
			    {
				// \r,\t,\e,\b
				startc = backslash_trans(*regparse);
			    }
		    }

		    // Normal printable char
		    אם (startc == -1)
			startc = PTR2CHAR(regparse);

		    // Previous char was '-', so this char is end of range.
		    אם (emit_range)
		    {
			صحيح	endc = startc;
			range_endpoint = true;

			startc = oldstartc;
			אם (startc > endc)
			    EMSG_RET_FAIL(_(e_reverse_range_in_character_class));

			אם (endc > startc + 2)
			{
			    // Emit a range instead of the sequence of
			    // individual characters.
			    אם (startc == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    אחרת
				--post_ptr; // remove NFA_CONCAT
			    EMIT(endc);
			    EMIT(NFA_RANGE);
			    EMIT(NFA_CONCAT);
			}
			אחרת אם (has_mbyte && ((*mb_char2len)(startc) > 1
				    || (*mb_char2len)(endc) > 1))
			{
			    // Emit the characters in the range.
			    // "startc" was already emitted, so skip it.
			    //
			    עבור (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			אחרת
			{
			    // Emit the range. "startc" was already emitted, so
			    // skip it.
			    עבור (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			emit_range = FALSE;
			startc = -1;
		    }
		    אחרת
		    {
			// This char (startc) is not part of a range. Just
			// emit it.
			// Normally, simply emit startc. But if we get char
			// code=0 from a collating char, then replace it with
			// 0x0a.
			// This is needed to completely mimic the behaviour of
			// the backtracking engine.
			אם (startc == NFA_NEWL)
			{
			    // Line break can't be matched as part of the
			    // collection, add an OR below. But not for negated
			    // range.
			    אם (!negated)
				extra = NFA_ADD_NL;
			}
			אחרת
			{
			    אם (got_coll_char == TRUE && startc == 0)
			    {
				EMIT(0x0a);
				EMIT(NFA_CONCAT);
			    }
			    אחרת
			    {
				EMIT(startc);
				אם (!(enc_utf8 && (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse)))))
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
		    אם (!range_endpoint && enc_utf8 &&
			    (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse))))
		    {
			صحيح i = utf_ptr2len(regparse);

			c = utf_ptr2char(regparse + i);

			// Add composing characters
			עבור (;;)
			{
			    אם (c == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    אחרת
				EMIT(c);
			    EMIT(NFA_CONCAT);
			    אם ((i += utf_char2len(c)) >= plen)
				اكسر;
			    c = utf_ptr2char(regparse + i);
			}
			EMIT(NFA_COMPOSING);
			EMIT(NFA_CONCAT);
		    }
		    MB_PTR_ADV(regparse);
		} // while (p < endp)

		MB_PTR_BACK(old_regparse, regparse);
		אם (*regparse == '-')	    // if last, '-' is just a char
		{
		    EMIT('-');
		    EMIT(NFA_CONCAT);
		}

		// skip the trailing ]
		regparse = endp;
		MB_PTR_ADV(regparse);

		// Mark end of the collection.
		אם (negated == TRUE)
		    EMIT(NFA_END_NEG_COLL);
		אחרת
		    EMIT(NFA_END_COLL);

		// \_[] also matches \n but it's not negated
		אם (extra == NFA_ADD_NL)
		{
		    EMIT(reg_string ? NL : NFA_NEWL);
		    EMIT(NFA_OR);
		}

		החזר OK;
	    } // if exists closing ]

	    אם (reg_strict)
		EMSG_RET_FAIL(_(e_missing_rsb_after_str_lsb));
	    // FALLTHROUGH

	افتراضي:
	    {
		صحيح	plen;

nfa_do_multibyte:
		// plen is length of current char with composing chars
		אם (enc_utf8 && ((*mb_char2len)(c)
			    != (plen = utfc_ptr2len(old_regparse))
						       || utf_iscomposing(c)))
		{
		    صحيح i = 0;

		    // A base character plus composing characters, or just one
		    // or more composing characters.
		    // This requires creating a separate atom as if enclosing
		    // the characters in (), where NFA_COMPOSING is the ( and
		    // NFA_END_COMPOSING is the ). Note that right now we are
		    // building the postfix form, not the NFA itself;
		    // a composing char could be: a, b, c, NFA_COMPOSING
		    // where 'b' and 'c' are chars with codes > 256.
		    עבור (;;)
		    {
			EMIT(c);
			אם (i > 0)
			    EMIT(NFA_CONCAT);
			אם ((i += utf_char2len(c)) >= plen)
			    اكسر;
			c = utf_ptr2char(old_regparse + i);
		    }
		    EMIT(NFA_COMPOSING);
		    regparse = old_regparse + plen;
		}
		אחרת
		{
		    c = no_Magic(c);
		    EMIT(c);
		}
		החזר OK;
	    }
    }

    החזר OK;
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
    ساكن صحيح
nfa_regpiece(فراغ)
{
    صحيح		i;
    صحيح		op;
    صحيح		ret;
    long	minval, maxval;
    صحيح		greedy = TRUE;      // Braces are prefixed with '-' ?
    parse_state_T old_state;
    parse_state_T new_state;
    long	c2;
    صحيح		old_post_pos;
    صحيح		my_post_start;
    صحيح		quest;

    // Save the current parse state, so that we can use it if <atom>{m,n} is
    // next.
    save_parse_state(&old_state);

    // store current pos in the postfix form, for \{m,n} involving 0s
    my_post_start = (صحيح)(post_ptr - post_start);

    ret = nfa_regatom();
    אם (ret == FAIL)
	החזר FAIL;	    // cascaded error

    op = peekchr();
    אם (re_multi_type(op) == NOT_MULTI)
	החזר OK;

    skipchr();
    بدل (op)
    {
	حالة Magic('*'):
	    EMIT(NFA_STAR);
	    اكسر;

	حالة Magic('+'):
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
	    אם (nfa_regatom() == FAIL)
		החזר FAIL;
	    EMIT(NFA_STAR);
	    EMIT(NFA_CONCAT);
	    skipchr();		// skip the \+
	    اكسر;

	حالة Magic('@'):
	    c2 = (long)getdecchrs();
	    op = no_Magic(getchr());
	    i = 0;
	    بدل(op)
	    {
		حالة '=':
		    // \@=
		    i = NFA_PREV_ATOM_NO_WIDTH;
		    اكسر;
		حالة '!':
		    // \@!
		    i = NFA_PREV_ATOM_NO_WIDTH_NEG;
		    اكسر;
		حالة '<':
		    op = no_Magic(getchr());
		    אם (op == '=')
			// \@<=
			i = NFA_PREV_ATOM_JUST_BEFORE;
		    אחרת אם (op == '!')
			// \@<!
			i = NFA_PREV_ATOM_JUST_BEFORE_NEG;
		    اكسر;
		حالة '>':
		    // \@>
		    i = NFA_PREV_ATOM_LIKE_PATTERN;
		    اكسر;
	    }
	    אם (i == 0)
	    {
		semsg(_(e_nfa_regexp_unknown_operator_at_chr), op);
		החזר FAIL;
	    }
	    EMIT(i);
	    אם (i == NFA_PREV_ATOM_JUST_BEFORE
					|| i == NFA_PREV_ATOM_JUST_BEFORE_NEG)
		EMIT(c2);
	    اكسر;

	حالة Magic('?'):
	حالة Magic('='):
	    EMIT(NFA_QUEST);
	    اكسر;

	حالة Magic('{'):
	    // a{2,5} will expand to 'aaa?a?a?'
	    // a{-1,3} will expand to 'aa??a??', where ?? is the nongreedy
	    // version of '?'
	    // \v(ab){2,3} will expand to '(ab)(ab)(ab)?', where all the
	    // parenthesis have the same id

	    greedy = TRUE;
	    c2 = peekchr();
	    אם (c2 == '-' || c2 == Magic('-'))
	    {
		skipchr();
		greedy = FALSE;
	    }
	    אם (!read_limits(&minval, &maxval))
		EMSG_RET_FAIL(_(e_nfa_regexp_error_reading_repetition_limits));

	    //  <atom>{0,inf}, <atom>{0,} and <atom>{}  are equivalent to
	    //  <atom>*
	    אם (minval == 0 && maxval == MAX_LIMIT)
	    {
		אם (greedy)		// { { (match the braces)
		    // \{}, \{0,}
		    EMIT(NFA_STAR);
		אחרת			// { { (match the braces)
		    // \{-}, \{-0,}
		    EMIT(NFA_STAR_NONGREEDY);
		اكسر;
	    }

	    // Special case: x{0} or x{-0}
	    אם (maxval == 0)
	    {
		// Ignore result of previous call to nfa_regatom()
		post_ptr = post_start + my_post_start;
		// NFA_EMPTY is 0-length and works everywhere
		EMIT(NFA_EMPTY);
		החזר OK;
	    }

	    // The engine is very inefficient (uses too many states) when the
	    // maximum is much larger than the minimum and when the maximum is
	    // large.  However, when maxval is MAX_LIMIT, it is okay, as this
	    // will emit NFA_STAR.
	    // Bail out if we can use the other engine, but only, when the
	    // pattern does not need the NFA engine like (e.g. [[:upper:]]\{2,\}
	    // does not work with characters > 8 bit with the BT engine)
	    אם ((nfa_re_flags & RE_AUTO)
				   && (maxval > 500 || maxval > minval + 200)
				   && (maxval != MAX_LIMIT && minval < 200)
				   && !wants_nfa)
		החזר FAIL;

	    // Ignore previous call to nfa_regatom()
	    post_ptr = post_start + my_post_start;
	    // Save parse state after the repeated atom and the \{}
	    save_parse_state(&new_state);

	    quest = (greedy == TRUE? NFA_QUEST : NFA_QUEST_NONGREEDY);
	    עבור (i = 0; i < maxval; i++)
	    {
		// Goto beginning of the repeated atom
		restore_parse_state(&old_state);
		old_post_pos = (صحيح)(post_ptr - post_start);
		אם (nfa_regatom() == FAIL)
		    החזר FAIL;
		// after "minval" times, atoms are optional
		אם (i + 1 > minval)
		{
		    אם (maxval == MAX_LIMIT)
		    {
			אם (greedy)
			    EMIT(NFA_STAR);
			אחרת
			    EMIT(NFA_STAR_NONGREEDY);
		    }
		    אחרת
			EMIT(quest);
		}
		אם (old_post_pos != my_post_start)
		    EMIT(NFA_CONCAT);
		אם (i + 1 > minval && maxval == MAX_LIMIT)
		    اكسر;
	    }

	    // Go to just after the repeated atom and the \{}
	    restore_parse_state(&new_state);
	    curchr = -1;

	    اكسر;


	افتراضي:
	    اكسر;
    }	// end switch

    אם (re_multi_type(peekchr()) != NOT_MULTI)
	// Can't have a multi follow a multi.
	EMSG_RET_FAIL(_(e_nfa_regexp_cant_have_multi_follow_multi));

    החזר OK;
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
    ساكن صحيح
nfa_regconcat(فراغ)
{
    صحيح		cont = TRUE;
    صحيح		first = TRUE;

    כלעוד (cont)
    {
	بدل (peekchr())
	{
	    حالة NUL:
	    حالة Magic('|'):
	    حالة Magic('&'):
	    حالة Magic(')'):
		cont = FALSE;
		اكسر;

	    حالة Magic('Z'):
		regflags |= RF_ICOMBINE;
		skipchr_keepstart();
		اكسر;
	    حالة Magic('c'):
		regflags |= RF_ICASE;
		skipchr_keepstart();
		اكسر;
	    حالة Magic('C'):
		regflags |= RF_NOICASE;
		skipchr_keepstart();
		اكسر;
	    حالة Magic('v'):
		reg_magic = MAGIC_ALL;
		skipchr_keepstart();
		curchr = -1;
		اكسر;
	    حالة Magic('m'):
		reg_magic = MAGIC_ON;
		skipchr_keepstart();
		curchr = -1;
		اكسر;
	    حالة Magic('M'):
		reg_magic = MAGIC_OFF;
		skipchr_keepstart();
		curchr = -1;
		اكسر;
	    حالة Magic('V'):
		reg_magic = MAGIC_NONE;
		skipchr_keepstart();
		curchr = -1;
		اكسر;

	    افتراضي:
		אם (nfa_regpiece() == FAIL)
		    החזר FAIL;
		אם (first == FALSE)
		    EMIT(NFA_CONCAT);
		אחרת
		    first = FALSE;
		اكسر;
	}
    }

    החזר OK;
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
    ساكن صحيح
nfa_regbranch(فراغ)
{
    صحيح		old_post_pos;

    old_post_pos = (صحيح)(post_ptr - post_start);

    // First branch, possibly the only one
    אם (nfa_regconcat() == FAIL)
	החזר FAIL;

    // Try next concats
    כלעוד (peekchr() == Magic('&'))
    {
	skipchr();
	// if concat is empty do emit a node
	אם (old_post_pos == (صحيح)(post_ptr - post_start))
	    EMIT(NFA_EMPTY);
	EMIT(NFA_NOPEN);
	EMIT(NFA_PREV_ATOM_NO_WIDTH);
	old_post_pos = (صحيح)(post_ptr - post_start);
	אם (nfa_regconcat() == FAIL)
	    החזר FAIL;
	// if concat is empty do emit a node
	אם (old_post_pos == (صحيح)(post_ptr - post_start))
	    EMIT(NFA_EMPTY);
	EMIT(NFA_CONCAT);
    }

    // if a branch is empty, emit one node for it
    אם (old_post_pos == (صحيح)(post_ptr - post_start))
	EMIT(NFA_EMPTY);

    החזר OK;
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
    ساكن صحيح
nfa_reg(
    صحيح		paren)	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
{
    صحيح		parno = 0;
    صحيح		status = FAIL;

    אם (paren == REG_PAREN)
    {
	אם (regnpar >= NSUBEXP) // Too many `('
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_parens));
	parno = regnpar++;
    }
#ifdef FEAT_SYN_HL
    אחרת אם (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	אם (regnzpar >= NSUBEXP)
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_z));
	parno = regnzpar++;
    }
#endif

    אם (nfa_reg_parse_depth >= REG_MAX_PAREN_DEPTH)
	EMSG_RET_FAIL(_(e_command_too_complex));
    ++nfa_reg_parse_depth;

    אם (nfa_regbranch() == FAIL)
	اذهب theend;	    // cascaded error

    כלעוד (peekchr() == Magic('|'))
    {
	skipchr();
	אם (nfa_regbranch() == FAIL)
	    اذهب theend;    // cascaded error
	EMIT(NFA_OR);
    }

    // Check for proper termination.
    אם (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
	אם (paren == REG_NPAREN)
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	אחרת
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	rc_did_emsg = TRUE;
	اذهب theend;
    }
    אחרת אם (paren == REG_NOPAREN && peekchr() != NUL)
    {
	אם (peekchr() == Magic(')'))
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	אחרת
	    emsg(_(e_nfa_regexp_proper_termination_error));
	rc_did_emsg = TRUE;
	اذهب theend;
    }
    /*
     * Here we set the flag allowing back references to this set of
     * parentheses.
     */
    אם (paren == REG_PAREN)
    {
	had_endbrace[parno] = TRUE;     // have seen the close paren
	EMIT(NFA_MOPEN + parno);
    }
#ifdef FEAT_SYN_HL
    אחרת אם (paren == REG_ZPAREN)
	EMIT(NFA_ZOPEN + parno);
#endif

    status = OK;

theend:
    --nfa_reg_parse_depth;
    החזר status;
}

#ifdef DEBUG
ساكن char_u code[50];

    ساكن فراغ
nfa_set_code(صحيح c)
{
    صحيح	    addnl = FALSE;

    אם (c >= NFA_FIRST_NL && c <= NFA_LAST_NL)
    {
	addnl = TRUE;
	c -= NFA_ADD_NL;
    }

    STRCPY(code, "");
    بدل (c)
    {
	حالة NFA_MATCH:	    STRCPY(code, "NFA_MATCH "); اكسر;
	حالة NFA_SPLIT:	    STRCPY(code, "NFA_SPLIT "); اكسر;
	حالة NFA_CONCAT:    STRCPY(code, "NFA_CONCAT "); اكسر;
	حالة NFA_NEWL:	    STRCPY(code, "NFA_NEWL "); اكسر;
	حالة NFA_ZSTART:    STRCPY(code, "NFA_ZSTART"); اكسر;
	حالة NFA_ZEND:	    STRCPY(code, "NFA_ZEND"); اكسر;

	حالة NFA_BACKREF1:  STRCPY(code, "NFA_BACKREF1"); اكسر;
	حالة NFA_BACKREF2:  STRCPY(code, "NFA_BACKREF2"); اكسر;
	حالة NFA_BACKREF3:  STRCPY(code, "NFA_BACKREF3"); اكسر;
	حالة NFA_BACKREF4:  STRCPY(code, "NFA_BACKREF4"); اكسر;
	حالة NFA_BACKREF5:  STRCPY(code, "NFA_BACKREF5"); اكسر;
	حالة NFA_BACKREF6:  STRCPY(code, "NFA_BACKREF6"); اكسر;
	حالة NFA_BACKREF7:  STRCPY(code, "NFA_BACKREF7"); اكسر;
	حالة NFA_BACKREF8:  STRCPY(code, "NFA_BACKREF8"); اكسر;
	حالة NFA_BACKREF9:  STRCPY(code, "NFA_BACKREF9"); اكسر;
# ifdef FEAT_SYN_HL
	حالة NFA_ZREF1:	    STRCPY(code, "NFA_ZREF1"); اكسر;
	حالة NFA_ZREF2:	    STRCPY(code, "NFA_ZREF2"); اكسر;
	حالة NFA_ZREF3:	    STRCPY(code, "NFA_ZREF3"); اكسر;
	حالة NFA_ZREF4:	    STRCPY(code, "NFA_ZREF4"); اكسر;
	حالة NFA_ZREF5:	    STRCPY(code, "NFA_ZREF5"); اكسر;
	حالة NFA_ZREF6:	    STRCPY(code, "NFA_ZREF6"); اكسر;
	حالة NFA_ZREF7:	    STRCPY(code, "NFA_ZREF7"); اكسر;
	حالة NFA_ZREF8:	    STRCPY(code, "NFA_ZREF8"); اكسر;
	حالة NFA_ZREF9:	    STRCPY(code, "NFA_ZREF9"); اكسر;
# endif
	حالة NFA_SKIP:	    STRCPY(code, "NFA_SKIP"); اكسر;

	حالة NFA_PREV_ATOM_NO_WIDTH:
			    STRCPY(code, "NFA_PREV_ATOM_NO_WIDTH"); اكسر;
	حالة NFA_PREV_ATOM_NO_WIDTH_NEG:
			    STRCPY(code, "NFA_PREV_ATOM_NO_WIDTH_NEG"); اكسر;
	حالة NFA_PREV_ATOM_JUST_BEFORE:
			    STRCPY(code, "NFA_PREV_ATOM_JUST_BEFORE"); اكسر;
	حالة NFA_PREV_ATOM_JUST_BEFORE_NEG:
			 STRCPY(code, "NFA_PREV_ATOM_JUST_BEFORE_NEG"); اكسر;
	حالة NFA_PREV_ATOM_LIKE_PATTERN:
			    STRCPY(code, "NFA_PREV_ATOM_LIKE_PATTERN"); اكسر;

	حالة NFA_NOPEN:		    STRCPY(code, "NFA_NOPEN"); اكسر;
	حالة NFA_NCLOSE:	    STRCPY(code, "NFA_NCLOSE"); اكسر;
	حالة NFA_START_INVISIBLE:   STRCPY(code, "NFA_START_INVISIBLE"); اكسر;
	حالة NFA_START_INVISIBLE_FIRST:
			     STRCPY(code, "NFA_START_INVISIBLE_FIRST"); اكسر;
	حالة NFA_START_INVISIBLE_NEG:
			       STRCPY(code, "NFA_START_INVISIBLE_NEG"); اكسر;
	حالة NFA_START_INVISIBLE_NEG_FIRST:
			 STRCPY(code, "NFA_START_INVISIBLE_NEG_FIRST"); اكسر;
	حالة NFA_START_INVISIBLE_BEFORE:
			    STRCPY(code, "NFA_START_INVISIBLE_BEFORE"); اكسر;
	حالة NFA_START_INVISIBLE_BEFORE_FIRST:
		      STRCPY(code, "NFA_START_INVISIBLE_BEFORE_FIRST"); اكسر;
	حالة NFA_START_INVISIBLE_BEFORE_NEG:
			STRCPY(code, "NFA_START_INVISIBLE_BEFORE_NEG"); اكسر;
	حالة NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
		  STRCPY(code, "NFA_START_INVISIBLE_BEFORE_NEG_FIRST"); اكسر;
	حالة NFA_START_PATTERN:   STRCPY(code, "NFA_START_PATTERN"); اكسر;
	حالة NFA_END_INVISIBLE:	    STRCPY(code, "NFA_END_INVISIBLE"); اكسر;
	حالة NFA_END_INVISIBLE_NEG: STRCPY(code, "NFA_END_INVISIBLE_NEG"); اكسر;
	حالة NFA_END_PATTERN:	    STRCPY(code, "NFA_END_PATTERN"); اكسر;

	حالة NFA_COMPOSING:	    STRCPY(code, "NFA_COMPOSING"); اكسر;
	حالة NFA_END_COMPOSING:	    STRCPY(code, "NFA_END_COMPOSING"); اكسر;
	حالة NFA_OPT_CHARS:	    STRCPY(code, "NFA_OPT_CHARS"); اكسر;

	حالة NFA_MOPEN:
	حالة NFA_MOPEN1:
	حالة NFA_MOPEN2:
	حالة NFA_MOPEN3:
	حالة NFA_MOPEN4:
	حالة NFA_MOPEN5:
	حالة NFA_MOPEN6:
	حالة NFA_MOPEN7:
	حالة NFA_MOPEN8:
	حالة NFA_MOPEN9:
	    STRCPY(code, "NFA_MOPEN(x)");
	    code[10] = c - NFA_MOPEN + '0';
	    اكسر;
	حالة NFA_MCLOSE:
	حالة NFA_MCLOSE1:
	حالة NFA_MCLOSE2:
	حالة NFA_MCLOSE3:
	حالة NFA_MCLOSE4:
	حالة NFA_MCLOSE5:
	حالة NFA_MCLOSE6:
	حالة NFA_MCLOSE7:
	حالة NFA_MCLOSE8:
	حالة NFA_MCLOSE9:
	    STRCPY(code, "NFA_MCLOSE(x)");
	    code[11] = c - NFA_MCLOSE + '0';
	    اكسر;
# ifdef FEAT_SYN_HL
	حالة NFA_ZOPEN:
	حالة NFA_ZOPEN1:
	حالة NFA_ZOPEN2:
	حالة NFA_ZOPEN3:
	حالة NFA_ZOPEN4:
	حالة NFA_ZOPEN5:
	حالة NFA_ZOPEN6:
	حالة NFA_ZOPEN7:
	حالة NFA_ZOPEN8:
	حالة NFA_ZOPEN9:
	    STRCPY(code, "NFA_ZOPEN(x)");
	    code[10] = c - NFA_ZOPEN + '0';
	    اكسر;
	حالة NFA_ZCLOSE:
	حالة NFA_ZCLOSE1:
	حالة NFA_ZCLOSE2:
	حالة NFA_ZCLOSE3:
	حالة NFA_ZCLOSE4:
	حالة NFA_ZCLOSE5:
	حالة NFA_ZCLOSE6:
	حالة NFA_ZCLOSE7:
	حالة NFA_ZCLOSE8:
	حالة NFA_ZCLOSE9:
	    STRCPY(code, "NFA_ZCLOSE(x)");
	    code[11] = c - NFA_ZCLOSE + '0';
	    اكسر;
# endif
	حالة NFA_EOL:		STRCPY(code, "NFA_EOL "); اكسر;
	حالة NFA_BOL:		STRCPY(code, "NFA_BOL "); اكسر;
	حالة NFA_EOW:		STRCPY(code, "NFA_EOW "); اكسر;
	حالة NFA_BOW:		STRCPY(code, "NFA_BOW "); اكسر;
	حالة NFA_EOF:		STRCPY(code, "NFA_EOF "); اكسر;
	حالة NFA_BOF:		STRCPY(code, "NFA_BOF "); اكسر;
	حالة NFA_LNUM:		STRCPY(code, "NFA_LNUM "); اكسر;
	حالة NFA_LNUM_GT:	STRCPY(code, "NFA_LNUM_GT "); اكسر;
	حالة NFA_LNUM_LT:	STRCPY(code, "NFA_LNUM_LT "); اكسر;
	حالة NFA_COL:		STRCPY(code, "NFA_COL "); اكسر;
	حالة NFA_COL_GT:	STRCPY(code, "NFA_COL_GT "); اكسر;
	حالة NFA_COL_LT:	STRCPY(code, "NFA_COL_LT "); اكسر;
	حالة NFA_VCOL:		STRCPY(code, "NFA_VCOL "); اكسر;
	حالة NFA_VCOL_GT:	STRCPY(code, "NFA_VCOL_GT "); اكسر;
	حالة NFA_VCOL_LT:	STRCPY(code, "NFA_VCOL_LT "); اكسر;
	حالة NFA_MARK:		STRCPY(code, "NFA_MARK "); اكسر;
	حالة NFA_MARK_GT:	STRCPY(code, "NFA_MARK_GT "); اكسر;
	حالة NFA_MARK_LT:	STRCPY(code, "NFA_MARK_LT "); اكسر;
	حالة NFA_CURSOR:	STRCPY(code, "NFA_CURSOR "); اكسر;
	حالة NFA_VISUAL:	STRCPY(code, "NFA_VISUAL "); اكسر;
	حالة NFA_ANY_COMPOSING:	STRCPY(code, "NFA_ANY_COMPOSING "); اكسر;

	حالة NFA_STAR:		STRCPY(code, "NFA_STAR "); اكسر;
	حالة NFA_STAR_NONGREEDY: STRCPY(code, "NFA_STAR_NONGREEDY "); اكسر;
	حالة NFA_QUEST:		STRCPY(code, "NFA_QUEST"); اكسر;
	حالة NFA_QUEST_NONGREEDY: STRCPY(code, "NFA_QUEST_NON_GREEDY"); اكسر;
	حالة NFA_EMPTY:		STRCPY(code, "NFA_EMPTY"); اكسر;
	حالة NFA_OR:		STRCPY(code, "NFA_OR"); اكسر;

	حالة NFA_START_COLL:	STRCPY(code, "NFA_START_COLL"); اكسر;
	حالة NFA_END_COLL:	STRCPY(code, "NFA_END_COLL"); اكسر;
	حالة NFA_START_NEG_COLL: STRCPY(code, "NFA_START_NEG_COLL"); اكسر;
	حالة NFA_END_NEG_COLL:	STRCPY(code, "NFA_END_NEG_COLL"); اكسر;
	حالة NFA_RANGE:		STRCPY(code, "NFA_RANGE"); اكسر;
	حالة NFA_RANGE_MIN:	STRCPY(code, "NFA_RANGE_MIN"); اكسر;
	حالة NFA_RANGE_MAX:	STRCPY(code, "NFA_RANGE_MAX"); اكسر;

	حالة NFA_CLASS_ALNUM:	STRCPY(code, "NFA_CLASS_ALNUM"); اكسر;
	حالة NFA_CLASS_ALPHA:	STRCPY(code, "NFA_CLASS_ALPHA"); اكسر;
	حالة NFA_CLASS_BLANK:	STRCPY(code, "NFA_CLASS_BLANK"); اكسر;
	حالة NFA_CLASS_CNTRL:	STRCPY(code, "NFA_CLASS_CNTRL"); اكسر;
	حالة NFA_CLASS_DIGIT:	STRCPY(code, "NFA_CLASS_DIGIT"); اكسر;
	حالة NFA_CLASS_GRAPH:	STRCPY(code, "NFA_CLASS_GRAPH"); اكسر;
	حالة NFA_CLASS_LOWER:	STRCPY(code, "NFA_CLASS_LOWER"); اكسر;
	حالة NFA_CLASS_PRINT:	STRCPY(code, "NFA_CLASS_PRINT"); اكسر;
	حالة NFA_CLASS_PUNCT:	STRCPY(code, "NFA_CLASS_PUNCT"); اكسر;
	حالة NFA_CLASS_SPACE:	STRCPY(code, "NFA_CLASS_SPACE"); اكسر;
	حالة NFA_CLASS_UPPER:	STRCPY(code, "NFA_CLASS_UPPER"); اكسر;
	حالة NFA_CLASS_XDIGIT:	STRCPY(code, "NFA_CLASS_XDIGIT"); اكسر;
	حالة NFA_CLASS_TAB:	STRCPY(code, "NFA_CLASS_TAB"); اكسر;
	حالة NFA_CLASS_RETURN:	STRCPY(code, "NFA_CLASS_RETURN"); اكسر;
	حالة NFA_CLASS_BACKSPACE:   STRCPY(code, "NFA_CLASS_BACKSPACE"); اكسر;
	حالة NFA_CLASS_ESCAPE:	STRCPY(code, "NFA_CLASS_ESCAPE"); اكسر;
	حالة NFA_CLASS_IDENT:	STRCPY(code, "NFA_CLASS_IDENT"); اكسر;
	حالة NFA_CLASS_KEYWORD:	STRCPY(code, "NFA_CLASS_KEYWORD"); اكسر;
	حالة NFA_CLASS_FNAME:	STRCPY(code, "NFA_CLASS_FNAME"); اكسر;

	حالة NFA_ANY:	STRCPY(code, "NFA_ANY"); اكسر;
	حالة NFA_IDENT:	STRCPY(code, "NFA_IDENT"); اكسر;
	حالة NFA_SIDENT:STRCPY(code, "NFA_SIDENT"); اكسر;
	حالة NFA_KWORD:	STRCPY(code, "NFA_KWORD"); اكسر;
	حالة NFA_SKWORD:STRCPY(code, "NFA_SKWORD"); اكسر;
	حالة NFA_FNAME:	STRCPY(code, "NFA_FNAME"); اكسر;
	حالة NFA_SFNAME:STRCPY(code, "NFA_SFNAME"); اكسر;
	حالة NFA_PRINT:	STRCPY(code, "NFA_PRINT"); اكسر;
	حالة NFA_SPRINT:STRCPY(code, "NFA_SPRINT"); اكسر;
	حالة NFA_WHITE:	STRCPY(code, "NFA_WHITE"); اكسر;
	حالة NFA_NWHITE:STRCPY(code, "NFA_NWHITE"); اكسر;
	حالة NFA_DIGIT:	STRCPY(code, "NFA_DIGIT"); اكسر;
	حالة NFA_NDIGIT:STRCPY(code, "NFA_NDIGIT"); اكسر;
	حالة NFA_HEX:	STRCPY(code, "NFA_HEX"); اكسر;
	حالة NFA_NHEX:	STRCPY(code, "NFA_NHEX"); اكسر;
	حالة NFA_OCTAL:	STRCPY(code, "NFA_OCTAL"); اكسر;
	حالة NFA_NOCTAL:STRCPY(code, "NFA_NOCTAL"); اكسر;
	حالة NFA_WORD:	STRCPY(code, "NFA_WORD"); اكسر;
	حالة NFA_NWORD:	STRCPY(code, "NFA_NWORD"); اكسر;
	حالة NFA_HEAD:	STRCPY(code, "NFA_HEAD"); اكسر;
	حالة NFA_NHEAD:	STRCPY(code, "NFA_NHEAD"); اكسر;
	حالة NFA_ALPHA:	STRCPY(code, "NFA_ALPHA"); اكسر;
	حالة NFA_NALPHA:STRCPY(code, "NFA_NALPHA"); اكسر;
	حالة NFA_LOWER:	STRCPY(code, "NFA_LOWER"); اكسر;
	حالة NFA_NLOWER:STRCPY(code, "NFA_NLOWER"); اكسر;
	حالة NFA_UPPER:	STRCPY(code, "NFA_UPPER"); اكسر;
	حالة NFA_NUPPER:STRCPY(code, "NFA_NUPPER"); اكسر;
	حالة NFA_LOWER_IC:  STRCPY(code, "NFA_LOWER_IC"); اكسر;
	حالة NFA_NLOWER_IC: STRCPY(code, "NFA_NLOWER_IC"); اكسر;
	حالة NFA_UPPER_IC:  STRCPY(code, "NFA_UPPER_IC"); اكسر;
	حالة NFA_NUPPER_IC: STRCPY(code, "NFA_NUPPER_IC"); اكسر;

	افتراضي:
	    STRCPY(code, "CHAR(x)");
	    code[5] = c;
    }

    אם (addnl == TRUE)
	STRCAT(code, " + NEWLINE ");

}

# ifdef ENABLE_LOG
ساكن FILE *log_fd;
ساكن char_u e_log_open_failed[] = N_("Could not open temporary log file for writing, displaying on stderr... ");

/*
 * Print the postfix notation of the current regexp.
 */
    ساكن فراغ
nfa_postfix_dump(char_u *expr, صحيح retval)
{
    صحيح *p;
    FILE *f;

    f = fopen(NFA_REGEXP_DUMP_LOG, "a");
    אם (f == عدم)
	החזר;

    fprintf(f, "\n-------------------------\n");
    אם (retval == FAIL)
	fprintf(f, ">>> NFA engine failed... \n");
    אחרת אם (retval == OK)
	fprintf(f, ">>> NFA engine succeeded !\n");
    fprintf(f, "Regexp: \"%s\"\nPostfix notation (char): \"", expr);
    עבור (p = post_start; *p && p < post_ptr; p++)
    {
	nfa_set_code(*p);
	fprintf(f, "%s, ", code);
    }
    fprintf(f, "\"\nPostfix notation (int): ");
    עבור (p = post_start; *p && p < post_ptr; p++)
	fprintf(f, "%d ", *p);
    fprintf(f, "\n\n");
    fclose(f);
}

/*
 * Print the NFA starting with a root node "state".
 */
    ساكن فراغ
nfa_print_state(FILE *debugf, nfa_state_T *state)
{
    garray_T indent;

    ga_init2(&indent, 1, 64);
    ga_append(&indent, '\0');
    nfa_print_state2(debugf, state, &indent);
    ga_clear(&indent);
}

    ساكن فراغ
nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent)
{
    char_u  *p;

    אם (state == عدم)
	החזר;

    fprintf(debugf, "(%2d)", abs(state->id));

    // Output indent
    p = (char_u *)indent->ga_data;
    אם (indent->ga_len >= 3)
    {
	صحيح	last = indent->ga_len - 3;
	char_u	save[2];

	STRNCPY(save, &p[last], 2);
	memcpy(&p[last], "+-", 2);
	fprintf(debugf, " %s", p);
	STRNCPY(&p[last], save, 2);
    }
    אחרת
	fprintf(debugf, " %s", p);

    nfa_set_code(state->c);
    fprintf(debugf, "%s (%d) (id=%d) val=%d\n",
		 code,
		 state->c,
		 abs(state->id),
		 state->val);
    אם (state->id < 0)
	החזר;

    state->id = abs(state->id) * -1;

    // grow indent for state->out
    indent->ga_len -= 1;
    אם (state->out1)
	GA_CONCAT_LITERAL(indent, "| ");
    אחרת
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
    ساكن فراغ
nfa_dump(nfa_regprog_T *prog)
{
    FILE *debugf = fopen(NFA_REGEXP_DUMP_LOG, "a");

    אם (debugf == عدم)
	החזר;

    nfa_print_state(debugf, prog->start);

    אם (prog->reganch)
	fprintf(debugf, "reganch: %d\n", prog->reganch);
    אם (prog->regstart != NUL)
	fprintf(debugf, "regstart: %c (decimal: %d)\n",
		prog->regstart, prog->regstart);
    אם (prog->match_text != عدم)
	fprintf(debugf, "match_text: \"%s\"\n", prog->match_text);

    fclose(debugf);
}
# endif	    // ENABLE_LOG
#endif	    // DEBUG

/*
 * Parse r.e. @expr and convert it into postfix form.
 * Return the postfix string on success, NULL otherwise.
 */
    ساكن صحيح *
re2post(فراغ)
{
    אם (nfa_reg(REG_NOPAREN) == FAIL)
	החזר عدم;
    EMIT(NFA_MOPEN);
    החזר post_start;
}

// NB. Some of the code below is inspired by Russ's.

/*
 * Represents an NFA state plus zero or one or two arrows exiting.
 * if c == MATCH, no arrows out; matching state.
 * If c == SPLIT, unlabeled arrows to out and out1 (if != NULL).
 * If c < 256, labeled arrow with character c to out.
 */

ساكن nfa_state_T	*state_ptr; // points to nfa_prog->state

/*
 * Allocate and initialize nfa_state_T.
 */
    ساكن nfa_state_T *
alloc_state(صحيح c, nfa_state_T *out, nfa_state_T *out1)
{
    nfa_state_T *s;

    אם (istate >= nstate)
	החזר عدم;

    s = &state_ptr[istate++];

    s->c    = c;
    s->out  = out;
    s->out1 = out1;
    s->val  = 0;

    s->id   = istate;
    s->lastlist[0] = 0;
    s->lastlist[1] = 0;

    החזר s;
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
تعريفنوع اتحاد Ptrlist Ptrlist;
اتحاد Ptrlist
{
    Ptrlist	*next;
    nfa_state_T	*s;
};

بنية Frag
{
    nfa_state_T *start;
    Ptrlist	*out;
};
تعريفنوع بنية Frag Frag_T;

// Reused across compilations by post2nfa(); freed in free_regexp_stuff().
ساكن Frag_T *nfa_stack;
ساكن صحيح nfa_stack_len;

/*
 * Initialize a Frag_T struct and return it.
 */
    ساكن Frag_T
frag(nfa_state_T *start, Ptrlist *out)
{
    Frag_T n;

    n.start = start;
    n.out = out;
    החזר n;
}

/*
 * Create singleton list containing just outp.
 */
    ساكن Ptrlist *
list1(
    nfa_state_T	**outp)
{
    Ptrlist *l;

    l = (Ptrlist *)outp;
    l->next = عدم;
    החזר l;
}

/*
 * Patch the list of states at out to point to start.
 */
    ساكن فراغ
patch(Ptrlist *l, nfa_state_T *s)
{
    Ptrlist *next;

    עבור (; l; l = next)
    {
	next = l->next;
	l->s = s;
    }
}


/*
 * Join the two lists l1 and l2, returning the combination.
 */
    ساكن Ptrlist *
append(Ptrlist *l1, Ptrlist *l2)
{
    Ptrlist *oldl1;

    oldl1 = l1;
    כלעוד (l1->next)
	l1 = l1->next;
    l1->next = l2;
    החזר oldl1;
}

/*
 * Stack used for transforming postfix form into NFA.
 */
ساكن Frag_T empty;

    ساكن فراغ
st_error(صحيح *postfix UNUSED, صحيح *end UNUSED, صحيح *p UNUSED)
{
#ifdef NFA_REGEXP_ERROR_LOG
    FILE *df;
    صحيح *p2;

    df = fopen(NFA_REGEXP_ERROR_LOG, "a");
    אם (df)
    {
	fprintf(df, "Error popping the stack!\n");
# ifdef DEBUG
	fprintf(df, "Current regexp is \"%s\"\n", nfa_regengine.expr);
# endif
	fprintf(df, "Postfix form is: ");
# ifdef DEBUG
	עבור (p2 = postfix; p2 < end; p2++)
	{
	    nfa_set_code(*p2);
	    fprintf(df, "%s, ", code);
	}
	nfa_set_code(*p);
	fprintf(df, "\nCurrent position is: ");
	עבור (p2 = postfix; p2 <= p; p2 ++)
	{
	    nfa_set_code(*p2);
	    fprintf(df, "%s, ", code);
	}
# אחרת
	עבור (p2 = postfix; p2 < end; p2++)
	    fprintf(df, "%d, ", *p2);
	fprintf(df, "\nCurrent position is: ");
	עבור (p2 = postfix; p2 <= p; p2 ++)
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
    ساكن فراغ
st_push(Frag_T s, Frag_T **p, Frag_T *stack_end)
{
    Frag_T *stackp = *p;

    אם (stackp >= stack_end)
	החזר;
    *stackp = s;
    *p = *p + 1;
}

/*
 * Pop an item from the stack.
 */
    ساكن Frag_T
st_pop(Frag_T **p, Frag_T *stack)
{
    Frag_T *stackp;

    *p = *p - 1;
    stackp = *p;
    אם (stackp < stack)
	החזר empty;
    החזר **p;
}

/*
 * Estimate the maximum byte length of anything matching "state".
 * When unknown or unlimited return -1.
 */
    ساكن صحيح
nfa_max_width(nfa_state_T *startstate, صحيح depth)
{
    صحيح		    l, r;
    nfa_state_T	    *state = startstate;
    صحيح		    len = 0;

    // detect looping in a NFA_SPLIT
    אם (depth > 4)
	החזר -1;

    כלעוד (state != عدم)
    {
	بدل (state->c)
	{
	    حالة NFA_END_INVISIBLE:
	    حالة NFA_END_INVISIBLE_NEG:
		// the end, return what we have
		החזר len;

	    حالة NFA_SPLIT:
		// two alternatives, use the maximum
		l = nfa_max_width(state->out, depth + 1);
		r = nfa_max_width(state->out1, depth + 1);
		אם (l < 0 || r < 0)
		    החזר -1;
		החזר len + (l > r ? l : r);

	    حالة NFA_ANY:
	    حالة NFA_START_COLL:
	    حالة NFA_START_NEG_COLL:
		// matches some character, including composing chars
		אם (enc_utf8)
		    len += MB_MAXBYTES;
		אחרת אם (has_mbyte)
		    len += 2;
		אחרת
		    ++len;
		אם (state->c != NFA_ANY)
		{
		    // Skip over the compiled collection.
		    // malformed NFAs must not crash width estimation.
		    אם (state->out1 == عدم || state->out1->out == عدم)
			החזר -1;
		    state = state->out1->out;
		    استمر;
		}
		اكسر;

	    حالة NFA_DIGIT:
	    حالة NFA_WHITE:
	    حالة NFA_HEX:
	    حالة NFA_OCTAL:
		// ascii
		++len;
		اكسر;

	    حالة NFA_IDENT:
	    حالة NFA_SIDENT:
	    حالة NFA_KWORD:
	    حالة NFA_SKWORD:
	    حالة NFA_FNAME:
	    حالة NFA_SFNAME:
	    حالة NFA_PRINT:
	    حالة NFA_SPRINT:
	    حالة NFA_NWHITE:
	    حالة NFA_NDIGIT:
	    حالة NFA_NHEX:
	    حالة NFA_NOCTAL:
	    حالة NFA_WORD:
	    حالة NFA_NWORD:
	    حالة NFA_HEAD:
	    حالة NFA_NHEAD:
	    حالة NFA_ALPHA:
	    حالة NFA_NALPHA:
	    حالة NFA_LOWER:
	    حالة NFA_NLOWER:
	    حالة NFA_UPPER:
	    حالة NFA_NUPPER:
	    حالة NFA_LOWER_IC:
	    حالة NFA_NLOWER_IC:
	    حالة NFA_UPPER_IC:
	    حالة NFA_NUPPER_IC:
	    حالة NFA_ANY_COMPOSING:
		// possibly non-ascii
		אם (has_mbyte)
		    len += 3;
		אחרת
		    ++len;
		اكسر;

	    حالة NFA_START_INVISIBLE:
	    حالة NFA_START_INVISIBLE_NEG:
	    حالة NFA_START_INVISIBLE_BEFORE:
	    حالة NFA_START_INVISIBLE_BEFORE_NEG:
		// zero-width, out1 points to the END state
		state = state->out1->out;
		استمر;

	    حالة NFA_BACKREF1:
	    حالة NFA_BACKREF2:
	    حالة NFA_BACKREF3:
	    حالة NFA_BACKREF4:
	    حالة NFA_BACKREF5:
	    حالة NFA_BACKREF6:
	    حالة NFA_BACKREF7:
	    حالة NFA_BACKREF8:
	    حالة NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	    حالة NFA_ZREF1:
	    حالة NFA_ZREF2:
	    حالة NFA_ZREF3:
	    حالة NFA_ZREF4:
	    حالة NFA_ZREF5:
	    حالة NFA_ZREF6:
	    حالة NFA_ZREF7:
	    حالة NFA_ZREF8:
	    حالة NFA_ZREF9:
#endif
	    حالة NFA_NEWL:
	    حالة NFA_SKIP:
		// unknown width
		החזר -1;

	    حالة NFA_BOL:
	    حالة NFA_EOL:
	    حالة NFA_BOF:
	    حالة NFA_EOF:
	    حالة NFA_BOW:
	    حالة NFA_EOW:
	    حالة NFA_MOPEN:
	    حالة NFA_MOPEN1:
	    حالة NFA_MOPEN2:
	    حالة NFA_MOPEN3:
	    حالة NFA_MOPEN4:
	    حالة NFA_MOPEN5:
	    حالة NFA_MOPEN6:
	    حالة NFA_MOPEN7:
	    حالة NFA_MOPEN8:
	    حالة NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	    حالة NFA_ZOPEN:
	    حالة NFA_ZOPEN1:
	    حالة NFA_ZOPEN2:
	    حالة NFA_ZOPEN3:
	    حالة NFA_ZOPEN4:
	    حالة NFA_ZOPEN5:
	    حالة NFA_ZOPEN6:
	    حالة NFA_ZOPEN7:
	    حالة NFA_ZOPEN8:
	    حالة NFA_ZOPEN9:
	    حالة NFA_ZCLOSE:
	    حالة NFA_ZCLOSE1:
	    حالة NFA_ZCLOSE2:
	    حالة NFA_ZCLOSE3:
	    حالة NFA_ZCLOSE4:
	    حالة NFA_ZCLOSE5:
	    حالة NFA_ZCLOSE6:
	    حالة NFA_ZCLOSE7:
	    حالة NFA_ZCLOSE8:
	    حالة NFA_ZCLOSE9:
#endif
	    حالة NFA_MCLOSE:
	    حالة NFA_MCLOSE1:
	    حالة NFA_MCLOSE2:
	    حالة NFA_MCLOSE3:
	    حالة NFA_MCLOSE4:
	    حالة NFA_MCLOSE5:
	    حالة NFA_MCLOSE6:
	    حالة NFA_MCLOSE7:
	    حالة NFA_MCLOSE8:
	    حالة NFA_MCLOSE9:
	    حالة NFA_NOPEN:
	    حالة NFA_NCLOSE:

	    حالة NFA_LNUM_GT:
	    حالة NFA_LNUM_LT:
	    حالة NFA_COL_GT:
	    حالة NFA_COL_LT:
	    حالة NFA_VCOL_GT:
	    حالة NFA_VCOL_LT:
	    حالة NFA_MARK_GT:
	    حالة NFA_MARK_LT:
	    حالة NFA_VISUAL:
	    حالة NFA_LNUM:
	    حالة NFA_CURSOR:
	    حالة NFA_COL:
	    حالة NFA_VCOL:
	    حالة NFA_MARK:

	    حالة NFA_ZSTART:
	    حالة NFA_ZEND:
	    حالة NFA_OPT_CHARS:
	    حالة NFA_EMPTY:
	    حالة NFA_START_PATTERN:
	    حالة NFA_END_PATTERN:
	    حالة NFA_COMPOSING:
	    حالة NFA_END_COMPOSING:
		// zero-width
		اكسر;

	    افتراضي:
		אם (state->c < 0)
		    // don't know what this is
		    החזר -1;
		// normal character
		len += MB_CHAR2LEN(state->c);
		اكسر;
	}

	// normal way to continue
	state = state->out;
    }

    // unrecognized, "cannot happen"
    החזר -1;
}

/*
 * Convert a postfix form into its equivalent NFA.
 * Return the NFA start state on success, NULL otherwise.
 */
    ساكن nfa_state_T *
post2nfa(صحيح *postfix, صحيح *end, صحيح nfa_calc_size)
{
    صحيح		*p;
    صحيح		mopen;
    صحيح		mclose;
    Frag_T	*stack = عدم;
    Frag_T	*stackp = عدم;
    Frag_T	*stack_end = عدم;
    Frag_T	e1;
    Frag_T	e2;
    Frag_T	e;
    nfa_state_T	*s;
    nfa_state_T	*s1;
    nfa_state_T	*matchstate;
    nfa_state_T	*ret = عدم;

    אם (postfix == عدم)
	החזר عدم;

#define PUSH(s)	    st_push((s), &stackp, stack_end)
#define POP()	    st_pop(&stackp, stack);		\
		    אם (stackp < stack)			\
		    {					\
			st_error(postfix, end, p);	\
			החזר عدم;			\
		    }

    אם (nfa_calc_size == FALSE)
    {
	// Reuse the fragment stack across compilations, growing when needed;
	// it is freed in free_regexp_stuff().
	אם (nfa_stack == عدم || nfa_stack_len < nstate + 1)
	{
	    vim_free(nfa_stack);
	    nfa_stack = ALLOC_MULT(Frag_T, nstate + 1);
	    אם (nfa_stack == عدم)
	    {
		nfa_stack_len = 0;
		החזר عدم;
	    }
	    nfa_stack_len = nstate + 1;
	}
	stack = nfa_stack;
	stackp = stack;
	stack_end = stack + (nstate + 1);
    }

    עבור (p = postfix; p < end; ++p)
    {
	بدل (*p)
	{
	حالة NFA_CONCAT:
	    // Concatenation.
	    // Pay attention: this operator does not exist in the r.e. itself
	    // (it is implicit, really).  It is added when r.e. is translated
	    // to postfix form in re2post().
	    אם (nfa_calc_size == TRUE)
	    {
		// nstate += 0;
		اكسر;
	    }
	    e2 = POP();
	    e1 = POP();
	    patch(e1.out, e2.start);
	    PUSH(frag(e1.start, e2.out));
	    اكسر;

	حالة NFA_OR:
	    // Alternation
	    אם (nfa_calc_size == TRUE)
	    {
		nstate++;
		اكسر;
	    }
	    e2 = POP();
	    e1 = POP();
	    s = alloc_state(NFA_SPLIT, e1.start, e2.start);
	    אם (s == عدم)
		اذهب theend;
	    PUSH(frag(s, append(e1.out, e2.out)));
	    اكسر;

	حالة NFA_STAR:
	    // Zero or more, prefer more
	    אם (nfa_calc_size == TRUE)
	    {
		nstate++;
		اكسر;
	    }
	    e = POP();
	    s = alloc_state(NFA_SPLIT, e.start, عدم);
	    אם (s == عدم)
		اذهب theend;
	    patch(e.out, s);
	    PUSH(frag(s, list1(&s->out1)));
	    اكسر;

	حالة NFA_STAR_NONGREEDY:
	    // Zero or more, prefer zero
	    אם (nfa_calc_size == TRUE)
	    {
		nstate++;
		اكسر;
	    }
	    e = POP();
	    s = alloc_state(NFA_SPLIT, عدم, e.start);
	    אם (s == عدم)
		اذهب theend;
	    patch(e.out, s);
	    PUSH(frag(s, list1(&s->out)));
	    اكسر;

	حالة NFA_QUEST:
	    // one or zero atoms=> greedy match
	    אם (nfa_calc_size == TRUE)
	    {
		nstate++;
		اكسر;
	    }
	    e = POP();
	    s = alloc_state(NFA_SPLIT, e.start, عدم);
	    אם (s == عدم)
		اذهب theend;
	    PUSH(frag(s, append(e.out, list1(&s->out1))));
	    اكسر;

	حالة NFA_QUEST_NONGREEDY:
	    // zero or one atoms => non-greedy match
	    אם (nfa_calc_size == TRUE)
	    {
		nstate++;
		اكسر;
	    }
	    e = POP();
	    s = alloc_state(NFA_SPLIT, عدم, e.start);
	    אם (s == عدم)
		اذهب theend;
	    PUSH(frag(s, append(e.out, list1(&s->out))));
	    اكسر;

	حالة NFA_END_COLL:
	حالة NFA_END_NEG_COLL:
	    // On the stack is the sequence starting with NFA_START_COLL or
	    // NFA_START_NEG_COLL and all possible characters. Patch it to
	    // add the output to the start.
	    אם (nfa_calc_size == TRUE)
	    {
		nstate++;
		اكسر;
	    }
	    e = POP();
	    s = alloc_state(NFA_END_COLL, عدم, عدم);
	    אם (s == عدم)
		اذهب theend;
	    patch(e.out, s);
	    e.start->out1 = s;
	    PUSH(frag(e.start, list1(&s->out)));
	    اكسر;

	حالة NFA_RANGE:
	    // Before this are two characters, the low and high end of a
	    // range.  Turn them into two states with MIN and MAX.
	    אם (nfa_calc_size == TRUE)
	    {
		// nstate += 0;
		اكسر;
	    }
	    e2 = POP();
	    e1 = POP();
	    e2.start->val = e2.start->c;
	    e2.start->c = NFA_RANGE_MAX;
	    e1.start->val = e1.start->c;
	    e1.start->c = NFA_RANGE_MIN;
	    patch(e1.out, e2.start);
	    PUSH(frag(e1.start, e2.out));
	    اكسر;

	حالة NFA_EMPTY:
	    // 0-length, used in a repetition with max/min count of 0
	    אם (nfa_calc_size == TRUE)
	    {
		nstate++;
		اكسر;
	    }
	    s = alloc_state(NFA_EMPTY, عدم, عدم);
	    אם (s == عدم)
		اذهب theend;
	    PUSH(frag(s, list1(&s->out)));
	    اكسر;

	حالة NFA_OPT_CHARS:
	  {
	    صحيح    n;

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
	    אם (nfa_calc_size == TRUE)
	    {
		nstate += n;
		اكسر;
	    }
	    s = عدم; // avoid compiler warning
	    e1.out = عدم; // stores list with out1's
	    s1 = عدم; // previous NFA_SPLIT to connect to
	    כלעוד (n-- > 0)
	    {
		e = POP(); // get character
		s = alloc_state(NFA_SPLIT, e.start, عدم);
		אם (s == عدم)
		    اذهب theend;
		אם (e1.out == عدم)
		    e1 = e;
		patch(e.out, s1);
		append(e1.out, list1(&s->out1));
		s1 = s;
	    }
	    PUSH(frag(s, e1.out));
	    اكسر;
	  }

	حالة NFA_PREV_ATOM_NO_WIDTH:
	حالة NFA_PREV_ATOM_NO_WIDTH_NEG:
	حالة NFA_PREV_ATOM_JUST_BEFORE:
	حالة NFA_PREV_ATOM_JUST_BEFORE_NEG:
	حالة NFA_PREV_ATOM_LIKE_PATTERN:
	  {
	    صحيح before = (*p == NFA_PREV_ATOM_JUST_BEFORE
				      || *p == NFA_PREV_ATOM_JUST_BEFORE_NEG);
	    صحيح pattern = (*p == NFA_PREV_ATOM_LIKE_PATTERN);
	    صحيح start_state;
	    صحيح end_state;
	    صحيح n = 0;
	    nfa_state_T *zend;
	    nfa_state_T *skip;

	    بدل (*p)
	    {
		حالة NFA_PREV_ATOM_NO_WIDTH:
		    start_state = NFA_START_INVISIBLE;
		    end_state = NFA_END_INVISIBLE;
		    اكسر;
		حالة NFA_PREV_ATOM_NO_WIDTH_NEG:
		    start_state = NFA_START_INVISIBLE_NEG;
		    end_state = NFA_END_INVISIBLE_NEG;
		    اكسر;
		حالة NFA_PREV_ATOM_JUST_BEFORE:
		    start_state = NFA_START_INVISIBLE_BEFORE;
		    end_state = NFA_END_INVISIBLE;
		    اكسر;
		حالة NFA_PREV_ATOM_JUST_BEFORE_NEG:
		    start_state = NFA_START_INVISIBLE_BEFORE_NEG;
		    end_state = NFA_END_INVISIBLE_NEG;
		    اكسر;
		افتراضي: // NFA_PREV_ATOM_LIKE_PATTERN:
		    start_state = NFA_START_PATTERN;
		    end_state = NFA_END_PATTERN;
		    اكسر;
	    }

	    אם (before)
		n = *++p; // get the count

	    // The \@= operator: match the preceding atom with zero width.
	    // The \@! operator: no match for the preceding atom.
	    // The \@<= operator: match for the preceding atom.
	    // The \@<! operator: no match for the preceding atom.
	    // Surrounds the preceding atom with START_INVISIBLE and
	    // END_INVISIBLE, similarly to MOPEN.

	    אם (nfa_calc_size == TRUE)
	    {
		nstate += pattern ? 4 : 2;
		اكسر;
	    }
	    e = POP();
	    s1 = alloc_state(end_state, عدم, عدم);
	    אם (s1 == عدم)
		اذهب theend;

	    s = alloc_state(start_state, e.start, s1);
	    אם (s == عدم)
		اذهب theend;
	    אם (pattern)
	    {
		// NFA_ZEND -> NFA_END_PATTERN -> NFA_SKIP -> what follows.
		skip = alloc_state(NFA_SKIP, عدم, عدم);
		אם (skip == عدم)
		    اذهب theend;
		zend = alloc_state(NFA_ZEND, s1, عدم);
		אם (zend == عدم)
		    اذهب theend;
		s1->out= skip;
		patch(e.out, zend);
		PUSH(frag(s, list1(&skip->out)));
	    }
	    אחרת
	    {
		patch(e.out, s1);
		PUSH(frag(s, list1(&s1->out)));
		אם (before)
		{
		    אם (n <= 0)
			// See if we can guess the maximum width, it avoids a
			// lot of pointless tries.
			n = nfa_max_width(e.start, 0);
		    s->val = n; // store the count
		}
	    }
	    اكسر;
	  }

	حالة NFA_COMPOSING:	// char with composing char
#אם 0
	    // TODO
	    אם (regflags & RF_ICOMBINE)
	    {
		// use the base character only
	    }
#endif
	    // FALLTHROUGH

	حالة NFA_MOPEN:	// \( \) Submatch
	حالة NFA_MOPEN1:
	حالة NFA_MOPEN2:
	حالة NFA_MOPEN3:
	حالة NFA_MOPEN4:
	حالة NFA_MOPEN5:
	حالة NFA_MOPEN6:
	حالة NFA_MOPEN7:
	حالة NFA_MOPEN8:
	حالة NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	حالة NFA_ZOPEN:	// \z( \) Submatch
	حالة NFA_ZOPEN1:
	حالة NFA_ZOPEN2:
	حالة NFA_ZOPEN3:
	حالة NFA_ZOPEN4:
	حالة NFA_ZOPEN5:
	حالة NFA_ZOPEN6:
	حالة NFA_ZOPEN7:
	حالة NFA_ZOPEN8:
	حالة NFA_ZOPEN9:
#endif
	حالة NFA_NOPEN:	// \%( \) "Invisible Submatch"
	    אם (nfa_calc_size == TRUE)
	    {
		nstate += 2;
		اكسر;
	    }

	    mopen = *p;
	    بدل (*p)
	    {
		حالة NFA_NOPEN: mclose = NFA_NCLOSE; اكسر;
#ifdef FEAT_SYN_HL
		حالة NFA_ZOPEN: mclose = NFA_ZCLOSE; اكسر;
		حالة NFA_ZOPEN1: mclose = NFA_ZCLOSE1; اكسر;
		حالة NFA_ZOPEN2: mclose = NFA_ZCLOSE2; اكسر;
		حالة NFA_ZOPEN3: mclose = NFA_ZCLOSE3; اكسر;
		حالة NFA_ZOPEN4: mclose = NFA_ZCLOSE4; اكسر;
		حالة NFA_ZOPEN5: mclose = NFA_ZCLOSE5; اكسر;
		حالة NFA_ZOPEN6: mclose = NFA_ZCLOSE6; اكسر;
		حالة NFA_ZOPEN7: mclose = NFA_ZCLOSE7; اكسر;
		حالة NFA_ZOPEN8: mclose = NFA_ZCLOSE8; اكسر;
		حالة NFA_ZOPEN9: mclose = NFA_ZCLOSE9; اكسر;
#endif
		حالة NFA_COMPOSING: mclose = NFA_END_COMPOSING; اكسر;
		افتراضي:
		    // NFA_MOPEN, NFA_MOPEN1 .. NFA_MOPEN9
		    mclose = *p + NSUBEXP;
		    اكسر;
	    }

	    // Allow "NFA_MOPEN" as a valid postfix representation for
	    // the empty regexp "". In this case, the NFA will be
	    // NFA_MOPEN -> NFA_MCLOSE. Note that this also allows
	    // empty groups of parenthesis, and empty mbyte chars
	    אם (stackp == stack)
	    {
		s = alloc_state(mopen, عدم, عدم);
		אם (s == عدم)
		    اذهب theend;
		s1 = alloc_state(mclose, عدم, عدم);
		אם (s1 == عدم)
		    اذهب theend;
		patch(list1(&s->out), s1);
		PUSH(frag(s, list1(&s1->out)));
		اكسر;
	    }

	    // At least one node was emitted before NFA_MOPEN, so
	    // at least one node will be between NFA_MOPEN and NFA_MCLOSE
	    e = POP();
	    s = alloc_state(mopen, e.start, عدم);   // `('
	    אם (s == عدم)
		اذهب theend;

	    s1 = alloc_state(mclose, عدم, عدم);   // `)'
	    אם (s1 == عدم)
		اذهب theend;
	    patch(e.out, s1);

	    אם (mopen == NFA_COMPOSING)
		// COMPOSING->out1 = END_COMPOSING
		patch(list1(&s->out1), s1);

	    PUSH(frag(s, list1(&s1->out)));
	    اكسر;

	حالة NFA_BACKREF1:
	حالة NFA_BACKREF2:
	حالة NFA_BACKREF3:
	حالة NFA_BACKREF4:
	حالة NFA_BACKREF5:
	حالة NFA_BACKREF6:
	حالة NFA_BACKREF7:
	حالة NFA_BACKREF8:
	حالة NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	حالة NFA_ZREF1:
	حالة NFA_ZREF2:
	حالة NFA_ZREF3:
	حالة NFA_ZREF4:
	حالة NFA_ZREF5:
	حالة NFA_ZREF6:
	حالة NFA_ZREF7:
	حالة NFA_ZREF8:
	حالة NFA_ZREF9:
#endif
	    אם (nfa_calc_size == TRUE)
	    {
		nstate += 2;
		اكسر;
	    }
	    s = alloc_state(*p, عدم, عدم);
	    אם (s == عدم)
		اذهب theend;
	    s1 = alloc_state(NFA_SKIP, عدم, عدم);
	    אם (s1 == عدم)
		اذهب theend;
	    patch(list1(&s->out), s1);
	    PUSH(frag(s, list1(&s1->out)));
	    اكسر;

	حالة NFA_LNUM:
	حالة NFA_LNUM_GT:
	حالة NFA_LNUM_LT:
	حالة NFA_VCOL:
	حالة NFA_VCOL_GT:
	حالة NFA_VCOL_LT:
	حالة NFA_COL:
	حالة NFA_COL_GT:
	حالة NFA_COL_LT:
	حالة NFA_MARK:
	حالة NFA_MARK_GT:
	حالة NFA_MARK_LT:
	  {
	    صحيح n = *++p; // lnum, col or mark name

	    אם (nfa_calc_size == TRUE)
	    {
		nstate += 1;
		اكسر;
	    }
	    s = alloc_state(p[-1], عدم, عدم);
	    אם (s == عدم)
		اذهب theend;
	    s->val = n;
	    PUSH(frag(s, list1(&s->out)));
	    اكسر;
	  }

	حالة NFA_ZSTART:
	حالة NFA_ZEND:
	افتراضي:
	    // Operands
	    אם (nfa_calc_size == TRUE)
	    {
		nstate++;
		اكسر;
	    }
	    s = alloc_state(*p, عدم, عدم);
	    אם (s == عدم)
		اذهب theend;
	    PUSH(frag(s, list1(&s->out)));
	    اكسر;

	} // switch(*p)

    } // for (p = postfix; *p; ++p)

    אם (nfa_calc_size == TRUE)
    {
	nstate++;
	اذهب theend;	// Return value when counting size is ignored anyway
    }

    e = POP();
    אם (stackp != stack)
	EMSG_RET_NULL(_(e_nfa_regexp_while_converting_from_postfix_to_nfa_too_many_stats_left_on_stack));

    אם (istate >= nstate)
	EMSG_RET_NULL(_(e_nfa_regexp_not_enough_space_to_store_whole_nfa));

    matchstate = &state_ptr[istate++]; // the match state
    matchstate->c = NFA_MATCH;
    matchstate->out = matchstate->out1 = عدم;
    matchstate->id = 0;

    patch(e.out, matchstate);
    ret = e.start;

theend:
    החזר ret;

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
    ساكن فراغ
nfa_postprocess(nfa_regprog_T *prog)
{
    صحيح i;
    صحيح c;

    עבור (i = 0; i < prog->nstate; ++i)
    {
	c = prog->state[i].c;
	אם (c == NFA_START_INVISIBLE
		|| c == NFA_START_INVISIBLE_NEG
		|| c == NFA_START_INVISIBLE_BEFORE
		|| c == NFA_START_INVISIBLE_BEFORE_NEG)
	{
	    صحيح directly;

	    // Do it directly when what follows is possibly the end of the
	    // match.
	    אם (match_follows(prog->state[i].out1->out, 0))
		directly = TRUE;
	    אחרת
	    {
		صحيح ch_invisible = failure_chance(prog->state[i].out, 0);
		صحيح ch_follows = failure_chance(prog->state[i].out1->out, 0);

		// Postpone when the invisible match is expensive or has a
		// lower chance of failing.
		אם (c == NFA_START_INVISIBLE_BEFORE
		     || c == NFA_START_INVISIBLE_BEFORE_NEG)
		{
		    // "before" matches are very expensive when
		    // unbounded, always prefer what follows then,
		    // unless what follows will always match.
		    // Otherwise strongly prefer what follows.
		    אם (prog->state[i].val <= 0 && ch_follows > 0)
			directly = FALSE;
		    אחרת
			directly = ch_follows * 10 < ch_invisible;
		}
		אחרת
		{
		    // normal invisible, first do the one with the
		    // highest failure chance
		    directly = ch_follows < ch_invisible;
		}
	    }
	    אם (directly)
		// switch to the _FIRST state
		++prog->state[i].c;
	}
    }
}

/////////////////////////////////////////////////////////////////
// NFA execution code.
/////////////////////////////////////////////////////////////////

تعريفنوع بنية
{
    صحيح	    in_use; // number of subexpr with useful info

    // When REG_MULTI is TRUE list.multi is used, otherwise list.line.
    اتحاد
    {
	بنية multipos
	{
	    linenr_T	start_lnum;
	    linenr_T	end_lnum;
	    colnr_T	start_col;
	    colnr_T	end_col;
	} multi[NSUBEXP];
	بنية linepos
	{
	    char_u	*start;
	    char_u	*end;
	} line[NSUBEXP];
    } list;
    colnr_T	orig_start_col;  // list.multi[0].start_col without \zs
} regsub_T;

تعريفنوع بنية
{
    regsub_T	norm; // \( .. \) matches
#ifdef FEAT_SYN_HL
    regsub_T	synt; // \z( .. \) matches
#endif
} regsubs_T;

// nfa_pim_T stores a Postponed Invisible Match.
تعريفنوع بنية nfa_pim_S nfa_pim_T;
بنية nfa_pim_S
{
    صحيح		result;		// NFA_PIM_*, see below
    nfa_state_T	*state;		// the invisible match start state
    regsubs_T	subs;		// submatch info, only party used
    اتحاد
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
تعريفنوع بنية
{
    nfa_state_T	*state;
    صحيح		count;
    nfa_pim_T	pim;		// if pim.result != NFA_PIM_UNUSED: postponed
				// invisible match
    regsubs_T	subs;		// submatch info, only party used
} nfa_thread_T;

// nfa_list_T contains the alternative NFA execution states.
تعريفنوع بنية
{
    nfa_thread_T    *t;		// allocated array of states
    صحيح		    n;		// nr of states currently in "t"
    صحيح		    len;	// max nr of states in "t"
    صحيح		    id;		// ID of the list
    صحيح		    has_pim;	// TRUE when any state has a PIM
} nfa_list_T;

#ifdef ENABLE_LOG
ساكن فراغ log_subexpr(regsub_T *sub);

    ساكن فراغ
log_subsexpr(regsubs_T *subs)
{
    log_subexpr(&subs->norm);
# ifdef FEAT_SYN_HL
    אם (rex.nfa_has_zsubexpr)
	log_subexpr(&subs->synt);
# endif
}

    ساكن فراغ
log_subexpr(regsub_T *sub)
{
    صحيح j;

    עבור (j = 0; j < sub->in_use; j++)
	אם (REG_MULTI)
	    fprintf(log_fd,
		    "*** group %d, start: c=%d, l=%d, end: c=%d, l=%d\n",
		    j,
		    sub->list.multi[j].start_col,
		    (صحيح)sub->list.multi[j].start_lnum,
		    sub->list.multi[j].end_col,
		    (صحيح)sub->list.multi[j].end_lnum);
	אחרת
	{
	    حرف *s = (حرف *)sub->list.line[j].start;
	    حرف *e = (حرف *)sub->list.line[j].end;

	    fprintf(log_fd, "*** group %d, start: \"%s\", end: \"%s\"\n",
		    j,
		    s == عدم ? "NULL" : s,
		    e == عدم ? "NULL" : e);
	}
}

    ساكن حرف *
pim_info(nfa_pim_T *pim)
{
    ساكن حرف buf[30];

    אם (pim == عدم || pim->result == NFA_PIM_UNUSED)
	buf[0] = NUL;
    אחרת
    {
	sprintf(buf, " PIM col %d", REG_MULTI ? (صحيح)pim->end.pos.col
		: (صحيح)(pim->end.ptr - rex.input));
    }
    החזר buf;
}

#endif

// Used during execution: whether a match has been found.
ساكن صحيح	    nfa_match;
#ifdef FEAT_RELTIME
ساكن صحيح	   *nfa_timed_out;
#endif

ساكن مضمن فراغ copy_sub(regsub_T *to, regsub_T *from);
ساكن صحيح pim_equal(nfa_pim_T *one, nfa_pim_T *two);

/*
 * Copy postponed invisible match info from "from" to "to".
 */
    ساكن فراغ
copy_pim(nfa_pim_T *to, nfa_pim_T *from)
{
    to->result = from->result;
    to->state = from->state;
    copy_sub(&to->subs.norm, &from->subs.norm);
#ifdef FEAT_SYN_HL
    אם (rex.nfa_has_zsubexpr)
	copy_sub(&to->subs.synt, &from->subs.synt);
#endif
    to->end = from->end;
}

    ساكن فراغ
clear_sub(regsub_T *sub)
{
    אם (REG_MULTI)
	// Use 0xff to set lnum to -1
	vim_memset(sub->list.multi, 0xff,
				  حجم(بنية multipos) * rex.nfa_nsubexpr);
    אחרת
	vim_memset(sub->list.line, 0,
				   حجم(بنية linepos) * rex.nfa_nsubexpr);
    sub->in_use = 0;
}

/*
 * Copy the submatches from "from" to "to".
 */
    ساكن مضمن فراغ
copy_sub(regsub_T *to, regsub_T *from)
{
    to->in_use = from->in_use;
    אם (from->in_use <= 0)
	החזר;

    // Copy the match start and end positions.  addstate() calls this for
    // every state it adds, so it is very hot.  Patterns without capturing
    // groups only use subexpression zero (the whole match), so special-case
    // "in_use == 1" with a direct assignment to avoid the mch_memmove() call.
    אם (REG_MULTI)
    {
	אם (from->in_use == 1)
	    to->list.multi[0] = from->list.multi[0];
	אחרת
	    mch_memmove(&to->list.multi[0],
		    &from->list.multi[0],
		    حجم(بنية multipos) * from->in_use);
	to->orig_start_col = from->orig_start_col;
    }
    אחרת
    {
	אם (from->in_use == 1)
	    to->list.line[0] = from->list.line[0];
	אחרת
	    mch_memmove(&to->list.line[0],
		    &from->list.line[0],
		    حجم(بنية linepos) * from->in_use);
    }
}

/*
 * Like copy_sub() but exclude the main match.
 */
    ساكن فراغ
copy_sub_off(regsub_T *to, regsub_T *from)
{
    אם (to->in_use < from->in_use)
	to->in_use = from->in_use;
    אם (from->in_use <= 1)
	החזר;

    // Copy the match start and end positions.
    אם (REG_MULTI)
	mch_memmove(&to->list.multi[1],
		&from->list.multi[1],
		حجم(بنية multipos) * (from->in_use - 1));
    אחרת
	mch_memmove(&to->list.line[1],
		&from->list.line[1],
		حجم(بنية linepos) * (from->in_use - 1));
}

/*
 * Like copy_sub() but only do the end of the main match if \ze is present.
 */
    ساكن فراغ
copy_ze_off(regsub_T *to, regsub_T *from)
{
    אם (!rex.nfa_has_zend)
	החזר;

    אם (REG_MULTI)
    {
	אם (from->list.multi[0].end_lnum >= 0)
	{
	    to->list.multi[0].end_lnum = from->list.multi[0].end_lnum;
	    to->list.multi[0].end_col = from->list.multi[0].end_col;
	}
    }
    אחרת
    {
	אם (from->list.line[0].end != عدم)
	    to->list.line[0].end = from->list.line[0].end;
    }
}

/*
 * Return TRUE if "sub1" and "sub2" have the same start positions.
 * When using back-references also check the end position.
 */
    ساكن صحيح
sub_equal(regsub_T *sub1, regsub_T *sub2)
{
    صحيح		i;
    صحيح		todo;
    linenr_T	s1;
    linenr_T	s2;
    char_u	*sp1;
    char_u	*sp2;

    todo = sub1->in_use > sub2->in_use ? sub1->in_use : sub2->in_use;
    אם (REG_MULTI)
    {
	עבור (i = 0; i < todo; ++i)
	{
	    אם (i < sub1->in_use)
		s1 = sub1->list.multi[i].start_lnum;
	    אחרת
		s1 = -1;
	    אם (i < sub2->in_use)
		s2 = sub2->list.multi[i].start_lnum;
	    אחרת
		s2 = -1;
	    אם (s1 != s2)
		החזר FALSE;
	    אם (s1 != -1 && sub1->list.multi[i].start_col
					     != sub2->list.multi[i].start_col)
		החזר FALSE;

	    אם (rex.nfa_has_backref)
	    {
		אם (i < sub1->in_use)
		    s1 = sub1->list.multi[i].end_lnum;
		אחרת
		    s1 = -1;
		אם (i < sub2->in_use)
		    s2 = sub2->list.multi[i].end_lnum;
		אחרת
		    s2 = -1;
		אם (s1 != s2)
		    החזר FALSE;
		אם (s1 != -1 && sub1->list.multi[i].end_col
					       != sub2->list.multi[i].end_col)
		החזר FALSE;
	    }
	}
    }
    אחרת
    {
	עבור (i = 0; i < todo; ++i)
	{
	    אם (i < sub1->in_use)
		sp1 = sub1->list.line[i].start;
	    אחרת
		sp1 = عدم;
	    אם (i < sub2->in_use)
		sp2 = sub2->list.line[i].start;
	    אחרת
		sp2 = عدم;
	    אם (sp1 != sp2)
		החזר FALSE;
	    אם (rex.nfa_has_backref)
	    {
		אם (i < sub1->in_use)
		    sp1 = sub1->list.line[i].end;
		אחרת
		    sp1 = عدم;
		אם (i < sub2->in_use)
		    sp2 = sub2->list.line[i].end;
		אחרת
		    sp2 = عدم;
		אם (sp1 != sp2)
		    החזר FALSE;
	    }
	}
    }

    החזר TRUE;
}

#ifdef FEAT_RELTIME
/*
 * Check if we are past the time limit, if there is one.
 */
    ساكن صحيح
nfa_did_time_out(فراغ)
{
    אם (*timeout_flag)
    {
	אם (nfa_timed_out != عدم)
	{
# ifdef FEAT_EVAL
	    אם (!*nfa_timed_out)
		ch_log(عدم, "NFA regexp timed out");
# endif
	    *nfa_timed_out = TRUE;
	}
	החזר TRUE;
    }
    החזר FALSE;
}
#endif

#ifdef ENABLE_LOG
    ساكن فراغ
open_debug_log(صحيح result)
{
    log_fd = fopen(NFA_REGEXP_RUN_LOG, "a");
    אם (log_fd == عدم)
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

    ساكن فراغ
report_state(حرف *action,
	     regsub_T *sub,
	     nfa_state_T *state,
	     صحيح lid,
	     nfa_pim_T *pim)
{
    صحيح col;

    אם (sub->in_use <= 0)
	col = -1;
    אחרת אם (REG_MULTI)
	col = sub->list.multi[0].start_col;
    אחרת
	col = (صحيح)(sub->list.line[0].start - rex.line);
    nfa_set_code(state->c);
    אם (log_fd == عدم)
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
    ساكن صحيح
has_state_with_pos(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim)	// postponed match or NULL
{
    nfa_thread_T	*thread;
    صحيح			i;

    עבור (i = 0; i < l->n; ++i)
    {
	thread = &l->t[i];
	אם (thread->state->id == state->id
		&& sub_equal(&thread->subs.norm, &subs->norm)
#ifdef FEAT_SYN_HL
		&& (!rex.nfa_has_zsubexpr
				|| sub_equal(&thread->subs.synt, &subs->synt))
#endif
		&& pim_equal(&thread->pim, pim))
	    החזר TRUE;
    }
    החזר FALSE;
}

/*
 * Return TRUE if "one" and "two" are equal.  That includes when both are not
 * set.
 */
    ساكن صحيح
pim_equal(nfa_pim_T *one, nfa_pim_T *two)
{
    صحيح one_unused = (one == عدم || one->result == NFA_PIM_UNUSED);
    صحيح two_unused = (two == عدم || two->result == NFA_PIM_UNUSED);

    אם (one_unused)
	// one is unused: equal when two is also unused
	החזר two_unused;
    אם (two_unused)
	// one is used and two is not: not equal
	החזר FALSE;
    // compare the state id
    אם (one->state->id != two->state->id)
	החזר FALSE;
    // compare the position
    אם (REG_MULTI)
	החזר one->end.pos.lnum == two->end.pos.lnum
	    && one->end.pos.col == two->end.pos.col;
    החזר one->end.ptr == two->end.ptr;
}

/*
 * Return TRUE if "state" leads to a NFA_MATCH without advancing the input.
 */
    ساكن صحيح
match_follows(nfa_state_T *startstate, صحيح depth)
{
    nfa_state_T	    *state = startstate;

    // avoid too much recursion
    אם (depth > 10)
	החזר FALSE;

    כלעוד (state != عدم)
    {
	بدل (state->c)
	{
	    حالة NFA_MATCH:
	    حالة NFA_MCLOSE:
	    حالة NFA_END_INVISIBLE:
	    حالة NFA_END_INVISIBLE_NEG:
	    حالة NFA_END_PATTERN:
		החזר TRUE;

	    حالة NFA_SPLIT:
		החזר match_follows(state->out, depth + 1)
				     || match_follows(state->out1, depth + 1);

	    حالة NFA_START_INVISIBLE:
	    حالة NFA_START_INVISIBLE_FIRST:
	    حالة NFA_START_INVISIBLE_BEFORE:
	    حالة NFA_START_INVISIBLE_BEFORE_FIRST:
	    حالة NFA_START_INVISIBLE_NEG:
	    حالة NFA_START_INVISIBLE_NEG_FIRST:
	    حالة NFA_START_INVISIBLE_BEFORE_NEG:
	    حالة NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
	    حالة NFA_COMPOSING:
		// skip ahead to next state
		state = state->out1->out;
		استمر;

	    حالة NFA_ANY:
	    حالة NFA_ANY_COMPOSING:
	    حالة NFA_IDENT:
	    حالة NFA_SIDENT:
	    حالة NFA_KWORD:
	    حالة NFA_SKWORD:
	    حالة NFA_FNAME:
	    حالة NFA_SFNAME:
	    حالة NFA_PRINT:
	    حالة NFA_SPRINT:
	    حالة NFA_WHITE:
	    حالة NFA_NWHITE:
	    حالة NFA_DIGIT:
	    حالة NFA_NDIGIT:
	    حالة NFA_HEX:
	    حالة NFA_NHEX:
	    حالة NFA_OCTAL:
	    حالة NFA_NOCTAL:
	    حالة NFA_WORD:
	    حالة NFA_NWORD:
	    حالة NFA_HEAD:
	    حالة NFA_NHEAD:
	    حالة NFA_ALPHA:
	    حالة NFA_NALPHA:
	    حالة NFA_LOWER:
	    حالة NFA_NLOWER:
	    حالة NFA_UPPER:
	    حالة NFA_NUPPER:
	    حالة NFA_LOWER_IC:
	    حالة NFA_NLOWER_IC:
	    حالة NFA_UPPER_IC:
	    حالة NFA_NUPPER_IC:
	    حالة NFA_START_COLL:
	    حالة NFA_START_NEG_COLL:
	    حالة NFA_NEWL:
		// state will advance input
		החזר FALSE;

	    افتراضي:
		אם (state->c > 0)
		    // state will advance input
		    החזר FALSE;

		// Others: zero-width or possibly zero-width, might still find
		// a match at the same position, keep looking.
		اكسر;
	}
	state = state->out;
    }
    החזר FALSE;
}


/*
 * Return TRUE if "state" is already in list "l".
 */
    ساكن صحيح
state_in_list(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs)	// pointers to subexpressions
{
    אם (state->lastlist[nfa_ll_index] == l->id)
    {
	אם (!rex.nfa_has_backref || has_state_with_pos(l, state, subs, عدم))
	    החזר TRUE;
    }
    החזר FALSE;
}

// Offset used for "off" by addstate_here().
#define ADDSTATE_HERE_OFFSET 10

/*
 * Add "state" and possibly what follows to state list ".".
 * Returns "subs_arg", possibly copied into temp_subs.
 * Returns NULL when recursiveness is too deep or timed out.
 */
    ساكن regsubs_T *
addstate(
    nfa_list_T		*l,	    // runtime state list
    nfa_state_T		*state,	    // state to update
    regsubs_T		*subs_arg,  // pointers to subexpressions
    nfa_pim_T		*pim,	    // postponed look-behind match
    صحيح			off_arg,    // byte offset, when -1 go to next line
    صحيح			depth)	    // recursion depth
{
    صحيح			subidx;
    صحيح			off = off_arg;
    صحيح			add_here = FALSE;
    صحيح			listindex = 0;
    صحيح			k;
    صحيح			found = FALSE;
    nfa_thread_T	*thread;
    بنية multipos	save_multipos;
    صحيح			save_in_use;
    char_u		*save_ptr;
    صحيح			i;
    regsub_T		*sub;
    regsubs_T		*subs = subs_arg;
    ساكن regsubs_T	temp_subs;
#ifdef ENABLE_LOG
    صحيح			did_print = FALSE;
#endif

#ifdef FEAT_RELTIME
    אם (nfa_did_time_out())
	החזר عدم;
#endif

    // This function is called recursively.  When the depth is too much we run
    // out of stack and crash, limit recursiveness here.
    אם (depth >= 5000 || subs == عدم)
	החזר عدم;

    אם (off_arg <= -ADDSTATE_HERE_OFFSET)
    {
	add_here = TRUE;
	off = 0;
	listindex = -(off_arg + ADDSTATE_HERE_OFFSET);
    }

    بدل (state->c)
    {
	حالة NFA_NCLOSE:
	حالة NFA_MCLOSE:
	حالة NFA_MCLOSE1:
	حالة NFA_MCLOSE2:
	حالة NFA_MCLOSE3:
	حالة NFA_MCLOSE4:
	حالة NFA_MCLOSE5:
	حالة NFA_MCLOSE6:
	حالة NFA_MCLOSE7:
	حالة NFA_MCLOSE8:
	حالة NFA_MCLOSE9:
#ifdef FEAT_SYN_HL
	حالة NFA_ZCLOSE:
	حالة NFA_ZCLOSE1:
	حالة NFA_ZCLOSE2:
	حالة NFA_ZCLOSE3:
	حالة NFA_ZCLOSE4:
	حالة NFA_ZCLOSE5:
	حالة NFA_ZCLOSE6:
	حالة NFA_ZCLOSE7:
	حالة NFA_ZCLOSE8:
	حالة NFA_ZCLOSE9:
#endif
	حالة NFA_MOPEN:
	حالة NFA_ZEND:
	حالة NFA_SPLIT:
	حالة NFA_EMPTY:
	    // These nodes are not added themselves but their "out" and/or
	    // "out1" may be added below.
	    اكسر;

	حالة NFA_BOL:
	حالة NFA_BOF:
	    // "^" won't match past end-of-line, don't bother trying.
	    // Except when at the end of the line, or when we are going to the
	    // next line for a look-behind match.
	    אם (rex.input > rex.line
		    && *rex.input != NUL
		    && (nfa_endp == عدم
			|| !REG_MULTI
			|| rex.lnum == nfa_endp->se_u.pos.lnum))
		اذهب skip_add;
	    // FALLTHROUGH

	حالة NFA_MOPEN1:
	حالة NFA_MOPEN2:
	حالة NFA_MOPEN3:
	حالة NFA_MOPEN4:
	حالة NFA_MOPEN5:
	حالة NFA_MOPEN6:
	حالة NFA_MOPEN7:
	حالة NFA_MOPEN8:
	حالة NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	حالة NFA_ZOPEN:
	حالة NFA_ZOPEN1:
	حالة NFA_ZOPEN2:
	حالة NFA_ZOPEN3:
	حالة NFA_ZOPEN4:
	حالة NFA_ZOPEN5:
	حالة NFA_ZOPEN6:
	حالة NFA_ZOPEN7:
	حالة NFA_ZOPEN8:
	حالة NFA_ZOPEN9:
#endif
	حالة NFA_NOPEN:
	حالة NFA_ZSTART:
	    // These nodes need to be added so that we can bail out when it
	    // was added to this list before at the same position to avoid an
	    // endless loop for "\(\)*"

	افتراضي:
	    אם (state->lastlist[nfa_ll_index] == l->id && state->c != NFA_SKIP)
	    {
		// This state is already in the list, don't add it again,
		// unless it is an MOPEN that is used for a backreference or
		// when there is a PIM. For NFA_MATCH check the position,
		// lower position is preferred.
		אם (!rex.nfa_has_backref && pim == عدم && !l->has_pim
						     && state->c != NFA_MATCH)
		{
		    // When called from addstate_here() do insert before
		    // existing states.
		    אם (add_here)
		    {
			עבור (k = 0; k < l->n && k < listindex; ++k)
			    אם (l->t[k].state->id == state->id)
			    {
				found = TRUE;
				اكسر;
			    }
		    }
		    אם (!add_here || found)
		    {
skip_add:
#ifdef ENABLE_LOG
			nfa_set_code(state->c);
			fprintf(log_fd, "> Not adding state %d to list %d. char %d: %s pim: %s has_pim: %d found: %d\n",
			    abs(state->id), l->id, state->c, code,
			    pim == عدم ? "NULL" : "yes", l->has_pim, found);
#endif
			החזר subs;
		    }
		}

		// Do not add the state again when it exists with the same
		// positions.
		אם (has_state_with_pos(l, state, subs, pim))
		    اذهب skip_add;
	    }

	    // When there are backreferences or PIMs the number of states may
	    // be (a lot) bigger than anticipated.
	    אם (l->n == l->len)
	    {
		صحيح		newlen = l->len * 3 / 2 + 50;
		size_t		newsize = newlen * حجم(nfa_thread_T);
		nfa_thread_T	*newt;

		אם ((long)(newsize >> 10) >= p_mmp)
		{
		    emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		    החזר عدم;
		}
		אם (subs != &temp_subs)
		{
		    // "subs" may point into the current array, need to make a
		    // copy before it becomes invalid.
		    copy_sub(&temp_subs.norm, &subs->norm);
#ifdef FEAT_SYN_HL
		    אם (rex.nfa_has_zsubexpr)
			copy_sub(&temp_subs.synt, &subs->synt);
#endif
		    subs = &temp_subs;
		}

		newt = vim_realloc(l->t, newsize);
		אם (newt == عدم)
		{
		    // out of memory
		    החזר عدم;
		}
		l->t = newt;
		l->len = newlen;
	    }

	    // add the state to the list
	    state->lastlist[nfa_ll_index] = l->id;
	    thread = &l->t[l->n++];
	    thread->state = state;
	    אם (pim == عدم)
		thread->pim.result = NFA_PIM_UNUSED;
	    אחרת
	    {
		copy_pim(&thread->pim, pim);
		l->has_pim = TRUE;
	    }
	    copy_sub(&thread->subs.norm, &subs->norm);
#ifdef FEAT_SYN_HL
	    אם (rex.nfa_has_zsubexpr)
		copy_sub(&thread->subs.synt, &subs->synt);
#endif
#ifdef ENABLE_LOG
	    report_state("Adding", &thread->subs.norm, state, l->id, pim);
	    did_print = TRUE;
#endif
    }

#ifdef ENABLE_LOG
    אם (!did_print)
	report_state("Processing", &subs->norm, state, l->id, pim);
#endif
    بدل (state->c)
    {
	حالة NFA_MATCH:
	    اكسر;

	حالة NFA_SPLIT:
	    // order matters here
	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    subs = addstate(l, state->out1, subs, pim, off_arg, depth + 1);
	    اكسر;

	حالة NFA_EMPTY:
	حالة NFA_NOPEN:
	حالة NFA_NCLOSE:
	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    اكسر;

	حالة NFA_MOPEN:
	حالة NFA_MOPEN1:
	حالة NFA_MOPEN2:
	حالة NFA_MOPEN3:
	حالة NFA_MOPEN4:
	حالة NFA_MOPEN5:
	حالة NFA_MOPEN6:
	حالة NFA_MOPEN7:
	حالة NFA_MOPEN8:
	حالة NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	حالة NFA_ZOPEN:
	حالة NFA_ZOPEN1:
	حالة NFA_ZOPEN2:
	حالة NFA_ZOPEN3:
	حالة NFA_ZOPEN4:
	حالة NFA_ZOPEN5:
	حالة NFA_ZOPEN6:
	حالة NFA_ZOPEN7:
	حالة NFA_ZOPEN8:
	حالة NFA_ZOPEN9:
#endif
	حالة NFA_ZSTART:
	    אם (state->c == NFA_ZSTART)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#ifdef FEAT_SYN_HL
	    אחרת אם (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
	    {
		subidx = state->c - NFA_ZOPEN;
		sub = &subs->synt;
	    }
#endif
	    אחרת
	    {
		subidx = state->c - NFA_MOPEN;
		sub = &subs->norm;
	    }

	    // avoid compiler warnings
	    save_ptr = عدم;
	    CLEAR_FIELD(save_multipos);

	    // Set the position (with "off" added) in the subexpression.  Save
	    // and restore it when it was in use.  Otherwise fill any gap.
	    אם (REG_MULTI)
	    {
		אם (subidx < sub->in_use)
		{
		    save_multipos = sub->list.multi[subidx];
		    save_in_use = -1;
		}
		אחרת
		{
		    save_in_use = sub->in_use;
		    עבור (i = sub->in_use; i < subidx; ++i)
		    {
			sub->list.multi[i].start_lnum = -1;
			sub->list.multi[i].end_lnum = -1;
		    }
		    sub->in_use = subidx + 1;
		}
		אם (off == -1)
		{
		    sub->list.multi[subidx].start_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].start_col = 0;
		}
		אחרת
		{
		    sub->list.multi[subidx].start_lnum = rex.lnum;
		    sub->list.multi[subidx].start_col =
					 (colnr_T)(rex.input - rex.line + off);
		}
		sub->list.multi[subidx].end_lnum = -1;
	    }
	    אחרת
	    {
		אם (subidx < sub->in_use)
		{
		    save_ptr = sub->list.line[subidx].start;
		    save_in_use = -1;
		}
		אחרת
		{
		    save_in_use = sub->in_use;
		    עבור (i = sub->in_use; i < subidx; ++i)
		    {
			sub->list.line[i].start = عدم;
			sub->list.line[i].end = عدم;
		    }
		    sub->in_use = subidx + 1;
		}
		sub->list.line[subidx].start = rex.input + off;
	    }

	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    אם (subs == عدم)
		اكسر;
	    // "subs" may have changed, need to set "sub" again
#ifdef FEAT_SYN_HL
	    אם (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
		sub = &subs->synt;
	    אחרת
#endif
		sub = &subs->norm;

	    אם (save_in_use == -1)
	    {
		אם (REG_MULTI)
		    sub->list.multi[subidx] = save_multipos;
		אחרת
		    sub->list.line[subidx].start = save_ptr;
	    }
	    אחרת
		sub->in_use = save_in_use;
	    اكسر;

	حالة NFA_MCLOSE:
	    אם (rex.nfa_has_zend && (REG_MULTI
			? subs->norm.list.multi[0].end_lnum >= 0
			: subs->norm.list.line[0].end != عدم))
	    {
		// Do not overwrite the position set by \ze.
		subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
		اكسر;
	    }
	    // FALLTHROUGH
	حالة NFA_MCLOSE1:
	حالة NFA_MCLOSE2:
	حالة NFA_MCLOSE3:
	حالة NFA_MCLOSE4:
	حالة NFA_MCLOSE5:
	حالة NFA_MCLOSE6:
	حالة NFA_MCLOSE7:
	حالة NFA_MCLOSE8:
	حالة NFA_MCLOSE9:
#ifdef FEAT_SYN_HL
	حالة NFA_ZCLOSE:
	حالة NFA_ZCLOSE1:
	حالة NFA_ZCLOSE2:
	حالة NFA_ZCLOSE3:
	حالة NFA_ZCLOSE4:
	حالة NFA_ZCLOSE5:
	حالة NFA_ZCLOSE6:
	حالة NFA_ZCLOSE7:
	حالة NFA_ZCLOSE8:
	حالة NFA_ZCLOSE9:
#endif
	حالة NFA_ZEND:
	    אם (state->c == NFA_ZEND)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#ifdef FEAT_SYN_HL
	    אחרת אם (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
	    {
		subidx = state->c - NFA_ZCLOSE;
		sub = &subs->synt;
	    }
#endif
	    אחרת
	    {
		subidx = state->c - NFA_MCLOSE;
		sub = &subs->norm;
	    }

	    // We don't fill in gaps here, there must have been an MOPEN that
	    // has done that.
	    save_in_use = sub->in_use;
	    אם (sub->in_use <= subidx)
		sub->in_use = subidx + 1;
	    אם (REG_MULTI)
	    {
		save_multipos = sub->list.multi[subidx];
		אם (off == -1)
		{
		    sub->list.multi[subidx].end_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].end_col = 0;
		}
		אחרת
		{
		    sub->list.multi[subidx].end_lnum = rex.lnum;
		    sub->list.multi[subidx].end_col =
					  (colnr_T)(rex.input - rex.line + off);
		}
		// avoid compiler warnings
		save_ptr = عدم;
	    }
	    אחרת
	    {
		save_ptr = sub->list.line[subidx].end;
		sub->list.line[subidx].end = rex.input + off;
		// avoid compiler warnings
		CLEAR_FIELD(save_multipos);
	    }

	    subs = addstate(l, state->out, subs, pim, off_arg, depth + 1);
	    אם (subs == عدم)
		اكسر;
	    // "subs" may have changed, need to set "sub" again
#ifdef FEAT_SYN_HL
	    אם (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
		sub = &subs->synt;
	    אחרת
#endif
		sub = &subs->norm;

	    אם (REG_MULTI)
		sub->list.multi[subidx] = save_multipos;
	    אחרת
		sub->list.line[subidx].end = save_ptr;
	    sub->in_use = save_in_use;
	    اكسر;
    }
    החזר subs;
}

/*
 * Like addstate(), but the new state(s) are put at position "*ip".
 * Used for zero-width matches, next state to use is the added one.
 * This makes sure the order of states to be tried does not change, which
 * matters for alternatives.
 */
    ساكن regsubs_T *
addstate_here(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim,   // postponed look-behind match
    صحيح			*ip)
{
    صحيح tlen = l->n;
    صحيح count;
    صحيح listidx = *ip;
    regsubs_T *r;

    // First add the state(s) at the end, so that we know how many there are.
    // Pass the listidx as offset (avoids adding another argument to
    // addstate()).
    r = addstate(l, state, subs, pim, -listidx - ADDSTATE_HERE_OFFSET, 0);
    אם (r == عدم)
	החזר عدم;

    // when "*ip" was at the end of the list, nothing to do
    אם (listidx + 1 == tlen)
	החזר r;

    // re-order to put the new state at the current position
    count = l->n - tlen;
    אם (count == 0)
	החזר r; // no state got added
    אם (count == 1)
    {
	// overwrite the current state
	l->t[listidx] = l->t[l->n - 1];
    }
    אחרת אם (count > 1)
    {
	אם (l->n + count - 1 >= l->len)
	{
	    // not enough space to move the new states, reallocate the list
	    // and move the states to the right position
	    صحيح		    newlen = l->len * 3 / 2 + 50;
	    size_t	    newsize = newlen * حجم(nfa_thread_T);
	    nfa_thread_T    *newl;

	    אם ((long)(newsize >> 10) >= p_mmp)
	    {
		emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		החזר عدم;
	    }
	    newl = alloc(newsize);
	    אם (newl == عدم)
		החזר عدم;
	    l->len = newlen;
	    mch_memmove(&(newl[0]),
		    &(l->t[0]),
		    حجم(nfa_thread_T) * listidx);
	    mch_memmove(&(newl[listidx]),
		    &(l->t[l->n - count]),
		    حجم(nfa_thread_T) * count);
	    mch_memmove(&(newl[listidx + count]),
		    &(l->t[listidx + 1]),
		    حجم(nfa_thread_T) * (l->n - count - listidx - 1));
	    vim_free(l->t);
	    l->t = newl;
	}
	אחרת
	{
	    // make space for new states, then move them from the
	    // end to the current position
	    mch_memmove(&(l->t[listidx + count]),
		    &(l->t[listidx + 1]),
		    حجم(nfa_thread_T) * (l->n - listidx - 1));
	    mch_memmove(&(l->t[listidx]),
		    &(l->t[l->n - 1]),
		    حجم(nfa_thread_T) * count);
	}
    }
    --l->n;
    *ip = listidx - 1;

    החזר r;
}

/*
 * Check character class "class" against current character c.
 */
    ساكن صحيح
check_char_class(صحيح class, صحيح c)
{
    بدل (class)
    {
	حالة NFA_CLASS_ALNUM:
	    אם (c >= 1 && c < 128 && isalnum(c))
		החזר OK;
	    اكسر;
	حالة NFA_CLASS_ALPHA:
	    אם (c >= 1 && c < 128 && isalpha(c))
		החזר OK;
	    اكسر;
	حالة NFA_CLASS_BLANK:
	    אם (c == ' ' || c == '\t')
		החזר OK;
	    اكسر;
	حالة NFA_CLASS_CNTRL:
	    אם (c >= 1 && c <= 127 && iscntrl(c))
		החזר OK;
	    اكسر;
	حالة NFA_CLASS_DIGIT:
	    אם (VIM_ISDIGIT(c))
		החזר OK;
	    اكسر;
	حالة NFA_CLASS_GRAPH:
	    אם (c >= 1 && c <= 127 && isgraph(c))
		החזר OK;
	    اكسر;
	حالة NFA_CLASS_LOWER:
	    אם (MB_ISLOWER(c) && c != 170 && c != 186)
		החזר OK;
	    اكسر;
	حالة NFA_CLASS_PRINT:
	    אם (vim_isprintc(c))
		החזר OK;
	    اكسر;
	حالة NFA_CLASS_PUNCT:
	    אם (c >= 1 && c < 128 && ispunct(c))
		החזר OK;
	    اكسر;
	حالة NFA_CLASS_SPACE:
	    אם ((c >= 9 && c <= 13) || (c == ' '))
		החזר OK;
	    اكسر;
	حالة NFA_CLASS_UPPER:
	    אם (MB_ISUPPER(c))
		החזר OK;
	    اكسر;
	حالة NFA_CLASS_XDIGIT:
	    אם (vim_isxdigit(c))
		החזר OK;
	    اكسر;
	حالة NFA_CLASS_TAB:
	    אם (c == '\t')
		החזר OK;
	    اكسر;
	حالة NFA_CLASS_RETURN:
	    אם (c == '\r')
		החזר OK;
	    اكسر;
	حالة NFA_CLASS_BACKSPACE:
	    אם (c == '\b')
		החזר OK;
	    اكسر;
	حالة NFA_CLASS_ESCAPE:
	    אם (c == '\033')
		החזר OK;
	    اكسر;
	حالة NFA_CLASS_IDENT:
	    אם (vim_isIDc(c))
		החזר OK;
	    اكسر;
	حالة NFA_CLASS_KEYWORD:
	    אם (reg_iswordc(c))
		החזר OK;
	    اكسر;
	حالة NFA_CLASS_FNAME:
	    אם (vim_isfilec(c))
		החזר OK;
	    اكسر;

	افتراضي:
	    // should not be here :P
	    siemsg(_(e_nfa_regexp_invalid_character_class_nr), class);
	    החזר FAIL;
    }
    החזר FAIL;
}

/*
 * Check for a match with subexpression "subidx".
 * Return TRUE if it matches.
 */
    ساكن صحيح
match_backref(
    regsub_T	*sub,	    // pointers to subexpressions
    صحيح		subidx,
    صحيح		*bytelen)   // out: length of match in bytes
{
    صحيح		len;

    אם (sub->in_use <= subidx)
    {
retempty:
	// backref was not set, match an empty string
	*bytelen = 0;
	החזר TRUE;
    }

    אם (REG_MULTI)
    {
	אם (sub->list.multi[subidx].start_lnum < 0
				       || sub->list.multi[subidx].end_lnum < 0)
	    اذهب retempty;
	אם (sub->list.multi[subidx].start_lnum == rex.lnum
			       && sub->list.multi[subidx].end_lnum == rex.lnum)
	{
	    len = sub->list.multi[subidx].end_col
					  - sub->list.multi[subidx].start_col;
	    אם (cstrncmp(rex.line + sub->list.multi[subidx].start_col,
							 rex.input, &len) == 0)
	    {
		*bytelen = len;
		החזר TRUE;
	    }
	}
	אחרת
	{
	    אם (match_with_backref(
			sub->list.multi[subidx].start_lnum,
			sub->list.multi[subidx].start_col,
			sub->list.multi[subidx].end_lnum,
			sub->list.multi[subidx].end_col,
			bytelen) == RA_MATCH)
		החזר TRUE;
	}
    }
    אחרת
    {
	אם (sub->list.line[subidx].start == عدم
					|| sub->list.line[subidx].end == عدم)
	    اذهب retempty;
	len = (صحيح)(sub->list.line[subidx].end - sub->list.line[subidx].start);
	אם (cstrncmp(sub->list.line[subidx].start, rex.input, &len) == 0)
	{
	    *bytelen = len;
	    החזר TRUE;
	}
    }
    החזר FALSE;
}

#ifdef FEAT_SYN_HL

/*
 * Check for a match with \z subexpression "subidx".
 * Return TRUE if it matches.
 */
    ساكن صحيح
match_zref(
    صحيح		subidx,
    صحيح		*bytelen)   // out: length of match in bytes
{
    صحيح		len;

    cleanup_zsubexpr();
    אם (re_extmatch_in == عدم || re_extmatch_in->matches[subidx] == عدم)
    {
	// backref was not set, match an empty string
	*bytelen = 0;
	החזר TRUE;
    }

    len = (صحيح)STRLEN(re_extmatch_in->matches[subidx]);
    אם (cstrncmp(re_extmatch_in->matches[subidx], rex.input, &len) == 0)
    {
	*bytelen = len;
	החזר TRUE;
    }
    החזר FALSE;
}
#endif

/*
 * Save list IDs for all NFA states of "prog" into "list".
 * Also reset the IDs to zero.
 * Only used for the recursive value lastlist[1].
 */
    ساكن فراغ
nfa_save_listids(nfa_regprog_T *prog, صحيح *list)
{
    صحيح		    i;
    nfa_state_T	    *p;

    // Order in the list is reverse, it's a bit faster that way.
    p = &prog->state[0];
    עבור (i = prog->nstate; --i >= 0; )
    {
	list[i] = p->lastlist[1];
	p->lastlist[1] = 0;
	++p;
    }
}

/*
 * Restore list IDs from "list" to all NFA states.
 */
    ساكن فراغ
nfa_restore_listids(nfa_regprog_T *prog, صحيح *list)
{
    صحيح		    i;
    nfa_state_T	    *p;

    p = &prog->state[0];
    עבור (i = prog->nstate; --i >= 0; )
    {
	p->lastlist[1] = list[i];
	++p;
    }
}

    ساكن صحيح
nfa_re_num_cmp(long_u val, صحيح op, long_u pos)
{
    אם (op == 1) החזר pos > val;
    אם (op == 2) החזר pos < val;
    החזר val == pos;
}

ساكن صحيح nfa_regmatch(nfa_regprog_T *prog, nfa_state_T *start, regsubs_T *submatch, regsubs_T *m);

/*
 * Recursively call nfa_regmatch()
 * "pim" is NULL or contains info about a Postponed Invisible Match (start
 * position).
 */
    ساكن صحيح
recursive_regmatch(
    nfa_state_T	    *state,
    nfa_pim_T	    *pim,
    nfa_regprog_T   *prog,
    regsubs_T	    *submatch,
    regsubs_T	    *m,
    صحيح		    **listids,
    صحيح		    *listids_len)
{
    صحيح		save_reginput_col = (صحيح)(rex.input - rex.line);
    صحيح		save_reglnum = rex.lnum;
    صحيح		save_nfa_match = nfa_match;
    صحيح		save_nfa_listid = rex.nfa_listid;
    save_se_T   *save_nfa_endp = nfa_endp;
    save_se_T   endpos;
    save_se_T   *endposp = عدم;
    صحيح		result;
    صحيح		need_restore = FALSE;

    אם (pim != عدم)
    {
	// start at the position where the postponed match was
	אם (REG_MULTI)
	    rex.input = rex.line + pim->end.pos.col;
	אחרת
	    rex.input = pim->end.ptr;
    }

    אם (state->c == NFA_START_INVISIBLE_BEFORE
	    || state->c == NFA_START_INVISIBLE_BEFORE_FIRST
	    || state->c == NFA_START_INVISIBLE_BEFORE_NEG
	    || state->c == NFA_START_INVISIBLE_BEFORE_NEG_FIRST)
    {
	// The recursive match must end at the current position. When "pim" is
	// not NULL it specifies the current position.
	endposp = &endpos;
	אם (REG_MULTI)
	{
	    אם (pim == عدم)
	    {
		endpos.se_u.pos.col = (صحيح)(rex.input - rex.line);
		endpos.se_u.pos.lnum = rex.lnum;
	    }
	    אחרת
		endpos.se_u.pos = pim->end.pos;
	}
	אחרת
	{
	    אם (pim == عدم)
		endpos.se_u.ptr = rex.input;
	    אחרת
		endpos.se_u.ptr = pim->end.ptr;
	}

	// Go back the specified number of bytes, or as far as the
	// start of the previous line, to try matching "\@<=" or
	// not matching "\@<!". This is very inefficient, limit the number of
	// bytes if possible.
	אם (state->val <= 0)
	{
	    אם (REG_MULTI)
	    {
		rex.line = reg_getline(--rex.lnum);
		אם (rex.line == عدم)
		    // can't go before the first line
		    rex.line = reg_getline(++rex.lnum);
	    }
	    rex.input = rex.line;
	}
	אחרת
	{
	    אם (REG_MULTI && (صحيح)(rex.input - rex.line) < state->val)
	    {
		// Not enough bytes in this line, go to end of
		// previous line.
		rex.line = reg_getline(--rex.lnum);
		אם (rex.line == عدم)
		{
		    // can't go before the first line
		    rex.line = reg_getline(++rex.lnum);
		    rex.input = rex.line;
		}
		אחרת
		    rex.input = rex.line + reg_getline_len(rex.lnum);
	    }
	    אם ((صحيح)(rex.input - rex.line) >= state->val)
	    {
		rex.input -= state->val;
		אם (has_mbyte)
		    rex.input -= mb_head_off(rex.line, rex.input);
	    }
	    אחרת
		rex.input = rex.line;
	}
    }

#ifdef ENABLE_LOG
    אם (log_fd != stderr)
	fclose(log_fd);
    log_fd = عدم;
#endif
    // Have to clear the lastlist field of the NFA nodes, so that
    // nfa_regmatch() and addstate() can run properly after recursion.
    אם (nfa_ll_index == 1)
    {
	// Already calling nfa_regmatch() recursively.  Save the lastlist[1]
	// values and clear them.
	אם (*listids == عدم || *listids_len < prog->nstate)
	{
	    vim_free(*listids);
	    *listids = ALLOC_MULT(صحيح, prog->nstate);
	    אם (*listids == عدم)
	    {
		emsg(_(e_nfa_regexp_could_not_allocate_memory_for_branch_traversal));
		החזר 0;
	    }
	    *listids_len = prog->nstate;
	}
	nfa_save_listids(prog, *listids);
	need_restore = TRUE;
	// any value of rex.nfa_listid will do
    }
    אחרת
    {
	// First recursive nfa_regmatch() call, switch to the second lastlist
	// entry.  Make sure rex.nfa_listid is different from a previous
	// recursive call, because some states may still have this ID.
	++nfa_ll_index;
	אם (rex.nfa_listid <= rex.nfa_alt_listid)
	    rex.nfa_listid = rex.nfa_alt_listid;
    }

    // Call nfa_regmatch() to check if the current concat matches at this
    // position. The concat ends with the node NFA_END_INVISIBLE
    nfa_endp = endposp;
    result = nfa_regmatch(prog, state->out, submatch, m);

    אם (need_restore)
	nfa_restore_listids(prog, *listids);
    אחרת
    {
	--nfa_ll_index;
	rex.nfa_alt_listid = rex.nfa_listid;
    }

    // restore position in input text
    rex.lnum = save_reglnum;
    אם (REG_MULTI)
	rex.line = reg_getline(rex.lnum);
    rex.input = rex.line + save_reginput_col;
    אם (result != NFA_TOO_EXPENSIVE)
    {
	nfa_match = save_nfa_match;
	rex.nfa_listid = save_nfa_listid;
    }
    nfa_endp = save_nfa_endp;

#ifdef ENABLE_LOG
    open_debug_log(result);
#endif

    החזר result;
}

/*
 * Estimate the chance of a match with "state" failing.
 * empty match: 0
 * NFA_ANY: 1
 * specific character: 99
 */
    ساكن صحيح
failure_chance(nfa_state_T *state, صحيح depth)
{
    صحيح c = state->c;
    صحيح l, r;

    // detect looping
    אם (depth > 4)
	החזר 1;

    بدل (c)
    {
	حالة NFA_SPLIT:
	    אם (state->out->c == NFA_SPLIT || state->out1->c == NFA_SPLIT)
		// avoid recursive stuff
		החזר 1;
	    // two alternatives, use the lowest failure chance
	    l = failure_chance(state->out, depth + 1);
	    r = failure_chance(state->out1, depth + 1);
	    החזר l < r ? l : r;

	حالة NFA_ANY:
	    // matches anything, unlikely to fail
	    החזר 1;

	حالة NFA_MATCH:
	حالة NFA_MCLOSE:
	حالة NFA_ANY_COMPOSING:
	    // empty match works always
	    החזר 0;

	حالة NFA_START_INVISIBLE:
	حالة NFA_START_INVISIBLE_FIRST:
	حالة NFA_START_INVISIBLE_NEG:
	حالة NFA_START_INVISIBLE_NEG_FIRST:
	حالة NFA_START_INVISIBLE_BEFORE:
	حالة NFA_START_INVISIBLE_BEFORE_FIRST:
	حالة NFA_START_INVISIBLE_BEFORE_NEG:
	حالة NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
	حالة NFA_START_PATTERN:
	    // recursive regmatch is expensive, use low failure chance
	    החזר 5;

	حالة NFA_BOL:
	حالة NFA_EOL:
	حالة NFA_BOF:
	حالة NFA_EOF:
	حالة NFA_NEWL:
	    החזר 99;

	حالة NFA_BOW:
	حالة NFA_EOW:
	    החזר 90;

	حالة NFA_MOPEN:
	حالة NFA_MOPEN1:
	حالة NFA_MOPEN2:
	حالة NFA_MOPEN3:
	حالة NFA_MOPEN4:
	حالة NFA_MOPEN5:
	حالة NFA_MOPEN6:
	حالة NFA_MOPEN7:
	حالة NFA_MOPEN8:
	حالة NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	حالة NFA_ZOPEN:
	حالة NFA_ZOPEN1:
	حالة NFA_ZOPEN2:
	حالة NFA_ZOPEN3:
	حالة NFA_ZOPEN4:
	حالة NFA_ZOPEN5:
	حالة NFA_ZOPEN6:
	حالة NFA_ZOPEN7:
	حالة NFA_ZOPEN8:
	حالة NFA_ZOPEN9:
	حالة NFA_ZCLOSE:
	حالة NFA_ZCLOSE1:
	حالة NFA_ZCLOSE2:
	حالة NFA_ZCLOSE3:
	حالة NFA_ZCLOSE4:
	حالة NFA_ZCLOSE5:
	حالة NFA_ZCLOSE6:
	حالة NFA_ZCLOSE7:
	حالة NFA_ZCLOSE8:
	حالة NFA_ZCLOSE9:
#endif
	حالة NFA_NOPEN:
	حالة NFA_MCLOSE1:
	حالة NFA_MCLOSE2:
	حالة NFA_MCLOSE3:
	حالة NFA_MCLOSE4:
	حالة NFA_MCLOSE5:
	حالة NFA_MCLOSE6:
	حالة NFA_MCLOSE7:
	حالة NFA_MCLOSE8:
	حالة NFA_MCLOSE9:
	حالة NFA_NCLOSE:
	    החזר failure_chance(state->out, depth + 1);

	حالة NFA_BACKREF1:
	حالة NFA_BACKREF2:
	حالة NFA_BACKREF3:
	حالة NFA_BACKREF4:
	حالة NFA_BACKREF5:
	حالة NFA_BACKREF6:
	حالة NFA_BACKREF7:
	حالة NFA_BACKREF8:
	حالة NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	حالة NFA_ZREF1:
	حالة NFA_ZREF2:
	حالة NFA_ZREF3:
	حالة NFA_ZREF4:
	حالة NFA_ZREF5:
	حالة NFA_ZREF6:
	حالة NFA_ZREF7:
	حالة NFA_ZREF8:
	حالة NFA_ZREF9:
#endif
	    // backreferences don't match in many places
	    החזר 94;

	حالة NFA_LNUM_GT:
	حالة NFA_LNUM_LT:
	حالة NFA_COL_GT:
	حالة NFA_COL_LT:
	حالة NFA_VCOL_GT:
	حالة NFA_VCOL_LT:
	حالة NFA_MARK_GT:
	حالة NFA_MARK_LT:
	حالة NFA_VISUAL:
	    // before/after positions don't match very often
	    החזר 85;

	حالة NFA_LNUM:
	    החזר 90;

	حالة NFA_CURSOR:
	حالة NFA_COL:
	حالة NFA_VCOL:
	حالة NFA_MARK:
	    // specific positions rarely match
	    החזר 98;

	حالة NFA_COMPOSING:
	    החזר 95;

	افتراضي:
	    אם (c > 0)
		// character match fails often
		החזר 95;
    }

    // something else, includes character classes
    החזר 50;
}

/*
 * Skip until the char "c" we know a match must start with.
 */
    ساكن صحيح
skip_to_start(صحيح c, colnr_T *colp)
{
    char_u *s;

    // Used often, do some work to avoid call overhead.
    אם (!rex.reg_ic && !has_mbyte)
	s = vim_strbyte(rex.line + *colp, c);
    אחרת
	s = cstrchr(rex.line + *colp, c);
    אם (s == عدم)
	החזר FAIL;
    *colp = (صحيح)(s - rex.line);
    החזר OK;
}

/*
 * Check for a match with match_text.
 * Called after skip_to_start() has found regstart.
 * Returns zero for no match, 1 for a match.
 */
    ساكن long
find_match_text(colnr_T *startcol, صحيح regstart, char_u *match_text)
{
    colnr_T col = *startcol;
    صحيح	    c1, c2;
    صحيح	    len1, len2;
    صحيح	    match;

    עבור (;;)
    {
	match = TRUE;
	// Skip the input character: case-folding can change its byte length.
	len2 = MB_CPTR2LEN(rex.line + col);
	עבור (len1 = 0; match_text[len1] != NUL; len1 += MB_CHAR2LEN(c1))
	{
	    c1 = PTR2CHAR(match_text + len1);
	    c2 = PTR2CHAR(rex.line + col + len2);
	    אם (c1 != c2 && (!rex.reg_ic || MB_CASEFOLD(c1) != MB_CASEFOLD(c2)))
	    {
		match = FALSE;
		اكسر;
	    }
	    len2 += enc_utf8 ? utf_ptr2len(rex.line + col + len2)
							     : MB_CHAR2LEN(c2);
	}
	אם (match
		// check that no composing char follows
		&& !(enc_utf8
			  && utf_iscomposing(PTR2CHAR(rex.line + col + len2))))
	{
	    cleanup_subexpr();
	    אם (REG_MULTI)
	    {
		rex.reg_startpos[0].lnum = rex.lnum;
		rex.reg_startpos[0].col = col;
		rex.reg_endpos[0].lnum = rex.lnum;
		rex.reg_endpos[0].col = col + len2;
	    }
	    אחרת
	    {
		rex.reg_startp[0] = rex.line + col;
		rex.reg_endp[0] = rex.line + col + len2;
	    }
	    *startcol = col;
	    החזר 1L;
	}

	// Try finding regstart after the current match.
	col += MB_CPTR2LEN(rex.line + col); // skip the input character
	אם (skip_to_start(regstart, &col) == FAIL)
	    اكسر;
    }

    *startcol = col;
    החזר 0L;
}

/*
 * Check whether the composing characters at the current input position match
 * the NFA composing sub-expression that starts at "sta" (the first state
 * after NFA_COMPOSING).  "curc" is the base character and "clen" its byte
 * length.
 * Returns OK when it matches, FAIL otherwise.
 */
    ساكن صحيح
match_composing(nfa_state_T *sta, صحيح curc, صحيح clen)
{
    صحيح		mc = curc;
    صحيح		len = 0;
    صحيح		cchars[MAX_MCO];
    صحيح		ccount = 0;
    صحيح		j;

    אם (utf_iscomposing(sta->c))
	// Only match composing character(s), ignore base character.  Used
	// for ".{composing}" and "{composing}" (no preceding character).
	len += mb_char2len(mc);

    אם (rex.reg_icombine && len == 0)
	// If \Z was present, then ignore composing characters.  When
	// ignoring the base character this always matches.
	החזר sta->c == curc ? OK : FAIL;

    // Check base character matches first, unless ignored.
    אם (len == 0 && mc != sta->c)
	החזר FAIL;

    אם (len == 0)
    {
	len += mb_char2len(mc);
	sta = sta->out;
    }

    // We don't care about the order of composing characters.  Get them into
    // cchars[] first.
    כלעוד (len < clen)
    {
	mc = mb_ptr2char(rex.input + len);
	cchars[ccount++] = mc;
	len += mb_char2len(mc);
	אם (ccount == MAX_MCO)
	    اكسر;
    }

    // Check that each composing char in the pattern matches a composing char
    // in the text.  We do not check if all composing chars are matched.
    כלעוד (sta->c != NFA_END_COMPOSING)
    {
	עבור (j = 0; j < ccount; ++j)
	    אם (cchars[j] == sta->c)
		اكسر;
	אם (j == ccount)
	    החזר FAIL;
	sta = sta->out;
    }

    החזר OK;
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
    ساكن صحيح
nfa_regmatch(
    nfa_regprog_T	*prog,
    nfa_state_T		*start,
    regsubs_T		*submatch,
    regsubs_T		*m)
{
    صحيح		result = FALSE;
    size_t	size = 0;
    صحيح		flag = 0;
    صحيح		go_to_nextline = FALSE;
    nfa_thread_T *t;
    nfa_list_T	list[2];
    صحيح		listidx;
    nfa_list_T	*thislist;
    nfa_list_T	*nextlist;
    صحيح		*listids = عدم;
    صحيح		listids_len = 0;
    nfa_state_T *add_state;
    صحيح		add_here;
    صحيح		add_count;
    صحيح		add_off = 0;
    صحيح		toplevel = start->c == NFA_MOPEN;
    regsubs_T	*r;
#ifdef NFA_REGEXP_DEBUG_LOG
    FILE	*debug;
#endif

    // Some patterns may take a long time to match, especially when using
    // recursive_regmatch(). Allow interrupting them with CTRL-C.
    fast_breakcheck();
    אם (got_int)
	החזר FALSE;
#ifdef FEAT_RELTIME
    אם (nfa_did_time_out())
	החזר FALSE;
#endif

#ifdef NFA_REGEXP_DEBUG_LOG
    debug = fopen(NFA_REGEXP_DEBUG_LOG, "a");
    אם (debug == عدم)
    {
	semsg("(NFA) COULD NOT OPEN %s!", NFA_REGEXP_DEBUG_LOG);
	החזר FALSE;
    }
#endif
    nfa_match = FALSE;

    // Allocate memory for the lists of nodes.
    size = (prog->nstate + 1) * حجم(nfa_thread_T);

    // Reuse cached list buffers from prog when available (top-level call).
    // Recursive calls must allocate their own buffers.
    אם (toplevel && prog->listbuf[0] != عدم)
    {
	list[0].t = (nfa_thread_T *)prog->listbuf[0];
	list[1].t = (nfa_thread_T *)prog->listbuf[1];
	prog->listbuf[0] = عدم;
	prog->listbuf[1] = عدم;
    }
    אחרת
    {
	list[0].t = alloc(size);
	list[1].t = alloc(size);
    }
    list[0].len = prog->nstate + 1;
    list[1].len = prog->nstate + 1;
    אם (list[0].t == عدم || list[1].t == عدم)
	اذهب theend;

#ifdef ENABLE_LOG
    log_fd = fopen(NFA_REGEXP_RUN_LOG, "a");
    אם (log_fd == عدم)
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
    אם (toplevel)
    {
	אם (REG_MULTI)
	{
	    m->norm.list.multi[0].start_lnum = rex.lnum;
	    m->norm.list.multi[0].start_col = (colnr_T)(rex.input - rex.line);
	    m->norm.orig_start_col = m->norm.list.multi[0].start_col;
	}
	אחרת
	    m->norm.list.line[0].start = rex.input;
	m->norm.in_use = 1;
	r = addstate(thislist, start->out, m, عدم, 0, 0);
    }
    אחרת
	r = addstate(thislist, start, m, عدم, 0, 0);
    אם (r == عدم)
    {
	nfa_match = NFA_TOO_EXPENSIVE;
	اذهب theend;
    }

#define	ADD_STATE_IF_MATCH(state)	\
    אם (result)				\
    {					\
	add_state = state->out;		\
	add_off = clen;			\
    }

    /*
     * Run for each character.
     */
    עבור (;;)
    {
	صحيح	curc;
	صحيح	clen;

	אם (has_mbyte)
	{
	    // Fast path for an ASCII byte not followed by a composing
	    // character, matching the check in utfc_ptr2len().  Avoids two
	    // indirect calls for the common case.
	    אם (rex.input[0] != NUL && rex.input[0] < 0x80 && rex.input[1] < 0x80)
	    {
		curc = rex.input[0];
		clen = 1;
	    }
	    אחרת
	    {
		curc = (*mb_ptr2char)(rex.input);
		clen = (*mb_ptr2len)(rex.input);
	    }
	}
	אחרת
	{
	    curc = *rex.input;
	    clen = 1;
	}
	אם (curc == NUL)
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
	אם (prog->re_engine == AUTOMATIC_ENGINE
		&& (rex.nfa_listid >= NFA_MAX_STATES
#ifdef FEAT_EVAL
		    || nfa_fail_for_testing
#endif
		    ))
	{
	    // too many states, retry with old engine
	    nfa_match = NFA_TOO_EXPENSIVE;
	    اذهب theend;
	}

	thislist->id = rex.nfa_listid;
	nextlist->id = rex.nfa_listid + 1;

#ifdef ENABLE_LOG
	fprintf(log_fd, "------------------------------------------\n");
	fprintf(log_fd, ">>> Reginput is \"%s\"\n", rex.input);
	fprintf(log_fd, ">>> Advanced one character... Current char is %c (code %d) \n", curc, (صحيح)curc);
	fprintf(log_fd, ">>> Thislist has %d states available: ", thislist->n);
	{
	    صحيح i;

	    עבור (i = 0; i < thislist->n; i++)
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
	אם (thislist->n == 0)
	    اكسر;

	// compute nextlist
	עבור (listidx = 0; listidx < thislist->n; ++listidx)
	{
	    // If the list gets very long there probably is something wrong.
	    // At least allow interrupting with CTRL-C.  Only check once in a
	    // while, calling ui_breakcheck() for every state is too slow.
	    ساكن صحيح	breakcheck_count = 0;  // using "static" makes it faster
	    אם (unlikely(++breakcheck_count >= 100000))
	    {
		ui_breakcheck();
		breakcheck_count = 0;
	    }
	    אם (got_int)
		اكسر;
#ifdef FEAT_RELTIME
	    אם (nfa_did_time_out())
		اكسر;
#endif
	    t = &thislist->t[listidx];

#ifdef NFA_REGEXP_DEBUG_LOG
	    nfa_set_code(t->state->c);
	    fprintf(debug, "%s, ", code);
#endif
#ifdef ENABLE_LOG
	    {
		صحيح col;

		אם (t->subs.norm.in_use <= 0)
		    col = -1;
		אחרת אם (REG_MULTI)
		    col = t->subs.norm.list.multi[0].start_col;
		אחרת
		    col = (صحيح)(t->subs.norm.list.line[0].start - rex.line);
		nfa_set_code(t->state->c);
		fprintf(log_fd, "(%d) char %d %s (start col %d)%s... \n",
			abs(t->state->id), (صحيح)t->state->c, code, col,
			pim_info(&t->pim));
	    }
#endif

	    /*
	     * Handle the possible codes of the current state.
	     * The most important is NFA_MATCH.
	     */
	    add_state = عدم;
	    add_here = FALSE;
	    add_count = 0;
	    بدل (t->state->c)
	    {
	    حالة NFA_MATCH:
	      {
		// If the match is not at the start of the line, ends before a
		// composing characters and rex.reg_icombine is not set, that
		// is not really a match.
		אם (enc_utf8 && !rex.reg_icombine
			     && rex.input != rex.line && utf_iscomposing(curc))
		    اكسر;

		nfa_match = TRUE;
		copy_sub(&submatch->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		אם (rex.nfa_has_zsubexpr)
		    copy_sub(&submatch->synt, &t->subs.synt);
#endif
#ifdef ENABLE_LOG
		log_subsexpr(&t->subs);
#endif
		// Found the left-most longest match, do not look at any other
		// states at this position.  When the list of states is going
		// to be empty quit without advancing, so that "rex.input" is
		// correct.
		אם (nextlist->n == 0)
		    clen = 0;
		اذهب nextchar;
	      }

	    حالة NFA_END_INVISIBLE:
	    حالة NFA_END_INVISIBLE_NEG:
	    حالة NFA_END_PATTERN:
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
		אם (nfa_endp != عدم)
		{
		    אם (REG_MULTI)
			fprintf(log_fd, "Current lnum: %d, endp lnum: %d; current col: %d, endp col: %d\n",
				(صحيح)rex.lnum,
				(صحيح)nfa_endp->se_u.pos.lnum,
				(صحيح)(rex.input - rex.line),
				nfa_endp->se_u.pos.col);
		    אחרת
			fprintf(log_fd, "Current col: %d, endp col: %d\n",
				(صحيح)(rex.input - rex.line),
				(صحيح)(nfa_endp->se_u.ptr - rex.input));
		}
#endif
		// If "nfa_endp" is set it's only a match if it ends at
		// "nfa_endp"
		אם (nfa_endp != عدم && (REG_MULTI
			? (rex.lnum != nfa_endp->se_u.pos.lnum
			    || (صحيح)(rex.input - rex.line)
						!= nfa_endp->se_u.pos.col)
			: rex.input != nfa_endp->se_u.ptr))
		    اكسر;

		// do not set submatches for \@!
		אם (t->state->c != NFA_END_INVISIBLE_NEG)
		{
		    copy_sub(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		    אם (rex.nfa_has_zsubexpr)
			copy_sub(&m->synt, &t->subs.synt);
#endif
		}
#ifdef ENABLE_LOG
		fprintf(log_fd, "Match found:\n");
		log_subsexpr(m);
#endif
		nfa_match = TRUE;
		// See comment above at "goto nextchar".
		אם (nextlist->n == 0)
		    clen = 0;
		اذهب nextchar;

	    حالة NFA_START_INVISIBLE:
	    حالة NFA_START_INVISIBLE_FIRST:
	    حالة NFA_START_INVISIBLE_NEG:
	    حالة NFA_START_INVISIBLE_NEG_FIRST:
	    حالة NFA_START_INVISIBLE_BEFORE:
	    حالة NFA_START_INVISIBLE_BEFORE_FIRST:
	    حالة NFA_START_INVISIBLE_BEFORE_NEG:
	    حالة NFA_START_INVISIBLE_BEFORE_NEG_FIRST:
		{
#ifdef ENABLE_LOG
		    fprintf(log_fd, "Failure chance invisible: %d, what follows: %d\n",
			    failure_chance(t->state->out, 0),
			    failure_chance(t->state->out1->out, 0));
#endif
		    // Do it directly if there already is a PIM or when
		    // nfa_postprocess() detected it will work better.
		    אם (t->pim.result != NFA_PIM_UNUSED
			 || t->state->c == NFA_START_INVISIBLE_FIRST
			 || t->state->c == NFA_START_INVISIBLE_NEG_FIRST
			 || t->state->c == NFA_START_INVISIBLE_BEFORE_FIRST
			 || t->state->c == NFA_START_INVISIBLE_BEFORE_NEG_FIRST)
		    {
			صحيح in_use = m->norm.in_use;

			// Copy submatch info for the recursive call, opposite
			// of what happens on success below.
			copy_sub_off(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
			אם (rex.nfa_has_zsubexpr)
			    copy_sub_off(&m->synt, &t->subs.synt);
#endif

			/*
			 * First try matching the invisible match, then what
			 * follows.
			 */
			result = recursive_regmatch(t->state, عدم, prog,
					  submatch, m, &listids, &listids_len);
			אם (result == NFA_TOO_EXPENSIVE)
			{
			    nfa_match = result;
			    اذهب theend;
			}

			// for \@! and \@<! it is a match when the result is
			// FALSE
			אם (result != (t->state->c == NFA_START_INVISIBLE_NEG
			       || t->state->c == NFA_START_INVISIBLE_NEG_FIRST
			       || t->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			       || t->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&t->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
			    אם (rex.nfa_has_zsubexpr)
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
		    אחרת
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
			אם (REG_MULTI)
			{
			    pim.end.pos.col = (صحيح)(rex.input - rex.line);
			    pim.end.pos.lnum = rex.lnum;
			}
			אחרת
			    pim.end.ptr = rex.input;

			// t->state->out1 is the corresponding END_INVISIBLE
			// node; Add its out to the current list (zero-width
			// match).
			אם (addstate_here(thislist, t->state->out1->out,
					     &t->subs, &pim, &listidx) == عدم)
			{
			    nfa_match = NFA_TOO_EXPENSIVE;
			    اذهب theend;
			}
		    }
		}
		اكسر;

	    حالة NFA_START_PATTERN:
	      {
		nfa_state_T *skip = عدم;
#ifdef ENABLE_LOG
		صحيح	    skip_lid = 0;
#endif

		// There is no point in trying to match the pattern if the
		// output state is not going to be added to the list.
		אם (state_in_list(nextlist, t->state->out1->out, &t->subs))
		{
		    skip = t->state->out1->out;
#ifdef ENABLE_LOG
		    skip_lid = nextlist->id;
#endif
		}
		אחרת אם (state_in_list(nextlist,
					  t->state->out1->out->out, &t->subs))
		{
		    skip = t->state->out1->out->out;
#ifdef ENABLE_LOG
		    skip_lid = nextlist->id;
#endif
		}
		אחרת אם (state_in_list(thislist,
					  t->state->out1->out->out, &t->subs))
		{
		    skip = t->state->out1->out->out;
#ifdef ENABLE_LOG
		    skip_lid = thislist->id;
#endif
		}
		אם (skip != عدم)
		{
#ifdef ENABLE_LOG
		    nfa_set_code(skip->c);
		    fprintf(log_fd, "> Not trying to match pattern, output state %d is already in list %d. char %d: %s\n",
			    abs(skip->id), skip_lid, skip->c, code);
#endif
		    اكسر;
		}
		// Copy submatch info to the recursive call, opposite of what
		// happens afterwards.
		copy_sub_off(&m->norm, &t->subs.norm);
#ifdef FEAT_SYN_HL
		אם (rex.nfa_has_zsubexpr)
		    copy_sub_off(&m->synt, &t->subs.synt);
#endif

		// First try matching the pattern.
		result = recursive_regmatch(t->state, عدم, prog,
					  submatch, m, &listids, &listids_len);
		אם (result == NFA_TOO_EXPENSIVE)
		{
		    nfa_match = result;
		    اذهب theend;
		}
		אם (result)
		{
		    صحيح bytelen;

#ifdef ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN matches:\n");
		    log_subsexpr(m);
#endif
		    // Copy submatch info from the recursive call
		    copy_sub_off(&t->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
		    אם (rex.nfa_has_zsubexpr)
			copy_sub_off(&t->subs.synt, &m->synt);
#endif
		    // Now we need to skip over the matched text and then
		    // continue with what follows.
		    אם (REG_MULTI)
			// TODO: multi-line match
			bytelen = m->norm.list.multi[0].end_col
						  - (صحيح)(rex.input - rex.line);
		    אחרת
			bytelen = (صحيح)(m->norm.list.line[0].end - rex.input);

#ifdef ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN length: %d\n", bytelen);
#endif
		    אם (bytelen == 0)
		    {
			// empty match, output of corresponding
			// NFA_END_PATTERN/NFA_SKIP to be used at current
			// position
			add_here = TRUE;
			add_state = t->state->out1->out->out;
		    }
		    אחרת אם (bytelen <= clen)
		    {
			// match current character, output of corresponding
			// NFA_END_PATTERN to be used at next position.
			add_state = t->state->out1->out->out;
			add_off = clen;
		    }
		    אחרת
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			add_state = t->state->out1->out;
			add_off = bytelen;
			add_count = bytelen - clen;
		    }
		}
		اكسر;
	      }

	    حالة NFA_BOL:
		אם (rex.input == rex.line)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		اكسر;

	    حالة NFA_EOL:
		אם (curc == NUL)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		اكسر;

	    حالة NFA_BOW:
		result = TRUE;

		אם (curc == NUL)
		    result = FALSE;
		אחרת אם (has_mbyte)
		{
		    صحيح this_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.input, rex.reg_buf);
		    אם (this_class <= 1)
			result = FALSE;
		    אחרת אם (reg_prev_class() == this_class)
			result = FALSE;
		}
		אחרת אם (!vim_iswordc_buf(curc, rex.reg_buf)
			   || (rex.input > rex.line
			       && vim_iswordc_buf(rex.input[-1], rex.reg_buf)))
		    result = FALSE;
		אם (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		اكسر;

	    حالة NFA_EOW:
		result = TRUE;
		אם (rex.input == rex.line)
		    result = FALSE;
		אחרת אם (has_mbyte)
		{
		    صحيح this_class, prev_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.input, rex.reg_buf);
		    prev_class = reg_prev_class();
		    אם (this_class == prev_class
					|| prev_class == 0 || prev_class == 1)
			result = FALSE;
		}
		אחרת אם (!vim_iswordc_buf(rex.input[-1], rex.reg_buf)
			|| (rex.input[0] != NUL
					&& vim_iswordc_buf(curc, rex.reg_buf)))
		    result = FALSE;
		אם (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		اكسر;

	    حالة NFA_BOF:
		אם (rex.lnum == 0 && rex.input == rex.line
				     && (!REG_MULTI || rex.reg_firstlnum == 1))
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		اكسر;

	    حالة NFA_EOF:
		אם (rex.lnum == rex.reg_maxline && curc == NUL)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		اكسر;

	    حالة NFA_COMPOSING:
	    {
		nfa_state_T *end;

		result = match_composing(t->state->out, curc, clen);

		end = t->state->out1;	    // NFA_END_COMPOSING
		ADD_STATE_IF_MATCH(end);
		اكسر;
	    }

	    حالة NFA_NEWL:
		אם (curc == NUL && !rex.reg_line_lbr && REG_MULTI
						&& rex.lnum <= rex.reg_maxline)
		{
		    go_to_nextline = TRUE;
		    // Pass -1 for the offset, which means taking the position
		    // at the start of the next line.
		    add_state = t->state->out;
		    add_off = -1;
		}
		אחרת אם (curc == '\n' && rex.reg_line_lbr)
		{
		    // match \n as if it is an ordinary character
		    add_state = t->state->out;
		    add_off = 1;
		}
		اكسر;

	    حالة NFA_START_COLL:
	    حالة NFA_START_NEG_COLL:
	      {
		// What follows is a list of characters, until NFA_END_COLL.
		// One of them must match or none of them must match.
		nfa_state_T	*state;
		صحيح		result_if_matched;
		صحيح		c1, c2;

		// Never match EOL. If it's part of the collection it is added
		// as a separate state with an OR.
		אם (curc == NUL)
		    اكسر;

		state = t->state->out;
		result_if_matched = (t->state->c == NFA_START_COLL);
		עבור (;;)
		{
		    אם (state->c == NFA_COMPOSING)
		    {
			nfa_state_T *end;

			result = match_composing(t->state->out->out, curc,
									clen);

			אם (t->state->out->out1 != عدم
				&& t->state->out->out1->c == NFA_END_COMPOSING)
			{
			    end = t->state->out->out1;
			    ADD_STATE_IF_MATCH(end);
			}
			اكسر;
		    }
		    אם (state->c == NFA_END_COLL)
		    {
			result = !result_if_matched;
			اكسر;
		    }
		    אם (state->c == NFA_RANGE_MIN)
		    {
			c1 = state->val;
			state = state->out; // advance to NFA_RANGE_MAX
			c2 = state->val;
#ifdef ENABLE_LOG
			fprintf(log_fd, "NFA_RANGE_MIN curc=%d c1=%d c2=%d\n",
				curc, c1, c2);
#endif
			אם (curc >= c1 && curc <= c2)
			{
			    result = result_if_matched;
			    اكسر;
			}
			אם (rex.reg_ic)
			{
			    صحيح curc_low = MB_CASEFOLD(curc);
			    صحيح done = FALSE;

			    עבור ( ; c1 <= c2; ++c1)
				אם (MB_CASEFOLD(c1) == curc_low)
				{
				    result = result_if_matched;
				    done = TRUE;
				    اكسر;
				}
			    אם (done)
				اكسر;
			}
		    }
		    אחרת אם (state->c < 0 ? check_char_class(state->c, curc)
			       : (curc == state->c
				   || (rex.reg_ic && MB_CASEFOLD(curc)
						    == MB_CASEFOLD(state->c))))
		    {
			result = result_if_matched;
			اكسر;
		    }
		    state = state->out;
		}
		אם (result)
		{
		    // next state is in out of the NFA_END_COLL, out1 of
		    // START points to the END state
		    add_state = t->state->out1->out;
		    add_off = clen;
		}
		اكسر;
	      }

	    حالة NFA_ANY:
		// Any char except '\0', (end of input) does not match.
		אם (curc > 0)
		{
		    add_state = t->state->out;
		    add_off = clen;
		}
		اكسر;

	    حالة NFA_ANY_COMPOSING:
		// On a composing character skip over it.  Otherwise do
		// nothing.  Always matches.
		אם (enc_utf8 && utf_iscomposing(curc))
		{
		    add_off = clen;
		}
		אחרת
		{
		    add_here = TRUE;
		    add_off = 0;
		}
		add_state = t->state->out;
		اكسر;

	    /*
	     * Character classes like \a for alpha, \d for digit etc.
	     */
	    حالة NFA_IDENT:	//  \i
		result = vim_isIDc(curc);
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_SIDENT:	//  \I
		result = !VIM_ISDIGIT(curc) && vim_isIDc(curc);
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_KWORD:	//  \k
		result = vim_iswordp_buf(rex.input, rex.reg_buf);
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_SKWORD:	//  \K
		result = !VIM_ISDIGIT(curc)
				     && vim_iswordp_buf(rex.input, rex.reg_buf);
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_FNAME:	//  \f
		result = vim_isfilec(curc);
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_SFNAME:	//  \F
		result = !VIM_ISDIGIT(curc) && vim_isfilec(curc);
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_PRINT:	//  \p
		result = vim_isprintc(PTR2CHAR(rex.input));
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_SPRINT:	//  \P
		result = !VIM_ISDIGIT(curc) && vim_isprintc(PTR2CHAR(rex.input));
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_WHITE:	//  \s
		result = VIM_ISWHITE(curc);
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_NWHITE:	//  \S
		result = curc != NUL && !VIM_ISWHITE(curc);
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_DIGIT:	//  \d
		result = ri_digit(curc);
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_NDIGIT:	//  \D
		result = curc != NUL && !ri_digit(curc);
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_HEX:	//  \x
		result = ri_hex(curc);
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_NHEX:	//  \X
		result = curc != NUL && !ri_hex(curc);
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_OCTAL:	//  \o
		result = ri_octal(curc);
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_NOCTAL:	//  \O
		result = curc != NUL && !ri_octal(curc);
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_WORD:	//  \w
		result = ri_word(curc);
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_NWORD:	//  \W
		result = curc != NUL && !ri_word(curc);
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_HEAD:	//  \h
		result = ri_head(curc);
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_NHEAD:	//  \H
		result = curc != NUL && !ri_head(curc);
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_ALPHA:	//  \a
		result = ri_alpha(curc);
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_NALPHA:	//  \A
		result = curc != NUL && !ri_alpha(curc);
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_LOWER:	//  \l
		result = ri_lower(curc);
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_NLOWER:	//  \L
		result = curc != NUL && !ri_lower(curc);
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_UPPER:	//  \u
		result = ri_upper(curc);
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_NUPPER:	// \U
		result = curc != NUL && !ri_upper(curc);
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_LOWER_IC:	// [a-z]
		result = ri_lower(curc) || (rex.reg_ic && ri_upper(curc));
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_NLOWER_IC:	// [^a-z]
		result = curc != NUL
			&& !(ri_lower(curc) || (rex.reg_ic && ri_upper(curc)));
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_UPPER_IC:	// [A-Z]
		result = ri_upper(curc) || (rex.reg_ic && ri_lower(curc));
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_NUPPER_IC:	// ^[A-Z]
		result = curc != NUL
			&& !(ri_upper(curc) || (rex.reg_ic && ri_lower(curc)));
		ADD_STATE_IF_MATCH(t->state);
		اكسر;

	    حالة NFA_BACKREF1:
	    حالة NFA_BACKREF2:
	    حالة NFA_BACKREF3:
	    حالة NFA_BACKREF4:
	    حالة NFA_BACKREF5:
	    حالة NFA_BACKREF6:
	    حالة NFA_BACKREF7:
	    حالة NFA_BACKREF8:
	    حالة NFA_BACKREF9:
#ifdef FEAT_SYN_HL
	    حالة NFA_ZREF1:
	    حالة NFA_ZREF2:
	    حالة NFA_ZREF3:
	    حالة NFA_ZREF4:
	    حالة NFA_ZREF5:
	    حالة NFA_ZREF6:
	    حالة NFA_ZREF7:
	    حالة NFA_ZREF8:
	    حالة NFA_ZREF9:
#endif
		// \1 .. \9  \z1 .. \z9
	      {
		صحيح subidx;
		صحيح bytelen;

#ifdef FEAT_SYN_HL
		אם (t->state->c >= NFA_BACKREF1 && t->state->c <= NFA_BACKREF9)
#endif
		{
		    subidx = t->state->c - NFA_BACKREF1 + 1;
		    result = match_backref(&t->subs.norm, subidx, &bytelen);
		}
#ifdef FEAT_SYN_HL
		אחרת
		{
		    subidx = t->state->c - NFA_ZREF1 + 1;
		    result = match_zref(subidx, &bytelen);
		}
#endif

		אם (result)
		{
		    אם (bytelen == 0)
		    {
			// empty match always works, output of NFA_SKIP to be
			// used next
			add_here = TRUE;
			add_state = t->state->out->out;
		    }
		    אחרת אם (bytelen <= clen)
		    {
			// match current character, jump ahead to out of
			// NFA_SKIP
			add_state = t->state->out->out;
			add_off = clen;
		    }
		    אחרת
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			add_state = t->state->out;
			add_off = bytelen;
			add_count = bytelen - clen;
		    }
		}
		اكسر;
	      }
	    حالة NFA_SKIP:
	      // character of previous matching \1 .. \9  or \@>
	      אם (t->count - clen <= 0)
	      {
		  // end of match, go to what follows
		  add_state = t->state->out;
		  add_off = clen;
	      }
	      אחרת
	      {
		  // add state again with decremented count
		  add_state = t->state;
		  add_off = 0;
		  add_count = t->count - clen;
	      }
	      اكسر;

	    حالة NFA_LNUM:
	    حالة NFA_LNUM_GT:
	    حالة NFA_LNUM_LT:
		result = (REG_MULTI &&
			nfa_re_num_cmp(t->state->val, t->state->c - NFA_LNUM,
			    (long_u)(rex.lnum + rex.reg_firstlnum)));
		אם (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		اكسر;

	    حالة NFA_COL:
	    حالة NFA_COL_GT:
	    حالة NFA_COL_LT:
		result = nfa_re_num_cmp(t->state->val, t->state->c - NFA_COL,
			(long_u)(rex.input - rex.line) + 1);
		אם (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		اكسر;

	    حالة NFA_VCOL:
	    حالة NFA_VCOL_GT:
	    حالة NFA_VCOL_LT:
		{
		    صحيح     op = t->state->c - NFA_VCOL;
		    colnr_T col = (colnr_T)(rex.input - rex.line);
		    win_T   *wp = rex.reg_win == عدم ? curwin : rex.reg_win;

		    // Bail out quickly when there can't be a match, avoid the
		    // overhead of win_linetabsize() on long lines.
		    אם (op != 1 && col > t->state->val
			    * (has_mbyte ? MB_MAXBYTES : 1))
			اكسر;
		    result = FALSE;
		    אם (op == 1 && col - 1 > t->state->val && col > 100)
		    {
			صحيح ts = wp->w_buffer->b_p_ts;

			// Guess that a character won't use more columns than
			// 'tabstop', with a minimum of 4.
			אם (ts < 4)
			    ts = 4;
			result = col > t->state->val * ts;
		    }
		    אם (!result)
		    {
			linenr_T    lnum = REG_MULTI
					    ? rex.reg_firstlnum + rex.lnum : 1;
			long_u	    vcol;

			אם (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_buffer->b_ml.ml_line_count))
			    lnum = 1;
			vcol = (long_u)win_linetabsize(wp, lnum, rex.line, col);
			result = nfa_re_num_cmp(t->state->val, op, vcol + 1);
		    }
		    אם (result)
		    {
			add_here = TRUE;
			add_state = t->state->out;
		    }
		}
		اكسر;

	    حالة NFA_MARK:
	    حالة NFA_MARK_GT:
	    حالة NFA_MARK_LT:
	      {
		pos_T	*pos;
		size_t	col = REG_MULTI ? rex.input - rex.line : 0;

		pos = getmark_buf(rex.reg_buf, t->state->val, FALSE);

		// Line may have been freed, get it again.
		אם (REG_MULTI)
		{
		    rex.line = reg_getline(rex.lnum);
		    rex.input = rex.line + col;
		}

		// Compare the mark position to the match position, if the mark
		// exists and mark is set in reg_buf.
		אם (pos != عدم && pos->lnum > 0)
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
		    אם (result)
		    {
			add_here = TRUE;
			add_state = t->state->out;
		    }
		}
		اكسر;
	      }

	    حالة NFA_CURSOR:
		result = (rex.reg_win != عدم
			&& (rex.lnum + rex.reg_firstlnum
						 == rex.reg_win->w_cursor.lnum)
			&& ((colnr_T)(rex.input - rex.line)
						== rex.reg_win->w_cursor.col));
		אם (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		اكسر;

	    حالة NFA_VISUAL:
		result = reg_match_visual();
		אם (result)
		{
		    add_here = TRUE;
		    add_state = t->state->out;
		}
		اكسر;

	    حالة NFA_MOPEN1:
	    حالة NFA_MOPEN2:
	    حالة NFA_MOPEN3:
	    حالة NFA_MOPEN4:
	    حالة NFA_MOPEN5:
	    حالة NFA_MOPEN6:
	    حالة NFA_MOPEN7:
	    حالة NFA_MOPEN8:
	    حالة NFA_MOPEN9:
#ifdef FEAT_SYN_HL
	    حالة NFA_ZOPEN:
	    حالة NFA_ZOPEN1:
	    حالة NFA_ZOPEN2:
	    حالة NFA_ZOPEN3:
	    حالة NFA_ZOPEN4:
	    حالة NFA_ZOPEN5:
	    حالة NFA_ZOPEN6:
	    حالة NFA_ZOPEN7:
	    حالة NFA_ZOPEN8:
	    حالة NFA_ZOPEN9:
#endif
	    حالة NFA_NOPEN:
	    حالة NFA_ZSTART:
		// These states are only added to be able to bail out when
		// they are added again, nothing is to be done.
		اكسر;

	    افتراضي:	// regular character
	      {
		صحيح c = t->state->c;

#ifdef DEBUG
		אם (c < 0)
		    siemsg("Negative state char: %ld", (long)c);
#endif
		result = (c == curc);

		אם (!result && rex.reg_ic)
		    result = MB_CASEFOLD(c) == MB_CASEFOLD(curc);
		// If rex.reg_icombine is not set only skip over the character
		// itself.  When it is set skip over composing characters.
		אם (result && enc_utf8 && !rex.reg_icombine)
		    clen = utf_ptr2len(rex.input);
		ADD_STATE_IF_MATCH(t->state);
		اكسر;
	      }

	    } // switch (t->state->c)

	    אם (add_state != عدم)
	    {
		nfa_pim_T *pim;
		nfa_pim_T pim_copy;

		אם (t->pim.result == NFA_PIM_UNUSED)
		    pim = عدم;
		אחרת
		    pim = &t->pim;

		// Handle the postponed invisible match if the match might end
		// without advancing and before the end of the line.
		אם (pim != عدم && (clen == 0 || match_follows(add_state, 0)))
		{
		    אם (pim->result == NFA_PIM_TODO)
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
			אם (result != (pim->state->c == NFA_START_INVISIBLE_NEG
			     || pim->state->c == NFA_START_INVISIBLE_NEG_FIRST
			     || pim->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&pim->subs.norm, &m->norm);
#ifdef FEAT_SYN_HL
			    אם (rex.nfa_has_zsubexpr)
				copy_sub_off(&pim->subs.synt, &m->synt);
#endif
			}
		    }
		    אחרת
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
		    אם (result != (pim->state->c == NFA_START_INVISIBLE_NEG
			     || pim->state->c == NFA_START_INVISIBLE_NEG_FIRST
			     || pim->state->c
					   == NFA_START_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_START_INVISIBLE_BEFORE_NEG_FIRST))
		    {
			// Copy submatch info from the recursive call
			copy_sub_off(&t->subs.norm, &pim->subs.norm);
#ifdef FEAT_SYN_HL
			אם (rex.nfa_has_zsubexpr)
			    copy_sub_off(&t->subs.synt, &pim->subs.synt);
#endif
		    }
		    אחרת
			// look-behind match failed, don't add the state
			استمر;

		    // Postponed invisible match was handled, don't add it to
		    // following states.
		    pim = عدم;
		}

		// If "pim" points into l->t it will become invalid when
		// adding the state causes the list to be reallocated.  Make a
		// local copy to avoid that.
		אם (pim == &t->pim)
		{
		    copy_pim(&pim_copy, pim);
		    pim = &pim_copy;
		}

		אם (add_here)
		    r = addstate_here(thislist, add_state, &t->subs,
								pim, &listidx);
		אחרת
		{
		    r = addstate(nextlist, add_state, &t->subs, pim, add_off, 0);
		    אם (add_count > 0)
			nextlist->t[nextlist->n - 1].count = add_count;
		}
		אם (r == عدم)
		{
		    nfa_match = NFA_TOO_EXPENSIVE;
		    اذهب theend;
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
	אם (nfa_match == FALSE
		&& ((toplevel
			&& rex.lnum == 0
			&& clen != 0
			&& (rex.reg_maxcol == 0
			  || (colnr_T)(rex.input - rex.line) < rex.reg_maxcol))
		    || (nfa_endp != عدم
			&& (REG_MULTI
			    ? (rex.lnum < nfa_endp->se_u.pos.lnum
			       || (rex.lnum == nfa_endp->se_u.pos.lnum
				   && (صحيح)(rex.input - rex.line)
						    < nfa_endp->se_u.pos.col))
			    : rex.input < nfa_endp->se_u.ptr))))
	{
#ifdef ENABLE_LOG
	    fprintf(log_fd, "(---) STARTSTATE\n");
#endif
	    // Inline optimized code for addstate() if we know the state is
	    // the first MOPEN.
	    אם (toplevel)
	    {
		صحيح add = TRUE;
		صحيح c;

		אם (prog->regstart != NUL && clen != 0)
		{
		    אם (nextlist->n == 0)
		    {
			colnr_T col = (colnr_T)(rex.input - rex.line) + clen;

			// Nextlist is empty, we can skip ahead to the
			// character that must appear at the start.
			אם (skip_to_start(prog->regstart, &col) == FAIL)
			    اكسر;
#ifdef ENABLE_LOG
			fprintf(log_fd, "  Skipping ahead %d bytes to regstart\n",
				col - ((colnr_T)(rex.input - rex.line) + clen));
#endif
			rex.input = rex.line + col - clen;
		    }
		    אחרת
		    {
			// Checking if the required start character matches is
			// cheaper than adding a state that won't match.
			c = PTR2CHAR(rex.input + clen);
			אם (c != prog->regstart && (!rex.reg_ic
			     || MB_CASEFOLD(c) != MB_CASEFOLD(prog->regstart)))
			{
#ifdef ENABLE_LOG
			    fprintf(log_fd, "  Skipping start state, regstart does not match\n");
#endif
			    add = FALSE;
			}
		    }
		}

		אם (add)
		{
		    אם (REG_MULTI)
		    {
			m->norm.list.multi[0].start_col =
					 (colnr_T)(rex.input - rex.line) + clen;
			m->norm.orig_start_col =
					       m->norm.list.multi[0].start_col;
		    }
		    אחרת
			m->norm.list.line[0].start = rex.input + clen;
		    אם (addstate(nextlist, start->out, m, عدم, clen, 0) == عدم)
		    {
			nfa_match = NFA_TOO_EXPENSIVE;
			اذهب theend;
		    }
		}
	    }
	    אחרת
	    {
		char_u	    *save_line = rex.line;
		char_u	    *save_input = rex.input;
		linenr_T    save_lnum = rex.lnum;

		// At the end of a line the match can only start on the next
		// line, use that position instead of the line break.
		אם (REG_MULTI && clen == 0 && nfa_endp != عدم
					 && rex.lnum < nfa_endp->se_u.pos.lnum)
		{
		    char_u  *next_line = reg_getline(rex.lnum + 1);

		    אם (next_line != عدم)
		    {
			rex.line = next_line;
			rex.input = rex.line;
			++rex.lnum;
		    }
		}

		r = addstate(nextlist, start, m, عدم, clen, 0);

		rex.line = save_line;
		rex.input = save_input;
		rex.lnum = save_lnum;

		אם (r == عدم)
		{
		    nfa_match = NFA_TOO_EXPENSIVE;
		    اذهب theend;
		}
	    }
	}

#ifdef ENABLE_LOG
	fprintf(log_fd, ">>> Thislist had %d states available: ", thislist->n);
	{
	    صحيح i;

	    עבור (i = 0; i < thislist->n; i++)
		fprintf(log_fd, "%d  ", abs(thislist->t[i].state->id));
	}
	fprintf(log_fd, "\n");
#endif

nextchar:
	// Advance to the next character, or advance to the next line, or
	// finish.
	אם (clen != 0)
	    rex.input += clen;
	אחרת אם (go_to_nextline || (nfa_endp != عدم && REG_MULTI
					&& rex.lnum < nfa_endp->se_u.pos.lnum))
	    reg_nextline();
	אחרת
	    اكسر;

	// Allow interrupting with CTRL-C.
	line_breakcheck();
	אם (got_int)
	    اكسر;
#ifdef FEAT_RELTIME
	אם (nfa_did_time_out())
	    اكسر;
#endif
    }

#ifdef ENABLE_LOG
    אם (log_fd != stderr)
	fclose(log_fd);
    log_fd = عدم;
#endif

theend:
    // Cache list buffers in prog for reuse, or free if prog already has
    // cached buffers (recursive call case).
    אם (prog->listbuf[0] == عدم && list[0].t != عدم && list[1].t != عدم)
    {
	prog->listbuf[0] = list[0].t;
	prog->listbuf[1] = list[1].t;
    }
    אחרת
    {
	vim_free(list[0].t);
	vim_free(list[1].t);
    }
    vim_free(listids);
#undef ADD_STATE_IF_MATCH
#ifdef NFA_REGEXP_DEBUG_LOG
    fclose(debug);
#endif

    החזר nfa_match;
}

/*
 * Try match of "prog" with at rex.line["col"].
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    ساكن long
nfa_regtry(
    nfa_regprog_T   *prog,
    colnr_T	    col,
    صحيح		    *timed_out UNUSED)	// flag set on timeout or NULL
{
    صحيح		i;
    regsubs_T	subs, m;
    nfa_state_T	*start = prog->start;
    صحيح		result;
#ifdef ENABLE_LOG
    FILE	*f;
#endif

    rex.input = rex.line + col;
#ifdef FEAT_RELTIME
    nfa_timed_out = timed_out;
#endif

#ifdef ENABLE_LOG
    f = fopen(NFA_REGEXP_RUN_LOG, "a");
    אם (f != عدم)
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
    אחרת
	emsg("Could not open temporary log file for writing");
#endif

    clear_sub(&subs.norm);
    clear_sub(&m.norm);
#ifdef FEAT_SYN_HL
    clear_sub(&subs.synt);
    clear_sub(&m.synt);
#endif

    result = nfa_regmatch(prog, start, &subs, &m);
    אם (result == FALSE)
	החזר 0;
    אחרת אם (result == NFA_TOO_EXPENSIVE)
	החזר result;

    cleanup_subexpr();
    אם (REG_MULTI)
    {
	עבור (i = 0; i < subs.norm.in_use; i++)
	{
	    rex.reg_startpos[i].lnum = subs.norm.list.multi[i].start_lnum;
	    rex.reg_startpos[i].col = subs.norm.list.multi[i].start_col;

	    rex.reg_endpos[i].lnum = subs.norm.list.multi[i].end_lnum;
	    rex.reg_endpos[i].col = subs.norm.list.multi[i].end_col;
	}
	אם (rex.reg_mmatch != عدم)
	    rex.reg_mmatch->rmm_matchcol = subs.norm.orig_start_col;

	אם (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].col = col;
	}
	אם (rex.reg_endpos[0].lnum < 0)
	{
	    // pattern has a \ze but it didn't match, use current end
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].col = (صحيح)(rex.input - rex.line);
	}
	אחרת
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    אחרת
    {
	עבור (i = 0; i < subs.norm.in_use; i++)
	{
	    rex.reg_startp[i] = subs.norm.list.line[i].start;
	    rex.reg_endp[i] = subs.norm.list.line[i].end;
	}

	אם (rex.reg_startp[0] == عدم)
	    rex.reg_startp[0] = rex.line + col;
	אם (rex.reg_endp[0] == عدم)
	    rex.reg_endp[0] = rex.input;
    }

#ifdef FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = عدم;

    אם (prog->reghasz == REX_SET)
    {
	cleanup_zsubexpr();
	re_extmatch_out = make_extmatch();
	אם (re_extmatch_out == عدم)
	    החזר 0;
	// Loop over \z1, \z2, etc.  There is no \z0.
	עבור (i = 1; i < subs.synt.in_use; i++)
	{
	    אם (REG_MULTI)
	    {
		بنية multipos *mpos = &subs.synt.list.multi[i];

		// Only accept single line matches that are valid.
		אם (mpos->start_lnum >= 0
			&& mpos->start_lnum == mpos->end_lnum
			&& mpos->end_col >= mpos->start_col)
		    re_extmatch_out->matches[i] =
			vim_strnsave(reg_getline(mpos->start_lnum)
							    + mpos->start_col,
					     mpos->end_col - mpos->start_col);
	    }
	    אחרת
	    {
		بنية linepos *lpos = &subs.synt.list.line[i];

		אם (lpos->start != عدم && lpos->end != عدم)
		    re_extmatch_out->matches[i] =
			    vim_strnsave(lpos->start, lpos->end - lpos->start);
	    }
	}
    }
#endif

    החזר 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 *
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    ساكن long
nfa_regexec_both(
    char_u	*line,
    colnr_T	startcol,	// column to start looking for match
    صحيح		*timed_out)	// flag set on timeout or NULL
{
    nfa_regprog_T   *prog;
    long	    retval = 0L;
    صحيح		    i;
    colnr_T	    col = startcol;

    אם (REG_MULTI)
    {
	prog = (nfa_regprog_T *)rex.reg_mmatch->regprog;
	line = reg_getline((linenr_T)0);    // relative to the cursor
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    אחרת
    {
	prog = (nfa_regprog_T *)rex.reg_match->regprog;
	rex.reg_startp = rex.reg_match->startp;
	rex.reg_endp = rex.reg_match->endp;
    }

    // Be paranoid...
    אם (prog == عدم || line == عدم)
    {
	iemsg(e_null_argument);
	اذهب theend;
    }

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    אם (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    אחרת אם (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    אם (prog->regflags & RF_ICOMBINE)
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

    אם (prog->reganch && col > 0)
	החזר 0L;

    rex.need_clear_subexpr = TRUE;
#ifdef FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    אם (prog->reghasz == REX_SET)
    {
	rex.nfa_has_zsubexpr = TRUE;
	rex.need_clear_zsubexpr = TRUE;
    }
    אחרת
    {
	rex.nfa_has_zsubexpr = FALSE;
	rex.need_clear_zsubexpr = FALSE;
    }
#endif

    אם (prog->regstart != NUL)
    {
	// Skip ahead until a character we know the match must start with.
	// When there is none there is no match.
	אם (skip_to_start(prog->regstart, &col) == FAIL)
	    החזר 0L;

	// If match_text is set it contains the full text that must match.
	// Nothing else to try. Doesn't handle combining chars well.
	אם (prog->match_text != عدم && *prog->match_text != NUL && !rex.reg_icombine)
	{
	    retval = find_match_text(&col, prog->regstart, prog->match_text);
	    אם (REG_MULTI)
		rex.reg_mmatch->rmm_matchcol = col;
	    אחרת
		rex.reg_match->rm_matchcol = col;
	    החזר retval;
	}
    }

    // If the start column is past the maximum column: no need to try.
    אם (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	اذهب theend;

    // Set the "nstate" used by nfa_regcomp() to zero to trigger an error when
    // it's accidentally used during execution.
    nstate = 0;
    עבור (i = 0; i < prog->nstate; ++i)
    {
	prog->state[i].id = i;
	prog->state[i].lastlist[0] = 0;
	prog->state[i].lastlist[1] = 0;
    }

    retval = nfa_regtry(prog, col, timed_out);

#ifdef DEBUG
    nfa_regengine.expr = عدم;
#endif

theend:
    אם (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs and
	// \ze are used.
	אם (REG_MULTI)
	{
	    lpos_T *start = &rex.reg_mmatch->startpos[0];
	    lpos_T *end = &rex.reg_mmatch->endpos[0];

	    אם (end->lnum < start->lnum
			|| (end->lnum == start->lnum && end->col < start->col))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];
	}
	אחרת
	{
	    אם (rex.reg_match->endp[0] < rex.reg_match->startp[0])
		rex.reg_match->endp[0] = rex.reg_match->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_match->rm_matchcol = col;
	}
    }

    החזר retval;
}

/*
 * Compile a regular expression into internal code for the NFA matcher.
 * Returns the program in allocated space.  Returns NULL for an error.
 */
    ساكن regprog_T *
nfa_regcomp(char_u *expr, صحيح re_flags)
{
    nfa_regprog_T	*prog = عدم;
    size_t		prog_size;
    صحيح			*postfix;

    אם (expr == عدم)
	החזר عدم;

#ifdef DEBUG
    nfa_regengine.expr = expr;
#endif
    nfa_re_flags = re_flags;

    init_class_tab();

    אם (nfa_regcomp_start(expr, re_flags) == FAIL)
	החזר عدم;

    // Build postfix form of the regexp. Needed to build the NFA
    // (and count its size).
    postfix = re2post();
    אם (postfix == عدم)
	اذهب fail;	    // Cascaded (syntax?) error

    /*
     * In order to build the NFA, we parse the input regexp twice:
     * 1. first pass to count size (so we can allocate space)
     * 2. second to emit code
     */
#ifdef ENABLE_LOG
    {
	FILE *f = fopen(NFA_REGEXP_RUN_LOG, "a");

	אם (f != عدم)
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
    prog_size = offsetof(nfa_regprog_T, state) + حجم(nfa_state_T) * nstate;
    prog = alloc(prog_size);
    אם (prog == عدم)
	اذهب fail;
    state_ptr = prog->state;
    prog->re_in_use = FALSE;
    prog->listbuf[0] = عدم;
    prog->listbuf[1] = عدم;

    /*
     * PASS 2
     * Build the NFA
     */
    prog->start = post2nfa(postfix, post_ptr, FALSE);
    אם (prog->start == عدم)
	اذهب fail;

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
    nfa_regengine.expr = عدم;
#endif

out:
    // The postfix list and fragment stack are reused by the next compilation
    // (and freed in free_regexp_stuff()), but drop ones that grew large for a
    // big pattern to avoid holding much memory.
    אם (post_start_len > NFA_POSTFIX_KEEP)
    {
	VIM_CLEAR(post_start);
	post_start_len = 0;
    }
    post_ptr = post_end = عدم;
    אם (nfa_stack_len > NFA_STACK_KEEP)
    {
	VIM_CLEAR(nfa_stack);
	nfa_stack_len = 0;
    }
    state_ptr = عدم;
    החזר (regprog_T *)prog;

fail:
    VIM_CLEAR(prog);
#ifdef ENABLE_LOG
    nfa_postfix_dump(expr, FAIL);
#endif
#ifdef DEBUG
    nfa_regengine.expr = عدم;
#endif
    اذهب out;
}

/*
 * Free a compiled regexp program, returned by nfa_regcomp().
 */
    ساكن فراغ
nfa_regfree(regprog_T *prog)
{
    אם (prog == عدم)
	החזר;

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
    ساكن صحيح
nfa_regexec_nl(
    regmatch_T	*rmp,
    char_u	*line,	// string to match against
    colnr_T	col,	// column to start looking for match
    صحيح		line_lbr)
{
    rex.reg_match = rmp;
    rex.reg_mmatch = عدم;
    rex.reg_maxline = 0;
    rex.reg_line_lbr = line_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = عدم;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;
    החזר nfa_regexec_both(line, col, عدم);
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
    ساكن long
nfa_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	col,		// column to start looking for match
    صحيح		*timed_out)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    החזר nfa_regexec_both(عدم, col, timed_out);
}

#ifdef DEBUG
# undef ENABLE_LOG
#endif
