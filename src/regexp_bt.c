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
#define শীর্ষ_4		41	//	Match head char
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

স্থির পূর্ণসংখ্যা	অঙ্ক_complex_braces; // Complex \{...} count
স্থির char_u	*regcode;	// Code-emit pointer, or JUST_CALC_SIZE
স্থির long	regsize;	// Code size.
স্থির পূর্ণসংখ্যা	reg_toolong;	// TRUE when offset out of range
স্থির পূর্ণসংখ্যা	bt_reg_parse_depth;	// nesting depth in reg()
স্থির char_u	had_endbrace[NSUBEXP];	// flags, TRUE if end of () found
স্থির long	brace_min[10];	// Minimums for complex brace repeats
স্থির long	brace_max[10];	// Maximums for complex brace repeats
স্থির পূর্ণসংখ্যা	brace_গুনতি[10]; // Current counts for complex brace repeats
স্থির পূর্ণসংখ্যা	one_exactly = FALSE;	// only do one char for EXACTLY

// When making changes to classchars also change nfa_classcodes.
স্থির char_u	*classchars = (char_u *)".iIkKfFpPsSdDxXoOwWhHaAlLuU";
স্থির ধ্রুবক পূর্ণসংখ্যা classcodes[] = {
    ANY, IDENT, SIDENT, KWORD, SKWORD,
    FNAME, SFNAME, PRINT, SPRINT,
    WHITE, NWHITE, DIGIT, NDIGIT,
    HEX, NHEX, OCTAL, NOCTAL,
    WORD, NWORD, শীর্ষ_4, NHEAD,
    ALPHA, NALPHA, LOWER, NLOWER,
    UPPER, NUPPER
};

/*
 * Search between "start" (the first char of the range) and "end" (the closing
 * "]") and try to recognize a character class in expanded form, for example
 * [0-9].  On success, return the atom to be emitted, on failure 0.
 */
    স্থির পূর্ণসংখ্যা
bt_recognize_char_class(char_u *start, char_u *end)
{
    পূর্ণসংখ্যা		newl;
    পূর্ণসংখ্যা		config = get_char_class_bits(start, end, &newl);

    // A newline would need the ADD_NL variant, don't bother with it here.
    যদি (config < 0 || newl)
	ফেরত 0;

	// The old engine has no case-insensitive class opcode, so [a-z] and [A-Z]
	// are left as collections.  The classes below are case-independent.
    নির্বাচন (config)
    {
	ক্ষেত্র CLASS_o9:
	    ফেরত DIGIT;
	ক্ষেত্র CLASS_not | CLASS_o9:
	    ফেরত NDIGIT;
	ক্ষেত্র CLASS_af | CLASS_AF | CLASS_o9:
	    ফেরত HEX;
	ক্ষেত্র CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    ফেরত NHEX;
	ক্ষেত্র CLASS_o7:
	    ফেরত OCTAL;
	ক্ষেত্র CLASS_not | CLASS_o7:
	    ফেরত NOCTAL;
	ক্ষেত্র CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    ফেরত WORD;
	ক্ষেত্র CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    ফেরত NWORD;
	ক্ষেত্র CLASS_az | CLASS_AZ | CLASS_underscore:
	    ফেরত শীর্ষ_4;
	ক্ষেত্র CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    ফেরত NHEAD;
	ক্ষেত্র CLASS_az | CLASS_AZ:
	    ফেরত ALPHA;
	ক্ষেত্র CLASS_not | CLASS_az | CLASS_AZ:
	    ফেরত NALPHA;
    }
    ফেরত 0;
}

/*
 * When regcode is set to this value, code is not emitted and size is computed
 * instead.
 */
#define JUST_CALC_আকার	((char_u *) -1)

// Values for rs_state in regitem_T.
প্রকারলেখ গণনা regstate_E
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
প্রকারলেখ কাঠামো
{
    সংঘ
    {
	char_u	*ptr;	// rex.input pointer, for single-line regexp
	lpos_T	pos;	// rex.input pos, for multi-line regexp
    } rs_u;
    পূর্ণসংখ্যা		rs_len;
} regsave_T;

// struct to save start/end pointer/position in for \(\)
প্রকারলেখ কাঠামো
{
    সংঘ
    {
	char_u	*ptr;
	lpos_T	pos;
    } se_u;
} save_se_T;

// used for BEHIND and NOBEHIND matching
প্রকারলেখ কাঠামো regbehind_S
{
    regsave_T	save_after;
    regsave_T	save_behind;
    পূর্ণসংখ্যা		save_need_clear_subexpr;
    save_se_T   save_start[NSUBEXP];
    save_se_T   save_end[NSUBEXP];
} regbehind_T;

/*
 * When there are alternatives a regstate_T is put on the regstack to remember
 * what we are doing.
 * Before it may be another type of item, depending on rs_state, to remember
 * more things.
 */
প্রকারলেখ কাঠামো regitem_S
{
    regstate_T	rs_state;	// what we are doing, one of RS_ above
    short	rs_no;		// submatch nr or BEHIND/NOBEHIND
    char_u	*rs_scan;	// current node in program
    সংঘ
    {
	save_se_T  sesave;
	regsave_T  regsave;
    } rs_un;			// room for saving rex.input
} regitem_T;


// used for STAR, PLUS and BRACE_SIMPLE matching
প্রকারলেখ কাঠামো regstar_S
{
    পূর্ণসংখ্যা		nextb;		// next byte
    পূর্ণসংখ্যা		nextb_ic;	// next byte reverse case
    long	গুনতি;
    long	minval;
    long	maxval;
} regstar_T;

// used to store input position when a BACK was encountered, so that we now if
// we made any progress since the last time.
প্রকারলেখ কাঠামো backpos_S
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
স্থির garray_T	regstack = {0, 0, 0, 0, ফাঁকা};
স্থির garray_T	backpos = {0, 0, 0, 0, ফাঁকা};

স্থির regsave_T behind_pos;

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
#define OP(p)		((পূর্ণসংখ্যা)*(p))
#define পরবর্তী_2(p)		(((*((p) + 1) & 0377) << 8) + (*((p) + 2) & 0377))
#define OPERAND(p)	((p) + 3)
// Obtain an operand that was stored as four bytes, MSB first.
#define OPERAND_MIN(p)	(((long)(p)[3] << 24) + ((long)(p)[4] << 16) \
			+ ((long)(p)[5] << 8) + (long)(p)[6])
// Obtain a second operand stored as four bytes.
#define OPERAND_MAX(p)	OPERAND_MIN((p) + 4)
// Obtain a second single-byte operand stored after a four bytes operand.
#define OPERAND_CMP(p)	(p)[7]

স্থির char_u *reg(পূর্ণসংখ্যা paren, পূর্ণসংখ্যা *flagp);

#ifdef BT_REGEXP_DUMP
স্থির শূন্য	regdump(char_u *, bt_regprog_T *);
#endif

স্থির পূর্ণসংখ্যা	re_অঙ্ক_cmp(long_u val, char_u *scan);

#ifdef DEBUG
স্থির char_u	*regprop(char_u *);

স্থির পূর্ণসংখ্যা	regnarrate = 0;
#endif


/*
 * Setup to parse the regexp.  Used once to get the length and once to do it.
 */
    স্থির শূন্য
regcomp_start(
    char_u	*রাশিমালা,
    পূর্ণসংখ্যা		re_flags)	    // see vim_regcomp()
{
    initchr(রাশিমালা);
    যদি (re_flags & RE_MAGIC)
	reg_magic = MAGIC_ON;
    অন্যথা
	reg_magic = MAGIC_OFF;
    reg_string = (re_flags & RE_STRING);
    reg_strict = (re_flags & RE_STRICT);
    get_cpo_flags();

    অঙ্ক_complex_braces = 0;
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
#যদি defined(FEAT_SYN_HL)
    had_eol = FALSE;
#endif
}

/*
 * Return TRUE if MULTIBYTECODE should be used instead of EXACTLY for
 * character "c".
 */
    স্থির পূর্ণসংখ্যা
use_multibytecode(পূর্ণসংখ্যা c)
{
    ফেরত has_mbyte && (*mb_char2len)(c) > 1
		     && (re_multi_type(peekchr()) != NOT_MULTI
			     || (enc_utf8 && utf_iscomposing(c)));
}

/*
 * Emit (if appropriate) a byte of code
 */
    স্থির শূন্য
regc(পূর্ণসংখ্যা b)
{
    যদি (regcode == JUST_CALC_আকার)
	regsize++;
    অন্যথা
	*regcode++ = b;
}

/*
 * Emit (if appropriate) a multi-byte character of code
 */
    স্থির শূন্য
regmbc(পূর্ণসংখ্যা c)
{
    যদি (!has_mbyte && c > 0xff)
	ফেরত;
    যদি (regcode == JUST_CALC_আকার)
	regsize += (*mb_char2len)(c);
    অন্যথা
	regcode += (*mb_char2bytes)(c, regcode);
}


/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * NOTE: When changing this function, also change nfa_emit_equi_class()
 */
    স্থির শূন্য
reg_equi_class(পূর্ণসংখ্যা c)
{
    যদি (enc_utf8 || STRCMP(p_enc, "latin1") == 0
					 || STRCMP(p_enc, "iso-8859-15") == 0)
    {
	নির্বাচন (c)
	{
	    // Do not use '\300' style, it results in a negative number.
	    ক্ষেত্র 'A': ক্ষেত্র 0xc0: ক্ষেত্র 0xc1: ক্ষেত্র 0xc2: ক্ষেত্র 0xc3: ক্ষেত্র 0xc4:
	    ক্ষেত্র 0xc5: ক্ষেত্র 0x100: ক্ষেত্র 0x102: ক্ষেত্র 0x104: ক্ষেত্র 0x1cd:
	    ক্ষেত্র 0x1de: ক্ষেত্র 0x1e0: ক্ষেত্র 0x1fa: ক্ষেত্র 0x202: ক্ষেত্র 0x226:
	    ক্ষেত্র 0x23a: ক্ষেত্র 0x1e00: ক্ষেত্র 0x1ea0: ক্ষেত্র 0x1ea2: ক্ষেত্র 0x1ea4:
	    ক্ষেত্র 0x1ea6: ক্ষেত্র 0x1ea8: ক্ষেত্র 0x1eaa: ক্ষেত্র 0x1eac: ক্ষেত্র 0x1eae:
	    ক্ষেত্র 0x1eb0: ক্ষেত্র 0x1eb2: ক্ষেত্র 0x1eb4: ক্ষেত্র 0x1eb6:
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
		      ফেরত;
	    ক্ষেত্র 'B': ক্ষেত্র 0x181: ক্ষেত্র 0x243: ক্ষেত্র 0x1e02:
	    ক্ষেত্র 0x1e04: ক্ষেত্র 0x1e06:
		      regmbc('B');
		      regmbc(0x181); regmbc(0x243); regmbc(0x1e02);
		      regmbc(0x1e04); regmbc(0x1e06);
		      ফেরত;
	    ক্ষেত্র 'C': ক্ষেত্র 0xc7:
	    ক্ষেত্র 0x106: ক্ষেত্র 0x108: ক্ষেত্র 0x10a: ক্ষেত্র 0x10c: ক্ষেত্র 0x187:
	    ক্ষেত্র 0x23b: ক্ষেত্র 0x1e08: ক্ষেত্র 0xa792:
		      regmbc('C'); regmbc(0xc7);
		      regmbc(0x106); regmbc(0x108); regmbc(0x10a);
		      regmbc(0x10c); regmbc(0x187); regmbc(0x23b);
		      regmbc(0x1e08); regmbc(0xa792);
		      ফেরত;
	    ক্ষেত্র 'D': ক্ষেত্র 0x10e: ক্ষেত্র 0x110: ক্ষেত্র 0x18a:
	    ক্ষেত্র 0x1e0a: ক্ষেত্র 0x1e0c: ক্ষেত্র 0x1e0e: ক্ষেত্র 0x1e10:
	    ক্ষেত্র 0x1e12:
		      regmbc('D'); regmbc(0x10e); regmbc(0x110);
		      regmbc(0x18a); regmbc(0x1e0a); regmbc(0x1e0c);
		      regmbc(0x1e0e); regmbc(0x1e10); regmbc(0x1e12);
		      ফেরত;
	    ক্ষেত্র 'E': ক্ষেত্র 0xc8: ক্ষেত্র 0xc9: ক্ষেত্র 0xca: ক্ষেত্র 0xcb:
	    ক্ষেত্র 0x112: ক্ষেত্র 0x114: ক্ষেত্র 0x116: ক্ষেত্র 0x118: ক্ষেত্র 0x11a:
	    ক্ষেত্র 0x204: ক্ষেত্র 0x206: ক্ষেত্র 0x228: ক্ষেত্র 0x246: ক্ষেত্র 0x1e14:
	    ক্ষেত্র 0x1e16: ক্ষেত্র 0x1e18: ক্ষেত্র 0x1e1a: ক্ষেত্র 0x1e1c:
	    ক্ষেত্র 0x1eb8: ক্ষেত্র 0x1eba: ক্ষেত্র 0x1ebc: ক্ষেত্র 0x1ebe:
	    ক্ষেত্র 0x1ec0: ক্ষেত্র 0x1ec2: ক্ষেত্র 0x1ec4: ক্ষেত্র 0x1ec6:
		      regmbc('E'); regmbc(0xc8); regmbc(0xc9);
		      regmbc(0xca); regmbc(0xcb); regmbc(0x112);
		      regmbc(0x114); regmbc(0x116); regmbc(0x118);
		      regmbc(0x11a); regmbc(0x204); regmbc(0x206);
		      regmbc(0x228); regmbc(0x246); regmbc(0x1e14);
		      regmbc(0x1e16); regmbc(0x1e18); regmbc(0x1e1a);
		      regmbc(0x1e1c); regmbc(0x1eb8); regmbc(0x1eba);
		      regmbc(0x1ebc); regmbc(0x1ebe); regmbc(0x1ec0);
		      regmbc(0x1ec2); regmbc(0x1ec4); regmbc(0x1ec6);
		      ফেরত;
	    ক্ষেত্র 'F': ক্ষেত্র 0x191: ক্ষেত্র 0x1e1e: ক্ষেত্র 0xa798:
		      regmbc('F'); regmbc(0x191); regmbc(0x1e1e);
		      regmbc(0xa798);
		      ফেরত;
	    ক্ষেত্র 'G': ক্ষেত্র 0x11c: ক্ষেত্র 0x11e: ক্ষেত্র 0x120:
	    ক্ষেত্র 0x122: ক্ষেত্র 0x193: ক্ষেত্র 0x1e4: ক্ষেত্র 0x1e6:
	    ক্ষেত্র 0x1f4: ক্ষেত্র 0x1e20: ক্ষেত্র 0xa7a0:
		      regmbc('G'); regmbc(0x11c); regmbc(0x11e);
		      regmbc(0x120); regmbc(0x122); regmbc(0x193);
		      regmbc(0x1e4); regmbc(0x1e6); regmbc(0x1f4);
		      regmbc(0x1e20); regmbc(0xa7a0);
		      ফেরত;
	    ক্ষেত্র 'H': ক্ষেত্র 0x124: ক্ষেত্র 0x126: ক্ষেত্র 0x21e:
	    ক্ষেত্র 0x1e22: ক্ষেত্র 0x1e24: ক্ষেত্র 0x1e26:
	    ক্ষেত্র 0x1e28: ক্ষেত্র 0x1e2a: ক্ষেত্র 0x2c67:
		      regmbc('H'); regmbc(0x124); regmbc(0x126);
		      regmbc(0x21e); regmbc(0x1e22); regmbc(0x1e24);
		      regmbc(0x1e26); regmbc(0x1e28); regmbc(0x1e2a);
		      regmbc(0x2c67);
		      ফেরত;
	    ক্ষেত্র 'I': ক্ষেত্র 0xcc: ক্ষেত্র 0xcd: ক্ষেত্র 0xce: ক্ষেত্র 0xcf:
	    ক্ষেত্র 0x128: ক্ষেত্র 0x12a: ক্ষেত্র 0x12c: ক্ষেত্র 0x12e:
	    ক্ষেত্র 0x130: ক্ষেত্র 0x197: ক্ষেত্র 0x1cf: ক্ষেত্র 0x208:
	    ক্ষেত্র 0x20a: ক্ষেত্র 0x1e2c: ক্ষেত্র 0x1e2e: ক্ষেত্র 0x1ec8:
	    ক্ষেত্র 0x1eca:
		      regmbc('I'); regmbc(0xcc); regmbc(0xcd);
		      regmbc(0xce); regmbc(0xcf); regmbc(0x128);
		      regmbc(0x12a); regmbc(0x12c); regmbc(0x12e);
		      regmbc(0x130); regmbc(0x197); regmbc(0x1cf);
		      regmbc(0x208); regmbc(0x20a); regmbc(0x1e2c);
		      regmbc(0x1e2e); regmbc(0x1ec8); regmbc(0x1eca);
		      ফেরত;
	    ক্ষেত্র 'J': ক্ষেত্র 0x134: ক্ষেত্র 0x248:
		      regmbc('J'); regmbc(0x134); regmbc(0x248);
		      ফেরত;
	    ক্ষেত্র 'K': ক্ষেত্র 0x136: ক্ষেত্র 0x198: ক্ষেত্র 0x1e8: ক্ষেত্র 0x1e30:
	    ক্ষেত্র 0x1e32: ক্ষেত্র 0x1e34: ক্ষেত্র 0x2c69: ক্ষেত্র 0xa740:
		      regmbc('K'); regmbc(0x136); regmbc(0x198);
		      regmbc(0x1e8); regmbc(0x1e30); regmbc(0x1e32);
		      regmbc(0x1e34); regmbc(0x2c69); regmbc(0xa740);
		      ফেরত;
	    ক্ষেত্র 'L': ক্ষেত্র 0x139: ক্ষেত্র 0x13b: ক্ষেত্র 0x13d: ক্ষেত্র 0x13f:
	    ক্ষেত্র 0x141: ক্ষেত্র 0x23d: ক্ষেত্র 0x1e36: ক্ষেত্র 0x1e38:
	    ক্ষেত্র 0x1e3a: ক্ষেত্র 0x1e3c: ক্ষেত্র 0x2c60:
		      regmbc('L'); regmbc(0x139); regmbc(0x13b);
		      regmbc(0x13d); regmbc(0x13f); regmbc(0x141);
		      regmbc(0x23d); regmbc(0x1e36); regmbc(0x1e38);
		      regmbc(0x1e3a); regmbc(0x1e3c); regmbc(0x2c60);
		      ফেরত;
	    ক্ষেত্র 'M': ক্ষেত্র 0x1e3e: ক্ষেত্র 0x1e40: ক্ষেত্র 0x1e42:
		      regmbc('M'); regmbc(0x1e3e); regmbc(0x1e40);
		      regmbc(0x1e42);
		      ফেরত;
	    ক্ষেত্র 'N': ক্ষেত্র 0xd1:
	    ক্ষেত্র 0x143: ক্ষেত্র 0x145: ক্ষেত্র 0x147: ক্ষেত্র 0x1f8:
	    ক্ষেত্র 0x1e44: ক্ষেত্র 0x1e46: ক্ষেত্র 0x1e48: ক্ষেত্র 0x1e4a:
	    ক্ষেত্র 0xa7a4:
		      regmbc('N'); regmbc(0xd1);
		      regmbc(0x143); regmbc(0x145); regmbc(0x147);
		      regmbc(0x1f8); regmbc(0x1e44); regmbc(0x1e46);
		      regmbc(0x1e48); regmbc(0x1e4a); regmbc(0xa7a4);
		      ফেরত;
	    ক্ষেত্র 'O': ক্ষেত্র 0xd2: ক্ষেত্র 0xd3: ক্ষেত্র 0xd4: ক্ষেত্র 0xd5: ক্ষেত্র 0xd6:
	    ক্ষেত্র 0xd8: ক্ষেত্র 0x14c: ক্ষেত্র 0x14e: ক্ষেত্র 0x150: ক্ষেত্র 0x19f:
	    ক্ষেত্র 0x1a0: ক্ষেত্র 0x1d1: ক্ষেত্র 0x1ea: ক্ষেত্র 0x1ec: ক্ষেত্র 0x1fe:
	    ক্ষেত্র 0x20c: ক্ষেত্র 0x20e: ক্ষেত্র 0x22a: ক্ষেত্র 0x22c: ক্ষেত্র 0x22e:
	    ক্ষেত্র 0x230: ক্ষেত্র 0x1e4c: ক্ষেত্র 0x1e4e: ক্ষেত্র 0x1e50: ক্ষেত্র 0x1e52:
	    ক্ষেত্র 0x1ecc: ক্ষেত্র 0x1ece: ক্ষেত্র 0x1ed0: ক্ষেত্র 0x1ed2: ক্ষেত্র 0x1ed4:
	    ক্ষেত্র 0x1ed6: ক্ষেত্র 0x1ed8: ক্ষেত্র 0x1eda: ক্ষেত্র 0x1edc: ক্ষেত্র 0x1ede:
	    ক্ষেত্র 0x1ee0: ক্ষেত্র 0x1ee2:
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
		      ফেরত;
	    ক্ষেত্র 'P': ক্ষেত্র 0x1a4: ক্ষেত্র 0x1e54: ক্ষেত্র 0x1e56: ক্ষেত্র 0x2c63:
		      regmbc('P'); regmbc(0x1a4); regmbc(0x1e54);
		      regmbc(0x1e56); regmbc(0x2c63);
		      ফেরত;
	    ক্ষেত্র 'Q': ক্ষেত্র 0x24a:
		      regmbc('Q'); regmbc(0x24a);
		      ফেরত;
	    ক্ষেত্র 'R': ক্ষেত্র 0x154: ক্ষেত্র 0x156: ক্ষেত্র 0x158: ক্ষেত্র 0x210:
	    ক্ষেত্র 0x212: ক্ষেত্র 0x24c: ক্ষেত্র 0x1e58: ক্ষেত্র 0x1e5a:
	    ক্ষেত্র 0x1e5c: ক্ষেত্র 0x1e5e: ক্ষেত্র 0x2c64: ক্ষেত্র 0xa7a6:
		      regmbc('R'); regmbc(0x154); regmbc(0x156);
		      regmbc(0x210); regmbc(0x212); regmbc(0x158);
		      regmbc(0x24c); regmbc(0x1e58); regmbc(0x1e5a);
		      regmbc(0x1e5c); regmbc(0x1e5e); regmbc(0x2c64);
		      regmbc(0xa7a6);
		      ফেরত;
	    ক্ষেত্র 'S': ক্ষেত্র 0x15a: ক্ষেত্র 0x15c: ক্ষেত্র 0x15e: ক্ষেত্র 0x160:
	    ক্ষেত্র 0x218: ক্ষেত্র 0x1e60: ক্ষেত্র 0x1e62: ক্ষেত্র 0x1e64:
	    ক্ষেত্র 0x1e66: ক্ষেত্র 0x1e68: ক্ষেত্র 0x2c7e: ক্ষেত্র 0xa7a8:
		      regmbc('S'); regmbc(0x15a); regmbc(0x15c);
		      regmbc(0x15e); regmbc(0x160); regmbc(0x218);
		      regmbc(0x1e60); regmbc(0x1e62); regmbc(0x1e64);
		      regmbc(0x1e66); regmbc(0x1e68); regmbc(0x2c7e);
			  regmbc(0xa7a8);
		      ফেরত;
	    ক্ষেত্র 'T': ক্ষেত্র 0x162: ক্ষেত্র 0x164: ক্ষেত্র 0x166: ক্ষেত্র 0x1ac:
	    ক্ষেত্র 0x1ae: ক্ষেত্র 0x21a: ক্ষেত্র 0x23e: ক্ষেত্র 0x1e6a: ক্ষেত্র 0x1e6c:
	    ক্ষেত্র 0x1e6e: ক্ষেত্র 0x1e70:
		      regmbc('T'); regmbc(0x162); regmbc(0x164);
		      regmbc(0x166); regmbc(0x1ac); regmbc(0x23e);
		      regmbc(0x1ae); regmbc(0x21a); regmbc(0x1e6a);
		      regmbc(0x1e6c); regmbc(0x1e6e); regmbc(0x1e70);
		      ফেরত;
	    ক্ষেত্র 'U': ক্ষেত্র 0xd9: ক্ষেত্র 0xda: ক্ষেত্র 0xdb: ক্ষেত্র 0xdc:
	    ক্ষেত্র 0x168: ক্ষেত্র 0x16a: ক্ষেত্র 0x16c: ক্ষেত্র 0x16e:
	    ক্ষেত্র 0x170: ক্ষেত্র 0x172: ক্ষেত্র 0x1af: ক্ষেত্র 0x1d3:
	    ক্ষেত্র 0x1d5: ক্ষেত্র 0x1d7: ক্ষেত্র 0x1d9: ক্ষেত্র 0x1db:
	    ক্ষেত্র 0x214: ক্ষেত্র 0x216: ক্ষেত্র 0x244: ক্ষেত্র 0x1e72:
	    ক্ষেত্র 0x1e74: ক্ষেত্র 0x1e76: ক্ষেত্র 0x1e78: ক্ষেত্র 0x1e7a:
	    ক্ষেত্র 0x1ee4: ক্ষেত্র 0x1ee6: ক্ষেত্র 0x1ee8: ক্ষেত্র 0x1eea:
	    ক্ষেত্র 0x1eec: ক্ষেত্র 0x1eee: ক্ষেত্র 0x1ef0:
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
		      ফেরত;
	    ক্ষেত্র 'V': ক্ষেত্র 0x1b2: ক্ষেত্র 0x1e7c: ক্ষেত্র 0x1e7e:
		      regmbc('V'); regmbc(0x1b2); regmbc(0x1e7c);
		      regmbc(0x1e7e);
		      ফেরত;
	    ক্ষেত্র 'W': ক্ষেত্র 0x174: ক্ষেত্র 0x1e80: ক্ষেত্র 0x1e82:
	    ক্ষেত্র 0x1e84: ক্ষেত্র 0x1e86: ক্ষেত্র 0x1e88:
		      regmbc('W'); regmbc(0x174); regmbc(0x1e80);
		      regmbc(0x1e82); regmbc(0x1e84); regmbc(0x1e86);
		      regmbc(0x1e88);
		      ফেরত;
	    ক্ষেত্র 'X': ক্ষেত্র 0x1e8a: ক্ষেত্র 0x1e8c:
		      regmbc('X'); regmbc(0x1e8a); regmbc(0x1e8c);
		      ফেরত;
	    ক্ষেত্র 'Y': ক্ষেত্র 0xdd:
	    ক্ষেত্র 0x176: ক্ষেত্র 0x178: ক্ষেত্র 0x1b3: ক্ষেত্র 0x232: ক্ষেত্র 0x24e:
	    ক্ষেত্র 0x1e8e: ক্ষেত্র 0x1ef2: ক্ষেত্র 0x1ef6: ক্ষেত্র 0x1ef4: ক্ষেত্র 0x1ef8:
		      regmbc('Y'); regmbc(0xdd); regmbc(0x176);
		      regmbc(0x178); regmbc(0x1b3); regmbc(0x232);
		      regmbc(0x24e); regmbc(0x1e8e); regmbc(0x1ef2);
		      regmbc(0x1ef4); regmbc(0x1ef6); regmbc(0x1ef8);
		      ফেরত;
	    ক্ষেত্র 'Z': ক্ষেত্র 0x179: ক্ষেত্র 0x17b: ক্ষেত্র 0x17d: ক্ষেত্র 0x1b5:
	    ক্ষেত্র 0x1e90: ক্ষেত্র 0x1e92: ক্ষেত্র 0x1e94: ক্ষেত্র 0x2c6b:
		      regmbc('Z'); regmbc(0x179); regmbc(0x17b);
		      regmbc(0x17d); regmbc(0x1b5); regmbc(0x1e90);
		      regmbc(0x1e92); regmbc(0x1e94); regmbc(0x2c6b);
		      ফেরত;
	    ক্ষেত্র 'a': ক্ষেত্র 0xe0: ক্ষেত্র 0xe1: ক্ষেত্র 0xe2:
	    ক্ষেত্র 0xe3: ক্ষেত্র 0xe4: ক্ষেত্র 0xe5: ক্ষেত্র 0x101: ক্ষেত্র 0x103:
	    ক্ষেত্র 0x105: ক্ষেত্র 0x1ce: ক্ষেত্র 0x1df: ক্ষেত্র 0x1e1: ক্ষেত্র 0x1fb:
	    ক্ষেত্র 0x201: ক্ষেত্র 0x203: ক্ষেত্র 0x227: ক্ষেত্র 0x1d8f: ক্ষেত্র 0x1e01:
	    ক্ষেত্র 0x1e9a: ক্ষেত্র 0x1ea1: ক্ষেত্র 0x1ea3: ক্ষেত্র 0x1ea5:
	    ক্ষেত্র 0x1ea7: ক্ষেত্র 0x1ea9: ক্ষেত্র 0x1eab: ক্ষেত্র 0x1ead:
	    ক্ষেত্র 0x1eaf: ক্ষেত্র 0x1eb1: ক্ষেত্র 0x1eb3: ক্ষেত্র 0x1eb5:
	    ক্ষেত্র 0x1eb7: ক্ষেত্র 0x2c65:
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
		      ফেরত;
	    ক্ষেত্র 'b': ক্ষেত্র 0x180: ক্ষেত্র 0x253: ক্ষেত্র 0x1d6c: ক্ষেত্র 0x1d80:
	    ক্ষেত্র 0x1e03: ক্ষেত্র 0x1e05: ক্ষেত্র 0x1e07:
		      regmbc('b');
		      regmbc(0x180); regmbc(0x253); regmbc(0x1d6c);
		      regmbc(0x1d80); regmbc(0x1e03); regmbc(0x1e05);
		      regmbc(0x1e07);
		      ফেরত;
	    ক্ষেত্র 'c': ক্ষেত্র 0xe7:
	    ক্ষেত্র 0x107: ক্ষেত্র 0x109: ক্ষেত্র 0x10b: ক্ষেত্র 0x10d: ক্ষেত্র 0x188:
	    ক্ষেত্র 0x23c: ক্ষেত্র 0x1e09: ক্ষেত্র 0xa793: ক্ষেত্র 0xa794:
		      regmbc('c'); regmbc(0xe7); regmbc(0x107);
		      regmbc(0x109); regmbc(0x10b); regmbc(0x10d);
		      regmbc(0x188); regmbc(0x23c); regmbc(0x1e09);
		      regmbc(0xa793); regmbc(0xa794);
		      ফেরত;
	    ক্ষেত্র 'd': ক্ষেত্র 0x10f: ক্ষেত্র 0x111: ক্ষেত্র 0x257: ক্ষেত্র 0x1d6d:
	    ক্ষেত্র 0x1d81: ক্ষেত্র 0x1d91: ক্ষেত্র 0x1e0b: ক্ষেত্র 0x1e0d:
	    ক্ষেত্র 0x1e0f: ক্ষেত্র 0x1e11: ক্ষেত্র 0x1e13:
		      regmbc('d'); regmbc(0x10f); regmbc(0x111);
		      regmbc(0x257); regmbc(0x1d6d); regmbc(0x1d81);
		      regmbc(0x1d91); regmbc(0x1e0b); regmbc(0x1e0d);
		      regmbc(0x1e0f); regmbc(0x1e11); regmbc(0x1e13);
		      ফেরত;
	    ক্ষেত্র 'e': ক্ষেত্র 0xe8: ক্ষেত্র 0xe9: ক্ষেত্র 0xea: ক্ষেত্র 0xeb:
	    ক্ষেত্র 0x113: ক্ষেত্র 0x115: ক্ষেত্র 0x117: ক্ষেত্র 0x119:
	    ক্ষেত্র 0x11b: ক্ষেত্র 0x205: ক্ষেত্র 0x207: ক্ষেত্র 0x229:
	    ক্ষেত্র 0x247: ক্ষেত্র 0x1d92: ক্ষেত্র 0x1e15: ক্ষেত্র 0x1e17:
	    ক্ষেত্র 0x1e19: ক্ষেত্র 0x1e1b: ক্ষেত্র 0x1eb9: ক্ষেত্র 0x1ebb:
	    ক্ষেত্র 0x1e1d: ক্ষেত্র 0x1ebd: ক্ষেত্র 0x1ebf: ক্ষেত্র 0x1ec1:
	    ক্ষেত্র 0x1ec3: ক্ষেত্র 0x1ec5: ক্ষেত্র 0x1ec7:
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
		      ফেরত;
	    ক্ষেত্র 'f': ক্ষেত্র 0x192: ক্ষেত্র 0x1d6e: ক্ষেত্র 0x1d82:
	    ক্ষেত্র 0x1e1f: ক্ষেত্র 0xa799:
		     regmbc('f'); regmbc(0x192); regmbc(0x1d6e);
		     regmbc(0x1d82); regmbc(0x1e1f); regmbc(0xa799);
		     ফেরত;
	    ক্ষেত্র 'g': ক্ষেত্র 0x11d: ক্ষেত্র 0x11f: ক্ষেত্র 0x121: ক্ষেত্র 0x123:
	    ক্ষেত্র 0x1e5: ক্ষেত্র 0x1e7: ক্ষেত্র 0x260: ক্ষেত্র 0x1f5: ক্ষেত্র 0x1d83:
	    ক্ষেত্র 0x1e21: ক্ষেত্র 0xa7a1:
		      regmbc('g'); regmbc(0x11d); regmbc(0x11f);
		      regmbc(0x121); regmbc(0x123); regmbc(0x1e5);
		      regmbc(0x1e7); regmbc(0x1f5); regmbc(0x260);
		      regmbc(0x1d83); regmbc(0x1e21); regmbc(0xa7a1);
		      ফেরত;
	    ক্ষেত্র 'h': ক্ষেত্র 0x125: ক্ষেত্র 0x127: ক্ষেত্র 0x21f: ক্ষেত্র 0x1e23:
	    ক্ষেত্র 0x1e25: ক্ষেত্র 0x1e27: ক্ষেত্র 0x1e29: ক্ষেত্র 0x1e2b:
	    ক্ষেত্র 0x1e96: ক্ষেত্র 0x2c68: ক্ষেত্র 0xa795:
		      regmbc('h'); regmbc(0x125); regmbc(0x127);
		      regmbc(0x21f); regmbc(0x1e23); regmbc(0x1e25);
		      regmbc(0x1e27); regmbc(0x1e29); regmbc(0x1e2b);
		      regmbc(0x1e96); regmbc(0x2c68); regmbc(0xa795);
		      ফেরত;
	    ক্ষেত্র 'i': ক্ষেত্র 0xec: ক্ষেত্র 0xed: ক্ষেত্র 0xee: ক্ষেত্র 0xef:
	    ক্ষেত্র 0x129: ক্ষেত্র 0x12b: ক্ষেত্র 0x12d: ক্ষেত্র 0x12f:
	    ক্ষেত্র 0x1d0: ক্ষেত্র 0x209: ক্ষেত্র 0x20b: ক্ষেত্র 0x268:
	    ক্ষেত্র 0x1d96: ক্ষেত্র 0x1e2d: ক্ষেত্র 0x1e2f: ক্ষেত্র 0x1ec9:
	    ক্ষেত্র 0x1ecb:
		      regmbc('i'); regmbc(0xec); regmbc(0xed);
		      regmbc(0xee); regmbc(0xef); regmbc(0x129);
		      regmbc(0x12b); regmbc(0x12d); regmbc(0x12f);
		      regmbc(0x1d0); regmbc(0x209); regmbc(0x20b);
		      regmbc(0x268); regmbc(0x1d96); regmbc(0x1e2d);
		      regmbc(0x1e2f); regmbc(0x1ec9); regmbc(0x1ecb);
		      ফেরত;
	    ক্ষেত্র 'j': ক্ষেত্র 0x135: ক্ষেত্র 0x1f0: ক্ষেত্র 0x249:
		      regmbc('j'); regmbc(0x135); regmbc(0x1f0);
		      regmbc(0x249);
		      ফেরত;
	    ক্ষেত্র 'k': ক্ষেত্র 0x137: ক্ষেত্র 0x199: ক্ষেত্র 0x1e9:
	    ক্ষেত্র 0x1d84: ক্ষেত্র 0x1e31: ক্ষেত্র 0x1e33: ক্ষেত্র 0x1e35:
	    ক্ষেত্র 0x2c6a: ক্ষেত্র 0xa741:
		      regmbc('k'); regmbc(0x137); regmbc(0x199);
		      regmbc(0x1e9); regmbc(0x1d84); regmbc(0x1e31);
		      regmbc(0x1e33); regmbc(0x1e35); regmbc(0x2c6a);
		      regmbc(0xa741);
		      ফেরত;
	    ক্ষেত্র 'l': ক্ষেত্র 0x13a: ক্ষেত্র 0x13c: ক্ষেত্র 0x13e:
	    ক্ষেত্র 0x140: ক্ষেত্র 0x142: ক্ষেত্র 0x19a: ক্ষেত্র 0x1e37:
	    ক্ষেত্র 0x1e39: ক্ষেত্র 0x1e3b: ক্ষেত্র 0x1e3d: ক্ষেত্র 0x2c61:
		      regmbc('l'); regmbc(0x13a); regmbc(0x13c);
		      regmbc(0x13e); regmbc(0x140); regmbc(0x142);
		      regmbc(0x19a); regmbc(0x1e37); regmbc(0x1e39);
		      regmbc(0x1e3b); regmbc(0x1e3d); regmbc(0x2c61);
		      ফেরত;
	    ক্ষেত্র 'm': ক্ষেত্র 0x1d6f: ক্ষেত্র 0x1e3f: ক্ষেত্র 0x1e41: ক্ষেত্র 0x1e43:
		      regmbc('m'); regmbc(0x1d6f); regmbc(0x1e3f);
		      regmbc(0x1e41); regmbc(0x1e43);
		      ফেরত;
	    ক্ষেত্র 'n': ক্ষেত্র 0xf1: ক্ষেত্র 0x144: ক্ষেত্র 0x146: ক্ষেত্র 0x148:
	    ক্ষেত্র 0x149: ক্ষেত্র 0x1f9: ক্ষেত্র 0x1d70: ক্ষেত্র 0x1d87:
	    ক্ষেত্র 0x1e45: ক্ষেত্র 0x1e47: ক্ষেত্র 0x1e49: ক্ষেত্র 0x1e4b:
	    ক্ষেত্র 0xa7a5:
		      regmbc('n'); regmbc(0xf1); regmbc(0x144);
		      regmbc(0x146); regmbc(0x148); regmbc(0x149);
		      regmbc(0x1f9); regmbc(0x1d70); regmbc(0x1d87);
		      regmbc(0x1e45); regmbc(0x1e47); regmbc(0x1e49);
		      regmbc(0x1e4b); regmbc(0xa7a5);
		      ফেরত;
	    ক্ষেত্র 'o': ক্ষেত্র 0xf2: ক্ষেত্র 0xf3: ক্ষেত্র 0xf4: ক্ষেত্র 0xf5:
	    ক্ষেত্র 0xf6: ক্ষেত্র 0xf8: ক্ষেত্র 0x14d: ক্ষেত্র 0x14f: ক্ষেত্র 0x151:
	    ক্ষেত্র 0x1a1: ক্ষেত্র 0x1d2: ক্ষেত্র 0x1eb: ক্ষেত্র 0x1ed: ক্ষেত্র 0x1ff:
	    ক্ষেত্র 0x20d: ক্ষেত্র 0x20f: ক্ষেত্র 0x22b: ক্ষেত্র 0x22d: ক্ষেত্র 0x22f:
	    ক্ষেত্র 0x231: ক্ষেত্র 0x275: ক্ষেত্র 0x1e4d: ক্ষেত্র 0x1e4f:
	    ক্ষেত্র 0x1e51: ক্ষেত্র 0x1e53: ক্ষেত্র 0x1ecd: ক্ষেত্র 0x1ecf:
	    ক্ষেত্র 0x1ed1: ক্ষেত্র 0x1ed3: ক্ষেত্র 0x1ed5: ক্ষেত্র 0x1ed7:
	    ক্ষেত্র 0x1ed9: ক্ষেত্র 0x1edb: ক্ষেত্র 0x1edd: ক্ষেত্র 0x1edf:
	    ক্ষেত্র 0x1ee1: ক্ষেত্র 0x1ee3:
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
		      ফেরত;
	    ক্ষেত্র 'p': ক্ষেত্র 0x1a5: ক্ষেত্র 0x1d71: ক্ষেত্র 0x1d88: ক্ষেত্র 0x1d7d:
	    ক্ষেত্র 0x1e55: ক্ষেত্র 0x1e57:
		      regmbc('p'); regmbc(0x1a5); regmbc(0x1d71);
		      regmbc(0x1d7d); regmbc(0x1d88); regmbc(0x1e55);
		      regmbc(0x1e57);
		      ফেরত;
	    ক্ষেত্র 'q': ক্ষেত্র 0x24b: ক্ষেত্র 0x2a0:
		      regmbc('q'); regmbc(0x24b); regmbc(0x2a0);
		      ফেরত;
	    ক্ষেত্র 'r': ক্ষেত্র 0x155: ক্ষেত্র 0x157: ক্ষেত্র 0x159: ক্ষেত্র 0x211:
	    ক্ষেত্র 0x213: ক্ষেত্র 0x24d: ক্ষেত্র 0x27d: ক্ষেত্র 0x1d72: ক্ষেত্র 0x1d73:
	    ক্ষেত্র 0x1d89: ক্ষেত্র 0x1e59: ক্ষেত্র 0x1e5b: ক্ষেত্র 0x1e5d: ক্ষেত্র 0x1e5f:
	    ক্ষেত্র 0xa7a7:
		      regmbc('r'); regmbc(0x155); regmbc(0x157);
		      regmbc(0x159); regmbc(0x211); regmbc(0x213);
		      regmbc(0x24d); regmbc(0x1d72); regmbc(0x1d73);
		      regmbc(0x1d89); regmbc(0x1e59); regmbc(0x27d);
		      regmbc(0x1e5b); regmbc(0x1e5d); regmbc(0x1e5f);
		      regmbc(0xa7a7);
		      ফেরত;
	    ক্ষেত্র 's': ক্ষেত্র 0x15b: ক্ষেত্র 0x15d: ক্ষেত্র 0x15f: ক্ষেত্র 0x161:
	    ক্ষেত্র 0x1e61: ক্ষেত্র 0x219: ক্ষেত্র 0x23f: ক্ষেত্র 0x1d74: ক্ষেত্র 0x1d8a:
	    ক্ষেত্র 0x1e63: ক্ষেত্র 0x1e65: ক্ষেত্র 0x1e67: ক্ষেত্র 0x1e69: ক্ষেত্র 0xa7a9:
		      regmbc('s'); regmbc(0x15b); regmbc(0x15d);
		      regmbc(0x15f); regmbc(0x161); regmbc(0x23f);
		      regmbc(0x219); regmbc(0x1d74); regmbc(0x1d8a);
		      regmbc(0x1e61); regmbc(0x1e63); regmbc(0x1e65);
		      regmbc(0x1e67); regmbc(0x1e69); regmbc(0xa7a9);
		      ফেরত;
	    ক্ষেত্র 't': ক্ষেত্র 0x163: ক্ষেত্র 0x165: ক্ষেত্র 0x167: ক্ষেত্র 0x1ab:
	    ক্ষেত্র 0x1ad: ক্ষেত্র 0x21b: ক্ষেত্র 0x288: ক্ষেত্র 0x1d75: ক্ষেত্র 0x1e6b:
	    ক্ষেত্র 0x1e6d: ক্ষেত্র 0x1e6f: ক্ষেত্র 0x1e71: ক্ষেত্র 0x1e97: ক্ষেত্র 0x2c66:
		      regmbc('t'); regmbc(0x163); regmbc(0x165);
		      regmbc(0x167); regmbc(0x1ab); regmbc(0x21b);
		      regmbc(0x1ad); regmbc(0x288); regmbc(0x1d75);
		      regmbc(0x1e6b); regmbc(0x1e6d); regmbc(0x1e6f);
		      regmbc(0x1e71); regmbc(0x1e97); regmbc(0x2c66);
		      ফেরত;
	    ক্ষেত্র 'u': ক্ষেত্র 0xf9: ক্ষেত্র 0xfa: ক্ষেত্র 0xfb: ক্ষেত্র 0xfc:
	    ক্ষেত্র 0x169: ক্ষেত্র 0x16b: ক্ষেত্র 0x16d: ক্ষেত্র 0x16f:
	    ক্ষেত্র 0x171: ক্ষেত্র 0x173: ক্ষেত্র 0x1b0: ক্ষেত্র 0x1d4:
	    ক্ষেত্র 0x1d6: ক্ষেত্র 0x1d8: ক্ষেত্র 0x1da: ক্ষেত্র 0x1dc:
	    ক্ষেত্র 0x215: ক্ষেত্র 0x217: ক্ষেত্র 0x289: ক্ষেত্র 0x1e73:
	    ক্ষেত্র 0x1d7e: ক্ষেত্র 0x1d99: ক্ষেত্র 0x1e75: ক্ষেত্র 0x1e77:
	    ক্ষেত্র 0x1e79: ক্ষেত্র 0x1e7b: ক্ষেত্র 0x1ee5: ক্ষেত্র 0x1ee7:
	    ক্ষেত্র 0x1ee9: ক্ষেত্র 0x1eeb: ক্ষেত্র 0x1eed: ক্ষেত্র 0x1eef:
	    ক্ষেত্র 0x1ef1:
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
		      ফেরত;
	    ক্ষেত্র 'v': ক্ষেত্র 0x28b: ক্ষেত্র 0x1d8c: ক্ষেত্র 0x1e7d: ক্ষেত্র 0x1e7f:
		      regmbc('v'); regmbc(0x28b); regmbc(0x1d8c);
		      regmbc(0x1e7d); regmbc(0x1e7f);
		      ফেরত;
	    ক্ষেত্র 'w': ক্ষেত্র 0x175: ক্ষেত্র 0x1e81: ক্ষেত্র 0x1e83:
	    ক্ষেত্র 0x1e85: ক্ষেত্র 0x1e87: ক্ষেত্র 0x1e89: ক্ষেত্র 0x1e98:
		      regmbc('w'); regmbc(0x175); regmbc(0x1e81);
		      regmbc(0x1e83); regmbc(0x1e85); regmbc(0x1e87);
		      regmbc(0x1e89); regmbc(0x1e98);
		      ফেরত;
	    ক্ষেত্র 'x': ক্ষেত্র 0x1e8b: ক্ষেত্র 0x1e8d:
		      regmbc('x'); regmbc(0x1e8b); regmbc(0x1e8d);
		      ফেরত;
	    ক্ষেত্র 'y': ক্ষেত্র 0xfd: ক্ষেত্র 0xff: ক্ষেত্র 0x177: ক্ষেত্র 0x1b4:
	    ক্ষেত্র 0x233: ক্ষেত্র 0x24f: ক্ষেত্র 0x1e8f: ক্ষেত্র 0x1e99: ক্ষেত্র 0x1ef3:
	    ক্ষেত্র 0x1ef5: ক্ষেত্র 0x1ef7: ক্ষেত্র 0x1ef9:
		      regmbc('y'); regmbc(0xfd); regmbc(0xff);
		      regmbc(0x177); regmbc(0x1b4); regmbc(0x233);
		      regmbc(0x24f); regmbc(0x1e8f); regmbc(0x1e99);
		      regmbc(0x1ef3); regmbc(0x1ef5); regmbc(0x1ef7);
		      regmbc(0x1ef9);
		      ফেরত;
	    ক্ষেত্র 'z': ক্ষেত্র 0x17a: ক্ষেত্র 0x17c: ক্ষেত্র 0x17e: ক্ষেত্র 0x1b6:
	    ক্ষেত্র 0x1d76: ক্ষেত্র 0x1d8e: ক্ষেত্র 0x1e91: ক্ষেত্র 0x1e93:
	    ক্ষেত্র 0x1e95: ক্ষেত্র 0x2c6c:
		      regmbc('z'); regmbc(0x17a); regmbc(0x17c);
		      regmbc(0x17e); regmbc(0x1b6); regmbc(0x1d76);
		      regmbc(0x1d8e); regmbc(0x1e91); regmbc(0x1e93);
		      regmbc(0x1e95); regmbc(0x2c6c);
		      ফেরত;
	}
    }
    regmbc(c);
}

/*
 * Emit a node.
 * Return pointer to generated code.
 */
    স্থির char_u *
regnode(পূর্ণসংখ্যা op)
{
    char_u  *ret;

    ret = regcode;
    যদি (ret == JUST_CALC_আকার)
	regsize += 3;
    অন্যথা
    {
	*regcode++ = op;
	*regcode++ = NUL;		// Null "next" pointer.
	*regcode++ = NUL;
    }
    ফেরত ret;
}

/*
 * Write a long as four bytes at "p" and return pointer to the next char.
 */
    স্থির char_u *
re_put_long(char_u *p, long_u val)
{
    *p++ = (char_u) ((val >> 24) & 0377);
    *p++ = (char_u) ((val >> 16) & 0377);
    *p++ = (char_u) ((val >> 8) & 0377);
    *p++ = (char_u) (val & 0377);
    ফেরত p;
}

/*
 * regnext - dig the "next" pointer out of a node
 * Returns NULL when calculating size, when there is no next item and when
 * there is an error.
 */
    স্থির char_u *
regnext(char_u *p)
{
    পূর্ণসংখ্যা	    offset;

    যদি (p == JUST_CALC_আকার || reg_toolong)
	ফেরত ফাঁকা;

    offset = পরবর্তী_2(p);
    যদি (offset == 0)
	ফেরত ফাঁকা;

    যদি (OP(p) == BACK)
	ফেরত p - offset;
    অন্যথা
	ফেরত p + offset;
}

/*
 * Set the next-pointer at the end of a node chain.
 */
    স্থির শূন্য
regtail(char_u *p, char_u *val)
{
    char_u	*scan;
    char_u	*অস্থায়ী;
    পূর্ণসংখ্যা		offset;

    যদি (p == JUST_CALC_আকার)
	ফেরত;

    // Find last node.
    scan = p;
    জন্য (;;)
    {
	অস্থায়ী = regnext(scan);
	যদি (অস্থায়ী == ফাঁকা)
	    বিরতি;
	scan = অস্থায়ী;
    }

    যদি (OP(scan) == BACK)
	offset = (পূর্ণসংখ্যা)(scan - val);
    অন্যথা
	offset = (পূর্ণসংখ্যা)(val - scan);
    // When the offset uses more than 16 bits it can no longer fit in the two
    // bytes available.  Use a global flag to avoid having to check return
    // values in too many places.
    যদি (offset > 0xffff)
	reg_toolong = TRUE;
    অন্যথা
    {
	*(scan + 1) = (char_u) (((unsigned)offset >> 8) & 0377);
	*(scan + 2) = (char_u) (offset & 0377);
    }
}

/*
 * Like regtail, on item after a BRANCH; nop if none.
 */
    স্থির শূন্য
regoptail(char_u *p, char_u *val)
{
    // When op is neither BRANCH nor BRACE_COMPLEX0-9, it is "operandless"
    যদি (p == ফাঁকা || p == JUST_CALC_আকার
	    || (OP(p) != BRANCH
		&& (OP(p) < BRACE_COMPLEX || OP(p) > BRACE_COMPLEX + 9)))
	ফেরত;
    regtail(OPERAND(p), val);
}

/*
 * Insert an operator in front of already-emitted operand
 *
 * Means relocating the operand.
 */
    স্থির শূন্য
reginsert(পূর্ণসংখ্যা op, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    যদি (regcode == JUST_CALC_আকার)
    {
	regsize += 3;
	ফেরত;
    }
    src = regcode;
    regcode += 3;
    dst = regcode;
    যখন (src > opnd)
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
    স্থির শূন্য
reginsert_nr(পূর্ণসংখ্যা op, long val, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    যদি (regcode == JUST_CALC_আকার)
    {
	regsize += 7;
	ফেরত;
    }
    src = regcode;
    regcode += 7;
    dst = regcode;
    যখন (src > opnd)
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
    স্থির শূন্য
reginsert_limits(
    পূর্ণসংখ্যা		op,
    long	minval,
    long	maxval,
    char_u	*opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    যদি (regcode == JUST_CALC_আকার)
    {
	regsize += 11;
	ফেরত;
    }
    src = regcode;
    regcode += 11;
    dst = regcode;
    যখন (src > opnd)
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
    স্থির পূর্ণসংখ্যা
seen_endbrace(পূর্ণসংখ্যা refnum)
{
    যদি (!had_endbrace[refnum])
    {
	char_u *p;

	// Trick: check if "@<=" or "@<!" follows, in which case
	// the \1 can appear before the referenced match.
	জন্য (p = regparse; *p != NUL; ++p)
	    যদি (p[0] == '@' && p[1] == '<' && (p[2] == '!' || p[2] == '='))
		বিরতি;
	যদি (*p == NUL)
	{
	    emsg(_(e_illegal_back_reference));
	    rc_did_emsg = TRUE;
	    ফেরত FALSE;
	}
    }
    ফেরত TRUE;
}

/*
 * Parse the lowest level.
 *
 * Optimization:  gobbles an entire sequence of ordinary characters so that
 * it can turn them into a single node, which is smaller to store and
 * faster to run.  Don't do this when one_exactly is set.
 */
    স্থির char_u *
regatom(পূর্ণসংখ্যা *flagp)
{
    char_u	    *ret;
    পূর্ণসংখ্যা		    flags;
    পূর্ণসংখ্যা		    c;
    char_u	    *p;
    পূর্ণসংখ্যা		    extra = 0;
    পূর্ণসংখ্যা		    save_পূর্ব_at_start = পূর্ব_at_start;

    *flagp = WORST;		// Tentatively.

    c = getchr();
    নির্বাচন (c)
    {
      ক্ষেত্র Magic('^'):
	ret = regnode(BOL);
	বিরতি;

      ক্ষেত্র Magic('$'):
	ret = regnode(EOL);
#যদি defined(FEAT_SYN_HL)
	had_eol = TRUE;
#endif
	বিরতি;

      ক্ষেত্র Magic('<'):
	ret = regnode(BOW);
	বিরতি;

      ক্ষেত্র Magic('>'):
	ret = regnode(EOW);
	বিরতি;

      ক্ষেত্র Magic('_'):
	c = no_Magic(getchr());
	যদি (c == '^')		// "\_^" is start-of-line
	{
	    ret = regnode(BOL);
	    বিরতি;
	}
	যদি (c == '$')		// "\_$" is end-of-line
	{
	    ret = regnode(EOL);
#যদি defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#endif
	    বিরতি;
	}

	extra = ADD_NL;
	*flagp |= HASNL;

	// "\_[" is character range plus newline
	যদি (c == '[')
	    যাও collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	// Character classes.
      ক্ষেত্র Magic('.'):
      ক্ষেত্র Magic('i'):
      ক্ষেত্র Magic('I'):
      ক্ষেত্র Magic('k'):
      ক্ষেত্র Magic('K'):
      ক্ষেত্র Magic('f'):
      ক্ষেত্র Magic('F'):
      ক্ষেত্র Magic('p'):
      ক্ষেত্র Magic('P'):
      ক্ষেত্র Magic('s'):
      ক্ষেত্র Magic('S'):
      ক্ষেত্র Magic('d'):
      ক্ষেত্র Magic('D'):
      ক্ষেত্র Magic('x'):
      ক্ষেত্র Magic('X'):
      ক্ষেত্র Magic('o'):
      ক্ষেত্র Magic('O'):
      ক্ষেত্র Magic('w'):
      ক্ষেত্র Magic('W'):
      ক্ষেত্র Magic('h'):
      ক্ষেত্র Magic('H'):
      ক্ষেত্র Magic('a'):
      ক্ষেত্র Magic('A'):
      ক্ষেত্র Magic('l'):
      ক্ষেত্র Magic('L'):
      ক্ষেত্র Magic('u'):
      ক্ষেত্র Magic('U'):
	p = vim_strchr(classchars, no_Magic(c));
	যদি (p == ফাঁকা)
	    EMSG_RET_NULL(_(e_invalid_use_of_underscore));

	// When '.' is followed by a composing char ignore the dot, so that
	// the composing char is matched here.
	যদি (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	{
	    c = getchr();
	    যাও do_multibyte;
	}
	ret = regnode(classcodes[p - classchars] + extra);
	*flagp |= HASWIDTH | SIMPLE;
	বিরতি;

      ক্ষেত্র Magic('n'):
	যদি (reg_string)
	{
	    // In a string "\n" matches a newline character.
	    ret = regnode(EXACTLY);
	    regc(NL);
	    regc(NUL);
	    *flagp |= HASWIDTH | SIMPLE;
	}
	অন্যথা
	{
	    // In buffer text "\n" matches the end of a line.
	    ret = regnode(NEWL);
	    *flagp |= HASWIDTH | HASNL;
	}
	বিরতি;

      ক্ষেত্র Magic('('):
	যদি (one_exactly)
	    EMSG_ONE_RET_NULL;
	ret = reg(REG_PAREN, &flags);
	যদি (ret == ফাঁকা)
	    ফেরত ফাঁকা;
	*flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
	বিরতি;

      ক্ষেত্র NUL:
      ক্ষেত্র Magic('|'):
      ক্ষেত্র Magic('&'):
      ক্ষেত্র Magic(')'):
	যদি (one_exactly)
	    EMSG_ONE_RET_NULL;
	// Supposed to be caught earlier.
	IEMSG_RET_NULL(e_internal_error_in_regexp);
	// NOTREACHED

      ক্ষেত্র Magic('='):
      ক্ষেত্র Magic('?'):
      ক্ষেত্র Magic('+'):
      ক্ষেত্র Magic('@'):
      ক্ষেত্র Magic('{'):
      ক্ষেত্র Magic('*'):
	c = no_Magic(c);
	EMSG3_RET_NULL(_(e_str_chr_follows_nothing),
		(c == '*' ? reg_magic >= MAGIC_ON : reg_magic == MAGIC_ALL), c);
	// NOTREACHED

      ক্ষেত্র Magic('~'):		// previous substitute pattern
	    যদি (reg_পূর্ব_sub != ফাঁকা)
	    {
		char_u	    *lp;

		ret = regnode(EXACTLY);
		lp = reg_পূর্ব_sub;
		যখন (*lp != NUL)
		    regc(*lp++);
		regc(NUL);
		যদি (*reg_পূর্ব_sub != NUL)
		{
		    *flagp |= HASWIDTH;
		    যদি ((lp - reg_পূর্ব_sub) == 1)
			*flagp |= SIMPLE;
		}
	    }
	    অন্যথা
		EMSG_RET_NULL(_(e_no_previous_substitute_regular_expression));
	    বিরতি;

      ক্ষেত্র Magic('1'):
      ক্ষেত্র Magic('2'):
      ক্ষেত্র Magic('3'):
      ক্ষেত্র Magic('4'):
      ক্ষেত্র Magic('5'):
      ক্ষেত্র Magic('6'):
      ক্ষেত্র Magic('7'):
      ক্ষেত্র Magic('8'):
      ক্ষেত্র Magic('9'):
	    {
		পূর্ণসংখ্যা		    refnum;

		refnum = c - Magic('0');
		যদি (!seen_endbrace(refnum))
		    ফেরত ফাঁকা;
		ret = regnode(BACKREF + refnum);
	    }
	    বিরতি;

      ক্ষেত্র Magic('z'):
	{
	    c = no_Magic(getchr());
	    নির্বাচন (c)
	    {
#ifdef FEAT_SYN_HL
		ক্ষেত্র '(': যদি ((reg_do_extmatch & REX_SET) == 0)
			      EMSG_RET_NULL(_(e_z_not_allowed_here));
			  যদি (one_exactly)
			      EMSG_ONE_RET_NULL;
			  ret = reg(REG_ZPAREN, &flags);
			  যদি (ret == ফাঁকা)
			      ফেরত ফাঁকা;
			  *flagp |= flags & (HASWIDTH|SPSTART|HASNL|HASLOOKBH);
			  re_has_z = REX_SET;
			  বিরতি;

		ক্ষেত্র '1':
		ক্ষেত্র '2':
		ক্ষেত্র '3':
		ক্ষেত্র '4':
		ক্ষেত্র '5':
		ক্ষেত্র '6':
		ক্ষেত্র '7':
		ক্ষেত্র '8':
		ক্ষেত্র '9': যদি ((reg_do_extmatch & REX_USE) == 0)
			      EMSG_RET_NULL(_(e_z1_z9_not_allowed_here));
			  ret = regnode(ZREF + c - '0');
			  re_has_z = REX_USE;
			  বিরতি;
#endif

		ক্ষেত্র 's': ret = regnode(MOPEN + 0);
			  যদি (re_mult_পরবর্তী("\\zs") == FAIL)
			      ফেরত ফাঁকা;
			  বিরতি;

		ক্ষেত্র 'e': ret = regnode(MCLOSE + 0);
			  যদি (re_mult_পরবর্তী("\\ze") == FAIL)
			      ফেরত ফাঁকা;
			  বিরতি;

		পূর্বনির্ধারিত:  EMSG_RET_NULL(_(e_invalid_character_after_bsl_z));
	    }
	}
	বিরতি;

      ক্ষেত্র Magic('%'):
	{
	    c = no_Magic(getchr());
	    নির্বাচন (c)
	    {
		// () without a back reference
		ক্ষেত্র '(':
		    যদি (one_exactly)
			EMSG_ONE_RET_NULL;
		    ret = reg(REG_NPAREN, &flags);
		    যদি (ret == ফাঁকা)
			ফেরত ফাঁকা;
		    *flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
		    বিরতি;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		ক্ষেত্র '^':
		    ret = regnode(RE_BOF);
		    বিরতি;

		ক্ষেত্র '$':
		    ret = regnode(RE_EOF);
		    বিরতি;

		ক্ষেত্র '#':
		    যদি (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_start_of_pattern),
								  regparse[1]);
			ফেরত FAIL;
		    }
		    ret = regnode(CURSOR);
		    বিরতি;

		ক্ষেত্র 'V':
		    ret = regnode(RE_VISUAL);
		    বিরতি;

		ক্ষেত্র 'C':
		    ret = regnode(RE_COMPOSING);
		    বিরতি;

		// \%[abc]: Emit as a list of branches, all ending at the last
		// branch which matches nothing.
		ক্ষেত্র '[':
			  যদি (one_exactly)	// doesn't nest
			      EMSG_ONE_RET_NULL;
			  {
			      char_u	*lastbranch;
			      char_u	*lastnode = ফাঁকা;
			      char_u	*br;

			      ret = ফাঁকা;
			      যখন ((c = getchr()) != ']')
			      {
				  যদি (c == NUL)
				      EMSG2_RET_NULL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
				  br = regnode(BRANCH);
				  যদি (ret == ফাঁকা)
				      ret = br;
				  অন্যথা
				  {
				      regtail(lastnode, br);
				      যদি (reg_toolong)
					  ফেরত ফাঁকা;
				  }

				  ungetchr();
				  one_exactly = TRUE;
				  lastnode = regatom(flagp);
				  one_exactly = FALSE;
				  যদি (lastnode == ফাঁকা)
				      ফেরত ফাঁকা;
			      }
			      যদি (ret == ফাঁকা)
				  EMSG2_RET_NULL(_(e_empty_str_brackets),
						      reg_magic == MAGIC_ALL);
			      lastbranch = regnode(BRANCH);
			      br = regnode(NOTHING);
			      যদি (ret != JUST_CALC_আকার)
			      {
				  regtail(lastnode, br);
				  regtail(lastbranch, br);
				  // connect all branches to the NOTHING
				  // branch at the end
				  জন্য (br = ret; br != lastnode; )
				  {
				      যদি (OP(br) == BRANCH)
				      {
					  regtail(br, lastbranch);
					  যদি (reg_toolong)
					      ফেরত ফাঁকা;
					  br = OPERAND(br);
				      }
				      অন্যথা
					  br = regnext(br);
				  }
			      }
			      *flagp &= ~(HASWIDTH | SIMPLE);
			      বিরতি;
			  }

		ক্ষেত্র 'd':   // %d123 decimal
		ক্ষেত্র 'o':   // %o123 octal
		ক্ষেত্র 'x':   // %xab hex 2
		ক্ষেত্র 'u':   // %uabcd hex 4
		ক্ষেত্র 'U':   // %U1234abcd hex 8
			  {
			      vimlong_T i;

			      নির্বাচন (c)
			      {
				  ক্ষেত্র 'd': i = getdecchrs(); বিরতি;
				  ক্ষেত্র 'o': i = getoctchrs(); বিরতি;
				  ক্ষেত্র 'x': i = gethexchrs(2); বিরতি;
				  ক্ষেত্র 'u': i = gethexchrs(4); বিরতি;
				  ক্ষেত্র 'U': i = gethexchrs(8); বিরতি;
				  পূর্বনির্ধারিত:  i = -1; বিরতি;
			      }

			      যদি (i < 0 || i > INT_MAX)
				  EMSG2_RET_NULL(
					    _(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			      যদি (use_multibytecode(i))
				  ret = regnode(MULTIBYTECODE);
			      অন্যথা
				  ret = regnode(EXACTLY);
			      যদি (i == 0)
				  regc(0x0a);
			      অন্যথা
				  regmbc((পূর্ণসংখ্যা)i);
			      regc(NUL);
			      *flagp |= HASWIDTH;
			      বিরতি;
			  }

		পূর্বনির্ধারিত:
			  যদি (VIM_ISDIGIT(c) || c == '<' || c == '>'
						|| c == '\'' || c == '.')
			  {
			      long_u	n = 0;
			      পূর্ণসংখ্যা	cmp;
			      পূর্ণসংখ্যা	cur = FALSE;
			      পূর্ণসংখ্যা	got_digit = FALSE;

			      cmp = c;
			      যদি (cmp == '<' || cmp == '>')
				  c = getchr();
			      যদি (no_Magic(c) == '.')
			      {
				  cur = TRUE;
				  c = getchr();
			      }
			      যখন (VIM_ISDIGIT(c))
			      {
				  got_digit = TRUE;
				  n = n * 10 + (c - '0');
				  c = getchr();
			      }
			      যদি (no_Magic(c) == '\'' && n == 0)
			      {
				  // "\%'m", "\%<'m" and "\%>'m": Mark
				  c = getchr();
				  ret = regnode(RE_MARK);
				  যদি (ret == JUST_CALC_আকার)
				      regsize += 2;
				  অন্যথা
				  {
				      *regcode++ = c;
				      *regcode++ = cmp;
				  }
				  বিরতি;
			      }
			      অন্যথা যদি ((c == 'l' || c == 'c' || c == 'v')
					  && (cur || got_digit))
			      {
				  যদি (cur && n)
				  {
				    semsg(_(e_regexp_সংখ্যা_after_dot_pos_অনুসন্ধান_chr),
								  no_Magic(c));
				    rc_did_emsg = TRUE;
				    ফেরত ফাঁকা;
				  }
				  যদি (c == 'l')
				  {
				      যদি (cur)
					  n = curwin->w_cursor.lnum;
				      ret = regnode(RE_LNUM);
				      যদি (save_পূর্ব_at_start)
					  at_start = TRUE;
				  }
				  অন্যথা যদি (c == 'c')
				  {
				      যদি (cur)
				      {
					  n = curwin->w_cursor.col;
					  n++;
				      }
				      ret = regnode(RE_COL);
				  }
				  অন্যথা
				  {
				      যদি (cur)
				      {
					  colnr_T vcol = 0;

					  getvvcol(curwin, &curwin->w_cursor,
							 ফাঁকা, ফাঁকা, &vcol, 0);
					  ++vcol;
					  n = vcol;
				      }
				      ret = regnode(RE_VCOL);
				  }
				  যদি (ret == JUST_CALC_আকার)
				      regsize += 5;
				  অন্যথা
				  {
				      // put the number and the optional
				      // comparator after the opcode
				      regcode = re_put_long(regcode, n);
				      *regcode++ = cmp;
				  }
				  বিরতি;
			      }
			  }

			  EMSG2_RET_NULL(_(e_invalid_character_after_str),
						      reg_magic == MAGIC_ALL);
	    }
	}
	বিরতি;

      ক্ষেত্র Magic('['):
collection:
	{
	    char_u	*lp;

	    // If there is no matching ']', we assume the '[' is a normal
	    // character.  This makes 'incsearch' and ":help [" work.
	    lp = skip_anyof(regparse);
	    যদি (*lp == ']')	// there is a matching ']'
	    {
		পূর্ণসংখ্যা	startc = -1;	// > 0 when next '-' is a range
		পূর্ণসংখ্যা	endc;

		যদি (extra == 0)
		{
		    পূর্ণসংখ্যা	cl = bt_recognize_char_class(regparse, lp);

		    যদি (cl != 0)
		    {
			ret = regnode(cl);
			regparse = lp;
			skipchr();
			*flagp |= HASWIDTH | SIMPLE;
			বিরতি;
		    }
		}

		// In a character class, different parsing rules apply.
		// Not even \ is special anymore, nothing is.
		যদি (*regparse == '^')	    // Complement of range.
		{
		    ret = regnode(ANYBUT + extra);
		    regparse++;
		}
		অন্যথা
		    ret = regnode(ANYOF + extra);

		// At the start ']' and '-' mean the literal character.
		যদি (*regparse == ']' || *regparse == '-')
		{
		    startc = *regparse;
		    regc(*regparse++);
		}

		যখন (*regparse != NUL && *regparse != ']')
		{
		    যদি (*regparse == '-')
		    {
			++regparse;
			// The '-' is not used for a range at the end and
			// after or before a '\n'.
			যদি (*regparse == ']' || *regparse == NUL
				|| startc == -1
				|| (regparse[0] == '\\' && regparse[1] == 'n'))
			{
			    regc('-');
			    startc = '-';	// [--x] is a range
			}
			অন্যথা
			{
			    // Also accept "a-[.z.]"
			    endc = 0;
			    যদি (*regparse == '[')
				endc = get_coll_element(&regparse);
			    যদি (endc == 0)
			    {
				যদি (has_mbyte)
				    endc = mb_ptr2char_adv(&regparse);
				অন্যথা
				    endc = *regparse++;
			    }

			    // Handle \o40, \x20 and \u20AC style sequences
			    যদি (endc == '\\' && !reg_cpo_lit && !reg_cpo_bsl)
				endc = coll_get_char();

			    যদি (startc > endc)
				EMSG_RET_NULL(_(e_reverse_range_in_character_class));
			    যদি (has_mbyte && ((*mb_char2len)(startc) > 1
						 || (*mb_char2len)(endc) > 1))
			    {
				// Limit to a range of 256 chars.
				যদি (endc > startc + 256)
				    EMSG_RET_NULL(_(e_range_too_large_in_character_class));
				যখন (++startc <= endc)
				    regmbc(startc);
			    }
			    অন্যথা
			    {
				যখন (++startc <= endc)
				    regc(startc);
			    }
			    startc = -1;
			}
		    }
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    অন্যথা যদি (*regparse == '\\'
			    && !reg_cpo_bsl
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != ফাঁকা
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR,
						       regparse[1]) != ফাঁকা)))
		    {
			regparse++;
			যদি (*regparse == 'n')
			{
			    // '\n' in range: also match NL
			    যদি (ret != JUST_CALC_আকার)
			    {
				// Using \n inside [^] does not change what
				// matches. "[^\n]" is the same as ".".
				যদি (*ret == ANYOF)
				{
				    *ret = ANYOF + ADD_NL;
				    *flagp |= HASNL;
				}
				// else: must have had a \n already
			    }
			    regparse++;
			    startc = -1;
			}
			অন্যথা যদি (*regparse == 'd'
				|| *regparse == 'o'
				|| *regparse == 'x'
				|| *regparse == 'u'
				|| *regparse == 'U')
			{
			    startc = coll_get_char();
			    // max UTF-8 Codepoint is U+10FFFF,
			    // but allow values until INT_MAX
			    যদি (startc == INT_MAX)
				EMSG_RET_NULL(_(e_unicode_val_too_large));
			    যদি (startc == 0)
				regc(0x0a);
			    অন্যথা
				regmbc(startc);
			}
			অন্যথা
			{
			    startc = backslash_trans(*regparse++);
			    regc(startc);
			}
		    }
		    অন্যথা যদি (*regparse == '[')
		    {
			পূর্ণসংখ্যা c_class;
			পূর্ণসংখ্যা cu;

			c_class = get_char_class(&regparse);
			startc = -1;
			// Characters assumed to be 8 bits!
			নির্বাচন (c_class)
			{
			    ক্ষেত্র CLASS_NONE:
				c_class = get_equi_class(&regparse);
				যদি (c_class != 0)
				{
				    // produce equivalence class
				    reg_equi_class(c_class);
				}
				অন্যথা যদি ((c_class =
					    get_coll_element(&regparse)) != 0)
				{
				    // produce a collating element
				    regmbc(c_class);
				}
				অন্যথা
				{
				    // literal '[', allow [[-x] as a range
				    startc = *regparse++;
				    regc(startc);
				}
				বিরতি;
			    ক্ষেত্র CLASS_ALNUM:
				জন্য (cu = 1; cu < 128; cu++)
				    যদি (isalnum(cu))
					regmbc(cu);
				বিরতি;
			    ক্ষেত্র CLASS_ALPHA:
				জন্য (cu = 1; cu < 128; cu++)
				    যদি (isalpha(cu))
					regmbc(cu);
				বিরতি;
			    ক্ষেত্র CLASS_BLANK:
				regc(' ');
				regc('\t');
				বিরতি;
			    ক্ষেত্র CLASS_CNTRL:
				জন্য (cu = 1; cu <= 127; cu++)
				    যদি (iscntrl(cu))
					regmbc(cu);
				বিরতি;
			    ক্ষেত্র CLASS_DIGIT:
				জন্য (cu = 1; cu <= 127; cu++)
				    যদি (VIM_ISDIGIT(cu))
					regmbc(cu);
				বিরতি;
			    ক্ষেত্র CLASS_GRAPH:
				জন্য (cu = 1; cu <= 127; cu++)
				    যদি (isgraph(cu))
					regmbc(cu);
				বিরতি;
			    ক্ষেত্র CLASS_LOWER:
				জন্য (cu = 1; cu <= 255; cu++)
				    যদি (MB_ISLOWER(cu) && cu != 170
								 && cu != 186)
					regmbc(cu);
				বিরতি;
			    ক্ষেত্র CLASS_PRINT:
				জন্য (cu = 1; cu <= 255; cu++)
				    যদি (vim_isprintc(cu))
					regmbc(cu);
				বিরতি;
			    ক্ষেত্র CLASS_PUNCT:
				জন্য (cu = 1; cu < 128; cu++)
				    যদি (ispunct(cu))
					regmbc(cu);
				বিরতি;
			    ক্ষেত্র CLASS_SPACE:
				জন্য (cu = 9; cu <= 13; cu++)
				    regc(cu);
				regc(' ');
				বিরতি;
			    ক্ষেত্র CLASS_UPPER:
				জন্য (cu = 1; cu <= 255; cu++)
				    যদি (MB_ISUPPER(cu))
					regmbc(cu);
				বিরতি;
			    ক্ষেত্র CLASS_XDIGIT:
				জন্য (cu = 1; cu <= 255; cu++)
				    যদি (vim_isxdigit(cu))
					regmbc(cu);
				বিরতি;
			    ক্ষেত্র CLASS_TAB:
				regc('\t');
				বিরতি;
			    ক্ষেত্র CLASS_RETURN:
				regc('\r');
				বিরতি;
			    ক্ষেত্র CLASS_BACKSPACE:
				regc('\b');
				বিরতি;
			    ক্ষেত্র CLASS_ESCAPE:
				regc('\033');
				বিরতি;
			    ক্ষেত্র CLASS_IDENT:
				জন্য (cu = 1; cu <= 255; cu++)
				    যদি (vim_isIDc(cu))
					regmbc(cu);
				বিরতি;
			    ক্ষেত্র CLASS_KEYWORD:
				জন্য (cu = 1; cu <= 255; cu++)
				    যদি (reg_iswordc(cu))
					regmbc(cu);
				বিরতি;
			    ক্ষেত্র CLASS_FNAME:
				জন্য (cu = 1; cu <= 255; cu++)
				    যদি (vim_isfilec(cu))
					regmbc(cu);
				বিরতি;
			}
		    }
		    অন্যথা
		    {
			যদি (has_mbyte)
			{
			    পূর্ণসংখ্যা	len;

			    // produce a multibyte character, including any
			    // following composing characters
			    startc = mb_ptr2char(regparse);
			    len = (*mb_ptr2len)(regparse);
			    যদি (enc_utf8 && utf_char2len(startc) != len)
				startc = -1;	// composing chars
			    যখন (--len >= 0)
				regc(*regparse++);
			}
			অন্যথা
			{
			    startc = *regparse++;
			    regc(startc);
			}
		    }
		}
		regc(NUL);
		prevchr_len = 1;	// last char was the ']'
		যদি (*regparse != ']')
		    EMSG_RET_NULL(_(e_too_many_brackets));  // Cannot happen?
		skipchr();	    // let's be friends with the lexer again
		*flagp |= HASWIDTH | SIMPLE;
		বিরতি;
	    }
	    অন্যথা যদি (reg_strict)
		EMSG2_RET_NULL(_(e_missing_rsb_after_str_lsb),
							reg_magic > MAGIC_OFF);
	}
	// FALLTHROUGH

      পূর্বনির্ধারিত:
	{
	    পূর্ণসংখ্যা		len;

	    // A multi-byte character is handled as a separate atom if it's
	    // before a multi and when it's a composing char.
	    যদি (use_multibytecode(c))
	    {
do_multibyte:
		ret = regnode(MULTIBYTECODE);
		regmbc(c);
		*flagp |= HASWIDTH | SIMPLE;
		বিরতি;
	    }

	    ret = regnode(EXACTLY);

	    // Append characters as long as:
	    // - there is no following multi, we then need the character in
	    //   front of it as a single character operand
	    // - not running into a Magic character
	    // - "one_exactly" is not set
	    // But always emit at least one character.  Might be a Multi,
	    // e.g., a "[" without matching "]".
	    জন্য (len = 0; c != NUL && (len == 0
			|| (re_multi_type(peekchr()) == NOT_MULTI
			    && !one_exactly
			    && !is_Magic(c))); ++len)
	    {
		c = no_Magic(c);
		যদি (has_mbyte)
		{
		    regmbc(c);
		    যদি (enc_utf8)
		    {
			পূর্ণসংখ্যা	l;

			// Need to get composing character too.
			জন্য (;;)
			{
			    l = utf_ptr2len(regparse);
			    যদি (!UTF_COMPOSINGLIKE(regparse, regparse + l))
				বিরতি;
			    regmbc(utf_ptr2char(regparse));
			    skipchr();
			}
		    }
		}
		অন্যথা
		    regc(c);
		c = getchr();
	    }
	    ungetchr();

	    regc(NUL);
	    *flagp |= HASWIDTH;
	    যদি (len == 1)
		*flagp |= SIMPLE;
	}
	বিরতি;
    }

    ফেরত ret;
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
    স্থির char_u *
regpiece(পূর্ণসংখ্যা *flagp)
{
    char_u	    *ret;
    পূর্ণসংখ্যা		    op;
    char_u	    *পরবর্তী;
    পূর্ণসংখ্যা		    flags;
    long	    minval;
    long	    maxval;

    ret = regatom(&flags);
    যদি (ret == ফাঁকা)
	ফেরত ফাঁকা;

    op = peekchr();
    যদি (re_multi_type(op) == NOT_MULTI)
    {
	*flagp = flags;
	ফেরত ret;
    }
    // default flags
    *flagp = (WORST | SPSTART | (flags & (HASNL | HASLOOKBH)));

    skipchr();
    নির্বাচন (op)
    {
	ক্ষেত্র Magic('*'):
	    যদি (flags & SIMPLE)
		reginsert(STAR, ret);
	    অন্যথা
	    {
		// Emit x* as (x&|), where & means "self".
		reginsert(BRANCH, ret); // Either x
		regoptail(ret, regnode(BACK));	// and loop
		regoptail(ret, ret);	// back
		regtail(ret, regnode(BRANCH));	// or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    বিরতি;

	ক্ষেত্র Magic('+'):
	    যদি (flags & SIMPLE)
		reginsert(PLUS, ret);
	    অন্যথা
	    {
		// Emit x+ as x(&|), where & means "self".
		পরবর্তী = regnode(BRANCH); // Either
		regtail(ret, পরবর্তী);
		regtail(regnode(BACK), ret);	// loop back
		regtail(পরবর্তী, regnode(BRANCH)); // or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    *flagp = (WORST | HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    বিরতি;

	ক্ষেত্র Magic('@'):
	    {
		পূর্ণসংখ্যা	lop = END;
		long	nr;

		nr = (long)getdecchrs();
		নির্বাচন (no_Magic(getchr()))
		{
		    ক্ষেত্র '=': lop = MATCH; বিরতি;		  // \@=
		    ক্ষেত্র '!': lop = NOMATCH; বিরতি;		  // \@!
		    ক্ষেত্র '>': lop = SUBPAT; বিরতি;		  // \@>
		    ক্ষেত্র '<': নির্বাচন (no_Magic(getchr()))
			      {
				  ক্ষেত্র '=': lop = BEHIND; বিরতি;   // \@<=
				  ক্ষেত্র '!': lop = NOBEHIND; বিরতি; // \@<!
			      }
		}
		যদি (lop == END)
		    EMSG2_RET_NULL(_(e_invalid_character_after_str_at),
						      reg_magic == MAGIC_ALL);
		// Look behind must match with behind_pos.
		যদি (lop == BEHIND || lop == NOBEHIND)
		{
		    regtail(ret, regnode(BHPOS));
		    *flagp |= HASLOOKBH;
		}
		regtail(ret, regnode(END)); // operand ends
		যদি (lop == BEHIND || lop == NOBEHIND)
		{
		    যদি (nr < 0)
			nr = 0; // no limit is same as zero limit
		    reginsert_nr(lop, nr, ret);
		}
		অন্যথা
		    reginsert(lop, ret);
		বিরতি;
	    }

	ক্ষেত্র Magic('?'):
	ক্ষেত্র Magic('='):
	    // Emit x= as (x|)
	    reginsert(BRANCH, ret);		// Either x
	    regtail(ret, regnode(BRANCH));	// or
	    পরবর্তী = regnode(NOTHING);		// null.
	    regtail(ret, পরবর্তী);
	    regoptail(ret, পরবর্তী);
	    বিরতি;

	ক্ষেত্র Magic('{'):
	    যদি (!read_limits(&minval, &maxval))
		ফেরত ফাঁকা;
	    যদি (flags & SIMPLE)
	    {
		reginsert(BRACE_SIMPLE, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
	    }
	    অন্যথা
	    {
		যদি (অঙ্ক_complex_braces >= 10)
		    EMSG2_RET_NULL(_(e_too_many_complex_str_curly),
						      reg_magic == MAGIC_ALL);
		reginsert(BRACE_COMPLEX + অঙ্ক_complex_braces, ret);
		regoptail(ret, regnode(BACK));
		regoptail(ret, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
		++অঙ্ক_complex_braces;
	    }
	    যদি (minval > 0 && maxval > 0)
		*flagp = (HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    বিরতি;
    }
    যদি (re_multi_type(peekchr()) != NOT_MULTI)
    {
	// Can't have a multi follow a multi.
	যদি (peekchr() == Magic('*'))
	    EMSG2_RET_NULL(_(e_nested_str), reg_magic >= MAGIC_ON);
	EMSG3_RET_NULL(_(e_nested_str_chr), reg_magic == MAGIC_ALL,
							  no_Magic(peekchr()));
    }

    ফেরত ret;
}

/*
 * Parse one alternative of an | or & operator.
 * Implements the concatenation operator.
 */
    স্থির char_u *
regconcat(পূর্ণসংখ্যা *flagp)
{
    char_u	*first = ফাঁকা;
    char_u	*chain = ফাঁকা;
    char_u	*latest;
    পূর্ণসংখ্যা		flags;
    পূর্ণসংখ্যা		cont = TRUE;

    *flagp = WORST;		// Tentatively.

    যখন (cont)
    {
	নির্বাচন (peekchr())
	{
	    ক্ষেত্র NUL:
	    ক্ষেত্র Magic('|'):
	    ক্ষেত্র Magic('&'):
	    ক্ষেত্র Magic(')'):
			    cont = FALSE;
			    বিরতি;
	    ক্ষেত্র Magic('Z'):
			    regflags |= RF_ICOMBINE;
			    skipchr_keepstart();
			    বিরতি;
	    ক্ষেত্র Magic('c'):
			    regflags |= RF_ICASE;
			    skipchr_keepstart();
			    বিরতি;
	    ক্ষেত্র Magic('C'):
			    regflags |= RF_NOICASE;
			    skipchr_keepstart();
			    বিরতি;
	    ক্ষেত্র Magic('v'):
			    reg_magic = MAGIC_ALL;
			    skipchr_keepstart();
			    curchr = -1;
			    বিরতি;
	    ক্ষেত্র Magic('m'):
			    reg_magic = MAGIC_ON;
			    skipchr_keepstart();
			    curchr = -1;
			    বিরতি;
	    ক্ষেত্র Magic('M'):
			    reg_magic = MAGIC_OFF;
			    skipchr_keepstart();
			    curchr = -1;
			    বিরতি;
	    ক্ষেত্র Magic('V'):
			    reg_magic = MAGIC_NONE;
			    skipchr_keepstart();
			    curchr = -1;
			    বিরতি;
	    পূর্বনির্ধারিত:
			    latest = regpiece(&flags);
			    যদি (latest == ফাঁকা || reg_toolong)
				ফেরত ফাঁকা;
			    *flagp |= flags & (HASWIDTH | HASNL | HASLOOKBH);
			    যদি (chain == ফাঁকা)	// First piece.
				*flagp |= flags & SPSTART;
			    অন্যথা
				regtail(chain, latest);
			    chain = latest;
			    যদি (first == ফাঁকা)
				first = latest;
			    বিরতি;
	}
    }
    যদি (first == ফাঁকা)		// Loop ran zero times.
	first = regnode(NOTHING);
    ফেরত first;
}

/*
 * Parse one alternative of an | operator.
 * Implements the & operator.
 */
    স্থির char_u *
regbranch(পূর্ণসংখ্যা *flagp)
{
    char_u	*ret;
    char_u	*chain = ফাঁকা;
    char_u	*latest;
    পূর্ণসংখ্যা		flags;

    *flagp = WORST | HASNL;		// Tentatively.

    ret = regnode(BRANCH);
    জন্য (;;)
    {
	latest = regconcat(&flags);
	যদি (latest == ফাঁকা)
	    ফেরত ফাঁকা;
	// If one of the branches has width, the whole thing has.  If one of
	// the branches anchors at start-of-line, the whole thing does.
	// If one of the branches uses look-behind, the whole thing does.
	*flagp |= flags & (HASWIDTH | SPSTART | HASLOOKBH);
	// If one of the branches doesn't match a line-break, the whole thing
	// doesn't.
	*flagp &= ~HASNL | (flags & HASNL);
	যদি (chain != ফাঁকা)
	    regtail(chain, latest);
	যদি (peekchr() != Magic('&'))
	    বিরতি;
	skipchr();
	regtail(latest, regnode(END)); // operand ends
	যদি (reg_toolong)
	    বিরতি;
	reginsert(MATCH, latest);
	chain = latest;
    }

    ফেরত ret;
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
    স্থির char_u *
reg(
    পূর্ণসংখ্যা		paren,	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
    পূর্ণসংখ্যা		*flagp)
{
    char_u	*ret;
    char_u	*br;
    char_u	*ender;
    পূর্ণসংখ্যা		parno = 0;
    পূর্ণসংখ্যা		flags;

    *flagp = HASWIDTH;		// Tentatively.

#ifdef FEAT_SYN_HL
    যদি (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	যদি (regnzpar >= NSUBEXP)
	    EMSG_RET_NULL(_(e_too_many_z));
	parno = regnzpar;
	regnzpar++;
	ret = regnode(ZOPEN + parno);
    }
    অন্যথা
#endif
	যদি (paren == REG_PAREN)
    {
	// Make a MOPEN node.
	যদি (regnpar >= NSUBEXP)
	    EMSG2_RET_NULL(_(e_too_many_str_open), reg_magic == MAGIC_ALL);
	parno = regnpar;
	++regnpar;
	ret = regnode(MOPEN + parno);
    }
    অন্যথা যদি (paren == REG_NPAREN)
    {
	// Make a NOPEN node.
	ret = regnode(NOPEN);
    }
    অন্যথা
	ret = ফাঁকা;

    যদি (bt_reg_parse_depth >= REG_MAX_PAREN_DEPTH)
	EMSG_RET_NULL(_(e_command_too_complex));
    ++bt_reg_parse_depth;

    // Pick up the branches, linking them together.
    br = regbranch(&flags);
    যদি (br == ফাঁকা)
    {
	ret = ফাঁকা;
	যাও theend;
    }
    যদি (ret != ফাঁকা)
	regtail(ret, br);	// [MZ]OPEN -> first.
    অন্যথা
	ret = br;
    // If one of the branches can be zero-width, the whole thing can.
    // If one of the branches has * at start or matches a line-break, the
    // whole thing can.
    যদি (!(flags & HASWIDTH))
	*flagp &= ~HASWIDTH;
    *flagp |= flags & (SPSTART | HASNL | HASLOOKBH);
    যখন (peekchr() == Magic('|'))
    {
	skipchr();
	br = regbranch(&flags);
	যদি (br == ফাঁকা || reg_toolong)
	{
	    ret = ফাঁকা;
	    যাও theend;
	}
	regtail(ret, br);	// BRANCH -> BRANCH.
	যদি (!(flags & HASWIDTH))
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
    জন্য (br = ret; br != ফাঁকা; br = regnext(br))
	regoptail(br, ender);

    // Check for proper termination.
    যদি (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
#ifdef FEAT_SYN_HL
	যদি (paren == REG_ZPAREN)
	{
	    emsg(_(e_unmatched_z));
	    rc_did_emsg = TRUE;
	    ret = ফাঁকা;
	    যাও theend;
	}
	অন্যথা
#endif
	যদি (paren == REG_NPAREN)
	{
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = ফাঁকা;
	    যাও theend;
	}
	অন্যথা
	{
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = ফাঁকা;
	    যাও theend;
	}
    }
    অন্যথা যদি (paren == REG_NOPAREN && peekchr() != NUL)
    {
	যদি (curchr == Magic(')'))
	{
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = ফাঁকা;
	    যাও theend;
	}
	অন্যথা
	{
	    emsg(_(e_trailing_characters));	// "Can't happen".
	    rc_did_emsg = TRUE;
	    ret = ফাঁকা;
	    যাও theend;
	}
    }
    // Here we set the flag allowing back references to this set of
    // parentheses.
    যদি (paren == REG_PAREN)
	had_endbrace[parno] = TRUE;	// have seen the close paren

theend:
    --bt_reg_parse_depth;
    ফেরত ret;
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
    স্থির regprog_T *
bt_regcomp(char_u *রাশিমালা, পূর্ণসংখ্যা re_flags)
{
    bt_regprog_T    *r;
    char_u	*scan;
    char_u	*longest;
    পূর্ণসংখ্যা		len;
    পূর্ণসংখ্যা		flags;

    যদি (রাশিমালা == ফাঁকা)
	IEMSG_RET_NULL(e_null_argument);

    init_class_tab();

    // First pass: determine size, legality.
    regcomp_start(রাশিমালা, re_flags);
    regcode = JUST_CALC_আকার;
    regc(REGMAGIC);
    যদি (reg(REG_NOPAREN, &flags) == ফাঁকা)
	ফেরত ফাঁকা;

    // Allocate space.
    r = alloc(offsetof(bt_regprog_T, program) + regsize);
    যদি (r == ফাঁকা)
	ফেরত ফাঁকা;
    r->re_in_use = FALSE;
#ifdef DEBUG
    r->regsz = regsize;
#endif

    // Second pass: emit code.
    regcomp_start(রাশিমালা, re_flags);
    regcode = r->program;
    regc(REGMAGIC);
    যদি (reg(REG_NOPAREN, &flags) == ফাঁকা || reg_toolong)
    {
	vim_free(r);
	যদি (reg_toolong)
	    EMSG_RET_NULL(_(e_pattern_too_long));
	ফেরত ফাঁকা;
    }

    // Dig out information for optimizations.
    r->regstart = NUL;		// Worst-case defaults.
    r->reganch = 0;
    r->regmust = ফাঁকা;
    r->regmlen = 0;
    r->regflags = regflags;
    যদি (flags & HASNL)
	r->regflags |= RF_HASNL;
    যদি (flags & HASLOOKBH)
	r->regflags |= RF_LOOKBH;
#ifdef FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    r->reghasz = re_has_z;
#endif
    scan = r->program + 1;	// First BRANCH.
    যদি (OP(regnext(scan)) == END)   // Only one top-level choice.
    {
	scan = OPERAND(scan);

	// Starting-point info.
	যদি (OP(scan) == BOL || OP(scan) == RE_BOF)
	{
	    r->reganch++;
	    scan = regnext(scan);
	}

	যদি (OP(scan) == EXACTLY)
	{
	    যদি (has_mbyte)
		r->regstart = (*mb_ptr2char)(OPERAND(scan));
	    অন্যথা
		r->regstart = *OPERAND(scan);
	}
	অন্যথা যদি ((OP(scan) == BOW
		    || OP(scan) == EOW
		    || OP(scan) == NOTHING
		    || OP(scan) == MOPEN + 0 || OP(scan) == NOPEN
		    || OP(scan) == MCLOSE + 0 || OP(scan) == NCLOSE)
		 && OP(regnext(scan)) == EXACTLY)
	{
	    যদি (has_mbyte)
		r->regstart = (*mb_ptr2char)(OPERAND(regnext(scan)));
	    অন্যথা
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
	যদি ((flags & SPSTART || OP(scan) == BOW || OP(scan) == EOW)
							  && !(flags & HASNL))
	{
	    size_t  scanlen;

	    longest = ফাঁকা;
	    len = 0;
	    জন্য (; scan != ফাঁকা; scan = regnext(scan))
	    {
		যদি (OP(scan) == EXACTLY)
		{
		    scanlen = STRLEN(OPERAND(scan));
		    যদি (scanlen >= (size_t)len)
		    {
			longest = OPERAND(scan);
			len = (পূর্ণসংখ্যা)scanlen;
		    }
		}
	    }
	    r->regmust = longest;
	    r->regmlen = len;
	}
    }
#ifdef BT_REGEXP_DUMP
    regdump(রাশিমালা, r);
#endif
    r->engine = &bt_regengine;
    ফেরত (regprog_T *)r;
}

#যদি defined(FEAT_SYN_HL)
/*
 * Check if during the previous call to vim_regcomp the EOL item "$" has been
 * found.  This is messy, but it works fine.
 */
    পূর্ণসংখ্যা
vim_regcomp_had_eol(শূন্য)
{
    ফেরত had_eol;
}
#endif

/*
 * Get a number after a backslash that is inside [].
 * When nothing is recognized return a backslash.
 */
    স্থির পূর্ণসংখ্যা
coll_get_char(শূন্য)
{
    vimlong_T	nr = -1;

    নির্বাচন (*regparse++)
    {
	ক্ষেত্র 'd': nr = getdecchrs(); বিরতি;
	ক্ষেত্র 'o': nr = getoctchrs(); বিরতি;
	ক্ষেত্র 'x': nr = gethexchrs(2); বিরতি;
	ক্ষেত্র 'u': nr = gethexchrs(4); বিরতি;
	ক্ষেত্র 'U': nr = gethexchrs(8); বিরতি;
    }
    যদি (nr < 0)
    {
	// If getting the number fails be backwards compatible: the character
	// is a backslash.
	--regparse;
	nr = '\\';
    }
    যদি (nr > INT_MAX)
	nr = INT_MAX;
    ফেরত nr;
}

/*
 * Free a compiled regexp program, returned by bt_regcomp().
 */
    স্থির শূন্য
bt_regfree(regprog_T *prog)
{
    vim_free(prog);
}

#define ADVANCE_REGINPUT() MB_PTR_ADV(rex.প্রবেশ)

/*
 * The arguments from BRACE_LIMITS are stored here.  They are actually local
 * to regmatch(), but they are here to reduce the amount of stack space used
 * (it can be called recursively many times).
 */
স্থির long	bl_minval;
স্থির long	bl_maxval;

/*
 * Save the input line and position in a regsave_T.
 */
    স্থির শূন্য
reg_save(regsave_T *save, garray_T *gap)
{
    যদি (REG_MULTI)
    {
	save->rs_u.pos.col = (colnr_T)(rex.প্রবেশ - rex.সারি);
	save->rs_u.pos.lnum = rex.lnum;
    }
    অন্যথা
	save->rs_u.ptr = rex.প্রবেশ;
    save->rs_len = gap->ga_len;
}

/*
 * Restore the input line and position from a regsave_T.
 */
    স্থির শূন্য
reg_restore(regsave_T *save, garray_T *gap)
{
    যদি (REG_MULTI)
    {
	যদি (rex.lnum != save->rs_u.pos.lnum)
	{
	    // only call reg_getline() when the line number changed to save
	    // a bit of time
	    rex.lnum = save->rs_u.pos.lnum;
	    rex.সারি = reg_getline(rex.lnum);
	}
	rex.প্রবেশ = rex.সারি + save->rs_u.pos.col;
    }
    অন্যথা
	rex.প্রবেশ = save->rs_u.ptr;
    gap->ga_len = save->rs_len;
}

/*
 * Return TRUE if current position is equal to saved position.
 */
    স্থির পূর্ণসংখ্যা
reg_save_equal(regsave_T *save)
{
    যদি (REG_MULTI)
	ফেরত rex.lnum == save->rs_u.pos.lnum
				  && rex.প্রবেশ == rex.সারি + save->rs_u.pos.col;
    ফেরত rex.প্রবেশ == save->rs_u.ptr;
}

// Save the sub-expressions before attempting a match.
#define save_se(savep, posp, pp) \
    REG_MULTI ? save_se_multi((savep), (posp)) : save_se_one((savep), (pp))

// After a failed match restore the sub-expressions.
#define restore_se(savep, posp, pp) \
{ \
    যদি (REG_MULTI) \
	*(posp) = (savep)->se_u.pos; \
    অন্যথা \
	*(pp) = (savep)->se_u.ptr; \
}

/*
 * Tentatively set the sub-expression start to the current position (after
 * calling regmatch() they will have changed).  Need to save the existing
 * values for when there is no match.
 * Use se_save() to use pointer (save_se_multi()) or position (save_se_one()),
 * depending on REG_MULTI.
 */
    স্থির শূন্য
save_se_multi(save_se_T *savep, lpos_T *posp)
{
    savep->se_u.pos = *posp;
    posp->lnum = rex.lnum;
    posp->col = (colnr_T)(rex.প্রবেশ - rex.সারি);
}

    স্থির শূন্য
save_se_one(save_se_T *savep, char_u **pp)
{
    savep->se_u.ptr = *pp;
    *pp = rex.প্রবেশ;
}

/*
 * regrepeat - repeatedly match something simple, return how many.
 * Advances rex.input (and rex.lnum) to just after the matched chars.
 */
    স্থির পূর্ণসংখ্যা
regrepeat(
    char_u	*p,
    long	maxcount)   // maximum number of matches allowed
{
    long	গুনতি = 0;
    char_u	*scan;
    char_u	*opnd;
    পূর্ণসংখ্যা		mask;
    পূর্ণসংখ্যা		testval = 0;

    scan = rex.প্রবেশ;	    // Make local copy of rex.input for speed.
    opnd = OPERAND(p);
    নির্বাচন (OP(p))
    {
      ক্ষেত্র ANY:
      ক্ষেত্র ANY + ADD_NL:
	যখন (গুনতি < maxcount)
	{
	    // Matching anything means we continue until end-of-line (or
	    // end-of-file for ANY + ADD_NL), only limited by maxcount.
	    যখন (*scan != NUL && গুনতি < maxcount)
	    {
		++গুনতি;
		MB_PTR_ADV(scan);
	    }
	    যদি (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
				      || rex.reg_সারি_lbr || গুনতি == maxcount)
		বিরতি;
	    ++গুনতি;		// count the line-break
	    reg_nextline();
	    scan = rex.প্রবেশ;
	    যদি (got_int)
		বিরতি;
	}
	বিরতি;

      ক্ষেত্র IDENT:
      ক্ষেত্র IDENT + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      ক্ষেত্র SIDENT:
      ক্ষেত্র SIDENT + ADD_NL:
	যখন (গুনতি < maxcount)
	{
	    যদি (vim_isIDc(PTR2CHAR(scan)) && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    অন্যথা যদি (*scan == NUL)
	    {
		যদি (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_সারি_lbr)
		    বিরতি;
		reg_nextline();
		scan = rex.প্রবেশ;
		যদি (got_int)
		    বিরতি;
	    }
	    অন্যথা যদি (rex.reg_সারি_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    অন্যথা
		বিরতি;
	    ++গুনতি;
	}
	বিরতি;

      ক্ষেত্র KWORD:
      ক্ষেত্র KWORD + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      ক্ষেত্র SKWORD:
      ক্ষেত্র SKWORD + ADD_NL:
	যখন (গুনতি < maxcount)
	{
	    যদি (vim_iswordp_buf(scan, rex.reg_buf)
					  && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    অন্যথা যদি (*scan == NUL)
	    {
		যদি (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_সারি_lbr)
		    বিরতি;
		reg_nextline();
		scan = rex.প্রবেশ;
		যদি (got_int)
		    বিরতি;
	    }
	    অন্যথা যদি (rex.reg_সারি_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    অন্যথা
		বিরতি;
	    ++গুনতি;
	}
	বিরতি;

      ক্ষেত্র FNAME:
      ক্ষেত্র FNAME + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      ক্ষেত্র SFNAME:
      ক্ষেত্র SFNAME + ADD_NL:
	যখন (গুনতি < maxcount)
	{
	    যদি (vim_isfilec(PTR2CHAR(scan)) && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    অন্যথা যদি (*scan == NUL)
	    {
		যদি (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_সারি_lbr)
		    বিরতি;
		reg_nextline();
		scan = rex.প্রবেশ;
		যদি (got_int)
		    বিরতি;
	    }
	    অন্যথা যদি (rex.reg_সারি_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    অন্যথা
		বিরতি;
	    ++গুনতি;
	}
	বিরতি;

      ক্ষেত্র PRINT:
      ক্ষেত্র PRINT + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      ক্ষেত্র SPRINT:
      ক্ষেত্র SPRINT + ADD_NL:
	যখন (গুনতি < maxcount)
	{
	    যদি (*scan == NUL)
	    {
		যদি (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_সারি_lbr)
		    বিরতি;
		reg_nextline();
		scan = rex.প্রবেশ;
		যদি (got_int)
		    বিরতি;
	    }
	    অন্যথা যদি (vim_isprintc(PTR2CHAR(scan)) == 1
					  && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    অন্যথা যদি (rex.reg_সারি_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    অন্যথা
		বিরতি;
	    ++গুনতি;
	}
	বিরতি;

      ক্ষেত্র WHITE:
      ক্ষেত্র WHITE + ADD_NL:
	testval = mask = RI_WHITE;
do_class:
	যখন (গুনতি < maxcount)
	{
	    পূর্ণসংখ্যা		l;

	    যদি (*scan == NUL)
	    {
		যদি (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_সারি_lbr)
		    বিরতি;
		reg_nextline();
		scan = rex.প্রবেশ;
		যদি (got_int)
		    বিরতি;
	    }
	    অন্যথা যদি (has_mbyte && (l = (*mb_ptr2len)(scan)) > 1)
	    {
		যদি (testval != 0)
		    বিরতি;
		scan += l;
	    }
	    অন্যথা যদি ((class_tab[*scan] & mask) == testval)
		++scan;
	    অন্যথা যদি (rex.reg_সারি_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    অন্যথা
		বিরতি;
	    ++গুনতি;
	}
	বিরতি;

      ক্ষেত্র NWHITE:
      ক্ষেত্র NWHITE + ADD_NL:
	mask = RI_WHITE;
	যাও do_class;
      ক্ষেত্র DIGIT:
      ক্ষেত্র DIGIT + ADD_NL:
	testval = mask = RI_DIGIT;
	যাও do_class;
      ক্ষেত্র NDIGIT:
      ক্ষেত্র NDIGIT + ADD_NL:
	mask = RI_DIGIT;
	যাও do_class;
      ক্ষেত্র HEX:
      ক্ষেত্র HEX + ADD_NL:
	testval = mask = RI_HEX;
	যাও do_class;
      ক্ষেত্র NHEX:
      ক্ষেত্র NHEX + ADD_NL:
	mask = RI_HEX;
	যাও do_class;
      ক্ষেত্র OCTAL:
      ক্ষেত্র OCTAL + ADD_NL:
	testval = mask = RI_OCTAL;
	যাও do_class;
      ক্ষেত্র NOCTAL:
      ক্ষেত্র NOCTAL + ADD_NL:
	mask = RI_OCTAL;
	যাও do_class;
      ক্ষেত্র WORD:
      ক্ষেত্র WORD + ADD_NL:
	testval = mask = RI_WORD;
	যাও do_class;
      ক্ষেত্র NWORD:
      ক্ষেত্র NWORD + ADD_NL:
	mask = RI_WORD;
	যাও do_class;
      ক্ষেত্র শীর্ষ_4:
      ক্ষেত্র শীর্ষ_4 + ADD_NL:
	testval = mask = RI_শীর্ষ;
	যাও do_class;
      ক্ষেত্র NHEAD:
      ক্ষেত্র NHEAD + ADD_NL:
	mask = RI_শীর্ষ;
	যাও do_class;
      ক্ষেত্র ALPHA:
      ক্ষেত্র ALPHA + ADD_NL:
	testval = mask = RI_ALPHA;
	যাও do_class;
      ক্ষেত্র NALPHA:
      ক্ষেত্র NALPHA + ADD_NL:
	mask = RI_ALPHA;
	যাও do_class;
      ক্ষেত্র LOWER:
      ক্ষেত্র LOWER + ADD_NL:
	testval = mask = RI_LOWER;
	যাও do_class;
      ক্ষেত্র NLOWER:
      ক্ষেত্র NLOWER + ADD_NL:
	mask = RI_LOWER;
	যাও do_class;
      ক্ষেত্র UPPER:
      ক্ষেত্র UPPER + ADD_NL:
	testval = mask = RI_UPPER;
	যাও do_class;
      ক্ষেত্র NUPPER:
      ক্ষেত্র NUPPER + ADD_NL:
	mask = RI_UPPER;
	যাও do_class;

      ক্ষেত্র EXACTLY:
	{
	    পূর্ণসংখ্যা	    cu, cl;

	    // This doesn't do a multi-byte character, because a MULTIBYTECODE
	    // would have been used for it.  It does handle single-byte
	    // characters, such as latin1.
	    যদি (rex.reg_ic)
	    {
		cu = MB_TOUPPER(*opnd);
		cl = MB_TOLOWER(*opnd);
		যখন (গুনতি < maxcount && (*scan == cu || *scan == cl))
		{
		    গুনতি++;
		    scan++;
		}
	    }
	    অন্যথা
	    {
		cu = *opnd;
		যখন (গুনতি < maxcount && *scan == cu)
		{
		    গুনতি++;
		    scan++;
		}
	    }
	    বিরতি;
	}

      ক্ষেত্র MULTIBYTECODE:
	{
	    পূর্ণসংখ্যা		i, len, cf = 0;

	    // Safety check (just in case 'encoding' was changed since
	    // compiling the program).
	    যদি ((len = (*mb_ptr2len)(opnd)) > 1)
	    {
		যদি (rex.reg_ic && enc_utf8)
		    cf = utf_fold(utf_ptr2char(opnd));
		যখন (গুনতি < maxcount && (*mb_ptr2len)(scan) >= len)
		{
		    জন্য (i = 0; i < len; ++i)
			যদি (opnd[i] != scan[i])
			    বিরতি;
		    যদি (i < len && (!rex.reg_ic || !enc_utf8
					|| utf_fold(utf_ptr2char(scan)) != cf))
			বিরতি;
		    scan += len;
		    ++গুনতি;
		}
	    }
	}
	বিরতি;

      ক্ষেত্র ANYOF:
      ক্ষেত্র ANYOF + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH

      ক্ষেত্র ANYBUT:
      ক্ষেত্র ANYBUT + ADD_NL:
	যখন (গুনতি < maxcount)
	{
	    পূর্ণসংখ্যা len;

	    যদি (*scan == NUL)
	    {
		যদি (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_সারি_lbr)
		    বিরতি;
		reg_nextline();
		scan = rex.প্রবেশ;
		যদি (got_int)
		    বিরতি;
	    }
	    অন্যথা যদি (rex.reg_সারি_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    অন্যথা যদি (has_mbyte && (len = (*mb_ptr2len)(scan)) > 1)
	    {
		যদি ((cstrchr(opnd, (*mb_ptr2char)(scan)) == ফাঁকা) == testval)
		    বিরতি;
		scan += len;
	    }
	    অন্যথা
	    {
		যদি ((cstrchr(opnd, *scan) == ফাঁকা) == testval)
		    বিরতি;
		++scan;
	    }
	    ++গুনতি;
	}
	বিরতি;

      ক্ষেত্র NEWL:
	যখন (গুনতি < maxcount
		&& ((*scan == NUL && rex.lnum <= rex.reg_maxline
				       && !rex.reg_সারি_lbr && REG_MULTI)
		    || (*scan == '\n' && rex.reg_সারি_lbr)))
	{
	    গুনতি++;
	    যদি (rex.reg_সারি_lbr)
		ADVANCE_REGINPUT();
	    অন্যথা
		reg_nextline();
	    scan = rex.প্রবেশ;
	    যদি (got_int)
		বিরতি;
	}
	বিরতি;

      পূর্বনির্ধারিত:			// Oh dear.  Called inappropriately.
	iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	printf("Called regrepeat with op code %d\n", OP(p));
#endif
	বিরতি;
    }

    rex.প্রবেশ = scan;

    ফেরত (পূর্ণসংখ্যা)গুনতি;
}

/*
 * Push an item onto the regstack.
 * Returns pointer to new item.  Returns NULL when out of memory.
 */
    স্থির regitem_T *
regstack_ঠেলো(regstate_T state, char_u *scan)
{
    regitem_T	*rp;

    যদি ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
    {
	emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
	ফেরত ফাঁকা;
    }
    যদি (GA_GROW_FAILS(&regstack, (পূর্ণসংখ্যা)আকার(regitem_T)))
	ফেরত ফাঁকা;

    rp = (regitem_T *)((অক্ষর *)regstack.ga_তথ্য + regstack.ga_len);
    rp->rs_state = state;
    rp->rs_scan = scan;

    regstack.ga_len += আকার(regitem_T);
    ফেরত rp;
}

/*
 * Pop an item from the regstack.
 */
    স্থির শূন্য
regstack_বের_করো(char_u **scan)
{
    regitem_T	*rp;

    rp = (regitem_T *)((অক্ষর *)regstack.ga_তথ্য + regstack.ga_len) - 1;
    *scan = rp->rs_scan;

    regstack.ga_len -= আকার(regitem_T);
}

#ifdef FEAT_RELTIME
/*
 * Check if the timer expired, return TRUE if so.
 */
    স্থির পূর্ণসংখ্যা
bt_did_time_out(পূর্ণসংখ্যা *timed_out)
{
    যদি (*timeout_flag)
    {
	যদি (timed_out != ফাঁকা)
	{
# ifdef FEAT_EVAL
	    যদি (!*timed_out)
		ch_log(ফাঁকা, "BT regexp timed out");
# endif
	    *timed_out = TRUE;
	}
	ফেরত TRUE;
    }
    ফেরত FALSE;
}
#endif

/*
 * Save the current subexpr to "bp", so that they can be restored
 * later by restore_subexpr().
 */
    স্থির শূন্য
save_subexpr(regbehind_T *bp)
{
    পূর্ণসংখ্যা i;

    // When "rex.need_clear_subexpr" is set we don't need to save the values,
    // only remember that this flag needs to be set again when restoring.
    bp->save_need_clear_subexpr = rex.need_clear_subexpr;
    যদি (rex.need_clear_subexpr)
	ফেরত;

    জন্য (i = 0; i < NSUBEXP; ++i)
    {
	যদি (REG_MULTI)
	{
	    bp->save_start[i].se_u.pos = rex.reg_startpos[i];
	    bp->save_end[i].se_u.pos = rex.reg_endpos[i];
	}
	অন্যথা
	{
	    bp->save_start[i].se_u.ptr = rex.reg_startp[i];
	    bp->save_end[i].se_u.ptr = rex.reg_endp[i];
	}
    }
}

/*
 * Restore the subexpr from "bp".
 */
    স্থির শূন্য
restore_subexpr(regbehind_T *bp)
{
    পূর্ণসংখ্যা i;

    // Only need to restore saved values when they are not to be cleared.
    rex.need_clear_subexpr = bp->save_need_clear_subexpr;
    যদি (rex.need_clear_subexpr)
	ফেরত;

    জন্য (i = 0; i < NSUBEXP; ++i)
    {
	যদি (REG_MULTI)
	{
	    rex.reg_startpos[i] = bp->save_start[i].se_u.pos;
	    rex.reg_endpos[i] = bp->save_end[i].se_u.pos;
	}
	অন্যথা
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
    স্থির পূর্ণসংখ্যা
regmatch(
    char_u	*scan,		    // Current node.
    পূর্ণসংখ্যা		*timed_out UNUSED)  // flag set on timeout or NULL
{
  char_u	*পরবর্তী;		// Next node.
  পূর্ণসংখ্যা		op;
  পূর্ণসংখ্যা		c;
  regitem_T	*rp;
  পূর্ণসংখ্যা		no;
  পূর্ণসংখ্যা		status;		// one of the RA_ values:

  // Make "regstack" and "backpos" empty.  They are allocated and freed in
  // bt_regexec_both() to reduce malloc()/free() calls.
  regstack.ga_len = 0;
  backpos.ga_len = 0;

  // Repeat until "regstack" is empty.
  জন্য (;;)
  {
    // Some patterns may take a long time to match, e.g., "\([a-z]\+\)\+Q".
    // Allow interrupting them with CTRL-C.
    fast_breakcheck();

#ifdef DEBUG
    যদি (scan != ফাঁকা && regnarrate)
    {
	mch_errmsg((অক্ষর *)regprop(scan));
	mch_errmsg("(\n");
    }
#endif

    // Repeat for items that can be matched sequentially, without using the
    // regstack.
    জন্য (;;)
    {
	যদি (got_int || scan == ফাঁকা)
	{
	    status = RA_FAIL;
	    বিরতি;
	}
#ifdef FEAT_RELTIME
	যদি (bt_did_time_out(timed_out))
	{
	    status = RA_FAIL;
	    বিরতি;
	}
#endif
	status = RA_CONT;

#ifdef DEBUG
	যদি (regnarrate)
	{
	    mch_errmsg((অক্ষর *)regprop(scan));
	    mch_errmsg("...\n");
# ifdef FEAT_SYN_HL
	    যদি (re_extmatch_in != ফাঁকা)
	    {
		পূর্ণসংখ্যা i;

		mch_errmsg(_("External submatches:\n"));
		জন্য (i = 0; i < NSUBEXP; i++)
		{
		    mch_errmsg("    \"");
		    যদি (re_extmatch_in->matches[i] != ফাঁকা)
			mch_errmsg((অক্ষর *)re_extmatch_in->matches[i]);
		    mch_errmsg("\"\n");
		}
	    }
# endif
	}
#endif
	পরবর্তী = regnext(scan);

	op = OP(scan);
	// Check for character class with NL added.
	যদি (!rex.reg_সারি_lbr && WITH_NL(op) && REG_MULTI
			     && *rex.প্রবেশ == NUL && rex.lnum <= rex.reg_maxline)
	{
	    reg_nextline();
	}
	অন্যথা যদি (rex.reg_সারি_lbr && WITH_NL(op) && *rex.প্রবেশ == '\n')
	{
	    ADVANCE_REGINPUT();
	}
	অন্যথা
	{
	  যদি (WITH_NL(op))
	      op -= ADD_NL;
	  যদি (has_mbyte)
	      c = (*mb_ptr2char)(rex.প্রবেশ);
	  অন্যথা
	      c = *rex.প্রবেশ;
	  নির্বাচন (op)
	  {
	  ক্ষেত্র BOL:
	    যদি (rex.প্রবেশ != rex.সারি)
		status = RA_NOMATCH;
	    বিরতি;

	  ক্ষেত্র EOL:
	    যদি (c != NUL)
		status = RA_NOMATCH;
	    বিরতি;

	  ক্ষেত্র RE_BOF:
	    // We're not at the beginning of the file when below the first
	    // line where we started, not at the start of the line or we
	    // didn't start at the first line of the buffer.
	    যদি (rex.lnum != 0 || rex.প্রবেশ != rex.সারি
				       || (REG_MULTI && rex.reg_firstlnum > 1))
		status = RA_NOMATCH;
	    বিরতি;

	  ক্ষেত্র RE_EOF:
	    যদি (rex.lnum != rex.reg_maxline || c != NUL)
		status = RA_NOMATCH;
	    বিরতি;

	  ক্ষেত্র CURSOR:
	    // Check if the buffer is in a window and compare the
	    // rex.reg_win->w_cursor position to the match position.
	    যদি (rex.reg_win == ফাঁকা
		    || (rex.lnum + rex.reg_firstlnum
						 != rex.reg_win->w_cursor.lnum)
		    || ((colnr_T)(rex.প্রবেশ - rex.সারি)
						 != rex.reg_win->w_cursor.col))
		status = RA_NOMATCH;
	    বিরতি;

	  ক্ষেত্র RE_MARK:
	    // Compare the mark position to the match position.
	    {
		পূর্ণসংখ্যা	mark = OPERAND(scan)[0];
		পূর্ণসংখ্যা	cmp = OPERAND(scan)[1];
		pos_T	*pos;
		size_t	col = REG_MULTI ? rex.প্রবেশ - rex.সারি : 0;

		pos = getmark_buf(rex.reg_buf, mark, FALSE);

		// Line may have been freed, get it again.
		যদি (REG_MULTI)
		{
		    rex.সারি = reg_getline(rex.lnum);
		    rex.প্রবেশ = rex.সারি + col;
		}

		যদি (pos == ফাঁকা		     // mark doesn't exist
			|| pos->lnum <= 0)   // mark isn't set in reg_buf
		{
		    status = RA_NOMATCH;
		}
		অন্যথা
		{
		    colnr_T pos_col = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->col == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->col;

		    যদি ((pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_col == (colnr_T)(rex.প্রবেশ - rex.সারি)
				    ? (cmp == '<' || cmp == '>')
				    : (pos_col < (colnr_T)(rex.প্রবেশ - rex.সারি)
					? cmp != '>'
					: cmp != '<'))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? cmp != '>'
				    : cmp != '<')))
		    status = RA_NOMATCH;
		}
	    }
	    বিরতি;

	  ক্ষেত্র RE_VISUAL:
	    যদি (!reg_match_visual())
		status = RA_NOMATCH;
	    বিরতি;

	  ক্ষেত্র RE_LNUM:
	    যদি (!REG_MULTI || !re_অঙ্ক_cmp((long_u)(rex.lnum + rex.reg_firstlnum),
									scan))
		status = RA_NOMATCH;
	    বিরতি;

	  ক্ষেত্র RE_COL:
	    যদি (!re_অঙ্ক_cmp((long_u)(rex.প্রবেশ - rex.সারি) + 1, scan))
		status = RA_NOMATCH;
	    বিরতি;

	  ক্ষেত্র RE_VCOL:
	    {
		win_T	    *wp = rex.reg_win == ফাঁকা ? curwin : rex.reg_win;
		linenr_T    lnum = REG_MULTI ? rex.reg_firstlnum + rex.lnum : 1;
		long_u	    vcol;

		যদি (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_বাফার->b_ml.ml_সারি_গুনতি))
		    lnum = 1;
		vcol = (long_u)win_linetabsize(wp, lnum, rex.সারি,
					      (colnr_T)(rex.প্রবেশ - rex.সারি));
		যদি (!re_অঙ্ক_cmp(vcol + 1, scan))
		    status = RA_NOMATCH;
	    }
	    বিরতি;

	  ক্ষেত্র BOW:	// \<word; rex.input points to w
	    যদি (c == NUL)	// Can't match at end of line
		status = RA_NOMATCH;
	    অন্যথা যদি (has_mbyte)
	    {
		পূর্ণসংখ্যা this_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.প্রবেশ, rex.reg_buf);
		যদি (this_class <= 1)
		    status = RA_NOMATCH;  // not on a word at all
		অন্যথা যদি (reg_পূর্ব_class() == this_class)
		    status = RA_NOMATCH;  // previous char is in same word
	    }
	    অন্যথা
	    {
		যদি (!vim_iswordc_buf(c, rex.reg_buf) || (rex.প্রবেশ > rex.সারি
				&& vim_iswordc_buf(rex.প্রবেশ[-1], rex.reg_buf)))
		    status = RA_NOMATCH;
	    }
	    বিরতি;

	  ক্ষেত্র EOW:	// word\>; rex.input points after d
	    যদি (rex.প্রবেশ == rex.সারি)    // Can't match at start of line
		status = RA_NOMATCH;
	    অন্যথা যদি (has_mbyte)
	    {
		পূর্ণসংখ্যা this_class, পূর্ব_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.প্রবেশ, rex.reg_buf);
		পূর্ব_class = reg_পূর্ব_class();
		যদি (this_class == পূর্ব_class
			|| পূর্ব_class == 0 || পূর্ব_class == 1)
		    status = RA_NOMATCH;
	    }
	    অন্যথা
	    {
		যদি (!vim_iswordc_buf(rex.প্রবেশ[-1], rex.reg_buf)
			|| (rex.প্রবেশ[0] != NUL
					   && vim_iswordc_buf(c, rex.reg_buf)))
		    status = RA_NOMATCH;
	    }
	    বিরতি; // Matched with EOW

	  ক্ষেত্র ANY:
	    // ANY does not match new lines.
	    যদি (c == NUL)
		status = RA_NOMATCH;
	    অন্যথা
		ADVANCE_REGINPUT();
	    বিরতি;

	  ক্ষেত্র IDENT:
	    যদি (!vim_isIDc(c))
		status = RA_NOMATCH;
	    অন্যথা
		ADVANCE_REGINPUT();
	    বিরতি;

	  ক্ষেত্র SIDENT:
	    যদি (VIM_ISDIGIT(*rex.প্রবেশ) || !vim_isIDc(c))
		status = RA_NOMATCH;
	    অন্যথা
		ADVANCE_REGINPUT();
	    বিরতি;

	  ক্ষেত্র KWORD:
	    যদি (!vim_iswordp_buf(rex.প্রবেশ, rex.reg_buf))
		status = RA_NOMATCH;
	    অন্যথা
		ADVANCE_REGINPUT();
	    বিরতি;

	  ক্ষেত্র SKWORD:
	    যদি (VIM_ISDIGIT(*rex.প্রবেশ)
				    || !vim_iswordp_buf(rex.প্রবেশ, rex.reg_buf))
		status = RA_NOMATCH;
	    অন্যথা
		ADVANCE_REGINPUT();
	    বিরতি;

	  ক্ষেত্র FNAME:
	    যদি (!vim_isfilec(c))
		status = RA_NOMATCH;
	    অন্যথা
		ADVANCE_REGINPUT();
	    বিরতি;

	  ক্ষেত্র SFNAME:
	    যদি (VIM_ISDIGIT(*rex.প্রবেশ) || !vim_isfilec(c))
		status = RA_NOMATCH;
	    অন্যথা
		ADVANCE_REGINPUT();
	    বিরতি;

	  ক্ষেত্র PRINT:
	    যদি (!vim_isprintc(PTR2CHAR(rex.প্রবেশ)))
		status = RA_NOMATCH;
	    অন্যথা
		ADVANCE_REGINPUT();
	    বিরতি;

	  ক্ষেত্র SPRINT:
	    যদি (VIM_ISDIGIT(*rex.প্রবেশ) || !vim_isprintc(PTR2CHAR(rex.প্রবেশ)))
		status = RA_NOMATCH;
	    অন্যথা
		ADVANCE_REGINPUT();
	    বিরতি;

	  ক্ষেত্র WHITE:
	    যদি (!VIM_ISWHITE(c))
		status = RA_NOMATCH;
	    অন্যথা
		ADVANCE_REGINPUT();
	    বিরতি;

	  ক্ষেত্র NWHITE:
	    যদি (c == NUL || VIM_ISWHITE(c))
		status = RA_NOMATCH;
	    অন্যথা
		ADVANCE_REGINPUT();
	    বিরতি;

	  ক্ষেত্র DIGIT:
	    যদি (!ri_digit(c))
		status = RA_NOMATCH;
	    অন্যথা
		ADVANCE_REGINPUT();
	    বিরতি;

	  ক্ষেত্র NDIGIT:
	    যদি (c == NUL || ri_digit(c))
		status = RA_NOMATCH;
	    অন্যথা
		ADVANCE_REGINPUT();
	    বিরতি;

	  ক্ষেত্র HEX:
	    যদি (!ri_hex(c))
		status = RA_NOMATCH;
	    অন্যথা
		ADVANCE_REGINPUT();
	    বিরতি;

	  ক্ষেত্র NHEX:
	    যদি (c == NUL || ri_hex(c))
		status = RA_NOMATCH;
	    অন্যথা
		ADVANCE_REGINPUT();
	    বিরতি;

	  ক্ষেত্র OCTAL:
	    যদি (!ri_octal(c))
		status = RA_NOMATCH;
	    অন্যথা
		ADVANCE_REGINPUT();
	    বিরতি;

	  ক্ষেত্র NOCTAL:
	    যদি (c == NUL || ri_octal(c))
		status = RA_NOMATCH;
	    অন্যথা
		ADVANCE_REGINPUT();
	    বিরতি;

	  ক্ষেত্র WORD:
	    যদি (!ri_word(c))
		status = RA_NOMATCH;
	    অন্যথা
		ADVANCE_REGINPUT();
	    বিরতি;

	  ক্ষেত্র NWORD:
	    যদি (c == NUL || ri_word(c))
		status = RA_NOMATCH;
	    অন্যথা
		ADVANCE_REGINPUT();
	    বিরতি;

	  ক্ষেত্র শীর্ষ_4:
	    যদি (!ri_শীর্ষ(c))
		status = RA_NOMATCH;
	    অন্যথা
		ADVANCE_REGINPUT();
	    বিরতি;

	  ক্ষেত্র NHEAD:
	    যদি (c == NUL || ri_শীর্ষ(c))
		status = RA_NOMATCH;
	    অন্যথা
		ADVANCE_REGINPUT();
	    বিরতি;

	  ক্ষেত্র ALPHA:
	    যদি (!ri_alpha(c))
		status = RA_NOMATCH;
	    অন্যথা
		ADVANCE_REGINPUT();
	    বিরতি;

	  ক্ষেত্র NALPHA:
	    যদি (c == NUL || ri_alpha(c))
		status = RA_NOMATCH;
	    অন্যথা
		ADVANCE_REGINPUT();
	    বিরতি;

	  ক্ষেত্র LOWER:
	    যদি (!ri_lower(c))
		status = RA_NOMATCH;
	    অন্যথা
		ADVANCE_REGINPUT();
	    বিরতি;

	  ক্ষেত্র NLOWER:
	    যদি (c == NUL || ri_lower(c))
		status = RA_NOMATCH;
	    অন্যথা
		ADVANCE_REGINPUT();
	    বিরতি;

	  ক্ষেত্র UPPER:
	    যদি (!ri_upper(c))
		status = RA_NOMATCH;
	    অন্যথা
		ADVANCE_REGINPUT();
	    বিরতি;

	  ক্ষেত্র NUPPER:
	    যদি (c == NUL || ri_upper(c))
		status = RA_NOMATCH;
	    অন্যথা
		ADVANCE_REGINPUT();
	    বিরতি;

	  ক্ষেত্র EXACTLY:
	    {
		পূর্ণসংখ্যা	len;
		char_u	*opnd;

		opnd = OPERAND(scan);
		// Inline the first byte, for speed.
		যদি (*opnd != *rex.প্রবেশ
			&& (!rex.reg_ic
			    || (!enc_utf8
			      && MB_TOLOWER(*opnd) != MB_TOLOWER(*rex.প্রবেশ))))
		    status = RA_NOMATCH;
		অন্যথা যদি (*opnd == NUL)
		{
		    // match empty string always works; happens when "~" is
		    // empty.
		}
		অন্যথা
		{
		    যদি (opnd[1] == NUL && !(enc_utf8 && rex.reg_ic))
		    {
			len = 1;	// matched a single byte above
		    }
		    অন্যথা
		    {
			// Need to match first byte again for multi-byte.
			len = (পূর্ণসংখ্যা)STRLEN(opnd);
			যদি (cstrncmp(opnd, rex.প্রবেশ, &len) != 0)
			    status = RA_NOMATCH;
		    }
		    // Check for following composing character, unless %C
		    // follows (skips over all composing chars).
		    যদি (status != RA_NOMATCH
			    && enc_utf8
			    && UTF_COMPOSINGLIKE(rex.প্রবেশ, rex.প্রবেশ + len)
			    && !rex.reg_icombine
			    && OP(পরবর্তী) != RE_COMPOSING)
		    {
			// raaron: This code makes a composing character get
			// ignored, which is the correct behavior (sometimes)
			// for voweled Hebrew texts.
			status = RA_NOMATCH;
		    }
		    যদি (status != RA_NOMATCH)
			rex.প্রবেশ += len;
		}
	    }
	    বিরতি;

	  ক্ষেত্র ANYOF:
	  ক্ষেত্র ANYBUT:
	    {
		char_u  *q = OPERAND(scan);

		যদি (c == NUL)
		    status = RA_NOMATCH;
		অন্যথা যদি ((cstrchr(q, c) == ফাঁকা) == (op == ANYOF))
		    status = RA_NOMATCH;
		অন্যথা
		{
		    // Check following combining characters
		    পূর্ণসংখ্যা	len = 0;
		    পূর্ণসংখ্যা i;

		    যদি (enc_utf8)
			len = utfc_ptr2len(q) - utf_ptr2len(q);

		    MB_CPTR_ADV(rex.প্রবেশ);
		    MB_CPTR_ADV(q);

		    যদি (!enc_utf8 || len == 0)
			বিরতি;

		    জন্য (i = 0; i < len; ++i)
			যদি (q[i] != rex.প্রবেশ[i])
			{
			    status = RA_NOMATCH;
			    বিরতি;
			}
		    rex.প্রবেশ += len;
		}
		বিরতি;
	    }

	  ক্ষেত্র MULTIBYTECODE:
	    যদি (has_mbyte)
	    {
		পূর্ণসংখ্যা	i, len;
		char_u	*opnd;
		পূর্ণসংখ্যা	opndc = 0, inpc;

		opnd = OPERAND(scan);
		// Safety check (just in case 'encoding' was changed since
		// compiling the program).
		যদি ((len = (*mb_ptr2len)(opnd)) < 2)
		{
		    status = RA_NOMATCH;
		    বিরতি;
		}
		যদি (enc_utf8)
		    opndc = utf_ptr2char(opnd);
		যদি (enc_utf8 && utf_iscomposing(opndc))
		{
		    // When only a composing char is given match at any
		    // position where that composing char appears.
		    status = RA_NOMATCH;
		    জন্য (i = 0; rex.প্রবেশ[i] != NUL;
						i += utf_ptr2len(rex.প্রবেশ + i))
		    {
			inpc = utf_ptr2char(rex.প্রবেশ + i);
			যদি (!utf_iscomposing(inpc))
			{
			    যদি (i > 0)
				বিরতি;
			}
			অন্যথা যদি (opndc == inpc)
			{
			    // Include all following composing chars.
			    len = i + utfc_ptr2len(rex.প্রবেশ + i);
			    status = RA_MATCH;
			    বিরতি;
			}
		    }
		}
		অন্যথা যদি (enc_utf8)
		{
		    যদি (cstrncmp(opnd, rex.প্রবেশ, &len) != 0)
		    {
			status = RA_NOMATCH;
			বিরতি;
		    }
		}
		অন্যথা
		    জন্য (i = 0; i < len; ++i)
			যদি (opnd[i] != rex.প্রবেশ[i])
			{
			    status = RA_NOMATCH;
			    বিরতি;
			}
		rex.প্রবেশ += len;
	    }
	    অন্যথা
		status = RA_NOMATCH;
	    বিরতি;
	  ক্ষেত্র RE_COMPOSING:
	    যদি (enc_utf8)
	    {
		// Skip composing characters.
		যখন (utf_iscomposing(utf_ptr2char(rex.প্রবেশ)))
		    MB_CPTR_ADV(rex.প্রবেশ);
	    }
	    বিরতি;

	  ক্ষেত্র NOTHING:
	    বিরতি;

	  ক্ষেত্র BACK:
	    {
		পূর্ণসংখ্যা		i;
		backpos_T	*bp;

		// When we run into BACK we need to check if we don't keep
		// looping without matching any input.  The second and later
		// times a BACK is encountered it fails if the input is still
		// at the same position as the previous time.
		// The positions are stored in "backpos" and found by the
		// current value of "scan", the position in the RE program.
		bp = (backpos_T *)backpos.ga_তথ্য;
		জন্য (i = 0; i < backpos.ga_len; ++i)
		    যদি (bp[i].bp_scan == scan)
			বিরতি;
		যদি (i == backpos.ga_len)
		{
		    // First time at this BACK, make room to store the pos.
		    যদি (GA_GROW_FAILS(&backpos, 1))
			status = RA_FAIL;
		    অন্যথা
		    {
			// get "ga_data" again, it may have changed
			bp = (backpos_T *)backpos.ga_তথ্য;
			bp[i].bp_scan = scan;
			++backpos.ga_len;
		    }
		}
		অন্যথা যদি (reg_save_equal(&bp[i].bp_pos))
		    // Still at same position as last time, fail.
		    status = RA_NOMATCH;

		যদি (status != RA_FAIL && status != RA_NOMATCH)
		    reg_save(&bp[i].bp_pos, &backpos);
	    }
	    বিরতি;

	  ক্ষেত্র MOPEN + 0:   // Match start: \zs
	  ক্ষেত্র MOPEN + 1:   // \(
	  ক্ষেত্র MOPEN + 2:
	  ক্ষেত্র MOPEN + 3:
	  ক্ষেত্র MOPEN + 4:
	  ক্ষেত্র MOPEN + 5:
	  ক্ষেত্র MOPEN + 6:
	  ক্ষেত্র MOPEN + 7:
	  ক্ষেত্র MOPEN + 8:
	  ক্ষেত্র MOPEN + 9:
	    {
		no = op - MOPEN;
		cleanup_subexpr();
		rp = regstack_ঠেলো(RS_MOPEN, scan);
		যদি (rp == ফাঁকা)
		    status = RA_FAIL;
		অন্যথা
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_startpos[no],
							  &rex.reg_startp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    বিরতি;

	  ক্ষেত্র NOPEN:	    // \%(
	  ক্ষেত্র NCLOSE:	    // \) after \%(
		যদি (regstack_ঠেলো(RS_NOPEN, scan) == ফাঁকা)
		    status = RA_FAIL;
		// We simply continue and handle the result when done.
		বিরতি;

#ifdef FEAT_SYN_HL
	  ক্ষেত্র ZOPEN + 1:
	  ক্ষেত্র ZOPEN + 2:
	  ক্ষেত্র ZOPEN + 3:
	  ক্ষেত্র ZOPEN + 4:
	  ক্ষেত্র ZOPEN + 5:
	  ক্ষেত্র ZOPEN + 6:
	  ক্ষেত্র ZOPEN + 7:
	  ক্ষেত্র ZOPEN + 8:
	  ক্ষেত্র ZOPEN + 9:
	    {
		no = op - ZOPEN;
		cleanup_zsubexpr();
		rp = regstack_ঠেলো(RS_ZOPEN, scan);
		যদি (rp == ফাঁকা)
		    status = RA_FAIL;
		অন্যথা
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_startzpos[no],
							     &reg_startzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    বিরতি;
#endif

	  ক্ষেত্র MCLOSE + 0:  // Match end: \ze
	  ক্ষেত্র MCLOSE + 1:  // \)
	  ক্ষেত্র MCLOSE + 2:
	  ক্ষেত্র MCLOSE + 3:
	  ক্ষেত্র MCLOSE + 4:
	  ক্ষেত্র MCLOSE + 5:
	  ক্ষেত্র MCLOSE + 6:
	  ক্ষেত্র MCLOSE + 7:
	  ক্ষেত্র MCLOSE + 8:
	  ক্ষেত্র MCLOSE + 9:
	    {
		no = op - MCLOSE;
		cleanup_subexpr();
		rp = regstack_ঠেলো(RS_MCLOSE, scan);
		যদি (rp == ফাঁকা)
		    status = RA_FAIL;
		অন্যথা
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_endpos[no],
							    &rex.reg_endp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    বিরতি;

#ifdef FEAT_SYN_HL
	  ক্ষেত্র ZCLOSE + 1:  // \) after \z(
	  ক্ষেত্র ZCLOSE + 2:
	  ক্ষেত্র ZCLOSE + 3:
	  ক্ষেত্র ZCLOSE + 4:
	  ক্ষেত্র ZCLOSE + 5:
	  ক্ষেত্র ZCLOSE + 6:
	  ক্ষেত্র ZCLOSE + 7:
	  ক্ষেত্র ZCLOSE + 8:
	  ক্ষেত্র ZCLOSE + 9:
	    {
		no = op - ZCLOSE;
		cleanup_zsubexpr();
		rp = regstack_ঠেলো(RS_ZCLOSE, scan);
		যদি (rp == ফাঁকা)
		    status = RA_FAIL;
		অন্যথা
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_endzpos[no],
							      &reg_endzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    বিরতি;
#endif

	  ক্ষেত্র BACKREF + 1:
	  ক্ষেত্র BACKREF + 2:
	  ক্ষেত্র BACKREF + 3:
	  ক্ষেত্র BACKREF + 4:
	  ক্ষেত্র BACKREF + 5:
	  ক্ষেত্র BACKREF + 6:
	  ক্ষেত্র BACKREF + 7:
	  ক্ষেত্র BACKREF + 8:
	  ক্ষেত্র BACKREF + 9:
	    {
		পূর্ণসংখ্যা		len;

		no = op - BACKREF;
		cleanup_subexpr();
		যদি (!REG_MULTI)		// Single-line regexp
		{
		    যদি (rex.reg_startp[no] == ফাঁকা || rex.reg_endp[no] == ফাঁকা)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    অন্যথা
		    {
			// Compare current input with back-ref in the same
			// line.
			len = (পূর্ণসংখ্যা)(rex.reg_endp[no] - rex.reg_startp[no]);
			যদি (cstrncmp(rex.reg_startp[no], rex.প্রবেশ, &len) != 0)
			    status = RA_NOMATCH;
		    }
		}
		অন্যথা				// Multi-line regexp
		{
		    যদি (rex.reg_startpos[no].lnum < 0
						|| rex.reg_endpos[no].lnum < 0)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    অন্যথা
		    {
			যদি (rex.reg_startpos[no].lnum == rex.lnum
				&& rex.reg_endpos[no].lnum == rex.lnum)
			{
			    // Compare back-ref within the current line.
			    len = rex.reg_endpos[no].col
						    - rex.reg_startpos[no].col;
			    যদি (cstrncmp(rex.সারি + rex.reg_startpos[no].col,
							  rex.প্রবেশ, &len) != 0)
				status = RA_NOMATCH;
			}
			অন্যথা
			{
			    // Messy situation: Need to compare between two
			    // lines.
			    পূর্ণসংখ্যা r = match_with_backref(
					    rex.reg_startpos[no].lnum,
					    rex.reg_startpos[no].col,
					    rex.reg_endpos[no].lnum,
					    rex.reg_endpos[no].col,
					    &len);

			    যদি (r != RA_MATCH)
				status = r;
			}
		    }
		}

		// Matched the backref, skip over it.
		rex.প্রবেশ += len;
	    }
	    বিরতি;

#ifdef FEAT_SYN_HL
	  ক্ষেত্র ZREF + 1:
	  ক্ষেত্র ZREF + 2:
	  ক্ষেত্র ZREF + 3:
	  ক্ষেত্র ZREF + 4:
	  ক্ষেত্র ZREF + 5:
	  ক্ষেত্র ZREF + 6:
	  ক্ষেত্র ZREF + 7:
	  ক্ষেত্র ZREF + 8:
	  ক্ষেত্র ZREF + 9:
	    {
		পূর্ণসংখ্যা	len;

		cleanup_zsubexpr();
		no = op - ZREF;
		যদি (re_extmatch_in != ফাঁকা
			&& re_extmatch_in->matches[no] != ফাঁকা)
		{
		    len = (পূর্ণসংখ্যা)STRLEN(re_extmatch_in->matches[no]);
		    যদি (cstrncmp(re_extmatch_in->matches[no],
							  rex.প্রবেশ, &len) != 0)
			status = RA_NOMATCH;
		    অন্যথা
			rex.প্রবেশ += len;
		}
		অন্যথা
		{
		    // Backref was not set: Match an empty string.
		}
	    }
	    বিরতি;
#endif

	  ক্ষেত্র BRANCH:
	    {
		যদি (OP(পরবর্তী) != BRANCH) // No choice.
		    পরবর্তী = OPERAND(scan);	// Avoid recursion.
		অন্যথা
		{
		    rp = regstack_ঠেলো(RS_BRANCH, scan);
		    যদি (rp == ফাঁকা)
			status = RA_FAIL;
		    অন্যথা
			status = RA_BREAK;	// rest is below
		}
	    }
	    বিরতি;

	  ক্ষেত্র BRACE_LIMITS:
	    {
		যদি (OP(পরবর্তী) == BRACE_SIMPLE)
		{
		    bl_minval = OPERAND_MIN(scan);
		    bl_maxval = OPERAND_MAX(scan);
		}
		অন্যথা যদি (OP(পরবর্তী) >= BRACE_COMPLEX
			&& OP(পরবর্তী) < BRACE_COMPLEX + 10)
		{
		    no = OP(পরবর্তী) - BRACE_COMPLEX;
		    brace_min[no] = OPERAND_MIN(scan);
		    brace_max[no] = OPERAND_MAX(scan);
		    brace_গুনতি[no] = 0;
		}
		অন্যথা
		{
		    internal_error("BRACE_LIMITS");
		    status = RA_FAIL;
		}
	    }
	    বিরতি;

	  ক্ষেত্র BRACE_COMPLEX + 0:
	  ক্ষেত্র BRACE_COMPLEX + 1:
	  ক্ষেত্র BRACE_COMPLEX + 2:
	  ক্ষেত্র BRACE_COMPLEX + 3:
	  ক্ষেত্র BRACE_COMPLEX + 4:
	  ক্ষেত্র BRACE_COMPLEX + 5:
	  ক্ষেত্র BRACE_COMPLEX + 6:
	  ক্ষেত্র BRACE_COMPLEX + 7:
	  ক্ষেত্র BRACE_COMPLEX + 8:
	  ক্ষেত্র BRACE_COMPLEX + 9:
	    {
		no = op - BRACE_COMPLEX;
		++brace_গুনতি[no];

		// If not matched enough times yet, try one more
		যদি (brace_গুনতি[no] <= (brace_min[no] <= brace_max[no]
					     ? brace_min[no] : brace_max[no]))
		{
		    rp = regstack_ঠেলো(RS_BRCPLX_MORE, scan);
		    যদি (rp == ফাঁকা)
			status = RA_FAIL;
		    অন্যথা
		    {
			rp->rs_no = no;
			reg_save(&rp->rs_un.regsave, &backpos);
			পরবর্তী = OPERAND(scan);
			// We continue and handle the result when done.
		    }
		    বিরতি;
		}

		// If matched enough times, may try matching some more
		যদি (brace_min[no] <= brace_max[no])
		{
		    // Range is the normal way around, use longest match
		    যদি (brace_গুনতি[no] <= brace_max[no])
		    {
			rp = regstack_ঠেলো(RS_BRCPLX_LONG, scan);
			যদি (rp == ফাঁকা)
			    status = RA_FAIL;
			অন্যথা
			{
			    rp->rs_no = no;
			    reg_save(&rp->rs_un.regsave, &backpos);
			    পরবর্তী = OPERAND(scan);
			    // We continue and handle the result when done.
			}
		    }
		}
		অন্যথা
		{
		    // Range is backwards, use shortest match first
		    যদি (brace_গুনতি[no] <= brace_min[no])
		    {
			rp = regstack_ঠেলো(RS_BRCPLX_SHORT, scan);
			যদি (rp == ফাঁকা)
			    status = RA_FAIL;
			অন্যথা
			{
			    reg_save(&rp->rs_un.regsave, &backpos);
			    // We continue and handle the result when done.
			}
		    }
		}
	    }
	    বিরতি;

	  ক্ষেত্র BRACE_SIMPLE:
	  ক্ষেত্র STAR:
	  ক্ষেত্র PLUS:
	    {
		regstar_T	rst;

		// Lookahead to avoid useless match attempts when we know
		// what character comes next.
		যদি (OP(পরবর্তী) == EXACTLY)
		{
		    rst.nextb = *OPERAND(পরবর্তী);
		    যদি (rex.reg_ic)
		    {
			যদি (MB_ISUPPER(rst.nextb))
			    rst.nextb_ic = MB_TOLOWER(rst.nextb);
			অন্যথা
			    rst.nextb_ic = MB_TOUPPER(rst.nextb);
		    }
		    অন্যথা
			rst.nextb_ic = rst.nextb;
		}
		অন্যথা
		{
		    rst.nextb = NUL;
		    rst.nextb_ic = NUL;
		}
		যদি (op != BRACE_SIMPLE)
		{
		    rst.minval = (op == STAR) ? 0 : 1;
		    rst.maxval = MAX_LIMIT;
		}
		অন্যথা
		{
		    rst.minval = bl_minval;
		    rst.maxval = bl_maxval;
		}

		// When maxval > minval, try matching as much as possible, up
		// to maxval.  When maxval < minval, try matching at least the
		// minimal number (since the range is backwards, that's also
		// maxval!).
		rst.গুনতি = regrepeat(OPERAND(scan), rst.maxval);
		যদি (got_int)
		{
		    status = RA_FAIL;
		    বিরতি;
		}
		যদি (rst.minval <= rst.maxval
			  ? rst.গুনতি >= rst.minval : rst.গুনতি >= rst.maxval)
		{
		    // It could match.  Prepare for trying to match what
		    // follows.  The code is below.  Parameters are stored in
		    // a regstar_T on the regstack.
		    যদি ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
		    {
			emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
			status = RA_FAIL;
		    }
		    অন্যথা যদি (GA_GROW_FAILS(&regstack, (পূর্ণসংখ্যা)আকার(regstar_T)))
			status = RA_FAIL;
		    অন্যথা
		    {
			regstack.ga_len += আকার(regstar_T);
			rp = regstack_ঠেলো(rst.minval <= rst.maxval
					? RS_STAR_LONG : RS_STAR_SHORT, scan);
			যদি (rp == ফাঁকা)
			    status = RA_FAIL;
			অন্যথা
			{
			    *(((regstar_T *)rp) - 1) = rst;
			    status = RA_BREAK;	    // skip the restore bits
			}
		    }
		}
		অন্যথা
		    status = RA_NOMATCH;

	    }
	    বিরতি;

	  ক্ষেত্র NOMATCH:
	  ক্ষেত্র MATCH:
	  ক্ষেত্র SUBPAT:
	    rp = regstack_ঠেলো(RS_NOMATCH, scan);
	    যদি (rp == ফাঁকা)
		status = RA_FAIL;
	    অন্যথা
	    {
		rp->rs_no = op;
		reg_save(&rp->rs_un.regsave, &backpos);
		পরবর্তী = OPERAND(scan);
		// We continue and handle the result when done.
	    }
	    বিরতি;

	  ক্ষেত্র BEHIND:
	  ক্ষেত্র NOBEHIND:
	    // Need a bit of room to store extra positions.
	    যদি ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
	    {
		emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		status = RA_FAIL;
	    }
	    অন্যথা যদি (GA_GROW_FAILS(&regstack, (পূর্ণসংখ্যা)আকার(regbehind_T)))
		status = RA_FAIL;
	    অন্যথা
	    {
		regstack.ga_len += আকার(regbehind_T);
		rp = regstack_ঠেলো(RS_BEHIND1, scan);
		যদি (rp == ফাঁকা)
		    status = RA_FAIL;
		অন্যথা
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
	    বিরতি;

	  ক্ষেত্র BHPOS:
	    যদি (REG_MULTI)
	    {
		যদি (behind_pos.rs_u.pos.col != (colnr_T)(rex.প্রবেশ - rex.সারি)
			|| behind_pos.rs_u.pos.lnum != rex.lnum)
		    status = RA_NOMATCH;
	    }
	    অন্যথা যদি (behind_pos.rs_u.ptr != rex.প্রবেশ)
		status = RA_NOMATCH;
	    বিরতি;

	  ক্ষেত্র NEWL:
	    যদি ((c != NUL || !REG_MULTI || rex.lnum > rex.reg_maxline
			     || rex.reg_সারি_lbr)
					   && (c != '\n' || !rex.reg_সারি_lbr))
		status = RA_NOMATCH;
	    অন্যথা যদি (rex.reg_সারি_lbr)
		ADVANCE_REGINPUT();
	    অন্যথা
		reg_nextline();
	    বিরতি;

	  ক্ষেত্র END:
	    status = RA_MATCH;	// Success!
	    বিরতি;

	  পূর্বনির্ধারিত:
	    iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	    printf("Illegal op code %d\n", op);
#endif
	    status = RA_FAIL;
	    বিরতি;
	  }
	}

	// If we can't continue sequentially, break the inner loop.
	যদি (status != RA_CONT)
	    বিরতি;

	// Continue in inner loop, advance to next item.
	scan = পরবর্তী;

    } // end of inner loop

    // If there is something on the regstack execute the code for the state.
    // If the state is popped then loop and use the older state.
    যখন (regstack.ga_len > 0 && status != RA_FAIL)
    {
	rp = (regitem_T *)((অক্ষর *)regstack.ga_তথ্য + regstack.ga_len) - 1;
	নির্বাচন (rp->rs_state)
	{
	  ক্ষেত্র RS_NOPEN:
	    // Result is passed on as-is, simply pop the state.
	    regstack_বের_করো(&scan);
	    বিরতি;

	  ক্ষেত্র RS_MOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    যদি (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_startpos[rp->rs_no],
						  &rex.reg_startp[rp->rs_no]);
	    regstack_বের_করো(&scan);
	    বিরতি;

#ifdef FEAT_SYN_HL
	  ক্ষেত্র RS_ZOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    যদি (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_startzpos[rp->rs_no],
						 &reg_startzp[rp->rs_no]);
	    regstack_বের_করো(&scan);
	    বিরতি;
#endif

	  ক্ষেত্র RS_MCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    যদি (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_endpos[rp->rs_no],
						    &rex.reg_endp[rp->rs_no]);
	    regstack_বের_করো(&scan);
	    বিরতি;

#ifdef FEAT_SYN_HL
	  ক্ষেত্র RS_ZCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    যদি (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_endzpos[rp->rs_no],
						   &reg_endzp[rp->rs_no]);
	    regstack_বের_করো(&scan);
	    বিরতি;
#endif

	  ক্ষেত্র RS_BRANCH:
	    যদি (status == RA_MATCH)
		// this branch matched, use it
		regstack_বের_করো(&scan);
	    অন্যথা
	    {
		যদি (status != RA_BREAK)
		{
		    // After a non-matching branch: try next one.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    scan = rp->rs_scan;
		}
		যদি (scan == ফাঁকা || OP(scan) != BRANCH)
		{
		    // no more branches, didn't find a match
		    status = RA_NOMATCH;
		    regstack_বের_করো(&scan);
		}
		অন্যথা
		{
		    // Prepare to try a branch.
		    rp->rs_scan = regnext(scan);
		    reg_save(&rp->rs_un.regsave, &backpos);
		    scan = OPERAND(scan);
		}
	    }
	    বিরতি;

	  ক্ষেত্র RS_BRCPLX_MORE:
	    // Pop the state.  Restore pointers when there is no match.
	    যদি (status == RA_NOMATCH)
	    {
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_গুনতি[rp->rs_no];	// decrement match count
	    }
	    regstack_বের_করো(&scan);
	    বিরতি;

	  ক্ষেত্র RS_BRCPLX_LONG:
	    // Pop the state.  Restore pointers when there is no match.
	    যদি (status == RA_NOMATCH)
	    {
		// There was no match, but we did find enough matches.
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_গুনতি[rp->rs_no];
		// continue with the items after "\{}"
		status = RA_CONT;
	    }
	    regstack_বের_করো(&scan);
	    যদি (status == RA_CONT)
		scan = regnext(scan);
	    বিরতি;

	  ক্ষেত্র RS_BRCPLX_SHORT:
	    // Pop the state.  Restore pointers when there is no match.
	    যদি (status == RA_NOMATCH)
		// There was no match, try to match one more item.
		reg_restore(&rp->rs_un.regsave, &backpos);
	    regstack_বের_করো(&scan);
	    যদি (status == RA_NOMATCH)
	    {
		scan = OPERAND(scan);
		status = RA_CONT;
	    }
	    বিরতি;

	  ক্ষেত্র RS_NOMATCH:
	    // Pop the state.  If the operand matches for NOMATCH or
	    // doesn't match for MATCH/SUBPAT, we fail.  Otherwise backup,
	    // except for SUBPAT, and continue with the next item.
	    যদি (status == (rp->rs_no == NOMATCH ? RA_MATCH : RA_NOMATCH))
		status = RA_NOMATCH;
	    অন্যথা
	    {
		status = RA_CONT;
		যদি (rp->rs_no != SUBPAT)	// zero-width
		    reg_restore(&rp->rs_un.regsave, &backpos);
	    }
	    regstack_বের_করো(&scan);
	    যদি (status == RA_CONT)
		scan = regnext(scan);
	    বিরতি;

	  ক্ষেত্র RS_BEHIND1:
	    যদি (status == RA_NOMATCH)
	    {
		regstack_বের_করো(&scan);
		regstack.ga_len -= আকার(regbehind_T);
	    }
	    অন্যথা
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
	    বিরতি;

	  ক্ষেত্র RS_BEHIND2:
	    // Looping for BEHIND / NOBEHIND match.
	    যদি (status == RA_MATCH && reg_save_equal(&behind_pos))
	    {
		// found a match that ends where "next" started
		behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		যদি (rp->rs_no == BEHIND)
		    reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
		অন্যথা
		{
		    // But we didn't want a match.  Need to restore the
		    // subexpr, because what follows matched, so they have
		    // been set.
		    status = RA_NOMATCH;
		    restore_subexpr(((regbehind_T *)rp) - 1);
		}
		regstack_বের_করো(&scan);
		regstack.ga_len -= আকার(regbehind_T);
	    }
	    অন্যথা
	    {
		long limit;

		// No match or a match that doesn't end where we want it: Go
		// back one character.  May go to previous line once.
		no = OK;
		limit = OPERAND_MIN(rp->rs_scan);
		যদি (REG_MULTI)
		{
		    যদি (limit > 0
			    && ((rp->rs_un.regsave.rs_u.pos.lnum
						    < behind_pos.rs_u.pos.lnum
				    ? (colnr_T)STRLEN(rex.সারি)
				    : behind_pos.rs_u.pos.col)
				- rp->rs_un.regsave.rs_u.pos.col >= limit))
			no = FAIL;
		    অন্যথা যদি (rp->rs_un.regsave.rs_u.pos.col == 0)
		    {
			যদি (rp->rs_un.regsave.rs_u.pos.lnum
					< behind_pos.rs_u.pos.lnum
				|| reg_getline(
					--rp->rs_un.regsave.rs_u.pos.lnum)
								  == ফাঁকা)
			    no = FAIL;
			অন্যথা
			{
			    reg_restore(&rp->rs_un.regsave, &backpos);
			    rp->rs_un.regsave.rs_u.pos.col =
						 (colnr_T)STRLEN(rex.সারি);
			}
		    }
		    অন্যথা
		    {
			যদি (has_mbyte)
			{
			    char_u *সারি =
				  reg_getline(rp->rs_un.regsave.rs_u.pos.lnum);

			    rp->rs_un.regsave.rs_u.pos.col -=
				(*mb_শীর্ষ_off)(সারি, সারি
				    + rp->rs_un.regsave.rs_u.pos.col - 1) + 1;
			}
			অন্যথা
			    --rp->rs_un.regsave.rs_u.pos.col;
		    }
		}
		অন্যথা
		{
		    যদি (rp->rs_un.regsave.rs_u.ptr == rex.সারি)
			no = FAIL;
		    অন্যথা
		    {
			MB_PTR_BACK(rex.সারি, rp->rs_un.regsave.rs_u.ptr);
			যদি (limit > 0 && (long)(behind_pos.rs_u.ptr
				     - rp->rs_un.regsave.rs_u.ptr) > limit)
			    no = FAIL;
		    }
		}
		যদি (no == OK)
		{
		    // Advanced, prepare for finding match again.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    scan = OPERAND(rp->rs_scan) + 4;
		    যদি (status == RA_MATCH)
		    {
			// We did match, so subexpr may have been changed,
			// need to restore them for the next try.
			status = RA_NOMATCH;
			restore_subexpr(((regbehind_T *)rp) - 1);
		    }
		}
		অন্যথা
		{
		    // Can't advance.  For NOBEHIND that's a match.
		    behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		    যদি (rp->rs_no == NOBEHIND)
		    {
			reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
			status = RA_MATCH;
		    }
		    অন্যথা
		    {
			// We do want a proper match.  Need to restore the
			// subexpr if we had a match, because they may have
			// been set.
			যদি (status == RA_MATCH)
			{
			    status = RA_NOMATCH;
			    restore_subexpr(((regbehind_T *)rp) - 1);
			}
		    }
		    regstack_বের_করো(&scan);
		    regstack.ga_len -= আকার(regbehind_T);
		}
	    }
	    বিরতি;

	  ক্ষেত্র RS_STAR_LONG:
	  ক্ষেত্র RS_STAR_SHORT:
	    {
		regstar_T	    *rst = ((regstar_T *)rp) - 1;

		যদি (status == RA_MATCH)
		{
		    regstack_বের_করো(&scan);
		    regstack.ga_len -= আকার(regstar_T);
		    বিরতি;
		}

		// Tried once already, restore input pointers.
		যদি (status != RA_BREAK)
		    reg_restore(&rp->rs_un.regsave, &backpos);

		// Repeat until we found a position where it could match.
		জন্য (;;)
		{
		    যদি (status != RA_BREAK)
		    {
			// Tried first position already, advance.
			যদি (rp->rs_state == RS_STAR_LONG)
			{
			    // Trying for longest match, but couldn't or
			    // didn't match -- back up one char.
			    যদি (--rst->গুনতি < rst->minval)
				বিরতি;
			    যদি (rex.প্রবেশ == rex.সারি)
			    {
				// backup to last char of previous line
				যদি (rex.lnum == 0)
				{
				    status = RA_NOMATCH;
				    বিরতি;
				}
				--rex.lnum;
				rex.সারি = reg_getline(rex.lnum);
				// Just in case regrepeat() didn't count
				// right.
				যদি (rex.সারি == ফাঁকা)
				    বিরতি;
				rex.প্রবেশ = rex.সারি + reg_getline_len(rex.lnum);
				fast_breakcheck();
			    }
			    অন্যথা
				MB_PTR_BACK(rex.সারি, rex.প্রবেশ);
			}
			অন্যথা
			{
			    // Range is backwards, use shortest match first.
			    // Careful: maxval and minval are exchanged!
			    // Couldn't or didn't match: try advancing one
			    // char.
			    যদি (rst->গুনতি == rst->minval
				  || regrepeat(OPERAND(rp->rs_scan), 1L) == 0)
				বিরতি;
			    ++rst->গুনতি;
			}
			যদি (got_int)
			    বিরতি;
		    }
		    অন্যথা
			status = RA_NOMATCH;

		    // If it could match, try it.
		    যদি (rst->nextb == NUL || *rex.প্রবেশ == rst->nextb
					     || *rex.প্রবেশ == rst->nextb_ic)
		    {
			reg_save(&rp->rs_un.regsave, &backpos);
			scan = regnext(rp->rs_scan);
			status = RA_CONT;
			বিরতি;
		    }
		}
		যদি (status != RA_CONT)
		{
		    // Failed.
		    regstack_বের_করো(&scan);
		    regstack.ga_len -= আকার(regstar_T);
		    status = RA_NOMATCH;
		}
	    }
	    বিরতি;
	}

	// If we want to continue the inner loop or didn't pop a state
	// continue matching loop
	যদি (status == RA_CONT || rp == (regitem_T *)
			     ((অক্ষর *)regstack.ga_তথ্য + regstack.ga_len) - 1)
	    বিরতি;

#ifdef FEAT_RELTIME
	যদি (bt_did_time_out(timed_out))
	{
	    status = RA_FAIL;
	    বিরতি;
	}
#endif
    }

    // May need to continue with the inner loop, starting at "scan".
    যদি (status == RA_CONT)
	চালু;

    // If the regstack is empty or something failed we are done.
    যদি (regstack.ga_len == 0 || status == RA_FAIL)
    {
	যদি (scan == ফাঁকা)
	{
	    // We get here only if there's trouble -- normally "case END" is
	    // the terminating point.
	    iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	    printf("Premature EOL\n");
#endif
	}
	ফেরত (status == RA_MATCH);
    }

  } // End of loop until the regstack is empty.

  // NOTREACHED
}

/*
 * regtry - try match of "prog" with at rex.line["col"].
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    স্থির long
regtry(
    bt_regprog_T	*prog,
    colnr_T		col,
    পূর্ণসংখ্যা			*timed_out)	// flag set on timeout or NULL
{
    rex.প্রবেশ = rex.সারি + col;
    rex.need_clear_subexpr = TRUE;
#ifdef FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    rex.need_clear_zsubexpr = (prog->reghasz == REX_SET);
#endif

    যদি (regmatch(prog->program + 1, timed_out) == 0)
	ফেরত 0;

    cleanup_subexpr();
    যদি (REG_MULTI)
    {
	যদি (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].col = col;
	}
	যদি (rex.reg_endpos[0].lnum < 0)
	{
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].col = (পূর্ণসংখ্যা)(rex.প্রবেশ - rex.সারি);
	}
	অন্যথা
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    অন্যথা
    {
	যদি (rex.reg_startp[0] == ফাঁকা)
	    rex.reg_startp[0] = rex.সারি + col;
	যদি (rex.reg_endp[0] == ফাঁকা)
	    rex.reg_endp[0] = rex.প্রবেশ;
    }
#ifdef FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = ফাঁকা;

    যদি (prog->reghasz == REX_SET)
    {
	পূর্ণসংখ্যা		i;

	cleanup_zsubexpr();
	re_extmatch_out = make_extmatch();
	যদি (re_extmatch_out == ফাঁকা)
	    ফেরত 0;
	জন্য (i = 0; i < NSUBEXP; i++)
	{
	    যদি (REG_MULTI)
	    {
		// Only accept single line matches.
		যদি (reg_startzpos[i].lnum >= 0
			&& reg_endzpos[i].lnum == reg_startzpos[i].lnum
			&& reg_endzpos[i].col >= reg_startzpos[i].col)
		    re_extmatch_out->matches[i] =
			vim_strnsave(reg_getline(reg_startzpos[i].lnum)
						       + reg_startzpos[i].col,
				   reg_endzpos[i].col - reg_startzpos[i].col);
	    }
	    অন্যথা
	    {
		যদি (reg_startzp[i] != ফাঁকা && reg_endzp[i] != ফাঁকা)
		    re_extmatch_out->matches[i] =
			    vim_strnsave(reg_startzp[i],
						reg_endzp[i] - reg_startzp[i]);
	    }
	}
    }
#endif
    ফেরত 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    স্থির long
bt_regexec_both(
    char_u	*সারি,
    colnr_T	startcol,	// column to start looking for match
    পূর্ণসংখ্যা		*timed_out)	// flag set on timeout or NULL
{
    bt_regprog_T    *prog;
    char_u	    *s;
    colnr_T	    col = startcol;
    long	    retval = 0L;

    // Create "regstack" and "backpos" if they are not allocated yet.
    // We allocate *_INITIAL amount of bytes first and then set the grow size
    // to much bigger value to avoid many malloc calls in case of deep regular
    // expressions.
    যদি (regstack.ga_তথ্য == ফাঁকা)
    {
	// Use an item size of 1 byte, since we push different things
	// onto the regstack.
	ga_init2(&regstack, 1, REGSTACK_INITIAL);
	(শূন্য)ga_grow(&regstack, REGSTACK_INITIAL);
	regstack.ga_growsize = REGSTACK_INITIAL * 8;
    }

    যদি (backpos.ga_তথ্য == ফাঁকা)
    {
	ga_init2(&backpos, আকার(backpos_T), BACKPOS_INITIAL);
	(শূন্য)ga_grow(&backpos, BACKPOS_INITIAL);
	backpos.ga_growsize = BACKPOS_INITIAL * 8;
    }

    যদি (REG_MULTI)
    {
	prog = (bt_regprog_T *)rex.reg_mmatch->regprog;
	সারি = reg_getline((linenr_T)0);
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    অন্যথা
    {
	prog = (bt_regprog_T *)rex.reg_match->regprog;
	rex.reg_startp = rex.reg_match->startp;
	rex.reg_endp = rex.reg_match->endp;
    }

    // Be paranoid...
    যদি (prog == ফাঁকা || সারি == ফাঁকা)
    {
	iemsg(e_null_argument);
	যাও theend;
    }

    // Check validity of program.
    যদি (prog_magic_wrong())
	যাও theend;

    // If the start column is past the maximum column: no need to try.
    যদি (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	যাও theend;

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    যদি (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    অন্যথা যদি (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    যদি (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    // If there is a "must appear" string, look for it.
    যদি (prog->regmust != ফাঁকা)
    {
	পূর্ণসংখ্যা c;

	যদি (has_mbyte)
	    c = (*mb_ptr2char)(prog->regmust);
	অন্যথা
	    c = *prog->regmust;
	s = সারি + col;

	// This is used very often, esp. for ":global".  Use three versions of
	// the loop to avoid overhead of conditions.
	যদি (!rex.reg_ic && !has_mbyte)
	    যখন ((s = vim_strbyte(s, c)) != ফাঁকা)
	    {
		পূর্ণসংখ্যা len = prog->regmlen;
		যদি (cstrncmp(s, prog->regmust, &len) == 0)
		    বিরতি;		// Found it.
		++s;
	    }
	অন্যথা যদি (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	    যখন ((s = vim_strchr(s, c)) != ফাঁকা)
	    {
		পূর্ণসংখ্যা len = prog->regmlen;
		যদি (cstrncmp(s, prog->regmust, &len) == 0)
		    বিরতি;		// Found it.
		MB_PTR_ADV(s);
	    }
	অন্যথা
	    যখন ((s = cstrchr(s, c)) != ফাঁকা)
	    {
		পূর্ণসংখ্যা len = prog->regmlen;
		যদি (cstrncmp(s, prog->regmust, &len) == 0)
		    বিরতি;		// Found it.
		MB_PTR_ADV(s);
	    }
	যদি (s == ফাঁকা)		// Not present.
	    যাও theend;
    }

    rex.সারি = সারি;
    rex.lnum = 0;
    reg_toolong = FALSE;

    // Simplest case: Anchored match need be tried only once.
    যদি (prog->reganch)
    {
	পূর্ণসংখ্যা	c;

	যদি (has_mbyte)
	    c = (*mb_ptr2char)(rex.সারি + col);
	অন্যথা
	    c = rex.সারি[col];
	যদি (prog->regstart == NUL
		|| prog->regstart == c
		|| (rex.reg_ic
		    && (((enc_utf8 && utf_fold(prog->regstart) == utf_fold(c)))
			|| (c < 255 && prog->regstart < 255 &&
			    MB_TOLOWER(prog->regstart) == MB_TOLOWER(c)))))
	    retval = regtry(prog, col, timed_out);
	অন্যথা
	    retval = 0;
    }
    অন্যথা
    {
	// Messy cases:  unanchored match.
	যখন (!got_int)
	{
	    যদি (prog->regstart != NUL)
	    {
		// Skip until the char we know it must start with.
		// Used often, do some work to avoid call overhead.
		যদি (!rex.reg_ic && !has_mbyte)
		    s = vim_strbyte(rex.সারি + col, prog->regstart);
		অন্যথা
		    s = cstrchr(rex.সারি + col, prog->regstart);
		যদি (s == ফাঁকা)
		{
		    retval = 0;
		    বিরতি;
		}
		col = (পূর্ণসংখ্যা)(s - rex.সারি);
	    }

	    // Check for maximum column to try.
	    যদি (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	    {
		retval = 0;
		বিরতি;
	    }

	    retval = regtry(prog, col, timed_out);
	    যদি (retval > 0)
		বিরতি;

	    // if not currently on the first line, get it again
	    যদি (rex.lnum != 0)
	    {
		rex.lnum = 0;
		rex.সারি = reg_getline((linenr_T)0);
	    }
	    যদি (rex.সারি[col] == NUL)
		বিরতি;
	    যদি (has_mbyte)
		col += (*mb_ptr2len)(rex.সারি + col);
	    অন্যথা
		++col;
#ifdef FEAT_RELTIME
	    যদি (bt_did_time_out(timed_out))
		বিরতি;
#endif
	}
    }

theend:
    // Free "reg_tofree" when it's a bit big.
    // Free regstack and backpos if they are bigger than their initial size.
    যদি (reg_tofreelen > 400)
	VIM_CLEAR(reg_tofree);
    যদি (regstack.ga_maxlen > REGSTACK_INITIAL)
	ga_clear(&regstack);
    যদি (backpos.ga_maxlen > BACKPOS_INITIAL)
	ga_clear(&backpos);

    যদি (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs
	// and \ze are used.
	যদি (REG_MULTI)
	{
	    lpos_T *start = &rex.reg_mmatch->startpos[0];
	    lpos_T *end = &rex.reg_mmatch->endpos[0];

	    যদি (end->lnum < start->lnum
			|| (end->lnum == start->lnum && end->col < start->col))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_mmatch->rmm_matchcol = col;
	}
	অন্যথা
	{
	    যদি (rex.reg_match->endp[0] < rex.reg_match->startp[0])
		rex.reg_match->endp[0] = rex.reg_match->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_match->rm_matchcol = col;
	}
    }

    ফেরত retval;
}

/*
 * Match a regexp against a string.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 * if "line_lbr" is TRUE  consider a "\n" in "line" to be a line break.
 *
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    স্থির পূর্ণসংখ্যা
bt_regexec_nl(
    regmatch_T	*rmp,
    char_u	*সারি,	// string to match against
    colnr_T	col,	// column to start looking for match
    পূর্ণসংখ্যা		সারি_lbr)
{
    rex.reg_match = rmp;
    rex.reg_mmatch = ফাঁকা;
    rex.reg_maxline = 0;
    rex.reg_সারি_lbr = সারি_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = ফাঁকা;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;

    ফেরত bt_regexec_both(সারি, col, ফাঁকা);
}

/*
 * Match a regexp against multiple lines.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 *
 * Return zero if there is no match.  Return number of lines contained in the
 * match otherwise.
 */
    স্থির long
bt_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	col,		// column to start looking for match
    পূর্ণসংখ্যা		*timed_out)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    ফেরত bt_regexec_both(ফাঁকা, col, timed_out);
}

/*
 * Compare a number with the operand of RE_LNUM, RE_COL or RE_VCOL.
 */
    স্থির পূর্ণসংখ্যা
re_অঙ্ক_cmp(long_u val, char_u *scan)
{
    long_u  n = OPERAND_MIN(scan);

    যদি (OPERAND_CMP(scan) == '>')
	ফেরত val > n;
    যদি (OPERAND_CMP(scan) == '<')
	ফেরত val < n;
    ফেরত val == n;
}

#ifdef BT_REGEXP_DUMP

/*
 * regdump - dump a regexp onto stdout in vaguely comprehensible form
 */
    স্থির শূন্য
regdump(char_u *pattern, bt_regprog_T *r)
{
    char_u  *s;
    পূর্ণসংখ্যা	    op = EXACTLY;	// Arbitrary non-END op.
    char_u  *পরবর্তী;
    char_u  *end = ফাঁকা;
    FILE    *f;

# ifdef BT_REGEXP_LOG
    f = fopen("bt_regexp_log.log", "a");
# অন্যথা
    f = stdout;
# endif
    যদি (f == ফাঁকা)
	ফেরত;
    fprintf(f, "-------------------------------------\n\r\nregcomp(%s):\r\n", pattern);

    s = r->program + 1;
    // Loop until we find the END that isn't before a referred next (an END
    // can also appear in a NOMATCH operand).
    যখন ((op != END || s <= end) && s < r->program + r->regsz)
    {
	op = OP(s);
	fprintf(f, "%2d%s", (পূর্ণসংখ্যা)(s - r->program), regprop(s)); // Where, what.
	পরবর্তী = (s + 3 <= r->program + r->regsz) ? regnext(s) : ফাঁকা;
	যদি (পরবর্তী == ফাঁকা)	// Next ptr.
	    fprintf(f, "(0)");
	অন্যথা
	    fprintf(f, "(%d)", (পূর্ণসংখ্যা)((s - r->program) + (পরবর্তী - s)));
	যদি (end < পরবর্তী)
	    end = পরবর্তী;
	যদি (op == BRACE_LIMITS)
	{
	    // Two ints
	    fprintf(f, " minval %ld, maxval %ld", OPERAND_MIN(s), OPERAND_MAX(s));
	    s += 8;
	}
	অন্যথা যদি (op == BEHIND || op == NOBEHIND)
	{
	    // one int
	    fprintf(f, " count %ld", OPERAND_MIN(s));
	    s += 4;
	}
	অন্যথা যদি (op == RE_LNUM || op == RE_COL || op == RE_VCOL)
	{
	    // one int plus comparator
	    fprintf(f, " count %ld", OPERAND_MIN(s));
	    s += 5;
	}
	s += 3;
	যদি (op == MULTIBYTECODE)
	{
	    fprintf(f, " mbc=%d", utf_ptr2char(s));
	    s += utfc_ptr2len(s);
	}
	যদি (op == ANYOF || op == ANYOF + ADD_NL
		|| op == ANYBUT || op == ANYBUT + ADD_NL
		|| op == EXACTLY)
	{
	    // Literal string, where present.
	    fprintf(f, "\nxxxxxxxxx\n");
	    যখন (*s != NUL && s < r->program + r->regsz)
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
    যদি (r->regstart != NUL)
	fprintf(f, "start `%s' 0x%x; ", r->regstart < 256
		? (অক্ষর *)transchar(r->regstart)
		: "multibyte", r->regstart);
    যদি (r->reganch)
	fprintf(f, "anchored; ");
    যদি (r->regmust != ফাঁকা)
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
    স্থির char_u *
regprop(char_u *op)
{
    অক্ষর	    *p;
    স্থির অক্ষর	    buf[50];
    স্থির size_t   buflen = 0;

    STRCPY(buf, ":");
    buflen = 1;

    নির্বাচন ((পূর্ণসংখ্যা) OP(op))
    {
      ক্ষেত্র BOL:
	p = "BOL";
	বিরতি;
      ক্ষেত্র EOL:
	p = "EOL";
	বিরতি;
      ক্ষেত্র RE_BOF:
	p = "BOF";
	বিরতি;
      ক্ষেত্র RE_EOF:
	p = "EOF";
	বিরতি;
      ক্ষেত্র CURSOR:
	p = "CURSOR";
	বিরতি;
      ক্ষেত্র RE_VISUAL:
	p = "RE_VISUAL";
	বিরতি;
      ক্ষেত্র RE_LNUM:
	p = "RE_LNUM";
	বিরতি;
      ক্ষেত্র RE_MARK:
	p = "RE_MARK";
	বিরতি;
      ক্ষেত্র RE_COL:
	p = "RE_COL";
	বিরতি;
      ক্ষেত্র RE_VCOL:
	p = "RE_VCOL";
	বিরতি;
      ক্ষেত্র BOW:
	p = "BOW";
	বিরতি;
      ক্ষেত্র EOW:
	p = "EOW";
	বিরতি;
      ক্ষেত্র ANY:
	p = "ANY";
	বিরতি;
      ক্ষেত্র ANY + ADD_NL:
	p = "ANY+NL";
	বিরতি;
      ক্ষেত্র ANYOF:
	p = "ANYOF";
	বিরতি;
      ক্ষেত্র ANYOF + ADD_NL:
	p = "ANYOF+NL";
	বিরতি;
      ক্ষেত্র ANYBUT:
	p = "ANYBUT";
	বিরতি;
      ক্ষেত্র ANYBUT + ADD_NL:
	p = "ANYBUT+NL";
	বিরতি;
      ক্ষেত্র IDENT:
	p = "IDENT";
	বিরতি;
      ক্ষেত্র IDENT + ADD_NL:
	p = "IDENT+NL";
	বিরতি;
      ক্ষেত্র SIDENT:
	p = "SIDENT";
	বিরতি;
      ক্ষেত্র SIDENT + ADD_NL:
	p = "SIDENT+NL";
	বিরতি;
      ক্ষেত্র KWORD:
	p = "KWORD";
	বিরতি;
      ক্ষেত্র KWORD + ADD_NL:
	p = "KWORD+NL";
	বিরতি;
      ক্ষেত্র SKWORD:
	p = "SKWORD";
	বিরতি;
      ক্ষেত্র SKWORD + ADD_NL:
	p = "SKWORD+NL";
	বিরতি;
      ক্ষেত্র FNAME:
	p = "FNAME";
	বিরতি;
      ক্ষেত্র FNAME + ADD_NL:
	p = "FNAME+NL";
	বিরতি;
      ক্ষেত্র SFNAME:
	p = "SFNAME";
	বিরতি;
      ক্ষেত্র SFNAME + ADD_NL:
	p = "SFNAME+NL";
	বিরতি;
      ক্ষেত্র PRINT:
	p = "PRINT";
	বিরতি;
      ক্ষেত্র PRINT + ADD_NL:
	p = "PRINT+NL";
	বিরতি;
      ক্ষেত্র SPRINT:
	p = "SPRINT";
	বিরতি;
      ক্ষেত্র SPRINT + ADD_NL:
	p = "SPRINT+NL";
	বিরতি;
      ক্ষেত্র WHITE:
	p = "WHITE";
	বিরতি;
      ক্ষেত্র WHITE + ADD_NL:
	p = "WHITE+NL";
	বিরতি;
      ক্ষেত্র NWHITE:
	p = "NWHITE";
	বিরতি;
      ক্ষেত্র NWHITE + ADD_NL:
	p = "NWHITE+NL";
	বিরতি;
      ক্ষেত্র DIGIT:
	p = "DIGIT";
	বিরতি;
      ক্ষেত্র DIGIT + ADD_NL:
	p = "DIGIT+NL";
	বিরতি;
      ক্ষেত্র NDIGIT:
	p = "NDIGIT";
	বিরতি;
      ক্ষেত্র NDIGIT + ADD_NL:
	p = "NDIGIT+NL";
	বিরতি;
      ক্ষেত্র HEX:
	p = "HEX";
	বিরতি;
      ক্ষেত্র HEX + ADD_NL:
	p = "HEX+NL";
	বিরতি;
      ক্ষেত্র NHEX:
	p = "NHEX";
	বিরতি;
      ক্ষেত্র NHEX + ADD_NL:
	p = "NHEX+NL";
	বিরতি;
      ক্ষেত্র OCTAL:
	p = "OCTAL";
	বিরতি;
      ক্ষেত্র OCTAL + ADD_NL:
	p = "OCTAL+NL";
	বিরতি;
      ক্ষেত্র NOCTAL:
	p = "NOCTAL";
	বিরতি;
      ক্ষেত্র NOCTAL + ADD_NL:
	p = "NOCTAL+NL";
	বিরতি;
      ক্ষেত্র WORD:
	p = "WORD";
	বিরতি;
      ক্ষেত্র WORD + ADD_NL:
	p = "WORD+NL";
	বিরতি;
      ক্ষেত্র NWORD:
	p = "NWORD";
	বিরতি;
      ক্ষেত্র NWORD + ADD_NL:
	p = "NWORD+NL";
	বিরতি;
      ক্ষেত্র শীর্ষ_4:
	p = "HEAD";
	বিরতি;
      ক্ষেত্র শীর্ষ_4 + ADD_NL:
	p = "HEAD+NL";
	বিরতি;
      ক্ষেত্র NHEAD:
	p = "NHEAD";
	বিরতি;
      ক্ষেত্র NHEAD + ADD_NL:
	p = "NHEAD+NL";
	বিরতি;
      ক্ষেত্র ALPHA:
	p = "ALPHA";
	বিরতি;
      ক্ষেত্র ALPHA + ADD_NL:
	p = "ALPHA+NL";
	বিরতি;
      ক্ষেত্র NALPHA:
	p = "NALPHA";
	বিরতি;
      ক্ষেত্র NALPHA + ADD_NL:
	p = "NALPHA+NL";
	বিরতি;
      ক্ষেত্র LOWER:
	p = "LOWER";
	বিরতি;
      ক্ষেত্র LOWER + ADD_NL:
	p = "LOWER+NL";
	বিরতি;
      ক্ষেত্র NLOWER:
	p = "NLOWER";
	বিরতি;
      ক্ষেত্র NLOWER + ADD_NL:
	p = "NLOWER+NL";
	বিরতি;
      ক্ষেত্র UPPER:
	p = "UPPER";
	বিরতি;
      ক্ষেত্র UPPER + ADD_NL:
	p = "UPPER+NL";
	বিরতি;
      ক্ষেত্র NUPPER:
	p = "NUPPER";
	বিরতি;
      ক্ষেত্র NUPPER + ADD_NL:
	p = "NUPPER+NL";
	বিরতি;
      ক্ষেত্র BRANCH:
	p = "BRANCH";
	বিরতি;
      ক্ষেত্র EXACTLY:
	p = "EXACTLY";
	বিরতি;
      ক্ষেত্র NOTHING:
	p = "NOTHING";
	বিরতি;
      ক্ষেত্র BACK:
	p = "BACK";
	বিরতি;
      ক্ষেত্র END:
	p = "END";
	বিরতি;
      ক্ষেত্র MOPEN + 0:
	p = "MATCH START";
	বিরতি;
      ক্ষেত্র MOPEN + 1:
      ক্ষেত্র MOPEN + 2:
      ক্ষেত্র MOPEN + 3:
      ক্ষেত্র MOPEN + 4:
      ক্ষেত্র MOPEN + 5:
      ক্ষেত্র MOPEN + 6:
      ক্ষেত্র MOPEN + 7:
      ক্ষেত্র MOPEN + 8:
      ক্ষেত্র MOPEN + 9:
	buflen += vim_snprintf(buf + buflen, আকার(buf) - buflen, "MOPEN%d", OP(op) - MOPEN);
	p = ফাঁকা;
	বিরতি;
      ক্ষেত্র MCLOSE + 0:
	p = "MATCH END";
	বিরতি;
      ক্ষেত্র MCLOSE + 1:
      ক্ষেত্র MCLOSE + 2:
      ক্ষেত্র MCLOSE + 3:
      ক্ষেত্র MCLOSE + 4:
      ক্ষেত্র MCLOSE + 5:
      ক্ষেত্র MCLOSE + 6:
      ক্ষেত্র MCLOSE + 7:
      ক্ষেত্র MCLOSE + 8:
      ক্ষেত্র MCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, আকার(buf) - buflen, "MCLOSE%d", OP(op) - MCLOSE);
	p = ফাঁকা;
	বিরতি;
      ক্ষেত্র BACKREF + 1:
      ক্ষেত্র BACKREF + 2:
      ক্ষেত্র BACKREF + 3:
      ক্ষেত্র BACKREF + 4:
      ক্ষেত্র BACKREF + 5:
      ক্ষেত্র BACKREF + 6:
      ক্ষেত্র BACKREF + 7:
      ক্ষেত্র BACKREF + 8:
      ক্ষেত্র BACKREF + 9:
	buflen += vim_snprintf(buf + buflen, আকার(buf) - buflen, "BACKREF%d", OP(op) - BACKREF);
	p = ফাঁকা;
	বিরতি;
      ক্ষেত্র NOPEN:
	p = "NOPEN";
	বিরতি;
      ক্ষেত্র NCLOSE:
	p = "NCLOSE";
	বিরতি;
# ifdef FEAT_SYN_HL
      ক্ষেত্র ZOPEN + 1:
      ক্ষেত্র ZOPEN + 2:
      ক্ষেত্র ZOPEN + 3:
      ক্ষেত্র ZOPEN + 4:
      ক্ষেত্র ZOPEN + 5:
      ক্ষেত্র ZOPEN + 6:
      ক্ষেত্র ZOPEN + 7:
      ক্ষেত্র ZOPEN + 8:
      ক্ষেত্র ZOPEN + 9:
	buflen += vim_snprintf(buf + buflen, আকার(buf) - buflen, "ZOPEN%d", OP(op) - ZOPEN);
	p = ফাঁকা;
	বিরতি;
      ক্ষেত্র ZCLOSE + 1:
      ক্ষেত্র ZCLOSE + 2:
      ক্ষেত্র ZCLOSE + 3:
      ক্ষেত্র ZCLOSE + 4:
      ক্ষেত্র ZCLOSE + 5:
      ক্ষেত্র ZCLOSE + 6:
      ক্ষেত্র ZCLOSE + 7:
      ক্ষেত্র ZCLOSE + 8:
      ক্ষেত্র ZCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, আকার(buf) - buflen, "ZCLOSE%d", OP(op) - ZCLOSE);
	p = ফাঁকা;
	বিরতি;
      ক্ষেত্র ZREF + 1:
      ক্ষেত্র ZREF + 2:
      ক্ষেত্র ZREF + 3:
      ক্ষেত্র ZREF + 4:
      ক্ষেত্র ZREF + 5:
      ক্ষেত্র ZREF + 6:
      ক্ষেত্র ZREF + 7:
      ক্ষেত্র ZREF + 8:
      ক্ষেত্র ZREF + 9:
	buflen += vim_snprintf(buf + buflen, আকার(buf) - buflen, "ZREF%d", OP(op) - ZREF);
	p = ফাঁকা;
	বিরতি;
# endif
      ক্ষেত্র STAR:
	p = "STAR";
	বিরতি;
      ক্ষেত্র PLUS:
	p = "PLUS";
	বিরতি;
      ক্ষেত্র NOMATCH:
	p = "NOMATCH";
	বিরতি;
      ক্ষেত্র MATCH:
	p = "MATCH";
	বিরতি;
      ক্ষেত্র BEHIND:
	p = "BEHIND";
	বিরতি;
      ক্ষেত্র NOBEHIND:
	p = "NOBEHIND";
	বিরতি;
      ক্ষেত্র SUBPAT:
	p = "SUBPAT";
	বিরতি;
      ক্ষেত্র BRACE_LIMITS:
	p = "BRACE_LIMITS";
	বিরতি;
      ক্ষেত্র BRACE_SIMPLE:
	p = "BRACE_SIMPLE";
	বিরতি;
      ক্ষেত্র BRACE_COMPLEX + 0:
      ক্ষেত্র BRACE_COMPLEX + 1:
      ক্ষেত্র BRACE_COMPLEX + 2:
      ক্ষেত্র BRACE_COMPLEX + 3:
      ক্ষেত্র BRACE_COMPLEX + 4:
      ক্ষেত্র BRACE_COMPLEX + 5:
      ক্ষেত্র BRACE_COMPLEX + 6:
      ক্ষেত্র BRACE_COMPLEX + 7:
      ক্ষেত্র BRACE_COMPLEX + 8:
      ক্ষেত্র BRACE_COMPLEX + 9:
	buflen += vim_snprintf(buf + buflen, আকার(buf) - buflen, "BRACE_COMPLEX%d", OP(op) - BRACE_COMPLEX);
	p = ফাঁকা;
	বিরতি;
      ক্ষেত্র MULTIBYTECODE:
	p = "MULTIBYTECODE";
	বিরতি;
      ক্ষেত্র NEWL:
	p = "NEWL";
	বিরতি;
      পূর্বনির্ধারিত:
	buflen += vim_snprintf(buf + buflen, আকার(buf) - buflen, "corrupt %d", OP(op));
	p = ফাঁকা;
	বিরতি;
    }
    যদি (p != ফাঁকা)
	vim_strncpy((char_u *)buf + buflen, (char_u *)p, আকার(buf) - buflen - 1);
    ফেরত (char_u *)buf;
}
#endif	    // DEBUG
