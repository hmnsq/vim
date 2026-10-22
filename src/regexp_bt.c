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
#define END		0	//	End of program or NOMATCH operand.
#define BOL		1	//	Match "" at beginning of line.
#define EOL		2	//	Match "" at end of line.
#define BRANCH		3	// node Match this alternative, or the
				//	next...
#define BACK		4	//	Match "", "next" ptr points backward.
#define EXACTLY		5	// str	Match this string.
#define NOTHING		6	//	Match empty string.
#define STAR		7	// node Match this (simple) thing 0 or more
				//	times.
#define PLUS		8	// node Match this (simple) thing 1 or more
				//	times.
#define MATCH		9	// node match the operand zero-width
#define NOMATCH		10	// node check for no match with operand
#define BEHIND		11	// node look behind for a match with operand
#define NOBEHIND	12	// node look behind for no match with operand
#define SUBPAT		13	// node match the operand here
#define BRACE_SIMPLE	14	// node Match this (simple) thing between m and
				//	n times (\{m,n\}).
#define BOW		15	//	Match "" after [^a-zA-Z0-9_]
#define EOW		16	//	Match "" at    [^a-zA-Z0-9_]
#define BRACE_LIMITS	17	// nr nr  define the min & max for BRACE_SIMPLE
				//	and BRACE_COMPLEX.
#define NEWL		18	//	Match line-break
#define BHPOS		19	//	End position for BEHIND or NOBEHIND


// character classes: 20-48 normal, 50-78 include a line-break
#define ADD_NL		30
#define FIRST_NL	ANY + ADD_NL
#define ANY		20	//	Match any one character.
#define ANYOF		21	// str	Match any character in this string.
#define ANYBUT		22	// str	Match any character not in this
				//	string.
#define IDENT		23	//	Match identifier char
#define SIDENT		24	//	Match identifier char but no digit
#define KWORD		25	//	Match keyword char
#define SKWORD		26	//	Match word char but no digit
#define FNAME		27	//	Match file name char
#define SFNAME		28	//	Match file name char but no digit
#define PRINT		29	//	Match printable char
#define SPRINT		30	//	Match printable char but no digit
#define WHITE		31	//	Match whitespace char
#define NWHITE		32	//	Match non-whitespace char
#define DIGIT		33	//	Match digit char
#define NDIGIT		34	//	Match non-digit char
#define HEX		35	//	Match hex char
#define NHEX		36	//	Match non-hex char
#define OCTAL		37	//	Match octal char
#define NOCTAL		38	//	Match non-octal char
#define WORD		39	//	Match word char
#define NWORD		40	//	Match non-word char
#define HEAD		41	//	Match head char
#define NHEAD		42	//	Match non-head char
#define ALPHA		43	//	Match alpha char
#define NALPHA		44	//	Match non-alpha char
#define LOWER		45	//	Match lowercase char
#define NLOWER		46	//	Match non-lowercase char
#define UPPER		47	//	Match uppercase char
#define NUPPER		48	//	Match non-uppercase char
#define LAST_NL		NUPPER + ADD_NL
#define WITH_NL(op)	((op) >= FIRST_NL && (op) <= LAST_NL)

#define MOPEN		80  // -89	 Mark this point in input as start of
				//	 \( subexpr.  MOPEN + 0 marks start of
				//	 match.
#define MCLOSE		90  // -99	 Analogous to MOPEN.  MCLOSE + 0 marks
				//	 end of match.
#define BACKREF		100 // -109 node Match same string again \1-\9

#ifdef FEAT_SYN_HL
# define ZOPEN		110 // -119	 Mark this point in input as start of
				//	 \z( subexpr.
# define ZCLOSE		120 // -129	 Analogous to ZOPEN.
# define ZREF		130 // -139 node Match external submatch \z1-\z9
#endif

#define BRACE_COMPLEX	140 // -149 node Match nodes between m & n times

#define NOPEN		150	//	Mark this point in input as start of
				//	\%( subexpr.
#define NCLOSE		151	//	Analogous to NOPEN.

#define MULTIBYTECODE	200	// mbc	Match one multi-byte character
#define RE_BOF		201	//	Match "" at beginning of file.
#define RE_EOF		202	//	Match "" at end of file.
#define CURSOR		203	//	Match location of cursor.

#define RE_LNUM		204	// nr cmp  Match line number
#define RE_COL		205	// nr cmp  Match column number
#define RE_VCOL		206	// nr cmp  Match virtual column number

#define RE_MARK		207	// mark cmp  Match mark position
#define RE_VISUAL	208	//	Match Visual area
#define RE_COMPOSING	209	// any composing characters

/*
 * Flags to be passed up and down.
 */
#define HASWIDTH	0x1	// Known never to match null string.
#define SIMPLE		0x2	// Simple enough to be STAR/PLUS operand.
#define SPSTART		0x4	// Starts with * or +.
#define HASNL		0x8	// Contains some \n.
#define HASLOOKBH	0x10	// Contains "\@<=" or "\@<!".
#define WORST		0	// Worst case.

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଅଙ୍କ_complex_braces; // Complex \{...} count
ସ୍ଥିର char_u	*regcode;	// Code-emit pointer, or JUST_CALC_SIZE
ସ୍ଥିର long	regsize;	// Code size.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	reg_toolong;	// TRUE when offset out of range
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	bt_reg_parse_depth;	// nesting depth in reg()
ସ୍ଥିର char_u	had_endbrace[NSUBEXP];	// flags, TRUE if end of () found
ସ୍ଥିର long	brace_min[10];	// Minimums for complex brace repeats
ସ୍ଥିର long	brace_max[10];	// Maximums for complex brace repeats
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	brace_ଗଣନା[10]; // Current counts for complex brace repeats
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	one_exactly = FALSE;	// only do one char for EXACTLY

// When making changes to classchars also change nfa_classcodes.
ସ୍ଥିର char_u	*classchars = (char_u *)".iIkKfFpPsSdDxXoOwWhHaAlLuU";
ସ୍ଥିର ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା classcodes[] = {
    ANY, IDENT, SIDENT, KWORD, SKWORD,
    FNAME, SFNAME, PRINT, SPRINT,
    WHITE, NWHITE, DIGIT, NDIGIT,
    HEX, NHEX, OCTAL, NOCTAL,
    WORD, NWORD, HEAD, NHEAD,
    ALPHA, NALPHA, LOWER, NLOWER,
    UPPER, NUPPER
};

/*
 * Search between "start" (the first char of the range) and "end" (the closing
 * "]") and try to recognize a character class in expanded form, for example
 * [0-9].  On success, return the atom to be emitted, on failure 0.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bt_recognize_char_class(char_u *start, char_u *end)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		newl;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		config = get_char_class_bits(start, end, &newl);

    // A newline would need the ADD_NL variant, don't bother with it here.
    ଯଦି (config < 0 || newl)
	ଫେରନ୍ତୁ 0;

	// The old engine has no case-insensitive class opcode, so [a-z] and [A-Z]
	// are left as collections.  The classes below are case-independent.
    ବାଛ (config)
    {
	ସ୍ଥିତି CLASS_o9:
	    ଫେରନ୍ତୁ DIGIT;
	ସ୍ଥିତି CLASS_not | CLASS_o9:
	    ଫେରନ୍ତୁ NDIGIT;
	ସ୍ଥିତି CLASS_af | CLASS_AF | CLASS_o9:
	    ଫେରନ୍ତୁ HEX;
	ସ୍ଥିତି CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    ଫେରନ୍ତୁ NHEX;
	ସ୍ଥିତି CLASS_o7:
	    ଫେରନ୍ତୁ OCTAL;
	ସ୍ଥିତି CLASS_not | CLASS_o7:
	    ଫେରନ୍ତୁ NOCTAL;
	ସ୍ଥିତି CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    ଫେରନ୍ତୁ WORD;
	ସ୍ଥିତି CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    ଫେରନ୍ତୁ NWORD;
	ସ୍ଥିତି CLASS_az | CLASS_AZ | CLASS_underscore:
	    ଫେରନ୍ତୁ HEAD;
	ସ୍ଥିତି CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    ଫେରନ୍ତୁ NHEAD;
	ସ୍ଥିତି CLASS_az | CLASS_AZ:
	    ଫେରନ୍ତୁ ALPHA;
	ସ୍ଥିତି CLASS_not | CLASS_az | CLASS_AZ:
	    ଫେରନ୍ତୁ NALPHA;
    }
    ଫେରନ୍ତୁ 0;
}

/*
 * When regcode is set to this value, code is not emitted and size is computed
 * instead.
 */
#define JUST_CALC_SIZE	((char_u *) -1)

// Values for rs_state in regitem_T.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା regstate_E
{
    RS_NOPEN = 0	// NOPEN and NCLOSE
    , RS_MOPEN		// MOPEN + [0-9]
    , RS_MCLOSE		// MCLOSE + [0-9]
#ifdef FEAT_SYN_HL
    , RS_ZOPEN		// ZOPEN + [0-9]
    , RS_ZCLOSE		// ZCLOSE + [0-9]
#endif
    , RS_BRANCH		// BRANCH
    , RS_BRCPLX_MORE	// BRACE_COMPLEX and trying one more match
    , RS_BRCPLX_LONG	// BRACE_COMPLEX and trying longest match
    , RS_BRCPLX_SHORT	// BRACE_COMPLEX and trying shortest match
    , RS_NOMATCH	// NOMATCH
    , RS_BEHIND1	// BEHIND / NOBEHIND matching rest
    , RS_BEHIND2	// BEHIND / NOBEHIND matching behind part
    , RS_STAR_LONG	// STAR/PLUS/BRACE_SIMPLE longest match
    , RS_STAR_SHORT	// STAR/PLUS/BRACE_SIMPLE shortest match
} regstate_T;

/*
 * Structure used to save the current input state, when it needs to be
 * restored after trying a match.  Used by reg_save() and reg_restore().
 * Also stores the length of "backpos".
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    ସଂଘ
    {
	char_u	*ptr;	// rex.input pointer, for single-line regexp
	lpos_T	pos;	// rex.input pos, for multi-line regexp
    } rs_u;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		rs_len;
} regsave_T;

// struct to save start/end pointer/position in for \(\)
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    ସଂଘ
    {
	char_u	*ptr;
	lpos_T	pos;
    } se_u;
} save_se_T;

// used for BEHIND and NOBEHIND matching
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା regbehind_S
{
    regsave_T	save_after;
    regsave_T	save_behind;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_need_clear_subexpr;
    save_se_T   save_start[NSUBEXP];
    save_se_T   save_end[NSUBEXP];
} regbehind_T;

/*
 * When there are alternatives a regstate_T is put on the regstack to remember
 * what we are doing.
 * Before it may be another type of item, depending on rs_state, to remember
 * more things.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା regitem_S
{
    regstate_T	rs_state;	// what we are doing, one of RS_ above
    short	rs_no;		// submatch nr or BEHIND/NOBEHIND
    char_u	*rs_scan;	// current node in program
    ସଂଘ
    {
	save_se_T  sesave;
	regsave_T  regsave;
    } rs_un;			// room for saving rex.input
} regitem_T;


// used for STAR, PLUS and BRACE_SIMPLE matching
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା regstar_S
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nextb;		// next byte
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nextb_ic;	// next byte reverse case
    long	ଗଣନା_2;
    long	minval;
    long	maxval;
} regstar_T;

// used to store input position when a BACK was encountered, so that we now if
// we made any progress since the last time.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା backpos_S
{
    char_u	*bp_scan;	// "scan" where BACK was encountered
    regsave_T	bp_pos;		// last input position
} backpos_T;

/*
 * "regstack" and "backpos" are used by regmatch().  They are kept over calls
 * to avoid invoking malloc() and free() often.
 * "regstack" is a stack with regitem_T items, sometimes preceded by regstar_T
 * or regbehind_T.
 * "backpos_T" is a table with backpos_T for BACK
 */
ସ୍ଥିର garray_T	regstack = {0, 0, 0, 0, ଖାଲି};
ସ୍ଥିର garray_T	backpos = {0, 0, 0, 0, ଖାଲି};

ସ୍ଥିର regsave_T behind_pos;

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
#define REGSTACK_INITIAL	2048
#define BACKPOS_INITIAL		64

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
#define OP(p)		((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)*(p))
#define NEXT(p)		(((*((p) + 1) & 0377) << 8) + (*((p) + 2) & 0377))
#define OPERAND(p)	((p) + 3)
// Obtain an operand that was stored as four bytes, MSB first.
#define OPERAND_MIN(p)	(((long)(p)[3] << 24) + ((long)(p)[4] << 16) \
			+ ((long)(p)[5] << 8) + (long)(p)[6])
// Obtain a second operand stored as four bytes.
#define OPERAND_MAX(p)	OPERAND_MIN((p) + 4)
// Obtain a second single-byte operand stored after a four bytes operand.
#define OPERAND_CMP(p)	(p)[7]

ସ୍ଥିର char_u *reg(ପୂର୍ଣ୍ଣସଂଖ୍ୟା paren, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *flagp);

#ifdef BT_REGEXP_DUMP
ସ୍ଥିର ଶୂନ୍ୟ	regdump(char_u *, bt_regprog_T *);
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	re_ଅଙ୍କ_cmp(long_u val, char_u *scan);

#ifdef DEBUG
ସ୍ଥିର char_u	*regprop(char_u *);

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	regnarrate = 0;
#endif


/*
 * Setup to parse the regexp.  Used once to get the length and once to do it.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
regcomp_start(
    char_u	*ପଦସମଷ୍ଟି,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		re_flags)	    // see vim_regcomp()
{
    initchr(ପଦସମଷ୍ଟି);
    ଯଦି (re_flags & RE_MAGIC)
	reg_magic = MAGIC_ON;
    ଅନ୍ୟଥା
	reg_magic = MAGIC_OFF;
    reg_string = (re_flags & RE_STRING);
    reg_strict = (re_flags & RE_STRICT);
    get_cpo_flags();

    ଅଙ୍କ_complex_braces = 0;
    regnpar = 1;
    CLEAR_FIELD(had_endbrace);
#ifdef FEAT_SYN_HL
    regnzpar = 1;
    re_has_z = 0;
#endif
    regsize = 0L;
    reg_toolong = FALSE;
    bt_reg_parse_depth = 0;
    regflags = 0;
#ଯଦି defined(FEAT_SYN_HL)
    had_eol = FALSE;
#endif
}

/*
 * Return TRUE if MULTIBYTECODE should be used instead of EXACTLY for
 * character "c".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
use_multibytecode(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଫେରନ୍ତୁ has_mbyte && (*mb_char2len)(c) > 1
		     && (re_multi_type(peekchr()) != NOT_MULTI
			     || (enc_utf8 && utf_iscomposing(c)));
}

/*
 * Emit (if appropriate) a byte of code
 */
    ସ୍ଥିର ଶୂନ୍ୟ
regc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା b)
{
    ଯଦି (regcode == JUST_CALC_SIZE)
	regsize++;
    ଅନ୍ୟଥା
	*regcode++ = b;
}

/*
 * Emit (if appropriate) a multi-byte character of code
 */
    ସ୍ଥିର ଶୂନ୍ୟ
regmbc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (!has_mbyte && c > 0xff)
	ଫେରନ୍ତୁ;
    ଯଦି (regcode == JUST_CALC_SIZE)
	regsize += (*mb_char2len)(c);
    ଅନ୍ୟଥା
	regcode += (*mb_char2bytes)(c, regcode);
}


/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * NOTE: When changing this function, also change nfa_emit_equi_class()
 */
    ସ୍ଥିର ଶୂନ୍ୟ
reg_equi_class(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (enc_utf8 || STRCMP(p_enc, "latin1") == 0
					 || STRCMP(p_enc, "iso-8859-15") == 0)
    {
	ବାଛ (c)
	{
	    // Do not use '\300' style, it results in a negative number.
	    ସ୍ଥିତି 'A': ସ୍ଥିତି 0xc0: ସ୍ଥିତି 0xc1: ସ୍ଥିତି 0xc2: ସ୍ଥିତି 0xc3: ସ୍ଥିତି 0xc4:
	    ସ୍ଥିତି 0xc5: ସ୍ଥିତି 0x100: ସ୍ଥିତି 0x102: ସ୍ଥିତି 0x104: ସ୍ଥିତି 0x1cd:
	    ସ୍ଥିତି 0x1de: ସ୍ଥିତି 0x1e0: ସ୍ଥିତି 0x1fa: ସ୍ଥିତି 0x202: ସ୍ଥିତି 0x226:
	    ସ୍ଥିତି 0x23a: ସ୍ଥିତି 0x1e00: ସ୍ଥିତି 0x1ea0: ସ୍ଥିତି 0x1ea2: ସ୍ଥିତି 0x1ea4:
	    ସ୍ଥିତି 0x1ea6: ସ୍ଥିତି 0x1ea8: ସ୍ଥିତି 0x1eaa: ସ୍ଥିତି 0x1eac: ସ୍ଥିତି 0x1eae:
	    ସ୍ଥିତି 0x1eb0: ସ୍ଥିତି 0x1eb2: ସ୍ଥିତି 0x1eb4: ସ୍ଥିତି 0x1eb6:
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
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'B': ସ୍ଥିତି 0x181: ସ୍ଥିତି 0x243: ସ୍ଥିତି 0x1e02:
	    ସ୍ଥିତି 0x1e04: ସ୍ଥିତି 0x1e06:
		      regmbc('B');
		      regmbc(0x181); regmbc(0x243); regmbc(0x1e02);
		      regmbc(0x1e04); regmbc(0x1e06);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'C': ସ୍ଥିତି 0xc7:
	    ସ୍ଥିତି 0x106: ସ୍ଥିତି 0x108: ସ୍ଥିତି 0x10a: ସ୍ଥିତି 0x10c: ସ୍ଥିତି 0x187:
	    ସ୍ଥିତି 0x23b: ସ୍ଥିତି 0x1e08: ସ୍ଥିତି 0xa792:
		      regmbc('C'); regmbc(0xc7);
		      regmbc(0x106); regmbc(0x108); regmbc(0x10a);
		      regmbc(0x10c); regmbc(0x187); regmbc(0x23b);
		      regmbc(0x1e08); regmbc(0xa792);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'D': ସ୍ଥିତି 0x10e: ସ୍ଥିତି 0x110: ସ୍ଥିତି 0x18a:
	    ସ୍ଥିତି 0x1e0a: ସ୍ଥିତି 0x1e0c: ସ୍ଥିତି 0x1e0e: ସ୍ଥିତି 0x1e10:
	    ସ୍ଥିତି 0x1e12:
		      regmbc('D'); regmbc(0x10e); regmbc(0x110);
		      regmbc(0x18a); regmbc(0x1e0a); regmbc(0x1e0c);
		      regmbc(0x1e0e); regmbc(0x1e10); regmbc(0x1e12);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'E': ସ୍ଥିତି 0xc8: ସ୍ଥିତି 0xc9: ସ୍ଥିତି 0xca: ସ୍ଥିତି 0xcb:
	    ସ୍ଥିତି 0x112: ସ୍ଥିତି 0x114: ସ୍ଥିତି 0x116: ସ୍ଥିତି 0x118: ସ୍ଥିତି 0x11a:
	    ସ୍ଥିତି 0x204: ସ୍ଥିତି 0x206: ସ୍ଥିତି 0x228: ସ୍ଥିତି 0x246: ସ୍ଥିତି 0x1e14:
	    ସ୍ଥିତି 0x1e16: ସ୍ଥିତି 0x1e18: ସ୍ଥିତି 0x1e1a: ସ୍ଥିତି 0x1e1c:
	    ସ୍ଥିତି 0x1eb8: ସ୍ଥିତି 0x1eba: ସ୍ଥିତି 0x1ebc: ସ୍ଥିତି 0x1ebe:
	    ସ୍ଥିତି 0x1ec0: ସ୍ଥିତି 0x1ec2: ସ୍ଥିତି 0x1ec4: ସ୍ଥିତି 0x1ec6:
		      regmbc('E'); regmbc(0xc8); regmbc(0xc9);
		      regmbc(0xca); regmbc(0xcb); regmbc(0x112);
		      regmbc(0x114); regmbc(0x116); regmbc(0x118);
		      regmbc(0x11a); regmbc(0x204); regmbc(0x206);
		      regmbc(0x228); regmbc(0x246); regmbc(0x1e14);
		      regmbc(0x1e16); regmbc(0x1e18); regmbc(0x1e1a);
		      regmbc(0x1e1c); regmbc(0x1eb8); regmbc(0x1eba);
		      regmbc(0x1ebc); regmbc(0x1ebe); regmbc(0x1ec0);
		      regmbc(0x1ec2); regmbc(0x1ec4); regmbc(0x1ec6);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'F': ସ୍ଥିତି 0x191: ସ୍ଥିତି 0x1e1e: ସ୍ଥିତି 0xa798:
		      regmbc('F'); regmbc(0x191); regmbc(0x1e1e);
		      regmbc(0xa798);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'G': ସ୍ଥିତି 0x11c: ସ୍ଥିତି 0x11e: ସ୍ଥିତି 0x120:
	    ସ୍ଥିତି 0x122: ସ୍ଥିତି 0x193: ସ୍ଥିତି 0x1e4: ସ୍ଥିତି 0x1e6:
	    ସ୍ଥିତି 0x1f4: ସ୍ଥିତି 0x1e20: ସ୍ଥିତି 0xa7a0:
		      regmbc('G'); regmbc(0x11c); regmbc(0x11e);
		      regmbc(0x120); regmbc(0x122); regmbc(0x193);
		      regmbc(0x1e4); regmbc(0x1e6); regmbc(0x1f4);
		      regmbc(0x1e20); regmbc(0xa7a0);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'H': ସ୍ଥିତି 0x124: ସ୍ଥିତି 0x126: ସ୍ଥିତି 0x21e:
	    ସ୍ଥିତି 0x1e22: ସ୍ଥିତି 0x1e24: ସ୍ଥିତି 0x1e26:
	    ସ୍ଥିତି 0x1e28: ସ୍ଥିତି 0x1e2a: ସ୍ଥିତି 0x2c67:
		      regmbc('H'); regmbc(0x124); regmbc(0x126);
		      regmbc(0x21e); regmbc(0x1e22); regmbc(0x1e24);
		      regmbc(0x1e26); regmbc(0x1e28); regmbc(0x1e2a);
		      regmbc(0x2c67);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'I': ସ୍ଥିତି 0xcc: ସ୍ଥିତି 0xcd: ସ୍ଥିତି 0xce: ସ୍ଥିତି 0xcf:
	    ସ୍ଥିତି 0x128: ସ୍ଥିତି 0x12a: ସ୍ଥିତି 0x12c: ସ୍ଥିତି 0x12e:
	    ସ୍ଥିତି 0x130: ସ୍ଥିତି 0x197: ସ୍ଥିତି 0x1cf: ସ୍ଥିତି 0x208:
	    ସ୍ଥିତି 0x20a: ସ୍ଥିତି 0x1e2c: ସ୍ଥିତି 0x1e2e: ସ୍ଥିତି 0x1ec8:
	    ସ୍ଥିତି 0x1eca:
		      regmbc('I'); regmbc(0xcc); regmbc(0xcd);
		      regmbc(0xce); regmbc(0xcf); regmbc(0x128);
		      regmbc(0x12a); regmbc(0x12c); regmbc(0x12e);
		      regmbc(0x130); regmbc(0x197); regmbc(0x1cf);
		      regmbc(0x208); regmbc(0x20a); regmbc(0x1e2c);
		      regmbc(0x1e2e); regmbc(0x1ec8); regmbc(0x1eca);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'J': ସ୍ଥିତି 0x134: ସ୍ଥିତି 0x248:
		      regmbc('J'); regmbc(0x134); regmbc(0x248);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'K': ସ୍ଥିତି 0x136: ସ୍ଥିତି 0x198: ସ୍ଥିତି 0x1e8: ସ୍ଥିତି 0x1e30:
	    ସ୍ଥିତି 0x1e32: ସ୍ଥିତି 0x1e34: ସ୍ଥିତି 0x2c69: ସ୍ଥିତି 0xa740:
		      regmbc('K'); regmbc(0x136); regmbc(0x198);
		      regmbc(0x1e8); regmbc(0x1e30); regmbc(0x1e32);
		      regmbc(0x1e34); regmbc(0x2c69); regmbc(0xa740);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'L': ସ୍ଥିତି 0x139: ସ୍ଥିତି 0x13b: ସ୍ଥିତି 0x13d: ସ୍ଥିତି 0x13f:
	    ସ୍ଥିତି 0x141: ସ୍ଥିତି 0x23d: ସ୍ଥିତି 0x1e36: ସ୍ଥିତି 0x1e38:
	    ସ୍ଥିତି 0x1e3a: ସ୍ଥିତି 0x1e3c: ସ୍ଥିତି 0x2c60:
		      regmbc('L'); regmbc(0x139); regmbc(0x13b);
		      regmbc(0x13d); regmbc(0x13f); regmbc(0x141);
		      regmbc(0x23d); regmbc(0x1e36); regmbc(0x1e38);
		      regmbc(0x1e3a); regmbc(0x1e3c); regmbc(0x2c60);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'M': ସ୍ଥିତି 0x1e3e: ସ୍ଥିତି 0x1e40: ସ୍ଥିତି 0x1e42:
		      regmbc('M'); regmbc(0x1e3e); regmbc(0x1e40);
		      regmbc(0x1e42);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'N': ସ୍ଥିତି 0xd1:
	    ସ୍ଥିତି 0x143: ସ୍ଥିତି 0x145: ସ୍ଥିତି 0x147: ସ୍ଥିତି 0x1f8:
	    ସ୍ଥିତି 0x1e44: ସ୍ଥିତି 0x1e46: ସ୍ଥିତି 0x1e48: ସ୍ଥିତି 0x1e4a:
	    ସ୍ଥିତି 0xa7a4:
		      regmbc('N'); regmbc(0xd1);
		      regmbc(0x143); regmbc(0x145); regmbc(0x147);
		      regmbc(0x1f8); regmbc(0x1e44); regmbc(0x1e46);
		      regmbc(0x1e48); regmbc(0x1e4a); regmbc(0xa7a4);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'O': ସ୍ଥିତି 0xd2: ସ୍ଥିତି 0xd3: ସ୍ଥିତି 0xd4: ସ୍ଥିତି 0xd5: ସ୍ଥିତି 0xd6:
	    ସ୍ଥିତି 0xd8: ସ୍ଥିତି 0x14c: ସ୍ଥିତି 0x14e: ସ୍ଥିତି 0x150: ସ୍ଥିତି 0x19f:
	    ସ୍ଥିତି 0x1a0: ସ୍ଥିତି 0x1d1: ସ୍ଥିତି 0x1ea: ସ୍ଥିତି 0x1ec: ସ୍ଥିତି 0x1fe:
	    ସ୍ଥିତି 0x20c: ସ୍ଥିତି 0x20e: ସ୍ଥିତି 0x22a: ସ୍ଥିତି 0x22c: ସ୍ଥିତି 0x22e:
	    ସ୍ଥିତି 0x230: ସ୍ଥିତି 0x1e4c: ସ୍ଥିତି 0x1e4e: ସ୍ଥିତି 0x1e50: ସ୍ଥିତି 0x1e52:
	    ସ୍ଥିତି 0x1ecc: ସ୍ଥିତି 0x1ece: ସ୍ଥିତି 0x1ed0: ସ୍ଥିତି 0x1ed2: ସ୍ଥିତି 0x1ed4:
	    ସ୍ଥିତି 0x1ed6: ସ୍ଥିତି 0x1ed8: ସ୍ଥିତି 0x1eda: ସ୍ଥିତି 0x1edc: ସ୍ଥିତି 0x1ede:
	    ସ୍ଥିତି 0x1ee0: ସ୍ଥିତି 0x1ee2:
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
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'P': ସ୍ଥିତି 0x1a4: ସ୍ଥିତି 0x1e54: ସ୍ଥିତି 0x1e56: ସ୍ଥିତି 0x2c63:
		      regmbc('P'); regmbc(0x1a4); regmbc(0x1e54);
		      regmbc(0x1e56); regmbc(0x2c63);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'Q': ସ୍ଥିତି 0x24a:
		      regmbc('Q'); regmbc(0x24a);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'R': ସ୍ଥିତି 0x154: ସ୍ଥିତି 0x156: ସ୍ଥିତି 0x158: ସ୍ଥିତି 0x210:
	    ସ୍ଥିତି 0x212: ସ୍ଥିତି 0x24c: ସ୍ଥିତି 0x1e58: ସ୍ଥିତି 0x1e5a:
	    ସ୍ଥିତି 0x1e5c: ସ୍ଥିତି 0x1e5e: ସ୍ଥିତି 0x2c64: ସ୍ଥିତି 0xa7a6:
		      regmbc('R'); regmbc(0x154); regmbc(0x156);
		      regmbc(0x210); regmbc(0x212); regmbc(0x158);
		      regmbc(0x24c); regmbc(0x1e58); regmbc(0x1e5a);
		      regmbc(0x1e5c); regmbc(0x1e5e); regmbc(0x2c64);
		      regmbc(0xa7a6);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'S': ସ୍ଥିତି 0x15a: ସ୍ଥିତି 0x15c: ସ୍ଥିତି 0x15e: ସ୍ଥିତି 0x160:
	    ସ୍ଥିତି 0x218: ସ୍ଥିତି 0x1e60: ସ୍ଥିତି 0x1e62: ସ୍ଥିତି 0x1e64:
	    ସ୍ଥିତି 0x1e66: ସ୍ଥିତି 0x1e68: ସ୍ଥିତି 0x2c7e: ସ୍ଥିତି 0xa7a8:
		      regmbc('S'); regmbc(0x15a); regmbc(0x15c);
		      regmbc(0x15e); regmbc(0x160); regmbc(0x218);
		      regmbc(0x1e60); regmbc(0x1e62); regmbc(0x1e64);
		      regmbc(0x1e66); regmbc(0x1e68); regmbc(0x2c7e);
			  regmbc(0xa7a8);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'T': ସ୍ଥିତି 0x162: ସ୍ଥିତି 0x164: ସ୍ଥିତି 0x166: ସ୍ଥିତି 0x1ac:
	    ସ୍ଥିତି 0x1ae: ସ୍ଥିତି 0x21a: ସ୍ଥିତି 0x23e: ସ୍ଥିତି 0x1e6a: ସ୍ଥିତି 0x1e6c:
	    ସ୍ଥିତି 0x1e6e: ସ୍ଥିତି 0x1e70:
		      regmbc('T'); regmbc(0x162); regmbc(0x164);
		      regmbc(0x166); regmbc(0x1ac); regmbc(0x23e);
		      regmbc(0x1ae); regmbc(0x21a); regmbc(0x1e6a);
		      regmbc(0x1e6c); regmbc(0x1e6e); regmbc(0x1e70);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'U': ସ୍ଥିତି 0xd9: ସ୍ଥିତି 0xda: ସ୍ଥିତି 0xdb: ସ୍ଥିତି 0xdc:
	    ସ୍ଥିତି 0x168: ସ୍ଥିତି 0x16a: ସ୍ଥିତି 0x16c: ସ୍ଥିତି 0x16e:
	    ସ୍ଥିତି 0x170: ସ୍ଥିତି 0x172: ସ୍ଥିତି 0x1af: ସ୍ଥିତି 0x1d3:
	    ସ୍ଥିତି 0x1d5: ସ୍ଥିତି 0x1d7: ସ୍ଥିତି 0x1d9: ସ୍ଥିତି 0x1db:
	    ସ୍ଥିତି 0x214: ସ୍ଥିତି 0x216: ସ୍ଥିତି 0x244: ସ୍ଥିତି 0x1e72:
	    ସ୍ଥିତି 0x1e74: ସ୍ଥିତି 0x1e76: ସ୍ଥିତି 0x1e78: ସ୍ଥିତି 0x1e7a:
	    ସ୍ଥିତି 0x1ee4: ସ୍ଥିତି 0x1ee6: ସ୍ଥିତି 0x1ee8: ସ୍ଥିତି 0x1eea:
	    ସ୍ଥିତି 0x1eec: ସ୍ଥିତି 0x1eee: ସ୍ଥିତି 0x1ef0:
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
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'V': ସ୍ଥିତି 0x1b2: ସ୍ଥିତି 0x1e7c: ସ୍ଥିତି 0x1e7e:
		      regmbc('V'); regmbc(0x1b2); regmbc(0x1e7c);
		      regmbc(0x1e7e);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'W': ସ୍ଥିତି 0x174: ସ୍ଥିତି 0x1e80: ସ୍ଥିତି 0x1e82:
	    ସ୍ଥିତି 0x1e84: ସ୍ଥିତି 0x1e86: ସ୍ଥିତି 0x1e88:
		      regmbc('W'); regmbc(0x174); regmbc(0x1e80);
		      regmbc(0x1e82); regmbc(0x1e84); regmbc(0x1e86);
		      regmbc(0x1e88);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'X': ସ୍ଥିତି 0x1e8a: ସ୍ଥିତି 0x1e8c:
		      regmbc('X'); regmbc(0x1e8a); regmbc(0x1e8c);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'Y': ସ୍ଥିତି 0xdd:
	    ସ୍ଥିତି 0x176: ସ୍ଥିତି 0x178: ସ୍ଥିତି 0x1b3: ସ୍ଥିତି 0x232: ସ୍ଥିତି 0x24e:
	    ସ୍ଥିତି 0x1e8e: ସ୍ଥିତି 0x1ef2: ସ୍ଥିତି 0x1ef6: ସ୍ଥିତି 0x1ef4: ସ୍ଥିତି 0x1ef8:
		      regmbc('Y'); regmbc(0xdd); regmbc(0x176);
		      regmbc(0x178); regmbc(0x1b3); regmbc(0x232);
		      regmbc(0x24e); regmbc(0x1e8e); regmbc(0x1ef2);
		      regmbc(0x1ef4); regmbc(0x1ef6); regmbc(0x1ef8);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'Z': ସ୍ଥିତି 0x179: ସ୍ଥିତି 0x17b: ସ୍ଥିତି 0x17d: ସ୍ଥିତି 0x1b5:
	    ସ୍ଥିତି 0x1e90: ସ୍ଥିତି 0x1e92: ସ୍ଥିତି 0x1e94: ସ୍ଥିତି 0x2c6b:
		      regmbc('Z'); regmbc(0x179); regmbc(0x17b);
		      regmbc(0x17d); regmbc(0x1b5); regmbc(0x1e90);
		      regmbc(0x1e92); regmbc(0x1e94); regmbc(0x2c6b);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'a': ସ୍ଥିତି 0xe0: ସ୍ଥିତି 0xe1: ସ୍ଥିତି 0xe2:
	    ସ୍ଥିତି 0xe3: ସ୍ଥିତି 0xe4: ସ୍ଥିତି 0xe5: ସ୍ଥିତି 0x101: ସ୍ଥିତି 0x103:
	    ସ୍ଥିତି 0x105: ସ୍ଥିତି 0x1ce: ସ୍ଥିତି 0x1df: ସ୍ଥିତି 0x1e1: ସ୍ଥିତି 0x1fb:
	    ସ୍ଥିତି 0x201: ସ୍ଥିତି 0x203: ସ୍ଥିତି 0x227: ସ୍ଥିତି 0x1d8f: ସ୍ଥିତି 0x1e01:
	    ସ୍ଥିତି 0x1e9a: ସ୍ଥିତି 0x1ea1: ସ୍ଥିତି 0x1ea3: ସ୍ଥିତି 0x1ea5:
	    ସ୍ଥିତି 0x1ea7: ସ୍ଥିତି 0x1ea9: ସ୍ଥିତି 0x1eab: ସ୍ଥିତି 0x1ead:
	    ସ୍ଥିତି 0x1eaf: ସ୍ଥିତି 0x1eb1: ସ୍ଥିତି 0x1eb3: ସ୍ଥିତି 0x1eb5:
	    ସ୍ଥିତି 0x1eb7: ସ୍ଥିତି 0x2c65:
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
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'b': ସ୍ଥିତି 0x180: ସ୍ଥିତି 0x253: ସ୍ଥିତି 0x1d6c: ସ୍ଥିତି 0x1d80:
	    ସ୍ଥିତି 0x1e03: ସ୍ଥିତି 0x1e05: ସ୍ଥିତି 0x1e07:
		      regmbc('b');
		      regmbc(0x180); regmbc(0x253); regmbc(0x1d6c);
		      regmbc(0x1d80); regmbc(0x1e03); regmbc(0x1e05);
		      regmbc(0x1e07);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'c': ସ୍ଥିତି 0xe7:
	    ସ୍ଥିତି 0x107: ସ୍ଥିତି 0x109: ସ୍ଥିତି 0x10b: ସ୍ଥିତି 0x10d: ସ୍ଥିତି 0x188:
	    ସ୍ଥିତି 0x23c: ସ୍ଥିତି 0x1e09: ସ୍ଥିତି 0xa793: ସ୍ଥିତି 0xa794:
		      regmbc('c'); regmbc(0xe7); regmbc(0x107);
		      regmbc(0x109); regmbc(0x10b); regmbc(0x10d);
		      regmbc(0x188); regmbc(0x23c); regmbc(0x1e09);
		      regmbc(0xa793); regmbc(0xa794);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'd': ସ୍ଥିତି 0x10f: ସ୍ଥିତି 0x111: ସ୍ଥିତି 0x257: ସ୍ଥିତି 0x1d6d:
	    ସ୍ଥିତି 0x1d81: ସ୍ଥିତି 0x1d91: ସ୍ଥିତି 0x1e0b: ସ୍ଥିତି 0x1e0d:
	    ସ୍ଥିତି 0x1e0f: ସ୍ଥିତି 0x1e11: ସ୍ଥିତି 0x1e13:
		      regmbc('d'); regmbc(0x10f); regmbc(0x111);
		      regmbc(0x257); regmbc(0x1d6d); regmbc(0x1d81);
		      regmbc(0x1d91); regmbc(0x1e0b); regmbc(0x1e0d);
		      regmbc(0x1e0f); regmbc(0x1e11); regmbc(0x1e13);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'e': ସ୍ଥିତି 0xe8: ସ୍ଥିତି 0xe9: ସ୍ଥିତି 0xea: ସ୍ଥିତି 0xeb:
	    ସ୍ଥିତି 0x113: ସ୍ଥିତି 0x115: ସ୍ଥିତି 0x117: ସ୍ଥିତି 0x119:
	    ସ୍ଥିତି 0x11b: ସ୍ଥିତି 0x205: ସ୍ଥିତି 0x207: ସ୍ଥିତି 0x229:
	    ସ୍ଥିତି 0x247: ସ୍ଥିତି 0x1d92: ସ୍ଥିତି 0x1e15: ସ୍ଥିତି 0x1e17:
	    ସ୍ଥିତି 0x1e19: ସ୍ଥିତି 0x1e1b: ସ୍ଥିତି 0x1eb9: ସ୍ଥିତି 0x1ebb:
	    ସ୍ଥିତି 0x1e1d: ସ୍ଥିତି 0x1ebd: ସ୍ଥିତି 0x1ebf: ସ୍ଥିତି 0x1ec1:
	    ସ୍ଥିତି 0x1ec3: ସ୍ଥିତି 0x1ec5: ସ୍ଥିତି 0x1ec7:
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
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'f': ସ୍ଥିତି 0x192: ସ୍ଥିତି 0x1d6e: ସ୍ଥିତି 0x1d82:
	    ସ୍ଥିତି 0x1e1f: ସ୍ଥିତି 0xa799:
		     regmbc('f'); regmbc(0x192); regmbc(0x1d6e);
		     regmbc(0x1d82); regmbc(0x1e1f); regmbc(0xa799);
		     ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'g': ସ୍ଥିତି 0x11d: ସ୍ଥିତି 0x11f: ସ୍ଥିତି 0x121: ସ୍ଥିତି 0x123:
	    ସ୍ଥିତି 0x1e5: ସ୍ଥିତି 0x1e7: ସ୍ଥିତି 0x260: ସ୍ଥିତି 0x1f5: ସ୍ଥିତି 0x1d83:
	    ସ୍ଥିତି 0x1e21: ସ୍ଥିତି 0xa7a1:
		      regmbc('g'); regmbc(0x11d); regmbc(0x11f);
		      regmbc(0x121); regmbc(0x123); regmbc(0x1e5);
		      regmbc(0x1e7); regmbc(0x1f5); regmbc(0x260);
		      regmbc(0x1d83); regmbc(0x1e21); regmbc(0xa7a1);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'h': ସ୍ଥିତି 0x125: ସ୍ଥିତି 0x127: ସ୍ଥିତି 0x21f: ସ୍ଥିତି 0x1e23:
	    ସ୍ଥିତି 0x1e25: ସ୍ଥିତି 0x1e27: ସ୍ଥିତି 0x1e29: ସ୍ଥିତି 0x1e2b:
	    ସ୍ଥିତି 0x1e96: ସ୍ଥିତି 0x2c68: ସ୍ଥିତି 0xa795:
		      regmbc('h'); regmbc(0x125); regmbc(0x127);
		      regmbc(0x21f); regmbc(0x1e23); regmbc(0x1e25);
		      regmbc(0x1e27); regmbc(0x1e29); regmbc(0x1e2b);
		      regmbc(0x1e96); regmbc(0x2c68); regmbc(0xa795);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'i': ସ୍ଥିତି 0xec: ସ୍ଥିତି 0xed: ସ୍ଥିତି 0xee: ସ୍ଥିତି 0xef:
	    ସ୍ଥିତି 0x129: ସ୍ଥିତି 0x12b: ସ୍ଥିତି 0x12d: ସ୍ଥିତି 0x12f:
	    ସ୍ଥିତି 0x1d0: ସ୍ଥିତି 0x209: ସ୍ଥିତି 0x20b: ସ୍ଥିତି 0x268:
	    ସ୍ଥିତି 0x1d96: ସ୍ଥିତି 0x1e2d: ସ୍ଥିତି 0x1e2f: ସ୍ଥିତି 0x1ec9:
	    ସ୍ଥିତି 0x1ecb:
		      regmbc('i'); regmbc(0xec); regmbc(0xed);
		      regmbc(0xee); regmbc(0xef); regmbc(0x129);
		      regmbc(0x12b); regmbc(0x12d); regmbc(0x12f);
		      regmbc(0x1d0); regmbc(0x209); regmbc(0x20b);
		      regmbc(0x268); regmbc(0x1d96); regmbc(0x1e2d);
		      regmbc(0x1e2f); regmbc(0x1ec9); regmbc(0x1ecb);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'j': ସ୍ଥିତି 0x135: ସ୍ଥିତି 0x1f0: ସ୍ଥିତି 0x249:
		      regmbc('j'); regmbc(0x135); regmbc(0x1f0);
		      regmbc(0x249);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'k': ସ୍ଥିତି 0x137: ସ୍ଥିତି 0x199: ସ୍ଥିତି 0x1e9:
	    ସ୍ଥିତି 0x1d84: ସ୍ଥିତି 0x1e31: ସ୍ଥିତି 0x1e33: ସ୍ଥିତି 0x1e35:
	    ସ୍ଥିତି 0x2c6a: ସ୍ଥିତି 0xa741:
		      regmbc('k'); regmbc(0x137); regmbc(0x199);
		      regmbc(0x1e9); regmbc(0x1d84); regmbc(0x1e31);
		      regmbc(0x1e33); regmbc(0x1e35); regmbc(0x2c6a);
		      regmbc(0xa741);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'l': ସ୍ଥିତି 0x13a: ସ୍ଥିତି 0x13c: ସ୍ଥିତି 0x13e:
	    ସ୍ଥିତି 0x140: ସ୍ଥିତି 0x142: ସ୍ଥିତି 0x19a: ସ୍ଥିତି 0x1e37:
	    ସ୍ଥିତି 0x1e39: ସ୍ଥିତି 0x1e3b: ସ୍ଥିତି 0x1e3d: ସ୍ଥିତି 0x2c61:
		      regmbc('l'); regmbc(0x13a); regmbc(0x13c);
		      regmbc(0x13e); regmbc(0x140); regmbc(0x142);
		      regmbc(0x19a); regmbc(0x1e37); regmbc(0x1e39);
		      regmbc(0x1e3b); regmbc(0x1e3d); regmbc(0x2c61);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'm': ସ୍ଥିତି 0x1d6f: ସ୍ଥିତି 0x1e3f: ସ୍ଥିତି 0x1e41: ସ୍ଥିତି 0x1e43:
		      regmbc('m'); regmbc(0x1d6f); regmbc(0x1e3f);
		      regmbc(0x1e41); regmbc(0x1e43);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'n': ସ୍ଥିତି 0xf1: ସ୍ଥିତି 0x144: ସ୍ଥିତି 0x146: ସ୍ଥିତି 0x148:
	    ସ୍ଥିତି 0x149: ସ୍ଥିତି 0x1f9: ସ୍ଥିତି 0x1d70: ସ୍ଥିତି 0x1d87:
	    ସ୍ଥିତି 0x1e45: ସ୍ଥିତି 0x1e47: ସ୍ଥିତି 0x1e49: ସ୍ଥିତି 0x1e4b:
	    ସ୍ଥିତି 0xa7a5:
		      regmbc('n'); regmbc(0xf1); regmbc(0x144);
		      regmbc(0x146); regmbc(0x148); regmbc(0x149);
		      regmbc(0x1f9); regmbc(0x1d70); regmbc(0x1d87);
		      regmbc(0x1e45); regmbc(0x1e47); regmbc(0x1e49);
		      regmbc(0x1e4b); regmbc(0xa7a5);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'o': ସ୍ଥିତି 0xf2: ସ୍ଥିତି 0xf3: ସ୍ଥିତି 0xf4: ସ୍ଥିତି 0xf5:
	    ସ୍ଥିତି 0xf6: ସ୍ଥିତି 0xf8: ସ୍ଥିତି 0x14d: ସ୍ଥିତି 0x14f: ସ୍ଥିତି 0x151:
	    ସ୍ଥିତି 0x1a1: ସ୍ଥିତି 0x1d2: ସ୍ଥିତି 0x1eb: ସ୍ଥିତି 0x1ed: ସ୍ଥିତି 0x1ff:
	    ସ୍ଥିତି 0x20d: ସ୍ଥିତି 0x20f: ସ୍ଥିତି 0x22b: ସ୍ଥିତି 0x22d: ସ୍ଥିତି 0x22f:
	    ସ୍ଥିତି 0x231: ସ୍ଥିତି 0x275: ସ୍ଥିତି 0x1e4d: ସ୍ଥିତି 0x1e4f:
	    ସ୍ଥିତି 0x1e51: ସ୍ଥିତି 0x1e53: ସ୍ଥିତି 0x1ecd: ସ୍ଥିତି 0x1ecf:
	    ସ୍ଥିତି 0x1ed1: ସ୍ଥିତି 0x1ed3: ସ୍ଥିତି 0x1ed5: ସ୍ଥିତି 0x1ed7:
	    ସ୍ଥିତି 0x1ed9: ସ୍ଥିତି 0x1edb: ସ୍ଥିତି 0x1edd: ସ୍ଥିତି 0x1edf:
	    ସ୍ଥିତି 0x1ee1: ସ୍ଥିତି 0x1ee3:
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
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'p': ସ୍ଥିତି 0x1a5: ସ୍ଥିତି 0x1d71: ସ୍ଥିତି 0x1d88: ସ୍ଥିତି 0x1d7d:
	    ସ୍ଥିତି 0x1e55: ସ୍ଥିତି 0x1e57:
		      regmbc('p'); regmbc(0x1a5); regmbc(0x1d71);
		      regmbc(0x1d7d); regmbc(0x1d88); regmbc(0x1e55);
		      regmbc(0x1e57);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'q': ସ୍ଥିତି 0x24b: ସ୍ଥିତି 0x2a0:
		      regmbc('q'); regmbc(0x24b); regmbc(0x2a0);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'r': ସ୍ଥିତି 0x155: ସ୍ଥିତି 0x157: ସ୍ଥିତି 0x159: ସ୍ଥିତି 0x211:
	    ସ୍ଥିତି 0x213: ସ୍ଥିତି 0x24d: ସ୍ଥିତି 0x27d: ସ୍ଥିତି 0x1d72: ସ୍ଥିତି 0x1d73:
	    ସ୍ଥିତି 0x1d89: ସ୍ଥିତି 0x1e59: ସ୍ଥିତି 0x1e5b: ସ୍ଥିତି 0x1e5d: ସ୍ଥିତି 0x1e5f:
	    ସ୍ଥିତି 0xa7a7:
		      regmbc('r'); regmbc(0x155); regmbc(0x157);
		      regmbc(0x159); regmbc(0x211); regmbc(0x213);
		      regmbc(0x24d); regmbc(0x1d72); regmbc(0x1d73);
		      regmbc(0x1d89); regmbc(0x1e59); regmbc(0x27d);
		      regmbc(0x1e5b); regmbc(0x1e5d); regmbc(0x1e5f);
		      regmbc(0xa7a7);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 's': ସ୍ଥିତି 0x15b: ସ୍ଥିତି 0x15d: ସ୍ଥିତି 0x15f: ସ୍ଥିତି 0x161:
	    ସ୍ଥିତି 0x1e61: ସ୍ଥିତି 0x219: ସ୍ଥିତି 0x23f: ସ୍ଥିତି 0x1d74: ସ୍ଥିତି 0x1d8a:
	    ସ୍ଥିତି 0x1e63: ସ୍ଥିତି 0x1e65: ସ୍ଥିତି 0x1e67: ସ୍ଥିତି 0x1e69: ସ୍ଥିତି 0xa7a9:
		      regmbc('s'); regmbc(0x15b); regmbc(0x15d);
		      regmbc(0x15f); regmbc(0x161); regmbc(0x23f);
		      regmbc(0x219); regmbc(0x1d74); regmbc(0x1d8a);
		      regmbc(0x1e61); regmbc(0x1e63); regmbc(0x1e65);
		      regmbc(0x1e67); regmbc(0x1e69); regmbc(0xa7a9);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 't': ସ୍ଥିତି 0x163: ସ୍ଥିତି 0x165: ସ୍ଥିତି 0x167: ସ୍ଥିତି 0x1ab:
	    ସ୍ଥିତି 0x1ad: ସ୍ଥିତି 0x21b: ସ୍ଥିତି 0x288: ସ୍ଥିତି 0x1d75: ସ୍ଥିତି 0x1e6b:
	    ସ୍ଥିତି 0x1e6d: ସ୍ଥିତି 0x1e6f: ସ୍ଥିତି 0x1e71: ସ୍ଥିତି 0x1e97: ସ୍ଥିତି 0x2c66:
		      regmbc('t'); regmbc(0x163); regmbc(0x165);
		      regmbc(0x167); regmbc(0x1ab); regmbc(0x21b);
		      regmbc(0x1ad); regmbc(0x288); regmbc(0x1d75);
		      regmbc(0x1e6b); regmbc(0x1e6d); regmbc(0x1e6f);
		      regmbc(0x1e71); regmbc(0x1e97); regmbc(0x2c66);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'u': ସ୍ଥିତି 0xf9: ସ୍ଥିତି 0xfa: ସ୍ଥିତି 0xfb: ସ୍ଥିତି 0xfc:
	    ସ୍ଥିତି 0x169: ସ୍ଥିତି 0x16b: ସ୍ଥିତି 0x16d: ସ୍ଥିତି 0x16f:
	    ସ୍ଥିତି 0x171: ସ୍ଥିତି 0x173: ସ୍ଥିତି 0x1b0: ସ୍ଥିତି 0x1d4:
	    ସ୍ଥିତି 0x1d6: ସ୍ଥିତି 0x1d8: ସ୍ଥିତି 0x1da: ସ୍ଥିତି 0x1dc:
	    ସ୍ଥିତି 0x215: ସ୍ଥିତି 0x217: ସ୍ଥିତି 0x289: ସ୍ଥିତି 0x1e73:
	    ସ୍ଥିତି 0x1d7e: ସ୍ଥିତି 0x1d99: ସ୍ଥିତି 0x1e75: ସ୍ଥିତି 0x1e77:
	    ସ୍ଥିତି 0x1e79: ସ୍ଥିତି 0x1e7b: ସ୍ଥିତି 0x1ee5: ସ୍ଥିତି 0x1ee7:
	    ସ୍ଥିତି 0x1ee9: ସ୍ଥିତି 0x1eeb: ସ୍ଥିତି 0x1eed: ସ୍ଥିତି 0x1eef:
	    ସ୍ଥିତି 0x1ef1:
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
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'v': ସ୍ଥିତି 0x28b: ସ୍ଥିତି 0x1d8c: ସ୍ଥିତି 0x1e7d: ସ୍ଥିତି 0x1e7f:
		      regmbc('v'); regmbc(0x28b); regmbc(0x1d8c);
		      regmbc(0x1e7d); regmbc(0x1e7f);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'w': ସ୍ଥିତି 0x175: ସ୍ଥିତି 0x1e81: ସ୍ଥିତି 0x1e83:
	    ସ୍ଥିତି 0x1e85: ସ୍ଥିତି 0x1e87: ସ୍ଥିତି 0x1e89: ସ୍ଥିତି 0x1e98:
		      regmbc('w'); regmbc(0x175); regmbc(0x1e81);
		      regmbc(0x1e83); regmbc(0x1e85); regmbc(0x1e87);
		      regmbc(0x1e89); regmbc(0x1e98);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'x': ସ୍ଥିତି 0x1e8b: ସ୍ଥିତି 0x1e8d:
		      regmbc('x'); regmbc(0x1e8b); regmbc(0x1e8d);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'y': ସ୍ଥିତି 0xfd: ସ୍ଥିତି 0xff: ସ୍ଥିତି 0x177: ସ୍ଥିତି 0x1b4:
	    ସ୍ଥିତି 0x233: ସ୍ଥିତି 0x24f: ସ୍ଥିତି 0x1e8f: ସ୍ଥିତି 0x1e99: ସ୍ଥିତି 0x1ef3:
	    ସ୍ଥିତି 0x1ef5: ସ୍ଥିତି 0x1ef7: ସ୍ଥିତି 0x1ef9:
		      regmbc('y'); regmbc(0xfd); regmbc(0xff);
		      regmbc(0x177); regmbc(0x1b4); regmbc(0x233);
		      regmbc(0x24f); regmbc(0x1e8f); regmbc(0x1e99);
		      regmbc(0x1ef3); regmbc(0x1ef5); regmbc(0x1ef7);
		      regmbc(0x1ef9);
		      ଫେରନ୍ତୁ;
	    ସ୍ଥିତି 'z': ସ୍ଥିତି 0x17a: ସ୍ଥିତି 0x17c: ସ୍ଥିତି 0x17e: ସ୍ଥିତି 0x1b6:
	    ସ୍ଥିତି 0x1d76: ସ୍ଥିତି 0x1d8e: ସ୍ଥିତି 0x1e91: ସ୍ଥିତି 0x1e93:
	    ସ୍ଥିତି 0x1e95: ସ୍ଥିତି 0x2c6c:
		      regmbc('z'); regmbc(0x17a); regmbc(0x17c);
		      regmbc(0x17e); regmbc(0x1b6); regmbc(0x1d76);
		      regmbc(0x1d8e); regmbc(0x1e91); regmbc(0x1e93);
		      regmbc(0x1e95); regmbc(0x2c6c);
		      ଫେରନ୍ତୁ;
	}
    }
    regmbc(c);
}

/*
 * Emit a node.
 * Return pointer to generated code.
 */
    ସ୍ଥିର char_u *
regnode(ପୂର୍ଣ୍ଣସଂଖ୍ୟା op)
{
    char_u  *ret;

    ret = regcode;
    ଯଦି (ret == JUST_CALC_SIZE)
	regsize += 3;
    ଅନ୍ୟଥା
    {
	*regcode++ = op;
	*regcode++ = NUL;		// Null "next" pointer.
	*regcode++ = NUL;
    }
    ଫେରନ୍ତୁ ret;
}

/*
 * Write a long as four bytes at "p" and return pointer to the next char.
 */
    ସ୍ଥିର char_u *
re_put_long(char_u *p, long_u val)
{
    *p++ = (char_u) ((val >> 24) & 0377);
    *p++ = (char_u) ((val >> 16) & 0377);
    *p++ = (char_u) ((val >> 8) & 0377);
    *p++ = (char_u) (val & 0377);
    ଫେରନ୍ତୁ p;
}

/*
 * regnext - dig the "next" pointer out of a node
 * Returns NULL when calculating size, when there is no next item and when
 * there is an error.
 */
    ସ୍ଥିର char_u *
regnext(char_u *p)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    offset;

    ଯଦି (p == JUST_CALC_SIZE || reg_toolong)
	ଫେରନ୍ତୁ ଖାଲି;

    offset = NEXT(p);
    ଯଦି (offset == 0)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (OP(p) == BACK)
	ଫେରନ୍ତୁ p - offset;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ p + offset;
}

/*
 * Set the next-pointer at the end of a node chain.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
regtail(char_u *p, char_u *val)
{
    char_u	*scan;
    char_u	*temp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		offset;

    ଯଦି (p == JUST_CALC_SIZE)
	ଫେରନ୍ତୁ;

    // Find last node.
    scan = p;
    ପାଇଁ (;;)
    {
	temp = regnext(scan);
	ଯଦି (temp == ଖାଲି)
	    ଭାଙ୍ଗ;
	scan = temp;
    }

    ଯଦି (OP(scan) == BACK)
	offset = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(scan - val);
    ଅନ୍ୟଥା
	offset = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(val - scan);
    // When the offset uses more than 16 bits it can no longer fit in the two
    // bytes available.  Use a global flag to avoid having to check return
    // values in too many places.
    ଯଦି (offset > 0xffff)
	reg_toolong = TRUE;
    ଅନ୍ୟଥା
    {
	*(scan + 1) = (char_u) (((unsigned)offset >> 8) & 0377);
	*(scan + 2) = (char_u) (offset & 0377);
    }
}

/*
 * Like regtail, on item after a BRANCH; nop if none.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
regoptail(char_u *p, char_u *val)
{
    // When op is neither BRANCH nor BRACE_COMPLEX0-9, it is "operandless"
    ଯଦି (p == ଖାଲି || p == JUST_CALC_SIZE
	    || (OP(p) != BRANCH
		&& (OP(p) < BRACE_COMPLEX || OP(p) > BRACE_COMPLEX + 9)))
	ଫେରନ୍ତୁ;
    regtail(OPERAND(p), val);
}

/*
 * Insert an operator in front of already-emitted operand
 *
 * Means relocating the operand.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
reginsert(ପୂର୍ଣ୍ଣସଂଖ୍ୟା op, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    ଯଦି (regcode == JUST_CALC_SIZE)
    {
	regsize += 3;
	ଫେରନ୍ତୁ;
    }
    src = regcode;
    regcode += 3;
    dst = regcode;
    ଯେତେବେଳେ (src > opnd)
	*--dst = *--src;

    place = opnd;		// Op node, where operand used to be.
    *place++ = op;
    *place++ = NUL;
    *place = NUL;
}

/*
 * Insert an operator in front of already-emitted operand.
 * Add a number to the operator.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
reginsert_nr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା op, long val, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    ଯଦି (regcode == JUST_CALC_SIZE)
    {
	regsize += 7;
	ଫେରନ୍ତୁ;
    }
    src = regcode;
    regcode += 7;
    dst = regcode;
    ଯେତେବେଳେ (src > opnd)
	*--dst = *--src;

    place = opnd;		// Op node, where operand used to be.
    *place++ = op;
    *place++ = NUL;
    *place++ = NUL;
    re_put_long(place, (long_u)val);
}

/*
 * Insert an operator in front of already-emitted operand.
 * The operator has the given limit values as operands.  Also set next pointer.
 *
 * Means relocating the operand.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
reginsert_limits(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		op,
    long	minval,
    long	maxval,
    char_u	*opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    ଯଦି (regcode == JUST_CALC_SIZE)
    {
	regsize += 11;
	ଫେରନ୍ତୁ;
    }
    src = regcode;
    regcode += 11;
    dst = regcode;
    ଯେତେବେଳେ (src > opnd)
	*--dst = *--src;

    place = opnd;		// Op node, where operand used to be.
    *place++ = op;
    *place++ = NUL;
    *place++ = NUL;
    place = re_put_long(place, (long_u)minval);
    place = re_put_long(place, (long_u)maxval);
    regtail(opnd, place);
}

/*
 * Return TRUE if the back reference is legal. We must have seen the close
 * brace.
 * TODO: Should also check that we don't refer to something that is repeated
 * (+*=): what instance of the repetition should we match?
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
seen_endbrace(ପୂର୍ଣ୍ଣସଂଖ୍ୟା refnum)
{
    ଯଦି (!had_endbrace[refnum])
    {
	char_u *p;

	// Trick: check if "@<=" or "@<!" follows, in which case
	// the \1 can appear before the referenced match.
	ପାଇଁ (p = regparse; *p != NUL; ++p)
	    ଯଦି (p[0] == '@' && p[1] == '<' && (p[2] == '!' || p[2] == '='))
		ଭାଙ୍ଗ;
	ଯଦି (*p == NUL)
	{
	    emsg(_(e_illegal_back_reference));
	    rc_did_emsg = TRUE;
	    ଫେରନ୍ତୁ FALSE;
	}
    }
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Parse the lowest level.
 *
 * Optimization:  gobbles an entire sequence of ordinary characters so that
 * it can turn them into a single node, which is smaller to store and
 * faster to run.  Don't do this when one_exactly is set.
 */
    ସ୍ଥିର char_u *
regatom(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *flagp)
{
    char_u	    *ret;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    flags;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    c;
    char_u	    *p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    extra = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    save_prev_at_start = prev_at_start;

    *flagp = WORST;		// Tentatively.

    c = getchr();
    ବାଛ (c)
    {
      ସ୍ଥିତି Magic('^'):
	ret = regnode(BOL);
	ଭାଙ୍ଗ;

      ସ୍ଥିତି Magic('$'):
	ret = regnode(EOL);
#ଯଦି defined(FEAT_SYN_HL)
	had_eol = TRUE;
#endif
	ଭାଙ୍ଗ;

      ସ୍ଥିତି Magic('<'):
	ret = regnode(BOW);
	ଭାଙ୍ଗ;

      ସ୍ଥିତି Magic('>'):
	ret = regnode(EOW);
	ଭାଙ୍ଗ;

      ସ୍ଥିତି Magic('_'):
	c = no_Magic(getchr());
	ଯଦି (c == '^')		// "\_^" is start-of-line
	{
	    ret = regnode(BOL);
	    ଭାଙ୍ଗ;
	}
	ଯଦି (c == '$')		// "\_$" is end-of-line
	{
	    ret = regnode(EOL);
#ଯଦି defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#endif
	    ଭାଙ୍ଗ;
	}

	extra = ADD_NL;
	*flagp |= HASNL;

	// "\_[" is character range plus newline
	ଯଦି (c == '[')
	    ଯାଅ collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	// Character classes.
      ସ୍ଥିତି Magic('.'):
      ସ୍ଥିତି Magic('i'):
      ସ୍ଥିତି Magic('I'):
      ସ୍ଥିତି Magic('k'):
      ସ୍ଥିତି Magic('K'):
      ସ୍ଥିତି Magic('f'):
      ସ୍ଥିତି Magic('F'):
      ସ୍ଥିତି Magic('p'):
      ସ୍ଥିତି Magic('P'):
      ସ୍ଥିତି Magic('s'):
      ସ୍ଥିତି Magic('S'):
      ସ୍ଥିତି Magic('d'):
      ସ୍ଥିତି Magic('D'):
      ସ୍ଥିତି Magic('x'):
      ସ୍ଥିତି Magic('X'):
      ସ୍ଥିତି Magic('o'):
      ସ୍ଥିତି Magic('O'):
      ସ୍ଥିତି Magic('w'):
      ସ୍ଥିତି Magic('W'):
      ସ୍ଥିତି Magic('h'):
      ସ୍ଥିତି Magic('H'):
      ସ୍ଥିତି Magic('a'):
      ସ୍ଥିତି Magic('A'):
      ସ୍ଥିତି Magic('l'):
      ସ୍ଥିତି Magic('L'):
      ସ୍ଥିତି Magic('u'):
      ସ୍ଥିତି Magic('U'):
	p = vim_strchr(classchars, no_Magic(c));
	ଯଦି (p == ଖାଲି)
	    EMSG_RET_NULL(_(e_invalid_use_of_underscore));

	// When '.' is followed by a composing char ignore the dot, so that
	// the composing char is matched here.
	ଯଦି (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	{
	    c = getchr();
	    ଯାଅ do_multibyte;
	}
	ret = regnode(classcodes[p - classchars] + extra);
	*flagp |= HASWIDTH | SIMPLE;
	ଭାଙ୍ଗ;

      ସ୍ଥିତି Magic('n'):
	ଯଦି (reg_string)
	{
	    // In a string "\n" matches a newline character.
	    ret = regnode(EXACTLY);
	    regc(NL);
	    regc(NUL);
	    *flagp |= HASWIDTH | SIMPLE;
	}
	ଅନ୍ୟଥା
	{
	    // In buffer text "\n" matches the end of a line.
	    ret = regnode(NEWL);
	    *flagp |= HASWIDTH | HASNL;
	}
	ଭାଙ୍ଗ;

      ସ୍ଥିତି Magic('('):
	ଯଦି (one_exactly)
	    EMSG_ONE_RET_NULL;
	ret = reg(REG_PAREN, &flags);
	ଯଦି (ret == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	*flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
	ଭାଙ୍ଗ;

      ସ୍ଥିତି NUL:
      ସ୍ଥିତି Magic('|'):
      ସ୍ଥିତି Magic('&'):
      ସ୍ଥିତି Magic(')'):
	ଯଦି (one_exactly)
	    EMSG_ONE_RET_NULL;
	// Supposed to be caught earlier.
	IEMSG_RET_NULL(e_internal_error_in_regexp);
	// NOTREACHED

      ସ୍ଥିତି Magic('='):
      ସ୍ଥିତି Magic('?'):
      ସ୍ଥିତି Magic('+'):
      ସ୍ଥିତି Magic('@'):
      ସ୍ଥିତି Magic('{'):
      ସ୍ଥିତି Magic('*'):
	c = no_Magic(c);
	EMSG3_RET_NULL(_(e_str_chr_follows_nothing),
		(c == '*' ? reg_magic >= MAGIC_ON : reg_magic == MAGIC_ALL), c);
	// NOTREACHED

      ସ୍ଥିତି Magic('~'):		// previous substitute pattern
	    ଯଦି (reg_prev_sub != ଖାଲି)
	    {
		char_u	    *lp;

		ret = regnode(EXACTLY);
		lp = reg_prev_sub;
		ଯେତେବେଳେ (*lp != NUL)
		    regc(*lp++);
		regc(NUL);
		ଯଦି (*reg_prev_sub != NUL)
		{
		    *flagp |= HASWIDTH;
		    ଯଦି ((lp - reg_prev_sub) == 1)
			*flagp |= SIMPLE;
		}
	    }
	    ଅନ୍ୟଥା
		EMSG_RET_NULL(_(e_no_previous_substitute_regular_expression));
	    ଭାଙ୍ଗ;

      ସ୍ଥିତି Magic('1'):
      ସ୍ଥିତି Magic('2'):
      ସ୍ଥିତି Magic('3'):
      ସ୍ଥିତି Magic('4'):
      ସ୍ଥିତି Magic('5'):
      ସ୍ଥିତି Magic('6'):
      ସ୍ଥିତି Magic('7'):
      ସ୍ଥିତି Magic('8'):
      ସ୍ଥିତି Magic('9'):
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    refnum;

		refnum = c - Magic('0');
		ଯଦି (!seen_endbrace(refnum))
		    ଫେରନ୍ତୁ ଖାଲି;
		ret = regnode(BACKREF + refnum);
	    }
	    ଭାଙ୍ଗ;

      ସ୍ଥିତି Magic('z'):
	{
	    c = no_Magic(getchr());
	    ବାଛ (c)
	    {
#ifdef FEAT_SYN_HL
		ସ୍ଥିତି '(': ଯଦି ((reg_do_extmatch & REX_SET) == 0)
			      EMSG_RET_NULL(_(e_z_not_allowed_here));
			  ଯଦି (one_exactly)
			      EMSG_ONE_RET_NULL;
			  ret = reg(REG_ZPAREN, &flags);
			  ଯଦି (ret == ଖାଲି)
			      ଫେରନ୍ତୁ ଖାଲି;
			  *flagp |= flags & (HASWIDTH|SPSTART|HASNL|HASLOOKBH);
			  re_has_z = REX_SET;
			  ଭାଙ୍ଗ;

		ସ୍ଥିତି '1':
		ସ୍ଥିତି '2':
		ସ୍ଥିତି '3':
		ସ୍ଥିତି '4':
		ସ୍ଥିତି '5':
		ସ୍ଥିତି '6':
		ସ୍ଥିତି '7':
		ସ୍ଥିତି '8':
		ସ୍ଥିତି '9': ଯଦି ((reg_do_extmatch & REX_USE) == 0)
			      EMSG_RET_NULL(_(e_z1_z9_not_allowed_here));
			  ret = regnode(ZREF + c - '0');
			  re_has_z = REX_USE;
			  ଭାଙ୍ଗ;
#endif

		ସ୍ଥିତି 's': ret = regnode(MOPEN + 0);
			  ଯଦି (re_mult_next("\\zs") == FAIL)
			      ଫେରନ୍ତୁ ଖାଲି;
			  ଭାଙ୍ଗ;

		ସ୍ଥିତି 'e': ret = regnode(MCLOSE + 0);
			  ଯଦି (re_mult_next("\\ze") == FAIL)
			      ଫେରନ୍ତୁ ଖାଲି;
			  ଭାଙ୍ଗ;

		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  EMSG_RET_NULL(_(e_invalid_character_after_bsl_z));
	    }
	}
	ଭାଙ୍ଗ;

      ସ୍ଥିତି Magic('%'):
	{
	    c = no_Magic(getchr());
	    ବାଛ (c)
	    {
		// () without a back reference
		ସ୍ଥିତି '(':
		    ଯଦି (one_exactly)
			EMSG_ONE_RET_NULL;
		    ret = reg(REG_NPAREN, &flags);
		    ଯଦି (ret == ଖାଲି)
			ଫେରନ୍ତୁ ଖାଲି;
		    *flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
		    ଭାଙ୍ଗ;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		ସ୍ଥିତି '^':
		    ret = regnode(RE_BOF);
		    ଭାଙ୍ଗ;

		ସ୍ଥିତି '$':
		    ret = regnode(RE_EOF);
		    ଭାଙ୍ଗ;

		ସ୍ଥିତି '#':
		    ଯଦି (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_start_of_pattern),
								  regparse[1]);
			ଫେରନ୍ତୁ FAIL;
		    }
		    ret = regnode(CURSOR);
		    ଭାଙ୍ଗ;

		ସ୍ଥିତି 'V':
		    ret = regnode(RE_VISUAL);
		    ଭାଙ୍ଗ;

		ସ୍ଥିତି 'C':
		    ret = regnode(RE_COMPOSING);
		    ଭାଙ୍ଗ;

		// \%[abc]: Emit as a list of branches, all ending at the last
		// branch which matches nothing.
		ସ୍ଥିତି '[':
			  ଯଦି (one_exactly)	// doesn't nest
			      EMSG_ONE_RET_NULL;
			  {
			      char_u	*lastbranch;
			      char_u	*lastnode = ଖାଲି;
			      char_u	*br;

			      ret = ଖାଲି;
			      ଯେତେବେଳେ ((c = getchr()) != ']')
			      {
				  ଯଦି (c == NUL)
				      EMSG2_RET_NULL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
				  br = regnode(BRANCH);
				  ଯଦି (ret == ଖାଲି)
				      ret = br;
				  ଅନ୍ୟଥା
				  {
				      regtail(lastnode, br);
				      ଯଦି (reg_toolong)
					  ଫେରନ୍ତୁ ଖାଲି;
				  }

				  ungetchr();
				  one_exactly = TRUE;
				  lastnode = regatom(flagp);
				  one_exactly = FALSE;
				  ଯଦି (lastnode == ଖାଲି)
				      ଫେରନ୍ତୁ ଖାଲି;
			      }
			      ଯଦି (ret == ଖାଲି)
				  EMSG2_RET_NULL(_(e_empty_str_brackets),
						      reg_magic == MAGIC_ALL);
			      lastbranch = regnode(BRANCH);
			      br = regnode(NOTHING);
			      ଯଦି (ret != JUST_CALC_SIZE)
			      {
				  regtail(lastnode, br);
				  regtail(lastbranch, br);
				  // connect all branches to the NOTHING
				  // branch at the end
				  ପାଇଁ (br = ret; br != lastnode; )
				  {
				      ଯଦି (OP(br) == BRANCH)
				      {
					  regtail(br, lastbranch);
					  ଯଦି (reg_toolong)
					      ଫେରନ୍ତୁ ଖାଲି;
					  br = OPERAND(br);
				      }
				      ଅନ୍ୟଥା
					  br = regnext(br);
				  }
			      }
			      *flagp &= ~(HASWIDTH | SIMPLE);
			      ଭାଙ୍ଗ;
			  }

		ସ୍ଥିତି 'd':   // %d123 decimal
		ସ୍ଥିତି 'o':   // %o123 octal
		ସ୍ଥିତି 'x':   // %xab hex 2
		ସ୍ଥିତି 'u':   // %uabcd hex 4
		ସ୍ଥିତି 'U':   // %U1234abcd hex 8
			  {
			      vimlong_T i;

			      ବାଛ (c)
			      {
				  ସ୍ଥିତି 'd': i = getdecchrs(); ଭାଙ୍ଗ;
				  ସ୍ଥିତି 'o': i = getoctchrs(); ଭାଙ୍ଗ;
				  ସ୍ଥିତି 'x': i = gethexchrs(2); ଭାଙ୍ଗ;
				  ସ୍ଥିତି 'u': i = gethexchrs(4); ଭାଙ୍ଗ;
				  ସ୍ଥିତି 'U': i = gethexchrs(8); ଭାଙ୍ଗ;
				  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  i = -1; ଭାଙ୍ଗ;
			      }

			      ଯଦି (i < 0 || i > INT_MAX)
				  EMSG2_RET_NULL(
					    _(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			      ଯଦି (use_multibytecode(i))
				  ret = regnode(MULTIBYTECODE);
			      ଅନ୍ୟଥା
				  ret = regnode(EXACTLY);
			      ଯଦି (i == 0)
				  regc(0x0a);
			      ଅନ୍ୟଥା
				  regmbc((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)i);
			      regc(NUL);
			      *flagp |= HASWIDTH;
			      ଭାଙ୍ଗ;
			  }

		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			  ଯଦି (VIM_ISDIGIT(c) || c == '<' || c == '>'
						|| c == '\'' || c == '.')
			  {
			      long_u	n = 0;
			      ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cmp;
			      ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cur = FALSE;
			      ପୂର୍ଣ୍ଣସଂଖ୍ୟା	got_digit = FALSE;

			      cmp = c;
			      ଯଦି (cmp == '<' || cmp == '>')
				  c = getchr();
			      ଯଦି (no_Magic(c) == '.')
			      {
				  cur = TRUE;
				  c = getchr();
			      }
			      ଯେତେବେଳେ (VIM_ISDIGIT(c))
			      {
				  got_digit = TRUE;
				  n = n * 10 + (c - '0');
				  c = getchr();
			      }
			      ଯଦି (no_Magic(c) == '\'' && n == 0)
			      {
				  // "\%'m", "\%<'m" and "\%>'m": Mark
				  c = getchr();
				  ret = regnode(RE_MARK);
				  ଯଦି (ret == JUST_CALC_SIZE)
				      regsize += 2;
				  ଅନ୍ୟଥା
				  {
				      *regcode++ = c;
				      *regcode++ = cmp;
				  }
				  ଭାଙ୍ଗ;
			      }
			      ଅନ୍ୟଥା ଯଦି ((c == 'l' || c == 'c' || c == 'v')
					  && (cur || got_digit))
			      {
				  ଯଦି (cur && n)
				  {
				    semsg(_(e_regexp_ସଂଖ୍ୟା_after_dot_pos_ସନ୍ଧାନ_chr),
								  no_Magic(c));
				    rc_did_emsg = TRUE;
				    ଫେରନ୍ତୁ ଖାଲି;
				  }
				  ଯଦି (c == 'l')
				  {
				      ଯଦି (cur)
					  n = curwin->w_cursor.lnum;
				      ret = regnode(RE_LNUM);
				      ଯଦି (save_prev_at_start)
					  at_start = TRUE;
				  }
				  ଅନ୍ୟଥା ଯଦି (c == 'c')
				  {
				      ଯଦି (cur)
				      {
					  n = curwin->w_cursor.col;
					  n++;
				      }
				      ret = regnode(RE_COL);
				  }
				  ଅନ୍ୟଥା
				  {
				      ଯଦି (cur)
				      {
					  colnr_T vcol = 0;

					  getvvcol(curwin, &curwin->w_cursor,
							 ଖାଲି, ଖାଲି, &vcol, 0);
					  ++vcol;
					  n = vcol;
				      }
				      ret = regnode(RE_VCOL);
				  }
				  ଯଦି (ret == JUST_CALC_SIZE)
				      regsize += 5;
				  ଅନ୍ୟଥା
				  {
				      // put the number and the optional
				      // comparator after the opcode
				      regcode = re_put_long(regcode, n);
				      *regcode++ = cmp;
				  }
				  ଭାଙ୍ଗ;
			      }
			  }

			  EMSG2_RET_NULL(_(e_invalid_character_after_str),
						      reg_magic == MAGIC_ALL);
	    }
	}
	ଭାଙ୍ଗ;

      ସ୍ଥିତି Magic('['):
collection:
	{
	    char_u	*lp;

	    // If there is no matching ']', we assume the '[' is a normal
	    // character.  This makes 'incsearch' and ":help [" work.
	    lp = skip_anyof(regparse);
	    ଯଦି (*lp == ']')	// there is a matching ']'
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	startc = -1;	// > 0 when next '-' is a range
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	endc;

		ଯଦି (extra == 0)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cl = bt_recognize_char_class(regparse, lp);

		    ଯଦି (cl != 0)
		    {
			ret = regnode(cl);
			regparse = lp;
			skipchr();
			*flagp |= HASWIDTH | SIMPLE;
			ଭାଙ୍ଗ;
		    }
		}

		// In a character class, different parsing rules apply.
		// Not even \ is special anymore, nothing is.
		ଯଦି (*regparse == '^')	    // Complement of range.
		{
		    ret = regnode(ANYBUT + extra);
		    regparse++;
		}
		ଅନ୍ୟଥା
		    ret = regnode(ANYOF + extra);

		// At the start ']' and '-' mean the literal character.
		ଯଦି (*regparse == ']' || *regparse == '-')
		{
		    startc = *regparse;
		    regc(*regparse++);
		}

		ଯେତେବେଳେ (*regparse != NUL && *regparse != ']')
		{
		    ଯଦି (*regparse == '-')
		    {
			++regparse;
			// The '-' is not used for a range at the end and
			// after or before a '\n'.
			ଯଦି (*regparse == ']' || *regparse == NUL
				|| startc == -1
				|| (regparse[0] == '\\' && regparse[1] == 'n'))
			{
			    regc('-');
			    startc = '-';	// [--x] is a range
			}
			ଅନ୍ୟଥା
			{
			    // Also accept "a-[.z.]"
			    endc = 0;
			    ଯଦି (*regparse == '[')
				endc = get_coll_element(&regparse);
			    ଯଦି (endc == 0)
			    {
				ଯଦି (has_mbyte)
				    endc = mb_ptr2char_adv(&regparse);
				ଅନ୍ୟଥା
				    endc = *regparse++;
			    }

			    // Handle \o40, \x20 and \u20AC style sequences
			    ଯଦି (endc == '\\' && !reg_cpo_lit && !reg_cpo_bsl)
				endc = coll_get_char();

			    ଯଦି (startc > endc)
				EMSG_RET_NULL(_(e_reverse_range_in_character_class));
			    ଯଦି (has_mbyte && ((*mb_char2len)(startc) > 1
						 || (*mb_char2len)(endc) > 1))
			    {
				// Limit to a range of 256 chars.
				ଯଦି (endc > startc + 256)
				    EMSG_RET_NULL(_(e_range_too_large_in_character_class));
				ଯେତେବେଳେ (++startc <= endc)
				    regmbc(startc);
			    }
			    ଅନ୍ୟଥା
			    {
				ଯେତେବେଳେ (++startc <= endc)
				    regc(startc);
			    }
			    startc = -1;
			}
		    }
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    ଅନ୍ୟଥା ଯଦି (*regparse == '\\'
			    && !reg_cpo_bsl
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != ଖାଲି
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR,
						       regparse[1]) != ଖାଲି)))
		    {
			regparse++;
			ଯଦି (*regparse == 'n')
			{
			    // '\n' in range: also match NL
			    ଯଦି (ret != JUST_CALC_SIZE)
			    {
				// Using \n inside [^] does not change what
				// matches. "[^\n]" is the same as ".".
				ଯଦି (*ret == ANYOF)
				{
				    *ret = ANYOF + ADD_NL;
				    *flagp |= HASNL;
				}
				// else: must have had a \n already
			    }
			    regparse++;
			    startc = -1;
			}
			ଅନ୍ୟଥା ଯଦି (*regparse == 'd'
				|| *regparse == 'o'
				|| *regparse == 'x'
				|| *regparse == 'u'
				|| *regparse == 'U')
			{
			    startc = coll_get_char();
			    // max UTF-8 Codepoint is U+10FFFF,
			    // but allow values until INT_MAX
			    ଯଦି (startc == INT_MAX)
				EMSG_RET_NULL(_(e_unicode_val_too_large));
			    ଯଦି (startc == 0)
				regc(0x0a);
			    ଅନ୍ୟଥା
				regmbc(startc);
			}
			ଅନ୍ୟଥା
			{
			    startc = backslash_trans(*regparse++);
			    regc(startc);
			}
		    }
		    ଅନ୍ୟଥା ଯଦି (*regparse == '[')
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା c_class;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା cu;

			c_class = get_char_class(&regparse);
			startc = -1;
			// Characters assumed to be 8 bits!
			ବାଛ (c_class)
			{
			    ସ୍ଥିତି CLASS_NONE:
				c_class = get_equi_class(&regparse);
				ଯଦି (c_class != 0)
				{
				    // produce equivalence class
				    reg_equi_class(c_class);
				}
				ଅନ୍ୟଥା ଯଦି ((c_class =
					    get_coll_element(&regparse)) != 0)
				{
				    // produce a collating element
				    regmbc(c_class);
				}
				ଅନ୍ୟଥା
				{
				    // literal '[', allow [[-x] as a range
				    startc = *regparse++;
				    regc(startc);
				}
				ଭାଙ୍ଗ;
			    ସ୍ଥିତି CLASS_ALNUM:
				ପାଇଁ (cu = 1; cu < 128; cu++)
				    ଯଦି (isalnum(cu))
					regmbc(cu);
				ଭାଙ୍ଗ;
			    ସ୍ଥିତି CLASS_ALPHA:
				ପାଇଁ (cu = 1; cu < 128; cu++)
				    ଯଦି (isalpha(cu))
					regmbc(cu);
				ଭାଙ୍ଗ;
			    ସ୍ଥିତି CLASS_BLANK:
				regc(' ');
				regc('\t');
				ଭାଙ୍ଗ;
			    ସ୍ଥିତି CLASS_CNTRL:
				ପାଇଁ (cu = 1; cu <= 127; cu++)
				    ଯଦି (iscntrl(cu))
					regmbc(cu);
				ଭାଙ୍ଗ;
			    ସ୍ଥିତି CLASS_DIGIT:
				ପାଇଁ (cu = 1; cu <= 127; cu++)
				    ଯଦି (VIM_ISDIGIT(cu))
					regmbc(cu);
				ଭାଙ୍ଗ;
			    ସ୍ଥିତି CLASS_GRAPH:
				ପାଇଁ (cu = 1; cu <= 127; cu++)
				    ଯଦି (isgraph(cu))
					regmbc(cu);
				ଭାଙ୍ଗ;
			    ସ୍ଥିତି CLASS_LOWER:
				ପାଇଁ (cu = 1; cu <= 255; cu++)
				    ଯଦି (MB_ISLOWER(cu) && cu != 170
								 && cu != 186)
					regmbc(cu);
				ଭାଙ୍ଗ;
			    ସ୍ଥିତି CLASS_PRINT:
				ପାଇଁ (cu = 1; cu <= 255; cu++)
				    ଯଦି (vim_isprintc(cu))
					regmbc(cu);
				ଭାଙ୍ଗ;
			    ସ୍ଥିତି CLASS_PUNCT:
				ପାଇଁ (cu = 1; cu < 128; cu++)
				    ଯଦି (ispunct(cu))
					regmbc(cu);
				ଭାଙ୍ଗ;
			    ସ୍ଥିତି CLASS_SPACE:
				ପାଇଁ (cu = 9; cu <= 13; cu++)
				    regc(cu);
				regc(' ');
				ଭାଙ୍ଗ;
			    ସ୍ଥିତି CLASS_UPPER:
				ପାଇଁ (cu = 1; cu <= 255; cu++)
				    ଯଦି (MB_ISUPPER(cu))
					regmbc(cu);
				ଭାଙ୍ଗ;
			    ସ୍ଥିତି CLASS_XDIGIT:
				ପାଇଁ (cu = 1; cu <= 255; cu++)
				    ଯଦି (vim_isxdigit(cu))
					regmbc(cu);
				ଭାଙ୍ଗ;
			    ସ୍ଥିତି CLASS_TAB:
				regc('\t');
				ଭାଙ୍ଗ;
			    ସ୍ଥିତି CLASS_RETURN:
				regc('\r');
				ଭାଙ୍ଗ;
			    ସ୍ଥିତି CLASS_BACKSPACE:
				regc('\b');
				ଭାଙ୍ଗ;
			    ସ୍ଥିତି CLASS_ESCAPE:
				regc('\033');
				ଭାଙ୍ଗ;
			    ସ୍ଥିତି CLASS_IDENT:
				ପାଇଁ (cu = 1; cu <= 255; cu++)
				    ଯଦି (vim_isIDc(cu))
					regmbc(cu);
				ଭାଙ୍ଗ;
			    ସ୍ଥିତି CLASS_KEYWORD:
				ପାଇଁ (cu = 1; cu <= 255; cu++)
				    ଯଦି (reg_iswordc(cu))
					regmbc(cu);
				ଭାଙ୍ଗ;
			    ସ୍ଥିତି CLASS_FNAME:
				ପାଇଁ (cu = 1; cu <= 255; cu++)
				    ଯଦି (vim_isfilec(cu))
					regmbc(cu);
				ଭାଙ୍ଗ;
			}
		    }
		    ଅନ୍ୟଥା
		    {
			ଯଦି (has_mbyte)
			{
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len;

			    // produce a multibyte character, including any
			    // following composing characters
			    startc = mb_ptr2char(regparse);
			    len = (*mb_ptr2len)(regparse);
			    ଯଦି (enc_utf8 && utf_char2len(startc) != len)
				startc = -1;	// composing chars
			    ଯେତେବେଳେ (--len >= 0)
				regc(*regparse++);
			}
			ଅନ୍ୟଥା
			{
			    startc = *regparse++;
			    regc(startc);
			}
		    }
		}
		regc(NUL);
		prevchr_len = 1;	// last char was the ']'
		ଯଦି (*regparse != ']')
		    EMSG_RET_NULL(_(e_too_many_brackets));  // Cannot happen?
		skipchr();	    // let's be friends with the lexer again
		*flagp |= HASWIDTH | SIMPLE;
		ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା ଯଦି (reg_strict)
		EMSG2_RET_NULL(_(e_missing_rsb_after_str_lsb),
							reg_magic > MAGIC_OFF);
	}
	// FALLTHROUGH

      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

	    // A multi-byte character is handled as a separate atom if it's
	    // before a multi and when it's a composing char.
	    ଯଦି (use_multibytecode(c))
	    {
do_multibyte:
		ret = regnode(MULTIBYTECODE);
		regmbc(c);
		*flagp |= HASWIDTH | SIMPLE;
		ଭାଙ୍ଗ;
	    }

	    ret = regnode(EXACTLY);

	    // Append characters as long as:
	    // - there is no following multi, we then need the character in
	    //   front of it as a single character operand
	    // - not running into a Magic character
	    // - "one_exactly" is not set
	    // But always emit at least one character.  Might be a Multi,
	    // e.g., a "[" without matching "]".
	    ପାଇଁ (len = 0; c != NUL && (len == 0
			|| (re_multi_type(peekchr()) == NOT_MULTI
			    && !one_exactly
			    && !is_Magic(c))); ++len)
	    {
		c = no_Magic(c);
		ଯଦି (has_mbyte)
		{
		    regmbc(c);
		    ଯଦି (enc_utf8)
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା	l;

			// Need to get composing character too.
			ପାଇଁ (;;)
			{
			    l = utf_ptr2len(regparse);
			    ଯଦି (!UTF_COMPOSINGLIKE(regparse, regparse + l))
				ଭାଙ୍ଗ;
			    regmbc(utf_ptr2char(regparse));
			    skipchr();
			}
		    }
		}
		ଅନ୍ୟଥା
		    regc(c);
		c = getchr();
	    }
	    ungetchr();

	    regc(NUL);
	    *flagp |= HASWIDTH;
	    ଯଦି (len == 1)
		*flagp |= SIMPLE;
	}
	ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ ret;
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
    ସ୍ଥିର char_u *
regpiece(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *flagp)
{
    char_u	    *ret;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    op;
    char_u	    *next;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    flags;
    long	    minval;
    long	    maxval;

    ret = regatom(&flags);
    ଯଦି (ret == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    op = peekchr();
    ଯଦି (re_multi_type(op) == NOT_MULTI)
    {
	*flagp = flags;
	ଫେରନ୍ତୁ ret;
    }
    // default flags
    *flagp = (WORST | SPSTART | (flags & (HASNL | HASLOOKBH)));

    skipchr();
    ବାଛ (op)
    {
	ସ୍ଥିତି Magic('*'):
	    ଯଦି (flags & SIMPLE)
		reginsert(STAR, ret);
	    ଅନ୍ୟଥା
	    {
		// Emit x* as (x&|), where & means "self".
		reginsert(BRANCH, ret); // Either x
		regoptail(ret, regnode(BACK));	// and loop
		regoptail(ret, ret);	// back
		regtail(ret, regnode(BRANCH));	// or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି Magic('+'):
	    ଯଦି (flags & SIMPLE)
		reginsert(PLUS, ret);
	    ଅନ୍ୟଥା
	    {
		// Emit x+ as x(&|), where & means "self".
		next = regnode(BRANCH); // Either
		regtail(ret, next);
		regtail(regnode(BACK), ret);	// loop back
		regtail(next, regnode(BRANCH)); // or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    *flagp = (WORST | HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି Magic('@'):
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	lop = END;
		long	nr;

		nr = (long)getdecchrs();
		ବାଛ (no_Magic(getchr()))
		{
		    ସ୍ଥିତି '=': lop = MATCH; ଭାଙ୍ଗ;		  // \@=
		    ସ୍ଥିତି '!': lop = NOMATCH; ଭାଙ୍ଗ;		  // \@!
		    ସ୍ଥିତି '>': lop = SUBPAT; ଭାଙ୍ଗ;		  // \@>
		    ସ୍ଥିତି '<': ବାଛ (no_Magic(getchr()))
			      {
				  ସ୍ଥିତି '=': lop = BEHIND; ଭାଙ୍ଗ;   // \@<=
				  ସ୍ଥିତି '!': lop = NOBEHIND; ଭାଙ୍ଗ; // \@<!
			      }
		}
		ଯଦି (lop == END)
		    EMSG2_RET_NULL(_(e_invalid_character_after_str_at),
						      reg_magic == MAGIC_ALL);
		// Look behind must match with behind_pos.
		ଯଦି (lop == BEHIND || lop == NOBEHIND)
		{
		    regtail(ret, regnode(BHPOS));
		    *flagp |= HASLOOKBH;
		}
		regtail(ret, regnode(END)); // operand ends
		ଯଦି (lop == BEHIND || lop == NOBEHIND)
		{
		    ଯଦି (nr < 0)
			nr = 0; // no limit is same as zero limit
		    reginsert_nr(lop, nr, ret);
		}
		ଅନ୍ୟଥା
		    reginsert(lop, ret);
		ଭାଙ୍ଗ;
	    }

	ସ୍ଥିତି Magic('?'):
	ସ୍ଥିତି Magic('='):
	    // Emit x= as (x|)
	    reginsert(BRANCH, ret);		// Either x
	    regtail(ret, regnode(BRANCH));	// or
	    next = regnode(NOTHING);		// null.
	    regtail(ret, next);
	    regoptail(ret, next);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି Magic('{'):
	    ଯଦି (!read_limits(&minval, &maxval))
		ଫେରନ୍ତୁ ଖାଲି;
	    ଯଦି (flags & SIMPLE)
	    {
		reginsert(BRACE_SIMPLE, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (ଅଙ୍କ_complex_braces >= 10)
		    EMSG2_RET_NULL(_(e_too_many_complex_str_curly),
						      reg_magic == MAGIC_ALL);
		reginsert(BRACE_COMPLEX + ଅଙ୍କ_complex_braces, ret);
		regoptail(ret, regnode(BACK));
		regoptail(ret, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
		++ଅଙ୍କ_complex_braces;
	    }
	    ଯଦି (minval > 0 && maxval > 0)
		*flagp = (HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    ଭାଙ୍ଗ;
    }
    ଯଦି (re_multi_type(peekchr()) != NOT_MULTI)
    {
	// Can't have a multi follow a multi.
	ଯଦି (peekchr() == Magic('*'))
	    EMSG2_RET_NULL(_(e_nested_str), reg_magic >= MAGIC_ON);
	EMSG3_RET_NULL(_(e_nested_str_chr), reg_magic == MAGIC_ALL,
							  no_Magic(peekchr()));
    }

    ଫେରନ୍ତୁ ret;
}

/*
 * Parse one alternative of an | or & operator.
 * Implements the concatenation operator.
 */
    ସ୍ଥିର char_u *
regconcat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *flagp)
{
    char_u	*first = ଖାଲି;
    char_u	*chain = ଖାଲି;
    char_u	*latest;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cont = TRUE;

    *flagp = WORST;		// Tentatively.

    ଯେତେବେଳେ (cont)
    {
	ବାଛ (peekchr())
	{
	    ସ୍ଥିତି NUL:
	    ସ୍ଥିତି Magic('|'):
	    ସ୍ଥିତି Magic('&'):
	    ସ୍ଥିତି Magic(')'):
			    cont = FALSE;
			    ଭାଙ୍ଗ;
	    ସ୍ଥିତି Magic('Z'):
			    regflags |= RF_ICOMBINE;
			    skipchr_keepstart();
			    ଭାଙ୍ଗ;
	    ସ୍ଥିତି Magic('c'):
			    regflags |= RF_ICASE;
			    skipchr_keepstart();
			    ଭାଙ୍ଗ;
	    ସ୍ଥିତି Magic('C'):
			    regflags |= RF_NOICASE;
			    skipchr_keepstart();
			    ଭାଙ୍ଗ;
	    ସ୍ଥିତି Magic('v'):
			    reg_magic = MAGIC_ALL;
			    skipchr_keepstart();
			    curchr = -1;
			    ଭାଙ୍ଗ;
	    ସ୍ଥିତି Magic('m'):
			    reg_magic = MAGIC_ON;
			    skipchr_keepstart();
			    curchr = -1;
			    ଭାଙ୍ଗ;
	    ସ୍ଥିତି Magic('M'):
			    reg_magic = MAGIC_OFF;
			    skipchr_keepstart();
			    curchr = -1;
			    ଭାଙ୍ଗ;
	    ସ୍ଥିତି Magic('V'):
			    reg_magic = MAGIC_NONE;
			    skipchr_keepstart();
			    curchr = -1;
			    ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			    latest = regpiece(&flags);
			    ଯଦି (latest == ଖାଲି || reg_toolong)
				ଫେରନ୍ତୁ ଖାଲି;
			    *flagp |= flags & (HASWIDTH | HASNL | HASLOOKBH);
			    ଯଦି (chain == ଖାଲି)	// First piece.
				*flagp |= flags & SPSTART;
			    ଅନ୍ୟଥା
				regtail(chain, latest);
			    chain = latest;
			    ଯଦି (first == ଖାଲି)
				first = latest;
			    ଭାଙ୍ଗ;
	}
    }
    ଯଦି (first == ଖାଲି)		// Loop ran zero times.
	first = regnode(NOTHING);
    ଫେରନ୍ତୁ first;
}

/*
 * Parse one alternative of an | operator.
 * Implements the & operator.
 */
    ସ୍ଥିର char_u *
regbranch(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *flagp)
{
    char_u	*ret;
    char_u	*chain = ଖାଲି;
    char_u	*latest;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags;

    *flagp = WORST | HASNL;		// Tentatively.

    ret = regnode(BRANCH);
    ପାଇଁ (;;)
    {
	latest = regconcat(&flags);
	ଯଦି (latest == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	// If one of the branches has width, the whole thing has.  If one of
	// the branches anchors at start-of-line, the whole thing does.
	// If one of the branches uses look-behind, the whole thing does.
	*flagp |= flags & (HASWIDTH | SPSTART | HASLOOKBH);
	// If one of the branches doesn't match a line-break, the whole thing
	// doesn't.
	*flagp &= ~HASNL | (flags & HASNL);
	ଯଦି (chain != ଖାଲି)
	    regtail(chain, latest);
	ଯଦି (peekchr() != Magic('&'))
	    ଭାଙ୍ଗ;
	skipchr();
	regtail(latest, regnode(END)); // operand ends
	ଯଦି (reg_toolong)
	    ଭାଙ୍ଗ;
	reginsert(MATCH, latest);
	chain = latest;
    }

    ଫେରନ୍ତୁ ret;
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
    ସ୍ଥିର char_u *
reg(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		paren,	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*flagp)
{
    char_u	*ret;
    char_u	*br;
    char_u	*ender;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		parno = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags;

    *flagp = HASWIDTH;		// Tentatively.

#ifdef FEAT_SYN_HL
    ଯଦି (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	ଯଦି (regnzpar >= NSUBEXP)
	    EMSG_RET_NULL(_(e_too_many_z));
	parno = regnzpar;
	regnzpar++;
	ret = regnode(ZOPEN + parno);
    }
    ଅନ୍ୟଥା
#endif
	ଯଦି (paren == REG_PAREN)
    {
	// Make a MOPEN node.
	ଯଦି (regnpar >= NSUBEXP)
	    EMSG2_RET_NULL(_(e_too_many_str_open), reg_magic == MAGIC_ALL);
	parno = regnpar;
	++regnpar;
	ret = regnode(MOPEN + parno);
    }
    ଅନ୍ୟଥା ଯଦି (paren == REG_NPAREN)
    {
	// Make a NOPEN node.
	ret = regnode(NOPEN);
    }
    ଅନ୍ୟଥା
	ret = ଖାଲି;

    ଯଦି (bt_reg_parse_depth >= REG_MAX_PAREN_DEPTH)
	EMSG_RET_NULL(_(e_command_too_complex));
    ++bt_reg_parse_depth;

    // Pick up the branches, linking them together.
    br = regbranch(&flags);
    ଯଦି (br == ଖାଲି)
    {
	ret = ଖାଲି;
	ଯାଅ theend;
    }
    ଯଦି (ret != ଖାଲି)
	regtail(ret, br);	// [MZ]OPEN -> first.
    ଅନ୍ୟଥା
	ret = br;
    // If one of the branches can be zero-width, the whole thing can.
    // If one of the branches has * at start or matches a line-break, the
    // whole thing can.
    ଯଦି (!(flags & HASWIDTH))
	*flagp &= ~HASWIDTH;
    *flagp |= flags & (SPSTART | HASNL | HASLOOKBH);
    ଯେତେବେଳେ (peekchr() == Magic('|'))
    {
	skipchr();
	br = regbranch(&flags);
	ଯଦି (br == ଖାଲି || reg_toolong)
	{
	    ret = ଖାଲି;
	    ଯାଅ theend;
	}
	regtail(ret, br);	// BRANCH -> BRANCH.
	ଯଦି (!(flags & HASWIDTH))
	    *flagp &= ~HASWIDTH;
	*flagp |= flags & (SPSTART | HASNL | HASLOOKBH);
    }

    // Make a closing node, and hook it on the end.
    ender = regnode(
#ifdef FEAT_SYN_HL
	    paren == REG_ZPAREN ? ZCLOSE + parno :
#endif
	    paren == REG_PAREN ? MCLOSE + parno :
	    paren == REG_NPAREN ? NCLOSE : END);
    regtail(ret, ender);

    // Hook the tails of the branches to the closing node.
    ପାଇଁ (br = ret; br != ଖାଲି; br = regnext(br))
	regoptail(br, ender);

    // Check for proper termination.
    ଯଦି (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
#ifdef FEAT_SYN_HL
	ଯଦି (paren == REG_ZPAREN)
	{
	    emsg(_(e_unmatched_z));
	    rc_did_emsg = TRUE;
	    ret = ଖାଲି;
	    ଯାଅ theend;
	}
	ଅନ୍ୟଥା
#endif
	ଯଦି (paren == REG_NPAREN)
	{
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = ଖାଲି;
	    ଯାଅ theend;
	}
	ଅନ୍ୟଥା
	{
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = ଖାଲି;
	    ଯାଅ theend;
	}
    }
    ଅନ୍ୟଥା ଯଦି (paren == REG_NOPAREN && peekchr() != NUL)
    {
	ଯଦି (curchr == Magic(')'))
	{
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = ଖାଲି;
	    ଯାଅ theend;
	}
	ଅନ୍ୟଥା
	{
	    emsg(_(e_trailing_characters));	// "Can't happen".
	    rc_did_emsg = TRUE;
	    ret = ଖାଲି;
	    ଯାଅ theend;
	}
    }
    // Here we set the flag allowing back references to this set of
    // parentheses.
    ଯଦି (paren == REG_PAREN)
	had_endbrace[parno] = TRUE;	// have seen the close paren

theend:
    --bt_reg_parse_depth;
    ଫେରନ୍ତୁ ret;
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
    ସ୍ଥିର regprog_T *
bt_regcomp(char_u *ପଦସମଷ୍ଟି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା re_flags)
{
    bt_regprog_T    *r;
    char_u	*scan;
    char_u	*longest;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags;

    ଯଦି (ପଦସମଷ୍ଟି == ଖାଲି)
	IEMSG_RET_NULL(e_null_argument);

    init_class_tab();

    // First pass: determine size, legality.
    regcomp_start(ପଦସମଷ୍ଟି, re_flags);
    regcode = JUST_CALC_SIZE;
    regc(REGMAGIC);
    ଯଦି (reg(REG_NOPAREN, &flags) == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // Allocate space.
    r = alloc(offsetof(bt_regprog_T, program) + regsize);
    ଯଦି (r == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    r->re_in_use = FALSE;
#ifdef DEBUG
    r->regsz = regsize;
#endif

    // Second pass: emit code.
    regcomp_start(ପଦସମଷ୍ଟି, re_flags);
    regcode = r->program;
    regc(REGMAGIC);
    ଯଦି (reg(REG_NOPAREN, &flags) == ଖାଲି || reg_toolong)
    {
	vim_free(r);
	ଯଦି (reg_toolong)
	    EMSG_RET_NULL(_(e_pattern_too_long));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // Dig out information for optimizations.
    r->regstart = NUL;		// Worst-case defaults.
    r->reganch = 0;
    r->regmust = ଖାଲି;
    r->regmlen = 0;
    r->regflags = regflags;
    ଯଦି (flags & HASNL)
	r->regflags |= RF_HASNL;
    ଯଦି (flags & HASLOOKBH)
	r->regflags |= RF_LOOKBH;
#ifdef FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    r->reghasz = re_has_z;
#endif
    scan = r->program + 1;	// First BRANCH.
    ଯଦି (OP(regnext(scan)) == END)   // Only one top-level choice.
    {
	scan = OPERAND(scan);

	// Starting-point info.
	ଯଦି (OP(scan) == BOL || OP(scan) == RE_BOF)
	{
	    r->reganch++;
	    scan = regnext(scan);
	}

	ଯଦି (OP(scan) == EXACTLY)
	{
	    ଯଦି (has_mbyte)
		r->regstart = (*mb_ptr2char)(OPERAND(scan));
	    ଅନ୍ୟଥା
		r->regstart = *OPERAND(scan);
	}
	ଅନ୍ୟଥା ଯଦି ((OP(scan) == BOW
		    || OP(scan) == EOW
		    || OP(scan) == NOTHING
		    || OP(scan) == MOPEN + 0 || OP(scan) == NOPEN
		    || OP(scan) == MCLOSE + 0 || OP(scan) == NCLOSE)
		 && OP(regnext(scan)) == EXACTLY)
	{
	    ଯଦି (has_mbyte)
		r->regstart = (*mb_ptr2char)(OPERAND(regnext(scan)));
	    ଅନ୍ୟଥା
		r->regstart = *OPERAND(regnext(scan));
	}

	// If there's something expensive in the r.e., find the longest
	// literal string that must appear and make it the regmust.  Resolve
	// ties in favor of later strings, since the regstart check works
	// with the beginning of the r.e. and avoiding duplication
	// strengthens checking.  Not a strong reason, but sufficient in the
	// absence of others.

	// When the r.e. starts with BOW, it is faster to look for a regmust
	// first. Used a lot for "#" and "*" commands. (Added by mool).
	ଯଦି ((flags & SPSTART || OP(scan) == BOW || OP(scan) == EOW)
							  && !(flags & HASNL))
	{
	    size_t  scanlen;

	    longest = ଖାଲି;
	    len = 0;
	    ପାଇଁ (; scan != ଖାଲି; scan = regnext(scan))
	    {
		ଯଦି (OP(scan) == EXACTLY)
		{
		    scanlen = STRLEN(OPERAND(scan));
		    ଯଦି (scanlen >= (size_t)len)
		    {
			longest = OPERAND(scan);
			len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)scanlen;
		    }
		}
	    }
	    r->regmust = longest;
	    r->regmlen = len;
	}
    }
#ifdef BT_REGEXP_DUMP
    regdump(ପଦସମଷ୍ଟି, r);
#endif
    r->engine = &bt_regengine;
    ଫେରନ୍ତୁ (regprog_T *)r;
}

#ଯଦି defined(FEAT_SYN_HL)
/*
 * Check if during the previous call to vim_regcomp the EOL item "$" has been
 * found.  This is messy, but it works fine.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_regcomp_had_eol(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ had_eol;
}
#endif

/*
 * Get a number after a backslash that is inside [].
 * When nothing is recognized return a backslash.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
coll_get_char(ଶୂନ୍ୟ)
{
    vimlong_T	nr = -1;

    ବାଛ (*regparse++)
    {
	ସ୍ଥିତି 'd': nr = getdecchrs(); ଭାଙ୍ଗ;
	ସ୍ଥିତି 'o': nr = getoctchrs(); ଭାଙ୍ଗ;
	ସ୍ଥିତି 'x': nr = gethexchrs(2); ଭାଙ୍ଗ;
	ସ୍ଥିତି 'u': nr = gethexchrs(4); ଭାଙ୍ଗ;
	ସ୍ଥିତି 'U': nr = gethexchrs(8); ଭାଙ୍ଗ;
    }
    ଯଦି (nr < 0)
    {
	// If getting the number fails be backwards compatible: the character
	// is a backslash.
	--regparse;
	nr = '\\';
    }
    ଯଦି (nr > INT_MAX)
	nr = INT_MAX;
    ଫେରନ୍ତୁ nr;
}

/*
 * Free a compiled regexp program, returned by bt_regcomp().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
bt_regfree(regprog_T *prog)
{
    vim_free(prog);
}

#define ADVANCE_REGINPUT() MB_PTR_ADV(rex.ନିବେଶ)

/*
 * The arguments from BRACE_LIMITS are stored here.  They are actually local
 * to regmatch(), but they are here to reduce the amount of stack space used
 * (it can be called recursively many times).
 */
ସ୍ଥିର long	bl_minval;
ସ୍ଥିର long	bl_maxval;

/*
 * Save the input line and position in a regsave_T.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
reg_save(regsave_T *save, garray_T *gap)
{
    ଯଦି (REG_MULTI)
    {
	save->rs_u.pos.col = (colnr_T)(rex.ନିବେଶ - rex.ଧାଡ଼ି);
	save->rs_u.pos.lnum = rex.lnum;
    }
    ଅନ୍ୟଥା
	save->rs_u.ptr = rex.ନିବେଶ;
    save->rs_len = gap->ga_len;
}

/*
 * Restore the input line and position from a regsave_T.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
reg_restore(regsave_T *save, garray_T *gap)
{
    ଯଦି (REG_MULTI)
    {
	ଯଦି (rex.lnum != save->rs_u.pos.lnum)
	{
	    // only call reg_getline() when the line number changed to save
	    // a bit of time
	    rex.lnum = save->rs_u.pos.lnum;
	    rex.ଧାଡ଼ି = reg_getline(rex.lnum);
	}
	rex.ନିବେଶ = rex.ଧାଡ଼ି + save->rs_u.pos.col;
    }
    ଅନ୍ୟଥା
	rex.ନିବେଶ = save->rs_u.ptr;
    gap->ga_len = save->rs_len;
}

/*
 * Return TRUE if current position is equal to saved position.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
reg_save_equal(regsave_T *save)
{
    ଯଦି (REG_MULTI)
	ଫେରନ୍ତୁ rex.lnum == save->rs_u.pos.lnum
				  && rex.ନିବେଶ == rex.ଧାଡ଼ି + save->rs_u.pos.col;
    ଫେରନ୍ତୁ rex.ନିବେଶ == save->rs_u.ptr;
}

// Save the sub-expressions before attempting a match.
#define save_se(savep, posp, pp) \
    REG_MULTI ? save_se_multi((savep), (posp)) : save_se_one((savep), (pp))

// After a failed match restore the sub-expressions.
#define restore_se(savep, posp, pp) \
{ \
    ଯଦି (REG_MULTI) \
	*(posp) = (savep)->se_u.pos; \
    ଅନ୍ୟଥା \
	*(pp) = (savep)->se_u.ptr; \
}

/*
 * Tentatively set the sub-expression start to the current position (after
 * calling regmatch() they will have changed).  Need to save the existing
 * values for when there is no match.
 * Use se_save() to use pointer (save_se_multi()) or position (save_se_one()),
 * depending on REG_MULTI.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
save_se_multi(save_se_T *savep, lpos_T *posp)
{
    savep->se_u.pos = *posp;
    posp->lnum = rex.lnum;
    posp->col = (colnr_T)(rex.ନିବେଶ - rex.ଧାଡ଼ି);
}

    ସ୍ଥିର ଶୂନ୍ୟ
save_se_one(save_se_T *savep, char_u **pp)
{
    savep->se_u.ptr = *pp;
    *pp = rex.ନିବେଶ;
}

/*
 * regrepeat - repeatedly match something simple, return how many.
 * Advances rex.input (and rex.lnum) to just after the matched chars.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
regrepeat(
    char_u	*p,
    long	maxcount)   // maximum number of matches allowed
{
    long	ଗଣନା_2 = 0;
    char_u	*scan;
    char_u	*opnd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mask;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		testval = 0;

    scan = rex.ନିବେଶ;	    // Make local copy of rex.input for speed.
    opnd = OPERAND(p);
    ବାଛ (OP(p))
    {
      ସ୍ଥିତି ANY:
      ସ୍ଥିତି ANY + ADD_NL:
	ଯେତେବେଳେ (ଗଣନା_2 < maxcount)
	{
	    // Matching anything means we continue until end-of-line (or
	    // end-of-file for ANY + ADD_NL), only limited by maxcount.
	    ଯେତେବେଳେ (*scan != NUL && ଗଣନା_2 < maxcount)
	    {
		++ଗଣନା_2;
		MB_PTR_ADV(scan);
	    }
	    ଯଦି (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
				      || rex.reg_ଧାଡ଼ି_lbr || ଗଣନା_2 == maxcount)
		ଭାଙ୍ଗ;
	    ++ଗଣନା_2;		// count the line-break
	    reg_nextline();
	    scan = rex.ନିବେଶ;
	    ଯଦି (got_int)
		ଭାଙ୍ଗ;
	}
	ଭାଙ୍ଗ;

      ସ୍ଥିତି IDENT:
      ସ୍ଥିତି IDENT + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      ସ୍ଥିତି SIDENT:
      ସ୍ଥିତି SIDENT + ADD_NL:
	ଯେତେବେଳେ (ଗଣନା_2 < maxcount)
	{
	    ଯଦି (vim_isIDc(PTR2CHAR(scan)) && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    ଅନ୍ୟଥା ଯଦି (*scan == NUL)
	    {
		ଯଦି (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ଧାଡ଼ି_lbr)
		    ଭାଙ୍ଗ;
		reg_nextline();
		scan = rex.ନିବେଶ;
		ଯଦି (got_int)
		    ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା ଯଦି (rex.reg_ଧାଡ଼ି_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    ଅନ୍ୟଥା
		ଭାଙ୍ଗ;
	    ++ଗଣନା_2;
	}
	ଭାଙ୍ଗ;

      ସ୍ଥିତି KWORD:
      ସ୍ଥିତି KWORD + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      ସ୍ଥିତି SKWORD:
      ସ୍ଥିତି SKWORD + ADD_NL:
	ଯେତେବେଳେ (ଗଣନା_2 < maxcount)
	{
	    ଯଦି (vim_iswordp_buf(scan, rex.reg_buf)
					  && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    ଅନ୍ୟଥା ଯଦି (*scan == NUL)
	    {
		ଯଦି (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ଧାଡ଼ି_lbr)
		    ଭାଙ୍ଗ;
		reg_nextline();
		scan = rex.ନିବେଶ;
		ଯଦି (got_int)
		    ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା ଯଦି (rex.reg_ଧାଡ଼ି_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    ଅନ୍ୟଥା
		ଭାଙ୍ଗ;
	    ++ଗଣନା_2;
	}
	ଭାଙ୍ଗ;

      ସ୍ଥିତି FNAME:
      ସ୍ଥିତି FNAME + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      ସ୍ଥିତି SFNAME:
      ସ୍ଥିତି SFNAME + ADD_NL:
	ଯେତେବେଳେ (ଗଣନା_2 < maxcount)
	{
	    ଯଦି (vim_isfilec(PTR2CHAR(scan)) && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    ଅନ୍ୟଥା ଯଦି (*scan == NUL)
	    {
		ଯଦି (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ଧାଡ଼ି_lbr)
		    ଭାଙ୍ଗ;
		reg_nextline();
		scan = rex.ନିବେଶ;
		ଯଦି (got_int)
		    ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା ଯଦି (rex.reg_ଧାଡ଼ି_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    ଅନ୍ୟଥା
		ଭାଙ୍ଗ;
	    ++ଗଣନା_2;
	}
	ଭାଙ୍ଗ;

      ସ୍ଥିତି PRINT:
      ସ୍ଥିତି PRINT + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      ସ୍ଥିତି SPRINT:
      ସ୍ଥିତି SPRINT + ADD_NL:
	ଯେତେବେଳେ (ଗଣନା_2 < maxcount)
	{
	    ଯଦି (*scan == NUL)
	    {
		ଯଦି (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ଧାଡ଼ି_lbr)
		    ଭାଙ୍ଗ;
		reg_nextline();
		scan = rex.ନିବେଶ;
		ଯଦି (got_int)
		    ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା ଯଦି (vim_isprintc(PTR2CHAR(scan)) == 1
					  && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    ଅନ୍ୟଥା ଯଦି (rex.reg_ଧାଡ଼ି_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    ଅନ୍ୟଥା
		ଭାଙ୍ଗ;
	    ++ଗଣନା_2;
	}
	ଭାଙ୍ଗ;

      ସ୍ଥିତି WHITE:
      ସ୍ଥିତି WHITE + ADD_NL:
	testval = mask = RI_WHITE;
do_class:
	ଯେତେବେଳେ (ଗଣନା_2 < maxcount)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;

	    ଯଦି (*scan == NUL)
	    {
		ଯଦି (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ଧାଡ଼ି_lbr)
		    ଭାଙ୍ଗ;
		reg_nextline();
		scan = rex.ନିବେଶ;
		ଯଦି (got_int)
		    ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା ଯଦି (has_mbyte && (l = (*mb_ptr2len)(scan)) > 1)
	    {
		ଯଦି (testval != 0)
		    ଭାଙ୍ଗ;
		scan += l;
	    }
	    ଅନ୍ୟଥା ଯଦି ((class_tab[*scan] & mask) == testval)
		++scan;
	    ଅନ୍ୟଥା ଯଦି (rex.reg_ଧାଡ଼ି_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    ଅନ୍ୟଥା
		ଭାଙ୍ଗ;
	    ++ଗଣନା_2;
	}
	ଭାଙ୍ଗ;

      ସ୍ଥିତି NWHITE:
      ସ୍ଥିତି NWHITE + ADD_NL:
	mask = RI_WHITE;
	ଯାଅ do_class;
      ସ୍ଥିତି DIGIT:
      ସ୍ଥିତି DIGIT + ADD_NL:
	testval = mask = RI_DIGIT;
	ଯାଅ do_class;
      ସ୍ଥିତି NDIGIT:
      ସ୍ଥିତି NDIGIT + ADD_NL:
	mask = RI_DIGIT;
	ଯାଅ do_class;
      ସ୍ଥିତି HEX:
      ସ୍ଥିତି HEX + ADD_NL:
	testval = mask = RI_HEX;
	ଯାଅ do_class;
      ସ୍ଥିତି NHEX:
      ସ୍ଥିତି NHEX + ADD_NL:
	mask = RI_HEX;
	ଯାଅ do_class;
      ସ୍ଥିତି OCTAL:
      ସ୍ଥିତି OCTAL + ADD_NL:
	testval = mask = RI_OCTAL;
	ଯାଅ do_class;
      ସ୍ଥିତି NOCTAL:
      ସ୍ଥିତି NOCTAL + ADD_NL:
	mask = RI_OCTAL;
	ଯାଅ do_class;
      ସ୍ଥିତି WORD:
      ସ୍ଥିତି WORD + ADD_NL:
	testval = mask = RI_WORD;
	ଯାଅ do_class;
      ସ୍ଥିତି NWORD:
      ସ୍ଥିତି NWORD + ADD_NL:
	mask = RI_WORD;
	ଯାଅ do_class;
      ସ୍ଥିତି HEAD:
      ସ୍ଥିତି HEAD + ADD_NL:
	testval = mask = RI_HEAD;
	ଯାଅ do_class;
      ସ୍ଥିତି NHEAD:
      ସ୍ଥିତି NHEAD + ADD_NL:
	mask = RI_HEAD;
	ଯାଅ do_class;
      ସ୍ଥିତି ALPHA:
      ସ୍ଥିତି ALPHA + ADD_NL:
	testval = mask = RI_ALPHA;
	ଯାଅ do_class;
      ସ୍ଥିତି NALPHA:
      ସ୍ଥିତି NALPHA + ADD_NL:
	mask = RI_ALPHA;
	ଯାଅ do_class;
      ସ୍ଥିତି LOWER:
      ସ୍ଥିତି LOWER + ADD_NL:
	testval = mask = RI_LOWER;
	ଯାଅ do_class;
      ସ୍ଥିତି NLOWER:
      ସ୍ଥିତି NLOWER + ADD_NL:
	mask = RI_LOWER;
	ଯାଅ do_class;
      ସ୍ଥିତି UPPER:
      ସ୍ଥିତି UPPER + ADD_NL:
	testval = mask = RI_UPPER;
	ଯାଅ do_class;
      ସ୍ଥିତି NUPPER:
      ସ୍ଥିତି NUPPER + ADD_NL:
	mask = RI_UPPER;
	ଯାଅ do_class;

      ସ୍ଥିତି EXACTLY:
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cu, cl;

	    // This doesn't do a multi-byte character, because a MULTIBYTECODE
	    // would have been used for it.  It does handle single-byte
	    // characters, such as latin1.
	    ଯଦି (rex.reg_ic)
	    {
		cu = MB_TOUPPER(*opnd);
		cl = MB_TOLOWER(*opnd);
		ଯେତେବେଳେ (ଗଣନା_2 < maxcount && (*scan == cu || *scan == cl))
		{
		    ଗଣନା_2++;
		    scan++;
		}
	    }
	    ଅନ୍ୟଥା
	    {
		cu = *opnd;
		ଯେତେବେଳେ (ଗଣନା_2 < maxcount && *scan == cu)
		{
		    ଗଣନା_2++;
		    scan++;
		}
	    }
	    ଭାଙ୍ଗ;
	}

      ସ୍ଥିତି MULTIBYTECODE:
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, len, cf = 0;

	    // Safety check (just in case 'encoding' was changed since
	    // compiling the program).
	    ଯଦି ((len = (*mb_ptr2len)(opnd)) > 1)
	    {
		ଯଦି (rex.reg_ic && enc_utf8)
		    cf = utf_fold(utf_ptr2char(opnd));
		ଯେତେବେଳେ (ଗଣନା_2 < maxcount && (*mb_ptr2len)(scan) >= len)
		{
		    ପାଇଁ (i = 0; i < len; ++i)
			ଯଦି (opnd[i] != scan[i])
			    ଭାଙ୍ଗ;
		    ଯଦି (i < len && (!rex.reg_ic || !enc_utf8
					|| utf_fold(utf_ptr2char(scan)) != cf))
			ଭାଙ୍ଗ;
		    scan += len;
		    ++ଗଣନା_2;
		}
	    }
	}
	ଭାଙ୍ଗ;

      ସ୍ଥିତି ANYOF:
      ସ୍ଥିତି ANYOF + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH

      ସ୍ଥିତି ANYBUT:
      ସ୍ଥିତି ANYBUT + ADD_NL:
	ଯେତେବେଳେ (ଗଣନା_2 < maxcount)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;

	    ଯଦି (*scan == NUL)
	    {
		ଯଦି (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ଧାଡ଼ି_lbr)
		    ଭାଙ୍ଗ;
		reg_nextline();
		scan = rex.ନିବେଶ;
		ଯଦି (got_int)
		    ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା ଯଦି (rex.reg_ଧାଡ଼ି_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    ଅନ୍ୟଥା ଯଦି (has_mbyte && (len = (*mb_ptr2len)(scan)) > 1)
	    {
		ଯଦି ((cstrchr(opnd, (*mb_ptr2char)(scan)) == ଖାଲି) == testval)
		    ଭାଙ୍ଗ;
		scan += len;
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି ((cstrchr(opnd, *scan) == ଖାଲି) == testval)
		    ଭାଙ୍ଗ;
		++scan;
	    }
	    ++ଗଣନା_2;
	}
	ଭାଙ୍ଗ;

      ସ୍ଥିତି NEWL:
	ଯେତେବେଳେ (ଗଣନା_2 < maxcount
		&& ((*scan == NUL && rex.lnum <= rex.reg_maxline
				       && !rex.reg_ଧାଡ଼ି_lbr && REG_MULTI)
		    || (*scan == '\n' && rex.reg_ଧାଡ଼ି_lbr)))
	{
	    ଗଣନା_2++;
	    ଯଦି (rex.reg_ଧାଡ଼ି_lbr)
		ADVANCE_REGINPUT();
	    ଅନ୍ୟଥା
		reg_nextline();
	    scan = rex.ନିବେଶ;
	    ଯଦି (got_int)
		ଭାଙ୍ଗ;
	}
	ଭାଙ୍ଗ;

      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:			// Oh dear.  Called inappropriately.
	iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	printf("Called regrepeat with op code %d\n", OP(p));
#endif
	ଭାଙ୍ଗ;
    }

    rex.ନିବେଶ = scan;

    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଗଣନା_2;
}

/*
 * Push an item onto the regstack.
 * Returns pointer to new item.  Returns NULL when out of memory.
 */
    ସ୍ଥିର regitem_T *
regstack_ଠେଲ(regstate_T state, char_u *scan)
{
    regitem_T	*rp;

    ଯଦି ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
    {
	emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (GA_GROW_FAILS(&regstack, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଆକାର(regitem_T)))
	ଫେରନ୍ତୁ ଖାଲି;

    rp = (regitem_T *)((ଅକ୍ଷର *)regstack.ga_data + regstack.ga_len);
    rp->rs_state = state;
    rp->rs_scan = scan;

    regstack.ga_len += ଆକାର(regitem_T);
    ଫେରନ୍ତୁ rp;
}

/*
 * Pop an item from the regstack.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
regstack_କାଢ଼(char_u **scan)
{
    regitem_T	*rp;

    rp = (regitem_T *)((ଅକ୍ଷର *)regstack.ga_data + regstack.ga_len) - 1;
    *scan = rp->rs_scan;

    regstack.ga_len -= ଆକାର(regitem_T);
}

#ifdef FEAT_RELTIME
/*
 * Check if the timer expired, return TRUE if so.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bt_did_time_out(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *timed_out)
{
    ଯଦି (*timeout_flag)
    {
	ଯଦି (timed_out != ଖାଲି)
	{
# ifdef FEAT_EVAL
	    ଯଦି (!*timed_out)
		ch_log(ଖାଲି, "BT regexp timed out");
# endif
	    *timed_out = TRUE;
	}
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}
#endif

/*
 * Save the current subexpr to "bp", so that they can be restored
 * later by restore_subexpr().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
save_subexpr(regbehind_T *bp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    // When "rex.need_clear_subexpr" is set we don't need to save the values,
    // only remember that this flag needs to be set again when restoring.
    bp->save_need_clear_subexpr = rex.need_clear_subexpr;
    ଯଦି (rex.need_clear_subexpr)
	ଫେରନ୍ତୁ;

    ପାଇଁ (i = 0; i < NSUBEXP; ++i)
    {
	ଯଦି (REG_MULTI)
	{
	    bp->save_start[i].se_u.pos = rex.reg_startpos[i];
	    bp->save_end[i].se_u.pos = rex.reg_endpos[i];
	}
	ଅନ୍ୟଥା
	{
	    bp->save_start[i].se_u.ptr = rex.reg_startp[i];
	    bp->save_end[i].se_u.ptr = rex.reg_endp[i];
	}
    }
}

/*
 * Restore the subexpr from "bp".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
restore_subexpr(regbehind_T *bp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    // Only need to restore saved values when they are not to be cleared.
    rex.need_clear_subexpr = bp->save_need_clear_subexpr;
    ଯଦି (rex.need_clear_subexpr)
	ଫେରନ୍ତୁ;

    ପାଇଁ (i = 0; i < NSUBEXP; ++i)
    {
	ଯଦି (REG_MULTI)
	{
	    rex.reg_startpos[i] = bp->save_start[i].se_u.pos;
	    rex.reg_endpos[i] = bp->save_end[i].se_u.pos;
	}
	ଅନ୍ୟଥା
	{
	    rex.reg_startp[i] = bp->save_start[i].se_u.ptr;
	    rex.reg_endp[i] = bp->save_end[i].se_u.ptr;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
regmatch(
    char_u	*scan,		    // Current node.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*timed_out UNUSED)  // flag set on timeout or NULL
{
  char_u	*next;		// Next node.
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା		op;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
  regitem_T	*rp;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା		no;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status;		// one of the RA_ values:

  // Make "regstack" and "backpos" empty.  They are allocated and freed in
  // bt_regexec_both() to reduce malloc()/free() calls.
  regstack.ga_len = 0;
  backpos.ga_len = 0;

  // Repeat until "regstack" is empty.
  ପାଇଁ (;;)
  {
    // Some patterns may take a long time to match, e.g., "\([a-z]\+\)\+Q".
    // Allow interrupting them with CTRL-C.
    fast_breakcheck();

#ifdef DEBUG
    ଯଦି (scan != ଖାଲି && regnarrate)
    {
	mch_errmsg((ଅକ୍ଷର *)regprop(scan));
	mch_errmsg("(\n");
    }
#endif

    // Repeat for items that can be matched sequentially, without using the
    // regstack.
    ପାଇଁ (;;)
    {
	ଯଦି (got_int || scan == ଖାଲି)
	{
	    status = RA_FAIL;
	    ଭାଙ୍ଗ;
	}
#ifdef FEAT_RELTIME
	ଯଦି (bt_did_time_out(timed_out))
	{
	    status = RA_FAIL;
	    ଭାଙ୍ଗ;
	}
#endif
	status = RA_CONT;

#ifdef DEBUG
	ଯଦି (regnarrate)
	{
	    mch_errmsg((ଅକ୍ଷର *)regprop(scan));
	    mch_errmsg("...\n");
# ifdef FEAT_SYN_HL
	    ଯଦି (re_extmatch_in != ଖାଲି)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

		mch_errmsg(_("External submatches:\n"));
		ପାଇଁ (i = 0; i < NSUBEXP; i++)
		{
		    mch_errmsg("    \"");
		    ଯଦି (re_extmatch_in->matches[i] != ଖାଲି)
			mch_errmsg((ଅକ୍ଷର *)re_extmatch_in->matches[i]);
		    mch_errmsg("\"\n");
		}
	    }
# endif
	}
#endif
	next = regnext(scan);

	op = OP(scan);
	// Check for character class with NL added.
	ଯଦି (!rex.reg_ଧାଡ଼ି_lbr && WITH_NL(op) && REG_MULTI
			     && *rex.ନିବେଶ == NUL && rex.lnum <= rex.reg_maxline)
	{
	    reg_nextline();
	}
	ଅନ୍ୟଥା ଯଦି (rex.reg_ଧାଡ଼ି_lbr && WITH_NL(op) && *rex.ନିବେଶ == '\n')
	{
	    ADVANCE_REGINPUT();
	}
	ଅନ୍ୟଥା
	{
	  ଯଦି (WITH_NL(op))
	      op -= ADD_NL;
	  ଯଦି (has_mbyte)
	      c = (*mb_ptr2char)(rex.ନିବେଶ);
	  ଅନ୍ୟଥା
	      c = *rex.ନିବେଶ;
	  ବାଛ (op)
	  {
	  ସ୍ଥିତି BOL:
	    ଯଦି (rex.ନିବେଶ != rex.ଧାଡ଼ି)
		status = RA_NOMATCH;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି EOL:
	    ଯଦି (c != NUL)
		status = RA_NOMATCH;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି RE_BOF:
	    // We're not at the beginning of the file when below the first
	    // line where we started, not at the start of the line or we
	    // didn't start at the first line of the buffer.
	    ଯଦି (rex.lnum != 0 || rex.ନିବେଶ != rex.ଧାଡ଼ି
				       || (REG_MULTI && rex.reg_firstlnum > 1))
		status = RA_NOMATCH;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି RE_EOF:
	    ଯଦି (rex.lnum != rex.reg_maxline || c != NUL)
		status = RA_NOMATCH;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି CURSOR:
	    // Check if the buffer is in a window and compare the
	    // rex.reg_win->w_cursor position to the match position.
	    ଯଦି (rex.reg_win == ଖାଲି
		    || (rex.lnum + rex.reg_firstlnum
						 != rex.reg_win->w_cursor.lnum)
		    || ((colnr_T)(rex.ନିବେଶ - rex.ଧାଡ଼ି)
						 != rex.reg_win->w_cursor.col))
		status = RA_NOMATCH;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି RE_MARK:
	    // Compare the mark position to the match position.
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	mark = OPERAND(scan)[0];
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cmp = OPERAND(scan)[1];
		pos_T	*pos;
		size_t	col = REG_MULTI ? rex.ନିବେଶ - rex.ଧାଡ଼ି : 0;

		pos = getmark_buf(rex.reg_buf, mark, FALSE);

		// Line may have been freed, get it again.
		ଯଦି (REG_MULTI)
		{
		    rex.ଧାଡ଼ି = reg_getline(rex.lnum);
		    rex.ନିବେଶ = rex.ଧାଡ଼ି + col;
		}

		ଯଦି (pos == ଖାଲି		     // mark doesn't exist
			|| pos->lnum <= 0)   // mark isn't set in reg_buf
		{
		    status = RA_NOMATCH;
		}
		ଅନ୍ୟଥା
		{
		    colnr_T pos_col = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->col == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->col;

		    ଯଦି ((pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_col == (colnr_T)(rex.ନିବେଶ - rex.ଧାଡ଼ି)
				    ? (cmp == '<' || cmp == '>')
				    : (pos_col < (colnr_T)(rex.ନିବେଶ - rex.ଧାଡ଼ି)
					? cmp != '>'
					: cmp != '<'))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? cmp != '>'
				    : cmp != '<')))
		    status = RA_NOMATCH;
		}
	    }
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି RE_VISUAL:
	    ଯଦି (!reg_match_visual())
		status = RA_NOMATCH;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି RE_LNUM:
	    ଯଦି (!REG_MULTI || !re_ଅଙ୍କ_cmp((long_u)(rex.lnum + rex.reg_firstlnum),
									scan))
		status = RA_NOMATCH;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି RE_COL:
	    ଯଦି (!re_ଅଙ୍କ_cmp((long_u)(rex.ନିବେଶ - rex.ଧାଡ଼ି) + 1, scan))
		status = RA_NOMATCH;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି RE_VCOL:
	    {
		win_T	    *wp = rex.reg_win == ଖାଲି ? curwin : rex.reg_win;
		linenr_T    lnum = REG_MULTI ? rex.reg_firstlnum + rex.lnum : 1;
		long_u	    vcol;

		ଯଦି (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା))
		    lnum = 1;
		vcol = (long_u)win_linetabsize(wp, lnum, rex.ଧାଡ଼ି,
					      (colnr_T)(rex.ନିବେଶ - rex.ଧାଡ଼ି));
		ଯଦି (!re_ଅଙ୍କ_cmp(vcol + 1, scan))
		    status = RA_NOMATCH;
	    }
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି BOW:	// \<word; rex.input points to w
	    ଯଦି (c == NUL)	// Can't match at end of line
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା ଯଦି (has_mbyte)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା this_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.ନିବେଶ, rex.reg_buf);
		ଯଦି (this_class <= 1)
		    status = RA_NOMATCH;  // not on a word at all
		ଅନ୍ୟଥା ଯଦି (reg_prev_class() == this_class)
		    status = RA_NOMATCH;  // previous char is in same word
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (!vim_iswordc_buf(c, rex.reg_buf) || (rex.ନିବେଶ > rex.ଧାଡ଼ି
				&& vim_iswordc_buf(rex.ନିବେଶ[-1], rex.reg_buf)))
		    status = RA_NOMATCH;
	    }
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି EOW:	// word\>; rex.input points after d
	    ଯଦି (rex.ନିବେଶ == rex.ଧାଡ଼ି)    // Can't match at start of line
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା ଯଦି (has_mbyte)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା this_class, prev_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.ନିବେଶ, rex.reg_buf);
		prev_class = reg_prev_class();
		ଯଦି (this_class == prev_class
			|| prev_class == 0 || prev_class == 1)
		    status = RA_NOMATCH;
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (!vim_iswordc_buf(rex.ନିବେଶ[-1], rex.reg_buf)
			|| (rex.ନିବେଶ[0] != NUL
					   && vim_iswordc_buf(c, rex.reg_buf)))
		    status = RA_NOMATCH;
	    }
	    ଭାଙ୍ଗ; // Matched with EOW

	  ସ୍ଥିତି ANY:
	    // ANY does not match new lines.
	    ଯଦି (c == NUL)
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
		ADVANCE_REGINPUT();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି IDENT:
	    ଯଦି (!vim_isIDc(c))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
		ADVANCE_REGINPUT();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି SIDENT:
	    ଯଦି (VIM_ISDIGIT(*rex.ନିବେଶ) || !vim_isIDc(c))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
		ADVANCE_REGINPUT();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି KWORD:
	    ଯଦି (!vim_iswordp_buf(rex.ନିବେଶ, rex.reg_buf))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
		ADVANCE_REGINPUT();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି SKWORD:
	    ଯଦି (VIM_ISDIGIT(*rex.ନିବେଶ)
				    || !vim_iswordp_buf(rex.ନିବେଶ, rex.reg_buf))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
		ADVANCE_REGINPUT();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି FNAME:
	    ଯଦି (!vim_isfilec(c))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
		ADVANCE_REGINPUT();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି SFNAME:
	    ଯଦି (VIM_ISDIGIT(*rex.ନିବେଶ) || !vim_isfilec(c))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
		ADVANCE_REGINPUT();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି PRINT:
	    ଯଦି (!vim_isprintc(PTR2CHAR(rex.ନିବେଶ)))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
		ADVANCE_REGINPUT();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି SPRINT:
	    ଯଦି (VIM_ISDIGIT(*rex.ନିବେଶ) || !vim_isprintc(PTR2CHAR(rex.ନିବେଶ)))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
		ADVANCE_REGINPUT();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି WHITE:
	    ଯଦି (!VIM_ISWHITE(c))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
		ADVANCE_REGINPUT();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି NWHITE:
	    ଯଦି (c == NUL || VIM_ISWHITE(c))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
		ADVANCE_REGINPUT();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି DIGIT:
	    ଯଦି (!ri_digit(c))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
		ADVANCE_REGINPUT();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି NDIGIT:
	    ଯଦି (c == NUL || ri_digit(c))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
		ADVANCE_REGINPUT();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି HEX:
	    ଯଦି (!ri_hex(c))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
		ADVANCE_REGINPUT();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି NHEX:
	    ଯଦି (c == NUL || ri_hex(c))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
		ADVANCE_REGINPUT();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି OCTAL:
	    ଯଦି (!ri_octal(c))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
		ADVANCE_REGINPUT();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି NOCTAL:
	    ଯଦି (c == NUL || ri_octal(c))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
		ADVANCE_REGINPUT();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି WORD:
	    ଯଦି (!ri_word(c))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
		ADVANCE_REGINPUT();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି NWORD:
	    ଯଦି (c == NUL || ri_word(c))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
		ADVANCE_REGINPUT();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି HEAD:
	    ଯଦି (!ri_head(c))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
		ADVANCE_REGINPUT();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି NHEAD:
	    ଯଦି (c == NUL || ri_head(c))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
		ADVANCE_REGINPUT();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି ALPHA:
	    ଯଦି (!ri_alpha(c))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
		ADVANCE_REGINPUT();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି NALPHA:
	    ଯଦି (c == NUL || ri_alpha(c))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
		ADVANCE_REGINPUT();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି LOWER:
	    ଯଦି (!ri_lower(c))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
		ADVANCE_REGINPUT();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି NLOWER:
	    ଯଦି (c == NUL || ri_lower(c))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
		ADVANCE_REGINPUT();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି UPPER:
	    ଯଦି (!ri_upper(c))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
		ADVANCE_REGINPUT();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି NUPPER:
	    ଯଦି (c == NUL || ri_upper(c))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
		ADVANCE_REGINPUT();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି EXACTLY:
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len;
		char_u	*opnd;

		opnd = OPERAND(scan);
		// Inline the first byte, for speed.
		ଯଦି (*opnd != *rex.ନିବେଶ
			&& (!rex.reg_ic
			    || (!enc_utf8
			      && MB_TOLOWER(*opnd) != MB_TOLOWER(*rex.ନିବେଶ))))
		    status = RA_NOMATCH;
		ଅନ୍ୟଥା ଯଦି (*opnd == NUL)
		{
		    // match empty string always works; happens when "~" is
		    // empty.
		}
		ଅନ୍ୟଥା
		{
		    ଯଦି (opnd[1] == NUL && !(enc_utf8 && rex.reg_ic))
		    {
			len = 1;	// matched a single byte above
		    }
		    ଅନ୍ୟଥା
		    {
			// Need to match first byte again for multi-byte.
			len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(opnd);
			ଯଦି (cstrncmp(opnd, rex.ନିବେଶ, &len) != 0)
			    status = RA_NOMATCH;
		    }
		    // Check for following composing character, unless %C
		    // follows (skips over all composing chars).
		    ଯଦି (status != RA_NOMATCH
			    && enc_utf8
			    && UTF_COMPOSINGLIKE(rex.ନିବେଶ, rex.ନିବେଶ + len)
			    && !rex.reg_icombine
			    && OP(next) != RE_COMPOSING)
		    {
			// raaron: This code makes a composing character get
			// ignored, which is the correct behavior (sometimes)
			// for voweled Hebrew texts.
			status = RA_NOMATCH;
		    }
		    ଯଦି (status != RA_NOMATCH)
			rex.ନିବେଶ += len;
		}
	    }
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି ANYOF:
	  ସ୍ଥିତି ANYBUT:
	    {
		char_u  *q = OPERAND(scan);

		ଯଦି (c == NUL)
		    status = RA_NOMATCH;
		ଅନ୍ୟଥା ଯଦି ((cstrchr(q, c) == ଖାଲି) == (op == ANYOF))
		    status = RA_NOMATCH;
		ଅନ୍ୟଥା
		{
		    // Check following combining characters
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len = 0;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

		    ଯଦି (enc_utf8)
			len = utfc_ptr2len(q) - utf_ptr2len(q);

		    MB_CPTR_ADV(rex.ନିବେଶ);
		    MB_CPTR_ADV(q);

		    ଯଦି (!enc_utf8 || len == 0)
			ଭାଙ୍ଗ;

		    ପାଇଁ (i = 0; i < len; ++i)
			ଯଦି (q[i] != rex.ନିବେଶ[i])
			{
			    status = RA_NOMATCH;
			    ଭାଙ୍ଗ;
			}
		    rex.ନିବେଶ += len;
		}
		ଭାଙ୍ଗ;
	    }

	  ସ୍ଥିତି MULTIBYTECODE:
	    ଯଦି (has_mbyte)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i, len;
		char_u	*opnd;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	opndc = 0, inpc;

		opnd = OPERAND(scan);
		// Safety check (just in case 'encoding' was changed since
		// compiling the program).
		ଯଦି ((len = (*mb_ptr2len)(opnd)) < 2)
		{
		    status = RA_NOMATCH;
		    ଭାଙ୍ଗ;
		}
		ଯଦି (enc_utf8)
		    opndc = utf_ptr2char(opnd);
		ଯଦି (enc_utf8 && utf_iscomposing(opndc))
		{
		    // When only a composing char is given match at any
		    // position where that composing char appears.
		    status = RA_NOMATCH;
		    ପାଇଁ (i = 0; rex.ନିବେଶ[i] != NUL;
						i += utf_ptr2len(rex.ନିବେଶ + i))
		    {
			inpc = utf_ptr2char(rex.ନିବେଶ + i);
			ଯଦି (!utf_iscomposing(inpc))
			{
			    ଯଦି (i > 0)
				ଭାଙ୍ଗ;
			}
			ଅନ୍ୟଥା ଯଦି (opndc == inpc)
			{
			    // Include all following composing chars.
			    len = i + utfc_ptr2len(rex.ନିବେଶ + i);
			    status = RA_MATCH;
			    ଭାଙ୍ଗ;
			}
		    }
		}
		ଅନ୍ୟଥା ଯଦି (enc_utf8)
		{
		    ଯଦି (cstrncmp(opnd, rex.ନିବେଶ, &len) != 0)
		    {
			status = RA_NOMATCH;
			ଭାଙ୍ଗ;
		    }
		}
		ଅନ୍ୟଥା
		    ପାଇଁ (i = 0; i < len; ++i)
			ଯଦି (opnd[i] != rex.ନିବେଶ[i])
			{
			    status = RA_NOMATCH;
			    ଭାଙ୍ଗ;
			}
		rex.ନିବେଶ += len;
	    }
	    ଅନ୍ୟଥା
		status = RA_NOMATCH;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି RE_COMPOSING:
	    ଯଦି (enc_utf8)
	    {
		// Skip composing characters.
		ଯେତେବେଳେ (utf_iscomposing(utf_ptr2char(rex.ନିବେଶ)))
		    MB_CPTR_ADV(rex.ନିବେଶ);
	    }
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି NOTHING:
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି BACK:
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
		backpos_T	*bp;

		// When we run into BACK we need to check if we don't keep
		// looping without matching any input.  The second and later
		// times a BACK is encountered it fails if the input is still
		// at the same position as the previous time.
		// The positions are stored in "backpos" and found by the
		// current value of "scan", the position in the RE program.
		bp = (backpos_T *)backpos.ga_data;
		ପାଇଁ (i = 0; i < backpos.ga_len; ++i)
		    ଯଦି (bp[i].bp_scan == scan)
			ଭାଙ୍ଗ;
		ଯଦି (i == backpos.ga_len)
		{
		    // First time at this BACK, make room to store the pos.
		    ଯଦି (GA_GROW_FAILS(&backpos, 1))
			status = RA_FAIL;
		    ଅନ୍ୟଥା
		    {
			// get "ga_data" again, it may have changed
			bp = (backpos_T *)backpos.ga_data;
			bp[i].bp_scan = scan;
			++backpos.ga_len;
		    }
		}
		ଅନ୍ୟଥା ଯଦି (reg_save_equal(&bp[i].bp_pos))
		    // Still at same position as last time, fail.
		    status = RA_NOMATCH;

		ଯଦି (status != RA_FAIL && status != RA_NOMATCH)
		    reg_save(&bp[i].bp_pos, &backpos);
	    }
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି MOPEN + 0:   // Match start: \zs
	  ସ୍ଥିତି MOPEN + 1:   // \(
	  ସ୍ଥିତି MOPEN + 2:
	  ସ୍ଥିତି MOPEN + 3:
	  ସ୍ଥିତି MOPEN + 4:
	  ସ୍ଥିତି MOPEN + 5:
	  ସ୍ଥିତି MOPEN + 6:
	  ସ୍ଥିତି MOPEN + 7:
	  ସ୍ଥିତି MOPEN + 8:
	  ସ୍ଥିତି MOPEN + 9:
	    {
		no = op - MOPEN;
		cleanup_subexpr();
		rp = regstack_ଠେଲ(RS_MOPEN, scan);
		ଯଦି (rp == ଖାଲି)
		    status = RA_FAIL;
		ଅନ୍ୟଥା
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_startpos[no],
							  &rex.reg_startp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି NOPEN:	    // \%(
	  ସ୍ଥିତି NCLOSE:	    // \) after \%(
		ଯଦି (regstack_ଠେଲ(RS_NOPEN, scan) == ଖାଲି)
		    status = RA_FAIL;
		// We simply continue and handle the result when done.
		ଭାଙ୍ଗ;

#ifdef FEAT_SYN_HL
	  ସ୍ଥିତି ZOPEN + 1:
	  ସ୍ଥିତି ZOPEN + 2:
	  ସ୍ଥିତି ZOPEN + 3:
	  ସ୍ଥିତି ZOPEN + 4:
	  ସ୍ଥିତି ZOPEN + 5:
	  ସ୍ଥିତି ZOPEN + 6:
	  ସ୍ଥିତି ZOPEN + 7:
	  ସ୍ଥିତି ZOPEN + 8:
	  ସ୍ଥିତି ZOPEN + 9:
	    {
		no = op - ZOPEN;
		cleanup_zsubexpr();
		rp = regstack_ଠେଲ(RS_ZOPEN, scan);
		ଯଦି (rp == ଖାଲି)
		    status = RA_FAIL;
		ଅନ୍ୟଥା
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_startzpos[no],
							     &reg_startzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    ଭାଙ୍ଗ;
#endif

	  ସ୍ଥିତି MCLOSE + 0:  // Match end: \ze
	  ସ୍ଥିତି MCLOSE + 1:  // \)
	  ସ୍ଥିତି MCLOSE + 2:
	  ସ୍ଥିତି MCLOSE + 3:
	  ସ୍ଥିତି MCLOSE + 4:
	  ସ୍ଥିତି MCLOSE + 5:
	  ସ୍ଥିତି MCLOSE + 6:
	  ସ୍ଥିତି MCLOSE + 7:
	  ସ୍ଥିତି MCLOSE + 8:
	  ସ୍ଥିତି MCLOSE + 9:
	    {
		no = op - MCLOSE;
		cleanup_subexpr();
		rp = regstack_ଠେଲ(RS_MCLOSE, scan);
		ଯଦି (rp == ଖାଲି)
		    status = RA_FAIL;
		ଅନ୍ୟଥା
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_endpos[no],
							    &rex.reg_endp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    ଭାଙ୍ଗ;

#ifdef FEAT_SYN_HL
	  ସ୍ଥିତି ZCLOSE + 1:  // \) after \z(
	  ସ୍ଥିତି ZCLOSE + 2:
	  ସ୍ଥିତି ZCLOSE + 3:
	  ସ୍ଥିତି ZCLOSE + 4:
	  ସ୍ଥିତି ZCLOSE + 5:
	  ସ୍ଥିତି ZCLOSE + 6:
	  ସ୍ଥିତି ZCLOSE + 7:
	  ସ୍ଥିତି ZCLOSE + 8:
	  ସ୍ଥିତି ZCLOSE + 9:
	    {
		no = op - ZCLOSE;
		cleanup_zsubexpr();
		rp = regstack_ଠେଲ(RS_ZCLOSE, scan);
		ଯଦି (rp == ଖାଲି)
		    status = RA_FAIL;
		ଅନ୍ୟଥା
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_endzpos[no],
							      &reg_endzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    ଭାଙ୍ଗ;
#endif

	  ସ୍ଥିତି BACKREF + 1:
	  ସ୍ଥିତି BACKREF + 2:
	  ସ୍ଥିତି BACKREF + 3:
	  ସ୍ଥିତି BACKREF + 4:
	  ସ୍ଥିତି BACKREF + 5:
	  ସ୍ଥିତି BACKREF + 6:
	  ସ୍ଥିତି BACKREF + 7:
	  ସ୍ଥିତି BACKREF + 8:
	  ସ୍ଥିତି BACKREF + 9:
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

		no = op - BACKREF;
		cleanup_subexpr();
		ଯଦି (!REG_MULTI)		// Single-line regexp
		{
		    ଯଦି (rex.reg_startp[no] == ଖାଲି || rex.reg_endp[no] == ଖାଲି)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    ଅନ୍ୟଥା
		    {
			// Compare current input with back-ref in the same
			// line.
			len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(rex.reg_endp[no] - rex.reg_startp[no]);
			ଯଦି (cstrncmp(rex.reg_startp[no], rex.ନିବେଶ, &len) != 0)
			    status = RA_NOMATCH;
		    }
		}
		ଅନ୍ୟଥା				// Multi-line regexp
		{
		    ଯଦି (rex.reg_startpos[no].lnum < 0
						|| rex.reg_endpos[no].lnum < 0)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    ଅନ୍ୟଥା
		    {
			ଯଦି (rex.reg_startpos[no].lnum == rex.lnum
				&& rex.reg_endpos[no].lnum == rex.lnum)
			{
			    // Compare back-ref within the current line.
			    len = rex.reg_endpos[no].col
						    - rex.reg_startpos[no].col;
			    ଯଦି (cstrncmp(rex.ଧାଡ଼ି + rex.reg_startpos[no].col,
							  rex.ନିବେଶ, &len) != 0)
				status = RA_NOMATCH;
			}
			ଅନ୍ୟଥା
			{
			    // Messy situation: Need to compare between two
			    // lines.
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = match_with_backref(
					    rex.reg_startpos[no].lnum,
					    rex.reg_startpos[no].col,
					    rex.reg_endpos[no].lnum,
					    rex.reg_endpos[no].col,
					    &len);

			    ଯଦି (r != RA_MATCH)
				status = r;
			}
		    }
		}

		// Matched the backref, skip over it.
		rex.ନିବେଶ += len;
	    }
	    ଭାଙ୍ଗ;

#ifdef FEAT_SYN_HL
	  ସ୍ଥିତି ZREF + 1:
	  ସ୍ଥିତି ZREF + 2:
	  ସ୍ଥିତି ZREF + 3:
	  ସ୍ଥିତି ZREF + 4:
	  ସ୍ଥିତି ZREF + 5:
	  ସ୍ଥିତି ZREF + 6:
	  ସ୍ଥିତି ZREF + 7:
	  ସ୍ଥିତି ZREF + 8:
	  ସ୍ଥିତି ZREF + 9:
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len;

		cleanup_zsubexpr();
		no = op - ZREF;
		ଯଦି (re_extmatch_in != ଖାଲି
			&& re_extmatch_in->matches[no] != ଖାଲି)
		{
		    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(re_extmatch_in->matches[no]);
		    ଯଦି (cstrncmp(re_extmatch_in->matches[no],
							  rex.ନିବେଶ, &len) != 0)
			status = RA_NOMATCH;
		    ଅନ୍ୟଥା
			rex.ନିବେଶ += len;
		}
		ଅନ୍ୟଥା
		{
		    // Backref was not set: Match an empty string.
		}
	    }
	    ଭାଙ୍ଗ;
#endif

	  ସ୍ଥିତି BRANCH:
	    {
		ଯଦି (OP(next) != BRANCH) // No choice.
		    next = OPERAND(scan);	// Avoid recursion.
		ଅନ୍ୟଥା
		{
		    rp = regstack_ଠେଲ(RS_BRANCH, scan);
		    ଯଦି (rp == ଖାଲି)
			status = RA_FAIL;
		    ଅନ୍ୟଥା
			status = RA_BREAK;	// rest is below
		}
	    }
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି BRACE_LIMITS:
	    {
		ଯଦି (OP(next) == BRACE_SIMPLE)
		{
		    bl_minval = OPERAND_MIN(scan);
		    bl_maxval = OPERAND_MAX(scan);
		}
		ଅନ୍ୟଥା ଯଦି (OP(next) >= BRACE_COMPLEX
			&& OP(next) < BRACE_COMPLEX + 10)
		{
		    no = OP(next) - BRACE_COMPLEX;
		    brace_min[no] = OPERAND_MIN(scan);
		    brace_max[no] = OPERAND_MAX(scan);
		    brace_ଗଣନା[no] = 0;
		}
		ଅନ୍ୟଥା
		{
		    internal_error("BRACE_LIMITS");
		    status = RA_FAIL;
		}
	    }
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି BRACE_COMPLEX + 0:
	  ସ୍ଥିତି BRACE_COMPLEX + 1:
	  ସ୍ଥିତି BRACE_COMPLEX + 2:
	  ସ୍ଥିତି BRACE_COMPLEX + 3:
	  ସ୍ଥିତି BRACE_COMPLEX + 4:
	  ସ୍ଥିତି BRACE_COMPLEX + 5:
	  ସ୍ଥିତି BRACE_COMPLEX + 6:
	  ସ୍ଥିତି BRACE_COMPLEX + 7:
	  ସ୍ଥିତି BRACE_COMPLEX + 8:
	  ସ୍ଥିତି BRACE_COMPLEX + 9:
	    {
		no = op - BRACE_COMPLEX;
		++brace_ଗଣନା[no];

		// If not matched enough times yet, try one more
		ଯଦି (brace_ଗଣନା[no] <= (brace_min[no] <= brace_max[no]
					     ? brace_min[no] : brace_max[no]))
		{
		    rp = regstack_ଠେଲ(RS_BRCPLX_MORE, scan);
		    ଯଦି (rp == ଖାଲି)
			status = RA_FAIL;
		    ଅନ୍ୟଥା
		    {
			rp->rs_no = no;
			reg_save(&rp->rs_un.regsave, &backpos);
			next = OPERAND(scan);
			// We continue and handle the result when done.
		    }
		    ଭାଙ୍ଗ;
		}

		// If matched enough times, may try matching some more
		ଯଦି (brace_min[no] <= brace_max[no])
		{
		    // Range is the normal way around, use longest match
		    ଯଦି (brace_ଗଣନା[no] <= brace_max[no])
		    {
			rp = regstack_ଠେଲ(RS_BRCPLX_LONG, scan);
			ଯଦି (rp == ଖାଲି)
			    status = RA_FAIL;
			ଅନ୍ୟଥା
			{
			    rp->rs_no = no;
			    reg_save(&rp->rs_un.regsave, &backpos);
			    next = OPERAND(scan);
			    // We continue and handle the result when done.
			}
		    }
		}
		ଅନ୍ୟଥା
		{
		    // Range is backwards, use shortest match first
		    ଯଦି (brace_ଗଣନା[no] <= brace_min[no])
		    {
			rp = regstack_ଠେଲ(RS_BRCPLX_SHORT, scan);
			ଯଦି (rp == ଖାଲି)
			    status = RA_FAIL;
			ଅନ୍ୟଥା
			{
			    reg_save(&rp->rs_un.regsave, &backpos);
			    // We continue and handle the result when done.
			}
		    }
		}
	    }
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି BRACE_SIMPLE:
	  ସ୍ଥିତି STAR:
	  ସ୍ଥିତି PLUS:
	    {
		regstar_T	rst;

		// Lookahead to avoid useless match attempts when we know
		// what character comes next.
		ଯଦି (OP(next) == EXACTLY)
		{
		    rst.nextb = *OPERAND(next);
		    ଯଦି (rex.reg_ic)
		    {
			ଯଦି (MB_ISUPPER(rst.nextb))
			    rst.nextb_ic = MB_TOLOWER(rst.nextb);
			ଅନ୍ୟଥା
			    rst.nextb_ic = MB_TOUPPER(rst.nextb);
		    }
		    ଅନ୍ୟଥା
			rst.nextb_ic = rst.nextb;
		}
		ଅନ୍ୟଥା
		{
		    rst.nextb = NUL;
		    rst.nextb_ic = NUL;
		}
		ଯଦି (op != BRACE_SIMPLE)
		{
		    rst.minval = (op == STAR) ? 0 : 1;
		    rst.maxval = MAX_LIMIT;
		}
		ଅନ୍ୟଥା
		{
		    rst.minval = bl_minval;
		    rst.maxval = bl_maxval;
		}

		// When maxval > minval, try matching as much as possible, up
		// to maxval.  When maxval < minval, try matching at least the
		// minimal number (since the range is backwards, that's also
		// maxval!).
		rst.ଗଣନା_2 = regrepeat(OPERAND(scan), rst.maxval);
		ଯଦି (got_int)
		{
		    status = RA_FAIL;
		    ଭାଙ୍ଗ;
		}
		ଯଦି (rst.minval <= rst.maxval
			  ? rst.ଗଣନା_2 >= rst.minval : rst.ଗଣନା_2 >= rst.maxval)
		{
		    // It could match.  Prepare for trying to match what
		    // follows.  The code is below.  Parameters are stored in
		    // a regstar_T on the regstack.
		    ଯଦି ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
		    {
			emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
			status = RA_FAIL;
		    }
		    ଅନ୍ୟଥା ଯଦି (GA_GROW_FAILS(&regstack, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଆକାର(regstar_T)))
			status = RA_FAIL;
		    ଅନ୍ୟଥା
		    {
			regstack.ga_len += ଆକାର(regstar_T);
			rp = regstack_ଠେଲ(rst.minval <= rst.maxval
					? RS_STAR_LONG : RS_STAR_SHORT, scan);
			ଯଦି (rp == ଖାଲି)
			    status = RA_FAIL;
			ଅନ୍ୟଥା
			{
			    *(((regstar_T *)rp) - 1) = rst;
			    status = RA_BREAK;	    // skip the restore bits
			}
		    }
		}
		ଅନ୍ୟଥା
		    status = RA_NOMATCH;

	    }
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି NOMATCH:
	  ସ୍ଥିତି MATCH:
	  ସ୍ଥିତି SUBPAT:
	    rp = regstack_ଠେଲ(RS_NOMATCH, scan);
	    ଯଦି (rp == ଖାଲି)
		status = RA_FAIL;
	    ଅନ୍ୟଥା
	    {
		rp->rs_no = op;
		reg_save(&rp->rs_un.regsave, &backpos);
		next = OPERAND(scan);
		// We continue and handle the result when done.
	    }
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି BEHIND:
	  ସ୍ଥିତି NOBEHIND:
	    // Need a bit of room to store extra positions.
	    ଯଦି ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
	    {
		emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		status = RA_FAIL;
	    }
	    ଅନ୍ୟଥା ଯଦି (GA_GROW_FAILS(&regstack, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଆକାର(regbehind_T)))
		status = RA_FAIL;
	    ଅନ୍ୟଥା
	    {
		regstack.ga_len += ଆକାର(regbehind_T);
		rp = regstack_ଠେଲ(RS_BEHIND1, scan);
		ଯଦି (rp == ଖାଲି)
		    status = RA_FAIL;
		ଅନ୍ୟଥା
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
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି BHPOS:
	    ଯଦି (REG_MULTI)
	    {
		ଯଦି (behind_pos.rs_u.pos.col != (colnr_T)(rex.ନିବେଶ - rex.ଧାଡ଼ି)
			|| behind_pos.rs_u.pos.lnum != rex.lnum)
		    status = RA_NOMATCH;
	    }
	    ଅନ୍ୟଥା ଯଦି (behind_pos.rs_u.ptr != rex.ନିବେଶ)
		status = RA_NOMATCH;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି NEWL:
	    ଯଦି ((c != NUL || !REG_MULTI || rex.lnum > rex.reg_maxline
			     || rex.reg_ଧାଡ଼ି_lbr)
					   && (c != '\n' || !rex.reg_ଧାଡ଼ି_lbr))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା ଯଦି (rex.reg_ଧାଡ଼ି_lbr)
		ADVANCE_REGINPUT();
	    ଅନ୍ୟଥା
		reg_nextline();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି END:
	    status = RA_MATCH;	// Success!
	    ଭାଙ୍ଗ;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	    printf("Illegal op code %d\n", op);
#endif
	    status = RA_FAIL;
	    ଭାଙ୍ଗ;
	  }
	}

	// If we can't continue sequentially, break the inner loop.
	ଯଦି (status != RA_CONT)
	    ଭାଙ୍ଗ;

	// Continue in inner loop, advance to next item.
	scan = next;

    } // end of inner loop

    // If there is something on the regstack execute the code for the state.
    // If the state is popped then loop and use the older state.
    ଯେତେବେଳେ (regstack.ga_len > 0 && status != RA_FAIL)
    {
	rp = (regitem_T *)((ଅକ୍ଷର *)regstack.ga_data + regstack.ga_len) - 1;
	ବାଛ (rp->rs_state)
	{
	  ସ୍ଥିତି RS_NOPEN:
	    // Result is passed on as-is, simply pop the state.
	    regstack_କାଢ଼(&scan);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି RS_MOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    ଯଦି (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_startpos[rp->rs_no],
						  &rex.reg_startp[rp->rs_no]);
	    regstack_କାଢ଼(&scan);
	    ଭାଙ୍ଗ;

#ifdef FEAT_SYN_HL
	  ସ୍ଥିତି RS_ZOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    ଯଦି (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_startzpos[rp->rs_no],
						 &reg_startzp[rp->rs_no]);
	    regstack_କାଢ଼(&scan);
	    ଭାଙ୍ଗ;
#endif

	  ସ୍ଥିତି RS_MCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    ଯଦି (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_endpos[rp->rs_no],
						    &rex.reg_endp[rp->rs_no]);
	    regstack_କାଢ଼(&scan);
	    ଭାଙ୍ଗ;

#ifdef FEAT_SYN_HL
	  ସ୍ଥିତି RS_ZCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    ଯଦି (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_endzpos[rp->rs_no],
						   &reg_endzp[rp->rs_no]);
	    regstack_କାଢ଼(&scan);
	    ଭାଙ୍ଗ;
#endif

	  ସ୍ଥିତି RS_BRANCH:
	    ଯଦି (status == RA_MATCH)
		// this branch matched, use it
		regstack_କାଢ଼(&scan);
	    ଅନ୍ୟଥା
	    {
		ଯଦି (status != RA_BREAK)
		{
		    // After a non-matching branch: try next one.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    scan = rp->rs_scan;
		}
		ଯଦି (scan == ଖାଲି || OP(scan) != BRANCH)
		{
		    // no more branches, didn't find a match
		    status = RA_NOMATCH;
		    regstack_କାଢ଼(&scan);
		}
		ଅନ୍ୟଥା
		{
		    // Prepare to try a branch.
		    rp->rs_scan = regnext(scan);
		    reg_save(&rp->rs_un.regsave, &backpos);
		    scan = OPERAND(scan);
		}
	    }
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି RS_BRCPLX_MORE:
	    // Pop the state.  Restore pointers when there is no match.
	    ଯଦି (status == RA_NOMATCH)
	    {
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_ଗଣନା[rp->rs_no];	// decrement match count
	    }
	    regstack_କାଢ଼(&scan);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି RS_BRCPLX_LONG:
	    // Pop the state.  Restore pointers when there is no match.
	    ଯଦି (status == RA_NOMATCH)
	    {
		// There was no match, but we did find enough matches.
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_ଗଣନା[rp->rs_no];
		// continue with the items after "\{}"
		status = RA_CONT;
	    }
	    regstack_କାଢ଼(&scan);
	    ଯଦି (status == RA_CONT)
		scan = regnext(scan);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି RS_BRCPLX_SHORT:
	    // Pop the state.  Restore pointers when there is no match.
	    ଯଦି (status == RA_NOMATCH)
		// There was no match, try to match one more item.
		reg_restore(&rp->rs_un.regsave, &backpos);
	    regstack_କାଢ଼(&scan);
	    ଯଦି (status == RA_NOMATCH)
	    {
		scan = OPERAND(scan);
		status = RA_CONT;
	    }
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି RS_NOMATCH:
	    // Pop the state.  If the operand matches for NOMATCH or
	    // doesn't match for MATCH/SUBPAT, we fail.  Otherwise backup,
	    // except for SUBPAT, and continue with the next item.
	    ଯଦି (status == (rp->rs_no == NOMATCH ? RA_MATCH : RA_NOMATCH))
		status = RA_NOMATCH;
	    ଅନ୍ୟଥା
	    {
		status = RA_CONT;
		ଯଦି (rp->rs_no != SUBPAT)	// zero-width
		    reg_restore(&rp->rs_un.regsave, &backpos);
	    }
	    regstack_କାଢ଼(&scan);
	    ଯଦି (status == RA_CONT)
		scan = regnext(scan);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି RS_BEHIND1:
	    ଯଦି (status == RA_NOMATCH)
	    {
		regstack_କାଢ଼(&scan);
		regstack.ga_len -= ଆକାର(regbehind_T);
	    }
	    ଅନ୍ୟଥା
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
		scan = OPERAND(rp->rs_scan) + 4;
	    }
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି RS_BEHIND2:
	    // Looping for BEHIND / NOBEHIND match.
	    ଯଦି (status == RA_MATCH && reg_save_equal(&behind_pos))
	    {
		// found a match that ends where "next" started
		behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		ଯଦି (rp->rs_no == BEHIND)
		    reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
		ଅନ୍ୟଥା
		{
		    // But we didn't want a match.  Need to restore the
		    // subexpr, because what follows matched, so they have
		    // been set.
		    status = RA_NOMATCH;
		    restore_subexpr(((regbehind_T *)rp) - 1);
		}
		regstack_କାଢ଼(&scan);
		regstack.ga_len -= ଆକାର(regbehind_T);
	    }
	    ଅନ୍ୟଥା
	    {
		long limit;

		// No match or a match that doesn't end where we want it: Go
		// back one character.  May go to previous line once.
		no = OK;
		limit = OPERAND_MIN(rp->rs_scan);
		ଯଦି (REG_MULTI)
		{
		    ଯଦି (limit > 0
			    && ((rp->rs_un.regsave.rs_u.pos.lnum
						    < behind_pos.rs_u.pos.lnum
				    ? (colnr_T)STRLEN(rex.ଧାଡ଼ି)
				    : behind_pos.rs_u.pos.col)
				- rp->rs_un.regsave.rs_u.pos.col >= limit))
			no = FAIL;
		    ଅନ୍ୟଥା ଯଦି (rp->rs_un.regsave.rs_u.pos.col == 0)
		    {
			ଯଦି (rp->rs_un.regsave.rs_u.pos.lnum
					< behind_pos.rs_u.pos.lnum
				|| reg_getline(
					--rp->rs_un.regsave.rs_u.pos.lnum)
								  == ଖାଲି)
			    no = FAIL;
			ଅନ୍ୟଥା
			{
			    reg_restore(&rp->rs_un.regsave, &backpos);
			    rp->rs_un.regsave.rs_u.pos.col =
						 (colnr_T)STRLEN(rex.ଧାଡ଼ି);
			}
		    }
		    ଅନ୍ୟଥା
		    {
			ଯଦି (has_mbyte)
			{
			    char_u *ଧାଡ଼ି =
				  reg_getline(rp->rs_un.regsave.rs_u.pos.lnum);

			    rp->rs_un.regsave.rs_u.pos.col -=
				(*mb_head_off)(ଧାଡ଼ି, ଧାଡ଼ି
				    + rp->rs_un.regsave.rs_u.pos.col - 1) + 1;
			}
			ଅନ୍ୟଥା
			    --rp->rs_un.regsave.rs_u.pos.col;
		    }
		}
		ଅନ୍ୟଥା
		{
		    ଯଦି (rp->rs_un.regsave.rs_u.ptr == rex.ଧାଡ଼ି)
			no = FAIL;
		    ଅନ୍ୟଥା
		    {
			MB_PTR_BACK(rex.ଧାଡ଼ି, rp->rs_un.regsave.rs_u.ptr);
			ଯଦି (limit > 0 && (long)(behind_pos.rs_u.ptr
				     - rp->rs_un.regsave.rs_u.ptr) > limit)
			    no = FAIL;
		    }
		}
		ଯଦି (no == OK)
		{
		    // Advanced, prepare for finding match again.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    scan = OPERAND(rp->rs_scan) + 4;
		    ଯଦି (status == RA_MATCH)
		    {
			// We did match, so subexpr may have been changed,
			// need to restore them for the next try.
			status = RA_NOMATCH;
			restore_subexpr(((regbehind_T *)rp) - 1);
		    }
		}
		ଅନ୍ୟଥା
		{
		    // Can't advance.  For NOBEHIND that's a match.
		    behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		    ଯଦି (rp->rs_no == NOBEHIND)
		    {
			reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
			status = RA_MATCH;
		    }
		    ଅନ୍ୟଥା
		    {
			// We do want a proper match.  Need to restore the
			// subexpr if we had a match, because they may have
			// been set.
			ଯଦି (status == RA_MATCH)
			{
			    status = RA_NOMATCH;
			    restore_subexpr(((regbehind_T *)rp) - 1);
			}
		    }
		    regstack_କାଢ଼(&scan);
		    regstack.ga_len -= ଆକାର(regbehind_T);
		}
	    }
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି RS_STAR_LONG:
	  ସ୍ଥିତି RS_STAR_SHORT:
	    {
		regstar_T	    *rst = ((regstar_T *)rp) - 1;

		ଯଦି (status == RA_MATCH)
		{
		    regstack_କାଢ଼(&scan);
		    regstack.ga_len -= ଆକାର(regstar_T);
		    ଭାଙ୍ଗ;
		}

		// Tried once already, restore input pointers.
		ଯଦି (status != RA_BREAK)
		    reg_restore(&rp->rs_un.regsave, &backpos);

		// Repeat until we found a position where it could match.
		ପାଇଁ (;;)
		{
		    ଯଦି (status != RA_BREAK)
		    {
			// Tried first position already, advance.
			ଯଦି (rp->rs_state == RS_STAR_LONG)
			{
			    // Trying for longest match, but couldn't or
			    // didn't match -- back up one char.
			    ଯଦି (--rst->ଗଣନା_2 < rst->minval)
				ଭାଙ୍ଗ;
			    ଯଦି (rex.ନିବେଶ == rex.ଧାଡ଼ି)
			    {
				// backup to last char of previous line
				ଯଦି (rex.lnum == 0)
				{
				    status = RA_NOMATCH;
				    ଭାଙ୍ଗ;
				}
				--rex.lnum;
				rex.ଧାଡ଼ି = reg_getline(rex.lnum);
				// Just in case regrepeat() didn't count
				// right.
				ଯଦି (rex.ଧାଡ଼ି == ଖାଲି)
				    ଭାଙ୍ଗ;
				rex.ନିବେଶ = rex.ଧାଡ଼ି + reg_getline_len(rex.lnum);
				fast_breakcheck();
			    }
			    ଅନ୍ୟଥା
				MB_PTR_BACK(rex.ଧାଡ଼ି, rex.ନିବେଶ);
			}
			ଅନ୍ୟଥା
			{
			    // Range is backwards, use shortest match first.
			    // Careful: maxval and minval are exchanged!
			    // Couldn't or didn't match: try advancing one
			    // char.
			    ଯଦି (rst->ଗଣନା_2 == rst->minval
				  || regrepeat(OPERAND(rp->rs_scan), 1L) == 0)
				ଭାଙ୍ଗ;
			    ++rst->ଗଣନା_2;
			}
			ଯଦି (got_int)
			    ଭାଙ୍ଗ;
		    }
		    ଅନ୍ୟଥା
			status = RA_NOMATCH;

		    // If it could match, try it.
		    ଯଦି (rst->nextb == NUL || *rex.ନିବେଶ == rst->nextb
					     || *rex.ନିବେଶ == rst->nextb_ic)
		    {
			reg_save(&rp->rs_un.regsave, &backpos);
			scan = regnext(rp->rs_scan);
			status = RA_CONT;
			ଭାଙ୍ଗ;
		    }
		}
		ଯଦି (status != RA_CONT)
		{
		    // Failed.
		    regstack_କାଢ଼(&scan);
		    regstack.ga_len -= ଆକାର(regstar_T);
		    status = RA_NOMATCH;
		}
	    }
	    ଭାଙ୍ଗ;
	}

	// If we want to continue the inner loop or didn't pop a state
	// continue matching loop
	ଯଦି (status == RA_CONT || rp == (regitem_T *)
			     ((ଅକ୍ଷର *)regstack.ga_data + regstack.ga_len) - 1)
	    ଭାଙ୍ଗ;

#ifdef FEAT_RELTIME
	ଯଦି (bt_did_time_out(timed_out))
	{
	    status = RA_FAIL;
	    ଭାଙ୍ଗ;
	}
#endif
    }

    // May need to continue with the inner loop, starting at "scan".
    ଯଦି (status == RA_CONT)
	ଜାରି;

    // If the regstack is empty or something failed we are done.
    ଯଦି (regstack.ga_len == 0 || status == RA_FAIL)
    {
	ଯଦି (scan == ଖାଲି)
	{
	    // We get here only if there's trouble -- normally "case END" is
	    // the terminating point.
	    iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	    printf("Premature EOL\n");
#endif
	}
	ଫେରନ୍ତୁ (status == RA_MATCH);
    }

  } // End of loop until the regstack is empty.

  // NOTREACHED
}

/*
 * regtry - try match of "prog" with at rex.line["col"].
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    ସ୍ଥିର long
regtry(
    bt_regprog_T	*prog,
    colnr_T		col,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			*timed_out)	// flag set on timeout or NULL
{
    rex.ନିବେଶ = rex.ଧାଡ଼ି + col;
    rex.need_clear_subexpr = TRUE;
#ifdef FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    rex.need_clear_zsubexpr = (prog->reghasz == REX_SET);
#endif

    ଯଦି (regmatch(prog->program + 1, timed_out) == 0)
	ଫେରନ୍ତୁ 0;

    cleanup_subexpr();
    ଯଦି (REG_MULTI)
    {
	ଯଦି (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].col = col;
	}
	ଯଦି (rex.reg_endpos[0].lnum < 0)
	{
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(rex.ନିବେଶ - rex.ଧାଡ଼ି);
	}
	ଅନ୍ୟଥା
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (rex.reg_startp[0] == ଖାଲି)
	    rex.reg_startp[0] = rex.ଧାଡ଼ି + col;
	ଯଦି (rex.reg_endp[0] == ଖାଲି)
	    rex.reg_endp[0] = rex.ନିବେଶ;
    }
#ifdef FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = ଖାଲି;

    ଯଦି (prog->reghasz == REX_SET)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

	cleanup_zsubexpr();
	re_extmatch_out = make_extmatch();
	ଯଦି (re_extmatch_out == ଖାଲି)
	    ଫେରନ୍ତୁ 0;
	ପାଇଁ (i = 0; i < NSUBEXP; i++)
	{
	    ଯଦି (REG_MULTI)
	    {
		// Only accept single line matches.
		ଯଦି (reg_startzpos[i].lnum >= 0
			&& reg_endzpos[i].lnum == reg_startzpos[i].lnum
			&& reg_endzpos[i].col >= reg_startzpos[i].col)
		    re_extmatch_out->matches[i] =
			vim_strnsave(reg_getline(reg_startzpos[i].lnum)
						       + reg_startzpos[i].col,
				   reg_endzpos[i].col - reg_startzpos[i].col);
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (reg_startzp[i] != ଖାଲି && reg_endzp[i] != ଖାଲି)
		    re_extmatch_out->matches[i] =
			    vim_strnsave(reg_startzp[i],
						reg_endzp[i] - reg_startzp[i]);
	    }
	}
    }
#endif
    ଫେରନ୍ତୁ 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    ସ୍ଥିର long
bt_regexec_both(
    char_u	*ଧାଡ଼ି,
    colnr_T	startcol,	// column to start looking for match
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*timed_out)	// flag set on timeout or NULL
{
    bt_regprog_T    *prog;
    char_u	    *s;
    colnr_T	    col = startcol;
    long	    retval = 0L;

    // Create "regstack" and "backpos" if they are not allocated yet.
    // We allocate *_INITIAL amount of bytes first and then set the grow size
    // to much bigger value to avoid many malloc calls in case of deep regular
    // expressions.
    ଯଦି (regstack.ga_data == ଖାଲି)
    {
	// Use an item size of 1 byte, since we push different things
	// onto the regstack.
	ga_init2(&regstack, 1, REGSTACK_INITIAL);
	(ଶୂନ୍ୟ)ga_grow(&regstack, REGSTACK_INITIAL);
	regstack.ga_growsize = REGSTACK_INITIAL * 8;
    }

    ଯଦି (backpos.ga_data == ଖାଲି)
    {
	ga_init2(&backpos, ଆକାର(backpos_T), BACKPOS_INITIAL);
	(ଶୂନ୍ୟ)ga_grow(&backpos, BACKPOS_INITIAL);
	backpos.ga_growsize = BACKPOS_INITIAL * 8;
    }

    ଯଦି (REG_MULTI)
    {
	prog = (bt_regprog_T *)rex.reg_mmatch->regprog;
	ଧାଡ଼ି = reg_getline((linenr_T)0);
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    ଅନ୍ୟଥା
    {
	prog = (bt_regprog_T *)rex.reg_match->regprog;
	rex.reg_startp = rex.reg_match->startp;
	rex.reg_endp = rex.reg_match->endp;
    }

    // Be paranoid...
    ଯଦି (prog == ଖାଲି || ଧାଡ଼ି == ଖାଲି)
    {
	iemsg(e_null_argument);
	ଯାଅ theend;
    }

    // Check validity of program.
    ଯଦି (prog_magic_wrong())
	ଯାଅ theend;

    // If the start column is past the maximum column: no need to try.
    ଯଦି (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	ଯାଅ theend;

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    ଯଦି (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    ଅନ୍ୟଥା ଯଦି (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    ଯଦି (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    // If there is a "must appear" string, look for it.
    ଯଦି (prog->regmust != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;

	ଯଦି (has_mbyte)
	    c = (*mb_ptr2char)(prog->regmust);
	ଅନ୍ୟଥା
	    c = *prog->regmust;
	s = ଧାଡ଼ି + col;

	// This is used very often, esp. for ":global".  Use three versions of
	// the loop to avoid overhead of conditions.
	ଯଦି (!rex.reg_ic && !has_mbyte)
	    ଯେତେବେଳେ ((s = vim_strbyte(s, c)) != ଖାଲି)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = prog->regmlen;
		ଯଦି (cstrncmp(s, prog->regmust, &len) == 0)
		    ଭାଙ୍ଗ;		// Found it.
		++s;
	    }
	ଅନ୍ୟଥା ଯଦି (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	    ଯେତେବେଳେ ((s = vim_strchr(s, c)) != ଖାଲି)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = prog->regmlen;
		ଯଦି (cstrncmp(s, prog->regmust, &len) == 0)
		    ଭାଙ୍ଗ;		// Found it.
		MB_PTR_ADV(s);
	    }
	ଅନ୍ୟଥା
	    ଯେତେବେଳେ ((s = cstrchr(s, c)) != ଖାଲି)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = prog->regmlen;
		ଯଦି (cstrncmp(s, prog->regmust, &len) == 0)
		    ଭାଙ୍ଗ;		// Found it.
		MB_PTR_ADV(s);
	    }
	ଯଦି (s == ଖାଲି)		// Not present.
	    ଯାଅ theend;
    }

    rex.ଧାଡ଼ି = ଧାଡ଼ି;
    rex.lnum = 0;
    reg_toolong = FALSE;

    // Simplest case: Anchored match need be tried only once.
    ଯଦି (prog->reganch)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	c;

	ଯଦି (has_mbyte)
	    c = (*mb_ptr2char)(rex.ଧାଡ଼ି + col);
	ଅନ୍ୟଥା
	    c = rex.ଧାଡ଼ି[col];
	ଯଦି (prog->regstart == NUL
		|| prog->regstart == c
		|| (rex.reg_ic
		    && (((enc_utf8 && utf_fold(prog->regstart) == utf_fold(c)))
			|| (c < 255 && prog->regstart < 255 &&
			    MB_TOLOWER(prog->regstart) == MB_TOLOWER(c)))))
	    retval = regtry(prog, col, timed_out);
	ଅନ୍ୟଥା
	    retval = 0;
    }
    ଅନ୍ୟଥା
    {
	// Messy cases:  unanchored match.
	ଯେତେବେଳେ (!got_int)
	{
	    ଯଦି (prog->regstart != NUL)
	    {
		// Skip until the char we know it must start with.
		// Used often, do some work to avoid call overhead.
		ଯଦି (!rex.reg_ic && !has_mbyte)
		    s = vim_strbyte(rex.ଧାଡ଼ି + col, prog->regstart);
		ଅନ୍ୟଥା
		    s = cstrchr(rex.ଧାଡ଼ି + col, prog->regstart);
		ଯଦି (s == ଖାଲି)
		{
		    retval = 0;
		    ଭାଙ୍ଗ;
		}
		col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(s - rex.ଧାଡ଼ି);
	    }

	    // Check for maximum column to try.
	    ଯଦି (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	    {
		retval = 0;
		ଭାଙ୍ଗ;
	    }

	    retval = regtry(prog, col, timed_out);
	    ଯଦି (retval > 0)
		ଭାଙ୍ଗ;

	    // if not currently on the first line, get it again
	    ଯଦି (rex.lnum != 0)
	    {
		rex.lnum = 0;
		rex.ଧାଡ଼ି = reg_getline((linenr_T)0);
	    }
	    ଯଦି (rex.ଧାଡ଼ି[col] == NUL)
		ଭାଙ୍ଗ;
	    ଯଦି (has_mbyte)
		col += (*mb_ptr2len)(rex.ଧାଡ଼ି + col);
	    ଅନ୍ୟଥା
		++col;
#ifdef FEAT_RELTIME
	    ଯଦି (bt_did_time_out(timed_out))
		ଭାଙ୍ଗ;
#endif
	}
    }

theend:
    // Free "reg_tofree" when it's a bit big.
    // Free regstack and backpos if they are bigger than their initial size.
    ଯଦି (reg_tofreelen > 400)
	VIM_CLEAR(reg_tofree);
    ଯଦି (regstack.ga_maxlen > REGSTACK_INITIAL)
	ga_clear(&regstack);
    ଯଦି (backpos.ga_maxlen > BACKPOS_INITIAL)
	ga_clear(&backpos);

    ଯଦି (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs
	// and \ze are used.
	ଯଦି (REG_MULTI)
	{
	    lpos_T *start = &rex.reg_mmatch->startpos[0];
	    lpos_T *end = &rex.reg_mmatch->endpos[0];

	    ଯଦି (end->lnum < start->lnum
			|| (end->lnum == start->lnum && end->col < start->col))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_mmatch->rmm_matchcol = col;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (rex.reg_match->endp[0] < rex.reg_match->startp[0])
		rex.reg_match->endp[0] = rex.reg_match->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_match->rm_matchcol = col;
	}
    }

    ଫେରନ୍ତୁ retval;
}

/*
 * Match a regexp against a string.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 * if "line_lbr" is TRUE  consider a "\n" in "line" to be a line break.
 *
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bt_regexec_nl(
    regmatch_T	*rmp,
    char_u	*ଧାଡ଼ି,	// string to match against
    colnr_T	col,	// column to start looking for match
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଧାଡ଼ି_lbr)
{
    rex.reg_match = rmp;
    rex.reg_mmatch = ଖାଲି;
    rex.reg_maxline = 0;
    rex.reg_ଧାଡ଼ି_lbr = ଧାଡ଼ି_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = ଖାଲି;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;

    ଫେରନ୍ତୁ bt_regexec_both(ଧାଡ଼ି, col, ଖାଲି);
}

/*
 * Match a regexp against multiple lines.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 *
 * Return zero if there is no match.  Return number of lines contained in the
 * match otherwise.
 */
    ସ୍ଥିର long
bt_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	col,		// column to start looking for match
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*timed_out)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    ଫେରନ୍ତୁ bt_regexec_both(ଖାଲି, col, timed_out);
}

/*
 * Compare a number with the operand of RE_LNUM, RE_COL or RE_VCOL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
re_ଅଙ୍କ_cmp(long_u val, char_u *scan)
{
    long_u  n = OPERAND_MIN(scan);

    ଯଦି (OPERAND_CMP(scan) == '>')
	ଫେରନ୍ତୁ val > n;
    ଯଦି (OPERAND_CMP(scan) == '<')
	ଫେରନ୍ତୁ val < n;
    ଫେରନ୍ତୁ val == n;
}

#ifdef BT_REGEXP_DUMP

/*
 * regdump - dump a regexp onto stdout in vaguely comprehensible form
 */
    ସ୍ଥିର ଶୂନ୍ୟ
regdump(char_u *pattern, bt_regprog_T *r)
{
    char_u  *s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    op = EXACTLY;	// Arbitrary non-END op.
    char_u  *next;
    char_u  *end = ଖାଲି;
    FILE    *f;

# ifdef BT_REGEXP_LOG
    f = fopen("bt_regexp_log.log", "a");
# ଅନ୍ୟଥା
    f = stdout;
# endif
    ଯଦି (f == ଖାଲି)
	ଫେରନ୍ତୁ;
    fprintf(f, "-------------------------------------\n\r\nregcomp(%s):\r\n", pattern);

    s = r->program + 1;
    // Loop until we find the END that isn't before a referred next (an END
    // can also appear in a NOMATCH operand).
    ଯେତେବେଳେ ((op != END || s <= end) && s < r->program + r->regsz)
    {
	op = OP(s);
	fprintf(f, "%2d%s", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(s - r->program), regprop(s)); // Where, what.
	next = (s + 3 <= r->program + r->regsz) ? regnext(s) : ଖାଲି;
	ଯଦି (next == ଖାଲି)	// Next ptr.
	    fprintf(f, "(0)");
	ଅନ୍ୟଥା
	    fprintf(f, "(%d)", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((s - r->program) + (next - s)));
	ଯଦି (end < next)
	    end = next;
	ଯଦି (op == BRACE_LIMITS)
	{
	    // Two ints
	    fprintf(f, " minval %ld, maxval %ld", OPERAND_MIN(s), OPERAND_MAX(s));
	    s += 8;
	}
	ଅନ୍ୟଥା ଯଦି (op == BEHIND || op == NOBEHIND)
	{
	    // one int
	    fprintf(f, " count %ld", OPERAND_MIN(s));
	    s += 4;
	}
	ଅନ୍ୟଥା ଯଦି (op == RE_LNUM || op == RE_COL || op == RE_VCOL)
	{
	    // one int plus comparator
	    fprintf(f, " count %ld", OPERAND_MIN(s));
	    s += 5;
	}
	s += 3;
	ଯଦି (op == MULTIBYTECODE)
	{
	    fprintf(f, " mbc=%d", utf_ptr2char(s));
	    s += utfc_ptr2len(s);
	}
	ଯଦି (op == ANYOF || op == ANYOF + ADD_NL
		|| op == ANYBUT || op == ANYBUT + ADD_NL
		|| op == EXACTLY)
	{
	    // Literal string, where present.
	    fprintf(f, "\nxxxxxxxxx\n");
	    ଯେତେବେଳେ (*s != NUL && s < r->program + r->regsz)
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
    ଯଦି (r->regstart != NUL)
	fprintf(f, "start `%s' 0x%x; ", r->regstart < 256
		? (ଅକ୍ଷର *)transchar(r->regstart)
		: "multibyte", r->regstart);
    ଯଦି (r->reganch)
	fprintf(f, "anchored; ");
    ଯଦି (r->regmust != ଖାଲି)
	fprintf(f, "must have \"%s\"", r->regmust);
    fprintf(f, "\r\n");

# ifdef BT_REGEXP_LOG
    fclose(f);
# endif
}
#endif	    // BT_REGEXP_DUMP

#ifdef DEBUG
/*
 * regprop - printable representation of opcode
 */
    ସ୍ଥିର char_u *
regprop(char_u *op)
{
    ଅକ୍ଷର	    *p;
    ସ୍ଥିର ଅକ୍ଷର	    buf[50];
    ସ୍ଥିର size_t   buflen = 0;

    STRCPY(buf, ":");
    buflen = 1;

    ବାଛ ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା) OP(op))
    {
      ସ୍ଥିତି BOL:
	p = "BOL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି EOL:
	p = "EOL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି RE_BOF:
	p = "BOF";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି RE_EOF:
	p = "EOF";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି CURSOR:
	p = "CURSOR";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି RE_VISUAL:
	p = "RE_VISUAL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି RE_LNUM:
	p = "RE_LNUM";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି RE_MARK:
	p = "RE_MARK";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି RE_COL:
	p = "RE_COL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି RE_VCOL:
	p = "RE_VCOL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି BOW:
	p = "BOW";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି EOW:
	p = "EOW";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି ANY:
	p = "ANY";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି ANY + ADD_NL:
	p = "ANY+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି ANYOF:
	p = "ANYOF";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି ANYOF + ADD_NL:
	p = "ANYOF+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି ANYBUT:
	p = "ANYBUT";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି ANYBUT + ADD_NL:
	p = "ANYBUT+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି IDENT:
	p = "IDENT";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି IDENT + ADD_NL:
	p = "IDENT+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି SIDENT:
	p = "SIDENT";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି SIDENT + ADD_NL:
	p = "SIDENT+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି KWORD:
	p = "KWORD";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି KWORD + ADD_NL:
	p = "KWORD+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି SKWORD:
	p = "SKWORD";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି SKWORD + ADD_NL:
	p = "SKWORD+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି FNAME:
	p = "FNAME";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି FNAME + ADD_NL:
	p = "FNAME+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି SFNAME:
	p = "SFNAME";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି SFNAME + ADD_NL:
	p = "SFNAME+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି PRINT:
	p = "PRINT";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି PRINT + ADD_NL:
	p = "PRINT+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି SPRINT:
	p = "SPRINT";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି SPRINT + ADD_NL:
	p = "SPRINT+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି WHITE:
	p = "WHITE";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି WHITE + ADD_NL:
	p = "WHITE+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି NWHITE:
	p = "NWHITE";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି NWHITE + ADD_NL:
	p = "NWHITE+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି DIGIT:
	p = "DIGIT";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି DIGIT + ADD_NL:
	p = "DIGIT+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି NDIGIT:
	p = "NDIGIT";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି NDIGIT + ADD_NL:
	p = "NDIGIT+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି HEX:
	p = "HEX";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି HEX + ADD_NL:
	p = "HEX+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି NHEX:
	p = "NHEX";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି NHEX + ADD_NL:
	p = "NHEX+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି OCTAL:
	p = "OCTAL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି OCTAL + ADD_NL:
	p = "OCTAL+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି NOCTAL:
	p = "NOCTAL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି NOCTAL + ADD_NL:
	p = "NOCTAL+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି WORD:
	p = "WORD";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି WORD + ADD_NL:
	p = "WORD+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି NWORD:
	p = "NWORD";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି NWORD + ADD_NL:
	p = "NWORD+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି HEAD:
	p = "HEAD";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି HEAD + ADD_NL:
	p = "HEAD+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି NHEAD:
	p = "NHEAD";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି NHEAD + ADD_NL:
	p = "NHEAD+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି ALPHA:
	p = "ALPHA";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି ALPHA + ADD_NL:
	p = "ALPHA+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି NALPHA:
	p = "NALPHA";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି NALPHA + ADD_NL:
	p = "NALPHA+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି LOWER:
	p = "LOWER";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି LOWER + ADD_NL:
	p = "LOWER+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି NLOWER:
	p = "NLOWER";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି NLOWER + ADD_NL:
	p = "NLOWER+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି UPPER:
	p = "UPPER";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି UPPER + ADD_NL:
	p = "UPPER+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି NUPPER:
	p = "NUPPER";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି NUPPER + ADD_NL:
	p = "NUPPER+NL";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି BRANCH:
	p = "BRANCH";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି EXACTLY:
	p = "EXACTLY";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି NOTHING:
	p = "NOTHING";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି BACK:
	p = "BACK";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି END:
	p = "END";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି MOPEN + 0:
	p = "MATCH START";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି MOPEN + 1:
      ସ୍ଥିତି MOPEN + 2:
      ସ୍ଥିତି MOPEN + 3:
      ସ୍ଥିତି MOPEN + 4:
      ସ୍ଥିତି MOPEN + 5:
      ସ୍ଥିତି MOPEN + 6:
      ସ୍ଥିତି MOPEN + 7:
      ସ୍ଥିତି MOPEN + 8:
      ସ୍ଥିତି MOPEN + 9:
	buflen += vim_snprintf(buf + buflen, ଆକାର(buf) - buflen, "MOPEN%d", OP(op) - MOPEN);
	p = ଖାଲି;
	ଭାଙ୍ଗ;
      ସ୍ଥିତି MCLOSE + 0:
	p = "MATCH END";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି MCLOSE + 1:
      ସ୍ଥିତି MCLOSE + 2:
      ସ୍ଥିତି MCLOSE + 3:
      ସ୍ଥିତି MCLOSE + 4:
      ସ୍ଥିତି MCLOSE + 5:
      ସ୍ଥିତି MCLOSE + 6:
      ସ୍ଥିତି MCLOSE + 7:
      ସ୍ଥିତି MCLOSE + 8:
      ସ୍ଥିତି MCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, ଆକାର(buf) - buflen, "MCLOSE%d", OP(op) - MCLOSE);
	p = ଖାଲି;
	ଭାଙ୍ଗ;
      ସ୍ଥିତି BACKREF + 1:
      ସ୍ଥିତି BACKREF + 2:
      ସ୍ଥିତି BACKREF + 3:
      ସ୍ଥିତି BACKREF + 4:
      ସ୍ଥିତି BACKREF + 5:
      ସ୍ଥିତି BACKREF + 6:
      ସ୍ଥିତି BACKREF + 7:
      ସ୍ଥିତି BACKREF + 8:
      ସ୍ଥିତି BACKREF + 9:
	buflen += vim_snprintf(buf + buflen, ଆକାର(buf) - buflen, "BACKREF%d", OP(op) - BACKREF);
	p = ଖାଲି;
	ଭାଙ୍ଗ;
      ସ୍ଥିତି NOPEN:
	p = "NOPEN";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି NCLOSE:
	p = "NCLOSE";
	ଭାଙ୍ଗ;
# ifdef FEAT_SYN_HL
      ସ୍ଥିତି ZOPEN + 1:
      ସ୍ଥିତି ZOPEN + 2:
      ସ୍ଥିତି ZOPEN + 3:
      ସ୍ଥିତି ZOPEN + 4:
      ସ୍ଥିତି ZOPEN + 5:
      ସ୍ଥିତି ZOPEN + 6:
      ସ୍ଥିତି ZOPEN + 7:
      ସ୍ଥିତି ZOPEN + 8:
      ସ୍ଥିତି ZOPEN + 9:
	buflen += vim_snprintf(buf + buflen, ଆକାର(buf) - buflen, "ZOPEN%d", OP(op) - ZOPEN);
	p = ଖାଲି;
	ଭାଙ୍ଗ;
      ସ୍ଥିତି ZCLOSE + 1:
      ସ୍ଥିତି ZCLOSE + 2:
      ସ୍ଥିତି ZCLOSE + 3:
      ସ୍ଥିତି ZCLOSE + 4:
      ସ୍ଥିତି ZCLOSE + 5:
      ସ୍ଥିତି ZCLOSE + 6:
      ସ୍ଥିତି ZCLOSE + 7:
      ସ୍ଥିତି ZCLOSE + 8:
      ସ୍ଥିତି ZCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, ଆକାର(buf) - buflen, "ZCLOSE%d", OP(op) - ZCLOSE);
	p = ଖାଲି;
	ଭାଙ୍ଗ;
      ସ୍ଥିତି ZREF + 1:
      ସ୍ଥିତି ZREF + 2:
      ସ୍ଥିତି ZREF + 3:
      ସ୍ଥିତି ZREF + 4:
      ସ୍ଥିତି ZREF + 5:
      ସ୍ଥିତି ZREF + 6:
      ସ୍ଥିତି ZREF + 7:
      ସ୍ଥିତି ZREF + 8:
      ସ୍ଥିତି ZREF + 9:
	buflen += vim_snprintf(buf + buflen, ଆକାର(buf) - buflen, "ZREF%d", OP(op) - ZREF);
	p = ଖାଲି;
	ଭାଙ୍ଗ;
# endif
      ସ୍ଥିତି STAR:
	p = "STAR";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି PLUS:
	p = "PLUS";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି NOMATCH:
	p = "NOMATCH";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି MATCH:
	p = "MATCH";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି BEHIND:
	p = "BEHIND";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି NOBEHIND:
	p = "NOBEHIND";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି SUBPAT:
	p = "SUBPAT";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି BRACE_LIMITS:
	p = "BRACE_LIMITS";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି BRACE_SIMPLE:
	p = "BRACE_SIMPLE";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି BRACE_COMPLEX + 0:
      ସ୍ଥିତି BRACE_COMPLEX + 1:
      ସ୍ଥିତି BRACE_COMPLEX + 2:
      ସ୍ଥିତି BRACE_COMPLEX + 3:
      ସ୍ଥିତି BRACE_COMPLEX + 4:
      ସ୍ଥିତି BRACE_COMPLEX + 5:
      ସ୍ଥିତି BRACE_COMPLEX + 6:
      ସ୍ଥିତି BRACE_COMPLEX + 7:
      ସ୍ଥିତି BRACE_COMPLEX + 8:
      ସ୍ଥିତି BRACE_COMPLEX + 9:
	buflen += vim_snprintf(buf + buflen, ଆକାର(buf) - buflen, "BRACE_COMPLEX%d", OP(op) - BRACE_COMPLEX);
	p = ଖାଲି;
	ଭାଙ୍ଗ;
      ସ୍ଥିତି MULTIBYTECODE:
	p = "MULTIBYTECODE";
	ଭାଙ୍ଗ;
      ସ୍ଥିତି NEWL:
	p = "NEWL";
	ଭାଙ୍ଗ;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	buflen += vim_snprintf(buf + buflen, ଆକାର(buf) - buflen, "corrupt %d", OP(op));
	p = ଖାଲି;
	ଭାଙ୍ଗ;
    }
    ଯଦି (p != ଖାଲି)
	vim_strncpy((char_u *)buf + buflen, (char_u *)p, ଆକାର(buf) - buflen - 1);
    ଫେରନ୍ତୁ (char_u *)buf;
}
#endif	    // DEBUG
