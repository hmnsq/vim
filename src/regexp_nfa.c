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
#నిర్వచితమైతే DEBUG
# define NFA_REGEXP_ERROR_LOG	"nfa_regexp_error.log"
# define ENABLE_LOG
# define NFA_REGEXP_DUMP_LOG	"nfa_regexp_dump.log"
# define NFA_REGEXP_నిరంతరం_LOG	"nfa_regexp_run.log"
# define NFA_REGEXP_DEBUG_LOG	"nfa_regexp_debug.log"
#అయితే_ముగింపు

// Added to NFA_ANY - NFA_NUPPER_IC to include a NL.
#నిర్వచించు NFA_కూడు_NL		31

గణన
{
    NFA_విభజించు = -1024,
    NFA_సరిపోలిక,
    NFA_ఖాళీలు,			    // matches 0-length

    NFA_ప్రారంభం_COLL,		    // [abc] start
    NFA_చివర_COLL,		    // [abc] end
    NFA_ప్రారంభం_NEG_COLL,		    // [^abc] start
    NFA_చివర_NEG_COLL,		    // [^abc] end (postfix only)
    NFA_RANGE,			    // range of the two previous items
				    // (postfix only)
    NFA_RANGE_MIN,		    // low end of a range
    NFA_RANGE_MAX,		    // high end of a range

    NFA_CONCAT,			    // concatenate two previous items (postfix
				    // only)
    NFA_OR,			    // \| (postfix only)
    NFA_నక్షత్రం,			    // greedy * (postfix only)
    NFA_నక్షత్రం_NONGREEDY,		    // non-greedy * (postfix only)
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
    NFA_ప్రారంభం_INVISIBLE,
    NFA_ప్రారంభం_INVISIBLE_మొదటి,
    NFA_ప్రారంభం_INVISIBLE_NEG,
    NFA_ప్రారంభం_INVISIBLE_NEG_మొదటి,
    NFA_ప్రారంభం_INVISIBLE_BEFORE,
    NFA_ప్రారంభం_INVISIBLE_BEFORE_మొదటి,
    NFA_ప్రారంభం_INVISIBLE_BEFORE_NEG,
    NFA_ప్రారంభం_INVISIBLE_BEFORE_NEG_మొదటి,
    NFA_ప్రారంభం_నమూనా,
    NFA_చివర_INVISIBLE,
    NFA_చివర_INVISIBLE_NEG,
    NFA_చివర_నమూనా,
    NFA_COMPOSING,		    // Next nodes in NFA are part of the
				    // composing multibyte char
    NFA_చివర_COMPOSING,		    // End of a composing char in the NFA
    NFA_ANY_COMPOSING,		    // \%C: Any composing characters.
    NFA_OPT_CHARS,		    // \%[abc]

    // The following are used only in the postfix form, not in the NFA
    NFA_PREV_ATOM_NO_వెడల్పు,	    // Used for \@=
    NFA_PREV_ATOM_NO_వెడల్పు_NEG,	    // Used for \@!
    NFA_PREV_ATOM_JUST_BEFORE,	    // Used for \@<=
    NFA_PREV_ATOM_JUST_BEFORE_NEG,  // Used for \@<!
    NFA_PREV_ATOM_LIKE_నమూనా,	    // Used for \@>

    NFA_BACKREF1,		    // \1
    NFA_BACKREF2,		    // \2
    NFA_BACKREF3,		    // \3
    NFA_BACKREF4,		    // \4
    NFA_BACKREF5,		    // \5
    NFA_BACKREF6,		    // \6
    NFA_BACKREF7,		    // \7
    NFA_BACKREF8,		    // \8
    NFA_BACKREF9,		    // \9
#నిర్వచితమైతే FEAT_SYN_HL
    NFA_ZREF1,			    // \z1
    NFA_ZREF2,			    // \z2
    NFA_ZREF3,			    // \z3
    NFA_ZREF4,			    // \z4
    NFA_ZREF5,			    // \z5
    NFA_ZREF6,			    // \z6
    NFA_ZREF7,			    // \z7
    NFA_ZREF8,			    // \z8
    NFA_ZREF9,			    // \z9
#అయితే_ముగింపు
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

#నిర్వచితమైతే FEAT_SYN_HL
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
#అయితే_ముగింపు

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
    NFA_మాట,		//	Match word char
    NFA_NWORD,		//	Match non-word char
    NFA_తల,		//	Match head char
    NFA_NHEAD,		//	Match non-head char
    NFA_ALPHA,		//	Match alpha char
    NFA_NALPHA,		//	Match non-alpha char
    NFA_క్రింది,		//	Match lowercase char
    NFA_NLOWER,		//	Match non-lowercase char
    NFA_పై,		//	Match uppercase char
    NFA_NUPPER,		//	Match non-uppercase char
    NFA_క్రింది_IC,	//	Match [a-z]
    NFA_NLOWER_IC,	//	Match [^a-z]
    NFA_పై_IC,	//	Match [A-Z]
    NFA_NUPPER_IC,	//	Match [^A-Z]

    NFA_మొదటి_NL = NFA_ANY + NFA_కూడు_NL,
    NFA_చివరి_NL = NFA_NUPPER_IC + NFA_కూడు_NL,

    NFA_CURSOR,		//	Match cursor pos
    NFA_LNUM,		//	Match line number
    NFA_LNUM_GT,	//	Match > line number
    NFA_LNUM_LT,	//	Match < line number
    NFA_నిలువు,		//	Match cursor column
    NFA_నిలువు_GT,		//	Match > cursor column
    NFA_నిలువు_LT,		//	Match < cursor column
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
    NFA_CLASS_లేఖాచిత్రం,
    NFA_CLASS_క్రింది,
    NFA_CLASS_PRINT,
    NFA_CLASS_PUNCT,
    NFA_CLASS_SPACE,
    NFA_CLASS_పై,
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
స్థిర స్థిరరాశి పూర్ణాంకం nfa_classcodes[] = {
    NFA_ANY, NFA_IDENT, NFA_SIDENT, NFA_KWORD,NFA_SKWORD,
    NFA_FNAME, NFA_SFNAME, NFA_PRINT, NFA_SPRINT,
    NFA_WHITE, NFA_NWHITE, NFA_DIGIT, NFA_NDIGIT,
    NFA_HEX, NFA_NHEX, NFA_OCTAL, NFA_NOCTAL,
    NFA_మాట, NFA_NWORD, NFA_తల, NFA_NHEAD,
    NFA_ALPHA, NFA_NALPHA, NFA_క్రింది, NFA_NLOWER,
    NFA_పై, NFA_NUPPER
};

// Variables only used in nfa_regcomp() and descendants.
స్థిర పూర్ణాంకం nfa_re_flags; // re_flags passed to nfa_regcomp()
స్థిర పూర్ణాంకం *post_ప్రారంభం;  // holds the postfix form of r.e.
స్థిర పూర్ణాంకం post_ప్రారంభం_len;  // size of allocated post_start (in ints)
స్థిర పూర్ణాంకం *post_చివర;
స్థిర పూర్ణాంకం *post_ptr;
స్థిర పూర్ణాంకం nfa_reg_parse_లోతు;	// nesting depth in nfa_reg()

// The postfix list (post_start) and fragment stack (nfa_stack) are reused
// across compilations; a buffer that grew past these sizes for a big pattern
// is freed afterwards instead of being kept, to avoid holding much memory.
#నిర్వచించు NFA_POSTFIX_KEEP	10000	// number of ints (~40 Kbyte)
#నిర్వచించు NFA_దొంతర_KEEP		4000	// number of Frag_T (~64 Kbyte)

// Set when the pattern should use the NFA engine.
// E.g. [[:upper:]] only allows 8bit characters for BT engine,
// while NFA engine handles multibyte characters correctly.
స్థిర పూర్ణాంకం కోరుతుంది_nfa;

స్థిర పూర్ణాంకం nstate;	// Number of states in the NFA.
స్థిర పూర్ణాంకం istate;	// Index in the state vector, used in alloc_state()

// If not NULL match must end at this position
స్థిర save_se_T *nfa_endp = ఖాళీ;

// 0 for first call to nfa_regmatch(), 1 for recursive call.
స్థిర పూర్ణాంకం nfa_ll_సూచీ = 0;

స్థిర పూర్ణాంకం realloc_post_list(శూన్య);
స్థిర పూర్ణాంకం nfa_reg(పూర్ణాంకం paren);
#నిర్వచితమైతే DEBUG
స్థిర శూన్య nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent);
#అయితే_ముగింపు
స్థిర పూర్ణాంకం సరిపోలిక_follows(nfa_state_T *startstate, పూర్ణాంకం లోతు);
స్థిర పూర్ణాంకం failure_chance(nfa_state_T *state, పూర్ణాంకం లోతు);

// helper functions used when doing re2post() ... regatom() parsing
#నిర్వచించు EMIT(c)	చేయి {				\
		    ఒకవేళ (post_ptr >= post_చివర && realloc_post_list() == FAIL) \
			తిరిగి FAIL;		\
		    *post_ptr++ = c;		\
		} అయితే (0)

/*
 * Initialize internal variables before NFA compilation.
 * Return OK on success, FAIL otherwise.
 */
    స్థిర పూర్ణాంకం
nfa_regcomp_ప్రారంభం(
    char_u	*సమాసం,
    పూర్ణాంకం		re_flags)	    // see vim_regcomp()
{
    size_t	postfix_కొలత;
    పూర్ణాంకం		nstate_max;

    nstate = 0;
    istate = 0;
    // A reasonable estimation for maximum size
    nstate_max = (పూర్ణాంకం)(STRLEN(సమాసం) + 1) * 25;

    // Some items blow up in size, such as [A-z].  Add more space for that.
    // When it is still not enough realloc_post_list() will be used.
    nstate_max += 1000;

    // Size for postfix representation of expr.
    postfix_కొలత = పరిమాణం(పూర్ణాంకం) * nstate_max;

    // Reuse the postfix buffer across compilations, only growing it when the
    // estimate exceeds the current size; it is freed in free_regexp_stuff().
    ఒకవేళ (post_ప్రారంభం == ఖాళీ || post_ప్రారంభం_len < nstate_max)
    {
	vim_free(post_ప్రారంభం);
	post_ప్రారంభం = alloc(postfix_కొలత);
	ఒకవేళ (post_ప్రారంభం == ఖాళీ)
	{
	    post_ప్రారంభం_len = 0;
	    తిరిగి FAIL;
	}
	post_ప్రారంభం_len = nstate_max;
    }
    post_ptr = post_ప్రారంభం;
    post_చివర = post_ప్రారంభం + post_ప్రారంభం_len;
    కోరుతుంది_nfa = FALSE;
    rex.nfa_has_zend = FALSE;
    rex.nfa_has_backref = FALSE;
    nfa_reg_parse_లోతు = 0;

    // shared with BT engine
    regcomp_ప్రారంభం(సమాసం, re_flags);

    తిరిగి OK;
}

/*
 * Figure out if the NFA state list starts with an anchor, must match at start
 * of the line.
 */
    స్థిర పూర్ణాంకం
nfa_get_reganch(nfa_state_T *ప్రారంభం, పూర్ణాంకం లోతు)
{
    nfa_state_T *p = ప్రారంభం;

    ఒకవేళ (లోతు > 4)
	తిరిగి 0;

    అయితే (p != ఖాళీ)
    {
	ఎంపిక (p->c)
	{
	    స్థితి NFA_BOL:
	    స్థితి NFA_BOF:
		తిరిగి 1; // yes!

	    స్థితి NFA_ZSTART:
	    స్థితి NFA_ZEND:
	    స్థితి NFA_CURSOR:
	    స్థితి NFA_VISUAL:

	    స్థితి NFA_MOPEN:
	    స్థితి NFA_MOPEN1:
	    స్థితి NFA_MOPEN2:
	    స్థితి NFA_MOPEN3:
	    స్థితి NFA_MOPEN4:
	    స్థితి NFA_MOPEN5:
	    స్థితి NFA_MOPEN6:
	    స్థితి NFA_MOPEN7:
	    స్థితి NFA_MOPEN8:
	    స్థితి NFA_MOPEN9:
	    స్థితి NFA_NOPEN:
#నిర్వచితమైతే FEAT_SYN_HL
	    స్థితి NFA_ZOPEN:
	    స్థితి NFA_ZOPEN1:
	    స్థితి NFA_ZOPEN2:
	    స్థితి NFA_ZOPEN3:
	    స్థితి NFA_ZOPEN4:
	    స్థితి NFA_ZOPEN5:
	    స్థితి NFA_ZOPEN6:
	    స్థితి NFA_ZOPEN7:
	    స్థితి NFA_ZOPEN8:
	    స్థితి NFA_ZOPEN9:
#అయితే_ముగింపు
		p = p->బహిర్గతం;
		ఆపు;

	    స్థితి NFA_విభజించు:
		తిరిగి nfa_get_reganch(p->బహిర్గతం, లోతు + 1)
				       && nfa_get_reganch(p->out1, లోతు + 1);

	    అప్రమేయ:
		తిరిగి 0; // noooo
	}
    }
    తిరిగి 0;
}

/*
 * Figure out if the NFA state list starts with a character which must match
 * at start of the match.
 */
    స్థిర పూర్ణాంకం
nfa_get_regstart(nfa_state_T *ప్రారంభం, పూర్ణాంకం లోతు)
{
    nfa_state_T *p = ప్రారంభం;

    ఒకవేళ (లోతు > 4)
	తిరిగి 0;

    అయితే (p != ఖాళీ)
    {
	ఎంపిక (p->c)
	{
	    // all kinds of zero-width matches
	    స్థితి NFA_BOL:
	    స్థితి NFA_BOF:
	    స్థితి NFA_BOW:
	    స్థితి NFA_EOW:
	    స్థితి NFA_ZSTART:
	    స్థితి NFA_ZEND:
	    స్థితి NFA_CURSOR:
	    స్థితి NFA_VISUAL:
	    స్థితి NFA_LNUM:
	    స్థితి NFA_LNUM_GT:
	    స్థితి NFA_LNUM_LT:
	    స్థితి NFA_నిలువు:
	    స్థితి NFA_నిలువు_GT:
	    స్థితి NFA_నిలువు_LT:
	    స్థితి NFA_VCOL:
	    స్థితి NFA_VCOL_GT:
	    స్థితి NFA_VCOL_LT:
	    స్థితి NFA_MARK:
	    స్థితి NFA_MARK_GT:
	    స్థితి NFA_MARK_LT:

	    స్థితి NFA_MOPEN:
	    స్థితి NFA_MOPEN1:
	    స్థితి NFA_MOPEN2:
	    స్థితి NFA_MOPEN3:
	    స్థితి NFA_MOPEN4:
	    స్థితి NFA_MOPEN5:
	    స్థితి NFA_MOPEN6:
	    స్థితి NFA_MOPEN7:
	    స్థితి NFA_MOPEN8:
	    స్థితి NFA_MOPEN9:
	    స్థితి NFA_NOPEN:
#నిర్వచితమైతే FEAT_SYN_HL
	    స్థితి NFA_ZOPEN:
	    స్థితి NFA_ZOPEN1:
	    స్థితి NFA_ZOPEN2:
	    స్థితి NFA_ZOPEN3:
	    స్థితి NFA_ZOPEN4:
	    స్థితి NFA_ZOPEN5:
	    స్థితి NFA_ZOPEN6:
	    స్థితి NFA_ZOPEN7:
	    స్థితి NFA_ZOPEN8:
	    స్థితి NFA_ZOPEN9:
#అయితే_ముగింపు
		p = p->బహిర్గతం;
		ఆపు;

	    స్థితి NFA_విభజించు:
	    {
		పూర్ణాంకం c1 = nfa_get_regstart(p->బహిర్గతం, లోతు + 1);
		పూర్ణాంకం c2 = nfa_get_regstart(p->out1, లోతు + 1);

		ఒకవేళ (c1 == c2)
		    తిరిగి c1; // yes!
		తిరిగి 0;
	    }

	    అప్రమేయ:
		ఒకవేళ (p->c > 0)
		    తిరిగి p->c; // yes!
		తిరిగి 0;
	}
    }
    తిరిగి 0;
}

/*
 * Figure out if the NFA state list contains just literal text and nothing
 * else.  If so return a string in allocated memory with what must match after
 * regstart.  Otherwise return NULL.
 */
    స్థిర char_u *
nfa_get_సరిపోలిక_పాఠ్యం(nfa_state_T *ప్రారంభం)
{
    nfa_state_T *p = ప్రారంభం;
    పూర్ణాంకం		len = 0;
    char_u	*ret;
    char_u	*s;

    ఒకవేళ (p->c != NFA_MOPEN)
	తిరిగి ఖాళీ; // just in case
    p = p->బహిర్గతం;
    అయితే (p->c > 0)
    {
	len += MB_CHAR2LEN(p->c);
	p = p->బహిర్గతం;
    }
    ఒకవేళ (p->c != NFA_MCLOSE || p->బహిర్గతం->c != NFA_సరిపోలిక)
	తిరిగి ఖాళీ;

    ret = alloc(len);
    ఒకవేళ (ret == ఖాళీ)
	తిరిగి ఖాళీ;

    p = ప్రారంభం->బహిర్గతం->బహిర్గతం; // skip first char, it goes into regstart
    s = ret;
    అయితే (p->c > 0)
    {
	ఒకవేళ (has_mbyte)
	    s += (*mb_char2bytes)(p->c, s);
	లేకపోతే
	    *s++ = p->c;
	p = p->బహిర్గతం;
    }
    *s = NUL;
    తిరిగి ret;
}

/*
 * Allocate more space for post_start.  Called when
 * running above the estimated number of states.
 */
    స్థిర పూర్ణాంకం
realloc_post_list(శూన్య)
{
    పూర్ణాంకం   nstate_max = (పూర్ణాంకం)(post_చివర - post_ప్రారంభం);
    పూర్ణాంకం   new_max;
    పూర్ణాంకం   *new_ప్రారంభం;
    పూర్ణాంకం	  *old_ప్రారంభం;

    // For weird patterns the number of states can be very high. Increasing by
    // 50% seems a reasonable compromise between memory use and speed.
    new_max = nstate_max * 3 / 2;
    new_ప్రారంభం = ALLOC_MULT(పూర్ణాంకం, new_max);
    ఒకవేళ (new_ప్రారంభం == ఖాళీ)
	తిరిగి FAIL;
    mch_memmove(new_ప్రారంభం, post_ప్రారంభం, nstate_max * పరిమాణం(పూర్ణాంకం));
    old_ప్రారంభం = post_ప్రారంభం;
    post_ప్రారంభం = new_ప్రారంభం;
    post_ప్రారంభం_len = new_max;
    post_ptr = new_ప్రారంభం + (post_ptr - old_ప్రారంభం);
    post_చివర = post_ప్రారంభం + new_max;
    vim_free(old_ప్రారంభం);
    తిరిగి OK;
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
    స్థిర పూర్ణాంకం
nfa_recognize_char_class(char_u *ప్రారంభం, char_u *చివర, పూర్ణాంకం extra_newl)
{
    పూర్ణాంకం		newl;
    పూర్ణాంకం		config = get_char_class_బిట్లు(ప్రారంభం, చివర, &newl);

    ఒకవేళ (config < 0)
	తిరిగి FAIL;

    ఒకవేళ (extra_newl == TRUE)
	newl = TRUE;

    ఒకవేళ (newl == TRUE)
	extra_newl = NFA_కూడు_NL;

    ఎంపిక (config)
    {
	స్థితి CLASS_o9:
	    తిరిగి extra_newl + NFA_DIGIT;
	స్థితి CLASS_not |  CLASS_o9:
	    తిరిగి extra_newl + NFA_NDIGIT;
	స్థితి CLASS_af | CLASS_AF | CLASS_o9:
	    తిరిగి extra_newl + NFA_HEX;
	స్థితి CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    తిరిగి extra_newl + NFA_NHEX;
	స్థితి CLASS_o7:
	    తిరిగి extra_newl + NFA_OCTAL;
	స్థితి CLASS_not | CLASS_o7:
	    తిరిగి extra_newl + NFA_NOCTAL;
	స్థితి CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    తిరిగి extra_newl + NFA_మాట;
	స్థితి CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    తిరిగి extra_newl + NFA_NWORD;
	స్థితి CLASS_az | CLASS_AZ | CLASS_underscore:
	    తిరిగి extra_newl + NFA_తల;
	స్థితి CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    తిరిగి extra_newl + NFA_NHEAD;
	స్థితి CLASS_az | CLASS_AZ:
	    తిరిగి extra_newl + NFA_ALPHA;
	స్థితి CLASS_not | CLASS_az | CLASS_AZ:
	    తిరిగి extra_newl + NFA_NALPHA;
	స్థితి CLASS_az:
	   తిరిగి extra_newl + NFA_క్రింది_IC;
	స్థితి CLASS_not | CLASS_az:
	    తిరిగి extra_newl + NFA_NLOWER_IC;
	స్థితి CLASS_AZ:
	    తిరిగి extra_newl + NFA_పై_IC;
	స్థితి CLASS_not | CLASS_AZ:
	    తిరిగి extra_newl + NFA_NUPPER_IC;
    }
    తిరిగి FAIL;
}

/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * Emits bytes in postfix notation: 'a,b,NFA_OR,c,NFA_OR' is
 * equivalent to 'a OR b OR c'
 *
 * NOTE! When changing this function, also update reg_equi_class()
 */
    స్థిర పూర్ణాంకం
nfa_emit_equi_class(పూర్ణాంకం c)
{
#నిర్వచించు EMIT2(c)    EMIT(c); EMIT(NFA_CONCAT);

    ఒకవేళ (enc_utf8 || STRCMP(p_enc, "latin1") == 0
					 || STRCMP(p_enc, "iso-8859-15") == 0)
    {
#నిర్వచించు A_grave 0xc0
#నిర్వచించు A_acute 0xc1
#నిర్వచించు A_circumflex 0xc2
#నిర్వచించు A_virguilla 0xc3
#నిర్వచించు A_diaeresis 0xc4
#నిర్వచించు A_ring 0xc5
#నిర్వచించు C_cedilla 0xc7
#నిర్వచించు E_grave 0xc8
#నిర్వచించు E_acute 0xc9
#నిర్వచించు E_circumflex 0xca
#నిర్వచించు E_diaeresis 0xcb
#నిర్వచించు I_grave 0xcc
#నిర్వచించు I_acute 0xcd
#నిర్వచించు I_circumflex 0xce
#నిర్వచించు I_diaeresis 0xcf
#నిర్వచించు N_virguilla 0xd1
#నిర్వచించు O_grave 0xd2
#నిర్వచించు O_acute 0xd3
#నిర్వచించు O_circumflex 0xd4
#నిర్వచించు O_virguilla 0xd5
#నిర్వచించు O_diaeresis 0xd6
#నిర్వచించు O_slash 0xd8
#నిర్వచించు U_grave 0xd9
#నిర్వచించు U_acute 0xda
#నిర్వచించు U_circumflex 0xdb
#నిర్వచించు U_diaeresis 0xdc
#నిర్వచించు Y_acute 0xdd
#నిర్వచించు a_grave 0xe0
#నిర్వచించు a_acute 0xe1
#నిర్వచించు a_circumflex 0xe2
#నిర్వచించు a_virguilla 0xe3
#నిర్వచించు a_diaeresis 0xe4
#నిర్వచించు a_ring 0xe5
#నిర్వచించు c_cedilla 0xe7
#నిర్వచించు e_grave 0xe8
#నిర్వచించు e_acute 0xe9
#నిర్వచించు e_circumflex 0xea
#నిర్వచించు e_diaeresis 0xeb
#నిర్వచించు i_grave 0xec
#నిర్వచించు i_acute 0xed
#నిర్వచించు i_circumflex 0xee
#నిర్వచించు i_diaeresis 0xef
#నిర్వచించు n_virguilla 0xf1
#నిర్వచించు o_grave 0xf2
#నిర్వచించు o_acute 0xf3
#నిర్వచించు o_circumflex 0xf4
#నిర్వచించు o_virguilla 0xf5
#నిర్వచించు o_diaeresis 0xf6
#నిర్వచించు o_slash 0xf8
#నిర్వచించు u_grave 0xf9
#నిర్వచించు u_acute 0xfa
#నిర్వచించు u_circumflex 0xfb
#నిర్వచించు u_diaeresis 0xfc
#నిర్వచించు y_acute 0xfd
#నిర్వచించు y_diaeresis 0xff
	ఎంపిక (c)
	{
	    స్థితి 'A': స్థితి A_grave: స్థితి A_acute: స్థితి A_circumflex:
	    స్థితి A_virguilla: స్థితి A_diaeresis: స్థితి A_ring:
	    స్థితి 0x100: స్థితి 0x102: స్థితి 0x104: స్థితి 0x1cd:
	    స్థితి 0x1de: స్థితి 0x1e0: స్థితి 0x1fa: స్థితి 0x200:
	    స్థితి 0x202: స్థితి 0x226: స్థితి 0x23a: స్థితి 0x1e00:
	    స్థితి 0x1ea0: స్థితి 0x1ea2: స్థితి 0x1ea4: స్థితి 0x1ea6:
	    స్థితి 0x1ea8: స్థితి 0x1eaa: స్థితి 0x1eac: స్థితి 0x1eae:
	    స్థితి 0x1eb0: స్థితి 0x1eb2: స్థితి 0x1eb4: స్థితి 0x1eb6:
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
		    తిరిగి OK;

	    స్థితి 'B': స్థితి 0x181: స్థితి 0x243: స్థితి 0x1e02:
	    స్థితి 0x1e04: స్థితి 0x1e06:
		    EMIT2('B')
		    EMIT2(0x181) EMIT2(0x243) EMIT2(0x1e02)
		    EMIT2(0x1e04) EMIT2(0x1e06)
		    తిరిగి OK;

	    స్థితి 'C': స్థితి C_cedilla: స్థితి 0x106: స్థితి 0x108:
	    స్థితి 0x10a: స్థితి 0x10c: స్థితి 0x187: స్థితి 0x23b:
	    స్థితి 0x1e08: స్థితి 0xa792:
		    EMIT2('C') EMIT2(C_cedilla)
		    EMIT2(0x106) EMIT2(0x108) EMIT2(0x10a)
		    EMIT2(0x10c) EMIT2(0x187) EMIT2(0x23b)
		    EMIT2(0x1e08) EMIT2(0xa792)
		    తిరిగి OK;

	    స్థితి 'D': స్థితి 0x10e: స్థితి 0x110: స్థితి 0x18a:
	    స్థితి 0x1e0a: స్థితి 0x1e0c: స్థితి 0x1e0e: స్థితి 0x1e10:
	    స్థితి 0x1e12:
		    EMIT2('D') EMIT2(0x10e) EMIT2(0x110) EMIT2(0x18a)
		    EMIT2(0x1e0a) EMIT2(0x1e0c) EMIT2(0x1e0e)
		    EMIT2(0x1e10) EMIT2(0x1e12)
		    తిరిగి OK;

	    స్థితి 'E': స్థితి E_grave: స్థితి E_acute: స్థితి E_circumflex:
	    స్థితి E_diaeresis: స్థితి 0x112: స్థితి 0x114: స్థితి 0x116:
	    స్థితి 0x118: స్థితి 0x11a: స్థితి 0x204: స్థితి 0x206:
	    స్థితి 0x228: స్థితి 0x246: స్థితి 0x1e14: స్థితి 0x1e16:
	    స్థితి 0x1e18: స్థితి 0x1e1a: స్థితి 0x1e1c: స్థితి 0x1eb8:
	    స్థితి 0x1eba: స్థితి 0x1ebc: స్థితి 0x1ebe: స్థితి 0x1ec0:
	    స్థితి 0x1ec2: స్థితి 0x1ec4: స్థితి 0x1ec6:
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
		    తిరిగి OK;

	    స్థితి 'F': స్థితి 0x191: స్థితి 0x1e1e: స్థితి 0xa798:
		    EMIT2('F') EMIT2(0x191) EMIT2(0x1e1e) EMIT2(0xa798)
		    తిరిగి OK;

	    స్థితి 'G': స్థితి 0x11c: స్థితి 0x11e: స్థితి 0x120:
	    స్థితి 0x122: స్థితి 0x193: స్థితి 0x1e4: స్థితి 0x1e6:
	    స్థితి 0x1f4: స్థితి 0x1e20: స్థితి 0xa7a0:
		    EMIT2('G') EMIT2(0x11c) EMIT2(0x11e) EMIT2(0x120)
		    EMIT2(0x122) EMIT2(0x193) EMIT2(0x1e4)
		    EMIT2(0x1e6) EMIT2(0x1f4) EMIT2(0x1e20)
		    EMIT2(0xa7a0)
		    తిరిగి OK;

	    స్థితి 'H': స్థితి 0x124: స్థితి 0x126: స్థితి 0x21e:
	    స్థితి 0x1e22: స్థితి 0x1e24: స్థితి 0x1e26: స్థితి 0x1e28:
	    స్థితి 0x1e2a: స్థితి 0x2c67:
		    EMIT2('H') EMIT2(0x124) EMIT2(0x126) EMIT2(0x21e)
		    EMIT2(0x1e22) EMIT2(0x1e24) EMIT2(0x1e26)
		    EMIT2(0x1e28) EMIT2(0x1e2a) EMIT2(0x2c67)
		    తిరిగి OK;

	    స్థితి 'I': స్థితి I_grave: స్థితి I_acute: స్థితి I_circumflex:
	    స్థితి I_diaeresis: స్థితి 0x128: స్థితి 0x12a: స్థితి 0x12c:
	    స్థితి 0x12e: స్థితి 0x130: స్థితి 0x197: స్థితి 0x1cf:
	    స్థితి 0x208: స్థితి 0x20a: స్థితి 0x1e2c: స్థితి 0x1e2e:
	    స్థితి 0x1ec8: స్థితి 0x1eca:
		    EMIT2('I') EMIT2(I_grave) EMIT2(I_acute)
		    EMIT2(I_circumflex) EMIT2(I_diaeresis)
		    EMIT2(0x128) EMIT2(0x12a) EMIT2(0x12c)
		    EMIT2(0x12e) EMIT2(0x130) EMIT2(0x197)
		    EMIT2(0x1cf) EMIT2(0x208) EMIT2(0x20a)
		    EMIT2(0x1e2c) EMIT2(0x1e2e) EMIT2(0x1ec8)
		    EMIT2(0x1eca)
		    తిరిగి OK;

	    స్థితి 'J': స్థితి 0x134: స్థితి 0x248:
		    EMIT2('J') EMIT2(0x134) EMIT2(0x248)
		    తిరిగి OK;

	    స్థితి 'K': స్థితి 0x136: స్థితి 0x198: స్థితి 0x1e8: స్థితి 0x1e30:
	    స్థితి 0x1e32: స్థితి 0x1e34: స్థితి 0x2c69: స్థితి 0xa740:
		    EMIT2('K') EMIT2(0x136) EMIT2(0x198) EMIT2(0x1e8)
		    EMIT2(0x1e30) EMIT2(0x1e32) EMIT2(0x1e34)
		    EMIT2(0x2c69) EMIT2(0xa740)
		    తిరిగి OK;

	    స్థితి 'L': స్థితి 0x139: స్థితి 0x13b: స్థితి 0x13d:
	    స్థితి 0x13f: స్థితి 0x141: స్థితి 0x23d: స్థితి 0x1e36:
	    స్థితి 0x1e38: స్థితి 0x1e3a: స్థితి 0x1e3c: స్థితి 0x2c60:
		    EMIT2('L') EMIT2(0x139) EMIT2(0x13b)
		    EMIT2(0x13d) EMIT2(0x13f) EMIT2(0x141)
		    EMIT2(0x23d) EMIT2(0x1e36) EMIT2(0x1e38)
		    EMIT2(0x1e3a) EMIT2(0x1e3c) EMIT2(0x2c60)
		    తిరిగి OK;

	    స్థితి 'M': స్థితి 0x1e3e: స్థితి 0x1e40: స్థితి 0x1e42:
		    EMIT2('M') EMIT2(0x1e3e) EMIT2(0x1e40)
		    EMIT2(0x1e42)
		    తిరిగి OK;

	    స్థితి 'N': స్థితి N_virguilla:
	    స్థితి 0x143: స్థితి 0x145: స్థితి 0x147: స్థితి 0x1f8:
	    స్థితి 0x1e44: స్థితి 0x1e46: స్థితి 0x1e48: స్థితి 0x1e4a:
	    స్థితి 0xa7a4:
		    EMIT2('N') EMIT2(N_virguilla)
		    EMIT2(0x143) EMIT2(0x145) EMIT2(0x147)
		    EMIT2(0x1f8) EMIT2(0x1e44) EMIT2(0x1e46)
		    EMIT2(0x1e48) EMIT2(0x1e4a) EMIT2(0xa7a4)
		    తిరిగి OK;

	    స్థితి 'O': స్థితి O_grave: స్థితి O_acute: స్థితి O_circumflex:
	    స్థితి O_virguilla: స్థితి O_diaeresis: స్థితి O_slash:
	    స్థితి 0x14c: స్థితి 0x14e: స్థితి 0x150: స్థితి 0x19f:
	    స్థితి 0x1a0: స్థితి 0x1d1: స్థితి 0x1ea: స్థితి 0x1ec:
	    స్థితి 0x1fe: స్థితి 0x20c: స్థితి 0x20e: స్థితి 0x22a:
	    స్థితి 0x22c: స్థితి 0x22e: స్థితి 0x230: స్థితి 0x1e4c:
	    స్థితి 0x1e4e: స్థితి 0x1e50: స్థితి 0x1e52: స్థితి 0x1ecc:
	    స్థితి 0x1ece: స్థితి 0x1ed0: స్థితి 0x1ed2: స్థితి 0x1ed4:
	    స్థితి 0x1ed6: స్థితి 0x1ed8: స్థితి 0x1eda: స్థితి 0x1edc:
	    స్థితి 0x1ede: స్థితి 0x1ee0: స్థితి 0x1ee2:
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
		    తిరిగి OK;

	    స్థితి 'P': స్థితి 0x1a4: స్థితి 0x1e54: స్థితి 0x1e56: స్థితి 0x2c63:
		    EMIT2('P') EMIT2(0x1a4) EMIT2(0x1e54) EMIT2(0x1e56)
		    EMIT2(0x2c63)
		    తిరిగి OK;

	    స్థితి 'Q': స్థితి 0x24a:
		    EMIT2('Q') EMIT2(0x24a)
		    తిరిగి OK;

	    స్థితి 'R': స్థితి 0x154: స్థితి 0x156: స్థితి 0x158: స్థితి 0x210:
	    స్థితి 0x212: స్థితి 0x24c: స్థితి 0x1e58: స్థితి 0x1e5a:
	    స్థితి 0x1e5c: స్థితి 0x1e5e: స్థితి 0x2c64: స్థితి 0xa7a6:
		    EMIT2('R') EMIT2(0x154) EMIT2(0x156) EMIT2(0x158)
		    EMIT2(0x210) EMIT2(0x212) EMIT2(0x24c) EMIT2(0x1e58)
		    EMIT2(0x1e5a) EMIT2(0x1e5c) EMIT2(0x1e5e) EMIT2(0x2c64)
		    EMIT2(0xa7a6)
		    తిరిగి OK;

	    స్థితి 'S': స్థితి 0x15a: స్థితి 0x15c: స్థితి 0x15e: స్థితి 0x160:
	    స్థితి 0x218: స్థితి 0x1e60: స్థితి 0x1e62: స్థితి 0x1e64:
	    స్థితి 0x1e66: స్థితి 0x1e68: స్థితి 0x2c7e: స్థితి 0xa7a8:
		    EMIT2('S') EMIT2(0x15a) EMIT2(0x15c) EMIT2(0x15e)
		    EMIT2(0x160) EMIT2(0x218) EMIT2(0x1e60) EMIT2(0x1e62)
		    EMIT2(0x1e64) EMIT2(0x1e66) EMIT2(0x1e68) EMIT2(0x2c7e)
		    EMIT2(0xa7a8)
		    తిరిగి OK;

	    స్థితి 'T': స్థితి 0x162: స్థితి 0x164: స్థితి 0x166: స్థితి 0x1ac:
	    స్థితి 0x1ae: స్థితి 0x21a: స్థితి 0x23e: స్థితి 0x1e6a: స్థితి 0x1e6c:
	    స్థితి 0x1e6e: స్థితి 0x1e70:
		    EMIT2('T') EMIT2(0x162) EMIT2(0x164) EMIT2(0x166)
		    EMIT2(0x1ac) EMIT2(0x1ae) EMIT2(0x23e) EMIT2(0x21a)
		    EMIT2(0x1e6a) EMIT2(0x1e6c) EMIT2(0x1e6e) EMIT2(0x1e70)
		    తిరిగి OK;

	    స్థితి 'U': స్థితి U_grave: స్థితి U_acute: స్థితి U_diaeresis:
	    స్థితి U_circumflex: స్థితి 0x168: స్థితి 0x16a: స్థితి 0x16c:
	    స్థితి 0x16e: స్థితి 0x170: స్థితి 0x172: స్థితి 0x1af:
	    స్థితి 0x1d3: స్థితి 0x1d5: స్థితి 0x1d7: స్థితి 0x1d9:
	    స్థితి 0x1db: స్థితి 0x214: స్థితి 0x216: స్థితి 0x244:
	    స్థితి 0x1e72: స్థితి 0x1e74: స్థితి 0x1e76: స్థితి 0x1e78:
	    స్థితి 0x1e7a: స్థితి 0x1ee4: స్థితి 0x1ee6: స్థితి 0x1ee8:
	    స్థితి 0x1eea: స్థితి 0x1eec: స్థితి 0x1eee: స్థితి 0x1ef0:
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
		    తిరిగి OK;

	    స్థితి 'V': స్థితి 0x1b2: స్థితి 0x1e7c: స్థితి 0x1e7e:
		    EMIT2('V') EMIT2(0x1b2) EMIT2(0x1e7c) EMIT2(0x1e7e)
		    తిరిగి OK;

	    స్థితి 'W': స్థితి 0x174: స్థితి 0x1e80: స్థితి 0x1e82: స్థితి 0x1e84:
	    స్థితి 0x1e86: స్థితి 0x1e88:
		    EMIT2('W') EMIT2(0x174) EMIT2(0x1e80) EMIT2(0x1e82)
		    EMIT2(0x1e84) EMIT2(0x1e86) EMIT2(0x1e88)
		    తిరిగి OK;

	    స్థితి 'X': స్థితి 0x1e8a: స్థితి 0x1e8c:
		    EMIT2('X') EMIT2(0x1e8a) EMIT2(0x1e8c)
		    తిరిగి OK;

	    స్థితి 'Y': స్థితి Y_acute: స్థితి 0x176: స్థితి 0x178:
	    స్థితి 0x1b3: స్థితి 0x232: స్థితి 0x24e: స్థితి 0x1e8e:
	    స్థితి 0x1ef2: స్థితి 0x1ef4: స్థితి 0x1ef6: స్థితి 0x1ef8:
		    EMIT2('Y') EMIT2(Y_acute)
		    EMIT2(0x176) EMIT2(0x178) EMIT2(0x1b3)
		    EMIT2(0x232) EMIT2(0x24e) EMIT2(0x1e8e)
		    EMIT2(0x1ef2) EMIT2(0x1ef4) EMIT2(0x1ef6)
		    EMIT2(0x1ef8)
		    తిరిగి OK;

	    స్థితి 'Z': స్థితి 0x179: స్థితి 0x17b: స్థితి 0x17d:
	    స్థితి 0x1b5: స్థితి 0x1e90: స్థితి 0x1e92: స్థితి 0x1e94:
	    స్థితి 0x2c6b:
		    EMIT2('Z') EMIT2(0x179) EMIT2(0x17b) EMIT2(0x17d)
		    EMIT2(0x1b5) EMIT2(0x1e90) EMIT2(0x1e92)
		    EMIT2(0x1e94) EMIT2(0x2c6b)
		    తిరిగి OK;

	    స్థితి 'a': స్థితి a_grave: స్థితి a_acute: స్థితి a_circumflex:
	    స్థితి a_virguilla: స్థితి a_diaeresis: స్థితి a_ring:
	    స్థితి 0x101: స్థితి 0x103: స్థితి 0x105: స్థితి 0x1ce:
	    స్థితి 0x1df: స్థితి 0x1e1: స్థితి 0x1fb: స్థితి 0x201:
	    స్థితి 0x203: స్థితి 0x227: స్థితి 0x1d8f: స్థితి 0x1e01:
	    స్థితి 0x1e9a: స్థితి 0x1ea1: స్థితి 0x1ea3: స్థితి 0x1ea5:
	    స్థితి 0x1ea7: స్థితి 0x1ea9: స్థితి 0x1eab: స్థితి 0x1ead:
	    స్థితి 0x1eaf: స్థితి 0x1eb1: స్థితి 0x1eb3: స్థితి 0x1eb5:
	    స్థితి 0x1eb7: స్థితి 0x2c65:
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
		    తిరిగి OK;

	    స్థితి 'b': స్థితి 0x180: స్థితి 0x253: స్థితి 0x1d6c: స్థితి 0x1d80:
	    స్థితి 0x1e03: స్థితి 0x1e05: స్థితి 0x1e07:
		    EMIT2('b') EMIT2(0x180) EMIT2(0x253) EMIT2(0x1d6c)
		    EMIT2(0x1d80) EMIT2(0x1e03) EMIT2(0x1e05) EMIT2(0x1e07)
		    తిరిగి OK;

	    స్థితి 'c': స్థితి c_cedilla: స్థితి 0x107: స్థితి 0x109: స్థితి 0x10b:
	    స్థితి 0x10d: స్థితి 0x188: స్థితి 0x23c: స్థితి 0x1e09: స్థితి 0xa793:
	    స్థితి 0xa794:
		    EMIT2('c') EMIT2(c_cedilla)
		    EMIT2(0x107) EMIT2(0x109) EMIT2(0x10b)
		    EMIT2(0x10d) EMIT2(0x188) EMIT2(0x23c)
		    EMIT2(0x1e09) EMIT2(0xa793) EMIT2(0xa794)
		    తిరిగి OK;

	    స్థితి 'd': స్థితి 0x10f: స్థితి 0x111: స్థితి 0x257: స్థితి 0x1d6d:
	    స్థితి 0x1d81: స్థితి 0x1d91: స్థితి 0x1e0b: స్థితి 0x1e0d: స్థితి 0x1e0f:
	    స్థితి 0x1e11: స్థితి 0x1e13:
		    EMIT2('d') EMIT2(0x10f) EMIT2(0x111)
		    EMIT2(0x257) EMIT2(0x1d6d) EMIT2(0x1d81)
		    EMIT2(0x1d91) EMIT2(0x1e0b) EMIT2(0x1e0d)
		    EMIT2(0x1e0f) EMIT2(0x1e11) EMIT2(0x1e13)
		    తిరిగి OK;

	    స్థితి 'e': స్థితి e_grave: స్థితి e_acute: స్థితి e_circumflex:
	    స్థితి e_diaeresis: స్థితి 0x113: స్థితి 0x115: స్థితి 0x117:
	    స్థితి 0x119: స్థితి 0x11b: స్థితి 0x205: స్థితి 0x207:
	    స్థితి 0x229: స్థితి 0x247: స్థితి 0x1d92: స్థితి 0x1e15:
	    స్థితి 0x1e17: స్థితి 0x1e19: స్థితి 0x1e1b: స్థితి 0x1e1d:
	    స్థితి 0x1eb9: స్థితి 0x1ebb: స్థితి 0x1ebd: స్థితి 0x1ebf:
	    స్థితి 0x1ec1: స్థితి 0x1ec3: స్థితి 0x1ec5: స్థితి 0x1ec7:
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
		    తిరిగి OK;

	    స్థితి 'f': స్థితి 0x192: స్థితి 0x1d6e: స్థితి 0x1d82:
	    స్థితి 0x1e1f: స్థితి 0xa799:
		    EMIT2('f') EMIT2(0x192) EMIT2(0x1d6e) EMIT2(0x1d82)
		    EMIT2(0x1e1f) EMIT2(0xa799)
		    తిరిగి OK;

	    స్థితి 'g': స్థితి 0x11d: స్థితి 0x11f: స్థితి 0x121: స్థితి 0x123:
	    స్థితి 0x1e5: స్థితి 0x1e7: స్థితి 0x1f5: స్థితి 0x260: స్థితి 0x1d83:
	    స్థితి 0x1e21: స్థితి 0xa7a1:
		    EMIT2('g') EMIT2(0x11d) EMIT2(0x11f) EMIT2(0x121)
		    EMIT2(0x123) EMIT2(0x1e5) EMIT2(0x1e7)
		    EMIT2(0x1f5) EMIT2(0x260) EMIT2(0x1d83)
		    EMIT2(0x1e21) EMIT2(0xa7a1)
		    తిరిగి OK;

	    స్థితి 'h': స్థితి 0x125: స్థితి 0x127: స్థితి 0x21f: స్థితి 0x1e23:
	    స్థితి 0x1e25: స్థితి 0x1e27: స్థితి 0x1e29: స్థితి 0x1e2b:
	    స్థితి 0x1e96: స్థితి 0x2c68: స్థితి 0xa795:
		    EMIT2('h') EMIT2(0x125) EMIT2(0x127) EMIT2(0x21f)
		    EMIT2(0x1e23) EMIT2(0x1e25) EMIT2(0x1e27)
		    EMIT2(0x1e29) EMIT2(0x1e2b) EMIT2(0x1e96)
		    EMIT2(0x2c68) EMIT2(0xa795)
		    తిరిగి OK;

	    స్థితి 'i': స్థితి i_grave: స్థితి i_acute: స్థితి i_circumflex:
	    స్థితి i_diaeresis: స్థితి 0x129: స్థితి 0x12b: స్థితి 0x12d:
	    స్థితి 0x12f: స్థితి 0x1d0: స్థితి 0x209: స్థితి 0x20b:
	    స్థితి 0x268: స్థితి 0x1d96: స్థితి 0x1e2d: స్థితి 0x1e2f:
	    స్థితి 0x1ec9: స్థితి 0x1ecb:
		    EMIT2('i') EMIT2(i_grave) EMIT2(i_acute)
		    EMIT2(i_circumflex) EMIT2(i_diaeresis)
		    EMIT2(0x129) EMIT2(0x12b) EMIT2(0x12d)
		    EMIT2(0x12f) EMIT2(0x1d0) EMIT2(0x209)
		    EMIT2(0x20b) EMIT2(0x268) EMIT2(0x1d96)
		    EMIT2(0x1e2d) EMIT2(0x1e2f) EMIT2(0x1ec9)
		    EMIT2(0x1ecb)
		    తిరిగి OK;

	    స్థితి 'j': స్థితి 0x135: స్థితి 0x1f0: స్థితి 0x249:
		    EMIT2('j') EMIT2(0x135) EMIT2(0x1f0) EMIT2(0x249)
		    తిరిగి OK;

	    స్థితి 'k': స్థితి 0x137: స్థితి 0x199: స్థితి 0x1e9: స్థితి 0x1d84:
	    స్థితి 0x1e31: స్థితి 0x1e33: స్థితి 0x1e35: స్థితి 0x2c6a: స్థితి 0xa741:
		    EMIT2('k') EMIT2(0x137) EMIT2(0x199) EMIT2(0x1e9)
		    EMIT2(0x1d84) EMIT2(0x1e31) EMIT2(0x1e33)
		    EMIT2(0x1e35) EMIT2(0x2c6a) EMIT2(0xa741)
		    తిరిగి OK;

	    స్థితి 'l': స్థితి 0x13a: స్థితి 0x13c: స్థితి 0x13e: స్థితి 0x140:
	    స్థితి 0x142: స్థితి 0x19a: స్థితి 0x1e37: స్థితి 0x1e39: స్థితి 0x1e3b:
	    స్థితి 0x1e3d: స్థితి 0x2c61:
		    EMIT2('l') EMIT2(0x13a) EMIT2(0x13c)
		    EMIT2(0x13e) EMIT2(0x140) EMIT2(0x142)
		    EMIT2(0x19a) EMIT2(0x1e37) EMIT2(0x1e39)
		    EMIT2(0x1e3b) EMIT2(0x1e3d) EMIT2(0x2c61)
		    తిరిగి OK;

	    స్థితి 'm': స్థితి 0x1d6f: స్థితి 0x1e3f: స్థితి 0x1e41: స్థితి 0x1e43:
		    EMIT2('m') EMIT2(0x1d6f) EMIT2(0x1e3f)
		    EMIT2(0x1e41) EMIT2(0x1e43)
		    తిరిగి OK;

	    స్థితి 'n': స్థితి n_virguilla: స్థితి 0x144: స్థితి 0x146: స్థితి 0x148:
	    స్థితి 0x149: స్థితి 0x1f9: స్థితి 0x1d70: స్థితి 0x1d87: స్థితి 0x1e45:
	    స్థితి 0x1e47: స్థితి 0x1e49: స్థితి 0x1e4b: స్థితి 0xa7a5:
		    EMIT2('n') EMIT2(n_virguilla)
		    EMIT2(0x144) EMIT2(0x146) EMIT2(0x148)
		    EMIT2(0x149) EMIT2(0x1f9) EMIT2(0x1d70)
		    EMIT2(0x1d87) EMIT2(0x1e45) EMIT2(0x1e47)
		    EMIT2(0x1e49) EMIT2(0x1e4b) EMIT2(0xa7a5)
		    తిరిగి OK;

	    స్థితి 'o': స్థితి o_grave: స్థితి o_acute: స్థితి o_circumflex:
	    స్థితి o_virguilla: స్థితి o_diaeresis: స్థితి o_slash:
	    స్థితి 0x14d: స్థితి 0x14f: స్థితి 0x151: స్థితి 0x1a1:
	    స్థితి 0x1d2: స్థితి 0x1eb: స్థితి 0x1ed: స్థితి 0x1ff:
	    స్థితి 0x20d: స్థితి 0x20f: స్థితి 0x22b: స్థితి 0x22d:
	    స్థితి 0x22f: స్థితి 0x231: స్థితి 0x275: స్థితి 0x1e4d:
	    స్థితి 0x1e4f: స్థితి 0x1e51: స్థితి 0x1e53: స్థితి 0x1ecd:
	    స్థితి 0x1ecf: స్థితి 0x1ed1: స్థితి 0x1ed3: స్థితి 0x1ed5:
	    స్థితి 0x1ed7: స్థితి 0x1ed9: స్థితి 0x1edb: స్థితి 0x1edd:
	    స్థితి 0x1edf: స్థితి 0x1ee1: స్థితి 0x1ee3:
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
		    తిరిగి OK;

	    స్థితి 'p': స్థితి 0x1a5: స్థితి 0x1d71: స్థితి 0x1d7d: స్థితి 0x1d88:
	    స్థితి 0x1e55: స్థితి 0x1e57:
		    EMIT2('p') EMIT2(0x1a5) EMIT2(0x1d71) EMIT2(0x1d7d)
		    EMIT2(0x1d88) EMIT2(0x1e55) EMIT2(0x1e57)
		    తిరిగి OK;

	    స్థితి 'q': స్థితి 0x24b: స్థితి 0x2a0:
		    EMIT2('q') EMIT2(0x24b) EMIT2(0x2a0)
		    తిరిగి OK;

	    స్థితి 'r': స్థితి 0x155: స్థితి 0x157: స్థితి 0x159: స్థితి 0x211:
	    స్థితి 0x213: స్థితి 0x24d: స్థితి 0x27d: స్థితి 0x1d72: స్థితి 0x1d73:
	    స్థితి 0x1d89: స్థితి 0x1e59: స్థితి 0x1e5b: స్థితి 0x1e5d: స్థితి 0x1e5f:
	    స్థితి 0xa7a7:
		    EMIT2('r') EMIT2(0x155) EMIT2(0x157) EMIT2(0x159)
		    EMIT2(0x211) EMIT2(0x213) EMIT2(0x24d) EMIT2(0x27d)
		    EMIT2(0x1d72) EMIT2(0x1d73) EMIT2(0x1d89) EMIT2(0x1e59)
		    EMIT2(0x1e5b) EMIT2(0x1e5d) EMIT2(0x1e5f) EMIT2(0xa7a7)
		    తిరిగి OK;

	    స్థితి 's': స్థితి 0x15b: స్థితి 0x15d: స్థితి 0x15f: స్థితి 0x161:
	    స్థితి 0x219: స్థితి 0x23f: స్థితి 0x1d74: స్థితి 0x1d8a: స్థితి 0x1e61:
	    స్థితి 0x1e63: స్థితి 0x1e65: స్థితి 0x1e67: స్థితి 0x1e69: స్థితి 0xa7a9:
		    EMIT2('s') EMIT2(0x15b) EMIT2(0x15d) EMIT2(0x15f)
		    EMIT2(0x161) EMIT2(0x219) EMIT2(0x23f) EMIT2(0x1d74)
		    EMIT2(0x1d8a) EMIT2(0x1e61) EMIT2(0x1e63) EMIT2(0x1e65)
		    EMIT2(0x1e67) EMIT2(0x1e69) EMIT2(0xa7a9)
		    తిరిగి OK;

	    స్థితి 't': స్థితి 0x163: స్థితి 0x165: స్థితి 0x167: స్థితి 0x1ab:
	    స్థితి 0x1ad: స్థితి 0x21b: స్థితి 0x288: స్థితి 0x1d75: స్థితి 0x1e6b:
	    స్థితి 0x1e6d: స్థితి 0x1e6f: స్థితి 0x1e71: స్థితి 0x1e97: స్థితి 0x2c66:
		    EMIT2('t') EMIT2(0x163) EMIT2(0x165) EMIT2(0x167)
		    EMIT2(0x1ab) EMIT2(0x1ad) EMIT2(0x21b) EMIT2(0x288)
		    EMIT2(0x1d75) EMIT2(0x1e6b) EMIT2(0x1e6d) EMIT2(0x1e6f)
		    EMIT2(0x1e71) EMIT2(0x1e97) EMIT2(0x2c66)
		    తిరిగి OK;

	    స్థితి 'u': స్థితి u_grave: స్థితి u_acute: స్థితి u_circumflex:
	    స్థితి u_diaeresis: స్థితి 0x169: స్థితి 0x16b: స్థితి 0x16d:
	    స్థితి 0x16f: స్థితి 0x171: స్థితి 0x173: స్థితి 0x1b0: స్థితి 0x1d4:
	    స్థితి 0x1d6: స్థితి 0x1d8: స్థితి 0x1da: స్థితి 0x1dc: స్థితి 0x215:
	    స్థితి 0x217: స్థితి 0x289: స్థితి 0x1d7e: స్థితి 0x1d99: స్థితి 0x1e73:
	    స్థితి 0x1e75: స్థితి 0x1e77: స్థితి 0x1e79: స్థితి 0x1e7b:
	    స్థితి 0x1ee5: స్థితి 0x1ee7: స్థితి 0x1ee9: స్థితి 0x1eeb:
	    స్థితి 0x1eed: స్థితి 0x1eef: స్థితి 0x1ef1:
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
		    తిరిగి OK;

	    స్థితి 'v': స్థితి 0x28b: స్థితి 0x1d8c: స్థితి 0x1e7d: స్థితి 0x1e7f:
		    EMIT2('v') EMIT2(0x28b) EMIT2(0x1d8c) EMIT2(0x1e7d)
		    EMIT2(0x1e7f)
		    తిరిగి OK;

	    స్థితి 'w': స్థితి 0x175: స్థితి 0x1e81: స్థితి 0x1e83: స్థితి 0x1e85:
	    స్థితి 0x1e87: స్థితి 0x1e89: స్థితి 0x1e98:
		    EMIT2('w') EMIT2(0x175) EMIT2(0x1e81) EMIT2(0x1e83)
		    EMIT2(0x1e85) EMIT2(0x1e87) EMIT2(0x1e89) EMIT2(0x1e98)
		    తిరిగి OK;

	    స్థితి 'x': స్థితి 0x1e8b: స్థితి 0x1e8d:
		    EMIT2('x') EMIT2(0x1e8b) EMIT2(0x1e8d)
		    తిరిగి OK;

	    స్థితి 'y': స్థితి y_acute: స్థితి y_diaeresis: స్థితి 0x177:
	    స్థితి 0x1b4: స్థితి 0x233: స్థితి 0x24f: స్థితి 0x1e8f:
	    స్థితి 0x1e99: స్థితి 0x1ef3: స్థితి 0x1ef5: స్థితి 0x1ef7:
	    స్థితి 0x1ef9:
		    EMIT2('y') EMIT2(y_acute) EMIT2(y_diaeresis)
		    EMIT2(0x177) EMIT2(0x1b4) EMIT2(0x233) EMIT2(0x24f)
		    EMIT2(0x1e8f) EMIT2(0x1e99) EMIT2(0x1ef3)
		    EMIT2(0x1ef5) EMIT2(0x1ef7) EMIT2(0x1ef9)
		    తిరిగి OK;

	    స్థితి 'z': స్థితి 0x17a: స్థితి 0x17c: స్థితి 0x17e: స్థితి 0x1b6:
	    స్థితి 0x1d76: స్థితి 0x1d8e: స్థితి 0x1e91: స్థితి 0x1e93:
	    స్థితి 0x1e95: స్థితి 0x2c6c:
		    EMIT2('z') EMIT2(0x17a) EMIT2(0x17c) EMIT2(0x17e)
		    EMIT2(0x1b6) EMIT2(0x1d76) EMIT2(0x1d8e) EMIT2(0x1e91)
		    EMIT2(0x1e93) EMIT2(0x1e95) EMIT2(0x2c6c)
		    తిరిగి OK;

	    // default: character itself
	}
    }

    EMIT2(c);
    తిరిగి OK;
#నిర్వచనం_రద్దు EMIT2
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
    స్థిర పూర్ణాంకం
nfa_regatom(శూన్య)
{
    పూర్ణాంకం		c;
    పూర్ణాంకం		charclass;
    పూర్ణాంకం		equiclass;
    పూర్ణాంకం		collclass;
    పూర్ణాంకం		got_coll_char;
    char_u	*p;
    char_u	*endp;
    char_u	*old_regparse = regparse;
    పూర్ణాంకం		extra = 0;
    పూర్ణాంకం		emit_range;
    పూర్ణాంకం		negated;
    పూర్ణాంకం		ఫలితం;
    పూర్ణాంకం		startc = -1;
    పూర్ణాంకం		save_prev_at_ప్రారంభం = prev_at_ప్రారంభం;

    c = getchr();
    ఎంపిక (c)
    {
	స్థితి NUL:
	    EMSG_RET_FAIL(_(e_nfa_regexp_చివర_encountered_prematurely));

	స్థితి Magic('^'):
	    EMIT(NFA_BOL);
	    ఆపు;

	స్థితి Magic('$'):
	    EMIT(NFA_EOL);
#ఒకవేళ defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#అయితే_ముగింపు
	    ఆపు;

	స్థితి Magic('<'):
	    EMIT(NFA_BOW);
	    ఆపు;

	స్థితి Magic('>'):
	    EMIT(NFA_EOW);
	    ఆపు;

	స్థితి Magic('_'):
	    c = no_Magic(getchr());
	    ఒకవేళ (c == NUL)
		EMSG_RET_FAIL(_(e_nfa_regexp_చివర_encountered_prematurely));

	    ఒకవేళ (c == '^')	// "\_^" is start-of-line
	    {
		EMIT(NFA_BOL);
		ఆపు;
	    }
	    ఒకవేళ (c == '$')	// "\_$" is end-of-line
	    {
		EMIT(NFA_EOL);
#ఒకవేళ defined(FEAT_SYN_HL)
		had_eol = TRUE;
#అయితే_ముగింపు
		ఆపు;
	    }

	    extra = NFA_కూడు_NL;

	    // "\_[" is collection plus newline
	    ఒకవేళ (c == '[')
		వెళ్ళు collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	/*
	 * Character classes.
	 */
	స్థితి Magic('.'):
	స్థితి Magic('i'):
	స్థితి Magic('I'):
	స్థితి Magic('k'):
	స్థితి Magic('K'):
	స్థితి Magic('f'):
	స్థితి Magic('F'):
	స్థితి Magic('p'):
	స్థితి Magic('P'):
	స్థితి Magic('s'):
	స్థితి Magic('S'):
	స్థితి Magic('d'):
	స్థితి Magic('D'):
	స్థితి Magic('x'):
	స్థితి Magic('X'):
	స్థితి Magic('o'):
	స్థితి Magic('O'):
	స్థితి Magic('w'):
	స్థితి Magic('W'):
	స్థితి Magic('h'):
	స్థితి Magic('H'):
	స్థితి Magic('a'):
	స్థితి Magic('A'):
	స్థితి Magic('l'):
	స్థితి Magic('L'):
	స్థితి Magic('u'):
	స్థితి Magic('U'):
	    p = vim_strchr(classchars, no_Magic(c));
	    ఒకవేళ (p == ఖాళీ)
	    {
		ఒకవేళ (extra == NFA_కూడు_NL)
		{
		    semsg(_(e_nfa_regexp_invalid_character_class_nr), c);
		    rc_did_emsg = TRUE;
		    తిరిగి FAIL;
		}
		siemsg("Unknown character class char: %d", c);
		తిరిగి FAIL;
	    }

	    // When '.' is followed by a composing char ignore the dot, so that
	    // the composing char is matched here.
	    ఒకవేళ (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	    {
		old_regparse = regparse;
		c = getchr();
		వెళ్ళు nfa_do_multibyte;
	    }
	    EMIT(nfa_classcodes[p - classchars]);
	    ఒకవేళ (extra == NFA_కూడు_NL)
	    {
		EMIT(NFA_NEWL);
		EMIT(NFA_OR);
		regflags |= RF_HASNL;
	    }
	    ఆపు;

	స్థితి Magic('n'):
	    ఒకవేళ (reg_string)
		// In a string "\n" matches a newline character.
		EMIT(NL);
	    లేకపోతే
	    {
		// In buffer text "\n" matches the end of a line.
		EMIT(NFA_NEWL);
		regflags |= RF_HASNL;
	    }
	    ఆపు;

	స్థితి Magic('('):
	    ఒకవేళ (nfa_reg(REG_PAREN) == FAIL)
		తిరిగి FAIL;	    // cascaded error
	    ఆపు;

	స్థితి Magic('|'):
	స్థితి Magic('&'):
	స్థితి Magic(')'):
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    తిరిగి FAIL;

	స్థితి Magic('='):
	స్థితి Magic('?'):
	స్థితి Magic('+'):
	స్థితి Magic('@'):
	స్థితి Magic('*'):
	స్థితి Magic('{'):
	    // these should follow an atom, not form an atom
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    తిరిగి FAIL;

	స్థితి Magic('~'):
	    {
		char_u	    *lp;

		// Previous substitute pattern.
		// Generated as "\%(pattern\)".
		ఒకవేళ (reg_prev_sub == ఖాళీ)
		{
		    emsg(_(e_no_మునుపటి_substitute_regular_వ్యక్తీకరణ));
		    తిరిగి FAIL;
		}
		కొరకు (lp = reg_prev_sub; *lp != NUL; MB_CPTR_ADV(lp))
		{
		    EMIT(PTR2CHAR(lp));
		    ఒకవేళ (lp != reg_prev_sub)
			EMIT(NFA_CONCAT);
		}
		EMIT(NFA_NOPEN);
		ఆపు;
	    }

	స్థితి Magic('1'):
	స్థితి Magic('2'):
	స్థితి Magic('3'):
	స్థితి Magic('4'):
	స్థితి Magic('5'):
	స్థితి Magic('6'):
	స్థితి Magic('7'):
	స్థితి Magic('8'):
	స్థితి Magic('9'):
	    {
		పూర్ణాంకం refnum = no_Magic(c) - '1';

		ఒకవేళ (!చూసిన_endbrace(refnum + 1))
		    తిరిగి FAIL;
		EMIT(NFA_BACKREF1 + refnum);
		rex.nfa_has_backref = TRUE;
	    }
	    ఆపు;

	స్థితి Magic('z'):
	    c = no_Magic(getchr());
	    ఎంపిక (c)
	    {
		స్థితి 's':
		    EMIT(NFA_ZSTART);
		    ఒకవేళ (re_mult_తదుపరి("\\zs") == FAIL)
			తిరిగి FAIL;
		    ఆపు;
		స్థితి 'e':
		    EMIT(NFA_ZEND);
		    rex.nfa_has_zend = TRUE;
		    ఒకవేళ (re_mult_తదుపరి("\\ze") == FAIL)
			తిరిగి FAIL;
		    ఆపు;
#నిర్వచితమైతే FEAT_SYN_HL
		స్థితి '1':
		స్థితి '2':
		స్థితి '3':
		స్థితి '4':
		స్థితి '5':
		స్థితి '6':
		స్థితి '7':
		స్థితి '8':
		స్థితి '9':
		    // \z1...\z9
		    ఒకవేళ ((reg_do_extmatch & REX_USE) == 0)
			EMSG_RET_FAIL(_(e_z1_z9_not_allowed_ఇక్కడ));
		    EMIT(NFA_ZREF1 + (no_Magic(c) - '1'));
		    // No need to set rex.nfa_has_backref, the sub-matches don't
		    // change when \z1 .. \z9 matches or not.
		    re_has_z = REX_USE;
		    ఆపు;
		స్థితి '(':
		    // \z(
		    ఒకవేళ ((reg_do_extmatch & REX_సమితి) == 0)
			EMSG_RET_FAIL(_(e_z_not_allowed_ఇక్కడ));
		    ఒకవేళ (nfa_reg(REG_ZPAREN) == FAIL)
			తిరిగి FAIL;	    // cascaded error
		    re_has_z = REX_సమితి;
		    ఆపు;
#అయితే_ముగింపు
		అప్రమేయ:
		    semsg(_(e_nfa_regexp_unknown_operator_z_chr), no_Magic(c));
		    తిరిగి FAIL;
	    }
	    ఆపు;

	స్థితి Magic('%'):
	    c = no_Magic(getchr());
	    ఎంపిక (c)
	    {
		// () without a back reference
		స్థితి '(':
		    ఒకవేళ (nfa_reg(REG_NPAREN) == FAIL)
			తిరిగి FAIL;
		    EMIT(NFA_NOPEN);
		    ఆపు;

		స్థితి 'd':   // %d123 decimal
		స్థితి 'o':   // %o123 octal
		స్థితి 'x':   // %xab hex 2
		స్థితి 'u':   // %uabcd hex 4
		స్థితి 'U':   // %U1234abcd hex 8
		    {
			vimlong_T nr;

			ఎంపిక (c)
			{
			    స్థితి 'd': nr = getdecchrs(); ఆపు;
			    స్థితి 'o': nr = getoctchrs(); ఆపు;
			    స్థితి 'x': nr = gethexchrs(2); ఆపు;
			    స్థితి 'u': nr = gethexchrs(4); ఆపు;
			    స్థితి 'U': nr = gethexchrs(8); ఆపు;
			    అప్రమేయ:  nr = -1; ఆపు;
			}

			ఒకవేళ (nr < 0 || nr > గరిష్ఠ_పూర్ణాంకం)
			    EMSG2_RET_FAIL(_(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			// A NUL is stored in the text as NL
			// TODO: what if a composing character follows?
			EMIT(nr == 0 ? 0x0a : (దీర్ఘ)nr);
		    }
		    ఆపు;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		స్థితి '^':
		    EMIT(NFA_BOF);
		    ఆపు;

		స్థితి '$':
		    EMIT(NFA_EOF);
		    ఆపు;

		స్థితి '#':
		    ఒకవేళ (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_ప్రారంభం_యొక్క_నమూనా),
								  regparse[1]);
			తిరిగి FAIL;
		    }
		    EMIT(NFA_CURSOR);
		    ఆపు;

		స్థితి 'V':
		    EMIT(NFA_VISUAL);
		    ఆపు;

		స్థితి 'C':
		    EMIT(NFA_ANY_COMPOSING);
		    ఆపు;

		స్థితి '[':
		    {
			పూర్ణాంకం	    n;

			// \%[abc]
			కొరకు (n = 0; (c = peekchr()) != ']'; ++n)
			{
			    ఒకవేళ (c == NUL)
				EMSG2_RET_FAIL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
			    // recursive call!
			    ఒకవేళ (nfa_regatom() == FAIL)
				తిరిగి FAIL;
			}
			getchr();  // get the ]
			ఒకవేళ (n == 0)
			    EMSG2_RET_FAIL(_(e_ఖాళీలు_str_brackets),
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
			ఆపు;
		    }

		అప్రమేయ:
		    {
			long_u	n = 0;
			పూర్ణాంకం	cmp = c;
			పూర్ణాంకం	cur = FALSE;
			పూర్ణాంకం	got_digit = FALSE;

			ఒకవేళ (c == '<' || c == '>')
			    c = getchr();
			ఒకవేళ (no_Magic(c) == '.')
			{
			    cur = TRUE;
			    c = getchr();
			}
			అయితే (VIM_ISDIGIT(c))
			{
			    long_u tmp;

			    ఒకవేళ (cur)
			    {
				semsg(_(e_regexp_సంఖ్య_after_dot_pos_శోధన_chr),
								  no_Magic(c));
				తిరిగి FAIL;
			    }
			    tmp = n * 10 + (c - '0');

			    ఒకవేళ (tmp < n)
			    {
				// overflow.
				emsg(_(e_percent_విలువ_too_large));
				తిరిగి FAIL;
			    }
			    n = tmp;
			    c = getchr();
			    got_digit = TRUE;
			}
			ఒకవేళ (c == 'l' || c == 'c' || c == 'v')
			{
			    long_u హద్దు = గరిష్ఠ_పూర్ణాంకం;

			    ఒకవేళ (!cur && !got_digit)
			    {
				semsg(_(e_nfa_regexp_missing_విలువ_లోపల_chr),
								  no_Magic(c));
				తిరిగి FAIL;
			    }
			    ఒకవేళ (c == 'l')
			    {
				ఒకవేళ (cur)
				    n = curwin->w_cursor.lnum;
				// \%{n}l  \%{n}<l  \%{n}>l
				EMIT(cmp == '<' ? NFA_LNUM_LT :
				     cmp == '>' ? NFA_LNUM_GT : NFA_LNUM);
				ఒకవేళ (save_prev_at_ప్రారంభం)
				    at_ప్రారంభం = TRUE;
			    }
			    లేకపోతే ఒకవేళ (c == 'c')
			    {
				ఒకవేళ (cur)
				{
				    n = curwin->w_cursor.నిలువు;
				    n++;
				}
				// \%{n}c  \%{n}<c  \%{n}>c
				EMIT(cmp == '<' ? NFA_నిలువు_LT :
				     cmp == '>' ? NFA_నిలువు_GT : NFA_నిలువు);
			    }
			    లేకపోతే
			    {
				ఒకవేళ (cur)
				{
				    colnr_T vcol = 0;

				    getvvcol(curwin, &curwin->w_cursor,
							 ఖాళీ, ఖాళీ, &vcol, 0);
				    n = ++vcol;
				}
				// \%{n}v  \%{n}<v  \%{n}>v
				EMIT(cmp == '<' ? NFA_VCOL_LT :
				     cmp == '>' ? NFA_VCOL_GT : NFA_VCOL);
				హద్దు = గరిష్ఠ_పూర్ణాంకం / MB_MAXBYTES;
			    }
			    ఒకవేళ (n >= హద్దు)
			    {
				emsg(_(e_percent_విలువ_too_large));
				తిరిగి FAIL;
			    }
			    EMIT((పూర్ణాంకం)n);
			    ఆపు;
			}
			లేకపోతే ఒకవేళ (no_Magic(c) == '\'' && n == 0)
			{
			    // \%'m  \%<'m  \%>'m
			    EMIT(cmp == '<' ? NFA_MARK_LT :
				 cmp == '>' ? NFA_MARK_GT : NFA_MARK);
			    EMIT(getchr());
			    ఆపు;
			}
		    }
		    semsg(_(e_nfa_regexp_unknown_operator_percent_chr),
								  no_Magic(c));
		    తిరిగి FAIL;
	    }
	    ఆపు;

	స్థితి Magic('['):
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
	    ఒకవేళ (*endp == ']')
	    {
		పూర్ణాంకం plen;
		bool range_endpoint;
		/*
		 * Try to reverse engineer character classes. For example,
		 * recognize that [0-9] stands for \d and [A-Za-z_] for \h,
		 * and perform the necessary substitutions in the NFA.
		 */
		ఫలితం = nfa_recognize_char_class(regparse, endp,
							 extra == NFA_కూడు_NL);
		ఒకవేళ (ఫలితం != FAIL)
		{
		    ఒకవేళ (ఫలితం >= NFA_మొదటి_NL && ఫలితం <= NFA_చివరి_NL)
		    {
			EMIT(ఫలితం - NFA_కూడు_NL);
			EMIT(NFA_NEWL);
			EMIT(NFA_OR);
		    }
		    లేకపోతే
			EMIT(ఫలితం);
		    regparse = endp;
		    MB_PTR_ADV(regparse);
		    తిరిగి OK;
		}
		/*
		 * Failed to recognize a character class. Use the simple
		 * version that turns [abc] into 'a' OR 'b' OR 'c'
		 */
		startc = -1;
		negated = FALSE;
		ఒకవేళ (*regparse == '^')			// negated range
		{
		    negated = TRUE;
		    MB_PTR_ADV(regparse);
		    EMIT(NFA_ప్రారంభం_NEG_COLL);
		}
		లేకపోతే
		    EMIT(NFA_ప్రారంభం_COLL);
		ఒకవేళ (*regparse == '-')
		{
		    startc = '-';
		    EMIT(startc);
		    EMIT(NFA_CONCAT);
		    MB_PTR_ADV(regparse);
		}
		// Emit the OR branches for each character in the []
		emit_range = FALSE;
		అయితే (regparse < endp)
		{
		    పూర్ణాంకం	    oldstartc = startc;
		    range_endpoint = false;

		    startc = -1;
		    got_coll_char = FALSE;
		    ఒకవేళ (*regparse == '[')
		    {
			// Check for [: :], [= =], [. .]
			equiclass = collclass = 0;
			charclass = get_char_class(&regparse);
			ఒకవేళ (charclass == CLASS_NONE)
			{
			    equiclass = get_equi_class(&regparse);
			    ఒకవేళ (equiclass == 0)
				collclass = get_coll_element(&regparse);
			}

			// Character class like [:alpha:]
			ఒకవేళ (charclass != CLASS_NONE)
			{
			    ఎంపిక (charclass)
			    {
				స్థితి CLASS_ALNUM:
				    EMIT(NFA_CLASS_ALNUM);
				    ఆపు;
				స్థితి CLASS_ALPHA:
				    EMIT(NFA_CLASS_ALPHA);
				    ఆపు;
				స్థితి CLASS_BLANK:
				    EMIT(NFA_CLASS_BLANK);
				    ఆపు;
				స్థితి CLASS_CNTRL:
				    EMIT(NFA_CLASS_CNTRL);
				    ఆపు;
				స్థితి CLASS_DIGIT:
				    EMIT(NFA_CLASS_DIGIT);
				    ఆపు;
				స్థితి CLASS_లేఖాచిత్రం:
				    EMIT(NFA_CLASS_లేఖాచిత్రం);
				    ఆపు;
				స్థితి CLASS_క్రింది:
				    కోరుతుంది_nfa = TRUE;
				    EMIT(NFA_CLASS_క్రింది);
				    ఆపు;
				స్థితి CLASS_PRINT:
				    EMIT(NFA_CLASS_PRINT);
				    ఆపు;
				స్థితి CLASS_PUNCT:
				    EMIT(NFA_CLASS_PUNCT);
				    ఆపు;
				స్థితి CLASS_SPACE:
				    EMIT(NFA_CLASS_SPACE);
				    ఆపు;
				స్థితి CLASS_పై:
				    కోరుతుంది_nfa = TRUE;
				    EMIT(NFA_CLASS_పై);
				    ఆపు;
				స్థితి CLASS_XDIGIT:
				    EMIT(NFA_CLASS_XDIGIT);
				    ఆపు;
				స్థితి CLASS_TAB:
				    EMIT(NFA_CLASS_TAB);
				    ఆపు;
				స్థితి CLASS_RETURN:
				    EMIT(NFA_CLASS_RETURN);
				    ఆపు;
				స్థితి CLASS_BACKSPACE:
				    EMIT(NFA_CLASS_BACKSPACE);
				    ఆపు;
				స్థితి CLASS_ESCAPE:
				    EMIT(NFA_CLASS_ESCAPE);
				    ఆపు;
				స్థితి CLASS_IDENT:
				    EMIT(NFA_CLASS_IDENT);
				    ఆపు;
				స్థితి CLASS_KEYWORD:
				    EMIT(NFA_CLASS_KEYWORD);
				    ఆపు;
				స్థితి CLASS_FNAME:
				    EMIT(NFA_CLASS_FNAME);
				    ఆపు;
			    }
			    EMIT(NFA_CONCAT);
			    కొనసాగు;
			}
			// Try equivalence class [=a=] and the like
			ఒకవేళ (equiclass != 0)
			{
			    ఫలితం = nfa_emit_equi_class(equiclass);
			    ఒకవేళ (ఫలితం == FAIL)
			    {
				// should never happen
				EMSG_RET_FAIL(_(e_error_building_nfa_with_equivalence_class));
			    }
			    కొనసాగు;
			}
			// Try collating class like [. .]
			ఒకవేళ (collclass != 0)
			{
			    startc = collclass;	 // allow [.a.]-x as a range
			    // Will emit the proper atom at the end of the
			    // while loop.
			}
		    }
		    // Try a range like 'a-x' or '\t-z'. Also allows '-' as a
		    // start character.
		    ఒకవేళ (*regparse == '-' && oldstartc != -1)
		    {
			emit_range = TRUE;
			startc = oldstartc;
			MB_PTR_ADV(regparse);
			కొనసాగు;	    // reading the end of the range
		    }

		    // Now handle simple and escaped characters.
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    ఒకవేళ (*regparse == '\\'
			    && !reg_cpo_bsl
			    && regparse + 1 <= endp
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != ఖాళీ
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR, regparse[1])
								      != ఖాళీ)
			    )
			)
		    {
			MB_PTR_ADV(regparse);

			ఒకవేళ (*regparse == 'n')
			    startc = (reg_string || emit_range
					|| regparse[1] == '-') ? NL : NFA_NEWL;
			లేకపోతే ఒకవేళ (*regparse == 'd'
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
				ఒకవేళ (startc == గరిష్ఠ_పూర్ణాంకం)
				    EMSG_RET_FAIL(_(e_unicode_val_too_large));
				got_coll_char = TRUE;
				MB_PTR_వెనుక(old_regparse, regparse);
			    }
			    లేకపోతే
			    {
				// \r,\t,\e,\b
				startc = backslash_trans(*regparse);
			    }
		    }

		    // Normal printable char
		    ఒకవేళ (startc == -1)
			startc = PTR2CHAR(regparse);

		    // Previous char was '-', so this char is end of range.
		    ఒకవేళ (emit_range)
		    {
			పూర్ణాంకం	endc = startc;
			range_endpoint = true;

			startc = oldstartc;
			ఒకవేళ (startc > endc)
			    EMSG_RET_FAIL(_(e_తిరగేయి_range_లోపల_character_class));

			ఒకవేళ (endc > startc + 2)
			{
			    // Emit a range instead of the sequence of
			    // individual characters.
			    ఒకవేళ (startc == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    లేకపోతే
				--post_ptr; // remove NFA_CONCAT
			    EMIT(endc);
			    EMIT(NFA_RANGE);
			    EMIT(NFA_CONCAT);
			}
			లేకపోతే ఒకవేళ (has_mbyte && ((*mb_char2len)(startc) > 1
				    || (*mb_char2len)(endc) > 1))
			{
			    // Emit the characters in the range.
			    // "startc" was already emitted, so skip it.
			    //
			    కొరకు (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			లేకపోతే
			{
			    // Emit the range. "startc" was already emitted, so
			    // skip it.
			    కొరకు (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			emit_range = FALSE;
			startc = -1;
		    }
		    లేకపోతే
		    {
			// This char (startc) is not part of a range. Just
			// emit it.
			// Normally, simply emit startc. But if we get char
			// code=0 from a collating char, then replace it with
			// 0x0a.
			// This is needed to completely mimic the behaviour of
			// the backtracking engine.
			ఒకవేళ (startc == NFA_NEWL)
			{
			    // Line break can't be matched as part of the
			    // collection, add an OR below. But not for negated
			    // range.
			    ఒకవేళ (!negated)
				extra = NFA_కూడు_NL;
			}
			లేకపోతే
			{
			    ఒకవేళ (got_coll_char == TRUE && startc == 0)
			    {
				EMIT(0x0a);
				EMIT(NFA_CONCAT);
			    }
			    లేకపోతే
			    {
				EMIT(startc);
				ఒకవేళ (!(enc_utf8 && (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse)))))
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
		    ఒకవేళ (!range_endpoint && enc_utf8 &&
			    (utf_ptr2len(regparse) != (plen = utfc_ptr2len(regparse))))
		    {
			పూర్ణాంకం i = utf_ptr2len(regparse);

			c = utf_ptr2char(regparse + i);

			// Add composing characters
			కొరకు (;;)
			{
			    ఒకవేళ (c == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    లేకపోతే
				EMIT(c);
			    EMIT(NFA_CONCAT);
			    ఒకవేళ ((i += utf_char2len(c)) >= plen)
				ఆపు;
			    c = utf_ptr2char(regparse + i);
			}
			EMIT(NFA_COMPOSING);
			EMIT(NFA_CONCAT);
		    }
		    MB_PTR_ADV(regparse);
		} // while (p < endp)

		MB_PTR_వెనుక(old_regparse, regparse);
		ఒకవేళ (*regparse == '-')	    // if last, '-' is just a char
		{
		    EMIT('-');
		    EMIT(NFA_CONCAT);
		}

		// skip the trailing ]
		regparse = endp;
		MB_PTR_ADV(regparse);

		// Mark end of the collection.
		ఒకవేళ (negated == TRUE)
		    EMIT(NFA_చివర_NEG_COLL);
		లేకపోతే
		    EMIT(NFA_చివర_COLL);

		// \_[] also matches \n but it's not negated
		ఒకవేళ (extra == NFA_కూడు_NL)
		{
		    EMIT(reg_string ? NL : NFA_NEWL);
		    EMIT(NFA_OR);
		}

		తిరిగి OK;
	    } // if exists closing ]

	    ఒకవేళ (reg_strict)
		EMSG_RET_FAIL(_(e_missing_rsb_after_str_lsb));
	    // FALLTHROUGH

	అప్రమేయ:
	    {
		పూర్ణాంకం	plen;

nfa_do_multibyte:
		// plen is length of current char with composing chars
		ఒకవేళ (enc_utf8 && ((*mb_char2len)(c)
			    != (plen = utfc_ptr2len(old_regparse))
						       || utf_iscomposing(c)))
		{
		    పూర్ణాంకం i = 0;

		    // A base character plus composing characters, or just one
		    // or more composing characters.
		    // This requires creating a separate atom as if enclosing
		    // the characters in (), where NFA_COMPOSING is the ( and
		    // NFA_END_COMPOSING is the ). Note that right now we are
		    // building the postfix form, not the NFA itself;
		    // a composing char could be: a, b, c, NFA_COMPOSING
		    // where 'b' and 'c' are chars with codes > 256.
		    కొరకు (;;)
		    {
			EMIT(c);
			ఒకవేళ (i > 0)
			    EMIT(NFA_CONCAT);
			ఒకవేళ ((i += utf_char2len(c)) >= plen)
			    ఆపు;
			c = utf_ptr2char(old_regparse + i);
		    }
		    EMIT(NFA_COMPOSING);
		    regparse = old_regparse + plen;
		}
		లేకపోతే
		{
		    c = no_Magic(c);
		    EMIT(c);
		}
		తిరిగి OK;
	    }
    }

    తిరిగి OK;
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
    స్థిర పూర్ణాంకం
nfa_regpiece(శూన్య)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		op;
    పూర్ణాంకం		ret;
    దీర్ఘ	minval, maxval;
    పూర్ణాంకం		greedy = TRUE;      // Braces are prefixed with '-' ?
    parse_state_T old_state;
    parse_state_T new_state;
    దీర్ఘ	c2;
    పూర్ణాంకం		old_post_pos;
    పూర్ణాంకం		my_post_ప్రారంభం;
    పూర్ణాంకం		quest;

    // Save the current parse state, so that we can use it if <atom>{m,n} is
    // next.
    save_parse_state(&old_state);

    // store current pos in the postfix form, for \{m,n} involving 0s
    my_post_ప్రారంభం = (పూర్ణాంకం)(post_ptr - post_ప్రారంభం);

    ret = nfa_regatom();
    ఒకవేళ (ret == FAIL)
	తిరిగి FAIL;	    // cascaded error

    op = peekchr();
    ఒకవేళ (re_multi_type(op) == NOT_MULTI)
	తిరిగి OK;

    skipchr();
    ఎంపిక (op)
    {
	స్థితి Magic('*'):
	    EMIT(NFA_నక్షత్రం);
	    ఆపు;

	స్థితి Magic('+'):
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
	    ఒకవేళ (nfa_regatom() == FAIL)
		తిరిగి FAIL;
	    EMIT(NFA_నక్షత్రం);
	    EMIT(NFA_CONCAT);
	    skipchr();		// skip the \+
	    ఆపు;

	స్థితి Magic('@'):
	    c2 = (దీర్ఘ)getdecchrs();
	    op = no_Magic(getchr());
	    i = 0;
	    ఎంపిక(op)
	    {
		స్థితి '=':
		    // \@=
		    i = NFA_PREV_ATOM_NO_వెడల్పు;
		    ఆపు;
		స్థితి '!':
		    // \@!
		    i = NFA_PREV_ATOM_NO_వెడల్పు_NEG;
		    ఆపు;
		స్థితి '<':
		    op = no_Magic(getchr());
		    ఒకవేళ (op == '=')
			// \@<=
			i = NFA_PREV_ATOM_JUST_BEFORE;
		    లేకపోతే ఒకవేళ (op == '!')
			// \@<!
			i = NFA_PREV_ATOM_JUST_BEFORE_NEG;
		    ఆపు;
		స్థితి '>':
		    // \@>
		    i = NFA_PREV_ATOM_LIKE_నమూనా;
		    ఆపు;
	    }
	    ఒకవేళ (i == 0)
	    {
		semsg(_(e_nfa_regexp_unknown_operator_at_chr), op);
		తిరిగి FAIL;
	    }
	    EMIT(i);
	    ఒకవేళ (i == NFA_PREV_ATOM_JUST_BEFORE
					|| i == NFA_PREV_ATOM_JUST_BEFORE_NEG)
		EMIT(c2);
	    ఆపు;

	స్థితి Magic('?'):
	స్థితి Magic('='):
	    EMIT(NFA_QUEST);
	    ఆపు;

	స్థితి Magic('{'):
	    // a{2,5} will expand to 'aaa?a?a?'
	    // a{-1,3} will expand to 'aa??a??', where ?? is the nongreedy
	    // version of '?'
	    // \v(ab){2,3} will expand to '(ab)(ab)(ab)?', where all the
	    // parenthesis have the same id

	    greedy = TRUE;
	    c2 = peekchr();
	    ఒకవేళ (c2 == '-' || c2 == Magic('-'))
	    {
		skipchr();
		greedy = FALSE;
	    }
	    ఒకవేళ (!read_limits(&minval, &maxval))
		EMSG_RET_FAIL(_(e_nfa_regexp_error_reading_repetition_limits));

	    //  <atom>{0,inf}, <atom>{0,} and <atom>{}  are equivalent to
	    //  <atom>*
	    ఒకవేళ (minval == 0 && maxval == MAX_హద్దు)
	    {
		ఒకవేళ (greedy)		// { { (match the braces)
		    // \{}, \{0,}
		    EMIT(NFA_నక్షత్రం);
		లేకపోతే			// { { (match the braces)
		    // \{-}, \{-0,}
		    EMIT(NFA_నక్షత్రం_NONGREEDY);
		ఆపు;
	    }

	    // Special case: x{0} or x{-0}
	    ఒకవేళ (maxval == 0)
	    {
		// Ignore result of previous call to nfa_regatom()
		post_ptr = post_ప్రారంభం + my_post_ప్రారంభం;
		// NFA_EMPTY is 0-length and works everywhere
		EMIT(NFA_ఖాళీలు);
		తిరిగి OK;
	    }

	    // The engine is very inefficient (uses too many states) when the
	    // maximum is much larger than the minimum and when the maximum is
	    // large.  However, when maxval is MAX_LIMIT, it is okay, as this
	    // will emit NFA_STAR.
	    // Bail out if we can use the other engine, but only, when the
	    // pattern does not need the NFA engine like (e.g. [[:upper:]]\{2,\}
	    // does not work with characters > 8 bit with the BT engine)
	    ఒకవేళ ((nfa_re_flags & RE_AUTO)
				   && (maxval > 500 || maxval > minval + 200)
				   && (maxval != MAX_హద్దు && minval < 200)
				   && !కోరుతుంది_nfa)
		తిరిగి FAIL;

	    // Ignore previous call to nfa_regatom()
	    post_ptr = post_ప్రారంభం + my_post_ప్రారంభం;
	    // Save parse state after the repeated atom and the \{}
	    save_parse_state(&new_state);

	    quest = (greedy == TRUE? NFA_QUEST : NFA_QUEST_NONGREEDY);
	    కొరకు (i = 0; i < maxval; i++)
	    {
		// Goto beginning of the repeated atom
		restore_parse_state(&old_state);
		old_post_pos = (పూర్ణాంకం)(post_ptr - post_ప్రారంభం);
		ఒకవేళ (nfa_regatom() == FAIL)
		    తిరిగి FAIL;
		// after "minval" times, atoms are optional
		ఒకవేళ (i + 1 > minval)
		{
		    ఒకవేళ (maxval == MAX_హద్దు)
		    {
			ఒకవేళ (greedy)
			    EMIT(NFA_నక్షత్రం);
			లేకపోతే
			    EMIT(NFA_నక్షత్రం_NONGREEDY);
		    }
		    లేకపోతే
			EMIT(quest);
		}
		ఒకవేళ (old_post_pos != my_post_ప్రారంభం)
		    EMIT(NFA_CONCAT);
		ఒకవేళ (i + 1 > minval && maxval == MAX_హద్దు)
		    ఆపు;
	    }

	    // Go to just after the repeated atom and the \{}
	    restore_parse_state(&new_state);
	    curchr = -1;

	    ఆపు;


	అప్రమేయ:
	    ఆపు;
    }	// end switch

    ఒకవేళ (re_multi_type(peekchr()) != NOT_MULTI)
	// Can't have a multi follow a multi.
	EMSG_RET_FAIL(_(e_nfa_regexp_cant_have_multi_follow_multi));

    తిరిగి OK;
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
    స్థిర పూర్ణాంకం
nfa_regconcat(శూన్య)
{
    పూర్ణాంకం		cont = TRUE;
    పూర్ణాంకం		మొదటి = TRUE;

    అయితే (cont)
    {
	ఎంపిక (peekchr())
	{
	    స్థితి NUL:
	    స్థితి Magic('|'):
	    స్థితి Magic('&'):
	    స్థితి Magic(')'):
		cont = FALSE;
		ఆపు;

	    స్థితి Magic('Z'):
		regflags |= RF_ICOMBINE;
		skipchr_keepstart();
		ఆపు;
	    స్థితి Magic('c'):
		regflags |= RF_ICASE;
		skipchr_keepstart();
		ఆపు;
	    స్థితి Magic('C'):
		regflags |= RF_NOICASE;
		skipchr_keepstart();
		ఆపు;
	    స్థితి Magic('v'):
		reg_magic = MAGIC_ALL;
		skipchr_keepstart();
		curchr = -1;
		ఆపు;
	    స్థితి Magic('m'):
		reg_magic = MAGIC_ON;
		skipchr_keepstart();
		curchr = -1;
		ఆపు;
	    స్థితి Magic('M'):
		reg_magic = MAGIC_OFF;
		skipchr_keepstart();
		curchr = -1;
		ఆపు;
	    స్థితి Magic('V'):
		reg_magic = MAGIC_NONE;
		skipchr_keepstart();
		curchr = -1;
		ఆపు;

	    అప్రమేయ:
		ఒకవేళ (nfa_regpiece() == FAIL)
		    తిరిగి FAIL;
		ఒకవేళ (మొదటి == FALSE)
		    EMIT(NFA_CONCAT);
		లేకపోతే
		    మొదటి = FALSE;
		ఆపు;
	}
    }

    తిరిగి OK;
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
    స్థిర పూర్ణాంకం
nfa_regbranch(శూన్య)
{
    పూర్ణాంకం		old_post_pos;

    old_post_pos = (పూర్ణాంకం)(post_ptr - post_ప్రారంభం);

    // First branch, possibly the only one
    ఒకవేళ (nfa_regconcat() == FAIL)
	తిరిగి FAIL;

    // Try next concats
    అయితే (peekchr() == Magic('&'))
    {
	skipchr();
	// if concat is empty do emit a node
	ఒకవేళ (old_post_pos == (పూర్ణాంకం)(post_ptr - post_ప్రారంభం))
	    EMIT(NFA_ఖాళీలు);
	EMIT(NFA_NOPEN);
	EMIT(NFA_PREV_ATOM_NO_వెడల్పు);
	old_post_pos = (పూర్ణాంకం)(post_ptr - post_ప్రారంభం);
	ఒకవేళ (nfa_regconcat() == FAIL)
	    తిరిగి FAIL;
	// if concat is empty do emit a node
	ఒకవేళ (old_post_pos == (పూర్ణాంకం)(post_ptr - post_ప్రారంభం))
	    EMIT(NFA_ఖాళీలు);
	EMIT(NFA_CONCAT);
    }

    // if a branch is empty, emit one node for it
    ఒకవేళ (old_post_pos == (పూర్ణాంకం)(post_ptr - post_ప్రారంభం))
	EMIT(NFA_ఖాళీలు);

    తిరిగి OK;
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
    స్థిర పూర్ణాంకం
nfa_reg(
    పూర్ణాంకం		paren)	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
{
    పూర్ణాంకం		parno = 0;
    పూర్ణాంకం		స్థితి_2 = FAIL;

    ఒకవేళ (paren == REG_PAREN)
    {
	ఒకవేళ (regnpar >= NSUBEXP) // Too many `('
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_parens));
	parno = regnpar++;
    }
#నిర్వచితమైతే FEAT_SYN_HL
    లేకపోతే ఒకవేళ (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	ఒకవేళ (regnzpar >= NSUBEXP)
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_z));
	parno = regnzpar++;
    }
#అయితే_ముగింపు

    ఒకవేళ (nfa_reg_parse_లోతు >= REG_MAX_PAREN_లోతు)
	EMSG_RET_FAIL(_(e_command_too_complex));
    ++nfa_reg_parse_లోతు;

    ఒకవేళ (nfa_regbranch() == FAIL)
	వెళ్ళు theend;	    // cascaded error

    అయితే (peekchr() == Magic('|'))
    {
	skipchr();
	ఒకవేళ (nfa_regbranch() == FAIL)
	    వెళ్ళు theend;    // cascaded error
	EMIT(NFA_OR);
    }

    // Check for proper termination.
    ఒకవేళ (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
	ఒకవేళ (paren == REG_NPAREN)
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	లేకపోతే
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	rc_did_emsg = TRUE;
	వెళ్ళు theend;
    }
    లేకపోతే ఒకవేళ (paren == REG_NOPAREN && peekchr() != NUL)
    {
	ఒకవేళ (peekchr() == Magic(')'))
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	లేకపోతే
	    emsg(_(e_nfa_regexp_proper_termination_error));
	rc_did_emsg = TRUE;
	వెళ్ళు theend;
    }
    /*
     * Here we set the flag allowing back references to this set of
     * parentheses.
     */
    ఒకవేళ (paren == REG_PAREN)
    {
	had_endbrace[parno] = TRUE;     // have seen the close paren
	EMIT(NFA_MOPEN + parno);
    }
#నిర్వచితమైతే FEAT_SYN_HL
    లేకపోతే ఒకవేళ (paren == REG_ZPAREN)
	EMIT(NFA_ZOPEN + parno);
#అయితే_ముగింపు

    స్థితి_2 = OK;

theend:
    --nfa_reg_parse_లోతు;
    తిరిగి స్థితి_2;
}

#నిర్వచితమైతే DEBUG
స్థిర char_u సంకేతం_2[50];

    స్థిర శూన్య
nfa_సమితి_సంకేతం(పూర్ణాంకం c)
{
    పూర్ణాంకం	    addnl = FALSE;

    ఒకవేళ (c >= NFA_మొదటి_NL && c <= NFA_చివరి_NL)
    {
	addnl = TRUE;
	c -= NFA_కూడు_NL;
    }

    STRCPY(సంకేతం_2, "");
    ఎంపిక (c)
    {
	స్థితి NFA_సరిపోలిక:	    STRCPY(సంకేతం_2, "NFA_MATCH "); ఆపు;
	స్థితి NFA_విభజించు:	    STRCPY(సంకేతం_2, "NFA_SPLIT "); ఆపు;
	స్థితి NFA_CONCAT:    STRCPY(సంకేతం_2, "NFA_CONCAT "); ఆపు;
	స్థితి NFA_NEWL:	    STRCPY(సంకేతం_2, "NFA_NEWL "); ఆపు;
	స్థితి NFA_ZSTART:    STRCPY(సంకేతం_2, "NFA_ZSTART"); ఆపు;
	స్థితి NFA_ZEND:	    STRCPY(సంకేతం_2, "NFA_ZEND"); ఆపు;

	స్థితి NFA_BACKREF1:  STRCPY(సంకేతం_2, "NFA_BACKREF1"); ఆపు;
	స్థితి NFA_BACKREF2:  STRCPY(సంకేతం_2, "NFA_BACKREF2"); ఆపు;
	స్థితి NFA_BACKREF3:  STRCPY(సంకేతం_2, "NFA_BACKREF3"); ఆపు;
	స్థితి NFA_BACKREF4:  STRCPY(సంకేతం_2, "NFA_BACKREF4"); ఆపు;
	స్థితి NFA_BACKREF5:  STRCPY(సంకేతం_2, "NFA_BACKREF5"); ఆపు;
	స్థితి NFA_BACKREF6:  STRCPY(సంకేతం_2, "NFA_BACKREF6"); ఆపు;
	స్థితి NFA_BACKREF7:  STRCPY(సంకేతం_2, "NFA_BACKREF7"); ఆపు;
	స్థితి NFA_BACKREF8:  STRCPY(సంకేతం_2, "NFA_BACKREF8"); ఆపు;
	స్థితి NFA_BACKREF9:  STRCPY(సంకేతం_2, "NFA_BACKREF9"); ఆపు;
# ifdef FEAT_SYN_HL
	స్థితి NFA_ZREF1:	    STRCPY(సంకేతం_2, "NFA_ZREF1"); ఆపు;
	స్థితి NFA_ZREF2:	    STRCPY(సంకేతం_2, "NFA_ZREF2"); ఆపు;
	స్థితి NFA_ZREF3:	    STRCPY(సంకేతం_2, "NFA_ZREF3"); ఆపు;
	స్థితి NFA_ZREF4:	    STRCPY(సంకేతం_2, "NFA_ZREF4"); ఆపు;
	స్థితి NFA_ZREF5:	    STRCPY(సంకేతం_2, "NFA_ZREF5"); ఆపు;
	స్థితి NFA_ZREF6:	    STRCPY(సంకేతం_2, "NFA_ZREF6"); ఆపు;
	స్థితి NFA_ZREF7:	    STRCPY(సంకేతం_2, "NFA_ZREF7"); ఆపు;
	స్థితి NFA_ZREF8:	    STRCPY(సంకేతం_2, "NFA_ZREF8"); ఆపు;
	స్థితి NFA_ZREF9:	    STRCPY(సంకేతం_2, "NFA_ZREF9"); ఆపు;
# endif
	స్థితి NFA_SKIP:	    STRCPY(సంకేతం_2, "NFA_SKIP"); ఆపు;

	స్థితి NFA_PREV_ATOM_NO_వెడల్పు:
			    STRCPY(సంకేతం_2, "NFA_PREV_ATOM_NO_WIDTH"); ఆపు;
	స్థితి NFA_PREV_ATOM_NO_వెడల్పు_NEG:
			    STRCPY(సంకేతం_2, "NFA_PREV_ATOM_NO_WIDTH_NEG"); ఆపు;
	స్థితి NFA_PREV_ATOM_JUST_BEFORE:
			    STRCPY(సంకేతం_2, "NFA_PREV_ATOM_JUST_BEFORE"); ఆపు;
	స్థితి NFA_PREV_ATOM_JUST_BEFORE_NEG:
			 STRCPY(సంకేతం_2, "NFA_PREV_ATOM_JUST_BEFORE_NEG"); ఆపు;
	స్థితి NFA_PREV_ATOM_LIKE_నమూనా:
			    STRCPY(సంకేతం_2, "NFA_PREV_ATOM_LIKE_PATTERN"); ఆపు;

	స్థితి NFA_NOPEN:		    STRCPY(సంకేతం_2, "NFA_NOPEN"); ఆపు;
	స్థితి NFA_NCLOSE:	    STRCPY(సంకేతం_2, "NFA_NCLOSE"); ఆపు;
	స్థితి NFA_ప్రారంభం_INVISIBLE:   STRCPY(సంకేతం_2, "NFA_START_INVISIBLE"); ఆపు;
	స్థితి NFA_ప్రారంభం_INVISIBLE_మొదటి:
			     STRCPY(సంకేతం_2, "NFA_START_INVISIBLE_FIRST"); ఆపు;
	స్థితి NFA_ప్రారంభం_INVISIBLE_NEG:
			       STRCPY(సంకేతం_2, "NFA_START_INVISIBLE_NEG"); ఆపు;
	స్థితి NFA_ప్రారంభం_INVISIBLE_NEG_మొదటి:
			 STRCPY(సంకేతం_2, "NFA_START_INVISIBLE_NEG_FIRST"); ఆపు;
	స్థితి NFA_ప్రారంభం_INVISIBLE_BEFORE:
			    STRCPY(సంకేతం_2, "NFA_START_INVISIBLE_BEFORE"); ఆపు;
	స్థితి NFA_ప్రారంభం_INVISIBLE_BEFORE_మొదటి:
		      STRCPY(సంకేతం_2, "NFA_START_INVISIBLE_BEFORE_FIRST"); ఆపు;
	స్థితి NFA_ప్రారంభం_INVISIBLE_BEFORE_NEG:
			STRCPY(సంకేతం_2, "NFA_START_INVISIBLE_BEFORE_NEG"); ఆపు;
	స్థితి NFA_ప్రారంభం_INVISIBLE_BEFORE_NEG_మొదటి:
		  STRCPY(సంకేతం_2, "NFA_START_INVISIBLE_BEFORE_NEG_FIRST"); ఆపు;
	స్థితి NFA_ప్రారంభం_నమూనా:   STRCPY(సంకేతం_2, "NFA_START_PATTERN"); ఆపు;
	స్థితి NFA_చివర_INVISIBLE:	    STRCPY(సంకేతం_2, "NFA_END_INVISIBLE"); ఆపు;
	స్థితి NFA_చివర_INVISIBLE_NEG: STRCPY(సంకేతం_2, "NFA_END_INVISIBLE_NEG"); ఆపు;
	స్థితి NFA_చివర_నమూనా:	    STRCPY(సంకేతం_2, "NFA_END_PATTERN"); ఆపు;

	స్థితి NFA_COMPOSING:	    STRCPY(సంకేతం_2, "NFA_COMPOSING"); ఆపు;
	స్థితి NFA_చివర_COMPOSING:	    STRCPY(సంకేతం_2, "NFA_END_COMPOSING"); ఆపు;
	స్థితి NFA_OPT_CHARS:	    STRCPY(సంకేతం_2, "NFA_OPT_CHARS"); ఆపు;

	స్థితి NFA_MOPEN:
	స్థితి NFA_MOPEN1:
	స్థితి NFA_MOPEN2:
	స్థితి NFA_MOPEN3:
	స్థితి NFA_MOPEN4:
	స్థితి NFA_MOPEN5:
	స్థితి NFA_MOPEN6:
	స్థితి NFA_MOPEN7:
	స్థితి NFA_MOPEN8:
	స్థితి NFA_MOPEN9:
	    STRCPY(సంకేతం_2, "NFA_MOPEN(x)");
	    సంకేతం_2[10] = c - NFA_MOPEN + '0';
	    ఆపు;
	స్థితి NFA_MCLOSE:
	స్థితి NFA_MCLOSE1:
	స్థితి NFA_MCLOSE2:
	స్థితి NFA_MCLOSE3:
	స్థితి NFA_MCLOSE4:
	స్థితి NFA_MCLOSE5:
	స్థితి NFA_MCLOSE6:
	స్థితి NFA_MCLOSE7:
	స్థితి NFA_MCLOSE8:
	స్థితి NFA_MCLOSE9:
	    STRCPY(సంకేతం_2, "NFA_MCLOSE(x)");
	    సంకేతం_2[11] = c - NFA_MCLOSE + '0';
	    ఆపు;
# ifdef FEAT_SYN_HL
	స్థితి NFA_ZOPEN:
	స్థితి NFA_ZOPEN1:
	స్థితి NFA_ZOPEN2:
	స్థితి NFA_ZOPEN3:
	స్థితి NFA_ZOPEN4:
	స్థితి NFA_ZOPEN5:
	స్థితి NFA_ZOPEN6:
	స్థితి NFA_ZOPEN7:
	స్థితి NFA_ZOPEN8:
	స్థితి NFA_ZOPEN9:
	    STRCPY(సంకేతం_2, "NFA_ZOPEN(x)");
	    సంకేతం_2[10] = c - NFA_ZOPEN + '0';
	    ఆపు;
	స్థితి NFA_ZCLOSE:
	స్థితి NFA_ZCLOSE1:
	స్థితి NFA_ZCLOSE2:
	స్థితి NFA_ZCLOSE3:
	స్థితి NFA_ZCLOSE4:
	స్థితి NFA_ZCLOSE5:
	స్థితి NFA_ZCLOSE6:
	స్థితి NFA_ZCLOSE7:
	స్థితి NFA_ZCLOSE8:
	స్థితి NFA_ZCLOSE9:
	    STRCPY(సంకేతం_2, "NFA_ZCLOSE(x)");
	    సంకేతం_2[11] = c - NFA_ZCLOSE + '0';
	    ఆపు;
# endif
	స్థితి NFA_EOL:		STRCPY(సంకేతం_2, "NFA_EOL "); ఆపు;
	స్థితి NFA_BOL:		STRCPY(సంకేతం_2, "NFA_BOL "); ఆపు;
	స్థితి NFA_EOW:		STRCPY(సంకేతం_2, "NFA_EOW "); ఆపు;
	స్థితి NFA_BOW:		STRCPY(సంకేతం_2, "NFA_BOW "); ఆపు;
	స్థితి NFA_EOF:		STRCPY(సంకేతం_2, "NFA_EOF "); ఆపు;
	స్థితి NFA_BOF:		STRCPY(సంకేతం_2, "NFA_BOF "); ఆపు;
	స్థితి NFA_LNUM:		STRCPY(సంకేతం_2, "NFA_LNUM "); ఆపు;
	స్థితి NFA_LNUM_GT:	STRCPY(సంకేతం_2, "NFA_LNUM_GT "); ఆపు;
	స్థితి NFA_LNUM_LT:	STRCPY(సంకేతం_2, "NFA_LNUM_LT "); ఆపు;
	స్థితి NFA_నిలువు:		STRCPY(సంకేతం_2, "NFA_COL "); ఆపు;
	స్థితి NFA_నిలువు_GT:	STRCPY(సంకేతం_2, "NFA_COL_GT "); ఆపు;
	స్థితి NFA_నిలువు_LT:	STRCPY(సంకేతం_2, "NFA_COL_LT "); ఆపు;
	స్థితి NFA_VCOL:		STRCPY(సంకేతం_2, "NFA_VCOL "); ఆపు;
	స్థితి NFA_VCOL_GT:	STRCPY(సంకేతం_2, "NFA_VCOL_GT "); ఆపు;
	స్థితి NFA_VCOL_LT:	STRCPY(సంకేతం_2, "NFA_VCOL_LT "); ఆపు;
	స్థితి NFA_MARK:		STRCPY(సంకేతం_2, "NFA_MARK "); ఆపు;
	స్థితి NFA_MARK_GT:	STRCPY(సంకేతం_2, "NFA_MARK_GT "); ఆపు;
	స్థితి NFA_MARK_LT:	STRCPY(సంకేతం_2, "NFA_MARK_LT "); ఆపు;
	స్థితి NFA_CURSOR:	STRCPY(సంకేతం_2, "NFA_CURSOR "); ఆపు;
	స్థితి NFA_VISUAL:	STRCPY(సంకేతం_2, "NFA_VISUAL "); ఆపు;
	స్థితి NFA_ANY_COMPOSING:	STRCPY(సంకేతం_2, "NFA_ANY_COMPOSING "); ఆపు;

	స్థితి NFA_నక్షత్రం:		STRCPY(సంకేతం_2, "NFA_STAR "); ఆపు;
	స్థితి NFA_నక్షత్రం_NONGREEDY: STRCPY(సంకేతం_2, "NFA_STAR_NONGREEDY "); ఆపు;
	స్థితి NFA_QUEST:		STRCPY(సంకేతం_2, "NFA_QUEST"); ఆపు;
	స్థితి NFA_QUEST_NONGREEDY: STRCPY(సంకేతం_2, "NFA_QUEST_NON_GREEDY"); ఆపు;
	స్థితి NFA_ఖాళీలు:		STRCPY(సంకేతం_2, "NFA_EMPTY"); ఆపు;
	స్థితి NFA_OR:		STRCPY(సంకేతం_2, "NFA_OR"); ఆపు;

	స్థితి NFA_ప్రారంభం_COLL:	STRCPY(సంకేతం_2, "NFA_START_COLL"); ఆపు;
	స్థితి NFA_చివర_COLL:	STRCPY(సంకేతం_2, "NFA_END_COLL"); ఆపు;
	స్థితి NFA_ప్రారంభం_NEG_COLL: STRCPY(సంకేతం_2, "NFA_START_NEG_COLL"); ఆపు;
	స్థితి NFA_చివర_NEG_COLL:	STRCPY(సంకేతం_2, "NFA_END_NEG_COLL"); ఆపు;
	స్థితి NFA_RANGE:		STRCPY(సంకేతం_2, "NFA_RANGE"); ఆపు;
	స్థితి NFA_RANGE_MIN:	STRCPY(సంకేతం_2, "NFA_RANGE_MIN"); ఆపు;
	స్థితి NFA_RANGE_MAX:	STRCPY(సంకేతం_2, "NFA_RANGE_MAX"); ఆపు;

	స్థితి NFA_CLASS_ALNUM:	STRCPY(సంకేతం_2, "NFA_CLASS_ALNUM"); ఆపు;
	స్థితి NFA_CLASS_ALPHA:	STRCPY(సంకేతం_2, "NFA_CLASS_ALPHA"); ఆపు;
	స్థితి NFA_CLASS_BLANK:	STRCPY(సంకేతం_2, "NFA_CLASS_BLANK"); ఆపు;
	స్థితి NFA_CLASS_CNTRL:	STRCPY(సంకేతం_2, "NFA_CLASS_CNTRL"); ఆపు;
	స్థితి NFA_CLASS_DIGIT:	STRCPY(సంకేతం_2, "NFA_CLASS_DIGIT"); ఆపు;
	స్థితి NFA_CLASS_లేఖాచిత్రం:	STRCPY(సంకేతం_2, "NFA_CLASS_GRAPH"); ఆపు;
	స్థితి NFA_CLASS_క్రింది:	STRCPY(సంకేతం_2, "NFA_CLASS_LOWER"); ఆపు;
	స్థితి NFA_CLASS_PRINT:	STRCPY(సంకేతం_2, "NFA_CLASS_PRINT"); ఆపు;
	స్థితి NFA_CLASS_PUNCT:	STRCPY(సంకేతం_2, "NFA_CLASS_PUNCT"); ఆపు;
	స్థితి NFA_CLASS_SPACE:	STRCPY(సంకేతం_2, "NFA_CLASS_SPACE"); ఆపు;
	స్థితి NFA_CLASS_పై:	STRCPY(సంకేతం_2, "NFA_CLASS_UPPER"); ఆపు;
	స్థితి NFA_CLASS_XDIGIT:	STRCPY(సంకేతం_2, "NFA_CLASS_XDIGIT"); ఆపు;
	స్థితి NFA_CLASS_TAB:	STRCPY(సంకేతం_2, "NFA_CLASS_TAB"); ఆపు;
	స్థితి NFA_CLASS_RETURN:	STRCPY(సంకేతం_2, "NFA_CLASS_RETURN"); ఆపు;
	స్థితి NFA_CLASS_BACKSPACE:   STRCPY(సంకేతం_2, "NFA_CLASS_BACKSPACE"); ఆపు;
	స్థితి NFA_CLASS_ESCAPE:	STRCPY(సంకేతం_2, "NFA_CLASS_ESCAPE"); ఆపు;
	స్థితి NFA_CLASS_IDENT:	STRCPY(సంకేతం_2, "NFA_CLASS_IDENT"); ఆపు;
	స్థితి NFA_CLASS_KEYWORD:	STRCPY(సంకేతం_2, "NFA_CLASS_KEYWORD"); ఆపు;
	స్థితి NFA_CLASS_FNAME:	STRCPY(సంకేతం_2, "NFA_CLASS_FNAME"); ఆపు;

	స్థితి NFA_ANY:	STRCPY(సంకేతం_2, "NFA_ANY"); ఆపు;
	స్థితి NFA_IDENT:	STRCPY(సంకేతం_2, "NFA_IDENT"); ఆపు;
	స్థితి NFA_SIDENT:STRCPY(సంకేతం_2, "NFA_SIDENT"); ఆపు;
	స్థితి NFA_KWORD:	STRCPY(సంకేతం_2, "NFA_KWORD"); ఆపు;
	స్థితి NFA_SKWORD:STRCPY(సంకేతం_2, "NFA_SKWORD"); ఆపు;
	స్థితి NFA_FNAME:	STRCPY(సంకేతం_2, "NFA_FNAME"); ఆపు;
	స్థితి NFA_SFNAME:STRCPY(సంకేతం_2, "NFA_SFNAME"); ఆపు;
	స్థితి NFA_PRINT:	STRCPY(సంకేతం_2, "NFA_PRINT"); ఆపు;
	స్థితి NFA_SPRINT:STRCPY(సంకేతం_2, "NFA_SPRINT"); ఆపు;
	స్థితి NFA_WHITE:	STRCPY(సంకేతం_2, "NFA_WHITE"); ఆపు;
	స్థితి NFA_NWHITE:STRCPY(సంకేతం_2, "NFA_NWHITE"); ఆపు;
	స్థితి NFA_DIGIT:	STRCPY(సంకేతం_2, "NFA_DIGIT"); ఆపు;
	స్థితి NFA_NDIGIT:STRCPY(సంకేతం_2, "NFA_NDIGIT"); ఆపు;
	స్థితి NFA_HEX:	STRCPY(సంకేతం_2, "NFA_HEX"); ఆపు;
	స్థితి NFA_NHEX:	STRCPY(సంకేతం_2, "NFA_NHEX"); ఆపు;
	స్థితి NFA_OCTAL:	STRCPY(సంకేతం_2, "NFA_OCTAL"); ఆపు;
	స్థితి NFA_NOCTAL:STRCPY(సంకేతం_2, "NFA_NOCTAL"); ఆపు;
	స్థితి NFA_మాట:	STRCPY(సంకేతం_2, "NFA_WORD"); ఆపు;
	స్థితి NFA_NWORD:	STRCPY(సంకేతం_2, "NFA_NWORD"); ఆపు;
	స్థితి NFA_తల:	STRCPY(సంకేతం_2, "NFA_HEAD"); ఆపు;
	స్థితి NFA_NHEAD:	STRCPY(సంకేతం_2, "NFA_NHEAD"); ఆపు;
	స్థితి NFA_ALPHA:	STRCPY(సంకేతం_2, "NFA_ALPHA"); ఆపు;
	స్థితి NFA_NALPHA:STRCPY(సంకేతం_2, "NFA_NALPHA"); ఆపు;
	స్థితి NFA_క్రింది:	STRCPY(సంకేతం_2, "NFA_LOWER"); ఆపు;
	స్థితి NFA_NLOWER:STRCPY(సంకేతం_2, "NFA_NLOWER"); ఆపు;
	స్థితి NFA_పై:	STRCPY(సంకేతం_2, "NFA_UPPER"); ఆపు;
	స్థితి NFA_NUPPER:STRCPY(సంకేతం_2, "NFA_NUPPER"); ఆపు;
	స్థితి NFA_క్రింది_IC:  STRCPY(సంకేతం_2, "NFA_LOWER_IC"); ఆపు;
	స్థితి NFA_NLOWER_IC: STRCPY(సంకేతం_2, "NFA_NLOWER_IC"); ఆపు;
	స్థితి NFA_పై_IC:  STRCPY(సంకేతం_2, "NFA_UPPER_IC"); ఆపు;
	స్థితి NFA_NUPPER_IC: STRCPY(సంకేతం_2, "NFA_NUPPER_IC"); ఆపు;

	అప్రమేయ:
	    STRCPY(సంకేతం_2, "CHAR(x)");
	    సంకేతం_2[5] = c;
    }

    ఒకవేళ (addnl == TRUE)
	STRCAT(సంకేతం_2, " + NEWLINE ");

}

# ifdef ENABLE_LOG
స్థిర FILE *log_fd;
స్థిర char_u e_log_open_failed[] = N_("Could not open temporary log file for writing, displaying on stderr... ");

/*
 * Print the postfix notation of the current regexp.
 */
    స్థిర శూన్య
nfa_postfix_dump(char_u *సమాసం, పూర్ణాంకం retval)
{
    పూర్ణాంకం *p;
    FILE *f;

    f = fopen(NFA_REGEXP_DUMP_LOG, "a");
    ఒకవేళ (f == ఖాళీ)
	తిరిగి;

    fprintf(f, "\n-------------------------\n");
    ఒకవేళ (retval == FAIL)
	fprintf(f, ">>> NFA engine failed... \n");
    లేకపోతే ఒకవేళ (retval == OK)
	fprintf(f, ">>> NFA engine succeeded !\n");
    fprintf(f, "Regexp: \"%s\"\nPostfix notation (char): \"", సమాసం);
    కొరకు (p = post_ప్రారంభం; *p && p < post_ptr; p++)
    {
	nfa_సమితి_సంకేతం(*p);
	fprintf(f, "%s, ", సంకేతం_2);
    }
    fprintf(f, "\"\nPostfix notation (int): ");
    కొరకు (p = post_ప్రారంభం; *p && p < post_ptr; p++)
	fprintf(f, "%d ", *p);
    fprintf(f, "\n\n");
    fclose(f);
}

/*
 * Print the NFA starting with a root node "state".
 */
    స్థిర శూన్య
nfa_print_state(FILE *debugf, nfa_state_T *state)
{
    garray_T indent;

    ga_init2(&indent, 1, 64);
    ga_append(&indent, '\0');
    nfa_print_state2(debugf, state, &indent);
    ga_clear(&indent);
}

    స్థిర శూన్య
nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent)
{
    char_u  *p;

    ఒకవేళ (state == ఖాళీ)
	తిరిగి;

    fprintf(debugf, "(%2d)", నిరపేక్ష(state->id));

    // Output indent
    p = (char_u *)indent->ga_దత్తాంశం;
    ఒకవేళ (indent->ga_len >= 3)
    {
	పూర్ణాంకం	చివరి = indent->ga_len - 3;
	char_u	save[2];

	STRNCPY(save, &p[చివరి], 2);
	జ్ఞాపకం_నకలు(&p[చివరి], "+-", 2);
	fprintf(debugf, " %s", p);
	STRNCPY(&p[చివరి], save, 2);
    }
    లేకపోతే
	fprintf(debugf, " %s", p);

    nfa_సమితి_సంకేతం(state->c);
    fprintf(debugf, "%s (%d) (id=%d) val=%d\n",
		 సంకేతం_2,
		 state->c,
		 నిరపేక్ష(state->id),
		 state->val);
    ఒకవేళ (state->id < 0)
	తిరిగి;

    state->id = నిరపేక్ష(state->id) * -1;

    // grow indent for state->out
    indent->ga_len -= 1;
    ఒకవేళ (state->out1)
	GA_CONCAT_LITERAL(indent, "| ");
    లేకపోతే
	GA_CONCAT_LITERAL(indent, "  ");
    ga_append(indent, NUL);

    nfa_print_state2(debugf, state->బహిర్గతం, indent);

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
    స్థిర శూన్య
nfa_dump(nfa_regprog_T *prog)
{
    FILE *debugf = fopen(NFA_REGEXP_DUMP_LOG, "a");

    ఒకవేళ (debugf == ఖాళీ)
	తిరిగి;

    nfa_print_state(debugf, prog->ప్రారంభం);

    ఒకవేళ (prog->reganch)
	fprintf(debugf, "reganch: %d\n", prog->reganch);
    ఒకవేళ (prog->regstart != NUL)
	fprintf(debugf, "regstart: %c (decimal: %d)\n",
		prog->regstart, prog->regstart);
    ఒకవేళ (prog->సరిపోలిక_పాఠ్యం != ఖాళీ)
	fprintf(debugf, "match_text: \"%s\"\n", prog->సరిపోలిక_పాఠ్యం);

    fclose(debugf);
}
# endif	    // ENABLE_LOG
#అయితే_ముగింపు	    // DEBUG

/*
 * Parse r.e. @expr and convert it into postfix form.
 * Return the postfix string on success, NULL otherwise.
 */
    స్థిర పూర్ణాంకం *
re2post(శూన్య)
{
    ఒకవేళ (nfa_reg(REG_NOPAREN) == FAIL)
	తిరిగి ఖాళీ;
    EMIT(NFA_MOPEN);
    తిరిగి post_ప్రారంభం;
}

// NB. Some of the code below is inspired by Russ's.

/*
 * Represents an NFA state plus zero or one or two arrows exiting.
 * if c == MATCH, no arrows out; matching state.
 * If c == SPLIT, unlabeled arrows to out and out1 (if != NULL).
 * If c < 256, labeled arrow with character c to out.
 */

స్థిర nfa_state_T	*state_ptr; // points to nfa_prog->state

/*
 * Allocate and initialize nfa_state_T.
 */
    స్థిర nfa_state_T *
alloc_state(పూర్ణాంకం c, nfa_state_T *బహిర్గతం, nfa_state_T *out1)
{
    nfa_state_T *s;

    ఒకవేళ (istate >= nstate)
	తిరిగి ఖాళీ;

    s = &state_ptr[istate++];

    s->c    = c;
    s->బహిర్గతం  = బహిర్గతం;
    s->out1 = out1;
    s->val  = 0;

    s->id   = istate;
    s->lastlist[0] = 0;
    s->lastlist[1] = 0;

    తిరిగి s;
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
రకనిర్వచనం సంఘం Ptrlist Ptrlist;
సంఘం Ptrlist
{
    Ptrlist	*తదుపరి;
    nfa_state_T	*s;
};

నిర్మాణం Frag
{
    nfa_state_T *ప్రారంభం;
    Ptrlist	*బహిర్గతం;
};
రకనిర్వచనం నిర్మాణం Frag Frag_T;

// Reused across compilations by post2nfa(); freed in free_regexp_stuff().
స్థిర Frag_T *nfa_దొంతర;
స్థిర పూర్ణాంకం nfa_దొంతర_len;

/*
 * Initialize a Frag_T struct and return it.
 */
    స్థిర Frag_T
frag(nfa_state_T *ప్రారంభం, Ptrlist *బహిర్గతం)
{
    Frag_T n;

    n.ప్రారంభం = ప్రారంభం;
    n.బహిర్గతం = బహిర్గతం;
    తిరిగి n;
}

/*
 * Create singleton list containing just outp.
 */
    స్థిర Ptrlist *
list1(
    nfa_state_T	**outp)
{
    Ptrlist *l;

    l = (Ptrlist *)outp;
    l->తదుపరి = ఖాళీ;
    తిరిగి l;
}

/*
 * Patch the list of states at out to point to start.
 */
    స్థిర శూన్య
patch(Ptrlist *l, nfa_state_T *s)
{
    Ptrlist *తదుపరి;

    కొరకు (; l; l = తదుపరి)
    {
	తదుపరి = l->తదుపరి;
	l->s = s;
    }
}


/*
 * Join the two lists l1 and l2, returning the combination.
 */
    స్థిర Ptrlist *
append(Ptrlist *l1, Ptrlist *l2)
{
    Ptrlist *oldl1;

    oldl1 = l1;
    అయితే (l1->తదుపరి)
	l1 = l1->తదుపరి;
    l1->తదుపరి = l2;
    తిరిగి oldl1;
}

/*
 * Stack used for transforming postfix form into NFA.
 */
స్థిర Frag_T ఖాళీలు_2;

    స్థిర శూన్య
st_error(పూర్ణాంకం *postfix వాడని, పూర్ణాంకం *చివర వాడని, పూర్ణాంకం *p వాడని)
{
#నిర్వచితమైతే NFA_REGEXP_ERROR_LOG
    FILE *df;
    పూర్ణాంకం *p2;

    df = fopen(NFA_REGEXP_ERROR_LOG, "a");
    ఒకవేళ (df)
    {
	fprintf(df, "Error popping the stack!\n");
# ifdef DEBUG
	fprintf(df, "Current regexp is \"%s\"\n", nfa_regengine.సమాసం);
# endif
	fprintf(df, "Postfix form is: ");
# ifdef DEBUG
	కొరకు (p2 = postfix; p2 < చివర; p2++)
	{
	    nfa_సమితి_సంకేతం(*p2);
	    fprintf(df, "%s, ", సంకేతం_2);
	}
	nfa_సమితి_సంకేతం(*p);
	fprintf(df, "\nCurrent position is: ");
	కొరకు (p2 = postfix; p2 <= p; p2 ++)
	{
	    nfa_సమితి_సంకేతం(*p2);
	    fprintf(df, "%s, ", సంకేతం_2);
	}
# లేకపోతే
	కొరకు (p2 = postfix; p2 < చివర; p2++)
	    fprintf(df, "%d, ", *p2);
	fprintf(df, "\nCurrent position is: ");
	కొరకు (p2 = postfix; p2 <= p; p2 ++)
	    fprintf(df, "%d, ", *p2);
# endif
	fprintf(df, "\n--------------------------\n");
	fclose(df);
    }
#అయితే_ముగింపు
    emsg(_(e_nfa_regexp_could_not_తీయి_దొంతర));
}

/*
 * Push an item onto the stack.
 */
    స్థిర శూన్య
st_నెట్టు(Frag_T s, Frag_T **p, Frag_T *దొంతర_చివర)
{
    Frag_T *stackp = *p;

    ఒకవేళ (stackp >= దొంతర_చివర)
	తిరిగి;
    *stackp = s;
    *p = *p + 1;
}

/*
 * Pop an item from the stack.
 */
    స్థిర Frag_T
st_తీయి(Frag_T **p, Frag_T *దొంతర)
{
    Frag_T *stackp;

    *p = *p - 1;
    stackp = *p;
    ఒకవేళ (stackp < దొంతర)
	తిరిగి ఖాళీలు_2;
    తిరిగి **p;
}

/*
 * Estimate the maximum byte length of anything matching "state".
 * When unknown or unlimited return -1.
 */
    స్థిర పూర్ణాంకం
nfa_max_వెడల్పు(nfa_state_T *startstate, పూర్ణాంకం లోతు)
{
    పూర్ణాంకం		    l, r;
    nfa_state_T	    *state = startstate;
    పూర్ణాంకం		    len = 0;

    // detect looping in a NFA_SPLIT
    ఒకవేళ (లోతు > 4)
	తిరిగి -1;

    అయితే (state != ఖాళీ)
    {
	ఎంపిక (state->c)
	{
	    స్థితి NFA_చివర_INVISIBLE:
	    స్థితి NFA_చివర_INVISIBLE_NEG:
		// the end, return what we have
		తిరిగి len;

	    స్థితి NFA_విభజించు:
		// two alternatives, use the maximum
		l = nfa_max_వెడల్పు(state->బహిర్గతం, లోతు + 1);
		r = nfa_max_వెడల్పు(state->out1, లోతు + 1);
		ఒకవేళ (l < 0 || r < 0)
		    తిరిగి -1;
		తిరిగి len + (l > r ? l : r);

	    స్థితి NFA_ANY:
	    స్థితి NFA_ప్రారంభం_COLL:
	    స్థితి NFA_ప్రారంభం_NEG_COLL:
		// matches some character, including composing chars
		ఒకవేళ (enc_utf8)
		    len += MB_MAXBYTES;
		లేకపోతే ఒకవేళ (has_mbyte)
		    len += 2;
		లేకపోతే
		    ++len;
		ఒకవేళ (state->c != NFA_ANY)
		{
		    // Skip over the compiled collection.
		    // malformed NFAs must not crash width estimation.
		    ఒకవేళ (state->out1 == ఖాళీ || state->out1->బహిర్గతం == ఖాళీ)
			తిరిగి -1;
		    state = state->out1->బహిర్గతం;
		    కొనసాగు;
		}
		ఆపు;

	    స్థితి NFA_DIGIT:
	    స్థితి NFA_WHITE:
	    స్థితి NFA_HEX:
	    స్థితి NFA_OCTAL:
		// ascii
		++len;
		ఆపు;

	    స్థితి NFA_IDENT:
	    స్థితి NFA_SIDENT:
	    స్థితి NFA_KWORD:
	    స్థితి NFA_SKWORD:
	    స్థితి NFA_FNAME:
	    స్థితి NFA_SFNAME:
	    స్థితి NFA_PRINT:
	    స్థితి NFA_SPRINT:
	    స్థితి NFA_NWHITE:
	    స్థితి NFA_NDIGIT:
	    స్థితి NFA_NHEX:
	    స్థితి NFA_NOCTAL:
	    స్థితి NFA_మాట:
	    స్థితి NFA_NWORD:
	    స్థితి NFA_తల:
	    స్థితి NFA_NHEAD:
	    స్థితి NFA_ALPHA:
	    స్థితి NFA_NALPHA:
	    స్థితి NFA_క్రింది:
	    స్థితి NFA_NLOWER:
	    స్థితి NFA_పై:
	    స్థితి NFA_NUPPER:
	    స్థితి NFA_క్రింది_IC:
	    స్థితి NFA_NLOWER_IC:
	    స్థితి NFA_పై_IC:
	    స్థితి NFA_NUPPER_IC:
	    స్థితి NFA_ANY_COMPOSING:
		// possibly non-ascii
		ఒకవేళ (has_mbyte)
		    len += 3;
		లేకపోతే
		    ++len;
		ఆపు;

	    స్థితి NFA_ప్రారంభం_INVISIBLE:
	    స్థితి NFA_ప్రారంభం_INVISIBLE_NEG:
	    స్థితి NFA_ప్రారంభం_INVISIBLE_BEFORE:
	    స్థితి NFA_ప్రారంభం_INVISIBLE_BEFORE_NEG:
		// zero-width, out1 points to the END state
		state = state->out1->బహిర్గతం;
		కొనసాగు;

	    స్థితి NFA_BACKREF1:
	    స్థితి NFA_BACKREF2:
	    స్థితి NFA_BACKREF3:
	    స్థితి NFA_BACKREF4:
	    స్థితి NFA_BACKREF5:
	    స్థితి NFA_BACKREF6:
	    స్థితి NFA_BACKREF7:
	    స్థితి NFA_BACKREF8:
	    స్థితి NFA_BACKREF9:
#నిర్వచితమైతే FEAT_SYN_HL
	    స్థితి NFA_ZREF1:
	    స్థితి NFA_ZREF2:
	    స్థితి NFA_ZREF3:
	    స్థితి NFA_ZREF4:
	    స్థితి NFA_ZREF5:
	    స్థితి NFA_ZREF6:
	    స్థితి NFA_ZREF7:
	    స్థితి NFA_ZREF8:
	    స్థితి NFA_ZREF9:
#అయితే_ముగింపు
	    స్థితి NFA_NEWL:
	    స్థితి NFA_SKIP:
		// unknown width
		తిరిగి -1;

	    స్థితి NFA_BOL:
	    స్థితి NFA_EOL:
	    స్థితి NFA_BOF:
	    స్థితి NFA_EOF:
	    స్థితి NFA_BOW:
	    స్థితి NFA_EOW:
	    స్థితి NFA_MOPEN:
	    స్థితి NFA_MOPEN1:
	    స్థితి NFA_MOPEN2:
	    స్థితి NFA_MOPEN3:
	    స్థితి NFA_MOPEN4:
	    స్థితి NFA_MOPEN5:
	    స్థితి NFA_MOPEN6:
	    స్థితి NFA_MOPEN7:
	    స్థితి NFA_MOPEN8:
	    స్థితి NFA_MOPEN9:
#నిర్వచితమైతే FEAT_SYN_HL
	    స్థితి NFA_ZOPEN:
	    స్థితి NFA_ZOPEN1:
	    స్థితి NFA_ZOPEN2:
	    స్థితి NFA_ZOPEN3:
	    స్థితి NFA_ZOPEN4:
	    స్థితి NFA_ZOPEN5:
	    స్థితి NFA_ZOPEN6:
	    స్థితి NFA_ZOPEN7:
	    స్థితి NFA_ZOPEN8:
	    స్థితి NFA_ZOPEN9:
	    స్థితి NFA_ZCLOSE:
	    స్థితి NFA_ZCLOSE1:
	    స్థితి NFA_ZCLOSE2:
	    స్థితి NFA_ZCLOSE3:
	    స్థితి NFA_ZCLOSE4:
	    స్థితి NFA_ZCLOSE5:
	    స్థితి NFA_ZCLOSE6:
	    స్థితి NFA_ZCLOSE7:
	    స్థితి NFA_ZCLOSE8:
	    స్థితి NFA_ZCLOSE9:
#అయితే_ముగింపు
	    స్థితి NFA_MCLOSE:
	    స్థితి NFA_MCLOSE1:
	    స్థితి NFA_MCLOSE2:
	    స్థితి NFA_MCLOSE3:
	    స్థితి NFA_MCLOSE4:
	    స్థితి NFA_MCLOSE5:
	    స్థితి NFA_MCLOSE6:
	    స్థితి NFA_MCLOSE7:
	    స్థితి NFA_MCLOSE8:
	    స్థితి NFA_MCLOSE9:
	    స్థితి NFA_NOPEN:
	    స్థితి NFA_NCLOSE:

	    స్థితి NFA_LNUM_GT:
	    స్థితి NFA_LNUM_LT:
	    స్థితి NFA_నిలువు_GT:
	    స్థితి NFA_నిలువు_LT:
	    స్థితి NFA_VCOL_GT:
	    స్థితి NFA_VCOL_LT:
	    స్థితి NFA_MARK_GT:
	    స్థితి NFA_MARK_LT:
	    స్థితి NFA_VISUAL:
	    స్థితి NFA_LNUM:
	    స్థితి NFA_CURSOR:
	    స్థితి NFA_నిలువు:
	    స్థితి NFA_VCOL:
	    స్థితి NFA_MARK:

	    స్థితి NFA_ZSTART:
	    స్థితి NFA_ZEND:
	    స్థితి NFA_OPT_CHARS:
	    స్థితి NFA_ఖాళీలు:
	    స్థితి NFA_ప్రారంభం_నమూనా:
	    స్థితి NFA_చివర_నమూనా:
	    స్థితి NFA_COMPOSING:
	    స్థితి NFA_చివర_COMPOSING:
		// zero-width
		ఆపు;

	    అప్రమేయ:
		ఒకవేళ (state->c < 0)
		    // don't know what this is
		    తిరిగి -1;
		// normal character
		len += MB_CHAR2LEN(state->c);
		ఆపు;
	}

	// normal way to continue
	state = state->బహిర్గతం;
    }

    // unrecognized, "cannot happen"
    తిరిగి -1;
}

/*
 * Convert a postfix form into its equivalent NFA.
 * Return the NFA start state on success, NULL otherwise.
 */
    స్థిర nfa_state_T *
post2nfa(పూర్ణాంకం *postfix, పూర్ణాంకం *చివర, పూర్ణాంకం nfa_calc_కొలత)
{
    పూర్ణాంకం		*p;
    పూర్ణాంకం		mopen;
    పూర్ణాంకం		mclose;
    Frag_T	*దొంతర = ఖాళీ;
    Frag_T	*stackp = ఖాళీ;
    Frag_T	*దొంతర_చివర = ఖాళీ;
    Frag_T	e1;
    Frag_T	e2;
    Frag_T	e;
    nfa_state_T	*s;
    nfa_state_T	*s1;
    nfa_state_T	*matchstate;
    nfa_state_T	*ret = ఖాళీ;

    ఒకవేళ (postfix == ఖాళీ)
	తిరిగి ఖాళీ;

#నిర్వచించు నెట్టు_2(s)	    st_నెట్టు((s), &stackp, దొంతర_చివర)
#నిర్వచించు తీయి_2()	    st_తీయి(&stackp, దొంతర);		\
		    ఒకవేళ (stackp < దొంతర)			\
		    {					\
			st_error(postfix, చివర, p);	\
			తిరిగి ఖాళీ;			\
		    }

    ఒకవేళ (nfa_calc_కొలత == FALSE)
    {
	// Reuse the fragment stack across compilations, growing when needed;
	// it is freed in free_regexp_stuff().
	ఒకవేళ (nfa_దొంతర == ఖాళీ || nfa_దొంతర_len < nstate + 1)
	{
	    vim_free(nfa_దొంతర);
	    nfa_దొంతర = ALLOC_MULT(Frag_T, nstate + 1);
	    ఒకవేళ (nfa_దొంతర == ఖాళీ)
	    {
		nfa_దొంతర_len = 0;
		తిరిగి ఖాళీ;
	    }
	    nfa_దొంతర_len = nstate + 1;
	}
	దొంతర = nfa_దొంతర;
	stackp = దొంతర;
	దొంతర_చివర = దొంతర + (nstate + 1);
    }

    కొరకు (p = postfix; p < చివర; ++p)
    {
	ఎంపిక (*p)
	{
	స్థితి NFA_CONCAT:
	    // Concatenation.
	    // Pay attention: this operator does not exist in the r.e. itself
	    // (it is implicit, really).  It is added when r.e. is translated
	    // to postfix form in re2post().
	    ఒకవేళ (nfa_calc_కొలత == TRUE)
	    {
		// nstate += 0;
		ఆపు;
	    }
	    e2 = తీయి_2();
	    e1 = తీయి_2();
	    patch(e1.బహిర్గతం, e2.ప్రారంభం);
	    నెట్టు_2(frag(e1.ప్రారంభం, e2.బహిర్గతం));
	    ఆపు;

	స్థితి NFA_OR:
	    // Alternation
	    ఒకవేళ (nfa_calc_కొలత == TRUE)
	    {
		nstate++;
		ఆపు;
	    }
	    e2 = తీయి_2();
	    e1 = తీయి_2();
	    s = alloc_state(NFA_విభజించు, e1.ప్రారంభం, e2.ప్రారంభం);
	    ఒకవేళ (s == ఖాళీ)
		వెళ్ళు theend;
	    నెట్టు_2(frag(s, append(e1.బహిర్గతం, e2.బహిర్గతం)));
	    ఆపు;

	స్థితి NFA_నక్షత్రం:
	    // Zero or more, prefer more
	    ఒకవేళ (nfa_calc_కొలత == TRUE)
	    {
		nstate++;
		ఆపు;
	    }
	    e = తీయి_2();
	    s = alloc_state(NFA_విభజించు, e.ప్రారంభం, ఖాళీ);
	    ఒకవేళ (s == ఖాళీ)
		వెళ్ళు theend;
	    patch(e.బహిర్గతం, s);
	    నెట్టు_2(frag(s, list1(&s->out1)));
	    ఆపు;

	స్థితి NFA_నక్షత్రం_NONGREEDY:
	    // Zero or more, prefer zero
	    ఒకవేళ (nfa_calc_కొలత == TRUE)
	    {
		nstate++;
		ఆపు;
	    }
	    e = తీయి_2();
	    s = alloc_state(NFA_విభజించు, ఖాళీ, e.ప్రారంభం);
	    ఒకవేళ (s == ఖాళీ)
		వెళ్ళు theend;
	    patch(e.బహిర్గతం, s);
	    నెట్టు_2(frag(s, list1(&s->బహిర్గతం)));
	    ఆపు;

	స్థితి NFA_QUEST:
	    // one or zero atoms=> greedy match
	    ఒకవేళ (nfa_calc_కొలత == TRUE)
	    {
		nstate++;
		ఆపు;
	    }
	    e = తీయి_2();
	    s = alloc_state(NFA_విభజించు, e.ప్రారంభం, ఖాళీ);
	    ఒకవేళ (s == ఖాళీ)
		వెళ్ళు theend;
	    నెట్టు_2(frag(s, append(e.బహిర్గతం, list1(&s->out1))));
	    ఆపు;

	స్థితి NFA_QUEST_NONGREEDY:
	    // zero or one atoms => non-greedy match
	    ఒకవేళ (nfa_calc_కొలత == TRUE)
	    {
		nstate++;
		ఆపు;
	    }
	    e = తీయి_2();
	    s = alloc_state(NFA_విభజించు, ఖాళీ, e.ప్రారంభం);
	    ఒకవేళ (s == ఖాళీ)
		వెళ్ళు theend;
	    నెట్టు_2(frag(s, append(e.బహిర్గతం, list1(&s->బహిర్గతం))));
	    ఆపు;

	స్థితి NFA_చివర_COLL:
	స్థితి NFA_చివర_NEG_COLL:
	    // On the stack is the sequence starting with NFA_START_COLL or
	    // NFA_START_NEG_COLL and all possible characters. Patch it to
	    // add the output to the start.
	    ఒకవేళ (nfa_calc_కొలత == TRUE)
	    {
		nstate++;
		ఆపు;
	    }
	    e = తీయి_2();
	    s = alloc_state(NFA_చివర_COLL, ఖాళీ, ఖాళీ);
	    ఒకవేళ (s == ఖాళీ)
		వెళ్ళు theend;
	    patch(e.బహిర్గతం, s);
	    e.ప్రారంభం->out1 = s;
	    నెట్టు_2(frag(e.ప్రారంభం, list1(&s->బహిర్గతం)));
	    ఆపు;

	స్థితి NFA_RANGE:
	    // Before this are two characters, the low and high end of a
	    // range.  Turn them into two states with MIN and MAX.
	    ఒకవేళ (nfa_calc_కొలత == TRUE)
	    {
		// nstate += 0;
		ఆపు;
	    }
	    e2 = తీయి_2();
	    e1 = తీయి_2();
	    e2.ప్రారంభం->val = e2.ప్రారంభం->c;
	    e2.ప్రారంభం->c = NFA_RANGE_MAX;
	    e1.ప్రారంభం->val = e1.ప్రారంభం->c;
	    e1.ప్రారంభం->c = NFA_RANGE_MIN;
	    patch(e1.బహిర్గతం, e2.ప్రారంభం);
	    నెట్టు_2(frag(e1.ప్రారంభం, e2.బహిర్గతం));
	    ఆపు;

	స్థితి NFA_ఖాళీలు:
	    // 0-length, used in a repetition with max/min count of 0
	    ఒకవేళ (nfa_calc_కొలత == TRUE)
	    {
		nstate++;
		ఆపు;
	    }
	    s = alloc_state(NFA_ఖాళీలు, ఖాళీ, ఖాళీ);
	    ఒకవేళ (s == ఖాళీ)
		వెళ్ళు theend;
	    నెట్టు_2(frag(s, list1(&s->బహిర్గతం)));
	    ఆపు;

	స్థితి NFA_OPT_CHARS:
	  {
	    పూర్ణాంకం    n;

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
	    ఒకవేళ (nfa_calc_కొలత == TRUE)
	    {
		nstate += n;
		ఆపు;
	    }
	    s = ఖాళీ; // avoid compiler warning
	    e1.బహిర్గతం = ఖాళీ; // stores list with out1's
	    s1 = ఖాళీ; // previous NFA_SPLIT to connect to
	    అయితే (n-- > 0)
	    {
		e = తీయి_2(); // get character
		s = alloc_state(NFA_విభజించు, e.ప్రారంభం, ఖాళీ);
		ఒకవేళ (s == ఖాళీ)
		    వెళ్ళు theend;
		ఒకవేళ (e1.బహిర్గతం == ఖాళీ)
		    e1 = e;
		patch(e.బహిర్గతం, s1);
		append(e1.బహిర్గతం, list1(&s->out1));
		s1 = s;
	    }
	    నెట్టు_2(frag(s, e1.బహిర్గతం));
	    ఆపు;
	  }

	స్థితి NFA_PREV_ATOM_NO_వెడల్పు:
	స్థితి NFA_PREV_ATOM_NO_వెడల్పు_NEG:
	స్థితి NFA_PREV_ATOM_JUST_BEFORE:
	స్థితి NFA_PREV_ATOM_JUST_BEFORE_NEG:
	స్థితి NFA_PREV_ATOM_LIKE_నమూనా:
	  {
	    పూర్ణాంకం before = (*p == NFA_PREV_ATOM_JUST_BEFORE
				      || *p == NFA_PREV_ATOM_JUST_BEFORE_NEG);
	    పూర్ణాంకం నమూనా = (*p == NFA_PREV_ATOM_LIKE_నమూనా);
	    పూర్ణాంకం ప్రారంభం_state;
	    పూర్ణాంకం చివర_state;
	    పూర్ణాంకం n = 0;
	    nfa_state_T *zend;
	    nfa_state_T *skip;

	    ఎంపిక (*p)
	    {
		స్థితి NFA_PREV_ATOM_NO_వెడల్పు:
		    ప్రారంభం_state = NFA_ప్రారంభం_INVISIBLE;
		    చివర_state = NFA_చివర_INVISIBLE;
		    ఆపు;
		స్థితి NFA_PREV_ATOM_NO_వెడల్పు_NEG:
		    ప్రారంభం_state = NFA_ప్రారంభం_INVISIBLE_NEG;
		    చివర_state = NFA_చివర_INVISIBLE_NEG;
		    ఆపు;
		స్థితి NFA_PREV_ATOM_JUST_BEFORE:
		    ప్రారంభం_state = NFA_ప్రారంభం_INVISIBLE_BEFORE;
		    చివర_state = NFA_చివర_INVISIBLE;
		    ఆపు;
		స్థితి NFA_PREV_ATOM_JUST_BEFORE_NEG:
		    ప్రారంభం_state = NFA_ప్రారంభం_INVISIBLE_BEFORE_NEG;
		    చివర_state = NFA_చివర_INVISIBLE_NEG;
		    ఆపు;
		అప్రమేయ: // NFA_PREV_ATOM_LIKE_PATTERN:
		    ప్రారంభం_state = NFA_ప్రారంభం_నమూనా;
		    చివర_state = NFA_చివర_నమూనా;
		    ఆపు;
	    }

	    ఒకవేళ (before)
		n = *++p; // get the count

	    // The \@= operator: match the preceding atom with zero width.
	    // The \@! operator: no match for the preceding atom.
	    // The \@<= operator: match for the preceding atom.
	    // The \@<! operator: no match for the preceding atom.
	    // Surrounds the preceding atom with START_INVISIBLE and
	    // END_INVISIBLE, similarly to MOPEN.

	    ఒకవేళ (nfa_calc_కొలత == TRUE)
	    {
		nstate += నమూనా ? 4 : 2;
		ఆపు;
	    }
	    e = తీయి_2();
	    s1 = alloc_state(చివర_state, ఖాళీ, ఖాళీ);
	    ఒకవేళ (s1 == ఖాళీ)
		వెళ్ళు theend;

	    s = alloc_state(ప్రారంభం_state, e.ప్రారంభం, s1);
	    ఒకవేళ (s == ఖాళీ)
		వెళ్ళు theend;
	    ఒకవేళ (నమూనా)
	    {
		// NFA_ZEND -> NFA_END_PATTERN -> NFA_SKIP -> what follows.
		skip = alloc_state(NFA_SKIP, ఖాళీ, ఖాళీ);
		ఒకవేళ (skip == ఖాళీ)
		    వెళ్ళు theend;
		zend = alloc_state(NFA_ZEND, s1, ఖాళీ);
		ఒకవేళ (zend == ఖాళీ)
		    వెళ్ళు theend;
		s1->బహిర్గతం= skip;
		patch(e.బహిర్గతం, zend);
		నెట్టు_2(frag(s, list1(&skip->బహిర్గతం)));
	    }
	    లేకపోతే
	    {
		patch(e.బహిర్గతం, s1);
		నెట్టు_2(frag(s, list1(&s1->బహిర్గతం)));
		ఒకవేళ (before)
		{
		    ఒకవేళ (n <= 0)
			// See if we can guess the maximum width, it avoids a
			// lot of pointless tries.
			n = nfa_max_వెడల్పు(e.ప్రారంభం, 0);
		    s->val = n; // store the count
		}
	    }
	    ఆపు;
	  }

	స్థితి NFA_COMPOSING:	// char with composing char
#ఒకవేళ 0
	    // TODO
	    ఒకవేళ (regflags & RF_ICOMBINE)
	    {
		// use the base character only
	    }
#అయితే_ముగింపు
	    // FALLTHROUGH

	స్థితి NFA_MOPEN:	// \( \) Submatch
	స్థితి NFA_MOPEN1:
	స్థితి NFA_MOPEN2:
	స్థితి NFA_MOPEN3:
	స్థితి NFA_MOPEN4:
	స్థితి NFA_MOPEN5:
	స్థితి NFA_MOPEN6:
	స్థితి NFA_MOPEN7:
	స్థితి NFA_MOPEN8:
	స్థితి NFA_MOPEN9:
#నిర్వచితమైతే FEAT_SYN_HL
	స్థితి NFA_ZOPEN:	// \z( \) Submatch
	స్థితి NFA_ZOPEN1:
	స్థితి NFA_ZOPEN2:
	స్థితి NFA_ZOPEN3:
	స్థితి NFA_ZOPEN4:
	స్థితి NFA_ZOPEN5:
	స్థితి NFA_ZOPEN6:
	స్థితి NFA_ZOPEN7:
	స్థితి NFA_ZOPEN8:
	స్థితి NFA_ZOPEN9:
#అయితే_ముగింపు
	స్థితి NFA_NOPEN:	// \%( \) "Invisible Submatch"
	    ఒకవేళ (nfa_calc_కొలత == TRUE)
	    {
		nstate += 2;
		ఆపు;
	    }

	    mopen = *p;
	    ఎంపిక (*p)
	    {
		స్థితి NFA_NOPEN: mclose = NFA_NCLOSE; ఆపు;
#నిర్వచితమైతే FEAT_SYN_HL
		స్థితి NFA_ZOPEN: mclose = NFA_ZCLOSE; ఆపు;
		స్థితి NFA_ZOPEN1: mclose = NFA_ZCLOSE1; ఆపు;
		స్థితి NFA_ZOPEN2: mclose = NFA_ZCLOSE2; ఆపు;
		స్థితి NFA_ZOPEN3: mclose = NFA_ZCLOSE3; ఆపు;
		స్థితి NFA_ZOPEN4: mclose = NFA_ZCLOSE4; ఆపు;
		స్థితి NFA_ZOPEN5: mclose = NFA_ZCLOSE5; ఆపు;
		స్థితి NFA_ZOPEN6: mclose = NFA_ZCLOSE6; ఆపు;
		స్థితి NFA_ZOPEN7: mclose = NFA_ZCLOSE7; ఆపు;
		స్థితి NFA_ZOPEN8: mclose = NFA_ZCLOSE8; ఆపు;
		స్థితి NFA_ZOPEN9: mclose = NFA_ZCLOSE9; ఆపు;
#అయితే_ముగింపు
		స్థితి NFA_COMPOSING: mclose = NFA_చివర_COMPOSING; ఆపు;
		అప్రమేయ:
		    // NFA_MOPEN, NFA_MOPEN1 .. NFA_MOPEN9
		    mclose = *p + NSUBEXP;
		    ఆపు;
	    }

	    // Allow "NFA_MOPEN" as a valid postfix representation for
	    // the empty regexp "". In this case, the NFA will be
	    // NFA_MOPEN -> NFA_MCLOSE. Note that this also allows
	    // empty groups of parenthesis, and empty mbyte chars
	    ఒకవేళ (stackp == దొంతర)
	    {
		s = alloc_state(mopen, ఖాళీ, ఖాళీ);
		ఒకవేళ (s == ఖాళీ)
		    వెళ్ళు theend;
		s1 = alloc_state(mclose, ఖాళీ, ఖాళీ);
		ఒకవేళ (s1 == ఖాళీ)
		    వెళ్ళు theend;
		patch(list1(&s->బహిర్గతం), s1);
		నెట్టు_2(frag(s, list1(&s1->బహిర్గతం)));
		ఆపు;
	    }

	    // At least one node was emitted before NFA_MOPEN, so
	    // at least one node will be between NFA_MOPEN and NFA_MCLOSE
	    e = తీయి_2();
	    s = alloc_state(mopen, e.ప్రారంభం, ఖాళీ);   // `('
	    ఒకవేళ (s == ఖాళీ)
		వెళ్ళు theend;

	    s1 = alloc_state(mclose, ఖాళీ, ఖాళీ);   // `)'
	    ఒకవేళ (s1 == ఖాళీ)
		వెళ్ళు theend;
	    patch(e.బహిర్గతం, s1);

	    ఒకవేళ (mopen == NFA_COMPOSING)
		// COMPOSING->out1 = END_COMPOSING
		patch(list1(&s->out1), s1);

	    నెట్టు_2(frag(s, list1(&s1->బహిర్గతం)));
	    ఆపు;

	స్థితి NFA_BACKREF1:
	స్థితి NFA_BACKREF2:
	స్థితి NFA_BACKREF3:
	స్థితి NFA_BACKREF4:
	స్థితి NFA_BACKREF5:
	స్థితి NFA_BACKREF6:
	స్థితి NFA_BACKREF7:
	స్థితి NFA_BACKREF8:
	స్థితి NFA_BACKREF9:
#నిర్వచితమైతే FEAT_SYN_HL
	స్థితి NFA_ZREF1:
	స్థితి NFA_ZREF2:
	స్థితి NFA_ZREF3:
	స్థితి NFA_ZREF4:
	స్థితి NFA_ZREF5:
	స్థితి NFA_ZREF6:
	స్థితి NFA_ZREF7:
	స్థితి NFA_ZREF8:
	స్థితి NFA_ZREF9:
#అయితే_ముగింపు
	    ఒకవేళ (nfa_calc_కొలత == TRUE)
	    {
		nstate += 2;
		ఆపు;
	    }
	    s = alloc_state(*p, ఖాళీ, ఖాళీ);
	    ఒకవేళ (s == ఖాళీ)
		వెళ్ళు theend;
	    s1 = alloc_state(NFA_SKIP, ఖాళీ, ఖాళీ);
	    ఒకవేళ (s1 == ఖాళీ)
		వెళ్ళు theend;
	    patch(list1(&s->బహిర్గతం), s1);
	    నెట్టు_2(frag(s, list1(&s1->బహిర్గతం)));
	    ఆపు;

	స్థితి NFA_LNUM:
	స్థితి NFA_LNUM_GT:
	స్థితి NFA_LNUM_LT:
	స్థితి NFA_VCOL:
	స్థితి NFA_VCOL_GT:
	స్థితి NFA_VCOL_LT:
	స్థితి NFA_నిలువు:
	స్థితి NFA_నిలువు_GT:
	స్థితి NFA_నిలువు_LT:
	స్థితి NFA_MARK:
	స్థితి NFA_MARK_GT:
	స్థితి NFA_MARK_LT:
	  {
	    పూర్ణాంకం n = *++p; // lnum, col or mark name

	    ఒకవేళ (nfa_calc_కొలత == TRUE)
	    {
		nstate += 1;
		ఆపు;
	    }
	    s = alloc_state(p[-1], ఖాళీ, ఖాళీ);
	    ఒకవేళ (s == ఖాళీ)
		వెళ్ళు theend;
	    s->val = n;
	    నెట్టు_2(frag(s, list1(&s->బహిర్గతం)));
	    ఆపు;
	  }

	స్థితి NFA_ZSTART:
	స్థితి NFA_ZEND:
	అప్రమేయ:
	    // Operands
	    ఒకవేళ (nfa_calc_కొలత == TRUE)
	    {
		nstate++;
		ఆపు;
	    }
	    s = alloc_state(*p, ఖాళీ, ఖాళీ);
	    ఒకవేళ (s == ఖాళీ)
		వెళ్ళు theend;
	    నెట్టు_2(frag(s, list1(&s->బహిర్గతం)));
	    ఆపు;

	} // switch(*p)

    } // for (p = postfix; *p; ++p)

    ఒకవేళ (nfa_calc_కొలత == TRUE)
    {
	nstate++;
	వెళ్ళు theend;	// Return value when counting size is ignored anyway
    }

    e = తీయి_2();
    ఒకవేళ (stackp != దొంతర)
	EMSG_RET_NULL(_(e_nfa_regexp_while_converting_from_postfix_గమ్యం_nfa_too_many_stats_ఎడమ_on_దొంతర));

    ఒకవేళ (istate >= nstate)
	EMSG_RET_NULL(_(e_nfa_regexp_not_enough_space_గమ్యం_store_సంపూర్ణ_nfa));

    matchstate = &state_ptr[istate++]; // the match state
    matchstate->c = NFA_సరిపోలిక;
    matchstate->బహిర్గతం = matchstate->out1 = ఖాళీ;
    matchstate->id = 0;

    patch(e.బహిర్గతం, matchstate);
    ret = e.ప్రారంభం;

theend:
    తిరిగి ret;

#నిర్వచనం_రద్దు POP1
#నిర్వచనం_రద్దు PUSH1
#నిర్వచనం_రద్దు POP2
#నిర్వచనం_రద్దు PUSH2
#నిర్వచనం_రద్దు తీయి_2
#నిర్వచనం_రద్దు నెట్టు_2
}

/*
 * After building the NFA program, inspect it to add optimization hints.
 */
    స్థిర శూన్య
nfa_postprocess(nfa_regprog_T *prog)
{
    పూర్ణాంకం i;
    పూర్ణాంకం c;

    కొరకు (i = 0; i < prog->nstate; ++i)
    {
	c = prog->state[i].c;
	ఒకవేళ (c == NFA_ప్రారంభం_INVISIBLE
		|| c == NFA_ప్రారంభం_INVISIBLE_NEG
		|| c == NFA_ప్రారంభం_INVISIBLE_BEFORE
		|| c == NFA_ప్రారంభం_INVISIBLE_BEFORE_NEG)
	{
	    పూర్ణాంకం directly;

	    // Do it directly when what follows is possibly the end of the
	    // match.
	    ఒకవేళ (సరిపోలిక_follows(prog->state[i].out1->బహిర్గతం, 0))
		directly = TRUE;
	    లేకపోతే
	    {
		పూర్ణాంకం ch_invisible = failure_chance(prog->state[i].బహిర్గతం, 0);
		పూర్ణాంకం ch_follows = failure_chance(prog->state[i].out1->బహిర్గతం, 0);

		// Postpone when the invisible match is expensive or has a
		// lower chance of failing.
		ఒకవేళ (c == NFA_ప్రారంభం_INVISIBLE_BEFORE
		     || c == NFA_ప్రారంభం_INVISIBLE_BEFORE_NEG)
		{
		    // "before" matches are very expensive when
		    // unbounded, always prefer what follows then,
		    // unless what follows will always match.
		    // Otherwise strongly prefer what follows.
		    ఒకవేళ (prog->state[i].val <= 0 && ch_follows > 0)
			directly = FALSE;
		    లేకపోతే
			directly = ch_follows * 10 < ch_invisible;
		}
		లేకపోతే
		{
		    // normal invisible, first do the one with the
		    // highest failure chance
		    directly = ch_follows < ch_invisible;
		}
	    }
	    ఒకవేళ (directly)
		// switch to the _FIRST state
		++prog->state[i].c;
	}
    }
}

/////////////////////////////////////////////////////////////////
// NFA execution code.
/////////////////////////////////////////////////////////////////

రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం	    లోపల_use; // number of subexpr with useful info

    // When REG_MULTI is TRUE list.multi is used, otherwise list.line.
    సంఘం
    {
	నిర్మాణం multipos
	{
	    linenr_T	ప్రారంభం_lnum;
	    linenr_T	చివర_lnum;
	    colnr_T	ప్రారంభం_నిలువు;
	    colnr_T	చివర_నిలువు;
	} multi[NSUBEXP];
	నిర్మాణం linepos
	{
	    char_u	*ప్రారంభం;
	    char_u	*చివర;
	} పంక్తి[NSUBEXP];
    } list;
    colnr_T	orig_ప్రారంభం_నిలువు;  // list.multi[0].start_col without \zs
} regsub_T;

రకనిర్వచనం నిర్మాణం
{
    regsub_T	norm; // \( .. \) matches
#నిర్వచితమైతే FEAT_SYN_HL
    regsub_T	synt; // \z( .. \) matches
#అయితే_ముగింపు
} regsubs_T;

// nfa_pim_T stores a Postponed Invisible Match.
రకనిర్వచనం నిర్మాణం nfa_pim_S nfa_pim_T;
నిర్మాణం nfa_pim_S
{
    పూర్ణాంకం		ఫలితం;		// NFA_PIM_*, see below
    nfa_state_T	*state;		// the invisible match start state
    regsubs_T	subs;		// submatch info, only party used
    సంఘం
    {
	lpos_T	pos;
	char_u	*ptr;
    } చివర;			// where the match must end
};

// Values for done in nfa_pim_T.
#నిర్వచించు NFA_PIM_వాడని   0	// pim not used
#నిర్వచించు NFA_PIM_TODO     1	// pim not done yet
#నిర్వచించు NFA_PIM_సరిపోలిక    2	// pim executed, matches
#నిర్వచించు NFA_PIM_NOMATCH  3	// pim executed, no match


// nfa_thread_T contains execution information of a NFA state
రకనిర్వచనం నిర్మాణం
{
    nfa_state_T	*state;
    పూర్ణాంకం		లెక్క;
    nfa_pim_T	pim;		// if pim.result != NFA_PIM_UNUSED: postponed
				// invisible match
    regsubs_T	subs;		// submatch info, only party used
} nfa_thread_T;

// nfa_list_T contains the alternative NFA execution states.
రకనిర్వచనం నిర్మాణం
{
    nfa_thread_T    *t;		// allocated array of states
    పూర్ణాంకం		    n;		// nr of states currently in "t"
    పూర్ణాంకం		    len;	// max nr of states in "t"
    పూర్ణాంకం		    id;		// ID of the list
    పూర్ణాంకం		    has_pim;	// TRUE when any state has a PIM
} nfa_list_T;

#నిర్వచితమైతే ENABLE_LOG
స్థిర శూన్య log_subexpr(regsub_T *sub);

    స్థిర శూన్య
log_subsexpr(regsubs_T *subs)
{
    log_subexpr(&subs->norm);
# ifdef FEAT_SYN_HL
    ఒకవేళ (rex.nfa_has_zsubexpr)
	log_subexpr(&subs->synt);
# endif
}

    స్థిర శూన్య
log_subexpr(regsub_T *sub)
{
    పూర్ణాంకం j;

    కొరకు (j = 0; j < sub->లోపల_use; j++)
	ఒకవేళ (REG_MULTI)
	    fprintf(log_fd,
		    "*** group %d, start: c=%d, l=%d, end: c=%d, l=%d\n",
		    j,
		    sub->list.multi[j].ప్రారంభం_నిలువు,
		    (పూర్ణాంకం)sub->list.multi[j].ప్రారంభం_lnum,
		    sub->list.multi[j].చివర_నిలువు,
		    (పూర్ణాంకం)sub->list.multi[j].చివర_lnum);
	లేకపోతే
	{
	    అక్షరం *s = (అక్షరం *)sub->list.పంక్తి[j].ప్రారంభం;
	    అక్షరం *e = (అక్షరం *)sub->list.పంక్తి[j].చివర;

	    fprintf(log_fd, "*** group %d, start: \"%s\", end: \"%s\"\n",
		    j,
		    s == ఖాళీ ? "NULL" : s,
		    e == ఖాళీ ? "NULL" : e);
	}
}

    స్థిర అక్షరం *
pim_info(nfa_pim_T *pim)
{
    స్థిర అక్షరం buf[30];

    ఒకవేళ (pim == ఖాళీ || pim->ఫలితం == NFA_PIM_వాడని)
	buf[0] = NUL;
    లేకపోతే
    {
	sprintf(buf, " PIM col %d", REG_MULTI ? (పూర్ణాంకం)pim->చివర.pos.నిలువు
		: (పూర్ణాంకం)(pim->చివర.ptr - rex.ఆదానం));
    }
    తిరిగి buf;
}

#అయితే_ముగింపు

// Used during execution: whether a match has been found.
స్థిర పూర్ణాంకం	    nfa_సరిపోలిక;
#నిర్వచితమైతే FEAT_RELTIME
స్థిర పూర్ణాంకం	   *nfa_timed_బహిర్గతం;
#అయితే_ముగింపు

స్థిర అంతఃవరుస శూన్య copy_sub(regsub_T *గమ్యం, regsub_T *from);
స్థిర పూర్ణాంకం pim_equal(nfa_pim_T *one, nfa_pim_T *two);

/*
 * Copy postponed invisible match info from "from" to "to".
 */
    స్థిర శూన్య
copy_pim(nfa_pim_T *గమ్యం, nfa_pim_T *from)
{
    గమ్యం->ఫలితం = from->ఫలితం;
    గమ్యం->state = from->state;
    copy_sub(&గమ్యం->subs.norm, &from->subs.norm);
#నిర్వచితమైతే FEAT_SYN_HL
    ఒకవేళ (rex.nfa_has_zsubexpr)
	copy_sub(&గమ్యం->subs.synt, &from->subs.synt);
#అయితే_ముగింపు
    గమ్యం->చివర = from->చివర;
}

    స్థిర శూన్య
clear_sub(regsub_T *sub)
{
    ఒకవేళ (REG_MULTI)
	// Use 0xff to set lnum to -1
	vim_memset(sub->list.multi, 0xff,
				  పరిమాణం(నిర్మాణం multipos) * rex.nfa_nsubexpr);
    లేకపోతే
	vim_memset(sub->list.పంక్తి, 0,
				   పరిమాణం(నిర్మాణం linepos) * rex.nfa_nsubexpr);
    sub->లోపల_use = 0;
}

/*
 * Copy the submatches from "from" to "to".
 */
    స్థిర అంతఃవరుస శూన్య
copy_sub(regsub_T *గమ్యం, regsub_T *from)
{
    గమ్యం->లోపల_use = from->లోపల_use;
    ఒకవేళ (from->లోపల_use <= 0)
	తిరిగి;

    // Copy the match start and end positions.  addstate() calls this for
    // every state it adds, so it is very hot.  Patterns without capturing
    // groups only use subexpression zero (the whole match), so special-case
    // "in_use == 1" with a direct assignment to avoid the mch_memmove() call.
    ఒకవేళ (REG_MULTI)
    {
	ఒకవేళ (from->లోపల_use == 1)
	    గమ్యం->list.multi[0] = from->list.multi[0];
	లేకపోతే
	    mch_memmove(&గమ్యం->list.multi[0],
		    &from->list.multi[0],
		    పరిమాణం(నిర్మాణం multipos) * from->లోపల_use);
	గమ్యం->orig_ప్రారంభం_నిలువు = from->orig_ప్రారంభం_నిలువు;
    }
    లేకపోతే
    {
	ఒకవేళ (from->లోపల_use == 1)
	    గమ్యం->list.పంక్తి[0] = from->list.పంక్తి[0];
	లేకపోతే
	    mch_memmove(&గమ్యం->list.పంక్తి[0],
		    &from->list.పంక్తి[0],
		    పరిమాణం(నిర్మాణం linepos) * from->లోపల_use);
    }
}

/*
 * Like copy_sub() but exclude the main match.
 */
    స్థిర శూన్య
copy_sub_off(regsub_T *గమ్యం, regsub_T *from)
{
    ఒకవేళ (గమ్యం->లోపల_use < from->లోపల_use)
	గమ్యం->లోపల_use = from->లోపల_use;
    ఒకవేళ (from->లోపల_use <= 1)
	తిరిగి;

    // Copy the match start and end positions.
    ఒకవేళ (REG_MULTI)
	mch_memmove(&గమ్యం->list.multi[1],
		&from->list.multi[1],
		పరిమాణం(నిర్మాణం multipos) * (from->లోపల_use - 1));
    లేకపోతే
	mch_memmove(&గమ్యం->list.పంక్తి[1],
		&from->list.పంక్తి[1],
		పరిమాణం(నిర్మాణం linepos) * (from->లోపల_use - 1));
}

/*
 * Like copy_sub() but only do the end of the main match if \ze is present.
 */
    స్థిర శూన్య
copy_ze_off(regsub_T *గమ్యం, regsub_T *from)
{
    ఒకవేళ (!rex.nfa_has_zend)
	తిరిగి;

    ఒకవేళ (REG_MULTI)
    {
	ఒకవేళ (from->list.multi[0].చివర_lnum >= 0)
	{
	    గమ్యం->list.multi[0].చివర_lnum = from->list.multi[0].చివర_lnum;
	    గమ్యం->list.multi[0].చివర_నిలువు = from->list.multi[0].చివర_నిలువు;
	}
    }
    లేకపోతే
    {
	ఒకవేళ (from->list.పంక్తి[0].చివర != ఖాళీ)
	    గమ్యం->list.పంక్తి[0].చివర = from->list.పంక్తి[0].చివర;
    }
}

/*
 * Return TRUE if "sub1" and "sub2" have the same start positions.
 * When using back-references also check the end position.
 */
    స్థిర పూర్ణాంకం
sub_equal(regsub_T *sub1, regsub_T *sub2)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		todo;
    linenr_T	s1;
    linenr_T	s2;
    char_u	*sp1;
    char_u	*sp2;

    todo = sub1->లోపల_use > sub2->లోపల_use ? sub1->లోపల_use : sub2->లోపల_use;
    ఒకవేళ (REG_MULTI)
    {
	కొరకు (i = 0; i < todo; ++i)
	{
	    ఒకవేళ (i < sub1->లోపల_use)
		s1 = sub1->list.multi[i].ప్రారంభం_lnum;
	    లేకపోతే
		s1 = -1;
	    ఒకవేళ (i < sub2->లోపల_use)
		s2 = sub2->list.multi[i].ప్రారంభం_lnum;
	    లేకపోతే
		s2 = -1;
	    ఒకవేళ (s1 != s2)
		తిరిగి FALSE;
	    ఒకవేళ (s1 != -1 && sub1->list.multi[i].ప్రారంభం_నిలువు
					     != sub2->list.multi[i].ప్రారంభం_నిలువు)
		తిరిగి FALSE;

	    ఒకవేళ (rex.nfa_has_backref)
	    {
		ఒకవేళ (i < sub1->లోపల_use)
		    s1 = sub1->list.multi[i].చివర_lnum;
		లేకపోతే
		    s1 = -1;
		ఒకవేళ (i < sub2->లోపల_use)
		    s2 = sub2->list.multi[i].చివర_lnum;
		లేకపోతే
		    s2 = -1;
		ఒకవేళ (s1 != s2)
		    తిరిగి FALSE;
		ఒకవేళ (s1 != -1 && sub1->list.multi[i].చివర_నిలువు
					       != sub2->list.multi[i].చివర_నిలువు)
		తిరిగి FALSE;
	    }
	}
    }
    లేకపోతే
    {
	కొరకు (i = 0; i < todo; ++i)
	{
	    ఒకవేళ (i < sub1->లోపల_use)
		sp1 = sub1->list.పంక్తి[i].ప్రారంభం;
	    లేకపోతే
		sp1 = ఖాళీ;
	    ఒకవేళ (i < sub2->లోపల_use)
		sp2 = sub2->list.పంక్తి[i].ప్రారంభం;
	    లేకపోతే
		sp2 = ఖాళీ;
	    ఒకవేళ (sp1 != sp2)
		తిరిగి FALSE;
	    ఒకవేళ (rex.nfa_has_backref)
	    {
		ఒకవేళ (i < sub1->లోపల_use)
		    sp1 = sub1->list.పంక్తి[i].చివర;
		లేకపోతే
		    sp1 = ఖాళీ;
		ఒకవేళ (i < sub2->లోపల_use)
		    sp2 = sub2->list.పంక్తి[i].చివర;
		లేకపోతే
		    sp2 = ఖాళీ;
		ఒకవేళ (sp1 != sp2)
		    తిరిగి FALSE;
	    }
	}
    }

    తిరిగి TRUE;
}

#నిర్వచితమైతే FEAT_RELTIME
/*
 * Check if we are past the time limit, if there is one.
 */
    స్థిర పూర్ణాంకం
nfa_did_సమయం_బహిర్గతం(శూన్య)
{
    ఒకవేళ (*timeout_flag)
    {
	ఒకవేళ (nfa_timed_బహిర్గతం != ఖాళీ)
	{
# ifdef FEAT_EVAL
	    ఒకవేళ (!*nfa_timed_బహిర్గతం)
		ch_log(ఖాళీ, "NFA regexp timed out");
# endif
	    *nfa_timed_బహిర్గతం = TRUE;
	}
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}
#అయితే_ముగింపు

#నిర్వచితమైతే ENABLE_LOG
    స్థిర శూన్య
open_debug_log(పూర్ణాంకం ఫలితం)
{
    log_fd = fopen(NFA_REGEXP_నిరంతరం_LOG, "a");
    ఒకవేళ (log_fd == ఖాళీ)
    {
	emsg(_(e_log_open_failed));
	log_fd = stderr;
    }

    fprintf(log_fd, "****************************\n");
    fprintf(log_fd, "FINISHED RUNNING nfa_regmatch() recursively\n");
    fprintf(log_fd, "MATCH = %s\n", ఫలితం == TRUE ? "OK" : ఫలితం == MAYBE
	    ? "MAYBE" : "FALSE");
    fprintf(log_fd, "****************************\n");
}

    స్థిర శూన్య
report_state(అక్షరం *action,
	     regsub_T *sub,
	     nfa_state_T *state,
	     పూర్ణాంకం lid,
	     nfa_pim_T *pim)
{
    పూర్ణాంకం నిలువు;

    ఒకవేళ (sub->లోపల_use <= 0)
	నిలువు = -1;
    లేకపోతే ఒకవేళ (REG_MULTI)
	నిలువు = sub->list.multi[0].ప్రారంభం_నిలువు;
    లేకపోతే
	నిలువు = (పూర్ణాంకం)(sub->list.పంక్తి[0].ప్రారంభం - rex.పంక్తి);
    nfa_సమితి_సంకేతం(state->c);
    ఒకవేళ (log_fd == ఖాళీ)
	open_debug_log(MAYBE);

    fprintf(log_fd, "> %s state %d to list %d. char %d: %s (start col %d)%s\n",
	    action, నిరపేక్ష(state->id), lid, state->c, సంకేతం_2, నిలువు,
	    pim_info(pim));
}
#అయితే_ముగింపు

/*
 * Return TRUE if the same state is already in list "l" with the same
 * positions as "subs".
 */
    స్థిర పూర్ణాంకం
has_state_with_pos(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim)	// postponed match or NULL
{
    nfa_thread_T	*thread;
    పూర్ణాంకం			i;

    కొరకు (i = 0; i < l->n; ++i)
    {
	thread = &l->t[i];
	ఒకవేళ (thread->state->id == state->id
		&& sub_equal(&thread->subs.norm, &subs->norm)
#నిర్వచితమైతే FEAT_SYN_HL
		&& (!rex.nfa_has_zsubexpr
				|| sub_equal(&thread->subs.synt, &subs->synt))
#అయితే_ముగింపు
		&& pim_equal(&thread->pim, pim))
	    తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Return TRUE if "one" and "two" are equal.  That includes when both are not
 * set.
 */
    స్థిర పూర్ణాంకం
pim_equal(nfa_pim_T *one, nfa_pim_T *two)
{
    పూర్ణాంకం one_వాడని = (one == ఖాళీ || one->ఫలితం == NFA_PIM_వాడని);
    పూర్ణాంకం two_వాడని = (two == ఖాళీ || two->ఫలితం == NFA_PIM_వాడని);

    ఒకవేళ (one_వాడని)
	// one is unused: equal when two is also unused
	తిరిగి two_వాడని;
    ఒకవేళ (two_వాడని)
	// one is used and two is not: not equal
	తిరిగి FALSE;
    // compare the state id
    ఒకవేళ (one->state->id != two->state->id)
	తిరిగి FALSE;
    // compare the position
    ఒకవేళ (REG_MULTI)
	తిరిగి one->చివర.pos.lnum == two->చివర.pos.lnum
	    && one->చివర.pos.నిలువు == two->చివర.pos.నిలువు;
    తిరిగి one->చివర.ptr == two->చివర.ptr;
}

/*
 * Return TRUE if "state" leads to a NFA_MATCH without advancing the input.
 */
    స్థిర పూర్ణాంకం
సరిపోలిక_follows(nfa_state_T *startstate, పూర్ణాంకం లోతు)
{
    nfa_state_T	    *state = startstate;

    // avoid too much recursion
    ఒకవేళ (లోతు > 10)
	తిరిగి FALSE;

    అయితే (state != ఖాళీ)
    {
	ఎంపిక (state->c)
	{
	    స్థితి NFA_సరిపోలిక:
	    స్థితి NFA_MCLOSE:
	    స్థితి NFA_చివర_INVISIBLE:
	    స్థితి NFA_చివర_INVISIBLE_NEG:
	    స్థితి NFA_చివర_నమూనా:
		తిరిగి TRUE;

	    స్థితి NFA_విభజించు:
		తిరిగి సరిపోలిక_follows(state->బహిర్గతం, లోతు + 1)
				     || సరిపోలిక_follows(state->out1, లోతు + 1);

	    స్థితి NFA_ప్రారంభం_INVISIBLE:
	    స్థితి NFA_ప్రారంభం_INVISIBLE_మొదటి:
	    స్థితి NFA_ప్రారంభం_INVISIBLE_BEFORE:
	    స్థితి NFA_ప్రారంభం_INVISIBLE_BEFORE_మొదటి:
	    స్థితి NFA_ప్రారంభం_INVISIBLE_NEG:
	    స్థితి NFA_ప్రారంభం_INVISIBLE_NEG_మొదటి:
	    స్థితి NFA_ప్రారంభం_INVISIBLE_BEFORE_NEG:
	    స్థితి NFA_ప్రారంభం_INVISIBLE_BEFORE_NEG_మొదటి:
	    స్థితి NFA_COMPOSING:
		// skip ahead to next state
		state = state->out1->బహిర్గతం;
		కొనసాగు;

	    స్థితి NFA_ANY:
	    స్థితి NFA_ANY_COMPOSING:
	    స్థితి NFA_IDENT:
	    స్థితి NFA_SIDENT:
	    స్థితి NFA_KWORD:
	    స్థితి NFA_SKWORD:
	    స్థితి NFA_FNAME:
	    స్థితి NFA_SFNAME:
	    స్థితి NFA_PRINT:
	    స్థితి NFA_SPRINT:
	    స్థితి NFA_WHITE:
	    స్థితి NFA_NWHITE:
	    స్థితి NFA_DIGIT:
	    స్థితి NFA_NDIGIT:
	    స్థితి NFA_HEX:
	    స్థితి NFA_NHEX:
	    స్థితి NFA_OCTAL:
	    స్థితి NFA_NOCTAL:
	    స్థితి NFA_మాట:
	    స్థితి NFA_NWORD:
	    స్థితి NFA_తల:
	    స్థితి NFA_NHEAD:
	    స్థితి NFA_ALPHA:
	    స్థితి NFA_NALPHA:
	    స్థితి NFA_క్రింది:
	    స్థితి NFA_NLOWER:
	    స్థితి NFA_పై:
	    స్థితి NFA_NUPPER:
	    స్థితి NFA_క్రింది_IC:
	    స్థితి NFA_NLOWER_IC:
	    స్థితి NFA_పై_IC:
	    స్థితి NFA_NUPPER_IC:
	    స్థితి NFA_ప్రారంభం_COLL:
	    స్థితి NFA_ప్రారంభం_NEG_COLL:
	    స్థితి NFA_NEWL:
		// state will advance input
		తిరిగి FALSE;

	    అప్రమేయ:
		ఒకవేళ (state->c > 0)
		    // state will advance input
		    తిరిగి FALSE;

		// Others: zero-width or possibly zero-width, might still find
		// a match at the same position, keep looking.
		ఆపు;
	}
	state = state->బహిర్గతం;
    }
    తిరిగి FALSE;
}


/*
 * Return TRUE if "state" is already in list "l".
 */
    స్థిర పూర్ణాంకం
state_లోపల_list(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs)	// pointers to subexpressions
{
    ఒకవేళ (state->lastlist[nfa_ll_సూచీ] == l->id)
    {
	ఒకవేళ (!rex.nfa_has_backref || has_state_with_pos(l, state, subs, ఖాళీ))
	    తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

// Offset used for "off" by addstate_here().
#నిర్వచించు ADDSTATE_ఇక్కడ_స్థానభ్రంశం 10

/*
 * Add "state" and possibly what follows to state list ".".
 * Returns "subs_arg", possibly copied into temp_subs.
 * Returns NULL when recursiveness is too deep or timed out.
 */
    స్థిర regsubs_T *
addstate(
    nfa_list_T		*l,	    // runtime state list
    nfa_state_T		*state,	    // state to update
    regsubs_T		*subs_arg,  // pointers to subexpressions
    nfa_pim_T		*pim,	    // postponed look-behind match
    పూర్ణాంకం			off_arg,    // byte offset, when -1 go to next line
    పూర్ణాంకం			లోతు)	    // recursion depth
{
    పూర్ణాంకం			subidx;
    పూర్ణాంకం			off = off_arg;
    పూర్ణాంకం			కూడు_ఇక్కడ = FALSE;
    పూర్ణాంకం			listindex = 0;
    పూర్ణాంకం			k;
    పూర్ణాంకం			దొరికింది = FALSE;
    nfa_thread_T	*thread;
    నిర్మాణం multipos	save_multipos;
    పూర్ణాంకం			save_లోపల_use;
    char_u		*save_ptr;
    పూర్ణాంకం			i;
    regsub_T		*sub;
    regsubs_T		*subs = subs_arg;
    స్థిర regsubs_T	తాత్కాలిక_subs;
#నిర్వచితమైతే ENABLE_LOG
    పూర్ణాంకం			did_print = FALSE;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_RELTIME
    ఒకవేళ (nfa_did_సమయం_బహిర్గతం())
	తిరిగి ఖాళీ;
#అయితే_ముగింపు

    // This function is called recursively.  When the depth is too much we run
    // out of stack and crash, limit recursiveness here.
    ఒకవేళ (లోతు >= 5000 || subs == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (off_arg <= -ADDSTATE_ఇక్కడ_స్థానభ్రంశం)
    {
	కూడు_ఇక్కడ = TRUE;
	off = 0;
	listindex = -(off_arg + ADDSTATE_ఇక్కడ_స్థానభ్రంశం);
    }

    ఎంపిక (state->c)
    {
	స్థితి NFA_NCLOSE:
	స్థితి NFA_MCLOSE:
	స్థితి NFA_MCLOSE1:
	స్థితి NFA_MCLOSE2:
	స్థితి NFA_MCLOSE3:
	స్థితి NFA_MCLOSE4:
	స్థితి NFA_MCLOSE5:
	స్థితి NFA_MCLOSE6:
	స్థితి NFA_MCLOSE7:
	స్థితి NFA_MCLOSE8:
	స్థితి NFA_MCLOSE9:
#నిర్వచితమైతే FEAT_SYN_HL
	స్థితి NFA_ZCLOSE:
	స్థితి NFA_ZCLOSE1:
	స్థితి NFA_ZCLOSE2:
	స్థితి NFA_ZCLOSE3:
	స్థితి NFA_ZCLOSE4:
	స్థితి NFA_ZCLOSE5:
	స్థితి NFA_ZCLOSE6:
	స్థితి NFA_ZCLOSE7:
	స్థితి NFA_ZCLOSE8:
	స్థితి NFA_ZCLOSE9:
#అయితే_ముగింపు
	స్థితి NFA_MOPEN:
	స్థితి NFA_ZEND:
	స్థితి NFA_విభజించు:
	స్థితి NFA_ఖాళీలు:
	    // These nodes are not added themselves but their "out" and/or
	    // "out1" may be added below.
	    ఆపు;

	స్థితి NFA_BOL:
	స్థితి NFA_BOF:
	    // "^" won't match past end-of-line, don't bother trying.
	    // Except when at the end of the line, or when we are going to the
	    // next line for a look-behind match.
	    ఒకవేళ (rex.ఆదానం > rex.పంక్తి
		    && *rex.ఆదానం != NUL
		    && (nfa_endp == ఖాళీ
			|| !REG_MULTI
			|| rex.lnum == nfa_endp->se_u.pos.lnum))
		వెళ్ళు skip_కూడు;
	    // FALLTHROUGH

	స్థితి NFA_MOPEN1:
	స్థితి NFA_MOPEN2:
	స్థితి NFA_MOPEN3:
	స్థితి NFA_MOPEN4:
	స్థితి NFA_MOPEN5:
	స్థితి NFA_MOPEN6:
	స్థితి NFA_MOPEN7:
	స్థితి NFA_MOPEN8:
	స్థితి NFA_MOPEN9:
#నిర్వచితమైతే FEAT_SYN_HL
	స్థితి NFA_ZOPEN:
	స్థితి NFA_ZOPEN1:
	స్థితి NFA_ZOPEN2:
	స్థితి NFA_ZOPEN3:
	స్థితి NFA_ZOPEN4:
	స్థితి NFA_ZOPEN5:
	స్థితి NFA_ZOPEN6:
	స్థితి NFA_ZOPEN7:
	స్థితి NFA_ZOPEN8:
	స్థితి NFA_ZOPEN9:
#అయితే_ముగింపు
	స్థితి NFA_NOPEN:
	స్థితి NFA_ZSTART:
	    // These nodes need to be added so that we can bail out when it
	    // was added to this list before at the same position to avoid an
	    // endless loop for "\(\)*"

	అప్రమేయ:
	    ఒకవేళ (state->lastlist[nfa_ll_సూచీ] == l->id && state->c != NFA_SKIP)
	    {
		// This state is already in the list, don't add it again,
		// unless it is an MOPEN that is used for a backreference or
		// when there is a PIM. For NFA_MATCH check the position,
		// lower position is preferred.
		ఒకవేళ (!rex.nfa_has_backref && pim == ఖాళీ && !l->has_pim
						     && state->c != NFA_సరిపోలిక)
		{
		    // When called from addstate_here() do insert before
		    // existing states.
		    ఒకవేళ (కూడు_ఇక్కడ)
		    {
			కొరకు (k = 0; k < l->n && k < listindex; ++k)
			    ఒకవేళ (l->t[k].state->id == state->id)
			    {
				దొరికింది = TRUE;
				ఆపు;
			    }
		    }
		    ఒకవేళ (!కూడు_ఇక్కడ || దొరికింది)
		    {
skip_కూడు:
#నిర్వచితమైతే ENABLE_LOG
			nfa_సమితి_సంకేతం(state->c);
			fprintf(log_fd, "> Not adding state %d to list %d. char %d: %s pim: %s has_pim: %d found: %d\n",
			    నిరపేక్ష(state->id), l->id, state->c, సంకేతం_2,
			    pim == ఖాళీ ? "NULL" : "yes", l->has_pim, దొరికింది);
#అయితే_ముగింపు
			తిరిగి subs;
		    }
		}

		// Do not add the state again when it exists with the same
		// positions.
		ఒకవేళ (has_state_with_pos(l, state, subs, pim))
		    వెళ్ళు skip_కూడు;
	    }

	    // When there are backreferences or PIMs the number of states may
	    // be (a lot) bigger than anticipated.
	    ఒకవేళ (l->n == l->len)
	    {
		పూర్ణాంకం		newlen = l->len * 3 / 2 + 50;
		size_t		newsize = newlen * పరిమాణం(nfa_thread_T);
		nfa_thread_T	*newt;

		ఒకవేళ ((దీర్ఘ)(newsize >> 10) >= p_mmp)
		{
		    emsg(_(e_నమూనా_ఉపయోగిస్తుంది_మరిన్ని_జ్ఞాపకం_than_maxmempattern));
		    తిరిగి ఖాళీ;
		}
		ఒకవేళ (subs != &తాత్కాలిక_subs)
		{
		    // "subs" may point into the current array, need to make a
		    // copy before it becomes invalid.
		    copy_sub(&తాత్కాలిక_subs.norm, &subs->norm);
#నిర్వచితమైతే FEAT_SYN_HL
		    ఒకవేళ (rex.nfa_has_zsubexpr)
			copy_sub(&తాత్కాలిక_subs.synt, &subs->synt);
#అయితే_ముగింపు
		    subs = &తాత్కాలిక_subs;
		}

		newt = vim_realloc(l->t, newsize);
		ఒకవేళ (newt == ఖాళీ)
		{
		    // out of memory
		    తిరిగి ఖాళీ;
		}
		l->t = newt;
		l->len = newlen;
	    }

	    // add the state to the list
	    state->lastlist[nfa_ll_సూచీ] = l->id;
	    thread = &l->t[l->n++];
	    thread->state = state;
	    ఒకవేళ (pim == ఖాళీ)
		thread->pim.ఫలితం = NFA_PIM_వాడని;
	    లేకపోతే
	    {
		copy_pim(&thread->pim, pim);
		l->has_pim = TRUE;
	    }
	    copy_sub(&thread->subs.norm, &subs->norm);
#నిర్వచితమైతే FEAT_SYN_HL
	    ఒకవేళ (rex.nfa_has_zsubexpr)
		copy_sub(&thread->subs.synt, &subs->synt);
#అయితే_ముగింపు
#నిర్వచితమైతే ENABLE_LOG
	    report_state("Adding", &thread->subs.norm, state, l->id, pim);
	    did_print = TRUE;
#అయితే_ముగింపు
    }

#నిర్వచితమైతే ENABLE_LOG
    ఒకవేళ (!did_print)
	report_state("Processing", &subs->norm, state, l->id, pim);
#అయితే_ముగింపు
    ఎంపిక (state->c)
    {
	స్థితి NFA_సరిపోలిక:
	    ఆపు;

	స్థితి NFA_విభజించు:
	    // order matters here
	    subs = addstate(l, state->బహిర్గతం, subs, pim, off_arg, లోతు + 1);
	    subs = addstate(l, state->out1, subs, pim, off_arg, లోతు + 1);
	    ఆపు;

	స్థితి NFA_ఖాళీలు:
	స్థితి NFA_NOPEN:
	స్థితి NFA_NCLOSE:
	    subs = addstate(l, state->బహిర్గతం, subs, pim, off_arg, లోతు + 1);
	    ఆపు;

	స్థితి NFA_MOPEN:
	స్థితి NFA_MOPEN1:
	స్థితి NFA_MOPEN2:
	స్థితి NFA_MOPEN3:
	స్థితి NFA_MOPEN4:
	స్థితి NFA_MOPEN5:
	స్థితి NFA_MOPEN6:
	స్థితి NFA_MOPEN7:
	స్థితి NFA_MOPEN8:
	స్థితి NFA_MOPEN9:
#నిర్వచితమైతే FEAT_SYN_HL
	స్థితి NFA_ZOPEN:
	స్థితి NFA_ZOPEN1:
	స్థితి NFA_ZOPEN2:
	స్థితి NFA_ZOPEN3:
	స్థితి NFA_ZOPEN4:
	స్థితి NFA_ZOPEN5:
	స్థితి NFA_ZOPEN6:
	స్థితి NFA_ZOPEN7:
	స్థితి NFA_ZOPEN8:
	స్థితి NFA_ZOPEN9:
#అయితే_ముగింపు
	స్థితి NFA_ZSTART:
	    ఒకవేళ (state->c == NFA_ZSTART)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#నిర్వచితమైతే FEAT_SYN_HL
	    లేకపోతే ఒకవేళ (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
	    {
		subidx = state->c - NFA_ZOPEN;
		sub = &subs->synt;
	    }
#అయితే_ముగింపు
	    లేకపోతే
	    {
		subidx = state->c - NFA_MOPEN;
		sub = &subs->norm;
	    }

	    // avoid compiler warnings
	    save_ptr = ఖాళీ;
	    CLEAR_క్షేత్రం(save_multipos);

	    // Set the position (with "off" added) in the subexpression.  Save
	    // and restore it when it was in use.  Otherwise fill any gap.
	    ఒకవేళ (REG_MULTI)
	    {
		ఒకవేళ (subidx < sub->లోపల_use)
		{
		    save_multipos = sub->list.multi[subidx];
		    save_లోపల_use = -1;
		}
		లేకపోతే
		{
		    save_లోపల_use = sub->లోపల_use;
		    కొరకు (i = sub->లోపల_use; i < subidx; ++i)
		    {
			sub->list.multi[i].ప్రారంభం_lnum = -1;
			sub->list.multi[i].చివర_lnum = -1;
		    }
		    sub->లోపల_use = subidx + 1;
		}
		ఒకవేళ (off == -1)
		{
		    sub->list.multi[subidx].ప్రారంభం_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].ప్రారంభం_నిలువు = 0;
		}
		లేకపోతే
		{
		    sub->list.multi[subidx].ప్రారంభం_lnum = rex.lnum;
		    sub->list.multi[subidx].ప్రారంభం_నిలువు =
					 (colnr_T)(rex.ఆదానం - rex.పంక్తి + off);
		}
		sub->list.multi[subidx].చివర_lnum = -1;
	    }
	    లేకపోతే
	    {
		ఒకవేళ (subidx < sub->లోపల_use)
		{
		    save_ptr = sub->list.పంక్తి[subidx].ప్రారంభం;
		    save_లోపల_use = -1;
		}
		లేకపోతే
		{
		    save_లోపల_use = sub->లోపల_use;
		    కొరకు (i = sub->లోపల_use; i < subidx; ++i)
		    {
			sub->list.పంక్తి[i].ప్రారంభం = ఖాళీ;
			sub->list.పంక్తి[i].చివర = ఖాళీ;
		    }
		    sub->లోపల_use = subidx + 1;
		}
		sub->list.పంక్తి[subidx].ప్రారంభం = rex.ఆదానం + off;
	    }

	    subs = addstate(l, state->బహిర్గతం, subs, pim, off_arg, లోతు + 1);
	    ఒకవేళ (subs == ఖాళీ)
		ఆపు;
	    // "subs" may have changed, need to set "sub" again
#నిర్వచితమైతే FEAT_SYN_HL
	    ఒకవేళ (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
		sub = &subs->synt;
	    లేకపోతే
#అయితే_ముగింపు
		sub = &subs->norm;

	    ఒకవేళ (save_లోపల_use == -1)
	    {
		ఒకవేళ (REG_MULTI)
		    sub->list.multi[subidx] = save_multipos;
		లేకపోతే
		    sub->list.పంక్తి[subidx].ప్రారంభం = save_ptr;
	    }
	    లేకపోతే
		sub->లోపల_use = save_లోపల_use;
	    ఆపు;

	స్థితి NFA_MCLOSE:
	    ఒకవేళ (rex.nfa_has_zend && (REG_MULTI
			? subs->norm.list.multi[0].చివర_lnum >= 0
			: subs->norm.list.పంక్తి[0].చివర != ఖాళీ))
	    {
		// Do not overwrite the position set by \ze.
		subs = addstate(l, state->బహిర్గతం, subs, pim, off_arg, లోతు + 1);
		ఆపు;
	    }
	    // FALLTHROUGH
	స్థితి NFA_MCLOSE1:
	స్థితి NFA_MCLOSE2:
	స్థితి NFA_MCLOSE3:
	స్థితి NFA_MCLOSE4:
	స్థితి NFA_MCLOSE5:
	స్థితి NFA_MCLOSE6:
	స్థితి NFA_MCLOSE7:
	స్థితి NFA_MCLOSE8:
	స్థితి NFA_MCLOSE9:
#నిర్వచితమైతే FEAT_SYN_HL
	స్థితి NFA_ZCLOSE:
	స్థితి NFA_ZCLOSE1:
	స్థితి NFA_ZCLOSE2:
	స్థితి NFA_ZCLOSE3:
	స్థితి NFA_ZCLOSE4:
	స్థితి NFA_ZCLOSE5:
	స్థితి NFA_ZCLOSE6:
	స్థితి NFA_ZCLOSE7:
	స్థితి NFA_ZCLOSE8:
	స్థితి NFA_ZCLOSE9:
#అయితే_ముగింపు
	స్థితి NFA_ZEND:
	    ఒకవేళ (state->c == NFA_ZEND)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#నిర్వచితమైతే FEAT_SYN_HL
	    లేకపోతే ఒకవేళ (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
	    {
		subidx = state->c - NFA_ZCLOSE;
		sub = &subs->synt;
	    }
#అయితే_ముగింపు
	    లేకపోతే
	    {
		subidx = state->c - NFA_MCLOSE;
		sub = &subs->norm;
	    }

	    // We don't fill in gaps here, there must have been an MOPEN that
	    // has done that.
	    save_లోపల_use = sub->లోపల_use;
	    ఒకవేళ (sub->లోపల_use <= subidx)
		sub->లోపల_use = subidx + 1;
	    ఒకవేళ (REG_MULTI)
	    {
		save_multipos = sub->list.multi[subidx];
		ఒకవేళ (off == -1)
		{
		    sub->list.multi[subidx].చివర_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].చివర_నిలువు = 0;
		}
		లేకపోతే
		{
		    sub->list.multi[subidx].చివర_lnum = rex.lnum;
		    sub->list.multi[subidx].చివర_నిలువు =
					  (colnr_T)(rex.ఆదానం - rex.పంక్తి + off);
		}
		// avoid compiler warnings
		save_ptr = ఖాళీ;
	    }
	    లేకపోతే
	    {
		save_ptr = sub->list.పంక్తి[subidx].చివర;
		sub->list.పంక్తి[subidx].చివర = rex.ఆదానం + off;
		// avoid compiler warnings
		CLEAR_క్షేత్రం(save_multipos);
	    }

	    subs = addstate(l, state->బహిర్గతం, subs, pim, off_arg, లోతు + 1);
	    ఒకవేళ (subs == ఖాళీ)
		ఆపు;
	    // "subs" may have changed, need to set "sub" again
#నిర్వచితమైతే FEAT_SYN_HL
	    ఒకవేళ (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
		sub = &subs->synt;
	    లేకపోతే
#అయితే_ముగింపు
		sub = &subs->norm;

	    ఒకవేళ (REG_MULTI)
		sub->list.multi[subidx] = save_multipos;
	    లేకపోతే
		sub->list.పంక్తి[subidx].చివర = save_ptr;
	    sub->లోపల_use = save_లోపల_use;
	    ఆపు;
    }
    తిరిగి subs;
}

/*
 * Like addstate(), but the new state(s) are put at position "*ip".
 * Used for zero-width matches, next state to use is the added one.
 * This makes sure the order of states to be tried does not change, which
 * matters for alternatives.
 */
    స్థిర regsubs_T *
addstate_ఇక్కడ(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim,   // postponed look-behind match
    పూర్ణాంకం			*ip)
{
    పూర్ణాంకం tlen = l->n;
    పూర్ణాంకం లెక్క;
    పూర్ణాంకం listidx = *ip;
    regsubs_T *r;

    // First add the state(s) at the end, so that we know how many there are.
    // Pass the listidx as offset (avoids adding another argument to
    // addstate()).
    r = addstate(l, state, subs, pim, -listidx - ADDSTATE_ఇక్కడ_స్థానభ్రంశం, 0);
    ఒకవేళ (r == ఖాళీ)
	తిరిగి ఖాళీ;

    // when "*ip" was at the end of the list, nothing to do
    ఒకవేళ (listidx + 1 == tlen)
	తిరిగి r;

    // re-order to put the new state at the current position
    లెక్క = l->n - tlen;
    ఒకవేళ (లెక్క == 0)
	తిరిగి r; // no state got added
    ఒకవేళ (లెక్క == 1)
    {
	// overwrite the current state
	l->t[listidx] = l->t[l->n - 1];
    }
    లేకపోతే ఒకవేళ (లెక్క > 1)
    {
	ఒకవేళ (l->n + లెక్క - 1 >= l->len)
	{
	    // not enough space to move the new states, reallocate the list
	    // and move the states to the right position
	    పూర్ణాంకం		    newlen = l->len * 3 / 2 + 50;
	    size_t	    newsize = newlen * పరిమాణం(nfa_thread_T);
	    nfa_thread_T    *newl;

	    ఒకవేళ ((దీర్ఘ)(newsize >> 10) >= p_mmp)
	    {
		emsg(_(e_నమూనా_ఉపయోగిస్తుంది_మరిన్ని_జ్ఞాపకం_than_maxmempattern));
		తిరిగి ఖాళీ;
	    }
	    newl = alloc(newsize);
	    ఒకవేళ (newl == ఖాళీ)
		తిరిగి ఖాళీ;
	    l->len = newlen;
	    mch_memmove(&(newl[0]),
		    &(l->t[0]),
		    పరిమాణం(nfa_thread_T) * listidx);
	    mch_memmove(&(newl[listidx]),
		    &(l->t[l->n - లెక్క]),
		    పరిమాణం(nfa_thread_T) * లెక్క);
	    mch_memmove(&(newl[listidx + లెక్క]),
		    &(l->t[listidx + 1]),
		    పరిమాణం(nfa_thread_T) * (l->n - లెక్క - listidx - 1));
	    vim_free(l->t);
	    l->t = newl;
	}
	లేకపోతే
	{
	    // make space for new states, then move them from the
	    // end to the current position
	    mch_memmove(&(l->t[listidx + లెక్క]),
		    &(l->t[listidx + 1]),
		    పరిమాణం(nfa_thread_T) * (l->n - listidx - 1));
	    mch_memmove(&(l->t[listidx]),
		    &(l->t[l->n - 1]),
		    పరిమాణం(nfa_thread_T) * లెక్క);
	}
    }
    --l->n;
    *ip = listidx - 1;

    తిరిగి r;
}

/*
 * Check character class "class" against current character c.
 */
    స్థిర పూర్ణాంకం
check_char_class(పూర్ణాంకం class, పూర్ణాంకం c)
{
    ఎంపిక (class)
    {
	స్థితి NFA_CLASS_ALNUM:
	    ఒకవేళ (c >= 1 && c < 128 && అక్షరమా_అంకెనా(c))
		తిరిగి OK;
	    ఆపు;
	స్థితి NFA_CLASS_ALPHA:
	    ఒకవేళ (c >= 1 && c < 128 && అక్షరమా(c))
		తిరిగి OK;
	    ఆపు;
	స్థితి NFA_CLASS_BLANK:
	    ఒకవేళ (c == ' ' || c == '\t')
		తిరిగి OK;
	    ఆపు;
	స్థితి NFA_CLASS_CNTRL:
	    ఒకవేళ (c >= 1 && c <= 127 && iscntrl(c))
		తిరిగి OK;
	    ఆపు;
	స్థితి NFA_CLASS_DIGIT:
	    ఒకవేళ (VIM_ISDIGIT(c))
		తిరిగి OK;
	    ఆపు;
	స్థితి NFA_CLASS_లేఖాచిత్రం:
	    ఒకవేళ (c >= 1 && c <= 127 && isgraph(c))
		తిరిగి OK;
	    ఆపు;
	స్థితి NFA_CLASS_క్రింది:
	    ఒకవేళ (MB_ISLOWER(c) && c != 170 && c != 186)
		తిరిగి OK;
	    ఆపు;
	స్థితి NFA_CLASS_PRINT:
	    ఒకవేళ (vim_isprintc(c))
		తిరిగి OK;
	    ఆపు;
	స్థితి NFA_CLASS_PUNCT:
	    ఒకవేళ (c >= 1 && c < 128 && ispunct(c))
		తిరిగి OK;
	    ఆపు;
	స్థితి NFA_CLASS_SPACE:
	    ఒకవేళ ((c >= 9 && c <= 13) || (c == ' '))
		తిరిగి OK;
	    ఆపు;
	స్థితి NFA_CLASS_పై:
	    ఒకవేళ (MB_ISUPPER(c))
		తిరిగి OK;
	    ఆపు;
	స్థితి NFA_CLASS_XDIGIT:
	    ఒకవేళ (vim_isxdigit(c))
		తిరిగి OK;
	    ఆపు;
	స్థితి NFA_CLASS_TAB:
	    ఒకవేళ (c == '\t')
		తిరిగి OK;
	    ఆపు;
	స్థితి NFA_CLASS_RETURN:
	    ఒకవేళ (c == '\r')
		తిరిగి OK;
	    ఆపు;
	స్థితి NFA_CLASS_BACKSPACE:
	    ఒకవేళ (c == '\b')
		తిరిగి OK;
	    ఆపు;
	స్థితి NFA_CLASS_ESCAPE:
	    ఒకవేళ (c == '\033')
		తిరిగి OK;
	    ఆపు;
	స్థితి NFA_CLASS_IDENT:
	    ఒకవేళ (vim_isIDc(c))
		తిరిగి OK;
	    ఆపు;
	స్థితి NFA_CLASS_KEYWORD:
	    ఒకవేళ (reg_iswordc(c))
		తిరిగి OK;
	    ఆపు;
	స్థితి NFA_CLASS_FNAME:
	    ఒకవేళ (vim_isfilec(c))
		తిరిగి OK;
	    ఆపు;

	అప్రమేయ:
	    // should not be here :P
	    siemsg(_(e_nfa_regexp_invalid_character_class_nr), class);
	    తిరిగి FAIL;
    }
    తిరిగి FAIL;
}

/*
 * Check for a match with subexpression "subidx".
 * Return TRUE if it matches.
 */
    స్థిర పూర్ణాంకం
సరిపోలిక_backref(
    regsub_T	*sub,	    // pointers to subexpressions
    పూర్ణాంకం		subidx,
    పూర్ణాంకం		*bytelen)   // out: length of match in bytes
{
    పూర్ణాంకం		len;

    ఒకవేళ (sub->లోపల_use <= subidx)
    {
retempty:
	// backref was not set, match an empty string
	*bytelen = 0;
	తిరిగి TRUE;
    }

    ఒకవేళ (REG_MULTI)
    {
	ఒకవేళ (sub->list.multi[subidx].ప్రారంభం_lnum < 0
				       || sub->list.multi[subidx].చివర_lnum < 0)
	    వెళ్ళు retempty;
	ఒకవేళ (sub->list.multi[subidx].ప్రారంభం_lnum == rex.lnum
			       && sub->list.multi[subidx].చివర_lnum == rex.lnum)
	{
	    len = sub->list.multi[subidx].చివర_నిలువు
					  - sub->list.multi[subidx].ప్రారంభం_నిలువు;
	    ఒకవేళ (cstrncmp(rex.పంక్తి + sub->list.multi[subidx].ప్రారంభం_నిలువు,
							 rex.ఆదానం, &len) == 0)
	    {
		*bytelen = len;
		తిరిగి TRUE;
	    }
	}
	లేకపోతే
	{
	    ఒకవేళ (సరిపోలిక_with_backref(
			sub->list.multi[subidx].ప్రారంభం_lnum,
			sub->list.multi[subidx].ప్రారంభం_నిలువు,
			sub->list.multi[subidx].చివర_lnum,
			sub->list.multi[subidx].చివర_నిలువు,
			bytelen) == RA_సరిపోలిక)
		తిరిగి TRUE;
	}
    }
    లేకపోతే
    {
	ఒకవేళ (sub->list.పంక్తి[subidx].ప్రారంభం == ఖాళీ
					|| sub->list.పంక్తి[subidx].చివర == ఖాళీ)
	    వెళ్ళు retempty;
	len = (పూర్ణాంకం)(sub->list.పంక్తి[subidx].చివర - sub->list.పంక్తి[subidx].ప్రారంభం);
	ఒకవేళ (cstrncmp(sub->list.పంక్తి[subidx].ప్రారంభం, rex.ఆదానం, &len) == 0)
	{
	    *bytelen = len;
	    తిరిగి TRUE;
	}
    }
    తిరిగి FALSE;
}

#నిర్వచితమైతే FEAT_SYN_HL

/*
 * Check for a match with \z subexpression "subidx".
 * Return TRUE if it matches.
 */
    స్థిర పూర్ణాంకం
సరిపోలిక_zref(
    పూర్ణాంకం		subidx,
    పూర్ణాంకం		*bytelen)   // out: length of match in bytes
{
    పూర్ణాంకం		len;

    cleanup_zsubexpr();
    ఒకవేళ (re_extmatch_లోపల == ఖాళీ || re_extmatch_లోపల->matches[subidx] == ఖాళీ)
    {
	// backref was not set, match an empty string
	*bytelen = 0;
	తిరిగి TRUE;
    }

    len = (పూర్ణాంకం)STRLEN(re_extmatch_లోపల->matches[subidx]);
    ఒకవేళ (cstrncmp(re_extmatch_లోపల->matches[subidx], rex.ఆదానం, &len) == 0)
    {
	*bytelen = len;
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}
#అయితే_ముగింపు

/*
 * Save list IDs for all NFA states of "prog" into "list".
 * Also reset the IDs to zero.
 * Only used for the recursive value lastlist[1].
 */
    స్థిర శూన్య
nfa_save_listids(nfa_regprog_T *prog, పూర్ణాంకం *list)
{
    పూర్ణాంకం		    i;
    nfa_state_T	    *p;

    // Order in the list is reverse, it's a bit faster that way.
    p = &prog->state[0];
    కొరకు (i = prog->nstate; --i >= 0; )
    {
	list[i] = p->lastlist[1];
	p->lastlist[1] = 0;
	++p;
    }
}

/*
 * Restore list IDs from "list" to all NFA states.
 */
    స్థిర శూన్య
nfa_restore_listids(nfa_regprog_T *prog, పూర్ణాంకం *list)
{
    పూర్ణాంకం		    i;
    nfa_state_T	    *p;

    p = &prog->state[0];
    కొరకు (i = prog->nstate; --i >= 0; )
    {
	p->lastlist[1] = list[i];
	++p;
    }
}

    స్థిర పూర్ణాంకం
nfa_re_అంకె_cmp(long_u val, పూర్ణాంకం op, long_u pos)
{
    ఒకవేళ (op == 1) తిరిగి pos > val;
    ఒకవేళ (op == 2) తిరిగి pos < val;
    తిరిగి val == pos;
}

స్థిర పూర్ణాంకం nfa_regmatch(nfa_regprog_T *prog, nfa_state_T *ప్రారంభం, regsubs_T *submatch, regsubs_T *m);

/*
 * Recursively call nfa_regmatch()
 * "pim" is NULL or contains info about a Postponed Invisible Match (start
 * position).
 */
    స్థిర పూర్ణాంకం
recursive_regmatch(
    nfa_state_T	    *state,
    nfa_pim_T	    *pim,
    nfa_regprog_T   *prog,
    regsubs_T	    *submatch,
    regsubs_T	    *m,
    పూర్ణాంకం		    **listids,
    పూర్ణాంకం		    *listids_len)
{
    పూర్ణాంకం		save_reginput_నిలువు = (పూర్ణాంకం)(rex.ఆదానం - rex.పంక్తి);
    పూర్ణాంకం		save_reglnum = rex.lnum;
    పూర్ణాంకం		save_nfa_సరిపోలిక = nfa_సరిపోలిక;
    పూర్ణాంకం		save_nfa_listid = rex.nfa_listid;
    save_se_T   *save_nfa_endp = nfa_endp;
    save_se_T   endpos;
    save_se_T   *endposp = ఖాళీ;
    పూర్ణాంకం		ఫలితం;
    పూర్ణాంకం		need_restore = FALSE;

    ఒకవేళ (pim != ఖాళీ)
    {
	// start at the position where the postponed match was
	ఒకవేళ (REG_MULTI)
	    rex.ఆదానం = rex.పంక్తి + pim->చివర.pos.నిలువు;
	లేకపోతే
	    rex.ఆదానం = pim->చివర.ptr;
    }

    ఒకవేళ (state->c == NFA_ప్రారంభం_INVISIBLE_BEFORE
	    || state->c == NFA_ప్రారంభం_INVISIBLE_BEFORE_మొదటి
	    || state->c == NFA_ప్రారంభం_INVISIBLE_BEFORE_NEG
	    || state->c == NFA_ప్రారంభం_INVISIBLE_BEFORE_NEG_మొదటి)
    {
	// The recursive match must end at the current position. When "pim" is
	// not NULL it specifies the current position.
	endposp = &endpos;
	ఒకవేళ (REG_MULTI)
	{
	    ఒకవేళ (pim == ఖాళీ)
	    {
		endpos.se_u.pos.నిలువు = (పూర్ణాంకం)(rex.ఆదానం - rex.పంక్తి);
		endpos.se_u.pos.lnum = rex.lnum;
	    }
	    లేకపోతే
		endpos.se_u.pos = pim->చివర.pos;
	}
	లేకపోతే
	{
	    ఒకవేళ (pim == ఖాళీ)
		endpos.se_u.ptr = rex.ఆదానం;
	    లేకపోతే
		endpos.se_u.ptr = pim->చివర.ptr;
	}

	// Go back the specified number of bytes, or as far as the
	// start of the previous line, to try matching "\@<=" or
	// not matching "\@<!". This is very inefficient, limit the number of
	// bytes if possible.
	ఒకవేళ (state->val <= 0)
	{
	    ఒకవేళ (REG_MULTI)
	    {
		rex.పంక్తి = reg_getline(--rex.lnum);
		ఒకవేళ (rex.పంక్తి == ఖాళీ)
		    // can't go before the first line
		    rex.పంక్తి = reg_getline(++rex.lnum);
	    }
	    rex.ఆదానం = rex.పంక్తి;
	}
	లేకపోతే
	{
	    ఒకవేళ (REG_MULTI && (పూర్ణాంకం)(rex.ఆదానం - rex.పంక్తి) < state->val)
	    {
		// Not enough bytes in this line, go to end of
		// previous line.
		rex.పంక్తి = reg_getline(--rex.lnum);
		ఒకవేళ (rex.పంక్తి == ఖాళీ)
		{
		    // can't go before the first line
		    rex.పంక్తి = reg_getline(++rex.lnum);
		    rex.ఆదానం = rex.పంక్తి;
		}
		లేకపోతే
		    rex.ఆదానం = rex.పంక్తి + reg_getline_len(rex.lnum);
	    }
	    ఒకవేళ ((పూర్ణాంకం)(rex.ఆదానం - rex.పంక్తి) >= state->val)
	    {
		rex.ఆదానం -= state->val;
		ఒకవేళ (has_mbyte)
		    rex.ఆదానం -= mb_తల_off(rex.పంక్తి, rex.ఆదానం);
	    }
	    లేకపోతే
		rex.ఆదానం = rex.పంక్తి;
	}
    }

#నిర్వచితమైతే ENABLE_LOG
    ఒకవేళ (log_fd != stderr)
	fclose(log_fd);
    log_fd = ఖాళీ;
#అయితే_ముగింపు
    // Have to clear the lastlist field of the NFA nodes, so that
    // nfa_regmatch() and addstate() can run properly after recursion.
    ఒకవేళ (nfa_ll_సూచీ == 1)
    {
	// Already calling nfa_regmatch() recursively.  Save the lastlist[1]
	// values and clear them.
	ఒకవేళ (*listids == ఖాళీ || *listids_len < prog->nstate)
	{
	    vim_free(*listids);
	    *listids = ALLOC_MULT(పూర్ణాంకం, prog->nstate);
	    ఒకవేళ (*listids == ఖాళీ)
	    {
		emsg(_(e_nfa_regexp_could_not_allocate_జ్ఞాపకం_for_branch_traversal));
		తిరిగి 0;
	    }
	    *listids_len = prog->nstate;
	}
	nfa_save_listids(prog, *listids);
	need_restore = TRUE;
	// any value of rex.nfa_listid will do
    }
    లేకపోతే
    {
	// First recursive nfa_regmatch() call, switch to the second lastlist
	// entry.  Make sure rex.nfa_listid is different from a previous
	// recursive call, because some states may still have this ID.
	++nfa_ll_సూచీ;
	ఒకవేళ (rex.nfa_listid <= rex.nfa_alt_listid)
	    rex.nfa_listid = rex.nfa_alt_listid;
    }

    // Call nfa_regmatch() to check if the current concat matches at this
    // position. The concat ends with the node NFA_END_INVISIBLE
    nfa_endp = endposp;
    ఫలితం = nfa_regmatch(prog, state->బహిర్గతం, submatch, m);

    ఒకవేళ (need_restore)
	nfa_restore_listids(prog, *listids);
    లేకపోతే
    {
	--nfa_ll_సూచీ;
	rex.nfa_alt_listid = rex.nfa_listid;
    }

    // restore position in input text
    rex.lnum = save_reglnum;
    ఒకవేళ (REG_MULTI)
	rex.పంక్తి = reg_getline(rex.lnum);
    rex.ఆదానం = rex.పంక్తి + save_reginput_నిలువు;
    ఒకవేళ (ఫలితం != NFA_TOO_EXPENSIVE)
    {
	nfa_సరిపోలిక = save_nfa_సరిపోలిక;
	rex.nfa_listid = save_nfa_listid;
    }
    nfa_endp = save_nfa_endp;

#నిర్వచితమైతే ENABLE_LOG
    open_debug_log(ఫలితం);
#అయితే_ముగింపు

    తిరిగి ఫలితం;
}

/*
 * Estimate the chance of a match with "state" failing.
 * empty match: 0
 * NFA_ANY: 1
 * specific character: 99
 */
    స్థిర పూర్ణాంకం
failure_chance(nfa_state_T *state, పూర్ణాంకం లోతు)
{
    పూర్ణాంకం c = state->c;
    పూర్ణాంకం l, r;

    // detect looping
    ఒకవేళ (లోతు > 4)
	తిరిగి 1;

    ఎంపిక (c)
    {
	స్థితి NFA_విభజించు:
	    ఒకవేళ (state->బహిర్గతం->c == NFA_విభజించు || state->out1->c == NFA_విభజించు)
		// avoid recursive stuff
		తిరిగి 1;
	    // two alternatives, use the lowest failure chance
	    l = failure_chance(state->బహిర్గతం, లోతు + 1);
	    r = failure_chance(state->out1, లోతు + 1);
	    తిరిగి l < r ? l : r;

	స్థితి NFA_ANY:
	    // matches anything, unlikely to fail
	    తిరిగి 1;

	స్థితి NFA_సరిపోలిక:
	స్థితి NFA_MCLOSE:
	స్థితి NFA_ANY_COMPOSING:
	    // empty match works always
	    తిరిగి 0;

	స్థితి NFA_ప్రారంభం_INVISIBLE:
	స్థితి NFA_ప్రారంభం_INVISIBLE_మొదటి:
	స్థితి NFA_ప్రారంభం_INVISIBLE_NEG:
	స్థితి NFA_ప్రారంభం_INVISIBLE_NEG_మొదటి:
	స్థితి NFA_ప్రారంభం_INVISIBLE_BEFORE:
	స్థితి NFA_ప్రారంభం_INVISIBLE_BEFORE_మొదటి:
	స్థితి NFA_ప్రారంభం_INVISIBLE_BEFORE_NEG:
	స్థితి NFA_ప్రారంభం_INVISIBLE_BEFORE_NEG_మొదటి:
	స్థితి NFA_ప్రారంభం_నమూనా:
	    // recursive regmatch is expensive, use low failure chance
	    తిరిగి 5;

	స్థితి NFA_BOL:
	స్థితి NFA_EOL:
	స్థితి NFA_BOF:
	స్థితి NFA_EOF:
	స్థితి NFA_NEWL:
	    తిరిగి 99;

	స్థితి NFA_BOW:
	స్థితి NFA_EOW:
	    తిరిగి 90;

	స్థితి NFA_MOPEN:
	స్థితి NFA_MOPEN1:
	స్థితి NFA_MOPEN2:
	స్థితి NFA_MOPEN3:
	స్థితి NFA_MOPEN4:
	స్థితి NFA_MOPEN5:
	స్థితి NFA_MOPEN6:
	స్థితి NFA_MOPEN7:
	స్థితి NFA_MOPEN8:
	స్థితి NFA_MOPEN9:
#నిర్వచితమైతే FEAT_SYN_HL
	స్థితి NFA_ZOPEN:
	స్థితి NFA_ZOPEN1:
	స్థితి NFA_ZOPEN2:
	స్థితి NFA_ZOPEN3:
	స్థితి NFA_ZOPEN4:
	స్థితి NFA_ZOPEN5:
	స్థితి NFA_ZOPEN6:
	స్థితి NFA_ZOPEN7:
	స్థితి NFA_ZOPEN8:
	స్థితి NFA_ZOPEN9:
	స్థితి NFA_ZCLOSE:
	స్థితి NFA_ZCLOSE1:
	స్థితి NFA_ZCLOSE2:
	స్థితి NFA_ZCLOSE3:
	స్థితి NFA_ZCLOSE4:
	స్థితి NFA_ZCLOSE5:
	స్థితి NFA_ZCLOSE6:
	స్థితి NFA_ZCLOSE7:
	స్థితి NFA_ZCLOSE8:
	స్థితి NFA_ZCLOSE9:
#అయితే_ముగింపు
	స్థితి NFA_NOPEN:
	స్థితి NFA_MCLOSE1:
	స్థితి NFA_MCLOSE2:
	స్థితి NFA_MCLOSE3:
	స్థితి NFA_MCLOSE4:
	స్థితి NFA_MCLOSE5:
	స్థితి NFA_MCLOSE6:
	స్థితి NFA_MCLOSE7:
	స్థితి NFA_MCLOSE8:
	స్థితి NFA_MCLOSE9:
	స్థితి NFA_NCLOSE:
	    తిరిగి failure_chance(state->బహిర్గతం, లోతు + 1);

	స్థితి NFA_BACKREF1:
	స్థితి NFA_BACKREF2:
	స్థితి NFA_BACKREF3:
	స్థితి NFA_BACKREF4:
	స్థితి NFA_BACKREF5:
	స్థితి NFA_BACKREF6:
	స్థితి NFA_BACKREF7:
	స్థితి NFA_BACKREF8:
	స్థితి NFA_BACKREF9:
#నిర్వచితమైతే FEAT_SYN_HL
	స్థితి NFA_ZREF1:
	స్థితి NFA_ZREF2:
	స్థితి NFA_ZREF3:
	స్థితి NFA_ZREF4:
	స్థితి NFA_ZREF5:
	స్థితి NFA_ZREF6:
	స్థితి NFA_ZREF7:
	స్థితి NFA_ZREF8:
	స్థితి NFA_ZREF9:
#అయితే_ముగింపు
	    // backreferences don't match in many places
	    తిరిగి 94;

	స్థితి NFA_LNUM_GT:
	స్థితి NFA_LNUM_LT:
	స్థితి NFA_నిలువు_GT:
	స్థితి NFA_నిలువు_LT:
	స్థితి NFA_VCOL_GT:
	స్థితి NFA_VCOL_LT:
	స్థితి NFA_MARK_GT:
	స్థితి NFA_MARK_LT:
	స్థితి NFA_VISUAL:
	    // before/after positions don't match very often
	    తిరిగి 85;

	స్థితి NFA_LNUM:
	    తిరిగి 90;

	స్థితి NFA_CURSOR:
	స్థితి NFA_నిలువు:
	స్థితి NFA_VCOL:
	స్థితి NFA_MARK:
	    // specific positions rarely match
	    తిరిగి 98;

	స్థితి NFA_COMPOSING:
	    తిరిగి 95;

	అప్రమేయ:
	    ఒకవేళ (c > 0)
		// character match fails often
		తిరిగి 95;
    }

    // something else, includes character classes
    తిరిగి 50;
}

/*
 * Skip until the char "c" we know a match must start with.
 */
    స్థిర పూర్ణాంకం
skip_గమ్యం_ప్రారంభం(పూర్ణాంకం c, colnr_T *colp)
{
    char_u *s;

    // Used often, do some work to avoid call overhead.
    ఒకవేళ (!rex.reg_ic && !has_mbyte)
	s = vim_strbyte(rex.పంక్తి + *colp, c);
    లేకపోతే
	s = cstrchr(rex.పంక్తి + *colp, c);
    ఒకవేళ (s == ఖాళీ)
	తిరిగి FAIL;
    *colp = (పూర్ణాంకం)(s - rex.పంక్తి);
    తిరిగి OK;
}

/*
 * Check for a match with match_text.
 * Called after skip_to_start() has found regstart.
 * Returns zero for no match, 1 for a match.
 */
    స్థిర దీర్ఘ
కనుగొను_సరిపోలిక_పాఠ్యం(colnr_T *startcol, పూర్ణాంకం regstart, char_u *సరిపోలిక_పాఠ్యం)
{
    colnr_T నిలువు = *startcol;
    పూర్ణాంకం	    c1, c2;
    పూర్ణాంకం	    len1, len2;
    పూర్ణాంకం	    సరిపోలిక;

    కొరకు (;;)
    {
	సరిపోలిక = TRUE;
	// Skip the input character: case-folding can change its byte length.
	len2 = MB_CPTR2LEN(rex.పంక్తి + నిలువు);
	కొరకు (len1 = 0; సరిపోలిక_పాఠ్యం[len1] != NUL; len1 += MB_CHAR2LEN(c1))
	{
	    c1 = PTR2CHAR(సరిపోలిక_పాఠ్యం + len1);
	    c2 = PTR2CHAR(rex.పంక్తి + నిలువు + len2);
	    ఒకవేళ (c1 != c2 && (!rex.reg_ic || MB_CASEFOLD(c1) != MB_CASEFOLD(c2)))
	    {
		సరిపోలిక = FALSE;
		ఆపు;
	    }
	    len2 += enc_utf8 ? utf_ptr2len(rex.పంక్తి + నిలువు + len2)
							     : MB_CHAR2LEN(c2);
	}
	ఒకవేళ (సరిపోలిక
		// check that no composing char follows
		&& !(enc_utf8
			  && utf_iscomposing(PTR2CHAR(rex.పంక్తి + నిలువు + len2))))
	{
	    cleanup_subexpr();
	    ఒకవేళ (REG_MULTI)
	    {
		rex.reg_startpos[0].lnum = rex.lnum;
		rex.reg_startpos[0].నిలువు = నిలువు;
		rex.reg_endpos[0].lnum = rex.lnum;
		rex.reg_endpos[0].నిలువు = నిలువు + len2;
	    }
	    లేకపోతే
	    {
		rex.reg_startp[0] = rex.పంక్తి + నిలువు;
		rex.reg_endp[0] = rex.పంక్తి + నిలువు + len2;
	    }
	    *startcol = నిలువు;
	    తిరిగి 1L;
	}

	// Try finding regstart after the current match.
	నిలువు += MB_CPTR2LEN(rex.పంక్తి + నిలువు); // skip the input character
	ఒకవేళ (skip_గమ్యం_ప్రారంభం(regstart, &నిలువు) == FAIL)
	    ఆపు;
    }

    *startcol = నిలువు;
    తిరిగి 0L;
}

/*
 * Check whether the composing characters at the current input position match
 * the NFA composing sub-expression that starts at "sta" (the first state
 * after NFA_COMPOSING).  "curc" is the base character and "clen" its byte
 * length.
 * Returns OK when it matches, FAIL otherwise.
 */
    స్థిర పూర్ణాంకం
సరిపోలిక_composing(nfa_state_T *sta, పూర్ణాంకం curc, పూర్ణాంకం clen)
{
    పూర్ణాంకం		mc = curc;
    పూర్ణాంకం		len = 0;
    పూర్ణాంకం		cchars[MAX_MCO];
    పూర్ణాంకం		ccount = 0;
    పూర్ణాంకం		j;

    ఒకవేళ (utf_iscomposing(sta->c))
	// Only match composing character(s), ignore base character.  Used
	// for ".{composing}" and "{composing}" (no preceding character).
	len += mb_char2len(mc);

    ఒకవేళ (rex.reg_icombine && len == 0)
	// If \Z was present, then ignore composing characters.  When
	// ignoring the base character this always matches.
	తిరిగి sta->c == curc ? OK : FAIL;

    // Check base character matches first, unless ignored.
    ఒకవేళ (len == 0 && mc != sta->c)
	తిరిగి FAIL;

    ఒకవేళ (len == 0)
    {
	len += mb_char2len(mc);
	sta = sta->బహిర్గతం;
    }

    // We don't care about the order of composing characters.  Get them into
    // cchars[] first.
    అయితే (len < clen)
    {
	mc = mb_ptr2char(rex.ఆదానం + len);
	cchars[ccount++] = mc;
	len += mb_char2len(mc);
	ఒకవేళ (ccount == MAX_MCO)
	    ఆపు;
    }

    // Check that each composing char in the pattern matches a composing char
    // in the text.  We do not check if all composing chars are matched.
    అయితే (sta->c != NFA_చివర_COMPOSING)
    {
	కొరకు (j = 0; j < ccount; ++j)
	    ఒకవేళ (cchars[j] == sta->c)
		ఆపు;
	ఒకవేళ (j == ccount)
	    తిరిగి FAIL;
	sta = sta->బహిర్గతం;
    }

    తిరిగి OK;
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
    స్థిర పూర్ణాంకం
nfa_regmatch(
    nfa_regprog_T	*prog,
    nfa_state_T		*ప్రారంభం,
    regsubs_T		*submatch,
    regsubs_T		*m)
{
    పూర్ణాంకం		ఫలితం = FALSE;
    size_t	కొలత = 0;
    పూర్ణాంకం		flag = 0;
    పూర్ణాంకం		go_గమ్యం_nextline = FALSE;
    nfa_thread_T *t;
    nfa_list_T	list[2];
    పూర్ణాంకం		listidx;
    nfa_list_T	*thislist;
    nfa_list_T	*nextlist;
    పూర్ణాంకం		*listids = ఖాళీ;
    పూర్ణాంకం		listids_len = 0;
    nfa_state_T *కూడు_state;
    పూర్ణాంకం		కూడు_ఇక్కడ;
    పూర్ణాంకం		కూడు_లెక్క;
    పూర్ణాంకం		కూడు_off = 0;
    పూర్ణాంకం		toplevel = ప్రారంభం->c == NFA_MOPEN;
    regsubs_T	*r;
#నిర్వచితమైతే NFA_REGEXP_DEBUG_LOG
    FILE	*debug;
#అయితే_ముగింపు

    // Some patterns may take a long time to match, especially when using
    // recursive_regmatch(). Allow interrupting them with CTRL-C.
    fast_breakcheck();
    ఒకవేళ (got_int)
	తిరిగి FALSE;
#నిర్వచితమైతే FEAT_RELTIME
    ఒకవేళ (nfa_did_సమయం_బహిర్గతం())
	తిరిగి FALSE;
#అయితే_ముగింపు

#నిర్వచితమైతే NFA_REGEXP_DEBUG_LOG
    debug = fopen(NFA_REGEXP_DEBUG_LOG, "a");
    ఒకవేళ (debug == ఖాళీ)
    {
	semsg("(NFA) COULD NOT OPEN %s!", NFA_REGEXP_DEBUG_LOG);
	తిరిగి FALSE;
    }
#అయితే_ముగింపు
    nfa_సరిపోలిక = FALSE;

    // Allocate memory for the lists of nodes.
    కొలత = (prog->nstate + 1) * పరిమాణం(nfa_thread_T);

    // Reuse cached list buffers from prog when available (top-level call).
    // Recursive calls must allocate their own buffers.
    ఒకవేళ (toplevel && prog->listbuf[0] != ఖాళీ)
    {
	list[0].t = (nfa_thread_T *)prog->listbuf[0];
	list[1].t = (nfa_thread_T *)prog->listbuf[1];
	prog->listbuf[0] = ఖాళీ;
	prog->listbuf[1] = ఖాళీ;
    }
    లేకపోతే
    {
	list[0].t = alloc(కొలత);
	list[1].t = alloc(కొలత);
    }
    list[0].len = prog->nstate + 1;
    list[1].len = prog->nstate + 1;
    ఒకవేళ (list[0].t == ఖాళీ || list[1].t == ఖాళీ)
	వెళ్ళు theend;

#నిర్వచితమైతే ENABLE_LOG
    log_fd = fopen(NFA_REGEXP_నిరంతరం_LOG, "a");
    ఒకవేళ (log_fd == ఖాళీ)
    {
	emsg(_(e_log_open_failed));
	log_fd = stderr;
    }
    fprintf(log_fd, "**********************************\n");
    nfa_సమితి_సంకేతం(ప్రారంభం->c);
    fprintf(log_fd, " RUNNING nfa_regmatch() starting with state %d, code %s\n",
    నిరపేక్ష(ప్రారంభం->id), సంకేతం_2);
    fprintf(log_fd, "**********************************\n");
#అయితే_ముగింపు

    thislist = &list[0];
    thislist->n = 0;
    thislist->has_pim = FALSE;
    nextlist = &list[1];
    nextlist->n = 0;
    nextlist->has_pim = FALSE;
#నిర్వచితమైతే ENABLE_LOG
    fprintf(log_fd, "(---) STARTSTATE first\n");
#అయితే_ముగింపు
    thislist->id = rex.nfa_listid + 1;

    // Inline optimized code for addstate(thislist, start, m, 0) if we know
    // it's the first MOPEN.
    ఒకవేళ (toplevel)
    {
	ఒకవేళ (REG_MULTI)
	{
	    m->norm.list.multi[0].ప్రారంభం_lnum = rex.lnum;
	    m->norm.list.multi[0].ప్రారంభం_నిలువు = (colnr_T)(rex.ఆదానం - rex.పంక్తి);
	    m->norm.orig_ప్రారంభం_నిలువు = m->norm.list.multi[0].ప్రారంభం_నిలువు;
	}
	లేకపోతే
	    m->norm.list.పంక్తి[0].ప్రారంభం = rex.ఆదానం;
	m->norm.లోపల_use = 1;
	r = addstate(thislist, ప్రారంభం->బహిర్గతం, m, ఖాళీ, 0, 0);
    }
    లేకపోతే
	r = addstate(thislist, ప్రారంభం, m, ఖాళీ, 0, 0);
    ఒకవేళ (r == ఖాళీ)
    {
	nfa_సరిపోలిక = NFA_TOO_EXPENSIVE;
	వెళ్ళు theend;
    }

#నిర్వచించు	కూడు_STATE_IF_సరిపోలిక(state)	\
    ఒకవేళ (ఫలితం)				\
    {					\
	కూడు_state = state->బహిర్గతం;		\
	కూడు_off = clen;			\
    }

    /*
     * Run for each character.
     */
    కొరకు (;;)
    {
	పూర్ణాంకం	curc;
	పూర్ణాంకం	clen;

	ఒకవేళ (has_mbyte)
	{
	    // Fast path for an ASCII byte not followed by a composing
	    // character, matching the check in utfc_ptr2len().  Avoids two
	    // indirect calls for the common case.
	    ఒకవేళ (rex.ఆదానం[0] != NUL && rex.ఆదానం[0] < 0x80 && rex.ఆదానం[1] < 0x80)
	    {
		curc = rex.ఆదానం[0];
		clen = 1;
	    }
	    లేకపోతే
	    {
		curc = (*mb_ptr2char)(rex.ఆదానం);
		clen = (*mb_ptr2len)(rex.ఆదానం);
	    }
	}
	లేకపోతే
	{
	    curc = *rex.ఆదానం;
	    clen = 1;
	}
	ఒకవేళ (curc == NUL)
	{
	    clen = 0;
	    go_గమ్యం_nextline = FALSE;
	}

	// swap lists
	thislist = &list[flag];
	nextlist = &list[flag ^= 1];
	nextlist->n = 0;	    // clear nextlist
	nextlist->has_pim = FALSE;
	++rex.nfa_listid;
	ఒకవేళ (prog->re_engine == AUTOMATIC_ENGINE
		&& (rex.nfa_listid >= NFA_MAX_స్థితులు
#నిర్వచితమైతే FEAT_EVAL
		    || nfa_fail_for_testing
#అయితే_ముగింపు
		    ))
	{
	    // too many states, retry with old engine
	    nfa_సరిపోలిక = NFA_TOO_EXPENSIVE;
	    వెళ్ళు theend;
	}

	thislist->id = rex.nfa_listid;
	nextlist->id = rex.nfa_listid + 1;

#నిర్వచితమైతే ENABLE_LOG
	fprintf(log_fd, "------------------------------------------\n");
	fprintf(log_fd, ">>> Reginput is \"%s\"\n", rex.ఆదానం);
	fprintf(log_fd, ">>> Advanced one character... Current char is %c (code %d) \n", curc, (పూర్ణాంకం)curc);
	fprintf(log_fd, ">>> Thislist has %d states available: ", thislist->n);
	{
	    పూర్ణాంకం i;

	    కొరకు (i = 0; i < thislist->n; i++)
		fprintf(log_fd, "%d  ", నిరపేక్ష(thislist->t[i].state->id));
	}
	fprintf(log_fd, "\n");
#అయితే_ముగింపు

#నిర్వచితమైతే NFA_REGEXP_DEBUG_LOG
	fprintf(debug, "\n-------------------\n");
#అయితే_ముగింపు
	/*
	 * If the state lists are empty we can stop.
	 */
	ఒకవేళ (thislist->n == 0)
	    ఆపు;

	// compute nextlist
	కొరకు (listidx = 0; listidx < thislist->n; ++listidx)
	{
	    // If the list gets very long there probably is something wrong.
	    // At least allow interrupting with CTRL-C.  Only check once in a
	    // while, calling ui_breakcheck() for every state is too slow.
	    స్థిర పూర్ణాంకం	breakcheck_లెక్క = 0;  // using "static" makes it faster
	    ఒకవేళ (unlikely(++breakcheck_లెక్క >= 100000))
	    {
		ui_breakcheck();
		breakcheck_లెక్క = 0;
	    }
	    ఒకవేళ (got_int)
		ఆపు;
#నిర్వచితమైతే FEAT_RELTIME
	    ఒకవేళ (nfa_did_సమయం_బహిర్గతం())
		ఆపు;
#అయితే_ముగింపు
	    t = &thislist->t[listidx];

#నిర్వచితమైతే NFA_REGEXP_DEBUG_LOG
	    nfa_సమితి_సంకేతం(t->state->c);
	    fprintf(debug, "%s, ", సంకేతం_2);
#అయితే_ముగింపు
#నిర్వచితమైతే ENABLE_LOG
	    {
		పూర్ణాంకం నిలువు;

		ఒకవేళ (t->subs.norm.లోపల_use <= 0)
		    నిలువు = -1;
		లేకపోతే ఒకవేళ (REG_MULTI)
		    నిలువు = t->subs.norm.list.multi[0].ప్రారంభం_నిలువు;
		లేకపోతే
		    నిలువు = (పూర్ణాంకం)(t->subs.norm.list.పంక్తి[0].ప్రారంభం - rex.పంక్తి);
		nfa_సమితి_సంకేతం(t->state->c);
		fprintf(log_fd, "(%d) char %d %s (start col %d)%s... \n",
			నిరపేక్ష(t->state->id), (పూర్ణాంకం)t->state->c, సంకేతం_2, నిలువు,
			pim_info(&t->pim));
	    }
#అయితే_ముగింపు

	    /*
	     * Handle the possible codes of the current state.
	     * The most important is NFA_MATCH.
	     */
	    కూడు_state = ఖాళీ;
	    కూడు_ఇక్కడ = FALSE;
	    కూడు_లెక్క = 0;
	    ఎంపిక (t->state->c)
	    {
	    స్థితి NFA_సరిపోలిక:
	      {
		// If the match is not at the start of the line, ends before a
		// composing characters and rex.reg_icombine is not set, that
		// is not really a match.
		ఒకవేళ (enc_utf8 && !rex.reg_icombine
			     && rex.ఆదానం != rex.పంక్తి && utf_iscomposing(curc))
		    ఆపు;

		nfa_సరిపోలిక = TRUE;
		copy_sub(&submatch->norm, &t->subs.norm);
#నిర్వచితమైతే FEAT_SYN_HL
		ఒకవేళ (rex.nfa_has_zsubexpr)
		    copy_sub(&submatch->synt, &t->subs.synt);
#అయితే_ముగింపు
#నిర్వచితమైతే ENABLE_LOG
		log_subsexpr(&t->subs);
#అయితే_ముగింపు
		// Found the left-most longest match, do not look at any other
		// states at this position.  When the list of states is going
		// to be empty quit without advancing, so that "rex.input" is
		// correct.
		ఒకవేళ (nextlist->n == 0)
		    clen = 0;
		వెళ్ళు nextchar;
	      }

	    స్థితి NFA_చివర_INVISIBLE:
	    స్థితి NFA_చివర_INVISIBLE_NEG:
	    స్థితి NFA_చివర_నమూనా:
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
#నిర్వచితమైతే ENABLE_LOG
		ఒకవేళ (nfa_endp != ఖాళీ)
		{
		    ఒకవేళ (REG_MULTI)
			fprintf(log_fd, "Current lnum: %d, endp lnum: %d; current col: %d, endp col: %d\n",
				(పూర్ణాంకం)rex.lnum,
				(పూర్ణాంకం)nfa_endp->se_u.pos.lnum,
				(పూర్ణాంకం)(rex.ఆదానం - rex.పంక్తి),
				nfa_endp->se_u.pos.నిలువు);
		    లేకపోతే
			fprintf(log_fd, "Current col: %d, endp col: %d\n",
				(పూర్ణాంకం)(rex.ఆదానం - rex.పంక్తి),
				(పూర్ణాంకం)(nfa_endp->se_u.ptr - rex.ఆదానం));
		}
#అయితే_ముగింపు
		// If "nfa_endp" is set it's only a match if it ends at
		// "nfa_endp"
		ఒకవేళ (nfa_endp != ఖాళీ && (REG_MULTI
			? (rex.lnum != nfa_endp->se_u.pos.lnum
			    || (పూర్ణాంకం)(rex.ఆదానం - rex.పంక్తి)
						!= nfa_endp->se_u.pos.నిలువు)
			: rex.ఆదానం != nfa_endp->se_u.ptr))
		    ఆపు;

		// do not set submatches for \@!
		ఒకవేళ (t->state->c != NFA_చివర_INVISIBLE_NEG)
		{
		    copy_sub(&m->norm, &t->subs.norm);
#నిర్వచితమైతే FEAT_SYN_HL
		    ఒకవేళ (rex.nfa_has_zsubexpr)
			copy_sub(&m->synt, &t->subs.synt);
#అయితే_ముగింపు
		}
#నిర్వచితమైతే ENABLE_LOG
		fprintf(log_fd, "Match found:\n");
		log_subsexpr(m);
#అయితే_ముగింపు
		nfa_సరిపోలిక = TRUE;
		// See comment above at "goto nextchar".
		ఒకవేళ (nextlist->n == 0)
		    clen = 0;
		వెళ్ళు nextchar;

	    స్థితి NFA_ప్రారంభం_INVISIBLE:
	    స్థితి NFA_ప్రారంభం_INVISIBLE_మొదటి:
	    స్థితి NFA_ప్రారంభం_INVISIBLE_NEG:
	    స్థితి NFA_ప్రారంభం_INVISIBLE_NEG_మొదటి:
	    స్థితి NFA_ప్రారంభం_INVISIBLE_BEFORE:
	    స్థితి NFA_ప్రారంభం_INVISIBLE_BEFORE_మొదటి:
	    స్థితి NFA_ప్రారంభం_INVISIBLE_BEFORE_NEG:
	    స్థితి NFA_ప్రారంభం_INVISIBLE_BEFORE_NEG_మొదటి:
		{
#నిర్వచితమైతే ENABLE_LOG
		    fprintf(log_fd, "Failure chance invisible: %d, what follows: %d\n",
			    failure_chance(t->state->బహిర్గతం, 0),
			    failure_chance(t->state->out1->బహిర్గతం, 0));
#అయితే_ముగింపు
		    // Do it directly if there already is a PIM or when
		    // nfa_postprocess() detected it will work better.
		    ఒకవేళ (t->pim.ఫలితం != NFA_PIM_వాడని
			 || t->state->c == NFA_ప్రారంభం_INVISIBLE_మొదటి
			 || t->state->c == NFA_ప్రారంభం_INVISIBLE_NEG_మొదటి
			 || t->state->c == NFA_ప్రారంభం_INVISIBLE_BEFORE_మొదటి
			 || t->state->c == NFA_ప్రారంభం_INVISIBLE_BEFORE_NEG_మొదటి)
		    {
			పూర్ణాంకం లోపల_use = m->norm.లోపల_use;

			// Copy submatch info for the recursive call, opposite
			// of what happens on success below.
			copy_sub_off(&m->norm, &t->subs.norm);
#నిర్వచితమైతే FEAT_SYN_HL
			ఒకవేళ (rex.nfa_has_zsubexpr)
			    copy_sub_off(&m->synt, &t->subs.synt);
#అయితే_ముగింపు

			/*
			 * First try matching the invisible match, then what
			 * follows.
			 */
			ఫలితం = recursive_regmatch(t->state, ఖాళీ, prog,
					  submatch, m, &listids, &listids_len);
			ఒకవేళ (ఫలితం == NFA_TOO_EXPENSIVE)
			{
			    nfa_సరిపోలిక = ఫలితం;
			    వెళ్ళు theend;
			}

			// for \@! and \@<! it is a match when the result is
			// FALSE
			ఒకవేళ (ఫలితం != (t->state->c == NFA_ప్రారంభం_INVISIBLE_NEG
			       || t->state->c == NFA_ప్రారంభం_INVISIBLE_NEG_మొదటి
			       || t->state->c
					   == NFA_ప్రారంభం_INVISIBLE_BEFORE_NEG
			       || t->state->c
				     == NFA_ప్రారంభం_INVISIBLE_BEFORE_NEG_మొదటి))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&t->subs.norm, &m->norm);
#నిర్వచితమైతే FEAT_SYN_HL
			    ఒకవేళ (rex.nfa_has_zsubexpr)
				copy_sub_off(&t->subs.synt, &m->synt);
#అయితే_ముగింపు
			    // If the pattern has \ze and it matched in the
			    // sub pattern, use it.
			    copy_ze_off(&t->subs.norm, &m->norm);

			    // t->state->out1 is the corresponding
			    // END_INVISIBLE node; Add its out to the current
			    // list (zero-width match).
			    కూడు_ఇక్కడ = TRUE;
			    కూడు_state = t->state->out1->బహిర్గతం;
			}
			m->norm.లోపల_use = లోపల_use;
		    }
		    లేకపోతే
		    {
			nfa_pim_T pim;

			/*
			 * First try matching what follows.  Only if a match
			 * is found verify the invisible match matches.  Add a
			 * nfa_pim_T to the following states, it contains info
			 * about the invisible match.
			 */
			pim.state = t->state;
			pim.ఫలితం = NFA_PIM_TODO;
			pim.subs.norm.లోపల_use = 0;
#నిర్వచితమైతే FEAT_SYN_HL
			pim.subs.synt.లోపల_use = 0;
#అయితే_ముగింపు
			ఒకవేళ (REG_MULTI)
			{
			    pim.చివర.pos.నిలువు = (పూర్ణాంకం)(rex.ఆదానం - rex.పంక్తి);
			    pim.చివర.pos.lnum = rex.lnum;
			}
			లేకపోతే
			    pim.చివర.ptr = rex.ఆదానం;

			// t->state->out1 is the corresponding END_INVISIBLE
			// node; Add its out to the current list (zero-width
			// match).
			ఒకవేళ (addstate_ఇక్కడ(thislist, t->state->out1->బహిర్గతం,
					     &t->subs, &pim, &listidx) == ఖాళీ)
			{
			    nfa_సరిపోలిక = NFA_TOO_EXPENSIVE;
			    వెళ్ళు theend;
			}
		    }
		}
		ఆపు;

	    స్థితి NFA_ప్రారంభం_నమూనా:
	      {
		nfa_state_T *skip = ఖాళీ;
#నిర్వచితమైతే ENABLE_LOG
		పూర్ణాంకం	    skip_lid = 0;
#అయితే_ముగింపు

		// There is no point in trying to match the pattern if the
		// output state is not going to be added to the list.
		ఒకవేళ (state_లోపల_list(nextlist, t->state->out1->బహిర్గతం, &t->subs))
		{
		    skip = t->state->out1->బహిర్గతం;
#నిర్వచితమైతే ENABLE_LOG
		    skip_lid = nextlist->id;
#అయితే_ముగింపు
		}
		లేకపోతే ఒకవేళ (state_లోపల_list(nextlist,
					  t->state->out1->బహిర్గతం->బహిర్గతం, &t->subs))
		{
		    skip = t->state->out1->బహిర్గతం->బహిర్గతం;
#నిర్వచితమైతే ENABLE_LOG
		    skip_lid = nextlist->id;
#అయితే_ముగింపు
		}
		లేకపోతే ఒకవేళ (state_లోపల_list(thislist,
					  t->state->out1->బహిర్గతం->బహిర్గతం, &t->subs))
		{
		    skip = t->state->out1->బహిర్గతం->బహిర్గతం;
#నిర్వచితమైతే ENABLE_LOG
		    skip_lid = thislist->id;
#అయితే_ముగింపు
		}
		ఒకవేళ (skip != ఖాళీ)
		{
#నిర్వచితమైతే ENABLE_LOG
		    nfa_సమితి_సంకేతం(skip->c);
		    fprintf(log_fd, "> Not trying to match pattern, output state %d is already in list %d. char %d: %s\n",
			    నిరపేక్ష(skip->id), skip_lid, skip->c, సంకేతం_2);
#అయితే_ముగింపు
		    ఆపు;
		}
		// Copy submatch info to the recursive call, opposite of what
		// happens afterwards.
		copy_sub_off(&m->norm, &t->subs.norm);
#నిర్వచితమైతే FEAT_SYN_HL
		ఒకవేళ (rex.nfa_has_zsubexpr)
		    copy_sub_off(&m->synt, &t->subs.synt);
#అయితే_ముగింపు

		// First try matching the pattern.
		ఫలితం = recursive_regmatch(t->state, ఖాళీ, prog,
					  submatch, m, &listids, &listids_len);
		ఒకవేళ (ఫలితం == NFA_TOO_EXPENSIVE)
		{
		    nfa_సరిపోలిక = ఫలితం;
		    వెళ్ళు theend;
		}
		ఒకవేళ (ఫలితం)
		{
		    పూర్ణాంకం bytelen;

#నిర్వచితమైతే ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN matches:\n");
		    log_subsexpr(m);
#అయితే_ముగింపు
		    // Copy submatch info from the recursive call
		    copy_sub_off(&t->subs.norm, &m->norm);
#నిర్వచితమైతే FEAT_SYN_HL
		    ఒకవేళ (rex.nfa_has_zsubexpr)
			copy_sub_off(&t->subs.synt, &m->synt);
#అయితే_ముగింపు
		    // Now we need to skip over the matched text and then
		    // continue with what follows.
		    ఒకవేళ (REG_MULTI)
			// TODO: multi-line match
			bytelen = m->norm.list.multi[0].చివర_నిలువు
						  - (పూర్ణాంకం)(rex.ఆదానం - rex.పంక్తి);
		    లేకపోతే
			bytelen = (పూర్ణాంకం)(m->norm.list.పంక్తి[0].చివర - rex.ఆదానం);

#నిర్వచితమైతే ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN length: %d\n", bytelen);
#అయితే_ముగింపు
		    ఒకవేళ (bytelen == 0)
		    {
			// empty match, output of corresponding
			// NFA_END_PATTERN/NFA_SKIP to be used at current
			// position
			కూడు_ఇక్కడ = TRUE;
			కూడు_state = t->state->out1->బహిర్గతం->బహిర్గతం;
		    }
		    లేకపోతే ఒకవేళ (bytelen <= clen)
		    {
			// match current character, output of corresponding
			// NFA_END_PATTERN to be used at next position.
			కూడు_state = t->state->out1->బహిర్గతం->బహిర్గతం;
			కూడు_off = clen;
		    }
		    లేకపోతే
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			కూడు_state = t->state->out1->బహిర్గతం;
			కూడు_off = bytelen;
			కూడు_లెక్క = bytelen - clen;
		    }
		}
		ఆపు;
	      }

	    స్థితి NFA_BOL:
		ఒకవేళ (rex.ఆదానం == rex.పంక్తి)
		{
		    కూడు_ఇక్కడ = TRUE;
		    కూడు_state = t->state->బహిర్గతం;
		}
		ఆపు;

	    స్థితి NFA_EOL:
		ఒకవేళ (curc == NUL)
		{
		    కూడు_ఇక్కడ = TRUE;
		    కూడు_state = t->state->బహిర్గతం;
		}
		ఆపు;

	    స్థితి NFA_BOW:
		ఫలితం = TRUE;

		ఒకవేళ (curc == NUL)
		    ఫలితం = FALSE;
		లేకపోతే ఒకవేళ (has_mbyte)
		{
		    పూర్ణాంకం this_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.ఆదానం, rex.reg_buf);
		    ఒకవేళ (this_class <= 1)
			ఫలితం = FALSE;
		    లేకపోతే ఒకవేళ (reg_prev_class() == this_class)
			ఫలితం = FALSE;
		}
		లేకపోతే ఒకవేళ (!vim_iswordc_buf(curc, rex.reg_buf)
			   || (rex.ఆదానం > rex.పంక్తి
			       && vim_iswordc_buf(rex.ఆదానం[-1], rex.reg_buf)))
		    ఫలితం = FALSE;
		ఒకవేళ (ఫలితం)
		{
		    కూడు_ఇక్కడ = TRUE;
		    కూడు_state = t->state->బహిర్గతం;
		}
		ఆపు;

	    స్థితి NFA_EOW:
		ఫలితం = TRUE;
		ఒకవేళ (rex.ఆదానం == rex.పంక్తి)
		    ఫలితం = FALSE;
		లేకపోతే ఒకవేళ (has_mbyte)
		{
		    పూర్ణాంకం this_class, prev_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.ఆదానం, rex.reg_buf);
		    prev_class = reg_prev_class();
		    ఒకవేళ (this_class == prev_class
					|| prev_class == 0 || prev_class == 1)
			ఫలితం = FALSE;
		}
		లేకపోతే ఒకవేళ (!vim_iswordc_buf(rex.ఆదానం[-1], rex.reg_buf)
			|| (rex.ఆదానం[0] != NUL
					&& vim_iswordc_buf(curc, rex.reg_buf)))
		    ఫలితం = FALSE;
		ఒకవేళ (ఫలితం)
		{
		    కూడు_ఇక్కడ = TRUE;
		    కూడు_state = t->state->బహిర్గతం;
		}
		ఆపు;

	    స్థితి NFA_BOF:
		ఒకవేళ (rex.lnum == 0 && rex.ఆదానం == rex.పంక్తి
				     && (!REG_MULTI || rex.reg_firstlnum == 1))
		{
		    కూడు_ఇక్కడ = TRUE;
		    కూడు_state = t->state->బహిర్గతం;
		}
		ఆపు;

	    స్థితి NFA_EOF:
		ఒకవేళ (rex.lnum == rex.reg_maxline && curc == NUL)
		{
		    కూడు_ఇక్కడ = TRUE;
		    కూడు_state = t->state->బహిర్గతం;
		}
		ఆపు;

	    స్థితి NFA_COMPOSING:
	    {
		nfa_state_T *చివర;

		ఫలితం = సరిపోలిక_composing(t->state->బహిర్గతం, curc, clen);

		చివర = t->state->out1;	    // NFA_END_COMPOSING
		కూడు_STATE_IF_సరిపోలిక(చివర);
		ఆపు;
	    }

	    స్థితి NFA_NEWL:
		ఒకవేళ (curc == NUL && !rex.reg_పంక్తి_lbr && REG_MULTI
						&& rex.lnum <= rex.reg_maxline)
		{
		    go_గమ్యం_nextline = TRUE;
		    // Pass -1 for the offset, which means taking the position
		    // at the start of the next line.
		    కూడు_state = t->state->బహిర్గతం;
		    కూడు_off = -1;
		}
		లేకపోతే ఒకవేళ (curc == '\n' && rex.reg_పంక్తి_lbr)
		{
		    // match \n as if it is an ordinary character
		    కూడు_state = t->state->బహిర్గతం;
		    కూడు_off = 1;
		}
		ఆపు;

	    స్థితి NFA_ప్రారంభం_COLL:
	    స్థితి NFA_ప్రారంభం_NEG_COLL:
	      {
		// What follows is a list of characters, until NFA_END_COLL.
		// One of them must match or none of them must match.
		nfa_state_T	*state;
		పూర్ణాంకం		ఫలితం_if_matched;
		పూర్ణాంకం		c1, c2;

		// Never match EOL. If it's part of the collection it is added
		// as a separate state with an OR.
		ఒకవేళ (curc == NUL)
		    ఆపు;

		state = t->state->బహిర్గతం;
		ఫలితం_if_matched = (t->state->c == NFA_ప్రారంభం_COLL);
		కొరకు (;;)
		{
		    ఒకవేళ (state->c == NFA_COMPOSING)
		    {
			nfa_state_T *చివర;

			ఫలితం = సరిపోలిక_composing(t->state->బహిర్గతం->బహిర్గతం, curc,
									clen);

			ఒకవేళ (t->state->బహిర్గతం->out1 != ఖాళీ
				&& t->state->బహిర్గతం->out1->c == NFA_చివర_COMPOSING)
			{
			    చివర = t->state->బహిర్గతం->out1;
			    కూడు_STATE_IF_సరిపోలిక(చివర);
			}
			ఆపు;
		    }
		    ఒకవేళ (state->c == NFA_చివర_COLL)
		    {
			ఫలితం = !ఫలితం_if_matched;
			ఆపు;
		    }
		    ఒకవేళ (state->c == NFA_RANGE_MIN)
		    {
			c1 = state->val;
			state = state->బహిర్గతం; // advance to NFA_RANGE_MAX
			c2 = state->val;
#నిర్వచితమైతే ENABLE_LOG
			fprintf(log_fd, "NFA_RANGE_MIN curc=%d c1=%d c2=%d\n",
				curc, c1, c2);
#అయితే_ముగింపు
			ఒకవేళ (curc >= c1 && curc <= c2)
			{
			    ఫలితం = ఫలితం_if_matched;
			    ఆపు;
			}
			ఒకవేళ (rex.reg_ic)
			{
			    పూర్ణాంకం curc_దిగువ = MB_CASEFOLD(curc);
			    పూర్ణాంకం పూర్తి = FALSE;

			    కొరకు ( ; c1 <= c2; ++c1)
				ఒకవేళ (MB_CASEFOLD(c1) == curc_దిగువ)
				{
				    ఫలితం = ఫలితం_if_matched;
				    పూర్తి = TRUE;
				    ఆపు;
				}
			    ఒకవేళ (పూర్తి)
				ఆపు;
			}
		    }
		    లేకపోతే ఒకవేళ (state->c < 0 ? check_char_class(state->c, curc)
			       : (curc == state->c
				   || (rex.reg_ic && MB_CASEFOLD(curc)
						    == MB_CASEFOLD(state->c))))
		    {
			ఫలితం = ఫలితం_if_matched;
			ఆపు;
		    }
		    state = state->బహిర్గతం;
		}
		ఒకవేళ (ఫలితం)
		{
		    // next state is in out of the NFA_END_COLL, out1 of
		    // START points to the END state
		    కూడు_state = t->state->out1->బహిర్గతం;
		    కూడు_off = clen;
		}
		ఆపు;
	      }

	    స్థితి NFA_ANY:
		// Any char except '\0', (end of input) does not match.
		ఒకవేళ (curc > 0)
		{
		    కూడు_state = t->state->బహిర్గతం;
		    కూడు_off = clen;
		}
		ఆపు;

	    స్థితి NFA_ANY_COMPOSING:
		// On a composing character skip over it.  Otherwise do
		// nothing.  Always matches.
		ఒకవేళ (enc_utf8 && utf_iscomposing(curc))
		{
		    కూడు_off = clen;
		}
		లేకపోతే
		{
		    కూడు_ఇక్కడ = TRUE;
		    కూడు_off = 0;
		}
		కూడు_state = t->state->బహిర్గతం;
		ఆపు;

	    /*
	     * Character classes like \a for alpha, \d for digit etc.
	     */
	    స్థితి NFA_IDENT:	//  \i
		ఫలితం = vim_isIDc(curc);
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_SIDENT:	//  \I
		ఫలితం = !VIM_ISDIGIT(curc) && vim_isIDc(curc);
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_KWORD:	//  \k
		ఫలితం = vim_iswordp_buf(rex.ఆదానం, rex.reg_buf);
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_SKWORD:	//  \K
		ఫలితం = !VIM_ISDIGIT(curc)
				     && vim_iswordp_buf(rex.ఆదానం, rex.reg_buf);
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_FNAME:	//  \f
		ఫలితం = vim_isfilec(curc);
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_SFNAME:	//  \F
		ఫలితం = !VIM_ISDIGIT(curc) && vim_isfilec(curc);
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_PRINT:	//  \p
		ఫలితం = vim_isprintc(PTR2CHAR(rex.ఆదానం));
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_SPRINT:	//  \P
		ఫలితం = !VIM_ISDIGIT(curc) && vim_isprintc(PTR2CHAR(rex.ఆదానం));
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_WHITE:	//  \s
		ఫలితం = VIM_ISWHITE(curc);
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_NWHITE:	//  \S
		ఫలితం = curc != NUL && !VIM_ISWHITE(curc);
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_DIGIT:	//  \d
		ఫలితం = ri_digit(curc);
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_NDIGIT:	//  \D
		ఫలితం = curc != NUL && !ri_digit(curc);
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_HEX:	//  \x
		ఫలితం = ri_hex(curc);
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_NHEX:	//  \X
		ఫలితం = curc != NUL && !ri_hex(curc);
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_OCTAL:	//  \o
		ఫలితం = ri_octal(curc);
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_NOCTAL:	//  \O
		ఫలితం = curc != NUL && !ri_octal(curc);
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_మాట:	//  \w
		ఫలితం = ri_మాట(curc);
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_NWORD:	//  \W
		ఫలితం = curc != NUL && !ri_మాట(curc);
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_తల:	//  \h
		ఫలితం = ri_తల(curc);
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_NHEAD:	//  \H
		ఫలితం = curc != NUL && !ri_తల(curc);
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_ALPHA:	//  \a
		ఫలితం = ri_alpha(curc);
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_NALPHA:	//  \A
		ఫలితం = curc != NUL && !ri_alpha(curc);
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_క్రింది:	//  \l
		ఫలితం = ri_క్రింది(curc);
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_NLOWER:	//  \L
		ఫలితం = curc != NUL && !ri_క్రింది(curc);
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_పై:	//  \u
		ఫలితం = ri_పై(curc);
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_NUPPER:	// \U
		ఫలితం = curc != NUL && !ri_పై(curc);
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_క్రింది_IC:	// [a-z]
		ఫలితం = ri_క్రింది(curc) || (rex.reg_ic && ri_పై(curc));
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_NLOWER_IC:	// [^a-z]
		ఫలితం = curc != NUL
			&& !(ri_క్రింది(curc) || (rex.reg_ic && ri_పై(curc)));
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_పై_IC:	// [A-Z]
		ఫలితం = ri_పై(curc) || (rex.reg_ic && ri_క్రింది(curc));
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_NUPPER_IC:	// ^[A-Z]
		ఫలితం = curc != NUL
			&& !(ri_పై(curc) || (rex.reg_ic && ri_క్రింది(curc)));
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;

	    స్థితి NFA_BACKREF1:
	    స్థితి NFA_BACKREF2:
	    స్థితి NFA_BACKREF3:
	    స్థితి NFA_BACKREF4:
	    స్థితి NFA_BACKREF5:
	    స్థితి NFA_BACKREF6:
	    స్థితి NFA_BACKREF7:
	    స్థితి NFA_BACKREF8:
	    స్థితి NFA_BACKREF9:
#నిర్వచితమైతే FEAT_SYN_HL
	    స్థితి NFA_ZREF1:
	    స్థితి NFA_ZREF2:
	    స్థితి NFA_ZREF3:
	    స్థితి NFA_ZREF4:
	    స్థితి NFA_ZREF5:
	    స్థితి NFA_ZREF6:
	    స్థితి NFA_ZREF7:
	    స్థితి NFA_ZREF8:
	    స్థితి NFA_ZREF9:
#అయితే_ముగింపు
		// \1 .. \9  \z1 .. \z9
	      {
		పూర్ణాంకం subidx;
		పూర్ణాంకం bytelen;

#నిర్వచితమైతే FEAT_SYN_HL
		ఒకవేళ (t->state->c >= NFA_BACKREF1 && t->state->c <= NFA_BACKREF9)
#అయితే_ముగింపు
		{
		    subidx = t->state->c - NFA_BACKREF1 + 1;
		    ఫలితం = సరిపోలిక_backref(&t->subs.norm, subidx, &bytelen);
		}
#నిర్వచితమైతే FEAT_SYN_HL
		లేకపోతే
		{
		    subidx = t->state->c - NFA_ZREF1 + 1;
		    ఫలితం = సరిపోలిక_zref(subidx, &bytelen);
		}
#అయితే_ముగింపు

		ఒకవేళ (ఫలితం)
		{
		    ఒకవేళ (bytelen == 0)
		    {
			// empty match always works, output of NFA_SKIP to be
			// used next
			కూడు_ఇక్కడ = TRUE;
			కూడు_state = t->state->బహిర్గతం->బహిర్గతం;
		    }
		    లేకపోతే ఒకవేళ (bytelen <= clen)
		    {
			// match current character, jump ahead to out of
			// NFA_SKIP
			కూడు_state = t->state->బహిర్గతం->బహిర్గతం;
			కూడు_off = clen;
		    }
		    లేకపోతే
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			కూడు_state = t->state->బహిర్గతం;
			కూడు_off = bytelen;
			కూడు_లెక్క = bytelen - clen;
		    }
		}
		ఆపు;
	      }
	    స్థితి NFA_SKIP:
	      // character of previous matching \1 .. \9  or \@>
	      ఒకవేళ (t->లెక్క - clen <= 0)
	      {
		  // end of match, go to what follows
		  కూడు_state = t->state->బహిర్గతం;
		  కూడు_off = clen;
	      }
	      లేకపోతే
	      {
		  // add state again with decremented count
		  కూడు_state = t->state;
		  కూడు_off = 0;
		  కూడు_లెక్క = t->లెక్క - clen;
	      }
	      ఆపు;

	    స్థితి NFA_LNUM:
	    స్థితి NFA_LNUM_GT:
	    స్థితి NFA_LNUM_LT:
		ఫలితం = (REG_MULTI &&
			nfa_re_అంకె_cmp(t->state->val, t->state->c - NFA_LNUM,
			    (long_u)(rex.lnum + rex.reg_firstlnum)));
		ఒకవేళ (ఫలితం)
		{
		    కూడు_ఇక్కడ = TRUE;
		    కూడు_state = t->state->బహిర్గతం;
		}
		ఆపు;

	    స్థితి NFA_నిలువు:
	    స్థితి NFA_నిలువు_GT:
	    స్థితి NFA_నిలువు_LT:
		ఫలితం = nfa_re_అంకె_cmp(t->state->val, t->state->c - NFA_నిలువు,
			(long_u)(rex.ఆదానం - rex.పంక్తి) + 1);
		ఒకవేళ (ఫలితం)
		{
		    కూడు_ఇక్కడ = TRUE;
		    కూడు_state = t->state->బహిర్గతం;
		}
		ఆపు;

	    స్థితి NFA_VCOL:
	    స్థితి NFA_VCOL_GT:
	    స్థితి NFA_VCOL_LT:
		{
		    పూర్ణాంకం     op = t->state->c - NFA_VCOL;
		    colnr_T నిలువు = (colnr_T)(rex.ఆదానం - rex.పంక్తి);
		    win_T   *wp = rex.reg_win == ఖాళీ ? curwin : rex.reg_win;

		    // Bail out quickly when there can't be a match, avoid the
		    // overhead of win_linetabsize() on long lines.
		    ఒకవేళ (op != 1 && నిలువు > t->state->val
			    * (has_mbyte ? MB_MAXBYTES : 1))
			ఆపు;
		    ఫలితం = FALSE;
		    ఒకవేళ (op == 1 && నిలువు - 1 > t->state->val && నిలువు > 100)
		    {
			పూర్ణాంకం ts = wp->w_బఫరు->b_p_ts;

			// Guess that a character won't use more columns than
			// 'tabstop', with a minimum of 4.
			ఒకవేళ (ts < 4)
			    ts = 4;
			ఫలితం = నిలువు > t->state->val * ts;
		    }
		    ఒకవేళ (!ఫలితం)
		    {
			linenr_T    lnum = REG_MULTI
					    ? rex.reg_firstlnum + rex.lnum : 1;
			long_u	    vcol;

			ఒకవేళ (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క))
			    lnum = 1;
			vcol = (long_u)win_linetabsize(wp, lnum, rex.పంక్తి, నిలువు);
			ఫలితం = nfa_re_అంకె_cmp(t->state->val, op, vcol + 1);
		    }
		    ఒకవేళ (ఫలితం)
		    {
			కూడు_ఇక్కడ = TRUE;
			కూడు_state = t->state->బహిర్గతం;
		    }
		}
		ఆపు;

	    స్థితి NFA_MARK:
	    స్థితి NFA_MARK_GT:
	    స్థితి NFA_MARK_LT:
	      {
		pos_T	*pos;
		size_t	నిలువు = REG_MULTI ? rex.ఆదానం - rex.పంక్తి : 0;

		pos = getmark_buf(rex.reg_buf, t->state->val, FALSE);

		// Line may have been freed, get it again.
		ఒకవేళ (REG_MULTI)
		{
		    rex.పంక్తి = reg_getline(rex.lnum);
		    rex.ఆదానం = rex.పంక్తి + నిలువు;
		}

		// Compare the mark position to the match position, if the mark
		// exists and mark is set in reg_buf.
		ఒకవేళ (pos != ఖాళీ && pos->lnum > 0)
		{
		    colnr_T pos_నిలువు = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->నిలువు == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->నిలువు;

		    ఫలితం = (pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_నిలువు == (colnr_T)(rex.ఆదానం - rex.పంక్తి)
				    ? t->state->c == NFA_MARK
				    : (pos_నిలువు < (colnr_T)(rex.ఆదానం - rex.పంక్తి)
					? t->state->c == NFA_MARK_GT
					: t->state->c == NFA_MARK_LT))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? t->state->c == NFA_MARK_GT
				    : t->state->c == NFA_MARK_LT));
		    ఒకవేళ (ఫలితం)
		    {
			కూడు_ఇక్కడ = TRUE;
			కూడు_state = t->state->బహిర్గతం;
		    }
		}
		ఆపు;
	      }

	    స్థితి NFA_CURSOR:
		ఫలితం = (rex.reg_win != ఖాళీ
			&& (rex.lnum + rex.reg_firstlnum
						 == rex.reg_win->w_cursor.lnum)
			&& ((colnr_T)(rex.ఆదానం - rex.పంక్తి)
						== rex.reg_win->w_cursor.నిలువు));
		ఒకవేళ (ఫలితం)
		{
		    కూడు_ఇక్కడ = TRUE;
		    కూడు_state = t->state->బహిర్గతం;
		}
		ఆపు;

	    స్థితి NFA_VISUAL:
		ఫలితం = reg_సరిపోలిక_visual();
		ఒకవేళ (ఫలితం)
		{
		    కూడు_ఇక్కడ = TRUE;
		    కూడు_state = t->state->బహిర్గతం;
		}
		ఆపు;

	    స్థితి NFA_MOPEN1:
	    స్థితి NFA_MOPEN2:
	    స్థితి NFA_MOPEN3:
	    స్థితి NFA_MOPEN4:
	    స్థితి NFA_MOPEN5:
	    స్థితి NFA_MOPEN6:
	    స్థితి NFA_MOPEN7:
	    స్థితి NFA_MOPEN8:
	    స్థితి NFA_MOPEN9:
#నిర్వచితమైతే FEAT_SYN_HL
	    స్థితి NFA_ZOPEN:
	    స్థితి NFA_ZOPEN1:
	    స్థితి NFA_ZOPEN2:
	    స్థితి NFA_ZOPEN3:
	    స్థితి NFA_ZOPEN4:
	    స్థితి NFA_ZOPEN5:
	    స్థితి NFA_ZOPEN6:
	    స్థితి NFA_ZOPEN7:
	    స్థితి NFA_ZOPEN8:
	    స్థితి NFA_ZOPEN9:
#అయితే_ముగింపు
	    స్థితి NFA_NOPEN:
	    స్థితి NFA_ZSTART:
		// These states are only added to be able to bail out when
		// they are added again, nothing is to be done.
		ఆపు;

	    అప్రమేయ:	// regular character
	      {
		పూర్ణాంకం c = t->state->c;

#నిర్వచితమైతే DEBUG
		ఒకవేళ (c < 0)
		    siemsg("Negative state char: %ld", (దీర్ఘ)c);
#అయితే_ముగింపు
		ఫలితం = (c == curc);

		ఒకవేళ (!ఫలితం && rex.reg_ic)
		    ఫలితం = MB_CASEFOLD(c) == MB_CASEFOLD(curc);
		// If rex.reg_icombine is not set only skip over the character
		// itself.  When it is set skip over composing characters.
		ఒకవేళ (ఫలితం && enc_utf8 && !rex.reg_icombine)
		    clen = utf_ptr2len(rex.ఆదానం);
		కూడు_STATE_IF_సరిపోలిక(t->state);
		ఆపు;
	      }

	    } // switch (t->state->c)

	    ఒకవేళ (కూడు_state != ఖాళీ)
	    {
		nfa_pim_T *pim;
		nfa_pim_T pim_copy;

		ఒకవేళ (t->pim.ఫలితం == NFA_PIM_వాడని)
		    pim = ఖాళీ;
		లేకపోతే
		    pim = &t->pim;

		// Handle the postponed invisible match if the match might end
		// without advancing and before the end of the line.
		ఒకవేళ (pim != ఖాళీ && (clen == 0 || సరిపోలిక_follows(కూడు_state, 0)))
		{
		    ఒకవేళ (pim->ఫలితం == NFA_PIM_TODO)
		    {
#నిర్వచితమైతే ENABLE_LOG
			fprintf(log_fd, "\n");
			fprintf(log_fd, "==================================\n");
			fprintf(log_fd, "Postponed recursive nfa_regmatch()\n");
			fprintf(log_fd, "\n");
#అయితే_ముగింపు
			ఫలితం = recursive_regmatch(pim->state, pim,
				    prog, submatch, m, &listids, &listids_len);
			pim->ఫలితం = ఫలితం ? NFA_PIM_సరిపోలిక : NFA_PIM_NOMATCH;
			// for \@! and \@<! it is a match when the result is
			// FALSE
			ఒకవేళ (ఫలితం != (pim->state->c == NFA_ప్రారంభం_INVISIBLE_NEG
			     || pim->state->c == NFA_ప్రారంభం_INVISIBLE_NEG_మొదటి
			     || pim->state->c
					   == NFA_ప్రారంభం_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_ప్రారంభం_INVISIBLE_BEFORE_NEG_మొదటి))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&pim->subs.norm, &m->norm);
#నిర్వచితమైతే FEAT_SYN_HL
			    ఒకవేళ (rex.nfa_has_zsubexpr)
				copy_sub_off(&pim->subs.synt, &m->synt);
#అయితే_ముగింపు
			}
		    }
		    లేకపోతే
		    {
			ఫలితం = (pim->ఫలితం == NFA_PIM_సరిపోలిక);
#నిర్వచితమైతే ENABLE_LOG
			fprintf(log_fd, "\n");
			fprintf(log_fd, "Using previous recursive nfa_regmatch() result, result == %d\n", pim->ఫలితం);
			fprintf(log_fd, "MATCH = %s\n", ఫలితం == TRUE ? "OK" : "FALSE");
			fprintf(log_fd, "\n");
#అయితే_ముగింపు
		    }

		    // for \@! and \@<! it is a match when result is FALSE
		    ఒకవేళ (ఫలితం != (pim->state->c == NFA_ప్రారంభం_INVISIBLE_NEG
			     || pim->state->c == NFA_ప్రారంభం_INVISIBLE_NEG_మొదటి
			     || pim->state->c
					   == NFA_ప్రారంభం_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_ప్రారంభం_INVISIBLE_BEFORE_NEG_మొదటి))
		    {
			// Copy submatch info from the recursive call
			copy_sub_off(&t->subs.norm, &pim->subs.norm);
#నిర్వచితమైతే FEAT_SYN_HL
			ఒకవేళ (rex.nfa_has_zsubexpr)
			    copy_sub_off(&t->subs.synt, &pim->subs.synt);
#అయితే_ముగింపు
		    }
		    లేకపోతే
			// look-behind match failed, don't add the state
			కొనసాగు;

		    // Postponed invisible match was handled, don't add it to
		    // following states.
		    pim = ఖాళీ;
		}

		// If "pim" points into l->t it will become invalid when
		// adding the state causes the list to be reallocated.  Make a
		// local copy to avoid that.
		ఒకవేళ (pim == &t->pim)
		{
		    copy_pim(&pim_copy, pim);
		    pim = &pim_copy;
		}

		ఒకవేళ (కూడు_ఇక్కడ)
		    r = addstate_ఇక్కడ(thislist, కూడు_state, &t->subs,
								pim, &listidx);
		లేకపోతే
		{
		    r = addstate(nextlist, కూడు_state, &t->subs, pim, కూడు_off, 0);
		    ఒకవేళ (కూడు_లెక్క > 0)
			nextlist->t[nextlist->n - 1].లెక్క = కూడు_లెక్క;
		}
		ఒకవేళ (r == ఖాళీ)
		{
		    nfa_సరిపోలిక = NFA_TOO_EXPENSIVE;
		    వెళ్ళు theend;
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
	ఒకవేళ (nfa_సరిపోలిక == FALSE
		&& ((toplevel
			&& rex.lnum == 0
			&& clen != 0
			&& (rex.reg_maxcol == 0
			  || (colnr_T)(rex.ఆదానం - rex.పంక్తి) < rex.reg_maxcol))
		    || (nfa_endp != ఖాళీ
			&& (REG_MULTI
			    ? (rex.lnum < nfa_endp->se_u.pos.lnum
			       || (rex.lnum == nfa_endp->se_u.pos.lnum
				   && (పూర్ణాంకం)(rex.ఆదానం - rex.పంక్తి)
						    < nfa_endp->se_u.pos.నిలువు))
			    : rex.ఆదానం < nfa_endp->se_u.ptr))))
	{
#నిర్వచితమైతే ENABLE_LOG
	    fprintf(log_fd, "(---) STARTSTATE\n");
#అయితే_ముగింపు
	    // Inline optimized code for addstate() if we know the state is
	    // the first MOPEN.
	    ఒకవేళ (toplevel)
	    {
		పూర్ణాంకం కూడు = TRUE;
		పూర్ణాంకం c;

		ఒకవేళ (prog->regstart != NUL && clen != 0)
		{
		    ఒకవేళ (nextlist->n == 0)
		    {
			colnr_T నిలువు = (colnr_T)(rex.ఆదానం - rex.పంక్తి) + clen;

			// Nextlist is empty, we can skip ahead to the
			// character that must appear at the start.
			ఒకవేళ (skip_గమ్యం_ప్రారంభం(prog->regstart, &నిలువు) == FAIL)
			    ఆపు;
#నిర్వచితమైతే ENABLE_LOG
			fprintf(log_fd, "  Skipping ahead %d bytes to regstart\n",
				నిలువు - ((colnr_T)(rex.ఆదానం - rex.పంక్తి) + clen));
#అయితే_ముగింపు
			rex.ఆదానం = rex.పంక్తి + నిలువు - clen;
		    }
		    లేకపోతే
		    {
			// Checking if the required start character matches is
			// cheaper than adding a state that won't match.
			c = PTR2CHAR(rex.ఆదానం + clen);
			ఒకవేళ (c != prog->regstart && (!rex.reg_ic
			     || MB_CASEFOLD(c) != MB_CASEFOLD(prog->regstart)))
			{
#నిర్వచితమైతే ENABLE_LOG
			    fprintf(log_fd, "  Skipping start state, regstart does not match\n");
#అయితే_ముగింపు
			    కూడు = FALSE;
			}
		    }
		}

		ఒకవేళ (కూడు)
		{
		    ఒకవేళ (REG_MULTI)
		    {
			m->norm.list.multi[0].ప్రారంభం_నిలువు =
					 (colnr_T)(rex.ఆదానం - rex.పంక్తి) + clen;
			m->norm.orig_ప్రారంభం_నిలువు =
					       m->norm.list.multi[0].ప్రారంభం_నిలువు;
		    }
		    లేకపోతే
			m->norm.list.పంక్తి[0].ప్రారంభం = rex.ఆదానం + clen;
		    ఒకవేళ (addstate(nextlist, ప్రారంభం->బహిర్గతం, m, ఖాళీ, clen, 0) == ఖాళీ)
		    {
			nfa_సరిపోలిక = NFA_TOO_EXPENSIVE;
			వెళ్ళు theend;
		    }
		}
	    }
	    లేకపోతే
	    {
		char_u	    *save_పంక్తి = rex.పంక్తి;
		char_u	    *save_ఆదానం = rex.ఆదానం;
		linenr_T    save_lnum = rex.lnum;

		// At the end of a line the match can only start on the next
		// line, use that position instead of the line break.
		ఒకవేళ (REG_MULTI && clen == 0 && nfa_endp != ఖాళీ
					 && rex.lnum < nfa_endp->se_u.pos.lnum)
		{
		    char_u  *తదుపరి_పంక్తి = reg_getline(rex.lnum + 1);

		    ఒకవేళ (తదుపరి_పంక్తి != ఖాళీ)
		    {
			rex.పంక్తి = తదుపరి_పంక్తి;
			rex.ఆదానం = rex.పంక్తి;
			++rex.lnum;
		    }
		}

		r = addstate(nextlist, ప్రారంభం, m, ఖాళీ, clen, 0);

		rex.పంక్తి = save_పంక్తి;
		rex.ఆదానం = save_ఆదానం;
		rex.lnum = save_lnum;

		ఒకవేళ (r == ఖాళీ)
		{
		    nfa_సరిపోలిక = NFA_TOO_EXPENSIVE;
		    వెళ్ళు theend;
		}
	    }
	}

#నిర్వచితమైతే ENABLE_LOG
	fprintf(log_fd, ">>> Thislist had %d states available: ", thislist->n);
	{
	    పూర్ణాంకం i;

	    కొరకు (i = 0; i < thislist->n; i++)
		fprintf(log_fd, "%d  ", నిరపేక్ష(thislist->t[i].state->id));
	}
	fprintf(log_fd, "\n");
#అయితే_ముగింపు

nextchar:
	// Advance to the next character, or advance to the next line, or
	// finish.
	ఒకవేళ (clen != 0)
	    rex.ఆదానం += clen;
	లేకపోతే ఒకవేళ (go_గమ్యం_nextline || (nfa_endp != ఖాళీ && REG_MULTI
					&& rex.lnum < nfa_endp->se_u.pos.lnum))
	    reg_nextline();
	లేకపోతే
	    ఆపు;

	// Allow interrupting with CTRL-C.
	పంక్తి_breakcheck();
	ఒకవేళ (got_int)
	    ఆపు;
#నిర్వచితమైతే FEAT_RELTIME
	ఒకవేళ (nfa_did_సమయం_బహిర్గతం())
	    ఆపు;
#అయితే_ముగింపు
    }

#నిర్వచితమైతే ENABLE_LOG
    ఒకవేళ (log_fd != stderr)
	fclose(log_fd);
    log_fd = ఖాళీ;
#అయితే_ముగింపు

theend:
    // Cache list buffers in prog for reuse, or free if prog already has
    // cached buffers (recursive call case).
    ఒకవేళ (prog->listbuf[0] == ఖాళీ && list[0].t != ఖాళీ && list[1].t != ఖాళీ)
    {
	prog->listbuf[0] = list[0].t;
	prog->listbuf[1] = list[1].t;
    }
    లేకపోతే
    {
	vim_free(list[0].t);
	vim_free(list[1].t);
    }
    vim_free(listids);
#నిర్వచనం_రద్దు కూడు_STATE_IF_సరిపోలిక
#నిర్వచితమైతే NFA_REGEXP_DEBUG_LOG
    fclose(debug);
#అయితే_ముగింపు

    తిరిగి nfa_సరిపోలిక;
}

/*
 * Try match of "prog" with at rex.line["col"].
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    స్థిర దీర్ఘ
nfa_regtry(
    nfa_regprog_T   *prog,
    colnr_T	    నిలువు,
    పూర్ణాంకం		    *timed_బహిర్గతం వాడని)	// flag set on timeout or NULL
{
    పూర్ణాంకం		i;
    regsubs_T	subs, m;
    nfa_state_T	*ప్రారంభం = prog->ప్రారంభం;
    పూర్ణాంకం		ఫలితం;
#నిర్వచితమైతే ENABLE_LOG
    FILE	*f;
#అయితే_ముగింపు

    rex.ఆదానం = rex.పంక్తి + నిలువు;
#నిర్వచితమైతే FEAT_RELTIME
    nfa_timed_బహిర్గతం = timed_బహిర్గతం;
#అయితే_ముగింపు

#నిర్వచితమైతే ENABLE_LOG
    f = fopen(NFA_REGEXP_నిరంతరం_LOG, "a");
    ఒకవేళ (f != ఖాళీ)
    {
	fprintf(f, "\n\n\t=======================================================\n");
# ifdef DEBUG
	fprintf(f, "\tRegexp is \"%s\"\n", nfa_regengine.సమాసం);
# endif
	fprintf(f, "\tInput text is \"%s\" \n", rex.ఆదానం);
	fprintf(f, "\t=======================================================\n\n");
	nfa_print_state(f, ప్రారంభం);
	fprintf(f, "\n\n");
	fclose(f);
    }
    లేకపోతే
	emsg("Could not open temporary log file for writing");
#అయితే_ముగింపు

    clear_sub(&subs.norm);
    clear_sub(&m.norm);
#నిర్వచితమైతే FEAT_SYN_HL
    clear_sub(&subs.synt);
    clear_sub(&m.synt);
#అయితే_ముగింపు

    ఫలితం = nfa_regmatch(prog, ప్రారంభం, &subs, &m);
    ఒకవేళ (ఫలితం == FALSE)
	తిరిగి 0;
    లేకపోతే ఒకవేళ (ఫలితం == NFA_TOO_EXPENSIVE)
	తిరిగి ఫలితం;

    cleanup_subexpr();
    ఒకవేళ (REG_MULTI)
    {
	కొరకు (i = 0; i < subs.norm.లోపల_use; i++)
	{
	    rex.reg_startpos[i].lnum = subs.norm.list.multi[i].ప్రారంభం_lnum;
	    rex.reg_startpos[i].నిలువు = subs.norm.list.multi[i].ప్రారంభం_నిలువు;

	    rex.reg_endpos[i].lnum = subs.norm.list.multi[i].చివర_lnum;
	    rex.reg_endpos[i].నిలువు = subs.norm.list.multi[i].చివర_నిలువు;
	}
	ఒకవేళ (rex.reg_mmatch != ఖాళీ)
	    rex.reg_mmatch->rmm_matchcol = subs.norm.orig_ప్రారంభం_నిలువు;

	ఒకవేళ (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].నిలువు = నిలువు;
	}
	ఒకవేళ (rex.reg_endpos[0].lnum < 0)
	{
	    // pattern has a \ze but it didn't match, use current end
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].నిలువు = (పూర్ణాంకం)(rex.ఆదానం - rex.పంక్తి);
	}
	లేకపోతే
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    లేకపోతే
    {
	కొరకు (i = 0; i < subs.norm.లోపల_use; i++)
	{
	    rex.reg_startp[i] = subs.norm.list.పంక్తి[i].ప్రారంభం;
	    rex.reg_endp[i] = subs.norm.list.పంక్తి[i].చివర;
	}

	ఒకవేళ (rex.reg_startp[0] == ఖాళీ)
	    rex.reg_startp[0] = rex.పంక్తి + నిలువు;
	ఒకవేళ (rex.reg_endp[0] == ఖాళీ)
	    rex.reg_endp[0] = rex.ఆదానం;
    }

#నిర్వచితమైతే FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_బహిర్గతం);
    re_extmatch_బహిర్గతం = ఖాళీ;

    ఒకవేళ (prog->reghasz == REX_సమితి)
    {
	cleanup_zsubexpr();
	re_extmatch_బహిర్గతం = make_extmatch();
	ఒకవేళ (re_extmatch_బహిర్గతం == ఖాళీ)
	    తిరిగి 0;
	// Loop over \z1, \z2, etc.  There is no \z0.
	కొరకు (i = 1; i < subs.synt.లోపల_use; i++)
	{
	    ఒకవేళ (REG_MULTI)
	    {
		నిర్మాణం multipos *mpos = &subs.synt.list.multi[i];

		// Only accept single line matches that are valid.
		ఒకవేళ (mpos->ప్రారంభం_lnum >= 0
			&& mpos->ప్రారంభం_lnum == mpos->చివర_lnum
			&& mpos->చివర_నిలువు >= mpos->ప్రారంభం_నిలువు)
		    re_extmatch_బహిర్గతం->matches[i] =
			vim_strnsave(reg_getline(mpos->ప్రారంభం_lnum)
							    + mpos->ప్రారంభం_నిలువు,
					     mpos->చివర_నిలువు - mpos->ప్రారంభం_నిలువు);
	    }
	    లేకపోతే
	    {
		నిర్మాణం linepos *lpos = &subs.synt.list.పంక్తి[i];

		ఒకవేళ (lpos->ప్రారంభం != ఖాళీ && lpos->చివర != ఖాళీ)
		    re_extmatch_బహిర్గతం->matches[i] =
			    vim_strnsave(lpos->ప్రారంభం, lpos->చివర - lpos->ప్రారంభం);
	    }
	}
    }
#అయితే_ముగింపు

    తిరిగి 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 *
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    స్థిర దీర్ఘ
nfa_regexec_both(
    char_u	*పంక్తి,
    colnr_T	startcol,	// column to start looking for match
    పూర్ణాంకం		*timed_బహిర్గతం)	// flag set on timeout or NULL
{
    nfa_regprog_T   *prog;
    దీర్ఘ	    retval = 0L;
    పూర్ణాంకం		    i;
    colnr_T	    నిలువు = startcol;

    ఒకవేళ (REG_MULTI)
    {
	prog = (nfa_regprog_T *)rex.reg_mmatch->regprog;
	పంక్తి = reg_getline((linenr_T)0);    // relative to the cursor
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    లేకపోతే
    {
	prog = (nfa_regprog_T *)rex.reg_సరిపోలిక->regprog;
	rex.reg_startp = rex.reg_సరిపోలిక->startp;
	rex.reg_endp = rex.reg_సరిపోలిక->endp;
    }

    // Be paranoid...
    ఒకవేళ (prog == ఖాళీ || పంక్తి == ఖాళీ)
    {
	iemsg(e_null_వాదం);
	వెళ్ళు theend;
    }

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    ఒకవేళ (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    లేకపోతే ఒకవేళ (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    ఒకవేళ (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    rex.పంక్తి = పంక్తి;
    rex.lnum = 0;    // relative to line

    rex.nfa_has_zend = prog->has_zend;
    rex.nfa_has_backref = prog->has_backref;
    rex.nfa_nsubexpr = prog->nsubexp;
    rex.nfa_listid = 1;
    rex.nfa_alt_listid = 2;
#నిర్వచితమైతే DEBUG
    nfa_regengine.సమాసం = prog->నమూనా;
#అయితే_ముగింపు

    ఒకవేళ (prog->reganch && నిలువు > 0)
	తిరిగి 0L;

    rex.need_clear_subexpr = TRUE;
#నిర్వచితమైతే FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    ఒకవేళ (prog->reghasz == REX_సమితి)
    {
	rex.nfa_has_zsubexpr = TRUE;
	rex.need_clear_zsubexpr = TRUE;
    }
    లేకపోతే
    {
	rex.nfa_has_zsubexpr = FALSE;
	rex.need_clear_zsubexpr = FALSE;
    }
#అయితే_ముగింపు

    ఒకవేళ (prog->regstart != NUL)
    {
	// Skip ahead until a character we know the match must start with.
	// When there is none there is no match.
	ఒకవేళ (skip_గమ్యం_ప్రారంభం(prog->regstart, &నిలువు) == FAIL)
	    తిరిగి 0L;

	// If match_text is set it contains the full text that must match.
	// Nothing else to try. Doesn't handle combining chars well.
	ఒకవేళ (prog->సరిపోలిక_పాఠ్యం != ఖాళీ && *prog->సరిపోలిక_పాఠ్యం != NUL && !rex.reg_icombine)
	{
	    retval = కనుగొను_సరిపోలిక_పాఠ్యం(&నిలువు, prog->regstart, prog->సరిపోలిక_పాఠ్యం);
	    ఒకవేళ (REG_MULTI)
		rex.reg_mmatch->rmm_matchcol = నిలువు;
	    లేకపోతే
		rex.reg_సరిపోలిక->rm_matchcol = నిలువు;
	    తిరిగి retval;
	}
    }

    // If the start column is past the maximum column: no need to try.
    ఒకవేళ (rex.reg_maxcol > 0 && నిలువు >= rex.reg_maxcol)
	వెళ్ళు theend;

    // Set the "nstate" used by nfa_regcomp() to zero to trigger an error when
    // it's accidentally used during execution.
    nstate = 0;
    కొరకు (i = 0; i < prog->nstate; ++i)
    {
	prog->state[i].id = i;
	prog->state[i].lastlist[0] = 0;
	prog->state[i].lastlist[1] = 0;
    }

    retval = nfa_regtry(prog, నిలువు, timed_బహిర్గతం);

#నిర్వచితమైతే DEBUG
    nfa_regengine.సమాసం = ఖాళీ;
#అయితే_ముగింపు

theend:
    ఒకవేళ (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs and
	// \ze are used.
	ఒకవేళ (REG_MULTI)
	{
	    lpos_T *ప్రారంభం = &rex.reg_mmatch->startpos[0];
	    lpos_T *చివర = &rex.reg_mmatch->endpos[0];

	    ఒకవేళ (చివర->lnum < ప్రారంభం->lnum
			|| (చివర->lnum == ప్రారంభం->lnum && చివర->నిలువు < ప్రారంభం->నిలువు))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];
	}
	లేకపోతే
	{
	    ఒకవేళ (rex.reg_సరిపోలిక->endp[0] < rex.reg_సరిపోలిక->startp[0])
		rex.reg_సరిపోలిక->endp[0] = rex.reg_సరిపోలిక->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_సరిపోలిక->rm_matchcol = నిలువు;
	}
    }

    తిరిగి retval;
}

/*
 * Compile a regular expression into internal code for the NFA matcher.
 * Returns the program in allocated space.  Returns NULL for an error.
 */
    స్థిర regprog_T *
nfa_regcomp(char_u *సమాసం, పూర్ణాంకం re_flags)
{
    nfa_regprog_T	*prog = ఖాళీ;
    size_t		prog_కొలత;
    పూర్ణాంకం			*postfix;

    ఒకవేళ (సమాసం == ఖాళీ)
	తిరిగి ఖాళీ;

#నిర్వచితమైతే DEBUG
    nfa_regengine.సమాసం = సమాసం;
#అయితే_ముగింపు
    nfa_re_flags = re_flags;

    init_class_tab();

    ఒకవేళ (nfa_regcomp_ప్రారంభం(సమాసం, re_flags) == FAIL)
	తిరిగి ఖాళీ;

    // Build postfix form of the regexp. Needed to build the NFA
    // (and count its size).
    postfix = re2post();
    ఒకవేళ (postfix == ఖాళీ)
	వెళ్ళు fail;	    // Cascaded (syntax?) error

    /*
     * In order to build the NFA, we parse the input regexp twice:
     * 1. first pass to count size (so we can allocate space)
     * 2. second to emit code
     */
#నిర్వచితమైతే ENABLE_LOG
    {
	FILE *f = fopen(NFA_REGEXP_నిరంతరం_LOG, "a");

	ఒకవేళ (f != ఖాళీ)
	{
	    fprintf(f, "\n*****************************\n\n\n\n\tCompiling regexp \"%s\"... hold on !\n", సమాసం);
	    fclose(f);
	}
    }
#అయితే_ముగింపు

    /*
     * PASS 1
     * Count number of NFA states in "nstate". Do not build the NFA.
     */
    post2nfa(postfix, post_ptr, TRUE);

    // allocate the regprog with space for the compiled regexp
    prog_కొలత = offsetof(nfa_regprog_T, state) + పరిమాణం(nfa_state_T) * nstate;
    prog = alloc(prog_కొలత);
    ఒకవేళ (prog == ఖాళీ)
	వెళ్ళు fail;
    state_ptr = prog->state;
    prog->re_లోపల_use = FALSE;
    prog->listbuf[0] = ఖాళీ;
    prog->listbuf[1] = ఖాళీ;

    /*
     * PASS 2
     * Build the NFA
     */
    prog->ప్రారంభం = post2nfa(postfix, post_ptr, FALSE);
    ఒకవేళ (prog->ప్రారంభం == ఖాళీ)
	వెళ్ళు fail;

    prog->regflags = regflags;
    prog->engine = &nfa_regengine;
    prog->nstate = nstate;
    prog->has_zend = rex.nfa_has_zend;
    prog->has_backref = rex.nfa_has_backref;
    prog->nsubexp = regnpar;

    nfa_postprocess(prog);

    prog->reganch = nfa_get_reganch(prog->ప్రారంభం, 0);
    prog->regstart = nfa_get_regstart(prog->ప్రారంభం, 0);
    prog->సరిపోలిక_పాఠ్యం = nfa_get_సరిపోలిక_పాఠ్యం(prog->ప్రారంభం);

#నిర్వచితమైతే ENABLE_LOG
    nfa_postfix_dump(సమాసం, OK);
    nfa_dump(prog);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    prog->reghasz = re_has_z;
#అయితే_ముగింపు
    prog->నమూనా = vim_strsave(సమాసం);
#నిర్వచితమైతే DEBUG
    nfa_regengine.సమాసం = ఖాళీ;
#అయితే_ముగింపు

బహిర్గతం:
    // The postfix list and fragment stack are reused by the next compilation
    // (and freed in free_regexp_stuff()), but drop ones that grew large for a
    // big pattern to avoid holding much memory.
    ఒకవేళ (post_ప్రారంభం_len > NFA_POSTFIX_KEEP)
    {
	VIM_CLEAR(post_ప్రారంభం);
	post_ప్రారంభం_len = 0;
    }
    post_ptr = post_చివర = ఖాళీ;
    ఒకవేళ (nfa_దొంతర_len > NFA_దొంతర_KEEP)
    {
	VIM_CLEAR(nfa_దొంతర);
	nfa_దొంతర_len = 0;
    }
    state_ptr = ఖాళీ;
    తిరిగి (regprog_T *)prog;

fail:
    VIM_CLEAR(prog);
#నిర్వచితమైతే ENABLE_LOG
    nfa_postfix_dump(సమాసం, FAIL);
#అయితే_ముగింపు
#నిర్వచితమైతే DEBUG
    nfa_regengine.సమాసం = ఖాళీ;
#అయితే_ముగింపు
    వెళ్ళు బహిర్గతం;
}

/*
 * Free a compiled regexp program, returned by nfa_regcomp().
 */
    స్థిర శూన్య
nfa_regfree(regprog_T *prog)
{
    ఒకవేళ (prog == ఖాళీ)
	తిరిగి;

    vim_free(((nfa_regprog_T *)prog)->సరిపోలిక_పాఠ్యం);
    vim_free(((nfa_regprog_T *)prog)->నమూనా);
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
    స్థిర పూర్ణాంకం
nfa_regexec_nl(
    regmatch_T	*rmp,
    char_u	*పంక్తి,	// string to match against
    colnr_T	నిలువు,	// column to start looking for match
    పూర్ణాంకం		పంక్తి_lbr)
{
    rex.reg_సరిపోలిక = rmp;
    rex.reg_mmatch = ఖాళీ;
    rex.reg_maxline = 0;
    rex.reg_పంక్తి_lbr = పంక్తి_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = ఖాళీ;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;
    తిరిగి nfa_regexec_both(పంక్తి, నిలువు, ఖాళీ);
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
    స్థిర దీర్ఘ
nfa_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	నిలువు,		// column to start looking for match
    పూర్ణాంకం		*timed_బహిర్గతం)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    తిరిగి nfa_regexec_both(ఖాళీ, నిలువు, timed_బహిర్గతం);
}

#నిర్వచితమైతే DEBUG
# undef ENABLE_LOG
#అయితే_ముగింపు
