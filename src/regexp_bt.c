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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਾ_3		0	//	End of program or NOMATCH operand.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOL		1	//	Match "" at beginning of line.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EOL		2	//	Match "" at end of line.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BRANCH		3	// node Match this alternative, or the
				//	next...
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪਿੱਠ_2		4	//	Match "", "next" ptr points backward.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXACTLY		5	// str	Match this string.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NOTHING		6	//	Match empty string.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਤਾਰਾ		7	// node Match this (simple) thing 0 or more
				//	times.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLUS		8	// node Match this (simple) thing 1 or more
				//	times.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਮੇਲ_2		9	// node match the operand zero-width
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NOMATCH		10	// node check for no match with operand
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BEHIND		11	// node look behind for a match with operand
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NOBEHIND	12	// node look behind for no match with operand
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SUBPAT		13	// node match the operand here
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BRACE_SIMPLE	14	// node Match this (simple) thing between m and
				//	n times (\{m,n\}).
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOW		15	//	Match "" after [^a-zA-Z0-9_]
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EOW		16	//	Match "" at    [^a-zA-Z0-9_]
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BRACE_LIMITS	17	// nr nr  define the min & max for BRACE_SIMPLE
				//	and BRACE_COMPLEX.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NEWL		18	//	Match line-break
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BHPOS		19	//	End position for BEHIND or NOBEHIND


// character classes: 20-48 normal, 50-78 include a line-break
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਜੋੜੋ_NL		30
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪਹਿਲਾ_NL	ANY + ਜੋੜੋ_NL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANY		20	//	Match any one character.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANYOF		21	// str	Match any character in this string.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANYBUT		22	// str	Match any character not in this
				//	string.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IDENT		23	//	Match identifier char
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIDENT		24	//	Match identifier char but no digit
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KWORD		25	//	Match keyword char
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SKWORD		26	//	Match word char but no digit
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FNAME		27	//	Match file name char
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SFNAME		28	//	Match file name char but no digit
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PRINT		29	//	Match printable char
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPRINT		30	//	Match printable char but no digit
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WHITE		31	//	Match whitespace char
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NWHITE		32	//	Match non-whitespace char
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIGIT		33	//	Match digit char
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NDIGIT		34	//	Match non-digit char
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HEX		35	//	Match hex char
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NHEX		36	//	Match non-hex char
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OCTAL		37	//	Match octal char
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NOCTAL		38	//	Match non-octal char
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸ਼ਬਦ_2		39	//	Match word char
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NWORD		40	//	Match non-word char
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰ_2		41	//	Match head char
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NHEAD		42	//	Match non-head char
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALPHA		43	//	Match alpha char
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NALPHA		44	//	Match non-alpha char
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਹੇਠਲਾ_2		45	//	Match lowercase char
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NLOWER		46	//	Match non-lowercase char
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਉੱਤਲਾ		47	//	Match uppercase char
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NUPPER		48	//	Match non-uppercase char
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਆਖਰੀ_NL		NUPPER + ਜੋੜੋ_NL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WITH_NL(op)	((op) >= ਪਹਿਲਾ_NL && (op) <= ਆਖਰੀ_NL)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOPEN		80  // -89	 Mark this point in input as start of
				//	 \( subexpr.  MOPEN + 0 marks start of
				//	 match.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MCLOSE		90  // -99	 Analogous to MOPEN.  MCLOSE + 0 marks
				//	 end of match.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BACKREF		100 // -109 node Match same string again \1-\9

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
# define ZOPEN		110 // -119	 Mark this point in input as start of
				//	 \z( subexpr.
# define ZCLOSE		120 // -129	 Analogous to ZOPEN.
# define ZREF		130 // -139 node Match external submatch \z1-\z9
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BRACE_COMPLEX	140 // -149 node Match nodes between m & n times

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NOPEN		150	//	Mark this point in input as start of
				//	\%( subexpr.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NCLOSE		151	//	Analogous to NOPEN.

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MULTIBYTECODE	200	// mbc	Match one multi-byte character
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RE_BOF		201	//	Match "" at beginning of file.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RE_EOF		202	//	Match "" at end of file.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURSOR		203	//	Match location of cursor.

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RE_LNUM		204	// nr cmp  Match line number
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RE_ਖਾਨਾ		205	// nr cmp  Match column number
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RE_VCOL		206	// nr cmp  Match virtual column number

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RE_MARK		207	// mark cmp  Match mark position
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RE_VISUAL	208	//	Match Visual area
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RE_COMPOSING	209	// any composing characters

/*
 * Flags to be passed up and down.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HASWIDTH	0x1	// Known never to match null string.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIMPLE		0x2	// Simple enough to be STAR/PLUS operand.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPSTART		0x4	// Starts with * or +.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HASNL		0x8	// Contains some \n.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HASLOOKBH	0x10	// Contains "\@<=" or "\@<!".
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WORST		0	// Worst case.

ਸਥਿਰ ਪੂਰਨ	ਅੰਕ_complex_braces; // Complex \{...} count
ਸਥਿਰ char_u	*regcode;	// Code-emit pointer, or JUST_CALC_SIZE
ਸਥਿਰ ਲੰਮਾ	regsize;	// Code size.
ਸਥਿਰ ਪੂਰਨ	reg_toolong;	// TRUE when offset out of range
ਸਥਿਰ ਪੂਰਨ	bt_reg_parse_ਡੂੰਘਾਈ;	// nesting depth in reg()
ਸਥਿਰ char_u	had_endbrace[NSUBEXP];	// flags, TRUE if end of () found
ਸਥਿਰ ਲੰਮਾ	brace_min[10];	// Minimums for complex brace repeats
ਸਥਿਰ ਲੰਮਾ	brace_max[10];	// Maximums for complex brace repeats
ਸਥਿਰ ਪੂਰਨ	brace_ਗਿਣਤੀ[10]; // Current counts for complex brace repeats
ਸਥਿਰ ਪੂਰਨ	one_exactly = FALSE;	// only do one char for EXACTLY

// When making changes to classchars also change nfa_classcodes.
ਸਥਿਰ char_u	*classchars = (char_u *)".iIkKfFpPsSdDxXoOwWhHaAlLuU";
ਸਥਿਰ ਅਚਲ ਪੂਰਨ classcodes[] = {
    ANY, IDENT, SIDENT, KWORD, SKWORD,
    FNAME, SFNAME, PRINT, SPRINT,
    WHITE, NWHITE, DIGIT, NDIGIT,
    HEX, NHEX, OCTAL, NOCTAL,
    ਸ਼ਬਦ_2, NWORD, ਸਿਰ_2, NHEAD,
    ALPHA, NALPHA, ਹੇਠਲਾ_2, NLOWER,
    ਉੱਤਲਾ, NUPPER
};

/*
 * Search between "start" (the first char of the range) and "end" (the closing
 * "]") and try to recognize a character class in expanded form, for example
 * [0-9].  On success, return the atom to be emitted, on failure 0.
 */
    ਸਥਿਰ ਪੂਰਨ
bt_recognize_char_class(char_u *ਸ਼ੁਰੂ, char_u *ਸਿਰਾ)
{
    ਪੂਰਨ		newl;
    ਪੂਰਨ		config = get_char_class_ਬਿੱਟ(ਸ਼ੁਰੂ, ਸਿਰਾ, &newl);

    // A newline would need the ADD_NL variant, don't bother with it here.
    ਜੇ (config < 0 || newl)
	ਵਾਪਸ 0;

	// The old engine has no case-insensitive class opcode, so [a-z] and [A-Z]
	// are left as collections.  The classes below are case-independent.
    ਚੋਣ (config)
    {
	ਹਾਲਤ CLASS_o9:
	    ਵਾਪਸ DIGIT;
	ਹਾਲਤ CLASS_not | CLASS_o9:
	    ਵਾਪਸ NDIGIT;
	ਹਾਲਤ CLASS_af | CLASS_AF | CLASS_o9:
	    ਵਾਪਸ HEX;
	ਹਾਲਤ CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    ਵਾਪਸ NHEX;
	ਹਾਲਤ CLASS_o7:
	    ਵਾਪਸ OCTAL;
	ਹਾਲਤ CLASS_not | CLASS_o7:
	    ਵਾਪਸ NOCTAL;
	ਹਾਲਤ CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    ਵਾਪਸ ਸ਼ਬਦ_2;
	ਹਾਲਤ CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    ਵਾਪਸ NWORD;
	ਹਾਲਤ CLASS_az | CLASS_AZ | CLASS_underscore:
	    ਵਾਪਸ ਸਿਰ_2;
	ਹਾਲਤ CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    ਵਾਪਸ NHEAD;
	ਹਾਲਤ CLASS_az | CLASS_AZ:
	    ਵਾਪਸ ALPHA;
	ਹਾਲਤ CLASS_not | CLASS_az | CLASS_AZ:
	    ਵਾਪਸ NALPHA;
    }
    ਵਾਪਸ 0;
}

/*
 * When regcode is set to this value, code is not emitted and size is computed
 * instead.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JUST_CALC_ਮਾਪ	((char_u *) -1)

// Values for rs_state in regitem_T.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ regstate_E
{
    RS_NOPEN = 0	// NOPEN and NCLOSE
    , RS_MOPEN		// MOPEN + [0-9]
    , RS_MCLOSE		// MCLOSE + [0-9]
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    , RS_ZOPEN		// ZOPEN + [0-9]
    , RS_ZCLOSE		// ZCLOSE + [0-9]
#ਜੇ_ਅੰਤ
    , RS_BRANCH		// BRANCH
    , RS_BRCPLX_ਹੋਰ	// BRACE_COMPLEX and trying one more match
    , RS_BRCPLX_LONG	// BRACE_COMPLEX and trying longest match
    , RS_BRCPLX_SHORT	// BRACE_COMPLEX and trying shortest match
    , RS_NOMATCH	// NOMATCH
    , RS_BEHIND1	// BEHIND / NOBEHIND matching rest
    , RS_BEHIND2	// BEHIND / NOBEHIND matching behind part
    , RS_ਤਾਰਾ_LONG	// STAR/PLUS/BRACE_SIMPLE longest match
    , RS_ਤਾਰਾ_SHORT	// STAR/PLUS/BRACE_SIMPLE shortest match
} regstate_T;

/*
 * Structure used to save the current input state, when it needs to be
 * restored after trying a match.  Used by reg_save() and reg_restore().
 * Also stores the length of "backpos".
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਸੰਘ
    {
	char_u	*ptr;	// rex.input pointer, for single-line regexp
	lpos_T	pos;	// rex.input pos, for multi-line regexp
    } rs_u;
    ਪੂਰਨ		rs_len;
} regsave_T;

// struct to save start/end pointer/position in for \(\)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਸੰਘ
    {
	char_u	*ptr;
	lpos_T	pos;
    } se_u;
} save_se_T;

// used for BEHIND and NOBEHIND matching
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ regbehind_S
{
    regsave_T	save_after;
    regsave_T	save_behind;
    ਪੂਰਨ		save_need_clear_subexpr;
    save_se_T   save_ਸ਼ੁਰੂ[NSUBEXP];
    save_se_T   save_ਸਿਰਾ[NSUBEXP];
} regbehind_T;

/*
 * When there are alternatives a regstate_T is put on the regstack to remember
 * what we are doing.
 * Before it may be another type of item, depending on rs_state, to remember
 * more things.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ regitem_S
{
    regstate_T	rs_state;	// what we are doing, one of RS_ above
    ਛੋਟਾ	rs_no;		// submatch nr or BEHIND/NOBEHIND
    char_u	*rs_ਜਾਂਚ;	// current node in program
    ਸੰਘ
    {
	save_se_T  sesave;
	regsave_T  regsave;
    } rs_un;			// room for saving rex.input
} regitem_T;


// used for STAR, PLUS and BRACE_SIMPLE matching
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ regstar_S
{
    ਪੂਰਨ		nextb;		// next byte
    ਪੂਰਨ		nextb_ic;	// next byte reverse case
    ਲੰਮਾ	ਗਿਣਤੀ_2;
    ਲੰਮਾ	minval;
    ਲੰਮਾ	maxval;
} regstar_T;

// used to store input position when a BACK was encountered, so that we now if
// we made any progress since the last time.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ backpos_S
{
    char_u	*bp_ਜਾਂਚ;	// "scan" where BACK was encountered
    regsave_T	bp_pos;		// last input position
} backpos_T;

/*
 * "regstack" and "backpos" are used by regmatch().  They are kept over calls
 * to avoid invoking malloc() and free() often.
 * "regstack" is a stack with regitem_T items, sometimes preceded by regstar_T
 * or regbehind_T.
 * "backpos_T" is a table with backpos_T for BACK
 */
ਸਥਿਰ garray_T	regstack = {0, 0, 0, 0, ਨਿਰਾ};
ਸਥਿਰ garray_T	backpos = {0, 0, 0, 0, ਨਿਰਾ};

ਸਥਿਰ regsave_T behind_pos;

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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REGSTACK_INITIAL	2048
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BACKPOS_INITIAL		64

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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OP(p)		((ਪੂਰਨ)*(p))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅਗਲਾ_2(p)		(((*((p) + 1) & 0377) << 8) + (*((p) + 2) & 0377))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੰਕਾਰਜ(p)	((p) + 3)
// Obtain an operand that was stored as four bytes, MSB first.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੰਕਾਰਜ_MIN(p)	(((ਲੰਮਾ)(p)[3] << 24) + ((ਲੰਮਾ)(p)[4] << 16) \
			+ ((ਲੰਮਾ)(p)[5] << 8) + (ਲੰਮਾ)(p)[6])
// Obtain a second operand stored as four bytes.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੰਕਾਰਜ_MAX(p)	ਸੰਕਾਰਜ_MIN((p) + 4)
// Obtain a second single-byte operand stored after a four bytes operand.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੰਕਾਰਜ_CMP(p)	(p)[7]

ਸਥਿਰ char_u *reg(ਪੂਰਨ paren, ਪੂਰਨ *flagp);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BT_REGEXP_DUMP
ਸਥਿਰ ਖਾਲੀ	regdump(char_u *, bt_regprog_T *);
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ	re_ਅੰਕ_cmp(long_u val, char_u *ਜਾਂਚ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
ਸਥਿਰ char_u	*regprop(char_u *);

ਸਥਿਰ ਪੂਰਨ	regnarrate = 0;
#ਜੇ_ਅੰਤ


/*
 * Setup to parse the regexp.  Used once to get the length and once to do it.
 */
    ਸਥਿਰ ਖਾਲੀ
regcomp_ਸ਼ੁਰੂ(
    char_u	*ਵਿਅੰਜਕ,
    ਪੂਰਨ		re_flags)	    // see vim_regcomp()
{
    initchr(ਵਿਅੰਜਕ);
    ਜੇ (re_flags & RE_MAGIC)
	reg_magic = MAGIC_ON;
    ਨਹੀਂਤਾਂ
	reg_magic = MAGIC_OFF;
    reg_string = (re_flags & RE_STRING);
    reg_strict = (re_flags & RE_STRICT);
    get_cpo_flags();

    ਅੰਕ_complex_braces = 0;
    regnpar = 1;
    CLEAR_ਖੇਤਰ(had_endbrace);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    regnzpar = 1;
    re_has_z = 0;
#ਜੇ_ਅੰਤ
    regsize = 0L;
    reg_toolong = FALSE;
    bt_reg_parse_ਡੂੰਘਾਈ = 0;
    regflags = 0;
#ਜੇ defined(FEAT_SYN_HL)
    had_eol = FALSE;
#ਜੇ_ਅੰਤ
}

/*
 * Return TRUE if MULTIBYTECODE should be used instead of EXACTLY for
 * character "c".
 */
    ਸਥਿਰ ਪੂਰਨ
use_multibytecode(ਪੂਰਨ c)
{
    ਵਾਪਸ has_mbyte && (*mb_char2len)(c) > 1
		     && (re_multi_type(peekchr()) != NOT_MULTI
			     || (enc_utf8 && utf_iscomposing(c)));
}

/*
 * Emit (if appropriate) a byte of code
 */
    ਸਥਿਰ ਖਾਲੀ
regc(ਪੂਰਨ b)
{
    ਜੇ (regcode == JUST_CALC_ਮਾਪ)
	regsize++;
    ਨਹੀਂਤਾਂ
	*regcode++ = b;
}

/*
 * Emit (if appropriate) a multi-byte character of code
 */
    ਸਥਿਰ ਖਾਲੀ
regmbc(ਪੂਰਨ c)
{
    ਜੇ (!has_mbyte && c > 0xff)
	ਵਾਪਸ;
    ਜੇ (regcode == JUST_CALC_ਮਾਪ)
	regsize += (*mb_char2len)(c);
    ਨਹੀਂਤਾਂ
	regcode += (*mb_char2bytes)(c, regcode);
}


/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * NOTE: When changing this function, also change nfa_emit_equi_class()
 */
    ਸਥਿਰ ਖਾਲੀ
reg_equi_class(ਪੂਰਨ c)
{
    ਜੇ (enc_utf8 || STRCMP(p_enc, "latin1") == 0
					 || STRCMP(p_enc, "iso-8859-15") == 0)
    {
	ਚੋਣ (c)
	{
	    // Do not use '\300' style, it results in a negative number.
	    ਹਾਲਤ 'A': ਹਾਲਤ 0xc0: ਹਾਲਤ 0xc1: ਹਾਲਤ 0xc2: ਹਾਲਤ 0xc3: ਹਾਲਤ 0xc4:
	    ਹਾਲਤ 0xc5: ਹਾਲਤ 0x100: ਹਾਲਤ 0x102: ਹਾਲਤ 0x104: ਹਾਲਤ 0x1cd:
	    ਹਾਲਤ 0x1de: ਹਾਲਤ 0x1e0: ਹਾਲਤ 0x1fa: ਹਾਲਤ 0x202: ਹਾਲਤ 0x226:
	    ਹਾਲਤ 0x23a: ਹਾਲਤ 0x1e00: ਹਾਲਤ 0x1ea0: ਹਾਲਤ 0x1ea2: ਹਾਲਤ 0x1ea4:
	    ਹਾਲਤ 0x1ea6: ਹਾਲਤ 0x1ea8: ਹਾਲਤ 0x1eaa: ਹਾਲਤ 0x1eac: ਹਾਲਤ 0x1eae:
	    ਹਾਲਤ 0x1eb0: ਹਾਲਤ 0x1eb2: ਹਾਲਤ 0x1eb4: ਹਾਲਤ 0x1eb6:
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
		      ਵਾਪਸ;
	    ਹਾਲਤ 'B': ਹਾਲਤ 0x181: ਹਾਲਤ 0x243: ਹਾਲਤ 0x1e02:
	    ਹਾਲਤ 0x1e04: ਹਾਲਤ 0x1e06:
		      regmbc('B');
		      regmbc(0x181); regmbc(0x243); regmbc(0x1e02);
		      regmbc(0x1e04); regmbc(0x1e06);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'C': ਹਾਲਤ 0xc7:
	    ਹਾਲਤ 0x106: ਹਾਲਤ 0x108: ਹਾਲਤ 0x10a: ਹਾਲਤ 0x10c: ਹਾਲਤ 0x187:
	    ਹਾਲਤ 0x23b: ਹਾਲਤ 0x1e08: ਹਾਲਤ 0xa792:
		      regmbc('C'); regmbc(0xc7);
		      regmbc(0x106); regmbc(0x108); regmbc(0x10a);
		      regmbc(0x10c); regmbc(0x187); regmbc(0x23b);
		      regmbc(0x1e08); regmbc(0xa792);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'D': ਹਾਲਤ 0x10e: ਹਾਲਤ 0x110: ਹਾਲਤ 0x18a:
	    ਹਾਲਤ 0x1e0a: ਹਾਲਤ 0x1e0c: ਹਾਲਤ 0x1e0e: ਹਾਲਤ 0x1e10:
	    ਹਾਲਤ 0x1e12:
		      regmbc('D'); regmbc(0x10e); regmbc(0x110);
		      regmbc(0x18a); regmbc(0x1e0a); regmbc(0x1e0c);
		      regmbc(0x1e0e); regmbc(0x1e10); regmbc(0x1e12);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'E': ਹਾਲਤ 0xc8: ਹਾਲਤ 0xc9: ਹਾਲਤ 0xca: ਹਾਲਤ 0xcb:
	    ਹਾਲਤ 0x112: ਹਾਲਤ 0x114: ਹਾਲਤ 0x116: ਹਾਲਤ 0x118: ਹਾਲਤ 0x11a:
	    ਹਾਲਤ 0x204: ਹਾਲਤ 0x206: ਹਾਲਤ 0x228: ਹਾਲਤ 0x246: ਹਾਲਤ 0x1e14:
	    ਹਾਲਤ 0x1e16: ਹਾਲਤ 0x1e18: ਹਾਲਤ 0x1e1a: ਹਾਲਤ 0x1e1c:
	    ਹਾਲਤ 0x1eb8: ਹਾਲਤ 0x1eba: ਹਾਲਤ 0x1ebc: ਹਾਲਤ 0x1ebe:
	    ਹਾਲਤ 0x1ec0: ਹਾਲਤ 0x1ec2: ਹਾਲਤ 0x1ec4: ਹਾਲਤ 0x1ec6:
		      regmbc('E'); regmbc(0xc8); regmbc(0xc9);
		      regmbc(0xca); regmbc(0xcb); regmbc(0x112);
		      regmbc(0x114); regmbc(0x116); regmbc(0x118);
		      regmbc(0x11a); regmbc(0x204); regmbc(0x206);
		      regmbc(0x228); regmbc(0x246); regmbc(0x1e14);
		      regmbc(0x1e16); regmbc(0x1e18); regmbc(0x1e1a);
		      regmbc(0x1e1c); regmbc(0x1eb8); regmbc(0x1eba);
		      regmbc(0x1ebc); regmbc(0x1ebe); regmbc(0x1ec0);
		      regmbc(0x1ec2); regmbc(0x1ec4); regmbc(0x1ec6);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'F': ਹਾਲਤ 0x191: ਹਾਲਤ 0x1e1e: ਹਾਲਤ 0xa798:
		      regmbc('F'); regmbc(0x191); regmbc(0x1e1e);
		      regmbc(0xa798);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'G': ਹਾਲਤ 0x11c: ਹਾਲਤ 0x11e: ਹਾਲਤ 0x120:
	    ਹਾਲਤ 0x122: ਹਾਲਤ 0x193: ਹਾਲਤ 0x1e4: ਹਾਲਤ 0x1e6:
	    ਹਾਲਤ 0x1f4: ਹਾਲਤ 0x1e20: ਹਾਲਤ 0xa7a0:
		      regmbc('G'); regmbc(0x11c); regmbc(0x11e);
		      regmbc(0x120); regmbc(0x122); regmbc(0x193);
		      regmbc(0x1e4); regmbc(0x1e6); regmbc(0x1f4);
		      regmbc(0x1e20); regmbc(0xa7a0);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'H': ਹਾਲਤ 0x124: ਹਾਲਤ 0x126: ਹਾਲਤ 0x21e:
	    ਹਾਲਤ 0x1e22: ਹਾਲਤ 0x1e24: ਹਾਲਤ 0x1e26:
	    ਹਾਲਤ 0x1e28: ਹਾਲਤ 0x1e2a: ਹਾਲਤ 0x2c67:
		      regmbc('H'); regmbc(0x124); regmbc(0x126);
		      regmbc(0x21e); regmbc(0x1e22); regmbc(0x1e24);
		      regmbc(0x1e26); regmbc(0x1e28); regmbc(0x1e2a);
		      regmbc(0x2c67);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'I': ਹਾਲਤ 0xcc: ਹਾਲਤ 0xcd: ਹਾਲਤ 0xce: ਹਾਲਤ 0xcf:
	    ਹਾਲਤ 0x128: ਹਾਲਤ 0x12a: ਹਾਲਤ 0x12c: ਹਾਲਤ 0x12e:
	    ਹਾਲਤ 0x130: ਹਾਲਤ 0x197: ਹਾਲਤ 0x1cf: ਹਾਲਤ 0x208:
	    ਹਾਲਤ 0x20a: ਹਾਲਤ 0x1e2c: ਹਾਲਤ 0x1e2e: ਹਾਲਤ 0x1ec8:
	    ਹਾਲਤ 0x1eca:
		      regmbc('I'); regmbc(0xcc); regmbc(0xcd);
		      regmbc(0xce); regmbc(0xcf); regmbc(0x128);
		      regmbc(0x12a); regmbc(0x12c); regmbc(0x12e);
		      regmbc(0x130); regmbc(0x197); regmbc(0x1cf);
		      regmbc(0x208); regmbc(0x20a); regmbc(0x1e2c);
		      regmbc(0x1e2e); regmbc(0x1ec8); regmbc(0x1eca);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'J': ਹਾਲਤ 0x134: ਹਾਲਤ 0x248:
		      regmbc('J'); regmbc(0x134); regmbc(0x248);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'K': ਹਾਲਤ 0x136: ਹਾਲਤ 0x198: ਹਾਲਤ 0x1e8: ਹਾਲਤ 0x1e30:
	    ਹਾਲਤ 0x1e32: ਹਾਲਤ 0x1e34: ਹਾਲਤ 0x2c69: ਹਾਲਤ 0xa740:
		      regmbc('K'); regmbc(0x136); regmbc(0x198);
		      regmbc(0x1e8); regmbc(0x1e30); regmbc(0x1e32);
		      regmbc(0x1e34); regmbc(0x2c69); regmbc(0xa740);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'L': ਹਾਲਤ 0x139: ਹਾਲਤ 0x13b: ਹਾਲਤ 0x13d: ਹਾਲਤ 0x13f:
	    ਹਾਲਤ 0x141: ਹਾਲਤ 0x23d: ਹਾਲਤ 0x1e36: ਹਾਲਤ 0x1e38:
	    ਹਾਲਤ 0x1e3a: ਹਾਲਤ 0x1e3c: ਹਾਲਤ 0x2c60:
		      regmbc('L'); regmbc(0x139); regmbc(0x13b);
		      regmbc(0x13d); regmbc(0x13f); regmbc(0x141);
		      regmbc(0x23d); regmbc(0x1e36); regmbc(0x1e38);
		      regmbc(0x1e3a); regmbc(0x1e3c); regmbc(0x2c60);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'M': ਹਾਲਤ 0x1e3e: ਹਾਲਤ 0x1e40: ਹਾਲਤ 0x1e42:
		      regmbc('M'); regmbc(0x1e3e); regmbc(0x1e40);
		      regmbc(0x1e42);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'N': ਹਾਲਤ 0xd1:
	    ਹਾਲਤ 0x143: ਹਾਲਤ 0x145: ਹਾਲਤ 0x147: ਹਾਲਤ 0x1f8:
	    ਹਾਲਤ 0x1e44: ਹਾਲਤ 0x1e46: ਹਾਲਤ 0x1e48: ਹਾਲਤ 0x1e4a:
	    ਹਾਲਤ 0xa7a4:
		      regmbc('N'); regmbc(0xd1);
		      regmbc(0x143); regmbc(0x145); regmbc(0x147);
		      regmbc(0x1f8); regmbc(0x1e44); regmbc(0x1e46);
		      regmbc(0x1e48); regmbc(0x1e4a); regmbc(0xa7a4);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'O': ਹਾਲਤ 0xd2: ਹਾਲਤ 0xd3: ਹਾਲਤ 0xd4: ਹਾਲਤ 0xd5: ਹਾਲਤ 0xd6:
	    ਹਾਲਤ 0xd8: ਹਾਲਤ 0x14c: ਹਾਲਤ 0x14e: ਹਾਲਤ 0x150: ਹਾਲਤ 0x19f:
	    ਹਾਲਤ 0x1a0: ਹਾਲਤ 0x1d1: ਹਾਲਤ 0x1ea: ਹਾਲਤ 0x1ec: ਹਾਲਤ 0x1fe:
	    ਹਾਲਤ 0x20c: ਹਾਲਤ 0x20e: ਹਾਲਤ 0x22a: ਹਾਲਤ 0x22c: ਹਾਲਤ 0x22e:
	    ਹਾਲਤ 0x230: ਹਾਲਤ 0x1e4c: ਹਾਲਤ 0x1e4e: ਹਾਲਤ 0x1e50: ਹਾਲਤ 0x1e52:
	    ਹਾਲਤ 0x1ecc: ਹਾਲਤ 0x1ece: ਹਾਲਤ 0x1ed0: ਹਾਲਤ 0x1ed2: ਹਾਲਤ 0x1ed4:
	    ਹਾਲਤ 0x1ed6: ਹਾਲਤ 0x1ed8: ਹਾਲਤ 0x1eda: ਹਾਲਤ 0x1edc: ਹਾਲਤ 0x1ede:
	    ਹਾਲਤ 0x1ee0: ਹਾਲਤ 0x1ee2:
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
		      ਵਾਪਸ;
	    ਹਾਲਤ 'P': ਹਾਲਤ 0x1a4: ਹਾਲਤ 0x1e54: ਹਾਲਤ 0x1e56: ਹਾਲਤ 0x2c63:
		      regmbc('P'); regmbc(0x1a4); regmbc(0x1e54);
		      regmbc(0x1e56); regmbc(0x2c63);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'Q': ਹਾਲਤ 0x24a:
		      regmbc('Q'); regmbc(0x24a);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'R': ਹਾਲਤ 0x154: ਹਾਲਤ 0x156: ਹਾਲਤ 0x158: ਹਾਲਤ 0x210:
	    ਹਾਲਤ 0x212: ਹਾਲਤ 0x24c: ਹਾਲਤ 0x1e58: ਹਾਲਤ 0x1e5a:
	    ਹਾਲਤ 0x1e5c: ਹਾਲਤ 0x1e5e: ਹਾਲਤ 0x2c64: ਹਾਲਤ 0xa7a6:
		      regmbc('R'); regmbc(0x154); regmbc(0x156);
		      regmbc(0x210); regmbc(0x212); regmbc(0x158);
		      regmbc(0x24c); regmbc(0x1e58); regmbc(0x1e5a);
		      regmbc(0x1e5c); regmbc(0x1e5e); regmbc(0x2c64);
		      regmbc(0xa7a6);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'S': ਹਾਲਤ 0x15a: ਹਾਲਤ 0x15c: ਹਾਲਤ 0x15e: ਹਾਲਤ 0x160:
	    ਹਾਲਤ 0x218: ਹਾਲਤ 0x1e60: ਹਾਲਤ 0x1e62: ਹਾਲਤ 0x1e64:
	    ਹਾਲਤ 0x1e66: ਹਾਲਤ 0x1e68: ਹਾਲਤ 0x2c7e: ਹਾਲਤ 0xa7a8:
		      regmbc('S'); regmbc(0x15a); regmbc(0x15c);
		      regmbc(0x15e); regmbc(0x160); regmbc(0x218);
		      regmbc(0x1e60); regmbc(0x1e62); regmbc(0x1e64);
		      regmbc(0x1e66); regmbc(0x1e68); regmbc(0x2c7e);
			  regmbc(0xa7a8);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'T': ਹਾਲਤ 0x162: ਹਾਲਤ 0x164: ਹਾਲਤ 0x166: ਹਾਲਤ 0x1ac:
	    ਹਾਲਤ 0x1ae: ਹਾਲਤ 0x21a: ਹਾਲਤ 0x23e: ਹਾਲਤ 0x1e6a: ਹਾਲਤ 0x1e6c:
	    ਹਾਲਤ 0x1e6e: ਹਾਲਤ 0x1e70:
		      regmbc('T'); regmbc(0x162); regmbc(0x164);
		      regmbc(0x166); regmbc(0x1ac); regmbc(0x23e);
		      regmbc(0x1ae); regmbc(0x21a); regmbc(0x1e6a);
		      regmbc(0x1e6c); regmbc(0x1e6e); regmbc(0x1e70);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'U': ਹਾਲਤ 0xd9: ਹਾਲਤ 0xda: ਹਾਲਤ 0xdb: ਹਾਲਤ 0xdc:
	    ਹਾਲਤ 0x168: ਹਾਲਤ 0x16a: ਹਾਲਤ 0x16c: ਹਾਲਤ 0x16e:
	    ਹਾਲਤ 0x170: ਹਾਲਤ 0x172: ਹਾਲਤ 0x1af: ਹਾਲਤ 0x1d3:
	    ਹਾਲਤ 0x1d5: ਹਾਲਤ 0x1d7: ਹਾਲਤ 0x1d9: ਹਾਲਤ 0x1db:
	    ਹਾਲਤ 0x214: ਹਾਲਤ 0x216: ਹਾਲਤ 0x244: ਹਾਲਤ 0x1e72:
	    ਹਾਲਤ 0x1e74: ਹਾਲਤ 0x1e76: ਹਾਲਤ 0x1e78: ਹਾਲਤ 0x1e7a:
	    ਹਾਲਤ 0x1ee4: ਹਾਲਤ 0x1ee6: ਹਾਲਤ 0x1ee8: ਹਾਲਤ 0x1eea:
	    ਹਾਲਤ 0x1eec: ਹਾਲਤ 0x1eee: ਹਾਲਤ 0x1ef0:
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
		      ਵਾਪਸ;
	    ਹਾਲਤ 'V': ਹਾਲਤ 0x1b2: ਹਾਲਤ 0x1e7c: ਹਾਲਤ 0x1e7e:
		      regmbc('V'); regmbc(0x1b2); regmbc(0x1e7c);
		      regmbc(0x1e7e);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'W': ਹਾਲਤ 0x174: ਹਾਲਤ 0x1e80: ਹਾਲਤ 0x1e82:
	    ਹਾਲਤ 0x1e84: ਹਾਲਤ 0x1e86: ਹਾਲਤ 0x1e88:
		      regmbc('W'); regmbc(0x174); regmbc(0x1e80);
		      regmbc(0x1e82); regmbc(0x1e84); regmbc(0x1e86);
		      regmbc(0x1e88);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'X': ਹਾਲਤ 0x1e8a: ਹਾਲਤ 0x1e8c:
		      regmbc('X'); regmbc(0x1e8a); regmbc(0x1e8c);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'Y': ਹਾਲਤ 0xdd:
	    ਹਾਲਤ 0x176: ਹਾਲਤ 0x178: ਹਾਲਤ 0x1b3: ਹਾਲਤ 0x232: ਹਾਲਤ 0x24e:
	    ਹਾਲਤ 0x1e8e: ਹਾਲਤ 0x1ef2: ਹਾਲਤ 0x1ef6: ਹਾਲਤ 0x1ef4: ਹਾਲਤ 0x1ef8:
		      regmbc('Y'); regmbc(0xdd); regmbc(0x176);
		      regmbc(0x178); regmbc(0x1b3); regmbc(0x232);
		      regmbc(0x24e); regmbc(0x1e8e); regmbc(0x1ef2);
		      regmbc(0x1ef4); regmbc(0x1ef6); regmbc(0x1ef8);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'Z': ਹਾਲਤ 0x179: ਹਾਲਤ 0x17b: ਹਾਲਤ 0x17d: ਹਾਲਤ 0x1b5:
	    ਹਾਲਤ 0x1e90: ਹਾਲਤ 0x1e92: ਹਾਲਤ 0x1e94: ਹਾਲਤ 0x2c6b:
		      regmbc('Z'); regmbc(0x179); regmbc(0x17b);
		      regmbc(0x17d); regmbc(0x1b5); regmbc(0x1e90);
		      regmbc(0x1e92); regmbc(0x1e94); regmbc(0x2c6b);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'a': ਹਾਲਤ 0xe0: ਹਾਲਤ 0xe1: ਹਾਲਤ 0xe2:
	    ਹਾਲਤ 0xe3: ਹਾਲਤ 0xe4: ਹਾਲਤ 0xe5: ਹਾਲਤ 0x101: ਹਾਲਤ 0x103:
	    ਹਾਲਤ 0x105: ਹਾਲਤ 0x1ce: ਹਾਲਤ 0x1df: ਹਾਲਤ 0x1e1: ਹਾਲਤ 0x1fb:
	    ਹਾਲਤ 0x201: ਹਾਲਤ 0x203: ਹਾਲਤ 0x227: ਹਾਲਤ 0x1d8f: ਹਾਲਤ 0x1e01:
	    ਹਾਲਤ 0x1e9a: ਹਾਲਤ 0x1ea1: ਹਾਲਤ 0x1ea3: ਹਾਲਤ 0x1ea5:
	    ਹਾਲਤ 0x1ea7: ਹਾਲਤ 0x1ea9: ਹਾਲਤ 0x1eab: ਹਾਲਤ 0x1ead:
	    ਹਾਲਤ 0x1eaf: ਹਾਲਤ 0x1eb1: ਹਾਲਤ 0x1eb3: ਹਾਲਤ 0x1eb5:
	    ਹਾਲਤ 0x1eb7: ਹਾਲਤ 0x2c65:
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
		      ਵਾਪਸ;
	    ਹਾਲਤ 'b': ਹਾਲਤ 0x180: ਹਾਲਤ 0x253: ਹਾਲਤ 0x1d6c: ਹਾਲਤ 0x1d80:
	    ਹਾਲਤ 0x1e03: ਹਾਲਤ 0x1e05: ਹਾਲਤ 0x1e07:
		      regmbc('b');
		      regmbc(0x180); regmbc(0x253); regmbc(0x1d6c);
		      regmbc(0x1d80); regmbc(0x1e03); regmbc(0x1e05);
		      regmbc(0x1e07);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'c': ਹਾਲਤ 0xe7:
	    ਹਾਲਤ 0x107: ਹਾਲਤ 0x109: ਹਾਲਤ 0x10b: ਹਾਲਤ 0x10d: ਹਾਲਤ 0x188:
	    ਹਾਲਤ 0x23c: ਹਾਲਤ 0x1e09: ਹਾਲਤ 0xa793: ਹਾਲਤ 0xa794:
		      regmbc('c'); regmbc(0xe7); regmbc(0x107);
		      regmbc(0x109); regmbc(0x10b); regmbc(0x10d);
		      regmbc(0x188); regmbc(0x23c); regmbc(0x1e09);
		      regmbc(0xa793); regmbc(0xa794);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'd': ਹਾਲਤ 0x10f: ਹਾਲਤ 0x111: ਹਾਲਤ 0x257: ਹਾਲਤ 0x1d6d:
	    ਹਾਲਤ 0x1d81: ਹਾਲਤ 0x1d91: ਹਾਲਤ 0x1e0b: ਹਾਲਤ 0x1e0d:
	    ਹਾਲਤ 0x1e0f: ਹਾਲਤ 0x1e11: ਹਾਲਤ 0x1e13:
		      regmbc('d'); regmbc(0x10f); regmbc(0x111);
		      regmbc(0x257); regmbc(0x1d6d); regmbc(0x1d81);
		      regmbc(0x1d91); regmbc(0x1e0b); regmbc(0x1e0d);
		      regmbc(0x1e0f); regmbc(0x1e11); regmbc(0x1e13);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'e': ਹਾਲਤ 0xe8: ਹਾਲਤ 0xe9: ਹਾਲਤ 0xea: ਹਾਲਤ 0xeb:
	    ਹਾਲਤ 0x113: ਹਾਲਤ 0x115: ਹਾਲਤ 0x117: ਹਾਲਤ 0x119:
	    ਹਾਲਤ 0x11b: ਹਾਲਤ 0x205: ਹਾਲਤ 0x207: ਹਾਲਤ 0x229:
	    ਹਾਲਤ 0x247: ਹਾਲਤ 0x1d92: ਹਾਲਤ 0x1e15: ਹਾਲਤ 0x1e17:
	    ਹਾਲਤ 0x1e19: ਹਾਲਤ 0x1e1b: ਹਾਲਤ 0x1eb9: ਹਾਲਤ 0x1ebb:
	    ਹਾਲਤ 0x1e1d: ਹਾਲਤ 0x1ebd: ਹਾਲਤ 0x1ebf: ਹਾਲਤ 0x1ec1:
	    ਹਾਲਤ 0x1ec3: ਹਾਲਤ 0x1ec5: ਹਾਲਤ 0x1ec7:
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
		      ਵਾਪਸ;
	    ਹਾਲਤ 'f': ਹਾਲਤ 0x192: ਹਾਲਤ 0x1d6e: ਹਾਲਤ 0x1d82:
	    ਹਾਲਤ 0x1e1f: ਹਾਲਤ 0xa799:
		     regmbc('f'); regmbc(0x192); regmbc(0x1d6e);
		     regmbc(0x1d82); regmbc(0x1e1f); regmbc(0xa799);
		     ਵਾਪਸ;
	    ਹਾਲਤ 'g': ਹਾਲਤ 0x11d: ਹਾਲਤ 0x11f: ਹਾਲਤ 0x121: ਹਾਲਤ 0x123:
	    ਹਾਲਤ 0x1e5: ਹਾਲਤ 0x1e7: ਹਾਲਤ 0x260: ਹਾਲਤ 0x1f5: ਹਾਲਤ 0x1d83:
	    ਹਾਲਤ 0x1e21: ਹਾਲਤ 0xa7a1:
		      regmbc('g'); regmbc(0x11d); regmbc(0x11f);
		      regmbc(0x121); regmbc(0x123); regmbc(0x1e5);
		      regmbc(0x1e7); regmbc(0x1f5); regmbc(0x260);
		      regmbc(0x1d83); regmbc(0x1e21); regmbc(0xa7a1);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'h': ਹਾਲਤ 0x125: ਹਾਲਤ 0x127: ਹਾਲਤ 0x21f: ਹਾਲਤ 0x1e23:
	    ਹਾਲਤ 0x1e25: ਹਾਲਤ 0x1e27: ਹਾਲਤ 0x1e29: ਹਾਲਤ 0x1e2b:
	    ਹਾਲਤ 0x1e96: ਹਾਲਤ 0x2c68: ਹਾਲਤ 0xa795:
		      regmbc('h'); regmbc(0x125); regmbc(0x127);
		      regmbc(0x21f); regmbc(0x1e23); regmbc(0x1e25);
		      regmbc(0x1e27); regmbc(0x1e29); regmbc(0x1e2b);
		      regmbc(0x1e96); regmbc(0x2c68); regmbc(0xa795);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'i': ਹਾਲਤ 0xec: ਹਾਲਤ 0xed: ਹਾਲਤ 0xee: ਹਾਲਤ 0xef:
	    ਹਾਲਤ 0x129: ਹਾਲਤ 0x12b: ਹਾਲਤ 0x12d: ਹਾਲਤ 0x12f:
	    ਹਾਲਤ 0x1d0: ਹਾਲਤ 0x209: ਹਾਲਤ 0x20b: ਹਾਲਤ 0x268:
	    ਹਾਲਤ 0x1d96: ਹਾਲਤ 0x1e2d: ਹਾਲਤ 0x1e2f: ਹਾਲਤ 0x1ec9:
	    ਹਾਲਤ 0x1ecb:
		      regmbc('i'); regmbc(0xec); regmbc(0xed);
		      regmbc(0xee); regmbc(0xef); regmbc(0x129);
		      regmbc(0x12b); regmbc(0x12d); regmbc(0x12f);
		      regmbc(0x1d0); regmbc(0x209); regmbc(0x20b);
		      regmbc(0x268); regmbc(0x1d96); regmbc(0x1e2d);
		      regmbc(0x1e2f); regmbc(0x1ec9); regmbc(0x1ecb);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'j': ਹਾਲਤ 0x135: ਹਾਲਤ 0x1f0: ਹਾਲਤ 0x249:
		      regmbc('j'); regmbc(0x135); regmbc(0x1f0);
		      regmbc(0x249);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'k': ਹਾਲਤ 0x137: ਹਾਲਤ 0x199: ਹਾਲਤ 0x1e9:
	    ਹਾਲਤ 0x1d84: ਹਾਲਤ 0x1e31: ਹਾਲਤ 0x1e33: ਹਾਲਤ 0x1e35:
	    ਹਾਲਤ 0x2c6a: ਹਾਲਤ 0xa741:
		      regmbc('k'); regmbc(0x137); regmbc(0x199);
		      regmbc(0x1e9); regmbc(0x1d84); regmbc(0x1e31);
		      regmbc(0x1e33); regmbc(0x1e35); regmbc(0x2c6a);
		      regmbc(0xa741);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'l': ਹਾਲਤ 0x13a: ਹਾਲਤ 0x13c: ਹਾਲਤ 0x13e:
	    ਹਾਲਤ 0x140: ਹਾਲਤ 0x142: ਹਾਲਤ 0x19a: ਹਾਲਤ 0x1e37:
	    ਹਾਲਤ 0x1e39: ਹਾਲਤ 0x1e3b: ਹਾਲਤ 0x1e3d: ਹਾਲਤ 0x2c61:
		      regmbc('l'); regmbc(0x13a); regmbc(0x13c);
		      regmbc(0x13e); regmbc(0x140); regmbc(0x142);
		      regmbc(0x19a); regmbc(0x1e37); regmbc(0x1e39);
		      regmbc(0x1e3b); regmbc(0x1e3d); regmbc(0x2c61);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'm': ਹਾਲਤ 0x1d6f: ਹਾਲਤ 0x1e3f: ਹਾਲਤ 0x1e41: ਹਾਲਤ 0x1e43:
		      regmbc('m'); regmbc(0x1d6f); regmbc(0x1e3f);
		      regmbc(0x1e41); regmbc(0x1e43);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'n': ਹਾਲਤ 0xf1: ਹਾਲਤ 0x144: ਹਾਲਤ 0x146: ਹਾਲਤ 0x148:
	    ਹਾਲਤ 0x149: ਹਾਲਤ 0x1f9: ਹਾਲਤ 0x1d70: ਹਾਲਤ 0x1d87:
	    ਹਾਲਤ 0x1e45: ਹਾਲਤ 0x1e47: ਹਾਲਤ 0x1e49: ਹਾਲਤ 0x1e4b:
	    ਹਾਲਤ 0xa7a5:
		      regmbc('n'); regmbc(0xf1); regmbc(0x144);
		      regmbc(0x146); regmbc(0x148); regmbc(0x149);
		      regmbc(0x1f9); regmbc(0x1d70); regmbc(0x1d87);
		      regmbc(0x1e45); regmbc(0x1e47); regmbc(0x1e49);
		      regmbc(0x1e4b); regmbc(0xa7a5);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'o': ਹਾਲਤ 0xf2: ਹਾਲਤ 0xf3: ਹਾਲਤ 0xf4: ਹਾਲਤ 0xf5:
	    ਹਾਲਤ 0xf6: ਹਾਲਤ 0xf8: ਹਾਲਤ 0x14d: ਹਾਲਤ 0x14f: ਹਾਲਤ 0x151:
	    ਹਾਲਤ 0x1a1: ਹਾਲਤ 0x1d2: ਹਾਲਤ 0x1eb: ਹਾਲਤ 0x1ed: ਹਾਲਤ 0x1ff:
	    ਹਾਲਤ 0x20d: ਹਾਲਤ 0x20f: ਹਾਲਤ 0x22b: ਹਾਲਤ 0x22d: ਹਾਲਤ 0x22f:
	    ਹਾਲਤ 0x231: ਹਾਲਤ 0x275: ਹਾਲਤ 0x1e4d: ਹਾਲਤ 0x1e4f:
	    ਹਾਲਤ 0x1e51: ਹਾਲਤ 0x1e53: ਹਾਲਤ 0x1ecd: ਹਾਲਤ 0x1ecf:
	    ਹਾਲਤ 0x1ed1: ਹਾਲਤ 0x1ed3: ਹਾਲਤ 0x1ed5: ਹਾਲਤ 0x1ed7:
	    ਹਾਲਤ 0x1ed9: ਹਾਲਤ 0x1edb: ਹਾਲਤ 0x1edd: ਹਾਲਤ 0x1edf:
	    ਹਾਲਤ 0x1ee1: ਹਾਲਤ 0x1ee3:
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
		      ਵਾਪਸ;
	    ਹਾਲਤ 'p': ਹਾਲਤ 0x1a5: ਹਾਲਤ 0x1d71: ਹਾਲਤ 0x1d88: ਹਾਲਤ 0x1d7d:
	    ਹਾਲਤ 0x1e55: ਹਾਲਤ 0x1e57:
		      regmbc('p'); regmbc(0x1a5); regmbc(0x1d71);
		      regmbc(0x1d7d); regmbc(0x1d88); regmbc(0x1e55);
		      regmbc(0x1e57);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'q': ਹਾਲਤ 0x24b: ਹਾਲਤ 0x2a0:
		      regmbc('q'); regmbc(0x24b); regmbc(0x2a0);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'r': ਹਾਲਤ 0x155: ਹਾਲਤ 0x157: ਹਾਲਤ 0x159: ਹਾਲਤ 0x211:
	    ਹਾਲਤ 0x213: ਹਾਲਤ 0x24d: ਹਾਲਤ 0x27d: ਹਾਲਤ 0x1d72: ਹਾਲਤ 0x1d73:
	    ਹਾਲਤ 0x1d89: ਹਾਲਤ 0x1e59: ਹਾਲਤ 0x1e5b: ਹਾਲਤ 0x1e5d: ਹਾਲਤ 0x1e5f:
	    ਹਾਲਤ 0xa7a7:
		      regmbc('r'); regmbc(0x155); regmbc(0x157);
		      regmbc(0x159); regmbc(0x211); regmbc(0x213);
		      regmbc(0x24d); regmbc(0x1d72); regmbc(0x1d73);
		      regmbc(0x1d89); regmbc(0x1e59); regmbc(0x27d);
		      regmbc(0x1e5b); regmbc(0x1e5d); regmbc(0x1e5f);
		      regmbc(0xa7a7);
		      ਵਾਪਸ;
	    ਹਾਲਤ 's': ਹਾਲਤ 0x15b: ਹਾਲਤ 0x15d: ਹਾਲਤ 0x15f: ਹਾਲਤ 0x161:
	    ਹਾਲਤ 0x1e61: ਹਾਲਤ 0x219: ਹਾਲਤ 0x23f: ਹਾਲਤ 0x1d74: ਹਾਲਤ 0x1d8a:
	    ਹਾਲਤ 0x1e63: ਹਾਲਤ 0x1e65: ਹਾਲਤ 0x1e67: ਹਾਲਤ 0x1e69: ਹਾਲਤ 0xa7a9:
		      regmbc('s'); regmbc(0x15b); regmbc(0x15d);
		      regmbc(0x15f); regmbc(0x161); regmbc(0x23f);
		      regmbc(0x219); regmbc(0x1d74); regmbc(0x1d8a);
		      regmbc(0x1e61); regmbc(0x1e63); regmbc(0x1e65);
		      regmbc(0x1e67); regmbc(0x1e69); regmbc(0xa7a9);
		      ਵਾਪਸ;
	    ਹਾਲਤ 't': ਹਾਲਤ 0x163: ਹਾਲਤ 0x165: ਹਾਲਤ 0x167: ਹਾਲਤ 0x1ab:
	    ਹਾਲਤ 0x1ad: ਹਾਲਤ 0x21b: ਹਾਲਤ 0x288: ਹਾਲਤ 0x1d75: ਹਾਲਤ 0x1e6b:
	    ਹਾਲਤ 0x1e6d: ਹਾਲਤ 0x1e6f: ਹਾਲਤ 0x1e71: ਹਾਲਤ 0x1e97: ਹਾਲਤ 0x2c66:
		      regmbc('t'); regmbc(0x163); regmbc(0x165);
		      regmbc(0x167); regmbc(0x1ab); regmbc(0x21b);
		      regmbc(0x1ad); regmbc(0x288); regmbc(0x1d75);
		      regmbc(0x1e6b); regmbc(0x1e6d); regmbc(0x1e6f);
		      regmbc(0x1e71); regmbc(0x1e97); regmbc(0x2c66);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'u': ਹਾਲਤ 0xf9: ਹਾਲਤ 0xfa: ਹਾਲਤ 0xfb: ਹਾਲਤ 0xfc:
	    ਹਾਲਤ 0x169: ਹਾਲਤ 0x16b: ਹਾਲਤ 0x16d: ਹਾਲਤ 0x16f:
	    ਹਾਲਤ 0x171: ਹਾਲਤ 0x173: ਹਾਲਤ 0x1b0: ਹਾਲਤ 0x1d4:
	    ਹਾਲਤ 0x1d6: ਹਾਲਤ 0x1d8: ਹਾਲਤ 0x1da: ਹਾਲਤ 0x1dc:
	    ਹਾਲਤ 0x215: ਹਾਲਤ 0x217: ਹਾਲਤ 0x289: ਹਾਲਤ 0x1e73:
	    ਹਾਲਤ 0x1d7e: ਹਾਲਤ 0x1d99: ਹਾਲਤ 0x1e75: ਹਾਲਤ 0x1e77:
	    ਹਾਲਤ 0x1e79: ਹਾਲਤ 0x1e7b: ਹਾਲਤ 0x1ee5: ਹਾਲਤ 0x1ee7:
	    ਹਾਲਤ 0x1ee9: ਹਾਲਤ 0x1eeb: ਹਾਲਤ 0x1eed: ਹਾਲਤ 0x1eef:
	    ਹਾਲਤ 0x1ef1:
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
		      ਵਾਪਸ;
	    ਹਾਲਤ 'v': ਹਾਲਤ 0x28b: ਹਾਲਤ 0x1d8c: ਹਾਲਤ 0x1e7d: ਹਾਲਤ 0x1e7f:
		      regmbc('v'); regmbc(0x28b); regmbc(0x1d8c);
		      regmbc(0x1e7d); regmbc(0x1e7f);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'w': ਹਾਲਤ 0x175: ਹਾਲਤ 0x1e81: ਹਾਲਤ 0x1e83:
	    ਹਾਲਤ 0x1e85: ਹਾਲਤ 0x1e87: ਹਾਲਤ 0x1e89: ਹਾਲਤ 0x1e98:
		      regmbc('w'); regmbc(0x175); regmbc(0x1e81);
		      regmbc(0x1e83); regmbc(0x1e85); regmbc(0x1e87);
		      regmbc(0x1e89); regmbc(0x1e98);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'x': ਹਾਲਤ 0x1e8b: ਹਾਲਤ 0x1e8d:
		      regmbc('x'); regmbc(0x1e8b); regmbc(0x1e8d);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'y': ਹਾਲਤ 0xfd: ਹਾਲਤ 0xff: ਹਾਲਤ 0x177: ਹਾਲਤ 0x1b4:
	    ਹਾਲਤ 0x233: ਹਾਲਤ 0x24f: ਹਾਲਤ 0x1e8f: ਹਾਲਤ 0x1e99: ਹਾਲਤ 0x1ef3:
	    ਹਾਲਤ 0x1ef5: ਹਾਲਤ 0x1ef7: ਹਾਲਤ 0x1ef9:
		      regmbc('y'); regmbc(0xfd); regmbc(0xff);
		      regmbc(0x177); regmbc(0x1b4); regmbc(0x233);
		      regmbc(0x24f); regmbc(0x1e8f); regmbc(0x1e99);
		      regmbc(0x1ef3); regmbc(0x1ef5); regmbc(0x1ef7);
		      regmbc(0x1ef9);
		      ਵਾਪਸ;
	    ਹਾਲਤ 'z': ਹਾਲਤ 0x17a: ਹਾਲਤ 0x17c: ਹਾਲਤ 0x17e: ਹਾਲਤ 0x1b6:
	    ਹਾਲਤ 0x1d76: ਹਾਲਤ 0x1d8e: ਹਾਲਤ 0x1e91: ਹਾਲਤ 0x1e93:
	    ਹਾਲਤ 0x1e95: ਹਾਲਤ 0x2c6c:
		      regmbc('z'); regmbc(0x17a); regmbc(0x17c);
		      regmbc(0x17e); regmbc(0x1b6); regmbc(0x1d76);
		      regmbc(0x1d8e); regmbc(0x1e91); regmbc(0x1e93);
		      regmbc(0x1e95); regmbc(0x2c6c);
		      ਵਾਪਸ;
	}
    }
    regmbc(c);
}

/*
 * Emit a node.
 * Return pointer to generated code.
 */
    ਸਥਿਰ char_u *
regnode(ਪੂਰਨ op)
{
    char_u  *ret;

    ret = regcode;
    ਜੇ (ret == JUST_CALC_ਮਾਪ)
	regsize += 3;
    ਨਹੀਂਤਾਂ
    {
	*regcode++ = op;
	*regcode++ = NUL;		// Null "next" pointer.
	*regcode++ = NUL;
    }
    ਵਾਪਸ ret;
}

/*
 * Write a long as four bytes at "p" and return pointer to the next char.
 */
    ਸਥਿਰ char_u *
re_put_long(char_u *p, long_u val)
{
    *p++ = (char_u) ((val >> 24) & 0377);
    *p++ = (char_u) ((val >> 16) & 0377);
    *p++ = (char_u) ((val >> 8) & 0377);
    *p++ = (char_u) (val & 0377);
    ਵਾਪਸ p;
}

/*
 * regnext - dig the "next" pointer out of a node
 * Returns NULL when calculating size, when there is no next item and when
 * there is an error.
 */
    ਸਥਿਰ char_u *
regnext(char_u *p)
{
    ਪੂਰਨ	    ਵਿੱਥ;

    ਜੇ (p == JUST_CALC_ਮਾਪ || reg_toolong)
	ਵਾਪਸ ਨਿਰਾ;

    ਵਿੱਥ = ਅਗਲਾ_2(p);
    ਜੇ (ਵਿੱਥ == 0)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (OP(p) == ਪਿੱਠ_2)
	ਵਾਪਸ p - ਵਿੱਥ;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ p + ਵਿੱਥ;
}

/*
 * Set the next-pointer at the end of a node chain.
 */
    ਸਥਿਰ ਖਾਲੀ
regtail(char_u *p, char_u *val)
{
    char_u	*ਜਾਂਚ;
    char_u	*ਆਰਜ਼ੀ;
    ਪੂਰਨ		ਵਿੱਥ;

    ਜੇ (p == JUST_CALC_ਮਾਪ)
	ਵਾਪਸ;

    // Find last node.
    ਜਾਂਚ = p;
    ਲਈ (;;)
    {
	ਆਰਜ਼ੀ = regnext(ਜਾਂਚ);
	ਜੇ (ਆਰਜ਼ੀ == ਨਿਰਾ)
	    ਰੋਕੋ;
	ਜਾਂਚ = ਆਰਜ਼ੀ;
    }

    ਜੇ (OP(ਜਾਂਚ) == ਪਿੱਠ_2)
	ਵਿੱਥ = (ਪੂਰਨ)(ਜਾਂਚ - val);
    ਨਹੀਂਤਾਂ
	ਵਿੱਥ = (ਪੂਰਨ)(val - ਜਾਂਚ);
    // When the offset uses more than 16 bits it can no longer fit in the two
    // bytes available.  Use a global flag to avoid having to check return
    // values in too many places.
    ਜੇ (ਵਿੱਥ > 0xffff)
	reg_toolong = TRUE;
    ਨਹੀਂਤਾਂ
    {
	*(ਜਾਂਚ + 1) = (char_u) (((ਬਿਨਾਨਿਸ਼ਾਨੀ)ਵਿੱਥ >> 8) & 0377);
	*(ਜਾਂਚ + 2) = (char_u) (ਵਿੱਥ & 0377);
    }
}

/*
 * Like regtail, on item after a BRANCH; nop if none.
 */
    ਸਥਿਰ ਖਾਲੀ
regoptail(char_u *p, char_u *val)
{
    // When op is neither BRANCH nor BRACE_COMPLEX0-9, it is "operandless"
    ਜੇ (p == ਨਿਰਾ || p == JUST_CALC_ਮਾਪ
	    || (OP(p) != BRANCH
		&& (OP(p) < BRACE_COMPLEX || OP(p) > BRACE_COMPLEX + 9)))
	ਵਾਪਸ;
    regtail(ਸੰਕਾਰਜ(p), val);
}

/*
 * Insert an operator in front of already-emitted operand
 *
 * Means relocating the operand.
 */
    ਸਥਿਰ ਖਾਲੀ
reginsert(ਪੂਰਨ op, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*ਰੱਖੋ;

    ਜੇ (regcode == JUST_CALC_ਮਾਪ)
    {
	regsize += 3;
	ਵਾਪਸ;
    }
    src = regcode;
    regcode += 3;
    dst = regcode;
    ਜਦੋਂ (src > opnd)
	*--dst = *--src;

    ਰੱਖੋ = opnd;		// Op node, where operand used to be.
    *ਰੱਖੋ++ = op;
    *ਰੱਖੋ++ = NUL;
    *ਰੱਖੋ = NUL;
}

/*
 * Insert an operator in front of already-emitted operand.
 * Add a number to the operator.
 */
    ਸਥਿਰ ਖਾਲੀ
reginsert_nr(ਪੂਰਨ op, ਲੰਮਾ val, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*ਰੱਖੋ;

    ਜੇ (regcode == JUST_CALC_ਮਾਪ)
    {
	regsize += 7;
	ਵਾਪਸ;
    }
    src = regcode;
    regcode += 7;
    dst = regcode;
    ਜਦੋਂ (src > opnd)
	*--dst = *--src;

    ਰੱਖੋ = opnd;		// Op node, where operand used to be.
    *ਰੱਖੋ++ = op;
    *ਰੱਖੋ++ = NUL;
    *ਰੱਖੋ++ = NUL;
    re_put_long(ਰੱਖੋ, (long_u)val);
}

/*
 * Insert an operator in front of already-emitted operand.
 * The operator has the given limit values as operands.  Also set next pointer.
 *
 * Means relocating the operand.
 */
    ਸਥਿਰ ਖਾਲੀ
reginsert_limits(
    ਪੂਰਨ		op,
    ਲੰਮਾ	minval,
    ਲੰਮਾ	maxval,
    char_u	*opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*ਰੱਖੋ;

    ਜੇ (regcode == JUST_CALC_ਮਾਪ)
    {
	regsize += 11;
	ਵਾਪਸ;
    }
    src = regcode;
    regcode += 11;
    dst = regcode;
    ਜਦੋਂ (src > opnd)
	*--dst = *--src;

    ਰੱਖੋ = opnd;		// Op node, where operand used to be.
    *ਰੱਖੋ++ = op;
    *ਰੱਖੋ++ = NUL;
    *ਰੱਖੋ++ = NUL;
    ਰੱਖੋ = re_put_long(ਰੱਖੋ, (long_u)minval);
    ਰੱਖੋ = re_put_long(ਰੱਖੋ, (long_u)maxval);
    regtail(opnd, ਰੱਖੋ);
}

/*
 * Return TRUE if the back reference is legal. We must have seen the close
 * brace.
 * TODO: Should also check that we don't refer to something that is repeated
 * (+*=): what instance of the repetition should we match?
 */
    ਸਥਿਰ ਪੂਰਨ
ਵੇਖਿਆ_endbrace(ਪੂਰਨ refnum)
{
    ਜੇ (!had_endbrace[refnum])
    {
	char_u *p;

	// Trick: check if "@<=" or "@<!" follows, in which case
	// the \1 can appear before the referenced match.
	ਲਈ (p = regparse; *p != NUL; ++p)
	    ਜੇ (p[0] == '@' && p[1] == '<' && (p[2] == '!' || p[2] == '='))
		ਰੋਕੋ;
	ਜੇ (*p == NUL)
	{
	    emsg(_(e_illegal_ਪਿੱਠ_reference));
	    rc_did_emsg = TRUE;
	    ਵਾਪਸ FALSE;
	}
    }
    ਵਾਪਸ TRUE;
}

/*
 * Parse the lowest level.
 *
 * Optimization:  gobbles an entire sequence of ordinary characters so that
 * it can turn them into a single node, which is smaller to store and
 * faster to run.  Don't do this when one_exactly is set.
 */
    ਸਥਿਰ char_u *
regatom(ਪੂਰਨ *flagp)
{
    char_u	    *ret;
    ਪੂਰਨ		    flags;
    ਪੂਰਨ		    c;
    char_u	    *p;
    ਪੂਰਨ		    extra = 0;
    ਪੂਰਨ		    save_prev_at_ਸ਼ੁਰੂ = prev_at_ਸ਼ੁਰੂ;

    *flagp = WORST;		// Tentatively.

    c = getchr();
    ਚੋਣ (c)
    {
      ਹਾਲਤ Magic('^'):
	ret = regnode(BOL);
	ਰੋਕੋ;

      ਹਾਲਤ Magic('$'):
	ret = regnode(EOL);
#ਜੇ defined(FEAT_SYN_HL)
	had_eol = TRUE;
#ਜੇ_ਅੰਤ
	ਰੋਕੋ;

      ਹਾਲਤ Magic('<'):
	ret = regnode(BOW);
	ਰੋਕੋ;

      ਹਾਲਤ Magic('>'):
	ret = regnode(EOW);
	ਰੋਕੋ;

      ਹਾਲਤ Magic('_'):
	c = no_Magic(getchr());
	ਜੇ (c == '^')		// "\_^" is start-of-line
	{
	    ret = regnode(BOL);
	    ਰੋਕੋ;
	}
	ਜੇ (c == '$')		// "\_$" is end-of-line
	{
	    ret = regnode(EOL);
#ਜੇ defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#ਜੇ_ਅੰਤ
	    ਰੋਕੋ;
	}

	extra = ਜੋੜੋ_NL;
	*flagp |= HASNL;

	// "\_[" is character range plus newline
	ਜੇ (c == '[')
	    ਜਾਓ collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	// Character classes.
      ਹਾਲਤ Magic('.'):
      ਹਾਲਤ Magic('i'):
      ਹਾਲਤ Magic('I'):
      ਹਾਲਤ Magic('k'):
      ਹਾਲਤ Magic('K'):
      ਹਾਲਤ Magic('f'):
      ਹਾਲਤ Magic('F'):
      ਹਾਲਤ Magic('p'):
      ਹਾਲਤ Magic('P'):
      ਹਾਲਤ Magic('s'):
      ਹਾਲਤ Magic('S'):
      ਹਾਲਤ Magic('d'):
      ਹਾਲਤ Magic('D'):
      ਹਾਲਤ Magic('x'):
      ਹਾਲਤ Magic('X'):
      ਹਾਲਤ Magic('o'):
      ਹਾਲਤ Magic('O'):
      ਹਾਲਤ Magic('w'):
      ਹਾਲਤ Magic('W'):
      ਹਾਲਤ Magic('h'):
      ਹਾਲਤ Magic('H'):
      ਹਾਲਤ Magic('a'):
      ਹਾਲਤ Magic('A'):
      ਹਾਲਤ Magic('l'):
      ਹਾਲਤ Magic('L'):
      ਹਾਲਤ Magic('u'):
      ਹਾਲਤ Magic('U'):
	p = vim_strchr(classchars, no_Magic(c));
	ਜੇ (p == ਨਿਰਾ)
	    EMSG_RET_NULL(_(e_invalid_use_ਦਾ_underscore));

	// When '.' is followed by a composing char ignore the dot, so that
	// the composing char is matched here.
	ਜੇ (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	{
	    c = getchr();
	    ਜਾਓ do_multibyte;
	}
	ret = regnode(classcodes[p - classchars] + extra);
	*flagp |= HASWIDTH | SIMPLE;
	ਰੋਕੋ;

      ਹਾਲਤ Magic('n'):
	ਜੇ (reg_string)
	{
	    // In a string "\n" matches a newline character.
	    ret = regnode(EXACTLY);
	    regc(NL);
	    regc(NUL);
	    *flagp |= HASWIDTH | SIMPLE;
	}
	ਨਹੀਂਤਾਂ
	{
	    // In buffer text "\n" matches the end of a line.
	    ret = regnode(NEWL);
	    *flagp |= HASWIDTH | HASNL;
	}
	ਰੋਕੋ;

      ਹਾਲਤ Magic('('):
	ਜੇ (one_exactly)
	    EMSG_ONE_RET_NULL;
	ret = reg(REG_PAREN, &flags);
	ਜੇ (ret == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	*flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
	ਰੋਕੋ;

      ਹਾਲਤ NUL:
      ਹਾਲਤ Magic('|'):
      ਹਾਲਤ Magic('&'):
      ਹਾਲਤ Magic(')'):
	ਜੇ (one_exactly)
	    EMSG_ONE_RET_NULL;
	// Supposed to be caught earlier.
	IEMSG_RET_NULL(e_ਅੰਦਰੂਨੀ_error_ਵਿੱਚ_regexp);
	// NOTREACHED

      ਹਾਲਤ Magic('='):
      ਹਾਲਤ Magic('?'):
      ਹਾਲਤ Magic('+'):
      ਹਾਲਤ Magic('@'):
      ਹਾਲਤ Magic('{'):
      ਹਾਲਤ Magic('*'):
	c = no_Magic(c);
	EMSG3_RET_NULL(_(e_str_chr_follows_nothing),
		(c == '*' ? reg_magic >= MAGIC_ON : reg_magic == MAGIC_ALL), c);
	// NOTREACHED

      ਹਾਲਤ Magic('~'):		// previous substitute pattern
	    ਜੇ (reg_prev_sub != ਨਿਰਾ)
	    {
		char_u	    *lp;

		ret = regnode(EXACTLY);
		lp = reg_prev_sub;
		ਜਦੋਂ (*lp != NUL)
		    regc(*lp++);
		regc(NUL);
		ਜੇ (*reg_prev_sub != NUL)
		{
		    *flagp |= HASWIDTH;
		    ਜੇ ((lp - reg_prev_sub) == 1)
			*flagp |= SIMPLE;
		}
	    }
	    ਨਹੀਂਤਾਂ
		EMSG_RET_NULL(_(e_no_ਪਿਛਲਾ_substitute_regular_ਵਿਅੰਜਕ));
	    ਰੋਕੋ;

      ਹਾਲਤ Magic('1'):
      ਹਾਲਤ Magic('2'):
      ਹਾਲਤ Magic('3'):
      ਹਾਲਤ Magic('4'):
      ਹਾਲਤ Magic('5'):
      ਹਾਲਤ Magic('6'):
      ਹਾਲਤ Magic('7'):
      ਹਾਲਤ Magic('8'):
      ਹਾਲਤ Magic('9'):
	    {
		ਪੂਰਨ		    refnum;

		refnum = c - Magic('0');
		ਜੇ (!ਵੇਖਿਆ_endbrace(refnum))
		    ਵਾਪਸ ਨਿਰਾ;
		ret = regnode(BACKREF + refnum);
	    }
	    ਰੋਕੋ;

      ਹਾਲਤ Magic('z'):
	{
	    c = no_Magic(getchr());
	    ਚੋਣ (c)
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
		ਹਾਲਤ '(': ਜੇ ((reg_do_extmatch & REX_ਸੈੱਟ) == 0)
			      EMSG_RET_NULL(_(e_z_not_allowed_ਇੱਥੇ));
			  ਜੇ (one_exactly)
			      EMSG_ONE_RET_NULL;
			  ret = reg(REG_ZPAREN, &flags);
			  ਜੇ (ret == ਨਿਰਾ)
			      ਵਾਪਸ ਨਿਰਾ;
			  *flagp |= flags & (HASWIDTH|SPSTART|HASNL|HASLOOKBH);
			  re_has_z = REX_ਸੈੱਟ;
			  ਰੋਕੋ;

		ਹਾਲਤ '1':
		ਹਾਲਤ '2':
		ਹਾਲਤ '3':
		ਹਾਲਤ '4':
		ਹਾਲਤ '5':
		ਹਾਲਤ '6':
		ਹਾਲਤ '7':
		ਹਾਲਤ '8':
		ਹਾਲਤ '9': ਜੇ ((reg_do_extmatch & REX_USE) == 0)
			      EMSG_RET_NULL(_(e_z1_z9_not_allowed_ਇੱਥੇ));
			  ret = regnode(ZREF + c - '0');
			  re_has_z = REX_USE;
			  ਰੋਕੋ;
#ਜੇ_ਅੰਤ

		ਹਾਲਤ 's': ret = regnode(MOPEN + 0);
			  ਜੇ (re_mult_ਅਗਲਾ("\\zs") == FAIL)
			      ਵਾਪਸ ਨਿਰਾ;
			  ਰੋਕੋ;

		ਹਾਲਤ 'e': ret = regnode(MCLOSE + 0);
			  ਜੇ (re_mult_ਅਗਲਾ("\\ze") == FAIL)
			      ਵਾਪਸ ਨਿਰਾ;
			  ਰੋਕੋ;

		ਮੂਲ:  EMSG_RET_NULL(_(e_invalid_character_after_bsl_z));
	    }
	}
	ਰੋਕੋ;

      ਹਾਲਤ Magic('%'):
	{
	    c = no_Magic(getchr());
	    ਚੋਣ (c)
	    {
		// () without a back reference
		ਹਾਲਤ '(':
		    ਜੇ (one_exactly)
			EMSG_ONE_RET_NULL;
		    ret = reg(REG_NPAREN, &flags);
		    ਜੇ (ret == ਨਿਰਾ)
			ਵਾਪਸ ਨਿਰਾ;
		    *flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
		    ਰੋਕੋ;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		ਹਾਲਤ '^':
		    ret = regnode(RE_BOF);
		    ਰੋਕੋ;

		ਹਾਲਤ '$':
		    ret = regnode(RE_EOF);
		    ਰੋਕੋ;

		ਹਾਲਤ '#':
		    ਜੇ (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_ਸ਼ੁਰੂ_ਦਾ_ਨਮੂਨਾ),
								  regparse[1]);
			ਵਾਪਸ FAIL;
		    }
		    ret = regnode(CURSOR);
		    ਰੋਕੋ;

		ਹਾਲਤ 'V':
		    ret = regnode(RE_VISUAL);
		    ਰੋਕੋ;

		ਹਾਲਤ 'C':
		    ret = regnode(RE_COMPOSING);
		    ਰੋਕੋ;

		// \%[abc]: Emit as a list of branches, all ending at the last
		// branch which matches nothing.
		ਹਾਲਤ '[':
			  ਜੇ (one_exactly)	// doesn't nest
			      EMSG_ONE_RET_NULL;
			  {
			      char_u	*lastbranch;
			      char_u	*lastnode = ਨਿਰਾ;
			      char_u	*br;

			      ret = ਨਿਰਾ;
			      ਜਦੋਂ ((c = getchr()) != ']')
			      {
				  ਜੇ (c == NUL)
				      EMSG2_RET_NULL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
				  br = regnode(BRANCH);
				  ਜੇ (ret == ਨਿਰਾ)
				      ret = br;
				  ਨਹੀਂਤਾਂ
				  {
				      regtail(lastnode, br);
				      ਜੇ (reg_toolong)
					  ਵਾਪਸ ਨਿਰਾ;
				  }

				  ungetchr();
				  one_exactly = TRUE;
				  lastnode = regatom(flagp);
				  one_exactly = FALSE;
				  ਜੇ (lastnode == ਨਿਰਾ)
				      ਵਾਪਸ ਨਿਰਾ;
			      }
			      ਜੇ (ret == ਨਿਰਾ)
				  EMSG2_RET_NULL(_(e_ਖਾਲੀ_ਥਾਵਾਂ_str_brackets),
						      reg_magic == MAGIC_ALL);
			      lastbranch = regnode(BRANCH);
			      br = regnode(NOTHING);
			      ਜੇ (ret != JUST_CALC_ਮਾਪ)
			      {
				  regtail(lastnode, br);
				  regtail(lastbranch, br);
				  // connect all branches to the NOTHING
				  // branch at the end
				  ਲਈ (br = ret; br != lastnode; )
				  {
				      ਜੇ (OP(br) == BRANCH)
				      {
					  regtail(br, lastbranch);
					  ਜੇ (reg_toolong)
					      ਵਾਪਸ ਨਿਰਾ;
					  br = ਸੰਕਾਰਜ(br);
				      }
				      ਨਹੀਂਤਾਂ
					  br = regnext(br);
				  }
			      }
			      *flagp &= ~(HASWIDTH | SIMPLE);
			      ਰੋਕੋ;
			  }

		ਹਾਲਤ 'd':   // %d123 decimal
		ਹਾਲਤ 'o':   // %o123 octal
		ਹਾਲਤ 'x':   // %xab hex 2
		ਹਾਲਤ 'u':   // %uabcd hex 4
		ਹਾਲਤ 'U':   // %U1234abcd hex 8
			  {
			      vimlong_T i;

			      ਚੋਣ (c)
			      {
				  ਹਾਲਤ 'd': i = getdecchrs(); ਰੋਕੋ;
				  ਹਾਲਤ 'o': i = getoctchrs(); ਰੋਕੋ;
				  ਹਾਲਤ 'x': i = gethexchrs(2); ਰੋਕੋ;
				  ਹਾਲਤ 'u': i = gethexchrs(4); ਰੋਕੋ;
				  ਹਾਲਤ 'U': i = gethexchrs(8); ਰੋਕੋ;
				  ਮੂਲ:  i = -1; ਰੋਕੋ;
			      }

			      ਜੇ (i < 0 || i > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
				  EMSG2_RET_NULL(
					    _(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			      ਜੇ (use_multibytecode(i))
				  ret = regnode(MULTIBYTECODE);
			      ਨਹੀਂਤਾਂ
				  ret = regnode(EXACTLY);
			      ਜੇ (i == 0)
				  regc(0x0a);
			      ਨਹੀਂਤਾਂ
				  regmbc((ਪੂਰਨ)i);
			      regc(NUL);
			      *flagp |= HASWIDTH;
			      ਰੋਕੋ;
			  }

		ਮੂਲ:
			  ਜੇ (VIM_ISDIGIT(c) || c == '<' || c == '>'
						|| c == '\'' || c == '.')
			  {
			      long_u	n = 0;
			      ਪੂਰਨ	cmp;
			      ਪੂਰਨ	cur = FALSE;
			      ਪੂਰਨ	got_digit = FALSE;

			      cmp = c;
			      ਜੇ (cmp == '<' || cmp == '>')
				  c = getchr();
			      ਜੇ (no_Magic(c) == '.')
			      {
				  cur = TRUE;
				  c = getchr();
			      }
			      ਜਦੋਂ (VIM_ISDIGIT(c))
			      {
				  got_digit = TRUE;
				  n = n * 10 + (c - '0');
				  c = getchr();
			      }
			      ਜੇ (no_Magic(c) == '\'' && n == 0)
			      {
				  // "\%'m", "\%<'m" and "\%>'m": Mark
				  c = getchr();
				  ret = regnode(RE_MARK);
				  ਜੇ (ret == JUST_CALC_ਮਾਪ)
				      regsize += 2;
				  ਨਹੀਂਤਾਂ
				  {
				      *regcode++ = c;
				      *regcode++ = cmp;
				  }
				  ਰੋਕੋ;
			      }
			      ਨਹੀਂਤਾਂ ਜੇ ((c == 'l' || c == 'c' || c == 'v')
					  && (cur || got_digit))
			      {
				  ਜੇ (cur && n)
				  {
				    semsg(_(e_regexp_ਸੰਖਿਆ_after_dot_pos_ਖੋਜ_chr),
								  no_Magic(c));
				    rc_did_emsg = TRUE;
				    ਵਾਪਸ ਨਿਰਾ;
				  }
				  ਜੇ (c == 'l')
				  {
				      ਜੇ (cur)
					  n = curwin->w_cursor.lnum;
				      ret = regnode(RE_LNUM);
				      ਜੇ (save_prev_at_ਸ਼ੁਰੂ)
					  at_ਸ਼ੁਰੂ = TRUE;
				  }
				  ਨਹੀਂਤਾਂ ਜੇ (c == 'c')
				  {
				      ਜੇ (cur)
				      {
					  n = curwin->w_cursor.ਖਾਨਾ;
					  n++;
				      }
				      ret = regnode(RE_ਖਾਨਾ);
				  }
				  ਨਹੀਂਤਾਂ
				  {
				      ਜੇ (cur)
				      {
					  colnr_T vcol = 0;

					  getvvcol(curwin, &curwin->w_cursor,
							 ਨਿਰਾ, ਨਿਰਾ, &vcol, 0);
					  ++vcol;
					  n = vcol;
				      }
				      ret = regnode(RE_VCOL);
				  }
				  ਜੇ (ret == JUST_CALC_ਮਾਪ)
				      regsize += 5;
				  ਨਹੀਂਤਾਂ
				  {
				      // put the number and the optional
				      // comparator after the opcode
				      regcode = re_put_long(regcode, n);
				      *regcode++ = cmp;
				  }
				  ਰੋਕੋ;
			      }
			  }

			  EMSG2_RET_NULL(_(e_invalid_character_after_str),
						      reg_magic == MAGIC_ALL);
	    }
	}
	ਰੋਕੋ;

      ਹਾਲਤ Magic('['):
collection:
	{
	    char_u	*lp;

	    // If there is no matching ']', we assume the '[' is a normal
	    // character.  This makes 'incsearch' and ":help [" work.
	    lp = skip_anyof(regparse);
	    ਜੇ (*lp == ']')	// there is a matching ']'
	    {
		ਪੂਰਨ	startc = -1;	// > 0 when next '-' is a range
		ਪੂਰਨ	endc;

		ਜੇ (extra == 0)
		{
		    ਪੂਰਨ	cl = bt_recognize_char_class(regparse, lp);

		    ਜੇ (cl != 0)
		    {
			ret = regnode(cl);
			regparse = lp;
			skipchr();
			*flagp |= HASWIDTH | SIMPLE;
			ਰੋਕੋ;
		    }
		}

		// In a character class, different parsing rules apply.
		// Not even \ is special anymore, nothing is.
		ਜੇ (*regparse == '^')	    // Complement of range.
		{
		    ret = regnode(ANYBUT + extra);
		    regparse++;
		}
		ਨਹੀਂਤਾਂ
		    ret = regnode(ANYOF + extra);

		// At the start ']' and '-' mean the literal character.
		ਜੇ (*regparse == ']' || *regparse == '-')
		{
		    startc = *regparse;
		    regc(*regparse++);
		}

		ਜਦੋਂ (*regparse != NUL && *regparse != ']')
		{
		    ਜੇ (*regparse == '-')
		    {
			++regparse;
			// The '-' is not used for a range at the end and
			// after or before a '\n'.
			ਜੇ (*regparse == ']' || *regparse == NUL
				|| startc == -1
				|| (regparse[0] == '\\' && regparse[1] == 'n'))
			{
			    regc('-');
			    startc = '-';	// [--x] is a range
			}
			ਨਹੀਂਤਾਂ
			{
			    // Also accept "a-[.z.]"
			    endc = 0;
			    ਜੇ (*regparse == '[')
				endc = get_coll_element(&regparse);
			    ਜੇ (endc == 0)
			    {
				ਜੇ (has_mbyte)
				    endc = mb_ptr2char_adv(&regparse);
				ਨਹੀਂਤਾਂ
				    endc = *regparse++;
			    }

			    // Handle \o40, \x20 and \u20AC style sequences
			    ਜੇ (endc == '\\' && !reg_cpo_lit && !reg_cpo_bsl)
				endc = coll_get_char();

			    ਜੇ (startc > endc)
				EMSG_RET_NULL(_(e_ਉਲਟ_range_ਵਿੱਚ_character_class));
			    ਜੇ (has_mbyte && ((*mb_char2len)(startc) > 1
						 || (*mb_char2len)(endc) > 1))
			    {
				// Limit to a range of 256 chars.
				ਜੇ (endc > startc + 256)
				    EMSG_RET_NULL(_(e_range_too_large_ਵਿੱਚ_character_class));
				ਜਦੋਂ (++startc <= endc)
				    regmbc(startc);
			    }
			    ਨਹੀਂਤਾਂ
			    {
				ਜਦੋਂ (++startc <= endc)
				    regc(startc);
			    }
			    startc = -1;
			}
		    }
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    ਨਹੀਂਤਾਂ ਜੇ (*regparse == '\\'
			    && !reg_cpo_bsl
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != ਨਿਰਾ
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR,
						       regparse[1]) != ਨਿਰਾ)))
		    {
			regparse++;
			ਜੇ (*regparse == 'n')
			{
			    // '\n' in range: also match NL
			    ਜੇ (ret != JUST_CALC_ਮਾਪ)
			    {
				// Using \n inside [^] does not change what
				// matches. "[^\n]" is the same as ".".
				ਜੇ (*ret == ANYOF)
				{
				    *ret = ANYOF + ਜੋੜੋ_NL;
				    *flagp |= HASNL;
				}
				// else: must have had a \n already
			    }
			    regparse++;
			    startc = -1;
			}
			ਨਹੀਂਤਾਂ ਜੇ (*regparse == 'd'
				|| *regparse == 'o'
				|| *regparse == 'x'
				|| *regparse == 'u'
				|| *regparse == 'U')
			{
			    startc = coll_get_char();
			    // max UTF-8 Codepoint is U+10FFFF,
			    // but allow values until INT_MAX
			    ਜੇ (startc == ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
				EMSG_RET_NULL(_(e_unicode_val_too_large));
			    ਜੇ (startc == 0)
				regc(0x0a);
			    ਨਹੀਂਤਾਂ
				regmbc(startc);
			}
			ਨਹੀਂਤਾਂ
			{
			    startc = backslash_trans(*regparse++);
			    regc(startc);
			}
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (*regparse == '[')
		    {
			ਪੂਰਨ c_class;
			ਪੂਰਨ cu;

			c_class = get_char_class(&regparse);
			startc = -1;
			// Characters assumed to be 8 bits!
			ਚੋਣ (c_class)
			{
			    ਹਾਲਤ CLASS_NONE:
				c_class = get_equi_class(&regparse);
				ਜੇ (c_class != 0)
				{
				    // produce equivalence class
				    reg_equi_class(c_class);
				}
				ਨਹੀਂਤਾਂ ਜੇ ((c_class =
					    get_coll_element(&regparse)) != 0)
				{
				    // produce a collating element
				    regmbc(c_class);
				}
				ਨਹੀਂਤਾਂ
				{
				    // literal '[', allow [[-x] as a range
				    startc = *regparse++;
				    regc(startc);
				}
				ਰੋਕੋ;
			    ਹਾਲਤ CLASS_ALNUM:
				ਲਈ (cu = 1; cu < 128; cu++)
				    ਜੇ (ਅੱਖਰ_ਜਾਂ_ਅੰਕ(cu))
					regmbc(cu);
				ਰੋਕੋ;
			    ਹਾਲਤ CLASS_ALPHA:
				ਲਈ (cu = 1; cu < 128; cu++)
				    ਜੇ (ਅੱਖਰ_ਹੈ(cu))
					regmbc(cu);
				ਰੋਕੋ;
			    ਹਾਲਤ CLASS_BLANK:
				regc(' ');
				regc('\t');
				ਰੋਕੋ;
			    ਹਾਲਤ CLASS_CNTRL:
				ਲਈ (cu = 1; cu <= 127; cu++)
				    ਜੇ (iscntrl(cu))
					regmbc(cu);
				ਰੋਕੋ;
			    ਹਾਲਤ CLASS_DIGIT:
				ਲਈ (cu = 1; cu <= 127; cu++)
				    ਜੇ (VIM_ISDIGIT(cu))
					regmbc(cu);
				ਰੋਕੋ;
			    ਹਾਲਤ CLASS_ਗ੍ਰਾਫ:
				ਲਈ (cu = 1; cu <= 127; cu++)
				    ਜੇ (isgraph(cu))
					regmbc(cu);
				ਰੋਕੋ;
			    ਹਾਲਤ CLASS_ਹੇਠਲਾ:
				ਲਈ (cu = 1; cu <= 255; cu++)
				    ਜੇ (MB_ISLOWER(cu) && cu != 170
								 && cu != 186)
					regmbc(cu);
				ਰੋਕੋ;
			    ਹਾਲਤ CLASS_PRINT:
				ਲਈ (cu = 1; cu <= 255; cu++)
				    ਜੇ (vim_isprintc(cu))
					regmbc(cu);
				ਰੋਕੋ;
			    ਹਾਲਤ CLASS_PUNCT:
				ਲਈ (cu = 1; cu < 128; cu++)
				    ਜੇ (ispunct(cu))
					regmbc(cu);
				ਰੋਕੋ;
			    ਹਾਲਤ CLASS_SPACE:
				ਲਈ (cu = 9; cu <= 13; cu++)
				    regc(cu);
				regc(' ');
				ਰੋਕੋ;
			    ਹਾਲਤ CLASS_ਉੱਤਲਾ:
				ਲਈ (cu = 1; cu <= 255; cu++)
				    ਜੇ (MB_ISUPPER(cu))
					regmbc(cu);
				ਰੋਕੋ;
			    ਹਾਲਤ CLASS_XDIGIT:
				ਲਈ (cu = 1; cu <= 255; cu++)
				    ਜੇ (vim_isxdigit(cu))
					regmbc(cu);
				ਰੋਕੋ;
			    ਹਾਲਤ CLASS_TAB:
				regc('\t');
				ਰੋਕੋ;
			    ਹਾਲਤ CLASS_RETURN:
				regc('\r');
				ਰੋਕੋ;
			    ਹਾਲਤ CLASS_BACKSPACE:
				regc('\b');
				ਰੋਕੋ;
			    ਹਾਲਤ CLASS_ESCAPE:
				regc('\033');
				ਰੋਕੋ;
			    ਹਾਲਤ CLASS_IDENT:
				ਲਈ (cu = 1; cu <= 255; cu++)
				    ਜੇ (vim_isIDc(cu))
					regmbc(cu);
				ਰੋਕੋ;
			    ਹਾਲਤ CLASS_KEYWORD:
				ਲਈ (cu = 1; cu <= 255; cu++)
				    ਜੇ (reg_iswordc(cu))
					regmbc(cu);
				ਰੋਕੋ;
			    ਹਾਲਤ CLASS_FNAME:
				ਲਈ (cu = 1; cu <= 255; cu++)
				    ਜੇ (vim_isfilec(cu))
					regmbc(cu);
				ਰੋਕੋ;
			}
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ (has_mbyte)
			{
			    ਪੂਰਨ	len;

			    // produce a multibyte character, including any
			    // following composing characters
			    startc = mb_ptr2char(regparse);
			    len = (*mb_ptr2len)(regparse);
			    ਜੇ (enc_utf8 && utf_char2len(startc) != len)
				startc = -1;	// composing chars
			    ਜਦੋਂ (--len >= 0)
				regc(*regparse++);
			}
			ਨਹੀਂਤਾਂ
			{
			    startc = *regparse++;
			    regc(startc);
			}
		    }
		}
		regc(NUL);
		prevchr_len = 1;	// last char was the ']'
		ਜੇ (*regparse != ']')
		    EMSG_RET_NULL(_(e_too_many_brackets));  // Cannot happen?
		skipchr();	    // let's be friends with the lexer again
		*flagp |= HASWIDTH | SIMPLE;
		ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (reg_strict)
		EMSG2_RET_NULL(_(e_missing_rsb_after_str_lsb),
							reg_magic > MAGIC_OFF);
	}
	// FALLTHROUGH

      ਮੂਲ:
	{
	    ਪੂਰਨ		len;

	    // A multi-byte character is handled as a separate atom if it's
	    // before a multi and when it's a composing char.
	    ਜੇ (use_multibytecode(c))
	    {
do_multibyte:
		ret = regnode(MULTIBYTECODE);
		regmbc(c);
		*flagp |= HASWIDTH | SIMPLE;
		ਰੋਕੋ;
	    }

	    ret = regnode(EXACTLY);

	    // Append characters as long as:
	    // - there is no following multi, we then need the character in
	    //   front of it as a single character operand
	    // - not running into a Magic character
	    // - "one_exactly" is not set
	    // But always emit at least one character.  Might be a Multi,
	    // e.g., a "[" without matching "]".
	    ਲਈ (len = 0; c != NUL && (len == 0
			|| (re_multi_type(peekchr()) == NOT_MULTI
			    && !one_exactly
			    && !is_Magic(c))); ++len)
	    {
		c = no_Magic(c);
		ਜੇ (has_mbyte)
		{
		    regmbc(c);
		    ਜੇ (enc_utf8)
		    {
			ਪੂਰਨ	l;

			// Need to get composing character too.
			ਲਈ (;;)
			{
			    l = utf_ptr2len(regparse);
			    ਜੇ (!UTF_COMPOSINGLIKE(regparse, regparse + l))
				ਰੋਕੋ;
			    regmbc(utf_ptr2char(regparse));
			    skipchr();
			}
		    }
		}
		ਨਹੀਂਤਾਂ
		    regc(c);
		c = getchr();
	    }
	    ungetchr();

	    regc(NUL);
	    *flagp |= HASWIDTH;
	    ਜੇ (len == 1)
		*flagp |= SIMPLE;
	}
	ਰੋਕੋ;
    }

    ਵਾਪਸ ret;
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
    ਸਥਿਰ char_u *
regpiece(ਪੂਰਨ *flagp)
{
    char_u	    *ret;
    ਪੂਰਨ		    op;
    char_u	    *ਅਗਲਾ;
    ਪੂਰਨ		    flags;
    ਲੰਮਾ	    minval;
    ਲੰਮਾ	    maxval;

    ret = regatom(&flags);
    ਜੇ (ret == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    op = peekchr();
    ਜੇ (re_multi_type(op) == NOT_MULTI)
    {
	*flagp = flags;
	ਵਾਪਸ ret;
    }
    // default flags
    *flagp = (WORST | SPSTART | (flags & (HASNL | HASLOOKBH)));

    skipchr();
    ਚੋਣ (op)
    {
	ਹਾਲਤ Magic('*'):
	    ਜੇ (flags & SIMPLE)
		reginsert(ਤਾਰਾ, ret);
	    ਨਹੀਂਤਾਂ
	    {
		// Emit x* as (x&|), where & means "self".
		reginsert(BRANCH, ret); // Either x
		regoptail(ret, regnode(ਪਿੱਠ_2));	// and loop
		regoptail(ret, ret);	//ਪਿੱਠ
		regtail(ret, regnode(BRANCH));	// or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ Magic('+'):
	    ਜੇ (flags & SIMPLE)
		reginsert(PLUS, ret);
	    ਨਹੀਂਤਾਂ
	    {
		// Emit x+ as x(&|), where & means "self".
		ਅਗਲਾ = regnode(BRANCH); // Either
		regtail(ret, ਅਗਲਾ);
		regtail(regnode(ਪਿੱਠ_2), ret);	// loop back
		regtail(ਅਗਲਾ, regnode(BRANCH)); // or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    *flagp = (WORST | HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    ਰੋਕੋ;

	ਹਾਲਤ Magic('@'):
	    {
		ਪੂਰਨ	lop = ਸਿਰਾ_3;
		ਲੰਮਾ	nr;

		nr = (ਲੰਮਾ)getdecchrs();
		ਚੋਣ (no_Magic(getchr()))
		{
		    ਹਾਲਤ '=': lop = ਮੇਲ_2; ਰੋਕੋ;		  // \@=
		    ਹਾਲਤ '!': lop = NOMATCH; ਰੋਕੋ;		  // \@!
		    ਹਾਲਤ '>': lop = SUBPAT; ਰੋਕੋ;		  // \@>
		    ਹਾਲਤ '<': ਚੋਣ (no_Magic(getchr()))
			      {
				  ਹਾਲਤ '=': lop = BEHIND; ਰੋਕੋ;   // \@<=
				  ਹਾਲਤ '!': lop = NOBEHIND; ਰੋਕੋ; // \@<!
			      }
		}
		ਜੇ (lop == ਸਿਰਾ_3)
		    EMSG2_RET_NULL(_(e_invalid_character_after_str_at),
						      reg_magic == MAGIC_ALL);
		// Look behind must match with behind_pos.
		ਜੇ (lop == BEHIND || lop == NOBEHIND)
		{
		    regtail(ret, regnode(BHPOS));
		    *flagp |= HASLOOKBH;
		}
		regtail(ret, regnode(ਸਿਰਾ_3)); // operand ends
		ਜੇ (lop == BEHIND || lop == NOBEHIND)
		{
		    ਜੇ (nr < 0)
			nr = 0; // no limit is same as zero limit
		    reginsert_nr(lop, nr, ret);
		}
		ਨਹੀਂਤਾਂ
		    reginsert(lop, ret);
		ਰੋਕੋ;
	    }

	ਹਾਲਤ Magic('?'):
	ਹਾਲਤ Magic('='):
	    // Emit x= as (x|)
	    reginsert(BRANCH, ret);		// Either x
	    regtail(ret, regnode(BRANCH));	// or
	    ਅਗਲਾ = regnode(NOTHING);		// null.
	    regtail(ret, ਅਗਲਾ);
	    regoptail(ret, ਅਗਲਾ);
	    ਰੋਕੋ;

	ਹਾਲਤ Magic('{'):
	    ਜੇ (!read_limits(&minval, &maxval))
		ਵਾਪਸ ਨਿਰਾ;
	    ਜੇ (flags & SIMPLE)
	    {
		reginsert(BRACE_SIMPLE, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (ਅੰਕ_complex_braces >= 10)
		    EMSG2_RET_NULL(_(e_too_many_complex_str_curly),
						      reg_magic == MAGIC_ALL);
		reginsert(BRACE_COMPLEX + ਅੰਕ_complex_braces, ret);
		regoptail(ret, regnode(ਪਿੱਠ_2));
		regoptail(ret, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
		++ਅੰਕ_complex_braces;
	    }
	    ਜੇ (minval > 0 && maxval > 0)
		*flagp = (HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    ਰੋਕੋ;
    }
    ਜੇ (re_multi_type(peekchr()) != NOT_MULTI)
    {
	// Can't have a multi follow a multi.
	ਜੇ (peekchr() == Magic('*'))
	    EMSG2_RET_NULL(_(e_nested_str), reg_magic >= MAGIC_ON);
	EMSG3_RET_NULL(_(e_nested_str_chr), reg_magic == MAGIC_ALL,
							  no_Magic(peekchr()));
    }

    ਵਾਪਸ ret;
}

/*
 * Parse one alternative of an | or & operator.
 * Implements the concatenation operator.
 */
    ਸਥਿਰ char_u *
regconcat(ਪੂਰਨ *flagp)
{
    char_u	*ਪਹਿਲਾ = ਨਿਰਾ;
    char_u	*chain = ਨਿਰਾ;
    char_u	*latest;
    ਪੂਰਨ		flags;
    ਪੂਰਨ		cont = TRUE;

    *flagp = WORST;		// Tentatively.

    ਜਦੋਂ (cont)
    {
	ਚੋਣ (peekchr())
	{
	    ਹਾਲਤ NUL:
	    ਹਾਲਤ Magic('|'):
	    ਹਾਲਤ Magic('&'):
	    ਹਾਲਤ Magic(')'):
			    cont = FALSE;
			    ਰੋਕੋ;
	    ਹਾਲਤ Magic('Z'):
			    regflags |= RF_ICOMBINE;
			    skipchr_keepstart();
			    ਰੋਕੋ;
	    ਹਾਲਤ Magic('c'):
			    regflags |= RF_ICASE;
			    skipchr_keepstart();
			    ਰੋਕੋ;
	    ਹਾਲਤ Magic('C'):
			    regflags |= RF_NOICASE;
			    skipchr_keepstart();
			    ਰੋਕੋ;
	    ਹਾਲਤ Magic('v'):
			    reg_magic = MAGIC_ALL;
			    skipchr_keepstart();
			    curchr = -1;
			    ਰੋਕੋ;
	    ਹਾਲਤ Magic('m'):
			    reg_magic = MAGIC_ON;
			    skipchr_keepstart();
			    curchr = -1;
			    ਰੋਕੋ;
	    ਹਾਲਤ Magic('M'):
			    reg_magic = MAGIC_OFF;
			    skipchr_keepstart();
			    curchr = -1;
			    ਰੋਕੋ;
	    ਹਾਲਤ Magic('V'):
			    reg_magic = MAGIC_NONE;
			    skipchr_keepstart();
			    curchr = -1;
			    ਰੋਕੋ;
	    ਮੂਲ:
			    latest = regpiece(&flags);
			    ਜੇ (latest == ਨਿਰਾ || reg_toolong)
				ਵਾਪਸ ਨਿਰਾ;
			    *flagp |= flags & (HASWIDTH | HASNL | HASLOOKBH);
			    ਜੇ (chain == ਨਿਰਾ)	// First piece.
				*flagp |= flags & SPSTART;
			    ਨਹੀਂਤਾਂ
				regtail(chain, latest);
			    chain = latest;
			    ਜੇ (ਪਹਿਲਾ == ਨਿਰਾ)
				ਪਹਿਲਾ = latest;
			    ਰੋਕੋ;
	}
    }
    ਜੇ (ਪਹਿਲਾ == ਨਿਰਾ)		// Loop ran zero times.
	ਪਹਿਲਾ = regnode(NOTHING);
    ਵਾਪਸ ਪਹਿਲਾ;
}

/*
 * Parse one alternative of an | operator.
 * Implements the & operator.
 */
    ਸਥਿਰ char_u *
regbranch(ਪੂਰਨ *flagp)
{
    char_u	*ret;
    char_u	*chain = ਨਿਰਾ;
    char_u	*latest;
    ਪੂਰਨ		flags;

    *flagp = WORST | HASNL;		// Tentatively.

    ret = regnode(BRANCH);
    ਲਈ (;;)
    {
	latest = regconcat(&flags);
	ਜੇ (latest == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	// If one of the branches has width, the whole thing has.  If one of
	// the branches anchors at start-of-line, the whole thing does.
	// If one of the branches uses look-behind, the whole thing does.
	*flagp |= flags & (HASWIDTH | SPSTART | HASLOOKBH);
	// If one of the branches doesn't match a line-break, the whole thing
	// doesn't.
	*flagp &= ~HASNL | (flags & HASNL);
	ਜੇ (chain != ਨਿਰਾ)
	    regtail(chain, latest);
	ਜੇ (peekchr() != Magic('&'))
	    ਰੋਕੋ;
	skipchr();
	regtail(latest, regnode(ਸਿਰਾ_3)); // operand ends
	ਜੇ (reg_toolong)
	    ਰੋਕੋ;
	reginsert(ਮੇਲ_2, latest);
	chain = latest;
    }

    ਵਾਪਸ ret;
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
    ਸਥਿਰ char_u *
reg(
    ਪੂਰਨ		paren,	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
    ਪੂਰਨ		*flagp)
{
    char_u	*ret;
    char_u	*br;
    char_u	*ender;
    ਪੂਰਨ		parno = 0;
    ਪੂਰਨ		flags;

    *flagp = HASWIDTH;		// Tentatively.

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ਜੇ (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	ਜੇ (regnzpar >= NSUBEXP)
	    EMSG_RET_NULL(_(e_too_many_z));
	parno = regnzpar;
	regnzpar++;
	ret = regnode(ZOPEN + parno);
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	ਜੇ (paren == REG_PAREN)
    {
	// Make a MOPEN node.
	ਜੇ (regnpar >= NSUBEXP)
	    EMSG2_RET_NULL(_(e_too_many_str_open), reg_magic == MAGIC_ALL);
	parno = regnpar;
	++regnpar;
	ret = regnode(MOPEN + parno);
    }
    ਨਹੀਂਤਾਂ ਜੇ (paren == REG_NPAREN)
    {
	// Make a NOPEN node.
	ret = regnode(NOPEN);
    }
    ਨਹੀਂਤਾਂ
	ret = ਨਿਰਾ;

    ਜੇ (bt_reg_parse_ਡੂੰਘਾਈ >= REG_MAX_PAREN_ਡੂੰਘਾਈ)
	EMSG_RET_NULL(_(e_command_too_complex));
    ++bt_reg_parse_ਡੂੰਘਾਈ;

    // Pick up the branches, linking them together.
    br = regbranch(&flags);
    ਜੇ (br == ਨਿਰਾ)
    {
	ret = ਨਿਰਾ;
	ਜਾਓ theend;
    }
    ਜੇ (ret != ਨਿਰਾ)
	regtail(ret, br);	// [MZ]OPEN -> first.
    ਨਹੀਂਤਾਂ
	ret = br;
    // If one of the branches can be zero-width, the whole thing can.
    // If one of the branches has * at start or matches a line-break, the
    // whole thing can.
    ਜੇ (!(flags & HASWIDTH))
	*flagp &= ~HASWIDTH;
    *flagp |= flags & (SPSTART | HASNL | HASLOOKBH);
    ਜਦੋਂ (peekchr() == Magic('|'))
    {
	skipchr();
	br = regbranch(&flags);
	ਜੇ (br == ਨਿਰਾ || reg_toolong)
	{
	    ret = ਨਿਰਾ;
	    ਜਾਓ theend;
	}
	regtail(ret, br);	// BRANCH -> BRANCH.
	ਜੇ (!(flags & HASWIDTH))
	    *flagp &= ~HASWIDTH;
	*flagp |= flags & (SPSTART | HASNL | HASLOOKBH);
    }

    // Make a closing node, and hook it on the end.
    ender = regnode(
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	    paren == REG_ZPAREN ? ZCLOSE + parno :
#ਜੇ_ਅੰਤ
	    paren == REG_PAREN ? MCLOSE + parno :
	    paren == REG_NPAREN ? NCLOSE : ਸਿਰਾ_3);
    regtail(ret, ender);

    // Hook the tails of the branches to the closing node.
    ਲਈ (br = ret; br != ਨਿਰਾ; br = regnext(br))
	regoptail(br, ender);

    // Check for proper termination.
    ਜੇ (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	ਜੇ (paren == REG_ZPAREN)
	{
	    emsg(_(e_unmatched_z));
	    rc_did_emsg = TRUE;
	    ret = ਨਿਰਾ;
	    ਜਾਓ theend;
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	ਜੇ (paren == REG_NPAREN)
	{
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = ਨਿਰਾ;
	    ਜਾਓ theend;
	}
	ਨਹੀਂਤਾਂ
	{
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = ਨਿਰਾ;
	    ਜਾਓ theend;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (paren == REG_NOPAREN && peekchr() != NUL)
    {
	ਜੇ (curchr == Magic(')'))
	{
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = ਨਿਰਾ;
	    ਜਾਓ theend;
	}
	ਨਹੀਂਤਾਂ
	{
	    emsg(_(e_trailing_characters));	// "Can't happen".
	    rc_did_emsg = TRUE;
	    ret = ਨਿਰਾ;
	    ਜਾਓ theend;
	}
    }
    // Here we set the flag allowing back references to this set of
    // parentheses.
    ਜੇ (paren == REG_PAREN)
	had_endbrace[parno] = TRUE;	// have seen the close paren

theend:
    --bt_reg_parse_ਡੂੰਘਾਈ;
    ਵਾਪਸ ret;
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
    ਸਥਿਰ regprog_T *
bt_regcomp(char_u *ਵਿਅੰਜਕ, ਪੂਰਨ re_flags)
{
    bt_regprog_T    *r;
    char_u	*ਜਾਂਚ;
    char_u	*longest;
    ਪੂਰਨ		len;
    ਪੂਰਨ		flags;

    ਜੇ (ਵਿਅੰਜਕ == ਨਿਰਾ)
	IEMSG_RET_NULL(e_null_ਦਲੀਲ);

    init_class_tab();

    // First pass: determine size, legality.
    regcomp_ਸ਼ੁਰੂ(ਵਿਅੰਜਕ, re_flags);
    regcode = JUST_CALC_ਮਾਪ;
    regc(REGMAGIC);
    ਜੇ (reg(REG_NOPAREN, &flags) == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // Allocate space.
    r = alloc(offsetof(bt_regprog_T, ਪ੍ਰੋਗਰਾਮ) + regsize);
    ਜੇ (r == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    r->re_ਵਿੱਚ_use = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
    r->regsz = regsize;
#ਜੇ_ਅੰਤ

    // Second pass: emit code.
    regcomp_ਸ਼ੁਰੂ(ਵਿਅੰਜਕ, re_flags);
    regcode = r->ਪ੍ਰੋਗਰਾਮ;
    regc(REGMAGIC);
    ਜੇ (reg(REG_NOPAREN, &flags) == ਨਿਰਾ || reg_toolong)
    {
	vim_free(r);
	ਜੇ (reg_toolong)
	    EMSG_RET_NULL(_(e_ਨਮੂਨਾ_too_long));
	ਵਾਪਸ ਨਿਰਾ;
    }

    // Dig out information for optimizations.
    r->regstart = NUL;		// Worst-case defaults.
    r->reganch = 0;
    r->regmust = ਨਿਰਾ;
    r->regmlen = 0;
    r->regflags = regflags;
    ਜੇ (flags & HASNL)
	r->regflags |= RF_HASNL;
    ਜੇ (flags & HASLOOKBH)
	r->regflags |= RF_LOOKBH;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    r->reghasz = re_has_z;
#ਜੇ_ਅੰਤ
    ਜਾਂਚ = r->ਪ੍ਰੋਗਰਾਮ + 1;	// First BRANCH.
    ਜੇ (OP(regnext(ਜਾਂਚ)) == ਸਿਰਾ_3)   // Only one top-level choice.
    {
	ਜਾਂਚ = ਸੰਕਾਰਜ(ਜਾਂਚ);

	// Starting-point info.
	ਜੇ (OP(ਜਾਂਚ) == BOL || OP(ਜਾਂਚ) == RE_BOF)
	{
	    r->reganch++;
	    ਜਾਂਚ = regnext(ਜਾਂਚ);
	}

	ਜੇ (OP(ਜਾਂਚ) == EXACTLY)
	{
	    ਜੇ (has_mbyte)
		r->regstart = (*mb_ptr2char)(ਸੰਕਾਰਜ(ਜਾਂਚ));
	    ਨਹੀਂਤਾਂ
		r->regstart = *ਸੰਕਾਰਜ(ਜਾਂਚ);
	}
	ਨਹੀਂਤਾਂ ਜੇ ((OP(ਜਾਂਚ) == BOW
		    || OP(ਜਾਂਚ) == EOW
		    || OP(ਜਾਂਚ) == NOTHING
		    || OP(ਜਾਂਚ) == MOPEN + 0 || OP(ਜਾਂਚ) == NOPEN
		    || OP(ਜਾਂਚ) == MCLOSE + 0 || OP(ਜਾਂਚ) == NCLOSE)
		 && OP(regnext(ਜਾਂਚ)) == EXACTLY)
	{
	    ਜੇ (has_mbyte)
		r->regstart = (*mb_ptr2char)(ਸੰਕਾਰਜ(regnext(ਜਾਂਚ)));
	    ਨਹੀਂਤਾਂ
		r->regstart = *ਸੰਕਾਰਜ(regnext(ਜਾਂਚ));
	}

	// If there's something expensive in the r.e., find the longest
	// literal string that must appear and make it the regmust.  Resolve
	// ties in favor of later strings, since the regstart check works
	// with the beginning of the r.e. and avoiding duplication
	// strengthens checking.  Not a strong reason, but sufficient in the
	// absence of others.

	// When the r.e. starts with BOW, it is faster to look for a regmust
	// first. Used a lot for "#" and "*" commands. (Added by mool).
	ਜੇ ((flags & SPSTART || OP(ਜਾਂਚ) == BOW || OP(ਜਾਂਚ) == EOW)
							  && !(flags & HASNL))
	{
	    size_t  scanlen;

	    longest = ਨਿਰਾ;
	    len = 0;
	    ਲਈ (; ਜਾਂਚ != ਨਿਰਾ; ਜਾਂਚ = regnext(ਜਾਂਚ))
	    {
		ਜੇ (OP(ਜਾਂਚ) == EXACTLY)
		{
		    scanlen = STRLEN(ਸੰਕਾਰਜ(ਜਾਂਚ));
		    ਜੇ (scanlen >= (size_t)len)
		    {
			longest = ਸੰਕਾਰਜ(ਜਾਂਚ);
			len = (ਪੂਰਨ)scanlen;
		    }
		}
	    }
	    r->regmust = longest;
	    r->regmlen = len;
	}
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BT_REGEXP_DUMP
    regdump(ਵਿਅੰਜਕ, r);
#ਜੇ_ਅੰਤ
    r->engine = &bt_regengine;
    ਵਾਪਸ (regprog_T *)r;
}

#ਜੇ defined(FEAT_SYN_HL)
/*
 * Check if during the previous call to vim_regcomp the EOL item "$" has been
 * found.  This is messy, but it works fine.
 */
    ਪੂਰਨ
vim_regcomp_had_eol(ਖਾਲੀ)
{
    ਵਾਪਸ had_eol;
}
#ਜੇ_ਅੰਤ

/*
 * Get a number after a backslash that is inside [].
 * When nothing is recognized return a backslash.
 */
    ਸਥਿਰ ਪੂਰਨ
coll_get_char(ਖਾਲੀ)
{
    vimlong_T	nr = -1;

    ਚੋਣ (*regparse++)
    {
	ਹਾਲਤ 'd': nr = getdecchrs(); ਰੋਕੋ;
	ਹਾਲਤ 'o': nr = getoctchrs(); ਰੋਕੋ;
	ਹਾਲਤ 'x': nr = gethexchrs(2); ਰੋਕੋ;
	ਹਾਲਤ 'u': nr = gethexchrs(4); ਰੋਕੋ;
	ਹਾਲਤ 'U': nr = gethexchrs(8); ਰੋਕੋ;
    }
    ਜੇ (nr < 0)
    {
	// If getting the number fails be backwards compatible: the character
	// is a backslash.
	--regparse;
	nr = '\\';
    }
    ਜੇ (nr > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
	nr = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
    ਵਾਪਸ nr;
}

/*
 * Free a compiled regexp program, returned by bt_regcomp().
 */
    ਸਥਿਰ ਖਾਲੀ
bt_regfree(regprog_T *prog)
{
    vim_free(prog);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ADVANCE_REGINPUT() MB_PTR_ADV(rex.ਨਿਵੇਸ਼)

/*
 * The arguments from BRACE_LIMITS are stored here.  They are actually local
 * to regmatch(), but they are here to reduce the amount of stack space used
 * (it can be called recursively many times).
 */
ਸਥਿਰ ਲੰਮਾ	bl_minval;
ਸਥਿਰ ਲੰਮਾ	bl_maxval;

/*
 * Save the input line and position in a regsave_T.
 */
    ਸਥਿਰ ਖਾਲੀ
reg_save(regsave_T *save, garray_T *gap)
{
    ਜੇ (REG_MULTI)
    {
	save->rs_u.pos.ਖਾਨਾ = (colnr_T)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ);
	save->rs_u.pos.lnum = rex.lnum;
    }
    ਨਹੀਂਤਾਂ
	save->rs_u.ptr = rex.ਨਿਵੇਸ਼;
    save->rs_len = gap->ga_len;
}

/*
 * Restore the input line and position from a regsave_T.
 */
    ਸਥਿਰ ਖਾਲੀ
reg_restore(regsave_T *save, garray_T *gap)
{
    ਜੇ (REG_MULTI)
    {
	ਜੇ (rex.lnum != save->rs_u.pos.lnum)
	{
	    // only call reg_getline() when the line number changed to save
	    // a bit of time
	    rex.lnum = save->rs_u.pos.lnum;
	    rex.ਸਤਰ = reg_getline(rex.lnum);
	}
	rex.ਨਿਵੇਸ਼ = rex.ਸਤਰ + save->rs_u.pos.ਖਾਨਾ;
    }
    ਨਹੀਂਤਾਂ
	rex.ਨਿਵੇਸ਼ = save->rs_u.ptr;
    gap->ga_len = save->rs_len;
}

/*
 * Return TRUE if current position is equal to saved position.
 */
    ਸਥਿਰ ਪੂਰਨ
reg_save_equal(regsave_T *save)
{
    ਜੇ (REG_MULTI)
	ਵਾਪਸ rex.lnum == save->rs_u.pos.lnum
				  && rex.ਨਿਵੇਸ਼ == rex.ਸਤਰ + save->rs_u.pos.ਖਾਨਾ;
    ਵਾਪਸ rex.ਨਿਵੇਸ਼ == save->rs_u.ptr;
}

// Save the sub-expressions before attempting a match.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ save_se(savep, posp, pp) \
    REG_MULTI ? save_se_multi((savep), (posp)) : save_se_one((savep), (pp))

// After a failed match restore the sub-expressions.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ restore_se(savep, posp, pp) \
{ \
    ਜੇ (REG_MULTI) \
	*(posp) = (savep)->se_u.pos; \
    ਨਹੀਂਤਾਂ \
	*(pp) = (savep)->se_u.ptr; \
}

/*
 * Tentatively set the sub-expression start to the current position (after
 * calling regmatch() they will have changed).  Need to save the existing
 * values for when there is no match.
 * Use se_save() to use pointer (save_se_multi()) or position (save_se_one()),
 * depending on REG_MULTI.
 */
    ਸਥਿਰ ਖਾਲੀ
save_se_multi(save_se_T *savep, lpos_T *posp)
{
    savep->se_u.pos = *posp;
    posp->lnum = rex.lnum;
    posp->ਖਾਨਾ = (colnr_T)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ);
}

    ਸਥਿਰ ਖਾਲੀ
save_se_one(save_se_T *savep, char_u **pp)
{
    savep->se_u.ptr = *pp;
    *pp = rex.ਨਿਵੇਸ਼;
}

/*
 * regrepeat - repeatedly match something simple, return how many.
 * Advances rex.input (and rex.lnum) to just after the matched chars.
 */
    ਸਥਿਰ ਪੂਰਨ
regrepeat(
    char_u	*p,
    ਲੰਮਾ	maxcount)   // maximum number of matches allowed
{
    ਲੰਮਾ	ਗਿਣਤੀ_2 = 0;
    char_u	*ਜਾਂਚ;
    char_u	*opnd;
    ਪੂਰਨ		ਨਕਾਬ;
    ਪੂਰਨ		testval = 0;

    ਜਾਂਚ = rex.ਨਿਵੇਸ਼;	    // Make local copy of rex.input for speed.
    opnd = ਸੰਕਾਰਜ(p);
    ਚੋਣ (OP(p))
    {
      ਹਾਲਤ ANY:
      ਹਾਲਤ ANY + ਜੋੜੋ_NL:
	ਜਦੋਂ (ਗਿਣਤੀ_2 < maxcount)
	{
	    // Matching anything means we continue until end-of-line (or
	    // end-of-file for ANY + ADD_NL), only limited by maxcount.
	    ਜਦੋਂ (*ਜਾਂਚ != NUL && ਗਿਣਤੀ_2 < maxcount)
	    {
		++ਗਿਣਤੀ_2;
		MB_PTR_ADV(ਜਾਂਚ);
	    }
	    ਜੇ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
				      || rex.reg_ਸਤਰ_lbr || ਗਿਣਤੀ_2 == maxcount)
		ਰੋਕੋ;
	    ++ਗਿਣਤੀ_2;		// count the line-break
	    reg_nextline();
	    ਜਾਂਚ = rex.ਨਿਵੇਸ਼;
	    ਜੇ (got_int)
		ਰੋਕੋ;
	}
	ਰੋਕੋ;

      ਹਾਲਤ IDENT:
      ਹਾਲਤ IDENT + ਜੋੜੋ_NL:
	testval = TRUE;
	// FALLTHROUGH
      ਹਾਲਤ SIDENT:
      ਹਾਲਤ SIDENT + ਜੋੜੋ_NL:
	ਜਦੋਂ (ਗਿਣਤੀ_2 < maxcount)
	{
	    ਜੇ (vim_isIDc(PTR2CHAR(ਜਾਂਚ)) && (testval || !VIM_ISDIGIT(*ਜਾਂਚ)))
	    {
		MB_PTR_ADV(ਜਾਂਚ);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*ਜਾਂਚ == NUL)
	    {
		ਜੇ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ਸਤਰ_lbr)
		    ਰੋਕੋ;
		reg_nextline();
		ਜਾਂਚ = rex.ਨਿਵੇਸ਼;
		ਜੇ (got_int)
		    ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (rex.reg_ਸਤਰ_lbr && *ਜਾਂਚ == '\n' && WITH_NL(OP(p)))
		++ਜਾਂਚ;
	    ਨਹੀਂਤਾਂ
		ਰੋਕੋ;
	    ++ਗਿਣਤੀ_2;
	}
	ਰੋਕੋ;

      ਹਾਲਤ KWORD:
      ਹਾਲਤ KWORD + ਜੋੜੋ_NL:
	testval = TRUE;
	// FALLTHROUGH
      ਹਾਲਤ SKWORD:
      ਹਾਲਤ SKWORD + ਜੋੜੋ_NL:
	ਜਦੋਂ (ਗਿਣਤੀ_2 < maxcount)
	{
	    ਜੇ (vim_iswordp_buf(ਜਾਂਚ, rex.reg_buf)
					  && (testval || !VIM_ISDIGIT(*ਜਾਂਚ)))
	    {
		MB_PTR_ADV(ਜਾਂਚ);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*ਜਾਂਚ == NUL)
	    {
		ਜੇ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ਸਤਰ_lbr)
		    ਰੋਕੋ;
		reg_nextline();
		ਜਾਂਚ = rex.ਨਿਵੇਸ਼;
		ਜੇ (got_int)
		    ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (rex.reg_ਸਤਰ_lbr && *ਜਾਂਚ == '\n' && WITH_NL(OP(p)))
		++ਜਾਂਚ;
	    ਨਹੀਂਤਾਂ
		ਰੋਕੋ;
	    ++ਗਿਣਤੀ_2;
	}
	ਰੋਕੋ;

      ਹਾਲਤ FNAME:
      ਹਾਲਤ FNAME + ਜੋੜੋ_NL:
	testval = TRUE;
	// FALLTHROUGH
      ਹਾਲਤ SFNAME:
      ਹਾਲਤ SFNAME + ਜੋੜੋ_NL:
	ਜਦੋਂ (ਗਿਣਤੀ_2 < maxcount)
	{
	    ਜੇ (vim_isfilec(PTR2CHAR(ਜਾਂਚ)) && (testval || !VIM_ISDIGIT(*ਜਾਂਚ)))
	    {
		MB_PTR_ADV(ਜਾਂਚ);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*ਜਾਂਚ == NUL)
	    {
		ਜੇ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ਸਤਰ_lbr)
		    ਰੋਕੋ;
		reg_nextline();
		ਜਾਂਚ = rex.ਨਿਵੇਸ਼;
		ਜੇ (got_int)
		    ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (rex.reg_ਸਤਰ_lbr && *ਜਾਂਚ == '\n' && WITH_NL(OP(p)))
		++ਜਾਂਚ;
	    ਨਹੀਂਤਾਂ
		ਰੋਕੋ;
	    ++ਗਿਣਤੀ_2;
	}
	ਰੋਕੋ;

      ਹਾਲਤ PRINT:
      ਹਾਲਤ PRINT + ਜੋੜੋ_NL:
	testval = TRUE;
	// FALLTHROUGH
      ਹਾਲਤ SPRINT:
      ਹਾਲਤ SPRINT + ਜੋੜੋ_NL:
	ਜਦੋਂ (ਗਿਣਤੀ_2 < maxcount)
	{
	    ਜੇ (*ਜਾਂਚ == NUL)
	    {
		ਜੇ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ਸਤਰ_lbr)
		    ਰੋਕੋ;
		reg_nextline();
		ਜਾਂਚ = rex.ਨਿਵੇਸ਼;
		ਜੇ (got_int)
		    ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (vim_isprintc(PTR2CHAR(ਜਾਂਚ)) == 1
					  && (testval || !VIM_ISDIGIT(*ਜਾਂਚ)))
	    {
		MB_PTR_ADV(ਜਾਂਚ);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (rex.reg_ਸਤਰ_lbr && *ਜਾਂਚ == '\n' && WITH_NL(OP(p)))
		++ਜਾਂਚ;
	    ਨਹੀਂਤਾਂ
		ਰੋਕੋ;
	    ++ਗਿਣਤੀ_2;
	}
	ਰੋਕੋ;

      ਹਾਲਤ WHITE:
      ਹਾਲਤ WHITE + ਜੋੜੋ_NL:
	testval = ਨਕਾਬ = RI_WHITE;
do_class:
	ਜਦੋਂ (ਗਿਣਤੀ_2 < maxcount)
	{
	    ਪੂਰਨ		l;

	    ਜੇ (*ਜਾਂਚ == NUL)
	    {
		ਜੇ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ਸਤਰ_lbr)
		    ਰੋਕੋ;
		reg_nextline();
		ਜਾਂਚ = rex.ਨਿਵੇਸ਼;
		ਜੇ (got_int)
		    ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (has_mbyte && (l = (*mb_ptr2len)(ਜਾਂਚ)) > 1)
	    {
		ਜੇ (testval != 0)
		    ਰੋਕੋ;
		ਜਾਂਚ += l;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ ((class_tab[*ਜਾਂਚ] & ਨਕਾਬ) == testval)
		++ਜਾਂਚ;
	    ਨਹੀਂਤਾਂ ਜੇ (rex.reg_ਸਤਰ_lbr && *ਜਾਂਚ == '\n' && WITH_NL(OP(p)))
		++ਜਾਂਚ;
	    ਨਹੀਂਤਾਂ
		ਰੋਕੋ;
	    ++ਗਿਣਤੀ_2;
	}
	ਰੋਕੋ;

      ਹਾਲਤ NWHITE:
      ਹਾਲਤ NWHITE + ਜੋੜੋ_NL:
	ਨਕਾਬ = RI_WHITE;
	ਜਾਓ do_class;
      ਹਾਲਤ DIGIT:
      ਹਾਲਤ DIGIT + ਜੋੜੋ_NL:
	testval = ਨਕਾਬ = RI_DIGIT;
	ਜਾਓ do_class;
      ਹਾਲਤ NDIGIT:
      ਹਾਲਤ NDIGIT + ਜੋੜੋ_NL:
	ਨਕਾਬ = RI_DIGIT;
	ਜਾਓ do_class;
      ਹਾਲਤ HEX:
      ਹਾਲਤ HEX + ਜੋੜੋ_NL:
	testval = ਨਕਾਬ = RI_HEX;
	ਜਾਓ do_class;
      ਹਾਲਤ NHEX:
      ਹਾਲਤ NHEX + ਜੋੜੋ_NL:
	ਨਕਾਬ = RI_HEX;
	ਜਾਓ do_class;
      ਹਾਲਤ OCTAL:
      ਹਾਲਤ OCTAL + ਜੋੜੋ_NL:
	testval = ਨਕਾਬ = RI_OCTAL;
	ਜਾਓ do_class;
      ਹਾਲਤ NOCTAL:
      ਹਾਲਤ NOCTAL + ਜੋੜੋ_NL:
	ਨਕਾਬ = RI_OCTAL;
	ਜਾਓ do_class;
      ਹਾਲਤ ਸ਼ਬਦ_2:
      ਹਾਲਤ ਸ਼ਬਦ_2 + ਜੋੜੋ_NL:
	testval = ਨਕਾਬ = RI_ਸ਼ਬਦ;
	ਜਾਓ do_class;
      ਹਾਲਤ NWORD:
      ਹਾਲਤ NWORD + ਜੋੜੋ_NL:
	ਨਕਾਬ = RI_ਸ਼ਬਦ;
	ਜਾਓ do_class;
      ਹਾਲਤ ਸਿਰ_2:
      ਹਾਲਤ ਸਿਰ_2 + ਜੋੜੋ_NL:
	testval = ਨਕਾਬ = RI_ਸਿਰ;
	ਜਾਓ do_class;
      ਹਾਲਤ NHEAD:
      ਹਾਲਤ NHEAD + ਜੋੜੋ_NL:
	ਨਕਾਬ = RI_ਸਿਰ;
	ਜਾਓ do_class;
      ਹਾਲਤ ALPHA:
      ਹਾਲਤ ALPHA + ਜੋੜੋ_NL:
	testval = ਨਕਾਬ = RI_ALPHA;
	ਜਾਓ do_class;
      ਹਾਲਤ NALPHA:
      ਹਾਲਤ NALPHA + ਜੋੜੋ_NL:
	ਨਕਾਬ = RI_ALPHA;
	ਜਾਓ do_class;
      ਹਾਲਤ ਹੇਠਲਾ_2:
      ਹਾਲਤ ਹੇਠਲਾ_2 + ਜੋੜੋ_NL:
	testval = ਨਕਾਬ = RI_ਹੇਠਲਾ;
	ਜਾਓ do_class;
      ਹਾਲਤ NLOWER:
      ਹਾਲਤ NLOWER + ਜੋੜੋ_NL:
	ਨਕਾਬ = RI_ਹੇਠਲਾ;
	ਜਾਓ do_class;
      ਹਾਲਤ ਉੱਤਲਾ:
      ਹਾਲਤ ਉੱਤਲਾ + ਜੋੜੋ_NL:
	testval = ਨਕਾਬ = RI_ਉੱਤਲਾ;
	ਜਾਓ do_class;
      ਹਾਲਤ NUPPER:
      ਹਾਲਤ NUPPER + ਜੋੜੋ_NL:
	ਨਕਾਬ = RI_ਉੱਤਲਾ;
	ਜਾਓ do_class;

      ਹਾਲਤ EXACTLY:
	{
	    ਪੂਰਨ	    cu, cl;

	    // This doesn't do a multi-byte character, because a MULTIBYTECODE
	    // would have been used for it.  It does handle single-byte
	    // characters, such as latin1.
	    ਜੇ (rex.reg_ic)
	    {
		cu = MB_TOUPPER(*opnd);
		cl = MB_TOLOWER(*opnd);
		ਜਦੋਂ (ਗਿਣਤੀ_2 < maxcount && (*ਜਾਂਚ == cu || *ਜਾਂਚ == cl))
		{
		    ਗਿਣਤੀ_2++;
		    ਜਾਂਚ++;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		cu = *opnd;
		ਜਦੋਂ (ਗਿਣਤੀ_2 < maxcount && *ਜਾਂਚ == cu)
		{
		    ਗਿਣਤੀ_2++;
		    ਜਾਂਚ++;
		}
	    }
	    ਰੋਕੋ;
	}

      ਹਾਲਤ MULTIBYTECODE:
	{
	    ਪੂਰਨ		i, len, cf = 0;

	    // Safety check (just in case 'encoding' was changed since
	    // compiling the program).
	    ਜੇ ((len = (*mb_ptr2len)(opnd)) > 1)
	    {
		ਜੇ (rex.reg_ic && enc_utf8)
		    cf = utf_fold(utf_ptr2char(opnd));
		ਜਦੋਂ (ਗਿਣਤੀ_2 < maxcount && (*mb_ptr2len)(ਜਾਂਚ) >= len)
		{
		    ਲਈ (i = 0; i < len; ++i)
			ਜੇ (opnd[i] != ਜਾਂਚ[i])
			    ਰੋਕੋ;
		    ਜੇ (i < len && (!rex.reg_ic || !enc_utf8
					|| utf_fold(utf_ptr2char(ਜਾਂਚ)) != cf))
			ਰੋਕੋ;
		    ਜਾਂਚ += len;
		    ++ਗਿਣਤੀ_2;
		}
	    }
	}
	ਰੋਕੋ;

      ਹਾਲਤ ANYOF:
      ਹਾਲਤ ANYOF + ਜੋੜੋ_NL:
	testval = TRUE;
	// FALLTHROUGH

      ਹਾਲਤ ANYBUT:
      ਹਾਲਤ ANYBUT + ਜੋੜੋ_NL:
	ਜਦੋਂ (ਗਿਣਤੀ_2 < maxcount)
	{
	    ਪੂਰਨ len;

	    ਜੇ (*ਜਾਂਚ == NUL)
	    {
		ਜੇ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ਸਤਰ_lbr)
		    ਰੋਕੋ;
		reg_nextline();
		ਜਾਂਚ = rex.ਨਿਵੇਸ਼;
		ਜੇ (got_int)
		    ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (rex.reg_ਸਤਰ_lbr && *ਜਾਂਚ == '\n' && WITH_NL(OP(p)))
		++ਜਾਂਚ;
	    ਨਹੀਂਤਾਂ ਜੇ (has_mbyte && (len = (*mb_ptr2len)(ਜਾਂਚ)) > 1)
	    {
		ਜੇ ((cstrchr(opnd, (*mb_ptr2char)(ਜਾਂਚ)) == ਨਿਰਾ) == testval)
		    ਰੋਕੋ;
		ਜਾਂਚ += len;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ ((cstrchr(opnd, *ਜਾਂਚ) == ਨਿਰਾ) == testval)
		    ਰੋਕੋ;
		++ਜਾਂਚ;
	    }
	    ++ਗਿਣਤੀ_2;
	}
	ਰੋਕੋ;

      ਹਾਲਤ NEWL:
	ਜਦੋਂ (ਗਿਣਤੀ_2 < maxcount
		&& ((*ਜਾਂਚ == NUL && rex.lnum <= rex.reg_maxline
				       && !rex.reg_ਸਤਰ_lbr && REG_MULTI)
		    || (*ਜਾਂਚ == '\n' && rex.reg_ਸਤਰ_lbr)))
	{
	    ਗਿਣਤੀ_2++;
	    ਜੇ (rex.reg_ਸਤਰ_lbr)
		ADVANCE_REGINPUT();
	    ਨਹੀਂਤਾਂ
		reg_nextline();
	    ਜਾਂਚ = rex.ਨਿਵੇਸ਼;
	    ਜੇ (got_int)
		ਰੋਕੋ;
	}
	ਰੋਕੋ;

      ਮੂਲ:			// Oh dear.  Called inappropriately.
	iemsg(e_corrupted_regexp_ਪ੍ਰੋਗਰਾਮ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
	ਲਿਖੋ("Called regrepeat with op code %d\n", OP(p));
#ਜੇ_ਅੰਤ
	ਰੋਕੋ;
    }

    rex.ਨਿਵੇਸ਼ = ਜਾਂਚ;

    ਵਾਪਸ (ਪੂਰਨ)ਗਿਣਤੀ_2;
}

/*
 * Push an item onto the regstack.
 * Returns pointer to new item.  Returns NULL when out of memory.
 */
    ਸਥਿਰ regitem_T *
regstack_ਧੱਕੋ(regstate_T state, char_u *ਜਾਂਚ)
{
    regitem_T	*rp;

    ਜੇ ((ਲੰਮਾ)((ਬਿਨਾਨਿਸ਼ਾਨੀ)regstack.ga_len >> 10) >= p_mmp)
    {
	emsg(_(e_ਨਮੂਨਾ_ਵਰਤਦਾ_ਹੋਰ_ਸਮ੍ਰਿਤੀ_than_maxmempattern));
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (GA_GROW_FAILS(&regstack, (ਪੂਰਨ)ਆਕਾਰ(regitem_T)))
	ਵਾਪਸ ਨਿਰਾ;

    rp = (regitem_T *)((ਅੱਖਰ *)regstack.ga_ਅੰਕੜੇ + regstack.ga_len);
    rp->rs_state = state;
    rp->rs_ਜਾਂਚ = ਜਾਂਚ;

    regstack.ga_len += ਆਕਾਰ(regitem_T);
    ਵਾਪਸ rp;
}

/*
 * Pop an item from the regstack.
 */
    ਸਥਿਰ ਖਾਲੀ
regstack_ਕੱਢੋ(char_u **ਜਾਂਚ)
{
    regitem_T	*rp;

    rp = (regitem_T *)((ਅੱਖਰ *)regstack.ga_ਅੰਕੜੇ + regstack.ga_len) - 1;
    *ਜਾਂਚ = rp->rs_ਜਾਂਚ;

    regstack.ga_len -= ਆਕਾਰ(regitem_T);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
/*
 * Check if the timer expired, return TRUE if so.
 */
    ਸਥਿਰ ਪੂਰਨ
bt_did_ਸਮਾਂ_ਬਾਹਰਲਾ(ਪੂਰਨ *timed_ਬਾਹਰਲਾ)
{
    ਜੇ (*timeout_flag)
    {
	ਜੇ (timed_ਬਾਹਰਲਾ != ਨਿਰਾ)
	{
# ifdef FEAT_EVAL
	    ਜੇ (!*timed_ਬਾਹਰਲਾ)
		ch_log(ਨਿਰਾ, "BT regexp timed out");
# endif
	    *timed_ਬਾਹਰਲਾ = TRUE;
	}
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ

/*
 * Save the current subexpr to "bp", so that they can be restored
 * later by restore_subexpr().
 */
    ਸਥਿਰ ਖਾਲੀ
save_subexpr(regbehind_T *bp)
{
    ਪੂਰਨ i;

    // When "rex.need_clear_subexpr" is set we don't need to save the values,
    // only remember that this flag needs to be set again when restoring.
    bp->save_need_clear_subexpr = rex.need_clear_subexpr;
    ਜੇ (rex.need_clear_subexpr)
	ਵਾਪਸ;

    ਲਈ (i = 0; i < NSUBEXP; ++i)
    {
	ਜੇ (REG_MULTI)
	{
	    bp->save_ਸ਼ੁਰੂ[i].se_u.pos = rex.reg_startpos[i];
	    bp->save_ਸਿਰਾ[i].se_u.pos = rex.reg_endpos[i];
	}
	ਨਹੀਂਤਾਂ
	{
	    bp->save_ਸ਼ੁਰੂ[i].se_u.ptr = rex.reg_startp[i];
	    bp->save_ਸਿਰਾ[i].se_u.ptr = rex.reg_endp[i];
	}
    }
}

/*
 * Restore the subexpr from "bp".
 */
    ਸਥਿਰ ਖਾਲੀ
restore_subexpr(regbehind_T *bp)
{
    ਪੂਰਨ i;

    // Only need to restore saved values when they are not to be cleared.
    rex.need_clear_subexpr = bp->save_need_clear_subexpr;
    ਜੇ (rex.need_clear_subexpr)
	ਵਾਪਸ;

    ਲਈ (i = 0; i < NSUBEXP; ++i)
    {
	ਜੇ (REG_MULTI)
	{
	    rex.reg_startpos[i] = bp->save_ਸ਼ੁਰੂ[i].se_u.pos;
	    rex.reg_endpos[i] = bp->save_ਸਿਰਾ[i].se_u.pos;
	}
	ਨਹੀਂਤਾਂ
	{
	    rex.reg_startp[i] = bp->save_ਸ਼ੁਰੂ[i].se_u.ptr;
	    rex.reg_endp[i] = bp->save_ਸਿਰਾ[i].se_u.ptr;
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
    ਸਥਿਰ ਪੂਰਨ
regmatch(
    char_u	*ਜਾਂਚ,		    // Current node.
    ਪੂਰਨ		*timed_ਬਾਹਰਲਾ ਅਣਵਰਤਿਆ)  // flag set on timeout or NULL
{
  char_u	*ਅਗਲਾ;		// Next node.
  ਪੂਰਨ		op;
  ਪੂਰਨ		c;
  regitem_T	*rp;
  ਪੂਰਨ		no;
  ਪੂਰਨ		ਸਥਿਤੀ;		// one of the RA_ values:

  // Make "regstack" and "backpos" empty.  They are allocated and freed in
  // bt_regexec_both() to reduce malloc()/free() calls.
  regstack.ga_len = 0;
  backpos.ga_len = 0;

  // Repeat until "regstack" is empty.
  ਲਈ (;;)
  {
    // Some patterns may take a long time to match, e.g., "\([a-z]\+\)\+Q".
    // Allow interrupting them with CTRL-C.
    fast_breakcheck();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
    ਜੇ (ਜਾਂਚ != ਨਿਰਾ && regnarrate)
    {
	mch_errmsg((ਅੱਖਰ *)regprop(ਜਾਂਚ));
	mch_errmsg("(\n");
    }
#ਜੇ_ਅੰਤ

    // Repeat for items that can be matched sequentially, without using the
    // regstack.
    ਲਈ (;;)
    {
	ਜੇ (got_int || ਜਾਂਚ == ਨਿਰਾ)
	{
	    ਸਥਿਤੀ = RA_FAIL;
	    ਰੋਕੋ;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
	ਜੇ (bt_did_ਸਮਾਂ_ਬਾਹਰਲਾ(timed_ਬਾਹਰਲਾ))
	{
	    ਸਥਿਤੀ = RA_FAIL;
	    ਰੋਕੋ;
	}
#ਜੇ_ਅੰਤ
	ਸਥਿਤੀ = RA_CONT;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
	ਜੇ (regnarrate)
	{
	    mch_errmsg((ਅੱਖਰ *)regprop(ਜਾਂਚ));
	    mch_errmsg("...\n");
# ifdef FEAT_SYN_HL
	    ਜੇ (re_extmatch_ਵਿੱਚ != ਨਿਰਾ)
	    {
		ਪੂਰਨ i;

		mch_errmsg(_("External submatches:\n"));
		ਲਈ (i = 0; i < NSUBEXP; i++)
		{
		    mch_errmsg("    \"");
		    ਜੇ (re_extmatch_ਵਿੱਚ->matches[i] != ਨਿਰਾ)
			mch_errmsg((ਅੱਖਰ *)re_extmatch_ਵਿੱਚ->matches[i]);
		    mch_errmsg("\"\n");
		}
	    }
# endif
	}
#ਜੇ_ਅੰਤ
	ਅਗਲਾ = regnext(ਜਾਂਚ);

	op = OP(ਜਾਂਚ);
	// Check for character class with NL added.
	ਜੇ (!rex.reg_ਸਤਰ_lbr && WITH_NL(op) && REG_MULTI
			     && *rex.ਨਿਵੇਸ਼ == NUL && rex.lnum <= rex.reg_maxline)
	{
	    reg_nextline();
	}
	ਨਹੀਂਤਾਂ ਜੇ (rex.reg_ਸਤਰ_lbr && WITH_NL(op) && *rex.ਨਿਵੇਸ਼ == '\n')
	{
	    ADVANCE_REGINPUT();
	}
	ਨਹੀਂਤਾਂ
	{
	  ਜੇ (WITH_NL(op))
	      op -= ਜੋੜੋ_NL;
	  ਜੇ (has_mbyte)
	      c = (*mb_ptr2char)(rex.ਨਿਵੇਸ਼);
	  ਨਹੀਂਤਾਂ
	      c = *rex.ਨਿਵੇਸ਼;
	  ਚੋਣ (op)
	  {
	  ਹਾਲਤ BOL:
	    ਜੇ (rex.ਨਿਵੇਸ਼ != rex.ਸਤਰ)
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਰੋਕੋ;

	  ਹਾਲਤ EOL:
	    ਜੇ (c != NUL)
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਰੋਕੋ;

	  ਹਾਲਤ RE_BOF:
	    // We're not at the beginning of the file when below the first
	    // line where we started, not at the start of the line or we
	    // didn't start at the first line of the buffer.
	    ਜੇ (rex.lnum != 0 || rex.ਨਿਵੇਸ਼ != rex.ਸਤਰ
				       || (REG_MULTI && rex.reg_firstlnum > 1))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਰੋਕੋ;

	  ਹਾਲਤ RE_EOF:
	    ਜੇ (rex.lnum != rex.reg_maxline || c != NUL)
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਰੋਕੋ;

	  ਹਾਲਤ CURSOR:
	    // Check if the buffer is in a window and compare the
	    // rex.reg_win->w_cursor position to the match position.
	    ਜੇ (rex.reg_win == ਨਿਰਾ
		    || (rex.lnum + rex.reg_firstlnum
						 != rex.reg_win->w_cursor.lnum)
		    || ((colnr_T)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ)
						 != rex.reg_win->w_cursor.ਖਾਨਾ))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਰੋਕੋ;

	  ਹਾਲਤ RE_MARK:
	    // Compare the mark position to the match position.
	    {
		ਪੂਰਨ	mark = ਸੰਕਾਰਜ(ਜਾਂਚ)[0];
		ਪੂਰਨ	cmp = ਸੰਕਾਰਜ(ਜਾਂਚ)[1];
		pos_T	*pos;
		size_t	ਖਾਨਾ = REG_MULTI ? rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ : 0;

		pos = getmark_buf(rex.reg_buf, mark, FALSE);

		// Line may have been freed, get it again.
		ਜੇ (REG_MULTI)
		{
		    rex.ਸਤਰ = reg_getline(rex.lnum);
		    rex.ਨਿਵੇਸ਼ = rex.ਸਤਰ + ਖਾਨਾ;
		}

		ਜੇ (pos == ਨਿਰਾ		     // mark doesn't exist
			|| pos->lnum <= 0)   // mark isn't set in reg_buf
		{
		    ਸਥਿਤੀ = RA_NOMATCH;
		}
		ਨਹੀਂਤਾਂ
		{
		    colnr_T pos_ਖਾਨਾ = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->ਖਾਨਾ == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->ਖਾਨਾ;

		    ਜੇ ((pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_ਖਾਨਾ == (colnr_T)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ)
				    ? (cmp == '<' || cmp == '>')
				    : (pos_ਖਾਨਾ < (colnr_T)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ)
					? cmp != '>'
					: cmp != '<'))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? cmp != '>'
				    : cmp != '<')))
		    ਸਥਿਤੀ = RA_NOMATCH;
		}
	    }
	    ਰੋਕੋ;

	  ਹਾਲਤ RE_VISUAL:
	    ਜੇ (!reg_ਮੇਲ_visual())
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਰੋਕੋ;

	  ਹਾਲਤ RE_LNUM:
	    ਜੇ (!REG_MULTI || !re_ਅੰਕ_cmp((long_u)(rex.lnum + rex.reg_firstlnum),
									ਜਾਂਚ))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਰੋਕੋ;

	  ਹਾਲਤ RE_ਖਾਨਾ:
	    ਜੇ (!re_ਅੰਕ_cmp((long_u)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ) + 1, ਜਾਂਚ))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਰੋਕੋ;

	  ਹਾਲਤ RE_VCOL:
	    {
		win_T	    *wp = rex.reg_win == ਨਿਰਾ ? curwin : rex.reg_win;
		linenr_T    lnum = REG_MULTI ? rex.reg_firstlnum + rex.lnum : 1;
		long_u	    vcol;

		ਜੇ (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ))
		    lnum = 1;
		vcol = (long_u)win_linetabsize(wp, lnum, rex.ਸਤਰ,
					      (colnr_T)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ));
		ਜੇ (!re_ਅੰਕ_cmp(vcol + 1, ਜਾਂਚ))
		    ਸਥਿਤੀ = RA_NOMATCH;
	    }
	    ਰੋਕੋ;

	  ਹਾਲਤ BOW:	// \<word; rex.input points to w
	    ਜੇ (c == NUL)	// Can't match at end of line
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ ਜੇ (has_mbyte)
	    {
		ਪੂਰਨ this_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.ਨਿਵੇਸ਼, rex.reg_buf);
		ਜੇ (this_class <= 1)
		    ਸਥਿਤੀ = RA_NOMATCH;  // not on a word at all
		ਨਹੀਂਤਾਂ ਜੇ (reg_prev_class() == this_class)
		    ਸਥਿਤੀ = RA_NOMATCH;  // previous char is in same word
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (!vim_iswordc_buf(c, rex.reg_buf) || (rex.ਨਿਵੇਸ਼ > rex.ਸਤਰ
				&& vim_iswordc_buf(rex.ਨਿਵੇਸ਼[-1], rex.reg_buf)))
		    ਸਥਿਤੀ = RA_NOMATCH;
	    }
	    ਰੋਕੋ;

	  ਹਾਲਤ EOW:	// word\>; rex.input points after d
	    ਜੇ (rex.ਨਿਵੇਸ਼ == rex.ਸਤਰ)    // Can't match at start of line
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ ਜੇ (has_mbyte)
	    {
		ਪੂਰਨ this_class, prev_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.ਨਿਵੇਸ਼, rex.reg_buf);
		prev_class = reg_prev_class();
		ਜੇ (this_class == prev_class
			|| prev_class == 0 || prev_class == 1)
		    ਸਥਿਤੀ = RA_NOMATCH;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (!vim_iswordc_buf(rex.ਨਿਵੇਸ਼[-1], rex.reg_buf)
			|| (rex.ਨਿਵੇਸ਼[0] != NUL
					   && vim_iswordc_buf(c, rex.reg_buf)))
		    ਸਥਿਤੀ = RA_NOMATCH;
	    }
	    ਰੋਕੋ; // Matched with EOW

	  ਹਾਲਤ ANY:
	    // ANY does not match new lines.
	    ਜੇ (c == NUL)
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
		ADVANCE_REGINPUT();
	    ਰੋਕੋ;

	  ਹਾਲਤ IDENT:
	    ਜੇ (!vim_isIDc(c))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
		ADVANCE_REGINPUT();
	    ਰੋਕੋ;

	  ਹਾਲਤ SIDENT:
	    ਜੇ (VIM_ISDIGIT(*rex.ਨਿਵੇਸ਼) || !vim_isIDc(c))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
		ADVANCE_REGINPUT();
	    ਰੋਕੋ;

	  ਹਾਲਤ KWORD:
	    ਜੇ (!vim_iswordp_buf(rex.ਨਿਵੇਸ਼, rex.reg_buf))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
		ADVANCE_REGINPUT();
	    ਰੋਕੋ;

	  ਹਾਲਤ SKWORD:
	    ਜੇ (VIM_ISDIGIT(*rex.ਨਿਵੇਸ਼)
				    || !vim_iswordp_buf(rex.ਨਿਵੇਸ਼, rex.reg_buf))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
		ADVANCE_REGINPUT();
	    ਰੋਕੋ;

	  ਹਾਲਤ FNAME:
	    ਜੇ (!vim_isfilec(c))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
		ADVANCE_REGINPUT();
	    ਰੋਕੋ;

	  ਹਾਲਤ SFNAME:
	    ਜੇ (VIM_ISDIGIT(*rex.ਨਿਵੇਸ਼) || !vim_isfilec(c))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
		ADVANCE_REGINPUT();
	    ਰੋਕੋ;

	  ਹਾਲਤ PRINT:
	    ਜੇ (!vim_isprintc(PTR2CHAR(rex.ਨਿਵੇਸ਼)))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
		ADVANCE_REGINPUT();
	    ਰੋਕੋ;

	  ਹਾਲਤ SPRINT:
	    ਜੇ (VIM_ISDIGIT(*rex.ਨਿਵੇਸ਼) || !vim_isprintc(PTR2CHAR(rex.ਨਿਵੇਸ਼)))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
		ADVANCE_REGINPUT();
	    ਰੋਕੋ;

	  ਹਾਲਤ WHITE:
	    ਜੇ (!VIM_ISWHITE(c))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
		ADVANCE_REGINPUT();
	    ਰੋਕੋ;

	  ਹਾਲਤ NWHITE:
	    ਜੇ (c == NUL || VIM_ISWHITE(c))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
		ADVANCE_REGINPUT();
	    ਰੋਕੋ;

	  ਹਾਲਤ DIGIT:
	    ਜੇ (!ri_digit(c))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
		ADVANCE_REGINPUT();
	    ਰੋਕੋ;

	  ਹਾਲਤ NDIGIT:
	    ਜੇ (c == NUL || ri_digit(c))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
		ADVANCE_REGINPUT();
	    ਰੋਕੋ;

	  ਹਾਲਤ HEX:
	    ਜੇ (!ri_hex(c))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
		ADVANCE_REGINPUT();
	    ਰੋਕੋ;

	  ਹਾਲਤ NHEX:
	    ਜੇ (c == NUL || ri_hex(c))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
		ADVANCE_REGINPUT();
	    ਰੋਕੋ;

	  ਹਾਲਤ OCTAL:
	    ਜੇ (!ri_octal(c))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
		ADVANCE_REGINPUT();
	    ਰੋਕੋ;

	  ਹਾਲਤ NOCTAL:
	    ਜੇ (c == NUL || ri_octal(c))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
		ADVANCE_REGINPUT();
	    ਰੋਕੋ;

	  ਹਾਲਤ ਸ਼ਬਦ_2:
	    ਜੇ (!ri_ਸ਼ਬਦ(c))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
		ADVANCE_REGINPUT();
	    ਰੋਕੋ;

	  ਹਾਲਤ NWORD:
	    ਜੇ (c == NUL || ri_ਸ਼ਬਦ(c))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
		ADVANCE_REGINPUT();
	    ਰੋਕੋ;

	  ਹਾਲਤ ਸਿਰ_2:
	    ਜੇ (!ri_ਸਿਰ(c))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
		ADVANCE_REGINPUT();
	    ਰੋਕੋ;

	  ਹਾਲਤ NHEAD:
	    ਜੇ (c == NUL || ri_ਸਿਰ(c))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
		ADVANCE_REGINPUT();
	    ਰੋਕੋ;

	  ਹਾਲਤ ALPHA:
	    ਜੇ (!ri_alpha(c))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
		ADVANCE_REGINPUT();
	    ਰੋਕੋ;

	  ਹਾਲਤ NALPHA:
	    ਜੇ (c == NUL || ri_alpha(c))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
		ADVANCE_REGINPUT();
	    ਰੋਕੋ;

	  ਹਾਲਤ ਹੇਠਲਾ_2:
	    ਜੇ (!ri_ਹੇਠਲਾ(c))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
		ADVANCE_REGINPUT();
	    ਰੋਕੋ;

	  ਹਾਲਤ NLOWER:
	    ਜੇ (c == NUL || ri_ਹੇਠਲਾ(c))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
		ADVANCE_REGINPUT();
	    ਰੋਕੋ;

	  ਹਾਲਤ ਉੱਤਲਾ:
	    ਜੇ (!ri_ਉੱਤਲਾ(c))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
		ADVANCE_REGINPUT();
	    ਰੋਕੋ;

	  ਹਾਲਤ NUPPER:
	    ਜੇ (c == NUL || ri_ਉੱਤਲਾ(c))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
		ADVANCE_REGINPUT();
	    ਰੋਕੋ;

	  ਹਾਲਤ EXACTLY:
	    {
		ਪੂਰਨ	len;
		char_u	*opnd;

		opnd = ਸੰਕਾਰਜ(ਜਾਂਚ);
		// Inline the first byte, for speed.
		ਜੇ (*opnd != *rex.ਨਿਵੇਸ਼
			&& (!rex.reg_ic
			    || (!enc_utf8
			      && MB_TOLOWER(*opnd) != MB_TOLOWER(*rex.ਨਿਵੇਸ਼))))
		    ਸਥਿਤੀ = RA_NOMATCH;
		ਨਹੀਂਤਾਂ ਜੇ (*opnd == NUL)
		{
		    // match empty string always works; happens when "~" is
		    // empty.
		}
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (opnd[1] == NUL && !(enc_utf8 && rex.reg_ic))
		    {
			len = 1;	// matched a single byte above
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// Need to match first byte again for multi-byte.
			len = (ਪੂਰਨ)STRLEN(opnd);
			ਜੇ (cstrncmp(opnd, rex.ਨਿਵੇਸ਼, &len) != 0)
			    ਸਥਿਤੀ = RA_NOMATCH;
		    }
		    // Check for following composing character, unless %C
		    // follows (skips over all composing chars).
		    ਜੇ (ਸਥਿਤੀ != RA_NOMATCH
			    && enc_utf8
			    && UTF_COMPOSINGLIKE(rex.ਨਿਵੇਸ਼, rex.ਨਿਵੇਸ਼ + len)
			    && !rex.reg_icombine
			    && OP(ਅਗਲਾ) != RE_COMPOSING)
		    {
			// raaron: This code makes a composing character get
			// ignored, which is the correct behavior (sometimes)
			// for voweled Hebrew texts.
			ਸਥਿਤੀ = RA_NOMATCH;
		    }
		    ਜੇ (ਸਥਿਤੀ != RA_NOMATCH)
			rex.ਨਿਵੇਸ਼ += len;
		}
	    }
	    ਰੋਕੋ;

	  ਹਾਲਤ ANYOF:
	  ਹਾਲਤ ANYBUT:
	    {
		char_u  *q = ਸੰਕਾਰਜ(ਜਾਂਚ);

		ਜੇ (c == NUL)
		    ਸਥਿਤੀ = RA_NOMATCH;
		ਨਹੀਂਤਾਂ ਜੇ ((cstrchr(q, c) == ਨਿਰਾ) == (op == ANYOF))
		    ਸਥਿਤੀ = RA_NOMATCH;
		ਨਹੀਂਤਾਂ
		{
		    // Check following combining characters
		    ਪੂਰਨ	len = 0;
		    ਪੂਰਨ i;

		    ਜੇ (enc_utf8)
			len = utfc_ptr2len(q) - utf_ptr2len(q);

		    MB_CPTR_ADV(rex.ਨਿਵੇਸ਼);
		    MB_CPTR_ADV(q);

		    ਜੇ (!enc_utf8 || len == 0)
			ਰੋਕੋ;

		    ਲਈ (i = 0; i < len; ++i)
			ਜੇ (q[i] != rex.ਨਿਵੇਸ਼[i])
			{
			    ਸਥਿਤੀ = RA_NOMATCH;
			    ਰੋਕੋ;
			}
		    rex.ਨਿਵੇਸ਼ += len;
		}
		ਰੋਕੋ;
	    }

	  ਹਾਲਤ MULTIBYTECODE:
	    ਜੇ (has_mbyte)
	    {
		ਪੂਰਨ	i, len;
		char_u	*opnd;
		ਪੂਰਨ	opndc = 0, inpc;

		opnd = ਸੰਕਾਰਜ(ਜਾਂਚ);
		// Safety check (just in case 'encoding' was changed since
		// compiling the program).
		ਜੇ ((len = (*mb_ptr2len)(opnd)) < 2)
		{
		    ਸਥਿਤੀ = RA_NOMATCH;
		    ਰੋਕੋ;
		}
		ਜੇ (enc_utf8)
		    opndc = utf_ptr2char(opnd);
		ਜੇ (enc_utf8 && utf_iscomposing(opndc))
		{
		    // When only a composing char is given match at any
		    // position where that composing char appears.
		    ਸਥਿਤੀ = RA_NOMATCH;
		    ਲਈ (i = 0; rex.ਨਿਵੇਸ਼[i] != NUL;
						i += utf_ptr2len(rex.ਨਿਵੇਸ਼ + i))
		    {
			inpc = utf_ptr2char(rex.ਨਿਵੇਸ਼ + i);
			ਜੇ (!utf_iscomposing(inpc))
			{
			    ਜੇ (i > 0)
				ਰੋਕੋ;
			}
			ਨਹੀਂਤਾਂ ਜੇ (opndc == inpc)
			{
			    // Include all following composing chars.
			    len = i + utfc_ptr2len(rex.ਨਿਵੇਸ਼ + i);
			    ਸਥਿਤੀ = RA_ਮੇਲ;
			    ਰੋਕੋ;
			}
		    }
		}
		ਨਹੀਂਤਾਂ ਜੇ (enc_utf8)
		{
		    ਜੇ (cstrncmp(opnd, rex.ਨਿਵੇਸ਼, &len) != 0)
		    {
			ਸਥਿਤੀ = RA_NOMATCH;
			ਰੋਕੋ;
		    }
		}
		ਨਹੀਂਤਾਂ
		    ਲਈ (i = 0; i < len; ++i)
			ਜੇ (opnd[i] != rex.ਨਿਵੇਸ਼[i])
			{
			    ਸਥਿਤੀ = RA_NOMATCH;
			    ਰੋਕੋ;
			}
		rex.ਨਿਵੇਸ਼ += len;
	    }
	    ਨਹੀਂਤਾਂ
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਰੋਕੋ;
	  ਹਾਲਤ RE_COMPOSING:
	    ਜੇ (enc_utf8)
	    {
		// Skip composing characters.
		ਜਦੋਂ (utf_iscomposing(utf_ptr2char(rex.ਨਿਵੇਸ਼)))
		    MB_CPTR_ADV(rex.ਨਿਵੇਸ਼);
	    }
	    ਰੋਕੋ;

	  ਹਾਲਤ NOTHING:
	    ਰੋਕੋ;

	  ਹਾਲਤ ਪਿੱਠ_2:
	    {
		ਪੂਰਨ		i;
		backpos_T	*bp;

		// When we run into BACK we need to check if we don't keep
		// looping without matching any input.  The second and later
		// times a BACK is encountered it fails if the input is still
		// at the same position as the previous time.
		// The positions are stored in "backpos" and found by the
		// current value of "scan", the position in the RE program.
		bp = (backpos_T *)backpos.ga_ਅੰਕੜੇ;
		ਲਈ (i = 0; i < backpos.ga_len; ++i)
		    ਜੇ (bp[i].bp_ਜਾਂਚ == ਜਾਂਚ)
			ਰੋਕੋ;
		ਜੇ (i == backpos.ga_len)
		{
		    // First time at this BACK, make room to store the pos.
		    ਜੇ (GA_GROW_FAILS(&backpos, 1))
			ਸਥਿਤੀ = RA_FAIL;
		    ਨਹੀਂਤਾਂ
		    {
			// get "ga_data" again, it may have changed
			bp = (backpos_T *)backpos.ga_ਅੰਕੜੇ;
			bp[i].bp_ਜਾਂਚ = ਜਾਂਚ;
			++backpos.ga_len;
		    }
		}
		ਨਹੀਂਤਾਂ ਜੇ (reg_save_equal(&bp[i].bp_pos))
		    // Still at same position as last time, fail.
		    ਸਥਿਤੀ = RA_NOMATCH;

		ਜੇ (ਸਥਿਤੀ != RA_FAIL && ਸਥਿਤੀ != RA_NOMATCH)
		    reg_save(&bp[i].bp_pos, &backpos);
	    }
	    ਰੋਕੋ;

	  ਹਾਲਤ MOPEN + 0:   // Match start: \zs
	  ਹਾਲਤ MOPEN + 1:   // \(
	  ਹਾਲਤ MOPEN + 2:
	  ਹਾਲਤ MOPEN + 3:
	  ਹਾਲਤ MOPEN + 4:
	  ਹਾਲਤ MOPEN + 5:
	  ਹਾਲਤ MOPEN + 6:
	  ਹਾਲਤ MOPEN + 7:
	  ਹਾਲਤ MOPEN + 8:
	  ਹਾਲਤ MOPEN + 9:
	    {
		no = op - MOPEN;
		cleanup_subexpr();
		rp = regstack_ਧੱਕੋ(RS_MOPEN, ਜਾਂਚ);
		ਜੇ (rp == ਨਿਰਾ)
		    ਸਥਿਤੀ = RA_FAIL;
		ਨਹੀਂਤਾਂ
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_startpos[no],
							  &rex.reg_startp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    ਰੋਕੋ;

	  ਹਾਲਤ NOPEN:	    // \%(
	  ਹਾਲਤ NCLOSE:	    // \) after \%(
		ਜੇ (regstack_ਧੱਕੋ(RS_NOPEN, ਜਾਂਚ) == ਨਿਰਾ)
		    ਸਥਿਤੀ = RA_FAIL;
		// We simply continue and handle the result when done.
		ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	  ਹਾਲਤ ZOPEN + 1:
	  ਹਾਲਤ ZOPEN + 2:
	  ਹਾਲਤ ZOPEN + 3:
	  ਹਾਲਤ ZOPEN + 4:
	  ਹਾਲਤ ZOPEN + 5:
	  ਹਾਲਤ ZOPEN + 6:
	  ਹਾਲਤ ZOPEN + 7:
	  ਹਾਲਤ ZOPEN + 8:
	  ਹਾਲਤ ZOPEN + 9:
	    {
		no = op - ZOPEN;
		cleanup_zsubexpr();
		rp = regstack_ਧੱਕੋ(RS_ZOPEN, ਜਾਂਚ);
		ਜੇ (rp == ਨਿਰਾ)
		    ਸਥਿਤੀ = RA_FAIL;
		ਨਹੀਂਤਾਂ
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_startzpos[no],
							     &reg_startzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ

	  ਹਾਲਤ MCLOSE + 0:  // Match end: \ze
	  ਹਾਲਤ MCLOSE + 1:  // \)
	  ਹਾਲਤ MCLOSE + 2:
	  ਹਾਲਤ MCLOSE + 3:
	  ਹਾਲਤ MCLOSE + 4:
	  ਹਾਲਤ MCLOSE + 5:
	  ਹਾਲਤ MCLOSE + 6:
	  ਹਾਲਤ MCLOSE + 7:
	  ਹਾਲਤ MCLOSE + 8:
	  ਹਾਲਤ MCLOSE + 9:
	    {
		no = op - MCLOSE;
		cleanup_subexpr();
		rp = regstack_ਧੱਕੋ(RS_MCLOSE, ਜਾਂਚ);
		ਜੇ (rp == ਨਿਰਾ)
		    ਸਥਿਤੀ = RA_FAIL;
		ਨਹੀਂਤਾਂ
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_endpos[no],
							    &rex.reg_endp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	  ਹਾਲਤ ZCLOSE + 1:  // \) after \z(
	  ਹਾਲਤ ZCLOSE + 2:
	  ਹਾਲਤ ZCLOSE + 3:
	  ਹਾਲਤ ZCLOSE + 4:
	  ਹਾਲਤ ZCLOSE + 5:
	  ਹਾਲਤ ZCLOSE + 6:
	  ਹਾਲਤ ZCLOSE + 7:
	  ਹਾਲਤ ZCLOSE + 8:
	  ਹਾਲਤ ZCLOSE + 9:
	    {
		no = op - ZCLOSE;
		cleanup_zsubexpr();
		rp = regstack_ਧੱਕੋ(RS_ZCLOSE, ਜਾਂਚ);
		ਜੇ (rp == ਨਿਰਾ)
		    ਸਥਿਤੀ = RA_FAIL;
		ਨਹੀਂਤਾਂ
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_endzpos[no],
							      &reg_endzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ

	  ਹਾਲਤ BACKREF + 1:
	  ਹਾਲਤ BACKREF + 2:
	  ਹਾਲਤ BACKREF + 3:
	  ਹਾਲਤ BACKREF + 4:
	  ਹਾਲਤ BACKREF + 5:
	  ਹਾਲਤ BACKREF + 6:
	  ਹਾਲਤ BACKREF + 7:
	  ਹਾਲਤ BACKREF + 8:
	  ਹਾਲਤ BACKREF + 9:
	    {
		ਪੂਰਨ		len;

		no = op - BACKREF;
		cleanup_subexpr();
		ਜੇ (!REG_MULTI)		// Single-line regexp
		{
		    ਜੇ (rex.reg_startp[no] == ਨਿਰਾ || rex.reg_endp[no] == ਨਿਰਾ)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// Compare current input with back-ref in the same
			// line.
			len = (ਪੂਰਨ)(rex.reg_endp[no] - rex.reg_startp[no]);
			ਜੇ (cstrncmp(rex.reg_startp[no], rex.ਨਿਵੇਸ਼, &len) != 0)
			    ਸਥਿਤੀ = RA_NOMATCH;
		    }
		}
		ਨਹੀਂਤਾਂ				// Multi-line regexp
		{
		    ਜੇ (rex.reg_startpos[no].lnum < 0
						|| rex.reg_endpos[no].lnum < 0)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ (rex.reg_startpos[no].lnum == rex.lnum
				&& rex.reg_endpos[no].lnum == rex.lnum)
			{
			    // Compare back-ref within the current line.
			    len = rex.reg_endpos[no].ਖਾਨਾ
						    - rex.reg_startpos[no].ਖਾਨਾ;
			    ਜੇ (cstrncmp(rex.ਸਤਰ + rex.reg_startpos[no].ਖਾਨਾ,
							  rex.ਨਿਵੇਸ਼, &len) != 0)
				ਸਥਿਤੀ = RA_NOMATCH;
			}
			ਨਹੀਂਤਾਂ
			{
			    // Messy situation: Need to compare between two
			    // lines.
			    ਪੂਰਨ r = ਮੇਲ_with_backref(
					    rex.reg_startpos[no].lnum,
					    rex.reg_startpos[no].ਖਾਨਾ,
					    rex.reg_endpos[no].lnum,
					    rex.reg_endpos[no].ਖਾਨਾ,
					    &len);

			    ਜੇ (r != RA_ਮੇਲ)
				ਸਥਿਤੀ = r;
			}
		    }
		}

		// Matched the backref, skip over it.
		rex.ਨਿਵੇਸ਼ += len;
	    }
	    ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	  ਹਾਲਤ ZREF + 1:
	  ਹਾਲਤ ZREF + 2:
	  ਹਾਲਤ ZREF + 3:
	  ਹਾਲਤ ZREF + 4:
	  ਹਾਲਤ ZREF + 5:
	  ਹਾਲਤ ZREF + 6:
	  ਹਾਲਤ ZREF + 7:
	  ਹਾਲਤ ZREF + 8:
	  ਹਾਲਤ ZREF + 9:
	    {
		ਪੂਰਨ	len;

		cleanup_zsubexpr();
		no = op - ZREF;
		ਜੇ (re_extmatch_ਵਿੱਚ != ਨਿਰਾ
			&& re_extmatch_ਵਿੱਚ->matches[no] != ਨਿਰਾ)
		{
		    len = (ਪੂਰਨ)STRLEN(re_extmatch_ਵਿੱਚ->matches[no]);
		    ਜੇ (cstrncmp(re_extmatch_ਵਿੱਚ->matches[no],
							  rex.ਨਿਵੇਸ਼, &len) != 0)
			ਸਥਿਤੀ = RA_NOMATCH;
		    ਨਹੀਂਤਾਂ
			rex.ਨਿਵੇਸ਼ += len;
		}
		ਨਹੀਂਤਾਂ
		{
		    // Backref was not set: Match an empty string.
		}
	    }
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ

	  ਹਾਲਤ BRANCH:
	    {
		ਜੇ (OP(ਅਗਲਾ) != BRANCH) // No choice.
		    ਅਗਲਾ = ਸੰਕਾਰਜ(ਜਾਂਚ);	// Avoid recursion.
		ਨਹੀਂਤਾਂ
		{
		    rp = regstack_ਧੱਕੋ(RS_BRANCH, ਜਾਂਚ);
		    ਜੇ (rp == ਨਿਰਾ)
			ਸਥਿਤੀ = RA_FAIL;
		    ਨਹੀਂਤਾਂ
			ਸਥਿਤੀ = RA_BREAK;	// rest is below
		}
	    }
	    ਰੋਕੋ;

	  ਹਾਲਤ BRACE_LIMITS:
	    {
		ਜੇ (OP(ਅਗਲਾ) == BRACE_SIMPLE)
		{
		    bl_minval = ਸੰਕਾਰਜ_MIN(ਜਾਂਚ);
		    bl_maxval = ਸੰਕਾਰਜ_MAX(ਜਾਂਚ);
		}
		ਨਹੀਂਤਾਂ ਜੇ (OP(ਅਗਲਾ) >= BRACE_COMPLEX
			&& OP(ਅਗਲਾ) < BRACE_COMPLEX + 10)
		{
		    no = OP(ਅਗਲਾ) - BRACE_COMPLEX;
		    brace_min[no] = ਸੰਕਾਰਜ_MIN(ਜਾਂਚ);
		    brace_max[no] = ਸੰਕਾਰਜ_MAX(ਜਾਂਚ);
		    brace_ਗਿਣਤੀ[no] = 0;
		}
		ਨਹੀਂਤਾਂ
		{
		    ਅੰਦਰੂਨੀ_error("BRACE_LIMITS");
		    ਸਥਿਤੀ = RA_FAIL;
		}
	    }
	    ਰੋਕੋ;

	  ਹਾਲਤ BRACE_COMPLEX + 0:
	  ਹਾਲਤ BRACE_COMPLEX + 1:
	  ਹਾਲਤ BRACE_COMPLEX + 2:
	  ਹਾਲਤ BRACE_COMPLEX + 3:
	  ਹਾਲਤ BRACE_COMPLEX + 4:
	  ਹਾਲਤ BRACE_COMPLEX + 5:
	  ਹਾਲਤ BRACE_COMPLEX + 6:
	  ਹਾਲਤ BRACE_COMPLEX + 7:
	  ਹਾਲਤ BRACE_COMPLEX + 8:
	  ਹਾਲਤ BRACE_COMPLEX + 9:
	    {
		no = op - BRACE_COMPLEX;
		++brace_ਗਿਣਤੀ[no];

		// If not matched enough times yet, try one more
		ਜੇ (brace_ਗਿਣਤੀ[no] <= (brace_min[no] <= brace_max[no]
					     ? brace_min[no] : brace_max[no]))
		{
		    rp = regstack_ਧੱਕੋ(RS_BRCPLX_ਹੋਰ, ਜਾਂਚ);
		    ਜੇ (rp == ਨਿਰਾ)
			ਸਥਿਤੀ = RA_FAIL;
		    ਨਹੀਂਤਾਂ
		    {
			rp->rs_no = no;
			reg_save(&rp->rs_un.regsave, &backpos);
			ਅਗਲਾ = ਸੰਕਾਰਜ(ਜਾਂਚ);
			// We continue and handle the result when done.
		    }
		    ਰੋਕੋ;
		}

		// If matched enough times, may try matching some more
		ਜੇ (brace_min[no] <= brace_max[no])
		{
		    // Range is the normal way around, use longest match
		    ਜੇ (brace_ਗਿਣਤੀ[no] <= brace_max[no])
		    {
			rp = regstack_ਧੱਕੋ(RS_BRCPLX_LONG, ਜਾਂਚ);
			ਜੇ (rp == ਨਿਰਾ)
			    ਸਥਿਤੀ = RA_FAIL;
			ਨਹੀਂਤਾਂ
			{
			    rp->rs_no = no;
			    reg_save(&rp->rs_un.regsave, &backpos);
			    ਅਗਲਾ = ਸੰਕਾਰਜ(ਜਾਂਚ);
			    // We continue and handle the result when done.
			}
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    // Range is backwards, use shortest match first
		    ਜੇ (brace_ਗਿਣਤੀ[no] <= brace_min[no])
		    {
			rp = regstack_ਧੱਕੋ(RS_BRCPLX_SHORT, ਜਾਂਚ);
			ਜੇ (rp == ਨਿਰਾ)
			    ਸਥਿਤੀ = RA_FAIL;
			ਨਹੀਂਤਾਂ
			{
			    reg_save(&rp->rs_un.regsave, &backpos);
			    // We continue and handle the result when done.
			}
		    }
		}
	    }
	    ਰੋਕੋ;

	  ਹਾਲਤ BRACE_SIMPLE:
	  ਹਾਲਤ ਤਾਰਾ:
	  ਹਾਲਤ PLUS:
	    {
		regstar_T	rst;

		// Lookahead to avoid useless match attempts when we know
		// what character comes next.
		ਜੇ (OP(ਅਗਲਾ) == EXACTLY)
		{
		    rst.nextb = *ਸੰਕਾਰਜ(ਅਗਲਾ);
		    ਜੇ (rex.reg_ic)
		    {
			ਜੇ (MB_ISUPPER(rst.nextb))
			    rst.nextb_ic = MB_TOLOWER(rst.nextb);
			ਨਹੀਂਤਾਂ
			    rst.nextb_ic = MB_TOUPPER(rst.nextb);
		    }
		    ਨਹੀਂਤਾਂ
			rst.nextb_ic = rst.nextb;
		}
		ਨਹੀਂਤਾਂ
		{
		    rst.nextb = NUL;
		    rst.nextb_ic = NUL;
		}
		ਜੇ (op != BRACE_SIMPLE)
		{
		    rst.minval = (op == ਤਾਰਾ) ? 0 : 1;
		    rst.maxval = MAX_ਹੱਦਬੰਦੀ;
		}
		ਨਹੀਂਤਾਂ
		{
		    rst.minval = bl_minval;
		    rst.maxval = bl_maxval;
		}

		// When maxval > minval, try matching as much as possible, up
		// to maxval.  When maxval < minval, try matching at least the
		// minimal number (since the range is backwards, that's also
		// maxval!).
		rst.ਗਿਣਤੀ_2 = regrepeat(ਸੰਕਾਰਜ(ਜਾਂਚ), rst.maxval);
		ਜੇ (got_int)
		{
		    ਸਥਿਤੀ = RA_FAIL;
		    ਰੋਕੋ;
		}
		ਜੇ (rst.minval <= rst.maxval
			  ? rst.ਗਿਣਤੀ_2 >= rst.minval : rst.ਗਿਣਤੀ_2 >= rst.maxval)
		{
		    // It could match.  Prepare for trying to match what
		    // follows.  The code is below.  Parameters are stored in
		    // a regstar_T on the regstack.
		    ਜੇ ((ਲੰਮਾ)((ਬਿਨਾਨਿਸ਼ਾਨੀ)regstack.ga_len >> 10) >= p_mmp)
		    {
			emsg(_(e_ਨਮੂਨਾ_ਵਰਤਦਾ_ਹੋਰ_ਸਮ੍ਰਿਤੀ_than_maxmempattern));
			ਸਥਿਤੀ = RA_FAIL;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (GA_GROW_FAILS(&regstack, (ਪੂਰਨ)ਆਕਾਰ(regstar_T)))
			ਸਥਿਤੀ = RA_FAIL;
		    ਨਹੀਂਤਾਂ
		    {
			regstack.ga_len += ਆਕਾਰ(regstar_T);
			rp = regstack_ਧੱਕੋ(rst.minval <= rst.maxval
					? RS_ਤਾਰਾ_LONG : RS_ਤਾਰਾ_SHORT, ਜਾਂਚ);
			ਜੇ (rp == ਨਿਰਾ)
			    ਸਥਿਤੀ = RA_FAIL;
			ਨਹੀਂਤਾਂ
			{
			    *(((regstar_T *)rp) - 1) = rst;
			    ਸਥਿਤੀ = RA_BREAK;	    // skip the restore bits
			}
		    }
		}
		ਨਹੀਂਤਾਂ
		    ਸਥਿਤੀ = RA_NOMATCH;

	    }
	    ਰੋਕੋ;

	  ਹਾਲਤ NOMATCH:
	  ਹਾਲਤ ਮੇਲ_2:
	  ਹਾਲਤ SUBPAT:
	    rp = regstack_ਧੱਕੋ(RS_NOMATCH, ਜਾਂਚ);
	    ਜੇ (rp == ਨਿਰਾ)
		ਸਥਿਤੀ = RA_FAIL;
	    ਨਹੀਂਤਾਂ
	    {
		rp->rs_no = op;
		reg_save(&rp->rs_un.regsave, &backpos);
		ਅਗਲਾ = ਸੰਕਾਰਜ(ਜਾਂਚ);
		// We continue and handle the result when done.
	    }
	    ਰੋਕੋ;

	  ਹਾਲਤ BEHIND:
	  ਹਾਲਤ NOBEHIND:
	    // Need a bit of room to store extra positions.
	    ਜੇ ((ਲੰਮਾ)((ਬਿਨਾਨਿਸ਼ਾਨੀ)regstack.ga_len >> 10) >= p_mmp)
	    {
		emsg(_(e_ਨਮੂਨਾ_ਵਰਤਦਾ_ਹੋਰ_ਸਮ੍ਰਿਤੀ_than_maxmempattern));
		ਸਥਿਤੀ = RA_FAIL;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (GA_GROW_FAILS(&regstack, (ਪੂਰਨ)ਆਕਾਰ(regbehind_T)))
		ਸਥਿਤੀ = RA_FAIL;
	    ਨਹੀਂਤਾਂ
	    {
		regstack.ga_len += ਆਕਾਰ(regbehind_T);
		rp = regstack_ਧੱਕੋ(RS_BEHIND1, ਜਾਂਚ);
		ਜੇ (rp == ਨਿਰਾ)
		    ਸਥਿਤੀ = RA_FAIL;
		ਨਹੀਂਤਾਂ
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
	    ਰੋਕੋ;

	  ਹਾਲਤ BHPOS:
	    ਜੇ (REG_MULTI)
	    {
		ਜੇ (behind_pos.rs_u.pos.ਖਾਨਾ != (colnr_T)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ)
			|| behind_pos.rs_u.pos.lnum != rex.lnum)
		    ਸਥਿਤੀ = RA_NOMATCH;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (behind_pos.rs_u.ptr != rex.ਨਿਵੇਸ਼)
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਰੋਕੋ;

	  ਹਾਲਤ NEWL:
	    ਜੇ ((c != NUL || !REG_MULTI || rex.lnum > rex.reg_maxline
			     || rex.reg_ਸਤਰ_lbr)
					   && (c != '\n' || !rex.reg_ਸਤਰ_lbr))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ ਜੇ (rex.reg_ਸਤਰ_lbr)
		ADVANCE_REGINPUT();
	    ਨਹੀਂਤਾਂ
		reg_nextline();
	    ਰੋਕੋ;

	  ਹਾਲਤ ਸਿਰਾ_3:
	    ਸਥਿਤੀ = RA_ਮੇਲ;	// Success!
	    ਰੋਕੋ;

	  ਮੂਲ:
	    iemsg(e_corrupted_regexp_ਪ੍ਰੋਗਰਾਮ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
	    ਲਿਖੋ("Illegal op code %d\n", op);
#ਜੇ_ਅੰਤ
	    ਸਥਿਤੀ = RA_FAIL;
	    ਰੋਕੋ;
	  }
	}

	// If we can't continue sequentially, break the inner loop.
	ਜੇ (ਸਥਿਤੀ != RA_CONT)
	    ਰੋਕੋ;

	// Continue in inner loop, advance to next item.
	ਜਾਂਚ = ਅਗਲਾ;

    } // end of inner loop

    // If there is something on the regstack execute the code for the state.
    // If the state is popped then loop and use the older state.
    ਜਦੋਂ (regstack.ga_len > 0 && ਸਥਿਤੀ != RA_FAIL)
    {
	rp = (regitem_T *)((ਅੱਖਰ *)regstack.ga_ਅੰਕੜੇ + regstack.ga_len) - 1;
	ਚੋਣ (rp->rs_state)
	{
	  ਹਾਲਤ RS_NOPEN:
	    // Result is passed on as-is, simply pop the state.
	    regstack_ਕੱਢੋ(&ਜਾਂਚ);
	    ਰੋਕੋ;

	  ਹਾਲਤ RS_MOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    ਜੇ (ਸਥਿਤੀ == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_startpos[rp->rs_no],
						  &rex.reg_startp[rp->rs_no]);
	    regstack_ਕੱਢੋ(&ਜਾਂਚ);
	    ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	  ਹਾਲਤ RS_ZOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    ਜੇ (ਸਥਿਤੀ == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_startzpos[rp->rs_no],
						 &reg_startzp[rp->rs_no]);
	    regstack_ਕੱਢੋ(&ਜਾਂਚ);
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ

	  ਹਾਲਤ RS_MCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    ਜੇ (ਸਥਿਤੀ == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_endpos[rp->rs_no],
						    &rex.reg_endp[rp->rs_no]);
	    regstack_ਕੱਢੋ(&ਜਾਂਚ);
	    ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	  ਹਾਲਤ RS_ZCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    ਜੇ (ਸਥਿਤੀ == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_endzpos[rp->rs_no],
						   &reg_endzp[rp->rs_no]);
	    regstack_ਕੱਢੋ(&ਜਾਂਚ);
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ

	  ਹਾਲਤ RS_BRANCH:
	    ਜੇ (ਸਥਿਤੀ == RA_ਮੇਲ)
		// this branch matched, use it
		regstack_ਕੱਢੋ(&ਜਾਂਚ);
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (ਸਥਿਤੀ != RA_BREAK)
		{
		    // After a non-matching branch: try next one.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    ਜਾਂਚ = rp->rs_ਜਾਂਚ;
		}
		ਜੇ (ਜਾਂਚ == ਨਿਰਾ || OP(ਜਾਂਚ) != BRANCH)
		{
		    // no more branches, didn't find a match
		    ਸਥਿਤੀ = RA_NOMATCH;
		    regstack_ਕੱਢੋ(&ਜਾਂਚ);
		}
		ਨਹੀਂਤਾਂ
		{
		    // Prepare to try a branch.
		    rp->rs_ਜਾਂਚ = regnext(ਜਾਂਚ);
		    reg_save(&rp->rs_un.regsave, &backpos);
		    ਜਾਂਚ = ਸੰਕਾਰਜ(ਜਾਂਚ);
		}
	    }
	    ਰੋਕੋ;

	  ਹਾਲਤ RS_BRCPLX_ਹੋਰ:
	    // Pop the state.  Restore pointers when there is no match.
	    ਜੇ (ਸਥਿਤੀ == RA_NOMATCH)
	    {
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_ਗਿਣਤੀ[rp->rs_no];	// decrement match count
	    }
	    regstack_ਕੱਢੋ(&ਜਾਂਚ);
	    ਰੋਕੋ;

	  ਹਾਲਤ RS_BRCPLX_LONG:
	    // Pop the state.  Restore pointers when there is no match.
	    ਜੇ (ਸਥਿਤੀ == RA_NOMATCH)
	    {
		// There was no match, but we did find enough matches.
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_ਗਿਣਤੀ[rp->rs_no];
		// continue with the items after "\{}"
		ਸਥਿਤੀ = RA_CONT;
	    }
	    regstack_ਕੱਢੋ(&ਜਾਂਚ);
	    ਜੇ (ਸਥਿਤੀ == RA_CONT)
		ਜਾਂਚ = regnext(ਜਾਂਚ);
	    ਰੋਕੋ;

	  ਹਾਲਤ RS_BRCPLX_SHORT:
	    // Pop the state.  Restore pointers when there is no match.
	    ਜੇ (ਸਥਿਤੀ == RA_NOMATCH)
		// There was no match, try to match one more item.
		reg_restore(&rp->rs_un.regsave, &backpos);
	    regstack_ਕੱਢੋ(&ਜਾਂਚ);
	    ਜੇ (ਸਥਿਤੀ == RA_NOMATCH)
	    {
		ਜਾਂਚ = ਸੰਕਾਰਜ(ਜਾਂਚ);
		ਸਥਿਤੀ = RA_CONT;
	    }
	    ਰੋਕੋ;

	  ਹਾਲਤ RS_NOMATCH:
	    // Pop the state.  If the operand matches for NOMATCH or
	    // doesn't match for MATCH/SUBPAT, we fail.  Otherwise backup,
	    // except for SUBPAT, and continue with the next item.
	    ਜੇ (ਸਥਿਤੀ == (rp->rs_no == NOMATCH ? RA_ਮੇਲ : RA_NOMATCH))
		ਸਥਿਤੀ = RA_NOMATCH;
	    ਨਹੀਂਤਾਂ
	    {
		ਸਥਿਤੀ = RA_CONT;
		ਜੇ (rp->rs_no != SUBPAT)	// zero-width
		    reg_restore(&rp->rs_un.regsave, &backpos);
	    }
	    regstack_ਕੱਢੋ(&ਜਾਂਚ);
	    ਜੇ (ਸਥਿਤੀ == RA_CONT)
		ਜਾਂਚ = regnext(ਜਾਂਚ);
	    ਰੋਕੋ;

	  ਹਾਲਤ RS_BEHIND1:
	    ਜੇ (ਸਥਿਤੀ == RA_NOMATCH)
	    {
		regstack_ਕੱਢੋ(&ਜਾਂਚ);
		regstack.ga_len -= ਆਕਾਰ(regbehind_T);
	    }
	    ਨਹੀਂਤਾਂ
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
		ਜਾਂਚ = ਸੰਕਾਰਜ(rp->rs_ਜਾਂਚ) + 4;
	    }
	    ਰੋਕੋ;

	  ਹਾਲਤ RS_BEHIND2:
	    // Looping for BEHIND / NOBEHIND match.
	    ਜੇ (ਸਥਿਤੀ == RA_ਮੇਲ && reg_save_equal(&behind_pos))
	    {
		// found a match that ends where "next" started
		behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		ਜੇ (rp->rs_no == BEHIND)
		    reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
		ਨਹੀਂਤਾਂ
		{
		    // But we didn't want a match.  Need to restore the
		    // subexpr, because what follows matched, so they have
		    // been set.
		    ਸਥਿਤੀ = RA_NOMATCH;
		    restore_subexpr(((regbehind_T *)rp) - 1);
		}
		regstack_ਕੱਢੋ(&ਜਾਂਚ);
		regstack.ga_len -= ਆਕਾਰ(regbehind_T);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਲੰਮਾ ਹੱਦਬੰਦੀ;

		// No match or a match that doesn't end where we want it: Go
		// back one character.  May go to previous line once.
		no = OK;
		ਹੱਦਬੰਦੀ = ਸੰਕਾਰਜ_MIN(rp->rs_ਜਾਂਚ);
		ਜੇ (REG_MULTI)
		{
		    ਜੇ (ਹੱਦਬੰਦੀ > 0
			    && ((rp->rs_un.regsave.rs_u.pos.lnum
						    < behind_pos.rs_u.pos.lnum
				    ? (colnr_T)STRLEN(rex.ਸਤਰ)
				    : behind_pos.rs_u.pos.ਖਾਨਾ)
				- rp->rs_un.regsave.rs_u.pos.ਖਾਨਾ >= ਹੱਦਬੰਦੀ))
			no = FAIL;
		    ਨਹੀਂਤਾਂ ਜੇ (rp->rs_un.regsave.rs_u.pos.ਖਾਨਾ == 0)
		    {
			ਜੇ (rp->rs_un.regsave.rs_u.pos.lnum
					< behind_pos.rs_u.pos.lnum
				|| reg_getline(
					--rp->rs_un.regsave.rs_u.pos.lnum)
								  == ਨਿਰਾ)
			    no = FAIL;
			ਨਹੀਂਤਾਂ
			{
			    reg_restore(&rp->rs_un.regsave, &backpos);
			    rp->rs_un.regsave.rs_u.pos.ਖਾਨਾ =
						 (colnr_T)STRLEN(rex.ਸਤਰ);
			}
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ (has_mbyte)
			{
			    char_u *ਸਤਰ =
				  reg_getline(rp->rs_un.regsave.rs_u.pos.lnum);

			    rp->rs_un.regsave.rs_u.pos.ਖਾਨਾ -=
				(*mb_ਸਿਰ_off)(ਸਤਰ, ਸਤਰ
				    + rp->rs_un.regsave.rs_u.pos.ਖਾਨਾ - 1) + 1;
			}
			ਨਹੀਂਤਾਂ
			    --rp->rs_un.regsave.rs_u.pos.ਖਾਨਾ;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (rp->rs_un.regsave.rs_u.ptr == rex.ਸਤਰ)
			no = FAIL;
		    ਨਹੀਂਤਾਂ
		    {
			MB_PTR_ਪਿੱਠ(rex.ਸਤਰ, rp->rs_un.regsave.rs_u.ptr);
			ਜੇ (ਹੱਦਬੰਦੀ > 0 && (ਲੰਮਾ)(behind_pos.rs_u.ptr
				     - rp->rs_un.regsave.rs_u.ptr) > ਹੱਦਬੰਦੀ)
			    no = FAIL;
		    }
		}
		ਜੇ (no == OK)
		{
		    // Advanced, prepare for finding match again.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    ਜਾਂਚ = ਸੰਕਾਰਜ(rp->rs_ਜਾਂਚ) + 4;
		    ਜੇ (ਸਥਿਤੀ == RA_ਮੇਲ)
		    {
			// We did match, so subexpr may have been changed,
			// need to restore them for the next try.
			ਸਥਿਤੀ = RA_NOMATCH;
			restore_subexpr(((regbehind_T *)rp) - 1);
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    // Can't advance.  For NOBEHIND that's a match.
		    behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		    ਜੇ (rp->rs_no == NOBEHIND)
		    {
			reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
			ਸਥਿਤੀ = RA_ਮੇਲ;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// We do want a proper match.  Need to restore the
			// subexpr if we had a match, because they may have
			// been set.
			ਜੇ (ਸਥਿਤੀ == RA_ਮੇਲ)
			{
			    ਸਥਿਤੀ = RA_NOMATCH;
			    restore_subexpr(((regbehind_T *)rp) - 1);
			}
		    }
		    regstack_ਕੱਢੋ(&ਜਾਂਚ);
		    regstack.ga_len -= ਆਕਾਰ(regbehind_T);
		}
	    }
	    ਰੋਕੋ;

	  ਹਾਲਤ RS_ਤਾਰਾ_LONG:
	  ਹਾਲਤ RS_ਤਾਰਾ_SHORT:
	    {
		regstar_T	    *rst = ((regstar_T *)rp) - 1;

		ਜੇ (ਸਥਿਤੀ == RA_ਮੇਲ)
		{
		    regstack_ਕੱਢੋ(&ਜਾਂਚ);
		    regstack.ga_len -= ਆਕਾਰ(regstar_T);
		    ਰੋਕੋ;
		}

		// Tried once already, restore input pointers.
		ਜੇ (ਸਥਿਤੀ != RA_BREAK)
		    reg_restore(&rp->rs_un.regsave, &backpos);

		// Repeat until we found a position where it could match.
		ਲਈ (;;)
		{
		    ਜੇ (ਸਥਿਤੀ != RA_BREAK)
		    {
			// Tried first position already, advance.
			ਜੇ (rp->rs_state == RS_ਤਾਰਾ_LONG)
			{
			    // Trying for longest match, but couldn't or
			    // didn't match -- back up one char.
			    ਜੇ (--rst->ਗਿਣਤੀ_2 < rst->minval)
				ਰੋਕੋ;
			    ਜੇ (rex.ਨਿਵੇਸ਼ == rex.ਸਤਰ)
			    {
				// backup to last char of previous line
				ਜੇ (rex.lnum == 0)
				{
				    ਸਥਿਤੀ = RA_NOMATCH;
				    ਰੋਕੋ;
				}
				--rex.lnum;
				rex.ਸਤਰ = reg_getline(rex.lnum);
				// Just in case regrepeat() didn't count
				// right.
				ਜੇ (rex.ਸਤਰ == ਨਿਰਾ)
				    ਰੋਕੋ;
				rex.ਨਿਵੇਸ਼ = rex.ਸਤਰ + reg_getline_len(rex.lnum);
				fast_breakcheck();
			    }
			    ਨਹੀਂਤਾਂ
				MB_PTR_ਪਿੱਠ(rex.ਸਤਰ, rex.ਨਿਵੇਸ਼);
			}
			ਨਹੀਂਤਾਂ
			{
			    // Range is backwards, use shortest match first.
			    // Careful: maxval and minval are exchanged!
			    // Couldn't or didn't match: try advancing one
			    // char.
			    ਜੇ (rst->ਗਿਣਤੀ_2 == rst->minval
				  || regrepeat(ਸੰਕਾਰਜ(rp->rs_ਜਾਂਚ), 1L) == 0)
				ਰੋਕੋ;
			    ++rst->ਗਿਣਤੀ_2;
			}
			ਜੇ (got_int)
			    ਰੋਕੋ;
		    }
		    ਨਹੀਂਤਾਂ
			ਸਥਿਤੀ = RA_NOMATCH;

		    // If it could match, try it.
		    ਜੇ (rst->nextb == NUL || *rex.ਨਿਵੇਸ਼ == rst->nextb
					     || *rex.ਨਿਵੇਸ਼ == rst->nextb_ic)
		    {
			reg_save(&rp->rs_un.regsave, &backpos);
			ਜਾਂਚ = regnext(rp->rs_ਜਾਂਚ);
			ਸਥਿਤੀ = RA_CONT;
			ਰੋਕੋ;
		    }
		}
		ਜੇ (ਸਥਿਤੀ != RA_CONT)
		{
		    // Failed.
		    regstack_ਕੱਢੋ(&ਜਾਂਚ);
		    regstack.ga_len -= ਆਕਾਰ(regstar_T);
		    ਸਥਿਤੀ = RA_NOMATCH;
		}
	    }
	    ਰੋਕੋ;
	}

	// If we want to continue the inner loop or didn't pop a state
	// continue matching loop
	ਜੇ (ਸਥਿਤੀ == RA_CONT || rp == (regitem_T *)
			     ((ਅੱਖਰ *)regstack.ga_ਅੰਕੜੇ + regstack.ga_len) - 1)
	    ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
	ਜੇ (bt_did_ਸਮਾਂ_ਬਾਹਰਲਾ(timed_ਬਾਹਰਲਾ))
	{
	    ਸਥਿਤੀ = RA_FAIL;
	    ਰੋਕੋ;
	}
#ਜੇ_ਅੰਤ
    }

    // May need to continue with the inner loop, starting at "scan".
    ਜੇ (ਸਥਿਤੀ == RA_CONT)
	ਜਾਰੀ;

    // If the regstack is empty or something failed we are done.
    ਜੇ (regstack.ga_len == 0 || ਸਥਿਤੀ == RA_FAIL)
    {
	ਜੇ (ਜਾਂਚ == ਨਿਰਾ)
	{
	    // We get here only if there's trouble -- normally "case END" is
	    // the terminating point.
	    iemsg(e_corrupted_regexp_ਪ੍ਰੋਗਰਾਮ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
	    ਲਿਖੋ("Premature EOL\n");
#ਜੇ_ਅੰਤ
	}
	ਵਾਪਸ (ਸਥਿਤੀ == RA_ਮੇਲ);
    }

  } // End of loop until the regstack is empty.

  // NOTREACHED
}

/*
 * regtry - try match of "prog" with at rex.line["col"].
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    ਸਥਿਰ ਲੰਮਾ
regtry(
    bt_regprog_T	*prog,
    colnr_T		ਖਾਨਾ,
    ਪੂਰਨ			*timed_ਬਾਹਰਲਾ)	// flag set on timeout or NULL
{
    rex.ਨਿਵੇਸ਼ = rex.ਸਤਰ + ਖਾਨਾ;
    rex.need_clear_subexpr = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    rex.need_clear_zsubexpr = (prog->reghasz == REX_ਸੈੱਟ);
#ਜੇ_ਅੰਤ

    ਜੇ (regmatch(prog->ਪ੍ਰੋਗਰਾਮ + 1, timed_ਬਾਹਰਲਾ) == 0)
	ਵਾਪਸ 0;

    cleanup_subexpr();
    ਜੇ (REG_MULTI)
    {
	ਜੇ (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].ਖਾਨਾ = ਖਾਨਾ;
	}
	ਜੇ (rex.reg_endpos[0].lnum < 0)
	{
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].ਖਾਨਾ = (ਪੂਰਨ)(rex.ਨਿਵੇਸ਼ - rex.ਸਤਰ);
	}
	ਨਹੀਂਤਾਂ
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (rex.reg_startp[0] == ਨਿਰਾ)
	    rex.reg_startp[0] = rex.ਸਤਰ + ਖਾਨਾ;
	ਜੇ (rex.reg_endp[0] == ਨਿਰਾ)
	    rex.reg_endp[0] = rex.ਨਿਵੇਸ਼;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_ਬਾਹਰਲਾ);
    re_extmatch_ਬਾਹਰਲਾ = ਨਿਰਾ;

    ਜੇ (prog->reghasz == REX_ਸੈੱਟ)
    {
	ਪੂਰਨ		i;

	cleanup_zsubexpr();
	re_extmatch_ਬਾਹਰਲਾ = make_extmatch();
	ਜੇ (re_extmatch_ਬਾਹਰਲਾ == ਨਿਰਾ)
	    ਵਾਪਸ 0;
	ਲਈ (i = 0; i < NSUBEXP; i++)
	{
	    ਜੇ (REG_MULTI)
	    {
		// Only accept single line matches.
		ਜੇ (reg_startzpos[i].lnum >= 0
			&& reg_endzpos[i].lnum == reg_startzpos[i].lnum
			&& reg_endzpos[i].ਖਾਨਾ >= reg_startzpos[i].ਖਾਨਾ)
		    re_extmatch_ਬਾਹਰਲਾ->matches[i] =
			vim_strnsave(reg_getline(reg_startzpos[i].lnum)
						       + reg_startzpos[i].ਖਾਨਾ,
				   reg_endzpos[i].ਖਾਨਾ - reg_startzpos[i].ਖਾਨਾ);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (reg_startzp[i] != ਨਿਰਾ && reg_endzp[i] != ਨਿਰਾ)
		    re_extmatch_ਬਾਹਰਲਾ->matches[i] =
			    vim_strnsave(reg_startzp[i],
						reg_endzp[i] - reg_startzp[i]);
	    }
	}
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    ਸਥਿਰ ਲੰਮਾ
bt_regexec_both(
    char_u	*ਸਤਰ,
    colnr_T	startcol,	// column to start looking for match
    ਪੂਰਨ		*timed_ਬਾਹਰਲਾ)	// flag set on timeout or NULL
{
    bt_regprog_T    *prog;
    char_u	    *s;
    colnr_T	    ਖਾਨਾ = startcol;
    ਲੰਮਾ	    retval = 0L;

    // Create "regstack" and "backpos" if they are not allocated yet.
    // We allocate *_INITIAL amount of bytes first and then set the grow size
    // to much bigger value to avoid many malloc calls in case of deep regular
    // expressions.
    ਜੇ (regstack.ga_ਅੰਕੜੇ == ਨਿਰਾ)
    {
	// Use an item size of 1 byte, since we push different things
	// onto the regstack.
	ga_init2(&regstack, 1, REGSTACK_INITIAL);
	(ਖਾਲੀ)ga_grow(&regstack, REGSTACK_INITIAL);
	regstack.ga_growsize = REGSTACK_INITIAL * 8;
    }

    ਜੇ (backpos.ga_ਅੰਕੜੇ == ਨਿਰਾ)
    {
	ga_init2(&backpos, ਆਕਾਰ(backpos_T), BACKPOS_INITIAL);
	(ਖਾਲੀ)ga_grow(&backpos, BACKPOS_INITIAL);
	backpos.ga_growsize = BACKPOS_INITIAL * 8;
    }

    ਜੇ (REG_MULTI)
    {
	prog = (bt_regprog_T *)rex.reg_mmatch->regprog;
	ਸਤਰ = reg_getline((linenr_T)0);
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    ਨਹੀਂਤਾਂ
    {
	prog = (bt_regprog_T *)rex.reg_ਮੇਲ->regprog;
	rex.reg_startp = rex.reg_ਮੇਲ->startp;
	rex.reg_endp = rex.reg_ਮੇਲ->endp;
    }

    // Be paranoid...
    ਜੇ (prog == ਨਿਰਾ || ਸਤਰ == ਨਿਰਾ)
    {
	iemsg(e_null_ਦਲੀਲ);
	ਜਾਓ theend;
    }

    // Check validity of program.
    ਜੇ (prog_magic_wrong())
	ਜਾਓ theend;

    // If the start column is past the maximum column: no need to try.
    ਜੇ (rex.reg_maxcol > 0 && ਖਾਨਾ >= rex.reg_maxcol)
	ਜਾਓ theend;

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    ਜੇ (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    ਨਹੀਂਤਾਂ ਜੇ (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    ਜੇ (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    // If there is a "must appear" string, look for it.
    ਜੇ (prog->regmust != ਨਿਰਾ)
    {
	ਪੂਰਨ c;

	ਜੇ (has_mbyte)
	    c = (*mb_ptr2char)(prog->regmust);
	ਨਹੀਂਤਾਂ
	    c = *prog->regmust;
	s = ਸਤਰ + ਖਾਨਾ;

	// This is used very often, esp. for ":global".  Use three versions of
	// the loop to avoid overhead of conditions.
	ਜੇ (!rex.reg_ic && !has_mbyte)
	    ਜਦੋਂ ((s = vim_strbyte(s, c)) != ਨਿਰਾ)
	    {
		ਪੂਰਨ len = prog->regmlen;
		ਜੇ (cstrncmp(s, prog->regmust, &len) == 0)
		    ਰੋਕੋ;		// Found it.
		++s;
	    }
	ਨਹੀਂਤਾਂ ਜੇ (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	    ਜਦੋਂ ((s = vim_strchr(s, c)) != ਨਿਰਾ)
	    {
		ਪੂਰਨ len = prog->regmlen;
		ਜੇ (cstrncmp(s, prog->regmust, &len) == 0)
		    ਰੋਕੋ;		// Found it.
		MB_PTR_ADV(s);
	    }
	ਨਹੀਂਤਾਂ
	    ਜਦੋਂ ((s = cstrchr(s, c)) != ਨਿਰਾ)
	    {
		ਪੂਰਨ len = prog->regmlen;
		ਜੇ (cstrncmp(s, prog->regmust, &len) == 0)
		    ਰੋਕੋ;		// Found it.
		MB_PTR_ADV(s);
	    }
	ਜੇ (s == ਨਿਰਾ)		// Not present.
	    ਜਾਓ theend;
    }

    rex.ਸਤਰ = ਸਤਰ;
    rex.lnum = 0;
    reg_toolong = FALSE;

    // Simplest case: Anchored match need be tried only once.
    ਜੇ (prog->reganch)
    {
	ਪੂਰਨ	c;

	ਜੇ (has_mbyte)
	    c = (*mb_ptr2char)(rex.ਸਤਰ + ਖਾਨਾ);
	ਨਹੀਂਤਾਂ
	    c = rex.ਸਤਰ[ਖਾਨਾ];
	ਜੇ (prog->regstart == NUL
		|| prog->regstart == c
		|| (rex.reg_ic
		    && (((enc_utf8 && utf_fold(prog->regstart) == utf_fold(c)))
			|| (c < 255 && prog->regstart < 255 &&
			    MB_TOLOWER(prog->regstart) == MB_TOLOWER(c)))))
	    retval = regtry(prog, ਖਾਨਾ, timed_ਬਾਹਰਲਾ);
	ਨਹੀਂਤਾਂ
	    retval = 0;
    }
    ਨਹੀਂਤਾਂ
    {
	// Messy cases:  unanchored match.
	ਜਦੋਂ (!got_int)
	{
	    ਜੇ (prog->regstart != NUL)
	    {
		// Skip until the char we know it must start with.
		// Used often, do some work to avoid call overhead.
		ਜੇ (!rex.reg_ic && !has_mbyte)
		    s = vim_strbyte(rex.ਸਤਰ + ਖਾਨਾ, prog->regstart);
		ਨਹੀਂਤਾਂ
		    s = cstrchr(rex.ਸਤਰ + ਖਾਨਾ, prog->regstart);
		ਜੇ (s == ਨਿਰਾ)
		{
		    retval = 0;
		    ਰੋਕੋ;
		}
		ਖਾਨਾ = (ਪੂਰਨ)(s - rex.ਸਤਰ);
	    }

	    // Check for maximum column to try.
	    ਜੇ (rex.reg_maxcol > 0 && ਖਾਨਾ >= rex.reg_maxcol)
	    {
		retval = 0;
		ਰੋਕੋ;
	    }

	    retval = regtry(prog, ਖਾਨਾ, timed_ਬਾਹਰਲਾ);
	    ਜੇ (retval > 0)
		ਰੋਕੋ;

	    // if not currently on the first line, get it again
	    ਜੇ (rex.lnum != 0)
	    {
		rex.lnum = 0;
		rex.ਸਤਰ = reg_getline((linenr_T)0);
	    }
	    ਜੇ (rex.ਸਤਰ[ਖਾਨਾ] == NUL)
		ਰੋਕੋ;
	    ਜੇ (has_mbyte)
		ਖਾਨਾ += (*mb_ptr2len)(rex.ਸਤਰ + ਖਾਨਾ);
	    ਨਹੀਂਤਾਂ
		++ਖਾਨਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
	    ਜੇ (bt_did_ਸਮਾਂ_ਬਾਹਰਲਾ(timed_ਬਾਹਰਲਾ))
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	}
    }

theend:
    // Free "reg_tofree" when it's a bit big.
    // Free regstack and backpos if they are bigger than their initial size.
    ਜੇ (reg_tofreelen > 400)
	VIM_CLEAR(reg_tofree);
    ਜੇ (regstack.ga_maxlen > REGSTACK_INITIAL)
	ga_clear(&regstack);
    ਜੇ (backpos.ga_maxlen > BACKPOS_INITIAL)
	ga_clear(&backpos);

    ਜੇ (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs
	// and \ze are used.
	ਜੇ (REG_MULTI)
	{
	    lpos_T *ਸ਼ੁਰੂ = &rex.reg_mmatch->startpos[0];
	    lpos_T *ਸਿਰਾ = &rex.reg_mmatch->endpos[0];

	    ਜੇ (ਸਿਰਾ->lnum < ਸ਼ੁਰੂ->lnum
			|| (ਸਿਰਾ->lnum == ਸ਼ੁਰੂ->lnum && ਸਿਰਾ->ਖਾਨਾ < ਸ਼ੁਰੂ->ਖਾਨਾ))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_mmatch->rmm_matchcol = ਖਾਨਾ;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (rex.reg_ਮੇਲ->endp[0] < rex.reg_ਮੇਲ->startp[0])
		rex.reg_ਮੇਲ->endp[0] = rex.reg_ਮੇਲ->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_ਮੇਲ->rm_matchcol = ਖਾਨਾ;
	}
    }

    ਵਾਪਸ retval;
}

/*
 * Match a regexp against a string.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 * if "line_lbr" is TRUE  consider a "\n" in "line" to be a line break.
 *
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    ਸਥਿਰ ਪੂਰਨ
bt_regexec_nl(
    regmatch_T	*rmp,
    char_u	*ਸਤਰ,	// string to match against
    colnr_T	ਖਾਨਾ,	// column to start looking for match
    ਪੂਰਨ		ਸਤਰ_lbr)
{
    rex.reg_ਮੇਲ = rmp;
    rex.reg_mmatch = ਨਿਰਾ;
    rex.reg_maxline = 0;
    rex.reg_ਸਤਰ_lbr = ਸਤਰ_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = ਨਿਰਾ;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;

    ਵਾਪਸ bt_regexec_both(ਸਤਰ, ਖਾਨਾ, ਨਿਰਾ);
}

/*
 * Match a regexp against multiple lines.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 *
 * Return zero if there is no match.  Return number of lines contained in the
 * match otherwise.
 */
    ਸਥਿਰ ਲੰਮਾ
bt_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	ਖਾਨਾ,		// column to start looking for match
    ਪੂਰਨ		*timed_ਬਾਹਰਲਾ)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    ਵਾਪਸ bt_regexec_both(ਨਿਰਾ, ਖਾਨਾ, timed_ਬਾਹਰਲਾ);
}

/*
 * Compare a number with the operand of RE_LNUM, RE_COL or RE_VCOL.
 */
    ਸਥਿਰ ਪੂਰਨ
re_ਅੰਕ_cmp(long_u val, char_u *ਜਾਂਚ)
{
    long_u  n = ਸੰਕਾਰਜ_MIN(ਜਾਂਚ);

    ਜੇ (ਸੰਕਾਰਜ_CMP(ਜਾਂਚ) == '>')
	ਵਾਪਸ val > n;
    ਜੇ (ਸੰਕਾਰਜ_CMP(ਜਾਂਚ) == '<')
	ਵਾਪਸ val < n;
    ਵਾਪਸ val == n;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BT_REGEXP_DUMP

/*
 * regdump - dump a regexp onto stdout in vaguely comprehensible form
 */
    ਸਥਿਰ ਖਾਲੀ
regdump(char_u *ਨਮੂਨਾ, bt_regprog_T *r)
{
    char_u  *s;
    ਪੂਰਨ	    op = EXACTLY;	// Arbitrary non-END op.
    char_u  *ਅਗਲਾ;
    char_u  *ਸਿਰਾ = ਨਿਰਾ;
    FILE    *f;

# ifdef BT_REGEXP_LOG
    f = fopen("bt_regexp_log.log", "a");
# ਨਹੀਂਤਾਂ
    f = stdout;
# endif
    ਜੇ (f == ਨਿਰਾ)
	ਵਾਪਸ;
    fprintf(f, "-------------------------------------\n\r\nregcomp(%s):\r\n", ਨਮੂਨਾ);

    s = r->ਪ੍ਰੋਗਰਾਮ + 1;
    // Loop until we find the END that isn't before a referred next (an END
    // can also appear in a NOMATCH operand).
    ਜਦੋਂ ((op != ਸਿਰਾ_3 || s <= ਸਿਰਾ) && s < r->ਪ੍ਰੋਗਰਾਮ + r->regsz)
    {
	op = OP(s);
	fprintf(f, "%2d%s", (ਪੂਰਨ)(s - r->ਪ੍ਰੋਗਰਾਮ), regprop(s)); // Where, what.
	ਅਗਲਾ = (s + 3 <= r->ਪ੍ਰੋਗਰਾਮ + r->regsz) ? regnext(s) : ਨਿਰਾ;
	ਜੇ (ਅਗਲਾ == ਨਿਰਾ)	// Next ptr.
	    fprintf(f, "(0)");
	ਨਹੀਂਤਾਂ
	    fprintf(f, "(%d)", (ਪੂਰਨ)((s - r->ਪ੍ਰੋਗਰਾਮ) + (ਅਗਲਾ - s)));
	ਜੇ (ਸਿਰਾ < ਅਗਲਾ)
	    ਸਿਰਾ = ਅਗਲਾ;
	ਜੇ (op == BRACE_LIMITS)
	{
	    // Two ints
	    fprintf(f, " minval %ld, maxval %ld", ਸੰਕਾਰਜ_MIN(s), ਸੰਕਾਰਜ_MAX(s));
	    s += 8;
	}
	ਨਹੀਂਤਾਂ ਜੇ (op == BEHIND || op == NOBEHIND)
	{
	    // one int
	    fprintf(f, " count %ld", ਸੰਕਾਰਜ_MIN(s));
	    s += 4;
	}
	ਨਹੀਂਤਾਂ ਜੇ (op == RE_LNUM || op == RE_ਖਾਨਾ || op == RE_VCOL)
	{
	    // one int plus comparator
	    fprintf(f, " count %ld", ਸੰਕਾਰਜ_MIN(s));
	    s += 5;
	}
	s += 3;
	ਜੇ (op == MULTIBYTECODE)
	{
	    fprintf(f, " mbc=%d", utf_ptr2char(s));
	    s += utfc_ptr2len(s);
	}
	ਜੇ (op == ANYOF || op == ANYOF + ਜੋੜੋ_NL
		|| op == ANYBUT || op == ANYBUT + ਜੋੜੋ_NL
		|| op == EXACTLY)
	{
	    // Literal string, where present.
	    fprintf(f, "\nxxxxxxxxx\n");
	    ਜਦੋਂ (*s != NUL && s < r->ਪ੍ਰੋਗਰਾਮ + r->regsz)
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
    ਜੇ (r->regstart != NUL)
	fprintf(f, "start `%s' 0x%x; ", r->regstart < 256
		? (ਅੱਖਰ *)transchar(r->regstart)
		: "multibyte", r->regstart);
    ਜੇ (r->reganch)
	fprintf(f, "anchored; ");
    ਜੇ (r->regmust != ਨਿਰਾ)
	fprintf(f, "must have \"%s\"", r->regmust);
    fprintf(f, "\r\n");

# ifdef BT_REGEXP_LOG
    fclose(f);
# endif
}
#ਜੇ_ਅੰਤ	    // BT_REGEXP_DUMP

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
/*
 * regprop - printable representation of opcode
 */
    ਸਥਿਰ char_u *
regprop(char_u *op)
{
    ਅੱਖਰ	    *p;
    ਸਥਿਰ ਅੱਖਰ	    buf[50];
    ਸਥਿਰ size_t   buflen = 0;

    STRCPY(buf, ":");
    buflen = 1;

    ਚੋਣ ((ਪੂਰਨ) OP(op))
    {
      ਹਾਲਤ BOL:
	p = "BOL";
	ਰੋਕੋ;
      ਹਾਲਤ EOL:
	p = "EOL";
	ਰੋਕੋ;
      ਹਾਲਤ RE_BOF:
	p = "BOF";
	ਰੋਕੋ;
      ਹਾਲਤ RE_EOF:
	p = "EOF";
	ਰੋਕੋ;
      ਹਾਲਤ CURSOR:
	p = "CURSOR";
	ਰੋਕੋ;
      ਹਾਲਤ RE_VISUAL:
	p = "RE_VISUAL";
	ਰੋਕੋ;
      ਹਾਲਤ RE_LNUM:
	p = "RE_LNUM";
	ਰੋਕੋ;
      ਹਾਲਤ RE_MARK:
	p = "RE_MARK";
	ਰੋਕੋ;
      ਹਾਲਤ RE_ਖਾਨਾ:
	p = "RE_COL";
	ਰੋਕੋ;
      ਹਾਲਤ RE_VCOL:
	p = "RE_VCOL";
	ਰੋਕੋ;
      ਹਾਲਤ BOW:
	p = "BOW";
	ਰੋਕੋ;
      ਹਾਲਤ EOW:
	p = "EOW";
	ਰੋਕੋ;
      ਹਾਲਤ ANY:
	p = "ANY";
	ਰੋਕੋ;
      ਹਾਲਤ ANY + ਜੋੜੋ_NL:
	p = "ANY+NL";
	ਰੋਕੋ;
      ਹਾਲਤ ANYOF:
	p = "ANYOF";
	ਰੋਕੋ;
      ਹਾਲਤ ANYOF + ਜੋੜੋ_NL:
	p = "ANYOF+NL";
	ਰੋਕੋ;
      ਹਾਲਤ ANYBUT:
	p = "ANYBUT";
	ਰੋਕੋ;
      ਹਾਲਤ ANYBUT + ਜੋੜੋ_NL:
	p = "ANYBUT+NL";
	ਰੋਕੋ;
      ਹਾਲਤ IDENT:
	p = "IDENT";
	ਰੋਕੋ;
      ਹਾਲਤ IDENT + ਜੋੜੋ_NL:
	p = "IDENT+NL";
	ਰੋਕੋ;
      ਹਾਲਤ SIDENT:
	p = "SIDENT";
	ਰੋਕੋ;
      ਹਾਲਤ SIDENT + ਜੋੜੋ_NL:
	p = "SIDENT+NL";
	ਰੋਕੋ;
      ਹਾਲਤ KWORD:
	p = "KWORD";
	ਰੋਕੋ;
      ਹਾਲਤ KWORD + ਜੋੜੋ_NL:
	p = "KWORD+NL";
	ਰੋਕੋ;
      ਹਾਲਤ SKWORD:
	p = "SKWORD";
	ਰੋਕੋ;
      ਹਾਲਤ SKWORD + ਜੋੜੋ_NL:
	p = "SKWORD+NL";
	ਰੋਕੋ;
      ਹਾਲਤ FNAME:
	p = "FNAME";
	ਰੋਕੋ;
      ਹਾਲਤ FNAME + ਜੋੜੋ_NL:
	p = "FNAME+NL";
	ਰੋਕੋ;
      ਹਾਲਤ SFNAME:
	p = "SFNAME";
	ਰੋਕੋ;
      ਹਾਲਤ SFNAME + ਜੋੜੋ_NL:
	p = "SFNAME+NL";
	ਰੋਕੋ;
      ਹਾਲਤ PRINT:
	p = "PRINT";
	ਰੋਕੋ;
      ਹਾਲਤ PRINT + ਜੋੜੋ_NL:
	p = "PRINT+NL";
	ਰੋਕੋ;
      ਹਾਲਤ SPRINT:
	p = "SPRINT";
	ਰੋਕੋ;
      ਹਾਲਤ SPRINT + ਜੋੜੋ_NL:
	p = "SPRINT+NL";
	ਰੋਕੋ;
      ਹਾਲਤ WHITE:
	p = "WHITE";
	ਰੋਕੋ;
      ਹਾਲਤ WHITE + ਜੋੜੋ_NL:
	p = "WHITE+NL";
	ਰੋਕੋ;
      ਹਾਲਤ NWHITE:
	p = "NWHITE";
	ਰੋਕੋ;
      ਹਾਲਤ NWHITE + ਜੋੜੋ_NL:
	p = "NWHITE+NL";
	ਰੋਕੋ;
      ਹਾਲਤ DIGIT:
	p = "DIGIT";
	ਰੋਕੋ;
      ਹਾਲਤ DIGIT + ਜੋੜੋ_NL:
	p = "DIGIT+NL";
	ਰੋਕੋ;
      ਹਾਲਤ NDIGIT:
	p = "NDIGIT";
	ਰੋਕੋ;
      ਹਾਲਤ NDIGIT + ਜੋੜੋ_NL:
	p = "NDIGIT+NL";
	ਰੋਕੋ;
      ਹਾਲਤ HEX:
	p = "HEX";
	ਰੋਕੋ;
      ਹਾਲਤ HEX + ਜੋੜੋ_NL:
	p = "HEX+NL";
	ਰੋਕੋ;
      ਹਾਲਤ NHEX:
	p = "NHEX";
	ਰੋਕੋ;
      ਹਾਲਤ NHEX + ਜੋੜੋ_NL:
	p = "NHEX+NL";
	ਰੋਕੋ;
      ਹਾਲਤ OCTAL:
	p = "OCTAL";
	ਰੋਕੋ;
      ਹਾਲਤ OCTAL + ਜੋੜੋ_NL:
	p = "OCTAL+NL";
	ਰੋਕੋ;
      ਹਾਲਤ NOCTAL:
	p = "NOCTAL";
	ਰੋਕੋ;
      ਹਾਲਤ NOCTAL + ਜੋੜੋ_NL:
	p = "NOCTAL+NL";
	ਰੋਕੋ;
      ਹਾਲਤ ਸ਼ਬਦ_2:
	p = "WORD";
	ਰੋਕੋ;
      ਹਾਲਤ ਸ਼ਬਦ_2 + ਜੋੜੋ_NL:
	p = "WORD+NL";
	ਰੋਕੋ;
      ਹਾਲਤ NWORD:
	p = "NWORD";
	ਰੋਕੋ;
      ਹਾਲਤ NWORD + ਜੋੜੋ_NL:
	p = "NWORD+NL";
	ਰੋਕੋ;
      ਹਾਲਤ ਸਿਰ_2:
	p = "HEAD";
	ਰੋਕੋ;
      ਹਾਲਤ ਸਿਰ_2 + ਜੋੜੋ_NL:
	p = "HEAD+NL";
	ਰੋਕੋ;
      ਹਾਲਤ NHEAD:
	p = "NHEAD";
	ਰੋਕੋ;
      ਹਾਲਤ NHEAD + ਜੋੜੋ_NL:
	p = "NHEAD+NL";
	ਰੋਕੋ;
      ਹਾਲਤ ALPHA:
	p = "ALPHA";
	ਰੋਕੋ;
      ਹਾਲਤ ALPHA + ਜੋੜੋ_NL:
	p = "ALPHA+NL";
	ਰੋਕੋ;
      ਹਾਲਤ NALPHA:
	p = "NALPHA";
	ਰੋਕੋ;
      ਹਾਲਤ NALPHA + ਜੋੜੋ_NL:
	p = "NALPHA+NL";
	ਰੋਕੋ;
      ਹਾਲਤ ਹੇਠਲਾ_2:
	p = "LOWER";
	ਰੋਕੋ;
      ਹਾਲਤ ਹੇਠਲਾ_2 + ਜੋੜੋ_NL:
	p = "LOWER+NL";
	ਰੋਕੋ;
      ਹਾਲਤ NLOWER:
	p = "NLOWER";
	ਰੋਕੋ;
      ਹਾਲਤ NLOWER + ਜੋੜੋ_NL:
	p = "NLOWER+NL";
	ਰੋਕੋ;
      ਹਾਲਤ ਉੱਤਲਾ:
	p = "UPPER";
	ਰੋਕੋ;
      ਹਾਲਤ ਉੱਤਲਾ + ਜੋੜੋ_NL:
	p = "UPPER+NL";
	ਰੋਕੋ;
      ਹਾਲਤ NUPPER:
	p = "NUPPER";
	ਰੋਕੋ;
      ਹਾਲਤ NUPPER + ਜੋੜੋ_NL:
	p = "NUPPER+NL";
	ਰੋਕੋ;
      ਹਾਲਤ BRANCH:
	p = "BRANCH";
	ਰੋਕੋ;
      ਹਾਲਤ EXACTLY:
	p = "EXACTLY";
	ਰੋਕੋ;
      ਹਾਲਤ NOTHING:
	p = "NOTHING";
	ਰੋਕੋ;
      ਹਾਲਤ ਪਿੱਠ_2:
	p = "BACK";
	ਰੋਕੋ;
      ਹਾਲਤ ਸਿਰਾ_3:
	p = "END";
	ਰੋਕੋ;
      ਹਾਲਤ MOPEN + 0:
	p = "MATCH START";
	ਰੋਕੋ;
      ਹਾਲਤ MOPEN + 1:
      ਹਾਲਤ MOPEN + 2:
      ਹਾਲਤ MOPEN + 3:
      ਹਾਲਤ MOPEN + 4:
      ਹਾਲਤ MOPEN + 5:
      ਹਾਲਤ MOPEN + 6:
      ਹਾਲਤ MOPEN + 7:
      ਹਾਲਤ MOPEN + 8:
      ਹਾਲਤ MOPEN + 9:
	buflen += vim_snprintf(buf + buflen, ਆਕਾਰ(buf) - buflen, "MOPEN%d", OP(op) - MOPEN);
	p = ਨਿਰਾ;
	ਰੋਕੋ;
      ਹਾਲਤ MCLOSE + 0:
	p = "MATCH END";
	ਰੋਕੋ;
      ਹਾਲਤ MCLOSE + 1:
      ਹਾਲਤ MCLOSE + 2:
      ਹਾਲਤ MCLOSE + 3:
      ਹਾਲਤ MCLOSE + 4:
      ਹਾਲਤ MCLOSE + 5:
      ਹਾਲਤ MCLOSE + 6:
      ਹਾਲਤ MCLOSE + 7:
      ਹਾਲਤ MCLOSE + 8:
      ਹਾਲਤ MCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, ਆਕਾਰ(buf) - buflen, "MCLOSE%d", OP(op) - MCLOSE);
	p = ਨਿਰਾ;
	ਰੋਕੋ;
      ਹਾਲਤ BACKREF + 1:
      ਹਾਲਤ BACKREF + 2:
      ਹਾਲਤ BACKREF + 3:
      ਹਾਲਤ BACKREF + 4:
      ਹਾਲਤ BACKREF + 5:
      ਹਾਲਤ BACKREF + 6:
      ਹਾਲਤ BACKREF + 7:
      ਹਾਲਤ BACKREF + 8:
      ਹਾਲਤ BACKREF + 9:
	buflen += vim_snprintf(buf + buflen, ਆਕਾਰ(buf) - buflen, "BACKREF%d", OP(op) - BACKREF);
	p = ਨਿਰਾ;
	ਰੋਕੋ;
      ਹਾਲਤ NOPEN:
	p = "NOPEN";
	ਰੋਕੋ;
      ਹਾਲਤ NCLOSE:
	p = "NCLOSE";
	ਰੋਕੋ;
# ifdef FEAT_SYN_HL
      ਹਾਲਤ ZOPEN + 1:
      ਹਾਲਤ ZOPEN + 2:
      ਹਾਲਤ ZOPEN + 3:
      ਹਾਲਤ ZOPEN + 4:
      ਹਾਲਤ ZOPEN + 5:
      ਹਾਲਤ ZOPEN + 6:
      ਹਾਲਤ ZOPEN + 7:
      ਹਾਲਤ ZOPEN + 8:
      ਹਾਲਤ ZOPEN + 9:
	buflen += vim_snprintf(buf + buflen, ਆਕਾਰ(buf) - buflen, "ZOPEN%d", OP(op) - ZOPEN);
	p = ਨਿਰਾ;
	ਰੋਕੋ;
      ਹਾਲਤ ZCLOSE + 1:
      ਹਾਲਤ ZCLOSE + 2:
      ਹਾਲਤ ZCLOSE + 3:
      ਹਾਲਤ ZCLOSE + 4:
      ਹਾਲਤ ZCLOSE + 5:
      ਹਾਲਤ ZCLOSE + 6:
      ਹਾਲਤ ZCLOSE + 7:
      ਹਾਲਤ ZCLOSE + 8:
      ਹਾਲਤ ZCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, ਆਕਾਰ(buf) - buflen, "ZCLOSE%d", OP(op) - ZCLOSE);
	p = ਨਿਰਾ;
	ਰੋਕੋ;
      ਹਾਲਤ ZREF + 1:
      ਹਾਲਤ ZREF + 2:
      ਹਾਲਤ ZREF + 3:
      ਹਾਲਤ ZREF + 4:
      ਹਾਲਤ ZREF + 5:
      ਹਾਲਤ ZREF + 6:
      ਹਾਲਤ ZREF + 7:
      ਹਾਲਤ ZREF + 8:
      ਹਾਲਤ ZREF + 9:
	buflen += vim_snprintf(buf + buflen, ਆਕਾਰ(buf) - buflen, "ZREF%d", OP(op) - ZREF);
	p = ਨਿਰਾ;
	ਰੋਕੋ;
# endif
      ਹਾਲਤ ਤਾਰਾ:
	p = "STAR";
	ਰੋਕੋ;
      ਹਾਲਤ PLUS:
	p = "PLUS";
	ਰੋਕੋ;
      ਹਾਲਤ NOMATCH:
	p = "NOMATCH";
	ਰੋਕੋ;
      ਹਾਲਤ ਮੇਲ_2:
	p = "MATCH";
	ਰੋਕੋ;
      ਹਾਲਤ BEHIND:
	p = "BEHIND";
	ਰੋਕੋ;
      ਹਾਲਤ NOBEHIND:
	p = "NOBEHIND";
	ਰੋਕੋ;
      ਹਾਲਤ SUBPAT:
	p = "SUBPAT";
	ਰੋਕੋ;
      ਹਾਲਤ BRACE_LIMITS:
	p = "BRACE_LIMITS";
	ਰੋਕੋ;
      ਹਾਲਤ BRACE_SIMPLE:
	p = "BRACE_SIMPLE";
	ਰੋਕੋ;
      ਹਾਲਤ BRACE_COMPLEX + 0:
      ਹਾਲਤ BRACE_COMPLEX + 1:
      ਹਾਲਤ BRACE_COMPLEX + 2:
      ਹਾਲਤ BRACE_COMPLEX + 3:
      ਹਾਲਤ BRACE_COMPLEX + 4:
      ਹਾਲਤ BRACE_COMPLEX + 5:
      ਹਾਲਤ BRACE_COMPLEX + 6:
      ਹਾਲਤ BRACE_COMPLEX + 7:
      ਹਾਲਤ BRACE_COMPLEX + 8:
      ਹਾਲਤ BRACE_COMPLEX + 9:
	buflen += vim_snprintf(buf + buflen, ਆਕਾਰ(buf) - buflen, "BRACE_COMPLEX%d", OP(op) - BRACE_COMPLEX);
	p = ਨਿਰਾ;
	ਰੋਕੋ;
      ਹਾਲਤ MULTIBYTECODE:
	p = "MULTIBYTECODE";
	ਰੋਕੋ;
      ਹਾਲਤ NEWL:
	p = "NEWL";
	ਰੋਕੋ;
      ਮੂਲ:
	buflen += vim_snprintf(buf + buflen, ਆਕਾਰ(buf) - buflen, "corrupt %d", OP(op));
	p = ਨਿਰਾ;
	ਰੋਕੋ;
    }
    ਜੇ (p != ਨਿਰਾ)
	vim_strncpy((char_u *)buf + buflen, (char_u *)p, ਆਕਾਰ(buf) - buflen - 1);
    ਵਾਪਸ (char_u *)buf;
}
#ਜੇ_ਅੰਤ	    // DEBUG
