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

સ્થિર પૂર્ણાંક	અંક_complex_braces; // Complex \{...} count
સ્થિર char_u	*regcode;	// Code-emit pointer, or JUST_CALC_SIZE
સ્થિર long	regsize;	// Code size.
સ્થિર પૂર્ણાંક	reg_toolong;	// TRUE when offset out of range
સ્થિર પૂર્ણાંક	bt_reg_parse_depth;	// nesting depth in reg()
સ્થિર char_u	had_endbrace[NSUBEXP];	// flags, TRUE if end of () found
સ્થિર long	brace_min[10];	// Minimums for complex brace repeats
સ્થિર long	brace_max[10];	// Maximums for complex brace repeats
સ્થિર પૂર્ણાંક	brace_ગણતરી[10]; // Current counts for complex brace repeats
સ્થિર પૂર્ણાંક	one_exactly = FALSE;	// only do one char for EXACTLY

// When making changes to classchars also change nfa_classcodes.
સ્થિર char_u	*classchars = (char_u *)".iIkKfFpPsSdDxXoOwWhHaAlLuU";
સ્થિર અચલ પૂર્ણાંક classcodes[] = {
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
    સ્થિર પૂર્ણાંક
bt_recognize_char_class(char_u *start, char_u *end)
{
    પૂર્ણાંક		newl;
    પૂર્ણાંક		config = get_char_class_bits(start, end, &newl);

    // A newline would need the ADD_NL variant, don't bother with it here.
    જો (config < 0 || newl)
	પાછા 0;

	// The old engine has no case-insensitive class opcode, so [a-z] and [A-Z]
	// are left as collections.  The classes below are case-independent.
    પસંદગી (config)
    {
	સ્થિતિ CLASS_o9:
	    પાછા DIGIT;
	સ્થિતિ CLASS_not | CLASS_o9:
	    પાછા NDIGIT;
	સ્થિતિ CLASS_af | CLASS_AF | CLASS_o9:
	    પાછા HEX;
	સ્થિતિ CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    પાછા NHEX;
	સ્થિતિ CLASS_o7:
	    પાછા OCTAL;
	સ્થિતિ CLASS_not | CLASS_o7:
	    પાછા NOCTAL;
	સ્થિતિ CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    પાછા WORD;
	સ્થિતિ CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    પાછા NWORD;
	સ્થિતિ CLASS_az | CLASS_AZ | CLASS_underscore:
	    પાછા HEAD;
	સ્થિતિ CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    પાછા NHEAD;
	સ્થિતિ CLASS_az | CLASS_AZ:
	    પાછા ALPHA;
	સ્થિતિ CLASS_not | CLASS_az | CLASS_AZ:
	    પાછા NALPHA;
    }
    પાછા 0;
}

/*
 * When regcode is set to this value, code is not emitted and size is computed
 * instead.
 */
#define JUST_CALC_SIZE	((char_u *) -1)

// Values for rs_state in regitem_T.
પ્રકારવ્યાખ્યા ગણના regstate_E
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
પ્રકારવ્યાખ્યા સંરચના
{
    સંઘ
    {
	char_u	*ptr;	// rex.input pointer, for single-line regexp
	lpos_T	pos;	// rex.input pos, for multi-line regexp
    } rs_u;
    પૂર્ણાંક		rs_len;
} regsave_T;

// struct to save start/end pointer/position in for \(\)
પ્રકારવ્યાખ્યા સંરચના
{
    સંઘ
    {
	char_u	*ptr;
	lpos_T	pos;
    } se_u;
} save_se_T;

// used for BEHIND and NOBEHIND matching
પ્રકારવ્યાખ્યા સંરચના regbehind_S
{
    regsave_T	save_after;
    regsave_T	save_behind;
    પૂર્ણાંક		save_need_clear_subexpr;
    save_se_T   save_start[NSUBEXP];
    save_se_T   save_end[NSUBEXP];
} regbehind_T;

/*
 * When there are alternatives a regstate_T is put on the regstack to remember
 * what we are doing.
 * Before it may be another type of item, depending on rs_state, to remember
 * more things.
 */
પ્રકારવ્યાખ્યા સંરચના regitem_S
{
    regstate_T	rs_state;	// what we are doing, one of RS_ above
    short	rs_no;		// submatch nr or BEHIND/NOBEHIND
    char_u	*rs_scan;	// current node in program
    સંઘ
    {
	save_se_T  sesave;
	regsave_T  regsave;
    } rs_un;			// room for saving rex.input
} regitem_T;


// used for STAR, PLUS and BRACE_SIMPLE matching
પ્રકારવ્યાખ્યા સંરચના regstar_S
{
    પૂર્ણાંક		nextb;		// next byte
    પૂર્ણાંક		nextb_ic;	// next byte reverse case
    long	ગણતરી;
    long	minval;
    long	maxval;
} regstar_T;

// used to store input position when a BACK was encountered, so that we now if
// we made any progress since the last time.
પ્રકારવ્યાખ્યા સંરચના backpos_S
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
સ્થિર garray_T	regstack = {0, 0, 0, 0, ખાલી};
સ્થિર garray_T	backpos = {0, 0, 0, 0, ખાલી};

સ્થિર regsave_T behind_pos;

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
#define OP(p)		((પૂર્ણાંક)*(p))
#define NEXT(p)		(((*((p) + 1) & 0377) << 8) + (*((p) + 2) & 0377))
#define OPERAND(p)	((p) + 3)
// Obtain an operand that was stored as four bytes, MSB first.
#define OPERAND_MIN(p)	(((long)(p)[3] << 24) + ((long)(p)[4] << 16) \
			+ ((long)(p)[5] << 8) + (long)(p)[6])
// Obtain a second operand stored as four bytes.
#define OPERAND_MAX(p)	OPERAND_MIN((p) + 4)
// Obtain a second single-byte operand stored after a four bytes operand.
#define OPERAND_CMP(p)	(p)[7]

સ્થિર char_u *reg(પૂર્ણાંક paren, પૂર્ણાંક *flagp);

#ifdef BT_REGEXP_DUMP
સ્થિર શૂન્ય	regdump(char_u *, bt_regprog_T *);
#endif

સ્થિર પૂર્ણાંક	re_અંક_cmp(long_u val, char_u *scan);

#ifdef DEBUG
સ્થિર char_u	*regprop(char_u *);

સ્થિર પૂર્ણાંક	regnarrate = 0;
#endif


/*
 * Setup to parse the regexp.  Used once to get the length and once to do it.
 */
    સ્થિર શૂન્ય
regcomp_start(
    char_u	*પદાવલિ,
    પૂર્ણાંક		re_flags)	    // see vim_regcomp()
{
    initchr(પદાવલિ);
    જો (re_flags & RE_MAGIC)
	reg_magic = MAGIC_ON;
    નહીંતર
	reg_magic = MAGIC_OFF;
    reg_string = (re_flags & RE_STRING);
    reg_strict = (re_flags & RE_STRICT);
    get_cpo_flags();

    અંક_complex_braces = 0;
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
#જો defined(FEAT_SYN_HL)
    had_eol = FALSE;
#endif
}

/*
 * Return TRUE if MULTIBYTECODE should be used instead of EXACTLY for
 * character "c".
 */
    સ્થિર પૂર્ણાંક
use_multibytecode(પૂર્ણાંક c)
{
    પાછા has_mbyte && (*mb_char2len)(c) > 1
		     && (re_multi_type(peekchr()) != NOT_MULTI
			     || (enc_utf8 && utf_iscomposing(c)));
}

/*
 * Emit (if appropriate) a byte of code
 */
    સ્થિર શૂન્ય
regc(પૂર્ણાંક b)
{
    જો (regcode == JUST_CALC_SIZE)
	regsize++;
    નહીંતર
	*regcode++ = b;
}

/*
 * Emit (if appropriate) a multi-byte character of code
 */
    સ્થિર શૂન્ય
regmbc(પૂર્ણાંક c)
{
    જો (!has_mbyte && c > 0xff)
	પાછા;
    જો (regcode == JUST_CALC_SIZE)
	regsize += (*mb_char2len)(c);
    નહીંતર
	regcode += (*mb_char2bytes)(c, regcode);
}


/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * NOTE: When changing this function, also change nfa_emit_equi_class()
 */
    સ્થિર શૂન્ય
reg_equi_class(પૂર્ણાંક c)
{
    જો (enc_utf8 || STRCMP(p_enc, "latin1") == 0
					 || STRCMP(p_enc, "iso-8859-15") == 0)
    {
	પસંદગી (c)
	{
	    // Do not use '\300' style, it results in a negative number.
	    સ્થિતિ 'A': સ્થિતિ 0xc0: સ્થિતિ 0xc1: સ્થિતિ 0xc2: સ્થિતિ 0xc3: સ્થિતિ 0xc4:
	    સ્થિતિ 0xc5: સ્થિતિ 0x100: સ્થિતિ 0x102: સ્થિતિ 0x104: સ્થિતિ 0x1cd:
	    સ્થિતિ 0x1de: સ્થિતિ 0x1e0: સ્થિતિ 0x1fa: સ્થિતિ 0x202: સ્થિતિ 0x226:
	    સ્થિતિ 0x23a: સ્થિતિ 0x1e00: સ્થિતિ 0x1ea0: સ્થિતિ 0x1ea2: સ્થિતિ 0x1ea4:
	    સ્થિતિ 0x1ea6: સ્થિતિ 0x1ea8: સ્થિતિ 0x1eaa: સ્થિતિ 0x1eac: સ્થિતિ 0x1eae:
	    સ્થિતિ 0x1eb0: સ્થિતિ 0x1eb2: સ્થિતિ 0x1eb4: સ્થિતિ 0x1eb6:
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
		      પાછા;
	    સ્થિતિ 'B': સ્થિતિ 0x181: સ્થિતિ 0x243: સ્થિતિ 0x1e02:
	    સ્થિતિ 0x1e04: સ્થિતિ 0x1e06:
		      regmbc('B');
		      regmbc(0x181); regmbc(0x243); regmbc(0x1e02);
		      regmbc(0x1e04); regmbc(0x1e06);
		      પાછા;
	    સ્થિતિ 'C': સ્થિતિ 0xc7:
	    સ્થિતિ 0x106: સ્થિતિ 0x108: સ્થિતિ 0x10a: સ્થિતિ 0x10c: સ્થિતિ 0x187:
	    સ્થિતિ 0x23b: સ્થિતિ 0x1e08: સ્થિતિ 0xa792:
		      regmbc('C'); regmbc(0xc7);
		      regmbc(0x106); regmbc(0x108); regmbc(0x10a);
		      regmbc(0x10c); regmbc(0x187); regmbc(0x23b);
		      regmbc(0x1e08); regmbc(0xa792);
		      પાછા;
	    સ્થિતિ 'D': સ્થિતિ 0x10e: સ્થિતિ 0x110: સ્થિતિ 0x18a:
	    સ્થિતિ 0x1e0a: સ્થિતિ 0x1e0c: સ્થિતિ 0x1e0e: સ્થિતિ 0x1e10:
	    સ્થિતિ 0x1e12:
		      regmbc('D'); regmbc(0x10e); regmbc(0x110);
		      regmbc(0x18a); regmbc(0x1e0a); regmbc(0x1e0c);
		      regmbc(0x1e0e); regmbc(0x1e10); regmbc(0x1e12);
		      પાછા;
	    સ્થિતિ 'E': સ્થિતિ 0xc8: સ્થિતિ 0xc9: સ્થિતિ 0xca: સ્થિતિ 0xcb:
	    સ્થિતિ 0x112: સ્થિતિ 0x114: સ્થિતિ 0x116: સ્થિતિ 0x118: સ્થિતિ 0x11a:
	    સ્થિતિ 0x204: સ્થિતિ 0x206: સ્થિતિ 0x228: સ્થિતિ 0x246: સ્થિતિ 0x1e14:
	    સ્થિતિ 0x1e16: સ્થિતિ 0x1e18: સ્થિતિ 0x1e1a: સ્થિતિ 0x1e1c:
	    સ્થિતિ 0x1eb8: સ્થિતિ 0x1eba: સ્થિતિ 0x1ebc: સ્થિતિ 0x1ebe:
	    સ્થિતિ 0x1ec0: સ્થિતિ 0x1ec2: સ્થિતિ 0x1ec4: સ્થિતિ 0x1ec6:
		      regmbc('E'); regmbc(0xc8); regmbc(0xc9);
		      regmbc(0xca); regmbc(0xcb); regmbc(0x112);
		      regmbc(0x114); regmbc(0x116); regmbc(0x118);
		      regmbc(0x11a); regmbc(0x204); regmbc(0x206);
		      regmbc(0x228); regmbc(0x246); regmbc(0x1e14);
		      regmbc(0x1e16); regmbc(0x1e18); regmbc(0x1e1a);
		      regmbc(0x1e1c); regmbc(0x1eb8); regmbc(0x1eba);
		      regmbc(0x1ebc); regmbc(0x1ebe); regmbc(0x1ec0);
		      regmbc(0x1ec2); regmbc(0x1ec4); regmbc(0x1ec6);
		      પાછા;
	    સ્થિતિ 'F': સ્થિતિ 0x191: સ્થિતિ 0x1e1e: સ્થિતિ 0xa798:
		      regmbc('F'); regmbc(0x191); regmbc(0x1e1e);
		      regmbc(0xa798);
		      પાછા;
	    સ્થિતિ 'G': સ્થિતિ 0x11c: સ્થિતિ 0x11e: સ્થિતિ 0x120:
	    સ્થિતિ 0x122: સ્થિતિ 0x193: સ્થિતિ 0x1e4: સ્થિતિ 0x1e6:
	    સ્થિતિ 0x1f4: સ્થિતિ 0x1e20: સ્થિતિ 0xa7a0:
		      regmbc('G'); regmbc(0x11c); regmbc(0x11e);
		      regmbc(0x120); regmbc(0x122); regmbc(0x193);
		      regmbc(0x1e4); regmbc(0x1e6); regmbc(0x1f4);
		      regmbc(0x1e20); regmbc(0xa7a0);
		      પાછા;
	    સ્થિતિ 'H': સ્થિતિ 0x124: સ્થિતિ 0x126: સ્થિતિ 0x21e:
	    સ્થિતિ 0x1e22: સ્થિતિ 0x1e24: સ્થિતિ 0x1e26:
	    સ્થિતિ 0x1e28: સ્થિતિ 0x1e2a: સ્થિતિ 0x2c67:
		      regmbc('H'); regmbc(0x124); regmbc(0x126);
		      regmbc(0x21e); regmbc(0x1e22); regmbc(0x1e24);
		      regmbc(0x1e26); regmbc(0x1e28); regmbc(0x1e2a);
		      regmbc(0x2c67);
		      પાછા;
	    સ્થિતિ 'I': સ્થિતિ 0xcc: સ્થિતિ 0xcd: સ્થિતિ 0xce: સ્થિતિ 0xcf:
	    સ્થિતિ 0x128: સ્થિતિ 0x12a: સ્થિતિ 0x12c: સ્થિતિ 0x12e:
	    સ્થિતિ 0x130: સ્થિતિ 0x197: સ્થિતિ 0x1cf: સ્થિતિ 0x208:
	    સ્થિતિ 0x20a: સ્થિતિ 0x1e2c: સ્થિતિ 0x1e2e: સ્થિતિ 0x1ec8:
	    સ્થિતિ 0x1eca:
		      regmbc('I'); regmbc(0xcc); regmbc(0xcd);
		      regmbc(0xce); regmbc(0xcf); regmbc(0x128);
		      regmbc(0x12a); regmbc(0x12c); regmbc(0x12e);
		      regmbc(0x130); regmbc(0x197); regmbc(0x1cf);
		      regmbc(0x208); regmbc(0x20a); regmbc(0x1e2c);
		      regmbc(0x1e2e); regmbc(0x1ec8); regmbc(0x1eca);
		      પાછા;
	    સ્થિતિ 'J': સ્થિતિ 0x134: સ્થિતિ 0x248:
		      regmbc('J'); regmbc(0x134); regmbc(0x248);
		      પાછા;
	    સ્થિતિ 'K': સ્થિતિ 0x136: સ્થિતિ 0x198: સ્થિતિ 0x1e8: સ્થિતિ 0x1e30:
	    સ્થિતિ 0x1e32: સ્થિતિ 0x1e34: સ્થિતિ 0x2c69: સ્થિતિ 0xa740:
		      regmbc('K'); regmbc(0x136); regmbc(0x198);
		      regmbc(0x1e8); regmbc(0x1e30); regmbc(0x1e32);
		      regmbc(0x1e34); regmbc(0x2c69); regmbc(0xa740);
		      પાછા;
	    સ્થિતિ 'L': સ્થિતિ 0x139: સ્થિતિ 0x13b: સ્થિતિ 0x13d: સ્થિતિ 0x13f:
	    સ્થિતિ 0x141: સ્થિતિ 0x23d: સ્થિતિ 0x1e36: સ્થિતિ 0x1e38:
	    સ્થિતિ 0x1e3a: સ્થિતિ 0x1e3c: સ્થિતિ 0x2c60:
		      regmbc('L'); regmbc(0x139); regmbc(0x13b);
		      regmbc(0x13d); regmbc(0x13f); regmbc(0x141);
		      regmbc(0x23d); regmbc(0x1e36); regmbc(0x1e38);
		      regmbc(0x1e3a); regmbc(0x1e3c); regmbc(0x2c60);
		      પાછા;
	    સ્થિતિ 'M': સ્થિતિ 0x1e3e: સ્થિતિ 0x1e40: સ્થિતિ 0x1e42:
		      regmbc('M'); regmbc(0x1e3e); regmbc(0x1e40);
		      regmbc(0x1e42);
		      પાછા;
	    સ્થિતિ 'N': સ્થિતિ 0xd1:
	    સ્થિતિ 0x143: સ્થિતિ 0x145: સ્થિતિ 0x147: સ્થિતિ 0x1f8:
	    સ્થિતિ 0x1e44: સ્થિતિ 0x1e46: સ્થિતિ 0x1e48: સ્થિતિ 0x1e4a:
	    સ્થિતિ 0xa7a4:
		      regmbc('N'); regmbc(0xd1);
		      regmbc(0x143); regmbc(0x145); regmbc(0x147);
		      regmbc(0x1f8); regmbc(0x1e44); regmbc(0x1e46);
		      regmbc(0x1e48); regmbc(0x1e4a); regmbc(0xa7a4);
		      પાછા;
	    સ્થિતિ 'O': સ્થિતિ 0xd2: સ્થિતિ 0xd3: સ્થિતિ 0xd4: સ્થિતિ 0xd5: સ્થિતિ 0xd6:
	    સ્થિતિ 0xd8: સ્થિતિ 0x14c: સ્થિતિ 0x14e: સ્થિતિ 0x150: સ્થિતિ 0x19f:
	    સ્થિતિ 0x1a0: સ્થિતિ 0x1d1: સ્થિતિ 0x1ea: સ્થિતિ 0x1ec: સ્થિતિ 0x1fe:
	    સ્થિતિ 0x20c: સ્થિતિ 0x20e: સ્થિતિ 0x22a: સ્થિતિ 0x22c: સ્થિતિ 0x22e:
	    સ્થિતિ 0x230: સ્થિતિ 0x1e4c: સ્થિતિ 0x1e4e: સ્થિતિ 0x1e50: સ્થિતિ 0x1e52:
	    સ્થિતિ 0x1ecc: સ્થિતિ 0x1ece: સ્થિતિ 0x1ed0: સ્થિતિ 0x1ed2: સ્થિતિ 0x1ed4:
	    સ્થિતિ 0x1ed6: સ્થિતિ 0x1ed8: સ્થિતિ 0x1eda: સ્થિતિ 0x1edc: સ્થિતિ 0x1ede:
	    સ્થિતિ 0x1ee0: સ્થિતિ 0x1ee2:
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
		      પાછા;
	    સ્થિતિ 'P': સ્થિતિ 0x1a4: સ્થિતિ 0x1e54: સ્થિતિ 0x1e56: સ્થિતિ 0x2c63:
		      regmbc('P'); regmbc(0x1a4); regmbc(0x1e54);
		      regmbc(0x1e56); regmbc(0x2c63);
		      પાછા;
	    સ્થિતિ 'Q': સ્થિતિ 0x24a:
		      regmbc('Q'); regmbc(0x24a);
		      પાછા;
	    સ્થિતિ 'R': સ્થિતિ 0x154: સ્થિતિ 0x156: સ્થિતિ 0x158: સ્થિતિ 0x210:
	    સ્થિતિ 0x212: સ્થિતિ 0x24c: સ્થિતિ 0x1e58: સ્થિતિ 0x1e5a:
	    સ્થિતિ 0x1e5c: સ્થિતિ 0x1e5e: સ્થિતિ 0x2c64: સ્થિતિ 0xa7a6:
		      regmbc('R'); regmbc(0x154); regmbc(0x156);
		      regmbc(0x210); regmbc(0x212); regmbc(0x158);
		      regmbc(0x24c); regmbc(0x1e58); regmbc(0x1e5a);
		      regmbc(0x1e5c); regmbc(0x1e5e); regmbc(0x2c64);
		      regmbc(0xa7a6);
		      પાછા;
	    સ્થિતિ 'S': સ્થિતિ 0x15a: સ્થિતિ 0x15c: સ્થિતિ 0x15e: સ્થિતિ 0x160:
	    સ્થિતિ 0x218: સ્થિતિ 0x1e60: સ્થિતિ 0x1e62: સ્થિતિ 0x1e64:
	    સ્થિતિ 0x1e66: સ્થિતિ 0x1e68: સ્થિતિ 0x2c7e: સ્થિતિ 0xa7a8:
		      regmbc('S'); regmbc(0x15a); regmbc(0x15c);
		      regmbc(0x15e); regmbc(0x160); regmbc(0x218);
		      regmbc(0x1e60); regmbc(0x1e62); regmbc(0x1e64);
		      regmbc(0x1e66); regmbc(0x1e68); regmbc(0x2c7e);
			  regmbc(0xa7a8);
		      પાછા;
	    સ્થિતિ 'T': સ્થિતિ 0x162: સ્થિતિ 0x164: સ્થિતિ 0x166: સ્થિતિ 0x1ac:
	    સ્થિતિ 0x1ae: સ્થિતિ 0x21a: સ્થિતિ 0x23e: સ્થિતિ 0x1e6a: સ્થિતિ 0x1e6c:
	    સ્થિતિ 0x1e6e: સ્થિતિ 0x1e70:
		      regmbc('T'); regmbc(0x162); regmbc(0x164);
		      regmbc(0x166); regmbc(0x1ac); regmbc(0x23e);
		      regmbc(0x1ae); regmbc(0x21a); regmbc(0x1e6a);
		      regmbc(0x1e6c); regmbc(0x1e6e); regmbc(0x1e70);
		      પાછા;
	    સ્થિતિ 'U': સ્થિતિ 0xd9: સ્થિતિ 0xda: સ્થિતિ 0xdb: સ્થિતિ 0xdc:
	    સ્થિતિ 0x168: સ્થિતિ 0x16a: સ્થિતિ 0x16c: સ્થિતિ 0x16e:
	    સ્થિતિ 0x170: સ્થિતિ 0x172: સ્થિતિ 0x1af: સ્થિતિ 0x1d3:
	    સ્થિતિ 0x1d5: સ્થિતિ 0x1d7: સ્થિતિ 0x1d9: સ્થિતિ 0x1db:
	    સ્થિતિ 0x214: સ્થિતિ 0x216: સ્થિતિ 0x244: સ્થિતિ 0x1e72:
	    સ્થિતિ 0x1e74: સ્થિતિ 0x1e76: સ્થિતિ 0x1e78: સ્થિતિ 0x1e7a:
	    સ્થિતિ 0x1ee4: સ્થિતિ 0x1ee6: સ્થિતિ 0x1ee8: સ્થિતિ 0x1eea:
	    સ્થિતિ 0x1eec: સ્થિતિ 0x1eee: સ્થિતિ 0x1ef0:
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
		      પાછા;
	    સ્થિતિ 'V': સ્થિતિ 0x1b2: સ્થિતિ 0x1e7c: સ્થિતિ 0x1e7e:
		      regmbc('V'); regmbc(0x1b2); regmbc(0x1e7c);
		      regmbc(0x1e7e);
		      પાછા;
	    સ્થિતિ 'W': સ્થિતિ 0x174: સ્થિતિ 0x1e80: સ્થિતિ 0x1e82:
	    સ્થિતિ 0x1e84: સ્થિતિ 0x1e86: સ્થિતિ 0x1e88:
		      regmbc('W'); regmbc(0x174); regmbc(0x1e80);
		      regmbc(0x1e82); regmbc(0x1e84); regmbc(0x1e86);
		      regmbc(0x1e88);
		      પાછા;
	    સ્થિતિ 'X': સ્થિતિ 0x1e8a: સ્થિતિ 0x1e8c:
		      regmbc('X'); regmbc(0x1e8a); regmbc(0x1e8c);
		      પાછા;
	    સ્થિતિ 'Y': સ્થિતિ 0xdd:
	    સ્થિતિ 0x176: સ્થિતિ 0x178: સ્થિતિ 0x1b3: સ્થિતિ 0x232: સ્થિતિ 0x24e:
	    સ્થિતિ 0x1e8e: સ્થિતિ 0x1ef2: સ્થિતિ 0x1ef6: સ્થિતિ 0x1ef4: સ્થિતિ 0x1ef8:
		      regmbc('Y'); regmbc(0xdd); regmbc(0x176);
		      regmbc(0x178); regmbc(0x1b3); regmbc(0x232);
		      regmbc(0x24e); regmbc(0x1e8e); regmbc(0x1ef2);
		      regmbc(0x1ef4); regmbc(0x1ef6); regmbc(0x1ef8);
		      પાછા;
	    સ્થિતિ 'Z': સ્થિતિ 0x179: સ્થિતિ 0x17b: સ્થિતિ 0x17d: સ્થિતિ 0x1b5:
	    સ્થિતિ 0x1e90: સ્થિતિ 0x1e92: સ્થિતિ 0x1e94: સ્થિતિ 0x2c6b:
		      regmbc('Z'); regmbc(0x179); regmbc(0x17b);
		      regmbc(0x17d); regmbc(0x1b5); regmbc(0x1e90);
		      regmbc(0x1e92); regmbc(0x1e94); regmbc(0x2c6b);
		      પાછા;
	    સ્થિતિ 'a': સ્થિતિ 0xe0: સ્થિતિ 0xe1: સ્થિતિ 0xe2:
	    સ્થિતિ 0xe3: સ્થિતિ 0xe4: સ્થિતિ 0xe5: સ્થિતિ 0x101: સ્થિતિ 0x103:
	    સ્થિતિ 0x105: સ્થિતિ 0x1ce: સ્થિતિ 0x1df: સ્થિતિ 0x1e1: સ્થિતિ 0x1fb:
	    સ્થિતિ 0x201: સ્થિતિ 0x203: સ્થિતિ 0x227: સ્થિતિ 0x1d8f: સ્થિતિ 0x1e01:
	    સ્થિતિ 0x1e9a: સ્થિતિ 0x1ea1: સ્થિતિ 0x1ea3: સ્થિતિ 0x1ea5:
	    સ્થિતિ 0x1ea7: સ્થિતિ 0x1ea9: સ્થિતિ 0x1eab: સ્થિતિ 0x1ead:
	    સ્થિતિ 0x1eaf: સ્થિતિ 0x1eb1: સ્થિતિ 0x1eb3: સ્થિતિ 0x1eb5:
	    સ્થિતિ 0x1eb7: સ્થિતિ 0x2c65:
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
		      પાછા;
	    સ્થિતિ 'b': સ્થિતિ 0x180: સ્થિતિ 0x253: સ્થિતિ 0x1d6c: સ્થિતિ 0x1d80:
	    સ્થિતિ 0x1e03: સ્થિતિ 0x1e05: સ્થિતિ 0x1e07:
		      regmbc('b');
		      regmbc(0x180); regmbc(0x253); regmbc(0x1d6c);
		      regmbc(0x1d80); regmbc(0x1e03); regmbc(0x1e05);
		      regmbc(0x1e07);
		      પાછા;
	    સ્થિતિ 'c': સ્થિતિ 0xe7:
	    સ્થિતિ 0x107: સ્થિતિ 0x109: સ્થિતિ 0x10b: સ્થિતિ 0x10d: સ્થિતિ 0x188:
	    સ્થિતિ 0x23c: સ્થિતિ 0x1e09: સ્થિતિ 0xa793: સ્થિતિ 0xa794:
		      regmbc('c'); regmbc(0xe7); regmbc(0x107);
		      regmbc(0x109); regmbc(0x10b); regmbc(0x10d);
		      regmbc(0x188); regmbc(0x23c); regmbc(0x1e09);
		      regmbc(0xa793); regmbc(0xa794);
		      પાછા;
	    સ્થિતિ 'd': સ્થિતિ 0x10f: સ્થિતિ 0x111: સ્થિતિ 0x257: સ્થિતિ 0x1d6d:
	    સ્થિતિ 0x1d81: સ્થિતિ 0x1d91: સ્થિતિ 0x1e0b: સ્થિતિ 0x1e0d:
	    સ્થિતિ 0x1e0f: સ્થિતિ 0x1e11: સ્થિતિ 0x1e13:
		      regmbc('d'); regmbc(0x10f); regmbc(0x111);
		      regmbc(0x257); regmbc(0x1d6d); regmbc(0x1d81);
		      regmbc(0x1d91); regmbc(0x1e0b); regmbc(0x1e0d);
		      regmbc(0x1e0f); regmbc(0x1e11); regmbc(0x1e13);
		      પાછા;
	    સ્થિતિ 'e': સ્થિતિ 0xe8: સ્થિતિ 0xe9: સ્થિતિ 0xea: સ્થિતિ 0xeb:
	    સ્થિતિ 0x113: સ્થિતિ 0x115: સ્થિતિ 0x117: સ્થિતિ 0x119:
	    સ્થિતિ 0x11b: સ્થિતિ 0x205: સ્થિતિ 0x207: સ્થિતિ 0x229:
	    સ્થિતિ 0x247: સ્થિતિ 0x1d92: સ્થિતિ 0x1e15: સ્થિતિ 0x1e17:
	    સ્થિતિ 0x1e19: સ્થિતિ 0x1e1b: સ્થિતિ 0x1eb9: સ્થિતિ 0x1ebb:
	    સ્થિતિ 0x1e1d: સ્થિતિ 0x1ebd: સ્થિતિ 0x1ebf: સ્થિતિ 0x1ec1:
	    સ્થિતિ 0x1ec3: સ્થિતિ 0x1ec5: સ્થિતિ 0x1ec7:
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
		      પાછા;
	    સ્થિતિ 'f': સ્થિતિ 0x192: સ્થિતિ 0x1d6e: સ્થિતિ 0x1d82:
	    સ્થિતિ 0x1e1f: સ્થિતિ 0xa799:
		     regmbc('f'); regmbc(0x192); regmbc(0x1d6e);
		     regmbc(0x1d82); regmbc(0x1e1f); regmbc(0xa799);
		     પાછા;
	    સ્થિતિ 'g': સ્થિતિ 0x11d: સ્થિતિ 0x11f: સ્થિતિ 0x121: સ્થિતિ 0x123:
	    સ્થિતિ 0x1e5: સ્થિતિ 0x1e7: સ્થિતિ 0x260: સ્થિતિ 0x1f5: સ્થિતિ 0x1d83:
	    સ્થિતિ 0x1e21: સ્થિતિ 0xa7a1:
		      regmbc('g'); regmbc(0x11d); regmbc(0x11f);
		      regmbc(0x121); regmbc(0x123); regmbc(0x1e5);
		      regmbc(0x1e7); regmbc(0x1f5); regmbc(0x260);
		      regmbc(0x1d83); regmbc(0x1e21); regmbc(0xa7a1);
		      પાછા;
	    સ્થિતિ 'h': સ્થિતિ 0x125: સ્થિતિ 0x127: સ્થિતિ 0x21f: સ્થિતિ 0x1e23:
	    સ્થિતિ 0x1e25: સ્થિતિ 0x1e27: સ્થિતિ 0x1e29: સ્થિતિ 0x1e2b:
	    સ્થિતિ 0x1e96: સ્થિતિ 0x2c68: સ્થિતિ 0xa795:
		      regmbc('h'); regmbc(0x125); regmbc(0x127);
		      regmbc(0x21f); regmbc(0x1e23); regmbc(0x1e25);
		      regmbc(0x1e27); regmbc(0x1e29); regmbc(0x1e2b);
		      regmbc(0x1e96); regmbc(0x2c68); regmbc(0xa795);
		      પાછા;
	    સ્થિતિ 'i': સ્થિતિ 0xec: સ્થિતિ 0xed: સ્થિતિ 0xee: સ્થિતિ 0xef:
	    સ્થિતિ 0x129: સ્થિતિ 0x12b: સ્થિતિ 0x12d: સ્થિતિ 0x12f:
	    સ્થિતિ 0x1d0: સ્થિતિ 0x209: સ્થિતિ 0x20b: સ્થિતિ 0x268:
	    સ્થિતિ 0x1d96: સ્થિતિ 0x1e2d: સ્થિતિ 0x1e2f: સ્થિતિ 0x1ec9:
	    સ્થિતિ 0x1ecb:
		      regmbc('i'); regmbc(0xec); regmbc(0xed);
		      regmbc(0xee); regmbc(0xef); regmbc(0x129);
		      regmbc(0x12b); regmbc(0x12d); regmbc(0x12f);
		      regmbc(0x1d0); regmbc(0x209); regmbc(0x20b);
		      regmbc(0x268); regmbc(0x1d96); regmbc(0x1e2d);
		      regmbc(0x1e2f); regmbc(0x1ec9); regmbc(0x1ecb);
		      પાછા;
	    સ્થિતિ 'j': સ્થિતિ 0x135: સ્થિતિ 0x1f0: સ્થિતિ 0x249:
		      regmbc('j'); regmbc(0x135); regmbc(0x1f0);
		      regmbc(0x249);
		      પાછા;
	    સ્થિતિ 'k': સ્થિતિ 0x137: સ્થિતિ 0x199: સ્થિતિ 0x1e9:
	    સ્થિતિ 0x1d84: સ્થિતિ 0x1e31: સ્થિતિ 0x1e33: સ્થિતિ 0x1e35:
	    સ્થિતિ 0x2c6a: સ્થિતિ 0xa741:
		      regmbc('k'); regmbc(0x137); regmbc(0x199);
		      regmbc(0x1e9); regmbc(0x1d84); regmbc(0x1e31);
		      regmbc(0x1e33); regmbc(0x1e35); regmbc(0x2c6a);
		      regmbc(0xa741);
		      પાછા;
	    સ્થિતિ 'l': સ્થિતિ 0x13a: સ્થિતિ 0x13c: સ્થિતિ 0x13e:
	    સ્થિતિ 0x140: સ્થિતિ 0x142: સ્થિતિ 0x19a: સ્થિતિ 0x1e37:
	    સ્થિતિ 0x1e39: સ્થિતિ 0x1e3b: સ્થિતિ 0x1e3d: સ્થિતિ 0x2c61:
		      regmbc('l'); regmbc(0x13a); regmbc(0x13c);
		      regmbc(0x13e); regmbc(0x140); regmbc(0x142);
		      regmbc(0x19a); regmbc(0x1e37); regmbc(0x1e39);
		      regmbc(0x1e3b); regmbc(0x1e3d); regmbc(0x2c61);
		      પાછા;
	    સ્થિતિ 'm': સ્થિતિ 0x1d6f: સ્થિતિ 0x1e3f: સ્થિતિ 0x1e41: સ્થિતિ 0x1e43:
		      regmbc('m'); regmbc(0x1d6f); regmbc(0x1e3f);
		      regmbc(0x1e41); regmbc(0x1e43);
		      પાછા;
	    સ્થિતિ 'n': સ્થિતિ 0xf1: સ્થિતિ 0x144: સ્થિતિ 0x146: સ્થિતિ 0x148:
	    સ્થિતિ 0x149: સ્થિતિ 0x1f9: સ્થિતિ 0x1d70: સ્થિતિ 0x1d87:
	    સ્થિતિ 0x1e45: સ્થિતિ 0x1e47: સ્થિતિ 0x1e49: સ્થિતિ 0x1e4b:
	    સ્થિતિ 0xa7a5:
		      regmbc('n'); regmbc(0xf1); regmbc(0x144);
		      regmbc(0x146); regmbc(0x148); regmbc(0x149);
		      regmbc(0x1f9); regmbc(0x1d70); regmbc(0x1d87);
		      regmbc(0x1e45); regmbc(0x1e47); regmbc(0x1e49);
		      regmbc(0x1e4b); regmbc(0xa7a5);
		      પાછા;
	    સ્થિતિ 'o': સ્થિતિ 0xf2: સ્થિતિ 0xf3: સ્થિતિ 0xf4: સ્થિતિ 0xf5:
	    સ્થિતિ 0xf6: સ્થિતિ 0xf8: સ્થિતિ 0x14d: સ્થિતિ 0x14f: સ્થિતિ 0x151:
	    સ્થિતિ 0x1a1: સ્થિતિ 0x1d2: સ્થિતિ 0x1eb: સ્થિતિ 0x1ed: સ્થિતિ 0x1ff:
	    સ્થિતિ 0x20d: સ્થિતિ 0x20f: સ્થિતિ 0x22b: સ્થિતિ 0x22d: સ્થિતિ 0x22f:
	    સ્થિતિ 0x231: સ્થિતિ 0x275: સ્થિતિ 0x1e4d: સ્થિતિ 0x1e4f:
	    સ્થિતિ 0x1e51: સ્થિતિ 0x1e53: સ્થિતિ 0x1ecd: સ્થિતિ 0x1ecf:
	    સ્થિતિ 0x1ed1: સ્થિતિ 0x1ed3: સ્થિતિ 0x1ed5: સ્થિતિ 0x1ed7:
	    સ્થિતિ 0x1ed9: સ્થિતિ 0x1edb: સ્થિતિ 0x1edd: સ્થિતિ 0x1edf:
	    સ્થિતિ 0x1ee1: સ્થિતિ 0x1ee3:
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
		      પાછા;
	    સ્થિતિ 'p': સ્થિતિ 0x1a5: સ્થિતિ 0x1d71: સ્થિતિ 0x1d88: સ્થિતિ 0x1d7d:
	    સ્થિતિ 0x1e55: સ્થિતિ 0x1e57:
		      regmbc('p'); regmbc(0x1a5); regmbc(0x1d71);
		      regmbc(0x1d7d); regmbc(0x1d88); regmbc(0x1e55);
		      regmbc(0x1e57);
		      પાછા;
	    સ્થિતિ 'q': સ્થિતિ 0x24b: સ્થિતિ 0x2a0:
		      regmbc('q'); regmbc(0x24b); regmbc(0x2a0);
		      પાછા;
	    સ્થિતિ 'r': સ્થિતિ 0x155: સ્થિતિ 0x157: સ્થિતિ 0x159: સ્થિતિ 0x211:
	    સ્થિતિ 0x213: સ્થિતિ 0x24d: સ્થિતિ 0x27d: સ્થિતિ 0x1d72: સ્થિતિ 0x1d73:
	    સ્થિતિ 0x1d89: સ્થિતિ 0x1e59: સ્થિતિ 0x1e5b: સ્થિતિ 0x1e5d: સ્થિતિ 0x1e5f:
	    સ્થિતિ 0xa7a7:
		      regmbc('r'); regmbc(0x155); regmbc(0x157);
		      regmbc(0x159); regmbc(0x211); regmbc(0x213);
		      regmbc(0x24d); regmbc(0x1d72); regmbc(0x1d73);
		      regmbc(0x1d89); regmbc(0x1e59); regmbc(0x27d);
		      regmbc(0x1e5b); regmbc(0x1e5d); regmbc(0x1e5f);
		      regmbc(0xa7a7);
		      પાછા;
	    સ્થિતિ 's': સ્થિતિ 0x15b: સ્થિતિ 0x15d: સ્થિતિ 0x15f: સ્થિતિ 0x161:
	    સ્થિતિ 0x1e61: સ્થિતિ 0x219: સ્થિતિ 0x23f: સ્થિતિ 0x1d74: સ્થિતિ 0x1d8a:
	    સ્થિતિ 0x1e63: સ્થિતિ 0x1e65: સ્થિતિ 0x1e67: સ્થિતિ 0x1e69: સ્થિતિ 0xa7a9:
		      regmbc('s'); regmbc(0x15b); regmbc(0x15d);
		      regmbc(0x15f); regmbc(0x161); regmbc(0x23f);
		      regmbc(0x219); regmbc(0x1d74); regmbc(0x1d8a);
		      regmbc(0x1e61); regmbc(0x1e63); regmbc(0x1e65);
		      regmbc(0x1e67); regmbc(0x1e69); regmbc(0xa7a9);
		      પાછા;
	    સ્થિતિ 't': સ્થિતિ 0x163: સ્થિતિ 0x165: સ્થિતિ 0x167: સ્થિતિ 0x1ab:
	    સ્થિતિ 0x1ad: સ્થિતિ 0x21b: સ્થિતિ 0x288: સ્થિતિ 0x1d75: સ્થિતિ 0x1e6b:
	    સ્થિતિ 0x1e6d: સ્થિતિ 0x1e6f: સ્થિતિ 0x1e71: સ્થિતિ 0x1e97: સ્થિતિ 0x2c66:
		      regmbc('t'); regmbc(0x163); regmbc(0x165);
		      regmbc(0x167); regmbc(0x1ab); regmbc(0x21b);
		      regmbc(0x1ad); regmbc(0x288); regmbc(0x1d75);
		      regmbc(0x1e6b); regmbc(0x1e6d); regmbc(0x1e6f);
		      regmbc(0x1e71); regmbc(0x1e97); regmbc(0x2c66);
		      પાછા;
	    સ્થિતિ 'u': સ્થિતિ 0xf9: સ્થિતિ 0xfa: સ્થિતિ 0xfb: સ્થિતિ 0xfc:
	    સ્થિતિ 0x169: સ્થિતિ 0x16b: સ્થિતિ 0x16d: સ્થિતિ 0x16f:
	    સ્થિતિ 0x171: સ્થિતિ 0x173: સ્થિતિ 0x1b0: સ્થિતિ 0x1d4:
	    સ્થિતિ 0x1d6: સ્થિતિ 0x1d8: સ્થિતિ 0x1da: સ્થિતિ 0x1dc:
	    સ્થિતિ 0x215: સ્થિતિ 0x217: સ્થિતિ 0x289: સ્થિતિ 0x1e73:
	    સ્થિતિ 0x1d7e: સ્થિતિ 0x1d99: સ્થિતિ 0x1e75: સ્થિતિ 0x1e77:
	    સ્થિતિ 0x1e79: સ્થિતિ 0x1e7b: સ્થિતિ 0x1ee5: સ્થિતિ 0x1ee7:
	    સ્થિતિ 0x1ee9: સ્થિતિ 0x1eeb: સ્થિતિ 0x1eed: સ્થિતિ 0x1eef:
	    સ્થિતિ 0x1ef1:
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
		      પાછા;
	    સ્થિતિ 'v': સ્થિતિ 0x28b: સ્થિતિ 0x1d8c: સ્થિતિ 0x1e7d: સ્થિતિ 0x1e7f:
		      regmbc('v'); regmbc(0x28b); regmbc(0x1d8c);
		      regmbc(0x1e7d); regmbc(0x1e7f);
		      પાછા;
	    સ્થિતિ 'w': સ્થિતિ 0x175: સ્થિતિ 0x1e81: સ્થિતિ 0x1e83:
	    સ્થિતિ 0x1e85: સ્થિતિ 0x1e87: સ્થિતિ 0x1e89: સ્થિતિ 0x1e98:
		      regmbc('w'); regmbc(0x175); regmbc(0x1e81);
		      regmbc(0x1e83); regmbc(0x1e85); regmbc(0x1e87);
		      regmbc(0x1e89); regmbc(0x1e98);
		      પાછા;
	    સ્થિતિ 'x': સ્થિતિ 0x1e8b: સ્થિતિ 0x1e8d:
		      regmbc('x'); regmbc(0x1e8b); regmbc(0x1e8d);
		      પાછા;
	    સ્થિતિ 'y': સ્થિતિ 0xfd: સ્થિતિ 0xff: સ્થિતિ 0x177: સ્થિતિ 0x1b4:
	    સ્થિતિ 0x233: સ્થિતિ 0x24f: સ્થિતિ 0x1e8f: સ્થિતિ 0x1e99: સ્થિતિ 0x1ef3:
	    સ્થિતિ 0x1ef5: સ્થિતિ 0x1ef7: સ્થિતિ 0x1ef9:
		      regmbc('y'); regmbc(0xfd); regmbc(0xff);
		      regmbc(0x177); regmbc(0x1b4); regmbc(0x233);
		      regmbc(0x24f); regmbc(0x1e8f); regmbc(0x1e99);
		      regmbc(0x1ef3); regmbc(0x1ef5); regmbc(0x1ef7);
		      regmbc(0x1ef9);
		      પાછા;
	    સ્થિતિ 'z': સ્થિતિ 0x17a: સ્થિતિ 0x17c: સ્થિતિ 0x17e: સ્થિતિ 0x1b6:
	    સ્થિતિ 0x1d76: સ્થિતિ 0x1d8e: સ્થિતિ 0x1e91: સ્થિતિ 0x1e93:
	    સ્થિતિ 0x1e95: સ્થિતિ 0x2c6c:
		      regmbc('z'); regmbc(0x17a); regmbc(0x17c);
		      regmbc(0x17e); regmbc(0x1b6); regmbc(0x1d76);
		      regmbc(0x1d8e); regmbc(0x1e91); regmbc(0x1e93);
		      regmbc(0x1e95); regmbc(0x2c6c);
		      પાછા;
	}
    }
    regmbc(c);
}

/*
 * Emit a node.
 * Return pointer to generated code.
 */
    સ્થિર char_u *
regnode(પૂર્ણાંક op)
{
    char_u  *ret;

    ret = regcode;
    જો (ret == JUST_CALC_SIZE)
	regsize += 3;
    નહીંતર
    {
	*regcode++ = op;
	*regcode++ = NUL;		// Null "next" pointer.
	*regcode++ = NUL;
    }
    પાછા ret;
}

/*
 * Write a long as four bytes at "p" and return pointer to the next char.
 */
    સ્થિર char_u *
re_put_long(char_u *p, long_u val)
{
    *p++ = (char_u) ((val >> 24) & 0377);
    *p++ = (char_u) ((val >> 16) & 0377);
    *p++ = (char_u) ((val >> 8) & 0377);
    *p++ = (char_u) (val & 0377);
    પાછા p;
}

/*
 * regnext - dig the "next" pointer out of a node
 * Returns NULL when calculating size, when there is no next item and when
 * there is an error.
 */
    સ્થિર char_u *
regnext(char_u *p)
{
    પૂર્ણાંક	    offset;

    જો (p == JUST_CALC_SIZE || reg_toolong)
	પાછા ખાલી;

    offset = NEXT(p);
    જો (offset == 0)
	પાછા ખાલી;

    જો (OP(p) == BACK)
	પાછા p - offset;
    નહીંતર
	પાછા p + offset;
}

/*
 * Set the next-pointer at the end of a node chain.
 */
    સ્થિર શૂન્ય
regtail(char_u *p, char_u *val)
{
    char_u	*scan;
    char_u	*temp;
    પૂર્ણાંક		offset;

    જો (p == JUST_CALC_SIZE)
	પાછા;

    // Find last node.
    scan = p;
    માટે (;;)
    {
	temp = regnext(scan);
	જો (temp == ખાલી)
	    તોડો;
	scan = temp;
    }

    જો (OP(scan) == BACK)
	offset = (પૂર્ણાંક)(scan - val);
    નહીંતર
	offset = (પૂર્ણાંક)(val - scan);
    // When the offset uses more than 16 bits it can no longer fit in the two
    // bytes available.  Use a global flag to avoid having to check return
    // values in too many places.
    જો (offset > 0xffff)
	reg_toolong = TRUE;
    નહીંતર
    {
	*(scan + 1) = (char_u) (((unsigned)offset >> 8) & 0377);
	*(scan + 2) = (char_u) (offset & 0377);
    }
}

/*
 * Like regtail, on item after a BRANCH; nop if none.
 */
    સ્થિર શૂન્ય
regoptail(char_u *p, char_u *val)
{
    // When op is neither BRANCH nor BRACE_COMPLEX0-9, it is "operandless"
    જો (p == ખાલી || p == JUST_CALC_SIZE
	    || (OP(p) != BRANCH
		&& (OP(p) < BRACE_COMPLEX || OP(p) > BRACE_COMPLEX + 9)))
	પાછા;
    regtail(OPERAND(p), val);
}

/*
 * Insert an operator in front of already-emitted operand
 *
 * Means relocating the operand.
 */
    સ્થિર શૂન્ય
reginsert(પૂર્ણાંક op, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    જો (regcode == JUST_CALC_SIZE)
    {
	regsize += 3;
	પાછા;
    }
    src = regcode;
    regcode += 3;
    dst = regcode;
    જ્યારે (src > opnd)
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
    સ્થિર શૂન્ય
reginsert_nr(પૂર્ણાંક op, long val, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    જો (regcode == JUST_CALC_SIZE)
    {
	regsize += 7;
	પાછા;
    }
    src = regcode;
    regcode += 7;
    dst = regcode;
    જ્યારે (src > opnd)
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
    સ્થિર શૂન્ય
reginsert_limits(
    પૂર્ણાંક		op,
    long	minval,
    long	maxval,
    char_u	*opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    જો (regcode == JUST_CALC_SIZE)
    {
	regsize += 11;
	પાછા;
    }
    src = regcode;
    regcode += 11;
    dst = regcode;
    જ્યારે (src > opnd)
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
    સ્થિર પૂર્ણાંક
seen_endbrace(પૂર્ણાંક refnum)
{
    જો (!had_endbrace[refnum])
    {
	char_u *p;

	// Trick: check if "@<=" or "@<!" follows, in which case
	// the \1 can appear before the referenced match.
	માટે (p = regparse; *p != NUL; ++p)
	    જો (p[0] == '@' && p[1] == '<' && (p[2] == '!' || p[2] == '='))
		તોડો;
	જો (*p == NUL)
	{
	    emsg(_(e_illegal_back_reference));
	    rc_did_emsg = TRUE;
	    પાછા FALSE;
	}
    }
    પાછા TRUE;
}

/*
 * Parse the lowest level.
 *
 * Optimization:  gobbles an entire sequence of ordinary characters so that
 * it can turn them into a single node, which is smaller to store and
 * faster to run.  Don't do this when one_exactly is set.
 */
    સ્થિર char_u *
regatom(પૂર્ણાંક *flagp)
{
    char_u	    *ret;
    પૂર્ણાંક		    flags;
    પૂર્ણાંક		    c;
    char_u	    *p;
    પૂર્ણાંક		    extra = 0;
    પૂર્ણાંક		    save_prev_at_start = prev_at_start;

    *flagp = WORST;		// Tentatively.

    c = getchr();
    પસંદગી (c)
    {
      સ્થિતિ Magic('^'):
	ret = regnode(BOL);
	તોડો;

      સ્થિતિ Magic('$'):
	ret = regnode(EOL);
#જો defined(FEAT_SYN_HL)
	had_eol = TRUE;
#endif
	તોડો;

      સ્થિતિ Magic('<'):
	ret = regnode(BOW);
	તોડો;

      સ્થિતિ Magic('>'):
	ret = regnode(EOW);
	તોડો;

      સ્થિતિ Magic('_'):
	c = no_Magic(getchr());
	જો (c == '^')		// "\_^" is start-of-line
	{
	    ret = regnode(BOL);
	    તોડો;
	}
	જો (c == '$')		// "\_$" is end-of-line
	{
	    ret = regnode(EOL);
#જો defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#endif
	    તોડો;
	}

	extra = ADD_NL;
	*flagp |= HASNL;

	// "\_[" is character range plus newline
	જો (c == '[')
	    જાઓ collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	// Character classes.
      સ્થિતિ Magic('.'):
      સ્થિતિ Magic('i'):
      સ્થિતિ Magic('I'):
      સ્થિતિ Magic('k'):
      સ્થિતિ Magic('K'):
      સ્થિતિ Magic('f'):
      સ્થિતિ Magic('F'):
      સ્થિતિ Magic('p'):
      સ્થિતિ Magic('P'):
      સ્થિતિ Magic('s'):
      સ્થિતિ Magic('S'):
      સ્થિતિ Magic('d'):
      સ્થિતિ Magic('D'):
      સ્થિતિ Magic('x'):
      સ્થિતિ Magic('X'):
      સ્થિતિ Magic('o'):
      સ્થિતિ Magic('O'):
      સ્થિતિ Magic('w'):
      સ્થિતિ Magic('W'):
      સ્થિતિ Magic('h'):
      સ્થિતિ Magic('H'):
      સ્થિતિ Magic('a'):
      સ્થિતિ Magic('A'):
      સ્થિતિ Magic('l'):
      સ્થિતિ Magic('L'):
      સ્થિતિ Magic('u'):
      સ્થિતિ Magic('U'):
	p = vim_strchr(classchars, no_Magic(c));
	જો (p == ખાલી)
	    EMSG_RET_NULL(_(e_invalid_use_of_underscore));

	// When '.' is followed by a composing char ignore the dot, so that
	// the composing char is matched here.
	જો (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	{
	    c = getchr();
	    જાઓ do_multibyte;
	}
	ret = regnode(classcodes[p - classchars] + extra);
	*flagp |= HASWIDTH | SIMPLE;
	તોડો;

      સ્થિતિ Magic('n'):
	જો (reg_string)
	{
	    // In a string "\n" matches a newline character.
	    ret = regnode(EXACTLY);
	    regc(NL);
	    regc(NUL);
	    *flagp |= HASWIDTH | SIMPLE;
	}
	નહીંતર
	{
	    // In buffer text "\n" matches the end of a line.
	    ret = regnode(NEWL);
	    *flagp |= HASWIDTH | HASNL;
	}
	તોડો;

      સ્થિતિ Magic('('):
	જો (one_exactly)
	    EMSG_ONE_RET_NULL;
	ret = reg(REG_PAREN, &flags);
	જો (ret == ખાલી)
	    પાછા ખાલી;
	*flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
	તોડો;

      સ્થિતિ NUL:
      સ્થિતિ Magic('|'):
      સ્થિતિ Magic('&'):
      સ્થિતિ Magic(')'):
	જો (one_exactly)
	    EMSG_ONE_RET_NULL;
	// Supposed to be caught earlier.
	IEMSG_RET_NULL(e_internal_error_in_regexp);
	// NOTREACHED

      સ્થિતિ Magic('='):
      સ્થિતિ Magic('?'):
      સ્થિતિ Magic('+'):
      સ્થિતિ Magic('@'):
      સ્થિતિ Magic('{'):
      સ્થિતિ Magic('*'):
	c = no_Magic(c);
	EMSG3_RET_NULL(_(e_str_chr_follows_nothing),
		(c == '*' ? reg_magic >= MAGIC_ON : reg_magic == MAGIC_ALL), c);
	// NOTREACHED

      સ્થિતિ Magic('~'):		// previous substitute pattern
	    જો (reg_prev_sub != ખાલી)
	    {
		char_u	    *lp;

		ret = regnode(EXACTLY);
		lp = reg_prev_sub;
		જ્યારે (*lp != NUL)
		    regc(*lp++);
		regc(NUL);
		જો (*reg_prev_sub != NUL)
		{
		    *flagp |= HASWIDTH;
		    જો ((lp - reg_prev_sub) == 1)
			*flagp |= SIMPLE;
		}
	    }
	    નહીંતર
		EMSG_RET_NULL(_(e_no_previous_substitute_regular_expression));
	    તોડો;

      સ્થિતિ Magic('1'):
      સ્થિતિ Magic('2'):
      સ્થિતિ Magic('3'):
      સ્થિતિ Magic('4'):
      સ્થિતિ Magic('5'):
      સ્થિતિ Magic('6'):
      સ્થિતિ Magic('7'):
      સ્થિતિ Magic('8'):
      સ્થિતિ Magic('9'):
	    {
		પૂર્ણાંક		    refnum;

		refnum = c - Magic('0');
		જો (!seen_endbrace(refnum))
		    પાછા ખાલી;
		ret = regnode(BACKREF + refnum);
	    }
	    તોડો;

      સ્થિતિ Magic('z'):
	{
	    c = no_Magic(getchr());
	    પસંદગી (c)
	    {
#ifdef FEAT_SYN_HL
		સ્થિતિ '(': જો ((reg_do_extmatch & REX_SET) == 0)
			      EMSG_RET_NULL(_(e_z_not_allowed_here));
			  જો (one_exactly)
			      EMSG_ONE_RET_NULL;
			  ret = reg(REG_ZPAREN, &flags);
			  જો (ret == ખાલી)
			      પાછા ખાલી;
			  *flagp |= flags & (HASWIDTH|SPSTART|HASNL|HASLOOKBH);
			  re_has_z = REX_SET;
			  તોડો;

		સ્થિતિ '1':
		સ્થિતિ '2':
		સ્થિતિ '3':
		સ્થિતિ '4':
		સ્થિતિ '5':
		સ્થિતિ '6':
		સ્થિતિ '7':
		સ્થિતિ '8':
		સ્થિતિ '9': જો ((reg_do_extmatch & REX_USE) == 0)
			      EMSG_RET_NULL(_(e_z1_z9_not_allowed_here));
			  ret = regnode(ZREF + c - '0');
			  re_has_z = REX_USE;
			  તોડો;
#endif

		સ્થિતિ 's': ret = regnode(MOPEN + 0);
			  જો (re_mult_next("\\zs") == FAIL)
			      પાછા ખાલી;
			  તોડો;

		સ્થિતિ 'e': ret = regnode(MCLOSE + 0);
			  જો (re_mult_next("\\ze") == FAIL)
			      પાછા ખાલી;
			  તોડો;

		મૂળભૂત:  EMSG_RET_NULL(_(e_invalid_character_after_bsl_z));
	    }
	}
	તોડો;

      સ્થિતિ Magic('%'):
	{
	    c = no_Magic(getchr());
	    પસંદગી (c)
	    {
		// () without a back reference
		સ્થિતિ '(':
		    જો (one_exactly)
			EMSG_ONE_RET_NULL;
		    ret = reg(REG_NPAREN, &flags);
		    જો (ret == ખાલી)
			પાછા ખાલી;
		    *flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
		    તોડો;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		સ્થિતિ '^':
		    ret = regnode(RE_BOF);
		    તોડો;

		સ્થિતિ '$':
		    ret = regnode(RE_EOF);
		    તોડો;

		સ્થિતિ '#':
		    જો (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_start_of_pattern),
								  regparse[1]);
			પાછા FAIL;
		    }
		    ret = regnode(CURSOR);
		    તોડો;

		સ્થિતિ 'V':
		    ret = regnode(RE_VISUAL);
		    તોડો;

		સ્થિતિ 'C':
		    ret = regnode(RE_COMPOSING);
		    તોડો;

		// \%[abc]: Emit as a list of branches, all ending at the last
		// branch which matches nothing.
		સ્થિતિ '[':
			  જો (one_exactly)	// doesn't nest
			      EMSG_ONE_RET_NULL;
			  {
			      char_u	*lastbranch;
			      char_u	*lastnode = ખાલી;
			      char_u	*br;

			      ret = ખાલી;
			      જ્યારે ((c = getchr()) != ']')
			      {
				  જો (c == NUL)
				      EMSG2_RET_NULL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
				  br = regnode(BRANCH);
				  જો (ret == ખાલી)
				      ret = br;
				  નહીંતર
				  {
				      regtail(lastnode, br);
				      જો (reg_toolong)
					  પાછા ખાલી;
				  }

				  ungetchr();
				  one_exactly = TRUE;
				  lastnode = regatom(flagp);
				  one_exactly = FALSE;
				  જો (lastnode == ખાલી)
				      પાછા ખાલી;
			      }
			      જો (ret == ખાલી)
				  EMSG2_RET_NULL(_(e_empty_str_brackets),
						      reg_magic == MAGIC_ALL);
			      lastbranch = regnode(BRANCH);
			      br = regnode(NOTHING);
			      જો (ret != JUST_CALC_SIZE)
			      {
				  regtail(lastnode, br);
				  regtail(lastbranch, br);
				  // connect all branches to the NOTHING
				  // branch at the end
				  માટે (br = ret; br != lastnode; )
				  {
				      જો (OP(br) == BRANCH)
				      {
					  regtail(br, lastbranch);
					  જો (reg_toolong)
					      પાછા ખાલી;
					  br = OPERAND(br);
				      }
				      નહીંતર
					  br = regnext(br);
				  }
			      }
			      *flagp &= ~(HASWIDTH | SIMPLE);
			      તોડો;
			  }

		સ્થિતિ 'd':   // %d123 decimal
		સ્થિતિ 'o':   // %o123 octal
		સ્થિતિ 'x':   // %xab hex 2
		સ્થિતિ 'u':   // %uabcd hex 4
		સ્થિતિ 'U':   // %U1234abcd hex 8
			  {
			      vimlong_T i;

			      પસંદગી (c)
			      {
				  સ્થિતિ 'd': i = getdecchrs(); તોડો;
				  સ્થિતિ 'o': i = getoctchrs(); તોડો;
				  સ્થિતિ 'x': i = gethexchrs(2); તોડો;
				  સ્થિતિ 'u': i = gethexchrs(4); તોડો;
				  સ્થિતિ 'U': i = gethexchrs(8); તોડો;
				  મૂળભૂત:  i = -1; તોડો;
			      }

			      જો (i < 0 || i > INT_MAX)
				  EMSG2_RET_NULL(
					    _(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			      જો (use_multibytecode(i))
				  ret = regnode(MULTIBYTECODE);
			      નહીંતર
				  ret = regnode(EXACTLY);
			      જો (i == 0)
				  regc(0x0a);
			      નહીંતર
				  regmbc((પૂર્ણાંક)i);
			      regc(NUL);
			      *flagp |= HASWIDTH;
			      તોડો;
			  }

		મૂળભૂત:
			  જો (VIM_ISDIGIT(c) || c == '<' || c == '>'
						|| c == '\'' || c == '.')
			  {
			      long_u	n = 0;
			      પૂર્ણાંક	cmp;
			      પૂર્ણાંક	cur = FALSE;
			      પૂર્ણાંક	got_digit = FALSE;

			      cmp = c;
			      જો (cmp == '<' || cmp == '>')
				  c = getchr();
			      જો (no_Magic(c) == '.')
			      {
				  cur = TRUE;
				  c = getchr();
			      }
			      જ્યારે (VIM_ISDIGIT(c))
			      {
				  got_digit = TRUE;
				  n = n * 10 + (c - '0');
				  c = getchr();
			      }
			      જો (no_Magic(c) == '\'' && n == 0)
			      {
				  // "\%'m", "\%<'m" and "\%>'m": Mark
				  c = getchr();
				  ret = regnode(RE_MARK);
				  જો (ret == JUST_CALC_SIZE)
				      regsize += 2;
				  નહીંતર
				  {
				      *regcode++ = c;
				      *regcode++ = cmp;
				  }
				  તોડો;
			      }
			      નહીંતર જો ((c == 'l' || c == 'c' || c == 'v')
					  && (cur || got_digit))
			      {
				  જો (cur && n)
				  {
				    semsg(_(e_regexp_સંખ્યા_after_dot_pos_શોધ_chr),
								  no_Magic(c));
				    rc_did_emsg = TRUE;
				    પાછા ખાલી;
				  }
				  જો (c == 'l')
				  {
				      જો (cur)
					  n = curwin->w_cursor.lnum;
				      ret = regnode(RE_LNUM);
				      જો (save_prev_at_start)
					  at_start = TRUE;
				  }
				  નહીંતર જો (c == 'c')
				  {
				      જો (cur)
				      {
					  n = curwin->w_cursor.col;
					  n++;
				      }
				      ret = regnode(RE_COL);
				  }
				  નહીંતર
				  {
				      જો (cur)
				      {
					  colnr_T vcol = 0;

					  getvvcol(curwin, &curwin->w_cursor,
							 ખાલી, ખાલી, &vcol, 0);
					  ++vcol;
					  n = vcol;
				      }
				      ret = regnode(RE_VCOL);
				  }
				  જો (ret == JUST_CALC_SIZE)
				      regsize += 5;
				  નહીંતર
				  {
				      // put the number and the optional
				      // comparator after the opcode
				      regcode = re_put_long(regcode, n);
				      *regcode++ = cmp;
				  }
				  તોડો;
			      }
			  }

			  EMSG2_RET_NULL(_(e_invalid_character_after_str),
						      reg_magic == MAGIC_ALL);
	    }
	}
	તોડો;

      સ્થિતિ Magic('['):
collection:
	{
	    char_u	*lp;

	    // If there is no matching ']', we assume the '[' is a normal
	    // character.  This makes 'incsearch' and ":help [" work.
	    lp = skip_anyof(regparse);
	    જો (*lp == ']')	// there is a matching ']'
	    {
		પૂર્ણાંક	startc = -1;	// > 0 when next '-' is a range
		પૂર્ણાંક	endc;

		જો (extra == 0)
		{
		    પૂર્ણાંક	cl = bt_recognize_char_class(regparse, lp);

		    જો (cl != 0)
		    {
			ret = regnode(cl);
			regparse = lp;
			skipchr();
			*flagp |= HASWIDTH | SIMPLE;
			તોડો;
		    }
		}

		// In a character class, different parsing rules apply.
		// Not even \ is special anymore, nothing is.
		જો (*regparse == '^')	    // Complement of range.
		{
		    ret = regnode(ANYBUT + extra);
		    regparse++;
		}
		નહીંતર
		    ret = regnode(ANYOF + extra);

		// At the start ']' and '-' mean the literal character.
		જો (*regparse == ']' || *regparse == '-')
		{
		    startc = *regparse;
		    regc(*regparse++);
		}

		જ્યારે (*regparse != NUL && *regparse != ']')
		{
		    જો (*regparse == '-')
		    {
			++regparse;
			// The '-' is not used for a range at the end and
			// after or before a '\n'.
			જો (*regparse == ']' || *regparse == NUL
				|| startc == -1
				|| (regparse[0] == '\\' && regparse[1] == 'n'))
			{
			    regc('-');
			    startc = '-';	// [--x] is a range
			}
			નહીંતર
			{
			    // Also accept "a-[.z.]"
			    endc = 0;
			    જો (*regparse == '[')
				endc = get_coll_element(&regparse);
			    જો (endc == 0)
			    {
				જો (has_mbyte)
				    endc = mb_ptr2char_adv(&regparse);
				નહીંતર
				    endc = *regparse++;
			    }

			    // Handle \o40, \x20 and \u20AC style sequences
			    જો (endc == '\\' && !reg_cpo_lit && !reg_cpo_bsl)
				endc = coll_get_char();

			    જો (startc > endc)
				EMSG_RET_NULL(_(e_reverse_range_in_character_class));
			    જો (has_mbyte && ((*mb_char2len)(startc) > 1
						 || (*mb_char2len)(endc) > 1))
			    {
				// Limit to a range of 256 chars.
				જો (endc > startc + 256)
				    EMSG_RET_NULL(_(e_range_too_large_in_character_class));
				જ્યારે (++startc <= endc)
				    regmbc(startc);
			    }
			    નહીંતર
			    {
				જ્યારે (++startc <= endc)
				    regc(startc);
			    }
			    startc = -1;
			}
		    }
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    નહીંતર જો (*regparse == '\\'
			    && !reg_cpo_bsl
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != ખાલી
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR,
						       regparse[1]) != ખાલી)))
		    {
			regparse++;
			જો (*regparse == 'n')
			{
			    // '\n' in range: also match NL
			    જો (ret != JUST_CALC_SIZE)
			    {
				// Using \n inside [^] does not change what
				// matches. "[^\n]" is the same as ".".
				જો (*ret == ANYOF)
				{
				    *ret = ANYOF + ADD_NL;
				    *flagp |= HASNL;
				}
				// else: must have had a \n already
			    }
			    regparse++;
			    startc = -1;
			}
			નહીંતર જો (*regparse == 'd'
				|| *regparse == 'o'
				|| *regparse == 'x'
				|| *regparse == 'u'
				|| *regparse == 'U')
			{
			    startc = coll_get_char();
			    // max UTF-8 Codepoint is U+10FFFF,
			    // but allow values until INT_MAX
			    જો (startc == INT_MAX)
				EMSG_RET_NULL(_(e_unicode_val_too_large));
			    જો (startc == 0)
				regc(0x0a);
			    નહીંતર
				regmbc(startc);
			}
			નહીંતર
			{
			    startc = backslash_trans(*regparse++);
			    regc(startc);
			}
		    }
		    નહીંતર જો (*regparse == '[')
		    {
			પૂર્ણાંક c_class;
			પૂર્ણાંક cu;

			c_class = get_char_class(&regparse);
			startc = -1;
			// Characters assumed to be 8 bits!
			પસંદગી (c_class)
			{
			    સ્થિતિ CLASS_NONE:
				c_class = get_equi_class(&regparse);
				જો (c_class != 0)
				{
				    // produce equivalence class
				    reg_equi_class(c_class);
				}
				નહીંતર જો ((c_class =
					    get_coll_element(&regparse)) != 0)
				{
				    // produce a collating element
				    regmbc(c_class);
				}
				નહીંતર
				{
				    // literal '[', allow [[-x] as a range
				    startc = *regparse++;
				    regc(startc);
				}
				તોડો;
			    સ્થિતિ CLASS_ALNUM:
				માટે (cu = 1; cu < 128; cu++)
				    જો (isalnum(cu))
					regmbc(cu);
				તોડો;
			    સ્થિતિ CLASS_ALPHA:
				માટે (cu = 1; cu < 128; cu++)
				    જો (isalpha(cu))
					regmbc(cu);
				તોડો;
			    સ્થિતિ CLASS_BLANK:
				regc(' ');
				regc('\t');
				તોડો;
			    સ્થિતિ CLASS_CNTRL:
				માટે (cu = 1; cu <= 127; cu++)
				    જો (iscntrl(cu))
					regmbc(cu);
				તોડો;
			    સ્થિતિ CLASS_DIGIT:
				માટે (cu = 1; cu <= 127; cu++)
				    જો (VIM_ISDIGIT(cu))
					regmbc(cu);
				તોડો;
			    સ્થિતિ CLASS_GRAPH:
				માટે (cu = 1; cu <= 127; cu++)
				    જો (isgraph(cu))
					regmbc(cu);
				તોડો;
			    સ્થિતિ CLASS_LOWER:
				માટે (cu = 1; cu <= 255; cu++)
				    જો (MB_ISLOWER(cu) && cu != 170
								 && cu != 186)
					regmbc(cu);
				તોડો;
			    સ્થિતિ CLASS_PRINT:
				માટે (cu = 1; cu <= 255; cu++)
				    જો (vim_isprintc(cu))
					regmbc(cu);
				તોડો;
			    સ્થિતિ CLASS_PUNCT:
				માટે (cu = 1; cu < 128; cu++)
				    જો (ispunct(cu))
					regmbc(cu);
				તોડો;
			    સ્થિતિ CLASS_SPACE:
				માટે (cu = 9; cu <= 13; cu++)
				    regc(cu);
				regc(' ');
				તોડો;
			    સ્થિતિ CLASS_UPPER:
				માટે (cu = 1; cu <= 255; cu++)
				    જો (MB_ISUPPER(cu))
					regmbc(cu);
				તોડો;
			    સ્થિતિ CLASS_XDIGIT:
				માટે (cu = 1; cu <= 255; cu++)
				    જો (vim_isxdigit(cu))
					regmbc(cu);
				તોડો;
			    સ્થિતિ CLASS_TAB:
				regc('\t');
				તોડો;
			    સ્થિતિ CLASS_RETURN:
				regc('\r');
				તોડો;
			    સ્થિતિ CLASS_BACKSPACE:
				regc('\b');
				તોડો;
			    સ્થિતિ CLASS_ESCAPE:
				regc('\033');
				તોડો;
			    સ્થિતિ CLASS_IDENT:
				માટે (cu = 1; cu <= 255; cu++)
				    જો (vim_isIDc(cu))
					regmbc(cu);
				તોડો;
			    સ્થિતિ CLASS_KEYWORD:
				માટે (cu = 1; cu <= 255; cu++)
				    જો (reg_iswordc(cu))
					regmbc(cu);
				તોડો;
			    સ્થિતિ CLASS_FNAME:
				માટે (cu = 1; cu <= 255; cu++)
				    જો (vim_isfilec(cu))
					regmbc(cu);
				તોડો;
			}
		    }
		    નહીંતર
		    {
			જો (has_mbyte)
			{
			    પૂર્ણાંક	len;

			    // produce a multibyte character, including any
			    // following composing characters
			    startc = mb_ptr2char(regparse);
			    len = (*mb_ptr2len)(regparse);
			    જો (enc_utf8 && utf_char2len(startc) != len)
				startc = -1;	// composing chars
			    જ્યારે (--len >= 0)
				regc(*regparse++);
			}
			નહીંતર
			{
			    startc = *regparse++;
			    regc(startc);
			}
		    }
		}
		regc(NUL);
		prevchr_len = 1;	// last char was the ']'
		જો (*regparse != ']')
		    EMSG_RET_NULL(_(e_too_many_brackets));  // Cannot happen?
		skipchr();	    // let's be friends with the lexer again
		*flagp |= HASWIDTH | SIMPLE;
		તોડો;
	    }
	    નહીંતર જો (reg_strict)
		EMSG2_RET_NULL(_(e_missing_rsb_after_str_lsb),
							reg_magic > MAGIC_OFF);
	}
	// FALLTHROUGH

      મૂળભૂત:
	{
	    પૂર્ણાંક		len;

	    // A multi-byte character is handled as a separate atom if it's
	    // before a multi and when it's a composing char.
	    જો (use_multibytecode(c))
	    {
do_multibyte:
		ret = regnode(MULTIBYTECODE);
		regmbc(c);
		*flagp |= HASWIDTH | SIMPLE;
		તોડો;
	    }

	    ret = regnode(EXACTLY);

	    // Append characters as long as:
	    // - there is no following multi, we then need the character in
	    //   front of it as a single character operand
	    // - not running into a Magic character
	    // - "one_exactly" is not set
	    // But always emit at least one character.  Might be a Multi,
	    // e.g., a "[" without matching "]".
	    માટે (len = 0; c != NUL && (len == 0
			|| (re_multi_type(peekchr()) == NOT_MULTI
			    && !one_exactly
			    && !is_Magic(c))); ++len)
	    {
		c = no_Magic(c);
		જો (has_mbyte)
		{
		    regmbc(c);
		    જો (enc_utf8)
		    {
			પૂર્ણાંક	l;

			// Need to get composing character too.
			માટે (;;)
			{
			    l = utf_ptr2len(regparse);
			    જો (!UTF_COMPOSINGLIKE(regparse, regparse + l))
				તોડો;
			    regmbc(utf_ptr2char(regparse));
			    skipchr();
			}
		    }
		}
		નહીંતર
		    regc(c);
		c = getchr();
	    }
	    ungetchr();

	    regc(NUL);
	    *flagp |= HASWIDTH;
	    જો (len == 1)
		*flagp |= SIMPLE;
	}
	તોડો;
    }

    પાછા ret;
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
    સ્થિર char_u *
regpiece(પૂર્ણાંક *flagp)
{
    char_u	    *ret;
    પૂર્ણાંક		    op;
    char_u	    *next;
    પૂર્ણાંક		    flags;
    long	    minval;
    long	    maxval;

    ret = regatom(&flags);
    જો (ret == ખાલી)
	પાછા ખાલી;

    op = peekchr();
    જો (re_multi_type(op) == NOT_MULTI)
    {
	*flagp = flags;
	પાછા ret;
    }
    // default flags
    *flagp = (WORST | SPSTART | (flags & (HASNL | HASLOOKBH)));

    skipchr();
    પસંદગી (op)
    {
	સ્થિતિ Magic('*'):
	    જો (flags & SIMPLE)
		reginsert(STAR, ret);
	    નહીંતર
	    {
		// Emit x* as (x&|), where & means "self".
		reginsert(BRANCH, ret); // Either x
		regoptail(ret, regnode(BACK));	// and loop
		regoptail(ret, ret);	// back
		regtail(ret, regnode(BRANCH));	// or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    તોડો;

	સ્થિતિ Magic('+'):
	    જો (flags & SIMPLE)
		reginsert(PLUS, ret);
	    નહીંતર
	    {
		// Emit x+ as x(&|), where & means "self".
		next = regnode(BRANCH); // Either
		regtail(ret, next);
		regtail(regnode(BACK), ret);	// loop back
		regtail(next, regnode(BRANCH)); // or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    *flagp = (WORST | HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    તોડો;

	સ્થિતિ Magic('@'):
	    {
		પૂર્ણાંક	lop = END;
		long	nr;

		nr = (long)getdecchrs();
		પસંદગી (no_Magic(getchr()))
		{
		    સ્થિતિ '=': lop = MATCH; તોડો;		  // \@=
		    સ્થિતિ '!': lop = NOMATCH; તોડો;		  // \@!
		    સ્થિતિ '>': lop = SUBPAT; તોડો;		  // \@>
		    સ્થિતિ '<': પસંદગી (no_Magic(getchr()))
			      {
				  સ્થિતિ '=': lop = BEHIND; તોડો;   // \@<=
				  સ્થિતિ '!': lop = NOBEHIND; તોડો; // \@<!
			      }
		}
		જો (lop == END)
		    EMSG2_RET_NULL(_(e_invalid_character_after_str_at),
						      reg_magic == MAGIC_ALL);
		// Look behind must match with behind_pos.
		જો (lop == BEHIND || lop == NOBEHIND)
		{
		    regtail(ret, regnode(BHPOS));
		    *flagp |= HASLOOKBH;
		}
		regtail(ret, regnode(END)); // operand ends
		જો (lop == BEHIND || lop == NOBEHIND)
		{
		    જો (nr < 0)
			nr = 0; // no limit is same as zero limit
		    reginsert_nr(lop, nr, ret);
		}
		નહીંતર
		    reginsert(lop, ret);
		તોડો;
	    }

	સ્થિતિ Magic('?'):
	સ્થિતિ Magic('='):
	    // Emit x= as (x|)
	    reginsert(BRANCH, ret);		// Either x
	    regtail(ret, regnode(BRANCH));	// or
	    next = regnode(NOTHING);		// null.
	    regtail(ret, next);
	    regoptail(ret, next);
	    તોડો;

	સ્થિતિ Magic('{'):
	    જો (!read_limits(&minval, &maxval))
		પાછા ખાલી;
	    જો (flags & SIMPLE)
	    {
		reginsert(BRACE_SIMPLE, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
	    }
	    નહીંતર
	    {
		જો (અંક_complex_braces >= 10)
		    EMSG2_RET_NULL(_(e_too_many_complex_str_curly),
						      reg_magic == MAGIC_ALL);
		reginsert(BRACE_COMPLEX + અંક_complex_braces, ret);
		regoptail(ret, regnode(BACK));
		regoptail(ret, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
		++અંક_complex_braces;
	    }
	    જો (minval > 0 && maxval > 0)
		*flagp = (HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    તોડો;
    }
    જો (re_multi_type(peekchr()) != NOT_MULTI)
    {
	// Can't have a multi follow a multi.
	જો (peekchr() == Magic('*'))
	    EMSG2_RET_NULL(_(e_nested_str), reg_magic >= MAGIC_ON);
	EMSG3_RET_NULL(_(e_nested_str_chr), reg_magic == MAGIC_ALL,
							  no_Magic(peekchr()));
    }

    પાછા ret;
}

/*
 * Parse one alternative of an | or & operator.
 * Implements the concatenation operator.
 */
    સ્થિર char_u *
regconcat(પૂર્ણાંક *flagp)
{
    char_u	*first = ખાલી;
    char_u	*chain = ખાલી;
    char_u	*latest;
    પૂર્ણાંક		flags;
    પૂર્ણાંક		cont = TRUE;

    *flagp = WORST;		// Tentatively.

    જ્યારે (cont)
    {
	પસંદગી (peekchr())
	{
	    સ્થિતિ NUL:
	    સ્થિતિ Magic('|'):
	    સ્થિતિ Magic('&'):
	    સ્થિતિ Magic(')'):
			    cont = FALSE;
			    તોડો;
	    સ્થિતિ Magic('Z'):
			    regflags |= RF_ICOMBINE;
			    skipchr_keepstart();
			    તોડો;
	    સ્થિતિ Magic('c'):
			    regflags |= RF_ICASE;
			    skipchr_keepstart();
			    તોડો;
	    સ્થિતિ Magic('C'):
			    regflags |= RF_NOICASE;
			    skipchr_keepstart();
			    તોડો;
	    સ્થિતિ Magic('v'):
			    reg_magic = MAGIC_ALL;
			    skipchr_keepstart();
			    curchr = -1;
			    તોડો;
	    સ્થિતિ Magic('m'):
			    reg_magic = MAGIC_ON;
			    skipchr_keepstart();
			    curchr = -1;
			    તોડો;
	    સ્થિતિ Magic('M'):
			    reg_magic = MAGIC_OFF;
			    skipchr_keepstart();
			    curchr = -1;
			    તોડો;
	    સ્થિતિ Magic('V'):
			    reg_magic = MAGIC_NONE;
			    skipchr_keepstart();
			    curchr = -1;
			    તોડો;
	    મૂળભૂત:
			    latest = regpiece(&flags);
			    જો (latest == ખાલી || reg_toolong)
				પાછા ખાલી;
			    *flagp |= flags & (HASWIDTH | HASNL | HASLOOKBH);
			    જો (chain == ખાલી)	// First piece.
				*flagp |= flags & SPSTART;
			    નહીંતર
				regtail(chain, latest);
			    chain = latest;
			    જો (first == ખાલી)
				first = latest;
			    તોડો;
	}
    }
    જો (first == ખાલી)		// Loop ran zero times.
	first = regnode(NOTHING);
    પાછા first;
}

/*
 * Parse one alternative of an | operator.
 * Implements the & operator.
 */
    સ્થિર char_u *
regbranch(પૂર્ણાંક *flagp)
{
    char_u	*ret;
    char_u	*chain = ખાલી;
    char_u	*latest;
    પૂર્ણાંક		flags;

    *flagp = WORST | HASNL;		// Tentatively.

    ret = regnode(BRANCH);
    માટે (;;)
    {
	latest = regconcat(&flags);
	જો (latest == ખાલી)
	    પાછા ખાલી;
	// If one of the branches has width, the whole thing has.  If one of
	// the branches anchors at start-of-line, the whole thing does.
	// If one of the branches uses look-behind, the whole thing does.
	*flagp |= flags & (HASWIDTH | SPSTART | HASLOOKBH);
	// If one of the branches doesn't match a line-break, the whole thing
	// doesn't.
	*flagp &= ~HASNL | (flags & HASNL);
	જો (chain != ખાલી)
	    regtail(chain, latest);
	જો (peekchr() != Magic('&'))
	    તોડો;
	skipchr();
	regtail(latest, regnode(END)); // operand ends
	જો (reg_toolong)
	    તોડો;
	reginsert(MATCH, latest);
	chain = latest;
    }

    પાછા ret;
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
    સ્થિર char_u *
reg(
    પૂર્ણાંક		paren,	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
    પૂર્ણાંક		*flagp)
{
    char_u	*ret;
    char_u	*br;
    char_u	*ender;
    પૂર્ણાંક		parno = 0;
    પૂર્ણાંક		flags;

    *flagp = HASWIDTH;		// Tentatively.

#ifdef FEAT_SYN_HL
    જો (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	જો (regnzpar >= NSUBEXP)
	    EMSG_RET_NULL(_(e_too_many_z));
	parno = regnzpar;
	regnzpar++;
	ret = regnode(ZOPEN + parno);
    }
    નહીંતર
#endif
	જો (paren == REG_PAREN)
    {
	// Make a MOPEN node.
	જો (regnpar >= NSUBEXP)
	    EMSG2_RET_NULL(_(e_too_many_str_open), reg_magic == MAGIC_ALL);
	parno = regnpar;
	++regnpar;
	ret = regnode(MOPEN + parno);
    }
    નહીંતર જો (paren == REG_NPAREN)
    {
	// Make a NOPEN node.
	ret = regnode(NOPEN);
    }
    નહીંતર
	ret = ખાલી;

    જો (bt_reg_parse_depth >= REG_MAX_PAREN_DEPTH)
	EMSG_RET_NULL(_(e_command_too_complex));
    ++bt_reg_parse_depth;

    // Pick up the branches, linking them together.
    br = regbranch(&flags);
    જો (br == ખાલી)
    {
	ret = ખાલી;
	જાઓ theend;
    }
    જો (ret != ખાલી)
	regtail(ret, br);	// [MZ]OPEN -> first.
    નહીંતર
	ret = br;
    // If one of the branches can be zero-width, the whole thing can.
    // If one of the branches has * at start or matches a line-break, the
    // whole thing can.
    જો (!(flags & HASWIDTH))
	*flagp &= ~HASWIDTH;
    *flagp |= flags & (SPSTART | HASNL | HASLOOKBH);
    જ્યારે (peekchr() == Magic('|'))
    {
	skipchr();
	br = regbranch(&flags);
	જો (br == ખાલી || reg_toolong)
	{
	    ret = ખાલી;
	    જાઓ theend;
	}
	regtail(ret, br);	// BRANCH -> BRANCH.
	જો (!(flags & HASWIDTH))
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
    માટે (br = ret; br != ખાલી; br = regnext(br))
	regoptail(br, ender);

    // Check for proper termination.
    જો (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
#ifdef FEAT_SYN_HL
	જો (paren == REG_ZPAREN)
	{
	    emsg(_(e_unmatched_z));
	    rc_did_emsg = TRUE;
	    ret = ખાલી;
	    જાઓ theend;
	}
	નહીંતર
#endif
	જો (paren == REG_NPAREN)
	{
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = ખાલી;
	    જાઓ theend;
	}
	નહીંતર
	{
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = ખાલી;
	    જાઓ theend;
	}
    }
    નહીંતર જો (paren == REG_NOPAREN && peekchr() != NUL)
    {
	જો (curchr == Magic(')'))
	{
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = ખાલી;
	    જાઓ theend;
	}
	નહીંતર
	{
	    emsg(_(e_trailing_characters));	// "Can't happen".
	    rc_did_emsg = TRUE;
	    ret = ખાલી;
	    જાઓ theend;
	}
    }
    // Here we set the flag allowing back references to this set of
    // parentheses.
    જો (paren == REG_PAREN)
	had_endbrace[parno] = TRUE;	// have seen the close paren

theend:
    --bt_reg_parse_depth;
    પાછા ret;
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
    સ્થિર regprog_T *
bt_regcomp(char_u *પદાવલિ, પૂર્ણાંક re_flags)
{
    bt_regprog_T    *r;
    char_u	*scan;
    char_u	*longest;
    પૂર્ણાંક		len;
    પૂર્ણાંક		flags;

    જો (પદાવલિ == ખાલી)
	IEMSG_RET_NULL(e_null_argument);

    init_class_tab();

    // First pass: determine size, legality.
    regcomp_start(પદાવલિ, re_flags);
    regcode = JUST_CALC_SIZE;
    regc(REGMAGIC);
    જો (reg(REG_NOPAREN, &flags) == ખાલી)
	પાછા ખાલી;

    // Allocate space.
    r = alloc(offsetof(bt_regprog_T, program) + regsize);
    જો (r == ખાલી)
	પાછા ખાલી;
    r->re_in_use = FALSE;
#ifdef DEBUG
    r->regsz = regsize;
#endif

    // Second pass: emit code.
    regcomp_start(પદાવલિ, re_flags);
    regcode = r->program;
    regc(REGMAGIC);
    જો (reg(REG_NOPAREN, &flags) == ખાલી || reg_toolong)
    {
	vim_free(r);
	જો (reg_toolong)
	    EMSG_RET_NULL(_(e_pattern_too_long));
	પાછા ખાલી;
    }

    // Dig out information for optimizations.
    r->regstart = NUL;		// Worst-case defaults.
    r->reganch = 0;
    r->regmust = ખાલી;
    r->regmlen = 0;
    r->regflags = regflags;
    જો (flags & HASNL)
	r->regflags |= RF_HASNL;
    જો (flags & HASLOOKBH)
	r->regflags |= RF_LOOKBH;
#ifdef FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    r->reghasz = re_has_z;
#endif
    scan = r->program + 1;	// First BRANCH.
    જો (OP(regnext(scan)) == END)   // Only one top-level choice.
    {
	scan = OPERAND(scan);

	// Starting-point info.
	જો (OP(scan) == BOL || OP(scan) == RE_BOF)
	{
	    r->reganch++;
	    scan = regnext(scan);
	}

	જો (OP(scan) == EXACTLY)
	{
	    જો (has_mbyte)
		r->regstart = (*mb_ptr2char)(OPERAND(scan));
	    નહીંતર
		r->regstart = *OPERAND(scan);
	}
	નહીંતર જો ((OP(scan) == BOW
		    || OP(scan) == EOW
		    || OP(scan) == NOTHING
		    || OP(scan) == MOPEN + 0 || OP(scan) == NOPEN
		    || OP(scan) == MCLOSE + 0 || OP(scan) == NCLOSE)
		 && OP(regnext(scan)) == EXACTLY)
	{
	    જો (has_mbyte)
		r->regstart = (*mb_ptr2char)(OPERAND(regnext(scan)));
	    નહીંતર
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
	જો ((flags & SPSTART || OP(scan) == BOW || OP(scan) == EOW)
							  && !(flags & HASNL))
	{
	    size_t  scanlen;

	    longest = ખાલી;
	    len = 0;
	    માટે (; scan != ખાલી; scan = regnext(scan))
	    {
		જો (OP(scan) == EXACTLY)
		{
		    scanlen = STRLEN(OPERAND(scan));
		    જો (scanlen >= (size_t)len)
		    {
			longest = OPERAND(scan);
			len = (પૂર્ણાંક)scanlen;
		    }
		}
	    }
	    r->regmust = longest;
	    r->regmlen = len;
	}
    }
#ifdef BT_REGEXP_DUMP
    regdump(પદાવલિ, r);
#endif
    r->engine = &bt_regengine;
    પાછા (regprog_T *)r;
}

#જો defined(FEAT_SYN_HL)
/*
 * Check if during the previous call to vim_regcomp the EOL item "$" has been
 * found.  This is messy, but it works fine.
 */
    પૂર્ણાંક
vim_regcomp_had_eol(શૂન્ય)
{
    પાછા had_eol;
}
#endif

/*
 * Get a number after a backslash that is inside [].
 * When nothing is recognized return a backslash.
 */
    સ્થિર પૂર્ણાંક
coll_get_char(શૂન્ય)
{
    vimlong_T	nr = -1;

    પસંદગી (*regparse++)
    {
	સ્થિતિ 'd': nr = getdecchrs(); તોડો;
	સ્થિતિ 'o': nr = getoctchrs(); તોડો;
	સ્થિતિ 'x': nr = gethexchrs(2); તોડો;
	સ્થિતિ 'u': nr = gethexchrs(4); તોડો;
	સ્થિતિ 'U': nr = gethexchrs(8); તોડો;
    }
    જો (nr < 0)
    {
	// If getting the number fails be backwards compatible: the character
	// is a backslash.
	--regparse;
	nr = '\\';
    }
    જો (nr > INT_MAX)
	nr = INT_MAX;
    પાછા nr;
}

/*
 * Free a compiled regexp program, returned by bt_regcomp().
 */
    સ્થિર શૂન્ય
bt_regfree(regprog_T *prog)
{
    vim_free(prog);
}

#define ADVANCE_REGINPUT() MB_PTR_ADV(rex.આગત)

/*
 * The arguments from BRACE_LIMITS are stored here.  They are actually local
 * to regmatch(), but they are here to reduce the amount of stack space used
 * (it can be called recursively many times).
 */
સ્થિર long	bl_minval;
સ્થિર long	bl_maxval;

/*
 * Save the input line and position in a regsave_T.
 */
    સ્થિર શૂન્ય
reg_save(regsave_T *save, garray_T *gap)
{
    જો (REG_MULTI)
    {
	save->rs_u.pos.col = (colnr_T)(rex.આગત - rex.લીટી);
	save->rs_u.pos.lnum = rex.lnum;
    }
    નહીંતર
	save->rs_u.ptr = rex.આગત;
    save->rs_len = gap->ga_len;
}

/*
 * Restore the input line and position from a regsave_T.
 */
    સ્થિર શૂન્ય
reg_restore(regsave_T *save, garray_T *gap)
{
    જો (REG_MULTI)
    {
	જો (rex.lnum != save->rs_u.pos.lnum)
	{
	    // only call reg_getline() when the line number changed to save
	    // a bit of time
	    rex.lnum = save->rs_u.pos.lnum;
	    rex.લીટી = reg_getline(rex.lnum);
	}
	rex.આગત = rex.લીટી + save->rs_u.pos.col;
    }
    નહીંતર
	rex.આગત = save->rs_u.ptr;
    gap->ga_len = save->rs_len;
}

/*
 * Return TRUE if current position is equal to saved position.
 */
    સ્થિર પૂર્ણાંક
reg_save_equal(regsave_T *save)
{
    જો (REG_MULTI)
	પાછા rex.lnum == save->rs_u.pos.lnum
				  && rex.આગત == rex.લીટી + save->rs_u.pos.col;
    પાછા rex.આગત == save->rs_u.ptr;
}

// Save the sub-expressions before attempting a match.
#define save_se(savep, posp, pp) \
    REG_MULTI ? save_se_multi((savep), (posp)) : save_se_one((savep), (pp))

// After a failed match restore the sub-expressions.
#define restore_se(savep, posp, pp) \
{ \
    જો (REG_MULTI) \
	*(posp) = (savep)->se_u.pos; \
    નહીંતર \
	*(pp) = (savep)->se_u.ptr; \
}

/*
 * Tentatively set the sub-expression start to the current position (after
 * calling regmatch() they will have changed).  Need to save the existing
 * values for when there is no match.
 * Use se_save() to use pointer (save_se_multi()) or position (save_se_one()),
 * depending on REG_MULTI.
 */
    સ્થિર શૂન્ય
save_se_multi(save_se_T *savep, lpos_T *posp)
{
    savep->se_u.pos = *posp;
    posp->lnum = rex.lnum;
    posp->col = (colnr_T)(rex.આગત - rex.લીટી);
}

    સ્થિર શૂન્ય
save_se_one(save_se_T *savep, char_u **pp)
{
    savep->se_u.ptr = *pp;
    *pp = rex.આગત;
}

/*
 * regrepeat - repeatedly match something simple, return how many.
 * Advances rex.input (and rex.lnum) to just after the matched chars.
 */
    સ્થિર પૂર્ણાંક
regrepeat(
    char_u	*p,
    long	maxcount)   // maximum number of matches allowed
{
    long	ગણતરી = 0;
    char_u	*scan;
    char_u	*opnd;
    પૂર્ણાંક		mask;
    પૂર્ણાંક		testval = 0;

    scan = rex.આગત;	    // Make local copy of rex.input for speed.
    opnd = OPERAND(p);
    પસંદગી (OP(p))
    {
      સ્થિતિ ANY:
      સ્થિતિ ANY + ADD_NL:
	જ્યારે (ગણતરી < maxcount)
	{
	    // Matching anything means we continue until end-of-line (or
	    // end-of-file for ANY + ADD_NL), only limited by maxcount.
	    જ્યારે (*scan != NUL && ગણતરી < maxcount)
	    {
		++ગણતરી;
		MB_PTR_ADV(scan);
	    }
	    જો (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
				      || rex.reg_લીટી_lbr || ગણતરી == maxcount)
		તોડો;
	    ++ગણતરી;		// count the line-break
	    reg_nextline();
	    scan = rex.આગત;
	    જો (got_int)
		તોડો;
	}
	તોડો;

      સ્થિતિ IDENT:
      સ્થિતિ IDENT + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      સ્થિતિ SIDENT:
      સ્થિતિ SIDENT + ADD_NL:
	જ્યારે (ગણતરી < maxcount)
	{
	    જો (vim_isIDc(PTR2CHAR(scan)) && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    નહીંતર જો (*scan == NUL)
	    {
		જો (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_લીટી_lbr)
		    તોડો;
		reg_nextline();
		scan = rex.આગત;
		જો (got_int)
		    તોડો;
	    }
	    નહીંતર જો (rex.reg_લીટી_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    નહીંતર
		તોડો;
	    ++ગણતરી;
	}
	તોડો;

      સ્થિતિ KWORD:
      સ્થિતિ KWORD + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      સ્થિતિ SKWORD:
      સ્થિતિ SKWORD + ADD_NL:
	જ્યારે (ગણતરી < maxcount)
	{
	    જો (vim_iswordp_buf(scan, rex.reg_buf)
					  && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    નહીંતર જો (*scan == NUL)
	    {
		જો (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_લીટી_lbr)
		    તોડો;
		reg_nextline();
		scan = rex.આગત;
		જો (got_int)
		    તોડો;
	    }
	    નહીંતર જો (rex.reg_લીટી_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    નહીંતર
		તોડો;
	    ++ગણતરી;
	}
	તોડો;

      સ્થિતિ FNAME:
      સ્થિતિ FNAME + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      સ્થિતિ SFNAME:
      સ્થિતિ SFNAME + ADD_NL:
	જ્યારે (ગણતરી < maxcount)
	{
	    જો (vim_isfilec(PTR2CHAR(scan)) && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    નહીંતર જો (*scan == NUL)
	    {
		જો (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_લીટી_lbr)
		    તોડો;
		reg_nextline();
		scan = rex.આગત;
		જો (got_int)
		    તોડો;
	    }
	    નહીંતર જો (rex.reg_લીટી_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    નહીંતર
		તોડો;
	    ++ગણતરી;
	}
	તોડો;

      સ્થિતિ PRINT:
      સ્થિતિ PRINT + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      સ્થિતિ SPRINT:
      સ્થિતિ SPRINT + ADD_NL:
	જ્યારે (ગણતરી < maxcount)
	{
	    જો (*scan == NUL)
	    {
		જો (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_લીટી_lbr)
		    તોડો;
		reg_nextline();
		scan = rex.આગત;
		જો (got_int)
		    તોડો;
	    }
	    નહીંતર જો (vim_isprintc(PTR2CHAR(scan)) == 1
					  && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    નહીંતર જો (rex.reg_લીટી_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    નહીંતર
		તોડો;
	    ++ગણતરી;
	}
	તોડો;

      સ્થિતિ WHITE:
      સ્થિતિ WHITE + ADD_NL:
	testval = mask = RI_WHITE;
do_class:
	જ્યારે (ગણતરી < maxcount)
	{
	    પૂર્ણાંક		l;

	    જો (*scan == NUL)
	    {
		જો (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_લીટી_lbr)
		    તોડો;
		reg_nextline();
		scan = rex.આગત;
		જો (got_int)
		    તોડો;
	    }
	    નહીંતર જો (has_mbyte && (l = (*mb_ptr2len)(scan)) > 1)
	    {
		જો (testval != 0)
		    તોડો;
		scan += l;
	    }
	    નહીંતર જો ((class_tab[*scan] & mask) == testval)
		++scan;
	    નહીંતર જો (rex.reg_લીટી_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    નહીંતર
		તોડો;
	    ++ગણતરી;
	}
	તોડો;

      સ્થિતિ NWHITE:
      સ્થિતિ NWHITE + ADD_NL:
	mask = RI_WHITE;
	જાઓ do_class;
      સ્થિતિ DIGIT:
      સ્થિતિ DIGIT + ADD_NL:
	testval = mask = RI_DIGIT;
	જાઓ do_class;
      સ્થિતિ NDIGIT:
      સ્થિતિ NDIGIT + ADD_NL:
	mask = RI_DIGIT;
	જાઓ do_class;
      સ્થિતિ HEX:
      સ્થિતિ HEX + ADD_NL:
	testval = mask = RI_HEX;
	જાઓ do_class;
      સ્થિતિ NHEX:
      સ્થિતિ NHEX + ADD_NL:
	mask = RI_HEX;
	જાઓ do_class;
      સ્થિતિ OCTAL:
      સ્થિતિ OCTAL + ADD_NL:
	testval = mask = RI_OCTAL;
	જાઓ do_class;
      સ્થિતિ NOCTAL:
      સ્થિતિ NOCTAL + ADD_NL:
	mask = RI_OCTAL;
	જાઓ do_class;
      સ્થિતિ WORD:
      સ્થિતિ WORD + ADD_NL:
	testval = mask = RI_WORD;
	જાઓ do_class;
      સ્થિતિ NWORD:
      સ્થિતિ NWORD + ADD_NL:
	mask = RI_WORD;
	જાઓ do_class;
      સ્થિતિ HEAD:
      સ્થિતિ HEAD + ADD_NL:
	testval = mask = RI_HEAD;
	જાઓ do_class;
      સ્થિતિ NHEAD:
      સ્થિતિ NHEAD + ADD_NL:
	mask = RI_HEAD;
	જાઓ do_class;
      સ્થિતિ ALPHA:
      સ્થિતિ ALPHA + ADD_NL:
	testval = mask = RI_ALPHA;
	જાઓ do_class;
      સ્થિતિ NALPHA:
      સ્થિતિ NALPHA + ADD_NL:
	mask = RI_ALPHA;
	જાઓ do_class;
      સ્થિતિ LOWER:
      સ્થિતિ LOWER + ADD_NL:
	testval = mask = RI_LOWER;
	જાઓ do_class;
      સ્થિતિ NLOWER:
      સ્થિતિ NLOWER + ADD_NL:
	mask = RI_LOWER;
	જાઓ do_class;
      સ્થિતિ UPPER:
      સ્થિતિ UPPER + ADD_NL:
	testval = mask = RI_UPPER;
	જાઓ do_class;
      સ્થિતિ NUPPER:
      સ્થિતિ NUPPER + ADD_NL:
	mask = RI_UPPER;
	જાઓ do_class;

      સ્થિતિ EXACTLY:
	{
	    પૂર્ણાંક	    cu, cl;

	    // This doesn't do a multi-byte character, because a MULTIBYTECODE
	    // would have been used for it.  It does handle single-byte
	    // characters, such as latin1.
	    જો (rex.reg_ic)
	    {
		cu = MB_TOUPPER(*opnd);
		cl = MB_TOLOWER(*opnd);
		જ્યારે (ગણતરી < maxcount && (*scan == cu || *scan == cl))
		{
		    ગણતરી++;
		    scan++;
		}
	    }
	    નહીંતર
	    {
		cu = *opnd;
		જ્યારે (ગણતરી < maxcount && *scan == cu)
		{
		    ગણતરી++;
		    scan++;
		}
	    }
	    તોડો;
	}

      સ્થિતિ MULTIBYTECODE:
	{
	    પૂર્ણાંક		i, len, cf = 0;

	    // Safety check (just in case 'encoding' was changed since
	    // compiling the program).
	    જો ((len = (*mb_ptr2len)(opnd)) > 1)
	    {
		જો (rex.reg_ic && enc_utf8)
		    cf = utf_fold(utf_ptr2char(opnd));
		જ્યારે (ગણતરી < maxcount && (*mb_ptr2len)(scan) >= len)
		{
		    માટે (i = 0; i < len; ++i)
			જો (opnd[i] != scan[i])
			    તોડો;
		    જો (i < len && (!rex.reg_ic || !enc_utf8
					|| utf_fold(utf_ptr2char(scan)) != cf))
			તોડો;
		    scan += len;
		    ++ગણતરી;
		}
	    }
	}
	તોડો;

      સ્થિતિ ANYOF:
      સ્થિતિ ANYOF + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH

      સ્થિતિ ANYBUT:
      સ્થિતિ ANYBUT + ADD_NL:
	જ્યારે (ગણતરી < maxcount)
	{
	    પૂર્ણાંક len;

	    જો (*scan == NUL)
	    {
		જો (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_લીટી_lbr)
		    તોડો;
		reg_nextline();
		scan = rex.આગત;
		જો (got_int)
		    તોડો;
	    }
	    નહીંતર જો (rex.reg_લીટી_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    નહીંતર જો (has_mbyte && (len = (*mb_ptr2len)(scan)) > 1)
	    {
		જો ((cstrchr(opnd, (*mb_ptr2char)(scan)) == ખાલી) == testval)
		    તોડો;
		scan += len;
	    }
	    નહીંતર
	    {
		જો ((cstrchr(opnd, *scan) == ખાલી) == testval)
		    તોડો;
		++scan;
	    }
	    ++ગણતરી;
	}
	તોડો;

      સ્થિતિ NEWL:
	જ્યારે (ગણતરી < maxcount
		&& ((*scan == NUL && rex.lnum <= rex.reg_maxline
				       && !rex.reg_લીટી_lbr && REG_MULTI)
		    || (*scan == '\n' && rex.reg_લીટી_lbr)))
	{
	    ગણતરી++;
	    જો (rex.reg_લીટી_lbr)
		ADVANCE_REGINPUT();
	    નહીંતર
		reg_nextline();
	    scan = rex.આગત;
	    જો (got_int)
		તોડો;
	}
	તોડો;

      મૂળભૂત:			// Oh dear.  Called inappropriately.
	iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	printf("Called regrepeat with op code %d\n", OP(p));
#endif
	તોડો;
    }

    rex.આગત = scan;

    પાછા (પૂર્ણાંક)ગણતરી;
}

/*
 * Push an item onto the regstack.
 * Returns pointer to new item.  Returns NULL when out of memory.
 */
    સ્થિર regitem_T *
regstack_ધકેલો(regstate_T state, char_u *scan)
{
    regitem_T	*rp;

    જો ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
    {
	emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
	પાછા ખાલી;
    }
    જો (GA_GROW_FAILS(&regstack, (પૂર્ણાંક)કદ(regitem_T)))
	પાછા ખાલી;

    rp = (regitem_T *)((અક્ષર *)regstack.ga_data + regstack.ga_len);
    rp->rs_state = state;
    rp->rs_scan = scan;

    regstack.ga_len += કદ(regitem_T);
    પાછા rp;
}

/*
 * Pop an item from the regstack.
 */
    સ્થિર શૂન્ય
regstack_કાઢો(char_u **scan)
{
    regitem_T	*rp;

    rp = (regitem_T *)((અક્ષર *)regstack.ga_data + regstack.ga_len) - 1;
    *scan = rp->rs_scan;

    regstack.ga_len -= કદ(regitem_T);
}

#ifdef FEAT_RELTIME
/*
 * Check if the timer expired, return TRUE if so.
 */
    સ્થિર પૂર્ણાંક
bt_did_time_out(પૂર્ણાંક *timed_out)
{
    જો (*timeout_flag)
    {
	જો (timed_out != ખાલી)
	{
# ifdef FEAT_EVAL
	    જો (!*timed_out)
		ch_log(ખાલી, "BT regexp timed out");
# endif
	    *timed_out = TRUE;
	}
	પાછા TRUE;
    }
    પાછા FALSE;
}
#endif

/*
 * Save the current subexpr to "bp", so that they can be restored
 * later by restore_subexpr().
 */
    સ્થિર શૂન્ય
save_subexpr(regbehind_T *bp)
{
    પૂર્ણાંક i;

    // When "rex.need_clear_subexpr" is set we don't need to save the values,
    // only remember that this flag needs to be set again when restoring.
    bp->save_need_clear_subexpr = rex.need_clear_subexpr;
    જો (rex.need_clear_subexpr)
	પાછા;

    માટે (i = 0; i < NSUBEXP; ++i)
    {
	જો (REG_MULTI)
	{
	    bp->save_start[i].se_u.pos = rex.reg_startpos[i];
	    bp->save_end[i].se_u.pos = rex.reg_endpos[i];
	}
	નહીંતર
	{
	    bp->save_start[i].se_u.ptr = rex.reg_startp[i];
	    bp->save_end[i].se_u.ptr = rex.reg_endp[i];
	}
    }
}

/*
 * Restore the subexpr from "bp".
 */
    સ્થિર શૂન્ય
restore_subexpr(regbehind_T *bp)
{
    પૂર્ણાંક i;

    // Only need to restore saved values when they are not to be cleared.
    rex.need_clear_subexpr = bp->save_need_clear_subexpr;
    જો (rex.need_clear_subexpr)
	પાછા;

    માટે (i = 0; i < NSUBEXP; ++i)
    {
	જો (REG_MULTI)
	{
	    rex.reg_startpos[i] = bp->save_start[i].se_u.pos;
	    rex.reg_endpos[i] = bp->save_end[i].se_u.pos;
	}
	નહીંતર
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
    સ્થિર પૂર્ણાંક
regmatch(
    char_u	*scan,		    // Current node.
    પૂર્ણાંક		*timed_out UNUSED)  // flag set on timeout or NULL
{
  char_u	*next;		// Next node.
  પૂર્ણાંક		op;
  પૂર્ણાંક		c;
  regitem_T	*rp;
  પૂર્ણાંક		no;
  પૂર્ણાંક		status;		// one of the RA_ values:

  // Make "regstack" and "backpos" empty.  They are allocated and freed in
  // bt_regexec_both() to reduce malloc()/free() calls.
  regstack.ga_len = 0;
  backpos.ga_len = 0;

  // Repeat until "regstack" is empty.
  માટે (;;)
  {
    // Some patterns may take a long time to match, e.g., "\([a-z]\+\)\+Q".
    // Allow interrupting them with CTRL-C.
    fast_breakcheck();

#ifdef DEBUG
    જો (scan != ખાલી && regnarrate)
    {
	mch_errmsg((અક્ષર *)regprop(scan));
	mch_errmsg("(\n");
    }
#endif

    // Repeat for items that can be matched sequentially, without using the
    // regstack.
    માટે (;;)
    {
	જો (got_int || scan == ખાલી)
	{
	    status = RA_FAIL;
	    તોડો;
	}
#ifdef FEAT_RELTIME
	જો (bt_did_time_out(timed_out))
	{
	    status = RA_FAIL;
	    તોડો;
	}
#endif
	status = RA_CONT;

#ifdef DEBUG
	જો (regnarrate)
	{
	    mch_errmsg((અક્ષર *)regprop(scan));
	    mch_errmsg("...\n");
# ifdef FEAT_SYN_HL
	    જો (re_extmatch_in != ખાલી)
	    {
		પૂર્ણાંક i;

		mch_errmsg(_("External submatches:\n"));
		માટે (i = 0; i < NSUBEXP; i++)
		{
		    mch_errmsg("    \"");
		    જો (re_extmatch_in->matches[i] != ખાલી)
			mch_errmsg((અક્ષર *)re_extmatch_in->matches[i]);
		    mch_errmsg("\"\n");
		}
	    }
# endif
	}
#endif
	next = regnext(scan);

	op = OP(scan);
	// Check for character class with NL added.
	જો (!rex.reg_લીટી_lbr && WITH_NL(op) && REG_MULTI
			     && *rex.આગત == NUL && rex.lnum <= rex.reg_maxline)
	{
	    reg_nextline();
	}
	નહીંતર જો (rex.reg_લીટી_lbr && WITH_NL(op) && *rex.આગત == '\n')
	{
	    ADVANCE_REGINPUT();
	}
	નહીંતર
	{
	  જો (WITH_NL(op))
	      op -= ADD_NL;
	  જો (has_mbyte)
	      c = (*mb_ptr2char)(rex.આગત);
	  નહીંતર
	      c = *rex.આગત;
	  પસંદગી (op)
	  {
	  સ્થિતિ BOL:
	    જો (rex.આગત != rex.લીટી)
		status = RA_NOMATCH;
	    તોડો;

	  સ્થિતિ EOL:
	    જો (c != NUL)
		status = RA_NOMATCH;
	    તોડો;

	  સ્થિતિ RE_BOF:
	    // We're not at the beginning of the file when below the first
	    // line where we started, not at the start of the line or we
	    // didn't start at the first line of the buffer.
	    જો (rex.lnum != 0 || rex.આગત != rex.લીટી
				       || (REG_MULTI && rex.reg_firstlnum > 1))
		status = RA_NOMATCH;
	    તોડો;

	  સ્થિતિ RE_EOF:
	    જો (rex.lnum != rex.reg_maxline || c != NUL)
		status = RA_NOMATCH;
	    તોડો;

	  સ્થિતિ CURSOR:
	    // Check if the buffer is in a window and compare the
	    // rex.reg_win->w_cursor position to the match position.
	    જો (rex.reg_win == ખાલી
		    || (rex.lnum + rex.reg_firstlnum
						 != rex.reg_win->w_cursor.lnum)
		    || ((colnr_T)(rex.આગત - rex.લીટી)
						 != rex.reg_win->w_cursor.col))
		status = RA_NOMATCH;
	    તોડો;

	  સ્થિતિ RE_MARK:
	    // Compare the mark position to the match position.
	    {
		પૂર્ણાંક	mark = OPERAND(scan)[0];
		પૂર્ણાંક	cmp = OPERAND(scan)[1];
		pos_T	*pos;
		size_t	col = REG_MULTI ? rex.આગત - rex.લીટી : 0;

		pos = getmark_buf(rex.reg_buf, mark, FALSE);

		// Line may have been freed, get it again.
		જો (REG_MULTI)
		{
		    rex.લીટી = reg_getline(rex.lnum);
		    rex.આગત = rex.લીટી + col;
		}

		જો (pos == ખાલી		     // mark doesn't exist
			|| pos->lnum <= 0)   // mark isn't set in reg_buf
		{
		    status = RA_NOMATCH;
		}
		નહીંતર
		{
		    colnr_T pos_col = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->col == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->col;

		    જો ((pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_col == (colnr_T)(rex.આગત - rex.લીટી)
				    ? (cmp == '<' || cmp == '>')
				    : (pos_col < (colnr_T)(rex.આગત - rex.લીટી)
					? cmp != '>'
					: cmp != '<'))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? cmp != '>'
				    : cmp != '<')))
		    status = RA_NOMATCH;
		}
	    }
	    તોડો;

	  સ્થિતિ RE_VISUAL:
	    જો (!reg_match_visual())
		status = RA_NOMATCH;
	    તોડો;

	  સ્થિતિ RE_LNUM:
	    જો (!REG_MULTI || !re_અંક_cmp((long_u)(rex.lnum + rex.reg_firstlnum),
									scan))
		status = RA_NOMATCH;
	    તોડો;

	  સ્થિતિ RE_COL:
	    જો (!re_અંક_cmp((long_u)(rex.આગત - rex.લીટી) + 1, scan))
		status = RA_NOMATCH;
	    તોડો;

	  સ્થિતિ RE_VCOL:
	    {
		win_T	    *wp = rex.reg_win == ખાલી ? curwin : rex.reg_win;
		linenr_T    lnum = REG_MULTI ? rex.reg_firstlnum + rex.lnum : 1;
		long_u	    vcol;

		જો (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_buffer->b_ml.ml_લીટી_ગણતરી))
		    lnum = 1;
		vcol = (long_u)win_linetabsize(wp, lnum, rex.લીટી,
					      (colnr_T)(rex.આગત - rex.લીટી));
		જો (!re_અંક_cmp(vcol + 1, scan))
		    status = RA_NOMATCH;
	    }
	    તોડો;

	  સ્થિતિ BOW:	// \<word; rex.input points to w
	    જો (c == NUL)	// Can't match at end of line
		status = RA_NOMATCH;
	    નહીંતર જો (has_mbyte)
	    {
		પૂર્ણાંક this_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.આગત, rex.reg_buf);
		જો (this_class <= 1)
		    status = RA_NOMATCH;  // not on a word at all
		નહીંતર જો (reg_prev_class() == this_class)
		    status = RA_NOMATCH;  // previous char is in same word
	    }
	    નહીંતર
	    {
		જો (!vim_iswordc_buf(c, rex.reg_buf) || (rex.આગત > rex.લીટી
				&& vim_iswordc_buf(rex.આગત[-1], rex.reg_buf)))
		    status = RA_NOMATCH;
	    }
	    તોડો;

	  સ્થિતિ EOW:	// word\>; rex.input points after d
	    જો (rex.આગત == rex.લીટી)    // Can't match at start of line
		status = RA_NOMATCH;
	    નહીંતર જો (has_mbyte)
	    {
		પૂર્ણાંક this_class, prev_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.આગત, rex.reg_buf);
		prev_class = reg_prev_class();
		જો (this_class == prev_class
			|| prev_class == 0 || prev_class == 1)
		    status = RA_NOMATCH;
	    }
	    નહીંતર
	    {
		જો (!vim_iswordc_buf(rex.આગત[-1], rex.reg_buf)
			|| (rex.આગત[0] != NUL
					   && vim_iswordc_buf(c, rex.reg_buf)))
		    status = RA_NOMATCH;
	    }
	    તોડો; // Matched with EOW

	  સ્થિતિ ANY:
	    // ANY does not match new lines.
	    જો (c == NUL)
		status = RA_NOMATCH;
	    નહીંતર
		ADVANCE_REGINPUT();
	    તોડો;

	  સ્થિતિ IDENT:
	    જો (!vim_isIDc(c))
		status = RA_NOMATCH;
	    નહીંતર
		ADVANCE_REGINPUT();
	    તોડો;

	  સ્થિતિ SIDENT:
	    જો (VIM_ISDIGIT(*rex.આગત) || !vim_isIDc(c))
		status = RA_NOMATCH;
	    નહીંતર
		ADVANCE_REGINPUT();
	    તોડો;

	  સ્થિતિ KWORD:
	    જો (!vim_iswordp_buf(rex.આગત, rex.reg_buf))
		status = RA_NOMATCH;
	    નહીંતર
		ADVANCE_REGINPUT();
	    તોડો;

	  સ્થિતિ SKWORD:
	    જો (VIM_ISDIGIT(*rex.આગત)
				    || !vim_iswordp_buf(rex.આગત, rex.reg_buf))
		status = RA_NOMATCH;
	    નહીંતર
		ADVANCE_REGINPUT();
	    તોડો;

	  સ્થિતિ FNAME:
	    જો (!vim_isfilec(c))
		status = RA_NOMATCH;
	    નહીંતર
		ADVANCE_REGINPUT();
	    તોડો;

	  સ્થિતિ SFNAME:
	    જો (VIM_ISDIGIT(*rex.આગત) || !vim_isfilec(c))
		status = RA_NOMATCH;
	    નહીંતર
		ADVANCE_REGINPUT();
	    તોડો;

	  સ્થિતિ PRINT:
	    જો (!vim_isprintc(PTR2CHAR(rex.આગત)))
		status = RA_NOMATCH;
	    નહીંતર
		ADVANCE_REGINPUT();
	    તોડો;

	  સ્થિતિ SPRINT:
	    જો (VIM_ISDIGIT(*rex.આગત) || !vim_isprintc(PTR2CHAR(rex.આગત)))
		status = RA_NOMATCH;
	    નહીંતર
		ADVANCE_REGINPUT();
	    તોડો;

	  સ્થિતિ WHITE:
	    જો (!VIM_ISWHITE(c))
		status = RA_NOMATCH;
	    નહીંતર
		ADVANCE_REGINPUT();
	    તોડો;

	  સ્થિતિ NWHITE:
	    જો (c == NUL || VIM_ISWHITE(c))
		status = RA_NOMATCH;
	    નહીંતર
		ADVANCE_REGINPUT();
	    તોડો;

	  સ્થિતિ DIGIT:
	    જો (!ri_digit(c))
		status = RA_NOMATCH;
	    નહીંતર
		ADVANCE_REGINPUT();
	    તોડો;

	  સ્થિતિ NDIGIT:
	    જો (c == NUL || ri_digit(c))
		status = RA_NOMATCH;
	    નહીંતર
		ADVANCE_REGINPUT();
	    તોડો;

	  સ્થિતિ HEX:
	    જો (!ri_hex(c))
		status = RA_NOMATCH;
	    નહીંતર
		ADVANCE_REGINPUT();
	    તોડો;

	  સ્થિતિ NHEX:
	    જો (c == NUL || ri_hex(c))
		status = RA_NOMATCH;
	    નહીંતર
		ADVANCE_REGINPUT();
	    તોડો;

	  સ્થિતિ OCTAL:
	    જો (!ri_octal(c))
		status = RA_NOMATCH;
	    નહીંતર
		ADVANCE_REGINPUT();
	    તોડો;

	  સ્થિતિ NOCTAL:
	    જો (c == NUL || ri_octal(c))
		status = RA_NOMATCH;
	    નહીંતર
		ADVANCE_REGINPUT();
	    તોડો;

	  સ્થિતિ WORD:
	    જો (!ri_word(c))
		status = RA_NOMATCH;
	    નહીંતર
		ADVANCE_REGINPUT();
	    તોડો;

	  સ્થિતિ NWORD:
	    જો (c == NUL || ri_word(c))
		status = RA_NOMATCH;
	    નહીંતર
		ADVANCE_REGINPUT();
	    તોડો;

	  સ્થિતિ HEAD:
	    જો (!ri_head(c))
		status = RA_NOMATCH;
	    નહીંતર
		ADVANCE_REGINPUT();
	    તોડો;

	  સ્થિતિ NHEAD:
	    જો (c == NUL || ri_head(c))
		status = RA_NOMATCH;
	    નહીંતર
		ADVANCE_REGINPUT();
	    તોડો;

	  સ્થિતિ ALPHA:
	    જો (!ri_alpha(c))
		status = RA_NOMATCH;
	    નહીંતર
		ADVANCE_REGINPUT();
	    તોડો;

	  સ્થિતિ NALPHA:
	    જો (c == NUL || ri_alpha(c))
		status = RA_NOMATCH;
	    નહીંતર
		ADVANCE_REGINPUT();
	    તોડો;

	  સ્થિતિ LOWER:
	    જો (!ri_lower(c))
		status = RA_NOMATCH;
	    નહીંતર
		ADVANCE_REGINPUT();
	    તોડો;

	  સ્થિતિ NLOWER:
	    જો (c == NUL || ri_lower(c))
		status = RA_NOMATCH;
	    નહીંતર
		ADVANCE_REGINPUT();
	    તોડો;

	  સ્થિતિ UPPER:
	    જો (!ri_upper(c))
		status = RA_NOMATCH;
	    નહીંતર
		ADVANCE_REGINPUT();
	    તોડો;

	  સ્થિતિ NUPPER:
	    જો (c == NUL || ri_upper(c))
		status = RA_NOMATCH;
	    નહીંતર
		ADVANCE_REGINPUT();
	    તોડો;

	  સ્થિતિ EXACTLY:
	    {
		પૂર્ણાંક	len;
		char_u	*opnd;

		opnd = OPERAND(scan);
		// Inline the first byte, for speed.
		જો (*opnd != *rex.આગત
			&& (!rex.reg_ic
			    || (!enc_utf8
			      && MB_TOLOWER(*opnd) != MB_TOLOWER(*rex.આગત))))
		    status = RA_NOMATCH;
		નહીંતર જો (*opnd == NUL)
		{
		    // match empty string always works; happens when "~" is
		    // empty.
		}
		નહીંતર
		{
		    જો (opnd[1] == NUL && !(enc_utf8 && rex.reg_ic))
		    {
			len = 1;	// matched a single byte above
		    }
		    નહીંતર
		    {
			// Need to match first byte again for multi-byte.
			len = (પૂર્ણાંક)STRLEN(opnd);
			જો (cstrncmp(opnd, rex.આગત, &len) != 0)
			    status = RA_NOMATCH;
		    }
		    // Check for following composing character, unless %C
		    // follows (skips over all composing chars).
		    જો (status != RA_NOMATCH
			    && enc_utf8
			    && UTF_COMPOSINGLIKE(rex.આગત, rex.આગત + len)
			    && !rex.reg_icombine
			    && OP(next) != RE_COMPOSING)
		    {
			// raaron: This code makes a composing character get
			// ignored, which is the correct behavior (sometimes)
			// for voweled Hebrew texts.
			status = RA_NOMATCH;
		    }
		    જો (status != RA_NOMATCH)
			rex.આગત += len;
		}
	    }
	    તોડો;

	  સ્થિતિ ANYOF:
	  સ્થિતિ ANYBUT:
	    {
		char_u  *q = OPERAND(scan);

		જો (c == NUL)
		    status = RA_NOMATCH;
		નહીંતર જો ((cstrchr(q, c) == ખાલી) == (op == ANYOF))
		    status = RA_NOMATCH;
		નહીંતર
		{
		    // Check following combining characters
		    પૂર્ણાંક	len = 0;
		    પૂર્ણાંક i;

		    જો (enc_utf8)
			len = utfc_ptr2len(q) - utf_ptr2len(q);

		    MB_CPTR_ADV(rex.આગત);
		    MB_CPTR_ADV(q);

		    જો (!enc_utf8 || len == 0)
			તોડો;

		    માટે (i = 0; i < len; ++i)
			જો (q[i] != rex.આગત[i])
			{
			    status = RA_NOMATCH;
			    તોડો;
			}
		    rex.આગત += len;
		}
		તોડો;
	    }

	  સ્થિતિ MULTIBYTECODE:
	    જો (has_mbyte)
	    {
		પૂર્ણાંક	i, len;
		char_u	*opnd;
		પૂર્ણાંક	opndc = 0, inpc;

		opnd = OPERAND(scan);
		// Safety check (just in case 'encoding' was changed since
		// compiling the program).
		જો ((len = (*mb_ptr2len)(opnd)) < 2)
		{
		    status = RA_NOMATCH;
		    તોડો;
		}
		જો (enc_utf8)
		    opndc = utf_ptr2char(opnd);
		જો (enc_utf8 && utf_iscomposing(opndc))
		{
		    // When only a composing char is given match at any
		    // position where that composing char appears.
		    status = RA_NOMATCH;
		    માટે (i = 0; rex.આગત[i] != NUL;
						i += utf_ptr2len(rex.આગત + i))
		    {
			inpc = utf_ptr2char(rex.આગત + i);
			જો (!utf_iscomposing(inpc))
			{
			    જો (i > 0)
				તોડો;
			}
			નહીંતર જો (opndc == inpc)
			{
			    // Include all following composing chars.
			    len = i + utfc_ptr2len(rex.આગત + i);
			    status = RA_MATCH;
			    તોડો;
			}
		    }
		}
		નહીંતર જો (enc_utf8)
		{
		    જો (cstrncmp(opnd, rex.આગત, &len) != 0)
		    {
			status = RA_NOMATCH;
			તોડો;
		    }
		}
		નહીંતર
		    માટે (i = 0; i < len; ++i)
			જો (opnd[i] != rex.આગત[i])
			{
			    status = RA_NOMATCH;
			    તોડો;
			}
		rex.આગત += len;
	    }
	    નહીંતર
		status = RA_NOMATCH;
	    તોડો;
	  સ્થિતિ RE_COMPOSING:
	    જો (enc_utf8)
	    {
		// Skip composing characters.
		જ્યારે (utf_iscomposing(utf_ptr2char(rex.આગત)))
		    MB_CPTR_ADV(rex.આગત);
	    }
	    તોડો;

	  સ્થિતિ NOTHING:
	    તોડો;

	  સ્થિતિ BACK:
	    {
		પૂર્ણાંક		i;
		backpos_T	*bp;

		// When we run into BACK we need to check if we don't keep
		// looping without matching any input.  The second and later
		// times a BACK is encountered it fails if the input is still
		// at the same position as the previous time.
		// The positions are stored in "backpos" and found by the
		// current value of "scan", the position in the RE program.
		bp = (backpos_T *)backpos.ga_data;
		માટે (i = 0; i < backpos.ga_len; ++i)
		    જો (bp[i].bp_scan == scan)
			તોડો;
		જો (i == backpos.ga_len)
		{
		    // First time at this BACK, make room to store the pos.
		    જો (GA_GROW_FAILS(&backpos, 1))
			status = RA_FAIL;
		    નહીંતર
		    {
			// get "ga_data" again, it may have changed
			bp = (backpos_T *)backpos.ga_data;
			bp[i].bp_scan = scan;
			++backpos.ga_len;
		    }
		}
		નહીંતર જો (reg_save_equal(&bp[i].bp_pos))
		    // Still at same position as last time, fail.
		    status = RA_NOMATCH;

		જો (status != RA_FAIL && status != RA_NOMATCH)
		    reg_save(&bp[i].bp_pos, &backpos);
	    }
	    તોડો;

	  સ્થિતિ MOPEN + 0:   // Match start: \zs
	  સ્થિતિ MOPEN + 1:   // \(
	  સ્થિતિ MOPEN + 2:
	  સ્થિતિ MOPEN + 3:
	  સ્થિતિ MOPEN + 4:
	  સ્થિતિ MOPEN + 5:
	  સ્થિતિ MOPEN + 6:
	  સ્થિતિ MOPEN + 7:
	  સ્થિતિ MOPEN + 8:
	  સ્થિતિ MOPEN + 9:
	    {
		no = op - MOPEN;
		cleanup_subexpr();
		rp = regstack_ધકેલો(RS_MOPEN, scan);
		જો (rp == ખાલી)
		    status = RA_FAIL;
		નહીંતર
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_startpos[no],
							  &rex.reg_startp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    તોડો;

	  સ્થિતિ NOPEN:	    // \%(
	  સ્થિતિ NCLOSE:	    // \) after \%(
		જો (regstack_ધકેલો(RS_NOPEN, scan) == ખાલી)
		    status = RA_FAIL;
		// We simply continue and handle the result when done.
		તોડો;

#ifdef FEAT_SYN_HL
	  સ્થિતિ ZOPEN + 1:
	  સ્થિતિ ZOPEN + 2:
	  સ્થિતિ ZOPEN + 3:
	  સ્થિતિ ZOPEN + 4:
	  સ્થિતિ ZOPEN + 5:
	  સ્થિતિ ZOPEN + 6:
	  સ્થિતિ ZOPEN + 7:
	  સ્થિતિ ZOPEN + 8:
	  સ્થિતિ ZOPEN + 9:
	    {
		no = op - ZOPEN;
		cleanup_zsubexpr();
		rp = regstack_ધકેલો(RS_ZOPEN, scan);
		જો (rp == ખાલી)
		    status = RA_FAIL;
		નહીંતર
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_startzpos[no],
							     &reg_startzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    તોડો;
#endif

	  સ્થિતિ MCLOSE + 0:  // Match end: \ze
	  સ્થિતિ MCLOSE + 1:  // \)
	  સ્થિતિ MCLOSE + 2:
	  સ્થિતિ MCLOSE + 3:
	  સ્થિતિ MCLOSE + 4:
	  સ્થિતિ MCLOSE + 5:
	  સ્થિતિ MCLOSE + 6:
	  સ્થિતિ MCLOSE + 7:
	  સ્થિતિ MCLOSE + 8:
	  સ્થિતિ MCLOSE + 9:
	    {
		no = op - MCLOSE;
		cleanup_subexpr();
		rp = regstack_ધકેલો(RS_MCLOSE, scan);
		જો (rp == ખાલી)
		    status = RA_FAIL;
		નહીંતર
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_endpos[no],
							    &rex.reg_endp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    તોડો;

#ifdef FEAT_SYN_HL
	  સ્થિતિ ZCLOSE + 1:  // \) after \z(
	  સ્થિતિ ZCLOSE + 2:
	  સ્થિતિ ZCLOSE + 3:
	  સ્થિતિ ZCLOSE + 4:
	  સ્થિતિ ZCLOSE + 5:
	  સ્થિતિ ZCLOSE + 6:
	  સ્થિતિ ZCLOSE + 7:
	  સ્થિતિ ZCLOSE + 8:
	  સ્થિતિ ZCLOSE + 9:
	    {
		no = op - ZCLOSE;
		cleanup_zsubexpr();
		rp = regstack_ધકેલો(RS_ZCLOSE, scan);
		જો (rp == ખાલી)
		    status = RA_FAIL;
		નહીંતર
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_endzpos[no],
							      &reg_endzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    તોડો;
#endif

	  સ્થિતિ BACKREF + 1:
	  સ્થિતિ BACKREF + 2:
	  સ્થિતિ BACKREF + 3:
	  સ્થિતિ BACKREF + 4:
	  સ્થિતિ BACKREF + 5:
	  સ્થિતિ BACKREF + 6:
	  સ્થિતિ BACKREF + 7:
	  સ્થિતિ BACKREF + 8:
	  સ્થિતિ BACKREF + 9:
	    {
		પૂર્ણાંક		len;

		no = op - BACKREF;
		cleanup_subexpr();
		જો (!REG_MULTI)		// Single-line regexp
		{
		    જો (rex.reg_startp[no] == ખાલી || rex.reg_endp[no] == ખાલી)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    નહીંતર
		    {
			// Compare current input with back-ref in the same
			// line.
			len = (પૂર્ણાંક)(rex.reg_endp[no] - rex.reg_startp[no]);
			જો (cstrncmp(rex.reg_startp[no], rex.આગત, &len) != 0)
			    status = RA_NOMATCH;
		    }
		}
		નહીંતર				// Multi-line regexp
		{
		    જો (rex.reg_startpos[no].lnum < 0
						|| rex.reg_endpos[no].lnum < 0)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    નહીંતર
		    {
			જો (rex.reg_startpos[no].lnum == rex.lnum
				&& rex.reg_endpos[no].lnum == rex.lnum)
			{
			    // Compare back-ref within the current line.
			    len = rex.reg_endpos[no].col
						    - rex.reg_startpos[no].col;
			    જો (cstrncmp(rex.લીટી + rex.reg_startpos[no].col,
							  rex.આગત, &len) != 0)
				status = RA_NOMATCH;
			}
			નહીંતર
			{
			    // Messy situation: Need to compare between two
			    // lines.
			    પૂર્ણાંક r = match_with_backref(
					    rex.reg_startpos[no].lnum,
					    rex.reg_startpos[no].col,
					    rex.reg_endpos[no].lnum,
					    rex.reg_endpos[no].col,
					    &len);

			    જો (r != RA_MATCH)
				status = r;
			}
		    }
		}

		// Matched the backref, skip over it.
		rex.આગત += len;
	    }
	    તોડો;

#ifdef FEAT_SYN_HL
	  સ્થિતિ ZREF + 1:
	  સ્થિતિ ZREF + 2:
	  સ્થિતિ ZREF + 3:
	  સ્થિતિ ZREF + 4:
	  સ્થિતિ ZREF + 5:
	  સ્થિતિ ZREF + 6:
	  સ્થિતિ ZREF + 7:
	  સ્થિતિ ZREF + 8:
	  સ્થિતિ ZREF + 9:
	    {
		પૂર્ણાંક	len;

		cleanup_zsubexpr();
		no = op - ZREF;
		જો (re_extmatch_in != ખાલી
			&& re_extmatch_in->matches[no] != ખાલી)
		{
		    len = (પૂર્ણાંક)STRLEN(re_extmatch_in->matches[no]);
		    જો (cstrncmp(re_extmatch_in->matches[no],
							  rex.આગત, &len) != 0)
			status = RA_NOMATCH;
		    નહીંતર
			rex.આગત += len;
		}
		નહીંતર
		{
		    // Backref was not set: Match an empty string.
		}
	    }
	    તોડો;
#endif

	  સ્થિતિ BRANCH:
	    {
		જો (OP(next) != BRANCH) // No choice.
		    next = OPERAND(scan);	// Avoid recursion.
		નહીંતર
		{
		    rp = regstack_ધકેલો(RS_BRANCH, scan);
		    જો (rp == ખાલી)
			status = RA_FAIL;
		    નહીંતર
			status = RA_BREAK;	// rest is below
		}
	    }
	    તોડો;

	  સ્થિતિ BRACE_LIMITS:
	    {
		જો (OP(next) == BRACE_SIMPLE)
		{
		    bl_minval = OPERAND_MIN(scan);
		    bl_maxval = OPERAND_MAX(scan);
		}
		નહીંતર જો (OP(next) >= BRACE_COMPLEX
			&& OP(next) < BRACE_COMPLEX + 10)
		{
		    no = OP(next) - BRACE_COMPLEX;
		    brace_min[no] = OPERAND_MIN(scan);
		    brace_max[no] = OPERAND_MAX(scan);
		    brace_ગણતરી[no] = 0;
		}
		નહીંતર
		{
		    internal_error("BRACE_LIMITS");
		    status = RA_FAIL;
		}
	    }
	    તોડો;

	  સ્થિતિ BRACE_COMPLEX + 0:
	  સ્થિતિ BRACE_COMPLEX + 1:
	  સ્થિતિ BRACE_COMPLEX + 2:
	  સ્થિતિ BRACE_COMPLEX + 3:
	  સ્થિતિ BRACE_COMPLEX + 4:
	  સ્થિતિ BRACE_COMPLEX + 5:
	  સ્થિતિ BRACE_COMPLEX + 6:
	  સ્થિતિ BRACE_COMPLEX + 7:
	  સ્થિતિ BRACE_COMPLEX + 8:
	  સ્થિતિ BRACE_COMPLEX + 9:
	    {
		no = op - BRACE_COMPLEX;
		++brace_ગણતરી[no];

		// If not matched enough times yet, try one more
		જો (brace_ગણતરી[no] <= (brace_min[no] <= brace_max[no]
					     ? brace_min[no] : brace_max[no]))
		{
		    rp = regstack_ધકેલો(RS_BRCPLX_MORE, scan);
		    જો (rp == ખાલી)
			status = RA_FAIL;
		    નહીંતર
		    {
			rp->rs_no = no;
			reg_save(&rp->rs_un.regsave, &backpos);
			next = OPERAND(scan);
			// We continue and handle the result when done.
		    }
		    તોડો;
		}

		// If matched enough times, may try matching some more
		જો (brace_min[no] <= brace_max[no])
		{
		    // Range is the normal way around, use longest match
		    જો (brace_ગણતરી[no] <= brace_max[no])
		    {
			rp = regstack_ધકેલો(RS_BRCPLX_LONG, scan);
			જો (rp == ખાલી)
			    status = RA_FAIL;
			નહીંતર
			{
			    rp->rs_no = no;
			    reg_save(&rp->rs_un.regsave, &backpos);
			    next = OPERAND(scan);
			    // We continue and handle the result when done.
			}
		    }
		}
		નહીંતર
		{
		    // Range is backwards, use shortest match first
		    જો (brace_ગણતરી[no] <= brace_min[no])
		    {
			rp = regstack_ધકેલો(RS_BRCPLX_SHORT, scan);
			જો (rp == ખાલી)
			    status = RA_FAIL;
			નહીંતર
			{
			    reg_save(&rp->rs_un.regsave, &backpos);
			    // We continue and handle the result when done.
			}
		    }
		}
	    }
	    તોડો;

	  સ્થિતિ BRACE_SIMPLE:
	  સ્થિતિ STAR:
	  સ્થિતિ PLUS:
	    {
		regstar_T	rst;

		// Lookahead to avoid useless match attempts when we know
		// what character comes next.
		જો (OP(next) == EXACTLY)
		{
		    rst.nextb = *OPERAND(next);
		    જો (rex.reg_ic)
		    {
			જો (MB_ISUPPER(rst.nextb))
			    rst.nextb_ic = MB_TOLOWER(rst.nextb);
			નહીંતર
			    rst.nextb_ic = MB_TOUPPER(rst.nextb);
		    }
		    નહીંતર
			rst.nextb_ic = rst.nextb;
		}
		નહીંતર
		{
		    rst.nextb = NUL;
		    rst.nextb_ic = NUL;
		}
		જો (op != BRACE_SIMPLE)
		{
		    rst.minval = (op == STAR) ? 0 : 1;
		    rst.maxval = MAX_LIMIT;
		}
		નહીંતર
		{
		    rst.minval = bl_minval;
		    rst.maxval = bl_maxval;
		}

		// When maxval > minval, try matching as much as possible, up
		// to maxval.  When maxval < minval, try matching at least the
		// minimal number (since the range is backwards, that's also
		// maxval!).
		rst.ગણતરી = regrepeat(OPERAND(scan), rst.maxval);
		જો (got_int)
		{
		    status = RA_FAIL;
		    તોડો;
		}
		જો (rst.minval <= rst.maxval
			  ? rst.ગણતરી >= rst.minval : rst.ગણતરી >= rst.maxval)
		{
		    // It could match.  Prepare for trying to match what
		    // follows.  The code is below.  Parameters are stored in
		    // a regstar_T on the regstack.
		    જો ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
		    {
			emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
			status = RA_FAIL;
		    }
		    નહીંતર જો (GA_GROW_FAILS(&regstack, (પૂર્ણાંક)કદ(regstar_T)))
			status = RA_FAIL;
		    નહીંતર
		    {
			regstack.ga_len += કદ(regstar_T);
			rp = regstack_ધકેલો(rst.minval <= rst.maxval
					? RS_STAR_LONG : RS_STAR_SHORT, scan);
			જો (rp == ખાલી)
			    status = RA_FAIL;
			નહીંતર
			{
			    *(((regstar_T *)rp) - 1) = rst;
			    status = RA_BREAK;	    // skip the restore bits
			}
		    }
		}
		નહીંતર
		    status = RA_NOMATCH;

	    }
	    તોડો;

	  સ્થિતિ NOMATCH:
	  સ્થિતિ MATCH:
	  સ્થિતિ SUBPAT:
	    rp = regstack_ધકેલો(RS_NOMATCH, scan);
	    જો (rp == ખાલી)
		status = RA_FAIL;
	    નહીંતર
	    {
		rp->rs_no = op;
		reg_save(&rp->rs_un.regsave, &backpos);
		next = OPERAND(scan);
		// We continue and handle the result when done.
	    }
	    તોડો;

	  સ્થિતિ BEHIND:
	  સ્થિતિ NOBEHIND:
	    // Need a bit of room to store extra positions.
	    જો ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
	    {
		emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		status = RA_FAIL;
	    }
	    નહીંતર જો (GA_GROW_FAILS(&regstack, (પૂર્ણાંક)કદ(regbehind_T)))
		status = RA_FAIL;
	    નહીંતર
	    {
		regstack.ga_len += કદ(regbehind_T);
		rp = regstack_ધકેલો(RS_BEHIND1, scan);
		જો (rp == ખાલી)
		    status = RA_FAIL;
		નહીંતર
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
	    તોડો;

	  સ્થિતિ BHPOS:
	    જો (REG_MULTI)
	    {
		જો (behind_pos.rs_u.pos.col != (colnr_T)(rex.આગત - rex.લીટી)
			|| behind_pos.rs_u.pos.lnum != rex.lnum)
		    status = RA_NOMATCH;
	    }
	    નહીંતર જો (behind_pos.rs_u.ptr != rex.આગત)
		status = RA_NOMATCH;
	    તોડો;

	  સ્થિતિ NEWL:
	    જો ((c != NUL || !REG_MULTI || rex.lnum > rex.reg_maxline
			     || rex.reg_લીટી_lbr)
					   && (c != '\n' || !rex.reg_લીટી_lbr))
		status = RA_NOMATCH;
	    નહીંતર જો (rex.reg_લીટી_lbr)
		ADVANCE_REGINPUT();
	    નહીંતર
		reg_nextline();
	    તોડો;

	  સ્થિતિ END:
	    status = RA_MATCH;	// Success!
	    તોડો;

	  મૂળભૂત:
	    iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	    printf("Illegal op code %d\n", op);
#endif
	    status = RA_FAIL;
	    તોડો;
	  }
	}

	// If we can't continue sequentially, break the inner loop.
	જો (status != RA_CONT)
	    તોડો;

	// Continue in inner loop, advance to next item.
	scan = next;

    } // end of inner loop

    // If there is something on the regstack execute the code for the state.
    // If the state is popped then loop and use the older state.
    જ્યારે (regstack.ga_len > 0 && status != RA_FAIL)
    {
	rp = (regitem_T *)((અક્ષર *)regstack.ga_data + regstack.ga_len) - 1;
	પસંદગી (rp->rs_state)
	{
	  સ્થિતિ RS_NOPEN:
	    // Result is passed on as-is, simply pop the state.
	    regstack_કાઢો(&scan);
	    તોડો;

	  સ્થિતિ RS_MOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    જો (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_startpos[rp->rs_no],
						  &rex.reg_startp[rp->rs_no]);
	    regstack_કાઢો(&scan);
	    તોડો;

#ifdef FEAT_SYN_HL
	  સ્થિતિ RS_ZOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    જો (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_startzpos[rp->rs_no],
						 &reg_startzp[rp->rs_no]);
	    regstack_કાઢો(&scan);
	    તોડો;
#endif

	  સ્થિતિ RS_MCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    જો (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_endpos[rp->rs_no],
						    &rex.reg_endp[rp->rs_no]);
	    regstack_કાઢો(&scan);
	    તોડો;

#ifdef FEAT_SYN_HL
	  સ્થિતિ RS_ZCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    જો (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_endzpos[rp->rs_no],
						   &reg_endzp[rp->rs_no]);
	    regstack_કાઢો(&scan);
	    તોડો;
#endif

	  સ્થિતિ RS_BRANCH:
	    જો (status == RA_MATCH)
		// this branch matched, use it
		regstack_કાઢો(&scan);
	    નહીંતર
	    {
		જો (status != RA_BREAK)
		{
		    // After a non-matching branch: try next one.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    scan = rp->rs_scan;
		}
		જો (scan == ખાલી || OP(scan) != BRANCH)
		{
		    // no more branches, didn't find a match
		    status = RA_NOMATCH;
		    regstack_કાઢો(&scan);
		}
		નહીંતર
		{
		    // Prepare to try a branch.
		    rp->rs_scan = regnext(scan);
		    reg_save(&rp->rs_un.regsave, &backpos);
		    scan = OPERAND(scan);
		}
	    }
	    તોડો;

	  સ્થિતિ RS_BRCPLX_MORE:
	    // Pop the state.  Restore pointers when there is no match.
	    જો (status == RA_NOMATCH)
	    {
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_ગણતરી[rp->rs_no];	// decrement match count
	    }
	    regstack_કાઢો(&scan);
	    તોડો;

	  સ્થિતિ RS_BRCPLX_LONG:
	    // Pop the state.  Restore pointers when there is no match.
	    જો (status == RA_NOMATCH)
	    {
		// There was no match, but we did find enough matches.
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_ગણતરી[rp->rs_no];
		// continue with the items after "\{}"
		status = RA_CONT;
	    }
	    regstack_કાઢો(&scan);
	    જો (status == RA_CONT)
		scan = regnext(scan);
	    તોડો;

	  સ્થિતિ RS_BRCPLX_SHORT:
	    // Pop the state.  Restore pointers when there is no match.
	    જો (status == RA_NOMATCH)
		// There was no match, try to match one more item.
		reg_restore(&rp->rs_un.regsave, &backpos);
	    regstack_કાઢો(&scan);
	    જો (status == RA_NOMATCH)
	    {
		scan = OPERAND(scan);
		status = RA_CONT;
	    }
	    તોડો;

	  સ્થિતિ RS_NOMATCH:
	    // Pop the state.  If the operand matches for NOMATCH or
	    // doesn't match for MATCH/SUBPAT, we fail.  Otherwise backup,
	    // except for SUBPAT, and continue with the next item.
	    જો (status == (rp->rs_no == NOMATCH ? RA_MATCH : RA_NOMATCH))
		status = RA_NOMATCH;
	    નહીંતર
	    {
		status = RA_CONT;
		જો (rp->rs_no != SUBPAT)	// zero-width
		    reg_restore(&rp->rs_un.regsave, &backpos);
	    }
	    regstack_કાઢો(&scan);
	    જો (status == RA_CONT)
		scan = regnext(scan);
	    તોડો;

	  સ્થિતિ RS_BEHIND1:
	    જો (status == RA_NOMATCH)
	    {
		regstack_કાઢો(&scan);
		regstack.ga_len -= કદ(regbehind_T);
	    }
	    નહીંતર
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
	    તોડો;

	  સ્થિતિ RS_BEHIND2:
	    // Looping for BEHIND / NOBEHIND match.
	    જો (status == RA_MATCH && reg_save_equal(&behind_pos))
	    {
		// found a match that ends where "next" started
		behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		જો (rp->rs_no == BEHIND)
		    reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
		નહીંતર
		{
		    // But we didn't want a match.  Need to restore the
		    // subexpr, because what follows matched, so they have
		    // been set.
		    status = RA_NOMATCH;
		    restore_subexpr(((regbehind_T *)rp) - 1);
		}
		regstack_કાઢો(&scan);
		regstack.ga_len -= કદ(regbehind_T);
	    }
	    નહીંતર
	    {
		long limit;

		// No match or a match that doesn't end where we want it: Go
		// back one character.  May go to previous line once.
		no = OK;
		limit = OPERAND_MIN(rp->rs_scan);
		જો (REG_MULTI)
		{
		    જો (limit > 0
			    && ((rp->rs_un.regsave.rs_u.pos.lnum
						    < behind_pos.rs_u.pos.lnum
				    ? (colnr_T)STRLEN(rex.લીટી)
				    : behind_pos.rs_u.pos.col)
				- rp->rs_un.regsave.rs_u.pos.col >= limit))
			no = FAIL;
		    નહીંતર જો (rp->rs_un.regsave.rs_u.pos.col == 0)
		    {
			જો (rp->rs_un.regsave.rs_u.pos.lnum
					< behind_pos.rs_u.pos.lnum
				|| reg_getline(
					--rp->rs_un.regsave.rs_u.pos.lnum)
								  == ખાલી)
			    no = FAIL;
			નહીંતર
			{
			    reg_restore(&rp->rs_un.regsave, &backpos);
			    rp->rs_un.regsave.rs_u.pos.col =
						 (colnr_T)STRLEN(rex.લીટી);
			}
		    }
		    નહીંતર
		    {
			જો (has_mbyte)
			{
			    char_u *લીટી =
				  reg_getline(rp->rs_un.regsave.rs_u.pos.lnum);

			    rp->rs_un.regsave.rs_u.pos.col -=
				(*mb_head_off)(લીટી, લીટી
				    + rp->rs_un.regsave.rs_u.pos.col - 1) + 1;
			}
			નહીંતર
			    --rp->rs_un.regsave.rs_u.pos.col;
		    }
		}
		નહીંતર
		{
		    જો (rp->rs_un.regsave.rs_u.ptr == rex.લીટી)
			no = FAIL;
		    નહીંતર
		    {
			MB_PTR_BACK(rex.લીટી, rp->rs_un.regsave.rs_u.ptr);
			જો (limit > 0 && (long)(behind_pos.rs_u.ptr
				     - rp->rs_un.regsave.rs_u.ptr) > limit)
			    no = FAIL;
		    }
		}
		જો (no == OK)
		{
		    // Advanced, prepare for finding match again.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    scan = OPERAND(rp->rs_scan) + 4;
		    જો (status == RA_MATCH)
		    {
			// We did match, so subexpr may have been changed,
			// need to restore them for the next try.
			status = RA_NOMATCH;
			restore_subexpr(((regbehind_T *)rp) - 1);
		    }
		}
		નહીંતર
		{
		    // Can't advance.  For NOBEHIND that's a match.
		    behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		    જો (rp->rs_no == NOBEHIND)
		    {
			reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
			status = RA_MATCH;
		    }
		    નહીંતર
		    {
			// We do want a proper match.  Need to restore the
			// subexpr if we had a match, because they may have
			// been set.
			જો (status == RA_MATCH)
			{
			    status = RA_NOMATCH;
			    restore_subexpr(((regbehind_T *)rp) - 1);
			}
		    }
		    regstack_કાઢો(&scan);
		    regstack.ga_len -= કદ(regbehind_T);
		}
	    }
	    તોડો;

	  સ્થિતિ RS_STAR_LONG:
	  સ્થિતિ RS_STAR_SHORT:
	    {
		regstar_T	    *rst = ((regstar_T *)rp) - 1;

		જો (status == RA_MATCH)
		{
		    regstack_કાઢો(&scan);
		    regstack.ga_len -= કદ(regstar_T);
		    તોડો;
		}

		// Tried once already, restore input pointers.
		જો (status != RA_BREAK)
		    reg_restore(&rp->rs_un.regsave, &backpos);

		// Repeat until we found a position where it could match.
		માટે (;;)
		{
		    જો (status != RA_BREAK)
		    {
			// Tried first position already, advance.
			જો (rp->rs_state == RS_STAR_LONG)
			{
			    // Trying for longest match, but couldn't or
			    // didn't match -- back up one char.
			    જો (--rst->ગણતરી < rst->minval)
				તોડો;
			    જો (rex.આગત == rex.લીટી)
			    {
				// backup to last char of previous line
				જો (rex.lnum == 0)
				{
				    status = RA_NOMATCH;
				    તોડો;
				}
				--rex.lnum;
				rex.લીટી = reg_getline(rex.lnum);
				// Just in case regrepeat() didn't count
				// right.
				જો (rex.લીટી == ખાલી)
				    તોડો;
				rex.આગત = rex.લીટી + reg_getline_len(rex.lnum);
				fast_breakcheck();
			    }
			    નહીંતર
				MB_PTR_BACK(rex.લીટી, rex.આગત);
			}
			નહીંતર
			{
			    // Range is backwards, use shortest match first.
			    // Careful: maxval and minval are exchanged!
			    // Couldn't or didn't match: try advancing one
			    // char.
			    જો (rst->ગણતરી == rst->minval
				  || regrepeat(OPERAND(rp->rs_scan), 1L) == 0)
				તોડો;
			    ++rst->ગણતરી;
			}
			જો (got_int)
			    તોડો;
		    }
		    નહીંતર
			status = RA_NOMATCH;

		    // If it could match, try it.
		    જો (rst->nextb == NUL || *rex.આગત == rst->nextb
					     || *rex.આગત == rst->nextb_ic)
		    {
			reg_save(&rp->rs_un.regsave, &backpos);
			scan = regnext(rp->rs_scan);
			status = RA_CONT;
			તોડો;
		    }
		}
		જો (status != RA_CONT)
		{
		    // Failed.
		    regstack_કાઢો(&scan);
		    regstack.ga_len -= કદ(regstar_T);
		    status = RA_NOMATCH;
		}
	    }
	    તોડો;
	}

	// If we want to continue the inner loop or didn't pop a state
	// continue matching loop
	જો (status == RA_CONT || rp == (regitem_T *)
			     ((અક્ષર *)regstack.ga_data + regstack.ga_len) - 1)
	    તોડો;

#ifdef FEAT_RELTIME
	જો (bt_did_time_out(timed_out))
	{
	    status = RA_FAIL;
	    તોડો;
	}
#endif
    }

    // May need to continue with the inner loop, starting at "scan".
    જો (status == RA_CONT)
	ચાલુ;

    // If the regstack is empty or something failed we are done.
    જો (regstack.ga_len == 0 || status == RA_FAIL)
    {
	જો (scan == ખાલી)
	{
	    // We get here only if there's trouble -- normally "case END" is
	    // the terminating point.
	    iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	    printf("Premature EOL\n");
#endif
	}
	પાછા (status == RA_MATCH);
    }

  } // End of loop until the regstack is empty.

  // NOTREACHED
}

/*
 * regtry - try match of "prog" with at rex.line["col"].
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    સ્થિર long
regtry(
    bt_regprog_T	*prog,
    colnr_T		col,
    પૂર્ણાંક			*timed_out)	// flag set on timeout or NULL
{
    rex.આગત = rex.લીટી + col;
    rex.need_clear_subexpr = TRUE;
#ifdef FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    rex.need_clear_zsubexpr = (prog->reghasz == REX_SET);
#endif

    જો (regmatch(prog->program + 1, timed_out) == 0)
	પાછા 0;

    cleanup_subexpr();
    જો (REG_MULTI)
    {
	જો (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].col = col;
	}
	જો (rex.reg_endpos[0].lnum < 0)
	{
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].col = (પૂર્ણાંક)(rex.આગત - rex.લીટી);
	}
	નહીંતર
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    નહીંતર
    {
	જો (rex.reg_startp[0] == ખાલી)
	    rex.reg_startp[0] = rex.લીટી + col;
	જો (rex.reg_endp[0] == ખાલી)
	    rex.reg_endp[0] = rex.આગત;
    }
#ifdef FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = ખાલી;

    જો (prog->reghasz == REX_SET)
    {
	પૂર્ણાંક		i;

	cleanup_zsubexpr();
	re_extmatch_out = make_extmatch();
	જો (re_extmatch_out == ખાલી)
	    પાછા 0;
	માટે (i = 0; i < NSUBEXP; i++)
	{
	    જો (REG_MULTI)
	    {
		// Only accept single line matches.
		જો (reg_startzpos[i].lnum >= 0
			&& reg_endzpos[i].lnum == reg_startzpos[i].lnum
			&& reg_endzpos[i].col >= reg_startzpos[i].col)
		    re_extmatch_out->matches[i] =
			vim_strnsave(reg_getline(reg_startzpos[i].lnum)
						       + reg_startzpos[i].col,
				   reg_endzpos[i].col - reg_startzpos[i].col);
	    }
	    નહીંતર
	    {
		જો (reg_startzp[i] != ખાલી && reg_endzp[i] != ખાલી)
		    re_extmatch_out->matches[i] =
			    vim_strnsave(reg_startzp[i],
						reg_endzp[i] - reg_startzp[i]);
	    }
	}
    }
#endif
    પાછા 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    સ્થિર long
bt_regexec_both(
    char_u	*લીટી,
    colnr_T	startcol,	// column to start looking for match
    પૂર્ણાંક		*timed_out)	// flag set on timeout or NULL
{
    bt_regprog_T    *prog;
    char_u	    *s;
    colnr_T	    col = startcol;
    long	    retval = 0L;

    // Create "regstack" and "backpos" if they are not allocated yet.
    // We allocate *_INITIAL amount of bytes first and then set the grow size
    // to much bigger value to avoid many malloc calls in case of deep regular
    // expressions.
    જો (regstack.ga_data == ખાલી)
    {
	// Use an item size of 1 byte, since we push different things
	// onto the regstack.
	ga_init2(&regstack, 1, REGSTACK_INITIAL);
	(શૂન્ય)ga_grow(&regstack, REGSTACK_INITIAL);
	regstack.ga_growsize = REGSTACK_INITIAL * 8;
    }

    જો (backpos.ga_data == ખાલી)
    {
	ga_init2(&backpos, કદ(backpos_T), BACKPOS_INITIAL);
	(શૂન્ય)ga_grow(&backpos, BACKPOS_INITIAL);
	backpos.ga_growsize = BACKPOS_INITIAL * 8;
    }

    જો (REG_MULTI)
    {
	prog = (bt_regprog_T *)rex.reg_mmatch->regprog;
	લીટી = reg_getline((linenr_T)0);
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    નહીંતર
    {
	prog = (bt_regprog_T *)rex.reg_match->regprog;
	rex.reg_startp = rex.reg_match->startp;
	rex.reg_endp = rex.reg_match->endp;
    }

    // Be paranoid...
    જો (prog == ખાલી || લીટી == ખાલી)
    {
	iemsg(e_null_argument);
	જાઓ theend;
    }

    // Check validity of program.
    જો (prog_magic_wrong())
	જાઓ theend;

    // If the start column is past the maximum column: no need to try.
    જો (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	જાઓ theend;

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    જો (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    નહીંતર જો (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    જો (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    // If there is a "must appear" string, look for it.
    જો (prog->regmust != ખાલી)
    {
	પૂર્ણાંક c;

	જો (has_mbyte)
	    c = (*mb_ptr2char)(prog->regmust);
	નહીંતર
	    c = *prog->regmust;
	s = લીટી + col;

	// This is used very often, esp. for ":global".  Use three versions of
	// the loop to avoid overhead of conditions.
	જો (!rex.reg_ic && !has_mbyte)
	    જ્યારે ((s = vim_strbyte(s, c)) != ખાલી)
	    {
		પૂર્ણાંક len = prog->regmlen;
		જો (cstrncmp(s, prog->regmust, &len) == 0)
		    તોડો;		// Found it.
		++s;
	    }
	નહીંતર જો (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	    જ્યારે ((s = vim_strchr(s, c)) != ખાલી)
	    {
		પૂર્ણાંક len = prog->regmlen;
		જો (cstrncmp(s, prog->regmust, &len) == 0)
		    તોડો;		// Found it.
		MB_PTR_ADV(s);
	    }
	નહીંતર
	    જ્યારે ((s = cstrchr(s, c)) != ખાલી)
	    {
		પૂર્ણાંક len = prog->regmlen;
		જો (cstrncmp(s, prog->regmust, &len) == 0)
		    તોડો;		// Found it.
		MB_PTR_ADV(s);
	    }
	જો (s == ખાલી)		// Not present.
	    જાઓ theend;
    }

    rex.લીટી = લીટી;
    rex.lnum = 0;
    reg_toolong = FALSE;

    // Simplest case: Anchored match need be tried only once.
    જો (prog->reganch)
    {
	પૂર્ણાંક	c;

	જો (has_mbyte)
	    c = (*mb_ptr2char)(rex.લીટી + col);
	નહીંતર
	    c = rex.લીટી[col];
	જો (prog->regstart == NUL
		|| prog->regstart == c
		|| (rex.reg_ic
		    && (((enc_utf8 && utf_fold(prog->regstart) == utf_fold(c)))
			|| (c < 255 && prog->regstart < 255 &&
			    MB_TOLOWER(prog->regstart) == MB_TOLOWER(c)))))
	    retval = regtry(prog, col, timed_out);
	નહીંતર
	    retval = 0;
    }
    નહીંતર
    {
	// Messy cases:  unanchored match.
	જ્યારે (!got_int)
	{
	    જો (prog->regstart != NUL)
	    {
		// Skip until the char we know it must start with.
		// Used often, do some work to avoid call overhead.
		જો (!rex.reg_ic && !has_mbyte)
		    s = vim_strbyte(rex.લીટી + col, prog->regstart);
		નહીંતર
		    s = cstrchr(rex.લીટી + col, prog->regstart);
		જો (s == ખાલી)
		{
		    retval = 0;
		    તોડો;
		}
		col = (પૂર્ણાંક)(s - rex.લીટી);
	    }

	    // Check for maximum column to try.
	    જો (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	    {
		retval = 0;
		તોડો;
	    }

	    retval = regtry(prog, col, timed_out);
	    જો (retval > 0)
		તોડો;

	    // if not currently on the first line, get it again
	    જો (rex.lnum != 0)
	    {
		rex.lnum = 0;
		rex.લીટી = reg_getline((linenr_T)0);
	    }
	    જો (rex.લીટી[col] == NUL)
		તોડો;
	    જો (has_mbyte)
		col += (*mb_ptr2len)(rex.લીટી + col);
	    નહીંતર
		++col;
#ifdef FEAT_RELTIME
	    જો (bt_did_time_out(timed_out))
		તોડો;
#endif
	}
    }

theend:
    // Free "reg_tofree" when it's a bit big.
    // Free regstack and backpos if they are bigger than their initial size.
    જો (reg_tofreelen > 400)
	VIM_CLEAR(reg_tofree);
    જો (regstack.ga_maxlen > REGSTACK_INITIAL)
	ga_clear(&regstack);
    જો (backpos.ga_maxlen > BACKPOS_INITIAL)
	ga_clear(&backpos);

    જો (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs
	// and \ze are used.
	જો (REG_MULTI)
	{
	    lpos_T *start = &rex.reg_mmatch->startpos[0];
	    lpos_T *end = &rex.reg_mmatch->endpos[0];

	    જો (end->lnum < start->lnum
			|| (end->lnum == start->lnum && end->col < start->col))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_mmatch->rmm_matchcol = col;
	}
	નહીંતર
	{
	    જો (rex.reg_match->endp[0] < rex.reg_match->startp[0])
		rex.reg_match->endp[0] = rex.reg_match->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_match->rm_matchcol = col;
	}
    }

    પાછા retval;
}

/*
 * Match a regexp against a string.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 * if "line_lbr" is TRUE  consider a "\n" in "line" to be a line break.
 *
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    સ્થિર પૂર્ણાંક
bt_regexec_nl(
    regmatch_T	*rmp,
    char_u	*લીટી,	// string to match against
    colnr_T	col,	// column to start looking for match
    પૂર્ણાંક		લીટી_lbr)
{
    rex.reg_match = rmp;
    rex.reg_mmatch = ખાલી;
    rex.reg_maxline = 0;
    rex.reg_લીટી_lbr = લીટી_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = ખાલી;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;

    પાછા bt_regexec_both(લીટી, col, ખાલી);
}

/*
 * Match a regexp against multiple lines.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 *
 * Return zero if there is no match.  Return number of lines contained in the
 * match otherwise.
 */
    સ્થિર long
bt_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	col,		// column to start looking for match
    પૂર્ણાંક		*timed_out)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    પાછા bt_regexec_both(ખાલી, col, timed_out);
}

/*
 * Compare a number with the operand of RE_LNUM, RE_COL or RE_VCOL.
 */
    સ્થિર પૂર્ણાંક
re_અંક_cmp(long_u val, char_u *scan)
{
    long_u  n = OPERAND_MIN(scan);

    જો (OPERAND_CMP(scan) == '>')
	પાછા val > n;
    જો (OPERAND_CMP(scan) == '<')
	પાછા val < n;
    પાછા val == n;
}

#ifdef BT_REGEXP_DUMP

/*
 * regdump - dump a regexp onto stdout in vaguely comprehensible form
 */
    સ્થિર શૂન્ય
regdump(char_u *pattern, bt_regprog_T *r)
{
    char_u  *s;
    પૂર્ણાંક	    op = EXACTLY;	// Arbitrary non-END op.
    char_u  *next;
    char_u  *end = ખાલી;
    FILE    *f;

# ifdef BT_REGEXP_LOG
    f = fopen("bt_regexp_log.log", "a");
# નહીંતર
    f = stdout;
# endif
    જો (f == ખાલી)
	પાછા;
    fprintf(f, "-------------------------------------\n\r\nregcomp(%s):\r\n", pattern);

    s = r->program + 1;
    // Loop until we find the END that isn't before a referred next (an END
    // can also appear in a NOMATCH operand).
    જ્યારે ((op != END || s <= end) && s < r->program + r->regsz)
    {
	op = OP(s);
	fprintf(f, "%2d%s", (પૂર્ણાંક)(s - r->program), regprop(s)); // Where, what.
	next = (s + 3 <= r->program + r->regsz) ? regnext(s) : ખાલી;
	જો (next == ખાલી)	// Next ptr.
	    fprintf(f, "(0)");
	નહીંતર
	    fprintf(f, "(%d)", (પૂર્ણાંક)((s - r->program) + (next - s)));
	જો (end < next)
	    end = next;
	જો (op == BRACE_LIMITS)
	{
	    // Two ints
	    fprintf(f, " minval %ld, maxval %ld", OPERAND_MIN(s), OPERAND_MAX(s));
	    s += 8;
	}
	નહીંતર જો (op == BEHIND || op == NOBEHIND)
	{
	    // one int
	    fprintf(f, " count %ld", OPERAND_MIN(s));
	    s += 4;
	}
	નહીંતર જો (op == RE_LNUM || op == RE_COL || op == RE_VCOL)
	{
	    // one int plus comparator
	    fprintf(f, " count %ld", OPERAND_MIN(s));
	    s += 5;
	}
	s += 3;
	જો (op == MULTIBYTECODE)
	{
	    fprintf(f, " mbc=%d", utf_ptr2char(s));
	    s += utfc_ptr2len(s);
	}
	જો (op == ANYOF || op == ANYOF + ADD_NL
		|| op == ANYBUT || op == ANYBUT + ADD_NL
		|| op == EXACTLY)
	{
	    // Literal string, where present.
	    fprintf(f, "\nxxxxxxxxx\n");
	    જ્યારે (*s != NUL && s < r->program + r->regsz)
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
    જો (r->regstart != NUL)
	fprintf(f, "start `%s' 0x%x; ", r->regstart < 256
		? (અક્ષર *)transchar(r->regstart)
		: "multibyte", r->regstart);
    જો (r->reganch)
	fprintf(f, "anchored; ");
    જો (r->regmust != ખાલી)
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
    સ્થિર char_u *
regprop(char_u *op)
{
    અક્ષર	    *p;
    સ્થિર અક્ષર	    buf[50];
    સ્થિર size_t   buflen = 0;

    STRCPY(buf, ":");
    buflen = 1;

    પસંદગી ((પૂર્ણાંક) OP(op))
    {
      સ્થિતિ BOL:
	p = "BOL";
	તોડો;
      સ્થિતિ EOL:
	p = "EOL";
	તોડો;
      સ્થિતિ RE_BOF:
	p = "BOF";
	તોડો;
      સ્થિતિ RE_EOF:
	p = "EOF";
	તોડો;
      સ્થિતિ CURSOR:
	p = "CURSOR";
	તોડો;
      સ્થિતિ RE_VISUAL:
	p = "RE_VISUAL";
	તોડો;
      સ્થિતિ RE_LNUM:
	p = "RE_LNUM";
	તોડો;
      સ્થિતિ RE_MARK:
	p = "RE_MARK";
	તોડો;
      સ્થિતિ RE_COL:
	p = "RE_COL";
	તોડો;
      સ્થિતિ RE_VCOL:
	p = "RE_VCOL";
	તોડો;
      સ્થિતિ BOW:
	p = "BOW";
	તોડો;
      સ્થિતિ EOW:
	p = "EOW";
	તોડો;
      સ્થિતિ ANY:
	p = "ANY";
	તોડો;
      સ્થિતિ ANY + ADD_NL:
	p = "ANY+NL";
	તોડો;
      સ્થિતિ ANYOF:
	p = "ANYOF";
	તોડો;
      સ્થિતિ ANYOF + ADD_NL:
	p = "ANYOF+NL";
	તોડો;
      સ્થિતિ ANYBUT:
	p = "ANYBUT";
	તોડો;
      સ્થિતિ ANYBUT + ADD_NL:
	p = "ANYBUT+NL";
	તોડો;
      સ્થિતિ IDENT:
	p = "IDENT";
	તોડો;
      સ્થિતિ IDENT + ADD_NL:
	p = "IDENT+NL";
	તોડો;
      સ્થિતિ SIDENT:
	p = "SIDENT";
	તોડો;
      સ્થિતિ SIDENT + ADD_NL:
	p = "SIDENT+NL";
	તોડો;
      સ્થિતિ KWORD:
	p = "KWORD";
	તોડો;
      સ્થિતિ KWORD + ADD_NL:
	p = "KWORD+NL";
	તોડો;
      સ્થિતિ SKWORD:
	p = "SKWORD";
	તોડો;
      સ્થિતિ SKWORD + ADD_NL:
	p = "SKWORD+NL";
	તોડો;
      સ્થિતિ FNAME:
	p = "FNAME";
	તોડો;
      સ્થિતિ FNAME + ADD_NL:
	p = "FNAME+NL";
	તોડો;
      સ્થિતિ SFNAME:
	p = "SFNAME";
	તોડો;
      સ્થિતિ SFNAME + ADD_NL:
	p = "SFNAME+NL";
	તોડો;
      સ્થિતિ PRINT:
	p = "PRINT";
	તોડો;
      સ્થિતિ PRINT + ADD_NL:
	p = "PRINT+NL";
	તોડો;
      સ્થિતિ SPRINT:
	p = "SPRINT";
	તોડો;
      સ્થિતિ SPRINT + ADD_NL:
	p = "SPRINT+NL";
	તોડો;
      સ્થિતિ WHITE:
	p = "WHITE";
	તોડો;
      સ્થિતિ WHITE + ADD_NL:
	p = "WHITE+NL";
	તોડો;
      સ્થિતિ NWHITE:
	p = "NWHITE";
	તોડો;
      સ્થિતિ NWHITE + ADD_NL:
	p = "NWHITE+NL";
	તોડો;
      સ્થિતિ DIGIT:
	p = "DIGIT";
	તોડો;
      સ્થિતિ DIGIT + ADD_NL:
	p = "DIGIT+NL";
	તોડો;
      સ્થિતિ NDIGIT:
	p = "NDIGIT";
	તોડો;
      સ્થિતિ NDIGIT + ADD_NL:
	p = "NDIGIT+NL";
	તોડો;
      સ્થિતિ HEX:
	p = "HEX";
	તોડો;
      સ્થિતિ HEX + ADD_NL:
	p = "HEX+NL";
	તોડો;
      સ્થિતિ NHEX:
	p = "NHEX";
	તોડો;
      સ્થિતિ NHEX + ADD_NL:
	p = "NHEX+NL";
	તોડો;
      સ્થિતિ OCTAL:
	p = "OCTAL";
	તોડો;
      સ્થિતિ OCTAL + ADD_NL:
	p = "OCTAL+NL";
	તોડો;
      સ્થિતિ NOCTAL:
	p = "NOCTAL";
	તોડો;
      સ્થિતિ NOCTAL + ADD_NL:
	p = "NOCTAL+NL";
	તોડો;
      સ્થિતિ WORD:
	p = "WORD";
	તોડો;
      સ્થિતિ WORD + ADD_NL:
	p = "WORD+NL";
	તોડો;
      સ્થિતિ NWORD:
	p = "NWORD";
	તોડો;
      સ્થિતિ NWORD + ADD_NL:
	p = "NWORD+NL";
	તોડો;
      સ્થિતિ HEAD:
	p = "HEAD";
	તોડો;
      સ્થિતિ HEAD + ADD_NL:
	p = "HEAD+NL";
	તોડો;
      સ્થિતિ NHEAD:
	p = "NHEAD";
	તોડો;
      સ્થિતિ NHEAD + ADD_NL:
	p = "NHEAD+NL";
	તોડો;
      સ્થિતિ ALPHA:
	p = "ALPHA";
	તોડો;
      સ્થિતિ ALPHA + ADD_NL:
	p = "ALPHA+NL";
	તોડો;
      સ્થિતિ NALPHA:
	p = "NALPHA";
	તોડો;
      સ્થિતિ NALPHA + ADD_NL:
	p = "NALPHA+NL";
	તોડો;
      સ્થિતિ LOWER:
	p = "LOWER";
	તોડો;
      સ્થિતિ LOWER + ADD_NL:
	p = "LOWER+NL";
	તોડો;
      સ્થિતિ NLOWER:
	p = "NLOWER";
	તોડો;
      સ્થિતિ NLOWER + ADD_NL:
	p = "NLOWER+NL";
	તોડો;
      સ્થિતિ UPPER:
	p = "UPPER";
	તોડો;
      સ્થિતિ UPPER + ADD_NL:
	p = "UPPER+NL";
	તોડો;
      સ્થિતિ NUPPER:
	p = "NUPPER";
	તોડો;
      સ્થિતિ NUPPER + ADD_NL:
	p = "NUPPER+NL";
	તોડો;
      સ્થિતિ BRANCH:
	p = "BRANCH";
	તોડો;
      સ્થિતિ EXACTLY:
	p = "EXACTLY";
	તોડો;
      સ્થિતિ NOTHING:
	p = "NOTHING";
	તોડો;
      સ્થિતિ BACK:
	p = "BACK";
	તોડો;
      સ્થિતિ END:
	p = "END";
	તોડો;
      સ્થિતિ MOPEN + 0:
	p = "MATCH START";
	તોડો;
      સ્થિતિ MOPEN + 1:
      સ્થિતિ MOPEN + 2:
      સ્થિતિ MOPEN + 3:
      સ્થિતિ MOPEN + 4:
      સ્થિતિ MOPEN + 5:
      સ્થિતિ MOPEN + 6:
      સ્થિતિ MOPEN + 7:
      સ્થિતિ MOPEN + 8:
      સ્થિતિ MOPEN + 9:
	buflen += vim_snprintf(buf + buflen, કદ(buf) - buflen, "MOPEN%d", OP(op) - MOPEN);
	p = ખાલી;
	તોડો;
      સ્થિતિ MCLOSE + 0:
	p = "MATCH END";
	તોડો;
      સ્થિતિ MCLOSE + 1:
      સ્થિતિ MCLOSE + 2:
      સ્થિતિ MCLOSE + 3:
      સ્થિતિ MCLOSE + 4:
      સ્થિતિ MCLOSE + 5:
      સ્થિતિ MCLOSE + 6:
      સ્થિતિ MCLOSE + 7:
      સ્થિતિ MCLOSE + 8:
      સ્થિતિ MCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, કદ(buf) - buflen, "MCLOSE%d", OP(op) - MCLOSE);
	p = ખાલી;
	તોડો;
      સ્થિતિ BACKREF + 1:
      સ્થિતિ BACKREF + 2:
      સ્થિતિ BACKREF + 3:
      સ્થિતિ BACKREF + 4:
      સ્થિતિ BACKREF + 5:
      સ્થિતિ BACKREF + 6:
      સ્થિતિ BACKREF + 7:
      સ્થિતિ BACKREF + 8:
      સ્થિતિ BACKREF + 9:
	buflen += vim_snprintf(buf + buflen, કદ(buf) - buflen, "BACKREF%d", OP(op) - BACKREF);
	p = ખાલી;
	તોડો;
      સ્થિતિ NOPEN:
	p = "NOPEN";
	તોડો;
      સ્થિતિ NCLOSE:
	p = "NCLOSE";
	તોડો;
# ifdef FEAT_SYN_HL
      સ્થિતિ ZOPEN + 1:
      સ્થિતિ ZOPEN + 2:
      સ્થિતિ ZOPEN + 3:
      સ્થિતિ ZOPEN + 4:
      સ્થિતિ ZOPEN + 5:
      સ્થિતિ ZOPEN + 6:
      સ્થિતિ ZOPEN + 7:
      સ્થિતિ ZOPEN + 8:
      સ્થિતિ ZOPEN + 9:
	buflen += vim_snprintf(buf + buflen, કદ(buf) - buflen, "ZOPEN%d", OP(op) - ZOPEN);
	p = ખાલી;
	તોડો;
      સ્થિતિ ZCLOSE + 1:
      સ્થિતિ ZCLOSE + 2:
      સ્થિતિ ZCLOSE + 3:
      સ્થિતિ ZCLOSE + 4:
      સ્થિતિ ZCLOSE + 5:
      સ્થિતિ ZCLOSE + 6:
      સ્થિતિ ZCLOSE + 7:
      સ્થિતિ ZCLOSE + 8:
      સ્થિતિ ZCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, કદ(buf) - buflen, "ZCLOSE%d", OP(op) - ZCLOSE);
	p = ખાલી;
	તોડો;
      સ્થિતિ ZREF + 1:
      સ્થિતિ ZREF + 2:
      સ્થિતિ ZREF + 3:
      સ્થિતિ ZREF + 4:
      સ્થિતિ ZREF + 5:
      સ્થિતિ ZREF + 6:
      સ્થિતિ ZREF + 7:
      સ્થિતિ ZREF + 8:
      સ્થિતિ ZREF + 9:
	buflen += vim_snprintf(buf + buflen, કદ(buf) - buflen, "ZREF%d", OP(op) - ZREF);
	p = ખાલી;
	તોડો;
# endif
      સ્થિતિ STAR:
	p = "STAR";
	તોડો;
      સ્થિતિ PLUS:
	p = "PLUS";
	તોડો;
      સ્થિતિ NOMATCH:
	p = "NOMATCH";
	તોડો;
      સ્થિતિ MATCH:
	p = "MATCH";
	તોડો;
      સ્થિતિ BEHIND:
	p = "BEHIND";
	તોડો;
      સ્થિતિ NOBEHIND:
	p = "NOBEHIND";
	તોડો;
      સ્થિતિ SUBPAT:
	p = "SUBPAT";
	તોડો;
      સ્થિતિ BRACE_LIMITS:
	p = "BRACE_LIMITS";
	તોડો;
      સ્થિતિ BRACE_SIMPLE:
	p = "BRACE_SIMPLE";
	તોડો;
      સ્થિતિ BRACE_COMPLEX + 0:
      સ્થિતિ BRACE_COMPLEX + 1:
      સ્થિતિ BRACE_COMPLEX + 2:
      સ્થિતિ BRACE_COMPLEX + 3:
      સ્થિતિ BRACE_COMPLEX + 4:
      સ્થિતિ BRACE_COMPLEX + 5:
      સ્થિતિ BRACE_COMPLEX + 6:
      સ્થિતિ BRACE_COMPLEX + 7:
      સ્થિતિ BRACE_COMPLEX + 8:
      સ્થિતિ BRACE_COMPLEX + 9:
	buflen += vim_snprintf(buf + buflen, કદ(buf) - buflen, "BRACE_COMPLEX%d", OP(op) - BRACE_COMPLEX);
	p = ખાલી;
	તોડો;
      સ્થિતિ MULTIBYTECODE:
	p = "MULTIBYTECODE";
	તોડો;
      સ્થિતિ NEWL:
	p = "NEWL";
	તોડો;
      મૂળભૂત:
	buflen += vim_snprintf(buf + buflen, કદ(buf) - buflen, "corrupt %d", OP(op));
	p = ખાલી;
	તોડો;
    }
    જો (p != ખાલી)
	vim_strncpy((char_u *)buf + buflen, (char_u *)p, કદ(buf) - buflen - 1);
    પાછા (char_u *)buf;
}
#endif	    // DEBUG
