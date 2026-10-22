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

static ཧྲིལ	num_complex_braces; // Complex \{...} count
static char_u	*regcode;	// Code-emit pointer, or JUST_CALC_SIZE
static long	regsize;	// Code size.
static ཧྲིལ	reg_toolong;	// TRUE when offset out of range
static ཧྲིལ	bt_reg_parse_depth;	// nesting depth in reg()
static char_u	had_endbrace[NSUBEXP];	// flags, TRUE if end of () found
static long	brace_min[10];	// Minimums for complex brace repeats
static long	brace_max[10];	// Maximums for complex brace repeats
static ཧྲིལ	brace_count[10]; // Current counts for complex brace repeats
static ཧྲིལ	one_exactly = FALSE;	// only do one char for EXACTLY

// When making changes to classchars also change nfa_classcodes.
static char_u	*classchars = (char_u *)".iIkKfFpPsSdDxXoOwWhHaAlLuU";
static const ཧྲིལ classcodes[] = {
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
    static ཧྲིལ
bt_recognize_char_class(char_u *start, char_u *end)
{
    ཧྲིལ		newl;
    ཧྲིལ		config = get_char_class_bits(start, end, &newl);

    // A newline would need the ADD_NL variant, don't bother with it here.
    གལ་ཏེ (config < 0 || newl)
	ཕྱིར 0;

	// The old engine has no case-insensitive class opcode, so [a-z] and [A-Z]
	// are left as collections.  The classes below are case-independent.
    switch (config)
    {
	pak_sa CLASS_o9:
	    ཕྱིར DIGIT;
	pak_sa CLASS_not | CLASS_o9:
	    ཕྱིར NDIGIT;
	pak_sa CLASS_af | CLASS_AF | CLASS_o9:
	    ཕྱིར HEX;
	pak_sa CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    ཕྱིར NHEX;
	pak_sa CLASS_o7:
	    ཕྱིར OCTAL;
	pak_sa CLASS_not | CLASS_o7:
	    ཕྱིར NOCTAL;
	pak_sa CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    ཕྱིར WORD;
	pak_sa CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    ཕྱིར NWORD;
	pak_sa CLASS_az | CLASS_AZ | CLASS_underscore:
	    ཕྱིར HEAD;
	pak_sa CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    ཕྱིར NHEAD;
	pak_sa CLASS_az | CLASS_AZ:
	    ཕྱིར ALPHA;
	pak_sa CLASS_not | CLASS_az | CLASS_AZ:
	    ཕྱིར NALPHA;
    }
    ཕྱིར 0;
}

/*
 * When regcode is set to this value, code is not emitted and size is computed
 * instead.
 */
#define JUST_CALC_SIZE	((char_u *) -1)

// Values for rs_state in regitem_T.
typedef ga_nanaa regstate_E
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
typedef sa_nracanaa
{
    sa_ngha
    {
	char_u	*ptr;	// rex.input pointer, for single-line regexp
	lpos_T	pos;	// rex.input pos, for multi-line regexp
    } rs_u;
    ཧྲིལ		rs_len;
} regsave_T;

// struct to save start/end pointer/position in for \(\)
typedef sa_nracanaa
{
    sa_ngha
    {
	char_u	*ptr;
	lpos_T	pos;
    } se_u;
} save_se_T;

// used for BEHIND and NOBEHIND matching
typedef sa_nracanaa regbehind_S
{
    regsave_T	save_after;
    regsave_T	save_behind;
    ཧྲིལ		save_need_clear_subexpr;
    save_se_T   save_start[NSUBEXP];
    save_se_T   save_end[NSUBEXP];
} regbehind_T;

/*
 * When there are alternatives a regstate_T is put on the regstack to remember
 * what we are doing.
 * Before it may be another type of item, depending on rs_state, to remember
 * more things.
 */
typedef sa_nracanaa regitem_S
{
    regstate_T	rs_state;	// what we are doing, one of RS_ above
    short	rs_no;		// submatch nr or BEHIND/NOBEHIND
    char_u	*rs_scan;	// current node in program
    sa_ngha
    {
	save_se_T  sesave;
	regsave_T  regsave;
    } rs_un;			// room for saving rex.input
} regitem_T;


// used for STAR, PLUS and BRACE_SIMPLE matching
typedef sa_nracanaa regstar_S
{
    ཧྲིལ		nextb;		// next byte
    ཧྲིལ		nextb_ic;	// next byte reverse case
    long	count;
    long	minval;
    long	maxval;
} regstar_T;

// used to store input position when a BACK was encountered, so that we now if
// we made any progress since the last time.
typedef sa_nracanaa backpos_S
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
static garray_T	regstack = {0, 0, 0, 0, NULL};
static garray_T	backpos = {0, 0, 0, 0, NULL};

static regsave_T behind_pos;

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
#define OP(p)		((ཧྲིལ)*(p))
#define NEXT(p)		(((*((p) + 1) & 0377) << 8) + (*((p) + 2) & 0377))
#define OPERAND(p)	((p) + 3)
// Obtain an operand that was stored as four bytes, MSB first.
#define OPERAND_MIN(p)	(((long)(p)[3] << 24) + ((long)(p)[4] << 16) \
			+ ((long)(p)[5] << 8) + (long)(p)[6])
// Obtain a second operand stored as four bytes.
#define OPERAND_MAX(p)	OPERAND_MIN((p) + 4)
// Obtain a second single-byte operand stored after a four bytes operand.
#define OPERAND_CMP(p)	(p)[7]

static char_u *reg(ཧྲིལ paren, ཧྲིལ *flagp);

#ifdef BT_REGEXP_DUMP
static void	regdump(char_u *, bt_regprog_T *);
#endif

static ཧྲིལ	re_num_cmp(long_u val, char_u *scan);

#ifdef DEBUG
static char_u	*regprop(char_u *);

static ཧྲིལ	regnarrate = 0;
#endif


/*
 * Setup to parse the regexp.  Used once to get the length and once to do it.
 */
    static void
regcomp_start(
    char_u	*expr,
    ཧྲིལ		re_flags)	    // see vim_regcomp()
{
    initchr(expr);
    གལ་ཏེ (re_flags & RE_MAGIC)
	reg_magic = MAGIC_ON;
    དེ་མིན
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
#གལ་ཏེ defined(FEAT_SYN_HL)
    had_eol = FALSE;
#endif
}

/*
 * Return TRUE if MULTIBYTECODE should be used instead of EXACTLY for
 * character "c".
 */
    static ཧྲིལ
use_multibytecode(ཧྲིལ c)
{
    ཕྱིར has_mbyte && (*mb_char2len)(c) > 1
		     && (re_multi_type(peekchr()) != NOT_MULTI
			     || (enc_utf8 && utf_iscomposing(c)));
}

/*
 * Emit (if appropriate) a byte of code
 */
    static void
regc(ཧྲིལ b)
{
    གལ་ཏེ (regcode == JUST_CALC_SIZE)
	regsize++;
    དེ་མིན
	*regcode++ = b;
}

/*
 * Emit (if appropriate) a multi-byte character of code
 */
    static void
regmbc(ཧྲིལ c)
{
    གལ་ཏེ (!has_mbyte && c > 0xff)
	ཕྱིར;
    གལ་ཏེ (regcode == JUST_CALC_SIZE)
	regsize += (*mb_char2len)(c);
    དེ་མིན
	regcode += (*mb_char2bytes)(c, regcode);
}


/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * NOTE: When changing this function, also change nfa_emit_equi_class()
 */
    static void
reg_equi_class(ཧྲིལ c)
{
    གལ་ཏེ (enc_utf8 || STRCMP(p_enc, "latin1") == 0
					 || STRCMP(p_enc, "iso-8859-15") == 0)
    {
	switch (c)
	{
	    // Do not use '\300' style, it results in a negative number.
	    pak_sa 'A': pak_sa 0xc0: pak_sa 0xc1: pak_sa 0xc2: pak_sa 0xc3: pak_sa 0xc4:
	    pak_sa 0xc5: pak_sa 0x100: pak_sa 0x102: pak_sa 0x104: pak_sa 0x1cd:
	    pak_sa 0x1de: pak_sa 0x1e0: pak_sa 0x1fa: pak_sa 0x202: pak_sa 0x226:
	    pak_sa 0x23a: pak_sa 0x1e00: pak_sa 0x1ea0: pak_sa 0x1ea2: pak_sa 0x1ea4:
	    pak_sa 0x1ea6: pak_sa 0x1ea8: pak_sa 0x1eaa: pak_sa 0x1eac: pak_sa 0x1eae:
	    pak_sa 0x1eb0: pak_sa 0x1eb2: pak_sa 0x1eb4: pak_sa 0x1eb6:
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
		      ཕྱིར;
	    pak_sa 'B': pak_sa 0x181: pak_sa 0x243: pak_sa 0x1e02:
	    pak_sa 0x1e04: pak_sa 0x1e06:
		      regmbc('B');
		      regmbc(0x181); regmbc(0x243); regmbc(0x1e02);
		      regmbc(0x1e04); regmbc(0x1e06);
		      ཕྱིར;
	    pak_sa 'C': pak_sa 0xc7:
	    pak_sa 0x106: pak_sa 0x108: pak_sa 0x10a: pak_sa 0x10c: pak_sa 0x187:
	    pak_sa 0x23b: pak_sa 0x1e08: pak_sa 0xa792:
		      regmbc('C'); regmbc(0xc7);
		      regmbc(0x106); regmbc(0x108); regmbc(0x10a);
		      regmbc(0x10c); regmbc(0x187); regmbc(0x23b);
		      regmbc(0x1e08); regmbc(0xa792);
		      ཕྱིར;
	    pak_sa 'D': pak_sa 0x10e: pak_sa 0x110: pak_sa 0x18a:
	    pak_sa 0x1e0a: pak_sa 0x1e0c: pak_sa 0x1e0e: pak_sa 0x1e10:
	    pak_sa 0x1e12:
		      regmbc('D'); regmbc(0x10e); regmbc(0x110);
		      regmbc(0x18a); regmbc(0x1e0a); regmbc(0x1e0c);
		      regmbc(0x1e0e); regmbc(0x1e10); regmbc(0x1e12);
		      ཕྱིར;
	    pak_sa 'E': pak_sa 0xc8: pak_sa 0xc9: pak_sa 0xca: pak_sa 0xcb:
	    pak_sa 0x112: pak_sa 0x114: pak_sa 0x116: pak_sa 0x118: pak_sa 0x11a:
	    pak_sa 0x204: pak_sa 0x206: pak_sa 0x228: pak_sa 0x246: pak_sa 0x1e14:
	    pak_sa 0x1e16: pak_sa 0x1e18: pak_sa 0x1e1a: pak_sa 0x1e1c:
	    pak_sa 0x1eb8: pak_sa 0x1eba: pak_sa 0x1ebc: pak_sa 0x1ebe:
	    pak_sa 0x1ec0: pak_sa 0x1ec2: pak_sa 0x1ec4: pak_sa 0x1ec6:
		      regmbc('E'); regmbc(0xc8); regmbc(0xc9);
		      regmbc(0xca); regmbc(0xcb); regmbc(0x112);
		      regmbc(0x114); regmbc(0x116); regmbc(0x118);
		      regmbc(0x11a); regmbc(0x204); regmbc(0x206);
		      regmbc(0x228); regmbc(0x246); regmbc(0x1e14);
		      regmbc(0x1e16); regmbc(0x1e18); regmbc(0x1e1a);
		      regmbc(0x1e1c); regmbc(0x1eb8); regmbc(0x1eba);
		      regmbc(0x1ebc); regmbc(0x1ebe); regmbc(0x1ec0);
		      regmbc(0x1ec2); regmbc(0x1ec4); regmbc(0x1ec6);
		      ཕྱིར;
	    pak_sa 'F': pak_sa 0x191: pak_sa 0x1e1e: pak_sa 0xa798:
		      regmbc('F'); regmbc(0x191); regmbc(0x1e1e);
		      regmbc(0xa798);
		      ཕྱིར;
	    pak_sa 'G': pak_sa 0x11c: pak_sa 0x11e: pak_sa 0x120:
	    pak_sa 0x122: pak_sa 0x193: pak_sa 0x1e4: pak_sa 0x1e6:
	    pak_sa 0x1f4: pak_sa 0x1e20: pak_sa 0xa7a0:
		      regmbc('G'); regmbc(0x11c); regmbc(0x11e);
		      regmbc(0x120); regmbc(0x122); regmbc(0x193);
		      regmbc(0x1e4); regmbc(0x1e6); regmbc(0x1f4);
		      regmbc(0x1e20); regmbc(0xa7a0);
		      ཕྱིར;
	    pak_sa 'H': pak_sa 0x124: pak_sa 0x126: pak_sa 0x21e:
	    pak_sa 0x1e22: pak_sa 0x1e24: pak_sa 0x1e26:
	    pak_sa 0x1e28: pak_sa 0x1e2a: pak_sa 0x2c67:
		      regmbc('H'); regmbc(0x124); regmbc(0x126);
		      regmbc(0x21e); regmbc(0x1e22); regmbc(0x1e24);
		      regmbc(0x1e26); regmbc(0x1e28); regmbc(0x1e2a);
		      regmbc(0x2c67);
		      ཕྱིར;
	    pak_sa 'I': pak_sa 0xcc: pak_sa 0xcd: pak_sa 0xce: pak_sa 0xcf:
	    pak_sa 0x128: pak_sa 0x12a: pak_sa 0x12c: pak_sa 0x12e:
	    pak_sa 0x130: pak_sa 0x197: pak_sa 0x1cf: pak_sa 0x208:
	    pak_sa 0x20a: pak_sa 0x1e2c: pak_sa 0x1e2e: pak_sa 0x1ec8:
	    pak_sa 0x1eca:
		      regmbc('I'); regmbc(0xcc); regmbc(0xcd);
		      regmbc(0xce); regmbc(0xcf); regmbc(0x128);
		      regmbc(0x12a); regmbc(0x12c); regmbc(0x12e);
		      regmbc(0x130); regmbc(0x197); regmbc(0x1cf);
		      regmbc(0x208); regmbc(0x20a); regmbc(0x1e2c);
		      regmbc(0x1e2e); regmbc(0x1ec8); regmbc(0x1eca);
		      ཕྱིར;
	    pak_sa 'J': pak_sa 0x134: pak_sa 0x248:
		      regmbc('J'); regmbc(0x134); regmbc(0x248);
		      ཕྱིར;
	    pak_sa 'K': pak_sa 0x136: pak_sa 0x198: pak_sa 0x1e8: pak_sa 0x1e30:
	    pak_sa 0x1e32: pak_sa 0x1e34: pak_sa 0x2c69: pak_sa 0xa740:
		      regmbc('K'); regmbc(0x136); regmbc(0x198);
		      regmbc(0x1e8); regmbc(0x1e30); regmbc(0x1e32);
		      regmbc(0x1e34); regmbc(0x2c69); regmbc(0xa740);
		      ཕྱིར;
	    pak_sa 'L': pak_sa 0x139: pak_sa 0x13b: pak_sa 0x13d: pak_sa 0x13f:
	    pak_sa 0x141: pak_sa 0x23d: pak_sa 0x1e36: pak_sa 0x1e38:
	    pak_sa 0x1e3a: pak_sa 0x1e3c: pak_sa 0x2c60:
		      regmbc('L'); regmbc(0x139); regmbc(0x13b);
		      regmbc(0x13d); regmbc(0x13f); regmbc(0x141);
		      regmbc(0x23d); regmbc(0x1e36); regmbc(0x1e38);
		      regmbc(0x1e3a); regmbc(0x1e3c); regmbc(0x2c60);
		      ཕྱིར;
	    pak_sa 'M': pak_sa 0x1e3e: pak_sa 0x1e40: pak_sa 0x1e42:
		      regmbc('M'); regmbc(0x1e3e); regmbc(0x1e40);
		      regmbc(0x1e42);
		      ཕྱིར;
	    pak_sa 'N': pak_sa 0xd1:
	    pak_sa 0x143: pak_sa 0x145: pak_sa 0x147: pak_sa 0x1f8:
	    pak_sa 0x1e44: pak_sa 0x1e46: pak_sa 0x1e48: pak_sa 0x1e4a:
	    pak_sa 0xa7a4:
		      regmbc('N'); regmbc(0xd1);
		      regmbc(0x143); regmbc(0x145); regmbc(0x147);
		      regmbc(0x1f8); regmbc(0x1e44); regmbc(0x1e46);
		      regmbc(0x1e48); regmbc(0x1e4a); regmbc(0xa7a4);
		      ཕྱིར;
	    pak_sa 'O': pak_sa 0xd2: pak_sa 0xd3: pak_sa 0xd4: pak_sa 0xd5: pak_sa 0xd6:
	    pak_sa 0xd8: pak_sa 0x14c: pak_sa 0x14e: pak_sa 0x150: pak_sa 0x19f:
	    pak_sa 0x1a0: pak_sa 0x1d1: pak_sa 0x1ea: pak_sa 0x1ec: pak_sa 0x1fe:
	    pak_sa 0x20c: pak_sa 0x20e: pak_sa 0x22a: pak_sa 0x22c: pak_sa 0x22e:
	    pak_sa 0x230: pak_sa 0x1e4c: pak_sa 0x1e4e: pak_sa 0x1e50: pak_sa 0x1e52:
	    pak_sa 0x1ecc: pak_sa 0x1ece: pak_sa 0x1ed0: pak_sa 0x1ed2: pak_sa 0x1ed4:
	    pak_sa 0x1ed6: pak_sa 0x1ed8: pak_sa 0x1eda: pak_sa 0x1edc: pak_sa 0x1ede:
	    pak_sa 0x1ee0: pak_sa 0x1ee2:
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
		      ཕྱིར;
	    pak_sa 'P': pak_sa 0x1a4: pak_sa 0x1e54: pak_sa 0x1e56: pak_sa 0x2c63:
		      regmbc('P'); regmbc(0x1a4); regmbc(0x1e54);
		      regmbc(0x1e56); regmbc(0x2c63);
		      ཕྱིར;
	    pak_sa 'Q': pak_sa 0x24a:
		      regmbc('Q'); regmbc(0x24a);
		      ཕྱིར;
	    pak_sa 'R': pak_sa 0x154: pak_sa 0x156: pak_sa 0x158: pak_sa 0x210:
	    pak_sa 0x212: pak_sa 0x24c: pak_sa 0x1e58: pak_sa 0x1e5a:
	    pak_sa 0x1e5c: pak_sa 0x1e5e: pak_sa 0x2c64: pak_sa 0xa7a6:
		      regmbc('R'); regmbc(0x154); regmbc(0x156);
		      regmbc(0x210); regmbc(0x212); regmbc(0x158);
		      regmbc(0x24c); regmbc(0x1e58); regmbc(0x1e5a);
		      regmbc(0x1e5c); regmbc(0x1e5e); regmbc(0x2c64);
		      regmbc(0xa7a6);
		      ཕྱིར;
	    pak_sa 'S': pak_sa 0x15a: pak_sa 0x15c: pak_sa 0x15e: pak_sa 0x160:
	    pak_sa 0x218: pak_sa 0x1e60: pak_sa 0x1e62: pak_sa 0x1e64:
	    pak_sa 0x1e66: pak_sa 0x1e68: pak_sa 0x2c7e: pak_sa 0xa7a8:
		      regmbc('S'); regmbc(0x15a); regmbc(0x15c);
		      regmbc(0x15e); regmbc(0x160); regmbc(0x218);
		      regmbc(0x1e60); regmbc(0x1e62); regmbc(0x1e64);
		      regmbc(0x1e66); regmbc(0x1e68); regmbc(0x2c7e);
			  regmbc(0xa7a8);
		      ཕྱིར;
	    pak_sa 'T': pak_sa 0x162: pak_sa 0x164: pak_sa 0x166: pak_sa 0x1ac:
	    pak_sa 0x1ae: pak_sa 0x21a: pak_sa 0x23e: pak_sa 0x1e6a: pak_sa 0x1e6c:
	    pak_sa 0x1e6e: pak_sa 0x1e70:
		      regmbc('T'); regmbc(0x162); regmbc(0x164);
		      regmbc(0x166); regmbc(0x1ac); regmbc(0x23e);
		      regmbc(0x1ae); regmbc(0x21a); regmbc(0x1e6a);
		      regmbc(0x1e6c); regmbc(0x1e6e); regmbc(0x1e70);
		      ཕྱིར;
	    pak_sa 'U': pak_sa 0xd9: pak_sa 0xda: pak_sa 0xdb: pak_sa 0xdc:
	    pak_sa 0x168: pak_sa 0x16a: pak_sa 0x16c: pak_sa 0x16e:
	    pak_sa 0x170: pak_sa 0x172: pak_sa 0x1af: pak_sa 0x1d3:
	    pak_sa 0x1d5: pak_sa 0x1d7: pak_sa 0x1d9: pak_sa 0x1db:
	    pak_sa 0x214: pak_sa 0x216: pak_sa 0x244: pak_sa 0x1e72:
	    pak_sa 0x1e74: pak_sa 0x1e76: pak_sa 0x1e78: pak_sa 0x1e7a:
	    pak_sa 0x1ee4: pak_sa 0x1ee6: pak_sa 0x1ee8: pak_sa 0x1eea:
	    pak_sa 0x1eec: pak_sa 0x1eee: pak_sa 0x1ef0:
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
		      ཕྱིར;
	    pak_sa 'V': pak_sa 0x1b2: pak_sa 0x1e7c: pak_sa 0x1e7e:
		      regmbc('V'); regmbc(0x1b2); regmbc(0x1e7c);
		      regmbc(0x1e7e);
		      ཕྱིར;
	    pak_sa 'W': pak_sa 0x174: pak_sa 0x1e80: pak_sa 0x1e82:
	    pak_sa 0x1e84: pak_sa 0x1e86: pak_sa 0x1e88:
		      regmbc('W'); regmbc(0x174); regmbc(0x1e80);
		      regmbc(0x1e82); regmbc(0x1e84); regmbc(0x1e86);
		      regmbc(0x1e88);
		      ཕྱིར;
	    pak_sa 'X': pak_sa 0x1e8a: pak_sa 0x1e8c:
		      regmbc('X'); regmbc(0x1e8a); regmbc(0x1e8c);
		      ཕྱིར;
	    pak_sa 'Y': pak_sa 0xdd:
	    pak_sa 0x176: pak_sa 0x178: pak_sa 0x1b3: pak_sa 0x232: pak_sa 0x24e:
	    pak_sa 0x1e8e: pak_sa 0x1ef2: pak_sa 0x1ef6: pak_sa 0x1ef4: pak_sa 0x1ef8:
		      regmbc('Y'); regmbc(0xdd); regmbc(0x176);
		      regmbc(0x178); regmbc(0x1b3); regmbc(0x232);
		      regmbc(0x24e); regmbc(0x1e8e); regmbc(0x1ef2);
		      regmbc(0x1ef4); regmbc(0x1ef6); regmbc(0x1ef8);
		      ཕྱིར;
	    pak_sa 'Z': pak_sa 0x179: pak_sa 0x17b: pak_sa 0x17d: pak_sa 0x1b5:
	    pak_sa 0x1e90: pak_sa 0x1e92: pak_sa 0x1e94: pak_sa 0x2c6b:
		      regmbc('Z'); regmbc(0x179); regmbc(0x17b);
		      regmbc(0x17d); regmbc(0x1b5); regmbc(0x1e90);
		      regmbc(0x1e92); regmbc(0x1e94); regmbc(0x2c6b);
		      ཕྱིར;
	    pak_sa 'a': pak_sa 0xe0: pak_sa 0xe1: pak_sa 0xe2:
	    pak_sa 0xe3: pak_sa 0xe4: pak_sa 0xe5: pak_sa 0x101: pak_sa 0x103:
	    pak_sa 0x105: pak_sa 0x1ce: pak_sa 0x1df: pak_sa 0x1e1: pak_sa 0x1fb:
	    pak_sa 0x201: pak_sa 0x203: pak_sa 0x227: pak_sa 0x1d8f: pak_sa 0x1e01:
	    pak_sa 0x1e9a: pak_sa 0x1ea1: pak_sa 0x1ea3: pak_sa 0x1ea5:
	    pak_sa 0x1ea7: pak_sa 0x1ea9: pak_sa 0x1eab: pak_sa 0x1ead:
	    pak_sa 0x1eaf: pak_sa 0x1eb1: pak_sa 0x1eb3: pak_sa 0x1eb5:
	    pak_sa 0x1eb7: pak_sa 0x2c65:
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
		      ཕྱིར;
	    pak_sa 'b': pak_sa 0x180: pak_sa 0x253: pak_sa 0x1d6c: pak_sa 0x1d80:
	    pak_sa 0x1e03: pak_sa 0x1e05: pak_sa 0x1e07:
		      regmbc('b');
		      regmbc(0x180); regmbc(0x253); regmbc(0x1d6c);
		      regmbc(0x1d80); regmbc(0x1e03); regmbc(0x1e05);
		      regmbc(0x1e07);
		      ཕྱིར;
	    pak_sa 'c': pak_sa 0xe7:
	    pak_sa 0x107: pak_sa 0x109: pak_sa 0x10b: pak_sa 0x10d: pak_sa 0x188:
	    pak_sa 0x23c: pak_sa 0x1e09: pak_sa 0xa793: pak_sa 0xa794:
		      regmbc('c'); regmbc(0xe7); regmbc(0x107);
		      regmbc(0x109); regmbc(0x10b); regmbc(0x10d);
		      regmbc(0x188); regmbc(0x23c); regmbc(0x1e09);
		      regmbc(0xa793); regmbc(0xa794);
		      ཕྱིར;
	    pak_sa 'd': pak_sa 0x10f: pak_sa 0x111: pak_sa 0x257: pak_sa 0x1d6d:
	    pak_sa 0x1d81: pak_sa 0x1d91: pak_sa 0x1e0b: pak_sa 0x1e0d:
	    pak_sa 0x1e0f: pak_sa 0x1e11: pak_sa 0x1e13:
		      regmbc('d'); regmbc(0x10f); regmbc(0x111);
		      regmbc(0x257); regmbc(0x1d6d); regmbc(0x1d81);
		      regmbc(0x1d91); regmbc(0x1e0b); regmbc(0x1e0d);
		      regmbc(0x1e0f); regmbc(0x1e11); regmbc(0x1e13);
		      ཕྱིར;
	    pak_sa 'e': pak_sa 0xe8: pak_sa 0xe9: pak_sa 0xea: pak_sa 0xeb:
	    pak_sa 0x113: pak_sa 0x115: pak_sa 0x117: pak_sa 0x119:
	    pak_sa 0x11b: pak_sa 0x205: pak_sa 0x207: pak_sa 0x229:
	    pak_sa 0x247: pak_sa 0x1d92: pak_sa 0x1e15: pak_sa 0x1e17:
	    pak_sa 0x1e19: pak_sa 0x1e1b: pak_sa 0x1eb9: pak_sa 0x1ebb:
	    pak_sa 0x1e1d: pak_sa 0x1ebd: pak_sa 0x1ebf: pak_sa 0x1ec1:
	    pak_sa 0x1ec3: pak_sa 0x1ec5: pak_sa 0x1ec7:
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
		      ཕྱིར;
	    pak_sa 'f': pak_sa 0x192: pak_sa 0x1d6e: pak_sa 0x1d82:
	    pak_sa 0x1e1f: pak_sa 0xa799:
		     regmbc('f'); regmbc(0x192); regmbc(0x1d6e);
		     regmbc(0x1d82); regmbc(0x1e1f); regmbc(0xa799);
		     ཕྱིར;
	    pak_sa 'g': pak_sa 0x11d: pak_sa 0x11f: pak_sa 0x121: pak_sa 0x123:
	    pak_sa 0x1e5: pak_sa 0x1e7: pak_sa 0x260: pak_sa 0x1f5: pak_sa 0x1d83:
	    pak_sa 0x1e21: pak_sa 0xa7a1:
		      regmbc('g'); regmbc(0x11d); regmbc(0x11f);
		      regmbc(0x121); regmbc(0x123); regmbc(0x1e5);
		      regmbc(0x1e7); regmbc(0x1f5); regmbc(0x260);
		      regmbc(0x1d83); regmbc(0x1e21); regmbc(0xa7a1);
		      ཕྱིར;
	    pak_sa 'h': pak_sa 0x125: pak_sa 0x127: pak_sa 0x21f: pak_sa 0x1e23:
	    pak_sa 0x1e25: pak_sa 0x1e27: pak_sa 0x1e29: pak_sa 0x1e2b:
	    pak_sa 0x1e96: pak_sa 0x2c68: pak_sa 0xa795:
		      regmbc('h'); regmbc(0x125); regmbc(0x127);
		      regmbc(0x21f); regmbc(0x1e23); regmbc(0x1e25);
		      regmbc(0x1e27); regmbc(0x1e29); regmbc(0x1e2b);
		      regmbc(0x1e96); regmbc(0x2c68); regmbc(0xa795);
		      ཕྱིར;
	    pak_sa 'i': pak_sa 0xec: pak_sa 0xed: pak_sa 0xee: pak_sa 0xef:
	    pak_sa 0x129: pak_sa 0x12b: pak_sa 0x12d: pak_sa 0x12f:
	    pak_sa 0x1d0: pak_sa 0x209: pak_sa 0x20b: pak_sa 0x268:
	    pak_sa 0x1d96: pak_sa 0x1e2d: pak_sa 0x1e2f: pak_sa 0x1ec9:
	    pak_sa 0x1ecb:
		      regmbc('i'); regmbc(0xec); regmbc(0xed);
		      regmbc(0xee); regmbc(0xef); regmbc(0x129);
		      regmbc(0x12b); regmbc(0x12d); regmbc(0x12f);
		      regmbc(0x1d0); regmbc(0x209); regmbc(0x20b);
		      regmbc(0x268); regmbc(0x1d96); regmbc(0x1e2d);
		      regmbc(0x1e2f); regmbc(0x1ec9); regmbc(0x1ecb);
		      ཕྱིར;
	    pak_sa 'j': pak_sa 0x135: pak_sa 0x1f0: pak_sa 0x249:
		      regmbc('j'); regmbc(0x135); regmbc(0x1f0);
		      regmbc(0x249);
		      ཕྱིར;
	    pak_sa 'k': pak_sa 0x137: pak_sa 0x199: pak_sa 0x1e9:
	    pak_sa 0x1d84: pak_sa 0x1e31: pak_sa 0x1e33: pak_sa 0x1e35:
	    pak_sa 0x2c6a: pak_sa 0xa741:
		      regmbc('k'); regmbc(0x137); regmbc(0x199);
		      regmbc(0x1e9); regmbc(0x1d84); regmbc(0x1e31);
		      regmbc(0x1e33); regmbc(0x1e35); regmbc(0x2c6a);
		      regmbc(0xa741);
		      ཕྱིར;
	    pak_sa 'l': pak_sa 0x13a: pak_sa 0x13c: pak_sa 0x13e:
	    pak_sa 0x140: pak_sa 0x142: pak_sa 0x19a: pak_sa 0x1e37:
	    pak_sa 0x1e39: pak_sa 0x1e3b: pak_sa 0x1e3d: pak_sa 0x2c61:
		      regmbc('l'); regmbc(0x13a); regmbc(0x13c);
		      regmbc(0x13e); regmbc(0x140); regmbc(0x142);
		      regmbc(0x19a); regmbc(0x1e37); regmbc(0x1e39);
		      regmbc(0x1e3b); regmbc(0x1e3d); regmbc(0x2c61);
		      ཕྱིར;
	    pak_sa 'm': pak_sa 0x1d6f: pak_sa 0x1e3f: pak_sa 0x1e41: pak_sa 0x1e43:
		      regmbc('m'); regmbc(0x1d6f); regmbc(0x1e3f);
		      regmbc(0x1e41); regmbc(0x1e43);
		      ཕྱིར;
	    pak_sa 'n': pak_sa 0xf1: pak_sa 0x144: pak_sa 0x146: pak_sa 0x148:
	    pak_sa 0x149: pak_sa 0x1f9: pak_sa 0x1d70: pak_sa 0x1d87:
	    pak_sa 0x1e45: pak_sa 0x1e47: pak_sa 0x1e49: pak_sa 0x1e4b:
	    pak_sa 0xa7a5:
		      regmbc('n'); regmbc(0xf1); regmbc(0x144);
		      regmbc(0x146); regmbc(0x148); regmbc(0x149);
		      regmbc(0x1f9); regmbc(0x1d70); regmbc(0x1d87);
		      regmbc(0x1e45); regmbc(0x1e47); regmbc(0x1e49);
		      regmbc(0x1e4b); regmbc(0xa7a5);
		      ཕྱིར;
	    pak_sa 'o': pak_sa 0xf2: pak_sa 0xf3: pak_sa 0xf4: pak_sa 0xf5:
	    pak_sa 0xf6: pak_sa 0xf8: pak_sa 0x14d: pak_sa 0x14f: pak_sa 0x151:
	    pak_sa 0x1a1: pak_sa 0x1d2: pak_sa 0x1eb: pak_sa 0x1ed: pak_sa 0x1ff:
	    pak_sa 0x20d: pak_sa 0x20f: pak_sa 0x22b: pak_sa 0x22d: pak_sa 0x22f:
	    pak_sa 0x231: pak_sa 0x275: pak_sa 0x1e4d: pak_sa 0x1e4f:
	    pak_sa 0x1e51: pak_sa 0x1e53: pak_sa 0x1ecd: pak_sa 0x1ecf:
	    pak_sa 0x1ed1: pak_sa 0x1ed3: pak_sa 0x1ed5: pak_sa 0x1ed7:
	    pak_sa 0x1ed9: pak_sa 0x1edb: pak_sa 0x1edd: pak_sa 0x1edf:
	    pak_sa 0x1ee1: pak_sa 0x1ee3:
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
		      ཕྱིར;
	    pak_sa 'p': pak_sa 0x1a5: pak_sa 0x1d71: pak_sa 0x1d88: pak_sa 0x1d7d:
	    pak_sa 0x1e55: pak_sa 0x1e57:
		      regmbc('p'); regmbc(0x1a5); regmbc(0x1d71);
		      regmbc(0x1d7d); regmbc(0x1d88); regmbc(0x1e55);
		      regmbc(0x1e57);
		      ཕྱིར;
	    pak_sa 'q': pak_sa 0x24b: pak_sa 0x2a0:
		      regmbc('q'); regmbc(0x24b); regmbc(0x2a0);
		      ཕྱིར;
	    pak_sa 'r': pak_sa 0x155: pak_sa 0x157: pak_sa 0x159: pak_sa 0x211:
	    pak_sa 0x213: pak_sa 0x24d: pak_sa 0x27d: pak_sa 0x1d72: pak_sa 0x1d73:
	    pak_sa 0x1d89: pak_sa 0x1e59: pak_sa 0x1e5b: pak_sa 0x1e5d: pak_sa 0x1e5f:
	    pak_sa 0xa7a7:
		      regmbc('r'); regmbc(0x155); regmbc(0x157);
		      regmbc(0x159); regmbc(0x211); regmbc(0x213);
		      regmbc(0x24d); regmbc(0x1d72); regmbc(0x1d73);
		      regmbc(0x1d89); regmbc(0x1e59); regmbc(0x27d);
		      regmbc(0x1e5b); regmbc(0x1e5d); regmbc(0x1e5f);
		      regmbc(0xa7a7);
		      ཕྱིར;
	    pak_sa 's': pak_sa 0x15b: pak_sa 0x15d: pak_sa 0x15f: pak_sa 0x161:
	    pak_sa 0x1e61: pak_sa 0x219: pak_sa 0x23f: pak_sa 0x1d74: pak_sa 0x1d8a:
	    pak_sa 0x1e63: pak_sa 0x1e65: pak_sa 0x1e67: pak_sa 0x1e69: pak_sa 0xa7a9:
		      regmbc('s'); regmbc(0x15b); regmbc(0x15d);
		      regmbc(0x15f); regmbc(0x161); regmbc(0x23f);
		      regmbc(0x219); regmbc(0x1d74); regmbc(0x1d8a);
		      regmbc(0x1e61); regmbc(0x1e63); regmbc(0x1e65);
		      regmbc(0x1e67); regmbc(0x1e69); regmbc(0xa7a9);
		      ཕྱིར;
	    pak_sa 't': pak_sa 0x163: pak_sa 0x165: pak_sa 0x167: pak_sa 0x1ab:
	    pak_sa 0x1ad: pak_sa 0x21b: pak_sa 0x288: pak_sa 0x1d75: pak_sa 0x1e6b:
	    pak_sa 0x1e6d: pak_sa 0x1e6f: pak_sa 0x1e71: pak_sa 0x1e97: pak_sa 0x2c66:
		      regmbc('t'); regmbc(0x163); regmbc(0x165);
		      regmbc(0x167); regmbc(0x1ab); regmbc(0x21b);
		      regmbc(0x1ad); regmbc(0x288); regmbc(0x1d75);
		      regmbc(0x1e6b); regmbc(0x1e6d); regmbc(0x1e6f);
		      regmbc(0x1e71); regmbc(0x1e97); regmbc(0x2c66);
		      ཕྱིར;
	    pak_sa 'u': pak_sa 0xf9: pak_sa 0xfa: pak_sa 0xfb: pak_sa 0xfc:
	    pak_sa 0x169: pak_sa 0x16b: pak_sa 0x16d: pak_sa 0x16f:
	    pak_sa 0x171: pak_sa 0x173: pak_sa 0x1b0: pak_sa 0x1d4:
	    pak_sa 0x1d6: pak_sa 0x1d8: pak_sa 0x1da: pak_sa 0x1dc:
	    pak_sa 0x215: pak_sa 0x217: pak_sa 0x289: pak_sa 0x1e73:
	    pak_sa 0x1d7e: pak_sa 0x1d99: pak_sa 0x1e75: pak_sa 0x1e77:
	    pak_sa 0x1e79: pak_sa 0x1e7b: pak_sa 0x1ee5: pak_sa 0x1ee7:
	    pak_sa 0x1ee9: pak_sa 0x1eeb: pak_sa 0x1eed: pak_sa 0x1eef:
	    pak_sa 0x1ef1:
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
		      ཕྱིར;
	    pak_sa 'v': pak_sa 0x28b: pak_sa 0x1d8c: pak_sa 0x1e7d: pak_sa 0x1e7f:
		      regmbc('v'); regmbc(0x28b); regmbc(0x1d8c);
		      regmbc(0x1e7d); regmbc(0x1e7f);
		      ཕྱིར;
	    pak_sa 'w': pak_sa 0x175: pak_sa 0x1e81: pak_sa 0x1e83:
	    pak_sa 0x1e85: pak_sa 0x1e87: pak_sa 0x1e89: pak_sa 0x1e98:
		      regmbc('w'); regmbc(0x175); regmbc(0x1e81);
		      regmbc(0x1e83); regmbc(0x1e85); regmbc(0x1e87);
		      regmbc(0x1e89); regmbc(0x1e98);
		      ཕྱིར;
	    pak_sa 'x': pak_sa 0x1e8b: pak_sa 0x1e8d:
		      regmbc('x'); regmbc(0x1e8b); regmbc(0x1e8d);
		      ཕྱིར;
	    pak_sa 'y': pak_sa 0xfd: pak_sa 0xff: pak_sa 0x177: pak_sa 0x1b4:
	    pak_sa 0x233: pak_sa 0x24f: pak_sa 0x1e8f: pak_sa 0x1e99: pak_sa 0x1ef3:
	    pak_sa 0x1ef5: pak_sa 0x1ef7: pak_sa 0x1ef9:
		      regmbc('y'); regmbc(0xfd); regmbc(0xff);
		      regmbc(0x177); regmbc(0x1b4); regmbc(0x233);
		      regmbc(0x24f); regmbc(0x1e8f); regmbc(0x1e99);
		      regmbc(0x1ef3); regmbc(0x1ef5); regmbc(0x1ef7);
		      regmbc(0x1ef9);
		      ཕྱིར;
	    pak_sa 'z': pak_sa 0x17a: pak_sa 0x17c: pak_sa 0x17e: pak_sa 0x1b6:
	    pak_sa 0x1d76: pak_sa 0x1d8e: pak_sa 0x1e91: pak_sa 0x1e93:
	    pak_sa 0x1e95: pak_sa 0x2c6c:
		      regmbc('z'); regmbc(0x17a); regmbc(0x17c);
		      regmbc(0x17e); regmbc(0x1b6); regmbc(0x1d76);
		      regmbc(0x1d8e); regmbc(0x1e91); regmbc(0x1e93);
		      regmbc(0x1e95); regmbc(0x2c6c);
		      ཕྱིར;
	}
    }
    regmbc(c);
}

/*
 * Emit a node.
 * Return pointer to generated code.
 */
    static char_u *
regnode(ཧྲིལ op)
{
    char_u  *ret;

    ret = regcode;
    གལ་ཏེ (ret == JUST_CALC_SIZE)
	regsize += 3;
    དེ་མིན
    {
	*regcode++ = op;
	*regcode++ = NUL;		// Null "next" pointer.
	*regcode++ = NUL;
    }
    ཕྱིར ret;
}

/*
 * Write a long as four bytes at "p" and return pointer to the next char.
 */
    static char_u *
re_put_long(char_u *p, long_u val)
{
    *p++ = (char_u) ((val >> 24) & 0377);
    *p++ = (char_u) ((val >> 16) & 0377);
    *p++ = (char_u) ((val >> 8) & 0377);
    *p++ = (char_u) (val & 0377);
    ཕྱིར p;
}

/*
 * regnext - dig the "next" pointer out of a node
 * Returns NULL when calculating size, when there is no next item and when
 * there is an error.
 */
    static char_u *
regnext(char_u *p)
{
    ཧྲིལ	    offset;

    གལ་ཏེ (p == JUST_CALC_SIZE || reg_toolong)
	ཕྱིར NULL;

    offset = NEXT(p);
    གལ་ཏེ (offset == 0)
	ཕྱིར NULL;

    གལ་ཏེ (OP(p) == BACK)
	ཕྱིར p - offset;
    དེ་མིན
	ཕྱིར p + offset;
}

/*
 * Set the next-pointer at the end of a node chain.
 */
    static void
regtail(char_u *p, char_u *val)
{
    char_u	*scan;
    char_u	*temp;
    ཧྲིལ		offset;

    གལ་ཏེ (p == JUST_CALC_SIZE)
	ཕྱིར;

    // Find last node.
    scan = p;
    ཆེད་དུ (;;)
    {
	temp = regnext(scan);
	གལ་ཏེ (temp == NULL)
	    bha_nga;
	scan = temp;
    }

    གལ་ཏེ (OP(scan) == BACK)
	offset = (ཧྲིལ)(scan - val);
    དེ་མིན
	offset = (ཧྲིལ)(val - scan);
    // When the offset uses more than 16 bits it can no longer fit in the two
    // bytes available.  Use a global flag to avoid having to check return
    // values in too many places.
    གལ་ཏེ (offset > 0xffff)
	reg_toolong = TRUE;
    དེ་མིན
    {
	*(scan + 1) = (char_u) (((unsigned)offset >> 8) & 0377);
	*(scan + 2) = (char_u) (offset & 0377);
    }
}

/*
 * Like regtail, on item after a BRANCH; nop if none.
 */
    static void
regoptail(char_u *p, char_u *val)
{
    // When op is neither BRANCH nor BRACE_COMPLEX0-9, it is "operandless"
    གལ་ཏེ (p == NULL || p == JUST_CALC_SIZE
	    || (OP(p) != BRANCH
		&& (OP(p) < BRACE_COMPLEX || OP(p) > BRACE_COMPLEX + 9)))
	ཕྱིར;
    regtail(OPERAND(p), val);
}

/*
 * Insert an operator in front of already-emitted operand
 *
 * Means relocating the operand.
 */
    static void
reginsert(ཧྲིལ op, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    གལ་ཏེ (regcode == JUST_CALC_SIZE)
    {
	regsize += 3;
	ཕྱིར;
    }
    src = regcode;
    regcode += 3;
    dst = regcode;
    སྐབས (src > opnd)
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
    static void
reginsert_nr(ཧྲིལ op, long val, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    གལ་ཏེ (regcode == JUST_CALC_SIZE)
    {
	regsize += 7;
	ཕྱིར;
    }
    src = regcode;
    regcode += 7;
    dst = regcode;
    སྐབས (src > opnd)
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
    static void
reginsert_limits(
    ཧྲིལ		op,
    long	minval,
    long	maxval,
    char_u	*opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    གལ་ཏེ (regcode == JUST_CALC_SIZE)
    {
	regsize += 11;
	ཕྱིར;
    }
    src = regcode;
    regcode += 11;
    dst = regcode;
    སྐབས (src > opnd)
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
    static ཧྲིལ
seen_endbrace(ཧྲིལ refnum)
{
    གལ་ཏེ (!had_endbrace[refnum])
    {
	char_u *p;

	// Trick: check if "@<=" or "@<!" follows, in which case
	// the \1 can appear before the referenced match.
	ཆེད་དུ (p = regparse; *p != NUL; ++p)
	    གལ་ཏེ (p[0] == '@' && p[1] == '<' && (p[2] == '!' || p[2] == '='))
		bha_nga;
	གལ་ཏེ (*p == NUL)
	{
	    emsg(_(e_illegal_back_reference));
	    rc_did_emsg = TRUE;
	    ཕྱིར FALSE;
	}
    }
    ཕྱིར TRUE;
}

/*
 * Parse the lowest level.
 *
 * Optimization:  gobbles an entire sequence of ordinary characters so that
 * it can turn them into a single node, which is smaller to store and
 * faster to run.  Don't do this when one_exactly is set.
 */
    static char_u *
regatom(ཧྲིལ *flagp)
{
    char_u	    *ret;
    ཧྲིལ		    flags;
    ཧྲིལ		    c;
    char_u	    *p;
    ཧྲིལ		    extra = 0;
    ཧྲིལ		    save_prev_at_start = prev_at_start;

    *flagp = WORST;		// Tentatively.

    c = getchr();
    switch (c)
    {
      pak_sa Magic('^'):
	ret = regnode(BOL);
	bha_nga;

      pak_sa Magic('$'):
	ret = regnode(EOL);
#གལ་ཏེ defined(FEAT_SYN_HL)
	had_eol = TRUE;
#endif
	bha_nga;

      pak_sa Magic('<'):
	ret = regnode(BOW);
	bha_nga;

      pak_sa Magic('>'):
	ret = regnode(EOW);
	bha_nga;

      pak_sa Magic('_'):
	c = no_Magic(getchr());
	གལ་ཏེ (c == '^')		// "\_^" is start-of-line
	{
	    ret = regnode(BOL);
	    bha_nga;
	}
	གལ་ཏེ (c == '$')		// "\_$" is end-of-line
	{
	    ret = regnode(EOL);
#གལ་ཏེ defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#endif
	    bha_nga;
	}

	extra = ADD_NL;
	*flagp |= HASNL;

	// "\_[" is character range plus newline
	གལ་ཏེ (c == '[')
	    goto collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	// Character classes.
      pak_sa Magic('.'):
      pak_sa Magic('i'):
      pak_sa Magic('I'):
      pak_sa Magic('k'):
      pak_sa Magic('K'):
      pak_sa Magic('f'):
      pak_sa Magic('F'):
      pak_sa Magic('p'):
      pak_sa Magic('P'):
      pak_sa Magic('s'):
      pak_sa Magic('S'):
      pak_sa Magic('d'):
      pak_sa Magic('D'):
      pak_sa Magic('x'):
      pak_sa Magic('X'):
      pak_sa Magic('o'):
      pak_sa Magic('O'):
      pak_sa Magic('w'):
      pak_sa Magic('W'):
      pak_sa Magic('h'):
      pak_sa Magic('H'):
      pak_sa Magic('a'):
      pak_sa Magic('A'):
      pak_sa Magic('l'):
      pak_sa Magic('L'):
      pak_sa Magic('u'):
      pak_sa Magic('U'):
	p = vim_strchr(classchars, no_Magic(c));
	གལ་ཏེ (p == NULL)
	    EMSG_RET_NULL(_(e_invalid_use_of_underscore));

	// When '.' is followed by a composing char ignore the dot, so that
	// the composing char is matched here.
	གལ་ཏེ (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	{
	    c = getchr();
	    goto do_multibyte;
	}
	ret = regnode(classcodes[p - classchars] + extra);
	*flagp |= HASWIDTH | SIMPLE;
	bha_nga;

      pak_sa Magic('n'):
	གལ་ཏེ (reg_string)
	{
	    // In a string "\n" matches a newline character.
	    ret = regnode(EXACTLY);
	    regc(NL);
	    regc(NUL);
	    *flagp |= HASWIDTH | SIMPLE;
	}
	དེ་མིན
	{
	    // In buffer text "\n" matches the end of a line.
	    ret = regnode(NEWL);
	    *flagp |= HASWIDTH | HASNL;
	}
	bha_nga;

      pak_sa Magic('('):
	གལ་ཏེ (one_exactly)
	    EMSG_ONE_RET_NULL;
	ret = reg(REG_PAREN, &flags);
	གལ་ཏེ (ret == NULL)
	    ཕྱིར NULL;
	*flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
	bha_nga;

      pak_sa NUL:
      pak_sa Magic('|'):
      pak_sa Magic('&'):
      pak_sa Magic(')'):
	གལ་ཏེ (one_exactly)
	    EMSG_ONE_RET_NULL;
	// Supposed to be caught earlier.
	IEMSG_RET_NULL(e_internal_error_in_regexp);
	// NOTREACHED

      pak_sa Magic('='):
      pak_sa Magic('?'):
      pak_sa Magic('+'):
      pak_sa Magic('@'):
      pak_sa Magic('{'):
      pak_sa Magic('*'):
	c = no_Magic(c);
	EMSG3_RET_NULL(_(e_str_chr_follows_nothing),
		(c == '*' ? reg_magic >= MAGIC_ON : reg_magic == MAGIC_ALL), c);
	// NOTREACHED

      pak_sa Magic('~'):		// previous substitute pattern
	    གལ་ཏེ (reg_prev_sub != NULL)
	    {
		char_u	    *lp;

		ret = regnode(EXACTLY);
		lp = reg_prev_sub;
		སྐབས (*lp != NUL)
		    regc(*lp++);
		regc(NUL);
		གལ་ཏེ (*reg_prev_sub != NUL)
		{
		    *flagp |= HASWIDTH;
		    གལ་ཏེ ((lp - reg_prev_sub) == 1)
			*flagp |= SIMPLE;
		}
	    }
	    དེ་མིན
		EMSG_RET_NULL(_(e_no_previous_substitute_regular_expression));
	    bha_nga;

      pak_sa Magic('1'):
      pak_sa Magic('2'):
      pak_sa Magic('3'):
      pak_sa Magic('4'):
      pak_sa Magic('5'):
      pak_sa Magic('6'):
      pak_sa Magic('7'):
      pak_sa Magic('8'):
      pak_sa Magic('9'):
	    {
		ཧྲིལ		    refnum;

		refnum = c - Magic('0');
		གལ་ཏེ (!seen_endbrace(refnum))
		    ཕྱིར NULL;
		ret = regnode(BACKREF + refnum);
	    }
	    bha_nga;

      pak_sa Magic('z'):
	{
	    c = no_Magic(getchr());
	    switch (c)
	    {
#ifdef FEAT_SYN_HL
		pak_sa '(': གལ་ཏེ ((reg_do_extmatch & REX_SET) == 0)
			      EMSG_RET_NULL(_(e_z_not_allowed_here));
			  གལ་ཏེ (one_exactly)
			      EMSG_ONE_RET_NULL;
			  ret = reg(REG_ZPAREN, &flags);
			  གལ་ཏེ (ret == NULL)
			      ཕྱིར NULL;
			  *flagp |= flags & (HASWIDTH|SPSTART|HASNL|HASLOOKBH);
			  re_has_z = REX_SET;
			  bha_nga;

		pak_sa '1':
		pak_sa '2':
		pak_sa '3':
		pak_sa '4':
		pak_sa '5':
		pak_sa '6':
		pak_sa '7':
		pak_sa '8':
		pak_sa '9': གལ་ཏེ ((reg_do_extmatch & REX_USE) == 0)
			      EMSG_RET_NULL(_(e_z1_z9_not_allowed_here));
			  ret = regnode(ZREF + c - '0');
			  re_has_z = REX_USE;
			  bha_nga;
#endif

		pak_sa 's': ret = regnode(MOPEN + 0);
			  གལ་ཏེ (re_mult_next("\\zs") == FAIL)
			      ཕྱིར NULL;
			  bha_nga;

		pak_sa 'e': ret = regnode(MCLOSE + 0);
			  གལ་ཏེ (re_mult_next("\\ze") == FAIL)
			      ཕྱིར NULL;
			  bha_nga;

		default:  EMSG_RET_NULL(_(e_invalid_character_after_bsl_z));
	    }
	}
	bha_nga;

      pak_sa Magic('%'):
	{
	    c = no_Magic(getchr());
	    switch (c)
	    {
		// () without a back reference
		pak_sa '(':
		    གལ་ཏེ (one_exactly)
			EMSG_ONE_RET_NULL;
		    ret = reg(REG_NPAREN, &flags);
		    གལ་ཏེ (ret == NULL)
			ཕྱིར NULL;
		    *flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
		    bha_nga;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		pak_sa '^':
		    ret = regnode(RE_BOF);
		    bha_nga;

		pak_sa '$':
		    ret = regnode(RE_EOF);
		    bha_nga;

		pak_sa '#':
		    གལ་ཏེ (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_start_of_pattern),
								  regparse[1]);
			ཕྱིར FAIL;
		    }
		    ret = regnode(CURSOR);
		    bha_nga;

		pak_sa 'V':
		    ret = regnode(RE_VISUAL);
		    bha_nga;

		pak_sa 'C':
		    ret = regnode(RE_COMPOSING);
		    bha_nga;

		// \%[abc]: Emit as a list of branches, all ending at the last
		// branch which matches nothing.
		pak_sa '[':
			  གལ་ཏེ (one_exactly)	// doesn't nest
			      EMSG_ONE_RET_NULL;
			  {
			      char_u	*lastbranch;
			      char_u	*lastnode = NULL;
			      char_u	*br;

			      ret = NULL;
			      སྐབས ((c = getchr()) != ']')
			      {
				  གལ་ཏེ (c == NUL)
				      EMSG2_RET_NULL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
				  br = regnode(BRANCH);
				  གལ་ཏེ (ret == NULL)
				      ret = br;
				  དེ་མིན
				  {
				      regtail(lastnode, br);
				      གལ་ཏེ (reg_toolong)
					  ཕྱིར NULL;
				  }

				  ungetchr();
				  one_exactly = TRUE;
				  lastnode = regatom(flagp);
				  one_exactly = FALSE;
				  གལ་ཏེ (lastnode == NULL)
				      ཕྱིར NULL;
			      }
			      གལ་ཏེ (ret == NULL)
				  EMSG2_RET_NULL(_(e_empty_str_brackets),
						      reg_magic == MAGIC_ALL);
			      lastbranch = regnode(BRANCH);
			      br = regnode(NOTHING);
			      གལ་ཏེ (ret != JUST_CALC_SIZE)
			      {
				  regtail(lastnode, br);
				  regtail(lastbranch, br);
				  // connect all branches to the NOTHING
				  // branch at the end
				  ཆེད་དུ (br = ret; br != lastnode; )
				  {
				      གལ་ཏེ (OP(br) == BRANCH)
				      {
					  regtail(br, lastbranch);
					  གལ་ཏེ (reg_toolong)
					      ཕྱིར NULL;
					  br = OPERAND(br);
				      }
				      དེ་མིན
					  br = regnext(br);
				  }
			      }
			      *flagp &= ~(HASWIDTH | SIMPLE);
			      bha_nga;
			  }

		pak_sa 'd':   // %d123 decimal
		pak_sa 'o':   // %o123 octal
		pak_sa 'x':   // %xab hex 2
		pak_sa 'u':   // %uabcd hex 4
		pak_sa 'U':   // %U1234abcd hex 8
			  {
			      vimlong_T i;

			      switch (c)
			      {
				  pak_sa 'd': i = getdecchrs(); bha_nga;
				  pak_sa 'o': i = getoctchrs(); bha_nga;
				  pak_sa 'x': i = gethexchrs(2); bha_nga;
				  pak_sa 'u': i = gethexchrs(4); bha_nga;
				  pak_sa 'U': i = gethexchrs(8); bha_nga;
				  default:  i = -1; bha_nga;
			      }

			      གལ་ཏེ (i < 0 || i > INT_MAX)
				  EMSG2_RET_NULL(
					    _(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			      གལ་ཏེ (use_multibytecode(i))
				  ret = regnode(MULTIBYTECODE);
			      དེ་མིན
				  ret = regnode(EXACTLY);
			      གལ་ཏེ (i == 0)
				  regc(0x0a);
			      དེ་མིན
				  regmbc((ཧྲིལ)i);
			      regc(NUL);
			      *flagp |= HASWIDTH;
			      bha_nga;
			  }

		default:
			  གལ་ཏེ (VIM_ISDIGIT(c) || c == '<' || c == '>'
						|| c == '\'' || c == '.')
			  {
			      long_u	n = 0;
			      ཧྲིལ	cmp;
			      ཧྲིལ	cur = FALSE;
			      ཧྲིལ	got_digit = FALSE;

			      cmp = c;
			      གལ་ཏེ (cmp == '<' || cmp == '>')
				  c = getchr();
			      གལ་ཏེ (no_Magic(c) == '.')
			      {
				  cur = TRUE;
				  c = getchr();
			      }
			      སྐབས (VIM_ISDIGIT(c))
			      {
				  got_digit = TRUE;
				  n = n * 10 + (c - '0');
				  c = getchr();
			      }
			      གལ་ཏེ (no_Magic(c) == '\'' && n == 0)
			      {
				  // "\%'m", "\%<'m" and "\%>'m": Mark
				  c = getchr();
				  ret = regnode(RE_MARK);
				  གལ་ཏེ (ret == JUST_CALC_SIZE)
				      regsize += 2;
				  དེ་མིན
				  {
				      *regcode++ = c;
				      *regcode++ = cmp;
				  }
				  bha_nga;
			      }
			      དེ་མིན གལ་ཏེ ((c == 'l' || c == 'c' || c == 'v')
					  && (cur || got_digit))
			      {
				  གལ་ཏེ (cur && n)
				  {
				    semsg(_(e_regexp_number_after_dot_pos_search_chr),
								  no_Magic(c));
				    rc_did_emsg = TRUE;
				    ཕྱིར NULL;
				  }
				  གལ་ཏེ (c == 'l')
				  {
				      གལ་ཏེ (cur)
					  n = curwin->w_cursor.lnum;
				      ret = regnode(RE_LNUM);
				      གལ་ཏེ (save_prev_at_start)
					  at_start = TRUE;
				  }
				  དེ་མིན གལ་ཏེ (c == 'c')
				  {
				      གལ་ཏེ (cur)
				      {
					  n = curwin->w_cursor.col;
					  n++;
				      }
				      ret = regnode(RE_COL);
				  }
				  དེ་མིན
				  {
				      གལ་ཏེ (cur)
				      {
					  colnr_T vcol = 0;

					  getvvcol(curwin, &curwin->w_cursor,
							 NULL, NULL, &vcol, 0);
					  ++vcol;
					  n = vcol;
				      }
				      ret = regnode(RE_VCOL);
				  }
				  གལ་ཏེ (ret == JUST_CALC_SIZE)
				      regsize += 5;
				  དེ་མིན
				  {
				      // put the number and the optional
				      // comparator after the opcode
				      regcode = re_put_long(regcode, n);
				      *regcode++ = cmp;
				  }
				  bha_nga;
			      }
			  }

			  EMSG2_RET_NULL(_(e_invalid_character_after_str),
						      reg_magic == MAGIC_ALL);
	    }
	}
	bha_nga;

      pak_sa Magic('['):
collection:
	{
	    char_u	*lp;

	    // If there is no matching ']', we assume the '[' is a normal
	    // character.  This makes 'incsearch' and ":help [" work.
	    lp = skip_anyof(regparse);
	    གལ་ཏེ (*lp == ']')	// there is a matching ']'
	    {
		ཧྲིལ	startc = -1;	// > 0 when next '-' is a range
		ཧྲིལ	endc;

		གལ་ཏེ (extra == 0)
		{
		    ཧྲིལ	cl = bt_recognize_char_class(regparse, lp);

		    གལ་ཏེ (cl != 0)
		    {
			ret = regnode(cl);
			regparse = lp;
			skipchr();
			*flagp |= HASWIDTH | SIMPLE;
			bha_nga;
		    }
		}

		// In a character class, different parsing rules apply.
		// Not even \ is special anymore, nothing is.
		གལ་ཏེ (*regparse == '^')	    // Complement of range.
		{
		    ret = regnode(ANYBUT + extra);
		    regparse++;
		}
		དེ་མིན
		    ret = regnode(ANYOF + extra);

		// At the start ']' and '-' mean the literal character.
		གལ་ཏེ (*regparse == ']' || *regparse == '-')
		{
		    startc = *regparse;
		    regc(*regparse++);
		}

		སྐབས (*regparse != NUL && *regparse != ']')
		{
		    གལ་ཏེ (*regparse == '-')
		    {
			++regparse;
			// The '-' is not used for a range at the end and
			// after or before a '\n'.
			གལ་ཏེ (*regparse == ']' || *regparse == NUL
				|| startc == -1
				|| (regparse[0] == '\\' && regparse[1] == 'n'))
			{
			    regc('-');
			    startc = '-';	// [--x] is a range
			}
			དེ་མིན
			{
			    // Also accept "a-[.z.]"
			    endc = 0;
			    གལ་ཏེ (*regparse == '[')
				endc = get_coll_element(&regparse);
			    གལ་ཏེ (endc == 0)
			    {
				གལ་ཏེ (has_mbyte)
				    endc = mb_ptr2char_adv(&regparse);
				དེ་མིན
				    endc = *regparse++;
			    }

			    // Handle \o40, \x20 and \u20AC style sequences
			    གལ་ཏེ (endc == '\\' && !reg_cpo_lit && !reg_cpo_bsl)
				endc = coll_get_char();

			    གལ་ཏེ (startc > endc)
				EMSG_RET_NULL(_(e_reverse_range_in_character_class));
			    གལ་ཏེ (has_mbyte && ((*mb_char2len)(startc) > 1
						 || (*mb_char2len)(endc) > 1))
			    {
				// Limit to a range of 256 chars.
				གལ་ཏེ (endc > startc + 256)
				    EMSG_RET_NULL(_(e_range_too_large_in_character_class));
				སྐབས (++startc <= endc)
				    regmbc(startc);
			    }
			    དེ་མིན
			    {
				སྐབས (++startc <= endc)
				    regc(startc);
			    }
			    startc = -1;
			}
		    }
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    དེ་མིན གལ་ཏེ (*regparse == '\\'
			    && !reg_cpo_bsl
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != NULL
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR,
						       regparse[1]) != NULL)))
		    {
			regparse++;
			གལ་ཏེ (*regparse == 'n')
			{
			    // '\n' in range: also match NL
			    གལ་ཏེ (ret != JUST_CALC_SIZE)
			    {
				// Using \n inside [^] does not change what
				// matches. "[^\n]" is the same as ".".
				གལ་ཏེ (*ret == ANYOF)
				{
				    *ret = ANYOF + ADD_NL;
				    *flagp |= HASNL;
				}
				// else: must have had a \n already
			    }
			    regparse++;
			    startc = -1;
			}
			དེ་མིན གལ་ཏེ (*regparse == 'd'
				|| *regparse == 'o'
				|| *regparse == 'x'
				|| *regparse == 'u'
				|| *regparse == 'U')
			{
			    startc = coll_get_char();
			    // max UTF-8 Codepoint is U+10FFFF,
			    // but allow values until INT_MAX
			    གལ་ཏེ (startc == INT_MAX)
				EMSG_RET_NULL(_(e_unicode_val_too_large));
			    གལ་ཏེ (startc == 0)
				regc(0x0a);
			    དེ་མིན
				regmbc(startc);
			}
			དེ་མིན
			{
			    startc = backslash_trans(*regparse++);
			    regc(startc);
			}
		    }
		    དེ་མིན གལ་ཏེ (*regparse == '[')
		    {
			ཧྲིལ c_class;
			ཧྲིལ cu;

			c_class = get_char_class(&regparse);
			startc = -1;
			// Characters assumed to be 8 bits!
			switch (c_class)
			{
			    pak_sa CLASS_NONE:
				c_class = get_equi_class(&regparse);
				གལ་ཏེ (c_class != 0)
				{
				    // produce equivalence class
				    reg_equi_class(c_class);
				}
				དེ་མིན གལ་ཏེ ((c_class =
					    get_coll_element(&regparse)) != 0)
				{
				    // produce a collating element
				    regmbc(c_class);
				}
				དེ་མིན
				{
				    // literal '[', allow [[-x] as a range
				    startc = *regparse++;
				    regc(startc);
				}
				bha_nga;
			    pak_sa CLASS_ALNUM:
				ཆེད་དུ (cu = 1; cu < 128; cu++)
				    གལ་ཏེ (isalnum(cu))
					regmbc(cu);
				bha_nga;
			    pak_sa CLASS_ALPHA:
				ཆེད་དུ (cu = 1; cu < 128; cu++)
				    གལ་ཏེ (isalpha(cu))
					regmbc(cu);
				bha_nga;
			    pak_sa CLASS_BLANK:
				regc(' ');
				regc('\t');
				bha_nga;
			    pak_sa CLASS_CNTRL:
				ཆེད་དུ (cu = 1; cu <= 127; cu++)
				    གལ་ཏེ (iscntrl(cu))
					regmbc(cu);
				bha_nga;
			    pak_sa CLASS_DIGIT:
				ཆེད་དུ (cu = 1; cu <= 127; cu++)
				    གལ་ཏེ (VIM_ISDIGIT(cu))
					regmbc(cu);
				bha_nga;
			    pak_sa CLASS_GRAPH:
				ཆེད་དུ (cu = 1; cu <= 127; cu++)
				    གལ་ཏེ (isgraph(cu))
					regmbc(cu);
				bha_nga;
			    pak_sa CLASS_LOWER:
				ཆེད་དུ (cu = 1; cu <= 255; cu++)
				    གལ་ཏེ (MB_ISLOWER(cu) && cu != 170
								 && cu != 186)
					regmbc(cu);
				bha_nga;
			    pak_sa CLASS_PRINT:
				ཆེད་དུ (cu = 1; cu <= 255; cu++)
				    གལ་ཏེ (vim_isprintc(cu))
					regmbc(cu);
				bha_nga;
			    pak_sa CLASS_PUNCT:
				ཆེད་དུ (cu = 1; cu < 128; cu++)
				    གལ་ཏེ (ispunct(cu))
					regmbc(cu);
				bha_nga;
			    pak_sa CLASS_SPACE:
				ཆེད་དུ (cu = 9; cu <= 13; cu++)
				    regc(cu);
				regc(' ');
				bha_nga;
			    pak_sa CLASS_UPPER:
				ཆེད་དུ (cu = 1; cu <= 255; cu++)
				    གལ་ཏེ (MB_ISUPPER(cu))
					regmbc(cu);
				bha_nga;
			    pak_sa CLASS_XDIGIT:
				ཆེད་དུ (cu = 1; cu <= 255; cu++)
				    གལ་ཏེ (vim_isxdigit(cu))
					regmbc(cu);
				bha_nga;
			    pak_sa CLASS_TAB:
				regc('\t');
				bha_nga;
			    pak_sa CLASS_RETURN:
				regc('\r');
				bha_nga;
			    pak_sa CLASS_BACKSPACE:
				regc('\b');
				bha_nga;
			    pak_sa CLASS_ESCAPE:
				regc('\033');
				bha_nga;
			    pak_sa CLASS_IDENT:
				ཆེད་དུ (cu = 1; cu <= 255; cu++)
				    གལ་ཏེ (vim_isIDc(cu))
					regmbc(cu);
				bha_nga;
			    pak_sa CLASS_KEYWORD:
				ཆེད་དུ (cu = 1; cu <= 255; cu++)
				    གལ་ཏེ (reg_iswordc(cu))
					regmbc(cu);
				bha_nga;
			    pak_sa CLASS_FNAME:
				ཆེད་དུ (cu = 1; cu <= 255; cu++)
				    གལ་ཏེ (vim_isfilec(cu))
					regmbc(cu);
				bha_nga;
			}
		    }
		    དེ་མིན
		    {
			གལ་ཏེ (has_mbyte)
			{
			    ཧྲིལ	len;

			    // produce a multibyte character, including any
			    // following composing characters
			    startc = mb_ptr2char(regparse);
			    len = (*mb_ptr2len)(regparse);
			    གལ་ཏེ (enc_utf8 && utf_char2len(startc) != len)
				startc = -1;	// composing chars
			    སྐབས (--len >= 0)
				regc(*regparse++);
			}
			དེ་མིན
			{
			    startc = *regparse++;
			    regc(startc);
			}
		    }
		}
		regc(NUL);
		prevchr_len = 1;	// last char was the ']'
		གལ་ཏེ (*regparse != ']')
		    EMSG_RET_NULL(_(e_too_many_brackets));  // Cannot happen?
		skipchr();	    // let's be friends with the lexer again
		*flagp |= HASWIDTH | SIMPLE;
		bha_nga;
	    }
	    དེ་མིན གལ་ཏེ (reg_strict)
		EMSG2_RET_NULL(_(e_missing_rsb_after_str_lsb),
							reg_magic > MAGIC_OFF);
	}
	// FALLTHROUGH

      default:
	{
	    ཧྲིལ		len;

	    // A multi-byte character is handled as a separate atom if it's
	    // before a multi and when it's a composing char.
	    གལ་ཏེ (use_multibytecode(c))
	    {
do_multibyte:
		ret = regnode(MULTIBYTECODE);
		regmbc(c);
		*flagp |= HASWIDTH | SIMPLE;
		bha_nga;
	    }

	    ret = regnode(EXACTLY);

	    // Append characters as long as:
	    // - there is no following multi, we then need the character in
	    //   front of it as a single character operand
	    // - not running into a Magic character
	    // - "one_exactly" is not set
	    // But always emit at least one character.  Might be a Multi,
	    // e.g., a "[" without matching "]".
	    ཆེད་དུ (len = 0; c != NUL && (len == 0
			|| (re_multi_type(peekchr()) == NOT_MULTI
			    && !one_exactly
			    && !is_Magic(c))); ++len)
	    {
		c = no_Magic(c);
		གལ་ཏེ (has_mbyte)
		{
		    regmbc(c);
		    གལ་ཏེ (enc_utf8)
		    {
			ཧྲིལ	l;

			// Need to get composing character too.
			ཆེད་དུ (;;)
			{
			    l = utf_ptr2len(regparse);
			    གལ་ཏེ (!UTF_COMPOSINGLIKE(regparse, regparse + l))
				bha_nga;
			    regmbc(utf_ptr2char(regparse));
			    skipchr();
			}
		    }
		}
		དེ་མིན
		    regc(c);
		c = getchr();
	    }
	    ungetchr();

	    regc(NUL);
	    *flagp |= HASWIDTH;
	    གལ་ཏེ (len == 1)
		*flagp |= SIMPLE;
	}
	bha_nga;
    }

    ཕྱིར ret;
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
    static char_u *
regpiece(ཧྲིལ *flagp)
{
    char_u	    *ret;
    ཧྲིལ		    op;
    char_u	    *next;
    ཧྲིལ		    flags;
    long	    minval;
    long	    maxval;

    ret = regatom(&flags);
    གལ་ཏེ (ret == NULL)
	ཕྱིར NULL;

    op = peekchr();
    གལ་ཏེ (re_multi_type(op) == NOT_MULTI)
    {
	*flagp = flags;
	ཕྱིར ret;
    }
    // default flags
    *flagp = (WORST | SPSTART | (flags & (HASNL | HASLOOKBH)));

    skipchr();
    switch (op)
    {
	pak_sa Magic('*'):
	    གལ་ཏེ (flags & SIMPLE)
		reginsert(STAR, ret);
	    དེ་མིན
	    {
		// Emit x* as (x&|), where & means "self".
		reginsert(BRANCH, ret); // Either x
		regoptail(ret, regnode(BACK));	// and loop
		regoptail(ret, ret);	// back
		regtail(ret, regnode(BRANCH));	// or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    bha_nga;

	pak_sa Magic('+'):
	    གལ་ཏེ (flags & SIMPLE)
		reginsert(PLUS, ret);
	    དེ་མིན
	    {
		// Emit x+ as x(&|), where & means "self".
		next = regnode(BRANCH); // Either
		regtail(ret, next);
		regtail(regnode(BACK), ret);	// loop back
		regtail(next, regnode(BRANCH)); // or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    *flagp = (WORST | HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    bha_nga;

	pak_sa Magic('@'):
	    {
		ཧྲིལ	lop = END;
		long	nr;

		nr = (long)getdecchrs();
		switch (no_Magic(getchr()))
		{
		    pak_sa '=': lop = MATCH; bha_nga;		  // \@=
		    pak_sa '!': lop = NOMATCH; bha_nga;		  // \@!
		    pak_sa '>': lop = SUBPAT; bha_nga;		  // \@>
		    pak_sa '<': switch (no_Magic(getchr()))
			      {
				  pak_sa '=': lop = BEHIND; bha_nga;   // \@<=
				  pak_sa '!': lop = NOBEHIND; bha_nga; // \@<!
			      }
		}
		གལ་ཏེ (lop == END)
		    EMSG2_RET_NULL(_(e_invalid_character_after_str_at),
						      reg_magic == MAGIC_ALL);
		// Look behind must match with behind_pos.
		གལ་ཏེ (lop == BEHIND || lop == NOBEHIND)
		{
		    regtail(ret, regnode(BHPOS));
		    *flagp |= HASLOOKBH;
		}
		regtail(ret, regnode(END)); // operand ends
		གལ་ཏེ (lop == BEHIND || lop == NOBEHIND)
		{
		    གལ་ཏེ (nr < 0)
			nr = 0; // no limit is same as zero limit
		    reginsert_nr(lop, nr, ret);
		}
		དེ་མིན
		    reginsert(lop, ret);
		bha_nga;
	    }

	pak_sa Magic('?'):
	pak_sa Magic('='):
	    // Emit x= as (x|)
	    reginsert(BRANCH, ret);		// Either x
	    regtail(ret, regnode(BRANCH));	// or
	    next = regnode(NOTHING);		// null.
	    regtail(ret, next);
	    regoptail(ret, next);
	    bha_nga;

	pak_sa Magic('{'):
	    གལ་ཏེ (!read_limits(&minval, &maxval))
		ཕྱིར NULL;
	    གལ་ཏེ (flags & SIMPLE)
	    {
		reginsert(BRACE_SIMPLE, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
	    }
	    དེ་མིན
	    {
		གལ་ཏེ (num_complex_braces >= 10)
		    EMSG2_RET_NULL(_(e_too_many_complex_str_curly),
						      reg_magic == MAGIC_ALL);
		reginsert(BRACE_COMPLEX + num_complex_braces, ret);
		regoptail(ret, regnode(BACK));
		regoptail(ret, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
		++num_complex_braces;
	    }
	    གལ་ཏེ (minval > 0 && maxval > 0)
		*flagp = (HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    bha_nga;
    }
    གལ་ཏེ (re_multi_type(peekchr()) != NOT_MULTI)
    {
	// Can't have a multi follow a multi.
	གལ་ཏེ (peekchr() == Magic('*'))
	    EMSG2_RET_NULL(_(e_nested_str), reg_magic >= MAGIC_ON);
	EMSG3_RET_NULL(_(e_nested_str_chr), reg_magic == MAGIC_ALL,
							  no_Magic(peekchr()));
    }

    ཕྱིར ret;
}

/*
 * Parse one alternative of an | or & operator.
 * Implements the concatenation operator.
 */
    static char_u *
regconcat(ཧྲིལ *flagp)
{
    char_u	*first = NULL;
    char_u	*chain = NULL;
    char_u	*latest;
    ཧྲིལ		flags;
    ཧྲིལ		cont = TRUE;

    *flagp = WORST;		// Tentatively.

    སྐབས (cont)
    {
	switch (peekchr())
	{
	    pak_sa NUL:
	    pak_sa Magic('|'):
	    pak_sa Magic('&'):
	    pak_sa Magic(')'):
			    cont = FALSE;
			    bha_nga;
	    pak_sa Magic('Z'):
			    regflags |= RF_ICOMBINE;
			    skipchr_keepstart();
			    bha_nga;
	    pak_sa Magic('c'):
			    regflags |= RF_ICASE;
			    skipchr_keepstart();
			    bha_nga;
	    pak_sa Magic('C'):
			    regflags |= RF_NOICASE;
			    skipchr_keepstart();
			    bha_nga;
	    pak_sa Magic('v'):
			    reg_magic = MAGIC_ALL;
			    skipchr_keepstart();
			    curchr = -1;
			    bha_nga;
	    pak_sa Magic('m'):
			    reg_magic = MAGIC_ON;
			    skipchr_keepstart();
			    curchr = -1;
			    bha_nga;
	    pak_sa Magic('M'):
			    reg_magic = MAGIC_OFF;
			    skipchr_keepstart();
			    curchr = -1;
			    bha_nga;
	    pak_sa Magic('V'):
			    reg_magic = MAGIC_NONE;
			    skipchr_keepstart();
			    curchr = -1;
			    bha_nga;
	    default:
			    latest = regpiece(&flags);
			    གལ་ཏེ (latest == NULL || reg_toolong)
				ཕྱིར NULL;
			    *flagp |= flags & (HASWIDTH | HASNL | HASLOOKBH);
			    གལ་ཏེ (chain == NULL)	// First piece.
				*flagp |= flags & SPSTART;
			    དེ་མིན
				regtail(chain, latest);
			    chain = latest;
			    གལ་ཏེ (first == NULL)
				first = latest;
			    bha_nga;
	}
    }
    གལ་ཏེ (first == NULL)		// Loop ran zero times.
	first = regnode(NOTHING);
    ཕྱིར first;
}

/*
 * Parse one alternative of an | operator.
 * Implements the & operator.
 */
    static char_u *
regbranch(ཧྲིལ *flagp)
{
    char_u	*ret;
    char_u	*chain = NULL;
    char_u	*latest;
    ཧྲིལ		flags;

    *flagp = WORST | HASNL;		// Tentatively.

    ret = regnode(BRANCH);
    ཆེད་དུ (;;)
    {
	latest = regconcat(&flags);
	གལ་ཏེ (latest == NULL)
	    ཕྱིར NULL;
	// If one of the branches has width, the whole thing has.  If one of
	// the branches anchors at start-of-line, the whole thing does.
	// If one of the branches uses look-behind, the whole thing does.
	*flagp |= flags & (HASWIDTH | SPSTART | HASLOOKBH);
	// If one of the branches doesn't match a line-break, the whole thing
	// doesn't.
	*flagp &= ~HASNL | (flags & HASNL);
	གལ་ཏེ (chain != NULL)
	    regtail(chain, latest);
	གལ་ཏེ (peekchr() != Magic('&'))
	    bha_nga;
	skipchr();
	regtail(latest, regnode(END)); // operand ends
	གལ་ཏེ (reg_toolong)
	    bha_nga;
	reginsert(MATCH, latest);
	chain = latest;
    }

    ཕྱིར ret;
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
    static char_u *
reg(
    ཧྲིལ		paren,	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
    ཧྲིལ		*flagp)
{
    char_u	*ret;
    char_u	*br;
    char_u	*ender;
    ཧྲིལ		parno = 0;
    ཧྲིལ		flags;

    *flagp = HASWIDTH;		// Tentatively.

#ifdef FEAT_SYN_HL
    གལ་ཏེ (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	གལ་ཏེ (regnzpar >= NSUBEXP)
	    EMSG_RET_NULL(_(e_too_many_z));
	parno = regnzpar;
	regnzpar++;
	ret = regnode(ZOPEN + parno);
    }
    དེ་མིན
#endif
	གལ་ཏེ (paren == REG_PAREN)
    {
	// Make a MOPEN node.
	གལ་ཏེ (regnpar >= NSUBEXP)
	    EMSG2_RET_NULL(_(e_too_many_str_open), reg_magic == MAGIC_ALL);
	parno = regnpar;
	++regnpar;
	ret = regnode(MOPEN + parno);
    }
    དེ་མིན གལ་ཏེ (paren == REG_NPAREN)
    {
	// Make a NOPEN node.
	ret = regnode(NOPEN);
    }
    དེ་མིན
	ret = NULL;

    གལ་ཏེ (bt_reg_parse_depth >= REG_MAX_PAREN_DEPTH)
	EMSG_RET_NULL(_(e_command_too_complex));
    ++bt_reg_parse_depth;

    // Pick up the branches, linking them together.
    br = regbranch(&flags);
    གལ་ཏེ (br == NULL)
    {
	ret = NULL;
	goto theend;
    }
    གལ་ཏེ (ret != NULL)
	regtail(ret, br);	// [MZ]OPEN -> first.
    དེ་མིན
	ret = br;
    // If one of the branches can be zero-width, the whole thing can.
    // If one of the branches has * at start or matches a line-break, the
    // whole thing can.
    གལ་ཏེ (!(flags & HASWIDTH))
	*flagp &= ~HASWIDTH;
    *flagp |= flags & (SPSTART | HASNL | HASLOOKBH);
    སྐབས (peekchr() == Magic('|'))
    {
	skipchr();
	br = regbranch(&flags);
	གལ་ཏེ (br == NULL || reg_toolong)
	{
	    ret = NULL;
	    goto theend;
	}
	regtail(ret, br);	// BRANCH -> BRANCH.
	གལ་ཏེ (!(flags & HASWIDTH))
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
    ཆེད་དུ (br = ret; br != NULL; br = regnext(br))
	regoptail(br, ender);

    // Check for proper termination.
    གལ་ཏེ (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
#ifdef FEAT_SYN_HL
	གལ་ཏེ (paren == REG_ZPAREN)
	{
	    emsg(_(e_unmatched_z));
	    rc_did_emsg = TRUE;
	    ret = NULL;
	    goto theend;
	}
	དེ་མིན
#endif
	གལ་ཏེ (paren == REG_NPAREN)
	{
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = NULL;
	    goto theend;
	}
	དེ་མིན
	{
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = NULL;
	    goto theend;
	}
    }
    དེ་མིན གལ་ཏེ (paren == REG_NOPAREN && peekchr() != NUL)
    {
	གལ་ཏེ (curchr == Magic(')'))
	{
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = NULL;
	    goto theend;
	}
	དེ་མིན
	{
	    emsg(_(e_trailing_characters));	// "Can't happen".
	    rc_did_emsg = TRUE;
	    ret = NULL;
	    goto theend;
	}
    }
    // Here we set the flag allowing back references to this set of
    // parentheses.
    གལ་ཏེ (paren == REG_PAREN)
	had_endbrace[parno] = TRUE;	// have seen the close paren

theend:
    --bt_reg_parse_depth;
    ཕྱིར ret;
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
    static regprog_T *
bt_regcomp(char_u *expr, ཧྲིལ re_flags)
{
    bt_regprog_T    *r;
    char_u	*scan;
    char_u	*longest;
    ཧྲིལ		len;
    ཧྲིལ		flags;

    གལ་ཏེ (expr == NULL)
	IEMSG_RET_NULL(e_null_argument);

    init_class_tab();

    // First pass: determine size, legality.
    regcomp_start(expr, re_flags);
    regcode = JUST_CALC_SIZE;
    regc(REGMAGIC);
    གལ་ཏེ (reg(REG_NOPAREN, &flags) == NULL)
	ཕྱིར NULL;

    // Allocate space.
    r = alloc(offsetof(bt_regprog_T, program) + regsize);
    གལ་ཏེ (r == NULL)
	ཕྱིར NULL;
    r->re_in_use = FALSE;
#ifdef DEBUG
    r->regsz = regsize;
#endif

    // Second pass: emit code.
    regcomp_start(expr, re_flags);
    regcode = r->program;
    regc(REGMAGIC);
    གལ་ཏེ (reg(REG_NOPAREN, &flags) == NULL || reg_toolong)
    {
	vim_free(r);
	གལ་ཏེ (reg_toolong)
	    EMSG_RET_NULL(_(e_pattern_too_long));
	ཕྱིར NULL;
    }

    // Dig out information for optimizations.
    r->regstart = NUL;		// Worst-case defaults.
    r->reganch = 0;
    r->regmust = NULL;
    r->regmlen = 0;
    r->regflags = regflags;
    གལ་ཏེ (flags & HASNL)
	r->regflags |= RF_HASNL;
    གལ་ཏེ (flags & HASLOOKBH)
	r->regflags |= RF_LOOKBH;
#ifdef FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    r->reghasz = re_has_z;
#endif
    scan = r->program + 1;	// First BRANCH.
    གལ་ཏེ (OP(regnext(scan)) == END)   // Only one top-level choice.
    {
	scan = OPERAND(scan);

	// Starting-point info.
	གལ་ཏེ (OP(scan) == BOL || OP(scan) == RE_BOF)
	{
	    r->reganch++;
	    scan = regnext(scan);
	}

	གལ་ཏེ (OP(scan) == EXACTLY)
	{
	    གལ་ཏེ (has_mbyte)
		r->regstart = (*mb_ptr2char)(OPERAND(scan));
	    དེ་མིན
		r->regstart = *OPERAND(scan);
	}
	དེ་མིན གལ་ཏེ ((OP(scan) == BOW
		    || OP(scan) == EOW
		    || OP(scan) == NOTHING
		    || OP(scan) == MOPEN + 0 || OP(scan) == NOPEN
		    || OP(scan) == MCLOSE + 0 || OP(scan) == NCLOSE)
		 && OP(regnext(scan)) == EXACTLY)
	{
	    གལ་ཏེ (has_mbyte)
		r->regstart = (*mb_ptr2char)(OPERAND(regnext(scan)));
	    དེ་མིན
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
	གལ་ཏེ ((flags & SPSTART || OP(scan) == BOW || OP(scan) == EOW)
							  && !(flags & HASNL))
	{
	    size_t  scanlen;

	    longest = NULL;
	    len = 0;
	    ཆེད་དུ (; scan != NULL; scan = regnext(scan))
	    {
		གལ་ཏེ (OP(scan) == EXACTLY)
		{
		    scanlen = STRLEN(OPERAND(scan));
		    གལ་ཏེ (scanlen >= (size_t)len)
		    {
			longest = OPERAND(scan);
			len = (ཧྲིལ)scanlen;
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
    ཕྱིར (regprog_T *)r;
}

#གལ་ཏེ defined(FEAT_SYN_HL)
/*
 * Check if during the previous call to vim_regcomp the EOL item "$" has been
 * found.  This is messy, but it works fine.
 */
    ཧྲིལ
vim_regcomp_had_eol(void)
{
    ཕྱིར had_eol;
}
#endif

/*
 * Get a number after a backslash that is inside [].
 * When nothing is recognized return a backslash.
 */
    static ཧྲིལ
coll_get_char(void)
{
    vimlong_T	nr = -1;

    switch (*regparse++)
    {
	pak_sa 'd': nr = getdecchrs(); bha_nga;
	pak_sa 'o': nr = getoctchrs(); bha_nga;
	pak_sa 'x': nr = gethexchrs(2); bha_nga;
	pak_sa 'u': nr = gethexchrs(4); bha_nga;
	pak_sa 'U': nr = gethexchrs(8); bha_nga;
    }
    གལ་ཏེ (nr < 0)
    {
	// If getting the number fails be backwards compatible: the character
	// is a backslash.
	--regparse;
	nr = '\\';
    }
    གལ་ཏེ (nr > INT_MAX)
	nr = INT_MAX;
    ཕྱིར nr;
}

/*
 * Free a compiled regexp program, returned by bt_regcomp().
 */
    static void
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
static long	bl_minval;
static long	bl_maxval;

/*
 * Save the input line and position in a regsave_T.
 */
    static void
reg_save(regsave_T *save, garray_T *gap)
{
    གལ་ཏེ (REG_MULTI)
    {
	save->rs_u.pos.col = (colnr_T)(rex.input - rex.line);
	save->rs_u.pos.lnum = rex.lnum;
    }
    དེ་མིན
	save->rs_u.ptr = rex.input;
    save->rs_len = gap->ga_len;
}

/*
 * Restore the input line and position from a regsave_T.
 */
    static void
reg_restore(regsave_T *save, garray_T *gap)
{
    གལ་ཏེ (REG_MULTI)
    {
	གལ་ཏེ (rex.lnum != save->rs_u.pos.lnum)
	{
	    // only call reg_getline() when the line number changed to save
	    // a bit of time
	    rex.lnum = save->rs_u.pos.lnum;
	    rex.line = reg_getline(rex.lnum);
	}
	rex.input = rex.line + save->rs_u.pos.col;
    }
    དེ་མིན
	rex.input = save->rs_u.ptr;
    gap->ga_len = save->rs_len;
}

/*
 * Return TRUE if current position is equal to saved position.
 */
    static ཧྲིལ
reg_save_equal(regsave_T *save)
{
    གལ་ཏེ (REG_MULTI)
	ཕྱིར rex.lnum == save->rs_u.pos.lnum
				  && rex.input == rex.line + save->rs_u.pos.col;
    ཕྱིར rex.input == save->rs_u.ptr;
}

// Save the sub-expressions before attempting a match.
#define save_se(savep, posp, pp) \
    REG_MULTI ? save_se_multi((savep), (posp)) : save_se_one((savep), (pp))

// After a failed match restore the sub-expressions.
#define restore_se(savep, posp, pp) \
{ \
    གལ་ཏེ (REG_MULTI) \
	*(posp) = (savep)->se_u.pos; \
    དེ་མིན \
	*(pp) = (savep)->se_u.ptr; \
}

/*
 * Tentatively set the sub-expression start to the current position (after
 * calling regmatch() they will have changed).  Need to save the existing
 * values for when there is no match.
 * Use se_save() to use pointer (save_se_multi()) or position (save_se_one()),
 * depending on REG_MULTI.
 */
    static void
save_se_multi(save_se_T *savep, lpos_T *posp)
{
    savep->se_u.pos = *posp;
    posp->lnum = rex.lnum;
    posp->col = (colnr_T)(rex.input - rex.line);
}

    static void
save_se_one(save_se_T *savep, char_u **pp)
{
    savep->se_u.ptr = *pp;
    *pp = rex.input;
}

/*
 * regrepeat - repeatedly match something simple, return how many.
 * Advances rex.input (and rex.lnum) to just after the matched chars.
 */
    static ཧྲིལ
regrepeat(
    char_u	*p,
    long	maxcount)   // maximum number of matches allowed
{
    long	count = 0;
    char_u	*scan;
    char_u	*opnd;
    ཧྲིལ		mask;
    ཧྲིལ		testval = 0;

    scan = rex.input;	    // Make local copy of rex.input for speed.
    opnd = OPERAND(p);
    switch (OP(p))
    {
      pak_sa ANY:
      pak_sa ANY + ADD_NL:
	སྐབས (count < maxcount)
	{
	    // Matching anything means we continue until end-of-line (or
	    // end-of-file for ANY + ADD_NL), only limited by maxcount.
	    སྐབས (*scan != NUL && count < maxcount)
	    {
		++count;
		MB_PTR_ADV(scan);
	    }
	    གལ་ཏེ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
				      || rex.reg_line_lbr || count == maxcount)
		bha_nga;
	    ++count;		// count the line-break
	    reg_nextline();
	    scan = rex.input;
	    གལ་ཏེ (got_int)
		bha_nga;
	}
	bha_nga;

      pak_sa IDENT:
      pak_sa IDENT + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      pak_sa SIDENT:
      pak_sa SIDENT + ADD_NL:
	སྐབས (count < maxcount)
	{
	    གལ་ཏེ (vim_isIDc(PTR2CHAR(scan)) && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    དེ་མིན གལ་ཏེ (*scan == NUL)
	    {
		གལ་ཏེ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_line_lbr)
		    bha_nga;
		reg_nextline();
		scan = rex.input;
		གལ་ཏེ (got_int)
		    bha_nga;
	    }
	    དེ་མིན གལ་ཏེ (rex.reg_line_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    དེ་མིན
		bha_nga;
	    ++count;
	}
	bha_nga;

      pak_sa KWORD:
      pak_sa KWORD + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      pak_sa SKWORD:
      pak_sa SKWORD + ADD_NL:
	སྐབས (count < maxcount)
	{
	    གལ་ཏེ (vim_iswordp_buf(scan, rex.reg_buf)
					  && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    དེ་མིན གལ་ཏེ (*scan == NUL)
	    {
		གལ་ཏེ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_line_lbr)
		    bha_nga;
		reg_nextline();
		scan = rex.input;
		གལ་ཏེ (got_int)
		    bha_nga;
	    }
	    དེ་མིན གལ་ཏེ (rex.reg_line_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    དེ་མིན
		bha_nga;
	    ++count;
	}
	bha_nga;

      pak_sa FNAME:
      pak_sa FNAME + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      pak_sa SFNAME:
      pak_sa SFNAME + ADD_NL:
	སྐབས (count < maxcount)
	{
	    གལ་ཏེ (vim_isfilec(PTR2CHAR(scan)) && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    དེ་མིན གལ་ཏེ (*scan == NUL)
	    {
		གལ་ཏེ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_line_lbr)
		    bha_nga;
		reg_nextline();
		scan = rex.input;
		གལ་ཏེ (got_int)
		    bha_nga;
	    }
	    དེ་མིན གལ་ཏེ (rex.reg_line_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    དེ་མིན
		bha_nga;
	    ++count;
	}
	bha_nga;

      pak_sa PRINT:
      pak_sa PRINT + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      pak_sa SPRINT:
      pak_sa SPRINT + ADD_NL:
	སྐབས (count < maxcount)
	{
	    གལ་ཏེ (*scan == NUL)
	    {
		གལ་ཏེ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_line_lbr)
		    bha_nga;
		reg_nextline();
		scan = rex.input;
		གལ་ཏེ (got_int)
		    bha_nga;
	    }
	    དེ་མིན གལ་ཏེ (vim_isprintc(PTR2CHAR(scan)) == 1
					  && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    དེ་མིན གལ་ཏེ (rex.reg_line_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    དེ་མིན
		bha_nga;
	    ++count;
	}
	bha_nga;

      pak_sa WHITE:
      pak_sa WHITE + ADD_NL:
	testval = mask = RI_WHITE;
do_class:
	སྐབས (count < maxcount)
	{
	    ཧྲིལ		l;

	    གལ་ཏེ (*scan == NUL)
	    {
		གལ་ཏེ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_line_lbr)
		    bha_nga;
		reg_nextline();
		scan = rex.input;
		གལ་ཏེ (got_int)
		    bha_nga;
	    }
	    དེ་མིན གལ་ཏེ (has_mbyte && (l = (*mb_ptr2len)(scan)) > 1)
	    {
		གལ་ཏེ (testval != 0)
		    bha_nga;
		scan += l;
	    }
	    དེ་མིན གལ་ཏེ ((class_tab[*scan] & mask) == testval)
		++scan;
	    དེ་མིན གལ་ཏེ (rex.reg_line_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    དེ་མིན
		bha_nga;
	    ++count;
	}
	bha_nga;

      pak_sa NWHITE:
      pak_sa NWHITE + ADD_NL:
	mask = RI_WHITE;
	goto do_class;
      pak_sa DIGIT:
      pak_sa DIGIT + ADD_NL:
	testval = mask = RI_DIGIT;
	goto do_class;
      pak_sa NDIGIT:
      pak_sa NDIGIT + ADD_NL:
	mask = RI_DIGIT;
	goto do_class;
      pak_sa HEX:
      pak_sa HEX + ADD_NL:
	testval = mask = RI_HEX;
	goto do_class;
      pak_sa NHEX:
      pak_sa NHEX + ADD_NL:
	mask = RI_HEX;
	goto do_class;
      pak_sa OCTAL:
      pak_sa OCTAL + ADD_NL:
	testval = mask = RI_OCTAL;
	goto do_class;
      pak_sa NOCTAL:
      pak_sa NOCTAL + ADD_NL:
	mask = RI_OCTAL;
	goto do_class;
      pak_sa WORD:
      pak_sa WORD + ADD_NL:
	testval = mask = RI_WORD;
	goto do_class;
      pak_sa NWORD:
      pak_sa NWORD + ADD_NL:
	mask = RI_WORD;
	goto do_class;
      pak_sa HEAD:
      pak_sa HEAD + ADD_NL:
	testval = mask = RI_HEAD;
	goto do_class;
      pak_sa NHEAD:
      pak_sa NHEAD + ADD_NL:
	mask = RI_HEAD;
	goto do_class;
      pak_sa ALPHA:
      pak_sa ALPHA + ADD_NL:
	testval = mask = RI_ALPHA;
	goto do_class;
      pak_sa NALPHA:
      pak_sa NALPHA + ADD_NL:
	mask = RI_ALPHA;
	goto do_class;
      pak_sa LOWER:
      pak_sa LOWER + ADD_NL:
	testval = mask = RI_LOWER;
	goto do_class;
      pak_sa NLOWER:
      pak_sa NLOWER + ADD_NL:
	mask = RI_LOWER;
	goto do_class;
      pak_sa UPPER:
      pak_sa UPPER + ADD_NL:
	testval = mask = RI_UPPER;
	goto do_class;
      pak_sa NUPPER:
      pak_sa NUPPER + ADD_NL:
	mask = RI_UPPER;
	goto do_class;

      pak_sa EXACTLY:
	{
	    ཧྲིལ	    cu, cl;

	    // This doesn't do a multi-byte character, because a MULTIBYTECODE
	    // would have been used for it.  It does handle single-byte
	    // characters, such as latin1.
	    གལ་ཏེ (rex.reg_ic)
	    {
		cu = MB_TOUPPER(*opnd);
		cl = MB_TOLOWER(*opnd);
		སྐབས (count < maxcount && (*scan == cu || *scan == cl))
		{
		    count++;
		    scan++;
		}
	    }
	    དེ་མིན
	    {
		cu = *opnd;
		སྐབས (count < maxcount && *scan == cu)
		{
		    count++;
		    scan++;
		}
	    }
	    bha_nga;
	}

      pak_sa MULTIBYTECODE:
	{
	    ཧྲིལ		i, len, cf = 0;

	    // Safety check (just in case 'encoding' was changed since
	    // compiling the program).
	    གལ་ཏེ ((len = (*mb_ptr2len)(opnd)) > 1)
	    {
		གལ་ཏེ (rex.reg_ic && enc_utf8)
		    cf = utf_fold(utf_ptr2char(opnd));
		སྐབས (count < maxcount && (*mb_ptr2len)(scan) >= len)
		{
		    ཆེད་དུ (i = 0; i < len; ++i)
			གལ་ཏེ (opnd[i] != scan[i])
			    bha_nga;
		    གལ་ཏེ (i < len && (!rex.reg_ic || !enc_utf8
					|| utf_fold(utf_ptr2char(scan)) != cf))
			bha_nga;
		    scan += len;
		    ++count;
		}
	    }
	}
	bha_nga;

      pak_sa ANYOF:
      pak_sa ANYOF + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH

      pak_sa ANYBUT:
      pak_sa ANYBUT + ADD_NL:
	སྐབས (count < maxcount)
	{
	    ཧྲིལ len;

	    གལ་ཏེ (*scan == NUL)
	    {
		གལ་ཏེ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_line_lbr)
		    bha_nga;
		reg_nextline();
		scan = rex.input;
		གལ་ཏེ (got_int)
		    bha_nga;
	    }
	    དེ་མིན གལ་ཏེ (rex.reg_line_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    དེ་མིན གལ་ཏེ (has_mbyte && (len = (*mb_ptr2len)(scan)) > 1)
	    {
		གལ་ཏེ ((cstrchr(opnd, (*mb_ptr2char)(scan)) == NULL) == testval)
		    bha_nga;
		scan += len;
	    }
	    དེ་མིན
	    {
		གལ་ཏེ ((cstrchr(opnd, *scan) == NULL) == testval)
		    bha_nga;
		++scan;
	    }
	    ++count;
	}
	bha_nga;

      pak_sa NEWL:
	སྐབས (count < maxcount
		&& ((*scan == NUL && rex.lnum <= rex.reg_maxline
				       && !rex.reg_line_lbr && REG_MULTI)
		    || (*scan == '\n' && rex.reg_line_lbr)))
	{
	    count++;
	    གལ་ཏེ (rex.reg_line_lbr)
		ADVANCE_REGINPUT();
	    དེ་མིན
		reg_nextline();
	    scan = rex.input;
	    གལ་ཏེ (got_int)
		bha_nga;
	}
	bha_nga;

      default:			// Oh dear.  Called inappropriately.
	iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	printf("Called regrepeat with op code %d\n", OP(p));
#endif
	bha_nga;
    }

    rex.input = scan;

    ཕྱིར (ཧྲིལ)count;
}

/*
 * Push an item onto the regstack.
 * Returns pointer to new item.  Returns NULL when out of memory.
 */
    static regitem_T *
regstack_push(regstate_T state, char_u *scan)
{
    regitem_T	*rp;

    གལ་ཏེ ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
    {
	emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
	ཕྱིར NULL;
    }
    གལ་ཏེ (GA_GROW_FAILS(&regstack, (ཧྲིལ)parimaa_na(regitem_T)))
	ཕྱིར NULL;

    rp = (regitem_T *)((var_na *)regstack.ga_data + regstack.ga_len);
    rp->rs_state = state;
    rp->rs_scan = scan;

    regstack.ga_len += parimaa_na(regitem_T);
    ཕྱིར rp;
}

/*
 * Pop an item from the regstack.
 */
    static void
regstack_pop(char_u **scan)
{
    regitem_T	*rp;

    rp = (regitem_T *)((var_na *)regstack.ga_data + regstack.ga_len) - 1;
    *scan = rp->rs_scan;

    regstack.ga_len -= parimaa_na(regitem_T);
}

#ifdef FEAT_RELTIME
/*
 * Check if the timer expired, return TRUE if so.
 */
    static ཧྲིལ
bt_did_time_out(ཧྲིལ *timed_out)
{
    གལ་ཏེ (*timeout_flag)
    {
	གལ་ཏེ (timed_out != NULL)
	{
# ifdef FEAT_EVAL
	    གལ་ཏེ (!*timed_out)
		ch_log(NULL, "BT regexp timed out");
# endif
	    *timed_out = TRUE;
	}
	ཕྱིར TRUE;
    }
    ཕྱིར FALSE;
}
#endif

/*
 * Save the current subexpr to "bp", so that they can be restored
 * later by restore_subexpr().
 */
    static void
save_subexpr(regbehind_T *bp)
{
    ཧྲིལ i;

    // When "rex.need_clear_subexpr" is set we don't need to save the values,
    // only remember that this flag needs to be set again when restoring.
    bp->save_need_clear_subexpr = rex.need_clear_subexpr;
    གལ་ཏེ (rex.need_clear_subexpr)
	ཕྱིར;

    ཆེད་དུ (i = 0; i < NSUBEXP; ++i)
    {
	གལ་ཏེ (REG_MULTI)
	{
	    bp->save_start[i].se_u.pos = rex.reg_startpos[i];
	    bp->save_end[i].se_u.pos = rex.reg_endpos[i];
	}
	དེ་མིན
	{
	    bp->save_start[i].se_u.ptr = rex.reg_startp[i];
	    bp->save_end[i].se_u.ptr = rex.reg_endp[i];
	}
    }
}

/*
 * Restore the subexpr from "bp".
 */
    static void
restore_subexpr(regbehind_T *bp)
{
    ཧྲིལ i;

    // Only need to restore saved values when they are not to be cleared.
    rex.need_clear_subexpr = bp->save_need_clear_subexpr;
    གལ་ཏེ (rex.need_clear_subexpr)
	ཕྱིར;

    ཆེད་དུ (i = 0; i < NSUBEXP; ++i)
    {
	གལ་ཏེ (REG_MULTI)
	{
	    rex.reg_startpos[i] = bp->save_start[i].se_u.pos;
	    rex.reg_endpos[i] = bp->save_end[i].se_u.pos;
	}
	དེ་མིན
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
    static ཧྲིལ
regmatch(
    char_u	*scan,		    // Current node.
    ཧྲིལ		*timed_out UNUSED)  // flag set on timeout or NULL
{
  char_u	*next;		// Next node.
  ཧྲིལ		op;
  ཧྲིལ		c;
  regitem_T	*rp;
  ཧྲིལ		no;
  ཧྲིལ		status;		// one of the RA_ values:

  // Make "regstack" and "backpos" empty.  They are allocated and freed in
  // bt_regexec_both() to reduce malloc()/free() calls.
  regstack.ga_len = 0;
  backpos.ga_len = 0;

  // Repeat until "regstack" is empty.
  ཆེད་དུ (;;)
  {
    // Some patterns may take a long time to match, e.g., "\([a-z]\+\)\+Q".
    // Allow interrupting them with CTRL-C.
    fast_breakcheck();

#ifdef DEBUG
    གལ་ཏེ (scan != NULL && regnarrate)
    {
	mch_errmsg((var_na *)regprop(scan));
	mch_errmsg("(\n");
    }
#endif

    // Repeat for items that can be matched sequentially, without using the
    // regstack.
    ཆེད་དུ (;;)
    {
	གལ་ཏེ (got_int || scan == NULL)
	{
	    status = RA_FAIL;
	    bha_nga;
	}
#ifdef FEAT_RELTIME
	གལ་ཏེ (bt_did_time_out(timed_out))
	{
	    status = RA_FAIL;
	    bha_nga;
	}
#endif
	status = RA_CONT;

#ifdef DEBUG
	གལ་ཏེ (regnarrate)
	{
	    mch_errmsg((var_na *)regprop(scan));
	    mch_errmsg("...\n");
# ifdef FEAT_SYN_HL
	    གལ་ཏེ (re_extmatch_in != NULL)
	    {
		ཧྲིལ i;

		mch_errmsg(_("External submatches:\n"));
		ཆེད་དུ (i = 0; i < NSUBEXP; i++)
		{
		    mch_errmsg("    \"");
		    གལ་ཏེ (re_extmatch_in->matches[i] != NULL)
			mch_errmsg((var_na *)re_extmatch_in->matches[i]);
		    mch_errmsg("\"\n");
		}
	    }
# endif
	}
#endif
	next = regnext(scan);

	op = OP(scan);
	// Check for character class with NL added.
	གལ་ཏེ (!rex.reg_line_lbr && WITH_NL(op) && REG_MULTI
			     && *rex.input == NUL && rex.lnum <= rex.reg_maxline)
	{
	    reg_nextline();
	}
	དེ་མིན གལ་ཏེ (rex.reg_line_lbr && WITH_NL(op) && *rex.input == '\n')
	{
	    ADVANCE_REGINPUT();
	}
	དེ་མིན
	{
	  གལ་ཏེ (WITH_NL(op))
	      op -= ADD_NL;
	  གལ་ཏེ (has_mbyte)
	      c = (*mb_ptr2char)(rex.input);
	  དེ་མིན
	      c = *rex.input;
	  switch (op)
	  {
	  pak_sa BOL:
	    གལ་ཏེ (rex.input != rex.line)
		status = RA_NOMATCH;
	    bha_nga;

	  pak_sa EOL:
	    གལ་ཏེ (c != NUL)
		status = RA_NOMATCH;
	    bha_nga;

	  pak_sa RE_BOF:
	    // We're not at the beginning of the file when below the first
	    // line where we started, not at the start of the line or we
	    // didn't start at the first line of the buffer.
	    གལ་ཏེ (rex.lnum != 0 || rex.input != rex.line
				       || (REG_MULTI && rex.reg_firstlnum > 1))
		status = RA_NOMATCH;
	    bha_nga;

	  pak_sa RE_EOF:
	    གལ་ཏེ (rex.lnum != rex.reg_maxline || c != NUL)
		status = RA_NOMATCH;
	    bha_nga;

	  pak_sa CURSOR:
	    // Check if the buffer is in a window and compare the
	    // rex.reg_win->w_cursor position to the match position.
	    གལ་ཏེ (rex.reg_win == NULL
		    || (rex.lnum + rex.reg_firstlnum
						 != rex.reg_win->w_cursor.lnum)
		    || ((colnr_T)(rex.input - rex.line)
						 != rex.reg_win->w_cursor.col))
		status = RA_NOMATCH;
	    bha_nga;

	  pak_sa RE_MARK:
	    // Compare the mark position to the match position.
	    {
		ཧྲིལ	mark = OPERAND(scan)[0];
		ཧྲིལ	cmp = OPERAND(scan)[1];
		pos_T	*pos;
		size_t	col = REG_MULTI ? rex.input - rex.line : 0;

		pos = getmark_buf(rex.reg_buf, mark, FALSE);

		// Line may have been freed, get it again.
		གལ་ཏེ (REG_MULTI)
		{
		    rex.line = reg_getline(rex.lnum);
		    rex.input = rex.line + col;
		}

		གལ་ཏེ (pos == NULL		     // mark doesn't exist
			|| pos->lnum <= 0)   // mark isn't set in reg_buf
		{
		    status = RA_NOMATCH;
		}
		དེ་མིན
		{
		    colnr_T pos_col = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->col == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->col;

		    གལ་ཏེ ((pos->lnum == rex.lnum + rex.reg_firstlnum
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
	    bha_nga;

	  pak_sa RE_VISUAL:
	    གལ་ཏེ (!reg_match_visual())
		status = RA_NOMATCH;
	    bha_nga;

	  pak_sa RE_LNUM:
	    གལ་ཏེ (!REG_MULTI || !re_num_cmp((long_u)(rex.lnum + rex.reg_firstlnum),
									scan))
		status = RA_NOMATCH;
	    bha_nga;

	  pak_sa RE_COL:
	    གལ་ཏེ (!re_num_cmp((long_u)(rex.input - rex.line) + 1, scan))
		status = RA_NOMATCH;
	    bha_nga;

	  pak_sa RE_VCOL:
	    {
		win_T	    *wp = rex.reg_win == NULL ? curwin : rex.reg_win;
		linenr_T    lnum = REG_MULTI ? rex.reg_firstlnum + rex.lnum : 1;
		long_u	    vcol;

		གལ་ཏེ (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_buffer->b_ml.ml_line_count))
		    lnum = 1;
		vcol = (long_u)win_linetabsize(wp, lnum, rex.line,
					      (colnr_T)(rex.input - rex.line));
		གལ་ཏེ (!re_num_cmp(vcol + 1, scan))
		    status = RA_NOMATCH;
	    }
	    bha_nga;

	  pak_sa BOW:	// \<word; rex.input points to w
	    གལ་ཏེ (c == NUL)	// Can't match at end of line
		status = RA_NOMATCH;
	    དེ་མིན གལ་ཏེ (has_mbyte)
	    {
		ཧྲིལ this_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.input, rex.reg_buf);
		གལ་ཏེ (this_class <= 1)
		    status = RA_NOMATCH;  // not on a word at all
		དེ་མིན གལ་ཏེ (reg_prev_class() == this_class)
		    status = RA_NOMATCH;  // previous char is in same word
	    }
	    དེ་མིན
	    {
		གལ་ཏེ (!vim_iswordc_buf(c, rex.reg_buf) || (rex.input > rex.line
				&& vim_iswordc_buf(rex.input[-1], rex.reg_buf)))
		    status = RA_NOMATCH;
	    }
	    bha_nga;

	  pak_sa EOW:	// word\>; rex.input points after d
	    གལ་ཏེ (rex.input == rex.line)    // Can't match at start of line
		status = RA_NOMATCH;
	    དེ་མིན གལ་ཏེ (has_mbyte)
	    {
		ཧྲིལ this_class, prev_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.input, rex.reg_buf);
		prev_class = reg_prev_class();
		གལ་ཏེ (this_class == prev_class
			|| prev_class == 0 || prev_class == 1)
		    status = RA_NOMATCH;
	    }
	    དེ་མིན
	    {
		གལ་ཏེ (!vim_iswordc_buf(rex.input[-1], rex.reg_buf)
			|| (rex.input[0] != NUL
					   && vim_iswordc_buf(c, rex.reg_buf)))
		    status = RA_NOMATCH;
	    }
	    bha_nga; // Matched with EOW

	  pak_sa ANY:
	    // ANY does not match new lines.
	    གལ་ཏེ (c == NUL)
		status = RA_NOMATCH;
	    དེ་མིན
		ADVANCE_REGINPUT();
	    bha_nga;

	  pak_sa IDENT:
	    གལ་ཏེ (!vim_isIDc(c))
		status = RA_NOMATCH;
	    དེ་མིན
		ADVANCE_REGINPUT();
	    bha_nga;

	  pak_sa SIDENT:
	    གལ་ཏེ (VIM_ISDIGIT(*rex.input) || !vim_isIDc(c))
		status = RA_NOMATCH;
	    དེ་མིན
		ADVANCE_REGINPUT();
	    bha_nga;

	  pak_sa KWORD:
	    གལ་ཏེ (!vim_iswordp_buf(rex.input, rex.reg_buf))
		status = RA_NOMATCH;
	    དེ་མིན
		ADVANCE_REGINPUT();
	    bha_nga;

	  pak_sa SKWORD:
	    གལ་ཏེ (VIM_ISDIGIT(*rex.input)
				    || !vim_iswordp_buf(rex.input, rex.reg_buf))
		status = RA_NOMATCH;
	    དེ་མིན
		ADVANCE_REGINPUT();
	    bha_nga;

	  pak_sa FNAME:
	    གལ་ཏེ (!vim_isfilec(c))
		status = RA_NOMATCH;
	    དེ་མིན
		ADVANCE_REGINPUT();
	    bha_nga;

	  pak_sa SFNAME:
	    གལ་ཏེ (VIM_ISDIGIT(*rex.input) || !vim_isfilec(c))
		status = RA_NOMATCH;
	    དེ་མིན
		ADVANCE_REGINPUT();
	    bha_nga;

	  pak_sa PRINT:
	    གལ་ཏེ (!vim_isprintc(PTR2CHAR(rex.input)))
		status = RA_NOMATCH;
	    དེ་མིན
		ADVANCE_REGINPUT();
	    bha_nga;

	  pak_sa SPRINT:
	    གལ་ཏེ (VIM_ISDIGIT(*rex.input) || !vim_isprintc(PTR2CHAR(rex.input)))
		status = RA_NOMATCH;
	    དེ་མིན
		ADVANCE_REGINPUT();
	    bha_nga;

	  pak_sa WHITE:
	    གལ་ཏེ (!VIM_ISWHITE(c))
		status = RA_NOMATCH;
	    དེ་མིན
		ADVANCE_REGINPUT();
	    bha_nga;

	  pak_sa NWHITE:
	    གལ་ཏེ (c == NUL || VIM_ISWHITE(c))
		status = RA_NOMATCH;
	    དེ་མིན
		ADVANCE_REGINPUT();
	    bha_nga;

	  pak_sa DIGIT:
	    གལ་ཏེ (!ri_digit(c))
		status = RA_NOMATCH;
	    དེ་མིན
		ADVANCE_REGINPUT();
	    bha_nga;

	  pak_sa NDIGIT:
	    གལ་ཏེ (c == NUL || ri_digit(c))
		status = RA_NOMATCH;
	    དེ་མིན
		ADVANCE_REGINPUT();
	    bha_nga;

	  pak_sa HEX:
	    གལ་ཏེ (!ri_hex(c))
		status = RA_NOMATCH;
	    དེ་མིན
		ADVANCE_REGINPUT();
	    bha_nga;

	  pak_sa NHEX:
	    གལ་ཏེ (c == NUL || ri_hex(c))
		status = RA_NOMATCH;
	    དེ་མིན
		ADVANCE_REGINPUT();
	    bha_nga;

	  pak_sa OCTAL:
	    གལ་ཏེ (!ri_octal(c))
		status = RA_NOMATCH;
	    དེ་མིན
		ADVANCE_REGINPUT();
	    bha_nga;

	  pak_sa NOCTAL:
	    གལ་ཏེ (c == NUL || ri_octal(c))
		status = RA_NOMATCH;
	    དེ་མིན
		ADVANCE_REGINPUT();
	    bha_nga;

	  pak_sa WORD:
	    གལ་ཏེ (!ri_word(c))
		status = RA_NOMATCH;
	    དེ་མིན
		ADVANCE_REGINPUT();
	    bha_nga;

	  pak_sa NWORD:
	    གལ་ཏེ (c == NUL || ri_word(c))
		status = RA_NOMATCH;
	    དེ་མིན
		ADVANCE_REGINPUT();
	    bha_nga;

	  pak_sa HEAD:
	    གལ་ཏེ (!ri_head(c))
		status = RA_NOMATCH;
	    དེ་མིན
		ADVANCE_REGINPUT();
	    bha_nga;

	  pak_sa NHEAD:
	    གལ་ཏེ (c == NUL || ri_head(c))
		status = RA_NOMATCH;
	    དེ་མིན
		ADVANCE_REGINPUT();
	    bha_nga;

	  pak_sa ALPHA:
	    གལ་ཏེ (!ri_alpha(c))
		status = RA_NOMATCH;
	    དེ་མིན
		ADVANCE_REGINPUT();
	    bha_nga;

	  pak_sa NALPHA:
	    གལ་ཏེ (c == NUL || ri_alpha(c))
		status = RA_NOMATCH;
	    དེ་མིན
		ADVANCE_REGINPUT();
	    bha_nga;

	  pak_sa LOWER:
	    གལ་ཏེ (!ri_lower(c))
		status = RA_NOMATCH;
	    དེ་མིན
		ADVANCE_REGINPUT();
	    bha_nga;

	  pak_sa NLOWER:
	    གལ་ཏེ (c == NUL || ri_lower(c))
		status = RA_NOMATCH;
	    དེ་མིན
		ADVANCE_REGINPUT();
	    bha_nga;

	  pak_sa UPPER:
	    གལ་ཏེ (!ri_upper(c))
		status = RA_NOMATCH;
	    དེ་མིན
		ADVANCE_REGINPUT();
	    bha_nga;

	  pak_sa NUPPER:
	    གལ་ཏེ (c == NUL || ri_upper(c))
		status = RA_NOMATCH;
	    དེ་མིན
		ADVANCE_REGINPUT();
	    bha_nga;

	  pak_sa EXACTLY:
	    {
		ཧྲིལ	len;
		char_u	*opnd;

		opnd = OPERAND(scan);
		// Inline the first byte, for speed.
		གལ་ཏེ (*opnd != *rex.input
			&& (!rex.reg_ic
			    || (!enc_utf8
			      && MB_TOLOWER(*opnd) != MB_TOLOWER(*rex.input))))
		    status = RA_NOMATCH;
		དེ་མིན གལ་ཏེ (*opnd == NUL)
		{
		    // match empty string always works; happens when "~" is
		    // empty.
		}
		དེ་མིན
		{
		    གལ་ཏེ (opnd[1] == NUL && !(enc_utf8 && rex.reg_ic))
		    {
			len = 1;	// matched a single byte above
		    }
		    དེ་མིན
		    {
			// Need to match first byte again for multi-byte.
			len = (ཧྲིལ)STRLEN(opnd);
			གལ་ཏེ (cstrncmp(opnd, rex.input, &len) != 0)
			    status = RA_NOMATCH;
		    }
		    // Check for following composing character, unless %C
		    // follows (skips over all composing chars).
		    གལ་ཏེ (status != RA_NOMATCH
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
		    གལ་ཏེ (status != RA_NOMATCH)
			rex.input += len;
		}
	    }
	    bha_nga;

	  pak_sa ANYOF:
	  pak_sa ANYBUT:
	    {
		char_u  *q = OPERAND(scan);

		གལ་ཏེ (c == NUL)
		    status = RA_NOMATCH;
		དེ་མིན གལ་ཏེ ((cstrchr(q, c) == NULL) == (op == ANYOF))
		    status = RA_NOMATCH;
		དེ་མིན
		{
		    // Check following combining characters
		    ཧྲིལ	len = 0;
		    ཧྲིལ i;

		    གལ་ཏེ (enc_utf8)
			len = utfc_ptr2len(q) - utf_ptr2len(q);

		    MB_CPTR_ADV(rex.input);
		    MB_CPTR_ADV(q);

		    གལ་ཏེ (!enc_utf8 || len == 0)
			bha_nga;

		    ཆེད་དུ (i = 0; i < len; ++i)
			གལ་ཏེ (q[i] != rex.input[i])
			{
			    status = RA_NOMATCH;
			    bha_nga;
			}
		    rex.input += len;
		}
		bha_nga;
	    }

	  pak_sa MULTIBYTECODE:
	    གལ་ཏེ (has_mbyte)
	    {
		ཧྲིལ	i, len;
		char_u	*opnd;
		ཧྲིལ	opndc = 0, inpc;

		opnd = OPERAND(scan);
		// Safety check (just in case 'encoding' was changed since
		// compiling the program).
		གལ་ཏེ ((len = (*mb_ptr2len)(opnd)) < 2)
		{
		    status = RA_NOMATCH;
		    bha_nga;
		}
		གལ་ཏེ (enc_utf8)
		    opndc = utf_ptr2char(opnd);
		གལ་ཏེ (enc_utf8 && utf_iscomposing(opndc))
		{
		    // When only a composing char is given match at any
		    // position where that composing char appears.
		    status = RA_NOMATCH;
		    ཆེད་དུ (i = 0; rex.input[i] != NUL;
						i += utf_ptr2len(rex.input + i))
		    {
			inpc = utf_ptr2char(rex.input + i);
			གལ་ཏེ (!utf_iscomposing(inpc))
			{
			    གལ་ཏེ (i > 0)
				bha_nga;
			}
			དེ་མིན གལ་ཏེ (opndc == inpc)
			{
			    // Include all following composing chars.
			    len = i + utfc_ptr2len(rex.input + i);
			    status = RA_MATCH;
			    bha_nga;
			}
		    }
		}
		དེ་མིན གལ་ཏེ (enc_utf8)
		{
		    གལ་ཏེ (cstrncmp(opnd, rex.input, &len) != 0)
		    {
			status = RA_NOMATCH;
			bha_nga;
		    }
		}
		དེ་མིན
		    ཆེད་དུ (i = 0; i < len; ++i)
			གལ་ཏེ (opnd[i] != rex.input[i])
			{
			    status = RA_NOMATCH;
			    bha_nga;
			}
		rex.input += len;
	    }
	    དེ་མིན
		status = RA_NOMATCH;
	    bha_nga;
	  pak_sa RE_COMPOSING:
	    གལ་ཏེ (enc_utf8)
	    {
		// Skip composing characters.
		སྐབས (utf_iscomposing(utf_ptr2char(rex.input)))
		    MB_CPTR_ADV(rex.input);
	    }
	    bha_nga;

	  pak_sa NOTHING:
	    bha_nga;

	  pak_sa BACK:
	    {
		ཧྲིལ		i;
		backpos_T	*bp;

		// When we run into BACK we need to check if we don't keep
		// looping without matching any input.  The second and later
		// times a BACK is encountered it fails if the input is still
		// at the same position as the previous time.
		// The positions are stored in "backpos" and found by the
		// current value of "scan", the position in the RE program.
		bp = (backpos_T *)backpos.ga_data;
		ཆེད་དུ (i = 0; i < backpos.ga_len; ++i)
		    གལ་ཏེ (bp[i].bp_scan == scan)
			bha_nga;
		གལ་ཏེ (i == backpos.ga_len)
		{
		    // First time at this BACK, make room to store the pos.
		    གལ་ཏེ (GA_GROW_FAILS(&backpos, 1))
			status = RA_FAIL;
		    དེ་མིན
		    {
			// get "ga_data" again, it may have changed
			bp = (backpos_T *)backpos.ga_data;
			bp[i].bp_scan = scan;
			++backpos.ga_len;
		    }
		}
		དེ་མིན གལ་ཏེ (reg_save_equal(&bp[i].bp_pos))
		    // Still at same position as last time, fail.
		    status = RA_NOMATCH;

		གལ་ཏེ (status != RA_FAIL && status != RA_NOMATCH)
		    reg_save(&bp[i].bp_pos, &backpos);
	    }
	    bha_nga;

	  pak_sa MOPEN + 0:   // Match start: \zs
	  pak_sa MOPEN + 1:   // \(
	  pak_sa MOPEN + 2:
	  pak_sa MOPEN + 3:
	  pak_sa MOPEN + 4:
	  pak_sa MOPEN + 5:
	  pak_sa MOPEN + 6:
	  pak_sa MOPEN + 7:
	  pak_sa MOPEN + 8:
	  pak_sa MOPEN + 9:
	    {
		no = op - MOPEN;
		cleanup_subexpr();
		rp = regstack_push(RS_MOPEN, scan);
		གལ་ཏེ (rp == NULL)
		    status = RA_FAIL;
		དེ་མིན
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_startpos[no],
							  &rex.reg_startp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    bha_nga;

	  pak_sa NOPEN:	    // \%(
	  pak_sa NCLOSE:	    // \) after \%(
		གལ་ཏེ (regstack_push(RS_NOPEN, scan) == NULL)
		    status = RA_FAIL;
		// We simply continue and handle the result when done.
		bha_nga;

#ifdef FEAT_SYN_HL
	  pak_sa ZOPEN + 1:
	  pak_sa ZOPEN + 2:
	  pak_sa ZOPEN + 3:
	  pak_sa ZOPEN + 4:
	  pak_sa ZOPEN + 5:
	  pak_sa ZOPEN + 6:
	  pak_sa ZOPEN + 7:
	  pak_sa ZOPEN + 8:
	  pak_sa ZOPEN + 9:
	    {
		no = op - ZOPEN;
		cleanup_zsubexpr();
		rp = regstack_push(RS_ZOPEN, scan);
		གལ་ཏེ (rp == NULL)
		    status = RA_FAIL;
		དེ་མིན
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_startzpos[no],
							     &reg_startzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    bha_nga;
#endif

	  pak_sa MCLOSE + 0:  // Match end: \ze
	  pak_sa MCLOSE + 1:  // \)
	  pak_sa MCLOSE + 2:
	  pak_sa MCLOSE + 3:
	  pak_sa MCLOSE + 4:
	  pak_sa MCLOSE + 5:
	  pak_sa MCLOSE + 6:
	  pak_sa MCLOSE + 7:
	  pak_sa MCLOSE + 8:
	  pak_sa MCLOSE + 9:
	    {
		no = op - MCLOSE;
		cleanup_subexpr();
		rp = regstack_push(RS_MCLOSE, scan);
		གལ་ཏེ (rp == NULL)
		    status = RA_FAIL;
		དེ་མིན
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_endpos[no],
							    &rex.reg_endp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    bha_nga;

#ifdef FEAT_SYN_HL
	  pak_sa ZCLOSE + 1:  // \) after \z(
	  pak_sa ZCLOSE + 2:
	  pak_sa ZCLOSE + 3:
	  pak_sa ZCLOSE + 4:
	  pak_sa ZCLOSE + 5:
	  pak_sa ZCLOSE + 6:
	  pak_sa ZCLOSE + 7:
	  pak_sa ZCLOSE + 8:
	  pak_sa ZCLOSE + 9:
	    {
		no = op - ZCLOSE;
		cleanup_zsubexpr();
		rp = regstack_push(RS_ZCLOSE, scan);
		གལ་ཏེ (rp == NULL)
		    status = RA_FAIL;
		དེ་མིན
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_endzpos[no],
							      &reg_endzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    bha_nga;
#endif

	  pak_sa BACKREF + 1:
	  pak_sa BACKREF + 2:
	  pak_sa BACKREF + 3:
	  pak_sa BACKREF + 4:
	  pak_sa BACKREF + 5:
	  pak_sa BACKREF + 6:
	  pak_sa BACKREF + 7:
	  pak_sa BACKREF + 8:
	  pak_sa BACKREF + 9:
	    {
		ཧྲིལ		len;

		no = op - BACKREF;
		cleanup_subexpr();
		གལ་ཏེ (!REG_MULTI)		// Single-line regexp
		{
		    གལ་ཏེ (rex.reg_startp[no] == NULL || rex.reg_endp[no] == NULL)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    དེ་མིན
		    {
			// Compare current input with back-ref in the same
			// line.
			len = (ཧྲིལ)(rex.reg_endp[no] - rex.reg_startp[no]);
			གལ་ཏེ (cstrncmp(rex.reg_startp[no], rex.input, &len) != 0)
			    status = RA_NOMATCH;
		    }
		}
		དེ་མིན				// Multi-line regexp
		{
		    གལ་ཏེ (rex.reg_startpos[no].lnum < 0
						|| rex.reg_endpos[no].lnum < 0)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    དེ་མིན
		    {
			གལ་ཏེ (rex.reg_startpos[no].lnum == rex.lnum
				&& rex.reg_endpos[no].lnum == rex.lnum)
			{
			    // Compare back-ref within the current line.
			    len = rex.reg_endpos[no].col
						    - rex.reg_startpos[no].col;
			    གལ་ཏེ (cstrncmp(rex.line + rex.reg_startpos[no].col,
							  rex.input, &len) != 0)
				status = RA_NOMATCH;
			}
			དེ་མིན
			{
			    // Messy situation: Need to compare between two
			    // lines.
			    ཧྲིལ r = match_with_backref(
					    rex.reg_startpos[no].lnum,
					    rex.reg_startpos[no].col,
					    rex.reg_endpos[no].lnum,
					    rex.reg_endpos[no].col,
					    &len);

			    གལ་ཏེ (r != RA_MATCH)
				status = r;
			}
		    }
		}

		// Matched the backref, skip over it.
		rex.input += len;
	    }
	    bha_nga;

#ifdef FEAT_SYN_HL
	  pak_sa ZREF + 1:
	  pak_sa ZREF + 2:
	  pak_sa ZREF + 3:
	  pak_sa ZREF + 4:
	  pak_sa ZREF + 5:
	  pak_sa ZREF + 6:
	  pak_sa ZREF + 7:
	  pak_sa ZREF + 8:
	  pak_sa ZREF + 9:
	    {
		ཧྲིལ	len;

		cleanup_zsubexpr();
		no = op - ZREF;
		གལ་ཏེ (re_extmatch_in != NULL
			&& re_extmatch_in->matches[no] != NULL)
		{
		    len = (ཧྲིལ)STRLEN(re_extmatch_in->matches[no]);
		    གལ་ཏེ (cstrncmp(re_extmatch_in->matches[no],
							  rex.input, &len) != 0)
			status = RA_NOMATCH;
		    དེ་མིན
			rex.input += len;
		}
		དེ་མིན
		{
		    // Backref was not set: Match an empty string.
		}
	    }
	    bha_nga;
#endif

	  pak_sa BRANCH:
	    {
		གལ་ཏེ (OP(next) != BRANCH) // No choice.
		    next = OPERAND(scan);	// Avoid recursion.
		དེ་མིན
		{
		    rp = regstack_push(RS_BRANCH, scan);
		    གལ་ཏེ (rp == NULL)
			status = RA_FAIL;
		    དེ་མིན
			status = RA_BREAK;	// rest is below
		}
	    }
	    bha_nga;

	  pak_sa BRACE_LIMITS:
	    {
		གལ་ཏེ (OP(next) == BRACE_SIMPLE)
		{
		    bl_minval = OPERAND_MIN(scan);
		    bl_maxval = OPERAND_MAX(scan);
		}
		དེ་མིན གལ་ཏེ (OP(next) >= BRACE_COMPLEX
			&& OP(next) < BRACE_COMPLEX + 10)
		{
		    no = OP(next) - BRACE_COMPLEX;
		    brace_min[no] = OPERAND_MIN(scan);
		    brace_max[no] = OPERAND_MAX(scan);
		    brace_count[no] = 0;
		}
		དེ་མིན
		{
		    internal_error("BRACE_LIMITS");
		    status = RA_FAIL;
		}
	    }
	    bha_nga;

	  pak_sa BRACE_COMPLEX + 0:
	  pak_sa BRACE_COMPLEX + 1:
	  pak_sa BRACE_COMPLEX + 2:
	  pak_sa BRACE_COMPLEX + 3:
	  pak_sa BRACE_COMPLEX + 4:
	  pak_sa BRACE_COMPLEX + 5:
	  pak_sa BRACE_COMPLEX + 6:
	  pak_sa BRACE_COMPLEX + 7:
	  pak_sa BRACE_COMPLEX + 8:
	  pak_sa BRACE_COMPLEX + 9:
	    {
		no = op - BRACE_COMPLEX;
		++brace_count[no];

		// If not matched enough times yet, try one more
		གལ་ཏེ (brace_count[no] <= (brace_min[no] <= brace_max[no]
					     ? brace_min[no] : brace_max[no]))
		{
		    rp = regstack_push(RS_BRCPLX_MORE, scan);
		    གལ་ཏེ (rp == NULL)
			status = RA_FAIL;
		    དེ་མིན
		    {
			rp->rs_no = no;
			reg_save(&rp->rs_un.regsave, &backpos);
			next = OPERAND(scan);
			// We continue and handle the result when done.
		    }
		    bha_nga;
		}

		// If matched enough times, may try matching some more
		གལ་ཏེ (brace_min[no] <= brace_max[no])
		{
		    // Range is the normal way around, use longest match
		    གལ་ཏེ (brace_count[no] <= brace_max[no])
		    {
			rp = regstack_push(RS_BRCPLX_LONG, scan);
			གལ་ཏེ (rp == NULL)
			    status = RA_FAIL;
			དེ་མིན
			{
			    rp->rs_no = no;
			    reg_save(&rp->rs_un.regsave, &backpos);
			    next = OPERAND(scan);
			    // We continue and handle the result when done.
			}
		    }
		}
		དེ་མིན
		{
		    // Range is backwards, use shortest match first
		    གལ་ཏེ (brace_count[no] <= brace_min[no])
		    {
			rp = regstack_push(RS_BRCPLX_SHORT, scan);
			གལ་ཏེ (rp == NULL)
			    status = RA_FAIL;
			དེ་མིན
			{
			    reg_save(&rp->rs_un.regsave, &backpos);
			    // We continue and handle the result when done.
			}
		    }
		}
	    }
	    bha_nga;

	  pak_sa BRACE_SIMPLE:
	  pak_sa STAR:
	  pak_sa PLUS:
	    {
		regstar_T	rst;

		// Lookahead to avoid useless match attempts when we know
		// what character comes next.
		གལ་ཏེ (OP(next) == EXACTLY)
		{
		    rst.nextb = *OPERAND(next);
		    གལ་ཏེ (rex.reg_ic)
		    {
			གལ་ཏེ (MB_ISUPPER(rst.nextb))
			    rst.nextb_ic = MB_TOLOWER(rst.nextb);
			དེ་མིན
			    rst.nextb_ic = MB_TOUPPER(rst.nextb);
		    }
		    དེ་མིན
			rst.nextb_ic = rst.nextb;
		}
		དེ་མིན
		{
		    rst.nextb = NUL;
		    rst.nextb_ic = NUL;
		}
		གལ་ཏེ (op != BRACE_SIMPLE)
		{
		    rst.minval = (op == STAR) ? 0 : 1;
		    rst.maxval = MAX_LIMIT;
		}
		དེ་མིན
		{
		    rst.minval = bl_minval;
		    rst.maxval = bl_maxval;
		}

		// When maxval > minval, try matching as much as possible, up
		// to maxval.  When maxval < minval, try matching at least the
		// minimal number (since the range is backwards, that's also
		// maxval!).
		rst.count = regrepeat(OPERAND(scan), rst.maxval);
		གལ་ཏེ (got_int)
		{
		    status = RA_FAIL;
		    bha_nga;
		}
		གལ་ཏེ (rst.minval <= rst.maxval
			  ? rst.count >= rst.minval : rst.count >= rst.maxval)
		{
		    // It could match.  Prepare for trying to match what
		    // follows.  The code is below.  Parameters are stored in
		    // a regstar_T on the regstack.
		    གལ་ཏེ ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
		    {
			emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
			status = RA_FAIL;
		    }
		    དེ་མིན གལ་ཏེ (GA_GROW_FAILS(&regstack, (ཧྲིལ)parimaa_na(regstar_T)))
			status = RA_FAIL;
		    དེ་མིན
		    {
			regstack.ga_len += parimaa_na(regstar_T);
			rp = regstack_push(rst.minval <= rst.maxval
					? RS_STAR_LONG : RS_STAR_SHORT, scan);
			གལ་ཏེ (rp == NULL)
			    status = RA_FAIL;
			དེ་མིན
			{
			    *(((regstar_T *)rp) - 1) = rst;
			    status = RA_BREAK;	    // skip the restore bits
			}
		    }
		}
		དེ་མིན
		    status = RA_NOMATCH;

	    }
	    bha_nga;

	  pak_sa NOMATCH:
	  pak_sa MATCH:
	  pak_sa SUBPAT:
	    rp = regstack_push(RS_NOMATCH, scan);
	    གལ་ཏེ (rp == NULL)
		status = RA_FAIL;
	    དེ་མིན
	    {
		rp->rs_no = op;
		reg_save(&rp->rs_un.regsave, &backpos);
		next = OPERAND(scan);
		// We continue and handle the result when done.
	    }
	    bha_nga;

	  pak_sa BEHIND:
	  pak_sa NOBEHIND:
	    // Need a bit of room to store extra positions.
	    གལ་ཏེ ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
	    {
		emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		status = RA_FAIL;
	    }
	    དེ་མིན གལ་ཏེ (GA_GROW_FAILS(&regstack, (ཧྲིལ)parimaa_na(regbehind_T)))
		status = RA_FAIL;
	    དེ་མིན
	    {
		regstack.ga_len += parimaa_na(regbehind_T);
		rp = regstack_push(RS_BEHIND1, scan);
		གལ་ཏེ (rp == NULL)
		    status = RA_FAIL;
		དེ་མིན
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
	    bha_nga;

	  pak_sa BHPOS:
	    གལ་ཏེ (REG_MULTI)
	    {
		གལ་ཏེ (behind_pos.rs_u.pos.col != (colnr_T)(rex.input - rex.line)
			|| behind_pos.rs_u.pos.lnum != rex.lnum)
		    status = RA_NOMATCH;
	    }
	    དེ་མིན གལ་ཏེ (behind_pos.rs_u.ptr != rex.input)
		status = RA_NOMATCH;
	    bha_nga;

	  pak_sa NEWL:
	    གལ་ཏེ ((c != NUL || !REG_MULTI || rex.lnum > rex.reg_maxline
			     || rex.reg_line_lbr)
					   && (c != '\n' || !rex.reg_line_lbr))
		status = RA_NOMATCH;
	    དེ་མིན གལ་ཏེ (rex.reg_line_lbr)
		ADVANCE_REGINPUT();
	    དེ་མིན
		reg_nextline();
	    bha_nga;

	  pak_sa END:
	    status = RA_MATCH;	// Success!
	    bha_nga;

	  default:
	    iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	    printf("Illegal op code %d\n", op);
#endif
	    status = RA_FAIL;
	    bha_nga;
	  }
	}

	// If we can't continue sequentially, break the inner loop.
	གལ་ཏེ (status != RA_CONT)
	    bha_nga;

	// Continue in inner loop, advance to next item.
	scan = next;

    } // end of inner loop

    // If there is something on the regstack execute the code for the state.
    // If the state is popped then loop and use the older state.
    སྐབས (regstack.ga_len > 0 && status != RA_FAIL)
    {
	rp = (regitem_T *)((var_na *)regstack.ga_data + regstack.ga_len) - 1;
	switch (rp->rs_state)
	{
	  pak_sa RS_NOPEN:
	    // Result is passed on as-is, simply pop the state.
	    regstack_pop(&scan);
	    bha_nga;

	  pak_sa RS_MOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    གལ་ཏེ (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_startpos[rp->rs_no],
						  &rex.reg_startp[rp->rs_no]);
	    regstack_pop(&scan);
	    bha_nga;

#ifdef FEAT_SYN_HL
	  pak_sa RS_ZOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    གལ་ཏེ (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_startzpos[rp->rs_no],
						 &reg_startzp[rp->rs_no]);
	    regstack_pop(&scan);
	    bha_nga;
#endif

	  pak_sa RS_MCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    གལ་ཏེ (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_endpos[rp->rs_no],
						    &rex.reg_endp[rp->rs_no]);
	    regstack_pop(&scan);
	    bha_nga;

#ifdef FEAT_SYN_HL
	  pak_sa RS_ZCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    གལ་ཏེ (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_endzpos[rp->rs_no],
						   &reg_endzp[rp->rs_no]);
	    regstack_pop(&scan);
	    bha_nga;
#endif

	  pak_sa RS_BRANCH:
	    གལ་ཏེ (status == RA_MATCH)
		// this branch matched, use it
		regstack_pop(&scan);
	    དེ་མིན
	    {
		གལ་ཏེ (status != RA_BREAK)
		{
		    // After a non-matching branch: try next one.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    scan = rp->rs_scan;
		}
		གལ་ཏེ (scan == NULL || OP(scan) != BRANCH)
		{
		    // no more branches, didn't find a match
		    status = RA_NOMATCH;
		    regstack_pop(&scan);
		}
		དེ་མིན
		{
		    // Prepare to try a branch.
		    rp->rs_scan = regnext(scan);
		    reg_save(&rp->rs_un.regsave, &backpos);
		    scan = OPERAND(scan);
		}
	    }
	    bha_nga;

	  pak_sa RS_BRCPLX_MORE:
	    // Pop the state.  Restore pointers when there is no match.
	    གལ་ཏེ (status == RA_NOMATCH)
	    {
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_count[rp->rs_no];	// decrement match count
	    }
	    regstack_pop(&scan);
	    bha_nga;

	  pak_sa RS_BRCPLX_LONG:
	    // Pop the state.  Restore pointers when there is no match.
	    གལ་ཏེ (status == RA_NOMATCH)
	    {
		// There was no match, but we did find enough matches.
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_count[rp->rs_no];
		// continue with the items after "\{}"
		status = RA_CONT;
	    }
	    regstack_pop(&scan);
	    གལ་ཏེ (status == RA_CONT)
		scan = regnext(scan);
	    bha_nga;

	  pak_sa RS_BRCPLX_SHORT:
	    // Pop the state.  Restore pointers when there is no match.
	    གལ་ཏེ (status == RA_NOMATCH)
		// There was no match, try to match one more item.
		reg_restore(&rp->rs_un.regsave, &backpos);
	    regstack_pop(&scan);
	    གལ་ཏེ (status == RA_NOMATCH)
	    {
		scan = OPERAND(scan);
		status = RA_CONT;
	    }
	    bha_nga;

	  pak_sa RS_NOMATCH:
	    // Pop the state.  If the operand matches for NOMATCH or
	    // doesn't match for MATCH/SUBPAT, we fail.  Otherwise backup,
	    // except for SUBPAT, and continue with the next item.
	    གལ་ཏེ (status == (rp->rs_no == NOMATCH ? RA_MATCH : RA_NOMATCH))
		status = RA_NOMATCH;
	    དེ་མིན
	    {
		status = RA_CONT;
		གལ་ཏེ (rp->rs_no != SUBPAT)	// zero-width
		    reg_restore(&rp->rs_un.regsave, &backpos);
	    }
	    regstack_pop(&scan);
	    གལ་ཏེ (status == RA_CONT)
		scan = regnext(scan);
	    bha_nga;

	  pak_sa RS_BEHIND1:
	    གལ་ཏེ (status == RA_NOMATCH)
	    {
		regstack_pop(&scan);
		regstack.ga_len -= parimaa_na(regbehind_T);
	    }
	    དེ་མིན
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
	    bha_nga;

	  pak_sa RS_BEHIND2:
	    // Looping for BEHIND / NOBEHIND match.
	    གལ་ཏེ (status == RA_MATCH && reg_save_equal(&behind_pos))
	    {
		// found a match that ends where "next" started
		behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		གལ་ཏེ (rp->rs_no == BEHIND)
		    reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
		དེ་མིན
		{
		    // But we didn't want a match.  Need to restore the
		    // subexpr, because what follows matched, so they have
		    // been set.
		    status = RA_NOMATCH;
		    restore_subexpr(((regbehind_T *)rp) - 1);
		}
		regstack_pop(&scan);
		regstack.ga_len -= parimaa_na(regbehind_T);
	    }
	    དེ་མིན
	    {
		long limit;

		// No match or a match that doesn't end where we want it: Go
		// back one character.  May go to previous line once.
		no = OK;
		limit = OPERAND_MIN(rp->rs_scan);
		གལ་ཏེ (REG_MULTI)
		{
		    གལ་ཏེ (limit > 0
			    && ((rp->rs_un.regsave.rs_u.pos.lnum
						    < behind_pos.rs_u.pos.lnum
				    ? (colnr_T)STRLEN(rex.line)
				    : behind_pos.rs_u.pos.col)
				- rp->rs_un.regsave.rs_u.pos.col >= limit))
			no = FAIL;
		    དེ་མིན གལ་ཏེ (rp->rs_un.regsave.rs_u.pos.col == 0)
		    {
			གལ་ཏེ (rp->rs_un.regsave.rs_u.pos.lnum
					< behind_pos.rs_u.pos.lnum
				|| reg_getline(
					--rp->rs_un.regsave.rs_u.pos.lnum)
								  == NULL)
			    no = FAIL;
			དེ་མིན
			{
			    reg_restore(&rp->rs_un.regsave, &backpos);
			    rp->rs_un.regsave.rs_u.pos.col =
						 (colnr_T)STRLEN(rex.line);
			}
		    }
		    དེ་མིན
		    {
			གལ་ཏེ (has_mbyte)
			{
			    char_u *line =
				  reg_getline(rp->rs_un.regsave.rs_u.pos.lnum);

			    rp->rs_un.regsave.rs_u.pos.col -=
				(*mb_head_off)(line, line
				    + rp->rs_un.regsave.rs_u.pos.col - 1) + 1;
			}
			དེ་མིན
			    --rp->rs_un.regsave.rs_u.pos.col;
		    }
		}
		དེ་མིན
		{
		    གལ་ཏེ (rp->rs_un.regsave.rs_u.ptr == rex.line)
			no = FAIL;
		    དེ་མིན
		    {
			MB_PTR_BACK(rex.line, rp->rs_un.regsave.rs_u.ptr);
			གལ་ཏེ (limit > 0 && (long)(behind_pos.rs_u.ptr
				     - rp->rs_un.regsave.rs_u.ptr) > limit)
			    no = FAIL;
		    }
		}
		གལ་ཏེ (no == OK)
		{
		    // Advanced, prepare for finding match again.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    scan = OPERAND(rp->rs_scan) + 4;
		    གལ་ཏེ (status == RA_MATCH)
		    {
			// We did match, so subexpr may have been changed,
			// need to restore them for the next try.
			status = RA_NOMATCH;
			restore_subexpr(((regbehind_T *)rp) - 1);
		    }
		}
		དེ་མིན
		{
		    // Can't advance.  For NOBEHIND that's a match.
		    behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		    གལ་ཏེ (rp->rs_no == NOBEHIND)
		    {
			reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
			status = RA_MATCH;
		    }
		    དེ་མིན
		    {
			// We do want a proper match.  Need to restore the
			// subexpr if we had a match, because they may have
			// been set.
			གལ་ཏེ (status == RA_MATCH)
			{
			    status = RA_NOMATCH;
			    restore_subexpr(((regbehind_T *)rp) - 1);
			}
		    }
		    regstack_pop(&scan);
		    regstack.ga_len -= parimaa_na(regbehind_T);
		}
	    }
	    bha_nga;

	  pak_sa RS_STAR_LONG:
	  pak_sa RS_STAR_SHORT:
	    {
		regstar_T	    *rst = ((regstar_T *)rp) - 1;

		གལ་ཏེ (status == RA_MATCH)
		{
		    regstack_pop(&scan);
		    regstack.ga_len -= parimaa_na(regstar_T);
		    bha_nga;
		}

		// Tried once already, restore input pointers.
		གལ་ཏེ (status != RA_BREAK)
		    reg_restore(&rp->rs_un.regsave, &backpos);

		// Repeat until we found a position where it could match.
		ཆེད་དུ (;;)
		{
		    གལ་ཏེ (status != RA_BREAK)
		    {
			// Tried first position already, advance.
			གལ་ཏེ (rp->rs_state == RS_STAR_LONG)
			{
			    // Trying for longest match, but couldn't or
			    // didn't match -- back up one char.
			    གལ་ཏེ (--rst->count < rst->minval)
				bha_nga;
			    གལ་ཏེ (rex.input == rex.line)
			    {
				// backup to last char of previous line
				གལ་ཏེ (rex.lnum == 0)
				{
				    status = RA_NOMATCH;
				    bha_nga;
				}
				--rex.lnum;
				rex.line = reg_getline(rex.lnum);
				// Just in case regrepeat() didn't count
				// right.
				གལ་ཏེ (rex.line == NULL)
				    bha_nga;
				rex.input = rex.line + reg_getline_len(rex.lnum);
				fast_breakcheck();
			    }
			    དེ་མིན
				MB_PTR_BACK(rex.line, rex.input);
			}
			དེ་མིན
			{
			    // Range is backwards, use shortest match first.
			    // Careful: maxval and minval are exchanged!
			    // Couldn't or didn't match: try advancing one
			    // char.
			    གལ་ཏེ (rst->count == rst->minval
				  || regrepeat(OPERAND(rp->rs_scan), 1L) == 0)
				bha_nga;
			    ++rst->count;
			}
			གལ་ཏེ (got_int)
			    bha_nga;
		    }
		    དེ་མིན
			status = RA_NOMATCH;

		    // If it could match, try it.
		    གལ་ཏེ (rst->nextb == NUL || *rex.input == rst->nextb
					     || *rex.input == rst->nextb_ic)
		    {
			reg_save(&rp->rs_un.regsave, &backpos);
			scan = regnext(rp->rs_scan);
			status = RA_CONT;
			bha_nga;
		    }
		}
		གལ་ཏེ (status != RA_CONT)
		{
		    // Failed.
		    regstack_pop(&scan);
		    regstack.ga_len -= parimaa_na(regstar_T);
		    status = RA_NOMATCH;
		}
	    }
	    bha_nga;
	}

	// If we want to continue the inner loop or didn't pop a state
	// continue matching loop
	གལ་ཏེ (status == RA_CONT || rp == (regitem_T *)
			     ((var_na *)regstack.ga_data + regstack.ga_len) - 1)
	    bha_nga;

#ifdef FEAT_RELTIME
	གལ་ཏེ (bt_did_time_out(timed_out))
	{
	    status = RA_FAIL;
	    bha_nga;
	}
#endif
    }

    // May need to continue with the inner loop, starting at "scan".
    གལ་ཏེ (status == RA_CONT)
	continue;

    // If the regstack is empty or something failed we are done.
    གལ་ཏེ (regstack.ga_len == 0 || status == RA_FAIL)
    {
	གལ་ཏེ (scan == NULL)
	{
	    // We get here only if there's trouble -- normally "case END" is
	    // the terminating point.
	    iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	    printf("Premature EOL\n");
#endif
	}
	ཕྱིར (status == RA_MATCH);
    }

  } // End of loop until the regstack is empty.

  // NOTREACHED
}

/*
 * regtry - try match of "prog" with at rex.line["col"].
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    static long
regtry(
    bt_regprog_T	*prog,
    colnr_T		col,
    ཧྲིལ			*timed_out)	// flag set on timeout or NULL
{
    rex.input = rex.line + col;
    rex.need_clear_subexpr = TRUE;
#ifdef FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    rex.need_clear_zsubexpr = (prog->reghasz == REX_SET);
#endif

    གལ་ཏེ (regmatch(prog->program + 1, timed_out) == 0)
	ཕྱིར 0;

    cleanup_subexpr();
    གལ་ཏེ (REG_MULTI)
    {
	གལ་ཏེ (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].col = col;
	}
	གལ་ཏེ (rex.reg_endpos[0].lnum < 0)
	{
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].col = (ཧྲིལ)(rex.input - rex.line);
	}
	དེ་མིན
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    དེ་མིན
    {
	གལ་ཏེ (rex.reg_startp[0] == NULL)
	    rex.reg_startp[0] = rex.line + col;
	གལ་ཏེ (rex.reg_endp[0] == NULL)
	    rex.reg_endp[0] = rex.input;
    }
#ifdef FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = NULL;

    གལ་ཏེ (prog->reghasz == REX_SET)
    {
	ཧྲིལ		i;

	cleanup_zsubexpr();
	re_extmatch_out = make_extmatch();
	གལ་ཏེ (re_extmatch_out == NULL)
	    ཕྱིར 0;
	ཆེད་དུ (i = 0; i < NSUBEXP; i++)
	{
	    གལ་ཏེ (REG_MULTI)
	    {
		// Only accept single line matches.
		གལ་ཏེ (reg_startzpos[i].lnum >= 0
			&& reg_endzpos[i].lnum == reg_startzpos[i].lnum
			&& reg_endzpos[i].col >= reg_startzpos[i].col)
		    re_extmatch_out->matches[i] =
			vim_strnsave(reg_getline(reg_startzpos[i].lnum)
						       + reg_startzpos[i].col,
				   reg_endzpos[i].col - reg_startzpos[i].col);
	    }
	    དེ་མིན
	    {
		གལ་ཏེ (reg_startzp[i] != NULL && reg_endzp[i] != NULL)
		    re_extmatch_out->matches[i] =
			    vim_strnsave(reg_startzp[i],
						reg_endzp[i] - reg_startzp[i]);
	    }
	}
    }
#endif
    ཕྱིར 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    static long
bt_regexec_both(
    char_u	*line,
    colnr_T	startcol,	// column to start looking for match
    ཧྲིལ		*timed_out)	// flag set on timeout or NULL
{
    bt_regprog_T    *prog;
    char_u	    *s;
    colnr_T	    col = startcol;
    long	    retval = 0L;

    // Create "regstack" and "backpos" if they are not allocated yet.
    // We allocate *_INITIAL amount of bytes first and then set the grow size
    // to much bigger value to avoid many malloc calls in case of deep regular
    // expressions.
    གལ་ཏེ (regstack.ga_data == NULL)
    {
	// Use an item size of 1 byte, since we push different things
	// onto the regstack.
	ga_init2(&regstack, 1, REGSTACK_INITIAL);
	(void)ga_grow(&regstack, REGSTACK_INITIAL);
	regstack.ga_growsize = REGSTACK_INITIAL * 8;
    }

    གལ་ཏེ (backpos.ga_data == NULL)
    {
	ga_init2(&backpos, parimaa_na(backpos_T), BACKPOS_INITIAL);
	(void)ga_grow(&backpos, BACKPOS_INITIAL);
	backpos.ga_growsize = BACKPOS_INITIAL * 8;
    }

    གལ་ཏེ (REG_MULTI)
    {
	prog = (bt_regprog_T *)rex.reg_mmatch->regprog;
	line = reg_getline((linenr_T)0);
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    དེ་མིན
    {
	prog = (bt_regprog_T *)rex.reg_match->regprog;
	rex.reg_startp = rex.reg_match->startp;
	rex.reg_endp = rex.reg_match->endp;
    }

    // Be paranoid...
    གལ་ཏེ (prog == NULL || line == NULL)
    {
	iemsg(e_null_argument);
	goto theend;
    }

    // Check validity of program.
    གལ་ཏེ (prog_magic_wrong())
	goto theend;

    // If the start column is past the maximum column: no need to try.
    གལ་ཏེ (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	goto theend;

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    གལ་ཏེ (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    དེ་མིན གལ་ཏེ (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    གལ་ཏེ (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    // If there is a "must appear" string, look for it.
    གལ་ཏེ (prog->regmust != NULL)
    {
	ཧྲིལ c;

	གལ་ཏེ (has_mbyte)
	    c = (*mb_ptr2char)(prog->regmust);
	དེ་མིན
	    c = *prog->regmust;
	s = line + col;

	// This is used very often, esp. for ":global".  Use three versions of
	// the loop to avoid overhead of conditions.
	གལ་ཏེ (!rex.reg_ic && !has_mbyte)
	    སྐབས ((s = vim_strbyte(s, c)) != NULL)
	    {
		ཧྲིལ len = prog->regmlen;
		གལ་ཏེ (cstrncmp(s, prog->regmust, &len) == 0)
		    bha_nga;		// Found it.
		++s;
	    }
	དེ་མིན གལ་ཏེ (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	    སྐབས ((s = vim_strchr(s, c)) != NULL)
	    {
		ཧྲིལ len = prog->regmlen;
		གལ་ཏེ (cstrncmp(s, prog->regmust, &len) == 0)
		    bha_nga;		// Found it.
		MB_PTR_ADV(s);
	    }
	དེ་མིན
	    སྐབས ((s = cstrchr(s, c)) != NULL)
	    {
		ཧྲིལ len = prog->regmlen;
		གལ་ཏེ (cstrncmp(s, prog->regmust, &len) == 0)
		    bha_nga;		// Found it.
		MB_PTR_ADV(s);
	    }
	གལ་ཏེ (s == NULL)		// Not present.
	    goto theend;
    }

    rex.line = line;
    rex.lnum = 0;
    reg_toolong = FALSE;

    // Simplest case: Anchored match need be tried only once.
    གལ་ཏེ (prog->reganch)
    {
	ཧྲིལ	c;

	གལ་ཏེ (has_mbyte)
	    c = (*mb_ptr2char)(rex.line + col);
	དེ་མིན
	    c = rex.line[col];
	གལ་ཏེ (prog->regstart == NUL
		|| prog->regstart == c
		|| (rex.reg_ic
		    && (((enc_utf8 && utf_fold(prog->regstart) == utf_fold(c)))
			|| (c < 255 && prog->regstart < 255 &&
			    MB_TOLOWER(prog->regstart) == MB_TOLOWER(c)))))
	    retval = regtry(prog, col, timed_out);
	དེ་མིན
	    retval = 0;
    }
    དེ་མིན
    {
	// Messy cases:  unanchored match.
	སྐབས (!got_int)
	{
	    གལ་ཏེ (prog->regstart != NUL)
	    {
		// Skip until the char we know it must start with.
		// Used often, do some work to avoid call overhead.
		གལ་ཏེ (!rex.reg_ic && !has_mbyte)
		    s = vim_strbyte(rex.line + col, prog->regstart);
		དེ་མིན
		    s = cstrchr(rex.line + col, prog->regstart);
		གལ་ཏེ (s == NULL)
		{
		    retval = 0;
		    bha_nga;
		}
		col = (ཧྲིལ)(s - rex.line);
	    }

	    // Check for maximum column to try.
	    གལ་ཏེ (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	    {
		retval = 0;
		bha_nga;
	    }

	    retval = regtry(prog, col, timed_out);
	    གལ་ཏེ (retval > 0)
		bha_nga;

	    // if not currently on the first line, get it again
	    གལ་ཏེ (rex.lnum != 0)
	    {
		rex.lnum = 0;
		rex.line = reg_getline((linenr_T)0);
	    }
	    གལ་ཏེ (rex.line[col] == NUL)
		bha_nga;
	    གལ་ཏེ (has_mbyte)
		col += (*mb_ptr2len)(rex.line + col);
	    དེ་མིན
		++col;
#ifdef FEAT_RELTIME
	    གལ་ཏེ (bt_did_time_out(timed_out))
		bha_nga;
#endif
	}
    }

theend:
    // Free "reg_tofree" when it's a bit big.
    // Free regstack and backpos if they are bigger than their initial size.
    གལ་ཏེ (reg_tofreelen > 400)
	VIM_CLEAR(reg_tofree);
    གལ་ཏེ (regstack.ga_maxlen > REGSTACK_INITIAL)
	ga_clear(&regstack);
    གལ་ཏེ (backpos.ga_maxlen > BACKPOS_INITIAL)
	ga_clear(&backpos);

    གལ་ཏེ (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs
	// and \ze are used.
	གལ་ཏེ (REG_MULTI)
	{
	    lpos_T *start = &rex.reg_mmatch->startpos[0];
	    lpos_T *end = &rex.reg_mmatch->endpos[0];

	    གལ་ཏེ (end->lnum < start->lnum
			|| (end->lnum == start->lnum && end->col < start->col))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_mmatch->rmm_matchcol = col;
	}
	དེ་མིན
	{
	    གལ་ཏེ (rex.reg_match->endp[0] < rex.reg_match->startp[0])
		rex.reg_match->endp[0] = rex.reg_match->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_match->rm_matchcol = col;
	}
    }

    ཕྱིར retval;
}

/*
 * Match a regexp against a string.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 * if "line_lbr" is TRUE  consider a "\n" in "line" to be a line break.
 *
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    static ཧྲིལ
bt_regexec_nl(
    regmatch_T	*rmp,
    char_u	*line,	// string to match against
    colnr_T	col,	// column to start looking for match
    ཧྲིལ		line_lbr)
{
    rex.reg_match = rmp;
    rex.reg_mmatch = NULL;
    rex.reg_maxline = 0;
    rex.reg_line_lbr = line_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = NULL;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;

    ཕྱིར bt_regexec_both(line, col, NULL);
}

/*
 * Match a regexp against multiple lines.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 *
 * Return zero if there is no match.  Return number of lines contained in the
 * match otherwise.
 */
    static long
bt_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	col,		// column to start looking for match
    ཧྲིལ		*timed_out)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    ཕྱིར bt_regexec_both(NULL, col, timed_out);
}

/*
 * Compare a number with the operand of RE_LNUM, RE_COL or RE_VCOL.
 */
    static ཧྲིལ
re_num_cmp(long_u val, char_u *scan)
{
    long_u  n = OPERAND_MIN(scan);

    གལ་ཏེ (OPERAND_CMP(scan) == '>')
	ཕྱིར val > n;
    གལ་ཏེ (OPERAND_CMP(scan) == '<')
	ཕྱིར val < n;
    ཕྱིར val == n;
}

#ifdef BT_REGEXP_DUMP

/*
 * regdump - dump a regexp onto stdout in vaguely comprehensible form
 */
    static void
regdump(char_u *pattern, bt_regprog_T *r)
{
    char_u  *s;
    ཧྲིལ	    op = EXACTLY;	// Arbitrary non-END op.
    char_u  *next;
    char_u  *end = NULL;
    FILE    *f;

# ifdef BT_REGEXP_LOG
    f = fopen("bt_regexp_log.log", "a");
# དེ་མིན
    f = stdout;
# endif
    གལ་ཏེ (f == NULL)
	ཕྱིར;
    fprintf(f, "-------------------------------------\n\r\nregcomp(%s):\r\n", pattern);

    s = r->program + 1;
    // Loop until we find the END that isn't before a referred next (an END
    // can also appear in a NOMATCH operand).
    སྐབས ((op != END || s <= end) && s < r->program + r->regsz)
    {
	op = OP(s);
	fprintf(f, "%2d%s", (ཧྲིལ)(s - r->program), regprop(s)); // Where, what.
	next = (s + 3 <= r->program + r->regsz) ? regnext(s) : NULL;
	གལ་ཏེ (next == NULL)	// Next ptr.
	    fprintf(f, "(0)");
	དེ་མིན
	    fprintf(f, "(%d)", (ཧྲིལ)((s - r->program) + (next - s)));
	གལ་ཏེ (end < next)
	    end = next;
	གལ་ཏེ (op == BRACE_LIMITS)
	{
	    // Two ints
	    fprintf(f, " minval %ld, maxval %ld", OPERAND_MIN(s), OPERAND_MAX(s));
	    s += 8;
	}
	དེ་མིན གལ་ཏེ (op == BEHIND || op == NOBEHIND)
	{
	    // one int
	    fprintf(f, " count %ld", OPERAND_MIN(s));
	    s += 4;
	}
	དེ་མིན གལ་ཏེ (op == RE_LNUM || op == RE_COL || op == RE_VCOL)
	{
	    // one int plus comparator
	    fprintf(f, " count %ld", OPERAND_MIN(s));
	    s += 5;
	}
	s += 3;
	གལ་ཏེ (op == MULTIBYTECODE)
	{
	    fprintf(f, " mbc=%d", utf_ptr2char(s));
	    s += utfc_ptr2len(s);
	}
	གལ་ཏེ (op == ANYOF || op == ANYOF + ADD_NL
		|| op == ANYBUT || op == ANYBUT + ADD_NL
		|| op == EXACTLY)
	{
	    // Literal string, where present.
	    fprintf(f, "\nxxxxxxxxx\n");
	    སྐབས (*s != NUL && s < r->program + r->regsz)
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
    གལ་ཏེ (r->regstart != NUL)
	fprintf(f, "start `%s' 0x%x; ", r->regstart < 256
		? (var_na *)transchar(r->regstart)
		: "multibyte", r->regstart);
    གལ་ཏེ (r->reganch)
	fprintf(f, "anchored; ");
    གལ་ཏེ (r->regmust != NULL)
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
    static char_u *
regprop(char_u *op)
{
    var_na	    *p;
    static var_na	    buf[50];
    static size_t   buflen = 0;

    STRCPY(buf, ":");
    buflen = 1;

    switch ((ཧྲིལ) OP(op))
    {
      pak_sa BOL:
	p = "BOL";
	bha_nga;
      pak_sa EOL:
	p = "EOL";
	bha_nga;
      pak_sa RE_BOF:
	p = "BOF";
	bha_nga;
      pak_sa RE_EOF:
	p = "EOF";
	bha_nga;
      pak_sa CURSOR:
	p = "CURSOR";
	bha_nga;
      pak_sa RE_VISUAL:
	p = "RE_VISUAL";
	bha_nga;
      pak_sa RE_LNUM:
	p = "RE_LNUM";
	bha_nga;
      pak_sa RE_MARK:
	p = "RE_MARK";
	bha_nga;
      pak_sa RE_COL:
	p = "RE_COL";
	bha_nga;
      pak_sa RE_VCOL:
	p = "RE_VCOL";
	bha_nga;
      pak_sa BOW:
	p = "BOW";
	bha_nga;
      pak_sa EOW:
	p = "EOW";
	bha_nga;
      pak_sa ANY:
	p = "ANY";
	bha_nga;
      pak_sa ANY + ADD_NL:
	p = "ANY+NL";
	bha_nga;
      pak_sa ANYOF:
	p = "ANYOF";
	bha_nga;
      pak_sa ANYOF + ADD_NL:
	p = "ANYOF+NL";
	bha_nga;
      pak_sa ANYBUT:
	p = "ANYBUT";
	bha_nga;
      pak_sa ANYBUT + ADD_NL:
	p = "ANYBUT+NL";
	bha_nga;
      pak_sa IDENT:
	p = "IDENT";
	bha_nga;
      pak_sa IDENT + ADD_NL:
	p = "IDENT+NL";
	bha_nga;
      pak_sa SIDENT:
	p = "SIDENT";
	bha_nga;
      pak_sa SIDENT + ADD_NL:
	p = "SIDENT+NL";
	bha_nga;
      pak_sa KWORD:
	p = "KWORD";
	bha_nga;
      pak_sa KWORD + ADD_NL:
	p = "KWORD+NL";
	bha_nga;
      pak_sa SKWORD:
	p = "SKWORD";
	bha_nga;
      pak_sa SKWORD + ADD_NL:
	p = "SKWORD+NL";
	bha_nga;
      pak_sa FNAME:
	p = "FNAME";
	bha_nga;
      pak_sa FNAME + ADD_NL:
	p = "FNAME+NL";
	bha_nga;
      pak_sa SFNAME:
	p = "SFNAME";
	bha_nga;
      pak_sa SFNAME + ADD_NL:
	p = "SFNAME+NL";
	bha_nga;
      pak_sa PRINT:
	p = "PRINT";
	bha_nga;
      pak_sa PRINT + ADD_NL:
	p = "PRINT+NL";
	bha_nga;
      pak_sa SPRINT:
	p = "SPRINT";
	bha_nga;
      pak_sa SPRINT + ADD_NL:
	p = "SPRINT+NL";
	bha_nga;
      pak_sa WHITE:
	p = "WHITE";
	bha_nga;
      pak_sa WHITE + ADD_NL:
	p = "WHITE+NL";
	bha_nga;
      pak_sa NWHITE:
	p = "NWHITE";
	bha_nga;
      pak_sa NWHITE + ADD_NL:
	p = "NWHITE+NL";
	bha_nga;
      pak_sa DIGIT:
	p = "DIGIT";
	bha_nga;
      pak_sa DIGIT + ADD_NL:
	p = "DIGIT+NL";
	bha_nga;
      pak_sa NDIGIT:
	p = "NDIGIT";
	bha_nga;
      pak_sa NDIGIT + ADD_NL:
	p = "NDIGIT+NL";
	bha_nga;
      pak_sa HEX:
	p = "HEX";
	bha_nga;
      pak_sa HEX + ADD_NL:
	p = "HEX+NL";
	bha_nga;
      pak_sa NHEX:
	p = "NHEX";
	bha_nga;
      pak_sa NHEX + ADD_NL:
	p = "NHEX+NL";
	bha_nga;
      pak_sa OCTAL:
	p = "OCTAL";
	bha_nga;
      pak_sa OCTAL + ADD_NL:
	p = "OCTAL+NL";
	bha_nga;
      pak_sa NOCTAL:
	p = "NOCTAL";
	bha_nga;
      pak_sa NOCTAL + ADD_NL:
	p = "NOCTAL+NL";
	bha_nga;
      pak_sa WORD:
	p = "WORD";
	bha_nga;
      pak_sa WORD + ADD_NL:
	p = "WORD+NL";
	bha_nga;
      pak_sa NWORD:
	p = "NWORD";
	bha_nga;
      pak_sa NWORD + ADD_NL:
	p = "NWORD+NL";
	bha_nga;
      pak_sa HEAD:
	p = "HEAD";
	bha_nga;
      pak_sa HEAD + ADD_NL:
	p = "HEAD+NL";
	bha_nga;
      pak_sa NHEAD:
	p = "NHEAD";
	bha_nga;
      pak_sa NHEAD + ADD_NL:
	p = "NHEAD+NL";
	bha_nga;
      pak_sa ALPHA:
	p = "ALPHA";
	bha_nga;
      pak_sa ALPHA + ADD_NL:
	p = "ALPHA+NL";
	bha_nga;
      pak_sa NALPHA:
	p = "NALPHA";
	bha_nga;
      pak_sa NALPHA + ADD_NL:
	p = "NALPHA+NL";
	bha_nga;
      pak_sa LOWER:
	p = "LOWER";
	bha_nga;
      pak_sa LOWER + ADD_NL:
	p = "LOWER+NL";
	bha_nga;
      pak_sa NLOWER:
	p = "NLOWER";
	bha_nga;
      pak_sa NLOWER + ADD_NL:
	p = "NLOWER+NL";
	bha_nga;
      pak_sa UPPER:
	p = "UPPER";
	bha_nga;
      pak_sa UPPER + ADD_NL:
	p = "UPPER+NL";
	bha_nga;
      pak_sa NUPPER:
	p = "NUPPER";
	bha_nga;
      pak_sa NUPPER + ADD_NL:
	p = "NUPPER+NL";
	bha_nga;
      pak_sa BRANCH:
	p = "BRANCH";
	bha_nga;
      pak_sa EXACTLY:
	p = "EXACTLY";
	bha_nga;
      pak_sa NOTHING:
	p = "NOTHING";
	bha_nga;
      pak_sa BACK:
	p = "BACK";
	bha_nga;
      pak_sa END:
	p = "END";
	bha_nga;
      pak_sa MOPEN + 0:
	p = "MATCH START";
	bha_nga;
      pak_sa MOPEN + 1:
      pak_sa MOPEN + 2:
      pak_sa MOPEN + 3:
      pak_sa MOPEN + 4:
      pak_sa MOPEN + 5:
      pak_sa MOPEN + 6:
      pak_sa MOPEN + 7:
      pak_sa MOPEN + 8:
      pak_sa MOPEN + 9:
	buflen += vim_snprintf(buf + buflen, parimaa_na(buf) - buflen, "MOPEN%d", OP(op) - MOPEN);
	p = NULL;
	bha_nga;
      pak_sa MCLOSE + 0:
	p = "MATCH END";
	bha_nga;
      pak_sa MCLOSE + 1:
      pak_sa MCLOSE + 2:
      pak_sa MCLOSE + 3:
      pak_sa MCLOSE + 4:
      pak_sa MCLOSE + 5:
      pak_sa MCLOSE + 6:
      pak_sa MCLOSE + 7:
      pak_sa MCLOSE + 8:
      pak_sa MCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, parimaa_na(buf) - buflen, "MCLOSE%d", OP(op) - MCLOSE);
	p = NULL;
	bha_nga;
      pak_sa BACKREF + 1:
      pak_sa BACKREF + 2:
      pak_sa BACKREF + 3:
      pak_sa BACKREF + 4:
      pak_sa BACKREF + 5:
      pak_sa BACKREF + 6:
      pak_sa BACKREF + 7:
      pak_sa BACKREF + 8:
      pak_sa BACKREF + 9:
	buflen += vim_snprintf(buf + buflen, parimaa_na(buf) - buflen, "BACKREF%d", OP(op) - BACKREF);
	p = NULL;
	bha_nga;
      pak_sa NOPEN:
	p = "NOPEN";
	bha_nga;
      pak_sa NCLOSE:
	p = "NCLOSE";
	bha_nga;
# ifdef FEAT_SYN_HL
      pak_sa ZOPEN + 1:
      pak_sa ZOPEN + 2:
      pak_sa ZOPEN + 3:
      pak_sa ZOPEN + 4:
      pak_sa ZOPEN + 5:
      pak_sa ZOPEN + 6:
      pak_sa ZOPEN + 7:
      pak_sa ZOPEN + 8:
      pak_sa ZOPEN + 9:
	buflen += vim_snprintf(buf + buflen, parimaa_na(buf) - buflen, "ZOPEN%d", OP(op) - ZOPEN);
	p = NULL;
	bha_nga;
      pak_sa ZCLOSE + 1:
      pak_sa ZCLOSE + 2:
      pak_sa ZCLOSE + 3:
      pak_sa ZCLOSE + 4:
      pak_sa ZCLOSE + 5:
      pak_sa ZCLOSE + 6:
      pak_sa ZCLOSE + 7:
      pak_sa ZCLOSE + 8:
      pak_sa ZCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, parimaa_na(buf) - buflen, "ZCLOSE%d", OP(op) - ZCLOSE);
	p = NULL;
	bha_nga;
      pak_sa ZREF + 1:
      pak_sa ZREF + 2:
      pak_sa ZREF + 3:
      pak_sa ZREF + 4:
      pak_sa ZREF + 5:
      pak_sa ZREF + 6:
      pak_sa ZREF + 7:
      pak_sa ZREF + 8:
      pak_sa ZREF + 9:
	buflen += vim_snprintf(buf + buflen, parimaa_na(buf) - buflen, "ZREF%d", OP(op) - ZREF);
	p = NULL;
	bha_nga;
# endif
      pak_sa STAR:
	p = "STAR";
	bha_nga;
      pak_sa PLUS:
	p = "PLUS";
	bha_nga;
      pak_sa NOMATCH:
	p = "NOMATCH";
	bha_nga;
      pak_sa MATCH:
	p = "MATCH";
	bha_nga;
      pak_sa BEHIND:
	p = "BEHIND";
	bha_nga;
      pak_sa NOBEHIND:
	p = "NOBEHIND";
	bha_nga;
      pak_sa SUBPAT:
	p = "SUBPAT";
	bha_nga;
      pak_sa BRACE_LIMITS:
	p = "BRACE_LIMITS";
	bha_nga;
      pak_sa BRACE_SIMPLE:
	p = "BRACE_SIMPLE";
	bha_nga;
      pak_sa BRACE_COMPLEX + 0:
      pak_sa BRACE_COMPLEX + 1:
      pak_sa BRACE_COMPLEX + 2:
      pak_sa BRACE_COMPLEX + 3:
      pak_sa BRACE_COMPLEX + 4:
      pak_sa BRACE_COMPLEX + 5:
      pak_sa BRACE_COMPLEX + 6:
      pak_sa BRACE_COMPLEX + 7:
      pak_sa BRACE_COMPLEX + 8:
      pak_sa BRACE_COMPLEX + 9:
	buflen += vim_snprintf(buf + buflen, parimaa_na(buf) - buflen, "BRACE_COMPLEX%d", OP(op) - BRACE_COMPLEX);
	p = NULL;
	bha_nga;
      pak_sa MULTIBYTECODE:
	p = "MULTIBYTECODE";
	bha_nga;
      pak_sa NEWL:
	p = "NEWL";
	bha_nga;
      default:
	buflen += vim_snprintf(buf + buflen, parimaa_na(buf) - buflen, "corrupt %d", OP(op));
	p = NULL;
	bha_nga;
    }
    གལ་ཏེ (p != NULL)
	vim_strncpy((char_u *)buf + buflen, (char_u *)p, parimaa_na(buf) - buflen - 1);
    ཕྱིར (char_u *)buf;
}
#endif	    // DEBUG
