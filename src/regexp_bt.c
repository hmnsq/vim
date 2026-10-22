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

സ്ഥിതം പൂർണ്ണസംഖ്യ	അക്കം_complex_braces; // Complex \{...} count
സ്ഥിതം char_u	*regcode;	// Code-emit pointer, or JUST_CALC_SIZE
സ്ഥിതം long	regsize;	// Code size.
സ്ഥിതം പൂർണ്ണസംഖ്യ	reg_toolong;	// TRUE when offset out of range
സ്ഥിതം പൂർണ്ണസംഖ്യ	bt_reg_parse_depth;	// nesting depth in reg()
സ്ഥിതം char_u	had_endbrace[NSUBEXP];	// flags, TRUE if end of () found
സ്ഥിതം long	brace_min[10];	// Minimums for complex brace repeats
സ്ഥിതം long	brace_max[10];	// Maximums for complex brace repeats
സ്ഥിതം പൂർണ്ണസംഖ്യ	brace_എണ്ണം[10]; // Current counts for complex brace repeats
സ്ഥിതം പൂർണ്ണസംഖ്യ	one_exactly = FALSE;	// only do one char for EXACTLY

// When making changes to classchars also change nfa_classcodes.
സ്ഥിതം char_u	*classchars = (char_u *)".iIkKfFpPsSdDxXoOwWhHaAlLuU";
സ്ഥിതം സ്ഥിരം പൂർണ്ണസംഖ്യ classcodes[] = {
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
bt_recognize_char_class(char_u *start, char_u *end)
{
    പൂർണ്ണസംഖ്യ		newl;
    പൂർണ്ണസംഖ്യ		config = get_char_class_bits(start, end, &newl);

    // A newline would need the ADD_NL variant, don't bother with it here.
    എങ്കിൽ (config < 0 || newl)
	തിരികെ 0;

	// The old engine has no case-insensitive class opcode, so [a-z] and [A-Z]
	// are left as collections.  The classes below are case-independent.
    തിരഞ്ഞെടുപ്പ് (config)
    {
	അവസ്ഥ CLASS_o9:
	    തിരികെ DIGIT;
	അവസ്ഥ CLASS_not | CLASS_o9:
	    തിരികെ NDIGIT;
	അവസ്ഥ CLASS_af | CLASS_AF | CLASS_o9:
	    തിരികെ HEX;
	അവസ്ഥ CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    തിരികെ NHEX;
	അവസ്ഥ CLASS_o7:
	    തിരികെ OCTAL;
	അവസ്ഥ CLASS_not | CLASS_o7:
	    തിരികെ NOCTAL;
	അവസ്ഥ CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    തിരികെ WORD;
	അവസ്ഥ CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    തിരികെ NWORD;
	അവസ്ഥ CLASS_az | CLASS_AZ | CLASS_underscore:
	    തിരികെ HEAD;
	അവസ്ഥ CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    തിരികെ NHEAD;
	അവസ്ഥ CLASS_az | CLASS_AZ:
	    തിരികെ ALPHA;
	അവസ്ഥ CLASS_not | CLASS_az | CLASS_AZ:
	    തിരികെ NALPHA;
    }
    തിരികെ 0;
}

/*
 * When regcode is set to this value, code is not emitted and size is computed
 * instead.
 */
#define JUST_CALC_SIZE	((char_u *) -1)

// Values for rs_state in regitem_T.
തരനിർവചനം എണ്ണൽ regstate_E
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
തരനിർവചനം ഘടന
{
    സംഘം
    {
	char_u	*ptr;	// rex.input pointer, for single-line regexp
	lpos_T	pos;	// rex.input pos, for multi-line regexp
    } rs_u;
    പൂർണ്ണസംഖ്യ		rs_len;
} regsave_T;

// struct to save start/end pointer/position in for \(\)
തരനിർവചനം ഘടന
{
    സംഘം
    {
	char_u	*ptr;
	lpos_T	pos;
    } se_u;
} save_se_T;

// used for BEHIND and NOBEHIND matching
തരനിർവചനം ഘടന regbehind_S
{
    regsave_T	save_after;
    regsave_T	save_behind;
    പൂർണ്ണസംഖ്യ		save_need_clear_subexpr;
    save_se_T   save_start[NSUBEXP];
    save_se_T   save_end[NSUBEXP];
} regbehind_T;

/*
 * When there are alternatives a regstate_T is put on the regstack to remember
 * what we are doing.
 * Before it may be another type of item, depending on rs_state, to remember
 * more things.
 */
തരനിർവചനം ഘടന regitem_S
{
    regstate_T	rs_state;	// what we are doing, one of RS_ above
    short	rs_no;		// submatch nr or BEHIND/NOBEHIND
    char_u	*rs_scan;	// current node in program
    സംഘം
    {
	save_se_T  sesave;
	regsave_T  regsave;
    } rs_un;			// room for saving rex.input
} regitem_T;


// used for STAR, PLUS and BRACE_SIMPLE matching
തരനിർവചനം ഘടന regstar_S
{
    പൂർണ്ണസംഖ്യ		nextb;		// next byte
    പൂർണ്ണസംഖ്യ		nextb_ic;	// next byte reverse case
    long	എണ്ണം;
    long	minval;
    long	maxval;
} regstar_T;

// used to store input position when a BACK was encountered, so that we now if
// we made any progress since the last time.
തരനിർവചനം ഘടന backpos_S
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
സ്ഥിതം garray_T	regstack = {0, 0, 0, 0, ശൂന്യ};
സ്ഥിതം garray_T	backpos = {0, 0, 0, 0, ശൂന്യ};

സ്ഥിതം regsave_T behind_pos;

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
#define OP(p)		((പൂർണ്ണസംഖ്യ)*(p))
#define NEXT(p)		(((*((p) + 1) & 0377) << 8) + (*((p) + 2) & 0377))
#define OPERAND(p)	((p) + 3)
// Obtain an operand that was stored as four bytes, MSB first.
#define OPERAND_MIN(p)	(((long)(p)[3] << 24) + ((long)(p)[4] << 16) \
			+ ((long)(p)[5] << 8) + (long)(p)[6])
// Obtain a second operand stored as four bytes.
#define OPERAND_MAX(p)	OPERAND_MIN((p) + 4)
// Obtain a second single-byte operand stored after a four bytes operand.
#define OPERAND_CMP(p)	(p)[7]

സ്ഥിതം char_u *reg(പൂർണ്ണസംഖ്യ paren, പൂർണ്ണസംഖ്യ *flagp);

#ifdef BT_REGEXP_DUMP
സ്ഥിതം ശൂന്യം	regdump(char_u *, bt_regprog_T *);
#endif

സ്ഥിതം പൂർണ്ണസംഖ്യ	re_അക്കം_cmp(long_u val, char_u *scan);

#ifdef DEBUG
സ്ഥിതം char_u	*regprop(char_u *);

സ്ഥിതം പൂർണ്ണസംഖ്യ	regnarrate = 0;
#endif


/*
 * Setup to parse the regexp.  Used once to get the length and once to do it.
 */
    സ്ഥിതം ശൂന്യം
regcomp_start(
    char_u	*വ്യഞ്ജകം,
    പൂർണ്ണസംഖ്യ		re_flags)	    // see vim_regcomp()
{
    initchr(വ്യഞ്ജകം);
    എങ്കിൽ (re_flags & RE_MAGIC)
	reg_magic = MAGIC_ON;
    അല്ലെങ്കിൽ
	reg_magic = MAGIC_OFF;
    reg_string = (re_flags & RE_STRING);
    reg_strict = (re_flags & RE_STRICT);
    get_cpo_flags();

    അക്കം_complex_braces = 0;
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
#എങ്കിൽ defined(FEAT_SYN_HL)
    had_eol = FALSE;
#endif
}

/*
 * Return TRUE if MULTIBYTECODE should be used instead of EXACTLY for
 * character "c".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
use_multibytecode(പൂർണ്ണസംഖ്യ c)
{
    തിരികെ has_mbyte && (*mb_char2len)(c) > 1
		     && (re_multi_type(peekchr()) != NOT_MULTI
			     || (enc_utf8 && utf_iscomposing(c)));
}

/*
 * Emit (if appropriate) a byte of code
 */
    സ്ഥിതം ശൂന്യം
regc(പൂർണ്ണസംഖ്യ b)
{
    എങ്കിൽ (regcode == JUST_CALC_SIZE)
	regsize++;
    അല്ലെങ്കിൽ
	*regcode++ = b;
}

/*
 * Emit (if appropriate) a multi-byte character of code
 */
    സ്ഥിതം ശൂന്യം
regmbc(പൂർണ്ണസംഖ്യ c)
{
    എങ്കിൽ (!has_mbyte && c > 0xff)
	തിരികെ;
    എങ്കിൽ (regcode == JUST_CALC_SIZE)
	regsize += (*mb_char2len)(c);
    അല്ലെങ്കിൽ
	regcode += (*mb_char2bytes)(c, regcode);
}


/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * NOTE: When changing this function, also change nfa_emit_equi_class()
 */
    സ്ഥിതം ശൂന്യം
reg_equi_class(പൂർണ്ണസംഖ്യ c)
{
    എങ്കിൽ (enc_utf8 || STRCMP(p_enc, "latin1") == 0
					 || STRCMP(p_enc, "iso-8859-15") == 0)
    {
	തിരഞ്ഞെടുപ്പ് (c)
	{
	    // Do not use '\300' style, it results in a negative number.
	    അവസ്ഥ 'A': അവസ്ഥ 0xc0: അവസ്ഥ 0xc1: അവസ്ഥ 0xc2: അവസ്ഥ 0xc3: അവസ്ഥ 0xc4:
	    അവസ്ഥ 0xc5: അവസ്ഥ 0x100: അവസ്ഥ 0x102: അവസ്ഥ 0x104: അവസ്ഥ 0x1cd:
	    അവസ്ഥ 0x1de: അവസ്ഥ 0x1e0: അവസ്ഥ 0x1fa: അവസ്ഥ 0x202: അവസ്ഥ 0x226:
	    അവസ്ഥ 0x23a: അവസ്ഥ 0x1e00: അവസ്ഥ 0x1ea0: അവസ്ഥ 0x1ea2: അവസ്ഥ 0x1ea4:
	    അവസ്ഥ 0x1ea6: അവസ്ഥ 0x1ea8: അവസ്ഥ 0x1eaa: അവസ്ഥ 0x1eac: അവസ്ഥ 0x1eae:
	    അവസ്ഥ 0x1eb0: അവസ്ഥ 0x1eb2: അവസ്ഥ 0x1eb4: അവസ്ഥ 0x1eb6:
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
		      തിരികെ;
	    അവസ്ഥ 'B': അവസ്ഥ 0x181: അവസ്ഥ 0x243: അവസ്ഥ 0x1e02:
	    അവസ്ഥ 0x1e04: അവസ്ഥ 0x1e06:
		      regmbc('B');
		      regmbc(0x181); regmbc(0x243); regmbc(0x1e02);
		      regmbc(0x1e04); regmbc(0x1e06);
		      തിരികെ;
	    അവസ്ഥ 'C': അവസ്ഥ 0xc7:
	    അവസ്ഥ 0x106: അവസ്ഥ 0x108: അവസ്ഥ 0x10a: അവസ്ഥ 0x10c: അവസ്ഥ 0x187:
	    അവസ്ഥ 0x23b: അവസ്ഥ 0x1e08: അവസ്ഥ 0xa792:
		      regmbc('C'); regmbc(0xc7);
		      regmbc(0x106); regmbc(0x108); regmbc(0x10a);
		      regmbc(0x10c); regmbc(0x187); regmbc(0x23b);
		      regmbc(0x1e08); regmbc(0xa792);
		      തിരികെ;
	    അവസ്ഥ 'D': അവസ്ഥ 0x10e: അവസ്ഥ 0x110: അവസ്ഥ 0x18a:
	    അവസ്ഥ 0x1e0a: അവസ്ഥ 0x1e0c: അവസ്ഥ 0x1e0e: അവസ്ഥ 0x1e10:
	    അവസ്ഥ 0x1e12:
		      regmbc('D'); regmbc(0x10e); regmbc(0x110);
		      regmbc(0x18a); regmbc(0x1e0a); regmbc(0x1e0c);
		      regmbc(0x1e0e); regmbc(0x1e10); regmbc(0x1e12);
		      തിരികെ;
	    അവസ്ഥ 'E': അവസ്ഥ 0xc8: അവസ്ഥ 0xc9: അവസ്ഥ 0xca: അവസ്ഥ 0xcb:
	    അവസ്ഥ 0x112: അവസ്ഥ 0x114: അവസ്ഥ 0x116: അവസ്ഥ 0x118: അവസ്ഥ 0x11a:
	    അവസ്ഥ 0x204: അവസ്ഥ 0x206: അവസ്ഥ 0x228: അവസ്ഥ 0x246: അവസ്ഥ 0x1e14:
	    അവസ്ഥ 0x1e16: അവസ്ഥ 0x1e18: അവസ്ഥ 0x1e1a: അവസ്ഥ 0x1e1c:
	    അവസ്ഥ 0x1eb8: അവസ്ഥ 0x1eba: അവസ്ഥ 0x1ebc: അവസ്ഥ 0x1ebe:
	    അവസ്ഥ 0x1ec0: അവസ്ഥ 0x1ec2: അവസ്ഥ 0x1ec4: അവസ്ഥ 0x1ec6:
		      regmbc('E'); regmbc(0xc8); regmbc(0xc9);
		      regmbc(0xca); regmbc(0xcb); regmbc(0x112);
		      regmbc(0x114); regmbc(0x116); regmbc(0x118);
		      regmbc(0x11a); regmbc(0x204); regmbc(0x206);
		      regmbc(0x228); regmbc(0x246); regmbc(0x1e14);
		      regmbc(0x1e16); regmbc(0x1e18); regmbc(0x1e1a);
		      regmbc(0x1e1c); regmbc(0x1eb8); regmbc(0x1eba);
		      regmbc(0x1ebc); regmbc(0x1ebe); regmbc(0x1ec0);
		      regmbc(0x1ec2); regmbc(0x1ec4); regmbc(0x1ec6);
		      തിരികെ;
	    അവസ്ഥ 'F': അവസ്ഥ 0x191: അവസ്ഥ 0x1e1e: അവസ്ഥ 0xa798:
		      regmbc('F'); regmbc(0x191); regmbc(0x1e1e);
		      regmbc(0xa798);
		      തിരികെ;
	    അവസ്ഥ 'G': അവസ്ഥ 0x11c: അവസ്ഥ 0x11e: അവസ്ഥ 0x120:
	    അവസ്ഥ 0x122: അവസ്ഥ 0x193: അവസ്ഥ 0x1e4: അവസ്ഥ 0x1e6:
	    അവസ്ഥ 0x1f4: അവസ്ഥ 0x1e20: അവസ്ഥ 0xa7a0:
		      regmbc('G'); regmbc(0x11c); regmbc(0x11e);
		      regmbc(0x120); regmbc(0x122); regmbc(0x193);
		      regmbc(0x1e4); regmbc(0x1e6); regmbc(0x1f4);
		      regmbc(0x1e20); regmbc(0xa7a0);
		      തിരികെ;
	    അവസ്ഥ 'H': അവസ്ഥ 0x124: അവസ്ഥ 0x126: അവസ്ഥ 0x21e:
	    അവസ്ഥ 0x1e22: അവസ്ഥ 0x1e24: അവസ്ഥ 0x1e26:
	    അവസ്ഥ 0x1e28: അവസ്ഥ 0x1e2a: അവസ്ഥ 0x2c67:
		      regmbc('H'); regmbc(0x124); regmbc(0x126);
		      regmbc(0x21e); regmbc(0x1e22); regmbc(0x1e24);
		      regmbc(0x1e26); regmbc(0x1e28); regmbc(0x1e2a);
		      regmbc(0x2c67);
		      തിരികെ;
	    അവസ്ഥ 'I': അവസ്ഥ 0xcc: അവസ്ഥ 0xcd: അവസ്ഥ 0xce: അവസ്ഥ 0xcf:
	    അവസ്ഥ 0x128: അവസ്ഥ 0x12a: അവസ്ഥ 0x12c: അവസ്ഥ 0x12e:
	    അവസ്ഥ 0x130: അവസ്ഥ 0x197: അവസ്ഥ 0x1cf: അവസ്ഥ 0x208:
	    അവസ്ഥ 0x20a: അവസ്ഥ 0x1e2c: അവസ്ഥ 0x1e2e: അവസ്ഥ 0x1ec8:
	    അവസ്ഥ 0x1eca:
		      regmbc('I'); regmbc(0xcc); regmbc(0xcd);
		      regmbc(0xce); regmbc(0xcf); regmbc(0x128);
		      regmbc(0x12a); regmbc(0x12c); regmbc(0x12e);
		      regmbc(0x130); regmbc(0x197); regmbc(0x1cf);
		      regmbc(0x208); regmbc(0x20a); regmbc(0x1e2c);
		      regmbc(0x1e2e); regmbc(0x1ec8); regmbc(0x1eca);
		      തിരികെ;
	    അവസ്ഥ 'J': അവസ്ഥ 0x134: അവസ്ഥ 0x248:
		      regmbc('J'); regmbc(0x134); regmbc(0x248);
		      തിരികെ;
	    അവസ്ഥ 'K': അവസ്ഥ 0x136: അവസ്ഥ 0x198: അവസ്ഥ 0x1e8: അവസ്ഥ 0x1e30:
	    അവസ്ഥ 0x1e32: അവസ്ഥ 0x1e34: അവസ്ഥ 0x2c69: അവസ്ഥ 0xa740:
		      regmbc('K'); regmbc(0x136); regmbc(0x198);
		      regmbc(0x1e8); regmbc(0x1e30); regmbc(0x1e32);
		      regmbc(0x1e34); regmbc(0x2c69); regmbc(0xa740);
		      തിരികെ;
	    അവസ്ഥ 'L': അവസ്ഥ 0x139: അവസ്ഥ 0x13b: അവസ്ഥ 0x13d: അവസ്ഥ 0x13f:
	    അവസ്ഥ 0x141: അവസ്ഥ 0x23d: അവസ്ഥ 0x1e36: അവസ്ഥ 0x1e38:
	    അവസ്ഥ 0x1e3a: അവസ്ഥ 0x1e3c: അവസ്ഥ 0x2c60:
		      regmbc('L'); regmbc(0x139); regmbc(0x13b);
		      regmbc(0x13d); regmbc(0x13f); regmbc(0x141);
		      regmbc(0x23d); regmbc(0x1e36); regmbc(0x1e38);
		      regmbc(0x1e3a); regmbc(0x1e3c); regmbc(0x2c60);
		      തിരികെ;
	    അവസ്ഥ 'M': അവസ്ഥ 0x1e3e: അവസ്ഥ 0x1e40: അവസ്ഥ 0x1e42:
		      regmbc('M'); regmbc(0x1e3e); regmbc(0x1e40);
		      regmbc(0x1e42);
		      തിരികെ;
	    അവസ്ഥ 'N': അവസ്ഥ 0xd1:
	    അവസ്ഥ 0x143: അവസ്ഥ 0x145: അവസ്ഥ 0x147: അവസ്ഥ 0x1f8:
	    അവസ്ഥ 0x1e44: അവസ്ഥ 0x1e46: അവസ്ഥ 0x1e48: അവസ്ഥ 0x1e4a:
	    അവസ്ഥ 0xa7a4:
		      regmbc('N'); regmbc(0xd1);
		      regmbc(0x143); regmbc(0x145); regmbc(0x147);
		      regmbc(0x1f8); regmbc(0x1e44); regmbc(0x1e46);
		      regmbc(0x1e48); regmbc(0x1e4a); regmbc(0xa7a4);
		      തിരികെ;
	    അവസ്ഥ 'O': അവസ്ഥ 0xd2: അവസ്ഥ 0xd3: അവസ്ഥ 0xd4: അവസ്ഥ 0xd5: അവസ്ഥ 0xd6:
	    അവസ്ഥ 0xd8: അവസ്ഥ 0x14c: അവസ്ഥ 0x14e: അവസ്ഥ 0x150: അവസ്ഥ 0x19f:
	    അവസ്ഥ 0x1a0: അവസ്ഥ 0x1d1: അവസ്ഥ 0x1ea: അവസ്ഥ 0x1ec: അവസ്ഥ 0x1fe:
	    അവസ്ഥ 0x20c: അവസ്ഥ 0x20e: അവസ്ഥ 0x22a: അവസ്ഥ 0x22c: അവസ്ഥ 0x22e:
	    അവസ്ഥ 0x230: അവസ്ഥ 0x1e4c: അവസ്ഥ 0x1e4e: അവസ്ഥ 0x1e50: അവസ്ഥ 0x1e52:
	    അവസ്ഥ 0x1ecc: അവസ്ഥ 0x1ece: അവസ്ഥ 0x1ed0: അവസ്ഥ 0x1ed2: അവസ്ഥ 0x1ed4:
	    അവസ്ഥ 0x1ed6: അവസ്ഥ 0x1ed8: അവസ്ഥ 0x1eda: അവസ്ഥ 0x1edc: അവസ്ഥ 0x1ede:
	    അവസ്ഥ 0x1ee0: അവസ്ഥ 0x1ee2:
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
		      തിരികെ;
	    അവസ്ഥ 'P': അവസ്ഥ 0x1a4: അവസ്ഥ 0x1e54: അവസ്ഥ 0x1e56: അവസ്ഥ 0x2c63:
		      regmbc('P'); regmbc(0x1a4); regmbc(0x1e54);
		      regmbc(0x1e56); regmbc(0x2c63);
		      തിരികെ;
	    അവസ്ഥ 'Q': അവസ്ഥ 0x24a:
		      regmbc('Q'); regmbc(0x24a);
		      തിരികെ;
	    അവസ്ഥ 'R': അവസ്ഥ 0x154: അവസ്ഥ 0x156: അവസ്ഥ 0x158: അവസ്ഥ 0x210:
	    അവസ്ഥ 0x212: അവസ്ഥ 0x24c: അവസ്ഥ 0x1e58: അവസ്ഥ 0x1e5a:
	    അവസ്ഥ 0x1e5c: അവസ്ഥ 0x1e5e: അവസ്ഥ 0x2c64: അവസ്ഥ 0xa7a6:
		      regmbc('R'); regmbc(0x154); regmbc(0x156);
		      regmbc(0x210); regmbc(0x212); regmbc(0x158);
		      regmbc(0x24c); regmbc(0x1e58); regmbc(0x1e5a);
		      regmbc(0x1e5c); regmbc(0x1e5e); regmbc(0x2c64);
		      regmbc(0xa7a6);
		      തിരികെ;
	    അവസ്ഥ 'S': അവസ്ഥ 0x15a: അവസ്ഥ 0x15c: അവസ്ഥ 0x15e: അവസ്ഥ 0x160:
	    അവസ്ഥ 0x218: അവസ്ഥ 0x1e60: അവസ്ഥ 0x1e62: അവസ്ഥ 0x1e64:
	    അവസ്ഥ 0x1e66: അവസ്ഥ 0x1e68: അവസ്ഥ 0x2c7e: അവസ്ഥ 0xa7a8:
		      regmbc('S'); regmbc(0x15a); regmbc(0x15c);
		      regmbc(0x15e); regmbc(0x160); regmbc(0x218);
		      regmbc(0x1e60); regmbc(0x1e62); regmbc(0x1e64);
		      regmbc(0x1e66); regmbc(0x1e68); regmbc(0x2c7e);
			  regmbc(0xa7a8);
		      തിരികെ;
	    അവസ്ഥ 'T': അവസ്ഥ 0x162: അവസ്ഥ 0x164: അവസ്ഥ 0x166: അവസ്ഥ 0x1ac:
	    അവസ്ഥ 0x1ae: അവസ്ഥ 0x21a: അവസ്ഥ 0x23e: അവസ്ഥ 0x1e6a: അവസ്ഥ 0x1e6c:
	    അവസ്ഥ 0x1e6e: അവസ്ഥ 0x1e70:
		      regmbc('T'); regmbc(0x162); regmbc(0x164);
		      regmbc(0x166); regmbc(0x1ac); regmbc(0x23e);
		      regmbc(0x1ae); regmbc(0x21a); regmbc(0x1e6a);
		      regmbc(0x1e6c); regmbc(0x1e6e); regmbc(0x1e70);
		      തിരികെ;
	    അവസ്ഥ 'U': അവസ്ഥ 0xd9: അവസ്ഥ 0xda: അവസ്ഥ 0xdb: അവസ്ഥ 0xdc:
	    അവസ്ഥ 0x168: അവസ്ഥ 0x16a: അവസ്ഥ 0x16c: അവസ്ഥ 0x16e:
	    അവസ്ഥ 0x170: അവസ്ഥ 0x172: അവസ്ഥ 0x1af: അവസ്ഥ 0x1d3:
	    അവസ്ഥ 0x1d5: അവസ്ഥ 0x1d7: അവസ്ഥ 0x1d9: അവസ്ഥ 0x1db:
	    അവസ്ഥ 0x214: അവസ്ഥ 0x216: അവസ്ഥ 0x244: അവസ്ഥ 0x1e72:
	    അവസ്ഥ 0x1e74: അവസ്ഥ 0x1e76: അവസ്ഥ 0x1e78: അവസ്ഥ 0x1e7a:
	    അവസ്ഥ 0x1ee4: അവസ്ഥ 0x1ee6: അവസ്ഥ 0x1ee8: അവസ്ഥ 0x1eea:
	    അവസ്ഥ 0x1eec: അവസ്ഥ 0x1eee: അവസ്ഥ 0x1ef0:
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
		      തിരികെ;
	    അവസ്ഥ 'V': അവസ്ഥ 0x1b2: അവസ്ഥ 0x1e7c: അവസ്ഥ 0x1e7e:
		      regmbc('V'); regmbc(0x1b2); regmbc(0x1e7c);
		      regmbc(0x1e7e);
		      തിരികെ;
	    അവസ്ഥ 'W': അവസ്ഥ 0x174: അവസ്ഥ 0x1e80: അവസ്ഥ 0x1e82:
	    അവസ്ഥ 0x1e84: അവസ്ഥ 0x1e86: അവസ്ഥ 0x1e88:
		      regmbc('W'); regmbc(0x174); regmbc(0x1e80);
		      regmbc(0x1e82); regmbc(0x1e84); regmbc(0x1e86);
		      regmbc(0x1e88);
		      തിരികെ;
	    അവസ്ഥ 'X': അവസ്ഥ 0x1e8a: അവസ്ഥ 0x1e8c:
		      regmbc('X'); regmbc(0x1e8a); regmbc(0x1e8c);
		      തിരികെ;
	    അവസ്ഥ 'Y': അവസ്ഥ 0xdd:
	    അവസ്ഥ 0x176: അവസ്ഥ 0x178: അവസ്ഥ 0x1b3: അവസ്ഥ 0x232: അവസ്ഥ 0x24e:
	    അവസ്ഥ 0x1e8e: അവസ്ഥ 0x1ef2: അവസ്ഥ 0x1ef6: അവസ്ഥ 0x1ef4: അവസ്ഥ 0x1ef8:
		      regmbc('Y'); regmbc(0xdd); regmbc(0x176);
		      regmbc(0x178); regmbc(0x1b3); regmbc(0x232);
		      regmbc(0x24e); regmbc(0x1e8e); regmbc(0x1ef2);
		      regmbc(0x1ef4); regmbc(0x1ef6); regmbc(0x1ef8);
		      തിരികെ;
	    അവസ്ഥ 'Z': അവസ്ഥ 0x179: അവസ്ഥ 0x17b: അവസ്ഥ 0x17d: അവസ്ഥ 0x1b5:
	    അവസ്ഥ 0x1e90: അവസ്ഥ 0x1e92: അവസ്ഥ 0x1e94: അവസ്ഥ 0x2c6b:
		      regmbc('Z'); regmbc(0x179); regmbc(0x17b);
		      regmbc(0x17d); regmbc(0x1b5); regmbc(0x1e90);
		      regmbc(0x1e92); regmbc(0x1e94); regmbc(0x2c6b);
		      തിരികെ;
	    അവസ്ഥ 'a': അവസ്ഥ 0xe0: അവസ്ഥ 0xe1: അവസ്ഥ 0xe2:
	    അവസ്ഥ 0xe3: അവസ്ഥ 0xe4: അവസ്ഥ 0xe5: അവസ്ഥ 0x101: അവസ്ഥ 0x103:
	    അവസ്ഥ 0x105: അവസ്ഥ 0x1ce: അവസ്ഥ 0x1df: അവസ്ഥ 0x1e1: അവസ്ഥ 0x1fb:
	    അവസ്ഥ 0x201: അവസ്ഥ 0x203: അവസ്ഥ 0x227: അവസ്ഥ 0x1d8f: അവസ്ഥ 0x1e01:
	    അവസ്ഥ 0x1e9a: അവസ്ഥ 0x1ea1: അവസ്ഥ 0x1ea3: അവസ്ഥ 0x1ea5:
	    അവസ്ഥ 0x1ea7: അവസ്ഥ 0x1ea9: അവസ്ഥ 0x1eab: അവസ്ഥ 0x1ead:
	    അവസ്ഥ 0x1eaf: അവസ്ഥ 0x1eb1: അവസ്ഥ 0x1eb3: അവസ്ഥ 0x1eb5:
	    അവസ്ഥ 0x1eb7: അവസ്ഥ 0x2c65:
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
		      തിരികെ;
	    അവസ്ഥ 'b': അവസ്ഥ 0x180: അവസ്ഥ 0x253: അവസ്ഥ 0x1d6c: അവസ്ഥ 0x1d80:
	    അവസ്ഥ 0x1e03: അവസ്ഥ 0x1e05: അവസ്ഥ 0x1e07:
		      regmbc('b');
		      regmbc(0x180); regmbc(0x253); regmbc(0x1d6c);
		      regmbc(0x1d80); regmbc(0x1e03); regmbc(0x1e05);
		      regmbc(0x1e07);
		      തിരികെ;
	    അവസ്ഥ 'c': അവസ്ഥ 0xe7:
	    അവസ്ഥ 0x107: അവസ്ഥ 0x109: അവസ്ഥ 0x10b: അവസ്ഥ 0x10d: അവസ്ഥ 0x188:
	    അവസ്ഥ 0x23c: അവസ്ഥ 0x1e09: അവസ്ഥ 0xa793: അവസ്ഥ 0xa794:
		      regmbc('c'); regmbc(0xe7); regmbc(0x107);
		      regmbc(0x109); regmbc(0x10b); regmbc(0x10d);
		      regmbc(0x188); regmbc(0x23c); regmbc(0x1e09);
		      regmbc(0xa793); regmbc(0xa794);
		      തിരികെ;
	    അവസ്ഥ 'd': അവസ്ഥ 0x10f: അവസ്ഥ 0x111: അവസ്ഥ 0x257: അവസ്ഥ 0x1d6d:
	    അവസ്ഥ 0x1d81: അവസ്ഥ 0x1d91: അവസ്ഥ 0x1e0b: അവസ്ഥ 0x1e0d:
	    അവസ്ഥ 0x1e0f: അവസ്ഥ 0x1e11: അവസ്ഥ 0x1e13:
		      regmbc('d'); regmbc(0x10f); regmbc(0x111);
		      regmbc(0x257); regmbc(0x1d6d); regmbc(0x1d81);
		      regmbc(0x1d91); regmbc(0x1e0b); regmbc(0x1e0d);
		      regmbc(0x1e0f); regmbc(0x1e11); regmbc(0x1e13);
		      തിരികെ;
	    അവസ്ഥ 'e': അവസ്ഥ 0xe8: അവസ്ഥ 0xe9: അവസ്ഥ 0xea: അവസ്ഥ 0xeb:
	    അവസ്ഥ 0x113: അവസ്ഥ 0x115: അവസ്ഥ 0x117: അവസ്ഥ 0x119:
	    അവസ്ഥ 0x11b: അവസ്ഥ 0x205: അവസ്ഥ 0x207: അവസ്ഥ 0x229:
	    അവസ്ഥ 0x247: അവസ്ഥ 0x1d92: അവസ്ഥ 0x1e15: അവസ്ഥ 0x1e17:
	    അവസ്ഥ 0x1e19: അവസ്ഥ 0x1e1b: അവസ്ഥ 0x1eb9: അവസ്ഥ 0x1ebb:
	    അവസ്ഥ 0x1e1d: അവസ്ഥ 0x1ebd: അവസ്ഥ 0x1ebf: അവസ്ഥ 0x1ec1:
	    അവസ്ഥ 0x1ec3: അവസ്ഥ 0x1ec5: അവസ്ഥ 0x1ec7:
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
		      തിരികെ;
	    അവസ്ഥ 'f': അവസ്ഥ 0x192: അവസ്ഥ 0x1d6e: അവസ്ഥ 0x1d82:
	    അവസ്ഥ 0x1e1f: അവസ്ഥ 0xa799:
		     regmbc('f'); regmbc(0x192); regmbc(0x1d6e);
		     regmbc(0x1d82); regmbc(0x1e1f); regmbc(0xa799);
		     തിരികെ;
	    അവസ്ഥ 'g': അവസ്ഥ 0x11d: അവസ്ഥ 0x11f: അവസ്ഥ 0x121: അവസ്ഥ 0x123:
	    അവസ്ഥ 0x1e5: അവസ്ഥ 0x1e7: അവസ്ഥ 0x260: അവസ്ഥ 0x1f5: അവസ്ഥ 0x1d83:
	    അവസ്ഥ 0x1e21: അവസ്ഥ 0xa7a1:
		      regmbc('g'); regmbc(0x11d); regmbc(0x11f);
		      regmbc(0x121); regmbc(0x123); regmbc(0x1e5);
		      regmbc(0x1e7); regmbc(0x1f5); regmbc(0x260);
		      regmbc(0x1d83); regmbc(0x1e21); regmbc(0xa7a1);
		      തിരികെ;
	    അവസ്ഥ 'h': അവസ്ഥ 0x125: അവസ്ഥ 0x127: അവസ്ഥ 0x21f: അവസ്ഥ 0x1e23:
	    അവസ്ഥ 0x1e25: അവസ്ഥ 0x1e27: അവസ്ഥ 0x1e29: അവസ്ഥ 0x1e2b:
	    അവസ്ഥ 0x1e96: അവസ്ഥ 0x2c68: അവസ്ഥ 0xa795:
		      regmbc('h'); regmbc(0x125); regmbc(0x127);
		      regmbc(0x21f); regmbc(0x1e23); regmbc(0x1e25);
		      regmbc(0x1e27); regmbc(0x1e29); regmbc(0x1e2b);
		      regmbc(0x1e96); regmbc(0x2c68); regmbc(0xa795);
		      തിരികെ;
	    അവസ്ഥ 'i': അവസ്ഥ 0xec: അവസ്ഥ 0xed: അവസ്ഥ 0xee: അവസ്ഥ 0xef:
	    അവസ്ഥ 0x129: അവസ്ഥ 0x12b: അവസ്ഥ 0x12d: അവസ്ഥ 0x12f:
	    അവസ്ഥ 0x1d0: അവസ്ഥ 0x209: അവസ്ഥ 0x20b: അവസ്ഥ 0x268:
	    അവസ്ഥ 0x1d96: അവസ്ഥ 0x1e2d: അവസ്ഥ 0x1e2f: അവസ്ഥ 0x1ec9:
	    അവസ്ഥ 0x1ecb:
		      regmbc('i'); regmbc(0xec); regmbc(0xed);
		      regmbc(0xee); regmbc(0xef); regmbc(0x129);
		      regmbc(0x12b); regmbc(0x12d); regmbc(0x12f);
		      regmbc(0x1d0); regmbc(0x209); regmbc(0x20b);
		      regmbc(0x268); regmbc(0x1d96); regmbc(0x1e2d);
		      regmbc(0x1e2f); regmbc(0x1ec9); regmbc(0x1ecb);
		      തിരികെ;
	    അവസ്ഥ 'j': അവസ്ഥ 0x135: അവസ്ഥ 0x1f0: അവസ്ഥ 0x249:
		      regmbc('j'); regmbc(0x135); regmbc(0x1f0);
		      regmbc(0x249);
		      തിരികെ;
	    അവസ്ഥ 'k': അവസ്ഥ 0x137: അവസ്ഥ 0x199: അവസ്ഥ 0x1e9:
	    അവസ്ഥ 0x1d84: അവസ്ഥ 0x1e31: അവസ്ഥ 0x1e33: അവസ്ഥ 0x1e35:
	    അവസ്ഥ 0x2c6a: അവസ്ഥ 0xa741:
		      regmbc('k'); regmbc(0x137); regmbc(0x199);
		      regmbc(0x1e9); regmbc(0x1d84); regmbc(0x1e31);
		      regmbc(0x1e33); regmbc(0x1e35); regmbc(0x2c6a);
		      regmbc(0xa741);
		      തിരികെ;
	    അവസ്ഥ 'l': അവസ്ഥ 0x13a: അവസ്ഥ 0x13c: അവസ്ഥ 0x13e:
	    അവസ്ഥ 0x140: അവസ്ഥ 0x142: അവസ്ഥ 0x19a: അവസ്ഥ 0x1e37:
	    അവസ്ഥ 0x1e39: അവസ്ഥ 0x1e3b: അവസ്ഥ 0x1e3d: അവസ്ഥ 0x2c61:
		      regmbc('l'); regmbc(0x13a); regmbc(0x13c);
		      regmbc(0x13e); regmbc(0x140); regmbc(0x142);
		      regmbc(0x19a); regmbc(0x1e37); regmbc(0x1e39);
		      regmbc(0x1e3b); regmbc(0x1e3d); regmbc(0x2c61);
		      തിരികെ;
	    അവസ്ഥ 'm': അവസ്ഥ 0x1d6f: അവസ്ഥ 0x1e3f: അവസ്ഥ 0x1e41: അവസ്ഥ 0x1e43:
		      regmbc('m'); regmbc(0x1d6f); regmbc(0x1e3f);
		      regmbc(0x1e41); regmbc(0x1e43);
		      തിരികെ;
	    അവസ്ഥ 'n': അവസ്ഥ 0xf1: അവസ്ഥ 0x144: അവസ്ഥ 0x146: അവസ്ഥ 0x148:
	    അവസ്ഥ 0x149: അവസ്ഥ 0x1f9: അവസ്ഥ 0x1d70: അവസ്ഥ 0x1d87:
	    അവസ്ഥ 0x1e45: അവസ്ഥ 0x1e47: അവസ്ഥ 0x1e49: അവസ്ഥ 0x1e4b:
	    അവസ്ഥ 0xa7a5:
		      regmbc('n'); regmbc(0xf1); regmbc(0x144);
		      regmbc(0x146); regmbc(0x148); regmbc(0x149);
		      regmbc(0x1f9); regmbc(0x1d70); regmbc(0x1d87);
		      regmbc(0x1e45); regmbc(0x1e47); regmbc(0x1e49);
		      regmbc(0x1e4b); regmbc(0xa7a5);
		      തിരികെ;
	    അവസ്ഥ 'o': അവസ്ഥ 0xf2: അവസ്ഥ 0xf3: അവസ്ഥ 0xf4: അവസ്ഥ 0xf5:
	    അവസ്ഥ 0xf6: അവസ്ഥ 0xf8: അവസ്ഥ 0x14d: അവസ്ഥ 0x14f: അവസ്ഥ 0x151:
	    അവസ്ഥ 0x1a1: അവസ്ഥ 0x1d2: അവസ്ഥ 0x1eb: അവസ്ഥ 0x1ed: അവസ്ഥ 0x1ff:
	    അവസ്ഥ 0x20d: അവസ്ഥ 0x20f: അവസ്ഥ 0x22b: അവസ്ഥ 0x22d: അവസ്ഥ 0x22f:
	    അവസ്ഥ 0x231: അവസ്ഥ 0x275: അവസ്ഥ 0x1e4d: അവസ്ഥ 0x1e4f:
	    അവസ്ഥ 0x1e51: അവസ്ഥ 0x1e53: അവസ്ഥ 0x1ecd: അവസ്ഥ 0x1ecf:
	    അവസ്ഥ 0x1ed1: അവസ്ഥ 0x1ed3: അവസ്ഥ 0x1ed5: അവസ്ഥ 0x1ed7:
	    അവസ്ഥ 0x1ed9: അവസ്ഥ 0x1edb: അവസ്ഥ 0x1edd: അവസ്ഥ 0x1edf:
	    അവസ്ഥ 0x1ee1: അവസ്ഥ 0x1ee3:
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
		      തിരികെ;
	    അവസ്ഥ 'p': അവസ്ഥ 0x1a5: അവസ്ഥ 0x1d71: അവസ്ഥ 0x1d88: അവസ്ഥ 0x1d7d:
	    അവസ്ഥ 0x1e55: അവസ്ഥ 0x1e57:
		      regmbc('p'); regmbc(0x1a5); regmbc(0x1d71);
		      regmbc(0x1d7d); regmbc(0x1d88); regmbc(0x1e55);
		      regmbc(0x1e57);
		      തിരികെ;
	    അവസ്ഥ 'q': അവസ്ഥ 0x24b: അവസ്ഥ 0x2a0:
		      regmbc('q'); regmbc(0x24b); regmbc(0x2a0);
		      തിരികെ;
	    അവസ്ഥ 'r': അവസ്ഥ 0x155: അവസ്ഥ 0x157: അവസ്ഥ 0x159: അവസ്ഥ 0x211:
	    അവസ്ഥ 0x213: അവസ്ഥ 0x24d: അവസ്ഥ 0x27d: അവസ്ഥ 0x1d72: അവസ്ഥ 0x1d73:
	    അവസ്ഥ 0x1d89: അവസ്ഥ 0x1e59: അവസ്ഥ 0x1e5b: അവസ്ഥ 0x1e5d: അവസ്ഥ 0x1e5f:
	    അവസ്ഥ 0xa7a7:
		      regmbc('r'); regmbc(0x155); regmbc(0x157);
		      regmbc(0x159); regmbc(0x211); regmbc(0x213);
		      regmbc(0x24d); regmbc(0x1d72); regmbc(0x1d73);
		      regmbc(0x1d89); regmbc(0x1e59); regmbc(0x27d);
		      regmbc(0x1e5b); regmbc(0x1e5d); regmbc(0x1e5f);
		      regmbc(0xa7a7);
		      തിരികെ;
	    അവസ്ഥ 's': അവസ്ഥ 0x15b: അവസ്ഥ 0x15d: അവസ്ഥ 0x15f: അവസ്ഥ 0x161:
	    അവസ്ഥ 0x1e61: അവസ്ഥ 0x219: അവസ്ഥ 0x23f: അവസ്ഥ 0x1d74: അവസ്ഥ 0x1d8a:
	    അവസ്ഥ 0x1e63: അവസ്ഥ 0x1e65: അവസ്ഥ 0x1e67: അവസ്ഥ 0x1e69: അവസ്ഥ 0xa7a9:
		      regmbc('s'); regmbc(0x15b); regmbc(0x15d);
		      regmbc(0x15f); regmbc(0x161); regmbc(0x23f);
		      regmbc(0x219); regmbc(0x1d74); regmbc(0x1d8a);
		      regmbc(0x1e61); regmbc(0x1e63); regmbc(0x1e65);
		      regmbc(0x1e67); regmbc(0x1e69); regmbc(0xa7a9);
		      തിരികെ;
	    അവസ്ഥ 't': അവസ്ഥ 0x163: അവസ്ഥ 0x165: അവസ്ഥ 0x167: അവസ്ഥ 0x1ab:
	    അവസ്ഥ 0x1ad: അവസ്ഥ 0x21b: അവസ്ഥ 0x288: അവസ്ഥ 0x1d75: അവസ്ഥ 0x1e6b:
	    അവസ്ഥ 0x1e6d: അവസ്ഥ 0x1e6f: അവസ്ഥ 0x1e71: അവസ്ഥ 0x1e97: അവസ്ഥ 0x2c66:
		      regmbc('t'); regmbc(0x163); regmbc(0x165);
		      regmbc(0x167); regmbc(0x1ab); regmbc(0x21b);
		      regmbc(0x1ad); regmbc(0x288); regmbc(0x1d75);
		      regmbc(0x1e6b); regmbc(0x1e6d); regmbc(0x1e6f);
		      regmbc(0x1e71); regmbc(0x1e97); regmbc(0x2c66);
		      തിരികെ;
	    അവസ്ഥ 'u': അവസ്ഥ 0xf9: അവസ്ഥ 0xfa: അവസ്ഥ 0xfb: അവസ്ഥ 0xfc:
	    അവസ്ഥ 0x169: അവസ്ഥ 0x16b: അവസ്ഥ 0x16d: അവസ്ഥ 0x16f:
	    അവസ്ഥ 0x171: അവസ്ഥ 0x173: അവസ്ഥ 0x1b0: അവസ്ഥ 0x1d4:
	    അവസ്ഥ 0x1d6: അവസ്ഥ 0x1d8: അവസ്ഥ 0x1da: അവസ്ഥ 0x1dc:
	    അവസ്ഥ 0x215: അവസ്ഥ 0x217: അവസ്ഥ 0x289: അവസ്ഥ 0x1e73:
	    അവസ്ഥ 0x1d7e: അവസ്ഥ 0x1d99: അവസ്ഥ 0x1e75: അവസ്ഥ 0x1e77:
	    അവസ്ഥ 0x1e79: അവസ്ഥ 0x1e7b: അവസ്ഥ 0x1ee5: അവസ്ഥ 0x1ee7:
	    അവസ്ഥ 0x1ee9: അവസ്ഥ 0x1eeb: അവസ്ഥ 0x1eed: അവസ്ഥ 0x1eef:
	    അവസ്ഥ 0x1ef1:
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
		      തിരികെ;
	    അവസ്ഥ 'v': അവസ്ഥ 0x28b: അവസ്ഥ 0x1d8c: അവസ്ഥ 0x1e7d: അവസ്ഥ 0x1e7f:
		      regmbc('v'); regmbc(0x28b); regmbc(0x1d8c);
		      regmbc(0x1e7d); regmbc(0x1e7f);
		      തിരികെ;
	    അവസ്ഥ 'w': അവസ്ഥ 0x175: അവസ്ഥ 0x1e81: അവസ്ഥ 0x1e83:
	    അവസ്ഥ 0x1e85: അവസ്ഥ 0x1e87: അവസ്ഥ 0x1e89: അവസ്ഥ 0x1e98:
		      regmbc('w'); regmbc(0x175); regmbc(0x1e81);
		      regmbc(0x1e83); regmbc(0x1e85); regmbc(0x1e87);
		      regmbc(0x1e89); regmbc(0x1e98);
		      തിരികെ;
	    അവസ്ഥ 'x': അവസ്ഥ 0x1e8b: അവസ്ഥ 0x1e8d:
		      regmbc('x'); regmbc(0x1e8b); regmbc(0x1e8d);
		      തിരികെ;
	    അവസ്ഥ 'y': അവസ്ഥ 0xfd: അവസ്ഥ 0xff: അവസ്ഥ 0x177: അവസ്ഥ 0x1b4:
	    അവസ്ഥ 0x233: അവസ്ഥ 0x24f: അവസ്ഥ 0x1e8f: അവസ്ഥ 0x1e99: അവസ്ഥ 0x1ef3:
	    അവസ്ഥ 0x1ef5: അവസ്ഥ 0x1ef7: അവസ്ഥ 0x1ef9:
		      regmbc('y'); regmbc(0xfd); regmbc(0xff);
		      regmbc(0x177); regmbc(0x1b4); regmbc(0x233);
		      regmbc(0x24f); regmbc(0x1e8f); regmbc(0x1e99);
		      regmbc(0x1ef3); regmbc(0x1ef5); regmbc(0x1ef7);
		      regmbc(0x1ef9);
		      തിരികെ;
	    അവസ്ഥ 'z': അവസ്ഥ 0x17a: അവസ്ഥ 0x17c: അവസ്ഥ 0x17e: അവസ്ഥ 0x1b6:
	    അവസ്ഥ 0x1d76: അവസ്ഥ 0x1d8e: അവസ്ഥ 0x1e91: അവസ്ഥ 0x1e93:
	    അവസ്ഥ 0x1e95: അവസ്ഥ 0x2c6c:
		      regmbc('z'); regmbc(0x17a); regmbc(0x17c);
		      regmbc(0x17e); regmbc(0x1b6); regmbc(0x1d76);
		      regmbc(0x1d8e); regmbc(0x1e91); regmbc(0x1e93);
		      regmbc(0x1e95); regmbc(0x2c6c);
		      തിരികെ;
	}
    }
    regmbc(c);
}

/*
 * Emit a node.
 * Return pointer to generated code.
 */
    സ്ഥിതം char_u *
regnode(പൂർണ്ണസംഖ്യ op)
{
    char_u  *ret;

    ret = regcode;
    എങ്കിൽ (ret == JUST_CALC_SIZE)
	regsize += 3;
    അല്ലെങ്കിൽ
    {
	*regcode++ = op;
	*regcode++ = NUL;		// Null "next" pointer.
	*regcode++ = NUL;
    }
    തിരികെ ret;
}

/*
 * Write a long as four bytes at "p" and return pointer to the next char.
 */
    സ്ഥിതം char_u *
re_put_long(char_u *p, long_u val)
{
    *p++ = (char_u) ((val >> 24) & 0377);
    *p++ = (char_u) ((val >> 16) & 0377);
    *p++ = (char_u) ((val >> 8) & 0377);
    *p++ = (char_u) (val & 0377);
    തിരികെ p;
}

/*
 * regnext - dig the "next" pointer out of a node
 * Returns NULL when calculating size, when there is no next item and when
 * there is an error.
 */
    സ്ഥിതം char_u *
regnext(char_u *p)
{
    പൂർണ്ണസംഖ്യ	    offset;

    എങ്കിൽ (p == JUST_CALC_SIZE || reg_toolong)
	തിരികെ ശൂന്യ;

    offset = NEXT(p);
    എങ്കിൽ (offset == 0)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (OP(p) == BACK)
	തിരികെ p - offset;
    അല്ലെങ്കിൽ
	തിരികെ p + offset;
}

/*
 * Set the next-pointer at the end of a node chain.
 */
    സ്ഥിതം ശൂന്യം
regtail(char_u *p, char_u *val)
{
    char_u	*scan;
    char_u	*temp;
    പൂർണ്ണസംഖ്യ		offset;

    എങ്കിൽ (p == JUST_CALC_SIZE)
	തിരികെ;

    // Find last node.
    scan = p;
    വേണ്ടി (;;)
    {
	temp = regnext(scan);
	എങ്കിൽ (temp == ശൂന്യ)
	    നിർത്തുക;
	scan = temp;
    }

    എങ്കിൽ (OP(scan) == BACK)
	offset = (പൂർണ്ണസംഖ്യ)(scan - val);
    അല്ലെങ്കിൽ
	offset = (പൂർണ്ണസംഖ്യ)(val - scan);
    // When the offset uses more than 16 bits it can no longer fit in the two
    // bytes available.  Use a global flag to avoid having to check return
    // values in too many places.
    എങ്കിൽ (offset > 0xffff)
	reg_toolong = TRUE;
    അല്ലെങ്കിൽ
    {
	*(scan + 1) = (char_u) (((unsigned)offset >> 8) & 0377);
	*(scan + 2) = (char_u) (offset & 0377);
    }
}

/*
 * Like regtail, on item after a BRANCH; nop if none.
 */
    സ്ഥിതം ശൂന്യം
regoptail(char_u *p, char_u *val)
{
    // When op is neither BRANCH nor BRACE_COMPLEX0-9, it is "operandless"
    എങ്കിൽ (p == ശൂന്യ || p == JUST_CALC_SIZE
	    || (OP(p) != BRANCH
		&& (OP(p) < BRACE_COMPLEX || OP(p) > BRACE_COMPLEX + 9)))
	തിരികെ;
    regtail(OPERAND(p), val);
}

/*
 * Insert an operator in front of already-emitted operand
 *
 * Means relocating the operand.
 */
    സ്ഥിതം ശൂന്യം
reginsert(പൂർണ്ണസംഖ്യ op, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    എങ്കിൽ (regcode == JUST_CALC_SIZE)
    {
	regsize += 3;
	തിരികെ;
    }
    src = regcode;
    regcode += 3;
    dst = regcode;
    ആകുമ്പോൾ (src > opnd)
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
    സ്ഥിതം ശൂന്യം
reginsert_nr(പൂർണ്ണസംഖ്യ op, long val, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    എങ്കിൽ (regcode == JUST_CALC_SIZE)
    {
	regsize += 7;
	തിരികെ;
    }
    src = regcode;
    regcode += 7;
    dst = regcode;
    ആകുമ്പോൾ (src > opnd)
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
    സ്ഥിതം ശൂന്യം
reginsert_limits(
    പൂർണ്ണസംഖ്യ		op,
    long	minval,
    long	maxval,
    char_u	*opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    എങ്കിൽ (regcode == JUST_CALC_SIZE)
    {
	regsize += 11;
	തിരികെ;
    }
    src = regcode;
    regcode += 11;
    dst = regcode;
    ആകുമ്പോൾ (src > opnd)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
seen_endbrace(പൂർണ്ണസംഖ്യ refnum)
{
    എങ്കിൽ (!had_endbrace[refnum])
    {
	char_u *p;

	// Trick: check if "@<=" or "@<!" follows, in which case
	// the \1 can appear before the referenced match.
	വേണ്ടി (p = regparse; *p != NUL; ++p)
	    എങ്കിൽ (p[0] == '@' && p[1] == '<' && (p[2] == '!' || p[2] == '='))
		നിർത്തുക;
	എങ്കിൽ (*p == NUL)
	{
	    emsg(_(e_illegal_back_reference));
	    rc_did_emsg = TRUE;
	    തിരികെ FALSE;
	}
    }
    തിരികെ TRUE;
}

/*
 * Parse the lowest level.
 *
 * Optimization:  gobbles an entire sequence of ordinary characters so that
 * it can turn them into a single node, which is smaller to store and
 * faster to run.  Don't do this when one_exactly is set.
 */
    സ്ഥിതം char_u *
regatom(പൂർണ്ണസംഖ്യ *flagp)
{
    char_u	    *ret;
    പൂർണ്ണസംഖ്യ		    flags;
    പൂർണ്ണസംഖ്യ		    c;
    char_u	    *p;
    പൂർണ്ണസംഖ്യ		    extra = 0;
    പൂർണ്ണസംഖ്യ		    save_prev_at_start = prev_at_start;

    *flagp = WORST;		// Tentatively.

    c = getchr();
    തിരഞ്ഞെടുപ്പ് (c)
    {
      അവസ്ഥ Magic('^'):
	ret = regnode(BOL);
	നിർത്തുക;

      അവസ്ഥ Magic('$'):
	ret = regnode(EOL);
#എങ്കിൽ defined(FEAT_SYN_HL)
	had_eol = TRUE;
#endif
	നിർത്തുക;

      അവസ്ഥ Magic('<'):
	ret = regnode(BOW);
	നിർത്തുക;

      അവസ്ഥ Magic('>'):
	ret = regnode(EOW);
	നിർത്തുക;

      അവസ്ഥ Magic('_'):
	c = no_Magic(getchr());
	എങ്കിൽ (c == '^')		// "\_^" is start-of-line
	{
	    ret = regnode(BOL);
	    നിർത്തുക;
	}
	എങ്കിൽ (c == '$')		// "\_$" is end-of-line
	{
	    ret = regnode(EOL);
#എങ്കിൽ defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#endif
	    നിർത്തുക;
	}

	extra = ADD_NL;
	*flagp |= HASNL;

	// "\_[" is character range plus newline
	എങ്കിൽ (c == '[')
	    പോകുക collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	// Character classes.
      അവസ്ഥ Magic('.'):
      അവസ്ഥ Magic('i'):
      അവസ്ഥ Magic('I'):
      അവസ്ഥ Magic('k'):
      അവസ്ഥ Magic('K'):
      അവസ്ഥ Magic('f'):
      അവസ്ഥ Magic('F'):
      അവസ്ഥ Magic('p'):
      അവസ്ഥ Magic('P'):
      അവസ്ഥ Magic('s'):
      അവസ്ഥ Magic('S'):
      അവസ്ഥ Magic('d'):
      അവസ്ഥ Magic('D'):
      അവസ്ഥ Magic('x'):
      അവസ്ഥ Magic('X'):
      അവസ്ഥ Magic('o'):
      അവസ്ഥ Magic('O'):
      അവസ്ഥ Magic('w'):
      അവസ്ഥ Magic('W'):
      അവസ്ഥ Magic('h'):
      അവസ്ഥ Magic('H'):
      അവസ്ഥ Magic('a'):
      അവസ്ഥ Magic('A'):
      അവസ്ഥ Magic('l'):
      അവസ്ഥ Magic('L'):
      അവസ്ഥ Magic('u'):
      അവസ്ഥ Magic('U'):
	p = vim_strchr(classchars, no_Magic(c));
	എങ്കിൽ (p == ശൂന്യ)
	    EMSG_RET_NULL(_(e_invalid_use_of_underscore));

	// When '.' is followed by a composing char ignore the dot, so that
	// the composing char is matched here.
	എങ്കിൽ (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	{
	    c = getchr();
	    പോകുക do_multibyte;
	}
	ret = regnode(classcodes[p - classchars] + extra);
	*flagp |= HASWIDTH | SIMPLE;
	നിർത്തുക;

      അവസ്ഥ Magic('n'):
	എങ്കിൽ (reg_string)
	{
	    // In a string "\n" matches a newline character.
	    ret = regnode(EXACTLY);
	    regc(NL);
	    regc(NUL);
	    *flagp |= HASWIDTH | SIMPLE;
	}
	അല്ലെങ്കിൽ
	{
	    // In buffer text "\n" matches the end of a line.
	    ret = regnode(NEWL);
	    *flagp |= HASWIDTH | HASNL;
	}
	നിർത്തുക;

      അവസ്ഥ Magic('('):
	എങ്കിൽ (one_exactly)
	    EMSG_ONE_RET_NULL;
	ret = reg(REG_PAREN, &flags);
	എങ്കിൽ (ret == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	*flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
	നിർത്തുക;

      അവസ്ഥ NUL:
      അവസ്ഥ Magic('|'):
      അവസ്ഥ Magic('&'):
      അവസ്ഥ Magic(')'):
	എങ്കിൽ (one_exactly)
	    EMSG_ONE_RET_NULL;
	// Supposed to be caught earlier.
	IEMSG_RET_NULL(e_internal_error_in_regexp);
	// NOTREACHED

      അവസ്ഥ Magic('='):
      അവസ്ഥ Magic('?'):
      അവസ്ഥ Magic('+'):
      അവസ്ഥ Magic('@'):
      അവസ്ഥ Magic('{'):
      അവസ്ഥ Magic('*'):
	c = no_Magic(c);
	EMSG3_RET_NULL(_(e_str_chr_follows_nothing),
		(c == '*' ? reg_magic >= MAGIC_ON : reg_magic == MAGIC_ALL), c);
	// NOTREACHED

      അവസ്ഥ Magic('~'):		// previous substitute pattern
	    എങ്കിൽ (reg_prev_sub != ശൂന്യ)
	    {
		char_u	    *lp;

		ret = regnode(EXACTLY);
		lp = reg_prev_sub;
		ആകുമ്പോൾ (*lp != NUL)
		    regc(*lp++);
		regc(NUL);
		എങ്കിൽ (*reg_prev_sub != NUL)
		{
		    *flagp |= HASWIDTH;
		    എങ്കിൽ ((lp - reg_prev_sub) == 1)
			*flagp |= SIMPLE;
		}
	    }
	    അല്ലെങ്കിൽ
		EMSG_RET_NULL(_(e_no_previous_substitute_regular_expression));
	    നിർത്തുക;

      അവസ്ഥ Magic('1'):
      അവസ്ഥ Magic('2'):
      അവസ്ഥ Magic('3'):
      അവസ്ഥ Magic('4'):
      അവസ്ഥ Magic('5'):
      അവസ്ഥ Magic('6'):
      അവസ്ഥ Magic('7'):
      അവസ്ഥ Magic('8'):
      അവസ്ഥ Magic('9'):
	    {
		പൂർണ്ണസംഖ്യ		    refnum;

		refnum = c - Magic('0');
		എങ്കിൽ (!seen_endbrace(refnum))
		    തിരികെ ശൂന്യ;
		ret = regnode(BACKREF + refnum);
	    }
	    നിർത്തുക;

      അവസ്ഥ Magic('z'):
	{
	    c = no_Magic(getchr());
	    തിരഞ്ഞെടുപ്പ് (c)
	    {
#ifdef FEAT_SYN_HL
		അവസ്ഥ '(': എങ്കിൽ ((reg_do_extmatch & REX_SET) == 0)
			      EMSG_RET_NULL(_(e_z_not_allowed_here));
			  എങ്കിൽ (one_exactly)
			      EMSG_ONE_RET_NULL;
			  ret = reg(REG_ZPAREN, &flags);
			  എങ്കിൽ (ret == ശൂന്യ)
			      തിരികെ ശൂന്യ;
			  *flagp |= flags & (HASWIDTH|SPSTART|HASNL|HASLOOKBH);
			  re_has_z = REX_SET;
			  നിർത്തുക;

		അവസ്ഥ '1':
		അവസ്ഥ '2':
		അവസ്ഥ '3':
		അവസ്ഥ '4':
		അവസ്ഥ '5':
		അവസ്ഥ '6':
		അവസ്ഥ '7':
		അവസ്ഥ '8':
		അവസ്ഥ '9': എങ്കിൽ ((reg_do_extmatch & REX_USE) == 0)
			      EMSG_RET_NULL(_(e_z1_z9_not_allowed_here));
			  ret = regnode(ZREF + c - '0');
			  re_has_z = REX_USE;
			  നിർത്തുക;
#endif

		അവസ്ഥ 's': ret = regnode(MOPEN + 0);
			  എങ്കിൽ (re_mult_next("\\zs") == FAIL)
			      തിരികെ ശൂന്യ;
			  നിർത്തുക;

		അവസ്ഥ 'e': ret = regnode(MCLOSE + 0);
			  എങ്കിൽ (re_mult_next("\\ze") == FAIL)
			      തിരികെ ശൂന്യ;
			  നിർത്തുക;

		സ്ഥിരസ്ഥിതി:  EMSG_RET_NULL(_(e_invalid_character_after_bsl_z));
	    }
	}
	നിർത്തുക;

      അവസ്ഥ Magic('%'):
	{
	    c = no_Magic(getchr());
	    തിരഞ്ഞെടുപ്പ് (c)
	    {
		// () without a back reference
		അവസ്ഥ '(':
		    എങ്കിൽ (one_exactly)
			EMSG_ONE_RET_NULL;
		    ret = reg(REG_NPAREN, &flags);
		    എങ്കിൽ (ret == ശൂന്യ)
			തിരികെ ശൂന്യ;
		    *flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
		    നിർത്തുക;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		അവസ്ഥ '^':
		    ret = regnode(RE_BOF);
		    നിർത്തുക;

		അവസ്ഥ '$':
		    ret = regnode(RE_EOF);
		    നിർത്തുക;

		അവസ്ഥ '#':
		    എങ്കിൽ (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_start_of_pattern),
								  regparse[1]);
			തിരികെ FAIL;
		    }
		    ret = regnode(CURSOR);
		    നിർത്തുക;

		അവസ്ഥ 'V':
		    ret = regnode(RE_VISUAL);
		    നിർത്തുക;

		അവസ്ഥ 'C':
		    ret = regnode(RE_COMPOSING);
		    നിർത്തുക;

		// \%[abc]: Emit as a list of branches, all ending at the last
		// branch which matches nothing.
		അവസ്ഥ '[':
			  എങ്കിൽ (one_exactly)	// doesn't nest
			      EMSG_ONE_RET_NULL;
			  {
			      char_u	*lastbranch;
			      char_u	*lastnode = ശൂന്യ;
			      char_u	*br;

			      ret = ശൂന്യ;
			      ആകുമ്പോൾ ((c = getchr()) != ']')
			      {
				  എങ്കിൽ (c == NUL)
				      EMSG2_RET_NULL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
				  br = regnode(BRANCH);
				  എങ്കിൽ (ret == ശൂന്യ)
				      ret = br;
				  അല്ലെങ്കിൽ
				  {
				      regtail(lastnode, br);
				      എങ്കിൽ (reg_toolong)
					  തിരികെ ശൂന്യ;
				  }

				  ungetchr();
				  one_exactly = TRUE;
				  lastnode = regatom(flagp);
				  one_exactly = FALSE;
				  എങ്കിൽ (lastnode == ശൂന്യ)
				      തിരികെ ശൂന്യ;
			      }
			      എങ്കിൽ (ret == ശൂന്യ)
				  EMSG2_RET_NULL(_(e_empty_str_brackets),
						      reg_magic == MAGIC_ALL);
			      lastbranch = regnode(BRANCH);
			      br = regnode(NOTHING);
			      എങ്കിൽ (ret != JUST_CALC_SIZE)
			      {
				  regtail(lastnode, br);
				  regtail(lastbranch, br);
				  // connect all branches to the NOTHING
				  // branch at the end
				  വേണ്ടി (br = ret; br != lastnode; )
				  {
				      എങ്കിൽ (OP(br) == BRANCH)
				      {
					  regtail(br, lastbranch);
					  എങ്കിൽ (reg_toolong)
					      തിരികെ ശൂന്യ;
					  br = OPERAND(br);
				      }
				      അല്ലെങ്കിൽ
					  br = regnext(br);
				  }
			      }
			      *flagp &= ~(HASWIDTH | SIMPLE);
			      നിർത്തുക;
			  }

		അവസ്ഥ 'd':   // %d123 decimal
		അവസ്ഥ 'o':   // %o123 octal
		അവസ്ഥ 'x':   // %xab hex 2
		അവസ്ഥ 'u':   // %uabcd hex 4
		അവസ്ഥ 'U':   // %U1234abcd hex 8
			  {
			      vimlong_T i;

			      തിരഞ്ഞെടുപ്പ് (c)
			      {
				  അവസ്ഥ 'd': i = getdecchrs(); നിർത്തുക;
				  അവസ്ഥ 'o': i = getoctchrs(); നിർത്തുക;
				  അവസ്ഥ 'x': i = gethexchrs(2); നിർത്തുക;
				  അവസ്ഥ 'u': i = gethexchrs(4); നിർത്തുക;
				  അവസ്ഥ 'U': i = gethexchrs(8); നിർത്തുക;
				  സ്ഥിരസ്ഥിതി:  i = -1; നിർത്തുക;
			      }

			      എങ്കിൽ (i < 0 || i > INT_MAX)
				  EMSG2_RET_NULL(
					    _(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			      എങ്കിൽ (use_multibytecode(i))
				  ret = regnode(MULTIBYTECODE);
			      അല്ലെങ്കിൽ
				  ret = regnode(EXACTLY);
			      എങ്കിൽ (i == 0)
				  regc(0x0a);
			      അല്ലെങ്കിൽ
				  regmbc((പൂർണ്ണസംഖ്യ)i);
			      regc(NUL);
			      *flagp |= HASWIDTH;
			      നിർത്തുക;
			  }

		സ്ഥിരസ്ഥിതി:
			  എങ്കിൽ (VIM_ISDIGIT(c) || c == '<' || c == '>'
						|| c == '\'' || c == '.')
			  {
			      long_u	n = 0;
			      പൂർണ്ണസംഖ്യ	cmp;
			      പൂർണ്ണസംഖ്യ	cur = FALSE;
			      പൂർണ്ണസംഖ്യ	got_digit = FALSE;

			      cmp = c;
			      എങ്കിൽ (cmp == '<' || cmp == '>')
				  c = getchr();
			      എങ്കിൽ (no_Magic(c) == '.')
			      {
				  cur = TRUE;
				  c = getchr();
			      }
			      ആകുമ്പോൾ (VIM_ISDIGIT(c))
			      {
				  got_digit = TRUE;
				  n = n * 10 + (c - '0');
				  c = getchr();
			      }
			      എങ്കിൽ (no_Magic(c) == '\'' && n == 0)
			      {
				  // "\%'m", "\%<'m" and "\%>'m": Mark
				  c = getchr();
				  ret = regnode(RE_MARK);
				  എങ്കിൽ (ret == JUST_CALC_SIZE)
				      regsize += 2;
				  അല്ലെങ്കിൽ
				  {
				      *regcode++ = c;
				      *regcode++ = cmp;
				  }
				  നിർത്തുക;
			      }
			      അല്ലെങ്കിൽ എങ്കിൽ ((c == 'l' || c == 'c' || c == 'v')
					  && (cur || got_digit))
			      {
				  എങ്കിൽ (cur && n)
				  {
				    semsg(_(e_regexp_സംഖ്യ_after_dot_pos_തിരയൽ_chr),
								  no_Magic(c));
				    rc_did_emsg = TRUE;
				    തിരികെ ശൂന്യ;
				  }
				  എങ്കിൽ (c == 'l')
				  {
				      എങ്കിൽ (cur)
					  n = curwin->w_cursor.lnum;
				      ret = regnode(RE_LNUM);
				      എങ്കിൽ (save_prev_at_start)
					  at_start = TRUE;
				  }
				  അല്ലെങ്കിൽ എങ്കിൽ (c == 'c')
				  {
				      എങ്കിൽ (cur)
				      {
					  n = curwin->w_cursor.col;
					  n++;
				      }
				      ret = regnode(RE_COL);
				  }
				  അല്ലെങ്കിൽ
				  {
				      എങ്കിൽ (cur)
				      {
					  colnr_T vcol = 0;

					  getvvcol(curwin, &curwin->w_cursor,
							 ശൂന്യ, ശൂന്യ, &vcol, 0);
					  ++vcol;
					  n = vcol;
				      }
				      ret = regnode(RE_VCOL);
				  }
				  എങ്കിൽ (ret == JUST_CALC_SIZE)
				      regsize += 5;
				  അല്ലെങ്കിൽ
				  {
				      // put the number and the optional
				      // comparator after the opcode
				      regcode = re_put_long(regcode, n);
				      *regcode++ = cmp;
				  }
				  നിർത്തുക;
			      }
			  }

			  EMSG2_RET_NULL(_(e_invalid_character_after_str),
						      reg_magic == MAGIC_ALL);
	    }
	}
	നിർത്തുക;

      അവസ്ഥ Magic('['):
collection:
	{
	    char_u	*lp;

	    // If there is no matching ']', we assume the '[' is a normal
	    // character.  This makes 'incsearch' and ":help [" work.
	    lp = skip_anyof(regparse);
	    എങ്കിൽ (*lp == ']')	// there is a matching ']'
	    {
		പൂർണ്ണസംഖ്യ	startc = -1;	// > 0 when next '-' is a range
		പൂർണ്ണസംഖ്യ	endc;

		എങ്കിൽ (extra == 0)
		{
		    പൂർണ്ണസംഖ്യ	cl = bt_recognize_char_class(regparse, lp);

		    എങ്കിൽ (cl != 0)
		    {
			ret = regnode(cl);
			regparse = lp;
			skipchr();
			*flagp |= HASWIDTH | SIMPLE;
			നിർത്തുക;
		    }
		}

		// In a character class, different parsing rules apply.
		// Not even \ is special anymore, nothing is.
		എങ്കിൽ (*regparse == '^')	    // Complement of range.
		{
		    ret = regnode(ANYBUT + extra);
		    regparse++;
		}
		അല്ലെങ്കിൽ
		    ret = regnode(ANYOF + extra);

		// At the start ']' and '-' mean the literal character.
		എങ്കിൽ (*regparse == ']' || *regparse == '-')
		{
		    startc = *regparse;
		    regc(*regparse++);
		}

		ആകുമ്പോൾ (*regparse != NUL && *regparse != ']')
		{
		    എങ്കിൽ (*regparse == '-')
		    {
			++regparse;
			// The '-' is not used for a range at the end and
			// after or before a '\n'.
			എങ്കിൽ (*regparse == ']' || *regparse == NUL
				|| startc == -1
				|| (regparse[0] == '\\' && regparse[1] == 'n'))
			{
			    regc('-');
			    startc = '-';	// [--x] is a range
			}
			അല്ലെങ്കിൽ
			{
			    // Also accept "a-[.z.]"
			    endc = 0;
			    എങ്കിൽ (*regparse == '[')
				endc = get_coll_element(&regparse);
			    എങ്കിൽ (endc == 0)
			    {
				എങ്കിൽ (has_mbyte)
				    endc = mb_ptr2char_adv(&regparse);
				അല്ലെങ്കിൽ
				    endc = *regparse++;
			    }

			    // Handle \o40, \x20 and \u20AC style sequences
			    എങ്കിൽ (endc == '\\' && !reg_cpo_lit && !reg_cpo_bsl)
				endc = coll_get_char();

			    എങ്കിൽ (startc > endc)
				EMSG_RET_NULL(_(e_reverse_range_in_character_class));
			    എങ്കിൽ (has_mbyte && ((*mb_char2len)(startc) > 1
						 || (*mb_char2len)(endc) > 1))
			    {
				// Limit to a range of 256 chars.
				എങ്കിൽ (endc > startc + 256)
				    EMSG_RET_NULL(_(e_range_too_large_in_character_class));
				ആകുമ്പോൾ (++startc <= endc)
				    regmbc(startc);
			    }
			    അല്ലെങ്കിൽ
			    {
				ആകുമ്പോൾ (++startc <= endc)
				    regc(startc);
			    }
			    startc = -1;
			}
		    }
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    അല്ലെങ്കിൽ എങ്കിൽ (*regparse == '\\'
			    && !reg_cpo_bsl
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != ശൂന്യ
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR,
						       regparse[1]) != ശൂന്യ)))
		    {
			regparse++;
			എങ്കിൽ (*regparse == 'n')
			{
			    // '\n' in range: also match NL
			    എങ്കിൽ (ret != JUST_CALC_SIZE)
			    {
				// Using \n inside [^] does not change what
				// matches. "[^\n]" is the same as ".".
				എങ്കിൽ (*ret == ANYOF)
				{
				    *ret = ANYOF + ADD_NL;
				    *flagp |= HASNL;
				}
				// else: must have had a \n already
			    }
			    regparse++;
			    startc = -1;
			}
			അല്ലെങ്കിൽ എങ്കിൽ (*regparse == 'd'
				|| *regparse == 'o'
				|| *regparse == 'x'
				|| *regparse == 'u'
				|| *regparse == 'U')
			{
			    startc = coll_get_char();
			    // max UTF-8 Codepoint is U+10FFFF,
			    // but allow values until INT_MAX
			    എങ്കിൽ (startc == INT_MAX)
				EMSG_RET_NULL(_(e_unicode_val_too_large));
			    എങ്കിൽ (startc == 0)
				regc(0x0a);
			    അല്ലെങ്കിൽ
				regmbc(startc);
			}
			അല്ലെങ്കിൽ
			{
			    startc = backslash_trans(*regparse++);
			    regc(startc);
			}
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (*regparse == '[')
		    {
			പൂർണ്ണസംഖ്യ c_class;
			പൂർണ്ണസംഖ്യ cu;

			c_class = get_char_class(&regparse);
			startc = -1;
			// Characters assumed to be 8 bits!
			തിരഞ്ഞെടുപ്പ് (c_class)
			{
			    അവസ്ഥ CLASS_NONE:
				c_class = get_equi_class(&regparse);
				എങ്കിൽ (c_class != 0)
				{
				    // produce equivalence class
				    reg_equi_class(c_class);
				}
				അല്ലെങ്കിൽ എങ്കിൽ ((c_class =
					    get_coll_element(&regparse)) != 0)
				{
				    // produce a collating element
				    regmbc(c_class);
				}
				അല്ലെങ്കിൽ
				{
				    // literal '[', allow [[-x] as a range
				    startc = *regparse++;
				    regc(startc);
				}
				നിർത്തുക;
			    അവസ്ഥ CLASS_ALNUM:
				വേണ്ടി (cu = 1; cu < 128; cu++)
				    എങ്കിൽ (isalnum(cu))
					regmbc(cu);
				നിർത്തുക;
			    അവസ്ഥ CLASS_ALPHA:
				വേണ്ടി (cu = 1; cu < 128; cu++)
				    എങ്കിൽ (isalpha(cu))
					regmbc(cu);
				നിർത്തുക;
			    അവസ്ഥ CLASS_BLANK:
				regc(' ');
				regc('\t');
				നിർത്തുക;
			    അവസ്ഥ CLASS_CNTRL:
				വേണ്ടി (cu = 1; cu <= 127; cu++)
				    എങ്കിൽ (iscntrl(cu))
					regmbc(cu);
				നിർത്തുക;
			    അവസ്ഥ CLASS_DIGIT:
				വേണ്ടി (cu = 1; cu <= 127; cu++)
				    എങ്കിൽ (VIM_ISDIGIT(cu))
					regmbc(cu);
				നിർത്തുക;
			    അവസ്ഥ CLASS_GRAPH:
				വേണ്ടി (cu = 1; cu <= 127; cu++)
				    എങ്കിൽ (isgraph(cu))
					regmbc(cu);
				നിർത്തുക;
			    അവസ്ഥ CLASS_LOWER:
				വേണ്ടി (cu = 1; cu <= 255; cu++)
				    എങ്കിൽ (MB_ISLOWER(cu) && cu != 170
								 && cu != 186)
					regmbc(cu);
				നിർത്തുക;
			    അവസ്ഥ CLASS_PRINT:
				വേണ്ടി (cu = 1; cu <= 255; cu++)
				    എങ്കിൽ (vim_isprintc(cu))
					regmbc(cu);
				നിർത്തുക;
			    അവസ്ഥ CLASS_PUNCT:
				വേണ്ടി (cu = 1; cu < 128; cu++)
				    എങ്കിൽ (ispunct(cu))
					regmbc(cu);
				നിർത്തുക;
			    അവസ്ഥ CLASS_SPACE:
				വേണ്ടി (cu = 9; cu <= 13; cu++)
				    regc(cu);
				regc(' ');
				നിർത്തുക;
			    അവസ്ഥ CLASS_UPPER:
				വേണ്ടി (cu = 1; cu <= 255; cu++)
				    എങ്കിൽ (MB_ISUPPER(cu))
					regmbc(cu);
				നിർത്തുക;
			    അവസ്ഥ CLASS_XDIGIT:
				വേണ്ടി (cu = 1; cu <= 255; cu++)
				    എങ്കിൽ (vim_isxdigit(cu))
					regmbc(cu);
				നിർത്തുക;
			    അവസ്ഥ CLASS_TAB:
				regc('\t');
				നിർത്തുക;
			    അവസ്ഥ CLASS_RETURN:
				regc('\r');
				നിർത്തുക;
			    അവസ്ഥ CLASS_BACKSPACE:
				regc('\b');
				നിർത്തുക;
			    അവസ്ഥ CLASS_ESCAPE:
				regc('\033');
				നിർത്തുക;
			    അവസ്ഥ CLASS_IDENT:
				വേണ്ടി (cu = 1; cu <= 255; cu++)
				    എങ്കിൽ (vim_isIDc(cu))
					regmbc(cu);
				നിർത്തുക;
			    അവസ്ഥ CLASS_KEYWORD:
				വേണ്ടി (cu = 1; cu <= 255; cu++)
				    എങ്കിൽ (reg_iswordc(cu))
					regmbc(cu);
				നിർത്തുക;
			    അവസ്ഥ CLASS_FNAME:
				വേണ്ടി (cu = 1; cu <= 255; cu++)
				    എങ്കിൽ (vim_isfilec(cu))
					regmbc(cu);
				നിർത്തുക;
			}
		    }
		    അല്ലെങ്കിൽ
		    {
			എങ്കിൽ (has_mbyte)
			{
			    പൂർണ്ണസംഖ്യ	len;

			    // produce a multibyte character, including any
			    // following composing characters
			    startc = mb_ptr2char(regparse);
			    len = (*mb_ptr2len)(regparse);
			    എങ്കിൽ (enc_utf8 && utf_char2len(startc) != len)
				startc = -1;	// composing chars
			    ആകുമ്പോൾ (--len >= 0)
				regc(*regparse++);
			}
			അല്ലെങ്കിൽ
			{
			    startc = *regparse++;
			    regc(startc);
			}
		    }
		}
		regc(NUL);
		prevchr_len = 1;	// last char was the ']'
		എങ്കിൽ (*regparse != ']')
		    EMSG_RET_NULL(_(e_too_many_brackets));  // Cannot happen?
		skipchr();	    // let's be friends with the lexer again
		*flagp |= HASWIDTH | SIMPLE;
		നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (reg_strict)
		EMSG2_RET_NULL(_(e_missing_rsb_after_str_lsb),
							reg_magic > MAGIC_OFF);
	}
	// FALLTHROUGH

      സ്ഥിരസ്ഥിതി:
	{
	    പൂർണ്ണസംഖ്യ		len;

	    // A multi-byte character is handled as a separate atom if it's
	    // before a multi and when it's a composing char.
	    എങ്കിൽ (use_multibytecode(c))
	    {
do_multibyte:
		ret = regnode(MULTIBYTECODE);
		regmbc(c);
		*flagp |= HASWIDTH | SIMPLE;
		നിർത്തുക;
	    }

	    ret = regnode(EXACTLY);

	    // Append characters as long as:
	    // - there is no following multi, we then need the character in
	    //   front of it as a single character operand
	    // - not running into a Magic character
	    // - "one_exactly" is not set
	    // But always emit at least one character.  Might be a Multi,
	    // e.g., a "[" without matching "]".
	    വേണ്ടി (len = 0; c != NUL && (len == 0
			|| (re_multi_type(peekchr()) == NOT_MULTI
			    && !one_exactly
			    && !is_Magic(c))); ++len)
	    {
		c = no_Magic(c);
		എങ്കിൽ (has_mbyte)
		{
		    regmbc(c);
		    എങ്കിൽ (enc_utf8)
		    {
			പൂർണ്ണസംഖ്യ	l;

			// Need to get composing character too.
			വേണ്ടി (;;)
			{
			    l = utf_ptr2len(regparse);
			    എങ്കിൽ (!UTF_COMPOSINGLIKE(regparse, regparse + l))
				നിർത്തുക;
			    regmbc(utf_ptr2char(regparse));
			    skipchr();
			}
		    }
		}
		അല്ലെങ്കിൽ
		    regc(c);
		c = getchr();
	    }
	    ungetchr();

	    regc(NUL);
	    *flagp |= HASWIDTH;
	    എങ്കിൽ (len == 1)
		*flagp |= SIMPLE;
	}
	നിർത്തുക;
    }

    തിരികെ ret;
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
    സ്ഥിതം char_u *
regpiece(പൂർണ്ണസംഖ്യ *flagp)
{
    char_u	    *ret;
    പൂർണ്ണസംഖ്യ		    op;
    char_u	    *next;
    പൂർണ്ണസംഖ്യ		    flags;
    long	    minval;
    long	    maxval;

    ret = regatom(&flags);
    എങ്കിൽ (ret == ശൂന്യ)
	തിരികെ ശൂന്യ;

    op = peekchr();
    എങ്കിൽ (re_multi_type(op) == NOT_MULTI)
    {
	*flagp = flags;
	തിരികെ ret;
    }
    // default flags
    *flagp = (WORST | SPSTART | (flags & (HASNL | HASLOOKBH)));

    skipchr();
    തിരഞ്ഞെടുപ്പ് (op)
    {
	അവസ്ഥ Magic('*'):
	    എങ്കിൽ (flags & SIMPLE)
		reginsert(STAR, ret);
	    അല്ലെങ്കിൽ
	    {
		// Emit x* as (x&|), where & means "self".
		reginsert(BRANCH, ret); // Either x
		regoptail(ret, regnode(BACK));	// and loop
		regoptail(ret, ret);	// back
		regtail(ret, regnode(BRANCH));	// or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    നിർത്തുക;

	അവസ്ഥ Magic('+'):
	    എങ്കിൽ (flags & SIMPLE)
		reginsert(PLUS, ret);
	    അല്ലെങ്കിൽ
	    {
		// Emit x+ as x(&|), where & means "self".
		next = regnode(BRANCH); // Either
		regtail(ret, next);
		regtail(regnode(BACK), ret);	// loop back
		regtail(next, regnode(BRANCH)); // or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    *flagp = (WORST | HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    നിർത്തുക;

	അവസ്ഥ Magic('@'):
	    {
		പൂർണ്ണസംഖ്യ	lop = END;
		long	nr;

		nr = (long)getdecchrs();
		തിരഞ്ഞെടുപ്പ് (no_Magic(getchr()))
		{
		    അവസ്ഥ '=': lop = MATCH; നിർത്തുക;		  // \@=
		    അവസ്ഥ '!': lop = NOMATCH; നിർത്തുക;		  // \@!
		    അവസ്ഥ '>': lop = SUBPAT; നിർത്തുക;		  // \@>
		    അവസ്ഥ '<': തിരഞ്ഞെടുപ്പ് (no_Magic(getchr()))
			      {
				  അവസ്ഥ '=': lop = BEHIND; നിർത്തുക;   // \@<=
				  അവസ്ഥ '!': lop = NOBEHIND; നിർത്തുക; // \@<!
			      }
		}
		എങ്കിൽ (lop == END)
		    EMSG2_RET_NULL(_(e_invalid_character_after_str_at),
						      reg_magic == MAGIC_ALL);
		// Look behind must match with behind_pos.
		എങ്കിൽ (lop == BEHIND || lop == NOBEHIND)
		{
		    regtail(ret, regnode(BHPOS));
		    *flagp |= HASLOOKBH;
		}
		regtail(ret, regnode(END)); // operand ends
		എങ്കിൽ (lop == BEHIND || lop == NOBEHIND)
		{
		    എങ്കിൽ (nr < 0)
			nr = 0; // no limit is same as zero limit
		    reginsert_nr(lop, nr, ret);
		}
		അല്ലെങ്കിൽ
		    reginsert(lop, ret);
		നിർത്തുക;
	    }

	അവസ്ഥ Magic('?'):
	അവസ്ഥ Magic('='):
	    // Emit x= as (x|)
	    reginsert(BRANCH, ret);		// Either x
	    regtail(ret, regnode(BRANCH));	// or
	    next = regnode(NOTHING);		// null.
	    regtail(ret, next);
	    regoptail(ret, next);
	    നിർത്തുക;

	അവസ്ഥ Magic('{'):
	    എങ്കിൽ (!read_limits(&minval, &maxval))
		തിരികെ ശൂന്യ;
	    എങ്കിൽ (flags & SIMPLE)
	    {
		reginsert(BRACE_SIMPLE, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (അക്കം_complex_braces >= 10)
		    EMSG2_RET_NULL(_(e_too_many_complex_str_curly),
						      reg_magic == MAGIC_ALL);
		reginsert(BRACE_COMPLEX + അക്കം_complex_braces, ret);
		regoptail(ret, regnode(BACK));
		regoptail(ret, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
		++അക്കം_complex_braces;
	    }
	    എങ്കിൽ (minval > 0 && maxval > 0)
		*flagp = (HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    നിർത്തുക;
    }
    എങ്കിൽ (re_multi_type(peekchr()) != NOT_MULTI)
    {
	// Can't have a multi follow a multi.
	എങ്കിൽ (peekchr() == Magic('*'))
	    EMSG2_RET_NULL(_(e_nested_str), reg_magic >= MAGIC_ON);
	EMSG3_RET_NULL(_(e_nested_str_chr), reg_magic == MAGIC_ALL,
							  no_Magic(peekchr()));
    }

    തിരികെ ret;
}

/*
 * Parse one alternative of an | or & operator.
 * Implements the concatenation operator.
 */
    സ്ഥിതം char_u *
regconcat(പൂർണ്ണസംഖ്യ *flagp)
{
    char_u	*first = ശൂന്യ;
    char_u	*chain = ശൂന്യ;
    char_u	*latest;
    പൂർണ്ണസംഖ്യ		flags;
    പൂർണ്ണസംഖ്യ		cont = TRUE;

    *flagp = WORST;		// Tentatively.

    ആകുമ്പോൾ (cont)
    {
	തിരഞ്ഞെടുപ്പ് (peekchr())
	{
	    അവസ്ഥ NUL:
	    അവസ്ഥ Magic('|'):
	    അവസ്ഥ Magic('&'):
	    അവസ്ഥ Magic(')'):
			    cont = FALSE;
			    നിർത്തുക;
	    അവസ്ഥ Magic('Z'):
			    regflags |= RF_ICOMBINE;
			    skipchr_keepstart();
			    നിർത്തുക;
	    അവസ്ഥ Magic('c'):
			    regflags |= RF_ICASE;
			    skipchr_keepstart();
			    നിർത്തുക;
	    അവസ്ഥ Magic('C'):
			    regflags |= RF_NOICASE;
			    skipchr_keepstart();
			    നിർത്തുക;
	    അവസ്ഥ Magic('v'):
			    reg_magic = MAGIC_ALL;
			    skipchr_keepstart();
			    curchr = -1;
			    നിർത്തുക;
	    അവസ്ഥ Magic('m'):
			    reg_magic = MAGIC_ON;
			    skipchr_keepstart();
			    curchr = -1;
			    നിർത്തുക;
	    അവസ്ഥ Magic('M'):
			    reg_magic = MAGIC_OFF;
			    skipchr_keepstart();
			    curchr = -1;
			    നിർത്തുക;
	    അവസ്ഥ Magic('V'):
			    reg_magic = MAGIC_NONE;
			    skipchr_keepstart();
			    curchr = -1;
			    നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:
			    latest = regpiece(&flags);
			    എങ്കിൽ (latest == ശൂന്യ || reg_toolong)
				തിരികെ ശൂന്യ;
			    *flagp |= flags & (HASWIDTH | HASNL | HASLOOKBH);
			    എങ്കിൽ (chain == ശൂന്യ)	// First piece.
				*flagp |= flags & SPSTART;
			    അല്ലെങ്കിൽ
				regtail(chain, latest);
			    chain = latest;
			    എങ്കിൽ (first == ശൂന്യ)
				first = latest;
			    നിർത്തുക;
	}
    }
    എങ്കിൽ (first == ശൂന്യ)		// Loop ran zero times.
	first = regnode(NOTHING);
    തിരികെ first;
}

/*
 * Parse one alternative of an | operator.
 * Implements the & operator.
 */
    സ്ഥിതം char_u *
regbranch(പൂർണ്ണസംഖ്യ *flagp)
{
    char_u	*ret;
    char_u	*chain = ശൂന്യ;
    char_u	*latest;
    പൂർണ്ണസംഖ്യ		flags;

    *flagp = WORST | HASNL;		// Tentatively.

    ret = regnode(BRANCH);
    വേണ്ടി (;;)
    {
	latest = regconcat(&flags);
	എങ്കിൽ (latest == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	// If one of the branches has width, the whole thing has.  If one of
	// the branches anchors at start-of-line, the whole thing does.
	// If one of the branches uses look-behind, the whole thing does.
	*flagp |= flags & (HASWIDTH | SPSTART | HASLOOKBH);
	// If one of the branches doesn't match a line-break, the whole thing
	// doesn't.
	*flagp &= ~HASNL | (flags & HASNL);
	എങ്കിൽ (chain != ശൂന്യ)
	    regtail(chain, latest);
	എങ്കിൽ (peekchr() != Magic('&'))
	    നിർത്തുക;
	skipchr();
	regtail(latest, regnode(END)); // operand ends
	എങ്കിൽ (reg_toolong)
	    നിർത്തുക;
	reginsert(MATCH, latest);
	chain = latest;
    }

    തിരികെ ret;
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
    സ്ഥിതം char_u *
reg(
    പൂർണ്ണസംഖ്യ		paren,	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
    പൂർണ്ണസംഖ്യ		*flagp)
{
    char_u	*ret;
    char_u	*br;
    char_u	*ender;
    പൂർണ്ണസംഖ്യ		parno = 0;
    പൂർണ്ണസംഖ്യ		flags;

    *flagp = HASWIDTH;		// Tentatively.

#ifdef FEAT_SYN_HL
    എങ്കിൽ (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	എങ്കിൽ (regnzpar >= NSUBEXP)
	    EMSG_RET_NULL(_(e_too_many_z));
	parno = regnzpar;
	regnzpar++;
	ret = regnode(ZOPEN + parno);
    }
    അല്ലെങ്കിൽ
#endif
	എങ്കിൽ (paren == REG_PAREN)
    {
	// Make a MOPEN node.
	എങ്കിൽ (regnpar >= NSUBEXP)
	    EMSG2_RET_NULL(_(e_too_many_str_open), reg_magic == MAGIC_ALL);
	parno = regnpar;
	++regnpar;
	ret = regnode(MOPEN + parno);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (paren == REG_NPAREN)
    {
	// Make a NOPEN node.
	ret = regnode(NOPEN);
    }
    അല്ലെങ്കിൽ
	ret = ശൂന്യ;

    എങ്കിൽ (bt_reg_parse_depth >= REG_MAX_PAREN_DEPTH)
	EMSG_RET_NULL(_(e_command_too_complex));
    ++bt_reg_parse_depth;

    // Pick up the branches, linking them together.
    br = regbranch(&flags);
    എങ്കിൽ (br == ശൂന്യ)
    {
	ret = ശൂന്യ;
	പോകുക theend;
    }
    എങ്കിൽ (ret != ശൂന്യ)
	regtail(ret, br);	// [MZ]OPEN -> first.
    അല്ലെങ്കിൽ
	ret = br;
    // If one of the branches can be zero-width, the whole thing can.
    // If one of the branches has * at start or matches a line-break, the
    // whole thing can.
    എങ്കിൽ (!(flags & HASWIDTH))
	*flagp &= ~HASWIDTH;
    *flagp |= flags & (SPSTART | HASNL | HASLOOKBH);
    ആകുമ്പോൾ (peekchr() == Magic('|'))
    {
	skipchr();
	br = regbranch(&flags);
	എങ്കിൽ (br == ശൂന്യ || reg_toolong)
	{
	    ret = ശൂന്യ;
	    പോകുക theend;
	}
	regtail(ret, br);	// BRANCH -> BRANCH.
	എങ്കിൽ (!(flags & HASWIDTH))
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
    വേണ്ടി (br = ret; br != ശൂന്യ; br = regnext(br))
	regoptail(br, ender);

    // Check for proper termination.
    എങ്കിൽ (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
#ifdef FEAT_SYN_HL
	എങ്കിൽ (paren == REG_ZPAREN)
	{
	    emsg(_(e_unmatched_z));
	    rc_did_emsg = TRUE;
	    ret = ശൂന്യ;
	    പോകുക theend;
	}
	അല്ലെങ്കിൽ
#endif
	എങ്കിൽ (paren == REG_NPAREN)
	{
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = ശൂന്യ;
	    പോകുക theend;
	}
	അല്ലെങ്കിൽ
	{
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = ശൂന്യ;
	    പോകുക theend;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (paren == REG_NOPAREN && peekchr() != NUL)
    {
	എങ്കിൽ (curchr == Magic(')'))
	{
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = ശൂന്യ;
	    പോകുക theend;
	}
	അല്ലെങ്കിൽ
	{
	    emsg(_(e_trailing_characters));	// "Can't happen".
	    rc_did_emsg = TRUE;
	    ret = ശൂന്യ;
	    പോകുക theend;
	}
    }
    // Here we set the flag allowing back references to this set of
    // parentheses.
    എങ്കിൽ (paren == REG_PAREN)
	had_endbrace[parno] = TRUE;	// have seen the close paren

theend:
    --bt_reg_parse_depth;
    തിരികെ ret;
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
    സ്ഥിതം regprog_T *
bt_regcomp(char_u *വ്യഞ്ജകം, പൂർണ്ണസംഖ്യ re_flags)
{
    bt_regprog_T    *r;
    char_u	*scan;
    char_u	*longest;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		flags;

    എങ്കിൽ (വ്യഞ്ജകം == ശൂന്യ)
	IEMSG_RET_NULL(e_null_argument);

    init_class_tab();

    // First pass: determine size, legality.
    regcomp_start(വ്യഞ്ജകം, re_flags);
    regcode = JUST_CALC_SIZE;
    regc(REGMAGIC);
    എങ്കിൽ (reg(REG_NOPAREN, &flags) == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // Allocate space.
    r = alloc(offsetof(bt_regprog_T, program) + regsize);
    എങ്കിൽ (r == ശൂന്യ)
	തിരികെ ശൂന്യ;
    r->re_in_use = FALSE;
#ifdef DEBUG
    r->regsz = regsize;
#endif

    // Second pass: emit code.
    regcomp_start(വ്യഞ്ജകം, re_flags);
    regcode = r->program;
    regc(REGMAGIC);
    എങ്കിൽ (reg(REG_NOPAREN, &flags) == ശൂന്യ || reg_toolong)
    {
	vim_free(r);
	എങ്കിൽ (reg_toolong)
	    EMSG_RET_NULL(_(e_pattern_too_long));
	തിരികെ ശൂന്യ;
    }

    // Dig out information for optimizations.
    r->regstart = NUL;		// Worst-case defaults.
    r->reganch = 0;
    r->regmust = ശൂന്യ;
    r->regmlen = 0;
    r->regflags = regflags;
    എങ്കിൽ (flags & HASNL)
	r->regflags |= RF_HASNL;
    എങ്കിൽ (flags & HASLOOKBH)
	r->regflags |= RF_LOOKBH;
#ifdef FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    r->reghasz = re_has_z;
#endif
    scan = r->program + 1;	// First BRANCH.
    എങ്കിൽ (OP(regnext(scan)) == END)   // Only one top-level choice.
    {
	scan = OPERAND(scan);

	// Starting-point info.
	എങ്കിൽ (OP(scan) == BOL || OP(scan) == RE_BOF)
	{
	    r->reganch++;
	    scan = regnext(scan);
	}

	എങ്കിൽ (OP(scan) == EXACTLY)
	{
	    എങ്കിൽ (has_mbyte)
		r->regstart = (*mb_ptr2char)(OPERAND(scan));
	    അല്ലെങ്കിൽ
		r->regstart = *OPERAND(scan);
	}
	അല്ലെങ്കിൽ എങ്കിൽ ((OP(scan) == BOW
		    || OP(scan) == EOW
		    || OP(scan) == NOTHING
		    || OP(scan) == MOPEN + 0 || OP(scan) == NOPEN
		    || OP(scan) == MCLOSE + 0 || OP(scan) == NCLOSE)
		 && OP(regnext(scan)) == EXACTLY)
	{
	    എങ്കിൽ (has_mbyte)
		r->regstart = (*mb_ptr2char)(OPERAND(regnext(scan)));
	    അല്ലെങ്കിൽ
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
	എങ്കിൽ ((flags & SPSTART || OP(scan) == BOW || OP(scan) == EOW)
							  && !(flags & HASNL))
	{
	    size_t  scanlen;

	    longest = ശൂന്യ;
	    len = 0;
	    വേണ്ടി (; scan != ശൂന്യ; scan = regnext(scan))
	    {
		എങ്കിൽ (OP(scan) == EXACTLY)
		{
		    scanlen = STRLEN(OPERAND(scan));
		    എങ്കിൽ (scanlen >= (size_t)len)
		    {
			longest = OPERAND(scan);
			len = (പൂർണ്ണസംഖ്യ)scanlen;
		    }
		}
	    }
	    r->regmust = longest;
	    r->regmlen = len;
	}
    }
#ifdef BT_REGEXP_DUMP
    regdump(വ്യഞ്ജകം, r);
#endif
    r->engine = &bt_regengine;
    തിരികെ (regprog_T *)r;
}

#എങ്കിൽ defined(FEAT_SYN_HL)
/*
 * Check if during the previous call to vim_regcomp the EOL item "$" has been
 * found.  This is messy, but it works fine.
 */
    പൂർണ്ണസംഖ്യ
vim_regcomp_had_eol(ശൂന്യം)
{
    തിരികെ had_eol;
}
#endif

/*
 * Get a number after a backslash that is inside [].
 * When nothing is recognized return a backslash.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
coll_get_char(ശൂന്യം)
{
    vimlong_T	nr = -1;

    തിരഞ്ഞെടുപ്പ് (*regparse++)
    {
	അവസ്ഥ 'd': nr = getdecchrs(); നിർത്തുക;
	അവസ്ഥ 'o': nr = getoctchrs(); നിർത്തുക;
	അവസ്ഥ 'x': nr = gethexchrs(2); നിർത്തുക;
	അവസ്ഥ 'u': nr = gethexchrs(4); നിർത്തുക;
	അവസ്ഥ 'U': nr = gethexchrs(8); നിർത്തുക;
    }
    എങ്കിൽ (nr < 0)
    {
	// If getting the number fails be backwards compatible: the character
	// is a backslash.
	--regparse;
	nr = '\\';
    }
    എങ്കിൽ (nr > INT_MAX)
	nr = INT_MAX;
    തിരികെ nr;
}

/*
 * Free a compiled regexp program, returned by bt_regcomp().
 */
    സ്ഥിതം ശൂന്യം
bt_regfree(regprog_T *prog)
{
    vim_free(prog);
}

#define ADVANCE_REGINPUT() MB_PTR_ADV(rex.നിവേശം)

/*
 * The arguments from BRACE_LIMITS are stored here.  They are actually local
 * to regmatch(), but they are here to reduce the amount of stack space used
 * (it can be called recursively many times).
 */
സ്ഥിതം long	bl_minval;
സ്ഥിതം long	bl_maxval;

/*
 * Save the input line and position in a regsave_T.
 */
    സ്ഥിതം ശൂന്യം
reg_save(regsave_T *save, garray_T *gap)
{
    എങ്കിൽ (REG_MULTI)
    {
	save->rs_u.pos.col = (colnr_T)(rex.നിവേശം - rex.വരി);
	save->rs_u.pos.lnum = rex.lnum;
    }
    അല്ലെങ്കിൽ
	save->rs_u.ptr = rex.നിവേശം;
    save->rs_len = gap->ga_len;
}

/*
 * Restore the input line and position from a regsave_T.
 */
    സ്ഥിതം ശൂന്യം
reg_restore(regsave_T *save, garray_T *gap)
{
    എങ്കിൽ (REG_MULTI)
    {
	എങ്കിൽ (rex.lnum != save->rs_u.pos.lnum)
	{
	    // only call reg_getline() when the line number changed to save
	    // a bit of time
	    rex.lnum = save->rs_u.pos.lnum;
	    rex.വരി = reg_getline(rex.lnum);
	}
	rex.നിവേശം = rex.വരി + save->rs_u.pos.col;
    }
    അല്ലെങ്കിൽ
	rex.നിവേശം = save->rs_u.ptr;
    gap->ga_len = save->rs_len;
}

/*
 * Return TRUE if current position is equal to saved position.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
reg_save_equal(regsave_T *save)
{
    എങ്കിൽ (REG_MULTI)
	തിരികെ rex.lnum == save->rs_u.pos.lnum
				  && rex.നിവേശം == rex.വരി + save->rs_u.pos.col;
    തിരികെ rex.നിവേശം == save->rs_u.ptr;
}

// Save the sub-expressions before attempting a match.
#define save_se(savep, posp, pp) \
    REG_MULTI ? save_se_multi((savep), (posp)) : save_se_one((savep), (pp))

// After a failed match restore the sub-expressions.
#define restore_se(savep, posp, pp) \
{ \
    എങ്കിൽ (REG_MULTI) \
	*(posp) = (savep)->se_u.pos; \
    അല്ലെങ്കിൽ \
	*(pp) = (savep)->se_u.ptr; \
}

/*
 * Tentatively set the sub-expression start to the current position (after
 * calling regmatch() they will have changed).  Need to save the existing
 * values for when there is no match.
 * Use se_save() to use pointer (save_se_multi()) or position (save_se_one()),
 * depending on REG_MULTI.
 */
    സ്ഥിതം ശൂന്യം
save_se_multi(save_se_T *savep, lpos_T *posp)
{
    savep->se_u.pos = *posp;
    posp->lnum = rex.lnum;
    posp->col = (colnr_T)(rex.നിവേശം - rex.വരി);
}

    സ്ഥിതം ശൂന്യം
save_se_one(save_se_T *savep, char_u **pp)
{
    savep->se_u.ptr = *pp;
    *pp = rex.നിവേശം;
}

/*
 * regrepeat - repeatedly match something simple, return how many.
 * Advances rex.input (and rex.lnum) to just after the matched chars.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
regrepeat(
    char_u	*p,
    long	maxcount)   // maximum number of matches allowed
{
    long	എണ്ണം = 0;
    char_u	*scan;
    char_u	*opnd;
    പൂർണ്ണസംഖ്യ		mask;
    പൂർണ്ണസംഖ്യ		testval = 0;

    scan = rex.നിവേശം;	    // Make local copy of rex.input for speed.
    opnd = OPERAND(p);
    തിരഞ്ഞെടുപ്പ് (OP(p))
    {
      അവസ്ഥ ANY:
      അവസ്ഥ ANY + ADD_NL:
	ആകുമ്പോൾ (എണ്ണം < maxcount)
	{
	    // Matching anything means we continue until end-of-line (or
	    // end-of-file for ANY + ADD_NL), only limited by maxcount.
	    ആകുമ്പോൾ (*scan != NUL && എണ്ണം < maxcount)
	    {
		++എണ്ണം;
		MB_PTR_ADV(scan);
	    }
	    എങ്കിൽ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
				      || rex.reg_വരി_lbr || എണ്ണം == maxcount)
		നിർത്തുക;
	    ++എണ്ണം;		// count the line-break
	    reg_nextline();
	    scan = rex.നിവേശം;
	    എങ്കിൽ (got_int)
		നിർത്തുക;
	}
	നിർത്തുക;

      അവസ്ഥ IDENT:
      അവസ്ഥ IDENT + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      അവസ്ഥ SIDENT:
      അവസ്ഥ SIDENT + ADD_NL:
	ആകുമ്പോൾ (എണ്ണം < maxcount)
	{
	    എങ്കിൽ (vim_isIDc(PTR2CHAR(scan)) && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (*scan == NUL)
	    {
		എങ്കിൽ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_വരി_lbr)
		    നിർത്തുക;
		reg_nextline();
		scan = rex.നിവേശം;
		എങ്കിൽ (got_int)
		    നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (rex.reg_വരി_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    അല്ലെങ്കിൽ
		നിർത്തുക;
	    ++എണ്ണം;
	}
	നിർത്തുക;

      അവസ്ഥ KWORD:
      അവസ്ഥ KWORD + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      അവസ്ഥ SKWORD:
      അവസ്ഥ SKWORD + ADD_NL:
	ആകുമ്പോൾ (എണ്ണം < maxcount)
	{
	    എങ്കിൽ (vim_iswordp_buf(scan, rex.reg_buf)
					  && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (*scan == NUL)
	    {
		എങ്കിൽ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_വരി_lbr)
		    നിർത്തുക;
		reg_nextline();
		scan = rex.നിവേശം;
		എങ്കിൽ (got_int)
		    നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (rex.reg_വരി_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    അല്ലെങ്കിൽ
		നിർത്തുക;
	    ++എണ്ണം;
	}
	നിർത്തുക;

      അവസ്ഥ FNAME:
      അവസ്ഥ FNAME + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      അവസ്ഥ SFNAME:
      അവസ്ഥ SFNAME + ADD_NL:
	ആകുമ്പോൾ (എണ്ണം < maxcount)
	{
	    എങ്കിൽ (vim_isfilec(PTR2CHAR(scan)) && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (*scan == NUL)
	    {
		എങ്കിൽ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_വരി_lbr)
		    നിർത്തുക;
		reg_nextline();
		scan = rex.നിവേശം;
		എങ്കിൽ (got_int)
		    നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (rex.reg_വരി_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    അല്ലെങ്കിൽ
		നിർത്തുക;
	    ++എണ്ണം;
	}
	നിർത്തുക;

      അവസ്ഥ PRINT:
      അവസ്ഥ PRINT + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      അവസ്ഥ SPRINT:
      അവസ്ഥ SPRINT + ADD_NL:
	ആകുമ്പോൾ (എണ്ണം < maxcount)
	{
	    എങ്കിൽ (*scan == NUL)
	    {
		എങ്കിൽ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_വരി_lbr)
		    നിർത്തുക;
		reg_nextline();
		scan = rex.നിവേശം;
		എങ്കിൽ (got_int)
		    നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (vim_isprintc(PTR2CHAR(scan)) == 1
					  && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (rex.reg_വരി_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    അല്ലെങ്കിൽ
		നിർത്തുക;
	    ++എണ്ണം;
	}
	നിർത്തുക;

      അവസ്ഥ WHITE:
      അവസ്ഥ WHITE + ADD_NL:
	testval = mask = RI_WHITE;
do_class:
	ആകുമ്പോൾ (എണ്ണം < maxcount)
	{
	    പൂർണ്ണസംഖ്യ		l;

	    എങ്കിൽ (*scan == NUL)
	    {
		എങ്കിൽ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_വരി_lbr)
		    നിർത്തുക;
		reg_nextline();
		scan = rex.നിവേശം;
		എങ്കിൽ (got_int)
		    നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte && (l = (*mb_ptr2len)(scan)) > 1)
	    {
		എങ്കിൽ (testval != 0)
		    നിർത്തുക;
		scan += l;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ ((class_tab[*scan] & mask) == testval)
		++scan;
	    അല്ലെങ്കിൽ എങ്കിൽ (rex.reg_വരി_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    അല്ലെങ്കിൽ
		നിർത്തുക;
	    ++എണ്ണം;
	}
	നിർത്തുക;

      അവസ്ഥ NWHITE:
      അവസ്ഥ NWHITE + ADD_NL:
	mask = RI_WHITE;
	പോകുക do_class;
      അവസ്ഥ DIGIT:
      അവസ്ഥ DIGIT + ADD_NL:
	testval = mask = RI_DIGIT;
	പോകുക do_class;
      അവസ്ഥ NDIGIT:
      അവസ്ഥ NDIGIT + ADD_NL:
	mask = RI_DIGIT;
	പോകുക do_class;
      അവസ്ഥ HEX:
      അവസ്ഥ HEX + ADD_NL:
	testval = mask = RI_HEX;
	പോകുക do_class;
      അവസ്ഥ NHEX:
      അവസ്ഥ NHEX + ADD_NL:
	mask = RI_HEX;
	പോകുക do_class;
      അവസ്ഥ OCTAL:
      അവസ്ഥ OCTAL + ADD_NL:
	testval = mask = RI_OCTAL;
	പോകുക do_class;
      അവസ്ഥ NOCTAL:
      അവസ്ഥ NOCTAL + ADD_NL:
	mask = RI_OCTAL;
	പോകുക do_class;
      അവസ്ഥ WORD:
      അവസ്ഥ WORD + ADD_NL:
	testval = mask = RI_WORD;
	പോകുക do_class;
      അവസ്ഥ NWORD:
      അവസ്ഥ NWORD + ADD_NL:
	mask = RI_WORD;
	പോകുക do_class;
      അവസ്ഥ HEAD:
      അവസ്ഥ HEAD + ADD_NL:
	testval = mask = RI_HEAD;
	പോകുക do_class;
      അവസ്ഥ NHEAD:
      അവസ്ഥ NHEAD + ADD_NL:
	mask = RI_HEAD;
	പോകുക do_class;
      അവസ്ഥ ALPHA:
      അവസ്ഥ ALPHA + ADD_NL:
	testval = mask = RI_ALPHA;
	പോകുക do_class;
      അവസ്ഥ NALPHA:
      അവസ്ഥ NALPHA + ADD_NL:
	mask = RI_ALPHA;
	പോകുക do_class;
      അവസ്ഥ LOWER:
      അവസ്ഥ LOWER + ADD_NL:
	testval = mask = RI_LOWER;
	പോകുക do_class;
      അവസ്ഥ NLOWER:
      അവസ്ഥ NLOWER + ADD_NL:
	mask = RI_LOWER;
	പോകുക do_class;
      അവസ്ഥ UPPER:
      അവസ്ഥ UPPER + ADD_NL:
	testval = mask = RI_UPPER;
	പോകുക do_class;
      അവസ്ഥ NUPPER:
      അവസ്ഥ NUPPER + ADD_NL:
	mask = RI_UPPER;
	പോകുക do_class;

      അവസ്ഥ EXACTLY:
	{
	    പൂർണ്ണസംഖ്യ	    cu, cl;

	    // This doesn't do a multi-byte character, because a MULTIBYTECODE
	    // would have been used for it.  It does handle single-byte
	    // characters, such as latin1.
	    എങ്കിൽ (rex.reg_ic)
	    {
		cu = MB_TOUPPER(*opnd);
		cl = MB_TOLOWER(*opnd);
		ആകുമ്പോൾ (എണ്ണം < maxcount && (*scan == cu || *scan == cl))
		{
		    എണ്ണം++;
		    scan++;
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		cu = *opnd;
		ആകുമ്പോൾ (എണ്ണം < maxcount && *scan == cu)
		{
		    എണ്ണം++;
		    scan++;
		}
	    }
	    നിർത്തുക;
	}

      അവസ്ഥ MULTIBYTECODE:
	{
	    പൂർണ്ണസംഖ്യ		i, len, cf = 0;

	    // Safety check (just in case 'encoding' was changed since
	    // compiling the program).
	    എങ്കിൽ ((len = (*mb_ptr2len)(opnd)) > 1)
	    {
		എങ്കിൽ (rex.reg_ic && enc_utf8)
		    cf = utf_fold(utf_ptr2char(opnd));
		ആകുമ്പോൾ (എണ്ണം < maxcount && (*mb_ptr2len)(scan) >= len)
		{
		    വേണ്ടി (i = 0; i < len; ++i)
			എങ്കിൽ (opnd[i] != scan[i])
			    നിർത്തുക;
		    എങ്കിൽ (i < len && (!rex.reg_ic || !enc_utf8
					|| utf_fold(utf_ptr2char(scan)) != cf))
			നിർത്തുക;
		    scan += len;
		    ++എണ്ണം;
		}
	    }
	}
	നിർത്തുക;

      അവസ്ഥ ANYOF:
      അവസ്ഥ ANYOF + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH

      അവസ്ഥ ANYBUT:
      അവസ്ഥ ANYBUT + ADD_NL:
	ആകുമ്പോൾ (എണ്ണം < maxcount)
	{
	    പൂർണ്ണസംഖ്യ len;

	    എങ്കിൽ (*scan == NUL)
	    {
		എങ്കിൽ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_വരി_lbr)
		    നിർത്തുക;
		reg_nextline();
		scan = rex.നിവേശം;
		എങ്കിൽ (got_int)
		    നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (rex.reg_വരി_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte && (len = (*mb_ptr2len)(scan)) > 1)
	    {
		എങ്കിൽ ((cstrchr(opnd, (*mb_ptr2char)(scan)) == ശൂന്യ) == testval)
		    നിർത്തുക;
		scan += len;
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ ((cstrchr(opnd, *scan) == ശൂന്യ) == testval)
		    നിർത്തുക;
		++scan;
	    }
	    ++എണ്ണം;
	}
	നിർത്തുക;

      അവസ്ഥ NEWL:
	ആകുമ്പോൾ (എണ്ണം < maxcount
		&& ((*scan == NUL && rex.lnum <= rex.reg_maxline
				       && !rex.reg_വരി_lbr && REG_MULTI)
		    || (*scan == '\n' && rex.reg_വരി_lbr)))
	{
	    എണ്ണം++;
	    എങ്കിൽ (rex.reg_വരി_lbr)
		ADVANCE_REGINPUT();
	    അല്ലെങ്കിൽ
		reg_nextline();
	    scan = rex.നിവേശം;
	    എങ്കിൽ (got_int)
		നിർത്തുക;
	}
	നിർത്തുക;

      സ്ഥിരസ്ഥിതി:			// Oh dear.  Called inappropriately.
	iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	printf("Called regrepeat with op code %d\n", OP(p));
#endif
	നിർത്തുക;
    }

    rex.നിവേശം = scan;

    തിരികെ (പൂർണ്ണസംഖ്യ)എണ്ണം;
}

/*
 * Push an item onto the regstack.
 * Returns pointer to new item.  Returns NULL when out of memory.
 */
    സ്ഥിതം regitem_T *
regstack_തള്ളുക(regstate_T state, char_u *scan)
{
    regitem_T	*rp;

    എങ്കിൽ ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
    {
	emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
	തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (GA_GROW_FAILS(&regstack, (പൂർണ്ണസംഖ്യ)വലുപ്പം(regitem_T)))
	തിരികെ ശൂന്യ;

    rp = (regitem_T *)((അക്ഷരം *)regstack.ga_data + regstack.ga_len);
    rp->rs_state = state;
    rp->rs_scan = scan;

    regstack.ga_len += വലുപ്പം(regitem_T);
    തിരികെ rp;
}

/*
 * Pop an item from the regstack.
 */
    സ്ഥിതം ശൂന്യം
regstack_എടുക്കുക(char_u **scan)
{
    regitem_T	*rp;

    rp = (regitem_T *)((അക്ഷരം *)regstack.ga_data + regstack.ga_len) - 1;
    *scan = rp->rs_scan;

    regstack.ga_len -= വലുപ്പം(regitem_T);
}

#ifdef FEAT_RELTIME
/*
 * Check if the timer expired, return TRUE if so.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
bt_did_time_out(പൂർണ്ണസംഖ്യ *timed_out)
{
    എങ്കിൽ (*timeout_flag)
    {
	എങ്കിൽ (timed_out != ശൂന്യ)
	{
# ifdef FEAT_EVAL
	    എങ്കിൽ (!*timed_out)
		ch_log(ശൂന്യ, "BT regexp timed out");
# endif
	    *timed_out = TRUE;
	}
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}
#endif

/*
 * Save the current subexpr to "bp", so that they can be restored
 * later by restore_subexpr().
 */
    സ്ഥിതം ശൂന്യം
save_subexpr(regbehind_T *bp)
{
    പൂർണ്ണസംഖ്യ i;

    // When "rex.need_clear_subexpr" is set we don't need to save the values,
    // only remember that this flag needs to be set again when restoring.
    bp->save_need_clear_subexpr = rex.need_clear_subexpr;
    എങ്കിൽ (rex.need_clear_subexpr)
	തിരികെ;

    വേണ്ടി (i = 0; i < NSUBEXP; ++i)
    {
	എങ്കിൽ (REG_MULTI)
	{
	    bp->save_start[i].se_u.pos = rex.reg_startpos[i];
	    bp->save_end[i].se_u.pos = rex.reg_endpos[i];
	}
	അല്ലെങ്കിൽ
	{
	    bp->save_start[i].se_u.ptr = rex.reg_startp[i];
	    bp->save_end[i].se_u.ptr = rex.reg_endp[i];
	}
    }
}

/*
 * Restore the subexpr from "bp".
 */
    സ്ഥിതം ശൂന്യം
restore_subexpr(regbehind_T *bp)
{
    പൂർണ്ണസംഖ്യ i;

    // Only need to restore saved values when they are not to be cleared.
    rex.need_clear_subexpr = bp->save_need_clear_subexpr;
    എങ്കിൽ (rex.need_clear_subexpr)
	തിരികെ;

    വേണ്ടി (i = 0; i < NSUBEXP; ++i)
    {
	എങ്കിൽ (REG_MULTI)
	{
	    rex.reg_startpos[i] = bp->save_start[i].se_u.pos;
	    rex.reg_endpos[i] = bp->save_end[i].se_u.pos;
	}
	അല്ലെങ്കിൽ
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
regmatch(
    char_u	*scan,		    // Current node.
    പൂർണ്ണസംഖ്യ		*timed_out UNUSED)  // flag set on timeout or NULL
{
  char_u	*next;		// Next node.
  പൂർണ്ണസംഖ്യ		op;
  പൂർണ്ണസംഖ്യ		c;
  regitem_T	*rp;
  പൂർണ്ണസംഖ്യ		no;
  പൂർണ്ണസംഖ്യ		status;		// one of the RA_ values:

  // Make "regstack" and "backpos" empty.  They are allocated and freed in
  // bt_regexec_both() to reduce malloc()/free() calls.
  regstack.ga_len = 0;
  backpos.ga_len = 0;

  // Repeat until "regstack" is empty.
  വേണ്ടി (;;)
  {
    // Some patterns may take a long time to match, e.g., "\([a-z]\+\)\+Q".
    // Allow interrupting them with CTRL-C.
    fast_breakcheck();

#ifdef DEBUG
    എങ്കിൽ (scan != ശൂന്യ && regnarrate)
    {
	mch_errmsg((അക്ഷരം *)regprop(scan));
	mch_errmsg("(\n");
    }
#endif

    // Repeat for items that can be matched sequentially, without using the
    // regstack.
    വേണ്ടി (;;)
    {
	എങ്കിൽ (got_int || scan == ശൂന്യ)
	{
	    status = RA_FAIL;
	    നിർത്തുക;
	}
#ifdef FEAT_RELTIME
	എങ്കിൽ (bt_did_time_out(timed_out))
	{
	    status = RA_FAIL;
	    നിർത്തുക;
	}
#endif
	status = RA_CONT;

#ifdef DEBUG
	എങ്കിൽ (regnarrate)
	{
	    mch_errmsg((അക്ഷരം *)regprop(scan));
	    mch_errmsg("...\n");
# ifdef FEAT_SYN_HL
	    എങ്കിൽ (re_extmatch_in != ശൂന്യ)
	    {
		പൂർണ്ണസംഖ്യ i;

		mch_errmsg(_("External submatches:\n"));
		വേണ്ടി (i = 0; i < NSUBEXP; i++)
		{
		    mch_errmsg("    \"");
		    എങ്കിൽ (re_extmatch_in->matches[i] != ശൂന്യ)
			mch_errmsg((അക്ഷരം *)re_extmatch_in->matches[i]);
		    mch_errmsg("\"\n");
		}
	    }
# endif
	}
#endif
	next = regnext(scan);

	op = OP(scan);
	// Check for character class with NL added.
	എങ്കിൽ (!rex.reg_വരി_lbr && WITH_NL(op) && REG_MULTI
			     && *rex.നിവേശം == NUL && rex.lnum <= rex.reg_maxline)
	{
	    reg_nextline();
	}
	അല്ലെങ്കിൽ എങ്കിൽ (rex.reg_വരി_lbr && WITH_NL(op) && *rex.നിവേശം == '\n')
	{
	    ADVANCE_REGINPUT();
	}
	അല്ലെങ്കിൽ
	{
	  എങ്കിൽ (WITH_NL(op))
	      op -= ADD_NL;
	  എങ്കിൽ (has_mbyte)
	      c = (*mb_ptr2char)(rex.നിവേശം);
	  അല്ലെങ്കിൽ
	      c = *rex.നിവേശം;
	  തിരഞ്ഞെടുപ്പ് (op)
	  {
	  അവസ്ഥ BOL:
	    എങ്കിൽ (rex.നിവേശം != rex.വരി)
		status = RA_NOMATCH;
	    നിർത്തുക;

	  അവസ്ഥ EOL:
	    എങ്കിൽ (c != NUL)
		status = RA_NOMATCH;
	    നിർത്തുക;

	  അവസ്ഥ RE_BOF:
	    // We're not at the beginning of the file when below the first
	    // line where we started, not at the start of the line or we
	    // didn't start at the first line of the buffer.
	    എങ്കിൽ (rex.lnum != 0 || rex.നിവേശം != rex.വരി
				       || (REG_MULTI && rex.reg_firstlnum > 1))
		status = RA_NOMATCH;
	    നിർത്തുക;

	  അവസ്ഥ RE_EOF:
	    എങ്കിൽ (rex.lnum != rex.reg_maxline || c != NUL)
		status = RA_NOMATCH;
	    നിർത്തുക;

	  അവസ്ഥ CURSOR:
	    // Check if the buffer is in a window and compare the
	    // rex.reg_win->w_cursor position to the match position.
	    എങ്കിൽ (rex.reg_win == ശൂന്യ
		    || (rex.lnum + rex.reg_firstlnum
						 != rex.reg_win->w_cursor.lnum)
		    || ((colnr_T)(rex.നിവേശം - rex.വരി)
						 != rex.reg_win->w_cursor.col))
		status = RA_NOMATCH;
	    നിർത്തുക;

	  അവസ്ഥ RE_MARK:
	    // Compare the mark position to the match position.
	    {
		പൂർണ്ണസംഖ്യ	mark = OPERAND(scan)[0];
		പൂർണ്ണസംഖ്യ	cmp = OPERAND(scan)[1];
		pos_T	*pos;
		size_t	col = REG_MULTI ? rex.നിവേശം - rex.വരി : 0;

		pos = getmark_buf(rex.reg_buf, mark, FALSE);

		// Line may have been freed, get it again.
		എങ്കിൽ (REG_MULTI)
		{
		    rex.വരി = reg_getline(rex.lnum);
		    rex.നിവേശം = rex.വരി + col;
		}

		എങ്കിൽ (pos == ശൂന്യ		     // mark doesn't exist
			|| pos->lnum <= 0)   // mark isn't set in reg_buf
		{
		    status = RA_NOMATCH;
		}
		അല്ലെങ്കിൽ
		{
		    colnr_T pos_col = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->col == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->col;

		    എങ്കിൽ ((pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_col == (colnr_T)(rex.നിവേശം - rex.വരി)
				    ? (cmp == '<' || cmp == '>')
				    : (pos_col < (colnr_T)(rex.നിവേശം - rex.വരി)
					? cmp != '>'
					: cmp != '<'))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? cmp != '>'
				    : cmp != '<')))
		    status = RA_NOMATCH;
		}
	    }
	    നിർത്തുക;

	  അവസ്ഥ RE_VISUAL:
	    എങ്കിൽ (!reg_match_visual())
		status = RA_NOMATCH;
	    നിർത്തുക;

	  അവസ്ഥ RE_LNUM:
	    എങ്കിൽ (!REG_MULTI || !re_അക്കം_cmp((long_u)(rex.lnum + rex.reg_firstlnum),
									scan))
		status = RA_NOMATCH;
	    നിർത്തുക;

	  അവസ്ഥ RE_COL:
	    എങ്കിൽ (!re_അക്കം_cmp((long_u)(rex.നിവേശം - rex.വരി) + 1, scan))
		status = RA_NOMATCH;
	    നിർത്തുക;

	  അവസ്ഥ RE_VCOL:
	    {
		win_T	    *wp = rex.reg_win == ശൂന്യ ? curwin : rex.reg_win;
		linenr_T    lnum = REG_MULTI ? rex.reg_firstlnum + rex.lnum : 1;
		long_u	    vcol;

		എങ്കിൽ (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_buffer->b_ml.ml_വരി_എണ്ണം))
		    lnum = 1;
		vcol = (long_u)win_linetabsize(wp, lnum, rex.വരി,
					      (colnr_T)(rex.നിവേശം - rex.വരി));
		എങ്കിൽ (!re_അക്കം_cmp(vcol + 1, scan))
		    status = RA_NOMATCH;
	    }
	    നിർത്തുക;

	  അവസ്ഥ BOW:	// \<word; rex.input points to w
	    എങ്കിൽ (c == NUL)	// Can't match at end of line
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte)
	    {
		പൂർണ്ണസംഖ്യ this_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.നിവേശം, rex.reg_buf);
		എങ്കിൽ (this_class <= 1)
		    status = RA_NOMATCH;  // not on a word at all
		അല്ലെങ്കിൽ എങ്കിൽ (reg_prev_class() == this_class)
		    status = RA_NOMATCH;  // previous char is in same word
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (!vim_iswordc_buf(c, rex.reg_buf) || (rex.നിവേശം > rex.വരി
				&& vim_iswordc_buf(rex.നിവേശം[-1], rex.reg_buf)))
		    status = RA_NOMATCH;
	    }
	    നിർത്തുക;

	  അവസ്ഥ EOW:	// word\>; rex.input points after d
	    എങ്കിൽ (rex.നിവേശം == rex.വരി)    // Can't match at start of line
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte)
	    {
		പൂർണ്ണസംഖ്യ this_class, prev_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.നിവേശം, rex.reg_buf);
		prev_class = reg_prev_class();
		എങ്കിൽ (this_class == prev_class
			|| prev_class == 0 || prev_class == 1)
		    status = RA_NOMATCH;
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (!vim_iswordc_buf(rex.നിവേശം[-1], rex.reg_buf)
			|| (rex.നിവേശം[0] != NUL
					   && vim_iswordc_buf(c, rex.reg_buf)))
		    status = RA_NOMATCH;
	    }
	    നിർത്തുക; // Matched with EOW

	  അവസ്ഥ ANY:
	    // ANY does not match new lines.
	    എങ്കിൽ (c == NUL)
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
		ADVANCE_REGINPUT();
	    നിർത്തുക;

	  അവസ്ഥ IDENT:
	    എങ്കിൽ (!vim_isIDc(c))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
		ADVANCE_REGINPUT();
	    നിർത്തുക;

	  അവസ്ഥ SIDENT:
	    എങ്കിൽ (VIM_ISDIGIT(*rex.നിവേശം) || !vim_isIDc(c))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
		ADVANCE_REGINPUT();
	    നിർത്തുക;

	  അവസ്ഥ KWORD:
	    എങ്കിൽ (!vim_iswordp_buf(rex.നിവേശം, rex.reg_buf))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
		ADVANCE_REGINPUT();
	    നിർത്തുക;

	  അവസ്ഥ SKWORD:
	    എങ്കിൽ (VIM_ISDIGIT(*rex.നിവേശം)
				    || !vim_iswordp_buf(rex.നിവേശം, rex.reg_buf))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
		ADVANCE_REGINPUT();
	    നിർത്തുക;

	  അവസ്ഥ FNAME:
	    എങ്കിൽ (!vim_isfilec(c))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
		ADVANCE_REGINPUT();
	    നിർത്തുക;

	  അവസ്ഥ SFNAME:
	    എങ്കിൽ (VIM_ISDIGIT(*rex.നിവേശം) || !vim_isfilec(c))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
		ADVANCE_REGINPUT();
	    നിർത്തുക;

	  അവസ്ഥ PRINT:
	    എങ്കിൽ (!vim_isprintc(PTR2CHAR(rex.നിവേശം)))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
		ADVANCE_REGINPUT();
	    നിർത്തുക;

	  അവസ്ഥ SPRINT:
	    എങ്കിൽ (VIM_ISDIGIT(*rex.നിവേശം) || !vim_isprintc(PTR2CHAR(rex.നിവേശം)))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
		ADVANCE_REGINPUT();
	    നിർത്തുക;

	  അവസ്ഥ WHITE:
	    എങ്കിൽ (!VIM_ISWHITE(c))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
		ADVANCE_REGINPUT();
	    നിർത്തുക;

	  അവസ്ഥ NWHITE:
	    എങ്കിൽ (c == NUL || VIM_ISWHITE(c))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
		ADVANCE_REGINPUT();
	    നിർത്തുക;

	  അവസ്ഥ DIGIT:
	    എങ്കിൽ (!ri_digit(c))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
		ADVANCE_REGINPUT();
	    നിർത്തുക;

	  അവസ്ഥ NDIGIT:
	    എങ്കിൽ (c == NUL || ri_digit(c))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
		ADVANCE_REGINPUT();
	    നിർത്തുക;

	  അവസ്ഥ HEX:
	    എങ്കിൽ (!ri_hex(c))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
		ADVANCE_REGINPUT();
	    നിർത്തുക;

	  അവസ്ഥ NHEX:
	    എങ്കിൽ (c == NUL || ri_hex(c))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
		ADVANCE_REGINPUT();
	    നിർത്തുക;

	  അവസ്ഥ OCTAL:
	    എങ്കിൽ (!ri_octal(c))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
		ADVANCE_REGINPUT();
	    നിർത്തുക;

	  അവസ്ഥ NOCTAL:
	    എങ്കിൽ (c == NUL || ri_octal(c))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
		ADVANCE_REGINPUT();
	    നിർത്തുക;

	  അവസ്ഥ WORD:
	    എങ്കിൽ (!ri_word(c))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
		ADVANCE_REGINPUT();
	    നിർത്തുക;

	  അവസ്ഥ NWORD:
	    എങ്കിൽ (c == NUL || ri_word(c))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
		ADVANCE_REGINPUT();
	    നിർത്തുക;

	  അവസ്ഥ HEAD:
	    എങ്കിൽ (!ri_head(c))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
		ADVANCE_REGINPUT();
	    നിർത്തുക;

	  അവസ്ഥ NHEAD:
	    എങ്കിൽ (c == NUL || ri_head(c))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
		ADVANCE_REGINPUT();
	    നിർത്തുക;

	  അവസ്ഥ ALPHA:
	    എങ്കിൽ (!ri_alpha(c))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
		ADVANCE_REGINPUT();
	    നിർത്തുക;

	  അവസ്ഥ NALPHA:
	    എങ്കിൽ (c == NUL || ri_alpha(c))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
		ADVANCE_REGINPUT();
	    നിർത്തുക;

	  അവസ്ഥ LOWER:
	    എങ്കിൽ (!ri_lower(c))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
		ADVANCE_REGINPUT();
	    നിർത്തുക;

	  അവസ്ഥ NLOWER:
	    എങ്കിൽ (c == NUL || ri_lower(c))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
		ADVANCE_REGINPUT();
	    നിർത്തുക;

	  അവസ്ഥ UPPER:
	    എങ്കിൽ (!ri_upper(c))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
		ADVANCE_REGINPUT();
	    നിർത്തുക;

	  അവസ്ഥ NUPPER:
	    എങ്കിൽ (c == NUL || ri_upper(c))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
		ADVANCE_REGINPUT();
	    നിർത്തുക;

	  അവസ്ഥ EXACTLY:
	    {
		പൂർണ്ണസംഖ്യ	len;
		char_u	*opnd;

		opnd = OPERAND(scan);
		// Inline the first byte, for speed.
		എങ്കിൽ (*opnd != *rex.നിവേശം
			&& (!rex.reg_ic
			    || (!enc_utf8
			      && MB_TOLOWER(*opnd) != MB_TOLOWER(*rex.നിവേശം))))
		    status = RA_NOMATCH;
		അല്ലെങ്കിൽ എങ്കിൽ (*opnd == NUL)
		{
		    // match empty string always works; happens when "~" is
		    // empty.
		}
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (opnd[1] == NUL && !(enc_utf8 && rex.reg_ic))
		    {
			len = 1;	// matched a single byte above
		    }
		    അല്ലെങ്കിൽ
		    {
			// Need to match first byte again for multi-byte.
			len = (പൂർണ്ണസംഖ്യ)STRLEN(opnd);
			എങ്കിൽ (cstrncmp(opnd, rex.നിവേശം, &len) != 0)
			    status = RA_NOMATCH;
		    }
		    // Check for following composing character, unless %C
		    // follows (skips over all composing chars).
		    എങ്കിൽ (status != RA_NOMATCH
			    && enc_utf8
			    && UTF_COMPOSINGLIKE(rex.നിവേശം, rex.നിവേശം + len)
			    && !rex.reg_icombine
			    && OP(next) != RE_COMPOSING)
		    {
			// raaron: This code makes a composing character get
			// ignored, which is the correct behavior (sometimes)
			// for voweled Hebrew texts.
			status = RA_NOMATCH;
		    }
		    എങ്കിൽ (status != RA_NOMATCH)
			rex.നിവേശം += len;
		}
	    }
	    നിർത്തുക;

	  അവസ്ഥ ANYOF:
	  അവസ്ഥ ANYBUT:
	    {
		char_u  *q = OPERAND(scan);

		എങ്കിൽ (c == NUL)
		    status = RA_NOMATCH;
		അല്ലെങ്കിൽ എങ്കിൽ ((cstrchr(q, c) == ശൂന്യ) == (op == ANYOF))
		    status = RA_NOMATCH;
		അല്ലെങ്കിൽ
		{
		    // Check following combining characters
		    പൂർണ്ണസംഖ്യ	len = 0;
		    പൂർണ്ണസംഖ്യ i;

		    എങ്കിൽ (enc_utf8)
			len = utfc_ptr2len(q) - utf_ptr2len(q);

		    MB_CPTR_ADV(rex.നിവേശം);
		    MB_CPTR_ADV(q);

		    എങ്കിൽ (!enc_utf8 || len == 0)
			നിർത്തുക;

		    വേണ്ടി (i = 0; i < len; ++i)
			എങ്കിൽ (q[i] != rex.നിവേശം[i])
			{
			    status = RA_NOMATCH;
			    നിർത്തുക;
			}
		    rex.നിവേശം += len;
		}
		നിർത്തുക;
	    }

	  അവസ്ഥ MULTIBYTECODE:
	    എങ്കിൽ (has_mbyte)
	    {
		പൂർണ്ണസംഖ്യ	i, len;
		char_u	*opnd;
		പൂർണ്ണസംഖ്യ	opndc = 0, inpc;

		opnd = OPERAND(scan);
		// Safety check (just in case 'encoding' was changed since
		// compiling the program).
		എങ്കിൽ ((len = (*mb_ptr2len)(opnd)) < 2)
		{
		    status = RA_NOMATCH;
		    നിർത്തുക;
		}
		എങ്കിൽ (enc_utf8)
		    opndc = utf_ptr2char(opnd);
		എങ്കിൽ (enc_utf8 && utf_iscomposing(opndc))
		{
		    // When only a composing char is given match at any
		    // position where that composing char appears.
		    status = RA_NOMATCH;
		    വേണ്ടി (i = 0; rex.നിവേശം[i] != NUL;
						i += utf_ptr2len(rex.നിവേശം + i))
		    {
			inpc = utf_ptr2char(rex.നിവേശം + i);
			എങ്കിൽ (!utf_iscomposing(inpc))
			{
			    എങ്കിൽ (i > 0)
				നിർത്തുക;
			}
			അല്ലെങ്കിൽ എങ്കിൽ (opndc == inpc)
			{
			    // Include all following composing chars.
			    len = i + utfc_ptr2len(rex.നിവേശം + i);
			    status = RA_MATCH;
			    നിർത്തുക;
			}
		    }
		}
		അല്ലെങ്കിൽ എങ്കിൽ (enc_utf8)
		{
		    എങ്കിൽ (cstrncmp(opnd, rex.നിവേശം, &len) != 0)
		    {
			status = RA_NOMATCH;
			നിർത്തുക;
		    }
		}
		അല്ലെങ്കിൽ
		    വേണ്ടി (i = 0; i < len; ++i)
			എങ്കിൽ (opnd[i] != rex.നിവേശം[i])
			{
			    status = RA_NOMATCH;
			    നിർത്തുക;
			}
		rex.നിവേശം += len;
	    }
	    അല്ലെങ്കിൽ
		status = RA_NOMATCH;
	    നിർത്തുക;
	  അവസ്ഥ RE_COMPOSING:
	    എങ്കിൽ (enc_utf8)
	    {
		// Skip composing characters.
		ആകുമ്പോൾ (utf_iscomposing(utf_ptr2char(rex.നിവേശം)))
		    MB_CPTR_ADV(rex.നിവേശം);
	    }
	    നിർത്തുക;

	  അവസ്ഥ NOTHING:
	    നിർത്തുക;

	  അവസ്ഥ BACK:
	    {
		പൂർണ്ണസംഖ്യ		i;
		backpos_T	*bp;

		// When we run into BACK we need to check if we don't keep
		// looping without matching any input.  The second and later
		// times a BACK is encountered it fails if the input is still
		// at the same position as the previous time.
		// The positions are stored in "backpos" and found by the
		// current value of "scan", the position in the RE program.
		bp = (backpos_T *)backpos.ga_data;
		വേണ്ടി (i = 0; i < backpos.ga_len; ++i)
		    എങ്കിൽ (bp[i].bp_scan == scan)
			നിർത്തുക;
		എങ്കിൽ (i == backpos.ga_len)
		{
		    // First time at this BACK, make room to store the pos.
		    എങ്കിൽ (GA_GROW_FAILS(&backpos, 1))
			status = RA_FAIL;
		    അല്ലെങ്കിൽ
		    {
			// get "ga_data" again, it may have changed
			bp = (backpos_T *)backpos.ga_data;
			bp[i].bp_scan = scan;
			++backpos.ga_len;
		    }
		}
		അല്ലെങ്കിൽ എങ്കിൽ (reg_save_equal(&bp[i].bp_pos))
		    // Still at same position as last time, fail.
		    status = RA_NOMATCH;

		എങ്കിൽ (status != RA_FAIL && status != RA_NOMATCH)
		    reg_save(&bp[i].bp_pos, &backpos);
	    }
	    നിർത്തുക;

	  അവസ്ഥ MOPEN + 0:   // Match start: \zs
	  അവസ്ഥ MOPEN + 1:   // \(
	  അവസ്ഥ MOPEN + 2:
	  അവസ്ഥ MOPEN + 3:
	  അവസ്ഥ MOPEN + 4:
	  അവസ്ഥ MOPEN + 5:
	  അവസ്ഥ MOPEN + 6:
	  അവസ്ഥ MOPEN + 7:
	  അവസ്ഥ MOPEN + 8:
	  അവസ്ഥ MOPEN + 9:
	    {
		no = op - MOPEN;
		cleanup_subexpr();
		rp = regstack_തള്ളുക(RS_MOPEN, scan);
		എങ്കിൽ (rp == ശൂന്യ)
		    status = RA_FAIL;
		അല്ലെങ്കിൽ
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_startpos[no],
							  &rex.reg_startp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    നിർത്തുക;

	  അവസ്ഥ NOPEN:	    // \%(
	  അവസ്ഥ NCLOSE:	    // \) after \%(
		എങ്കിൽ (regstack_തള്ളുക(RS_NOPEN, scan) == ശൂന്യ)
		    status = RA_FAIL;
		// We simply continue and handle the result when done.
		നിർത്തുക;

#ifdef FEAT_SYN_HL
	  അവസ്ഥ ZOPEN + 1:
	  അവസ്ഥ ZOPEN + 2:
	  അവസ്ഥ ZOPEN + 3:
	  അവസ്ഥ ZOPEN + 4:
	  അവസ്ഥ ZOPEN + 5:
	  അവസ്ഥ ZOPEN + 6:
	  അവസ്ഥ ZOPEN + 7:
	  അവസ്ഥ ZOPEN + 8:
	  അവസ്ഥ ZOPEN + 9:
	    {
		no = op - ZOPEN;
		cleanup_zsubexpr();
		rp = regstack_തള്ളുക(RS_ZOPEN, scan);
		എങ്കിൽ (rp == ശൂന്യ)
		    status = RA_FAIL;
		അല്ലെങ്കിൽ
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_startzpos[no],
							     &reg_startzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    നിർത്തുക;
#endif

	  അവസ്ഥ MCLOSE + 0:  // Match end: \ze
	  അവസ്ഥ MCLOSE + 1:  // \)
	  അവസ്ഥ MCLOSE + 2:
	  അവസ്ഥ MCLOSE + 3:
	  അവസ്ഥ MCLOSE + 4:
	  അവസ്ഥ MCLOSE + 5:
	  അവസ്ഥ MCLOSE + 6:
	  അവസ്ഥ MCLOSE + 7:
	  അവസ്ഥ MCLOSE + 8:
	  അവസ്ഥ MCLOSE + 9:
	    {
		no = op - MCLOSE;
		cleanup_subexpr();
		rp = regstack_തള്ളുക(RS_MCLOSE, scan);
		എങ്കിൽ (rp == ശൂന്യ)
		    status = RA_FAIL;
		അല്ലെങ്കിൽ
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_endpos[no],
							    &rex.reg_endp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    നിർത്തുക;

#ifdef FEAT_SYN_HL
	  അവസ്ഥ ZCLOSE + 1:  // \) after \z(
	  അവസ്ഥ ZCLOSE + 2:
	  അവസ്ഥ ZCLOSE + 3:
	  അവസ്ഥ ZCLOSE + 4:
	  അവസ്ഥ ZCLOSE + 5:
	  അവസ്ഥ ZCLOSE + 6:
	  അവസ്ഥ ZCLOSE + 7:
	  അവസ്ഥ ZCLOSE + 8:
	  അവസ്ഥ ZCLOSE + 9:
	    {
		no = op - ZCLOSE;
		cleanup_zsubexpr();
		rp = regstack_തള്ളുക(RS_ZCLOSE, scan);
		എങ്കിൽ (rp == ശൂന്യ)
		    status = RA_FAIL;
		അല്ലെങ്കിൽ
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_endzpos[no],
							      &reg_endzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    നിർത്തുക;
#endif

	  അവസ്ഥ BACKREF + 1:
	  അവസ്ഥ BACKREF + 2:
	  അവസ്ഥ BACKREF + 3:
	  അവസ്ഥ BACKREF + 4:
	  അവസ്ഥ BACKREF + 5:
	  അവസ്ഥ BACKREF + 6:
	  അവസ്ഥ BACKREF + 7:
	  അവസ്ഥ BACKREF + 8:
	  അവസ്ഥ BACKREF + 9:
	    {
		പൂർണ്ണസംഖ്യ		len;

		no = op - BACKREF;
		cleanup_subexpr();
		എങ്കിൽ (!REG_MULTI)		// Single-line regexp
		{
		    എങ്കിൽ (rex.reg_startp[no] == ശൂന്യ || rex.reg_endp[no] == ശൂന്യ)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    അല്ലെങ്കിൽ
		    {
			// Compare current input with back-ref in the same
			// line.
			len = (പൂർണ്ണസംഖ്യ)(rex.reg_endp[no] - rex.reg_startp[no]);
			എങ്കിൽ (cstrncmp(rex.reg_startp[no], rex.നിവേശം, &len) != 0)
			    status = RA_NOMATCH;
		    }
		}
		അല്ലെങ്കിൽ				// Multi-line regexp
		{
		    എങ്കിൽ (rex.reg_startpos[no].lnum < 0
						|| rex.reg_endpos[no].lnum < 0)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    അല്ലെങ്കിൽ
		    {
			എങ്കിൽ (rex.reg_startpos[no].lnum == rex.lnum
				&& rex.reg_endpos[no].lnum == rex.lnum)
			{
			    // Compare back-ref within the current line.
			    len = rex.reg_endpos[no].col
						    - rex.reg_startpos[no].col;
			    എങ്കിൽ (cstrncmp(rex.വരി + rex.reg_startpos[no].col,
							  rex.നിവേശം, &len) != 0)
				status = RA_NOMATCH;
			}
			അല്ലെങ്കിൽ
			{
			    // Messy situation: Need to compare between two
			    // lines.
			    പൂർണ്ണസംഖ്യ r = match_with_backref(
					    rex.reg_startpos[no].lnum,
					    rex.reg_startpos[no].col,
					    rex.reg_endpos[no].lnum,
					    rex.reg_endpos[no].col,
					    &len);

			    എങ്കിൽ (r != RA_MATCH)
				status = r;
			}
		    }
		}

		// Matched the backref, skip over it.
		rex.നിവേശം += len;
	    }
	    നിർത്തുക;

#ifdef FEAT_SYN_HL
	  അവസ്ഥ ZREF + 1:
	  അവസ്ഥ ZREF + 2:
	  അവസ്ഥ ZREF + 3:
	  അവസ്ഥ ZREF + 4:
	  അവസ്ഥ ZREF + 5:
	  അവസ്ഥ ZREF + 6:
	  അവസ്ഥ ZREF + 7:
	  അവസ്ഥ ZREF + 8:
	  അവസ്ഥ ZREF + 9:
	    {
		പൂർണ്ണസംഖ്യ	len;

		cleanup_zsubexpr();
		no = op - ZREF;
		എങ്കിൽ (re_extmatch_in != ശൂന്യ
			&& re_extmatch_in->matches[no] != ശൂന്യ)
		{
		    len = (പൂർണ്ണസംഖ്യ)STRLEN(re_extmatch_in->matches[no]);
		    എങ്കിൽ (cstrncmp(re_extmatch_in->matches[no],
							  rex.നിവേശം, &len) != 0)
			status = RA_NOMATCH;
		    അല്ലെങ്കിൽ
			rex.നിവേശം += len;
		}
		അല്ലെങ്കിൽ
		{
		    // Backref was not set: Match an empty string.
		}
	    }
	    നിർത്തുക;
#endif

	  അവസ്ഥ BRANCH:
	    {
		എങ്കിൽ (OP(next) != BRANCH) // No choice.
		    next = OPERAND(scan);	// Avoid recursion.
		അല്ലെങ്കിൽ
		{
		    rp = regstack_തള്ളുക(RS_BRANCH, scan);
		    എങ്കിൽ (rp == ശൂന്യ)
			status = RA_FAIL;
		    അല്ലെങ്കിൽ
			status = RA_BREAK;	// rest is below
		}
	    }
	    നിർത്തുക;

	  അവസ്ഥ BRACE_LIMITS:
	    {
		എങ്കിൽ (OP(next) == BRACE_SIMPLE)
		{
		    bl_minval = OPERAND_MIN(scan);
		    bl_maxval = OPERAND_MAX(scan);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (OP(next) >= BRACE_COMPLEX
			&& OP(next) < BRACE_COMPLEX + 10)
		{
		    no = OP(next) - BRACE_COMPLEX;
		    brace_min[no] = OPERAND_MIN(scan);
		    brace_max[no] = OPERAND_MAX(scan);
		    brace_എണ്ണം[no] = 0;
		}
		അല്ലെങ്കിൽ
		{
		    internal_error("BRACE_LIMITS");
		    status = RA_FAIL;
		}
	    }
	    നിർത്തുക;

	  അവസ്ഥ BRACE_COMPLEX + 0:
	  അവസ്ഥ BRACE_COMPLEX + 1:
	  അവസ്ഥ BRACE_COMPLEX + 2:
	  അവസ്ഥ BRACE_COMPLEX + 3:
	  അവസ്ഥ BRACE_COMPLEX + 4:
	  അവസ്ഥ BRACE_COMPLEX + 5:
	  അവസ്ഥ BRACE_COMPLEX + 6:
	  അവസ്ഥ BRACE_COMPLEX + 7:
	  അവസ്ഥ BRACE_COMPLEX + 8:
	  അവസ്ഥ BRACE_COMPLEX + 9:
	    {
		no = op - BRACE_COMPLEX;
		++brace_എണ്ണം[no];

		// If not matched enough times yet, try one more
		എങ്കിൽ (brace_എണ്ണം[no] <= (brace_min[no] <= brace_max[no]
					     ? brace_min[no] : brace_max[no]))
		{
		    rp = regstack_തള്ളുക(RS_BRCPLX_MORE, scan);
		    എങ്കിൽ (rp == ശൂന്യ)
			status = RA_FAIL;
		    അല്ലെങ്കിൽ
		    {
			rp->rs_no = no;
			reg_save(&rp->rs_un.regsave, &backpos);
			next = OPERAND(scan);
			// We continue and handle the result when done.
		    }
		    നിർത്തുക;
		}

		// If matched enough times, may try matching some more
		എങ്കിൽ (brace_min[no] <= brace_max[no])
		{
		    // Range is the normal way around, use longest match
		    എങ്കിൽ (brace_എണ്ണം[no] <= brace_max[no])
		    {
			rp = regstack_തള്ളുക(RS_BRCPLX_LONG, scan);
			എങ്കിൽ (rp == ശൂന്യ)
			    status = RA_FAIL;
			അല്ലെങ്കിൽ
			{
			    rp->rs_no = no;
			    reg_save(&rp->rs_un.regsave, &backpos);
			    next = OPERAND(scan);
			    // We continue and handle the result when done.
			}
		    }
		}
		അല്ലെങ്കിൽ
		{
		    // Range is backwards, use shortest match first
		    എങ്കിൽ (brace_എണ്ണം[no] <= brace_min[no])
		    {
			rp = regstack_തള്ളുക(RS_BRCPLX_SHORT, scan);
			എങ്കിൽ (rp == ശൂന്യ)
			    status = RA_FAIL;
			അല്ലെങ്കിൽ
			{
			    reg_save(&rp->rs_un.regsave, &backpos);
			    // We continue and handle the result when done.
			}
		    }
		}
	    }
	    നിർത്തുക;

	  അവസ്ഥ BRACE_SIMPLE:
	  അവസ്ഥ STAR:
	  അവസ്ഥ PLUS:
	    {
		regstar_T	rst;

		// Lookahead to avoid useless match attempts when we know
		// what character comes next.
		എങ്കിൽ (OP(next) == EXACTLY)
		{
		    rst.nextb = *OPERAND(next);
		    എങ്കിൽ (rex.reg_ic)
		    {
			എങ്കിൽ (MB_ISUPPER(rst.nextb))
			    rst.nextb_ic = MB_TOLOWER(rst.nextb);
			അല്ലെങ്കിൽ
			    rst.nextb_ic = MB_TOUPPER(rst.nextb);
		    }
		    അല്ലെങ്കിൽ
			rst.nextb_ic = rst.nextb;
		}
		അല്ലെങ്കിൽ
		{
		    rst.nextb = NUL;
		    rst.nextb_ic = NUL;
		}
		എങ്കിൽ (op != BRACE_SIMPLE)
		{
		    rst.minval = (op == STAR) ? 0 : 1;
		    rst.maxval = MAX_LIMIT;
		}
		അല്ലെങ്കിൽ
		{
		    rst.minval = bl_minval;
		    rst.maxval = bl_maxval;
		}

		// When maxval > minval, try matching as much as possible, up
		// to maxval.  When maxval < minval, try matching at least the
		// minimal number (since the range is backwards, that's also
		// maxval!).
		rst.എണ്ണം = regrepeat(OPERAND(scan), rst.maxval);
		എങ്കിൽ (got_int)
		{
		    status = RA_FAIL;
		    നിർത്തുക;
		}
		എങ്കിൽ (rst.minval <= rst.maxval
			  ? rst.എണ്ണം >= rst.minval : rst.എണ്ണം >= rst.maxval)
		{
		    // It could match.  Prepare for trying to match what
		    // follows.  The code is below.  Parameters are stored in
		    // a regstar_T on the regstack.
		    എങ്കിൽ ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
		    {
			emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
			status = RA_FAIL;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (GA_GROW_FAILS(&regstack, (പൂർണ്ണസംഖ്യ)വലുപ്പം(regstar_T)))
			status = RA_FAIL;
		    അല്ലെങ്കിൽ
		    {
			regstack.ga_len += വലുപ്പം(regstar_T);
			rp = regstack_തള്ളുക(rst.minval <= rst.maxval
					? RS_STAR_LONG : RS_STAR_SHORT, scan);
			എങ്കിൽ (rp == ശൂന്യ)
			    status = RA_FAIL;
			അല്ലെങ്കിൽ
			{
			    *(((regstar_T *)rp) - 1) = rst;
			    status = RA_BREAK;	    // skip the restore bits
			}
		    }
		}
		അല്ലെങ്കിൽ
		    status = RA_NOMATCH;

	    }
	    നിർത്തുക;

	  അവസ്ഥ NOMATCH:
	  അവസ്ഥ MATCH:
	  അവസ്ഥ SUBPAT:
	    rp = regstack_തള്ളുക(RS_NOMATCH, scan);
	    എങ്കിൽ (rp == ശൂന്യ)
		status = RA_FAIL;
	    അല്ലെങ്കിൽ
	    {
		rp->rs_no = op;
		reg_save(&rp->rs_un.regsave, &backpos);
		next = OPERAND(scan);
		// We continue and handle the result when done.
	    }
	    നിർത്തുക;

	  അവസ്ഥ BEHIND:
	  അവസ്ഥ NOBEHIND:
	    // Need a bit of room to store extra positions.
	    എങ്കിൽ ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
	    {
		emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		status = RA_FAIL;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (GA_GROW_FAILS(&regstack, (പൂർണ്ണസംഖ്യ)വലുപ്പം(regbehind_T)))
		status = RA_FAIL;
	    അല്ലെങ്കിൽ
	    {
		regstack.ga_len += വലുപ്പം(regbehind_T);
		rp = regstack_തള്ളുക(RS_BEHIND1, scan);
		എങ്കിൽ (rp == ശൂന്യ)
		    status = RA_FAIL;
		അല്ലെങ്കിൽ
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
	    നിർത്തുക;

	  അവസ്ഥ BHPOS:
	    എങ്കിൽ (REG_MULTI)
	    {
		എങ്കിൽ (behind_pos.rs_u.pos.col != (colnr_T)(rex.നിവേശം - rex.വരി)
			|| behind_pos.rs_u.pos.lnum != rex.lnum)
		    status = RA_NOMATCH;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (behind_pos.rs_u.ptr != rex.നിവേശം)
		status = RA_NOMATCH;
	    നിർത്തുക;

	  അവസ്ഥ NEWL:
	    എങ്കിൽ ((c != NUL || !REG_MULTI || rex.lnum > rex.reg_maxline
			     || rex.reg_വരി_lbr)
					   && (c != '\n' || !rex.reg_വരി_lbr))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ എങ്കിൽ (rex.reg_വരി_lbr)
		ADVANCE_REGINPUT();
	    അല്ലെങ്കിൽ
		reg_nextline();
	    നിർത്തുക;

	  അവസ്ഥ END:
	    status = RA_MATCH;	// Success!
	    നിർത്തുക;

	  സ്ഥിരസ്ഥിതി:
	    iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	    printf("Illegal op code %d\n", op);
#endif
	    status = RA_FAIL;
	    നിർത്തുക;
	  }
	}

	// If we can't continue sequentially, break the inner loop.
	എങ്കിൽ (status != RA_CONT)
	    നിർത്തുക;

	// Continue in inner loop, advance to next item.
	scan = next;

    } // end of inner loop

    // If there is something on the regstack execute the code for the state.
    // If the state is popped then loop and use the older state.
    ആകുമ്പോൾ (regstack.ga_len > 0 && status != RA_FAIL)
    {
	rp = (regitem_T *)((അക്ഷരം *)regstack.ga_data + regstack.ga_len) - 1;
	തിരഞ്ഞെടുപ്പ് (rp->rs_state)
	{
	  അവസ്ഥ RS_NOPEN:
	    // Result is passed on as-is, simply pop the state.
	    regstack_എടുക്കുക(&scan);
	    നിർത്തുക;

	  അവസ്ഥ RS_MOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    എങ്കിൽ (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_startpos[rp->rs_no],
						  &rex.reg_startp[rp->rs_no]);
	    regstack_എടുക്കുക(&scan);
	    നിർത്തുക;

#ifdef FEAT_SYN_HL
	  അവസ്ഥ RS_ZOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    എങ്കിൽ (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_startzpos[rp->rs_no],
						 &reg_startzp[rp->rs_no]);
	    regstack_എടുക്കുക(&scan);
	    നിർത്തുക;
#endif

	  അവസ്ഥ RS_MCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    എങ്കിൽ (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_endpos[rp->rs_no],
						    &rex.reg_endp[rp->rs_no]);
	    regstack_എടുക്കുക(&scan);
	    നിർത്തുക;

#ifdef FEAT_SYN_HL
	  അവസ്ഥ RS_ZCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    എങ്കിൽ (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_endzpos[rp->rs_no],
						   &reg_endzp[rp->rs_no]);
	    regstack_എടുക്കുക(&scan);
	    നിർത്തുക;
#endif

	  അവസ്ഥ RS_BRANCH:
	    എങ്കിൽ (status == RA_MATCH)
		// this branch matched, use it
		regstack_എടുക്കുക(&scan);
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (status != RA_BREAK)
		{
		    // After a non-matching branch: try next one.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    scan = rp->rs_scan;
		}
		എങ്കിൽ (scan == ശൂന്യ || OP(scan) != BRANCH)
		{
		    // no more branches, didn't find a match
		    status = RA_NOMATCH;
		    regstack_എടുക്കുക(&scan);
		}
		അല്ലെങ്കിൽ
		{
		    // Prepare to try a branch.
		    rp->rs_scan = regnext(scan);
		    reg_save(&rp->rs_un.regsave, &backpos);
		    scan = OPERAND(scan);
		}
	    }
	    നിർത്തുക;

	  അവസ്ഥ RS_BRCPLX_MORE:
	    // Pop the state.  Restore pointers when there is no match.
	    എങ്കിൽ (status == RA_NOMATCH)
	    {
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_എണ്ണം[rp->rs_no];	// decrement match count
	    }
	    regstack_എടുക്കുക(&scan);
	    നിർത്തുക;

	  അവസ്ഥ RS_BRCPLX_LONG:
	    // Pop the state.  Restore pointers when there is no match.
	    എങ്കിൽ (status == RA_NOMATCH)
	    {
		// There was no match, but we did find enough matches.
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_എണ്ണം[rp->rs_no];
		// continue with the items after "\{}"
		status = RA_CONT;
	    }
	    regstack_എടുക്കുക(&scan);
	    എങ്കിൽ (status == RA_CONT)
		scan = regnext(scan);
	    നിർത്തുക;

	  അവസ്ഥ RS_BRCPLX_SHORT:
	    // Pop the state.  Restore pointers when there is no match.
	    എങ്കിൽ (status == RA_NOMATCH)
		// There was no match, try to match one more item.
		reg_restore(&rp->rs_un.regsave, &backpos);
	    regstack_എടുക്കുക(&scan);
	    എങ്കിൽ (status == RA_NOMATCH)
	    {
		scan = OPERAND(scan);
		status = RA_CONT;
	    }
	    നിർത്തുക;

	  അവസ്ഥ RS_NOMATCH:
	    // Pop the state.  If the operand matches for NOMATCH or
	    // doesn't match for MATCH/SUBPAT, we fail.  Otherwise backup,
	    // except for SUBPAT, and continue with the next item.
	    എങ്കിൽ (status == (rp->rs_no == NOMATCH ? RA_MATCH : RA_NOMATCH))
		status = RA_NOMATCH;
	    അല്ലെങ്കിൽ
	    {
		status = RA_CONT;
		എങ്കിൽ (rp->rs_no != SUBPAT)	// zero-width
		    reg_restore(&rp->rs_un.regsave, &backpos);
	    }
	    regstack_എടുക്കുക(&scan);
	    എങ്കിൽ (status == RA_CONT)
		scan = regnext(scan);
	    നിർത്തുക;

	  അവസ്ഥ RS_BEHIND1:
	    എങ്കിൽ (status == RA_NOMATCH)
	    {
		regstack_എടുക്കുക(&scan);
		regstack.ga_len -= വലുപ്പം(regbehind_T);
	    }
	    അല്ലെങ്കിൽ
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
	    നിർത്തുക;

	  അവസ്ഥ RS_BEHIND2:
	    // Looping for BEHIND / NOBEHIND match.
	    എങ്കിൽ (status == RA_MATCH && reg_save_equal(&behind_pos))
	    {
		// found a match that ends where "next" started
		behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		എങ്കിൽ (rp->rs_no == BEHIND)
		    reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
		അല്ലെങ്കിൽ
		{
		    // But we didn't want a match.  Need to restore the
		    // subexpr, because what follows matched, so they have
		    // been set.
		    status = RA_NOMATCH;
		    restore_subexpr(((regbehind_T *)rp) - 1);
		}
		regstack_എടുക്കുക(&scan);
		regstack.ga_len -= വലുപ്പം(regbehind_T);
	    }
	    അല്ലെങ്കിൽ
	    {
		long limit;

		// No match or a match that doesn't end where we want it: Go
		// back one character.  May go to previous line once.
		no = OK;
		limit = OPERAND_MIN(rp->rs_scan);
		എങ്കിൽ (REG_MULTI)
		{
		    എങ്കിൽ (limit > 0
			    && ((rp->rs_un.regsave.rs_u.pos.lnum
						    < behind_pos.rs_u.pos.lnum
				    ? (colnr_T)STRLEN(rex.വരി)
				    : behind_pos.rs_u.pos.col)
				- rp->rs_un.regsave.rs_u.pos.col >= limit))
			no = FAIL;
		    അല്ലെങ്കിൽ എങ്കിൽ (rp->rs_un.regsave.rs_u.pos.col == 0)
		    {
			എങ്കിൽ (rp->rs_un.regsave.rs_u.pos.lnum
					< behind_pos.rs_u.pos.lnum
				|| reg_getline(
					--rp->rs_un.regsave.rs_u.pos.lnum)
								  == ശൂന്യ)
			    no = FAIL;
			അല്ലെങ്കിൽ
			{
			    reg_restore(&rp->rs_un.regsave, &backpos);
			    rp->rs_un.regsave.rs_u.pos.col =
						 (colnr_T)STRLEN(rex.വരി);
			}
		    }
		    അല്ലെങ്കിൽ
		    {
			എങ്കിൽ (has_mbyte)
			{
			    char_u *വരി =
				  reg_getline(rp->rs_un.regsave.rs_u.pos.lnum);

			    rp->rs_un.regsave.rs_u.pos.col -=
				(*mb_head_off)(വരി, വരി
				    + rp->rs_un.regsave.rs_u.pos.col - 1) + 1;
			}
			അല്ലെങ്കിൽ
			    --rp->rs_un.regsave.rs_u.pos.col;
		    }
		}
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (rp->rs_un.regsave.rs_u.ptr == rex.വരി)
			no = FAIL;
		    അല്ലെങ്കിൽ
		    {
			MB_PTR_BACK(rex.വരി, rp->rs_un.regsave.rs_u.ptr);
			എങ്കിൽ (limit > 0 && (long)(behind_pos.rs_u.ptr
				     - rp->rs_un.regsave.rs_u.ptr) > limit)
			    no = FAIL;
		    }
		}
		എങ്കിൽ (no == OK)
		{
		    // Advanced, prepare for finding match again.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    scan = OPERAND(rp->rs_scan) + 4;
		    എങ്കിൽ (status == RA_MATCH)
		    {
			// We did match, so subexpr may have been changed,
			// need to restore them for the next try.
			status = RA_NOMATCH;
			restore_subexpr(((regbehind_T *)rp) - 1);
		    }
		}
		അല്ലെങ്കിൽ
		{
		    // Can't advance.  For NOBEHIND that's a match.
		    behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		    എങ്കിൽ (rp->rs_no == NOBEHIND)
		    {
			reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
			status = RA_MATCH;
		    }
		    അല്ലെങ്കിൽ
		    {
			// We do want a proper match.  Need to restore the
			// subexpr if we had a match, because they may have
			// been set.
			എങ്കിൽ (status == RA_MATCH)
			{
			    status = RA_NOMATCH;
			    restore_subexpr(((regbehind_T *)rp) - 1);
			}
		    }
		    regstack_എടുക്കുക(&scan);
		    regstack.ga_len -= വലുപ്പം(regbehind_T);
		}
	    }
	    നിർത്തുക;

	  അവസ്ഥ RS_STAR_LONG:
	  അവസ്ഥ RS_STAR_SHORT:
	    {
		regstar_T	    *rst = ((regstar_T *)rp) - 1;

		എങ്കിൽ (status == RA_MATCH)
		{
		    regstack_എടുക്കുക(&scan);
		    regstack.ga_len -= വലുപ്പം(regstar_T);
		    നിർത്തുക;
		}

		// Tried once already, restore input pointers.
		എങ്കിൽ (status != RA_BREAK)
		    reg_restore(&rp->rs_un.regsave, &backpos);

		// Repeat until we found a position where it could match.
		വേണ്ടി (;;)
		{
		    എങ്കിൽ (status != RA_BREAK)
		    {
			// Tried first position already, advance.
			എങ്കിൽ (rp->rs_state == RS_STAR_LONG)
			{
			    // Trying for longest match, but couldn't or
			    // didn't match -- back up one char.
			    എങ്കിൽ (--rst->എണ്ണം < rst->minval)
				നിർത്തുക;
			    എങ്കിൽ (rex.നിവേശം == rex.വരി)
			    {
				// backup to last char of previous line
				എങ്കിൽ (rex.lnum == 0)
				{
				    status = RA_NOMATCH;
				    നിർത്തുക;
				}
				--rex.lnum;
				rex.വരി = reg_getline(rex.lnum);
				// Just in case regrepeat() didn't count
				// right.
				എങ്കിൽ (rex.വരി == ശൂന്യ)
				    നിർത്തുക;
				rex.നിവേശം = rex.വരി + reg_getline_len(rex.lnum);
				fast_breakcheck();
			    }
			    അല്ലെങ്കിൽ
				MB_PTR_BACK(rex.വരി, rex.നിവേശം);
			}
			അല്ലെങ്കിൽ
			{
			    // Range is backwards, use shortest match first.
			    // Careful: maxval and minval are exchanged!
			    // Couldn't or didn't match: try advancing one
			    // char.
			    എങ്കിൽ (rst->എണ്ണം == rst->minval
				  || regrepeat(OPERAND(rp->rs_scan), 1L) == 0)
				നിർത്തുക;
			    ++rst->എണ്ണം;
			}
			എങ്കിൽ (got_int)
			    നിർത്തുക;
		    }
		    അല്ലെങ്കിൽ
			status = RA_NOMATCH;

		    // If it could match, try it.
		    എങ്കിൽ (rst->nextb == NUL || *rex.നിവേശം == rst->nextb
					     || *rex.നിവേശം == rst->nextb_ic)
		    {
			reg_save(&rp->rs_un.regsave, &backpos);
			scan = regnext(rp->rs_scan);
			status = RA_CONT;
			നിർത്തുക;
		    }
		}
		എങ്കിൽ (status != RA_CONT)
		{
		    // Failed.
		    regstack_എടുക്കുക(&scan);
		    regstack.ga_len -= വലുപ്പം(regstar_T);
		    status = RA_NOMATCH;
		}
	    }
	    നിർത്തുക;
	}

	// If we want to continue the inner loop or didn't pop a state
	// continue matching loop
	എങ്കിൽ (status == RA_CONT || rp == (regitem_T *)
			     ((അക്ഷരം *)regstack.ga_data + regstack.ga_len) - 1)
	    നിർത്തുക;

#ifdef FEAT_RELTIME
	എങ്കിൽ (bt_did_time_out(timed_out))
	{
	    status = RA_FAIL;
	    നിർത്തുക;
	}
#endif
    }

    // May need to continue with the inner loop, starting at "scan".
    എങ്കിൽ (status == RA_CONT)
	തുടരുക;

    // If the regstack is empty or something failed we are done.
    എങ്കിൽ (regstack.ga_len == 0 || status == RA_FAIL)
    {
	എങ്കിൽ (scan == ശൂന്യ)
	{
	    // We get here only if there's trouble -- normally "case END" is
	    // the terminating point.
	    iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	    printf("Premature EOL\n");
#endif
	}
	തിരികെ (status == RA_MATCH);
    }

  } // End of loop until the regstack is empty.

  // NOTREACHED
}

/*
 * regtry - try match of "prog" with at rex.line["col"].
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    സ്ഥിതം long
regtry(
    bt_regprog_T	*prog,
    colnr_T		col,
    പൂർണ്ണസംഖ്യ			*timed_out)	// flag set on timeout or NULL
{
    rex.നിവേശം = rex.വരി + col;
    rex.need_clear_subexpr = TRUE;
#ifdef FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    rex.need_clear_zsubexpr = (prog->reghasz == REX_SET);
#endif

    എങ്കിൽ (regmatch(prog->program + 1, timed_out) == 0)
	തിരികെ 0;

    cleanup_subexpr();
    എങ്കിൽ (REG_MULTI)
    {
	എങ്കിൽ (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].col = col;
	}
	എങ്കിൽ (rex.reg_endpos[0].lnum < 0)
	{
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].col = (പൂർണ്ണസംഖ്യ)(rex.നിവേശം - rex.വരി);
	}
	അല്ലെങ്കിൽ
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (rex.reg_startp[0] == ശൂന്യ)
	    rex.reg_startp[0] = rex.വരി + col;
	എങ്കിൽ (rex.reg_endp[0] == ശൂന്യ)
	    rex.reg_endp[0] = rex.നിവേശം;
    }
#ifdef FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = ശൂന്യ;

    എങ്കിൽ (prog->reghasz == REX_SET)
    {
	പൂർണ്ണസംഖ്യ		i;

	cleanup_zsubexpr();
	re_extmatch_out = make_extmatch();
	എങ്കിൽ (re_extmatch_out == ശൂന്യ)
	    തിരികെ 0;
	വേണ്ടി (i = 0; i < NSUBEXP; i++)
	{
	    എങ്കിൽ (REG_MULTI)
	    {
		// Only accept single line matches.
		എങ്കിൽ (reg_startzpos[i].lnum >= 0
			&& reg_endzpos[i].lnum == reg_startzpos[i].lnum
			&& reg_endzpos[i].col >= reg_startzpos[i].col)
		    re_extmatch_out->matches[i] =
			vim_strnsave(reg_getline(reg_startzpos[i].lnum)
						       + reg_startzpos[i].col,
				   reg_endzpos[i].col - reg_startzpos[i].col);
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (reg_startzp[i] != ശൂന്യ && reg_endzp[i] != ശൂന്യ)
		    re_extmatch_out->matches[i] =
			    vim_strnsave(reg_startzp[i],
						reg_endzp[i] - reg_startzp[i]);
	    }
	}
    }
#endif
    തിരികെ 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    സ്ഥിതം long
bt_regexec_both(
    char_u	*വരി,
    colnr_T	startcol,	// column to start looking for match
    പൂർണ്ണസംഖ്യ		*timed_out)	// flag set on timeout or NULL
{
    bt_regprog_T    *prog;
    char_u	    *s;
    colnr_T	    col = startcol;
    long	    retval = 0L;

    // Create "regstack" and "backpos" if they are not allocated yet.
    // We allocate *_INITIAL amount of bytes first and then set the grow size
    // to much bigger value to avoid many malloc calls in case of deep regular
    // expressions.
    എങ്കിൽ (regstack.ga_data == ശൂന്യ)
    {
	// Use an item size of 1 byte, since we push different things
	// onto the regstack.
	ga_init2(&regstack, 1, REGSTACK_INITIAL);
	(ശൂന്യം)ga_grow(&regstack, REGSTACK_INITIAL);
	regstack.ga_growsize = REGSTACK_INITIAL * 8;
    }

    എങ്കിൽ (backpos.ga_data == ശൂന്യ)
    {
	ga_init2(&backpos, വലുപ്പം(backpos_T), BACKPOS_INITIAL);
	(ശൂന്യം)ga_grow(&backpos, BACKPOS_INITIAL);
	backpos.ga_growsize = BACKPOS_INITIAL * 8;
    }

    എങ്കിൽ (REG_MULTI)
    {
	prog = (bt_regprog_T *)rex.reg_mmatch->regprog;
	വരി = reg_getline((linenr_T)0);
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    അല്ലെങ്കിൽ
    {
	prog = (bt_regprog_T *)rex.reg_match->regprog;
	rex.reg_startp = rex.reg_match->startp;
	rex.reg_endp = rex.reg_match->endp;
    }

    // Be paranoid...
    എങ്കിൽ (prog == ശൂന്യ || വരി == ശൂന്യ)
    {
	iemsg(e_null_argument);
	പോകുക theend;
    }

    // Check validity of program.
    എങ്കിൽ (prog_magic_wrong())
	പോകുക theend;

    // If the start column is past the maximum column: no need to try.
    എങ്കിൽ (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	പോകുക theend;

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    എങ്കിൽ (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    അല്ലെങ്കിൽ എങ്കിൽ (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    എങ്കിൽ (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    // If there is a "must appear" string, look for it.
    എങ്കിൽ (prog->regmust != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ c;

	എങ്കിൽ (has_mbyte)
	    c = (*mb_ptr2char)(prog->regmust);
	അല്ലെങ്കിൽ
	    c = *prog->regmust;
	s = വരി + col;

	// This is used very often, esp. for ":global".  Use three versions of
	// the loop to avoid overhead of conditions.
	എങ്കിൽ (!rex.reg_ic && !has_mbyte)
	    ആകുമ്പോൾ ((s = vim_strbyte(s, c)) != ശൂന്യ)
	    {
		പൂർണ്ണസംഖ്യ len = prog->regmlen;
		എങ്കിൽ (cstrncmp(s, prog->regmust, &len) == 0)
		    നിർത്തുക;		// Found it.
		++s;
	    }
	അല്ലെങ്കിൽ എങ്കിൽ (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	    ആകുമ്പോൾ ((s = vim_strchr(s, c)) != ശൂന്യ)
	    {
		പൂർണ്ണസംഖ്യ len = prog->regmlen;
		എങ്കിൽ (cstrncmp(s, prog->regmust, &len) == 0)
		    നിർത്തുക;		// Found it.
		MB_PTR_ADV(s);
	    }
	അല്ലെങ്കിൽ
	    ആകുമ്പോൾ ((s = cstrchr(s, c)) != ശൂന്യ)
	    {
		പൂർണ്ണസംഖ്യ len = prog->regmlen;
		എങ്കിൽ (cstrncmp(s, prog->regmust, &len) == 0)
		    നിർത്തുക;		// Found it.
		MB_PTR_ADV(s);
	    }
	എങ്കിൽ (s == ശൂന്യ)		// Not present.
	    പോകുക theend;
    }

    rex.വരി = വരി;
    rex.lnum = 0;
    reg_toolong = FALSE;

    // Simplest case: Anchored match need be tried only once.
    എങ്കിൽ (prog->reganch)
    {
	പൂർണ്ണസംഖ്യ	c;

	എങ്കിൽ (has_mbyte)
	    c = (*mb_ptr2char)(rex.വരി + col);
	അല്ലെങ്കിൽ
	    c = rex.വരി[col];
	എങ്കിൽ (prog->regstart == NUL
		|| prog->regstart == c
		|| (rex.reg_ic
		    && (((enc_utf8 && utf_fold(prog->regstart) == utf_fold(c)))
			|| (c < 255 && prog->regstart < 255 &&
			    MB_TOLOWER(prog->regstart) == MB_TOLOWER(c)))))
	    retval = regtry(prog, col, timed_out);
	അല്ലെങ്കിൽ
	    retval = 0;
    }
    അല്ലെങ്കിൽ
    {
	// Messy cases:  unanchored match.
	ആകുമ്പോൾ (!got_int)
	{
	    എങ്കിൽ (prog->regstart != NUL)
	    {
		// Skip until the char we know it must start with.
		// Used often, do some work to avoid call overhead.
		എങ്കിൽ (!rex.reg_ic && !has_mbyte)
		    s = vim_strbyte(rex.വരി + col, prog->regstart);
		അല്ലെങ്കിൽ
		    s = cstrchr(rex.വരി + col, prog->regstart);
		എങ്കിൽ (s == ശൂന്യ)
		{
		    retval = 0;
		    നിർത്തുക;
		}
		col = (പൂർണ്ണസംഖ്യ)(s - rex.വരി);
	    }

	    // Check for maximum column to try.
	    എങ്കിൽ (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	    {
		retval = 0;
		നിർത്തുക;
	    }

	    retval = regtry(prog, col, timed_out);
	    എങ്കിൽ (retval > 0)
		നിർത്തുക;

	    // if not currently on the first line, get it again
	    എങ്കിൽ (rex.lnum != 0)
	    {
		rex.lnum = 0;
		rex.വരി = reg_getline((linenr_T)0);
	    }
	    എങ്കിൽ (rex.വരി[col] == NUL)
		നിർത്തുക;
	    എങ്കിൽ (has_mbyte)
		col += (*mb_ptr2len)(rex.വരി + col);
	    അല്ലെങ്കിൽ
		++col;
#ifdef FEAT_RELTIME
	    എങ്കിൽ (bt_did_time_out(timed_out))
		നിർത്തുക;
#endif
	}
    }

theend:
    // Free "reg_tofree" when it's a bit big.
    // Free regstack and backpos if they are bigger than their initial size.
    എങ്കിൽ (reg_tofreelen > 400)
	VIM_CLEAR(reg_tofree);
    എങ്കിൽ (regstack.ga_maxlen > REGSTACK_INITIAL)
	ga_clear(&regstack);
    എങ്കിൽ (backpos.ga_maxlen > BACKPOS_INITIAL)
	ga_clear(&backpos);

    എങ്കിൽ (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs
	// and \ze are used.
	എങ്കിൽ (REG_MULTI)
	{
	    lpos_T *start = &rex.reg_mmatch->startpos[0];
	    lpos_T *end = &rex.reg_mmatch->endpos[0];

	    എങ്കിൽ (end->lnum < start->lnum
			|| (end->lnum == start->lnum && end->col < start->col))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_mmatch->rmm_matchcol = col;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (rex.reg_match->endp[0] < rex.reg_match->startp[0])
		rex.reg_match->endp[0] = rex.reg_match->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_match->rm_matchcol = col;
	}
    }

    തിരികെ retval;
}

/*
 * Match a regexp against a string.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 * if "line_lbr" is TRUE  consider a "\n" in "line" to be a line break.
 *
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
bt_regexec_nl(
    regmatch_T	*rmp,
    char_u	*വരി,	// string to match against
    colnr_T	col,	// column to start looking for match
    പൂർണ്ണസംഖ്യ		വരി_lbr)
{
    rex.reg_match = rmp;
    rex.reg_mmatch = ശൂന്യ;
    rex.reg_maxline = 0;
    rex.reg_വരി_lbr = വരി_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = ശൂന്യ;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;

    തിരികെ bt_regexec_both(വരി, col, ശൂന്യ);
}

/*
 * Match a regexp against multiple lines.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 *
 * Return zero if there is no match.  Return number of lines contained in the
 * match otherwise.
 */
    സ്ഥിതം long
bt_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	col,		// column to start looking for match
    പൂർണ്ണസംഖ്യ		*timed_out)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    തിരികെ bt_regexec_both(ശൂന്യ, col, timed_out);
}

/*
 * Compare a number with the operand of RE_LNUM, RE_COL or RE_VCOL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
re_അക്കം_cmp(long_u val, char_u *scan)
{
    long_u  n = OPERAND_MIN(scan);

    എങ്കിൽ (OPERAND_CMP(scan) == '>')
	തിരികെ val > n;
    എങ്കിൽ (OPERAND_CMP(scan) == '<')
	തിരികെ val < n;
    തിരികെ val == n;
}

#ifdef BT_REGEXP_DUMP

/*
 * regdump - dump a regexp onto stdout in vaguely comprehensible form
 */
    സ്ഥിതം ശൂന്യം
regdump(char_u *pattern, bt_regprog_T *r)
{
    char_u  *s;
    പൂർണ്ണസംഖ്യ	    op = EXACTLY;	// Arbitrary non-END op.
    char_u  *next;
    char_u  *end = ശൂന്യ;
    FILE    *f;

# ifdef BT_REGEXP_LOG
    f = fopen("bt_regexp_log.log", "a");
# അല്ലെങ്കിൽ
    f = stdout;
# endif
    എങ്കിൽ (f == ശൂന്യ)
	തിരികെ;
    fprintf(f, "-------------------------------------\n\r\nregcomp(%s):\r\n", pattern);

    s = r->program + 1;
    // Loop until we find the END that isn't before a referred next (an END
    // can also appear in a NOMATCH operand).
    ആകുമ്പോൾ ((op != END || s <= end) && s < r->program + r->regsz)
    {
	op = OP(s);
	fprintf(f, "%2d%s", (പൂർണ്ണസംഖ്യ)(s - r->program), regprop(s)); // Where, what.
	next = (s + 3 <= r->program + r->regsz) ? regnext(s) : ശൂന്യ;
	എങ്കിൽ (next == ശൂന്യ)	// Next ptr.
	    fprintf(f, "(0)");
	അല്ലെങ്കിൽ
	    fprintf(f, "(%d)", (പൂർണ്ണസംഖ്യ)((s - r->program) + (next - s)));
	എങ്കിൽ (end < next)
	    end = next;
	എങ്കിൽ (op == BRACE_LIMITS)
	{
	    // Two ints
	    fprintf(f, " minval %ld, maxval %ld", OPERAND_MIN(s), OPERAND_MAX(s));
	    s += 8;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (op == BEHIND || op == NOBEHIND)
	{
	    // one int
	    fprintf(f, " count %ld", OPERAND_MIN(s));
	    s += 4;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (op == RE_LNUM || op == RE_COL || op == RE_VCOL)
	{
	    // one int plus comparator
	    fprintf(f, " count %ld", OPERAND_MIN(s));
	    s += 5;
	}
	s += 3;
	എങ്കിൽ (op == MULTIBYTECODE)
	{
	    fprintf(f, " mbc=%d", utf_ptr2char(s));
	    s += utfc_ptr2len(s);
	}
	എങ്കിൽ (op == ANYOF || op == ANYOF + ADD_NL
		|| op == ANYBUT || op == ANYBUT + ADD_NL
		|| op == EXACTLY)
	{
	    // Literal string, where present.
	    fprintf(f, "\nxxxxxxxxx\n");
	    ആകുമ്പോൾ (*s != NUL && s < r->program + r->regsz)
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
    എങ്കിൽ (r->regstart != NUL)
	fprintf(f, "start `%s' 0x%x; ", r->regstart < 256
		? (അക്ഷരം *)transchar(r->regstart)
		: "multibyte", r->regstart);
    എങ്കിൽ (r->reganch)
	fprintf(f, "anchored; ");
    എങ്കിൽ (r->regmust != ശൂന്യ)
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
    സ്ഥിതം char_u *
regprop(char_u *op)
{
    അക്ഷരം	    *p;
    സ്ഥിതം അക്ഷരം	    buf[50];
    സ്ഥിതം size_t   buflen = 0;

    STRCPY(buf, ":");
    buflen = 1;

    തിരഞ്ഞെടുപ്പ് ((പൂർണ്ണസംഖ്യ) OP(op))
    {
      അവസ്ഥ BOL:
	p = "BOL";
	നിർത്തുക;
      അവസ്ഥ EOL:
	p = "EOL";
	നിർത്തുക;
      അവസ്ഥ RE_BOF:
	p = "BOF";
	നിർത്തുക;
      അവസ്ഥ RE_EOF:
	p = "EOF";
	നിർത്തുക;
      അവസ്ഥ CURSOR:
	p = "CURSOR";
	നിർത്തുക;
      അവസ്ഥ RE_VISUAL:
	p = "RE_VISUAL";
	നിർത്തുക;
      അവസ്ഥ RE_LNUM:
	p = "RE_LNUM";
	നിർത്തുക;
      അവസ്ഥ RE_MARK:
	p = "RE_MARK";
	നിർത്തുക;
      അവസ്ഥ RE_COL:
	p = "RE_COL";
	നിർത്തുക;
      അവസ്ഥ RE_VCOL:
	p = "RE_VCOL";
	നിർത്തുക;
      അവസ്ഥ BOW:
	p = "BOW";
	നിർത്തുക;
      അവസ്ഥ EOW:
	p = "EOW";
	നിർത്തുക;
      അവസ്ഥ ANY:
	p = "ANY";
	നിർത്തുക;
      അവസ്ഥ ANY + ADD_NL:
	p = "ANY+NL";
	നിർത്തുക;
      അവസ്ഥ ANYOF:
	p = "ANYOF";
	നിർത്തുക;
      അവസ്ഥ ANYOF + ADD_NL:
	p = "ANYOF+NL";
	നിർത്തുക;
      അവസ്ഥ ANYBUT:
	p = "ANYBUT";
	നിർത്തുക;
      അവസ്ഥ ANYBUT + ADD_NL:
	p = "ANYBUT+NL";
	നിർത്തുക;
      അവസ്ഥ IDENT:
	p = "IDENT";
	നിർത്തുക;
      അവസ്ഥ IDENT + ADD_NL:
	p = "IDENT+NL";
	നിർത്തുക;
      അവസ്ഥ SIDENT:
	p = "SIDENT";
	നിർത്തുക;
      അവസ്ഥ SIDENT + ADD_NL:
	p = "SIDENT+NL";
	നിർത്തുക;
      അവസ്ഥ KWORD:
	p = "KWORD";
	നിർത്തുക;
      അവസ്ഥ KWORD + ADD_NL:
	p = "KWORD+NL";
	നിർത്തുക;
      അവസ്ഥ SKWORD:
	p = "SKWORD";
	നിർത്തുക;
      അവസ്ഥ SKWORD + ADD_NL:
	p = "SKWORD+NL";
	നിർത്തുക;
      അവസ്ഥ FNAME:
	p = "FNAME";
	നിർത്തുക;
      അവസ്ഥ FNAME + ADD_NL:
	p = "FNAME+NL";
	നിർത്തുക;
      അവസ്ഥ SFNAME:
	p = "SFNAME";
	നിർത്തുക;
      അവസ്ഥ SFNAME + ADD_NL:
	p = "SFNAME+NL";
	നിർത്തുക;
      അവസ്ഥ PRINT:
	p = "PRINT";
	നിർത്തുക;
      അവസ്ഥ PRINT + ADD_NL:
	p = "PRINT+NL";
	നിർത്തുക;
      അവസ്ഥ SPRINT:
	p = "SPRINT";
	നിർത്തുക;
      അവസ്ഥ SPRINT + ADD_NL:
	p = "SPRINT+NL";
	നിർത്തുക;
      അവസ്ഥ WHITE:
	p = "WHITE";
	നിർത്തുക;
      അവസ്ഥ WHITE + ADD_NL:
	p = "WHITE+NL";
	നിർത്തുക;
      അവസ്ഥ NWHITE:
	p = "NWHITE";
	നിർത്തുക;
      അവസ്ഥ NWHITE + ADD_NL:
	p = "NWHITE+NL";
	നിർത്തുക;
      അവസ്ഥ DIGIT:
	p = "DIGIT";
	നിർത്തുക;
      അവസ്ഥ DIGIT + ADD_NL:
	p = "DIGIT+NL";
	നിർത്തുക;
      അവസ്ഥ NDIGIT:
	p = "NDIGIT";
	നിർത്തുക;
      അവസ്ഥ NDIGIT + ADD_NL:
	p = "NDIGIT+NL";
	നിർത്തുക;
      അവസ്ഥ HEX:
	p = "HEX";
	നിർത്തുക;
      അവസ്ഥ HEX + ADD_NL:
	p = "HEX+NL";
	നിർത്തുക;
      അവസ്ഥ NHEX:
	p = "NHEX";
	നിർത്തുക;
      അവസ്ഥ NHEX + ADD_NL:
	p = "NHEX+NL";
	നിർത്തുക;
      അവസ്ഥ OCTAL:
	p = "OCTAL";
	നിർത്തുക;
      അവസ്ഥ OCTAL + ADD_NL:
	p = "OCTAL+NL";
	നിർത്തുക;
      അവസ്ഥ NOCTAL:
	p = "NOCTAL";
	നിർത്തുക;
      അവസ്ഥ NOCTAL + ADD_NL:
	p = "NOCTAL+NL";
	നിർത്തുക;
      അവസ്ഥ WORD:
	p = "WORD";
	നിർത്തുക;
      അവസ്ഥ WORD + ADD_NL:
	p = "WORD+NL";
	നിർത്തുക;
      അവസ്ഥ NWORD:
	p = "NWORD";
	നിർത്തുക;
      അവസ്ഥ NWORD + ADD_NL:
	p = "NWORD+NL";
	നിർത്തുക;
      അവസ്ഥ HEAD:
	p = "HEAD";
	നിർത്തുക;
      അവസ്ഥ HEAD + ADD_NL:
	p = "HEAD+NL";
	നിർത്തുക;
      അവസ്ഥ NHEAD:
	p = "NHEAD";
	നിർത്തുക;
      അവസ്ഥ NHEAD + ADD_NL:
	p = "NHEAD+NL";
	നിർത്തുക;
      അവസ്ഥ ALPHA:
	p = "ALPHA";
	നിർത്തുക;
      അവസ്ഥ ALPHA + ADD_NL:
	p = "ALPHA+NL";
	നിർത്തുക;
      അവസ്ഥ NALPHA:
	p = "NALPHA";
	നിർത്തുക;
      അവസ്ഥ NALPHA + ADD_NL:
	p = "NALPHA+NL";
	നിർത്തുക;
      അവസ്ഥ LOWER:
	p = "LOWER";
	നിർത്തുക;
      അവസ്ഥ LOWER + ADD_NL:
	p = "LOWER+NL";
	നിർത്തുക;
      അവസ്ഥ NLOWER:
	p = "NLOWER";
	നിർത്തുക;
      അവസ്ഥ NLOWER + ADD_NL:
	p = "NLOWER+NL";
	നിർത്തുക;
      അവസ്ഥ UPPER:
	p = "UPPER";
	നിർത്തുക;
      അവസ്ഥ UPPER + ADD_NL:
	p = "UPPER+NL";
	നിർത്തുക;
      അവസ്ഥ NUPPER:
	p = "NUPPER";
	നിർത്തുക;
      അവസ്ഥ NUPPER + ADD_NL:
	p = "NUPPER+NL";
	നിർത്തുക;
      അവസ്ഥ BRANCH:
	p = "BRANCH";
	നിർത്തുക;
      അവസ്ഥ EXACTLY:
	p = "EXACTLY";
	നിർത്തുക;
      അവസ്ഥ NOTHING:
	p = "NOTHING";
	നിർത്തുക;
      അവസ്ഥ BACK:
	p = "BACK";
	നിർത്തുക;
      അവസ്ഥ END:
	p = "END";
	നിർത്തുക;
      അവസ്ഥ MOPEN + 0:
	p = "MATCH START";
	നിർത്തുക;
      അവസ്ഥ MOPEN + 1:
      അവസ്ഥ MOPEN + 2:
      അവസ്ഥ MOPEN + 3:
      അവസ്ഥ MOPEN + 4:
      അവസ്ഥ MOPEN + 5:
      അവസ്ഥ MOPEN + 6:
      അവസ്ഥ MOPEN + 7:
      അവസ്ഥ MOPEN + 8:
      അവസ്ഥ MOPEN + 9:
	buflen += vim_snprintf(buf + buflen, വലുപ്പം(buf) - buflen, "MOPEN%d", OP(op) - MOPEN);
	p = ശൂന്യ;
	നിർത്തുക;
      അവസ്ഥ MCLOSE + 0:
	p = "MATCH END";
	നിർത്തുക;
      അവസ്ഥ MCLOSE + 1:
      അവസ്ഥ MCLOSE + 2:
      അവസ്ഥ MCLOSE + 3:
      അവസ്ഥ MCLOSE + 4:
      അവസ്ഥ MCLOSE + 5:
      അവസ്ഥ MCLOSE + 6:
      അവസ്ഥ MCLOSE + 7:
      അവസ്ഥ MCLOSE + 8:
      അവസ്ഥ MCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, വലുപ്പം(buf) - buflen, "MCLOSE%d", OP(op) - MCLOSE);
	p = ശൂന്യ;
	നിർത്തുക;
      അവസ്ഥ BACKREF + 1:
      അവസ്ഥ BACKREF + 2:
      അവസ്ഥ BACKREF + 3:
      അവസ്ഥ BACKREF + 4:
      അവസ്ഥ BACKREF + 5:
      അവസ്ഥ BACKREF + 6:
      അവസ്ഥ BACKREF + 7:
      അവസ്ഥ BACKREF + 8:
      അവസ്ഥ BACKREF + 9:
	buflen += vim_snprintf(buf + buflen, വലുപ്പം(buf) - buflen, "BACKREF%d", OP(op) - BACKREF);
	p = ശൂന്യ;
	നിർത്തുക;
      അവസ്ഥ NOPEN:
	p = "NOPEN";
	നിർത്തുക;
      അവസ്ഥ NCLOSE:
	p = "NCLOSE";
	നിർത്തുക;
# ifdef FEAT_SYN_HL
      അവസ്ഥ ZOPEN + 1:
      അവസ്ഥ ZOPEN + 2:
      അവസ്ഥ ZOPEN + 3:
      അവസ്ഥ ZOPEN + 4:
      അവസ്ഥ ZOPEN + 5:
      അവസ്ഥ ZOPEN + 6:
      അവസ്ഥ ZOPEN + 7:
      അവസ്ഥ ZOPEN + 8:
      അവസ്ഥ ZOPEN + 9:
	buflen += vim_snprintf(buf + buflen, വലുപ്പം(buf) - buflen, "ZOPEN%d", OP(op) - ZOPEN);
	p = ശൂന്യ;
	നിർത്തുക;
      അവസ്ഥ ZCLOSE + 1:
      അവസ്ഥ ZCLOSE + 2:
      അവസ്ഥ ZCLOSE + 3:
      അവസ്ഥ ZCLOSE + 4:
      അവസ്ഥ ZCLOSE + 5:
      അവസ്ഥ ZCLOSE + 6:
      അവസ്ഥ ZCLOSE + 7:
      അവസ്ഥ ZCLOSE + 8:
      അവസ്ഥ ZCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, വലുപ്പം(buf) - buflen, "ZCLOSE%d", OP(op) - ZCLOSE);
	p = ശൂന്യ;
	നിർത്തുക;
      അവസ്ഥ ZREF + 1:
      അവസ്ഥ ZREF + 2:
      അവസ്ഥ ZREF + 3:
      അവസ്ഥ ZREF + 4:
      അവസ്ഥ ZREF + 5:
      അവസ്ഥ ZREF + 6:
      അവസ്ഥ ZREF + 7:
      അവസ്ഥ ZREF + 8:
      അവസ്ഥ ZREF + 9:
	buflen += vim_snprintf(buf + buflen, വലുപ്പം(buf) - buflen, "ZREF%d", OP(op) - ZREF);
	p = ശൂന്യ;
	നിർത്തുക;
# endif
      അവസ്ഥ STAR:
	p = "STAR";
	നിർത്തുക;
      അവസ്ഥ PLUS:
	p = "PLUS";
	നിർത്തുക;
      അവസ്ഥ NOMATCH:
	p = "NOMATCH";
	നിർത്തുക;
      അവസ്ഥ MATCH:
	p = "MATCH";
	നിർത്തുക;
      അവസ്ഥ BEHIND:
	p = "BEHIND";
	നിർത്തുക;
      അവസ്ഥ NOBEHIND:
	p = "NOBEHIND";
	നിർത്തുക;
      അവസ്ഥ SUBPAT:
	p = "SUBPAT";
	നിർത്തുക;
      അവസ്ഥ BRACE_LIMITS:
	p = "BRACE_LIMITS";
	നിർത്തുക;
      അവസ്ഥ BRACE_SIMPLE:
	p = "BRACE_SIMPLE";
	നിർത്തുക;
      അവസ്ഥ BRACE_COMPLEX + 0:
      അവസ്ഥ BRACE_COMPLEX + 1:
      അവസ്ഥ BRACE_COMPLEX + 2:
      അവസ്ഥ BRACE_COMPLEX + 3:
      അവസ്ഥ BRACE_COMPLEX + 4:
      അവസ്ഥ BRACE_COMPLEX + 5:
      അവസ്ഥ BRACE_COMPLEX + 6:
      അവസ്ഥ BRACE_COMPLEX + 7:
      അവസ്ഥ BRACE_COMPLEX + 8:
      അവസ്ഥ BRACE_COMPLEX + 9:
	buflen += vim_snprintf(buf + buflen, വലുപ്പം(buf) - buflen, "BRACE_COMPLEX%d", OP(op) - BRACE_COMPLEX);
	p = ശൂന്യ;
	നിർത്തുക;
      അവസ്ഥ MULTIBYTECODE:
	p = "MULTIBYTECODE";
	നിർത്തുക;
      അവസ്ഥ NEWL:
	p = "NEWL";
	നിർത്തുക;
      സ്ഥിരസ്ഥിതി:
	buflen += vim_snprintf(buf + buflen, വലുപ്പം(buf) - buflen, "corrupt %d", OP(op));
	p = ശൂന്യ;
	നിർത്തുക;
    }
    എങ്കിൽ (p != ശൂന്യ)
	vim_strncpy((char_u *)buf + buflen, (char_u *)p, വലുപ്പം(buf) - buflen - 1);
    തിരികെ (char_u *)buf;
}
#endif	    // DEBUG
