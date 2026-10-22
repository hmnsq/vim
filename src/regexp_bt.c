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
#ವ್ಯಾಖ್ಯಾನಿಸು ಕೊನೆ_3		0	//	End of program or NOMATCH operand.
#ವ್ಯಾಖ್ಯಾನಿಸು BOL		1	//	Match "" at beginning of line.
#ವ್ಯಾಖ್ಯಾನಿಸು EOL		2	//	Match "" at end of line.
#ವ್ಯಾಖ್ಯಾನಿಸು BRANCH		3	// node Match this alternative, or the
				//	next...
#ವ್ಯಾಖ್ಯಾನಿಸು ಹಿಂಭಾಗ_2		4	//	Match "", "next" ptr points backward.
#ವ್ಯಾಖ್ಯಾನಿಸು EXACTLY		5	// str	Match this string.
#ವ್ಯಾಖ್ಯಾನಿಸು NOTHING		6	//	Match empty string.
#ವ್ಯಾಖ್ಯಾನಿಸು ನಕ್ಷತ್ರ		7	// node Match this (simple) thing 0 or more
				//	times.
#ವ್ಯಾಖ್ಯಾನಿಸು PLUS		8	// node Match this (simple) thing 1 or more
				//	times.
#ವ್ಯಾಖ್ಯಾನಿಸು ಹೊಂದಿಕೆ_2		9	// node match the operand zero-width
#ವ್ಯಾಖ್ಯಾನಿಸು NOMATCH		10	// node check for no match with operand
#ವ್ಯಾಖ್ಯಾನಿಸು BEHIND		11	// node look behind for a match with operand
#ವ್ಯಾಖ್ಯಾನಿಸು NOBEHIND	12	// node look behind for no match with operand
#ವ್ಯಾಖ್ಯಾನಿಸು SUBPAT		13	// node match the operand here
#ವ್ಯಾಖ್ಯಾನಿಸು BRACE_SIMPLE	14	// node Match this (simple) thing between m and
				//	n times (\{m,n\}).
#ವ್ಯಾಖ್ಯಾನಿಸು BOW		15	//	Match "" after [^a-zA-Z0-9_]
#ವ್ಯಾಖ್ಯಾನಿಸು EOW		16	//	Match "" at    [^a-zA-Z0-9_]
#ವ್ಯಾಖ್ಯಾನಿಸು BRACE_LIMITS	17	// nr nr  define the min & max for BRACE_SIMPLE
				//	and BRACE_COMPLEX.
#ವ್ಯಾಖ್ಯಾನಿಸು NEWL		18	//	Match line-break
#ವ್ಯಾಖ್ಯಾನಿಸು BHPOS		19	//	End position for BEHIND or NOBEHIND


// character classes: 20-48 normal, 50-78 include a line-break
#ವ್ಯಾಖ್ಯಾನಿಸು ಕೂಡಿಸು_NL		30
#ವ್ಯಾಖ್ಯಾನಿಸು ಮೊದಲ_NL	ANY + ಕೂಡಿಸು_NL
#ವ್ಯಾಖ್ಯಾನಿಸು ANY		20	//	Match any one character.
#ವ್ಯಾಖ್ಯಾನಿಸು ANYOF		21	// str	Match any character in this string.
#ವ್ಯಾಖ್ಯಾನಿಸು ANYBUT		22	// str	Match any character not in this
				//	string.
#ವ್ಯಾಖ್ಯಾನಿಸು IDENT		23	//	Match identifier char
#ವ್ಯಾಖ್ಯಾನಿಸು SIDENT		24	//	Match identifier char but no digit
#ವ್ಯಾಖ್ಯಾನಿಸು KWORD		25	//	Match keyword char
#ವ್ಯಾಖ್ಯಾನಿಸು SKWORD		26	//	Match word char but no digit
#ವ್ಯಾಖ್ಯಾನಿಸು FNAME		27	//	Match file name char
#ವ್ಯಾಖ್ಯಾನಿಸು SFNAME		28	//	Match file name char but no digit
#ವ್ಯಾಖ್ಯಾನಿಸು PRINT		29	//	Match printable char
#ವ್ಯಾಖ್ಯಾನಿಸು SPRINT		30	//	Match printable char but no digit
#ವ್ಯಾಖ್ಯಾನಿಸು WHITE		31	//	Match whitespace char
#ವ್ಯಾಖ್ಯಾನಿಸು NWHITE		32	//	Match non-whitespace char
#ವ್ಯಾಖ್ಯಾನಿಸು DIGIT		33	//	Match digit char
#ವ್ಯಾಖ್ಯಾನಿಸು NDIGIT		34	//	Match non-digit char
#ವ್ಯಾಖ್ಯಾನಿಸು HEX		35	//	Match hex char
#ವ್ಯಾಖ್ಯಾನಿಸು NHEX		36	//	Match non-hex char
#ವ್ಯಾಖ್ಯಾನಿಸು OCTAL		37	//	Match octal char
#ವ್ಯಾಖ್ಯಾನಿಸು NOCTAL		38	//	Match non-octal char
#ವ್ಯಾಖ್ಯಾನಿಸು ಶಬ್ದ_2		39	//	Match word char
#ವ್ಯಾಖ್ಯಾನಿಸು NWORD		40	//	Match non-word char
#ವ್ಯಾಖ್ಯಾನಿಸು ತಲೆ_2		41	//	Match head char
#ವ್ಯಾಖ್ಯಾನಿಸು NHEAD		42	//	Match non-head char
#ವ್ಯಾಖ್ಯಾನಿಸು ALPHA		43	//	Match alpha char
#ವ್ಯಾಖ್ಯಾನಿಸು NALPHA		44	//	Match non-alpha char
#ವ್ಯಾಖ್ಯಾನಿಸು ಅಧೋ_2		45	//	Match lowercase char
#ವ್ಯಾಖ್ಯಾನಿಸು NLOWER		46	//	Match non-lowercase char
#ವ್ಯಾಖ್ಯಾನಿಸು ಊರ್ಧ್ವ		47	//	Match uppercase char
#ವ್ಯಾಖ್ಯಾನಿಸು NUPPER		48	//	Match non-uppercase char
#ವ್ಯಾಖ್ಯಾನಿಸು ಕೊನೆಯ_NL		NUPPER + ಕೂಡಿಸು_NL
#ವ್ಯಾಖ್ಯಾನಿಸು WITH_NL(op)	((op) >= ಮೊದಲ_NL && (op) <= ಕೊನೆಯ_NL)

#ವ್ಯಾಖ್ಯಾನಿಸು MOPEN		80  // -89	 Mark this point in input as start of
				//	 \( subexpr.  MOPEN + 0 marks start of
				//	 match.
#ವ್ಯಾಖ್ಯಾನಿಸು MCLOSE		90  // -99	 Analogous to MOPEN.  MCLOSE + 0 marks
				//	 end of match.
#ವ್ಯಾಖ್ಯಾನಿಸು BACKREF		100 // -109 node Match same string again \1-\9

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
# define ZOPEN		110 // -119	 Mark this point in input as start of
				//	 \z( subexpr.
# define ZCLOSE		120 // -129	 Analogous to ZOPEN.
# define ZREF		130 // -139 node Match external submatch \z1-\z9
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು BRACE_COMPLEX	140 // -149 node Match nodes between m & n times

#ವ್ಯಾಖ್ಯಾನಿಸು NOPEN		150	//	Mark this point in input as start of
				//	\%( subexpr.
#ವ್ಯಾಖ್ಯಾನಿಸು NCLOSE		151	//	Analogous to NOPEN.

#ವ್ಯಾಖ್ಯಾನಿಸು MULTIBYTECODE	200	// mbc	Match one multi-byte character
#ವ್ಯಾಖ್ಯಾನಿಸು RE_BOF		201	//	Match "" at beginning of file.
#ವ್ಯಾಖ್ಯಾನಿಸು RE_EOF		202	//	Match "" at end of file.
#ವ್ಯಾಖ್ಯಾನಿಸು CURSOR		203	//	Match location of cursor.

#ವ್ಯಾಖ್ಯಾನಿಸು RE_LNUM		204	// nr cmp  Match line number
#ವ್ಯಾಖ್ಯಾನಿಸು RE_ಲಂಬಸಾಲು		205	// nr cmp  Match column number
#ವ್ಯಾಖ್ಯಾನಿಸು RE_VCOL		206	// nr cmp  Match virtual column number

#ವ್ಯಾಖ್ಯಾನಿಸು RE_MARK		207	// mark cmp  Match mark position
#ವ್ಯಾಖ್ಯಾನಿಸು RE_VISUAL	208	//	Match Visual area
#ವ್ಯಾಖ್ಯಾನಿಸು RE_COMPOSING	209	// any composing characters

/*
 * Flags to be passed up and down.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು HASWIDTH	0x1	// Known never to match null string.
#ವ್ಯಾಖ್ಯಾನಿಸು SIMPLE		0x2	// Simple enough to be STAR/PLUS operand.
#ವ್ಯಾಖ್ಯಾನಿಸು SPSTART		0x4	// Starts with * or +.
#ವ್ಯಾಖ್ಯಾನಿಸು HASNL		0x8	// Contains some \n.
#ವ್ಯಾಖ್ಯಾನಿಸು HASLOOKBH	0x10	// Contains "\@<=" or "\@<!".
#ವ್ಯಾಖ್ಯಾನಿಸು WORST		0	// Worst case.

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಅಂಕೆ_complex_braces; // Complex \{...} count
ಸ್ಥಿರ char_u	*regcode;	// Code-emit pointer, or JUST_CALC_SIZE
ಸ್ಥಿರ ದೀರ್ಘ	regsize;	// Code size.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	reg_toolong;	// TRUE when offset out of range
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	bt_reg_parse_ಆಳ;	// nesting depth in reg()
ಸ್ಥಿರ char_u	had_endbrace[NSUBEXP];	// flags, TRUE if end of () found
ಸ್ಥಿರ ದೀರ್ಘ	brace_min[10];	// Minimums for complex brace repeats
ಸ್ಥಿರ ದೀರ್ಘ	brace_max[10];	// Maximums for complex brace repeats
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	brace_ಎಣಿಕೆ[10]; // Current counts for complex brace repeats
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	one_exactly = FALSE;	// only do one char for EXACTLY

// When making changes to classchars also change nfa_classcodes.
ಸ್ಥಿರ char_u	*classchars = (char_u *)".iIkKfFpPsSdDxXoOwWhHaAlLuU";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ classcodes[] = {
    ANY, IDENT, SIDENT, KWORD, SKWORD,
    FNAME, SFNAME, PRINT, SPRINT,
    WHITE, NWHITE, DIGIT, NDIGIT,
    HEX, NHEX, OCTAL, NOCTAL,
    ಶಬ್ದ_2, NWORD, ತಲೆ_2, NHEAD,
    ALPHA, NALPHA, ಅಧೋ_2, NLOWER,
    ಊರ್ಧ್ವ, NUPPER
};

/*
 * Search between "start" (the first char of the range) and "end" (the closing
 * "]") and try to recognize a character class in expanded form, for example
 * [0-9].  On success, return the atom to be emitted, on failure 0.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
bt_recognize_char_class(char_u *ಆರಂಭ, char_u *ಕೊನೆ)
{
    ಪೂರ್ಣಾಂಕ		newl;
    ಪೂರ್ಣಾಂಕ		config = get_char_class_ಬಿಟ್ಟುಗಳು(ಆರಂಭ, ಕೊನೆ, &newl);

    // A newline would need the ADD_NL variant, don't bother with it here.
    ಆದರೆ (config < 0 || newl)
	ಹಿಂತಿರುಗಿ 0;

	// The old engine has no case-insensitive class opcode, so [a-z] and [A-Z]
	// are left as collections.  The classes below are case-independent.
    ಆಯ್ಕೆ (config)
    {
	ಸ್ಥಿತಿ CLASS_o9:
	    ಹಿಂತಿರುಗಿ DIGIT;
	ಸ್ಥಿತಿ CLASS_not | CLASS_o9:
	    ಹಿಂತಿರುಗಿ NDIGIT;
	ಸ್ಥಿತಿ CLASS_af | CLASS_AF | CLASS_o9:
	    ಹಿಂತಿರುಗಿ HEX;
	ಸ್ಥಿತಿ CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    ಹಿಂತಿರುಗಿ NHEX;
	ಸ್ಥಿತಿ CLASS_o7:
	    ಹಿಂತಿರುಗಿ OCTAL;
	ಸ್ಥಿತಿ CLASS_not | CLASS_o7:
	    ಹಿಂತಿರುಗಿ NOCTAL;
	ಸ್ಥಿತಿ CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    ಹಿಂತಿರುಗಿ ಶಬ್ದ_2;
	ಸ್ಥಿತಿ CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    ಹಿಂತಿರುಗಿ NWORD;
	ಸ್ಥಿತಿ CLASS_az | CLASS_AZ | CLASS_underscore:
	    ಹಿಂತಿರುಗಿ ತಲೆ_2;
	ಸ್ಥಿತಿ CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    ಹಿಂತಿರುಗಿ NHEAD;
	ಸ್ಥಿತಿ CLASS_az | CLASS_AZ:
	    ಹಿಂತಿರುಗಿ ALPHA;
	ಸ್ಥಿತಿ CLASS_not | CLASS_az | CLASS_AZ:
	    ಹಿಂತಿರುಗಿ NALPHA;
    }
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * When regcode is set to this value, code is not emitted and size is computed
 * instead.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು JUST_CALC_ಗಾತ್ರ	((char_u *) -1)

// Values for rs_state in regitem_T.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ regstate_E
{
    RS_NOPEN = 0	// NOPEN and NCLOSE
    , RS_MOPEN		// MOPEN + [0-9]
    , RS_MCLOSE		// MCLOSE + [0-9]
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    , RS_ZOPEN		// ZOPEN + [0-9]
    , RS_ZCLOSE		// ZCLOSE + [0-9]
#ಆದರೆ_ಅಂತ್ಯ
    , RS_BRANCH		// BRANCH
    , RS_BRCPLX_ಇನ್ನಷ್ಟು	// BRACE_COMPLEX and trying one more match
    , RS_BRCPLX_LONG	// BRACE_COMPLEX and trying longest match
    , RS_BRCPLX_SHORT	// BRACE_COMPLEX and trying shortest match
    , RS_NOMATCH	// NOMATCH
    , RS_BEHIND1	// BEHIND / NOBEHIND matching rest
    , RS_BEHIND2	// BEHIND / NOBEHIND matching behind part
    , RS_ನಕ್ಷತ್ರ_LONG	// STAR/PLUS/BRACE_SIMPLE longest match
    , RS_ನಕ್ಷತ್ರ_SHORT	// STAR/PLUS/BRACE_SIMPLE shortest match
} regstate_T;

/*
 * Structure used to save the current input state, when it needs to be
 * restored after trying a match.  Used by reg_save() and reg_restore().
 * Also stores the length of "backpos".
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಒಕ್ಕೂಟ
    {
	char_u	*ptr;	// rex.input pointer, for single-line regexp
	lpos_T	pos;	// rex.input pos, for multi-line regexp
    } rs_u;
    ಪೂರ್ಣಾಂಕ		rs_len;
} regsave_T;

// struct to save start/end pointer/position in for \(\)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಒಕ್ಕೂಟ
    {
	char_u	*ptr;
	lpos_T	pos;
    } se_u;
} save_se_T;

// used for BEHIND and NOBEHIND matching
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ regbehind_S
{
    regsave_T	save_after;
    regsave_T	save_behind;
    ಪೂರ್ಣಾಂಕ		save_need_clear_subexpr;
    save_se_T   save_ಆರಂಭ[NSUBEXP];
    save_se_T   save_ಕೊನೆ[NSUBEXP];
} regbehind_T;

/*
 * When there are alternatives a regstate_T is put on the regstack to remember
 * what we are doing.
 * Before it may be another type of item, depending on rs_state, to remember
 * more things.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ regitem_S
{
    regstate_T	rs_state;	// what we are doing, one of RS_ above
    ಗಿಡ್ಡ	rs_no;		// submatch nr or BEHIND/NOBEHIND
    char_u	*rs_ಪರಿಶೀಲನೆ;	// current node in program
    ಒಕ್ಕೂಟ
    {
	save_se_T  sesave;
	regsave_T  regsave;
    } rs_un;			// room for saving rex.input
} regitem_T;


// used for STAR, PLUS and BRACE_SIMPLE matching
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ regstar_S
{
    ಪೂರ್ಣಾಂಕ		nextb;		// next byte
    ಪೂರ್ಣಾಂಕ		nextb_ic;	// next byte reverse case
    ದೀರ್ಘ	ಎಣಿಕೆ_2;
    ದೀರ್ಘ	minval;
    ದೀರ್ಘ	maxval;
} regstar_T;

// used to store input position when a BACK was encountered, so that we now if
// we made any progress since the last time.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ backpos_S
{
    char_u	*bp_ಪರಿಶೀಲನೆ;	// "scan" where BACK was encountered
    regsave_T	bp_pos;		// last input position
} backpos_T;

/*
 * "regstack" and "backpos" are used by regmatch().  They are kept over calls
 * to avoid invoking malloc() and free() often.
 * "regstack" is a stack with regitem_T items, sometimes preceded by regstar_T
 * or regbehind_T.
 * "backpos_T" is a table with backpos_T for BACK
 */
ಸ್ಥಿರ garray_T	regstack = {0, 0, 0, 0, ಖಾಲಿ};
ಸ್ಥಿರ garray_T	backpos = {0, 0, 0, 0, ಖಾಲಿ};

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
#ವ್ಯಾಖ್ಯಾನಿಸು REGSTACK_INITIAL	2048
#ವ್ಯಾಖ್ಯಾನಿಸು BACKPOS_INITIAL		64

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
#ವ್ಯಾಖ್ಯಾನಿಸು OP(p)		((ಪೂರ್ಣಾಂಕ)*(p))
#ವ್ಯಾಖ್ಯಾನಿಸು ಮುಂದಿನ_2(p)		(((*((p) + 1) & 0377) << 8) + (*((p) + 2) & 0377))
#ವ್ಯಾಖ್ಯಾನಿಸು ಕಾರ್ಯಾಂಶ(p)	((p) + 3)
// Obtain an operand that was stored as four bytes, MSB first.
#ವ್ಯಾಖ್ಯಾನಿಸು ಕಾರ್ಯಾಂಶ_MIN(p)	(((ದೀರ್ಘ)(p)[3] << 24) + ((ದೀರ್ಘ)(p)[4] << 16) \
			+ ((ದೀರ್ಘ)(p)[5] << 8) + (ದೀರ್ಘ)(p)[6])
// Obtain a second operand stored as four bytes.
#ವ್ಯಾಖ್ಯಾನಿಸು ಕಾರ್ಯಾಂಶ_MAX(p)	ಕಾರ್ಯಾಂಶ_MIN((p) + 4)
// Obtain a second single-byte operand stored after a four bytes operand.
#ವ್ಯಾಖ್ಯಾನಿಸು ಕಾರ್ಯಾಂಶ_CMP(p)	(p)[7]

ಸ್ಥಿರ char_u *reg(ಪೂರ್ಣಾಂಕ paren, ಪೂರ್ಣಾಂಕ *flagp);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BT_REGEXP_DUMP
ಸ್ಥಿರ ಶೂನ್ಯ	regdump(char_u *, bt_regprog_T *);
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	re_ಅಂಕೆ_cmp(long_u val, char_u *ಪರಿಶೀಲನೆ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
ಸ್ಥಿರ char_u	*regprop(char_u *);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	regnarrate = 0;
#ಆದರೆ_ಅಂತ್ಯ


/*
 * Setup to parse the regexp.  Used once to get the length and once to do it.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
regcomp_ಆರಂಭ(
    char_u	*ಅಭಿವ್ಯಕ್ತಿ,
    ಪೂರ್ಣಾಂಕ		re_flags)	    // see vim_regcomp()
{
    initchr(ಅಭಿವ್ಯಕ್ತಿ);
    ಆದರೆ (re_flags & RE_MAGIC)
	reg_magic = MAGIC_ON;
    ಇಲ್ಲದಿದ್ದರೆ
	reg_magic = MAGIC_OFF;
    reg_string = (re_flags & RE_STRING);
    reg_strict = (re_flags & RE_STRICT);
    get_cpo_flags();

    ಅಂಕೆ_complex_braces = 0;
    regnpar = 1;
    CLEAR_ಕ್ಷೇತ್ರ(had_endbrace);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    regnzpar = 1;
    re_has_z = 0;
#ಆದರೆ_ಅಂತ್ಯ
    regsize = 0L;
    reg_toolong = FALSE;
    bt_reg_parse_ಆಳ = 0;
    regflags = 0;
#ಆದರೆ defined(FEAT_SYN_HL)
    had_eol = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Return TRUE if MULTIBYTECODE should be used instead of EXACTLY for
 * character "c".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
use_multibytecode(ಪೂರ್ಣಾಂಕ c)
{
    ಹಿಂತಿರುಗಿ has_mbyte && (*mb_char2len)(c) > 1
		     && (re_multi_type(peekchr()) != NOT_MULTI
			     || (enc_utf8 && utf_iscomposing(c)));
}

/*
 * Emit (if appropriate) a byte of code
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
regc(ಪೂರ್ಣಾಂಕ b)
{
    ಆದರೆ (regcode == JUST_CALC_ಗಾತ್ರ)
	regsize++;
    ಇಲ್ಲದಿದ್ದರೆ
	*regcode++ = b;
}

/*
 * Emit (if appropriate) a multi-byte character of code
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
regmbc(ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (!has_mbyte && c > 0xff)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (regcode == JUST_CALC_ಗಾತ್ರ)
	regsize += (*mb_char2len)(c);
    ಇಲ್ಲದಿದ್ದರೆ
	regcode += (*mb_char2bytes)(c, regcode);
}


/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * NOTE: When changing this function, also change nfa_emit_equi_class()
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
reg_equi_class(ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (enc_utf8 || STRCMP(p_enc, "latin1") == 0
					 || STRCMP(p_enc, "iso-8859-15") == 0)
    {
	ಆಯ್ಕೆ (c)
	{
	    // Do not use '\300' style, it results in a negative number.
	    ಸ್ಥಿತಿ 'A': ಸ್ಥಿತಿ 0xc0: ಸ್ಥಿತಿ 0xc1: ಸ್ಥಿತಿ 0xc2: ಸ್ಥಿತಿ 0xc3: ಸ್ಥಿತಿ 0xc4:
	    ಸ್ಥಿತಿ 0xc5: ಸ್ಥಿತಿ 0x100: ಸ್ಥಿತಿ 0x102: ಸ್ಥಿತಿ 0x104: ಸ್ಥಿತಿ 0x1cd:
	    ಸ್ಥಿತಿ 0x1de: ಸ್ಥಿತಿ 0x1e0: ಸ್ಥಿತಿ 0x1fa: ಸ್ಥಿತಿ 0x202: ಸ್ಥಿತಿ 0x226:
	    ಸ್ಥಿತಿ 0x23a: ಸ್ಥಿತಿ 0x1e00: ಸ್ಥಿತಿ 0x1ea0: ಸ್ಥಿತಿ 0x1ea2: ಸ್ಥಿತಿ 0x1ea4:
	    ಸ್ಥಿತಿ 0x1ea6: ಸ್ಥಿತಿ 0x1ea8: ಸ್ಥಿತಿ 0x1eaa: ಸ್ಥಿತಿ 0x1eac: ಸ್ಥಿತಿ 0x1eae:
	    ಸ್ಥಿತಿ 0x1eb0: ಸ್ಥಿತಿ 0x1eb2: ಸ್ಥಿತಿ 0x1eb4: ಸ್ಥಿತಿ 0x1eb6:
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
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'B': ಸ್ಥಿತಿ 0x181: ಸ್ಥಿತಿ 0x243: ಸ್ಥಿತಿ 0x1e02:
	    ಸ್ಥಿತಿ 0x1e04: ಸ್ಥಿತಿ 0x1e06:
		      regmbc('B');
		      regmbc(0x181); regmbc(0x243); regmbc(0x1e02);
		      regmbc(0x1e04); regmbc(0x1e06);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'C': ಸ್ಥಿತಿ 0xc7:
	    ಸ್ಥಿತಿ 0x106: ಸ್ಥಿತಿ 0x108: ಸ್ಥಿತಿ 0x10a: ಸ್ಥಿತಿ 0x10c: ಸ್ಥಿತಿ 0x187:
	    ಸ್ಥಿತಿ 0x23b: ಸ್ಥಿತಿ 0x1e08: ಸ್ಥಿತಿ 0xa792:
		      regmbc('C'); regmbc(0xc7);
		      regmbc(0x106); regmbc(0x108); regmbc(0x10a);
		      regmbc(0x10c); regmbc(0x187); regmbc(0x23b);
		      regmbc(0x1e08); regmbc(0xa792);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'D': ಸ್ಥಿತಿ 0x10e: ಸ್ಥಿತಿ 0x110: ಸ್ಥಿತಿ 0x18a:
	    ಸ್ಥಿತಿ 0x1e0a: ಸ್ಥಿತಿ 0x1e0c: ಸ್ಥಿತಿ 0x1e0e: ಸ್ಥಿತಿ 0x1e10:
	    ಸ್ಥಿತಿ 0x1e12:
		      regmbc('D'); regmbc(0x10e); regmbc(0x110);
		      regmbc(0x18a); regmbc(0x1e0a); regmbc(0x1e0c);
		      regmbc(0x1e0e); regmbc(0x1e10); regmbc(0x1e12);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'E': ಸ್ಥಿತಿ 0xc8: ಸ್ಥಿತಿ 0xc9: ಸ್ಥಿತಿ 0xca: ಸ್ಥಿತಿ 0xcb:
	    ಸ್ಥಿತಿ 0x112: ಸ್ಥಿತಿ 0x114: ಸ್ಥಿತಿ 0x116: ಸ್ಥಿತಿ 0x118: ಸ್ಥಿತಿ 0x11a:
	    ಸ್ಥಿತಿ 0x204: ಸ್ಥಿತಿ 0x206: ಸ್ಥಿತಿ 0x228: ಸ್ಥಿತಿ 0x246: ಸ್ಥಿತಿ 0x1e14:
	    ಸ್ಥಿತಿ 0x1e16: ಸ್ಥಿತಿ 0x1e18: ಸ್ಥಿತಿ 0x1e1a: ಸ್ಥಿತಿ 0x1e1c:
	    ಸ್ಥಿತಿ 0x1eb8: ಸ್ಥಿತಿ 0x1eba: ಸ್ಥಿತಿ 0x1ebc: ಸ್ಥಿತಿ 0x1ebe:
	    ಸ್ಥಿತಿ 0x1ec0: ಸ್ಥಿತಿ 0x1ec2: ಸ್ಥಿತಿ 0x1ec4: ಸ್ಥಿತಿ 0x1ec6:
		      regmbc('E'); regmbc(0xc8); regmbc(0xc9);
		      regmbc(0xca); regmbc(0xcb); regmbc(0x112);
		      regmbc(0x114); regmbc(0x116); regmbc(0x118);
		      regmbc(0x11a); regmbc(0x204); regmbc(0x206);
		      regmbc(0x228); regmbc(0x246); regmbc(0x1e14);
		      regmbc(0x1e16); regmbc(0x1e18); regmbc(0x1e1a);
		      regmbc(0x1e1c); regmbc(0x1eb8); regmbc(0x1eba);
		      regmbc(0x1ebc); regmbc(0x1ebe); regmbc(0x1ec0);
		      regmbc(0x1ec2); regmbc(0x1ec4); regmbc(0x1ec6);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'F': ಸ್ಥಿತಿ 0x191: ಸ್ಥಿತಿ 0x1e1e: ಸ್ಥಿತಿ 0xa798:
		      regmbc('F'); regmbc(0x191); regmbc(0x1e1e);
		      regmbc(0xa798);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'G': ಸ್ಥಿತಿ 0x11c: ಸ್ಥಿತಿ 0x11e: ಸ್ಥಿತಿ 0x120:
	    ಸ್ಥಿತಿ 0x122: ಸ್ಥಿತಿ 0x193: ಸ್ಥಿತಿ 0x1e4: ಸ್ಥಿತಿ 0x1e6:
	    ಸ್ಥಿತಿ 0x1f4: ಸ್ಥಿತಿ 0x1e20: ಸ್ಥಿತಿ 0xa7a0:
		      regmbc('G'); regmbc(0x11c); regmbc(0x11e);
		      regmbc(0x120); regmbc(0x122); regmbc(0x193);
		      regmbc(0x1e4); regmbc(0x1e6); regmbc(0x1f4);
		      regmbc(0x1e20); regmbc(0xa7a0);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'H': ಸ್ಥಿತಿ 0x124: ಸ್ಥಿತಿ 0x126: ಸ್ಥಿತಿ 0x21e:
	    ಸ್ಥಿತಿ 0x1e22: ಸ್ಥಿತಿ 0x1e24: ಸ್ಥಿತಿ 0x1e26:
	    ಸ್ಥಿತಿ 0x1e28: ಸ್ಥಿತಿ 0x1e2a: ಸ್ಥಿತಿ 0x2c67:
		      regmbc('H'); regmbc(0x124); regmbc(0x126);
		      regmbc(0x21e); regmbc(0x1e22); regmbc(0x1e24);
		      regmbc(0x1e26); regmbc(0x1e28); regmbc(0x1e2a);
		      regmbc(0x2c67);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'I': ಸ್ಥಿತಿ 0xcc: ಸ್ಥಿತಿ 0xcd: ಸ್ಥಿತಿ 0xce: ಸ್ಥಿತಿ 0xcf:
	    ಸ್ಥಿತಿ 0x128: ಸ್ಥಿತಿ 0x12a: ಸ್ಥಿತಿ 0x12c: ಸ್ಥಿತಿ 0x12e:
	    ಸ್ಥಿತಿ 0x130: ಸ್ಥಿತಿ 0x197: ಸ್ಥಿತಿ 0x1cf: ಸ್ಥಿತಿ 0x208:
	    ಸ್ಥಿತಿ 0x20a: ಸ್ಥಿತಿ 0x1e2c: ಸ್ಥಿತಿ 0x1e2e: ಸ್ಥಿತಿ 0x1ec8:
	    ಸ್ಥಿತಿ 0x1eca:
		      regmbc('I'); regmbc(0xcc); regmbc(0xcd);
		      regmbc(0xce); regmbc(0xcf); regmbc(0x128);
		      regmbc(0x12a); regmbc(0x12c); regmbc(0x12e);
		      regmbc(0x130); regmbc(0x197); regmbc(0x1cf);
		      regmbc(0x208); regmbc(0x20a); regmbc(0x1e2c);
		      regmbc(0x1e2e); regmbc(0x1ec8); regmbc(0x1eca);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'J': ಸ್ಥಿತಿ 0x134: ಸ್ಥಿತಿ 0x248:
		      regmbc('J'); regmbc(0x134); regmbc(0x248);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'K': ಸ್ಥಿತಿ 0x136: ಸ್ಥಿತಿ 0x198: ಸ್ಥಿತಿ 0x1e8: ಸ್ಥಿತಿ 0x1e30:
	    ಸ್ಥಿತಿ 0x1e32: ಸ್ಥಿತಿ 0x1e34: ಸ್ಥಿತಿ 0x2c69: ಸ್ಥಿತಿ 0xa740:
		      regmbc('K'); regmbc(0x136); regmbc(0x198);
		      regmbc(0x1e8); regmbc(0x1e30); regmbc(0x1e32);
		      regmbc(0x1e34); regmbc(0x2c69); regmbc(0xa740);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'L': ಸ್ಥಿತಿ 0x139: ಸ್ಥಿತಿ 0x13b: ಸ್ಥಿತಿ 0x13d: ಸ್ಥಿತಿ 0x13f:
	    ಸ್ಥಿತಿ 0x141: ಸ್ಥಿತಿ 0x23d: ಸ್ಥಿತಿ 0x1e36: ಸ್ಥಿತಿ 0x1e38:
	    ಸ್ಥಿತಿ 0x1e3a: ಸ್ಥಿತಿ 0x1e3c: ಸ್ಥಿತಿ 0x2c60:
		      regmbc('L'); regmbc(0x139); regmbc(0x13b);
		      regmbc(0x13d); regmbc(0x13f); regmbc(0x141);
		      regmbc(0x23d); regmbc(0x1e36); regmbc(0x1e38);
		      regmbc(0x1e3a); regmbc(0x1e3c); regmbc(0x2c60);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'M': ಸ್ಥಿತಿ 0x1e3e: ಸ್ಥಿತಿ 0x1e40: ಸ್ಥಿತಿ 0x1e42:
		      regmbc('M'); regmbc(0x1e3e); regmbc(0x1e40);
		      regmbc(0x1e42);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'N': ಸ್ಥಿತಿ 0xd1:
	    ಸ್ಥಿತಿ 0x143: ಸ್ಥಿತಿ 0x145: ಸ್ಥಿತಿ 0x147: ಸ್ಥಿತಿ 0x1f8:
	    ಸ್ಥಿತಿ 0x1e44: ಸ್ಥಿತಿ 0x1e46: ಸ್ಥಿತಿ 0x1e48: ಸ್ಥಿತಿ 0x1e4a:
	    ಸ್ಥಿತಿ 0xa7a4:
		      regmbc('N'); regmbc(0xd1);
		      regmbc(0x143); regmbc(0x145); regmbc(0x147);
		      regmbc(0x1f8); regmbc(0x1e44); regmbc(0x1e46);
		      regmbc(0x1e48); regmbc(0x1e4a); regmbc(0xa7a4);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'O': ಸ್ಥಿತಿ 0xd2: ಸ್ಥಿತಿ 0xd3: ಸ್ಥಿತಿ 0xd4: ಸ್ಥಿತಿ 0xd5: ಸ್ಥಿತಿ 0xd6:
	    ಸ್ಥಿತಿ 0xd8: ಸ್ಥಿತಿ 0x14c: ಸ್ಥಿತಿ 0x14e: ಸ್ಥಿತಿ 0x150: ಸ್ಥಿತಿ 0x19f:
	    ಸ್ಥಿತಿ 0x1a0: ಸ್ಥಿತಿ 0x1d1: ಸ್ಥಿತಿ 0x1ea: ಸ್ಥಿತಿ 0x1ec: ಸ್ಥಿತಿ 0x1fe:
	    ಸ್ಥಿತಿ 0x20c: ಸ್ಥಿತಿ 0x20e: ಸ್ಥಿತಿ 0x22a: ಸ್ಥಿತಿ 0x22c: ಸ್ಥಿತಿ 0x22e:
	    ಸ್ಥಿತಿ 0x230: ಸ್ಥಿತಿ 0x1e4c: ಸ್ಥಿತಿ 0x1e4e: ಸ್ಥಿತಿ 0x1e50: ಸ್ಥಿತಿ 0x1e52:
	    ಸ್ಥಿತಿ 0x1ecc: ಸ್ಥಿತಿ 0x1ece: ಸ್ಥಿತಿ 0x1ed0: ಸ್ಥಿತಿ 0x1ed2: ಸ್ಥಿತಿ 0x1ed4:
	    ಸ್ಥಿತಿ 0x1ed6: ಸ್ಥಿತಿ 0x1ed8: ಸ್ಥಿತಿ 0x1eda: ಸ್ಥಿತಿ 0x1edc: ಸ್ಥಿತಿ 0x1ede:
	    ಸ್ಥಿತಿ 0x1ee0: ಸ್ಥಿತಿ 0x1ee2:
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
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'P': ಸ್ಥಿತಿ 0x1a4: ಸ್ಥಿತಿ 0x1e54: ಸ್ಥಿತಿ 0x1e56: ಸ್ಥಿತಿ 0x2c63:
		      regmbc('P'); regmbc(0x1a4); regmbc(0x1e54);
		      regmbc(0x1e56); regmbc(0x2c63);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'Q': ಸ್ಥಿತಿ 0x24a:
		      regmbc('Q'); regmbc(0x24a);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'R': ಸ್ಥಿತಿ 0x154: ಸ್ಥಿತಿ 0x156: ಸ್ಥಿತಿ 0x158: ಸ್ಥಿತಿ 0x210:
	    ಸ್ಥಿತಿ 0x212: ಸ್ಥಿತಿ 0x24c: ಸ್ಥಿತಿ 0x1e58: ಸ್ಥಿತಿ 0x1e5a:
	    ಸ್ಥಿತಿ 0x1e5c: ಸ್ಥಿತಿ 0x1e5e: ಸ್ಥಿತಿ 0x2c64: ಸ್ಥಿತಿ 0xa7a6:
		      regmbc('R'); regmbc(0x154); regmbc(0x156);
		      regmbc(0x210); regmbc(0x212); regmbc(0x158);
		      regmbc(0x24c); regmbc(0x1e58); regmbc(0x1e5a);
		      regmbc(0x1e5c); regmbc(0x1e5e); regmbc(0x2c64);
		      regmbc(0xa7a6);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'S': ಸ್ಥಿತಿ 0x15a: ಸ್ಥಿತಿ 0x15c: ಸ್ಥಿತಿ 0x15e: ಸ್ಥಿತಿ 0x160:
	    ಸ್ಥಿತಿ 0x218: ಸ್ಥಿತಿ 0x1e60: ಸ್ಥಿತಿ 0x1e62: ಸ್ಥಿತಿ 0x1e64:
	    ಸ್ಥಿತಿ 0x1e66: ಸ್ಥಿತಿ 0x1e68: ಸ್ಥಿತಿ 0x2c7e: ಸ್ಥಿತಿ 0xa7a8:
		      regmbc('S'); regmbc(0x15a); regmbc(0x15c);
		      regmbc(0x15e); regmbc(0x160); regmbc(0x218);
		      regmbc(0x1e60); regmbc(0x1e62); regmbc(0x1e64);
		      regmbc(0x1e66); regmbc(0x1e68); regmbc(0x2c7e);
			  regmbc(0xa7a8);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'T': ಸ್ಥಿತಿ 0x162: ಸ್ಥಿತಿ 0x164: ಸ್ಥಿತಿ 0x166: ಸ್ಥಿತಿ 0x1ac:
	    ಸ್ಥಿತಿ 0x1ae: ಸ್ಥಿತಿ 0x21a: ಸ್ಥಿತಿ 0x23e: ಸ್ಥಿತಿ 0x1e6a: ಸ್ಥಿತಿ 0x1e6c:
	    ಸ್ಥಿತಿ 0x1e6e: ಸ್ಥಿತಿ 0x1e70:
		      regmbc('T'); regmbc(0x162); regmbc(0x164);
		      regmbc(0x166); regmbc(0x1ac); regmbc(0x23e);
		      regmbc(0x1ae); regmbc(0x21a); regmbc(0x1e6a);
		      regmbc(0x1e6c); regmbc(0x1e6e); regmbc(0x1e70);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'U': ಸ್ಥಿತಿ 0xd9: ಸ್ಥಿತಿ 0xda: ಸ್ಥಿತಿ 0xdb: ಸ್ಥಿತಿ 0xdc:
	    ಸ್ಥಿತಿ 0x168: ಸ್ಥಿತಿ 0x16a: ಸ್ಥಿತಿ 0x16c: ಸ್ಥಿತಿ 0x16e:
	    ಸ್ಥಿತಿ 0x170: ಸ್ಥಿತಿ 0x172: ಸ್ಥಿತಿ 0x1af: ಸ್ಥಿತಿ 0x1d3:
	    ಸ್ಥಿತಿ 0x1d5: ಸ್ಥಿತಿ 0x1d7: ಸ್ಥಿತಿ 0x1d9: ಸ್ಥಿತಿ 0x1db:
	    ಸ್ಥಿತಿ 0x214: ಸ್ಥಿತಿ 0x216: ಸ್ಥಿತಿ 0x244: ಸ್ಥಿತಿ 0x1e72:
	    ಸ್ಥಿತಿ 0x1e74: ಸ್ಥಿತಿ 0x1e76: ಸ್ಥಿತಿ 0x1e78: ಸ್ಥಿತಿ 0x1e7a:
	    ಸ್ಥಿತಿ 0x1ee4: ಸ್ಥಿತಿ 0x1ee6: ಸ್ಥಿತಿ 0x1ee8: ಸ್ಥಿತಿ 0x1eea:
	    ಸ್ಥಿತಿ 0x1eec: ಸ್ಥಿತಿ 0x1eee: ಸ್ಥಿತಿ 0x1ef0:
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
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'V': ಸ್ಥಿತಿ 0x1b2: ಸ್ಥಿತಿ 0x1e7c: ಸ್ಥಿತಿ 0x1e7e:
		      regmbc('V'); regmbc(0x1b2); regmbc(0x1e7c);
		      regmbc(0x1e7e);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'W': ಸ್ಥಿತಿ 0x174: ಸ್ಥಿತಿ 0x1e80: ಸ್ಥಿತಿ 0x1e82:
	    ಸ್ಥಿತಿ 0x1e84: ಸ್ಥಿತಿ 0x1e86: ಸ್ಥಿತಿ 0x1e88:
		      regmbc('W'); regmbc(0x174); regmbc(0x1e80);
		      regmbc(0x1e82); regmbc(0x1e84); regmbc(0x1e86);
		      regmbc(0x1e88);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'X': ಸ್ಥಿತಿ 0x1e8a: ಸ್ಥಿತಿ 0x1e8c:
		      regmbc('X'); regmbc(0x1e8a); regmbc(0x1e8c);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'Y': ಸ್ಥಿತಿ 0xdd:
	    ಸ್ಥಿತಿ 0x176: ಸ್ಥಿತಿ 0x178: ಸ್ಥಿತಿ 0x1b3: ಸ್ಥಿತಿ 0x232: ಸ್ಥಿತಿ 0x24e:
	    ಸ್ಥಿತಿ 0x1e8e: ಸ್ಥಿತಿ 0x1ef2: ಸ್ಥಿತಿ 0x1ef6: ಸ್ಥಿತಿ 0x1ef4: ಸ್ಥಿತಿ 0x1ef8:
		      regmbc('Y'); regmbc(0xdd); regmbc(0x176);
		      regmbc(0x178); regmbc(0x1b3); regmbc(0x232);
		      regmbc(0x24e); regmbc(0x1e8e); regmbc(0x1ef2);
		      regmbc(0x1ef4); regmbc(0x1ef6); regmbc(0x1ef8);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'Z': ಸ್ಥಿತಿ 0x179: ಸ್ಥಿತಿ 0x17b: ಸ್ಥಿತಿ 0x17d: ಸ್ಥಿತಿ 0x1b5:
	    ಸ್ಥಿತಿ 0x1e90: ಸ್ಥಿತಿ 0x1e92: ಸ್ಥಿತಿ 0x1e94: ಸ್ಥಿತಿ 0x2c6b:
		      regmbc('Z'); regmbc(0x179); regmbc(0x17b);
		      regmbc(0x17d); regmbc(0x1b5); regmbc(0x1e90);
		      regmbc(0x1e92); regmbc(0x1e94); regmbc(0x2c6b);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'a': ಸ್ಥಿತಿ 0xe0: ಸ್ಥಿತಿ 0xe1: ಸ್ಥಿತಿ 0xe2:
	    ಸ್ಥಿತಿ 0xe3: ಸ್ಥಿತಿ 0xe4: ಸ್ಥಿತಿ 0xe5: ಸ್ಥಿತಿ 0x101: ಸ್ಥಿತಿ 0x103:
	    ಸ್ಥಿತಿ 0x105: ಸ್ಥಿತಿ 0x1ce: ಸ್ಥಿತಿ 0x1df: ಸ್ಥಿತಿ 0x1e1: ಸ್ಥಿತಿ 0x1fb:
	    ಸ್ಥಿತಿ 0x201: ಸ್ಥಿತಿ 0x203: ಸ್ಥಿತಿ 0x227: ಸ್ಥಿತಿ 0x1d8f: ಸ್ಥಿತಿ 0x1e01:
	    ಸ್ಥಿತಿ 0x1e9a: ಸ್ಥಿತಿ 0x1ea1: ಸ್ಥಿತಿ 0x1ea3: ಸ್ಥಿತಿ 0x1ea5:
	    ಸ್ಥಿತಿ 0x1ea7: ಸ್ಥಿತಿ 0x1ea9: ಸ್ಥಿತಿ 0x1eab: ಸ್ಥಿತಿ 0x1ead:
	    ಸ್ಥಿತಿ 0x1eaf: ಸ್ಥಿತಿ 0x1eb1: ಸ್ಥಿತಿ 0x1eb3: ಸ್ಥಿತಿ 0x1eb5:
	    ಸ್ಥಿತಿ 0x1eb7: ಸ್ಥಿತಿ 0x2c65:
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
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'b': ಸ್ಥಿತಿ 0x180: ಸ್ಥಿತಿ 0x253: ಸ್ಥಿತಿ 0x1d6c: ಸ್ಥಿತಿ 0x1d80:
	    ಸ್ಥಿತಿ 0x1e03: ಸ್ಥಿತಿ 0x1e05: ಸ್ಥಿತಿ 0x1e07:
		      regmbc('b');
		      regmbc(0x180); regmbc(0x253); regmbc(0x1d6c);
		      regmbc(0x1d80); regmbc(0x1e03); regmbc(0x1e05);
		      regmbc(0x1e07);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'c': ಸ್ಥಿತಿ 0xe7:
	    ಸ್ಥಿತಿ 0x107: ಸ್ಥಿತಿ 0x109: ಸ್ಥಿತಿ 0x10b: ಸ್ಥಿತಿ 0x10d: ಸ್ಥಿತಿ 0x188:
	    ಸ್ಥಿತಿ 0x23c: ಸ್ಥಿತಿ 0x1e09: ಸ್ಥಿತಿ 0xa793: ಸ್ಥಿತಿ 0xa794:
		      regmbc('c'); regmbc(0xe7); regmbc(0x107);
		      regmbc(0x109); regmbc(0x10b); regmbc(0x10d);
		      regmbc(0x188); regmbc(0x23c); regmbc(0x1e09);
		      regmbc(0xa793); regmbc(0xa794);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'd': ಸ್ಥಿತಿ 0x10f: ಸ್ಥಿತಿ 0x111: ಸ್ಥಿತಿ 0x257: ಸ್ಥಿತಿ 0x1d6d:
	    ಸ್ಥಿತಿ 0x1d81: ಸ್ಥಿತಿ 0x1d91: ಸ್ಥಿತಿ 0x1e0b: ಸ್ಥಿತಿ 0x1e0d:
	    ಸ್ಥಿತಿ 0x1e0f: ಸ್ಥಿತಿ 0x1e11: ಸ್ಥಿತಿ 0x1e13:
		      regmbc('d'); regmbc(0x10f); regmbc(0x111);
		      regmbc(0x257); regmbc(0x1d6d); regmbc(0x1d81);
		      regmbc(0x1d91); regmbc(0x1e0b); regmbc(0x1e0d);
		      regmbc(0x1e0f); regmbc(0x1e11); regmbc(0x1e13);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'e': ಸ್ಥಿತಿ 0xe8: ಸ್ಥಿತಿ 0xe9: ಸ್ಥಿತಿ 0xea: ಸ್ಥಿತಿ 0xeb:
	    ಸ್ಥಿತಿ 0x113: ಸ್ಥಿತಿ 0x115: ಸ್ಥಿತಿ 0x117: ಸ್ಥಿತಿ 0x119:
	    ಸ್ಥಿತಿ 0x11b: ಸ್ಥಿತಿ 0x205: ಸ್ಥಿತಿ 0x207: ಸ್ಥಿತಿ 0x229:
	    ಸ್ಥಿತಿ 0x247: ಸ್ಥಿತಿ 0x1d92: ಸ್ಥಿತಿ 0x1e15: ಸ್ಥಿತಿ 0x1e17:
	    ಸ್ಥಿತಿ 0x1e19: ಸ್ಥಿತಿ 0x1e1b: ಸ್ಥಿತಿ 0x1eb9: ಸ್ಥಿತಿ 0x1ebb:
	    ಸ್ಥಿತಿ 0x1e1d: ಸ್ಥಿತಿ 0x1ebd: ಸ್ಥಿತಿ 0x1ebf: ಸ್ಥಿತಿ 0x1ec1:
	    ಸ್ಥಿತಿ 0x1ec3: ಸ್ಥಿತಿ 0x1ec5: ಸ್ಥಿತಿ 0x1ec7:
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
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'f': ಸ್ಥಿತಿ 0x192: ಸ್ಥಿತಿ 0x1d6e: ಸ್ಥಿತಿ 0x1d82:
	    ಸ್ಥಿತಿ 0x1e1f: ಸ್ಥಿತಿ 0xa799:
		     regmbc('f'); regmbc(0x192); regmbc(0x1d6e);
		     regmbc(0x1d82); regmbc(0x1e1f); regmbc(0xa799);
		     ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'g': ಸ್ಥಿತಿ 0x11d: ಸ್ಥಿತಿ 0x11f: ಸ್ಥಿತಿ 0x121: ಸ್ಥಿತಿ 0x123:
	    ಸ್ಥಿತಿ 0x1e5: ಸ್ಥಿತಿ 0x1e7: ಸ್ಥಿತಿ 0x260: ಸ್ಥಿತಿ 0x1f5: ಸ್ಥಿತಿ 0x1d83:
	    ಸ್ಥಿತಿ 0x1e21: ಸ್ಥಿತಿ 0xa7a1:
		      regmbc('g'); regmbc(0x11d); regmbc(0x11f);
		      regmbc(0x121); regmbc(0x123); regmbc(0x1e5);
		      regmbc(0x1e7); regmbc(0x1f5); regmbc(0x260);
		      regmbc(0x1d83); regmbc(0x1e21); regmbc(0xa7a1);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'h': ಸ್ಥಿತಿ 0x125: ಸ್ಥಿತಿ 0x127: ಸ್ಥಿತಿ 0x21f: ಸ್ಥಿತಿ 0x1e23:
	    ಸ್ಥಿತಿ 0x1e25: ಸ್ಥಿತಿ 0x1e27: ಸ್ಥಿತಿ 0x1e29: ಸ್ಥಿತಿ 0x1e2b:
	    ಸ್ಥಿತಿ 0x1e96: ಸ್ಥಿತಿ 0x2c68: ಸ್ಥಿತಿ 0xa795:
		      regmbc('h'); regmbc(0x125); regmbc(0x127);
		      regmbc(0x21f); regmbc(0x1e23); regmbc(0x1e25);
		      regmbc(0x1e27); regmbc(0x1e29); regmbc(0x1e2b);
		      regmbc(0x1e96); regmbc(0x2c68); regmbc(0xa795);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'i': ಸ್ಥಿತಿ 0xec: ಸ್ಥಿತಿ 0xed: ಸ್ಥಿತಿ 0xee: ಸ್ಥಿತಿ 0xef:
	    ಸ್ಥಿತಿ 0x129: ಸ್ಥಿತಿ 0x12b: ಸ್ಥಿತಿ 0x12d: ಸ್ಥಿತಿ 0x12f:
	    ಸ್ಥಿತಿ 0x1d0: ಸ್ಥಿತಿ 0x209: ಸ್ಥಿತಿ 0x20b: ಸ್ಥಿತಿ 0x268:
	    ಸ್ಥಿತಿ 0x1d96: ಸ್ಥಿತಿ 0x1e2d: ಸ್ಥಿತಿ 0x1e2f: ಸ್ಥಿತಿ 0x1ec9:
	    ಸ್ಥಿತಿ 0x1ecb:
		      regmbc('i'); regmbc(0xec); regmbc(0xed);
		      regmbc(0xee); regmbc(0xef); regmbc(0x129);
		      regmbc(0x12b); regmbc(0x12d); regmbc(0x12f);
		      regmbc(0x1d0); regmbc(0x209); regmbc(0x20b);
		      regmbc(0x268); regmbc(0x1d96); regmbc(0x1e2d);
		      regmbc(0x1e2f); regmbc(0x1ec9); regmbc(0x1ecb);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'j': ಸ್ಥಿತಿ 0x135: ಸ್ಥಿತಿ 0x1f0: ಸ್ಥಿತಿ 0x249:
		      regmbc('j'); regmbc(0x135); regmbc(0x1f0);
		      regmbc(0x249);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'k': ಸ್ಥಿತಿ 0x137: ಸ್ಥಿತಿ 0x199: ಸ್ಥಿತಿ 0x1e9:
	    ಸ್ಥಿತಿ 0x1d84: ಸ್ಥಿತಿ 0x1e31: ಸ್ಥಿತಿ 0x1e33: ಸ್ಥಿತಿ 0x1e35:
	    ಸ್ಥಿತಿ 0x2c6a: ಸ್ಥಿತಿ 0xa741:
		      regmbc('k'); regmbc(0x137); regmbc(0x199);
		      regmbc(0x1e9); regmbc(0x1d84); regmbc(0x1e31);
		      regmbc(0x1e33); regmbc(0x1e35); regmbc(0x2c6a);
		      regmbc(0xa741);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'l': ಸ್ಥಿತಿ 0x13a: ಸ್ಥಿತಿ 0x13c: ಸ್ಥಿತಿ 0x13e:
	    ಸ್ಥಿತಿ 0x140: ಸ್ಥಿತಿ 0x142: ಸ್ಥಿತಿ 0x19a: ಸ್ಥಿತಿ 0x1e37:
	    ಸ್ಥಿತಿ 0x1e39: ಸ್ಥಿತಿ 0x1e3b: ಸ್ಥಿತಿ 0x1e3d: ಸ್ಥಿತಿ 0x2c61:
		      regmbc('l'); regmbc(0x13a); regmbc(0x13c);
		      regmbc(0x13e); regmbc(0x140); regmbc(0x142);
		      regmbc(0x19a); regmbc(0x1e37); regmbc(0x1e39);
		      regmbc(0x1e3b); regmbc(0x1e3d); regmbc(0x2c61);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'm': ಸ್ಥಿತಿ 0x1d6f: ಸ್ಥಿತಿ 0x1e3f: ಸ್ಥಿತಿ 0x1e41: ಸ್ಥಿತಿ 0x1e43:
		      regmbc('m'); regmbc(0x1d6f); regmbc(0x1e3f);
		      regmbc(0x1e41); regmbc(0x1e43);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'n': ಸ್ಥಿತಿ 0xf1: ಸ್ಥಿತಿ 0x144: ಸ್ಥಿತಿ 0x146: ಸ್ಥಿತಿ 0x148:
	    ಸ್ಥಿತಿ 0x149: ಸ್ಥಿತಿ 0x1f9: ಸ್ಥಿತಿ 0x1d70: ಸ್ಥಿತಿ 0x1d87:
	    ಸ್ಥಿತಿ 0x1e45: ಸ್ಥಿತಿ 0x1e47: ಸ್ಥಿತಿ 0x1e49: ಸ್ಥಿತಿ 0x1e4b:
	    ಸ್ಥಿತಿ 0xa7a5:
		      regmbc('n'); regmbc(0xf1); regmbc(0x144);
		      regmbc(0x146); regmbc(0x148); regmbc(0x149);
		      regmbc(0x1f9); regmbc(0x1d70); regmbc(0x1d87);
		      regmbc(0x1e45); regmbc(0x1e47); regmbc(0x1e49);
		      regmbc(0x1e4b); regmbc(0xa7a5);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'o': ಸ್ಥಿತಿ 0xf2: ಸ್ಥಿತಿ 0xf3: ಸ್ಥಿತಿ 0xf4: ಸ್ಥಿತಿ 0xf5:
	    ಸ್ಥಿತಿ 0xf6: ಸ್ಥಿತಿ 0xf8: ಸ್ಥಿತಿ 0x14d: ಸ್ಥಿತಿ 0x14f: ಸ್ಥಿತಿ 0x151:
	    ಸ್ಥಿತಿ 0x1a1: ಸ್ಥಿತಿ 0x1d2: ಸ್ಥಿತಿ 0x1eb: ಸ್ಥಿತಿ 0x1ed: ಸ್ಥಿತಿ 0x1ff:
	    ಸ್ಥಿತಿ 0x20d: ಸ್ಥಿತಿ 0x20f: ಸ್ಥಿತಿ 0x22b: ಸ್ಥಿತಿ 0x22d: ಸ್ಥಿತಿ 0x22f:
	    ಸ್ಥಿತಿ 0x231: ಸ್ಥಿತಿ 0x275: ಸ್ಥಿತಿ 0x1e4d: ಸ್ಥಿತಿ 0x1e4f:
	    ಸ್ಥಿತಿ 0x1e51: ಸ್ಥಿತಿ 0x1e53: ಸ್ಥಿತಿ 0x1ecd: ಸ್ಥಿತಿ 0x1ecf:
	    ಸ್ಥಿತಿ 0x1ed1: ಸ್ಥಿತಿ 0x1ed3: ಸ್ಥಿತಿ 0x1ed5: ಸ್ಥಿತಿ 0x1ed7:
	    ಸ್ಥಿತಿ 0x1ed9: ಸ್ಥಿತಿ 0x1edb: ಸ್ಥಿತಿ 0x1edd: ಸ್ಥಿತಿ 0x1edf:
	    ಸ್ಥಿತಿ 0x1ee1: ಸ್ಥಿತಿ 0x1ee3:
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
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'p': ಸ್ಥಿತಿ 0x1a5: ಸ್ಥಿತಿ 0x1d71: ಸ್ಥಿತಿ 0x1d88: ಸ್ಥಿತಿ 0x1d7d:
	    ಸ್ಥಿತಿ 0x1e55: ಸ್ಥಿತಿ 0x1e57:
		      regmbc('p'); regmbc(0x1a5); regmbc(0x1d71);
		      regmbc(0x1d7d); regmbc(0x1d88); regmbc(0x1e55);
		      regmbc(0x1e57);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'q': ಸ್ಥಿತಿ 0x24b: ಸ್ಥಿತಿ 0x2a0:
		      regmbc('q'); regmbc(0x24b); regmbc(0x2a0);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'r': ಸ್ಥಿತಿ 0x155: ಸ್ಥಿತಿ 0x157: ಸ್ಥಿತಿ 0x159: ಸ್ಥಿತಿ 0x211:
	    ಸ್ಥಿತಿ 0x213: ಸ್ಥಿತಿ 0x24d: ಸ್ಥಿತಿ 0x27d: ಸ್ಥಿತಿ 0x1d72: ಸ್ಥಿತಿ 0x1d73:
	    ಸ್ಥಿತಿ 0x1d89: ಸ್ಥಿತಿ 0x1e59: ಸ್ಥಿತಿ 0x1e5b: ಸ್ಥಿತಿ 0x1e5d: ಸ್ಥಿತಿ 0x1e5f:
	    ಸ್ಥಿತಿ 0xa7a7:
		      regmbc('r'); regmbc(0x155); regmbc(0x157);
		      regmbc(0x159); regmbc(0x211); regmbc(0x213);
		      regmbc(0x24d); regmbc(0x1d72); regmbc(0x1d73);
		      regmbc(0x1d89); regmbc(0x1e59); regmbc(0x27d);
		      regmbc(0x1e5b); regmbc(0x1e5d); regmbc(0x1e5f);
		      regmbc(0xa7a7);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 's': ಸ್ಥಿತಿ 0x15b: ಸ್ಥಿತಿ 0x15d: ಸ್ಥಿತಿ 0x15f: ಸ್ಥಿತಿ 0x161:
	    ಸ್ಥಿತಿ 0x1e61: ಸ್ಥಿತಿ 0x219: ಸ್ಥಿತಿ 0x23f: ಸ್ಥಿತಿ 0x1d74: ಸ್ಥಿತಿ 0x1d8a:
	    ಸ್ಥಿತಿ 0x1e63: ಸ್ಥಿತಿ 0x1e65: ಸ್ಥಿತಿ 0x1e67: ಸ್ಥಿತಿ 0x1e69: ಸ್ಥಿತಿ 0xa7a9:
		      regmbc('s'); regmbc(0x15b); regmbc(0x15d);
		      regmbc(0x15f); regmbc(0x161); regmbc(0x23f);
		      regmbc(0x219); regmbc(0x1d74); regmbc(0x1d8a);
		      regmbc(0x1e61); regmbc(0x1e63); regmbc(0x1e65);
		      regmbc(0x1e67); regmbc(0x1e69); regmbc(0xa7a9);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 't': ಸ್ಥಿತಿ 0x163: ಸ್ಥಿತಿ 0x165: ಸ್ಥಿತಿ 0x167: ಸ್ಥಿತಿ 0x1ab:
	    ಸ್ಥಿತಿ 0x1ad: ಸ್ಥಿತಿ 0x21b: ಸ್ಥಿತಿ 0x288: ಸ್ಥಿತಿ 0x1d75: ಸ್ಥಿತಿ 0x1e6b:
	    ಸ್ಥಿತಿ 0x1e6d: ಸ್ಥಿತಿ 0x1e6f: ಸ್ಥಿತಿ 0x1e71: ಸ್ಥಿತಿ 0x1e97: ಸ್ಥಿತಿ 0x2c66:
		      regmbc('t'); regmbc(0x163); regmbc(0x165);
		      regmbc(0x167); regmbc(0x1ab); regmbc(0x21b);
		      regmbc(0x1ad); regmbc(0x288); regmbc(0x1d75);
		      regmbc(0x1e6b); regmbc(0x1e6d); regmbc(0x1e6f);
		      regmbc(0x1e71); regmbc(0x1e97); regmbc(0x2c66);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'u': ಸ್ಥಿತಿ 0xf9: ಸ್ಥಿತಿ 0xfa: ಸ್ಥಿತಿ 0xfb: ಸ್ಥಿತಿ 0xfc:
	    ಸ್ಥಿತಿ 0x169: ಸ್ಥಿತಿ 0x16b: ಸ್ಥಿತಿ 0x16d: ಸ್ಥಿತಿ 0x16f:
	    ಸ್ಥಿತಿ 0x171: ಸ್ಥಿತಿ 0x173: ಸ್ಥಿತಿ 0x1b0: ಸ್ಥಿತಿ 0x1d4:
	    ಸ್ಥಿತಿ 0x1d6: ಸ್ಥಿತಿ 0x1d8: ಸ್ಥಿತಿ 0x1da: ಸ್ಥಿತಿ 0x1dc:
	    ಸ್ಥಿತಿ 0x215: ಸ್ಥಿತಿ 0x217: ಸ್ಥಿತಿ 0x289: ಸ್ಥಿತಿ 0x1e73:
	    ಸ್ಥಿತಿ 0x1d7e: ಸ್ಥಿತಿ 0x1d99: ಸ್ಥಿತಿ 0x1e75: ಸ್ಥಿತಿ 0x1e77:
	    ಸ್ಥಿತಿ 0x1e79: ಸ್ಥಿತಿ 0x1e7b: ಸ್ಥಿತಿ 0x1ee5: ಸ್ಥಿತಿ 0x1ee7:
	    ಸ್ಥಿತಿ 0x1ee9: ಸ್ಥಿತಿ 0x1eeb: ಸ್ಥಿತಿ 0x1eed: ಸ್ಥಿತಿ 0x1eef:
	    ಸ್ಥಿತಿ 0x1ef1:
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
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'v': ಸ್ಥಿತಿ 0x28b: ಸ್ಥಿತಿ 0x1d8c: ಸ್ಥಿತಿ 0x1e7d: ಸ್ಥಿತಿ 0x1e7f:
		      regmbc('v'); regmbc(0x28b); regmbc(0x1d8c);
		      regmbc(0x1e7d); regmbc(0x1e7f);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'w': ಸ್ಥಿತಿ 0x175: ಸ್ಥಿತಿ 0x1e81: ಸ್ಥಿತಿ 0x1e83:
	    ಸ್ಥಿತಿ 0x1e85: ಸ್ಥಿತಿ 0x1e87: ಸ್ಥಿತಿ 0x1e89: ಸ್ಥಿತಿ 0x1e98:
		      regmbc('w'); regmbc(0x175); regmbc(0x1e81);
		      regmbc(0x1e83); regmbc(0x1e85); regmbc(0x1e87);
		      regmbc(0x1e89); regmbc(0x1e98);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'x': ಸ್ಥಿತಿ 0x1e8b: ಸ್ಥಿತಿ 0x1e8d:
		      regmbc('x'); regmbc(0x1e8b); regmbc(0x1e8d);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'y': ಸ್ಥಿತಿ 0xfd: ಸ್ಥಿತಿ 0xff: ಸ್ಥಿತಿ 0x177: ಸ್ಥಿತಿ 0x1b4:
	    ಸ್ಥಿತಿ 0x233: ಸ್ಥಿತಿ 0x24f: ಸ್ಥಿತಿ 0x1e8f: ಸ್ಥಿತಿ 0x1e99: ಸ್ಥಿತಿ 0x1ef3:
	    ಸ್ಥಿತಿ 0x1ef5: ಸ್ಥಿತಿ 0x1ef7: ಸ್ಥಿತಿ 0x1ef9:
		      regmbc('y'); regmbc(0xfd); regmbc(0xff);
		      regmbc(0x177); regmbc(0x1b4); regmbc(0x233);
		      regmbc(0x24f); regmbc(0x1e8f); regmbc(0x1e99);
		      regmbc(0x1ef3); regmbc(0x1ef5); regmbc(0x1ef7);
		      regmbc(0x1ef9);
		      ಹಿಂತಿರುಗಿ;
	    ಸ್ಥಿತಿ 'z': ಸ್ಥಿತಿ 0x17a: ಸ್ಥಿತಿ 0x17c: ಸ್ಥಿತಿ 0x17e: ಸ್ಥಿತಿ 0x1b6:
	    ಸ್ಥಿತಿ 0x1d76: ಸ್ಥಿತಿ 0x1d8e: ಸ್ಥಿತಿ 0x1e91: ಸ್ಥಿತಿ 0x1e93:
	    ಸ್ಥಿತಿ 0x1e95: ಸ್ಥಿತಿ 0x2c6c:
		      regmbc('z'); regmbc(0x17a); regmbc(0x17c);
		      regmbc(0x17e); regmbc(0x1b6); regmbc(0x1d76);
		      regmbc(0x1d8e); regmbc(0x1e91); regmbc(0x1e93);
		      regmbc(0x1e95); regmbc(0x2c6c);
		      ಹಿಂತಿರುಗಿ;
	}
    }
    regmbc(c);
}

/*
 * Emit a node.
 * Return pointer to generated code.
 */
    ಸ್ಥಿರ char_u *
regnode(ಪೂರ್ಣಾಂಕ op)
{
    char_u  *ret;

    ret = regcode;
    ಆದರೆ (ret == JUST_CALC_ಗಾತ್ರ)
	regsize += 3;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	*regcode++ = op;
	*regcode++ = NUL;		// Null "next" pointer.
	*regcode++ = NUL;
    }
    ಹಿಂತಿರುಗಿ ret;
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
    ಹಿಂತಿರುಗಿ p;
}

/*
 * regnext - dig the "next" pointer out of a node
 * Returns NULL when calculating size, when there is no next item and when
 * there is an error.
 */
    ಸ್ಥಿರ char_u *
regnext(char_u *p)
{
    ಪೂರ್ಣಾಂಕ	    ಸ್ಥಾನಾಂತರ;

    ಆದರೆ (p == JUST_CALC_ಗಾತ್ರ || reg_toolong)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಸ್ಥಾನಾಂತರ = ಮುಂದಿನ_2(p);
    ಆದರೆ (ಸ್ಥಾನಾಂತರ == 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (OP(p) == ಹಿಂಭಾಗ_2)
	ಹಿಂತಿರುಗಿ p - ಸ್ಥಾನಾಂತರ;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ p + ಸ್ಥಾನಾಂತರ;
}

/*
 * Set the next-pointer at the end of a node chain.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
regtail(char_u *p, char_u *val)
{
    char_u	*ಪರಿಶೀಲನೆ;
    char_u	*ತಾತ್ಕಾಲಿಕ;
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಾನಾಂತರ;

    ಆದರೆ (p == JUST_CALC_ಗಾತ್ರ)
	ಹಿಂತಿರುಗಿ;

    // Find last node.
    ಪರಿಶೀಲನೆ = p;
    ಗಾಗಿ (;;)
    {
	ತಾತ್ಕಾಲಿಕ = regnext(ಪರಿಶೀಲನೆ);
	ಆದರೆ (ತಾತ್ಕಾಲಿಕ == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	ಪರಿಶೀಲನೆ = ತಾತ್ಕಾಲಿಕ;
    }

    ಆದರೆ (OP(ಪರಿಶೀಲನೆ) == ಹಿಂಭಾಗ_2)
	ಸ್ಥಾನಾಂತರ = (ಪೂರ್ಣಾಂಕ)(ಪರಿಶೀಲನೆ - val);
    ಇಲ್ಲದಿದ್ದರೆ
	ಸ್ಥಾನಾಂತರ = (ಪೂರ್ಣಾಂಕ)(val - ಪರಿಶೀಲನೆ);
    // When the offset uses more than 16 bits it can no longer fit in the two
    // bytes available.  Use a global flag to avoid having to check return
    // values in too many places.
    ಆದರೆ (ಸ್ಥಾನಾಂತರ > 0xffff)
	reg_toolong = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	*(ಪರಿಶೀಲನೆ + 1) = (char_u) (((ಚಿಹ್ನೆರಹಿತ)ಸ್ಥಾನಾಂತರ >> 8) & 0377);
	*(ಪರಿಶೀಲನೆ + 2) = (char_u) (ಸ್ಥಾನಾಂತರ & 0377);
    }
}

/*
 * Like regtail, on item after a BRANCH; nop if none.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
regoptail(char_u *p, char_u *val)
{
    // When op is neither BRANCH nor BRACE_COMPLEX0-9, it is "operandless"
    ಆದರೆ (p == ಖಾಲಿ || p == JUST_CALC_ಗಾತ್ರ
	    || (OP(p) != BRANCH
		&& (OP(p) < BRACE_COMPLEX || OP(p) > BRACE_COMPLEX + 9)))
	ಹಿಂತಿರುಗಿ;
    regtail(ಕಾರ್ಯಾಂಶ(p), val);
}

/*
 * Insert an operator in front of already-emitted operand
 *
 * Means relocating the operand.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
reginsert(ಪೂರ್ಣಾಂಕ op, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*ಇರಿಸು;

    ಆದರೆ (regcode == JUST_CALC_ಗಾತ್ರ)
    {
	regsize += 3;
	ಹಿಂತಿರುಗಿ;
    }
    src = regcode;
    regcode += 3;
    dst = regcode;
    ಆಗ (src > opnd)
	*--dst = *--src;

    ಇರಿಸು = opnd;		// Op node, where operand used to be.
    *ಇರಿಸು++ = op;
    *ಇರಿಸು++ = NUL;
    *ಇರಿಸು = NUL;
}

/*
 * Insert an operator in front of already-emitted operand.
 * Add a number to the operator.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
reginsert_nr(ಪೂರ್ಣಾಂಕ op, ದೀರ್ಘ val, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*ಇರಿಸು;

    ಆದರೆ (regcode == JUST_CALC_ಗಾತ್ರ)
    {
	regsize += 7;
	ಹಿಂತಿರುಗಿ;
    }
    src = regcode;
    regcode += 7;
    dst = regcode;
    ಆಗ (src > opnd)
	*--dst = *--src;

    ಇರಿಸು = opnd;		// Op node, where operand used to be.
    *ಇರಿಸು++ = op;
    *ಇರಿಸು++ = NUL;
    *ಇರಿಸು++ = NUL;
    re_put_long(ಇರಿಸು, (long_u)val);
}

/*
 * Insert an operator in front of already-emitted operand.
 * The operator has the given limit values as operands.  Also set next pointer.
 *
 * Means relocating the operand.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
reginsert_limits(
    ಪೂರ್ಣಾಂಕ		op,
    ದೀರ್ಘ	minval,
    ದೀರ್ಘ	maxval,
    char_u	*opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*ಇರಿಸು;

    ಆದರೆ (regcode == JUST_CALC_ಗಾತ್ರ)
    {
	regsize += 11;
	ಹಿಂತಿರುಗಿ;
    }
    src = regcode;
    regcode += 11;
    dst = regcode;
    ಆಗ (src > opnd)
	*--dst = *--src;

    ಇರಿಸು = opnd;		// Op node, where operand used to be.
    *ಇರಿಸು++ = op;
    *ಇರಿಸು++ = NUL;
    *ಇರಿಸು++ = NUL;
    ಇರಿಸು = re_put_long(ಇರಿಸು, (long_u)minval);
    ಇರಿಸು = re_put_long(ಇರಿಸು, (long_u)maxval);
    regtail(opnd, ಇರಿಸು);
}

/*
 * Return TRUE if the back reference is legal. We must have seen the close
 * brace.
 * TODO: Should also check that we don't refer to something that is repeated
 * (+*=): what instance of the repetition should we match?
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ನೋಡಿದ_endbrace(ಪೂರ್ಣಾಂಕ refnum)
{
    ಆದರೆ (!had_endbrace[refnum])
    {
	char_u *p;

	// Trick: check if "@<=" or "@<!" follows, in which case
	// the \1 can appear before the referenced match.
	ಗಾಗಿ (p = regparse; *p != NUL; ++p)
	    ಆದರೆ (p[0] == '@' && p[1] == '<' && (p[2] == '!' || p[2] == '='))
		ನಿಲ್ಲಿಸು;
	ಆದರೆ (*p == NUL)
	{
	    emsg(_(e_illegal_ಹಿಂಭಾಗ_reference));
	    rc_did_emsg = TRUE;
	    ಹಿಂತಿರುಗಿ FALSE;
	}
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Parse the lowest level.
 *
 * Optimization:  gobbles an entire sequence of ordinary characters so that
 * it can turn them into a single node, which is smaller to store and
 * faster to run.  Don't do this when one_exactly is set.
 */
    ಸ್ಥಿರ char_u *
regatom(ಪೂರ್ಣಾಂಕ *flagp)
{
    char_u	    *ret;
    ಪೂರ್ಣಾಂಕ		    flags;
    ಪೂರ್ಣಾಂಕ		    c;
    char_u	    *p;
    ಪೂರ್ಣಾಂಕ		    extra = 0;
    ಪೂರ್ಣಾಂಕ		    save_prev_at_ಆರಂಭ = prev_at_ಆರಂಭ;

    *flagp = WORST;		// Tentatively.

    c = getchr();
    ಆಯ್ಕೆ (c)
    {
      ಸ್ಥಿತಿ Magic('^'):
	ret = regnode(BOL);
	ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ Magic('$'):
	ret = regnode(EOL);
#ಆದರೆ defined(FEAT_SYN_HL)
	had_eol = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
	ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ Magic('<'):
	ret = regnode(BOW);
	ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ Magic('>'):
	ret = regnode(EOW);
	ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ Magic('_'):
	c = no_Magic(getchr());
	ಆದರೆ (c == '^')		// "\_^" is start-of-line
	{
	    ret = regnode(BOL);
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (c == '$')		// "\_$" is end-of-line
	{
	    ret = regnode(EOL);
#ಆದರೆ defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
	    ನಿಲ್ಲಿಸು;
	}

	extra = ಕೂಡಿಸು_NL;
	*flagp |= HASNL;

	// "\_[" is character range plus newline
	ಆದರೆ (c == '[')
	    ಹೋಗು collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	// Character classes.
      ಸ್ಥಿತಿ Magic('.'):
      ಸ್ಥಿತಿ Magic('i'):
      ಸ್ಥಿತಿ Magic('I'):
      ಸ್ಥಿತಿ Magic('k'):
      ಸ್ಥಿತಿ Magic('K'):
      ಸ್ಥಿತಿ Magic('f'):
      ಸ್ಥಿತಿ Magic('F'):
      ಸ್ಥಿತಿ Magic('p'):
      ಸ್ಥಿತಿ Magic('P'):
      ಸ್ಥಿತಿ Magic('s'):
      ಸ್ಥಿತಿ Magic('S'):
      ಸ್ಥಿತಿ Magic('d'):
      ಸ್ಥಿತಿ Magic('D'):
      ಸ್ಥಿತಿ Magic('x'):
      ಸ್ಥಿತಿ Magic('X'):
      ಸ್ಥಿತಿ Magic('o'):
      ಸ್ಥಿತಿ Magic('O'):
      ಸ್ಥಿತಿ Magic('w'):
      ಸ್ಥಿತಿ Magic('W'):
      ಸ್ಥಿತಿ Magic('h'):
      ಸ್ಥಿತಿ Magic('H'):
      ಸ್ಥಿತಿ Magic('a'):
      ಸ್ಥಿತಿ Magic('A'):
      ಸ್ಥಿತಿ Magic('l'):
      ಸ್ಥಿತಿ Magic('L'):
      ಸ್ಥಿತಿ Magic('u'):
      ಸ್ಥಿತಿ Magic('U'):
	p = vim_strchr(classchars, no_Magic(c));
	ಆದರೆ (p == ಖಾಲಿ)
	    EMSG_RET_NULL(_(e_invalid_use_ದ_underscore));

	// When '.' is followed by a composing char ignore the dot, so that
	// the composing char is matched here.
	ಆದರೆ (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	{
	    c = getchr();
	    ಹೋಗು do_multibyte;
	}
	ret = regnode(classcodes[p - classchars] + extra);
	*flagp |= HASWIDTH | SIMPLE;
	ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ Magic('n'):
	ಆದರೆ (reg_string)
	{
	    // In a string "\n" matches a newline character.
	    ret = regnode(EXACTLY);
	    regc(NL);
	    regc(NUL);
	    *flagp |= HASWIDTH | SIMPLE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // In buffer text "\n" matches the end of a line.
	    ret = regnode(NEWL);
	    *flagp |= HASWIDTH | HASNL;
	}
	ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ Magic('('):
	ಆದರೆ (one_exactly)
	    EMSG_ONE_RET_NULL;
	ret = reg(REG_PAREN, &flags);
	ಆದರೆ (ret == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	*flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
	ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ NUL:
      ಸ್ಥಿತಿ Magic('|'):
      ಸ್ಥಿತಿ Magic('&'):
      ಸ್ಥಿತಿ Magic(')'):
	ಆದರೆ (one_exactly)
	    EMSG_ONE_RET_NULL;
	// Supposed to be caught earlier.
	IEMSG_RET_NULL(e_ಆಂತರಿಕ_error_ಒಳ_regexp);
	// NOTREACHED

      ಸ್ಥಿತಿ Magic('='):
      ಸ್ಥಿತಿ Magic('?'):
      ಸ್ಥಿತಿ Magic('+'):
      ಸ್ಥಿತಿ Magic('@'):
      ಸ್ಥಿತಿ Magic('{'):
      ಸ್ಥಿತಿ Magic('*'):
	c = no_Magic(c);
	EMSG3_RET_NULL(_(e_str_chr_follows_nothing),
		(c == '*' ? reg_magic >= MAGIC_ON : reg_magic == MAGIC_ALL), c);
	// NOTREACHED

      ಸ್ಥಿತಿ Magic('~'):		// previous substitute pattern
	    ಆದರೆ (reg_prev_sub != ಖಾಲಿ)
	    {
		char_u	    *lp;

		ret = regnode(EXACTLY);
		lp = reg_prev_sub;
		ಆಗ (*lp != NUL)
		    regc(*lp++);
		regc(NUL);
		ಆದರೆ (*reg_prev_sub != NUL)
		{
		    *flagp |= HASWIDTH;
		    ಆದರೆ ((lp - reg_prev_sub) == 1)
			*flagp |= SIMPLE;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		EMSG_RET_NULL(_(e_no_ಹಿಂದಿನ_substitute_regular_ಅಭಿವ್ಯಕ್ತಿ));
	    ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ Magic('1'):
      ಸ್ಥಿತಿ Magic('2'):
      ಸ್ಥಿತಿ Magic('3'):
      ಸ್ಥಿತಿ Magic('4'):
      ಸ್ಥಿತಿ Magic('5'):
      ಸ್ಥಿತಿ Magic('6'):
      ಸ್ಥಿತಿ Magic('7'):
      ಸ್ಥಿತಿ Magic('8'):
      ಸ್ಥಿತಿ Magic('9'):
	    {
		ಪೂರ್ಣಾಂಕ		    refnum;

		refnum = c - Magic('0');
		ಆದರೆ (!ನೋಡಿದ_endbrace(refnum))
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		ret = regnode(BACKREF + refnum);
	    }
	    ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ Magic('z'):
	{
	    c = no_Magic(getchr());
	    ಆಯ್ಕೆ (c)
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
		ಸ್ಥಿತಿ '(': ಆದರೆ ((reg_do_extmatch & REX_ಗಣ) == 0)
			      EMSG_RET_NULL(_(e_z_not_allowed_ಇಲ್ಲಿ));
			  ಆದರೆ (one_exactly)
			      EMSG_ONE_RET_NULL;
			  ret = reg(REG_ZPAREN, &flags);
			  ಆದರೆ (ret == ಖಾಲಿ)
			      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
			  *flagp |= flags & (HASWIDTH|SPSTART|HASNL|HASLOOKBH);
			  re_has_z = REX_ಗಣ;
			  ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ '1':
		ಸ್ಥಿತಿ '2':
		ಸ್ಥಿತಿ '3':
		ಸ್ಥಿತಿ '4':
		ಸ್ಥಿತಿ '5':
		ಸ್ಥಿತಿ '6':
		ಸ್ಥಿತಿ '7':
		ಸ್ಥಿತಿ '8':
		ಸ್ಥಿತಿ '9': ಆದರೆ ((reg_do_extmatch & REX_USE) == 0)
			      EMSG_RET_NULL(_(e_z1_z9_not_allowed_ಇಲ್ಲಿ));
			  ret = regnode(ZREF + c - '0');
			  re_has_z = REX_USE;
			  ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

		ಸ್ಥಿತಿ 's': ret = regnode(MOPEN + 0);
			  ಆದರೆ (re_mult_ಮುಂದಿನ("\\zs") == FAIL)
			      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
			  ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 'e': ret = regnode(MCLOSE + 0);
			  ಆದರೆ (re_mult_ಮುಂದಿನ("\\ze") == FAIL)
			      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
			  ನಿಲ್ಲಿಸು;

		ಪೂರ್ವನಿಯೋಜಿತ:  EMSG_RET_NULL(_(e_invalid_character_after_bsl_z));
	    }
	}
	ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ Magic('%'):
	{
	    c = no_Magic(getchr());
	    ಆಯ್ಕೆ (c)
	    {
		// () without a back reference
		ಸ್ಥಿತಿ '(':
		    ಆದರೆ (one_exactly)
			EMSG_ONE_RET_NULL;
		    ret = reg(REG_NPAREN, &flags);
		    ಆದರೆ (ret == ಖಾಲಿ)
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		    *flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
		    ನಿಲ್ಲಿಸು;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		ಸ್ಥಿತಿ '^':
		    ret = regnode(RE_BOF);
		    ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ '$':
		    ret = regnode(RE_EOF);
		    ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ '#':
		    ಆದರೆ (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_ಆರಂಭ_ದ_ನಮೂನೆ),
								  regparse[1]);
			ಹಿಂತಿರುಗಿ FAIL;
		    }
		    ret = regnode(CURSOR);
		    ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 'V':
		    ret = regnode(RE_VISUAL);
		    ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ 'C':
		    ret = regnode(RE_COMPOSING);
		    ನಿಲ್ಲಿಸು;

		// \%[abc]: Emit as a list of branches, all ending at the last
		// branch which matches nothing.
		ಸ್ಥಿತಿ '[':
			  ಆದರೆ (one_exactly)	// doesn't nest
			      EMSG_ONE_RET_NULL;
			  {
			      char_u	*lastbranch;
			      char_u	*lastnode = ಖಾಲಿ;
			      char_u	*br;

			      ret = ಖಾಲಿ;
			      ಆಗ ((c = getchr()) != ']')
			      {
				  ಆದರೆ (c == NUL)
				      EMSG2_RET_NULL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
				  br = regnode(BRANCH);
				  ಆದರೆ (ret == ಖಾಲಿ)
				      ret = br;
				  ಇಲ್ಲದಿದ್ದರೆ
				  {
				      regtail(lastnode, br);
				      ಆದರೆ (reg_toolong)
					  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
				  }

				  ungetchr();
				  one_exactly = TRUE;
				  lastnode = regatom(flagp);
				  one_exactly = FALSE;
				  ಆದರೆ (lastnode == ಖಾಲಿ)
				      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
			      }
			      ಆದರೆ (ret == ಖಾಲಿ)
				  EMSG2_RET_NULL(_(e_ಖಾಲಿಗಳು_str_brackets),
						      reg_magic == MAGIC_ALL);
			      lastbranch = regnode(BRANCH);
			      br = regnode(NOTHING);
			      ಆದರೆ (ret != JUST_CALC_ಗಾತ್ರ)
			      {
				  regtail(lastnode, br);
				  regtail(lastbranch, br);
				  // connect all branches to the NOTHING
				  // branch at the end
				  ಗಾಗಿ (br = ret; br != lastnode; )
				  {
				      ಆದರೆ (OP(br) == BRANCH)
				      {
					  regtail(br, lastbranch);
					  ಆದರೆ (reg_toolong)
					      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
					  br = ಕಾರ್ಯಾಂಶ(br);
				      }
				      ಇಲ್ಲದಿದ್ದರೆ
					  br = regnext(br);
				  }
			      }
			      *flagp &= ~(HASWIDTH | SIMPLE);
			      ನಿಲ್ಲಿಸು;
			  }

		ಸ್ಥಿತಿ 'd':   // %d123 decimal
		ಸ್ಥಿತಿ 'o':   // %o123 octal
		ಸ್ಥಿತಿ 'x':   // %xab hex 2
		ಸ್ಥಿತಿ 'u':   // %uabcd hex 4
		ಸ್ಥಿತಿ 'U':   // %U1234abcd hex 8
			  {
			      vimlong_T i;

			      ಆಯ್ಕೆ (c)
			      {
				  ಸ್ಥಿತಿ 'd': i = getdecchrs(); ನಿಲ್ಲಿಸು;
				  ಸ್ಥಿತಿ 'o': i = getoctchrs(); ನಿಲ್ಲಿಸು;
				  ಸ್ಥಿತಿ 'x': i = gethexchrs(2); ನಿಲ್ಲಿಸು;
				  ಸ್ಥಿತಿ 'u': i = gethexchrs(4); ನಿಲ್ಲಿಸು;
				  ಸ್ಥಿತಿ 'U': i = gethexchrs(8); ನಿಲ್ಲಿಸು;
				  ಪೂರ್ವನಿಯೋಜಿತ:  i = -1; ನಿಲ್ಲಿಸು;
			      }

			      ಆದರೆ (i < 0 || i > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
				  EMSG2_RET_NULL(
					    _(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			      ಆದರೆ (use_multibytecode(i))
				  ret = regnode(MULTIBYTECODE);
			      ಇಲ್ಲದಿದ್ದರೆ
				  ret = regnode(EXACTLY);
			      ಆದರೆ (i == 0)
				  regc(0x0a);
			      ಇಲ್ಲದಿದ್ದರೆ
				  regmbc((ಪೂರ್ಣಾಂಕ)i);
			      regc(NUL);
			      *flagp |= HASWIDTH;
			      ನಿಲ್ಲಿಸು;
			  }

		ಪೂರ್ವನಿಯೋಜಿತ:
			  ಆದರೆ (VIM_ISDIGIT(c) || c == '<' || c == '>'
						|| c == '\'' || c == '.')
			  {
			      long_u	n = 0;
			      ಪೂರ್ಣಾಂಕ	cmp;
			      ಪೂರ್ಣಾಂಕ	cur = FALSE;
			      ಪೂರ್ಣಾಂಕ	got_digit = FALSE;

			      cmp = c;
			      ಆದರೆ (cmp == '<' || cmp == '>')
				  c = getchr();
			      ಆದರೆ (no_Magic(c) == '.')
			      {
				  cur = TRUE;
				  c = getchr();
			      }
			      ಆಗ (VIM_ISDIGIT(c))
			      {
				  got_digit = TRUE;
				  n = n * 10 + (c - '0');
				  c = getchr();
			      }
			      ಆದರೆ (no_Magic(c) == '\'' && n == 0)
			      {
				  // "\%'m", "\%<'m" and "\%>'m": Mark
				  c = getchr();
				  ret = regnode(RE_MARK);
				  ಆದರೆ (ret == JUST_CALC_ಗಾತ್ರ)
				      regsize += 2;
				  ಇಲ್ಲದಿದ್ದರೆ
				  {
				      *regcode++ = c;
				      *regcode++ = cmp;
				  }
				  ನಿಲ್ಲಿಸು;
			      }
			      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((c == 'l' || c == 'c' || c == 'v')
					  && (cur || got_digit))
			      {
				  ಆದರೆ (cur && n)
				  {
				    semsg(_(e_regexp_ಸಂಖ್ಯೆ_after_dot_pos_ಹುಡುಕು_chr),
								  no_Magic(c));
				    rc_did_emsg = TRUE;
				    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
				  }
				  ಆದರೆ (c == 'l')
				  {
				      ಆದರೆ (cur)
					  n = curwin->w_cursor.lnum;
				      ret = regnode(RE_LNUM);
				      ಆದರೆ (save_prev_at_ಆರಂಭ)
					  at_ಆರಂಭ = TRUE;
				  }
				  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 'c')
				  {
				      ಆದರೆ (cur)
				      {
					  n = curwin->w_cursor.ಲಂಬಸಾಲು;
					  n++;
				      }
				      ret = regnode(RE_ಲಂಬಸಾಲು);
				  }
				  ಇಲ್ಲದಿದ್ದರೆ
				  {
				      ಆದರೆ (cur)
				      {
					  colnr_T vcol = 0;

					  getvvcol(curwin, &curwin->w_cursor,
							 ಖಾಲಿ, ಖಾಲಿ, &vcol, 0);
					  ++vcol;
					  n = vcol;
				      }
				      ret = regnode(RE_VCOL);
				  }
				  ಆದರೆ (ret == JUST_CALC_ಗಾತ್ರ)
				      regsize += 5;
				  ಇಲ್ಲದಿದ್ದರೆ
				  {
				      // put the number and the optional
				      // comparator after the opcode
				      regcode = re_put_long(regcode, n);
				      *regcode++ = cmp;
				  }
				  ನಿಲ್ಲಿಸು;
			      }
			  }

			  EMSG2_RET_NULL(_(e_invalid_character_after_str),
						      reg_magic == MAGIC_ALL);
	    }
	}
	ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ Magic('['):
collection:
	{
	    char_u	*lp;

	    // If there is no matching ']', we assume the '[' is a normal
	    // character.  This makes 'incsearch' and ":help [" work.
	    lp = skip_anyof(regparse);
	    ಆದರೆ (*lp == ']')	// there is a matching ']'
	    {
		ಪೂರ್ಣಾಂಕ	startc = -1;	// > 0 when next '-' is a range
		ಪೂರ್ಣಾಂಕ	endc;

		ಆದರೆ (extra == 0)
		{
		    ಪೂರ್ಣಾಂಕ	cl = bt_recognize_char_class(regparse, lp);

		    ಆದರೆ (cl != 0)
		    {
			ret = regnode(cl);
			regparse = lp;
			skipchr();
			*flagp |= HASWIDTH | SIMPLE;
			ನಿಲ್ಲಿಸು;
		    }
		}

		// In a character class, different parsing rules apply.
		// Not even \ is special anymore, nothing is.
		ಆದರೆ (*regparse == '^')	    // Complement of range.
		{
		    ret = regnode(ANYBUT + extra);
		    regparse++;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ret = regnode(ANYOF + extra);

		// At the start ']' and '-' mean the literal character.
		ಆದರೆ (*regparse == ']' || *regparse == '-')
		{
		    startc = *regparse;
		    regc(*regparse++);
		}

		ಆಗ (*regparse != NUL && *regparse != ']')
		{
		    ಆದರೆ (*regparse == '-')
		    {
			++regparse;
			// The '-' is not used for a range at the end and
			// after or before a '\n'.
			ಆದರೆ (*regparse == ']' || *regparse == NUL
				|| startc == -1
				|| (regparse[0] == '\\' && regparse[1] == 'n'))
			{
			    regc('-');
			    startc = '-';	// [--x] is a range
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    // Also accept "a-[.z.]"
			    endc = 0;
			    ಆದರೆ (*regparse == '[')
				endc = get_coll_element(&regparse);
			    ಆದರೆ (endc == 0)
			    {
				ಆದರೆ (has_mbyte)
				    endc = mb_ptr2char_adv(&regparse);
				ಇಲ್ಲದಿದ್ದರೆ
				    endc = *regparse++;
			    }

			    // Handle \o40, \x20 and \u20AC style sequences
			    ಆದರೆ (endc == '\\' && !reg_cpo_lit && !reg_cpo_bsl)
				endc = coll_get_char();

			    ಆದರೆ (startc > endc)
				EMSG_RET_NULL(_(e_ಹಿಮ್ಮುಖ_range_ಒಳ_character_class));
			    ಆದರೆ (has_mbyte && ((*mb_char2len)(startc) > 1
						 || (*mb_char2len)(endc) > 1))
			    {
				// Limit to a range of 256 chars.
				ಆದರೆ (endc > startc + 256)
				    EMSG_RET_NULL(_(e_range_too_large_ಒಳ_character_class));
				ಆಗ (++startc <= endc)
				    regmbc(startc);
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
			    {
				ಆಗ (++startc <= endc)
				    regc(startc);
			    }
			    startc = -1;
			}
		    }
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*regparse == '\\'
			    && !reg_cpo_bsl
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != ಖಾಲಿ
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR,
						       regparse[1]) != ಖಾಲಿ)))
		    {
			regparse++;
			ಆದರೆ (*regparse == 'n')
			{
			    // '\n' in range: also match NL
			    ಆದರೆ (ret != JUST_CALC_ಗಾತ್ರ)
			    {
				// Using \n inside [^] does not change what
				// matches. "[^\n]" is the same as ".".
				ಆದರೆ (*ret == ANYOF)
				{
				    *ret = ANYOF + ಕೂಡಿಸು_NL;
				    *flagp |= HASNL;
				}
				// else: must have had a \n already
			    }
			    regparse++;
			    startc = -1;
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*regparse == 'd'
				|| *regparse == 'o'
				|| *regparse == 'x'
				|| *regparse == 'u'
				|| *regparse == 'U')
			{
			    startc = coll_get_char();
			    // max UTF-8 Codepoint is U+10FFFF,
			    // but allow values until INT_MAX
			    ಆದರೆ (startc == ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
				EMSG_RET_NULL(_(e_unicode_val_too_large));
			    ಆದರೆ (startc == 0)
				regc(0x0a);
			    ಇಲ್ಲದಿದ್ದರೆ
				regmbc(startc);
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    startc = backslash_trans(*regparse++);
			    regc(startc);
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*regparse == '[')
		    {
			ಪೂರ್ಣಾಂಕ c_class;
			ಪೂರ್ಣಾಂಕ cu;

			c_class = get_char_class(&regparse);
			startc = -1;
			// Characters assumed to be 8 bits!
			ಆಯ್ಕೆ (c_class)
			{
			    ಸ್ಥಿತಿ CLASS_NONE:
				c_class = get_equi_class(&regparse);
				ಆದರೆ (c_class != 0)
				{
				    // produce equivalence class
				    reg_equi_class(c_class);
				}
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((c_class =
					    get_coll_element(&regparse)) != 0)
				{
				    // produce a collating element
				    regmbc(c_class);
				}
				ಇಲ್ಲದಿದ್ದರೆ
				{
				    // literal '[', allow [[-x] as a range
				    startc = *regparse++;
				    regc(startc);
				}
				ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ CLASS_ALNUM:
				ಗಾಗಿ (cu = 1; cu < 128; cu++)
				    ಆದರೆ (ಅಕ್ಷರವೇ_ಅಂಕಿಯೇ(cu))
					regmbc(cu);
				ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ CLASS_ALPHA:
				ಗಾಗಿ (cu = 1; cu < 128; cu++)
				    ಆದರೆ (ಅಕ್ಷರವೇ(cu))
					regmbc(cu);
				ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ CLASS_BLANK:
				regc(' ');
				regc('\t');
				ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ CLASS_CNTRL:
				ಗಾಗಿ (cu = 1; cu <= 127; cu++)
				    ಆದರೆ (iscntrl(cu))
					regmbc(cu);
				ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ CLASS_DIGIT:
				ಗಾಗಿ (cu = 1; cu <= 127; cu++)
				    ಆದರೆ (VIM_ISDIGIT(cu))
					regmbc(cu);
				ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ CLASS_ನಕ್ಷೆ:
				ಗಾಗಿ (cu = 1; cu <= 127; cu++)
				    ಆದರೆ (isgraph(cu))
					regmbc(cu);
				ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ CLASS_ಅಧೋ:
				ಗಾಗಿ (cu = 1; cu <= 255; cu++)
				    ಆದರೆ (MB_ISLOWER(cu) && cu != 170
								 && cu != 186)
					regmbc(cu);
				ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ CLASS_PRINT:
				ಗಾಗಿ (cu = 1; cu <= 255; cu++)
				    ಆದರೆ (vim_isprintc(cu))
					regmbc(cu);
				ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ CLASS_PUNCT:
				ಗಾಗಿ (cu = 1; cu < 128; cu++)
				    ಆದರೆ (ispunct(cu))
					regmbc(cu);
				ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ CLASS_SPACE:
				ಗಾಗಿ (cu = 9; cu <= 13; cu++)
				    regc(cu);
				regc(' ');
				ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ CLASS_ಊರ್ಧ್ವ:
				ಗಾಗಿ (cu = 1; cu <= 255; cu++)
				    ಆದರೆ (MB_ISUPPER(cu))
					regmbc(cu);
				ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ CLASS_XDIGIT:
				ಗಾಗಿ (cu = 1; cu <= 255; cu++)
				    ಆದರೆ (vim_isxdigit(cu))
					regmbc(cu);
				ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ CLASS_TAB:
				regc('\t');
				ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ CLASS_RETURN:
				regc('\r');
				ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ CLASS_BACKSPACE:
				regc('\b');
				ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ CLASS_ESCAPE:
				regc('\033');
				ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ CLASS_IDENT:
				ಗಾಗಿ (cu = 1; cu <= 255; cu++)
				    ಆದರೆ (vim_isIDc(cu))
					regmbc(cu);
				ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ CLASS_KEYWORD:
				ಗಾಗಿ (cu = 1; cu <= 255; cu++)
				    ಆದರೆ (reg_iswordc(cu))
					regmbc(cu);
				ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ CLASS_FNAME:
				ಗಾಗಿ (cu = 1; cu <= 255; cu++)
				    ಆದರೆ (vim_isfilec(cu))
					regmbc(cu);
				ನಿಲ್ಲಿಸು;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ (has_mbyte)
			{
			    ಪೂರ್ಣಾಂಕ	len;

			    // produce a multibyte character, including any
			    // following composing characters
			    startc = mb_ptr2char(regparse);
			    len = (*mb_ptr2len)(regparse);
			    ಆದರೆ (enc_utf8 && utf_char2len(startc) != len)
				startc = -1;	// composing chars
			    ಆಗ (--len >= 0)
				regc(*regparse++);
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    startc = *regparse++;
			    regc(startc);
			}
		    }
		}
		regc(NUL);
		prevchr_len = 1;	// last char was the ']'
		ಆದರೆ (*regparse != ']')
		    EMSG_RET_NULL(_(e_too_many_brackets));  // Cannot happen?
		skipchr();	    // let's be friends with the lexer again
		*flagp |= HASWIDTH | SIMPLE;
		ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (reg_strict)
		EMSG2_RET_NULL(_(e_missing_rsb_after_str_lsb),
							reg_magic > MAGIC_OFF);
	}
	// FALLTHROUGH

      ಪೂರ್ವನಿಯೋಜಿತ:
	{
	    ಪೂರ್ಣಾಂಕ		len;

	    // A multi-byte character is handled as a separate atom if it's
	    // before a multi and when it's a composing char.
	    ಆದರೆ (use_multibytecode(c))
	    {
do_multibyte:
		ret = regnode(MULTIBYTECODE);
		regmbc(c);
		*flagp |= HASWIDTH | SIMPLE;
		ನಿಲ್ಲಿಸು;
	    }

	    ret = regnode(EXACTLY);

	    // Append characters as long as:
	    // - there is no following multi, we then need the character in
	    //   front of it as a single character operand
	    // - not running into a Magic character
	    // - "one_exactly" is not set
	    // But always emit at least one character.  Might be a Multi,
	    // e.g., a "[" without matching "]".
	    ಗಾಗಿ (len = 0; c != NUL && (len == 0
			|| (re_multi_type(peekchr()) == NOT_MULTI
			    && !one_exactly
			    && !is_Magic(c))); ++len)
	    {
		c = no_Magic(c);
		ಆದರೆ (has_mbyte)
		{
		    regmbc(c);
		    ಆದರೆ (enc_utf8)
		    {
			ಪೂರ್ಣಾಂಕ	l;

			// Need to get composing character too.
			ಗಾಗಿ (;;)
			{
			    l = utf_ptr2len(regparse);
			    ಆದರೆ (!UTF_COMPOSINGLIKE(regparse, regparse + l))
				ನಿಲ್ಲಿಸು;
			    regmbc(utf_ptr2char(regparse));
			    skipchr();
			}
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    regc(c);
		c = getchr();
	    }
	    ungetchr();

	    regc(NUL);
	    *flagp |= HASWIDTH;
	    ಆದರೆ (len == 1)
		*flagp |= SIMPLE;
	}
	ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ ret;
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
regpiece(ಪೂರ್ಣಾಂಕ *flagp)
{
    char_u	    *ret;
    ಪೂರ್ಣಾಂಕ		    op;
    char_u	    *ಮುಂದಿನ;
    ಪೂರ್ಣಾಂಕ		    flags;
    ದೀರ್ಘ	    minval;
    ದೀರ್ಘ	    maxval;

    ret = regatom(&flags);
    ಆದರೆ (ret == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    op = peekchr();
    ಆದರೆ (re_multi_type(op) == NOT_MULTI)
    {
	*flagp = flags;
	ಹಿಂತಿರುಗಿ ret;
    }
    // default flags
    *flagp = (WORST | SPSTART | (flags & (HASNL | HASLOOKBH)));

    skipchr();
    ಆಯ್ಕೆ (op)
    {
	ಸ್ಥಿತಿ Magic('*'):
	    ಆದರೆ (flags & SIMPLE)
		reginsert(ನಕ್ಷತ್ರ, ret);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Emit x* as (x&|), where & means "self".
		reginsert(BRANCH, ret); // Either x
		regoptail(ret, regnode(ಹಿಂಭಾಗ_2));	// and loop
		regoptail(ret, ret);	//ಹಿಂಭಾಗ
		regtail(ret, regnode(BRANCH));	// or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Magic('+'):
	    ಆದರೆ (flags & SIMPLE)
		reginsert(PLUS, ret);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Emit x+ as x(&|), where & means "self".
		ಮುಂದಿನ = regnode(BRANCH); // Either
		regtail(ret, ಮುಂದಿನ);
		regtail(regnode(ಹಿಂಭಾಗ_2), ret);	// loop back
		regtail(ಮುಂದಿನ, regnode(BRANCH)); // or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    *flagp = (WORST | HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Magic('@'):
	    {
		ಪೂರ್ಣಾಂಕ	lop = ಕೊನೆ_3;
		ದೀರ್ಘ	nr;

		nr = (ದೀರ್ಘ)getdecchrs();
		ಆಯ್ಕೆ (no_Magic(getchr()))
		{
		    ಸ್ಥಿತಿ '=': lop = ಹೊಂದಿಕೆ_2; ನಿಲ್ಲಿಸು;		  // \@=
		    ಸ್ಥಿತಿ '!': lop = NOMATCH; ನಿಲ್ಲಿಸು;		  // \@!
		    ಸ್ಥಿತಿ '>': lop = SUBPAT; ನಿಲ್ಲಿಸು;		  // \@>
		    ಸ್ಥಿತಿ '<': ಆಯ್ಕೆ (no_Magic(getchr()))
			      {
				  ಸ್ಥಿತಿ '=': lop = BEHIND; ನಿಲ್ಲಿಸು;   // \@<=
				  ಸ್ಥಿತಿ '!': lop = NOBEHIND; ನಿಲ್ಲಿಸು; // \@<!
			      }
		}
		ಆದರೆ (lop == ಕೊನೆ_3)
		    EMSG2_RET_NULL(_(e_invalid_character_after_str_at),
						      reg_magic == MAGIC_ALL);
		// Look behind must match with behind_pos.
		ಆದರೆ (lop == BEHIND || lop == NOBEHIND)
		{
		    regtail(ret, regnode(BHPOS));
		    *flagp |= HASLOOKBH;
		}
		regtail(ret, regnode(ಕೊನೆ_3)); // operand ends
		ಆದರೆ (lop == BEHIND || lop == NOBEHIND)
		{
		    ಆದರೆ (nr < 0)
			nr = 0; // no limit is same as zero limit
		    reginsert_nr(lop, nr, ret);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    reginsert(lop, ret);
		ನಿಲ್ಲಿಸು;
	    }

	ಸ್ಥಿತಿ Magic('?'):
	ಸ್ಥಿತಿ Magic('='):
	    // Emit x= as (x|)
	    reginsert(BRANCH, ret);		// Either x
	    regtail(ret, regnode(BRANCH));	// or
	    ಮುಂದಿನ = regnode(NOTHING);		// null.
	    regtail(ret, ಮುಂದಿನ);
	    regoptail(ret, ಮುಂದಿನ);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Magic('{'):
	    ಆದರೆ (!read_limits(&minval, &maxval))
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    ಆದರೆ (flags & SIMPLE)
	    {
		reginsert(BRACE_SIMPLE, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (ಅಂಕೆ_complex_braces >= 10)
		    EMSG2_RET_NULL(_(e_too_many_complex_str_curly),
						      reg_magic == MAGIC_ALL);
		reginsert(BRACE_COMPLEX + ಅಂಕೆ_complex_braces, ret);
		regoptail(ret, regnode(ಹಿಂಭಾಗ_2));
		regoptail(ret, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
		++ಅಂಕೆ_complex_braces;
	    }
	    ಆದರೆ (minval > 0 && maxval > 0)
		*flagp = (HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ (re_multi_type(peekchr()) != NOT_MULTI)
    {
	// Can't have a multi follow a multi.
	ಆದರೆ (peekchr() == Magic('*'))
	    EMSG2_RET_NULL(_(e_nested_str), reg_magic >= MAGIC_ON);
	EMSG3_RET_NULL(_(e_nested_str_chr), reg_magic == MAGIC_ALL,
							  no_Magic(peekchr()));
    }

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Parse one alternative of an | or & operator.
 * Implements the concatenation operator.
 */
    ಸ್ಥಿರ char_u *
regconcat(ಪೂರ್ಣಾಂಕ *flagp)
{
    char_u	*ಮೊದಲ = ಖಾಲಿ;
    char_u	*chain = ಖಾಲಿ;
    char_u	*latest;
    ಪೂರ್ಣಾಂಕ		flags;
    ಪೂರ್ಣಾಂಕ		cont = TRUE;

    *flagp = WORST;		// Tentatively.

    ಆಗ (cont)
    {
	ಆಯ್ಕೆ (peekchr())
	{
	    ಸ್ಥಿತಿ NUL:
	    ಸ್ಥಿತಿ Magic('|'):
	    ಸ್ಥಿತಿ Magic('&'):
	    ಸ್ಥಿತಿ Magic(')'):
			    cont = FALSE;
			    ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ Magic('Z'):
			    regflags |= RF_ICOMBINE;
			    skipchr_keepstart();
			    ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ Magic('c'):
			    regflags |= RF_ICASE;
			    skipchr_keepstart();
			    ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ Magic('C'):
			    regflags |= RF_NOICASE;
			    skipchr_keepstart();
			    ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ Magic('v'):
			    reg_magic = MAGIC_ALL;
			    skipchr_keepstart();
			    curchr = -1;
			    ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ Magic('m'):
			    reg_magic = MAGIC_ON;
			    skipchr_keepstart();
			    curchr = -1;
			    ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ Magic('M'):
			    reg_magic = MAGIC_OFF;
			    skipchr_keepstart();
			    curchr = -1;
			    ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ Magic('V'):
			    reg_magic = MAGIC_NONE;
			    skipchr_keepstart();
			    curchr = -1;
			    ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:
			    latest = regpiece(&flags);
			    ಆದರೆ (latest == ಖಾಲಿ || reg_toolong)
				ಹಿಂತಿರುಗಿ ಖಾಲಿ;
			    *flagp |= flags & (HASWIDTH | HASNL | HASLOOKBH);
			    ಆದರೆ (chain == ಖಾಲಿ)	// First piece.
				*flagp |= flags & SPSTART;
			    ಇಲ್ಲದಿದ್ದರೆ
				regtail(chain, latest);
			    chain = latest;
			    ಆದರೆ (ಮೊದಲ == ಖಾಲಿ)
				ಮೊದಲ = latest;
			    ನಿಲ್ಲಿಸು;
	}
    }
    ಆದರೆ (ಮೊದಲ == ಖಾಲಿ)		// Loop ran zero times.
	ಮೊದಲ = regnode(NOTHING);
    ಹಿಂತಿರುಗಿ ಮೊದಲ;
}

/*
 * Parse one alternative of an | operator.
 * Implements the & operator.
 */
    ಸ್ಥಿರ char_u *
regbranch(ಪೂರ್ಣಾಂಕ *flagp)
{
    char_u	*ret;
    char_u	*chain = ಖಾಲಿ;
    char_u	*latest;
    ಪೂರ್ಣಾಂಕ		flags;

    *flagp = WORST | HASNL;		// Tentatively.

    ret = regnode(BRANCH);
    ಗಾಗಿ (;;)
    {
	latest = regconcat(&flags);
	ಆದರೆ (latest == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	// If one of the branches has width, the whole thing has.  If one of
	// the branches anchors at start-of-line, the whole thing does.
	// If one of the branches uses look-behind, the whole thing does.
	*flagp |= flags & (HASWIDTH | SPSTART | HASLOOKBH);
	// If one of the branches doesn't match a line-break, the whole thing
	// doesn't.
	*flagp &= ~HASNL | (flags & HASNL);
	ಆದರೆ (chain != ಖಾಲಿ)
	    regtail(chain, latest);
	ಆದರೆ (peekchr() != Magic('&'))
	    ನಿಲ್ಲಿಸು;
	skipchr();
	regtail(latest, regnode(ಕೊನೆ_3)); // operand ends
	ಆದರೆ (reg_toolong)
	    ನಿಲ್ಲಿಸು;
	reginsert(ಹೊಂದಿಕೆ_2, latest);
	chain = latest;
    }

    ಹಿಂತಿರುಗಿ ret;
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
    ಪೂರ್ಣಾಂಕ		paren,	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
    ಪೂರ್ಣಾಂಕ		*flagp)
{
    char_u	*ret;
    char_u	*br;
    char_u	*ender;
    ಪೂರ್ಣಾಂಕ		parno = 0;
    ಪೂರ್ಣಾಂಕ		flags;

    *flagp = HASWIDTH;		// Tentatively.

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ಆದರೆ (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	ಆದರೆ (regnzpar >= NSUBEXP)
	    EMSG_RET_NULL(_(e_too_many_z));
	parno = regnzpar;
	regnzpar++;
	ret = regnode(ZOPEN + parno);
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (paren == REG_PAREN)
    {
	// Make a MOPEN node.
	ಆದರೆ (regnpar >= NSUBEXP)
	    EMSG2_RET_NULL(_(e_too_many_str_open), reg_magic == MAGIC_ALL);
	parno = regnpar;
	++regnpar;
	ret = regnode(MOPEN + parno);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (paren == REG_NPAREN)
    {
	// Make a NOPEN node.
	ret = regnode(NOPEN);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ret = ಖಾಲಿ;

    ಆದರೆ (bt_reg_parse_ಆಳ >= REG_MAX_PAREN_ಆಳ)
	EMSG_RET_NULL(_(e_command_too_complex));
    ++bt_reg_parse_ಆಳ;

    // Pick up the branches, linking them together.
    br = regbranch(&flags);
    ಆದರೆ (br == ಖಾಲಿ)
    {
	ret = ಖಾಲಿ;
	ಹೋಗು theend;
    }
    ಆದರೆ (ret != ಖಾಲಿ)
	regtail(ret, br);	// [MZ]OPEN -> first.
    ಇಲ್ಲದಿದ್ದರೆ
	ret = br;
    // If one of the branches can be zero-width, the whole thing can.
    // If one of the branches has * at start or matches a line-break, the
    // whole thing can.
    ಆದರೆ (!(flags & HASWIDTH))
	*flagp &= ~HASWIDTH;
    *flagp |= flags & (SPSTART | HASNL | HASLOOKBH);
    ಆಗ (peekchr() == Magic('|'))
    {
	skipchr();
	br = regbranch(&flags);
	ಆದರೆ (br == ಖಾಲಿ || reg_toolong)
	{
	    ret = ಖಾಲಿ;
	    ಹೋಗು theend;
	}
	regtail(ret, br);	// BRANCH -> BRANCH.
	ಆದರೆ (!(flags & HASWIDTH))
	    *flagp &= ~HASWIDTH;
	*flagp |= flags & (SPSTART | HASNL | HASLOOKBH);
    }

    // Make a closing node, and hook it on the end.
    ender = regnode(
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	    paren == REG_ZPAREN ? ZCLOSE + parno :
#ಆದರೆ_ಅಂತ್ಯ
	    paren == REG_PAREN ? MCLOSE + parno :
	    paren == REG_NPAREN ? NCLOSE : ಕೊನೆ_3);
    regtail(ret, ender);

    // Hook the tails of the branches to the closing node.
    ಗಾಗಿ (br = ret; br != ಖಾಲಿ; br = regnext(br))
	regoptail(br, ender);

    // Check for proper termination.
    ಆದರೆ (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	ಆದರೆ (paren == REG_ZPAREN)
	{
	    emsg(_(e_unmatched_z));
	    rc_did_emsg = TRUE;
	    ret = ಖಾಲಿ;
	    ಹೋಗು theend;
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (paren == REG_NPAREN)
	{
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = ಖಾಲಿ;
	    ಹೋಗು theend;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = ಖಾಲಿ;
	    ಹೋಗು theend;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (paren == REG_NOPAREN && peekchr() != NUL)
    {
	ಆದರೆ (curchr == Magic(')'))
	{
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = ಖಾಲಿ;
	    ಹೋಗು theend;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    emsg(_(e_trailing_characters));	// "Can't happen".
	    rc_did_emsg = TRUE;
	    ret = ಖಾಲಿ;
	    ಹೋಗು theend;
	}
    }
    // Here we set the flag allowing back references to this set of
    // parentheses.
    ಆದರೆ (paren == REG_PAREN)
	had_endbrace[parno] = TRUE;	// have seen the close paren

theend:
    --bt_reg_parse_ಆಳ;
    ಹಿಂತಿರುಗಿ ret;
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
bt_regcomp(char_u *ಅಭಿವ್ಯಕ್ತಿ, ಪೂರ್ಣಾಂಕ re_flags)
{
    bt_regprog_T    *r;
    char_u	*ಪರಿಶೀಲನೆ;
    char_u	*longest;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		flags;

    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ == ಖಾಲಿ)
	IEMSG_RET_NULL(e_null_ವಾದ);

    init_class_tab();

    // First pass: determine size, legality.
    regcomp_ಆರಂಭ(ಅಭಿವ್ಯಕ್ತಿ, re_flags);
    regcode = JUST_CALC_ಗಾತ್ರ;
    regc(REGMAGIC);
    ಆದರೆ (reg(REG_NOPAREN, &flags) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Allocate space.
    r = alloc(offsetof(bt_regprog_T, ಕಾರ್ಯಕ್ರಮ) + regsize);
    ಆದರೆ (r == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    r->re_ಒಳ_use = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
    r->regsz = regsize;
#ಆದರೆ_ಅಂತ್ಯ

    // Second pass: emit code.
    regcomp_ಆರಂಭ(ಅಭಿವ್ಯಕ್ತಿ, re_flags);
    regcode = r->ಕಾರ್ಯಕ್ರಮ;
    regc(REGMAGIC);
    ಆದರೆ (reg(REG_NOPAREN, &flags) == ಖಾಲಿ || reg_toolong)
    {
	vim_free(r);
	ಆದರೆ (reg_toolong)
	    EMSG_RET_NULL(_(e_ನಮೂನೆ_too_long));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // Dig out information for optimizations.
    r->regstart = NUL;		// Worst-case defaults.
    r->reganch = 0;
    r->regmust = ಖಾಲಿ;
    r->regmlen = 0;
    r->regflags = regflags;
    ಆದರೆ (flags & HASNL)
	r->regflags |= RF_HASNL;
    ಆದರೆ (flags & HASLOOKBH)
	r->regflags |= RF_LOOKBH;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    r->reghasz = re_has_z;
#ಆದರೆ_ಅಂತ್ಯ
    ಪರಿಶೀಲನೆ = r->ಕಾರ್ಯಕ್ರಮ + 1;	// First BRANCH.
    ಆದರೆ (OP(regnext(ಪರಿಶೀಲನೆ)) == ಕೊನೆ_3)   // Only one top-level choice.
    {
	ಪರಿಶೀಲನೆ = ಕಾರ್ಯಾಂಶ(ಪರಿಶೀಲನೆ);

	// Starting-point info.
	ಆದರೆ (OP(ಪರಿಶೀಲನೆ) == BOL || OP(ಪರಿಶೀಲನೆ) == RE_BOF)
	{
	    r->reganch++;
	    ಪರಿಶೀಲನೆ = regnext(ಪರಿಶೀಲನೆ);
	}

	ಆದರೆ (OP(ಪರಿಶೀಲನೆ) == EXACTLY)
	{
	    ಆದರೆ (has_mbyte)
		r->regstart = (*mb_ptr2char)(ಕಾರ್ಯಾಂಶ(ಪರಿಶೀಲನೆ));
	    ಇಲ್ಲದಿದ್ದರೆ
		r->regstart = *ಕಾರ್ಯಾಂಶ(ಪರಿಶೀಲನೆ);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((OP(ಪರಿಶೀಲನೆ) == BOW
		    || OP(ಪರಿಶೀಲನೆ) == EOW
		    || OP(ಪರಿಶೀಲನೆ) == NOTHING
		    || OP(ಪರಿಶೀಲನೆ) == MOPEN + 0 || OP(ಪರಿಶೀಲನೆ) == NOPEN
		    || OP(ಪರಿಶೀಲನೆ) == MCLOSE + 0 || OP(ಪರಿಶೀಲನೆ) == NCLOSE)
		 && OP(regnext(ಪರಿಶೀಲನೆ)) == EXACTLY)
	{
	    ಆದರೆ (has_mbyte)
		r->regstart = (*mb_ptr2char)(ಕಾರ್ಯಾಂಶ(regnext(ಪರಿಶೀಲನೆ)));
	    ಇಲ್ಲದಿದ್ದರೆ
		r->regstart = *ಕಾರ್ಯಾಂಶ(regnext(ಪರಿಶೀಲನೆ));
	}

	// If there's something expensive in the r.e., find the longest
	// literal string that must appear and make it the regmust.  Resolve
	// ties in favor of later strings, since the regstart check works
	// with the beginning of the r.e. and avoiding duplication
	// strengthens checking.  Not a strong reason, but sufficient in the
	// absence of others.

	// When the r.e. starts with BOW, it is faster to look for a regmust
	// first. Used a lot for "#" and "*" commands. (Added by mool).
	ಆದರೆ ((flags & SPSTART || OP(ಪರಿಶೀಲನೆ) == BOW || OP(ಪರಿಶೀಲನೆ) == EOW)
							  && !(flags & HASNL))
	{
	    size_t  scanlen;

	    longest = ಖಾಲಿ;
	    len = 0;
	    ಗಾಗಿ (; ಪರಿಶೀಲನೆ != ಖಾಲಿ; ಪರಿಶೀಲನೆ = regnext(ಪರಿಶೀಲನೆ))
	    {
		ಆದರೆ (OP(ಪರಿಶೀಲನೆ) == EXACTLY)
		{
		    scanlen = STRLEN(ಕಾರ್ಯಾಂಶ(ಪರಿಶೀಲನೆ));
		    ಆದರೆ (scanlen >= (size_t)len)
		    {
			longest = ಕಾರ್ಯಾಂಶ(ಪರಿಶೀಲನೆ);
			len = (ಪೂರ್ಣಾಂಕ)scanlen;
		    }
		}
	    }
	    r->regmust = longest;
	    r->regmlen = len;
	}
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BT_REGEXP_DUMP
    regdump(ಅಭಿವ್ಯಕ್ತಿ, r);
#ಆದರೆ_ಅಂತ್ಯ
    r->engine = &bt_regengine;
    ಹಿಂತಿರುಗಿ (regprog_T *)r;
}

#ಆದರೆ defined(FEAT_SYN_HL)
/*
 * Check if during the previous call to vim_regcomp the EOL item "$" has been
 * found.  This is messy, but it works fine.
 */
    ಪೂರ್ಣಾಂಕ
vim_regcomp_had_eol(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ had_eol;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Get a number after a backslash that is inside [].
 * When nothing is recognized return a backslash.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
coll_get_char(ಶೂನ್ಯ)
{
    vimlong_T	nr = -1;

    ಆಯ್ಕೆ (*regparse++)
    {
	ಸ್ಥಿತಿ 'd': nr = getdecchrs(); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'o': nr = getoctchrs(); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'x': nr = gethexchrs(2); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'u': nr = gethexchrs(4); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'U': nr = gethexchrs(8); ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ (nr < 0)
    {
	// If getting the number fails be backwards compatible: the character
	// is a backslash.
	--regparse;
	nr = '\\';
    }
    ಆದರೆ (nr > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
	nr = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
    ಹಿಂತಿರುಗಿ nr;
}

/*
 * Free a compiled regexp program, returned by bt_regcomp().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
bt_regfree(regprog_T *prog)
{
    vim_free(prog);
}

#ವ್ಯಾಖ್ಯಾನಿಸು ADVANCE_REGINPUT() MB_PTR_ADV(rex.ಒಳಹರಿವು)

/*
 * The arguments from BRACE_LIMITS are stored here.  They are actually local
 * to regmatch(), but they are here to reduce the amount of stack space used
 * (it can be called recursively many times).
 */
ಸ್ಥಿರ ದೀರ್ಘ	bl_minval;
ಸ್ಥಿರ ದೀರ್ಘ	bl_maxval;

/*
 * Save the input line and position in a regsave_T.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
reg_save(regsave_T *save, garray_T *gap)
{
    ಆದರೆ (REG_MULTI)
    {
	save->rs_u.pos.ಲಂಬಸಾಲು = (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು);
	save->rs_u.pos.lnum = rex.lnum;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	save->rs_u.ptr = rex.ಒಳಹರಿವು;
    save->rs_len = gap->ga_len;
}

/*
 * Restore the input line and position from a regsave_T.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
reg_restore(regsave_T *save, garray_T *gap)
{
    ಆದರೆ (REG_MULTI)
    {
	ಆದರೆ (rex.lnum != save->rs_u.pos.lnum)
	{
	    // only call reg_getline() when the line number changed to save
	    // a bit of time
	    rex.lnum = save->rs_u.pos.lnum;
	    rex.ಸಾಲು = reg_getline(rex.lnum);
	}
	rex.ಒಳಹರಿವು = rex.ಸಾಲು + save->rs_u.pos.ಲಂಬಸಾಲು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	rex.ಒಳಹರಿವು = save->rs_u.ptr;
    gap->ga_len = save->rs_len;
}

/*
 * Return TRUE if current position is equal to saved position.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
reg_save_equal(regsave_T *save)
{
    ಆದರೆ (REG_MULTI)
	ಹಿಂತಿರುಗಿ rex.lnum == save->rs_u.pos.lnum
				  && rex.ಒಳಹರಿವು == rex.ಸಾಲು + save->rs_u.pos.ಲಂಬಸಾಲು;
    ಹಿಂತಿರುಗಿ rex.ಒಳಹರಿವು == save->rs_u.ptr;
}

// Save the sub-expressions before attempting a match.
#ವ್ಯಾಖ್ಯಾನಿಸು save_se(savep, posp, pp) \
    REG_MULTI ? save_se_multi((savep), (posp)) : save_se_one((savep), (pp))

// After a failed match restore the sub-expressions.
#ವ್ಯಾಖ್ಯಾನಿಸು restore_se(savep, posp, pp) \
{ \
    ಆದರೆ (REG_MULTI) \
	*(posp) = (savep)->se_u.pos; \
    ಇಲ್ಲದಿದ್ದರೆ \
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
    posp->ಲಂಬಸಾಲು = (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು);
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
regrepeat(
    char_u	*p,
    ದೀರ್ಘ	maxcount)   // maximum number of matches allowed
{
    ದೀರ್ಘ	ಎಣಿಕೆ_2 = 0;
    char_u	*ಪರಿಶೀಲನೆ;
    char_u	*opnd;
    ಪೂರ್ಣಾಂಕ		ಮುಖವಾಡ;
    ಪೂರ್ಣಾಂಕ		testval = 0;

    ಪರಿಶೀಲನೆ = rex.ಒಳಹರಿವು;	    // Make local copy of rex.input for speed.
    opnd = ಕಾರ್ಯಾಂಶ(p);
    ಆಯ್ಕೆ (OP(p))
    {
      ಸ್ಥಿತಿ ANY:
      ಸ್ಥಿತಿ ANY + ಕೂಡಿಸು_NL:
	ಆಗ (ಎಣಿಕೆ_2 < maxcount)
	{
	    // Matching anything means we continue until end-of-line (or
	    // end-of-file for ANY + ADD_NL), only limited by maxcount.
	    ಆಗ (*ಪರಿಶೀಲನೆ != NUL && ಎಣಿಕೆ_2 < maxcount)
	    {
		++ಎಣಿಕೆ_2;
		MB_PTR_ADV(ಪರಿಶೀಲನೆ);
	    }
	    ಆದರೆ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
				      || rex.reg_ಸಾಲು_lbr || ಎಣಿಕೆ_2 == maxcount)
		ನಿಲ್ಲಿಸು;
	    ++ಎಣಿಕೆ_2;		// count the line-break
	    reg_nextline();
	    ಪರಿಶೀಲನೆ = rex.ಒಳಹರಿವು;
	    ಆದರೆ (got_int)
		ನಿಲ್ಲಿಸು;
	}
	ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ IDENT:
      ಸ್ಥಿತಿ IDENT + ಕೂಡಿಸು_NL:
	testval = TRUE;
	// FALLTHROUGH
      ಸ್ಥಿತಿ SIDENT:
      ಸ್ಥಿತಿ SIDENT + ಕೂಡಿಸು_NL:
	ಆಗ (ಎಣಿಕೆ_2 < maxcount)
	{
	    ಆದರೆ (vim_isIDc(PTR2CHAR(ಪರಿಶೀಲನೆ)) && (testval || !VIM_ISDIGIT(*ಪರಿಶೀಲನೆ)))
	    {
		MB_PTR_ADV(ಪರಿಶೀಲನೆ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ಪರಿಶೀಲನೆ == NUL)
	    {
		ಆದರೆ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ಸಾಲು_lbr)
		    ನಿಲ್ಲಿಸು;
		reg_nextline();
		ಪರಿಶೀಲನೆ = rex.ಒಳಹರಿವು;
		ಆದರೆ (got_int)
		    ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rex.reg_ಸಾಲು_lbr && *ಪರಿಶೀಲನೆ == '\n' && WITH_NL(OP(p)))
		++ಪರಿಶೀಲನೆ;
	    ಇಲ್ಲದಿದ್ದರೆ
		ನಿಲ್ಲಿಸು;
	    ++ಎಣಿಕೆ_2;
	}
	ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ KWORD:
      ಸ್ಥಿತಿ KWORD + ಕೂಡಿಸು_NL:
	testval = TRUE;
	// FALLTHROUGH
      ಸ್ಥಿತಿ SKWORD:
      ಸ್ಥಿತಿ SKWORD + ಕೂಡಿಸು_NL:
	ಆಗ (ಎಣಿಕೆ_2 < maxcount)
	{
	    ಆದರೆ (vim_iswordp_buf(ಪರಿಶೀಲನೆ, rex.reg_buf)
					  && (testval || !VIM_ISDIGIT(*ಪರಿಶೀಲನೆ)))
	    {
		MB_PTR_ADV(ಪರಿಶೀಲನೆ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ಪರಿಶೀಲನೆ == NUL)
	    {
		ಆದರೆ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ಸಾಲು_lbr)
		    ನಿಲ್ಲಿಸು;
		reg_nextline();
		ಪರಿಶೀಲನೆ = rex.ಒಳಹರಿವು;
		ಆದರೆ (got_int)
		    ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rex.reg_ಸಾಲು_lbr && *ಪರಿಶೀಲನೆ == '\n' && WITH_NL(OP(p)))
		++ಪರಿಶೀಲನೆ;
	    ಇಲ್ಲದಿದ್ದರೆ
		ನಿಲ್ಲಿಸು;
	    ++ಎಣಿಕೆ_2;
	}
	ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ FNAME:
      ಸ್ಥಿತಿ FNAME + ಕೂಡಿಸು_NL:
	testval = TRUE;
	// FALLTHROUGH
      ಸ್ಥಿತಿ SFNAME:
      ಸ್ಥಿತಿ SFNAME + ಕೂಡಿಸು_NL:
	ಆಗ (ಎಣಿಕೆ_2 < maxcount)
	{
	    ಆದರೆ (vim_isfilec(PTR2CHAR(ಪರಿಶೀಲನೆ)) && (testval || !VIM_ISDIGIT(*ಪರಿಶೀಲನೆ)))
	    {
		MB_PTR_ADV(ಪರಿಶೀಲನೆ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ಪರಿಶೀಲನೆ == NUL)
	    {
		ಆದರೆ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ಸಾಲು_lbr)
		    ನಿಲ್ಲಿಸು;
		reg_nextline();
		ಪರಿಶೀಲನೆ = rex.ಒಳಹರಿವು;
		ಆದರೆ (got_int)
		    ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rex.reg_ಸಾಲು_lbr && *ಪರಿಶೀಲನೆ == '\n' && WITH_NL(OP(p)))
		++ಪರಿಶೀಲನೆ;
	    ಇಲ್ಲದಿದ್ದರೆ
		ನಿಲ್ಲಿಸು;
	    ++ಎಣಿಕೆ_2;
	}
	ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ PRINT:
      ಸ್ಥಿತಿ PRINT + ಕೂಡಿಸು_NL:
	testval = TRUE;
	// FALLTHROUGH
      ಸ್ಥಿತಿ SPRINT:
      ಸ್ಥಿತಿ SPRINT + ಕೂಡಿಸು_NL:
	ಆಗ (ಎಣಿಕೆ_2 < maxcount)
	{
	    ಆದರೆ (*ಪರಿಶೀಲನೆ == NUL)
	    {
		ಆದರೆ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ಸಾಲು_lbr)
		    ನಿಲ್ಲಿಸು;
		reg_nextline();
		ಪರಿಶೀಲನೆ = rex.ಒಳಹರಿವು;
		ಆದರೆ (got_int)
		    ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_isprintc(PTR2CHAR(ಪರಿಶೀಲನೆ)) == 1
					  && (testval || !VIM_ISDIGIT(*ಪರಿಶೀಲನೆ)))
	    {
		MB_PTR_ADV(ಪರಿಶೀಲನೆ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rex.reg_ಸಾಲು_lbr && *ಪರಿಶೀಲನೆ == '\n' && WITH_NL(OP(p)))
		++ಪರಿಶೀಲನೆ;
	    ಇಲ್ಲದಿದ್ದರೆ
		ನಿಲ್ಲಿಸು;
	    ++ಎಣಿಕೆ_2;
	}
	ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ WHITE:
      ಸ್ಥಿತಿ WHITE + ಕೂಡಿಸು_NL:
	testval = ಮುಖವಾಡ = RI_WHITE;
do_class:
	ಆಗ (ಎಣಿಕೆ_2 < maxcount)
	{
	    ಪೂರ್ಣಾಂಕ		l;

	    ಆದರೆ (*ಪರಿಶೀಲನೆ == NUL)
	    {
		ಆದರೆ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ಸಾಲು_lbr)
		    ನಿಲ್ಲಿಸು;
		reg_nextline();
		ಪರಿಶೀಲನೆ = rex.ಒಳಹರಿವು;
		ಆದರೆ (got_int)
		    ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte && (l = (*mb_ptr2len)(ಪರಿಶೀಲನೆ)) > 1)
	    {
		ಆದರೆ (testval != 0)
		    ನಿಲ್ಲಿಸು;
		ಪರಿಶೀಲನೆ += l;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((class_tab[*ಪರಿಶೀಲನೆ] & ಮುಖವಾಡ) == testval)
		++ಪರಿಶೀಲನೆ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rex.reg_ಸಾಲು_lbr && *ಪರಿಶೀಲನೆ == '\n' && WITH_NL(OP(p)))
		++ಪರಿಶೀಲನೆ;
	    ಇಲ್ಲದಿದ್ದರೆ
		ನಿಲ್ಲಿಸು;
	    ++ಎಣಿಕೆ_2;
	}
	ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ NWHITE:
      ಸ್ಥಿತಿ NWHITE + ಕೂಡಿಸು_NL:
	ಮುಖವಾಡ = RI_WHITE;
	ಹೋಗು do_class;
      ಸ್ಥಿತಿ DIGIT:
      ಸ್ಥಿತಿ DIGIT + ಕೂಡಿಸು_NL:
	testval = ಮುಖವಾಡ = RI_DIGIT;
	ಹೋಗು do_class;
      ಸ್ಥಿತಿ NDIGIT:
      ಸ್ಥಿತಿ NDIGIT + ಕೂಡಿಸು_NL:
	ಮುಖವಾಡ = RI_DIGIT;
	ಹೋಗು do_class;
      ಸ್ಥಿತಿ HEX:
      ಸ್ಥಿತಿ HEX + ಕೂಡಿಸು_NL:
	testval = ಮುಖವಾಡ = RI_HEX;
	ಹೋಗು do_class;
      ಸ್ಥಿತಿ NHEX:
      ಸ್ಥಿತಿ NHEX + ಕೂಡಿಸು_NL:
	ಮುಖವಾಡ = RI_HEX;
	ಹೋಗು do_class;
      ಸ್ಥಿತಿ OCTAL:
      ಸ್ಥಿತಿ OCTAL + ಕೂಡಿಸು_NL:
	testval = ಮುಖವಾಡ = RI_OCTAL;
	ಹೋಗು do_class;
      ಸ್ಥಿತಿ NOCTAL:
      ಸ್ಥಿತಿ NOCTAL + ಕೂಡಿಸು_NL:
	ಮುಖವಾಡ = RI_OCTAL;
	ಹೋಗು do_class;
      ಸ್ಥಿತಿ ಶಬ್ದ_2:
      ಸ್ಥಿತಿ ಶಬ್ದ_2 + ಕೂಡಿಸು_NL:
	testval = ಮುಖವಾಡ = RI_ಶಬ್ದ;
	ಹೋಗು do_class;
      ಸ್ಥಿತಿ NWORD:
      ಸ್ಥಿತಿ NWORD + ಕೂಡಿಸು_NL:
	ಮುಖವಾಡ = RI_ಶಬ್ದ;
	ಹೋಗು do_class;
      ಸ್ಥಿತಿ ತಲೆ_2:
      ಸ್ಥಿತಿ ತಲೆ_2 + ಕೂಡಿಸು_NL:
	testval = ಮುಖವಾಡ = RI_ತಲೆ;
	ಹೋಗು do_class;
      ಸ್ಥಿತಿ NHEAD:
      ಸ್ಥಿತಿ NHEAD + ಕೂಡಿಸು_NL:
	ಮುಖವಾಡ = RI_ತಲೆ;
	ಹೋಗು do_class;
      ಸ್ಥಿತಿ ALPHA:
      ಸ್ಥಿತಿ ALPHA + ಕೂಡಿಸು_NL:
	testval = ಮುಖವಾಡ = RI_ALPHA;
	ಹೋಗು do_class;
      ಸ್ಥಿತಿ NALPHA:
      ಸ್ಥಿತಿ NALPHA + ಕೂಡಿಸು_NL:
	ಮುಖವಾಡ = RI_ALPHA;
	ಹೋಗು do_class;
      ಸ್ಥಿತಿ ಅಧೋ_2:
      ಸ್ಥಿತಿ ಅಧೋ_2 + ಕೂಡಿಸು_NL:
	testval = ಮುಖವಾಡ = RI_ಅಧೋ;
	ಹೋಗು do_class;
      ಸ್ಥಿತಿ NLOWER:
      ಸ್ಥಿತಿ NLOWER + ಕೂಡಿಸು_NL:
	ಮುಖವಾಡ = RI_ಅಧೋ;
	ಹೋಗು do_class;
      ಸ್ಥಿತಿ ಊರ್ಧ್ವ:
      ಸ್ಥಿತಿ ಊರ್ಧ್ವ + ಕೂಡಿಸು_NL:
	testval = ಮುಖವಾಡ = RI_ಊರ್ಧ್ವ;
	ಹೋಗು do_class;
      ಸ್ಥಿತಿ NUPPER:
      ಸ್ಥಿತಿ NUPPER + ಕೂಡಿಸು_NL:
	ಮುಖವಾಡ = RI_ಊರ್ಧ್ವ;
	ಹೋಗು do_class;

      ಸ್ಥಿತಿ EXACTLY:
	{
	    ಪೂರ್ಣಾಂಕ	    cu, cl;

	    // This doesn't do a multi-byte character, because a MULTIBYTECODE
	    // would have been used for it.  It does handle single-byte
	    // characters, such as latin1.
	    ಆದರೆ (rex.reg_ic)
	    {
		cu = MB_TOUPPER(*opnd);
		cl = MB_TOLOWER(*opnd);
		ಆಗ (ಎಣಿಕೆ_2 < maxcount && (*ಪರಿಶೀಲನೆ == cu || *ಪರಿಶೀಲನೆ == cl))
		{
		    ಎಣಿಕೆ_2++;
		    ಪರಿಶೀಲನೆ++;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		cu = *opnd;
		ಆಗ (ಎಣಿಕೆ_2 < maxcount && *ಪರಿಶೀಲನೆ == cu)
		{
		    ಎಣಿಕೆ_2++;
		    ಪರಿಶೀಲನೆ++;
		}
	    }
	    ನಿಲ್ಲಿಸು;
	}

      ಸ್ಥಿತಿ MULTIBYTECODE:
	{
	    ಪೂರ್ಣಾಂಕ		i, len, cf = 0;

	    // Safety check (just in case 'encoding' was changed since
	    // compiling the program).
	    ಆದರೆ ((len = (*mb_ptr2len)(opnd)) > 1)
	    {
		ಆದರೆ (rex.reg_ic && enc_utf8)
		    cf = utf_fold(utf_ptr2char(opnd));
		ಆಗ (ಎಣಿಕೆ_2 < maxcount && (*mb_ptr2len)(ಪರಿಶೀಲನೆ) >= len)
		{
		    ಗಾಗಿ (i = 0; i < len; ++i)
			ಆದರೆ (opnd[i] != ಪರಿಶೀಲನೆ[i])
			    ನಿಲ್ಲಿಸು;
		    ಆದರೆ (i < len && (!rex.reg_ic || !enc_utf8
					|| utf_fold(utf_ptr2char(ಪರಿಶೀಲನೆ)) != cf))
			ನಿಲ್ಲಿಸು;
		    ಪರಿಶೀಲನೆ += len;
		    ++ಎಣಿಕೆ_2;
		}
	    }
	}
	ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ ANYOF:
      ಸ್ಥಿತಿ ANYOF + ಕೂಡಿಸು_NL:
	testval = TRUE;
	// FALLTHROUGH

      ಸ್ಥಿತಿ ANYBUT:
      ಸ್ಥಿತಿ ANYBUT + ಕೂಡಿಸು_NL:
	ಆಗ (ಎಣಿಕೆ_2 < maxcount)
	{
	    ಪೂರ್ಣಾಂಕ len;

	    ಆದರೆ (*ಪರಿಶೀಲನೆ == NUL)
	    {
		ಆದರೆ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ಸಾಲು_lbr)
		    ನಿಲ್ಲಿಸು;
		reg_nextline();
		ಪರಿಶೀಲನೆ = rex.ಒಳಹರಿವು;
		ಆದರೆ (got_int)
		    ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rex.reg_ಸಾಲು_lbr && *ಪರಿಶೀಲನೆ == '\n' && WITH_NL(OP(p)))
		++ಪರಿಶೀಲನೆ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte && (len = (*mb_ptr2len)(ಪರಿಶೀಲನೆ)) > 1)
	    {
		ಆದರೆ ((cstrchr(opnd, (*mb_ptr2char)(ಪರಿಶೀಲನೆ)) == ಖಾಲಿ) == testval)
		    ನಿಲ್ಲಿಸು;
		ಪರಿಶೀಲನೆ += len;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ ((cstrchr(opnd, *ಪರಿಶೀಲನೆ) == ಖಾಲಿ) == testval)
		    ನಿಲ್ಲಿಸು;
		++ಪರಿಶೀಲನೆ;
	    }
	    ++ಎಣಿಕೆ_2;
	}
	ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ NEWL:
	ಆಗ (ಎಣಿಕೆ_2 < maxcount
		&& ((*ಪರಿಶೀಲನೆ == NUL && rex.lnum <= rex.reg_maxline
				       && !rex.reg_ಸಾಲು_lbr && REG_MULTI)
		    || (*ಪರಿಶೀಲನೆ == '\n' && rex.reg_ಸಾಲು_lbr)))
	{
	    ಎಣಿಕೆ_2++;
	    ಆದರೆ (rex.reg_ಸಾಲು_lbr)
		ADVANCE_REGINPUT();
	    ಇಲ್ಲದಿದ್ದರೆ
		reg_nextline();
	    ಪರಿಶೀಲನೆ = rex.ಒಳಹರಿವು;
	    ಆದರೆ (got_int)
		ನಿಲ್ಲಿಸು;
	}
	ನಿಲ್ಲಿಸು;

      ಪೂರ್ವನಿಯೋಜಿತ:			// Oh dear.  Called inappropriately.
	iemsg(e_corrupted_regexp_ಕಾರ್ಯಕ್ರಮ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
	ಬರೆ("Called regrepeat with op code %d\n", OP(p));
#ಆದರೆ_ಅಂತ್ಯ
	ನಿಲ್ಲಿಸು;
    }

    rex.ಒಳಹರಿವು = ಪರಿಶೀಲನೆ;

    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)ಎಣಿಕೆ_2;
}

/*
 * Push an item onto the regstack.
 * Returns pointer to new item.  Returns NULL when out of memory.
 */
    ಸ್ಥಿರ regitem_T *
regstack_ತಳ್ಳು(regstate_T state, char_u *ಪರಿಶೀಲನೆ)
{
    regitem_T	*rp;

    ಆದರೆ ((ದೀರ್ಘ)((ಚಿಹ್ನೆರಹಿತ)regstack.ga_len >> 10) >= p_mmp)
    {
	emsg(_(e_ನಮೂನೆ_ಬಳಸುತ್ತದೆ_ಇನ್ನಷ್ಟು_ಸ್ಮರಣೆ_than_maxmempattern));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (GA_GROW_FAILS(&regstack, (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(regitem_T)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    rp = (regitem_T *)((ಅಕ್ಷರ *)regstack.ga_ದತ್ತಾಂಶ + regstack.ga_len);
    rp->rs_state = state;
    rp->rs_ಪರಿಶೀಲನೆ = ಪರಿಶೀಲನೆ;

    regstack.ga_len += ಗಾತ್ರ(regitem_T);
    ಹಿಂತಿರುಗಿ rp;
}

/*
 * Pop an item from the regstack.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
regstack_ತೆಗೆ(char_u **ಪರಿಶೀಲನೆ)
{
    regitem_T	*rp;

    rp = (regitem_T *)((ಅಕ್ಷರ *)regstack.ga_ದತ್ತಾಂಶ + regstack.ga_len) - 1;
    *ಪರಿಶೀಲನೆ = rp->rs_ಪರಿಶೀಲನೆ;

    regstack.ga_len -= ಗಾತ್ರ(regitem_T);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
/*
 * Check if the timer expired, return TRUE if so.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
bt_did_ಸಮಯ_ಹೊರಹೋಗು(ಪೂರ್ಣಾಂಕ *timed_ಹೊರಹೋಗು)
{
    ಆದರೆ (*timeout_flag)
    {
	ಆದರೆ (timed_ಹೊರಹೋಗು != ಖಾಲಿ)
	{
# ifdef FEAT_EVAL
	    ಆದರೆ (!*timed_ಹೊರಹೋಗು)
		ch_log(ಖಾಲಿ, "BT regexp timed out");
# endif
	    *timed_ಹೊರಹೋಗು = TRUE;
	}
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Save the current subexpr to "bp", so that they can be restored
 * later by restore_subexpr().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
save_subexpr(regbehind_T *bp)
{
    ಪೂರ್ಣಾಂಕ i;

    // When "rex.need_clear_subexpr" is set we don't need to save the values,
    // only remember that this flag needs to be set again when restoring.
    bp->save_need_clear_subexpr = rex.need_clear_subexpr;
    ಆದರೆ (rex.need_clear_subexpr)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (i = 0; i < NSUBEXP; ++i)
    {
	ಆದರೆ (REG_MULTI)
	{
	    bp->save_ಆರಂಭ[i].se_u.pos = rex.reg_startpos[i];
	    bp->save_ಕೊನೆ[i].se_u.pos = rex.reg_endpos[i];
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    bp->save_ಆರಂಭ[i].se_u.ptr = rex.reg_startp[i];
	    bp->save_ಕೊನೆ[i].se_u.ptr = rex.reg_endp[i];
	}
    }
}

/*
 * Restore the subexpr from "bp".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
restore_subexpr(regbehind_T *bp)
{
    ಪೂರ್ಣಾಂಕ i;

    // Only need to restore saved values when they are not to be cleared.
    rex.need_clear_subexpr = bp->save_need_clear_subexpr;
    ಆದರೆ (rex.need_clear_subexpr)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (i = 0; i < NSUBEXP; ++i)
    {
	ಆದರೆ (REG_MULTI)
	{
	    rex.reg_startpos[i] = bp->save_ಆರಂಭ[i].se_u.pos;
	    rex.reg_endpos[i] = bp->save_ಕೊನೆ[i].se_u.pos;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    rex.reg_startp[i] = bp->save_ಆರಂಭ[i].se_u.ptr;
	    rex.reg_endp[i] = bp->save_ಕೊನೆ[i].se_u.ptr;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
regmatch(
    char_u	*ಪರಿಶೀಲನೆ,		    // Current node.
    ಪೂರ್ಣಾಂಕ		*timed_ಹೊರಹೋಗು ಬಳಸದ)  // flag set on timeout or NULL
{
  char_u	*ಮುಂದಿನ;		// Next node.
  ಪೂರ್ಣಾಂಕ		op;
  ಪೂರ್ಣಾಂಕ		c;
  regitem_T	*rp;
  ಪೂರ್ಣಾಂಕ		no;
  ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2;		// one of the RA_ values:

  // Make "regstack" and "backpos" empty.  They are allocated and freed in
  // bt_regexec_both() to reduce malloc()/free() calls.
  regstack.ga_len = 0;
  backpos.ga_len = 0;

  // Repeat until "regstack" is empty.
  ಗಾಗಿ (;;)
  {
    // Some patterns may take a long time to match, e.g., "\([a-z]\+\)\+Q".
    // Allow interrupting them with CTRL-C.
    fast_breakcheck();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
    ಆದರೆ (ಪರಿಶೀಲನೆ != ಖಾಲಿ && regnarrate)
    {
	mch_errmsg((ಅಕ್ಷರ *)regprop(ಪರಿಶೀಲನೆ));
	mch_errmsg("(\n");
    }
#ಆದರೆ_ಅಂತ್ಯ

    // Repeat for items that can be matched sequentially, without using the
    // regstack.
    ಗಾಗಿ (;;)
    {
	ಆದರೆ (got_int || ಪರಿಶೀಲನೆ == ಖಾಲಿ)
	{
	    ಸ್ಥಿತಿ_2 = RA_FAIL;
	    ನಿಲ್ಲಿಸು;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
	ಆದರೆ (bt_did_ಸಮಯ_ಹೊರಹೋಗು(timed_ಹೊರಹೋಗು))
	{
	    ಸ್ಥಿತಿ_2 = RA_FAIL;
	    ನಿಲ್ಲಿಸು;
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ_2 = RA_CONT;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
	ಆದರೆ (regnarrate)
	{
	    mch_errmsg((ಅಕ್ಷರ *)regprop(ಪರಿಶೀಲನೆ));
	    mch_errmsg("...\n");
# ifdef FEAT_SYN_HL
	    ಆದರೆ (re_extmatch_ಒಳ != ಖಾಲಿ)
	    {
		ಪೂರ್ಣಾಂಕ i;

		mch_errmsg(_("External submatches:\n"));
		ಗಾಗಿ (i = 0; i < NSUBEXP; i++)
		{
		    mch_errmsg("    \"");
		    ಆದರೆ (re_extmatch_ಒಳ->matches[i] != ಖಾಲಿ)
			mch_errmsg((ಅಕ್ಷರ *)re_extmatch_ಒಳ->matches[i]);
		    mch_errmsg("\"\n");
		}
	    }
# endif
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಮುಂದಿನ = regnext(ಪರಿಶೀಲನೆ);

	op = OP(ಪರಿಶೀಲನೆ);
	// Check for character class with NL added.
	ಆದರೆ (!rex.reg_ಸಾಲು_lbr && WITH_NL(op) && REG_MULTI
			     && *rex.ಒಳಹರಿವು == NUL && rex.lnum <= rex.reg_maxline)
	{
	    reg_nextline();
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rex.reg_ಸಾಲು_lbr && WITH_NL(op) && *rex.ಒಳಹರಿವು == '\n')
	{
	    ADVANCE_REGINPUT();
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	  ಆದರೆ (WITH_NL(op))
	      op -= ಕೂಡಿಸು_NL;
	  ಆದರೆ (has_mbyte)
	      c = (*mb_ptr2char)(rex.ಒಳಹರಿವು);
	  ಇಲ್ಲದಿದ್ದರೆ
	      c = *rex.ಒಳಹರಿವು;
	  ಆಯ್ಕೆ (op)
	  {
	  ಸ್ಥಿತಿ BOL:
	    ಆದರೆ (rex.ಒಳಹರಿವು != rex.ಸಾಲು)
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ EOL:
	    ಆದರೆ (c != NUL)
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ RE_BOF:
	    // We're not at the beginning of the file when below the first
	    // line where we started, not at the start of the line or we
	    // didn't start at the first line of the buffer.
	    ಆದರೆ (rex.lnum != 0 || rex.ಒಳಹರಿವು != rex.ಸಾಲು
				       || (REG_MULTI && rex.reg_firstlnum > 1))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ RE_EOF:
	    ಆದರೆ (rex.lnum != rex.reg_maxline || c != NUL)
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ CURSOR:
	    // Check if the buffer is in a window and compare the
	    // rex.reg_win->w_cursor position to the match position.
	    ಆದರೆ (rex.reg_win == ಖಾಲಿ
		    || (rex.lnum + rex.reg_firstlnum
						 != rex.reg_win->w_cursor.lnum)
		    || ((colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು)
						 != rex.reg_win->w_cursor.ಲಂಬಸಾಲು))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ RE_MARK:
	    // Compare the mark position to the match position.
	    {
		ಪೂರ್ಣಾಂಕ	mark = ಕಾರ್ಯಾಂಶ(ಪರಿಶೀಲನೆ)[0];
		ಪೂರ್ಣಾಂಕ	cmp = ಕಾರ್ಯಾಂಶ(ಪರಿಶೀಲನೆ)[1];
		pos_T	*pos;
		size_t	ಲಂಬಸಾಲು = REG_MULTI ? rex.ಒಳಹರಿವು - rex.ಸಾಲು : 0;

		pos = getmark_buf(rex.reg_buf, mark, FALSE);

		// Line may have been freed, get it again.
		ಆದರೆ (REG_MULTI)
		{
		    rex.ಸಾಲು = reg_getline(rex.lnum);
		    rex.ಒಳಹರಿವು = rex.ಸಾಲು + ಲಂಬಸಾಲು;
		}

		ಆದರೆ (pos == ಖಾಲಿ		     // mark doesn't exist
			|| pos->lnum <= 0)   // mark isn't set in reg_buf
		{
		    ಸ್ಥಿತಿ_2 = RA_NOMATCH;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    colnr_T pos_ಲಂಬಸಾಲು = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->ಲಂಬಸಾಲು == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->ಲಂಬಸಾಲು;

		    ಆದರೆ ((pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_ಲಂಬಸಾಲು == (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು)
				    ? (cmp == '<' || cmp == '>')
				    : (pos_ಲಂಬಸಾಲು < (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು)
					? cmp != '>'
					: cmp != '<'))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? cmp != '>'
				    : cmp != '<')))
		    ಸ್ಥಿತಿ_2 = RA_NOMATCH;
		}
	    }
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ RE_VISUAL:
	    ಆದರೆ (!reg_ಹೊಂದಿಕೆ_visual())
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ RE_LNUM:
	    ಆದರೆ (!REG_MULTI || !re_ಅಂಕೆ_cmp((long_u)(rex.lnum + rex.reg_firstlnum),
									ಪರಿಶೀಲನೆ))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ RE_ಲಂಬಸಾಲು:
	    ಆದರೆ (!re_ಅಂಕೆ_cmp((long_u)(rex.ಒಳಹರಿವು - rex.ಸಾಲು) + 1, ಪರಿಶೀಲನೆ))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ RE_VCOL:
	    {
		win_T	    *wp = rex.reg_win == ಖಾಲಿ ? curwin : rex.reg_win;
		linenr_T    lnum = REG_MULTI ? rex.reg_firstlnum + rex.lnum : 1;
		long_u	    vcol;

		ಆದರೆ (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ))
		    lnum = 1;
		vcol = (long_u)win_linetabsize(wp, lnum, rex.ಸಾಲು,
					      (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು));
		ಆದರೆ (!re_ಅಂಕೆ_cmp(vcol + 1, ಪರಿಶೀಲನೆ))
		    ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    }
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ BOW:	// \<word; rex.input points to w
	    ಆದರೆ (c == NUL)	// Can't match at end of line
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte)
	    {
		ಪೂರ್ಣಾಂಕ this_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.ಒಳಹರಿವು, rex.reg_buf);
		ಆದರೆ (this_class <= 1)
		    ಸ್ಥಿತಿ_2 = RA_NOMATCH;  // not on a word at all
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (reg_prev_class() == this_class)
		    ಸ್ಥಿತಿ_2 = RA_NOMATCH;  // previous char is in same word
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (!vim_iswordc_buf(c, rex.reg_buf) || (rex.ಒಳಹರಿವು > rex.ಸಾಲು
				&& vim_iswordc_buf(rex.ಒಳಹರಿವು[-1], rex.reg_buf)))
		    ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    }
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ EOW:	// word\>; rex.input points after d
	    ಆದರೆ (rex.ಒಳಹರಿವು == rex.ಸಾಲು)    // Can't match at start of line
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte)
	    {
		ಪೂರ್ಣಾಂಕ this_class, prev_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.ಒಳಹರಿವು, rex.reg_buf);
		prev_class = reg_prev_class();
		ಆದರೆ (this_class == prev_class
			|| prev_class == 0 || prev_class == 1)
		    ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (!vim_iswordc_buf(rex.ಒಳಹರಿವು[-1], rex.reg_buf)
			|| (rex.ಒಳಹರಿವು[0] != NUL
					   && vim_iswordc_buf(c, rex.reg_buf)))
		    ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    }
	    ನಿಲ್ಲಿಸು; // Matched with EOW

	  ಸ್ಥಿತಿ ANY:
	    // ANY does not match new lines.
	    ಆದರೆ (c == NUL)
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
		ADVANCE_REGINPUT();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ IDENT:
	    ಆದರೆ (!vim_isIDc(c))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
		ADVANCE_REGINPUT();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ SIDENT:
	    ಆದರೆ (VIM_ISDIGIT(*rex.ಒಳಹರಿವು) || !vim_isIDc(c))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
		ADVANCE_REGINPUT();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ KWORD:
	    ಆದರೆ (!vim_iswordp_buf(rex.ಒಳಹರಿವು, rex.reg_buf))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
		ADVANCE_REGINPUT();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ SKWORD:
	    ಆದರೆ (VIM_ISDIGIT(*rex.ಒಳಹರಿವು)
				    || !vim_iswordp_buf(rex.ಒಳಹರಿವು, rex.reg_buf))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
		ADVANCE_REGINPUT();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ FNAME:
	    ಆದರೆ (!vim_isfilec(c))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
		ADVANCE_REGINPUT();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ SFNAME:
	    ಆದರೆ (VIM_ISDIGIT(*rex.ಒಳಹರಿವು) || !vim_isfilec(c))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
		ADVANCE_REGINPUT();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ PRINT:
	    ಆದರೆ (!vim_isprintc(PTR2CHAR(rex.ಒಳಹರಿವು)))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
		ADVANCE_REGINPUT();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ SPRINT:
	    ಆದರೆ (VIM_ISDIGIT(*rex.ಒಳಹರಿವು) || !vim_isprintc(PTR2CHAR(rex.ಒಳಹರಿವು)))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
		ADVANCE_REGINPUT();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ WHITE:
	    ಆದರೆ (!VIM_ISWHITE(c))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
		ADVANCE_REGINPUT();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ NWHITE:
	    ಆದರೆ (c == NUL || VIM_ISWHITE(c))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
		ADVANCE_REGINPUT();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ DIGIT:
	    ಆದರೆ (!ri_digit(c))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
		ADVANCE_REGINPUT();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ NDIGIT:
	    ಆದರೆ (c == NUL || ri_digit(c))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
		ADVANCE_REGINPUT();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ HEX:
	    ಆದರೆ (!ri_hex(c))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
		ADVANCE_REGINPUT();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ NHEX:
	    ಆದರೆ (c == NUL || ri_hex(c))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
		ADVANCE_REGINPUT();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ OCTAL:
	    ಆದರೆ (!ri_octal(c))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
		ADVANCE_REGINPUT();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ NOCTAL:
	    ಆದರೆ (c == NUL || ri_octal(c))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
		ADVANCE_REGINPUT();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ ಶಬ್ದ_2:
	    ಆದರೆ (!ri_ಶಬ್ದ(c))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
		ADVANCE_REGINPUT();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ NWORD:
	    ಆದರೆ (c == NUL || ri_ಶಬ್ದ(c))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
		ADVANCE_REGINPUT();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ ತಲೆ_2:
	    ಆದರೆ (!ri_ತಲೆ(c))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
		ADVANCE_REGINPUT();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ NHEAD:
	    ಆದರೆ (c == NUL || ri_ತಲೆ(c))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
		ADVANCE_REGINPUT();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ ALPHA:
	    ಆದರೆ (!ri_alpha(c))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
		ADVANCE_REGINPUT();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ NALPHA:
	    ಆದರೆ (c == NUL || ri_alpha(c))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
		ADVANCE_REGINPUT();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ ಅಧೋ_2:
	    ಆದರೆ (!ri_ಅಧೋ(c))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
		ADVANCE_REGINPUT();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ NLOWER:
	    ಆದರೆ (c == NUL || ri_ಅಧೋ(c))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
		ADVANCE_REGINPUT();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ ಊರ್ಧ್ವ:
	    ಆದರೆ (!ri_ಊರ್ಧ್ವ(c))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
		ADVANCE_REGINPUT();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ NUPPER:
	    ಆದರೆ (c == NUL || ri_ಊರ್ಧ್ವ(c))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
		ADVANCE_REGINPUT();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ EXACTLY:
	    {
		ಪೂರ್ಣಾಂಕ	len;
		char_u	*opnd;

		opnd = ಕಾರ್ಯಾಂಶ(ಪರಿಶೀಲನೆ);
		// Inline the first byte, for speed.
		ಆದರೆ (*opnd != *rex.ಒಳಹರಿವು
			&& (!rex.reg_ic
			    || (!enc_utf8
			      && MB_TOLOWER(*opnd) != MB_TOLOWER(*rex.ಒಳಹರಿವು))))
		    ಸ್ಥಿತಿ_2 = RA_NOMATCH;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*opnd == NUL)
		{
		    // match empty string always works; happens when "~" is
		    // empty.
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (opnd[1] == NUL && !(enc_utf8 && rex.reg_ic))
		    {
			len = 1;	// matched a single byte above
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// Need to match first byte again for multi-byte.
			len = (ಪೂರ್ಣಾಂಕ)STRLEN(opnd);
			ಆದರೆ (cstrncmp(opnd, rex.ಒಳಹರಿವು, &len) != 0)
			    ಸ್ಥಿತಿ_2 = RA_NOMATCH;
		    }
		    // Check for following composing character, unless %C
		    // follows (skips over all composing chars).
		    ಆದರೆ (ಸ್ಥಿತಿ_2 != RA_NOMATCH
			    && enc_utf8
			    && UTF_COMPOSINGLIKE(rex.ಒಳಹರಿವು, rex.ಒಳಹರಿವು + len)
			    && !rex.reg_icombine
			    && OP(ಮುಂದಿನ) != RE_COMPOSING)
		    {
			// raaron: This code makes a composing character get
			// ignored, which is the correct behavior (sometimes)
			// for voweled Hebrew texts.
			ಸ್ಥಿತಿ_2 = RA_NOMATCH;
		    }
		    ಆದರೆ (ಸ್ಥಿತಿ_2 != RA_NOMATCH)
			rex.ಒಳಹರಿವು += len;
		}
	    }
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ ANYOF:
	  ಸ್ಥಿತಿ ANYBUT:
	    {
		char_u  *q = ಕಾರ್ಯಾಂಶ(ಪರಿಶೀಲನೆ);

		ಆದರೆ (c == NUL)
		    ಸ್ಥಿತಿ_2 = RA_NOMATCH;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((cstrchr(q, c) == ಖಾಲಿ) == (op == ANYOF))
		    ಸ್ಥಿತಿ_2 = RA_NOMATCH;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Check following combining characters
		    ಪೂರ್ಣಾಂಕ	len = 0;
		    ಪೂರ್ಣಾಂಕ i;

		    ಆದರೆ (enc_utf8)
			len = utfc_ptr2len(q) - utf_ptr2len(q);

		    MB_CPTR_ADV(rex.ಒಳಹರಿವು);
		    MB_CPTR_ADV(q);

		    ಆದರೆ (!enc_utf8 || len == 0)
			ನಿಲ್ಲಿಸು;

		    ಗಾಗಿ (i = 0; i < len; ++i)
			ಆದರೆ (q[i] != rex.ಒಳಹರಿವು[i])
			{
			    ಸ್ಥಿತಿ_2 = RA_NOMATCH;
			    ನಿಲ್ಲಿಸು;
			}
		    rex.ಒಳಹರಿವು += len;
		}
		ನಿಲ್ಲಿಸು;
	    }

	  ಸ್ಥಿತಿ MULTIBYTECODE:
	    ಆದರೆ (has_mbyte)
	    {
		ಪೂರ್ಣಾಂಕ	i, len;
		char_u	*opnd;
		ಪೂರ್ಣಾಂಕ	opndc = 0, inpc;

		opnd = ಕಾರ್ಯಾಂಶ(ಪರಿಶೀಲನೆ);
		// Safety check (just in case 'encoding' was changed since
		// compiling the program).
		ಆದರೆ ((len = (*mb_ptr2len)(opnd)) < 2)
		{
		    ಸ್ಥಿತಿ_2 = RA_NOMATCH;
		    ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (enc_utf8)
		    opndc = utf_ptr2char(opnd);
		ಆದರೆ (enc_utf8 && utf_iscomposing(opndc))
		{
		    // When only a composing char is given match at any
		    // position where that composing char appears.
		    ಸ್ಥಿತಿ_2 = RA_NOMATCH;
		    ಗಾಗಿ (i = 0; rex.ಒಳಹರಿವು[i] != NUL;
						i += utf_ptr2len(rex.ಒಳಹರಿವು + i))
		    {
			inpc = utf_ptr2char(rex.ಒಳಹರಿವು + i);
			ಆದರೆ (!utf_iscomposing(inpc))
			{
			    ಆದರೆ (i > 0)
				ನಿಲ್ಲಿಸು;
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (opndc == inpc)
			{
			    // Include all following composing chars.
			    len = i + utfc_ptr2len(rex.ಒಳಹರಿವು + i);
			    ಸ್ಥಿತಿ_2 = RA_ಹೊಂದಿಕೆ;
			    ನಿಲ್ಲಿಸು;
			}
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (enc_utf8)
		{
		    ಆದರೆ (cstrncmp(opnd, rex.ಒಳಹರಿವು, &len) != 0)
		    {
			ಸ್ಥಿತಿ_2 = RA_NOMATCH;
			ನಿಲ್ಲಿಸು;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ಗಾಗಿ (i = 0; i < len; ++i)
			ಆದರೆ (opnd[i] != rex.ಒಳಹರಿವು[i])
			{
			    ಸ್ಥಿತಿ_2 = RA_NOMATCH;
			    ನಿಲ್ಲಿಸು;
			}
		rex.ಒಳಹರಿವು += len;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ RE_COMPOSING:
	    ಆದರೆ (enc_utf8)
	    {
		// Skip composing characters.
		ಆಗ (utf_iscomposing(utf_ptr2char(rex.ಒಳಹರಿವು)))
		    MB_CPTR_ADV(rex.ಒಳಹರಿವು);
	    }
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ NOTHING:
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ ಹಿಂಭಾಗ_2:
	    {
		ಪೂರ್ಣಾಂಕ		i;
		backpos_T	*bp;

		// When we run into BACK we need to check if we don't keep
		// looping without matching any input.  The second and later
		// times a BACK is encountered it fails if the input is still
		// at the same position as the previous time.
		// The positions are stored in "backpos" and found by the
		// current value of "scan", the position in the RE program.
		bp = (backpos_T *)backpos.ga_ದತ್ತಾಂಶ;
		ಗಾಗಿ (i = 0; i < backpos.ga_len; ++i)
		    ಆದರೆ (bp[i].bp_ಪರಿಶೀಲನೆ == ಪರಿಶೀಲನೆ)
			ನಿಲ್ಲಿಸು;
		ಆದರೆ (i == backpos.ga_len)
		{
		    // First time at this BACK, make room to store the pos.
		    ಆದರೆ (GA_GROW_FAILS(&backpos, 1))
			ಸ್ಥಿತಿ_2 = RA_FAIL;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// get "ga_data" again, it may have changed
			bp = (backpos_T *)backpos.ga_ದತ್ತಾಂಶ;
			bp[i].bp_ಪರಿಶೀಲನೆ = ಪರಿಶೀಲನೆ;
			++backpos.ga_len;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (reg_save_equal(&bp[i].bp_pos))
		    // Still at same position as last time, fail.
		    ಸ್ಥಿತಿ_2 = RA_NOMATCH;

		ಆದರೆ (ಸ್ಥಿತಿ_2 != RA_FAIL && ಸ್ಥಿತಿ_2 != RA_NOMATCH)
		    reg_save(&bp[i].bp_pos, &backpos);
	    }
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ MOPEN + 0:   // Match start: \zs
	  ಸ್ಥಿತಿ MOPEN + 1:   // \(
	  ಸ್ಥಿತಿ MOPEN + 2:
	  ಸ್ಥಿತಿ MOPEN + 3:
	  ಸ್ಥಿತಿ MOPEN + 4:
	  ಸ್ಥಿತಿ MOPEN + 5:
	  ಸ್ಥಿತಿ MOPEN + 6:
	  ಸ್ಥಿತಿ MOPEN + 7:
	  ಸ್ಥಿತಿ MOPEN + 8:
	  ಸ್ಥಿತಿ MOPEN + 9:
	    {
		no = op - MOPEN;
		cleanup_subexpr();
		rp = regstack_ತಳ್ಳು(RS_MOPEN, ಪರಿಶೀಲನೆ);
		ಆದರೆ (rp == ಖಾಲಿ)
		    ಸ್ಥಿತಿ_2 = RA_FAIL;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_startpos[no],
							  &rex.reg_startp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ NOPEN:	    // \%(
	  ಸ್ಥಿತಿ NCLOSE:	    // \) after \%(
		ಆದರೆ (regstack_ತಳ್ಳು(RS_NOPEN, ಪರಿಶೀಲನೆ) == ಖಾಲಿ)
		    ಸ್ಥಿತಿ_2 = RA_FAIL;
		// We simply continue and handle the result when done.
		ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	  ಸ್ಥಿತಿ ZOPEN + 1:
	  ಸ್ಥಿತಿ ZOPEN + 2:
	  ಸ್ಥಿತಿ ZOPEN + 3:
	  ಸ್ಥಿತಿ ZOPEN + 4:
	  ಸ್ಥಿತಿ ZOPEN + 5:
	  ಸ್ಥಿತಿ ZOPEN + 6:
	  ಸ್ಥಿತಿ ZOPEN + 7:
	  ಸ್ಥಿತಿ ZOPEN + 8:
	  ಸ್ಥಿತಿ ZOPEN + 9:
	    {
		no = op - ZOPEN;
		cleanup_zsubexpr();
		rp = regstack_ತಳ್ಳು(RS_ZOPEN, ಪರಿಶೀಲನೆ);
		ಆದರೆ (rp == ಖಾಲಿ)
		    ಸ್ಥಿತಿ_2 = RA_FAIL;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_startzpos[no],
							     &reg_startzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

	  ಸ್ಥಿತಿ MCLOSE + 0:  // Match end: \ze
	  ಸ್ಥಿತಿ MCLOSE + 1:  // \)
	  ಸ್ಥಿತಿ MCLOSE + 2:
	  ಸ್ಥಿತಿ MCLOSE + 3:
	  ಸ್ಥಿತಿ MCLOSE + 4:
	  ಸ್ಥಿತಿ MCLOSE + 5:
	  ಸ್ಥಿತಿ MCLOSE + 6:
	  ಸ್ಥಿತಿ MCLOSE + 7:
	  ಸ್ಥಿತಿ MCLOSE + 8:
	  ಸ್ಥಿತಿ MCLOSE + 9:
	    {
		no = op - MCLOSE;
		cleanup_subexpr();
		rp = regstack_ತಳ್ಳು(RS_MCLOSE, ಪರಿಶೀಲನೆ);
		ಆದರೆ (rp == ಖಾಲಿ)
		    ಸ್ಥಿತಿ_2 = RA_FAIL;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_endpos[no],
							    &rex.reg_endp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	  ಸ್ಥಿತಿ ZCLOSE + 1:  // \) after \z(
	  ಸ್ಥಿತಿ ZCLOSE + 2:
	  ಸ್ಥಿತಿ ZCLOSE + 3:
	  ಸ್ಥಿತಿ ZCLOSE + 4:
	  ಸ್ಥಿತಿ ZCLOSE + 5:
	  ಸ್ಥಿತಿ ZCLOSE + 6:
	  ಸ್ಥಿತಿ ZCLOSE + 7:
	  ಸ್ಥಿತಿ ZCLOSE + 8:
	  ಸ್ಥಿತಿ ZCLOSE + 9:
	    {
		no = op - ZCLOSE;
		cleanup_zsubexpr();
		rp = regstack_ತಳ್ಳು(RS_ZCLOSE, ಪರಿಶೀಲನೆ);
		ಆದರೆ (rp == ಖಾಲಿ)
		    ಸ್ಥಿತಿ_2 = RA_FAIL;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_endzpos[no],
							      &reg_endzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

	  ಸ್ಥಿತಿ BACKREF + 1:
	  ಸ್ಥಿತಿ BACKREF + 2:
	  ಸ್ಥಿತಿ BACKREF + 3:
	  ಸ್ಥಿತಿ BACKREF + 4:
	  ಸ್ಥಿತಿ BACKREF + 5:
	  ಸ್ಥಿತಿ BACKREF + 6:
	  ಸ್ಥಿತಿ BACKREF + 7:
	  ಸ್ಥಿತಿ BACKREF + 8:
	  ಸ್ಥಿತಿ BACKREF + 9:
	    {
		ಪೂರ್ಣಾಂಕ		len;

		no = op - BACKREF;
		cleanup_subexpr();
		ಆದರೆ (!REG_MULTI)		// Single-line regexp
		{
		    ಆದರೆ (rex.reg_startp[no] == ಖಾಲಿ || rex.reg_endp[no] == ಖಾಲಿ)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// Compare current input with back-ref in the same
			// line.
			len = (ಪೂರ್ಣಾಂಕ)(rex.reg_endp[no] - rex.reg_startp[no]);
			ಆದರೆ (cstrncmp(rex.reg_startp[no], rex.ಒಳಹರಿವು, &len) != 0)
			    ಸ್ಥಿತಿ_2 = RA_NOMATCH;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ				// Multi-line regexp
		{
		    ಆದರೆ (rex.reg_startpos[no].lnum < 0
						|| rex.reg_endpos[no].lnum < 0)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ (rex.reg_startpos[no].lnum == rex.lnum
				&& rex.reg_endpos[no].lnum == rex.lnum)
			{
			    // Compare back-ref within the current line.
			    len = rex.reg_endpos[no].ಲಂಬಸಾಲು
						    - rex.reg_startpos[no].ಲಂಬಸಾಲು;
			    ಆದರೆ (cstrncmp(rex.ಸಾಲು + rex.reg_startpos[no].ಲಂಬಸಾಲು,
							  rex.ಒಳಹರಿವು, &len) != 0)
				ಸ್ಥಿತಿ_2 = RA_NOMATCH;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    // Messy situation: Need to compare between two
			    // lines.
			    ಪೂರ್ಣಾಂಕ r = ಹೊಂದಿಕೆ_with_backref(
					    rex.reg_startpos[no].lnum,
					    rex.reg_startpos[no].ಲಂಬಸಾಲು,
					    rex.reg_endpos[no].lnum,
					    rex.reg_endpos[no].ಲಂಬಸಾಲು,
					    &len);

			    ಆದರೆ (r != RA_ಹೊಂದಿಕೆ)
				ಸ್ಥಿತಿ_2 = r;
			}
		    }
		}

		// Matched the backref, skip over it.
		rex.ಒಳಹರಿವು += len;
	    }
	    ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	  ಸ್ಥಿತಿ ZREF + 1:
	  ಸ್ಥಿತಿ ZREF + 2:
	  ಸ್ಥಿತಿ ZREF + 3:
	  ಸ್ಥಿತಿ ZREF + 4:
	  ಸ್ಥಿತಿ ZREF + 5:
	  ಸ್ಥಿತಿ ZREF + 6:
	  ಸ್ಥಿತಿ ZREF + 7:
	  ಸ್ಥಿತಿ ZREF + 8:
	  ಸ್ಥಿತಿ ZREF + 9:
	    {
		ಪೂರ್ಣಾಂಕ	len;

		cleanup_zsubexpr();
		no = op - ZREF;
		ಆದರೆ (re_extmatch_ಒಳ != ಖಾಲಿ
			&& re_extmatch_ಒಳ->matches[no] != ಖಾಲಿ)
		{
		    len = (ಪೂರ್ಣಾಂಕ)STRLEN(re_extmatch_ಒಳ->matches[no]);
		    ಆದರೆ (cstrncmp(re_extmatch_ಒಳ->matches[no],
							  rex.ಒಳಹರಿವು, &len) != 0)
			ಸ್ಥಿತಿ_2 = RA_NOMATCH;
		    ಇಲ್ಲದಿದ್ದರೆ
			rex.ಒಳಹರಿವು += len;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Backref was not set: Match an empty string.
		}
	    }
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

	  ಸ್ಥಿತಿ BRANCH:
	    {
		ಆದರೆ (OP(ಮುಂದಿನ) != BRANCH) // No choice.
		    ಮುಂದಿನ = ಕಾರ್ಯಾಂಶ(ಪರಿಶೀಲನೆ);	// Avoid recursion.
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    rp = regstack_ತಳ್ಳು(RS_BRANCH, ಪರಿಶೀಲನೆ);
		    ಆದರೆ (rp == ಖಾಲಿ)
			ಸ್ಥಿತಿ_2 = RA_FAIL;
		    ಇಲ್ಲದಿದ್ದರೆ
			ಸ್ಥಿತಿ_2 = RA_BREAK;	// rest is below
		}
	    }
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ BRACE_LIMITS:
	    {
		ಆದರೆ (OP(ಮುಂದಿನ) == BRACE_SIMPLE)
		{
		    bl_minval = ಕಾರ್ಯಾಂಶ_MIN(ಪರಿಶೀಲನೆ);
		    bl_maxval = ಕಾರ್ಯಾಂಶ_MAX(ಪರಿಶೀಲನೆ);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (OP(ಮುಂದಿನ) >= BRACE_COMPLEX
			&& OP(ಮುಂದಿನ) < BRACE_COMPLEX + 10)
		{
		    no = OP(ಮುಂದಿನ) - BRACE_COMPLEX;
		    brace_min[no] = ಕಾರ್ಯಾಂಶ_MIN(ಪರಿಶೀಲನೆ);
		    brace_max[no] = ಕಾರ್ಯಾಂಶ_MAX(ಪರಿಶೀಲನೆ);
		    brace_ಎಣಿಕೆ[no] = 0;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆಂತರಿಕ_error("BRACE_LIMITS");
		    ಸ್ಥಿತಿ_2 = RA_FAIL;
		}
	    }
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ BRACE_COMPLEX + 0:
	  ಸ್ಥಿತಿ BRACE_COMPLEX + 1:
	  ಸ್ಥಿತಿ BRACE_COMPLEX + 2:
	  ಸ್ಥಿತಿ BRACE_COMPLEX + 3:
	  ಸ್ಥಿತಿ BRACE_COMPLEX + 4:
	  ಸ್ಥಿತಿ BRACE_COMPLEX + 5:
	  ಸ್ಥಿತಿ BRACE_COMPLEX + 6:
	  ಸ್ಥಿತಿ BRACE_COMPLEX + 7:
	  ಸ್ಥಿತಿ BRACE_COMPLEX + 8:
	  ಸ್ಥಿತಿ BRACE_COMPLEX + 9:
	    {
		no = op - BRACE_COMPLEX;
		++brace_ಎಣಿಕೆ[no];

		// If not matched enough times yet, try one more
		ಆದರೆ (brace_ಎಣಿಕೆ[no] <= (brace_min[no] <= brace_max[no]
					     ? brace_min[no] : brace_max[no]))
		{
		    rp = regstack_ತಳ್ಳು(RS_BRCPLX_ಇನ್ನಷ್ಟು, ಪರಿಶೀಲನೆ);
		    ಆದರೆ (rp == ಖಾಲಿ)
			ಸ್ಥಿತಿ_2 = RA_FAIL;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			rp->rs_no = no;
			reg_save(&rp->rs_un.regsave, &backpos);
			ಮುಂದಿನ = ಕಾರ್ಯಾಂಶ(ಪರಿಶೀಲನೆ);
			// We continue and handle the result when done.
		    }
		    ನಿಲ್ಲಿಸು;
		}

		// If matched enough times, may try matching some more
		ಆದರೆ (brace_min[no] <= brace_max[no])
		{
		    // Range is the normal way around, use longest match
		    ಆದರೆ (brace_ಎಣಿಕೆ[no] <= brace_max[no])
		    {
			rp = regstack_ತಳ್ಳು(RS_BRCPLX_LONG, ಪರಿಶೀಲನೆ);
			ಆದರೆ (rp == ಖಾಲಿ)
			    ಸ್ಥಿತಿ_2 = RA_FAIL;
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    rp->rs_no = no;
			    reg_save(&rp->rs_un.regsave, &backpos);
			    ಮುಂದಿನ = ಕಾರ್ಯಾಂಶ(ಪರಿಶೀಲನೆ);
			    // We continue and handle the result when done.
			}
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Range is backwards, use shortest match first
		    ಆದರೆ (brace_ಎಣಿಕೆ[no] <= brace_min[no])
		    {
			rp = regstack_ತಳ್ಳು(RS_BRCPLX_SHORT, ಪರಿಶೀಲನೆ);
			ಆದರೆ (rp == ಖಾಲಿ)
			    ಸ್ಥಿತಿ_2 = RA_FAIL;
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    reg_save(&rp->rs_un.regsave, &backpos);
			    // We continue and handle the result when done.
			}
		    }
		}
	    }
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ BRACE_SIMPLE:
	  ಸ್ಥಿತಿ ನಕ್ಷತ್ರ:
	  ಸ್ಥಿತಿ PLUS:
	    {
		regstar_T	rst;

		// Lookahead to avoid useless match attempts when we know
		// what character comes next.
		ಆದರೆ (OP(ಮುಂದಿನ) == EXACTLY)
		{
		    rst.nextb = *ಕಾರ್ಯಾಂಶ(ಮುಂದಿನ);
		    ಆದರೆ (rex.reg_ic)
		    {
			ಆದರೆ (MB_ISUPPER(rst.nextb))
			    rst.nextb_ic = MB_TOLOWER(rst.nextb);
			ಇಲ್ಲದಿದ್ದರೆ
			    rst.nextb_ic = MB_TOUPPER(rst.nextb);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			rst.nextb_ic = rst.nextb;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    rst.nextb = NUL;
		    rst.nextb_ic = NUL;
		}
		ಆದರೆ (op != BRACE_SIMPLE)
		{
		    rst.minval = (op == ನಕ್ಷತ್ರ) ? 0 : 1;
		    rst.maxval = MAX_ಎಲ್ಲೆ;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    rst.minval = bl_minval;
		    rst.maxval = bl_maxval;
		}

		// When maxval > minval, try matching as much as possible, up
		// to maxval.  When maxval < minval, try matching at least the
		// minimal number (since the range is backwards, that's also
		// maxval!).
		rst.ಎಣಿಕೆ_2 = regrepeat(ಕಾರ್ಯಾಂಶ(ಪರಿಶೀಲನೆ), rst.maxval);
		ಆದರೆ (got_int)
		{
		    ಸ್ಥಿತಿ_2 = RA_FAIL;
		    ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (rst.minval <= rst.maxval
			  ? rst.ಎಣಿಕೆ_2 >= rst.minval : rst.ಎಣಿಕೆ_2 >= rst.maxval)
		{
		    // It could match.  Prepare for trying to match what
		    // follows.  The code is below.  Parameters are stored in
		    // a regstar_T on the regstack.
		    ಆದರೆ ((ದೀರ್ಘ)((ಚಿಹ್ನೆರಹಿತ)regstack.ga_len >> 10) >= p_mmp)
		    {
			emsg(_(e_ನಮೂನೆ_ಬಳಸುತ್ತದೆ_ಇನ್ನಷ್ಟು_ಸ್ಮರಣೆ_than_maxmempattern));
			ಸ್ಥಿತಿ_2 = RA_FAIL;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (GA_GROW_FAILS(&regstack, (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(regstar_T)))
			ಸ್ಥಿತಿ_2 = RA_FAIL;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			regstack.ga_len += ಗಾತ್ರ(regstar_T);
			rp = regstack_ತಳ್ಳು(rst.minval <= rst.maxval
					? RS_ನಕ್ಷತ್ರ_LONG : RS_ನಕ್ಷತ್ರ_SHORT, ಪರಿಶೀಲನೆ);
			ಆದರೆ (rp == ಖಾಲಿ)
			    ಸ್ಥಿತಿ_2 = RA_FAIL;
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    *(((regstar_T *)rp) - 1) = rst;
			    ಸ್ಥಿತಿ_2 = RA_BREAK;	    // skip the restore bits
			}
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ಸ್ಥಿತಿ_2 = RA_NOMATCH;

	    }
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ NOMATCH:
	  ಸ್ಥಿತಿ ಹೊಂದಿಕೆ_2:
	  ಸ್ಥಿತಿ SUBPAT:
	    rp = regstack_ತಳ್ಳು(RS_NOMATCH, ಪರಿಶೀಲನೆ);
	    ಆದರೆ (rp == ಖಾಲಿ)
		ಸ್ಥಿತಿ_2 = RA_FAIL;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		rp->rs_no = op;
		reg_save(&rp->rs_un.regsave, &backpos);
		ಮುಂದಿನ = ಕಾರ್ಯಾಂಶ(ಪರಿಶೀಲನೆ);
		// We continue and handle the result when done.
	    }
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ BEHIND:
	  ಸ್ಥಿತಿ NOBEHIND:
	    // Need a bit of room to store extra positions.
	    ಆದರೆ ((ದೀರ್ಘ)((ಚಿಹ್ನೆರಹಿತ)regstack.ga_len >> 10) >= p_mmp)
	    {
		emsg(_(e_ನಮೂನೆ_ಬಳಸುತ್ತದೆ_ಇನ್ನಷ್ಟು_ಸ್ಮರಣೆ_than_maxmempattern));
		ಸ್ಥಿತಿ_2 = RA_FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (GA_GROW_FAILS(&regstack, (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(regbehind_T)))
		ಸ್ಥಿತಿ_2 = RA_FAIL;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		regstack.ga_len += ಗಾತ್ರ(regbehind_T);
		rp = regstack_ತಳ್ಳು(RS_BEHIND1, ಪರಿಶೀಲನೆ);
		ಆದರೆ (rp == ಖಾಲಿ)
		    ಸ್ಥಿತಿ_2 = RA_FAIL;
		ಇಲ್ಲದಿದ್ದರೆ
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
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ BHPOS:
	    ಆದರೆ (REG_MULTI)
	    {
		ಆದರೆ (behind_pos.rs_u.pos.ಲಂಬಸಾಲು != (colnr_T)(rex.ಒಳಹರಿವು - rex.ಸಾಲು)
			|| behind_pos.rs_u.pos.lnum != rex.lnum)
		    ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (behind_pos.rs_u.ptr != rex.ಒಳಹರಿವು)
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ NEWL:
	    ಆದರೆ ((c != NUL || !REG_MULTI || rex.lnum > rex.reg_maxline
			     || rex.reg_ಸಾಲು_lbr)
					   && (c != '\n' || !rex.reg_ಸಾಲು_lbr))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rex.reg_ಸಾಲು_lbr)
		ADVANCE_REGINPUT();
	    ಇಲ್ಲದಿದ್ದರೆ
		reg_nextline();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ ಕೊನೆ_3:
	    ಸ್ಥಿತಿ_2 = RA_ಹೊಂದಿಕೆ;	// Success!
	    ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    iemsg(e_corrupted_regexp_ಕಾರ್ಯಕ್ರಮ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
	    ಬರೆ("Illegal op code %d\n", op);
#ಆದರೆ_ಅಂತ್ಯ
	    ಸ್ಥಿತಿ_2 = RA_FAIL;
	    ನಿಲ್ಲಿಸು;
	  }
	}

	// If we can't continue sequentially, break the inner loop.
	ಆದರೆ (ಸ್ಥಿತಿ_2 != RA_CONT)
	    ನಿಲ್ಲಿಸು;

	// Continue in inner loop, advance to next item.
	ಪರಿಶೀಲನೆ = ಮುಂದಿನ;

    } // end of inner loop

    // If there is something on the regstack execute the code for the state.
    // If the state is popped then loop and use the older state.
    ಆಗ (regstack.ga_len > 0 && ಸ್ಥಿತಿ_2 != RA_FAIL)
    {
	rp = (regitem_T *)((ಅಕ್ಷರ *)regstack.ga_ದತ್ತಾಂಶ + regstack.ga_len) - 1;
	ಆಯ್ಕೆ (rp->rs_state)
	{
	  ಸ್ಥಿತಿ RS_NOPEN:
	    // Result is passed on as-is, simply pop the state.
	    regstack_ತೆಗೆ(&ಪರಿಶೀಲನೆ);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ RS_MOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    ಆದರೆ (ಸ್ಥಿತಿ_2 == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_startpos[rp->rs_no],
						  &rex.reg_startp[rp->rs_no]);
	    regstack_ತೆಗೆ(&ಪರಿಶೀಲನೆ);
	    ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	  ಸ್ಥಿತಿ RS_ZOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    ಆದರೆ (ಸ್ಥಿತಿ_2 == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_startzpos[rp->rs_no],
						 &reg_startzp[rp->rs_no]);
	    regstack_ತೆಗೆ(&ಪರಿಶೀಲನೆ);
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

	  ಸ್ಥಿತಿ RS_MCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    ಆದರೆ (ಸ್ಥಿತಿ_2 == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_endpos[rp->rs_no],
						    &rex.reg_endp[rp->rs_no]);
	    regstack_ತೆಗೆ(&ಪರಿಶೀಲನೆ);
	    ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	  ಸ್ಥಿತಿ RS_ZCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    ಆದರೆ (ಸ್ಥಿತಿ_2 == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_endzpos[rp->rs_no],
						   &reg_endzp[rp->rs_no]);
	    regstack_ತೆಗೆ(&ಪರಿಶೀಲನೆ);
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

	  ಸ್ಥಿತಿ RS_BRANCH:
	    ಆದರೆ (ಸ್ಥಿತಿ_2 == RA_ಹೊಂದಿಕೆ)
		// this branch matched, use it
		regstack_ತೆಗೆ(&ಪರಿಶೀಲನೆ);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (ಸ್ಥಿತಿ_2 != RA_BREAK)
		{
		    // After a non-matching branch: try next one.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    ಪರಿಶೀಲನೆ = rp->rs_ಪರಿಶೀಲನೆ;
		}
		ಆದರೆ (ಪರಿಶೀಲನೆ == ಖಾಲಿ || OP(ಪರಿಶೀಲನೆ) != BRANCH)
		{
		    // no more branches, didn't find a match
		    ಸ್ಥಿತಿ_2 = RA_NOMATCH;
		    regstack_ತೆಗೆ(&ಪರಿಶೀಲನೆ);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Prepare to try a branch.
		    rp->rs_ಪರಿಶೀಲನೆ = regnext(ಪರಿಶೀಲನೆ);
		    reg_save(&rp->rs_un.regsave, &backpos);
		    ಪರಿಶೀಲನೆ = ಕಾರ್ಯಾಂಶ(ಪರಿಶೀಲನೆ);
		}
	    }
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ RS_BRCPLX_ಇನ್ನಷ್ಟು:
	    // Pop the state.  Restore pointers when there is no match.
	    ಆದರೆ (ಸ್ಥಿತಿ_2 == RA_NOMATCH)
	    {
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_ಎಣಿಕೆ[rp->rs_no];	// decrement match count
	    }
	    regstack_ತೆಗೆ(&ಪರಿಶೀಲನೆ);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ RS_BRCPLX_LONG:
	    // Pop the state.  Restore pointers when there is no match.
	    ಆದರೆ (ಸ್ಥಿತಿ_2 == RA_NOMATCH)
	    {
		// There was no match, but we did find enough matches.
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_ಎಣಿಕೆ[rp->rs_no];
		// continue with the items after "\{}"
		ಸ್ಥಿತಿ_2 = RA_CONT;
	    }
	    regstack_ತೆಗೆ(&ಪರಿಶೀಲನೆ);
	    ಆದರೆ (ಸ್ಥಿತಿ_2 == RA_CONT)
		ಪರಿಶೀಲನೆ = regnext(ಪರಿಶೀಲನೆ);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ RS_BRCPLX_SHORT:
	    // Pop the state.  Restore pointers when there is no match.
	    ಆದರೆ (ಸ್ಥಿತಿ_2 == RA_NOMATCH)
		// There was no match, try to match one more item.
		reg_restore(&rp->rs_un.regsave, &backpos);
	    regstack_ತೆಗೆ(&ಪರಿಶೀಲನೆ);
	    ಆದರೆ (ಸ್ಥಿತಿ_2 == RA_NOMATCH)
	    {
		ಪರಿಶೀಲನೆ = ಕಾರ್ಯಾಂಶ(ಪರಿಶೀಲನೆ);
		ಸ್ಥಿತಿ_2 = RA_CONT;
	    }
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ RS_NOMATCH:
	    // Pop the state.  If the operand matches for NOMATCH or
	    // doesn't match for MATCH/SUBPAT, we fail.  Otherwise backup,
	    // except for SUBPAT, and continue with the next item.
	    ಆದರೆ (ಸ್ಥಿತಿ_2 == (rp->rs_no == NOMATCH ? RA_ಹೊಂದಿಕೆ : RA_NOMATCH))
		ಸ್ಥಿತಿ_2 = RA_NOMATCH;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಸ್ಥಿತಿ_2 = RA_CONT;
		ಆದರೆ (rp->rs_no != SUBPAT)	// zero-width
		    reg_restore(&rp->rs_un.regsave, &backpos);
	    }
	    regstack_ತೆಗೆ(&ಪರಿಶೀಲನೆ);
	    ಆದರೆ (ಸ್ಥಿತಿ_2 == RA_CONT)
		ಪರಿಶೀಲನೆ = regnext(ಪರಿಶೀಲನೆ);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ RS_BEHIND1:
	    ಆದರೆ (ಸ್ಥಿತಿ_2 == RA_NOMATCH)
	    {
		regstack_ತೆಗೆ(&ಪರಿಶೀಲನೆ);
		regstack.ga_len -= ಗಾತ್ರ(regbehind_T);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
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
		ಪರಿಶೀಲನೆ = ಕಾರ್ಯಾಂಶ(rp->rs_ಪರಿಶೀಲನೆ) + 4;
	    }
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ RS_BEHIND2:
	    // Looping for BEHIND / NOBEHIND match.
	    ಆದರೆ (ಸ್ಥಿತಿ_2 == RA_ಹೊಂದಿಕೆ && reg_save_equal(&behind_pos))
	    {
		// found a match that ends where "next" started
		behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		ಆದರೆ (rp->rs_no == BEHIND)
		    reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // But we didn't want a match.  Need to restore the
		    // subexpr, because what follows matched, so they have
		    // been set.
		    ಸ್ಥಿತಿ_2 = RA_NOMATCH;
		    restore_subexpr(((regbehind_T *)rp) - 1);
		}
		regstack_ತೆಗೆ(&ಪರಿಶೀಲನೆ);
		regstack.ga_len -= ಗಾತ್ರ(regbehind_T);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ದೀರ್ಘ ಎಲ್ಲೆ;

		// No match or a match that doesn't end where we want it: Go
		// back one character.  May go to previous line once.
		no = OK;
		ಎಲ್ಲೆ = ಕಾರ್ಯಾಂಶ_MIN(rp->rs_ಪರಿಶೀಲನೆ);
		ಆದರೆ (REG_MULTI)
		{
		    ಆದರೆ (ಎಲ್ಲೆ > 0
			    && ((rp->rs_un.regsave.rs_u.pos.lnum
						    < behind_pos.rs_u.pos.lnum
				    ? (colnr_T)STRLEN(rex.ಸಾಲು)
				    : behind_pos.rs_u.pos.ಲಂಬಸಾಲು)
				- rp->rs_un.regsave.rs_u.pos.ಲಂಬಸಾಲು >= ಎಲ್ಲೆ))
			no = FAIL;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rp->rs_un.regsave.rs_u.pos.ಲಂಬಸಾಲು == 0)
		    {
			ಆದರೆ (rp->rs_un.regsave.rs_u.pos.lnum
					< behind_pos.rs_u.pos.lnum
				|| reg_getline(
					--rp->rs_un.regsave.rs_u.pos.lnum)
								  == ಖಾಲಿ)
			    no = FAIL;
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    reg_restore(&rp->rs_un.regsave, &backpos);
			    rp->rs_un.regsave.rs_u.pos.ಲಂಬಸಾಲು =
						 (colnr_T)STRLEN(rex.ಸಾಲು);
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ (has_mbyte)
			{
			    char_u *ಸಾಲು =
				  reg_getline(rp->rs_un.regsave.rs_u.pos.lnum);

			    rp->rs_un.regsave.rs_u.pos.ಲಂಬಸಾಲು -=
				(*mb_ತಲೆ_off)(ಸಾಲು, ಸಾಲು
				    + rp->rs_un.regsave.rs_u.pos.ಲಂಬಸಾಲು - 1) + 1;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			    --rp->rs_un.regsave.rs_u.pos.ಲಂಬಸಾಲು;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (rp->rs_un.regsave.rs_u.ptr == rex.ಸಾಲು)
			no = FAIL;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			MB_PTR_ಹಿಂಭಾಗ(rex.ಸಾಲು, rp->rs_un.regsave.rs_u.ptr);
			ಆದರೆ (ಎಲ್ಲೆ > 0 && (ದೀರ್ಘ)(behind_pos.rs_u.ptr
				     - rp->rs_un.regsave.rs_u.ptr) > ಎಲ್ಲೆ)
			    no = FAIL;
		    }
		}
		ಆದರೆ (no == OK)
		{
		    // Advanced, prepare for finding match again.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    ಪರಿಶೀಲನೆ = ಕಾರ್ಯಾಂಶ(rp->rs_ಪರಿಶೀಲನೆ) + 4;
		    ಆದರೆ (ಸ್ಥಿತಿ_2 == RA_ಹೊಂದಿಕೆ)
		    {
			// We did match, so subexpr may have been changed,
			// need to restore them for the next try.
			ಸ್ಥಿತಿ_2 = RA_NOMATCH;
			restore_subexpr(((regbehind_T *)rp) - 1);
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Can't advance.  For NOBEHIND that's a match.
		    behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		    ಆದರೆ (rp->rs_no == NOBEHIND)
		    {
			reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
			ಸ್ಥಿತಿ_2 = RA_ಹೊಂದಿಕೆ;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// We do want a proper match.  Need to restore the
			// subexpr if we had a match, because they may have
			// been set.
			ಆದರೆ (ಸ್ಥಿತಿ_2 == RA_ಹೊಂದಿಕೆ)
			{
			    ಸ್ಥಿತಿ_2 = RA_NOMATCH;
			    restore_subexpr(((regbehind_T *)rp) - 1);
			}
		    }
		    regstack_ತೆಗೆ(&ಪರಿಶೀಲನೆ);
		    regstack.ga_len -= ಗಾತ್ರ(regbehind_T);
		}
	    }
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ RS_ನಕ್ಷತ್ರ_LONG:
	  ಸ್ಥಿತಿ RS_ನಕ್ಷತ್ರ_SHORT:
	    {
		regstar_T	    *rst = ((regstar_T *)rp) - 1;

		ಆದರೆ (ಸ್ಥಿತಿ_2 == RA_ಹೊಂದಿಕೆ)
		{
		    regstack_ತೆಗೆ(&ಪರಿಶೀಲನೆ);
		    regstack.ga_len -= ಗಾತ್ರ(regstar_T);
		    ನಿಲ್ಲಿಸು;
		}

		// Tried once already, restore input pointers.
		ಆದರೆ (ಸ್ಥಿತಿ_2 != RA_BREAK)
		    reg_restore(&rp->rs_un.regsave, &backpos);

		// Repeat until we found a position where it could match.
		ಗಾಗಿ (;;)
		{
		    ಆದರೆ (ಸ್ಥಿತಿ_2 != RA_BREAK)
		    {
			// Tried first position already, advance.
			ಆದರೆ (rp->rs_state == RS_ನಕ್ಷತ್ರ_LONG)
			{
			    // Trying for longest match, but couldn't or
			    // didn't match -- back up one char.
			    ಆದರೆ (--rst->ಎಣಿಕೆ_2 < rst->minval)
				ನಿಲ್ಲಿಸು;
			    ಆದರೆ (rex.ಒಳಹರಿವು == rex.ಸಾಲು)
			    {
				// backup to last char of previous line
				ಆದರೆ (rex.lnum == 0)
				{
				    ಸ್ಥಿತಿ_2 = RA_NOMATCH;
				    ನಿಲ್ಲಿಸು;
				}
				--rex.lnum;
				rex.ಸಾಲು = reg_getline(rex.lnum);
				// Just in case regrepeat() didn't count
				// right.
				ಆದರೆ (rex.ಸಾಲು == ಖಾಲಿ)
				    ನಿಲ್ಲಿಸು;
				rex.ಒಳಹರಿವು = rex.ಸಾಲು + reg_getline_len(rex.lnum);
				fast_breakcheck();
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
				MB_PTR_ಹಿಂಭಾಗ(rex.ಸಾಲು, rex.ಒಳಹರಿವು);
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    // Range is backwards, use shortest match first.
			    // Careful: maxval and minval are exchanged!
			    // Couldn't or didn't match: try advancing one
			    // char.
			    ಆದರೆ (rst->ಎಣಿಕೆ_2 == rst->minval
				  || regrepeat(ಕಾರ್ಯಾಂಶ(rp->rs_ಪರಿಶೀಲನೆ), 1L) == 0)
				ನಿಲ್ಲಿಸು;
			    ++rst->ಎಣಿಕೆ_2;
			}
			ಆದರೆ (got_int)
			    ನಿಲ್ಲಿಸು;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			ಸ್ಥಿತಿ_2 = RA_NOMATCH;

		    // If it could match, try it.
		    ಆದರೆ (rst->nextb == NUL || *rex.ಒಳಹರಿವು == rst->nextb
					     || *rex.ಒಳಹರಿವು == rst->nextb_ic)
		    {
			reg_save(&rp->rs_un.regsave, &backpos);
			ಪರಿಶೀಲನೆ = regnext(rp->rs_ಪರಿಶೀಲನೆ);
			ಸ್ಥಿತಿ_2 = RA_CONT;
			ನಿಲ್ಲಿಸು;
		    }
		}
		ಆದರೆ (ಸ್ಥಿತಿ_2 != RA_CONT)
		{
		    // Failed.
		    regstack_ತೆಗೆ(&ಪರಿಶೀಲನೆ);
		    regstack.ga_len -= ಗಾತ್ರ(regstar_T);
		    ಸ್ಥಿತಿ_2 = RA_NOMATCH;
		}
	    }
	    ನಿಲ್ಲಿಸು;
	}

	// If we want to continue the inner loop or didn't pop a state
	// continue matching loop
	ಆದರೆ (ಸ್ಥಿತಿ_2 == RA_CONT || rp == (regitem_T *)
			     ((ಅಕ್ಷರ *)regstack.ga_ದತ್ತಾಂಶ + regstack.ga_len) - 1)
	    ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
	ಆದರೆ (bt_did_ಸಮಯ_ಹೊರಹೋಗು(timed_ಹೊರಹೋಗು))
	{
	    ಸ್ಥಿತಿ_2 = RA_FAIL;
	    ನಿಲ್ಲಿಸು;
	}
#ಆದರೆ_ಅಂತ್ಯ
    }

    // May need to continue with the inner loop, starting at "scan".
    ಆದರೆ (ಸ್ಥಿತಿ_2 == RA_CONT)
	ಮುಂದುವರಿ;

    // If the regstack is empty or something failed we are done.
    ಆದರೆ (regstack.ga_len == 0 || ಸ್ಥಿತಿ_2 == RA_FAIL)
    {
	ಆದರೆ (ಪರಿಶೀಲನೆ == ಖಾಲಿ)
	{
	    // We get here only if there's trouble -- normally "case END" is
	    // the terminating point.
	    iemsg(e_corrupted_regexp_ಕಾರ್ಯಕ್ರಮ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
	    ಬರೆ("Premature EOL\n");
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಹಿಂತಿರುಗಿ (ಸ್ಥಿತಿ_2 == RA_ಹೊಂದಿಕೆ);
    }

  } // End of loop until the regstack is empty.

  // NOTREACHED
}

/*
 * regtry - try match of "prog" with at rex.line["col"].
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    ಸ್ಥಿರ ದೀರ್ಘ
regtry(
    bt_regprog_T	*prog,
    colnr_T		ಲಂಬಸಾಲು,
    ಪೂರ್ಣಾಂಕ			*timed_ಹೊರಹೋಗು)	// flag set on timeout or NULL
{
    rex.ಒಳಹರಿವು = rex.ಸಾಲು + ಲಂಬಸಾಲು;
    rex.need_clear_subexpr = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    rex.need_clear_zsubexpr = (prog->reghasz == REX_ಗಣ);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (regmatch(prog->ಕಾರ್ಯಕ್ರಮ + 1, timed_ಹೊರಹೋಗು) == 0)
	ಹಿಂತಿರುಗಿ 0;

    cleanup_subexpr();
    ಆದರೆ (REG_MULTI)
    {
	ಆದರೆ (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
	}
	ಆದರೆ (rex.reg_endpos[0].lnum < 0)
	{
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)(rex.ಒಳಹರಿವು - rex.ಸಾಲು);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (rex.reg_startp[0] == ಖಾಲಿ)
	    rex.reg_startp[0] = rex.ಸಾಲು + ಲಂಬಸಾಲು;
	ಆದರೆ (rex.reg_endp[0] == ಖಾಲಿ)
	    rex.reg_endp[0] = rex.ಒಳಹರಿವು;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_ಹೊರಹೋಗು);
    re_extmatch_ಹೊರಹೋಗು = ಖಾಲಿ;

    ಆದರೆ (prog->reghasz == REX_ಗಣ)
    {
	ಪೂರ್ಣಾಂಕ		i;

	cleanup_zsubexpr();
	re_extmatch_ಹೊರಹೋಗು = make_extmatch();
	ಆದರೆ (re_extmatch_ಹೊರಹೋಗು == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ 0;
	ಗಾಗಿ (i = 0; i < NSUBEXP; i++)
	{
	    ಆದರೆ (REG_MULTI)
	    {
		// Only accept single line matches.
		ಆದರೆ (reg_startzpos[i].lnum >= 0
			&& reg_endzpos[i].lnum == reg_startzpos[i].lnum
			&& reg_endzpos[i].ಲಂಬಸಾಲು >= reg_startzpos[i].ಲಂಬಸಾಲು)
		    re_extmatch_ಹೊರಹೋಗು->matches[i] =
			vim_strnsave(reg_getline(reg_startzpos[i].lnum)
						       + reg_startzpos[i].ಲಂಬಸಾಲು,
				   reg_endzpos[i].ಲಂಬಸಾಲು - reg_startzpos[i].ಲಂಬಸಾಲು);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (reg_startzp[i] != ಖಾಲಿ && reg_endzp[i] != ಖಾಲಿ)
		    re_extmatch_ಹೊರಹೋಗು->matches[i] =
			    vim_strnsave(reg_startzp[i],
						reg_endzp[i] - reg_startzp[i]);
	    }
	}
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    ಸ್ಥಿರ ದೀರ್ಘ
bt_regexec_both(
    char_u	*ಸಾಲು,
    colnr_T	startcol,	// column to start looking for match
    ಪೂರ್ಣಾಂಕ		*timed_ಹೊರಹೋಗು)	// flag set on timeout or NULL
{
    bt_regprog_T    *prog;
    char_u	    *s;
    colnr_T	    ಲಂಬಸಾಲು = startcol;
    ದೀರ್ಘ	    retval = 0L;

    // Create "regstack" and "backpos" if they are not allocated yet.
    // We allocate *_INITIAL amount of bytes first and then set the grow size
    // to much bigger value to avoid many malloc calls in case of deep regular
    // expressions.
    ಆದರೆ (regstack.ga_ದತ್ತಾಂಶ == ಖಾಲಿ)
    {
	// Use an item size of 1 byte, since we push different things
	// onto the regstack.
	ga_init2(&regstack, 1, REGSTACK_INITIAL);
	(ಶೂನ್ಯ)ga_grow(&regstack, REGSTACK_INITIAL);
	regstack.ga_growsize = REGSTACK_INITIAL * 8;
    }

    ಆದರೆ (backpos.ga_ದತ್ತಾಂಶ == ಖಾಲಿ)
    {
	ga_init2(&backpos, ಗಾತ್ರ(backpos_T), BACKPOS_INITIAL);
	(ಶೂನ್ಯ)ga_grow(&backpos, BACKPOS_INITIAL);
	backpos.ga_growsize = BACKPOS_INITIAL * 8;
    }

    ಆದರೆ (REG_MULTI)
    {
	prog = (bt_regprog_T *)rex.reg_mmatch->regprog;
	ಸಾಲು = reg_getline((linenr_T)0);
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	prog = (bt_regprog_T *)rex.reg_ಹೊಂದಿಕೆ->regprog;
	rex.reg_startp = rex.reg_ಹೊಂದಿಕೆ->startp;
	rex.reg_endp = rex.reg_ಹೊಂದಿಕೆ->endp;
    }

    // Be paranoid...
    ಆದರೆ (prog == ಖಾಲಿ || ಸಾಲು == ಖಾಲಿ)
    {
	iemsg(e_null_ವಾದ);
	ಹೋಗು theend;
    }

    // Check validity of program.
    ಆದರೆ (prog_magic_wrong())
	ಹೋಗು theend;

    // If the start column is past the maximum column: no need to try.
    ಆದರೆ (rex.reg_maxcol > 0 && ಲಂಬಸಾಲು >= rex.reg_maxcol)
	ಹೋಗು theend;

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    ಆದರೆ (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    ಆದರೆ (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    // If there is a "must appear" string, look for it.
    ಆದರೆ (prog->regmust != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ c;

	ಆದರೆ (has_mbyte)
	    c = (*mb_ptr2char)(prog->regmust);
	ಇಲ್ಲದಿದ್ದರೆ
	    c = *prog->regmust;
	s = ಸಾಲು + ಲಂಬಸಾಲು;

	// This is used very often, esp. for ":global".  Use three versions of
	// the loop to avoid overhead of conditions.
	ಆದರೆ (!rex.reg_ic && !has_mbyte)
	    ಆಗ ((s = vim_strbyte(s, c)) != ಖಾಲಿ)
	    {
		ಪೂರ್ಣಾಂಕ len = prog->regmlen;
		ಆದರೆ (cstrncmp(s, prog->regmust, &len) == 0)
		    ನಿಲ್ಲಿಸು;		// Found it.
		++s;
	    }
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	    ಆಗ ((s = vim_strchr(s, c)) != ಖಾಲಿ)
	    {
		ಪೂರ್ಣಾಂಕ len = prog->regmlen;
		ಆದರೆ (cstrncmp(s, prog->regmust, &len) == 0)
		    ನಿಲ್ಲಿಸು;		// Found it.
		MB_PTR_ADV(s);
	    }
	ಇಲ್ಲದಿದ್ದರೆ
	    ಆಗ ((s = cstrchr(s, c)) != ಖಾಲಿ)
	    {
		ಪೂರ್ಣಾಂಕ len = prog->regmlen;
		ಆದರೆ (cstrncmp(s, prog->regmust, &len) == 0)
		    ನಿಲ್ಲಿಸು;		// Found it.
		MB_PTR_ADV(s);
	    }
	ಆದರೆ (s == ಖಾಲಿ)		// Not present.
	    ಹೋಗು theend;
    }

    rex.ಸಾಲು = ಸಾಲು;
    rex.lnum = 0;
    reg_toolong = FALSE;

    // Simplest case: Anchored match need be tried only once.
    ಆದರೆ (prog->reganch)
    {
	ಪೂರ್ಣಾಂಕ	c;

	ಆದರೆ (has_mbyte)
	    c = (*mb_ptr2char)(rex.ಸಾಲು + ಲಂಬಸಾಲು);
	ಇಲ್ಲದಿದ್ದರೆ
	    c = rex.ಸಾಲು[ಲಂಬಸಾಲು];
	ಆದರೆ (prog->regstart == NUL
		|| prog->regstart == c
		|| (rex.reg_ic
		    && (((enc_utf8 && utf_fold(prog->regstart) == utf_fold(c)))
			|| (c < 255 && prog->regstart < 255 &&
			    MB_TOLOWER(prog->regstart) == MB_TOLOWER(c)))))
	    retval = regtry(prog, ಲಂಬಸಾಲು, timed_ಹೊರಹೋಗು);
	ಇಲ್ಲದಿದ್ದರೆ
	    retval = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Messy cases:  unanchored match.
	ಆಗ (!got_int)
	{
	    ಆದರೆ (prog->regstart != NUL)
	    {
		// Skip until the char we know it must start with.
		// Used often, do some work to avoid call overhead.
		ಆದರೆ (!rex.reg_ic && !has_mbyte)
		    s = vim_strbyte(rex.ಸಾಲು + ಲಂಬಸಾಲು, prog->regstart);
		ಇಲ್ಲದಿದ್ದರೆ
		    s = cstrchr(rex.ಸಾಲು + ಲಂಬಸಾಲು, prog->regstart);
		ಆದರೆ (s == ಖಾಲಿ)
		{
		    retval = 0;
		    ನಿಲ್ಲಿಸು;
		}
		ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)(s - rex.ಸಾಲು);
	    }

	    // Check for maximum column to try.
	    ಆದರೆ (rex.reg_maxcol > 0 && ಲಂಬಸಾಲು >= rex.reg_maxcol)
	    {
		retval = 0;
		ನಿಲ್ಲಿಸು;
	    }

	    retval = regtry(prog, ಲಂಬಸಾಲು, timed_ಹೊರಹೋಗು);
	    ಆದರೆ (retval > 0)
		ನಿಲ್ಲಿಸು;

	    // if not currently on the first line, get it again
	    ಆದರೆ (rex.lnum != 0)
	    {
		rex.lnum = 0;
		rex.ಸಾಲು = reg_getline((linenr_T)0);
	    }
	    ಆದರೆ (rex.ಸಾಲು[ಲಂಬಸಾಲು] == NUL)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (has_mbyte)
		ಲಂಬಸಾಲು += (*mb_ptr2len)(rex.ಸಾಲು + ಲಂಬಸಾಲು);
	    ಇಲ್ಲದಿದ್ದರೆ
		++ಲಂಬಸಾಲು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
	    ಆದರೆ (bt_did_ಸಮಯ_ಹೊರಹೋಗು(timed_ಹೊರಹೋಗು))
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	}
    }

theend:
    // Free "reg_tofree" when it's a bit big.
    // Free regstack and backpos if they are bigger than their initial size.
    ಆದರೆ (reg_tofreelen > 400)
	VIM_CLEAR(reg_tofree);
    ಆದರೆ (regstack.ga_maxlen > REGSTACK_INITIAL)
	ga_clear(&regstack);
    ಆದರೆ (backpos.ga_maxlen > BACKPOS_INITIAL)
	ga_clear(&backpos);

    ಆದರೆ (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs
	// and \ze are used.
	ಆದರೆ (REG_MULTI)
	{
	    lpos_T *ಆರಂಭ = &rex.reg_mmatch->startpos[0];
	    lpos_T *ಕೊನೆ = &rex.reg_mmatch->endpos[0];

	    ಆದರೆ (ಕೊನೆ->lnum < ಆರಂಭ->lnum
			|| (ಕೊನೆ->lnum == ಆರಂಭ->lnum && ಕೊನೆ->ಲಂಬಸಾಲು < ಆರಂಭ->ಲಂಬಸಾಲು))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_mmatch->rmm_matchcol = ಲಂಬಸಾಲು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (rex.reg_ಹೊಂದಿಕೆ->endp[0] < rex.reg_ಹೊಂದಿಕೆ->startp[0])
		rex.reg_ಹೊಂದಿಕೆ->endp[0] = rex.reg_ಹೊಂದಿಕೆ->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_ಹೊಂದಿಕೆ->rm_matchcol = ಲಂಬಸಾಲು;
	}
    }

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Match a regexp against a string.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 * if "line_lbr" is TRUE  consider a "\n" in "line" to be a line break.
 *
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
bt_regexec_nl(
    regmatch_T	*rmp,
    char_u	*ಸಾಲು,	// string to match against
    colnr_T	ಲಂಬಸಾಲು,	// column to start looking for match
    ಪೂರ್ಣಾಂಕ		ಸಾಲು_lbr)
{
    rex.reg_ಹೊಂದಿಕೆ = rmp;
    rex.reg_mmatch = ಖಾಲಿ;
    rex.reg_maxline = 0;
    rex.reg_ಸಾಲು_lbr = ಸಾಲು_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = ಖಾಲಿ;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;

    ಹಿಂತಿರುಗಿ bt_regexec_both(ಸಾಲು, ಲಂಬಸಾಲು, ಖಾಲಿ);
}

/*
 * Match a regexp against multiple lines.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 *
 * Return zero if there is no match.  Return number of lines contained in the
 * match otherwise.
 */
    ಸ್ಥಿರ ದೀರ್ಘ
bt_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	ಲಂಬಸಾಲು,		// column to start looking for match
    ಪೂರ್ಣಾಂಕ		*timed_ಹೊರಹೋಗು)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    ಹಿಂತಿರುಗಿ bt_regexec_both(ಖಾಲಿ, ಲಂಬಸಾಲು, timed_ಹೊರಹೋಗು);
}

/*
 * Compare a number with the operand of RE_LNUM, RE_COL or RE_VCOL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
re_ಅಂಕೆ_cmp(long_u val, char_u *ಪರಿಶೀಲನೆ)
{
    long_u  n = ಕಾರ್ಯಾಂಶ_MIN(ಪರಿಶೀಲನೆ);

    ಆದರೆ (ಕಾರ್ಯಾಂಶ_CMP(ಪರಿಶೀಲನೆ) == '>')
	ಹಿಂತಿರುಗಿ val > n;
    ಆದರೆ (ಕಾರ್ಯಾಂಶ_CMP(ಪರಿಶೀಲನೆ) == '<')
	ಹಿಂತಿರುಗಿ val < n;
    ಹಿಂತಿರುಗಿ val == n;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BT_REGEXP_DUMP

/*
 * regdump - dump a regexp onto stdout in vaguely comprehensible form
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
regdump(char_u *ನಮೂನೆ, bt_regprog_T *r)
{
    char_u  *s;
    ಪೂರ್ಣಾಂಕ	    op = EXACTLY;	// Arbitrary non-END op.
    char_u  *ಮುಂದಿನ;
    char_u  *ಕೊನೆ = ಖಾಲಿ;
    FILE    *f;

# ifdef BT_REGEXP_LOG
    f = fopen("bt_regexp_log.log", "a");
# ಇಲ್ಲದಿದ್ದರೆ
    f = stdout;
# endif
    ಆದರೆ (f == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    fprintf(f, "-------------------------------------\n\r\nregcomp(%s):\r\n", ನಮೂನೆ);

    s = r->ಕಾರ್ಯಕ್ರಮ + 1;
    // Loop until we find the END that isn't before a referred next (an END
    // can also appear in a NOMATCH operand).
    ಆಗ ((op != ಕೊನೆ_3 || s <= ಕೊನೆ) && s < r->ಕಾರ್ಯಕ್ರಮ + r->regsz)
    {
	op = OP(s);
	fprintf(f, "%2d%s", (ಪೂರ್ಣಾಂಕ)(s - r->ಕಾರ್ಯಕ್ರಮ), regprop(s)); // Where, what.
	ಮುಂದಿನ = (s + 3 <= r->ಕಾರ್ಯಕ್ರಮ + r->regsz) ? regnext(s) : ಖಾಲಿ;
	ಆದರೆ (ಮುಂದಿನ == ಖಾಲಿ)	// Next ptr.
	    fprintf(f, "(0)");
	ಇಲ್ಲದಿದ್ದರೆ
	    fprintf(f, "(%d)", (ಪೂರ್ಣಾಂಕ)((s - r->ಕಾರ್ಯಕ್ರಮ) + (ಮುಂದಿನ - s)));
	ಆದರೆ (ಕೊನೆ < ಮುಂದಿನ)
	    ಕೊನೆ = ಮುಂದಿನ;
	ಆದರೆ (op == BRACE_LIMITS)
	{
	    // Two ints
	    fprintf(f, " minval %ld, maxval %ld", ಕಾರ್ಯಾಂಶ_MIN(s), ಕಾರ್ಯಾಂಶ_MAX(s));
	    s += 8;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (op == BEHIND || op == NOBEHIND)
	{
	    // one int
	    fprintf(f, " count %ld", ಕಾರ್ಯಾಂಶ_MIN(s));
	    s += 4;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (op == RE_LNUM || op == RE_ಲಂಬಸಾಲು || op == RE_VCOL)
	{
	    // one int plus comparator
	    fprintf(f, " count %ld", ಕಾರ್ಯಾಂಶ_MIN(s));
	    s += 5;
	}
	s += 3;
	ಆದರೆ (op == MULTIBYTECODE)
	{
	    fprintf(f, " mbc=%d", utf_ptr2char(s));
	    s += utfc_ptr2len(s);
	}
	ಆದರೆ (op == ANYOF || op == ANYOF + ಕೂಡಿಸು_NL
		|| op == ANYBUT || op == ANYBUT + ಕೂಡಿಸು_NL
		|| op == EXACTLY)
	{
	    // Literal string, where present.
	    fprintf(f, "\nxxxxxxxxx\n");
	    ಆಗ (*s != NUL && s < r->ಕಾರ್ಯಕ್ರಮ + r->regsz)
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
    ಆದರೆ (r->regstart != NUL)
	fprintf(f, "start `%s' 0x%x; ", r->regstart < 256
		? (ಅಕ್ಷರ *)transchar(r->regstart)
		: "multibyte", r->regstart);
    ಆದರೆ (r->reganch)
	fprintf(f, "anchored; ");
    ಆದರೆ (r->regmust != ಖಾಲಿ)
	fprintf(f, "must have \"%s\"", r->regmust);
    fprintf(f, "\r\n");

# ifdef BT_REGEXP_LOG
    fclose(f);
# endif
}
#ಆದರೆ_ಅಂತ್ಯ	    // BT_REGEXP_DUMP

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
/*
 * regprop - printable representation of opcode
 */
    ಸ್ಥಿರ char_u *
regprop(char_u *op)
{
    ಅಕ್ಷರ	    *p;
    ಸ್ಥಿರ ಅಕ್ಷರ	    buf[50];
    ಸ್ಥಿರ size_t   buflen = 0;

    STRCPY(buf, ":");
    buflen = 1;

    ಆಯ್ಕೆ ((ಪೂರ್ಣಾಂಕ) OP(op))
    {
      ಸ್ಥಿತಿ BOL:
	p = "BOL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ EOL:
	p = "EOL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ RE_BOF:
	p = "BOF";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ RE_EOF:
	p = "EOF";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ CURSOR:
	p = "CURSOR";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ RE_VISUAL:
	p = "RE_VISUAL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ RE_LNUM:
	p = "RE_LNUM";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ RE_MARK:
	p = "RE_MARK";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ RE_ಲಂಬಸಾಲು:
	p = "RE_COL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ RE_VCOL:
	p = "RE_VCOL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ BOW:
	p = "BOW";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ EOW:
	p = "EOW";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ ANY:
	p = "ANY";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ ANY + ಕೂಡಿಸು_NL:
	p = "ANY+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ ANYOF:
	p = "ANYOF";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ ANYOF + ಕೂಡಿಸು_NL:
	p = "ANYOF+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ ANYBUT:
	p = "ANYBUT";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ ANYBUT + ಕೂಡಿಸು_NL:
	p = "ANYBUT+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ IDENT:
	p = "IDENT";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ IDENT + ಕೂಡಿಸು_NL:
	p = "IDENT+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ SIDENT:
	p = "SIDENT";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ SIDENT + ಕೂಡಿಸು_NL:
	p = "SIDENT+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ KWORD:
	p = "KWORD";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ KWORD + ಕೂಡಿಸು_NL:
	p = "KWORD+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ SKWORD:
	p = "SKWORD";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ SKWORD + ಕೂಡಿಸು_NL:
	p = "SKWORD+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ FNAME:
	p = "FNAME";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ FNAME + ಕೂಡಿಸು_NL:
	p = "FNAME+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ SFNAME:
	p = "SFNAME";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ SFNAME + ಕೂಡಿಸು_NL:
	p = "SFNAME+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ PRINT:
	p = "PRINT";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ PRINT + ಕೂಡಿಸು_NL:
	p = "PRINT+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ SPRINT:
	p = "SPRINT";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ SPRINT + ಕೂಡಿಸು_NL:
	p = "SPRINT+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ WHITE:
	p = "WHITE";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ WHITE + ಕೂಡಿಸು_NL:
	p = "WHITE+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NWHITE:
	p = "NWHITE";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NWHITE + ಕೂಡಿಸು_NL:
	p = "NWHITE+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ DIGIT:
	p = "DIGIT";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ DIGIT + ಕೂಡಿಸು_NL:
	p = "DIGIT+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NDIGIT:
	p = "NDIGIT";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NDIGIT + ಕೂಡಿಸು_NL:
	p = "NDIGIT+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ HEX:
	p = "HEX";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ HEX + ಕೂಡಿಸು_NL:
	p = "HEX+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NHEX:
	p = "NHEX";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NHEX + ಕೂಡಿಸು_NL:
	p = "NHEX+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ OCTAL:
	p = "OCTAL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ OCTAL + ಕೂಡಿಸು_NL:
	p = "OCTAL+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NOCTAL:
	p = "NOCTAL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NOCTAL + ಕೂಡಿಸು_NL:
	p = "NOCTAL+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ ಶಬ್ದ_2:
	p = "WORD";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ ಶಬ್ದ_2 + ಕೂಡಿಸು_NL:
	p = "WORD+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NWORD:
	p = "NWORD";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NWORD + ಕೂಡಿಸು_NL:
	p = "NWORD+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ ತಲೆ_2:
	p = "HEAD";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ ತಲೆ_2 + ಕೂಡಿಸು_NL:
	p = "HEAD+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NHEAD:
	p = "NHEAD";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NHEAD + ಕೂಡಿಸು_NL:
	p = "NHEAD+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ ALPHA:
	p = "ALPHA";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ ALPHA + ಕೂಡಿಸು_NL:
	p = "ALPHA+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NALPHA:
	p = "NALPHA";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NALPHA + ಕೂಡಿಸು_NL:
	p = "NALPHA+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ ಅಧೋ_2:
	p = "LOWER";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ ಅಧೋ_2 + ಕೂಡಿಸು_NL:
	p = "LOWER+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NLOWER:
	p = "NLOWER";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NLOWER + ಕೂಡಿಸು_NL:
	p = "NLOWER+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ ಊರ್ಧ್ವ:
	p = "UPPER";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ ಊರ್ಧ್ವ + ಕೂಡಿಸು_NL:
	p = "UPPER+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NUPPER:
	p = "NUPPER";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NUPPER + ಕೂಡಿಸು_NL:
	p = "NUPPER+NL";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ BRANCH:
	p = "BRANCH";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ EXACTLY:
	p = "EXACTLY";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NOTHING:
	p = "NOTHING";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ ಹಿಂಭಾಗ_2:
	p = "BACK";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ ಕೊನೆ_3:
	p = "END";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ MOPEN + 0:
	p = "MATCH START";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ MOPEN + 1:
      ಸ್ಥಿತಿ MOPEN + 2:
      ಸ್ಥಿತಿ MOPEN + 3:
      ಸ್ಥಿತಿ MOPEN + 4:
      ಸ್ಥಿತಿ MOPEN + 5:
      ಸ್ಥಿತಿ MOPEN + 6:
      ಸ್ಥಿತಿ MOPEN + 7:
      ಸ್ಥಿತಿ MOPEN + 8:
      ಸ್ಥಿತಿ MOPEN + 9:
	buflen += vim_snprintf(buf + buflen, ಗಾತ್ರ(buf) - buflen, "MOPEN%d", OP(op) - MOPEN);
	p = ಖಾಲಿ;
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ MCLOSE + 0:
	p = "MATCH END";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ MCLOSE + 1:
      ಸ್ಥಿತಿ MCLOSE + 2:
      ಸ್ಥಿತಿ MCLOSE + 3:
      ಸ್ಥಿತಿ MCLOSE + 4:
      ಸ್ಥಿತಿ MCLOSE + 5:
      ಸ್ಥಿತಿ MCLOSE + 6:
      ಸ್ಥಿತಿ MCLOSE + 7:
      ಸ್ಥಿತಿ MCLOSE + 8:
      ಸ್ಥಿತಿ MCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, ಗಾತ್ರ(buf) - buflen, "MCLOSE%d", OP(op) - MCLOSE);
	p = ಖಾಲಿ;
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ BACKREF + 1:
      ಸ್ಥಿತಿ BACKREF + 2:
      ಸ್ಥಿತಿ BACKREF + 3:
      ಸ್ಥಿತಿ BACKREF + 4:
      ಸ್ಥಿತಿ BACKREF + 5:
      ಸ್ಥಿತಿ BACKREF + 6:
      ಸ್ಥಿತಿ BACKREF + 7:
      ಸ್ಥಿತಿ BACKREF + 8:
      ಸ್ಥಿತಿ BACKREF + 9:
	buflen += vim_snprintf(buf + buflen, ಗಾತ್ರ(buf) - buflen, "BACKREF%d", OP(op) - BACKREF);
	p = ಖಾಲಿ;
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NOPEN:
	p = "NOPEN";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NCLOSE:
	p = "NCLOSE";
	ನಿಲ್ಲಿಸು;
# ifdef FEAT_SYN_HL
      ಸ್ಥಿತಿ ZOPEN + 1:
      ಸ್ಥಿತಿ ZOPEN + 2:
      ಸ್ಥಿತಿ ZOPEN + 3:
      ಸ್ಥಿತಿ ZOPEN + 4:
      ಸ್ಥಿತಿ ZOPEN + 5:
      ಸ್ಥಿತಿ ZOPEN + 6:
      ಸ್ಥಿತಿ ZOPEN + 7:
      ಸ್ಥಿತಿ ZOPEN + 8:
      ಸ್ಥಿತಿ ZOPEN + 9:
	buflen += vim_snprintf(buf + buflen, ಗಾತ್ರ(buf) - buflen, "ZOPEN%d", OP(op) - ZOPEN);
	p = ಖಾಲಿ;
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ ZCLOSE + 1:
      ಸ್ಥಿತಿ ZCLOSE + 2:
      ಸ್ಥಿತಿ ZCLOSE + 3:
      ಸ್ಥಿತಿ ZCLOSE + 4:
      ಸ್ಥಿತಿ ZCLOSE + 5:
      ಸ್ಥಿತಿ ZCLOSE + 6:
      ಸ್ಥಿತಿ ZCLOSE + 7:
      ಸ್ಥಿತಿ ZCLOSE + 8:
      ಸ್ಥಿತಿ ZCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, ಗಾತ್ರ(buf) - buflen, "ZCLOSE%d", OP(op) - ZCLOSE);
	p = ಖಾಲಿ;
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ ZREF + 1:
      ಸ್ಥಿತಿ ZREF + 2:
      ಸ್ಥಿತಿ ZREF + 3:
      ಸ್ಥಿತಿ ZREF + 4:
      ಸ್ಥಿತಿ ZREF + 5:
      ಸ್ಥಿತಿ ZREF + 6:
      ಸ್ಥಿತಿ ZREF + 7:
      ಸ್ಥಿತಿ ZREF + 8:
      ಸ್ಥಿತಿ ZREF + 9:
	buflen += vim_snprintf(buf + buflen, ಗಾತ್ರ(buf) - buflen, "ZREF%d", OP(op) - ZREF);
	p = ಖಾಲಿ;
	ನಿಲ್ಲಿಸು;
# endif
      ಸ್ಥಿತಿ ನಕ್ಷತ್ರ:
	p = "STAR";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ PLUS:
	p = "PLUS";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NOMATCH:
	p = "NOMATCH";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ ಹೊಂದಿಕೆ_2:
	p = "MATCH";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ BEHIND:
	p = "BEHIND";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NOBEHIND:
	p = "NOBEHIND";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ SUBPAT:
	p = "SUBPAT";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ BRACE_LIMITS:
	p = "BRACE_LIMITS";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ BRACE_SIMPLE:
	p = "BRACE_SIMPLE";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ BRACE_COMPLEX + 0:
      ಸ್ಥಿತಿ BRACE_COMPLEX + 1:
      ಸ್ಥಿತಿ BRACE_COMPLEX + 2:
      ಸ್ಥಿತಿ BRACE_COMPLEX + 3:
      ಸ್ಥಿತಿ BRACE_COMPLEX + 4:
      ಸ್ಥಿತಿ BRACE_COMPLEX + 5:
      ಸ್ಥಿತಿ BRACE_COMPLEX + 6:
      ಸ್ಥಿತಿ BRACE_COMPLEX + 7:
      ಸ್ಥಿತಿ BRACE_COMPLEX + 8:
      ಸ್ಥಿತಿ BRACE_COMPLEX + 9:
	buflen += vim_snprintf(buf + buflen, ಗಾತ್ರ(buf) - buflen, "BRACE_COMPLEX%d", OP(op) - BRACE_COMPLEX);
	p = ಖಾಲಿ;
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ MULTIBYTECODE:
	p = "MULTIBYTECODE";
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NEWL:
	p = "NEWL";
	ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ:
	buflen += vim_snprintf(buf + buflen, ಗಾತ್ರ(buf) - buflen, "corrupt %d", OP(op));
	p = ಖಾಲಿ;
	ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ (p != ಖಾಲಿ)
	vim_strncpy((char_u *)buf + buflen, (char_u *)p, ಗಾತ್ರ(buf) - buflen - 1);
    ಹಿಂತಿರುಗಿ (char_u *)buf;
}
#ಆದರೆ_ಅಂತ್ಯ	    // DEBUG
