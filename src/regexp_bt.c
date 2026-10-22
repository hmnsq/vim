/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * Backtracking regular expression implementation.
 *
 * This file is included in "regexp.c".
 *
 * NOTICE:
 *
 * This is NOT the original regular expression code as written by Henry
 * Spencer.  This code has been modified specifically for use with the VIM
 * editor, and should not be used separately from Vim.  If you want a good
 * regular expression library, get the original code.  The copyright notice
 * that follows is from the original.
 *
 * END NOTICE
 *
 *	Copyright (c) 1986 by University of Toronto.
 *	Written by Henry Spencer.  Not derived from licensed software.
 *
 *	Permission is granted to anyone to use this software for any
 *	purpose on any computer system, and to redistribute it freely,
 *	subject to the following restrictions:
 *
 *	1. The author is not responsible for the consequences of use of
 *		this software, no matter how awful, even if they arise
 *		from defects in it.
 *
 *	2. The origin of this software must not be misrepresented, either
 *		by explicit claim or by omission.
 *
 *	3. Altered versions must be plainly marked as such, and must not
 *		be misrepresented as being the original software.
 *
 * Beware that some of this code is subtly aware of the way operator
 * precedence is structured in regular expressions.  Serious changes in
 * regular-expression syntax might require a total rethink.
 *
 * Changes have been made by Tony Andrews, Olaf 'Rhialto' Seibert, Robert
 * Webb, Ciaran McCreesh and Bram Moolenaar.
 * Named character class support added by Walter Briscoe (1998 Jul 01)
 */

/*
 * The "internal use only" fields in regexp.h are present to pass info from
 * compile to execute that permits the execute phase to run lots faster on
 * simple cases.  They are:
 *
 * regstart	char that must begin a match; NUL if none obvious; Can be a
 *		multi-byte character.
 * reganch	is the match anchored (at beginning-of-line only)?
 * regmust	string (pointer into program) that match must include, or NULL
 * regmlen	length of regmust string
 * regflags	RF_ values or'ed together
 *
 * Regstart and reganch permit very fast decisions on suitable starting points
 * for a match, cutting down the work a lot.  Regmust permits fast rejection
 * of lines that cannot possibly match.  The regmust tests are costly enough
 * that vim_regcomp() supplies a regmust only if the r.e. contains something
 * potentially expensive (at present, the only such thing detected is * or +
 * at the start of the r.e., which can involve a lot of backup).  Regmlen is
 * supplied because the test in vim_regexec() needs it and vim_regcomp() is
 * computing it anyway.
 */

/*
 * Structure for regexp "program".  This is essentially a linear encoding
 * of a nondeterministic finite-state machine (aka syntax charts or
 * "railroad normal form" in parsing technology).  Each node is an opcode
 * plus a "next" pointer, possibly plus an operand.  "Next" pointers of
 * all nodes except BRANCH and BRACES_COMPLEX implement concatenation; a "next"
 * pointer with a BRANCH on both ends of it is connecting two alternatives.
 * (Here we have one of the subtle syntax dependencies:	an individual BRANCH
 * (as opposed to a collection of them) is never concatenated with anything
 * because of operator precedence).  The "next" pointer of a BRACES_COMPLEX
 * node points to the node after the stuff to be repeated.
 * The operand of some types of node is a literal string; for others, it is a
 * node leading into a sub-FSM.  In particular, the operand of a BRANCH node
 * is the first node of the branch.
 * (NB this is *not* a tree structure: the tail of the branch connects to the
 * thing following the set of BRANCHes.)
 *
 * pattern	is coded like:
 *
 *			  +-----------------+
 *			  |		    V
 * <aa>\|<bb>	BRANCH <aa> BRANCH <bb> --> END
 *		     |	    ^	 |	    ^
 *		     +------+	 +----------+
 *
 *
 *		       +------------------+
 *		       V		  |
 * <aa>*	BRANCH BRANCH <aa> --> BACK BRANCH --> NOTHING --> END
 *		     |	    |		    ^			   ^
 *		     |	    +---------------+			   |
 *		     +---------------------------------------------+
 *
 *
 *		       +----------------------+
 *		       V		      |
 * <aa>\+	BRANCH <aa> --> BRANCH --> BACK  BRANCH --> NOTHING --> END
 *		     |		     |		 ^			^
 *		     |		     +-----------+			|
 *		     +--------------------------------------------------+
 *
 *
 *					+-------------------------+
 *					V			  |
 * <aa>\{}	BRANCH BRACE_LIMITS --> BRACE_COMPLEX <aa> --> BACK  END
 *		     |				    |		     ^
 *		     |				    +----------------+
 *		     +-----------------------------------------------+
 *
 *
 * <aa>\@!<bb>	BRANCH NOMATCH <aa> --> END  <bb> --> END
 *		     |	     |		      ^       ^
 *		     |	     +----------------+       |
 *		     +--------------------------------+
 *
 *						      +---------+
 *						      |		V
 * \z[abc]	BRANCH BRANCH  a  BRANCH  b  BRANCH  c	BRANCH	NOTHING --> END
 *		     |	    |	       |	  |	^		    ^
 *		     |	    |	       |	  +-----+		    |
 *		     |	    |	       +----------------+		    |
 *		     |	    +---------------------------+		    |
 *		     +------------------------------------------------------+
 *
 * They all start with a BRANCH for "\|" alternatives, even when there is only
 * one alternative.
 */

/*
 * The opcodes are:
 */

// definition	number		   opnd?    meaning
#నిర్వచించు చివర_3		0	//	End of program or NOMATCH operand.
#నిర్వచించు BOL		1	//	Match "" at beginning of line.
#నిర్వచించు EOL		2	//	Match "" at end of line.
#నిర్వచించు BRANCH		3	// node Match this alternative, or the
				//	next...
#నిర్వచించు వెనుక_2		4	//	Match "", "next" ptr points backward.
#నిర్వచించు EXACTLY		5	// str	Match this string.
#నిర్వచించు NOTHING		6	//	Match empty string.
#నిర్వచించు నక్షత్రం		7	// node Match this (simple) thing 0 or more
				//	times.
#నిర్వచించు PLUS		8	// node Match this (simple) thing 1 or more
				//	times.
#నిర్వచించు సరిపోలిక_2		9	// node match the operand zero-width
#నిర్వచించు NOMATCH		10	// node check for no match with operand
#నిర్వచించు BEHIND		11	// node look behind for a match with operand
#నిర్వచించు NOBEHIND	12	// node look behind for no match with operand
#నిర్వచించు SUBPAT		13	// node match the operand here
#నిర్వచించు BRACE_SIMPLE	14	// node Match this (simple) thing between m and
				//	n times (\{m,n\}).
#నిర్వచించు BOW		15	//	Match "" after [^a-zA-Z0-9_]
#నిర్వచించు EOW		16	//	Match "" at    [^a-zA-Z0-9_]
#నిర్వచించు BRACE_LIMITS	17	// nr nr  define the min & max for BRACE_SIMPLE
				//	and BRACE_COMPLEX.
#నిర్వచించు NEWL		18	//	Match line-break
#నిర్వచించు BHPOS		19	//	End position for BEHIND or NOBEHIND


// character classes: 20-48 normal, 50-78 include a line-break
#నిర్వచించు కూడు_NL		30
#నిర్వచించు మొదటి_NL	ANY + కూడు_NL
#నిర్వచించు ANY		20	//	Match any one character.
#నిర్వచించు ANYOF		21	// str	Match any character in this string.
#నిర్వచించు ANYBUT		22	// str	Match any character not in this
				//	string.
#నిర్వచించు IDENT		23	//	Match identifier char
#నిర్వచించు SIDENT		24	//	Match identifier char but no digit
#నిర్వచించు KWORD		25	//	Match keyword char
#నిర్వచించు SKWORD		26	//	Match word char but no digit
#నిర్వచించు FNAME		27	//	Match file name char
#నిర్వచించు SFNAME		28	//	Match file name char but no digit
#నిర్వచించు PRINT		29	//	Match printable char
#నిర్వచించు SPRINT		30	//	Match printable char but no digit
#నిర్వచించు WHITE		31	//	Match whitespace char
#నిర్వచించు NWHITE		32	//	Match non-whitespace char
#నిర్వచించు DIGIT		33	//	Match digit char
#నిర్వచించు NDIGIT		34	//	Match non-digit char
#నిర్వచించు HEX		35	//	Match hex char
#నిర్వచించు NHEX		36	//	Match non-hex char
#నిర్వచించు OCTAL		37	//	Match octal char
#నిర్వచించు NOCTAL		38	//	Match non-octal char
#నిర్వచించు మాట_2		39	//	Match word char
#నిర్వచించు NWORD		40	//	Match non-word char
#నిర్వచించు తల_2		41	//	Match head char
#నిర్వచించు NHEAD		42	//	Match non-head char
#నిర్వచించు ALPHA		43	//	Match alpha char
#నిర్వచించు NALPHA		44	//	Match non-alpha char
#నిర్వచించు క్రింది_2		45	//	Match lowercase char
#నిర్వచించు NLOWER		46	//	Match non-lowercase char
#నిర్వచించు పై		47	//	Match uppercase char
#నిర్వచించు NUPPER		48	//	Match non-uppercase char
#నిర్వచించు చివరి_NL		NUPPER + కూడు_NL
#నిర్వచించు WITH_NL(op)	((op) >= మొదటి_NL && (op) <= చివరి_NL)

#నిర్వచించు MOPEN		80  // -89	 Mark this point in input as start of
				//	 \( subexpr.  MOPEN + 0 marks start of
				//	 match.
#నిర్వచించు MCLOSE		90  // -99	 Analogous to MOPEN.  MCLOSE + 0 marks
				//	 end of match.
#నిర్వచించు BACKREF		100 // -109 node Match same string again \1-\9

#నిర్వచితమైతే FEAT_SYN_HL
# define ZOPEN		110 // -119	 Mark this point in input as start of
				//	 \z( subexpr.
# define ZCLOSE		120 // -129	 Analogous to ZOPEN.
# define ZREF		130 // -139 node Match external submatch \z1-\z9
#అయితే_ముగింపు

#నిర్వచించు BRACE_COMPLEX	140 // -149 node Match nodes between m & n times

#నిర్వచించు NOPEN		150	//	Mark this point in input as start of
				//	\%( subexpr.
#నిర్వచించు NCLOSE		151	//	Analogous to NOPEN.

#నిర్వచించు MULTIBYTECODE	200	// mbc	Match one multi-byte character
#నిర్వచించు RE_BOF		201	//	Match "" at beginning of file.
#నిర్వచించు RE_EOF		202	//	Match "" at end of file.
#నిర్వచించు CURSOR		203	//	Match location of cursor.

#నిర్వచించు RE_LNUM		204	// nr cmp  Match line number
#నిర్వచించు RE_నిలువు		205	// nr cmp  Match column number
#నిర్వచించు RE_VCOL		206	// nr cmp  Match virtual column number

#నిర్వచించు RE_MARK		207	// mark cmp  Match mark position
#నిర్వచించు RE_VISUAL	208	//	Match Visual area
#నిర్వచించు RE_COMPOSING	209	// any composing characters

/*
 * Flags to be passed up and down.
 */
#నిర్వచించు HASWIDTH	0x1	// Known never to match null string.
#నిర్వచించు SIMPLE		0x2	// Simple enough to be STAR/PLUS operand.
#నిర్వచించు SPSTART		0x4	// Starts with * or +.
#నిర్వచించు HASNL		0x8	// Contains some \n.
#నిర్వచించు HASLOOKBH	0x10	// Contains "\@<=" or "\@<!".
#నిర్వచించు WORST		0	// Worst case.

స్థిర పూర్ణాంకం	అంకె_complex_braces; // Complex \{...} count
స్థిర char_u	*regcode;	// Code-emit pointer, or JUST_CALC_SIZE
స్థిర దీర్ఘ	regsize;	// Code size.
స్థిర పూర్ణాంకం	reg_toolong;	// TRUE when offset out of range
స్థిర పూర్ణాంకం	bt_reg_parse_లోతు;	// nesting depth in reg()
స్థిర char_u	had_endbrace[NSUBEXP];	// flags, TRUE if end of () found
స్థిర దీర్ఘ	brace_min[10];	// Minimums for complex brace repeats
స్థిర దీర్ఘ	brace_max[10];	// Maximums for complex brace repeats
స్థిర పూర్ణాంకం	brace_లెక్క[10]; // Current counts for complex brace repeats
స్థిర పూర్ణాంకం	one_exactly = FALSE;	// only do one char for EXACTLY

// When making changes to classchars also change nfa_classcodes.
స్థిర char_u	*classchars = (char_u *)".iIkKfFpPsSdDxXoOwWhHaAlLuU";
స్థిర స్థిరరాశి పూర్ణాంకం classcodes[] = {
    ANY, IDENT, SIDENT, KWORD, SKWORD,
    FNAME, SFNAME, PRINT, SPRINT,
    WHITE, NWHITE, DIGIT, NDIGIT,
    HEX, NHEX, OCTAL, NOCTAL,
    మాట_2, NWORD, తల_2, NHEAD,
    ALPHA, NALPHA, క్రింది_2, NLOWER,
    పై, NUPPER
};

/*
 * Search between "start" (the first char of the range) and "end" (the closing
 * "]") and try to recognize a character class in expanded form, for example
 * [0-9].  On success, return the atom to be emitted, on failure 0.
 */
    స్థిర పూర్ణాంకం
bt_recognize_char_class(char_u *ప్రారంభం, char_u *చివర)
{
    పూర్ణాంకం		newl;
    పూర్ణాంకం		config = get_char_class_బిట్లు(ప్రారంభం, చివర, &newl);

    // A newline would need the ADD_NL variant, don't bother with it here.
    ఒకవేళ (config < 0 || newl)
	తిరిగి 0;

	// The old engine has no case-insensitive class opcode, so [a-z] and [A-Z]
	// are left as collections.  The classes below are case-independent.
    ఎంపిక (config)
    {
	స్థితి CLASS_o9:
	    తిరిగి DIGIT;
	స్థితి CLASS_not | CLASS_o9:
	    తిరిగి NDIGIT;
	స్థితి CLASS_af | CLASS_AF | CLASS_o9:
	    తిరిగి HEX;
	స్థితి CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    తిరిగి NHEX;
	స్థితి CLASS_o7:
	    తిరిగి OCTAL;
	స్థితి CLASS_not | CLASS_o7:
	    తిరిగి NOCTAL;
	స్థితి CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    తిరిగి మాట_2;
	స్థితి CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    తిరిగి NWORD;
	స్థితి CLASS_az | CLASS_AZ | CLASS_underscore:
	    తిరిగి తల_2;
	స్థితి CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    తిరిగి NHEAD;
	స్థితి CLASS_az | CLASS_AZ:
	    తిరిగి ALPHA;
	స్థితి CLASS_not | CLASS_az | CLASS_AZ:
	    తిరిగి NALPHA;
    }
    తిరిగి 0;
}

/*
 * When regcode is set to this value, code is not emitted and size is computed
 * instead.
 */
#నిర్వచించు JUST_CALC_కొలత	((char_u *) -1)

// Values for rs_state in regitem_T.
రకనిర్వచనం గణన regstate_E
{
    RS_NOPEN = 0	// NOPEN and NCLOSE
    , RS_MOPEN		// MOPEN + [0-9]
    , RS_MCLOSE		// MCLOSE + [0-9]
#నిర్వచితమైతే FEAT_SYN_HL
    , RS_ZOPEN		// ZOPEN + [0-9]
    , RS_ZCLOSE		// ZCLOSE + [0-9]
#అయితే_ముగింపు
    , RS_BRANCH		// BRANCH
    , RS_BRCPLX_మరిన్ని	// BRACE_COMPLEX and trying one more match
    , RS_BRCPLX_LONG	// BRACE_COMPLEX and trying longest match
    , RS_BRCPLX_SHORT	// BRACE_COMPLEX and trying shortest match
    , RS_NOMATCH	// NOMATCH
    , RS_BEHIND1	// BEHIND / NOBEHIND matching rest
    , RS_BEHIND2	// BEHIND / NOBEHIND matching behind part
    , RS_నక్షత్రం_LONG	// STAR/PLUS/BRACE_SIMPLE longest match
    , RS_నక్షత్రం_SHORT	// STAR/PLUS/BRACE_SIMPLE shortest match
} regstate_T;

/*
 * Structure used to save the current input state, when it needs to be
 * restored after trying a match.  Used by reg_save() and reg_restore().
 * Also stores the length of "backpos".
 */
రకనిర్వచనం నిర్మాణం
{
    సంఘం
    {
	char_u	*ptr;	// rex.input pointer, for single-line regexp
	lpos_T	pos;	// rex.input pos, for multi-line regexp
    } rs_u;
    పూర్ణాంకం		rs_len;
} regsave_T;

// struct to save start/end pointer/position in for \(\)
రకనిర్వచనం నిర్మాణం
{
    సంఘం
    {
	char_u	*ptr;
	lpos_T	pos;
    } se_u;
} save_se_T;

// used for BEHIND and NOBEHIND matching
రకనిర్వచనం నిర్మాణం regbehind_S
{
    regsave_T	save_after;
    regsave_T	save_behind;
    పూర్ణాంకం		save_need_clear_subexpr;
    save_se_T   save_ప్రారంభం[NSUBEXP];
    save_se_T   save_చివర[NSUBEXP];
} regbehind_T;

/*
 * When there are alternatives a regstate_T is put on the regstack to remember
 * what we are doing.
 * Before it may be another type of item, depending on rs_state, to remember
 * more things.
 */
రకనిర్వచనం నిర్మాణం regitem_S
{
    regstate_T	rs_state;	// what we are doing, one of RS_ above
    పొట్టి	rs_no;		// submatch nr or BEHIND/NOBEHIND
    char_u	*rs_పరిశీలన;	// current node in program
    సంఘం
    {
	save_se_T  sesave;
	regsave_T  regsave;
    } rs_un;			// room for saving rex.input
} regitem_T;


// used for STAR, PLUS and BRACE_SIMPLE matching
రకనిర్వచనం నిర్మాణం regstar_S
{
    పూర్ణాంకం		nextb;		// next byte
    పూర్ణాంకం		nextb_ic;	// next byte reverse case
    దీర్ఘ	లెక్క;
    దీర్ఘ	minval;
    దీర్ఘ	maxval;
} regstar_T;

// used to store input position when a BACK was encountered, so that we now if
// we made any progress since the last time.
రకనిర్వచనం నిర్మాణం backpos_S
{
    char_u	*bp_పరిశీలన;	// "scan" where BACK was encountered
    regsave_T	bp_pos;		// last input position
} backpos_T;

/*
 * "regstack" and "backpos" are used by regmatch().  They are kept over calls
 * to avoid invoking malloc() and free() often.
 * "regstack" is a stack with regitem_T items, sometimes preceded by regstar_T
 * or regbehind_T.
 * "backpos_T" is a table with backpos_T for BACK
 */
స్థిర garray_T	regstack = {0, 0, 0, 0, ఖాళీ};
స్థిర garray_T	backpos = {0, 0, 0, 0, ఖాళీ};

స్థిర regsave_T behind_pos;

/*
 * Both for regstack and backpos tables we use the following strategy of
 * allocation (to reduce malloc/free calls):
 * - Initial size is fairly small.
 * - When needed, the tables are grown bigger (8 times at first, double after
 *   that).
 * - After executing the match we free the memory only if the array has grown.
 *   Thus the memory is kept allocated when it's at the initial size.
 * This makes it fast while not keeping a lot of memory allocated.
 * A three times speed increase was observed when using many simple patterns.
 */
#నిర్వచించు REGSTACK_INITIAL	2048
#నిర్వచించు BACKPOS_INITIAL		64

/*
 * Opcode notes:
 *
 * BRANCH	The set of branches constituting a single choice are hooked
 *		together with their "next" pointers, since precedence prevents
 *		anything being concatenated to any individual branch.  The
 *		"next" pointer of the last BRANCH in a choice points to the
 *		thing following the whole choice.  This is also where the
 *		final "next" pointer of each individual branch points; each
 *		branch starts with the operand node of a BRANCH node.
 *
 * BACK		Normal "next" pointers all implicitly point forward; BACK
 *		exists to make loop structures possible.
 *
 * STAR,PLUS	'=', and complex '*' and '+', are implemented as circular
 *		BRANCH structures using BACK.  Simple cases (one character
 *		per match) are implemented with STAR and PLUS for speed
 *		and to minimize recursive plunges.
 *
 * BRACE_LIMITS	This is always followed by a BRACE_SIMPLE or BRACE_COMPLEX
 *		node, and defines the min and max limits to be used for that
 *		node.
 *
 * MOPEN,MCLOSE	...are numbered at compile time.
 * ZOPEN,ZCLOSE	...ditto
 */

/*
 * A node is one char of opcode followed by two chars of "next" pointer.
 * "Next" pointers are stored as two 8-bit bytes, high order first.  The
 * value is a positive offset from the opcode of the node containing it.
 * An operand, if any, simply follows the node.  (Note that much of the
 * code generation knows about this implicit relationship.)
 *
 * Using two bytes for the "next" pointer is vast overkill for most things,
 * but allows patterns to get big without disasters.
 */
#నిర్వచించు OP(p)		((పూర్ణాంకం)*(p))
#నిర్వచించు తదుపరి_2(p)		(((*((p) + 1) & 0377) << 8) + (*((p) + 2) & 0377))
#నిర్వచించు నిర్వాహ్యం(p)	((p) + 3)
// Obtain an operand that was stored as four bytes, MSB first.
#నిర్వచించు నిర్వాహ్యం_MIN(p)	(((దీర్ఘ)(p)[3] << 24) + ((దీర్ఘ)(p)[4] << 16) \
			+ ((దీర్ఘ)(p)[5] << 8) + (దీర్ఘ)(p)[6])
// Obtain a second operand stored as four bytes.
#నిర్వచించు నిర్వాహ్యం_MAX(p)	నిర్వాహ్యం_MIN((p) + 4)
// Obtain a second single-byte operand stored after a four bytes operand.
#నిర్వచించు నిర్వాహ్యం_CMP(p)	(p)[7]

స్థిర char_u *reg(పూర్ణాంకం paren, పూర్ణాంకం *flagp);

#నిర్వచితమైతే BT_REGEXP_DUMP
స్థిర శూన్య	regdump(char_u *, bt_regprog_T *);
#అయితే_ముగింపు

స్థిర పూర్ణాంకం	re_అంకె_cmp(long_u val, char_u *పరిశీలన);

#నిర్వచితమైతే DEBUG
స్థిర char_u	*regprop(char_u *);

స్థిర పూర్ణాంకం	regnarrate = 0;
#అయితే_ముగింపు


/*
 * Setup to parse the regexp.  Used once to get the length and once to do it.
 */
    స్థిర శూన్య
regcomp_ప్రారంభం(
    char_u	*సమాసం,
    పూర్ణాంకం		re_flags)	    // see vim_regcomp()
{
    initchr(సమాసం);
    ఒకవేళ (re_flags & RE_MAGIC)
	reg_magic = MAGIC_ON;
    లేకపోతే
	reg_magic = MAGIC_OFF;
    reg_string = (re_flags & RE_STRING);
    reg_strict = (re_flags & RE_STRICT);
    get_cpo_flags();

    అంకె_complex_braces = 0;
    regnpar = 1;
    CLEAR_క్షేత్రం(had_endbrace);
#నిర్వచితమైతే FEAT_SYN_HL
    regnzpar = 1;
    re_has_z = 0;
#అయితే_ముగింపు
    regsize = 0L;
    reg_toolong = FALSE;
    bt_reg_parse_లోతు = 0;
    regflags = 0;
#ఒకవేళ defined(FEAT_SYN_HL)
    had_eol = FALSE;
#అయితే_ముగింపు
}

/*
 * Return TRUE if MULTIBYTECODE should be used instead of EXACTLY for
 * character "c".
 */
    స్థిర పూర్ణాంకం
use_multibytecode(పూర్ణాంకం c)
{
    తిరిగి has_mbyte && (*mb_char2len)(c) > 1
		     && (re_multi_type(peekchr()) != NOT_MULTI
			     || (enc_utf8 && utf_iscomposing(c)));
}

/*
 * Emit (if appropriate) a byte of code
 */
    స్థిర శూన్య
regc(పూర్ణాంకం b)
{
    ఒకవేళ (regcode == JUST_CALC_కొలత)
	regsize++;
    లేకపోతే
	*regcode++ = b;
}

/*
 * Emit (if appropriate) a multi-byte character of code
 */
    స్థిర శూన్య
regmbc(పూర్ణాంకం c)
{
    ఒకవేళ (!has_mbyte && c > 0xff)
	తిరిగి;
    ఒకవేళ (regcode == JUST_CALC_కొలత)
	regsize += (*mb_char2len)(c);
    లేకపోతే
	regcode += (*mb_char2bytes)(c, regcode);
}


/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * NOTE: When changing this function, also change nfa_emit_equi_class()
 */
    స్థిర శూన్య
reg_equi_class(పూర్ణాంకం c)
{
    ఒకవేళ (enc_utf8 || STRCMP(p_enc, "latin1") == 0
					 || STRCMP(p_enc, "iso-8859-15") == 0)
    {
	ఎంపిక (c)
	{
	    // Do not use '\300' style, it results in a negative number.
	    స్థితి 'A': స్థితి 0xc0: స్థితి 0xc1: స్థితి 0xc2: స్థితి 0xc3: స్థితి 0xc4:
	    స్థితి 0xc5: స్థితి 0x100: స్థితి 0x102: స్థితి 0x104: స్థితి 0x1cd:
	    స్థితి 0x1de: స్థితి 0x1e0: స్థితి 0x1fa: స్థితి 0x202: స్థితి 0x226:
	    స్థితి 0x23a: స్థితి 0x1e00: స్థితి 0x1ea0: స్థితి 0x1ea2: స్థితి 0x1ea4:
	    స్థితి 0x1ea6: స్థితి 0x1ea8: స్థితి 0x1eaa: స్థితి 0x1eac: స్థితి 0x1eae:
	    స్థితి 0x1eb0: స్థితి 0x1eb2: స్థితి 0x1eb4: స్థితి 0x1eb6:
		      regmbc('A'); regmbc(0xc0); regmbc(0xc1); regmbc(0xc2);
		      regmbc(0xc3); regmbc(0xc4); regmbc(0xc5);
		      regmbc(0x100); regmbc(0x102); regmbc(0x104);
		      regmbc(0x1cd); regmbc(0x1de); regmbc(0x1e0);
		      regmbc(0x1fa); regmbc(0x202); regmbc(0x226);
		      regmbc(0x23a); regmbc(0x1e00); regmbc(0x1ea0);
		      regmbc(0x1ea2); regmbc(0x1ea4); regmbc(0x1ea6);
		      regmbc(0x1ea8); regmbc(0x1eaa); regmbc(0x1eac);
		      regmbc(0x1eae); regmbc(0x1eb0); regmbc(0x1eb2);
		      regmbc(0x1eb4); regmbc(0x1eb6);
		      తిరిగి;
	    స్థితి 'B': స్థితి 0x181: స్థితి 0x243: స్థితి 0x1e02:
	    స్థితి 0x1e04: స్థితి 0x1e06:
		      regmbc('B');
		      regmbc(0x181); regmbc(0x243); regmbc(0x1e02);
		      regmbc(0x1e04); regmbc(0x1e06);
		      తిరిగి;
	    స్థితి 'C': స్థితి 0xc7:
	    స్థితి 0x106: స్థితి 0x108: స్థితి 0x10a: స్థితి 0x10c: స్థితి 0x187:
	    స్థితి 0x23b: స్థితి 0x1e08: స్థితి 0xa792:
		      regmbc('C'); regmbc(0xc7);
		      regmbc(0x106); regmbc(0x108); regmbc(0x10a);
		      regmbc(0x10c); regmbc(0x187); regmbc(0x23b);
		      regmbc(0x1e08); regmbc(0xa792);
		      తిరిగి;
	    స్థితి 'D': స్థితి 0x10e: స్థితి 0x110: స్థితి 0x18a:
	    స్థితి 0x1e0a: స్థితి 0x1e0c: స్థితి 0x1e0e: స్థితి 0x1e10:
	    స్థితి 0x1e12:
		      regmbc('D'); regmbc(0x10e); regmbc(0x110);
		      regmbc(0x18a); regmbc(0x1e0a); regmbc(0x1e0c);
		      regmbc(0x1e0e); regmbc(0x1e10); regmbc(0x1e12);
		      తిరిగి;
	    స్థితి 'E': స్థితి 0xc8: స్థితి 0xc9: స్థితి 0xca: స్థితి 0xcb:
	    స్థితి 0x112: స్థితి 0x114: స్థితి 0x116: స్థితి 0x118: స్థితి 0x11a:
	    స్థితి 0x204: స్థితి 0x206: స్థితి 0x228: స్థితి 0x246: స్థితి 0x1e14:
	    స్థితి 0x1e16: స్థితి 0x1e18: స్థితి 0x1e1a: స్థితి 0x1e1c:
	    స్థితి 0x1eb8: స్థితి 0x1eba: స్థితి 0x1ebc: స్థితి 0x1ebe:
	    స్థితి 0x1ec0: స్థితి 0x1ec2: స్థితి 0x1ec4: స్థితి 0x1ec6:
		      regmbc('E'); regmbc(0xc8); regmbc(0xc9);
		      regmbc(0xca); regmbc(0xcb); regmbc(0x112);
		      regmbc(0x114); regmbc(0x116); regmbc(0x118);
		      regmbc(0x11a); regmbc(0x204); regmbc(0x206);
		      regmbc(0x228); regmbc(0x246); regmbc(0x1e14);
		      regmbc(0x1e16); regmbc(0x1e18); regmbc(0x1e1a);
		      regmbc(0x1e1c); regmbc(0x1eb8); regmbc(0x1eba);
		      regmbc(0x1ebc); regmbc(0x1ebe); regmbc(0x1ec0);
		      regmbc(0x1ec2); regmbc(0x1ec4); regmbc(0x1ec6);
		      తిరిగి;
	    స్థితి 'F': స్థితి 0x191: స్థితి 0x1e1e: స్థితి 0xa798:
		      regmbc('F'); regmbc(0x191); regmbc(0x1e1e);
		      regmbc(0xa798);
		      తిరిగి;
	    స్థితి 'G': స్థితి 0x11c: స్థితి 0x11e: స్థితి 0x120:
	    స్థితి 0x122: స్థితి 0x193: స్థితి 0x1e4: స్థితి 0x1e6:
	    స్థితి 0x1f4: స్థితి 0x1e20: స్థితి 0xa7a0:
		      regmbc('G'); regmbc(0x11c); regmbc(0x11e);
		      regmbc(0x120); regmbc(0x122); regmbc(0x193);
		      regmbc(0x1e4); regmbc(0x1e6); regmbc(0x1f4);
		      regmbc(0x1e20); regmbc(0xa7a0);
		      తిరిగి;
	    స్థితి 'H': స్థితి 0x124: స్థితి 0x126: స్థితి 0x21e:
	    స్థితి 0x1e22: స్థితి 0x1e24: స్థితి 0x1e26:
	    స్థితి 0x1e28: స్థితి 0x1e2a: స్థితి 0x2c67:
		      regmbc('H'); regmbc(0x124); regmbc(0x126);
		      regmbc(0x21e); regmbc(0x1e22); regmbc(0x1e24);
		      regmbc(0x1e26); regmbc(0x1e28); regmbc(0x1e2a);
		      regmbc(0x2c67);
		      తిరిగి;
	    స్థితి 'I': స్థితి 0xcc: స్థితి 0xcd: స్థితి 0xce: స్థితి 0xcf:
	    స్థితి 0x128: స్థితి 0x12a: స్థితి 0x12c: స్థితి 0x12e:
	    స్థితి 0x130: స్థితి 0x197: స్థితి 0x1cf: స్థితి 0x208:
	    స్థితి 0x20a: స్థితి 0x1e2c: స్థితి 0x1e2e: స్థితి 0x1ec8:
	    స్థితి 0x1eca:
		      regmbc('I'); regmbc(0xcc); regmbc(0xcd);
		      regmbc(0xce); regmbc(0xcf); regmbc(0x128);
		      regmbc(0x12a); regmbc(0x12c); regmbc(0x12e);
		      regmbc(0x130); regmbc(0x197); regmbc(0x1cf);
		      regmbc(0x208); regmbc(0x20a); regmbc(0x1e2c);
		      regmbc(0x1e2e); regmbc(0x1ec8); regmbc(0x1eca);
		      తిరిగి;
	    స్థితి 'J': స్థితి 0x134: స్థితి 0x248:
		      regmbc('J'); regmbc(0x134); regmbc(0x248);
		      తిరిగి;
	    స్థితి 'K': స్థితి 0x136: స్థితి 0x198: స్థితి 0x1e8: స్థితి 0x1e30:
	    స్థితి 0x1e32: స్థితి 0x1e34: స్థితి 0x2c69: స్థితి 0xa740:
		      regmbc('K'); regmbc(0x136); regmbc(0x198);
		      regmbc(0x1e8); regmbc(0x1e30); regmbc(0x1e32);
		      regmbc(0x1e34); regmbc(0x2c69); regmbc(0xa740);
		      తిరిగి;
	    స్థితి 'L': స్థితి 0x139: స్థితి 0x13b: స్థితి 0x13d: స్థితి 0x13f:
	    స్థితి 0x141: స్థితి 0x23d: స్థితి 0x1e36: స్థితి 0x1e38:
	    స్థితి 0x1e3a: స్థితి 0x1e3c: స్థితి 0x2c60:
		      regmbc('L'); regmbc(0x139); regmbc(0x13b);
		      regmbc(0x13d); regmbc(0x13f); regmbc(0x141);
		      regmbc(0x23d); regmbc(0x1e36); regmbc(0x1e38);
		      regmbc(0x1e3a); regmbc(0x1e3c); regmbc(0x2c60);
		      తిరిగి;
	    స్థితి 'M': స్థితి 0x1e3e: స్థితి 0x1e40: స్థితి 0x1e42:
		      regmbc('M'); regmbc(0x1e3e); regmbc(0x1e40);
		      regmbc(0x1e42);
		      తిరిగి;
	    స్థితి 'N': స్థితి 0xd1:
	    స్థితి 0x143: స్థితి 0x145: స్థితి 0x147: స్థితి 0x1f8:
	    స్థితి 0x1e44: స్థితి 0x1e46: స్థితి 0x1e48: స్థితి 0x1e4a:
	    స్థితి 0xa7a4:
		      regmbc('N'); regmbc(0xd1);
		      regmbc(0x143); regmbc(0x145); regmbc(0x147);
		      regmbc(0x1f8); regmbc(0x1e44); regmbc(0x1e46);
		      regmbc(0x1e48); regmbc(0x1e4a); regmbc(0xa7a4);
		      తిరిగి;
	    స్థితి 'O': స్థితి 0xd2: స్థితి 0xd3: స్థితి 0xd4: స్థితి 0xd5: స్థితి 0xd6:
	    స్థితి 0xd8: స్థితి 0x14c: స్థితి 0x14e: స్థితి 0x150: స్థితి 0x19f:
	    స్థితి 0x1a0: స్థితి 0x1d1: స్థితి 0x1ea: స్థితి 0x1ec: స్థితి 0x1fe:
	    స్థితి 0x20c: స్థితి 0x20e: స్థితి 0x22a: స్థితి 0x22c: స్థితి 0x22e:
	    స్థితి 0x230: స్థితి 0x1e4c: స్థితి 0x1e4e: స్థితి 0x1e50: స్థితి 0x1e52:
	    స్థితి 0x1ecc: స్థితి 0x1ece: స్థితి 0x1ed0: స్థితి 0x1ed2: స్థితి 0x1ed4:
	    స్థితి 0x1ed6: స్థితి 0x1ed8: స్థితి 0x1eda: స్థితి 0x1edc: స్థితి 0x1ede:
	    స్థితి 0x1ee0: స్థితి 0x1ee2:
		      regmbc('O'); regmbc(0xd2); regmbc(0xd3); regmbc(0xd4);
		      regmbc(0xd5); regmbc(0xd6); regmbc(0xd8);
		      regmbc(0x14c); regmbc(0x14e); regmbc(0x150);
		      regmbc(0x19f); regmbc(0x1a0); regmbc(0x1d1);
		      regmbc(0x1ea); regmbc(0x1ec); regmbc(0x1fe);
		      regmbc(0x20c); regmbc(0x20e); regmbc(0x22a);
		      regmbc(0x22c); regmbc(0x22e); regmbc(0x230);
		      regmbc(0x1e4c); regmbc(0x1e4e); regmbc(0x1e50);
		      regmbc(0x1e52); regmbc(0x1ecc); regmbc(0x1ece);
		      regmbc(0x1ed0); regmbc(0x1ed2); regmbc(0x1ed4);
		      regmbc(0x1ed6); regmbc(0x1ed8); regmbc(0x1eda);
		      regmbc(0x1edc); regmbc(0x1ede); regmbc(0x1ee0);
		      regmbc(0x1ee2);
		      తిరిగి;
	    స్థితి 'P': స్థితి 0x1a4: స్థితి 0x1e54: స్థితి 0x1e56: స్థితి 0x2c63:
		      regmbc('P'); regmbc(0x1a4); regmbc(0x1e54);
		      regmbc(0x1e56); regmbc(0x2c63);
		      తిరిగి;
	    స్థితి 'Q': స్థితి 0x24a:
		      regmbc('Q'); regmbc(0x24a);
		      తిరిగి;
	    స్థితి 'R': స్థితి 0x154: స్థితి 0x156: స్థితి 0x158: స్థితి 0x210:
	    స్థితి 0x212: స్థితి 0x24c: స్థితి 0x1e58: స్థితి 0x1e5a:
	    స్థితి 0x1e5c: స్థితి 0x1e5e: స్థితి 0x2c64: స్థితి 0xa7a6:
		      regmbc('R'); regmbc(0x154); regmbc(0x156);
		      regmbc(0x210); regmbc(0x212); regmbc(0x158);
		      regmbc(0x24c); regmbc(0x1e58); regmbc(0x1e5a);
		      regmbc(0x1e5c); regmbc(0x1e5e); regmbc(0x2c64);
		      regmbc(0xa7a6);
		      తిరిగి;
	    స్థితి 'S': స్థితి 0x15a: స్థితి 0x15c: స్థితి 0x15e: స్థితి 0x160:
	    స్థితి 0x218: స్థితి 0x1e60: స్థితి 0x1e62: స్థితి 0x1e64:
	    స్థితి 0x1e66: స్థితి 0x1e68: స్థితి 0x2c7e: స్థితి 0xa7a8:
		      regmbc('S'); regmbc(0x15a); regmbc(0x15c);
		      regmbc(0x15e); regmbc(0x160); regmbc(0x218);
		      regmbc(0x1e60); regmbc(0x1e62); regmbc(0x1e64);
		      regmbc(0x1e66); regmbc(0x1e68); regmbc(0x2c7e);
			  regmbc(0xa7a8);
		      తిరిగి;
	    స్థితి 'T': స్థితి 0x162: స్థితి 0x164: స్థితి 0x166: స్థితి 0x1ac:
	    స్థితి 0x1ae: స్థితి 0x21a: స్థితి 0x23e: స్థితి 0x1e6a: స్థితి 0x1e6c:
	    స్థితి 0x1e6e: స్థితి 0x1e70:
		      regmbc('T'); regmbc(0x162); regmbc(0x164);
		      regmbc(0x166); regmbc(0x1ac); regmbc(0x23e);
		      regmbc(0x1ae); regmbc(0x21a); regmbc(0x1e6a);
		      regmbc(0x1e6c); regmbc(0x1e6e); regmbc(0x1e70);
		      తిరిగి;
	    స్థితి 'U': స్థితి 0xd9: స్థితి 0xda: స్థితి 0xdb: స్థితి 0xdc:
	    స్థితి 0x168: స్థితి 0x16a: స్థితి 0x16c: స్థితి 0x16e:
	    స్థితి 0x170: స్థితి 0x172: స్థితి 0x1af: స్థితి 0x1d3:
	    స్థితి 0x1d5: స్థితి 0x1d7: స్థితి 0x1d9: స్థితి 0x1db:
	    స్థితి 0x214: స్థితి 0x216: స్థితి 0x244: స్థితి 0x1e72:
	    స్థితి 0x1e74: స్థితి 0x1e76: స్థితి 0x1e78: స్థితి 0x1e7a:
	    స్థితి 0x1ee4: స్థితి 0x1ee6: స్థితి 0x1ee8: స్థితి 0x1eea:
	    స్థితి 0x1eec: స్థితి 0x1eee: స్థితి 0x1ef0:
		      regmbc('U'); regmbc(0xd9); regmbc(0xda);
		      regmbc(0xdb); regmbc(0xdc); regmbc(0x168);
		      regmbc(0x16a); regmbc(0x16c); regmbc(0x16e);
		      regmbc(0x170); regmbc(0x172); regmbc(0x1af);
		      regmbc(0x1d3); regmbc(0x1d5); regmbc(0x1d7);
		      regmbc(0x1d9); regmbc(0x1db); regmbc(0x214);
		      regmbc(0x216); regmbc(0x244); regmbc(0x1e72);
		      regmbc(0x1e74); regmbc(0x1e76); regmbc(0x1e78);
		      regmbc(0x1e7a); regmbc(0x1ee4); regmbc(0x1ee6);
		      regmbc(0x1ee8); regmbc(0x1eea); regmbc(0x1eec);
		      regmbc(0x1eee); regmbc(0x1ef0);
		      తిరిగి;
	    స్థితి 'V': స్థితి 0x1b2: స్థితి 0x1e7c: స్థితి 0x1e7e:
		      regmbc('V'); regmbc(0x1b2); regmbc(0x1e7c);
		      regmbc(0x1e7e);
		      తిరిగి;
	    స్థితి 'W': స్థితి 0x174: స్థితి 0x1e80: స్థితి 0x1e82:
	    స్థితి 0x1e84: స్థితి 0x1e86: స్థితి 0x1e88:
		      regmbc('W'); regmbc(0x174); regmbc(0x1e80);
		      regmbc(0x1e82); regmbc(0x1e84); regmbc(0x1e86);
		      regmbc(0x1e88);
		      తిరిగి;
	    స్థితి 'X': స్థితి 0x1e8a: స్థితి 0x1e8c:
		      regmbc('X'); regmbc(0x1e8a); regmbc(0x1e8c);
		      తిరిగి;
	    స్థితి 'Y': స్థితి 0xdd:
	    స్థితి 0x176: స్థితి 0x178: స్థితి 0x1b3: స్థితి 0x232: స్థితి 0x24e:
	    స్థితి 0x1e8e: స్థితి 0x1ef2: స్థితి 0x1ef6: స్థితి 0x1ef4: స్థితి 0x1ef8:
		      regmbc('Y'); regmbc(0xdd); regmbc(0x176);
		      regmbc(0x178); regmbc(0x1b3); regmbc(0x232);
		      regmbc(0x24e); regmbc(0x1e8e); regmbc(0x1ef2);
		      regmbc(0x1ef4); regmbc(0x1ef6); regmbc(0x1ef8);
		      తిరిగి;
	    స్థితి 'Z': స్థితి 0x179: స్థితి 0x17b: స్థితి 0x17d: స్థితి 0x1b5:
	    స్థితి 0x1e90: స్థితి 0x1e92: స్థితి 0x1e94: స్థితి 0x2c6b:
		      regmbc('Z'); regmbc(0x179); regmbc(0x17b);
		      regmbc(0x17d); regmbc(0x1b5); regmbc(0x1e90);
		      regmbc(0x1e92); regmbc(0x1e94); regmbc(0x2c6b);
		      తిరిగి;
	    స్థితి 'a': స్థితి 0xe0: స్థితి 0xe1: స్థితి 0xe2:
	    స్థితి 0xe3: స్థితి 0xe4: స్థితి 0xe5: స్థితి 0x101: స్థితి 0x103:
	    స్థితి 0x105: స్థితి 0x1ce: స్థితి 0x1df: స్థితి 0x1e1: స్థితి 0x1fb:
	    స్థితి 0x201: స్థితి 0x203: స్థితి 0x227: స్థితి 0x1d8f: స్థితి 0x1e01:
	    స్థితి 0x1e9a: స్థితి 0x1ea1: స్థితి 0x1ea3: స్థితి 0x1ea5:
	    స్థితి 0x1ea7: స్థితి 0x1ea9: స్థితి 0x1eab: స్థితి 0x1ead:
	    స్థితి 0x1eaf: స్థితి 0x1eb1: స్థితి 0x1eb3: స్థితి 0x1eb5:
	    స్థితి 0x1eb7: స్థితి 0x2c65:
		      regmbc('a'); regmbc(0xe0); regmbc(0xe1);
		      regmbc(0xe2); regmbc(0xe3); regmbc(0xe4);
		      regmbc(0xe5); regmbc(0x101); regmbc(0x103);
		      regmbc(0x105); regmbc(0x1ce); regmbc(0x1df);
		      regmbc(0x1e1); regmbc(0x1fb); regmbc(0x201);
		      regmbc(0x203); regmbc(0x227); regmbc(0x1d8f);
		      regmbc(0x1e01); regmbc(0x1e9a); regmbc(0x1ea1);
		      regmbc(0x1ea3); regmbc(0x1ea5); regmbc(0x1ea7);
		      regmbc(0x1ea9); regmbc(0x1eab); regmbc(0x1ead);
		      regmbc(0x1eaf); regmbc(0x1eb1); regmbc(0x1eb3);
		      regmbc(0x1eb5); regmbc(0x1eb7); regmbc(0x2c65);
		      తిరిగి;
	    స్థితి 'b': స్థితి 0x180: స్థితి 0x253: స్థితి 0x1d6c: స్థితి 0x1d80:
	    స్థితి 0x1e03: స్థితి 0x1e05: స్థితి 0x1e07:
		      regmbc('b');
		      regmbc(0x180); regmbc(0x253); regmbc(0x1d6c);
		      regmbc(0x1d80); regmbc(0x1e03); regmbc(0x1e05);
		      regmbc(0x1e07);
		      తిరిగి;
	    స్థితి 'c': స్థితి 0xe7:
	    స్థితి 0x107: స్థితి 0x109: స్థితి 0x10b: స్థితి 0x10d: స్థితి 0x188:
	    స్థితి 0x23c: స్థితి 0x1e09: స్థితి 0xa793: స్థితి 0xa794:
		      regmbc('c'); regmbc(0xe7); regmbc(0x107);
		      regmbc(0x109); regmbc(0x10b); regmbc(0x10d);
		      regmbc(0x188); regmbc(0x23c); regmbc(0x1e09);
		      regmbc(0xa793); regmbc(0xa794);
		      తిరిగి;
	    స్థితి 'd': స్థితి 0x10f: స్థితి 0x111: స్థితి 0x257: స్థితి 0x1d6d:
	    స్థితి 0x1d81: స్థితి 0x1d91: స్థితి 0x1e0b: స్థితి 0x1e0d:
	    స్థితి 0x1e0f: స్థితి 0x1e11: స్థితి 0x1e13:
		      regmbc('d'); regmbc(0x10f); regmbc(0x111);
		      regmbc(0x257); regmbc(0x1d6d); regmbc(0x1d81);
		      regmbc(0x1d91); regmbc(0x1e0b); regmbc(0x1e0d);
		      regmbc(0x1e0f); regmbc(0x1e11); regmbc(0x1e13);
		      తిరిగి;
	    స్థితి 'e': స్థితి 0xe8: స్థితి 0xe9: స్థితి 0xea: స్థితి 0xeb:
	    స్థితి 0x113: స్థితి 0x115: స్థితి 0x117: స్థితి 0x119:
	    స్థితి 0x11b: స్థితి 0x205: స్థితి 0x207: స్థితి 0x229:
	    స్థితి 0x247: స్థితి 0x1d92: స్థితి 0x1e15: స్థితి 0x1e17:
	    స్థితి 0x1e19: స్థితి 0x1e1b: స్థితి 0x1eb9: స్థితి 0x1ebb:
	    స్థితి 0x1e1d: స్థితి 0x1ebd: స్థితి 0x1ebf: స్థితి 0x1ec1:
	    స్థితి 0x1ec3: స్థితి 0x1ec5: స్థితి 0x1ec7:
		      regmbc('e'); regmbc(0xe8); regmbc(0xe9);
		      regmbc(0xea); regmbc(0xeb); regmbc(0x113);
		      regmbc(0x115); regmbc(0x117); regmbc(0x119);
		      regmbc(0x11b); regmbc(0x205); regmbc(0x207);
		      regmbc(0x229); regmbc(0x247); regmbc(0x1d92);
		      regmbc(0x1e15); regmbc(0x1e17); regmbc(0x1e19);
		      regmbc(0x1e1b); regmbc(0x1e1d); regmbc(0x1eb9);
		      regmbc(0x1ebb); regmbc(0x1ebd); regmbc(0x1ebf);
		      regmbc(0x1ec1); regmbc(0x1ec3); regmbc(0x1ec5);
		      regmbc(0x1ec7);
		      తిరిగి;
	    స్థితి 'f': స్థితి 0x192: స్థితి 0x1d6e: స్థితి 0x1d82:
	    స్థితి 0x1e1f: స్థితి 0xa799:
		     regmbc('f'); regmbc(0x192); regmbc(0x1d6e);
		     regmbc(0x1d82); regmbc(0x1e1f); regmbc(0xa799);
		     తిరిగి;
	    స్థితి 'g': స్థితి 0x11d: స్థితి 0x11f: స్థితి 0x121: స్థితి 0x123:
	    స్థితి 0x1e5: స్థితి 0x1e7: స్థితి 0x260: స్థితి 0x1f5: స్థితి 0x1d83:
	    స్థితి 0x1e21: స్థితి 0xa7a1:
		      regmbc('g'); regmbc(0x11d); regmbc(0x11f);
		      regmbc(0x121); regmbc(0x123); regmbc(0x1e5);
		      regmbc(0x1e7); regmbc(0x1f5); regmbc(0x260);
		      regmbc(0x1d83); regmbc(0x1e21); regmbc(0xa7a1);
		      తిరిగి;
	    స్థితి 'h': స్థితి 0x125: స్థితి 0x127: స్థితి 0x21f: స్థితి 0x1e23:
	    స్థితి 0x1e25: స్థితి 0x1e27: స్థితి 0x1e29: స్థితి 0x1e2b:
	    స్థితి 0x1e96: స్థితి 0x2c68: స్థితి 0xa795:
		      regmbc('h'); regmbc(0x125); regmbc(0x127);
		      regmbc(0x21f); regmbc(0x1e23); regmbc(0x1e25);
		      regmbc(0x1e27); regmbc(0x1e29); regmbc(0x1e2b);
		      regmbc(0x1e96); regmbc(0x2c68); regmbc(0xa795);
		      తిరిగి;
	    స్థితి 'i': స్థితి 0xec: స్థితి 0xed: స్థితి 0xee: స్థితి 0xef:
	    స్థితి 0x129: స్థితి 0x12b: స్థితి 0x12d: స్థితి 0x12f:
	    స్థితి 0x1d0: స్థితి 0x209: స్థితి 0x20b: స్థితి 0x268:
	    స్థితి 0x1d96: స్థితి 0x1e2d: స్థితి 0x1e2f: స్థితి 0x1ec9:
	    స్థితి 0x1ecb:
		      regmbc('i'); regmbc(0xec); regmbc(0xed);
		      regmbc(0xee); regmbc(0xef); regmbc(0x129);
		      regmbc(0x12b); regmbc(0x12d); regmbc(0x12f);
		      regmbc(0x1d0); regmbc(0x209); regmbc(0x20b);
		      regmbc(0x268); regmbc(0x1d96); regmbc(0x1e2d);
		      regmbc(0x1e2f); regmbc(0x1ec9); regmbc(0x1ecb);
		      తిరిగి;
	    స్థితి 'j': స్థితి 0x135: స్థితి 0x1f0: స్థితి 0x249:
		      regmbc('j'); regmbc(0x135); regmbc(0x1f0);
		      regmbc(0x249);
		      తిరిగి;
	    స్థితి 'k': స్థితి 0x137: స్థితి 0x199: స్థితి 0x1e9:
	    స్థితి 0x1d84: స్థితి 0x1e31: స్థితి 0x1e33: స్థితి 0x1e35:
	    స్థితి 0x2c6a: స్థితి 0xa741:
		      regmbc('k'); regmbc(0x137); regmbc(0x199);
		      regmbc(0x1e9); regmbc(0x1d84); regmbc(0x1e31);
		      regmbc(0x1e33); regmbc(0x1e35); regmbc(0x2c6a);
		      regmbc(0xa741);
		      తిరిగి;
	    స్థితి 'l': స్థితి 0x13a: స్థితి 0x13c: స్థితి 0x13e:
	    స్థితి 0x140: స్థితి 0x142: స్థితి 0x19a: స్థితి 0x1e37:
	    స్థితి 0x1e39: స్థితి 0x1e3b: స్థితి 0x1e3d: స్థితి 0x2c61:
		      regmbc('l'); regmbc(0x13a); regmbc(0x13c);
		      regmbc(0x13e); regmbc(0x140); regmbc(0x142);
		      regmbc(0x19a); regmbc(0x1e37); regmbc(0x1e39);
		      regmbc(0x1e3b); regmbc(0x1e3d); regmbc(0x2c61);
		      తిరిగి;
	    స్థితి 'm': స్థితి 0x1d6f: స్థితి 0x1e3f: స్థితి 0x1e41: స్థితి 0x1e43:
		      regmbc('m'); regmbc(0x1d6f); regmbc(0x1e3f);
		      regmbc(0x1e41); regmbc(0x1e43);
		      తిరిగి;
	    స్థితి 'n': స్థితి 0xf1: స్థితి 0x144: స్థితి 0x146: స్థితి 0x148:
	    స్థితి 0x149: స్థితి 0x1f9: స్థితి 0x1d70: స్థితి 0x1d87:
	    స్థితి 0x1e45: స్థితి 0x1e47: స్థితి 0x1e49: స్థితి 0x1e4b:
	    స్థితి 0xa7a5:
		      regmbc('n'); regmbc(0xf1); regmbc(0x144);
		      regmbc(0x146); regmbc(0x148); regmbc(0x149);
		      regmbc(0x1f9); regmbc(0x1d70); regmbc(0x1d87);
		      regmbc(0x1e45); regmbc(0x1e47); regmbc(0x1e49);
		      regmbc(0x1e4b); regmbc(0xa7a5);
		      తిరిగి;
	    స్థితి 'o': స్థితి 0xf2: స్థితి 0xf3: స్థితి 0xf4: స్థితి 0xf5:
	    స్థితి 0xf6: స్థితి 0xf8: స్థితి 0x14d: స్థితి 0x14f: స్థితి 0x151:
	    స్థితి 0x1a1: స్థితి 0x1d2: స్థితి 0x1eb: స్థితి 0x1ed: స్థితి 0x1ff:
	    స్థితి 0x20d: స్థితి 0x20f: స్థితి 0x22b: స్థితి 0x22d: స్థితి 0x22f:
	    స్థితి 0x231: స్థితి 0x275: స్థితి 0x1e4d: స్థితి 0x1e4f:
	    స్థితి 0x1e51: స్థితి 0x1e53: స్థితి 0x1ecd: స్థితి 0x1ecf:
	    స్థితి 0x1ed1: స్థితి 0x1ed3: స్థితి 0x1ed5: స్థితి 0x1ed7:
	    స్థితి 0x1ed9: స్థితి 0x1edb: స్థితి 0x1edd: స్థితి 0x1edf:
	    స్థితి 0x1ee1: స్థితి 0x1ee3:
		      regmbc('o'); regmbc(0xf2); regmbc(0xf3);
		      regmbc(0xf4); regmbc(0xf5); regmbc(0xf6);
		      regmbc(0xf8); regmbc(0x14d); regmbc(0x14f);
		      regmbc(0x151); regmbc(0x1a1); regmbc(0x1d2);
		      regmbc(0x1eb); regmbc(0x1ed); regmbc(0x1ff);
		      regmbc(0x20d); regmbc(0x20f); regmbc(0x22b);
		      regmbc(0x22d); regmbc(0x22f); regmbc(0x231);
		      regmbc(0x275); regmbc(0x1e4d); regmbc(0x1e4f);
		      regmbc(0x1e51); regmbc(0x1e53); regmbc(0x1ecd);
		      regmbc(0x1ecf); regmbc(0x1ed1); regmbc(0x1ed3);
		      regmbc(0x1ed5); regmbc(0x1ed7); regmbc(0x1ed9);
		      regmbc(0x1edb); regmbc(0x1edd); regmbc(0x1edf);
		      regmbc(0x1ee1); regmbc(0x1ee3);
		      తిరిగి;
	    స్థితి 'p': స్థితి 0x1a5: స్థితి 0x1d71: స్థితి 0x1d88: స్థితి 0x1d7d:
	    స్థితి 0x1e55: స్థితి 0x1e57:
		      regmbc('p'); regmbc(0x1a5); regmbc(0x1d71);
		      regmbc(0x1d7d); regmbc(0x1d88); regmbc(0x1e55);
		      regmbc(0x1e57);
		      తిరిగి;
	    స్థితి 'q': స్థితి 0x24b: స్థితి 0x2a0:
		      regmbc('q'); regmbc(0x24b); regmbc(0x2a0);
		      తిరిగి;
	    స్థితి 'r': స్థితి 0x155: స్థితి 0x157: స్థితి 0x159: స్థితి 0x211:
	    స్థితి 0x213: స్థితి 0x24d: స్థితి 0x27d: స్థితి 0x1d72: స్థితి 0x1d73:
	    స్థితి 0x1d89: స్థితి 0x1e59: స్థితి 0x1e5b: స్థితి 0x1e5d: స్థితి 0x1e5f:
	    స్థితి 0xa7a7:
		      regmbc('r'); regmbc(0x155); regmbc(0x157);
		      regmbc(0x159); regmbc(0x211); regmbc(0x213);
		      regmbc(0x24d); regmbc(0x1d72); regmbc(0x1d73);
		      regmbc(0x1d89); regmbc(0x1e59); regmbc(0x27d);
		      regmbc(0x1e5b); regmbc(0x1e5d); regmbc(0x1e5f);
		      regmbc(0xa7a7);
		      తిరిగి;
	    స్థితి 's': స్థితి 0x15b: స్థితి 0x15d: స్థితి 0x15f: స్థితి 0x161:
	    స్థితి 0x1e61: స్థితి 0x219: స్థితి 0x23f: స్థితి 0x1d74: స్థితి 0x1d8a:
	    స్థితి 0x1e63: స్థితి 0x1e65: స్థితి 0x1e67: స్థితి 0x1e69: స్థితి 0xa7a9:
		      regmbc('s'); regmbc(0x15b); regmbc(0x15d);
		      regmbc(0x15f); regmbc(0x161); regmbc(0x23f);
		      regmbc(0x219); regmbc(0x1d74); regmbc(0x1d8a);
		      regmbc(0x1e61); regmbc(0x1e63); regmbc(0x1e65);
		      regmbc(0x1e67); regmbc(0x1e69); regmbc(0xa7a9);
		      తిరిగి;
	    స్థితి 't': స్థితి 0x163: స్థితి 0x165: స్థితి 0x167: స్థితి 0x1ab:
	    స్థితి 0x1ad: స్థితి 0x21b: స్థితి 0x288: స్థితి 0x1d75: స్థితి 0x1e6b:
	    స్థితి 0x1e6d: స్థితి 0x1e6f: స్థితి 0x1e71: స్థితి 0x1e97: స్థితి 0x2c66:
		      regmbc('t'); regmbc(0x163); regmbc(0x165);
		      regmbc(0x167); regmbc(0x1ab); regmbc(0x21b);
		      regmbc(0x1ad); regmbc(0x288); regmbc(0x1d75);
		      regmbc(0x1e6b); regmbc(0x1e6d); regmbc(0x1e6f);
		      regmbc(0x1e71); regmbc(0x1e97); regmbc(0x2c66);
		      తిరిగి;
	    స్థితి 'u': స్థితి 0xf9: స్థితి 0xfa: స్థితి 0xfb: స్థితి 0xfc:
	    స్థితి 0x169: స్థితి 0x16b: స్థితి 0x16d: స్థితి 0x16f:
	    స్థితి 0x171: స్థితి 0x173: స్థితి 0x1b0: స్థితి 0x1d4:
	    స్థితి 0x1d6: స్థితి 0x1d8: స్థితి 0x1da: స్థితి 0x1dc:
	    స్థితి 0x215: స్థితి 0x217: స్థితి 0x289: స్థితి 0x1e73:
	    స్థితి 0x1d7e: స్థితి 0x1d99: స్థితి 0x1e75: స్థితి 0x1e77:
	    స్థితి 0x1e79: స్థితి 0x1e7b: స్థితి 0x1ee5: స్థితి 0x1ee7:
	    స్థితి 0x1ee9: స్థితి 0x1eeb: స్థితి 0x1eed: స్థితి 0x1eef:
	    స్థితి 0x1ef1:
		      regmbc('u'); regmbc(0xf9); regmbc(0xfa);
		      regmbc(0xfb); regmbc(0xfc); regmbc(0x169);
		      regmbc(0x16b); regmbc(0x16d); regmbc(0x16f);
		      regmbc(0x171); regmbc(0x173); regmbc(0x1d6);
		      regmbc(0x1d8); regmbc(0x1da); regmbc(0x1dc);
		      regmbc(0x215); regmbc(0x217); regmbc(0x1b0);
		      regmbc(0x1d4); regmbc(0x289); regmbc(0x1d7e);
		      regmbc(0x1d99); regmbc(0x1e73); regmbc(0x1e75);
		      regmbc(0x1e77); regmbc(0x1e79); regmbc(0x1e7b);
		      regmbc(0x1ee5); regmbc(0x1ee7); regmbc(0x1ee9);
		      regmbc(0x1eeb); regmbc(0x1eed); regmbc(0x1eef);
		      regmbc(0x1ef1);
		      తిరిగి;
	    స్థితి 'v': స్థితి 0x28b: స్థితి 0x1d8c: స్థితి 0x1e7d: స్థితి 0x1e7f:
		      regmbc('v'); regmbc(0x28b); regmbc(0x1d8c);
		      regmbc(0x1e7d); regmbc(0x1e7f);
		      తిరిగి;
	    స్థితి 'w': స్థితి 0x175: స్థితి 0x1e81: స్థితి 0x1e83:
	    స్థితి 0x1e85: స్థితి 0x1e87: స్థితి 0x1e89: స్థితి 0x1e98:
		      regmbc('w'); regmbc(0x175); regmbc(0x1e81);
		      regmbc(0x1e83); regmbc(0x1e85); regmbc(0x1e87);
		      regmbc(0x1e89); regmbc(0x1e98);
		      తిరిగి;
	    స్థితి 'x': స్థితి 0x1e8b: స్థితి 0x1e8d:
		      regmbc('x'); regmbc(0x1e8b); regmbc(0x1e8d);
		      తిరిగి;
	    స్థితి 'y': స్థితి 0xfd: స్థితి 0xff: స్థితి 0x177: స్థితి 0x1b4:
	    స్థితి 0x233: స్థితి 0x24f: స్థితి 0x1e8f: స్థితి 0x1e99: స్థితి 0x1ef3:
	    స్థితి 0x1ef5: స్థితి 0x1ef7: స్థితి 0x1ef9:
		      regmbc('y'); regmbc(0xfd); regmbc(0xff);
		      regmbc(0x177); regmbc(0x1b4); regmbc(0x233);
		      regmbc(0x24f); regmbc(0x1e8f); regmbc(0x1e99);
		      regmbc(0x1ef3); regmbc(0x1ef5); regmbc(0x1ef7);
		      regmbc(0x1ef9);
		      తిరిగి;
	    స్థితి 'z': స్థితి 0x17a: స్థితి 0x17c: స్థితి 0x17e: స్థితి 0x1b6:
	    స్థితి 0x1d76: స్థితి 0x1d8e: స్థితి 0x1e91: స్థితి 0x1e93:
	    స్థితి 0x1e95: స్థితి 0x2c6c:
		      regmbc('z'); regmbc(0x17a); regmbc(0x17c);
		      regmbc(0x17e); regmbc(0x1b6); regmbc(0x1d76);
		      regmbc(0x1d8e); regmbc(0x1e91); regmbc(0x1e93);
		      regmbc(0x1e95); regmbc(0x2c6c);
		      తిరిగి;
	}
    }
    regmbc(c);
}

/*
 * Emit a node.
 * Return pointer to generated code.
 */
    స్థిర char_u *
regnode(పూర్ణాంకం op)
{
    char_u  *ret;

    ret = regcode;
    ఒకవేళ (ret == JUST_CALC_కొలత)
	regsize += 3;
    లేకపోతే
    {
	*regcode++ = op;
	*regcode++ = NUL;		// Null "next" pointer.
	*regcode++ = NUL;
    }
    తిరిగి ret;
}

/*
 * Write a long as four bytes at "p" and return pointer to the next char.
 */
    స్థిర char_u *
re_put_long(char_u *p, long_u val)
{
    *p++ = (char_u) ((val >> 24) & 0377);
    *p++ = (char_u) ((val >> 16) & 0377);
    *p++ = (char_u) ((val >> 8) & 0377);
    *p++ = (char_u) (val & 0377);
    తిరిగి p;
}

/*
 * regnext - dig the "next" pointer out of a node
 * Returns NULL when calculating size, when there is no next item and when
 * there is an error.
 */
    స్థిర char_u *
regnext(char_u *p)
{
    పూర్ణాంకం	    స్థానభ్రంశం;

    ఒకవేళ (p == JUST_CALC_కొలత || reg_toolong)
	తిరిగి ఖాళీ;

    స్థానభ్రంశం = తదుపరి_2(p);
    ఒకవేళ (స్థానభ్రంశం == 0)
	తిరిగి ఖాళీ;

    ఒకవేళ (OP(p) == వెనుక_2)
	తిరిగి p - స్థానభ్రంశం;
    లేకపోతే
	తిరిగి p + స్థానభ్రంశం;
}

/*
 * Set the next-pointer at the end of a node chain.
 */
    స్థిర శూన్య
regtail(char_u *p, char_u *val)
{
    char_u	*పరిశీలన;
    char_u	*తాత్కాలిక;
    పూర్ణాంకం		స్థానభ్రంశం;

    ఒకవేళ (p == JUST_CALC_కొలత)
	తిరిగి;

    // Find last node.
    పరిశీలన = p;
    కొరకు (;;)
    {
	తాత్కాలిక = regnext(పరిశీలన);
	ఒకవేళ (తాత్కాలిక == ఖాళీ)
	    ఆపు;
	పరిశీలన = తాత్కాలిక;
    }

    ఒకవేళ (OP(పరిశీలన) == వెనుక_2)
	స్థానభ్రంశం = (పూర్ణాంకం)(పరిశీలన - val);
    లేకపోతే
	స్థానభ్రంశం = (పూర్ణాంకం)(val - పరిశీలన);
    // When the offset uses more than 16 bits it can no longer fit in the two
    // bytes available.  Use a global flag to avoid having to check return
    // values in too many places.
    ఒకవేళ (స్థానభ్రంశం > 0xffff)
	reg_toolong = TRUE;
    లేకపోతే
    {
	*(పరిశీలన + 1) = (char_u) (((గుర్తులేని)స్థానభ్రంశం >> 8) & 0377);
	*(పరిశీలన + 2) = (char_u) (స్థానభ్రంశం & 0377);
    }
}

/*
 * Like regtail, on item after a BRANCH; nop if none.
 */
    స్థిర శూన్య
regoptail(char_u *p, char_u *val)
{
    // When op is neither BRANCH nor BRACE_COMPLEX0-9, it is "operandless"
    ఒకవేళ (p == ఖాళీ || p == JUST_CALC_కొలత
	    || (OP(p) != BRANCH
		&& (OP(p) < BRACE_COMPLEX || OP(p) > BRACE_COMPLEX + 9)))
	తిరిగి;
    regtail(నిర్వాహ్యం(p), val);
}

/*
 * Insert an operator in front of already-emitted operand
 *
 * Means relocating the operand.
 */
    స్థిర శూన్య
reginsert(పూర్ణాంకం op, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*ఉంచు;

    ఒకవేళ (regcode == JUST_CALC_కొలత)
    {
	regsize += 3;
	తిరిగి;
    }
    src = regcode;
    regcode += 3;
    dst = regcode;
    అయితే (src > opnd)
	*--dst = *--src;

    ఉంచు = opnd;		// Op node, where operand used to be.
    *ఉంచు++ = op;
    *ఉంచు++ = NUL;
    *ఉంచు = NUL;
}

/*
 * Insert an operator in front of already-emitted operand.
 * Add a number to the operator.
 */
    స్థిర శూన్య
reginsert_nr(పూర్ణాంకం op, దీర్ఘ val, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*ఉంచు;

    ఒకవేళ (regcode == JUST_CALC_కొలత)
    {
	regsize += 7;
	తిరిగి;
    }
    src = regcode;
    regcode += 7;
    dst = regcode;
    అయితే (src > opnd)
	*--dst = *--src;

    ఉంచు = opnd;		// Op node, where operand used to be.
    *ఉంచు++ = op;
    *ఉంచు++ = NUL;
    *ఉంచు++ = NUL;
    re_put_long(ఉంచు, (long_u)val);
}

/*
 * Insert an operator in front of already-emitted operand.
 * The operator has the given limit values as operands.  Also set next pointer.
 *
 * Means relocating the operand.
 */
    స్థిర శూన్య
reginsert_limits(
    పూర్ణాంకం		op,
    దీర్ఘ	minval,
    దీర్ఘ	maxval,
    char_u	*opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*ఉంచు;

    ఒకవేళ (regcode == JUST_CALC_కొలత)
    {
	regsize += 11;
	తిరిగి;
    }
    src = regcode;
    regcode += 11;
    dst = regcode;
    అయితే (src > opnd)
	*--dst = *--src;

    ఉంచు = opnd;		// Op node, where operand used to be.
    *ఉంచు++ = op;
    *ఉంచు++ = NUL;
    *ఉంచు++ = NUL;
    ఉంచు = re_put_long(ఉంచు, (long_u)minval);
    ఉంచు = re_put_long(ఉంచు, (long_u)maxval);
    regtail(opnd, ఉంచు);
}

/*
 * Return TRUE if the back reference is legal. We must have seen the close
 * brace.
 * TODO: Should also check that we don't refer to something that is repeated
 * (+*=): what instance of the repetition should we match?
 */
    స్థిర పూర్ణాంకం
చూసిన_endbrace(పూర్ణాంకం refnum)
{
    ఒకవేళ (!had_endbrace[refnum])
    {
	char_u *p;

	// Trick: check if "@<=" or "@<!" follows, in which case
	// the \1 can appear before the referenced match.
	కొరకు (p = regparse; *p != NUL; ++p)
	    ఒకవేళ (p[0] == '@' && p[1] == '<' && (p[2] == '!' || p[2] == '='))
		ఆపు;
	ఒకవేళ (*p == NUL)
	{
	    emsg(_(e_illegal_వెనుక_reference));
	    rc_did_emsg = TRUE;
	    తిరిగి FALSE;
	}
    }
    తిరిగి TRUE;
}

/*
 * Parse the lowest level.
 *
 * Optimization:  gobbles an entire sequence of ordinary characters so that
 * it can turn them into a single node, which is smaller to store and
 * faster to run.  Don't do this when one_exactly is set.
 */
    స్థిర char_u *
regatom(పూర్ణాంకం *flagp)
{
    char_u	    *ret;
    పూర్ణాంకం		    flags;
    పూర్ణాంకం		    c;
    char_u	    *p;
    పూర్ణాంకం		    extra = 0;
    పూర్ణాంకం		    save_prev_at_ప్రారంభం = prev_at_ప్రారంభం;

    *flagp = WORST;		// Tentatively.

    c = getchr();
    ఎంపిక (c)
    {
      స్థితి Magic('^'):
	ret = regnode(BOL);
	ఆపు;

      స్థితి Magic('$'):
	ret = regnode(EOL);
#ఒకవేళ defined(FEAT_SYN_HL)
	had_eol = TRUE;
#అయితే_ముగింపు
	ఆపు;

      స్థితి Magic('<'):
	ret = regnode(BOW);
	ఆపు;

      స్థితి Magic('>'):
	ret = regnode(EOW);
	ఆపు;

      స్థితి Magic('_'):
	c = no_Magic(getchr());
	ఒకవేళ (c == '^')		// "\_^" is start-of-line
	{
	    ret = regnode(BOL);
	    ఆపు;
	}
	ఒకవేళ (c == '$')		// "\_$" is end-of-line
	{
	    ret = regnode(EOL);
#ఒకవేళ defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#అయితే_ముగింపు
	    ఆపు;
	}

	extra = కూడు_NL;
	*flagp |= HASNL;

	// "\_[" is character range plus newline
	ఒకవేళ (c == '[')
	    వెళ్ళు collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	// Character classes.
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
	    EMSG_RET_NULL(_(e_invalid_use_యొక్క_underscore));

	// When '.' is followed by a composing char ignore the dot, so that
	// the composing char is matched here.
	ఒకవేళ (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	{
	    c = getchr();
	    వెళ్ళు do_multibyte;
	}
	ret = regnode(classcodes[p - classchars] + extra);
	*flagp |= HASWIDTH | SIMPLE;
	ఆపు;

      స్థితి Magic('n'):
	ఒకవేళ (reg_string)
	{
	    // In a string "\n" matches a newline character.
	    ret = regnode(EXACTLY);
	    regc(NL);
	    regc(NUL);
	    *flagp |= HASWIDTH | SIMPLE;
	}
	లేకపోతే
	{
	    // In buffer text "\n" matches the end of a line.
	    ret = regnode(NEWL);
	    *flagp |= HASWIDTH | HASNL;
	}
	ఆపు;

      స్థితి Magic('('):
	ఒకవేళ (one_exactly)
	    EMSG_ONE_RET_NULL;
	ret = reg(REG_PAREN, &flags);
	ఒకవేళ (ret == ఖాళీ)
	    తిరిగి ఖాళీ;
	*flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
	ఆపు;

      స్థితి NUL:
      స్థితి Magic('|'):
      స్థితి Magic('&'):
      స్థితి Magic(')'):
	ఒకవేళ (one_exactly)
	    EMSG_ONE_RET_NULL;
	// Supposed to be caught earlier.
	IEMSG_RET_NULL(e_అంతర్గత_error_లోపల_regexp);
	// NOTREACHED

      స్థితి Magic('='):
      స్థితి Magic('?'):
      స్థితి Magic('+'):
      స్థితి Magic('@'):
      స్థితి Magic('{'):
      స్థితి Magic('*'):
	c = no_Magic(c);
	EMSG3_RET_NULL(_(e_str_chr_follows_nothing),
		(c == '*' ? reg_magic >= MAGIC_ON : reg_magic == MAGIC_ALL), c);
	// NOTREACHED

      స్థితి Magic('~'):		// previous substitute pattern
	    ఒకవేళ (reg_prev_sub != ఖాళీ)
	    {
		char_u	    *lp;

		ret = regnode(EXACTLY);
		lp = reg_prev_sub;
		అయితే (*lp != NUL)
		    regc(*lp++);
		regc(NUL);
		ఒకవేళ (*reg_prev_sub != NUL)
		{
		    *flagp |= HASWIDTH;
		    ఒకవేళ ((lp - reg_prev_sub) == 1)
			*flagp |= SIMPLE;
		}
	    }
	    లేకపోతే
		EMSG_RET_NULL(_(e_no_మునుపటి_substitute_regular_వ్యక్తీకరణ));
	    ఆపు;

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
		పూర్ణాంకం		    refnum;

		refnum = c - Magic('0');
		ఒకవేళ (!చూసిన_endbrace(refnum))
		    తిరిగి ఖాళీ;
		ret = regnode(BACKREF + refnum);
	    }
	    ఆపు;

      స్థితి Magic('z'):
	{
	    c = no_Magic(getchr());
	    ఎంపిక (c)
	    {
#నిర్వచితమైతే FEAT_SYN_HL
		స్థితి '(': ఒకవేళ ((reg_do_extmatch & REX_సమితి) == 0)
			      EMSG_RET_NULL(_(e_z_not_allowed_ఇక్కడ));
			  ఒకవేళ (one_exactly)
			      EMSG_ONE_RET_NULL;
			  ret = reg(REG_ZPAREN, &flags);
			  ఒకవేళ (ret == ఖాళీ)
			      తిరిగి ఖాళీ;
			  *flagp |= flags & (HASWIDTH|SPSTART|HASNL|HASLOOKBH);
			  re_has_z = REX_సమితి;
			  ఆపు;

		స్థితి '1':
		స్థితి '2':
		స్థితి '3':
		స్థితి '4':
		స్థితి '5':
		స్థితి '6':
		స్థితి '7':
		స్థితి '8':
		స్థితి '9': ఒకవేళ ((reg_do_extmatch & REX_USE) == 0)
			      EMSG_RET_NULL(_(e_z1_z9_not_allowed_ఇక్కడ));
			  ret = regnode(ZREF + c - '0');
			  re_has_z = REX_USE;
			  ఆపు;
#అయితే_ముగింపు

		స్థితి 's': ret = regnode(MOPEN + 0);
			  ఒకవేళ (re_mult_తదుపరి("\\zs") == FAIL)
			      తిరిగి ఖాళీ;
			  ఆపు;

		స్థితి 'e': ret = regnode(MCLOSE + 0);
			  ఒకవేళ (re_mult_తదుపరి("\\ze") == FAIL)
			      తిరిగి ఖాళీ;
			  ఆపు;

		అప్రమేయ:  EMSG_RET_NULL(_(e_invalid_character_after_bsl_z));
	    }
	}
	ఆపు;

      స్థితి Magic('%'):
	{
	    c = no_Magic(getchr());
	    ఎంపిక (c)
	    {
		// () without a back reference
		స్థితి '(':
		    ఒకవేళ (one_exactly)
			EMSG_ONE_RET_NULL;
		    ret = reg(REG_NPAREN, &flags);
		    ఒకవేళ (ret == ఖాళీ)
			తిరిగి ఖాళీ;
		    *flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
		    ఆపు;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		స్థితి '^':
		    ret = regnode(RE_BOF);
		    ఆపు;

		స్థితి '$':
		    ret = regnode(RE_EOF);
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
		    ret = regnode(CURSOR);
		    ఆపు;

		స్థితి 'V':
		    ret = regnode(RE_VISUAL);
		    ఆపు;

		స్థితి 'C':
		    ret = regnode(RE_COMPOSING);
		    ఆపు;

		// \%[abc]: Emit as a list of branches, all ending at the last
		// branch which matches nothing.
		స్థితి '[':
			  ఒకవేళ (one_exactly)	// doesn't nest
			      EMSG_ONE_RET_NULL;
			  {
			      char_u	*lastbranch;
			      char_u	*lastnode = ఖాళీ;
			      char_u	*br;

			      ret = ఖాళీ;
			      అయితే ((c = getchr()) != ']')
			      {
				  ఒకవేళ (c == NUL)
				      EMSG2_RET_NULL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
				  br = regnode(BRANCH);
				  ఒకవేళ (ret == ఖాళీ)
				      ret = br;
				  లేకపోతే
				  {
				      regtail(lastnode, br);
				      ఒకవేళ (reg_toolong)
					  తిరిగి ఖాళీ;
				  }

				  ungetchr();
				  one_exactly = TRUE;
				  lastnode = regatom(flagp);
				  one_exactly = FALSE;
				  ఒకవేళ (lastnode == ఖాళీ)
				      తిరిగి ఖాళీ;
			      }
			      ఒకవేళ (ret == ఖాళీ)
				  EMSG2_RET_NULL(_(e_ఖాళీలు_str_brackets),
						      reg_magic == MAGIC_ALL);
			      lastbranch = regnode(BRANCH);
			      br = regnode(NOTHING);
			      ఒకవేళ (ret != JUST_CALC_కొలత)
			      {
				  regtail(lastnode, br);
				  regtail(lastbranch, br);
				  // connect all branches to the NOTHING
				  // branch at the end
				  కొరకు (br = ret; br != lastnode; )
				  {
				      ఒకవేళ (OP(br) == BRANCH)
				      {
					  regtail(br, lastbranch);
					  ఒకవేళ (reg_toolong)
					      తిరిగి ఖాళీ;
					  br = నిర్వాహ్యం(br);
				      }
				      లేకపోతే
					  br = regnext(br);
				  }
			      }
			      *flagp &= ~(HASWIDTH | SIMPLE);
			      ఆపు;
			  }

		స్థితి 'd':   // %d123 decimal
		స్థితి 'o':   // %o123 octal
		స్థితి 'x':   // %xab hex 2
		స్థితి 'u':   // %uabcd hex 4
		స్థితి 'U':   // %U1234abcd hex 8
			  {
			      vimlong_T i;

			      ఎంపిక (c)
			      {
				  స్థితి 'd': i = getdecchrs(); ఆపు;
				  స్థితి 'o': i = getoctchrs(); ఆపు;
				  స్థితి 'x': i = gethexchrs(2); ఆపు;
				  స్థితి 'u': i = gethexchrs(4); ఆపు;
				  స్థితి 'U': i = gethexchrs(8); ఆపు;
				  అప్రమేయ:  i = -1; ఆపు;
			      }

			      ఒకవేళ (i < 0 || i > గరిష్ఠ_పూర్ణాంకం)
				  EMSG2_RET_NULL(
					    _(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			      ఒకవేళ (use_multibytecode(i))
				  ret = regnode(MULTIBYTECODE);
			      లేకపోతే
				  ret = regnode(EXACTLY);
			      ఒకవేళ (i == 0)
				  regc(0x0a);
			      లేకపోతే
				  regmbc((పూర్ణాంకం)i);
			      regc(NUL);
			      *flagp |= HASWIDTH;
			      ఆపు;
			  }

		అప్రమేయ:
			  ఒకవేళ (VIM_ISDIGIT(c) || c == '<' || c == '>'
						|| c == '\'' || c == '.')
			  {
			      long_u	n = 0;
			      పూర్ణాంకం	cmp;
			      పూర్ణాంకం	cur = FALSE;
			      పూర్ణాంకం	got_digit = FALSE;

			      cmp = c;
			      ఒకవేళ (cmp == '<' || cmp == '>')
				  c = getchr();
			      ఒకవేళ (no_Magic(c) == '.')
			      {
				  cur = TRUE;
				  c = getchr();
			      }
			      అయితే (VIM_ISDIGIT(c))
			      {
				  got_digit = TRUE;
				  n = n * 10 + (c - '0');
				  c = getchr();
			      }
			      ఒకవేళ (no_Magic(c) == '\'' && n == 0)
			      {
				  // "\%'m", "\%<'m" and "\%>'m": Mark
				  c = getchr();
				  ret = regnode(RE_MARK);
				  ఒకవేళ (ret == JUST_CALC_కొలత)
				      regsize += 2;
				  లేకపోతే
				  {
				      *regcode++ = c;
				      *regcode++ = cmp;
				  }
				  ఆపు;
			      }
			      లేకపోతే ఒకవేళ ((c == 'l' || c == 'c' || c == 'v')
					  && (cur || got_digit))
			      {
				  ఒకవేళ (cur && n)
				  {
				    semsg(_(e_regexp_సంఖ్య_after_dot_pos_శోధన_chr),
								  no_Magic(c));
				    rc_did_emsg = TRUE;
				    తిరిగి ఖాళీ;
				  }
				  ఒకవేళ (c == 'l')
				  {
				      ఒకవేళ (cur)
					  n = curwin->w_cursor.lnum;
				      ret = regnode(RE_LNUM);
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
				      ret = regnode(RE_నిలువు);
				  }
				  లేకపోతే
				  {
				      ఒకవేళ (cur)
				      {
					  colnr_T vcol = 0;

					  getvvcol(curwin, &curwin->w_cursor,
							 ఖాళీ, ఖాళీ, &vcol, 0);
					  ++vcol;
					  n = vcol;
				      }
				      ret = regnode(RE_VCOL);
				  }
				  ఒకవేళ (ret == JUST_CALC_కొలత)
				      regsize += 5;
				  లేకపోతే
				  {
				      // put the number and the optional
				      // comparator after the opcode
				      regcode = re_put_long(regcode, n);
				      *regcode++ = cmp;
				  }
				  ఆపు;
			      }
			  }

			  EMSG2_RET_NULL(_(e_invalid_character_after_str),
						      reg_magic == MAGIC_ALL);
	    }
	}
	ఆపు;

      స్థితి Magic('['):
collection:
	{
	    char_u	*lp;

	    // If there is no matching ']', we assume the '[' is a normal
	    // character.  This makes 'incsearch' and ":help [" work.
	    lp = skip_anyof(regparse);
	    ఒకవేళ (*lp == ']')	// there is a matching ']'
	    {
		పూర్ణాంకం	startc = -1;	// > 0 when next '-' is a range
		పూర్ణాంకం	endc;

		ఒకవేళ (extra == 0)
		{
		    పూర్ణాంకం	cl = bt_recognize_char_class(regparse, lp);

		    ఒకవేళ (cl != 0)
		    {
			ret = regnode(cl);
			regparse = lp;
			skipchr();
			*flagp |= HASWIDTH | SIMPLE;
			ఆపు;
		    }
		}

		// In a character class, different parsing rules apply.
		// Not even \ is special anymore, nothing is.
		ఒకవేళ (*regparse == '^')	    // Complement of range.
		{
		    ret = regnode(ANYBUT + extra);
		    regparse++;
		}
		లేకపోతే
		    ret = regnode(ANYOF + extra);

		// At the start ']' and '-' mean the literal character.
		ఒకవేళ (*regparse == ']' || *regparse == '-')
		{
		    startc = *regparse;
		    regc(*regparse++);
		}

		అయితే (*regparse != NUL && *regparse != ']')
		{
		    ఒకవేళ (*regparse == '-')
		    {
			++regparse;
			// The '-' is not used for a range at the end and
			// after or before a '\n'.
			ఒకవేళ (*regparse == ']' || *regparse == NUL
				|| startc == -1
				|| (regparse[0] == '\\' && regparse[1] == 'n'))
			{
			    regc('-');
			    startc = '-';	// [--x] is a range
			}
			లేకపోతే
			{
			    // Also accept "a-[.z.]"
			    endc = 0;
			    ఒకవేళ (*regparse == '[')
				endc = get_coll_element(&regparse);
			    ఒకవేళ (endc == 0)
			    {
				ఒకవేళ (has_mbyte)
				    endc = mb_ptr2char_adv(&regparse);
				లేకపోతే
				    endc = *regparse++;
			    }

			    // Handle \o40, \x20 and \u20AC style sequences
			    ఒకవేళ (endc == '\\' && !reg_cpo_lit && !reg_cpo_bsl)
				endc = coll_get_char();

			    ఒకవేళ (startc > endc)
				EMSG_RET_NULL(_(e_తిరగేయి_range_లోపల_character_class));
			    ఒకవేళ (has_mbyte && ((*mb_char2len)(startc) > 1
						 || (*mb_char2len)(endc) > 1))
			    {
				// Limit to a range of 256 chars.
				ఒకవేళ (endc > startc + 256)
				    EMSG_RET_NULL(_(e_range_too_large_లోపల_character_class));
				అయితే (++startc <= endc)
				    regmbc(startc);
			    }
			    లేకపోతే
			    {
				అయితే (++startc <= endc)
				    regc(startc);
			    }
			    startc = -1;
			}
		    }
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    లేకపోతే ఒకవేళ (*regparse == '\\'
			    && !reg_cpo_bsl
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != ఖాళీ
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR,
						       regparse[1]) != ఖాళీ)))
		    {
			regparse++;
			ఒకవేళ (*regparse == 'n')
			{
			    // '\n' in range: also match NL
			    ఒకవేళ (ret != JUST_CALC_కొలత)
			    {
				// Using \n inside [^] does not change what
				// matches. "[^\n]" is the same as ".".
				ఒకవేళ (*ret == ANYOF)
				{
				    *ret = ANYOF + కూడు_NL;
				    *flagp |= HASNL;
				}
				// else: must have had a \n already
			    }
			    regparse++;
			    startc = -1;
			}
			లేకపోతే ఒకవేళ (*regparse == 'd'
				|| *regparse == 'o'
				|| *regparse == 'x'
				|| *regparse == 'u'
				|| *regparse == 'U')
			{
			    startc = coll_get_char();
			    // max UTF-8 Codepoint is U+10FFFF,
			    // but allow values until INT_MAX
			    ఒకవేళ (startc == గరిష్ఠ_పూర్ణాంకం)
				EMSG_RET_NULL(_(e_unicode_val_too_large));
			    ఒకవేళ (startc == 0)
				regc(0x0a);
			    లేకపోతే
				regmbc(startc);
			}
			లేకపోతే
			{
			    startc = backslash_trans(*regparse++);
			    regc(startc);
			}
		    }
		    లేకపోతే ఒకవేళ (*regparse == '[')
		    {
			పూర్ణాంకం c_class;
			పూర్ణాంకం cu;

			c_class = get_char_class(&regparse);
			startc = -1;
			// Characters assumed to be 8 bits!
			ఎంపిక (c_class)
			{
			    స్థితి CLASS_NONE:
				c_class = get_equi_class(&regparse);
				ఒకవేళ (c_class != 0)
				{
				    // produce equivalence class
				    reg_equi_class(c_class);
				}
				లేకపోతే ఒకవేళ ((c_class =
					    get_coll_element(&regparse)) != 0)
				{
				    // produce a collating element
				    regmbc(c_class);
				}
				లేకపోతే
				{
				    // literal '[', allow [[-x] as a range
				    startc = *regparse++;
				    regc(startc);
				}
				ఆపు;
			    స్థితి CLASS_ALNUM:
				కొరకు (cu = 1; cu < 128; cu++)
				    ఒకవేళ (అక్షరమా_అంకెనా(cu))
					regmbc(cu);
				ఆపు;
			    స్థితి CLASS_ALPHA:
				కొరకు (cu = 1; cu < 128; cu++)
				    ఒకవేళ (అక్షరమా(cu))
					regmbc(cu);
				ఆపు;
			    స్థితి CLASS_BLANK:
				regc(' ');
				regc('\t');
				ఆపు;
			    స్థితి CLASS_CNTRL:
				కొరకు (cu = 1; cu <= 127; cu++)
				    ఒకవేళ (iscntrl(cu))
					regmbc(cu);
				ఆపు;
			    స్థితి CLASS_DIGIT:
				కొరకు (cu = 1; cu <= 127; cu++)
				    ఒకవేళ (VIM_ISDIGIT(cu))
					regmbc(cu);
				ఆపు;
			    స్థితి CLASS_లేఖాచిత్రం:
				కొరకు (cu = 1; cu <= 127; cu++)
				    ఒకవేళ (isgraph(cu))
					regmbc(cu);
				ఆపు;
			    స్థితి CLASS_క్రింది:
				కొరకు (cu = 1; cu <= 255; cu++)
				    ఒకవేళ (MB_ISLOWER(cu) && cu != 170
								 && cu != 186)
					regmbc(cu);
				ఆపు;
			    స్థితి CLASS_PRINT:
				కొరకు (cu = 1; cu <= 255; cu++)
				    ఒకవేళ (vim_isprintc(cu))
					regmbc(cu);
				ఆపు;
			    స్థితి CLASS_PUNCT:
				కొరకు (cu = 1; cu < 128; cu++)
				    ఒకవేళ (ispunct(cu))
					regmbc(cu);
				ఆపు;
			    స్థితి CLASS_SPACE:
				కొరకు (cu = 9; cu <= 13; cu++)
				    regc(cu);
				regc(' ');
				ఆపు;
			    స్థితి CLASS_పై:
				కొరకు (cu = 1; cu <= 255; cu++)
				    ఒకవేళ (MB_ISUPPER(cu))
					regmbc(cu);
				ఆపు;
			    స్థితి CLASS_XDIGIT:
				కొరకు (cu = 1; cu <= 255; cu++)
				    ఒకవేళ (vim_isxdigit(cu))
					regmbc(cu);
				ఆపు;
			    స్థితి CLASS_TAB:
				regc('\t');
				ఆపు;
			    స్థితి CLASS_RETURN:
				regc('\r');
				ఆపు;
			    స్థితి CLASS_BACKSPACE:
				regc('\b');
				ఆపు;
			    స్థితి CLASS_ESCAPE:
				regc('\033');
				ఆపు;
			    స్థితి CLASS_IDENT:
				కొరకు (cu = 1; cu <= 255; cu++)
				    ఒకవేళ (vim_isIDc(cu))
					regmbc(cu);
				ఆపు;
			    స్థితి CLASS_KEYWORD:
				కొరకు (cu = 1; cu <= 255; cu++)
				    ఒకవేళ (reg_iswordc(cu))
					regmbc(cu);
				ఆపు;
			    స్థితి CLASS_FNAME:
				కొరకు (cu = 1; cu <= 255; cu++)
				    ఒకవేళ (vim_isfilec(cu))
					regmbc(cu);
				ఆపు;
			}
		    }
		    లేకపోతే
		    {
			ఒకవేళ (has_mbyte)
			{
			    పూర్ణాంకం	len;

			    // produce a multibyte character, including any
			    // following composing characters
			    startc = mb_ptr2char(regparse);
			    len = (*mb_ptr2len)(regparse);
			    ఒకవేళ (enc_utf8 && utf_char2len(startc) != len)
				startc = -1;	// composing chars
			    అయితే (--len >= 0)
				regc(*regparse++);
			}
			లేకపోతే
			{
			    startc = *regparse++;
			    regc(startc);
			}
		    }
		}
		regc(NUL);
		prevchr_len = 1;	// last char was the ']'
		ఒకవేళ (*regparse != ']')
		    EMSG_RET_NULL(_(e_too_many_brackets));  // Cannot happen?
		skipchr();	    // let's be friends with the lexer again
		*flagp |= HASWIDTH | SIMPLE;
		ఆపు;
	    }
	    లేకపోతే ఒకవేళ (reg_strict)
		EMSG2_RET_NULL(_(e_missing_rsb_after_str_lsb),
							reg_magic > MAGIC_OFF);
	}
	// FALLTHROUGH

      అప్రమేయ:
	{
	    పూర్ణాంకం		len;

	    // A multi-byte character is handled as a separate atom if it's
	    // before a multi and when it's a composing char.
	    ఒకవేళ (use_multibytecode(c))
	    {
do_multibyte:
		ret = regnode(MULTIBYTECODE);
		regmbc(c);
		*flagp |= HASWIDTH | SIMPLE;
		ఆపు;
	    }

	    ret = regnode(EXACTLY);

	    // Append characters as long as:
	    // - there is no following multi, we then need the character in
	    //   front of it as a single character operand
	    // - not running into a Magic character
	    // - "one_exactly" is not set
	    // But always emit at least one character.  Might be a Multi,
	    // e.g., a "[" without matching "]".
	    కొరకు (len = 0; c != NUL && (len == 0
			|| (re_multi_type(peekchr()) == NOT_MULTI
			    && !one_exactly
			    && !is_Magic(c))); ++len)
	    {
		c = no_Magic(c);
		ఒకవేళ (has_mbyte)
		{
		    regmbc(c);
		    ఒకవేళ (enc_utf8)
		    {
			పూర్ణాంకం	l;

			// Need to get composing character too.
			కొరకు (;;)
			{
			    l = utf_ptr2len(regparse);
			    ఒకవేళ (!UTF_COMPOSINGLIKE(regparse, regparse + l))
				ఆపు;
			    regmbc(utf_ptr2char(regparse));
			    skipchr();
			}
		    }
		}
		లేకపోతే
		    regc(c);
		c = getchr();
	    }
	    ungetchr();

	    regc(NUL);
	    *flagp |= HASWIDTH;
	    ఒకవేళ (len == 1)
		*flagp |= SIMPLE;
	}
	ఆపు;
    }

    తిరిగి ret;
}

/*
 * Parse something followed by possible [*+=].
 *
 * Note that the branching code sequences used for = and the general cases
 * of * and + are somewhat optimized:  they use the same NOTHING node as
 * both the endmarker for their branch list and the body of the last branch.
 * It might seem that this node could be dispensed with entirely, but the
 * endmarker role is not redundant.
 */
    స్థిర char_u *
regpiece(పూర్ణాంకం *flagp)
{
    char_u	    *ret;
    పూర్ణాంకం		    op;
    char_u	    *తదుపరి;
    పూర్ణాంకం		    flags;
    దీర్ఘ	    minval;
    దీర్ఘ	    maxval;

    ret = regatom(&flags);
    ఒకవేళ (ret == ఖాళీ)
	తిరిగి ఖాళీ;

    op = peekchr();
    ఒకవేళ (re_multi_type(op) == NOT_MULTI)
    {
	*flagp = flags;
	తిరిగి ret;
    }
    // default flags
    *flagp = (WORST | SPSTART | (flags & (HASNL | HASLOOKBH)));

    skipchr();
    ఎంపిక (op)
    {
	స్థితి Magic('*'):
	    ఒకవేళ (flags & SIMPLE)
		reginsert(నక్షత్రం, ret);
	    లేకపోతే
	    {
		// Emit x* as (x&|), where & means "self".
		reginsert(BRANCH, ret); // Either x
		regoptail(ret, regnode(వెనుక_2));	// and loop
		regoptail(ret, ret);	//వెనుక
		regtail(ret, regnode(BRANCH));	// or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    ఆపు;

	స్థితి Magic('+'):
	    ఒకవేళ (flags & SIMPLE)
		reginsert(PLUS, ret);
	    లేకపోతే
	    {
		// Emit x+ as x(&|), where & means "self".
		తదుపరి = regnode(BRANCH); // Either
		regtail(ret, తదుపరి);
		regtail(regnode(వెనుక_2), ret);	// loop back
		regtail(తదుపరి, regnode(BRANCH)); // or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    *flagp = (WORST | HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    ఆపు;

	స్థితి Magic('@'):
	    {
		పూర్ణాంకం	lop = చివర_3;
		దీర్ఘ	nr;

		nr = (దీర్ఘ)getdecchrs();
		ఎంపిక (no_Magic(getchr()))
		{
		    స్థితి '=': lop = సరిపోలిక_2; ఆపు;		  // \@=
		    స్థితి '!': lop = NOMATCH; ఆపు;		  // \@!
		    స్థితి '>': lop = SUBPAT; ఆపు;		  // \@>
		    స్థితి '<': ఎంపిక (no_Magic(getchr()))
			      {
				  స్థితి '=': lop = BEHIND; ఆపు;   // \@<=
				  స్థితి '!': lop = NOBEHIND; ఆపు; // \@<!
			      }
		}
		ఒకవేళ (lop == చివర_3)
		    EMSG2_RET_NULL(_(e_invalid_character_after_str_at),
						      reg_magic == MAGIC_ALL);
		// Look behind must match with behind_pos.
		ఒకవేళ (lop == BEHIND || lop == NOBEHIND)
		{
		    regtail(ret, regnode(BHPOS));
		    *flagp |= HASLOOKBH;
		}
		regtail(ret, regnode(చివర_3)); // operand ends
		ఒకవేళ (lop == BEHIND || lop == NOBEHIND)
		{
		    ఒకవేళ (nr < 0)
			nr = 0; // no limit is same as zero limit
		    reginsert_nr(lop, nr, ret);
		}
		లేకపోతే
		    reginsert(lop, ret);
		ఆపు;
	    }

	స్థితి Magic('?'):
	స్థితి Magic('='):
	    // Emit x= as (x|)
	    reginsert(BRANCH, ret);		// Either x
	    regtail(ret, regnode(BRANCH));	// or
	    తదుపరి = regnode(NOTHING);		// null.
	    regtail(ret, తదుపరి);
	    regoptail(ret, తదుపరి);
	    ఆపు;

	స్థితి Magic('{'):
	    ఒకవేళ (!read_limits(&minval, &maxval))
		తిరిగి ఖాళీ;
	    ఒకవేళ (flags & SIMPLE)
	    {
		reginsert(BRACE_SIMPLE, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
	    }
	    లేకపోతే
	    {
		ఒకవేళ (అంకె_complex_braces >= 10)
		    EMSG2_RET_NULL(_(e_too_many_complex_str_curly),
						      reg_magic == MAGIC_ALL);
		reginsert(BRACE_COMPLEX + అంకె_complex_braces, ret);
		regoptail(ret, regnode(వెనుక_2));
		regoptail(ret, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
		++అంకె_complex_braces;
	    }
	    ఒకవేళ (minval > 0 && maxval > 0)
		*flagp = (HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    ఆపు;
    }
    ఒకవేళ (re_multi_type(peekchr()) != NOT_MULTI)
    {
	// Can't have a multi follow a multi.
	ఒకవేళ (peekchr() == Magic('*'))
	    EMSG2_RET_NULL(_(e_nested_str), reg_magic >= MAGIC_ON);
	EMSG3_RET_NULL(_(e_nested_str_chr), reg_magic == MAGIC_ALL,
							  no_Magic(peekchr()));
    }

    తిరిగి ret;
}

/*
 * Parse one alternative of an | or & operator.
 * Implements the concatenation operator.
 */
    స్థిర char_u *
regconcat(పూర్ణాంకం *flagp)
{
    char_u	*మొదటి = ఖాళీ;
    char_u	*chain = ఖాళీ;
    char_u	*latest;
    పూర్ణాంకం		flags;
    పూర్ణాంకం		cont = TRUE;

    *flagp = WORST;		// Tentatively.

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
			    latest = regpiece(&flags);
			    ఒకవేళ (latest == ఖాళీ || reg_toolong)
				తిరిగి ఖాళీ;
			    *flagp |= flags & (HASWIDTH | HASNL | HASLOOKBH);
			    ఒకవేళ (chain == ఖాళీ)	// First piece.
				*flagp |= flags & SPSTART;
			    లేకపోతే
				regtail(chain, latest);
			    chain = latest;
			    ఒకవేళ (మొదటి == ఖాళీ)
				మొదటి = latest;
			    ఆపు;
	}
    }
    ఒకవేళ (మొదటి == ఖాళీ)		// Loop ran zero times.
	మొదటి = regnode(NOTHING);
    తిరిగి మొదటి;
}

/*
 * Parse one alternative of an | operator.
 * Implements the & operator.
 */
    స్థిర char_u *
regbranch(పూర్ణాంకం *flagp)
{
    char_u	*ret;
    char_u	*chain = ఖాళీ;
    char_u	*latest;
    పూర్ణాంకం		flags;

    *flagp = WORST | HASNL;		// Tentatively.

    ret = regnode(BRANCH);
    కొరకు (;;)
    {
	latest = regconcat(&flags);
	ఒకవేళ (latest == ఖాళీ)
	    తిరిగి ఖాళీ;
	// If one of the branches has width, the whole thing has.  If one of
	// the branches anchors at start-of-line, the whole thing does.
	// If one of the branches uses look-behind, the whole thing does.
	*flagp |= flags & (HASWIDTH | SPSTART | HASLOOKBH);
	// If one of the branches doesn't match a line-break, the whole thing
	// doesn't.
	*flagp &= ~HASNL | (flags & HASNL);
	ఒకవేళ (chain != ఖాళీ)
	    regtail(chain, latest);
	ఒకవేళ (peekchr() != Magic('&'))
	    ఆపు;
	skipchr();
	regtail(latest, regnode(చివర_3)); // operand ends
	ఒకవేళ (reg_toolong)
	    ఆపు;
	reginsert(సరిపోలిక_2, latest);
	chain = latest;
    }

    తిరిగి ret;
}

/*
 * Parse regular expression, i.e. main body or parenthesized thing.
 *
 * Caller must absorb opening parenthesis.
 *
 * Combining parenthesis handling with the base level of regular expression
 * is a trifle forced, but the need to tie the tails of the branches to what
 * follows makes it hard to avoid.
 */
    స్థిర char_u *
reg(
    పూర్ణాంకం		paren,	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
    పూర్ణాంకం		*flagp)
{
    char_u	*ret;
    char_u	*br;
    char_u	*ender;
    పూర్ణాంకం		parno = 0;
    పూర్ణాంకం		flags;

    *flagp = HASWIDTH;		// Tentatively.

#నిర్వచితమైతే FEAT_SYN_HL
    ఒకవేళ (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	ఒకవేళ (regnzpar >= NSUBEXP)
	    EMSG_RET_NULL(_(e_too_many_z));
	parno = regnzpar;
	regnzpar++;
	ret = regnode(ZOPEN + parno);
    }
    లేకపోతే
#అయితే_ముగింపు
	ఒకవేళ (paren == REG_PAREN)
    {
	// Make a MOPEN node.
	ఒకవేళ (regnpar >= NSUBEXP)
	    EMSG2_RET_NULL(_(e_too_many_str_open), reg_magic == MAGIC_ALL);
	parno = regnpar;
	++regnpar;
	ret = regnode(MOPEN + parno);
    }
    లేకపోతే ఒకవేళ (paren == REG_NPAREN)
    {
	// Make a NOPEN node.
	ret = regnode(NOPEN);
    }
    లేకపోతే
	ret = ఖాళీ;

    ఒకవేళ (bt_reg_parse_లోతు >= REG_MAX_PAREN_లోతు)
	EMSG_RET_NULL(_(e_command_too_complex));
    ++bt_reg_parse_లోతు;

    // Pick up the branches, linking them together.
    br = regbranch(&flags);
    ఒకవేళ (br == ఖాళీ)
    {
	ret = ఖాళీ;
	వెళ్ళు theend;
    }
    ఒకవేళ (ret != ఖాళీ)
	regtail(ret, br);	// [MZ]OPEN -> first.
    లేకపోతే
	ret = br;
    // If one of the branches can be zero-width, the whole thing can.
    // If one of the branches has * at start or matches a line-break, the
    // whole thing can.
    ఒకవేళ (!(flags & HASWIDTH))
	*flagp &= ~HASWIDTH;
    *flagp |= flags & (SPSTART | HASNL | HASLOOKBH);
    అయితే (peekchr() == Magic('|'))
    {
	skipchr();
	br = regbranch(&flags);
	ఒకవేళ (br == ఖాళీ || reg_toolong)
	{
	    ret = ఖాళీ;
	    వెళ్ళు theend;
	}
	regtail(ret, br);	// BRANCH -> BRANCH.
	ఒకవేళ (!(flags & HASWIDTH))
	    *flagp &= ~HASWIDTH;
	*flagp |= flags & (SPSTART | HASNL | HASLOOKBH);
    }

    // Make a closing node, and hook it on the end.
    ender = regnode(
#నిర్వచితమైతే FEAT_SYN_HL
	    paren == REG_ZPAREN ? ZCLOSE + parno :
#అయితే_ముగింపు
	    paren == REG_PAREN ? MCLOSE + parno :
	    paren == REG_NPAREN ? NCLOSE : చివర_3);
    regtail(ret, ender);

    // Hook the tails of the branches to the closing node.
    కొరకు (br = ret; br != ఖాళీ; br = regnext(br))
	regoptail(br, ender);

    // Check for proper termination.
    ఒకవేళ (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
#నిర్వచితమైతే FEAT_SYN_HL
	ఒకవేళ (paren == REG_ZPAREN)
	{
	    emsg(_(e_unmatched_z));
	    rc_did_emsg = TRUE;
	    ret = ఖాళీ;
	    వెళ్ళు theend;
	}
	లేకపోతే
#అయితే_ముగింపు
	ఒకవేళ (paren == REG_NPAREN)
	{
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = ఖాళీ;
	    వెళ్ళు theend;
	}
	లేకపోతే
	{
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = ఖాళీ;
	    వెళ్ళు theend;
	}
    }
    లేకపోతే ఒకవేళ (paren == REG_NOPAREN && peekchr() != NUL)
    {
	ఒకవేళ (curchr == Magic(')'))
	{
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = ఖాళీ;
	    వెళ్ళు theend;
	}
	లేకపోతే
	{
	    emsg(_(e_trailing_characters));	// "Can't happen".
	    rc_did_emsg = TRUE;
	    ret = ఖాళీ;
	    వెళ్ళు theend;
	}
    }
    // Here we set the flag allowing back references to this set of
    // parentheses.
    ఒకవేళ (paren == REG_PAREN)
	had_endbrace[parno] = TRUE;	// have seen the close paren

theend:
    --bt_reg_parse_లోతు;
    తిరిగి ret;
}

/*
 * bt_regcomp() - compile a regular expression into internal code for the
 * traditional back track matcher.
 * Returns the program in allocated space.  Returns NULL for an error.
 *
 * We can't allocate space until we know how big the compiled form will be,
 * but we can't compile it (and thus know how big it is) until we've got a
 * place to put the code.  So we cheat:  we compile it twice, once with code
 * generation turned off and size counting turned on, and once "for real".
 * This also means that we don't allocate space until we are sure that the
 * thing really will compile successfully, and we never have to move the
 * code and thus invalidate pointers into it.  (Note that it has to be in
 * one piece because vim_free() must be able to free it all.)
 *
 * Whether upper/lower case is to be ignored is decided when executing the
 * program, it does not matter here.
 *
 * Beware that the optimization-preparation code in here knows about some
 * of the structure of the compiled regexp.
 * "re_flags": RE_MAGIC and/or RE_STRING.
 */
    స్థిర regprog_T *
bt_regcomp(char_u *సమాసం, పూర్ణాంకం re_flags)
{
    bt_regprog_T    *r;
    char_u	*పరిశీలన;
    char_u	*longest;
    పూర్ణాంకం		len;
    పూర్ణాంకం		flags;

    ఒకవేళ (సమాసం == ఖాళీ)
	IEMSG_RET_NULL(e_null_వాదం);

    init_class_tab();

    // First pass: determine size, legality.
    regcomp_ప్రారంభం(సమాసం, re_flags);
    regcode = JUST_CALC_కొలత;
    regc(REGMAGIC);
    ఒకవేళ (reg(REG_NOPAREN, &flags) == ఖాళీ)
	తిరిగి ఖాళీ;

    // Allocate space.
    r = alloc(offsetof(bt_regprog_T, కార్యక్రమం) + regsize);
    ఒకవేళ (r == ఖాళీ)
	తిరిగి ఖాళీ;
    r->re_లోపల_use = FALSE;
#నిర్వచితమైతే DEBUG
    r->regsz = regsize;
#అయితే_ముగింపు

    // Second pass: emit code.
    regcomp_ప్రారంభం(సమాసం, re_flags);
    regcode = r->కార్యక్రమం;
    regc(REGMAGIC);
    ఒకవేళ (reg(REG_NOPAREN, &flags) == ఖాళీ || reg_toolong)
    {
	vim_free(r);
	ఒకవేళ (reg_toolong)
	    EMSG_RET_NULL(_(e_నమూనా_too_long));
	తిరిగి ఖాళీ;
    }

    // Dig out information for optimizations.
    r->regstart = NUL;		// Worst-case defaults.
    r->reganch = 0;
    r->regmust = ఖాళీ;
    r->regmlen = 0;
    r->regflags = regflags;
    ఒకవేళ (flags & HASNL)
	r->regflags |= RF_HASNL;
    ఒకవేళ (flags & HASLOOKBH)
	r->regflags |= RF_LOOKBH;
#నిర్వచితమైతే FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    r->reghasz = re_has_z;
#అయితే_ముగింపు
    పరిశీలన = r->కార్యక్రమం + 1;	// First BRANCH.
    ఒకవేళ (OP(regnext(పరిశీలన)) == చివర_3)   // Only one top-level choice.
    {
	పరిశీలన = నిర్వాహ్యం(పరిశీలన);

	// Starting-point info.
	ఒకవేళ (OP(పరిశీలన) == BOL || OP(పరిశీలన) == RE_BOF)
	{
	    r->reganch++;
	    పరిశీలన = regnext(పరిశీలన);
	}

	ఒకవేళ (OP(పరిశీలన) == EXACTLY)
	{
	    ఒకవేళ (has_mbyte)
		r->regstart = (*mb_ptr2char)(నిర్వాహ్యం(పరిశీలన));
	    లేకపోతే
		r->regstart = *నిర్వాహ్యం(పరిశీలన);
	}
	లేకపోతే ఒకవేళ ((OP(పరిశీలన) == BOW
		    || OP(పరిశీలన) == EOW
		    || OP(పరిశీలన) == NOTHING
		    || OP(పరిశీలన) == MOPEN + 0 || OP(పరిశీలన) == NOPEN
		    || OP(పరిశీలన) == MCLOSE + 0 || OP(పరిశీలన) == NCLOSE)
		 && OP(regnext(పరిశీలన)) == EXACTLY)
	{
	    ఒకవేళ (has_mbyte)
		r->regstart = (*mb_ptr2char)(నిర్వాహ్యం(regnext(పరిశీలన)));
	    లేకపోతే
		r->regstart = *నిర్వాహ్యం(regnext(పరిశీలన));
	}

	// If there's something expensive in the r.e., find the longest
	// literal string that must appear and make it the regmust.  Resolve
	// ties in favor of later strings, since the regstart check works
	// with the beginning of the r.e. and avoiding duplication
	// strengthens checking.  Not a strong reason, but sufficient in the
	// absence of others.

	// When the r.e. starts with BOW, it is faster to look for a regmust
	// first. Used a lot for "#" and "*" commands. (Added by mool).
	ఒకవేళ ((flags & SPSTART || OP(పరిశీలన) == BOW || OP(పరిశీలన) == EOW)
							  && !(flags & HASNL))
	{
	    size_t  scanlen;

	    longest = ఖాళీ;
	    len = 0;
	    కొరకు (; పరిశీలన != ఖాళీ; పరిశీలన = regnext(పరిశీలన))
	    {
		ఒకవేళ (OP(పరిశీలన) == EXACTLY)
		{
		    scanlen = STRLEN(నిర్వాహ్యం(పరిశీలన));
		    ఒకవేళ (scanlen >= (size_t)len)
		    {
			longest = నిర్వాహ్యం(పరిశీలన);
			len = (పూర్ణాంకం)scanlen;
		    }
		}
	    }
	    r->regmust = longest;
	    r->regmlen = len;
	}
    }
#నిర్వచితమైతే BT_REGEXP_DUMP
    regdump(సమాసం, r);
#అయితే_ముగింపు
    r->engine = &bt_regengine;
    తిరిగి (regprog_T *)r;
}

#ఒకవేళ defined(FEAT_SYN_HL)
/*
 * Check if during the previous call to vim_regcomp the EOL item "$" has been
 * found.  This is messy, but it works fine.
 */
    పూర్ణాంకం
vim_regcomp_had_eol(శూన్య)
{
    తిరిగి had_eol;
}
#అయితే_ముగింపు

/*
 * Get a number after a backslash that is inside [].
 * When nothing is recognized return a backslash.
 */
    స్థిర పూర్ణాంకం
coll_get_char(శూన్య)
{
    vimlong_T	nr = -1;

    ఎంపిక (*regparse++)
    {
	స్థితి 'd': nr = getdecchrs(); ఆపు;
	స్థితి 'o': nr = getoctchrs(); ఆపు;
	స్థితి 'x': nr = gethexchrs(2); ఆపు;
	స్థితి 'u': nr = gethexchrs(4); ఆపు;
	స్థితి 'U': nr = gethexchrs(8); ఆపు;
    }
    ఒకవేళ (nr < 0)
    {
	// If getting the number fails be backwards compatible: the character
	// is a backslash.
	--regparse;
	nr = '\\';
    }
    ఒకవేళ (nr > గరిష్ఠ_పూర్ణాంకం)
	nr = గరిష్ఠ_పూర్ణాంకం;
    తిరిగి nr;
}

/*
 * Free a compiled regexp program, returned by bt_regcomp().
 */
    స్థిర శూన్య
bt_regfree(regprog_T *prog)
{
    vim_free(prog);
}

#నిర్వచించు ADVANCE_REGINPUT() MB_PTR_ADV(rex.ఆదానం)

/*
 * The arguments from BRACE_LIMITS are stored here.  They are actually local
 * to regmatch(), but they are here to reduce the amount of stack space used
 * (it can be called recursively many times).
 */
స్థిర దీర్ఘ	bl_minval;
స్థిర దీర్ఘ	bl_maxval;

/*
 * Save the input line and position in a regsave_T.
 */
    స్థిర శూన్య
reg_save(regsave_T *save, garray_T *gap)
{
    ఒకవేళ (REG_MULTI)
    {
	save->rs_u.pos.నిలువు = (colnr_T)(rex.ఆదానం - rex.పంక్తి);
	save->rs_u.pos.lnum = rex.lnum;
    }
    లేకపోతే
	save->rs_u.ptr = rex.ఆదానం;
    save->rs_len = gap->ga_len;
}

/*
 * Restore the input line and position from a regsave_T.
 */
    స్థిర శూన్య
reg_restore(regsave_T *save, garray_T *gap)
{
    ఒకవేళ (REG_MULTI)
    {
	ఒకవేళ (rex.lnum != save->rs_u.pos.lnum)
	{
	    // only call reg_getline() when the line number changed to save
	    // a bit of time
	    rex.lnum = save->rs_u.pos.lnum;
	    rex.పంక్తి = reg_getline(rex.lnum);
	}
	rex.ఆదానం = rex.పంక్తి + save->rs_u.pos.నిలువు;
    }
    లేకపోతే
	rex.ఆదానం = save->rs_u.ptr;
    gap->ga_len = save->rs_len;
}

/*
 * Return TRUE if current position is equal to saved position.
 */
    స్థిర పూర్ణాంకం
reg_save_equal(regsave_T *save)
{
    ఒకవేళ (REG_MULTI)
	తిరిగి rex.lnum == save->rs_u.pos.lnum
				  && rex.ఆదానం == rex.పంక్తి + save->rs_u.pos.నిలువు;
    తిరిగి rex.ఆదానం == save->rs_u.ptr;
}

// Save the sub-expressions before attempting a match.
#నిర్వచించు save_se(savep, posp, pp) \
    REG_MULTI ? save_se_multi((savep), (posp)) : save_se_one((savep), (pp))

// After a failed match restore the sub-expressions.
#నిర్వచించు restore_se(savep, posp, pp) \
{ \
    ఒకవేళ (REG_MULTI) \
	*(posp) = (savep)->se_u.pos; \
    లేకపోతే \
	*(pp) = (savep)->se_u.ptr; \
}

/*
 * Tentatively set the sub-expression start to the current position (after
 * calling regmatch() they will have changed).  Need to save the existing
 * values for when there is no match.
 * Use se_save() to use pointer (save_se_multi()) or position (save_se_one()),
 * depending on REG_MULTI.
 */
    స్థిర శూన్య
save_se_multi(save_se_T *savep, lpos_T *posp)
{
    savep->se_u.pos = *posp;
    posp->lnum = rex.lnum;
    posp->నిలువు = (colnr_T)(rex.ఆదానం - rex.పంక్తి);
}

    స్థిర శూన్య
save_se_one(save_se_T *savep, char_u **pp)
{
    savep->se_u.ptr = *pp;
    *pp = rex.ఆదానం;
}

/*
 * regrepeat - repeatedly match something simple, return how many.
 * Advances rex.input (and rex.lnum) to just after the matched chars.
 */
    స్థిర పూర్ణాంకం
regrepeat(
    char_u	*p,
    దీర్ఘ	maxcount)   // maximum number of matches allowed
{
    దీర్ఘ	లెక్క = 0;
    char_u	*పరిశీలన;
    char_u	*opnd;
    పూర్ణాంకం		ముసుగు;
    పూర్ణాంకం		testval = 0;

    పరిశీలన = rex.ఆదానం;	    // Make local copy of rex.input for speed.
    opnd = నిర్వాహ్యం(p);
    ఎంపిక (OP(p))
    {
      స్థితి ANY:
      స్థితి ANY + కూడు_NL:
	అయితే (లెక్క < maxcount)
	{
	    // Matching anything means we continue until end-of-line (or
	    // end-of-file for ANY + ADD_NL), only limited by maxcount.
	    అయితే (*పరిశీలన != NUL && లెక్క < maxcount)
	    {
		++లెక్క;
		MB_PTR_ADV(పరిశీలన);
	    }
	    ఒకవేళ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
				      || rex.reg_పంక్తి_lbr || లెక్క == maxcount)
		ఆపు;
	    ++లెక్క;		// count the line-break
	    reg_nextline();
	    పరిశీలన = rex.ఆదానం;
	    ఒకవేళ (got_int)
		ఆపు;
	}
	ఆపు;

      స్థితి IDENT:
      స్థితి IDENT + కూడు_NL:
	testval = TRUE;
	// FALLTHROUGH
      స్థితి SIDENT:
      స్థితి SIDENT + కూడు_NL:
	అయితే (లెక్క < maxcount)
	{
	    ఒకవేళ (vim_isIDc(PTR2CHAR(పరిశీలన)) && (testval || !VIM_ISDIGIT(*పరిశీలన)))
	    {
		MB_PTR_ADV(పరిశీలన);
	    }
	    లేకపోతే ఒకవేళ (*పరిశీలన == NUL)
	    {
		ఒకవేళ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_పంక్తి_lbr)
		    ఆపు;
		reg_nextline();
		పరిశీలన = rex.ఆదానం;
		ఒకవేళ (got_int)
		    ఆపు;
	    }
	    లేకపోతే ఒకవేళ (rex.reg_పంక్తి_lbr && *పరిశీలన == '\n' && WITH_NL(OP(p)))
		++పరిశీలన;
	    లేకపోతే
		ఆపు;
	    ++లెక్క;
	}
	ఆపు;

      స్థితి KWORD:
      స్థితి KWORD + కూడు_NL:
	testval = TRUE;
	// FALLTHROUGH
      స్థితి SKWORD:
      స్థితి SKWORD + కూడు_NL:
	అయితే (లెక్క < maxcount)
	{
	    ఒకవేళ (vim_iswordp_buf(పరిశీలన, rex.reg_buf)
					  && (testval || !VIM_ISDIGIT(*పరిశీలన)))
	    {
		MB_PTR_ADV(పరిశీలన);
	    }
	    లేకపోతే ఒకవేళ (*పరిశీలన == NUL)
	    {
		ఒకవేళ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_పంక్తి_lbr)
		    ఆపు;
		reg_nextline();
		పరిశీలన = rex.ఆదానం;
		ఒకవేళ (got_int)
		    ఆపు;
	    }
	    లేకపోతే ఒకవేళ (rex.reg_పంక్తి_lbr && *పరిశీలన == '\n' && WITH_NL(OP(p)))
		++పరిశీలన;
	    లేకపోతే
		ఆపు;
	    ++లెక్క;
	}
	ఆపు;

      స్థితి FNAME:
      స్థితి FNAME + కూడు_NL:
	testval = TRUE;
	// FALLTHROUGH
      స్థితి SFNAME:
      స్థితి SFNAME + కూడు_NL:
	అయితే (లెక్క < maxcount)
	{
	    ఒకవేళ (vim_isfilec(PTR2CHAR(పరిశీలన)) && (testval || !VIM_ISDIGIT(*పరిశీలన)))
	    {
		MB_PTR_ADV(పరిశీలన);
	    }
	    లేకపోతే ఒకవేళ (*పరిశీలన == NUL)
	    {
		ఒకవేళ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_పంక్తి_lbr)
		    ఆపు;
		reg_nextline();
		పరిశీలన = rex.ఆదానం;
		ఒకవేళ (got_int)
		    ఆపు;
	    }
	    లేకపోతే ఒకవేళ (rex.reg_పంక్తి_lbr && *పరిశీలన == '\n' && WITH_NL(OP(p)))
		++పరిశీలన;
	    లేకపోతే
		ఆపు;
	    ++లెక్క;
	}
	ఆపు;

      స్థితి PRINT:
      స్థితి PRINT + కూడు_NL:
	testval = TRUE;
	// FALLTHROUGH
      స్థితి SPRINT:
      స్థితి SPRINT + కూడు_NL:
	అయితే (లెక్క < maxcount)
	{
	    ఒకవేళ (*పరిశీలన == NUL)
	    {
		ఒకవేళ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_పంక్తి_lbr)
		    ఆపు;
		reg_nextline();
		పరిశీలన = rex.ఆదానం;
		ఒకవేళ (got_int)
		    ఆపు;
	    }
	    లేకపోతే ఒకవేళ (vim_isprintc(PTR2CHAR(పరిశీలన)) == 1
					  && (testval || !VIM_ISDIGIT(*పరిశీలన)))
	    {
		MB_PTR_ADV(పరిశీలన);
	    }
	    లేకపోతే ఒకవేళ (rex.reg_పంక్తి_lbr && *పరిశీలన == '\n' && WITH_NL(OP(p)))
		++పరిశీలన;
	    లేకపోతే
		ఆపు;
	    ++లెక్క;
	}
	ఆపు;

      స్థితి WHITE:
      స్థితి WHITE + కూడు_NL:
	testval = ముసుగు = RI_WHITE;
do_class:
	అయితే (లెక్క < maxcount)
	{
	    పూర్ణాంకం		l;

	    ఒకవేళ (*పరిశీలన == NUL)
	    {
		ఒకవేళ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_పంక్తి_lbr)
		    ఆపు;
		reg_nextline();
		పరిశీలన = rex.ఆదానం;
		ఒకవేళ (got_int)
		    ఆపు;
	    }
	    లేకపోతే ఒకవేళ (has_mbyte && (l = (*mb_ptr2len)(పరిశీలన)) > 1)
	    {
		ఒకవేళ (testval != 0)
		    ఆపు;
		పరిశీలన += l;
	    }
	    లేకపోతే ఒకవేళ ((class_tab[*పరిశీలన] & ముసుగు) == testval)
		++పరిశీలన;
	    లేకపోతే ఒకవేళ (rex.reg_పంక్తి_lbr && *పరిశీలన == '\n' && WITH_NL(OP(p)))
		++పరిశీలన;
	    లేకపోతే
		ఆపు;
	    ++లెక్క;
	}
	ఆపు;

      స్థితి NWHITE:
      స్థితి NWHITE + కూడు_NL:
	ముసుగు = RI_WHITE;
	వెళ్ళు do_class;
      స్థితి DIGIT:
      స్థితి DIGIT + కూడు_NL:
	testval = ముసుగు = RI_DIGIT;
	వెళ్ళు do_class;
      స్థితి NDIGIT:
      స్థితి NDIGIT + కూడు_NL:
	ముసుగు = RI_DIGIT;
	వెళ్ళు do_class;
      స్థితి HEX:
      స్థితి HEX + కూడు_NL:
	testval = ముసుగు = RI_HEX;
	వెళ్ళు do_class;
      స్థితి NHEX:
      స్థితి NHEX + కూడు_NL:
	ముసుగు = RI_HEX;
	వెళ్ళు do_class;
      స్థితి OCTAL:
      స్థితి OCTAL + కూడు_NL:
	testval = ముసుగు = RI_OCTAL;
	వెళ్ళు do_class;
      స్థితి NOCTAL:
      స్థితి NOCTAL + కూడు_NL:
	ముసుగు = RI_OCTAL;
	వెళ్ళు do_class;
      స్థితి మాట_2:
      స్థితి మాట_2 + కూడు_NL:
	testval = ముసుగు = RI_మాట;
	వెళ్ళు do_class;
      స్థితి NWORD:
      స్థితి NWORD + కూడు_NL:
	ముసుగు = RI_మాట;
	వెళ్ళు do_class;
      స్థితి తల_2:
      స్థితి తల_2 + కూడు_NL:
	testval = ముసుగు = RI_తల;
	వెళ్ళు do_class;
      స్థితి NHEAD:
      స్థితి NHEAD + కూడు_NL:
	ముసుగు = RI_తల;
	వెళ్ళు do_class;
      స్థితి ALPHA:
      స్థితి ALPHA + కూడు_NL:
	testval = ముసుగు = RI_ALPHA;
	వెళ్ళు do_class;
      స్థితి NALPHA:
      స్థితి NALPHA + కూడు_NL:
	ముసుగు = RI_ALPHA;
	వెళ్ళు do_class;
      స్థితి క్రింది_2:
      స్థితి క్రింది_2 + కూడు_NL:
	testval = ముసుగు = RI_క్రింది;
	వెళ్ళు do_class;
      స్థితి NLOWER:
      స్థితి NLOWER + కూడు_NL:
	ముసుగు = RI_క్రింది;
	వెళ్ళు do_class;
      స్థితి పై:
      స్థితి పై + కూడు_NL:
	testval = ముసుగు = RI_పై;
	వెళ్ళు do_class;
      స్థితి NUPPER:
      స్థితి NUPPER + కూడు_NL:
	ముసుగు = RI_పై;
	వెళ్ళు do_class;

      స్థితి EXACTLY:
	{
	    పూర్ణాంకం	    cu, cl;

	    // This doesn't do a multi-byte character, because a MULTIBYTECODE
	    // would have been used for it.  It does handle single-byte
	    // characters, such as latin1.
	    ఒకవేళ (rex.reg_ic)
	    {
		cu = MB_TOUPPER(*opnd);
		cl = MB_TOLOWER(*opnd);
		అయితే (లెక్క < maxcount && (*పరిశీలన == cu || *పరిశీలన == cl))
		{
		    లెక్క++;
		    పరిశీలన++;
		}
	    }
	    లేకపోతే
	    {
		cu = *opnd;
		అయితే (లెక్క < maxcount && *పరిశీలన == cu)
		{
		    లెక్క++;
		    పరిశీలన++;
		}
	    }
	    ఆపు;
	}

      స్థితి MULTIBYTECODE:
	{
	    పూర్ణాంకం		i, len, cf = 0;

	    // Safety check (just in case 'encoding' was changed since
	    // compiling the program).
	    ఒకవేళ ((len = (*mb_ptr2len)(opnd)) > 1)
	    {
		ఒకవేళ (rex.reg_ic && enc_utf8)
		    cf = utf_fold(utf_ptr2char(opnd));
		అయితే (లెక్క < maxcount && (*mb_ptr2len)(పరిశీలన) >= len)
		{
		    కొరకు (i = 0; i < len; ++i)
			ఒకవేళ (opnd[i] != పరిశీలన[i])
			    ఆపు;
		    ఒకవేళ (i < len && (!rex.reg_ic || !enc_utf8
					|| utf_fold(utf_ptr2char(పరిశీలన)) != cf))
			ఆపు;
		    పరిశీలన += len;
		    ++లెక్క;
		}
	    }
	}
	ఆపు;

      స్థితి ANYOF:
      స్థితి ANYOF + కూడు_NL:
	testval = TRUE;
	// FALLTHROUGH

      స్థితి ANYBUT:
      స్థితి ANYBUT + కూడు_NL:
	అయితే (లెక్క < maxcount)
	{
	    పూర్ణాంకం len;

	    ఒకవేళ (*పరిశీలన == NUL)
	    {
		ఒకవేళ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_పంక్తి_lbr)
		    ఆపు;
		reg_nextline();
		పరిశీలన = rex.ఆదానం;
		ఒకవేళ (got_int)
		    ఆపు;
	    }
	    లేకపోతే ఒకవేళ (rex.reg_పంక్తి_lbr && *పరిశీలన == '\n' && WITH_NL(OP(p)))
		++పరిశీలన;
	    లేకపోతే ఒకవేళ (has_mbyte && (len = (*mb_ptr2len)(పరిశీలన)) > 1)
	    {
		ఒకవేళ ((cstrchr(opnd, (*mb_ptr2char)(పరిశీలన)) == ఖాళీ) == testval)
		    ఆపు;
		పరిశీలన += len;
	    }
	    లేకపోతే
	    {
		ఒకవేళ ((cstrchr(opnd, *పరిశీలన) == ఖాళీ) == testval)
		    ఆపు;
		++పరిశీలన;
	    }
	    ++లెక్క;
	}
	ఆపు;

      స్థితి NEWL:
	అయితే (లెక్క < maxcount
		&& ((*పరిశీలన == NUL && rex.lnum <= rex.reg_maxline
				       && !rex.reg_పంక్తి_lbr && REG_MULTI)
		    || (*పరిశీలన == '\n' && rex.reg_పంక్తి_lbr)))
	{
	    లెక్క++;
	    ఒకవేళ (rex.reg_పంక్తి_lbr)
		ADVANCE_REGINPUT();
	    లేకపోతే
		reg_nextline();
	    పరిశీలన = rex.ఆదానం;
	    ఒకవేళ (got_int)
		ఆపు;
	}
	ఆపు;

      అప్రమేయ:			// Oh dear.  Called inappropriately.
	iemsg(e_corrupted_regexp_కార్యక్రమం);
#నిర్వచితమైతే DEBUG
	రాయి("Called regrepeat with op code %d\n", OP(p));
#అయితే_ముగింపు
	ఆపు;
    }

    rex.ఆదానం = పరిశీలన;

    తిరిగి (పూర్ణాంకం)లెక్క;
}

/*
 * Push an item onto the regstack.
 * Returns pointer to new item.  Returns NULL when out of memory.
 */
    స్థిర regitem_T *
regstack_నెట్టు(regstate_T state, char_u *పరిశీలన)
{
    regitem_T	*rp;

    ఒకవేళ ((దీర్ఘ)((గుర్తులేని)regstack.ga_len >> 10) >= p_mmp)
    {
	emsg(_(e_నమూనా_ఉపయోగిస్తుంది_మరిన్ని_జ్ఞాపకం_than_maxmempattern));
	తిరిగి ఖాళీ;
    }
    ఒకవేళ (GA_GROW_FAILS(&regstack, (పూర్ణాంకం)పరిమాణం(regitem_T)))
	తిరిగి ఖాళీ;

    rp = (regitem_T *)((అక్షరం *)regstack.ga_దత్తాంశం + regstack.ga_len);
    rp->rs_state = state;
    rp->rs_పరిశీలన = పరిశీలన;

    regstack.ga_len += పరిమాణం(regitem_T);
    తిరిగి rp;
}

/*
 * Pop an item from the regstack.
 */
    స్థిర శూన్య
regstack_తీయి(char_u **పరిశీలన)
{
    regitem_T	*rp;

    rp = (regitem_T *)((అక్షరం *)regstack.ga_దత్తాంశం + regstack.ga_len) - 1;
    *పరిశీలన = rp->rs_పరిశీలన;

    regstack.ga_len -= పరిమాణం(regitem_T);
}

#నిర్వచితమైతే FEAT_RELTIME
/*
 * Check if the timer expired, return TRUE if so.
 */
    స్థిర పూర్ణాంకం
bt_did_సమయం_బహిర్గతం(పూర్ణాంకం *timed_బహిర్గతం)
{
    ఒకవేళ (*timeout_flag)
    {
	ఒకవేళ (timed_బహిర్గతం != ఖాళీ)
	{
# ifdef FEAT_EVAL
	    ఒకవేళ (!*timed_బహిర్గతం)
		ch_log(ఖాళీ, "BT regexp timed out");
# endif
	    *timed_బహిర్గతం = TRUE;
	}
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}
#అయితే_ముగింపు

/*
 * Save the current subexpr to "bp", so that they can be restored
 * later by restore_subexpr().
 */
    స్థిర శూన్య
save_subexpr(regbehind_T *bp)
{
    పూర్ణాంకం i;

    // When "rex.need_clear_subexpr" is set we don't need to save the values,
    // only remember that this flag needs to be set again when restoring.
    bp->save_need_clear_subexpr = rex.need_clear_subexpr;
    ఒకవేళ (rex.need_clear_subexpr)
	తిరిగి;

    కొరకు (i = 0; i < NSUBEXP; ++i)
    {
	ఒకవేళ (REG_MULTI)
	{
	    bp->save_ప్రారంభం[i].se_u.pos = rex.reg_startpos[i];
	    bp->save_చివర[i].se_u.pos = rex.reg_endpos[i];
	}
	లేకపోతే
	{
	    bp->save_ప్రారంభం[i].se_u.ptr = rex.reg_startp[i];
	    bp->save_చివర[i].se_u.ptr = rex.reg_endp[i];
	}
    }
}

/*
 * Restore the subexpr from "bp".
 */
    స్థిర శూన్య
restore_subexpr(regbehind_T *bp)
{
    పూర్ణాంకం i;

    // Only need to restore saved values when they are not to be cleared.
    rex.need_clear_subexpr = bp->save_need_clear_subexpr;
    ఒకవేళ (rex.need_clear_subexpr)
	తిరిగి;

    కొరకు (i = 0; i < NSUBEXP; ++i)
    {
	ఒకవేళ (REG_MULTI)
	{
	    rex.reg_startpos[i] = bp->save_ప్రారంభం[i].se_u.pos;
	    rex.reg_endpos[i] = bp->save_చివర[i].se_u.pos;
	}
	లేకపోతే
	{
	    rex.reg_startp[i] = bp->save_ప్రారంభం[i].se_u.ptr;
	    rex.reg_endp[i] = bp->save_చివర[i].se_u.ptr;
	}
    }
}

/*
 * regmatch - main matching routine
 *
 * Conceptually the strategy is simple: Check to see whether the current node
 * matches, push an item onto the regstack and loop to see whether the rest
 * matches, and then act accordingly.  In practice we make some effort to
 * avoid using the regstack, in particular by going through "ordinary" nodes
 * (that don't need to know whether the rest of the match failed) by a nested
 * loop.
 *
 * Returns TRUE when there is a match.  Leaves rex.input and rex.lnum just after
 * the last matched character.
 * Returns FALSE when there is no match.  Leaves rex.input and rex.lnum in an
 * undefined state!
 */
    స్థిర పూర్ణాంకం
regmatch(
    char_u	*పరిశీలన,		    // Current node.
    పూర్ణాంకం		*timed_బహిర్గతం వాడని)  // flag set on timeout or NULL
{
  char_u	*తదుపరి;		// Next node.
  పూర్ణాంకం		op;
  పూర్ణాంకం		c;
  regitem_T	*rp;
  పూర్ణాంకం		no;
  పూర్ణాంకం		స్థితి_2;		// one of the RA_ values:

  // Make "regstack" and "backpos" empty.  They are allocated and freed in
  // bt_regexec_both() to reduce malloc()/free() calls.
  regstack.ga_len = 0;
  backpos.ga_len = 0;

  // Repeat until "regstack" is empty.
  కొరకు (;;)
  {
    // Some patterns may take a long time to match, e.g., "\([a-z]\+\)\+Q".
    // Allow interrupting them with CTRL-C.
    fast_breakcheck();

#నిర్వచితమైతే DEBUG
    ఒకవేళ (పరిశీలన != ఖాళీ && regnarrate)
    {
	mch_errmsg((అక్షరం *)regprop(పరిశీలన));
	mch_errmsg("(\n");
    }
#అయితే_ముగింపు

    // Repeat for items that can be matched sequentially, without using the
    // regstack.
    కొరకు (;;)
    {
	ఒకవేళ (got_int || పరిశీలన == ఖాళీ)
	{
	    స్థితి_2 = RA_FAIL;
	    ఆపు;
	}
#నిర్వచితమైతే FEAT_RELTIME
	ఒకవేళ (bt_did_సమయం_బహిర్గతం(timed_బహిర్గతం))
	{
	    స్థితి_2 = RA_FAIL;
	    ఆపు;
	}
#అయితే_ముగింపు
	స్థితి_2 = RA_CONT;

#నిర్వచితమైతే DEBUG
	ఒకవేళ (regnarrate)
	{
	    mch_errmsg((అక్షరం *)regprop(పరిశీలన));
	    mch_errmsg("...\n");
# ifdef FEAT_SYN_HL
	    ఒకవేళ (re_extmatch_లోపల != ఖాళీ)
	    {
		పూర్ణాంకం i;

		mch_errmsg(_("External submatches:\n"));
		కొరకు (i = 0; i < NSUBEXP; i++)
		{
		    mch_errmsg("    \"");
		    ఒకవేళ (re_extmatch_లోపల->matches[i] != ఖాళీ)
			mch_errmsg((అక్షరం *)re_extmatch_లోపల->matches[i]);
		    mch_errmsg("\"\n");
		}
	    }
# endif
	}
#అయితే_ముగింపు
	తదుపరి = regnext(పరిశీలన);

	op = OP(పరిశీలన);
	// Check for character class with NL added.
	ఒకవేళ (!rex.reg_పంక్తి_lbr && WITH_NL(op) && REG_MULTI
			     && *rex.ఆదానం == NUL && rex.lnum <= rex.reg_maxline)
	{
	    reg_nextline();
	}
	లేకపోతే ఒకవేళ (rex.reg_పంక్తి_lbr && WITH_NL(op) && *rex.ఆదానం == '\n')
	{
	    ADVANCE_REGINPUT();
	}
	లేకపోతే
	{
	  ఒకవేళ (WITH_NL(op))
	      op -= కూడు_NL;
	  ఒకవేళ (has_mbyte)
	      c = (*mb_ptr2char)(rex.ఆదానం);
	  లేకపోతే
	      c = *rex.ఆదానం;
	  ఎంపిక (op)
	  {
	  స్థితి BOL:
	    ఒకవేళ (rex.ఆదానం != rex.పంక్తి)
		స్థితి_2 = RA_NOMATCH;
	    ఆపు;

	  స్థితి EOL:
	    ఒకవేళ (c != NUL)
		స్థితి_2 = RA_NOMATCH;
	    ఆపు;

	  స్థితి RE_BOF:
	    // We're not at the beginning of the file when below the first
	    // line where we started, not at the start of the line or we
	    // didn't start at the first line of the buffer.
	    ఒకవేళ (rex.lnum != 0 || rex.ఆదానం != rex.పంక్తి
				       || (REG_MULTI && rex.reg_firstlnum > 1))
		స్థితి_2 = RA_NOMATCH;
	    ఆపు;

	  స్థితి RE_EOF:
	    ఒకవేళ (rex.lnum != rex.reg_maxline || c != NUL)
		స్థితి_2 = RA_NOMATCH;
	    ఆపు;

	  స్థితి CURSOR:
	    // Check if the buffer is in a window and compare the
	    // rex.reg_win->w_cursor position to the match position.
	    ఒకవేళ (rex.reg_win == ఖాళీ
		    || (rex.lnum + rex.reg_firstlnum
						 != rex.reg_win->w_cursor.lnum)
		    || ((colnr_T)(rex.ఆదానం - rex.పంక్తి)
						 != rex.reg_win->w_cursor.నిలువు))
		స్థితి_2 = RA_NOMATCH;
	    ఆపు;

	  స్థితి RE_MARK:
	    // Compare the mark position to the match position.
	    {
		పూర్ణాంకం	mark = నిర్వాహ్యం(పరిశీలన)[0];
		పూర్ణాంకం	cmp = నిర్వాహ్యం(పరిశీలన)[1];
		pos_T	*pos;
		size_t	నిలువు = REG_MULTI ? rex.ఆదానం - rex.పంక్తి : 0;

		pos = getmark_buf(rex.reg_buf, mark, FALSE);

		// Line may have been freed, get it again.
		ఒకవేళ (REG_MULTI)
		{
		    rex.పంక్తి = reg_getline(rex.lnum);
		    rex.ఆదానం = rex.పంక్తి + నిలువు;
		}

		ఒకవేళ (pos == ఖాళీ		     // mark doesn't exist
			|| pos->lnum <= 0)   // mark isn't set in reg_buf
		{
		    స్థితి_2 = RA_NOMATCH;
		}
		లేకపోతే
		{
		    colnr_T pos_నిలువు = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->నిలువు == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->నిలువు;

		    ఒకవేళ ((pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_నిలువు == (colnr_T)(rex.ఆదానం - rex.పంక్తి)
				    ? (cmp == '<' || cmp == '>')
				    : (pos_నిలువు < (colnr_T)(rex.ఆదానం - rex.పంక్తి)
					? cmp != '>'
					: cmp != '<'))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? cmp != '>'
				    : cmp != '<')))
		    స్థితి_2 = RA_NOMATCH;
		}
	    }
	    ఆపు;

	  స్థితి RE_VISUAL:
	    ఒకవేళ (!reg_సరిపోలిక_visual())
		స్థితి_2 = RA_NOMATCH;
	    ఆపు;

	  స్థితి RE_LNUM:
	    ఒకవేళ (!REG_MULTI || !re_అంకె_cmp((long_u)(rex.lnum + rex.reg_firstlnum),
									పరిశీలన))
		స్థితి_2 = RA_NOMATCH;
	    ఆపు;

	  స్థితి RE_నిలువు:
	    ఒకవేళ (!re_అంకె_cmp((long_u)(rex.ఆదానం - rex.పంక్తి) + 1, పరిశీలన))
		స్థితి_2 = RA_NOMATCH;
	    ఆపు;

	  స్థితి RE_VCOL:
	    {
		win_T	    *wp = rex.reg_win == ఖాళీ ? curwin : rex.reg_win;
		linenr_T    lnum = REG_MULTI ? rex.reg_firstlnum + rex.lnum : 1;
		long_u	    vcol;

		ఒకవేళ (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క))
		    lnum = 1;
		vcol = (long_u)win_linetabsize(wp, lnum, rex.పంక్తి,
					      (colnr_T)(rex.ఆదానం - rex.పంక్తి));
		ఒకవేళ (!re_అంకె_cmp(vcol + 1, పరిశీలన))
		    స్థితి_2 = RA_NOMATCH;
	    }
	    ఆపు;

	  స్థితి BOW:	// \<word; rex.input points to w
	    ఒకవేళ (c == NUL)	// Can't match at end of line
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే ఒకవేళ (has_mbyte)
	    {
		పూర్ణాంకం this_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.ఆదానం, rex.reg_buf);
		ఒకవేళ (this_class <= 1)
		    స్థితి_2 = RA_NOMATCH;  // not on a word at all
		లేకపోతే ఒకవేళ (reg_prev_class() == this_class)
		    స్థితి_2 = RA_NOMATCH;  // previous char is in same word
	    }
	    లేకపోతే
	    {
		ఒకవేళ (!vim_iswordc_buf(c, rex.reg_buf) || (rex.ఆదానం > rex.పంక్తి
				&& vim_iswordc_buf(rex.ఆదానం[-1], rex.reg_buf)))
		    స్థితి_2 = RA_NOMATCH;
	    }
	    ఆపు;

	  స్థితి EOW:	// word\>; rex.input points after d
	    ఒకవేళ (rex.ఆదానం == rex.పంక్తి)    // Can't match at start of line
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే ఒకవేళ (has_mbyte)
	    {
		పూర్ణాంకం this_class, prev_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.ఆదానం, rex.reg_buf);
		prev_class = reg_prev_class();
		ఒకవేళ (this_class == prev_class
			|| prev_class == 0 || prev_class == 1)
		    స్థితి_2 = RA_NOMATCH;
	    }
	    లేకపోతే
	    {
		ఒకవేళ (!vim_iswordc_buf(rex.ఆదానం[-1], rex.reg_buf)
			|| (rex.ఆదానం[0] != NUL
					   && vim_iswordc_buf(c, rex.reg_buf)))
		    స్థితి_2 = RA_NOMATCH;
	    }
	    ఆపు; // Matched with EOW

	  స్థితి ANY:
	    // ANY does not match new lines.
	    ఒకవేళ (c == NUL)
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
		ADVANCE_REGINPUT();
	    ఆపు;

	  స్థితి IDENT:
	    ఒకవేళ (!vim_isIDc(c))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
		ADVANCE_REGINPUT();
	    ఆపు;

	  స్థితి SIDENT:
	    ఒకవేళ (VIM_ISDIGIT(*rex.ఆదానం) || !vim_isIDc(c))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
		ADVANCE_REGINPUT();
	    ఆపు;

	  స్థితి KWORD:
	    ఒకవేళ (!vim_iswordp_buf(rex.ఆదానం, rex.reg_buf))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
		ADVANCE_REGINPUT();
	    ఆపు;

	  స్థితి SKWORD:
	    ఒకవేళ (VIM_ISDIGIT(*rex.ఆదానం)
				    || !vim_iswordp_buf(rex.ఆదానం, rex.reg_buf))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
		ADVANCE_REGINPUT();
	    ఆపు;

	  స్థితి FNAME:
	    ఒకవేళ (!vim_isfilec(c))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
		ADVANCE_REGINPUT();
	    ఆపు;

	  స్థితి SFNAME:
	    ఒకవేళ (VIM_ISDIGIT(*rex.ఆదానం) || !vim_isfilec(c))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
		ADVANCE_REGINPUT();
	    ఆపు;

	  స్థితి PRINT:
	    ఒకవేళ (!vim_isprintc(PTR2CHAR(rex.ఆదానం)))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
		ADVANCE_REGINPUT();
	    ఆపు;

	  స్థితి SPRINT:
	    ఒకవేళ (VIM_ISDIGIT(*rex.ఆదానం) || !vim_isprintc(PTR2CHAR(rex.ఆదానం)))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
		ADVANCE_REGINPUT();
	    ఆపు;

	  స్థితి WHITE:
	    ఒకవేళ (!VIM_ISWHITE(c))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
		ADVANCE_REGINPUT();
	    ఆపు;

	  స్థితి NWHITE:
	    ఒకవేళ (c == NUL || VIM_ISWHITE(c))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
		ADVANCE_REGINPUT();
	    ఆపు;

	  స్థితి DIGIT:
	    ఒకవేళ (!ri_digit(c))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
		ADVANCE_REGINPUT();
	    ఆపు;

	  స్థితి NDIGIT:
	    ఒకవేళ (c == NUL || ri_digit(c))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
		ADVANCE_REGINPUT();
	    ఆపు;

	  స్థితి HEX:
	    ఒకవేళ (!ri_hex(c))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
		ADVANCE_REGINPUT();
	    ఆపు;

	  స్థితి NHEX:
	    ఒకవేళ (c == NUL || ri_hex(c))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
		ADVANCE_REGINPUT();
	    ఆపు;

	  స్థితి OCTAL:
	    ఒకవేళ (!ri_octal(c))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
		ADVANCE_REGINPUT();
	    ఆపు;

	  స్థితి NOCTAL:
	    ఒకవేళ (c == NUL || ri_octal(c))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
		ADVANCE_REGINPUT();
	    ఆపు;

	  స్థితి మాట_2:
	    ఒకవేళ (!ri_మాట(c))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
		ADVANCE_REGINPUT();
	    ఆపు;

	  స్థితి NWORD:
	    ఒకవేళ (c == NUL || ri_మాట(c))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
		ADVANCE_REGINPUT();
	    ఆపు;

	  స్థితి తల_2:
	    ఒకవేళ (!ri_తల(c))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
		ADVANCE_REGINPUT();
	    ఆపు;

	  స్థితి NHEAD:
	    ఒకవేళ (c == NUL || ri_తల(c))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
		ADVANCE_REGINPUT();
	    ఆపు;

	  స్థితి ALPHA:
	    ఒకవేళ (!ri_alpha(c))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
		ADVANCE_REGINPUT();
	    ఆపు;

	  స్థితి NALPHA:
	    ఒకవేళ (c == NUL || ri_alpha(c))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
		ADVANCE_REGINPUT();
	    ఆపు;

	  స్థితి క్రింది_2:
	    ఒకవేళ (!ri_క్రింది(c))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
		ADVANCE_REGINPUT();
	    ఆపు;

	  స్థితి NLOWER:
	    ఒకవేళ (c == NUL || ri_క్రింది(c))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
		ADVANCE_REGINPUT();
	    ఆపు;

	  స్థితి పై:
	    ఒకవేళ (!ri_పై(c))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
		ADVANCE_REGINPUT();
	    ఆపు;

	  స్థితి NUPPER:
	    ఒకవేళ (c == NUL || ri_పై(c))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
		ADVANCE_REGINPUT();
	    ఆపు;

	  స్థితి EXACTLY:
	    {
		పూర్ణాంకం	len;
		char_u	*opnd;

		opnd = నిర్వాహ్యం(పరిశీలన);
		// Inline the first byte, for speed.
		ఒకవేళ (*opnd != *rex.ఆదానం
			&& (!rex.reg_ic
			    || (!enc_utf8
			      && MB_TOLOWER(*opnd) != MB_TOLOWER(*rex.ఆదానం))))
		    స్థితి_2 = RA_NOMATCH;
		లేకపోతే ఒకవేళ (*opnd == NUL)
		{
		    // match empty string always works; happens when "~" is
		    // empty.
		}
		లేకపోతే
		{
		    ఒకవేళ (opnd[1] == NUL && !(enc_utf8 && rex.reg_ic))
		    {
			len = 1;	// matched a single byte above
		    }
		    లేకపోతే
		    {
			// Need to match first byte again for multi-byte.
			len = (పూర్ణాంకం)STRLEN(opnd);
			ఒకవేళ (cstrncmp(opnd, rex.ఆదానం, &len) != 0)
			    స్థితి_2 = RA_NOMATCH;
		    }
		    // Check for following composing character, unless %C
		    // follows (skips over all composing chars).
		    ఒకవేళ (స్థితి_2 != RA_NOMATCH
			    && enc_utf8
			    && UTF_COMPOSINGLIKE(rex.ఆదానం, rex.ఆదానం + len)
			    && !rex.reg_icombine
			    && OP(తదుపరి) != RE_COMPOSING)
		    {
			// raaron: This code makes a composing character get
			// ignored, which is the correct behavior (sometimes)
			// for voweled Hebrew texts.
			స్థితి_2 = RA_NOMATCH;
		    }
		    ఒకవేళ (స్థితి_2 != RA_NOMATCH)
			rex.ఆదానం += len;
		}
	    }
	    ఆపు;

	  స్థితి ANYOF:
	  స్థితి ANYBUT:
	    {
		char_u  *q = నిర్వాహ్యం(పరిశీలన);

		ఒకవేళ (c == NUL)
		    స్థితి_2 = RA_NOMATCH;
		లేకపోతే ఒకవేళ ((cstrchr(q, c) == ఖాళీ) == (op == ANYOF))
		    స్థితి_2 = RA_NOMATCH;
		లేకపోతే
		{
		    // Check following combining characters
		    పూర్ణాంకం	len = 0;
		    పూర్ణాంకం i;

		    ఒకవేళ (enc_utf8)
			len = utfc_ptr2len(q) - utf_ptr2len(q);

		    MB_CPTR_ADV(rex.ఆదానం);
		    MB_CPTR_ADV(q);

		    ఒకవేళ (!enc_utf8 || len == 0)
			ఆపు;

		    కొరకు (i = 0; i < len; ++i)
			ఒకవేళ (q[i] != rex.ఆదానం[i])
			{
			    స్థితి_2 = RA_NOMATCH;
			    ఆపు;
			}
		    rex.ఆదానం += len;
		}
		ఆపు;
	    }

	  స్థితి MULTIBYTECODE:
	    ఒకవేళ (has_mbyte)
	    {
		పూర్ణాంకం	i, len;
		char_u	*opnd;
		పూర్ణాంకం	opndc = 0, inpc;

		opnd = నిర్వాహ్యం(పరిశీలన);
		// Safety check (just in case 'encoding' was changed since
		// compiling the program).
		ఒకవేళ ((len = (*mb_ptr2len)(opnd)) < 2)
		{
		    స్థితి_2 = RA_NOMATCH;
		    ఆపు;
		}
		ఒకవేళ (enc_utf8)
		    opndc = utf_ptr2char(opnd);
		ఒకవేళ (enc_utf8 && utf_iscomposing(opndc))
		{
		    // When only a composing char is given match at any
		    // position where that composing char appears.
		    స్థితి_2 = RA_NOMATCH;
		    కొరకు (i = 0; rex.ఆదానం[i] != NUL;
						i += utf_ptr2len(rex.ఆదానం + i))
		    {
			inpc = utf_ptr2char(rex.ఆదానం + i);
			ఒకవేళ (!utf_iscomposing(inpc))
			{
			    ఒకవేళ (i > 0)
				ఆపు;
			}
			లేకపోతే ఒకవేళ (opndc == inpc)
			{
			    // Include all following composing chars.
			    len = i + utfc_ptr2len(rex.ఆదానం + i);
			    స్థితి_2 = RA_సరిపోలిక;
			    ఆపు;
			}
		    }
		}
		లేకపోతే ఒకవేళ (enc_utf8)
		{
		    ఒకవేళ (cstrncmp(opnd, rex.ఆదానం, &len) != 0)
		    {
			స్థితి_2 = RA_NOMATCH;
			ఆపు;
		    }
		}
		లేకపోతే
		    కొరకు (i = 0; i < len; ++i)
			ఒకవేళ (opnd[i] != rex.ఆదానం[i])
			{
			    స్థితి_2 = RA_NOMATCH;
			    ఆపు;
			}
		rex.ఆదానం += len;
	    }
	    లేకపోతే
		స్థితి_2 = RA_NOMATCH;
	    ఆపు;
	  స్థితి RE_COMPOSING:
	    ఒకవేళ (enc_utf8)
	    {
		// Skip composing characters.
		అయితే (utf_iscomposing(utf_ptr2char(rex.ఆదానం)))
		    MB_CPTR_ADV(rex.ఆదానం);
	    }
	    ఆపు;

	  స్థితి NOTHING:
	    ఆపు;

	  స్థితి వెనుక_2:
	    {
		పూర్ణాంకం		i;
		backpos_T	*bp;

		// When we run into BACK we need to check if we don't keep
		// looping without matching any input.  The second and later
		// times a BACK is encountered it fails if the input is still
		// at the same position as the previous time.
		// The positions are stored in "backpos" and found by the
		// current value of "scan", the position in the RE program.
		bp = (backpos_T *)backpos.ga_దత్తాంశం;
		కొరకు (i = 0; i < backpos.ga_len; ++i)
		    ఒకవేళ (bp[i].bp_పరిశీలన == పరిశీలన)
			ఆపు;
		ఒకవేళ (i == backpos.ga_len)
		{
		    // First time at this BACK, make room to store the pos.
		    ఒకవేళ (GA_GROW_FAILS(&backpos, 1))
			స్థితి_2 = RA_FAIL;
		    లేకపోతే
		    {
			// get "ga_data" again, it may have changed
			bp = (backpos_T *)backpos.ga_దత్తాంశం;
			bp[i].bp_పరిశీలన = పరిశీలన;
			++backpos.ga_len;
		    }
		}
		లేకపోతే ఒకవేళ (reg_save_equal(&bp[i].bp_pos))
		    // Still at same position as last time, fail.
		    స్థితి_2 = RA_NOMATCH;

		ఒకవేళ (స్థితి_2 != RA_FAIL && స్థితి_2 != RA_NOMATCH)
		    reg_save(&bp[i].bp_pos, &backpos);
	    }
	    ఆపు;

	  స్థితి MOPEN + 0:   // Match start: \zs
	  స్థితి MOPEN + 1:   // \(
	  స్థితి MOPEN + 2:
	  స్థితి MOPEN + 3:
	  స్థితి MOPEN + 4:
	  స్థితి MOPEN + 5:
	  స్థితి MOPEN + 6:
	  స్థితి MOPEN + 7:
	  స్థితి MOPEN + 8:
	  స్థితి MOPEN + 9:
	    {
		no = op - MOPEN;
		cleanup_subexpr();
		rp = regstack_నెట్టు(RS_MOPEN, పరిశీలన);
		ఒకవేళ (rp == ఖాళీ)
		    స్థితి_2 = RA_FAIL;
		లేకపోతే
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_startpos[no],
							  &rex.reg_startp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    ఆపు;

	  స్థితి NOPEN:	    // \%(
	  స్థితి NCLOSE:	    // \) after \%(
		ఒకవేళ (regstack_నెట్టు(RS_NOPEN, పరిశీలన) == ఖాళీ)
		    స్థితి_2 = RA_FAIL;
		// We simply continue and handle the result when done.
		ఆపు;

#నిర్వచితమైతే FEAT_SYN_HL
	  స్థితి ZOPEN + 1:
	  స్థితి ZOPEN + 2:
	  స్థితి ZOPEN + 3:
	  స్థితి ZOPEN + 4:
	  స్థితి ZOPEN + 5:
	  స్థితి ZOPEN + 6:
	  స్థితి ZOPEN + 7:
	  స్థితి ZOPEN + 8:
	  స్థితి ZOPEN + 9:
	    {
		no = op - ZOPEN;
		cleanup_zsubexpr();
		rp = regstack_నెట్టు(RS_ZOPEN, పరిశీలన);
		ఒకవేళ (rp == ఖాళీ)
		    స్థితి_2 = RA_FAIL;
		లేకపోతే
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_startzpos[no],
							     &reg_startzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    ఆపు;
#అయితే_ముగింపు

	  స్థితి MCLOSE + 0:  // Match end: \ze
	  స్థితి MCLOSE + 1:  // \)
	  స్థితి MCLOSE + 2:
	  స్థితి MCLOSE + 3:
	  స్థితి MCLOSE + 4:
	  స్థితి MCLOSE + 5:
	  స్థితి MCLOSE + 6:
	  స్థితి MCLOSE + 7:
	  స్థితి MCLOSE + 8:
	  స్థితి MCLOSE + 9:
	    {
		no = op - MCLOSE;
		cleanup_subexpr();
		rp = regstack_నెట్టు(RS_MCLOSE, పరిశీలన);
		ఒకవేళ (rp == ఖాళీ)
		    స్థితి_2 = RA_FAIL;
		లేకపోతే
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_endpos[no],
							    &rex.reg_endp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    ఆపు;

#నిర్వచితమైతే FEAT_SYN_HL
	  స్థితి ZCLOSE + 1:  // \) after \z(
	  స్థితి ZCLOSE + 2:
	  స్థితి ZCLOSE + 3:
	  స్థితి ZCLOSE + 4:
	  స్థితి ZCLOSE + 5:
	  స్థితి ZCLOSE + 6:
	  స్థితి ZCLOSE + 7:
	  స్థితి ZCLOSE + 8:
	  స్థితి ZCLOSE + 9:
	    {
		no = op - ZCLOSE;
		cleanup_zsubexpr();
		rp = regstack_నెట్టు(RS_ZCLOSE, పరిశీలన);
		ఒకవేళ (rp == ఖాళీ)
		    స్థితి_2 = RA_FAIL;
		లేకపోతే
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_endzpos[no],
							      &reg_endzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    ఆపు;
#అయితే_ముగింపు

	  స్థితి BACKREF + 1:
	  స్థితి BACKREF + 2:
	  స్థితి BACKREF + 3:
	  స్థితి BACKREF + 4:
	  స్థితి BACKREF + 5:
	  స్థితి BACKREF + 6:
	  స్థితి BACKREF + 7:
	  స్థితి BACKREF + 8:
	  స్థితి BACKREF + 9:
	    {
		పూర్ణాంకం		len;

		no = op - BACKREF;
		cleanup_subexpr();
		ఒకవేళ (!REG_MULTI)		// Single-line regexp
		{
		    ఒకవేళ (rex.reg_startp[no] == ఖాళీ || rex.reg_endp[no] == ఖాళీ)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    లేకపోతే
		    {
			// Compare current input with back-ref in the same
			// line.
			len = (పూర్ణాంకం)(rex.reg_endp[no] - rex.reg_startp[no]);
			ఒకవేళ (cstrncmp(rex.reg_startp[no], rex.ఆదానం, &len) != 0)
			    స్థితి_2 = RA_NOMATCH;
		    }
		}
		లేకపోతే				// Multi-line regexp
		{
		    ఒకవేళ (rex.reg_startpos[no].lnum < 0
						|| rex.reg_endpos[no].lnum < 0)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    లేకపోతే
		    {
			ఒకవేళ (rex.reg_startpos[no].lnum == rex.lnum
				&& rex.reg_endpos[no].lnum == rex.lnum)
			{
			    // Compare back-ref within the current line.
			    len = rex.reg_endpos[no].నిలువు
						    - rex.reg_startpos[no].నిలువు;
			    ఒకవేళ (cstrncmp(rex.పంక్తి + rex.reg_startpos[no].నిలువు,
							  rex.ఆదానం, &len) != 0)
				స్థితి_2 = RA_NOMATCH;
			}
			లేకపోతే
			{
			    // Messy situation: Need to compare between two
			    // lines.
			    పూర్ణాంకం r = సరిపోలిక_with_backref(
					    rex.reg_startpos[no].lnum,
					    rex.reg_startpos[no].నిలువు,
					    rex.reg_endpos[no].lnum,
					    rex.reg_endpos[no].నిలువు,
					    &len);

			    ఒకవేళ (r != RA_సరిపోలిక)
				స్థితి_2 = r;
			}
		    }
		}

		// Matched the backref, skip over it.
		rex.ఆదానం += len;
	    }
	    ఆపు;

#నిర్వచితమైతే FEAT_SYN_HL
	  స్థితి ZREF + 1:
	  స్థితి ZREF + 2:
	  స్థితి ZREF + 3:
	  స్థితి ZREF + 4:
	  స్థితి ZREF + 5:
	  స్థితి ZREF + 6:
	  స్థితి ZREF + 7:
	  స్థితి ZREF + 8:
	  స్థితి ZREF + 9:
	    {
		పూర్ణాంకం	len;

		cleanup_zsubexpr();
		no = op - ZREF;
		ఒకవేళ (re_extmatch_లోపల != ఖాళీ
			&& re_extmatch_లోపల->matches[no] != ఖాళీ)
		{
		    len = (పూర్ణాంకం)STRLEN(re_extmatch_లోపల->matches[no]);
		    ఒకవేళ (cstrncmp(re_extmatch_లోపల->matches[no],
							  rex.ఆదానం, &len) != 0)
			స్థితి_2 = RA_NOMATCH;
		    లేకపోతే
			rex.ఆదానం += len;
		}
		లేకపోతే
		{
		    // Backref was not set: Match an empty string.
		}
	    }
	    ఆపు;
#అయితే_ముగింపు

	  స్థితి BRANCH:
	    {
		ఒకవేళ (OP(తదుపరి) != BRANCH) // No choice.
		    తదుపరి = నిర్వాహ్యం(పరిశీలన);	// Avoid recursion.
		లేకపోతే
		{
		    rp = regstack_నెట్టు(RS_BRANCH, పరిశీలన);
		    ఒకవేళ (rp == ఖాళీ)
			స్థితి_2 = RA_FAIL;
		    లేకపోతే
			స్థితి_2 = RA_BREAK;	// rest is below
		}
	    }
	    ఆపు;

	  స్థితి BRACE_LIMITS:
	    {
		ఒకవేళ (OP(తదుపరి) == BRACE_SIMPLE)
		{
		    bl_minval = నిర్వాహ్యం_MIN(పరిశీలన);
		    bl_maxval = నిర్వాహ్యం_MAX(పరిశీలన);
		}
		లేకపోతే ఒకవేళ (OP(తదుపరి) >= BRACE_COMPLEX
			&& OP(తదుపరి) < BRACE_COMPLEX + 10)
		{
		    no = OP(తదుపరి) - BRACE_COMPLEX;
		    brace_min[no] = నిర్వాహ్యం_MIN(పరిశీలన);
		    brace_max[no] = నిర్వాహ్యం_MAX(పరిశీలన);
		    brace_లెక్క[no] = 0;
		}
		లేకపోతే
		{
		    అంతర్గత_error("BRACE_LIMITS");
		    స్థితి_2 = RA_FAIL;
		}
	    }
	    ఆపు;

	  స్థితి BRACE_COMPLEX + 0:
	  స్థితి BRACE_COMPLEX + 1:
	  స్థితి BRACE_COMPLEX + 2:
	  స్థితి BRACE_COMPLEX + 3:
	  స్థితి BRACE_COMPLEX + 4:
	  స్థితి BRACE_COMPLEX + 5:
	  స్థితి BRACE_COMPLEX + 6:
	  స్థితి BRACE_COMPLEX + 7:
	  స్థితి BRACE_COMPLEX + 8:
	  స్థితి BRACE_COMPLEX + 9:
	    {
		no = op - BRACE_COMPLEX;
		++brace_లెక్క[no];

		// If not matched enough times yet, try one more
		ఒకవేళ (brace_లెక్క[no] <= (brace_min[no] <= brace_max[no]
					     ? brace_min[no] : brace_max[no]))
		{
		    rp = regstack_నెట్టు(RS_BRCPLX_మరిన్ని, పరిశీలన);
		    ఒకవేళ (rp == ఖాళీ)
			స్థితి_2 = RA_FAIL;
		    లేకపోతే
		    {
			rp->rs_no = no;
			reg_save(&rp->rs_un.regsave, &backpos);
			తదుపరి = నిర్వాహ్యం(పరిశీలన);
			// We continue and handle the result when done.
		    }
		    ఆపు;
		}

		// If matched enough times, may try matching some more
		ఒకవేళ (brace_min[no] <= brace_max[no])
		{
		    // Range is the normal way around, use longest match
		    ఒకవేళ (brace_లెక్క[no] <= brace_max[no])
		    {
			rp = regstack_నెట్టు(RS_BRCPLX_LONG, పరిశీలన);
			ఒకవేళ (rp == ఖాళీ)
			    స్థితి_2 = RA_FAIL;
			లేకపోతే
			{
			    rp->rs_no = no;
			    reg_save(&rp->rs_un.regsave, &backpos);
			    తదుపరి = నిర్వాహ్యం(పరిశీలన);
			    // We continue and handle the result when done.
			}
		    }
		}
		లేకపోతే
		{
		    // Range is backwards, use shortest match first
		    ఒకవేళ (brace_లెక్క[no] <= brace_min[no])
		    {
			rp = regstack_నెట్టు(RS_BRCPLX_SHORT, పరిశీలన);
			ఒకవేళ (rp == ఖాళీ)
			    స్థితి_2 = RA_FAIL;
			లేకపోతే
			{
			    reg_save(&rp->rs_un.regsave, &backpos);
			    // We continue and handle the result when done.
			}
		    }
		}
	    }
	    ఆపు;

	  స్థితి BRACE_SIMPLE:
	  స్థితి నక్షత్రం:
	  స్థితి PLUS:
	    {
		regstar_T	rst;

		// Lookahead to avoid useless match attempts when we know
		// what character comes next.
		ఒకవేళ (OP(తదుపరి) == EXACTLY)
		{
		    rst.nextb = *నిర్వాహ్యం(తదుపరి);
		    ఒకవేళ (rex.reg_ic)
		    {
			ఒకవేళ (MB_ISUPPER(rst.nextb))
			    rst.nextb_ic = MB_TOLOWER(rst.nextb);
			లేకపోతే
			    rst.nextb_ic = MB_TOUPPER(rst.nextb);
		    }
		    లేకపోతే
			rst.nextb_ic = rst.nextb;
		}
		లేకపోతే
		{
		    rst.nextb = NUL;
		    rst.nextb_ic = NUL;
		}
		ఒకవేళ (op != BRACE_SIMPLE)
		{
		    rst.minval = (op == నక్షత్రం) ? 0 : 1;
		    rst.maxval = MAX_హద్దు;
		}
		లేకపోతే
		{
		    rst.minval = bl_minval;
		    rst.maxval = bl_maxval;
		}

		// When maxval > minval, try matching as much as possible, up
		// to maxval.  When maxval < minval, try matching at least the
		// minimal number (since the range is backwards, that's also
		// maxval!).
		rst.లెక్క = regrepeat(నిర్వాహ్యం(పరిశీలన), rst.maxval);
		ఒకవేళ (got_int)
		{
		    స్థితి_2 = RA_FAIL;
		    ఆపు;
		}
		ఒకవేళ (rst.minval <= rst.maxval
			  ? rst.లెక్క >= rst.minval : rst.లెక్క >= rst.maxval)
		{
		    // It could match.  Prepare for trying to match what
		    // follows.  The code is below.  Parameters are stored in
		    // a regstar_T on the regstack.
		    ఒకవేళ ((దీర్ఘ)((గుర్తులేని)regstack.ga_len >> 10) >= p_mmp)
		    {
			emsg(_(e_నమూనా_ఉపయోగిస్తుంది_మరిన్ని_జ్ఞాపకం_than_maxmempattern));
			స్థితి_2 = RA_FAIL;
		    }
		    లేకపోతే ఒకవేళ (GA_GROW_FAILS(&regstack, (పూర్ణాంకం)పరిమాణం(regstar_T)))
			స్థితి_2 = RA_FAIL;
		    లేకపోతే
		    {
			regstack.ga_len += పరిమాణం(regstar_T);
			rp = regstack_నెట్టు(rst.minval <= rst.maxval
					? RS_నక్షత్రం_LONG : RS_నక్షత్రం_SHORT, పరిశీలన);
			ఒకవేళ (rp == ఖాళీ)
			    స్థితి_2 = RA_FAIL;
			లేకపోతే
			{
			    *(((regstar_T *)rp) - 1) = rst;
			    స్థితి_2 = RA_BREAK;	    // skip the restore bits
			}
		    }
		}
		లేకపోతే
		    స్థితి_2 = RA_NOMATCH;

	    }
	    ఆపు;

	  స్థితి NOMATCH:
	  స్థితి సరిపోలిక_2:
	  స్థితి SUBPAT:
	    rp = regstack_నెట్టు(RS_NOMATCH, పరిశీలన);
	    ఒకవేళ (rp == ఖాళీ)
		స్థితి_2 = RA_FAIL;
	    లేకపోతే
	    {
		rp->rs_no = op;
		reg_save(&rp->rs_un.regsave, &backpos);
		తదుపరి = నిర్వాహ్యం(పరిశీలన);
		// We continue and handle the result when done.
	    }
	    ఆపు;

	  స్థితి BEHIND:
	  స్థితి NOBEHIND:
	    // Need a bit of room to store extra positions.
	    ఒకవేళ ((దీర్ఘ)((గుర్తులేని)regstack.ga_len >> 10) >= p_mmp)
	    {
		emsg(_(e_నమూనా_ఉపయోగిస్తుంది_మరిన్ని_జ్ఞాపకం_than_maxmempattern));
		స్థితి_2 = RA_FAIL;
	    }
	    లేకపోతే ఒకవేళ (GA_GROW_FAILS(&regstack, (పూర్ణాంకం)పరిమాణం(regbehind_T)))
		స్థితి_2 = RA_FAIL;
	    లేకపోతే
	    {
		regstack.ga_len += పరిమాణం(regbehind_T);
		rp = regstack_నెట్టు(RS_BEHIND1, పరిశీలన);
		ఒకవేళ (rp == ఖాళీ)
		    స్థితి_2 = RA_FAIL;
		లేకపోతే
		{
		    // Need to save the subexpr to be able to restore them
		    // when there is a match but we don't use it.
		    save_subexpr(((regbehind_T *)rp) - 1);

		    rp->rs_no = op;
		    reg_save(&rp->rs_un.regsave, &backpos);
		    // First try if what follows matches.  If it does then we
		    // check the behind match by looping.
		}
	    }
	    ఆపు;

	  స్థితి BHPOS:
	    ఒకవేళ (REG_MULTI)
	    {
		ఒకవేళ (behind_pos.rs_u.pos.నిలువు != (colnr_T)(rex.ఆదానం - rex.పంక్తి)
			|| behind_pos.rs_u.pos.lnum != rex.lnum)
		    స్థితి_2 = RA_NOMATCH;
	    }
	    లేకపోతే ఒకవేళ (behind_pos.rs_u.ptr != rex.ఆదానం)
		స్థితి_2 = RA_NOMATCH;
	    ఆపు;

	  స్థితి NEWL:
	    ఒకవేళ ((c != NUL || !REG_MULTI || rex.lnum > rex.reg_maxline
			     || rex.reg_పంక్తి_lbr)
					   && (c != '\n' || !rex.reg_పంక్తి_lbr))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే ఒకవేళ (rex.reg_పంక్తి_lbr)
		ADVANCE_REGINPUT();
	    లేకపోతే
		reg_nextline();
	    ఆపు;

	  స్థితి చివర_3:
	    స్థితి_2 = RA_సరిపోలిక;	// Success!
	    ఆపు;

	  అప్రమేయ:
	    iemsg(e_corrupted_regexp_కార్యక్రమం);
#నిర్వచితమైతే DEBUG
	    రాయి("Illegal op code %d\n", op);
#అయితే_ముగింపు
	    స్థితి_2 = RA_FAIL;
	    ఆపు;
	  }
	}

	// If we can't continue sequentially, break the inner loop.
	ఒకవేళ (స్థితి_2 != RA_CONT)
	    ఆపు;

	// Continue in inner loop, advance to next item.
	పరిశీలన = తదుపరి;

    } // end of inner loop

    // If there is something on the regstack execute the code for the state.
    // If the state is popped then loop and use the older state.
    అయితే (regstack.ga_len > 0 && స్థితి_2 != RA_FAIL)
    {
	rp = (regitem_T *)((అక్షరం *)regstack.ga_దత్తాంశం + regstack.ga_len) - 1;
	ఎంపిక (rp->rs_state)
	{
	  స్థితి RS_NOPEN:
	    // Result is passed on as-is, simply pop the state.
	    regstack_తీయి(&పరిశీలన);
	    ఆపు;

	  స్థితి RS_MOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    ఒకవేళ (స్థితి_2 == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_startpos[rp->rs_no],
						  &rex.reg_startp[rp->rs_no]);
	    regstack_తీయి(&పరిశీలన);
	    ఆపు;

#నిర్వచితమైతే FEAT_SYN_HL
	  స్థితి RS_ZOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    ఒకవేళ (స్థితి_2 == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_startzpos[rp->rs_no],
						 &reg_startzp[rp->rs_no]);
	    regstack_తీయి(&పరిశీలన);
	    ఆపు;
#అయితే_ముగింపు

	  స్థితి RS_MCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    ఒకవేళ (స్థితి_2 == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_endpos[rp->rs_no],
						    &rex.reg_endp[rp->rs_no]);
	    regstack_తీయి(&పరిశీలన);
	    ఆపు;

#నిర్వచితమైతే FEAT_SYN_HL
	  స్థితి RS_ZCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    ఒకవేళ (స్థితి_2 == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_endzpos[rp->rs_no],
						   &reg_endzp[rp->rs_no]);
	    regstack_తీయి(&పరిశీలన);
	    ఆపు;
#అయితే_ముగింపు

	  స్థితి RS_BRANCH:
	    ఒకవేళ (స్థితి_2 == RA_సరిపోలిక)
		// this branch matched, use it
		regstack_తీయి(&పరిశీలన);
	    లేకపోతే
	    {
		ఒకవేళ (స్థితి_2 != RA_BREAK)
		{
		    // After a non-matching branch: try next one.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    పరిశీలన = rp->rs_పరిశీలన;
		}
		ఒకవేళ (పరిశీలన == ఖాళీ || OP(పరిశీలన) != BRANCH)
		{
		    // no more branches, didn't find a match
		    స్థితి_2 = RA_NOMATCH;
		    regstack_తీయి(&పరిశీలన);
		}
		లేకపోతే
		{
		    // Prepare to try a branch.
		    rp->rs_పరిశీలన = regnext(పరిశీలన);
		    reg_save(&rp->rs_un.regsave, &backpos);
		    పరిశీలన = నిర్వాహ్యం(పరిశీలన);
		}
	    }
	    ఆపు;

	  స్థితి RS_BRCPLX_మరిన్ని:
	    // Pop the state.  Restore pointers when there is no match.
	    ఒకవేళ (స్థితి_2 == RA_NOMATCH)
	    {
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_లెక్క[rp->rs_no];	// decrement match count
	    }
	    regstack_తీయి(&పరిశీలన);
	    ఆపు;

	  స్థితి RS_BRCPLX_LONG:
	    // Pop the state.  Restore pointers when there is no match.
	    ఒకవేళ (స్థితి_2 == RA_NOMATCH)
	    {
		// There was no match, but we did find enough matches.
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_లెక్క[rp->rs_no];
		// continue with the items after "\{}"
		స్థితి_2 = RA_CONT;
	    }
	    regstack_తీయి(&పరిశీలన);
	    ఒకవేళ (స్థితి_2 == RA_CONT)
		పరిశీలన = regnext(పరిశీలన);
	    ఆపు;

	  స్థితి RS_BRCPLX_SHORT:
	    // Pop the state.  Restore pointers when there is no match.
	    ఒకవేళ (స్థితి_2 == RA_NOMATCH)
		// There was no match, try to match one more item.
		reg_restore(&rp->rs_un.regsave, &backpos);
	    regstack_తీయి(&పరిశీలన);
	    ఒకవేళ (స్థితి_2 == RA_NOMATCH)
	    {
		పరిశీలన = నిర్వాహ్యం(పరిశీలన);
		స్థితి_2 = RA_CONT;
	    }
	    ఆపు;

	  స్థితి RS_NOMATCH:
	    // Pop the state.  If the operand matches for NOMATCH or
	    // doesn't match for MATCH/SUBPAT, we fail.  Otherwise backup,
	    // except for SUBPAT, and continue with the next item.
	    ఒకవేళ (స్థితి_2 == (rp->rs_no == NOMATCH ? RA_సరిపోలిక : RA_NOMATCH))
		స్థితి_2 = RA_NOMATCH;
	    లేకపోతే
	    {
		స్థితి_2 = RA_CONT;
		ఒకవేళ (rp->rs_no != SUBPAT)	// zero-width
		    reg_restore(&rp->rs_un.regsave, &backpos);
	    }
	    regstack_తీయి(&పరిశీలన);
	    ఒకవేళ (స్థితి_2 == RA_CONT)
		పరిశీలన = regnext(పరిశీలన);
	    ఆపు;

	  స్థితి RS_BEHIND1:
	    ఒకవేళ (స్థితి_2 == RA_NOMATCH)
	    {
		regstack_తీయి(&పరిశీలన);
		regstack.ga_len -= పరిమాణం(regbehind_T);
	    }
	    లేకపోతే
	    {
		// The stuff after BEHIND/NOBEHIND matches.  Now try if
		// the behind part does (not) match before the current
		// position in the input.  This must be done at every
		// position in the input and checking if the match ends at
		// the current position.

		// save the position after the found match for next
		reg_save(&(((regbehind_T *)rp) - 1)->save_after, &backpos);

		// Start looking for a match with operand at the current
		// position.  Go back one character until we find the
		// result, hitting the start of the line or the previous
		// line (for multi-line matching).
		// Set behind_pos to where the match should end, BHPOS
		// will match it.  Save the current value.
		(((regbehind_T *)rp) - 1)->save_behind = behind_pos;
		behind_pos = rp->rs_un.regsave;

		rp->rs_state = RS_BEHIND2;

		reg_restore(&rp->rs_un.regsave, &backpos);
		పరిశీలన = నిర్వాహ్యం(rp->rs_పరిశీలన) + 4;
	    }
	    ఆపు;

	  స్థితి RS_BEHIND2:
	    // Looping for BEHIND / NOBEHIND match.
	    ఒకవేళ (స్థితి_2 == RA_సరిపోలిక && reg_save_equal(&behind_pos))
	    {
		// found a match that ends where "next" started
		behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		ఒకవేళ (rp->rs_no == BEHIND)
		    reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
		లేకపోతే
		{
		    // But we didn't want a match.  Need to restore the
		    // subexpr, because what follows matched, so they have
		    // been set.
		    స్థితి_2 = RA_NOMATCH;
		    restore_subexpr(((regbehind_T *)rp) - 1);
		}
		regstack_తీయి(&పరిశీలన);
		regstack.ga_len -= పరిమాణం(regbehind_T);
	    }
	    లేకపోతే
	    {
		దీర్ఘ హద్దు;

		// No match or a match that doesn't end where we want it: Go
		// back one character.  May go to previous line once.
		no = OK;
		హద్దు = నిర్వాహ్యం_MIN(rp->rs_పరిశీలన);
		ఒకవేళ (REG_MULTI)
		{
		    ఒకవేళ (హద్దు > 0
			    && ((rp->rs_un.regsave.rs_u.pos.lnum
						    < behind_pos.rs_u.pos.lnum
				    ? (colnr_T)STRLEN(rex.పంక్తి)
				    : behind_pos.rs_u.pos.నిలువు)
				- rp->rs_un.regsave.rs_u.pos.నిలువు >= హద్దు))
			no = FAIL;
		    లేకపోతే ఒకవేళ (rp->rs_un.regsave.rs_u.pos.నిలువు == 0)
		    {
			ఒకవేళ (rp->rs_un.regsave.rs_u.pos.lnum
					< behind_pos.rs_u.pos.lnum
				|| reg_getline(
					--rp->rs_un.regsave.rs_u.pos.lnum)
								  == ఖాళీ)
			    no = FAIL;
			లేకపోతే
			{
			    reg_restore(&rp->rs_un.regsave, &backpos);
			    rp->rs_un.regsave.rs_u.pos.నిలువు =
						 (colnr_T)STRLEN(rex.పంక్తి);
			}
		    }
		    లేకపోతే
		    {
			ఒకవేళ (has_mbyte)
			{
			    char_u *పంక్తి =
				  reg_getline(rp->rs_un.regsave.rs_u.pos.lnum);

			    rp->rs_un.regsave.rs_u.pos.నిలువు -=
				(*mb_తల_off)(పంక్తి, పంక్తి
				    + rp->rs_un.regsave.rs_u.pos.నిలువు - 1) + 1;
			}
			లేకపోతే
			    --rp->rs_un.regsave.rs_u.pos.నిలువు;
		    }
		}
		లేకపోతే
		{
		    ఒకవేళ (rp->rs_un.regsave.rs_u.ptr == rex.పంక్తి)
			no = FAIL;
		    లేకపోతే
		    {
			MB_PTR_వెనుక(rex.పంక్తి, rp->rs_un.regsave.rs_u.ptr);
			ఒకవేళ (హద్దు > 0 && (దీర్ఘ)(behind_pos.rs_u.ptr
				     - rp->rs_un.regsave.rs_u.ptr) > హద్దు)
			    no = FAIL;
		    }
		}
		ఒకవేళ (no == OK)
		{
		    // Advanced, prepare for finding match again.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    పరిశీలన = నిర్వాహ్యం(rp->rs_పరిశీలన) + 4;
		    ఒకవేళ (స్థితి_2 == RA_సరిపోలిక)
		    {
			// We did match, so subexpr may have been changed,
			// need to restore them for the next try.
			స్థితి_2 = RA_NOMATCH;
			restore_subexpr(((regbehind_T *)rp) - 1);
		    }
		}
		లేకపోతే
		{
		    // Can't advance.  For NOBEHIND that's a match.
		    behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		    ఒకవేళ (rp->rs_no == NOBEHIND)
		    {
			reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
			స్థితి_2 = RA_సరిపోలిక;
		    }
		    లేకపోతే
		    {
			// We do want a proper match.  Need to restore the
			// subexpr if we had a match, because they may have
			// been set.
			ఒకవేళ (స్థితి_2 == RA_సరిపోలిక)
			{
			    స్థితి_2 = RA_NOMATCH;
			    restore_subexpr(((regbehind_T *)rp) - 1);
			}
		    }
		    regstack_తీయి(&పరిశీలన);
		    regstack.ga_len -= పరిమాణం(regbehind_T);
		}
	    }
	    ఆపు;

	  స్థితి RS_నక్షత్రం_LONG:
	  స్థితి RS_నక్షత్రం_SHORT:
	    {
		regstar_T	    *rst = ((regstar_T *)rp) - 1;

		ఒకవేళ (స్థితి_2 == RA_సరిపోలిక)
		{
		    regstack_తీయి(&పరిశీలన);
		    regstack.ga_len -= పరిమాణం(regstar_T);
		    ఆపు;
		}

		// Tried once already, restore input pointers.
		ఒకవేళ (స్థితి_2 != RA_BREAK)
		    reg_restore(&rp->rs_un.regsave, &backpos);

		// Repeat until we found a position where it could match.
		కొరకు (;;)
		{
		    ఒకవేళ (స్థితి_2 != RA_BREAK)
		    {
			// Tried first position already, advance.
			ఒకవేళ (rp->rs_state == RS_నక్షత్రం_LONG)
			{
			    // Trying for longest match, but couldn't or
			    // didn't match -- back up one char.
			    ఒకవేళ (--rst->లెక్క < rst->minval)
				ఆపు;
			    ఒకవేళ (rex.ఆదానం == rex.పంక్తి)
			    {
				// backup to last char of previous line
				ఒకవేళ (rex.lnum == 0)
				{
				    స్థితి_2 = RA_NOMATCH;
				    ఆపు;
				}
				--rex.lnum;
				rex.పంక్తి = reg_getline(rex.lnum);
				// Just in case regrepeat() didn't count
				// right.
				ఒకవేళ (rex.పంక్తి == ఖాళీ)
				    ఆపు;
				rex.ఆదానం = rex.పంక్తి + reg_getline_len(rex.lnum);
				fast_breakcheck();
			    }
			    లేకపోతే
				MB_PTR_వెనుక(rex.పంక్తి, rex.ఆదానం);
			}
			లేకపోతే
			{
			    // Range is backwards, use shortest match first.
			    // Careful: maxval and minval are exchanged!
			    // Couldn't or didn't match: try advancing one
			    // char.
			    ఒకవేళ (rst->లెక్క == rst->minval
				  || regrepeat(నిర్వాహ్యం(rp->rs_పరిశీలన), 1L) == 0)
				ఆపు;
			    ++rst->లెక్క;
			}
			ఒకవేళ (got_int)
			    ఆపు;
		    }
		    లేకపోతే
			స్థితి_2 = RA_NOMATCH;

		    // If it could match, try it.
		    ఒకవేళ (rst->nextb == NUL || *rex.ఆదానం == rst->nextb
					     || *rex.ఆదానం == rst->nextb_ic)
		    {
			reg_save(&rp->rs_un.regsave, &backpos);
			పరిశీలన = regnext(rp->rs_పరిశీలన);
			స్థితి_2 = RA_CONT;
			ఆపు;
		    }
		}
		ఒకవేళ (స్థితి_2 != RA_CONT)
		{
		    // Failed.
		    regstack_తీయి(&పరిశీలన);
		    regstack.ga_len -= పరిమాణం(regstar_T);
		    స్థితి_2 = RA_NOMATCH;
		}
	    }
	    ఆపు;
	}

	// If we want to continue the inner loop or didn't pop a state
	// continue matching loop
	ఒకవేళ (స్థితి_2 == RA_CONT || rp == (regitem_T *)
			     ((అక్షరం *)regstack.ga_దత్తాంశం + regstack.ga_len) - 1)
	    ఆపు;

#నిర్వచితమైతే FEAT_RELTIME
	ఒకవేళ (bt_did_సమయం_బహిర్గతం(timed_బహిర్గతం))
	{
	    స్థితి_2 = RA_FAIL;
	    ఆపు;
	}
#అయితే_ముగింపు
    }

    // May need to continue with the inner loop, starting at "scan".
    ఒకవేళ (స్థితి_2 == RA_CONT)
	కొనసాగు;

    // If the regstack is empty or something failed we are done.
    ఒకవేళ (regstack.ga_len == 0 || స్థితి_2 == RA_FAIL)
    {
	ఒకవేళ (పరిశీలన == ఖాళీ)
	{
	    // We get here only if there's trouble -- normally "case END" is
	    // the terminating point.
	    iemsg(e_corrupted_regexp_కార్యక్రమం);
#నిర్వచితమైతే DEBUG
	    రాయి("Premature EOL\n");
#అయితే_ముగింపు
	}
	తిరిగి (స్థితి_2 == RA_సరిపోలిక);
    }

  } // End of loop until the regstack is empty.

  // NOTREACHED
}

/*
 * regtry - try match of "prog" with at rex.line["col"].
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    స్థిర దీర్ఘ
regtry(
    bt_regprog_T	*prog,
    colnr_T		నిలువు,
    పూర్ణాంకం			*timed_బహిర్గతం)	// flag set on timeout or NULL
{
    rex.ఆదానం = rex.పంక్తి + నిలువు;
    rex.need_clear_subexpr = TRUE;
#నిర్వచితమైతే FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    rex.need_clear_zsubexpr = (prog->reghasz == REX_సమితి);
#అయితే_ముగింపు

    ఒకవేళ (regmatch(prog->కార్యక్రమం + 1, timed_బహిర్గతం) == 0)
	తిరిగి 0;

    cleanup_subexpr();
    ఒకవేళ (REG_MULTI)
    {
	ఒకవేళ (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].నిలువు = నిలువు;
	}
	ఒకవేళ (rex.reg_endpos[0].lnum < 0)
	{
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].నిలువు = (పూర్ణాంకం)(rex.ఆదానం - rex.పంక్తి);
	}
	లేకపోతే
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    లేకపోతే
    {
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
	పూర్ణాంకం		i;

	cleanup_zsubexpr();
	re_extmatch_బహిర్గతం = make_extmatch();
	ఒకవేళ (re_extmatch_బహిర్గతం == ఖాళీ)
	    తిరిగి 0;
	కొరకు (i = 0; i < NSUBEXP; i++)
	{
	    ఒకవేళ (REG_MULTI)
	    {
		// Only accept single line matches.
		ఒకవేళ (reg_startzpos[i].lnum >= 0
			&& reg_endzpos[i].lnum == reg_startzpos[i].lnum
			&& reg_endzpos[i].నిలువు >= reg_startzpos[i].నిలువు)
		    re_extmatch_బహిర్గతం->matches[i] =
			vim_strnsave(reg_getline(reg_startzpos[i].lnum)
						       + reg_startzpos[i].నిలువు,
				   reg_endzpos[i].నిలువు - reg_startzpos[i].నిలువు);
	    }
	    లేకపోతే
	    {
		ఒకవేళ (reg_startzp[i] != ఖాళీ && reg_endzp[i] != ఖాళీ)
		    re_extmatch_బహిర్గతం->matches[i] =
			    vim_strnsave(reg_startzp[i],
						reg_endzp[i] - reg_startzp[i]);
	    }
	}
    }
#అయితే_ముగింపు
    తిరిగి 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    స్థిర దీర్ఘ
bt_regexec_both(
    char_u	*పంక్తి,
    colnr_T	startcol,	// column to start looking for match
    పూర్ణాంకం		*timed_బహిర్గతం)	// flag set on timeout or NULL
{
    bt_regprog_T    *prog;
    char_u	    *s;
    colnr_T	    నిలువు = startcol;
    దీర్ఘ	    retval = 0L;

    // Create "regstack" and "backpos" if they are not allocated yet.
    // We allocate *_INITIAL amount of bytes first and then set the grow size
    // to much bigger value to avoid many malloc calls in case of deep regular
    // expressions.
    ఒకవేళ (regstack.ga_దత్తాంశం == ఖాళీ)
    {
	// Use an item size of 1 byte, since we push different things
	// onto the regstack.
	ga_init2(&regstack, 1, REGSTACK_INITIAL);
	(శూన్య)ga_grow(&regstack, REGSTACK_INITIAL);
	regstack.ga_growsize = REGSTACK_INITIAL * 8;
    }

    ఒకవేళ (backpos.ga_దత్తాంశం == ఖాళీ)
    {
	ga_init2(&backpos, పరిమాణం(backpos_T), BACKPOS_INITIAL);
	(శూన్య)ga_grow(&backpos, BACKPOS_INITIAL);
	backpos.ga_growsize = BACKPOS_INITIAL * 8;
    }

    ఒకవేళ (REG_MULTI)
    {
	prog = (bt_regprog_T *)rex.reg_mmatch->regprog;
	పంక్తి = reg_getline((linenr_T)0);
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    లేకపోతే
    {
	prog = (bt_regprog_T *)rex.reg_సరిపోలిక->regprog;
	rex.reg_startp = rex.reg_సరిపోలిక->startp;
	rex.reg_endp = rex.reg_సరిపోలిక->endp;
    }

    // Be paranoid...
    ఒకవేళ (prog == ఖాళీ || పంక్తి == ఖాళీ)
    {
	iemsg(e_null_వాదం);
	వెళ్ళు theend;
    }

    // Check validity of program.
    ఒకవేళ (prog_magic_wrong())
	వెళ్ళు theend;

    // If the start column is past the maximum column: no need to try.
    ఒకవేళ (rex.reg_maxcol > 0 && నిలువు >= rex.reg_maxcol)
	వెళ్ళు theend;

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    ఒకవేళ (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    లేకపోతే ఒకవేళ (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    ఒకవేళ (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    // If there is a "must appear" string, look for it.
    ఒకవేళ (prog->regmust != ఖాళీ)
    {
	పూర్ణాంకం c;

	ఒకవేళ (has_mbyte)
	    c = (*mb_ptr2char)(prog->regmust);
	లేకపోతే
	    c = *prog->regmust;
	s = పంక్తి + నిలువు;

	// This is used very often, esp. for ":global".  Use three versions of
	// the loop to avoid overhead of conditions.
	ఒకవేళ (!rex.reg_ic && !has_mbyte)
	    అయితే ((s = vim_strbyte(s, c)) != ఖాళీ)
	    {
		పూర్ణాంకం len = prog->regmlen;
		ఒకవేళ (cstrncmp(s, prog->regmust, &len) == 0)
		    ఆపు;		// Found it.
		++s;
	    }
	లేకపోతే ఒకవేళ (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	    అయితే ((s = vim_strchr(s, c)) != ఖాళీ)
	    {
		పూర్ణాంకం len = prog->regmlen;
		ఒకవేళ (cstrncmp(s, prog->regmust, &len) == 0)
		    ఆపు;		// Found it.
		MB_PTR_ADV(s);
	    }
	లేకపోతే
	    అయితే ((s = cstrchr(s, c)) != ఖాళీ)
	    {
		పూర్ణాంకం len = prog->regmlen;
		ఒకవేళ (cstrncmp(s, prog->regmust, &len) == 0)
		    ఆపు;		// Found it.
		MB_PTR_ADV(s);
	    }
	ఒకవేళ (s == ఖాళీ)		// Not present.
	    వెళ్ళు theend;
    }

    rex.పంక్తి = పంక్తి;
    rex.lnum = 0;
    reg_toolong = FALSE;

    // Simplest case: Anchored match need be tried only once.
    ఒకవేళ (prog->reganch)
    {
	పూర్ణాంకం	c;

	ఒకవేళ (has_mbyte)
	    c = (*mb_ptr2char)(rex.పంక్తి + నిలువు);
	లేకపోతే
	    c = rex.పంక్తి[నిలువు];
	ఒకవేళ (prog->regstart == NUL
		|| prog->regstart == c
		|| (rex.reg_ic
		    && (((enc_utf8 && utf_fold(prog->regstart) == utf_fold(c)))
			|| (c < 255 && prog->regstart < 255 &&
			    MB_TOLOWER(prog->regstart) == MB_TOLOWER(c)))))
	    retval = regtry(prog, నిలువు, timed_బహిర్గతం);
	లేకపోతే
	    retval = 0;
    }
    లేకపోతే
    {
	// Messy cases:  unanchored match.
	అయితే (!got_int)
	{
	    ఒకవేళ (prog->regstart != NUL)
	    {
		// Skip until the char we know it must start with.
		// Used often, do some work to avoid call overhead.
		ఒకవేళ (!rex.reg_ic && !has_mbyte)
		    s = vim_strbyte(rex.పంక్తి + నిలువు, prog->regstart);
		లేకపోతే
		    s = cstrchr(rex.పంక్తి + నిలువు, prog->regstart);
		ఒకవేళ (s == ఖాళీ)
		{
		    retval = 0;
		    ఆపు;
		}
		నిలువు = (పూర్ణాంకం)(s - rex.పంక్తి);
	    }

	    // Check for maximum column to try.
	    ఒకవేళ (rex.reg_maxcol > 0 && నిలువు >= rex.reg_maxcol)
	    {
		retval = 0;
		ఆపు;
	    }

	    retval = regtry(prog, నిలువు, timed_బహిర్గతం);
	    ఒకవేళ (retval > 0)
		ఆపు;

	    // if not currently on the first line, get it again
	    ఒకవేళ (rex.lnum != 0)
	    {
		rex.lnum = 0;
		rex.పంక్తి = reg_getline((linenr_T)0);
	    }
	    ఒకవేళ (rex.పంక్తి[నిలువు] == NUL)
		ఆపు;
	    ఒకవేళ (has_mbyte)
		నిలువు += (*mb_ptr2len)(rex.పంక్తి + నిలువు);
	    లేకపోతే
		++నిలువు;
#నిర్వచితమైతే FEAT_RELTIME
	    ఒకవేళ (bt_did_సమయం_బహిర్గతం(timed_బహిర్గతం))
		ఆపు;
#అయితే_ముగింపు
	}
    }

theend:
    // Free "reg_tofree" when it's a bit big.
    // Free regstack and backpos if they are bigger than their initial size.
    ఒకవేళ (reg_tofreelen > 400)
	VIM_CLEAR(reg_tofree);
    ఒకవేళ (regstack.ga_maxlen > REGSTACK_INITIAL)
	ga_clear(&regstack);
    ఒకవేళ (backpos.ga_maxlen > BACKPOS_INITIAL)
	ga_clear(&backpos);

    ఒకవేళ (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs
	// and \ze are used.
	ఒకవేళ (REG_MULTI)
	{
	    lpos_T *ప్రారంభం = &rex.reg_mmatch->startpos[0];
	    lpos_T *చివర = &rex.reg_mmatch->endpos[0];

	    ఒకవేళ (చివర->lnum < ప్రారంభం->lnum
			|| (చివర->lnum == ప్రారంభం->lnum && చివర->నిలువు < ప్రారంభం->నిలువు))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_mmatch->rmm_matchcol = నిలువు;
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
 * Match a regexp against a string.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 * if "line_lbr" is TRUE  consider a "\n" in "line" to be a line break.
 *
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    స్థిర పూర్ణాంకం
bt_regexec_nl(
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

    తిరిగి bt_regexec_both(పంక్తి, నిలువు, ఖాళీ);
}

/*
 * Match a regexp against multiple lines.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 *
 * Return zero if there is no match.  Return number of lines contained in the
 * match otherwise.
 */
    స్థిర దీర్ఘ
bt_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	నిలువు,		// column to start looking for match
    పూర్ణాంకం		*timed_బహిర్గతం)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    తిరిగి bt_regexec_both(ఖాళీ, నిలువు, timed_బహిర్గతం);
}

/*
 * Compare a number with the operand of RE_LNUM, RE_COL or RE_VCOL.
 */
    స్థిర పూర్ణాంకం
re_అంకె_cmp(long_u val, char_u *పరిశీలన)
{
    long_u  n = నిర్వాహ్యం_MIN(పరిశీలన);

    ఒకవేళ (నిర్వాహ్యం_CMP(పరిశీలన) == '>')
	తిరిగి val > n;
    ఒకవేళ (నిర్వాహ్యం_CMP(పరిశీలన) == '<')
	తిరిగి val < n;
    తిరిగి val == n;
}

#నిర్వచితమైతే BT_REGEXP_DUMP

/*
 * regdump - dump a regexp onto stdout in vaguely comprehensible form
 */
    స్థిర శూన్య
regdump(char_u *నమూనా, bt_regprog_T *r)
{
    char_u  *s;
    పూర్ణాంకం	    op = EXACTLY;	// Arbitrary non-END op.
    char_u  *తదుపరి;
    char_u  *చివర = ఖాళీ;
    FILE    *f;

# ifdef BT_REGEXP_LOG
    f = fopen("bt_regexp_log.log", "a");
# లేకపోతే
    f = stdout;
# endif
    ఒకవేళ (f == ఖాళీ)
	తిరిగి;
    fprintf(f, "-------------------------------------\n\r\nregcomp(%s):\r\n", నమూనా);

    s = r->కార్యక్రమం + 1;
    // Loop until we find the END that isn't before a referred next (an END
    // can also appear in a NOMATCH operand).
    అయితే ((op != చివర_3 || s <= చివర) && s < r->కార్యక్రమం + r->regsz)
    {
	op = OP(s);
	fprintf(f, "%2d%s", (పూర్ణాంకం)(s - r->కార్యక్రమం), regprop(s)); // Where, what.
	తదుపరి = (s + 3 <= r->కార్యక్రమం + r->regsz) ? regnext(s) : ఖాళీ;
	ఒకవేళ (తదుపరి == ఖాళీ)	// Next ptr.
	    fprintf(f, "(0)");
	లేకపోతే
	    fprintf(f, "(%d)", (పూర్ణాంకం)((s - r->కార్యక్రమం) + (తదుపరి - s)));
	ఒకవేళ (చివర < తదుపరి)
	    చివర = తదుపరి;
	ఒకవేళ (op == BRACE_LIMITS)
	{
	    // Two ints
	    fprintf(f, " minval %ld, maxval %ld", నిర్వాహ్యం_MIN(s), నిర్వాహ్యం_MAX(s));
	    s += 8;
	}
	లేకపోతే ఒకవేళ (op == BEHIND || op == NOBEHIND)
	{
	    // one int
	    fprintf(f, " count %ld", నిర్వాహ్యం_MIN(s));
	    s += 4;
	}
	లేకపోతే ఒకవేళ (op == RE_LNUM || op == RE_నిలువు || op == RE_VCOL)
	{
	    // one int plus comparator
	    fprintf(f, " count %ld", నిర్వాహ్యం_MIN(s));
	    s += 5;
	}
	s += 3;
	ఒకవేళ (op == MULTIBYTECODE)
	{
	    fprintf(f, " mbc=%d", utf_ptr2char(s));
	    s += utfc_ptr2len(s);
	}
	ఒకవేళ (op == ANYOF || op == ANYOF + కూడు_NL
		|| op == ANYBUT || op == ANYBUT + కూడు_NL
		|| op == EXACTLY)
	{
	    // Literal string, where present.
	    fprintf(f, "\nxxxxxxxxx\n");
	    అయితే (*s != NUL && s < r->కార్యక్రమం + r->regsz)
	    {
		fprintf(f, "%c", *s);
		s += utfc_ptr2len(s);  // advance by full char including combining
	    }
	    fprintf(f, "\nxxxxxxxxx\n");
	    s++;
	}
	fprintf(f, "\r\n");
    }

    // Header fields of interest.
    ఒకవేళ (r->regstart != NUL)
	fprintf(f, "start `%s' 0x%x; ", r->regstart < 256
		? (అక్షరం *)transchar(r->regstart)
		: "multibyte", r->regstart);
    ఒకవేళ (r->reganch)
	fprintf(f, "anchored; ");
    ఒకవేళ (r->regmust != ఖాళీ)
	fprintf(f, "must have \"%s\"", r->regmust);
    fprintf(f, "\r\n");

# ifdef BT_REGEXP_LOG
    fclose(f);
# endif
}
#అయితే_ముగింపు	    // BT_REGEXP_DUMP

#నిర్వచితమైతే DEBUG
/*
 * regprop - printable representation of opcode
 */
    స్థిర char_u *
regprop(char_u *op)
{
    అక్షరం	    *p;
    స్థిర అక్షరం	    buf[50];
    స్థిర size_t   buflen = 0;

    STRCPY(buf, ":");
    buflen = 1;

    ఎంపిక ((పూర్ణాంకం) OP(op))
    {
      స్థితి BOL:
	p = "BOL";
	ఆపు;
      స్థితి EOL:
	p = "EOL";
	ఆపు;
      స్థితి RE_BOF:
	p = "BOF";
	ఆపు;
      స్థితి RE_EOF:
	p = "EOF";
	ఆపు;
      స్థితి CURSOR:
	p = "CURSOR";
	ఆపు;
      స్థితి RE_VISUAL:
	p = "RE_VISUAL";
	ఆపు;
      స్థితి RE_LNUM:
	p = "RE_LNUM";
	ఆపు;
      స్థితి RE_MARK:
	p = "RE_MARK";
	ఆపు;
      స్థితి RE_నిలువు:
	p = "RE_COL";
	ఆపు;
      స్థితి RE_VCOL:
	p = "RE_VCOL";
	ఆపు;
      స్థితి BOW:
	p = "BOW";
	ఆపు;
      స్థితి EOW:
	p = "EOW";
	ఆపు;
      స్థితి ANY:
	p = "ANY";
	ఆపు;
      స్థితి ANY + కూడు_NL:
	p = "ANY+NL";
	ఆపు;
      స్థితి ANYOF:
	p = "ANYOF";
	ఆపు;
      స్థితి ANYOF + కూడు_NL:
	p = "ANYOF+NL";
	ఆపు;
      స్థితి ANYBUT:
	p = "ANYBUT";
	ఆపు;
      స్థితి ANYBUT + కూడు_NL:
	p = "ANYBUT+NL";
	ఆపు;
      స్థితి IDENT:
	p = "IDENT";
	ఆపు;
      స్థితి IDENT + కూడు_NL:
	p = "IDENT+NL";
	ఆపు;
      స్థితి SIDENT:
	p = "SIDENT";
	ఆపు;
      స్థితి SIDENT + కూడు_NL:
	p = "SIDENT+NL";
	ఆపు;
      స్థితి KWORD:
	p = "KWORD";
	ఆపు;
      స్థితి KWORD + కూడు_NL:
	p = "KWORD+NL";
	ఆపు;
      స్థితి SKWORD:
	p = "SKWORD";
	ఆపు;
      స్థితి SKWORD + కూడు_NL:
	p = "SKWORD+NL";
	ఆపు;
      స్థితి FNAME:
	p = "FNAME";
	ఆపు;
      స్థితి FNAME + కూడు_NL:
	p = "FNAME+NL";
	ఆపు;
      స్థితి SFNAME:
	p = "SFNAME";
	ఆపు;
      స్థితి SFNAME + కూడు_NL:
	p = "SFNAME+NL";
	ఆపు;
      స్థితి PRINT:
	p = "PRINT";
	ఆపు;
      స్థితి PRINT + కూడు_NL:
	p = "PRINT+NL";
	ఆపు;
      స్థితి SPRINT:
	p = "SPRINT";
	ఆపు;
      స్థితి SPRINT + కూడు_NL:
	p = "SPRINT+NL";
	ఆపు;
      స్థితి WHITE:
	p = "WHITE";
	ఆపు;
      స్థితి WHITE + కూడు_NL:
	p = "WHITE+NL";
	ఆపు;
      స్థితి NWHITE:
	p = "NWHITE";
	ఆపు;
      స్థితి NWHITE + కూడు_NL:
	p = "NWHITE+NL";
	ఆపు;
      స్థితి DIGIT:
	p = "DIGIT";
	ఆపు;
      స్థితి DIGIT + కూడు_NL:
	p = "DIGIT+NL";
	ఆపు;
      స్థితి NDIGIT:
	p = "NDIGIT";
	ఆపు;
      స్థితి NDIGIT + కూడు_NL:
	p = "NDIGIT+NL";
	ఆపు;
      స్థితి HEX:
	p = "HEX";
	ఆపు;
      స్థితి HEX + కూడు_NL:
	p = "HEX+NL";
	ఆపు;
      స్థితి NHEX:
	p = "NHEX";
	ఆపు;
      స్థితి NHEX + కూడు_NL:
	p = "NHEX+NL";
	ఆపు;
      స్థితి OCTAL:
	p = "OCTAL";
	ఆపు;
      స్థితి OCTAL + కూడు_NL:
	p = "OCTAL+NL";
	ఆపు;
      స్థితి NOCTAL:
	p = "NOCTAL";
	ఆపు;
      స్థితి NOCTAL + కూడు_NL:
	p = "NOCTAL+NL";
	ఆపు;
      స్థితి మాట_2:
	p = "WORD";
	ఆపు;
      స్థితి మాట_2 + కూడు_NL:
	p = "WORD+NL";
	ఆపు;
      స్థితి NWORD:
	p = "NWORD";
	ఆపు;
      స్థితి NWORD + కూడు_NL:
	p = "NWORD+NL";
	ఆపు;
      స్థితి తల_2:
	p = "HEAD";
	ఆపు;
      స్థితి తల_2 + కూడు_NL:
	p = "HEAD+NL";
	ఆపు;
      స్థితి NHEAD:
	p = "NHEAD";
	ఆపు;
      స్థితి NHEAD + కూడు_NL:
	p = "NHEAD+NL";
	ఆపు;
      స్థితి ALPHA:
	p = "ALPHA";
	ఆపు;
      స్థితి ALPHA + కూడు_NL:
	p = "ALPHA+NL";
	ఆపు;
      స్థితి NALPHA:
	p = "NALPHA";
	ఆపు;
      స్థితి NALPHA + కూడు_NL:
	p = "NALPHA+NL";
	ఆపు;
      స్థితి క్రింది_2:
	p = "LOWER";
	ఆపు;
      స్థితి క్రింది_2 + కూడు_NL:
	p = "LOWER+NL";
	ఆపు;
      స్థితి NLOWER:
	p = "NLOWER";
	ఆపు;
      స్థితి NLOWER + కూడు_NL:
	p = "NLOWER+NL";
	ఆపు;
      స్థితి పై:
	p = "UPPER";
	ఆపు;
      స్థితి పై + కూడు_NL:
	p = "UPPER+NL";
	ఆపు;
      స్థితి NUPPER:
	p = "NUPPER";
	ఆపు;
      స్థితి NUPPER + కూడు_NL:
	p = "NUPPER+NL";
	ఆపు;
      స్థితి BRANCH:
	p = "BRANCH";
	ఆపు;
      స్థితి EXACTLY:
	p = "EXACTLY";
	ఆపు;
      స్థితి NOTHING:
	p = "NOTHING";
	ఆపు;
      స్థితి వెనుక_2:
	p = "BACK";
	ఆపు;
      స్థితి చివర_3:
	p = "END";
	ఆపు;
      స్థితి MOPEN + 0:
	p = "MATCH START";
	ఆపు;
      స్థితి MOPEN + 1:
      స్థితి MOPEN + 2:
      స్థితి MOPEN + 3:
      స్థితి MOPEN + 4:
      స్థితి MOPEN + 5:
      స్థితి MOPEN + 6:
      స్థితి MOPEN + 7:
      స్థితి MOPEN + 8:
      స్థితి MOPEN + 9:
	buflen += vim_snprintf(buf + buflen, పరిమాణం(buf) - buflen, "MOPEN%d", OP(op) - MOPEN);
	p = ఖాళీ;
	ఆపు;
      స్థితి MCLOSE + 0:
	p = "MATCH END";
	ఆపు;
      స్థితి MCLOSE + 1:
      స్థితి MCLOSE + 2:
      స్థితి MCLOSE + 3:
      స్థితి MCLOSE + 4:
      స్థితి MCLOSE + 5:
      స్థితి MCLOSE + 6:
      స్థితి MCLOSE + 7:
      స్థితి MCLOSE + 8:
      స్థితి MCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, పరిమాణం(buf) - buflen, "MCLOSE%d", OP(op) - MCLOSE);
	p = ఖాళీ;
	ఆపు;
      స్థితి BACKREF + 1:
      స్థితి BACKREF + 2:
      స్థితి BACKREF + 3:
      స్థితి BACKREF + 4:
      స్థితి BACKREF + 5:
      స్థితి BACKREF + 6:
      స్థితి BACKREF + 7:
      స్థితి BACKREF + 8:
      స్థితి BACKREF + 9:
	buflen += vim_snprintf(buf + buflen, పరిమాణం(buf) - buflen, "BACKREF%d", OP(op) - BACKREF);
	p = ఖాళీ;
	ఆపు;
      స్థితి NOPEN:
	p = "NOPEN";
	ఆపు;
      స్థితి NCLOSE:
	p = "NCLOSE";
	ఆపు;
# ifdef FEAT_SYN_HL
      స్థితి ZOPEN + 1:
      స్థితి ZOPEN + 2:
      స్థితి ZOPEN + 3:
      స్థితి ZOPEN + 4:
      స్థితి ZOPEN + 5:
      స్థితి ZOPEN + 6:
      స్థితి ZOPEN + 7:
      స్థితి ZOPEN + 8:
      స్థితి ZOPEN + 9:
	buflen += vim_snprintf(buf + buflen, పరిమాణం(buf) - buflen, "ZOPEN%d", OP(op) - ZOPEN);
	p = ఖాళీ;
	ఆపు;
      స్థితి ZCLOSE + 1:
      స్థితి ZCLOSE + 2:
      స్థితి ZCLOSE + 3:
      స్థితి ZCLOSE + 4:
      స్థితి ZCLOSE + 5:
      స్థితి ZCLOSE + 6:
      స్థితి ZCLOSE + 7:
      స్థితి ZCLOSE + 8:
      స్థితి ZCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, పరిమాణం(buf) - buflen, "ZCLOSE%d", OP(op) - ZCLOSE);
	p = ఖాళీ;
	ఆపు;
      స్థితి ZREF + 1:
      స్థితి ZREF + 2:
      స్థితి ZREF + 3:
      స్థితి ZREF + 4:
      స్థితి ZREF + 5:
      స్థితి ZREF + 6:
      స్థితి ZREF + 7:
      స్థితి ZREF + 8:
      స్థితి ZREF + 9:
	buflen += vim_snprintf(buf + buflen, పరిమాణం(buf) - buflen, "ZREF%d", OP(op) - ZREF);
	p = ఖాళీ;
	ఆపు;
# endif
      స్థితి నక్షత్రం:
	p = "STAR";
	ఆపు;
      స్థితి PLUS:
	p = "PLUS";
	ఆపు;
      స్థితి NOMATCH:
	p = "NOMATCH";
	ఆపు;
      స్థితి సరిపోలిక_2:
	p = "MATCH";
	ఆపు;
      స్థితి BEHIND:
	p = "BEHIND";
	ఆపు;
      స్థితి NOBEHIND:
	p = "NOBEHIND";
	ఆపు;
      స్థితి SUBPAT:
	p = "SUBPAT";
	ఆపు;
      స్థితి BRACE_LIMITS:
	p = "BRACE_LIMITS";
	ఆపు;
      స్థితి BRACE_SIMPLE:
	p = "BRACE_SIMPLE";
	ఆపు;
      స్థితి BRACE_COMPLEX + 0:
      స్థితి BRACE_COMPLEX + 1:
      స్థితి BRACE_COMPLEX + 2:
      స్థితి BRACE_COMPLEX + 3:
      స్థితి BRACE_COMPLEX + 4:
      స్థితి BRACE_COMPLEX + 5:
      స్థితి BRACE_COMPLEX + 6:
      స్థితి BRACE_COMPLEX + 7:
      స్థితి BRACE_COMPLEX + 8:
      స్థితి BRACE_COMPLEX + 9:
	buflen += vim_snprintf(buf + buflen, పరిమాణం(buf) - buflen, "BRACE_COMPLEX%d", OP(op) - BRACE_COMPLEX);
	p = ఖాళీ;
	ఆపు;
      స్థితి MULTIBYTECODE:
	p = "MULTIBYTECODE";
	ఆపు;
      స్థితి NEWL:
	p = "NEWL";
	ఆపు;
      అప్రమేయ:
	buflen += vim_snprintf(buf + buflen, పరిమాణం(buf) - buflen, "corrupt %d", OP(op));
	p = ఖాళీ;
	ఆపు;
    }
    ఒకవేళ (p != ఖాళీ)
	vim_strncpy((char_u *)buf + buflen, (char_u *)p, పరిమాణం(buf) - buflen - 1);
    తిరిగి (char_u *)buf;
}
#అయితే_ముగింపు	    // DEBUG
