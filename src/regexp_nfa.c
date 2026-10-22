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
#جے_تعریف_شدہ DEBUG
# define NFA_REGEXP_ERROR_LOG	"nfa_regexp_error.log"
# define ENABLE_LOG
# define NFA_REGEXP_DUMP_LOG	"nfa_regexp_dump.log"
# define NFA_REGEXP_لگاتار_LOG	"nfa_regexp_run.log"
# define NFA_REGEXP_DEBUG_LOG	"nfa_regexp_debug.log"
#جے_انت

// Added to NFA_ANY - NFA_NUPPER_IC to include a NL.
#تعریف_کرو NFA_جوڑو_NL		31

تعداد
{
    NFA_ونڈو = -1024,
    NFA_میل,
    NFA_خالی_تھاواں,			    // matches 0-length

    NFA_شروع_COLL,		    // [abc] start
    NFA_سرا_COLL,		    // [abc] end
    NFA_شروع_NEG_COLL,		    // [^abc] start
    NFA_سرا_NEG_COLL,		    // [^abc] end (postfix only)
    NFA_RANGE,			    // range of the two previous items
				    // (postfix only)
    NFA_RANGE_MIN,		    // low end of a range
    NFA_RANGE_MAX,		    // high end of a range

    NFA_CONCAT,			    // concatenate two previous items (postfix
				    // only)
    NFA_OR,			    // \| (postfix only)
    NFA_تارا,			    // greedy * (postfix only)
    NFA_تارا_NONGREEDY,		    // non-greedy * (postfix only)
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
    NFA_شروع_INVISIBLE,
    NFA_شروع_INVISIBLE_پہلا,
    NFA_شروع_INVISIBLE_NEG,
    NFA_شروع_INVISIBLE_NEG_پہلا,
    NFA_شروع_INVISIBLE_BEFORE,
    NFA_شروع_INVISIBLE_BEFORE_پہلا,
    NFA_شروع_INVISIBLE_BEFORE_NEG,
    NFA_شروع_INVISIBLE_BEFORE_NEG_پہلا,
    NFA_شروع_نمونہ,
    NFA_سرا_INVISIBLE,
    NFA_سرا_INVISIBLE_NEG,
    NFA_سرا_نمونہ,
    NFA_COMPOSING,		    // Next nodes in NFA are part of the
				    // composing multibyte char
    NFA_سرا_COMPOSING,		    // End of a composing char in the NFA
    NFA_ANY_COMPOSING,		    // \%C: Any composing characters.
    NFA_OPT_CHARS,		    // \%[abc]

    // The following are used only in the postfix form, not in the NFA
    NFA_PREV_ATOM_NO_چوڑائی,	    // Used for \@=
    NFA_PREV_ATOM_NO_چوڑائی_NEG,	    // Used for \@!
    NFA_PREV_ATOM_JUST_BEFORE,	    // Used for \@<=
    NFA_PREV_ATOM_JUST_BEFORE_NEG,  // Used for \@<!
    NFA_PREV_ATOM_LIKE_نمونہ,	    // Used for \@>

    NFA_BACKREF1,		    // \1
    NFA_BACKREF2,		    // \2
    NFA_BACKREF3,		    // \3
    NFA_BACKREF4,		    // \4
    NFA_BACKREF5,		    // \5
    NFA_BACKREF6,		    // \6
    NFA_BACKREF7,		    // \7
    NFA_BACKREF8,		    // \8
    NFA_BACKREF9,		    // \9
#جے_تعریف_شدہ FEAT_SYN_HL
    NFA_ZREF1,			    // \z1
    NFA_ZREF2,			    // \z2
    NFA_ZREF3,			    // \z3
    NFA_ZREF4,			    // \z4
    NFA_ZREF5,			    // \z5
    NFA_ZREF6,			    // \z6
    NFA_ZREF7,			    // \z7
    NFA_ZREF8,			    // \z8
    NFA_ZREF9,			    // \z9
#جے_انت
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

#جے_تعریف_شدہ FEAT_SYN_HL
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
#جے_انت

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
    NFA_لفظ,		//	Match word char
    NFA_NWORD,		//	Match non-word char
    NFA_سر,		//	Match head char
    NFA_NHEAD,		//	Match non-head char
    NFA_ALPHA,		//	Match alpha char
    NFA_NALPHA,		//	Match non-alpha char
    NFA_ہیٹھلا,		//	Match lowercase char
    NFA_NLOWER,		//	Match non-lowercase char
    NFA_اتلا,		//	Match uppercase char
    NFA_NUPPER,		//	Match non-uppercase char
    NFA_ہیٹھلا_IC,	//	Match [a-z]
    NFA_NLOWER_IC,	//	Match [^a-z]
    NFA_اتلا_IC,	//	Match [A-Z]
    NFA_NUPPER_IC,	//	Match [^A-Z]

    NFA_پہلا_NL = NFA_ANY + NFA_جوڑو_NL,
    NFA_آخری_NL = NFA_NUPPER_IC + NFA_جوڑو_NL,

    NFA_CURSOR,		//	Match cursor pos
    NFA_LNUM,		//	Match line number
    NFA_LNUM_GT,	//	Match > line number
    NFA_LNUM_LT,	//	Match < line number
    NFA_خانہ,		//	Match cursor column
    NFA_خانہ_GT,		//	Match > cursor column
    NFA_خانہ_LT,		//	Match < cursor column
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
    NFA_CLASS_گراف,
    NFA_CLASS_ہیٹھلا,
    NFA_CLASS_PRINT,
    NFA_CLASS_PUNCT,
    NFA_CLASS_SPACE,
    NFA_CLASS_اتلا,
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
    NFA_لفظ, NFA_NWORD, NFA_سر, NFA_NHEAD,
    NFA_ALPHA, NFA_NALPHA, NFA_ہیٹھلا, NFA_NLOWER,
    NFA_اتلا, NFA_NUPPER
};

// Variables only used in nfa_regcomp() and descendants.
ساكن صحيح nfa_re_flags; // re_flags passed to nfa_regcomp()
ساكن صحيح *post_شروع;  // holds the postfix form of r.e.
ساكن صحيح post_شروع_len;  // size of allocated post_start (in ints)
ساكن صحيح *post_سرا;
ساكن صحيح *post_ptr;
ساكن صحيح nfa_reg_parse_ڈونگھائی;	// nesting depth in nfa_reg()

// The postfix list (post_start) and fragment stack (nfa_stack) are reused
// across compilations; a buffer that grew past these sizes for a big pattern
// is freed afterwards instead of being kept, to avoid holding much memory.
#تعریف_کرو NFA_POSTFIX_KEEP	10000	// number of ints (~40 Kbyte)
#تعریف_کرو NFA_ڈھیر_KEEP		4000	// number of Frag_T (~64 Kbyte)

// Set when the pattern should use the NFA engine.
// E.g. [[:upper:]] only allows 8bit characters for BT engine,
// while NFA engine handles multibyte characters correctly.
ساكن صحيح چاہندا_nfa;

ساكن صحيح nstate;	// Number of states in the NFA.
ساكن صحيح istate;	// Index in the state vector, used in alloc_state()

// If not NULL match must end at this position
ساكن save_se_T *nfa_endp = عدم;

// 0 for first call to nfa_regmatch(), 1 for recursive call.
ساكن صحيح nfa_ll_اشاریہ = 0;

ساكن صحيح realloc_post_list(فراغ);
ساكن صحيح nfa_reg(صحيح paren);
#جے_تعریف_شدہ DEBUG
ساكن فراغ nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent);
#جے_انت
ساكن صحيح میل_follows(nfa_state_T *startstate, صحيح ڈونگھائی);
ساكن صحيح failure_chance(nfa_state_T *state, صحيح ڈونگھائی);

// helper functions used when doing re2post() ... regatom() parsing
#تعریف_کرو EMIT(c)	افعل {				\
		    إذا (post_ptr >= post_سرا && realloc_post_list() == FAIL) \
			أرجع FAIL;		\
		    *post_ptr++ = c;		\
		} طالما (0)

/*
 * Initialize internal variables before NFA compilation.
 * Return OK on success, FAIL otherwise.
 */
    ساكن صحيح
nfa_regcomp_شروع(
    char_u	*اظہاریہ,
    صحيح		re_flags)	    // see vim_regcomp()
{
    size_t	postfix_ناپ;
    صحيح		nstate_max;

    nstate = 0;
    istate = 0;
    // A reasonable estimation for maximum size
    nstate_max = (صحيح)(STRLEN(اظہاریہ) + 1) * 25;

    // Some items blow up in size, such as [A-z].  Add more space for that.
    // When it is still not enough realloc_post_list() will be used.
    nstate_max += 1000;

    // Size for postfix representation of expr.
    postfix_ناپ = حجم(صحيح) * nstate_max;

    // Reuse the postfix buffer across compilations, only growing it when the
    // estimate exceeds the current size; it is freed in free_regexp_stuff().
    إذا (post_شروع == عدم || post_شروع_len < nstate_max)
    {
	vim_free(post_شروع);
	post_شروع = alloc(postfix_ناپ);
	إذا (post_شروع == عدم)
	{
	    post_شروع_len = 0;
	    أرجع FAIL;
	}
	post_شروع_len = nstate_max;
    }
    post_ptr = post_شروع;
    post_سرا = post_شروع + post_شروع_len;
    چاہندا_nfa = FALSE;
    rex.nfa_has_zend = FALSE;
    rex.nfa_has_backref = FALSE;
    nfa_reg_parse_ڈونگھائی = 0;

    // shared with BT engine
    regcomp_شروع(اظہاریہ, re_flags);

    أرجع OK;
}

/*
 * Figure out if the NFA state list starts with an anchor, must match at start
 * of the line.
 */
    ساكن صحيح
nfa_get_reganch(nfa_state_T *شروع, صحيح ڈونگھائی)
{
    nfa_state_T *p = شروع;

    إذا (ڈونگھائی > 4)
	أرجع 0;

    طالما (p != عدم)
    {
	بدل (p->c)
	{
	    حالة NFA_BOL:
	    حالة NFA_BOF:
		أرجع 1; // yes!

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
#جے_تعریف_شدہ FEAT_SYN_HL
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
#جے_انت
		p = p->باہرلا;
		اكسر;

	    حالة NFA_ونڈو:
		أرجع nfa_get_reganch(p->باہرلا, ڈونگھائی + 1)
				       && nfa_get_reganch(p->out1, ڈونگھائی + 1);

	    افتراضي:
		أرجع 0; // noooo
	}
    }
    أرجع 0;
}

/*
 * Figure out if the NFA state list starts with a character which must match
 * at start of the match.
 */
    ساكن صحيح
nfa_get_regstart(nfa_state_T *شروع, صحيح ڈونگھائی)
{
    nfa_state_T *p = شروع;

    إذا (ڈونگھائی > 4)
	أرجع 0;

    طالما (p != عدم)
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
	    حالة NFA_خانہ:
	    حالة NFA_خانہ_GT:
	    حالة NFA_خانہ_LT:
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
#جے_تعریف_شدہ FEAT_SYN_HL
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
#جے_انت
		p = p->باہرلا;
		اكسر;

	    حالة NFA_ونڈو:
	    {
		صحيح c1 = nfa_get_regstart(p->باہرلا, ڈونگھائی + 1);
		صحيح c2 = nfa_get_regstart(p->out1, ڈونگھائی + 1);

		إذا (c1 == c2)
		    أرجع c1; // yes!
		أرجع 0;
	    }

	    افتراضي:
		إذا (p->c > 0)
		    أرجع p->c; // yes!
		أرجع 0;
	}
    }
    أرجع 0;
}

/*
 * Figure out if the NFA state list contains just literal text and nothing
 * else.  If so return a string in allocated memory with what must match after
 * regstart.  Otherwise return NULL.
 */
    ساكن char_u *
nfa_get_میل_متن(nfa_state_T *شروع)
{
    nfa_state_T *p = شروع;
    صحيح		len = 0;
    char_u	*ret;
    char_u	*s;

    إذا (p->c != NFA_MOPEN)
	أرجع عدم; // just in case
    p = p->باہرلا;
    طالما (p->c > 0)
    {
	len += MB_CHAR2LEN(p->c);
	p = p->باہرلا;
    }
    إذا (p->c != NFA_MCLOSE || p->باہرلا->c != NFA_میل)
	أرجع عدم;

    ret = alloc(len);
    إذا (ret == عدم)
	أرجع عدم;

    p = شروع->باہرلا->باہرلا; // skip first char, it goes into regstart
    s = ret;
    طالما (p->c > 0)
    {
	إذا (has_mbyte)
	    s += (*mb_char2bytes)(p->c, s);
	وإلا
	    *s++ = p->c;
	p = p->باہرلا;
    }
    *s = NUL;
    أرجع ret;
}

/*
 * Allocate more space for post_start.  Called when
 * running above the estimated number of states.
 */
    ساكن صحيح
realloc_post_list(فراغ)
{
    صحيح   nstate_max = (صحيح)(post_سرا - post_شروع);
    صحيح   new_max;
    صحيح   *new_شروع;
    صحيح	  *old_شروع;

    // For weird patterns the number of states can be very high. Increasing by
    // 50% seems a reasonable compromise between memory use and speed.
    new_max = nstate_max * 3 / 2;
    new_شروع = ALLOC_MULT(صحيح, new_max);
    إذا (new_شروع == عدم)
	أرجع FAIL;
    mch_memmove(new_شروع, post_شروع, nstate_max * حجم(صحيح));
    old_شروع = post_شروع;
    post_شروع = new_شروع;
    post_شروع_len = new_max;
    post_ptr = new_شروع + (post_ptr - old_شروع);
    post_سرا = post_شروع + new_max;
    vim_free(old_شروع);
    أرجع OK;
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
nfa_recognize_char_class(char_u *شروع, char_u *سرا, صحيح extra_newl)
{
    صحيح		newl;
    صحيح		config = get_char_class_بٹ(شروع, سرا, &newl);

    إذا (config < 0)
	أرجع FAIL;

    إذا (extra_newl == TRUE)
	newl = TRUE;

    إذا (newl == TRUE)
	extra_newl = NFA_جوڑو_NL;

    بدل (config)
    {
	حالة CLASS_o9:
	    أرجع extra_newl + NFA_DIGIT;
	حالة CLASS_not |  CLASS_o9:
	    أرجع extra_newl + NFA_NDIGIT;
	حالة CLASS_af | CLASS_AF | CLASS_o9:
	    أرجع extra_newl + NFA_HEX;
	حالة CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    أرجع extra_newl + NFA_NHEX;
	حالة CLASS_o7:
	    أرجع extra_newl + NFA_OCTAL;
	حالة CLASS_not | CLASS_o7:
	    أرجع extra_newl + NFA_NOCTAL;
	حالة CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    أرجع extra_newl + NFA_لفظ;
	حالة CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    أرجع extra_newl + NFA_NWORD;
	حالة CLASS_az | CLASS_AZ | CLASS_underscore:
	    أرجع extra_newl + NFA_سر;
	حالة CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    أرجع extra_newl + NFA_NHEAD;
	حالة CLASS_az | CLASS_AZ:
	    أرجع extra_newl + NFA_ALPHA;
	حالة CLASS_not | CLASS_az | CLASS_AZ:
	    أرجع extra_newl + NFA_NALPHA;
	حالة CLASS_az:
	   أرجع extra_newl + NFA_ہیٹھلا_IC;
	حالة CLASS_not | CLASS_az:
	    أرجع extra_newl + NFA_NLOWER_IC;
	حالة CLASS_AZ:
	    أرجع extra_newl + NFA_اتلا_IC;
	حالة CLASS_not | CLASS_AZ:
	    أرجع extra_newl + NFA_NUPPER_IC;
    }
    أرجع FAIL;
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
#تعریف_کرو EMIT2(c)    EMIT(c); EMIT(NFA_CONCAT);

    إذا (enc_utf8 || STRCMP(p_enc, "latin1") == 0
					 || STRCMP(p_enc, "iso-8859-15") == 0)
    {
#تعریف_کرو A_grave 0xc0
#تعریف_کرو A_acute 0xc1
#تعریف_کرو A_circumflex 0xc2
#تعریف_کرو A_virguilla 0xc3
#تعریف_کرو A_diaeresis 0xc4
#تعریف_کرو A_ring 0xc5
#تعریف_کرو C_cedilla 0xc7
#تعریف_کرو E_grave 0xc8
#تعریف_کرو E_acute 0xc9
#تعریف_کرو E_circumflex 0xca
#تعریف_کرو E_diaeresis 0xcb
#تعریف_کرو I_grave 0xcc
#تعریف_کرو I_acute 0xcd
#تعریف_کرو I_circumflex 0xce
#تعریف_کرو I_diaeresis 0xcf
#تعریف_کرو N_virguilla 0xd1
#تعریف_کرو O_grave 0xd2
#تعریف_کرو O_acute 0xd3
#تعریف_کرو O_circumflex 0xd4
#تعریف_کرو O_virguilla 0xd5
#تعریف_کرو O_diaeresis 0xd6
#تعریف_کرو O_slash 0xd8
#تعریف_کرو U_grave 0xd9
#تعریف_کرو U_acute 0xda
#تعریف_کرو U_circumflex 0xdb
#تعریف_کرو U_diaeresis 0xdc
#تعریف_کرو Y_acute 0xdd
#تعریف_کرو a_grave 0xe0
#تعریف_کرو a_acute 0xe1
#تعریف_کرو a_circumflex 0xe2
#تعریف_کرو a_virguilla 0xe3
#تعریف_کرو a_diaeresis 0xe4
#تعریف_کرو a_ring 0xe5
#تعریف_کرو c_cedilla 0xe7
#تعریف_کرو e_grave 0xe8
#تعریف_کرو e_acute 0xe9
#تعریف_کرو e_circumflex 0xea
#تعریف_کرو e_diaeresis 0xeb
#تعریف_کرو i_grave 0xec
#تعریف_کرو i_acute 0xed
#تعریف_کرو i_circumflex 0xee
#تعریف_کرو i_diaeresis 0xef
#تعریف_کرو n_virguilla 0xf1
#تعریف_کرو o_grave 0xf2
#تعریف_کرو o_acute 0xf3
#تعریف_کرو o_circumflex 0xf4
#تعریف_کرو o_virguilla 0xf5
#تعریف_کرو o_diaeresis 0xf6
#تعریف_کرو o_slash 0xf8
#تعریف_کرو u_grave 0xf9
#تعریف_کرو u_acute 0xfa
#تعریف_کرو u_circumflex 0xfb
#تعریف_کرو u_diaeresis 0xfc
#تعریف_کرو y_acute 0xfd
#تعریف_کرو y_diaeresis 0xff
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
		    أرجع OK;

	    حالة 'B': حالة 0x181: حالة 0x243: حالة 0x1e02:
	    حالة 0x1e04: حالة 0x1e06:
		    EMIT2('B')
		    EMIT2(0x181) EMIT2(0x243) EMIT2(0x1e02)
		    EMIT2(0x1e04) EMIT2(0x1e06)
		    أرجع OK;

	    حالة 'C': حالة C_cedilla: حالة 0x106: حالة 0x108:
	    حالة 0x10a: حالة 0x10c: حالة 0x187: حالة 0x23b:
	    حالة 0x1e08: حالة 0xa792:
		    EMIT2('C') EMIT2(C_cedilla)
		    EMIT2(0x106) EMIT2(0x108) EMIT2(0x10a)
		    EMIT2(0x10c) EMIT2(0x187) EMIT2(0x23b)
		    EMIT2(0x1e08) EMIT2(0xa792)
		    أرجع OK;

	    حالة 'D': حالة 0x10e: حالة 0x110: حالة 0x18a:
	    حالة 0x1e0a: حالة 0x1e0c: حالة 0x1e0e: حالة 0x1e10:
	    حالة 0x1e12:
		    EMIT2('D') EMIT2(0x10e) EMIT2(0x110) EMIT2(0x18a)
		    EMIT2(0x1e0a) EMIT2(0x1e0c) EMIT2(0x1e0e)
		    EMIT2(0x1e10) EMIT2(0x1e12)
		    أرجع OK;

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
		    أرجع OK;

	    حالة 'F': حالة 0x191: حالة 0x1e1e: حالة 0xa798:
		    EMIT2('F') EMIT2(0x191) EMIT2(0x1e1e) EMIT2(0xa798)
		    أرجع OK;

	    حالة 'G': حالة 0x11c: حالة 0x11e: حالة 0x120:
	    حالة 0x122: حالة 0x193: حالة 0x1e4: حالة 0x1e6:
	    حالة 0x1f4: حالة 0x1e20: حالة 0xa7a0:
		    EMIT2('G') EMIT2(0x11c) EMIT2(0x11e) EMIT2(0x120)
		    EMIT2(0x122) EMIT2(0x193) EMIT2(0x1e4)
		    EMIT2(0x1e6) EMIT2(0x1f4) EMIT2(0x1e20)
		    EMIT2(0xa7a0)
		    أرجع OK;

	    حالة 'H': حالة 0x124: حالة 0x126: حالة 0x21e:
	    حالة 0x1e22: حالة 0x1e24: حالة 0x1e26: حالة 0x1e28:
	    حالة 0x1e2a: حالة 0x2c67:
		    EMIT2('H') EMIT2(0x124) EMIT2(0x126) EMIT2(0x21e)
		    EMIT2(0x1e22) EMIT2(0x1e24) EMIT2(0x1e26)
		    EMIT2(0x1e28) EMIT2(0x1e2a) EMIT2(0x2c67)
		    أرجع OK;

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
		    أرجع OK;

	    حالة 'J': حالة 0x134: حالة 0x248:
		    EMIT2('J') EMIT2(0x134) EMIT2(0x248)
		    أرجع OK;

	    حالة 'K': حالة 0x136: حالة 0x198: حالة 0x1e8: حالة 0x1e30:
	    حالة 0x1e32: حالة 0x1e34: حالة 0x2c69: حالة 0xa740:
		    EMIT2('K') EMIT2(0x136) EMIT2(0x198) EMIT2(0x1e8)
		    EMIT2(0x1e30) EMIT2(0x1e32) EMIT2(0x1e34)
		    EMIT2(0x2c69) EMIT2(0xa740)
		    أرجع OK;

	    حالة 'L': حالة 0x139: حالة 0x13b: حالة 0x13d:
	    حالة 0x13f: حالة 0x141: حالة 0x23d: حالة 0x1e36:
	    حالة 0x1e38: حالة 0x1e3a: حالة 0x1e3c: حالة 0x2c60:
		    EMIT2('L') EMIT2(0x139) EMIT2(0x13b)
		    EMIT2(0x13d) EMIT2(0x13f) EMIT2(0x141)
		    EMIT2(0x23d) EMIT2(0x1e36) EMIT2(0x1e38)
		    EMIT2(0x1e3a) EMIT2(0x1e3c) EMIT2(0x2c60)
		    أرجع OK;

	    حالة 'M': حالة 0x1e3e: حالة 0x1e40: حالة 0x1e42:
		    EMIT2('M') EMIT2(0x1e3e) EMIT2(0x1e40)
		    EMIT2(0x1e42)
		    أرجع OK;

	    حالة 'N': حالة N_virguilla:
	    حالة 0x143: حالة 0x145: حالة 0x147: حالة 0x1f8:
	    حالة 0x1e44: حالة 0x1e46: حالة 0x1e48: حالة 0x1e4a:
	    حالة 0xa7a4:
		    EMIT2('N') EMIT2(N_virguilla)
		    EMIT2(0x143) EMIT2(0x145) EMIT2(0x147)
		    EMIT2(0x1f8) EMIT2(0x1e44) EMIT2(0x1e46)
		    EMIT2(0x1e48) EMIT2(0x1e4a) EMIT2(0xa7a4)
		    أرجع OK;

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
		    أرجع OK;

	    حالة 'P': حالة 0x1a4: حالة 0x1e54: حالة 0x1e56: حالة 0x2c63:
		    EMIT2('P') EMIT2(0x1a4) EMIT2(0x1e54) EMIT2(0x1e56)
		    EMIT2(0x2c63)
		    أرجع OK;

	    حالة 'Q': حالة 0x24a:
		    EMIT2('Q') EMIT2(0x24a)
		    أرجع OK;

	    حالة 'R': حالة 0x154: حالة 0x156: حالة 0x158: حالة 0x210:
	    حالة 0x212: حالة 0x24c: حالة 0x1e58: حالة 0x1e5a:
	    حالة 0x1e5c: حالة 0x1e5e: حالة 0x2c64: حالة 0xa7a6:
		    EMIT2('R') EMIT2(0x154) EMIT2(0x156) EMIT2(0x158)
		    EMIT2(0x210) EMIT2(0x212) EMIT2(0x24c) EMIT2(0x1e58)
		    EMIT2(0x1e5a) EMIT2(0x1e5c) EMIT2(0x1e5e) EMIT2(0x2c64)
		    EMIT2(0xa7a6)
		    أرجع OK;

	    حالة 'S': حالة 0x15a: حالة 0x15c: حالة 0x15e: حالة 0x160:
	    حالة 0x218: حالة 0x1e60: حالة 0x1e62: حالة 0x1e64:
	    حالة 0x1e66: حالة 0x1e68: حالة 0x2c7e: حالة 0xa7a8:
		    EMIT2('S') EMIT2(0x15a) EMIT2(0x15c) EMIT2(0x15e)
		    EMIT2(0x160) EMIT2(0x218) EMIT2(0x1e60) EMIT2(0x1e62)
		    EMIT2(0x1e64) EMIT2(0x1e66) EMIT2(0x1e68) EMIT2(0x2c7e)
		    EMIT2(0xa7a8)
		    أرجع OK;

	    حالة 'T': حالة 0x162: حالة 0x164: حالة 0x166: حالة 0x1ac:
	    حالة 0x1ae: حالة 0x21a: حالة 0x23e: حالة 0x1e6a: حالة 0x1e6c:
	    حالة 0x1e6e: حالة 0x1e70:
		    EMIT2('T') EMIT2(0x162) EMIT2(0x164) EMIT2(0x166)
		    EMIT2(0x1ac) EMIT2(0x1ae) EMIT2(0x23e) EMIT2(0x21a)
		    EMIT2(0x1e6a) EMIT2(0x1e6c) EMIT2(0x1e6e) EMIT2(0x1e70)
		    أرجع OK;

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
		    أرجع OK;

	    حالة 'V': حالة 0x1b2: حالة 0x1e7c: حالة 0x1e7e:
		    EMIT2('V') EMIT2(0x1b2) EMIT2(0x1e7c) EMIT2(0x1e7e)
		    أرجع OK;

	    حالة 'W': حالة 0x174: حالة 0x1e80: حالة 0x1e82: حالة 0x1e84:
	    حالة 0x1e86: حالة 0x1e88:
		    EMIT2('W') EMIT2(0x174) EMIT2(0x1e80) EMIT2(0x1e82)
		    EMIT2(0x1e84) EMIT2(0x1e86) EMIT2(0x1e88)
		    أرجع OK;

	    حالة 'X': حالة 0x1e8a: حالة 0x1e8c:
		    EMIT2('X') EMIT2(0x1e8a) EMIT2(0x1e8c)
		    أرجع OK;

	    حالة 'Y': حالة Y_acute: حالة 0x176: حالة 0x178:
	    حالة 0x1b3: حالة 0x232: حالة 0x24e: حالة 0x1e8e:
	    حالة 0x1ef2: حالة 0x1ef4: حالة 0x1ef6: حالة 0x1ef8:
		    EMIT2('Y') EMIT2(Y_acute)
		    EMIT2(0x176) EMIT2(0x178) EMIT2(0x1b3)
		    EMIT2(0x232) EMIT2(0x24e) EMIT2(0x1e8e)
		    EMIT2(0x1ef2) EMIT2(0x1ef4) EMIT2(0x1ef6)
		    EMIT2(0x1ef8)
		    أرجع OK;

	    حالة 'Z': حالة 0x179: حالة 0x17b: حالة 0x17d:
	    حالة 0x1b5: حالة 0x1e90: حالة 0x1e92: حالة 0x1e94:
	    حالة 0x2c6b:
		    EMIT2('Z') EMIT2(0x179) EMIT2(0x17b) EMIT2(0x17d)
		    EMIT2(0x1b5) EMIT2(0x1e90) EMIT2(0x1e92)
		    EMIT2(0x1e94) EMIT2(0x2c6b)
		    أرجع OK;

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
		    أرجع OK;

	    حالة 'b': حالة 0x180: حالة 0x253: حالة 0x1d6c: حالة 0x1d80:
	    حالة 0x1e03: حالة 0x1e05: حالة 0x1e07:
		    EMIT2('b') EMIT2(0x180) EMIT2(0x253) EMIT2(0x1d6c)
		    EMIT2(0x1d80) EMIT2(0x1e03) EMIT2(0x1e05) EMIT2(0x1e07)
		    أرجع OK;

	    حالة 'c': حالة c_cedilla: حالة 0x107: حالة 0x109: حالة 0x10b:
	    حالة 0x10d: حالة 0x188: حالة 0x23c: حالة 0x1e09: حالة 0xa793:
	    حالة 0xa794:
		    EMIT2('c') EMIT2(c_cedilla)
		    EMIT2(0x107) EMIT2(0x109) EMIT2(0x10b)
		    EMIT2(0x10d) EMIT2(0x188) EMIT2(0x23c)
		    EMIT2(0x1e09) EMIT2(0xa793) EMIT2(0xa794)
		    أرجع OK;

	    حالة 'd': حالة 0x10f: حالة 0x111: حالة 0x257: حالة 0x1d6d:
	    حالة 0x1d81: حالة 0x1d91: حالة 0x1e0b: حالة 0x1e0d: حالة 0x1e0f:
	    حالة 0x1e11: حالة 0x1e13:
		    EMIT2('d') EMIT2(0x10f) EMIT2(0x111)
		    EMIT2(0x257) EMIT2(0x1d6d) EMIT2(0x1d81)
		    EMIT2(0x1d91) EMIT2(0x1e0b) EMIT2(0x1e0d)
		    EMIT2(0x1e0f) EMIT2(0x1e11) EMIT2(0x1e13)
		    أرجع OK;

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
		    أرجع OK;

	    حالة 'f': حالة 0x192: حالة 0x1d6e: حالة 0x1d82:
	    حالة 0x1e1f: حالة 0xa799:
		    EMIT2('f') EMIT2(0x192) EMIT2(0x1d6e) EMIT2(0x1d82)
		    EMIT2(0x1e1f) EMIT2(0xa799)
		    أرجع OK;

	    حالة 'g': حالة 0x11d: حالة 0x11f: حالة 0x121: حالة 0x123:
	    حالة 0x1e5: حالة 0x1e7: حالة 0x1f5: حالة 0x260: حالة 0x1d83:
	    حالة 0x1e21: حالة 0xa7a1:
		    EMIT2('g') EMIT2(0x11d) EMIT2(0x11f) EMIT2(0x121)
		    EMIT2(0x123) EMIT2(0x1e5) EMIT2(0x1e7)
		    EMIT2(0x1f5) EMIT2(0x260) EMIT2(0x1d83)
		    EMIT2(0x1e21) EMIT2(0xa7a1)
		    أرجع OK;

	    حالة 'h': حالة 0x125: حالة 0x127: حالة 0x21f: حالة 0x1e23:
	    حالة 0x1e25: حالة 0x1e27: حالة 0x1e29: حالة 0x1e2b:
	    حالة 0x1e96: حالة 0x2c68: حالة 0xa795:
		    EMIT2('h') EMIT2(0x125) EMIT2(0x127) EMIT2(0x21f)
		    EMIT2(0x1e23) EMIT2(0x1e25) EMIT2(0x1e27)
		    EMIT2(0x1e29) EMIT2(0x1e2b) EMIT2(0x1e96)
		    EMIT2(0x2c68) EMIT2(0xa795)
		    أرجع OK;

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
		    أرجع OK;

	    حالة 'j': حالة 0x135: حالة 0x1f0: حالة 0x249:
		    EMIT2('j') EMIT2(0x135) EMIT2(0x1f0) EMIT2(0x249)
		    أرجع OK;

	    حالة 'k': حالة 0x137: حالة 0x199: حالة 0x1e9: حالة 0x1d84:
	    حالة 0x1e31: حالة 0x1e33: حالة 0x1e35: حالة 0x2c6a: حالة 0xa741:
		    EMIT2('k') EMIT2(0x137) EMIT2(0x199) EMIT2(0x1e9)
		    EMIT2(0x1d84) EMIT2(0x1e31) EMIT2(0x1e33)
		    EMIT2(0x1e35) EMIT2(0x2c6a) EMIT2(0xa741)
		    أرجع OK;

	    حالة 'l': حالة 0x13a: حالة 0x13c: حالة 0x13e: حالة 0x140:
	    حالة 0x142: حالة 0x19a: حالة 0x1e37: حالة 0x1e39: حالة 0x1e3b:
	    حالة 0x1e3d: حالة 0x2c61:
		    EMIT2('l') EMIT2(0x13a) EMIT2(0x13c)
		    EMIT2(0x13e) EMIT2(0x140) EMIT2(0x142)
		    EMIT2(0x19a) EMIT2(0x1e37) EMIT2(0x1e39)
		    EMIT2(0x1e3b) EMIT2(0x1e3d) EMIT2(0x2c61)
		    أرجع OK;

	    حالة 'm': حالة 0x1d6f: حالة 0x1e3f: حالة 0x1e41: حالة 0x1e43:
		    EMIT2('m') EMIT2(0x1d6f) EMIT2(0x1e3f)
		    EMIT2(0x1e41) EMIT2(0x1e43)
		    أرجع OK;

	    حالة 'n': حالة n_virguilla: حالة 0x144: حالة 0x146: حالة 0x148:
	    حالة 0x149: حالة 0x1f9: حالة 0x1d70: حالة 0x1d87: حالة 0x1e45:
	    حالة 0x1e47: حالة 0x1e49: حالة 0x1e4b: حالة 0xa7a5:
		    EMIT2('n') EMIT2(n_virguilla)
		    EMIT2(0x144) EMIT2(0x146) EMIT2(0x148)
		    EMIT2(0x149) EMIT2(0x1f9) EMIT2(0x1d70)
		    EMIT2(0x1d87) EMIT2(0x1e45) EMIT2(0x1e47)
		    EMIT2(0x1e49) EMIT2(0x1e4b) EMIT2(0xa7a5)
		    أرجع OK;

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
		    أرجع OK;

	    حالة 'p': حالة 0x1a5: حالة 0x1d71: حالة 0x1d7d: حالة 0x1d88:
	    حالة 0x1e55: حالة 0x1e57:
		    EMIT2('p') EMIT2(0x1a5) EMIT2(0x1d71) EMIT2(0x1d7d)
		    EMIT2(0x1d88) EMIT2(0x1e55) EMIT2(0x1e57)
		    أرجع OK;

	    حالة 'q': حالة 0x24b: حالة 0x2a0:
		    EMIT2('q') EMIT2(0x24b) EMIT2(0x2a0)
		    أرجع OK;

	    حالة 'r': حالة 0x155: حالة 0x157: حالة 0x159: حالة 0x211:
	    حالة 0x213: حالة 0x24d: حالة 0x27d: حالة 0x1d72: حالة 0x1d73:
	    حالة 0x1d89: حالة 0x1e59: حالة 0x1e5b: حالة 0x1e5d: حالة 0x1e5f:
	    حالة 0xa7a7:
		    EMIT2('r') EMIT2(0x155) EMIT2(0x157) EMIT2(0x159)
		    EMIT2(0x211) EMIT2(0x213) EMIT2(0x24d) EMIT2(0x27d)
		    EMIT2(0x1d72) EMIT2(0x1d73) EMIT2(0x1d89) EMIT2(0x1e59)
		    EMIT2(0x1e5b) EMIT2(0x1e5d) EMIT2(0x1e5f) EMIT2(0xa7a7)
		    أرجع OK;

	    حالة 's': حالة 0x15b: حالة 0x15d: حالة 0x15f: حالة 0x161:
	    حالة 0x219: حالة 0x23f: حالة 0x1d74: حالة 0x1d8a: حالة 0x1e61:
	    حالة 0x1e63: حالة 0x1e65: حالة 0x1e67: حالة 0x1e69: حالة 0xa7a9:
		    EMIT2('s') EMIT2(0x15b) EMIT2(0x15d) EMIT2(0x15f)
		    EMIT2(0x161) EMIT2(0x219) EMIT2(0x23f) EMIT2(0x1d74)
		    EMIT2(0x1d8a) EMIT2(0x1e61) EMIT2(0x1e63) EMIT2(0x1e65)
		    EMIT2(0x1e67) EMIT2(0x1e69) EMIT2(0xa7a9)
		    أرجع OK;

	    حالة 't': حالة 0x163: حالة 0x165: حالة 0x167: حالة 0x1ab:
	    حالة 0x1ad: حالة 0x21b: حالة 0x288: حالة 0x1d75: حالة 0x1e6b:
	    حالة 0x1e6d: حالة 0x1e6f: حالة 0x1e71: حالة 0x1e97: حالة 0x2c66:
		    EMIT2('t') EMIT2(0x163) EMIT2(0x165) EMIT2(0x167)
		    EMIT2(0x1ab) EMIT2(0x1ad) EMIT2(0x21b) EMIT2(0x288)
		    EMIT2(0x1d75) EMIT2(0x1e6b) EMIT2(0x1e6d) EMIT2(0x1e6f)
		    EMIT2(0x1e71) EMIT2(0x1e97) EMIT2(0x2c66)
		    أرجع OK;

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
		    أرجع OK;

	    حالة 'v': حالة 0x28b: حالة 0x1d8c: حالة 0x1e7d: حالة 0x1e7f:
		    EMIT2('v') EMIT2(0x28b) EMIT2(0x1d8c) EMIT2(0x1e7d)
		    EMIT2(0x1e7f)
		    أرجع OK;

	    حالة 'w': حالة 0x175: حالة 0x1e81: حالة 0x1e83: حالة 0x1e85:
	    حالة 0x1e87: حالة 0x1e89: حالة 0x1e98:
		    EMIT2('w') EMIT2(0x175) EMIT2(0x1e81) EMIT2(0x1e83)
		    EMIT2(0x1e85) EMIT2(0x1e87) EMIT2(0x1e89) EMIT2(0x1e98)
		    أرجع OK;

	    حالة 'x': حالة 0x1e8b: حالة 0x1e8d:
		    EMIT2('x') EMIT2(0x1e8b) EMIT2(0x1e8d)
		    أرجع OK;

	    حالة 'y': حالة y_acute: حالة y_diaeresis: حالة 0x177:
	    حالة 0x1b4: حالة 0x233: حالة 0x24f: حالة 0x1e8f:
	    حالة 0x1e99: حالة 0x1ef3: حالة 0x1ef5: حالة 0x1ef7:
	    حالة 0x1ef9:
		    EMIT2('y') EMIT2(y_acute) EMIT2(y_diaeresis)
		    EMIT2(0x177) EMIT2(0x1b4) EMIT2(0x233) EMIT2(0x24f)
		    EMIT2(0x1e8f) EMIT2(0x1e99) EMIT2(0x1ef3)
		    EMIT2(0x1ef5) EMIT2(0x1ef7) EMIT2(0x1ef9)
		    أرجع OK;

	    حالة 'z': حالة 0x17a: حالة 0x17c: حالة 0x17e: حالة 0x1b6:
	    حالة 0x1d76: حالة 0x1d8e: حالة 0x1e91: حالة 0x1e93:
	    حالة 0x1e95: حالة 0x2c6c:
		    EMIT2('z') EMIT2(0x17a) EMIT2(0x17c) EMIT2(0x17e)
		    EMIT2(0x1b6) EMIT2(0x1d76) EMIT2(0x1d8e) EMIT2(0x1e91)
		    EMIT2(0x1e93) EMIT2(0x1e95) EMIT2(0x2c6c)
		    أرجع OK;

	    // default: character itself
	}
    }

    EMIT2(c);
    أرجع OK;
#تعریف_رد EMIT2
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
    صحيح		نتیجہ;
    صحيح		startc = -1;
    صحيح		save_prev_at_شروع = prev_at_شروع;

    c = getchr();
    بدل (c)
    {
	حالة NUL:
	    EMSG_RET_FAIL(_(e_nfa_regexp_سرا_encountered_prematurely));

	حالة Magic('^'):
	    EMIT(NFA_BOL);
	    اكسر;

	حالة Magic('$'):
	    EMIT(NFA_EOL);
#إذا defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#جے_انت
	    اكسر;

	حالة Magic('<'):
	    EMIT(NFA_BOW);
	    اكسر;

	حالة Magic('>'):
	    EMIT(NFA_EOW);
	    اكسر;

	حالة Magic('_'):
	    c = no_Magic(getchr());
	    إذا (c == NUL)
		EMSG_RET_FAIL(_(e_nfa_regexp_سرا_encountered_prematurely));

	    إذا (c == '^')	// "\_^" is start-of-line
	    {
		EMIT(NFA_BOL);
		اكسر;
	    }
	    إذا (c == '$')	// "\_$" is end-of-line
	    {
		EMIT(NFA_EOL);
#إذا defined(FEAT_SYN_HL)
		had_eol = TRUE;
#جے_انت
		اكسر;
	    }

	    extra = NFA_جوڑو_NL;

	    // "\_[" is collection plus newline
	    إذا (c == '[')
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
	    إذا (p == عدم)
	    {
		إذا (extra == NFA_جوڑو_NL)
		{
		    semsg(_(e_nfa_regexp_invalid_character_class_nr), c);
		    rc_did_emsg = TRUE;
		    أرجع FAIL;
		}
		siemsg("Unknown character class char: %d", c);
		أرجع FAIL;
	    }

	    // When '.' is followed by a composing char ignore the dot, so that
	    // the composing char is matched here.
	    إذا (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	    {
		old_regparse = regparse;
		c = getchr();
		اذهب nfa_do_multibyte;
	    }
	    EMIT(nfa_classcodes[p - classchars]);
	    إذا (extra == NFA_جوڑو_NL)
	    {
		EMIT(NFA_NEWL);
		EMIT(NFA_OR);
		regflags |= RF_HASNL;
	    }
	    اكسر;

	حالة Magic('n'):
	    إذا (reg_string)
		// In a string "\n" matches a newline character.
		EMIT(NL);
	    وإلا
	    {
		// In buffer text "\n" matches the end of a line.
		EMIT(NFA_NEWL);
		regflags |= RF_HASNL;
	    }
	    اكسر;

	حالة Magic('('):
	    إذا (nfa_reg(REG_PAREN) == FAIL)
		أرجع FAIL;	    // cascaded error
	    اكسر;

	حالة Magic('|'):
	حالة Magic('&'):
	حالة Magic(')'):
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    أرجع FAIL;

	حالة Magic('='):
	حالة Magic('?'):
	حالة Magic('+'):
	حالة Magic('@'):
	حالة Magic('*'):
	حالة Magic('{'):
	    // these should follow an atom, not form an atom
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    أرجع FAIL;

	حالة Magic('~'):
	    {
		char_u	    *lp;

		// Previous substitute pattern.
		// Generated as "\%(pattern\)".
		إذا (reg_prev_sub == عدم)
		{
		    emsg(_(e_no_پچھلا_substitute_regular_عبارت));
		    أرجع FAIL;
		}
		لكل (lp = reg_prev_sub; *lp != NUL; MB_CPTR_ADV(lp))
		{
		    EMIT(PTR2CHAR(lp));
		    إذا (lp != reg_prev_sub)
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

		إذا (!تکیا_endbrace(refnum + 1))
		    أرجع FAIL;
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
		    إذا (re_mult_اگلا("\\zs") == FAIL)
			أرجع FAIL;
		    اكسر;
		حالة 'e':
		    EMIT(NFA_ZEND);
		    rex.nfa_has_zend = TRUE;
		    إذا (re_mult_اگلا("\\ze") == FAIL)
			أرجع FAIL;
		    اكسر;
#جے_تعریف_شدہ FEAT_SYN_HL
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
		    إذا ((reg_do_extmatch & REX_USE) == 0)
			EMSG_RET_FAIL(_(e_z1_z9_not_allowed_ایتھے));
		    EMIT(NFA_ZREF1 + (no_Magic(c) - '1'));
		    // No need to set rex.nfa_has_backref, the sub-matches don't
		    // change when \z1 .. \z9 matches or not.
		    re_has_z = REX_USE;
		    اكسر;
		حالة '(':
		    // \z(
		    إذا ((reg_do_extmatch & REX_سیٹ) == 0)
			EMSG_RET_FAIL(_(e_z_not_allowed_ایتھے));
		    إذا (nfa_reg(REG_ZPAREN) == FAIL)
			أرجع FAIL;	    // cascaded error
		    re_has_z = REX_سیٹ;
		    اكسر;
#جے_انت
		افتراضي:
		    semsg(_(e_nfa_regexp_unknown_operator_z_chr), no_Magic(c));
		    أرجع FAIL;
	    }
	    اكسر;

	حالة Magic('%'):
	    c = no_Magic(getchr());
	    بدل (c)
	    {
		// () without a back reference
		حالة '(':
		    إذا (nfa_reg(REG_NPAREN) == FAIL)
			أرجع FAIL;
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

			إذا (nr < 0 || nr > ودھ_توں_ودھ_عدد)
			    EMSG2_RET_FAIL(_(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			// A NUL is stored in the text as NL
			// TODO: what if a composing character follows?
			EMIT(nr == 0 ? 0x0a : (لما)nr);
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
		    إذا (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_شروع_دا_نمونہ),
								  regparse[1]);
			أرجع FAIL;
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
			لكل (n = 0; (c = peekchr()) != ']'; ++n)
			{
			    إذا (c == NUL)
				EMSG2_RET_FAIL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
			    // recursive call!
			    إذا (nfa_regatom() == FAIL)
				أرجع FAIL;
			}
			getchr();  // get the ]
			إذا (n == 0)
			    EMSG2_RET_FAIL(_(e_خالی_تھاواں_str_brackets),
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

			إذا (c == '<' || c == '>')
			    c = getchr();
			إذا (no_Magic(c) == '.')
			{
			    cur = TRUE;
			    c = getchr();
			}
			طالما (VIM_ISDIGIT(c))
			{
			    long_u tmp;

			    إذا (cur)
			    {
				semsg(_(e_regexp_نمبر_after_dot_pos_تلاش_chr),
								  no_Magic(c));
				أرجع FAIL;
			    }
			    tmp = n * 10 + (c - '0');

			    إذا (tmp < n)
			    {
				// overflow.
				emsg(_(e_percent_قدر_too_large));
				أرجع FAIL;
			    }
			    n = tmp;
			    c = getchr();
			    got_digit = TRUE;
			}
			إذا (c == 'l' || c == 'c' || c == 'v')
			{
			    long_u حد_بندی = ودھ_توں_ودھ_عدد;

			    إذا (!cur && !got_digit)
			    {
				semsg(_(e_nfa_regexp_missing_قدر_وچ_chr),
								  no_Magic(c));
				أرجع FAIL;
			    }
			    إذا (c == 'l')
			    {
				إذا (cur)
				    n = curwin->w_cursor.lnum;
				// \%{n}l  \%{n}<l  \%{n}>l
				EMIT(cmp == '<' ? NFA_LNUM_LT :
				     cmp == '>' ? NFA_LNUM_GT : NFA_LNUM);
				إذا (save_prev_at_شروع)
				    at_شروع = TRUE;
			    }
			    وإلا إذا (c == 'c')
			    {
				إذا (cur)
				{
				    n = curwin->w_cursor.خانہ;
				    n++;
				}
				// \%{n}c  \%{n}<c  \%{n}>c
				EMIT(cmp == '<' ? NFA_خانہ_LT :
				     cmp == '>' ? NFA_خانہ_GT : NFA_خانہ);
			    }
			    وإلا
			    {
				إذا (cur)
				{
				    colnr_T vcol = 0;

				    getvvcol(curwin, &curwin->w_cursor,
							 عدم, عدم, &vcol, 0);
				    n = ++vcol;
				}
				// \%{n}v  \%{n}<v  \%{n}>v
				EMIT(cmp == '<' ? NFA_VCOL_LT :
				     cmp == '>' ? NFA_VCOL_GT : NFA_VCOL);
				حد_بندی = ودھ_توں_ودھ_عدد / MB_MAXBYTES;
			    }
			    إذا (n >= حد_بندی)
			    {
				emsg(_(e_percent_قدر_too_large));
				أرجع FAIL;
			    }
			    EMIT((صحيح)n);
			    اكسر;
			}
			وإلا إذا (no_Magic(c) == '\'' && n == 0)
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
		    أرجع FAIL;
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
	    إذا (*endp == ']')
	    {
		صحيح plen;
		bool range_endpoint;
		/*
		 * Try to reverse engineer character classes. For example,
		 * recognize that [0-9] stands for \d and [A-Za-z_] for \h,
		 * and perform the necessary substitutions in the NFA.
		 */
		نتیجہ = nfa_recognize_char_class(regparse, endp,
							 extra == NFA_جوڑو_NL);
		إذا (نتیجہ != FAIL)
		{
		    إذا (نتیجہ >= NFA_پہلا_NL && نتیجہ <= NFA_آخری_NL)
		    {
			EMIT(نتیجہ - NFA_جوڑو_NL);
			EMIT(NFA_NEWL);
			EMIT(NFA_OR);
		    }
		    وإلا
			EMIT(نتیجہ);
		    regparse = endp;
		    MB_PTR_ADV(regparse);
		    أرجع OK;
		}
		/*
		 * Failed to recognize a character class. Use the simple
		 * version that turns [abc] into 'a' OR 'b' OR 'c'
		 */
		startc = -1;
		negated = FALSE;
		إذا (*regparse == '^')			// negated range
		{
		    negated = TRUE;
		    MB_PTR_ADV(regparse);
		    EMIT(NFA_شروع_NEG_COLL);
		}
		وإلا
		    EMIT(NFA_شروع_COLL);
		إذا (*regparse == '-')
		{
		    startc = '-';
		    EMIT(startc);
		    EMIT(NFA_CONCAT);
		    MB_PTR_ADV(regparse);
		}
		// Emit the OR branches for each character in the []
		emit_range = FALSE;
		طالما (regparse < endp)
		{
		    صحيح	    oldstartc = startc;
		    range_endpoint = false;

		    startc = -1;
		    got_coll_char = FALSE;
		    إذا (*regparse == '[')
		    {
			// Check for [: :], [= =], [. .]
			equiclass = collclass = 0;
			charclass = get_char_class(&regparse);
			إذا (charclass == CLASS_NONE)
			{
			    equiclass = get_equi_class(&regparse);
			    إذا (equiclass == 0)
				collclass = get_coll_element(&regparse);
			}

			// Character class like [:alpha:]
			إذا (charclass != CLASS_NONE)
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
				حالة CLASS_گراف:
				    EMIT(NFA_CLASS_گراف);
				    اكسر;
				حالة CLASS_ہیٹھلا:
				    چاہندا_nfa = TRUE;
				    EMIT(NFA_CLASS_ہیٹھلا);
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
				حالة CLASS_اتلا:
				    چاہندا_nfa = TRUE;
				    EMIT(NFA_CLASS_اتلا);
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
			إذا (equiclass != 0)
			{
			    نتیجہ = nfa_emit_equi_class(equiclass);
			    إذا (نتیجہ == FAIL)
			    {
				// should never happen
				EMSG_RET_FAIL(_(e_error_building_nfa_with_equivalence_class));
			    }
			    استمر;
			}
			// Try collating class like [. .]
			إذا (collclass != 0)
			{
			    startc = collclass;	 // allow [.a.]-x as a range
			    // Will emit the proper atom at the end of the
			    // while loop.
			}
		    }
		    // Try a range like 'a-x' or '\t-z'. Also allows '-' as a
		    // start character.
		    إذا (*regparse == '-' && oldstartc != -1)
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
		    إذا (*regparse == '\\'
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

			إذا (*regparse == 'n')
			    startc = (reg_string || emit_range
					|| regparse[1] == '-') ? NL : NFA_NEWL;
			وإلا إذا (*regparse == 'd'
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
				إذا (startc == ودھ_توں_ودھ_عدد)
				    EMSG_RET_FAIL(_(e_unicode_val_too_large));
				got_coll_char = TRUE;
				MB_PTR_پٹھ(old_regparse, regparse);
			    }
			    وإلا
			    {
				// \r,\t,\e,\b
				startc = backslash_trans(*regparse);
			    }
		    }

		    // Normal printable char
		    إذا (startc == -1)
			startc = PTR2CHAR(regparse);

		    // Previous char was '-', so this char is end of range.
		    إذا (emit_range)
		    {
			صحيح	endc = startc;
			range_endpoint = true;

			startc = oldstartc;
			إذا (startc > endc)
			    EMSG_RET_FAIL(_(e_الٹ_range_وچ_character_class));

			إذا (endc > startc + 2)
			{
			    // Emit a range instead of the sequence of
			    // individual characters.
			    إذا (startc == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    وإلا
				--post_ptr; // remove NFA_CONCAT
			    EMIT(endc);
			    EMIT(NFA_RANGE);
			    EMIT(NFA_CONCAT);
			}
			وإلا إذا (has_mbyte && ((*mb_char2len)(startc) > 1
				    || (*mb_char2len)(endc) > 1))
			{
			    // Emit the characters in the range.
			    // "startc" was already emitted, so skip it.
			    //
			    لكل (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			وإلا
			{
			    // Emit the range. "startc" was already emitted, so
			    // skip it.
			    لكل (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			emit_range = FALSE;
			startc = -1;
		    }
		    وإلا
		    {
			// This char (startc) is not part of a range. Just
			// emit it.
			// Normally, simply emit startc. But if we get char
			// code=0 from a collating char, then replace it with
			// 0x0a.
			// This is needed to completely mimic the behaviour of
			// the backtracking engine.
			إذا (startc == NFA_NEWL)
			{
			    // Line break can't be matched as part of the
			    // collection, add an OR below. But not for negated
			    // range.
			    إذا (!negated)
				extra = NFA_جوڑو_NL;
			}
			وإلا
			{
			    إذا (got_coll_char == TRUE && startc == 0)
			    {
				EMIT(0x0a);
				EMIT(NFA_CONCAT);
			    }
			    وإلا
			    {
				EMIT(startc);
				إذا (!(enc_utf8 && (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse)))))
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
		    إذا (!range_endpoint && enc_utf8 &&
			    (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse))))
		    {
			صحيح i = utf_ptr2len(regparse);

			c = utf_ptr2char(regparse + i);

			// Add composing characters
			لكل (;;)
			{
			    إذا (c == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    وإلا
				EMIT(c);
			    EMIT(NFA_CONCAT);
			    إذا ((i += utf_char2len(c)) >= plen)
				اكسر;
			    c = utf_ptr2char(regparse + i);
			}
			EMIT(NFA_COMPOSING);
			EMIT(NFA_CONCAT);
		    }
		    MB_PTR_ADV(regparse);
		} // while (p < endp)

		MB_PTR_پٹھ(old_regparse, regparse);
		إذا (*regparse == '-')	    // if last, '-' is just a char
		{
		    EMIT('-');
		    EMIT(NFA_CONCAT);
		}

		// skip the trailing ]
		regparse = endp;
		MB_PTR_ADV(regparse);

		// Mark end of the collection.
		إذا (negated == TRUE)
		    EMIT(NFA_سرا_NEG_COLL);
		وإلا
		    EMIT(NFA_سرا_COLL);

		// \_[] also matches \n but it's not negated
		إذا (extra == NFA_جوڑو_NL)
		{
		    EMIT(reg_string ? NL : NFA_NEWL);
		    EMIT(NFA_OR);
		}

		أرجع OK;
	    } // if exists closing ]

	    إذا (reg_strict)
		EMSG_RET_FAIL(_(e_missing_rsb_after_str_lsb));
	    // FALLTHROUGH

	افتراضي:
	    {
		صحيح	plen;

nfa_do_multibyte:
		// plen is length of current char with composing chars
		إذا (enc_utf8 && ((*mb_char2len)(c)
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
		    لكل (;;)
		    {
			EMIT(c);
			إذا (i > 0)
			    EMIT(NFA_CONCAT);
			إذا ((i += utf_char2len(c)) >= plen)
			    اكسر;
			c = utf_ptr2char(old_regparse + i);
		    }
		    EMIT(NFA_COMPOSING);
		    regparse = old_regparse + plen;
		}
		وإلا
		{
		    c = no_Magic(c);
		    EMIT(c);
		}
		أرجع OK;
	    }
    }

    أرجع OK;
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
    لما	minval, maxval;
    صحيح		greedy = TRUE;      // Braces are prefixed with '-' ?
    parse_state_T old_state;
    parse_state_T new_state;
    لما	c2;
    صحيح		old_post_pos;
    صحيح		my_post_شروع;
    صحيح		quest;

    // Save the current parse state, so that we can use it if <atom>{m,n} is
    // next.
    save_parse_state(&old_state);

    // store current pos in the postfix form, for \{m,n} involving 0s
    my_post_شروع = (صحيح)(post_ptr - post_شروع);

    ret = nfa_regatom();
    إذا (ret == FAIL)
	أرجع FAIL;	    // cascaded error

    op = peekchr();
    إذا (re_multi_type(op) == NOT_MULTI)
	أرجع OK;

    skipchr();
    بدل (op)
    {
	حالة Magic('*'):
	    EMIT(NFA_تارا);
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
	    إذا (nfa_regatom() == FAIL)
		أرجع FAIL;
	    EMIT(NFA_تارا);
	    EMIT(NFA_CONCAT);
	    skipchr();		// skip the \+
	    اكسر;

	حالة Magic('@'):
	    c2 = (لما)getdecchrs();
	    op = no_Magic(getchr());
	    i = 0;
	    بدل(op)
	    {
		حالة '=':
		    // \@=
		    i = NFA_PREV_ATOM_NO_چوڑائی;
		    اكسر;
		حالة '!':
		    // \@!
		    i = NFA_PREV_ATOM_NO_چوڑائی_NEG;
		    اكسر;
		حالة '<':
		    op = no_Magic(getchr());
		    إذا (op == '=')
			// \@<=
			i = NFA_PREV_ATOM_JUST_BEFORE;
		    وإلا إذا (op == '!')
			// \@<!
			i = NFA_PREV_ATOM_JUST_BEFORE_NEG;
		    اكسر;
		حالة '>':
		    // \@>
		    i = NFA_PREV_ATOM_LIKE_نمونہ;
		    اكسر;
	    }
	    إذا (i == 0)
	    {
		semsg(_(e_nfa_regexp_unknown_operator_at_chr), op);
		أرجع FAIL;
	    }
	    EMIT(i);
	    إذا (i == NFA_PREV_ATOM_JUST_BEFORE
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
	    إذا (c2 == '-' || c2 == Magic('-'))
	    {
		skipchr();
		greedy = FALSE;
	    }
	    إذا (!read_limits(&minval, &maxval))
		EMSG_RET_FAIL(_(e_nfa_regexp_error_reading_repetition_limits));

	    //  <atom>{0,inf}, <atom>{0,} and <atom>{}  are equivalent to
	    //  <atom>*
	    إذا (minval == 0 && maxval == MAX_حد_بندی)
	    {
		إذا (greedy)		// { { (match the braces)
		    // \{}, \{0,}
		    EMIT(NFA_تارا);
		وإلا			// { { (match the braces)
		    // \{-}, \{-0,}
		    EMIT(NFA_تارا_NONGREEDY);
		اكسر;
	    }

	    // Special case: x{0} or x{-0}
	    إذا (maxval == 0)
	    {
		// Ignore result of previous call to nfa_regatom()
		post_ptr = post_شروع + my_post_شروع;
		// NFA_EMPTY is 0-length and works everywhere
		EMIT(NFA_خالی_تھاواں);
		أرجع OK;
	    }

	    // The engine is very inefficient (uses too many states) when the
	    // maximum is much larger than the minimum and when the maximum is
	    // large.  However, when maxval is MAX_LIMIT, it is okay, as this
	    // will emit NFA_STAR.
	    // Bail out if we can use the other engine, but only, when the
	    // pattern does not need the NFA engine like (e.g. [[:upper:]]\{2,\}
	    // does not work with characters > 8 bit with the BT engine)
	    إذا ((nfa_re_flags & RE_AUTO)
				   && (maxval > 500 || maxval > minval + 200)
				   && (maxval != MAX_حد_بندی && minval < 200)
				   && !چاہندا_nfa)
		أرجع FAIL;

	    // Ignore previous call to nfa_regatom()
	    post_ptr = post_شروع + my_post_شروع;
	    // Save parse state after the repeated atom and the \{}
	    save_parse_state(&new_state);

	    quest = (greedy == TRUE? NFA_QUEST : NFA_QUEST_NONGREEDY);
	    لكل (i = 0; i < maxval; i++)
	    {
		// Goto beginning of the repeated atom
		restore_parse_state(&old_state);
		old_post_pos = (صحيح)(post_ptr - post_شروع);
		إذا (nfa_regatom() == FAIL)
		    أرجع FAIL;
		// after "minval" times, atoms are optional
		إذا (i + 1 > minval)
		{
		    إذا (maxval == MAX_حد_بندی)
		    {
			إذا (greedy)
			    EMIT(NFA_تارا);
			وإلا
			    EMIT(NFA_تارا_NONGREEDY);
		    }
		    وإلا
			EMIT(quest);
		}
		إذا (old_post_pos != my_post_شروع)
		    EMIT(NFA_CONCAT);
		إذا (i + 1 > minval && maxval == MAX_حد_بندی)
		    اكسر;
	    }

	    // Go to just after the repeated atom and the \{}
	    restore_parse_state(&new_state);
	    curchr = -1;

	    اكسر;


	افتراضي:
	    اكسر;
    }	// end switch

    إذا (re_multi_type(peekchr()) != NOT_MULTI)
	// Can't have a multi follow a multi.
	EMSG_RET_FAIL(_(e_nfa_regexp_cant_have_multi_follow_multi));

    أرجع OK;
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
    صحيح		پہلا = TRUE;

    طالما (cont)
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
		إذا (nfa_regpiece() == FAIL)
		    أرجع FAIL;
		إذا (پہلا == FALSE)
		    EMIT(NFA_CONCAT);
		وإلا
		    پہلا = FALSE;
		اكسر;
	}
    }

    أرجع OK;
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

    old_post_pos = (صحيح)(post_ptr - post_شروع);

    // First branch, possibly the only one
    إذا (nfa_regconcat() == FAIL)
	أرجع FAIL;

    // Try next concats
    طالما (peekchr() == Magic('&'))
    {
	skipchr();
	// if concat is empty do emit a node
	إذا (old_post_pos == (صحيح)(post_ptr - post_شروع))
	    EMIT(NFA_خالی_تھاواں);
	EMIT(NFA_NOPEN);
	EMIT(NFA_PREV_ATOM_NO_چوڑائی);
	old_post_pos = (صحيح)(post_ptr - post_شروع);
	إذا (nfa_regconcat() == FAIL)
	    أرجع FAIL;
	// if concat is empty do emit a node
	إذا (old_post_pos == (صحيح)(post_ptr - post_شروع))
	    EMIT(NFA_خالی_تھاواں);
	EMIT(NFA_CONCAT);
    }

    // if a branch is empty, emit one node for it
    إذا (old_post_pos == (صحيح)(post_ptr - post_شروع))
	EMIT(NFA_خالی_تھاواں);

    أرجع OK;
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
    صحيح		حالت = FAIL;

    إذا (paren == REG_PAREN)
    {
	إذا (regnpar >= NSUBEXP) // Too many `('
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_parens));
	parno = regnpar++;
    }
#جے_تعریف_شدہ FEAT_SYN_HL
    وإلا إذا (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	إذا (regnzpar >= NSUBEXP)
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_z));
	parno = regnzpar++;
    }
#جے_انت

    إذا (nfa_reg_parse_ڈونگھائی >= REG_MAX_PAREN_ڈونگھائی)
	EMSG_RET_FAIL(_(e_command_too_complex));
    ++nfa_reg_parse_ڈونگھائی;

    إذا (nfa_regbranch() == FAIL)
	اذهب theend;	    // cascaded error

    طالما (peekchr() == Magic('|'))
    {
	skipchr();
	إذا (nfa_regbranch() == FAIL)
	    اذهب theend;    // cascaded error
	EMIT(NFA_OR);
    }

    // Check for proper termination.
    إذا (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
	إذا (paren == REG_NPAREN)
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	وإلا
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	rc_did_emsg = TRUE;
	اذهب theend;
    }
    وإلا إذا (paren == REG_NOPAREN && peekchr() != NUL)
    {
	إذا (peekchr() == Magic(')'))
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	وإلا
	    emsg(_(e_nfa_regexp_proper_termination_error));
	rc_did_emsg = TRUE;
	اذهب theend;
    }
    /*
     * Here we set the flag allowing back references to this set of
     * parentheses.
     */
    إذا (paren == REG_PAREN)
    {
	had_endbrace[parno] = TRUE;     // have seen the close paren
	EMIT(NFA_MOPEN + parno);
    }
#جے_تعریف_شدہ FEAT_SYN_HL
    وإلا إذا (paren == REG_ZPAREN)
	EMIT(NFA_ZOPEN + parno);
#جے_انت

    حالت = OK;

theend:
    --nfa_reg_parse_ڈونگھائی;
    أرجع حالت;
}

#جے_تعریف_شدہ DEBUG
ساكن char_u کوڈ[50];

    ساكن فراغ
nfa_سیٹ_کوڈ(صحيح c)
{
    صحيح	    addnl = FALSE;

    إذا (c >= NFA_پہلا_NL && c <= NFA_آخری_NL)
    {
	addnl = TRUE;
	c -= NFA_جوڑو_NL;
    }

    STRCPY(کوڈ, "");
    بدل (c)
    {
	حالة NFA_میل:	    STRCPY(کوڈ, "NFA_MATCH "); اكسر;
	حالة NFA_ونڈو:	    STRCPY(کوڈ, "NFA_SPLIT "); اكسر;
	حالة NFA_CONCAT:    STRCPY(کوڈ, "NFA_CONCAT "); اكسر;
	حالة NFA_NEWL:	    STRCPY(کوڈ, "NFA_NEWL "); اكسر;
	حالة NFA_ZSTART:    STRCPY(کوڈ, "NFA_ZSTART"); اكسر;
	حالة NFA_ZEND:	    STRCPY(کوڈ, "NFA_ZEND"); اكسر;

	حالة NFA_BACKREF1:  STRCPY(کوڈ, "NFA_BACKREF1"); اكسر;
	حالة NFA_BACKREF2:  STRCPY(کوڈ, "NFA_BACKREF2"); اكسر;
	حالة NFA_BACKREF3:  STRCPY(کوڈ, "NFA_BACKREF3"); اكسر;
	حالة NFA_BACKREF4:  STRCPY(کوڈ, "NFA_BACKREF4"); اكسر;
	حالة NFA_BACKREF5:  STRCPY(کوڈ, "NFA_BACKREF5"); اكسر;
	حالة NFA_BACKREF6:  STRCPY(کوڈ, "NFA_BACKREF6"); اكسر;
	حالة NFA_BACKREF7:  STRCPY(کوڈ, "NFA_BACKREF7"); اكسر;
	حالة NFA_BACKREF8:  STRCPY(کوڈ, "NFA_BACKREF8"); اكسر;
	حالة NFA_BACKREF9:  STRCPY(کوڈ, "NFA_BACKREF9"); اكسر;
# ifdef FEAT_SYN_HL
	حالة NFA_ZREF1:	    STRCPY(کوڈ, "NFA_ZREF1"); اكسر;
	حالة NFA_ZREF2:	    STRCPY(کوڈ, "NFA_ZREF2"); اكسر;
	حالة NFA_ZREF3:	    STRCPY(کوڈ, "NFA_ZREF3"); اكسر;
	حالة NFA_ZREF4:	    STRCPY(کوڈ, "NFA_ZREF4"); اكسر;
	حالة NFA_ZREF5:	    STRCPY(کوڈ, "NFA_ZREF5"); اكسر;
	حالة NFA_ZREF6:	    STRCPY(کوڈ, "NFA_ZREF6"); اكسر;
	حالة NFA_ZREF7:	    STRCPY(کوڈ, "NFA_ZREF7"); اكسر;
	حالة NFA_ZREF8:	    STRCPY(کوڈ, "NFA_ZREF8"); اكسر;
	حالة NFA_ZREF9:	    STRCPY(کوڈ, "NFA_ZREF9"); اكسر;
# endif
	حالة NFA_SKIP:	    STRCPY(کوڈ, "NFA_SKIP"); اكسر;

	حالة NFA_PREV_ATOM_NO_چوڑائی:
			    STRCPY(کوڈ, "NFA_PREV_ATOM_NO_WIDTH"); اكسر;
	حالة NFA_PREV_ATOM_NO_چوڑائی_NEG:
			    STRCPY(کوڈ, "NFA_PREV_ATOM_NO_WIDTH_NEG"); اكسر;
	حالة NFA_PREV_ATOM_JUST_BEFORE:
			    STRCPY(کوڈ, "NFA_PREV_ATOM_JUST_BEFORE"); اكسر;
	حالة NFA_PREV_ATOM_JUST_BEFORE_NEG:
			 STRCPY(کوڈ, "NFA_PREV_ATOM_JUST_BEFORE_NEG"); اكسر;
	حالة NFA_PREV_ATOM_LIKE_نمونہ:
			    STRCPY(کوڈ, "NFA_PREV_ATOM_LIKE_PATTERN"); اكسر;

	حالة NFA_NOPEN:		    STRCPY(کوڈ, "NFA_NOPEN"); اكسر;
	حالة NFA_NCLOSE:	    STRCPY(کوڈ, "NFA_NCLOSE"); اكسر;
	حالة NFA_شروع_INVISIBLE:   STRCPY(کوڈ, "NFA_START_INVISIBLE"); اكسر;
	حالة NFA_شروع_INVISIBLE_پہلا:
			     STRCPY(کوڈ, "NFA_START_INVISIBLE_FIRST"); اكسر;
	حالة NFA_شروع_INVISIBLE_NEG:
			       STRCPY(کوڈ, "NFA_START_INVISIBLE_NEG"); اكسر;
	حالة NFA_شروع_INVISIBLE_NEG_پہلا:
			 STRCPY(کوڈ, "NFA_START_INVISIBLE_NEG_FIRST"); اكسر;
	حالة NFA_شروع_INVISIBLE_BEFORE:
			    STRCPY(کوڈ, "NFA_START_INVISIBLE_BEFORE"); اكسر;
	حالة NFA_شروع_INVISIBLE_BEFORE_پہلا:
		      STRCPY(کوڈ, "NFA_START_INVISIBLE_BEFORE_FIRST"); اكسر;
	حالة NFA_شروع_INVISIBLE_BEFORE_NEG:
			STRCPY(کوڈ, "NFA_START_INVISIBLE_BEFORE_NEG"); اكسر;
	حالة NFA_شروع_INVISIBLE_BEFORE_NEG_پہلا:
		  STRCPY(کوڈ, "NFA_START_INVISIBLE_BEFORE_NEG_FIRST"); اكسر;
	حالة NFA_شروع_نمونہ:   STRCPY(کوڈ, "NFA_START_PATTERN"); اكسر;
	حالة NFA_سرا_INVISIBLE:	    STRCPY(کوڈ, "NFA_END_INVISIBLE"); اكسر;
	حالة NFA_سرا_INVISIBLE_NEG: STRCPY(کوڈ, "NFA_END_INVISIBLE_NEG"); اكسر;
	حالة NFA_سرا_نمونہ:	    STRCPY(کوڈ, "NFA_END_PATTERN"); اكسر;

	حالة NFA_COMPOSING:	    STRCPY(کوڈ, "NFA_COMPOSING"); اكسر;
	حالة NFA_سرا_COMPOSING:	    STRCPY(کوڈ, "NFA_END_COMPOSING"); اكسر;
	حالة NFA_OPT_CHARS:	    STRCPY(کوڈ, "NFA_OPT_CHARS"); اكسر;

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
	    STRCPY(کوڈ, "NFA_MOPEN(x)");
	    کوڈ[10] = c - NFA_MOPEN + '0';
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
	    STRCPY(کوڈ, "NFA_MCLOSE(x)");
	    کوڈ[11] = c - NFA_MCLOSE + '0';
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
	    STRCPY(کوڈ, "NFA_ZOPEN(x)");
	    کوڈ[10] = c - NFA_ZOPEN + '0';
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
	    STRCPY(کوڈ, "NFA_ZCLOSE(x)");
	    کوڈ[11] = c - NFA_ZCLOSE + '0';
	    اكسر;
# endif
	حالة NFA_EOL:		STRCPY(کوڈ, "NFA_EOL "); اكسر;
	حالة NFA_BOL:		STRCPY(کوڈ, "NFA_BOL "); اكسر;
	حالة NFA_EOW:		STRCPY(کوڈ, "NFA_EOW "); اكسر;
	حالة NFA_BOW:		STRCPY(کوڈ, "NFA_BOW "); اكسر;
	حالة NFA_EOF:		STRCPY(کوڈ, "NFA_EOF "); اكسر;
	حالة NFA_BOF:		STRCPY(کوڈ, "NFA_BOF "); اكسر;
	حالة NFA_LNUM:		STRCPY(کوڈ, "NFA_LNUM "); اكسر;
	حالة NFA_LNUM_GT:	STRCPY(کوڈ, "NFA_LNUM_GT "); اكسر;
	حالة NFA_LNUM_LT:	STRCPY(کوڈ, "NFA_LNUM_LT "); اكسر;
	حالة NFA_خانہ:		STRCPY(کوڈ, "NFA_COL "); اكسر;
	حالة NFA_خانہ_GT:	STRCPY(کوڈ, "NFA_COL_GT "); اكسر;
	حالة NFA_خانہ_LT:	STRCPY(کوڈ, "NFA_COL_LT "); اكسر;
	حالة NFA_VCOL:		STRCPY(کوڈ, "NFA_VCOL "); اكسر;
	حالة NFA_VCOL_GT:	STRCPY(کوڈ, "NFA_VCOL_GT "); اكسر;
	حالة NFA_VCOL_LT:	STRCPY(کوڈ, "NFA_VCOL_LT "); اكسر;
	حالة NFA_MARK:		STRCPY(کوڈ, "NFA_MARK "); اكسر;
	حالة NFA_MARK_GT:	STRCPY(کوڈ, "NFA_MARK_GT "); اكسر;
	حالة NFA_MARK_LT:	STRCPY(کوڈ, "NFA_MARK_LT "); اكسر;
	حالة NFA_CURSOR:	STRCPY(کوڈ, "NFA_CURSOR "); اكسر;
	حالة NFA_VISUAL:	STRCPY(کوڈ, "NFA_VISUAL "); اكسر;
	حالة NFA_ANY_COMPOSING:	STRCPY(کوڈ, "NFA_ANY_COMPOSING "); اكسر;

	حالة NFA_تارا:		STRCPY(کوڈ, "NFA_STAR "); اكسر;
	حالة NFA_تارا_NONGREEDY: STRCPY(کوڈ, "NFA_STAR_NONGREEDY "); اكسر;
	حالة NFA_QUEST:		STRCPY(کوڈ, "NFA_QUEST"); اكسر;
	حالة NFA_QUEST_NONGREEDY: STRCPY(کوڈ, "NFA_QUEST_NON_GREEDY"); اكسر;
	حالة NFA_خالی_تھاواں:		STRCPY(کوڈ, "NFA_EMPTY"); اكسر;
	حالة NFA_OR:		STRCPY(کوڈ, "NFA_OR"); اكسر;

	حالة NFA_شروع_COLL:	STRCPY(کوڈ, "NFA_START_COLL"); اكسر;
	حالة NFA_سرا_COLL:	STRCPY(کوڈ, "NFA_END_COLL"); اكسر;
	حالة NFA_شروع_NEG_COLL: STRCPY(کوڈ, "NFA_START_NEG_COLL"); اكسر;
	حالة NFA_سرا_NEG_COLL:	STRCPY(کوڈ, "NFA_END_NEG_COLL"); اكسر;
	حالة NFA_RANGE:		STRCPY(کوڈ, "NFA_RANGE"); اكسر;
	حالة NFA_RANGE_MIN:	STRCPY(کوڈ, "NFA_RANGE_MIN"); اكسر;
	حالة NFA_RANGE_MAX:	STRCPY(کوڈ, "NFA_RANGE_MAX"); اكسر;

	حالة NFA_CLASS_ALNUM:	STRCPY(کوڈ, "NFA_CLASS_ALNUM"); اكسر;
	حالة NFA_CLASS_ALPHA:	STRCPY(کوڈ, "NFA_CLASS_ALPHA"); اكسر;
	حالة NFA_CLASS_BLANK:	STRCPY(کوڈ, "NFA_CLASS_BLANK"); اكسر;
	حالة NFA_CLASS_CNTRL:	STRCPY(کوڈ, "NFA_CLASS_CNTRL"); اكسر;
	حالة NFA_CLASS_DIGIT:	STRCPY(کوڈ, "NFA_CLASS_DIGIT"); اكسر;
	حالة NFA_CLASS_گراف:	STRCPY(کوڈ, "NFA_CLASS_GRAPH"); اكسر;
	حالة NFA_CLASS_ہیٹھلا:	STRCPY(کوڈ, "NFA_CLASS_LOWER"); اكسر;
	حالة NFA_CLASS_PRINT:	STRCPY(کوڈ, "NFA_CLASS_PRINT"); اكسر;
	حالة NFA_CLASS_PUNCT:	STRCPY(کوڈ, "NFA_CLASS_PUNCT"); اكسر;
	حالة NFA_CLASS_SPACE:	STRCPY(کوڈ, "NFA_CLASS_SPACE"); اكسر;
	حالة NFA_CLASS_اتلا:	STRCPY(کوڈ, "NFA_CLASS_UPPER"); اكسر;
	حالة NFA_CLASS_XDIGIT:	STRCPY(کوڈ, "NFA_CLASS_XDIGIT"); اكسر;
	حالة NFA_CLASS_TAB:	STRCPY(کوڈ, "NFA_CLASS_TAB"); اكسر;
	حالة NFA_CLASS_RETURN:	STRCPY(کوڈ, "NFA_CLASS_RETURN"); اكسر;
	حالة NFA_CLASS_BACKSPACE:   STRCPY(کوڈ, "NFA_CLASS_BACKSPACE"); اكسر;
	حالة NFA_CLASS_ESCAPE:	STRCPY(کوڈ, "NFA_CLASS_ESCAPE"); اكسر;
	حالة NFA_CLASS_IDENT:	STRCPY(کوڈ, "NFA_CLASS_IDENT"); اكسر;
	حالة NFA_CLASS_KEYWORD:	STRCPY(کوڈ, "NFA_CLASS_KEYWORD"); اكسر;
	حالة NFA_CLASS_FNAME:	STRCPY(کوڈ, "NFA_CLASS_FNAME"); اكسر;

	حالة NFA_ANY:	STRCPY(کوڈ, "NFA_ANY"); اكسر;
	حالة NFA_IDENT:	STRCPY(کوڈ, "NFA_IDENT"); اكسر;
	حالة NFA_SIDENT:STRCPY(کوڈ, "NFA_SIDENT"); اكسر;
	حالة NFA_KWORD:	STRCPY(کوڈ, "NFA_KWORD"); اكسر;
	حالة NFA_SKWORD:STRCPY(کوڈ, "NFA_SKWORD"); اكسر;
	حالة NFA_FNAME:	STRCPY(کوڈ, "NFA_FNAME"); اكسر;
	حالة NFA_SFNAME:STRCPY(کوڈ, "NFA_SFNAME"); اكسر;
	حالة NFA_PRINT:	STRCPY(کوڈ, "NFA_PRINT"); اكسر;
	حالة NFA_SPRINT:STRCPY(کوڈ, "NFA_SPRINT"); اكسر;
	حالة NFA_WHITE:	STRCPY(کوڈ, "NFA_WHITE"); اكسر;
	حالة NFA_NWHITE:STRCPY(کوڈ, "NFA_NWHITE"); اكسر;
	حالة NFA_DIGIT:	STRCPY(کوڈ, "NFA_DIGIT"); اكسر;
	حالة NFA_NDIGIT:STRCPY(کوڈ, "NFA_NDIGIT"); اكسر;
	حالة NFA_HEX:	STRCPY(کوڈ, "NFA_HEX"); اكسر;
	حالة NFA_NHEX:	STRCPY(کوڈ, "NFA_NHEX"); اكسر;
	حالة NFA_OCTAL:	STRCPY(کوڈ, "NFA_OCTAL"); اكسر;
	حالة NFA_NOCTAL:STRCPY(کوڈ, "NFA_NOCTAL"); اكسر;
	حالة NFA_لفظ:	STRCPY(کوڈ, "NFA_WORD"); اكسر;
	حالة NFA_NWORD:	STRCPY(کوڈ, "NFA_NWORD"); اكسر;
	حالة NFA_سر:	STRCPY(کوڈ, "NFA_HEAD"); اكسر;
	حالة NFA_NHEAD:	STRCPY(کوڈ, "NFA_NHEAD"); اكسر;
	حالة NFA_ALPHA:	STRCPY(کوڈ, "NFA_ALPHA"); اكسر;
	حالة NFA_NALPHA:STRCPY(کوڈ, "NFA_NALPHA"); اكسر;
	حالة NFA_ہیٹھلا:	STRCPY(کوڈ, "NFA_LOWER"); اكسر;
	حالة NFA_NLOWER:STRCPY(کوڈ, "NFA_NLOWER"); اكسر;
	حالة NFA_اتلا:	STRCPY(کوڈ, "NFA_UPPER"); اكسر;
	حالة NFA_NUPPER:STRCPY(کوڈ, "NFA_NUPPER"); اكسر;
	حالة NFA_ہیٹھلا_IC:  STRCPY(کوڈ, "NFA_LOWER_IC"); اكسر;
	حالة NFA_NLOWER_IC: STRCPY(کوڈ, "NFA_NLOWER_IC"); اكسر;
	حالة NFA_اتلا_IC:  STRCPY(کوڈ, "NFA_UPPER_IC"); اكسر;
	حالة NFA_NUPPER_IC: STRCPY(کوڈ, "NFA_NUPPER_IC"); اكسر;

	افتراضي:
	    STRCPY(کوڈ, "CHAR(x)");
	    کوڈ[5] = c;
    }

    إذا (addnl == TRUE)
	STRCAT(کوڈ, " + NEWLINE ");

}

# ifdef ENABLE_LOG
ساكن FILE *log_fd;
ساكن char_u e_log_open_failed[] = N_("Could not open temporary log file for writing, displaying on stderr... ");

/*
 * Print the postfix notation of the current regexp.
 */
    ساكن فراغ
nfa_postfix_dump(char_u *اظہاریہ, صحيح retval)
{
    صحيح *p;
    FILE *f;

    f = fopen(NFA_REGEXP_DUMP_LOG, "a");
    إذا (f == عدم)
	أرجع;

    fprintf(f, "\n-------------------------\n");
    إذا (retval == FAIL)
	fprintf(f, ">>> NFA engine failed... \n");
    وإلا إذا (retval == OK)
	fprintf(f, ">>> NFA engine succeeded !\n");
    fprintf(f, "Regexp: \"%s\"\nPostfix notation (char): \"", اظہاریہ);
    لكل (p = post_شروع; *p && p < post_ptr; p++)
    {
	nfa_سیٹ_کوڈ(*p);
	fprintf(f, "%s, ", کوڈ);
    }
    fprintf(f, "\"\nPostfix notation (int): ");
    لكل (p = post_شروع; *p && p < post_ptr; p++)
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

    إذا (state == عدم)
	أرجع;

    fprintf(debugf, "(%2d)", مطلق_قدر(state->id));

    // Output indent
    p = (char_u *)indent->ga_انکڑے;
    إذا (indent->ga_len >= 3)
    {
	صحيح	آخری = indent->ga_len - 3;
	char_u	save[2];

	STRNCPY(save, &p[آخری], 2);
	حافظہ_نقل(&p[آخری], "+-", 2);
	fprintf(debugf, " %s", p);
	STRNCPY(&p[آخری], save, 2);
    }
    وإلا
	fprintf(debugf, " %s", p);

    nfa_سیٹ_کوڈ(state->c);
    fprintf(debugf, "%s (%d) (id=%d) val=%d\n",
		 کوڈ,
		 state->c,
		 مطلق_قدر(state->id),
		 state->val);
    إذا (state->id < 0)
	أرجع;

    state->id = مطلق_قدر(state->id) * -1;

    // grow indent for state->out
    indent->ga_len -= 1;
    إذا (state->out1)
	GA_CONCAT_LITERAL(indent, "| ");
    وإلا
	GA_CONCAT_LITERAL(indent, "  ");
    ga_append(indent, NUL);

    nfa_print_state2(debugf, state->باہرلا, indent);

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

    إذا (debugf == عدم)
	أرجع;

    nfa_print_state(debugf, prog->شروع);

    إذا (prog->reganch)
	fprintf(debugf, "reganch: %d\n", prog->reganch);
    إذا (prog->regstart != NUL)
	fprintf(debugf, "regstart: %c (decimal: %d)\n",
		prog->regstart, prog->regstart);
    إذا (prog->میل_متن != عدم)
	fprintf(debugf, "match_text: \"%s\"\n", prog->میل_متن);

    fclose(debugf);
}
# endif	    // ENABLE_LOG
#جے_انت	    // DEBUG

/*
 * Parse r.e. @expr and convert it into postfix form.
 * Return the postfix string on success, NULL otherwise.
 */
    ساكن صحيح *
re2post(فراغ)
{
    إذا (nfa_reg(REG_NOPAREN) == FAIL)
	أرجع عدم;
    EMIT(NFA_MOPEN);
    أرجع post_شروع;
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
alloc_state(صحيح c, nfa_state_T *باہرلا, nfa_state_T *out1)
{
    nfa_state_T *s;

    إذا (istate >= nstate)
	أرجع عدم;

    s = &state_ptr[istate++];

    s->c    = c;
    s->باہرلا  = باہرلا;
    s->out1 = out1;
    s->val  = 0;

    s->id   = istate;
    s->lastlist[0] = 0;
    s->lastlist[1] = 0;

    أرجع s;
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
    Ptrlist	*اگلا;
    nfa_state_T	*s;
};

بنية Frag
{
    nfa_state_T *شروع;
    Ptrlist	*باہرلا;
};
تعريفنوع بنية Frag Frag_T;

// Reused across compilations by post2nfa(); freed in free_regexp_stuff().
ساكن Frag_T *nfa_ڈھیر;
ساكن صحيح nfa_ڈھیر_len;

/*
 * Initialize a Frag_T struct and return it.
 */
    ساكن Frag_T
frag(nfa_state_T *شروع, Ptrlist *باہرلا)
{
    Frag_T n;

    n.شروع = شروع;
    n.باہرلا = باہرلا;
    أرجع n;
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
    l->اگلا = عدم;
    أرجع l;
}

/*
 * Patch the list of states at out to point to start.
 */
    ساكن فراغ
patch(Ptrlist *l, nfa_state_T *s)
{
    Ptrlist *اگلا;

    لكل (; l; l = اگلا)
    {
	اگلا = l->اگلا;
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
    طالما (l1->اگلا)
	l1 = l1->اگلا;
    l1->اگلا = l2;
    أرجع oldl1;
}

/*
 * Stack used for transforming postfix form into NFA.
 */
ساكن Frag_T خالی_تھاواں_2;

    ساكن فراغ
st_error(صحيح *postfix بن_ورتیا, صحيح *سرا بن_ورتیا, صحيح *p بن_ورتیا)
{
#جے_تعریف_شدہ NFA_REGEXP_ERROR_LOG
    FILE *df;
    صحيح *p2;

    df = fopen(NFA_REGEXP_ERROR_LOG, "a");
    إذا (df)
    {
	fprintf(df, "Error popping the stack!\n");
# ifdef DEBUG
	fprintf(df, "Current regexp is \"%s\"\n", nfa_regengine.اظہاریہ);
# endif
	fprintf(df, "Postfix form is: ");
# ifdef DEBUG
	لكل (p2 = postfix; p2 < سرا; p2++)
	{
	    nfa_سیٹ_کوڈ(*p2);
	    fprintf(df, "%s, ", کوڈ);
	}
	nfa_سیٹ_کوڈ(*p);
	fprintf(df, "\nCurrent position is: ");
	لكل (p2 = postfix; p2 <= p; p2 ++)
	{
	    nfa_سیٹ_کوڈ(*p2);
	    fprintf(df, "%s, ", کوڈ);
	}
# وإلا
	لكل (p2 = postfix; p2 < سرا; p2++)
	    fprintf(df, "%d, ", *p2);
	fprintf(df, "\nCurrent position is: ");
	لكل (p2 = postfix; p2 <= p; p2 ++)
	    fprintf(df, "%d, ", *p2);
# endif
	fprintf(df, "\n--------------------------\n");
	fclose(df);
    }
#جے_انت
    emsg(_(e_nfa_regexp_could_not_نکالو_ڈھیر));
}

/*
 * Push an item onto the stack.
 */
    ساكن فراغ
st_ڈالو(Frag_T s, Frag_T **p, Frag_T *ڈھیر_سرا)
{
    Frag_T *stackp = *p;

    إذا (stackp >= ڈھیر_سرا)
	أرجع;
    *stackp = s;
    *p = *p + 1;
}

/*
 * Pop an item from the stack.
 */
    ساكن Frag_T
st_نکالو(Frag_T **p, Frag_T *ڈھیر)
{
    Frag_T *stackp;

    *p = *p - 1;
    stackp = *p;
    إذا (stackp < ڈھیر)
	أرجع خالی_تھاواں_2;
    أرجع **p;
}

/*
 * Estimate the maximum byte length of anything matching "state".
 * When unknown or unlimited return -1.
 */
    ساكن صحيح
nfa_max_چوڑائی(nfa_state_T *startstate, صحيح ڈونگھائی)
{
    صحيح		    l, r;
    nfa_state_T	    *state = startstate;
    صحيح		    len = 0;

    // detect looping in a NFA_SPLIT
    إذا (ڈونگھائی > 4)
	أرجع -1;

    طالما (state != عدم)
    {
	بدل (state->c)
	{
	    حالة NFA_سرا_INVISIBLE:
	    حالة NFA_سرا_INVISIBLE_NEG:
		// the end, return what we have
		أرجع len;

	    حالة NFA_ونڈو:
		// two alternatives, use the maximum
		l = nfa_max_چوڑائی(state->باہرلا, ڈونگھائی + 1);
		r = nfa_max_چوڑائی(state->out1, ڈونگھائی + 1);
		إذا (l < 0 || r < 0)
		    أرجع -1;
		أرجع len + (l > r ? l : r);

	    حالة NFA_ANY:
	    حالة NFA_شروع_COLL:
	    حالة NFA_شروع_NEG_COLL:
		// matches some character, including composing chars
		إذا (enc_utf8)
		    len += MB_MAXBYTES;
		وإلا إذا (has_mbyte)
		    len += 2;
		وإلا
		    ++len;
		إذا (state->c != NFA_ANY)
		{
		    // Skip over the compiled collection.
		    // malformed NFAs must not crash width estimation.
		    إذا (state->out1 == عدم || state->out1->باہرلا == عدم)
			أرجع -1;
		    state = state->out1->باہرلا;
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
	    حالة NFA_لفظ:
	    حالة NFA_NWORD:
	    حالة NFA_سر:
	    حالة NFA_NHEAD:
	    حالة NFA_ALPHA:
	    حالة NFA_NALPHA:
	    حالة NFA_ہیٹھلا:
	    حالة NFA_NLOWER:
	    حالة NFA_اتلا:
	    حالة NFA_NUPPER:
	    حالة NFA_ہیٹھلا_IC:
	    حالة NFA_NLOWER_IC:
	    حالة NFA_اتلا_IC:
	    حالة NFA_NUPPER_IC:
	    حالة NFA_ANY_COMPOSING:
		// possibly non-ascii
		إذا (has_mbyte)
		    len += 3;
		وإلا
		    ++len;
		اكسر;

	    حالة NFA_شروع_INVISIBLE:
	    حالة NFA_شروع_INVISIBLE_NEG:
	    حالة NFA_شروع_INVISIBLE_BEFORE:
	    حالة NFA_شروع_INVISIBLE_BEFORE_NEG:
		// zero-width, out1 points to the END state
		state = state->out1->باہرلا;
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
#جے_تعریف_شدہ FEAT_SYN_HL
	    حالة NFA_ZREF1:
	    حالة NFA_ZREF2:
	    حالة NFA_ZREF3:
	    حالة NFA_ZREF4:
	    حالة NFA_ZREF5:
	    حالة NFA_ZREF6:
	    حالة NFA_ZREF7:
	    حالة NFA_ZREF8:
	    حالة NFA_ZREF9:
#جے_انت
	    حالة NFA_NEWL:
	    حالة NFA_SKIP:
		// unknown width
		أرجع -1;

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
#جے_تعریف_شدہ FEAT_SYN_HL
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
#جے_انت
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
	    حالة NFA_خانہ_GT:
	    حالة NFA_خانہ_LT:
	    حالة NFA_VCOL_GT:
	    حالة NFA_VCOL_LT:
	    حالة NFA_MARK_GT:
	    حالة NFA_MARK_LT:
	    حالة NFA_VISUAL:
	    حالة NFA_LNUM:
	    حالة NFA_CURSOR:
	    حالة NFA_خانہ:
	    حالة NFA_VCOL:
	    حالة NFA_MARK:

	    حالة NFA_ZSTART:
	    حالة NFA_ZEND:
	    حالة NFA_OPT_CHARS:
	    حالة NFA_خالی_تھاواں:
	    حالة NFA_شروع_نمونہ:
	    حالة NFA_سرا_نمونہ:
	    حالة NFA_COMPOSING:
	    حالة NFA_سرا_COMPOSING:
		// zero-width
		اكسر;

	    افتراضي:
		إذا (state->c < 0)
		    // don't know what this is
		    أرجع -1;
		// normal character
		len += MB_CHAR2LEN(state->c);
		اكسر;
	}

	// normal way to continue
	state = state->باہرلا;
    }

    // unrecognized, "cannot happen"
    أرجع -1;
}

/*
 * Convert a postfix form into its equivalent NFA.
 * Return the NFA start state on success, NULL otherwise.
 */
    ساكن nfa_state_T *
post2nfa(صحيح *postfix, صحيح *سرا, صحيح nfa_calc_ناپ)
{
    صحيح		*p;
    صحيح		mopen;
    صحيح		mclose;
    Frag_T	*ڈھیر = عدم;
    Frag_T	*stackp = عدم;
    Frag_T	*ڈھیر_سرا = عدم;
    Frag_T	e1;
    Frag_T	e2;
    Frag_T	e;
    nfa_state_T	*s;
    nfa_state_T	*s1;
    nfa_state_T	*matchstate;
    nfa_state_T	*ret = عدم;

    إذا (postfix == عدم)
	أرجع عدم;

#تعریف_کرو ڈالو_2(s)	    st_ڈالو((s), &stackp, ڈھیر_سرا)
#تعریف_کرو نکالو_2()	    st_نکالو(&stackp, ڈھیر);		\
		    إذا (stackp < ڈھیر)			\
		    {					\
			st_error(postfix, سرا, p);	\
			أرجع عدم;			\
		    }

    إذا (nfa_calc_ناپ == FALSE)
    {
	// Reuse the fragment stack across compilations, growing when needed;
	// it is freed in free_regexp_stuff().
	إذا (nfa_ڈھیر == عدم || nfa_ڈھیر_len < nstate + 1)
	{
	    vim_free(nfa_ڈھیر);
	    nfa_ڈھیر = ALLOC_MULT(Frag_T, nstate + 1);
	    إذا (nfa_ڈھیر == عدم)
	    {
		nfa_ڈھیر_len = 0;
		أرجع عدم;
	    }
	    nfa_ڈھیر_len = nstate + 1;
	}
	ڈھیر = nfa_ڈھیر;
	stackp = ڈھیر;
	ڈھیر_سرا = ڈھیر + (nstate + 1);
    }

    لكل (p = postfix; p < سرا; ++p)
    {
	بدل (*p)
	{
	حالة NFA_CONCAT:
	    // Concatenation.
	    // Pay attention: this operator does not exist in the r.e. itself
	    // (it is implicit, really).  It is added when r.e. is translated
	    // to postfix form in re2post().
	    إذا (nfa_calc_ناپ == TRUE)
	    {
		// nstate += 0;
		اكسر;
	    }
	    e2 = نکالو_2();
	    e1 = نکالو_2();
	    patch(e1.باہرلا, e2.شروع);
	    ڈالو_2(frag(e1.شروع, e2.باہرلا));
	    اكسر;

	حالة NFA_OR:
	    // Alternation
	    إذا (nfa_calc_ناپ == TRUE)
	    {
		nstate++;
		اكسر;
	    }
	    e2 = نکالو_2();
	    e1 = نکالو_2();
	    s = alloc_state(NFA_ونڈو, e1.شروع, e2.شروع);
	    إذا (s == عدم)
		اذهب theend;
	    ڈالو_2(frag(s, append(e1.باہرلا, e2.باہرلا)));
	    اكسر;

	حالة NFA_تارا:
	    // Zero or more, prefer more
	    إذا (nfa_calc_ناپ == TRUE)
	    {
		nstate++;
		اكسر;
	    }
	    e = نکالو_2();
	    s = alloc_state(NFA_ونڈو, e.شروع, عدم);
	    إذا (s == عدم)
		اذهب theend;
	    patch(e.باہرلا, s);
	    ڈالو_2(frag(s, list1(&s->out1)));
	    اكسر;

	حالة NFA_تارا_NONGREEDY:
	    // Zero or more, prefer zero
	    إذا (nfa_calc_ناپ == TRUE)
	    {
		nstate++;
		اكسر;
	    }
	    e = نکالو_2();
	    s = alloc_state(NFA_ونڈو, عدم, e.شروع);
	    إذا (s == عدم)
		اذهب theend;
	    patch(e.باہرلا, s);
	    ڈالو_2(frag(s, list1(&s->باہرلا)));
	    اكسر;

	حالة NFA_QUEST:
	    // one or zero atoms=> greedy match
	    إذا (nfa_calc_ناپ == TRUE)
	    {
		nstate++;
		اكسر;
	    }
	    e = نکالو_2();
	    s = alloc_state(NFA_ونڈو, e.شروع, عدم);
	    إذا (s == عدم)
		اذهب theend;
	    ڈالو_2(frag(s, append(e.باہرلا, list1(&s->out1))));
	    اكسر;

	حالة NFA_QUEST_NONGREEDY:
	    // zero or one atoms => non-greedy match
	    إذا (nfa_calc_ناپ == TRUE)
	    {
		nstate++;
		اكسر;
	    }
	    e = نکالو_2();
	    s = alloc_state(NFA_ونڈو, عدم, e.شروع);
	    إذا (s == عدم)
		اذهب theend;
	    ڈالو_2(frag(s, append(e.باہرلا, list1(&s->باہرلا))));
	    اكسر;

	حالة NFA_سرا_COLL:
	حالة NFA_سرا_NEG_COLL:
	    // On the stack is the sequence starting with NFA_START_COLL or
	    // NFA_START_NEG_COLL and all possible characters. Patch it to
	    // add the output to the start.
	    إذا (nfa_calc_ناپ == TRUE)
	    {
		nstate++;
		اكسر;
	    }
	    e = نکالو_2();
	    s = alloc_state(NFA_سرا_COLL, عدم, عدم);
	    إذا (s == عدم)
		اذهب theend;
	    patch(e.باہرلا, s);
	    e.شروع->out1 = s;
	    ڈالو_2(frag(e.شروع, list1(&s->باہرلا)));
	    اكسر;

	حالة NFA_RANGE:
	    // Before this are two characters, the low and high end of a
	    // range.  Turn them into two states with MIN and MAX.
	    إذا (nfa_calc_ناپ == TRUE)
	    {
		// nstate += 0;
		اكسر;
	    }
	    e2 = نکالو_2();
	    e1 = نکالو_2();
	    e2.شروع->val = e2.شروع->c;
	    e2.شروع->c = NFA_RANGE_MAX;
	    e1.شروع->val = e1.شروع->c;
	    e1.شروع->c = NFA_RANGE_MIN;
	    patch(e1.باہرلا, e2.شروع);
	    ڈالو_2(frag(e1.شروع, e2.باہرلا));
	    اكسر;

	حالة NFA_خالی_تھاواں:
	    // 0-length, used in a repetition with max/min count of 0
	    إذا (nfa_calc_ناپ == TRUE)
	    {
		nstate++;
		اكسر;
	    }
	    s = alloc_state(NFA_خالی_تھاواں, عدم, عدم);
	    إذا (s == عدم)
		اذهب theend;
	    ڈالو_2(frag(s, list1(&s->باہرلا)));
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
	    إذا (nfa_calc_ناپ == TRUE)
	    {
		nstate += n;
		اكسر;
	    }
	    s = عدم; // avoid compiler warning
	    e1.باہرلا = عدم; // stores list with out1's
	    s1 = عدم; // previous NFA_SPLIT to connect to
	    طالما (n-- > 0)
	    {
		e = نکالو_2(); // get character
		s = alloc_state(NFA_ونڈو, e.شروع, عدم);
		إذا (s == عدم)
		    اذهب theend;
		إذا (e1.باہرلا == عدم)
		    e1 = e;
		patch(e.باہرلا, s1);
		append(e1.باہرلا, list1(&s->out1));
		s1 = s;
	    }
	    ڈالو_2(frag(s, e1.باہرلا));
	    اكسر;
	  }

	حالة NFA_PREV_ATOM_NO_چوڑائی:
	حالة NFA_PREV_ATOM_NO_چوڑائی_NEG:
	حالة NFA_PREV_ATOM_JUST_BEFORE:
	حالة NFA_PREV_ATOM_JUST_BEFORE_NEG:
	حالة NFA_PREV_ATOM_LIKE_نمونہ:
	  {
	    صحيح before = (*p == NFA_PREV_ATOM_JUST_BEFORE
				      || *p == NFA_PREV_ATOM_JUST_BEFORE_NEG);
	    صحيح نمونہ = (*p == NFA_PREV_ATOM_LIKE_نمونہ);
	    صحيح شروع_state;
	    صحيح سرا_state;
	    صحيح n = 0;
	    nfa_state_T *zend;
	    nfa_state_T *skip;

	    بدل (*p)
	    {
		حالة NFA_PREV_ATOM_NO_چوڑائی:
		    شروع_state = NFA_شروع_INVISIBLE;
		    سرا_state = NFA_سرا_INVISIBLE;
		    اكسر;
		حالة NFA_PREV_ATOM_NO_چوڑائی_NEG:
		    شروع_state = NFA_شروع_INVISIBLE_NEG;
		    سرا_state = NFA_سرا_INVISIBLE_NEG;
		    اكسر;
		حالة NFA_PREV_ATOM_JUST_BEFORE:
		    شروع_state = NFA_شروع_INVISIBLE_BEFORE;
		    سرا_state = NFA_سرا_INVISIBLE;
		    اكسر;
		حالة NFA_PREV_ATOM_JUST_BEFORE_NEG:
		    شروع_state = NFA_شروع_INVISIBLE_BEFORE_NEG;
		    سرا_state = NFA_سرا_INVISIBLE_NEG;
		    اكسر;
		افتراضي: // NFA_PREV_ATOM_LIKE_PATTERN:
		    شروع_state = NFA_شروع_نمونہ;
		    سرا_state = NFA_سرا_نمونہ;
		    اكسر;
	    }

	    إذا (before)
		n = *++p; // get the count

	    // The \@= operator: match the preceding atom with zero width.
	    // The \@! operator: no match for the preceding atom.
	    // The \@<= operator: match for the preceding atom.
	    // The \@<! operator: no match for the preceding atom.
	    // Surrounds the preceding atom with START_INVISIBLE and
	    // END_INVISIBLE, similarly to MOPEN.

	    إذا (nfa_calc_ناپ == TRUE)
	    {
		nstate += نمونہ ? 4 : 2;
		اكسر;
	    }
	    e = نکالو_2();
	    s1 = alloc_state(سرا_state, عدم, عدم);
	    إذا (s1 == عدم)
		اذهب theend;

	    s = alloc_state(شروع_state, e.شروع, s1);
	    إذا (s == عدم)
		اذهب theend;
	    إذا (نمونہ)
	    {
		// NFA_ZEND -> NFA_END_PATTERN -> NFA_SKIP -> what follows.
		skip = alloc_state(NFA_SKIP, عدم, عدم);
		إذا (skip == عدم)
		    اذهب theend;
		zend = alloc_state(NFA_ZEND, s1, عدم);
		إذا (zend == عدم)
		    اذهب theend;
		s1->باہرلا= skip;
		patch(e.باہرلا, zend);
		ڈالو_2(frag(s, list1(&skip->باہرلا)));
	    }
	    وإلا
	    {
		patch(e.باہرلا, s1);
		ڈالو_2(frag(s, list1(&s1->باہرلا)));
		إذا (before)
		{
		    إذا (n <= 0)
			// See if we can guess the maximum width, it avoids a
			// lot of pointless tries.
			n = nfa_max_چوڑائی(e.شروع, 0);
		    s->val = n; // store the count
		}
	    }
	    اكسر;
	  }

	حالة NFA_COMPOSING:	// char with composing char
#إذا 0
	    // TODO
	    إذا (regflags & RF_ICOMBINE)
	    {
		// use the base character only
	    }
#جے_انت
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
#جے_تعریف_شدہ FEAT_SYN_HL
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
#جے_انت
	حالة NFA_NOPEN:	// \%( \) "Invisible Submatch"
	    إذا (nfa_calc_ناپ == TRUE)
	    {
		nstate += 2;
		اكسر;
	    }

	    mopen = *p;
	    بدل (*p)
	    {
		حالة NFA_NOPEN: mclose = NFA_NCLOSE; اكسر;
#جے_تعریف_شدہ FEAT_SYN_HL
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
#جے_انت
		حالة NFA_COMPOSING: mclose = NFA_سرا_COMPOSING; اكسر;
		افتراضي:
		    // NFA_MOPEN, NFA_MOPEN1 .. NFA_MOPEN9
		    mclose = *p + NSUBEXP;
		    اكسر;
	    }

	    // Allow "NFA_MOPEN" as a valid postfix representation for
	    // the empty regexp "". In this case, the NFA will be
	    // NFA_MOPEN -> NFA_MCLOSE. Note that this also allows
	    // empty groups of parenthesis, and empty mbyte chars
	    إذا (stackp == ڈھیر)
	    {
		s = alloc_state(mopen, عدم, عدم);
		إذا (s == عدم)
		    اذهب theend;
		s1 = alloc_state(mclose, عدم, عدم);
		إذا (s1 == عدم)
		    اذهب theend;
		patch(list1(&s->باہرلا), s1);
		ڈالو_2(frag(s, list1(&s1->باہرلا)));
		اكسر;
	    }

	    // At least one node was emitted before NFA_MOPEN, so
	    // at least one node will be between NFA_MOPEN and NFA_MCLOSE
	    e = نکالو_2();
	    s = alloc_state(mopen, e.شروع, عدم);   // `('
	    إذا (s == عدم)
		اذهب theend;

	    s1 = alloc_state(mclose, عدم, عدم);   // `)'
	    إذا (s1 == عدم)
		اذهب theend;
	    patch(e.باہرلا, s1);

	    إذا (mopen == NFA_COMPOSING)
		// COMPOSING->out1 = END_COMPOSING
		patch(list1(&s->out1), s1);

	    ڈالو_2(frag(s, list1(&s1->باہرلا)));
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
#جے_تعریف_شدہ FEAT_SYN_HL
	حالة NFA_ZREF1:
	حالة NFA_ZREF2:
	حالة NFA_ZREF3:
	حالة NFA_ZREF4:
	حالة NFA_ZREF5:
	حالة NFA_ZREF6:
	حالة NFA_ZREF7:
	حالة NFA_ZREF8:
	حالة NFA_ZREF9:
#جے_انت
	    إذا (nfa_calc_ناپ == TRUE)
	    {
		nstate += 2;
		اكسر;
	    }
	    s = alloc_state(*p, عدم, عدم);
	    إذا (s == عدم)
		اذهب theend;
	    s1 = alloc_state(NFA_SKIP, عدم, عدم);
	    إذا (s1 == عدم)
		اذهب theend;
	    patch(list1(&s->باہرلا), s1);
	    ڈالو_2(frag(s, list1(&s1->باہرلا)));
	    اكسر;

	حالة NFA_LNUM:
	حالة NFA_LNUM_GT:
	حالة NFA_LNUM_LT:
	حالة NFA_VCOL:
	حالة NFA_VCOL_GT:
	حالة NFA_VCOL_LT:
	حالة NFA_خانہ:
	حالة NFA_خانہ_GT:
	حالة NFA_خانہ_LT:
	حالة NFA_MARK:
	حالة NFA_MARK_GT:
	حالة NFA_MARK_LT:
	  {
	    صحيح n = *++p; // lnum, col or mark name

	    إذا (nfa_calc_ناپ == TRUE)
	    {
		nstate += 1;
		اكسر;
	    }
	    s = alloc_state(p[-1], عدم, عدم);
	    إذا (s == عدم)
		اذهب theend;
	    s->val = n;
	    ڈالو_2(frag(s, list1(&s->باہرلا)));
	    اكسر;
	  }

	حالة NFA_ZSTART:
	حالة NFA_ZEND:
	افتراضي:
	    // Operands
	    إذا (nfa_calc_ناپ == TRUE)
	    {
		nstate++;
		اكسر;
	    }
	    s = alloc_state(*p, عدم, عدم);
	    إذا (s == عدم)
		اذهب theend;
	    ڈالو_2(frag(s, list1(&s->باہرلا)));
	    اكسر;

	} // switch(*p)

    } // for (p = postfix; *p; ++p)

    إذا (nfa_calc_ناپ == TRUE)
    {
	nstate++;
	اذهب theend;	// Return value when counting size is ignored anyway
    }

    e = نکالو_2();
    إذا (stackp != ڈھیر)
	EMSG_RET_NULL(_(e_nfa_regexp_while_converting_from_postfix_منزل_nfa_too_many_stats_بایاں_on_ڈھیر));

    إذا (istate >= nstate)
	EMSG_RET_NULL(_(e_nfa_regexp_not_enough_space_منزل_store_سارا_nfa));

    matchstate = &state_ptr[istate++]; // the match state
    matchstate->c = NFA_میل;
    matchstate->باہرلا = matchstate->out1 = عدم;
    matchstate->id = 0;

    patch(e.باہرلا, matchstate);
    ret = e.شروع;

theend:
    أرجع ret;

#تعریف_رد POP1
#تعریف_رد PUSH1
#تعریف_رد POP2
#تعریف_رد PUSH2
#تعریف_رد نکالو_2
#تعریف_رد ڈالو_2
}

/*
 * After building the NFA program, inspect it to add optimization hints.
 */
    ساكن فراغ
nfa_postprocess(nfa_regprog_T *prog)
{
    صحيح i;
    صحيح c;

    لكل (i = 0; i < prog->nstate; ++i)
    {
	c = prog->state[i].c;
	إذا (c == NFA_شروع_INVISIBLE
		|| c == NFA_شروع_INVISIBLE_NEG
		|| c == NFA_شروع_INVISIBLE_BEFORE
		|| c == NFA_شروع_INVISIBLE_BEFORE_NEG)
	{
	    صحيح directly;

	    // Do it directly when what follows is possibly the end of the
	    // match.
	    إذا (میل_follows(prog->state[i].out1->باہرلا, 0))
		directly = TRUE;
	    وإلا
	    {
		صحيح ch_invisible = failure_chance(prog->state[i].باہرلا, 0);
		صحيح ch_follows = failure_chance(prog->state[i].out1->باہرلا, 0);

		// Postpone when the invisible match is expensive or has a
		// lower chance of failing.
		إذا (c == NFA_شروع_INVISIBLE_BEFORE
		     || c == NFA_شروع_INVISIBLE_BEFORE_NEG)
		{
		    // "before" matches are very expensive when
		    // unbounded, always prefer what follows then,
		    // unless what follows will always match.
		    // Otherwise strongly prefer what follows.
		    إذا (prog->state[i].val <= 0 && ch_follows > 0)
			directly = FALSE;
		    وإلا
			directly = ch_follows * 10 < ch_invisible;
		}
		وإلا
		{
		    // normal invisible, first do the one with the
		    // highest failure chance
		    directly = ch_follows < ch_invisible;
		}
	    }
	    إذا (directly)
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
    صحيح	    وچ_use; // number of subexpr with useful info

    // When REG_MULTI is TRUE list.multi is used, otherwise list.line.
    اتحاد
    {
	بنية multipos
	{
	    linenr_T	شروع_lnum;
	    linenr_T	سرا_lnum;
	    colnr_T	شروع_خانہ;
	    colnr_T	سرا_خانہ;
	} multi[NSUBEXP];
	بنية linepos
	{
	    char_u	*شروع;
	    char_u	*سرا;
	} سطر[NSUBEXP];
    } list;
    colnr_T	orig_شروع_خانہ;  // list.multi[0].start_col without \zs
} regsub_T;

تعريفنوع بنية
{
    regsub_T	norm; // \( .. \) matches
#جے_تعریف_شدہ FEAT_SYN_HL
    regsub_T	synt; // \z( .. \) matches
#جے_انت
} regsubs_T;

// nfa_pim_T stores a Postponed Invisible Match.
تعريفنوع بنية nfa_pim_S nfa_pim_T;
بنية nfa_pim_S
{
    صحيح		نتیجہ;		// NFA_PIM_*, see below
    nfa_state_T	*state;		// the invisible match start state
    regsubs_T	subs;		// submatch info, only party used
    اتحاد
    {
	lpos_T	pos;
	char_u	*ptr;
    } سرا;			// where the match must end
};

// Values for done in nfa_pim_T.
#تعریف_کرو NFA_PIM_بن_ورتیا   0	// pim not used
#تعریف_کرو NFA_PIM_TODO     1	// pim not done yet
#تعریف_کرو NFA_PIM_میل    2	// pim executed, matches
#تعریف_کرو NFA_PIM_NOMATCH  3	// pim executed, no match


// nfa_thread_T contains execution information of a NFA state
تعريفنوع بنية
{
    nfa_state_T	*state;
    صحيح		گنتی;
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

#جے_تعریف_شدہ ENABLE_LOG
ساكن فراغ log_subexpr(regsub_T *sub);

    ساكن فراغ
log_subsexpr(regsubs_T *subs)
{
    log_subexpr(&subs->norm);
# ifdef FEAT_SYN_HL
    إذا (rex.nfa_has_zsubexpr)
	log_subexpr(&subs->synt);
# endif
}

    ساكن فراغ
log_subexpr(regsub_T *sub)
{
    صحيح j;

    لكل (j = 0; j < sub->وچ_use; j++)
	إذا (REG_MULTI)
	    fprintf(log_fd,
		    "*** group %d, start: c=%d, l=%d, end: c=%d, l=%d\n",
		    j,
		    sub->list.multi[j].شروع_خانہ,
		    (صحيح)sub->list.multi[j].شروع_lnum,
		    sub->list.multi[j].سرا_خانہ,
		    (صحيح)sub->list.multi[j].سرا_lnum);
	وإلا
	{
	    حرف *s = (حرف *)sub->list.سطر[j].شروع;
	    حرف *e = (حرف *)sub->list.سطر[j].سرا;

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

    إذا (pim == عدم || pim->نتیجہ == NFA_PIM_بن_ورتیا)
	buf[0] = NUL;
    وإلا
    {
	sprintf(buf, " PIM col %d", REG_MULTI ? (صحيح)pim->سرا.pos.خانہ
		: (صحيح)(pim->سرا.ptr - rex.داخلہ));
    }
    أرجع buf;
}

#جے_انت

// Used during execution: whether a match has been found.
ساكن صحيح	    nfa_میل;
#جے_تعریف_شدہ FEAT_RELTIME
ساكن صحيح	   *nfa_timed_باہرلا;
#جے_انت

ساكن مضمن فراغ copy_sub(regsub_T *منزل, regsub_T *from);
ساكن صحيح pim_equal(nfa_pim_T *one, nfa_pim_T *two);

/*
 * Copy postponed invisible match info from "from" to "to".
 */
    ساكن فراغ
copy_pim(nfa_pim_T *منزل, nfa_pim_T *from)
{
    منزل->نتیجہ = from->نتیجہ;
    منزل->state = from->state;
    copy_sub(&منزل->subs.norm, &from->subs.norm);
#جے_تعریف_شدہ FEAT_SYN_HL
    إذا (rex.nfa_has_zsubexpr)
	copy_sub(&منزل->subs.synt, &from->subs.synt);
#جے_انت
    منزل->سرا = from->سرا;
}

    ساكن فراغ
clear_sub(regsub_T *sub)
{
    إذا (REG_MULTI)
	// Use 0xff to set lnum to -1
	vim_memset(sub->list.multi, 0xff,
				  حجم(بنية multipos) * rex.nfa_nsubexpr);
    وإلا
	vim_memset(sub->list.سطر, 0,
				   حجم(بنية linepos) * rex.nfa_nsubexpr);
    sub->وچ_use = 0;
}

/*
 * Copy the submatches from "from" to "to".
 */
    ساكن مضمن فراغ
copy_sub(regsub_T *منزل, regsub_T *from)
{
    منزل->وچ_use = from->وچ_use;
    إذا (from->وچ_use <= 0)
	أرجع;

    // Copy the match start and end positions.  addstate() calls this for
    // every state it adds, so it is very hot.  Patterns without capturing
    // groups only use subexpression zero (the whole match), so special-case
    // "in_use == 1" with a direct assignment to avoid the mch_memmove() call.
    إذا (REG_MULTI)
    {
	إذا (from->وچ_use == 1)
	    منزل->list.multi[0] = from->list.multi[0];
	وإلا
	    mch_memmove(&منزل->list.multi[0],
		    &from->list.multi[0],
		    حجم(بنية multipos) * from->وچ_use);
	منزل->orig_شروع_خانہ = from->orig_شروع_خانہ;
    }
    وإلا
    {
	إذا (from->وچ_use == 1)
	    منزل->list.سطر[0] = from->list.سطر[0];
	وإلا
	    mch_memmove(&منزل->list.سطر[0],
		    &from->list.سطر[0],
		    حجم(بنية linepos) * from->وچ_use);
    }
}

/*
 * Like copy_sub() but exclude the main match.
 */
    ساكن فراغ
copy_sub_off(regsub_T *منزل, regsub_T *from)
{
    إذا (منزل->وچ_use < from->وچ_use)
	منزل->وچ_use = from->وچ_use;
    إذا (from->وچ_use <= 1)
	أرجع;

    // Copy the match start and end positions.
    إذا (REG_MULTI)
	mch_memmove(&منزل->list.multi[1],
		&from->list.multi[1],
		حجم(بنية multipos) * (from->وچ_use - 1));
    وإلا
	mch_memmove(&منزل->list.سطر[1],
		&from->list.سطر[1],
		حجم(بنية linepos) * (from->وچ_use - 1));
}

/*
 * Like copy_sub() but only do the end of the main match if \ze is present.
 */
    ساكن فراغ
copy_ze_off(regsub_T *منزل, regsub_T *from)
{
    إذا (!rex.nfa_has_zend)
	أرجع;

    إذا (REG_MULTI)
    {
	إذا (from->list.multi[0].سرا_lnum >= 0)
	{
	    منزل->list.multi[0].سرا_lnum = from->list.multi[0].سرا_lnum;
	    منزل->list.multi[0].سرا_خانہ = from->list.multi[0].سرا_خانہ;
	}
    }
    وإلا
    {
	إذا (from->list.سطر[0].سرا != عدم)
	    منزل->list.سطر[0].سرا = from->list.سطر[0].سرا;
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

    todo = sub1->وچ_use > sub2->وچ_use ? sub1->وچ_use : sub2->وچ_use;
    إذا (REG_MULTI)
    {
	لكل (i = 0; i < todo; ++i)
	{
	    إذا (i < sub1->وچ_use)
		s1 = sub1->list.multi[i].شروع_lnum;
	    وإلا
		s1 = -1;
	    إذا (i < sub2->وچ_use)
		s2 = sub2->list.multi[i].شروع_lnum;
	    وإلا
		s2 = -1;
	    إذا (s1 != s2)
		أرجع FALSE;
	    إذا (s1 != -1 && sub1->list.multi[i].شروع_خانہ
					     != sub2->list.multi[i].شروع_خانہ)
		أرجع FALSE;

	    إذا (rex.nfa_has_backref)
	    {
		إذا (i < sub1->وچ_use)
		    s1 = sub1->list.multi[i].سرا_lnum;
		وإلا
		    s1 = -1;
		إذا (i < sub2->وچ_use)
		    s2 = sub2->list.multi[i].سرا_lnum;
		وإلا
		    s2 = -1;
		إذا (s1 != s2)
		    أرجع FALSE;
		إذا (s1 != -1 && sub1->list.multi[i].سرا_خانہ
					       != sub2->list.multi[i].سرا_خانہ)
		أرجع FALSE;
	    }
	}
    }
    وإلا
    {
	لكل (i = 0; i < todo; ++i)
	{
	    إذا (i < sub1->وچ_use)
		sp1 = sub1->list.سطر[i].شروع;
	    وإلا
		sp1 = عدم;
	    إذا (i < sub2->وچ_use)
		sp2 = sub2->list.سطر[i].شروع;
	    وإلا
		sp2 = عدم;
	    إذا (sp1 != sp2)
		أرجع FALSE;
	    إذا (rex.nfa_has_backref)
	    {
		إذا (i < sub1->وچ_use)
		    sp1 = sub1->list.سطر[i].سرا;
		وإلا
		    sp1 = عدم;
		إذا (i < sub2->وچ_use)
		    sp2 = sub2->list.سطر[i].سرا;
		وإلا
		    sp2 = عدم;
		إذا (sp1 != sp2)
		    أرجع FALSE;
	    }
	}
    }

    أرجع TRUE;
}

#جے_تعریف_شدہ FEAT_RELTIME
/*
 * Check if we are past the time limit, if there is one.
 */
    ساكن صحيح
nfa_did_ویلا_باہرلا(فراغ)
{
    إذا (*timeout_flag)
    {
	إذا (nfa_timed_باہرلا != عدم)
	{
# ifdef FEAT_EVAL
	    إذا (!*nfa_timed_باہرلا)
		ch_log(عدم, "NFA regexp timed out");
# endif
	    *nfa_timed_باہرلا = TRUE;
	}
	أرجع TRUE;
    }
    أرجع FALSE;
}
#جے_انت

#جے_تعریف_شدہ ENABLE_LOG
    ساكن فراغ
open_debug_log(صحيح نتیجہ)
{
    log_fd = fopen(NFA_REGEXP_لگاتار_LOG, "a");
    إذا (log_fd == عدم)
    {
	emsg(_(e_log_open_failed));
	log_fd = stderr;
    }

    fprintf(log_fd, "****************************\n");
    fprintf(log_fd, "FINISHED RUNNING nfa_regmatch() recursively\n");
    fprintf(log_fd, "MATCH = %s\n", نتیجہ == TRUE ? "OK" : نتیجہ == MAYBE
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
    صحيح خانہ;

    إذا (sub->وچ_use <= 0)
	خانہ = -1;
    وإلا إذا (REG_MULTI)
	خانہ = sub->list.multi[0].شروع_خانہ;
    وإلا
	خانہ = (صحيح)(sub->list.سطر[0].شروع - rex.سطر);
    nfa_سیٹ_کوڈ(state->c);
    إذا (log_fd == عدم)
	open_debug_log(MAYBE);

    fprintf(log_fd, "> %s state %d to list %d. char %d: %s (start col %d)%s\n",
	    action, مطلق_قدر(state->id), lid, state->c, کوڈ, خانہ,
	    pim_info(pim));
}
#جے_انت

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

    لكل (i = 0; i < l->n; ++i)
    {
	thread = &l->t[i];
	إذا (thread->state->id == state->id
		&& sub_equal(&thread->subs.norm, &subs->norm)
#جے_تعریف_شدہ FEAT_SYN_HL
		&& (!rex.nfa_has_zsubexpr
				|| sub_equal(&thread->subs.synt, &subs->synt))
#جے_انت
		&& pim_equal(&thread->pim, pim))
	    أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Return TRUE if "one" and "two" are equal.  That includes when both are not
 * set.
 */
    ساكن صحيح
pim_equal(nfa_pim_T *one, nfa_pim_T *two)
{
    صحيح one_بن_ورتیا = (one == عدم || one->نتیجہ == NFA_PIM_بن_ورتیا);
    صحيح two_بن_ورتیا = (two == عدم || two->نتیجہ == NFA_PIM_بن_ورتیا);

    إذا (one_بن_ورتیا)
	// one is unused: equal when two is also unused
	أرجع two_بن_ورتیا;
    إذا (two_بن_ورتیا)
	// one is used and two is not: not equal
	أرجع FALSE;
    // compare the state id
    إذا (one->state->id != two->state->id)
	أرجع FALSE;
    // compare the position
    إذا (REG_MULTI)
	أرجع one->سرا.pos.lnum == two->سرا.pos.lnum
	    && one->سرا.pos.خانہ == two->سرا.pos.خانہ;
    أرجع one->سرا.ptr == two->سرا.ptr;
}

/*
 * Return TRUE if "state" leads to a NFA_MATCH without advancing the input.
 */
    ساكن صحيح
میل_follows(nfa_state_T *startstate, صحيح ڈونگھائی)
{
    nfa_state_T	    *state = startstate;

    // avoid too much recursion
    إذا (ڈونگھائی > 10)
	أرجع FALSE;

    طالما (state != عدم)
    {
	بدل (state->c)
	{
	    حالة NFA_میل:
	    حالة NFA_MCLOSE:
	    حالة NFA_سرا_INVISIBLE:
	    حالة NFA_سرا_INVISIBLE_NEG:
	    حالة NFA_سرا_نمونہ:
		أرجع TRUE;

	    حالة NFA_ونڈو:
		أرجع میل_follows(state->باہرلا, ڈونگھائی + 1)
				     || میل_follows(state->out1, ڈونگھائی + 1);

	    حالة NFA_شروع_INVISIBLE:
	    حالة NFA_شروع_INVISIBLE_پہلا:
	    حالة NFA_شروع_INVISIBLE_BEFORE:
	    حالة NFA_شروع_INVISIBLE_BEFORE_پہلا:
	    حالة NFA_شروع_INVISIBLE_NEG:
	    حالة NFA_شروع_INVISIBLE_NEG_پہلا:
	    حالة NFA_شروع_INVISIBLE_BEFORE_NEG:
	    حالة NFA_شروع_INVISIBLE_BEFORE_NEG_پہلا:
	    حالة NFA_COMPOSING:
		// skip ahead to next state
		state = state->out1->باہرلا;
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
	    حالة NFA_لفظ:
	    حالة NFA_NWORD:
	    حالة NFA_سر:
	    حالة NFA_NHEAD:
	    حالة NFA_ALPHA:
	    حالة NFA_NALPHA:
	    حالة NFA_ہیٹھلا:
	    حالة NFA_NLOWER:
	    حالة NFA_اتلا:
	    حالة NFA_NUPPER:
	    حالة NFA_ہیٹھلا_IC:
	    حالة NFA_NLOWER_IC:
	    حالة NFA_اتلا_IC:
	    حالة NFA_NUPPER_IC:
	    حالة NFA_شروع_COLL:
	    حالة NFA_شروع_NEG_COLL:
	    حالة NFA_NEWL:
		// state will advance input
		أرجع FALSE;

	    افتراضي:
		إذا (state->c > 0)
		    // state will advance input
		    أرجع FALSE;

		// Others: zero-width or possibly zero-width, might still find
		// a match at the same position, keep looking.
		اكسر;
	}
	state = state->باہرلا;
    }
    أرجع FALSE;
}


/*
 * Return TRUE if "state" is already in list "l".
 */
    ساكن صحيح
state_وچ_list(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs)	// pointers to subexpressions
{
    إذا (state->lastlist[nfa_ll_اشاریہ] == l->id)
    {
	إذا (!rex.nfa_has_backref || has_state_with_pos(l, state, subs, عدم))
	    أرجع TRUE;
    }
    أرجع FALSE;
}

// Offset used for "off" by addstate_here().
#تعریف_کرو ADDSTATE_ایتھے_وتھ 10

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
    صحيح			ڈونگھائی)	    // recursion depth
{
    صحيح			subidx;
    صحيح			off = off_arg;
    صحيح			جوڑو_ایتھے = FALSE;
    صحيح			listindex = 0;
    صحيح			k;
    صحيح			لبھیا = FALSE;
    nfa_thread_T	*thread;
    بنية multipos	save_multipos;
    صحيح			save_وچ_use;
    char_u		*save_ptr;
    صحيح			i;
    regsub_T		*sub;
    regsubs_T		*subs = subs_arg;
    ساكن regsubs_T	عارضی_subs;
#جے_تعریف_شدہ ENABLE_LOG
    صحيح			did_print = FALSE;
#جے_انت

#جے_تعریف_شدہ FEAT_RELTIME
    إذا (nfa_did_ویلا_باہرلا())
	أرجع عدم;
#جے_انت

    // This function is called recursively.  When the depth is too much we run
    // out of stack and crash, limit recursiveness here.
    إذا (ڈونگھائی >= 5000 || subs == عدم)
	أرجع عدم;

    إذا (off_arg <= -ADDSTATE_ایتھے_وتھ)
    {
	جوڑو_ایتھے = TRUE;
	off = 0;
	listindex = -(off_arg + ADDSTATE_ایتھے_وتھ);
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
#جے_تعریف_شدہ FEAT_SYN_HL
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
#جے_انت
	حالة NFA_MOPEN:
	حالة NFA_ZEND:
	حالة NFA_ونڈو:
	حالة NFA_خالی_تھاواں:
	    // These nodes are not added themselves but their "out" and/or
	    // "out1" may be added below.
	    اكسر;

	حالة NFA_BOL:
	حالة NFA_BOF:
	    // "^" won't match past end-of-line, don't bother trying.
	    // Except when at the end of the line, or when we are going to the
	    // next line for a look-behind match.
	    إذا (rex.داخلہ > rex.سطر
		    && *rex.داخلہ != NUL
		    && (nfa_endp == عدم
			|| !REG_MULTI
			|| rex.lnum == nfa_endp->se_u.pos.lnum))
		اذهب skip_جوڑو;
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
#جے_تعریف_شدہ FEAT_SYN_HL
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
#جے_انت
	حالة NFA_NOPEN:
	حالة NFA_ZSTART:
	    // These nodes need to be added so that we can bail out when it
	    // was added to this list before at the same position to avoid an
	    // endless loop for "\(\)*"

	افتراضي:
	    إذا (state->lastlist[nfa_ll_اشاریہ] == l->id && state->c != NFA_SKIP)
	    {
		// This state is already in the list, don't add it again,
		// unless it is an MOPEN that is used for a backreference or
		// when there is a PIM. For NFA_MATCH check the position,
		// lower position is preferred.
		إذا (!rex.nfa_has_backref && pim == عدم && !l->has_pim
						     && state->c != NFA_میل)
		{
		    // When called from addstate_here() do insert before
		    // existing states.
		    إذا (جوڑو_ایتھے)
		    {
			لكل (k = 0; k < l->n && k < listindex; ++k)
			    إذا (l->t[k].state->id == state->id)
			    {
				لبھیا = TRUE;
				اكسر;
			    }
		    }
		    إذا (!جوڑو_ایتھے || لبھیا)
		    {
skip_جوڑو:
#جے_تعریف_شدہ ENABLE_LOG
			nfa_سیٹ_کوڈ(state->c);
			fprintf(log_fd, "> Not adding state %d to list %d. char %d: %s pim: %s has_pim: %d found: %d\n",
			    مطلق_قدر(state->id), l->id, state->c, کوڈ,
			    pim == عدم ? "NULL" : "yes", l->has_pim, لبھیا);
#جے_انت
			أرجع subs;
		    }
		}

		// Do not add the state again when it exists with the same
		// positions.
		إذا (has_state_with_pos(l, state, subs, pim))
		    اذهب skip_جوڑو;
	    }

	    // When there are backreferences or PIMs the number of states may
	    // be (a lot) bigger than anticipated.
	    إذا (l->n == l->len)
	    {
		صحيح		newlen = l->len * 3 / 2 + 50;
		size_t		newsize = newlen * حجم(nfa_thread_T);
		nfa_thread_T	*newt;

		إذا ((لما)(newsize >> 10) >= p_mmp)
		{
		    emsg(_(e_نمونہ_ورتدا_ہور_حافظہ_than_maxmempattern));
		    أرجع عدم;
		}
		إذا (subs != &عارضی_subs)
		{
		    // "subs" may point into the current array, need to make a
		    // copy before it becomes invalid.
		    copy_sub(&عارضی_subs.norm, &subs->norm);
#جے_تعریف_شدہ FEAT_SYN_HL
		    إذا (rex.nfa_has_zsubexpr)
			copy_sub(&عارضی_subs.synt, &subs->synt);
#جے_انت
		    subs = &عارضی_subs;
		}

		newt = vim_realloc(l->t, newsize);
		إذا (newt == عدم)
		{
		    // out of memory
		    أرجع عدم;
		}
		l->t = newt;
		l->len = newlen;
	    }

	    // add the state to the list
	    state->lastlist[nfa_ll_اشاریہ] = l->id;
	    thread = &l->t[l->n++];
	    thread->state = state;
	    إذا (pim == عدم)
		thread->pim.نتیجہ = NFA_PIM_بن_ورتیا;
	    وإلا
	    {
		copy_pim(&thread->pim, pim);
		l->has_pim = TRUE;
	    }
	    copy_sub(&thread->subs.norm, &subs->norm);
#جے_تعریف_شدہ FEAT_SYN_HL
	    إذا (rex.nfa_has_zsubexpr)
		copy_sub(&thread->subs.synt, &subs->synt);
#جے_انت
#جے_تعریف_شدہ ENABLE_LOG
	    report_state("Adding", &thread->subs.norm, state, l->id, pim);
	    did_print = TRUE;
#جے_انت
    }

#جے_تعریف_شدہ ENABLE_LOG
    إذا (!did_print)
	report_state("Processing", &subs->norm, state, l->id, pim);
#جے_انت
    بدل (state->c)
    {
	حالة NFA_میل:
	    اكسر;

	حالة NFA_ونڈو:
	    // order matters here
	    subs = addstate(l, state->باہرلا, subs, pim, off_arg, ڈونگھائی + 1);
	    subs = addstate(l, state->out1, subs, pim, off_arg, ڈونگھائی + 1);
	    اكسر;

	حالة NFA_خالی_تھاواں:
	حالة NFA_NOPEN:
	حالة NFA_NCLOSE:
	    subs = addstate(l, state->باہرلا, subs, pim, off_arg, ڈونگھائی + 1);
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
#جے_تعریف_شدہ FEAT_SYN_HL
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
#جے_انت
	حالة NFA_ZSTART:
	    إذا (state->c == NFA_ZSTART)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#جے_تعریف_شدہ FEAT_SYN_HL
	    وإلا إذا (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
	    {
		subidx = state->c - NFA_ZOPEN;
		sub = &subs->synt;
	    }
#جے_انت
	    وإلا
	    {
		subidx = state->c - NFA_MOPEN;
		sub = &subs->norm;
	    }

	    // avoid compiler warnings
	    save_ptr = عدم;
	    CLEAR_میدان(save_multipos);

	    // Set the position (with "off" added) in the subexpression.  Save
	    // and restore it when it was in use.  Otherwise fill any gap.
	    إذا (REG_MULTI)
	    {
		إذا (subidx < sub->وچ_use)
		{
		    save_multipos = sub->list.multi[subidx];
		    save_وچ_use = -1;
		}
		وإلا
		{
		    save_وچ_use = sub->وچ_use;
		    لكل (i = sub->وچ_use; i < subidx; ++i)
		    {
			sub->list.multi[i].شروع_lnum = -1;
			sub->list.multi[i].سرا_lnum = -1;
		    }
		    sub->وچ_use = subidx + 1;
		}
		إذا (off == -1)
		{
		    sub->list.multi[subidx].شروع_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].شروع_خانہ = 0;
		}
		وإلا
		{
		    sub->list.multi[subidx].شروع_lnum = rex.lnum;
		    sub->list.multi[subidx].شروع_خانہ =
					 (colnr_T)(rex.داخلہ - rex.سطر + off);
		}
		sub->list.multi[subidx].سرا_lnum = -1;
	    }
	    وإلا
	    {
		إذا (subidx < sub->وچ_use)
		{
		    save_ptr = sub->list.سطر[subidx].شروع;
		    save_وچ_use = -1;
		}
		وإلا
		{
		    save_وچ_use = sub->وچ_use;
		    لكل (i = sub->وچ_use; i < subidx; ++i)
		    {
			sub->list.سطر[i].شروع = عدم;
			sub->list.سطر[i].سرا = عدم;
		    }
		    sub->وچ_use = subidx + 1;
		}
		sub->list.سطر[subidx].شروع = rex.داخلہ + off;
	    }

	    subs = addstate(l, state->باہرلا, subs, pim, off_arg, ڈونگھائی + 1);
	    إذا (subs == عدم)
		اكسر;
	    // "subs" may have changed, need to set "sub" again
#جے_تعریف_شدہ FEAT_SYN_HL
	    إذا (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
		sub = &subs->synt;
	    وإلا
#جے_انت
		sub = &subs->norm;

	    إذا (save_وچ_use == -1)
	    {
		إذا (REG_MULTI)
		    sub->list.multi[subidx] = save_multipos;
		وإلا
		    sub->list.سطر[subidx].شروع = save_ptr;
	    }
	    وإلا
		sub->وچ_use = save_وچ_use;
	    اكسر;

	حالة NFA_MCLOSE:
	    إذا (rex.nfa_has_zend && (REG_MULTI
			? subs->norm.list.multi[0].سرا_lnum >= 0
			: subs->norm.list.سطر[0].سرا != عدم))
	    {
		// Do not overwrite the position set by \ze.
		subs = addstate(l, state->باہرلا, subs, pim, off_arg, ڈونگھائی + 1);
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
#جے_تعریف_شدہ FEAT_SYN_HL
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
#جے_انت
	حالة NFA_ZEND:
	    إذا (state->c == NFA_ZEND)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#جے_تعریف_شدہ FEAT_SYN_HL
	    وإلا إذا (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
	    {
		subidx = state->c - NFA_ZCLOSE;
		sub = &subs->synt;
	    }
#جے_انت
	    وإلا
	    {
		subidx = state->c - NFA_MCLOSE;
		sub = &subs->norm;
	    }

	    // We don't fill in gaps here, there must have been an MOPEN that
	    // has done that.
	    save_وچ_use = sub->وچ_use;
	    إذا (sub->وچ_use <= subidx)
		sub->وچ_use = subidx + 1;
	    إذا (REG_MULTI)
	    {
		save_multipos = sub->list.multi[subidx];
		إذا (off == -1)
		{
		    sub->list.multi[subidx].سرا_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].سرا_خانہ = 0;
		}
		وإلا
		{
		    sub->list.multi[subidx].سرا_lnum = rex.lnum;
		    sub->list.multi[subidx].سرا_خانہ =
					  (colnr_T)(rex.داخلہ - rex.سطر + off);
		}
		// avoid compiler warnings
		save_ptr = عدم;
	    }
	    وإلا
	    {
		save_ptr = sub->list.سطر[subidx].سرا;
		sub->list.سطر[subidx].سرا = rex.داخلہ + off;
		// avoid compiler warnings
		CLEAR_میدان(save_multipos);
	    }

	    subs = addstate(l, state->باہرلا, subs, pim, off_arg, ڈونگھائی + 1);
	    إذا (subs == عدم)
		اكسر;
	    // "subs" may have changed, need to set "sub" again
#جے_تعریف_شدہ FEAT_SYN_HL
	    إذا (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
		sub = &subs->synt;
	    وإلا
#جے_انت
		sub = &subs->norm;

	    إذا (REG_MULTI)
		sub->list.multi[subidx] = save_multipos;
	    وإلا
		sub->list.سطر[subidx].سرا = save_ptr;
	    sub->وچ_use = save_وچ_use;
	    اكسر;
    }
    أرجع subs;
}

/*
 * Like addstate(), but the new state(s) are put at position "*ip".
 * Used for zero-width matches, next state to use is the added one.
 * This makes sure the order of states to be tried does not change, which
 * matters for alternatives.
 */
    ساكن regsubs_T *
addstate_ایتھے(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim,   // postponed look-behind match
    صحيح			*ip)
{
    صحيح tlen = l->n;
    صحيح گنتی;
    صحيح listidx = *ip;
    regsubs_T *r;

    // First add the state(s) at the end, so that we know how many there are.
    // Pass the listidx as offset (avoids adding another argument to
    // addstate()).
    r = addstate(l, state, subs, pim, -listidx - ADDSTATE_ایتھے_وتھ, 0);
    إذا (r == عدم)
	أرجع عدم;

    // when "*ip" was at the end of the list, nothing to do
    إذا (listidx + 1 == tlen)
	أرجع r;

    // re-order to put the new state at the current position
    گنتی = l->n - tlen;
    إذا (گنتی == 0)
	أرجع r; // no state got added
    إذا (گنتی == 1)
    {
	// overwrite the current state
	l->t[listidx] = l->t[l->n - 1];
    }
    وإلا إذا (گنتی > 1)
    {
	إذا (l->n + گنتی - 1 >= l->len)
	{
	    // not enough space to move the new states, reallocate the list
	    // and move the states to the right position
	    صحيح		    newlen = l->len * 3 / 2 + 50;
	    size_t	    newsize = newlen * حجم(nfa_thread_T);
	    nfa_thread_T    *newl;

	    إذا ((لما)(newsize >> 10) >= p_mmp)
	    {
		emsg(_(e_نمونہ_ورتدا_ہور_حافظہ_than_maxmempattern));
		أرجع عدم;
	    }
	    newl = alloc(newsize);
	    إذا (newl == عدم)
		أرجع عدم;
	    l->len = newlen;
	    mch_memmove(&(newl[0]),
		    &(l->t[0]),
		    حجم(nfa_thread_T) * listidx);
	    mch_memmove(&(newl[listidx]),
		    &(l->t[l->n - گنتی]),
		    حجم(nfa_thread_T) * گنتی);
	    mch_memmove(&(newl[listidx + گنتی]),
		    &(l->t[listidx + 1]),
		    حجم(nfa_thread_T) * (l->n - گنتی - listidx - 1));
	    vim_free(l->t);
	    l->t = newl;
	}
	وإلا
	{
	    // make space for new states, then move them from the
	    // end to the current position
	    mch_memmove(&(l->t[listidx + گنتی]),
		    &(l->t[listidx + 1]),
		    حجم(nfa_thread_T) * (l->n - listidx - 1));
	    mch_memmove(&(l->t[listidx]),
		    &(l->t[l->n - 1]),
		    حجم(nfa_thread_T) * گنتی);
	}
    }
    --l->n;
    *ip = listidx - 1;

    أرجع r;
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
	    إذا (c >= 1 && c < 128 && حرف_یا_ہندسہ(c))
		أرجع OK;
	    اكسر;
	حالة NFA_CLASS_ALPHA:
	    إذا (c >= 1 && c < 128 && حرف_اے(c))
		أرجع OK;
	    اكسر;
	حالة NFA_CLASS_BLANK:
	    إذا (c == ' ' || c == '\t')
		أرجع OK;
	    اكسر;
	حالة NFA_CLASS_CNTRL:
	    إذا (c >= 1 && c <= 127 && iscntrl(c))
		أرجع OK;
	    اكسر;
	حالة NFA_CLASS_DIGIT:
	    إذا (VIM_ISDIGIT(c))
		أرجع OK;
	    اكسر;
	حالة NFA_CLASS_گراف:
	    إذا (c >= 1 && c <= 127 && isgraph(c))
		أرجع OK;
	    اكسر;
	حالة NFA_CLASS_ہیٹھلا:
	    إذا (MB_ISLOWER(c) && c != 170 && c != 186)
		أرجع OK;
	    اكسر;
	حالة NFA_CLASS_PRINT:
	    إذا (vim_isprintc(c))
		أرجع OK;
	    اكسر;
	حالة NFA_CLASS_PUNCT:
	    إذا (c >= 1 && c < 128 && ispunct(c))
		أرجع OK;
	    اكسر;
	حالة NFA_CLASS_SPACE:
	    إذا ((c >= 9 && c <= 13) || (c == ' '))
		أرجع OK;
	    اكسر;
	حالة NFA_CLASS_اتلا:
	    إذا (MB_ISUPPER(c))
		أرجع OK;
	    اكسر;
	حالة NFA_CLASS_XDIGIT:
	    إذا (vim_isxdigit(c))
		أرجع OK;
	    اكسر;
	حالة NFA_CLASS_TAB:
	    إذا (c == '\t')
		أرجع OK;
	    اكسر;
	حالة NFA_CLASS_RETURN:
	    إذا (c == '\r')
		أرجع OK;
	    اكسر;
	حالة NFA_CLASS_BACKSPACE:
	    إذا (c == '\b')
		أرجع OK;
	    اكسر;
	حالة NFA_CLASS_ESCAPE:
	    إذا (c == '\033')
		أرجع OK;
	    اكسر;
	حالة NFA_CLASS_IDENT:
	    إذا (vim_isIDc(c))
		أرجع OK;
	    اكسر;
	حالة NFA_CLASS_KEYWORD:
	    إذا (reg_iswordc(c))
		أرجع OK;
	    اكسر;
	حالة NFA_CLASS_FNAME:
	    إذا (vim_isfilec(c))
		أرجع OK;
	    اكسر;

	افتراضي:
	    // should not be here :P
	    siemsg(_(e_nfa_regexp_invalid_character_class_nr), class);
	    أرجع FAIL;
    }
    أرجع FAIL;
}

/*
 * Check for a match with subexpression "subidx".
 * Return TRUE if it matches.
 */
    ساكن صحيح
میل_backref(
    regsub_T	*sub,	    // pointers to subexpressions
    صحيح		subidx,
    صحيح		*bytelen)   // out: length of match in bytes
{
    صحيح		len;

    إذا (sub->وچ_use <= subidx)
    {
retempty:
	// backref was not set, match an empty string
	*bytelen = 0;
	أرجع TRUE;
    }

    إذا (REG_MULTI)
    {
	إذا (sub->list.multi[subidx].شروع_lnum < 0
				       || sub->list.multi[subidx].سرا_lnum < 0)
	    اذهب retempty;
	إذا (sub->list.multi[subidx].شروع_lnum == rex.lnum
			       && sub->list.multi[subidx].سرا_lnum == rex.lnum)
	{
	    len = sub->list.multi[subidx].سرا_خانہ
					  - sub->list.multi[subidx].شروع_خانہ;
	    إذا (cstrncmp(rex.سطر + sub->list.multi[subidx].شروع_خانہ,
							 rex.داخلہ, &len) == 0)
	    {
		*bytelen = len;
		أرجع TRUE;
	    }
	}
	وإلا
	{
	    إذا (میل_with_backref(
			sub->list.multi[subidx].شروع_lnum,
			sub->list.multi[subidx].شروع_خانہ,
			sub->list.multi[subidx].سرا_lnum,
			sub->list.multi[subidx].سرا_خانہ,
			bytelen) == RA_میل)
		أرجع TRUE;
	}
    }
    وإلا
    {
	إذا (sub->list.سطر[subidx].شروع == عدم
					|| sub->list.سطر[subidx].سرا == عدم)
	    اذهب retempty;
	len = (صحيح)(sub->list.سطر[subidx].سرا - sub->list.سطر[subidx].شروع);
	إذا (cstrncmp(sub->list.سطر[subidx].شروع, rex.داخلہ, &len) == 0)
	{
	    *bytelen = len;
	    أرجع TRUE;
	}
    }
    أرجع FALSE;
}

#جے_تعریف_شدہ FEAT_SYN_HL

/*
 * Check for a match with \z subexpression "subidx".
 * Return TRUE if it matches.
 */
    ساكن صحيح
میل_zref(
    صحيح		subidx,
    صحيح		*bytelen)   // out: length of match in bytes
{
    صحيح		len;

    cleanup_zsubexpr();
    إذا (re_extmatch_وچ == عدم || re_extmatch_وچ->matches[subidx] == عدم)
    {
	// backref was not set, match an empty string
	*bytelen = 0;
	أرجع TRUE;
    }

    len = (صحيح)STRLEN(re_extmatch_وچ->matches[subidx]);
    إذا (cstrncmp(re_extmatch_وچ->matches[subidx], rex.داخلہ, &len) == 0)
    {
	*bytelen = len;
	أرجع TRUE;
    }
    أرجع FALSE;
}
#جے_انت

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
    لكل (i = prog->nstate; --i >= 0; )
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
    لكل (i = prog->nstate; --i >= 0; )
    {
	p->lastlist[1] = list[i];
	++p;
    }
}

    ساكن صحيح
nfa_re_عدد_cmp(long_u val, صحيح op, long_u pos)
{
    إذا (op == 1) أرجع pos > val;
    إذا (op == 2) أرجع pos < val;
    أرجع val == pos;
}

ساكن صحيح nfa_regmatch(nfa_regprog_T *prog, nfa_state_T *شروع, regsubs_T *submatch, regsubs_T *m);

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
    صحيح		save_reginput_خانہ = (صحيح)(rex.داخلہ - rex.سطر);
    صحيح		save_reglnum = rex.lnum;
    صحيح		save_nfa_میل = nfa_میل;
    صحيح		save_nfa_listid = rex.nfa_listid;
    save_se_T   *save_nfa_endp = nfa_endp;
    save_se_T   endpos;
    save_se_T   *endposp = عدم;
    صحيح		نتیجہ;
    صحيح		need_restore = FALSE;

    إذا (pim != عدم)
    {
	// start at the position where the postponed match was
	إذا (REG_MULTI)
	    rex.داخلہ = rex.سطر + pim->سرا.pos.خانہ;
	وإلا
	    rex.داخلہ = pim->سرا.ptr;
    }

    إذا (state->c == NFA_شروع_INVISIBLE_BEFORE
	    || state->c == NFA_شروع_INVISIBLE_BEFORE_پہلا
	    || state->c == NFA_شروع_INVISIBLE_BEFORE_NEG
	    || state->c == NFA_شروع_INVISIBLE_BEFORE_NEG_پہلا)
    {
	// The recursive match must end at the current position. When "pim" is
	// not NULL it specifies the current position.
	endposp = &endpos;
	إذا (REG_MULTI)
	{
	    إذا (pim == عدم)
	    {
		endpos.se_u.pos.خانہ = (صحيح)(rex.داخلہ - rex.سطر);
		endpos.se_u.pos.lnum = rex.lnum;
	    }
	    وإلا
		endpos.se_u.pos = pim->سرا.pos;
	}
	وإلا
	{
	    إذا (pim == عدم)
		endpos.se_u.ptr = rex.داخلہ;
	    وإلا
		endpos.se_u.ptr = pim->سرا.ptr;
	}

	// Go back the specified number of bytes, or as far as the
	// start of the previous line, to try matching "\@<=" or
	// not matching "\@<!". This is very inefficient, limit the number of
	// bytes if possible.
	إذا (state->val <= 0)
	{
	    إذا (REG_MULTI)
	    {
		rex.سطر = reg_getline(--rex.lnum);
		إذا (rex.سطر == عدم)
		    // can't go before the first line
		    rex.سطر = reg_getline(++rex.lnum);
	    }
	    rex.داخلہ = rex.سطر;
	}
	وإلا
	{
	    إذا (REG_MULTI && (صحيح)(rex.داخلہ - rex.سطر) < state->val)
	    {
		// Not enough bytes in this line, go to end of
		// previous line.
		rex.سطر = reg_getline(--rex.lnum);
		إذا (rex.سطر == عدم)
		{
		    // can't go before the first line
		    rex.سطر = reg_getline(++rex.lnum);
		    rex.داخلہ = rex.سطر;
		}
		وإلا
		    rex.داخلہ = rex.سطر + reg_getline_len(rex.lnum);
	    }
	    إذا ((صحيح)(rex.داخلہ - rex.سطر) >= state->val)
	    {
		rex.داخلہ -= state->val;
		إذا (has_mbyte)
		    rex.داخلہ -= mb_سر_off(rex.سطر, rex.داخلہ);
	    }
	    وإلا
		rex.داخلہ = rex.سطر;
	}
    }

#جے_تعریف_شدہ ENABLE_LOG
    إذا (log_fd != stderr)
	fclose(log_fd);
    log_fd = عدم;
#جے_انت
    // Have to clear the lastlist field of the NFA nodes, so that
    // nfa_regmatch() and addstate() can run properly after recursion.
    إذا (nfa_ll_اشاریہ == 1)
    {
	// Already calling nfa_regmatch() recursively.  Save the lastlist[1]
	// values and clear them.
	إذا (*listids == عدم || *listids_len < prog->nstate)
	{
	    vim_free(*listids);
	    *listids = ALLOC_MULT(صحيح, prog->nstate);
	    إذا (*listids == عدم)
	    {
		emsg(_(e_nfa_regexp_could_not_allocate_حافظہ_for_branch_traversal));
		أرجع 0;
	    }
	    *listids_len = prog->nstate;
	}
	nfa_save_listids(prog, *listids);
	need_restore = TRUE;
	// any value of rex.nfa_listid will do
    }
    وإلا
    {
	// First recursive nfa_regmatch() call, switch to the second lastlist
	// entry.  Make sure rex.nfa_listid is different from a previous
	// recursive call, because some states may still have this ID.
	++nfa_ll_اشاریہ;
	إذا (rex.nfa_listid <= rex.nfa_alt_listid)
	    rex.nfa_listid = rex.nfa_alt_listid;
    }

    // Call nfa_regmatch() to check if the current concat matches at this
    // position. The concat ends with the node NFA_END_INVISIBLE
    nfa_endp = endposp;
    نتیجہ = nfa_regmatch(prog, state->باہرلا, submatch, m);

    إذا (need_restore)
	nfa_restore_listids(prog, *listids);
    وإلا
    {
	--nfa_ll_اشاریہ;
	rex.nfa_alt_listid = rex.nfa_listid;
    }

    // restore position in input text
    rex.lnum = save_reglnum;
    إذا (REG_MULTI)
	rex.سطر = reg_getline(rex.lnum);
    rex.داخلہ = rex.سطر + save_reginput_خانہ;
    إذا (نتیجہ != NFA_TOO_EXPENSIVE)
    {
	nfa_میل = save_nfa_میل;
	rex.nfa_listid = save_nfa_listid;
    }
    nfa_endp = save_nfa_endp;

#جے_تعریف_شدہ ENABLE_LOG
    open_debug_log(نتیجہ);
#جے_انت

    أرجع نتیجہ;
}

/*
 * Estimate the chance of a match with "state" failing.
 * empty match: 0
 * NFA_ANY: 1
 * specific character: 99
 */
    ساكن صحيح
failure_chance(nfa_state_T *state, صحيح ڈونگھائی)
{
    صحيح c = state->c;
    صحيح l, r;

    // detect looping
    إذا (ڈونگھائی > 4)
	أرجع 1;

    بدل (c)
    {
	حالة NFA_ونڈو:
	    إذا (state->باہرلا->c == NFA_ونڈو || state->out1->c == NFA_ونڈو)
		// avoid recursive stuff
		أرجع 1;
	    // two alternatives, use the lowest failure chance
	    l = failure_chance(state->باہرلا, ڈونگھائی + 1);
	    r = failure_chance(state->out1, ڈونگھائی + 1);
	    أرجع l < r ? l : r;

	حالة NFA_ANY:
	    // matches anything, unlikely to fail
	    أرجع 1;

	حالة NFA_میل:
	حالة NFA_MCLOSE:
	حالة NFA_ANY_COMPOSING:
	    // empty match works always
	    أرجع 0;

	حالة NFA_شروع_INVISIBLE:
	حالة NFA_شروع_INVISIBLE_پہلا:
	حالة NFA_شروع_INVISIBLE_NEG:
	حالة NFA_شروع_INVISIBLE_NEG_پہلا:
	حالة NFA_شروع_INVISIBLE_BEFORE:
	حالة NFA_شروع_INVISIBLE_BEFORE_پہلا:
	حالة NFA_شروع_INVISIBLE_BEFORE_NEG:
	حالة NFA_شروع_INVISIBLE_BEFORE_NEG_پہلا:
	حالة NFA_شروع_نمونہ:
	    // recursive regmatch is expensive, use low failure chance
	    أرجع 5;

	حالة NFA_BOL:
	حالة NFA_EOL:
	حالة NFA_BOF:
	حالة NFA_EOF:
	حالة NFA_NEWL:
	    أرجع 99;

	حالة NFA_BOW:
	حالة NFA_EOW:
	    أرجع 90;

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
#جے_تعریف_شدہ FEAT_SYN_HL
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
#جے_انت
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
	    أرجع failure_chance(state->باہرلا, ڈونگھائی + 1);

	حالة NFA_BACKREF1:
	حالة NFA_BACKREF2:
	حالة NFA_BACKREF3:
	حالة NFA_BACKREF4:
	حالة NFA_BACKREF5:
	حالة NFA_BACKREF6:
	حالة NFA_BACKREF7:
	حالة NFA_BACKREF8:
	حالة NFA_BACKREF9:
#جے_تعریف_شدہ FEAT_SYN_HL
	حالة NFA_ZREF1:
	حالة NFA_ZREF2:
	حالة NFA_ZREF3:
	حالة NFA_ZREF4:
	حالة NFA_ZREF5:
	حالة NFA_ZREF6:
	حالة NFA_ZREF7:
	حالة NFA_ZREF8:
	حالة NFA_ZREF9:
#جے_انت
	    // backreferences don't match in many places
	    أرجع 94;

	حالة NFA_LNUM_GT:
	حالة NFA_LNUM_LT:
	حالة NFA_خانہ_GT:
	حالة NFA_خانہ_LT:
	حالة NFA_VCOL_GT:
	حالة NFA_VCOL_LT:
	حالة NFA_MARK_GT:
	حالة NFA_MARK_LT:
	حالة NFA_VISUAL:
	    // before/after positions don't match very often
	    أرجع 85;

	حالة NFA_LNUM:
	    أرجع 90;

	حالة NFA_CURSOR:
	حالة NFA_خانہ:
	حالة NFA_VCOL:
	حالة NFA_MARK:
	    // specific positions rarely match
	    أرجع 98;

	حالة NFA_COMPOSING:
	    أرجع 95;

	افتراضي:
	    إذا (c > 0)
		// character match fails often
		أرجع 95;
    }

    // something else, includes character classes
    أرجع 50;
}

/*
 * Skip until the char "c" we know a match must start with.
 */
    ساكن صحيح
skip_منزل_شروع(صحيح c, colnr_T *colp)
{
    char_u *s;

    // Used often, do some work to avoid call overhead.
    إذا (!rex.reg_ic && !has_mbyte)
	s = vim_strbyte(rex.سطر + *colp, c);
    وإلا
	s = cstrchr(rex.سطر + *colp, c);
    إذا (s == عدم)
	أرجع FAIL;
    *colp = (صحيح)(s - rex.سطر);
    أرجع OK;
}

/*
 * Check for a match with match_text.
 * Called after skip_to_start() has found regstart.
 * Returns zero for no match, 1 for a match.
 */
    ساكن لما
لبھو_میل_متن(colnr_T *startcol, صحيح regstart, char_u *میل_متن)
{
    colnr_T خانہ = *startcol;
    صحيح	    c1, c2;
    صحيح	    len1, len2;
    صحيح	    میل;

    لكل (;;)
    {
	میل = TRUE;
	// Skip the input character: case-folding can change its byte length.
	len2 = MB_CPTR2LEN(rex.سطر + خانہ);
	لكل (len1 = 0; میل_متن[len1] != NUL; len1 += MB_CHAR2LEN(c1))
	{
	    c1 = PTR2CHAR(میل_متن + len1);
	    c2 = PTR2CHAR(rex.سطر + خانہ + len2);
	    إذا (c1 != c2 && (!rex.reg_ic || MB_CASEFOLD(c1) != MB_CASEFOLD(c2)))
	    {
		میل = FALSE;
		اكسر;
	    }
	    len2 += enc_utf8 ? utf_ptr2len(rex.سطر + خانہ + len2)
							     : MB_CHAR2LEN(c2);
	}
	إذا (میل
		// check that no composing char follows
		&& !(enc_utf8
			  && utf_iscomposing(PTR2CHAR(rex.سطر + خانہ + len2))))
	{
	    cleanup_subexpr();
	    إذا (REG_MULTI)
	    {
		rex.reg_startpos[0].lnum = rex.lnum;
		rex.reg_startpos[0].خانہ = خانہ;
		rex.reg_endpos[0].lnum = rex.lnum;
		rex.reg_endpos[0].خانہ = خانہ + len2;
	    }
	    وإلا
	    {
		rex.reg_startp[0] = rex.سطر + خانہ;
		rex.reg_endp[0] = rex.سطر + خانہ + len2;
	    }
	    *startcol = خانہ;
	    أرجع 1L;
	}

	// Try finding regstart after the current match.
	خانہ += MB_CPTR2LEN(rex.سطر + خانہ); // skip the input character
	إذا (skip_منزل_شروع(regstart, &خانہ) == FAIL)
	    اكسر;
    }

    *startcol = خانہ;
    أرجع 0L;
}

/*
 * Check whether the composing characters at the current input position match
 * the NFA composing sub-expression that starts at "sta" (the first state
 * after NFA_COMPOSING).  "curc" is the base character and "clen" its byte
 * length.
 * Returns OK when it matches, FAIL otherwise.
 */
    ساكن صحيح
میل_composing(nfa_state_T *sta, صحيح curc, صحيح clen)
{
    صحيح		mc = curc;
    صحيح		len = 0;
    صحيح		cchars[MAX_MCO];
    صحيح		ccount = 0;
    صحيح		j;

    إذا (utf_iscomposing(sta->c))
	// Only match composing character(s), ignore base character.  Used
	// for ".{composing}" and "{composing}" (no preceding character).
	len += mb_char2len(mc);

    إذا (rex.reg_icombine && len == 0)
	// If \Z was present, then ignore composing characters.  When
	// ignoring the base character this always matches.
	أرجع sta->c == curc ? OK : FAIL;

    // Check base character matches first, unless ignored.
    إذا (len == 0 && mc != sta->c)
	أرجع FAIL;

    إذا (len == 0)
    {
	len += mb_char2len(mc);
	sta = sta->باہرلا;
    }

    // We don't care about the order of composing characters.  Get them into
    // cchars[] first.
    طالما (len < clen)
    {
	mc = mb_ptr2char(rex.داخلہ + len);
	cchars[ccount++] = mc;
	len += mb_char2len(mc);
	إذا (ccount == MAX_MCO)
	    اكسر;
    }

    // Check that each composing char in the pattern matches a composing char
    // in the text.  We do not check if all composing chars are matched.
    طالما (sta->c != NFA_سرا_COMPOSING)
    {
	لكل (j = 0; j < ccount; ++j)
	    إذا (cchars[j] == sta->c)
		اكسر;
	إذا (j == ccount)
	    أرجع FAIL;
	sta = sta->باہرلا;
    }

    أرجع OK;
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
    nfa_state_T		*شروع,
    regsubs_T		*submatch,
    regsubs_T		*m)
{
    صحيح		نتیجہ = FALSE;
    size_t	ناپ = 0;
    صحيح		flag = 0;
    صحيح		go_منزل_nextline = FALSE;
    nfa_thread_T *t;
    nfa_list_T	list[2];
    صحيح		listidx;
    nfa_list_T	*thislist;
    nfa_list_T	*nextlist;
    صحيح		*listids = عدم;
    صحيح		listids_len = 0;
    nfa_state_T *جوڑو_state;
    صحيح		جوڑو_ایتھے;
    صحيح		جوڑو_گنتی;
    صحيح		جوڑو_off = 0;
    صحيح		toplevel = شروع->c == NFA_MOPEN;
    regsubs_T	*r;
#جے_تعریف_شدہ NFA_REGEXP_DEBUG_LOG
    FILE	*debug;
#جے_انت

    // Some patterns may take a long time to match, especially when using
    // recursive_regmatch(). Allow interrupting them with CTRL-C.
    fast_breakcheck();
    إذا (got_int)
	أرجع FALSE;
#جے_تعریف_شدہ FEAT_RELTIME
    إذا (nfa_did_ویلا_باہرلا())
	أرجع FALSE;
#جے_انت

#جے_تعریف_شدہ NFA_REGEXP_DEBUG_LOG
    debug = fopen(NFA_REGEXP_DEBUG_LOG, "a");
    إذا (debug == عدم)
    {
	semsg("(NFA) COULD NOT OPEN %s!", NFA_REGEXP_DEBUG_LOG);
	أرجع FALSE;
    }
#جے_انت
    nfa_میل = FALSE;

    // Allocate memory for the lists of nodes.
    ناپ = (prog->nstate + 1) * حجم(nfa_thread_T);

    // Reuse cached list buffers from prog when available (top-level call).
    // Recursive calls must allocate their own buffers.
    إذا (toplevel && prog->listbuf[0] != عدم)
    {
	list[0].t = (nfa_thread_T *)prog->listbuf[0];
	list[1].t = (nfa_thread_T *)prog->listbuf[1];
	prog->listbuf[0] = عدم;
	prog->listbuf[1] = عدم;
    }
    وإلا
    {
	list[0].t = alloc(ناپ);
	list[1].t = alloc(ناپ);
    }
    list[0].len = prog->nstate + 1;
    list[1].len = prog->nstate + 1;
    إذا (list[0].t == عدم || list[1].t == عدم)
	اذهب theend;

#جے_تعریف_شدہ ENABLE_LOG
    log_fd = fopen(NFA_REGEXP_لگاتار_LOG, "a");
    إذا (log_fd == عدم)
    {
	emsg(_(e_log_open_failed));
	log_fd = stderr;
    }
    fprintf(log_fd, "**********************************\n");
    nfa_سیٹ_کوڈ(شروع->c);
    fprintf(log_fd, " RUNNING nfa_regmatch() starting with state %d, code %s\n",
    مطلق_قدر(شروع->id), کوڈ);
    fprintf(log_fd, "**********************************\n");
#جے_انت

    thislist = &list[0];
    thislist->n = 0;
    thislist->has_pim = FALSE;
    nextlist = &list[1];
    nextlist->n = 0;
    nextlist->has_pim = FALSE;
#جے_تعریف_شدہ ENABLE_LOG
    fprintf(log_fd, "(---) STARTSTATE first\n");
#جے_انت
    thislist->id = rex.nfa_listid + 1;

    // Inline optimized code for addstate(thislist, start, m, 0) if we know
    // it's the first MOPEN.
    إذا (toplevel)
    {
	إذا (REG_MULTI)
	{
	    m->norm.list.multi[0].شروع_lnum = rex.lnum;
	    m->norm.list.multi[0].شروع_خانہ = (colnr_T)(rex.داخلہ - rex.سطر);
	    m->norm.orig_شروع_خانہ = m->norm.list.multi[0].شروع_خانہ;
	}
	وإلا
	    m->norm.list.سطر[0].شروع = rex.داخلہ;
	m->norm.وچ_use = 1;
	r = addstate(thislist, شروع->باہرلا, m, عدم, 0, 0);
    }
    وإلا
	r = addstate(thislist, شروع, m, عدم, 0, 0);
    إذا (r == عدم)
    {
	nfa_میل = NFA_TOO_EXPENSIVE;
	اذهب theend;
    }

#تعریف_کرو	جوڑو_STATE_IF_میل(state)	\
    إذا (نتیجہ)				\
    {					\
	جوڑو_state = state->باہرلا;		\
	جوڑو_off = clen;			\
    }

    /*
     * Run for each character.
     */
    لكل (;;)
    {
	صحيح	curc;
	صحيح	clen;

	إذا (has_mbyte)
	{
	    // Fast path for an ASCII byte not followed by a composing
	    // character, matching the check in utfc_ptr2len().  Avoids two
	    // indirect calls for the common case.
	    إذا (rex.داخلہ[0] != NUL && rex.داخلہ[0] < 0x80 && rex.داخلہ[1] < 0x80)
	    {
		curc = rex.داخلہ[0];
		clen = 1;
	    }
	    وإلا
	    {
		curc = (*mb_ptr2char)(rex.داخلہ);
		clen = (*mb_ptr2len)(rex.داخلہ);
	    }
	}
	وإلا
	{
	    curc = *rex.داخلہ;
	    clen = 1;
	}
	إذا (curc == NUL)
	{
	    clen = 0;
	    go_منزل_nextline = FALSE;
	}

	// swap lists
	thislist = &list[flag];
	nextlist = &list[flag ^= 1];
	nextlist->n = 0;	    // clear nextlist
	nextlist->has_pim = FALSE;
	++rex.nfa_listid;
	إذا (prog->re_engine == AUTOMATIC_ENGINE
		&& (rex.nfa_listid >= NFA_MAX_حالتاں
#جے_تعریف_شدہ FEAT_EVAL
		    || nfa_fail_for_testing
#جے_انت
		    ))
	{
	    // too many states, retry with old engine
	    nfa_میل = NFA_TOO_EXPENSIVE;
	    اذهب theend;
	}

	thislist->id = rex.nfa_listid;
	nextlist->id = rex.nfa_listid + 1;

#جے_تعریف_شدہ ENABLE_LOG
	fprintf(log_fd, "------------------------------------------\n");
	fprintf(log_fd, ">>> Reginput is \"%s\"\n", rex.داخلہ);
	fprintf(log_fd, ">>> Advanced one character... Current char is %c (code %d) \n", curc, (صحيح)curc);
	fprintf(log_fd, ">>> Thislist has %d states available: ", thislist->n);
	{
	    صحيح i;

	    لكل (i = 0; i < thislist->n; i++)
		fprintf(log_fd, "%d  ", مطلق_قدر(thislist->t[i].state->id));
	}
	fprintf(log_fd, "\n");
#جے_انت

#جے_تعریف_شدہ NFA_REGEXP_DEBUG_LOG
	fprintf(debug, "\n-------------------\n");
#جے_انت
	/*
	 * If the state lists are empty we can stop.
	 */
	إذا (thislist->n == 0)
	    اكسر;

	// compute nextlist
	لكل (listidx = 0; listidx < thislist->n; ++listidx)
	{
	    // If the list gets very long there probably is something wrong.
	    // At least allow interrupting with CTRL-C.  Only check once in a
	    // while, calling ui_breakcheck() for every state is too slow.
	    ساكن صحيح	breakcheck_گنتی = 0;  // using "static" makes it faster
	    إذا (unlikely(++breakcheck_گنتی >= 100000))
	    {
		ui_breakcheck();
		breakcheck_گنتی = 0;
	    }
	    إذا (got_int)
		اكسر;
#جے_تعریف_شدہ FEAT_RELTIME
	    إذا (nfa_did_ویلا_باہرلا())
		اكسر;
#جے_انت
	    t = &thislist->t[listidx];

#جے_تعریف_شدہ NFA_REGEXP_DEBUG_LOG
	    nfa_سیٹ_کوڈ(t->state->c);
	    fprintf(debug, "%s, ", کوڈ);
#جے_انت
#جے_تعریف_شدہ ENABLE_LOG
	    {
		صحيح خانہ;

		إذا (t->subs.norm.وچ_use <= 0)
		    خانہ = -1;
		وإلا إذا (REG_MULTI)
		    خانہ = t->subs.norm.list.multi[0].شروع_خانہ;
		وإلا
		    خانہ = (صحيح)(t->subs.norm.list.سطر[0].شروع - rex.سطر);
		nfa_سیٹ_کوڈ(t->state->c);
		fprintf(log_fd, "(%d) char %d %s (start col %d)%s... \n",
			مطلق_قدر(t->state->id), (صحيح)t->state->c, کوڈ, خانہ,
			pim_info(&t->pim));
	    }
#جے_انت

	    /*
	     * Handle the possible codes of the current state.
	     * The most important is NFA_MATCH.
	     */
	    جوڑو_state = عدم;
	    جوڑو_ایتھے = FALSE;
	    جوڑو_گنتی = 0;
	    بدل (t->state->c)
	    {
	    حالة NFA_میل:
	      {
		// If the match is not at the start of the line, ends before a
		// composing characters and rex.reg_icombine is not set, that
		// is not really a match.
		إذا (enc_utf8 && !rex.reg_icombine
			     && rex.داخلہ != rex.سطر && utf_iscomposing(curc))
		    اكسر;

		nfa_میل = TRUE;
		copy_sub(&submatch->norm, &t->subs.norm);
#جے_تعریف_شدہ FEAT_SYN_HL
		إذا (rex.nfa_has_zsubexpr)
		    copy_sub(&submatch->synt, &t->subs.synt);
#جے_انت
#جے_تعریف_شدہ ENABLE_LOG
		log_subsexpr(&t->subs);
#جے_انت
		// Found the left-most longest match, do not look at any other
		// states at this position.  When the list of states is going
		// to be empty quit without advancing, so that "rex.input" is
		// correct.
		إذا (nextlist->n == 0)
		    clen = 0;
		اذهب nextchar;
	      }

	    حالة NFA_سرا_INVISIBLE:
	    حالة NFA_سرا_INVISIBLE_NEG:
	    حالة NFA_سرا_نمونہ:
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
#جے_تعریف_شدہ ENABLE_LOG
		إذا (nfa_endp != عدم)
		{
		    إذا (REG_MULTI)
			fprintf(log_fd, "Current lnum: %d, endp lnum: %d; current col: %d, endp col: %d\n",
				(صحيح)rex.lnum,
				(صحيح)nfa_endp->se_u.pos.lnum,
				(صحيح)(rex.داخلہ - rex.سطر),
				nfa_endp->se_u.pos.خانہ);
		    وإلا
			fprintf(log_fd, "Current col: %d, endp col: %d\n",
				(صحيح)(rex.داخلہ - rex.سطر),
				(صحيح)(nfa_endp->se_u.ptr - rex.داخلہ));
		}
#جے_انت
		// If "nfa_endp" is set it's only a match if it ends at
		// "nfa_endp"
		إذا (nfa_endp != عدم && (REG_MULTI
			? (rex.lnum != nfa_endp->se_u.pos.lnum
			    || (صحيح)(rex.داخلہ - rex.سطر)
						!= nfa_endp->se_u.pos.خانہ)
			: rex.داخلہ != nfa_endp->se_u.ptr))
		    اكسر;

		// do not set submatches for \@!
		إذا (t->state->c != NFA_سرا_INVISIBLE_NEG)
		{
		    copy_sub(&m->norm, &t->subs.norm);
#جے_تعریف_شدہ FEAT_SYN_HL
		    إذا (rex.nfa_has_zsubexpr)
			copy_sub(&m->synt, &t->subs.synt);
#جے_انت
		}
#جے_تعریف_شدہ ENABLE_LOG
		fprintf(log_fd, "Match found:\n");
		log_subsexpr(m);
#جے_انت
		nfa_میل = TRUE;
		// See comment above at "goto nextchar".
		إذا (nextlist->n == 0)
		    clen = 0;
		اذهب nextchar;

	    حالة NFA_شروع_INVISIBLE:
	    حالة NFA_شروع_INVISIBLE_پہلا:
	    حالة NFA_شروع_INVISIBLE_NEG:
	    حالة NFA_شروع_INVISIBLE_NEG_پہلا:
	    حالة NFA_شروع_INVISIBLE_BEFORE:
	    حالة NFA_شروع_INVISIBLE_BEFORE_پہلا:
	    حالة NFA_شروع_INVISIBLE_BEFORE_NEG:
	    حالة NFA_شروع_INVISIBLE_BEFORE_NEG_پہلا:
		{
#جے_تعریف_شدہ ENABLE_LOG
		    fprintf(log_fd, "Failure chance invisible: %d, what follows: %d\n",
			    failure_chance(t->state->باہرلا, 0),
			    failure_chance(t->state->out1->باہرلا, 0));
#جے_انت
		    // Do it directly if there already is a PIM or when
		    // nfa_postprocess() detected it will work better.
		    إذا (t->pim.نتیجہ != NFA_PIM_بن_ورتیا
			 || t->state->c == NFA_شروع_INVISIBLE_پہلا
			 || t->state->c == NFA_شروع_INVISIBLE_NEG_پہلا
			 || t->state->c == NFA_شروع_INVISIBLE_BEFORE_پہلا
			 || t->state->c == NFA_شروع_INVISIBLE_BEFORE_NEG_پہلا)
		    {
			صحيح وچ_use = m->norm.وچ_use;

			// Copy submatch info for the recursive call, opposite
			// of what happens on success below.
			copy_sub_off(&m->norm, &t->subs.norm);
#جے_تعریف_شدہ FEAT_SYN_HL
			إذا (rex.nfa_has_zsubexpr)
			    copy_sub_off(&m->synt, &t->subs.synt);
#جے_انت

			/*
			 * First try matching the invisible match, then what
			 * follows.
			 */
			نتیجہ = recursive_regmatch(t->state, عدم, prog,
					  submatch, m, &listids, &listids_len);
			إذا (نتیجہ == NFA_TOO_EXPENSIVE)
			{
			    nfa_میل = نتیجہ;
			    اذهب theend;
			}

			// for \@! and \@<! it is a match when the result is
			// FALSE
			إذا (نتیجہ != (t->state->c == NFA_شروع_INVISIBLE_NEG
			       || t->state->c == NFA_شروع_INVISIBLE_NEG_پہلا
			       || t->state->c
					   == NFA_شروع_INVISIBLE_BEFORE_NEG
			       || t->state->c
				     == NFA_شروع_INVISIBLE_BEFORE_NEG_پہلا))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&t->subs.norm, &m->norm);
#جے_تعریف_شدہ FEAT_SYN_HL
			    إذا (rex.nfa_has_zsubexpr)
				copy_sub_off(&t->subs.synt, &m->synt);
#جے_انت
			    // If the pattern has \ze and it matched in the
			    // sub pattern, use it.
			    copy_ze_off(&t->subs.norm, &m->norm);

			    // t->state->out1 is the corresponding
			    // END_INVISIBLE node; Add its out to the current
			    // list (zero-width match).
			    جوڑو_ایتھے = TRUE;
			    جوڑو_state = t->state->out1->باہرلا;
			}
			m->norm.وچ_use = وچ_use;
		    }
		    وإلا
		    {
			nfa_pim_T pim;

			/*
			 * First try matching what follows.  Only if a match
			 * is found verify the invisible match matches.  Add a
			 * nfa_pim_T to the following states, it contains info
			 * about the invisible match.
			 */
			pim.state = t->state;
			pim.نتیجہ = NFA_PIM_TODO;
			pim.subs.norm.وچ_use = 0;
#جے_تعریف_شدہ FEAT_SYN_HL
			pim.subs.synt.وچ_use = 0;
#جے_انت
			إذا (REG_MULTI)
			{
			    pim.سرا.pos.خانہ = (صحيح)(rex.داخلہ - rex.سطر);
			    pim.سرا.pos.lnum = rex.lnum;
			}
			وإلا
			    pim.سرا.ptr = rex.داخلہ;

			// t->state->out1 is the corresponding END_INVISIBLE
			// node; Add its out to the current list (zero-width
			// match).
			إذا (addstate_ایتھے(thislist, t->state->out1->باہرلا,
					     &t->subs, &pim, &listidx) == عدم)
			{
			    nfa_میل = NFA_TOO_EXPENSIVE;
			    اذهب theend;
			}
		    }
		}
		اكسر;

	    حالة NFA_شروع_نمونہ:
	      {
		nfa_state_T *skip = عدم;
#جے_تعریف_شدہ ENABLE_LOG
		صحيح	    skip_lid = 0;
#جے_انت

		// There is no point in trying to match the pattern if the
		// output state is not going to be added to the list.
		إذا (state_وچ_list(nextlist, t->state->out1->باہرلا, &t->subs))
		{
		    skip = t->state->out1->باہرلا;
#جے_تعریف_شدہ ENABLE_LOG
		    skip_lid = nextlist->id;
#جے_انت
		}
		وإلا إذا (state_وچ_list(nextlist,
					  t->state->out1->باہرلا->باہرلا, &t->subs))
		{
		    skip = t->state->out1->باہرلا->باہرلا;
#جے_تعریف_شدہ ENABLE_LOG
		    skip_lid = nextlist->id;
#جے_انت
		}
		وإلا إذا (state_وچ_list(thislist,
					  t->state->out1->باہرلا->باہرلا, &t->subs))
		{
		    skip = t->state->out1->باہرلا->باہرلا;
#جے_تعریف_شدہ ENABLE_LOG
		    skip_lid = thislist->id;
#جے_انت
		}
		إذا (skip != عدم)
		{
#جے_تعریف_شدہ ENABLE_LOG
		    nfa_سیٹ_کوڈ(skip->c);
		    fprintf(log_fd, "> Not trying to match pattern, output state %d is already in list %d. char %d: %s\n",
			    مطلق_قدر(skip->id), skip_lid, skip->c, کوڈ);
#جے_انت
		    اكسر;
		}
		// Copy submatch info to the recursive call, opposite of what
		// happens afterwards.
		copy_sub_off(&m->norm, &t->subs.norm);
#جے_تعریف_شدہ FEAT_SYN_HL
		إذا (rex.nfa_has_zsubexpr)
		    copy_sub_off(&m->synt, &t->subs.synt);
#جے_انت

		// First try matching the pattern.
		نتیجہ = recursive_regmatch(t->state, عدم, prog,
					  submatch, m, &listids, &listids_len);
		إذا (نتیجہ == NFA_TOO_EXPENSIVE)
		{
		    nfa_میل = نتیجہ;
		    اذهب theend;
		}
		إذا (نتیجہ)
		{
		    صحيح bytelen;

#جے_تعریف_شدہ ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN matches:\n");
		    log_subsexpr(m);
#جے_انت
		    // Copy submatch info from the recursive call
		    copy_sub_off(&t->subs.norm, &m->norm);
#جے_تعریف_شدہ FEAT_SYN_HL
		    إذا (rex.nfa_has_zsubexpr)
			copy_sub_off(&t->subs.synt, &m->synt);
#جے_انت
		    // Now we need to skip over the matched text and then
		    // continue with what follows.
		    إذا (REG_MULTI)
			// TODO: multi-line match
			bytelen = m->norm.list.multi[0].سرا_خانہ
						  - (صحيح)(rex.داخلہ - rex.سطر);
		    وإلا
			bytelen = (صحيح)(m->norm.list.سطر[0].سرا - rex.داخلہ);

#جے_تعریف_شدہ ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN length: %d\n", bytelen);
#جے_انت
		    إذا (bytelen == 0)
		    {
			// empty match, output of corresponding
			// NFA_END_PATTERN/NFA_SKIP to be used at current
			// position
			جوڑو_ایتھے = TRUE;
			جوڑو_state = t->state->out1->باہرلا->باہرلا;
		    }
		    وإلا إذا (bytelen <= clen)
		    {
			// match current character, output of corresponding
			// NFA_END_PATTERN to be used at next position.
			جوڑو_state = t->state->out1->باہرلا->باہرلا;
			جوڑو_off = clen;
		    }
		    وإلا
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			جوڑو_state = t->state->out1->باہرلا;
			جوڑو_off = bytelen;
			جوڑو_گنتی = bytelen - clen;
		    }
		}
		اكسر;
	      }

	    حالة NFA_BOL:
		إذا (rex.داخلہ == rex.سطر)
		{
		    جوڑو_ایتھے = TRUE;
		    جوڑو_state = t->state->باہرلا;
		}
		اكسر;

	    حالة NFA_EOL:
		إذا (curc == NUL)
		{
		    جوڑو_ایتھے = TRUE;
		    جوڑو_state = t->state->باہرلا;
		}
		اكسر;

	    حالة NFA_BOW:
		نتیجہ = TRUE;

		إذا (curc == NUL)
		    نتیجہ = FALSE;
		وإلا إذا (has_mbyte)
		{
		    صحيح this_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.داخلہ, rex.reg_buf);
		    إذا (this_class <= 1)
			نتیجہ = FALSE;
		    وإلا إذا (reg_prev_class() == this_class)
			نتیجہ = FALSE;
		}
		وإلا إذا (!vim_iswordc_buf(curc, rex.reg_buf)
			   || (rex.داخلہ > rex.سطر
			       && vim_iswordc_buf(rex.داخلہ[-1], rex.reg_buf)))
		    نتیجہ = FALSE;
		إذا (نتیجہ)
		{
		    جوڑو_ایتھے = TRUE;
		    جوڑو_state = t->state->باہرلا;
		}
		اكسر;

	    حالة NFA_EOW:
		نتیجہ = TRUE;
		إذا (rex.داخلہ == rex.سطر)
		    نتیجہ = FALSE;
		وإلا إذا (has_mbyte)
		{
		    صحيح this_class, prev_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.داخلہ, rex.reg_buf);
		    prev_class = reg_prev_class();
		    إذا (this_class == prev_class
					|| prev_class == 0 || prev_class == 1)
			نتیجہ = FALSE;
		}
		وإلا إذا (!vim_iswordc_buf(rex.داخلہ[-1], rex.reg_buf)
			|| (rex.داخلہ[0] != NUL
					&& vim_iswordc_buf(curc, rex.reg_buf)))
		    نتیجہ = FALSE;
		إذا (نتیجہ)
		{
		    جوڑو_ایتھے = TRUE;
		    جوڑو_state = t->state->باہرلا;
		}
		اكسر;

	    حالة NFA_BOF:
		إذا (rex.lnum == 0 && rex.داخلہ == rex.سطر
				     && (!REG_MULTI || rex.reg_firstlnum == 1))
		{
		    جوڑو_ایتھے = TRUE;
		    جوڑو_state = t->state->باہرلا;
		}
		اكسر;

	    حالة NFA_EOF:
		إذا (rex.lnum == rex.reg_maxline && curc == NUL)
		{
		    جوڑو_ایتھے = TRUE;
		    جوڑو_state = t->state->باہرلا;
		}
		اكسر;

	    حالة NFA_COMPOSING:
	    {
		nfa_state_T *سرا;

		نتیجہ = میل_composing(t->state->باہرلا, curc, clen);

		سرا = t->state->out1;	    // NFA_END_COMPOSING
		جوڑو_STATE_IF_میل(سرا);
		اكسر;
	    }

	    حالة NFA_NEWL:
		إذا (curc == NUL && !rex.reg_سطر_lbr && REG_MULTI
						&& rex.lnum <= rex.reg_maxline)
		{
		    go_منزل_nextline = TRUE;
		    // Pass -1 for the offset, which means taking the position
		    // at the start of the next line.
		    جوڑو_state = t->state->باہرلا;
		    جوڑو_off = -1;
		}
		وإلا إذا (curc == '\n' && rex.reg_سطر_lbr)
		{
		    // match \n as if it is an ordinary character
		    جوڑو_state = t->state->باہرلا;
		    جوڑو_off = 1;
		}
		اكسر;

	    حالة NFA_شروع_COLL:
	    حالة NFA_شروع_NEG_COLL:
	      {
		// What follows is a list of characters, until NFA_END_COLL.
		// One of them must match or none of them must match.
		nfa_state_T	*state;
		صحيح		نتیجہ_if_matched;
		صحيح		c1, c2;

		// Never match EOL. If it's part of the collection it is added
		// as a separate state with an OR.
		إذا (curc == NUL)
		    اكسر;

		state = t->state->باہرلا;
		نتیجہ_if_matched = (t->state->c == NFA_شروع_COLL);
		لكل (;;)
		{
		    إذا (state->c == NFA_COMPOSING)
		    {
			nfa_state_T *سرا;

			نتیجہ = میل_composing(t->state->باہرلا->باہرلا, curc,
									clen);

			إذا (t->state->باہرلا->out1 != عدم
				&& t->state->باہرلا->out1->c == NFA_سرا_COMPOSING)
			{
			    سرا = t->state->باہرلا->out1;
			    جوڑو_STATE_IF_میل(سرا);
			}
			اكسر;
		    }
		    إذا (state->c == NFA_سرا_COLL)
		    {
			نتیجہ = !نتیجہ_if_matched;
			اكسر;
		    }
		    إذا (state->c == NFA_RANGE_MIN)
		    {
			c1 = state->val;
			state = state->باہرلا; // advance to NFA_RANGE_MAX
			c2 = state->val;
#جے_تعریف_شدہ ENABLE_LOG
			fprintf(log_fd, "NFA_RANGE_MIN curc=%d c1=%d c2=%d\n",
				curc, c1, c2);
#جے_انت
			إذا (curc >= c1 && curc <= c2)
			{
			    نتیجہ = نتیجہ_if_matched;
			    اكسر;
			}
			إذا (rex.reg_ic)
			{
			    صحيح curc_کم = MB_CASEFOLD(curc);
			    صحيح پورا = FALSE;

			    لكل ( ; c1 <= c2; ++c1)
				إذا (MB_CASEFOLD(c1) == curc_کم)
				{
				    نتیجہ = نتیجہ_if_matched;
				    پورا = TRUE;
				    اكسر;
				}
			    إذا (پورا)
				اكسر;
			}
		    }
		    وإلا إذا (state->c < 0 ? check_char_class(state->c, curc)
			       : (curc == state->c
				   || (rex.reg_ic && MB_CASEFOLD(curc)
						    == MB_CASEFOLD(state->c))))
		    {
			نتیجہ = نتیجہ_if_matched;
			اكسر;
		    }
		    state = state->باہرلا;
		}
		إذا (نتیجہ)
		{
		    // next state is in out of the NFA_END_COLL, out1 of
		    // START points to the END state
		    جوڑو_state = t->state->out1->باہرلا;
		    جوڑو_off = clen;
		}
		اكسر;
	      }

	    حالة NFA_ANY:
		// Any char except '\0', (end of input) does not match.
		إذا (curc > 0)
		{
		    جوڑو_state = t->state->باہرلا;
		    جوڑو_off = clen;
		}
		اكسر;

	    حالة NFA_ANY_COMPOSING:
		// On a composing character skip over it.  Otherwise do
		// nothing.  Always matches.
		إذا (enc_utf8 && utf_iscomposing(curc))
		{
		    جوڑو_off = clen;
		}
		وإلا
		{
		    جوڑو_ایتھے = TRUE;
		    جوڑو_off = 0;
		}
		جوڑو_state = t->state->باہرلا;
		اكسر;

	    /*
	     * Character classes like \a for alpha, \d for digit etc.
	     */
	    حالة NFA_IDENT:	//  \i
		نتیجہ = vim_isIDc(curc);
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_SIDENT:	//  \I
		نتیجہ = !VIM_ISDIGIT(curc) && vim_isIDc(curc);
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_KWORD:	//  \k
		نتیجہ = vim_iswordp_buf(rex.داخلہ, rex.reg_buf);
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_SKWORD:	//  \K
		نتیجہ = !VIM_ISDIGIT(curc)
				     && vim_iswordp_buf(rex.داخلہ, rex.reg_buf);
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_FNAME:	//  \f
		نتیجہ = vim_isfilec(curc);
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_SFNAME:	//  \F
		نتیجہ = !VIM_ISDIGIT(curc) && vim_isfilec(curc);
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_PRINT:	//  \p
		نتیجہ = vim_isprintc(PTR2CHAR(rex.داخلہ));
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_SPRINT:	//  \P
		نتیجہ = !VIM_ISDIGIT(curc) && vim_isprintc(PTR2CHAR(rex.داخلہ));
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_WHITE:	//  \s
		نتیجہ = VIM_ISWHITE(curc);
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_NWHITE:	//  \S
		نتیجہ = curc != NUL && !VIM_ISWHITE(curc);
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_DIGIT:	//  \d
		نتیجہ = ri_digit(curc);
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_NDIGIT:	//  \D
		نتیجہ = curc != NUL && !ri_digit(curc);
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_HEX:	//  \x
		نتیجہ = ri_hex(curc);
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_NHEX:	//  \X
		نتیجہ = curc != NUL && !ri_hex(curc);
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_OCTAL:	//  \o
		نتیجہ = ri_octal(curc);
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_NOCTAL:	//  \O
		نتیجہ = curc != NUL && !ri_octal(curc);
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_لفظ:	//  \w
		نتیجہ = ri_لفظ(curc);
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_NWORD:	//  \W
		نتیجہ = curc != NUL && !ri_لفظ(curc);
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_سر:	//  \h
		نتیجہ = ri_سر(curc);
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_NHEAD:	//  \H
		نتیجہ = curc != NUL && !ri_سر(curc);
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_ALPHA:	//  \a
		نتیجہ = ri_alpha(curc);
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_NALPHA:	//  \A
		نتیجہ = curc != NUL && !ri_alpha(curc);
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_ہیٹھلا:	//  \l
		نتیجہ = ri_ہیٹھلا(curc);
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_NLOWER:	//  \L
		نتیجہ = curc != NUL && !ri_ہیٹھلا(curc);
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_اتلا:	//  \u
		نتیجہ = ri_اتلا(curc);
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_NUPPER:	// \U
		نتیجہ = curc != NUL && !ri_اتلا(curc);
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_ہیٹھلا_IC:	// [a-z]
		نتیجہ = ri_ہیٹھلا(curc) || (rex.reg_ic && ri_اتلا(curc));
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_NLOWER_IC:	// [^a-z]
		نتیجہ = curc != NUL
			&& !(ri_ہیٹھلا(curc) || (rex.reg_ic && ri_اتلا(curc)));
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_اتلا_IC:	// [A-Z]
		نتیجہ = ri_اتلا(curc) || (rex.reg_ic && ri_ہیٹھلا(curc));
		جوڑو_STATE_IF_میل(t->state);
		اكسر;

	    حالة NFA_NUPPER_IC:	// ^[A-Z]
		نتیجہ = curc != NUL
			&& !(ri_اتلا(curc) || (rex.reg_ic && ri_ہیٹھلا(curc)));
		جوڑو_STATE_IF_میل(t->state);
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
#جے_تعریف_شدہ FEAT_SYN_HL
	    حالة NFA_ZREF1:
	    حالة NFA_ZREF2:
	    حالة NFA_ZREF3:
	    حالة NFA_ZREF4:
	    حالة NFA_ZREF5:
	    حالة NFA_ZREF6:
	    حالة NFA_ZREF7:
	    حالة NFA_ZREF8:
	    حالة NFA_ZREF9:
#جے_انت
		// \1 .. \9  \z1 .. \z9
	      {
		صحيح subidx;
		صحيح bytelen;

#جے_تعریف_شدہ FEAT_SYN_HL
		إذا (t->state->c >= NFA_BACKREF1 && t->state->c <= NFA_BACKREF9)
#جے_انت
		{
		    subidx = t->state->c - NFA_BACKREF1 + 1;
		    نتیجہ = میل_backref(&t->subs.norm, subidx, &bytelen);
		}
#جے_تعریف_شدہ FEAT_SYN_HL
		وإلا
		{
		    subidx = t->state->c - NFA_ZREF1 + 1;
		    نتیجہ = میل_zref(subidx, &bytelen);
		}
#جے_انت

		إذا (نتیجہ)
		{
		    إذا (bytelen == 0)
		    {
			// empty match always works, output of NFA_SKIP to be
			// used next
			جوڑو_ایتھے = TRUE;
			جوڑو_state = t->state->باہرلا->باہرلا;
		    }
		    وإلا إذا (bytelen <= clen)
		    {
			// match current character, jump ahead to out of
			// NFA_SKIP
			جوڑو_state = t->state->باہرلا->باہرلا;
			جوڑو_off = clen;
		    }
		    وإلا
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			جوڑو_state = t->state->باہرلا;
			جوڑو_off = bytelen;
			جوڑو_گنتی = bytelen - clen;
		    }
		}
		اكسر;
	      }
	    حالة NFA_SKIP:
	      // character of previous matching \1 .. \9  or \@>
	      إذا (t->گنتی - clen <= 0)
	      {
		  // end of match, go to what follows
		  جوڑو_state = t->state->باہرلا;
		  جوڑو_off = clen;
	      }
	      وإلا
	      {
		  // add state again with decremented count
		  جوڑو_state = t->state;
		  جوڑو_off = 0;
		  جوڑو_گنتی = t->گنتی - clen;
	      }
	      اكسر;

	    حالة NFA_LNUM:
	    حالة NFA_LNUM_GT:
	    حالة NFA_LNUM_LT:
		نتیجہ = (REG_MULTI &&
			nfa_re_عدد_cmp(t->state->val, t->state->c - NFA_LNUM,
			    (long_u)(rex.lnum + rex.reg_firstlnum)));
		إذا (نتیجہ)
		{
		    جوڑو_ایتھے = TRUE;
		    جوڑو_state = t->state->باہرلا;
		}
		اكسر;

	    حالة NFA_خانہ:
	    حالة NFA_خانہ_GT:
	    حالة NFA_خانہ_LT:
		نتیجہ = nfa_re_عدد_cmp(t->state->val, t->state->c - NFA_خانہ,
			(long_u)(rex.داخلہ - rex.سطر) + 1);
		إذا (نتیجہ)
		{
		    جوڑو_ایتھے = TRUE;
		    جوڑو_state = t->state->باہرلا;
		}
		اكسر;

	    حالة NFA_VCOL:
	    حالة NFA_VCOL_GT:
	    حالة NFA_VCOL_LT:
		{
		    صحيح     op = t->state->c - NFA_VCOL;
		    colnr_T خانہ = (colnr_T)(rex.داخلہ - rex.سطر);
		    win_T   *wp = rex.reg_win == عدم ? curwin : rex.reg_win;

		    // Bail out quickly when there can't be a match, avoid the
		    // overhead of win_linetabsize() on long lines.
		    إذا (op != 1 && خانہ > t->state->val
			    * (has_mbyte ? MB_MAXBYTES : 1))
			اكسر;
		    نتیجہ = FALSE;
		    إذا (op == 1 && خانہ - 1 > t->state->val && خانہ > 100)
		    {
			صحيح ts = wp->w_بفر->b_p_ts;

			// Guess that a character won't use more columns than
			// 'tabstop', with a minimum of 4.
			إذا (ts < 4)
			    ts = 4;
			نتیجہ = خانہ > t->state->val * ts;
		    }
		    إذا (!نتیجہ)
		    {
			linenr_T    lnum = REG_MULTI
					    ? rex.reg_firstlnum + rex.lnum : 1;
			long_u	    vcol;

			إذا (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_بفر->b_ml.ml_سطر_گنتی))
			    lnum = 1;
			vcol = (long_u)win_linetabsize(wp, lnum, rex.سطر, خانہ);
			نتیجہ = nfa_re_عدد_cmp(t->state->val, op, vcol + 1);
		    }
		    إذا (نتیجہ)
		    {
			جوڑو_ایتھے = TRUE;
			جوڑو_state = t->state->باہرلا;
		    }
		}
		اكسر;

	    حالة NFA_MARK:
	    حالة NFA_MARK_GT:
	    حالة NFA_MARK_LT:
	      {
		pos_T	*pos;
		size_t	خانہ = REG_MULTI ? rex.داخلہ - rex.سطر : 0;

		pos = getmark_buf(rex.reg_buf, t->state->val, FALSE);

		// Line may have been freed, get it again.
		إذا (REG_MULTI)
		{
		    rex.سطر = reg_getline(rex.lnum);
		    rex.داخلہ = rex.سطر + خانہ;
		}

		// Compare the mark position to the match position, if the mark
		// exists and mark is set in reg_buf.
		إذا (pos != عدم && pos->lnum > 0)
		{
		    colnr_T pos_خانہ = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->خانہ == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->خانہ;

		    نتیجہ = (pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_خانہ == (colnr_T)(rex.داخلہ - rex.سطر)
				    ? t->state->c == NFA_MARK
				    : (pos_خانہ < (colnr_T)(rex.داخلہ - rex.سطر)
					? t->state->c == NFA_MARK_GT
					: t->state->c == NFA_MARK_LT))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? t->state->c == NFA_MARK_GT
				    : t->state->c == NFA_MARK_LT));
		    إذا (نتیجہ)
		    {
			جوڑو_ایتھے = TRUE;
			جوڑو_state = t->state->باہرلا;
		    }
		}
		اكسر;
	      }

	    حالة NFA_CURSOR:
		نتیجہ = (rex.reg_win != عدم
			&& (rex.lnum + rex.reg_firstlnum
						 == rex.reg_win->w_cursor.lnum)
			&& ((colnr_T)(rex.داخلہ - rex.سطر)
						== rex.reg_win->w_cursor.خانہ));
		إذا (نتیجہ)
		{
		    جوڑو_ایتھے = TRUE;
		    جوڑو_state = t->state->باہرلا;
		}
		اكسر;

	    حالة NFA_VISUAL:
		نتیجہ = reg_میل_visual();
		إذا (نتیجہ)
		{
		    جوڑو_ایتھے = TRUE;
		    جوڑو_state = t->state->باہرلا;
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
#جے_تعریف_شدہ FEAT_SYN_HL
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
#جے_انت
	    حالة NFA_NOPEN:
	    حالة NFA_ZSTART:
		// These states are only added to be able to bail out when
		// they are added again, nothing is to be done.
		اكسر;

	    افتراضي:	// regular character
	      {
		صحيح c = t->state->c;

#جے_تعریف_شدہ DEBUG
		إذا (c < 0)
		    siemsg("Negative state char: %ld", (لما)c);
#جے_انت
		نتیجہ = (c == curc);

		إذا (!نتیجہ && rex.reg_ic)
		    نتیجہ = MB_CASEFOLD(c) == MB_CASEFOLD(curc);
		// If rex.reg_icombine is not set only skip over the character
		// itself.  When it is set skip over composing characters.
		إذا (نتیجہ && enc_utf8 && !rex.reg_icombine)
		    clen = utf_ptr2len(rex.داخلہ);
		جوڑو_STATE_IF_میل(t->state);
		اكسر;
	      }

	    } // switch (t->state->c)

	    إذا (جوڑو_state != عدم)
	    {
		nfa_pim_T *pim;
		nfa_pim_T pim_copy;

		إذا (t->pim.نتیجہ == NFA_PIM_بن_ورتیا)
		    pim = عدم;
		وإلا
		    pim = &t->pim;

		// Handle the postponed invisible match if the match might end
		// without advancing and before the end of the line.
		إذا (pim != عدم && (clen == 0 || میل_follows(جوڑو_state, 0)))
		{
		    إذا (pim->نتیجہ == NFA_PIM_TODO)
		    {
#جے_تعریف_شدہ ENABLE_LOG
			fprintf(log_fd, "\n");
			fprintf(log_fd, "==================================\n");
			fprintf(log_fd, "Postponed recursive nfa_regmatch()\n");
			fprintf(log_fd, "\n");
#جے_انت
			نتیجہ = recursive_regmatch(pim->state, pim,
				    prog, submatch, m, &listids, &listids_len);
			pim->نتیجہ = نتیجہ ? NFA_PIM_میل : NFA_PIM_NOMATCH;
			// for \@! and \@<! it is a match when the result is
			// FALSE
			إذا (نتیجہ != (pim->state->c == NFA_شروع_INVISIBLE_NEG
			     || pim->state->c == NFA_شروع_INVISIBLE_NEG_پہلا
			     || pim->state->c
					   == NFA_شروع_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_شروع_INVISIBLE_BEFORE_NEG_پہلا))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&pim->subs.norm, &m->norm);
#جے_تعریف_شدہ FEAT_SYN_HL
			    إذا (rex.nfa_has_zsubexpr)
				copy_sub_off(&pim->subs.synt, &m->synt);
#جے_انت
			}
		    }
		    وإلا
		    {
			نتیجہ = (pim->نتیجہ == NFA_PIM_میل);
#جے_تعریف_شدہ ENABLE_LOG
			fprintf(log_fd, "\n");
			fprintf(log_fd, "Using previous recursive nfa_regmatch() result, result == %d\n", pim->نتیجہ);
			fprintf(log_fd, "MATCH = %s\n", نتیجہ == TRUE ? "OK" : "FALSE");
			fprintf(log_fd, "\n");
#جے_انت
		    }

		    // for \@! and \@<! it is a match when result is FALSE
		    إذا (نتیجہ != (pim->state->c == NFA_شروع_INVISIBLE_NEG
			     || pim->state->c == NFA_شروع_INVISIBLE_NEG_پہلا
			     || pim->state->c
					   == NFA_شروع_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_شروع_INVISIBLE_BEFORE_NEG_پہلا))
		    {
			// Copy submatch info from the recursive call
			copy_sub_off(&t->subs.norm, &pim->subs.norm);
#جے_تعریف_شدہ FEAT_SYN_HL
			إذا (rex.nfa_has_zsubexpr)
			    copy_sub_off(&t->subs.synt, &pim->subs.synt);
#جے_انت
		    }
		    وإلا
			// look-behind match failed, don't add the state
			استمر;

		    // Postponed invisible match was handled, don't add it to
		    // following states.
		    pim = عدم;
		}

		// If "pim" points into l->t it will become invalid when
		// adding the state causes the list to be reallocated.  Make a
		// local copy to avoid that.
		إذا (pim == &t->pim)
		{
		    copy_pim(&pim_copy, pim);
		    pim = &pim_copy;
		}

		إذا (جوڑو_ایتھے)
		    r = addstate_ایتھے(thislist, جوڑو_state, &t->subs,
								pim, &listidx);
		وإلا
		{
		    r = addstate(nextlist, جوڑو_state, &t->subs, pim, جوڑو_off, 0);
		    إذا (جوڑو_گنتی > 0)
			nextlist->t[nextlist->n - 1].گنتی = جوڑو_گنتی;
		}
		إذا (r == عدم)
		{
		    nfa_میل = NFA_TOO_EXPENSIVE;
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
	إذا (nfa_میل == FALSE
		&& ((toplevel
			&& rex.lnum == 0
			&& clen != 0
			&& (rex.reg_maxcol == 0
			  || (colnr_T)(rex.داخلہ - rex.سطر) < rex.reg_maxcol))
		    || (nfa_endp != عدم
			&& (REG_MULTI
			    ? (rex.lnum < nfa_endp->se_u.pos.lnum
			       || (rex.lnum == nfa_endp->se_u.pos.lnum
				   && (صحيح)(rex.داخلہ - rex.سطر)
						    < nfa_endp->se_u.pos.خانہ))
			    : rex.داخلہ < nfa_endp->se_u.ptr))))
	{
#جے_تعریف_شدہ ENABLE_LOG
	    fprintf(log_fd, "(---) STARTSTATE\n");
#جے_انت
	    // Inline optimized code for addstate() if we know the state is
	    // the first MOPEN.
	    إذا (toplevel)
	    {
		صحيح جوڑو = TRUE;
		صحيح c;

		إذا (prog->regstart != NUL && clen != 0)
		{
		    إذا (nextlist->n == 0)
		    {
			colnr_T خانہ = (colnr_T)(rex.داخلہ - rex.سطر) + clen;

			// Nextlist is empty, we can skip ahead to the
			// character that must appear at the start.
			إذا (skip_منزل_شروع(prog->regstart, &خانہ) == FAIL)
			    اكسر;
#جے_تعریف_شدہ ENABLE_LOG
			fprintf(log_fd, "  Skipping ahead %d bytes to regstart\n",
				خانہ - ((colnr_T)(rex.داخلہ - rex.سطر) + clen));
#جے_انت
			rex.داخلہ = rex.سطر + خانہ - clen;
		    }
		    وإلا
		    {
			// Checking if the required start character matches is
			// cheaper than adding a state that won't match.
			c = PTR2CHAR(rex.داخلہ + clen);
			إذا (c != prog->regstart && (!rex.reg_ic
			     || MB_CASEFOLD(c) != MB_CASEFOLD(prog->regstart)))
			{
#جے_تعریف_شدہ ENABLE_LOG
			    fprintf(log_fd, "  Skipping start state, regstart does not match\n");
#جے_انت
			    جوڑو = FALSE;
			}
		    }
		}

		إذا (جوڑو)
		{
		    إذا (REG_MULTI)
		    {
			m->norm.list.multi[0].شروع_خانہ =
					 (colnr_T)(rex.داخلہ - rex.سطر) + clen;
			m->norm.orig_شروع_خانہ =
					       m->norm.list.multi[0].شروع_خانہ;
		    }
		    وإلا
			m->norm.list.سطر[0].شروع = rex.داخلہ + clen;
		    إذا (addstate(nextlist, شروع->باہرلا, m, عدم, clen, 0) == عدم)
		    {
			nfa_میل = NFA_TOO_EXPENSIVE;
			اذهب theend;
		    }
		}
	    }
	    وإلا
	    {
		char_u	    *save_سطر = rex.سطر;
		char_u	    *save_داخلہ = rex.داخلہ;
		linenr_T    save_lnum = rex.lnum;

		// At the end of a line the match can only start on the next
		// line, use that position instead of the line break.
		إذا (REG_MULTI && clen == 0 && nfa_endp != عدم
					 && rex.lnum < nfa_endp->se_u.pos.lnum)
		{
		    char_u  *اگلا_سطر = reg_getline(rex.lnum + 1);

		    إذا (اگلا_سطر != عدم)
		    {
			rex.سطر = اگلا_سطر;
			rex.داخلہ = rex.سطر;
			++rex.lnum;
		    }
		}

		r = addstate(nextlist, شروع, m, عدم, clen, 0);

		rex.سطر = save_سطر;
		rex.داخلہ = save_داخلہ;
		rex.lnum = save_lnum;

		إذا (r == عدم)
		{
		    nfa_میل = NFA_TOO_EXPENSIVE;
		    اذهب theend;
		}
	    }
	}

#جے_تعریف_شدہ ENABLE_LOG
	fprintf(log_fd, ">>> Thislist had %d states available: ", thislist->n);
	{
	    صحيح i;

	    لكل (i = 0; i < thislist->n; i++)
		fprintf(log_fd, "%d  ", مطلق_قدر(thislist->t[i].state->id));
	}
	fprintf(log_fd, "\n");
#جے_انت

nextchar:
	// Advance to the next character, or advance to the next line, or
	// finish.
	إذا (clen != 0)
	    rex.داخلہ += clen;
	وإلا إذا (go_منزل_nextline || (nfa_endp != عدم && REG_MULTI
					&& rex.lnum < nfa_endp->se_u.pos.lnum))
	    reg_nextline();
	وإلا
	    اكسر;

	// Allow interrupting with CTRL-C.
	سطر_breakcheck();
	إذا (got_int)
	    اكسر;
#جے_تعریف_شدہ FEAT_RELTIME
	إذا (nfa_did_ویلا_باہرلا())
	    اكسر;
#جے_انت
    }

#جے_تعریف_شدہ ENABLE_LOG
    إذا (log_fd != stderr)
	fclose(log_fd);
    log_fd = عدم;
#جے_انت

theend:
    // Cache list buffers in prog for reuse, or free if prog already has
    // cached buffers (recursive call case).
    إذا (prog->listbuf[0] == عدم && list[0].t != عدم && list[1].t != عدم)
    {
	prog->listbuf[0] = list[0].t;
	prog->listbuf[1] = list[1].t;
    }
    وإلا
    {
	vim_free(list[0].t);
	vim_free(list[1].t);
    }
    vim_free(listids);
#تعریف_رد جوڑو_STATE_IF_میل
#جے_تعریف_شدہ NFA_REGEXP_DEBUG_LOG
    fclose(debug);
#جے_انت

    أرجع nfa_میل;
}

/*
 * Try match of "prog" with at rex.line["col"].
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    ساكن لما
nfa_regtry(
    nfa_regprog_T   *prog,
    colnr_T	    خانہ,
    صحيح		    *timed_باہرلا بن_ورتیا)	// flag set on timeout or NULL
{
    صحيح		i;
    regsubs_T	subs, m;
    nfa_state_T	*شروع = prog->شروع;
    صحيح		نتیجہ;
#جے_تعریف_شدہ ENABLE_LOG
    FILE	*f;
#جے_انت

    rex.داخلہ = rex.سطر + خانہ;
#جے_تعریف_شدہ FEAT_RELTIME
    nfa_timed_باہرلا = timed_باہرلا;
#جے_انت

#جے_تعریف_شدہ ENABLE_LOG
    f = fopen(NFA_REGEXP_لگاتار_LOG, "a");
    إذا (f != عدم)
    {
	fprintf(f, "\n\n\t=======================================================\n");
# ifdef DEBUG
	fprintf(f, "\tRegexp is \"%s\"\n", nfa_regengine.اظہاریہ);
# endif
	fprintf(f, "\tInput text is \"%s\" \n", rex.داخلہ);
	fprintf(f, "\t=======================================================\n\n");
	nfa_print_state(f, شروع);
	fprintf(f, "\n\n");
	fclose(f);
    }
    وإلا
	emsg("Could not open temporary log file for writing");
#جے_انت

    clear_sub(&subs.norm);
    clear_sub(&m.norm);
#جے_تعریف_شدہ FEAT_SYN_HL
    clear_sub(&subs.synt);
    clear_sub(&m.synt);
#جے_انت

    نتیجہ = nfa_regmatch(prog, شروع, &subs, &m);
    إذا (نتیجہ == FALSE)
	أرجع 0;
    وإلا إذا (نتیجہ == NFA_TOO_EXPENSIVE)
	أرجع نتیجہ;

    cleanup_subexpr();
    إذا (REG_MULTI)
    {
	لكل (i = 0; i < subs.norm.وچ_use; i++)
	{
	    rex.reg_startpos[i].lnum = subs.norm.list.multi[i].شروع_lnum;
	    rex.reg_startpos[i].خانہ = subs.norm.list.multi[i].شروع_خانہ;

	    rex.reg_endpos[i].lnum = subs.norm.list.multi[i].سرا_lnum;
	    rex.reg_endpos[i].خانہ = subs.norm.list.multi[i].سرا_خانہ;
	}
	إذا (rex.reg_mmatch != عدم)
	    rex.reg_mmatch->rmm_matchcol = subs.norm.orig_شروع_خانہ;

	إذا (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].خانہ = خانہ;
	}
	إذا (rex.reg_endpos[0].lnum < 0)
	{
	    // pattern has a \ze but it didn't match, use current end
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].خانہ = (صحيح)(rex.داخلہ - rex.سطر);
	}
	وإلا
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    وإلا
    {
	لكل (i = 0; i < subs.norm.وچ_use; i++)
	{
	    rex.reg_startp[i] = subs.norm.list.سطر[i].شروع;
	    rex.reg_endp[i] = subs.norm.list.سطر[i].سرا;
	}

	إذا (rex.reg_startp[0] == عدم)
	    rex.reg_startp[0] = rex.سطر + خانہ;
	إذا (rex.reg_endp[0] == عدم)
	    rex.reg_endp[0] = rex.داخلہ;
    }

#جے_تعریف_شدہ FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_باہرلا);
    re_extmatch_باہرلا = عدم;

    إذا (prog->reghasz == REX_سیٹ)
    {
	cleanup_zsubexpr();
	re_extmatch_باہرلا = make_extmatch();
	إذا (re_extmatch_باہرلا == عدم)
	    أرجع 0;
	// Loop over \z1, \z2, etc.  There is no \z0.
	لكل (i = 1; i < subs.synt.وچ_use; i++)
	{
	    إذا (REG_MULTI)
	    {
		بنية multipos *mpos = &subs.synt.list.multi[i];

		// Only accept single line matches that are valid.
		إذا (mpos->شروع_lnum >= 0
			&& mpos->شروع_lnum == mpos->سرا_lnum
			&& mpos->سرا_خانہ >= mpos->شروع_خانہ)
		    re_extmatch_باہرلا->matches[i] =
			vim_strnsave(reg_getline(mpos->شروع_lnum)
							    + mpos->شروع_خانہ,
					     mpos->سرا_خانہ - mpos->شروع_خانہ);
	    }
	    وإلا
	    {
		بنية linepos *lpos = &subs.synt.list.سطر[i];

		إذا (lpos->شروع != عدم && lpos->سرا != عدم)
		    re_extmatch_باہرلا->matches[i] =
			    vim_strnsave(lpos->شروع, lpos->سرا - lpos->شروع);
	    }
	}
    }
#جے_انت

    أرجع 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 *
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    ساكن لما
nfa_regexec_both(
    char_u	*سطر,
    colnr_T	startcol,	// column to start looking for match
    صحيح		*timed_باہرلا)	// flag set on timeout or NULL
{
    nfa_regprog_T   *prog;
    لما	    retval = 0L;
    صحيح		    i;
    colnr_T	    خانہ = startcol;

    إذا (REG_MULTI)
    {
	prog = (nfa_regprog_T *)rex.reg_mmatch->regprog;
	سطر = reg_getline((linenr_T)0);    // relative to the cursor
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    وإلا
    {
	prog = (nfa_regprog_T *)rex.reg_میل->regprog;
	rex.reg_startp = rex.reg_میل->startp;
	rex.reg_endp = rex.reg_میل->endp;
    }

    // Be paranoid...
    إذا (prog == عدم || سطر == عدم)
    {
	iemsg(e_null_دلیل);
	اذهب theend;
    }

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    إذا (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    وإلا إذا (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    إذا (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    rex.سطر = سطر;
    rex.lnum = 0;    // relative to line

    rex.nfa_has_zend = prog->has_zend;
    rex.nfa_has_backref = prog->has_backref;
    rex.nfa_nsubexpr = prog->nsubexp;
    rex.nfa_listid = 1;
    rex.nfa_alt_listid = 2;
#جے_تعریف_شدہ DEBUG
    nfa_regengine.اظہاریہ = prog->نمونہ;
#جے_انت

    إذا (prog->reganch && خانہ > 0)
	أرجع 0L;

    rex.need_clear_subexpr = TRUE;
#جے_تعریف_شدہ FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    إذا (prog->reghasz == REX_سیٹ)
    {
	rex.nfa_has_zsubexpr = TRUE;
	rex.need_clear_zsubexpr = TRUE;
    }
    وإلا
    {
	rex.nfa_has_zsubexpr = FALSE;
	rex.need_clear_zsubexpr = FALSE;
    }
#جے_انت

    إذا (prog->regstart != NUL)
    {
	// Skip ahead until a character we know the match must start with.
	// When there is none there is no match.
	إذا (skip_منزل_شروع(prog->regstart, &خانہ) == FAIL)
	    أرجع 0L;

	// If match_text is set it contains the full text that must match.
	// Nothing else to try. Doesn't handle combining chars well.
	إذا (prog->میل_متن != عدم && *prog->میل_متن != NUL && !rex.reg_icombine)
	{
	    retval = لبھو_میل_متن(&خانہ, prog->regstart, prog->میل_متن);
	    إذا (REG_MULTI)
		rex.reg_mmatch->rmm_matchcol = خانہ;
	    وإلا
		rex.reg_میل->rm_matchcol = خانہ;
	    أرجع retval;
	}
    }

    // If the start column is past the maximum column: no need to try.
    إذا (rex.reg_maxcol > 0 && خانہ >= rex.reg_maxcol)
	اذهب theend;

    // Set the "nstate" used by nfa_regcomp() to zero to trigger an error when
    // it's accidentally used during execution.
    nstate = 0;
    لكل (i = 0; i < prog->nstate; ++i)
    {
	prog->state[i].id = i;
	prog->state[i].lastlist[0] = 0;
	prog->state[i].lastlist[1] = 0;
    }

    retval = nfa_regtry(prog, خانہ, timed_باہرلا);

#جے_تعریف_شدہ DEBUG
    nfa_regengine.اظہاریہ = عدم;
#جے_انت

theend:
    إذا (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs and
	// \ze are used.
	إذا (REG_MULTI)
	{
	    lpos_T *شروع = &rex.reg_mmatch->startpos[0];
	    lpos_T *سرا = &rex.reg_mmatch->endpos[0];

	    إذا (سرا->lnum < شروع->lnum
			|| (سرا->lnum == شروع->lnum && سرا->خانہ < شروع->خانہ))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];
	}
	وإلا
	{
	    إذا (rex.reg_میل->endp[0] < rex.reg_میل->startp[0])
		rex.reg_میل->endp[0] = rex.reg_میل->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_میل->rm_matchcol = خانہ;
	}
    }

    أرجع retval;
}

/*
 * Compile a regular expression into internal code for the NFA matcher.
 * Returns the program in allocated space.  Returns NULL for an error.
 */
    ساكن regprog_T *
nfa_regcomp(char_u *اظہاریہ, صحيح re_flags)
{
    nfa_regprog_T	*prog = عدم;
    size_t		prog_ناپ;
    صحيح			*postfix;

    إذا (اظہاریہ == عدم)
	أرجع عدم;

#جے_تعریف_شدہ DEBUG
    nfa_regengine.اظہاریہ = اظہاریہ;
#جے_انت
    nfa_re_flags = re_flags;

    init_class_tab();

    إذا (nfa_regcomp_شروع(اظہاریہ, re_flags) == FAIL)
	أرجع عدم;

    // Build postfix form of the regexp. Needed to build the NFA
    // (and count its size).
    postfix = re2post();
    إذا (postfix == عدم)
	اذهب fail;	    // Cascaded (syntax?) error

    /*
     * In order to build the NFA, we parse the input regexp twice:
     * 1. first pass to count size (so we can allocate space)
     * 2. second to emit code
     */
#جے_تعریف_شدہ ENABLE_LOG
    {
	FILE *f = fopen(NFA_REGEXP_لگاتار_LOG, "a");

	إذا (f != عدم)
	{
	    fprintf(f, "\n*****************************\n\n\n\n\tCompiling regexp \"%s\"... hold on !\n", اظہاریہ);
	    fclose(f);
	}
    }
#جے_انت

    /*
     * PASS 1
     * Count number of NFA states in "nstate". Do not build the NFA.
     */
    post2nfa(postfix, post_ptr, TRUE);

    // allocate the regprog with space for the compiled regexp
    prog_ناپ = offsetof(nfa_regprog_T, state) + حجم(nfa_state_T) * nstate;
    prog = alloc(prog_ناپ);
    إذا (prog == عدم)
	اذهب fail;
    state_ptr = prog->state;
    prog->re_وچ_use = FALSE;
    prog->listbuf[0] = عدم;
    prog->listbuf[1] = عدم;

    /*
     * PASS 2
     * Build the NFA
     */
    prog->شروع = post2nfa(postfix, post_ptr, FALSE);
    إذا (prog->شروع == عدم)
	اذهب fail;

    prog->regflags = regflags;
    prog->engine = &nfa_regengine;
    prog->nstate = nstate;
    prog->has_zend = rex.nfa_has_zend;
    prog->has_backref = rex.nfa_has_backref;
    prog->nsubexp = regnpar;

    nfa_postprocess(prog);

    prog->reganch = nfa_get_reganch(prog->شروع, 0);
    prog->regstart = nfa_get_regstart(prog->شروع, 0);
    prog->میل_متن = nfa_get_میل_متن(prog->شروع);

#جے_تعریف_شدہ ENABLE_LOG
    nfa_postfix_dump(اظہاریہ, OK);
    nfa_dump(prog);
#جے_انت
#جے_تعریف_شدہ FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    prog->reghasz = re_has_z;
#جے_انت
    prog->نمونہ = vim_strsave(اظہاریہ);
#جے_تعریف_شدہ DEBUG
    nfa_regengine.اظہاریہ = عدم;
#جے_انت

باہرلا:
    // The postfix list and fragment stack are reused by the next compilation
    // (and freed in free_regexp_stuff()), but drop ones that grew large for a
    // big pattern to avoid holding much memory.
    إذا (post_شروع_len > NFA_POSTFIX_KEEP)
    {
	VIM_CLEAR(post_شروع);
	post_شروع_len = 0;
    }
    post_ptr = post_سرا = عدم;
    إذا (nfa_ڈھیر_len > NFA_ڈھیر_KEEP)
    {
	VIM_CLEAR(nfa_ڈھیر);
	nfa_ڈھیر_len = 0;
    }
    state_ptr = عدم;
    أرجع (regprog_T *)prog;

fail:
    VIM_CLEAR(prog);
#جے_تعریف_شدہ ENABLE_LOG
    nfa_postfix_dump(اظہاریہ, FAIL);
#جے_انت
#جے_تعریف_شدہ DEBUG
    nfa_regengine.اظہاریہ = عدم;
#جے_انت
    اذهب باہرلا;
}

/*
 * Free a compiled regexp program, returned by nfa_regcomp().
 */
    ساكن فراغ
nfa_regfree(regprog_T *prog)
{
    إذا (prog == عدم)
	أرجع;

    vim_free(((nfa_regprog_T *)prog)->میل_متن);
    vim_free(((nfa_regprog_T *)prog)->نمونہ);
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
    char_u	*سطر,	// string to match against
    colnr_T	خانہ,	// column to start looking for match
    صحيح		سطر_lbr)
{
    rex.reg_میل = rmp;
    rex.reg_mmatch = عدم;
    rex.reg_maxline = 0;
    rex.reg_سطر_lbr = سطر_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = عدم;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;
    أرجع nfa_regexec_both(سطر, خانہ, عدم);
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
    ساكن لما
nfa_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	خانہ,		// column to start looking for match
    صحيح		*timed_باہرلا)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    أرجع nfa_regexec_both(عدم, خانہ, timed_باہرلا);
}

#جے_تعریف_شدہ DEBUG
# undef ENABLE_LOG
#جے_انت
