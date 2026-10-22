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
#اگر_تعریف_شدہ DEBUG
# define NFA_REGEXP_ERROR_LOG	"nfa_regexp_error.log"
# define ENABLE_LOG
# define NFA_REGEXP_DUMP_LOG	"nfa_regexp_dump.log"
# define NFA_REGEXP_تسلسل_LOG	"nfa_regexp_run.log"
# define NFA_REGEXP_DEBUG_LOG	"nfa_regexp_debug.log"
#اگر_ختم

// Added to NFA_ANY - NFA_NUPPER_IC to include a NL.
#تعریف_کرو NFA_جوڑو_NL		31

شمار
{
    NFA_بانٹو = -1024,
    NFA_مطابقت,
    NFA_خالی_خانے,			    // matches 0-length

    NFA_آغاز_COLL,		    // [abc] start
    NFA_انتہا_COLL,		    // [abc] end
    NFA_آغاز_NEG_COLL,		    // [^abc] start
    NFA_انتہا_NEG_COLL,		    // [^abc] end (postfix only)
    NFA_RANGE,			    // range of the two previous items
				    // (postfix only)
    NFA_RANGE_MIN,		    // low end of a range
    NFA_RANGE_MAX,		    // high end of a range

    NFA_CONCAT,			    // concatenate two previous items (postfix
				    // only)
    NFA_OR,			    // \| (postfix only)
    NFA_ستارہ,			    // greedy * (postfix only)
    NFA_ستارہ_NONGREEDY,		    // non-greedy * (postfix only)
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
    NFA_آغاز_INVISIBLE,
    NFA_آغاز_INVISIBLE_پہلا,
    NFA_آغاز_INVISIBLE_NEG,
    NFA_آغاز_INVISIBLE_NEG_پہلا,
    NFA_آغاز_INVISIBLE_BEFORE,
    NFA_آغاز_INVISIBLE_BEFORE_پہلا,
    NFA_آغاز_INVISIBLE_BEFORE_NEG,
    NFA_آغاز_INVISIBLE_BEFORE_NEG_پہلا,
    NFA_آغاز_نمونہ,
    NFA_انتہا_INVISIBLE,
    NFA_انتہا_INVISIBLE_NEG,
    NFA_انتہا_نمونہ,
    NFA_COMPOSING,		    // Next nodes in NFA are part of the
				    // composing multibyte char
    NFA_انتہا_COMPOSING,		    // End of a composing char in the NFA
    NFA_ANY_COMPOSING,		    // \%C: Any composing characters.
    NFA_OPT_CHARS,		    // \%[abc]

    // The following are used only in the postfix form, not in the NFA
    NFA_پچھلا_ATOM_NO_چوڑائی,	    // Used for \@=
    NFA_پچھلا_ATOM_NO_چوڑائی_NEG,	    // Used for \@!
    NFA_پچھلا_ATOM_JUST_BEFORE,	    // Used for \@<=
    NFA_پچھلا_ATOM_JUST_BEFORE_NEG,  // Used for \@<!
    NFA_پچھلا_ATOM_LIKE_نمونہ,	    // Used for \@>

    NFA_BACKREF1,		    // \1
    NFA_BACKREF2,		    // \2
    NFA_BACKREF3,		    // \3
    NFA_BACKREF4,		    // \4
    NFA_BACKREF5,		    // \5
    NFA_BACKREF6,		    // \6
    NFA_BACKREF7,		    // \7
    NFA_BACKREF8,		    // \8
    NFA_BACKREF9,		    // \9
#اگر_تعریف_شدہ FEAT_SYN_HL
    NFA_ZREF1,			    // \z1
    NFA_ZREF2,			    // \z2
    NFA_ZREF3,			    // \z3
    NFA_ZREF4,			    // \z4
    NFA_ZREF5,			    // \z5
    NFA_ZREF6,			    // \z6
    NFA_ZREF7,			    // \z7
    NFA_ZREF8,			    // \z8
    NFA_ZREF9,			    // \z9
#اگر_ختم
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

#اگر_تعریف_شدہ FEAT_SYN_HL
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
#اگر_ختم

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
    NFA_زیریں,		//	Match lowercase char
    NFA_NLOWER,		//	Match non-lowercase char
    NFA_بالائی,		//	Match uppercase char
    NFA_NUPPER,		//	Match non-uppercase char
    NFA_زیریں_IC,	//	Match [a-z]
    NFA_NLOWER_IC,	//	Match [^a-z]
    NFA_بالائی_IC,	//	Match [A-Z]
    NFA_NUPPER_IC,	//	Match [^A-Z]

    NFA_پہلا_NL = NFA_ANY + NFA_جوڑو_NL,
    NFA_آخری_NL = NFA_NUPPER_IC + NFA_جوڑو_NL,

    NFA_CURSOR,		//	Match cursor pos
    NFA_LNUM,		//	Match line number
    NFA_LNUM_GT,	//	Match > line number
    NFA_LNUM_LT,	//	Match < line number
    NFA_ستون,		//	Match cursor column
    NFA_ستون_GT,		//	Match > cursor column
    NFA_ستون_LT,		//	Match < cursor column
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
    NFA_CLASS_زیریں,
    NFA_CLASS_PRINT,
    NFA_CLASS_PUNCT,
    NFA_CLASS_SPACE,
    NFA_CLASS_بالائی,
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
ساکن ثابت صحیح nfa_classcodes[] = {
    NFA_ANY, NFA_IDENT, NFA_SIDENT, NFA_KWORD,NFA_SKWORD,
    NFA_FNAME, NFA_SFNAME, NFA_PRINT, NFA_SPRINT,
    NFA_WHITE, NFA_NWHITE, NFA_DIGIT, NFA_NDIGIT,
    NFA_HEX, NFA_NHEX, NFA_OCTAL, NFA_NOCTAL,
    NFA_لفظ, NFA_NWORD, NFA_سر, NFA_NHEAD,
    NFA_ALPHA, NFA_NALPHA, NFA_زیریں, NFA_NLOWER,
    NFA_بالائی, NFA_NUPPER
};

// Variables only used in nfa_regcomp() and descendants.
ساکن صحیح nfa_re_flags; // re_flags passed to nfa_regcomp()
ساکن صحیح *post_آغاز;  // holds the postfix form of r.e.
ساکن صحیح post_آغاز_len;  // size of allocated post_start (in ints)
ساکن صحیح *post_انتہا;
ساکن صحیح *post_ptr;
ساکن صحیح nfa_reg_parse_گہرائی;	// nesting depth in nfa_reg()

// The postfix list (post_start) and fragment stack (nfa_stack) are reused
// across compilations; a buffer that grew past these sizes for a big pattern
// is freed afterwards instead of being kept, to avoid holding much memory.
#تعریف_کرو NFA_POSTFIX_KEEP	10000	// number of ints (~40 Kbyte)
#تعریف_کرو NFA_ڈھیر_KEEP		4000	// number of Frag_T (~64 Kbyte)

// Set when the pattern should use the NFA engine.
// E.g. [[:upper:]] only allows 8bit characters for BT engine,
// while NFA engine handles multibyte characters correctly.
ساکن صحیح چاہتا_nfa;

ساکن صحیح nstate;	// Number of states in the NFA.
ساکن صحیح istate;	// Index in the state vector, used in alloc_state()

// If not NULL match must end at this position
ساکن save_se_T *nfa_endp = تہی;

// 0 for first call to nfa_regmatch(), 1 for recursive call.
ساکن صحیح nfa_ll_اشاریہ = 0;

ساکن صحیح realloc_post_list(خالی);
ساکن صحیح nfa_reg(صحیح paren);
#اگر_تعریف_شدہ DEBUG
ساکن خالی nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent);
#اگر_ختم
ساکن صحیح مطابقت_follows(nfa_state_T *startstate, صحیح گہرائی);
ساکن صحیح failure_chance(nfa_state_T *state, صحیح گہرائی);

// helper functions used when doing re2post() ... regatom() parsing
#تعریف_کرو EMIT(c)	کرو {				\
		    اگر (post_ptr >= post_انتہا && realloc_post_list() == FAIL) \
			واپس FAIL;		\
		    *post_ptr++ = c;		\
		} جب (0)

/*
 * Initialize internal variables before NFA compilation.
 * Return OK on success, FAIL otherwise.
 */
    ساکن صحیح
nfa_regcomp_آغاز(
    char_u	*اظہاریہ,
    صحیح		re_flags)	    // see vim_regcomp()
{
    size_t	postfix_سائز;
    صحیح		nstate_max;

    nstate = 0;
    istate = 0;
    // A reasonable estimation for maximum size
    nstate_max = (صحیح)(STRLEN(اظہاریہ) + 1) * 25;

    // Some items blow up in size, such as [A-z].  Add more space for that.
    // When it is still not enough realloc_post_list() will be used.
    nstate_max += 1000;

    // Size for postfix representation of expr.
    postfix_سائز = حجم(صحیح) * nstate_max;

    // Reuse the postfix buffer across compilations, only growing it when the
    // estimate exceeds the current size; it is freed in free_regexp_stuff().
    اگر (post_آغاز == تہی || post_آغاز_len < nstate_max)
    {
	vim_free(post_آغاز);
	post_آغاز = alloc(postfix_سائز);
	اگر (post_آغاز == تہی)
	{
	    post_آغاز_len = 0;
	    واپس FAIL;
	}
	post_آغاز_len = nstate_max;
    }
    post_ptr = post_آغاز;
    post_انتہا = post_آغاز + post_آغاز_len;
    چاہتا_nfa = FALSE;
    rex.nfa_has_zend = FALSE;
    rex.nfa_has_backref = FALSE;
    nfa_reg_parse_گہرائی = 0;

    // shared with BT engine
    regcomp_آغاز(اظہاریہ, re_flags);

    واپس OK;
}

/*
 * Figure out if the NFA state list starts with an anchor, must match at start
 * of the line.
 */
    ساکن صحیح
nfa_get_reganch(nfa_state_T *آغاز, صحیح گہرائی)
{
    nfa_state_T *p = آغاز;

    اگر (گہرائی > 4)
	واپس 0;

    جب (p != تہی)
    {
	انتخاب (p->c)
	{
	    حالت NFA_BOL:
	    حالت NFA_BOF:
		واپس 1; // yes!

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
#اگر_تعریف_شدہ FEAT_SYN_HL
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
#اگر_ختم
		p = p->برآمد;
		توڑو;

	    حالت NFA_بانٹو:
		واپس nfa_get_reganch(p->برآمد, گہرائی + 1)
				       && nfa_get_reganch(p->out1, گہرائی + 1);

	    طےشدہ:
		واپس 0; // noooo
	}
    }
    واپس 0;
}

/*
 * Figure out if the NFA state list starts with a character which must match
 * at start of the match.
 */
    ساکن صحیح
nfa_get_regstart(nfa_state_T *آغاز, صحیح گہرائی)
{
    nfa_state_T *p = آغاز;

    اگر (گہرائی > 4)
	واپس 0;

    جب (p != تہی)
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
	    حالت NFA_ستون:
	    حالت NFA_ستون_GT:
	    حالت NFA_ستون_LT:
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
#اگر_تعریف_شدہ FEAT_SYN_HL
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
#اگر_ختم
		p = p->برآمد;
		توڑو;

	    حالت NFA_بانٹو:
	    {
		صحیح c1 = nfa_get_regstart(p->برآمد, گہرائی + 1);
		صحیح c2 = nfa_get_regstart(p->out1, گہرائی + 1);

		اگر (c1 == c2)
		    واپس c1; // yes!
		واپس 0;
	    }

	    طےشدہ:
		اگر (p->c > 0)
		    واپس p->c; // yes!
		واپس 0;
	}
    }
    واپس 0;
}

/*
 * Figure out if the NFA state list contains just literal text and nothing
 * else.  If so return a string in allocated memory with what must match after
 * regstart.  Otherwise return NULL.
 */
    ساکن char_u *
nfa_get_مطابقت_متن(nfa_state_T *آغاز)
{
    nfa_state_T *p = آغاز;
    صحیح		len = 0;
    char_u	*ret;
    char_u	*s;

    اگر (p->c != NFA_MOPEN)
	واپس تہی; // just in case
    p = p->برآمد;
    جب (p->c > 0)
    {
	len += MB_CHAR2LEN(p->c);
	p = p->برآمد;
    }
    اگر (p->c != NFA_MCLOSE || p->برآمد->c != NFA_مطابقت)
	واپس تہی;

    ret = alloc(len);
    اگر (ret == تہی)
	واپس تہی;

    p = آغاز->برآمد->برآمد; // skip first char, it goes into regstart
    s = ret;
    جب (p->c > 0)
    {
	اگر (has_mbyte)
	    s += (*mb_char2bytes)(p->c, s);
	ورنہ
	    *s++ = p->c;
	p = p->برآمد;
    }
    *s = NUL;
    واپس ret;
}

/*
 * Allocate more space for post_start.  Called when
 * running above the estimated number of states.
 */
    ساکن صحیح
realloc_post_list(خالی)
{
    صحیح   nstate_max = (صحیح)(post_انتہا - post_آغاز);
    صحیح   new_max;
    صحیح   *new_آغاز;
    صحیح	  *old_آغاز;

    // For weird patterns the number of states can be very high. Increasing by
    // 50% seems a reasonable compromise between memory use and speed.
    new_max = nstate_max * 3 / 2;
    new_آغاز = ALLOC_MULT(صحیح, new_max);
    اگر (new_آغاز == تہی)
	واپس FAIL;
    mch_memmove(new_آغاز, post_آغاز, nstate_max * حجم(صحیح));
    old_آغاز = post_آغاز;
    post_آغاز = new_آغاز;
    post_آغاز_len = new_max;
    post_ptr = new_آغاز + (post_ptr - old_آغاز);
    post_انتہا = post_آغاز + new_max;
    vim_free(old_آغاز);
    واپس OK;
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
    ساکن صحیح
nfa_recognize_char_class(char_u *آغاز, char_u *انتہا, صحیح extra_newl)
{
    صحیح		newl;
    صحیح		config = get_char_class_بٹ(آغاز, انتہا, &newl);

    اگر (config < 0)
	واپس FAIL;

    اگر (extra_newl == TRUE)
	newl = TRUE;

    اگر (newl == TRUE)
	extra_newl = NFA_جوڑو_NL;

    انتخاب (config)
    {
	حالت CLASS_o9:
	    واپس extra_newl + NFA_DIGIT;
	حالت CLASS_not |  CLASS_o9:
	    واپس extra_newl + NFA_NDIGIT;
	حالت CLASS_af | CLASS_AF | CLASS_o9:
	    واپس extra_newl + NFA_HEX;
	حالت CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    واپس extra_newl + NFA_NHEX;
	حالت CLASS_o7:
	    واپس extra_newl + NFA_OCTAL;
	حالت CLASS_not | CLASS_o7:
	    واپس extra_newl + NFA_NOCTAL;
	حالت CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    واپس extra_newl + NFA_لفظ;
	حالت CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    واپس extra_newl + NFA_NWORD;
	حالت CLASS_az | CLASS_AZ | CLASS_underscore:
	    واپس extra_newl + NFA_سر;
	حالت CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    واپس extra_newl + NFA_NHEAD;
	حالت CLASS_az | CLASS_AZ:
	    واپس extra_newl + NFA_ALPHA;
	حالت CLASS_not | CLASS_az | CLASS_AZ:
	    واپس extra_newl + NFA_NALPHA;
	حالت CLASS_az:
	   واپس extra_newl + NFA_زیریں_IC;
	حالت CLASS_not | CLASS_az:
	    واپس extra_newl + NFA_NLOWER_IC;
	حالت CLASS_AZ:
	    واپس extra_newl + NFA_بالائی_IC;
	حالت CLASS_not | CLASS_AZ:
	    واپس extra_newl + NFA_NUPPER_IC;
    }
    واپس FAIL;
}

/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * Emits bytes in postfix notation: 'a,b,NFA_OR,c,NFA_OR' is
 * equivalent to 'a OR b OR c'
 *
 * NOTE! When changing this function, also update reg_equi_class()
 */
    ساکن صحیح
nfa_emit_equi_class(صحیح c)
{
#تعریف_کرو EMIT2(c)    EMIT(c); EMIT(NFA_CONCAT);

    اگر (enc_utf8 || STRCMP(p_enc, "latin1") == 0
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
		    واپس OK;

	    حالت 'B': حالت 0x181: حالت 0x243: حالت 0x1e02:
	    حالت 0x1e04: حالت 0x1e06:
		    EMIT2('B')
		    EMIT2(0x181) EMIT2(0x243) EMIT2(0x1e02)
		    EMIT2(0x1e04) EMIT2(0x1e06)
		    واپس OK;

	    حالت 'C': حالت C_cedilla: حالت 0x106: حالت 0x108:
	    حالت 0x10a: حالت 0x10c: حالت 0x187: حالت 0x23b:
	    حالت 0x1e08: حالت 0xa792:
		    EMIT2('C') EMIT2(C_cedilla)
		    EMIT2(0x106) EMIT2(0x108) EMIT2(0x10a)
		    EMIT2(0x10c) EMIT2(0x187) EMIT2(0x23b)
		    EMIT2(0x1e08) EMIT2(0xa792)
		    واپس OK;

	    حالت 'D': حالت 0x10e: حالت 0x110: حالت 0x18a:
	    حالت 0x1e0a: حالت 0x1e0c: حالت 0x1e0e: حالت 0x1e10:
	    حالت 0x1e12:
		    EMIT2('D') EMIT2(0x10e) EMIT2(0x110) EMIT2(0x18a)
		    EMIT2(0x1e0a) EMIT2(0x1e0c) EMIT2(0x1e0e)
		    EMIT2(0x1e10) EMIT2(0x1e12)
		    واپس OK;

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
		    واپس OK;

	    حالت 'F': حالت 0x191: حالت 0x1e1e: حالت 0xa798:
		    EMIT2('F') EMIT2(0x191) EMIT2(0x1e1e) EMIT2(0xa798)
		    واپس OK;

	    حالت 'G': حالت 0x11c: حالت 0x11e: حالت 0x120:
	    حالت 0x122: حالت 0x193: حالت 0x1e4: حالت 0x1e6:
	    حالت 0x1f4: حالت 0x1e20: حالت 0xa7a0:
		    EMIT2('G') EMIT2(0x11c) EMIT2(0x11e) EMIT2(0x120)
		    EMIT2(0x122) EMIT2(0x193) EMIT2(0x1e4)
		    EMIT2(0x1e6) EMIT2(0x1f4) EMIT2(0x1e20)
		    EMIT2(0xa7a0)
		    واپس OK;

	    حالت 'H': حالت 0x124: حالت 0x126: حالت 0x21e:
	    حالت 0x1e22: حالت 0x1e24: حالت 0x1e26: حالت 0x1e28:
	    حالت 0x1e2a: حالت 0x2c67:
		    EMIT2('H') EMIT2(0x124) EMIT2(0x126) EMIT2(0x21e)
		    EMIT2(0x1e22) EMIT2(0x1e24) EMIT2(0x1e26)
		    EMIT2(0x1e28) EMIT2(0x1e2a) EMIT2(0x2c67)
		    واپس OK;

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
		    واپس OK;

	    حالت 'J': حالت 0x134: حالت 0x248:
		    EMIT2('J') EMIT2(0x134) EMIT2(0x248)
		    واپس OK;

	    حالت 'K': حالت 0x136: حالت 0x198: حالت 0x1e8: حالت 0x1e30:
	    حالت 0x1e32: حالت 0x1e34: حالت 0x2c69: حالت 0xa740:
		    EMIT2('K') EMIT2(0x136) EMIT2(0x198) EMIT2(0x1e8)
		    EMIT2(0x1e30) EMIT2(0x1e32) EMIT2(0x1e34)
		    EMIT2(0x2c69) EMIT2(0xa740)
		    واپس OK;

	    حالت 'L': حالت 0x139: حالت 0x13b: حالت 0x13d:
	    حالت 0x13f: حالت 0x141: حالت 0x23d: حالت 0x1e36:
	    حالت 0x1e38: حالت 0x1e3a: حالت 0x1e3c: حالت 0x2c60:
		    EMIT2('L') EMIT2(0x139) EMIT2(0x13b)
		    EMIT2(0x13d) EMIT2(0x13f) EMIT2(0x141)
		    EMIT2(0x23d) EMIT2(0x1e36) EMIT2(0x1e38)
		    EMIT2(0x1e3a) EMIT2(0x1e3c) EMIT2(0x2c60)
		    واپس OK;

	    حالت 'M': حالت 0x1e3e: حالت 0x1e40: حالت 0x1e42:
		    EMIT2('M') EMIT2(0x1e3e) EMIT2(0x1e40)
		    EMIT2(0x1e42)
		    واپس OK;

	    حالت 'N': حالت N_virguilla:
	    حالت 0x143: حالت 0x145: حالت 0x147: حالت 0x1f8:
	    حالت 0x1e44: حالت 0x1e46: حالت 0x1e48: حالت 0x1e4a:
	    حالت 0xa7a4:
		    EMIT2('N') EMIT2(N_virguilla)
		    EMIT2(0x143) EMIT2(0x145) EMIT2(0x147)
		    EMIT2(0x1f8) EMIT2(0x1e44) EMIT2(0x1e46)
		    EMIT2(0x1e48) EMIT2(0x1e4a) EMIT2(0xa7a4)
		    واپس OK;

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
		    واپس OK;

	    حالت 'P': حالت 0x1a4: حالت 0x1e54: حالت 0x1e56: حالت 0x2c63:
		    EMIT2('P') EMIT2(0x1a4) EMIT2(0x1e54) EMIT2(0x1e56)
		    EMIT2(0x2c63)
		    واپس OK;

	    حالت 'Q': حالت 0x24a:
		    EMIT2('Q') EMIT2(0x24a)
		    واپس OK;

	    حالت 'R': حالت 0x154: حالت 0x156: حالت 0x158: حالت 0x210:
	    حالت 0x212: حالت 0x24c: حالت 0x1e58: حالت 0x1e5a:
	    حالت 0x1e5c: حالت 0x1e5e: حالت 0x2c64: حالت 0xa7a6:
		    EMIT2('R') EMIT2(0x154) EMIT2(0x156) EMIT2(0x158)
		    EMIT2(0x210) EMIT2(0x212) EMIT2(0x24c) EMIT2(0x1e58)
		    EMIT2(0x1e5a) EMIT2(0x1e5c) EMIT2(0x1e5e) EMIT2(0x2c64)
		    EMIT2(0xa7a6)
		    واپس OK;

	    حالت 'S': حالت 0x15a: حالت 0x15c: حالت 0x15e: حالت 0x160:
	    حالت 0x218: حالت 0x1e60: حالت 0x1e62: حالت 0x1e64:
	    حالت 0x1e66: حالت 0x1e68: حالت 0x2c7e: حالت 0xa7a8:
		    EMIT2('S') EMIT2(0x15a) EMIT2(0x15c) EMIT2(0x15e)
		    EMIT2(0x160) EMIT2(0x218) EMIT2(0x1e60) EMIT2(0x1e62)
		    EMIT2(0x1e64) EMIT2(0x1e66) EMIT2(0x1e68) EMIT2(0x2c7e)
		    EMIT2(0xa7a8)
		    واپس OK;

	    حالت 'T': حالت 0x162: حالت 0x164: حالت 0x166: حالت 0x1ac:
	    حالت 0x1ae: حالت 0x21a: حالت 0x23e: حالت 0x1e6a: حالت 0x1e6c:
	    حالت 0x1e6e: حالت 0x1e70:
		    EMIT2('T') EMIT2(0x162) EMIT2(0x164) EMIT2(0x166)
		    EMIT2(0x1ac) EMIT2(0x1ae) EMIT2(0x23e) EMIT2(0x21a)
		    EMIT2(0x1e6a) EMIT2(0x1e6c) EMIT2(0x1e6e) EMIT2(0x1e70)
		    واپس OK;

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
		    واپس OK;

	    حالت 'V': حالت 0x1b2: حالت 0x1e7c: حالت 0x1e7e:
		    EMIT2('V') EMIT2(0x1b2) EMIT2(0x1e7c) EMIT2(0x1e7e)
		    واپس OK;

	    حالت 'W': حالت 0x174: حالت 0x1e80: حالت 0x1e82: حالت 0x1e84:
	    حالت 0x1e86: حالت 0x1e88:
		    EMIT2('W') EMIT2(0x174) EMIT2(0x1e80) EMIT2(0x1e82)
		    EMIT2(0x1e84) EMIT2(0x1e86) EMIT2(0x1e88)
		    واپس OK;

	    حالت 'X': حالت 0x1e8a: حالت 0x1e8c:
		    EMIT2('X') EMIT2(0x1e8a) EMIT2(0x1e8c)
		    واپس OK;

	    حالت 'Y': حالت Y_acute: حالت 0x176: حالت 0x178:
	    حالت 0x1b3: حالت 0x232: حالت 0x24e: حالت 0x1e8e:
	    حالت 0x1ef2: حالت 0x1ef4: حالت 0x1ef6: حالت 0x1ef8:
		    EMIT2('Y') EMIT2(Y_acute)
		    EMIT2(0x176) EMIT2(0x178) EMIT2(0x1b3)
		    EMIT2(0x232) EMIT2(0x24e) EMIT2(0x1e8e)
		    EMIT2(0x1ef2) EMIT2(0x1ef4) EMIT2(0x1ef6)
		    EMIT2(0x1ef8)
		    واپس OK;

	    حالت 'Z': حالت 0x179: حالت 0x17b: حالت 0x17d:
	    حالت 0x1b5: حالت 0x1e90: حالت 0x1e92: حالت 0x1e94:
	    حالت 0x2c6b:
		    EMIT2('Z') EMIT2(0x179) EMIT2(0x17b) EMIT2(0x17d)
		    EMIT2(0x1b5) EMIT2(0x1e90) EMIT2(0x1e92)
		    EMIT2(0x1e94) EMIT2(0x2c6b)
		    واپس OK;

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
		    واپس OK;

	    حالت 'b': حالت 0x180: حالت 0x253: حالت 0x1d6c: حالت 0x1d80:
	    حالت 0x1e03: حالت 0x1e05: حالت 0x1e07:
		    EMIT2('b') EMIT2(0x180) EMIT2(0x253) EMIT2(0x1d6c)
		    EMIT2(0x1d80) EMIT2(0x1e03) EMIT2(0x1e05) EMIT2(0x1e07)
		    واپس OK;

	    حالت 'c': حالت c_cedilla: حالت 0x107: حالت 0x109: حالت 0x10b:
	    حالت 0x10d: حالت 0x188: حالت 0x23c: حالت 0x1e09: حالت 0xa793:
	    حالت 0xa794:
		    EMIT2('c') EMIT2(c_cedilla)
		    EMIT2(0x107) EMIT2(0x109) EMIT2(0x10b)
		    EMIT2(0x10d) EMIT2(0x188) EMIT2(0x23c)
		    EMIT2(0x1e09) EMIT2(0xa793) EMIT2(0xa794)
		    واپس OK;

	    حالت 'd': حالت 0x10f: حالت 0x111: حالت 0x257: حالت 0x1d6d:
	    حالت 0x1d81: حالت 0x1d91: حالت 0x1e0b: حالت 0x1e0d: حالت 0x1e0f:
	    حالت 0x1e11: حالت 0x1e13:
		    EMIT2('d') EMIT2(0x10f) EMIT2(0x111)
		    EMIT2(0x257) EMIT2(0x1d6d) EMIT2(0x1d81)
		    EMIT2(0x1d91) EMIT2(0x1e0b) EMIT2(0x1e0d)
		    EMIT2(0x1e0f) EMIT2(0x1e11) EMIT2(0x1e13)
		    واپس OK;

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
		    واپس OK;

	    حالت 'f': حالت 0x192: حالت 0x1d6e: حالت 0x1d82:
	    حالت 0x1e1f: حالت 0xa799:
		    EMIT2('f') EMIT2(0x192) EMIT2(0x1d6e) EMIT2(0x1d82)
		    EMIT2(0x1e1f) EMIT2(0xa799)
		    واپس OK;

	    حالت 'g': حالت 0x11d: حالت 0x11f: حالت 0x121: حالت 0x123:
	    حالت 0x1e5: حالت 0x1e7: حالت 0x1f5: حالت 0x260: حالت 0x1d83:
	    حالت 0x1e21: حالت 0xa7a1:
		    EMIT2('g') EMIT2(0x11d) EMIT2(0x11f) EMIT2(0x121)
		    EMIT2(0x123) EMIT2(0x1e5) EMIT2(0x1e7)
		    EMIT2(0x1f5) EMIT2(0x260) EMIT2(0x1d83)
		    EMIT2(0x1e21) EMIT2(0xa7a1)
		    واپس OK;

	    حالت 'h': حالت 0x125: حالت 0x127: حالت 0x21f: حالت 0x1e23:
	    حالت 0x1e25: حالت 0x1e27: حالت 0x1e29: حالت 0x1e2b:
	    حالت 0x1e96: حالت 0x2c68: حالت 0xa795:
		    EMIT2('h') EMIT2(0x125) EMIT2(0x127) EMIT2(0x21f)
		    EMIT2(0x1e23) EMIT2(0x1e25) EMIT2(0x1e27)
		    EMIT2(0x1e29) EMIT2(0x1e2b) EMIT2(0x1e96)
		    EMIT2(0x2c68) EMIT2(0xa795)
		    واپس OK;

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
		    واپس OK;

	    حالت 'j': حالت 0x135: حالت 0x1f0: حالت 0x249:
		    EMIT2('j') EMIT2(0x135) EMIT2(0x1f0) EMIT2(0x249)
		    واپس OK;

	    حالت 'k': حالت 0x137: حالت 0x199: حالت 0x1e9: حالت 0x1d84:
	    حالت 0x1e31: حالت 0x1e33: حالت 0x1e35: حالت 0x2c6a: حالت 0xa741:
		    EMIT2('k') EMIT2(0x137) EMIT2(0x199) EMIT2(0x1e9)
		    EMIT2(0x1d84) EMIT2(0x1e31) EMIT2(0x1e33)
		    EMIT2(0x1e35) EMIT2(0x2c6a) EMIT2(0xa741)
		    واپس OK;

	    حالت 'l': حالت 0x13a: حالت 0x13c: حالت 0x13e: حالت 0x140:
	    حالت 0x142: حالت 0x19a: حالت 0x1e37: حالت 0x1e39: حالت 0x1e3b:
	    حالت 0x1e3d: حالت 0x2c61:
		    EMIT2('l') EMIT2(0x13a) EMIT2(0x13c)
		    EMIT2(0x13e) EMIT2(0x140) EMIT2(0x142)
		    EMIT2(0x19a) EMIT2(0x1e37) EMIT2(0x1e39)
		    EMIT2(0x1e3b) EMIT2(0x1e3d) EMIT2(0x2c61)
		    واپس OK;

	    حالت 'm': حالت 0x1d6f: حالت 0x1e3f: حالت 0x1e41: حالت 0x1e43:
		    EMIT2('m') EMIT2(0x1d6f) EMIT2(0x1e3f)
		    EMIT2(0x1e41) EMIT2(0x1e43)
		    واپس OK;

	    حالت 'n': حالت n_virguilla: حالت 0x144: حالت 0x146: حالت 0x148:
	    حالت 0x149: حالت 0x1f9: حالت 0x1d70: حالت 0x1d87: حالت 0x1e45:
	    حالت 0x1e47: حالت 0x1e49: حالت 0x1e4b: حالت 0xa7a5:
		    EMIT2('n') EMIT2(n_virguilla)
		    EMIT2(0x144) EMIT2(0x146) EMIT2(0x148)
		    EMIT2(0x149) EMIT2(0x1f9) EMIT2(0x1d70)
		    EMIT2(0x1d87) EMIT2(0x1e45) EMIT2(0x1e47)
		    EMIT2(0x1e49) EMIT2(0x1e4b) EMIT2(0xa7a5)
		    واپس OK;

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
		    واپس OK;

	    حالت 'p': حالت 0x1a5: حالت 0x1d71: حالت 0x1d7d: حالت 0x1d88:
	    حالت 0x1e55: حالت 0x1e57:
		    EMIT2('p') EMIT2(0x1a5) EMIT2(0x1d71) EMIT2(0x1d7d)
		    EMIT2(0x1d88) EMIT2(0x1e55) EMIT2(0x1e57)
		    واپس OK;

	    حالت 'q': حالت 0x24b: حالت 0x2a0:
		    EMIT2('q') EMIT2(0x24b) EMIT2(0x2a0)
		    واپس OK;

	    حالت 'r': حالت 0x155: حالت 0x157: حالت 0x159: حالت 0x211:
	    حالت 0x213: حالت 0x24d: حالت 0x27d: حالت 0x1d72: حالت 0x1d73:
	    حالت 0x1d89: حالت 0x1e59: حالت 0x1e5b: حالت 0x1e5d: حالت 0x1e5f:
	    حالت 0xa7a7:
		    EMIT2('r') EMIT2(0x155) EMIT2(0x157) EMIT2(0x159)
		    EMIT2(0x211) EMIT2(0x213) EMIT2(0x24d) EMIT2(0x27d)
		    EMIT2(0x1d72) EMIT2(0x1d73) EMIT2(0x1d89) EMIT2(0x1e59)
		    EMIT2(0x1e5b) EMIT2(0x1e5d) EMIT2(0x1e5f) EMIT2(0xa7a7)
		    واپس OK;

	    حالت 's': حالت 0x15b: حالت 0x15d: حالت 0x15f: حالت 0x161:
	    حالت 0x219: حالت 0x23f: حالت 0x1d74: حالت 0x1d8a: حالت 0x1e61:
	    حالت 0x1e63: حالت 0x1e65: حالت 0x1e67: حالت 0x1e69: حالت 0xa7a9:
		    EMIT2('s') EMIT2(0x15b) EMIT2(0x15d) EMIT2(0x15f)
		    EMIT2(0x161) EMIT2(0x219) EMIT2(0x23f) EMIT2(0x1d74)
		    EMIT2(0x1d8a) EMIT2(0x1e61) EMIT2(0x1e63) EMIT2(0x1e65)
		    EMIT2(0x1e67) EMIT2(0x1e69) EMIT2(0xa7a9)
		    واپس OK;

	    حالت 't': حالت 0x163: حالت 0x165: حالت 0x167: حالت 0x1ab:
	    حالت 0x1ad: حالت 0x21b: حالت 0x288: حالت 0x1d75: حالت 0x1e6b:
	    حالت 0x1e6d: حالت 0x1e6f: حالت 0x1e71: حالت 0x1e97: حالت 0x2c66:
		    EMIT2('t') EMIT2(0x163) EMIT2(0x165) EMIT2(0x167)
		    EMIT2(0x1ab) EMIT2(0x1ad) EMIT2(0x21b) EMIT2(0x288)
		    EMIT2(0x1d75) EMIT2(0x1e6b) EMIT2(0x1e6d) EMIT2(0x1e6f)
		    EMIT2(0x1e71) EMIT2(0x1e97) EMIT2(0x2c66)
		    واپس OK;

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
		    واپس OK;

	    حالت 'v': حالت 0x28b: حالت 0x1d8c: حالت 0x1e7d: حالت 0x1e7f:
		    EMIT2('v') EMIT2(0x28b) EMIT2(0x1d8c) EMIT2(0x1e7d)
		    EMIT2(0x1e7f)
		    واپس OK;

	    حالت 'w': حالت 0x175: حالت 0x1e81: حالت 0x1e83: حالت 0x1e85:
	    حالت 0x1e87: حالت 0x1e89: حالت 0x1e98:
		    EMIT2('w') EMIT2(0x175) EMIT2(0x1e81) EMIT2(0x1e83)
		    EMIT2(0x1e85) EMIT2(0x1e87) EMIT2(0x1e89) EMIT2(0x1e98)
		    واپس OK;

	    حالت 'x': حالت 0x1e8b: حالت 0x1e8d:
		    EMIT2('x') EMIT2(0x1e8b) EMIT2(0x1e8d)
		    واپس OK;

	    حالت 'y': حالت y_acute: حالت y_diaeresis: حالت 0x177:
	    حالت 0x1b4: حالت 0x233: حالت 0x24f: حالت 0x1e8f:
	    حالت 0x1e99: حالت 0x1ef3: حالت 0x1ef5: حالت 0x1ef7:
	    حالت 0x1ef9:
		    EMIT2('y') EMIT2(y_acute) EMIT2(y_diaeresis)
		    EMIT2(0x177) EMIT2(0x1b4) EMIT2(0x233) EMIT2(0x24f)
		    EMIT2(0x1e8f) EMIT2(0x1e99) EMIT2(0x1ef3)
		    EMIT2(0x1ef5) EMIT2(0x1ef7) EMIT2(0x1ef9)
		    واپس OK;

	    حالت 'z': حالت 0x17a: حالت 0x17c: حالت 0x17e: حالت 0x1b6:
	    حالت 0x1d76: حالت 0x1d8e: حالت 0x1e91: حالت 0x1e93:
	    حالت 0x1e95: حالت 0x2c6c:
		    EMIT2('z') EMIT2(0x17a) EMIT2(0x17c) EMIT2(0x17e)
		    EMIT2(0x1b6) EMIT2(0x1d76) EMIT2(0x1d8e) EMIT2(0x1e91)
		    EMIT2(0x1e93) EMIT2(0x1e95) EMIT2(0x2c6c)
		    واپس OK;

	    // default: character itself
	}
    }

    EMIT2(c);
    واپس OK;
#تعریف_منسوخ EMIT2
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
    ساکن صحیح
nfa_regatom(خالی)
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
    صحیح		نتیجہ;
    صحیح		startc = -1;
    صحیح		save_پچھلا_at_آغاز = پچھلا_at_آغاز;

    c = getchr();
    انتخاب (c)
    {
	حالت NUL:
	    EMSG_RET_FAIL(_(e_nfa_regexp_انتہا_encountered_prematurely));

	حالت Magic('^'):
	    EMIT(NFA_BOL);
	    توڑو;

	حالت Magic('$'):
	    EMIT(NFA_EOL);
#اگر defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#اگر_ختم
	    توڑو;

	حالت Magic('<'):
	    EMIT(NFA_BOW);
	    توڑو;

	حالت Magic('>'):
	    EMIT(NFA_EOW);
	    توڑو;

	حالت Magic('_'):
	    c = no_Magic(getchr());
	    اگر (c == NUL)
		EMSG_RET_FAIL(_(e_nfa_regexp_انتہا_encountered_prematurely));

	    اگر (c == '^')	// "\_^" is start-of-line
	    {
		EMIT(NFA_BOL);
		توڑو;
	    }
	    اگر (c == '$')	// "\_$" is end-of-line
	    {
		EMIT(NFA_EOL);
#اگر defined(FEAT_SYN_HL)
		had_eol = TRUE;
#اگر_ختم
		توڑو;
	    }

	    extra = NFA_جوڑو_NL;

	    // "\_[" is collection plus newline
	    اگر (c == '[')
		جاؤ collection;

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
	    اگر (p == تہی)
	    {
		اگر (extra == NFA_جوڑو_NL)
		{
		    semsg(_(e_nfa_regexp_invalid_character_class_nr), c);
		    rc_did_emsg = TRUE;
		    واپس FAIL;
		}
		siemsg("Unknown character class char: %d", c);
		واپس FAIL;
	    }

	    // When '.' is followed by a composing char ignore the dot, so that
	    // the composing char is matched here.
	    اگر (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	    {
		old_regparse = regparse;
		c = getchr();
		جاؤ nfa_do_multibyte;
	    }
	    EMIT(nfa_classcodes[p - classchars]);
	    اگر (extra == NFA_جوڑو_NL)
	    {
		EMIT(NFA_NEWL);
		EMIT(NFA_OR);
		regflags |= RF_HASNL;
	    }
	    توڑو;

	حالت Magic('n'):
	    اگر (reg_string)
		// In a string "\n" matches a newline character.
		EMIT(NL);
	    ورنہ
	    {
		// In buffer text "\n" matches the end of a line.
		EMIT(NFA_NEWL);
		regflags |= RF_HASNL;
	    }
	    توڑو;

	حالت Magic('('):
	    اگر (nfa_reg(REG_PAREN) == FAIL)
		واپس FAIL;	    // cascaded error
	    توڑو;

	حالت Magic('|'):
	حالت Magic('&'):
	حالت Magic(')'):
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    واپس FAIL;

	حالت Magic('='):
	حالت Magic('?'):
	حالت Magic('+'):
	حالت Magic('@'):
	حالت Magic('*'):
	حالت Magic('{'):
	    // these should follow an atom, not form an atom
	    semsg(_(e_nfa_regexp_misplaced_chr), no_Magic(c));
	    واپس FAIL;

	حالت Magic('~'):
	    {
		char_u	    *lp;

		// Previous substitute pattern.
		// Generated as "\%(pattern\)".
		اگر (reg_پچھلا_sub == تہی)
		{
		    emsg(_(e_no_پچھلا_substitute_regular_عبارت));
		    واپس FAIL;
		}
		برائے (lp = reg_پچھلا_sub; *lp != NUL; MB_CPTR_ADV(lp))
		{
		    EMIT(PTR2CHAR(lp));
		    اگر (lp != reg_پچھلا_sub)
			EMIT(NFA_CONCAT);
		}
		EMIT(NFA_NOPEN);
		توڑو;
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

		اگر (!نظر_آیا_endbrace(refnum + 1))
		    واپس FAIL;
		EMIT(NFA_BACKREF1 + refnum);
		rex.nfa_has_backref = TRUE;
	    }
	    توڑو;

	حالت Magic('z'):
	    c = no_Magic(getchr());
	    انتخاب (c)
	    {
		حالت 's':
		    EMIT(NFA_ZSTART);
		    اگر (re_mult_اگلا("\\zs") == FAIL)
			واپس FAIL;
		    توڑو;
		حالت 'e':
		    EMIT(NFA_ZEND);
		    rex.nfa_has_zend = TRUE;
		    اگر (re_mult_اگلا("\\ze") == FAIL)
			واپس FAIL;
		    توڑو;
#اگر_تعریف_شدہ FEAT_SYN_HL
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
			EMSG_RET_FAIL(_(e_z1_z9_not_allowed_یہاں));
		    EMIT(NFA_ZREF1 + (no_Magic(c) - '1'));
		    // No need to set rex.nfa_has_backref, the sub-matches don't
		    // change when \z1 .. \z9 matches or not.
		    re_has_z = REX_USE;
		    توڑو;
		حالت '(':
		    // \z(
		    اگر ((reg_do_extmatch & REX_سیٹ) == 0)
			EMSG_RET_FAIL(_(e_z_not_allowed_یہاں));
		    اگر (nfa_reg(REG_ZPAREN) == FAIL)
			واپس FAIL;	    // cascaded error
		    re_has_z = REX_سیٹ;
		    توڑو;
#اگر_ختم
		طےشدہ:
		    semsg(_(e_nfa_regexp_unknown_operator_z_chr), no_Magic(c));
		    واپس FAIL;
	    }
	    توڑو;

	حالت Magic('%'):
	    c = no_Magic(getchr());
	    انتخاب (c)
	    {
		// () without a back reference
		حالت '(':
		    اگر (nfa_reg(REG_NPAREN) == FAIL)
			واپس FAIL;
		    EMIT(NFA_NOPEN);
		    توڑو;

		حالت 'd':   // %d123 decimal
		حالت 'o':   // %o123 octal
		حالت 'x':   // %xab hex 2
		حالت 'u':   // %uabcd hex 4
		حالت 'U':   // %U1234abcd hex 8
		    {
			vimlong_T nr;

			انتخاب (c)
			{
			    حالت 'd': nr = getdecchrs(); توڑو;
			    حالت 'o': nr = getoctchrs(); توڑو;
			    حالت 'x': nr = gethexchrs(2); توڑو;
			    حالت 'u': nr = gethexchrs(4); توڑو;
			    حالت 'U': nr = gethexchrs(8); توڑو;
			    طےشدہ:  nr = -1; توڑو;
			}

			اگر (nr < 0 || nr > زیادہ_سے_زیادہ_عدد)
			    EMSG2_RET_FAIL(_(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			// A NUL is stored in the text as NL
			// TODO: what if a composing character follows?
			EMIT(nr == 0 ? 0x0a : (طویل)nr);
		    }
		    توڑو;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		حالت '^':
		    EMIT(NFA_BOF);
		    توڑو;

		حالت '$':
		    EMIT(NFA_EOF);
		    توڑو;

		حالت '#':
		    اگر (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_آغاز_کا_نمونہ),
								  regparse[1]);
			واپس FAIL;
		    }
		    EMIT(NFA_CURSOR);
		    توڑو;

		حالت 'V':
		    EMIT(NFA_VISUAL);
		    توڑو;

		حالت 'C':
		    EMIT(NFA_ANY_COMPOSING);
		    توڑو;

		حالت '[':
		    {
			صحیح	    n;

			// \%[abc]
			برائے (n = 0; (c = peekchr()) != ']'; ++n)
			{
			    اگر (c == NUL)
				EMSG2_RET_FAIL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
			    // recursive call!
			    اگر (nfa_regatom() == FAIL)
				واپس FAIL;
			}
			getchr();  // get the ]
			اگر (n == 0)
			    EMSG2_RET_FAIL(_(e_خالی_خانے_str_brackets),
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
			توڑو;
		    }

		طےشدہ:
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
			جب (VIM_ISDIGIT(c))
			{
			    long_u tmp;

			    اگر (cur)
			    {
				semsg(_(e_regexp_نمبر_after_dot_pos_تلاش_chr),
								  no_Magic(c));
				واپس FAIL;
			    }
			    tmp = n * 10 + (c - '0');

			    اگر (tmp < n)
			    {
				// overflow.
				emsg(_(e_percent_قدر_too_large));
				واپس FAIL;
			    }
			    n = tmp;
			    c = getchr();
			    got_digit = TRUE;
			}
			اگر (c == 'l' || c == 'c' || c == 'v')
			{
			    long_u حد_بندی = زیادہ_سے_زیادہ_عدد;

			    اگر (!cur && !got_digit)
			    {
				semsg(_(e_nfa_regexp_missing_قدر_اندر_chr),
								  no_Magic(c));
				واپس FAIL;
			    }
			    اگر (c == 'l')
			    {
				اگر (cur)
				    n = curwin->w_cursor.lnum;
				// \%{n}l  \%{n}<l  \%{n}>l
				EMIT(cmp == '<' ? NFA_LNUM_LT :
				     cmp == '>' ? NFA_LNUM_GT : NFA_LNUM);
				اگر (save_پچھلا_at_آغاز)
				    at_آغاز = TRUE;
			    }
			    ورنہ اگر (c == 'c')
			    {
				اگر (cur)
				{
				    n = curwin->w_cursor.ستون;
				    n++;
				}
				// \%{n}c  \%{n}<c  \%{n}>c
				EMIT(cmp == '<' ? NFA_ستون_LT :
				     cmp == '>' ? NFA_ستون_GT : NFA_ستون);
			    }
			    ورنہ
			    {
				اگر (cur)
				{
				    colnr_T vcol = 0;

				    getvvcol(curwin, &curwin->w_cursor,
							 تہی, تہی, &vcol, 0);
				    n = ++vcol;
				}
				// \%{n}v  \%{n}<v  \%{n}>v
				EMIT(cmp == '<' ? NFA_VCOL_LT :
				     cmp == '>' ? NFA_VCOL_GT : NFA_VCOL);
				حد_بندی = زیادہ_سے_زیادہ_عدد / MB_MAXBYTES;
			    }
			    اگر (n >= حد_بندی)
			    {
				emsg(_(e_percent_قدر_too_large));
				واپس FAIL;
			    }
			    EMIT((صحیح)n);
			    توڑو;
			}
			ورنہ اگر (no_Magic(c) == '\'' && n == 0)
			{
			    // \%'m  \%<'m  \%>'m
			    EMIT(cmp == '<' ? NFA_MARK_LT :
				 cmp == '>' ? NFA_MARK_GT : NFA_MARK);
			    EMIT(getchr());
			    توڑو;
			}
		    }
		    semsg(_(e_nfa_regexp_unknown_operator_percent_chr),
								  no_Magic(c));
		    واپس FAIL;
	    }
	    توڑو;

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
		نتیجہ = nfa_recognize_char_class(regparse, endp,
							 extra == NFA_جوڑو_NL);
		اگر (نتیجہ != FAIL)
		{
		    اگر (نتیجہ >= NFA_پہلا_NL && نتیجہ <= NFA_آخری_NL)
		    {
			EMIT(نتیجہ - NFA_جوڑو_NL);
			EMIT(NFA_NEWL);
			EMIT(NFA_OR);
		    }
		    ورنہ
			EMIT(نتیجہ);
		    regparse = endp;
		    MB_PTR_ADV(regparse);
		    واپس OK;
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
		    EMIT(NFA_آغاز_NEG_COLL);
		}
		ورنہ
		    EMIT(NFA_آغاز_COLL);
		اگر (*regparse == '-')
		{
		    startc = '-';
		    EMIT(startc);
		    EMIT(NFA_CONCAT);
		    MB_PTR_ADV(regparse);
		}
		// Emit the OR branches for each character in the []
		emit_range = FALSE;
		جب (regparse < endp)
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
				    توڑو;
				حالت CLASS_ALPHA:
				    EMIT(NFA_CLASS_ALPHA);
				    توڑو;
				حالت CLASS_BLANK:
				    EMIT(NFA_CLASS_BLANK);
				    توڑو;
				حالت CLASS_CNTRL:
				    EMIT(NFA_CLASS_CNTRL);
				    توڑو;
				حالت CLASS_DIGIT:
				    EMIT(NFA_CLASS_DIGIT);
				    توڑو;
				حالت CLASS_گراف:
				    EMIT(NFA_CLASS_گراف);
				    توڑو;
				حالت CLASS_زیریں:
				    چاہتا_nfa = TRUE;
				    EMIT(NFA_CLASS_زیریں);
				    توڑو;
				حالت CLASS_PRINT:
				    EMIT(NFA_CLASS_PRINT);
				    توڑو;
				حالت CLASS_PUNCT:
				    EMIT(NFA_CLASS_PUNCT);
				    توڑو;
				حالت CLASS_SPACE:
				    EMIT(NFA_CLASS_SPACE);
				    توڑو;
				حالت CLASS_بالائی:
				    چاہتا_nfa = TRUE;
				    EMIT(NFA_CLASS_بالائی);
				    توڑو;
				حالت CLASS_XDIGIT:
				    EMIT(NFA_CLASS_XDIGIT);
				    توڑو;
				حالت CLASS_TAB:
				    EMIT(NFA_CLASS_TAB);
				    توڑو;
				حالت CLASS_RETURN:
				    EMIT(NFA_CLASS_RETURN);
				    توڑو;
				حالت CLASS_BACKSPACE:
				    EMIT(NFA_CLASS_BACKSPACE);
				    توڑو;
				حالت CLASS_ESCAPE:
				    EMIT(NFA_CLASS_ESCAPE);
				    توڑو;
				حالت CLASS_IDENT:
				    EMIT(NFA_CLASS_IDENT);
				    توڑو;
				حالت CLASS_KEYWORD:
				    EMIT(NFA_CLASS_KEYWORD);
				    توڑو;
				حالت CLASS_FNAME:
				    EMIT(NFA_CLASS_FNAME);
				    توڑو;
			    }
			    EMIT(NFA_CONCAT);
			    جاری;
			}
			// Try equivalence class [=a=] and the like
			اگر (equiclass != 0)
			{
			    نتیجہ = nfa_emit_equi_class(equiclass);
			    اگر (نتیجہ == FAIL)
			    {
				// should never happen
				EMSG_RET_FAIL(_(e_error_building_nfa_with_equivalence_class));
			    }
			    جاری;
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
			جاری;	    // reading the end of the range
		    }

		    // Now handle simple and escaped characters.
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    اگر (*regparse == '\\'
			    && !reg_cpo_bsl
			    && regparse + 1 <= endp
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != تہی
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR, regparse[1])
								      != تہی)
			    )
			)
		    {
			MB_PTR_ADV(regparse);

			اگر (*regparse == 'n')
			    startc = (reg_string || emit_range
					|| regparse[1] == '-') ? NL : NFA_NEWL;
			ورنہ اگر (*regparse == 'd'
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
				اگر (startc == زیادہ_سے_زیادہ_عدد)
				    EMSG_RET_FAIL(_(e_unicode_val_too_large));
				got_coll_char = TRUE;
				MB_PTR_عقب(old_regparse, regparse);
			    }
			    ورنہ
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
			    EMSG_RET_FAIL(_(e_الٹ_range_اندر_character_class));

			اگر (endc > startc + 2)
			{
			    // Emit a range instead of the sequence of
			    // individual characters.
			    اگر (startc == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    ورنہ
				--post_ptr; // remove NFA_CONCAT
			    EMIT(endc);
			    EMIT(NFA_RANGE);
			    EMIT(NFA_CONCAT);
			}
			ورنہ اگر (has_mbyte && ((*mb_char2len)(startc) > 1
				    || (*mb_char2len)(endc) > 1))
			{
			    // Emit the characters in the range.
			    // "startc" was already emitted, so skip it.
			    //
			    برائے (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			ورنہ
			{
			    // Emit the range. "startc" was already emitted, so
			    // skip it.
			    برائے (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			emit_range = FALSE;
			startc = -1;
		    }
		    ورنہ
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
				extra = NFA_جوڑو_NL;
			}
			ورنہ
			{
			    اگر (got_coll_char == TRUE && startc == 0)
			    {
				EMIT(0x0a);
				EMIT(NFA_CONCAT);
			    }
			    ورنہ
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
			برائے (;;)
			{
			    اگر (c == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    ورنہ
				EMIT(c);
			    EMIT(NFA_CONCAT);
			    اگر ((i += utf_char2len(c)) >= plen)
				توڑو;
			    c = utf_ptr2char(regparse + i);
			}
			EMIT(NFA_COMPOSING);
			EMIT(NFA_CONCAT);
		    }
		    MB_PTR_ADV(regparse);
		} // while (p < endp)

		MB_PTR_عقب(old_regparse, regparse);
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
		    EMIT(NFA_انتہا_NEG_COLL);
		ورنہ
		    EMIT(NFA_انتہا_COLL);

		// \_[] also matches \n but it's not negated
		اگر (extra == NFA_جوڑو_NL)
		{
		    EMIT(reg_string ? NL : NFA_NEWL);
		    EMIT(NFA_OR);
		}

		واپس OK;
	    } // if exists closing ]

	    اگر (reg_strict)
		EMSG_RET_FAIL(_(e_missing_rsb_after_str_lsb));
	    // FALLTHROUGH

	طےشدہ:
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
		    برائے (;;)
		    {
			EMIT(c);
			اگر (i > 0)
			    EMIT(NFA_CONCAT);
			اگر ((i += utf_char2len(c)) >= plen)
			    توڑو;
			c = utf_ptr2char(old_regparse + i);
		    }
		    EMIT(NFA_COMPOSING);
		    regparse = old_regparse + plen;
		}
		ورنہ
		{
		    c = no_Magic(c);
		    EMIT(c);
		}
		واپس OK;
	    }
    }

    واپس OK;
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
    ساکن صحیح
nfa_regpiece(خالی)
{
    صحیح		i;
    صحیح		op;
    صحیح		ret;
    طویل	minval, maxval;
    صحیح		greedy = TRUE;      // Braces are prefixed with '-' ?
    parse_state_T old_state;
    parse_state_T new_state;
    طویل	c2;
    صحیح		old_post_pos;
    صحیح		my_post_آغاز;
    صحیح		quest;

    // Save the current parse state, so that we can use it if <atom>{m,n} is
    // next.
    save_parse_state(&old_state);

    // store current pos in the postfix form, for \{m,n} involving 0s
    my_post_آغاز = (صحیح)(post_ptr - post_آغاز);

    ret = nfa_regatom();
    اگر (ret == FAIL)
	واپس FAIL;	    // cascaded error

    op = peekchr();
    اگر (re_multi_type(op) == NOT_MULTI)
	واپس OK;

    skipchr();
    انتخاب (op)
    {
	حالت Magic('*'):
	    EMIT(NFA_ستارہ);
	    توڑو;

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
		واپس FAIL;
	    EMIT(NFA_ستارہ);
	    EMIT(NFA_CONCAT);
	    skipchr();		// skip the \+
	    توڑو;

	حالت Magic('@'):
	    c2 = (طویل)getdecchrs();
	    op = no_Magic(getchr());
	    i = 0;
	    انتخاب(op)
	    {
		حالت '=':
		    // \@=
		    i = NFA_پچھلا_ATOM_NO_چوڑائی;
		    توڑو;
		حالت '!':
		    // \@!
		    i = NFA_پچھلا_ATOM_NO_چوڑائی_NEG;
		    توڑو;
		حالت '<':
		    op = no_Magic(getchr());
		    اگر (op == '=')
			// \@<=
			i = NFA_پچھلا_ATOM_JUST_BEFORE;
		    ورنہ اگر (op == '!')
			// \@<!
			i = NFA_پچھلا_ATOM_JUST_BEFORE_NEG;
		    توڑو;
		حالت '>':
		    // \@>
		    i = NFA_پچھلا_ATOM_LIKE_نمونہ;
		    توڑو;
	    }
	    اگر (i == 0)
	    {
		semsg(_(e_nfa_regexp_unknown_operator_at_chr), op);
		واپس FAIL;
	    }
	    EMIT(i);
	    اگر (i == NFA_پچھلا_ATOM_JUST_BEFORE
					|| i == NFA_پچھلا_ATOM_JUST_BEFORE_NEG)
		EMIT(c2);
	    توڑو;

	حالت Magic('?'):
	حالت Magic('='):
	    EMIT(NFA_QUEST);
	    توڑو;

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
	    اگر (minval == 0 && maxval == MAX_حد_بندی)
	    {
		اگر (greedy)		// { { (match the braces)
		    // \{}, \{0,}
		    EMIT(NFA_ستارہ);
		ورنہ			// { { (match the braces)
		    // \{-}, \{-0,}
		    EMIT(NFA_ستارہ_NONGREEDY);
		توڑو;
	    }

	    // Special case: x{0} or x{-0}
	    اگر (maxval == 0)
	    {
		// Ignore result of previous call to nfa_regatom()
		post_ptr = post_آغاز + my_post_آغاز;
		// NFA_EMPTY is 0-length and works everywhere
		EMIT(NFA_خالی_خانے);
		واپس OK;
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
				   && (maxval != MAX_حد_بندی && minval < 200)
				   && !چاہتا_nfa)
		واپس FAIL;

	    // Ignore previous call to nfa_regatom()
	    post_ptr = post_آغاز + my_post_آغاز;
	    // Save parse state after the repeated atom and the \{}
	    save_parse_state(&new_state);

	    quest = (greedy == TRUE? NFA_QUEST : NFA_QUEST_NONGREEDY);
	    برائے (i = 0; i < maxval; i++)
	    {
		// Goto beginning of the repeated atom
		restore_parse_state(&old_state);
		old_post_pos = (صحیح)(post_ptr - post_آغاز);
		اگر (nfa_regatom() == FAIL)
		    واپس FAIL;
		// after "minval" times, atoms are optional
		اگر (i + 1 > minval)
		{
		    اگر (maxval == MAX_حد_بندی)
		    {
			اگر (greedy)
			    EMIT(NFA_ستارہ);
			ورنہ
			    EMIT(NFA_ستارہ_NONGREEDY);
		    }
		    ورنہ
			EMIT(quest);
		}
		اگر (old_post_pos != my_post_آغاز)
		    EMIT(NFA_CONCAT);
		اگر (i + 1 > minval && maxval == MAX_حد_بندی)
		    توڑو;
	    }

	    // Go to just after the repeated atom and the \{}
	    restore_parse_state(&new_state);
	    curchr = -1;

	    توڑو;


	طےشدہ:
	    توڑو;
    }	// end switch

    اگر (re_multi_type(peekchr()) != NOT_MULTI)
	// Can't have a multi follow a multi.
	EMSG_RET_FAIL(_(e_nfa_regexp_cant_have_multi_follow_multi));

    واپس OK;
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
    ساکن صحیح
nfa_regconcat(خالی)
{
    صحیح		cont = TRUE;
    صحیح		پہلا = TRUE;

    جب (cont)
    {
	انتخاب (peekchr())
	{
	    حالت NUL:
	    حالت Magic('|'):
	    حالت Magic('&'):
	    حالت Magic(')'):
		cont = FALSE;
		توڑو;

	    حالت Magic('Z'):
		regflags |= RF_ICOMBINE;
		skipchr_keepstart();
		توڑو;
	    حالت Magic('c'):
		regflags |= RF_ICASE;
		skipchr_keepstart();
		توڑو;
	    حالت Magic('C'):
		regflags |= RF_NOICASE;
		skipchr_keepstart();
		توڑو;
	    حالت Magic('v'):
		reg_magic = MAGIC_ALL;
		skipchr_keepstart();
		curchr = -1;
		توڑو;
	    حالت Magic('m'):
		reg_magic = MAGIC_ON;
		skipchr_keepstart();
		curchr = -1;
		توڑو;
	    حالت Magic('M'):
		reg_magic = MAGIC_OFF;
		skipchr_keepstart();
		curchr = -1;
		توڑو;
	    حالت Magic('V'):
		reg_magic = MAGIC_NONE;
		skipchr_keepstart();
		curchr = -1;
		توڑو;

	    طےشدہ:
		اگر (nfa_regpiece() == FAIL)
		    واپس FAIL;
		اگر (پہلا == FALSE)
		    EMIT(NFA_CONCAT);
		ورنہ
		    پہلا = FALSE;
		توڑو;
	}
    }

    واپس OK;
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
    ساکن صحیح
nfa_regbranch(خالی)
{
    صحیح		old_post_pos;

    old_post_pos = (صحیح)(post_ptr - post_آغاز);

    // First branch, possibly the only one
    اگر (nfa_regconcat() == FAIL)
	واپس FAIL;

    // Try next concats
    جب (peekchr() == Magic('&'))
    {
	skipchr();
	// if concat is empty do emit a node
	اگر (old_post_pos == (صحیح)(post_ptr - post_آغاز))
	    EMIT(NFA_خالی_خانے);
	EMIT(NFA_NOPEN);
	EMIT(NFA_پچھلا_ATOM_NO_چوڑائی);
	old_post_pos = (صحیح)(post_ptr - post_آغاز);
	اگر (nfa_regconcat() == FAIL)
	    واپس FAIL;
	// if concat is empty do emit a node
	اگر (old_post_pos == (صحیح)(post_ptr - post_آغاز))
	    EMIT(NFA_خالی_خانے);
	EMIT(NFA_CONCAT);
    }

    // if a branch is empty, emit one node for it
    اگر (old_post_pos == (صحیح)(post_ptr - post_آغاز))
	EMIT(NFA_خالی_خانے);

    واپس OK;
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
    ساکن صحیح
nfa_reg(
    صحیح		paren)	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
{
    صحیح		parno = 0;
    صحیح		حالت_2 = FAIL;

    اگر (paren == REG_PAREN)
    {
	اگر (regnpar >= NSUBEXP) // Too many `('
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_parens));
	parno = regnpar++;
    }
#اگر_تعریف_شدہ FEAT_SYN_HL
    ورنہ اگر (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	اگر (regnzpar >= NSUBEXP)
	    EMSG_RET_FAIL(_(e_nfa_regexp_too_many_z));
	parno = regnzpar++;
    }
#اگر_ختم

    اگر (nfa_reg_parse_گہرائی >= REG_MAX_PAREN_گہرائی)
	EMSG_RET_FAIL(_(e_command_too_complex));
    ++nfa_reg_parse_گہرائی;

    اگر (nfa_regbranch() == FAIL)
	جاؤ theend;	    // cascaded error

    جب (peekchr() == Magic('|'))
    {
	skipchr();
	اگر (nfa_regbranch() == FAIL)
	    جاؤ theend;    // cascaded error
	EMIT(NFA_OR);
    }

    // Check for proper termination.
    اگر (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
	اگر (paren == REG_NPAREN)
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	ورنہ
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	rc_did_emsg = TRUE;
	جاؤ theend;
    }
    ورنہ اگر (paren == REG_NOPAREN && peekchr() != NUL)
    {
	اگر (peekchr() == Magic(')'))
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	ورنہ
	    emsg(_(e_nfa_regexp_proper_termination_error));
	rc_did_emsg = TRUE;
	جاؤ theend;
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
#اگر_تعریف_شدہ FEAT_SYN_HL
    ورنہ اگر (paren == REG_ZPAREN)
	EMIT(NFA_ZOPEN + parno);
#اگر_ختم

    حالت_2 = OK;

theend:
    --nfa_reg_parse_گہرائی;
    واپس حالت_2;
}

#اگر_تعریف_شدہ DEBUG
ساکن char_u رمز[50];

    ساکن خالی
nfa_سیٹ_رمز(صحیح c)
{
    صحیح	    addnl = FALSE;

    اگر (c >= NFA_پہلا_NL && c <= NFA_آخری_NL)
    {
	addnl = TRUE;
	c -= NFA_جوڑو_NL;
    }

    STRCPY(رمز, "");
    انتخاب (c)
    {
	حالت NFA_مطابقت:	    STRCPY(رمز, "NFA_MATCH "); توڑو;
	حالت NFA_بانٹو:	    STRCPY(رمز, "NFA_SPLIT "); توڑو;
	حالت NFA_CONCAT:    STRCPY(رمز, "NFA_CONCAT "); توڑو;
	حالت NFA_NEWL:	    STRCPY(رمز, "NFA_NEWL "); توڑو;
	حالت NFA_ZSTART:    STRCPY(رمز, "NFA_ZSTART"); توڑو;
	حالت NFA_ZEND:	    STRCPY(رمز, "NFA_ZEND"); توڑو;

	حالت NFA_BACKREF1:  STRCPY(رمز, "NFA_BACKREF1"); توڑو;
	حالت NFA_BACKREF2:  STRCPY(رمز, "NFA_BACKREF2"); توڑو;
	حالت NFA_BACKREF3:  STRCPY(رمز, "NFA_BACKREF3"); توڑو;
	حالت NFA_BACKREF4:  STRCPY(رمز, "NFA_BACKREF4"); توڑو;
	حالت NFA_BACKREF5:  STRCPY(رمز, "NFA_BACKREF5"); توڑو;
	حالت NFA_BACKREF6:  STRCPY(رمز, "NFA_BACKREF6"); توڑو;
	حالت NFA_BACKREF7:  STRCPY(رمز, "NFA_BACKREF7"); توڑو;
	حالت NFA_BACKREF8:  STRCPY(رمز, "NFA_BACKREF8"); توڑو;
	حالت NFA_BACKREF9:  STRCPY(رمز, "NFA_BACKREF9"); توڑو;
# ifdef FEAT_SYN_HL
	حالت NFA_ZREF1:	    STRCPY(رمز, "NFA_ZREF1"); توڑو;
	حالت NFA_ZREF2:	    STRCPY(رمز, "NFA_ZREF2"); توڑو;
	حالت NFA_ZREF3:	    STRCPY(رمز, "NFA_ZREF3"); توڑو;
	حالت NFA_ZREF4:	    STRCPY(رمز, "NFA_ZREF4"); توڑو;
	حالت NFA_ZREF5:	    STRCPY(رمز, "NFA_ZREF5"); توڑو;
	حالت NFA_ZREF6:	    STRCPY(رمز, "NFA_ZREF6"); توڑو;
	حالت NFA_ZREF7:	    STRCPY(رمز, "NFA_ZREF7"); توڑو;
	حالت NFA_ZREF8:	    STRCPY(رمز, "NFA_ZREF8"); توڑو;
	حالت NFA_ZREF9:	    STRCPY(رمز, "NFA_ZREF9"); توڑو;
# endif
	حالت NFA_SKIP:	    STRCPY(رمز, "NFA_SKIP"); توڑو;

	حالت NFA_پچھلا_ATOM_NO_چوڑائی:
			    STRCPY(رمز, "NFA_PREV_ATOM_NO_WIDTH"); توڑو;
	حالت NFA_پچھلا_ATOM_NO_چوڑائی_NEG:
			    STRCPY(رمز, "NFA_PREV_ATOM_NO_WIDTH_NEG"); توڑو;
	حالت NFA_پچھلا_ATOM_JUST_BEFORE:
			    STRCPY(رمز, "NFA_PREV_ATOM_JUST_BEFORE"); توڑو;
	حالت NFA_پچھلا_ATOM_JUST_BEFORE_NEG:
			 STRCPY(رمز, "NFA_PREV_ATOM_JUST_BEFORE_NEG"); توڑو;
	حالت NFA_پچھلا_ATOM_LIKE_نمونہ:
			    STRCPY(رمز, "NFA_PREV_ATOM_LIKE_PATTERN"); توڑو;

	حالت NFA_NOPEN:		    STRCPY(رمز, "NFA_NOPEN"); توڑو;
	حالت NFA_NCLOSE:	    STRCPY(رمز, "NFA_NCLOSE"); توڑو;
	حالت NFA_آغاز_INVISIBLE:   STRCPY(رمز, "NFA_START_INVISIBLE"); توڑو;
	حالت NFA_آغاز_INVISIBLE_پہلا:
			     STRCPY(رمز, "NFA_START_INVISIBLE_FIRST"); توڑو;
	حالت NFA_آغاز_INVISIBLE_NEG:
			       STRCPY(رمز, "NFA_START_INVISIBLE_NEG"); توڑو;
	حالت NFA_آغاز_INVISIBLE_NEG_پہلا:
			 STRCPY(رمز, "NFA_START_INVISIBLE_NEG_FIRST"); توڑو;
	حالت NFA_آغاز_INVISIBLE_BEFORE:
			    STRCPY(رمز, "NFA_START_INVISIBLE_BEFORE"); توڑو;
	حالت NFA_آغاز_INVISIBLE_BEFORE_پہلا:
		      STRCPY(رمز, "NFA_START_INVISIBLE_BEFORE_FIRST"); توڑو;
	حالت NFA_آغاز_INVISIBLE_BEFORE_NEG:
			STRCPY(رمز, "NFA_START_INVISIBLE_BEFORE_NEG"); توڑو;
	حالت NFA_آغاز_INVISIBLE_BEFORE_NEG_پہلا:
		  STRCPY(رمز, "NFA_START_INVISIBLE_BEFORE_NEG_FIRST"); توڑو;
	حالت NFA_آغاز_نمونہ:   STRCPY(رمز, "NFA_START_PATTERN"); توڑو;
	حالت NFA_انتہا_INVISIBLE:	    STRCPY(رمز, "NFA_END_INVISIBLE"); توڑو;
	حالت NFA_انتہا_INVISIBLE_NEG: STRCPY(رمز, "NFA_END_INVISIBLE_NEG"); توڑو;
	حالت NFA_انتہا_نمونہ:	    STRCPY(رمز, "NFA_END_PATTERN"); توڑو;

	حالت NFA_COMPOSING:	    STRCPY(رمز, "NFA_COMPOSING"); توڑو;
	حالت NFA_انتہا_COMPOSING:	    STRCPY(رمز, "NFA_END_COMPOSING"); توڑو;
	حالت NFA_OPT_CHARS:	    STRCPY(رمز, "NFA_OPT_CHARS"); توڑو;

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
	    STRCPY(رمز, "NFA_MOPEN(x)");
	    رمز[10] = c - NFA_MOPEN + '0';
	    توڑو;
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
	    STRCPY(رمز, "NFA_MCLOSE(x)");
	    رمز[11] = c - NFA_MCLOSE + '0';
	    توڑو;
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
	    STRCPY(رمز, "NFA_ZOPEN(x)");
	    رمز[10] = c - NFA_ZOPEN + '0';
	    توڑو;
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
	    STRCPY(رمز, "NFA_ZCLOSE(x)");
	    رمز[11] = c - NFA_ZCLOSE + '0';
	    توڑو;
# endif
	حالت NFA_EOL:		STRCPY(رمز, "NFA_EOL "); توڑو;
	حالت NFA_BOL:		STRCPY(رمز, "NFA_BOL "); توڑو;
	حالت NFA_EOW:		STRCPY(رمز, "NFA_EOW "); توڑو;
	حالت NFA_BOW:		STRCPY(رمز, "NFA_BOW "); توڑو;
	حالت NFA_EOF:		STRCPY(رمز, "NFA_EOF "); توڑو;
	حالت NFA_BOF:		STRCPY(رمز, "NFA_BOF "); توڑو;
	حالت NFA_LNUM:		STRCPY(رمز, "NFA_LNUM "); توڑو;
	حالت NFA_LNUM_GT:	STRCPY(رمز, "NFA_LNUM_GT "); توڑو;
	حالت NFA_LNUM_LT:	STRCPY(رمز, "NFA_LNUM_LT "); توڑو;
	حالت NFA_ستون:		STRCPY(رمز, "NFA_COL "); توڑو;
	حالت NFA_ستون_GT:	STRCPY(رمز, "NFA_COL_GT "); توڑو;
	حالت NFA_ستون_LT:	STRCPY(رمز, "NFA_COL_LT "); توڑو;
	حالت NFA_VCOL:		STRCPY(رمز, "NFA_VCOL "); توڑو;
	حالت NFA_VCOL_GT:	STRCPY(رمز, "NFA_VCOL_GT "); توڑو;
	حالت NFA_VCOL_LT:	STRCPY(رمز, "NFA_VCOL_LT "); توڑو;
	حالت NFA_MARK:		STRCPY(رمز, "NFA_MARK "); توڑو;
	حالت NFA_MARK_GT:	STRCPY(رمز, "NFA_MARK_GT "); توڑو;
	حالت NFA_MARK_LT:	STRCPY(رمز, "NFA_MARK_LT "); توڑو;
	حالت NFA_CURSOR:	STRCPY(رمز, "NFA_CURSOR "); توڑو;
	حالت NFA_VISUAL:	STRCPY(رمز, "NFA_VISUAL "); توڑو;
	حالت NFA_ANY_COMPOSING:	STRCPY(رمز, "NFA_ANY_COMPOSING "); توڑو;

	حالت NFA_ستارہ:		STRCPY(رمز, "NFA_STAR "); توڑو;
	حالت NFA_ستارہ_NONGREEDY: STRCPY(رمز, "NFA_STAR_NONGREEDY "); توڑو;
	حالت NFA_QUEST:		STRCPY(رمز, "NFA_QUEST"); توڑو;
	حالت NFA_QUEST_NONGREEDY: STRCPY(رمز, "NFA_QUEST_NON_GREEDY"); توڑو;
	حالت NFA_خالی_خانے:		STRCPY(رمز, "NFA_EMPTY"); توڑو;
	حالت NFA_OR:		STRCPY(رمز, "NFA_OR"); توڑو;

	حالت NFA_آغاز_COLL:	STRCPY(رمز, "NFA_START_COLL"); توڑو;
	حالت NFA_انتہا_COLL:	STRCPY(رمز, "NFA_END_COLL"); توڑو;
	حالت NFA_آغاز_NEG_COLL: STRCPY(رمز, "NFA_START_NEG_COLL"); توڑو;
	حالت NFA_انتہا_NEG_COLL:	STRCPY(رمز, "NFA_END_NEG_COLL"); توڑو;
	حالت NFA_RANGE:		STRCPY(رمز, "NFA_RANGE"); توڑو;
	حالت NFA_RANGE_MIN:	STRCPY(رمز, "NFA_RANGE_MIN"); توڑو;
	حالت NFA_RANGE_MAX:	STRCPY(رمز, "NFA_RANGE_MAX"); توڑو;

	حالت NFA_CLASS_ALNUM:	STRCPY(رمز, "NFA_CLASS_ALNUM"); توڑو;
	حالت NFA_CLASS_ALPHA:	STRCPY(رمز, "NFA_CLASS_ALPHA"); توڑو;
	حالت NFA_CLASS_BLANK:	STRCPY(رمز, "NFA_CLASS_BLANK"); توڑو;
	حالت NFA_CLASS_CNTRL:	STRCPY(رمز, "NFA_CLASS_CNTRL"); توڑو;
	حالت NFA_CLASS_DIGIT:	STRCPY(رمز, "NFA_CLASS_DIGIT"); توڑو;
	حالت NFA_CLASS_گراف:	STRCPY(رمز, "NFA_CLASS_GRAPH"); توڑو;
	حالت NFA_CLASS_زیریں:	STRCPY(رمز, "NFA_CLASS_LOWER"); توڑو;
	حالت NFA_CLASS_PRINT:	STRCPY(رمز, "NFA_CLASS_PRINT"); توڑو;
	حالت NFA_CLASS_PUNCT:	STRCPY(رمز, "NFA_CLASS_PUNCT"); توڑو;
	حالت NFA_CLASS_SPACE:	STRCPY(رمز, "NFA_CLASS_SPACE"); توڑو;
	حالت NFA_CLASS_بالائی:	STRCPY(رمز, "NFA_CLASS_UPPER"); توڑو;
	حالت NFA_CLASS_XDIGIT:	STRCPY(رمز, "NFA_CLASS_XDIGIT"); توڑو;
	حالت NFA_CLASS_TAB:	STRCPY(رمز, "NFA_CLASS_TAB"); توڑو;
	حالت NFA_CLASS_RETURN:	STRCPY(رمز, "NFA_CLASS_RETURN"); توڑو;
	حالت NFA_CLASS_BACKSPACE:   STRCPY(رمز, "NFA_CLASS_BACKSPACE"); توڑو;
	حالت NFA_CLASS_ESCAPE:	STRCPY(رمز, "NFA_CLASS_ESCAPE"); توڑو;
	حالت NFA_CLASS_IDENT:	STRCPY(رمز, "NFA_CLASS_IDENT"); توڑو;
	حالت NFA_CLASS_KEYWORD:	STRCPY(رمز, "NFA_CLASS_KEYWORD"); توڑو;
	حالت NFA_CLASS_FNAME:	STRCPY(رمز, "NFA_CLASS_FNAME"); توڑو;

	حالت NFA_ANY:	STRCPY(رمز, "NFA_ANY"); توڑو;
	حالت NFA_IDENT:	STRCPY(رمز, "NFA_IDENT"); توڑو;
	حالت NFA_SIDENT:STRCPY(رمز, "NFA_SIDENT"); توڑو;
	حالت NFA_KWORD:	STRCPY(رمز, "NFA_KWORD"); توڑو;
	حالت NFA_SKWORD:STRCPY(رمز, "NFA_SKWORD"); توڑو;
	حالت NFA_FNAME:	STRCPY(رمز, "NFA_FNAME"); توڑو;
	حالت NFA_SFNAME:STRCPY(رمز, "NFA_SFNAME"); توڑو;
	حالت NFA_PRINT:	STRCPY(رمز, "NFA_PRINT"); توڑو;
	حالت NFA_SPRINT:STRCPY(رمز, "NFA_SPRINT"); توڑو;
	حالت NFA_WHITE:	STRCPY(رمز, "NFA_WHITE"); توڑو;
	حالت NFA_NWHITE:STRCPY(رمز, "NFA_NWHITE"); توڑو;
	حالت NFA_DIGIT:	STRCPY(رمز, "NFA_DIGIT"); توڑو;
	حالت NFA_NDIGIT:STRCPY(رمز, "NFA_NDIGIT"); توڑو;
	حالت NFA_HEX:	STRCPY(رمز, "NFA_HEX"); توڑو;
	حالت NFA_NHEX:	STRCPY(رمز, "NFA_NHEX"); توڑو;
	حالت NFA_OCTAL:	STRCPY(رمز, "NFA_OCTAL"); توڑو;
	حالت NFA_NOCTAL:STRCPY(رمز, "NFA_NOCTAL"); توڑو;
	حالت NFA_لفظ:	STRCPY(رمز, "NFA_WORD"); توڑو;
	حالت NFA_NWORD:	STRCPY(رمز, "NFA_NWORD"); توڑو;
	حالت NFA_سر:	STRCPY(رمز, "NFA_HEAD"); توڑو;
	حالت NFA_NHEAD:	STRCPY(رمز, "NFA_NHEAD"); توڑو;
	حالت NFA_ALPHA:	STRCPY(رمز, "NFA_ALPHA"); توڑو;
	حالت NFA_NALPHA:STRCPY(رمز, "NFA_NALPHA"); توڑو;
	حالت NFA_زیریں:	STRCPY(رمز, "NFA_LOWER"); توڑو;
	حالت NFA_NLOWER:STRCPY(رمز, "NFA_NLOWER"); توڑو;
	حالت NFA_بالائی:	STRCPY(رمز, "NFA_UPPER"); توڑو;
	حالت NFA_NUPPER:STRCPY(رمز, "NFA_NUPPER"); توڑو;
	حالت NFA_زیریں_IC:  STRCPY(رمز, "NFA_LOWER_IC"); توڑو;
	حالت NFA_NLOWER_IC: STRCPY(رمز, "NFA_NLOWER_IC"); توڑو;
	حالت NFA_بالائی_IC:  STRCPY(رمز, "NFA_UPPER_IC"); توڑو;
	حالت NFA_NUPPER_IC: STRCPY(رمز, "NFA_NUPPER_IC"); توڑو;

	طےشدہ:
	    STRCPY(رمز, "CHAR(x)");
	    رمز[5] = c;
    }

    اگر (addnl == TRUE)
	STRCAT(رمز, " + NEWLINE ");

}

# ifdef ENABLE_LOG
ساکن FILE *log_fd;
ساکن char_u e_log_open_failed[] = N_("Could not open temporary log file for writing, displaying on stderr... ");

/*
 * Print the postfix notation of the current regexp.
 */
    ساکن خالی
nfa_postfix_dump(char_u *اظہاریہ, صحیح retval)
{
    صحیح *p;
    FILE *f;

    f = fopen(NFA_REGEXP_DUMP_LOG, "a");
    اگر (f == تہی)
	واپس;

    fprintf(f, "\n-------------------------\n");
    اگر (retval == FAIL)
	fprintf(f, ">>> NFA engine failed... \n");
    ورنہ اگر (retval == OK)
	fprintf(f, ">>> NFA engine succeeded !\n");
    fprintf(f, "Regexp: \"%s\"\nPostfix notation (char): \"", اظہاریہ);
    برائے (p = post_آغاز; *p && p < post_ptr; p++)
    {
	nfa_سیٹ_رمز(*p);
	fprintf(f, "%s, ", رمز);
    }
    fprintf(f, "\"\nPostfix notation (int): ");
    برائے (p = post_آغاز; *p && p < post_ptr; p++)
	fprintf(f, "%d ", *p);
    fprintf(f, "\n\n");
    fclose(f);
}

/*
 * Print the NFA starting with a root node "state".
 */
    ساکن خالی
nfa_print_state(FILE *debugf, nfa_state_T *state)
{
    garray_T indent;

    ga_init2(&indent, 1, 64);
    ga_append(&indent, '\0');
    nfa_print_state2(debugf, state, &indent);
    ga_clear(&indent);
}

    ساکن خالی
nfa_print_state2(FILE *debugf, nfa_state_T *state, garray_T *indent)
{
    char_u  *p;

    اگر (state == تہی)
	واپس;

    fprintf(debugf, "(%2d)", مطلق_قدر(state->id));

    // Output indent
    p = (char_u *)indent->ga_ڈیٹا;
    اگر (indent->ga_len >= 3)
    {
	صحیح	آخری = indent->ga_len - 3;
	char_u	save[2];

	STRNCPY(save, &p[آخری], 2);
	حافظہ_نقل(&p[آخری], "+-", 2);
	fprintf(debugf, " %s", p);
	STRNCPY(&p[آخری], save, 2);
    }
    ورنہ
	fprintf(debugf, " %s", p);

    nfa_سیٹ_رمز(state->c);
    fprintf(debugf, "%s (%d) (id=%d) val=%d\n",
		 رمز,
		 state->c,
		 مطلق_قدر(state->id),
		 state->val);
    اگر (state->id < 0)
	واپس;

    state->id = مطلق_قدر(state->id) * -1;

    // grow indent for state->out
    indent->ga_len -= 1;
    اگر (state->out1)
	GA_CONCAT_LITERAL(indent, "| ");
    ورنہ
	GA_CONCAT_LITERAL(indent, "  ");
    ga_append(indent, NUL);

    nfa_print_state2(debugf, state->برآمد, indent);

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
    ساکن خالی
nfa_dump(nfa_regprog_T *prog)
{
    FILE *debugf = fopen(NFA_REGEXP_DUMP_LOG, "a");

    اگر (debugf == تہی)
	واپس;

    nfa_print_state(debugf, prog->آغاز);

    اگر (prog->reganch)
	fprintf(debugf, "reganch: %d\n", prog->reganch);
    اگر (prog->regstart != NUL)
	fprintf(debugf, "regstart: %c (decimal: %d)\n",
		prog->regstart, prog->regstart);
    اگر (prog->مطابقت_متن != تہی)
	fprintf(debugf, "match_text: \"%s\"\n", prog->مطابقت_متن);

    fclose(debugf);
}
# endif	    // ENABLE_LOG
#اگر_ختم	    // DEBUG

/*
 * Parse r.e. @expr and convert it into postfix form.
 * Return the postfix string on success, NULL otherwise.
 */
    ساکن صحیح *
re2post(خالی)
{
    اگر (nfa_reg(REG_NOPAREN) == FAIL)
	واپس تہی;
    EMIT(NFA_MOPEN);
    واپس post_آغاز;
}

// NB. Some of the code below is inspired by Russ's.

/*
 * Represents an NFA state plus zero or one or two arrows exiting.
 * if c == MATCH, no arrows out; matching state.
 * If c == SPLIT, unlabeled arrows to out and out1 (if != NULL).
 * If c < 256, labeled arrow with character c to out.
 */

ساکن nfa_state_T	*state_ptr; // points to nfa_prog->state

/*
 * Allocate and initialize nfa_state_T.
 */
    ساکن nfa_state_T *
alloc_state(صحیح c, nfa_state_T *برآمد, nfa_state_T *out1)
{
    nfa_state_T *s;

    اگر (istate >= nstate)
	واپس تہی;

    s = &state_ptr[istate++];

    s->c    = c;
    s->برآمد  = برآمد;
    s->out1 = out1;
    s->val  = 0;

    s->id   = istate;
    s->lastlist[0] = 0;
    s->lastlist[1] = 0;

    واپس s;
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
قسمتعریف اتحاد Ptrlist Ptrlist;
اتحاد Ptrlist
{
    Ptrlist	*اگلا;
    nfa_state_T	*s;
};

ساخت Frag
{
    nfa_state_T *آغاز;
    Ptrlist	*برآمد;
};
قسمتعریف ساخت Frag Frag_T;

// Reused across compilations by post2nfa(); freed in free_regexp_stuff().
ساکن Frag_T *nfa_ڈھیر;
ساکن صحیح nfa_ڈھیر_len;

/*
 * Initialize a Frag_T struct and return it.
 */
    ساکن Frag_T
frag(nfa_state_T *آغاز, Ptrlist *برآمد)
{
    Frag_T n;

    n.آغاز = آغاز;
    n.برآمد = برآمد;
    واپس n;
}

/*
 * Create singleton list containing just outp.
 */
    ساکن Ptrlist *
list1(
    nfa_state_T	**outp)
{
    Ptrlist *l;

    l = (Ptrlist *)outp;
    l->اگلا = تہی;
    واپس l;
}

/*
 * Patch the list of states at out to point to start.
 */
    ساکن خالی
patch(Ptrlist *l, nfa_state_T *s)
{
    Ptrlist *اگلا;

    برائے (; l; l = اگلا)
    {
	اگلا = l->اگلا;
	l->s = s;
    }
}


/*
 * Join the two lists l1 and l2, returning the combination.
 */
    ساکن Ptrlist *
append(Ptrlist *l1, Ptrlist *l2)
{
    Ptrlist *oldl1;

    oldl1 = l1;
    جب (l1->اگلا)
	l1 = l1->اگلا;
    l1->اگلا = l2;
    واپس oldl1;
}

/*
 * Stack used for transforming postfix form into NFA.
 */
ساکن Frag_T خالی_خانے_2;

    ساکن خالی
st_error(صحیح *postfix غیر_مستعمل, صحیح *انتہا غیر_مستعمل, صحیح *p غیر_مستعمل)
{
#اگر_تعریف_شدہ NFA_REGEXP_ERROR_LOG
    FILE *df;
    صحیح *p2;

    df = fopen(NFA_REGEXP_ERROR_LOG, "a");
    اگر (df)
    {
	fprintf(df, "Error popping the stack!\n");
# ifdef DEBUG
	fprintf(df, "Current regexp is \"%s\"\n", nfa_regengine.اظہاریہ);
# endif
	fprintf(df, "Postfix form is: ");
# ifdef DEBUG
	برائے (p2 = postfix; p2 < انتہا; p2++)
	{
	    nfa_سیٹ_رمز(*p2);
	    fprintf(df, "%s, ", رمز);
	}
	nfa_سیٹ_رمز(*p);
	fprintf(df, "\nCurrent position is: ");
	برائے (p2 = postfix; p2 <= p; p2 ++)
	{
	    nfa_سیٹ_رمز(*p2);
	    fprintf(df, "%s, ", رمز);
	}
# ورنہ
	برائے (p2 = postfix; p2 < انتہا; p2++)
	    fprintf(df, "%d, ", *p2);
	fprintf(df, "\nCurrent position is: ");
	برائے (p2 = postfix; p2 <= p; p2 ++)
	    fprintf(df, "%d, ", *p2);
# endif
	fprintf(df, "\n--------------------------\n");
	fclose(df);
    }
#اگر_ختم
    emsg(_(e_nfa_regexp_could_not_نکالو_ڈھیر));
}

/*
 * Push an item onto the stack.
 */
    ساکن خالی
st_ڈالو(Frag_T s, Frag_T **p, Frag_T *ڈھیر_انتہا)
{
    Frag_T *stackp = *p;

    اگر (stackp >= ڈھیر_انتہا)
	واپس;
    *stackp = s;
    *p = *p + 1;
}

/*
 * Pop an item from the stack.
 */
    ساکن Frag_T
st_نکالو(Frag_T **p, Frag_T *ڈھیر)
{
    Frag_T *stackp;

    *p = *p - 1;
    stackp = *p;
    اگر (stackp < ڈھیر)
	واپس خالی_خانے_2;
    واپس **p;
}

/*
 * Estimate the maximum byte length of anything matching "state".
 * When unknown or unlimited return -1.
 */
    ساکن صحیح
nfa_max_چوڑائی(nfa_state_T *startstate, صحیح گہرائی)
{
    صحیح		    l, r;
    nfa_state_T	    *state = startstate;
    صحیح		    len = 0;

    // detect looping in a NFA_SPLIT
    اگر (گہرائی > 4)
	واپس -1;

    جب (state != تہی)
    {
	انتخاب (state->c)
	{
	    حالت NFA_انتہا_INVISIBLE:
	    حالت NFA_انتہا_INVISIBLE_NEG:
		// the end, return what we have
		واپس len;

	    حالت NFA_بانٹو:
		// two alternatives, use the maximum
		l = nfa_max_چوڑائی(state->برآمد, گہرائی + 1);
		r = nfa_max_چوڑائی(state->out1, گہرائی + 1);
		اگر (l < 0 || r < 0)
		    واپس -1;
		واپس len + (l > r ? l : r);

	    حالت NFA_ANY:
	    حالت NFA_آغاز_COLL:
	    حالت NFA_آغاز_NEG_COLL:
		// matches some character, including composing chars
		اگر (enc_utf8)
		    len += MB_MAXBYTES;
		ورنہ اگر (has_mbyte)
		    len += 2;
		ورنہ
		    ++len;
		اگر (state->c != NFA_ANY)
		{
		    // Skip over the compiled collection.
		    // malformed NFAs must not crash width estimation.
		    اگر (state->out1 == تہی || state->out1->برآمد == تہی)
			واپس -1;
		    state = state->out1->برآمد;
		    جاری;
		}
		توڑو;

	    حالت NFA_DIGIT:
	    حالت NFA_WHITE:
	    حالت NFA_HEX:
	    حالت NFA_OCTAL:
		// ascii
		++len;
		توڑو;

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
	    حالت NFA_لفظ:
	    حالت NFA_NWORD:
	    حالت NFA_سر:
	    حالت NFA_NHEAD:
	    حالت NFA_ALPHA:
	    حالت NFA_NALPHA:
	    حالت NFA_زیریں:
	    حالت NFA_NLOWER:
	    حالت NFA_بالائی:
	    حالت NFA_NUPPER:
	    حالت NFA_زیریں_IC:
	    حالت NFA_NLOWER_IC:
	    حالت NFA_بالائی_IC:
	    حالت NFA_NUPPER_IC:
	    حالت NFA_ANY_COMPOSING:
		// possibly non-ascii
		اگر (has_mbyte)
		    len += 3;
		ورنہ
		    ++len;
		توڑو;

	    حالت NFA_آغاز_INVISIBLE:
	    حالت NFA_آغاز_INVISIBLE_NEG:
	    حالت NFA_آغاز_INVISIBLE_BEFORE:
	    حالت NFA_آغاز_INVISIBLE_BEFORE_NEG:
		// zero-width, out1 points to the END state
		state = state->out1->برآمد;
		جاری;

	    حالت NFA_BACKREF1:
	    حالت NFA_BACKREF2:
	    حالت NFA_BACKREF3:
	    حالت NFA_BACKREF4:
	    حالت NFA_BACKREF5:
	    حالت NFA_BACKREF6:
	    حالت NFA_BACKREF7:
	    حالت NFA_BACKREF8:
	    حالت NFA_BACKREF9:
#اگر_تعریف_شدہ FEAT_SYN_HL
	    حالت NFA_ZREF1:
	    حالت NFA_ZREF2:
	    حالت NFA_ZREF3:
	    حالت NFA_ZREF4:
	    حالت NFA_ZREF5:
	    حالت NFA_ZREF6:
	    حالت NFA_ZREF7:
	    حالت NFA_ZREF8:
	    حالت NFA_ZREF9:
#اگر_ختم
	    حالت NFA_NEWL:
	    حالت NFA_SKIP:
		// unknown width
		واپس -1;

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
#اگر_تعریف_شدہ FEAT_SYN_HL
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
#اگر_ختم
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
	    حالت NFA_ستون_GT:
	    حالت NFA_ستون_LT:
	    حالت NFA_VCOL_GT:
	    حالت NFA_VCOL_LT:
	    حالت NFA_MARK_GT:
	    حالت NFA_MARK_LT:
	    حالت NFA_VISUAL:
	    حالت NFA_LNUM:
	    حالت NFA_CURSOR:
	    حالت NFA_ستون:
	    حالت NFA_VCOL:
	    حالت NFA_MARK:

	    حالت NFA_ZSTART:
	    حالت NFA_ZEND:
	    حالت NFA_OPT_CHARS:
	    حالت NFA_خالی_خانے:
	    حالت NFA_آغاز_نمونہ:
	    حالت NFA_انتہا_نمونہ:
	    حالت NFA_COMPOSING:
	    حالت NFA_انتہا_COMPOSING:
		// zero-width
		توڑو;

	    طےشدہ:
		اگر (state->c < 0)
		    // don't know what this is
		    واپس -1;
		// normal character
		len += MB_CHAR2LEN(state->c);
		توڑو;
	}

	// normal way to continue
	state = state->برآمد;
    }

    // unrecognized, "cannot happen"
    واپس -1;
}

/*
 * Convert a postfix form into its equivalent NFA.
 * Return the NFA start state on success, NULL otherwise.
 */
    ساکن nfa_state_T *
post2nfa(صحیح *postfix, صحیح *انتہا, صحیح nfa_calc_سائز)
{
    صحیح		*p;
    صحیح		mopen;
    صحیح		mclose;
    Frag_T	*ڈھیر = تہی;
    Frag_T	*stackp = تہی;
    Frag_T	*ڈھیر_انتہا = تہی;
    Frag_T	e1;
    Frag_T	e2;
    Frag_T	e;
    nfa_state_T	*s;
    nfa_state_T	*s1;
    nfa_state_T	*matchstate;
    nfa_state_T	*ret = تہی;

    اگر (postfix == تہی)
	واپس تہی;

#تعریف_کرو ڈالو_2(s)	    st_ڈالو((s), &stackp, ڈھیر_انتہا)
#تعریف_کرو نکالو_2()	    st_نکالو(&stackp, ڈھیر);		\
		    اگر (stackp < ڈھیر)			\
		    {					\
			st_error(postfix, انتہا, p);	\
			واپس تہی;			\
		    }

    اگر (nfa_calc_سائز == FALSE)
    {
	// Reuse the fragment stack across compilations, growing when needed;
	// it is freed in free_regexp_stuff().
	اگر (nfa_ڈھیر == تہی || nfa_ڈھیر_len < nstate + 1)
	{
	    vim_free(nfa_ڈھیر);
	    nfa_ڈھیر = ALLOC_MULT(Frag_T, nstate + 1);
	    اگر (nfa_ڈھیر == تہی)
	    {
		nfa_ڈھیر_len = 0;
		واپس تہی;
	    }
	    nfa_ڈھیر_len = nstate + 1;
	}
	ڈھیر = nfa_ڈھیر;
	stackp = ڈھیر;
	ڈھیر_انتہا = ڈھیر + (nstate + 1);
    }

    برائے (p = postfix; p < انتہا; ++p)
    {
	انتخاب (*p)
	{
	حالت NFA_CONCAT:
	    // Concatenation.
	    // Pay attention: this operator does not exist in the r.e. itself
	    // (it is implicit, really).  It is added when r.e. is translated
	    // to postfix form in re2post().
	    اگر (nfa_calc_سائز == TRUE)
	    {
		// nstate += 0;
		توڑو;
	    }
	    e2 = نکالو_2();
	    e1 = نکالو_2();
	    patch(e1.برآمد, e2.آغاز);
	    ڈالو_2(frag(e1.آغاز, e2.برآمد));
	    توڑو;

	حالت NFA_OR:
	    // Alternation
	    اگر (nfa_calc_سائز == TRUE)
	    {
		nstate++;
		توڑو;
	    }
	    e2 = نکالو_2();
	    e1 = نکالو_2();
	    s = alloc_state(NFA_بانٹو, e1.آغاز, e2.آغاز);
	    اگر (s == تہی)
		جاؤ theend;
	    ڈالو_2(frag(s, append(e1.برآمد, e2.برآمد)));
	    توڑو;

	حالت NFA_ستارہ:
	    // Zero or more, prefer more
	    اگر (nfa_calc_سائز == TRUE)
	    {
		nstate++;
		توڑو;
	    }
	    e = نکالو_2();
	    s = alloc_state(NFA_بانٹو, e.آغاز, تہی);
	    اگر (s == تہی)
		جاؤ theend;
	    patch(e.برآمد, s);
	    ڈالو_2(frag(s, list1(&s->out1)));
	    توڑو;

	حالت NFA_ستارہ_NONGREEDY:
	    // Zero or more, prefer zero
	    اگر (nfa_calc_سائز == TRUE)
	    {
		nstate++;
		توڑو;
	    }
	    e = نکالو_2();
	    s = alloc_state(NFA_بانٹو, تہی, e.آغاز);
	    اگر (s == تہی)
		جاؤ theend;
	    patch(e.برآمد, s);
	    ڈالو_2(frag(s, list1(&s->برآمد)));
	    توڑو;

	حالت NFA_QUEST:
	    // one or zero atoms=> greedy match
	    اگر (nfa_calc_سائز == TRUE)
	    {
		nstate++;
		توڑو;
	    }
	    e = نکالو_2();
	    s = alloc_state(NFA_بانٹو, e.آغاز, تہی);
	    اگر (s == تہی)
		جاؤ theend;
	    ڈالو_2(frag(s, append(e.برآمد, list1(&s->out1))));
	    توڑو;

	حالت NFA_QUEST_NONGREEDY:
	    // zero or one atoms => non-greedy match
	    اگر (nfa_calc_سائز == TRUE)
	    {
		nstate++;
		توڑو;
	    }
	    e = نکالو_2();
	    s = alloc_state(NFA_بانٹو, تہی, e.آغاز);
	    اگر (s == تہی)
		جاؤ theend;
	    ڈالو_2(frag(s, append(e.برآمد, list1(&s->برآمد))));
	    توڑو;

	حالت NFA_انتہا_COLL:
	حالت NFA_انتہا_NEG_COLL:
	    // On the stack is the sequence starting with NFA_START_COLL or
	    // NFA_START_NEG_COLL and all possible characters. Patch it to
	    // add the output to the start.
	    اگر (nfa_calc_سائز == TRUE)
	    {
		nstate++;
		توڑو;
	    }
	    e = نکالو_2();
	    s = alloc_state(NFA_انتہا_COLL, تہی, تہی);
	    اگر (s == تہی)
		جاؤ theend;
	    patch(e.برآمد, s);
	    e.آغاز->out1 = s;
	    ڈالو_2(frag(e.آغاز, list1(&s->برآمد)));
	    توڑو;

	حالت NFA_RANGE:
	    // Before this are two characters, the low and high end of a
	    // range.  Turn them into two states with MIN and MAX.
	    اگر (nfa_calc_سائز == TRUE)
	    {
		// nstate += 0;
		توڑو;
	    }
	    e2 = نکالو_2();
	    e1 = نکالو_2();
	    e2.آغاز->val = e2.آغاز->c;
	    e2.آغاز->c = NFA_RANGE_MAX;
	    e1.آغاز->val = e1.آغاز->c;
	    e1.آغاز->c = NFA_RANGE_MIN;
	    patch(e1.برآمد, e2.آغاز);
	    ڈالو_2(frag(e1.آغاز, e2.برآمد));
	    توڑو;

	حالت NFA_خالی_خانے:
	    // 0-length, used in a repetition with max/min count of 0
	    اگر (nfa_calc_سائز == TRUE)
	    {
		nstate++;
		توڑو;
	    }
	    s = alloc_state(NFA_خالی_خانے, تہی, تہی);
	    اگر (s == تہی)
		جاؤ theend;
	    ڈالو_2(frag(s, list1(&s->برآمد)));
	    توڑو;

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
	    اگر (nfa_calc_سائز == TRUE)
	    {
		nstate += n;
		توڑو;
	    }
	    s = تہی; // avoid compiler warning
	    e1.برآمد = تہی; // stores list with out1's
	    s1 = تہی; // previous NFA_SPLIT to connect to
	    جب (n-- > 0)
	    {
		e = نکالو_2(); // get character
		s = alloc_state(NFA_بانٹو, e.آغاز, تہی);
		اگر (s == تہی)
		    جاؤ theend;
		اگر (e1.برآمد == تہی)
		    e1 = e;
		patch(e.برآمد, s1);
		append(e1.برآمد, list1(&s->out1));
		s1 = s;
	    }
	    ڈالو_2(frag(s, e1.برآمد));
	    توڑو;
	  }

	حالت NFA_پچھلا_ATOM_NO_چوڑائی:
	حالت NFA_پچھلا_ATOM_NO_چوڑائی_NEG:
	حالت NFA_پچھلا_ATOM_JUST_BEFORE:
	حالت NFA_پچھلا_ATOM_JUST_BEFORE_NEG:
	حالت NFA_پچھلا_ATOM_LIKE_نمونہ:
	  {
	    صحیح before = (*p == NFA_پچھلا_ATOM_JUST_BEFORE
				      || *p == NFA_پچھلا_ATOM_JUST_BEFORE_NEG);
	    صحیح نمونہ = (*p == NFA_پچھلا_ATOM_LIKE_نمونہ);
	    صحیح آغاز_state;
	    صحیح انتہا_state;
	    صحیح n = 0;
	    nfa_state_T *zend;
	    nfa_state_T *skip;

	    انتخاب (*p)
	    {
		حالت NFA_پچھلا_ATOM_NO_چوڑائی:
		    آغاز_state = NFA_آغاز_INVISIBLE;
		    انتہا_state = NFA_انتہا_INVISIBLE;
		    توڑو;
		حالت NFA_پچھلا_ATOM_NO_چوڑائی_NEG:
		    آغاز_state = NFA_آغاز_INVISIBLE_NEG;
		    انتہا_state = NFA_انتہا_INVISIBLE_NEG;
		    توڑو;
		حالت NFA_پچھلا_ATOM_JUST_BEFORE:
		    آغاز_state = NFA_آغاز_INVISIBLE_BEFORE;
		    انتہا_state = NFA_انتہا_INVISIBLE;
		    توڑو;
		حالت NFA_پچھلا_ATOM_JUST_BEFORE_NEG:
		    آغاز_state = NFA_آغاز_INVISIBLE_BEFORE_NEG;
		    انتہا_state = NFA_انتہا_INVISIBLE_NEG;
		    توڑو;
		طےشدہ: // NFA_PREV_ATOM_LIKE_PATTERN:
		    آغاز_state = NFA_آغاز_نمونہ;
		    انتہا_state = NFA_انتہا_نمونہ;
		    توڑو;
	    }

	    اگر (before)
		n = *++p; // get the count

	    // The \@= operator: match the preceding atom with zero width.
	    // The \@! operator: no match for the preceding atom.
	    // The \@<= operator: match for the preceding atom.
	    // The \@<! operator: no match for the preceding atom.
	    // Surrounds the preceding atom with START_INVISIBLE and
	    // END_INVISIBLE, similarly to MOPEN.

	    اگر (nfa_calc_سائز == TRUE)
	    {
		nstate += نمونہ ? 4 : 2;
		توڑو;
	    }
	    e = نکالو_2();
	    s1 = alloc_state(انتہا_state, تہی, تہی);
	    اگر (s1 == تہی)
		جاؤ theend;

	    s = alloc_state(آغاز_state, e.آغاز, s1);
	    اگر (s == تہی)
		جاؤ theend;
	    اگر (نمونہ)
	    {
		// NFA_ZEND -> NFA_END_PATTERN -> NFA_SKIP -> what follows.
		skip = alloc_state(NFA_SKIP, تہی, تہی);
		اگر (skip == تہی)
		    جاؤ theend;
		zend = alloc_state(NFA_ZEND, s1, تہی);
		اگر (zend == تہی)
		    جاؤ theend;
		s1->برآمد= skip;
		patch(e.برآمد, zend);
		ڈالو_2(frag(s, list1(&skip->برآمد)));
	    }
	    ورنہ
	    {
		patch(e.برآمد, s1);
		ڈالو_2(frag(s, list1(&s1->برآمد)));
		اگر (before)
		{
		    اگر (n <= 0)
			// See if we can guess the maximum width, it avoids a
			// lot of pointless tries.
			n = nfa_max_چوڑائی(e.آغاز, 0);
		    s->val = n; // store the count
		}
	    }
	    توڑو;
	  }

	حالت NFA_COMPOSING:	// char with composing char
#اگر 0
	    // TODO
	    اگر (regflags & RF_ICOMBINE)
	    {
		// use the base character only
	    }
#اگر_ختم
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
#اگر_تعریف_شدہ FEAT_SYN_HL
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
#اگر_ختم
	حالت NFA_NOPEN:	// \%( \) "Invisible Submatch"
	    اگر (nfa_calc_سائز == TRUE)
	    {
		nstate += 2;
		توڑو;
	    }

	    mopen = *p;
	    انتخاب (*p)
	    {
		حالت NFA_NOPEN: mclose = NFA_NCLOSE; توڑو;
#اگر_تعریف_شدہ FEAT_SYN_HL
		حالت NFA_ZOPEN: mclose = NFA_ZCLOSE; توڑو;
		حالت NFA_ZOPEN1: mclose = NFA_ZCLOSE1; توڑو;
		حالت NFA_ZOPEN2: mclose = NFA_ZCLOSE2; توڑو;
		حالت NFA_ZOPEN3: mclose = NFA_ZCLOSE3; توڑو;
		حالت NFA_ZOPEN4: mclose = NFA_ZCLOSE4; توڑو;
		حالت NFA_ZOPEN5: mclose = NFA_ZCLOSE5; توڑو;
		حالت NFA_ZOPEN6: mclose = NFA_ZCLOSE6; توڑو;
		حالت NFA_ZOPEN7: mclose = NFA_ZCLOSE7; توڑو;
		حالت NFA_ZOPEN8: mclose = NFA_ZCLOSE8; توڑو;
		حالت NFA_ZOPEN9: mclose = NFA_ZCLOSE9; توڑو;
#اگر_ختم
		حالت NFA_COMPOSING: mclose = NFA_انتہا_COMPOSING; توڑو;
		طےشدہ:
		    // NFA_MOPEN, NFA_MOPEN1 .. NFA_MOPEN9
		    mclose = *p + NSUBEXP;
		    توڑو;
	    }

	    // Allow "NFA_MOPEN" as a valid postfix representation for
	    // the empty regexp "". In this case, the NFA will be
	    // NFA_MOPEN -> NFA_MCLOSE. Note that this also allows
	    // empty groups of parenthesis, and empty mbyte chars
	    اگر (stackp == ڈھیر)
	    {
		s = alloc_state(mopen, تہی, تہی);
		اگر (s == تہی)
		    جاؤ theend;
		s1 = alloc_state(mclose, تہی, تہی);
		اگر (s1 == تہی)
		    جاؤ theend;
		patch(list1(&s->برآمد), s1);
		ڈالو_2(frag(s, list1(&s1->برآمد)));
		توڑو;
	    }

	    // At least one node was emitted before NFA_MOPEN, so
	    // at least one node will be between NFA_MOPEN and NFA_MCLOSE
	    e = نکالو_2();
	    s = alloc_state(mopen, e.آغاز, تہی);   // `('
	    اگر (s == تہی)
		جاؤ theend;

	    s1 = alloc_state(mclose, تہی, تہی);   // `)'
	    اگر (s1 == تہی)
		جاؤ theend;
	    patch(e.برآمد, s1);

	    اگر (mopen == NFA_COMPOSING)
		// COMPOSING->out1 = END_COMPOSING
		patch(list1(&s->out1), s1);

	    ڈالو_2(frag(s, list1(&s1->برآمد)));
	    توڑو;

	حالت NFA_BACKREF1:
	حالت NFA_BACKREF2:
	حالت NFA_BACKREF3:
	حالت NFA_BACKREF4:
	حالت NFA_BACKREF5:
	حالت NFA_BACKREF6:
	حالت NFA_BACKREF7:
	حالت NFA_BACKREF8:
	حالت NFA_BACKREF9:
#اگر_تعریف_شدہ FEAT_SYN_HL
	حالت NFA_ZREF1:
	حالت NFA_ZREF2:
	حالت NFA_ZREF3:
	حالت NFA_ZREF4:
	حالت NFA_ZREF5:
	حالت NFA_ZREF6:
	حالت NFA_ZREF7:
	حالت NFA_ZREF8:
	حالت NFA_ZREF9:
#اگر_ختم
	    اگر (nfa_calc_سائز == TRUE)
	    {
		nstate += 2;
		توڑو;
	    }
	    s = alloc_state(*p, تہی, تہی);
	    اگر (s == تہی)
		جاؤ theend;
	    s1 = alloc_state(NFA_SKIP, تہی, تہی);
	    اگر (s1 == تہی)
		جاؤ theend;
	    patch(list1(&s->برآمد), s1);
	    ڈالو_2(frag(s, list1(&s1->برآمد)));
	    توڑو;

	حالت NFA_LNUM:
	حالت NFA_LNUM_GT:
	حالت NFA_LNUM_LT:
	حالت NFA_VCOL:
	حالت NFA_VCOL_GT:
	حالت NFA_VCOL_LT:
	حالت NFA_ستون:
	حالت NFA_ستون_GT:
	حالت NFA_ستون_LT:
	حالت NFA_MARK:
	حالت NFA_MARK_GT:
	حالت NFA_MARK_LT:
	  {
	    صحیح n = *++p; // lnum, col or mark name

	    اگر (nfa_calc_سائز == TRUE)
	    {
		nstate += 1;
		توڑو;
	    }
	    s = alloc_state(p[-1], تہی, تہی);
	    اگر (s == تہی)
		جاؤ theend;
	    s->val = n;
	    ڈالو_2(frag(s, list1(&s->برآمد)));
	    توڑو;
	  }

	حالت NFA_ZSTART:
	حالت NFA_ZEND:
	طےشدہ:
	    // Operands
	    اگر (nfa_calc_سائز == TRUE)
	    {
		nstate++;
		توڑو;
	    }
	    s = alloc_state(*p, تہی, تہی);
	    اگر (s == تہی)
		جاؤ theend;
	    ڈالو_2(frag(s, list1(&s->برآمد)));
	    توڑو;

	} // switch(*p)

    } // for (p = postfix; *p; ++p)

    اگر (nfa_calc_سائز == TRUE)
    {
	nstate++;
	جاؤ theend;	// Return value when counting size is ignored anyway
    }

    e = نکالو_2();
    اگر (stackp != ڈھیر)
	EMSG_RET_NULL(_(e_nfa_regexp_while_converting_from_postfix_مقصد_nfa_too_many_stats_بایاں_on_ڈھیر));

    اگر (istate >= nstate)
	EMSG_RET_NULL(_(e_nfa_regexp_not_enough_space_مقصد_store_پورا_nfa));

    matchstate = &state_ptr[istate++]; // the match state
    matchstate->c = NFA_مطابقت;
    matchstate->برآمد = matchstate->out1 = تہی;
    matchstate->id = 0;

    patch(e.برآمد, matchstate);
    ret = e.آغاز;

theend:
    واپس ret;

#تعریف_منسوخ POP1
#تعریف_منسوخ PUSH1
#تعریف_منسوخ POP2
#تعریف_منسوخ PUSH2
#تعریف_منسوخ نکالو_2
#تعریف_منسوخ ڈالو_2
}

/*
 * After building the NFA program, inspect it to add optimization hints.
 */
    ساکن خالی
nfa_postprocess(nfa_regprog_T *prog)
{
    صحیح i;
    صحیح c;

    برائے (i = 0; i < prog->nstate; ++i)
    {
	c = prog->state[i].c;
	اگر (c == NFA_آغاز_INVISIBLE
		|| c == NFA_آغاز_INVISIBLE_NEG
		|| c == NFA_آغاز_INVISIBLE_BEFORE
		|| c == NFA_آغاز_INVISIBLE_BEFORE_NEG)
	{
	    صحیح directly;

	    // Do it directly when what follows is possibly the end of the
	    // match.
	    اگر (مطابقت_follows(prog->state[i].out1->برآمد, 0))
		directly = TRUE;
	    ورنہ
	    {
		صحیح ch_invisible = failure_chance(prog->state[i].برآمد, 0);
		صحیح ch_follows = failure_chance(prog->state[i].out1->برآمد, 0);

		// Postpone when the invisible match is expensive or has a
		// lower chance of failing.
		اگر (c == NFA_آغاز_INVISIBLE_BEFORE
		     || c == NFA_آغاز_INVISIBLE_BEFORE_NEG)
		{
		    // "before" matches are very expensive when
		    // unbounded, always prefer what follows then,
		    // unless what follows will always match.
		    // Otherwise strongly prefer what follows.
		    اگر (prog->state[i].val <= 0 && ch_follows > 0)
			directly = FALSE;
		    ورنہ
			directly = ch_follows * 10 < ch_invisible;
		}
		ورنہ
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

قسمتعریف ساخت
{
    صحیح	    اندر_use; // number of subexpr with useful info

    // When REG_MULTI is TRUE list.multi is used, otherwise list.line.
    اتحاد
    {
	ساخت multipos
	{
	    linenr_T	آغاز_lnum;
	    linenr_T	انتہا_lnum;
	    colnr_T	آغاز_ستون;
	    colnr_T	انتہا_ستون;
	} multi[NSUBEXP];
	ساخت linepos
	{
	    char_u	*آغاز;
	    char_u	*انتہا;
	} سطر[NSUBEXP];
    } list;
    colnr_T	orig_آغاز_ستون;  // list.multi[0].start_col without \zs
} regsub_T;

قسمتعریف ساخت
{
    regsub_T	norm; // \( .. \) matches
#اگر_تعریف_شدہ FEAT_SYN_HL
    regsub_T	synt; // \z( .. \) matches
#اگر_ختم
} regsubs_T;

// nfa_pim_T stores a Postponed Invisible Match.
قسمتعریف ساخت nfa_pim_S nfa_pim_T;
ساخت nfa_pim_S
{
    صحیح		نتیجہ;		// NFA_PIM_*, see below
    nfa_state_T	*state;		// the invisible match start state
    regsubs_T	subs;		// submatch info, only party used
    اتحاد
    {
	lpos_T	pos;
	char_u	*ptr;
    } انتہا;			// where the match must end
};

// Values for done in nfa_pim_T.
#تعریف_کرو NFA_PIM_غیر_مستعمل   0	// pim not used
#تعریف_کرو NFA_PIM_TODO     1	// pim not done yet
#تعریف_کرو NFA_PIM_مطابقت    2	// pim executed, matches
#تعریف_کرو NFA_PIM_NOMATCH  3	// pim executed, no match


// nfa_thread_T contains execution information of a NFA state
قسمتعریف ساخت
{
    nfa_state_T	*state;
    صحیح		گنتی;
    nfa_pim_T	pim;		// if pim.result != NFA_PIM_UNUSED: postponed
				// invisible match
    regsubs_T	subs;		// submatch info, only party used
} nfa_thread_T;

// nfa_list_T contains the alternative NFA execution states.
قسمتعریف ساخت
{
    nfa_thread_T    *t;		// allocated array of states
    صحیح		    n;		// nr of states currently in "t"
    صحیح		    len;	// max nr of states in "t"
    صحیح		    id;		// ID of the list
    صحیح		    has_pim;	// TRUE when any state has a PIM
} nfa_list_T;

#اگر_تعریف_شدہ ENABLE_LOG
ساکن خالی log_subexpr(regsub_T *sub);

    ساکن خالی
log_subsexpr(regsubs_T *subs)
{
    log_subexpr(&subs->norm);
# ifdef FEAT_SYN_HL
    اگر (rex.nfa_has_zsubexpr)
	log_subexpr(&subs->synt);
# endif
}

    ساکن خالی
log_subexpr(regsub_T *sub)
{
    صحیح j;

    برائے (j = 0; j < sub->اندر_use; j++)
	اگر (REG_MULTI)
	    fprintf(log_fd,
		    "*** group %d, start: c=%d, l=%d, end: c=%d, l=%d\n",
		    j,
		    sub->list.multi[j].آغاز_ستون,
		    (صحیح)sub->list.multi[j].آغاز_lnum,
		    sub->list.multi[j].انتہا_ستون,
		    (صحیح)sub->list.multi[j].انتہا_lnum);
	ورنہ
	{
	    حرف *s = (حرف *)sub->list.سطر[j].آغاز;
	    حرف *e = (حرف *)sub->list.سطر[j].انتہا;

	    fprintf(log_fd, "*** group %d, start: \"%s\", end: \"%s\"\n",
		    j,
		    s == تہی ? "NULL" : s,
		    e == تہی ? "NULL" : e);
	}
}

    ساکن حرف *
pim_info(nfa_pim_T *pim)
{
    ساکن حرف buf[30];

    اگر (pim == تہی || pim->نتیجہ == NFA_PIM_غیر_مستعمل)
	buf[0] = NUL;
    ورنہ
    {
	sprintf(buf, " PIM col %d", REG_MULTI ? (صحیح)pim->انتہا.pos.ستون
		: (صحیح)(pim->انتہا.ptr - rex.داخلہ));
    }
    واپس buf;
}

#اگر_ختم

// Used during execution: whether a match has been found.
ساکن صحیح	    nfa_مطابقت;
#اگر_تعریف_شدہ FEAT_RELTIME
ساکن صحیح	   *nfa_timed_برآمد;
#اگر_ختم

ساکن اندرونی خالی copy_sub(regsub_T *مقصد, regsub_T *from);
ساکن صحیح pim_equal(nfa_pim_T *one, nfa_pim_T *two);

/*
 * Copy postponed invisible match info from "from" to "to".
 */
    ساکن خالی
copy_pim(nfa_pim_T *مقصد, nfa_pim_T *from)
{
    مقصد->نتیجہ = from->نتیجہ;
    مقصد->state = from->state;
    copy_sub(&مقصد->subs.norm, &from->subs.norm);
#اگر_تعریف_شدہ FEAT_SYN_HL
    اگر (rex.nfa_has_zsubexpr)
	copy_sub(&مقصد->subs.synt, &from->subs.synt);
#اگر_ختم
    مقصد->انتہا = from->انتہا;
}

    ساکن خالی
clear_sub(regsub_T *sub)
{
    اگر (REG_MULTI)
	// Use 0xff to set lnum to -1
	vim_memset(sub->list.multi, 0xff,
				  حجم(ساخت multipos) * rex.nfa_nsubexpr);
    ورنہ
	vim_memset(sub->list.سطر, 0,
				   حجم(ساخت linepos) * rex.nfa_nsubexpr);
    sub->اندر_use = 0;
}

/*
 * Copy the submatches from "from" to "to".
 */
    ساکن اندرونی خالی
copy_sub(regsub_T *مقصد, regsub_T *from)
{
    مقصد->اندر_use = from->اندر_use;
    اگر (from->اندر_use <= 0)
	واپس;

    // Copy the match start and end positions.  addstate() calls this for
    // every state it adds, so it is very hot.  Patterns without capturing
    // groups only use subexpression zero (the whole match), so special-case
    // "in_use == 1" with a direct assignment to avoid the mch_memmove() call.
    اگر (REG_MULTI)
    {
	اگر (from->اندر_use == 1)
	    مقصد->list.multi[0] = from->list.multi[0];
	ورنہ
	    mch_memmove(&مقصد->list.multi[0],
		    &from->list.multi[0],
		    حجم(ساخت multipos) * from->اندر_use);
	مقصد->orig_آغاز_ستون = from->orig_آغاز_ستون;
    }
    ورنہ
    {
	اگر (from->اندر_use == 1)
	    مقصد->list.سطر[0] = from->list.سطر[0];
	ورنہ
	    mch_memmove(&مقصد->list.سطر[0],
		    &from->list.سطر[0],
		    حجم(ساخت linepos) * from->اندر_use);
    }
}

/*
 * Like copy_sub() but exclude the main match.
 */
    ساکن خالی
copy_sub_off(regsub_T *مقصد, regsub_T *from)
{
    اگر (مقصد->اندر_use < from->اندر_use)
	مقصد->اندر_use = from->اندر_use;
    اگر (from->اندر_use <= 1)
	واپس;

    // Copy the match start and end positions.
    اگر (REG_MULTI)
	mch_memmove(&مقصد->list.multi[1],
		&from->list.multi[1],
		حجم(ساخت multipos) * (from->اندر_use - 1));
    ورنہ
	mch_memmove(&مقصد->list.سطر[1],
		&from->list.سطر[1],
		حجم(ساخت linepos) * (from->اندر_use - 1));
}

/*
 * Like copy_sub() but only do the end of the main match if \ze is present.
 */
    ساکن خالی
copy_ze_off(regsub_T *مقصد, regsub_T *from)
{
    اگر (!rex.nfa_has_zend)
	واپس;

    اگر (REG_MULTI)
    {
	اگر (from->list.multi[0].انتہا_lnum >= 0)
	{
	    مقصد->list.multi[0].انتہا_lnum = from->list.multi[0].انتہا_lnum;
	    مقصد->list.multi[0].انتہا_ستون = from->list.multi[0].انتہا_ستون;
	}
    }
    ورنہ
    {
	اگر (from->list.سطر[0].انتہا != تہی)
	    مقصد->list.سطر[0].انتہا = from->list.سطر[0].انتہا;
    }
}

/*
 * Return TRUE if "sub1" and "sub2" have the same start positions.
 * When using back-references also check the end position.
 */
    ساکن صحیح
sub_equal(regsub_T *sub1, regsub_T *sub2)
{
    صحیح		i;
    صحیح		todo;
    linenr_T	s1;
    linenr_T	s2;
    char_u	*sp1;
    char_u	*sp2;

    todo = sub1->اندر_use > sub2->اندر_use ? sub1->اندر_use : sub2->اندر_use;
    اگر (REG_MULTI)
    {
	برائے (i = 0; i < todo; ++i)
	{
	    اگر (i < sub1->اندر_use)
		s1 = sub1->list.multi[i].آغاز_lnum;
	    ورنہ
		s1 = -1;
	    اگر (i < sub2->اندر_use)
		s2 = sub2->list.multi[i].آغاز_lnum;
	    ورنہ
		s2 = -1;
	    اگر (s1 != s2)
		واپس FALSE;
	    اگر (s1 != -1 && sub1->list.multi[i].آغاز_ستون
					     != sub2->list.multi[i].آغاز_ستون)
		واپس FALSE;

	    اگر (rex.nfa_has_backref)
	    {
		اگر (i < sub1->اندر_use)
		    s1 = sub1->list.multi[i].انتہا_lnum;
		ورنہ
		    s1 = -1;
		اگر (i < sub2->اندر_use)
		    s2 = sub2->list.multi[i].انتہا_lnum;
		ورنہ
		    s2 = -1;
		اگر (s1 != s2)
		    واپس FALSE;
		اگر (s1 != -1 && sub1->list.multi[i].انتہا_ستون
					       != sub2->list.multi[i].انتہا_ستون)
		واپس FALSE;
	    }
	}
    }
    ورنہ
    {
	برائے (i = 0; i < todo; ++i)
	{
	    اگر (i < sub1->اندر_use)
		sp1 = sub1->list.سطر[i].آغاز;
	    ورنہ
		sp1 = تہی;
	    اگر (i < sub2->اندر_use)
		sp2 = sub2->list.سطر[i].آغاز;
	    ورنہ
		sp2 = تہی;
	    اگر (sp1 != sp2)
		واپس FALSE;
	    اگر (rex.nfa_has_backref)
	    {
		اگر (i < sub1->اندر_use)
		    sp1 = sub1->list.سطر[i].انتہا;
		ورنہ
		    sp1 = تہی;
		اگر (i < sub2->اندر_use)
		    sp2 = sub2->list.سطر[i].انتہا;
		ورنہ
		    sp2 = تہی;
		اگر (sp1 != sp2)
		    واپس FALSE;
	    }
	}
    }

    واپس TRUE;
}

#اگر_تعریف_شدہ FEAT_RELTIME
/*
 * Check if we are past the time limit, if there is one.
 */
    ساکن صحیح
nfa_did_وقت_برآمد(خالی)
{
    اگر (*timeout_flag)
    {
	اگر (nfa_timed_برآمد != تہی)
	{
# ifdef FEAT_EVAL
	    اگر (!*nfa_timed_برآمد)
		ch_log(تہی, "NFA regexp timed out");
# endif
	    *nfa_timed_برآمد = TRUE;
	}
	واپس TRUE;
    }
    واپس FALSE;
}
#اگر_ختم

#اگر_تعریف_شدہ ENABLE_LOG
    ساکن خالی
open_debug_log(صحیح نتیجہ)
{
    log_fd = fopen(NFA_REGEXP_تسلسل_LOG, "a");
    اگر (log_fd == تہی)
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

    ساکن خالی
report_state(حرف *action,
	     regsub_T *sub,
	     nfa_state_T *state,
	     صحیح lid,
	     nfa_pim_T *pim)
{
    صحیح ستون;

    اگر (sub->اندر_use <= 0)
	ستون = -1;
    ورنہ اگر (REG_MULTI)
	ستون = sub->list.multi[0].آغاز_ستون;
    ورنہ
	ستون = (صحیح)(sub->list.سطر[0].آغاز - rex.سطر);
    nfa_سیٹ_رمز(state->c);
    اگر (log_fd == تہی)
	open_debug_log(MAYBE);

    fprintf(log_fd, "> %s state %d to list %d. char %d: %s (start col %d)%s\n",
	    action, مطلق_قدر(state->id), lid, state->c, رمز, ستون,
	    pim_info(pim));
}
#اگر_ختم

/*
 * Return TRUE if the same state is already in list "l" with the same
 * positions as "subs".
 */
    ساکن صحیح
has_state_with_pos(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim)	// postponed match or NULL
{
    nfa_thread_T	*thread;
    صحیح			i;

    برائے (i = 0; i < l->n; ++i)
    {
	thread = &l->t[i];
	اگر (thread->state->id == state->id
		&& sub_equal(&thread->subs.norm, &subs->norm)
#اگر_تعریف_شدہ FEAT_SYN_HL
		&& (!rex.nfa_has_zsubexpr
				|| sub_equal(&thread->subs.synt, &subs->synt))
#اگر_ختم
		&& pim_equal(&thread->pim, pim))
	    واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Return TRUE if "one" and "two" are equal.  That includes when both are not
 * set.
 */
    ساکن صحیح
pim_equal(nfa_pim_T *one, nfa_pim_T *two)
{
    صحیح one_غیر_مستعمل = (one == تہی || one->نتیجہ == NFA_PIM_غیر_مستعمل);
    صحیح two_غیر_مستعمل = (two == تہی || two->نتیجہ == NFA_PIM_غیر_مستعمل);

    اگر (one_غیر_مستعمل)
	// one is unused: equal when two is also unused
	واپس two_غیر_مستعمل;
    اگر (two_غیر_مستعمل)
	// one is used and two is not: not equal
	واپس FALSE;
    // compare the state id
    اگر (one->state->id != two->state->id)
	واپس FALSE;
    // compare the position
    اگر (REG_MULTI)
	واپس one->انتہا.pos.lnum == two->انتہا.pos.lnum
	    && one->انتہا.pos.ستون == two->انتہا.pos.ستون;
    واپس one->انتہا.ptr == two->انتہا.ptr;
}

/*
 * Return TRUE if "state" leads to a NFA_MATCH without advancing the input.
 */
    ساکن صحیح
مطابقت_follows(nfa_state_T *startstate, صحیح گہرائی)
{
    nfa_state_T	    *state = startstate;

    // avoid too much recursion
    اگر (گہرائی > 10)
	واپس FALSE;

    جب (state != تہی)
    {
	انتخاب (state->c)
	{
	    حالت NFA_مطابقت:
	    حالت NFA_MCLOSE:
	    حالت NFA_انتہا_INVISIBLE:
	    حالت NFA_انتہا_INVISIBLE_NEG:
	    حالت NFA_انتہا_نمونہ:
		واپس TRUE;

	    حالت NFA_بانٹو:
		واپس مطابقت_follows(state->برآمد, گہرائی + 1)
				     || مطابقت_follows(state->out1, گہرائی + 1);

	    حالت NFA_آغاز_INVISIBLE:
	    حالت NFA_آغاز_INVISIBLE_پہلا:
	    حالت NFA_آغاز_INVISIBLE_BEFORE:
	    حالت NFA_آغاز_INVISIBLE_BEFORE_پہلا:
	    حالت NFA_آغاز_INVISIBLE_NEG:
	    حالت NFA_آغاز_INVISIBLE_NEG_پہلا:
	    حالت NFA_آغاز_INVISIBLE_BEFORE_NEG:
	    حالت NFA_آغاز_INVISIBLE_BEFORE_NEG_پہلا:
	    حالت NFA_COMPOSING:
		// skip ahead to next state
		state = state->out1->برآمد;
		جاری;

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
	    حالت NFA_لفظ:
	    حالت NFA_NWORD:
	    حالت NFA_سر:
	    حالت NFA_NHEAD:
	    حالت NFA_ALPHA:
	    حالت NFA_NALPHA:
	    حالت NFA_زیریں:
	    حالت NFA_NLOWER:
	    حالت NFA_بالائی:
	    حالت NFA_NUPPER:
	    حالت NFA_زیریں_IC:
	    حالت NFA_NLOWER_IC:
	    حالت NFA_بالائی_IC:
	    حالت NFA_NUPPER_IC:
	    حالت NFA_آغاز_COLL:
	    حالت NFA_آغاز_NEG_COLL:
	    حالت NFA_NEWL:
		// state will advance input
		واپس FALSE;

	    طےشدہ:
		اگر (state->c > 0)
		    // state will advance input
		    واپس FALSE;

		// Others: zero-width or possibly zero-width, might still find
		// a match at the same position, keep looking.
		توڑو;
	}
	state = state->برآمد;
    }
    واپس FALSE;
}


/*
 * Return TRUE if "state" is already in list "l".
 */
    ساکن صحیح
state_اندر_list(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs)	// pointers to subexpressions
{
    اگر (state->lastlist[nfa_ll_اشاریہ] == l->id)
    {
	اگر (!rex.nfa_has_backref || has_state_with_pos(l, state, subs, تہی))
	    واپس TRUE;
    }
    واپس FALSE;
}

// Offset used for "off" by addstate_here().
#تعریف_کرو ADDSTATE_یہاں_انحراف 10

/*
 * Add "state" and possibly what follows to state list ".".
 * Returns "subs_arg", possibly copied into temp_subs.
 * Returns NULL when recursiveness is too deep or timed out.
 */
    ساکن regsubs_T *
addstate(
    nfa_list_T		*l,	    // runtime state list
    nfa_state_T		*state,	    // state to update
    regsubs_T		*subs_arg,  // pointers to subexpressions
    nfa_pim_T		*pim,	    // postponed look-behind match
    صحیح			off_arg,    // byte offset, when -1 go to next line
    صحیح			گہرائی)	    // recursion depth
{
    صحیح			subidx;
    صحیح			off = off_arg;
    صحیح			جوڑو_یہاں = FALSE;
    صحیح			listindex = 0;
    صحیح			k;
    صحیح			ملا = FALSE;
    nfa_thread_T	*thread;
    ساخت multipos	save_multipos;
    صحیح			save_اندر_use;
    char_u		*save_ptr;
    صحیح			i;
    regsub_T		*sub;
    regsubs_T		*subs = subs_arg;
    ساکن regsubs_T	عارضی_subs;
#اگر_تعریف_شدہ ENABLE_LOG
    صحیح			did_print = FALSE;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_RELTIME
    اگر (nfa_did_وقت_برآمد())
	واپس تہی;
#اگر_ختم

    // This function is called recursively.  When the depth is too much we run
    // out of stack and crash, limit recursiveness here.
    اگر (گہرائی >= 5000 || subs == تہی)
	واپس تہی;

    اگر (off_arg <= -ADDSTATE_یہاں_انحراف)
    {
	جوڑو_یہاں = TRUE;
	off = 0;
	listindex = -(off_arg + ADDSTATE_یہاں_انحراف);
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
#اگر_تعریف_شدہ FEAT_SYN_HL
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
#اگر_ختم
	حالت NFA_MOPEN:
	حالت NFA_ZEND:
	حالت NFA_بانٹو:
	حالت NFA_خالی_خانے:
	    // These nodes are not added themselves but their "out" and/or
	    // "out1" may be added below.
	    توڑو;

	حالت NFA_BOL:
	حالت NFA_BOF:
	    // "^" won't match past end-of-line, don't bother trying.
	    // Except when at the end of the line, or when we are going to the
	    // next line for a look-behind match.
	    اگر (rex.داخلہ > rex.سطر
		    && *rex.داخلہ != NUL
		    && (nfa_endp == تہی
			|| !REG_MULTI
			|| rex.lnum == nfa_endp->se_u.pos.lnum))
		جاؤ skip_جوڑو;
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
#اگر_تعریف_شدہ FEAT_SYN_HL
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
#اگر_ختم
	حالت NFA_NOPEN:
	حالت NFA_ZSTART:
	    // These nodes need to be added so that we can bail out when it
	    // was added to this list before at the same position to avoid an
	    // endless loop for "\(\)*"

	طےشدہ:
	    اگر (state->lastlist[nfa_ll_اشاریہ] == l->id && state->c != NFA_SKIP)
	    {
		// This state is already in the list, don't add it again,
		// unless it is an MOPEN that is used for a backreference or
		// when there is a PIM. For NFA_MATCH check the position,
		// lower position is preferred.
		اگر (!rex.nfa_has_backref && pim == تہی && !l->has_pim
						     && state->c != NFA_مطابقت)
		{
		    // When called from addstate_here() do insert before
		    // existing states.
		    اگر (جوڑو_یہاں)
		    {
			برائے (k = 0; k < l->n && k < listindex; ++k)
			    اگر (l->t[k].state->id == state->id)
			    {
				ملا = TRUE;
				توڑو;
			    }
		    }
		    اگر (!جوڑو_یہاں || ملا)
		    {
skip_جوڑو:
#اگر_تعریف_شدہ ENABLE_LOG
			nfa_سیٹ_رمز(state->c);
			fprintf(log_fd, "> Not adding state %d to list %d. char %d: %s pim: %s has_pim: %d found: %d\n",
			    مطلق_قدر(state->id), l->id, state->c, رمز,
			    pim == تہی ? "NULL" : "yes", l->has_pim, ملا);
#اگر_ختم
			واپس subs;
		    }
		}

		// Do not add the state again when it exists with the same
		// positions.
		اگر (has_state_with_pos(l, state, subs, pim))
		    جاؤ skip_جوڑو;
	    }

	    // When there are backreferences or PIMs the number of states may
	    // be (a lot) bigger than anticipated.
	    اگر (l->n == l->len)
	    {
		صحیح		newlen = l->len * 3 / 2 + 50;
		size_t		newsize = newlen * حجم(nfa_thread_T);
		nfa_thread_T	*newt;

		اگر ((طویل)(newsize >> 10) >= p_mmp)
		{
		    emsg(_(e_نمونہ_استعمال_کرتا_مزید_حافظہ_than_maxmempattern));
		    واپس تہی;
		}
		اگر (subs != &عارضی_subs)
		{
		    // "subs" may point into the current array, need to make a
		    // copy before it becomes invalid.
		    copy_sub(&عارضی_subs.norm, &subs->norm);
#اگر_تعریف_شدہ FEAT_SYN_HL
		    اگر (rex.nfa_has_zsubexpr)
			copy_sub(&عارضی_subs.synt, &subs->synt);
#اگر_ختم
		    subs = &عارضی_subs;
		}

		newt = vim_realloc(l->t, newsize);
		اگر (newt == تہی)
		{
		    // out of memory
		    واپس تہی;
		}
		l->t = newt;
		l->len = newlen;
	    }

	    // add the state to the list
	    state->lastlist[nfa_ll_اشاریہ] = l->id;
	    thread = &l->t[l->n++];
	    thread->state = state;
	    اگر (pim == تہی)
		thread->pim.نتیجہ = NFA_PIM_غیر_مستعمل;
	    ورنہ
	    {
		copy_pim(&thread->pim, pim);
		l->has_pim = TRUE;
	    }
	    copy_sub(&thread->subs.norm, &subs->norm);
#اگر_تعریف_شدہ FEAT_SYN_HL
	    اگر (rex.nfa_has_zsubexpr)
		copy_sub(&thread->subs.synt, &subs->synt);
#اگر_ختم
#اگر_تعریف_شدہ ENABLE_LOG
	    report_state("Adding", &thread->subs.norm, state, l->id, pim);
	    did_print = TRUE;
#اگر_ختم
    }

#اگر_تعریف_شدہ ENABLE_LOG
    اگر (!did_print)
	report_state("Processing", &subs->norm, state, l->id, pim);
#اگر_ختم
    انتخاب (state->c)
    {
	حالت NFA_مطابقت:
	    توڑو;

	حالت NFA_بانٹو:
	    // order matters here
	    subs = addstate(l, state->برآمد, subs, pim, off_arg, گہرائی + 1);
	    subs = addstate(l, state->out1, subs, pim, off_arg, گہرائی + 1);
	    توڑو;

	حالت NFA_خالی_خانے:
	حالت NFA_NOPEN:
	حالت NFA_NCLOSE:
	    subs = addstate(l, state->برآمد, subs, pim, off_arg, گہرائی + 1);
	    توڑو;

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
#اگر_تعریف_شدہ FEAT_SYN_HL
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
#اگر_ختم
	حالت NFA_ZSTART:
	    اگر (state->c == NFA_ZSTART)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#اگر_تعریف_شدہ FEAT_SYN_HL
	    ورنہ اگر (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
	    {
		subidx = state->c - NFA_ZOPEN;
		sub = &subs->synt;
	    }
#اگر_ختم
	    ورنہ
	    {
		subidx = state->c - NFA_MOPEN;
		sub = &subs->norm;
	    }

	    // avoid compiler warnings
	    save_ptr = تہی;
	    CLEAR_میدان(save_multipos);

	    // Set the position (with "off" added) in the subexpression.  Save
	    // and restore it when it was in use.  Otherwise fill any gap.
	    اگر (REG_MULTI)
	    {
		اگر (subidx < sub->اندر_use)
		{
		    save_multipos = sub->list.multi[subidx];
		    save_اندر_use = -1;
		}
		ورنہ
		{
		    save_اندر_use = sub->اندر_use;
		    برائے (i = sub->اندر_use; i < subidx; ++i)
		    {
			sub->list.multi[i].آغاز_lnum = -1;
			sub->list.multi[i].انتہا_lnum = -1;
		    }
		    sub->اندر_use = subidx + 1;
		}
		اگر (off == -1)
		{
		    sub->list.multi[subidx].آغاز_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].آغاز_ستون = 0;
		}
		ورنہ
		{
		    sub->list.multi[subidx].آغاز_lnum = rex.lnum;
		    sub->list.multi[subidx].آغاز_ستون =
					 (colnr_T)(rex.داخلہ - rex.سطر + off);
		}
		sub->list.multi[subidx].انتہا_lnum = -1;
	    }
	    ورنہ
	    {
		اگر (subidx < sub->اندر_use)
		{
		    save_ptr = sub->list.سطر[subidx].آغاز;
		    save_اندر_use = -1;
		}
		ورنہ
		{
		    save_اندر_use = sub->اندر_use;
		    برائے (i = sub->اندر_use; i < subidx; ++i)
		    {
			sub->list.سطر[i].آغاز = تہی;
			sub->list.سطر[i].انتہا = تہی;
		    }
		    sub->اندر_use = subidx + 1;
		}
		sub->list.سطر[subidx].آغاز = rex.داخلہ + off;
	    }

	    subs = addstate(l, state->برآمد, subs, pim, off_arg, گہرائی + 1);
	    اگر (subs == تہی)
		توڑو;
	    // "subs" may have changed, need to set "sub" again
#اگر_تعریف_شدہ FEAT_SYN_HL
	    اگر (state->c >= NFA_ZOPEN && state->c <= NFA_ZOPEN9)
		sub = &subs->synt;
	    ورنہ
#اگر_ختم
		sub = &subs->norm;

	    اگر (save_اندر_use == -1)
	    {
		اگر (REG_MULTI)
		    sub->list.multi[subidx] = save_multipos;
		ورنہ
		    sub->list.سطر[subidx].آغاز = save_ptr;
	    }
	    ورنہ
		sub->اندر_use = save_اندر_use;
	    توڑو;

	حالت NFA_MCLOSE:
	    اگر (rex.nfa_has_zend && (REG_MULTI
			? subs->norm.list.multi[0].انتہا_lnum >= 0
			: subs->norm.list.سطر[0].انتہا != تہی))
	    {
		// Do not overwrite the position set by \ze.
		subs = addstate(l, state->برآمد, subs, pim, off_arg, گہرائی + 1);
		توڑو;
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
#اگر_تعریف_شدہ FEAT_SYN_HL
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
#اگر_ختم
	حالت NFA_ZEND:
	    اگر (state->c == NFA_ZEND)
	    {
		subidx = 0;
		sub = &subs->norm;
	    }
#اگر_تعریف_شدہ FEAT_SYN_HL
	    ورنہ اگر (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
	    {
		subidx = state->c - NFA_ZCLOSE;
		sub = &subs->synt;
	    }
#اگر_ختم
	    ورنہ
	    {
		subidx = state->c - NFA_MCLOSE;
		sub = &subs->norm;
	    }

	    // We don't fill in gaps here, there must have been an MOPEN that
	    // has done that.
	    save_اندر_use = sub->اندر_use;
	    اگر (sub->اندر_use <= subidx)
		sub->اندر_use = subidx + 1;
	    اگر (REG_MULTI)
	    {
		save_multipos = sub->list.multi[subidx];
		اگر (off == -1)
		{
		    sub->list.multi[subidx].انتہا_lnum = rex.lnum + 1;
		    sub->list.multi[subidx].انتہا_ستون = 0;
		}
		ورنہ
		{
		    sub->list.multi[subidx].انتہا_lnum = rex.lnum;
		    sub->list.multi[subidx].انتہا_ستون =
					  (colnr_T)(rex.داخلہ - rex.سطر + off);
		}
		// avoid compiler warnings
		save_ptr = تہی;
	    }
	    ورنہ
	    {
		save_ptr = sub->list.سطر[subidx].انتہا;
		sub->list.سطر[subidx].انتہا = rex.داخلہ + off;
		// avoid compiler warnings
		CLEAR_میدان(save_multipos);
	    }

	    subs = addstate(l, state->برآمد, subs, pim, off_arg, گہرائی + 1);
	    اگر (subs == تہی)
		توڑو;
	    // "subs" may have changed, need to set "sub" again
#اگر_تعریف_شدہ FEAT_SYN_HL
	    اگر (state->c >= NFA_ZCLOSE && state->c <= NFA_ZCLOSE9)
		sub = &subs->synt;
	    ورنہ
#اگر_ختم
		sub = &subs->norm;

	    اگر (REG_MULTI)
		sub->list.multi[subidx] = save_multipos;
	    ورنہ
		sub->list.سطر[subidx].انتہا = save_ptr;
	    sub->اندر_use = save_اندر_use;
	    توڑو;
    }
    واپس subs;
}

/*
 * Like addstate(), but the new state(s) are put at position "*ip".
 * Used for zero-width matches, next state to use is the added one.
 * This makes sure the order of states to be tried does not change, which
 * matters for alternatives.
 */
    ساکن regsubs_T *
addstate_یہاں(
    nfa_list_T		*l,	// runtime state list
    nfa_state_T		*state,	// state to update
    regsubs_T		*subs,	// pointers to subexpressions
    nfa_pim_T		*pim,   // postponed look-behind match
    صحیح			*ip)
{
    صحیح tlen = l->n;
    صحیح گنتی;
    صحیح listidx = *ip;
    regsubs_T *r;

    // First add the state(s) at the end, so that we know how many there are.
    // Pass the listidx as offset (avoids adding another argument to
    // addstate()).
    r = addstate(l, state, subs, pim, -listidx - ADDSTATE_یہاں_انحراف, 0);
    اگر (r == تہی)
	واپس تہی;

    // when "*ip" was at the end of the list, nothing to do
    اگر (listidx + 1 == tlen)
	واپس r;

    // re-order to put the new state at the current position
    گنتی = l->n - tlen;
    اگر (گنتی == 0)
	واپس r; // no state got added
    اگر (گنتی == 1)
    {
	// overwrite the current state
	l->t[listidx] = l->t[l->n - 1];
    }
    ورنہ اگر (گنتی > 1)
    {
	اگر (l->n + گنتی - 1 >= l->len)
	{
	    // not enough space to move the new states, reallocate the list
	    // and move the states to the right position
	    صحیح		    newlen = l->len * 3 / 2 + 50;
	    size_t	    newsize = newlen * حجم(nfa_thread_T);
	    nfa_thread_T    *newl;

	    اگر ((طویل)(newsize >> 10) >= p_mmp)
	    {
		emsg(_(e_نمونہ_استعمال_کرتا_مزید_حافظہ_than_maxmempattern));
		واپس تہی;
	    }
	    newl = alloc(newsize);
	    اگر (newl == تہی)
		واپس تہی;
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
	ورنہ
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

    واپس r;
}

/*
 * Check character class "class" against current character c.
 */
    ساکن صحیح
check_char_class(صحیح class, صحیح c)
{
    انتخاب (class)
    {
	حالت NFA_CLASS_ALNUM:
	    اگر (c >= 1 && c < 128 && حرف_یا_ہندسہ(c))
		واپس OK;
	    توڑو;
	حالت NFA_CLASS_ALPHA:
	    اگر (c >= 1 && c < 128 && حرف_ہے(c))
		واپس OK;
	    توڑو;
	حالت NFA_CLASS_BLANK:
	    اگر (c == ' ' || c == '\t')
		واپس OK;
	    توڑو;
	حالت NFA_CLASS_CNTRL:
	    اگر (c >= 1 && c <= 127 && iscntrl(c))
		واپس OK;
	    توڑو;
	حالت NFA_CLASS_DIGIT:
	    اگر (VIM_ISDIGIT(c))
		واپس OK;
	    توڑو;
	حالت NFA_CLASS_گراف:
	    اگر (c >= 1 && c <= 127 && isgraph(c))
		واپس OK;
	    توڑو;
	حالت NFA_CLASS_زیریں:
	    اگر (MB_ISLOWER(c) && c != 170 && c != 186)
		واپس OK;
	    توڑو;
	حالت NFA_CLASS_PRINT:
	    اگر (vim_isprintc(c))
		واپس OK;
	    توڑو;
	حالت NFA_CLASS_PUNCT:
	    اگر (c >= 1 && c < 128 && ispunct(c))
		واپس OK;
	    توڑو;
	حالت NFA_CLASS_SPACE:
	    اگر ((c >= 9 && c <= 13) || (c == ' '))
		واپس OK;
	    توڑو;
	حالت NFA_CLASS_بالائی:
	    اگر (MB_ISUPPER(c))
		واپس OK;
	    توڑو;
	حالت NFA_CLASS_XDIGIT:
	    اگر (vim_isxdigit(c))
		واپس OK;
	    توڑو;
	حالت NFA_CLASS_TAB:
	    اگر (c == '\t')
		واپس OK;
	    توڑو;
	حالت NFA_CLASS_RETURN:
	    اگر (c == '\r')
		واپس OK;
	    توڑو;
	حالت NFA_CLASS_BACKSPACE:
	    اگر (c == '\b')
		واپس OK;
	    توڑو;
	حالت NFA_CLASS_ESCAPE:
	    اگر (c == '\033')
		واپس OK;
	    توڑو;
	حالت NFA_CLASS_IDENT:
	    اگر (vim_isIDc(c))
		واپس OK;
	    توڑو;
	حالت NFA_CLASS_KEYWORD:
	    اگر (reg_iswordc(c))
		واپس OK;
	    توڑو;
	حالت NFA_CLASS_FNAME:
	    اگر (vim_isfilec(c))
		واپس OK;
	    توڑو;

	طےشدہ:
	    // should not be here :P
	    siemsg(_(e_nfa_regexp_invalid_character_class_nr), class);
	    واپس FAIL;
    }
    واپس FAIL;
}

/*
 * Check for a match with subexpression "subidx".
 * Return TRUE if it matches.
 */
    ساکن صحیح
مطابقت_backref(
    regsub_T	*sub,	    // pointers to subexpressions
    صحیح		subidx,
    صحیح		*bytelen)   // out: length of match in bytes
{
    صحیح		len;

    اگر (sub->اندر_use <= subidx)
    {
retempty:
	// backref was not set, match an empty string
	*bytelen = 0;
	واپس TRUE;
    }

    اگر (REG_MULTI)
    {
	اگر (sub->list.multi[subidx].آغاز_lnum < 0
				       || sub->list.multi[subidx].انتہا_lnum < 0)
	    جاؤ retempty;
	اگر (sub->list.multi[subidx].آغاز_lnum == rex.lnum
			       && sub->list.multi[subidx].انتہا_lnum == rex.lnum)
	{
	    len = sub->list.multi[subidx].انتہا_ستون
					  - sub->list.multi[subidx].آغاز_ستون;
	    اگر (cstrncmp(rex.سطر + sub->list.multi[subidx].آغاز_ستون,
							 rex.داخلہ, &len) == 0)
	    {
		*bytelen = len;
		واپس TRUE;
	    }
	}
	ورنہ
	{
	    اگر (مطابقت_with_backref(
			sub->list.multi[subidx].آغاز_lnum,
			sub->list.multi[subidx].آغاز_ستون,
			sub->list.multi[subidx].انتہا_lnum,
			sub->list.multi[subidx].انتہا_ستون,
			bytelen) == RA_مطابقت)
		واپس TRUE;
	}
    }
    ورنہ
    {
	اگر (sub->list.سطر[subidx].آغاز == تہی
					|| sub->list.سطر[subidx].انتہا == تہی)
	    جاؤ retempty;
	len = (صحیح)(sub->list.سطر[subidx].انتہا - sub->list.سطر[subidx].آغاز);
	اگر (cstrncmp(sub->list.سطر[subidx].آغاز, rex.داخلہ, &len) == 0)
	{
	    *bytelen = len;
	    واپس TRUE;
	}
    }
    واپس FALSE;
}

#اگر_تعریف_شدہ FEAT_SYN_HL

/*
 * Check for a match with \z subexpression "subidx".
 * Return TRUE if it matches.
 */
    ساکن صحیح
مطابقت_zref(
    صحیح		subidx,
    صحیح		*bytelen)   // out: length of match in bytes
{
    صحیح		len;

    cleanup_zsubexpr();
    اگر (re_extmatch_اندر == تہی || re_extmatch_اندر->matches[subidx] == تہی)
    {
	// backref was not set, match an empty string
	*bytelen = 0;
	واپس TRUE;
    }

    len = (صحیح)STRLEN(re_extmatch_اندر->matches[subidx]);
    اگر (cstrncmp(re_extmatch_اندر->matches[subidx], rex.داخلہ, &len) == 0)
    {
	*bytelen = len;
	واپس TRUE;
    }
    واپس FALSE;
}
#اگر_ختم

/*
 * Save list IDs for all NFA states of "prog" into "list".
 * Also reset the IDs to zero.
 * Only used for the recursive value lastlist[1].
 */
    ساکن خالی
nfa_save_listids(nfa_regprog_T *prog, صحیح *list)
{
    صحیح		    i;
    nfa_state_T	    *p;

    // Order in the list is reverse, it's a bit faster that way.
    p = &prog->state[0];
    برائے (i = prog->nstate; --i >= 0; )
    {
	list[i] = p->lastlist[1];
	p->lastlist[1] = 0;
	++p;
    }
}

/*
 * Restore list IDs from "list" to all NFA states.
 */
    ساکن خالی
nfa_restore_listids(nfa_regprog_T *prog, صحیح *list)
{
    صحیح		    i;
    nfa_state_T	    *p;

    p = &prog->state[0];
    برائے (i = prog->nstate; --i >= 0; )
    {
	p->lastlist[1] = list[i];
	++p;
    }
}

    ساکن صحیح
nfa_re_عدد_cmp(long_u val, صحیح op, long_u pos)
{
    اگر (op == 1) واپس pos > val;
    اگر (op == 2) واپس pos < val;
    واپس val == pos;
}

ساکن صحیح nfa_regmatch(nfa_regprog_T *prog, nfa_state_T *آغاز, regsubs_T *submatch, regsubs_T *m);

/*
 * Recursively call nfa_regmatch()
 * "pim" is NULL or contains info about a Postponed Invisible Match (start
 * position).
 */
    ساکن صحیح
recursive_regmatch(
    nfa_state_T	    *state,
    nfa_pim_T	    *pim,
    nfa_regprog_T   *prog,
    regsubs_T	    *submatch,
    regsubs_T	    *m,
    صحیح		    **listids,
    صحیح		    *listids_len)
{
    صحیح		save_reginput_ستون = (صحیح)(rex.داخلہ - rex.سطر);
    صحیح		save_reglnum = rex.lnum;
    صحیح		save_nfa_مطابقت = nfa_مطابقت;
    صحیح		save_nfa_listid = rex.nfa_listid;
    save_se_T   *save_nfa_endp = nfa_endp;
    save_se_T   endpos;
    save_se_T   *endposp = تہی;
    صحیح		نتیجہ;
    صحیح		need_restore = FALSE;

    اگر (pim != تہی)
    {
	// start at the position where the postponed match was
	اگر (REG_MULTI)
	    rex.داخلہ = rex.سطر + pim->انتہا.pos.ستون;
	ورنہ
	    rex.داخلہ = pim->انتہا.ptr;
    }

    اگر (state->c == NFA_آغاز_INVISIBLE_BEFORE
	    || state->c == NFA_آغاز_INVISIBLE_BEFORE_پہلا
	    || state->c == NFA_آغاز_INVISIBLE_BEFORE_NEG
	    || state->c == NFA_آغاز_INVISIBLE_BEFORE_NEG_پہلا)
    {
	// The recursive match must end at the current position. When "pim" is
	// not NULL it specifies the current position.
	endposp = &endpos;
	اگر (REG_MULTI)
	{
	    اگر (pim == تہی)
	    {
		endpos.se_u.pos.ستون = (صحیح)(rex.داخلہ - rex.سطر);
		endpos.se_u.pos.lnum = rex.lnum;
	    }
	    ورنہ
		endpos.se_u.pos = pim->انتہا.pos;
	}
	ورنہ
	{
	    اگر (pim == تہی)
		endpos.se_u.ptr = rex.داخلہ;
	    ورنہ
		endpos.se_u.ptr = pim->انتہا.ptr;
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
		اگر (rex.سطر == تہی)
		    // can't go before the first line
		    rex.سطر = reg_getline(++rex.lnum);
	    }
	    rex.داخلہ = rex.سطر;
	}
	ورنہ
	{
	    اگر (REG_MULTI && (صحیح)(rex.داخلہ - rex.سطر) < state->val)
	    {
		// Not enough bytes in this line, go to end of
		// previous line.
		rex.سطر = reg_getline(--rex.lnum);
		اگر (rex.سطر == تہی)
		{
		    // can't go before the first line
		    rex.سطر = reg_getline(++rex.lnum);
		    rex.داخلہ = rex.سطر;
		}
		ورنہ
		    rex.داخلہ = rex.سطر + reg_getline_len(rex.lnum);
	    }
	    اگر ((صحیح)(rex.داخلہ - rex.سطر) >= state->val)
	    {
		rex.داخلہ -= state->val;
		اگر (has_mbyte)
		    rex.داخلہ -= mb_سر_off(rex.سطر, rex.داخلہ);
	    }
	    ورنہ
		rex.داخلہ = rex.سطر;
	}
    }

#اگر_تعریف_شدہ ENABLE_LOG
    اگر (log_fd != stderr)
	fclose(log_fd);
    log_fd = تہی;
#اگر_ختم
    // Have to clear the lastlist field of the NFA nodes, so that
    // nfa_regmatch() and addstate() can run properly after recursion.
    اگر (nfa_ll_اشاریہ == 1)
    {
	// Already calling nfa_regmatch() recursively.  Save the lastlist[1]
	// values and clear them.
	اگر (*listids == تہی || *listids_len < prog->nstate)
	{
	    vim_free(*listids);
	    *listids = ALLOC_MULT(صحیح, prog->nstate);
	    اگر (*listids == تہی)
	    {
		emsg(_(e_nfa_regexp_could_not_allocate_حافظہ_for_branch_traversal));
		واپس 0;
	    }
	    *listids_len = prog->nstate;
	}
	nfa_save_listids(prog, *listids);
	need_restore = TRUE;
	// any value of rex.nfa_listid will do
    }
    ورنہ
    {
	// First recursive nfa_regmatch() call, switch to the second lastlist
	// entry.  Make sure rex.nfa_listid is different from a previous
	// recursive call, because some states may still have this ID.
	++nfa_ll_اشاریہ;
	اگر (rex.nfa_listid <= rex.nfa_alt_listid)
	    rex.nfa_listid = rex.nfa_alt_listid;
    }

    // Call nfa_regmatch() to check if the current concat matches at this
    // position. The concat ends with the node NFA_END_INVISIBLE
    nfa_endp = endposp;
    نتیجہ = nfa_regmatch(prog, state->برآمد, submatch, m);

    اگر (need_restore)
	nfa_restore_listids(prog, *listids);
    ورنہ
    {
	--nfa_ll_اشاریہ;
	rex.nfa_alt_listid = rex.nfa_listid;
    }

    // restore position in input text
    rex.lnum = save_reglnum;
    اگر (REG_MULTI)
	rex.سطر = reg_getline(rex.lnum);
    rex.داخلہ = rex.سطر + save_reginput_ستون;
    اگر (نتیجہ != NFA_TOO_EXPENSIVE)
    {
	nfa_مطابقت = save_nfa_مطابقت;
	rex.nfa_listid = save_nfa_listid;
    }
    nfa_endp = save_nfa_endp;

#اگر_تعریف_شدہ ENABLE_LOG
    open_debug_log(نتیجہ);
#اگر_ختم

    واپس نتیجہ;
}

/*
 * Estimate the chance of a match with "state" failing.
 * empty match: 0
 * NFA_ANY: 1
 * specific character: 99
 */
    ساکن صحیح
failure_chance(nfa_state_T *state, صحیح گہرائی)
{
    صحیح c = state->c;
    صحیح l, r;

    // detect looping
    اگر (گہرائی > 4)
	واپس 1;

    انتخاب (c)
    {
	حالت NFA_بانٹو:
	    اگر (state->برآمد->c == NFA_بانٹو || state->out1->c == NFA_بانٹو)
		// avoid recursive stuff
		واپس 1;
	    // two alternatives, use the lowest failure chance
	    l = failure_chance(state->برآمد, گہرائی + 1);
	    r = failure_chance(state->out1, گہرائی + 1);
	    واپس l < r ? l : r;

	حالت NFA_ANY:
	    // matches anything, unlikely to fail
	    واپس 1;

	حالت NFA_مطابقت:
	حالت NFA_MCLOSE:
	حالت NFA_ANY_COMPOSING:
	    // empty match works always
	    واپس 0;

	حالت NFA_آغاز_INVISIBLE:
	حالت NFA_آغاز_INVISIBLE_پہلا:
	حالت NFA_آغاز_INVISIBLE_NEG:
	حالت NFA_آغاز_INVISIBLE_NEG_پہلا:
	حالت NFA_آغاز_INVISIBLE_BEFORE:
	حالت NFA_آغاز_INVISIBLE_BEFORE_پہلا:
	حالت NFA_آغاز_INVISIBLE_BEFORE_NEG:
	حالت NFA_آغاز_INVISIBLE_BEFORE_NEG_پہلا:
	حالت NFA_آغاز_نمونہ:
	    // recursive regmatch is expensive, use low failure chance
	    واپس 5;

	حالت NFA_BOL:
	حالت NFA_EOL:
	حالت NFA_BOF:
	حالت NFA_EOF:
	حالت NFA_NEWL:
	    واپس 99;

	حالت NFA_BOW:
	حالت NFA_EOW:
	    واپس 90;

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
#اگر_تعریف_شدہ FEAT_SYN_HL
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
#اگر_ختم
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
	    واپس failure_chance(state->برآمد, گہرائی + 1);

	حالت NFA_BACKREF1:
	حالت NFA_BACKREF2:
	حالت NFA_BACKREF3:
	حالت NFA_BACKREF4:
	حالت NFA_BACKREF5:
	حالت NFA_BACKREF6:
	حالت NFA_BACKREF7:
	حالت NFA_BACKREF8:
	حالت NFA_BACKREF9:
#اگر_تعریف_شدہ FEAT_SYN_HL
	حالت NFA_ZREF1:
	حالت NFA_ZREF2:
	حالت NFA_ZREF3:
	حالت NFA_ZREF4:
	حالت NFA_ZREF5:
	حالت NFA_ZREF6:
	حالت NFA_ZREF7:
	حالت NFA_ZREF8:
	حالت NFA_ZREF9:
#اگر_ختم
	    // backreferences don't match in many places
	    واپس 94;

	حالت NFA_LNUM_GT:
	حالت NFA_LNUM_LT:
	حالت NFA_ستون_GT:
	حالت NFA_ستون_LT:
	حالت NFA_VCOL_GT:
	حالت NFA_VCOL_LT:
	حالت NFA_MARK_GT:
	حالت NFA_MARK_LT:
	حالت NFA_VISUAL:
	    // before/after positions don't match very often
	    واپس 85;

	حالت NFA_LNUM:
	    واپس 90;

	حالت NFA_CURSOR:
	حالت NFA_ستون:
	حالت NFA_VCOL:
	حالت NFA_MARK:
	    // specific positions rarely match
	    واپس 98;

	حالت NFA_COMPOSING:
	    واپس 95;

	طےشدہ:
	    اگر (c > 0)
		// character match fails often
		واپس 95;
    }

    // something else, includes character classes
    واپس 50;
}

/*
 * Skip until the char "c" we know a match must start with.
 */
    ساکن صحیح
skip_مقصد_آغاز(صحیح c, colnr_T *colp)
{
    char_u *s;

    // Used often, do some work to avoid call overhead.
    اگر (!rex.reg_ic && !has_mbyte)
	s = vim_strbyte(rex.سطر + *colp, c);
    ورنہ
	s = cstrchr(rex.سطر + *colp, c);
    اگر (s == تہی)
	واپس FAIL;
    *colp = (صحیح)(s - rex.سطر);
    واپس OK;
}

/*
 * Check for a match with match_text.
 * Called after skip_to_start() has found regstart.
 * Returns zero for no match, 1 for a match.
 */
    ساکن طویل
ڈھونڈو_مطابقت_متن(colnr_T *startcol, صحیح regstart, char_u *مطابقت_متن)
{
    colnr_T ستون = *startcol;
    صحیح	    c1, c2;
    صحیح	    len1, len2;
    صحیح	    مطابقت;

    برائے (;;)
    {
	مطابقت = TRUE;
	// Skip the input character: case-folding can change its byte length.
	len2 = MB_CPTR2LEN(rex.سطر + ستون);
	برائے (len1 = 0; مطابقت_متن[len1] != NUL; len1 += MB_CHAR2LEN(c1))
	{
	    c1 = PTR2CHAR(مطابقت_متن + len1);
	    c2 = PTR2CHAR(rex.سطر + ستون + len2);
	    اگر (c1 != c2 && (!rex.reg_ic || MB_CASEFOLD(c1) != MB_CASEFOLD(c2)))
	    {
		مطابقت = FALSE;
		توڑو;
	    }
	    len2 += enc_utf8 ? utf_ptr2len(rex.سطر + ستون + len2)
							     : MB_CHAR2LEN(c2);
	}
	اگر (مطابقت
		// check that no composing char follows
		&& !(enc_utf8
			  && utf_iscomposing(PTR2CHAR(rex.سطر + ستون + len2))))
	{
	    cleanup_subexpr();
	    اگر (REG_MULTI)
	    {
		rex.reg_startpos[0].lnum = rex.lnum;
		rex.reg_startpos[0].ستون = ستون;
		rex.reg_endpos[0].lnum = rex.lnum;
		rex.reg_endpos[0].ستون = ستون + len2;
	    }
	    ورنہ
	    {
		rex.reg_startp[0] = rex.سطر + ستون;
		rex.reg_endp[0] = rex.سطر + ستون + len2;
	    }
	    *startcol = ستون;
	    واپس 1L;
	}

	// Try finding regstart after the current match.
	ستون += MB_CPTR2LEN(rex.سطر + ستون); // skip the input character
	اگر (skip_مقصد_آغاز(regstart, &ستون) == FAIL)
	    توڑو;
    }

    *startcol = ستون;
    واپس 0L;
}

/*
 * Check whether the composing characters at the current input position match
 * the NFA composing sub-expression that starts at "sta" (the first state
 * after NFA_COMPOSING).  "curc" is the base character and "clen" its byte
 * length.
 * Returns OK when it matches, FAIL otherwise.
 */
    ساکن صحیح
مطابقت_composing(nfa_state_T *sta, صحیح curc, صحیح clen)
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
	واپس sta->c == curc ? OK : FAIL;

    // Check base character matches first, unless ignored.
    اگر (len == 0 && mc != sta->c)
	واپس FAIL;

    اگر (len == 0)
    {
	len += mb_char2len(mc);
	sta = sta->برآمد;
    }

    // We don't care about the order of composing characters.  Get them into
    // cchars[] first.
    جب (len < clen)
    {
	mc = mb_ptr2char(rex.داخلہ + len);
	cchars[ccount++] = mc;
	len += mb_char2len(mc);
	اگر (ccount == MAX_MCO)
	    توڑو;
    }

    // Check that each composing char in the pattern matches a composing char
    // in the text.  We do not check if all composing chars are matched.
    جب (sta->c != NFA_انتہا_COMPOSING)
    {
	برائے (j = 0; j < ccount; ++j)
	    اگر (cchars[j] == sta->c)
		توڑو;
	اگر (j == ccount)
	    واپس FAIL;
	sta = sta->برآمد;
    }

    واپس OK;
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
    ساکن صحیح
nfa_regmatch(
    nfa_regprog_T	*prog,
    nfa_state_T		*آغاز,
    regsubs_T		*submatch,
    regsubs_T		*m)
{
    صحیح		نتیجہ = FALSE;
    size_t	سائز = 0;
    صحیح		flag = 0;
    صحیح		go_مقصد_nextline = FALSE;
    nfa_thread_T *t;
    nfa_list_T	list[2];
    صحیح		listidx;
    nfa_list_T	*thislist;
    nfa_list_T	*nextlist;
    صحیح		*listids = تہی;
    صحیح		listids_len = 0;
    nfa_state_T *جوڑو_state;
    صحیح		جوڑو_یہاں;
    صحیح		جوڑو_گنتی;
    صحیح		جوڑو_off = 0;
    صحیح		toplevel = آغاز->c == NFA_MOPEN;
    regsubs_T	*r;
#اگر_تعریف_شدہ NFA_REGEXP_DEBUG_LOG
    FILE	*debug;
#اگر_ختم

    // Some patterns may take a long time to match, especially when using
    // recursive_regmatch(). Allow interrupting them with CTRL-C.
    fast_breakcheck();
    اگر (got_int)
	واپس FALSE;
#اگر_تعریف_شدہ FEAT_RELTIME
    اگر (nfa_did_وقت_برآمد())
	واپس FALSE;
#اگر_ختم

#اگر_تعریف_شدہ NFA_REGEXP_DEBUG_LOG
    debug = fopen(NFA_REGEXP_DEBUG_LOG, "a");
    اگر (debug == تہی)
    {
	semsg("(NFA) COULD NOT OPEN %s!", NFA_REGEXP_DEBUG_LOG);
	واپس FALSE;
    }
#اگر_ختم
    nfa_مطابقت = FALSE;

    // Allocate memory for the lists of nodes.
    سائز = (prog->nstate + 1) * حجم(nfa_thread_T);

    // Reuse cached list buffers from prog when available (top-level call).
    // Recursive calls must allocate their own buffers.
    اگر (toplevel && prog->listbuf[0] != تہی)
    {
	list[0].t = (nfa_thread_T *)prog->listbuf[0];
	list[1].t = (nfa_thread_T *)prog->listbuf[1];
	prog->listbuf[0] = تہی;
	prog->listbuf[1] = تہی;
    }
    ورنہ
    {
	list[0].t = alloc(سائز);
	list[1].t = alloc(سائز);
    }
    list[0].len = prog->nstate + 1;
    list[1].len = prog->nstate + 1;
    اگر (list[0].t == تہی || list[1].t == تہی)
	جاؤ theend;

#اگر_تعریف_شدہ ENABLE_LOG
    log_fd = fopen(NFA_REGEXP_تسلسل_LOG, "a");
    اگر (log_fd == تہی)
    {
	emsg(_(e_log_open_failed));
	log_fd = stderr;
    }
    fprintf(log_fd, "**********************************\n");
    nfa_سیٹ_رمز(آغاز->c);
    fprintf(log_fd, " RUNNING nfa_regmatch() starting with state %d, code %s\n",
    مطلق_قدر(آغاز->id), رمز);
    fprintf(log_fd, "**********************************\n");
#اگر_ختم

    thislist = &list[0];
    thislist->n = 0;
    thislist->has_pim = FALSE;
    nextlist = &list[1];
    nextlist->n = 0;
    nextlist->has_pim = FALSE;
#اگر_تعریف_شدہ ENABLE_LOG
    fprintf(log_fd, "(---) STARTSTATE first\n");
#اگر_ختم
    thislist->id = rex.nfa_listid + 1;

    // Inline optimized code for addstate(thislist, start, m, 0) if we know
    // it's the first MOPEN.
    اگر (toplevel)
    {
	اگر (REG_MULTI)
	{
	    m->norm.list.multi[0].آغاز_lnum = rex.lnum;
	    m->norm.list.multi[0].آغاز_ستون = (colnr_T)(rex.داخلہ - rex.سطر);
	    m->norm.orig_آغاز_ستون = m->norm.list.multi[0].آغاز_ستون;
	}
	ورنہ
	    m->norm.list.سطر[0].آغاز = rex.داخلہ;
	m->norm.اندر_use = 1;
	r = addstate(thislist, آغاز->برآمد, m, تہی, 0, 0);
    }
    ورنہ
	r = addstate(thislist, آغاز, m, تہی, 0, 0);
    اگر (r == تہی)
    {
	nfa_مطابقت = NFA_TOO_EXPENSIVE;
	جاؤ theend;
    }

#تعریف_کرو	جوڑو_STATE_IF_مطابقت(state)	\
    اگر (نتیجہ)				\
    {					\
	جوڑو_state = state->برآمد;		\
	جوڑو_off = clen;			\
    }

    /*
     * Run for each character.
     */
    برائے (;;)
    {
	صحیح	curc;
	صحیح	clen;

	اگر (has_mbyte)
	{
	    // Fast path for an ASCII byte not followed by a composing
	    // character, matching the check in utfc_ptr2len().  Avoids two
	    // indirect calls for the common case.
	    اگر (rex.داخلہ[0] != NUL && rex.داخلہ[0] < 0x80 && rex.داخلہ[1] < 0x80)
	    {
		curc = rex.داخلہ[0];
		clen = 1;
	    }
	    ورنہ
	    {
		curc = (*mb_ptr2char)(rex.داخلہ);
		clen = (*mb_ptr2len)(rex.داخلہ);
	    }
	}
	ورنہ
	{
	    curc = *rex.داخلہ;
	    clen = 1;
	}
	اگر (curc == NUL)
	{
	    clen = 0;
	    go_مقصد_nextline = FALSE;
	}

	// swap lists
	thislist = &list[flag];
	nextlist = &list[flag ^= 1];
	nextlist->n = 0;	    // clear nextlist
	nextlist->has_pim = FALSE;
	++rex.nfa_listid;
	اگر (prog->re_engine == AUTOMATIC_ENGINE
		&& (rex.nfa_listid >= NFA_MAX_حالتیں
#اگر_تعریف_شدہ FEAT_EVAL
		    || nfa_fail_for_testing
#اگر_ختم
		    ))
	{
	    // too many states, retry with old engine
	    nfa_مطابقت = NFA_TOO_EXPENSIVE;
	    جاؤ theend;
	}

	thislist->id = rex.nfa_listid;
	nextlist->id = rex.nfa_listid + 1;

#اگر_تعریف_شدہ ENABLE_LOG
	fprintf(log_fd, "------------------------------------------\n");
	fprintf(log_fd, ">>> Reginput is \"%s\"\n", rex.داخلہ);
	fprintf(log_fd, ">>> Advanced one character... Current char is %c (code %d) \n", curc, (صحیح)curc);
	fprintf(log_fd, ">>> Thislist has %d states available: ", thislist->n);
	{
	    صحیح i;

	    برائے (i = 0; i < thislist->n; i++)
		fprintf(log_fd, "%d  ", مطلق_قدر(thislist->t[i].state->id));
	}
	fprintf(log_fd, "\n");
#اگر_ختم

#اگر_تعریف_شدہ NFA_REGEXP_DEBUG_LOG
	fprintf(debug, "\n-------------------\n");
#اگر_ختم
	/*
	 * If the state lists are empty we can stop.
	 */
	اگر (thislist->n == 0)
	    توڑو;

	// compute nextlist
	برائے (listidx = 0; listidx < thislist->n; ++listidx)
	{
	    // If the list gets very long there probably is something wrong.
	    // At least allow interrupting with CTRL-C.  Only check once in a
	    // while, calling ui_breakcheck() for every state is too slow.
	    ساکن صحیح	breakcheck_گنتی = 0;  // using "static" makes it faster
	    اگر (unlikely(++breakcheck_گنتی >= 100000))
	    {
		ui_breakcheck();
		breakcheck_گنتی = 0;
	    }
	    اگر (got_int)
		توڑو;
#اگر_تعریف_شدہ FEAT_RELTIME
	    اگر (nfa_did_وقت_برآمد())
		توڑو;
#اگر_ختم
	    t = &thislist->t[listidx];

#اگر_تعریف_شدہ NFA_REGEXP_DEBUG_LOG
	    nfa_سیٹ_رمز(t->state->c);
	    fprintf(debug, "%s, ", رمز);
#اگر_ختم
#اگر_تعریف_شدہ ENABLE_LOG
	    {
		صحیح ستون;

		اگر (t->subs.norm.اندر_use <= 0)
		    ستون = -1;
		ورنہ اگر (REG_MULTI)
		    ستون = t->subs.norm.list.multi[0].آغاز_ستون;
		ورنہ
		    ستون = (صحیح)(t->subs.norm.list.سطر[0].آغاز - rex.سطر);
		nfa_سیٹ_رمز(t->state->c);
		fprintf(log_fd, "(%d) char %d %s (start col %d)%s... \n",
			مطلق_قدر(t->state->id), (صحیح)t->state->c, رمز, ستون,
			pim_info(&t->pim));
	    }
#اگر_ختم

	    /*
	     * Handle the possible codes of the current state.
	     * The most important is NFA_MATCH.
	     */
	    جوڑو_state = تہی;
	    جوڑو_یہاں = FALSE;
	    جوڑو_گنتی = 0;
	    انتخاب (t->state->c)
	    {
	    حالت NFA_مطابقت:
	      {
		// If the match is not at the start of the line, ends before a
		// composing characters and rex.reg_icombine is not set, that
		// is not really a match.
		اگر (enc_utf8 && !rex.reg_icombine
			     && rex.داخلہ != rex.سطر && utf_iscomposing(curc))
		    توڑو;

		nfa_مطابقت = TRUE;
		copy_sub(&submatch->norm, &t->subs.norm);
#اگر_تعریف_شدہ FEAT_SYN_HL
		اگر (rex.nfa_has_zsubexpr)
		    copy_sub(&submatch->synt, &t->subs.synt);
#اگر_ختم
#اگر_تعریف_شدہ ENABLE_LOG
		log_subsexpr(&t->subs);
#اگر_ختم
		// Found the left-most longest match, do not look at any other
		// states at this position.  When the list of states is going
		// to be empty quit without advancing, so that "rex.input" is
		// correct.
		اگر (nextlist->n == 0)
		    clen = 0;
		جاؤ nextchar;
	      }

	    حالت NFA_انتہا_INVISIBLE:
	    حالت NFA_انتہا_INVISIBLE_NEG:
	    حالت NFA_انتہا_نمونہ:
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
#اگر_تعریف_شدہ ENABLE_LOG
		اگر (nfa_endp != تہی)
		{
		    اگر (REG_MULTI)
			fprintf(log_fd, "Current lnum: %d, endp lnum: %d; current col: %d, endp col: %d\n",
				(صحیح)rex.lnum,
				(صحیح)nfa_endp->se_u.pos.lnum,
				(صحیح)(rex.داخلہ - rex.سطر),
				nfa_endp->se_u.pos.ستون);
		    ورنہ
			fprintf(log_fd, "Current col: %d, endp col: %d\n",
				(صحیح)(rex.داخلہ - rex.سطر),
				(صحیح)(nfa_endp->se_u.ptr - rex.داخلہ));
		}
#اگر_ختم
		// If "nfa_endp" is set it's only a match if it ends at
		// "nfa_endp"
		اگر (nfa_endp != تہی && (REG_MULTI
			? (rex.lnum != nfa_endp->se_u.pos.lnum
			    || (صحیح)(rex.داخلہ - rex.سطر)
						!= nfa_endp->se_u.pos.ستون)
			: rex.داخلہ != nfa_endp->se_u.ptr))
		    توڑو;

		// do not set submatches for \@!
		اگر (t->state->c != NFA_انتہا_INVISIBLE_NEG)
		{
		    copy_sub(&m->norm, &t->subs.norm);
#اگر_تعریف_شدہ FEAT_SYN_HL
		    اگر (rex.nfa_has_zsubexpr)
			copy_sub(&m->synt, &t->subs.synt);
#اگر_ختم
		}
#اگر_تعریف_شدہ ENABLE_LOG
		fprintf(log_fd, "Match found:\n");
		log_subsexpr(m);
#اگر_ختم
		nfa_مطابقت = TRUE;
		// See comment above at "goto nextchar".
		اگر (nextlist->n == 0)
		    clen = 0;
		جاؤ nextchar;

	    حالت NFA_آغاز_INVISIBLE:
	    حالت NFA_آغاز_INVISIBLE_پہلا:
	    حالت NFA_آغاز_INVISIBLE_NEG:
	    حالت NFA_آغاز_INVISIBLE_NEG_پہلا:
	    حالت NFA_آغاز_INVISIBLE_BEFORE:
	    حالت NFA_آغاز_INVISIBLE_BEFORE_پہلا:
	    حالت NFA_آغاز_INVISIBLE_BEFORE_NEG:
	    حالت NFA_آغاز_INVISIBLE_BEFORE_NEG_پہلا:
		{
#اگر_تعریف_شدہ ENABLE_LOG
		    fprintf(log_fd, "Failure chance invisible: %d, what follows: %d\n",
			    failure_chance(t->state->برآمد, 0),
			    failure_chance(t->state->out1->برآمد, 0));
#اگر_ختم
		    // Do it directly if there already is a PIM or when
		    // nfa_postprocess() detected it will work better.
		    اگر (t->pim.نتیجہ != NFA_PIM_غیر_مستعمل
			 || t->state->c == NFA_آغاز_INVISIBLE_پہلا
			 || t->state->c == NFA_آغاز_INVISIBLE_NEG_پہلا
			 || t->state->c == NFA_آغاز_INVISIBLE_BEFORE_پہلا
			 || t->state->c == NFA_آغاز_INVISIBLE_BEFORE_NEG_پہلا)
		    {
			صحیح اندر_use = m->norm.اندر_use;

			// Copy submatch info for the recursive call, opposite
			// of what happens on success below.
			copy_sub_off(&m->norm, &t->subs.norm);
#اگر_تعریف_شدہ FEAT_SYN_HL
			اگر (rex.nfa_has_zsubexpr)
			    copy_sub_off(&m->synt, &t->subs.synt);
#اگر_ختم

			/*
			 * First try matching the invisible match, then what
			 * follows.
			 */
			نتیجہ = recursive_regmatch(t->state, تہی, prog,
					  submatch, m, &listids, &listids_len);
			اگر (نتیجہ == NFA_TOO_EXPENSIVE)
			{
			    nfa_مطابقت = نتیجہ;
			    جاؤ theend;
			}

			// for \@! and \@<! it is a match when the result is
			// FALSE
			اگر (نتیجہ != (t->state->c == NFA_آغاز_INVISIBLE_NEG
			       || t->state->c == NFA_آغاز_INVISIBLE_NEG_پہلا
			       || t->state->c
					   == NFA_آغاز_INVISIBLE_BEFORE_NEG
			       || t->state->c
				     == NFA_آغاز_INVISIBLE_BEFORE_NEG_پہلا))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&t->subs.norm, &m->norm);
#اگر_تعریف_شدہ FEAT_SYN_HL
			    اگر (rex.nfa_has_zsubexpr)
				copy_sub_off(&t->subs.synt, &m->synt);
#اگر_ختم
			    // If the pattern has \ze and it matched in the
			    // sub pattern, use it.
			    copy_ze_off(&t->subs.norm, &m->norm);

			    // t->state->out1 is the corresponding
			    // END_INVISIBLE node; Add its out to the current
			    // list (zero-width match).
			    جوڑو_یہاں = TRUE;
			    جوڑو_state = t->state->out1->برآمد;
			}
			m->norm.اندر_use = اندر_use;
		    }
		    ورنہ
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
			pim.subs.norm.اندر_use = 0;
#اگر_تعریف_شدہ FEAT_SYN_HL
			pim.subs.synt.اندر_use = 0;
#اگر_ختم
			اگر (REG_MULTI)
			{
			    pim.انتہا.pos.ستون = (صحیح)(rex.داخلہ - rex.سطر);
			    pim.انتہا.pos.lnum = rex.lnum;
			}
			ورنہ
			    pim.انتہا.ptr = rex.داخلہ;

			// t->state->out1 is the corresponding END_INVISIBLE
			// node; Add its out to the current list (zero-width
			// match).
			اگر (addstate_یہاں(thislist, t->state->out1->برآمد,
					     &t->subs, &pim, &listidx) == تہی)
			{
			    nfa_مطابقت = NFA_TOO_EXPENSIVE;
			    جاؤ theend;
			}
		    }
		}
		توڑو;

	    حالت NFA_آغاز_نمونہ:
	      {
		nfa_state_T *skip = تہی;
#اگر_تعریف_شدہ ENABLE_LOG
		صحیح	    skip_lid = 0;
#اگر_ختم

		// There is no point in trying to match the pattern if the
		// output state is not going to be added to the list.
		اگر (state_اندر_list(nextlist, t->state->out1->برآمد, &t->subs))
		{
		    skip = t->state->out1->برآمد;
#اگر_تعریف_شدہ ENABLE_LOG
		    skip_lid = nextlist->id;
#اگر_ختم
		}
		ورنہ اگر (state_اندر_list(nextlist,
					  t->state->out1->برآمد->برآمد, &t->subs))
		{
		    skip = t->state->out1->برآمد->برآمد;
#اگر_تعریف_شدہ ENABLE_LOG
		    skip_lid = nextlist->id;
#اگر_ختم
		}
		ورنہ اگر (state_اندر_list(thislist,
					  t->state->out1->برآمد->برآمد, &t->subs))
		{
		    skip = t->state->out1->برآمد->برآمد;
#اگر_تعریف_شدہ ENABLE_LOG
		    skip_lid = thislist->id;
#اگر_ختم
		}
		اگر (skip != تہی)
		{
#اگر_تعریف_شدہ ENABLE_LOG
		    nfa_سیٹ_رمز(skip->c);
		    fprintf(log_fd, "> Not trying to match pattern, output state %d is already in list %d. char %d: %s\n",
			    مطلق_قدر(skip->id), skip_lid, skip->c, رمز);
#اگر_ختم
		    توڑو;
		}
		// Copy submatch info to the recursive call, opposite of what
		// happens afterwards.
		copy_sub_off(&m->norm, &t->subs.norm);
#اگر_تعریف_شدہ FEAT_SYN_HL
		اگر (rex.nfa_has_zsubexpr)
		    copy_sub_off(&m->synt, &t->subs.synt);
#اگر_ختم

		// First try matching the pattern.
		نتیجہ = recursive_regmatch(t->state, تہی, prog,
					  submatch, m, &listids, &listids_len);
		اگر (نتیجہ == NFA_TOO_EXPENSIVE)
		{
		    nfa_مطابقت = نتیجہ;
		    جاؤ theend;
		}
		اگر (نتیجہ)
		{
		    صحیح bytelen;

#اگر_تعریف_شدہ ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN matches:\n");
		    log_subsexpr(m);
#اگر_ختم
		    // Copy submatch info from the recursive call
		    copy_sub_off(&t->subs.norm, &m->norm);
#اگر_تعریف_شدہ FEAT_SYN_HL
		    اگر (rex.nfa_has_zsubexpr)
			copy_sub_off(&t->subs.synt, &m->synt);
#اگر_ختم
		    // Now we need to skip over the matched text and then
		    // continue with what follows.
		    اگر (REG_MULTI)
			// TODO: multi-line match
			bytelen = m->norm.list.multi[0].انتہا_ستون
						  - (صحیح)(rex.داخلہ - rex.سطر);
		    ورنہ
			bytelen = (صحیح)(m->norm.list.سطر[0].انتہا - rex.داخلہ);

#اگر_تعریف_شدہ ENABLE_LOG
		    fprintf(log_fd, "NFA_START_PATTERN length: %d\n", bytelen);
#اگر_ختم
		    اگر (bytelen == 0)
		    {
			// empty match, output of corresponding
			// NFA_END_PATTERN/NFA_SKIP to be used at current
			// position
			جوڑو_یہاں = TRUE;
			جوڑو_state = t->state->out1->برآمد->برآمد;
		    }
		    ورنہ اگر (bytelen <= clen)
		    {
			// match current character, output of corresponding
			// NFA_END_PATTERN to be used at next position.
			جوڑو_state = t->state->out1->برآمد->برآمد;
			جوڑو_off = clen;
		    }
		    ورنہ
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			جوڑو_state = t->state->out1->برآمد;
			جوڑو_off = bytelen;
			جوڑو_گنتی = bytelen - clen;
		    }
		}
		توڑو;
	      }

	    حالت NFA_BOL:
		اگر (rex.داخلہ == rex.سطر)
		{
		    جوڑو_یہاں = TRUE;
		    جوڑو_state = t->state->برآمد;
		}
		توڑو;

	    حالت NFA_EOL:
		اگر (curc == NUL)
		{
		    جوڑو_یہاں = TRUE;
		    جوڑو_state = t->state->برآمد;
		}
		توڑو;

	    حالت NFA_BOW:
		نتیجہ = TRUE;

		اگر (curc == NUL)
		    نتیجہ = FALSE;
		ورنہ اگر (has_mbyte)
		{
		    صحیح this_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.داخلہ, rex.reg_buf);
		    اگر (this_class <= 1)
			نتیجہ = FALSE;
		    ورنہ اگر (reg_پچھلا_class() == this_class)
			نتیجہ = FALSE;
		}
		ورنہ اگر (!vim_iswordc_buf(curc, rex.reg_buf)
			   || (rex.داخلہ > rex.سطر
			       && vim_iswordc_buf(rex.داخلہ[-1], rex.reg_buf)))
		    نتیجہ = FALSE;
		اگر (نتیجہ)
		{
		    جوڑو_یہاں = TRUE;
		    جوڑو_state = t->state->برآمد;
		}
		توڑو;

	    حالت NFA_EOW:
		نتیجہ = TRUE;
		اگر (rex.داخلہ == rex.سطر)
		    نتیجہ = FALSE;
		ورنہ اگر (has_mbyte)
		{
		    صحیح this_class, پچھلا_class;

		    // Get class of current and previous char (if it exists).
		    this_class = mb_get_class_buf(rex.داخلہ, rex.reg_buf);
		    پچھلا_class = reg_پچھلا_class();
		    اگر (this_class == پچھلا_class
					|| پچھلا_class == 0 || پچھلا_class == 1)
			نتیجہ = FALSE;
		}
		ورنہ اگر (!vim_iswordc_buf(rex.داخلہ[-1], rex.reg_buf)
			|| (rex.داخلہ[0] != NUL
					&& vim_iswordc_buf(curc, rex.reg_buf)))
		    نتیجہ = FALSE;
		اگر (نتیجہ)
		{
		    جوڑو_یہاں = TRUE;
		    جوڑو_state = t->state->برآمد;
		}
		توڑو;

	    حالت NFA_BOF:
		اگر (rex.lnum == 0 && rex.داخلہ == rex.سطر
				     && (!REG_MULTI || rex.reg_firstlnum == 1))
		{
		    جوڑو_یہاں = TRUE;
		    جوڑو_state = t->state->برآمد;
		}
		توڑو;

	    حالت NFA_EOF:
		اگر (rex.lnum == rex.reg_maxline && curc == NUL)
		{
		    جوڑو_یہاں = TRUE;
		    جوڑو_state = t->state->برآمد;
		}
		توڑو;

	    حالت NFA_COMPOSING:
	    {
		nfa_state_T *انتہا;

		نتیجہ = مطابقت_composing(t->state->برآمد, curc, clen);

		انتہا = t->state->out1;	    // NFA_END_COMPOSING
		جوڑو_STATE_IF_مطابقت(انتہا);
		توڑو;
	    }

	    حالت NFA_NEWL:
		اگر (curc == NUL && !rex.reg_سطر_lbr && REG_MULTI
						&& rex.lnum <= rex.reg_maxline)
		{
		    go_مقصد_nextline = TRUE;
		    // Pass -1 for the offset, which means taking the position
		    // at the start of the next line.
		    جوڑو_state = t->state->برآمد;
		    جوڑو_off = -1;
		}
		ورنہ اگر (curc == '\n' && rex.reg_سطر_lbr)
		{
		    // match \n as if it is an ordinary character
		    جوڑو_state = t->state->برآمد;
		    جوڑو_off = 1;
		}
		توڑو;

	    حالت NFA_آغاز_COLL:
	    حالت NFA_آغاز_NEG_COLL:
	      {
		// What follows is a list of characters, until NFA_END_COLL.
		// One of them must match or none of them must match.
		nfa_state_T	*state;
		صحیح		نتیجہ_if_matched;
		صحیح		c1, c2;

		// Never match EOL. If it's part of the collection it is added
		// as a separate state with an OR.
		اگر (curc == NUL)
		    توڑو;

		state = t->state->برآمد;
		نتیجہ_if_matched = (t->state->c == NFA_آغاز_COLL);
		برائے (;;)
		{
		    اگر (state->c == NFA_COMPOSING)
		    {
			nfa_state_T *انتہا;

			نتیجہ = مطابقت_composing(t->state->برآمد->برآمد, curc,
									clen);

			اگر (t->state->برآمد->out1 != تہی
				&& t->state->برآمد->out1->c == NFA_انتہا_COMPOSING)
			{
			    انتہا = t->state->برآمد->out1;
			    جوڑو_STATE_IF_مطابقت(انتہا);
			}
			توڑو;
		    }
		    اگر (state->c == NFA_انتہا_COLL)
		    {
			نتیجہ = !نتیجہ_if_matched;
			توڑو;
		    }
		    اگر (state->c == NFA_RANGE_MIN)
		    {
			c1 = state->val;
			state = state->برآمد; // advance to NFA_RANGE_MAX
			c2 = state->val;
#اگر_تعریف_شدہ ENABLE_LOG
			fprintf(log_fd, "NFA_RANGE_MIN curc=%d c1=%d c2=%d\n",
				curc, c1, c2);
#اگر_ختم
			اگر (curc >= c1 && curc <= c2)
			{
			    نتیجہ = نتیجہ_if_matched;
			    توڑو;
			}
			اگر (rex.reg_ic)
			{
			    صحیح curc_کم = MB_CASEFOLD(curc);
			    صحیح مکمل = FALSE;

			    برائے ( ; c1 <= c2; ++c1)
				اگر (MB_CASEFOLD(c1) == curc_کم)
				{
				    نتیجہ = نتیجہ_if_matched;
				    مکمل = TRUE;
				    توڑو;
				}
			    اگر (مکمل)
				توڑو;
			}
		    }
		    ورنہ اگر (state->c < 0 ? check_char_class(state->c, curc)
			       : (curc == state->c
				   || (rex.reg_ic && MB_CASEFOLD(curc)
						    == MB_CASEFOLD(state->c))))
		    {
			نتیجہ = نتیجہ_if_matched;
			توڑو;
		    }
		    state = state->برآمد;
		}
		اگر (نتیجہ)
		{
		    // next state is in out of the NFA_END_COLL, out1 of
		    // START points to the END state
		    جوڑو_state = t->state->out1->برآمد;
		    جوڑو_off = clen;
		}
		توڑو;
	      }

	    حالت NFA_ANY:
		// Any char except '\0', (end of input) does not match.
		اگر (curc > 0)
		{
		    جوڑو_state = t->state->برآمد;
		    جوڑو_off = clen;
		}
		توڑو;

	    حالت NFA_ANY_COMPOSING:
		// On a composing character skip over it.  Otherwise do
		// nothing.  Always matches.
		اگر (enc_utf8 && utf_iscomposing(curc))
		{
		    جوڑو_off = clen;
		}
		ورنہ
		{
		    جوڑو_یہاں = TRUE;
		    جوڑو_off = 0;
		}
		جوڑو_state = t->state->برآمد;
		توڑو;

	    /*
	     * Character classes like \a for alpha, \d for digit etc.
	     */
	    حالت NFA_IDENT:	//  \i
		نتیجہ = vim_isIDc(curc);
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_SIDENT:	//  \I
		نتیجہ = !VIM_ISDIGIT(curc) && vim_isIDc(curc);
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_KWORD:	//  \k
		نتیجہ = vim_iswordp_buf(rex.داخلہ, rex.reg_buf);
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_SKWORD:	//  \K
		نتیجہ = !VIM_ISDIGIT(curc)
				     && vim_iswordp_buf(rex.داخلہ, rex.reg_buf);
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_FNAME:	//  \f
		نتیجہ = vim_isfilec(curc);
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_SFNAME:	//  \F
		نتیجہ = !VIM_ISDIGIT(curc) && vim_isfilec(curc);
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_PRINT:	//  \p
		نتیجہ = vim_isprintc(PTR2CHAR(rex.داخلہ));
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_SPRINT:	//  \P
		نتیجہ = !VIM_ISDIGIT(curc) && vim_isprintc(PTR2CHAR(rex.داخلہ));
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_WHITE:	//  \s
		نتیجہ = VIM_ISWHITE(curc);
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_NWHITE:	//  \S
		نتیجہ = curc != NUL && !VIM_ISWHITE(curc);
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_DIGIT:	//  \d
		نتیجہ = ri_digit(curc);
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_NDIGIT:	//  \D
		نتیجہ = curc != NUL && !ri_digit(curc);
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_HEX:	//  \x
		نتیجہ = ri_hex(curc);
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_NHEX:	//  \X
		نتیجہ = curc != NUL && !ri_hex(curc);
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_OCTAL:	//  \o
		نتیجہ = ri_octal(curc);
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_NOCTAL:	//  \O
		نتیجہ = curc != NUL && !ri_octal(curc);
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_لفظ:	//  \w
		نتیجہ = ri_لفظ(curc);
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_NWORD:	//  \W
		نتیجہ = curc != NUL && !ri_لفظ(curc);
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_سر:	//  \h
		نتیجہ = ri_سر(curc);
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_NHEAD:	//  \H
		نتیجہ = curc != NUL && !ri_سر(curc);
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_ALPHA:	//  \a
		نتیجہ = ri_alpha(curc);
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_NALPHA:	//  \A
		نتیجہ = curc != NUL && !ri_alpha(curc);
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_زیریں:	//  \l
		نتیجہ = ri_زیریں(curc);
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_NLOWER:	//  \L
		نتیجہ = curc != NUL && !ri_زیریں(curc);
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_بالائی:	//  \u
		نتیجہ = ri_بالائی(curc);
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_NUPPER:	// \U
		نتیجہ = curc != NUL && !ri_بالائی(curc);
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_زیریں_IC:	// [a-z]
		نتیجہ = ri_زیریں(curc) || (rex.reg_ic && ri_بالائی(curc));
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_NLOWER_IC:	// [^a-z]
		نتیجہ = curc != NUL
			&& !(ri_زیریں(curc) || (rex.reg_ic && ri_بالائی(curc)));
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_بالائی_IC:	// [A-Z]
		نتیجہ = ri_بالائی(curc) || (rex.reg_ic && ri_زیریں(curc));
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_NUPPER_IC:	// ^[A-Z]
		نتیجہ = curc != NUL
			&& !(ri_بالائی(curc) || (rex.reg_ic && ri_زیریں(curc)));
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;

	    حالت NFA_BACKREF1:
	    حالت NFA_BACKREF2:
	    حالت NFA_BACKREF3:
	    حالت NFA_BACKREF4:
	    حالت NFA_BACKREF5:
	    حالت NFA_BACKREF6:
	    حالت NFA_BACKREF7:
	    حالت NFA_BACKREF8:
	    حالت NFA_BACKREF9:
#اگر_تعریف_شدہ FEAT_SYN_HL
	    حالت NFA_ZREF1:
	    حالت NFA_ZREF2:
	    حالت NFA_ZREF3:
	    حالت NFA_ZREF4:
	    حالت NFA_ZREF5:
	    حالت NFA_ZREF6:
	    حالت NFA_ZREF7:
	    حالت NFA_ZREF8:
	    حالت NFA_ZREF9:
#اگر_ختم
		// \1 .. \9  \z1 .. \z9
	      {
		صحیح subidx;
		صحیح bytelen;

#اگر_تعریف_شدہ FEAT_SYN_HL
		اگر (t->state->c >= NFA_BACKREF1 && t->state->c <= NFA_BACKREF9)
#اگر_ختم
		{
		    subidx = t->state->c - NFA_BACKREF1 + 1;
		    نتیجہ = مطابقت_backref(&t->subs.norm, subidx, &bytelen);
		}
#اگر_تعریف_شدہ FEAT_SYN_HL
		ورنہ
		{
		    subidx = t->state->c - NFA_ZREF1 + 1;
		    نتیجہ = مطابقت_zref(subidx, &bytelen);
		}
#اگر_ختم

		اگر (نتیجہ)
		{
		    اگر (bytelen == 0)
		    {
			// empty match always works, output of NFA_SKIP to be
			// used next
			جوڑو_یہاں = TRUE;
			جوڑو_state = t->state->برآمد->برآمد;
		    }
		    ورنہ اگر (bytelen <= clen)
		    {
			// match current character, jump ahead to out of
			// NFA_SKIP
			جوڑو_state = t->state->برآمد->برآمد;
			جوڑو_off = clen;
		    }
		    ورنہ
		    {
			// skip over the matched characters, set character
			// count in NFA_SKIP
			جوڑو_state = t->state->برآمد;
			جوڑو_off = bytelen;
			جوڑو_گنتی = bytelen - clen;
		    }
		}
		توڑو;
	      }
	    حالت NFA_SKIP:
	      // character of previous matching \1 .. \9  or \@>
	      اگر (t->گنتی - clen <= 0)
	      {
		  // end of match, go to what follows
		  جوڑو_state = t->state->برآمد;
		  جوڑو_off = clen;
	      }
	      ورنہ
	      {
		  // add state again with decremented count
		  جوڑو_state = t->state;
		  جوڑو_off = 0;
		  جوڑو_گنتی = t->گنتی - clen;
	      }
	      توڑو;

	    حالت NFA_LNUM:
	    حالت NFA_LNUM_GT:
	    حالت NFA_LNUM_LT:
		نتیجہ = (REG_MULTI &&
			nfa_re_عدد_cmp(t->state->val, t->state->c - NFA_LNUM,
			    (long_u)(rex.lnum + rex.reg_firstlnum)));
		اگر (نتیجہ)
		{
		    جوڑو_یہاں = TRUE;
		    جوڑو_state = t->state->برآمد;
		}
		توڑو;

	    حالت NFA_ستون:
	    حالت NFA_ستون_GT:
	    حالت NFA_ستون_LT:
		نتیجہ = nfa_re_عدد_cmp(t->state->val, t->state->c - NFA_ستون,
			(long_u)(rex.داخلہ - rex.سطر) + 1);
		اگر (نتیجہ)
		{
		    جوڑو_یہاں = TRUE;
		    جوڑو_state = t->state->برآمد;
		}
		توڑو;

	    حالت NFA_VCOL:
	    حالت NFA_VCOL_GT:
	    حالت NFA_VCOL_LT:
		{
		    صحیح     op = t->state->c - NFA_VCOL;
		    colnr_T ستون = (colnr_T)(rex.داخلہ - rex.سطر);
		    win_T   *wp = rex.reg_win == تہی ? curwin : rex.reg_win;

		    // Bail out quickly when there can't be a match, avoid the
		    // overhead of win_linetabsize() on long lines.
		    اگر (op != 1 && ستون > t->state->val
			    * (has_mbyte ? MB_MAXBYTES : 1))
			توڑو;
		    نتیجہ = FALSE;
		    اگر (op == 1 && ستون - 1 > t->state->val && ستون > 100)
		    {
			صحیح ts = wp->w_بفر->b_p_ts;

			// Guess that a character won't use more columns than
			// 'tabstop', with a minimum of 4.
			اگر (ts < 4)
			    ts = 4;
			نتیجہ = ستون > t->state->val * ts;
		    }
		    اگر (!نتیجہ)
		    {
			linenr_T    lnum = REG_MULTI
					    ? rex.reg_firstlnum + rex.lnum : 1;
			long_u	    vcol;

			اگر (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_بفر->b_ml.ml_سطر_گنتی))
			    lnum = 1;
			vcol = (long_u)win_linetabsize(wp, lnum, rex.سطر, ستون);
			نتیجہ = nfa_re_عدد_cmp(t->state->val, op, vcol + 1);
		    }
		    اگر (نتیجہ)
		    {
			جوڑو_یہاں = TRUE;
			جوڑو_state = t->state->برآمد;
		    }
		}
		توڑو;

	    حالت NFA_MARK:
	    حالت NFA_MARK_GT:
	    حالت NFA_MARK_LT:
	      {
		pos_T	*pos;
		size_t	ستون = REG_MULTI ? rex.داخلہ - rex.سطر : 0;

		pos = getmark_buf(rex.reg_buf, t->state->val, FALSE);

		// Line may have been freed, get it again.
		اگر (REG_MULTI)
		{
		    rex.سطر = reg_getline(rex.lnum);
		    rex.داخلہ = rex.سطر + ستون;
		}

		// Compare the mark position to the match position, if the mark
		// exists and mark is set in reg_buf.
		اگر (pos != تہی && pos->lnum > 0)
		{
		    colnr_T pos_ستون = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->ستون == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->ستون;

		    نتیجہ = (pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_ستون == (colnr_T)(rex.داخلہ - rex.سطر)
				    ? t->state->c == NFA_MARK
				    : (pos_ستون < (colnr_T)(rex.داخلہ - rex.سطر)
					? t->state->c == NFA_MARK_GT
					: t->state->c == NFA_MARK_LT))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? t->state->c == NFA_MARK_GT
				    : t->state->c == NFA_MARK_LT));
		    اگر (نتیجہ)
		    {
			جوڑو_یہاں = TRUE;
			جوڑو_state = t->state->برآمد;
		    }
		}
		توڑو;
	      }

	    حالت NFA_CURSOR:
		نتیجہ = (rex.reg_win != تہی
			&& (rex.lnum + rex.reg_firstlnum
						 == rex.reg_win->w_cursor.lnum)
			&& ((colnr_T)(rex.داخلہ - rex.سطر)
						== rex.reg_win->w_cursor.ستون));
		اگر (نتیجہ)
		{
		    جوڑو_یہاں = TRUE;
		    جوڑو_state = t->state->برآمد;
		}
		توڑو;

	    حالت NFA_VISUAL:
		نتیجہ = reg_مطابقت_visual();
		اگر (نتیجہ)
		{
		    جوڑو_یہاں = TRUE;
		    جوڑو_state = t->state->برآمد;
		}
		توڑو;

	    حالت NFA_MOPEN1:
	    حالت NFA_MOPEN2:
	    حالت NFA_MOPEN3:
	    حالت NFA_MOPEN4:
	    حالت NFA_MOPEN5:
	    حالت NFA_MOPEN6:
	    حالت NFA_MOPEN7:
	    حالت NFA_MOPEN8:
	    حالت NFA_MOPEN9:
#اگر_تعریف_شدہ FEAT_SYN_HL
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
#اگر_ختم
	    حالت NFA_NOPEN:
	    حالت NFA_ZSTART:
		// These states are only added to be able to bail out when
		// they are added again, nothing is to be done.
		توڑو;

	    طےشدہ:	// regular character
	      {
		صحیح c = t->state->c;

#اگر_تعریف_شدہ DEBUG
		اگر (c < 0)
		    siemsg("Negative state char: %ld", (طویل)c);
#اگر_ختم
		نتیجہ = (c == curc);

		اگر (!نتیجہ && rex.reg_ic)
		    نتیجہ = MB_CASEFOLD(c) == MB_CASEFOLD(curc);
		// If rex.reg_icombine is not set only skip over the character
		// itself.  When it is set skip over composing characters.
		اگر (نتیجہ && enc_utf8 && !rex.reg_icombine)
		    clen = utf_ptr2len(rex.داخلہ);
		جوڑو_STATE_IF_مطابقت(t->state);
		توڑو;
	      }

	    } // switch (t->state->c)

	    اگر (جوڑو_state != تہی)
	    {
		nfa_pim_T *pim;
		nfa_pim_T pim_copy;

		اگر (t->pim.نتیجہ == NFA_PIM_غیر_مستعمل)
		    pim = تہی;
		ورنہ
		    pim = &t->pim;

		// Handle the postponed invisible match if the match might end
		// without advancing and before the end of the line.
		اگر (pim != تہی && (clen == 0 || مطابقت_follows(جوڑو_state, 0)))
		{
		    اگر (pim->نتیجہ == NFA_PIM_TODO)
		    {
#اگر_تعریف_شدہ ENABLE_LOG
			fprintf(log_fd, "\n");
			fprintf(log_fd, "==================================\n");
			fprintf(log_fd, "Postponed recursive nfa_regmatch()\n");
			fprintf(log_fd, "\n");
#اگر_ختم
			نتیجہ = recursive_regmatch(pim->state, pim,
				    prog, submatch, m, &listids, &listids_len);
			pim->نتیجہ = نتیجہ ? NFA_PIM_مطابقت : NFA_PIM_NOMATCH;
			// for \@! and \@<! it is a match when the result is
			// FALSE
			اگر (نتیجہ != (pim->state->c == NFA_آغاز_INVISIBLE_NEG
			     || pim->state->c == NFA_آغاز_INVISIBLE_NEG_پہلا
			     || pim->state->c
					   == NFA_آغاز_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_آغاز_INVISIBLE_BEFORE_NEG_پہلا))
			{
			    // Copy submatch info from the recursive call
			    copy_sub_off(&pim->subs.norm, &m->norm);
#اگر_تعریف_شدہ FEAT_SYN_HL
			    اگر (rex.nfa_has_zsubexpr)
				copy_sub_off(&pim->subs.synt, &m->synt);
#اگر_ختم
			}
		    }
		    ورنہ
		    {
			نتیجہ = (pim->نتیجہ == NFA_PIM_مطابقت);
#اگر_تعریف_شدہ ENABLE_LOG
			fprintf(log_fd, "\n");
			fprintf(log_fd, "Using previous recursive nfa_regmatch() result, result == %d\n", pim->نتیجہ);
			fprintf(log_fd, "MATCH = %s\n", نتیجہ == TRUE ? "OK" : "FALSE");
			fprintf(log_fd, "\n");
#اگر_ختم
		    }

		    // for \@! and \@<! it is a match when result is FALSE
		    اگر (نتیجہ != (pim->state->c == NFA_آغاز_INVISIBLE_NEG
			     || pim->state->c == NFA_آغاز_INVISIBLE_NEG_پہلا
			     || pim->state->c
					   == NFA_آغاز_INVISIBLE_BEFORE_NEG
			     || pim->state->c
				     == NFA_آغاز_INVISIBLE_BEFORE_NEG_پہلا))
		    {
			// Copy submatch info from the recursive call
			copy_sub_off(&t->subs.norm, &pim->subs.norm);
#اگر_تعریف_شدہ FEAT_SYN_HL
			اگر (rex.nfa_has_zsubexpr)
			    copy_sub_off(&t->subs.synt, &pim->subs.synt);
#اگر_ختم
		    }
		    ورنہ
			// look-behind match failed, don't add the state
			جاری;

		    // Postponed invisible match was handled, don't add it to
		    // following states.
		    pim = تہی;
		}

		// If "pim" points into l->t it will become invalid when
		// adding the state causes the list to be reallocated.  Make a
		// local copy to avoid that.
		اگر (pim == &t->pim)
		{
		    copy_pim(&pim_copy, pim);
		    pim = &pim_copy;
		}

		اگر (جوڑو_یہاں)
		    r = addstate_یہاں(thislist, جوڑو_state, &t->subs,
								pim, &listidx);
		ورنہ
		{
		    r = addstate(nextlist, جوڑو_state, &t->subs, pim, جوڑو_off, 0);
		    اگر (جوڑو_گنتی > 0)
			nextlist->t[nextlist->n - 1].گنتی = جوڑو_گنتی;
		}
		اگر (r == تہی)
		{
		    nfa_مطابقت = NFA_TOO_EXPENSIVE;
		    جاؤ theend;
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
	اگر (nfa_مطابقت == FALSE
		&& ((toplevel
			&& rex.lnum == 0
			&& clen != 0
			&& (rex.reg_maxcol == 0
			  || (colnr_T)(rex.داخلہ - rex.سطر) < rex.reg_maxcol))
		    || (nfa_endp != تہی
			&& (REG_MULTI
			    ? (rex.lnum < nfa_endp->se_u.pos.lnum
			       || (rex.lnum == nfa_endp->se_u.pos.lnum
				   && (صحیح)(rex.داخلہ - rex.سطر)
						    < nfa_endp->se_u.pos.ستون))
			    : rex.داخلہ < nfa_endp->se_u.ptr))))
	{
#اگر_تعریف_شدہ ENABLE_LOG
	    fprintf(log_fd, "(---) STARTSTATE\n");
#اگر_ختم
	    // Inline optimized code for addstate() if we know the state is
	    // the first MOPEN.
	    اگر (toplevel)
	    {
		صحیح جوڑو = TRUE;
		صحیح c;

		اگر (prog->regstart != NUL && clen != 0)
		{
		    اگر (nextlist->n == 0)
		    {
			colnr_T ستون = (colnr_T)(rex.داخلہ - rex.سطر) + clen;

			// Nextlist is empty, we can skip ahead to the
			// character that must appear at the start.
			اگر (skip_مقصد_آغاز(prog->regstart, &ستون) == FAIL)
			    توڑو;
#اگر_تعریف_شدہ ENABLE_LOG
			fprintf(log_fd, "  Skipping ahead %d bytes to regstart\n",
				ستون - ((colnr_T)(rex.داخلہ - rex.سطر) + clen));
#اگر_ختم
			rex.داخلہ = rex.سطر + ستون - clen;
		    }
		    ورنہ
		    {
			// Checking if the required start character matches is
			// cheaper than adding a state that won't match.
			c = PTR2CHAR(rex.داخلہ + clen);
			اگر (c != prog->regstart && (!rex.reg_ic
			     || MB_CASEFOLD(c) != MB_CASEFOLD(prog->regstart)))
			{
#اگر_تعریف_شدہ ENABLE_LOG
			    fprintf(log_fd, "  Skipping start state, regstart does not match\n");
#اگر_ختم
			    جوڑو = FALSE;
			}
		    }
		}

		اگر (جوڑو)
		{
		    اگر (REG_MULTI)
		    {
			m->norm.list.multi[0].آغاز_ستون =
					 (colnr_T)(rex.داخلہ - rex.سطر) + clen;
			m->norm.orig_آغاز_ستون =
					       m->norm.list.multi[0].آغاز_ستون;
		    }
		    ورنہ
			m->norm.list.سطر[0].آغاز = rex.داخلہ + clen;
		    اگر (addstate(nextlist, آغاز->برآمد, m, تہی, clen, 0) == تہی)
		    {
			nfa_مطابقت = NFA_TOO_EXPENSIVE;
			جاؤ theend;
		    }
		}
	    }
	    ورنہ
	    {
		char_u	    *save_سطر = rex.سطر;
		char_u	    *save_داخلہ = rex.داخلہ;
		linenr_T    save_lnum = rex.lnum;

		// At the end of a line the match can only start on the next
		// line, use that position instead of the line break.
		اگر (REG_MULTI && clen == 0 && nfa_endp != تہی
					 && rex.lnum < nfa_endp->se_u.pos.lnum)
		{
		    char_u  *اگلا_سطر = reg_getline(rex.lnum + 1);

		    اگر (اگلا_سطر != تہی)
		    {
			rex.سطر = اگلا_سطر;
			rex.داخلہ = rex.سطر;
			++rex.lnum;
		    }
		}

		r = addstate(nextlist, آغاز, m, تہی, clen, 0);

		rex.سطر = save_سطر;
		rex.داخلہ = save_داخلہ;
		rex.lnum = save_lnum;

		اگر (r == تہی)
		{
		    nfa_مطابقت = NFA_TOO_EXPENSIVE;
		    جاؤ theend;
		}
	    }
	}

#اگر_تعریف_شدہ ENABLE_LOG
	fprintf(log_fd, ">>> Thislist had %d states available: ", thislist->n);
	{
	    صحیح i;

	    برائے (i = 0; i < thislist->n; i++)
		fprintf(log_fd, "%d  ", مطلق_قدر(thislist->t[i].state->id));
	}
	fprintf(log_fd, "\n");
#اگر_ختم

nextchar:
	// Advance to the next character, or advance to the next line, or
	// finish.
	اگر (clen != 0)
	    rex.داخلہ += clen;
	ورنہ اگر (go_مقصد_nextline || (nfa_endp != تہی && REG_MULTI
					&& rex.lnum < nfa_endp->se_u.pos.lnum))
	    reg_nextline();
	ورنہ
	    توڑو;

	// Allow interrupting with CTRL-C.
	سطر_breakcheck();
	اگر (got_int)
	    توڑو;
#اگر_تعریف_شدہ FEAT_RELTIME
	اگر (nfa_did_وقت_برآمد())
	    توڑو;
#اگر_ختم
    }

#اگر_تعریف_شدہ ENABLE_LOG
    اگر (log_fd != stderr)
	fclose(log_fd);
    log_fd = تہی;
#اگر_ختم

theend:
    // Cache list buffers in prog for reuse, or free if prog already has
    // cached buffers (recursive call case).
    اگر (prog->listbuf[0] == تہی && list[0].t != تہی && list[1].t != تہی)
    {
	prog->listbuf[0] = list[0].t;
	prog->listbuf[1] = list[1].t;
    }
    ورنہ
    {
	vim_free(list[0].t);
	vim_free(list[1].t);
    }
    vim_free(listids);
#تعریف_منسوخ جوڑو_STATE_IF_مطابقت
#اگر_تعریف_شدہ NFA_REGEXP_DEBUG_LOG
    fclose(debug);
#اگر_ختم

    واپس nfa_مطابقت;
}

/*
 * Try match of "prog" with at rex.line["col"].
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    ساکن طویل
nfa_regtry(
    nfa_regprog_T   *prog,
    colnr_T	    ستون,
    صحیح		    *timed_برآمد غیر_مستعمل)	// flag set on timeout or NULL
{
    صحیح		i;
    regsubs_T	subs, m;
    nfa_state_T	*آغاز = prog->آغاز;
    صحیح		نتیجہ;
#اگر_تعریف_شدہ ENABLE_LOG
    FILE	*f;
#اگر_ختم

    rex.داخلہ = rex.سطر + ستون;
#اگر_تعریف_شدہ FEAT_RELTIME
    nfa_timed_برآمد = timed_برآمد;
#اگر_ختم

#اگر_تعریف_شدہ ENABLE_LOG
    f = fopen(NFA_REGEXP_تسلسل_LOG, "a");
    اگر (f != تہی)
    {
	fprintf(f, "\n\n\t=======================================================\n");
# ifdef DEBUG
	fprintf(f, "\tRegexp is \"%s\"\n", nfa_regengine.اظہاریہ);
# endif
	fprintf(f, "\tInput text is \"%s\" \n", rex.داخلہ);
	fprintf(f, "\t=======================================================\n\n");
	nfa_print_state(f, آغاز);
	fprintf(f, "\n\n");
	fclose(f);
    }
    ورنہ
	emsg("Could not open temporary log file for writing");
#اگر_ختم

    clear_sub(&subs.norm);
    clear_sub(&m.norm);
#اگر_تعریف_شدہ FEAT_SYN_HL
    clear_sub(&subs.synt);
    clear_sub(&m.synt);
#اگر_ختم

    نتیجہ = nfa_regmatch(prog, آغاز, &subs, &m);
    اگر (نتیجہ == FALSE)
	واپس 0;
    ورنہ اگر (نتیجہ == NFA_TOO_EXPENSIVE)
	واپس نتیجہ;

    cleanup_subexpr();
    اگر (REG_MULTI)
    {
	برائے (i = 0; i < subs.norm.اندر_use; i++)
	{
	    rex.reg_startpos[i].lnum = subs.norm.list.multi[i].آغاز_lnum;
	    rex.reg_startpos[i].ستون = subs.norm.list.multi[i].آغاز_ستون;

	    rex.reg_endpos[i].lnum = subs.norm.list.multi[i].انتہا_lnum;
	    rex.reg_endpos[i].ستون = subs.norm.list.multi[i].انتہا_ستون;
	}
	اگر (rex.reg_mmatch != تہی)
	    rex.reg_mmatch->rmm_matchcol = subs.norm.orig_آغاز_ستون;

	اگر (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].ستون = ستون;
	}
	اگر (rex.reg_endpos[0].lnum < 0)
	{
	    // pattern has a \ze but it didn't match, use current end
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].ستون = (صحیح)(rex.داخلہ - rex.سطر);
	}
	ورنہ
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    ورنہ
    {
	برائے (i = 0; i < subs.norm.اندر_use; i++)
	{
	    rex.reg_startp[i] = subs.norm.list.سطر[i].آغاز;
	    rex.reg_endp[i] = subs.norm.list.سطر[i].انتہا;
	}

	اگر (rex.reg_startp[0] == تہی)
	    rex.reg_startp[0] = rex.سطر + ستون;
	اگر (rex.reg_endp[0] == تہی)
	    rex.reg_endp[0] = rex.داخلہ;
    }

#اگر_تعریف_شدہ FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_برآمد);
    re_extmatch_برآمد = تہی;

    اگر (prog->reghasz == REX_سیٹ)
    {
	cleanup_zsubexpr();
	re_extmatch_برآمد = make_extmatch();
	اگر (re_extmatch_برآمد == تہی)
	    واپس 0;
	// Loop over \z1, \z2, etc.  There is no \z0.
	برائے (i = 1; i < subs.synt.اندر_use; i++)
	{
	    اگر (REG_MULTI)
	    {
		ساخت multipos *mpos = &subs.synt.list.multi[i];

		// Only accept single line matches that are valid.
		اگر (mpos->آغاز_lnum >= 0
			&& mpos->آغاز_lnum == mpos->انتہا_lnum
			&& mpos->انتہا_ستون >= mpos->آغاز_ستون)
		    re_extmatch_برآمد->matches[i] =
			vim_strnsave(reg_getline(mpos->آغاز_lnum)
							    + mpos->آغاز_ستون,
					     mpos->انتہا_ستون - mpos->آغاز_ستون);
	    }
	    ورنہ
	    {
		ساخت linepos *lpos = &subs.synt.list.سطر[i];

		اگر (lpos->آغاز != تہی && lpos->انتہا != تہی)
		    re_extmatch_برآمد->matches[i] =
			    vim_strnsave(lpos->آغاز, lpos->انتہا - lpos->آغاز);
	    }
	}
    }
#اگر_ختم

    واپس 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 *
 * Returns <= 0 for failure, number of lines contained in the match otherwise.
 */
    ساکن طویل
nfa_regexec_both(
    char_u	*سطر,
    colnr_T	startcol,	// column to start looking for match
    صحیح		*timed_برآمد)	// flag set on timeout or NULL
{
    nfa_regprog_T   *prog;
    طویل	    retval = 0L;
    صحیح		    i;
    colnr_T	    ستون = startcol;

    اگر (REG_MULTI)
    {
	prog = (nfa_regprog_T *)rex.reg_mmatch->regprog;
	سطر = reg_getline((linenr_T)0);    // relative to the cursor
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    ورنہ
    {
	prog = (nfa_regprog_T *)rex.reg_مطابقت->regprog;
	rex.reg_startp = rex.reg_مطابقت->startp;
	rex.reg_endp = rex.reg_مطابقت->endp;
    }

    // Be paranoid...
    اگر (prog == تہی || سطر == تہی)
    {
	iemsg(e_null_دلیل);
	جاؤ theend;
    }

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    اگر (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    ورنہ اگر (prog->regflags & RF_NOICASE)
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
#اگر_تعریف_شدہ DEBUG
    nfa_regengine.اظہاریہ = prog->نمونہ;
#اگر_ختم

    اگر (prog->reganch && ستون > 0)
	واپس 0L;

    rex.need_clear_subexpr = TRUE;
#اگر_تعریف_شدہ FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    اگر (prog->reghasz == REX_سیٹ)
    {
	rex.nfa_has_zsubexpr = TRUE;
	rex.need_clear_zsubexpr = TRUE;
    }
    ورنہ
    {
	rex.nfa_has_zsubexpr = FALSE;
	rex.need_clear_zsubexpr = FALSE;
    }
#اگر_ختم

    اگر (prog->regstart != NUL)
    {
	// Skip ahead until a character we know the match must start with.
	// When there is none there is no match.
	اگر (skip_مقصد_آغاز(prog->regstart, &ستون) == FAIL)
	    واپس 0L;

	// If match_text is set it contains the full text that must match.
	// Nothing else to try. Doesn't handle combining chars well.
	اگر (prog->مطابقت_متن != تہی && *prog->مطابقت_متن != NUL && !rex.reg_icombine)
	{
	    retval = ڈھونڈو_مطابقت_متن(&ستون, prog->regstart, prog->مطابقت_متن);
	    اگر (REG_MULTI)
		rex.reg_mmatch->rmm_matchcol = ستون;
	    ورنہ
		rex.reg_مطابقت->rm_matchcol = ستون;
	    واپس retval;
	}
    }

    // If the start column is past the maximum column: no need to try.
    اگر (rex.reg_maxcol > 0 && ستون >= rex.reg_maxcol)
	جاؤ theend;

    // Set the "nstate" used by nfa_regcomp() to zero to trigger an error when
    // it's accidentally used during execution.
    nstate = 0;
    برائے (i = 0; i < prog->nstate; ++i)
    {
	prog->state[i].id = i;
	prog->state[i].lastlist[0] = 0;
	prog->state[i].lastlist[1] = 0;
    }

    retval = nfa_regtry(prog, ستون, timed_برآمد);

#اگر_تعریف_شدہ DEBUG
    nfa_regengine.اظہاریہ = تہی;
#اگر_ختم

theend:
    اگر (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs and
	// \ze are used.
	اگر (REG_MULTI)
	{
	    lpos_T *آغاز = &rex.reg_mmatch->startpos[0];
	    lpos_T *انتہا = &rex.reg_mmatch->endpos[0];

	    اگر (انتہا->lnum < آغاز->lnum
			|| (انتہا->lnum == آغاز->lnum && انتہا->ستون < آغاز->ستون))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];
	}
	ورنہ
	{
	    اگر (rex.reg_مطابقت->endp[0] < rex.reg_مطابقت->startp[0])
		rex.reg_مطابقت->endp[0] = rex.reg_مطابقت->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_مطابقت->rm_matchcol = ستون;
	}
    }

    واپس retval;
}

/*
 * Compile a regular expression into internal code for the NFA matcher.
 * Returns the program in allocated space.  Returns NULL for an error.
 */
    ساکن regprog_T *
nfa_regcomp(char_u *اظہاریہ, صحیح re_flags)
{
    nfa_regprog_T	*prog = تہی;
    size_t		prog_سائز;
    صحیح			*postfix;

    اگر (اظہاریہ == تہی)
	واپس تہی;

#اگر_تعریف_شدہ DEBUG
    nfa_regengine.اظہاریہ = اظہاریہ;
#اگر_ختم
    nfa_re_flags = re_flags;

    init_class_tab();

    اگر (nfa_regcomp_آغاز(اظہاریہ, re_flags) == FAIL)
	واپس تہی;

    // Build postfix form of the regexp. Needed to build the NFA
    // (and count its size).
    postfix = re2post();
    اگر (postfix == تہی)
	جاؤ fail;	    // Cascaded (syntax?) error

    /*
     * In order to build the NFA, we parse the input regexp twice:
     * 1. first pass to count size (so we can allocate space)
     * 2. second to emit code
     */
#اگر_تعریف_شدہ ENABLE_LOG
    {
	FILE *f = fopen(NFA_REGEXP_تسلسل_LOG, "a");

	اگر (f != تہی)
	{
	    fprintf(f, "\n*****************************\n\n\n\n\tCompiling regexp \"%s\"... hold on !\n", اظہاریہ);
	    fclose(f);
	}
    }
#اگر_ختم

    /*
     * PASS 1
     * Count number of NFA states in "nstate". Do not build the NFA.
     */
    post2nfa(postfix, post_ptr, TRUE);

    // allocate the regprog with space for the compiled regexp
    prog_سائز = offsetof(nfa_regprog_T, state) + حجم(nfa_state_T) * nstate;
    prog = alloc(prog_سائز);
    اگر (prog == تہی)
	جاؤ fail;
    state_ptr = prog->state;
    prog->re_اندر_use = FALSE;
    prog->listbuf[0] = تہی;
    prog->listbuf[1] = تہی;

    /*
     * PASS 2
     * Build the NFA
     */
    prog->آغاز = post2nfa(postfix, post_ptr, FALSE);
    اگر (prog->آغاز == تہی)
	جاؤ fail;

    prog->regflags = regflags;
    prog->engine = &nfa_regengine;
    prog->nstate = nstate;
    prog->has_zend = rex.nfa_has_zend;
    prog->has_backref = rex.nfa_has_backref;
    prog->nsubexp = regnpar;

    nfa_postprocess(prog);

    prog->reganch = nfa_get_reganch(prog->آغاز, 0);
    prog->regstart = nfa_get_regstart(prog->آغاز, 0);
    prog->مطابقت_متن = nfa_get_مطابقت_متن(prog->آغاز);

#اگر_تعریف_شدہ ENABLE_LOG
    nfa_postfix_dump(اظہاریہ, OK);
    nfa_dump(prog);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    prog->reghasz = re_has_z;
#اگر_ختم
    prog->نمونہ = vim_strsave(اظہاریہ);
#اگر_تعریف_شدہ DEBUG
    nfa_regengine.اظہاریہ = تہی;
#اگر_ختم

برآمد:
    // The postfix list and fragment stack are reused by the next compilation
    // (and freed in free_regexp_stuff()), but drop ones that grew large for a
    // big pattern to avoid holding much memory.
    اگر (post_آغاز_len > NFA_POSTFIX_KEEP)
    {
	VIM_CLEAR(post_آغاز);
	post_آغاز_len = 0;
    }
    post_ptr = post_انتہا = تہی;
    اگر (nfa_ڈھیر_len > NFA_ڈھیر_KEEP)
    {
	VIM_CLEAR(nfa_ڈھیر);
	nfa_ڈھیر_len = 0;
    }
    state_ptr = تہی;
    واپس (regprog_T *)prog;

fail:
    VIM_CLEAR(prog);
#اگر_تعریف_شدہ ENABLE_LOG
    nfa_postfix_dump(اظہاریہ, FAIL);
#اگر_ختم
#اگر_تعریف_شدہ DEBUG
    nfa_regengine.اظہاریہ = تہی;
#اگر_ختم
    جاؤ برآمد;
}

/*
 * Free a compiled regexp program, returned by nfa_regcomp().
 */
    ساکن خالی
nfa_regfree(regprog_T *prog)
{
    اگر (prog == تہی)
	واپس;

    vim_free(((nfa_regprog_T *)prog)->مطابقت_متن);
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
    ساکن صحیح
nfa_regexec_nl(
    regmatch_T	*rmp,
    char_u	*سطر,	// string to match against
    colnr_T	ستون,	// column to start looking for match
    صحیح		سطر_lbr)
{
    rex.reg_مطابقت = rmp;
    rex.reg_mmatch = تہی;
    rex.reg_maxline = 0;
    rex.reg_سطر_lbr = سطر_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = تہی;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;
    واپس nfa_regexec_both(سطر, ستون, تہی);
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
    ساکن طویل
nfa_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	ستون,		// column to start looking for match
    صحیح		*timed_برآمد)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    واپس nfa_regexec_both(تہی, ستون, timed_برآمد);
}

#اگر_تعریف_شدہ DEBUG
# undef ENABLE_LOG
#اگر_ختم
