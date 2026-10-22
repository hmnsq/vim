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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	ಅಂಕೆ_complex_braces; // Complex \{...} count
ಸ್ಥಿರ char_u	*regcode;	// Code-emit pointer, or JUST_CALC_SIZE
ಸ್ಥಿರ long	regsize;	// Code size.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	reg_toolong;	// TRUE when offset out of range
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	bt_reg_parse_depth;	// nesting depth in reg()
ಸ್ಥಿರ char_u	had_endbrace[NSUBEXP];	// flags, TRUE if end of () found
ಸ್ಥಿರ long	brace_min[10];	// Minimums for complex brace repeats
ಸ್ಥಿರ long	brace_max[10];	// Maximums for complex brace repeats
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	brace_ಎಣಿಕೆ[10]; // Current counts for complex brace repeats
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	one_exactly = FALSE;	// only do one char for EXACTLY

// When making changes to classchars also change nfa_classcodes.
ಸ್ಥಿರ char_u	*classchars = (char_u *)".iIkKfFpPsSdDxXoOwWhHaAlLuU";
ಸ್ಥಿರ ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ classcodes[] = {
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
bt_recognize_char_class(char_u *start, char_u *end)
{
    ಪೂರ್ಣಾಙ್ಕ		newl;
    ಪೂರ್ಣಾಙ್ಕ		config = get_char_class_bits(start, end, &newl);

    // A newline would need the ADD_NL variant, don't bother with it here.
    ಯದಿ (config < 0 || newl)
	ಪ್ರತ್ಯಾಗಮ 0;

	// The old engine has no case-insensitive class opcode, so [a-z] and [A-Z]
	// are left as collections.  The classes below are case-independent.
    ವಿಕಲ್ಪ (config)
    {
	ಪಕ್ಷ CLASS_o9:
	    ಪ್ರತ್ಯಾಗಮ DIGIT;
	ಪಕ್ಷ CLASS_not | CLASS_o9:
	    ಪ್ರತ್ಯಾಗಮ NDIGIT;
	ಪಕ್ಷ CLASS_af | CLASS_AF | CLASS_o9:
	    ಪ್ರತ್ಯಾಗಮ HEX;
	ಪಕ್ಷ CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    ಪ್ರತ್ಯಾಗಮ NHEX;
	ಪಕ್ಷ CLASS_o7:
	    ಪ್ರತ್ಯಾಗಮ OCTAL;
	ಪಕ್ಷ CLASS_not | CLASS_o7:
	    ಪ್ರತ್ಯಾಗಮ NOCTAL;
	ಪಕ್ಷ CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    ಪ್ರತ್ಯಾಗಮ WORD;
	ಪಕ್ಷ CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    ಪ್ರತ್ಯಾಗಮ NWORD;
	ಪಕ್ಷ CLASS_az | CLASS_AZ | CLASS_underscore:
	    ಪ್ರತ್ಯಾಗಮ HEAD;
	ಪಕ್ಷ CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    ಪ್ರತ್ಯಾಗಮ NHEAD;
	ಪಕ್ಷ CLASS_az | CLASS_AZ:
	    ಪ್ರತ್ಯಾಗಮ ALPHA;
	ಪಕ್ಷ CLASS_not | CLASS_az | CLASS_AZ:
	    ಪ್ರತ್ಯಾಗಮ NALPHA;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * When regcode is set to this value, code is not emitted and size is computed
 * instead.
 */
#define JUST_CALC_SIZE	((char_u *) -1)

// Values for rs_state in regitem_T.
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ regstate_E
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
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ಸಙ್ಘ
    {
	char_u	*ptr;	// rex.input pointer, for single-line regexp
	lpos_T	pos;	// rex.input pos, for multi-line regexp
    } rs_u;
    ಪೂರ್ಣಾಙ್ಕ		rs_len;
} regsave_T;

// struct to save start/end pointer/position in for \(\)
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ಸಙ್ಘ
    {
	char_u	*ptr;
	lpos_T	pos;
    } se_u;
} save_se_T;

// used for BEHIND and NOBEHIND matching
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ regbehind_S
{
    regsave_T	save_after;
    regsave_T	save_behind;
    ಪೂರ್ಣಾಙ್ಕ		save_need_clear_subexpr;
    save_se_T   save_start[NSUBEXP];
    save_se_T   save_end[NSUBEXP];
} regbehind_T;

/*
 * When there are alternatives a regstate_T is put on the regstack to remember
 * what we are doing.
 * Before it may be another type of item, depending on rs_state, to remember
 * more things.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ regitem_S
{
    regstate_T	rs_state;	// what we are doing, one of RS_ above
    short	rs_no;		// submatch nr or BEHIND/NOBEHIND
    char_u	*rs_scan;	// current node in program
    ಸಙ್ಘ
    {
	save_se_T  sesave;
	regsave_T  regsave;
    } rs_un;			// room for saving rex.input
} regitem_T;


// used for STAR, PLUS and BRACE_SIMPLE matching
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ regstar_S
{
    ಪೂರ್ಣಾಙ್ಕ		nextb;		// next byte
    ಪೂರ್ಣಾಙ್ಕ		nextb_ic;	// next byte reverse case
    long	ಎಣಿಕೆ;
    long	minval;
    long	maxval;
} regstar_T;

// used to store input position when a BACK was encountered, so that we now if
// we made any progress since the last time.
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ backpos_S
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
ಸ್ಥಿರ garray_T	regstack = {0, 0, 0, 0, ರಿಕ್ತ};
ಸ್ಥಿರ garray_T	backpos = {0, 0, 0, 0, ರಿಕ್ತ};

ಸ್ಥಿರ regsave_T behind_pos;

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
#define OP(p)		((ಪೂರ್ಣಾಙ್ಕ)*(p))
#define NEXT(p)		(((*((p) + 1) & 0377) << 8) + (*((p) + 2) & 0377))
#define OPERAND(p)	((p) + 3)
// Obtain an operand that was stored as four bytes, MSB first.
#define OPERAND_MIN(p)	(((long)(p)[3] << 24) + ((long)(p)[4] << 16) \
			+ ((long)(p)[5] << 8) + (long)(p)[6])
// Obtain a second operand stored as four bytes.
#define OPERAND_MAX(p)	OPERAND_MIN((p) + 4)
// Obtain a second single-byte operand stored after a four bytes operand.
#define OPERAND_CMP(p)	(p)[7]

ಸ್ಥಿರ char_u *reg(ಪೂರ್ಣಾಙ್ಕ paren, ಪೂರ್ಣಾಙ್ಕ *flagp);

#ifdef BT_REGEXP_DUMP
ಸ್ಥಿರ ಶೂನ್ಯ	regdump(char_u *, bt_regprog_T *);
#endif

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	re_ಅಂಕೆ_cmp(long_u val, char_u *scan);

#ifdef DEBUG
ಸ್ಥಿರ char_u	*regprop(char_u *);

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	regnarrate = 0;
#endif


/*
 * Setup to parse the regexp.  Used once to get the length and once to do it.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
regcomp_start(
    char_u	*ಅಭಿವ್ಯಕ್ತಿ,
    ಪೂರ್ಣಾಙ್ಕ		re_flags)	    // see vim_regcomp()
{
    initchr(ಅಭಿವ್ಯಕ್ತಿ);
    ಯದಿ (re_flags & RE_MAGIC)
	reg_magic = MAGIC_ON;
    ಅನ್ಯಥಾ
	reg_magic = MAGIC_OFF;
    reg_string = (re_flags & RE_STRING);
    reg_strict = (re_flags & RE_STRICT);
    get_cpo_flags();

    ಅಂಕೆ_complex_braces = 0;
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
#ಯದಿ defined(FEAT_SYN_HL)
    had_eol = FALSE;
#endif
}

/*
 * Return TRUE if MULTIBYTECODE should be used instead of EXACTLY for
 * character "c".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
use_multibytecode(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪ್ರತ್ಯಾಗಮ has_mbyte && (*mb_char2len)(c) > 1
		     && (re_multi_type(peekchr()) != NOT_MULTI
			     || (enc_utf8 && utf_iscomposing(c)));
}

/*
 * Emit (if appropriate) a byte of code
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
regc(ಪೂರ್ಣಾಙ್ಕ b)
{
    ಯದಿ (regcode == JUST_CALC_SIZE)
	regsize++;
    ಅನ್ಯಥಾ
	*regcode++ = b;
}

/*
 * Emit (if appropriate) a multi-byte character of code
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
regmbc(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಯದಿ (!has_mbyte && c > 0xff)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (regcode == JUST_CALC_SIZE)
	regsize += (*mb_char2len)(c);
    ಅನ್ಯಥಾ
	regcode += (*mb_char2bytes)(c, regcode);
}


/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * NOTE: When changing this function, also change nfa_emit_equi_class()
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
reg_equi_class(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಯದಿ (enc_utf8 || STRCMP(p_enc, "latin1") == 0
					 || STRCMP(p_enc, "iso-8859-15") == 0)
    {
	ವಿಕಲ್ಪ (c)
	{
	    // Do not use '\300' style, it results in a negative number.
	    ಪಕ್ಷ 'A': ಪಕ್ಷ 0xc0: ಪಕ್ಷ 0xc1: ಪಕ್ಷ 0xc2: ಪಕ್ಷ 0xc3: ಪಕ್ಷ 0xc4:
	    ಪಕ್ಷ 0xc5: ಪಕ್ಷ 0x100: ಪಕ್ಷ 0x102: ಪಕ್ಷ 0x104: ಪಕ್ಷ 0x1cd:
	    ಪಕ್ಷ 0x1de: ಪಕ್ಷ 0x1e0: ಪಕ್ಷ 0x1fa: ಪಕ್ಷ 0x202: ಪಕ್ಷ 0x226:
	    ಪಕ್ಷ 0x23a: ಪಕ್ಷ 0x1e00: ಪಕ್ಷ 0x1ea0: ಪಕ್ಷ 0x1ea2: ಪಕ್ಷ 0x1ea4:
	    ಪಕ್ಷ 0x1ea6: ಪಕ್ಷ 0x1ea8: ಪಕ್ಷ 0x1eaa: ಪಕ್ಷ 0x1eac: ಪಕ್ಷ 0x1eae:
	    ಪಕ್ಷ 0x1eb0: ಪಕ್ಷ 0x1eb2: ಪಕ್ಷ 0x1eb4: ಪಕ್ಷ 0x1eb6:
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
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'B': ಪಕ್ಷ 0x181: ಪಕ್ಷ 0x243: ಪಕ್ಷ 0x1e02:
	    ಪಕ್ಷ 0x1e04: ಪಕ್ಷ 0x1e06:
		      regmbc('B');
		      regmbc(0x181); regmbc(0x243); regmbc(0x1e02);
		      regmbc(0x1e04); regmbc(0x1e06);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'C': ಪಕ್ಷ 0xc7:
	    ಪಕ್ಷ 0x106: ಪಕ್ಷ 0x108: ಪಕ್ಷ 0x10a: ಪಕ್ಷ 0x10c: ಪಕ್ಷ 0x187:
	    ಪಕ್ಷ 0x23b: ಪಕ್ಷ 0x1e08: ಪಕ್ಷ 0xa792:
		      regmbc('C'); regmbc(0xc7);
		      regmbc(0x106); regmbc(0x108); regmbc(0x10a);
		      regmbc(0x10c); regmbc(0x187); regmbc(0x23b);
		      regmbc(0x1e08); regmbc(0xa792);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'D': ಪಕ್ಷ 0x10e: ಪಕ್ಷ 0x110: ಪಕ್ಷ 0x18a:
	    ಪಕ್ಷ 0x1e0a: ಪಕ್ಷ 0x1e0c: ಪಕ್ಷ 0x1e0e: ಪಕ್ಷ 0x1e10:
	    ಪಕ್ಷ 0x1e12:
		      regmbc('D'); regmbc(0x10e); regmbc(0x110);
		      regmbc(0x18a); regmbc(0x1e0a); regmbc(0x1e0c);
		      regmbc(0x1e0e); regmbc(0x1e10); regmbc(0x1e12);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'E': ಪಕ್ಷ 0xc8: ಪಕ್ಷ 0xc9: ಪಕ್ಷ 0xca: ಪಕ್ಷ 0xcb:
	    ಪಕ್ಷ 0x112: ಪಕ್ಷ 0x114: ಪಕ್ಷ 0x116: ಪಕ್ಷ 0x118: ಪಕ್ಷ 0x11a:
	    ಪಕ್ಷ 0x204: ಪಕ್ಷ 0x206: ಪಕ್ಷ 0x228: ಪಕ್ಷ 0x246: ಪಕ್ಷ 0x1e14:
	    ಪಕ್ಷ 0x1e16: ಪಕ್ಷ 0x1e18: ಪಕ್ಷ 0x1e1a: ಪಕ್ಷ 0x1e1c:
	    ಪಕ್ಷ 0x1eb8: ಪಕ್ಷ 0x1eba: ಪಕ್ಷ 0x1ebc: ಪಕ್ಷ 0x1ebe:
	    ಪಕ್ಷ 0x1ec0: ಪಕ್ಷ 0x1ec2: ಪಕ್ಷ 0x1ec4: ಪಕ್ಷ 0x1ec6:
		      regmbc('E'); regmbc(0xc8); regmbc(0xc9);
		      regmbc(0xca); regmbc(0xcb); regmbc(0x112);
		      regmbc(0x114); regmbc(0x116); regmbc(0x118);
		      regmbc(0x11a); regmbc(0x204); regmbc(0x206);
		      regmbc(0x228); regmbc(0x246); regmbc(0x1e14);
		      regmbc(0x1e16); regmbc(0x1e18); regmbc(0x1e1a);
		      regmbc(0x1e1c); regmbc(0x1eb8); regmbc(0x1eba);
		      regmbc(0x1ebc); regmbc(0x1ebe); regmbc(0x1ec0);
		      regmbc(0x1ec2); regmbc(0x1ec4); regmbc(0x1ec6);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'F': ಪಕ್ಷ 0x191: ಪಕ್ಷ 0x1e1e: ಪಕ್ಷ 0xa798:
		      regmbc('F'); regmbc(0x191); regmbc(0x1e1e);
		      regmbc(0xa798);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'G': ಪಕ್ಷ 0x11c: ಪಕ್ಷ 0x11e: ಪಕ್ಷ 0x120:
	    ಪಕ್ಷ 0x122: ಪಕ್ಷ 0x193: ಪಕ್ಷ 0x1e4: ಪಕ್ಷ 0x1e6:
	    ಪಕ್ಷ 0x1f4: ಪಕ್ಷ 0x1e20: ಪಕ್ಷ 0xa7a0:
		      regmbc('G'); regmbc(0x11c); regmbc(0x11e);
		      regmbc(0x120); regmbc(0x122); regmbc(0x193);
		      regmbc(0x1e4); regmbc(0x1e6); regmbc(0x1f4);
		      regmbc(0x1e20); regmbc(0xa7a0);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'H': ಪಕ್ಷ 0x124: ಪಕ್ಷ 0x126: ಪಕ್ಷ 0x21e:
	    ಪಕ್ಷ 0x1e22: ಪಕ್ಷ 0x1e24: ಪಕ್ಷ 0x1e26:
	    ಪಕ್ಷ 0x1e28: ಪಕ್ಷ 0x1e2a: ಪಕ್ಷ 0x2c67:
		      regmbc('H'); regmbc(0x124); regmbc(0x126);
		      regmbc(0x21e); regmbc(0x1e22); regmbc(0x1e24);
		      regmbc(0x1e26); regmbc(0x1e28); regmbc(0x1e2a);
		      regmbc(0x2c67);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'I': ಪಕ್ಷ 0xcc: ಪಕ್ಷ 0xcd: ಪಕ್ಷ 0xce: ಪಕ್ಷ 0xcf:
	    ಪಕ್ಷ 0x128: ಪಕ್ಷ 0x12a: ಪಕ್ಷ 0x12c: ಪಕ್ಷ 0x12e:
	    ಪಕ್ಷ 0x130: ಪಕ್ಷ 0x197: ಪಕ್ಷ 0x1cf: ಪಕ್ಷ 0x208:
	    ಪಕ್ಷ 0x20a: ಪಕ್ಷ 0x1e2c: ಪಕ್ಷ 0x1e2e: ಪಕ್ಷ 0x1ec8:
	    ಪಕ್ಷ 0x1eca:
		      regmbc('I'); regmbc(0xcc); regmbc(0xcd);
		      regmbc(0xce); regmbc(0xcf); regmbc(0x128);
		      regmbc(0x12a); regmbc(0x12c); regmbc(0x12e);
		      regmbc(0x130); regmbc(0x197); regmbc(0x1cf);
		      regmbc(0x208); regmbc(0x20a); regmbc(0x1e2c);
		      regmbc(0x1e2e); regmbc(0x1ec8); regmbc(0x1eca);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'J': ಪಕ್ಷ 0x134: ಪಕ್ಷ 0x248:
		      regmbc('J'); regmbc(0x134); regmbc(0x248);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'K': ಪಕ್ಷ 0x136: ಪಕ್ಷ 0x198: ಪಕ್ಷ 0x1e8: ಪಕ್ಷ 0x1e30:
	    ಪಕ್ಷ 0x1e32: ಪಕ್ಷ 0x1e34: ಪಕ್ಷ 0x2c69: ಪಕ್ಷ 0xa740:
		      regmbc('K'); regmbc(0x136); regmbc(0x198);
		      regmbc(0x1e8); regmbc(0x1e30); regmbc(0x1e32);
		      regmbc(0x1e34); regmbc(0x2c69); regmbc(0xa740);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'L': ಪಕ್ಷ 0x139: ಪಕ್ಷ 0x13b: ಪಕ್ಷ 0x13d: ಪಕ್ಷ 0x13f:
	    ಪಕ್ಷ 0x141: ಪಕ್ಷ 0x23d: ಪಕ್ಷ 0x1e36: ಪಕ್ಷ 0x1e38:
	    ಪಕ್ಷ 0x1e3a: ಪಕ್ಷ 0x1e3c: ಪಕ್ಷ 0x2c60:
		      regmbc('L'); regmbc(0x139); regmbc(0x13b);
		      regmbc(0x13d); regmbc(0x13f); regmbc(0x141);
		      regmbc(0x23d); regmbc(0x1e36); regmbc(0x1e38);
		      regmbc(0x1e3a); regmbc(0x1e3c); regmbc(0x2c60);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'M': ಪಕ್ಷ 0x1e3e: ಪಕ್ಷ 0x1e40: ಪಕ್ಷ 0x1e42:
		      regmbc('M'); regmbc(0x1e3e); regmbc(0x1e40);
		      regmbc(0x1e42);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'N': ಪಕ್ಷ 0xd1:
	    ಪಕ್ಷ 0x143: ಪಕ್ಷ 0x145: ಪಕ್ಷ 0x147: ಪಕ್ಷ 0x1f8:
	    ಪಕ್ಷ 0x1e44: ಪಕ್ಷ 0x1e46: ಪಕ್ಷ 0x1e48: ಪಕ್ಷ 0x1e4a:
	    ಪಕ್ಷ 0xa7a4:
		      regmbc('N'); regmbc(0xd1);
		      regmbc(0x143); regmbc(0x145); regmbc(0x147);
		      regmbc(0x1f8); regmbc(0x1e44); regmbc(0x1e46);
		      regmbc(0x1e48); regmbc(0x1e4a); regmbc(0xa7a4);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'O': ಪಕ್ಷ 0xd2: ಪಕ್ಷ 0xd3: ಪಕ್ಷ 0xd4: ಪಕ್ಷ 0xd5: ಪಕ್ಷ 0xd6:
	    ಪಕ್ಷ 0xd8: ಪಕ್ಷ 0x14c: ಪಕ್ಷ 0x14e: ಪಕ್ಷ 0x150: ಪಕ್ಷ 0x19f:
	    ಪಕ್ಷ 0x1a0: ಪಕ್ಷ 0x1d1: ಪಕ್ಷ 0x1ea: ಪಕ್ಷ 0x1ec: ಪಕ್ಷ 0x1fe:
	    ಪಕ್ಷ 0x20c: ಪಕ್ಷ 0x20e: ಪಕ್ಷ 0x22a: ಪಕ್ಷ 0x22c: ಪಕ್ಷ 0x22e:
	    ಪಕ್ಷ 0x230: ಪಕ್ಷ 0x1e4c: ಪಕ್ಷ 0x1e4e: ಪಕ್ಷ 0x1e50: ಪಕ್ಷ 0x1e52:
	    ಪಕ್ಷ 0x1ecc: ಪಕ್ಷ 0x1ece: ಪಕ್ಷ 0x1ed0: ಪಕ್ಷ 0x1ed2: ಪಕ್ಷ 0x1ed4:
	    ಪಕ್ಷ 0x1ed6: ಪಕ್ಷ 0x1ed8: ಪಕ್ಷ 0x1eda: ಪಕ್ಷ 0x1edc: ಪಕ್ಷ 0x1ede:
	    ಪಕ್ಷ 0x1ee0: ಪಕ್ಷ 0x1ee2:
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
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'P': ಪಕ್ಷ 0x1a4: ಪಕ್ಷ 0x1e54: ಪಕ್ಷ 0x1e56: ಪಕ್ಷ 0x2c63:
		      regmbc('P'); regmbc(0x1a4); regmbc(0x1e54);
		      regmbc(0x1e56); regmbc(0x2c63);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'Q': ಪಕ್ಷ 0x24a:
		      regmbc('Q'); regmbc(0x24a);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'R': ಪಕ್ಷ 0x154: ಪಕ್ಷ 0x156: ಪಕ್ಷ 0x158: ಪಕ್ಷ 0x210:
	    ಪಕ್ಷ 0x212: ಪಕ್ಷ 0x24c: ಪಕ್ಷ 0x1e58: ಪಕ್ಷ 0x1e5a:
	    ಪಕ್ಷ 0x1e5c: ಪಕ್ಷ 0x1e5e: ಪಕ್ಷ 0x2c64: ಪಕ್ಷ 0xa7a6:
		      regmbc('R'); regmbc(0x154); regmbc(0x156);
		      regmbc(0x210); regmbc(0x212); regmbc(0x158);
		      regmbc(0x24c); regmbc(0x1e58); regmbc(0x1e5a);
		      regmbc(0x1e5c); regmbc(0x1e5e); regmbc(0x2c64);
		      regmbc(0xa7a6);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'S': ಪಕ್ಷ 0x15a: ಪಕ್ಷ 0x15c: ಪಕ್ಷ 0x15e: ಪಕ್ಷ 0x160:
	    ಪಕ್ಷ 0x218: ಪಕ್ಷ 0x1e60: ಪಕ್ಷ 0x1e62: ಪಕ್ಷ 0x1e64:
	    ಪಕ್ಷ 0x1e66: ಪಕ್ಷ 0x1e68: ಪಕ್ಷ 0x2c7e: ಪಕ್ಷ 0xa7a8:
		      regmbc('S'); regmbc(0x15a); regmbc(0x15c);
		      regmbc(0x15e); regmbc(0x160); regmbc(0x218);
		      regmbc(0x1e60); regmbc(0x1e62); regmbc(0x1e64);
		      regmbc(0x1e66); regmbc(0x1e68); regmbc(0x2c7e);
			  regmbc(0xa7a8);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'T': ಪಕ್ಷ 0x162: ಪಕ್ಷ 0x164: ಪಕ್ಷ 0x166: ಪಕ್ಷ 0x1ac:
	    ಪಕ್ಷ 0x1ae: ಪಕ್ಷ 0x21a: ಪಕ್ಷ 0x23e: ಪಕ್ಷ 0x1e6a: ಪಕ್ಷ 0x1e6c:
	    ಪಕ್ಷ 0x1e6e: ಪಕ್ಷ 0x1e70:
		      regmbc('T'); regmbc(0x162); regmbc(0x164);
		      regmbc(0x166); regmbc(0x1ac); regmbc(0x23e);
		      regmbc(0x1ae); regmbc(0x21a); regmbc(0x1e6a);
		      regmbc(0x1e6c); regmbc(0x1e6e); regmbc(0x1e70);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'U': ಪಕ್ಷ 0xd9: ಪಕ್ಷ 0xda: ಪಕ್ಷ 0xdb: ಪಕ್ಷ 0xdc:
	    ಪಕ್ಷ 0x168: ಪಕ್ಷ 0x16a: ಪಕ್ಷ 0x16c: ಪಕ್ಷ 0x16e:
	    ಪಕ್ಷ 0x170: ಪಕ್ಷ 0x172: ಪಕ್ಷ 0x1af: ಪಕ್ಷ 0x1d3:
	    ಪಕ್ಷ 0x1d5: ಪಕ್ಷ 0x1d7: ಪಕ್ಷ 0x1d9: ಪಕ್ಷ 0x1db:
	    ಪಕ್ಷ 0x214: ಪಕ್ಷ 0x216: ಪಕ್ಷ 0x244: ಪಕ್ಷ 0x1e72:
	    ಪಕ್ಷ 0x1e74: ಪಕ್ಷ 0x1e76: ಪಕ್ಷ 0x1e78: ಪಕ್ಷ 0x1e7a:
	    ಪಕ್ಷ 0x1ee4: ಪಕ್ಷ 0x1ee6: ಪಕ್ಷ 0x1ee8: ಪಕ್ಷ 0x1eea:
	    ಪಕ್ಷ 0x1eec: ಪಕ್ಷ 0x1eee: ಪಕ್ಷ 0x1ef0:
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
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'V': ಪಕ್ಷ 0x1b2: ಪಕ್ಷ 0x1e7c: ಪಕ್ಷ 0x1e7e:
		      regmbc('V'); regmbc(0x1b2); regmbc(0x1e7c);
		      regmbc(0x1e7e);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'W': ಪಕ್ಷ 0x174: ಪಕ್ಷ 0x1e80: ಪಕ್ಷ 0x1e82:
	    ಪಕ್ಷ 0x1e84: ಪಕ್ಷ 0x1e86: ಪಕ್ಷ 0x1e88:
		      regmbc('W'); regmbc(0x174); regmbc(0x1e80);
		      regmbc(0x1e82); regmbc(0x1e84); regmbc(0x1e86);
		      regmbc(0x1e88);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'X': ಪಕ್ಷ 0x1e8a: ಪಕ್ಷ 0x1e8c:
		      regmbc('X'); regmbc(0x1e8a); regmbc(0x1e8c);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'Y': ಪಕ್ಷ 0xdd:
	    ಪಕ್ಷ 0x176: ಪಕ್ಷ 0x178: ಪಕ್ಷ 0x1b3: ಪಕ್ಷ 0x232: ಪಕ್ಷ 0x24e:
	    ಪಕ್ಷ 0x1e8e: ಪಕ್ಷ 0x1ef2: ಪಕ್ಷ 0x1ef6: ಪಕ್ಷ 0x1ef4: ಪಕ್ಷ 0x1ef8:
		      regmbc('Y'); regmbc(0xdd); regmbc(0x176);
		      regmbc(0x178); regmbc(0x1b3); regmbc(0x232);
		      regmbc(0x24e); regmbc(0x1e8e); regmbc(0x1ef2);
		      regmbc(0x1ef4); regmbc(0x1ef6); regmbc(0x1ef8);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'Z': ಪಕ್ಷ 0x179: ಪಕ್ಷ 0x17b: ಪಕ್ಷ 0x17d: ಪಕ್ಷ 0x1b5:
	    ಪಕ್ಷ 0x1e90: ಪಕ್ಷ 0x1e92: ಪಕ್ಷ 0x1e94: ಪಕ್ಷ 0x2c6b:
		      regmbc('Z'); regmbc(0x179); regmbc(0x17b);
		      regmbc(0x17d); regmbc(0x1b5); regmbc(0x1e90);
		      regmbc(0x1e92); regmbc(0x1e94); regmbc(0x2c6b);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'a': ಪಕ್ಷ 0xe0: ಪಕ್ಷ 0xe1: ಪಕ್ಷ 0xe2:
	    ಪಕ್ಷ 0xe3: ಪಕ್ಷ 0xe4: ಪಕ್ಷ 0xe5: ಪಕ್ಷ 0x101: ಪಕ್ಷ 0x103:
	    ಪಕ್ಷ 0x105: ಪಕ್ಷ 0x1ce: ಪಕ್ಷ 0x1df: ಪಕ್ಷ 0x1e1: ಪಕ್ಷ 0x1fb:
	    ಪಕ್ಷ 0x201: ಪಕ್ಷ 0x203: ಪಕ್ಷ 0x227: ಪಕ್ಷ 0x1d8f: ಪಕ್ಷ 0x1e01:
	    ಪಕ್ಷ 0x1e9a: ಪಕ್ಷ 0x1ea1: ಪಕ್ಷ 0x1ea3: ಪಕ್ಷ 0x1ea5:
	    ಪಕ್ಷ 0x1ea7: ಪಕ್ಷ 0x1ea9: ಪಕ್ಷ 0x1eab: ಪಕ್ಷ 0x1ead:
	    ಪಕ್ಷ 0x1eaf: ಪಕ್ಷ 0x1eb1: ಪಕ್ಷ 0x1eb3: ಪಕ್ಷ 0x1eb5:
	    ಪಕ್ಷ 0x1eb7: ಪಕ್ಷ 0x2c65:
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
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'b': ಪಕ್ಷ 0x180: ಪಕ್ಷ 0x253: ಪಕ್ಷ 0x1d6c: ಪಕ್ಷ 0x1d80:
	    ಪಕ್ಷ 0x1e03: ಪಕ್ಷ 0x1e05: ಪಕ್ಷ 0x1e07:
		      regmbc('b');
		      regmbc(0x180); regmbc(0x253); regmbc(0x1d6c);
		      regmbc(0x1d80); regmbc(0x1e03); regmbc(0x1e05);
		      regmbc(0x1e07);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'c': ಪಕ್ಷ 0xe7:
	    ಪಕ್ಷ 0x107: ಪಕ್ಷ 0x109: ಪಕ್ಷ 0x10b: ಪಕ್ಷ 0x10d: ಪಕ್ಷ 0x188:
	    ಪಕ್ಷ 0x23c: ಪಕ್ಷ 0x1e09: ಪಕ್ಷ 0xa793: ಪಕ್ಷ 0xa794:
		      regmbc('c'); regmbc(0xe7); regmbc(0x107);
		      regmbc(0x109); regmbc(0x10b); regmbc(0x10d);
		      regmbc(0x188); regmbc(0x23c); regmbc(0x1e09);
		      regmbc(0xa793); regmbc(0xa794);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'd': ಪಕ್ಷ 0x10f: ಪಕ್ಷ 0x111: ಪಕ್ಷ 0x257: ಪಕ್ಷ 0x1d6d:
	    ಪಕ್ಷ 0x1d81: ಪಕ್ಷ 0x1d91: ಪಕ್ಷ 0x1e0b: ಪಕ್ಷ 0x1e0d:
	    ಪಕ್ಷ 0x1e0f: ಪಕ್ಷ 0x1e11: ಪಕ್ಷ 0x1e13:
		      regmbc('d'); regmbc(0x10f); regmbc(0x111);
		      regmbc(0x257); regmbc(0x1d6d); regmbc(0x1d81);
		      regmbc(0x1d91); regmbc(0x1e0b); regmbc(0x1e0d);
		      regmbc(0x1e0f); regmbc(0x1e11); regmbc(0x1e13);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'e': ಪಕ್ಷ 0xe8: ಪಕ್ಷ 0xe9: ಪಕ್ಷ 0xea: ಪಕ್ಷ 0xeb:
	    ಪಕ್ಷ 0x113: ಪಕ್ಷ 0x115: ಪಕ್ಷ 0x117: ಪಕ್ಷ 0x119:
	    ಪಕ್ಷ 0x11b: ಪಕ್ಷ 0x205: ಪಕ್ಷ 0x207: ಪಕ್ಷ 0x229:
	    ಪಕ್ಷ 0x247: ಪಕ್ಷ 0x1d92: ಪಕ್ಷ 0x1e15: ಪಕ್ಷ 0x1e17:
	    ಪಕ್ಷ 0x1e19: ಪಕ್ಷ 0x1e1b: ಪಕ್ಷ 0x1eb9: ಪಕ್ಷ 0x1ebb:
	    ಪಕ್ಷ 0x1e1d: ಪಕ್ಷ 0x1ebd: ಪಕ್ಷ 0x1ebf: ಪಕ್ಷ 0x1ec1:
	    ಪಕ್ಷ 0x1ec3: ಪಕ್ಷ 0x1ec5: ಪಕ್ಷ 0x1ec7:
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
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'f': ಪಕ್ಷ 0x192: ಪಕ್ಷ 0x1d6e: ಪಕ್ಷ 0x1d82:
	    ಪಕ್ಷ 0x1e1f: ಪಕ್ಷ 0xa799:
		     regmbc('f'); regmbc(0x192); regmbc(0x1d6e);
		     regmbc(0x1d82); regmbc(0x1e1f); regmbc(0xa799);
		     ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'g': ಪಕ್ಷ 0x11d: ಪಕ್ಷ 0x11f: ಪಕ್ಷ 0x121: ಪಕ್ಷ 0x123:
	    ಪಕ್ಷ 0x1e5: ಪಕ್ಷ 0x1e7: ಪಕ್ಷ 0x260: ಪಕ್ಷ 0x1f5: ಪಕ್ಷ 0x1d83:
	    ಪಕ್ಷ 0x1e21: ಪಕ್ಷ 0xa7a1:
		      regmbc('g'); regmbc(0x11d); regmbc(0x11f);
		      regmbc(0x121); regmbc(0x123); regmbc(0x1e5);
		      regmbc(0x1e7); regmbc(0x1f5); regmbc(0x260);
		      regmbc(0x1d83); regmbc(0x1e21); regmbc(0xa7a1);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'h': ಪಕ್ಷ 0x125: ಪಕ್ಷ 0x127: ಪಕ್ಷ 0x21f: ಪಕ್ಷ 0x1e23:
	    ಪಕ್ಷ 0x1e25: ಪಕ್ಷ 0x1e27: ಪಕ್ಷ 0x1e29: ಪಕ್ಷ 0x1e2b:
	    ಪಕ್ಷ 0x1e96: ಪಕ್ಷ 0x2c68: ಪಕ್ಷ 0xa795:
		      regmbc('h'); regmbc(0x125); regmbc(0x127);
		      regmbc(0x21f); regmbc(0x1e23); regmbc(0x1e25);
		      regmbc(0x1e27); regmbc(0x1e29); regmbc(0x1e2b);
		      regmbc(0x1e96); regmbc(0x2c68); regmbc(0xa795);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'i': ಪಕ್ಷ 0xec: ಪಕ್ಷ 0xed: ಪಕ್ಷ 0xee: ಪಕ್ಷ 0xef:
	    ಪಕ್ಷ 0x129: ಪಕ್ಷ 0x12b: ಪಕ್ಷ 0x12d: ಪಕ್ಷ 0x12f:
	    ಪಕ್ಷ 0x1d0: ಪಕ್ಷ 0x209: ಪಕ್ಷ 0x20b: ಪಕ್ಷ 0x268:
	    ಪಕ್ಷ 0x1d96: ಪಕ್ಷ 0x1e2d: ಪಕ್ಷ 0x1e2f: ಪಕ್ಷ 0x1ec9:
	    ಪಕ್ಷ 0x1ecb:
		      regmbc('i'); regmbc(0xec); regmbc(0xed);
		      regmbc(0xee); regmbc(0xef); regmbc(0x129);
		      regmbc(0x12b); regmbc(0x12d); regmbc(0x12f);
		      regmbc(0x1d0); regmbc(0x209); regmbc(0x20b);
		      regmbc(0x268); regmbc(0x1d96); regmbc(0x1e2d);
		      regmbc(0x1e2f); regmbc(0x1ec9); regmbc(0x1ecb);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'j': ಪಕ್ಷ 0x135: ಪಕ್ಷ 0x1f0: ಪಕ್ಷ 0x249:
		      regmbc('j'); regmbc(0x135); regmbc(0x1f0);
		      regmbc(0x249);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'k': ಪಕ್ಷ 0x137: ಪಕ್ಷ 0x199: ಪಕ್ಷ 0x1e9:
	    ಪಕ್ಷ 0x1d84: ಪಕ್ಷ 0x1e31: ಪಕ್ಷ 0x1e33: ಪಕ್ಷ 0x1e35:
	    ಪಕ್ಷ 0x2c6a: ಪಕ್ಷ 0xa741:
		      regmbc('k'); regmbc(0x137); regmbc(0x199);
		      regmbc(0x1e9); regmbc(0x1d84); regmbc(0x1e31);
		      regmbc(0x1e33); regmbc(0x1e35); regmbc(0x2c6a);
		      regmbc(0xa741);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'l': ಪಕ್ಷ 0x13a: ಪಕ್ಷ 0x13c: ಪಕ್ಷ 0x13e:
	    ಪಕ್ಷ 0x140: ಪಕ್ಷ 0x142: ಪಕ್ಷ 0x19a: ಪಕ್ಷ 0x1e37:
	    ಪಕ್ಷ 0x1e39: ಪಕ್ಷ 0x1e3b: ಪಕ್ಷ 0x1e3d: ಪಕ್ಷ 0x2c61:
		      regmbc('l'); regmbc(0x13a); regmbc(0x13c);
		      regmbc(0x13e); regmbc(0x140); regmbc(0x142);
		      regmbc(0x19a); regmbc(0x1e37); regmbc(0x1e39);
		      regmbc(0x1e3b); regmbc(0x1e3d); regmbc(0x2c61);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'm': ಪಕ್ಷ 0x1d6f: ಪಕ್ಷ 0x1e3f: ಪಕ್ಷ 0x1e41: ಪಕ್ಷ 0x1e43:
		      regmbc('m'); regmbc(0x1d6f); regmbc(0x1e3f);
		      regmbc(0x1e41); regmbc(0x1e43);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'n': ಪಕ್ಷ 0xf1: ಪಕ್ಷ 0x144: ಪಕ್ಷ 0x146: ಪಕ್ಷ 0x148:
	    ಪಕ್ಷ 0x149: ಪಕ್ಷ 0x1f9: ಪಕ್ಷ 0x1d70: ಪಕ್ಷ 0x1d87:
	    ಪಕ್ಷ 0x1e45: ಪಕ್ಷ 0x1e47: ಪಕ್ಷ 0x1e49: ಪಕ್ಷ 0x1e4b:
	    ಪಕ್ಷ 0xa7a5:
		      regmbc('n'); regmbc(0xf1); regmbc(0x144);
		      regmbc(0x146); regmbc(0x148); regmbc(0x149);
		      regmbc(0x1f9); regmbc(0x1d70); regmbc(0x1d87);
		      regmbc(0x1e45); regmbc(0x1e47); regmbc(0x1e49);
		      regmbc(0x1e4b); regmbc(0xa7a5);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'o': ಪಕ್ಷ 0xf2: ಪಕ್ಷ 0xf3: ಪಕ್ಷ 0xf4: ಪಕ್ಷ 0xf5:
	    ಪಕ್ಷ 0xf6: ಪಕ್ಷ 0xf8: ಪಕ್ಷ 0x14d: ಪಕ್ಷ 0x14f: ಪಕ್ಷ 0x151:
	    ಪಕ್ಷ 0x1a1: ಪಕ್ಷ 0x1d2: ಪಕ್ಷ 0x1eb: ಪಕ್ಷ 0x1ed: ಪಕ್ಷ 0x1ff:
	    ಪಕ್ಷ 0x20d: ಪಕ್ಷ 0x20f: ಪಕ್ಷ 0x22b: ಪಕ್ಷ 0x22d: ಪಕ್ಷ 0x22f:
	    ಪಕ್ಷ 0x231: ಪಕ್ಷ 0x275: ಪಕ್ಷ 0x1e4d: ಪಕ್ಷ 0x1e4f:
	    ಪಕ್ಷ 0x1e51: ಪಕ್ಷ 0x1e53: ಪಕ್ಷ 0x1ecd: ಪಕ್ಷ 0x1ecf:
	    ಪಕ್ಷ 0x1ed1: ಪಕ್ಷ 0x1ed3: ಪಕ್ಷ 0x1ed5: ಪಕ್ಷ 0x1ed7:
	    ಪಕ್ಷ 0x1ed9: ಪಕ್ಷ 0x1edb: ಪಕ್ಷ 0x1edd: ಪಕ್ಷ 0x1edf:
	    ಪಕ್ಷ 0x1ee1: ಪಕ್ಷ 0x1ee3:
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
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'p': ಪಕ್ಷ 0x1a5: ಪಕ್ಷ 0x1d71: ಪಕ್ಷ 0x1d88: ಪಕ್ಷ 0x1d7d:
	    ಪಕ್ಷ 0x1e55: ಪಕ್ಷ 0x1e57:
		      regmbc('p'); regmbc(0x1a5); regmbc(0x1d71);
		      regmbc(0x1d7d); regmbc(0x1d88); regmbc(0x1e55);
		      regmbc(0x1e57);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'q': ಪಕ್ಷ 0x24b: ಪಕ್ಷ 0x2a0:
		      regmbc('q'); regmbc(0x24b); regmbc(0x2a0);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'r': ಪಕ್ಷ 0x155: ಪಕ್ಷ 0x157: ಪಕ್ಷ 0x159: ಪಕ್ಷ 0x211:
	    ಪಕ್ಷ 0x213: ಪಕ್ಷ 0x24d: ಪಕ್ಷ 0x27d: ಪಕ್ಷ 0x1d72: ಪಕ್ಷ 0x1d73:
	    ಪಕ್ಷ 0x1d89: ಪಕ್ಷ 0x1e59: ಪಕ್ಷ 0x1e5b: ಪಕ್ಷ 0x1e5d: ಪಕ್ಷ 0x1e5f:
	    ಪಕ್ಷ 0xa7a7:
		      regmbc('r'); regmbc(0x155); regmbc(0x157);
		      regmbc(0x159); regmbc(0x211); regmbc(0x213);
		      regmbc(0x24d); regmbc(0x1d72); regmbc(0x1d73);
		      regmbc(0x1d89); regmbc(0x1e59); regmbc(0x27d);
		      regmbc(0x1e5b); regmbc(0x1e5d); regmbc(0x1e5f);
		      regmbc(0xa7a7);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 's': ಪಕ್ಷ 0x15b: ಪಕ್ಷ 0x15d: ಪಕ್ಷ 0x15f: ಪಕ್ಷ 0x161:
	    ಪಕ್ಷ 0x1e61: ಪಕ್ಷ 0x219: ಪಕ್ಷ 0x23f: ಪಕ್ಷ 0x1d74: ಪಕ್ಷ 0x1d8a:
	    ಪಕ್ಷ 0x1e63: ಪಕ್ಷ 0x1e65: ಪಕ್ಷ 0x1e67: ಪಕ್ಷ 0x1e69: ಪಕ್ಷ 0xa7a9:
		      regmbc('s'); regmbc(0x15b); regmbc(0x15d);
		      regmbc(0x15f); regmbc(0x161); regmbc(0x23f);
		      regmbc(0x219); regmbc(0x1d74); regmbc(0x1d8a);
		      regmbc(0x1e61); regmbc(0x1e63); regmbc(0x1e65);
		      regmbc(0x1e67); regmbc(0x1e69); regmbc(0xa7a9);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 't': ಪಕ್ಷ 0x163: ಪಕ್ಷ 0x165: ಪಕ್ಷ 0x167: ಪಕ್ಷ 0x1ab:
	    ಪಕ್ಷ 0x1ad: ಪಕ್ಷ 0x21b: ಪಕ್ಷ 0x288: ಪಕ್ಷ 0x1d75: ಪಕ್ಷ 0x1e6b:
	    ಪಕ್ಷ 0x1e6d: ಪಕ್ಷ 0x1e6f: ಪಕ್ಷ 0x1e71: ಪಕ್ಷ 0x1e97: ಪಕ್ಷ 0x2c66:
		      regmbc('t'); regmbc(0x163); regmbc(0x165);
		      regmbc(0x167); regmbc(0x1ab); regmbc(0x21b);
		      regmbc(0x1ad); regmbc(0x288); regmbc(0x1d75);
		      regmbc(0x1e6b); regmbc(0x1e6d); regmbc(0x1e6f);
		      regmbc(0x1e71); regmbc(0x1e97); regmbc(0x2c66);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'u': ಪಕ್ಷ 0xf9: ಪಕ್ಷ 0xfa: ಪಕ್ಷ 0xfb: ಪಕ್ಷ 0xfc:
	    ಪಕ್ಷ 0x169: ಪಕ್ಷ 0x16b: ಪಕ್ಷ 0x16d: ಪಕ್ಷ 0x16f:
	    ಪಕ್ಷ 0x171: ಪಕ್ಷ 0x173: ಪಕ್ಷ 0x1b0: ಪಕ್ಷ 0x1d4:
	    ಪಕ್ಷ 0x1d6: ಪಕ್ಷ 0x1d8: ಪಕ್ಷ 0x1da: ಪಕ್ಷ 0x1dc:
	    ಪಕ್ಷ 0x215: ಪಕ್ಷ 0x217: ಪಕ್ಷ 0x289: ಪಕ್ಷ 0x1e73:
	    ಪಕ್ಷ 0x1d7e: ಪಕ್ಷ 0x1d99: ಪಕ್ಷ 0x1e75: ಪಕ್ಷ 0x1e77:
	    ಪಕ್ಷ 0x1e79: ಪಕ್ಷ 0x1e7b: ಪಕ್ಷ 0x1ee5: ಪಕ್ಷ 0x1ee7:
	    ಪಕ್ಷ 0x1ee9: ಪಕ್ಷ 0x1eeb: ಪಕ್ಷ 0x1eed: ಪಕ್ಷ 0x1eef:
	    ಪಕ್ಷ 0x1ef1:
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
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'v': ಪಕ್ಷ 0x28b: ಪಕ್ಷ 0x1d8c: ಪಕ್ಷ 0x1e7d: ಪಕ್ಷ 0x1e7f:
		      regmbc('v'); regmbc(0x28b); regmbc(0x1d8c);
		      regmbc(0x1e7d); regmbc(0x1e7f);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'w': ಪಕ್ಷ 0x175: ಪಕ್ಷ 0x1e81: ಪಕ್ಷ 0x1e83:
	    ಪಕ್ಷ 0x1e85: ಪಕ್ಷ 0x1e87: ಪಕ್ಷ 0x1e89: ಪಕ್ಷ 0x1e98:
		      regmbc('w'); regmbc(0x175); regmbc(0x1e81);
		      regmbc(0x1e83); regmbc(0x1e85); regmbc(0x1e87);
		      regmbc(0x1e89); regmbc(0x1e98);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'x': ಪಕ್ಷ 0x1e8b: ಪಕ್ಷ 0x1e8d:
		      regmbc('x'); regmbc(0x1e8b); regmbc(0x1e8d);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'y': ಪಕ್ಷ 0xfd: ಪಕ್ಷ 0xff: ಪಕ್ಷ 0x177: ಪಕ್ಷ 0x1b4:
	    ಪಕ್ಷ 0x233: ಪಕ್ಷ 0x24f: ಪಕ್ಷ 0x1e8f: ಪಕ್ಷ 0x1e99: ಪಕ್ಷ 0x1ef3:
	    ಪಕ್ಷ 0x1ef5: ಪಕ್ಷ 0x1ef7: ಪಕ್ಷ 0x1ef9:
		      regmbc('y'); regmbc(0xfd); regmbc(0xff);
		      regmbc(0x177); regmbc(0x1b4); regmbc(0x233);
		      regmbc(0x24f); regmbc(0x1e8f); regmbc(0x1e99);
		      regmbc(0x1ef3); regmbc(0x1ef5); regmbc(0x1ef7);
		      regmbc(0x1ef9);
		      ಪ್ರತ್ಯಾಗಮ;
	    ಪಕ್ಷ 'z': ಪಕ್ಷ 0x17a: ಪಕ್ಷ 0x17c: ಪಕ್ಷ 0x17e: ಪಕ್ಷ 0x1b6:
	    ಪಕ್ಷ 0x1d76: ಪಕ್ಷ 0x1d8e: ಪಕ್ಷ 0x1e91: ಪಕ್ಷ 0x1e93:
	    ಪಕ್ಷ 0x1e95: ಪಕ್ಷ 0x2c6c:
		      regmbc('z'); regmbc(0x17a); regmbc(0x17c);
		      regmbc(0x17e); regmbc(0x1b6); regmbc(0x1d76);
		      regmbc(0x1d8e); regmbc(0x1e91); regmbc(0x1e93);
		      regmbc(0x1e95); regmbc(0x2c6c);
		      ಪ್ರತ್ಯಾಗಮ;
	}
    }
    regmbc(c);
}

/*
 * Emit a node.
 * Return pointer to generated code.
 */
    ಸ್ಥಿರ char_u *
regnode(ಪೂರ್ಣಾಙ್ಕ op)
{
    char_u  *ret;

    ret = regcode;
    ಯದಿ (ret == JUST_CALC_SIZE)
	regsize += 3;
    ಅನ್ಯಥಾ
    {
	*regcode++ = op;
	*regcode++ = NUL;		// Null "next" pointer.
	*regcode++ = NUL;
    }
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Write a long as four bytes at "p" and return pointer to the next char.
 */
    ಸ್ಥಿರ char_u *
re_put_long(char_u *p, long_u val)
{
    *p++ = (char_u) ((val >> 24) & 0377);
    *p++ = (char_u) ((val >> 16) & 0377);
    *p++ = (char_u) ((val >> 8) & 0377);
    *p++ = (char_u) (val & 0377);
    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * regnext - dig the "next" pointer out of a node
 * Returns NULL when calculating size, when there is no next item and when
 * there is an error.
 */
    ಸ್ಥಿರ char_u *
regnext(char_u *p)
{
    ಪೂರ್ಣಾಙ್ಕ	    offset;

    ಯದಿ (p == JUST_CALC_SIZE || reg_toolong)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    offset = NEXT(p);
    ಯದಿ (offset == 0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (OP(p) == BACK)
	ಪ್ರತ್ಯಾಗಮ p - offset;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ p + offset;
}

/*
 * Set the next-pointer at the end of a node chain.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
regtail(char_u *p, char_u *val)
{
    char_u	*scan;
    char_u	*temp;
    ಪೂರ್ಣಾಙ್ಕ		offset;

    ಯದಿ (p == JUST_CALC_SIZE)
	ಪ್ರತ್ಯಾಗಮ;

    // Find last node.
    scan = p;
    ಪ್ರತಿ (;;)
    {
	temp = regnext(scan);
	ಯದಿ (temp == ರಿಕ್ತ)
	    ಭಙ್ಗ;
	scan = temp;
    }

    ಯದಿ (OP(scan) == BACK)
	offset = (ಪೂರ್ಣಾಙ್ಕ)(scan - val);
    ಅನ್ಯಥಾ
	offset = (ಪೂರ್ಣಾಙ್ಕ)(val - scan);
    // When the offset uses more than 16 bits it can no longer fit in the two
    // bytes available.  Use a global flag to avoid having to check return
    // values in too many places.
    ಯದಿ (offset > 0xffff)
	reg_toolong = TRUE;
    ಅನ್ಯಥಾ
    {
	*(scan + 1) = (char_u) (((unsigned)offset >> 8) & 0377);
	*(scan + 2) = (char_u) (offset & 0377);
    }
}

/*
 * Like regtail, on item after a BRANCH; nop if none.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
regoptail(char_u *p, char_u *val)
{
    // When op is neither BRANCH nor BRACE_COMPLEX0-9, it is "operandless"
    ಯದಿ (p == ರಿಕ್ತ || p == JUST_CALC_SIZE
	    || (OP(p) != BRANCH
		&& (OP(p) < BRACE_COMPLEX || OP(p) > BRACE_COMPLEX + 9)))
	ಪ್ರತ್ಯಾಗಮ;
    regtail(OPERAND(p), val);
}

/*
 * Insert an operator in front of already-emitted operand
 *
 * Means relocating the operand.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
reginsert(ಪೂರ್ಣಾಙ್ಕ op, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    ಯದಿ (regcode == JUST_CALC_SIZE)
    {
	regsize += 3;
	ಪ್ರತ್ಯಾಗಮ;
    }
    src = regcode;
    regcode += 3;
    dst = regcode;
    ಯಾವತ್ (src > opnd)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
reginsert_nr(ಪೂರ್ಣಾಙ್ಕ op, long val, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    ಯದಿ (regcode == JUST_CALC_SIZE)
    {
	regsize += 7;
	ಪ್ರತ್ಯಾಗಮ;
    }
    src = regcode;
    regcode += 7;
    dst = regcode;
    ಯಾವತ್ (src > opnd)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
reginsert_limits(
    ಪೂರ್ಣಾಙ್ಕ		op,
    long	minval,
    long	maxval,
    char_u	*opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    ಯದಿ (regcode == JUST_CALC_SIZE)
    {
	regsize += 11;
	ಪ್ರತ್ಯಾಗಮ;
    }
    src = regcode;
    regcode += 11;
    dst = regcode;
    ಯಾವತ್ (src > opnd)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
seen_endbrace(ಪೂರ್ಣಾಙ್ಕ refnum)
{
    ಯದಿ (!had_endbrace[refnum])
    {
	char_u *p;

	// Trick: check if "@<=" or "@<!" follows, in which case
	// the \1 can appear before the referenced match.
	ಪ್ರತಿ (p = regparse; *p != NUL; ++p)
	    ಯದಿ (p[0] == '@' && p[1] == '<' && (p[2] == '!' || p[2] == '='))
		ಭಙ್ಗ;
	ಯದಿ (*p == NUL)
	{
	    emsg(_(e_illegal_back_reference));
	    rc_did_emsg = TRUE;
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Parse the lowest level.
 *
 * Optimization:  gobbles an entire sequence of ordinary characters so that
 * it can turn them into a single node, which is smaller to store and
 * faster to run.  Don't do this when one_exactly is set.
 */
    ಸ್ಥಿರ char_u *
regatom(ಪೂರ್ಣಾಙ್ಕ *flagp)
{
    char_u	    *ret;
    ಪೂರ್ಣಾಙ್ಕ		    flags;
    ಪೂರ್ಣಾಙ್ಕ		    c;
    char_u	    *p;
    ಪೂರ್ಣಾಙ್ಕ		    extra = 0;
    ಪೂರ್ಣಾಙ್ಕ		    save_prev_at_start = prev_at_start;

    *flagp = WORST;		// Tentatively.

    c = getchr();
    ವಿಕಲ್ಪ (c)
    {
      ಪಕ್ಷ Magic('^'):
	ret = regnode(BOL);
	ಭಙ್ಗ;

      ಪಕ್ಷ Magic('$'):
	ret = regnode(EOL);
#ಯದಿ defined(FEAT_SYN_HL)
	had_eol = TRUE;
#endif
	ಭಙ್ಗ;

      ಪಕ್ಷ Magic('<'):
	ret = regnode(BOW);
	ಭಙ್ಗ;

      ಪಕ್ಷ Magic('>'):
	ret = regnode(EOW);
	ಭಙ್ಗ;

      ಪಕ್ಷ Magic('_'):
	c = no_Magic(getchr());
	ಯದಿ (c == '^')		// "\_^" is start-of-line
	{
	    ret = regnode(BOL);
	    ಭಙ್ಗ;
	}
	ಯದಿ (c == '$')		// "\_$" is end-of-line
	{
	    ret = regnode(EOL);
#ಯದಿ defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#endif
	    ಭಙ್ಗ;
	}

	extra = ADD_NL;
	*flagp |= HASNL;

	// "\_[" is character range plus newline
	ಯದಿ (c == '[')
	    ಗಚ್ಛ collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	// Character classes.
      ಪಕ್ಷ Magic('.'):
      ಪಕ್ಷ Magic('i'):
      ಪಕ್ಷ Magic('I'):
      ಪಕ್ಷ Magic('k'):
      ಪಕ್ಷ Magic('K'):
      ಪಕ್ಷ Magic('f'):
      ಪಕ್ಷ Magic('F'):
      ಪಕ್ಷ Magic('p'):
      ಪಕ್ಷ Magic('P'):
      ಪಕ್ಷ Magic('s'):
      ಪಕ್ಷ Magic('S'):
      ಪಕ್ಷ Magic('d'):
      ಪಕ್ಷ Magic('D'):
      ಪಕ್ಷ Magic('x'):
      ಪಕ್ಷ Magic('X'):
      ಪಕ್ಷ Magic('o'):
      ಪಕ್ಷ Magic('O'):
      ಪಕ್ಷ Magic('w'):
      ಪಕ್ಷ Magic('W'):
      ಪಕ್ಷ Magic('h'):
      ಪಕ್ಷ Magic('H'):
      ಪಕ್ಷ Magic('a'):
      ಪಕ್ಷ Magic('A'):
      ಪಕ್ಷ Magic('l'):
      ಪಕ್ಷ Magic('L'):
      ಪಕ್ಷ Magic('u'):
      ಪಕ್ಷ Magic('U'):
	p = vim_strchr(classchars, no_Magic(c));
	ಯದಿ (p == ರಿಕ್ತ)
	    EMSG_RET_NULL(_(e_invalid_use_of_underscore));

	// When '.' is followed by a composing char ignore the dot, so that
	// the composing char is matched here.
	ಯದಿ (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	{
	    c = getchr();
	    ಗಚ್ಛ do_multibyte;
	}
	ret = regnode(classcodes[p - classchars] + extra);
	*flagp |= HASWIDTH | SIMPLE;
	ಭಙ್ಗ;

      ಪಕ್ಷ Magic('n'):
	ಯದಿ (reg_string)
	{
	    // In a string "\n" matches a newline character.
	    ret = regnode(EXACTLY);
	    regc(NL);
	    regc(NUL);
	    *flagp |= HASWIDTH | SIMPLE;
	}
	ಅನ್ಯಥಾ
	{
	    // In buffer text "\n" matches the end of a line.
	    ret = regnode(NEWL);
	    *flagp |= HASWIDTH | HASNL;
	}
	ಭಙ್ಗ;

      ಪಕ್ಷ Magic('('):
	ಯದಿ (one_exactly)
	    EMSG_ONE_RET_NULL;
	ret = reg(REG_PAREN, &flags);
	ಯದಿ (ret == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	*flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
	ಭಙ್ಗ;

      ಪಕ್ಷ NUL:
      ಪಕ್ಷ Magic('|'):
      ಪಕ್ಷ Magic('&'):
      ಪಕ್ಷ Magic(')'):
	ಯದಿ (one_exactly)
	    EMSG_ONE_RET_NULL;
	// Supposed to be caught earlier.
	IEMSG_RET_NULL(e_internal_error_in_regexp);
	// NOTREACHED

      ಪಕ್ಷ Magic('='):
      ಪಕ್ಷ Magic('?'):
      ಪಕ್ಷ Magic('+'):
      ಪಕ್ಷ Magic('@'):
      ಪಕ್ಷ Magic('{'):
      ಪಕ್ಷ Magic('*'):
	c = no_Magic(c);
	EMSG3_RET_NULL(_(e_str_chr_follows_nothing),
		(c == '*' ? reg_magic >= MAGIC_ON : reg_magic == MAGIC_ALL), c);
	// NOTREACHED

      ಪಕ್ಷ Magic('~'):		// previous substitute pattern
	    ಯದಿ (reg_prev_sub != ರಿಕ್ತ)
	    {
		char_u	    *lp;

		ret = regnode(EXACTLY);
		lp = reg_prev_sub;
		ಯಾವತ್ (*lp != NUL)
		    regc(*lp++);
		regc(NUL);
		ಯದಿ (*reg_prev_sub != NUL)
		{
		    *flagp |= HASWIDTH;
		    ಯದಿ ((lp - reg_prev_sub) == 1)
			*flagp |= SIMPLE;
		}
	    }
	    ಅನ್ಯಥಾ
		EMSG_RET_NULL(_(e_no_previous_substitute_regular_expression));
	    ಭಙ್ಗ;

      ಪಕ್ಷ Magic('1'):
      ಪಕ್ಷ Magic('2'):
      ಪಕ್ಷ Magic('3'):
      ಪಕ್ಷ Magic('4'):
      ಪಕ್ಷ Magic('5'):
      ಪಕ್ಷ Magic('6'):
      ಪಕ್ಷ Magic('7'):
      ಪಕ್ಷ Magic('8'):
      ಪಕ್ಷ Magic('9'):
	    {
		ಪೂರ್ಣಾಙ್ಕ		    refnum;

		refnum = c - Magic('0');
		ಯದಿ (!seen_endbrace(refnum))
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		ret = regnode(BACKREF + refnum);
	    }
	    ಭಙ್ಗ;

      ಪಕ್ಷ Magic('z'):
	{
	    c = no_Magic(getchr());
	    ವಿಕಲ್ಪ (c)
	    {
#ifdef FEAT_SYN_HL
		ಪಕ್ಷ '(': ಯದಿ ((reg_do_extmatch & REX_SET) == 0)
			      EMSG_RET_NULL(_(e_z_not_allowed_here));
			  ಯದಿ (one_exactly)
			      EMSG_ONE_RET_NULL;
			  ret = reg(REG_ZPAREN, &flags);
			  ಯದಿ (ret == ರಿಕ್ತ)
			      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
			  *flagp |= flags & (HASWIDTH|SPSTART|HASNL|HASLOOKBH);
			  re_has_z = REX_SET;
			  ಭಙ್ಗ;

		ಪಕ್ಷ '1':
		ಪಕ್ಷ '2':
		ಪಕ್ಷ '3':
		ಪಕ್ಷ '4':
		ಪಕ್ಷ '5':
		ಪಕ್ಷ '6':
		ಪಕ್ಷ '7':
		ಪಕ್ಷ '8':
		ಪಕ್ಷ '9': ಯದಿ ((reg_do_extmatch & REX_USE) == 0)
			      EMSG_RET_NULL(_(e_z1_z9_not_allowed_here));
			  ret = regnode(ZREF + c - '0');
			  re_has_z = REX_USE;
			  ಭಙ್ಗ;
#endif

		ಪಕ್ಷ 's': ret = regnode(MOPEN + 0);
			  ಯದಿ (re_mult_next("\\zs") == FAIL)
			      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
			  ಭಙ್ಗ;

		ಪಕ್ಷ 'e': ret = regnode(MCLOSE + 0);
			  ಯದಿ (re_mult_next("\\ze") == FAIL)
			      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
			  ಭಙ್ಗ;

		ಸ್ವಭಾವ:  EMSG_RET_NULL(_(e_invalid_character_after_bsl_z));
	    }
	}
	ಭಙ್ಗ;

      ಪಕ್ಷ Magic('%'):
	{
	    c = no_Magic(getchr());
	    ವಿಕಲ್ಪ (c)
	    {
		// () without a back reference
		ಪಕ್ಷ '(':
		    ಯದಿ (one_exactly)
			EMSG_ONE_RET_NULL;
		    ret = reg(REG_NPAREN, &flags);
		    ಯದಿ (ret == ರಿಕ್ತ)
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		    *flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
		    ಭಙ್ಗ;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		ಪಕ್ಷ '^':
		    ret = regnode(RE_BOF);
		    ಭಙ್ಗ;

		ಪಕ್ಷ '$':
		    ret = regnode(RE_EOF);
		    ಭಙ್ಗ;

		ಪಕ್ಷ '#':
		    ಯದಿ (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_start_of_pattern),
								  regparse[1]);
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    }
		    ret = regnode(CURSOR);
		    ಭಙ್ಗ;

		ಪಕ್ಷ 'V':
		    ret = regnode(RE_VISUAL);
		    ಭಙ್ಗ;

		ಪಕ್ಷ 'C':
		    ret = regnode(RE_COMPOSING);
		    ಭಙ್ಗ;

		// \%[abc]: Emit as a list of branches, all ending at the last
		// branch which matches nothing.
		ಪಕ್ಷ '[':
			  ಯದಿ (one_exactly)	// doesn't nest
			      EMSG_ONE_RET_NULL;
			  {
			      char_u	*lastbranch;
			      char_u	*lastnode = ರಿಕ್ತ;
			      char_u	*br;

			      ret = ರಿಕ್ತ;
			      ಯಾವತ್ ((c = getchr()) != ']')
			      {
				  ಯದಿ (c == NUL)
				      EMSG2_RET_NULL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
				  br = regnode(BRANCH);
				  ಯದಿ (ret == ರಿಕ್ತ)
				      ret = br;
				  ಅನ್ಯಥಾ
				  {
				      regtail(lastnode, br);
				      ಯದಿ (reg_toolong)
					  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
				  }

				  ungetchr();
				  one_exactly = TRUE;
				  lastnode = regatom(flagp);
				  one_exactly = FALSE;
				  ಯದಿ (lastnode == ರಿಕ್ತ)
				      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
			      }
			      ಯದಿ (ret == ರಿಕ್ತ)
				  EMSG2_RET_NULL(_(e_empty_str_brackets),
						      reg_magic == MAGIC_ALL);
			      lastbranch = regnode(BRANCH);
			      br = regnode(NOTHING);
			      ಯದಿ (ret != JUST_CALC_SIZE)
			      {
				  regtail(lastnode, br);
				  regtail(lastbranch, br);
				  // connect all branches to the NOTHING
				  // branch at the end
				  ಪ್ರತಿ (br = ret; br != lastnode; )
				  {
				      ಯದಿ (OP(br) == BRANCH)
				      {
					  regtail(br, lastbranch);
					  ಯದಿ (reg_toolong)
					      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
					  br = OPERAND(br);
				      }
				      ಅನ್ಯಥಾ
					  br = regnext(br);
				  }
			      }
			      *flagp &= ~(HASWIDTH | SIMPLE);
			      ಭಙ್ಗ;
			  }

		ಪಕ್ಷ 'd':   // %d123 decimal
		ಪಕ್ಷ 'o':   // %o123 octal
		ಪಕ್ಷ 'x':   // %xab hex 2
		ಪಕ್ಷ 'u':   // %uabcd hex 4
		ಪಕ್ಷ 'U':   // %U1234abcd hex 8
			  {
			      vimlong_T i;

			      ವಿಕಲ್ಪ (c)
			      {
				  ಪಕ್ಷ 'd': i = getdecchrs(); ಭಙ್ಗ;
				  ಪಕ್ಷ 'o': i = getoctchrs(); ಭಙ್ಗ;
				  ಪಕ್ಷ 'x': i = gethexchrs(2); ಭಙ್ಗ;
				  ಪಕ್ಷ 'u': i = gethexchrs(4); ಭಙ್ಗ;
				  ಪಕ್ಷ 'U': i = gethexchrs(8); ಭಙ್ಗ;
				  ಸ್ವಭಾವ:  i = -1; ಭಙ್ಗ;
			      }

			      ಯದಿ (i < 0 || i > INT_MAX)
				  EMSG2_RET_NULL(
					    _(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			      ಯದಿ (use_multibytecode(i))
				  ret = regnode(MULTIBYTECODE);
			      ಅನ್ಯಥಾ
				  ret = regnode(EXACTLY);
			      ಯದಿ (i == 0)
				  regc(0x0a);
			      ಅನ್ಯಥಾ
				  regmbc((ಪೂರ್ಣಾಙ್ಕ)i);
			      regc(NUL);
			      *flagp |= HASWIDTH;
			      ಭಙ್ಗ;
			  }

		ಸ್ವಭಾವ:
			  ಯದಿ (VIM_ISDIGIT(c) || c == '<' || c == '>'
						|| c == '\'' || c == '.')
			  {
			      long_u	n = 0;
			      ಪೂರ್ಣಾಙ್ಕ	cmp;
			      ಪೂರ್ಣಾಙ್ಕ	cur = FALSE;
			      ಪೂರ್ಣಾಙ್ಕ	got_digit = FALSE;

			      cmp = c;
			      ಯದಿ (cmp == '<' || cmp == '>')
				  c = getchr();
			      ಯದಿ (no_Magic(c) == '.')
			      {
				  cur = TRUE;
				  c = getchr();
			      }
			      ಯಾವತ್ (VIM_ISDIGIT(c))
			      {
				  got_digit = TRUE;
				  n = n * 10 + (c - '0');
				  c = getchr();
			      }
			      ಯದಿ (no_Magic(c) == '\'' && n == 0)
			      {
				  // "\%'m", "\%<'m" and "\%>'m": Mark
				  c = getchr();
				  ret = regnode(RE_MARK);
				  ಯದಿ (ret == JUST_CALC_SIZE)
				      regsize += 2;
				  ಅನ್ಯಥಾ
				  {
				      *regcode++ = c;
				      *regcode++ = cmp;
				  }
				  ಭಙ್ಗ;
			      }
			      ಅನ್ಯಥಾ ಯದಿ ((c == 'l' || c == 'c' || c == 'v')
					  && (cur || got_digit))
			      {
				  ಯದಿ (cur && n)
				  {
				    semsg(_(e_regexp_ಸಂಖ್ಯೆ_after_dot_pos_ಹುಡುಕು_chr),
								  no_Magic(c));
				    rc_did_emsg = TRUE;
				    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
				  }
				  ಯದಿ (c == 'l')
				  {
				      ಯದಿ (cur)
					  n = curwin->w_cursor.lnum;
				      ret = regnode(RE_LNUM);
				      ಯದಿ (save_prev_at_start)
					  at_start = TRUE;
				  }
				  ಅನ್ಯಥಾ ಯದಿ (c == 'c')
				  {
				      ಯದಿ (cur)
				      {
					  n = curwin->w_cursor.col;
					  n++;
				      }
				      ret = regnode(RE_COL);
				  }
				  ಅನ್ಯಥಾ
				  {
				      ಯದಿ (cur)
				      {
					  colnr_T vcol = 0;

					  getvvcol(curwin, &curwin->w_cursor,
							 ರಿಕ್ತ, ರಿಕ್ತ, &vcol, 0);
					  ++vcol;
					  n = vcol;
				      }
				      ret = regnode(RE_VCOL);
				  }
				  ಯದಿ (ret == JUST_CALC_SIZE)
				      regsize += 5;
				  ಅನ್ಯಥಾ
				  {
				      // put the number and the optional
				      // comparator after the opcode
				      regcode = re_put_long(regcode, n);
				      *regcode++ = cmp;
				  }
				  ಭಙ್ಗ;
			      }
			  }

			  EMSG2_RET_NULL(_(e_invalid_character_after_str),
						      reg_magic == MAGIC_ALL);
	    }
	}
	ಭಙ್ಗ;

      ಪಕ್ಷ Magic('['):
collection:
	{
	    char_u	*lp;

	    // If there is no matching ']', we assume the '[' is a normal
	    // character.  This makes 'incsearch' and ":help [" work.
	    lp = skip_anyof(regparse);
	    ಯದಿ (*lp == ']')	// there is a matching ']'
	    {
		ಪೂರ್ಣಾಙ್ಕ	startc = -1;	// > 0 when next '-' is a range
		ಪೂರ್ಣಾಙ್ಕ	endc;

		ಯದಿ (extra == 0)
		{
		    ಪೂರ್ಣಾಙ್ಕ	cl = bt_recognize_char_class(regparse, lp);

		    ಯದಿ (cl != 0)
		    {
			ret = regnode(cl);
			regparse = lp;
			skipchr();
			*flagp |= HASWIDTH | SIMPLE;
			ಭಙ್ಗ;
		    }
		}

		// In a character class, different parsing rules apply.
		// Not even \ is special anymore, nothing is.
		ಯದಿ (*regparse == '^')	    // Complement of range.
		{
		    ret = regnode(ANYBUT + extra);
		    regparse++;
		}
		ಅನ್ಯಥಾ
		    ret = regnode(ANYOF + extra);

		// At the start ']' and '-' mean the literal character.
		ಯದಿ (*regparse == ']' || *regparse == '-')
		{
		    startc = *regparse;
		    regc(*regparse++);
		}

		ಯಾವತ್ (*regparse != NUL && *regparse != ']')
		{
		    ಯದಿ (*regparse == '-')
		    {
			++regparse;
			// The '-' is not used for a range at the end and
			// after or before a '\n'.
			ಯದಿ (*regparse == ']' || *regparse == NUL
				|| startc == -1
				|| (regparse[0] == '\\' && regparse[1] == 'n'))
			{
			    regc('-');
			    startc = '-';	// [--x] is a range
			}
			ಅನ್ಯಥಾ
			{
			    // Also accept "a-[.z.]"
			    endc = 0;
			    ಯದಿ (*regparse == '[')
				endc = get_coll_element(&regparse);
			    ಯದಿ (endc == 0)
			    {
				ಯದಿ (has_mbyte)
				    endc = mb_ptr2char_adv(&regparse);
				ಅನ್ಯಥಾ
				    endc = *regparse++;
			    }

			    // Handle \o40, \x20 and \u20AC style sequences
			    ಯದಿ (endc == '\\' && !reg_cpo_lit && !reg_cpo_bsl)
				endc = coll_get_char();

			    ಯದಿ (startc > endc)
				EMSG_RET_NULL(_(e_reverse_range_in_character_class));
			    ಯದಿ (has_mbyte && ((*mb_char2len)(startc) > 1
						 || (*mb_char2len)(endc) > 1))
			    {
				// Limit to a range of 256 chars.
				ಯದಿ (endc > startc + 256)
				    EMSG_RET_NULL(_(e_range_too_large_in_character_class));
				ಯಾವತ್ (++startc <= endc)
				    regmbc(startc);
			    }
			    ಅನ್ಯಥಾ
			    {
				ಯಾವತ್ (++startc <= endc)
				    regc(startc);
			    }
			    startc = -1;
			}
		    }
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    ಅನ್ಯಥಾ ಯದಿ (*regparse == '\\'
			    && !reg_cpo_bsl
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != ರಿಕ್ತ
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR,
						       regparse[1]) != ರಿಕ್ತ)))
		    {
			regparse++;
			ಯದಿ (*regparse == 'n')
			{
			    // '\n' in range: also match NL
			    ಯದಿ (ret != JUST_CALC_SIZE)
			    {
				// Using \n inside [^] does not change what
				// matches. "[^\n]" is the same as ".".
				ಯದಿ (*ret == ANYOF)
				{
				    *ret = ANYOF + ADD_NL;
				    *flagp |= HASNL;
				}
				// else: must have had a \n already
			    }
			    regparse++;
			    startc = -1;
			}
			ಅನ್ಯಥಾ ಯದಿ (*regparse == 'd'
				|| *regparse == 'o'
				|| *regparse == 'x'
				|| *regparse == 'u'
				|| *regparse == 'U')
			{
			    startc = coll_get_char();
			    // max UTF-8 Codepoint is U+10FFFF,
			    // but allow values until INT_MAX
			    ಯದಿ (startc == INT_MAX)
				EMSG_RET_NULL(_(e_unicode_val_too_large));
			    ಯದಿ (startc == 0)
				regc(0x0a);
			    ಅನ್ಯಥಾ
				regmbc(startc);
			}
			ಅನ್ಯಥಾ
			{
			    startc = backslash_trans(*regparse++);
			    regc(startc);
			}
		    }
		    ಅನ್ಯಥಾ ಯದಿ (*regparse == '[')
		    {
			ಪೂರ್ಣಾಙ್ಕ c_class;
			ಪೂರ್ಣಾಙ್ಕ cu;

			c_class = get_char_class(&regparse);
			startc = -1;
			// Characters assumed to be 8 bits!
			ವಿಕಲ್ಪ (c_class)
			{
			    ಪಕ್ಷ CLASS_NONE:
				c_class = get_equi_class(&regparse);
				ಯದಿ (c_class != 0)
				{
				    // produce equivalence class
				    reg_equi_class(c_class);
				}
				ಅನ್ಯಥಾ ಯದಿ ((c_class =
					    get_coll_element(&regparse)) != 0)
				{
				    // produce a collating element
				    regmbc(c_class);
				}
				ಅನ್ಯಥಾ
				{
				    // literal '[', allow [[-x] as a range
				    startc = *regparse++;
				    regc(startc);
				}
				ಭಙ್ಗ;
			    ಪಕ್ಷ CLASS_ALNUM:
				ಪ್ರತಿ (cu = 1; cu < 128; cu++)
				    ಯದಿ (isalnum(cu))
					regmbc(cu);
				ಭಙ್ಗ;
			    ಪಕ್ಷ CLASS_ALPHA:
				ಪ್ರತಿ (cu = 1; cu < 128; cu++)
				    ಯದಿ (isalpha(cu))
					regmbc(cu);
				ಭಙ್ಗ;
			    ಪಕ್ಷ CLASS_BLANK:
				regc(' ');
				regc('\t');
				ಭಙ್ಗ;
			    ಪಕ್ಷ CLASS_CNTRL:
				ಪ್ರತಿ (cu = 1; cu <= 127; cu++)
				    ಯದಿ (iscntrl(cu))
					regmbc(cu);
				ಭಙ್ಗ;
			    ಪಕ್ಷ CLASS_DIGIT:
				ಪ್ರತಿ (cu = 1; cu <= 127; cu++)
				    ಯದಿ (VIM_ISDIGIT(cu))
					regmbc(cu);
				ಭಙ್ಗ;
			    ಪಕ್ಷ CLASS_GRAPH:
				ಪ್ರತಿ (cu = 1; cu <= 127; cu++)
				    ಯದಿ (isgraph(cu))
					regmbc(cu);
				ಭಙ್ಗ;
			    ಪಕ್ಷ CLASS_LOWER:
				ಪ್ರತಿ (cu = 1; cu <= 255; cu++)
				    ಯದಿ (MB_ISLOWER(cu) && cu != 170
								 && cu != 186)
					regmbc(cu);
				ಭಙ್ಗ;
			    ಪಕ್ಷ CLASS_PRINT:
				ಪ್ರತಿ (cu = 1; cu <= 255; cu++)
				    ಯದಿ (vim_isprintc(cu))
					regmbc(cu);
				ಭಙ್ಗ;
			    ಪಕ್ಷ CLASS_PUNCT:
				ಪ್ರತಿ (cu = 1; cu < 128; cu++)
				    ಯದಿ (ispunct(cu))
					regmbc(cu);
				ಭಙ್ಗ;
			    ಪಕ್ಷ CLASS_SPACE:
				ಪ್ರತಿ (cu = 9; cu <= 13; cu++)
				    regc(cu);
				regc(' ');
				ಭಙ್ಗ;
			    ಪಕ್ಷ CLASS_UPPER:
				ಪ್ರತಿ (cu = 1; cu <= 255; cu++)
				    ಯದಿ (MB_ISUPPER(cu))
					regmbc(cu);
				ಭಙ್ಗ;
			    ಪಕ್ಷ CLASS_XDIGIT:
				ಪ್ರತಿ (cu = 1; cu <= 255; cu++)
				    ಯದಿ (vim_isxdigit(cu))
					regmbc(cu);
				ಭಙ್ಗ;
			    ಪಕ್ಷ CLASS_TAB:
				regc('\t');
				ಭಙ್ಗ;
			    ಪಕ್ಷ CLASS_RETURN:
				regc('\r');
				ಭಙ್ಗ;
			    ಪಕ್ಷ CLASS_BACKSPACE:
				regc('\b');
				ಭಙ್ಗ;
			    ಪಕ್ಷ CLASS_ESCAPE:
				regc('\033');
				ಭಙ್ಗ;
			    ಪಕ್ಷ CLASS_IDENT:
				ಪ್ರತಿ (cu = 1; cu <= 255; cu++)
				    ಯದಿ (vim_isIDc(cu))
					regmbc(cu);
				ಭಙ್ಗ;
			    ಪಕ್ಷ CLASS_KEYWORD:
				ಪ್ರತಿ (cu = 1; cu <= 255; cu++)
				    ಯದಿ (reg_iswordc(cu))
					regmbc(cu);
				ಭಙ್ಗ;
			    ಪಕ್ಷ CLASS_FNAME:
				ಪ್ರತಿ (cu = 1; cu <= 255; cu++)
				    ಯದಿ (vim_isfilec(cu))
					regmbc(cu);
				ಭಙ್ಗ;
			}
		    }
		    ಅನ್ಯಥಾ
		    {
			ಯದಿ (has_mbyte)
			{
			    ಪೂರ್ಣಾಙ್ಕ	len;

			    // produce a multibyte character, including any
			    // following composing characters
			    startc = mb_ptr2char(regparse);
			    len = (*mb_ptr2len)(regparse);
			    ಯದಿ (enc_utf8 && utf_char2len(startc) != len)
				startc = -1;	// composing chars
			    ಯಾವತ್ (--len >= 0)
				regc(*regparse++);
			}
			ಅನ್ಯಥಾ
			{
			    startc = *regparse++;
			    regc(startc);
			}
		    }
		}
		regc(NUL);
		prevchr_len = 1;	// last char was the ']'
		ಯದಿ (*regparse != ']')
		    EMSG_RET_NULL(_(e_too_many_brackets));  // Cannot happen?
		skipchr();	    // let's be friends with the lexer again
		*flagp |= HASWIDTH | SIMPLE;
		ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (reg_strict)
		EMSG2_RET_NULL(_(e_missing_rsb_after_str_lsb),
							reg_magic > MAGIC_OFF);
	}
	// FALLTHROUGH

      ಸ್ವಭಾವ:
	{
	    ಪೂರ್ಣಾಙ್ಕ		len;

	    // A multi-byte character is handled as a separate atom if it's
	    // before a multi and when it's a composing char.
	    ಯದಿ (use_multibytecode(c))
	    {
do_multibyte:
		ret = regnode(MULTIBYTECODE);
		regmbc(c);
		*flagp |= HASWIDTH | SIMPLE;
		ಭಙ್ಗ;
	    }

	    ret = regnode(EXACTLY);

	    // Append characters as long as:
	    // - there is no following multi, we then need the character in
	    //   front of it as a single character operand
	    // - not running into a Magic character
	    // - "one_exactly" is not set
	    // But always emit at least one character.  Might be a Multi,
	    // e.g., a "[" without matching "]".
	    ಪ್ರತಿ (len = 0; c != NUL && (len == 0
			|| (re_multi_type(peekchr()) == NOT_MULTI
			    && !one_exactly
			    && !is_Magic(c))); ++len)
	    {
		c = no_Magic(c);
		ಯದಿ (has_mbyte)
		{
		    regmbc(c);
		    ಯದಿ (enc_utf8)
		    {
			ಪೂರ್ಣಾಙ್ಕ	l;

			// Need to get composing character too.
			ಪ್ರತಿ (;;)
			{
			    l = utf_ptr2len(regparse);
			    ಯದಿ (!UTF_COMPOSINGLIKE(regparse, regparse + l))
				ಭಙ್ಗ;
			    regmbc(utf_ptr2char(regparse));
			    skipchr();
			}
		    }
		}
		ಅನ್ಯಥಾ
		    regc(c);
		c = getchr();
	    }
	    ungetchr();

	    regc(NUL);
	    *flagp |= HASWIDTH;
	    ಯದಿ (len == 1)
		*flagp |= SIMPLE;
	}
	ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ ret;
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
    ಸ್ಥಿರ char_u *
regpiece(ಪೂರ್ಣಾಙ್ಕ *flagp)
{
    char_u	    *ret;
    ಪೂರ್ಣಾಙ್ಕ		    op;
    char_u	    *next;
    ಪೂರ್ಣಾಙ್ಕ		    flags;
    long	    minval;
    long	    maxval;

    ret = regatom(&flags);
    ಯದಿ (ret == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    op = peekchr();
    ಯದಿ (re_multi_type(op) == NOT_MULTI)
    {
	*flagp = flags;
	ಪ್ರತ್ಯಾಗಮ ret;
    }
    // default flags
    *flagp = (WORST | SPSTART | (flags & (HASNL | HASLOOKBH)));

    skipchr();
    ವಿಕಲ್ಪ (op)
    {
	ಪಕ್ಷ Magic('*'):
	    ಯದಿ (flags & SIMPLE)
		reginsert(STAR, ret);
	    ಅನ್ಯಥಾ
	    {
		// Emit x* as (x&|), where & means "self".
		reginsert(BRANCH, ret); // Either x
		regoptail(ret, regnode(BACK));	// and loop
		regoptail(ret, ret);	// back
		regtail(ret, regnode(BRANCH));	// or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ Magic('+'):
	    ಯದಿ (flags & SIMPLE)
		reginsert(PLUS, ret);
	    ಅನ್ಯಥಾ
	    {
		// Emit x+ as x(&|), where & means "self".
		next = regnode(BRANCH); // Either
		regtail(ret, next);
		regtail(regnode(BACK), ret);	// loop back
		regtail(next, regnode(BRANCH)); // or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    *flagp = (WORST | HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    ಭಙ್ಗ;

	ಪಕ್ಷ Magic('@'):
	    {
		ಪೂರ್ಣಾಙ್ಕ	lop = END;
		long	nr;

		nr = (long)getdecchrs();
		ವಿಕಲ್ಪ (no_Magic(getchr()))
		{
		    ಪಕ್ಷ '=': lop = MATCH; ಭಙ್ಗ;		  // \@=
		    ಪಕ್ಷ '!': lop = NOMATCH; ಭಙ್ಗ;		  // \@!
		    ಪಕ್ಷ '>': lop = SUBPAT; ಭಙ್ಗ;		  // \@>
		    ಪಕ್ಷ '<': ವಿಕಲ್ಪ (no_Magic(getchr()))
			      {
				  ಪಕ್ಷ '=': lop = BEHIND; ಭಙ್ಗ;   // \@<=
				  ಪಕ್ಷ '!': lop = NOBEHIND; ಭಙ್ಗ; // \@<!
			      }
		}
		ಯದಿ (lop == END)
		    EMSG2_RET_NULL(_(e_invalid_character_after_str_at),
						      reg_magic == MAGIC_ALL);
		// Look behind must match with behind_pos.
		ಯದಿ (lop == BEHIND || lop == NOBEHIND)
		{
		    regtail(ret, regnode(BHPOS));
		    *flagp |= HASLOOKBH;
		}
		regtail(ret, regnode(END)); // operand ends
		ಯದಿ (lop == BEHIND || lop == NOBEHIND)
		{
		    ಯದಿ (nr < 0)
			nr = 0; // no limit is same as zero limit
		    reginsert_nr(lop, nr, ret);
		}
		ಅನ್ಯಥಾ
		    reginsert(lop, ret);
		ಭಙ್ಗ;
	    }

	ಪಕ್ಷ Magic('?'):
	ಪಕ್ಷ Magic('='):
	    // Emit x= as (x|)
	    reginsert(BRANCH, ret);		// Either x
	    regtail(ret, regnode(BRANCH));	// or
	    next = regnode(NOTHING);		// null.
	    regtail(ret, next);
	    regoptail(ret, next);
	    ಭಙ್ಗ;

	ಪಕ್ಷ Magic('{'):
	    ಯದಿ (!read_limits(&minval, &maxval))
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    ಯದಿ (flags & SIMPLE)
	    {
		reginsert(BRACE_SIMPLE, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (ಅಂಕೆ_complex_braces >= 10)
		    EMSG2_RET_NULL(_(e_too_many_complex_str_curly),
						      reg_magic == MAGIC_ALL);
		reginsert(BRACE_COMPLEX + ಅಂಕೆ_complex_braces, ret);
		regoptail(ret, regnode(BACK));
		regoptail(ret, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
		++ಅಂಕೆ_complex_braces;
	    }
	    ಯದಿ (minval > 0 && maxval > 0)
		*flagp = (HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    ಭಙ್ಗ;
    }
    ಯದಿ (re_multi_type(peekchr()) != NOT_MULTI)
    {
	// Can't have a multi follow a multi.
	ಯದಿ (peekchr() == Magic('*'))
	    EMSG2_RET_NULL(_(e_nested_str), reg_magic >= MAGIC_ON);
	EMSG3_RET_NULL(_(e_nested_str_chr), reg_magic == MAGIC_ALL,
							  no_Magic(peekchr()));
    }

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Parse one alternative of an | or & operator.
 * Implements the concatenation operator.
 */
    ಸ್ಥಿರ char_u *
regconcat(ಪೂರ್ಣಾಙ್ಕ *flagp)
{
    char_u	*first = ರಿಕ್ತ;
    char_u	*chain = ರಿಕ್ತ;
    char_u	*latest;
    ಪೂರ್ಣಾಙ್ಕ		flags;
    ಪೂರ್ಣಾಙ್ಕ		cont = TRUE;

    *flagp = WORST;		// Tentatively.

    ಯಾವತ್ (cont)
    {
	ವಿಕಲ್ಪ (peekchr())
	{
	    ಪಕ್ಷ NUL:
	    ಪಕ್ಷ Magic('|'):
	    ಪಕ್ಷ Magic('&'):
	    ಪಕ್ಷ Magic(')'):
			    cont = FALSE;
			    ಭಙ್ಗ;
	    ಪಕ್ಷ Magic('Z'):
			    regflags |= RF_ICOMBINE;
			    skipchr_keepstart();
			    ಭಙ್ಗ;
	    ಪಕ್ಷ Magic('c'):
			    regflags |= RF_ICASE;
			    skipchr_keepstart();
			    ಭಙ್ಗ;
	    ಪಕ್ಷ Magic('C'):
			    regflags |= RF_NOICASE;
			    skipchr_keepstart();
			    ಭಙ್ಗ;
	    ಪಕ್ಷ Magic('v'):
			    reg_magic = MAGIC_ALL;
			    skipchr_keepstart();
			    curchr = -1;
			    ಭಙ್ಗ;
	    ಪಕ್ಷ Magic('m'):
			    reg_magic = MAGIC_ON;
			    skipchr_keepstart();
			    curchr = -1;
			    ಭಙ್ಗ;
	    ಪಕ್ಷ Magic('M'):
			    reg_magic = MAGIC_OFF;
			    skipchr_keepstart();
			    curchr = -1;
			    ಭಙ್ಗ;
	    ಪಕ್ಷ Magic('V'):
			    reg_magic = MAGIC_NONE;
			    skipchr_keepstart();
			    curchr = -1;
			    ಭಙ್ಗ;
	    ಸ್ವಭಾವ:
			    latest = regpiece(&flags);
			    ಯದಿ (latest == ರಿಕ್ತ || reg_toolong)
				ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
			    *flagp |= flags & (HASWIDTH | HASNL | HASLOOKBH);
			    ಯದಿ (chain == ರಿಕ್ತ)	// First piece.
				*flagp |= flags & SPSTART;
			    ಅನ್ಯಥಾ
				regtail(chain, latest);
			    chain = latest;
			    ಯದಿ (first == ರಿಕ್ತ)
				first = latest;
			    ಭಙ್ಗ;
	}
    }
    ಯದಿ (first == ರಿಕ್ತ)		// Loop ran zero times.
	first = regnode(NOTHING);
    ಪ್ರತ್ಯಾಗಮ first;
}

/*
 * Parse one alternative of an | operator.
 * Implements the & operator.
 */
    ಸ್ಥಿರ char_u *
regbranch(ಪೂರ್ಣಾಙ್ಕ *flagp)
{
    char_u	*ret;
    char_u	*chain = ರಿಕ್ತ;
    char_u	*latest;
    ಪೂರ್ಣಾಙ್ಕ		flags;

    *flagp = WORST | HASNL;		// Tentatively.

    ret = regnode(BRANCH);
    ಪ್ರತಿ (;;)
    {
	latest = regconcat(&flags);
	ಯದಿ (latest == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	// If one of the branches has width, the whole thing has.  If one of
	// the branches anchors at start-of-line, the whole thing does.
	// If one of the branches uses look-behind, the whole thing does.
	*flagp |= flags & (HASWIDTH | SPSTART | HASLOOKBH);
	// If one of the branches doesn't match a line-break, the whole thing
	// doesn't.
	*flagp &= ~HASNL | (flags & HASNL);
	ಯದಿ (chain != ರಿಕ್ತ)
	    regtail(chain, latest);
	ಯದಿ (peekchr() != Magic('&'))
	    ಭಙ್ಗ;
	skipchr();
	regtail(latest, regnode(END)); // operand ends
	ಯದಿ (reg_toolong)
	    ಭಙ್ಗ;
	reginsert(MATCH, latest);
	chain = latest;
    }

    ಪ್ರತ್ಯಾಗಮ ret;
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
    ಸ್ಥಿರ char_u *
reg(
    ಪೂರ್ಣಾಙ್ಕ		paren,	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
    ಪೂರ್ಣಾಙ್ಕ		*flagp)
{
    char_u	*ret;
    char_u	*br;
    char_u	*ender;
    ಪೂರ್ಣಾಙ್ಕ		parno = 0;
    ಪೂರ್ಣಾಙ್ಕ		flags;

    *flagp = HASWIDTH;		// Tentatively.

#ifdef FEAT_SYN_HL
    ಯದಿ (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	ಯದಿ (regnzpar >= NSUBEXP)
	    EMSG_RET_NULL(_(e_too_many_z));
	parno = regnzpar;
	regnzpar++;
	ret = regnode(ZOPEN + parno);
    }
    ಅನ್ಯಥಾ
#endif
	ಯದಿ (paren == REG_PAREN)
    {
	// Make a MOPEN node.
	ಯದಿ (regnpar >= NSUBEXP)
	    EMSG2_RET_NULL(_(e_too_many_str_open), reg_magic == MAGIC_ALL);
	parno = regnpar;
	++regnpar;
	ret = regnode(MOPEN + parno);
    }
    ಅನ್ಯಥಾ ಯದಿ (paren == REG_NPAREN)
    {
	// Make a NOPEN node.
	ret = regnode(NOPEN);
    }
    ಅನ್ಯಥಾ
	ret = ರಿಕ್ತ;

    ಯದಿ (bt_reg_parse_depth >= REG_MAX_PAREN_DEPTH)
	EMSG_RET_NULL(_(e_command_too_complex));
    ++bt_reg_parse_depth;

    // Pick up the branches, linking them together.
    br = regbranch(&flags);
    ಯದಿ (br == ರಿಕ್ತ)
    {
	ret = ರಿಕ್ತ;
	ಗಚ್ಛ theend;
    }
    ಯದಿ (ret != ರಿಕ್ತ)
	regtail(ret, br);	// [MZ]OPEN -> first.
    ಅನ್ಯಥಾ
	ret = br;
    // If one of the branches can be zero-width, the whole thing can.
    // If one of the branches has * at start or matches a line-break, the
    // whole thing can.
    ಯದಿ (!(flags & HASWIDTH))
	*flagp &= ~HASWIDTH;
    *flagp |= flags & (SPSTART | HASNL | HASLOOKBH);
    ಯಾವತ್ (peekchr() == Magic('|'))
    {
	skipchr();
	br = regbranch(&flags);
	ಯದಿ (br == ರಿಕ್ತ || reg_toolong)
	{
	    ret = ರಿಕ್ತ;
	    ಗಚ್ಛ theend;
	}
	regtail(ret, br);	// BRANCH -> BRANCH.
	ಯದಿ (!(flags & HASWIDTH))
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
    ಪ್ರತಿ (br = ret; br != ರಿಕ್ತ; br = regnext(br))
	regoptail(br, ender);

    // Check for proper termination.
    ಯದಿ (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
#ifdef FEAT_SYN_HL
	ಯದಿ (paren == REG_ZPAREN)
	{
	    emsg(_(e_unmatched_z));
	    rc_did_emsg = TRUE;
	    ret = ರಿಕ್ತ;
	    ಗಚ್ಛ theend;
	}
	ಅನ್ಯಥಾ
#endif
	ಯದಿ (paren == REG_NPAREN)
	{
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = ರಿಕ್ತ;
	    ಗಚ್ಛ theend;
	}
	ಅನ್ಯಥಾ
	{
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = ರಿಕ್ತ;
	    ಗಚ್ಛ theend;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (paren == REG_NOPAREN && peekchr() != NUL)
    {
	ಯದಿ (curchr == Magic(')'))
	{
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = ರಿಕ್ತ;
	    ಗಚ್ಛ theend;
	}
	ಅನ್ಯಥಾ
	{
	    emsg(_(e_trailing_characters));	// "Can't happen".
	    rc_did_emsg = TRUE;
	    ret = ರಿಕ್ತ;
	    ಗಚ್ಛ theend;
	}
    }
    // Here we set the flag allowing back references to this set of
    // parentheses.
    ಯದಿ (paren == REG_PAREN)
	had_endbrace[parno] = TRUE;	// have seen the close paren

theend:
    --bt_reg_parse_depth;
    ಪ್ರತ್ಯಾಗಮ ret;
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
    ಸ್ಥಿರ regprog_T *
bt_regcomp(char_u *ಅಭಿವ್ಯಕ್ತಿ, ಪೂರ್ಣಾಙ್ಕ re_flags)
{
    bt_regprog_T    *r;
    char_u	*scan;
    char_u	*longest;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		flags;

    ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ == ರಿಕ್ತ)
	IEMSG_RET_NULL(e_null_argument);

    init_class_tab();

    // First pass: determine size, legality.
    regcomp_start(ಅಭಿವ್ಯಕ್ತಿ, re_flags);
    regcode = JUST_CALC_SIZE;
    regc(REGMAGIC);
    ಯದಿ (reg(REG_NOPAREN, &flags) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Allocate space.
    r = alloc(offsetof(bt_regprog_T, program) + regsize);
    ಯದಿ (r == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    r->re_in_use = FALSE;
#ifdef DEBUG
    r->regsz = regsize;
#endif

    // Second pass: emit code.
    regcomp_start(ಅಭಿವ್ಯಕ್ತಿ, re_flags);
    regcode = r->program;
    regc(REGMAGIC);
    ಯದಿ (reg(REG_NOPAREN, &flags) == ರಿಕ್ತ || reg_toolong)
    {
	vim_free(r);
	ಯದಿ (reg_toolong)
	    EMSG_RET_NULL(_(e_pattern_too_long));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // Dig out information for optimizations.
    r->regstart = NUL;		// Worst-case defaults.
    r->reganch = 0;
    r->regmust = ರಿಕ್ತ;
    r->regmlen = 0;
    r->regflags = regflags;
    ಯದಿ (flags & HASNL)
	r->regflags |= RF_HASNL;
    ಯದಿ (flags & HASLOOKBH)
	r->regflags |= RF_LOOKBH;
#ifdef FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    r->reghasz = re_has_z;
#endif
    scan = r->program + 1;	// First BRANCH.
    ಯದಿ (OP(regnext(scan)) == END)   // Only one top-level choice.
    {
	scan = OPERAND(scan);

	// Starting-point info.
	ಯದಿ (OP(scan) == BOL || OP(scan) == RE_BOF)
	{
	    r->reganch++;
	    scan = regnext(scan);
	}

	ಯದಿ (OP(scan) == EXACTLY)
	{
	    ಯದಿ (has_mbyte)
		r->regstart = (*mb_ptr2char)(OPERAND(scan));
	    ಅನ್ಯಥಾ
		r->regstart = *OPERAND(scan);
	}
	ಅನ್ಯಥಾ ಯದಿ ((OP(scan) == BOW
		    || OP(scan) == EOW
		    || OP(scan) == NOTHING
		    || OP(scan) == MOPEN + 0 || OP(scan) == NOPEN
		    || OP(scan) == MCLOSE + 0 || OP(scan) == NCLOSE)
		 && OP(regnext(scan)) == EXACTLY)
	{
	    ಯದಿ (has_mbyte)
		r->regstart = (*mb_ptr2char)(OPERAND(regnext(scan)));
	    ಅನ್ಯಥಾ
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
	ಯದಿ ((flags & SPSTART || OP(scan) == BOW || OP(scan) == EOW)
							  && !(flags & HASNL))
	{
	    size_t  scanlen;

	    longest = ರಿಕ್ತ;
	    len = 0;
	    ಪ್ರತಿ (; scan != ರಿಕ್ತ; scan = regnext(scan))
	    {
		ಯದಿ (OP(scan) == EXACTLY)
		{
		    scanlen = STRLEN(OPERAND(scan));
		    ಯದಿ (scanlen >= (size_t)len)
		    {
			longest = OPERAND(scan);
			len = (ಪೂರ್ಣಾಙ್ಕ)scanlen;
		    }
		}
	    }
	    r->regmust = longest;
	    r->regmlen = len;
	}
    }
#ifdef BT_REGEXP_DUMP
    regdump(ಅಭಿವ್ಯಕ್ತಿ, r);
#endif
    r->engine = &bt_regengine;
    ಪ್ರತ್ಯಾಗಮ (regprog_T *)r;
}

#ಯದಿ defined(FEAT_SYN_HL)
/*
 * Check if during the previous call to vim_regcomp the EOL item "$" has been
 * found.  This is messy, but it works fine.
 */
    ಪೂರ್ಣಾಙ್ಕ
vim_regcomp_had_eol(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ had_eol;
}
#endif

/*
 * Get a number after a backslash that is inside [].
 * When nothing is recognized return a backslash.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
coll_get_char(ಶೂನ್ಯ)
{
    vimlong_T	nr = -1;

    ವಿಕಲ್ಪ (*regparse++)
    {
	ಪಕ್ಷ 'd': nr = getdecchrs(); ಭಙ್ಗ;
	ಪಕ್ಷ 'o': nr = getoctchrs(); ಭಙ್ಗ;
	ಪಕ್ಷ 'x': nr = gethexchrs(2); ಭಙ್ಗ;
	ಪಕ್ಷ 'u': nr = gethexchrs(4); ಭಙ್ಗ;
	ಪಕ್ಷ 'U': nr = gethexchrs(8); ಭಙ್ಗ;
    }
    ಯದಿ (nr < 0)
    {
	// If getting the number fails be backwards compatible: the character
	// is a backslash.
	--regparse;
	nr = '\\';
    }
    ಯದಿ (nr > INT_MAX)
	nr = INT_MAX;
    ಪ್ರತ್ಯಾಗಮ nr;
}

/*
 * Free a compiled regexp program, returned by bt_regcomp().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
bt_regfree(regprog_T *prog)
{
    vim_free(prog);
}

#define ADVANCE_REGINPUT() MB_PTR_ADV(rex.ಒಳಹರಿವು)

/*
 * The arguments from BRACE_LIMITS are stored here.  They are actually local
 * to regmatch(), but they are here to reduce the amount of stack space used
 * (it can be called recursively many times).
 */
ಸ್ಥಿರ long	bl_minval;
ಸ್ಥಿರ long	bl_maxval;

/*
 * Save the input line and position in a regsave_T.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
reg_save(regsave_T *save, garray_T *gap)
{
    ಯದಿ (REG_MULTI)
    {
	save->rs_u.pos.col = (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು);
	save->rs_u.pos.lnum = rex.lnum;
    }
    ಅನ್ಯಥಾ
	save->rs_u.ptr = rex.ಒಳಹರಿವು;
    save->rs_len = gap->ga_len;
}

/*
 * Restore the input line and position from a regsave_T.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
reg_restore(regsave_T *save, garray_T *gap)
{
    ಯದಿ (REG_MULTI)
    {
	ಯದಿ (rex.lnum != save->rs_u.pos.lnum)
	{
	    // only call reg_getline() when the line number changed to save
	    // a bit of time
	    rex.lnum = save->rs_u.pos.lnum;
	    rex.ಸಾಲು = reg_getline(rex.lnum);
	}
	rex.ಒಳಹರಿವು = rex.ಸಾಲು + save->rs_u.pos.col;
    }
    ಅನ್ಯಥಾ
	rex.ಒಳಹರಿವು = save->rs_u.ptr;
    gap->ga_len = save->rs_len;
}

/*
 * Return TRUE if current position is equal to saved position.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
reg_save_equal(regsave_T *save)
{
    ಯದಿ (REG_MULTI)
	ಪ್ರತ್ಯಾಗಮ rex.lnum == save->rs_u.pos.lnum
				  && rex.ಒಳಹರಿವು == rex.ಸಾಲು + save->rs_u.pos.col;
    ಪ್ರತ್ಯಾಗಮ rex.ಒಳಹರಿವು == save->rs_u.ptr;
}

// Save the sub-expressions before attempting a match.
#define save_se(savep, posp, pp) \
    REG_MULTI ? save_se_multi((savep), (posp)) : save_se_one((savep), (pp))

// After a failed match restore the sub-expressions.
#define restore_se(savep, posp, pp) \
{ \
    ಯದಿ (REG_MULTI) \
	*(posp) = (savep)->se_u.pos; \
    ಅನ್ಯಥಾ \
	*(pp) = (savep)->se_u.ptr; \
}

/*
 * Tentatively set the sub-expression start to the current position (after
 * calling regmatch() they will have changed).  Need to save the existing
 * values for when there is no match.
 * Use se_save() to use pointer (save_se_multi()) or position (save_se_one()),
 * depending on REG_MULTI.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
save_se_multi(save_se_T *savep, lpos_T *posp)
{
    savep->se_u.pos = *posp;
    posp->lnum = rex.lnum;
    posp->col = (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
save_se_one(save_se_T *savep, char_u **pp)
{
    savep->se_u.ptr = *pp;
    *pp = rex.ಒಳಹರಿವು;
}

/*
 * regrepeat - repeatedly match something simple, return how many.
 * Advances rex.input (and rex.lnum) to just after the matched chars.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
regrepeat(
    char_u	*p,
    long	maxcount)   // maximum number of matches allowed
{
    long	ಎಣಿಕೆ = 0;
    char_u	*scan;
    char_u	*opnd;
    ಪೂರ್ಣಾಙ್ಕ		mask;
    ಪೂರ್ಣಾಙ್ಕ		testval = 0;

    scan = rex.ಒಳಹರಿವು;	    // Make local copy of rex.input for speed.
    opnd = OPERAND(p);
    ವಿಕಲ್ಪ (OP(p))
    {
      ಪಕ್ಷ ANY:
      ಪಕ್ಷ ANY + ADD_NL:
	ಯಾವತ್ (ಎಣಿಕೆ < maxcount)
	{
	    // Matching anything means we continue until end-of-line (or
	    // end-of-file for ANY + ADD_NL), only limited by maxcount.
	    ಯಾವತ್ (*scan != NUL && ಎಣಿಕೆ < maxcount)
	    {
		++ಎಣಿಕೆ;
		MB_PTR_ADV(scan);
	    }
	    ಯದಿ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
				      || rex.reg_ಸಾಲು_lbr || ಎಣಿಕೆ == maxcount)
		ಭಙ್ಗ;
	    ++ಎಣಿಕೆ;		// count the line-break
	    reg_nextline();
	    scan = rex.ಒಳಹರಿವು;
	    ಯದಿ (got_int)
		ಭಙ್ಗ;
	}
	ಭಙ್ಗ;

      ಪಕ್ಷ IDENT:
      ಪಕ್ಷ IDENT + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      ಪಕ್ಷ SIDENT:
      ಪಕ್ಷ SIDENT + ADD_NL:
	ಯಾವತ್ (ಎಣಿಕೆ < maxcount)
	{
	    ಯದಿ (vim_isIDc(PTR2CHAR(scan)) && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (*scan == NUL)
	    {
		ಯದಿ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ಸಾಲು_lbr)
		    ಭಙ್ಗ;
		reg_nextline();
		scan = rex.ಒಳಹರಿವು;
		ಯದಿ (got_int)
		    ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (rex.reg_ಸಾಲು_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    ಅನ್ಯಥಾ
		ಭಙ್ಗ;
	    ++ಎಣಿಕೆ;
	}
	ಭಙ್ಗ;

      ಪಕ್ಷ KWORD:
      ಪಕ್ಷ KWORD + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      ಪಕ್ಷ SKWORD:
      ಪಕ್ಷ SKWORD + ADD_NL:
	ಯಾವತ್ (ಎಣಿಕೆ < maxcount)
	{
	    ಯದಿ (vim_iswordp_buf(scan, rex.reg_buf)
					  && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (*scan == NUL)
	    {
		ಯದಿ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ಸಾಲು_lbr)
		    ಭಙ್ಗ;
		reg_nextline();
		scan = rex.ಒಳಹರಿವು;
		ಯದಿ (got_int)
		    ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (rex.reg_ಸಾಲು_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    ಅನ್ಯಥಾ
		ಭಙ್ಗ;
	    ++ಎಣಿಕೆ;
	}
	ಭಙ್ಗ;

      ಪಕ್ಷ FNAME:
      ಪಕ್ಷ FNAME + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      ಪಕ್ಷ SFNAME:
      ಪಕ್ಷ SFNAME + ADD_NL:
	ಯಾವತ್ (ಎಣಿಕೆ < maxcount)
	{
	    ಯದಿ (vim_isfilec(PTR2CHAR(scan)) && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (*scan == NUL)
	    {
		ಯದಿ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ಸಾಲು_lbr)
		    ಭಙ್ಗ;
		reg_nextline();
		scan = rex.ಒಳಹರಿವು;
		ಯದಿ (got_int)
		    ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (rex.reg_ಸಾಲು_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    ಅನ್ಯಥಾ
		ಭಙ್ಗ;
	    ++ಎಣಿಕೆ;
	}
	ಭಙ್ಗ;

      ಪಕ್ಷ PRINT:
      ಪಕ್ಷ PRINT + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      ಪಕ್ಷ SPRINT:
      ಪಕ್ಷ SPRINT + ADD_NL:
	ಯಾವತ್ (ಎಣಿಕೆ < maxcount)
	{
	    ಯದಿ (*scan == NUL)
	    {
		ಯದಿ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ಸಾಲು_lbr)
		    ಭಙ್ಗ;
		reg_nextline();
		scan = rex.ಒಳಹರಿವು;
		ಯದಿ (got_int)
		    ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (vim_isprintc(PTR2CHAR(scan)) == 1
					  && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (rex.reg_ಸಾಲು_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    ಅನ್ಯಥಾ
		ಭಙ್ಗ;
	    ++ಎಣಿಕೆ;
	}
	ಭಙ್ಗ;

      ಪಕ್ಷ WHITE:
      ಪಕ್ಷ WHITE + ADD_NL:
	testval = mask = RI_WHITE;
do_class:
	ಯಾವತ್ (ಎಣಿಕೆ < maxcount)
	{
	    ಪೂರ್ಣಾಙ್ಕ		l;

	    ಯದಿ (*scan == NUL)
	    {
		ಯದಿ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ಸಾಲು_lbr)
		    ಭಙ್ಗ;
		reg_nextline();
		scan = rex.ಒಳಹರಿವು;
		ಯದಿ (got_int)
		    ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (has_mbyte && (l = (*mb_ptr2len)(scan)) > 1)
	    {
		ಯದಿ (testval != 0)
		    ಭಙ್ಗ;
		scan += l;
	    }
	    ಅನ್ಯಥಾ ಯದಿ ((class_tab[*scan] & mask) == testval)
		++scan;
	    ಅನ್ಯಥಾ ಯದಿ (rex.reg_ಸಾಲು_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    ಅನ್ಯಥಾ
		ಭಙ್ಗ;
	    ++ಎಣಿಕೆ;
	}
	ಭಙ್ಗ;

      ಪಕ್ಷ NWHITE:
      ಪಕ್ಷ NWHITE + ADD_NL:
	mask = RI_WHITE;
	ಗಚ್ಛ do_class;
      ಪಕ್ಷ DIGIT:
      ಪಕ್ಷ DIGIT + ADD_NL:
	testval = mask = RI_DIGIT;
	ಗಚ್ಛ do_class;
      ಪಕ್ಷ NDIGIT:
      ಪಕ್ಷ NDIGIT + ADD_NL:
	mask = RI_DIGIT;
	ಗಚ್ಛ do_class;
      ಪಕ್ಷ HEX:
      ಪಕ್ಷ HEX + ADD_NL:
	testval = mask = RI_HEX;
	ಗಚ್ಛ do_class;
      ಪಕ್ಷ NHEX:
      ಪಕ್ಷ NHEX + ADD_NL:
	mask = RI_HEX;
	ಗಚ್ಛ do_class;
      ಪಕ್ಷ OCTAL:
      ಪಕ್ಷ OCTAL + ADD_NL:
	testval = mask = RI_OCTAL;
	ಗಚ್ಛ do_class;
      ಪಕ್ಷ NOCTAL:
      ಪಕ್ಷ NOCTAL + ADD_NL:
	mask = RI_OCTAL;
	ಗಚ್ಛ do_class;
      ಪಕ್ಷ WORD:
      ಪಕ್ಷ WORD + ADD_NL:
	testval = mask = RI_WORD;
	ಗಚ್ಛ do_class;
      ಪಕ್ಷ NWORD:
      ಪಕ್ಷ NWORD + ADD_NL:
	mask = RI_WORD;
	ಗಚ್ಛ do_class;
      ಪಕ್ಷ HEAD:
      ಪಕ್ಷ HEAD + ADD_NL:
	testval = mask = RI_HEAD;
	ಗಚ್ಛ do_class;
      ಪಕ್ಷ NHEAD:
      ಪಕ್ಷ NHEAD + ADD_NL:
	mask = RI_HEAD;
	ಗಚ್ಛ do_class;
      ಪಕ್ಷ ALPHA:
      ಪಕ್ಷ ALPHA + ADD_NL:
	testval = mask = RI_ALPHA;
	ಗಚ್ಛ do_class;
      ಪಕ್ಷ NALPHA:
      ಪಕ್ಷ NALPHA + ADD_NL:
	mask = RI_ALPHA;
	ಗಚ್ಛ do_class;
      ಪಕ್ಷ LOWER:
      ಪಕ್ಷ LOWER + ADD_NL:
	testval = mask = RI_LOWER;
	ಗಚ್ಛ do_class;
      ಪಕ್ಷ NLOWER:
      ಪಕ್ಷ NLOWER + ADD_NL:
	mask = RI_LOWER;
	ಗಚ್ಛ do_class;
      ಪಕ್ಷ UPPER:
      ಪಕ್ಷ UPPER + ADD_NL:
	testval = mask = RI_UPPER;
	ಗಚ್ಛ do_class;
      ಪಕ್ಷ NUPPER:
      ಪಕ್ಷ NUPPER + ADD_NL:
	mask = RI_UPPER;
	ಗಚ್ಛ do_class;

      ಪಕ್ಷ EXACTLY:
	{
	    ಪೂರ್ಣಾಙ್ಕ	    cu, cl;

	    // This doesn't do a multi-byte character, because a MULTIBYTECODE
	    // would have been used for it.  It does handle single-byte
	    // characters, such as latin1.
	    ಯದಿ (rex.reg_ic)
	    {
		cu = MB_TOUPPER(*opnd);
		cl = MB_TOLOWER(*opnd);
		ಯಾವತ್ (ಎಣಿಕೆ < maxcount && (*scan == cu || *scan == cl))
		{
		    ಎಣಿಕೆ++;
		    scan++;
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		cu = *opnd;
		ಯಾವತ್ (ಎಣಿಕೆ < maxcount && *scan == cu)
		{
		    ಎಣಿಕೆ++;
		    scan++;
		}
	    }
	    ಭಙ್ಗ;
	}

      ಪಕ್ಷ MULTIBYTECODE:
	{
	    ಪೂರ್ಣಾಙ್ಕ		i, len, cf = 0;

	    // Safety check (just in case 'encoding' was changed since
	    // compiling the program).
	    ಯದಿ ((len = (*mb_ptr2len)(opnd)) > 1)
	    {
		ಯದಿ (rex.reg_ic && enc_utf8)
		    cf = utf_fold(utf_ptr2char(opnd));
		ಯಾವತ್ (ಎಣಿಕೆ < maxcount && (*mb_ptr2len)(scan) >= len)
		{
		    ಪ್ರತಿ (i = 0; i < len; ++i)
			ಯದಿ (opnd[i] != scan[i])
			    ಭಙ್ಗ;
		    ಯದಿ (i < len && (!rex.reg_ic || !enc_utf8
					|| utf_fold(utf_ptr2char(scan)) != cf))
			ಭಙ್ಗ;
		    scan += len;
		    ++ಎಣಿಕೆ;
		}
	    }
	}
	ಭಙ್ಗ;

      ಪಕ್ಷ ANYOF:
      ಪಕ್ಷ ANYOF + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH

      ಪಕ್ಷ ANYBUT:
      ಪಕ್ಷ ANYBUT + ADD_NL:
	ಯಾವತ್ (ಎಣಿಕೆ < maxcount)
	{
	    ಪೂರ್ಣಾಙ್ಕ len;

	    ಯದಿ (*scan == NUL)
	    {
		ಯದಿ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ಸಾಲು_lbr)
		    ಭಙ್ಗ;
		reg_nextline();
		scan = rex.ಒಳಹರಿವು;
		ಯದಿ (got_int)
		    ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (rex.reg_ಸಾಲು_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    ಅನ್ಯಥಾ ಯದಿ (has_mbyte && (len = (*mb_ptr2len)(scan)) > 1)
	    {
		ಯದಿ ((cstrchr(opnd, (*mb_ptr2char)(scan)) == ರಿಕ್ತ) == testval)
		    ಭಙ್ಗ;
		scan += len;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ ((cstrchr(opnd, *scan) == ರಿಕ್ತ) == testval)
		    ಭಙ್ಗ;
		++scan;
	    }
	    ++ಎಣಿಕೆ;
	}
	ಭಙ್ಗ;

      ಪಕ್ಷ NEWL:
	ಯಾವತ್ (ಎಣಿಕೆ < maxcount
		&& ((*scan == NUL && rex.lnum <= rex.reg_maxline
				       && !rex.reg_ಸಾಲು_lbr && REG_MULTI)
		    || (*scan == '\n' && rex.reg_ಸಾಲು_lbr)))
	{
	    ಎಣಿಕೆ++;
	    ಯದಿ (rex.reg_ಸಾಲು_lbr)
		ADVANCE_REGINPUT();
	    ಅನ್ಯಥಾ
		reg_nextline();
	    scan = rex.ಒಳಹರಿವು;
	    ಯದಿ (got_int)
		ಭಙ್ಗ;
	}
	ಭಙ್ಗ;

      ಸ್ವಭಾವ:			// Oh dear.  Called inappropriately.
	iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	printf("Called regrepeat with op code %d\n", OP(p));
#endif
	ಭಙ್ಗ;
    }

    rex.ಒಳಹರಿವು = scan;

    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)ಎಣಿಕೆ;
}

/*
 * Push an item onto the regstack.
 * Returns pointer to new item.  Returns NULL when out of memory.
 */
    ಸ್ಥಿರ regitem_T *
regstack_ತಳ್ಳು(regstate_T state, char_u *scan)
{
    regitem_T	*rp;

    ಯದಿ ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
    {
	emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (GA_GROW_FAILS(&regstack, (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(regitem_T)))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    rp = (regitem_T *)((ವರ್ಣ *)regstack.ga_data + regstack.ga_len);
    rp->rs_state = state;
    rp->rs_scan = scan;

    regstack.ga_len += ಪರಿಮಾಣ(regitem_T);
    ಪ್ರತ್ಯಾಗಮ rp;
}

/*
 * Pop an item from the regstack.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
regstack_ತೆಗೆ(char_u **scan)
{
    regitem_T	*rp;

    rp = (regitem_T *)((ವರ್ಣ *)regstack.ga_data + regstack.ga_len) - 1;
    *scan = rp->rs_scan;

    regstack.ga_len -= ಪರಿಮಾಣ(regitem_T);
}

#ifdef FEAT_RELTIME
/*
 * Check if the timer expired, return TRUE if so.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
bt_did_time_out(ಪೂರ್ಣಾಙ್ಕ *timed_out)
{
    ಯದಿ (*timeout_flag)
    {
	ಯದಿ (timed_out != ರಿಕ್ತ)
	{
# ifdef FEAT_EVAL
	    ಯದಿ (!*timed_out)
		ch_log(ರಿಕ್ತ, "BT regexp timed out");
# endif
	    *timed_out = TRUE;
	}
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}
#endif

/*
 * Save the current subexpr to "bp", so that they can be restored
 * later by restore_subexpr().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
save_subexpr(regbehind_T *bp)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    // When "rex.need_clear_subexpr" is set we don't need to save the values,
    // only remember that this flag needs to be set again when restoring.
    bp->save_need_clear_subexpr = rex.need_clear_subexpr;
    ಯದಿ (rex.need_clear_subexpr)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (i = 0; i < NSUBEXP; ++i)
    {
	ಯದಿ (REG_MULTI)
	{
	    bp->save_start[i].se_u.pos = rex.reg_startpos[i];
	    bp->save_end[i].se_u.pos = rex.reg_endpos[i];
	}
	ಅನ್ಯಥಾ
	{
	    bp->save_start[i].se_u.ptr = rex.reg_startp[i];
	    bp->save_end[i].se_u.ptr = rex.reg_endp[i];
	}
    }
}

/*
 * Restore the subexpr from "bp".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
restore_subexpr(regbehind_T *bp)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    // Only need to restore saved values when they are not to be cleared.
    rex.need_clear_subexpr = bp->save_need_clear_subexpr;
    ಯದಿ (rex.need_clear_subexpr)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (i = 0; i < NSUBEXP; ++i)
    {
	ಯದಿ (REG_MULTI)
	{
	    rex.reg_startpos[i] = bp->save_start[i].se_u.pos;
	    rex.reg_endpos[i] = bp->save_end[i].se_u.pos;
	}
	ಅನ್ಯಥಾ
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
regmatch(
    char_u	*scan,		    // Current node.
    ಪೂರ್ಣಾಙ್ಕ		*timed_out UNUSED)  // flag set on timeout or NULL
{
  char_u	*next;		// Next node.
  ಪೂರ್ಣಾಙ್ಕ		op;
  ಪೂರ್ಣಾಙ್ಕ		c;
  regitem_T	*rp;
  ಪೂರ್ಣಾಙ್ಕ		no;
  ಪೂರ್ಣಾಙ್ಕ		status;		// one of the RA_ values:

  // Make "regstack" and "backpos" empty.  They are allocated and freed in
  // bt_regexec_both() to reduce malloc()/free() calls.
  regstack.ga_len = 0;
  backpos.ga_len = 0;

  // Repeat until "regstack" is empty.
  ಪ್ರತಿ (;;)
  {
    // Some patterns may take a long time to match, e.g., "\([a-z]\+\)\+Q".
    // Allow interrupting them with CTRL-C.
    fast_breakcheck();

#ifdef DEBUG
    ಯದಿ (scan != ರಿಕ್ತ && regnarrate)
    {
	mch_errmsg((ವರ್ಣ *)regprop(scan));
	mch_errmsg("(\n");
    }
#endif

    // Repeat for items that can be matched sequentially, without using the
    // regstack.
    ಪ್ರತಿ (;;)
    {
	ಯದಿ (got_int || scan == ರಿಕ್ತ)
	{
	    status = RA_FAIL;
	    ಭಙ್ಗ;
	}
#ifdef FEAT_RELTIME
	ಯದಿ (bt_did_time_out(timed_out))
	{
	    status = RA_FAIL;
	    ಭಙ್ಗ;
	}
#endif
	status = RA_CONT;

#ifdef DEBUG
	ಯದಿ (regnarrate)
	{
	    mch_errmsg((ವರ್ಣ *)regprop(scan));
	    mch_errmsg("...\n");
# ifdef FEAT_SYN_HL
	    ಯದಿ (re_extmatch_in != ರಿಕ್ತ)
	    {
		ಪೂರ್ಣಾಙ್ಕ i;

		mch_errmsg(_("External submatches:\n"));
		ಪ್ರತಿ (i = 0; i < NSUBEXP; i++)
		{
		    mch_errmsg("    \"");
		    ಯದಿ (re_extmatch_in->matches[i] != ರಿಕ್ತ)
			mch_errmsg((ವರ್ಣ *)re_extmatch_in->matches[i]);
		    mch_errmsg("\"\n");
		}
	    }
# endif
	}
#endif
	next = regnext(scan);

	op = OP(scan);
	// Check for character class with NL added.
	ಯದಿ (!rex.reg_ಸಾಲು_lbr && WITH_NL(op) && REG_MULTI
			     && *rex.ಒಳಹರಿವು == NUL && rex.lnum <= rex.reg_maxline)
	{
	    reg_nextline();
	}
	ಅನ್ಯಥಾ ಯದಿ (rex.reg_ಸಾಲು_lbr && WITH_NL(op) && *rex.ಒಳಹರಿವು == '\n')
	{
	    ADVANCE_REGINPUT();
	}
	ಅನ್ಯಥಾ
	{
	  ಯದಿ (WITH_NL(op))
	      op -= ADD_NL;
	  ಯದಿ (has_mbyte)
	      c = (*mb_ptr2char)(rex.ಒಳಹರಿವು);
	  ಅನ್ಯಥಾ
	      c = *rex.ಒಳಹರಿವು;
	  ವಿಕಲ್ಪ (op)
	  {
	  ಪಕ್ಷ BOL:
	    ಯದಿ (rex.ಒಳಹರಿವು != rex.ಸಾಲು)
		status = RA_NOMATCH;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ EOL:
	    ಯದಿ (c != NUL)
		status = RA_NOMATCH;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ RE_BOF:
	    // We're not at the beginning of the file when below the first
	    // line where we started, not at the start of the line or we
	    // didn't start at the first line of the buffer.
	    ಯದಿ (rex.lnum != 0 || rex.ಒಳಹರಿವು != rex.ಸಾಲು
				       || (REG_MULTI && rex.reg_firstlnum > 1))
		status = RA_NOMATCH;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ RE_EOF:
	    ಯದಿ (rex.lnum != rex.reg_maxline || c != NUL)
		status = RA_NOMATCH;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ CURSOR:
	    // Check if the buffer is in a window and compare the
	    // rex.reg_win->w_cursor position to the match position.
	    ಯದಿ (rex.reg_win == ರಿಕ್ತ
		    || (rex.lnum + rex.reg_firstlnum
						 != rex.reg_win->w_cursor.lnum)
		    || ((colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು)
						 != rex.reg_win->w_cursor.col))
		status = RA_NOMATCH;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ RE_MARK:
	    // Compare the mark position to the match position.
	    {
		ಪೂರ್ಣಾಙ್ಕ	mark = OPERAND(scan)[0];
		ಪೂರ್ಣಾಙ್ಕ	cmp = OPERAND(scan)[1];
		pos_T	*pos;
		size_t	col = REG_MULTI ? rex.ಒಳಹರಿವು - rex.ಸಾಲು : 0;

		pos = getmark_buf(rex.reg_buf, mark, FALSE);

		// Line may have been freed, get it again.
		ಯದಿ (REG_MULTI)
		{
		    rex.ಸಾಲು = reg_getline(rex.lnum);
		    rex.ಒಳಹರಿವು = rex.ಸಾಲು + col;
		}

		ಯದಿ (pos == ರಿಕ್ತ		     // mark doesn't exist
			|| pos->lnum <= 0)   // mark isn't set in reg_buf
		{
		    status = RA_NOMATCH;
		}
		ಅನ್ಯಥಾ
		{
		    colnr_T pos_col = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->col == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->col;

		    ಯದಿ ((pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_col == (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು)
				    ? (cmp == '<' || cmp == '>')
				    : (pos_col < (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು)
					? cmp != '>'
					: cmp != '<'))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? cmp != '>'
				    : cmp != '<')))
		    status = RA_NOMATCH;
		}
	    }
	    ಭಙ್ಗ;

	  ಪಕ್ಷ RE_VISUAL:
	    ಯದಿ (!reg_match_visual())
		status = RA_NOMATCH;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ RE_LNUM:
	    ಯದಿ (!REG_MULTI || !re_ಅಂಕೆ_cmp((long_u)(rex.lnum + rex.reg_firstlnum),
									scan))
		status = RA_NOMATCH;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ RE_COL:
	    ಯದಿ (!re_ಅಂಕೆ_cmp((long_u)(rex.ಒಳಹರಿವು - rex.ಸಾಲು) + 1, scan))
		status = RA_NOMATCH;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ RE_VCOL:
	    {
		win_T	    *wp = rex.reg_win == ರಿಕ್ತ ? curwin : rex.reg_win;
		linenr_T    lnum = REG_MULTI ? rex.reg_firstlnum + rex.lnum : 1;
		long_u	    vcol;

		ಯದಿ (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ))
		    lnum = 1;
		vcol = (long_u)win_linetabsize(wp, lnum, rex.ಸಾಲು,
					      (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು));
		ಯದಿ (!re_ಅಂಕೆ_cmp(vcol + 1, scan))
		    status = RA_NOMATCH;
	    }
	    ಭಙ್ಗ;

	  ಪಕ್ಷ BOW:	// \<word; rex.input points to w
	    ಯದಿ (c == NUL)	// Can't match at end of line
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ ಯದಿ (has_mbyte)
	    {
		ಪೂರ್ಣಾಙ್ಕ this_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.ಒಳಹರಿವು, rex.reg_buf);
		ಯದಿ (this_class <= 1)
		    status = RA_NOMATCH;  // not on a word at all
		ಅನ್ಯಥಾ ಯದಿ (reg_prev_class() == this_class)
		    status = RA_NOMATCH;  // previous char is in same word
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (!vim_iswordc_buf(c, rex.reg_buf) || (rex.ಒಳಹರಿವು > rex.ಸಾಲು
				&& vim_iswordc_buf(rex.ಒಳಹರಿವು[-1], rex.reg_buf)))
		    status = RA_NOMATCH;
	    }
	    ಭಙ್ಗ;

	  ಪಕ್ಷ EOW:	// word\>; rex.input points after d
	    ಯದಿ (rex.ಒಳಹರಿವು == rex.ಸಾಲು)    // Can't match at start of line
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ ಯದಿ (has_mbyte)
	    {
		ಪೂರ್ಣಾಙ್ಕ this_class, prev_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.ಒಳಹರಿವು, rex.reg_buf);
		prev_class = reg_prev_class();
		ಯದಿ (this_class == prev_class
			|| prev_class == 0 || prev_class == 1)
		    status = RA_NOMATCH;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (!vim_iswordc_buf(rex.ಒಳಹರಿವು[-1], rex.reg_buf)
			|| (rex.ಒಳಹರಿವು[0] != NUL
					   && vim_iswordc_buf(c, rex.reg_buf)))
		    status = RA_NOMATCH;
	    }
	    ಭಙ್ಗ; // Matched with EOW

	  ಪಕ್ಷ ANY:
	    // ANY does not match new lines.
	    ಯದಿ (c == NUL)
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
		ADVANCE_REGINPUT();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ IDENT:
	    ಯದಿ (!vim_isIDc(c))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
		ADVANCE_REGINPUT();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ SIDENT:
	    ಯದಿ (VIM_ISDIGIT(*rex.ಒಳಹರಿವು) || !vim_isIDc(c))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
		ADVANCE_REGINPUT();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ KWORD:
	    ಯದಿ (!vim_iswordp_buf(rex.ಒಳಹರಿವು, rex.reg_buf))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
		ADVANCE_REGINPUT();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ SKWORD:
	    ಯದಿ (VIM_ISDIGIT(*rex.ಒಳಹರಿವು)
				    || !vim_iswordp_buf(rex.ಒಳಹರಿವು, rex.reg_buf))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
		ADVANCE_REGINPUT();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ FNAME:
	    ಯದಿ (!vim_isfilec(c))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
		ADVANCE_REGINPUT();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ SFNAME:
	    ಯದಿ (VIM_ISDIGIT(*rex.ಒಳಹರಿವು) || !vim_isfilec(c))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
		ADVANCE_REGINPUT();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ PRINT:
	    ಯದಿ (!vim_isprintc(PTR2CHAR(rex.ಒಳಹರಿವು)))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
		ADVANCE_REGINPUT();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ SPRINT:
	    ಯದಿ (VIM_ISDIGIT(*rex.ಒಳಹರಿವು) || !vim_isprintc(PTR2CHAR(rex.ಒಳಹರಿವು)))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
		ADVANCE_REGINPUT();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ WHITE:
	    ಯದಿ (!VIM_ISWHITE(c))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
		ADVANCE_REGINPUT();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ NWHITE:
	    ಯದಿ (c == NUL || VIM_ISWHITE(c))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
		ADVANCE_REGINPUT();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ DIGIT:
	    ಯದಿ (!ri_digit(c))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
		ADVANCE_REGINPUT();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ NDIGIT:
	    ಯದಿ (c == NUL || ri_digit(c))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
		ADVANCE_REGINPUT();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ HEX:
	    ಯದಿ (!ri_hex(c))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
		ADVANCE_REGINPUT();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ NHEX:
	    ಯದಿ (c == NUL || ri_hex(c))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
		ADVANCE_REGINPUT();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ OCTAL:
	    ಯದಿ (!ri_octal(c))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
		ADVANCE_REGINPUT();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ NOCTAL:
	    ಯದಿ (c == NUL || ri_octal(c))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
		ADVANCE_REGINPUT();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ WORD:
	    ಯದಿ (!ri_word(c))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
		ADVANCE_REGINPUT();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ NWORD:
	    ಯದಿ (c == NUL || ri_word(c))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
		ADVANCE_REGINPUT();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ HEAD:
	    ಯದಿ (!ri_head(c))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
		ADVANCE_REGINPUT();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ NHEAD:
	    ಯದಿ (c == NUL || ri_head(c))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
		ADVANCE_REGINPUT();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ ALPHA:
	    ಯದಿ (!ri_alpha(c))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
		ADVANCE_REGINPUT();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ NALPHA:
	    ಯದಿ (c == NUL || ri_alpha(c))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
		ADVANCE_REGINPUT();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ LOWER:
	    ಯದಿ (!ri_lower(c))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
		ADVANCE_REGINPUT();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ NLOWER:
	    ಯದಿ (c == NUL || ri_lower(c))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
		ADVANCE_REGINPUT();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ UPPER:
	    ಯದಿ (!ri_upper(c))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
		ADVANCE_REGINPUT();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ NUPPER:
	    ಯದಿ (c == NUL || ri_upper(c))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
		ADVANCE_REGINPUT();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ EXACTLY:
	    {
		ಪೂರ್ಣಾಙ್ಕ	len;
		char_u	*opnd;

		opnd = OPERAND(scan);
		// Inline the first byte, for speed.
		ಯದಿ (*opnd != *rex.ಒಳಹರಿವು
			&& (!rex.reg_ic
			    || (!enc_utf8
			      && MB_TOLOWER(*opnd) != MB_TOLOWER(*rex.ಒಳಹರಿವು))))
		    status = RA_NOMATCH;
		ಅನ್ಯಥಾ ಯದಿ (*opnd == NUL)
		{
		    // match empty string always works; happens when "~" is
		    // empty.
		}
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (opnd[1] == NUL && !(enc_utf8 && rex.reg_ic))
		    {
			len = 1;	// matched a single byte above
		    }
		    ಅನ್ಯಥಾ
		    {
			// Need to match first byte again for multi-byte.
			len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(opnd);
			ಯದಿ (cstrncmp(opnd, rex.ಒಳಹರಿವು, &len) != 0)
			    status = RA_NOMATCH;
		    }
		    // Check for following composing character, unless %C
		    // follows (skips over all composing chars).
		    ಯದಿ (status != RA_NOMATCH
			    && enc_utf8
			    && UTF_COMPOSINGLIKE(rex.ಒಳಹರಿವು, rex.ಒಳಹರಿವು + len)
			    && !rex.reg_icombine
			    && OP(next) != RE_COMPOSING)
		    {
			// raaron: This code makes a composing character get
			// ignored, which is the correct behavior (sometimes)
			// for voweled Hebrew texts.
			status = RA_NOMATCH;
		    }
		    ಯದಿ (status != RA_NOMATCH)
			rex.ಒಳಹರಿವು += len;
		}
	    }
	    ಭಙ್ಗ;

	  ಪಕ್ಷ ANYOF:
	  ಪಕ್ಷ ANYBUT:
	    {
		char_u  *q = OPERAND(scan);

		ಯದಿ (c == NUL)
		    status = RA_NOMATCH;
		ಅನ್ಯಥಾ ಯದಿ ((cstrchr(q, c) == ರಿಕ್ತ) == (op == ANYOF))
		    status = RA_NOMATCH;
		ಅನ್ಯಥಾ
		{
		    // Check following combining characters
		    ಪೂರ್ಣಾಙ್ಕ	len = 0;
		    ಪೂರ್ಣಾಙ್ಕ i;

		    ಯದಿ (enc_utf8)
			len = utfc_ptr2len(q) - utf_ptr2len(q);

		    MB_CPTR_ADV(rex.ಒಳಹರಿವು);
		    MB_CPTR_ADV(q);

		    ಯದಿ (!enc_utf8 || len == 0)
			ಭಙ್ಗ;

		    ಪ್ರತಿ (i = 0; i < len; ++i)
			ಯದಿ (q[i] != rex.ಒಳಹರಿವು[i])
			{
			    status = RA_NOMATCH;
			    ಭಙ್ಗ;
			}
		    rex.ಒಳಹರಿವು += len;
		}
		ಭಙ್ಗ;
	    }

	  ಪಕ್ಷ MULTIBYTECODE:
	    ಯದಿ (has_mbyte)
	    {
		ಪೂರ್ಣಾಙ್ಕ	i, len;
		char_u	*opnd;
		ಪೂರ್ಣಾಙ್ಕ	opndc = 0, inpc;

		opnd = OPERAND(scan);
		// Safety check (just in case 'encoding' was changed since
		// compiling the program).
		ಯದಿ ((len = (*mb_ptr2len)(opnd)) < 2)
		{
		    status = RA_NOMATCH;
		    ಭಙ್ಗ;
		}
		ಯದಿ (enc_utf8)
		    opndc = utf_ptr2char(opnd);
		ಯದಿ (enc_utf8 && utf_iscomposing(opndc))
		{
		    // When only a composing char is given match at any
		    // position where that composing char appears.
		    status = RA_NOMATCH;
		    ಪ್ರತಿ (i = 0; rex.ಒಳಹರಿವು[i] != NUL;
						i += utf_ptr2len(rex.ಒಳಹರಿವು + i))
		    {
			inpc = utf_ptr2char(rex.ಒಳಹರಿವು + i);
			ಯದಿ (!utf_iscomposing(inpc))
			{
			    ಯದಿ (i > 0)
				ಭಙ್ಗ;
			}
			ಅನ್ಯಥಾ ಯದಿ (opndc == inpc)
			{
			    // Include all following composing chars.
			    len = i + utfc_ptr2len(rex.ಒಳಹರಿವು + i);
			    status = RA_MATCH;
			    ಭಙ್ಗ;
			}
		    }
		}
		ಅನ್ಯಥಾ ಯದಿ (enc_utf8)
		{
		    ಯದಿ (cstrncmp(opnd, rex.ಒಳಹರಿವು, &len) != 0)
		    {
			status = RA_NOMATCH;
			ಭಙ್ಗ;
		    }
		}
		ಅನ್ಯಥಾ
		    ಪ್ರತಿ (i = 0; i < len; ++i)
			ಯದಿ (opnd[i] != rex.ಒಳಹರಿವು[i])
			{
			    status = RA_NOMATCH;
			    ಭಙ್ಗ;
			}
		rex.ಒಳಹರಿವು += len;
	    }
	    ಅನ್ಯಥಾ
		status = RA_NOMATCH;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ RE_COMPOSING:
	    ಯದಿ (enc_utf8)
	    {
		// Skip composing characters.
		ಯಾವತ್ (utf_iscomposing(utf_ptr2char(rex.ಒಳಹರಿವು)))
		    MB_CPTR_ADV(rex.ಒಳಹರಿವು);
	    }
	    ಭಙ್ಗ;

	  ಪಕ್ಷ NOTHING:
	    ಭಙ್ಗ;

	  ಪಕ್ಷ BACK:
	    {
		ಪೂರ್ಣಾಙ್ಕ		i;
		backpos_T	*bp;

		// When we run into BACK we need to check if we don't keep
		// looping without matching any input.  The second and later
		// times a BACK is encountered it fails if the input is still
		// at the same position as the previous time.
		// The positions are stored in "backpos" and found by the
		// current value of "scan", the position in the RE program.
		bp = (backpos_T *)backpos.ga_data;
		ಪ್ರತಿ (i = 0; i < backpos.ga_len; ++i)
		    ಯದಿ (bp[i].bp_scan == scan)
			ಭಙ್ಗ;
		ಯದಿ (i == backpos.ga_len)
		{
		    // First time at this BACK, make room to store the pos.
		    ಯದಿ (GA_GROW_FAILS(&backpos, 1))
			status = RA_FAIL;
		    ಅನ್ಯಥಾ
		    {
			// get "ga_data" again, it may have changed
			bp = (backpos_T *)backpos.ga_data;
			bp[i].bp_scan = scan;
			++backpos.ga_len;
		    }
		}
		ಅನ್ಯಥಾ ಯದಿ (reg_save_equal(&bp[i].bp_pos))
		    // Still at same position as last time, fail.
		    status = RA_NOMATCH;

		ಯದಿ (status != RA_FAIL && status != RA_NOMATCH)
		    reg_save(&bp[i].bp_pos, &backpos);
	    }
	    ಭಙ್ಗ;

	  ಪಕ್ಷ MOPEN + 0:   // Match start: \zs
	  ಪಕ್ಷ MOPEN + 1:   // \(
	  ಪಕ್ಷ MOPEN + 2:
	  ಪಕ್ಷ MOPEN + 3:
	  ಪಕ್ಷ MOPEN + 4:
	  ಪಕ್ಷ MOPEN + 5:
	  ಪಕ್ಷ MOPEN + 6:
	  ಪಕ್ಷ MOPEN + 7:
	  ಪಕ್ಷ MOPEN + 8:
	  ಪಕ್ಷ MOPEN + 9:
	    {
		no = op - MOPEN;
		cleanup_subexpr();
		rp = regstack_ತಳ್ಳು(RS_MOPEN, scan);
		ಯದಿ (rp == ರಿಕ್ತ)
		    status = RA_FAIL;
		ಅನ್ಯಥಾ
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_startpos[no],
							  &rex.reg_startp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    ಭಙ್ಗ;

	  ಪಕ್ಷ NOPEN:	    // \%(
	  ಪಕ್ಷ NCLOSE:	    // \) after \%(
		ಯದಿ (regstack_ತಳ್ಳು(RS_NOPEN, scan) == ರಿಕ್ತ)
		    status = RA_FAIL;
		// We simply continue and handle the result when done.
		ಭಙ್ಗ;

#ifdef FEAT_SYN_HL
	  ಪಕ್ಷ ZOPEN + 1:
	  ಪಕ್ಷ ZOPEN + 2:
	  ಪಕ್ಷ ZOPEN + 3:
	  ಪಕ್ಷ ZOPEN + 4:
	  ಪಕ್ಷ ZOPEN + 5:
	  ಪಕ್ಷ ZOPEN + 6:
	  ಪಕ್ಷ ZOPEN + 7:
	  ಪಕ್ಷ ZOPEN + 8:
	  ಪಕ್ಷ ZOPEN + 9:
	    {
		no = op - ZOPEN;
		cleanup_zsubexpr();
		rp = regstack_ತಳ್ಳು(RS_ZOPEN, scan);
		ಯದಿ (rp == ರಿಕ್ತ)
		    status = RA_FAIL;
		ಅನ್ಯಥಾ
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_startzpos[no],
							     &reg_startzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    ಭಙ್ಗ;
#endif

	  ಪಕ್ಷ MCLOSE + 0:  // Match end: \ze
	  ಪಕ್ಷ MCLOSE + 1:  // \)
	  ಪಕ್ಷ MCLOSE + 2:
	  ಪಕ್ಷ MCLOSE + 3:
	  ಪಕ್ಷ MCLOSE + 4:
	  ಪಕ್ಷ MCLOSE + 5:
	  ಪಕ್ಷ MCLOSE + 6:
	  ಪಕ್ಷ MCLOSE + 7:
	  ಪಕ್ಷ MCLOSE + 8:
	  ಪಕ್ಷ MCLOSE + 9:
	    {
		no = op - MCLOSE;
		cleanup_subexpr();
		rp = regstack_ತಳ್ಳು(RS_MCLOSE, scan);
		ಯದಿ (rp == ರಿಕ್ತ)
		    status = RA_FAIL;
		ಅನ್ಯಥಾ
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_endpos[no],
							    &rex.reg_endp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    ಭಙ್ಗ;

#ifdef FEAT_SYN_HL
	  ಪಕ್ಷ ZCLOSE + 1:  // \) after \z(
	  ಪಕ್ಷ ZCLOSE + 2:
	  ಪಕ್ಷ ZCLOSE + 3:
	  ಪಕ್ಷ ZCLOSE + 4:
	  ಪಕ್ಷ ZCLOSE + 5:
	  ಪಕ್ಷ ZCLOSE + 6:
	  ಪಕ್ಷ ZCLOSE + 7:
	  ಪಕ್ಷ ZCLOSE + 8:
	  ಪಕ್ಷ ZCLOSE + 9:
	    {
		no = op - ZCLOSE;
		cleanup_zsubexpr();
		rp = regstack_ತಳ್ಳು(RS_ZCLOSE, scan);
		ಯದಿ (rp == ರಿಕ್ತ)
		    status = RA_FAIL;
		ಅನ್ಯಥಾ
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_endzpos[no],
							      &reg_endzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    ಭಙ್ಗ;
#endif

	  ಪಕ್ಷ BACKREF + 1:
	  ಪಕ್ಷ BACKREF + 2:
	  ಪಕ್ಷ BACKREF + 3:
	  ಪಕ್ಷ BACKREF + 4:
	  ಪಕ್ಷ BACKREF + 5:
	  ಪಕ್ಷ BACKREF + 6:
	  ಪಕ್ಷ BACKREF + 7:
	  ಪಕ್ಷ BACKREF + 8:
	  ಪಕ್ಷ BACKREF + 9:
	    {
		ಪೂರ್ಣಾಙ್ಕ		len;

		no = op - BACKREF;
		cleanup_subexpr();
		ಯದಿ (!REG_MULTI)		// Single-line regexp
		{
		    ಯದಿ (rex.reg_startp[no] == ರಿಕ್ತ || rex.reg_endp[no] == ರಿಕ್ತ)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    ಅನ್ಯಥಾ
		    {
			// Compare current input with back-ref in the same
			// line.
			len = (ಪೂರ್ಣಾಙ್ಕ)(rex.reg_endp[no] - rex.reg_startp[no]);
			ಯದಿ (cstrncmp(rex.reg_startp[no], rex.ಒಳಹರಿವು, &len) != 0)
			    status = RA_NOMATCH;
		    }
		}
		ಅನ್ಯಥಾ				// Multi-line regexp
		{
		    ಯದಿ (rex.reg_startpos[no].lnum < 0
						|| rex.reg_endpos[no].lnum < 0)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    ಅನ್ಯಥಾ
		    {
			ಯದಿ (rex.reg_startpos[no].lnum == rex.lnum
				&& rex.reg_endpos[no].lnum == rex.lnum)
			{
			    // Compare back-ref within the current line.
			    len = rex.reg_endpos[no].col
						    - rex.reg_startpos[no].col;
			    ಯದಿ (cstrncmp(rex.ಸಾಲು + rex.reg_startpos[no].col,
							  rex.ಒಳಹರಿವು, &len) != 0)
				status = RA_NOMATCH;
			}
			ಅನ್ಯಥಾ
			{
			    // Messy situation: Need to compare between two
			    // lines.
			    ಪೂರ್ಣಾಙ್ಕ r = match_with_backref(
					    rex.reg_startpos[no].lnum,
					    rex.reg_startpos[no].col,
					    rex.reg_endpos[no].lnum,
					    rex.reg_endpos[no].col,
					    &len);

			    ಯದಿ (r != RA_MATCH)
				status = r;
			}
		    }
		}

		// Matched the backref, skip over it.
		rex.ಒಳಹರಿವು += len;
	    }
	    ಭಙ್ಗ;

#ifdef FEAT_SYN_HL
	  ಪಕ್ಷ ZREF + 1:
	  ಪಕ್ಷ ZREF + 2:
	  ಪಕ್ಷ ZREF + 3:
	  ಪಕ್ಷ ZREF + 4:
	  ಪಕ್ಷ ZREF + 5:
	  ಪಕ್ಷ ZREF + 6:
	  ಪಕ್ಷ ZREF + 7:
	  ಪಕ್ಷ ZREF + 8:
	  ಪಕ್ಷ ZREF + 9:
	    {
		ಪೂರ್ಣಾಙ್ಕ	len;

		cleanup_zsubexpr();
		no = op - ZREF;
		ಯದಿ (re_extmatch_in != ರಿಕ್ತ
			&& re_extmatch_in->matches[no] != ರಿಕ್ತ)
		{
		    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(re_extmatch_in->matches[no]);
		    ಯದಿ (cstrncmp(re_extmatch_in->matches[no],
							  rex.ಒಳಹರಿವು, &len) != 0)
			status = RA_NOMATCH;
		    ಅನ್ಯಥಾ
			rex.ಒಳಹರಿವು += len;
		}
		ಅನ್ಯಥಾ
		{
		    // Backref was not set: Match an empty string.
		}
	    }
	    ಭಙ್ಗ;
#endif

	  ಪಕ್ಷ BRANCH:
	    {
		ಯದಿ (OP(next) != BRANCH) // No choice.
		    next = OPERAND(scan);	// Avoid recursion.
		ಅನ್ಯಥಾ
		{
		    rp = regstack_ತಳ್ಳು(RS_BRANCH, scan);
		    ಯದಿ (rp == ರಿಕ್ತ)
			status = RA_FAIL;
		    ಅನ್ಯಥಾ
			status = RA_BREAK;	// rest is below
		}
	    }
	    ಭಙ್ಗ;

	  ಪಕ್ಷ BRACE_LIMITS:
	    {
		ಯದಿ (OP(next) == BRACE_SIMPLE)
		{
		    bl_minval = OPERAND_MIN(scan);
		    bl_maxval = OPERAND_MAX(scan);
		}
		ಅನ್ಯಥಾ ಯದಿ (OP(next) >= BRACE_COMPLEX
			&& OP(next) < BRACE_COMPLEX + 10)
		{
		    no = OP(next) - BRACE_COMPLEX;
		    brace_min[no] = OPERAND_MIN(scan);
		    brace_max[no] = OPERAND_MAX(scan);
		    brace_ಎಣಿಕೆ[no] = 0;
		}
		ಅನ್ಯಥಾ
		{
		    internal_error("BRACE_LIMITS");
		    status = RA_FAIL;
		}
	    }
	    ಭಙ್ಗ;

	  ಪಕ್ಷ BRACE_COMPLEX + 0:
	  ಪಕ್ಷ BRACE_COMPLEX + 1:
	  ಪಕ್ಷ BRACE_COMPLEX + 2:
	  ಪಕ್ಷ BRACE_COMPLEX + 3:
	  ಪಕ್ಷ BRACE_COMPLEX + 4:
	  ಪಕ್ಷ BRACE_COMPLEX + 5:
	  ಪಕ್ಷ BRACE_COMPLEX + 6:
	  ಪಕ್ಷ BRACE_COMPLEX + 7:
	  ಪಕ್ಷ BRACE_COMPLEX + 8:
	  ಪಕ್ಷ BRACE_COMPLEX + 9:
	    {
		no = op - BRACE_COMPLEX;
		++brace_ಎಣಿಕೆ[no];

		// If not matched enough times yet, try one more
		ಯದಿ (brace_ಎಣಿಕೆ[no] <= (brace_min[no] <= brace_max[no]
					     ? brace_min[no] : brace_max[no]))
		{
		    rp = regstack_ತಳ್ಳು(RS_BRCPLX_MORE, scan);
		    ಯದಿ (rp == ರಿಕ್ತ)
			status = RA_FAIL;
		    ಅನ್ಯಥಾ
		    {
			rp->rs_no = no;
			reg_save(&rp->rs_un.regsave, &backpos);
			next = OPERAND(scan);
			// We continue and handle the result when done.
		    }
		    ಭಙ್ಗ;
		}

		// If matched enough times, may try matching some more
		ಯದಿ (brace_min[no] <= brace_max[no])
		{
		    // Range is the normal way around, use longest match
		    ಯದಿ (brace_ಎಣಿಕೆ[no] <= brace_max[no])
		    {
			rp = regstack_ತಳ್ಳು(RS_BRCPLX_LONG, scan);
			ಯದಿ (rp == ರಿಕ್ತ)
			    status = RA_FAIL;
			ಅನ್ಯಥಾ
			{
			    rp->rs_no = no;
			    reg_save(&rp->rs_un.regsave, &backpos);
			    next = OPERAND(scan);
			    // We continue and handle the result when done.
			}
		    }
		}
		ಅನ್ಯಥಾ
		{
		    // Range is backwards, use shortest match first
		    ಯದಿ (brace_ಎಣಿಕೆ[no] <= brace_min[no])
		    {
			rp = regstack_ತಳ್ಳು(RS_BRCPLX_SHORT, scan);
			ಯದಿ (rp == ರಿಕ್ತ)
			    status = RA_FAIL;
			ಅನ್ಯಥಾ
			{
			    reg_save(&rp->rs_un.regsave, &backpos);
			    // We continue and handle the result when done.
			}
		    }
		}
	    }
	    ಭಙ್ಗ;

	  ಪಕ್ಷ BRACE_SIMPLE:
	  ಪಕ್ಷ STAR:
	  ಪಕ್ಷ PLUS:
	    {
		regstar_T	rst;

		// Lookahead to avoid useless match attempts when we know
		// what character comes next.
		ಯದಿ (OP(next) == EXACTLY)
		{
		    rst.nextb = *OPERAND(next);
		    ಯದಿ (rex.reg_ic)
		    {
			ಯದಿ (MB_ISUPPER(rst.nextb))
			    rst.nextb_ic = MB_TOLOWER(rst.nextb);
			ಅನ್ಯಥಾ
			    rst.nextb_ic = MB_TOUPPER(rst.nextb);
		    }
		    ಅನ್ಯಥಾ
			rst.nextb_ic = rst.nextb;
		}
		ಅನ್ಯಥಾ
		{
		    rst.nextb = NUL;
		    rst.nextb_ic = NUL;
		}
		ಯದಿ (op != BRACE_SIMPLE)
		{
		    rst.minval = (op == STAR) ? 0 : 1;
		    rst.maxval = MAX_LIMIT;
		}
		ಅನ್ಯಥಾ
		{
		    rst.minval = bl_minval;
		    rst.maxval = bl_maxval;
		}

		// When maxval > minval, try matching as much as possible, up
		// to maxval.  When maxval < minval, try matching at least the
		// minimal number (since the range is backwards, that's also
		// maxval!).
		rst.ಎಣಿಕೆ = regrepeat(OPERAND(scan), rst.maxval);
		ಯದಿ (got_int)
		{
		    status = RA_FAIL;
		    ಭಙ್ಗ;
		}
		ಯದಿ (rst.minval <= rst.maxval
			  ? rst.ಎಣಿಕೆ >= rst.minval : rst.ಎಣಿಕೆ >= rst.maxval)
		{
		    // It could match.  Prepare for trying to match what
		    // follows.  The code is below.  Parameters are stored in
		    // a regstar_T on the regstack.
		    ಯದಿ ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
		    {
			emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
			status = RA_FAIL;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (GA_GROW_FAILS(&regstack, (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(regstar_T)))
			status = RA_FAIL;
		    ಅನ್ಯಥಾ
		    {
			regstack.ga_len += ಪರಿಮಾಣ(regstar_T);
			rp = regstack_ತಳ್ಳು(rst.minval <= rst.maxval
					? RS_STAR_LONG : RS_STAR_SHORT, scan);
			ಯದಿ (rp == ರಿಕ್ತ)
			    status = RA_FAIL;
			ಅನ್ಯಥಾ
			{
			    *(((regstar_T *)rp) - 1) = rst;
			    status = RA_BREAK;	    // skip the restore bits
			}
		    }
		}
		ಅನ್ಯಥಾ
		    status = RA_NOMATCH;

	    }
	    ಭಙ್ಗ;

	  ಪಕ್ಷ NOMATCH:
	  ಪಕ್ಷ MATCH:
	  ಪಕ್ಷ SUBPAT:
	    rp = regstack_ತಳ್ಳು(RS_NOMATCH, scan);
	    ಯದಿ (rp == ರಿಕ್ತ)
		status = RA_FAIL;
	    ಅನ್ಯಥಾ
	    {
		rp->rs_no = op;
		reg_save(&rp->rs_un.regsave, &backpos);
		next = OPERAND(scan);
		// We continue and handle the result when done.
	    }
	    ಭಙ್ಗ;

	  ಪಕ್ಷ BEHIND:
	  ಪಕ್ಷ NOBEHIND:
	    // Need a bit of room to store extra positions.
	    ಯದಿ ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
	    {
		emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		status = RA_FAIL;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (GA_GROW_FAILS(&regstack, (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(regbehind_T)))
		status = RA_FAIL;
	    ಅನ್ಯಥಾ
	    {
		regstack.ga_len += ಪರಿಮಾಣ(regbehind_T);
		rp = regstack_ತಳ್ಳು(RS_BEHIND1, scan);
		ಯದಿ (rp == ರಿಕ್ತ)
		    status = RA_FAIL;
		ಅನ್ಯಥಾ
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
	    ಭಙ್ಗ;

	  ಪಕ್ಷ BHPOS:
	    ಯದಿ (REG_MULTI)
	    {
		ಯದಿ (behind_pos.rs_u.pos.col != (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು)
			|| behind_pos.rs_u.pos.lnum != rex.lnum)
		    status = RA_NOMATCH;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (behind_pos.rs_u.ptr != rex.ಒಳಹರಿವು)
		status = RA_NOMATCH;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ NEWL:
	    ಯದಿ ((c != NUL || !REG_MULTI || rex.lnum > rex.reg_maxline
			     || rex.reg_ಸಾಲು_lbr)
					   && (c != '\n' || !rex.reg_ಸಾಲು_lbr))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ ಯದಿ (rex.reg_ಸಾಲು_lbr)
		ADVANCE_REGINPUT();
	    ಅನ್ಯಥಾ
		reg_nextline();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ END:
	    status = RA_MATCH;	// Success!
	    ಭಙ್ಗ;

	  ಸ್ವಭಾವ:
	    iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	    printf("Illegal op code %d\n", op);
#endif
	    status = RA_FAIL;
	    ಭಙ್ಗ;
	  }
	}

	// If we can't continue sequentially, break the inner loop.
	ಯದಿ (status != RA_CONT)
	    ಭಙ್ಗ;

	// Continue in inner loop, advance to next item.
	scan = next;

    } // end of inner loop

    // If there is something on the regstack execute the code for the state.
    // If the state is popped then loop and use the older state.
    ಯಾವತ್ (regstack.ga_len > 0 && status != RA_FAIL)
    {
	rp = (regitem_T *)((ವರ್ಣ *)regstack.ga_data + regstack.ga_len) - 1;
	ವಿಕಲ್ಪ (rp->rs_state)
	{
	  ಪಕ್ಷ RS_NOPEN:
	    // Result is passed on as-is, simply pop the state.
	    regstack_ತೆಗೆ(&scan);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ RS_MOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    ಯದಿ (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_startpos[rp->rs_no],
						  &rex.reg_startp[rp->rs_no]);
	    regstack_ತೆಗೆ(&scan);
	    ಭಙ್ಗ;

#ifdef FEAT_SYN_HL
	  ಪಕ್ಷ RS_ZOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    ಯದಿ (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_startzpos[rp->rs_no],
						 &reg_startzp[rp->rs_no]);
	    regstack_ತೆಗೆ(&scan);
	    ಭಙ್ಗ;
#endif

	  ಪಕ್ಷ RS_MCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    ಯದಿ (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_endpos[rp->rs_no],
						    &rex.reg_endp[rp->rs_no]);
	    regstack_ತೆಗೆ(&scan);
	    ಭಙ್ಗ;

#ifdef FEAT_SYN_HL
	  ಪಕ್ಷ RS_ZCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    ಯದಿ (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_endzpos[rp->rs_no],
						   &reg_endzp[rp->rs_no]);
	    regstack_ತೆಗೆ(&scan);
	    ಭಙ್ಗ;
#endif

	  ಪಕ್ಷ RS_BRANCH:
	    ಯದಿ (status == RA_MATCH)
		// this branch matched, use it
		regstack_ತೆಗೆ(&scan);
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (status != RA_BREAK)
		{
		    // After a non-matching branch: try next one.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    scan = rp->rs_scan;
		}
		ಯದಿ (scan == ರಿಕ್ತ || OP(scan) != BRANCH)
		{
		    // no more branches, didn't find a match
		    status = RA_NOMATCH;
		    regstack_ತೆಗೆ(&scan);
		}
		ಅನ್ಯಥಾ
		{
		    // Prepare to try a branch.
		    rp->rs_scan = regnext(scan);
		    reg_save(&rp->rs_un.regsave, &backpos);
		    scan = OPERAND(scan);
		}
	    }
	    ಭಙ್ಗ;

	  ಪಕ್ಷ RS_BRCPLX_MORE:
	    // Pop the state.  Restore pointers when there is no match.
	    ಯದಿ (status == RA_NOMATCH)
	    {
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_ಎಣಿಕೆ[rp->rs_no];	// decrement match count
	    }
	    regstack_ತೆಗೆ(&scan);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ RS_BRCPLX_LONG:
	    // Pop the state.  Restore pointers when there is no match.
	    ಯದಿ (status == RA_NOMATCH)
	    {
		// There was no match, but we did find enough matches.
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_ಎಣಿಕೆ[rp->rs_no];
		// continue with the items after "\{}"
		status = RA_CONT;
	    }
	    regstack_ತೆಗೆ(&scan);
	    ಯದಿ (status == RA_CONT)
		scan = regnext(scan);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ RS_BRCPLX_SHORT:
	    // Pop the state.  Restore pointers when there is no match.
	    ಯದಿ (status == RA_NOMATCH)
		// There was no match, try to match one more item.
		reg_restore(&rp->rs_un.regsave, &backpos);
	    regstack_ತೆಗೆ(&scan);
	    ಯದಿ (status == RA_NOMATCH)
	    {
		scan = OPERAND(scan);
		status = RA_CONT;
	    }
	    ಭಙ್ಗ;

	  ಪಕ್ಷ RS_NOMATCH:
	    // Pop the state.  If the operand matches for NOMATCH or
	    // doesn't match for MATCH/SUBPAT, we fail.  Otherwise backup,
	    // except for SUBPAT, and continue with the next item.
	    ಯದಿ (status == (rp->rs_no == NOMATCH ? RA_MATCH : RA_NOMATCH))
		status = RA_NOMATCH;
	    ಅನ್ಯಥಾ
	    {
		status = RA_CONT;
		ಯದಿ (rp->rs_no != SUBPAT)	// zero-width
		    reg_restore(&rp->rs_un.regsave, &backpos);
	    }
	    regstack_ತೆಗೆ(&scan);
	    ಯದಿ (status == RA_CONT)
		scan = regnext(scan);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ RS_BEHIND1:
	    ಯದಿ (status == RA_NOMATCH)
	    {
		regstack_ತೆಗೆ(&scan);
		regstack.ga_len -= ಪರಿಮಾಣ(regbehind_T);
	    }
	    ಅನ್ಯಥಾ
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
	    ಭಙ್ಗ;

	  ಪಕ್ಷ RS_BEHIND2:
	    // Looping for BEHIND / NOBEHIND match.
	    ಯದಿ (status == RA_MATCH && reg_save_equal(&behind_pos))
	    {
		// found a match that ends where "next" started
		behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		ಯದಿ (rp->rs_no == BEHIND)
		    reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
		ಅನ್ಯಥಾ
		{
		    // But we didn't want a match.  Need to restore the
		    // subexpr, because what follows matched, so they have
		    // been set.
		    status = RA_NOMATCH;
		    restore_subexpr(((regbehind_T *)rp) - 1);
		}
		regstack_ತೆಗೆ(&scan);
		regstack.ga_len -= ಪರಿಮಾಣ(regbehind_T);
	    }
	    ಅನ್ಯಥಾ
	    {
		long limit;

		// No match or a match that doesn't end where we want it: Go
		// back one character.  May go to previous line once.
		no = OK;
		limit = OPERAND_MIN(rp->rs_scan);
		ಯದಿ (REG_MULTI)
		{
		    ಯದಿ (limit > 0
			    && ((rp->rs_un.regsave.rs_u.pos.lnum
						    < behind_pos.rs_u.pos.lnum
				    ? (colnr_T)STRLEN(rex.ಸಾಲು)
				    : behind_pos.rs_u.pos.col)
				- rp->rs_un.regsave.rs_u.pos.col >= limit))
			no = FAIL;
		    ಅನ್ಯಥಾ ಯದಿ (rp->rs_un.regsave.rs_u.pos.col == 0)
		    {
			ಯದಿ (rp->rs_un.regsave.rs_u.pos.lnum
					< behind_pos.rs_u.pos.lnum
				|| reg_getline(
					--rp->rs_un.regsave.rs_u.pos.lnum)
								  == ರಿಕ್ತ)
			    no = FAIL;
			ಅನ್ಯಥಾ
			{
			    reg_restore(&rp->rs_un.regsave, &backpos);
			    rp->rs_un.regsave.rs_u.pos.col =
						 (colnr_T)STRLEN(rex.ಸಾಲು);
			}
		    }
		    ಅನ್ಯಥಾ
		    {
			ಯದಿ (has_mbyte)
			{
			    char_u *ಸಾಲು =
				  reg_getline(rp->rs_un.regsave.rs_u.pos.lnum);

			    rp->rs_un.regsave.rs_u.pos.col -=
				(*mb_head_off)(ಸಾಲು, ಸಾಲು
				    + rp->rs_un.regsave.rs_u.pos.col - 1) + 1;
			}
			ಅನ್ಯಥಾ
			    --rp->rs_un.regsave.rs_u.pos.col;
		    }
		}
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (rp->rs_un.regsave.rs_u.ptr == rex.ಸಾಲು)
			no = FAIL;
		    ಅನ್ಯಥಾ
		    {
			MB_PTR_BACK(rex.ಸಾಲು, rp->rs_un.regsave.rs_u.ptr);
			ಯದಿ (limit > 0 && (long)(behind_pos.rs_u.ptr
				     - rp->rs_un.regsave.rs_u.ptr) > limit)
			    no = FAIL;
		    }
		}
		ಯದಿ (no == OK)
		{
		    // Advanced, prepare for finding match again.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    scan = OPERAND(rp->rs_scan) + 4;
		    ಯದಿ (status == RA_MATCH)
		    {
			// We did match, so subexpr may have been changed,
			// need to restore them for the next try.
			status = RA_NOMATCH;
			restore_subexpr(((regbehind_T *)rp) - 1);
		    }
		}
		ಅನ್ಯಥಾ
		{
		    // Can't advance.  For NOBEHIND that's a match.
		    behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		    ಯದಿ (rp->rs_no == NOBEHIND)
		    {
			reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
			status = RA_MATCH;
		    }
		    ಅನ್ಯಥಾ
		    {
			// We do want a proper match.  Need to restore the
			// subexpr if we had a match, because they may have
			// been set.
			ಯದಿ (status == RA_MATCH)
			{
			    status = RA_NOMATCH;
			    restore_subexpr(((regbehind_T *)rp) - 1);
			}
		    }
		    regstack_ತೆಗೆ(&scan);
		    regstack.ga_len -= ಪರಿಮಾಣ(regbehind_T);
		}
	    }
	    ಭಙ್ಗ;

	  ಪಕ್ಷ RS_STAR_LONG:
	  ಪಕ್ಷ RS_STAR_SHORT:
	    {
		regstar_T	    *rst = ((regstar_T *)rp) - 1;

		ಯದಿ (status == RA_MATCH)
		{
		    regstack_ತೆಗೆ(&scan);
		    regstack.ga_len -= ಪರಿಮಾಣ(regstar_T);
		    ಭಙ್ಗ;
		}

		// Tried once already, restore input pointers.
		ಯದಿ (status != RA_BREAK)
		    reg_restore(&rp->rs_un.regsave, &backpos);

		// Repeat until we found a position where it could match.
		ಪ್ರತಿ (;;)
		{
		    ಯದಿ (status != RA_BREAK)
		    {
			// Tried first position already, advance.
			ಯದಿ (rp->rs_state == RS_STAR_LONG)
			{
			    // Trying for longest match, but couldn't or
			    // didn't match -- back up one char.
			    ಯದಿ (--rst->ಎಣಿಕೆ < rst->minval)
				ಭಙ್ಗ;
			    ಯದಿ (rex.ಒಳಹರಿವು == rex.ಸಾಲು)
			    {
				// backup to last char of previous line
				ಯದಿ (rex.lnum == 0)
				{
				    status = RA_NOMATCH;
				    ಭಙ್ಗ;
				}
				--rex.lnum;
				rex.ಸಾಲು = reg_getline(rex.lnum);
				// Just in case regrepeat() didn't count
				// right.
				ಯದಿ (rex.ಸಾಲು == ರಿಕ್ತ)
				    ಭಙ್ಗ;
				rex.ಒಳಹರಿವು = rex.ಸಾಲು + reg_getline_len(rex.lnum);
				fast_breakcheck();
			    }
			    ಅನ್ಯಥಾ
				MB_PTR_BACK(rex.ಸಾಲು, rex.ಒಳಹರಿವು);
			}
			ಅನ್ಯಥಾ
			{
			    // Range is backwards, use shortest match first.
			    // Careful: maxval and minval are exchanged!
			    // Couldn't or didn't match: try advancing one
			    // char.
			    ಯದಿ (rst->ಎಣಿಕೆ == rst->minval
				  || regrepeat(OPERAND(rp->rs_scan), 1L) == 0)
				ಭಙ್ಗ;
			    ++rst->ಎಣಿಕೆ;
			}
			ಯದಿ (got_int)
			    ಭಙ್ಗ;
		    }
		    ಅನ್ಯಥಾ
			status = RA_NOMATCH;

		    // If it could match, try it.
		    ಯದಿ (rst->nextb == NUL || *rex.ಒಳಹರಿವು == rst->nextb
					     || *rex.ಒಳಹರಿವು == rst->nextb_ic)
		    {
			reg_save(&rp->rs_un.regsave, &backpos);
			scan = regnext(rp->rs_scan);
			status = RA_CONT;
			ಭಙ್ಗ;
		    }
		}
		ಯದಿ (status != RA_CONT)
		{
		    // Failed.
		    regstack_ತೆಗೆ(&scan);
		    regstack.ga_len -= ಪರಿಮಾಣ(regstar_T);
		    status = RA_NOMATCH;
		}
	    }
	    ಭಙ್ಗ;
	}

	// If we want to continue the inner loop or didn't pop a state
	// continue matching loop
	ಯದಿ (status == RA_CONT || rp == (regitem_T *)
			     ((ವರ್ಣ *)regstack.ga_data + regstack.ga_len) - 1)
	    ಭಙ್ಗ;

#ifdef FEAT_RELTIME
	ಯದಿ (bt_did_time_out(timed_out))
	{
	    status = RA_FAIL;
	    ಭಙ್ಗ;
	}
#endif
    }

    // May need to continue with the inner loop, starting at "scan".
    ಯದಿ (status == RA_CONT)
	ಅನುವರ್ತ;

    // If the regstack is empty or something failed we are done.
    ಯದಿ (regstack.ga_len == 0 || status == RA_FAIL)
    {
	ಯದಿ (scan == ರಿಕ್ತ)
	{
	    // We get here only if there's trouble -- normally "case END" is
	    // the terminating point.
	    iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	    printf("Premature EOL\n");
#endif
	}
	ಪ್ರತ್ಯಾಗಮ (status == RA_MATCH);
    }

  } // End of loop until the regstack is empty.

  // NOTREACHED
}

/*
 * regtry - try match of "prog" with at rex.line["col"].
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    ಸ್ಥಿರ long
regtry(
    bt_regprog_T	*prog,
    colnr_T		col,
    ಪೂರ್ಣಾಙ್ಕ			*timed_out)	// flag set on timeout or NULL
{
    rex.ಒಳಹರಿವು = rex.ಸಾಲು + col;
    rex.need_clear_subexpr = TRUE;
#ifdef FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    rex.need_clear_zsubexpr = (prog->reghasz == REX_SET);
#endif

    ಯದಿ (regmatch(prog->program + 1, timed_out) == 0)
	ಪ್ರತ್ಯಾಗಮ 0;

    cleanup_subexpr();
    ಯದಿ (REG_MULTI)
    {
	ಯದಿ (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].col = col;
	}
	ಯದಿ (rex.reg_endpos[0].lnum < 0)
	{
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].col = (ಪೂರ್ಣಾಙ್ಕ)(rex.ಒಳಹರಿವು - rex.ಸಾಲು);
	}
	ಅನ್ಯಥಾ
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (rex.reg_startp[0] == ರಿಕ್ತ)
	    rex.reg_startp[0] = rex.ಸಾಲು + col;
	ಯದಿ (rex.reg_endp[0] == ರಿಕ್ತ)
	    rex.reg_endp[0] = rex.ಒಳಹರಿವು;
    }
#ifdef FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = ರಿಕ್ತ;

    ಯದಿ (prog->reghasz == REX_SET)
    {
	ಪೂರ್ಣಾಙ್ಕ		i;

	cleanup_zsubexpr();
	re_extmatch_out = make_extmatch();
	ಯದಿ (re_extmatch_out == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ 0;
	ಪ್ರತಿ (i = 0; i < NSUBEXP; i++)
	{
	    ಯದಿ (REG_MULTI)
	    {
		// Only accept single line matches.
		ಯದಿ (reg_startzpos[i].lnum >= 0
			&& reg_endzpos[i].lnum == reg_startzpos[i].lnum
			&& reg_endzpos[i].col >= reg_startzpos[i].col)
		    re_extmatch_out->matches[i] =
			vim_strnsave(reg_getline(reg_startzpos[i].lnum)
						       + reg_startzpos[i].col,
				   reg_endzpos[i].col - reg_startzpos[i].col);
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (reg_startzp[i] != ರಿಕ್ತ && reg_endzp[i] != ರಿಕ್ತ)
		    re_extmatch_out->matches[i] =
			    vim_strnsave(reg_startzp[i],
						reg_endzp[i] - reg_startzp[i]);
	    }
	}
    }
#endif
    ಪ್ರತ್ಯಾಗಮ 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    ಸ್ಥಿರ long
bt_regexec_both(
    char_u	*ಸಾಲು,
    colnr_T	startcol,	// column to start looking for match
    ಪೂರ್ಣಾಙ್ಕ		*timed_out)	// flag set on timeout or NULL
{
    bt_regprog_T    *prog;
    char_u	    *s;
    colnr_T	    col = startcol;
    long	    retval = 0L;

    // Create "regstack" and "backpos" if they are not allocated yet.
    // We allocate *_INITIAL amount of bytes first and then set the grow size
    // to much bigger value to avoid many malloc calls in case of deep regular
    // expressions.
    ಯದಿ (regstack.ga_data == ರಿಕ್ತ)
    {
	// Use an item size of 1 byte, since we push different things
	// onto the regstack.
	ga_init2(&regstack, 1, REGSTACK_INITIAL);
	(ಶೂನ್ಯ)ga_grow(&regstack, REGSTACK_INITIAL);
	regstack.ga_growsize = REGSTACK_INITIAL * 8;
    }

    ಯದಿ (backpos.ga_data == ರಿಕ್ತ)
    {
	ga_init2(&backpos, ಪರಿಮಾಣ(backpos_T), BACKPOS_INITIAL);
	(ಶೂನ್ಯ)ga_grow(&backpos, BACKPOS_INITIAL);
	backpos.ga_growsize = BACKPOS_INITIAL * 8;
    }

    ಯದಿ (REG_MULTI)
    {
	prog = (bt_regprog_T *)rex.reg_mmatch->regprog;
	ಸಾಲು = reg_getline((linenr_T)0);
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    ಅನ್ಯಥಾ
    {
	prog = (bt_regprog_T *)rex.reg_match->regprog;
	rex.reg_startp = rex.reg_match->startp;
	rex.reg_endp = rex.reg_match->endp;
    }

    // Be paranoid...
    ಯದಿ (prog == ರಿಕ್ತ || ಸಾಲು == ರಿಕ್ತ)
    {
	iemsg(e_null_argument);
	ಗಚ್ಛ theend;
    }

    // Check validity of program.
    ಯದಿ (prog_magic_wrong())
	ಗಚ್ಛ theend;

    // If the start column is past the maximum column: no need to try.
    ಯದಿ (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	ಗಚ್ಛ theend;

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    ಯದಿ (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    ಅನ್ಯಥಾ ಯದಿ (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    ಯದಿ (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    // If there is a "must appear" string, look for it.
    ಯದಿ (prog->regmust != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ c;

	ಯದಿ (has_mbyte)
	    c = (*mb_ptr2char)(prog->regmust);
	ಅನ್ಯಥಾ
	    c = *prog->regmust;
	s = ಸಾಲು + col;

	// This is used very often, esp. for ":global".  Use three versions of
	// the loop to avoid overhead of conditions.
	ಯದಿ (!rex.reg_ic && !has_mbyte)
	    ಯಾವತ್ ((s = vim_strbyte(s, c)) != ರಿಕ್ತ)
	    {
		ಪೂರ್ಣಾಙ್ಕ len = prog->regmlen;
		ಯದಿ (cstrncmp(s, prog->regmust, &len) == 0)
		    ಭಙ್ಗ;		// Found it.
		++s;
	    }
	ಅನ್ಯಥಾ ಯದಿ (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	    ಯಾವತ್ ((s = vim_strchr(s, c)) != ರಿಕ್ತ)
	    {
		ಪೂರ್ಣಾಙ್ಕ len = prog->regmlen;
		ಯದಿ (cstrncmp(s, prog->regmust, &len) == 0)
		    ಭಙ್ಗ;		// Found it.
		MB_PTR_ADV(s);
	    }
	ಅನ್ಯಥಾ
	    ಯಾವತ್ ((s = cstrchr(s, c)) != ರಿಕ್ತ)
	    {
		ಪೂರ್ಣಾಙ್ಕ len = prog->regmlen;
		ಯದಿ (cstrncmp(s, prog->regmust, &len) == 0)
		    ಭಙ್ಗ;		// Found it.
		MB_PTR_ADV(s);
	    }
	ಯದಿ (s == ರಿಕ್ತ)		// Not present.
	    ಗಚ್ಛ theend;
    }

    rex.ಸಾಲು = ಸಾಲು;
    rex.lnum = 0;
    reg_toolong = FALSE;

    // Simplest case: Anchored match need be tried only once.
    ಯದಿ (prog->reganch)
    {
	ಪೂರ್ಣಾಙ್ಕ	c;

	ಯದಿ (has_mbyte)
	    c = (*mb_ptr2char)(rex.ಸಾಲು + col);
	ಅನ್ಯಥಾ
	    c = rex.ಸಾಲು[col];
	ಯದಿ (prog->regstart == NUL
		|| prog->regstart == c
		|| (rex.reg_ic
		    && (((enc_utf8 && utf_fold(prog->regstart) == utf_fold(c)))
			|| (c < 255 && prog->regstart < 255 &&
			    MB_TOLOWER(prog->regstart) == MB_TOLOWER(c)))))
	    retval = regtry(prog, col, timed_out);
	ಅನ್ಯಥಾ
	    retval = 0;
    }
    ಅನ್ಯಥಾ
    {
	// Messy cases:  unanchored match.
	ಯಾವತ್ (!got_int)
	{
	    ಯದಿ (prog->regstart != NUL)
	    {
		// Skip until the char we know it must start with.
		// Used often, do some work to avoid call overhead.
		ಯದಿ (!rex.reg_ic && !has_mbyte)
		    s = vim_strbyte(rex.ಸಾಲು + col, prog->regstart);
		ಅನ್ಯಥಾ
		    s = cstrchr(rex.ಸಾಲು + col, prog->regstart);
		ಯದಿ (s == ರಿಕ್ತ)
		{
		    retval = 0;
		    ಭಙ್ಗ;
		}
		col = (ಪೂರ್ಣಾಙ್ಕ)(s - rex.ಸಾಲು);
	    }

	    // Check for maximum column to try.
	    ಯದಿ (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	    {
		retval = 0;
		ಭಙ್ಗ;
	    }

	    retval = regtry(prog, col, timed_out);
	    ಯದಿ (retval > 0)
		ಭಙ್ಗ;

	    // if not currently on the first line, get it again
	    ಯದಿ (rex.lnum != 0)
	    {
		rex.lnum = 0;
		rex.ಸಾಲು = reg_getline((linenr_T)0);
	    }
	    ಯದಿ (rex.ಸಾಲು[col] == NUL)
		ಭಙ್ಗ;
	    ಯದಿ (has_mbyte)
		col += (*mb_ptr2len)(rex.ಸಾಲು + col);
	    ಅನ್ಯಥಾ
		++col;
#ifdef FEAT_RELTIME
	    ಯದಿ (bt_did_time_out(timed_out))
		ಭಙ್ಗ;
#endif
	}
    }

theend:
    // Free "reg_tofree" when it's a bit big.
    // Free regstack and backpos if they are bigger than their initial size.
    ಯದಿ (reg_tofreelen > 400)
	VIM_CLEAR(reg_tofree);
    ಯದಿ (regstack.ga_maxlen > REGSTACK_INITIAL)
	ga_clear(&regstack);
    ಯದಿ (backpos.ga_maxlen > BACKPOS_INITIAL)
	ga_clear(&backpos);

    ಯದಿ (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs
	// and \ze are used.
	ಯದಿ (REG_MULTI)
	{
	    lpos_T *start = &rex.reg_mmatch->startpos[0];
	    lpos_T *end = &rex.reg_mmatch->endpos[0];

	    ಯದಿ (end->lnum < start->lnum
			|| (end->lnum == start->lnum && end->col < start->col))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_mmatch->rmm_matchcol = col;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (rex.reg_match->endp[0] < rex.reg_match->startp[0])
		rex.reg_match->endp[0] = rex.reg_match->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_match->rm_matchcol = col;
	}
    }

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Match a regexp against a string.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 * if "line_lbr" is TRUE  consider a "\n" in "line" to be a line break.
 *
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
bt_regexec_nl(
    regmatch_T	*rmp,
    char_u	*ಸಾಲು,	// string to match against
    colnr_T	col,	// column to start looking for match
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು_lbr)
{
    rex.reg_match = rmp;
    rex.reg_mmatch = ರಿಕ್ತ;
    rex.reg_maxline = 0;
    rex.reg_ಸಾಲು_lbr = ಸಾಲು_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = ರಿಕ್ತ;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;

    ಪ್ರತ್ಯಾಗಮ bt_regexec_both(ಸಾಲು, col, ರಿಕ್ತ);
}

/*
 * Match a regexp against multiple lines.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 *
 * Return zero if there is no match.  Return number of lines contained in the
 * match otherwise.
 */
    ಸ್ಥಿರ long
bt_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	col,		// column to start looking for match
    ಪೂರ್ಣಾಙ್ಕ		*timed_out)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    ಪ್ರತ್ಯಾಗಮ bt_regexec_both(ರಿಕ್ತ, col, timed_out);
}

/*
 * Compare a number with the operand of RE_LNUM, RE_COL or RE_VCOL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
re_ಅಂಕೆ_cmp(long_u val, char_u *scan)
{
    long_u  n = OPERAND_MIN(scan);

    ಯದಿ (OPERAND_CMP(scan) == '>')
	ಪ್ರತ್ಯಾಗಮ val > n;
    ಯದಿ (OPERAND_CMP(scan) == '<')
	ಪ್ರತ್ಯಾಗಮ val < n;
    ಪ್ರತ್ಯಾಗಮ val == n;
}

#ifdef BT_REGEXP_DUMP

/*
 * regdump - dump a regexp onto stdout in vaguely comprehensible form
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
regdump(char_u *pattern, bt_regprog_T *r)
{
    char_u  *s;
    ಪೂರ್ಣಾಙ್ಕ	    op = EXACTLY;	// Arbitrary non-END op.
    char_u  *next;
    char_u  *end = ರಿಕ್ತ;
    FILE    *f;

# ifdef BT_REGEXP_LOG
    f = fopen("bt_regexp_log.log", "a");
# ಅನ್ಯಥಾ
    f = stdout;
# endif
    ಯದಿ (f == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    fprintf(f, "-------------------------------------\n\r\nregcomp(%s):\r\n", pattern);

    s = r->program + 1;
    // Loop until we find the END that isn't before a referred next (an END
    // can also appear in a NOMATCH operand).
    ಯಾವತ್ ((op != END || s <= end) && s < r->program + r->regsz)
    {
	op = OP(s);
	fprintf(f, "%2d%s", (ಪೂರ್ಣಾಙ್ಕ)(s - r->program), regprop(s)); // Where, what.
	next = (s + 3 <= r->program + r->regsz) ? regnext(s) : ರಿಕ್ತ;
	ಯದಿ (next == ರಿಕ್ತ)	// Next ptr.
	    fprintf(f, "(0)");
	ಅನ್ಯಥಾ
	    fprintf(f, "(%d)", (ಪೂರ್ಣಾಙ್ಕ)((s - r->program) + (next - s)));
	ಯದಿ (end < next)
	    end = next;
	ಯದಿ (op == BRACE_LIMITS)
	{
	    // Two ints
	    fprintf(f, " minval %ld, maxval %ld", OPERAND_MIN(s), OPERAND_MAX(s));
	    s += 8;
	}
	ಅನ್ಯಥಾ ಯದಿ (op == BEHIND || op == NOBEHIND)
	{
	    // one int
	    fprintf(f, " count %ld", OPERAND_MIN(s));
	    s += 4;
	}
	ಅನ್ಯಥಾ ಯದಿ (op == RE_LNUM || op == RE_COL || op == RE_VCOL)
	{
	    // one int plus comparator
	    fprintf(f, " count %ld", OPERAND_MIN(s));
	    s += 5;
	}
	s += 3;
	ಯದಿ (op == MULTIBYTECODE)
	{
	    fprintf(f, " mbc=%d", utf_ptr2char(s));
	    s += utfc_ptr2len(s);
	}
	ಯದಿ (op == ANYOF || op == ANYOF + ADD_NL
		|| op == ANYBUT || op == ANYBUT + ADD_NL
		|| op == EXACTLY)
	{
	    // Literal string, where present.
	    fprintf(f, "\nxxxxxxxxx\n");
	    ಯಾವತ್ (*s != NUL && s < r->program + r->regsz)
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
    ಯದಿ (r->regstart != NUL)
	fprintf(f, "start `%s' 0x%x; ", r->regstart < 256
		? (ವರ್ಣ *)transchar(r->regstart)
		: "multibyte", r->regstart);
    ಯದಿ (r->reganch)
	fprintf(f, "anchored; ");
    ಯದಿ (r->regmust != ರಿಕ್ತ)
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
    ಸ್ಥಿರ char_u *
regprop(char_u *op)
{
    ವರ್ಣ	    *p;
    ಸ್ಥಿರ ವರ್ಣ	    buf[50];
    ಸ್ಥಿರ size_t   buflen = 0;

    STRCPY(buf, ":");
    buflen = 1;

    ವಿಕಲ್ಪ ((ಪೂರ್ಣಾಙ್ಕ) OP(op))
    {
      ಪಕ್ಷ BOL:
	p = "BOL";
	ಭಙ್ಗ;
      ಪಕ್ಷ EOL:
	p = "EOL";
	ಭಙ್ಗ;
      ಪಕ್ಷ RE_BOF:
	p = "BOF";
	ಭಙ್ಗ;
      ಪಕ್ಷ RE_EOF:
	p = "EOF";
	ಭಙ್ಗ;
      ಪಕ್ಷ CURSOR:
	p = "CURSOR";
	ಭಙ್ಗ;
      ಪಕ್ಷ RE_VISUAL:
	p = "RE_VISUAL";
	ಭಙ್ಗ;
      ಪಕ್ಷ RE_LNUM:
	p = "RE_LNUM";
	ಭಙ್ಗ;
      ಪಕ್ಷ RE_MARK:
	p = "RE_MARK";
	ಭಙ್ಗ;
      ಪಕ್ಷ RE_COL:
	p = "RE_COL";
	ಭಙ್ಗ;
      ಪಕ್ಷ RE_VCOL:
	p = "RE_VCOL";
	ಭಙ್ಗ;
      ಪಕ್ಷ BOW:
	p = "BOW";
	ಭಙ್ಗ;
      ಪಕ್ಷ EOW:
	p = "EOW";
	ಭಙ್ಗ;
      ಪಕ್ಷ ANY:
	p = "ANY";
	ಭಙ್ಗ;
      ಪಕ್ಷ ANY + ADD_NL:
	p = "ANY+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ ANYOF:
	p = "ANYOF";
	ಭಙ್ಗ;
      ಪಕ್ಷ ANYOF + ADD_NL:
	p = "ANYOF+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ ANYBUT:
	p = "ANYBUT";
	ಭಙ್ಗ;
      ಪಕ್ಷ ANYBUT + ADD_NL:
	p = "ANYBUT+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ IDENT:
	p = "IDENT";
	ಭಙ್ಗ;
      ಪಕ್ಷ IDENT + ADD_NL:
	p = "IDENT+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ SIDENT:
	p = "SIDENT";
	ಭಙ್ಗ;
      ಪಕ್ಷ SIDENT + ADD_NL:
	p = "SIDENT+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ KWORD:
	p = "KWORD";
	ಭಙ್ಗ;
      ಪಕ್ಷ KWORD + ADD_NL:
	p = "KWORD+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ SKWORD:
	p = "SKWORD";
	ಭಙ್ಗ;
      ಪಕ್ಷ SKWORD + ADD_NL:
	p = "SKWORD+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ FNAME:
	p = "FNAME";
	ಭಙ್ಗ;
      ಪಕ್ಷ FNAME + ADD_NL:
	p = "FNAME+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ SFNAME:
	p = "SFNAME";
	ಭಙ್ಗ;
      ಪಕ್ಷ SFNAME + ADD_NL:
	p = "SFNAME+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ PRINT:
	p = "PRINT";
	ಭಙ್ಗ;
      ಪಕ್ಷ PRINT + ADD_NL:
	p = "PRINT+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ SPRINT:
	p = "SPRINT";
	ಭಙ್ಗ;
      ಪಕ್ಷ SPRINT + ADD_NL:
	p = "SPRINT+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ WHITE:
	p = "WHITE";
	ಭಙ್ಗ;
      ಪಕ್ಷ WHITE + ADD_NL:
	p = "WHITE+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ NWHITE:
	p = "NWHITE";
	ಭಙ್ಗ;
      ಪಕ್ಷ NWHITE + ADD_NL:
	p = "NWHITE+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ DIGIT:
	p = "DIGIT";
	ಭಙ್ಗ;
      ಪಕ್ಷ DIGIT + ADD_NL:
	p = "DIGIT+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ NDIGIT:
	p = "NDIGIT";
	ಭಙ್ಗ;
      ಪಕ್ಷ NDIGIT + ADD_NL:
	p = "NDIGIT+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ HEX:
	p = "HEX";
	ಭಙ್ಗ;
      ಪಕ್ಷ HEX + ADD_NL:
	p = "HEX+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ NHEX:
	p = "NHEX";
	ಭಙ್ಗ;
      ಪಕ್ಷ NHEX + ADD_NL:
	p = "NHEX+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ OCTAL:
	p = "OCTAL";
	ಭಙ್ಗ;
      ಪಕ್ಷ OCTAL + ADD_NL:
	p = "OCTAL+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ NOCTAL:
	p = "NOCTAL";
	ಭಙ್ಗ;
      ಪಕ್ಷ NOCTAL + ADD_NL:
	p = "NOCTAL+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ WORD:
	p = "WORD";
	ಭಙ್ಗ;
      ಪಕ್ಷ WORD + ADD_NL:
	p = "WORD+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ NWORD:
	p = "NWORD";
	ಭಙ್ಗ;
      ಪಕ್ಷ NWORD + ADD_NL:
	p = "NWORD+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ HEAD:
	p = "HEAD";
	ಭಙ್ಗ;
      ಪಕ್ಷ HEAD + ADD_NL:
	p = "HEAD+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ NHEAD:
	p = "NHEAD";
	ಭಙ್ಗ;
      ಪಕ್ಷ NHEAD + ADD_NL:
	p = "NHEAD+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ ALPHA:
	p = "ALPHA";
	ಭಙ್ಗ;
      ಪಕ್ಷ ALPHA + ADD_NL:
	p = "ALPHA+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ NALPHA:
	p = "NALPHA";
	ಭಙ್ಗ;
      ಪಕ್ಷ NALPHA + ADD_NL:
	p = "NALPHA+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ LOWER:
	p = "LOWER";
	ಭಙ್ಗ;
      ಪಕ್ಷ LOWER + ADD_NL:
	p = "LOWER+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ NLOWER:
	p = "NLOWER";
	ಭಙ್ಗ;
      ಪಕ್ಷ NLOWER + ADD_NL:
	p = "NLOWER+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ UPPER:
	p = "UPPER";
	ಭಙ್ಗ;
      ಪಕ್ಷ UPPER + ADD_NL:
	p = "UPPER+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ NUPPER:
	p = "NUPPER";
	ಭಙ್ಗ;
      ಪಕ್ಷ NUPPER + ADD_NL:
	p = "NUPPER+NL";
	ಭಙ್ಗ;
      ಪಕ್ಷ BRANCH:
	p = "BRANCH";
	ಭಙ್ಗ;
      ಪಕ್ಷ EXACTLY:
	p = "EXACTLY";
	ಭಙ್ಗ;
      ಪಕ್ಷ NOTHING:
	p = "NOTHING";
	ಭಙ್ಗ;
      ಪಕ್ಷ BACK:
	p = "BACK";
	ಭಙ್ಗ;
      ಪಕ್ಷ END:
	p = "END";
	ಭಙ್ಗ;
      ಪಕ್ಷ MOPEN + 0:
	p = "MATCH START";
	ಭಙ್ಗ;
      ಪಕ್ಷ MOPEN + 1:
      ಪಕ್ಷ MOPEN + 2:
      ಪಕ್ಷ MOPEN + 3:
      ಪಕ್ಷ MOPEN + 4:
      ಪಕ್ಷ MOPEN + 5:
      ಪಕ್ಷ MOPEN + 6:
      ಪಕ್ಷ MOPEN + 7:
      ಪಕ್ಷ MOPEN + 8:
      ಪಕ್ಷ MOPEN + 9:
	buflen += vim_snprintf(buf + buflen, ಪರಿಮಾಣ(buf) - buflen, "MOPEN%d", OP(op) - MOPEN);
	p = ರಿಕ್ತ;
	ಭಙ್ಗ;
      ಪಕ್ಷ MCLOSE + 0:
	p = "MATCH END";
	ಭಙ್ಗ;
      ಪಕ್ಷ MCLOSE + 1:
      ಪಕ್ಷ MCLOSE + 2:
      ಪಕ್ಷ MCLOSE + 3:
      ಪಕ್ಷ MCLOSE + 4:
      ಪಕ್ಷ MCLOSE + 5:
      ಪಕ್ಷ MCLOSE + 6:
      ಪಕ್ಷ MCLOSE + 7:
      ಪಕ್ಷ MCLOSE + 8:
      ಪಕ್ಷ MCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, ಪರಿಮಾಣ(buf) - buflen, "MCLOSE%d", OP(op) - MCLOSE);
	p = ರಿಕ್ತ;
	ಭಙ್ಗ;
      ಪಕ್ಷ BACKREF + 1:
      ಪಕ್ಷ BACKREF + 2:
      ಪಕ್ಷ BACKREF + 3:
      ಪಕ್ಷ BACKREF + 4:
      ಪಕ್ಷ BACKREF + 5:
      ಪಕ್ಷ BACKREF + 6:
      ಪಕ್ಷ BACKREF + 7:
      ಪಕ್ಷ BACKREF + 8:
      ಪಕ್ಷ BACKREF + 9:
	buflen += vim_snprintf(buf + buflen, ಪರಿಮಾಣ(buf) - buflen, "BACKREF%d", OP(op) - BACKREF);
	p = ರಿಕ್ತ;
	ಭಙ್ಗ;
      ಪಕ್ಷ NOPEN:
	p = "NOPEN";
	ಭಙ್ಗ;
      ಪಕ್ಷ NCLOSE:
	p = "NCLOSE";
	ಭಙ್ಗ;
# ifdef FEAT_SYN_HL
      ಪಕ್ಷ ZOPEN + 1:
      ಪಕ್ಷ ZOPEN + 2:
      ಪಕ್ಷ ZOPEN + 3:
      ಪಕ್ಷ ZOPEN + 4:
      ಪಕ್ಷ ZOPEN + 5:
      ಪಕ್ಷ ZOPEN + 6:
      ಪಕ್ಷ ZOPEN + 7:
      ಪಕ್ಷ ZOPEN + 8:
      ಪಕ್ಷ ZOPEN + 9:
	buflen += vim_snprintf(buf + buflen, ಪರಿಮಾಣ(buf) - buflen, "ZOPEN%d", OP(op) - ZOPEN);
	p = ರಿಕ್ತ;
	ಭಙ್ಗ;
      ಪಕ್ಷ ZCLOSE + 1:
      ಪಕ್ಷ ZCLOSE + 2:
      ಪಕ್ಷ ZCLOSE + 3:
      ಪಕ್ಷ ZCLOSE + 4:
      ಪಕ್ಷ ZCLOSE + 5:
      ಪಕ್ಷ ZCLOSE + 6:
      ಪಕ್ಷ ZCLOSE + 7:
      ಪಕ್ಷ ZCLOSE + 8:
      ಪಕ್ಷ ZCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, ಪರಿಮಾಣ(buf) - buflen, "ZCLOSE%d", OP(op) - ZCLOSE);
	p = ರಿಕ್ತ;
	ಭಙ್ಗ;
      ಪಕ್ಷ ZREF + 1:
      ಪಕ್ಷ ZREF + 2:
      ಪಕ್ಷ ZREF + 3:
      ಪಕ್ಷ ZREF + 4:
      ಪಕ್ಷ ZREF + 5:
      ಪಕ್ಷ ZREF + 6:
      ಪಕ್ಷ ZREF + 7:
      ಪಕ್ಷ ZREF + 8:
      ಪಕ್ಷ ZREF + 9:
	buflen += vim_snprintf(buf + buflen, ಪರಿಮಾಣ(buf) - buflen, "ZREF%d", OP(op) - ZREF);
	p = ರಿಕ್ತ;
	ಭಙ್ಗ;
# endif
      ಪಕ್ಷ STAR:
	p = "STAR";
	ಭಙ್ಗ;
      ಪಕ್ಷ PLUS:
	p = "PLUS";
	ಭಙ್ಗ;
      ಪಕ್ಷ NOMATCH:
	p = "NOMATCH";
	ಭಙ್ಗ;
      ಪಕ್ಷ MATCH:
	p = "MATCH";
	ಭಙ್ಗ;
      ಪಕ್ಷ BEHIND:
	p = "BEHIND";
	ಭಙ್ಗ;
      ಪಕ್ಷ NOBEHIND:
	p = "NOBEHIND";
	ಭಙ್ಗ;
      ಪಕ್ಷ SUBPAT:
	p = "SUBPAT";
	ಭಙ್ಗ;
      ಪಕ್ಷ BRACE_LIMITS:
	p = "BRACE_LIMITS";
	ಭಙ್ಗ;
      ಪಕ್ಷ BRACE_SIMPLE:
	p = "BRACE_SIMPLE";
	ಭಙ್ಗ;
      ಪಕ್ಷ BRACE_COMPLEX + 0:
      ಪಕ್ಷ BRACE_COMPLEX + 1:
      ಪಕ್ಷ BRACE_COMPLEX + 2:
      ಪಕ್ಷ BRACE_COMPLEX + 3:
      ಪಕ್ಷ BRACE_COMPLEX + 4:
      ಪಕ್ಷ BRACE_COMPLEX + 5:
      ಪಕ್ಷ BRACE_COMPLEX + 6:
      ಪಕ್ಷ BRACE_COMPLEX + 7:
      ಪಕ್ಷ BRACE_COMPLEX + 8:
      ಪಕ್ಷ BRACE_COMPLEX + 9:
	buflen += vim_snprintf(buf + buflen, ಪರಿಮಾಣ(buf) - buflen, "BRACE_COMPLEX%d", OP(op) - BRACE_COMPLEX);
	p = ರಿಕ್ತ;
	ಭಙ್ಗ;
      ಪಕ್ಷ MULTIBYTECODE:
	p = "MULTIBYTECODE";
	ಭಙ್ಗ;
      ಪಕ್ಷ NEWL:
	p = "NEWL";
	ಭಙ್ಗ;
      ಸ್ವಭಾವ:
	buflen += vim_snprintf(buf + buflen, ಪರಿಮಾಣ(buf) - buflen, "corrupt %d", OP(op));
	p = ರಿಕ್ತ;
	ಭಙ್ಗ;
    }
    ಯದಿ (p != ರಿಕ್ತ)
	vim_strncpy((char_u *)buf + buflen, (char_u *)p, ಪರಿಮಾಣ(buf) - buflen - 1);
    ಪ್ರತ್ಯಾಗಮ (char_u *)buf;
}
#endif	    // DEBUG
