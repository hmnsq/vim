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

ساكن صحيح	num_complex_braces; // Complex \{...} count
ساكن char_u	*regcode;	// Code-emit pointer, or JUST_CALC_SIZE
ساكن long	regsize;	// Code size.
ساكن صحيح	reg_toolong;	// TRUE when offset out of range
ساكن صحيح	bt_reg_parse_depth;	// nesting depth in reg()
ساكن char_u	had_endbrace[NSUBEXP];	// flags, TRUE if end of () found
ساكن long	brace_min[10];	// Minimums for complex brace repeats
ساكن long	brace_max[10];	// Maximums for complex brace repeats
ساكن صحيح	brace_count[10]; // Current counts for complex brace repeats
ساكن صحيح	one_exactly = FALSE;	// only do one char for EXACTLY

// When making changes to classchars also change nfa_classcodes.
ساكن char_u	*classchars = (char_u *)".iIkKfFpPsSdDxXoOwWhHaAlLuU";
ساكن ثابت صحيح classcodes[] = {
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
    ساكن صحيح
bt_recognize_char_class(char_u *start, char_u *end)
{
    صحيح		newl;
    صحيح		config = get_char_class_bits(start, end, &newl);

    // A newline would need the ADD_NL variant, don't bother with it here.
    אם (config < 0 || newl)
	החזר 0;

	// The old engine has no case-insensitive class opcode, so [a-z] and [A-Z]
	// are left as collections.  The classes below are case-independent.
    بدل (config)
    {
	حالة CLASS_o9:
	    החזר DIGIT;
	حالة CLASS_not | CLASS_o9:
	    החזר NDIGIT;
	حالة CLASS_af | CLASS_AF | CLASS_o9:
	    החזר HEX;
	حالة CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    החזר NHEX;
	حالة CLASS_o7:
	    החזר OCTAL;
	حالة CLASS_not | CLASS_o7:
	    החזר NOCTAL;
	حالة CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    החזר WORD;
	حالة CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    החזר NWORD;
	حالة CLASS_az | CLASS_AZ | CLASS_underscore:
	    החזר HEAD;
	حالة CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    החזר NHEAD;
	حالة CLASS_az | CLASS_AZ:
	    החזר ALPHA;
	حالة CLASS_not | CLASS_az | CLASS_AZ:
	    החזר NALPHA;
    }
    החזר 0;
}

/*
 * When regcode is set to this value, code is not emitted and size is computed
 * instead.
 */
#define JUST_CALC_SIZE	((char_u *) -1)

// Values for rs_state in regitem_T.
تعريفنوع تعداد regstate_E
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
تعريفنوع بنية
{
    اتحاد
    {
	char_u	*ptr;	// rex.input pointer, for single-line regexp
	lpos_T	pos;	// rex.input pos, for multi-line regexp
    } rs_u;
    صحيح		rs_len;
} regsave_T;

// struct to save start/end pointer/position in for \(\)
تعريفنوع بنية
{
    اتحاد
    {
	char_u	*ptr;
	lpos_T	pos;
    } se_u;
} save_se_T;

// used for BEHIND and NOBEHIND matching
تعريفنوع بنية regbehind_S
{
    regsave_T	save_after;
    regsave_T	save_behind;
    صحيح		save_need_clear_subexpr;
    save_se_T   save_start[NSUBEXP];
    save_se_T   save_end[NSUBEXP];
} regbehind_T;

/*
 * When there are alternatives a regstate_T is put on the regstack to remember
 * what we are doing.
 * Before it may be another type of item, depending on rs_state, to remember
 * more things.
 */
تعريفنوع بنية regitem_S
{
    regstate_T	rs_state;	// what we are doing, one of RS_ above
    short	rs_no;		// submatch nr or BEHIND/NOBEHIND
    char_u	*rs_scan;	// current node in program
    اتحاد
    {
	save_se_T  sesave;
	regsave_T  regsave;
    } rs_un;			// room for saving rex.input
} regitem_T;


// used for STAR, PLUS and BRACE_SIMPLE matching
تعريفنوع بنية regstar_S
{
    صحيح		nextb;		// next byte
    صحيح		nextb_ic;	// next byte reverse case
    long	count;
    long	minval;
    long	maxval;
} regstar_T;

// used to store input position when a BACK was encountered, so that we now if
// we made any progress since the last time.
تعريفنوع بنية backpos_S
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
ساكن garray_T	regstack = {0, 0, 0, 0, عدم};
ساكن garray_T	backpos = {0, 0, 0, 0, عدم};

ساكن regsave_T behind_pos;

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
#define OP(p)		((صحيح)*(p))
#define NEXT(p)		(((*((p) + 1) & 0377) << 8) + (*((p) + 2) & 0377))
#define OPERAND(p)	((p) + 3)
// Obtain an operand that was stored as four bytes, MSB first.
#define OPERAND_MIN(p)	(((long)(p)[3] << 24) + ((long)(p)[4] << 16) \
			+ ((long)(p)[5] << 8) + (long)(p)[6])
// Obtain a second operand stored as four bytes.
#define OPERAND_MAX(p)	OPERAND_MIN((p) + 4)
// Obtain a second single-byte operand stored after a four bytes operand.
#define OPERAND_CMP(p)	(p)[7]

ساكن char_u *reg(صحيح paren, صحيح *flagp);

#ifdef BT_REGEXP_DUMP
ساكن فراغ	regdump(char_u *, bt_regprog_T *);
#endif

ساكن صحيح	re_num_cmp(long_u val, char_u *scan);

#ifdef DEBUG
ساكن char_u	*regprop(char_u *);

ساكن صحيح	regnarrate = 0;
#endif


/*
 * Setup to parse the regexp.  Used once to get the length and once to do it.
 */
    ساكن فراغ
regcomp_start(
    char_u	*expr,
    صحيح		re_flags)	    // see vim_regcomp()
{
    initchr(expr);
    אם (re_flags & RE_MAGIC)
	reg_magic = MAGIC_ON;
    אחרת
	reg_magic = MAGIC_OFF;
    reg_string = (re_flags & RE_STRING);
    reg_strict = (re_flags & RE_STRICT);
    get_cpo_flags();

    num_complex_braces = 0;
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
#אם defined(FEAT_SYN_HL)
    had_eol = FALSE;
#endif
}

/*
 * Return TRUE if MULTIBYTECODE should be used instead of EXACTLY for
 * character "c".
 */
    ساكن صحيح
use_multibytecode(صحيح c)
{
    החזר has_mbyte && (*mb_char2len)(c) > 1
		     && (re_multi_type(peekchr()) != NOT_MULTI
			     || (enc_utf8 && utf_iscomposing(c)));
}

/*
 * Emit (if appropriate) a byte of code
 */
    ساكن فراغ
regc(صحيح b)
{
    אם (regcode == JUST_CALC_SIZE)
	regsize++;
    אחרת
	*regcode++ = b;
}

/*
 * Emit (if appropriate) a multi-byte character of code
 */
    ساكن فراغ
regmbc(صحيح c)
{
    אם (!has_mbyte && c > 0xff)
	החזר;
    אם (regcode == JUST_CALC_SIZE)
	regsize += (*mb_char2len)(c);
    אחרת
	regcode += (*mb_char2bytes)(c, regcode);
}


/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * NOTE: When changing this function, also change nfa_emit_equi_class()
 */
    ساكن فراغ
reg_equi_class(صحيح c)
{
    אם (enc_utf8 || STRCMP(p_enc, "latin1") == 0
					 || STRCMP(p_enc, "iso-8859-15") == 0)
    {
	بدل (c)
	{
	    // Do not use '\300' style, it results in a negative number.
	    حالة 'A': حالة 0xc0: حالة 0xc1: حالة 0xc2: حالة 0xc3: حالة 0xc4:
	    حالة 0xc5: حالة 0x100: حالة 0x102: حالة 0x104: حالة 0x1cd:
	    حالة 0x1de: حالة 0x1e0: حالة 0x1fa: حالة 0x202: حالة 0x226:
	    حالة 0x23a: حالة 0x1e00: حالة 0x1ea0: حالة 0x1ea2: حالة 0x1ea4:
	    حالة 0x1ea6: حالة 0x1ea8: حالة 0x1eaa: حالة 0x1eac: حالة 0x1eae:
	    حالة 0x1eb0: حالة 0x1eb2: حالة 0x1eb4: حالة 0x1eb6:
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
		      החזר;
	    حالة 'B': حالة 0x181: حالة 0x243: حالة 0x1e02:
	    حالة 0x1e04: حالة 0x1e06:
		      regmbc('B');
		      regmbc(0x181); regmbc(0x243); regmbc(0x1e02);
		      regmbc(0x1e04); regmbc(0x1e06);
		      החזר;
	    حالة 'C': حالة 0xc7:
	    حالة 0x106: حالة 0x108: حالة 0x10a: حالة 0x10c: حالة 0x187:
	    حالة 0x23b: حالة 0x1e08: حالة 0xa792:
		      regmbc('C'); regmbc(0xc7);
		      regmbc(0x106); regmbc(0x108); regmbc(0x10a);
		      regmbc(0x10c); regmbc(0x187); regmbc(0x23b);
		      regmbc(0x1e08); regmbc(0xa792);
		      החזר;
	    حالة 'D': حالة 0x10e: حالة 0x110: حالة 0x18a:
	    حالة 0x1e0a: حالة 0x1e0c: حالة 0x1e0e: حالة 0x1e10:
	    حالة 0x1e12:
		      regmbc('D'); regmbc(0x10e); regmbc(0x110);
		      regmbc(0x18a); regmbc(0x1e0a); regmbc(0x1e0c);
		      regmbc(0x1e0e); regmbc(0x1e10); regmbc(0x1e12);
		      החזר;
	    حالة 'E': حالة 0xc8: حالة 0xc9: حالة 0xca: حالة 0xcb:
	    حالة 0x112: حالة 0x114: حالة 0x116: حالة 0x118: حالة 0x11a:
	    حالة 0x204: حالة 0x206: حالة 0x228: حالة 0x246: حالة 0x1e14:
	    حالة 0x1e16: حالة 0x1e18: حالة 0x1e1a: حالة 0x1e1c:
	    حالة 0x1eb8: حالة 0x1eba: حالة 0x1ebc: حالة 0x1ebe:
	    حالة 0x1ec0: حالة 0x1ec2: حالة 0x1ec4: حالة 0x1ec6:
		      regmbc('E'); regmbc(0xc8); regmbc(0xc9);
		      regmbc(0xca); regmbc(0xcb); regmbc(0x112);
		      regmbc(0x114); regmbc(0x116); regmbc(0x118);
		      regmbc(0x11a); regmbc(0x204); regmbc(0x206);
		      regmbc(0x228); regmbc(0x246); regmbc(0x1e14);
		      regmbc(0x1e16); regmbc(0x1e18); regmbc(0x1e1a);
		      regmbc(0x1e1c); regmbc(0x1eb8); regmbc(0x1eba);
		      regmbc(0x1ebc); regmbc(0x1ebe); regmbc(0x1ec0);
		      regmbc(0x1ec2); regmbc(0x1ec4); regmbc(0x1ec6);
		      החזר;
	    حالة 'F': حالة 0x191: حالة 0x1e1e: حالة 0xa798:
		      regmbc('F'); regmbc(0x191); regmbc(0x1e1e);
		      regmbc(0xa798);
		      החזר;
	    حالة 'G': حالة 0x11c: حالة 0x11e: حالة 0x120:
	    حالة 0x122: حالة 0x193: حالة 0x1e4: حالة 0x1e6:
	    حالة 0x1f4: حالة 0x1e20: حالة 0xa7a0:
		      regmbc('G'); regmbc(0x11c); regmbc(0x11e);
		      regmbc(0x120); regmbc(0x122); regmbc(0x193);
		      regmbc(0x1e4); regmbc(0x1e6); regmbc(0x1f4);
		      regmbc(0x1e20); regmbc(0xa7a0);
		      החזר;
	    حالة 'H': حالة 0x124: حالة 0x126: حالة 0x21e:
	    حالة 0x1e22: حالة 0x1e24: حالة 0x1e26:
	    حالة 0x1e28: حالة 0x1e2a: حالة 0x2c67:
		      regmbc('H'); regmbc(0x124); regmbc(0x126);
		      regmbc(0x21e); regmbc(0x1e22); regmbc(0x1e24);
		      regmbc(0x1e26); regmbc(0x1e28); regmbc(0x1e2a);
		      regmbc(0x2c67);
		      החזר;
	    حالة 'I': حالة 0xcc: حالة 0xcd: حالة 0xce: حالة 0xcf:
	    حالة 0x128: حالة 0x12a: حالة 0x12c: حالة 0x12e:
	    حالة 0x130: حالة 0x197: حالة 0x1cf: حالة 0x208:
	    حالة 0x20a: حالة 0x1e2c: حالة 0x1e2e: حالة 0x1ec8:
	    حالة 0x1eca:
		      regmbc('I'); regmbc(0xcc); regmbc(0xcd);
		      regmbc(0xce); regmbc(0xcf); regmbc(0x128);
		      regmbc(0x12a); regmbc(0x12c); regmbc(0x12e);
		      regmbc(0x130); regmbc(0x197); regmbc(0x1cf);
		      regmbc(0x208); regmbc(0x20a); regmbc(0x1e2c);
		      regmbc(0x1e2e); regmbc(0x1ec8); regmbc(0x1eca);
		      החזר;
	    حالة 'J': حالة 0x134: حالة 0x248:
		      regmbc('J'); regmbc(0x134); regmbc(0x248);
		      החזר;
	    حالة 'K': حالة 0x136: حالة 0x198: حالة 0x1e8: حالة 0x1e30:
	    حالة 0x1e32: حالة 0x1e34: حالة 0x2c69: حالة 0xa740:
		      regmbc('K'); regmbc(0x136); regmbc(0x198);
		      regmbc(0x1e8); regmbc(0x1e30); regmbc(0x1e32);
		      regmbc(0x1e34); regmbc(0x2c69); regmbc(0xa740);
		      החזר;
	    حالة 'L': حالة 0x139: حالة 0x13b: حالة 0x13d: حالة 0x13f:
	    حالة 0x141: حالة 0x23d: حالة 0x1e36: حالة 0x1e38:
	    حالة 0x1e3a: حالة 0x1e3c: حالة 0x2c60:
		      regmbc('L'); regmbc(0x139); regmbc(0x13b);
		      regmbc(0x13d); regmbc(0x13f); regmbc(0x141);
		      regmbc(0x23d); regmbc(0x1e36); regmbc(0x1e38);
		      regmbc(0x1e3a); regmbc(0x1e3c); regmbc(0x2c60);
		      החזר;
	    حالة 'M': حالة 0x1e3e: حالة 0x1e40: حالة 0x1e42:
		      regmbc('M'); regmbc(0x1e3e); regmbc(0x1e40);
		      regmbc(0x1e42);
		      החזר;
	    حالة 'N': حالة 0xd1:
	    حالة 0x143: حالة 0x145: حالة 0x147: حالة 0x1f8:
	    حالة 0x1e44: حالة 0x1e46: حالة 0x1e48: حالة 0x1e4a:
	    حالة 0xa7a4:
		      regmbc('N'); regmbc(0xd1);
		      regmbc(0x143); regmbc(0x145); regmbc(0x147);
		      regmbc(0x1f8); regmbc(0x1e44); regmbc(0x1e46);
		      regmbc(0x1e48); regmbc(0x1e4a); regmbc(0xa7a4);
		      החזר;
	    حالة 'O': حالة 0xd2: حالة 0xd3: حالة 0xd4: حالة 0xd5: حالة 0xd6:
	    حالة 0xd8: حالة 0x14c: حالة 0x14e: حالة 0x150: حالة 0x19f:
	    حالة 0x1a0: حالة 0x1d1: حالة 0x1ea: حالة 0x1ec: حالة 0x1fe:
	    حالة 0x20c: حالة 0x20e: حالة 0x22a: حالة 0x22c: حالة 0x22e:
	    حالة 0x230: حالة 0x1e4c: حالة 0x1e4e: حالة 0x1e50: حالة 0x1e52:
	    حالة 0x1ecc: حالة 0x1ece: حالة 0x1ed0: حالة 0x1ed2: حالة 0x1ed4:
	    حالة 0x1ed6: حالة 0x1ed8: حالة 0x1eda: حالة 0x1edc: حالة 0x1ede:
	    حالة 0x1ee0: حالة 0x1ee2:
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
		      החזר;
	    حالة 'P': حالة 0x1a4: حالة 0x1e54: حالة 0x1e56: حالة 0x2c63:
		      regmbc('P'); regmbc(0x1a4); regmbc(0x1e54);
		      regmbc(0x1e56); regmbc(0x2c63);
		      החזר;
	    حالة 'Q': حالة 0x24a:
		      regmbc('Q'); regmbc(0x24a);
		      החזר;
	    حالة 'R': حالة 0x154: حالة 0x156: حالة 0x158: حالة 0x210:
	    حالة 0x212: حالة 0x24c: حالة 0x1e58: حالة 0x1e5a:
	    حالة 0x1e5c: حالة 0x1e5e: حالة 0x2c64: حالة 0xa7a6:
		      regmbc('R'); regmbc(0x154); regmbc(0x156);
		      regmbc(0x210); regmbc(0x212); regmbc(0x158);
		      regmbc(0x24c); regmbc(0x1e58); regmbc(0x1e5a);
		      regmbc(0x1e5c); regmbc(0x1e5e); regmbc(0x2c64);
		      regmbc(0xa7a6);
		      החזר;
	    حالة 'S': حالة 0x15a: حالة 0x15c: حالة 0x15e: حالة 0x160:
	    حالة 0x218: حالة 0x1e60: حالة 0x1e62: حالة 0x1e64:
	    حالة 0x1e66: حالة 0x1e68: حالة 0x2c7e: حالة 0xa7a8:
		      regmbc('S'); regmbc(0x15a); regmbc(0x15c);
		      regmbc(0x15e); regmbc(0x160); regmbc(0x218);
		      regmbc(0x1e60); regmbc(0x1e62); regmbc(0x1e64);
		      regmbc(0x1e66); regmbc(0x1e68); regmbc(0x2c7e);
			  regmbc(0xa7a8);
		      החזר;
	    حالة 'T': حالة 0x162: حالة 0x164: حالة 0x166: حالة 0x1ac:
	    حالة 0x1ae: حالة 0x21a: حالة 0x23e: حالة 0x1e6a: حالة 0x1e6c:
	    حالة 0x1e6e: حالة 0x1e70:
		      regmbc('T'); regmbc(0x162); regmbc(0x164);
		      regmbc(0x166); regmbc(0x1ac); regmbc(0x23e);
		      regmbc(0x1ae); regmbc(0x21a); regmbc(0x1e6a);
		      regmbc(0x1e6c); regmbc(0x1e6e); regmbc(0x1e70);
		      החזר;
	    حالة 'U': حالة 0xd9: حالة 0xda: حالة 0xdb: حالة 0xdc:
	    حالة 0x168: حالة 0x16a: حالة 0x16c: حالة 0x16e:
	    حالة 0x170: حالة 0x172: حالة 0x1af: حالة 0x1d3:
	    حالة 0x1d5: حالة 0x1d7: حالة 0x1d9: حالة 0x1db:
	    حالة 0x214: حالة 0x216: حالة 0x244: حالة 0x1e72:
	    حالة 0x1e74: حالة 0x1e76: حالة 0x1e78: حالة 0x1e7a:
	    حالة 0x1ee4: حالة 0x1ee6: حالة 0x1ee8: حالة 0x1eea:
	    حالة 0x1eec: حالة 0x1eee: حالة 0x1ef0:
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
		      החזר;
	    حالة 'V': حالة 0x1b2: حالة 0x1e7c: حالة 0x1e7e:
		      regmbc('V'); regmbc(0x1b2); regmbc(0x1e7c);
		      regmbc(0x1e7e);
		      החזר;
	    حالة 'W': حالة 0x174: حالة 0x1e80: حالة 0x1e82:
	    حالة 0x1e84: حالة 0x1e86: حالة 0x1e88:
		      regmbc('W'); regmbc(0x174); regmbc(0x1e80);
		      regmbc(0x1e82); regmbc(0x1e84); regmbc(0x1e86);
		      regmbc(0x1e88);
		      החזר;
	    حالة 'X': حالة 0x1e8a: حالة 0x1e8c:
		      regmbc('X'); regmbc(0x1e8a); regmbc(0x1e8c);
		      החזר;
	    حالة 'Y': حالة 0xdd:
	    حالة 0x176: حالة 0x178: حالة 0x1b3: حالة 0x232: حالة 0x24e:
	    حالة 0x1e8e: حالة 0x1ef2: حالة 0x1ef6: حالة 0x1ef4: حالة 0x1ef8:
		      regmbc('Y'); regmbc(0xdd); regmbc(0x176);
		      regmbc(0x178); regmbc(0x1b3); regmbc(0x232);
		      regmbc(0x24e); regmbc(0x1e8e); regmbc(0x1ef2);
		      regmbc(0x1ef4); regmbc(0x1ef6); regmbc(0x1ef8);
		      החזר;
	    حالة 'Z': حالة 0x179: حالة 0x17b: حالة 0x17d: حالة 0x1b5:
	    حالة 0x1e90: حالة 0x1e92: حالة 0x1e94: حالة 0x2c6b:
		      regmbc('Z'); regmbc(0x179); regmbc(0x17b);
		      regmbc(0x17d); regmbc(0x1b5); regmbc(0x1e90);
		      regmbc(0x1e92); regmbc(0x1e94); regmbc(0x2c6b);
		      החזר;
	    حالة 'a': حالة 0xe0: حالة 0xe1: حالة 0xe2:
	    حالة 0xe3: حالة 0xe4: حالة 0xe5: حالة 0x101: حالة 0x103:
	    حالة 0x105: حالة 0x1ce: حالة 0x1df: حالة 0x1e1: حالة 0x1fb:
	    حالة 0x201: حالة 0x203: حالة 0x227: حالة 0x1d8f: حالة 0x1e01:
	    حالة 0x1e9a: حالة 0x1ea1: حالة 0x1ea3: حالة 0x1ea5:
	    حالة 0x1ea7: حالة 0x1ea9: حالة 0x1eab: حالة 0x1ead:
	    حالة 0x1eaf: حالة 0x1eb1: حالة 0x1eb3: حالة 0x1eb5:
	    حالة 0x1eb7: حالة 0x2c65:
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
		      החזר;
	    حالة 'b': حالة 0x180: حالة 0x253: حالة 0x1d6c: حالة 0x1d80:
	    حالة 0x1e03: حالة 0x1e05: حالة 0x1e07:
		      regmbc('b');
		      regmbc(0x180); regmbc(0x253); regmbc(0x1d6c);
		      regmbc(0x1d80); regmbc(0x1e03); regmbc(0x1e05);
		      regmbc(0x1e07);
		      החזר;
	    حالة 'c': حالة 0xe7:
	    حالة 0x107: حالة 0x109: حالة 0x10b: حالة 0x10d: حالة 0x188:
	    حالة 0x23c: حالة 0x1e09: حالة 0xa793: حالة 0xa794:
		      regmbc('c'); regmbc(0xe7); regmbc(0x107);
		      regmbc(0x109); regmbc(0x10b); regmbc(0x10d);
		      regmbc(0x188); regmbc(0x23c); regmbc(0x1e09);
		      regmbc(0xa793); regmbc(0xa794);
		      החזר;
	    حالة 'd': حالة 0x10f: حالة 0x111: حالة 0x257: حالة 0x1d6d:
	    حالة 0x1d81: حالة 0x1d91: حالة 0x1e0b: حالة 0x1e0d:
	    حالة 0x1e0f: حالة 0x1e11: حالة 0x1e13:
		      regmbc('d'); regmbc(0x10f); regmbc(0x111);
		      regmbc(0x257); regmbc(0x1d6d); regmbc(0x1d81);
		      regmbc(0x1d91); regmbc(0x1e0b); regmbc(0x1e0d);
		      regmbc(0x1e0f); regmbc(0x1e11); regmbc(0x1e13);
		      החזר;
	    حالة 'e': حالة 0xe8: حالة 0xe9: حالة 0xea: حالة 0xeb:
	    حالة 0x113: حالة 0x115: حالة 0x117: حالة 0x119:
	    حالة 0x11b: حالة 0x205: حالة 0x207: حالة 0x229:
	    حالة 0x247: حالة 0x1d92: حالة 0x1e15: حالة 0x1e17:
	    حالة 0x1e19: حالة 0x1e1b: حالة 0x1eb9: حالة 0x1ebb:
	    حالة 0x1e1d: حالة 0x1ebd: حالة 0x1ebf: حالة 0x1ec1:
	    حالة 0x1ec3: حالة 0x1ec5: حالة 0x1ec7:
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
		      החזר;
	    حالة 'f': حالة 0x192: حالة 0x1d6e: حالة 0x1d82:
	    حالة 0x1e1f: حالة 0xa799:
		     regmbc('f'); regmbc(0x192); regmbc(0x1d6e);
		     regmbc(0x1d82); regmbc(0x1e1f); regmbc(0xa799);
		     החזר;
	    حالة 'g': حالة 0x11d: حالة 0x11f: حالة 0x121: حالة 0x123:
	    حالة 0x1e5: حالة 0x1e7: حالة 0x260: حالة 0x1f5: حالة 0x1d83:
	    حالة 0x1e21: حالة 0xa7a1:
		      regmbc('g'); regmbc(0x11d); regmbc(0x11f);
		      regmbc(0x121); regmbc(0x123); regmbc(0x1e5);
		      regmbc(0x1e7); regmbc(0x1f5); regmbc(0x260);
		      regmbc(0x1d83); regmbc(0x1e21); regmbc(0xa7a1);
		      החזר;
	    حالة 'h': حالة 0x125: حالة 0x127: حالة 0x21f: حالة 0x1e23:
	    حالة 0x1e25: حالة 0x1e27: حالة 0x1e29: حالة 0x1e2b:
	    حالة 0x1e96: حالة 0x2c68: حالة 0xa795:
		      regmbc('h'); regmbc(0x125); regmbc(0x127);
		      regmbc(0x21f); regmbc(0x1e23); regmbc(0x1e25);
		      regmbc(0x1e27); regmbc(0x1e29); regmbc(0x1e2b);
		      regmbc(0x1e96); regmbc(0x2c68); regmbc(0xa795);
		      החזר;
	    حالة 'i': حالة 0xec: حالة 0xed: حالة 0xee: حالة 0xef:
	    حالة 0x129: حالة 0x12b: حالة 0x12d: حالة 0x12f:
	    حالة 0x1d0: حالة 0x209: حالة 0x20b: حالة 0x268:
	    حالة 0x1d96: حالة 0x1e2d: حالة 0x1e2f: حالة 0x1ec9:
	    حالة 0x1ecb:
		      regmbc('i'); regmbc(0xec); regmbc(0xed);
		      regmbc(0xee); regmbc(0xef); regmbc(0x129);
		      regmbc(0x12b); regmbc(0x12d); regmbc(0x12f);
		      regmbc(0x1d0); regmbc(0x209); regmbc(0x20b);
		      regmbc(0x268); regmbc(0x1d96); regmbc(0x1e2d);
		      regmbc(0x1e2f); regmbc(0x1ec9); regmbc(0x1ecb);
		      החזר;
	    حالة 'j': حالة 0x135: حالة 0x1f0: حالة 0x249:
		      regmbc('j'); regmbc(0x135); regmbc(0x1f0);
		      regmbc(0x249);
		      החזר;
	    حالة 'k': حالة 0x137: حالة 0x199: حالة 0x1e9:
	    حالة 0x1d84: حالة 0x1e31: حالة 0x1e33: حالة 0x1e35:
	    حالة 0x2c6a: حالة 0xa741:
		      regmbc('k'); regmbc(0x137); regmbc(0x199);
		      regmbc(0x1e9); regmbc(0x1d84); regmbc(0x1e31);
		      regmbc(0x1e33); regmbc(0x1e35); regmbc(0x2c6a);
		      regmbc(0xa741);
		      החזר;
	    حالة 'l': حالة 0x13a: حالة 0x13c: حالة 0x13e:
	    حالة 0x140: حالة 0x142: حالة 0x19a: حالة 0x1e37:
	    حالة 0x1e39: حالة 0x1e3b: حالة 0x1e3d: حالة 0x2c61:
		      regmbc('l'); regmbc(0x13a); regmbc(0x13c);
		      regmbc(0x13e); regmbc(0x140); regmbc(0x142);
		      regmbc(0x19a); regmbc(0x1e37); regmbc(0x1e39);
		      regmbc(0x1e3b); regmbc(0x1e3d); regmbc(0x2c61);
		      החזר;
	    حالة 'm': حالة 0x1d6f: حالة 0x1e3f: حالة 0x1e41: حالة 0x1e43:
		      regmbc('m'); regmbc(0x1d6f); regmbc(0x1e3f);
		      regmbc(0x1e41); regmbc(0x1e43);
		      החזר;
	    حالة 'n': حالة 0xf1: حالة 0x144: حالة 0x146: حالة 0x148:
	    حالة 0x149: حالة 0x1f9: حالة 0x1d70: حالة 0x1d87:
	    حالة 0x1e45: حالة 0x1e47: حالة 0x1e49: حالة 0x1e4b:
	    حالة 0xa7a5:
		      regmbc('n'); regmbc(0xf1); regmbc(0x144);
		      regmbc(0x146); regmbc(0x148); regmbc(0x149);
		      regmbc(0x1f9); regmbc(0x1d70); regmbc(0x1d87);
		      regmbc(0x1e45); regmbc(0x1e47); regmbc(0x1e49);
		      regmbc(0x1e4b); regmbc(0xa7a5);
		      החזר;
	    حالة 'o': حالة 0xf2: حالة 0xf3: حالة 0xf4: حالة 0xf5:
	    حالة 0xf6: حالة 0xf8: حالة 0x14d: حالة 0x14f: حالة 0x151:
	    حالة 0x1a1: حالة 0x1d2: حالة 0x1eb: حالة 0x1ed: حالة 0x1ff:
	    حالة 0x20d: حالة 0x20f: حالة 0x22b: حالة 0x22d: حالة 0x22f:
	    حالة 0x231: حالة 0x275: حالة 0x1e4d: حالة 0x1e4f:
	    حالة 0x1e51: حالة 0x1e53: حالة 0x1ecd: حالة 0x1ecf:
	    حالة 0x1ed1: حالة 0x1ed3: حالة 0x1ed5: حالة 0x1ed7:
	    حالة 0x1ed9: حالة 0x1edb: حالة 0x1edd: حالة 0x1edf:
	    حالة 0x1ee1: حالة 0x1ee3:
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
		      החזר;
	    حالة 'p': حالة 0x1a5: حالة 0x1d71: حالة 0x1d88: حالة 0x1d7d:
	    حالة 0x1e55: حالة 0x1e57:
		      regmbc('p'); regmbc(0x1a5); regmbc(0x1d71);
		      regmbc(0x1d7d); regmbc(0x1d88); regmbc(0x1e55);
		      regmbc(0x1e57);
		      החזר;
	    حالة 'q': حالة 0x24b: حالة 0x2a0:
		      regmbc('q'); regmbc(0x24b); regmbc(0x2a0);
		      החזר;
	    حالة 'r': حالة 0x155: حالة 0x157: حالة 0x159: حالة 0x211:
	    حالة 0x213: حالة 0x24d: حالة 0x27d: حالة 0x1d72: حالة 0x1d73:
	    حالة 0x1d89: حالة 0x1e59: حالة 0x1e5b: حالة 0x1e5d: حالة 0x1e5f:
	    حالة 0xa7a7:
		      regmbc('r'); regmbc(0x155); regmbc(0x157);
		      regmbc(0x159); regmbc(0x211); regmbc(0x213);
		      regmbc(0x24d); regmbc(0x1d72); regmbc(0x1d73);
		      regmbc(0x1d89); regmbc(0x1e59); regmbc(0x27d);
		      regmbc(0x1e5b); regmbc(0x1e5d); regmbc(0x1e5f);
		      regmbc(0xa7a7);
		      החזר;
	    حالة 's': حالة 0x15b: حالة 0x15d: حالة 0x15f: حالة 0x161:
	    حالة 0x1e61: حالة 0x219: حالة 0x23f: حالة 0x1d74: حالة 0x1d8a:
	    حالة 0x1e63: حالة 0x1e65: حالة 0x1e67: حالة 0x1e69: حالة 0xa7a9:
		      regmbc('s'); regmbc(0x15b); regmbc(0x15d);
		      regmbc(0x15f); regmbc(0x161); regmbc(0x23f);
		      regmbc(0x219); regmbc(0x1d74); regmbc(0x1d8a);
		      regmbc(0x1e61); regmbc(0x1e63); regmbc(0x1e65);
		      regmbc(0x1e67); regmbc(0x1e69); regmbc(0xa7a9);
		      החזר;
	    حالة 't': حالة 0x163: حالة 0x165: حالة 0x167: حالة 0x1ab:
	    حالة 0x1ad: حالة 0x21b: حالة 0x288: حالة 0x1d75: حالة 0x1e6b:
	    حالة 0x1e6d: حالة 0x1e6f: حالة 0x1e71: حالة 0x1e97: حالة 0x2c66:
		      regmbc('t'); regmbc(0x163); regmbc(0x165);
		      regmbc(0x167); regmbc(0x1ab); regmbc(0x21b);
		      regmbc(0x1ad); regmbc(0x288); regmbc(0x1d75);
		      regmbc(0x1e6b); regmbc(0x1e6d); regmbc(0x1e6f);
		      regmbc(0x1e71); regmbc(0x1e97); regmbc(0x2c66);
		      החזר;
	    حالة 'u': حالة 0xf9: حالة 0xfa: حالة 0xfb: حالة 0xfc:
	    حالة 0x169: حالة 0x16b: حالة 0x16d: حالة 0x16f:
	    حالة 0x171: حالة 0x173: حالة 0x1b0: حالة 0x1d4:
	    حالة 0x1d6: حالة 0x1d8: حالة 0x1da: حالة 0x1dc:
	    حالة 0x215: حالة 0x217: حالة 0x289: حالة 0x1e73:
	    حالة 0x1d7e: حالة 0x1d99: حالة 0x1e75: حالة 0x1e77:
	    حالة 0x1e79: حالة 0x1e7b: حالة 0x1ee5: حالة 0x1ee7:
	    حالة 0x1ee9: حالة 0x1eeb: حالة 0x1eed: حالة 0x1eef:
	    حالة 0x1ef1:
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
		      החזר;
	    حالة 'v': حالة 0x28b: حالة 0x1d8c: حالة 0x1e7d: حالة 0x1e7f:
		      regmbc('v'); regmbc(0x28b); regmbc(0x1d8c);
		      regmbc(0x1e7d); regmbc(0x1e7f);
		      החזר;
	    حالة 'w': حالة 0x175: حالة 0x1e81: حالة 0x1e83:
	    حالة 0x1e85: حالة 0x1e87: حالة 0x1e89: حالة 0x1e98:
		      regmbc('w'); regmbc(0x175); regmbc(0x1e81);
		      regmbc(0x1e83); regmbc(0x1e85); regmbc(0x1e87);
		      regmbc(0x1e89); regmbc(0x1e98);
		      החזר;
	    حالة 'x': حالة 0x1e8b: حالة 0x1e8d:
		      regmbc('x'); regmbc(0x1e8b); regmbc(0x1e8d);
		      החזר;
	    حالة 'y': حالة 0xfd: حالة 0xff: حالة 0x177: حالة 0x1b4:
	    حالة 0x233: حالة 0x24f: حالة 0x1e8f: حالة 0x1e99: حالة 0x1ef3:
	    حالة 0x1ef5: حالة 0x1ef7: حالة 0x1ef9:
		      regmbc('y'); regmbc(0xfd); regmbc(0xff);
		      regmbc(0x177); regmbc(0x1b4); regmbc(0x233);
		      regmbc(0x24f); regmbc(0x1e8f); regmbc(0x1e99);
		      regmbc(0x1ef3); regmbc(0x1ef5); regmbc(0x1ef7);
		      regmbc(0x1ef9);
		      החזר;
	    حالة 'z': حالة 0x17a: حالة 0x17c: حالة 0x17e: حالة 0x1b6:
	    حالة 0x1d76: حالة 0x1d8e: حالة 0x1e91: حالة 0x1e93:
	    حالة 0x1e95: حالة 0x2c6c:
		      regmbc('z'); regmbc(0x17a); regmbc(0x17c);
		      regmbc(0x17e); regmbc(0x1b6); regmbc(0x1d76);
		      regmbc(0x1d8e); regmbc(0x1e91); regmbc(0x1e93);
		      regmbc(0x1e95); regmbc(0x2c6c);
		      החזר;
	}
    }
    regmbc(c);
}

/*
 * Emit a node.
 * Return pointer to generated code.
 */
    ساكن char_u *
regnode(صحيح op)
{
    char_u  *ret;

    ret = regcode;
    אם (ret == JUST_CALC_SIZE)
	regsize += 3;
    אחרת
    {
	*regcode++ = op;
	*regcode++ = NUL;		// Null "next" pointer.
	*regcode++ = NUL;
    }
    החזר ret;
}

/*
 * Write a long as four bytes at "p" and return pointer to the next char.
 */
    ساكن char_u *
re_put_long(char_u *p, long_u val)
{
    *p++ = (char_u) ((val >> 24) & 0377);
    *p++ = (char_u) ((val >> 16) & 0377);
    *p++ = (char_u) ((val >> 8) & 0377);
    *p++ = (char_u) (val & 0377);
    החזר p;
}

/*
 * regnext - dig the "next" pointer out of a node
 * Returns NULL when calculating size, when there is no next item and when
 * there is an error.
 */
    ساكن char_u *
regnext(char_u *p)
{
    صحيح	    offset;

    אם (p == JUST_CALC_SIZE || reg_toolong)
	החזר عدم;

    offset = NEXT(p);
    אם (offset == 0)
	החזר عدم;

    אם (OP(p) == BACK)
	החזר p - offset;
    אחרת
	החזר p + offset;
}

/*
 * Set the next-pointer at the end of a node chain.
 */
    ساكن فراغ
regtail(char_u *p, char_u *val)
{
    char_u	*scan;
    char_u	*temp;
    صحيح		offset;

    אם (p == JUST_CALC_SIZE)
	החזר;

    // Find last node.
    scan = p;
    עבור (;;)
    {
	temp = regnext(scan);
	אם (temp == عدم)
	    اكسر;
	scan = temp;
    }

    אם (OP(scan) == BACK)
	offset = (صحيح)(scan - val);
    אחרת
	offset = (صحيح)(val - scan);
    // When the offset uses more than 16 bits it can no longer fit in the two
    // bytes available.  Use a global flag to avoid having to check return
    // values in too many places.
    אם (offset > 0xffff)
	reg_toolong = TRUE;
    אחרת
    {
	*(scan + 1) = (char_u) (((unsigned)offset >> 8) & 0377);
	*(scan + 2) = (char_u) (offset & 0377);
    }
}

/*
 * Like regtail, on item after a BRANCH; nop if none.
 */
    ساكن فراغ
regoptail(char_u *p, char_u *val)
{
    // When op is neither BRANCH nor BRACE_COMPLEX0-9, it is "operandless"
    אם (p == عدم || p == JUST_CALC_SIZE
	    || (OP(p) != BRANCH
		&& (OP(p) < BRACE_COMPLEX || OP(p) > BRACE_COMPLEX + 9)))
	החזר;
    regtail(OPERAND(p), val);
}

/*
 * Insert an operator in front of already-emitted operand
 *
 * Means relocating the operand.
 */
    ساكن فراغ
reginsert(صحيح op, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    אם (regcode == JUST_CALC_SIZE)
    {
	regsize += 3;
	החזר;
    }
    src = regcode;
    regcode += 3;
    dst = regcode;
    כלעוד (src > opnd)
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
    ساكن فراغ
reginsert_nr(صحيح op, long val, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    אם (regcode == JUST_CALC_SIZE)
    {
	regsize += 7;
	החזר;
    }
    src = regcode;
    regcode += 7;
    dst = regcode;
    כלעוד (src > opnd)
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
    ساكن فراغ
reginsert_limits(
    صحيح		op,
    long	minval,
    long	maxval,
    char_u	*opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    אם (regcode == JUST_CALC_SIZE)
    {
	regsize += 11;
	החזר;
    }
    src = regcode;
    regcode += 11;
    dst = regcode;
    כלעוד (src > opnd)
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
    ساكن صحيح
seen_endbrace(صحيح refnum)
{
    אם (!had_endbrace[refnum])
    {
	char_u *p;

	// Trick: check if "@<=" or "@<!" follows, in which case
	// the \1 can appear before the referenced match.
	עבור (p = regparse; *p != NUL; ++p)
	    אם (p[0] == '@' && p[1] == '<' && (p[2] == '!' || p[2] == '='))
		اكسر;
	אם (*p == NUL)
	{
	    emsg(_(e_illegal_back_reference));
	    rc_did_emsg = TRUE;
	    החזר FALSE;
	}
    }
    החזר TRUE;
}

/*
 * Parse the lowest level.
 *
 * Optimization:  gobbles an entire sequence of ordinary characters so that
 * it can turn them into a single node, which is smaller to store and
 * faster to run.  Don't do this when one_exactly is set.
 */
    ساكن char_u *
regatom(صحيح *flagp)
{
    char_u	    *ret;
    صحيح		    flags;
    صحيح		    c;
    char_u	    *p;
    صحيح		    extra = 0;
    صحيح		    save_prev_at_start = prev_at_start;

    *flagp = WORST;		// Tentatively.

    c = getchr();
    بدل (c)
    {
      حالة Magic('^'):
	ret = regnode(BOL);
	اكسر;

      حالة Magic('$'):
	ret = regnode(EOL);
#אם defined(FEAT_SYN_HL)
	had_eol = TRUE;
#endif
	اكسر;

      حالة Magic('<'):
	ret = regnode(BOW);
	اكسر;

      حالة Magic('>'):
	ret = regnode(EOW);
	اكسر;

      حالة Magic('_'):
	c = no_Magic(getchr());
	אם (c == '^')		// "\_^" is start-of-line
	{
	    ret = regnode(BOL);
	    اكسر;
	}
	אם (c == '$')		// "\_$" is end-of-line
	{
	    ret = regnode(EOL);
#אם defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#endif
	    اكسر;
	}

	extra = ADD_NL;
	*flagp |= HASNL;

	// "\_[" is character range plus newline
	אם (c == '[')
	    اذهب collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	// Character classes.
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
	אם (p == عدم)
	    EMSG_RET_NULL(_(e_invalid_use_of_underscore));

	// When '.' is followed by a composing char ignore the dot, so that
	// the composing char is matched here.
	אם (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	{
	    c = getchr();
	    اذهب do_multibyte;
	}
	ret = regnode(classcodes[p - classchars] + extra);
	*flagp |= HASWIDTH | SIMPLE;
	اكسر;

      حالة Magic('n'):
	אם (reg_string)
	{
	    // In a string "\n" matches a newline character.
	    ret = regnode(EXACTLY);
	    regc(NL);
	    regc(NUL);
	    *flagp |= HASWIDTH | SIMPLE;
	}
	אחרת
	{
	    // In buffer text "\n" matches the end of a line.
	    ret = regnode(NEWL);
	    *flagp |= HASWIDTH | HASNL;
	}
	اكسر;

      حالة Magic('('):
	אם (one_exactly)
	    EMSG_ONE_RET_NULL;
	ret = reg(REG_PAREN, &flags);
	אם (ret == عدم)
	    החזר عدم;
	*flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
	اكسر;

      حالة NUL:
      حالة Magic('|'):
      حالة Magic('&'):
      حالة Magic(')'):
	אם (one_exactly)
	    EMSG_ONE_RET_NULL;
	// Supposed to be caught earlier.
	IEMSG_RET_NULL(e_internal_error_in_regexp);
	// NOTREACHED

      حالة Magic('='):
      حالة Magic('?'):
      حالة Magic('+'):
      حالة Magic('@'):
      حالة Magic('{'):
      حالة Magic('*'):
	c = no_Magic(c);
	EMSG3_RET_NULL(_(e_str_chr_follows_nothing),
		(c == '*' ? reg_magic >= MAGIC_ON : reg_magic == MAGIC_ALL), c);
	// NOTREACHED

      حالة Magic('~'):		// previous substitute pattern
	    אם (reg_prev_sub != عدم)
	    {
		char_u	    *lp;

		ret = regnode(EXACTLY);
		lp = reg_prev_sub;
		כלעוד (*lp != NUL)
		    regc(*lp++);
		regc(NUL);
		אם (*reg_prev_sub != NUL)
		{
		    *flagp |= HASWIDTH;
		    אם ((lp - reg_prev_sub) == 1)
			*flagp |= SIMPLE;
		}
	    }
	    אחרת
		EMSG_RET_NULL(_(e_no_previous_substitute_regular_expression));
	    اكسر;

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
		صحيح		    refnum;

		refnum = c - Magic('0');
		אם (!seen_endbrace(refnum))
		    החזר عدم;
		ret = regnode(BACKREF + refnum);
	    }
	    اكسر;

      حالة Magic('z'):
	{
	    c = no_Magic(getchr());
	    بدل (c)
	    {
#ifdef FEAT_SYN_HL
		حالة '(': אם ((reg_do_extmatch & REX_SET) == 0)
			      EMSG_RET_NULL(_(e_z_not_allowed_here));
			  אם (one_exactly)
			      EMSG_ONE_RET_NULL;
			  ret = reg(REG_ZPAREN, &flags);
			  אם (ret == عدم)
			      החזר عدم;
			  *flagp |= flags & (HASWIDTH|SPSTART|HASNL|HASLOOKBH);
			  re_has_z = REX_SET;
			  اكسر;

		حالة '1':
		حالة '2':
		حالة '3':
		حالة '4':
		حالة '5':
		حالة '6':
		حالة '7':
		حالة '8':
		حالة '9': אם ((reg_do_extmatch & REX_USE) == 0)
			      EMSG_RET_NULL(_(e_z1_z9_not_allowed_here));
			  ret = regnode(ZREF + c - '0');
			  re_has_z = REX_USE;
			  اكسر;
#endif

		حالة 's': ret = regnode(MOPEN + 0);
			  אם (re_mult_next("\\zs") == FAIL)
			      החזר عدم;
			  اكسر;

		حالة 'e': ret = regnode(MCLOSE + 0);
			  אם (re_mult_next("\\ze") == FAIL)
			      החזר عدم;
			  اكسر;

		افتراضي:  EMSG_RET_NULL(_(e_invalid_character_after_bsl_z));
	    }
	}
	اكسر;

      حالة Magic('%'):
	{
	    c = no_Magic(getchr());
	    بدل (c)
	    {
		// () without a back reference
		حالة '(':
		    אם (one_exactly)
			EMSG_ONE_RET_NULL;
		    ret = reg(REG_NPAREN, &flags);
		    אם (ret == عدم)
			החזר عدم;
		    *flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
		    اكسر;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		حالة '^':
		    ret = regnode(RE_BOF);
		    اكسر;

		حالة '$':
		    ret = regnode(RE_EOF);
		    اكسر;

		حالة '#':
		    אם (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_start_of_pattern),
								  regparse[1]);
			החזר FAIL;
		    }
		    ret = regnode(CURSOR);
		    اكسر;

		حالة 'V':
		    ret = regnode(RE_VISUAL);
		    اكسر;

		حالة 'C':
		    ret = regnode(RE_COMPOSING);
		    اكسر;

		// \%[abc]: Emit as a list of branches, all ending at the last
		// branch which matches nothing.
		حالة '[':
			  אם (one_exactly)	// doesn't nest
			      EMSG_ONE_RET_NULL;
			  {
			      char_u	*lastbranch;
			      char_u	*lastnode = عدم;
			      char_u	*br;

			      ret = عدم;
			      כלעוד ((c = getchr()) != ']')
			      {
				  אם (c == NUL)
				      EMSG2_RET_NULL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
				  br = regnode(BRANCH);
				  אם (ret == عدم)
				      ret = br;
				  אחרת
				  {
				      regtail(lastnode, br);
				      אם (reg_toolong)
					  החזר عدم;
				  }

				  ungetchr();
				  one_exactly = TRUE;
				  lastnode = regatom(flagp);
				  one_exactly = FALSE;
				  אם (lastnode == عدم)
				      החזר عدم;
			      }
			      אם (ret == عدم)
				  EMSG2_RET_NULL(_(e_empty_str_brackets),
						      reg_magic == MAGIC_ALL);
			      lastbranch = regnode(BRANCH);
			      br = regnode(NOTHING);
			      אם (ret != JUST_CALC_SIZE)
			      {
				  regtail(lastnode, br);
				  regtail(lastbranch, br);
				  // connect all branches to the NOTHING
				  // branch at the end
				  עבור (br = ret; br != lastnode; )
				  {
				      אם (OP(br) == BRANCH)
				      {
					  regtail(br, lastbranch);
					  אם (reg_toolong)
					      החזר عدم;
					  br = OPERAND(br);
				      }
				      אחרת
					  br = regnext(br);
				  }
			      }
			      *flagp &= ~(HASWIDTH | SIMPLE);
			      اكسر;
			  }

		حالة 'd':   // %d123 decimal
		حالة 'o':   // %o123 octal
		حالة 'x':   // %xab hex 2
		حالة 'u':   // %uabcd hex 4
		حالة 'U':   // %U1234abcd hex 8
			  {
			      vimlong_T i;

			      بدل (c)
			      {
				  حالة 'd': i = getdecchrs(); اكسر;
				  حالة 'o': i = getoctchrs(); اكسر;
				  حالة 'x': i = gethexchrs(2); اكسر;
				  حالة 'u': i = gethexchrs(4); اكسر;
				  حالة 'U': i = gethexchrs(8); اكسر;
				  افتراضي:  i = -1; اكسر;
			      }

			      אם (i < 0 || i > INT_MAX)
				  EMSG2_RET_NULL(
					    _(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			      אם (use_multibytecode(i))
				  ret = regnode(MULTIBYTECODE);
			      אחרת
				  ret = regnode(EXACTLY);
			      אם (i == 0)
				  regc(0x0a);
			      אחרת
				  regmbc((صحيح)i);
			      regc(NUL);
			      *flagp |= HASWIDTH;
			      اكسر;
			  }

		افتراضي:
			  אם (VIM_ISDIGIT(c) || c == '<' || c == '>'
						|| c == '\'' || c == '.')
			  {
			      long_u	n = 0;
			      صحيح	cmp;
			      صحيح	cur = FALSE;
			      صحيح	got_digit = FALSE;

			      cmp = c;
			      אם (cmp == '<' || cmp == '>')
				  c = getchr();
			      אם (no_Magic(c) == '.')
			      {
				  cur = TRUE;
				  c = getchr();
			      }
			      כלעוד (VIM_ISDIGIT(c))
			      {
				  got_digit = TRUE;
				  n = n * 10 + (c - '0');
				  c = getchr();
			      }
			      אם (no_Magic(c) == '\'' && n == 0)
			      {
				  // "\%'m", "\%<'m" and "\%>'m": Mark
				  c = getchr();
				  ret = regnode(RE_MARK);
				  אם (ret == JUST_CALC_SIZE)
				      regsize += 2;
				  אחרת
				  {
				      *regcode++ = c;
				      *regcode++ = cmp;
				  }
				  اكسر;
			      }
			      אחרת אם ((c == 'l' || c == 'c' || c == 'v')
					  && (cur || got_digit))
			      {
				  אם (cur && n)
				  {
				    semsg(_(e_regexp_number_after_dot_pos_search_chr),
								  no_Magic(c));
				    rc_did_emsg = TRUE;
				    החזר عدم;
				  }
				  אם (c == 'l')
				  {
				      אם (cur)
					  n = curwin->w_cursor.lnum;
				      ret = regnode(RE_LNUM);
				      אם (save_prev_at_start)
					  at_start = TRUE;
				  }
				  אחרת אם (c == 'c')
				  {
				      אם (cur)
				      {
					  n = curwin->w_cursor.col;
					  n++;
				      }
				      ret = regnode(RE_COL);
				  }
				  אחרת
				  {
				      אם (cur)
				      {
					  colnr_T vcol = 0;

					  getvvcol(curwin, &curwin->w_cursor,
							 عدم, عدم, &vcol, 0);
					  ++vcol;
					  n = vcol;
				      }
				      ret = regnode(RE_VCOL);
				  }
				  אם (ret == JUST_CALC_SIZE)
				      regsize += 5;
				  אחרת
				  {
				      // put the number and the optional
				      // comparator after the opcode
				      regcode = re_put_long(regcode, n);
				      *regcode++ = cmp;
				  }
				  اكسر;
			      }
			  }

			  EMSG2_RET_NULL(_(e_invalid_character_after_str),
						      reg_magic == MAGIC_ALL);
	    }
	}
	اكسر;

      حالة Magic('['):
collection:
	{
	    char_u	*lp;

	    // If there is no matching ']', we assume the '[' is a normal
	    // character.  This makes 'incsearch' and ":help [" work.
	    lp = skip_anyof(regparse);
	    אם (*lp == ']')	// there is a matching ']'
	    {
		صحيح	startc = -1;	// > 0 when next '-' is a range
		صحيح	endc;

		אם (extra == 0)
		{
		    صحيح	cl = bt_recognize_char_class(regparse, lp);

		    אם (cl != 0)
		    {
			ret = regnode(cl);
			regparse = lp;
			skipchr();
			*flagp |= HASWIDTH | SIMPLE;
			اكسر;
		    }
		}

		// In a character class, different parsing rules apply.
		// Not even \ is special anymore, nothing is.
		אם (*regparse == '^')	    // Complement of range.
		{
		    ret = regnode(ANYBUT + extra);
		    regparse++;
		}
		אחרת
		    ret = regnode(ANYOF + extra);

		// At the start ']' and '-' mean the literal character.
		אם (*regparse == ']' || *regparse == '-')
		{
		    startc = *regparse;
		    regc(*regparse++);
		}

		כלעוד (*regparse != NUL && *regparse != ']')
		{
		    אם (*regparse == '-')
		    {
			++regparse;
			// The '-' is not used for a range at the end and
			// after or before a '\n'.
			אם (*regparse == ']' || *regparse == NUL
				|| startc == -1
				|| (regparse[0] == '\\' && regparse[1] == 'n'))
			{
			    regc('-');
			    startc = '-';	// [--x] is a range
			}
			אחרת
			{
			    // Also accept "a-[.z.]"
			    endc = 0;
			    אם (*regparse == '[')
				endc = get_coll_element(&regparse);
			    אם (endc == 0)
			    {
				אם (has_mbyte)
				    endc = mb_ptr2char_adv(&regparse);
				אחרת
				    endc = *regparse++;
			    }

			    // Handle \o40, \x20 and \u20AC style sequences
			    אם (endc == '\\' && !reg_cpo_lit && !reg_cpo_bsl)
				endc = coll_get_char();

			    אם (startc > endc)
				EMSG_RET_NULL(_(e_reverse_range_in_character_class));
			    אם (has_mbyte && ((*mb_char2len)(startc) > 1
						 || (*mb_char2len)(endc) > 1))
			    {
				// Limit to a range of 256 chars.
				אם (endc > startc + 256)
				    EMSG_RET_NULL(_(e_range_too_large_in_character_class));
				כלעוד (++startc <= endc)
				    regmbc(startc);
			    }
			    אחרת
			    {
				כלעוד (++startc <= endc)
				    regc(startc);
			    }
			    startc = -1;
			}
		    }
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    אחרת אם (*regparse == '\\'
			    && !reg_cpo_bsl
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != عدم
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR,
						       regparse[1]) != عدم)))
		    {
			regparse++;
			אם (*regparse == 'n')
			{
			    // '\n' in range: also match NL
			    אם (ret != JUST_CALC_SIZE)
			    {
				// Using \n inside [^] does not change what
				// matches. "[^\n]" is the same as ".".
				אם (*ret == ANYOF)
				{
				    *ret = ANYOF + ADD_NL;
				    *flagp |= HASNL;
				}
				// else: must have had a \n already
			    }
			    regparse++;
			    startc = -1;
			}
			אחרת אם (*regparse == 'd'
				|| *regparse == 'o'
				|| *regparse == 'x'
				|| *regparse == 'u'
				|| *regparse == 'U')
			{
			    startc = coll_get_char();
			    // max UTF-8 Codepoint is U+10FFFF,
			    // but allow values until INT_MAX
			    אם (startc == INT_MAX)
				EMSG_RET_NULL(_(e_unicode_val_too_large));
			    אם (startc == 0)
				regc(0x0a);
			    אחרת
				regmbc(startc);
			}
			אחרת
			{
			    startc = backslash_trans(*regparse++);
			    regc(startc);
			}
		    }
		    אחרת אם (*regparse == '[')
		    {
			صحيح c_class;
			صحيح cu;

			c_class = get_char_class(&regparse);
			startc = -1;
			// Characters assumed to be 8 bits!
			بدل (c_class)
			{
			    حالة CLASS_NONE:
				c_class = get_equi_class(&regparse);
				אם (c_class != 0)
				{
				    // produce equivalence class
				    reg_equi_class(c_class);
				}
				אחרת אם ((c_class =
					    get_coll_element(&regparse)) != 0)
				{
				    // produce a collating element
				    regmbc(c_class);
				}
				אחרת
				{
				    // literal '[', allow [[-x] as a range
				    startc = *regparse++;
				    regc(startc);
				}
				اكسر;
			    حالة CLASS_ALNUM:
				עבור (cu = 1; cu < 128; cu++)
				    אם (isalnum(cu))
					regmbc(cu);
				اكسر;
			    حالة CLASS_ALPHA:
				עבור (cu = 1; cu < 128; cu++)
				    אם (isalpha(cu))
					regmbc(cu);
				اكسر;
			    حالة CLASS_BLANK:
				regc(' ');
				regc('\t');
				اكسر;
			    حالة CLASS_CNTRL:
				עבור (cu = 1; cu <= 127; cu++)
				    אם (iscntrl(cu))
					regmbc(cu);
				اكسر;
			    حالة CLASS_DIGIT:
				עבור (cu = 1; cu <= 127; cu++)
				    אם (VIM_ISDIGIT(cu))
					regmbc(cu);
				اكسر;
			    حالة CLASS_GRAPH:
				עבור (cu = 1; cu <= 127; cu++)
				    אם (isgraph(cu))
					regmbc(cu);
				اكسر;
			    حالة CLASS_LOWER:
				עבור (cu = 1; cu <= 255; cu++)
				    אם (MB_ISLOWER(cu) && cu != 170
								 && cu != 186)
					regmbc(cu);
				اكسر;
			    حالة CLASS_PRINT:
				עבור (cu = 1; cu <= 255; cu++)
				    אם (vim_isprintc(cu))
					regmbc(cu);
				اكسر;
			    حالة CLASS_PUNCT:
				עבור (cu = 1; cu < 128; cu++)
				    אם (ispunct(cu))
					regmbc(cu);
				اكسر;
			    حالة CLASS_SPACE:
				עבור (cu = 9; cu <= 13; cu++)
				    regc(cu);
				regc(' ');
				اكسر;
			    حالة CLASS_UPPER:
				עבור (cu = 1; cu <= 255; cu++)
				    אם (MB_ISUPPER(cu))
					regmbc(cu);
				اكسر;
			    حالة CLASS_XDIGIT:
				עבור (cu = 1; cu <= 255; cu++)
				    אם (vim_isxdigit(cu))
					regmbc(cu);
				اكسر;
			    حالة CLASS_TAB:
				regc('\t');
				اكسر;
			    حالة CLASS_RETURN:
				regc('\r');
				اكسر;
			    حالة CLASS_BACKSPACE:
				regc('\b');
				اكسر;
			    حالة CLASS_ESCAPE:
				regc('\033');
				اكسر;
			    حالة CLASS_IDENT:
				עבור (cu = 1; cu <= 255; cu++)
				    אם (vim_isIDc(cu))
					regmbc(cu);
				اكسر;
			    حالة CLASS_KEYWORD:
				עבור (cu = 1; cu <= 255; cu++)
				    אם (reg_iswordc(cu))
					regmbc(cu);
				اكسر;
			    حالة CLASS_FNAME:
				עבור (cu = 1; cu <= 255; cu++)
				    אם (vim_isfilec(cu))
					regmbc(cu);
				اكسر;
			}
		    }
		    אחרת
		    {
			אם (has_mbyte)
			{
			    صحيح	len;

			    // produce a multibyte character, including any
			    // following composing characters
			    startc = mb_ptr2char(regparse);
			    len = (*mb_ptr2len)(regparse);
			    אם (enc_utf8 && utf_char2len(startc) != len)
				startc = -1;	// composing chars
			    כלעוד (--len >= 0)
				regc(*regparse++);
			}
			אחרת
			{
			    startc = *regparse++;
			    regc(startc);
			}
		    }
		}
		regc(NUL);
		prevchr_len = 1;	// last char was the ']'
		אם (*regparse != ']')
		    EMSG_RET_NULL(_(e_too_many_brackets));  // Cannot happen?
		skipchr();	    // let's be friends with the lexer again
		*flagp |= HASWIDTH | SIMPLE;
		اكسر;
	    }
	    אחרת אם (reg_strict)
		EMSG2_RET_NULL(_(e_missing_rsb_after_str_lsb),
							reg_magic > MAGIC_OFF);
	}
	// FALLTHROUGH

      افتراضي:
	{
	    صحيح		len;

	    // A multi-byte character is handled as a separate atom if it's
	    // before a multi and when it's a composing char.
	    אם (use_multibytecode(c))
	    {
do_multibyte:
		ret = regnode(MULTIBYTECODE);
		regmbc(c);
		*flagp |= HASWIDTH | SIMPLE;
		اكسر;
	    }

	    ret = regnode(EXACTLY);

	    // Append characters as long as:
	    // - there is no following multi, we then need the character in
	    //   front of it as a single character operand
	    // - not running into a Magic character
	    // - "one_exactly" is not set
	    // But always emit at least one character.  Might be a Multi,
	    // e.g., a "[" without matching "]".
	    עבור (len = 0; c != NUL && (len == 0
			|| (re_multi_type(peekchr()) == NOT_MULTI
			    && !one_exactly
			    && !is_Magic(c))); ++len)
	    {
		c = no_Magic(c);
		אם (has_mbyte)
		{
		    regmbc(c);
		    אם (enc_utf8)
		    {
			صحيح	l;

			// Need to get composing character too.
			עבור (;;)
			{
			    l = utf_ptr2len(regparse);
			    אם (!UTF_COMPOSINGLIKE(regparse, regparse + l))
				اكسر;
			    regmbc(utf_ptr2char(regparse));
			    skipchr();
			}
		    }
		}
		אחרת
		    regc(c);
		c = getchr();
	    }
	    ungetchr();

	    regc(NUL);
	    *flagp |= HASWIDTH;
	    אם (len == 1)
		*flagp |= SIMPLE;
	}
	اكسر;
    }

    החזר ret;
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
    ساكن char_u *
regpiece(صحيح *flagp)
{
    char_u	    *ret;
    صحيح		    op;
    char_u	    *next;
    صحيح		    flags;
    long	    minval;
    long	    maxval;

    ret = regatom(&flags);
    אם (ret == عدم)
	החזר عدم;

    op = peekchr();
    אם (re_multi_type(op) == NOT_MULTI)
    {
	*flagp = flags;
	החזר ret;
    }
    // default flags
    *flagp = (WORST | SPSTART | (flags & (HASNL | HASLOOKBH)));

    skipchr();
    بدل (op)
    {
	حالة Magic('*'):
	    אם (flags & SIMPLE)
		reginsert(STAR, ret);
	    אחרת
	    {
		// Emit x* as (x&|), where & means "self".
		reginsert(BRANCH, ret); // Either x
		regoptail(ret, regnode(BACK));	// and loop
		regoptail(ret, ret);	// back
		regtail(ret, regnode(BRANCH));	// or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    اكسر;

	حالة Magic('+'):
	    אם (flags & SIMPLE)
		reginsert(PLUS, ret);
	    אחרת
	    {
		// Emit x+ as x(&|), where & means "self".
		next = regnode(BRANCH); // Either
		regtail(ret, next);
		regtail(regnode(BACK), ret);	// loop back
		regtail(next, regnode(BRANCH)); // or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    *flagp = (WORST | HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    اكسر;

	حالة Magic('@'):
	    {
		صحيح	lop = END;
		long	nr;

		nr = (long)getdecchrs();
		بدل (no_Magic(getchr()))
		{
		    حالة '=': lop = MATCH; اكسر;		  // \@=
		    حالة '!': lop = NOMATCH; اكسر;		  // \@!
		    حالة '>': lop = SUBPAT; اكسر;		  // \@>
		    حالة '<': بدل (no_Magic(getchr()))
			      {
				  حالة '=': lop = BEHIND; اكسر;   // \@<=
				  حالة '!': lop = NOBEHIND; اكسر; // \@<!
			      }
		}
		אם (lop == END)
		    EMSG2_RET_NULL(_(e_invalid_character_after_str_at),
						      reg_magic == MAGIC_ALL);
		// Look behind must match with behind_pos.
		אם (lop == BEHIND || lop == NOBEHIND)
		{
		    regtail(ret, regnode(BHPOS));
		    *flagp |= HASLOOKBH;
		}
		regtail(ret, regnode(END)); // operand ends
		אם (lop == BEHIND || lop == NOBEHIND)
		{
		    אם (nr < 0)
			nr = 0; // no limit is same as zero limit
		    reginsert_nr(lop, nr, ret);
		}
		אחרת
		    reginsert(lop, ret);
		اكسر;
	    }

	حالة Magic('?'):
	حالة Magic('='):
	    // Emit x= as (x|)
	    reginsert(BRANCH, ret);		// Either x
	    regtail(ret, regnode(BRANCH));	// or
	    next = regnode(NOTHING);		// null.
	    regtail(ret, next);
	    regoptail(ret, next);
	    اكسر;

	حالة Magic('{'):
	    אם (!read_limits(&minval, &maxval))
		החזר عدم;
	    אם (flags & SIMPLE)
	    {
		reginsert(BRACE_SIMPLE, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
	    }
	    אחרת
	    {
		אם (num_complex_braces >= 10)
		    EMSG2_RET_NULL(_(e_too_many_complex_str_curly),
						      reg_magic == MAGIC_ALL);
		reginsert(BRACE_COMPLEX + num_complex_braces, ret);
		regoptail(ret, regnode(BACK));
		regoptail(ret, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
		++num_complex_braces;
	    }
	    אם (minval > 0 && maxval > 0)
		*flagp = (HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    اكسر;
    }
    אם (re_multi_type(peekchr()) != NOT_MULTI)
    {
	// Can't have a multi follow a multi.
	אם (peekchr() == Magic('*'))
	    EMSG2_RET_NULL(_(e_nested_str), reg_magic >= MAGIC_ON);
	EMSG3_RET_NULL(_(e_nested_str_chr), reg_magic == MAGIC_ALL,
							  no_Magic(peekchr()));
    }

    החזר ret;
}

/*
 * Parse one alternative of an | or & operator.
 * Implements the concatenation operator.
 */
    ساكن char_u *
regconcat(صحيح *flagp)
{
    char_u	*first = عدم;
    char_u	*chain = عدم;
    char_u	*latest;
    صحيح		flags;
    صحيح		cont = TRUE;

    *flagp = WORST;		// Tentatively.

    כלעוד (cont)
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
			    latest = regpiece(&flags);
			    אם (latest == عدم || reg_toolong)
				החזר عدم;
			    *flagp |= flags & (HASWIDTH | HASNL | HASLOOKBH);
			    אם (chain == عدم)	// First piece.
				*flagp |= flags & SPSTART;
			    אחרת
				regtail(chain, latest);
			    chain = latest;
			    אם (first == عدم)
				first = latest;
			    اكسر;
	}
    }
    אם (first == عدم)		// Loop ran zero times.
	first = regnode(NOTHING);
    החזר first;
}

/*
 * Parse one alternative of an | operator.
 * Implements the & operator.
 */
    ساكن char_u *
regbranch(صحيح *flagp)
{
    char_u	*ret;
    char_u	*chain = عدم;
    char_u	*latest;
    صحيح		flags;

    *flagp = WORST | HASNL;		// Tentatively.

    ret = regnode(BRANCH);
    עבור (;;)
    {
	latest = regconcat(&flags);
	אם (latest == عدم)
	    החזר عدم;
	// If one of the branches has width, the whole thing has.  If one of
	// the branches anchors at start-of-line, the whole thing does.
	// If one of the branches uses look-behind, the whole thing does.
	*flagp |= flags & (HASWIDTH | SPSTART | HASLOOKBH);
	// If one of the branches doesn't match a line-break, the whole thing
	// doesn't.
	*flagp &= ~HASNL | (flags & HASNL);
	אם (chain != عدم)
	    regtail(chain, latest);
	אם (peekchr() != Magic('&'))
	    اكسر;
	skipchr();
	regtail(latest, regnode(END)); // operand ends
	אם (reg_toolong)
	    اكسر;
	reginsert(MATCH, latest);
	chain = latest;
    }

    החזר ret;
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
    ساكن char_u *
reg(
    صحيح		paren,	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
    صحيح		*flagp)
{
    char_u	*ret;
    char_u	*br;
    char_u	*ender;
    صحيح		parno = 0;
    صحيح		flags;

    *flagp = HASWIDTH;		// Tentatively.

#ifdef FEAT_SYN_HL
    אם (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	אם (regnzpar >= NSUBEXP)
	    EMSG_RET_NULL(_(e_too_many_z));
	parno = regnzpar;
	regnzpar++;
	ret = regnode(ZOPEN + parno);
    }
    אחרת
#endif
	אם (paren == REG_PAREN)
    {
	// Make a MOPEN node.
	אם (regnpar >= NSUBEXP)
	    EMSG2_RET_NULL(_(e_too_many_str_open), reg_magic == MAGIC_ALL);
	parno = regnpar;
	++regnpar;
	ret = regnode(MOPEN + parno);
    }
    אחרת אם (paren == REG_NPAREN)
    {
	// Make a NOPEN node.
	ret = regnode(NOPEN);
    }
    אחרת
	ret = عدم;

    אם (bt_reg_parse_depth >= REG_MAX_PAREN_DEPTH)
	EMSG_RET_NULL(_(e_command_too_complex));
    ++bt_reg_parse_depth;

    // Pick up the branches, linking them together.
    br = regbranch(&flags);
    אם (br == عدم)
    {
	ret = عدم;
	اذهب theend;
    }
    אם (ret != عدم)
	regtail(ret, br);	// [MZ]OPEN -> first.
    אחרת
	ret = br;
    // If one of the branches can be zero-width, the whole thing can.
    // If one of the branches has * at start or matches a line-break, the
    // whole thing can.
    אם (!(flags & HASWIDTH))
	*flagp &= ~HASWIDTH;
    *flagp |= flags & (SPSTART | HASNL | HASLOOKBH);
    כלעוד (peekchr() == Magic('|'))
    {
	skipchr();
	br = regbranch(&flags);
	אם (br == عدم || reg_toolong)
	{
	    ret = عدم;
	    اذهب theend;
	}
	regtail(ret, br);	// BRANCH -> BRANCH.
	אם (!(flags & HASWIDTH))
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
    עבור (br = ret; br != عدم; br = regnext(br))
	regoptail(br, ender);

    // Check for proper termination.
    אם (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
#ifdef FEAT_SYN_HL
	אם (paren == REG_ZPAREN)
	{
	    emsg(_(e_unmatched_z));
	    rc_did_emsg = TRUE;
	    ret = عدم;
	    اذهب theend;
	}
	אחרת
#endif
	אם (paren == REG_NPAREN)
	{
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = عدم;
	    اذهب theend;
	}
	אחרת
	{
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = عدم;
	    اذهب theend;
	}
    }
    אחרת אם (paren == REG_NOPAREN && peekchr() != NUL)
    {
	אם (curchr == Magic(')'))
	{
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = عدم;
	    اذهب theend;
	}
	אחרת
	{
	    emsg(_(e_trailing_characters));	// "Can't happen".
	    rc_did_emsg = TRUE;
	    ret = عدم;
	    اذهب theend;
	}
    }
    // Here we set the flag allowing back references to this set of
    // parentheses.
    אם (paren == REG_PAREN)
	had_endbrace[parno] = TRUE;	// have seen the close paren

theend:
    --bt_reg_parse_depth;
    החזר ret;
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
    ساكن regprog_T *
bt_regcomp(char_u *expr, صحيح re_flags)
{
    bt_regprog_T    *r;
    char_u	*scan;
    char_u	*longest;
    صحيح		len;
    صحيح		flags;

    אם (expr == عدم)
	IEMSG_RET_NULL(e_null_argument);

    init_class_tab();

    // First pass: determine size, legality.
    regcomp_start(expr, re_flags);
    regcode = JUST_CALC_SIZE;
    regc(REGMAGIC);
    אם (reg(REG_NOPAREN, &flags) == عدم)
	החזר عدم;

    // Allocate space.
    r = alloc(offsetof(bt_regprog_T, program) + regsize);
    אם (r == عدم)
	החזר عدم;
    r->re_in_use = FALSE;
#ifdef DEBUG
    r->regsz = regsize;
#endif

    // Second pass: emit code.
    regcomp_start(expr, re_flags);
    regcode = r->program;
    regc(REGMAGIC);
    אם (reg(REG_NOPAREN, &flags) == عدم || reg_toolong)
    {
	vim_free(r);
	אם (reg_toolong)
	    EMSG_RET_NULL(_(e_pattern_too_long));
	החזר عدم;
    }

    // Dig out information for optimizations.
    r->regstart = NUL;		// Worst-case defaults.
    r->reganch = 0;
    r->regmust = عدم;
    r->regmlen = 0;
    r->regflags = regflags;
    אם (flags & HASNL)
	r->regflags |= RF_HASNL;
    אם (flags & HASLOOKBH)
	r->regflags |= RF_LOOKBH;
#ifdef FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    r->reghasz = re_has_z;
#endif
    scan = r->program + 1;	// First BRANCH.
    אם (OP(regnext(scan)) == END)   // Only one top-level choice.
    {
	scan = OPERAND(scan);

	// Starting-point info.
	אם (OP(scan) == BOL || OP(scan) == RE_BOF)
	{
	    r->reganch++;
	    scan = regnext(scan);
	}

	אם (OP(scan) == EXACTLY)
	{
	    אם (has_mbyte)
		r->regstart = (*mb_ptr2char)(OPERAND(scan));
	    אחרת
		r->regstart = *OPERAND(scan);
	}
	אחרת אם ((OP(scan) == BOW
		    || OP(scan) == EOW
		    || OP(scan) == NOTHING
		    || OP(scan) == MOPEN + 0 || OP(scan) == NOPEN
		    || OP(scan) == MCLOSE + 0 || OP(scan) == NCLOSE)
		 && OP(regnext(scan)) == EXACTLY)
	{
	    אם (has_mbyte)
		r->regstart = (*mb_ptr2char)(OPERAND(regnext(scan)));
	    אחרת
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
	אם ((flags & SPSTART || OP(scan) == BOW || OP(scan) == EOW)
							  && !(flags & HASNL))
	{
	    size_t  scanlen;

	    longest = عدم;
	    len = 0;
	    עבור (; scan != عدم; scan = regnext(scan))
	    {
		אם (OP(scan) == EXACTLY)
		{
		    scanlen = STRLEN(OPERAND(scan));
		    אם (scanlen >= (size_t)len)
		    {
			longest = OPERAND(scan);
			len = (صحيح)scanlen;
		    }
		}
	    }
	    r->regmust = longest;
	    r->regmlen = len;
	}
    }
#ifdef BT_REGEXP_DUMP
    regdump(expr, r);
#endif
    r->engine = &bt_regengine;
    החזר (regprog_T *)r;
}

#אם defined(FEAT_SYN_HL)
/*
 * Check if during the previous call to vim_regcomp the EOL item "$" has been
 * found.  This is messy, but it works fine.
 */
    صحيح
vim_regcomp_had_eol(فراغ)
{
    החזר had_eol;
}
#endif

/*
 * Get a number after a backslash that is inside [].
 * When nothing is recognized return a backslash.
 */
    ساكن صحيح
coll_get_char(فراغ)
{
    vimlong_T	nr = -1;

    بدل (*regparse++)
    {
	حالة 'd': nr = getdecchrs(); اكسر;
	حالة 'o': nr = getoctchrs(); اكسر;
	حالة 'x': nr = gethexchrs(2); اكسر;
	حالة 'u': nr = gethexchrs(4); اكسر;
	حالة 'U': nr = gethexchrs(8); اكسر;
    }
    אם (nr < 0)
    {
	// If getting the number fails be backwards compatible: the character
	// is a backslash.
	--regparse;
	nr = '\\';
    }
    אם (nr > INT_MAX)
	nr = INT_MAX;
    החזר nr;
}

/*
 * Free a compiled regexp program, returned by bt_regcomp().
 */
    ساكن فراغ
bt_regfree(regprog_T *prog)
{
    vim_free(prog);
}

#define ADVANCE_REGINPUT() MB_PTR_ADV(rex.input)

/*
 * The arguments from BRACE_LIMITS are stored here.  They are actually local
 * to regmatch(), but they are here to reduce the amount of stack space used
 * (it can be called recursively many times).
 */
ساكن long	bl_minval;
ساكن long	bl_maxval;

/*
 * Save the input line and position in a regsave_T.
 */
    ساكن فراغ
reg_save(regsave_T *save, garray_T *gap)
{
    אם (REG_MULTI)
    {
	save->rs_u.pos.col = (colnr_T)(rex.input - rex.line);
	save->rs_u.pos.lnum = rex.lnum;
    }
    אחרת
	save->rs_u.ptr = rex.input;
    save->rs_len = gap->ga_len;
}

/*
 * Restore the input line and position from a regsave_T.
 */
    ساكن فراغ
reg_restore(regsave_T *save, garray_T *gap)
{
    אם (REG_MULTI)
    {
	אם (rex.lnum != save->rs_u.pos.lnum)
	{
	    // only call reg_getline() when the line number changed to save
	    // a bit of time
	    rex.lnum = save->rs_u.pos.lnum;
	    rex.line = reg_getline(rex.lnum);
	}
	rex.input = rex.line + save->rs_u.pos.col;
    }
    אחרת
	rex.input = save->rs_u.ptr;
    gap->ga_len = save->rs_len;
}

/*
 * Return TRUE if current position is equal to saved position.
 */
    ساكن صحيح
reg_save_equal(regsave_T *save)
{
    אם (REG_MULTI)
	החזר rex.lnum == save->rs_u.pos.lnum
				  && rex.input == rex.line + save->rs_u.pos.col;
    החזר rex.input == save->rs_u.ptr;
}

// Save the sub-expressions before attempting a match.
#define save_se(savep, posp, pp) \
    REG_MULTI ? save_se_multi((savep), (posp)) : save_se_one((savep), (pp))

// After a failed match restore the sub-expressions.
#define restore_se(savep, posp, pp) \
{ \
    אם (REG_MULTI) \
	*(posp) = (savep)->se_u.pos; \
    אחרת \
	*(pp) = (savep)->se_u.ptr; \
}

/*
 * Tentatively set the sub-expression start to the current position (after
 * calling regmatch() they will have changed).  Need to save the existing
 * values for when there is no match.
 * Use se_save() to use pointer (save_se_multi()) or position (save_se_one()),
 * depending on REG_MULTI.
 */
    ساكن فراغ
save_se_multi(save_se_T *savep, lpos_T *posp)
{
    savep->se_u.pos = *posp;
    posp->lnum = rex.lnum;
    posp->col = (colnr_T)(rex.input - rex.line);
}

    ساكن فراغ
save_se_one(save_se_T *savep, char_u **pp)
{
    savep->se_u.ptr = *pp;
    *pp = rex.input;
}

/*
 * regrepeat - repeatedly match something simple, return how many.
 * Advances rex.input (and rex.lnum) to just after the matched chars.
 */
    ساكن صحيح
regrepeat(
    char_u	*p,
    long	maxcount)   // maximum number of matches allowed
{
    long	count = 0;
    char_u	*scan;
    char_u	*opnd;
    صحيح		mask;
    صحيح		testval = 0;

    scan = rex.input;	    // Make local copy of rex.input for speed.
    opnd = OPERAND(p);
    بدل (OP(p))
    {
      حالة ANY:
      حالة ANY + ADD_NL:
	כלעוד (count < maxcount)
	{
	    // Matching anything means we continue until end-of-line (or
	    // end-of-file for ANY + ADD_NL), only limited by maxcount.
	    כלעוד (*scan != NUL && count < maxcount)
	    {
		++count;
		MB_PTR_ADV(scan);
	    }
	    אם (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
				      || rex.reg_line_lbr || count == maxcount)
		اكسر;
	    ++count;		// count the line-break
	    reg_nextline();
	    scan = rex.input;
	    אם (got_int)
		اكسر;
	}
	اكسر;

      حالة IDENT:
      حالة IDENT + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      حالة SIDENT:
      حالة SIDENT + ADD_NL:
	כלעוד (count < maxcount)
	{
	    אם (vim_isIDc(PTR2CHAR(scan)) && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    אחרת אם (*scan == NUL)
	    {
		אם (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_line_lbr)
		    اكسر;
		reg_nextline();
		scan = rex.input;
		אם (got_int)
		    اكسر;
	    }
	    אחרת אם (rex.reg_line_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    אחרת
		اكسر;
	    ++count;
	}
	اكسر;

      حالة KWORD:
      حالة KWORD + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      حالة SKWORD:
      حالة SKWORD + ADD_NL:
	כלעוד (count < maxcount)
	{
	    אם (vim_iswordp_buf(scan, rex.reg_buf)
					  && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    אחרת אם (*scan == NUL)
	    {
		אם (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_line_lbr)
		    اكسر;
		reg_nextline();
		scan = rex.input;
		אם (got_int)
		    اكسر;
	    }
	    אחרת אם (rex.reg_line_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    אחרת
		اكسر;
	    ++count;
	}
	اكسر;

      حالة FNAME:
      حالة FNAME + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      حالة SFNAME:
      حالة SFNAME + ADD_NL:
	כלעוד (count < maxcount)
	{
	    אם (vim_isfilec(PTR2CHAR(scan)) && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    אחרת אם (*scan == NUL)
	    {
		אם (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_line_lbr)
		    اكسر;
		reg_nextline();
		scan = rex.input;
		אם (got_int)
		    اكسر;
	    }
	    אחרת אם (rex.reg_line_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    אחרת
		اكسر;
	    ++count;
	}
	اكسر;

      حالة PRINT:
      حالة PRINT + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      حالة SPRINT:
      حالة SPRINT + ADD_NL:
	כלעוד (count < maxcount)
	{
	    אם (*scan == NUL)
	    {
		אם (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_line_lbr)
		    اكسر;
		reg_nextline();
		scan = rex.input;
		אם (got_int)
		    اكسر;
	    }
	    אחרת אם (vim_isprintc(PTR2CHAR(scan)) == 1
					  && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    אחרת אם (rex.reg_line_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    אחרת
		اكسر;
	    ++count;
	}
	اكسر;

      حالة WHITE:
      حالة WHITE + ADD_NL:
	testval = mask = RI_WHITE;
do_class:
	כלעוד (count < maxcount)
	{
	    صحيح		l;

	    אם (*scan == NUL)
	    {
		אם (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_line_lbr)
		    اكسر;
		reg_nextline();
		scan = rex.input;
		אם (got_int)
		    اكسر;
	    }
	    אחרת אם (has_mbyte && (l = (*mb_ptr2len)(scan)) > 1)
	    {
		אם (testval != 0)
		    اكسر;
		scan += l;
	    }
	    אחרת אם ((class_tab[*scan] & mask) == testval)
		++scan;
	    אחרת אם (rex.reg_line_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    אחרת
		اكسر;
	    ++count;
	}
	اكسر;

      حالة NWHITE:
      حالة NWHITE + ADD_NL:
	mask = RI_WHITE;
	اذهب do_class;
      حالة DIGIT:
      حالة DIGIT + ADD_NL:
	testval = mask = RI_DIGIT;
	اذهب do_class;
      حالة NDIGIT:
      حالة NDIGIT + ADD_NL:
	mask = RI_DIGIT;
	اذهب do_class;
      حالة HEX:
      حالة HEX + ADD_NL:
	testval = mask = RI_HEX;
	اذهب do_class;
      حالة NHEX:
      حالة NHEX + ADD_NL:
	mask = RI_HEX;
	اذهب do_class;
      حالة OCTAL:
      حالة OCTAL + ADD_NL:
	testval = mask = RI_OCTAL;
	اذهب do_class;
      حالة NOCTAL:
      حالة NOCTAL + ADD_NL:
	mask = RI_OCTAL;
	اذهب do_class;
      حالة WORD:
      حالة WORD + ADD_NL:
	testval = mask = RI_WORD;
	اذهب do_class;
      حالة NWORD:
      حالة NWORD + ADD_NL:
	mask = RI_WORD;
	اذهب do_class;
      حالة HEAD:
      حالة HEAD + ADD_NL:
	testval = mask = RI_HEAD;
	اذهب do_class;
      حالة NHEAD:
      حالة NHEAD + ADD_NL:
	mask = RI_HEAD;
	اذهب do_class;
      حالة ALPHA:
      حالة ALPHA + ADD_NL:
	testval = mask = RI_ALPHA;
	اذهب do_class;
      حالة NALPHA:
      حالة NALPHA + ADD_NL:
	mask = RI_ALPHA;
	اذهب do_class;
      حالة LOWER:
      حالة LOWER + ADD_NL:
	testval = mask = RI_LOWER;
	اذهب do_class;
      حالة NLOWER:
      حالة NLOWER + ADD_NL:
	mask = RI_LOWER;
	اذهب do_class;
      حالة UPPER:
      حالة UPPER + ADD_NL:
	testval = mask = RI_UPPER;
	اذهب do_class;
      حالة NUPPER:
      حالة NUPPER + ADD_NL:
	mask = RI_UPPER;
	اذهب do_class;

      حالة EXACTLY:
	{
	    صحيح	    cu, cl;

	    // This doesn't do a multi-byte character, because a MULTIBYTECODE
	    // would have been used for it.  It does handle single-byte
	    // characters, such as latin1.
	    אם (rex.reg_ic)
	    {
		cu = MB_TOUPPER(*opnd);
		cl = MB_TOLOWER(*opnd);
		כלעוד (count < maxcount && (*scan == cu || *scan == cl))
		{
		    count++;
		    scan++;
		}
	    }
	    אחרת
	    {
		cu = *opnd;
		כלעוד (count < maxcount && *scan == cu)
		{
		    count++;
		    scan++;
		}
	    }
	    اكسر;
	}

      حالة MULTIBYTECODE:
	{
	    صحيح		i, len, cf = 0;

	    // Safety check (just in case 'encoding' was changed since
	    // compiling the program).
	    אם ((len = (*mb_ptr2len)(opnd)) > 1)
	    {
		אם (rex.reg_ic && enc_utf8)
		    cf = utf_fold(utf_ptr2char(opnd));
		כלעוד (count < maxcount && (*mb_ptr2len)(scan) >= len)
		{
		    עבור (i = 0; i < len; ++i)
			אם (opnd[i] != scan[i])
			    اكسر;
		    אם (i < len && (!rex.reg_ic || !enc_utf8
					|| utf_fold(utf_ptr2char(scan)) != cf))
			اكسر;
		    scan += len;
		    ++count;
		}
	    }
	}
	اكسر;

      حالة ANYOF:
      حالة ANYOF + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH

      حالة ANYBUT:
      حالة ANYBUT + ADD_NL:
	כלעוד (count < maxcount)
	{
	    صحيح len;

	    אם (*scan == NUL)
	    {
		אם (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_line_lbr)
		    اكسر;
		reg_nextline();
		scan = rex.input;
		אם (got_int)
		    اكسر;
	    }
	    אחרת אם (rex.reg_line_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    אחרת אם (has_mbyte && (len = (*mb_ptr2len)(scan)) > 1)
	    {
		אם ((cstrchr(opnd, (*mb_ptr2char)(scan)) == عدم) == testval)
		    اكسر;
		scan += len;
	    }
	    אחרת
	    {
		אם ((cstrchr(opnd, *scan) == عدم) == testval)
		    اكسر;
		++scan;
	    }
	    ++count;
	}
	اكسر;

      حالة NEWL:
	כלעוד (count < maxcount
		&& ((*scan == NUL && rex.lnum <= rex.reg_maxline
				       && !rex.reg_line_lbr && REG_MULTI)
		    || (*scan == '\n' && rex.reg_line_lbr)))
	{
	    count++;
	    אם (rex.reg_line_lbr)
		ADVANCE_REGINPUT();
	    אחרת
		reg_nextline();
	    scan = rex.input;
	    אם (got_int)
		اكسر;
	}
	اكسر;

      افتراضي:			// Oh dear.  Called inappropriately.
	iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	printf("Called regrepeat with op code %d\n", OP(p));
#endif
	اكسر;
    }

    rex.input = scan;

    החזר (صحيح)count;
}

/*
 * Push an item onto the regstack.
 * Returns pointer to new item.  Returns NULL when out of memory.
 */
    ساكن regitem_T *
regstack_push(regstate_T state, char_u *scan)
{
    regitem_T	*rp;

    אם ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
    {
	emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
	החזר عدم;
    }
    אם (GA_GROW_FAILS(&regstack, (صحيح)حجم(regitem_T)))
	החזר عدم;

    rp = (regitem_T *)((حرف *)regstack.ga_data + regstack.ga_len);
    rp->rs_state = state;
    rp->rs_scan = scan;

    regstack.ga_len += حجم(regitem_T);
    החזר rp;
}

/*
 * Pop an item from the regstack.
 */
    ساكن فراغ
regstack_pop(char_u **scan)
{
    regitem_T	*rp;

    rp = (regitem_T *)((حرف *)regstack.ga_data + regstack.ga_len) - 1;
    *scan = rp->rs_scan;

    regstack.ga_len -= حجم(regitem_T);
}

#ifdef FEAT_RELTIME
/*
 * Check if the timer expired, return TRUE if so.
 */
    ساكن صحيح
bt_did_time_out(صحيح *timed_out)
{
    אם (*timeout_flag)
    {
	אם (timed_out != عدم)
	{
# ifdef FEAT_EVAL
	    אם (!*timed_out)
		ch_log(عدم, "BT regexp timed out");
# endif
	    *timed_out = TRUE;
	}
	החזר TRUE;
    }
    החזר FALSE;
}
#endif

/*
 * Save the current subexpr to "bp", so that they can be restored
 * later by restore_subexpr().
 */
    ساكن فراغ
save_subexpr(regbehind_T *bp)
{
    صحيح i;

    // When "rex.need_clear_subexpr" is set we don't need to save the values,
    // only remember that this flag needs to be set again when restoring.
    bp->save_need_clear_subexpr = rex.need_clear_subexpr;
    אם (rex.need_clear_subexpr)
	החזר;

    עבור (i = 0; i < NSUBEXP; ++i)
    {
	אם (REG_MULTI)
	{
	    bp->save_start[i].se_u.pos = rex.reg_startpos[i];
	    bp->save_end[i].se_u.pos = rex.reg_endpos[i];
	}
	אחרת
	{
	    bp->save_start[i].se_u.ptr = rex.reg_startp[i];
	    bp->save_end[i].se_u.ptr = rex.reg_endp[i];
	}
    }
}

/*
 * Restore the subexpr from "bp".
 */
    ساكن فراغ
restore_subexpr(regbehind_T *bp)
{
    صحيح i;

    // Only need to restore saved values when they are not to be cleared.
    rex.need_clear_subexpr = bp->save_need_clear_subexpr;
    אם (rex.need_clear_subexpr)
	החזר;

    עבור (i = 0; i < NSUBEXP; ++i)
    {
	אם (REG_MULTI)
	{
	    rex.reg_startpos[i] = bp->save_start[i].se_u.pos;
	    rex.reg_endpos[i] = bp->save_end[i].se_u.pos;
	}
	אחרת
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
    ساكن صحيح
regmatch(
    char_u	*scan,		    // Current node.
    صحيح		*timed_out UNUSED)  // flag set on timeout or NULL
{
  char_u	*next;		// Next node.
  صحيح		op;
  صحيح		c;
  regitem_T	*rp;
  صحيح		no;
  صحيح		status;		// one of the RA_ values:

  // Make "regstack" and "backpos" empty.  They are allocated and freed in
  // bt_regexec_both() to reduce malloc()/free() calls.
  regstack.ga_len = 0;
  backpos.ga_len = 0;

  // Repeat until "regstack" is empty.
  עבור (;;)
  {
    // Some patterns may take a long time to match, e.g., "\([a-z]\+\)\+Q".
    // Allow interrupting them with CTRL-C.
    fast_breakcheck();

#ifdef DEBUG
    אם (scan != عدم && regnarrate)
    {
	mch_errmsg((حرف *)regprop(scan));
	mch_errmsg("(\n");
    }
#endif

    // Repeat for items that can be matched sequentially, without using the
    // regstack.
    עבור (;;)
    {
	אם (got_int || scan == عدم)
	{
	    status = RA_FAIL;
	    اكسر;
	}
#ifdef FEAT_RELTIME
	אם (bt_did_time_out(timed_out))
	{
	    status = RA_FAIL;
	    اكسر;
	}
#endif
	status = RA_CONT;

#ifdef DEBUG
	אם (regnarrate)
	{
	    mch_errmsg((حرف *)regprop(scan));
	    mch_errmsg("...\n");
# ifdef FEAT_SYN_HL
	    אם (re_extmatch_in != عدم)
	    {
		صحيح i;

		mch_errmsg(_("External submatches:\n"));
		עבור (i = 0; i < NSUBEXP; i++)
		{
		    mch_errmsg("    \"");
		    אם (re_extmatch_in->matches[i] != عدم)
			mch_errmsg((حرف *)re_extmatch_in->matches[i]);
		    mch_errmsg("\"\n");
		}
	    }
# endif
	}
#endif
	next = regnext(scan);

	op = OP(scan);
	// Check for character class with NL added.
	אם (!rex.reg_line_lbr && WITH_NL(op) && REG_MULTI
			     && *rex.input == NUL && rex.lnum <= rex.reg_maxline)
	{
	    reg_nextline();
	}
	אחרת אם (rex.reg_line_lbr && WITH_NL(op) && *rex.input == '\n')
	{
	    ADVANCE_REGINPUT();
	}
	אחרת
	{
	  אם (WITH_NL(op))
	      op -= ADD_NL;
	  אם (has_mbyte)
	      c = (*mb_ptr2char)(rex.input);
	  אחרת
	      c = *rex.input;
	  بدل (op)
	  {
	  حالة BOL:
	    אם (rex.input != rex.line)
		status = RA_NOMATCH;
	    اكسر;

	  حالة EOL:
	    אם (c != NUL)
		status = RA_NOMATCH;
	    اكسر;

	  حالة RE_BOF:
	    // We're not at the beginning of the file when below the first
	    // line where we started, not at the start of the line or we
	    // didn't start at the first line of the buffer.
	    אם (rex.lnum != 0 || rex.input != rex.line
				       || (REG_MULTI && rex.reg_firstlnum > 1))
		status = RA_NOMATCH;
	    اكسر;

	  حالة RE_EOF:
	    אם (rex.lnum != rex.reg_maxline || c != NUL)
		status = RA_NOMATCH;
	    اكسر;

	  حالة CURSOR:
	    // Check if the buffer is in a window and compare the
	    // rex.reg_win->w_cursor position to the match position.
	    אם (rex.reg_win == عدم
		    || (rex.lnum + rex.reg_firstlnum
						 != rex.reg_win->w_cursor.lnum)
		    || ((colnr_T)(rex.input - rex.line)
						 != rex.reg_win->w_cursor.col))
		status = RA_NOMATCH;
	    اكسر;

	  حالة RE_MARK:
	    // Compare the mark position to the match position.
	    {
		صحيح	mark = OPERAND(scan)[0];
		صحيح	cmp = OPERAND(scan)[1];
		pos_T	*pos;
		size_t	col = REG_MULTI ? rex.input - rex.line : 0;

		pos = getmark_buf(rex.reg_buf, mark, FALSE);

		// Line may have been freed, get it again.
		אם (REG_MULTI)
		{
		    rex.line = reg_getline(rex.lnum);
		    rex.input = rex.line + col;
		}

		אם (pos == عدم		     // mark doesn't exist
			|| pos->lnum <= 0)   // mark isn't set in reg_buf
		{
		    status = RA_NOMATCH;
		}
		אחרת
		{
		    colnr_T pos_col = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->col == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->col;

		    אם ((pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_col == (colnr_T)(rex.input - rex.line)
				    ? (cmp == '<' || cmp == '>')
				    : (pos_col < (colnr_T)(rex.input - rex.line)
					? cmp != '>'
					: cmp != '<'))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? cmp != '>'
				    : cmp != '<')))
		    status = RA_NOMATCH;
		}
	    }
	    اكسر;

	  حالة RE_VISUAL:
	    אם (!reg_match_visual())
		status = RA_NOMATCH;
	    اكسر;

	  حالة RE_LNUM:
	    אם (!REG_MULTI || !re_num_cmp((long_u)(rex.lnum + rex.reg_firstlnum),
									scan))
		status = RA_NOMATCH;
	    اكسر;

	  حالة RE_COL:
	    אם (!re_num_cmp((long_u)(rex.input - rex.line) + 1, scan))
		status = RA_NOMATCH;
	    اكسر;

	  حالة RE_VCOL:
	    {
		win_T	    *wp = rex.reg_win == عدم ? curwin : rex.reg_win;
		linenr_T    lnum = REG_MULTI ? rex.reg_firstlnum + rex.lnum : 1;
		long_u	    vcol;

		אם (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_buffer->b_ml.ml_line_count))
		    lnum = 1;
		vcol = (long_u)win_linetabsize(wp, lnum, rex.line,
					      (colnr_T)(rex.input - rex.line));
		אם (!re_num_cmp(vcol + 1, scan))
		    status = RA_NOMATCH;
	    }
	    اكسر;

	  حالة BOW:	// \<word; rex.input points to w
	    אם (c == NUL)	// Can't match at end of line
		status = RA_NOMATCH;
	    אחרת אם (has_mbyte)
	    {
		صحيح this_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.input, rex.reg_buf);
		אם (this_class <= 1)
		    status = RA_NOMATCH;  // not on a word at all
		אחרת אם (reg_prev_class() == this_class)
		    status = RA_NOMATCH;  // previous char is in same word
	    }
	    אחרת
	    {
		אם (!vim_iswordc_buf(c, rex.reg_buf) || (rex.input > rex.line
				&& vim_iswordc_buf(rex.input[-1], rex.reg_buf)))
		    status = RA_NOMATCH;
	    }
	    اكسر;

	  حالة EOW:	// word\>; rex.input points after d
	    אם (rex.input == rex.line)    // Can't match at start of line
		status = RA_NOMATCH;
	    אחרת אם (has_mbyte)
	    {
		صحيح this_class, prev_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.input, rex.reg_buf);
		prev_class = reg_prev_class();
		אם (this_class == prev_class
			|| prev_class == 0 || prev_class == 1)
		    status = RA_NOMATCH;
	    }
	    אחרת
	    {
		אם (!vim_iswordc_buf(rex.input[-1], rex.reg_buf)
			|| (rex.input[0] != NUL
					   && vim_iswordc_buf(c, rex.reg_buf)))
		    status = RA_NOMATCH;
	    }
	    اكسر; // Matched with EOW

	  حالة ANY:
	    // ANY does not match new lines.
	    אם (c == NUL)
		status = RA_NOMATCH;
	    אחרת
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة IDENT:
	    אם (!vim_isIDc(c))
		status = RA_NOMATCH;
	    אחרת
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة SIDENT:
	    אם (VIM_ISDIGIT(*rex.input) || !vim_isIDc(c))
		status = RA_NOMATCH;
	    אחרת
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة KWORD:
	    אם (!vim_iswordp_buf(rex.input, rex.reg_buf))
		status = RA_NOMATCH;
	    אחרת
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة SKWORD:
	    אם (VIM_ISDIGIT(*rex.input)
				    || !vim_iswordp_buf(rex.input, rex.reg_buf))
		status = RA_NOMATCH;
	    אחרת
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة FNAME:
	    אם (!vim_isfilec(c))
		status = RA_NOMATCH;
	    אחרת
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة SFNAME:
	    אם (VIM_ISDIGIT(*rex.input) || !vim_isfilec(c))
		status = RA_NOMATCH;
	    אחרת
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة PRINT:
	    אם (!vim_isprintc(PTR2CHAR(rex.input)))
		status = RA_NOMATCH;
	    אחרת
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة SPRINT:
	    אם (VIM_ISDIGIT(*rex.input) || !vim_isprintc(PTR2CHAR(rex.input)))
		status = RA_NOMATCH;
	    אחרת
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة WHITE:
	    אם (!VIM_ISWHITE(c))
		status = RA_NOMATCH;
	    אחרת
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة NWHITE:
	    אם (c == NUL || VIM_ISWHITE(c))
		status = RA_NOMATCH;
	    אחרת
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة DIGIT:
	    אם (!ri_digit(c))
		status = RA_NOMATCH;
	    אחרת
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة NDIGIT:
	    אם (c == NUL || ri_digit(c))
		status = RA_NOMATCH;
	    אחרת
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة HEX:
	    אם (!ri_hex(c))
		status = RA_NOMATCH;
	    אחרת
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة NHEX:
	    אם (c == NUL || ri_hex(c))
		status = RA_NOMATCH;
	    אחרת
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة OCTAL:
	    אם (!ri_octal(c))
		status = RA_NOMATCH;
	    אחרת
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة NOCTAL:
	    אם (c == NUL || ri_octal(c))
		status = RA_NOMATCH;
	    אחרת
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة WORD:
	    אם (!ri_word(c))
		status = RA_NOMATCH;
	    אחרת
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة NWORD:
	    אם (c == NUL || ri_word(c))
		status = RA_NOMATCH;
	    אחרת
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة HEAD:
	    אם (!ri_head(c))
		status = RA_NOMATCH;
	    אחרת
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة NHEAD:
	    אם (c == NUL || ri_head(c))
		status = RA_NOMATCH;
	    אחרת
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة ALPHA:
	    אם (!ri_alpha(c))
		status = RA_NOMATCH;
	    אחרת
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة NALPHA:
	    אם (c == NUL || ri_alpha(c))
		status = RA_NOMATCH;
	    אחרת
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة LOWER:
	    אם (!ri_lower(c))
		status = RA_NOMATCH;
	    אחרת
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة NLOWER:
	    אם (c == NUL || ri_lower(c))
		status = RA_NOMATCH;
	    אחרת
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة UPPER:
	    אם (!ri_upper(c))
		status = RA_NOMATCH;
	    אחרת
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة NUPPER:
	    אם (c == NUL || ri_upper(c))
		status = RA_NOMATCH;
	    אחרת
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة EXACTLY:
	    {
		صحيح	len;
		char_u	*opnd;

		opnd = OPERAND(scan);
		// Inline the first byte, for speed.
		אם (*opnd != *rex.input
			&& (!rex.reg_ic
			    || (!enc_utf8
			      && MB_TOLOWER(*opnd) != MB_TOLOWER(*rex.input))))
		    status = RA_NOMATCH;
		אחרת אם (*opnd == NUL)
		{
		    // match empty string always works; happens when "~" is
		    // empty.
		}
		אחרת
		{
		    אם (opnd[1] == NUL && !(enc_utf8 && rex.reg_ic))
		    {
			len = 1;	// matched a single byte above
		    }
		    אחרת
		    {
			// Need to match first byte again for multi-byte.
			len = (صحيح)STRLEN(opnd);
			אם (cstrncmp(opnd, rex.input, &len) != 0)
			    status = RA_NOMATCH;
		    }
		    // Check for following composing character, unless %C
		    // follows (skips over all composing chars).
		    אם (status != RA_NOMATCH
			    && enc_utf8
			    && UTF_COMPOSINGLIKE(rex.input, rex.input + len)
			    && !rex.reg_icombine
			    && OP(next) != RE_COMPOSING)
		    {
			// raaron: This code makes a composing character get
			// ignored, which is the correct behavior (sometimes)
			// for voweled Hebrew texts.
			status = RA_NOMATCH;
		    }
		    אם (status != RA_NOMATCH)
			rex.input += len;
		}
	    }
	    اكسر;

	  حالة ANYOF:
	  حالة ANYBUT:
	    {
		char_u  *q = OPERAND(scan);

		אם (c == NUL)
		    status = RA_NOMATCH;
		אחרת אם ((cstrchr(q, c) == عدم) == (op == ANYOF))
		    status = RA_NOMATCH;
		אחרת
		{
		    // Check following combining characters
		    صحيح	len = 0;
		    صحيح i;

		    אם (enc_utf8)
			len = utfc_ptr2len(q) - utf_ptr2len(q);

		    MB_CPTR_ADV(rex.input);
		    MB_CPTR_ADV(q);

		    אם (!enc_utf8 || len == 0)
			اكسر;

		    עבור (i = 0; i < len; ++i)
			אם (q[i] != rex.input[i])
			{
			    status = RA_NOMATCH;
			    اكسر;
			}
		    rex.input += len;
		}
		اكسر;
	    }

	  حالة MULTIBYTECODE:
	    אם (has_mbyte)
	    {
		صحيح	i, len;
		char_u	*opnd;
		صحيح	opndc = 0, inpc;

		opnd = OPERAND(scan);
		// Safety check (just in case 'encoding' was changed since
		// compiling the program).
		אם ((len = (*mb_ptr2len)(opnd)) < 2)
		{
		    status = RA_NOMATCH;
		    اكسر;
		}
		אם (enc_utf8)
		    opndc = utf_ptr2char(opnd);
		אם (enc_utf8 && utf_iscomposing(opndc))
		{
		    // When only a composing char is given match at any
		    // position where that composing char appears.
		    status = RA_NOMATCH;
		    עבור (i = 0; rex.input[i] != NUL;
						i += utf_ptr2len(rex.input + i))
		    {
			inpc = utf_ptr2char(rex.input + i);
			אם (!utf_iscomposing(inpc))
			{
			    אם (i > 0)
				اكسر;
			}
			אחרת אם (opndc == inpc)
			{
			    // Include all following composing chars.
			    len = i + utfc_ptr2len(rex.input + i);
			    status = RA_MATCH;
			    اكسر;
			}
		    }
		}
		אחרת אם (enc_utf8)
		{
		    אם (cstrncmp(opnd, rex.input, &len) != 0)
		    {
			status = RA_NOMATCH;
			اكسر;
		    }
		}
		אחרת
		    עבור (i = 0; i < len; ++i)
			אם (opnd[i] != rex.input[i])
			{
			    status = RA_NOMATCH;
			    اكسر;
			}
		rex.input += len;
	    }
	    אחרת
		status = RA_NOMATCH;
	    اكسر;
	  حالة RE_COMPOSING:
	    אם (enc_utf8)
	    {
		// Skip composing characters.
		כלעוד (utf_iscomposing(utf_ptr2char(rex.input)))
		    MB_CPTR_ADV(rex.input);
	    }
	    اكسر;

	  حالة NOTHING:
	    اكسر;

	  حالة BACK:
	    {
		صحيح		i;
		backpos_T	*bp;

		// When we run into BACK we need to check if we don't keep
		// looping without matching any input.  The second and later
		// times a BACK is encountered it fails if the input is still
		// at the same position as the previous time.
		// The positions are stored in "backpos" and found by the
		// current value of "scan", the position in the RE program.
		bp = (backpos_T *)backpos.ga_data;
		עבור (i = 0; i < backpos.ga_len; ++i)
		    אם (bp[i].bp_scan == scan)
			اكسر;
		אם (i == backpos.ga_len)
		{
		    // First time at this BACK, make room to store the pos.
		    אם (GA_GROW_FAILS(&backpos, 1))
			status = RA_FAIL;
		    אחרת
		    {
			// get "ga_data" again, it may have changed
			bp = (backpos_T *)backpos.ga_data;
			bp[i].bp_scan = scan;
			++backpos.ga_len;
		    }
		}
		אחרת אם (reg_save_equal(&bp[i].bp_pos))
		    // Still at same position as last time, fail.
		    status = RA_NOMATCH;

		אם (status != RA_FAIL && status != RA_NOMATCH)
		    reg_save(&bp[i].bp_pos, &backpos);
	    }
	    اكسر;

	  حالة MOPEN + 0:   // Match start: \zs
	  حالة MOPEN + 1:   // \(
	  حالة MOPEN + 2:
	  حالة MOPEN + 3:
	  حالة MOPEN + 4:
	  حالة MOPEN + 5:
	  حالة MOPEN + 6:
	  حالة MOPEN + 7:
	  حالة MOPEN + 8:
	  حالة MOPEN + 9:
	    {
		no = op - MOPEN;
		cleanup_subexpr();
		rp = regstack_push(RS_MOPEN, scan);
		אם (rp == عدم)
		    status = RA_FAIL;
		אחרת
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_startpos[no],
							  &rex.reg_startp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    اكسر;

	  حالة NOPEN:	    // \%(
	  حالة NCLOSE:	    // \) after \%(
		אם (regstack_push(RS_NOPEN, scan) == عدم)
		    status = RA_FAIL;
		// We simply continue and handle the result when done.
		اكسر;

#ifdef FEAT_SYN_HL
	  حالة ZOPEN + 1:
	  حالة ZOPEN + 2:
	  حالة ZOPEN + 3:
	  حالة ZOPEN + 4:
	  حالة ZOPEN + 5:
	  حالة ZOPEN + 6:
	  حالة ZOPEN + 7:
	  حالة ZOPEN + 8:
	  حالة ZOPEN + 9:
	    {
		no = op - ZOPEN;
		cleanup_zsubexpr();
		rp = regstack_push(RS_ZOPEN, scan);
		אם (rp == عدم)
		    status = RA_FAIL;
		אחרת
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_startzpos[no],
							     &reg_startzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    اكسر;
#endif

	  حالة MCLOSE + 0:  // Match end: \ze
	  حالة MCLOSE + 1:  // \)
	  حالة MCLOSE + 2:
	  حالة MCLOSE + 3:
	  حالة MCLOSE + 4:
	  حالة MCLOSE + 5:
	  حالة MCLOSE + 6:
	  حالة MCLOSE + 7:
	  حالة MCLOSE + 8:
	  حالة MCLOSE + 9:
	    {
		no = op - MCLOSE;
		cleanup_subexpr();
		rp = regstack_push(RS_MCLOSE, scan);
		אם (rp == عدم)
		    status = RA_FAIL;
		אחרת
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_endpos[no],
							    &rex.reg_endp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    اكسر;

#ifdef FEAT_SYN_HL
	  حالة ZCLOSE + 1:  // \) after \z(
	  حالة ZCLOSE + 2:
	  حالة ZCLOSE + 3:
	  حالة ZCLOSE + 4:
	  حالة ZCLOSE + 5:
	  حالة ZCLOSE + 6:
	  حالة ZCLOSE + 7:
	  حالة ZCLOSE + 8:
	  حالة ZCLOSE + 9:
	    {
		no = op - ZCLOSE;
		cleanup_zsubexpr();
		rp = regstack_push(RS_ZCLOSE, scan);
		אם (rp == عدم)
		    status = RA_FAIL;
		אחרת
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_endzpos[no],
							      &reg_endzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    اكسر;
#endif

	  حالة BACKREF + 1:
	  حالة BACKREF + 2:
	  حالة BACKREF + 3:
	  حالة BACKREF + 4:
	  حالة BACKREF + 5:
	  حالة BACKREF + 6:
	  حالة BACKREF + 7:
	  حالة BACKREF + 8:
	  حالة BACKREF + 9:
	    {
		صحيح		len;

		no = op - BACKREF;
		cleanup_subexpr();
		אם (!REG_MULTI)		// Single-line regexp
		{
		    אם (rex.reg_startp[no] == عدم || rex.reg_endp[no] == عدم)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    אחרת
		    {
			// Compare current input with back-ref in the same
			// line.
			len = (صحيح)(rex.reg_endp[no] - rex.reg_startp[no]);
			אם (cstrncmp(rex.reg_startp[no], rex.input, &len) != 0)
			    status = RA_NOMATCH;
		    }
		}
		אחרת				// Multi-line regexp
		{
		    אם (rex.reg_startpos[no].lnum < 0
						|| rex.reg_endpos[no].lnum < 0)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    אחרת
		    {
			אם (rex.reg_startpos[no].lnum == rex.lnum
				&& rex.reg_endpos[no].lnum == rex.lnum)
			{
			    // Compare back-ref within the current line.
			    len = rex.reg_endpos[no].col
						    - rex.reg_startpos[no].col;
			    אם (cstrncmp(rex.line + rex.reg_startpos[no].col,
							  rex.input, &len) != 0)
				status = RA_NOMATCH;
			}
			אחרת
			{
			    // Messy situation: Need to compare between two
			    // lines.
			    صحيح r = match_with_backref(
					    rex.reg_startpos[no].lnum,
					    rex.reg_startpos[no].col,
					    rex.reg_endpos[no].lnum,
					    rex.reg_endpos[no].col,
					    &len);

			    אם (r != RA_MATCH)
				status = r;
			}
		    }
		}

		// Matched the backref, skip over it.
		rex.input += len;
	    }
	    اكسر;

#ifdef FEAT_SYN_HL
	  حالة ZREF + 1:
	  حالة ZREF + 2:
	  حالة ZREF + 3:
	  حالة ZREF + 4:
	  حالة ZREF + 5:
	  حالة ZREF + 6:
	  حالة ZREF + 7:
	  حالة ZREF + 8:
	  حالة ZREF + 9:
	    {
		صحيح	len;

		cleanup_zsubexpr();
		no = op - ZREF;
		אם (re_extmatch_in != عدم
			&& re_extmatch_in->matches[no] != عدم)
		{
		    len = (صحيح)STRLEN(re_extmatch_in->matches[no]);
		    אם (cstrncmp(re_extmatch_in->matches[no],
							  rex.input, &len) != 0)
			status = RA_NOMATCH;
		    אחרת
			rex.input += len;
		}
		אחרת
		{
		    // Backref was not set: Match an empty string.
		}
	    }
	    اكسر;
#endif

	  حالة BRANCH:
	    {
		אם (OP(next) != BRANCH) // No choice.
		    next = OPERAND(scan);	// Avoid recursion.
		אחרת
		{
		    rp = regstack_push(RS_BRANCH, scan);
		    אם (rp == عدم)
			status = RA_FAIL;
		    אחרת
			status = RA_BREAK;	// rest is below
		}
	    }
	    اكسر;

	  حالة BRACE_LIMITS:
	    {
		אם (OP(next) == BRACE_SIMPLE)
		{
		    bl_minval = OPERAND_MIN(scan);
		    bl_maxval = OPERAND_MAX(scan);
		}
		אחרת אם (OP(next) >= BRACE_COMPLEX
			&& OP(next) < BRACE_COMPLEX + 10)
		{
		    no = OP(next) - BRACE_COMPLEX;
		    brace_min[no] = OPERAND_MIN(scan);
		    brace_max[no] = OPERAND_MAX(scan);
		    brace_count[no] = 0;
		}
		אחרת
		{
		    internal_error("BRACE_LIMITS");
		    status = RA_FAIL;
		}
	    }
	    اكسر;

	  حالة BRACE_COMPLEX + 0:
	  حالة BRACE_COMPLEX + 1:
	  حالة BRACE_COMPLEX + 2:
	  حالة BRACE_COMPLEX + 3:
	  حالة BRACE_COMPLEX + 4:
	  حالة BRACE_COMPLEX + 5:
	  حالة BRACE_COMPLEX + 6:
	  حالة BRACE_COMPLEX + 7:
	  حالة BRACE_COMPLEX + 8:
	  حالة BRACE_COMPLEX + 9:
	    {
		no = op - BRACE_COMPLEX;
		++brace_count[no];

		// If not matched enough times yet, try one more
		אם (brace_count[no] <= (brace_min[no] <= brace_max[no]
					     ? brace_min[no] : brace_max[no]))
		{
		    rp = regstack_push(RS_BRCPLX_MORE, scan);
		    אם (rp == عدم)
			status = RA_FAIL;
		    אחרת
		    {
			rp->rs_no = no;
			reg_save(&rp->rs_un.regsave, &backpos);
			next = OPERAND(scan);
			// We continue and handle the result when done.
		    }
		    اكسر;
		}

		// If matched enough times, may try matching some more
		אם (brace_min[no] <= brace_max[no])
		{
		    // Range is the normal way around, use longest match
		    אם (brace_count[no] <= brace_max[no])
		    {
			rp = regstack_push(RS_BRCPLX_LONG, scan);
			אם (rp == عدم)
			    status = RA_FAIL;
			אחרת
			{
			    rp->rs_no = no;
			    reg_save(&rp->rs_un.regsave, &backpos);
			    next = OPERAND(scan);
			    // We continue and handle the result when done.
			}
		    }
		}
		אחרת
		{
		    // Range is backwards, use shortest match first
		    אם (brace_count[no] <= brace_min[no])
		    {
			rp = regstack_push(RS_BRCPLX_SHORT, scan);
			אם (rp == عدم)
			    status = RA_FAIL;
			אחרת
			{
			    reg_save(&rp->rs_un.regsave, &backpos);
			    // We continue and handle the result when done.
			}
		    }
		}
	    }
	    اكسر;

	  حالة BRACE_SIMPLE:
	  حالة STAR:
	  حالة PLUS:
	    {
		regstar_T	rst;

		// Lookahead to avoid useless match attempts when we know
		// what character comes next.
		אם (OP(next) == EXACTLY)
		{
		    rst.nextb = *OPERAND(next);
		    אם (rex.reg_ic)
		    {
			אם (MB_ISUPPER(rst.nextb))
			    rst.nextb_ic = MB_TOLOWER(rst.nextb);
			אחרת
			    rst.nextb_ic = MB_TOUPPER(rst.nextb);
		    }
		    אחרת
			rst.nextb_ic = rst.nextb;
		}
		אחרת
		{
		    rst.nextb = NUL;
		    rst.nextb_ic = NUL;
		}
		אם (op != BRACE_SIMPLE)
		{
		    rst.minval = (op == STAR) ? 0 : 1;
		    rst.maxval = MAX_LIMIT;
		}
		אחרת
		{
		    rst.minval = bl_minval;
		    rst.maxval = bl_maxval;
		}

		// When maxval > minval, try matching as much as possible, up
		// to maxval.  When maxval < minval, try matching at least the
		// minimal number (since the range is backwards, that's also
		// maxval!).
		rst.count = regrepeat(OPERAND(scan), rst.maxval);
		אם (got_int)
		{
		    status = RA_FAIL;
		    اكسر;
		}
		אם (rst.minval <= rst.maxval
			  ? rst.count >= rst.minval : rst.count >= rst.maxval)
		{
		    // It could match.  Prepare for trying to match what
		    // follows.  The code is below.  Parameters are stored in
		    // a regstar_T on the regstack.
		    אם ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
		    {
			emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
			status = RA_FAIL;
		    }
		    אחרת אם (GA_GROW_FAILS(&regstack, (صحيح)حجم(regstar_T)))
			status = RA_FAIL;
		    אחרת
		    {
			regstack.ga_len += حجم(regstar_T);
			rp = regstack_push(rst.minval <= rst.maxval
					? RS_STAR_LONG : RS_STAR_SHORT, scan);
			אם (rp == عدم)
			    status = RA_FAIL;
			אחרת
			{
			    *(((regstar_T *)rp) - 1) = rst;
			    status = RA_BREAK;	    // skip the restore bits
			}
		    }
		}
		אחרת
		    status = RA_NOMATCH;

	    }
	    اكسر;

	  حالة NOMATCH:
	  حالة MATCH:
	  حالة SUBPAT:
	    rp = regstack_push(RS_NOMATCH, scan);
	    אם (rp == عدم)
		status = RA_FAIL;
	    אחרת
	    {
		rp->rs_no = op;
		reg_save(&rp->rs_un.regsave, &backpos);
		next = OPERAND(scan);
		// We continue and handle the result when done.
	    }
	    اكسر;

	  حالة BEHIND:
	  حالة NOBEHIND:
	    // Need a bit of room to store extra positions.
	    אם ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
	    {
		emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		status = RA_FAIL;
	    }
	    אחרת אם (GA_GROW_FAILS(&regstack, (صحيح)حجم(regbehind_T)))
		status = RA_FAIL;
	    אחרת
	    {
		regstack.ga_len += حجم(regbehind_T);
		rp = regstack_push(RS_BEHIND1, scan);
		אם (rp == عدم)
		    status = RA_FAIL;
		אחרת
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
	    اكسر;

	  حالة BHPOS:
	    אם (REG_MULTI)
	    {
		אם (behind_pos.rs_u.pos.col != (colnr_T)(rex.input - rex.line)
			|| behind_pos.rs_u.pos.lnum != rex.lnum)
		    status = RA_NOMATCH;
	    }
	    אחרת אם (behind_pos.rs_u.ptr != rex.input)
		status = RA_NOMATCH;
	    اكسر;

	  حالة NEWL:
	    אם ((c != NUL || !REG_MULTI || rex.lnum > rex.reg_maxline
			     || rex.reg_line_lbr)
					   && (c != '\n' || !rex.reg_line_lbr))
		status = RA_NOMATCH;
	    אחרת אם (rex.reg_line_lbr)
		ADVANCE_REGINPUT();
	    אחרת
		reg_nextline();
	    اكسر;

	  حالة END:
	    status = RA_MATCH;	// Success!
	    اكسر;

	  افتراضي:
	    iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	    printf("Illegal op code %d\n", op);
#endif
	    status = RA_FAIL;
	    اكسر;
	  }
	}

	// If we can't continue sequentially, break the inner loop.
	אם (status != RA_CONT)
	    اكسر;

	// Continue in inner loop, advance to next item.
	scan = next;

    } // end of inner loop

    // If there is something on the regstack execute the code for the state.
    // If the state is popped then loop and use the older state.
    כלעוד (regstack.ga_len > 0 && status != RA_FAIL)
    {
	rp = (regitem_T *)((حرف *)regstack.ga_data + regstack.ga_len) - 1;
	بدل (rp->rs_state)
	{
	  حالة RS_NOPEN:
	    // Result is passed on as-is, simply pop the state.
	    regstack_pop(&scan);
	    اكسر;

	  حالة RS_MOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    אם (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_startpos[rp->rs_no],
						  &rex.reg_startp[rp->rs_no]);
	    regstack_pop(&scan);
	    اكسر;

#ifdef FEAT_SYN_HL
	  حالة RS_ZOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    אם (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_startzpos[rp->rs_no],
						 &reg_startzp[rp->rs_no]);
	    regstack_pop(&scan);
	    اكسر;
#endif

	  حالة RS_MCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    אם (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_endpos[rp->rs_no],
						    &rex.reg_endp[rp->rs_no]);
	    regstack_pop(&scan);
	    اكسر;

#ifdef FEAT_SYN_HL
	  حالة RS_ZCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    אם (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_endzpos[rp->rs_no],
						   &reg_endzp[rp->rs_no]);
	    regstack_pop(&scan);
	    اكسر;
#endif

	  حالة RS_BRANCH:
	    אם (status == RA_MATCH)
		// this branch matched, use it
		regstack_pop(&scan);
	    אחרת
	    {
		אם (status != RA_BREAK)
		{
		    // After a non-matching branch: try next one.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    scan = rp->rs_scan;
		}
		אם (scan == عدم || OP(scan) != BRANCH)
		{
		    // no more branches, didn't find a match
		    status = RA_NOMATCH;
		    regstack_pop(&scan);
		}
		אחרת
		{
		    // Prepare to try a branch.
		    rp->rs_scan = regnext(scan);
		    reg_save(&rp->rs_un.regsave, &backpos);
		    scan = OPERAND(scan);
		}
	    }
	    اكسر;

	  حالة RS_BRCPLX_MORE:
	    // Pop the state.  Restore pointers when there is no match.
	    אם (status == RA_NOMATCH)
	    {
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_count[rp->rs_no];	// decrement match count
	    }
	    regstack_pop(&scan);
	    اكسر;

	  حالة RS_BRCPLX_LONG:
	    // Pop the state.  Restore pointers when there is no match.
	    אם (status == RA_NOMATCH)
	    {
		// There was no match, but we did find enough matches.
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_count[rp->rs_no];
		// continue with the items after "\{}"
		status = RA_CONT;
	    }
	    regstack_pop(&scan);
	    אם (status == RA_CONT)
		scan = regnext(scan);
	    اكسر;

	  حالة RS_BRCPLX_SHORT:
	    // Pop the state.  Restore pointers when there is no match.
	    אם (status == RA_NOMATCH)
		// There was no match, try to match one more item.
		reg_restore(&rp->rs_un.regsave, &backpos);
	    regstack_pop(&scan);
	    אם (status == RA_NOMATCH)
	    {
		scan = OPERAND(scan);
		status = RA_CONT;
	    }
	    اكسر;

	  حالة RS_NOMATCH:
	    // Pop the state.  If the operand matches for NOMATCH or
	    // doesn't match for MATCH/SUBPAT, we fail.  Otherwise backup,
	    // except for SUBPAT, and continue with the next item.
	    אם (status == (rp->rs_no == NOMATCH ? RA_MATCH : RA_NOMATCH))
		status = RA_NOMATCH;
	    אחרת
	    {
		status = RA_CONT;
		אם (rp->rs_no != SUBPAT)	// zero-width
		    reg_restore(&rp->rs_un.regsave, &backpos);
	    }
	    regstack_pop(&scan);
	    אם (status == RA_CONT)
		scan = regnext(scan);
	    اكسر;

	  حالة RS_BEHIND1:
	    אם (status == RA_NOMATCH)
	    {
		regstack_pop(&scan);
		regstack.ga_len -= حجم(regbehind_T);
	    }
	    אחרת
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
	    اكسر;

	  حالة RS_BEHIND2:
	    // Looping for BEHIND / NOBEHIND match.
	    אם (status == RA_MATCH && reg_save_equal(&behind_pos))
	    {
		// found a match that ends where "next" started
		behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		אם (rp->rs_no == BEHIND)
		    reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
		אחרת
		{
		    // But we didn't want a match.  Need to restore the
		    // subexpr, because what follows matched, so they have
		    // been set.
		    status = RA_NOMATCH;
		    restore_subexpr(((regbehind_T *)rp) - 1);
		}
		regstack_pop(&scan);
		regstack.ga_len -= حجم(regbehind_T);
	    }
	    אחרת
	    {
		long limit;

		// No match or a match that doesn't end where we want it: Go
		// back one character.  May go to previous line once.
		no = OK;
		limit = OPERAND_MIN(rp->rs_scan);
		אם (REG_MULTI)
		{
		    אם (limit > 0
			    && ((rp->rs_un.regsave.rs_u.pos.lnum
						    < behind_pos.rs_u.pos.lnum
				    ? (colnr_T)STRLEN(rex.line)
				    : behind_pos.rs_u.pos.col)
				- rp->rs_un.regsave.rs_u.pos.col >= limit))
			no = FAIL;
		    אחרת אם (rp->rs_un.regsave.rs_u.pos.col == 0)
		    {
			אם (rp->rs_un.regsave.rs_u.pos.lnum
					< behind_pos.rs_u.pos.lnum
				|| reg_getline(
					--rp->rs_un.regsave.rs_u.pos.lnum)
								  == عدم)
			    no = FAIL;
			אחרת
			{
			    reg_restore(&rp->rs_un.regsave, &backpos);
			    rp->rs_un.regsave.rs_u.pos.col =
						 (colnr_T)STRLEN(rex.line);
			}
		    }
		    אחרת
		    {
			אם (has_mbyte)
			{
			    char_u *line =
				  reg_getline(rp->rs_un.regsave.rs_u.pos.lnum);

			    rp->rs_un.regsave.rs_u.pos.col -=
				(*mb_head_off)(line, line
				    + rp->rs_un.regsave.rs_u.pos.col - 1) + 1;
			}
			אחרת
			    --rp->rs_un.regsave.rs_u.pos.col;
		    }
		}
		אחרת
		{
		    אם (rp->rs_un.regsave.rs_u.ptr == rex.line)
			no = FAIL;
		    אחרת
		    {
			MB_PTR_BACK(rex.line, rp->rs_un.regsave.rs_u.ptr);
			אם (limit > 0 && (long)(behind_pos.rs_u.ptr
				     - rp->rs_un.regsave.rs_u.ptr) > limit)
			    no = FAIL;
		    }
		}
		אם (no == OK)
		{
		    // Advanced, prepare for finding match again.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    scan = OPERAND(rp->rs_scan) + 4;
		    אם (status == RA_MATCH)
		    {
			// We did match, so subexpr may have been changed,
			// need to restore them for the next try.
			status = RA_NOMATCH;
			restore_subexpr(((regbehind_T *)rp) - 1);
		    }
		}
		אחרת
		{
		    // Can't advance.  For NOBEHIND that's a match.
		    behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		    אם (rp->rs_no == NOBEHIND)
		    {
			reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
			status = RA_MATCH;
		    }
		    אחרת
		    {
			// We do want a proper match.  Need to restore the
			// subexpr if we had a match, because they may have
			// been set.
			אם (status == RA_MATCH)
			{
			    status = RA_NOMATCH;
			    restore_subexpr(((regbehind_T *)rp) - 1);
			}
		    }
		    regstack_pop(&scan);
		    regstack.ga_len -= حجم(regbehind_T);
		}
	    }
	    اكسر;

	  حالة RS_STAR_LONG:
	  حالة RS_STAR_SHORT:
	    {
		regstar_T	    *rst = ((regstar_T *)rp) - 1;

		אם (status == RA_MATCH)
		{
		    regstack_pop(&scan);
		    regstack.ga_len -= حجم(regstar_T);
		    اكسر;
		}

		// Tried once already, restore input pointers.
		אם (status != RA_BREAK)
		    reg_restore(&rp->rs_un.regsave, &backpos);

		// Repeat until we found a position where it could match.
		עבור (;;)
		{
		    אם (status != RA_BREAK)
		    {
			// Tried first position already, advance.
			אם (rp->rs_state == RS_STAR_LONG)
			{
			    // Trying for longest match, but couldn't or
			    // didn't match -- back up one char.
			    אם (--rst->count < rst->minval)
				اكسر;
			    אם (rex.input == rex.line)
			    {
				// backup to last char of previous line
				אם (rex.lnum == 0)
				{
				    status = RA_NOMATCH;
				    اكسر;
				}
				--rex.lnum;
				rex.line = reg_getline(rex.lnum);
				// Just in case regrepeat() didn't count
				// right.
				אם (rex.line == عدم)
				    اكسر;
				rex.input = rex.line + reg_getline_len(rex.lnum);
				fast_breakcheck();
			    }
			    אחרת
				MB_PTR_BACK(rex.line, rex.input);
			}
			אחרת
			{
			    // Range is backwards, use shortest match first.
			    // Careful: maxval and minval are exchanged!
			    // Couldn't or didn't match: try advancing one
			    // char.
			    אם (rst->count == rst->minval
				  || regrepeat(OPERAND(rp->rs_scan), 1L) == 0)
				اكسر;
			    ++rst->count;
			}
			אם (got_int)
			    اكسر;
		    }
		    אחרת
			status = RA_NOMATCH;

		    // If it could match, try it.
		    אם (rst->nextb == NUL || *rex.input == rst->nextb
					     || *rex.input == rst->nextb_ic)
		    {
			reg_save(&rp->rs_un.regsave, &backpos);
			scan = regnext(rp->rs_scan);
			status = RA_CONT;
			اكسر;
		    }
		}
		אם (status != RA_CONT)
		{
		    // Failed.
		    regstack_pop(&scan);
		    regstack.ga_len -= حجم(regstar_T);
		    status = RA_NOMATCH;
		}
	    }
	    اكسر;
	}

	// If we want to continue the inner loop or didn't pop a state
	// continue matching loop
	אם (status == RA_CONT || rp == (regitem_T *)
			     ((حرف *)regstack.ga_data + regstack.ga_len) - 1)
	    اكسر;

#ifdef FEAT_RELTIME
	אם (bt_did_time_out(timed_out))
	{
	    status = RA_FAIL;
	    اكسر;
	}
#endif
    }

    // May need to continue with the inner loop, starting at "scan".
    אם (status == RA_CONT)
	استمر;

    // If the regstack is empty or something failed we are done.
    אם (regstack.ga_len == 0 || status == RA_FAIL)
    {
	אם (scan == عدم)
	{
	    // We get here only if there's trouble -- normally "case END" is
	    // the terminating point.
	    iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	    printf("Premature EOL\n");
#endif
	}
	החזר (status == RA_MATCH);
    }

  } // End of loop until the regstack is empty.

  // NOTREACHED
}

/*
 * regtry - try match of "prog" with at rex.line["col"].
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    ساكن long
regtry(
    bt_regprog_T	*prog,
    colnr_T		col,
    صحيح			*timed_out)	// flag set on timeout or NULL
{
    rex.input = rex.line + col;
    rex.need_clear_subexpr = TRUE;
#ifdef FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    rex.need_clear_zsubexpr = (prog->reghasz == REX_SET);
#endif

    אם (regmatch(prog->program + 1, timed_out) == 0)
	החזר 0;

    cleanup_subexpr();
    אם (REG_MULTI)
    {
	אם (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].col = col;
	}
	אם (rex.reg_endpos[0].lnum < 0)
	{
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].col = (صحيح)(rex.input - rex.line);
	}
	אחרת
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    אחרת
    {
	אם (rex.reg_startp[0] == عدم)
	    rex.reg_startp[0] = rex.line + col;
	אם (rex.reg_endp[0] == عدم)
	    rex.reg_endp[0] = rex.input;
    }
#ifdef FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = عدم;

    אם (prog->reghasz == REX_SET)
    {
	صحيح		i;

	cleanup_zsubexpr();
	re_extmatch_out = make_extmatch();
	אם (re_extmatch_out == عدم)
	    החזר 0;
	עבור (i = 0; i < NSUBEXP; i++)
	{
	    אם (REG_MULTI)
	    {
		// Only accept single line matches.
		אם (reg_startzpos[i].lnum >= 0
			&& reg_endzpos[i].lnum == reg_startzpos[i].lnum
			&& reg_endzpos[i].col >= reg_startzpos[i].col)
		    re_extmatch_out->matches[i] =
			vim_strnsave(reg_getline(reg_startzpos[i].lnum)
						       + reg_startzpos[i].col,
				   reg_endzpos[i].col - reg_startzpos[i].col);
	    }
	    אחרת
	    {
		אם (reg_startzp[i] != عدم && reg_endzp[i] != عدم)
		    re_extmatch_out->matches[i] =
			    vim_strnsave(reg_startzp[i],
						reg_endzp[i] - reg_startzp[i]);
	    }
	}
    }
#endif
    החזר 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    ساكن long
bt_regexec_both(
    char_u	*line,
    colnr_T	startcol,	// column to start looking for match
    صحيح		*timed_out)	// flag set on timeout or NULL
{
    bt_regprog_T    *prog;
    char_u	    *s;
    colnr_T	    col = startcol;
    long	    retval = 0L;

    // Create "regstack" and "backpos" if they are not allocated yet.
    // We allocate *_INITIAL amount of bytes first and then set the grow size
    // to much bigger value to avoid many malloc calls in case of deep regular
    // expressions.
    אם (regstack.ga_data == عدم)
    {
	// Use an item size of 1 byte, since we push different things
	// onto the regstack.
	ga_init2(&regstack, 1, REGSTACK_INITIAL);
	(فراغ)ga_grow(&regstack, REGSTACK_INITIAL);
	regstack.ga_growsize = REGSTACK_INITIAL * 8;
    }

    אם (backpos.ga_data == عدم)
    {
	ga_init2(&backpos, حجم(backpos_T), BACKPOS_INITIAL);
	(فراغ)ga_grow(&backpos, BACKPOS_INITIAL);
	backpos.ga_growsize = BACKPOS_INITIAL * 8;
    }

    אם (REG_MULTI)
    {
	prog = (bt_regprog_T *)rex.reg_mmatch->regprog;
	line = reg_getline((linenr_T)0);
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    אחרת
    {
	prog = (bt_regprog_T *)rex.reg_match->regprog;
	rex.reg_startp = rex.reg_match->startp;
	rex.reg_endp = rex.reg_match->endp;
    }

    // Be paranoid...
    אם (prog == عدم || line == عدم)
    {
	iemsg(e_null_argument);
	اذهب theend;
    }

    // Check validity of program.
    אם (prog_magic_wrong())
	اذهب theend;

    // If the start column is past the maximum column: no need to try.
    אם (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	اذهب theend;

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    אם (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    אחרת אם (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    אם (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    // If there is a "must appear" string, look for it.
    אם (prog->regmust != عدم)
    {
	صحيح c;

	אם (has_mbyte)
	    c = (*mb_ptr2char)(prog->regmust);
	אחרת
	    c = *prog->regmust;
	s = line + col;

	// This is used very often, esp. for ":global".  Use three versions of
	// the loop to avoid overhead of conditions.
	אם (!rex.reg_ic && !has_mbyte)
	    כלעוד ((s = vim_strbyte(s, c)) != عدم)
	    {
		صحيح len = prog->regmlen;
		אם (cstrncmp(s, prog->regmust, &len) == 0)
		    اكسر;		// Found it.
		++s;
	    }
	אחרת אם (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	    כלעוד ((s = vim_strchr(s, c)) != عدم)
	    {
		صحيح len = prog->regmlen;
		אם (cstrncmp(s, prog->regmust, &len) == 0)
		    اكسر;		// Found it.
		MB_PTR_ADV(s);
	    }
	אחרת
	    כלעוד ((s = cstrchr(s, c)) != عدم)
	    {
		صحيح len = prog->regmlen;
		אם (cstrncmp(s, prog->regmust, &len) == 0)
		    اكسر;		// Found it.
		MB_PTR_ADV(s);
	    }
	אם (s == عدم)		// Not present.
	    اذهب theend;
    }

    rex.line = line;
    rex.lnum = 0;
    reg_toolong = FALSE;

    // Simplest case: Anchored match need be tried only once.
    אם (prog->reganch)
    {
	صحيح	c;

	אם (has_mbyte)
	    c = (*mb_ptr2char)(rex.line + col);
	אחרת
	    c = rex.line[col];
	אם (prog->regstart == NUL
		|| prog->regstart == c
		|| (rex.reg_ic
		    && (((enc_utf8 && utf_fold(prog->regstart) == utf_fold(c)))
			|| (c < 255 && prog->regstart < 255 &&
			    MB_TOLOWER(prog->regstart) == MB_TOLOWER(c)))))
	    retval = regtry(prog, col, timed_out);
	אחרת
	    retval = 0;
    }
    אחרת
    {
	// Messy cases:  unanchored match.
	כלעוד (!got_int)
	{
	    אם (prog->regstart != NUL)
	    {
		// Skip until the char we know it must start with.
		// Used often, do some work to avoid call overhead.
		אם (!rex.reg_ic && !has_mbyte)
		    s = vim_strbyte(rex.line + col, prog->regstart);
		אחרת
		    s = cstrchr(rex.line + col, prog->regstart);
		אם (s == عدم)
		{
		    retval = 0;
		    اكسر;
		}
		col = (صحيح)(s - rex.line);
	    }

	    // Check for maximum column to try.
	    אם (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	    {
		retval = 0;
		اكسر;
	    }

	    retval = regtry(prog, col, timed_out);
	    אם (retval > 0)
		اكسر;

	    // if not currently on the first line, get it again
	    אם (rex.lnum != 0)
	    {
		rex.lnum = 0;
		rex.line = reg_getline((linenr_T)0);
	    }
	    אם (rex.line[col] == NUL)
		اكسر;
	    אם (has_mbyte)
		col += (*mb_ptr2len)(rex.line + col);
	    אחרת
		++col;
#ifdef FEAT_RELTIME
	    אם (bt_did_time_out(timed_out))
		اكسر;
#endif
	}
    }

theend:
    // Free "reg_tofree" when it's a bit big.
    // Free regstack and backpos if they are bigger than their initial size.
    אם (reg_tofreelen > 400)
	VIM_CLEAR(reg_tofree);
    אם (regstack.ga_maxlen > REGSTACK_INITIAL)
	ga_clear(&regstack);
    אם (backpos.ga_maxlen > BACKPOS_INITIAL)
	ga_clear(&backpos);

    אם (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs
	// and \ze are used.
	אם (REG_MULTI)
	{
	    lpos_T *start = &rex.reg_mmatch->startpos[0];
	    lpos_T *end = &rex.reg_mmatch->endpos[0];

	    אם (end->lnum < start->lnum
			|| (end->lnum == start->lnum && end->col < start->col))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_mmatch->rmm_matchcol = col;
	}
	אחרת
	{
	    אם (rex.reg_match->endp[0] < rex.reg_match->startp[0])
		rex.reg_match->endp[0] = rex.reg_match->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_match->rm_matchcol = col;
	}
    }

    החזר retval;
}

/*
 * Match a regexp against a string.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 * if "line_lbr" is TRUE  consider a "\n" in "line" to be a line break.
 *
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    ساكن صحيح
bt_regexec_nl(
    regmatch_T	*rmp,
    char_u	*line,	// string to match against
    colnr_T	col,	// column to start looking for match
    صحيح		line_lbr)
{
    rex.reg_match = rmp;
    rex.reg_mmatch = عدم;
    rex.reg_maxline = 0;
    rex.reg_line_lbr = line_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = عدم;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;

    החזר bt_regexec_both(line, col, عدم);
}

/*
 * Match a regexp against multiple lines.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 *
 * Return zero if there is no match.  Return number of lines contained in the
 * match otherwise.
 */
    ساكن long
bt_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	col,		// column to start looking for match
    صحيح		*timed_out)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    החזר bt_regexec_both(عدم, col, timed_out);
}

/*
 * Compare a number with the operand of RE_LNUM, RE_COL or RE_VCOL.
 */
    ساكن صحيح
re_num_cmp(long_u val, char_u *scan)
{
    long_u  n = OPERAND_MIN(scan);

    אם (OPERAND_CMP(scan) == '>')
	החזר val > n;
    אם (OPERAND_CMP(scan) == '<')
	החזר val < n;
    החזר val == n;
}

#ifdef BT_REGEXP_DUMP

/*
 * regdump - dump a regexp onto stdout in vaguely comprehensible form
 */
    ساكن فراغ
regdump(char_u *pattern, bt_regprog_T *r)
{
    char_u  *s;
    صحيح	    op = EXACTLY;	// Arbitrary non-END op.
    char_u  *next;
    char_u  *end = عدم;
    FILE    *f;

# ifdef BT_REGEXP_LOG
    f = fopen("bt_regexp_log.log", "a");
# אחרת
    f = stdout;
# endif
    אם (f == عدم)
	החזר;
    fprintf(f, "-------------------------------------\n\r\nregcomp(%s):\r\n", pattern);

    s = r->program + 1;
    // Loop until we find the END that isn't before a referred next (an END
    // can also appear in a NOMATCH operand).
    כלעוד ((op != END || s <= end) && s < r->program + r->regsz)
    {
	op = OP(s);
	fprintf(f, "%2d%s", (صحيح)(s - r->program), regprop(s)); // Where, what.
	next = (s + 3 <= r->program + r->regsz) ? regnext(s) : عدم;
	אם (next == عدم)	// Next ptr.
	    fprintf(f, "(0)");
	אחרת
	    fprintf(f, "(%d)", (صحيح)((s - r->program) + (next - s)));
	אם (end < next)
	    end = next;
	אם (op == BRACE_LIMITS)
	{
	    // Two ints
	    fprintf(f, " minval %ld, maxval %ld", OPERAND_MIN(s), OPERAND_MAX(s));
	    s += 8;
	}
	אחרת אם (op == BEHIND || op == NOBEHIND)
	{
	    // one int
	    fprintf(f, " count %ld", OPERAND_MIN(s));
	    s += 4;
	}
	אחרת אם (op == RE_LNUM || op == RE_COL || op == RE_VCOL)
	{
	    // one int plus comparator
	    fprintf(f, " count %ld", OPERAND_MIN(s));
	    s += 5;
	}
	s += 3;
	אם (op == MULTIBYTECODE)
	{
	    fprintf(f, " mbc=%d", utf_ptr2char(s));
	    s += utfc_ptr2len(s);
	}
	אם (op == ANYOF || op == ANYOF + ADD_NL
		|| op == ANYBUT || op == ANYBUT + ADD_NL
		|| op == EXACTLY)
	{
	    // Literal string, where present.
	    fprintf(f, "\nxxxxxxxxx\n");
	    כלעוד (*s != NUL && s < r->program + r->regsz)
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
    אם (r->regstart != NUL)
	fprintf(f, "start `%s' 0x%x; ", r->regstart < 256
		? (حرف *)transchar(r->regstart)
		: "multibyte", r->regstart);
    אם (r->reganch)
	fprintf(f, "anchored; ");
    אם (r->regmust != عدم)
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
    ساكن char_u *
regprop(char_u *op)
{
    حرف	    *p;
    ساكن حرف	    buf[50];
    ساكن size_t   buflen = 0;

    STRCPY(buf, ":");
    buflen = 1;

    بدل ((صحيح) OP(op))
    {
      حالة BOL:
	p = "BOL";
	اكسر;
      حالة EOL:
	p = "EOL";
	اكسر;
      حالة RE_BOF:
	p = "BOF";
	اكسر;
      حالة RE_EOF:
	p = "EOF";
	اكسر;
      حالة CURSOR:
	p = "CURSOR";
	اكسر;
      حالة RE_VISUAL:
	p = "RE_VISUAL";
	اكسر;
      حالة RE_LNUM:
	p = "RE_LNUM";
	اكسر;
      حالة RE_MARK:
	p = "RE_MARK";
	اكسر;
      حالة RE_COL:
	p = "RE_COL";
	اكسر;
      حالة RE_VCOL:
	p = "RE_VCOL";
	اكسر;
      حالة BOW:
	p = "BOW";
	اكسر;
      حالة EOW:
	p = "EOW";
	اكسر;
      حالة ANY:
	p = "ANY";
	اكسر;
      حالة ANY + ADD_NL:
	p = "ANY+NL";
	اكسر;
      حالة ANYOF:
	p = "ANYOF";
	اكسر;
      حالة ANYOF + ADD_NL:
	p = "ANYOF+NL";
	اكسر;
      حالة ANYBUT:
	p = "ANYBUT";
	اكسر;
      حالة ANYBUT + ADD_NL:
	p = "ANYBUT+NL";
	اكسر;
      حالة IDENT:
	p = "IDENT";
	اكسر;
      حالة IDENT + ADD_NL:
	p = "IDENT+NL";
	اكسر;
      حالة SIDENT:
	p = "SIDENT";
	اكسر;
      حالة SIDENT + ADD_NL:
	p = "SIDENT+NL";
	اكسر;
      حالة KWORD:
	p = "KWORD";
	اكسر;
      حالة KWORD + ADD_NL:
	p = "KWORD+NL";
	اكسر;
      حالة SKWORD:
	p = "SKWORD";
	اكسر;
      حالة SKWORD + ADD_NL:
	p = "SKWORD+NL";
	اكسر;
      حالة FNAME:
	p = "FNAME";
	اكسر;
      حالة FNAME + ADD_NL:
	p = "FNAME+NL";
	اكسر;
      حالة SFNAME:
	p = "SFNAME";
	اكسر;
      حالة SFNAME + ADD_NL:
	p = "SFNAME+NL";
	اكسر;
      حالة PRINT:
	p = "PRINT";
	اكسر;
      حالة PRINT + ADD_NL:
	p = "PRINT+NL";
	اكسر;
      حالة SPRINT:
	p = "SPRINT";
	اكسر;
      حالة SPRINT + ADD_NL:
	p = "SPRINT+NL";
	اكسر;
      حالة WHITE:
	p = "WHITE";
	اكسر;
      حالة WHITE + ADD_NL:
	p = "WHITE+NL";
	اكسر;
      حالة NWHITE:
	p = "NWHITE";
	اكسر;
      حالة NWHITE + ADD_NL:
	p = "NWHITE+NL";
	اكسر;
      حالة DIGIT:
	p = "DIGIT";
	اكسر;
      حالة DIGIT + ADD_NL:
	p = "DIGIT+NL";
	اكسر;
      حالة NDIGIT:
	p = "NDIGIT";
	اكسر;
      حالة NDIGIT + ADD_NL:
	p = "NDIGIT+NL";
	اكسر;
      حالة HEX:
	p = "HEX";
	اكسر;
      حالة HEX + ADD_NL:
	p = "HEX+NL";
	اكسر;
      حالة NHEX:
	p = "NHEX";
	اكسر;
      حالة NHEX + ADD_NL:
	p = "NHEX+NL";
	اكسر;
      حالة OCTAL:
	p = "OCTAL";
	اكسر;
      حالة OCTAL + ADD_NL:
	p = "OCTAL+NL";
	اكسر;
      حالة NOCTAL:
	p = "NOCTAL";
	اكسر;
      حالة NOCTAL + ADD_NL:
	p = "NOCTAL+NL";
	اكسر;
      حالة WORD:
	p = "WORD";
	اكسر;
      حالة WORD + ADD_NL:
	p = "WORD+NL";
	اكسر;
      حالة NWORD:
	p = "NWORD";
	اكسر;
      حالة NWORD + ADD_NL:
	p = "NWORD+NL";
	اكسر;
      حالة HEAD:
	p = "HEAD";
	اكسر;
      حالة HEAD + ADD_NL:
	p = "HEAD+NL";
	اكسر;
      حالة NHEAD:
	p = "NHEAD";
	اكسر;
      حالة NHEAD + ADD_NL:
	p = "NHEAD+NL";
	اكسر;
      حالة ALPHA:
	p = "ALPHA";
	اكسر;
      حالة ALPHA + ADD_NL:
	p = "ALPHA+NL";
	اكسر;
      حالة NALPHA:
	p = "NALPHA";
	اكسر;
      حالة NALPHA + ADD_NL:
	p = "NALPHA+NL";
	اكسر;
      حالة LOWER:
	p = "LOWER";
	اكسر;
      حالة LOWER + ADD_NL:
	p = "LOWER+NL";
	اكسر;
      حالة NLOWER:
	p = "NLOWER";
	اكسر;
      حالة NLOWER + ADD_NL:
	p = "NLOWER+NL";
	اكسر;
      حالة UPPER:
	p = "UPPER";
	اكسر;
      حالة UPPER + ADD_NL:
	p = "UPPER+NL";
	اكسر;
      حالة NUPPER:
	p = "NUPPER";
	اكسر;
      حالة NUPPER + ADD_NL:
	p = "NUPPER+NL";
	اكسر;
      حالة BRANCH:
	p = "BRANCH";
	اكسر;
      حالة EXACTLY:
	p = "EXACTLY";
	اكسر;
      حالة NOTHING:
	p = "NOTHING";
	اكسر;
      حالة BACK:
	p = "BACK";
	اكسر;
      حالة END:
	p = "END";
	اكسر;
      حالة MOPEN + 0:
	p = "MATCH START";
	اكسر;
      حالة MOPEN + 1:
      حالة MOPEN + 2:
      حالة MOPEN + 3:
      حالة MOPEN + 4:
      حالة MOPEN + 5:
      حالة MOPEN + 6:
      حالة MOPEN + 7:
      حالة MOPEN + 8:
      حالة MOPEN + 9:
	buflen += vim_snprintf(buf + buflen, حجم(buf) - buflen, "MOPEN%d", OP(op) - MOPEN);
	p = عدم;
	اكسر;
      حالة MCLOSE + 0:
	p = "MATCH END";
	اكسر;
      حالة MCLOSE + 1:
      حالة MCLOSE + 2:
      حالة MCLOSE + 3:
      حالة MCLOSE + 4:
      حالة MCLOSE + 5:
      حالة MCLOSE + 6:
      حالة MCLOSE + 7:
      حالة MCLOSE + 8:
      حالة MCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, حجم(buf) - buflen, "MCLOSE%d", OP(op) - MCLOSE);
	p = عدم;
	اكسر;
      حالة BACKREF + 1:
      حالة BACKREF + 2:
      حالة BACKREF + 3:
      حالة BACKREF + 4:
      حالة BACKREF + 5:
      حالة BACKREF + 6:
      حالة BACKREF + 7:
      حالة BACKREF + 8:
      حالة BACKREF + 9:
	buflen += vim_snprintf(buf + buflen, حجم(buf) - buflen, "BACKREF%d", OP(op) - BACKREF);
	p = عدم;
	اكسر;
      حالة NOPEN:
	p = "NOPEN";
	اكسر;
      حالة NCLOSE:
	p = "NCLOSE";
	اكسر;
# ifdef FEAT_SYN_HL
      حالة ZOPEN + 1:
      حالة ZOPEN + 2:
      حالة ZOPEN + 3:
      حالة ZOPEN + 4:
      حالة ZOPEN + 5:
      حالة ZOPEN + 6:
      حالة ZOPEN + 7:
      حالة ZOPEN + 8:
      حالة ZOPEN + 9:
	buflen += vim_snprintf(buf + buflen, حجم(buf) - buflen, "ZOPEN%d", OP(op) - ZOPEN);
	p = عدم;
	اكسر;
      حالة ZCLOSE + 1:
      حالة ZCLOSE + 2:
      حالة ZCLOSE + 3:
      حالة ZCLOSE + 4:
      حالة ZCLOSE + 5:
      حالة ZCLOSE + 6:
      حالة ZCLOSE + 7:
      حالة ZCLOSE + 8:
      حالة ZCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, حجم(buf) - buflen, "ZCLOSE%d", OP(op) - ZCLOSE);
	p = عدم;
	اكسر;
      حالة ZREF + 1:
      حالة ZREF + 2:
      حالة ZREF + 3:
      حالة ZREF + 4:
      حالة ZREF + 5:
      حالة ZREF + 6:
      حالة ZREF + 7:
      حالة ZREF + 8:
      حالة ZREF + 9:
	buflen += vim_snprintf(buf + buflen, حجم(buf) - buflen, "ZREF%d", OP(op) - ZREF);
	p = عدم;
	اكسر;
# endif
      حالة STAR:
	p = "STAR";
	اكسر;
      حالة PLUS:
	p = "PLUS";
	اكسر;
      حالة NOMATCH:
	p = "NOMATCH";
	اكسر;
      حالة MATCH:
	p = "MATCH";
	اكسر;
      حالة BEHIND:
	p = "BEHIND";
	اكسر;
      حالة NOBEHIND:
	p = "NOBEHIND";
	اكسر;
      حالة SUBPAT:
	p = "SUBPAT";
	اكسر;
      حالة BRACE_LIMITS:
	p = "BRACE_LIMITS";
	اكسر;
      حالة BRACE_SIMPLE:
	p = "BRACE_SIMPLE";
	اكسر;
      حالة BRACE_COMPLEX + 0:
      حالة BRACE_COMPLEX + 1:
      حالة BRACE_COMPLEX + 2:
      حالة BRACE_COMPLEX + 3:
      حالة BRACE_COMPLEX + 4:
      حالة BRACE_COMPLEX + 5:
      حالة BRACE_COMPLEX + 6:
      حالة BRACE_COMPLEX + 7:
      حالة BRACE_COMPLEX + 8:
      حالة BRACE_COMPLEX + 9:
	buflen += vim_snprintf(buf + buflen, حجم(buf) - buflen, "BRACE_COMPLEX%d", OP(op) - BRACE_COMPLEX);
	p = عدم;
	اكسر;
      حالة MULTIBYTECODE:
	p = "MULTIBYTECODE";
	اكسر;
      حالة NEWL:
	p = "NEWL";
	اكسر;
      افتراضي:
	buflen += vim_snprintf(buf + buflen, حجم(buf) - buflen, "corrupt %d", OP(op));
	p = عدم;
	اكسر;
    }
    אם (p != عدم)
	vim_strncpy((char_u *)buf + buflen, (char_u *)p, حجم(buf) - buflen - 1);
    החזר (char_u *)buf;
}
#endif	    // DEBUG
