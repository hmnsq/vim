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

ایستا صحیح	رقم_complex_braces; // Complex \{...} count
ایستا char_u	*regcode;	// Code-emit pointer, or JUST_CALC_SIZE
ایستا long	regsize;	// Code size.
ایستا صحیح	reg_toolong;	// TRUE when offset out of range
ایستا صحیح	bt_reg_parse_depth;	// nesting depth in reg()
ایستا char_u	had_endbrace[NSUBEXP];	// flags, TRUE if end of () found
ایستا long	brace_min[10];	// Minimums for complex brace repeats
ایستا long	brace_max[10];	// Maximums for complex brace repeats
ایستا صحیح	brace_شمار[10]; // Current counts for complex brace repeats
ایستا صحیح	one_exactly = FALSE;	// only do one char for EXACTLY

// When making changes to classchars also change nfa_classcodes.
ایستا char_u	*classchars = (char_u *)".iIkKfFpPsSdDxXoOwWhHaAlLuU";
ایستا ثابت صحیح classcodes[] = {
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
    ایستا صحیح
bt_recognize_char_class(char_u *start, char_u *end)
{
    صحیح		newl;
    صحیح		config = get_char_class_bits(start, end, &newl);

    // A newline would need the ADD_NL variant, don't bother with it here.
    اگر (config < 0 || newl)
	بازگشت 0;

	// The old engine has no case-insensitive class opcode, so [a-z] and [A-Z]
	// are left as collections.  The classes below are case-independent.
    انتخاب (config)
    {
	حالت CLASS_o9:
	    بازگشت DIGIT;
	حالت CLASS_not | CLASS_o9:
	    بازگشت NDIGIT;
	حالت CLASS_af | CLASS_AF | CLASS_o9:
	    بازگشت HEX;
	حالت CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    بازگشت NHEX;
	حالت CLASS_o7:
	    بازگشت OCTAL;
	حالت CLASS_not | CLASS_o7:
	    بازگشت NOCTAL;
	حالت CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    بازگشت WORD;
	حالت CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    بازگشت NWORD;
	حالت CLASS_az | CLASS_AZ | CLASS_underscore:
	    بازگشت HEAD;
	حالت CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    بازگشت NHEAD;
	حالت CLASS_az | CLASS_AZ:
	    بازگشت ALPHA;
	حالت CLASS_not | CLASS_az | CLASS_AZ:
	    بازگشت NALPHA;
    }
    بازگشت 0;
}

/*
 * When regcode is set to this value, code is not emitted and size is computed
 * instead.
 */
#define JUST_CALC_SIZE	((char_u *) -1)

// Values for rs_state in regitem_T.
تعریفنوع شمارش regstate_E
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
تعریفنوع ساختار
{
    اجتماع
    {
	char_u	*ptr;	// rex.input pointer, for single-line regexp
	lpos_T	pos;	// rex.input pos, for multi-line regexp
    } rs_u;
    صحیح		rs_len;
} regsave_T;

// struct to save start/end pointer/position in for \(\)
تعریفنوع ساختار
{
    اجتماع
    {
	char_u	*ptr;
	lpos_T	pos;
    } se_u;
} save_se_T;

// used for BEHIND and NOBEHIND matching
تعریفنوع ساختار regbehind_S
{
    regsave_T	save_after;
    regsave_T	save_behind;
    صحیح		save_need_clear_subexpr;
    save_se_T   save_start[NSUBEXP];
    save_se_T   save_end[NSUBEXP];
} regbehind_T;

/*
 * When there are alternatives a regstate_T is put on the regstack to remember
 * what we are doing.
 * Before it may be another type of item, depending on rs_state, to remember
 * more things.
 */
تعریفنوع ساختار regitem_S
{
    regstate_T	rs_state;	// what we are doing, one of RS_ above
    short	rs_no;		// submatch nr or BEHIND/NOBEHIND
    char_u	*rs_scan;	// current node in program
    اجتماع
    {
	save_se_T  sesave;
	regsave_T  regsave;
    } rs_un;			// room for saving rex.input
} regitem_T;


// used for STAR, PLUS and BRACE_SIMPLE matching
تعریفنوع ساختار regstar_S
{
    صحیح		nextb;		// next byte
    صحیح		nextb_ic;	// next byte reverse case
    long	شمار;
    long	minval;
    long	maxval;
} regstar_T;

// used to store input position when a BACK was encountered, so that we now if
// we made any progress since the last time.
تعریفنوع ساختار backpos_S
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
ایستا garray_T	regstack = {0, 0, 0, 0, پوچ};
ایستا garray_T	backpos = {0, 0, 0, 0, پوچ};

ایستا regsave_T behind_pos;

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
#define OP(p)		((صحیح)*(p))
#define NEXT(p)		(((*((p) + 1) & 0377) << 8) + (*((p) + 2) & 0377))
#define OPERAND(p)	((p) + 3)
// Obtain an operand that was stored as four bytes, MSB first.
#define OPERAND_MIN(p)	(((long)(p)[3] << 24) + ((long)(p)[4] << 16) \
			+ ((long)(p)[5] << 8) + (long)(p)[6])
// Obtain a second operand stored as four bytes.
#define OPERAND_MAX(p)	OPERAND_MIN((p) + 4)
// Obtain a second single-byte operand stored after a four bytes operand.
#define OPERAND_CMP(p)	(p)[7]

ایستا char_u *reg(صحیح paren, صحیح *flagp);

#ifdef BT_REGEXP_DUMP
ایستا تهی	regdump(char_u *, bt_regprog_T *);
#endif

ایستا صحیح	re_رقم_cmp(long_u val, char_u *scan);

#ifdef DEBUG
ایستا char_u	*regprop(char_u *);

ایستا صحیح	regnarrate = 0;
#endif


/*
 * Setup to parse the regexp.  Used once to get the length and once to do it.
 */
    ایستا تهی
regcomp_start(
    char_u	*عبارت,
    صحیح		re_flags)	    // see vim_regcomp()
{
    initchr(عبارت);
    اگر (re_flags & RE_MAGIC)
	reg_magic = MAGIC_ON;
    وگرنه
	reg_magic = MAGIC_OFF;
    reg_string = (re_flags & RE_STRING);
    reg_strict = (re_flags & RE_STRICT);
    get_cpo_flags();

    رقم_complex_braces = 0;
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
#اگر defined(FEAT_SYN_HL)
    had_eol = FALSE;
#endif
}

/*
 * Return TRUE if MULTIBYTECODE should be used instead of EXACTLY for
 * character "c".
 */
    ایستا صحیح
use_multibytecode(صحیح c)
{
    بازگشت has_mbyte && (*mb_char2len)(c) > 1
		     && (re_multi_type(peekchr()) != NOT_MULTI
			     || (enc_utf8 && utf_iscomposing(c)));
}

/*
 * Emit (if appropriate) a byte of code
 */
    ایستا تهی
regc(صحیح b)
{
    اگر (regcode == JUST_CALC_SIZE)
	regsize++;
    وگرنه
	*regcode++ = b;
}

/*
 * Emit (if appropriate) a multi-byte character of code
 */
    ایستا تهی
regmbc(صحیح c)
{
    اگر (!has_mbyte && c > 0xff)
	بازگشت;
    اگر (regcode == JUST_CALC_SIZE)
	regsize += (*mb_char2len)(c);
    وگرنه
	regcode += (*mb_char2bytes)(c, regcode);
}


/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * NOTE: When changing this function, also change nfa_emit_equi_class()
 */
    ایستا تهی
reg_equi_class(صحیح c)
{
    اگر (enc_utf8 || STRCMP(p_enc, "latin1") == 0
					 || STRCMP(p_enc, "iso-8859-15") == 0)
    {
	انتخاب (c)
	{
	    // Do not use '\300' style, it results in a negative number.
	    حالت 'A': حالت 0xc0: حالت 0xc1: حالت 0xc2: حالت 0xc3: حالت 0xc4:
	    حالت 0xc5: حالت 0x100: حالت 0x102: حالت 0x104: حالت 0x1cd:
	    حالت 0x1de: حالت 0x1e0: حالت 0x1fa: حالت 0x202: حالت 0x226:
	    حالت 0x23a: حالت 0x1e00: حالت 0x1ea0: حالت 0x1ea2: حالت 0x1ea4:
	    حالت 0x1ea6: حالت 0x1ea8: حالت 0x1eaa: حالت 0x1eac: حالت 0x1eae:
	    حالت 0x1eb0: حالت 0x1eb2: حالت 0x1eb4: حالت 0x1eb6:
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
		      بازگشت;
	    حالت 'B': حالت 0x181: حالت 0x243: حالت 0x1e02:
	    حالت 0x1e04: حالت 0x1e06:
		      regmbc('B');
		      regmbc(0x181); regmbc(0x243); regmbc(0x1e02);
		      regmbc(0x1e04); regmbc(0x1e06);
		      بازگشت;
	    حالت 'C': حالت 0xc7:
	    حالت 0x106: حالت 0x108: حالت 0x10a: حالت 0x10c: حالت 0x187:
	    حالت 0x23b: حالت 0x1e08: حالت 0xa792:
		      regmbc('C'); regmbc(0xc7);
		      regmbc(0x106); regmbc(0x108); regmbc(0x10a);
		      regmbc(0x10c); regmbc(0x187); regmbc(0x23b);
		      regmbc(0x1e08); regmbc(0xa792);
		      بازگشت;
	    حالت 'D': حالت 0x10e: حالت 0x110: حالت 0x18a:
	    حالت 0x1e0a: حالت 0x1e0c: حالت 0x1e0e: حالت 0x1e10:
	    حالت 0x1e12:
		      regmbc('D'); regmbc(0x10e); regmbc(0x110);
		      regmbc(0x18a); regmbc(0x1e0a); regmbc(0x1e0c);
		      regmbc(0x1e0e); regmbc(0x1e10); regmbc(0x1e12);
		      بازگشت;
	    حالت 'E': حالت 0xc8: حالت 0xc9: حالت 0xca: حالت 0xcb:
	    حالت 0x112: حالت 0x114: حالت 0x116: حالت 0x118: حالت 0x11a:
	    حالت 0x204: حالت 0x206: حالت 0x228: حالت 0x246: حالت 0x1e14:
	    حالت 0x1e16: حالت 0x1e18: حالت 0x1e1a: حالت 0x1e1c:
	    حالت 0x1eb8: حالت 0x1eba: حالت 0x1ebc: حالت 0x1ebe:
	    حالت 0x1ec0: حالت 0x1ec2: حالت 0x1ec4: حالت 0x1ec6:
		      regmbc('E'); regmbc(0xc8); regmbc(0xc9);
		      regmbc(0xca); regmbc(0xcb); regmbc(0x112);
		      regmbc(0x114); regmbc(0x116); regmbc(0x118);
		      regmbc(0x11a); regmbc(0x204); regmbc(0x206);
		      regmbc(0x228); regmbc(0x246); regmbc(0x1e14);
		      regmbc(0x1e16); regmbc(0x1e18); regmbc(0x1e1a);
		      regmbc(0x1e1c); regmbc(0x1eb8); regmbc(0x1eba);
		      regmbc(0x1ebc); regmbc(0x1ebe); regmbc(0x1ec0);
		      regmbc(0x1ec2); regmbc(0x1ec4); regmbc(0x1ec6);
		      بازگشت;
	    حالت 'F': حالت 0x191: حالت 0x1e1e: حالت 0xa798:
		      regmbc('F'); regmbc(0x191); regmbc(0x1e1e);
		      regmbc(0xa798);
		      بازگشت;
	    حالت 'G': حالت 0x11c: حالت 0x11e: حالت 0x120:
	    حالت 0x122: حالت 0x193: حالت 0x1e4: حالت 0x1e6:
	    حالت 0x1f4: حالت 0x1e20: حالت 0xa7a0:
		      regmbc('G'); regmbc(0x11c); regmbc(0x11e);
		      regmbc(0x120); regmbc(0x122); regmbc(0x193);
		      regmbc(0x1e4); regmbc(0x1e6); regmbc(0x1f4);
		      regmbc(0x1e20); regmbc(0xa7a0);
		      بازگشت;
	    حالت 'H': حالت 0x124: حالت 0x126: حالت 0x21e:
	    حالت 0x1e22: حالت 0x1e24: حالت 0x1e26:
	    حالت 0x1e28: حالت 0x1e2a: حالت 0x2c67:
		      regmbc('H'); regmbc(0x124); regmbc(0x126);
		      regmbc(0x21e); regmbc(0x1e22); regmbc(0x1e24);
		      regmbc(0x1e26); regmbc(0x1e28); regmbc(0x1e2a);
		      regmbc(0x2c67);
		      بازگشت;
	    حالت 'I': حالت 0xcc: حالت 0xcd: حالت 0xce: حالت 0xcf:
	    حالت 0x128: حالت 0x12a: حالت 0x12c: حالت 0x12e:
	    حالت 0x130: حالت 0x197: حالت 0x1cf: حالت 0x208:
	    حالت 0x20a: حالت 0x1e2c: حالت 0x1e2e: حالت 0x1ec8:
	    حالت 0x1eca:
		      regmbc('I'); regmbc(0xcc); regmbc(0xcd);
		      regmbc(0xce); regmbc(0xcf); regmbc(0x128);
		      regmbc(0x12a); regmbc(0x12c); regmbc(0x12e);
		      regmbc(0x130); regmbc(0x197); regmbc(0x1cf);
		      regmbc(0x208); regmbc(0x20a); regmbc(0x1e2c);
		      regmbc(0x1e2e); regmbc(0x1ec8); regmbc(0x1eca);
		      بازگشت;
	    حالت 'J': حالت 0x134: حالت 0x248:
		      regmbc('J'); regmbc(0x134); regmbc(0x248);
		      بازگشت;
	    حالت 'K': حالت 0x136: حالت 0x198: حالت 0x1e8: حالت 0x1e30:
	    حالت 0x1e32: حالت 0x1e34: حالت 0x2c69: حالت 0xa740:
		      regmbc('K'); regmbc(0x136); regmbc(0x198);
		      regmbc(0x1e8); regmbc(0x1e30); regmbc(0x1e32);
		      regmbc(0x1e34); regmbc(0x2c69); regmbc(0xa740);
		      بازگشت;
	    حالت 'L': حالت 0x139: حالت 0x13b: حالت 0x13d: حالت 0x13f:
	    حالت 0x141: حالت 0x23d: حالت 0x1e36: حالت 0x1e38:
	    حالت 0x1e3a: حالت 0x1e3c: حالت 0x2c60:
		      regmbc('L'); regmbc(0x139); regmbc(0x13b);
		      regmbc(0x13d); regmbc(0x13f); regmbc(0x141);
		      regmbc(0x23d); regmbc(0x1e36); regmbc(0x1e38);
		      regmbc(0x1e3a); regmbc(0x1e3c); regmbc(0x2c60);
		      بازگشت;
	    حالت 'M': حالت 0x1e3e: حالت 0x1e40: حالت 0x1e42:
		      regmbc('M'); regmbc(0x1e3e); regmbc(0x1e40);
		      regmbc(0x1e42);
		      بازگشت;
	    حالت 'N': حالت 0xd1:
	    حالت 0x143: حالت 0x145: حالت 0x147: حالت 0x1f8:
	    حالت 0x1e44: حالت 0x1e46: حالت 0x1e48: حالت 0x1e4a:
	    حالت 0xa7a4:
		      regmbc('N'); regmbc(0xd1);
		      regmbc(0x143); regmbc(0x145); regmbc(0x147);
		      regmbc(0x1f8); regmbc(0x1e44); regmbc(0x1e46);
		      regmbc(0x1e48); regmbc(0x1e4a); regmbc(0xa7a4);
		      بازگشت;
	    حالت 'O': حالت 0xd2: حالت 0xd3: حالت 0xd4: حالت 0xd5: حالت 0xd6:
	    حالت 0xd8: حالت 0x14c: حالت 0x14e: حالت 0x150: حالت 0x19f:
	    حالت 0x1a0: حالت 0x1d1: حالت 0x1ea: حالت 0x1ec: حالت 0x1fe:
	    حالت 0x20c: حالت 0x20e: حالت 0x22a: حالت 0x22c: حالت 0x22e:
	    حالت 0x230: حالت 0x1e4c: حالت 0x1e4e: حالت 0x1e50: حالت 0x1e52:
	    حالت 0x1ecc: حالت 0x1ece: حالت 0x1ed0: حالت 0x1ed2: حالت 0x1ed4:
	    حالت 0x1ed6: حالت 0x1ed8: حالت 0x1eda: حالت 0x1edc: حالت 0x1ede:
	    حالت 0x1ee0: حالت 0x1ee2:
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
		      بازگشت;
	    حالت 'P': حالت 0x1a4: حالت 0x1e54: حالت 0x1e56: حالت 0x2c63:
		      regmbc('P'); regmbc(0x1a4); regmbc(0x1e54);
		      regmbc(0x1e56); regmbc(0x2c63);
		      بازگشت;
	    حالت 'Q': حالت 0x24a:
		      regmbc('Q'); regmbc(0x24a);
		      بازگشت;
	    حالت 'R': حالت 0x154: حالت 0x156: حالت 0x158: حالت 0x210:
	    حالت 0x212: حالت 0x24c: حالت 0x1e58: حالت 0x1e5a:
	    حالت 0x1e5c: حالت 0x1e5e: حالت 0x2c64: حالت 0xa7a6:
		      regmbc('R'); regmbc(0x154); regmbc(0x156);
		      regmbc(0x210); regmbc(0x212); regmbc(0x158);
		      regmbc(0x24c); regmbc(0x1e58); regmbc(0x1e5a);
		      regmbc(0x1e5c); regmbc(0x1e5e); regmbc(0x2c64);
		      regmbc(0xa7a6);
		      بازگشت;
	    حالت 'S': حالت 0x15a: حالت 0x15c: حالت 0x15e: حالت 0x160:
	    حالت 0x218: حالت 0x1e60: حالت 0x1e62: حالت 0x1e64:
	    حالت 0x1e66: حالت 0x1e68: حالت 0x2c7e: حالت 0xa7a8:
		      regmbc('S'); regmbc(0x15a); regmbc(0x15c);
		      regmbc(0x15e); regmbc(0x160); regmbc(0x218);
		      regmbc(0x1e60); regmbc(0x1e62); regmbc(0x1e64);
		      regmbc(0x1e66); regmbc(0x1e68); regmbc(0x2c7e);
			  regmbc(0xa7a8);
		      بازگشت;
	    حالت 'T': حالت 0x162: حالت 0x164: حالت 0x166: حالت 0x1ac:
	    حالت 0x1ae: حالت 0x21a: حالت 0x23e: حالت 0x1e6a: حالت 0x1e6c:
	    حالت 0x1e6e: حالت 0x1e70:
		      regmbc('T'); regmbc(0x162); regmbc(0x164);
		      regmbc(0x166); regmbc(0x1ac); regmbc(0x23e);
		      regmbc(0x1ae); regmbc(0x21a); regmbc(0x1e6a);
		      regmbc(0x1e6c); regmbc(0x1e6e); regmbc(0x1e70);
		      بازگشت;
	    حالت 'U': حالت 0xd9: حالت 0xda: حالت 0xdb: حالت 0xdc:
	    حالت 0x168: حالت 0x16a: حالت 0x16c: حالت 0x16e:
	    حالت 0x170: حالت 0x172: حالت 0x1af: حالت 0x1d3:
	    حالت 0x1d5: حالت 0x1d7: حالت 0x1d9: حالت 0x1db:
	    حالت 0x214: حالت 0x216: حالت 0x244: حالت 0x1e72:
	    حالت 0x1e74: حالت 0x1e76: حالت 0x1e78: حالت 0x1e7a:
	    حالت 0x1ee4: حالت 0x1ee6: حالت 0x1ee8: حالت 0x1eea:
	    حالت 0x1eec: حالت 0x1eee: حالت 0x1ef0:
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
		      بازگشت;
	    حالت 'V': حالت 0x1b2: حالت 0x1e7c: حالت 0x1e7e:
		      regmbc('V'); regmbc(0x1b2); regmbc(0x1e7c);
		      regmbc(0x1e7e);
		      بازگشت;
	    حالت 'W': حالت 0x174: حالت 0x1e80: حالت 0x1e82:
	    حالت 0x1e84: حالت 0x1e86: حالت 0x1e88:
		      regmbc('W'); regmbc(0x174); regmbc(0x1e80);
		      regmbc(0x1e82); regmbc(0x1e84); regmbc(0x1e86);
		      regmbc(0x1e88);
		      بازگشت;
	    حالت 'X': حالت 0x1e8a: حالت 0x1e8c:
		      regmbc('X'); regmbc(0x1e8a); regmbc(0x1e8c);
		      بازگشت;
	    حالت 'Y': حالت 0xdd:
	    حالت 0x176: حالت 0x178: حالت 0x1b3: حالت 0x232: حالت 0x24e:
	    حالت 0x1e8e: حالت 0x1ef2: حالت 0x1ef6: حالت 0x1ef4: حالت 0x1ef8:
		      regmbc('Y'); regmbc(0xdd); regmbc(0x176);
		      regmbc(0x178); regmbc(0x1b3); regmbc(0x232);
		      regmbc(0x24e); regmbc(0x1e8e); regmbc(0x1ef2);
		      regmbc(0x1ef4); regmbc(0x1ef6); regmbc(0x1ef8);
		      بازگشت;
	    حالت 'Z': حالت 0x179: حالت 0x17b: حالت 0x17d: حالت 0x1b5:
	    حالت 0x1e90: حالت 0x1e92: حالت 0x1e94: حالت 0x2c6b:
		      regmbc('Z'); regmbc(0x179); regmbc(0x17b);
		      regmbc(0x17d); regmbc(0x1b5); regmbc(0x1e90);
		      regmbc(0x1e92); regmbc(0x1e94); regmbc(0x2c6b);
		      بازگشت;
	    حالت 'a': حالت 0xe0: حالت 0xe1: حالت 0xe2:
	    حالت 0xe3: حالت 0xe4: حالت 0xe5: حالت 0x101: حالت 0x103:
	    حالت 0x105: حالت 0x1ce: حالت 0x1df: حالت 0x1e1: حالت 0x1fb:
	    حالت 0x201: حالت 0x203: حالت 0x227: حالت 0x1d8f: حالت 0x1e01:
	    حالت 0x1e9a: حالت 0x1ea1: حالت 0x1ea3: حالت 0x1ea5:
	    حالت 0x1ea7: حالت 0x1ea9: حالت 0x1eab: حالت 0x1ead:
	    حالت 0x1eaf: حالت 0x1eb1: حالت 0x1eb3: حالت 0x1eb5:
	    حالت 0x1eb7: حالت 0x2c65:
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
		      بازگشت;
	    حالت 'b': حالت 0x180: حالت 0x253: حالت 0x1d6c: حالت 0x1d80:
	    حالت 0x1e03: حالت 0x1e05: حالت 0x1e07:
		      regmbc('b');
		      regmbc(0x180); regmbc(0x253); regmbc(0x1d6c);
		      regmbc(0x1d80); regmbc(0x1e03); regmbc(0x1e05);
		      regmbc(0x1e07);
		      بازگشت;
	    حالت 'c': حالت 0xe7:
	    حالت 0x107: حالت 0x109: حالت 0x10b: حالت 0x10d: حالت 0x188:
	    حالت 0x23c: حالت 0x1e09: حالت 0xa793: حالت 0xa794:
		      regmbc('c'); regmbc(0xe7); regmbc(0x107);
		      regmbc(0x109); regmbc(0x10b); regmbc(0x10d);
		      regmbc(0x188); regmbc(0x23c); regmbc(0x1e09);
		      regmbc(0xa793); regmbc(0xa794);
		      بازگشت;
	    حالت 'd': حالت 0x10f: حالت 0x111: حالت 0x257: حالت 0x1d6d:
	    حالت 0x1d81: حالت 0x1d91: حالت 0x1e0b: حالت 0x1e0d:
	    حالت 0x1e0f: حالت 0x1e11: حالت 0x1e13:
		      regmbc('d'); regmbc(0x10f); regmbc(0x111);
		      regmbc(0x257); regmbc(0x1d6d); regmbc(0x1d81);
		      regmbc(0x1d91); regmbc(0x1e0b); regmbc(0x1e0d);
		      regmbc(0x1e0f); regmbc(0x1e11); regmbc(0x1e13);
		      بازگشت;
	    حالت 'e': حالت 0xe8: حالت 0xe9: حالت 0xea: حالت 0xeb:
	    حالت 0x113: حالت 0x115: حالت 0x117: حالت 0x119:
	    حالت 0x11b: حالت 0x205: حالت 0x207: حالت 0x229:
	    حالت 0x247: حالت 0x1d92: حالت 0x1e15: حالت 0x1e17:
	    حالت 0x1e19: حالت 0x1e1b: حالت 0x1eb9: حالت 0x1ebb:
	    حالت 0x1e1d: حالت 0x1ebd: حالت 0x1ebf: حالت 0x1ec1:
	    حالت 0x1ec3: حالت 0x1ec5: حالت 0x1ec7:
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
		      بازگشت;
	    حالت 'f': حالت 0x192: حالت 0x1d6e: حالت 0x1d82:
	    حالت 0x1e1f: حالت 0xa799:
		     regmbc('f'); regmbc(0x192); regmbc(0x1d6e);
		     regmbc(0x1d82); regmbc(0x1e1f); regmbc(0xa799);
		     بازگشت;
	    حالت 'g': حالت 0x11d: حالت 0x11f: حالت 0x121: حالت 0x123:
	    حالت 0x1e5: حالت 0x1e7: حالت 0x260: حالت 0x1f5: حالت 0x1d83:
	    حالت 0x1e21: حالت 0xa7a1:
		      regmbc('g'); regmbc(0x11d); regmbc(0x11f);
		      regmbc(0x121); regmbc(0x123); regmbc(0x1e5);
		      regmbc(0x1e7); regmbc(0x1f5); regmbc(0x260);
		      regmbc(0x1d83); regmbc(0x1e21); regmbc(0xa7a1);
		      بازگشت;
	    حالت 'h': حالت 0x125: حالت 0x127: حالت 0x21f: حالت 0x1e23:
	    حالت 0x1e25: حالت 0x1e27: حالت 0x1e29: حالت 0x1e2b:
	    حالت 0x1e96: حالت 0x2c68: حالت 0xa795:
		      regmbc('h'); regmbc(0x125); regmbc(0x127);
		      regmbc(0x21f); regmbc(0x1e23); regmbc(0x1e25);
		      regmbc(0x1e27); regmbc(0x1e29); regmbc(0x1e2b);
		      regmbc(0x1e96); regmbc(0x2c68); regmbc(0xa795);
		      بازگشت;
	    حالت 'i': حالت 0xec: حالت 0xed: حالت 0xee: حالت 0xef:
	    حالت 0x129: حالت 0x12b: حالت 0x12d: حالت 0x12f:
	    حالت 0x1d0: حالت 0x209: حالت 0x20b: حالت 0x268:
	    حالت 0x1d96: حالت 0x1e2d: حالت 0x1e2f: حالت 0x1ec9:
	    حالت 0x1ecb:
		      regmbc('i'); regmbc(0xec); regmbc(0xed);
		      regmbc(0xee); regmbc(0xef); regmbc(0x129);
		      regmbc(0x12b); regmbc(0x12d); regmbc(0x12f);
		      regmbc(0x1d0); regmbc(0x209); regmbc(0x20b);
		      regmbc(0x268); regmbc(0x1d96); regmbc(0x1e2d);
		      regmbc(0x1e2f); regmbc(0x1ec9); regmbc(0x1ecb);
		      بازگشت;
	    حالت 'j': حالت 0x135: حالت 0x1f0: حالت 0x249:
		      regmbc('j'); regmbc(0x135); regmbc(0x1f0);
		      regmbc(0x249);
		      بازگشت;
	    حالت 'k': حالت 0x137: حالت 0x199: حالت 0x1e9:
	    حالت 0x1d84: حالت 0x1e31: حالت 0x1e33: حالت 0x1e35:
	    حالت 0x2c6a: حالت 0xa741:
		      regmbc('k'); regmbc(0x137); regmbc(0x199);
		      regmbc(0x1e9); regmbc(0x1d84); regmbc(0x1e31);
		      regmbc(0x1e33); regmbc(0x1e35); regmbc(0x2c6a);
		      regmbc(0xa741);
		      بازگشت;
	    حالت 'l': حالت 0x13a: حالت 0x13c: حالت 0x13e:
	    حالت 0x140: حالت 0x142: حالت 0x19a: حالت 0x1e37:
	    حالت 0x1e39: حالت 0x1e3b: حالت 0x1e3d: حالت 0x2c61:
		      regmbc('l'); regmbc(0x13a); regmbc(0x13c);
		      regmbc(0x13e); regmbc(0x140); regmbc(0x142);
		      regmbc(0x19a); regmbc(0x1e37); regmbc(0x1e39);
		      regmbc(0x1e3b); regmbc(0x1e3d); regmbc(0x2c61);
		      بازگشت;
	    حالت 'm': حالت 0x1d6f: حالت 0x1e3f: حالت 0x1e41: حالت 0x1e43:
		      regmbc('m'); regmbc(0x1d6f); regmbc(0x1e3f);
		      regmbc(0x1e41); regmbc(0x1e43);
		      بازگشت;
	    حالت 'n': حالت 0xf1: حالت 0x144: حالت 0x146: حالت 0x148:
	    حالت 0x149: حالت 0x1f9: حالت 0x1d70: حالت 0x1d87:
	    حالت 0x1e45: حالت 0x1e47: حالت 0x1e49: حالت 0x1e4b:
	    حالت 0xa7a5:
		      regmbc('n'); regmbc(0xf1); regmbc(0x144);
		      regmbc(0x146); regmbc(0x148); regmbc(0x149);
		      regmbc(0x1f9); regmbc(0x1d70); regmbc(0x1d87);
		      regmbc(0x1e45); regmbc(0x1e47); regmbc(0x1e49);
		      regmbc(0x1e4b); regmbc(0xa7a5);
		      بازگشت;
	    حالت 'o': حالت 0xf2: حالت 0xf3: حالت 0xf4: حالت 0xf5:
	    حالت 0xf6: حالت 0xf8: حالت 0x14d: حالت 0x14f: حالت 0x151:
	    حالت 0x1a1: حالت 0x1d2: حالت 0x1eb: حالت 0x1ed: حالت 0x1ff:
	    حالت 0x20d: حالت 0x20f: حالت 0x22b: حالت 0x22d: حالت 0x22f:
	    حالت 0x231: حالت 0x275: حالت 0x1e4d: حالت 0x1e4f:
	    حالت 0x1e51: حالت 0x1e53: حالت 0x1ecd: حالت 0x1ecf:
	    حالت 0x1ed1: حالت 0x1ed3: حالت 0x1ed5: حالت 0x1ed7:
	    حالت 0x1ed9: حالت 0x1edb: حالت 0x1edd: حالت 0x1edf:
	    حالت 0x1ee1: حالت 0x1ee3:
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
		      بازگشت;
	    حالت 'p': حالت 0x1a5: حالت 0x1d71: حالت 0x1d88: حالت 0x1d7d:
	    حالت 0x1e55: حالت 0x1e57:
		      regmbc('p'); regmbc(0x1a5); regmbc(0x1d71);
		      regmbc(0x1d7d); regmbc(0x1d88); regmbc(0x1e55);
		      regmbc(0x1e57);
		      بازگشت;
	    حالت 'q': حالت 0x24b: حالت 0x2a0:
		      regmbc('q'); regmbc(0x24b); regmbc(0x2a0);
		      بازگشت;
	    حالت 'r': حالت 0x155: حالت 0x157: حالت 0x159: حالت 0x211:
	    حالت 0x213: حالت 0x24d: حالت 0x27d: حالت 0x1d72: حالت 0x1d73:
	    حالت 0x1d89: حالت 0x1e59: حالت 0x1e5b: حالت 0x1e5d: حالت 0x1e5f:
	    حالت 0xa7a7:
		      regmbc('r'); regmbc(0x155); regmbc(0x157);
		      regmbc(0x159); regmbc(0x211); regmbc(0x213);
		      regmbc(0x24d); regmbc(0x1d72); regmbc(0x1d73);
		      regmbc(0x1d89); regmbc(0x1e59); regmbc(0x27d);
		      regmbc(0x1e5b); regmbc(0x1e5d); regmbc(0x1e5f);
		      regmbc(0xa7a7);
		      بازگشت;
	    حالت 's': حالت 0x15b: حالت 0x15d: حالت 0x15f: حالت 0x161:
	    حالت 0x1e61: حالت 0x219: حالت 0x23f: حالت 0x1d74: حالت 0x1d8a:
	    حالت 0x1e63: حالت 0x1e65: حالت 0x1e67: حالت 0x1e69: حالت 0xa7a9:
		      regmbc('s'); regmbc(0x15b); regmbc(0x15d);
		      regmbc(0x15f); regmbc(0x161); regmbc(0x23f);
		      regmbc(0x219); regmbc(0x1d74); regmbc(0x1d8a);
		      regmbc(0x1e61); regmbc(0x1e63); regmbc(0x1e65);
		      regmbc(0x1e67); regmbc(0x1e69); regmbc(0xa7a9);
		      بازگشت;
	    حالت 't': حالت 0x163: حالت 0x165: حالت 0x167: حالت 0x1ab:
	    حالت 0x1ad: حالت 0x21b: حالت 0x288: حالت 0x1d75: حالت 0x1e6b:
	    حالت 0x1e6d: حالت 0x1e6f: حالت 0x1e71: حالت 0x1e97: حالت 0x2c66:
		      regmbc('t'); regmbc(0x163); regmbc(0x165);
		      regmbc(0x167); regmbc(0x1ab); regmbc(0x21b);
		      regmbc(0x1ad); regmbc(0x288); regmbc(0x1d75);
		      regmbc(0x1e6b); regmbc(0x1e6d); regmbc(0x1e6f);
		      regmbc(0x1e71); regmbc(0x1e97); regmbc(0x2c66);
		      بازگشت;
	    حالت 'u': حالت 0xf9: حالت 0xfa: حالت 0xfb: حالت 0xfc:
	    حالت 0x169: حالت 0x16b: حالت 0x16d: حالت 0x16f:
	    حالت 0x171: حالت 0x173: حالت 0x1b0: حالت 0x1d4:
	    حالت 0x1d6: حالت 0x1d8: حالت 0x1da: حالت 0x1dc:
	    حالت 0x215: حالت 0x217: حالت 0x289: حالت 0x1e73:
	    حالت 0x1d7e: حالت 0x1d99: حالت 0x1e75: حالت 0x1e77:
	    حالت 0x1e79: حالت 0x1e7b: حالت 0x1ee5: حالت 0x1ee7:
	    حالت 0x1ee9: حالت 0x1eeb: حالت 0x1eed: حالت 0x1eef:
	    حالت 0x1ef1:
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
		      بازگشت;
	    حالت 'v': حالت 0x28b: حالت 0x1d8c: حالت 0x1e7d: حالت 0x1e7f:
		      regmbc('v'); regmbc(0x28b); regmbc(0x1d8c);
		      regmbc(0x1e7d); regmbc(0x1e7f);
		      بازگشت;
	    حالت 'w': حالت 0x175: حالت 0x1e81: حالت 0x1e83:
	    حالت 0x1e85: حالت 0x1e87: حالت 0x1e89: حالت 0x1e98:
		      regmbc('w'); regmbc(0x175); regmbc(0x1e81);
		      regmbc(0x1e83); regmbc(0x1e85); regmbc(0x1e87);
		      regmbc(0x1e89); regmbc(0x1e98);
		      بازگشت;
	    حالت 'x': حالت 0x1e8b: حالت 0x1e8d:
		      regmbc('x'); regmbc(0x1e8b); regmbc(0x1e8d);
		      بازگشت;
	    حالت 'y': حالت 0xfd: حالت 0xff: حالت 0x177: حالت 0x1b4:
	    حالت 0x233: حالت 0x24f: حالت 0x1e8f: حالت 0x1e99: حالت 0x1ef3:
	    حالت 0x1ef5: حالت 0x1ef7: حالت 0x1ef9:
		      regmbc('y'); regmbc(0xfd); regmbc(0xff);
		      regmbc(0x177); regmbc(0x1b4); regmbc(0x233);
		      regmbc(0x24f); regmbc(0x1e8f); regmbc(0x1e99);
		      regmbc(0x1ef3); regmbc(0x1ef5); regmbc(0x1ef7);
		      regmbc(0x1ef9);
		      بازگشت;
	    حالت 'z': حالت 0x17a: حالت 0x17c: حالت 0x17e: حالت 0x1b6:
	    حالت 0x1d76: حالت 0x1d8e: حالت 0x1e91: حالت 0x1e93:
	    حالت 0x1e95: حالت 0x2c6c:
		      regmbc('z'); regmbc(0x17a); regmbc(0x17c);
		      regmbc(0x17e); regmbc(0x1b6); regmbc(0x1d76);
		      regmbc(0x1d8e); regmbc(0x1e91); regmbc(0x1e93);
		      regmbc(0x1e95); regmbc(0x2c6c);
		      بازگشت;
	}
    }
    regmbc(c);
}

/*
 * Emit a node.
 * Return pointer to generated code.
 */
    ایستا char_u *
regnode(صحیح op)
{
    char_u  *ret;

    ret = regcode;
    اگر (ret == JUST_CALC_SIZE)
	regsize += 3;
    وگرنه
    {
	*regcode++ = op;
	*regcode++ = NUL;		// Null "next" pointer.
	*regcode++ = NUL;
    }
    بازگشت ret;
}

/*
 * Write a long as four bytes at "p" and return pointer to the next char.
 */
    ایستا char_u *
re_put_long(char_u *p, long_u val)
{
    *p++ = (char_u) ((val >> 24) & 0377);
    *p++ = (char_u) ((val >> 16) & 0377);
    *p++ = (char_u) ((val >> 8) & 0377);
    *p++ = (char_u) (val & 0377);
    بازگشت p;
}

/*
 * regnext - dig the "next" pointer out of a node
 * Returns NULL when calculating size, when there is no next item and when
 * there is an error.
 */
    ایستا char_u *
regnext(char_u *p)
{
    صحیح	    offset;

    اگر (p == JUST_CALC_SIZE || reg_toolong)
	بازگشت پوچ;

    offset = NEXT(p);
    اگر (offset == 0)
	بازگشت پوچ;

    اگر (OP(p) == BACK)
	بازگشت p - offset;
    وگرنه
	بازگشت p + offset;
}

/*
 * Set the next-pointer at the end of a node chain.
 */
    ایستا تهی
regtail(char_u *p, char_u *val)
{
    char_u	*scan;
    char_u	*temp;
    صحیح		offset;

    اگر (p == JUST_CALC_SIZE)
	بازگشت;

    // Find last node.
    scan = p;
    برای (;;)
    {
	temp = regnext(scan);
	اگر (temp == پوچ)
	    بشکن;
	scan = temp;
    }

    اگر (OP(scan) == BACK)
	offset = (صحیح)(scan - val);
    وگرنه
	offset = (صحیح)(val - scan);
    // When the offset uses more than 16 bits it can no longer fit in the two
    // bytes available.  Use a global flag to avoid having to check return
    // values in too many places.
    اگر (offset > 0xffff)
	reg_toolong = TRUE;
    وگرنه
    {
	*(scan + 1) = (char_u) (((unsigned)offset >> 8) & 0377);
	*(scan + 2) = (char_u) (offset & 0377);
    }
}

/*
 * Like regtail, on item after a BRANCH; nop if none.
 */
    ایستا تهی
regoptail(char_u *p, char_u *val)
{
    // When op is neither BRANCH nor BRACE_COMPLEX0-9, it is "operandless"
    اگر (p == پوچ || p == JUST_CALC_SIZE
	    || (OP(p) != BRANCH
		&& (OP(p) < BRACE_COMPLEX || OP(p) > BRACE_COMPLEX + 9)))
	بازگشت;
    regtail(OPERAND(p), val);
}

/*
 * Insert an operator in front of already-emitted operand
 *
 * Means relocating the operand.
 */
    ایستا تهی
reginsert(صحیح op, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    اگر (regcode == JUST_CALC_SIZE)
    {
	regsize += 3;
	بازگشت;
    }
    src = regcode;
    regcode += 3;
    dst = regcode;
    وقتی (src > opnd)
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
    ایستا تهی
reginsert_nr(صحیح op, long val, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    اگر (regcode == JUST_CALC_SIZE)
    {
	regsize += 7;
	بازگشت;
    }
    src = regcode;
    regcode += 7;
    dst = regcode;
    وقتی (src > opnd)
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
    ایستا تهی
reginsert_limits(
    صحیح		op,
    long	minval,
    long	maxval,
    char_u	*opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    اگر (regcode == JUST_CALC_SIZE)
    {
	regsize += 11;
	بازگشت;
    }
    src = regcode;
    regcode += 11;
    dst = regcode;
    وقتی (src > opnd)
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
    ایستا صحیح
seen_endbrace(صحیح refnum)
{
    اگر (!had_endbrace[refnum])
    {
	char_u *p;

	// Trick: check if "@<=" or "@<!" follows, in which case
	// the \1 can appear before the referenced match.
	برای (p = regparse; *p != NUL; ++p)
	    اگر (p[0] == '@' && p[1] == '<' && (p[2] == '!' || p[2] == '='))
		بشکن;
	اگر (*p == NUL)
	{
	    emsg(_(e_illegal_back_reference));
	    rc_did_emsg = TRUE;
	    بازگشت FALSE;
	}
    }
    بازگشت TRUE;
}

/*
 * Parse the lowest level.
 *
 * Optimization:  gobbles an entire sequence of ordinary characters so that
 * it can turn them into a single node, which is smaller to store and
 * faster to run.  Don't do this when one_exactly is set.
 */
    ایستا char_u *
regatom(صحیح *flagp)
{
    char_u	    *ret;
    صحیح		    flags;
    صحیح		    c;
    char_u	    *p;
    صحیح		    extra = 0;
    صحیح		    save_prev_at_start = prev_at_start;

    *flagp = WORST;		// Tentatively.

    c = getchr();
    انتخاب (c)
    {
      حالت Magic('^'):
	ret = regnode(BOL);
	بشکن;

      حالت Magic('$'):
	ret = regnode(EOL);
#اگر defined(FEAT_SYN_HL)
	had_eol = TRUE;
#endif
	بشکن;

      حالت Magic('<'):
	ret = regnode(BOW);
	بشکن;

      حالت Magic('>'):
	ret = regnode(EOW);
	بشکن;

      حالت Magic('_'):
	c = no_Magic(getchr());
	اگر (c == '^')		// "\_^" is start-of-line
	{
	    ret = regnode(BOL);
	    بشکن;
	}
	اگر (c == '$')		// "\_$" is end-of-line
	{
	    ret = regnode(EOL);
#اگر defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#endif
	    بشکن;
	}

	extra = ADD_NL;
	*flagp |= HASNL;

	// "\_[" is character range plus newline
	اگر (c == '[')
	    برو collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	// Character classes.
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
	اگر (p == پوچ)
	    EMSG_RET_NULL(_(e_invalid_use_of_underscore));

	// When '.' is followed by a composing char ignore the dot, so that
	// the composing char is matched here.
	اگر (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	{
	    c = getchr();
	    برو do_multibyte;
	}
	ret = regnode(classcodes[p - classchars] + extra);
	*flagp |= HASWIDTH | SIMPLE;
	بشکن;

      حالت Magic('n'):
	اگر (reg_string)
	{
	    // In a string "\n" matches a newline character.
	    ret = regnode(EXACTLY);
	    regc(NL);
	    regc(NUL);
	    *flagp |= HASWIDTH | SIMPLE;
	}
	وگرنه
	{
	    // In buffer text "\n" matches the end of a line.
	    ret = regnode(NEWL);
	    *flagp |= HASWIDTH | HASNL;
	}
	بشکن;

      حالت Magic('('):
	اگر (one_exactly)
	    EMSG_ONE_RET_NULL;
	ret = reg(REG_PAREN, &flags);
	اگر (ret == پوچ)
	    بازگشت پوچ;
	*flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
	بشکن;

      حالت NUL:
      حالت Magic('|'):
      حالت Magic('&'):
      حالت Magic(')'):
	اگر (one_exactly)
	    EMSG_ONE_RET_NULL;
	// Supposed to be caught earlier.
	IEMSG_RET_NULL(e_internal_error_in_regexp);
	// NOTREACHED

      حالت Magic('='):
      حالت Magic('?'):
      حالت Magic('+'):
      حالت Magic('@'):
      حالت Magic('{'):
      حالت Magic('*'):
	c = no_Magic(c);
	EMSG3_RET_NULL(_(e_str_chr_follows_nothing),
		(c == '*' ? reg_magic >= MAGIC_ON : reg_magic == MAGIC_ALL), c);
	// NOTREACHED

      حالت Magic('~'):		// previous substitute pattern
	    اگر (reg_prev_sub != پوچ)
	    {
		char_u	    *lp;

		ret = regnode(EXACTLY);
		lp = reg_prev_sub;
		وقتی (*lp != NUL)
		    regc(*lp++);
		regc(NUL);
		اگر (*reg_prev_sub != NUL)
		{
		    *flagp |= HASWIDTH;
		    اگر ((lp - reg_prev_sub) == 1)
			*flagp |= SIMPLE;
		}
	    }
	    وگرنه
		EMSG_RET_NULL(_(e_no_previous_substitute_regular_expression));
	    بشکن;

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
		صحیح		    refnum;

		refnum = c - Magic('0');
		اگر (!seen_endbrace(refnum))
		    بازگشت پوچ;
		ret = regnode(BACKREF + refnum);
	    }
	    بشکن;

      حالت Magic('z'):
	{
	    c = no_Magic(getchr());
	    انتخاب (c)
	    {
#ifdef FEAT_SYN_HL
		حالت '(': اگر ((reg_do_extmatch & REX_SET) == 0)
			      EMSG_RET_NULL(_(e_z_not_allowed_here));
			  اگر (one_exactly)
			      EMSG_ONE_RET_NULL;
			  ret = reg(REG_ZPAREN, &flags);
			  اگر (ret == پوچ)
			      بازگشت پوچ;
			  *flagp |= flags & (HASWIDTH|SPSTART|HASNL|HASLOOKBH);
			  re_has_z = REX_SET;
			  بشکن;

		حالت '1':
		حالت '2':
		حالت '3':
		حالت '4':
		حالت '5':
		حالت '6':
		حالت '7':
		حالت '8':
		حالت '9': اگر ((reg_do_extmatch & REX_USE) == 0)
			      EMSG_RET_NULL(_(e_z1_z9_not_allowed_here));
			  ret = regnode(ZREF + c - '0');
			  re_has_z = REX_USE;
			  بشکن;
#endif

		حالت 's': ret = regnode(MOPEN + 0);
			  اگر (re_mult_next("\\zs") == FAIL)
			      بازگشت پوچ;
			  بشکن;

		حالت 'e': ret = regnode(MCLOSE + 0);
			  اگر (re_mult_next("\\ze") == FAIL)
			      بازگشت پوچ;
			  بشکن;

		پیشفرض:  EMSG_RET_NULL(_(e_invalid_character_after_bsl_z));
	    }
	}
	بشکن;

      حالت Magic('%'):
	{
	    c = no_Magic(getchr());
	    انتخاب (c)
	    {
		// () without a back reference
		حالت '(':
		    اگر (one_exactly)
			EMSG_ONE_RET_NULL;
		    ret = reg(REG_NPAREN, &flags);
		    اگر (ret == پوچ)
			بازگشت پوچ;
		    *flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
		    بشکن;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		حالت '^':
		    ret = regnode(RE_BOF);
		    بشکن;

		حالت '$':
		    ret = regnode(RE_EOF);
		    بشکن;

		حالت '#':
		    اگر (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_start_of_pattern),
								  regparse[1]);
			بازگشت FAIL;
		    }
		    ret = regnode(CURSOR);
		    بشکن;

		حالت 'V':
		    ret = regnode(RE_VISUAL);
		    بشکن;

		حالت 'C':
		    ret = regnode(RE_COMPOSING);
		    بشکن;

		// \%[abc]: Emit as a list of branches, all ending at the last
		// branch which matches nothing.
		حالت '[':
			  اگر (one_exactly)	// doesn't nest
			      EMSG_ONE_RET_NULL;
			  {
			      char_u	*lastbranch;
			      char_u	*lastnode = پوچ;
			      char_u	*br;

			      ret = پوچ;
			      وقتی ((c = getchr()) != ']')
			      {
				  اگر (c == NUL)
				      EMSG2_RET_NULL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
				  br = regnode(BRANCH);
				  اگر (ret == پوچ)
				      ret = br;
				  وگرنه
				  {
				      regtail(lastnode, br);
				      اگر (reg_toolong)
					  بازگشت پوچ;
				  }

				  ungetchr();
				  one_exactly = TRUE;
				  lastnode = regatom(flagp);
				  one_exactly = FALSE;
				  اگر (lastnode == پوچ)
				      بازگشت پوچ;
			      }
			      اگر (ret == پوچ)
				  EMSG2_RET_NULL(_(e_empty_str_brackets),
						      reg_magic == MAGIC_ALL);
			      lastbranch = regnode(BRANCH);
			      br = regnode(NOTHING);
			      اگر (ret != JUST_CALC_SIZE)
			      {
				  regtail(lastnode, br);
				  regtail(lastbranch, br);
				  // connect all branches to the NOTHING
				  // branch at the end
				  برای (br = ret; br != lastnode; )
				  {
				      اگر (OP(br) == BRANCH)
				      {
					  regtail(br, lastbranch);
					  اگر (reg_toolong)
					      بازگشت پوچ;
					  br = OPERAND(br);
				      }
				      وگرنه
					  br = regnext(br);
				  }
			      }
			      *flagp &= ~(HASWIDTH | SIMPLE);
			      بشکن;
			  }

		حالت 'd':   // %d123 decimal
		حالت 'o':   // %o123 octal
		حالت 'x':   // %xab hex 2
		حالت 'u':   // %uabcd hex 4
		حالت 'U':   // %U1234abcd hex 8
			  {
			      vimlong_T i;

			      انتخاب (c)
			      {
				  حالت 'd': i = getdecchrs(); بشکن;
				  حالت 'o': i = getoctchrs(); بشکن;
				  حالت 'x': i = gethexchrs(2); بشکن;
				  حالت 'u': i = gethexchrs(4); بشکن;
				  حالت 'U': i = gethexchrs(8); بشکن;
				  پیشفرض:  i = -1; بشکن;
			      }

			      اگر (i < 0 || i > INT_MAX)
				  EMSG2_RET_NULL(
					    _(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			      اگر (use_multibytecode(i))
				  ret = regnode(MULTIBYTECODE);
			      وگرنه
				  ret = regnode(EXACTLY);
			      اگر (i == 0)
				  regc(0x0a);
			      وگرنه
				  regmbc((صحیح)i);
			      regc(NUL);
			      *flagp |= HASWIDTH;
			      بشکن;
			  }

		پیشفرض:
			  اگر (VIM_ISDIGIT(c) || c == '<' || c == '>'
						|| c == '\'' || c == '.')
			  {
			      long_u	n = 0;
			      صحیح	cmp;
			      صحیح	cur = FALSE;
			      صحیح	got_digit = FALSE;

			      cmp = c;
			      اگر (cmp == '<' || cmp == '>')
				  c = getchr();
			      اگر (no_Magic(c) == '.')
			      {
				  cur = TRUE;
				  c = getchr();
			      }
			      وقتی (VIM_ISDIGIT(c))
			      {
				  got_digit = TRUE;
				  n = n * 10 + (c - '0');
				  c = getchr();
			      }
			      اگر (no_Magic(c) == '\'' && n == 0)
			      {
				  // "\%'m", "\%<'m" and "\%>'m": Mark
				  c = getchr();
				  ret = regnode(RE_MARK);
				  اگر (ret == JUST_CALC_SIZE)
				      regsize += 2;
				  وگرنه
				  {
				      *regcode++ = c;
				      *regcode++ = cmp;
				  }
				  بشکن;
			      }
			      وگرنه اگر ((c == 'l' || c == 'c' || c == 'v')
					  && (cur || got_digit))
			      {
				  اگر (cur && n)
				  {
				    semsg(_(e_regexp_عدد_after_dot_pos_جستجو_chr),
								  no_Magic(c));
				    rc_did_emsg = TRUE;
				    بازگشت پوچ;
				  }
				  اگر (c == 'l')
				  {
				      اگر (cur)
					  n = curwin->w_cursor.lnum;
				      ret = regnode(RE_LNUM);
				      اگر (save_prev_at_start)
					  at_start = TRUE;
				  }
				  وگرنه اگر (c == 'c')
				  {
				      اگر (cur)
				      {
					  n = curwin->w_cursor.col;
					  n++;
				      }
				      ret = regnode(RE_COL);
				  }
				  وگرنه
				  {
				      اگر (cur)
				      {
					  colnr_T vcol = 0;

					  getvvcol(curwin, &curwin->w_cursor,
							 پوچ, پوچ, &vcol, 0);
					  ++vcol;
					  n = vcol;
				      }
				      ret = regnode(RE_VCOL);
				  }
				  اگر (ret == JUST_CALC_SIZE)
				      regsize += 5;
				  وگرنه
				  {
				      // put the number and the optional
				      // comparator after the opcode
				      regcode = re_put_long(regcode, n);
				      *regcode++ = cmp;
				  }
				  بشکن;
			      }
			  }

			  EMSG2_RET_NULL(_(e_invalid_character_after_str),
						      reg_magic == MAGIC_ALL);
	    }
	}
	بشکن;

      حالت Magic('['):
collection:
	{
	    char_u	*lp;

	    // If there is no matching ']', we assume the '[' is a normal
	    // character.  This makes 'incsearch' and ":help [" work.
	    lp = skip_anyof(regparse);
	    اگر (*lp == ']')	// there is a matching ']'
	    {
		صحیح	startc = -1;	// > 0 when next '-' is a range
		صحیح	endc;

		اگر (extra == 0)
		{
		    صحیح	cl = bt_recognize_char_class(regparse, lp);

		    اگر (cl != 0)
		    {
			ret = regnode(cl);
			regparse = lp;
			skipchr();
			*flagp |= HASWIDTH | SIMPLE;
			بشکن;
		    }
		}

		// In a character class, different parsing rules apply.
		// Not even \ is special anymore, nothing is.
		اگر (*regparse == '^')	    // Complement of range.
		{
		    ret = regnode(ANYBUT + extra);
		    regparse++;
		}
		وگرنه
		    ret = regnode(ANYOF + extra);

		// At the start ']' and '-' mean the literal character.
		اگر (*regparse == ']' || *regparse == '-')
		{
		    startc = *regparse;
		    regc(*regparse++);
		}

		وقتی (*regparse != NUL && *regparse != ']')
		{
		    اگر (*regparse == '-')
		    {
			++regparse;
			// The '-' is not used for a range at the end and
			// after or before a '\n'.
			اگر (*regparse == ']' || *regparse == NUL
				|| startc == -1
				|| (regparse[0] == '\\' && regparse[1] == 'n'))
			{
			    regc('-');
			    startc = '-';	// [--x] is a range
			}
			وگرنه
			{
			    // Also accept "a-[.z.]"
			    endc = 0;
			    اگر (*regparse == '[')
				endc = get_coll_element(&regparse);
			    اگر (endc == 0)
			    {
				اگر (has_mbyte)
				    endc = mb_ptr2char_adv(&regparse);
				وگرنه
				    endc = *regparse++;
			    }

			    // Handle \o40, \x20 and \u20AC style sequences
			    اگر (endc == '\\' && !reg_cpo_lit && !reg_cpo_bsl)
				endc = coll_get_char();

			    اگر (startc > endc)
				EMSG_RET_NULL(_(e_reverse_range_in_character_class));
			    اگر (has_mbyte && ((*mb_char2len)(startc) > 1
						 || (*mb_char2len)(endc) > 1))
			    {
				// Limit to a range of 256 chars.
				اگر (endc > startc + 256)
				    EMSG_RET_NULL(_(e_range_too_large_in_character_class));
				وقتی (++startc <= endc)
				    regmbc(startc);
			    }
			    وگرنه
			    {
				وقتی (++startc <= endc)
				    regc(startc);
			    }
			    startc = -1;
			}
		    }
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    وگرنه اگر (*regparse == '\\'
			    && !reg_cpo_bsl
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != پوچ
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR,
						       regparse[1]) != پوچ)))
		    {
			regparse++;
			اگر (*regparse == 'n')
			{
			    // '\n' in range: also match NL
			    اگر (ret != JUST_CALC_SIZE)
			    {
				// Using \n inside [^] does not change what
				// matches. "[^\n]" is the same as ".".
				اگر (*ret == ANYOF)
				{
				    *ret = ANYOF + ADD_NL;
				    *flagp |= HASNL;
				}
				// else: must have had a \n already
			    }
			    regparse++;
			    startc = -1;
			}
			وگرنه اگر (*regparse == 'd'
				|| *regparse == 'o'
				|| *regparse == 'x'
				|| *regparse == 'u'
				|| *regparse == 'U')
			{
			    startc = coll_get_char();
			    // max UTF-8 Codepoint is U+10FFFF,
			    // but allow values until INT_MAX
			    اگر (startc == INT_MAX)
				EMSG_RET_NULL(_(e_unicode_val_too_large));
			    اگر (startc == 0)
				regc(0x0a);
			    وگرنه
				regmbc(startc);
			}
			وگرنه
			{
			    startc = backslash_trans(*regparse++);
			    regc(startc);
			}
		    }
		    وگرنه اگر (*regparse == '[')
		    {
			صحیح c_class;
			صحیح cu;

			c_class = get_char_class(&regparse);
			startc = -1;
			// Characters assumed to be 8 bits!
			انتخاب (c_class)
			{
			    حالت CLASS_NONE:
				c_class = get_equi_class(&regparse);
				اگر (c_class != 0)
				{
				    // produce equivalence class
				    reg_equi_class(c_class);
				}
				وگرنه اگر ((c_class =
					    get_coll_element(&regparse)) != 0)
				{
				    // produce a collating element
				    regmbc(c_class);
				}
				وگرنه
				{
				    // literal '[', allow [[-x] as a range
				    startc = *regparse++;
				    regc(startc);
				}
				بشکن;
			    حالت CLASS_ALNUM:
				برای (cu = 1; cu < 128; cu++)
				    اگر (isalnum(cu))
					regmbc(cu);
				بشکن;
			    حالت CLASS_ALPHA:
				برای (cu = 1; cu < 128; cu++)
				    اگر (isalpha(cu))
					regmbc(cu);
				بشکن;
			    حالت CLASS_BLANK:
				regc(' ');
				regc('\t');
				بشکن;
			    حالت CLASS_CNTRL:
				برای (cu = 1; cu <= 127; cu++)
				    اگر (iscntrl(cu))
					regmbc(cu);
				بشکن;
			    حالت CLASS_DIGIT:
				برای (cu = 1; cu <= 127; cu++)
				    اگر (VIM_ISDIGIT(cu))
					regmbc(cu);
				بشکن;
			    حالت CLASS_GRAPH:
				برای (cu = 1; cu <= 127; cu++)
				    اگر (isgraph(cu))
					regmbc(cu);
				بشکن;
			    حالت CLASS_LOWER:
				برای (cu = 1; cu <= 255; cu++)
				    اگر (MB_ISLOWER(cu) && cu != 170
								 && cu != 186)
					regmbc(cu);
				بشکن;
			    حالت CLASS_PRINT:
				برای (cu = 1; cu <= 255; cu++)
				    اگر (vim_isprintc(cu))
					regmbc(cu);
				بشکن;
			    حالت CLASS_PUNCT:
				برای (cu = 1; cu < 128; cu++)
				    اگر (ispunct(cu))
					regmbc(cu);
				بشکن;
			    حالت CLASS_SPACE:
				برای (cu = 9; cu <= 13; cu++)
				    regc(cu);
				regc(' ');
				بشکن;
			    حالت CLASS_UPPER:
				برای (cu = 1; cu <= 255; cu++)
				    اگر (MB_ISUPPER(cu))
					regmbc(cu);
				بشکن;
			    حالت CLASS_XDIGIT:
				برای (cu = 1; cu <= 255; cu++)
				    اگر (vim_isxdigit(cu))
					regmbc(cu);
				بشکن;
			    حالت CLASS_TAB:
				regc('\t');
				بشکن;
			    حالت CLASS_RETURN:
				regc('\r');
				بشکن;
			    حالت CLASS_BACKSPACE:
				regc('\b');
				بشکن;
			    حالت CLASS_ESCAPE:
				regc('\033');
				بشکن;
			    حالت CLASS_IDENT:
				برای (cu = 1; cu <= 255; cu++)
				    اگر (vim_isIDc(cu))
					regmbc(cu);
				بشکن;
			    حالت CLASS_KEYWORD:
				برای (cu = 1; cu <= 255; cu++)
				    اگر (reg_iswordc(cu))
					regmbc(cu);
				بشکن;
			    حالت CLASS_FNAME:
				برای (cu = 1; cu <= 255; cu++)
				    اگر (vim_isfilec(cu))
					regmbc(cu);
				بشکن;
			}
		    }
		    وگرنه
		    {
			اگر (has_mbyte)
			{
			    صحیح	len;

			    // produce a multibyte character, including any
			    // following composing characters
			    startc = mb_ptr2char(regparse);
			    len = (*mb_ptr2len)(regparse);
			    اگر (enc_utf8 && utf_char2len(startc) != len)
				startc = -1;	// composing chars
			    وقتی (--len >= 0)
				regc(*regparse++);
			}
			وگرنه
			{
			    startc = *regparse++;
			    regc(startc);
			}
		    }
		}
		regc(NUL);
		prevchr_len = 1;	// last char was the ']'
		اگر (*regparse != ']')
		    EMSG_RET_NULL(_(e_too_many_brackets));  // Cannot happen?
		skipchr();	    // let's be friends with the lexer again
		*flagp |= HASWIDTH | SIMPLE;
		بشکن;
	    }
	    وگرنه اگر (reg_strict)
		EMSG2_RET_NULL(_(e_missing_rsb_after_str_lsb),
							reg_magic > MAGIC_OFF);
	}
	// FALLTHROUGH

      پیشفرض:
	{
	    صحیح		len;

	    // A multi-byte character is handled as a separate atom if it's
	    // before a multi and when it's a composing char.
	    اگر (use_multibytecode(c))
	    {
do_multibyte:
		ret = regnode(MULTIBYTECODE);
		regmbc(c);
		*flagp |= HASWIDTH | SIMPLE;
		بشکن;
	    }

	    ret = regnode(EXACTLY);

	    // Append characters as long as:
	    // - there is no following multi, we then need the character in
	    //   front of it as a single character operand
	    // - not running into a Magic character
	    // - "one_exactly" is not set
	    // But always emit at least one character.  Might be a Multi,
	    // e.g., a "[" without matching "]".
	    برای (len = 0; c != NUL && (len == 0
			|| (re_multi_type(peekchr()) == NOT_MULTI
			    && !one_exactly
			    && !is_Magic(c))); ++len)
	    {
		c = no_Magic(c);
		اگر (has_mbyte)
		{
		    regmbc(c);
		    اگر (enc_utf8)
		    {
			صحیح	l;

			// Need to get composing character too.
			برای (;;)
			{
			    l = utf_ptr2len(regparse);
			    اگر (!UTF_COMPOSINGLIKE(regparse, regparse + l))
				بشکن;
			    regmbc(utf_ptr2char(regparse));
			    skipchr();
			}
		    }
		}
		وگرنه
		    regc(c);
		c = getchr();
	    }
	    ungetchr();

	    regc(NUL);
	    *flagp |= HASWIDTH;
	    اگر (len == 1)
		*flagp |= SIMPLE;
	}
	بشکن;
    }

    بازگشت ret;
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
    ایستا char_u *
regpiece(صحیح *flagp)
{
    char_u	    *ret;
    صحیح		    op;
    char_u	    *next;
    صحیح		    flags;
    long	    minval;
    long	    maxval;

    ret = regatom(&flags);
    اگر (ret == پوچ)
	بازگشت پوچ;

    op = peekchr();
    اگر (re_multi_type(op) == NOT_MULTI)
    {
	*flagp = flags;
	بازگشت ret;
    }
    // default flags
    *flagp = (WORST | SPSTART | (flags & (HASNL | HASLOOKBH)));

    skipchr();
    انتخاب (op)
    {
	حالت Magic('*'):
	    اگر (flags & SIMPLE)
		reginsert(STAR, ret);
	    وگرنه
	    {
		// Emit x* as (x&|), where & means "self".
		reginsert(BRANCH, ret); // Either x
		regoptail(ret, regnode(BACK));	// and loop
		regoptail(ret, ret);	// back
		regtail(ret, regnode(BRANCH));	// or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    بشکن;

	حالت Magic('+'):
	    اگر (flags & SIMPLE)
		reginsert(PLUS, ret);
	    وگرنه
	    {
		// Emit x+ as x(&|), where & means "self".
		next = regnode(BRANCH); // Either
		regtail(ret, next);
		regtail(regnode(BACK), ret);	// loop back
		regtail(next, regnode(BRANCH)); // or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    *flagp = (WORST | HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    بشکن;

	حالت Magic('@'):
	    {
		صحیح	lop = END;
		long	nr;

		nr = (long)getdecchrs();
		انتخاب (no_Magic(getchr()))
		{
		    حالت '=': lop = MATCH; بشکن;		  // \@=
		    حالت '!': lop = NOMATCH; بشکن;		  // \@!
		    حالت '>': lop = SUBPAT; بشکن;		  // \@>
		    حالت '<': انتخاب (no_Magic(getchr()))
			      {
				  حالت '=': lop = BEHIND; بشکن;   // \@<=
				  حالت '!': lop = NOBEHIND; بشکن; // \@<!
			      }
		}
		اگر (lop == END)
		    EMSG2_RET_NULL(_(e_invalid_character_after_str_at),
						      reg_magic == MAGIC_ALL);
		// Look behind must match with behind_pos.
		اگر (lop == BEHIND || lop == NOBEHIND)
		{
		    regtail(ret, regnode(BHPOS));
		    *flagp |= HASLOOKBH;
		}
		regtail(ret, regnode(END)); // operand ends
		اگر (lop == BEHIND || lop == NOBEHIND)
		{
		    اگر (nr < 0)
			nr = 0; // no limit is same as zero limit
		    reginsert_nr(lop, nr, ret);
		}
		وگرنه
		    reginsert(lop, ret);
		بشکن;
	    }

	حالت Magic('?'):
	حالت Magic('='):
	    // Emit x= as (x|)
	    reginsert(BRANCH, ret);		// Either x
	    regtail(ret, regnode(BRANCH));	// or
	    next = regnode(NOTHING);		// null.
	    regtail(ret, next);
	    regoptail(ret, next);
	    بشکن;

	حالت Magic('{'):
	    اگر (!read_limits(&minval, &maxval))
		بازگشت پوچ;
	    اگر (flags & SIMPLE)
	    {
		reginsert(BRACE_SIMPLE, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
	    }
	    وگرنه
	    {
		اگر (رقم_complex_braces >= 10)
		    EMSG2_RET_NULL(_(e_too_many_complex_str_curly),
						      reg_magic == MAGIC_ALL);
		reginsert(BRACE_COMPLEX + رقم_complex_braces, ret);
		regoptail(ret, regnode(BACK));
		regoptail(ret, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
		++رقم_complex_braces;
	    }
	    اگر (minval > 0 && maxval > 0)
		*flagp = (HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    بشکن;
    }
    اگر (re_multi_type(peekchr()) != NOT_MULTI)
    {
	// Can't have a multi follow a multi.
	اگر (peekchr() == Magic('*'))
	    EMSG2_RET_NULL(_(e_nested_str), reg_magic >= MAGIC_ON);
	EMSG3_RET_NULL(_(e_nested_str_chr), reg_magic == MAGIC_ALL,
							  no_Magic(peekchr()));
    }

    بازگشت ret;
}

/*
 * Parse one alternative of an | or & operator.
 * Implements the concatenation operator.
 */
    ایستا char_u *
regconcat(صحیح *flagp)
{
    char_u	*first = پوچ;
    char_u	*chain = پوچ;
    char_u	*latest;
    صحیح		flags;
    صحیح		cont = TRUE;

    *flagp = WORST;		// Tentatively.

    وقتی (cont)
    {
	انتخاب (peekchr())
	{
	    حالت NUL:
	    حالت Magic('|'):
	    حالت Magic('&'):
	    حالت Magic(')'):
			    cont = FALSE;
			    بشکن;
	    حالت Magic('Z'):
			    regflags |= RF_ICOMBINE;
			    skipchr_keepstart();
			    بشکن;
	    حالت Magic('c'):
			    regflags |= RF_ICASE;
			    skipchr_keepstart();
			    بشکن;
	    حالت Magic('C'):
			    regflags |= RF_NOICASE;
			    skipchr_keepstart();
			    بشکن;
	    حالت Magic('v'):
			    reg_magic = MAGIC_ALL;
			    skipchr_keepstart();
			    curchr = -1;
			    بشکن;
	    حالت Magic('m'):
			    reg_magic = MAGIC_ON;
			    skipchr_keepstart();
			    curchr = -1;
			    بشکن;
	    حالت Magic('M'):
			    reg_magic = MAGIC_OFF;
			    skipchr_keepstart();
			    curchr = -1;
			    بشکن;
	    حالت Magic('V'):
			    reg_magic = MAGIC_NONE;
			    skipchr_keepstart();
			    curchr = -1;
			    بشکن;
	    پیشفرض:
			    latest = regpiece(&flags);
			    اگر (latest == پوچ || reg_toolong)
				بازگشت پوچ;
			    *flagp |= flags & (HASWIDTH | HASNL | HASLOOKBH);
			    اگر (chain == پوچ)	// First piece.
				*flagp |= flags & SPSTART;
			    وگرنه
				regtail(chain, latest);
			    chain = latest;
			    اگر (first == پوچ)
				first = latest;
			    بشکن;
	}
    }
    اگر (first == پوچ)		// Loop ran zero times.
	first = regnode(NOTHING);
    بازگشت first;
}

/*
 * Parse one alternative of an | operator.
 * Implements the & operator.
 */
    ایستا char_u *
regbranch(صحیح *flagp)
{
    char_u	*ret;
    char_u	*chain = پوچ;
    char_u	*latest;
    صحیح		flags;

    *flagp = WORST | HASNL;		// Tentatively.

    ret = regnode(BRANCH);
    برای (;;)
    {
	latest = regconcat(&flags);
	اگر (latest == پوچ)
	    بازگشت پوچ;
	// If one of the branches has width, the whole thing has.  If one of
	// the branches anchors at start-of-line, the whole thing does.
	// If one of the branches uses look-behind, the whole thing does.
	*flagp |= flags & (HASWIDTH | SPSTART | HASLOOKBH);
	// If one of the branches doesn't match a line-break, the whole thing
	// doesn't.
	*flagp &= ~HASNL | (flags & HASNL);
	اگر (chain != پوچ)
	    regtail(chain, latest);
	اگر (peekchr() != Magic('&'))
	    بشکن;
	skipchr();
	regtail(latest, regnode(END)); // operand ends
	اگر (reg_toolong)
	    بشکن;
	reginsert(MATCH, latest);
	chain = latest;
    }

    بازگشت ret;
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
    ایستا char_u *
reg(
    صحیح		paren,	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
    صحیح		*flagp)
{
    char_u	*ret;
    char_u	*br;
    char_u	*ender;
    صحیح		parno = 0;
    صحیح		flags;

    *flagp = HASWIDTH;		// Tentatively.

#ifdef FEAT_SYN_HL
    اگر (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	اگر (regnzpar >= NSUBEXP)
	    EMSG_RET_NULL(_(e_too_many_z));
	parno = regnzpar;
	regnzpar++;
	ret = regnode(ZOPEN + parno);
    }
    وگرنه
#endif
	اگر (paren == REG_PAREN)
    {
	// Make a MOPEN node.
	اگر (regnpar >= NSUBEXP)
	    EMSG2_RET_NULL(_(e_too_many_str_open), reg_magic == MAGIC_ALL);
	parno = regnpar;
	++regnpar;
	ret = regnode(MOPEN + parno);
    }
    وگرنه اگر (paren == REG_NPAREN)
    {
	// Make a NOPEN node.
	ret = regnode(NOPEN);
    }
    وگرنه
	ret = پوچ;

    اگر (bt_reg_parse_depth >= REG_MAX_PAREN_DEPTH)
	EMSG_RET_NULL(_(e_command_too_complex));
    ++bt_reg_parse_depth;

    // Pick up the branches, linking them together.
    br = regbranch(&flags);
    اگر (br == پوچ)
    {
	ret = پوچ;
	برو theend;
    }
    اگر (ret != پوچ)
	regtail(ret, br);	// [MZ]OPEN -> first.
    وگرنه
	ret = br;
    // If one of the branches can be zero-width, the whole thing can.
    // If one of the branches has * at start or matches a line-break, the
    // whole thing can.
    اگر (!(flags & HASWIDTH))
	*flagp &= ~HASWIDTH;
    *flagp |= flags & (SPSTART | HASNL | HASLOOKBH);
    وقتی (peekchr() == Magic('|'))
    {
	skipchr();
	br = regbranch(&flags);
	اگر (br == پوچ || reg_toolong)
	{
	    ret = پوچ;
	    برو theend;
	}
	regtail(ret, br);	// BRANCH -> BRANCH.
	اگر (!(flags & HASWIDTH))
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
    برای (br = ret; br != پوچ; br = regnext(br))
	regoptail(br, ender);

    // Check for proper termination.
    اگر (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
#ifdef FEAT_SYN_HL
	اگر (paren == REG_ZPAREN)
	{
	    emsg(_(e_unmatched_z));
	    rc_did_emsg = TRUE;
	    ret = پوچ;
	    برو theend;
	}
	وگرنه
#endif
	اگر (paren == REG_NPAREN)
	{
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = پوچ;
	    برو theend;
	}
	وگرنه
	{
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = پوچ;
	    برو theend;
	}
    }
    وگرنه اگر (paren == REG_NOPAREN && peekchr() != NUL)
    {
	اگر (curchr == Magic(')'))
	{
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = پوچ;
	    برو theend;
	}
	وگرنه
	{
	    emsg(_(e_trailing_characters));	// "Can't happen".
	    rc_did_emsg = TRUE;
	    ret = پوچ;
	    برو theend;
	}
    }
    // Here we set the flag allowing back references to this set of
    // parentheses.
    اگر (paren == REG_PAREN)
	had_endbrace[parno] = TRUE;	// have seen the close paren

theend:
    --bt_reg_parse_depth;
    بازگشت ret;
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
    ایستا regprog_T *
bt_regcomp(char_u *عبارت, صحیح re_flags)
{
    bt_regprog_T    *r;
    char_u	*scan;
    char_u	*longest;
    صحیح		len;
    صحیح		flags;

    اگر (عبارت == پوچ)
	IEMSG_RET_NULL(e_null_argument);

    init_class_tab();

    // First pass: determine size, legality.
    regcomp_start(عبارت, re_flags);
    regcode = JUST_CALC_SIZE;
    regc(REGMAGIC);
    اگر (reg(REG_NOPAREN, &flags) == پوچ)
	بازگشت پوچ;

    // Allocate space.
    r = alloc(offsetof(bt_regprog_T, program) + regsize);
    اگر (r == پوچ)
	بازگشت پوچ;
    r->re_in_use = FALSE;
#ifdef DEBUG
    r->regsz = regsize;
#endif

    // Second pass: emit code.
    regcomp_start(عبارت, re_flags);
    regcode = r->program;
    regc(REGMAGIC);
    اگر (reg(REG_NOPAREN, &flags) == پوچ || reg_toolong)
    {
	vim_free(r);
	اگر (reg_toolong)
	    EMSG_RET_NULL(_(e_pattern_too_long));
	بازگشت پوچ;
    }

    // Dig out information for optimizations.
    r->regstart = NUL;		// Worst-case defaults.
    r->reganch = 0;
    r->regmust = پوچ;
    r->regmlen = 0;
    r->regflags = regflags;
    اگر (flags & HASNL)
	r->regflags |= RF_HASNL;
    اگر (flags & HASLOOKBH)
	r->regflags |= RF_LOOKBH;
#ifdef FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    r->reghasz = re_has_z;
#endif
    scan = r->program + 1;	// First BRANCH.
    اگر (OP(regnext(scan)) == END)   // Only one top-level choice.
    {
	scan = OPERAND(scan);

	// Starting-point info.
	اگر (OP(scan) == BOL || OP(scan) == RE_BOF)
	{
	    r->reganch++;
	    scan = regnext(scan);
	}

	اگر (OP(scan) == EXACTLY)
	{
	    اگر (has_mbyte)
		r->regstart = (*mb_ptr2char)(OPERAND(scan));
	    وگرنه
		r->regstart = *OPERAND(scan);
	}
	وگرنه اگر ((OP(scan) == BOW
		    || OP(scan) == EOW
		    || OP(scan) == NOTHING
		    || OP(scan) == MOPEN + 0 || OP(scan) == NOPEN
		    || OP(scan) == MCLOSE + 0 || OP(scan) == NCLOSE)
		 && OP(regnext(scan)) == EXACTLY)
	{
	    اگر (has_mbyte)
		r->regstart = (*mb_ptr2char)(OPERAND(regnext(scan)));
	    وگرنه
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
	اگر ((flags & SPSTART || OP(scan) == BOW || OP(scan) == EOW)
							  && !(flags & HASNL))
	{
	    size_t  scanlen;

	    longest = پوچ;
	    len = 0;
	    برای (; scan != پوچ; scan = regnext(scan))
	    {
		اگر (OP(scan) == EXACTLY)
		{
		    scanlen = STRLEN(OPERAND(scan));
		    اگر (scanlen >= (size_t)len)
		    {
			longest = OPERAND(scan);
			len = (صحیح)scanlen;
		    }
		}
	    }
	    r->regmust = longest;
	    r->regmlen = len;
	}
    }
#ifdef BT_REGEXP_DUMP
    regdump(عبارت, r);
#endif
    r->engine = &bt_regengine;
    بازگشت (regprog_T *)r;
}

#اگر defined(FEAT_SYN_HL)
/*
 * Check if during the previous call to vim_regcomp the EOL item "$" has been
 * found.  This is messy, but it works fine.
 */
    صحیح
vim_regcomp_had_eol(تهی)
{
    بازگشت had_eol;
}
#endif

/*
 * Get a number after a backslash that is inside [].
 * When nothing is recognized return a backslash.
 */
    ایستا صحیح
coll_get_char(تهی)
{
    vimlong_T	nr = -1;

    انتخاب (*regparse++)
    {
	حالت 'd': nr = getdecchrs(); بشکن;
	حالت 'o': nr = getoctchrs(); بشکن;
	حالت 'x': nr = gethexchrs(2); بشکن;
	حالت 'u': nr = gethexchrs(4); بشکن;
	حالت 'U': nr = gethexchrs(8); بشکن;
    }
    اگر (nr < 0)
    {
	// If getting the number fails be backwards compatible: the character
	// is a backslash.
	--regparse;
	nr = '\\';
    }
    اگر (nr > INT_MAX)
	nr = INT_MAX;
    بازگشت nr;
}

/*
 * Free a compiled regexp program, returned by bt_regcomp().
 */
    ایستا تهی
bt_regfree(regprog_T *prog)
{
    vim_free(prog);
}

#define ADVANCE_REGINPUT() MB_PTR_ADV(rex.درون_داد)

/*
 * The arguments from BRACE_LIMITS are stored here.  They are actually local
 * to regmatch(), but they are here to reduce the amount of stack space used
 * (it can be called recursively many times).
 */
ایستا long	bl_minval;
ایستا long	bl_maxval;

/*
 * Save the input line and position in a regsave_T.
 */
    ایستا تهی
reg_save(regsave_T *save, garray_T *gap)
{
    اگر (REG_MULTI)
    {
	save->rs_u.pos.col = (colnr_T)(rex.درون_داد - rex.سطر);
	save->rs_u.pos.lnum = rex.lnum;
    }
    وگرنه
	save->rs_u.ptr = rex.درون_داد;
    save->rs_len = gap->ga_len;
}

/*
 * Restore the input line and position from a regsave_T.
 */
    ایستا تهی
reg_restore(regsave_T *save, garray_T *gap)
{
    اگر (REG_MULTI)
    {
	اگر (rex.lnum != save->rs_u.pos.lnum)
	{
	    // only call reg_getline() when the line number changed to save
	    // a bit of time
	    rex.lnum = save->rs_u.pos.lnum;
	    rex.سطر = reg_getline(rex.lnum);
	}
	rex.درون_داد = rex.سطر + save->rs_u.pos.col;
    }
    وگرنه
	rex.درون_داد = save->rs_u.ptr;
    gap->ga_len = save->rs_len;
}

/*
 * Return TRUE if current position is equal to saved position.
 */
    ایستا صحیح
reg_save_equal(regsave_T *save)
{
    اگر (REG_MULTI)
	بازگشت rex.lnum == save->rs_u.pos.lnum
				  && rex.درون_داد == rex.سطر + save->rs_u.pos.col;
    بازگشت rex.درون_داد == save->rs_u.ptr;
}

// Save the sub-expressions before attempting a match.
#define save_se(savep, posp, pp) \
    REG_MULTI ? save_se_multi((savep), (posp)) : save_se_one((savep), (pp))

// After a failed match restore the sub-expressions.
#define restore_se(savep, posp, pp) \
{ \
    اگر (REG_MULTI) \
	*(posp) = (savep)->se_u.pos; \
    وگرنه \
	*(pp) = (savep)->se_u.ptr; \
}

/*
 * Tentatively set the sub-expression start to the current position (after
 * calling regmatch() they will have changed).  Need to save the existing
 * values for when there is no match.
 * Use se_save() to use pointer (save_se_multi()) or position (save_se_one()),
 * depending on REG_MULTI.
 */
    ایستا تهی
save_se_multi(save_se_T *savep, lpos_T *posp)
{
    savep->se_u.pos = *posp;
    posp->lnum = rex.lnum;
    posp->col = (colnr_T)(rex.درون_داد - rex.سطر);
}

    ایستا تهی
save_se_one(save_se_T *savep, char_u **pp)
{
    savep->se_u.ptr = *pp;
    *pp = rex.درون_داد;
}

/*
 * regrepeat - repeatedly match something simple, return how many.
 * Advances rex.input (and rex.lnum) to just after the matched chars.
 */
    ایستا صحیح
regrepeat(
    char_u	*p,
    long	maxcount)   // maximum number of matches allowed
{
    long	شمار = 0;
    char_u	*scan;
    char_u	*opnd;
    صحیح		mask;
    صحیح		testval = 0;

    scan = rex.درون_داد;	    // Make local copy of rex.input for speed.
    opnd = OPERAND(p);
    انتخاب (OP(p))
    {
      حالت ANY:
      حالت ANY + ADD_NL:
	وقتی (شمار < maxcount)
	{
	    // Matching anything means we continue until end-of-line (or
	    // end-of-file for ANY + ADD_NL), only limited by maxcount.
	    وقتی (*scan != NUL && شمار < maxcount)
	    {
		++شمار;
		MB_PTR_ADV(scan);
	    }
	    اگر (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
				      || rex.reg_سطر_lbr || شمار == maxcount)
		بشکن;
	    ++شمار;		// count the line-break
	    reg_nextline();
	    scan = rex.درون_داد;
	    اگر (got_int)
		بشکن;
	}
	بشکن;

      حالت IDENT:
      حالت IDENT + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      حالت SIDENT:
      حالت SIDENT + ADD_NL:
	وقتی (شمار < maxcount)
	{
	    اگر (vim_isIDc(PTR2CHAR(scan)) && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    وگرنه اگر (*scan == NUL)
	    {
		اگر (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_سطر_lbr)
		    بشکن;
		reg_nextline();
		scan = rex.درون_داد;
		اگر (got_int)
		    بشکن;
	    }
	    وگرنه اگر (rex.reg_سطر_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    وگرنه
		بشکن;
	    ++شمار;
	}
	بشکن;

      حالت KWORD:
      حالت KWORD + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      حالت SKWORD:
      حالت SKWORD + ADD_NL:
	وقتی (شمار < maxcount)
	{
	    اگر (vim_iswordp_buf(scan, rex.reg_buf)
					  && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    وگرنه اگر (*scan == NUL)
	    {
		اگر (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_سطر_lbr)
		    بشکن;
		reg_nextline();
		scan = rex.درون_داد;
		اگر (got_int)
		    بشکن;
	    }
	    وگرنه اگر (rex.reg_سطر_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    وگرنه
		بشکن;
	    ++شمار;
	}
	بشکن;

      حالت FNAME:
      حالت FNAME + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      حالت SFNAME:
      حالت SFNAME + ADD_NL:
	وقتی (شمار < maxcount)
	{
	    اگر (vim_isfilec(PTR2CHAR(scan)) && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    وگرنه اگر (*scan == NUL)
	    {
		اگر (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_سطر_lbr)
		    بشکن;
		reg_nextline();
		scan = rex.درون_داد;
		اگر (got_int)
		    بشکن;
	    }
	    وگرنه اگر (rex.reg_سطر_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    وگرنه
		بشکن;
	    ++شمار;
	}
	بشکن;

      حالت PRINT:
      حالت PRINT + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      حالت SPRINT:
      حالت SPRINT + ADD_NL:
	وقتی (شمار < maxcount)
	{
	    اگر (*scan == NUL)
	    {
		اگر (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_سطر_lbr)
		    بشکن;
		reg_nextline();
		scan = rex.درون_داد;
		اگر (got_int)
		    بشکن;
	    }
	    وگرنه اگر (vim_isprintc(PTR2CHAR(scan)) == 1
					  && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    وگرنه اگر (rex.reg_سطر_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    وگرنه
		بشکن;
	    ++شمار;
	}
	بشکن;

      حالت WHITE:
      حالت WHITE + ADD_NL:
	testval = mask = RI_WHITE;
do_class:
	وقتی (شمار < maxcount)
	{
	    صحیح		l;

	    اگر (*scan == NUL)
	    {
		اگر (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_سطر_lbr)
		    بشکن;
		reg_nextline();
		scan = rex.درون_داد;
		اگر (got_int)
		    بشکن;
	    }
	    وگرنه اگر (has_mbyte && (l = (*mb_ptr2len)(scan)) > 1)
	    {
		اگر (testval != 0)
		    بشکن;
		scan += l;
	    }
	    وگرنه اگر ((class_tab[*scan] & mask) == testval)
		++scan;
	    وگرنه اگر (rex.reg_سطر_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    وگرنه
		بشکن;
	    ++شمار;
	}
	بشکن;

      حالت NWHITE:
      حالت NWHITE + ADD_NL:
	mask = RI_WHITE;
	برو do_class;
      حالت DIGIT:
      حالت DIGIT + ADD_NL:
	testval = mask = RI_DIGIT;
	برو do_class;
      حالت NDIGIT:
      حالت NDIGIT + ADD_NL:
	mask = RI_DIGIT;
	برو do_class;
      حالت HEX:
      حالت HEX + ADD_NL:
	testval = mask = RI_HEX;
	برو do_class;
      حالت NHEX:
      حالت NHEX + ADD_NL:
	mask = RI_HEX;
	برو do_class;
      حالت OCTAL:
      حالت OCTAL + ADD_NL:
	testval = mask = RI_OCTAL;
	برو do_class;
      حالت NOCTAL:
      حالت NOCTAL + ADD_NL:
	mask = RI_OCTAL;
	برو do_class;
      حالت WORD:
      حالت WORD + ADD_NL:
	testval = mask = RI_WORD;
	برو do_class;
      حالت NWORD:
      حالت NWORD + ADD_NL:
	mask = RI_WORD;
	برو do_class;
      حالت HEAD:
      حالت HEAD + ADD_NL:
	testval = mask = RI_HEAD;
	برو do_class;
      حالت NHEAD:
      حالت NHEAD + ADD_NL:
	mask = RI_HEAD;
	برو do_class;
      حالت ALPHA:
      حالت ALPHA + ADD_NL:
	testval = mask = RI_ALPHA;
	برو do_class;
      حالت NALPHA:
      حالت NALPHA + ADD_NL:
	mask = RI_ALPHA;
	برو do_class;
      حالت LOWER:
      حالت LOWER + ADD_NL:
	testval = mask = RI_LOWER;
	برو do_class;
      حالت NLOWER:
      حالت NLOWER + ADD_NL:
	mask = RI_LOWER;
	برو do_class;
      حالت UPPER:
      حالت UPPER + ADD_NL:
	testval = mask = RI_UPPER;
	برو do_class;
      حالت NUPPER:
      حالت NUPPER + ADD_NL:
	mask = RI_UPPER;
	برو do_class;

      حالت EXACTLY:
	{
	    صحیح	    cu, cl;

	    // This doesn't do a multi-byte character, because a MULTIBYTECODE
	    // would have been used for it.  It does handle single-byte
	    // characters, such as latin1.
	    اگر (rex.reg_ic)
	    {
		cu = MB_TOUPPER(*opnd);
		cl = MB_TOLOWER(*opnd);
		وقتی (شمار < maxcount && (*scan == cu || *scan == cl))
		{
		    شمار++;
		    scan++;
		}
	    }
	    وگرنه
	    {
		cu = *opnd;
		وقتی (شمار < maxcount && *scan == cu)
		{
		    شمار++;
		    scan++;
		}
	    }
	    بشکن;
	}

      حالت MULTIBYTECODE:
	{
	    صحیح		i, len, cf = 0;

	    // Safety check (just in case 'encoding' was changed since
	    // compiling the program).
	    اگر ((len = (*mb_ptr2len)(opnd)) > 1)
	    {
		اگر (rex.reg_ic && enc_utf8)
		    cf = utf_fold(utf_ptr2char(opnd));
		وقتی (شمار < maxcount && (*mb_ptr2len)(scan) >= len)
		{
		    برای (i = 0; i < len; ++i)
			اگر (opnd[i] != scan[i])
			    بشکن;
		    اگر (i < len && (!rex.reg_ic || !enc_utf8
					|| utf_fold(utf_ptr2char(scan)) != cf))
			بشکن;
		    scan += len;
		    ++شمار;
		}
	    }
	}
	بشکن;

      حالت ANYOF:
      حالت ANYOF + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH

      حالت ANYBUT:
      حالت ANYBUT + ADD_NL:
	وقتی (شمار < maxcount)
	{
	    صحیح len;

	    اگر (*scan == NUL)
	    {
		اگر (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_سطر_lbr)
		    بشکن;
		reg_nextline();
		scan = rex.درون_داد;
		اگر (got_int)
		    بشکن;
	    }
	    وگرنه اگر (rex.reg_سطر_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    وگرنه اگر (has_mbyte && (len = (*mb_ptr2len)(scan)) > 1)
	    {
		اگر ((cstrchr(opnd, (*mb_ptr2char)(scan)) == پوچ) == testval)
		    بشکن;
		scan += len;
	    }
	    وگرنه
	    {
		اگر ((cstrchr(opnd, *scan) == پوچ) == testval)
		    بشکن;
		++scan;
	    }
	    ++شمار;
	}
	بشکن;

      حالت NEWL:
	وقتی (شمار < maxcount
		&& ((*scan == NUL && rex.lnum <= rex.reg_maxline
				       && !rex.reg_سطر_lbr && REG_MULTI)
		    || (*scan == '\n' && rex.reg_سطر_lbr)))
	{
	    شمار++;
	    اگر (rex.reg_سطر_lbr)
		ADVANCE_REGINPUT();
	    وگرنه
		reg_nextline();
	    scan = rex.درون_داد;
	    اگر (got_int)
		بشکن;
	}
	بشکن;

      پیشفرض:			// Oh dear.  Called inappropriately.
	iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	printf("Called regrepeat with op code %d\n", OP(p));
#endif
	بشکن;
    }

    rex.درون_داد = scan;

    بازگشت (صحیح)شمار;
}

/*
 * Push an item onto the regstack.
 * Returns pointer to new item.  Returns NULL when out of memory.
 */
    ایستا regitem_T *
regstack_افزودن(regstate_T state, char_u *scan)
{
    regitem_T	*rp;

    اگر ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
    {
	emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
	بازگشت پوچ;
    }
    اگر (GA_GROW_FAILS(&regstack, (صحیح)اندازه(regitem_T)))
	بازگشت پوچ;

    rp = (regitem_T *)((نویسه *)regstack.ga_data + regstack.ga_len);
    rp->rs_state = state;
    rp->rs_scan = scan;

    regstack.ga_len += اندازه(regitem_T);
    بازگشت rp;
}

/*
 * Pop an item from the regstack.
 */
    ایستا تهی
regstack_برداشت(char_u **scan)
{
    regitem_T	*rp;

    rp = (regitem_T *)((نویسه *)regstack.ga_data + regstack.ga_len) - 1;
    *scan = rp->rs_scan;

    regstack.ga_len -= اندازه(regitem_T);
}

#ifdef FEAT_RELTIME
/*
 * Check if the timer expired, return TRUE if so.
 */
    ایستا صحیح
bt_did_time_out(صحیح *timed_out)
{
    اگر (*timeout_flag)
    {
	اگر (timed_out != پوچ)
	{
# ifdef FEAT_EVAL
	    اگر (!*timed_out)
		ch_log(پوچ, "BT regexp timed out");
# endif
	    *timed_out = TRUE;
	}
	بازگشت TRUE;
    }
    بازگشت FALSE;
}
#endif

/*
 * Save the current subexpr to "bp", so that they can be restored
 * later by restore_subexpr().
 */
    ایستا تهی
save_subexpr(regbehind_T *bp)
{
    صحیح i;

    // When "rex.need_clear_subexpr" is set we don't need to save the values,
    // only remember that this flag needs to be set again when restoring.
    bp->save_need_clear_subexpr = rex.need_clear_subexpr;
    اگر (rex.need_clear_subexpr)
	بازگشت;

    برای (i = 0; i < NSUBEXP; ++i)
    {
	اگر (REG_MULTI)
	{
	    bp->save_start[i].se_u.pos = rex.reg_startpos[i];
	    bp->save_end[i].se_u.pos = rex.reg_endpos[i];
	}
	وگرنه
	{
	    bp->save_start[i].se_u.ptr = rex.reg_startp[i];
	    bp->save_end[i].se_u.ptr = rex.reg_endp[i];
	}
    }
}

/*
 * Restore the subexpr from "bp".
 */
    ایستا تهی
restore_subexpr(regbehind_T *bp)
{
    صحیح i;

    // Only need to restore saved values when they are not to be cleared.
    rex.need_clear_subexpr = bp->save_need_clear_subexpr;
    اگر (rex.need_clear_subexpr)
	بازگشت;

    برای (i = 0; i < NSUBEXP; ++i)
    {
	اگر (REG_MULTI)
	{
	    rex.reg_startpos[i] = bp->save_start[i].se_u.pos;
	    rex.reg_endpos[i] = bp->save_end[i].se_u.pos;
	}
	وگرنه
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
    ایستا صحیح
regmatch(
    char_u	*scan,		    // Current node.
    صحیح		*timed_out UNUSED)  // flag set on timeout or NULL
{
  char_u	*next;		// Next node.
  صحیح		op;
  صحیح		c;
  regitem_T	*rp;
  صحیح		no;
  صحیح		status;		// one of the RA_ values:

  // Make "regstack" and "backpos" empty.  They are allocated and freed in
  // bt_regexec_both() to reduce malloc()/free() calls.
  regstack.ga_len = 0;
  backpos.ga_len = 0;

  // Repeat until "regstack" is empty.
  برای (;;)
  {
    // Some patterns may take a long time to match, e.g., "\([a-z]\+\)\+Q".
    // Allow interrupting them with CTRL-C.
    fast_breakcheck();

#ifdef DEBUG
    اگر (scan != پوچ && regnarrate)
    {
	mch_errmsg((نویسه *)regprop(scan));
	mch_errmsg("(\n");
    }
#endif

    // Repeat for items that can be matched sequentially, without using the
    // regstack.
    برای (;;)
    {
	اگر (got_int || scan == پوچ)
	{
	    status = RA_FAIL;
	    بشکن;
	}
#ifdef FEAT_RELTIME
	اگر (bt_did_time_out(timed_out))
	{
	    status = RA_FAIL;
	    بشکن;
	}
#endif
	status = RA_CONT;

#ifdef DEBUG
	اگر (regnarrate)
	{
	    mch_errmsg((نویسه *)regprop(scan));
	    mch_errmsg("...\n");
# ifdef FEAT_SYN_HL
	    اگر (re_extmatch_in != پوچ)
	    {
		صحیح i;

		mch_errmsg(_("External submatches:\n"));
		برای (i = 0; i < NSUBEXP; i++)
		{
		    mch_errmsg("    \"");
		    اگر (re_extmatch_in->matches[i] != پوچ)
			mch_errmsg((نویسه *)re_extmatch_in->matches[i]);
		    mch_errmsg("\"\n");
		}
	    }
# endif
	}
#endif
	next = regnext(scan);

	op = OP(scan);
	// Check for character class with NL added.
	اگر (!rex.reg_سطر_lbr && WITH_NL(op) && REG_MULTI
			     && *rex.درون_داد == NUL && rex.lnum <= rex.reg_maxline)
	{
	    reg_nextline();
	}
	وگرنه اگر (rex.reg_سطر_lbr && WITH_NL(op) && *rex.درون_داد == '\n')
	{
	    ADVANCE_REGINPUT();
	}
	وگرنه
	{
	  اگر (WITH_NL(op))
	      op -= ADD_NL;
	  اگر (has_mbyte)
	      c = (*mb_ptr2char)(rex.درون_داد);
	  وگرنه
	      c = *rex.درون_داد;
	  انتخاب (op)
	  {
	  حالت BOL:
	    اگر (rex.درون_داد != rex.سطر)
		status = RA_NOMATCH;
	    بشکن;

	  حالت EOL:
	    اگر (c != NUL)
		status = RA_NOMATCH;
	    بشکن;

	  حالت RE_BOF:
	    // We're not at the beginning of the file when below the first
	    // line where we started, not at the start of the line or we
	    // didn't start at the first line of the buffer.
	    اگر (rex.lnum != 0 || rex.درون_داد != rex.سطر
				       || (REG_MULTI && rex.reg_firstlnum > 1))
		status = RA_NOMATCH;
	    بشکن;

	  حالت RE_EOF:
	    اگر (rex.lnum != rex.reg_maxline || c != NUL)
		status = RA_NOMATCH;
	    بشکن;

	  حالت CURSOR:
	    // Check if the buffer is in a window and compare the
	    // rex.reg_win->w_cursor position to the match position.
	    اگر (rex.reg_win == پوچ
		    || (rex.lnum + rex.reg_firstlnum
						 != rex.reg_win->w_cursor.lnum)
		    || ((colnr_T)(rex.درون_داد - rex.سطر)
						 != rex.reg_win->w_cursor.col))
		status = RA_NOMATCH;
	    بشکن;

	  حالت RE_MARK:
	    // Compare the mark position to the match position.
	    {
		صحیح	mark = OPERAND(scan)[0];
		صحیح	cmp = OPERAND(scan)[1];
		pos_T	*pos;
		size_t	col = REG_MULTI ? rex.درون_داد - rex.سطر : 0;

		pos = getmark_buf(rex.reg_buf, mark, FALSE);

		// Line may have been freed, get it again.
		اگر (REG_MULTI)
		{
		    rex.سطر = reg_getline(rex.lnum);
		    rex.درون_داد = rex.سطر + col;
		}

		اگر (pos == پوچ		     // mark doesn't exist
			|| pos->lnum <= 0)   // mark isn't set in reg_buf
		{
		    status = RA_NOMATCH;
		}
		وگرنه
		{
		    colnr_T pos_col = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->col == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->col;

		    اگر ((pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_col == (colnr_T)(rex.درون_داد - rex.سطر)
				    ? (cmp == '<' || cmp == '>')
				    : (pos_col < (colnr_T)(rex.درون_داد - rex.سطر)
					? cmp != '>'
					: cmp != '<'))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? cmp != '>'
				    : cmp != '<')))
		    status = RA_NOMATCH;
		}
	    }
	    بشکن;

	  حالت RE_VISUAL:
	    اگر (!reg_match_visual())
		status = RA_NOMATCH;
	    بشکن;

	  حالت RE_LNUM:
	    اگر (!REG_MULTI || !re_رقم_cmp((long_u)(rex.lnum + rex.reg_firstlnum),
									scan))
		status = RA_NOMATCH;
	    بشکن;

	  حالت RE_COL:
	    اگر (!re_رقم_cmp((long_u)(rex.درون_داد - rex.سطر) + 1, scan))
		status = RA_NOMATCH;
	    بشکن;

	  حالت RE_VCOL:
	    {
		win_T	    *wp = rex.reg_win == پوچ ? curwin : rex.reg_win;
		linenr_T    lnum = REG_MULTI ? rex.reg_firstlnum + rex.lnum : 1;
		long_u	    vcol;

		اگر (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_buffer->b_ml.ml_سطر_شمار))
		    lnum = 1;
		vcol = (long_u)win_linetabsize(wp, lnum, rex.سطر,
					      (colnr_T)(rex.درون_داد - rex.سطر));
		اگر (!re_رقم_cmp(vcol + 1, scan))
		    status = RA_NOMATCH;
	    }
	    بشکن;

	  حالت BOW:	// \<word; rex.input points to w
	    اگر (c == NUL)	// Can't match at end of line
		status = RA_NOMATCH;
	    وگرنه اگر (has_mbyte)
	    {
		صحیح this_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.درون_داد, rex.reg_buf);
		اگر (this_class <= 1)
		    status = RA_NOMATCH;  // not on a word at all
		وگرنه اگر (reg_prev_class() == this_class)
		    status = RA_NOMATCH;  // previous char is in same word
	    }
	    وگرنه
	    {
		اگر (!vim_iswordc_buf(c, rex.reg_buf) || (rex.درون_داد > rex.سطر
				&& vim_iswordc_buf(rex.درون_داد[-1], rex.reg_buf)))
		    status = RA_NOMATCH;
	    }
	    بشکن;

	  حالت EOW:	// word\>; rex.input points after d
	    اگر (rex.درون_داد == rex.سطر)    // Can't match at start of line
		status = RA_NOMATCH;
	    وگرنه اگر (has_mbyte)
	    {
		صحیح this_class, prev_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.درون_داد, rex.reg_buf);
		prev_class = reg_prev_class();
		اگر (this_class == prev_class
			|| prev_class == 0 || prev_class == 1)
		    status = RA_NOMATCH;
	    }
	    وگرنه
	    {
		اگر (!vim_iswordc_buf(rex.درون_داد[-1], rex.reg_buf)
			|| (rex.درون_داد[0] != NUL
					   && vim_iswordc_buf(c, rex.reg_buf)))
		    status = RA_NOMATCH;
	    }
	    بشکن; // Matched with EOW

	  حالت ANY:
	    // ANY does not match new lines.
	    اگر (c == NUL)
		status = RA_NOMATCH;
	    وگرنه
		ADVANCE_REGINPUT();
	    بشکن;

	  حالت IDENT:
	    اگر (!vim_isIDc(c))
		status = RA_NOMATCH;
	    وگرنه
		ADVANCE_REGINPUT();
	    بشکن;

	  حالت SIDENT:
	    اگر (VIM_ISDIGIT(*rex.درون_داد) || !vim_isIDc(c))
		status = RA_NOMATCH;
	    وگرنه
		ADVANCE_REGINPUT();
	    بشکن;

	  حالت KWORD:
	    اگر (!vim_iswordp_buf(rex.درون_داد, rex.reg_buf))
		status = RA_NOMATCH;
	    وگرنه
		ADVANCE_REGINPUT();
	    بشکن;

	  حالت SKWORD:
	    اگر (VIM_ISDIGIT(*rex.درون_داد)
				    || !vim_iswordp_buf(rex.درون_داد, rex.reg_buf))
		status = RA_NOMATCH;
	    وگرنه
		ADVANCE_REGINPUT();
	    بشکن;

	  حالت FNAME:
	    اگر (!vim_isfilec(c))
		status = RA_NOMATCH;
	    وگرنه
		ADVANCE_REGINPUT();
	    بشکن;

	  حالت SFNAME:
	    اگر (VIM_ISDIGIT(*rex.درون_داد) || !vim_isfilec(c))
		status = RA_NOMATCH;
	    وگرنه
		ADVANCE_REGINPUT();
	    بشکن;

	  حالت PRINT:
	    اگر (!vim_isprintc(PTR2CHAR(rex.درون_داد)))
		status = RA_NOMATCH;
	    وگرنه
		ADVANCE_REGINPUT();
	    بشکن;

	  حالت SPRINT:
	    اگر (VIM_ISDIGIT(*rex.درون_داد) || !vim_isprintc(PTR2CHAR(rex.درون_داد)))
		status = RA_NOMATCH;
	    وگرنه
		ADVANCE_REGINPUT();
	    بشکن;

	  حالت WHITE:
	    اگر (!VIM_ISWHITE(c))
		status = RA_NOMATCH;
	    وگرنه
		ADVANCE_REGINPUT();
	    بشکن;

	  حالت NWHITE:
	    اگر (c == NUL || VIM_ISWHITE(c))
		status = RA_NOMATCH;
	    وگرنه
		ADVANCE_REGINPUT();
	    بشکن;

	  حالت DIGIT:
	    اگر (!ri_digit(c))
		status = RA_NOMATCH;
	    وگرنه
		ADVANCE_REGINPUT();
	    بشکن;

	  حالت NDIGIT:
	    اگر (c == NUL || ri_digit(c))
		status = RA_NOMATCH;
	    وگرنه
		ADVANCE_REGINPUT();
	    بشکن;

	  حالت HEX:
	    اگر (!ri_hex(c))
		status = RA_NOMATCH;
	    وگرنه
		ADVANCE_REGINPUT();
	    بشکن;

	  حالت NHEX:
	    اگر (c == NUL || ri_hex(c))
		status = RA_NOMATCH;
	    وگرنه
		ADVANCE_REGINPUT();
	    بشکن;

	  حالت OCTAL:
	    اگر (!ri_octal(c))
		status = RA_NOMATCH;
	    وگرنه
		ADVANCE_REGINPUT();
	    بشکن;

	  حالت NOCTAL:
	    اگر (c == NUL || ri_octal(c))
		status = RA_NOMATCH;
	    وگرنه
		ADVANCE_REGINPUT();
	    بشکن;

	  حالت WORD:
	    اگر (!ri_word(c))
		status = RA_NOMATCH;
	    وگرنه
		ADVANCE_REGINPUT();
	    بشکن;

	  حالت NWORD:
	    اگر (c == NUL || ri_word(c))
		status = RA_NOMATCH;
	    وگرنه
		ADVANCE_REGINPUT();
	    بشکن;

	  حالت HEAD:
	    اگر (!ri_head(c))
		status = RA_NOMATCH;
	    وگرنه
		ADVANCE_REGINPUT();
	    بشکن;

	  حالت NHEAD:
	    اگر (c == NUL || ri_head(c))
		status = RA_NOMATCH;
	    وگرنه
		ADVANCE_REGINPUT();
	    بشکن;

	  حالت ALPHA:
	    اگر (!ri_alpha(c))
		status = RA_NOMATCH;
	    وگرنه
		ADVANCE_REGINPUT();
	    بشکن;

	  حالت NALPHA:
	    اگر (c == NUL || ri_alpha(c))
		status = RA_NOMATCH;
	    وگرنه
		ADVANCE_REGINPUT();
	    بشکن;

	  حالت LOWER:
	    اگر (!ri_lower(c))
		status = RA_NOMATCH;
	    وگرنه
		ADVANCE_REGINPUT();
	    بشکن;

	  حالت NLOWER:
	    اگر (c == NUL || ri_lower(c))
		status = RA_NOMATCH;
	    وگرنه
		ADVANCE_REGINPUT();
	    بشکن;

	  حالت UPPER:
	    اگر (!ri_upper(c))
		status = RA_NOMATCH;
	    وگرنه
		ADVANCE_REGINPUT();
	    بشکن;

	  حالت NUPPER:
	    اگر (c == NUL || ri_upper(c))
		status = RA_NOMATCH;
	    وگرنه
		ADVANCE_REGINPUT();
	    بشکن;

	  حالت EXACTLY:
	    {
		صحیح	len;
		char_u	*opnd;

		opnd = OPERAND(scan);
		// Inline the first byte, for speed.
		اگر (*opnd != *rex.درون_داد
			&& (!rex.reg_ic
			    || (!enc_utf8
			      && MB_TOLOWER(*opnd) != MB_TOLOWER(*rex.درون_داد))))
		    status = RA_NOMATCH;
		وگرنه اگر (*opnd == NUL)
		{
		    // match empty string always works; happens when "~" is
		    // empty.
		}
		وگرنه
		{
		    اگر (opnd[1] == NUL && !(enc_utf8 && rex.reg_ic))
		    {
			len = 1;	// matched a single byte above
		    }
		    وگرنه
		    {
			// Need to match first byte again for multi-byte.
			len = (صحیح)STRLEN(opnd);
			اگر (cstrncmp(opnd, rex.درون_داد, &len) != 0)
			    status = RA_NOMATCH;
		    }
		    // Check for following composing character, unless %C
		    // follows (skips over all composing chars).
		    اگر (status != RA_NOMATCH
			    && enc_utf8
			    && UTF_COMPOSINGLIKE(rex.درون_داد, rex.درون_داد + len)
			    && !rex.reg_icombine
			    && OP(next) != RE_COMPOSING)
		    {
			// raaron: This code makes a composing character get
			// ignored, which is the correct behavior (sometimes)
			// for voweled Hebrew texts.
			status = RA_NOMATCH;
		    }
		    اگر (status != RA_NOMATCH)
			rex.درون_داد += len;
		}
	    }
	    بشکن;

	  حالت ANYOF:
	  حالت ANYBUT:
	    {
		char_u  *q = OPERAND(scan);

		اگر (c == NUL)
		    status = RA_NOMATCH;
		وگرنه اگر ((cstrchr(q, c) == پوچ) == (op == ANYOF))
		    status = RA_NOMATCH;
		وگرنه
		{
		    // Check following combining characters
		    صحیح	len = 0;
		    صحیح i;

		    اگر (enc_utf8)
			len = utfc_ptr2len(q) - utf_ptr2len(q);

		    MB_CPTR_ADV(rex.درون_داد);
		    MB_CPTR_ADV(q);

		    اگر (!enc_utf8 || len == 0)
			بشکن;

		    برای (i = 0; i < len; ++i)
			اگر (q[i] != rex.درون_داد[i])
			{
			    status = RA_NOMATCH;
			    بشکن;
			}
		    rex.درون_داد += len;
		}
		بشکن;
	    }

	  حالت MULTIBYTECODE:
	    اگر (has_mbyte)
	    {
		صحیح	i, len;
		char_u	*opnd;
		صحیح	opndc = 0, inpc;

		opnd = OPERAND(scan);
		// Safety check (just in case 'encoding' was changed since
		// compiling the program).
		اگر ((len = (*mb_ptr2len)(opnd)) < 2)
		{
		    status = RA_NOMATCH;
		    بشکن;
		}
		اگر (enc_utf8)
		    opndc = utf_ptr2char(opnd);
		اگر (enc_utf8 && utf_iscomposing(opndc))
		{
		    // When only a composing char is given match at any
		    // position where that composing char appears.
		    status = RA_NOMATCH;
		    برای (i = 0; rex.درون_داد[i] != NUL;
						i += utf_ptr2len(rex.درون_داد + i))
		    {
			inpc = utf_ptr2char(rex.درون_داد + i);
			اگر (!utf_iscomposing(inpc))
			{
			    اگر (i > 0)
				بشکن;
			}
			وگرنه اگر (opndc == inpc)
			{
			    // Include all following composing chars.
			    len = i + utfc_ptr2len(rex.درون_داد + i);
			    status = RA_MATCH;
			    بشکن;
			}
		    }
		}
		وگرنه اگر (enc_utf8)
		{
		    اگر (cstrncmp(opnd, rex.درون_داد, &len) != 0)
		    {
			status = RA_NOMATCH;
			بشکن;
		    }
		}
		وگرنه
		    برای (i = 0; i < len; ++i)
			اگر (opnd[i] != rex.درون_داد[i])
			{
			    status = RA_NOMATCH;
			    بشکن;
			}
		rex.درون_داد += len;
	    }
	    وگرنه
		status = RA_NOMATCH;
	    بشکن;
	  حالت RE_COMPOSING:
	    اگر (enc_utf8)
	    {
		// Skip composing characters.
		وقتی (utf_iscomposing(utf_ptr2char(rex.درون_داد)))
		    MB_CPTR_ADV(rex.درون_داد);
	    }
	    بشکن;

	  حالت NOTHING:
	    بشکن;

	  حالت BACK:
	    {
		صحیح		i;
		backpos_T	*bp;

		// When we run into BACK we need to check if we don't keep
		// looping without matching any input.  The second and later
		// times a BACK is encountered it fails if the input is still
		// at the same position as the previous time.
		// The positions are stored in "backpos" and found by the
		// current value of "scan", the position in the RE program.
		bp = (backpos_T *)backpos.ga_data;
		برای (i = 0; i < backpos.ga_len; ++i)
		    اگر (bp[i].bp_scan == scan)
			بشکن;
		اگر (i == backpos.ga_len)
		{
		    // First time at this BACK, make room to store the pos.
		    اگر (GA_GROW_FAILS(&backpos, 1))
			status = RA_FAIL;
		    وگرنه
		    {
			// get "ga_data" again, it may have changed
			bp = (backpos_T *)backpos.ga_data;
			bp[i].bp_scan = scan;
			++backpos.ga_len;
		    }
		}
		وگرنه اگر (reg_save_equal(&bp[i].bp_pos))
		    // Still at same position as last time, fail.
		    status = RA_NOMATCH;

		اگر (status != RA_FAIL && status != RA_NOMATCH)
		    reg_save(&bp[i].bp_pos, &backpos);
	    }
	    بشکن;

	  حالت MOPEN + 0:   // Match start: \zs
	  حالت MOPEN + 1:   // \(
	  حالت MOPEN + 2:
	  حالت MOPEN + 3:
	  حالت MOPEN + 4:
	  حالت MOPEN + 5:
	  حالت MOPEN + 6:
	  حالت MOPEN + 7:
	  حالت MOPEN + 8:
	  حالت MOPEN + 9:
	    {
		no = op - MOPEN;
		cleanup_subexpr();
		rp = regstack_افزودن(RS_MOPEN, scan);
		اگر (rp == پوچ)
		    status = RA_FAIL;
		وگرنه
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_startpos[no],
							  &rex.reg_startp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    بشکن;

	  حالت NOPEN:	    // \%(
	  حالت NCLOSE:	    // \) after \%(
		اگر (regstack_افزودن(RS_NOPEN, scan) == پوچ)
		    status = RA_FAIL;
		// We simply continue and handle the result when done.
		بشکن;

#ifdef FEAT_SYN_HL
	  حالت ZOPEN + 1:
	  حالت ZOPEN + 2:
	  حالت ZOPEN + 3:
	  حالت ZOPEN + 4:
	  حالت ZOPEN + 5:
	  حالت ZOPEN + 6:
	  حالت ZOPEN + 7:
	  حالت ZOPEN + 8:
	  حالت ZOPEN + 9:
	    {
		no = op - ZOPEN;
		cleanup_zsubexpr();
		rp = regstack_افزودن(RS_ZOPEN, scan);
		اگر (rp == پوچ)
		    status = RA_FAIL;
		وگرنه
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_startzpos[no],
							     &reg_startzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    بشکن;
#endif

	  حالت MCLOSE + 0:  // Match end: \ze
	  حالت MCLOSE + 1:  // \)
	  حالت MCLOSE + 2:
	  حالت MCLOSE + 3:
	  حالت MCLOSE + 4:
	  حالت MCLOSE + 5:
	  حالت MCLOSE + 6:
	  حالت MCLOSE + 7:
	  حالت MCLOSE + 8:
	  حالت MCLOSE + 9:
	    {
		no = op - MCLOSE;
		cleanup_subexpr();
		rp = regstack_افزودن(RS_MCLOSE, scan);
		اگر (rp == پوچ)
		    status = RA_FAIL;
		وگرنه
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_endpos[no],
							    &rex.reg_endp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    بشکن;

#ifdef FEAT_SYN_HL
	  حالت ZCLOSE + 1:  // \) after \z(
	  حالت ZCLOSE + 2:
	  حالت ZCLOSE + 3:
	  حالت ZCLOSE + 4:
	  حالت ZCLOSE + 5:
	  حالت ZCLOSE + 6:
	  حالت ZCLOSE + 7:
	  حالت ZCLOSE + 8:
	  حالت ZCLOSE + 9:
	    {
		no = op - ZCLOSE;
		cleanup_zsubexpr();
		rp = regstack_افزودن(RS_ZCLOSE, scan);
		اگر (rp == پوچ)
		    status = RA_FAIL;
		وگرنه
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_endzpos[no],
							      &reg_endzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    بشکن;
#endif

	  حالت BACKREF + 1:
	  حالت BACKREF + 2:
	  حالت BACKREF + 3:
	  حالت BACKREF + 4:
	  حالت BACKREF + 5:
	  حالت BACKREF + 6:
	  حالت BACKREF + 7:
	  حالت BACKREF + 8:
	  حالت BACKREF + 9:
	    {
		صحیح		len;

		no = op - BACKREF;
		cleanup_subexpr();
		اگر (!REG_MULTI)		// Single-line regexp
		{
		    اگر (rex.reg_startp[no] == پوچ || rex.reg_endp[no] == پوچ)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    وگرنه
		    {
			// Compare current input with back-ref in the same
			// line.
			len = (صحیح)(rex.reg_endp[no] - rex.reg_startp[no]);
			اگر (cstrncmp(rex.reg_startp[no], rex.درون_داد, &len) != 0)
			    status = RA_NOMATCH;
		    }
		}
		وگرنه				// Multi-line regexp
		{
		    اگر (rex.reg_startpos[no].lnum < 0
						|| rex.reg_endpos[no].lnum < 0)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    وگرنه
		    {
			اگر (rex.reg_startpos[no].lnum == rex.lnum
				&& rex.reg_endpos[no].lnum == rex.lnum)
			{
			    // Compare back-ref within the current line.
			    len = rex.reg_endpos[no].col
						    - rex.reg_startpos[no].col;
			    اگر (cstrncmp(rex.سطر + rex.reg_startpos[no].col,
							  rex.درون_داد, &len) != 0)
				status = RA_NOMATCH;
			}
			وگرنه
			{
			    // Messy situation: Need to compare between two
			    // lines.
			    صحیح r = match_with_backref(
					    rex.reg_startpos[no].lnum,
					    rex.reg_startpos[no].col,
					    rex.reg_endpos[no].lnum,
					    rex.reg_endpos[no].col,
					    &len);

			    اگر (r != RA_MATCH)
				status = r;
			}
		    }
		}

		// Matched the backref, skip over it.
		rex.درون_داد += len;
	    }
	    بشکن;

#ifdef FEAT_SYN_HL
	  حالت ZREF + 1:
	  حالت ZREF + 2:
	  حالت ZREF + 3:
	  حالت ZREF + 4:
	  حالت ZREF + 5:
	  حالت ZREF + 6:
	  حالت ZREF + 7:
	  حالت ZREF + 8:
	  حالت ZREF + 9:
	    {
		صحیح	len;

		cleanup_zsubexpr();
		no = op - ZREF;
		اگر (re_extmatch_in != پوچ
			&& re_extmatch_in->matches[no] != پوچ)
		{
		    len = (صحیح)STRLEN(re_extmatch_in->matches[no]);
		    اگر (cstrncmp(re_extmatch_in->matches[no],
							  rex.درون_داد, &len) != 0)
			status = RA_NOMATCH;
		    وگرنه
			rex.درون_داد += len;
		}
		وگرنه
		{
		    // Backref was not set: Match an empty string.
		}
	    }
	    بشکن;
#endif

	  حالت BRANCH:
	    {
		اگر (OP(next) != BRANCH) // No choice.
		    next = OPERAND(scan);	// Avoid recursion.
		وگرنه
		{
		    rp = regstack_افزودن(RS_BRANCH, scan);
		    اگر (rp == پوچ)
			status = RA_FAIL;
		    وگرنه
			status = RA_BREAK;	// rest is below
		}
	    }
	    بشکن;

	  حالت BRACE_LIMITS:
	    {
		اگر (OP(next) == BRACE_SIMPLE)
		{
		    bl_minval = OPERAND_MIN(scan);
		    bl_maxval = OPERAND_MAX(scan);
		}
		وگرنه اگر (OP(next) >= BRACE_COMPLEX
			&& OP(next) < BRACE_COMPLEX + 10)
		{
		    no = OP(next) - BRACE_COMPLEX;
		    brace_min[no] = OPERAND_MIN(scan);
		    brace_max[no] = OPERAND_MAX(scan);
		    brace_شمار[no] = 0;
		}
		وگرنه
		{
		    internal_error("BRACE_LIMITS");
		    status = RA_FAIL;
		}
	    }
	    بشکن;

	  حالت BRACE_COMPLEX + 0:
	  حالت BRACE_COMPLEX + 1:
	  حالت BRACE_COMPLEX + 2:
	  حالت BRACE_COMPLEX + 3:
	  حالت BRACE_COMPLEX + 4:
	  حالت BRACE_COMPLEX + 5:
	  حالت BRACE_COMPLEX + 6:
	  حالت BRACE_COMPLEX + 7:
	  حالت BRACE_COMPLEX + 8:
	  حالت BRACE_COMPLEX + 9:
	    {
		no = op - BRACE_COMPLEX;
		++brace_شمار[no];

		// If not matched enough times yet, try one more
		اگر (brace_شمار[no] <= (brace_min[no] <= brace_max[no]
					     ? brace_min[no] : brace_max[no]))
		{
		    rp = regstack_افزودن(RS_BRCPLX_MORE, scan);
		    اگر (rp == پوچ)
			status = RA_FAIL;
		    وگرنه
		    {
			rp->rs_no = no;
			reg_save(&rp->rs_un.regsave, &backpos);
			next = OPERAND(scan);
			// We continue and handle the result when done.
		    }
		    بشکن;
		}

		// If matched enough times, may try matching some more
		اگر (brace_min[no] <= brace_max[no])
		{
		    // Range is the normal way around, use longest match
		    اگر (brace_شمار[no] <= brace_max[no])
		    {
			rp = regstack_افزودن(RS_BRCPLX_LONG, scan);
			اگر (rp == پوچ)
			    status = RA_FAIL;
			وگرنه
			{
			    rp->rs_no = no;
			    reg_save(&rp->rs_un.regsave, &backpos);
			    next = OPERAND(scan);
			    // We continue and handle the result when done.
			}
		    }
		}
		وگرنه
		{
		    // Range is backwards, use shortest match first
		    اگر (brace_شمار[no] <= brace_min[no])
		    {
			rp = regstack_افزودن(RS_BRCPLX_SHORT, scan);
			اگر (rp == پوچ)
			    status = RA_FAIL;
			وگرنه
			{
			    reg_save(&rp->rs_un.regsave, &backpos);
			    // We continue and handle the result when done.
			}
		    }
		}
	    }
	    بشکن;

	  حالت BRACE_SIMPLE:
	  حالت STAR:
	  حالت PLUS:
	    {
		regstar_T	rst;

		// Lookahead to avoid useless match attempts when we know
		// what character comes next.
		اگر (OP(next) == EXACTLY)
		{
		    rst.nextb = *OPERAND(next);
		    اگر (rex.reg_ic)
		    {
			اگر (MB_ISUPPER(rst.nextb))
			    rst.nextb_ic = MB_TOLOWER(rst.nextb);
			وگرنه
			    rst.nextb_ic = MB_TOUPPER(rst.nextb);
		    }
		    وگرنه
			rst.nextb_ic = rst.nextb;
		}
		وگرنه
		{
		    rst.nextb = NUL;
		    rst.nextb_ic = NUL;
		}
		اگر (op != BRACE_SIMPLE)
		{
		    rst.minval = (op == STAR) ? 0 : 1;
		    rst.maxval = MAX_LIMIT;
		}
		وگرنه
		{
		    rst.minval = bl_minval;
		    rst.maxval = bl_maxval;
		}

		// When maxval > minval, try matching as much as possible, up
		// to maxval.  When maxval < minval, try matching at least the
		// minimal number (since the range is backwards, that's also
		// maxval!).
		rst.شمار = regrepeat(OPERAND(scan), rst.maxval);
		اگر (got_int)
		{
		    status = RA_FAIL;
		    بشکن;
		}
		اگر (rst.minval <= rst.maxval
			  ? rst.شمار >= rst.minval : rst.شمار >= rst.maxval)
		{
		    // It could match.  Prepare for trying to match what
		    // follows.  The code is below.  Parameters are stored in
		    // a regstar_T on the regstack.
		    اگر ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
		    {
			emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
			status = RA_FAIL;
		    }
		    وگرنه اگر (GA_GROW_FAILS(&regstack, (صحیح)اندازه(regstar_T)))
			status = RA_FAIL;
		    وگرنه
		    {
			regstack.ga_len += اندازه(regstar_T);
			rp = regstack_افزودن(rst.minval <= rst.maxval
					? RS_STAR_LONG : RS_STAR_SHORT, scan);
			اگر (rp == پوچ)
			    status = RA_FAIL;
			وگرنه
			{
			    *(((regstar_T *)rp) - 1) = rst;
			    status = RA_BREAK;	    // skip the restore bits
			}
		    }
		}
		وگرنه
		    status = RA_NOMATCH;

	    }
	    بشکن;

	  حالت NOMATCH:
	  حالت MATCH:
	  حالت SUBPAT:
	    rp = regstack_افزودن(RS_NOMATCH, scan);
	    اگر (rp == پوچ)
		status = RA_FAIL;
	    وگرنه
	    {
		rp->rs_no = op;
		reg_save(&rp->rs_un.regsave, &backpos);
		next = OPERAND(scan);
		// We continue and handle the result when done.
	    }
	    بشکن;

	  حالت BEHIND:
	  حالت NOBEHIND:
	    // Need a bit of room to store extra positions.
	    اگر ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
	    {
		emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		status = RA_FAIL;
	    }
	    وگرنه اگر (GA_GROW_FAILS(&regstack, (صحیح)اندازه(regbehind_T)))
		status = RA_FAIL;
	    وگرنه
	    {
		regstack.ga_len += اندازه(regbehind_T);
		rp = regstack_افزودن(RS_BEHIND1, scan);
		اگر (rp == پوچ)
		    status = RA_FAIL;
		وگرنه
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
	    بشکن;

	  حالت BHPOS:
	    اگر (REG_MULTI)
	    {
		اگر (behind_pos.rs_u.pos.col != (colnr_T)(rex.درون_داد - rex.سطر)
			|| behind_pos.rs_u.pos.lnum != rex.lnum)
		    status = RA_NOMATCH;
	    }
	    وگرنه اگر (behind_pos.rs_u.ptr != rex.درون_داد)
		status = RA_NOMATCH;
	    بشکن;

	  حالت NEWL:
	    اگر ((c != NUL || !REG_MULTI || rex.lnum > rex.reg_maxline
			     || rex.reg_سطر_lbr)
					   && (c != '\n' || !rex.reg_سطر_lbr))
		status = RA_NOMATCH;
	    وگرنه اگر (rex.reg_سطر_lbr)
		ADVANCE_REGINPUT();
	    وگرنه
		reg_nextline();
	    بشکن;

	  حالت END:
	    status = RA_MATCH;	// Success!
	    بشکن;

	  پیشفرض:
	    iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	    printf("Illegal op code %d\n", op);
#endif
	    status = RA_FAIL;
	    بشکن;
	  }
	}

	// If we can't continue sequentially, break the inner loop.
	اگر (status != RA_CONT)
	    بشکن;

	// Continue in inner loop, advance to next item.
	scan = next;

    } // end of inner loop

    // If there is something on the regstack execute the code for the state.
    // If the state is popped then loop and use the older state.
    وقتی (regstack.ga_len > 0 && status != RA_FAIL)
    {
	rp = (regitem_T *)((نویسه *)regstack.ga_data + regstack.ga_len) - 1;
	انتخاب (rp->rs_state)
	{
	  حالت RS_NOPEN:
	    // Result is passed on as-is, simply pop the state.
	    regstack_برداشت(&scan);
	    بشکن;

	  حالت RS_MOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    اگر (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_startpos[rp->rs_no],
						  &rex.reg_startp[rp->rs_no]);
	    regstack_برداشت(&scan);
	    بشکن;

#ifdef FEAT_SYN_HL
	  حالت RS_ZOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    اگر (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_startzpos[rp->rs_no],
						 &reg_startzp[rp->rs_no]);
	    regstack_برداشت(&scan);
	    بشکن;
#endif

	  حالت RS_MCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    اگر (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_endpos[rp->rs_no],
						    &rex.reg_endp[rp->rs_no]);
	    regstack_برداشت(&scan);
	    بشکن;

#ifdef FEAT_SYN_HL
	  حالت RS_ZCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    اگر (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_endzpos[rp->rs_no],
						   &reg_endzp[rp->rs_no]);
	    regstack_برداشت(&scan);
	    بشکن;
#endif

	  حالت RS_BRANCH:
	    اگر (status == RA_MATCH)
		// this branch matched, use it
		regstack_برداشت(&scan);
	    وگرنه
	    {
		اگر (status != RA_BREAK)
		{
		    // After a non-matching branch: try next one.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    scan = rp->rs_scan;
		}
		اگر (scan == پوچ || OP(scan) != BRANCH)
		{
		    // no more branches, didn't find a match
		    status = RA_NOMATCH;
		    regstack_برداشت(&scan);
		}
		وگرنه
		{
		    // Prepare to try a branch.
		    rp->rs_scan = regnext(scan);
		    reg_save(&rp->rs_un.regsave, &backpos);
		    scan = OPERAND(scan);
		}
	    }
	    بشکن;

	  حالت RS_BRCPLX_MORE:
	    // Pop the state.  Restore pointers when there is no match.
	    اگر (status == RA_NOMATCH)
	    {
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_شمار[rp->rs_no];	// decrement match count
	    }
	    regstack_برداشت(&scan);
	    بشکن;

	  حالت RS_BRCPLX_LONG:
	    // Pop the state.  Restore pointers when there is no match.
	    اگر (status == RA_NOMATCH)
	    {
		// There was no match, but we did find enough matches.
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_شمار[rp->rs_no];
		// continue with the items after "\{}"
		status = RA_CONT;
	    }
	    regstack_برداشت(&scan);
	    اگر (status == RA_CONT)
		scan = regnext(scan);
	    بشکن;

	  حالت RS_BRCPLX_SHORT:
	    // Pop the state.  Restore pointers when there is no match.
	    اگر (status == RA_NOMATCH)
		// There was no match, try to match one more item.
		reg_restore(&rp->rs_un.regsave, &backpos);
	    regstack_برداشت(&scan);
	    اگر (status == RA_NOMATCH)
	    {
		scan = OPERAND(scan);
		status = RA_CONT;
	    }
	    بشکن;

	  حالت RS_NOMATCH:
	    // Pop the state.  If the operand matches for NOMATCH or
	    // doesn't match for MATCH/SUBPAT, we fail.  Otherwise backup,
	    // except for SUBPAT, and continue with the next item.
	    اگر (status == (rp->rs_no == NOMATCH ? RA_MATCH : RA_NOMATCH))
		status = RA_NOMATCH;
	    وگرنه
	    {
		status = RA_CONT;
		اگر (rp->rs_no != SUBPAT)	// zero-width
		    reg_restore(&rp->rs_un.regsave, &backpos);
	    }
	    regstack_برداشت(&scan);
	    اگر (status == RA_CONT)
		scan = regnext(scan);
	    بشکن;

	  حالت RS_BEHIND1:
	    اگر (status == RA_NOMATCH)
	    {
		regstack_برداشت(&scan);
		regstack.ga_len -= اندازه(regbehind_T);
	    }
	    وگرنه
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
	    بشکن;

	  حالت RS_BEHIND2:
	    // Looping for BEHIND / NOBEHIND match.
	    اگر (status == RA_MATCH && reg_save_equal(&behind_pos))
	    {
		// found a match that ends where "next" started
		behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		اگر (rp->rs_no == BEHIND)
		    reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
		وگرنه
		{
		    // But we didn't want a match.  Need to restore the
		    // subexpr, because what follows matched, so they have
		    // been set.
		    status = RA_NOMATCH;
		    restore_subexpr(((regbehind_T *)rp) - 1);
		}
		regstack_برداشت(&scan);
		regstack.ga_len -= اندازه(regbehind_T);
	    }
	    وگرنه
	    {
		long limit;

		// No match or a match that doesn't end where we want it: Go
		// back one character.  May go to previous line once.
		no = OK;
		limit = OPERAND_MIN(rp->rs_scan);
		اگر (REG_MULTI)
		{
		    اگر (limit > 0
			    && ((rp->rs_un.regsave.rs_u.pos.lnum
						    < behind_pos.rs_u.pos.lnum
				    ? (colnr_T)STRLEN(rex.سطر)
				    : behind_pos.rs_u.pos.col)
				- rp->rs_un.regsave.rs_u.pos.col >= limit))
			no = FAIL;
		    وگرنه اگر (rp->rs_un.regsave.rs_u.pos.col == 0)
		    {
			اگر (rp->rs_un.regsave.rs_u.pos.lnum
					< behind_pos.rs_u.pos.lnum
				|| reg_getline(
					--rp->rs_un.regsave.rs_u.pos.lnum)
								  == پوچ)
			    no = FAIL;
			وگرنه
			{
			    reg_restore(&rp->rs_un.regsave, &backpos);
			    rp->rs_un.regsave.rs_u.pos.col =
						 (colnr_T)STRLEN(rex.سطر);
			}
		    }
		    وگرنه
		    {
			اگر (has_mbyte)
			{
			    char_u *سطر =
				  reg_getline(rp->rs_un.regsave.rs_u.pos.lnum);

			    rp->rs_un.regsave.rs_u.pos.col -=
				(*mb_head_off)(سطر, سطر
				    + rp->rs_un.regsave.rs_u.pos.col - 1) + 1;
			}
			وگرنه
			    --rp->rs_un.regsave.rs_u.pos.col;
		    }
		}
		وگرنه
		{
		    اگر (rp->rs_un.regsave.rs_u.ptr == rex.سطر)
			no = FAIL;
		    وگرنه
		    {
			MB_PTR_BACK(rex.سطر, rp->rs_un.regsave.rs_u.ptr);
			اگر (limit > 0 && (long)(behind_pos.rs_u.ptr
				     - rp->rs_un.regsave.rs_u.ptr) > limit)
			    no = FAIL;
		    }
		}
		اگر (no == OK)
		{
		    // Advanced, prepare for finding match again.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    scan = OPERAND(rp->rs_scan) + 4;
		    اگر (status == RA_MATCH)
		    {
			// We did match, so subexpr may have been changed,
			// need to restore them for the next try.
			status = RA_NOMATCH;
			restore_subexpr(((regbehind_T *)rp) - 1);
		    }
		}
		وگرنه
		{
		    // Can't advance.  For NOBEHIND that's a match.
		    behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		    اگر (rp->rs_no == NOBEHIND)
		    {
			reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
			status = RA_MATCH;
		    }
		    وگرنه
		    {
			// We do want a proper match.  Need to restore the
			// subexpr if we had a match, because they may have
			// been set.
			اگر (status == RA_MATCH)
			{
			    status = RA_NOMATCH;
			    restore_subexpr(((regbehind_T *)rp) - 1);
			}
		    }
		    regstack_برداشت(&scan);
		    regstack.ga_len -= اندازه(regbehind_T);
		}
	    }
	    بشکن;

	  حالت RS_STAR_LONG:
	  حالت RS_STAR_SHORT:
	    {
		regstar_T	    *rst = ((regstar_T *)rp) - 1;

		اگر (status == RA_MATCH)
		{
		    regstack_برداشت(&scan);
		    regstack.ga_len -= اندازه(regstar_T);
		    بشکن;
		}

		// Tried once already, restore input pointers.
		اگر (status != RA_BREAK)
		    reg_restore(&rp->rs_un.regsave, &backpos);

		// Repeat until we found a position where it could match.
		برای (;;)
		{
		    اگر (status != RA_BREAK)
		    {
			// Tried first position already, advance.
			اگر (rp->rs_state == RS_STAR_LONG)
			{
			    // Trying for longest match, but couldn't or
			    // didn't match -- back up one char.
			    اگر (--rst->شمار < rst->minval)
				بشکن;
			    اگر (rex.درون_داد == rex.سطر)
			    {
				// backup to last char of previous line
				اگر (rex.lnum == 0)
				{
				    status = RA_NOMATCH;
				    بشکن;
				}
				--rex.lnum;
				rex.سطر = reg_getline(rex.lnum);
				// Just in case regrepeat() didn't count
				// right.
				اگر (rex.سطر == پوچ)
				    بشکن;
				rex.درون_داد = rex.سطر + reg_getline_len(rex.lnum);
				fast_breakcheck();
			    }
			    وگرنه
				MB_PTR_BACK(rex.سطر, rex.درون_داد);
			}
			وگرنه
			{
			    // Range is backwards, use shortest match first.
			    // Careful: maxval and minval are exchanged!
			    // Couldn't or didn't match: try advancing one
			    // char.
			    اگر (rst->شمار == rst->minval
				  || regrepeat(OPERAND(rp->rs_scan), 1L) == 0)
				بشکن;
			    ++rst->شمار;
			}
			اگر (got_int)
			    بشکن;
		    }
		    وگرنه
			status = RA_NOMATCH;

		    // If it could match, try it.
		    اگر (rst->nextb == NUL || *rex.درون_داد == rst->nextb
					     || *rex.درون_داد == rst->nextb_ic)
		    {
			reg_save(&rp->rs_un.regsave, &backpos);
			scan = regnext(rp->rs_scan);
			status = RA_CONT;
			بشکن;
		    }
		}
		اگر (status != RA_CONT)
		{
		    // Failed.
		    regstack_برداشت(&scan);
		    regstack.ga_len -= اندازه(regstar_T);
		    status = RA_NOMATCH;
		}
	    }
	    بشکن;
	}

	// If we want to continue the inner loop or didn't pop a state
	// continue matching loop
	اگر (status == RA_CONT || rp == (regitem_T *)
			     ((نویسه *)regstack.ga_data + regstack.ga_len) - 1)
	    بشکن;

#ifdef FEAT_RELTIME
	اگر (bt_did_time_out(timed_out))
	{
	    status = RA_FAIL;
	    بشکن;
	}
#endif
    }

    // May need to continue with the inner loop, starting at "scan".
    اگر (status == RA_CONT)
	ادامه;

    // If the regstack is empty or something failed we are done.
    اگر (regstack.ga_len == 0 || status == RA_FAIL)
    {
	اگر (scan == پوچ)
	{
	    // We get here only if there's trouble -- normally "case END" is
	    // the terminating point.
	    iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	    printf("Premature EOL\n");
#endif
	}
	بازگشت (status == RA_MATCH);
    }

  } // End of loop until the regstack is empty.

  // NOTREACHED
}

/*
 * regtry - try match of "prog" with at rex.line["col"].
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    ایستا long
regtry(
    bt_regprog_T	*prog,
    colnr_T		col,
    صحیح			*timed_out)	// flag set on timeout or NULL
{
    rex.درون_داد = rex.سطر + col;
    rex.need_clear_subexpr = TRUE;
#ifdef FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    rex.need_clear_zsubexpr = (prog->reghasz == REX_SET);
#endif

    اگر (regmatch(prog->program + 1, timed_out) == 0)
	بازگشت 0;

    cleanup_subexpr();
    اگر (REG_MULTI)
    {
	اگر (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].col = col;
	}
	اگر (rex.reg_endpos[0].lnum < 0)
	{
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].col = (صحیح)(rex.درون_داد - rex.سطر);
	}
	وگرنه
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    وگرنه
    {
	اگر (rex.reg_startp[0] == پوچ)
	    rex.reg_startp[0] = rex.سطر + col;
	اگر (rex.reg_endp[0] == پوچ)
	    rex.reg_endp[0] = rex.درون_داد;
    }
#ifdef FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = پوچ;

    اگر (prog->reghasz == REX_SET)
    {
	صحیح		i;

	cleanup_zsubexpr();
	re_extmatch_out = make_extmatch();
	اگر (re_extmatch_out == پوچ)
	    بازگشت 0;
	برای (i = 0; i < NSUBEXP; i++)
	{
	    اگر (REG_MULTI)
	    {
		// Only accept single line matches.
		اگر (reg_startzpos[i].lnum >= 0
			&& reg_endzpos[i].lnum == reg_startzpos[i].lnum
			&& reg_endzpos[i].col >= reg_startzpos[i].col)
		    re_extmatch_out->matches[i] =
			vim_strnsave(reg_getline(reg_startzpos[i].lnum)
						       + reg_startzpos[i].col,
				   reg_endzpos[i].col - reg_startzpos[i].col);
	    }
	    وگرنه
	    {
		اگر (reg_startzp[i] != پوچ && reg_endzp[i] != پوچ)
		    re_extmatch_out->matches[i] =
			    vim_strnsave(reg_startzp[i],
						reg_endzp[i] - reg_startzp[i]);
	    }
	}
    }
#endif
    بازگشت 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    ایستا long
bt_regexec_both(
    char_u	*سطر,
    colnr_T	startcol,	// column to start looking for match
    صحیح		*timed_out)	// flag set on timeout or NULL
{
    bt_regprog_T    *prog;
    char_u	    *s;
    colnr_T	    col = startcol;
    long	    retval = 0L;

    // Create "regstack" and "backpos" if they are not allocated yet.
    // We allocate *_INITIAL amount of bytes first and then set the grow size
    // to much bigger value to avoid many malloc calls in case of deep regular
    // expressions.
    اگر (regstack.ga_data == پوچ)
    {
	// Use an item size of 1 byte, since we push different things
	// onto the regstack.
	ga_init2(&regstack, 1, REGSTACK_INITIAL);
	(تهی)ga_grow(&regstack, REGSTACK_INITIAL);
	regstack.ga_growsize = REGSTACK_INITIAL * 8;
    }

    اگر (backpos.ga_data == پوچ)
    {
	ga_init2(&backpos, اندازه(backpos_T), BACKPOS_INITIAL);
	(تهی)ga_grow(&backpos, BACKPOS_INITIAL);
	backpos.ga_growsize = BACKPOS_INITIAL * 8;
    }

    اگر (REG_MULTI)
    {
	prog = (bt_regprog_T *)rex.reg_mmatch->regprog;
	سطر = reg_getline((linenr_T)0);
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    وگرنه
    {
	prog = (bt_regprog_T *)rex.reg_match->regprog;
	rex.reg_startp = rex.reg_match->startp;
	rex.reg_endp = rex.reg_match->endp;
    }

    // Be paranoid...
    اگر (prog == پوچ || سطر == پوچ)
    {
	iemsg(e_null_argument);
	برو theend;
    }

    // Check validity of program.
    اگر (prog_magic_wrong())
	برو theend;

    // If the start column is past the maximum column: no need to try.
    اگر (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	برو theend;

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    اگر (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    وگرنه اگر (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    اگر (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    // If there is a "must appear" string, look for it.
    اگر (prog->regmust != پوچ)
    {
	صحیح c;

	اگر (has_mbyte)
	    c = (*mb_ptr2char)(prog->regmust);
	وگرنه
	    c = *prog->regmust;
	s = سطر + col;

	// This is used very often, esp. for ":global".  Use three versions of
	// the loop to avoid overhead of conditions.
	اگر (!rex.reg_ic && !has_mbyte)
	    وقتی ((s = vim_strbyte(s, c)) != پوچ)
	    {
		صحیح len = prog->regmlen;
		اگر (cstrncmp(s, prog->regmust, &len) == 0)
		    بشکن;		// Found it.
		++s;
	    }
	وگرنه اگر (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	    وقتی ((s = vim_strchr(s, c)) != پوچ)
	    {
		صحیح len = prog->regmlen;
		اگر (cstrncmp(s, prog->regmust, &len) == 0)
		    بشکن;		// Found it.
		MB_PTR_ADV(s);
	    }
	وگرنه
	    وقتی ((s = cstrchr(s, c)) != پوچ)
	    {
		صحیح len = prog->regmlen;
		اگر (cstrncmp(s, prog->regmust, &len) == 0)
		    بشکن;		// Found it.
		MB_PTR_ADV(s);
	    }
	اگر (s == پوچ)		// Not present.
	    برو theend;
    }

    rex.سطر = سطر;
    rex.lnum = 0;
    reg_toolong = FALSE;

    // Simplest case: Anchored match need be tried only once.
    اگر (prog->reganch)
    {
	صحیح	c;

	اگر (has_mbyte)
	    c = (*mb_ptr2char)(rex.سطر + col);
	وگرنه
	    c = rex.سطر[col];
	اگر (prog->regstart == NUL
		|| prog->regstart == c
		|| (rex.reg_ic
		    && (((enc_utf8 && utf_fold(prog->regstart) == utf_fold(c)))
			|| (c < 255 && prog->regstart < 255 &&
			    MB_TOLOWER(prog->regstart) == MB_TOLOWER(c)))))
	    retval = regtry(prog, col, timed_out);
	وگرنه
	    retval = 0;
    }
    وگرنه
    {
	// Messy cases:  unanchored match.
	وقتی (!got_int)
	{
	    اگر (prog->regstart != NUL)
	    {
		// Skip until the char we know it must start with.
		// Used often, do some work to avoid call overhead.
		اگر (!rex.reg_ic && !has_mbyte)
		    s = vim_strbyte(rex.سطر + col, prog->regstart);
		وگرنه
		    s = cstrchr(rex.سطر + col, prog->regstart);
		اگر (s == پوچ)
		{
		    retval = 0;
		    بشکن;
		}
		col = (صحیح)(s - rex.سطر);
	    }

	    // Check for maximum column to try.
	    اگر (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	    {
		retval = 0;
		بشکن;
	    }

	    retval = regtry(prog, col, timed_out);
	    اگر (retval > 0)
		بشکن;

	    // if not currently on the first line, get it again
	    اگر (rex.lnum != 0)
	    {
		rex.lnum = 0;
		rex.سطر = reg_getline((linenr_T)0);
	    }
	    اگر (rex.سطر[col] == NUL)
		بشکن;
	    اگر (has_mbyte)
		col += (*mb_ptr2len)(rex.سطر + col);
	    وگرنه
		++col;
#ifdef FEAT_RELTIME
	    اگر (bt_did_time_out(timed_out))
		بشکن;
#endif
	}
    }

theend:
    // Free "reg_tofree" when it's a bit big.
    // Free regstack and backpos if they are bigger than their initial size.
    اگر (reg_tofreelen > 400)
	VIM_CLEAR(reg_tofree);
    اگر (regstack.ga_maxlen > REGSTACK_INITIAL)
	ga_clear(&regstack);
    اگر (backpos.ga_maxlen > BACKPOS_INITIAL)
	ga_clear(&backpos);

    اگر (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs
	// and \ze are used.
	اگر (REG_MULTI)
	{
	    lpos_T *start = &rex.reg_mmatch->startpos[0];
	    lpos_T *end = &rex.reg_mmatch->endpos[0];

	    اگر (end->lnum < start->lnum
			|| (end->lnum == start->lnum && end->col < start->col))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_mmatch->rmm_matchcol = col;
	}
	وگرنه
	{
	    اگر (rex.reg_match->endp[0] < rex.reg_match->startp[0])
		rex.reg_match->endp[0] = rex.reg_match->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_match->rm_matchcol = col;
	}
    }

    بازگشت retval;
}

/*
 * Match a regexp against a string.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 * if "line_lbr" is TRUE  consider a "\n" in "line" to be a line break.
 *
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    ایستا صحیح
bt_regexec_nl(
    regmatch_T	*rmp,
    char_u	*سطر,	// string to match against
    colnr_T	col,	// column to start looking for match
    صحیح		سطر_lbr)
{
    rex.reg_match = rmp;
    rex.reg_mmatch = پوچ;
    rex.reg_maxline = 0;
    rex.reg_سطر_lbr = سطر_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = پوچ;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;

    بازگشت bt_regexec_both(سطر, col, پوچ);
}

/*
 * Match a regexp against multiple lines.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 *
 * Return zero if there is no match.  Return number of lines contained in the
 * match otherwise.
 */
    ایستا long
bt_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	col,		// column to start looking for match
    صحیح		*timed_out)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    بازگشت bt_regexec_both(پوچ, col, timed_out);
}

/*
 * Compare a number with the operand of RE_LNUM, RE_COL or RE_VCOL.
 */
    ایستا صحیح
re_رقم_cmp(long_u val, char_u *scan)
{
    long_u  n = OPERAND_MIN(scan);

    اگر (OPERAND_CMP(scan) == '>')
	بازگشت val > n;
    اگر (OPERAND_CMP(scan) == '<')
	بازگشت val < n;
    بازگشت val == n;
}

#ifdef BT_REGEXP_DUMP

/*
 * regdump - dump a regexp onto stdout in vaguely comprehensible form
 */
    ایستا تهی
regdump(char_u *pattern, bt_regprog_T *r)
{
    char_u  *s;
    صحیح	    op = EXACTLY;	// Arbitrary non-END op.
    char_u  *next;
    char_u  *end = پوچ;
    FILE    *f;

# ifdef BT_REGEXP_LOG
    f = fopen("bt_regexp_log.log", "a");
# وگرنه
    f = stdout;
# endif
    اگر (f == پوچ)
	بازگشت;
    fprintf(f, "-------------------------------------\n\r\nregcomp(%s):\r\n", pattern);

    s = r->program + 1;
    // Loop until we find the END that isn't before a referred next (an END
    // can also appear in a NOMATCH operand).
    وقتی ((op != END || s <= end) && s < r->program + r->regsz)
    {
	op = OP(s);
	fprintf(f, "%2d%s", (صحیح)(s - r->program), regprop(s)); // Where, what.
	next = (s + 3 <= r->program + r->regsz) ? regnext(s) : پوچ;
	اگر (next == پوچ)	// Next ptr.
	    fprintf(f, "(0)");
	وگرنه
	    fprintf(f, "(%d)", (صحیح)((s - r->program) + (next - s)));
	اگر (end < next)
	    end = next;
	اگر (op == BRACE_LIMITS)
	{
	    // Two ints
	    fprintf(f, " minval %ld, maxval %ld", OPERAND_MIN(s), OPERAND_MAX(s));
	    s += 8;
	}
	وگرنه اگر (op == BEHIND || op == NOBEHIND)
	{
	    // one int
	    fprintf(f, " count %ld", OPERAND_MIN(s));
	    s += 4;
	}
	وگرنه اگر (op == RE_LNUM || op == RE_COL || op == RE_VCOL)
	{
	    // one int plus comparator
	    fprintf(f, " count %ld", OPERAND_MIN(s));
	    s += 5;
	}
	s += 3;
	اگر (op == MULTIBYTECODE)
	{
	    fprintf(f, " mbc=%d", utf_ptr2char(s));
	    s += utfc_ptr2len(s);
	}
	اگر (op == ANYOF || op == ANYOF + ADD_NL
		|| op == ANYBUT || op == ANYBUT + ADD_NL
		|| op == EXACTLY)
	{
	    // Literal string, where present.
	    fprintf(f, "\nxxxxxxxxx\n");
	    وقتی (*s != NUL && s < r->program + r->regsz)
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
    اگر (r->regstart != NUL)
	fprintf(f, "start `%s' 0x%x; ", r->regstart < 256
		? (نویسه *)transchar(r->regstart)
		: "multibyte", r->regstart);
    اگر (r->reganch)
	fprintf(f, "anchored; ");
    اگر (r->regmust != پوچ)
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
    ایستا char_u *
regprop(char_u *op)
{
    نویسه	    *p;
    ایستا نویسه	    buf[50];
    ایستا size_t   buflen = 0;

    STRCPY(buf, ":");
    buflen = 1;

    انتخاب ((صحیح) OP(op))
    {
      حالت BOL:
	p = "BOL";
	بشکن;
      حالت EOL:
	p = "EOL";
	بشکن;
      حالت RE_BOF:
	p = "BOF";
	بشکن;
      حالت RE_EOF:
	p = "EOF";
	بشکن;
      حالت CURSOR:
	p = "CURSOR";
	بشکن;
      حالت RE_VISUAL:
	p = "RE_VISUAL";
	بشکن;
      حالت RE_LNUM:
	p = "RE_LNUM";
	بشکن;
      حالت RE_MARK:
	p = "RE_MARK";
	بشکن;
      حالت RE_COL:
	p = "RE_COL";
	بشکن;
      حالت RE_VCOL:
	p = "RE_VCOL";
	بشکن;
      حالت BOW:
	p = "BOW";
	بشکن;
      حالت EOW:
	p = "EOW";
	بشکن;
      حالت ANY:
	p = "ANY";
	بشکن;
      حالت ANY + ADD_NL:
	p = "ANY+NL";
	بشکن;
      حالت ANYOF:
	p = "ANYOF";
	بشکن;
      حالت ANYOF + ADD_NL:
	p = "ANYOF+NL";
	بشکن;
      حالت ANYBUT:
	p = "ANYBUT";
	بشکن;
      حالت ANYBUT + ADD_NL:
	p = "ANYBUT+NL";
	بشکن;
      حالت IDENT:
	p = "IDENT";
	بشکن;
      حالت IDENT + ADD_NL:
	p = "IDENT+NL";
	بشکن;
      حالت SIDENT:
	p = "SIDENT";
	بشکن;
      حالت SIDENT + ADD_NL:
	p = "SIDENT+NL";
	بشکن;
      حالت KWORD:
	p = "KWORD";
	بشکن;
      حالت KWORD + ADD_NL:
	p = "KWORD+NL";
	بشکن;
      حالت SKWORD:
	p = "SKWORD";
	بشکن;
      حالت SKWORD + ADD_NL:
	p = "SKWORD+NL";
	بشکن;
      حالت FNAME:
	p = "FNAME";
	بشکن;
      حالت FNAME + ADD_NL:
	p = "FNAME+NL";
	بشکن;
      حالت SFNAME:
	p = "SFNAME";
	بشکن;
      حالت SFNAME + ADD_NL:
	p = "SFNAME+NL";
	بشکن;
      حالت PRINT:
	p = "PRINT";
	بشکن;
      حالت PRINT + ADD_NL:
	p = "PRINT+NL";
	بشکن;
      حالت SPRINT:
	p = "SPRINT";
	بشکن;
      حالت SPRINT + ADD_NL:
	p = "SPRINT+NL";
	بشکن;
      حالت WHITE:
	p = "WHITE";
	بشکن;
      حالت WHITE + ADD_NL:
	p = "WHITE+NL";
	بشکن;
      حالت NWHITE:
	p = "NWHITE";
	بشکن;
      حالت NWHITE + ADD_NL:
	p = "NWHITE+NL";
	بشکن;
      حالت DIGIT:
	p = "DIGIT";
	بشکن;
      حالت DIGIT + ADD_NL:
	p = "DIGIT+NL";
	بشکن;
      حالت NDIGIT:
	p = "NDIGIT";
	بشکن;
      حالت NDIGIT + ADD_NL:
	p = "NDIGIT+NL";
	بشکن;
      حالت HEX:
	p = "HEX";
	بشکن;
      حالت HEX + ADD_NL:
	p = "HEX+NL";
	بشکن;
      حالت NHEX:
	p = "NHEX";
	بشکن;
      حالت NHEX + ADD_NL:
	p = "NHEX+NL";
	بشکن;
      حالت OCTAL:
	p = "OCTAL";
	بشکن;
      حالت OCTAL + ADD_NL:
	p = "OCTAL+NL";
	بشکن;
      حالت NOCTAL:
	p = "NOCTAL";
	بشکن;
      حالت NOCTAL + ADD_NL:
	p = "NOCTAL+NL";
	بشکن;
      حالت WORD:
	p = "WORD";
	بشکن;
      حالت WORD + ADD_NL:
	p = "WORD+NL";
	بشکن;
      حالت NWORD:
	p = "NWORD";
	بشکن;
      حالت NWORD + ADD_NL:
	p = "NWORD+NL";
	بشکن;
      حالت HEAD:
	p = "HEAD";
	بشکن;
      حالت HEAD + ADD_NL:
	p = "HEAD+NL";
	بشکن;
      حالت NHEAD:
	p = "NHEAD";
	بشکن;
      حالت NHEAD + ADD_NL:
	p = "NHEAD+NL";
	بشکن;
      حالت ALPHA:
	p = "ALPHA";
	بشکن;
      حالت ALPHA + ADD_NL:
	p = "ALPHA+NL";
	بشکن;
      حالت NALPHA:
	p = "NALPHA";
	بشکن;
      حالت NALPHA + ADD_NL:
	p = "NALPHA+NL";
	بشکن;
      حالت LOWER:
	p = "LOWER";
	بشکن;
      حالت LOWER + ADD_NL:
	p = "LOWER+NL";
	بشکن;
      حالت NLOWER:
	p = "NLOWER";
	بشکن;
      حالت NLOWER + ADD_NL:
	p = "NLOWER+NL";
	بشکن;
      حالت UPPER:
	p = "UPPER";
	بشکن;
      حالت UPPER + ADD_NL:
	p = "UPPER+NL";
	بشکن;
      حالت NUPPER:
	p = "NUPPER";
	بشکن;
      حالت NUPPER + ADD_NL:
	p = "NUPPER+NL";
	بشکن;
      حالت BRANCH:
	p = "BRANCH";
	بشکن;
      حالت EXACTLY:
	p = "EXACTLY";
	بشکن;
      حالت NOTHING:
	p = "NOTHING";
	بشکن;
      حالت BACK:
	p = "BACK";
	بشکن;
      حالت END:
	p = "END";
	بشکن;
      حالت MOPEN + 0:
	p = "MATCH START";
	بشکن;
      حالت MOPEN + 1:
      حالت MOPEN + 2:
      حالت MOPEN + 3:
      حالت MOPEN + 4:
      حالت MOPEN + 5:
      حالت MOPEN + 6:
      حالت MOPEN + 7:
      حالت MOPEN + 8:
      حالت MOPEN + 9:
	buflen += vim_snprintf(buf + buflen, اندازه(buf) - buflen, "MOPEN%d", OP(op) - MOPEN);
	p = پوچ;
	بشکن;
      حالت MCLOSE + 0:
	p = "MATCH END";
	بشکن;
      حالت MCLOSE + 1:
      حالت MCLOSE + 2:
      حالت MCLOSE + 3:
      حالت MCLOSE + 4:
      حالت MCLOSE + 5:
      حالت MCLOSE + 6:
      حالت MCLOSE + 7:
      حالت MCLOSE + 8:
      حالت MCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, اندازه(buf) - buflen, "MCLOSE%d", OP(op) - MCLOSE);
	p = پوچ;
	بشکن;
      حالت BACKREF + 1:
      حالت BACKREF + 2:
      حالت BACKREF + 3:
      حالت BACKREF + 4:
      حالت BACKREF + 5:
      حالت BACKREF + 6:
      حالت BACKREF + 7:
      حالت BACKREF + 8:
      حالت BACKREF + 9:
	buflen += vim_snprintf(buf + buflen, اندازه(buf) - buflen, "BACKREF%d", OP(op) - BACKREF);
	p = پوچ;
	بشکن;
      حالت NOPEN:
	p = "NOPEN";
	بشکن;
      حالت NCLOSE:
	p = "NCLOSE";
	بشکن;
# ifdef FEAT_SYN_HL
      حالت ZOPEN + 1:
      حالت ZOPEN + 2:
      حالت ZOPEN + 3:
      حالت ZOPEN + 4:
      حالت ZOPEN + 5:
      حالت ZOPEN + 6:
      حالت ZOPEN + 7:
      حالت ZOPEN + 8:
      حالت ZOPEN + 9:
	buflen += vim_snprintf(buf + buflen, اندازه(buf) - buflen, "ZOPEN%d", OP(op) - ZOPEN);
	p = پوچ;
	بشکن;
      حالت ZCLOSE + 1:
      حالت ZCLOSE + 2:
      حالت ZCLOSE + 3:
      حالت ZCLOSE + 4:
      حالت ZCLOSE + 5:
      حالت ZCLOSE + 6:
      حالت ZCLOSE + 7:
      حالت ZCLOSE + 8:
      حالت ZCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, اندازه(buf) - buflen, "ZCLOSE%d", OP(op) - ZCLOSE);
	p = پوچ;
	بشکن;
      حالت ZREF + 1:
      حالت ZREF + 2:
      حالت ZREF + 3:
      حالت ZREF + 4:
      حالت ZREF + 5:
      حالت ZREF + 6:
      حالت ZREF + 7:
      حالت ZREF + 8:
      حالت ZREF + 9:
	buflen += vim_snprintf(buf + buflen, اندازه(buf) - buflen, "ZREF%d", OP(op) - ZREF);
	p = پوچ;
	بشکن;
# endif
      حالت STAR:
	p = "STAR";
	بشکن;
      حالت PLUS:
	p = "PLUS";
	بشکن;
      حالت NOMATCH:
	p = "NOMATCH";
	بشکن;
      حالت MATCH:
	p = "MATCH";
	بشکن;
      حالت BEHIND:
	p = "BEHIND";
	بشکن;
      حالت NOBEHIND:
	p = "NOBEHIND";
	بشکن;
      حالت SUBPAT:
	p = "SUBPAT";
	بشکن;
      حالت BRACE_LIMITS:
	p = "BRACE_LIMITS";
	بشکن;
      حالت BRACE_SIMPLE:
	p = "BRACE_SIMPLE";
	بشکن;
      حالت BRACE_COMPLEX + 0:
      حالت BRACE_COMPLEX + 1:
      حالت BRACE_COMPLEX + 2:
      حالت BRACE_COMPLEX + 3:
      حالت BRACE_COMPLEX + 4:
      حالت BRACE_COMPLEX + 5:
      حالت BRACE_COMPLEX + 6:
      حالت BRACE_COMPLEX + 7:
      حالت BRACE_COMPLEX + 8:
      حالت BRACE_COMPLEX + 9:
	buflen += vim_snprintf(buf + buflen, اندازه(buf) - buflen, "BRACE_COMPLEX%d", OP(op) - BRACE_COMPLEX);
	p = پوچ;
	بشکن;
      حالت MULTIBYTECODE:
	p = "MULTIBYTECODE";
	بشکن;
      حالت NEWL:
	p = "NEWL";
	بشکن;
      پیشفرض:
	buflen += vim_snprintf(buf + buflen, اندازه(buf) - buflen, "corrupt %d", OP(op));
	p = پوچ;
	بشکن;
    }
    اگر (p != پوچ)
	vim_strncpy((char_u *)buf + buflen, (char_u *)p, اندازه(buf) - buflen - 1);
    بازگشت (char_u *)buf;
}
#endif	    // DEBUG
