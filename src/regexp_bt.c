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

நிலையான முழு	இலக்கம்_complex_braces; // Complex \{...} count
நிலையான char_u	*regcode;	// Code-emit pointer, or JUST_CALC_SIZE
நிலையான long	regsize;	// Code size.
நிலையான முழு	reg_toolong;	// TRUE when offset out of range
நிலையான முழு	bt_reg_parse_depth;	// nesting depth in reg()
நிலையான char_u	had_endbrace[NSUBEXP];	// flags, TRUE if end of () found
நிலையான long	brace_min[10];	// Minimums for complex brace repeats
நிலையான long	brace_max[10];	// Maximums for complex brace repeats
நிலையான முழு	brace_மொத்தம்[10]; // Current counts for complex brace repeats
நிலையான முழு	one_exactly = FALSE;	// only do one char for EXACTLY

// When making changes to classchars also change nfa_classcodes.
நிலையான char_u	*classchars = (char_u *)".iIkKfFpPsSdDxXoOwWhHaAlLuU";
நிலையான மாறிலி முழு classcodes[] = {
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
    நிலையான முழு
bt_recognize_char_class(char_u *start, char_u *end)
{
    முழு		newl;
    முழு		config = get_char_class_bits(start, end, &newl);

    // A newline would need the ADD_NL variant, don't bother with it here.
    எனில் (config < 0 || newl)
	திரும்பு 0;

	// The old engine has no case-insensitive class opcode, so [a-z] and [A-Z]
	// are left as collections.  The classes below are case-independent.
    தேர்வு (config)
    {
	நிலை CLASS_o9:
	    திரும்பு DIGIT;
	நிலை CLASS_not | CLASS_o9:
	    திரும்பு NDIGIT;
	நிலை CLASS_af | CLASS_AF | CLASS_o9:
	    திரும்பு HEX;
	நிலை CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    திரும்பு NHEX;
	நிலை CLASS_o7:
	    திரும்பு OCTAL;
	நிலை CLASS_not | CLASS_o7:
	    திரும்பு NOCTAL;
	நிலை CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    திரும்பு WORD;
	நிலை CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    திரும்பு NWORD;
	நிலை CLASS_az | CLASS_AZ | CLASS_underscore:
	    திரும்பு HEAD;
	நிலை CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    திரும்பு NHEAD;
	நிலை CLASS_az | CLASS_AZ:
	    திரும்பு ALPHA;
	நிலை CLASS_not | CLASS_az | CLASS_AZ:
	    திரும்பு NALPHA;
    }
    திரும்பு 0;
}

/*
 * When regcode is set to this value, code is not emitted and size is computed
 * instead.
 */
#define JUST_CALC_SIZE	((char_u *) -1)

// Values for rs_state in regitem_T.
வகைவரை எண்ணிக்கை regstate_E
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
வகைவரை கட்டமைப்பு
{
    ஒன்றிப்பு
    {
	char_u	*ptr;	// rex.input pointer, for single-line regexp
	lpos_T	pos;	// rex.input pos, for multi-line regexp
    } rs_u;
    முழு		rs_len;
} regsave_T;

// struct to save start/end pointer/position in for \(\)
வகைவரை கட்டமைப்பு
{
    ஒன்றிப்பு
    {
	char_u	*ptr;
	lpos_T	pos;
    } se_u;
} save_se_T;

// used for BEHIND and NOBEHIND matching
வகைவரை கட்டமைப்பு regbehind_S
{
    regsave_T	save_after;
    regsave_T	save_behind;
    முழு		save_need_clear_subexpr;
    save_se_T   save_start[NSUBEXP];
    save_se_T   save_end[NSUBEXP];
} regbehind_T;

/*
 * When there are alternatives a regstate_T is put on the regstack to remember
 * what we are doing.
 * Before it may be another type of item, depending on rs_state, to remember
 * more things.
 */
வகைவரை கட்டமைப்பு regitem_S
{
    regstate_T	rs_state;	// what we are doing, one of RS_ above
    short	rs_no;		// submatch nr or BEHIND/NOBEHIND
    char_u	*rs_scan;	// current node in program
    ஒன்றிப்பு
    {
	save_se_T  sesave;
	regsave_T  regsave;
    } rs_un;			// room for saving rex.input
} regitem_T;


// used for STAR, PLUS and BRACE_SIMPLE matching
வகைவரை கட்டமைப்பு regstar_S
{
    முழு		nextb;		// next byte
    முழு		nextb_ic;	// next byte reverse case
    long	மொத்தம்;
    long	minval;
    long	maxval;
} regstar_T;

// used to store input position when a BACK was encountered, so that we now if
// we made any progress since the last time.
வகைவரை கட்டமைப்பு backpos_S
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
நிலையான garray_T	regstack = {0, 0, 0, 0, வெற்று};
நிலையான garray_T	backpos = {0, 0, 0, 0, வெற்று};

நிலையான regsave_T behind_pos;

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
#define OP(p)		((முழு)*(p))
#define NEXT(p)		(((*((p) + 1) & 0377) << 8) + (*((p) + 2) & 0377))
#define OPERAND(p)	((p) + 3)
// Obtain an operand that was stored as four bytes, MSB first.
#define OPERAND_MIN(p)	(((long)(p)[3] << 24) + ((long)(p)[4] << 16) \
			+ ((long)(p)[5] << 8) + (long)(p)[6])
// Obtain a second operand stored as four bytes.
#define OPERAND_MAX(p)	OPERAND_MIN((p) + 4)
// Obtain a second single-byte operand stored after a four bytes operand.
#define OPERAND_CMP(p)	(p)[7]

நிலையான char_u *reg(முழு paren, முழு *flagp);

#ifdef BT_REGEXP_DUMP
நிலையான வெறுமை	regdump(char_u *, bt_regprog_T *);
#endif

நிலையான முழு	re_இலக்கம்_cmp(long_u val, char_u *scan);

#ifdef DEBUG
நிலையான char_u	*regprop(char_u *);

நிலையான முழு	regnarrate = 0;
#endif


/*
 * Setup to parse the regexp.  Used once to get the length and once to do it.
 */
    நிலையான வெறுமை
regcomp_start(
    char_u	*கோவை,
    முழு		re_flags)	    // see vim_regcomp()
{
    initchr(கோவை);
    எனில் (re_flags & RE_MAGIC)
	reg_magic = MAGIC_ON;
    இல்லையெனில்
	reg_magic = MAGIC_OFF;
    reg_string = (re_flags & RE_STRING);
    reg_strict = (re_flags & RE_STRICT);
    get_cpo_flags();

    இலக்கம்_complex_braces = 0;
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
#எனில் defined(FEAT_SYN_HL)
    had_eol = FALSE;
#endif
}

/*
 * Return TRUE if MULTIBYTECODE should be used instead of EXACTLY for
 * character "c".
 */
    நிலையான முழு
use_multibytecode(முழு c)
{
    திரும்பு has_mbyte && (*mb_char2len)(c) > 1
		     && (re_multi_type(peekchr()) != NOT_MULTI
			     || (enc_utf8 && utf_iscomposing(c)));
}

/*
 * Emit (if appropriate) a byte of code
 */
    நிலையான வெறுமை
regc(முழு b)
{
    எனில் (regcode == JUST_CALC_SIZE)
	regsize++;
    இல்லையெனில்
	*regcode++ = b;
}

/*
 * Emit (if appropriate) a multi-byte character of code
 */
    நிலையான வெறுமை
regmbc(முழு c)
{
    எனில் (!has_mbyte && c > 0xff)
	திரும்பு;
    எனில் (regcode == JUST_CALC_SIZE)
	regsize += (*mb_char2len)(c);
    இல்லையெனில்
	regcode += (*mb_char2bytes)(c, regcode);
}


/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * NOTE: When changing this function, also change nfa_emit_equi_class()
 */
    நிலையான வெறுமை
reg_equi_class(முழு c)
{
    எனில் (enc_utf8 || STRCMP(p_enc, "latin1") == 0
					 || STRCMP(p_enc, "iso-8859-15") == 0)
    {
	தேர்வு (c)
	{
	    // Do not use '\300' style, it results in a negative number.
	    நிலை 'A': நிலை 0xc0: நிலை 0xc1: நிலை 0xc2: நிலை 0xc3: நிலை 0xc4:
	    நிலை 0xc5: நிலை 0x100: நிலை 0x102: நிலை 0x104: நிலை 0x1cd:
	    நிலை 0x1de: நிலை 0x1e0: நிலை 0x1fa: நிலை 0x202: நிலை 0x226:
	    நிலை 0x23a: நிலை 0x1e00: நிலை 0x1ea0: நிலை 0x1ea2: நிலை 0x1ea4:
	    நிலை 0x1ea6: நிலை 0x1ea8: நிலை 0x1eaa: நிலை 0x1eac: நிலை 0x1eae:
	    நிலை 0x1eb0: நிலை 0x1eb2: நிலை 0x1eb4: நிலை 0x1eb6:
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
		      திரும்பு;
	    நிலை 'B': நிலை 0x181: நிலை 0x243: நிலை 0x1e02:
	    நிலை 0x1e04: நிலை 0x1e06:
		      regmbc('B');
		      regmbc(0x181); regmbc(0x243); regmbc(0x1e02);
		      regmbc(0x1e04); regmbc(0x1e06);
		      திரும்பு;
	    நிலை 'C': நிலை 0xc7:
	    நிலை 0x106: நிலை 0x108: நிலை 0x10a: நிலை 0x10c: நிலை 0x187:
	    நிலை 0x23b: நிலை 0x1e08: நிலை 0xa792:
		      regmbc('C'); regmbc(0xc7);
		      regmbc(0x106); regmbc(0x108); regmbc(0x10a);
		      regmbc(0x10c); regmbc(0x187); regmbc(0x23b);
		      regmbc(0x1e08); regmbc(0xa792);
		      திரும்பு;
	    நிலை 'D': நிலை 0x10e: நிலை 0x110: நிலை 0x18a:
	    நிலை 0x1e0a: நிலை 0x1e0c: நிலை 0x1e0e: நிலை 0x1e10:
	    நிலை 0x1e12:
		      regmbc('D'); regmbc(0x10e); regmbc(0x110);
		      regmbc(0x18a); regmbc(0x1e0a); regmbc(0x1e0c);
		      regmbc(0x1e0e); regmbc(0x1e10); regmbc(0x1e12);
		      திரும்பு;
	    நிலை 'E': நிலை 0xc8: நிலை 0xc9: நிலை 0xca: நிலை 0xcb:
	    நிலை 0x112: நிலை 0x114: நிலை 0x116: நிலை 0x118: நிலை 0x11a:
	    நிலை 0x204: நிலை 0x206: நிலை 0x228: நிலை 0x246: நிலை 0x1e14:
	    நிலை 0x1e16: நிலை 0x1e18: நிலை 0x1e1a: நிலை 0x1e1c:
	    நிலை 0x1eb8: நிலை 0x1eba: நிலை 0x1ebc: நிலை 0x1ebe:
	    நிலை 0x1ec0: நிலை 0x1ec2: நிலை 0x1ec4: நிலை 0x1ec6:
		      regmbc('E'); regmbc(0xc8); regmbc(0xc9);
		      regmbc(0xca); regmbc(0xcb); regmbc(0x112);
		      regmbc(0x114); regmbc(0x116); regmbc(0x118);
		      regmbc(0x11a); regmbc(0x204); regmbc(0x206);
		      regmbc(0x228); regmbc(0x246); regmbc(0x1e14);
		      regmbc(0x1e16); regmbc(0x1e18); regmbc(0x1e1a);
		      regmbc(0x1e1c); regmbc(0x1eb8); regmbc(0x1eba);
		      regmbc(0x1ebc); regmbc(0x1ebe); regmbc(0x1ec0);
		      regmbc(0x1ec2); regmbc(0x1ec4); regmbc(0x1ec6);
		      திரும்பு;
	    நிலை 'F': நிலை 0x191: நிலை 0x1e1e: நிலை 0xa798:
		      regmbc('F'); regmbc(0x191); regmbc(0x1e1e);
		      regmbc(0xa798);
		      திரும்பு;
	    நிலை 'G': நிலை 0x11c: நிலை 0x11e: நிலை 0x120:
	    நிலை 0x122: நிலை 0x193: நிலை 0x1e4: நிலை 0x1e6:
	    நிலை 0x1f4: நிலை 0x1e20: நிலை 0xa7a0:
		      regmbc('G'); regmbc(0x11c); regmbc(0x11e);
		      regmbc(0x120); regmbc(0x122); regmbc(0x193);
		      regmbc(0x1e4); regmbc(0x1e6); regmbc(0x1f4);
		      regmbc(0x1e20); regmbc(0xa7a0);
		      திரும்பு;
	    நிலை 'H': நிலை 0x124: நிலை 0x126: நிலை 0x21e:
	    நிலை 0x1e22: நிலை 0x1e24: நிலை 0x1e26:
	    நிலை 0x1e28: நிலை 0x1e2a: நிலை 0x2c67:
		      regmbc('H'); regmbc(0x124); regmbc(0x126);
		      regmbc(0x21e); regmbc(0x1e22); regmbc(0x1e24);
		      regmbc(0x1e26); regmbc(0x1e28); regmbc(0x1e2a);
		      regmbc(0x2c67);
		      திரும்பு;
	    நிலை 'I': நிலை 0xcc: நிலை 0xcd: நிலை 0xce: நிலை 0xcf:
	    நிலை 0x128: நிலை 0x12a: நிலை 0x12c: நிலை 0x12e:
	    நிலை 0x130: நிலை 0x197: நிலை 0x1cf: நிலை 0x208:
	    நிலை 0x20a: நிலை 0x1e2c: நிலை 0x1e2e: நிலை 0x1ec8:
	    நிலை 0x1eca:
		      regmbc('I'); regmbc(0xcc); regmbc(0xcd);
		      regmbc(0xce); regmbc(0xcf); regmbc(0x128);
		      regmbc(0x12a); regmbc(0x12c); regmbc(0x12e);
		      regmbc(0x130); regmbc(0x197); regmbc(0x1cf);
		      regmbc(0x208); regmbc(0x20a); regmbc(0x1e2c);
		      regmbc(0x1e2e); regmbc(0x1ec8); regmbc(0x1eca);
		      திரும்பு;
	    நிலை 'J': நிலை 0x134: நிலை 0x248:
		      regmbc('J'); regmbc(0x134); regmbc(0x248);
		      திரும்பு;
	    நிலை 'K': நிலை 0x136: நிலை 0x198: நிலை 0x1e8: நிலை 0x1e30:
	    நிலை 0x1e32: நிலை 0x1e34: நிலை 0x2c69: நிலை 0xa740:
		      regmbc('K'); regmbc(0x136); regmbc(0x198);
		      regmbc(0x1e8); regmbc(0x1e30); regmbc(0x1e32);
		      regmbc(0x1e34); regmbc(0x2c69); regmbc(0xa740);
		      திரும்பு;
	    நிலை 'L': நிலை 0x139: நிலை 0x13b: நிலை 0x13d: நிலை 0x13f:
	    நிலை 0x141: நிலை 0x23d: நிலை 0x1e36: நிலை 0x1e38:
	    நிலை 0x1e3a: நிலை 0x1e3c: நிலை 0x2c60:
		      regmbc('L'); regmbc(0x139); regmbc(0x13b);
		      regmbc(0x13d); regmbc(0x13f); regmbc(0x141);
		      regmbc(0x23d); regmbc(0x1e36); regmbc(0x1e38);
		      regmbc(0x1e3a); regmbc(0x1e3c); regmbc(0x2c60);
		      திரும்பு;
	    நிலை 'M': நிலை 0x1e3e: நிலை 0x1e40: நிலை 0x1e42:
		      regmbc('M'); regmbc(0x1e3e); regmbc(0x1e40);
		      regmbc(0x1e42);
		      திரும்பு;
	    நிலை 'N': நிலை 0xd1:
	    நிலை 0x143: நிலை 0x145: நிலை 0x147: நிலை 0x1f8:
	    நிலை 0x1e44: நிலை 0x1e46: நிலை 0x1e48: நிலை 0x1e4a:
	    நிலை 0xa7a4:
		      regmbc('N'); regmbc(0xd1);
		      regmbc(0x143); regmbc(0x145); regmbc(0x147);
		      regmbc(0x1f8); regmbc(0x1e44); regmbc(0x1e46);
		      regmbc(0x1e48); regmbc(0x1e4a); regmbc(0xa7a4);
		      திரும்பு;
	    நிலை 'O': நிலை 0xd2: நிலை 0xd3: நிலை 0xd4: நிலை 0xd5: நிலை 0xd6:
	    நிலை 0xd8: நிலை 0x14c: நிலை 0x14e: நிலை 0x150: நிலை 0x19f:
	    நிலை 0x1a0: நிலை 0x1d1: நிலை 0x1ea: நிலை 0x1ec: நிலை 0x1fe:
	    நிலை 0x20c: நிலை 0x20e: நிலை 0x22a: நிலை 0x22c: நிலை 0x22e:
	    நிலை 0x230: நிலை 0x1e4c: நிலை 0x1e4e: நிலை 0x1e50: நிலை 0x1e52:
	    நிலை 0x1ecc: நிலை 0x1ece: நிலை 0x1ed0: நிலை 0x1ed2: நிலை 0x1ed4:
	    நிலை 0x1ed6: நிலை 0x1ed8: நிலை 0x1eda: நிலை 0x1edc: நிலை 0x1ede:
	    நிலை 0x1ee0: நிலை 0x1ee2:
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
		      திரும்பு;
	    நிலை 'P': நிலை 0x1a4: நிலை 0x1e54: நிலை 0x1e56: நிலை 0x2c63:
		      regmbc('P'); regmbc(0x1a4); regmbc(0x1e54);
		      regmbc(0x1e56); regmbc(0x2c63);
		      திரும்பு;
	    நிலை 'Q': நிலை 0x24a:
		      regmbc('Q'); regmbc(0x24a);
		      திரும்பு;
	    நிலை 'R': நிலை 0x154: நிலை 0x156: நிலை 0x158: நிலை 0x210:
	    நிலை 0x212: நிலை 0x24c: நிலை 0x1e58: நிலை 0x1e5a:
	    நிலை 0x1e5c: நிலை 0x1e5e: நிலை 0x2c64: நிலை 0xa7a6:
		      regmbc('R'); regmbc(0x154); regmbc(0x156);
		      regmbc(0x210); regmbc(0x212); regmbc(0x158);
		      regmbc(0x24c); regmbc(0x1e58); regmbc(0x1e5a);
		      regmbc(0x1e5c); regmbc(0x1e5e); regmbc(0x2c64);
		      regmbc(0xa7a6);
		      திரும்பு;
	    நிலை 'S': நிலை 0x15a: நிலை 0x15c: நிலை 0x15e: நிலை 0x160:
	    நிலை 0x218: நிலை 0x1e60: நிலை 0x1e62: நிலை 0x1e64:
	    நிலை 0x1e66: நிலை 0x1e68: நிலை 0x2c7e: நிலை 0xa7a8:
		      regmbc('S'); regmbc(0x15a); regmbc(0x15c);
		      regmbc(0x15e); regmbc(0x160); regmbc(0x218);
		      regmbc(0x1e60); regmbc(0x1e62); regmbc(0x1e64);
		      regmbc(0x1e66); regmbc(0x1e68); regmbc(0x2c7e);
			  regmbc(0xa7a8);
		      திரும்பு;
	    நிலை 'T': நிலை 0x162: நிலை 0x164: நிலை 0x166: நிலை 0x1ac:
	    நிலை 0x1ae: நிலை 0x21a: நிலை 0x23e: நிலை 0x1e6a: நிலை 0x1e6c:
	    நிலை 0x1e6e: நிலை 0x1e70:
		      regmbc('T'); regmbc(0x162); regmbc(0x164);
		      regmbc(0x166); regmbc(0x1ac); regmbc(0x23e);
		      regmbc(0x1ae); regmbc(0x21a); regmbc(0x1e6a);
		      regmbc(0x1e6c); regmbc(0x1e6e); regmbc(0x1e70);
		      திரும்பு;
	    நிலை 'U': நிலை 0xd9: நிலை 0xda: நிலை 0xdb: நிலை 0xdc:
	    நிலை 0x168: நிலை 0x16a: நிலை 0x16c: நிலை 0x16e:
	    நிலை 0x170: நிலை 0x172: நிலை 0x1af: நிலை 0x1d3:
	    நிலை 0x1d5: நிலை 0x1d7: நிலை 0x1d9: நிலை 0x1db:
	    நிலை 0x214: நிலை 0x216: நிலை 0x244: நிலை 0x1e72:
	    நிலை 0x1e74: நிலை 0x1e76: நிலை 0x1e78: நிலை 0x1e7a:
	    நிலை 0x1ee4: நிலை 0x1ee6: நிலை 0x1ee8: நிலை 0x1eea:
	    நிலை 0x1eec: நிலை 0x1eee: நிலை 0x1ef0:
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
		      திரும்பு;
	    நிலை 'V': நிலை 0x1b2: நிலை 0x1e7c: நிலை 0x1e7e:
		      regmbc('V'); regmbc(0x1b2); regmbc(0x1e7c);
		      regmbc(0x1e7e);
		      திரும்பு;
	    நிலை 'W': நிலை 0x174: நிலை 0x1e80: நிலை 0x1e82:
	    நிலை 0x1e84: நிலை 0x1e86: நிலை 0x1e88:
		      regmbc('W'); regmbc(0x174); regmbc(0x1e80);
		      regmbc(0x1e82); regmbc(0x1e84); regmbc(0x1e86);
		      regmbc(0x1e88);
		      திரும்பு;
	    நிலை 'X': நிலை 0x1e8a: நிலை 0x1e8c:
		      regmbc('X'); regmbc(0x1e8a); regmbc(0x1e8c);
		      திரும்பு;
	    நிலை 'Y': நிலை 0xdd:
	    நிலை 0x176: நிலை 0x178: நிலை 0x1b3: நிலை 0x232: நிலை 0x24e:
	    நிலை 0x1e8e: நிலை 0x1ef2: நிலை 0x1ef6: நிலை 0x1ef4: நிலை 0x1ef8:
		      regmbc('Y'); regmbc(0xdd); regmbc(0x176);
		      regmbc(0x178); regmbc(0x1b3); regmbc(0x232);
		      regmbc(0x24e); regmbc(0x1e8e); regmbc(0x1ef2);
		      regmbc(0x1ef4); regmbc(0x1ef6); regmbc(0x1ef8);
		      திரும்பு;
	    நிலை 'Z': நிலை 0x179: நிலை 0x17b: நிலை 0x17d: நிலை 0x1b5:
	    நிலை 0x1e90: நிலை 0x1e92: நிலை 0x1e94: நிலை 0x2c6b:
		      regmbc('Z'); regmbc(0x179); regmbc(0x17b);
		      regmbc(0x17d); regmbc(0x1b5); regmbc(0x1e90);
		      regmbc(0x1e92); regmbc(0x1e94); regmbc(0x2c6b);
		      திரும்பு;
	    நிலை 'a': நிலை 0xe0: நிலை 0xe1: நிலை 0xe2:
	    நிலை 0xe3: நிலை 0xe4: நிலை 0xe5: நிலை 0x101: நிலை 0x103:
	    நிலை 0x105: நிலை 0x1ce: நிலை 0x1df: நிலை 0x1e1: நிலை 0x1fb:
	    நிலை 0x201: நிலை 0x203: நிலை 0x227: நிலை 0x1d8f: நிலை 0x1e01:
	    நிலை 0x1e9a: நிலை 0x1ea1: நிலை 0x1ea3: நிலை 0x1ea5:
	    நிலை 0x1ea7: நிலை 0x1ea9: நிலை 0x1eab: நிலை 0x1ead:
	    நிலை 0x1eaf: நிலை 0x1eb1: நிலை 0x1eb3: நிலை 0x1eb5:
	    நிலை 0x1eb7: நிலை 0x2c65:
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
		      திரும்பு;
	    நிலை 'b': நிலை 0x180: நிலை 0x253: நிலை 0x1d6c: நிலை 0x1d80:
	    நிலை 0x1e03: நிலை 0x1e05: நிலை 0x1e07:
		      regmbc('b');
		      regmbc(0x180); regmbc(0x253); regmbc(0x1d6c);
		      regmbc(0x1d80); regmbc(0x1e03); regmbc(0x1e05);
		      regmbc(0x1e07);
		      திரும்பு;
	    நிலை 'c': நிலை 0xe7:
	    நிலை 0x107: நிலை 0x109: நிலை 0x10b: நிலை 0x10d: நிலை 0x188:
	    நிலை 0x23c: நிலை 0x1e09: நிலை 0xa793: நிலை 0xa794:
		      regmbc('c'); regmbc(0xe7); regmbc(0x107);
		      regmbc(0x109); regmbc(0x10b); regmbc(0x10d);
		      regmbc(0x188); regmbc(0x23c); regmbc(0x1e09);
		      regmbc(0xa793); regmbc(0xa794);
		      திரும்பு;
	    நிலை 'd': நிலை 0x10f: நிலை 0x111: நிலை 0x257: நிலை 0x1d6d:
	    நிலை 0x1d81: நிலை 0x1d91: நிலை 0x1e0b: நிலை 0x1e0d:
	    நிலை 0x1e0f: நிலை 0x1e11: நிலை 0x1e13:
		      regmbc('d'); regmbc(0x10f); regmbc(0x111);
		      regmbc(0x257); regmbc(0x1d6d); regmbc(0x1d81);
		      regmbc(0x1d91); regmbc(0x1e0b); regmbc(0x1e0d);
		      regmbc(0x1e0f); regmbc(0x1e11); regmbc(0x1e13);
		      திரும்பு;
	    நிலை 'e': நிலை 0xe8: நிலை 0xe9: நிலை 0xea: நிலை 0xeb:
	    நிலை 0x113: நிலை 0x115: நிலை 0x117: நிலை 0x119:
	    நிலை 0x11b: நிலை 0x205: நிலை 0x207: நிலை 0x229:
	    நிலை 0x247: நிலை 0x1d92: நிலை 0x1e15: நிலை 0x1e17:
	    நிலை 0x1e19: நிலை 0x1e1b: நிலை 0x1eb9: நிலை 0x1ebb:
	    நிலை 0x1e1d: நிலை 0x1ebd: நிலை 0x1ebf: நிலை 0x1ec1:
	    நிலை 0x1ec3: நிலை 0x1ec5: நிலை 0x1ec7:
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
		      திரும்பு;
	    நிலை 'f': நிலை 0x192: நிலை 0x1d6e: நிலை 0x1d82:
	    நிலை 0x1e1f: நிலை 0xa799:
		     regmbc('f'); regmbc(0x192); regmbc(0x1d6e);
		     regmbc(0x1d82); regmbc(0x1e1f); regmbc(0xa799);
		     திரும்பு;
	    நிலை 'g': நிலை 0x11d: நிலை 0x11f: நிலை 0x121: நிலை 0x123:
	    நிலை 0x1e5: நிலை 0x1e7: நிலை 0x260: நிலை 0x1f5: நிலை 0x1d83:
	    நிலை 0x1e21: நிலை 0xa7a1:
		      regmbc('g'); regmbc(0x11d); regmbc(0x11f);
		      regmbc(0x121); regmbc(0x123); regmbc(0x1e5);
		      regmbc(0x1e7); regmbc(0x1f5); regmbc(0x260);
		      regmbc(0x1d83); regmbc(0x1e21); regmbc(0xa7a1);
		      திரும்பு;
	    நிலை 'h': நிலை 0x125: நிலை 0x127: நிலை 0x21f: நிலை 0x1e23:
	    நிலை 0x1e25: நிலை 0x1e27: நிலை 0x1e29: நிலை 0x1e2b:
	    நிலை 0x1e96: நிலை 0x2c68: நிலை 0xa795:
		      regmbc('h'); regmbc(0x125); regmbc(0x127);
		      regmbc(0x21f); regmbc(0x1e23); regmbc(0x1e25);
		      regmbc(0x1e27); regmbc(0x1e29); regmbc(0x1e2b);
		      regmbc(0x1e96); regmbc(0x2c68); regmbc(0xa795);
		      திரும்பு;
	    நிலை 'i': நிலை 0xec: நிலை 0xed: நிலை 0xee: நிலை 0xef:
	    நிலை 0x129: நிலை 0x12b: நிலை 0x12d: நிலை 0x12f:
	    நிலை 0x1d0: நிலை 0x209: நிலை 0x20b: நிலை 0x268:
	    நிலை 0x1d96: நிலை 0x1e2d: நிலை 0x1e2f: நிலை 0x1ec9:
	    நிலை 0x1ecb:
		      regmbc('i'); regmbc(0xec); regmbc(0xed);
		      regmbc(0xee); regmbc(0xef); regmbc(0x129);
		      regmbc(0x12b); regmbc(0x12d); regmbc(0x12f);
		      regmbc(0x1d0); regmbc(0x209); regmbc(0x20b);
		      regmbc(0x268); regmbc(0x1d96); regmbc(0x1e2d);
		      regmbc(0x1e2f); regmbc(0x1ec9); regmbc(0x1ecb);
		      திரும்பு;
	    நிலை 'j': நிலை 0x135: நிலை 0x1f0: நிலை 0x249:
		      regmbc('j'); regmbc(0x135); regmbc(0x1f0);
		      regmbc(0x249);
		      திரும்பு;
	    நிலை 'k': நிலை 0x137: நிலை 0x199: நிலை 0x1e9:
	    நிலை 0x1d84: நிலை 0x1e31: நிலை 0x1e33: நிலை 0x1e35:
	    நிலை 0x2c6a: நிலை 0xa741:
		      regmbc('k'); regmbc(0x137); regmbc(0x199);
		      regmbc(0x1e9); regmbc(0x1d84); regmbc(0x1e31);
		      regmbc(0x1e33); regmbc(0x1e35); regmbc(0x2c6a);
		      regmbc(0xa741);
		      திரும்பு;
	    நிலை 'l': நிலை 0x13a: நிலை 0x13c: நிலை 0x13e:
	    நிலை 0x140: நிலை 0x142: நிலை 0x19a: நிலை 0x1e37:
	    நிலை 0x1e39: நிலை 0x1e3b: நிலை 0x1e3d: நிலை 0x2c61:
		      regmbc('l'); regmbc(0x13a); regmbc(0x13c);
		      regmbc(0x13e); regmbc(0x140); regmbc(0x142);
		      regmbc(0x19a); regmbc(0x1e37); regmbc(0x1e39);
		      regmbc(0x1e3b); regmbc(0x1e3d); regmbc(0x2c61);
		      திரும்பு;
	    நிலை 'm': நிலை 0x1d6f: நிலை 0x1e3f: நிலை 0x1e41: நிலை 0x1e43:
		      regmbc('m'); regmbc(0x1d6f); regmbc(0x1e3f);
		      regmbc(0x1e41); regmbc(0x1e43);
		      திரும்பு;
	    நிலை 'n': நிலை 0xf1: நிலை 0x144: நிலை 0x146: நிலை 0x148:
	    நிலை 0x149: நிலை 0x1f9: நிலை 0x1d70: நிலை 0x1d87:
	    நிலை 0x1e45: நிலை 0x1e47: நிலை 0x1e49: நிலை 0x1e4b:
	    நிலை 0xa7a5:
		      regmbc('n'); regmbc(0xf1); regmbc(0x144);
		      regmbc(0x146); regmbc(0x148); regmbc(0x149);
		      regmbc(0x1f9); regmbc(0x1d70); regmbc(0x1d87);
		      regmbc(0x1e45); regmbc(0x1e47); regmbc(0x1e49);
		      regmbc(0x1e4b); regmbc(0xa7a5);
		      திரும்பு;
	    நிலை 'o': நிலை 0xf2: நிலை 0xf3: நிலை 0xf4: நிலை 0xf5:
	    நிலை 0xf6: நிலை 0xf8: நிலை 0x14d: நிலை 0x14f: நிலை 0x151:
	    நிலை 0x1a1: நிலை 0x1d2: நிலை 0x1eb: நிலை 0x1ed: நிலை 0x1ff:
	    நிலை 0x20d: நிலை 0x20f: நிலை 0x22b: நிலை 0x22d: நிலை 0x22f:
	    நிலை 0x231: நிலை 0x275: நிலை 0x1e4d: நிலை 0x1e4f:
	    நிலை 0x1e51: நிலை 0x1e53: நிலை 0x1ecd: நிலை 0x1ecf:
	    நிலை 0x1ed1: நிலை 0x1ed3: நிலை 0x1ed5: நிலை 0x1ed7:
	    நிலை 0x1ed9: நிலை 0x1edb: நிலை 0x1edd: நிலை 0x1edf:
	    நிலை 0x1ee1: நிலை 0x1ee3:
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
		      திரும்பு;
	    நிலை 'p': நிலை 0x1a5: நிலை 0x1d71: நிலை 0x1d88: நிலை 0x1d7d:
	    நிலை 0x1e55: நிலை 0x1e57:
		      regmbc('p'); regmbc(0x1a5); regmbc(0x1d71);
		      regmbc(0x1d7d); regmbc(0x1d88); regmbc(0x1e55);
		      regmbc(0x1e57);
		      திரும்பு;
	    நிலை 'q': நிலை 0x24b: நிலை 0x2a0:
		      regmbc('q'); regmbc(0x24b); regmbc(0x2a0);
		      திரும்பு;
	    நிலை 'r': நிலை 0x155: நிலை 0x157: நிலை 0x159: நிலை 0x211:
	    நிலை 0x213: நிலை 0x24d: நிலை 0x27d: நிலை 0x1d72: நிலை 0x1d73:
	    நிலை 0x1d89: நிலை 0x1e59: நிலை 0x1e5b: நிலை 0x1e5d: நிலை 0x1e5f:
	    நிலை 0xa7a7:
		      regmbc('r'); regmbc(0x155); regmbc(0x157);
		      regmbc(0x159); regmbc(0x211); regmbc(0x213);
		      regmbc(0x24d); regmbc(0x1d72); regmbc(0x1d73);
		      regmbc(0x1d89); regmbc(0x1e59); regmbc(0x27d);
		      regmbc(0x1e5b); regmbc(0x1e5d); regmbc(0x1e5f);
		      regmbc(0xa7a7);
		      திரும்பு;
	    நிலை 's': நிலை 0x15b: நிலை 0x15d: நிலை 0x15f: நிலை 0x161:
	    நிலை 0x1e61: நிலை 0x219: நிலை 0x23f: நிலை 0x1d74: நிலை 0x1d8a:
	    நிலை 0x1e63: நிலை 0x1e65: நிலை 0x1e67: நிலை 0x1e69: நிலை 0xa7a9:
		      regmbc('s'); regmbc(0x15b); regmbc(0x15d);
		      regmbc(0x15f); regmbc(0x161); regmbc(0x23f);
		      regmbc(0x219); regmbc(0x1d74); regmbc(0x1d8a);
		      regmbc(0x1e61); regmbc(0x1e63); regmbc(0x1e65);
		      regmbc(0x1e67); regmbc(0x1e69); regmbc(0xa7a9);
		      திரும்பு;
	    நிலை 't': நிலை 0x163: நிலை 0x165: நிலை 0x167: நிலை 0x1ab:
	    நிலை 0x1ad: நிலை 0x21b: நிலை 0x288: நிலை 0x1d75: நிலை 0x1e6b:
	    நிலை 0x1e6d: நிலை 0x1e6f: நிலை 0x1e71: நிலை 0x1e97: நிலை 0x2c66:
		      regmbc('t'); regmbc(0x163); regmbc(0x165);
		      regmbc(0x167); regmbc(0x1ab); regmbc(0x21b);
		      regmbc(0x1ad); regmbc(0x288); regmbc(0x1d75);
		      regmbc(0x1e6b); regmbc(0x1e6d); regmbc(0x1e6f);
		      regmbc(0x1e71); regmbc(0x1e97); regmbc(0x2c66);
		      திரும்பு;
	    நிலை 'u': நிலை 0xf9: நிலை 0xfa: நிலை 0xfb: நிலை 0xfc:
	    நிலை 0x169: நிலை 0x16b: நிலை 0x16d: நிலை 0x16f:
	    நிலை 0x171: நிலை 0x173: நிலை 0x1b0: நிலை 0x1d4:
	    நிலை 0x1d6: நிலை 0x1d8: நிலை 0x1da: நிலை 0x1dc:
	    நிலை 0x215: நிலை 0x217: நிலை 0x289: நிலை 0x1e73:
	    நிலை 0x1d7e: நிலை 0x1d99: நிலை 0x1e75: நிலை 0x1e77:
	    நிலை 0x1e79: நிலை 0x1e7b: நிலை 0x1ee5: நிலை 0x1ee7:
	    நிலை 0x1ee9: நிலை 0x1eeb: நிலை 0x1eed: நிலை 0x1eef:
	    நிலை 0x1ef1:
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
		      திரும்பு;
	    நிலை 'v': நிலை 0x28b: நிலை 0x1d8c: நிலை 0x1e7d: நிலை 0x1e7f:
		      regmbc('v'); regmbc(0x28b); regmbc(0x1d8c);
		      regmbc(0x1e7d); regmbc(0x1e7f);
		      திரும்பு;
	    நிலை 'w': நிலை 0x175: நிலை 0x1e81: நிலை 0x1e83:
	    நிலை 0x1e85: நிலை 0x1e87: நிலை 0x1e89: நிலை 0x1e98:
		      regmbc('w'); regmbc(0x175); regmbc(0x1e81);
		      regmbc(0x1e83); regmbc(0x1e85); regmbc(0x1e87);
		      regmbc(0x1e89); regmbc(0x1e98);
		      திரும்பு;
	    நிலை 'x': நிலை 0x1e8b: நிலை 0x1e8d:
		      regmbc('x'); regmbc(0x1e8b); regmbc(0x1e8d);
		      திரும்பு;
	    நிலை 'y': நிலை 0xfd: நிலை 0xff: நிலை 0x177: நிலை 0x1b4:
	    நிலை 0x233: நிலை 0x24f: நிலை 0x1e8f: நிலை 0x1e99: நிலை 0x1ef3:
	    நிலை 0x1ef5: நிலை 0x1ef7: நிலை 0x1ef9:
		      regmbc('y'); regmbc(0xfd); regmbc(0xff);
		      regmbc(0x177); regmbc(0x1b4); regmbc(0x233);
		      regmbc(0x24f); regmbc(0x1e8f); regmbc(0x1e99);
		      regmbc(0x1ef3); regmbc(0x1ef5); regmbc(0x1ef7);
		      regmbc(0x1ef9);
		      திரும்பு;
	    நிலை 'z': நிலை 0x17a: நிலை 0x17c: நிலை 0x17e: நிலை 0x1b6:
	    நிலை 0x1d76: நிலை 0x1d8e: நிலை 0x1e91: நிலை 0x1e93:
	    நிலை 0x1e95: நிலை 0x2c6c:
		      regmbc('z'); regmbc(0x17a); regmbc(0x17c);
		      regmbc(0x17e); regmbc(0x1b6); regmbc(0x1d76);
		      regmbc(0x1d8e); regmbc(0x1e91); regmbc(0x1e93);
		      regmbc(0x1e95); regmbc(0x2c6c);
		      திரும்பு;
	}
    }
    regmbc(c);
}

/*
 * Emit a node.
 * Return pointer to generated code.
 */
    நிலையான char_u *
regnode(முழு op)
{
    char_u  *ret;

    ret = regcode;
    எனில் (ret == JUST_CALC_SIZE)
	regsize += 3;
    இல்லையெனில்
    {
	*regcode++ = op;
	*regcode++ = NUL;		// Null "next" pointer.
	*regcode++ = NUL;
    }
    திரும்பு ret;
}

/*
 * Write a long as four bytes at "p" and return pointer to the next char.
 */
    நிலையான char_u *
re_put_long(char_u *p, long_u val)
{
    *p++ = (char_u) ((val >> 24) & 0377);
    *p++ = (char_u) ((val >> 16) & 0377);
    *p++ = (char_u) ((val >> 8) & 0377);
    *p++ = (char_u) (val & 0377);
    திரும்பு p;
}

/*
 * regnext - dig the "next" pointer out of a node
 * Returns NULL when calculating size, when there is no next item and when
 * there is an error.
 */
    நிலையான char_u *
regnext(char_u *p)
{
    முழு	    offset;

    எனில் (p == JUST_CALC_SIZE || reg_toolong)
	திரும்பு வெற்று;

    offset = NEXT(p);
    எனில் (offset == 0)
	திரும்பு வெற்று;

    எனில் (OP(p) == BACK)
	திரும்பு p - offset;
    இல்லையெனில்
	திரும்பு p + offset;
}

/*
 * Set the next-pointer at the end of a node chain.
 */
    நிலையான வெறுமை
regtail(char_u *p, char_u *val)
{
    char_u	*scan;
    char_u	*temp;
    முழு		offset;

    எனில் (p == JUST_CALC_SIZE)
	திரும்பு;

    // Find last node.
    scan = p;
    க்கு (;;)
    {
	temp = regnext(scan);
	எனில் (temp == வெற்று)
	    நிறுத்து;
	scan = temp;
    }

    எனில் (OP(scan) == BACK)
	offset = (முழு)(scan - val);
    இல்லையெனில்
	offset = (முழு)(val - scan);
    // When the offset uses more than 16 bits it can no longer fit in the two
    // bytes available.  Use a global flag to avoid having to check return
    // values in too many places.
    எனில் (offset > 0xffff)
	reg_toolong = TRUE;
    இல்லையெனில்
    {
	*(scan + 1) = (char_u) (((unsigned)offset >> 8) & 0377);
	*(scan + 2) = (char_u) (offset & 0377);
    }
}

/*
 * Like regtail, on item after a BRANCH; nop if none.
 */
    நிலையான வெறுமை
regoptail(char_u *p, char_u *val)
{
    // When op is neither BRANCH nor BRACE_COMPLEX0-9, it is "operandless"
    எனில் (p == வெற்று || p == JUST_CALC_SIZE
	    || (OP(p) != BRANCH
		&& (OP(p) < BRACE_COMPLEX || OP(p) > BRACE_COMPLEX + 9)))
	திரும்பு;
    regtail(OPERAND(p), val);
}

/*
 * Insert an operator in front of already-emitted operand
 *
 * Means relocating the operand.
 */
    நிலையான வெறுமை
reginsert(முழு op, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    எனில் (regcode == JUST_CALC_SIZE)
    {
	regsize += 3;
	திரும்பு;
    }
    src = regcode;
    regcode += 3;
    dst = regcode;
    போது (src > opnd)
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
    நிலையான வெறுமை
reginsert_nr(முழு op, long val, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    எனில் (regcode == JUST_CALC_SIZE)
    {
	regsize += 7;
	திரும்பு;
    }
    src = regcode;
    regcode += 7;
    dst = regcode;
    போது (src > opnd)
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
    நிலையான வெறுமை
reginsert_limits(
    முழு		op,
    long	minval,
    long	maxval,
    char_u	*opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    எனில் (regcode == JUST_CALC_SIZE)
    {
	regsize += 11;
	திரும்பு;
    }
    src = regcode;
    regcode += 11;
    dst = regcode;
    போது (src > opnd)
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
    நிலையான முழு
seen_endbrace(முழு refnum)
{
    எனில் (!had_endbrace[refnum])
    {
	char_u *p;

	// Trick: check if "@<=" or "@<!" follows, in which case
	// the \1 can appear before the referenced match.
	க்கு (p = regparse; *p != NUL; ++p)
	    எனில் (p[0] == '@' && p[1] == '<' && (p[2] == '!' || p[2] == '='))
		நிறுத்து;
	எனில் (*p == NUL)
	{
	    emsg(_(e_illegal_back_reference));
	    rc_did_emsg = TRUE;
	    திரும்பு FALSE;
	}
    }
    திரும்பு TRUE;
}

/*
 * Parse the lowest level.
 *
 * Optimization:  gobbles an entire sequence of ordinary characters so that
 * it can turn them into a single node, which is smaller to store and
 * faster to run.  Don't do this when one_exactly is set.
 */
    நிலையான char_u *
regatom(முழு *flagp)
{
    char_u	    *ret;
    முழு		    flags;
    முழு		    c;
    char_u	    *p;
    முழு		    extra = 0;
    முழு		    save_prev_at_start = prev_at_start;

    *flagp = WORST;		// Tentatively.

    c = getchr();
    தேர்வு (c)
    {
      நிலை Magic('^'):
	ret = regnode(BOL);
	நிறுத்து;

      நிலை Magic('$'):
	ret = regnode(EOL);
#எனில் defined(FEAT_SYN_HL)
	had_eol = TRUE;
#endif
	நிறுத்து;

      நிலை Magic('<'):
	ret = regnode(BOW);
	நிறுத்து;

      நிலை Magic('>'):
	ret = regnode(EOW);
	நிறுத்து;

      நிலை Magic('_'):
	c = no_Magic(getchr());
	எனில் (c == '^')		// "\_^" is start-of-line
	{
	    ret = regnode(BOL);
	    நிறுத்து;
	}
	எனில் (c == '$')		// "\_$" is end-of-line
	{
	    ret = regnode(EOL);
#எனில் defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#endif
	    நிறுத்து;
	}

	extra = ADD_NL;
	*flagp |= HASNL;

	// "\_[" is character range plus newline
	எனில் (c == '[')
	    செல் collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	// Character classes.
      நிலை Magic('.'):
      நிலை Magic('i'):
      நிலை Magic('I'):
      நிலை Magic('k'):
      நிலை Magic('K'):
      நிலை Magic('f'):
      நிலை Magic('F'):
      நிலை Magic('p'):
      நிலை Magic('P'):
      நிலை Magic('s'):
      நிலை Magic('S'):
      நிலை Magic('d'):
      நிலை Magic('D'):
      நிலை Magic('x'):
      நிலை Magic('X'):
      நிலை Magic('o'):
      நிலை Magic('O'):
      நிலை Magic('w'):
      நிலை Magic('W'):
      நிலை Magic('h'):
      நிலை Magic('H'):
      நிலை Magic('a'):
      நிலை Magic('A'):
      நிலை Magic('l'):
      நிலை Magic('L'):
      நிலை Magic('u'):
      நிலை Magic('U'):
	p = vim_strchr(classchars, no_Magic(c));
	எனில் (p == வெற்று)
	    EMSG_RET_NULL(_(e_invalid_use_of_underscore));

	// When '.' is followed by a composing char ignore the dot, so that
	// the composing char is matched here.
	எனில் (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	{
	    c = getchr();
	    செல் do_multibyte;
	}
	ret = regnode(classcodes[p - classchars] + extra);
	*flagp |= HASWIDTH | SIMPLE;
	நிறுத்து;

      நிலை Magic('n'):
	எனில் (reg_string)
	{
	    // In a string "\n" matches a newline character.
	    ret = regnode(EXACTLY);
	    regc(NL);
	    regc(NUL);
	    *flagp |= HASWIDTH | SIMPLE;
	}
	இல்லையெனில்
	{
	    // In buffer text "\n" matches the end of a line.
	    ret = regnode(NEWL);
	    *flagp |= HASWIDTH | HASNL;
	}
	நிறுத்து;

      நிலை Magic('('):
	எனில் (one_exactly)
	    EMSG_ONE_RET_NULL;
	ret = reg(REG_PAREN, &flags);
	எனில் (ret == வெற்று)
	    திரும்பு வெற்று;
	*flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
	நிறுத்து;

      நிலை NUL:
      நிலை Magic('|'):
      நிலை Magic('&'):
      நிலை Magic(')'):
	எனில் (one_exactly)
	    EMSG_ONE_RET_NULL;
	// Supposed to be caught earlier.
	IEMSG_RET_NULL(e_internal_error_in_regexp);
	// NOTREACHED

      நிலை Magic('='):
      நிலை Magic('?'):
      நிலை Magic('+'):
      நிலை Magic('@'):
      நிலை Magic('{'):
      நிலை Magic('*'):
	c = no_Magic(c);
	EMSG3_RET_NULL(_(e_str_chr_follows_nothing),
		(c == '*' ? reg_magic >= MAGIC_ON : reg_magic == MAGIC_ALL), c);
	// NOTREACHED

      நிலை Magic('~'):		// previous substitute pattern
	    எனில் (reg_prev_sub != வெற்று)
	    {
		char_u	    *lp;

		ret = regnode(EXACTLY);
		lp = reg_prev_sub;
		போது (*lp != NUL)
		    regc(*lp++);
		regc(NUL);
		எனில் (*reg_prev_sub != NUL)
		{
		    *flagp |= HASWIDTH;
		    எனில் ((lp - reg_prev_sub) == 1)
			*flagp |= SIMPLE;
		}
	    }
	    இல்லையெனில்
		EMSG_RET_NULL(_(e_no_previous_substitute_regular_expression));
	    நிறுத்து;

      நிலை Magic('1'):
      நிலை Magic('2'):
      நிலை Magic('3'):
      நிலை Magic('4'):
      நிலை Magic('5'):
      நிலை Magic('6'):
      நிலை Magic('7'):
      நிலை Magic('8'):
      நிலை Magic('9'):
	    {
		முழு		    refnum;

		refnum = c - Magic('0');
		எனில் (!seen_endbrace(refnum))
		    திரும்பு வெற்று;
		ret = regnode(BACKREF + refnum);
	    }
	    நிறுத்து;

      நிலை Magic('z'):
	{
	    c = no_Magic(getchr());
	    தேர்வு (c)
	    {
#ifdef FEAT_SYN_HL
		நிலை '(': எனில் ((reg_do_extmatch & REX_SET) == 0)
			      EMSG_RET_NULL(_(e_z_not_allowed_here));
			  எனில் (one_exactly)
			      EMSG_ONE_RET_NULL;
			  ret = reg(REG_ZPAREN, &flags);
			  எனில் (ret == வெற்று)
			      திரும்பு வெற்று;
			  *flagp |= flags & (HASWIDTH|SPSTART|HASNL|HASLOOKBH);
			  re_has_z = REX_SET;
			  நிறுத்து;

		நிலை '1':
		நிலை '2':
		நிலை '3':
		நிலை '4':
		நிலை '5':
		நிலை '6':
		நிலை '7':
		நிலை '8':
		நிலை '9': எனில் ((reg_do_extmatch & REX_USE) == 0)
			      EMSG_RET_NULL(_(e_z1_z9_not_allowed_here));
			  ret = regnode(ZREF + c - '0');
			  re_has_z = REX_USE;
			  நிறுத்து;
#endif

		நிலை 's': ret = regnode(MOPEN + 0);
			  எனில் (re_mult_next("\\zs") == FAIL)
			      திரும்பு வெற்று;
			  நிறுத்து;

		நிலை 'e': ret = regnode(MCLOSE + 0);
			  எனில் (re_mult_next("\\ze") == FAIL)
			      திரும்பு வெற்று;
			  நிறுத்து;

		இயல்பு:  EMSG_RET_NULL(_(e_invalid_character_after_bsl_z));
	    }
	}
	நிறுத்து;

      நிலை Magic('%'):
	{
	    c = no_Magic(getchr());
	    தேர்வு (c)
	    {
		// () without a back reference
		நிலை '(':
		    எனில் (one_exactly)
			EMSG_ONE_RET_NULL;
		    ret = reg(REG_NPAREN, &flags);
		    எனில் (ret == வெற்று)
			திரும்பு வெற்று;
		    *flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
		    நிறுத்து;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		நிலை '^':
		    ret = regnode(RE_BOF);
		    நிறுத்து;

		நிலை '$':
		    ret = regnode(RE_EOF);
		    நிறுத்து;

		நிலை '#':
		    எனில் (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_start_of_pattern),
								  regparse[1]);
			திரும்பு FAIL;
		    }
		    ret = regnode(CURSOR);
		    நிறுத்து;

		நிலை 'V':
		    ret = regnode(RE_VISUAL);
		    நிறுத்து;

		நிலை 'C':
		    ret = regnode(RE_COMPOSING);
		    நிறுத்து;

		// \%[abc]: Emit as a list of branches, all ending at the last
		// branch which matches nothing.
		நிலை '[':
			  எனில் (one_exactly)	// doesn't nest
			      EMSG_ONE_RET_NULL;
			  {
			      char_u	*lastbranch;
			      char_u	*lastnode = வெற்று;
			      char_u	*br;

			      ret = வெற்று;
			      போது ((c = getchr()) != ']')
			      {
				  எனில் (c == NUL)
				      EMSG2_RET_NULL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
				  br = regnode(BRANCH);
				  எனில் (ret == வெற்று)
				      ret = br;
				  இல்லையெனில்
				  {
				      regtail(lastnode, br);
				      எனில் (reg_toolong)
					  திரும்பு வெற்று;
				  }

				  ungetchr();
				  one_exactly = TRUE;
				  lastnode = regatom(flagp);
				  one_exactly = FALSE;
				  எனில் (lastnode == வெற்று)
				      திரும்பு வெற்று;
			      }
			      எனில் (ret == வெற்று)
				  EMSG2_RET_NULL(_(e_empty_str_brackets),
						      reg_magic == MAGIC_ALL);
			      lastbranch = regnode(BRANCH);
			      br = regnode(NOTHING);
			      எனில் (ret != JUST_CALC_SIZE)
			      {
				  regtail(lastnode, br);
				  regtail(lastbranch, br);
				  // connect all branches to the NOTHING
				  // branch at the end
				  க்கு (br = ret; br != lastnode; )
				  {
				      எனில் (OP(br) == BRANCH)
				      {
					  regtail(br, lastbranch);
					  எனில் (reg_toolong)
					      திரும்பு வெற்று;
					  br = OPERAND(br);
				      }
				      இல்லையெனில்
					  br = regnext(br);
				  }
			      }
			      *flagp &= ~(HASWIDTH | SIMPLE);
			      நிறுத்து;
			  }

		நிலை 'd':   // %d123 decimal
		நிலை 'o':   // %o123 octal
		நிலை 'x':   // %xab hex 2
		நிலை 'u':   // %uabcd hex 4
		நிலை 'U':   // %U1234abcd hex 8
			  {
			      vimlong_T i;

			      தேர்வு (c)
			      {
				  நிலை 'd': i = getdecchrs(); நிறுத்து;
				  நிலை 'o': i = getoctchrs(); நிறுத்து;
				  நிலை 'x': i = gethexchrs(2); நிறுத்து;
				  நிலை 'u': i = gethexchrs(4); நிறுத்து;
				  நிலை 'U': i = gethexchrs(8); நிறுத்து;
				  இயல்பு:  i = -1; நிறுத்து;
			      }

			      எனில் (i < 0 || i > INT_MAX)
				  EMSG2_RET_NULL(
					    _(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			      எனில் (use_multibytecode(i))
				  ret = regnode(MULTIBYTECODE);
			      இல்லையெனில்
				  ret = regnode(EXACTLY);
			      எனில் (i == 0)
				  regc(0x0a);
			      இல்லையெனில்
				  regmbc((முழு)i);
			      regc(NUL);
			      *flagp |= HASWIDTH;
			      நிறுத்து;
			  }

		இயல்பு:
			  எனில் (VIM_ISDIGIT(c) || c == '<' || c == '>'
						|| c == '\'' || c == '.')
			  {
			      long_u	n = 0;
			      முழு	cmp;
			      முழு	cur = FALSE;
			      முழு	got_digit = FALSE;

			      cmp = c;
			      எனில் (cmp == '<' || cmp == '>')
				  c = getchr();
			      எனில் (no_Magic(c) == '.')
			      {
				  cur = TRUE;
				  c = getchr();
			      }
			      போது (VIM_ISDIGIT(c))
			      {
				  got_digit = TRUE;
				  n = n * 10 + (c - '0');
				  c = getchr();
			      }
			      எனில் (no_Magic(c) == '\'' && n == 0)
			      {
				  // "\%'m", "\%<'m" and "\%>'m": Mark
				  c = getchr();
				  ret = regnode(RE_MARK);
				  எனில் (ret == JUST_CALC_SIZE)
				      regsize += 2;
				  இல்லையெனில்
				  {
				      *regcode++ = c;
				      *regcode++ = cmp;
				  }
				  நிறுத்து;
			      }
			      இல்லையெனில் எனில் ((c == 'l' || c == 'c' || c == 'v')
					  && (cur || got_digit))
			      {
				  எனில் (cur && n)
				  {
				    semsg(_(e_regexp_எண்_after_dot_pos_தேடல்_chr),
								  no_Magic(c));
				    rc_did_emsg = TRUE;
				    திரும்பு வெற்று;
				  }
				  எனில் (c == 'l')
				  {
				      எனில் (cur)
					  n = curwin->w_cursor.lnum;
				      ret = regnode(RE_LNUM);
				      எனில் (save_prev_at_start)
					  at_start = TRUE;
				  }
				  இல்லையெனில் எனில் (c == 'c')
				  {
				      எனில் (cur)
				      {
					  n = curwin->w_cursor.col;
					  n++;
				      }
				      ret = regnode(RE_COL);
				  }
				  இல்லையெனில்
				  {
				      எனில் (cur)
				      {
					  colnr_T vcol = 0;

					  getvvcol(curwin, &curwin->w_cursor,
							 வெற்று, வெற்று, &vcol, 0);
					  ++vcol;
					  n = vcol;
				      }
				      ret = regnode(RE_VCOL);
				  }
				  எனில் (ret == JUST_CALC_SIZE)
				      regsize += 5;
				  இல்லையெனில்
				  {
				      // put the number and the optional
				      // comparator after the opcode
				      regcode = re_put_long(regcode, n);
				      *regcode++ = cmp;
				  }
				  நிறுத்து;
			      }
			  }

			  EMSG2_RET_NULL(_(e_invalid_character_after_str),
						      reg_magic == MAGIC_ALL);
	    }
	}
	நிறுத்து;

      நிலை Magic('['):
collection:
	{
	    char_u	*lp;

	    // If there is no matching ']', we assume the '[' is a normal
	    // character.  This makes 'incsearch' and ":help [" work.
	    lp = skip_anyof(regparse);
	    எனில் (*lp == ']')	// there is a matching ']'
	    {
		முழு	startc = -1;	// > 0 when next '-' is a range
		முழு	endc;

		எனில் (extra == 0)
		{
		    முழு	cl = bt_recognize_char_class(regparse, lp);

		    எனில் (cl != 0)
		    {
			ret = regnode(cl);
			regparse = lp;
			skipchr();
			*flagp |= HASWIDTH | SIMPLE;
			நிறுத்து;
		    }
		}

		// In a character class, different parsing rules apply.
		// Not even \ is special anymore, nothing is.
		எனில் (*regparse == '^')	    // Complement of range.
		{
		    ret = regnode(ANYBUT + extra);
		    regparse++;
		}
		இல்லையெனில்
		    ret = regnode(ANYOF + extra);

		// At the start ']' and '-' mean the literal character.
		எனில் (*regparse == ']' || *regparse == '-')
		{
		    startc = *regparse;
		    regc(*regparse++);
		}

		போது (*regparse != NUL && *regparse != ']')
		{
		    எனில் (*regparse == '-')
		    {
			++regparse;
			// The '-' is not used for a range at the end and
			// after or before a '\n'.
			எனில் (*regparse == ']' || *regparse == NUL
				|| startc == -1
				|| (regparse[0] == '\\' && regparse[1] == 'n'))
			{
			    regc('-');
			    startc = '-';	// [--x] is a range
			}
			இல்லையெனில்
			{
			    // Also accept "a-[.z.]"
			    endc = 0;
			    எனில் (*regparse == '[')
				endc = get_coll_element(&regparse);
			    எனில் (endc == 0)
			    {
				எனில் (has_mbyte)
				    endc = mb_ptr2char_adv(&regparse);
				இல்லையெனில்
				    endc = *regparse++;
			    }

			    // Handle \o40, \x20 and \u20AC style sequences
			    எனில் (endc == '\\' && !reg_cpo_lit && !reg_cpo_bsl)
				endc = coll_get_char();

			    எனில் (startc > endc)
				EMSG_RET_NULL(_(e_reverse_range_in_character_class));
			    எனில் (has_mbyte && ((*mb_char2len)(startc) > 1
						 || (*mb_char2len)(endc) > 1))
			    {
				// Limit to a range of 256 chars.
				எனில் (endc > startc + 256)
				    EMSG_RET_NULL(_(e_range_too_large_in_character_class));
				போது (++startc <= endc)
				    regmbc(startc);
			    }
			    இல்லையெனில்
			    {
				போது (++startc <= endc)
				    regc(startc);
			    }
			    startc = -1;
			}
		    }
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    இல்லையெனில் எனில் (*regparse == '\\'
			    && !reg_cpo_bsl
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != வெற்று
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR,
						       regparse[1]) != வெற்று)))
		    {
			regparse++;
			எனில் (*regparse == 'n')
			{
			    // '\n' in range: also match NL
			    எனில் (ret != JUST_CALC_SIZE)
			    {
				// Using \n inside [^] does not change what
				// matches. "[^\n]" is the same as ".".
				எனில் (*ret == ANYOF)
				{
				    *ret = ANYOF + ADD_NL;
				    *flagp |= HASNL;
				}
				// else: must have had a \n already
			    }
			    regparse++;
			    startc = -1;
			}
			இல்லையெனில் எனில் (*regparse == 'd'
				|| *regparse == 'o'
				|| *regparse == 'x'
				|| *regparse == 'u'
				|| *regparse == 'U')
			{
			    startc = coll_get_char();
			    // max UTF-8 Codepoint is U+10FFFF,
			    // but allow values until INT_MAX
			    எனில் (startc == INT_MAX)
				EMSG_RET_NULL(_(e_unicode_val_too_large));
			    எனில் (startc == 0)
				regc(0x0a);
			    இல்லையெனில்
				regmbc(startc);
			}
			இல்லையெனில்
			{
			    startc = backslash_trans(*regparse++);
			    regc(startc);
			}
		    }
		    இல்லையெனில் எனில் (*regparse == '[')
		    {
			முழு c_class;
			முழு cu;

			c_class = get_char_class(&regparse);
			startc = -1;
			// Characters assumed to be 8 bits!
			தேர்வு (c_class)
			{
			    நிலை CLASS_NONE:
				c_class = get_equi_class(&regparse);
				எனில் (c_class != 0)
				{
				    // produce equivalence class
				    reg_equi_class(c_class);
				}
				இல்லையெனில் எனில் ((c_class =
					    get_coll_element(&regparse)) != 0)
				{
				    // produce a collating element
				    regmbc(c_class);
				}
				இல்லையெனில்
				{
				    // literal '[', allow [[-x] as a range
				    startc = *regparse++;
				    regc(startc);
				}
				நிறுத்து;
			    நிலை CLASS_ALNUM:
				க்கு (cu = 1; cu < 128; cu++)
				    எனில் (isalnum(cu))
					regmbc(cu);
				நிறுத்து;
			    நிலை CLASS_ALPHA:
				க்கு (cu = 1; cu < 128; cu++)
				    எனில் (isalpha(cu))
					regmbc(cu);
				நிறுத்து;
			    நிலை CLASS_BLANK:
				regc(' ');
				regc('\t');
				நிறுத்து;
			    நிலை CLASS_CNTRL:
				க்கு (cu = 1; cu <= 127; cu++)
				    எனில் (iscntrl(cu))
					regmbc(cu);
				நிறுத்து;
			    நிலை CLASS_DIGIT:
				க்கு (cu = 1; cu <= 127; cu++)
				    எனில் (VIM_ISDIGIT(cu))
					regmbc(cu);
				நிறுத்து;
			    நிலை CLASS_GRAPH:
				க்கு (cu = 1; cu <= 127; cu++)
				    எனில் (isgraph(cu))
					regmbc(cu);
				நிறுத்து;
			    நிலை CLASS_LOWER:
				க்கு (cu = 1; cu <= 255; cu++)
				    எனில் (MB_ISLOWER(cu) && cu != 170
								 && cu != 186)
					regmbc(cu);
				நிறுத்து;
			    நிலை CLASS_PRINT:
				க்கு (cu = 1; cu <= 255; cu++)
				    எனில் (vim_isprintc(cu))
					regmbc(cu);
				நிறுத்து;
			    நிலை CLASS_PUNCT:
				க்கு (cu = 1; cu < 128; cu++)
				    எனில் (ispunct(cu))
					regmbc(cu);
				நிறுத்து;
			    நிலை CLASS_SPACE:
				க்கு (cu = 9; cu <= 13; cu++)
				    regc(cu);
				regc(' ');
				நிறுத்து;
			    நிலை CLASS_UPPER:
				க்கு (cu = 1; cu <= 255; cu++)
				    எனில் (MB_ISUPPER(cu))
					regmbc(cu);
				நிறுத்து;
			    நிலை CLASS_XDIGIT:
				க்கு (cu = 1; cu <= 255; cu++)
				    எனில் (vim_isxdigit(cu))
					regmbc(cu);
				நிறுத்து;
			    நிலை CLASS_TAB:
				regc('\t');
				நிறுத்து;
			    நிலை CLASS_RETURN:
				regc('\r');
				நிறுத்து;
			    நிலை CLASS_BACKSPACE:
				regc('\b');
				நிறுத்து;
			    நிலை CLASS_ESCAPE:
				regc('\033');
				நிறுத்து;
			    நிலை CLASS_IDENT:
				க்கு (cu = 1; cu <= 255; cu++)
				    எனில் (vim_isIDc(cu))
					regmbc(cu);
				நிறுத்து;
			    நிலை CLASS_KEYWORD:
				க்கு (cu = 1; cu <= 255; cu++)
				    எனில் (reg_iswordc(cu))
					regmbc(cu);
				நிறுத்து;
			    நிலை CLASS_FNAME:
				க்கு (cu = 1; cu <= 255; cu++)
				    எனில் (vim_isfilec(cu))
					regmbc(cu);
				நிறுத்து;
			}
		    }
		    இல்லையெனில்
		    {
			எனில் (has_mbyte)
			{
			    முழு	len;

			    // produce a multibyte character, including any
			    // following composing characters
			    startc = mb_ptr2char(regparse);
			    len = (*mb_ptr2len)(regparse);
			    எனில் (enc_utf8 && utf_char2len(startc) != len)
				startc = -1;	// composing chars
			    போது (--len >= 0)
				regc(*regparse++);
			}
			இல்லையெனில்
			{
			    startc = *regparse++;
			    regc(startc);
			}
		    }
		}
		regc(NUL);
		prevchr_len = 1;	// last char was the ']'
		எனில் (*regparse != ']')
		    EMSG_RET_NULL(_(e_too_many_brackets));  // Cannot happen?
		skipchr();	    // let's be friends with the lexer again
		*flagp |= HASWIDTH | SIMPLE;
		நிறுத்து;
	    }
	    இல்லையெனில் எனில் (reg_strict)
		EMSG2_RET_NULL(_(e_missing_rsb_after_str_lsb),
							reg_magic > MAGIC_OFF);
	}
	// FALLTHROUGH

      இயல்பு:
	{
	    முழு		len;

	    // A multi-byte character is handled as a separate atom if it's
	    // before a multi and when it's a composing char.
	    எனில் (use_multibytecode(c))
	    {
do_multibyte:
		ret = regnode(MULTIBYTECODE);
		regmbc(c);
		*flagp |= HASWIDTH | SIMPLE;
		நிறுத்து;
	    }

	    ret = regnode(EXACTLY);

	    // Append characters as long as:
	    // - there is no following multi, we then need the character in
	    //   front of it as a single character operand
	    // - not running into a Magic character
	    // - "one_exactly" is not set
	    // But always emit at least one character.  Might be a Multi,
	    // e.g., a "[" without matching "]".
	    க்கு (len = 0; c != NUL && (len == 0
			|| (re_multi_type(peekchr()) == NOT_MULTI
			    && !one_exactly
			    && !is_Magic(c))); ++len)
	    {
		c = no_Magic(c);
		எனில் (has_mbyte)
		{
		    regmbc(c);
		    எனில் (enc_utf8)
		    {
			முழு	l;

			// Need to get composing character too.
			க்கு (;;)
			{
			    l = utf_ptr2len(regparse);
			    எனில் (!UTF_COMPOSINGLIKE(regparse, regparse + l))
				நிறுத்து;
			    regmbc(utf_ptr2char(regparse));
			    skipchr();
			}
		    }
		}
		இல்லையெனில்
		    regc(c);
		c = getchr();
	    }
	    ungetchr();

	    regc(NUL);
	    *flagp |= HASWIDTH;
	    எனில் (len == 1)
		*flagp |= SIMPLE;
	}
	நிறுத்து;
    }

    திரும்பு ret;
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
    நிலையான char_u *
regpiece(முழு *flagp)
{
    char_u	    *ret;
    முழு		    op;
    char_u	    *next;
    முழு		    flags;
    long	    minval;
    long	    maxval;

    ret = regatom(&flags);
    எனில் (ret == வெற்று)
	திரும்பு வெற்று;

    op = peekchr();
    எனில் (re_multi_type(op) == NOT_MULTI)
    {
	*flagp = flags;
	திரும்பு ret;
    }
    // default flags
    *flagp = (WORST | SPSTART | (flags & (HASNL | HASLOOKBH)));

    skipchr();
    தேர்வு (op)
    {
	நிலை Magic('*'):
	    எனில் (flags & SIMPLE)
		reginsert(STAR, ret);
	    இல்லையெனில்
	    {
		// Emit x* as (x&|), where & means "self".
		reginsert(BRANCH, ret); // Either x
		regoptail(ret, regnode(BACK));	// and loop
		regoptail(ret, ret);	// back
		regtail(ret, regnode(BRANCH));	// or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    நிறுத்து;

	நிலை Magic('+'):
	    எனில் (flags & SIMPLE)
		reginsert(PLUS, ret);
	    இல்லையெனில்
	    {
		// Emit x+ as x(&|), where & means "self".
		next = regnode(BRANCH); // Either
		regtail(ret, next);
		regtail(regnode(BACK), ret);	// loop back
		regtail(next, regnode(BRANCH)); // or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    *flagp = (WORST | HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    நிறுத்து;

	நிலை Magic('@'):
	    {
		முழு	lop = END;
		long	nr;

		nr = (long)getdecchrs();
		தேர்வு (no_Magic(getchr()))
		{
		    நிலை '=': lop = MATCH; நிறுத்து;		  // \@=
		    நிலை '!': lop = NOMATCH; நிறுத்து;		  // \@!
		    நிலை '>': lop = SUBPAT; நிறுத்து;		  // \@>
		    நிலை '<': தேர்வு (no_Magic(getchr()))
			      {
				  நிலை '=': lop = BEHIND; நிறுத்து;   // \@<=
				  நிலை '!': lop = NOBEHIND; நிறுத்து; // \@<!
			      }
		}
		எனில் (lop == END)
		    EMSG2_RET_NULL(_(e_invalid_character_after_str_at),
						      reg_magic == MAGIC_ALL);
		// Look behind must match with behind_pos.
		எனில் (lop == BEHIND || lop == NOBEHIND)
		{
		    regtail(ret, regnode(BHPOS));
		    *flagp |= HASLOOKBH;
		}
		regtail(ret, regnode(END)); // operand ends
		எனில் (lop == BEHIND || lop == NOBEHIND)
		{
		    எனில் (nr < 0)
			nr = 0; // no limit is same as zero limit
		    reginsert_nr(lop, nr, ret);
		}
		இல்லையெனில்
		    reginsert(lop, ret);
		நிறுத்து;
	    }

	நிலை Magic('?'):
	நிலை Magic('='):
	    // Emit x= as (x|)
	    reginsert(BRANCH, ret);		// Either x
	    regtail(ret, regnode(BRANCH));	// or
	    next = regnode(NOTHING);		// null.
	    regtail(ret, next);
	    regoptail(ret, next);
	    நிறுத்து;

	நிலை Magic('{'):
	    எனில் (!read_limits(&minval, &maxval))
		திரும்பு வெற்று;
	    எனில் (flags & SIMPLE)
	    {
		reginsert(BRACE_SIMPLE, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
	    }
	    இல்லையெனில்
	    {
		எனில் (இலக்கம்_complex_braces >= 10)
		    EMSG2_RET_NULL(_(e_too_many_complex_str_curly),
						      reg_magic == MAGIC_ALL);
		reginsert(BRACE_COMPLEX + இலக்கம்_complex_braces, ret);
		regoptail(ret, regnode(BACK));
		regoptail(ret, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
		++இலக்கம்_complex_braces;
	    }
	    எனில் (minval > 0 && maxval > 0)
		*flagp = (HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    நிறுத்து;
    }
    எனில் (re_multi_type(peekchr()) != NOT_MULTI)
    {
	// Can't have a multi follow a multi.
	எனில் (peekchr() == Magic('*'))
	    EMSG2_RET_NULL(_(e_nested_str), reg_magic >= MAGIC_ON);
	EMSG3_RET_NULL(_(e_nested_str_chr), reg_magic == MAGIC_ALL,
							  no_Magic(peekchr()));
    }

    திரும்பு ret;
}

/*
 * Parse one alternative of an | or & operator.
 * Implements the concatenation operator.
 */
    நிலையான char_u *
regconcat(முழு *flagp)
{
    char_u	*first = வெற்று;
    char_u	*chain = வெற்று;
    char_u	*latest;
    முழு		flags;
    முழு		cont = TRUE;

    *flagp = WORST;		// Tentatively.

    போது (cont)
    {
	தேர்வு (peekchr())
	{
	    நிலை NUL:
	    நிலை Magic('|'):
	    நிலை Magic('&'):
	    நிலை Magic(')'):
			    cont = FALSE;
			    நிறுத்து;
	    நிலை Magic('Z'):
			    regflags |= RF_ICOMBINE;
			    skipchr_keepstart();
			    நிறுத்து;
	    நிலை Magic('c'):
			    regflags |= RF_ICASE;
			    skipchr_keepstart();
			    நிறுத்து;
	    நிலை Magic('C'):
			    regflags |= RF_NOICASE;
			    skipchr_keepstart();
			    நிறுத்து;
	    நிலை Magic('v'):
			    reg_magic = MAGIC_ALL;
			    skipchr_keepstart();
			    curchr = -1;
			    நிறுத்து;
	    நிலை Magic('m'):
			    reg_magic = MAGIC_ON;
			    skipchr_keepstart();
			    curchr = -1;
			    நிறுத்து;
	    நிலை Magic('M'):
			    reg_magic = MAGIC_OFF;
			    skipchr_keepstart();
			    curchr = -1;
			    நிறுத்து;
	    நிலை Magic('V'):
			    reg_magic = MAGIC_NONE;
			    skipchr_keepstart();
			    curchr = -1;
			    நிறுத்து;
	    இயல்பு:
			    latest = regpiece(&flags);
			    எனில் (latest == வெற்று || reg_toolong)
				திரும்பு வெற்று;
			    *flagp |= flags & (HASWIDTH | HASNL | HASLOOKBH);
			    எனில் (chain == வெற்று)	// First piece.
				*flagp |= flags & SPSTART;
			    இல்லையெனில்
				regtail(chain, latest);
			    chain = latest;
			    எனில் (first == வெற்று)
				first = latest;
			    நிறுத்து;
	}
    }
    எனில் (first == வெற்று)		// Loop ran zero times.
	first = regnode(NOTHING);
    திரும்பு first;
}

/*
 * Parse one alternative of an | operator.
 * Implements the & operator.
 */
    நிலையான char_u *
regbranch(முழு *flagp)
{
    char_u	*ret;
    char_u	*chain = வெற்று;
    char_u	*latest;
    முழு		flags;

    *flagp = WORST | HASNL;		// Tentatively.

    ret = regnode(BRANCH);
    க்கு (;;)
    {
	latest = regconcat(&flags);
	எனில் (latest == வெற்று)
	    திரும்பு வெற்று;
	// If one of the branches has width, the whole thing has.  If one of
	// the branches anchors at start-of-line, the whole thing does.
	// If one of the branches uses look-behind, the whole thing does.
	*flagp |= flags & (HASWIDTH | SPSTART | HASLOOKBH);
	// If one of the branches doesn't match a line-break, the whole thing
	// doesn't.
	*flagp &= ~HASNL | (flags & HASNL);
	எனில் (chain != வெற்று)
	    regtail(chain, latest);
	எனில் (peekchr() != Magic('&'))
	    நிறுத்து;
	skipchr();
	regtail(latest, regnode(END)); // operand ends
	எனில் (reg_toolong)
	    நிறுத்து;
	reginsert(MATCH, latest);
	chain = latest;
    }

    திரும்பு ret;
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
    நிலையான char_u *
reg(
    முழு		paren,	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
    முழு		*flagp)
{
    char_u	*ret;
    char_u	*br;
    char_u	*ender;
    முழு		parno = 0;
    முழு		flags;

    *flagp = HASWIDTH;		// Tentatively.

#ifdef FEAT_SYN_HL
    எனில் (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	எனில் (regnzpar >= NSUBEXP)
	    EMSG_RET_NULL(_(e_too_many_z));
	parno = regnzpar;
	regnzpar++;
	ret = regnode(ZOPEN + parno);
    }
    இல்லையெனில்
#endif
	எனில் (paren == REG_PAREN)
    {
	// Make a MOPEN node.
	எனில் (regnpar >= NSUBEXP)
	    EMSG2_RET_NULL(_(e_too_many_str_open), reg_magic == MAGIC_ALL);
	parno = regnpar;
	++regnpar;
	ret = regnode(MOPEN + parno);
    }
    இல்லையெனில் எனில் (paren == REG_NPAREN)
    {
	// Make a NOPEN node.
	ret = regnode(NOPEN);
    }
    இல்லையெனில்
	ret = வெற்று;

    எனில் (bt_reg_parse_depth >= REG_MAX_PAREN_DEPTH)
	EMSG_RET_NULL(_(e_command_too_complex));
    ++bt_reg_parse_depth;

    // Pick up the branches, linking them together.
    br = regbranch(&flags);
    எனில் (br == வெற்று)
    {
	ret = வெற்று;
	செல் theend;
    }
    எனில் (ret != வெற்று)
	regtail(ret, br);	// [MZ]OPEN -> first.
    இல்லையெனில்
	ret = br;
    // If one of the branches can be zero-width, the whole thing can.
    // If one of the branches has * at start or matches a line-break, the
    // whole thing can.
    எனில் (!(flags & HASWIDTH))
	*flagp &= ~HASWIDTH;
    *flagp |= flags & (SPSTART | HASNL | HASLOOKBH);
    போது (peekchr() == Magic('|'))
    {
	skipchr();
	br = regbranch(&flags);
	எனில் (br == வெற்று || reg_toolong)
	{
	    ret = வெற்று;
	    செல் theend;
	}
	regtail(ret, br);	// BRANCH -> BRANCH.
	எனில் (!(flags & HASWIDTH))
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
    க்கு (br = ret; br != வெற்று; br = regnext(br))
	regoptail(br, ender);

    // Check for proper termination.
    எனில் (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
#ifdef FEAT_SYN_HL
	எனில் (paren == REG_ZPAREN)
	{
	    emsg(_(e_unmatched_z));
	    rc_did_emsg = TRUE;
	    ret = வெற்று;
	    செல் theend;
	}
	இல்லையெனில்
#endif
	எனில் (paren == REG_NPAREN)
	{
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = வெற்று;
	    செல் theend;
	}
	இல்லையெனில்
	{
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = வெற்று;
	    செல் theend;
	}
    }
    இல்லையெனில் எனில் (paren == REG_NOPAREN && peekchr() != NUL)
    {
	எனில் (curchr == Magic(')'))
	{
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = வெற்று;
	    செல் theend;
	}
	இல்லையெனில்
	{
	    emsg(_(e_trailing_characters));	// "Can't happen".
	    rc_did_emsg = TRUE;
	    ret = வெற்று;
	    செல் theend;
	}
    }
    // Here we set the flag allowing back references to this set of
    // parentheses.
    எனில் (paren == REG_PAREN)
	had_endbrace[parno] = TRUE;	// have seen the close paren

theend:
    --bt_reg_parse_depth;
    திரும்பு ret;
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
    நிலையான regprog_T *
bt_regcomp(char_u *கோவை, முழு re_flags)
{
    bt_regprog_T    *r;
    char_u	*scan;
    char_u	*longest;
    முழு		len;
    முழு		flags;

    எனில் (கோவை == வெற்று)
	IEMSG_RET_NULL(e_null_argument);

    init_class_tab();

    // First pass: determine size, legality.
    regcomp_start(கோவை, re_flags);
    regcode = JUST_CALC_SIZE;
    regc(REGMAGIC);
    எனில் (reg(REG_NOPAREN, &flags) == வெற்று)
	திரும்பு வெற்று;

    // Allocate space.
    r = alloc(offsetof(bt_regprog_T, program) + regsize);
    எனில் (r == வெற்று)
	திரும்பு வெற்று;
    r->re_in_use = FALSE;
#ifdef DEBUG
    r->regsz = regsize;
#endif

    // Second pass: emit code.
    regcomp_start(கோவை, re_flags);
    regcode = r->program;
    regc(REGMAGIC);
    எனில் (reg(REG_NOPAREN, &flags) == வெற்று || reg_toolong)
    {
	vim_free(r);
	எனில் (reg_toolong)
	    EMSG_RET_NULL(_(e_pattern_too_long));
	திரும்பு வெற்று;
    }

    // Dig out information for optimizations.
    r->regstart = NUL;		// Worst-case defaults.
    r->reganch = 0;
    r->regmust = வெற்று;
    r->regmlen = 0;
    r->regflags = regflags;
    எனில் (flags & HASNL)
	r->regflags |= RF_HASNL;
    எனில் (flags & HASLOOKBH)
	r->regflags |= RF_LOOKBH;
#ifdef FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    r->reghasz = re_has_z;
#endif
    scan = r->program + 1;	// First BRANCH.
    எனில் (OP(regnext(scan)) == END)   // Only one top-level choice.
    {
	scan = OPERAND(scan);

	// Starting-point info.
	எனில் (OP(scan) == BOL || OP(scan) == RE_BOF)
	{
	    r->reganch++;
	    scan = regnext(scan);
	}

	எனில் (OP(scan) == EXACTLY)
	{
	    எனில் (has_mbyte)
		r->regstart = (*mb_ptr2char)(OPERAND(scan));
	    இல்லையெனில்
		r->regstart = *OPERAND(scan);
	}
	இல்லையெனில் எனில் ((OP(scan) == BOW
		    || OP(scan) == EOW
		    || OP(scan) == NOTHING
		    || OP(scan) == MOPEN + 0 || OP(scan) == NOPEN
		    || OP(scan) == MCLOSE + 0 || OP(scan) == NCLOSE)
		 && OP(regnext(scan)) == EXACTLY)
	{
	    எனில் (has_mbyte)
		r->regstart = (*mb_ptr2char)(OPERAND(regnext(scan)));
	    இல்லையெனில்
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
	எனில் ((flags & SPSTART || OP(scan) == BOW || OP(scan) == EOW)
							  && !(flags & HASNL))
	{
	    size_t  scanlen;

	    longest = வெற்று;
	    len = 0;
	    க்கு (; scan != வெற்று; scan = regnext(scan))
	    {
		எனில் (OP(scan) == EXACTLY)
		{
		    scanlen = STRLEN(OPERAND(scan));
		    எனில் (scanlen >= (size_t)len)
		    {
			longest = OPERAND(scan);
			len = (முழு)scanlen;
		    }
		}
	    }
	    r->regmust = longest;
	    r->regmlen = len;
	}
    }
#ifdef BT_REGEXP_DUMP
    regdump(கோவை, r);
#endif
    r->engine = &bt_regengine;
    திரும்பு (regprog_T *)r;
}

#எனில் defined(FEAT_SYN_HL)
/*
 * Check if during the previous call to vim_regcomp the EOL item "$" has been
 * found.  This is messy, but it works fine.
 */
    முழு
vim_regcomp_had_eol(வெறுமை)
{
    திரும்பு had_eol;
}
#endif

/*
 * Get a number after a backslash that is inside [].
 * When nothing is recognized return a backslash.
 */
    நிலையான முழு
coll_get_char(வெறுமை)
{
    vimlong_T	nr = -1;

    தேர்வு (*regparse++)
    {
	நிலை 'd': nr = getdecchrs(); நிறுத்து;
	நிலை 'o': nr = getoctchrs(); நிறுத்து;
	நிலை 'x': nr = gethexchrs(2); நிறுத்து;
	நிலை 'u': nr = gethexchrs(4); நிறுத்து;
	நிலை 'U': nr = gethexchrs(8); நிறுத்து;
    }
    எனில் (nr < 0)
    {
	// If getting the number fails be backwards compatible: the character
	// is a backslash.
	--regparse;
	nr = '\\';
    }
    எனில் (nr > INT_MAX)
	nr = INT_MAX;
    திரும்பு nr;
}

/*
 * Free a compiled regexp program, returned by bt_regcomp().
 */
    நிலையான வெறுமை
bt_regfree(regprog_T *prog)
{
    vim_free(prog);
}

#define ADVANCE_REGINPUT() MB_PTR_ADV(rex.உள்ளீடு)

/*
 * The arguments from BRACE_LIMITS are stored here.  They are actually local
 * to regmatch(), but they are here to reduce the amount of stack space used
 * (it can be called recursively many times).
 */
நிலையான long	bl_minval;
நிலையான long	bl_maxval;

/*
 * Save the input line and position in a regsave_T.
 */
    நிலையான வெறுமை
reg_save(regsave_T *save, garray_T *gap)
{
    எனில் (REG_MULTI)
    {
	save->rs_u.pos.col = (colnr_T)(rex.உள்ளீடு - rex.வரி);
	save->rs_u.pos.lnum = rex.lnum;
    }
    இல்லையெனில்
	save->rs_u.ptr = rex.உள்ளீடு;
    save->rs_len = gap->ga_len;
}

/*
 * Restore the input line and position from a regsave_T.
 */
    நிலையான வெறுமை
reg_restore(regsave_T *save, garray_T *gap)
{
    எனில் (REG_MULTI)
    {
	எனில் (rex.lnum != save->rs_u.pos.lnum)
	{
	    // only call reg_getline() when the line number changed to save
	    // a bit of time
	    rex.lnum = save->rs_u.pos.lnum;
	    rex.வரி = reg_getline(rex.lnum);
	}
	rex.உள்ளீடு = rex.வரி + save->rs_u.pos.col;
    }
    இல்லையெனில்
	rex.உள்ளீடு = save->rs_u.ptr;
    gap->ga_len = save->rs_len;
}

/*
 * Return TRUE if current position is equal to saved position.
 */
    நிலையான முழு
reg_save_equal(regsave_T *save)
{
    எனில் (REG_MULTI)
	திரும்பு rex.lnum == save->rs_u.pos.lnum
				  && rex.உள்ளீடு == rex.வரி + save->rs_u.pos.col;
    திரும்பு rex.உள்ளீடு == save->rs_u.ptr;
}

// Save the sub-expressions before attempting a match.
#define save_se(savep, posp, pp) \
    REG_MULTI ? save_se_multi((savep), (posp)) : save_se_one((savep), (pp))

// After a failed match restore the sub-expressions.
#define restore_se(savep, posp, pp) \
{ \
    எனில் (REG_MULTI) \
	*(posp) = (savep)->se_u.pos; \
    இல்லையெனில் \
	*(pp) = (savep)->se_u.ptr; \
}

/*
 * Tentatively set the sub-expression start to the current position (after
 * calling regmatch() they will have changed).  Need to save the existing
 * values for when there is no match.
 * Use se_save() to use pointer (save_se_multi()) or position (save_se_one()),
 * depending on REG_MULTI.
 */
    நிலையான வெறுமை
save_se_multi(save_se_T *savep, lpos_T *posp)
{
    savep->se_u.pos = *posp;
    posp->lnum = rex.lnum;
    posp->col = (colnr_T)(rex.உள்ளீடு - rex.வரி);
}

    நிலையான வெறுமை
save_se_one(save_se_T *savep, char_u **pp)
{
    savep->se_u.ptr = *pp;
    *pp = rex.உள்ளீடு;
}

/*
 * regrepeat - repeatedly match something simple, return how many.
 * Advances rex.input (and rex.lnum) to just after the matched chars.
 */
    நிலையான முழு
regrepeat(
    char_u	*p,
    long	maxcount)   // maximum number of matches allowed
{
    long	மொத்தம் = 0;
    char_u	*scan;
    char_u	*opnd;
    முழு		mask;
    முழு		testval = 0;

    scan = rex.உள்ளீடு;	    // Make local copy of rex.input for speed.
    opnd = OPERAND(p);
    தேர்வு (OP(p))
    {
      நிலை ANY:
      நிலை ANY + ADD_NL:
	போது (மொத்தம் < maxcount)
	{
	    // Matching anything means we continue until end-of-line (or
	    // end-of-file for ANY + ADD_NL), only limited by maxcount.
	    போது (*scan != NUL && மொத்தம் < maxcount)
	    {
		++மொத்தம்;
		MB_PTR_ADV(scan);
	    }
	    எனில் (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
				      || rex.reg_வரி_lbr || மொத்தம் == maxcount)
		நிறுத்து;
	    ++மொத்தம்;		// count the line-break
	    reg_nextline();
	    scan = rex.உள்ளீடு;
	    எனில் (got_int)
		நிறுத்து;
	}
	நிறுத்து;

      நிலை IDENT:
      நிலை IDENT + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      நிலை SIDENT:
      நிலை SIDENT + ADD_NL:
	போது (மொத்தம் < maxcount)
	{
	    எனில் (vim_isIDc(PTR2CHAR(scan)) && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    இல்லையெனில் எனில் (*scan == NUL)
	    {
		எனில் (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_வரி_lbr)
		    நிறுத்து;
		reg_nextline();
		scan = rex.உள்ளீடு;
		எனில் (got_int)
		    நிறுத்து;
	    }
	    இல்லையெனில் எனில் (rex.reg_வரி_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    இல்லையெனில்
		நிறுத்து;
	    ++மொத்தம்;
	}
	நிறுத்து;

      நிலை KWORD:
      நிலை KWORD + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      நிலை SKWORD:
      நிலை SKWORD + ADD_NL:
	போது (மொத்தம் < maxcount)
	{
	    எனில் (vim_iswordp_buf(scan, rex.reg_buf)
					  && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    இல்லையெனில் எனில் (*scan == NUL)
	    {
		எனில் (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_வரி_lbr)
		    நிறுத்து;
		reg_nextline();
		scan = rex.உள்ளீடு;
		எனில் (got_int)
		    நிறுத்து;
	    }
	    இல்லையெனில் எனில் (rex.reg_வரி_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    இல்லையெனில்
		நிறுத்து;
	    ++மொத்தம்;
	}
	நிறுத்து;

      நிலை FNAME:
      நிலை FNAME + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      நிலை SFNAME:
      நிலை SFNAME + ADD_NL:
	போது (மொத்தம் < maxcount)
	{
	    எனில் (vim_isfilec(PTR2CHAR(scan)) && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    இல்லையெனில் எனில் (*scan == NUL)
	    {
		எனில் (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_வரி_lbr)
		    நிறுத்து;
		reg_nextline();
		scan = rex.உள்ளீடு;
		எனில் (got_int)
		    நிறுத்து;
	    }
	    இல்லையெனில் எனில் (rex.reg_வரி_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    இல்லையெனில்
		நிறுத்து;
	    ++மொத்தம்;
	}
	நிறுத்து;

      நிலை PRINT:
      நிலை PRINT + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      நிலை SPRINT:
      நிலை SPRINT + ADD_NL:
	போது (மொத்தம் < maxcount)
	{
	    எனில் (*scan == NUL)
	    {
		எனில் (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_வரி_lbr)
		    நிறுத்து;
		reg_nextline();
		scan = rex.உள்ளீடு;
		எனில் (got_int)
		    நிறுத்து;
	    }
	    இல்லையெனில் எனில் (vim_isprintc(PTR2CHAR(scan)) == 1
					  && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    இல்லையெனில் எனில் (rex.reg_வரி_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    இல்லையெனில்
		நிறுத்து;
	    ++மொத்தம்;
	}
	நிறுத்து;

      நிலை WHITE:
      நிலை WHITE + ADD_NL:
	testval = mask = RI_WHITE;
do_class:
	போது (மொத்தம் < maxcount)
	{
	    முழு		l;

	    எனில் (*scan == NUL)
	    {
		எனில் (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_வரி_lbr)
		    நிறுத்து;
		reg_nextline();
		scan = rex.உள்ளீடு;
		எனில் (got_int)
		    நிறுத்து;
	    }
	    இல்லையெனில் எனில் (has_mbyte && (l = (*mb_ptr2len)(scan)) > 1)
	    {
		எனில் (testval != 0)
		    நிறுத்து;
		scan += l;
	    }
	    இல்லையெனில் எனில் ((class_tab[*scan] & mask) == testval)
		++scan;
	    இல்லையெனில் எனில் (rex.reg_வரி_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    இல்லையெனில்
		நிறுத்து;
	    ++மொத்தம்;
	}
	நிறுத்து;

      நிலை NWHITE:
      நிலை NWHITE + ADD_NL:
	mask = RI_WHITE;
	செல் do_class;
      நிலை DIGIT:
      நிலை DIGIT + ADD_NL:
	testval = mask = RI_DIGIT;
	செல் do_class;
      நிலை NDIGIT:
      நிலை NDIGIT + ADD_NL:
	mask = RI_DIGIT;
	செல் do_class;
      நிலை HEX:
      நிலை HEX + ADD_NL:
	testval = mask = RI_HEX;
	செல் do_class;
      நிலை NHEX:
      நிலை NHEX + ADD_NL:
	mask = RI_HEX;
	செல் do_class;
      நிலை OCTAL:
      நிலை OCTAL + ADD_NL:
	testval = mask = RI_OCTAL;
	செல் do_class;
      நிலை NOCTAL:
      நிலை NOCTAL + ADD_NL:
	mask = RI_OCTAL;
	செல் do_class;
      நிலை WORD:
      நிலை WORD + ADD_NL:
	testval = mask = RI_WORD;
	செல் do_class;
      நிலை NWORD:
      நிலை NWORD + ADD_NL:
	mask = RI_WORD;
	செல் do_class;
      நிலை HEAD:
      நிலை HEAD + ADD_NL:
	testval = mask = RI_HEAD;
	செல் do_class;
      நிலை NHEAD:
      நிலை NHEAD + ADD_NL:
	mask = RI_HEAD;
	செல் do_class;
      நிலை ALPHA:
      நிலை ALPHA + ADD_NL:
	testval = mask = RI_ALPHA;
	செல் do_class;
      நிலை NALPHA:
      நிலை NALPHA + ADD_NL:
	mask = RI_ALPHA;
	செல் do_class;
      நிலை LOWER:
      நிலை LOWER + ADD_NL:
	testval = mask = RI_LOWER;
	செல் do_class;
      நிலை NLOWER:
      நிலை NLOWER + ADD_NL:
	mask = RI_LOWER;
	செல் do_class;
      நிலை UPPER:
      நிலை UPPER + ADD_NL:
	testval = mask = RI_UPPER;
	செல் do_class;
      நிலை NUPPER:
      நிலை NUPPER + ADD_NL:
	mask = RI_UPPER;
	செல் do_class;

      நிலை EXACTLY:
	{
	    முழு	    cu, cl;

	    // This doesn't do a multi-byte character, because a MULTIBYTECODE
	    // would have been used for it.  It does handle single-byte
	    // characters, such as latin1.
	    எனில் (rex.reg_ic)
	    {
		cu = MB_TOUPPER(*opnd);
		cl = MB_TOLOWER(*opnd);
		போது (மொத்தம் < maxcount && (*scan == cu || *scan == cl))
		{
		    மொத்தம்++;
		    scan++;
		}
	    }
	    இல்லையெனில்
	    {
		cu = *opnd;
		போது (மொத்தம் < maxcount && *scan == cu)
		{
		    மொத்தம்++;
		    scan++;
		}
	    }
	    நிறுத்து;
	}

      நிலை MULTIBYTECODE:
	{
	    முழு		i, len, cf = 0;

	    // Safety check (just in case 'encoding' was changed since
	    // compiling the program).
	    எனில் ((len = (*mb_ptr2len)(opnd)) > 1)
	    {
		எனில் (rex.reg_ic && enc_utf8)
		    cf = utf_fold(utf_ptr2char(opnd));
		போது (மொத்தம் < maxcount && (*mb_ptr2len)(scan) >= len)
		{
		    க்கு (i = 0; i < len; ++i)
			எனில் (opnd[i] != scan[i])
			    நிறுத்து;
		    எனில் (i < len && (!rex.reg_ic || !enc_utf8
					|| utf_fold(utf_ptr2char(scan)) != cf))
			நிறுத்து;
		    scan += len;
		    ++மொத்தம்;
		}
	    }
	}
	நிறுத்து;

      நிலை ANYOF:
      நிலை ANYOF + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH

      நிலை ANYBUT:
      நிலை ANYBUT + ADD_NL:
	போது (மொத்தம் < maxcount)
	{
	    முழு len;

	    எனில் (*scan == NUL)
	    {
		எனில் (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_வரி_lbr)
		    நிறுத்து;
		reg_nextline();
		scan = rex.உள்ளீடு;
		எனில் (got_int)
		    நிறுத்து;
	    }
	    இல்லையெனில் எனில் (rex.reg_வரி_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    இல்லையெனில் எனில் (has_mbyte && (len = (*mb_ptr2len)(scan)) > 1)
	    {
		எனில் ((cstrchr(opnd, (*mb_ptr2char)(scan)) == வெற்று) == testval)
		    நிறுத்து;
		scan += len;
	    }
	    இல்லையெனில்
	    {
		எனில் ((cstrchr(opnd, *scan) == வெற்று) == testval)
		    நிறுத்து;
		++scan;
	    }
	    ++மொத்தம்;
	}
	நிறுத்து;

      நிலை NEWL:
	போது (மொத்தம் < maxcount
		&& ((*scan == NUL && rex.lnum <= rex.reg_maxline
				       && !rex.reg_வரி_lbr && REG_MULTI)
		    || (*scan == '\n' && rex.reg_வரி_lbr)))
	{
	    மொத்தம்++;
	    எனில் (rex.reg_வரி_lbr)
		ADVANCE_REGINPUT();
	    இல்லையெனில்
		reg_nextline();
	    scan = rex.உள்ளீடு;
	    எனில் (got_int)
		நிறுத்து;
	}
	நிறுத்து;

      இயல்பு:			// Oh dear.  Called inappropriately.
	iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	printf("Called regrepeat with op code %d\n", OP(p));
#endif
	நிறுத்து;
    }

    rex.உள்ளீடு = scan;

    திரும்பு (முழு)மொத்தம்;
}

/*
 * Push an item onto the regstack.
 * Returns pointer to new item.  Returns NULL when out of memory.
 */
    நிலையான regitem_T *
regstack_தள்ளு(regstate_T state, char_u *scan)
{
    regitem_T	*rp;

    எனில் ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
    {
	emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
	திரும்பு வெற்று;
    }
    எனில் (GA_GROW_FAILS(&regstack, (முழு)அளவு(regitem_T)))
	திரும்பு வெற்று;

    rp = (regitem_T *)((எழுத்து *)regstack.ga_data + regstack.ga_len);
    rp->rs_state = state;
    rp->rs_scan = scan;

    regstack.ga_len += அளவு(regitem_T);
    திரும்பு rp;
}

/*
 * Pop an item from the regstack.
 */
    நிலையான வெறுமை
regstack_எடு(char_u **scan)
{
    regitem_T	*rp;

    rp = (regitem_T *)((எழுத்து *)regstack.ga_data + regstack.ga_len) - 1;
    *scan = rp->rs_scan;

    regstack.ga_len -= அளவு(regitem_T);
}

#ifdef FEAT_RELTIME
/*
 * Check if the timer expired, return TRUE if so.
 */
    நிலையான முழு
bt_did_time_out(முழு *timed_out)
{
    எனில் (*timeout_flag)
    {
	எனில் (timed_out != வெற்று)
	{
# ifdef FEAT_EVAL
	    எனில் (!*timed_out)
		ch_log(வெற்று, "BT regexp timed out");
# endif
	    *timed_out = TRUE;
	}
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}
#endif

/*
 * Save the current subexpr to "bp", so that they can be restored
 * later by restore_subexpr().
 */
    நிலையான வெறுமை
save_subexpr(regbehind_T *bp)
{
    முழு i;

    // When "rex.need_clear_subexpr" is set we don't need to save the values,
    // only remember that this flag needs to be set again when restoring.
    bp->save_need_clear_subexpr = rex.need_clear_subexpr;
    எனில் (rex.need_clear_subexpr)
	திரும்பு;

    க்கு (i = 0; i < NSUBEXP; ++i)
    {
	எனில் (REG_MULTI)
	{
	    bp->save_start[i].se_u.pos = rex.reg_startpos[i];
	    bp->save_end[i].se_u.pos = rex.reg_endpos[i];
	}
	இல்லையெனில்
	{
	    bp->save_start[i].se_u.ptr = rex.reg_startp[i];
	    bp->save_end[i].se_u.ptr = rex.reg_endp[i];
	}
    }
}

/*
 * Restore the subexpr from "bp".
 */
    நிலையான வெறுமை
restore_subexpr(regbehind_T *bp)
{
    முழு i;

    // Only need to restore saved values when they are not to be cleared.
    rex.need_clear_subexpr = bp->save_need_clear_subexpr;
    எனில் (rex.need_clear_subexpr)
	திரும்பு;

    க்கு (i = 0; i < NSUBEXP; ++i)
    {
	எனில் (REG_MULTI)
	{
	    rex.reg_startpos[i] = bp->save_start[i].se_u.pos;
	    rex.reg_endpos[i] = bp->save_end[i].se_u.pos;
	}
	இல்லையெனில்
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
    நிலையான முழு
regmatch(
    char_u	*scan,		    // Current node.
    முழு		*timed_out UNUSED)  // flag set on timeout or NULL
{
  char_u	*next;		// Next node.
  முழு		op;
  முழு		c;
  regitem_T	*rp;
  முழு		no;
  முழு		status;		// one of the RA_ values:

  // Make "regstack" and "backpos" empty.  They are allocated and freed in
  // bt_regexec_both() to reduce malloc()/free() calls.
  regstack.ga_len = 0;
  backpos.ga_len = 0;

  // Repeat until "regstack" is empty.
  க்கு (;;)
  {
    // Some patterns may take a long time to match, e.g., "\([a-z]\+\)\+Q".
    // Allow interrupting them with CTRL-C.
    fast_breakcheck();

#ifdef DEBUG
    எனில் (scan != வெற்று && regnarrate)
    {
	mch_errmsg((எழுத்து *)regprop(scan));
	mch_errmsg("(\n");
    }
#endif

    // Repeat for items that can be matched sequentially, without using the
    // regstack.
    க்கு (;;)
    {
	எனில் (got_int || scan == வெற்று)
	{
	    status = RA_FAIL;
	    நிறுத்து;
	}
#ifdef FEAT_RELTIME
	எனில் (bt_did_time_out(timed_out))
	{
	    status = RA_FAIL;
	    நிறுத்து;
	}
#endif
	status = RA_CONT;

#ifdef DEBUG
	எனில் (regnarrate)
	{
	    mch_errmsg((எழுத்து *)regprop(scan));
	    mch_errmsg("...\n");
# ifdef FEAT_SYN_HL
	    எனில் (re_extmatch_in != வெற்று)
	    {
		முழு i;

		mch_errmsg(_("External submatches:\n"));
		க்கு (i = 0; i < NSUBEXP; i++)
		{
		    mch_errmsg("    \"");
		    எனில் (re_extmatch_in->matches[i] != வெற்று)
			mch_errmsg((எழுத்து *)re_extmatch_in->matches[i]);
		    mch_errmsg("\"\n");
		}
	    }
# endif
	}
#endif
	next = regnext(scan);

	op = OP(scan);
	// Check for character class with NL added.
	எனில் (!rex.reg_வரி_lbr && WITH_NL(op) && REG_MULTI
			     && *rex.உள்ளீடு == NUL && rex.lnum <= rex.reg_maxline)
	{
	    reg_nextline();
	}
	இல்லையெனில் எனில் (rex.reg_வரி_lbr && WITH_NL(op) && *rex.உள்ளீடு == '\n')
	{
	    ADVANCE_REGINPUT();
	}
	இல்லையெனில்
	{
	  எனில் (WITH_NL(op))
	      op -= ADD_NL;
	  எனில் (has_mbyte)
	      c = (*mb_ptr2char)(rex.உள்ளீடு);
	  இல்லையெனில்
	      c = *rex.உள்ளீடு;
	  தேர்வு (op)
	  {
	  நிலை BOL:
	    எனில் (rex.உள்ளீடு != rex.வரி)
		status = RA_NOMATCH;
	    நிறுத்து;

	  நிலை EOL:
	    எனில் (c != NUL)
		status = RA_NOMATCH;
	    நிறுத்து;

	  நிலை RE_BOF:
	    // We're not at the beginning of the file when below the first
	    // line where we started, not at the start of the line or we
	    // didn't start at the first line of the buffer.
	    எனில் (rex.lnum != 0 || rex.உள்ளீடு != rex.வரி
				       || (REG_MULTI && rex.reg_firstlnum > 1))
		status = RA_NOMATCH;
	    நிறுத்து;

	  நிலை RE_EOF:
	    எனில் (rex.lnum != rex.reg_maxline || c != NUL)
		status = RA_NOMATCH;
	    நிறுத்து;

	  நிலை CURSOR:
	    // Check if the buffer is in a window and compare the
	    // rex.reg_win->w_cursor position to the match position.
	    எனில் (rex.reg_win == வெற்று
		    || (rex.lnum + rex.reg_firstlnum
						 != rex.reg_win->w_cursor.lnum)
		    || ((colnr_T)(rex.உள்ளீடு - rex.வரி)
						 != rex.reg_win->w_cursor.col))
		status = RA_NOMATCH;
	    நிறுத்து;

	  நிலை RE_MARK:
	    // Compare the mark position to the match position.
	    {
		முழு	mark = OPERAND(scan)[0];
		முழு	cmp = OPERAND(scan)[1];
		pos_T	*pos;
		size_t	col = REG_MULTI ? rex.உள்ளீடு - rex.வரி : 0;

		pos = getmark_buf(rex.reg_buf, mark, FALSE);

		// Line may have been freed, get it again.
		எனில் (REG_MULTI)
		{
		    rex.வரி = reg_getline(rex.lnum);
		    rex.உள்ளீடு = rex.வரி + col;
		}

		எனில் (pos == வெற்று		     // mark doesn't exist
			|| pos->lnum <= 0)   // mark isn't set in reg_buf
		{
		    status = RA_NOMATCH;
		}
		இல்லையெனில்
		{
		    colnr_T pos_col = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->col == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->col;

		    எனில் ((pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_col == (colnr_T)(rex.உள்ளீடு - rex.வரி)
				    ? (cmp == '<' || cmp == '>')
				    : (pos_col < (colnr_T)(rex.உள்ளீடு - rex.வரி)
					? cmp != '>'
					: cmp != '<'))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? cmp != '>'
				    : cmp != '<')))
		    status = RA_NOMATCH;
		}
	    }
	    நிறுத்து;

	  நிலை RE_VISUAL:
	    எனில் (!reg_match_visual())
		status = RA_NOMATCH;
	    நிறுத்து;

	  நிலை RE_LNUM:
	    எனில் (!REG_MULTI || !re_இலக்கம்_cmp((long_u)(rex.lnum + rex.reg_firstlnum),
									scan))
		status = RA_NOMATCH;
	    நிறுத்து;

	  நிலை RE_COL:
	    எனில் (!re_இலக்கம்_cmp((long_u)(rex.உள்ளீடு - rex.வரி) + 1, scan))
		status = RA_NOMATCH;
	    நிறுத்து;

	  நிலை RE_VCOL:
	    {
		win_T	    *wp = rex.reg_win == வெற்று ? curwin : rex.reg_win;
		linenr_T    lnum = REG_MULTI ? rex.reg_firstlnum + rex.lnum : 1;
		long_u	    vcol;

		எனில் (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_buffer->b_ml.ml_வரி_மொத்தம்))
		    lnum = 1;
		vcol = (long_u)win_linetabsize(wp, lnum, rex.வரி,
					      (colnr_T)(rex.உள்ளீடு - rex.வரி));
		எனில் (!re_இலக்கம்_cmp(vcol + 1, scan))
		    status = RA_NOMATCH;
	    }
	    நிறுத்து;

	  நிலை BOW:	// \<word; rex.input points to w
	    எனில் (c == NUL)	// Can't match at end of line
		status = RA_NOMATCH;
	    இல்லையெனில் எனில் (has_mbyte)
	    {
		முழு this_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.உள்ளீடு, rex.reg_buf);
		எனில் (this_class <= 1)
		    status = RA_NOMATCH;  // not on a word at all
		இல்லையெனில் எனில் (reg_prev_class() == this_class)
		    status = RA_NOMATCH;  // previous char is in same word
	    }
	    இல்லையெனில்
	    {
		எனில் (!vim_iswordc_buf(c, rex.reg_buf) || (rex.உள்ளீடு > rex.வரி
				&& vim_iswordc_buf(rex.உள்ளீடு[-1], rex.reg_buf)))
		    status = RA_NOMATCH;
	    }
	    நிறுத்து;

	  நிலை EOW:	// word\>; rex.input points after d
	    எனில் (rex.உள்ளீடு == rex.வரி)    // Can't match at start of line
		status = RA_NOMATCH;
	    இல்லையெனில் எனில் (has_mbyte)
	    {
		முழு this_class, prev_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.உள்ளீடு, rex.reg_buf);
		prev_class = reg_prev_class();
		எனில் (this_class == prev_class
			|| prev_class == 0 || prev_class == 1)
		    status = RA_NOMATCH;
	    }
	    இல்லையெனில்
	    {
		எனில் (!vim_iswordc_buf(rex.உள்ளீடு[-1], rex.reg_buf)
			|| (rex.உள்ளீடு[0] != NUL
					   && vim_iswordc_buf(c, rex.reg_buf)))
		    status = RA_NOMATCH;
	    }
	    நிறுத்து; // Matched with EOW

	  நிலை ANY:
	    // ANY does not match new lines.
	    எனில் (c == NUL)
		status = RA_NOMATCH;
	    இல்லையெனில்
		ADVANCE_REGINPUT();
	    நிறுத்து;

	  நிலை IDENT:
	    எனில் (!vim_isIDc(c))
		status = RA_NOMATCH;
	    இல்லையெனில்
		ADVANCE_REGINPUT();
	    நிறுத்து;

	  நிலை SIDENT:
	    எனில் (VIM_ISDIGIT(*rex.உள்ளீடு) || !vim_isIDc(c))
		status = RA_NOMATCH;
	    இல்லையெனில்
		ADVANCE_REGINPUT();
	    நிறுத்து;

	  நிலை KWORD:
	    எனில் (!vim_iswordp_buf(rex.உள்ளீடு, rex.reg_buf))
		status = RA_NOMATCH;
	    இல்லையெனில்
		ADVANCE_REGINPUT();
	    நிறுத்து;

	  நிலை SKWORD:
	    எனில் (VIM_ISDIGIT(*rex.உள்ளீடு)
				    || !vim_iswordp_buf(rex.உள்ளீடு, rex.reg_buf))
		status = RA_NOMATCH;
	    இல்லையெனில்
		ADVANCE_REGINPUT();
	    நிறுத்து;

	  நிலை FNAME:
	    எனில் (!vim_isfilec(c))
		status = RA_NOMATCH;
	    இல்லையெனில்
		ADVANCE_REGINPUT();
	    நிறுத்து;

	  நிலை SFNAME:
	    எனில் (VIM_ISDIGIT(*rex.உள்ளீடு) || !vim_isfilec(c))
		status = RA_NOMATCH;
	    இல்லையெனில்
		ADVANCE_REGINPUT();
	    நிறுத்து;

	  நிலை PRINT:
	    எனில் (!vim_isprintc(PTR2CHAR(rex.உள்ளீடு)))
		status = RA_NOMATCH;
	    இல்லையெனில்
		ADVANCE_REGINPUT();
	    நிறுத்து;

	  நிலை SPRINT:
	    எனில் (VIM_ISDIGIT(*rex.உள்ளீடு) || !vim_isprintc(PTR2CHAR(rex.உள்ளீடு)))
		status = RA_NOMATCH;
	    இல்லையெனில்
		ADVANCE_REGINPUT();
	    நிறுத்து;

	  நிலை WHITE:
	    எனில் (!VIM_ISWHITE(c))
		status = RA_NOMATCH;
	    இல்லையெனில்
		ADVANCE_REGINPUT();
	    நிறுத்து;

	  நிலை NWHITE:
	    எனில் (c == NUL || VIM_ISWHITE(c))
		status = RA_NOMATCH;
	    இல்லையெனில்
		ADVANCE_REGINPUT();
	    நிறுத்து;

	  நிலை DIGIT:
	    எனில் (!ri_digit(c))
		status = RA_NOMATCH;
	    இல்லையெனில்
		ADVANCE_REGINPUT();
	    நிறுத்து;

	  நிலை NDIGIT:
	    எனில் (c == NUL || ri_digit(c))
		status = RA_NOMATCH;
	    இல்லையெனில்
		ADVANCE_REGINPUT();
	    நிறுத்து;

	  நிலை HEX:
	    எனில் (!ri_hex(c))
		status = RA_NOMATCH;
	    இல்லையெனில்
		ADVANCE_REGINPUT();
	    நிறுத்து;

	  நிலை NHEX:
	    எனில் (c == NUL || ri_hex(c))
		status = RA_NOMATCH;
	    இல்லையெனில்
		ADVANCE_REGINPUT();
	    நிறுத்து;

	  நிலை OCTAL:
	    எனில் (!ri_octal(c))
		status = RA_NOMATCH;
	    இல்லையெனில்
		ADVANCE_REGINPUT();
	    நிறுத்து;

	  நிலை NOCTAL:
	    எனில் (c == NUL || ri_octal(c))
		status = RA_NOMATCH;
	    இல்லையெனில்
		ADVANCE_REGINPUT();
	    நிறுத்து;

	  நிலை WORD:
	    எனில் (!ri_word(c))
		status = RA_NOMATCH;
	    இல்லையெனில்
		ADVANCE_REGINPUT();
	    நிறுத்து;

	  நிலை NWORD:
	    எனில் (c == NUL || ri_word(c))
		status = RA_NOMATCH;
	    இல்லையெனில்
		ADVANCE_REGINPUT();
	    நிறுத்து;

	  நிலை HEAD:
	    எனில் (!ri_head(c))
		status = RA_NOMATCH;
	    இல்லையெனில்
		ADVANCE_REGINPUT();
	    நிறுத்து;

	  நிலை NHEAD:
	    எனில் (c == NUL || ri_head(c))
		status = RA_NOMATCH;
	    இல்லையெனில்
		ADVANCE_REGINPUT();
	    நிறுத்து;

	  நிலை ALPHA:
	    எனில் (!ri_alpha(c))
		status = RA_NOMATCH;
	    இல்லையெனில்
		ADVANCE_REGINPUT();
	    நிறுத்து;

	  நிலை NALPHA:
	    எனில் (c == NUL || ri_alpha(c))
		status = RA_NOMATCH;
	    இல்லையெனில்
		ADVANCE_REGINPUT();
	    நிறுத்து;

	  நிலை LOWER:
	    எனில் (!ri_lower(c))
		status = RA_NOMATCH;
	    இல்லையெனில்
		ADVANCE_REGINPUT();
	    நிறுத்து;

	  நிலை NLOWER:
	    எனில் (c == NUL || ri_lower(c))
		status = RA_NOMATCH;
	    இல்லையெனில்
		ADVANCE_REGINPUT();
	    நிறுத்து;

	  நிலை UPPER:
	    எனில் (!ri_upper(c))
		status = RA_NOMATCH;
	    இல்லையெனில்
		ADVANCE_REGINPUT();
	    நிறுத்து;

	  நிலை NUPPER:
	    எனில் (c == NUL || ri_upper(c))
		status = RA_NOMATCH;
	    இல்லையெனில்
		ADVANCE_REGINPUT();
	    நிறுத்து;

	  நிலை EXACTLY:
	    {
		முழு	len;
		char_u	*opnd;

		opnd = OPERAND(scan);
		// Inline the first byte, for speed.
		எனில் (*opnd != *rex.உள்ளீடு
			&& (!rex.reg_ic
			    || (!enc_utf8
			      && MB_TOLOWER(*opnd) != MB_TOLOWER(*rex.உள்ளீடு))))
		    status = RA_NOMATCH;
		இல்லையெனில் எனில் (*opnd == NUL)
		{
		    // match empty string always works; happens when "~" is
		    // empty.
		}
		இல்லையெனில்
		{
		    எனில் (opnd[1] == NUL && !(enc_utf8 && rex.reg_ic))
		    {
			len = 1;	// matched a single byte above
		    }
		    இல்லையெனில்
		    {
			// Need to match first byte again for multi-byte.
			len = (முழு)STRLEN(opnd);
			எனில் (cstrncmp(opnd, rex.உள்ளீடு, &len) != 0)
			    status = RA_NOMATCH;
		    }
		    // Check for following composing character, unless %C
		    // follows (skips over all composing chars).
		    எனில் (status != RA_NOMATCH
			    && enc_utf8
			    && UTF_COMPOSINGLIKE(rex.உள்ளீடு, rex.உள்ளீடு + len)
			    && !rex.reg_icombine
			    && OP(next) != RE_COMPOSING)
		    {
			// raaron: This code makes a composing character get
			// ignored, which is the correct behavior (sometimes)
			// for voweled Hebrew texts.
			status = RA_NOMATCH;
		    }
		    எனில் (status != RA_NOMATCH)
			rex.உள்ளீடு += len;
		}
	    }
	    நிறுத்து;

	  நிலை ANYOF:
	  நிலை ANYBUT:
	    {
		char_u  *q = OPERAND(scan);

		எனில் (c == NUL)
		    status = RA_NOMATCH;
		இல்லையெனில் எனில் ((cstrchr(q, c) == வெற்று) == (op == ANYOF))
		    status = RA_NOMATCH;
		இல்லையெனில்
		{
		    // Check following combining characters
		    முழு	len = 0;
		    முழு i;

		    எனில் (enc_utf8)
			len = utfc_ptr2len(q) - utf_ptr2len(q);

		    MB_CPTR_ADV(rex.உள்ளீடு);
		    MB_CPTR_ADV(q);

		    எனில் (!enc_utf8 || len == 0)
			நிறுத்து;

		    க்கு (i = 0; i < len; ++i)
			எனில் (q[i] != rex.உள்ளீடு[i])
			{
			    status = RA_NOMATCH;
			    நிறுத்து;
			}
		    rex.உள்ளீடு += len;
		}
		நிறுத்து;
	    }

	  நிலை MULTIBYTECODE:
	    எனில் (has_mbyte)
	    {
		முழு	i, len;
		char_u	*opnd;
		முழு	opndc = 0, inpc;

		opnd = OPERAND(scan);
		// Safety check (just in case 'encoding' was changed since
		// compiling the program).
		எனில் ((len = (*mb_ptr2len)(opnd)) < 2)
		{
		    status = RA_NOMATCH;
		    நிறுத்து;
		}
		எனில் (enc_utf8)
		    opndc = utf_ptr2char(opnd);
		எனில் (enc_utf8 && utf_iscomposing(opndc))
		{
		    // When only a composing char is given match at any
		    // position where that composing char appears.
		    status = RA_NOMATCH;
		    க்கு (i = 0; rex.உள்ளீடு[i] != NUL;
						i += utf_ptr2len(rex.உள்ளீடு + i))
		    {
			inpc = utf_ptr2char(rex.உள்ளீடு + i);
			எனில் (!utf_iscomposing(inpc))
			{
			    எனில் (i > 0)
				நிறுத்து;
			}
			இல்லையெனில் எனில் (opndc == inpc)
			{
			    // Include all following composing chars.
			    len = i + utfc_ptr2len(rex.உள்ளீடு + i);
			    status = RA_MATCH;
			    நிறுத்து;
			}
		    }
		}
		இல்லையெனில் எனில் (enc_utf8)
		{
		    எனில் (cstrncmp(opnd, rex.உள்ளீடு, &len) != 0)
		    {
			status = RA_NOMATCH;
			நிறுத்து;
		    }
		}
		இல்லையெனில்
		    க்கு (i = 0; i < len; ++i)
			எனில் (opnd[i] != rex.உள்ளீடு[i])
			{
			    status = RA_NOMATCH;
			    நிறுத்து;
			}
		rex.உள்ளீடு += len;
	    }
	    இல்லையெனில்
		status = RA_NOMATCH;
	    நிறுத்து;
	  நிலை RE_COMPOSING:
	    எனில் (enc_utf8)
	    {
		// Skip composing characters.
		போது (utf_iscomposing(utf_ptr2char(rex.உள்ளீடு)))
		    MB_CPTR_ADV(rex.உள்ளீடு);
	    }
	    நிறுத்து;

	  நிலை NOTHING:
	    நிறுத்து;

	  நிலை BACK:
	    {
		முழு		i;
		backpos_T	*bp;

		// When we run into BACK we need to check if we don't keep
		// looping without matching any input.  The second and later
		// times a BACK is encountered it fails if the input is still
		// at the same position as the previous time.
		// The positions are stored in "backpos" and found by the
		// current value of "scan", the position in the RE program.
		bp = (backpos_T *)backpos.ga_data;
		க்கு (i = 0; i < backpos.ga_len; ++i)
		    எனில் (bp[i].bp_scan == scan)
			நிறுத்து;
		எனில் (i == backpos.ga_len)
		{
		    // First time at this BACK, make room to store the pos.
		    எனில் (GA_GROW_FAILS(&backpos, 1))
			status = RA_FAIL;
		    இல்லையெனில்
		    {
			// get "ga_data" again, it may have changed
			bp = (backpos_T *)backpos.ga_data;
			bp[i].bp_scan = scan;
			++backpos.ga_len;
		    }
		}
		இல்லையெனில் எனில் (reg_save_equal(&bp[i].bp_pos))
		    // Still at same position as last time, fail.
		    status = RA_NOMATCH;

		எனில் (status != RA_FAIL && status != RA_NOMATCH)
		    reg_save(&bp[i].bp_pos, &backpos);
	    }
	    நிறுத்து;

	  நிலை MOPEN + 0:   // Match start: \zs
	  நிலை MOPEN + 1:   // \(
	  நிலை MOPEN + 2:
	  நிலை MOPEN + 3:
	  நிலை MOPEN + 4:
	  நிலை MOPEN + 5:
	  நிலை MOPEN + 6:
	  நிலை MOPEN + 7:
	  நிலை MOPEN + 8:
	  நிலை MOPEN + 9:
	    {
		no = op - MOPEN;
		cleanup_subexpr();
		rp = regstack_தள்ளு(RS_MOPEN, scan);
		எனில் (rp == வெற்று)
		    status = RA_FAIL;
		இல்லையெனில்
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_startpos[no],
							  &rex.reg_startp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    நிறுத்து;

	  நிலை NOPEN:	    // \%(
	  நிலை NCLOSE:	    // \) after \%(
		எனில் (regstack_தள்ளு(RS_NOPEN, scan) == வெற்று)
		    status = RA_FAIL;
		// We simply continue and handle the result when done.
		நிறுத்து;

#ifdef FEAT_SYN_HL
	  நிலை ZOPEN + 1:
	  நிலை ZOPEN + 2:
	  நிலை ZOPEN + 3:
	  நிலை ZOPEN + 4:
	  நிலை ZOPEN + 5:
	  நிலை ZOPEN + 6:
	  நிலை ZOPEN + 7:
	  நிலை ZOPEN + 8:
	  நிலை ZOPEN + 9:
	    {
		no = op - ZOPEN;
		cleanup_zsubexpr();
		rp = regstack_தள்ளு(RS_ZOPEN, scan);
		எனில் (rp == வெற்று)
		    status = RA_FAIL;
		இல்லையெனில்
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_startzpos[no],
							     &reg_startzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    நிறுத்து;
#endif

	  நிலை MCLOSE + 0:  // Match end: \ze
	  நிலை MCLOSE + 1:  // \)
	  நிலை MCLOSE + 2:
	  நிலை MCLOSE + 3:
	  நிலை MCLOSE + 4:
	  நிலை MCLOSE + 5:
	  நிலை MCLOSE + 6:
	  நிலை MCLOSE + 7:
	  நிலை MCLOSE + 8:
	  நிலை MCLOSE + 9:
	    {
		no = op - MCLOSE;
		cleanup_subexpr();
		rp = regstack_தள்ளு(RS_MCLOSE, scan);
		எனில் (rp == வெற்று)
		    status = RA_FAIL;
		இல்லையெனில்
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_endpos[no],
							    &rex.reg_endp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    நிறுத்து;

#ifdef FEAT_SYN_HL
	  நிலை ZCLOSE + 1:  // \) after \z(
	  நிலை ZCLOSE + 2:
	  நிலை ZCLOSE + 3:
	  நிலை ZCLOSE + 4:
	  நிலை ZCLOSE + 5:
	  நிலை ZCLOSE + 6:
	  நிலை ZCLOSE + 7:
	  நிலை ZCLOSE + 8:
	  நிலை ZCLOSE + 9:
	    {
		no = op - ZCLOSE;
		cleanup_zsubexpr();
		rp = regstack_தள்ளு(RS_ZCLOSE, scan);
		எனில் (rp == வெற்று)
		    status = RA_FAIL;
		இல்லையெனில்
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_endzpos[no],
							      &reg_endzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    நிறுத்து;
#endif

	  நிலை BACKREF + 1:
	  நிலை BACKREF + 2:
	  நிலை BACKREF + 3:
	  நிலை BACKREF + 4:
	  நிலை BACKREF + 5:
	  நிலை BACKREF + 6:
	  நிலை BACKREF + 7:
	  நிலை BACKREF + 8:
	  நிலை BACKREF + 9:
	    {
		முழு		len;

		no = op - BACKREF;
		cleanup_subexpr();
		எனில் (!REG_MULTI)		// Single-line regexp
		{
		    எனில் (rex.reg_startp[no] == வெற்று || rex.reg_endp[no] == வெற்று)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    இல்லையெனில்
		    {
			// Compare current input with back-ref in the same
			// line.
			len = (முழு)(rex.reg_endp[no] - rex.reg_startp[no]);
			எனில் (cstrncmp(rex.reg_startp[no], rex.உள்ளீடு, &len) != 0)
			    status = RA_NOMATCH;
		    }
		}
		இல்லையெனில்				// Multi-line regexp
		{
		    எனில் (rex.reg_startpos[no].lnum < 0
						|| rex.reg_endpos[no].lnum < 0)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    இல்லையெனில்
		    {
			எனில் (rex.reg_startpos[no].lnum == rex.lnum
				&& rex.reg_endpos[no].lnum == rex.lnum)
			{
			    // Compare back-ref within the current line.
			    len = rex.reg_endpos[no].col
						    - rex.reg_startpos[no].col;
			    எனில் (cstrncmp(rex.வரி + rex.reg_startpos[no].col,
							  rex.உள்ளீடு, &len) != 0)
				status = RA_NOMATCH;
			}
			இல்லையெனில்
			{
			    // Messy situation: Need to compare between two
			    // lines.
			    முழு r = match_with_backref(
					    rex.reg_startpos[no].lnum,
					    rex.reg_startpos[no].col,
					    rex.reg_endpos[no].lnum,
					    rex.reg_endpos[no].col,
					    &len);

			    எனில் (r != RA_MATCH)
				status = r;
			}
		    }
		}

		// Matched the backref, skip over it.
		rex.உள்ளீடு += len;
	    }
	    நிறுத்து;

#ifdef FEAT_SYN_HL
	  நிலை ZREF + 1:
	  நிலை ZREF + 2:
	  நிலை ZREF + 3:
	  நிலை ZREF + 4:
	  நிலை ZREF + 5:
	  நிலை ZREF + 6:
	  நிலை ZREF + 7:
	  நிலை ZREF + 8:
	  நிலை ZREF + 9:
	    {
		முழு	len;

		cleanup_zsubexpr();
		no = op - ZREF;
		எனில் (re_extmatch_in != வெற்று
			&& re_extmatch_in->matches[no] != வெற்று)
		{
		    len = (முழு)STRLEN(re_extmatch_in->matches[no]);
		    எனில் (cstrncmp(re_extmatch_in->matches[no],
							  rex.உள்ளீடு, &len) != 0)
			status = RA_NOMATCH;
		    இல்லையெனில்
			rex.உள்ளீடு += len;
		}
		இல்லையெனில்
		{
		    // Backref was not set: Match an empty string.
		}
	    }
	    நிறுத்து;
#endif

	  நிலை BRANCH:
	    {
		எனில் (OP(next) != BRANCH) // No choice.
		    next = OPERAND(scan);	// Avoid recursion.
		இல்லையெனில்
		{
		    rp = regstack_தள்ளு(RS_BRANCH, scan);
		    எனில் (rp == வெற்று)
			status = RA_FAIL;
		    இல்லையெனில்
			status = RA_BREAK;	// rest is below
		}
	    }
	    நிறுத்து;

	  நிலை BRACE_LIMITS:
	    {
		எனில் (OP(next) == BRACE_SIMPLE)
		{
		    bl_minval = OPERAND_MIN(scan);
		    bl_maxval = OPERAND_MAX(scan);
		}
		இல்லையெனில் எனில் (OP(next) >= BRACE_COMPLEX
			&& OP(next) < BRACE_COMPLEX + 10)
		{
		    no = OP(next) - BRACE_COMPLEX;
		    brace_min[no] = OPERAND_MIN(scan);
		    brace_max[no] = OPERAND_MAX(scan);
		    brace_மொத்தம்[no] = 0;
		}
		இல்லையெனில்
		{
		    internal_error("BRACE_LIMITS");
		    status = RA_FAIL;
		}
	    }
	    நிறுத்து;

	  நிலை BRACE_COMPLEX + 0:
	  நிலை BRACE_COMPLEX + 1:
	  நிலை BRACE_COMPLEX + 2:
	  நிலை BRACE_COMPLEX + 3:
	  நிலை BRACE_COMPLEX + 4:
	  நிலை BRACE_COMPLEX + 5:
	  நிலை BRACE_COMPLEX + 6:
	  நிலை BRACE_COMPLEX + 7:
	  நிலை BRACE_COMPLEX + 8:
	  நிலை BRACE_COMPLEX + 9:
	    {
		no = op - BRACE_COMPLEX;
		++brace_மொத்தம்[no];

		// If not matched enough times yet, try one more
		எனில் (brace_மொத்தம்[no] <= (brace_min[no] <= brace_max[no]
					     ? brace_min[no] : brace_max[no]))
		{
		    rp = regstack_தள்ளு(RS_BRCPLX_MORE, scan);
		    எனில் (rp == வெற்று)
			status = RA_FAIL;
		    இல்லையெனில்
		    {
			rp->rs_no = no;
			reg_save(&rp->rs_un.regsave, &backpos);
			next = OPERAND(scan);
			// We continue and handle the result when done.
		    }
		    நிறுத்து;
		}

		// If matched enough times, may try matching some more
		எனில் (brace_min[no] <= brace_max[no])
		{
		    // Range is the normal way around, use longest match
		    எனில் (brace_மொத்தம்[no] <= brace_max[no])
		    {
			rp = regstack_தள்ளு(RS_BRCPLX_LONG, scan);
			எனில் (rp == வெற்று)
			    status = RA_FAIL;
			இல்லையெனில்
			{
			    rp->rs_no = no;
			    reg_save(&rp->rs_un.regsave, &backpos);
			    next = OPERAND(scan);
			    // We continue and handle the result when done.
			}
		    }
		}
		இல்லையெனில்
		{
		    // Range is backwards, use shortest match first
		    எனில் (brace_மொத்தம்[no] <= brace_min[no])
		    {
			rp = regstack_தள்ளு(RS_BRCPLX_SHORT, scan);
			எனில் (rp == வெற்று)
			    status = RA_FAIL;
			இல்லையெனில்
			{
			    reg_save(&rp->rs_un.regsave, &backpos);
			    // We continue and handle the result when done.
			}
		    }
		}
	    }
	    நிறுத்து;

	  நிலை BRACE_SIMPLE:
	  நிலை STAR:
	  நிலை PLUS:
	    {
		regstar_T	rst;

		// Lookahead to avoid useless match attempts when we know
		// what character comes next.
		எனில் (OP(next) == EXACTLY)
		{
		    rst.nextb = *OPERAND(next);
		    எனில் (rex.reg_ic)
		    {
			எனில் (MB_ISUPPER(rst.nextb))
			    rst.nextb_ic = MB_TOLOWER(rst.nextb);
			இல்லையெனில்
			    rst.nextb_ic = MB_TOUPPER(rst.nextb);
		    }
		    இல்லையெனில்
			rst.nextb_ic = rst.nextb;
		}
		இல்லையெனில்
		{
		    rst.nextb = NUL;
		    rst.nextb_ic = NUL;
		}
		எனில் (op != BRACE_SIMPLE)
		{
		    rst.minval = (op == STAR) ? 0 : 1;
		    rst.maxval = MAX_LIMIT;
		}
		இல்லையெனில்
		{
		    rst.minval = bl_minval;
		    rst.maxval = bl_maxval;
		}

		// When maxval > minval, try matching as much as possible, up
		// to maxval.  When maxval < minval, try matching at least the
		// minimal number (since the range is backwards, that's also
		// maxval!).
		rst.மொத்தம் = regrepeat(OPERAND(scan), rst.maxval);
		எனில் (got_int)
		{
		    status = RA_FAIL;
		    நிறுத்து;
		}
		எனில் (rst.minval <= rst.maxval
			  ? rst.மொத்தம் >= rst.minval : rst.மொத்தம் >= rst.maxval)
		{
		    // It could match.  Prepare for trying to match what
		    // follows.  The code is below.  Parameters are stored in
		    // a regstar_T on the regstack.
		    எனில் ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
		    {
			emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
			status = RA_FAIL;
		    }
		    இல்லையெனில் எனில் (GA_GROW_FAILS(&regstack, (முழு)அளவு(regstar_T)))
			status = RA_FAIL;
		    இல்லையெனில்
		    {
			regstack.ga_len += அளவு(regstar_T);
			rp = regstack_தள்ளு(rst.minval <= rst.maxval
					? RS_STAR_LONG : RS_STAR_SHORT, scan);
			எனில் (rp == வெற்று)
			    status = RA_FAIL;
			இல்லையெனில்
			{
			    *(((regstar_T *)rp) - 1) = rst;
			    status = RA_BREAK;	    // skip the restore bits
			}
		    }
		}
		இல்லையெனில்
		    status = RA_NOMATCH;

	    }
	    நிறுத்து;

	  நிலை NOMATCH:
	  நிலை MATCH:
	  நிலை SUBPAT:
	    rp = regstack_தள்ளு(RS_NOMATCH, scan);
	    எனில் (rp == வெற்று)
		status = RA_FAIL;
	    இல்லையெனில்
	    {
		rp->rs_no = op;
		reg_save(&rp->rs_un.regsave, &backpos);
		next = OPERAND(scan);
		// We continue and handle the result when done.
	    }
	    நிறுத்து;

	  நிலை BEHIND:
	  நிலை NOBEHIND:
	    // Need a bit of room to store extra positions.
	    எனில் ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
	    {
		emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		status = RA_FAIL;
	    }
	    இல்லையெனில் எனில் (GA_GROW_FAILS(&regstack, (முழு)அளவு(regbehind_T)))
		status = RA_FAIL;
	    இல்லையெனில்
	    {
		regstack.ga_len += அளவு(regbehind_T);
		rp = regstack_தள்ளு(RS_BEHIND1, scan);
		எனில் (rp == வெற்று)
		    status = RA_FAIL;
		இல்லையெனில்
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
	    நிறுத்து;

	  நிலை BHPOS:
	    எனில் (REG_MULTI)
	    {
		எனில் (behind_pos.rs_u.pos.col != (colnr_T)(rex.உள்ளீடு - rex.வரி)
			|| behind_pos.rs_u.pos.lnum != rex.lnum)
		    status = RA_NOMATCH;
	    }
	    இல்லையெனில் எனில் (behind_pos.rs_u.ptr != rex.உள்ளீடு)
		status = RA_NOMATCH;
	    நிறுத்து;

	  நிலை NEWL:
	    எனில் ((c != NUL || !REG_MULTI || rex.lnum > rex.reg_maxline
			     || rex.reg_வரி_lbr)
					   && (c != '\n' || !rex.reg_வரி_lbr))
		status = RA_NOMATCH;
	    இல்லையெனில் எனில் (rex.reg_வரி_lbr)
		ADVANCE_REGINPUT();
	    இல்லையெனில்
		reg_nextline();
	    நிறுத்து;

	  நிலை END:
	    status = RA_MATCH;	// Success!
	    நிறுத்து;

	  இயல்பு:
	    iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	    printf("Illegal op code %d\n", op);
#endif
	    status = RA_FAIL;
	    நிறுத்து;
	  }
	}

	// If we can't continue sequentially, break the inner loop.
	எனில் (status != RA_CONT)
	    நிறுத்து;

	// Continue in inner loop, advance to next item.
	scan = next;

    } // end of inner loop

    // If there is something on the regstack execute the code for the state.
    // If the state is popped then loop and use the older state.
    போது (regstack.ga_len > 0 && status != RA_FAIL)
    {
	rp = (regitem_T *)((எழுத்து *)regstack.ga_data + regstack.ga_len) - 1;
	தேர்வு (rp->rs_state)
	{
	  நிலை RS_NOPEN:
	    // Result is passed on as-is, simply pop the state.
	    regstack_எடு(&scan);
	    நிறுத்து;

	  நிலை RS_MOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    எனில் (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_startpos[rp->rs_no],
						  &rex.reg_startp[rp->rs_no]);
	    regstack_எடு(&scan);
	    நிறுத்து;

#ifdef FEAT_SYN_HL
	  நிலை RS_ZOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    எனில் (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_startzpos[rp->rs_no],
						 &reg_startzp[rp->rs_no]);
	    regstack_எடு(&scan);
	    நிறுத்து;
#endif

	  நிலை RS_MCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    எனில் (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_endpos[rp->rs_no],
						    &rex.reg_endp[rp->rs_no]);
	    regstack_எடு(&scan);
	    நிறுத்து;

#ifdef FEAT_SYN_HL
	  நிலை RS_ZCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    எனில் (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_endzpos[rp->rs_no],
						   &reg_endzp[rp->rs_no]);
	    regstack_எடு(&scan);
	    நிறுத்து;
#endif

	  நிலை RS_BRANCH:
	    எனில் (status == RA_MATCH)
		// this branch matched, use it
		regstack_எடு(&scan);
	    இல்லையெனில்
	    {
		எனில் (status != RA_BREAK)
		{
		    // After a non-matching branch: try next one.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    scan = rp->rs_scan;
		}
		எனில் (scan == வெற்று || OP(scan) != BRANCH)
		{
		    // no more branches, didn't find a match
		    status = RA_NOMATCH;
		    regstack_எடு(&scan);
		}
		இல்லையெனில்
		{
		    // Prepare to try a branch.
		    rp->rs_scan = regnext(scan);
		    reg_save(&rp->rs_un.regsave, &backpos);
		    scan = OPERAND(scan);
		}
	    }
	    நிறுத்து;

	  நிலை RS_BRCPLX_MORE:
	    // Pop the state.  Restore pointers when there is no match.
	    எனில் (status == RA_NOMATCH)
	    {
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_மொத்தம்[rp->rs_no];	// decrement match count
	    }
	    regstack_எடு(&scan);
	    நிறுத்து;

	  நிலை RS_BRCPLX_LONG:
	    // Pop the state.  Restore pointers when there is no match.
	    எனில் (status == RA_NOMATCH)
	    {
		// There was no match, but we did find enough matches.
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_மொத்தம்[rp->rs_no];
		// continue with the items after "\{}"
		status = RA_CONT;
	    }
	    regstack_எடு(&scan);
	    எனில் (status == RA_CONT)
		scan = regnext(scan);
	    நிறுத்து;

	  நிலை RS_BRCPLX_SHORT:
	    // Pop the state.  Restore pointers when there is no match.
	    எனில் (status == RA_NOMATCH)
		// There was no match, try to match one more item.
		reg_restore(&rp->rs_un.regsave, &backpos);
	    regstack_எடு(&scan);
	    எனில் (status == RA_NOMATCH)
	    {
		scan = OPERAND(scan);
		status = RA_CONT;
	    }
	    நிறுத்து;

	  நிலை RS_NOMATCH:
	    // Pop the state.  If the operand matches for NOMATCH or
	    // doesn't match for MATCH/SUBPAT, we fail.  Otherwise backup,
	    // except for SUBPAT, and continue with the next item.
	    எனில் (status == (rp->rs_no == NOMATCH ? RA_MATCH : RA_NOMATCH))
		status = RA_NOMATCH;
	    இல்லையெனில்
	    {
		status = RA_CONT;
		எனில் (rp->rs_no != SUBPAT)	// zero-width
		    reg_restore(&rp->rs_un.regsave, &backpos);
	    }
	    regstack_எடு(&scan);
	    எனில் (status == RA_CONT)
		scan = regnext(scan);
	    நிறுத்து;

	  நிலை RS_BEHIND1:
	    எனில் (status == RA_NOMATCH)
	    {
		regstack_எடு(&scan);
		regstack.ga_len -= அளவு(regbehind_T);
	    }
	    இல்லையெனில்
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
	    நிறுத்து;

	  நிலை RS_BEHIND2:
	    // Looping for BEHIND / NOBEHIND match.
	    எனில் (status == RA_MATCH && reg_save_equal(&behind_pos))
	    {
		// found a match that ends where "next" started
		behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		எனில் (rp->rs_no == BEHIND)
		    reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
		இல்லையெனில்
		{
		    // But we didn't want a match.  Need to restore the
		    // subexpr, because what follows matched, so they have
		    // been set.
		    status = RA_NOMATCH;
		    restore_subexpr(((regbehind_T *)rp) - 1);
		}
		regstack_எடு(&scan);
		regstack.ga_len -= அளவு(regbehind_T);
	    }
	    இல்லையெனில்
	    {
		long limit;

		// No match or a match that doesn't end where we want it: Go
		// back one character.  May go to previous line once.
		no = OK;
		limit = OPERAND_MIN(rp->rs_scan);
		எனில் (REG_MULTI)
		{
		    எனில் (limit > 0
			    && ((rp->rs_un.regsave.rs_u.pos.lnum
						    < behind_pos.rs_u.pos.lnum
				    ? (colnr_T)STRLEN(rex.வரி)
				    : behind_pos.rs_u.pos.col)
				- rp->rs_un.regsave.rs_u.pos.col >= limit))
			no = FAIL;
		    இல்லையெனில் எனில் (rp->rs_un.regsave.rs_u.pos.col == 0)
		    {
			எனில் (rp->rs_un.regsave.rs_u.pos.lnum
					< behind_pos.rs_u.pos.lnum
				|| reg_getline(
					--rp->rs_un.regsave.rs_u.pos.lnum)
								  == வெற்று)
			    no = FAIL;
			இல்லையெனில்
			{
			    reg_restore(&rp->rs_un.regsave, &backpos);
			    rp->rs_un.regsave.rs_u.pos.col =
						 (colnr_T)STRLEN(rex.வரி);
			}
		    }
		    இல்லையெனில்
		    {
			எனில் (has_mbyte)
			{
			    char_u *வரி =
				  reg_getline(rp->rs_un.regsave.rs_u.pos.lnum);

			    rp->rs_un.regsave.rs_u.pos.col -=
				(*mb_head_off)(வரி, வரி
				    + rp->rs_un.regsave.rs_u.pos.col - 1) + 1;
			}
			இல்லையெனில்
			    --rp->rs_un.regsave.rs_u.pos.col;
		    }
		}
		இல்லையெனில்
		{
		    எனில் (rp->rs_un.regsave.rs_u.ptr == rex.வரி)
			no = FAIL;
		    இல்லையெனில்
		    {
			MB_PTR_BACK(rex.வரி, rp->rs_un.regsave.rs_u.ptr);
			எனில் (limit > 0 && (long)(behind_pos.rs_u.ptr
				     - rp->rs_un.regsave.rs_u.ptr) > limit)
			    no = FAIL;
		    }
		}
		எனில் (no == OK)
		{
		    // Advanced, prepare for finding match again.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    scan = OPERAND(rp->rs_scan) + 4;
		    எனில் (status == RA_MATCH)
		    {
			// We did match, so subexpr may have been changed,
			// need to restore them for the next try.
			status = RA_NOMATCH;
			restore_subexpr(((regbehind_T *)rp) - 1);
		    }
		}
		இல்லையெனில்
		{
		    // Can't advance.  For NOBEHIND that's a match.
		    behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		    எனில் (rp->rs_no == NOBEHIND)
		    {
			reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
			status = RA_MATCH;
		    }
		    இல்லையெனில்
		    {
			// We do want a proper match.  Need to restore the
			// subexpr if we had a match, because they may have
			// been set.
			எனில் (status == RA_MATCH)
			{
			    status = RA_NOMATCH;
			    restore_subexpr(((regbehind_T *)rp) - 1);
			}
		    }
		    regstack_எடு(&scan);
		    regstack.ga_len -= அளவு(regbehind_T);
		}
	    }
	    நிறுத்து;

	  நிலை RS_STAR_LONG:
	  நிலை RS_STAR_SHORT:
	    {
		regstar_T	    *rst = ((regstar_T *)rp) - 1;

		எனில் (status == RA_MATCH)
		{
		    regstack_எடு(&scan);
		    regstack.ga_len -= அளவு(regstar_T);
		    நிறுத்து;
		}

		// Tried once already, restore input pointers.
		எனில் (status != RA_BREAK)
		    reg_restore(&rp->rs_un.regsave, &backpos);

		// Repeat until we found a position where it could match.
		க்கு (;;)
		{
		    எனில் (status != RA_BREAK)
		    {
			// Tried first position already, advance.
			எனில் (rp->rs_state == RS_STAR_LONG)
			{
			    // Trying for longest match, but couldn't or
			    // didn't match -- back up one char.
			    எனில் (--rst->மொத்தம் < rst->minval)
				நிறுத்து;
			    எனில் (rex.உள்ளீடு == rex.வரி)
			    {
				// backup to last char of previous line
				எனில் (rex.lnum == 0)
				{
				    status = RA_NOMATCH;
				    நிறுத்து;
				}
				--rex.lnum;
				rex.வரி = reg_getline(rex.lnum);
				// Just in case regrepeat() didn't count
				// right.
				எனில் (rex.வரி == வெற்று)
				    நிறுத்து;
				rex.உள்ளீடு = rex.வரி + reg_getline_len(rex.lnum);
				fast_breakcheck();
			    }
			    இல்லையெனில்
				MB_PTR_BACK(rex.வரி, rex.உள்ளீடு);
			}
			இல்லையெனில்
			{
			    // Range is backwards, use shortest match first.
			    // Careful: maxval and minval are exchanged!
			    // Couldn't or didn't match: try advancing one
			    // char.
			    எனில் (rst->மொத்தம் == rst->minval
				  || regrepeat(OPERAND(rp->rs_scan), 1L) == 0)
				நிறுத்து;
			    ++rst->மொத்தம்;
			}
			எனில் (got_int)
			    நிறுத்து;
		    }
		    இல்லையெனில்
			status = RA_NOMATCH;

		    // If it could match, try it.
		    எனில் (rst->nextb == NUL || *rex.உள்ளீடு == rst->nextb
					     || *rex.உள்ளீடு == rst->nextb_ic)
		    {
			reg_save(&rp->rs_un.regsave, &backpos);
			scan = regnext(rp->rs_scan);
			status = RA_CONT;
			நிறுத்து;
		    }
		}
		எனில் (status != RA_CONT)
		{
		    // Failed.
		    regstack_எடு(&scan);
		    regstack.ga_len -= அளவு(regstar_T);
		    status = RA_NOMATCH;
		}
	    }
	    நிறுத்து;
	}

	// If we want to continue the inner loop or didn't pop a state
	// continue matching loop
	எனில் (status == RA_CONT || rp == (regitem_T *)
			     ((எழுத்து *)regstack.ga_data + regstack.ga_len) - 1)
	    நிறுத்து;

#ifdef FEAT_RELTIME
	எனில் (bt_did_time_out(timed_out))
	{
	    status = RA_FAIL;
	    நிறுத்து;
	}
#endif
    }

    // May need to continue with the inner loop, starting at "scan".
    எனில் (status == RA_CONT)
	தொடர்;

    // If the regstack is empty or something failed we are done.
    எனில் (regstack.ga_len == 0 || status == RA_FAIL)
    {
	எனில் (scan == வெற்று)
	{
	    // We get here only if there's trouble -- normally "case END" is
	    // the terminating point.
	    iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	    printf("Premature EOL\n");
#endif
	}
	திரும்பு (status == RA_MATCH);
    }

  } // End of loop until the regstack is empty.

  // NOTREACHED
}

/*
 * regtry - try match of "prog" with at rex.line["col"].
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    நிலையான long
regtry(
    bt_regprog_T	*prog,
    colnr_T		col,
    முழு			*timed_out)	// flag set on timeout or NULL
{
    rex.உள்ளீடு = rex.வரி + col;
    rex.need_clear_subexpr = TRUE;
#ifdef FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    rex.need_clear_zsubexpr = (prog->reghasz == REX_SET);
#endif

    எனில் (regmatch(prog->program + 1, timed_out) == 0)
	திரும்பு 0;

    cleanup_subexpr();
    எனில் (REG_MULTI)
    {
	எனில் (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].col = col;
	}
	எனில் (rex.reg_endpos[0].lnum < 0)
	{
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].col = (முழு)(rex.உள்ளீடு - rex.வரி);
	}
	இல்லையெனில்
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    இல்லையெனில்
    {
	எனில் (rex.reg_startp[0] == வெற்று)
	    rex.reg_startp[0] = rex.வரி + col;
	எனில் (rex.reg_endp[0] == வெற்று)
	    rex.reg_endp[0] = rex.உள்ளீடு;
    }
#ifdef FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = வெற்று;

    எனில் (prog->reghasz == REX_SET)
    {
	முழு		i;

	cleanup_zsubexpr();
	re_extmatch_out = make_extmatch();
	எனில் (re_extmatch_out == வெற்று)
	    திரும்பு 0;
	க்கு (i = 0; i < NSUBEXP; i++)
	{
	    எனில் (REG_MULTI)
	    {
		// Only accept single line matches.
		எனில் (reg_startzpos[i].lnum >= 0
			&& reg_endzpos[i].lnum == reg_startzpos[i].lnum
			&& reg_endzpos[i].col >= reg_startzpos[i].col)
		    re_extmatch_out->matches[i] =
			vim_strnsave(reg_getline(reg_startzpos[i].lnum)
						       + reg_startzpos[i].col,
				   reg_endzpos[i].col - reg_startzpos[i].col);
	    }
	    இல்லையெனில்
	    {
		எனில் (reg_startzp[i] != வெற்று && reg_endzp[i] != வெற்று)
		    re_extmatch_out->matches[i] =
			    vim_strnsave(reg_startzp[i],
						reg_endzp[i] - reg_startzp[i]);
	    }
	}
    }
#endif
    திரும்பு 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    நிலையான long
bt_regexec_both(
    char_u	*வரி,
    colnr_T	startcol,	// column to start looking for match
    முழு		*timed_out)	// flag set on timeout or NULL
{
    bt_regprog_T    *prog;
    char_u	    *s;
    colnr_T	    col = startcol;
    long	    retval = 0L;

    // Create "regstack" and "backpos" if they are not allocated yet.
    // We allocate *_INITIAL amount of bytes first and then set the grow size
    // to much bigger value to avoid many malloc calls in case of deep regular
    // expressions.
    எனில் (regstack.ga_data == வெற்று)
    {
	// Use an item size of 1 byte, since we push different things
	// onto the regstack.
	ga_init2(&regstack, 1, REGSTACK_INITIAL);
	(வெறுமை)ga_grow(&regstack, REGSTACK_INITIAL);
	regstack.ga_growsize = REGSTACK_INITIAL * 8;
    }

    எனில் (backpos.ga_data == வெற்று)
    {
	ga_init2(&backpos, அளவு(backpos_T), BACKPOS_INITIAL);
	(வெறுமை)ga_grow(&backpos, BACKPOS_INITIAL);
	backpos.ga_growsize = BACKPOS_INITIAL * 8;
    }

    எனில் (REG_MULTI)
    {
	prog = (bt_regprog_T *)rex.reg_mmatch->regprog;
	வரி = reg_getline((linenr_T)0);
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    இல்லையெனில்
    {
	prog = (bt_regprog_T *)rex.reg_match->regprog;
	rex.reg_startp = rex.reg_match->startp;
	rex.reg_endp = rex.reg_match->endp;
    }

    // Be paranoid...
    எனில் (prog == வெற்று || வரி == வெற்று)
    {
	iemsg(e_null_argument);
	செல் theend;
    }

    // Check validity of program.
    எனில் (prog_magic_wrong())
	செல் theend;

    // If the start column is past the maximum column: no need to try.
    எனில் (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	செல் theend;

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    எனில் (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    இல்லையெனில் எனில் (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    எனில் (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    // If there is a "must appear" string, look for it.
    எனில் (prog->regmust != வெற்று)
    {
	முழு c;

	எனில் (has_mbyte)
	    c = (*mb_ptr2char)(prog->regmust);
	இல்லையெனில்
	    c = *prog->regmust;
	s = வரி + col;

	// This is used very often, esp. for ":global".  Use three versions of
	// the loop to avoid overhead of conditions.
	எனில் (!rex.reg_ic && !has_mbyte)
	    போது ((s = vim_strbyte(s, c)) != வெற்று)
	    {
		முழு len = prog->regmlen;
		எனில் (cstrncmp(s, prog->regmust, &len) == 0)
		    நிறுத்து;		// Found it.
		++s;
	    }
	இல்லையெனில் எனில் (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	    போது ((s = vim_strchr(s, c)) != வெற்று)
	    {
		முழு len = prog->regmlen;
		எனில் (cstrncmp(s, prog->regmust, &len) == 0)
		    நிறுத்து;		// Found it.
		MB_PTR_ADV(s);
	    }
	இல்லையெனில்
	    போது ((s = cstrchr(s, c)) != வெற்று)
	    {
		முழு len = prog->regmlen;
		எனில் (cstrncmp(s, prog->regmust, &len) == 0)
		    நிறுத்து;		// Found it.
		MB_PTR_ADV(s);
	    }
	எனில் (s == வெற்று)		// Not present.
	    செல் theend;
    }

    rex.வரி = வரி;
    rex.lnum = 0;
    reg_toolong = FALSE;

    // Simplest case: Anchored match need be tried only once.
    எனில் (prog->reganch)
    {
	முழு	c;

	எனில் (has_mbyte)
	    c = (*mb_ptr2char)(rex.வரி + col);
	இல்லையெனில்
	    c = rex.வரி[col];
	எனில் (prog->regstart == NUL
		|| prog->regstart == c
		|| (rex.reg_ic
		    && (((enc_utf8 && utf_fold(prog->regstart) == utf_fold(c)))
			|| (c < 255 && prog->regstart < 255 &&
			    MB_TOLOWER(prog->regstart) == MB_TOLOWER(c)))))
	    retval = regtry(prog, col, timed_out);
	இல்லையெனில்
	    retval = 0;
    }
    இல்லையெனில்
    {
	// Messy cases:  unanchored match.
	போது (!got_int)
	{
	    எனில் (prog->regstart != NUL)
	    {
		// Skip until the char we know it must start with.
		// Used often, do some work to avoid call overhead.
		எனில் (!rex.reg_ic && !has_mbyte)
		    s = vim_strbyte(rex.வரி + col, prog->regstart);
		இல்லையெனில்
		    s = cstrchr(rex.வரி + col, prog->regstart);
		எனில் (s == வெற்று)
		{
		    retval = 0;
		    நிறுத்து;
		}
		col = (முழு)(s - rex.வரி);
	    }

	    // Check for maximum column to try.
	    எனில் (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	    {
		retval = 0;
		நிறுத்து;
	    }

	    retval = regtry(prog, col, timed_out);
	    எனில் (retval > 0)
		நிறுத்து;

	    // if not currently on the first line, get it again
	    எனில் (rex.lnum != 0)
	    {
		rex.lnum = 0;
		rex.வரி = reg_getline((linenr_T)0);
	    }
	    எனில் (rex.வரி[col] == NUL)
		நிறுத்து;
	    எனில் (has_mbyte)
		col += (*mb_ptr2len)(rex.வரி + col);
	    இல்லையெனில்
		++col;
#ifdef FEAT_RELTIME
	    எனில் (bt_did_time_out(timed_out))
		நிறுத்து;
#endif
	}
    }

theend:
    // Free "reg_tofree" when it's a bit big.
    // Free regstack and backpos if they are bigger than their initial size.
    எனில் (reg_tofreelen > 400)
	VIM_CLEAR(reg_tofree);
    எனில் (regstack.ga_maxlen > REGSTACK_INITIAL)
	ga_clear(&regstack);
    எனில் (backpos.ga_maxlen > BACKPOS_INITIAL)
	ga_clear(&backpos);

    எனில் (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs
	// and \ze are used.
	எனில் (REG_MULTI)
	{
	    lpos_T *start = &rex.reg_mmatch->startpos[0];
	    lpos_T *end = &rex.reg_mmatch->endpos[0];

	    எனில் (end->lnum < start->lnum
			|| (end->lnum == start->lnum && end->col < start->col))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_mmatch->rmm_matchcol = col;
	}
	இல்லையெனில்
	{
	    எனில் (rex.reg_match->endp[0] < rex.reg_match->startp[0])
		rex.reg_match->endp[0] = rex.reg_match->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_match->rm_matchcol = col;
	}
    }

    திரும்பு retval;
}

/*
 * Match a regexp against a string.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 * if "line_lbr" is TRUE  consider a "\n" in "line" to be a line break.
 *
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    நிலையான முழு
bt_regexec_nl(
    regmatch_T	*rmp,
    char_u	*வரி,	// string to match against
    colnr_T	col,	// column to start looking for match
    முழு		வரி_lbr)
{
    rex.reg_match = rmp;
    rex.reg_mmatch = வெற்று;
    rex.reg_maxline = 0;
    rex.reg_வரி_lbr = வரி_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = வெற்று;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;

    திரும்பு bt_regexec_both(வரி, col, வெற்று);
}

/*
 * Match a regexp against multiple lines.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 *
 * Return zero if there is no match.  Return number of lines contained in the
 * match otherwise.
 */
    நிலையான long
bt_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	col,		// column to start looking for match
    முழு		*timed_out)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    திரும்பு bt_regexec_both(வெற்று, col, timed_out);
}

/*
 * Compare a number with the operand of RE_LNUM, RE_COL or RE_VCOL.
 */
    நிலையான முழு
re_இலக்கம்_cmp(long_u val, char_u *scan)
{
    long_u  n = OPERAND_MIN(scan);

    எனில் (OPERAND_CMP(scan) == '>')
	திரும்பு val > n;
    எனில் (OPERAND_CMP(scan) == '<')
	திரும்பு val < n;
    திரும்பு val == n;
}

#ifdef BT_REGEXP_DUMP

/*
 * regdump - dump a regexp onto stdout in vaguely comprehensible form
 */
    நிலையான வெறுமை
regdump(char_u *pattern, bt_regprog_T *r)
{
    char_u  *s;
    முழு	    op = EXACTLY;	// Arbitrary non-END op.
    char_u  *next;
    char_u  *end = வெற்று;
    FILE    *f;

# ifdef BT_REGEXP_LOG
    f = fopen("bt_regexp_log.log", "a");
# இல்லையெனில்
    f = stdout;
# endif
    எனில் (f == வெற்று)
	திரும்பு;
    fprintf(f, "-------------------------------------\n\r\nregcomp(%s):\r\n", pattern);

    s = r->program + 1;
    // Loop until we find the END that isn't before a referred next (an END
    // can also appear in a NOMATCH operand).
    போது ((op != END || s <= end) && s < r->program + r->regsz)
    {
	op = OP(s);
	fprintf(f, "%2d%s", (முழு)(s - r->program), regprop(s)); // Where, what.
	next = (s + 3 <= r->program + r->regsz) ? regnext(s) : வெற்று;
	எனில் (next == வெற்று)	// Next ptr.
	    fprintf(f, "(0)");
	இல்லையெனில்
	    fprintf(f, "(%d)", (முழு)((s - r->program) + (next - s)));
	எனில் (end < next)
	    end = next;
	எனில் (op == BRACE_LIMITS)
	{
	    // Two ints
	    fprintf(f, " minval %ld, maxval %ld", OPERAND_MIN(s), OPERAND_MAX(s));
	    s += 8;
	}
	இல்லையெனில் எனில் (op == BEHIND || op == NOBEHIND)
	{
	    // one int
	    fprintf(f, " count %ld", OPERAND_MIN(s));
	    s += 4;
	}
	இல்லையெனில் எனில் (op == RE_LNUM || op == RE_COL || op == RE_VCOL)
	{
	    // one int plus comparator
	    fprintf(f, " count %ld", OPERAND_MIN(s));
	    s += 5;
	}
	s += 3;
	எனில் (op == MULTIBYTECODE)
	{
	    fprintf(f, " mbc=%d", utf_ptr2char(s));
	    s += utfc_ptr2len(s);
	}
	எனில் (op == ANYOF || op == ANYOF + ADD_NL
		|| op == ANYBUT || op == ANYBUT + ADD_NL
		|| op == EXACTLY)
	{
	    // Literal string, where present.
	    fprintf(f, "\nxxxxxxxxx\n");
	    போது (*s != NUL && s < r->program + r->regsz)
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
    எனில் (r->regstart != NUL)
	fprintf(f, "start `%s' 0x%x; ", r->regstart < 256
		? (எழுத்து *)transchar(r->regstart)
		: "multibyte", r->regstart);
    எனில் (r->reganch)
	fprintf(f, "anchored; ");
    எனில் (r->regmust != வெற்று)
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
    நிலையான char_u *
regprop(char_u *op)
{
    எழுத்து	    *p;
    நிலையான எழுத்து	    buf[50];
    நிலையான size_t   buflen = 0;

    STRCPY(buf, ":");
    buflen = 1;

    தேர்வு ((முழு) OP(op))
    {
      நிலை BOL:
	p = "BOL";
	நிறுத்து;
      நிலை EOL:
	p = "EOL";
	நிறுத்து;
      நிலை RE_BOF:
	p = "BOF";
	நிறுத்து;
      நிலை RE_EOF:
	p = "EOF";
	நிறுத்து;
      நிலை CURSOR:
	p = "CURSOR";
	நிறுத்து;
      நிலை RE_VISUAL:
	p = "RE_VISUAL";
	நிறுத்து;
      நிலை RE_LNUM:
	p = "RE_LNUM";
	நிறுத்து;
      நிலை RE_MARK:
	p = "RE_MARK";
	நிறுத்து;
      நிலை RE_COL:
	p = "RE_COL";
	நிறுத்து;
      நிலை RE_VCOL:
	p = "RE_VCOL";
	நிறுத்து;
      நிலை BOW:
	p = "BOW";
	நிறுத்து;
      நிலை EOW:
	p = "EOW";
	நிறுத்து;
      நிலை ANY:
	p = "ANY";
	நிறுத்து;
      நிலை ANY + ADD_NL:
	p = "ANY+NL";
	நிறுத்து;
      நிலை ANYOF:
	p = "ANYOF";
	நிறுத்து;
      நிலை ANYOF + ADD_NL:
	p = "ANYOF+NL";
	நிறுத்து;
      நிலை ANYBUT:
	p = "ANYBUT";
	நிறுத்து;
      நிலை ANYBUT + ADD_NL:
	p = "ANYBUT+NL";
	நிறுத்து;
      நிலை IDENT:
	p = "IDENT";
	நிறுத்து;
      நிலை IDENT + ADD_NL:
	p = "IDENT+NL";
	நிறுத்து;
      நிலை SIDENT:
	p = "SIDENT";
	நிறுத்து;
      நிலை SIDENT + ADD_NL:
	p = "SIDENT+NL";
	நிறுத்து;
      நிலை KWORD:
	p = "KWORD";
	நிறுத்து;
      நிலை KWORD + ADD_NL:
	p = "KWORD+NL";
	நிறுத்து;
      நிலை SKWORD:
	p = "SKWORD";
	நிறுத்து;
      நிலை SKWORD + ADD_NL:
	p = "SKWORD+NL";
	நிறுத்து;
      நிலை FNAME:
	p = "FNAME";
	நிறுத்து;
      நிலை FNAME + ADD_NL:
	p = "FNAME+NL";
	நிறுத்து;
      நிலை SFNAME:
	p = "SFNAME";
	நிறுத்து;
      நிலை SFNAME + ADD_NL:
	p = "SFNAME+NL";
	நிறுத்து;
      நிலை PRINT:
	p = "PRINT";
	நிறுத்து;
      நிலை PRINT + ADD_NL:
	p = "PRINT+NL";
	நிறுத்து;
      நிலை SPRINT:
	p = "SPRINT";
	நிறுத்து;
      நிலை SPRINT + ADD_NL:
	p = "SPRINT+NL";
	நிறுத்து;
      நிலை WHITE:
	p = "WHITE";
	நிறுத்து;
      நிலை WHITE + ADD_NL:
	p = "WHITE+NL";
	நிறுத்து;
      நிலை NWHITE:
	p = "NWHITE";
	நிறுத்து;
      நிலை NWHITE + ADD_NL:
	p = "NWHITE+NL";
	நிறுத்து;
      நிலை DIGIT:
	p = "DIGIT";
	நிறுத்து;
      நிலை DIGIT + ADD_NL:
	p = "DIGIT+NL";
	நிறுத்து;
      நிலை NDIGIT:
	p = "NDIGIT";
	நிறுத்து;
      நிலை NDIGIT + ADD_NL:
	p = "NDIGIT+NL";
	நிறுத்து;
      நிலை HEX:
	p = "HEX";
	நிறுத்து;
      நிலை HEX + ADD_NL:
	p = "HEX+NL";
	நிறுத்து;
      நிலை NHEX:
	p = "NHEX";
	நிறுத்து;
      நிலை NHEX + ADD_NL:
	p = "NHEX+NL";
	நிறுத்து;
      நிலை OCTAL:
	p = "OCTAL";
	நிறுத்து;
      நிலை OCTAL + ADD_NL:
	p = "OCTAL+NL";
	நிறுத்து;
      நிலை NOCTAL:
	p = "NOCTAL";
	நிறுத்து;
      நிலை NOCTAL + ADD_NL:
	p = "NOCTAL+NL";
	நிறுத்து;
      நிலை WORD:
	p = "WORD";
	நிறுத்து;
      நிலை WORD + ADD_NL:
	p = "WORD+NL";
	நிறுத்து;
      நிலை NWORD:
	p = "NWORD";
	நிறுத்து;
      நிலை NWORD + ADD_NL:
	p = "NWORD+NL";
	நிறுத்து;
      நிலை HEAD:
	p = "HEAD";
	நிறுத்து;
      நிலை HEAD + ADD_NL:
	p = "HEAD+NL";
	நிறுத்து;
      நிலை NHEAD:
	p = "NHEAD";
	நிறுத்து;
      நிலை NHEAD + ADD_NL:
	p = "NHEAD+NL";
	நிறுத்து;
      நிலை ALPHA:
	p = "ALPHA";
	நிறுத்து;
      நிலை ALPHA + ADD_NL:
	p = "ALPHA+NL";
	நிறுத்து;
      நிலை NALPHA:
	p = "NALPHA";
	நிறுத்து;
      நிலை NALPHA + ADD_NL:
	p = "NALPHA+NL";
	நிறுத்து;
      நிலை LOWER:
	p = "LOWER";
	நிறுத்து;
      நிலை LOWER + ADD_NL:
	p = "LOWER+NL";
	நிறுத்து;
      நிலை NLOWER:
	p = "NLOWER";
	நிறுத்து;
      நிலை NLOWER + ADD_NL:
	p = "NLOWER+NL";
	நிறுத்து;
      நிலை UPPER:
	p = "UPPER";
	நிறுத்து;
      நிலை UPPER + ADD_NL:
	p = "UPPER+NL";
	நிறுத்து;
      நிலை NUPPER:
	p = "NUPPER";
	நிறுத்து;
      நிலை NUPPER + ADD_NL:
	p = "NUPPER+NL";
	நிறுத்து;
      நிலை BRANCH:
	p = "BRANCH";
	நிறுத்து;
      நிலை EXACTLY:
	p = "EXACTLY";
	நிறுத்து;
      நிலை NOTHING:
	p = "NOTHING";
	நிறுத்து;
      நிலை BACK:
	p = "BACK";
	நிறுத்து;
      நிலை END:
	p = "END";
	நிறுத்து;
      நிலை MOPEN + 0:
	p = "MATCH START";
	நிறுத்து;
      நிலை MOPEN + 1:
      நிலை MOPEN + 2:
      நிலை MOPEN + 3:
      நிலை MOPEN + 4:
      நிலை MOPEN + 5:
      நிலை MOPEN + 6:
      நிலை MOPEN + 7:
      நிலை MOPEN + 8:
      நிலை MOPEN + 9:
	buflen += vim_snprintf(buf + buflen, அளவு(buf) - buflen, "MOPEN%d", OP(op) - MOPEN);
	p = வெற்று;
	நிறுத்து;
      நிலை MCLOSE + 0:
	p = "MATCH END";
	நிறுத்து;
      நிலை MCLOSE + 1:
      நிலை MCLOSE + 2:
      நிலை MCLOSE + 3:
      நிலை MCLOSE + 4:
      நிலை MCLOSE + 5:
      நிலை MCLOSE + 6:
      நிலை MCLOSE + 7:
      நிலை MCLOSE + 8:
      நிலை MCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, அளவு(buf) - buflen, "MCLOSE%d", OP(op) - MCLOSE);
	p = வெற்று;
	நிறுத்து;
      நிலை BACKREF + 1:
      நிலை BACKREF + 2:
      நிலை BACKREF + 3:
      நிலை BACKREF + 4:
      நிலை BACKREF + 5:
      நிலை BACKREF + 6:
      நிலை BACKREF + 7:
      நிலை BACKREF + 8:
      நிலை BACKREF + 9:
	buflen += vim_snprintf(buf + buflen, அளவு(buf) - buflen, "BACKREF%d", OP(op) - BACKREF);
	p = வெற்று;
	நிறுத்து;
      நிலை NOPEN:
	p = "NOPEN";
	நிறுத்து;
      நிலை NCLOSE:
	p = "NCLOSE";
	நிறுத்து;
# ifdef FEAT_SYN_HL
      நிலை ZOPEN + 1:
      நிலை ZOPEN + 2:
      நிலை ZOPEN + 3:
      நிலை ZOPEN + 4:
      நிலை ZOPEN + 5:
      நிலை ZOPEN + 6:
      நிலை ZOPEN + 7:
      நிலை ZOPEN + 8:
      நிலை ZOPEN + 9:
	buflen += vim_snprintf(buf + buflen, அளவு(buf) - buflen, "ZOPEN%d", OP(op) - ZOPEN);
	p = வெற்று;
	நிறுத்து;
      நிலை ZCLOSE + 1:
      நிலை ZCLOSE + 2:
      நிலை ZCLOSE + 3:
      நிலை ZCLOSE + 4:
      நிலை ZCLOSE + 5:
      நிலை ZCLOSE + 6:
      நிலை ZCLOSE + 7:
      நிலை ZCLOSE + 8:
      நிலை ZCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, அளவு(buf) - buflen, "ZCLOSE%d", OP(op) - ZCLOSE);
	p = வெற்று;
	நிறுத்து;
      நிலை ZREF + 1:
      நிலை ZREF + 2:
      நிலை ZREF + 3:
      நிலை ZREF + 4:
      நிலை ZREF + 5:
      நிலை ZREF + 6:
      நிலை ZREF + 7:
      நிலை ZREF + 8:
      நிலை ZREF + 9:
	buflen += vim_snprintf(buf + buflen, அளவு(buf) - buflen, "ZREF%d", OP(op) - ZREF);
	p = வெற்று;
	நிறுத்து;
# endif
      நிலை STAR:
	p = "STAR";
	நிறுத்து;
      நிலை PLUS:
	p = "PLUS";
	நிறுத்து;
      நிலை NOMATCH:
	p = "NOMATCH";
	நிறுத்து;
      நிலை MATCH:
	p = "MATCH";
	நிறுத்து;
      நிலை BEHIND:
	p = "BEHIND";
	நிறுத்து;
      நிலை NOBEHIND:
	p = "NOBEHIND";
	நிறுத்து;
      நிலை SUBPAT:
	p = "SUBPAT";
	நிறுத்து;
      நிலை BRACE_LIMITS:
	p = "BRACE_LIMITS";
	நிறுத்து;
      நிலை BRACE_SIMPLE:
	p = "BRACE_SIMPLE";
	நிறுத்து;
      நிலை BRACE_COMPLEX + 0:
      நிலை BRACE_COMPLEX + 1:
      நிலை BRACE_COMPLEX + 2:
      நிலை BRACE_COMPLEX + 3:
      நிலை BRACE_COMPLEX + 4:
      நிலை BRACE_COMPLEX + 5:
      நிலை BRACE_COMPLEX + 6:
      நிலை BRACE_COMPLEX + 7:
      நிலை BRACE_COMPLEX + 8:
      நிலை BRACE_COMPLEX + 9:
	buflen += vim_snprintf(buf + buflen, அளவு(buf) - buflen, "BRACE_COMPLEX%d", OP(op) - BRACE_COMPLEX);
	p = வெற்று;
	நிறுத்து;
      நிலை MULTIBYTECODE:
	p = "MULTIBYTECODE";
	நிறுத்து;
      நிலை NEWL:
	p = "NEWL";
	நிறுத்து;
      இயல்பு:
	buflen += vim_snprintf(buf + buflen, அளவு(buf) - buflen, "corrupt %d", OP(op));
	p = வெற்று;
	நிறுத்து;
    }
    எனில் (p != வெற்று)
	vim_strncpy((char_u *)buf + buflen, (char_u *)p, அளவு(buf) - buflen - 1);
    திரும்பு (char_u *)buf;
}
#endif	    // DEBUG
