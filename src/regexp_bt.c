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

स्थिर पूर्णाङ्क	अङ्कः_complex_braces; // Complex \{...} count
स्थिर char_u	*regcode;	// Code-emit pointer, or JUST_CALC_SIZE
स्थिर long	regsize;	// Code size.
स्थिर पूर्णाङ्क	reg_toolong;	// TRUE when offset out of range
स्थिर पूर्णाङ्क	bt_reg_parse_depth;	// nesting depth in reg()
स्थिर char_u	had_endbrace[NSUBEXP];	// flags, TRUE if end of () found
स्थिर long	brace_min[10];	// Minimums for complex brace repeats
स्थिर long	brace_max[10];	// Maximums for complex brace repeats
स्थिर पूर्णाङ्क	brace_गणनम्[10]; // Current counts for complex brace repeats
स्थिर पूर्णाङ्क	one_exactly = FALSE;	// only do one char for EXACTLY

// When making changes to classchars also change nfa_classcodes.
स्थिर char_u	*classchars = (char_u *)".iIkKfFpPsSdDxXoOwWhHaAlLuU";
स्थिर ध्रुव पूर्णाङ्क classcodes[] = {
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
    स्थिर पूर्णाङ्क
bt_recognize_char_class(char_u *start, char_u *end)
{
    पूर्णाङ्क		newl;
    पूर्णाङ्क		config = get_char_class_bits(start, end, &newl);

    // A newline would need the ADD_NL variant, don't bother with it here.
    जइ (config < 0 || newl)
	प्रत्यागम 0;

	// The old engine has no case-insensitive class opcode, so [a-z] and [A-Z]
	// are left as collections.  The classes below are case-independent.
    विकल्प (config)
    {
	पक्ष CLASS_o9:
	    प्रत्यागम DIGIT;
	पक्ष CLASS_not | CLASS_o9:
	    प्रत्यागम NDIGIT;
	पक्ष CLASS_af | CLASS_AF | CLASS_o9:
	    प्रत्यागम HEX;
	पक्ष CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    प्रत्यागम NHEX;
	पक्ष CLASS_o7:
	    प्रत्यागम OCTAL;
	पक्ष CLASS_not | CLASS_o7:
	    प्रत्यागम NOCTAL;
	पक्ष CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    प्रत्यागम WORD;
	पक्ष CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    प्रत्यागम NWORD;
	पक्ष CLASS_az | CLASS_AZ | CLASS_underscore:
	    प्रत्यागम HEAD;
	पक्ष CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    प्रत्यागम NHEAD;
	पक्ष CLASS_az | CLASS_AZ:
	    प्रत्यागम ALPHA;
	पक्ष CLASS_not | CLASS_az | CLASS_AZ:
	    प्रत्यागम NALPHA;
    }
    प्रत्यागम 0;
}

/*
 * When regcode is set to this value, code is not emitted and size is computed
 * instead.
 */
#define JUST_CALC_SIZE	((char_u *) -1)

// Values for rs_state in regitem_T.
प्रकारनिर्देश गणना regstate_E
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
प्रकारनिर्देश संरचना
{
    सङ्घ
    {
	char_u	*ptr;	// rex.input pointer, for single-line regexp
	lpos_T	pos;	// rex.input pos, for multi-line regexp
    } rs_u;
    पूर्णाङ्क		rs_len;
} regsave_T;

// struct to save start/end pointer/position in for \(\)
प्रकारनिर्देश संरचना
{
    सङ्घ
    {
	char_u	*ptr;
	lpos_T	pos;
    } se_u;
} save_se_T;

// used for BEHIND and NOBEHIND matching
प्रकारनिर्देश संरचना regbehind_S
{
    regsave_T	save_after;
    regsave_T	save_behind;
    पूर्णाङ्क		save_need_clear_subexpr;
    save_se_T   save_start[NSUBEXP];
    save_se_T   save_end[NSUBEXP];
} regbehind_T;

/*
 * When there are alternatives a regstate_T is put on the regstack to remember
 * what we are doing.
 * Before it may be another type of item, depending on rs_state, to remember
 * more things.
 */
प्रकारनिर्देश संरचना regitem_S
{
    regstate_T	rs_state;	// what we are doing, one of RS_ above
    short	rs_no;		// submatch nr or BEHIND/NOBEHIND
    char_u	*rs_scan;	// current node in program
    सङ्घ
    {
	save_se_T  sesave;
	regsave_T  regsave;
    } rs_un;			// room for saving rex.input
} regitem_T;


// used for STAR, PLUS and BRACE_SIMPLE matching
प्रकारनिर्देश संरचना regstar_S
{
    पूर्णाङ्क		nextb;		// next byte
    पूर्णाङ्क		nextb_ic;	// next byte reverse case
    long	गणनम्;
    long	minval;
    long	maxval;
} regstar_T;

// used to store input position when a BACK was encountered, so that we now if
// we made any progress since the last time.
प्रकारनिर्देश संरचना backpos_S
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
स्थिर garray_T	regstack = {0, 0, 0, 0, रिक्त};
स्थिर garray_T	backpos = {0, 0, 0, 0, रिक्त};

स्थिर regsave_T behind_pos;

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
#define OP(p)		((पूर्णाङ्क)*(p))
#define NEXT(p)		(((*((p) + 1) & 0377) << 8) + (*((p) + 2) & 0377))
#define OPERAND(p)	((p) + 3)
// Obtain an operand that was stored as four bytes, MSB first.
#define OPERAND_MIN(p)	(((long)(p)[3] << 24) + ((long)(p)[4] << 16) \
			+ ((long)(p)[5] << 8) + (long)(p)[6])
// Obtain a second operand stored as four bytes.
#define OPERAND_MAX(p)	OPERAND_MIN((p) + 4)
// Obtain a second single-byte operand stored after a four bytes operand.
#define OPERAND_CMP(p)	(p)[7]

स्थिर char_u *reg(पूर्णाङ्क paren, पूर्णाङ्क *flagp);

#ifdef BT_REGEXP_DUMP
स्थिर शून्य	regdump(char_u *, bt_regprog_T *);
#endif

स्थिर पूर्णाङ्क	re_अङ्कः_cmp(long_u val, char_u *scan);

#ifdef DEBUG
स्थिर char_u	*regprop(char_u *);

स्थिर पूर्णाङ्क	regnarrate = 0;
#endif


/*
 * Setup to parse the regexp.  Used once to get the length and once to do it.
 */
    स्थिर शून्य
regcomp_start(
    char_u	*व्यञ्जकम्,
    पूर्णाङ्क		re_flags)	    // see vim_regcomp()
{
    initchr(व्यञ्जकम्);
    जइ (re_flags & RE_MAGIC)
	reg_magic = MAGIC_ON;
    अण्णहा
	reg_magic = MAGIC_OFF;
    reg_string = (re_flags & RE_STRING);
    reg_strict = (re_flags & RE_STRICT);
    get_cpo_flags();

    अङ्कः_complex_braces = 0;
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
#जइ defined(FEAT_SYN_HL)
    had_eol = FALSE;
#endif
}

/*
 * Return TRUE if MULTIBYTECODE should be used instead of EXACTLY for
 * character "c".
 */
    स्थिर पूर्णाङ्क
use_multibytecode(पूर्णाङ्क c)
{
    प्रत्यागम has_mbyte && (*mb_char2len)(c) > 1
		     && (re_multi_type(peekchr()) != NOT_MULTI
			     || (enc_utf8 && utf_iscomposing(c)));
}

/*
 * Emit (if appropriate) a byte of code
 */
    स्थिर शून्य
regc(पूर्णाङ्क b)
{
    जइ (regcode == JUST_CALC_SIZE)
	regsize++;
    अण्णहा
	*regcode++ = b;
}

/*
 * Emit (if appropriate) a multi-byte character of code
 */
    स्थिर शून्य
regmbc(पूर्णाङ्क c)
{
    जइ (!has_mbyte && c > 0xff)
	प्रत्यागम;
    जइ (regcode == JUST_CALC_SIZE)
	regsize += (*mb_char2len)(c);
    अण्णहा
	regcode += (*mb_char2bytes)(c, regcode);
}


/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * NOTE: When changing this function, also change nfa_emit_equi_class()
 */
    स्थिर शून्य
reg_equi_class(पूर्णाङ्क c)
{
    जइ (enc_utf8 || STRCMP(p_enc, "latin1") == 0
					 || STRCMP(p_enc, "iso-8859-15") == 0)
    {
	विकल्प (c)
	{
	    // Do not use '\300' style, it results in a negative number.
	    पक्ष 'A': पक्ष 0xc0: पक्ष 0xc1: पक्ष 0xc2: पक्ष 0xc3: पक्ष 0xc4:
	    पक्ष 0xc5: पक्ष 0x100: पक्ष 0x102: पक्ष 0x104: पक्ष 0x1cd:
	    पक्ष 0x1de: पक्ष 0x1e0: पक्ष 0x1fa: पक्ष 0x202: पक्ष 0x226:
	    पक्ष 0x23a: पक्ष 0x1e00: पक्ष 0x1ea0: पक्ष 0x1ea2: पक्ष 0x1ea4:
	    पक्ष 0x1ea6: पक्ष 0x1ea8: पक्ष 0x1eaa: पक्ष 0x1eac: पक्ष 0x1eae:
	    पक्ष 0x1eb0: पक्ष 0x1eb2: पक्ष 0x1eb4: पक्ष 0x1eb6:
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
		      प्रत्यागम;
	    पक्ष 'B': पक्ष 0x181: पक्ष 0x243: पक्ष 0x1e02:
	    पक्ष 0x1e04: पक्ष 0x1e06:
		      regmbc('B');
		      regmbc(0x181); regmbc(0x243); regmbc(0x1e02);
		      regmbc(0x1e04); regmbc(0x1e06);
		      प्रत्यागम;
	    पक्ष 'C': पक्ष 0xc7:
	    पक्ष 0x106: पक्ष 0x108: पक्ष 0x10a: पक्ष 0x10c: पक्ष 0x187:
	    पक्ष 0x23b: पक्ष 0x1e08: पक्ष 0xa792:
		      regmbc('C'); regmbc(0xc7);
		      regmbc(0x106); regmbc(0x108); regmbc(0x10a);
		      regmbc(0x10c); regmbc(0x187); regmbc(0x23b);
		      regmbc(0x1e08); regmbc(0xa792);
		      प्रत्यागम;
	    पक्ष 'D': पक्ष 0x10e: पक्ष 0x110: पक्ष 0x18a:
	    पक्ष 0x1e0a: पक्ष 0x1e0c: पक्ष 0x1e0e: पक्ष 0x1e10:
	    पक्ष 0x1e12:
		      regmbc('D'); regmbc(0x10e); regmbc(0x110);
		      regmbc(0x18a); regmbc(0x1e0a); regmbc(0x1e0c);
		      regmbc(0x1e0e); regmbc(0x1e10); regmbc(0x1e12);
		      प्रत्यागम;
	    पक्ष 'E': पक्ष 0xc8: पक्ष 0xc9: पक्ष 0xca: पक्ष 0xcb:
	    पक्ष 0x112: पक्ष 0x114: पक्ष 0x116: पक्ष 0x118: पक्ष 0x11a:
	    पक्ष 0x204: पक्ष 0x206: पक्ष 0x228: पक्ष 0x246: पक्ष 0x1e14:
	    पक्ष 0x1e16: पक्ष 0x1e18: पक्ष 0x1e1a: पक्ष 0x1e1c:
	    पक्ष 0x1eb8: पक्ष 0x1eba: पक्ष 0x1ebc: पक्ष 0x1ebe:
	    पक्ष 0x1ec0: पक्ष 0x1ec2: पक्ष 0x1ec4: पक्ष 0x1ec6:
		      regmbc('E'); regmbc(0xc8); regmbc(0xc9);
		      regmbc(0xca); regmbc(0xcb); regmbc(0x112);
		      regmbc(0x114); regmbc(0x116); regmbc(0x118);
		      regmbc(0x11a); regmbc(0x204); regmbc(0x206);
		      regmbc(0x228); regmbc(0x246); regmbc(0x1e14);
		      regmbc(0x1e16); regmbc(0x1e18); regmbc(0x1e1a);
		      regmbc(0x1e1c); regmbc(0x1eb8); regmbc(0x1eba);
		      regmbc(0x1ebc); regmbc(0x1ebe); regmbc(0x1ec0);
		      regmbc(0x1ec2); regmbc(0x1ec4); regmbc(0x1ec6);
		      प्रत्यागम;
	    पक्ष 'F': पक्ष 0x191: पक्ष 0x1e1e: पक्ष 0xa798:
		      regmbc('F'); regmbc(0x191); regmbc(0x1e1e);
		      regmbc(0xa798);
		      प्रत्यागम;
	    पक्ष 'G': पक्ष 0x11c: पक्ष 0x11e: पक्ष 0x120:
	    पक्ष 0x122: पक्ष 0x193: पक्ष 0x1e4: पक्ष 0x1e6:
	    पक्ष 0x1f4: पक्ष 0x1e20: पक्ष 0xa7a0:
		      regmbc('G'); regmbc(0x11c); regmbc(0x11e);
		      regmbc(0x120); regmbc(0x122); regmbc(0x193);
		      regmbc(0x1e4); regmbc(0x1e6); regmbc(0x1f4);
		      regmbc(0x1e20); regmbc(0xa7a0);
		      प्रत्यागम;
	    पक्ष 'H': पक्ष 0x124: पक्ष 0x126: पक्ष 0x21e:
	    पक्ष 0x1e22: पक्ष 0x1e24: पक्ष 0x1e26:
	    पक्ष 0x1e28: पक्ष 0x1e2a: पक्ष 0x2c67:
		      regmbc('H'); regmbc(0x124); regmbc(0x126);
		      regmbc(0x21e); regmbc(0x1e22); regmbc(0x1e24);
		      regmbc(0x1e26); regmbc(0x1e28); regmbc(0x1e2a);
		      regmbc(0x2c67);
		      प्रत्यागम;
	    पक्ष 'I': पक्ष 0xcc: पक्ष 0xcd: पक्ष 0xce: पक्ष 0xcf:
	    पक्ष 0x128: पक्ष 0x12a: पक्ष 0x12c: पक्ष 0x12e:
	    पक्ष 0x130: पक्ष 0x197: पक्ष 0x1cf: पक्ष 0x208:
	    पक्ष 0x20a: पक्ष 0x1e2c: पक्ष 0x1e2e: पक्ष 0x1ec8:
	    पक्ष 0x1eca:
		      regmbc('I'); regmbc(0xcc); regmbc(0xcd);
		      regmbc(0xce); regmbc(0xcf); regmbc(0x128);
		      regmbc(0x12a); regmbc(0x12c); regmbc(0x12e);
		      regmbc(0x130); regmbc(0x197); regmbc(0x1cf);
		      regmbc(0x208); regmbc(0x20a); regmbc(0x1e2c);
		      regmbc(0x1e2e); regmbc(0x1ec8); regmbc(0x1eca);
		      प्रत्यागम;
	    पक्ष 'J': पक्ष 0x134: पक्ष 0x248:
		      regmbc('J'); regmbc(0x134); regmbc(0x248);
		      प्रत्यागम;
	    पक्ष 'K': पक्ष 0x136: पक्ष 0x198: पक्ष 0x1e8: पक्ष 0x1e30:
	    पक्ष 0x1e32: पक्ष 0x1e34: पक्ष 0x2c69: पक्ष 0xa740:
		      regmbc('K'); regmbc(0x136); regmbc(0x198);
		      regmbc(0x1e8); regmbc(0x1e30); regmbc(0x1e32);
		      regmbc(0x1e34); regmbc(0x2c69); regmbc(0xa740);
		      प्रत्यागम;
	    पक्ष 'L': पक्ष 0x139: पक्ष 0x13b: पक्ष 0x13d: पक्ष 0x13f:
	    पक्ष 0x141: पक्ष 0x23d: पक्ष 0x1e36: पक्ष 0x1e38:
	    पक्ष 0x1e3a: पक्ष 0x1e3c: पक्ष 0x2c60:
		      regmbc('L'); regmbc(0x139); regmbc(0x13b);
		      regmbc(0x13d); regmbc(0x13f); regmbc(0x141);
		      regmbc(0x23d); regmbc(0x1e36); regmbc(0x1e38);
		      regmbc(0x1e3a); regmbc(0x1e3c); regmbc(0x2c60);
		      प्रत्यागम;
	    पक्ष 'M': पक्ष 0x1e3e: पक्ष 0x1e40: पक्ष 0x1e42:
		      regmbc('M'); regmbc(0x1e3e); regmbc(0x1e40);
		      regmbc(0x1e42);
		      प्रत्यागम;
	    पक्ष 'N': पक्ष 0xd1:
	    पक्ष 0x143: पक्ष 0x145: पक्ष 0x147: पक्ष 0x1f8:
	    पक्ष 0x1e44: पक्ष 0x1e46: पक्ष 0x1e48: पक्ष 0x1e4a:
	    पक्ष 0xa7a4:
		      regmbc('N'); regmbc(0xd1);
		      regmbc(0x143); regmbc(0x145); regmbc(0x147);
		      regmbc(0x1f8); regmbc(0x1e44); regmbc(0x1e46);
		      regmbc(0x1e48); regmbc(0x1e4a); regmbc(0xa7a4);
		      प्रत्यागम;
	    पक्ष 'O': पक्ष 0xd2: पक्ष 0xd3: पक्ष 0xd4: पक्ष 0xd5: पक्ष 0xd6:
	    पक्ष 0xd8: पक्ष 0x14c: पक्ष 0x14e: पक्ष 0x150: पक्ष 0x19f:
	    पक्ष 0x1a0: पक्ष 0x1d1: पक्ष 0x1ea: पक्ष 0x1ec: पक्ष 0x1fe:
	    पक्ष 0x20c: पक्ष 0x20e: पक्ष 0x22a: पक्ष 0x22c: पक्ष 0x22e:
	    पक्ष 0x230: पक्ष 0x1e4c: पक्ष 0x1e4e: पक्ष 0x1e50: पक्ष 0x1e52:
	    पक्ष 0x1ecc: पक्ष 0x1ece: पक्ष 0x1ed0: पक्ष 0x1ed2: पक्ष 0x1ed4:
	    पक्ष 0x1ed6: पक्ष 0x1ed8: पक्ष 0x1eda: पक्ष 0x1edc: पक्ष 0x1ede:
	    पक्ष 0x1ee0: पक्ष 0x1ee2:
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
		      प्रत्यागम;
	    पक्ष 'P': पक्ष 0x1a4: पक्ष 0x1e54: पक्ष 0x1e56: पक्ष 0x2c63:
		      regmbc('P'); regmbc(0x1a4); regmbc(0x1e54);
		      regmbc(0x1e56); regmbc(0x2c63);
		      प्रत्यागम;
	    पक्ष 'Q': पक्ष 0x24a:
		      regmbc('Q'); regmbc(0x24a);
		      प्रत्यागम;
	    पक्ष 'R': पक्ष 0x154: पक्ष 0x156: पक्ष 0x158: पक्ष 0x210:
	    पक्ष 0x212: पक्ष 0x24c: पक्ष 0x1e58: पक्ष 0x1e5a:
	    पक्ष 0x1e5c: पक्ष 0x1e5e: पक्ष 0x2c64: पक्ष 0xa7a6:
		      regmbc('R'); regmbc(0x154); regmbc(0x156);
		      regmbc(0x210); regmbc(0x212); regmbc(0x158);
		      regmbc(0x24c); regmbc(0x1e58); regmbc(0x1e5a);
		      regmbc(0x1e5c); regmbc(0x1e5e); regmbc(0x2c64);
		      regmbc(0xa7a6);
		      प्रत्यागम;
	    पक्ष 'S': पक्ष 0x15a: पक्ष 0x15c: पक्ष 0x15e: पक्ष 0x160:
	    पक्ष 0x218: पक्ष 0x1e60: पक्ष 0x1e62: पक्ष 0x1e64:
	    पक्ष 0x1e66: पक्ष 0x1e68: पक्ष 0x2c7e: पक्ष 0xa7a8:
		      regmbc('S'); regmbc(0x15a); regmbc(0x15c);
		      regmbc(0x15e); regmbc(0x160); regmbc(0x218);
		      regmbc(0x1e60); regmbc(0x1e62); regmbc(0x1e64);
		      regmbc(0x1e66); regmbc(0x1e68); regmbc(0x2c7e);
			  regmbc(0xa7a8);
		      प्रत्यागम;
	    पक्ष 'T': पक्ष 0x162: पक्ष 0x164: पक्ष 0x166: पक्ष 0x1ac:
	    पक्ष 0x1ae: पक्ष 0x21a: पक्ष 0x23e: पक्ष 0x1e6a: पक्ष 0x1e6c:
	    पक्ष 0x1e6e: पक्ष 0x1e70:
		      regmbc('T'); regmbc(0x162); regmbc(0x164);
		      regmbc(0x166); regmbc(0x1ac); regmbc(0x23e);
		      regmbc(0x1ae); regmbc(0x21a); regmbc(0x1e6a);
		      regmbc(0x1e6c); regmbc(0x1e6e); regmbc(0x1e70);
		      प्रत्यागम;
	    पक्ष 'U': पक्ष 0xd9: पक्ष 0xda: पक्ष 0xdb: पक्ष 0xdc:
	    पक्ष 0x168: पक्ष 0x16a: पक्ष 0x16c: पक्ष 0x16e:
	    पक्ष 0x170: पक्ष 0x172: पक्ष 0x1af: पक्ष 0x1d3:
	    पक्ष 0x1d5: पक्ष 0x1d7: पक्ष 0x1d9: पक्ष 0x1db:
	    पक्ष 0x214: पक्ष 0x216: पक्ष 0x244: पक्ष 0x1e72:
	    पक्ष 0x1e74: पक्ष 0x1e76: पक्ष 0x1e78: पक्ष 0x1e7a:
	    पक्ष 0x1ee4: पक्ष 0x1ee6: पक्ष 0x1ee8: पक्ष 0x1eea:
	    पक्ष 0x1eec: पक्ष 0x1eee: पक्ष 0x1ef0:
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
		      प्रत्यागम;
	    पक्ष 'V': पक्ष 0x1b2: पक्ष 0x1e7c: पक्ष 0x1e7e:
		      regmbc('V'); regmbc(0x1b2); regmbc(0x1e7c);
		      regmbc(0x1e7e);
		      प्रत्यागम;
	    पक्ष 'W': पक्ष 0x174: पक्ष 0x1e80: पक्ष 0x1e82:
	    पक्ष 0x1e84: पक्ष 0x1e86: पक्ष 0x1e88:
		      regmbc('W'); regmbc(0x174); regmbc(0x1e80);
		      regmbc(0x1e82); regmbc(0x1e84); regmbc(0x1e86);
		      regmbc(0x1e88);
		      प्रत्यागम;
	    पक्ष 'X': पक्ष 0x1e8a: पक्ष 0x1e8c:
		      regmbc('X'); regmbc(0x1e8a); regmbc(0x1e8c);
		      प्रत्यागम;
	    पक्ष 'Y': पक्ष 0xdd:
	    पक्ष 0x176: पक्ष 0x178: पक्ष 0x1b3: पक्ष 0x232: पक्ष 0x24e:
	    पक्ष 0x1e8e: पक्ष 0x1ef2: पक्ष 0x1ef6: पक्ष 0x1ef4: पक्ष 0x1ef8:
		      regmbc('Y'); regmbc(0xdd); regmbc(0x176);
		      regmbc(0x178); regmbc(0x1b3); regmbc(0x232);
		      regmbc(0x24e); regmbc(0x1e8e); regmbc(0x1ef2);
		      regmbc(0x1ef4); regmbc(0x1ef6); regmbc(0x1ef8);
		      प्रत्यागम;
	    पक्ष 'Z': पक्ष 0x179: पक्ष 0x17b: पक्ष 0x17d: पक्ष 0x1b5:
	    पक्ष 0x1e90: पक्ष 0x1e92: पक्ष 0x1e94: पक्ष 0x2c6b:
		      regmbc('Z'); regmbc(0x179); regmbc(0x17b);
		      regmbc(0x17d); regmbc(0x1b5); regmbc(0x1e90);
		      regmbc(0x1e92); regmbc(0x1e94); regmbc(0x2c6b);
		      प्रत्यागम;
	    पक्ष 'a': पक्ष 0xe0: पक्ष 0xe1: पक्ष 0xe2:
	    पक्ष 0xe3: पक्ष 0xe4: पक्ष 0xe5: पक्ष 0x101: पक्ष 0x103:
	    पक्ष 0x105: पक्ष 0x1ce: पक्ष 0x1df: पक्ष 0x1e1: पक्ष 0x1fb:
	    पक्ष 0x201: पक्ष 0x203: पक्ष 0x227: पक्ष 0x1d8f: पक्ष 0x1e01:
	    पक्ष 0x1e9a: पक्ष 0x1ea1: पक्ष 0x1ea3: पक्ष 0x1ea5:
	    पक्ष 0x1ea7: पक्ष 0x1ea9: पक्ष 0x1eab: पक्ष 0x1ead:
	    पक्ष 0x1eaf: पक्ष 0x1eb1: पक्ष 0x1eb3: पक्ष 0x1eb5:
	    पक्ष 0x1eb7: पक्ष 0x2c65:
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
		      प्रत्यागम;
	    पक्ष 'b': पक्ष 0x180: पक्ष 0x253: पक्ष 0x1d6c: पक्ष 0x1d80:
	    पक्ष 0x1e03: पक्ष 0x1e05: पक्ष 0x1e07:
		      regmbc('b');
		      regmbc(0x180); regmbc(0x253); regmbc(0x1d6c);
		      regmbc(0x1d80); regmbc(0x1e03); regmbc(0x1e05);
		      regmbc(0x1e07);
		      प्रत्यागम;
	    पक्ष 'c': पक्ष 0xe7:
	    पक्ष 0x107: पक्ष 0x109: पक्ष 0x10b: पक्ष 0x10d: पक्ष 0x188:
	    पक्ष 0x23c: पक्ष 0x1e09: पक्ष 0xa793: पक्ष 0xa794:
		      regmbc('c'); regmbc(0xe7); regmbc(0x107);
		      regmbc(0x109); regmbc(0x10b); regmbc(0x10d);
		      regmbc(0x188); regmbc(0x23c); regmbc(0x1e09);
		      regmbc(0xa793); regmbc(0xa794);
		      प्रत्यागम;
	    पक्ष 'd': पक्ष 0x10f: पक्ष 0x111: पक्ष 0x257: पक्ष 0x1d6d:
	    पक्ष 0x1d81: पक्ष 0x1d91: पक्ष 0x1e0b: पक्ष 0x1e0d:
	    पक्ष 0x1e0f: पक्ष 0x1e11: पक्ष 0x1e13:
		      regmbc('d'); regmbc(0x10f); regmbc(0x111);
		      regmbc(0x257); regmbc(0x1d6d); regmbc(0x1d81);
		      regmbc(0x1d91); regmbc(0x1e0b); regmbc(0x1e0d);
		      regmbc(0x1e0f); regmbc(0x1e11); regmbc(0x1e13);
		      प्रत्यागम;
	    पक्ष 'e': पक्ष 0xe8: पक्ष 0xe9: पक्ष 0xea: पक्ष 0xeb:
	    पक्ष 0x113: पक्ष 0x115: पक्ष 0x117: पक्ष 0x119:
	    पक्ष 0x11b: पक्ष 0x205: पक्ष 0x207: पक्ष 0x229:
	    पक्ष 0x247: पक्ष 0x1d92: पक्ष 0x1e15: पक्ष 0x1e17:
	    पक्ष 0x1e19: पक्ष 0x1e1b: पक्ष 0x1eb9: पक्ष 0x1ebb:
	    पक्ष 0x1e1d: पक्ष 0x1ebd: पक्ष 0x1ebf: पक्ष 0x1ec1:
	    पक्ष 0x1ec3: पक्ष 0x1ec5: पक्ष 0x1ec7:
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
		      प्रत्यागम;
	    पक्ष 'f': पक्ष 0x192: पक्ष 0x1d6e: पक्ष 0x1d82:
	    पक्ष 0x1e1f: पक्ष 0xa799:
		     regmbc('f'); regmbc(0x192); regmbc(0x1d6e);
		     regmbc(0x1d82); regmbc(0x1e1f); regmbc(0xa799);
		     प्रत्यागम;
	    पक्ष 'g': पक्ष 0x11d: पक्ष 0x11f: पक्ष 0x121: पक्ष 0x123:
	    पक्ष 0x1e5: पक्ष 0x1e7: पक्ष 0x260: पक्ष 0x1f5: पक्ष 0x1d83:
	    पक्ष 0x1e21: पक्ष 0xa7a1:
		      regmbc('g'); regmbc(0x11d); regmbc(0x11f);
		      regmbc(0x121); regmbc(0x123); regmbc(0x1e5);
		      regmbc(0x1e7); regmbc(0x1f5); regmbc(0x260);
		      regmbc(0x1d83); regmbc(0x1e21); regmbc(0xa7a1);
		      प्रत्यागम;
	    पक्ष 'h': पक्ष 0x125: पक्ष 0x127: पक्ष 0x21f: पक्ष 0x1e23:
	    पक्ष 0x1e25: पक्ष 0x1e27: पक्ष 0x1e29: पक्ष 0x1e2b:
	    पक्ष 0x1e96: पक्ष 0x2c68: पक्ष 0xa795:
		      regmbc('h'); regmbc(0x125); regmbc(0x127);
		      regmbc(0x21f); regmbc(0x1e23); regmbc(0x1e25);
		      regmbc(0x1e27); regmbc(0x1e29); regmbc(0x1e2b);
		      regmbc(0x1e96); regmbc(0x2c68); regmbc(0xa795);
		      प्रत्यागम;
	    पक्ष 'i': पक्ष 0xec: पक्ष 0xed: पक्ष 0xee: पक्ष 0xef:
	    पक्ष 0x129: पक्ष 0x12b: पक्ष 0x12d: पक्ष 0x12f:
	    पक्ष 0x1d0: पक्ष 0x209: पक्ष 0x20b: पक्ष 0x268:
	    पक्ष 0x1d96: पक्ष 0x1e2d: पक्ष 0x1e2f: पक्ष 0x1ec9:
	    पक्ष 0x1ecb:
		      regmbc('i'); regmbc(0xec); regmbc(0xed);
		      regmbc(0xee); regmbc(0xef); regmbc(0x129);
		      regmbc(0x12b); regmbc(0x12d); regmbc(0x12f);
		      regmbc(0x1d0); regmbc(0x209); regmbc(0x20b);
		      regmbc(0x268); regmbc(0x1d96); regmbc(0x1e2d);
		      regmbc(0x1e2f); regmbc(0x1ec9); regmbc(0x1ecb);
		      प्रत्यागम;
	    पक्ष 'j': पक्ष 0x135: पक्ष 0x1f0: पक्ष 0x249:
		      regmbc('j'); regmbc(0x135); regmbc(0x1f0);
		      regmbc(0x249);
		      प्रत्यागम;
	    पक्ष 'k': पक्ष 0x137: पक्ष 0x199: पक्ष 0x1e9:
	    पक्ष 0x1d84: पक्ष 0x1e31: पक्ष 0x1e33: पक्ष 0x1e35:
	    पक्ष 0x2c6a: पक्ष 0xa741:
		      regmbc('k'); regmbc(0x137); regmbc(0x199);
		      regmbc(0x1e9); regmbc(0x1d84); regmbc(0x1e31);
		      regmbc(0x1e33); regmbc(0x1e35); regmbc(0x2c6a);
		      regmbc(0xa741);
		      प्रत्यागम;
	    पक्ष 'l': पक्ष 0x13a: पक्ष 0x13c: पक्ष 0x13e:
	    पक्ष 0x140: पक्ष 0x142: पक्ष 0x19a: पक्ष 0x1e37:
	    पक्ष 0x1e39: पक्ष 0x1e3b: पक्ष 0x1e3d: पक्ष 0x2c61:
		      regmbc('l'); regmbc(0x13a); regmbc(0x13c);
		      regmbc(0x13e); regmbc(0x140); regmbc(0x142);
		      regmbc(0x19a); regmbc(0x1e37); regmbc(0x1e39);
		      regmbc(0x1e3b); regmbc(0x1e3d); regmbc(0x2c61);
		      प्रत्यागम;
	    पक्ष 'm': पक्ष 0x1d6f: पक्ष 0x1e3f: पक्ष 0x1e41: पक्ष 0x1e43:
		      regmbc('m'); regmbc(0x1d6f); regmbc(0x1e3f);
		      regmbc(0x1e41); regmbc(0x1e43);
		      प्रत्यागम;
	    पक्ष 'n': पक्ष 0xf1: पक्ष 0x144: पक्ष 0x146: पक्ष 0x148:
	    पक्ष 0x149: पक्ष 0x1f9: पक्ष 0x1d70: पक्ष 0x1d87:
	    पक्ष 0x1e45: पक्ष 0x1e47: पक्ष 0x1e49: पक्ष 0x1e4b:
	    पक्ष 0xa7a5:
		      regmbc('n'); regmbc(0xf1); regmbc(0x144);
		      regmbc(0x146); regmbc(0x148); regmbc(0x149);
		      regmbc(0x1f9); regmbc(0x1d70); regmbc(0x1d87);
		      regmbc(0x1e45); regmbc(0x1e47); regmbc(0x1e49);
		      regmbc(0x1e4b); regmbc(0xa7a5);
		      प्रत्यागम;
	    पक्ष 'o': पक्ष 0xf2: पक्ष 0xf3: पक्ष 0xf4: पक्ष 0xf5:
	    पक्ष 0xf6: पक्ष 0xf8: पक्ष 0x14d: पक्ष 0x14f: पक्ष 0x151:
	    पक्ष 0x1a1: पक्ष 0x1d2: पक्ष 0x1eb: पक्ष 0x1ed: पक्ष 0x1ff:
	    पक्ष 0x20d: पक्ष 0x20f: पक्ष 0x22b: पक्ष 0x22d: पक्ष 0x22f:
	    पक्ष 0x231: पक्ष 0x275: पक्ष 0x1e4d: पक्ष 0x1e4f:
	    पक्ष 0x1e51: पक्ष 0x1e53: पक्ष 0x1ecd: पक्ष 0x1ecf:
	    पक्ष 0x1ed1: पक्ष 0x1ed3: पक्ष 0x1ed5: पक्ष 0x1ed7:
	    पक्ष 0x1ed9: पक्ष 0x1edb: पक्ष 0x1edd: पक्ष 0x1edf:
	    पक्ष 0x1ee1: पक्ष 0x1ee3:
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
		      प्रत्यागम;
	    पक्ष 'p': पक्ष 0x1a5: पक्ष 0x1d71: पक्ष 0x1d88: पक्ष 0x1d7d:
	    पक्ष 0x1e55: पक्ष 0x1e57:
		      regmbc('p'); regmbc(0x1a5); regmbc(0x1d71);
		      regmbc(0x1d7d); regmbc(0x1d88); regmbc(0x1e55);
		      regmbc(0x1e57);
		      प्रत्यागम;
	    पक्ष 'q': पक्ष 0x24b: पक्ष 0x2a0:
		      regmbc('q'); regmbc(0x24b); regmbc(0x2a0);
		      प्रत्यागम;
	    पक्ष 'r': पक्ष 0x155: पक्ष 0x157: पक्ष 0x159: पक्ष 0x211:
	    पक्ष 0x213: पक्ष 0x24d: पक्ष 0x27d: पक्ष 0x1d72: पक्ष 0x1d73:
	    पक्ष 0x1d89: पक्ष 0x1e59: पक्ष 0x1e5b: पक्ष 0x1e5d: पक्ष 0x1e5f:
	    पक्ष 0xa7a7:
		      regmbc('r'); regmbc(0x155); regmbc(0x157);
		      regmbc(0x159); regmbc(0x211); regmbc(0x213);
		      regmbc(0x24d); regmbc(0x1d72); regmbc(0x1d73);
		      regmbc(0x1d89); regmbc(0x1e59); regmbc(0x27d);
		      regmbc(0x1e5b); regmbc(0x1e5d); regmbc(0x1e5f);
		      regmbc(0xa7a7);
		      प्रत्यागम;
	    पक्ष 's': पक्ष 0x15b: पक्ष 0x15d: पक्ष 0x15f: पक्ष 0x161:
	    पक्ष 0x1e61: पक्ष 0x219: पक्ष 0x23f: पक्ष 0x1d74: पक्ष 0x1d8a:
	    पक्ष 0x1e63: पक्ष 0x1e65: पक्ष 0x1e67: पक्ष 0x1e69: पक्ष 0xa7a9:
		      regmbc('s'); regmbc(0x15b); regmbc(0x15d);
		      regmbc(0x15f); regmbc(0x161); regmbc(0x23f);
		      regmbc(0x219); regmbc(0x1d74); regmbc(0x1d8a);
		      regmbc(0x1e61); regmbc(0x1e63); regmbc(0x1e65);
		      regmbc(0x1e67); regmbc(0x1e69); regmbc(0xa7a9);
		      प्रत्यागम;
	    पक्ष 't': पक्ष 0x163: पक्ष 0x165: पक्ष 0x167: पक्ष 0x1ab:
	    पक्ष 0x1ad: पक्ष 0x21b: पक्ष 0x288: पक्ष 0x1d75: पक्ष 0x1e6b:
	    पक्ष 0x1e6d: पक्ष 0x1e6f: पक्ष 0x1e71: पक्ष 0x1e97: पक्ष 0x2c66:
		      regmbc('t'); regmbc(0x163); regmbc(0x165);
		      regmbc(0x167); regmbc(0x1ab); regmbc(0x21b);
		      regmbc(0x1ad); regmbc(0x288); regmbc(0x1d75);
		      regmbc(0x1e6b); regmbc(0x1e6d); regmbc(0x1e6f);
		      regmbc(0x1e71); regmbc(0x1e97); regmbc(0x2c66);
		      प्रत्यागम;
	    पक्ष 'u': पक्ष 0xf9: पक्ष 0xfa: पक्ष 0xfb: पक्ष 0xfc:
	    पक्ष 0x169: पक्ष 0x16b: पक्ष 0x16d: पक्ष 0x16f:
	    पक्ष 0x171: पक्ष 0x173: पक्ष 0x1b0: पक्ष 0x1d4:
	    पक्ष 0x1d6: पक्ष 0x1d8: पक्ष 0x1da: पक्ष 0x1dc:
	    पक्ष 0x215: पक्ष 0x217: पक्ष 0x289: पक्ष 0x1e73:
	    पक्ष 0x1d7e: पक्ष 0x1d99: पक्ष 0x1e75: पक्ष 0x1e77:
	    पक्ष 0x1e79: पक्ष 0x1e7b: पक्ष 0x1ee5: पक्ष 0x1ee7:
	    पक्ष 0x1ee9: पक्ष 0x1eeb: पक्ष 0x1eed: पक्ष 0x1eef:
	    पक्ष 0x1ef1:
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
		      प्रत्यागम;
	    पक्ष 'v': पक्ष 0x28b: पक्ष 0x1d8c: पक्ष 0x1e7d: पक्ष 0x1e7f:
		      regmbc('v'); regmbc(0x28b); regmbc(0x1d8c);
		      regmbc(0x1e7d); regmbc(0x1e7f);
		      प्रत्यागम;
	    पक्ष 'w': पक्ष 0x175: पक्ष 0x1e81: पक्ष 0x1e83:
	    पक्ष 0x1e85: पक्ष 0x1e87: पक्ष 0x1e89: पक्ष 0x1e98:
		      regmbc('w'); regmbc(0x175); regmbc(0x1e81);
		      regmbc(0x1e83); regmbc(0x1e85); regmbc(0x1e87);
		      regmbc(0x1e89); regmbc(0x1e98);
		      प्रत्यागम;
	    पक्ष 'x': पक्ष 0x1e8b: पक्ष 0x1e8d:
		      regmbc('x'); regmbc(0x1e8b); regmbc(0x1e8d);
		      प्रत्यागम;
	    पक्ष 'y': पक्ष 0xfd: पक्ष 0xff: पक्ष 0x177: पक्ष 0x1b4:
	    पक्ष 0x233: पक्ष 0x24f: पक्ष 0x1e8f: पक्ष 0x1e99: पक्ष 0x1ef3:
	    पक्ष 0x1ef5: पक्ष 0x1ef7: पक्ष 0x1ef9:
		      regmbc('y'); regmbc(0xfd); regmbc(0xff);
		      regmbc(0x177); regmbc(0x1b4); regmbc(0x233);
		      regmbc(0x24f); regmbc(0x1e8f); regmbc(0x1e99);
		      regmbc(0x1ef3); regmbc(0x1ef5); regmbc(0x1ef7);
		      regmbc(0x1ef9);
		      प्रत्यागम;
	    पक्ष 'z': पक्ष 0x17a: पक्ष 0x17c: पक्ष 0x17e: पक्ष 0x1b6:
	    पक्ष 0x1d76: पक्ष 0x1d8e: पक्ष 0x1e91: पक्ष 0x1e93:
	    पक्ष 0x1e95: पक्ष 0x2c6c:
		      regmbc('z'); regmbc(0x17a); regmbc(0x17c);
		      regmbc(0x17e); regmbc(0x1b6); regmbc(0x1d76);
		      regmbc(0x1d8e); regmbc(0x1e91); regmbc(0x1e93);
		      regmbc(0x1e95); regmbc(0x2c6c);
		      प्रत्यागम;
	}
    }
    regmbc(c);
}

/*
 * Emit a node.
 * Return pointer to generated code.
 */
    स्थिर char_u *
regnode(पूर्णाङ्क op)
{
    char_u  *ret;

    ret = regcode;
    जइ (ret == JUST_CALC_SIZE)
	regsize += 3;
    अण्णहा
    {
	*regcode++ = op;
	*regcode++ = NUL;		// Null "next" pointer.
	*regcode++ = NUL;
    }
    प्रत्यागम ret;
}

/*
 * Write a long as four bytes at "p" and return pointer to the next char.
 */
    स्थिर char_u *
re_put_long(char_u *p, long_u val)
{
    *p++ = (char_u) ((val >> 24) & 0377);
    *p++ = (char_u) ((val >> 16) & 0377);
    *p++ = (char_u) ((val >> 8) & 0377);
    *p++ = (char_u) (val & 0377);
    प्रत्यागम p;
}

/*
 * regnext - dig the "next" pointer out of a node
 * Returns NULL when calculating size, when there is no next item and when
 * there is an error.
 */
    स्थिर char_u *
regnext(char_u *p)
{
    पूर्णाङ्क	    offset;

    जइ (p == JUST_CALC_SIZE || reg_toolong)
	प्रत्यागम रिक्त;

    offset = NEXT(p);
    जइ (offset == 0)
	प्रत्यागम रिक्त;

    जइ (OP(p) == BACK)
	प्रत्यागम p - offset;
    अण्णहा
	प्रत्यागम p + offset;
}

/*
 * Set the next-pointer at the end of a node chain.
 */
    स्थिर शून्य
regtail(char_u *p, char_u *val)
{
    char_u	*scan;
    char_u	*temp;
    पूर्णाङ्क		offset;

    जइ (p == JUST_CALC_SIZE)
	प्रत्यागम;

    // Find last node.
    scan = p;
    प्रति (;;)
    {
	temp = regnext(scan);
	जइ (temp == रिक्त)
	    भङ्ग;
	scan = temp;
    }

    जइ (OP(scan) == BACK)
	offset = (पूर्णाङ्क)(scan - val);
    अण्णहा
	offset = (पूर्णाङ्क)(val - scan);
    // When the offset uses more than 16 bits it can no longer fit in the two
    // bytes available.  Use a global flag to avoid having to check return
    // values in too many places.
    जइ (offset > 0xffff)
	reg_toolong = TRUE;
    अण्णहा
    {
	*(scan + 1) = (char_u) (((unsigned)offset >> 8) & 0377);
	*(scan + 2) = (char_u) (offset & 0377);
    }
}

/*
 * Like regtail, on item after a BRANCH; nop if none.
 */
    स्थिर शून्य
regoptail(char_u *p, char_u *val)
{
    // When op is neither BRANCH nor BRACE_COMPLEX0-9, it is "operandless"
    जइ (p == रिक्त || p == JUST_CALC_SIZE
	    || (OP(p) != BRANCH
		&& (OP(p) < BRACE_COMPLEX || OP(p) > BRACE_COMPLEX + 9)))
	प्रत्यागम;
    regtail(OPERAND(p), val);
}

/*
 * Insert an operator in front of already-emitted operand
 *
 * Means relocating the operand.
 */
    स्थिर शून्य
reginsert(पूर्णाङ्क op, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    जइ (regcode == JUST_CALC_SIZE)
    {
	regsize += 3;
	प्रत्यागम;
    }
    src = regcode;
    regcode += 3;
    dst = regcode;
    जाव (src > opnd)
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
    स्थिर शून्य
reginsert_nr(पूर्णाङ्क op, long val, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    जइ (regcode == JUST_CALC_SIZE)
    {
	regsize += 7;
	प्रत्यागम;
    }
    src = regcode;
    regcode += 7;
    dst = regcode;
    जाव (src > opnd)
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
    स्थिर शून्य
reginsert_limits(
    पूर्णाङ्क		op,
    long	minval,
    long	maxval,
    char_u	*opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    जइ (regcode == JUST_CALC_SIZE)
    {
	regsize += 11;
	प्रत्यागम;
    }
    src = regcode;
    regcode += 11;
    dst = regcode;
    जाव (src > opnd)
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
    स्थिर पूर्णाङ्क
seen_endbrace(पूर्णाङ्क refnum)
{
    जइ (!had_endbrace[refnum])
    {
	char_u *p;

	// Trick: check if "@<=" or "@<!" follows, in which case
	// the \1 can appear before the referenced match.
	प्रति (p = regparse; *p != NUL; ++p)
	    जइ (p[0] == '@' && p[1] == '<' && (p[2] == '!' || p[2] == '='))
		भङ्ग;
	जइ (*p == NUL)
	{
	    emsg(_(e_illegal_back_reference));
	    rc_did_emsg = TRUE;
	    प्रत्यागम FALSE;
	}
    }
    प्रत्यागम TRUE;
}

/*
 * Parse the lowest level.
 *
 * Optimization:  gobbles an entire sequence of ordinary characters so that
 * it can turn them into a single node, which is smaller to store and
 * faster to run.  Don't do this when one_exactly is set.
 */
    स्थिर char_u *
regatom(पूर्णाङ्क *flagp)
{
    char_u	    *ret;
    पूर्णाङ्क		    flags;
    पूर्णाङ्क		    c;
    char_u	    *p;
    पूर्णाङ्क		    extra = 0;
    पूर्णाङ्क		    save_prev_at_start = prev_at_start;

    *flagp = WORST;		// Tentatively.

    c = getchr();
    विकल्प (c)
    {
      पक्ष Magic('^'):
	ret = regnode(BOL);
	भङ्ग;

      पक्ष Magic('$'):
	ret = regnode(EOL);
#जइ defined(FEAT_SYN_HL)
	had_eol = TRUE;
#endif
	भङ्ग;

      पक्ष Magic('<'):
	ret = regnode(BOW);
	भङ्ग;

      पक्ष Magic('>'):
	ret = regnode(EOW);
	भङ्ग;

      पक्ष Magic('_'):
	c = no_Magic(getchr());
	जइ (c == '^')		// "\_^" is start-of-line
	{
	    ret = regnode(BOL);
	    भङ्ग;
	}
	जइ (c == '$')		// "\_$" is end-of-line
	{
	    ret = regnode(EOL);
#जइ defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#endif
	    भङ्ग;
	}

	extra = ADD_NL;
	*flagp |= HASNL;

	// "\_[" is character range plus newline
	जइ (c == '[')
	    गच्छ collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	// Character classes.
      पक्ष Magic('.'):
      पक्ष Magic('i'):
      पक्ष Magic('I'):
      पक्ष Magic('k'):
      पक्ष Magic('K'):
      पक्ष Magic('f'):
      पक्ष Magic('F'):
      पक्ष Magic('p'):
      पक्ष Magic('P'):
      पक्ष Magic('s'):
      पक्ष Magic('S'):
      पक्ष Magic('d'):
      पक्ष Magic('D'):
      पक्ष Magic('x'):
      पक्ष Magic('X'):
      पक्ष Magic('o'):
      पक्ष Magic('O'):
      पक्ष Magic('w'):
      पक्ष Magic('W'):
      पक्ष Magic('h'):
      पक्ष Magic('H'):
      पक्ष Magic('a'):
      पक्ष Magic('A'):
      पक्ष Magic('l'):
      पक्ष Magic('L'):
      पक्ष Magic('u'):
      पक्ष Magic('U'):
	p = vim_strchr(classchars, no_Magic(c));
	जइ (p == रिक्त)
	    EMSG_RET_NULL(_(e_invalid_use_of_underscore));

	// When '.' is followed by a composing char ignore the dot, so that
	// the composing char is matched here.
	जइ (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	{
	    c = getchr();
	    गच्छ do_multibyte;
	}
	ret = regnode(classcodes[p - classchars] + extra);
	*flagp |= HASWIDTH | SIMPLE;
	भङ्ग;

      पक्ष Magic('n'):
	जइ (reg_string)
	{
	    // In a string "\n" matches a newline character.
	    ret = regnode(EXACTLY);
	    regc(NL);
	    regc(NUL);
	    *flagp |= HASWIDTH | SIMPLE;
	}
	अण्णहा
	{
	    // In buffer text "\n" matches the end of a line.
	    ret = regnode(NEWL);
	    *flagp |= HASWIDTH | HASNL;
	}
	भङ्ग;

      पक्ष Magic('('):
	जइ (one_exactly)
	    EMSG_ONE_RET_NULL;
	ret = reg(REG_PAREN, &flags);
	जइ (ret == रिक्त)
	    प्रत्यागम रिक्त;
	*flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
	भङ्ग;

      पक्ष NUL:
      पक्ष Magic('|'):
      पक्ष Magic('&'):
      पक्ष Magic(')'):
	जइ (one_exactly)
	    EMSG_ONE_RET_NULL;
	// Supposed to be caught earlier.
	IEMSG_RET_NULL(e_internal_error_in_regexp);
	// NOTREACHED

      पक्ष Magic('='):
      पक्ष Magic('?'):
      पक्ष Magic('+'):
      पक्ष Magic('@'):
      पक्ष Magic('{'):
      पक्ष Magic('*'):
	c = no_Magic(c);
	EMSG3_RET_NULL(_(e_str_chr_follows_nothing),
		(c == '*' ? reg_magic >= MAGIC_ON : reg_magic == MAGIC_ALL), c);
	// NOTREACHED

      पक्ष Magic('~'):		// previous substitute pattern
	    जइ (reg_prev_sub != रिक्त)
	    {
		char_u	    *lp;

		ret = regnode(EXACTLY);
		lp = reg_prev_sub;
		जाव (*lp != NUL)
		    regc(*lp++);
		regc(NUL);
		जइ (*reg_prev_sub != NUL)
		{
		    *flagp |= HASWIDTH;
		    जइ ((lp - reg_prev_sub) == 1)
			*flagp |= SIMPLE;
		}
	    }
	    अण्णहा
		EMSG_RET_NULL(_(e_no_previous_substitute_regular_expression));
	    भङ्ग;

      पक्ष Magic('1'):
      पक्ष Magic('2'):
      पक्ष Magic('3'):
      पक्ष Magic('4'):
      पक्ष Magic('5'):
      पक्ष Magic('6'):
      पक्ष Magic('7'):
      पक्ष Magic('8'):
      पक्ष Magic('9'):
	    {
		पूर्णाङ्क		    refnum;

		refnum = c - Magic('0');
		जइ (!seen_endbrace(refnum))
		    प्रत्यागम रिक्त;
		ret = regnode(BACKREF + refnum);
	    }
	    भङ्ग;

      पक्ष Magic('z'):
	{
	    c = no_Magic(getchr());
	    विकल्प (c)
	    {
#ifdef FEAT_SYN_HL
		पक्ष '(': जइ ((reg_do_extmatch & REX_SET) == 0)
			      EMSG_RET_NULL(_(e_z_not_allowed_here));
			  जइ (one_exactly)
			      EMSG_ONE_RET_NULL;
			  ret = reg(REG_ZPAREN, &flags);
			  जइ (ret == रिक्त)
			      प्रत्यागम रिक्त;
			  *flagp |= flags & (HASWIDTH|SPSTART|HASNL|HASLOOKBH);
			  re_has_z = REX_SET;
			  भङ्ग;

		पक्ष '1':
		पक्ष '2':
		पक्ष '3':
		पक्ष '4':
		पक्ष '5':
		पक्ष '6':
		पक्ष '7':
		पक्ष '8':
		पक्ष '9': जइ ((reg_do_extmatch & REX_USE) == 0)
			      EMSG_RET_NULL(_(e_z1_z9_not_allowed_here));
			  ret = regnode(ZREF + c - '0');
			  re_has_z = REX_USE;
			  भङ्ग;
#endif

		पक्ष 's': ret = regnode(MOPEN + 0);
			  जइ (re_mult_next("\\zs") == FAIL)
			      प्रत्यागम रिक्त;
			  भङ्ग;

		पक्ष 'e': ret = regnode(MCLOSE + 0);
			  जइ (re_mult_next("\\ze") == FAIL)
			      प्रत्यागम रिक्त;
			  भङ्ग;

		स्वभाव:  EMSG_RET_NULL(_(e_invalid_character_after_bsl_z));
	    }
	}
	भङ्ग;

      पक्ष Magic('%'):
	{
	    c = no_Magic(getchr());
	    विकल्प (c)
	    {
		// () without a back reference
		पक्ष '(':
		    जइ (one_exactly)
			EMSG_ONE_RET_NULL;
		    ret = reg(REG_NPAREN, &flags);
		    जइ (ret == रिक्त)
			प्रत्यागम रिक्त;
		    *flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
		    भङ्ग;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		पक्ष '^':
		    ret = regnode(RE_BOF);
		    भङ्ग;

		पक्ष '$':
		    ret = regnode(RE_EOF);
		    भङ्ग;

		पक्ष '#':
		    जइ (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_start_of_pattern),
								  regparse[1]);
			प्रत्यागम FAIL;
		    }
		    ret = regnode(CURSOR);
		    भङ्ग;

		पक्ष 'V':
		    ret = regnode(RE_VISUAL);
		    भङ्ग;

		पक्ष 'C':
		    ret = regnode(RE_COMPOSING);
		    भङ्ग;

		// \%[abc]: Emit as a list of branches, all ending at the last
		// branch which matches nothing.
		पक्ष '[':
			  जइ (one_exactly)	// doesn't nest
			      EMSG_ONE_RET_NULL;
			  {
			      char_u	*lastbranch;
			      char_u	*lastnode = रिक्त;
			      char_u	*br;

			      ret = रिक्त;
			      जाव ((c = getchr()) != ']')
			      {
				  जइ (c == NUL)
				      EMSG2_RET_NULL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
				  br = regnode(BRANCH);
				  जइ (ret == रिक्त)
				      ret = br;
				  अण्णहा
				  {
				      regtail(lastnode, br);
				      जइ (reg_toolong)
					  प्रत्यागम रिक्त;
				  }

				  ungetchr();
				  one_exactly = TRUE;
				  lastnode = regatom(flagp);
				  one_exactly = FALSE;
				  जइ (lastnode == रिक्त)
				      प्रत्यागम रिक्त;
			      }
			      जइ (ret == रिक्त)
				  EMSG2_RET_NULL(_(e_empty_str_brackets),
						      reg_magic == MAGIC_ALL);
			      lastbranch = regnode(BRANCH);
			      br = regnode(NOTHING);
			      जइ (ret != JUST_CALC_SIZE)
			      {
				  regtail(lastnode, br);
				  regtail(lastbranch, br);
				  // connect all branches to the NOTHING
				  // branch at the end
				  प्रति (br = ret; br != lastnode; )
				  {
				      जइ (OP(br) == BRANCH)
				      {
					  regtail(br, lastbranch);
					  जइ (reg_toolong)
					      प्रत्यागम रिक्त;
					  br = OPERAND(br);
				      }
				      अण्णहा
					  br = regnext(br);
				  }
			      }
			      *flagp &= ~(HASWIDTH | SIMPLE);
			      भङ्ग;
			  }

		पक्ष 'd':   // %d123 decimal
		पक्ष 'o':   // %o123 octal
		पक्ष 'x':   // %xab hex 2
		पक्ष 'u':   // %uabcd hex 4
		पक्ष 'U':   // %U1234abcd hex 8
			  {
			      vimlong_T i;

			      विकल्प (c)
			      {
				  पक्ष 'd': i = getdecchrs(); भङ्ग;
				  पक्ष 'o': i = getoctchrs(); भङ्ग;
				  पक्ष 'x': i = gethexchrs(2); भङ्ग;
				  पक्ष 'u': i = gethexchrs(4); भङ्ग;
				  पक्ष 'U': i = gethexchrs(8); भङ्ग;
				  स्वभाव:  i = -1; भङ्ग;
			      }

			      जइ (i < 0 || i > INT_MAX)
				  EMSG2_RET_NULL(
					    _(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			      जइ (use_multibytecode(i))
				  ret = regnode(MULTIBYTECODE);
			      अण्णहा
				  ret = regnode(EXACTLY);
			      जइ (i == 0)
				  regc(0x0a);
			      अण्णहा
				  regmbc((पूर्णाङ्क)i);
			      regc(NUL);
			      *flagp |= HASWIDTH;
			      भङ्ग;
			  }

		स्वभाव:
			  जइ (VIM_ISDIGIT(c) || c == '<' || c == '>'
						|| c == '\'' || c == '.')
			  {
			      long_u	n = 0;
			      पूर्णाङ्क	cmp;
			      पूर्णाङ्क	cur = FALSE;
			      पूर्णाङ्क	got_digit = FALSE;

			      cmp = c;
			      जइ (cmp == '<' || cmp == '>')
				  c = getchr();
			      जइ (no_Magic(c) == '.')
			      {
				  cur = TRUE;
				  c = getchr();
			      }
			      जाव (VIM_ISDIGIT(c))
			      {
				  got_digit = TRUE;
				  n = n * 10 + (c - '0');
				  c = getchr();
			      }
			      जइ (no_Magic(c) == '\'' && n == 0)
			      {
				  // "\%'m", "\%<'m" and "\%>'m": Mark
				  c = getchr();
				  ret = regnode(RE_MARK);
				  जइ (ret == JUST_CALC_SIZE)
				      regsize += 2;
				  अण्णहा
				  {
				      *regcode++ = c;
				      *regcode++ = cmp;
				  }
				  भङ्ग;
			      }
			      अण्णहा जइ ((c == 'l' || c == 'c' || c == 'v')
					  && (cur || got_digit))
			      {
				  जइ (cur && n)
				  {
				    semsg(_(e_regexp_सङ्ख्या_after_dot_pos_अन्वेषणम्_chr),
								  no_Magic(c));
				    rc_did_emsg = TRUE;
				    प्रत्यागम रिक्त;
				  }
				  जइ (c == 'l')
				  {
				      जइ (cur)
					  n = curwin->w_cursor.lnum;
				      ret = regnode(RE_LNUM);
				      जइ (save_prev_at_start)
					  at_start = TRUE;
				  }
				  अण्णहा जइ (c == 'c')
				  {
				      जइ (cur)
				      {
					  n = curwin->w_cursor.col;
					  n++;
				      }
				      ret = regnode(RE_COL);
				  }
				  अण्णहा
				  {
				      जइ (cur)
				      {
					  colnr_T vcol = 0;

					  getvvcol(curwin, &curwin->w_cursor,
							 रिक्त, रिक्त, &vcol, 0);
					  ++vcol;
					  n = vcol;
				      }
				      ret = regnode(RE_VCOL);
				  }
				  जइ (ret == JUST_CALC_SIZE)
				      regsize += 5;
				  अण्णहा
				  {
				      // put the number and the optional
				      // comparator after the opcode
				      regcode = re_put_long(regcode, n);
				      *regcode++ = cmp;
				  }
				  भङ्ग;
			      }
			  }

			  EMSG2_RET_NULL(_(e_invalid_character_after_str),
						      reg_magic == MAGIC_ALL);
	    }
	}
	भङ्ग;

      पक्ष Magic('['):
collection:
	{
	    char_u	*lp;

	    // If there is no matching ']', we assume the '[' is a normal
	    // character.  This makes 'incsearch' and ":help [" work.
	    lp = skip_anyof(regparse);
	    जइ (*lp == ']')	// there is a matching ']'
	    {
		पूर्णाङ्क	startc = -1;	// > 0 when next '-' is a range
		पूर्णाङ्क	endc;

		जइ (extra == 0)
		{
		    पूर्णाङ्क	cl = bt_recognize_char_class(regparse, lp);

		    जइ (cl != 0)
		    {
			ret = regnode(cl);
			regparse = lp;
			skipchr();
			*flagp |= HASWIDTH | SIMPLE;
			भङ्ग;
		    }
		}

		// In a character class, different parsing rules apply.
		// Not even \ is special anymore, nothing is.
		जइ (*regparse == '^')	    // Complement of range.
		{
		    ret = regnode(ANYBUT + extra);
		    regparse++;
		}
		अण्णहा
		    ret = regnode(ANYOF + extra);

		// At the start ']' and '-' mean the literal character.
		जइ (*regparse == ']' || *regparse == '-')
		{
		    startc = *regparse;
		    regc(*regparse++);
		}

		जाव (*regparse != NUL && *regparse != ']')
		{
		    जइ (*regparse == '-')
		    {
			++regparse;
			// The '-' is not used for a range at the end and
			// after or before a '\n'.
			जइ (*regparse == ']' || *regparse == NUL
				|| startc == -1
				|| (regparse[0] == '\\' && regparse[1] == 'n'))
			{
			    regc('-');
			    startc = '-';	// [--x] is a range
			}
			अण्णहा
			{
			    // Also accept "a-[.z.]"
			    endc = 0;
			    जइ (*regparse == '[')
				endc = get_coll_element(&regparse);
			    जइ (endc == 0)
			    {
				जइ (has_mbyte)
				    endc = mb_ptr2char_adv(&regparse);
				अण्णहा
				    endc = *regparse++;
			    }

			    // Handle \o40, \x20 and \u20AC style sequences
			    जइ (endc == '\\' && !reg_cpo_lit && !reg_cpo_bsl)
				endc = coll_get_char();

			    जइ (startc > endc)
				EMSG_RET_NULL(_(e_reverse_range_in_character_class));
			    जइ (has_mbyte && ((*mb_char2len)(startc) > 1
						 || (*mb_char2len)(endc) > 1))
			    {
				// Limit to a range of 256 chars.
				जइ (endc > startc + 256)
				    EMSG_RET_NULL(_(e_range_too_large_in_character_class));
				जाव (++startc <= endc)
				    regmbc(startc);
			    }
			    अण्णहा
			    {
				जाव (++startc <= endc)
				    regc(startc);
			    }
			    startc = -1;
			}
		    }
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    अण्णहा जइ (*regparse == '\\'
			    && !reg_cpo_bsl
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != रिक्त
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR,
						       regparse[1]) != रिक्त)))
		    {
			regparse++;
			जइ (*regparse == 'n')
			{
			    // '\n' in range: also match NL
			    जइ (ret != JUST_CALC_SIZE)
			    {
				// Using \n inside [^] does not change what
				// matches. "[^\n]" is the same as ".".
				जइ (*ret == ANYOF)
				{
				    *ret = ANYOF + ADD_NL;
				    *flagp |= HASNL;
				}
				// else: must have had a \n already
			    }
			    regparse++;
			    startc = -1;
			}
			अण्णहा जइ (*regparse == 'd'
				|| *regparse == 'o'
				|| *regparse == 'x'
				|| *regparse == 'u'
				|| *regparse == 'U')
			{
			    startc = coll_get_char();
			    // max UTF-8 Codepoint is U+10FFFF,
			    // but allow values until INT_MAX
			    जइ (startc == INT_MAX)
				EMSG_RET_NULL(_(e_unicode_val_too_large));
			    जइ (startc == 0)
				regc(0x0a);
			    अण्णहा
				regmbc(startc);
			}
			अण्णहा
			{
			    startc = backslash_trans(*regparse++);
			    regc(startc);
			}
		    }
		    अण्णहा जइ (*regparse == '[')
		    {
			पूर्णाङ्क c_class;
			पूर्णाङ्क cu;

			c_class = get_char_class(&regparse);
			startc = -1;
			// Characters assumed to be 8 bits!
			विकल्प (c_class)
			{
			    पक्ष CLASS_NONE:
				c_class = get_equi_class(&regparse);
				जइ (c_class != 0)
				{
				    // produce equivalence class
				    reg_equi_class(c_class);
				}
				अण्णहा जइ ((c_class =
					    get_coll_element(&regparse)) != 0)
				{
				    // produce a collating element
				    regmbc(c_class);
				}
				अण्णहा
				{
				    // literal '[', allow [[-x] as a range
				    startc = *regparse++;
				    regc(startc);
				}
				भङ्ग;
			    पक्ष CLASS_ALNUM:
				प्रति (cu = 1; cu < 128; cu++)
				    जइ (isalnum(cu))
					regmbc(cu);
				भङ्ग;
			    पक्ष CLASS_ALPHA:
				प्रति (cu = 1; cu < 128; cu++)
				    जइ (isalpha(cu))
					regmbc(cu);
				भङ्ग;
			    पक्ष CLASS_BLANK:
				regc(' ');
				regc('\t');
				भङ्ग;
			    पक्ष CLASS_CNTRL:
				प्रति (cu = 1; cu <= 127; cu++)
				    जइ (iscntrl(cu))
					regmbc(cu);
				भङ्ग;
			    पक्ष CLASS_DIGIT:
				प्रति (cu = 1; cu <= 127; cu++)
				    जइ (VIM_ISDIGIT(cu))
					regmbc(cu);
				भङ्ग;
			    पक्ष CLASS_GRAPH:
				प्रति (cu = 1; cu <= 127; cu++)
				    जइ (isgraph(cu))
					regmbc(cu);
				भङ्ग;
			    पक्ष CLASS_LOWER:
				प्रति (cu = 1; cu <= 255; cu++)
				    जइ (MB_ISLOWER(cu) && cu != 170
								 && cu != 186)
					regmbc(cu);
				भङ्ग;
			    पक्ष CLASS_PRINT:
				प्रति (cu = 1; cu <= 255; cu++)
				    जइ (vim_isprintc(cu))
					regmbc(cu);
				भङ्ग;
			    पक्ष CLASS_PUNCT:
				प्रति (cu = 1; cu < 128; cu++)
				    जइ (ispunct(cu))
					regmbc(cu);
				भङ्ग;
			    पक्ष CLASS_SPACE:
				प्रति (cu = 9; cu <= 13; cu++)
				    regc(cu);
				regc(' ');
				भङ्ग;
			    पक्ष CLASS_UPPER:
				प्रति (cu = 1; cu <= 255; cu++)
				    जइ (MB_ISUPPER(cu))
					regmbc(cu);
				भङ्ग;
			    पक्ष CLASS_XDIGIT:
				प्रति (cu = 1; cu <= 255; cu++)
				    जइ (vim_isxdigit(cu))
					regmbc(cu);
				भङ्ग;
			    पक्ष CLASS_TAB:
				regc('\t');
				भङ्ग;
			    पक्ष CLASS_RETURN:
				regc('\r');
				भङ्ग;
			    पक्ष CLASS_BACKSPACE:
				regc('\b');
				भङ्ग;
			    पक्ष CLASS_ESCAPE:
				regc('\033');
				भङ्ग;
			    पक्ष CLASS_IDENT:
				प्रति (cu = 1; cu <= 255; cu++)
				    जइ (vim_isIDc(cu))
					regmbc(cu);
				भङ्ग;
			    पक्ष CLASS_KEYWORD:
				प्रति (cu = 1; cu <= 255; cu++)
				    जइ (reg_iswordc(cu))
					regmbc(cu);
				भङ्ग;
			    पक्ष CLASS_FNAME:
				प्रति (cu = 1; cu <= 255; cu++)
				    जइ (vim_isfilec(cu))
					regmbc(cu);
				भङ्ग;
			}
		    }
		    अण्णहा
		    {
			जइ (has_mbyte)
			{
			    पूर्णाङ्क	len;

			    // produce a multibyte character, including any
			    // following composing characters
			    startc = mb_ptr2char(regparse);
			    len = (*mb_ptr2len)(regparse);
			    जइ (enc_utf8 && utf_char2len(startc) != len)
				startc = -1;	// composing chars
			    जाव (--len >= 0)
				regc(*regparse++);
			}
			अण्णहा
			{
			    startc = *regparse++;
			    regc(startc);
			}
		    }
		}
		regc(NUL);
		prevchr_len = 1;	// last char was the ']'
		जइ (*regparse != ']')
		    EMSG_RET_NULL(_(e_too_many_brackets));  // Cannot happen?
		skipchr();	    // let's be friends with the lexer again
		*flagp |= HASWIDTH | SIMPLE;
		भङ्ग;
	    }
	    अण्णहा जइ (reg_strict)
		EMSG2_RET_NULL(_(e_missing_rsb_after_str_lsb),
							reg_magic > MAGIC_OFF);
	}
	// FALLTHROUGH

      स्वभाव:
	{
	    पूर्णाङ्क		len;

	    // A multi-byte character is handled as a separate atom if it's
	    // before a multi and when it's a composing char.
	    जइ (use_multibytecode(c))
	    {
do_multibyte:
		ret = regnode(MULTIBYTECODE);
		regmbc(c);
		*flagp |= HASWIDTH | SIMPLE;
		भङ्ग;
	    }

	    ret = regnode(EXACTLY);

	    // Append characters as long as:
	    // - there is no following multi, we then need the character in
	    //   front of it as a single character operand
	    // - not running into a Magic character
	    // - "one_exactly" is not set
	    // But always emit at least one character.  Might be a Multi,
	    // e.g., a "[" without matching "]".
	    प्रति (len = 0; c != NUL && (len == 0
			|| (re_multi_type(peekchr()) == NOT_MULTI
			    && !one_exactly
			    && !is_Magic(c))); ++len)
	    {
		c = no_Magic(c);
		जइ (has_mbyte)
		{
		    regmbc(c);
		    जइ (enc_utf8)
		    {
			पूर्णाङ्क	l;

			// Need to get composing character too.
			प्रति (;;)
			{
			    l = utf_ptr2len(regparse);
			    जइ (!UTF_COMPOSINGLIKE(regparse, regparse + l))
				भङ्ग;
			    regmbc(utf_ptr2char(regparse));
			    skipchr();
			}
		    }
		}
		अण्णहा
		    regc(c);
		c = getchr();
	    }
	    ungetchr();

	    regc(NUL);
	    *flagp |= HASWIDTH;
	    जइ (len == 1)
		*flagp |= SIMPLE;
	}
	भङ्ग;
    }

    प्रत्यागम ret;
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
    स्थिर char_u *
regpiece(पूर्णाङ्क *flagp)
{
    char_u	    *ret;
    पूर्णाङ्क		    op;
    char_u	    *next;
    पूर्णाङ्क		    flags;
    long	    minval;
    long	    maxval;

    ret = regatom(&flags);
    जइ (ret == रिक्त)
	प्रत्यागम रिक्त;

    op = peekchr();
    जइ (re_multi_type(op) == NOT_MULTI)
    {
	*flagp = flags;
	प्रत्यागम ret;
    }
    // default flags
    *flagp = (WORST | SPSTART | (flags & (HASNL | HASLOOKBH)));

    skipchr();
    विकल्प (op)
    {
	पक्ष Magic('*'):
	    जइ (flags & SIMPLE)
		reginsert(STAR, ret);
	    अण्णहा
	    {
		// Emit x* as (x&|), where & means "self".
		reginsert(BRANCH, ret); // Either x
		regoptail(ret, regnode(BACK));	// and loop
		regoptail(ret, ret);	// back
		regtail(ret, regnode(BRANCH));	// or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    भङ्ग;

	पक्ष Magic('+'):
	    जइ (flags & SIMPLE)
		reginsert(PLUS, ret);
	    अण्णहा
	    {
		// Emit x+ as x(&|), where & means "self".
		next = regnode(BRANCH); // Either
		regtail(ret, next);
		regtail(regnode(BACK), ret);	// loop back
		regtail(next, regnode(BRANCH)); // or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    *flagp = (WORST | HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    भङ्ग;

	पक्ष Magic('@'):
	    {
		पूर्णाङ्क	lop = END;
		long	nr;

		nr = (long)getdecchrs();
		विकल्प (no_Magic(getchr()))
		{
		    पक्ष '=': lop = MATCH; भङ्ग;		  // \@=
		    पक्ष '!': lop = NOMATCH; भङ्ग;		  // \@!
		    पक्ष '>': lop = SUBPAT; भङ्ग;		  // \@>
		    पक्ष '<': विकल्प (no_Magic(getchr()))
			      {
				  पक्ष '=': lop = BEHIND; भङ्ग;   // \@<=
				  पक्ष '!': lop = NOBEHIND; भङ्ग; // \@<!
			      }
		}
		जइ (lop == END)
		    EMSG2_RET_NULL(_(e_invalid_character_after_str_at),
						      reg_magic == MAGIC_ALL);
		// Look behind must match with behind_pos.
		जइ (lop == BEHIND || lop == NOBEHIND)
		{
		    regtail(ret, regnode(BHPOS));
		    *flagp |= HASLOOKBH;
		}
		regtail(ret, regnode(END)); // operand ends
		जइ (lop == BEHIND || lop == NOBEHIND)
		{
		    जइ (nr < 0)
			nr = 0; // no limit is same as zero limit
		    reginsert_nr(lop, nr, ret);
		}
		अण्णहा
		    reginsert(lop, ret);
		भङ्ग;
	    }

	पक्ष Magic('?'):
	पक्ष Magic('='):
	    // Emit x= as (x|)
	    reginsert(BRANCH, ret);		// Either x
	    regtail(ret, regnode(BRANCH));	// or
	    next = regnode(NOTHING);		// null.
	    regtail(ret, next);
	    regoptail(ret, next);
	    भङ्ग;

	पक्ष Magic('{'):
	    जइ (!read_limits(&minval, &maxval))
		प्रत्यागम रिक्त;
	    जइ (flags & SIMPLE)
	    {
		reginsert(BRACE_SIMPLE, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
	    }
	    अण्णहा
	    {
		जइ (अङ्कः_complex_braces >= 10)
		    EMSG2_RET_NULL(_(e_too_many_complex_str_curly),
						      reg_magic == MAGIC_ALL);
		reginsert(BRACE_COMPLEX + अङ्कः_complex_braces, ret);
		regoptail(ret, regnode(BACK));
		regoptail(ret, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
		++अङ्कः_complex_braces;
	    }
	    जइ (minval > 0 && maxval > 0)
		*flagp = (HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    भङ्ग;
    }
    जइ (re_multi_type(peekchr()) != NOT_MULTI)
    {
	// Can't have a multi follow a multi.
	जइ (peekchr() == Magic('*'))
	    EMSG2_RET_NULL(_(e_nested_str), reg_magic >= MAGIC_ON);
	EMSG3_RET_NULL(_(e_nested_str_chr), reg_magic == MAGIC_ALL,
							  no_Magic(peekchr()));
    }

    प्रत्यागम ret;
}

/*
 * Parse one alternative of an | or & operator.
 * Implements the concatenation operator.
 */
    स्थिर char_u *
regconcat(पूर्णाङ्क *flagp)
{
    char_u	*first = रिक्त;
    char_u	*chain = रिक्त;
    char_u	*latest;
    पूर्णाङ्क		flags;
    पूर्णाङ्क		cont = TRUE;

    *flagp = WORST;		// Tentatively.

    जाव (cont)
    {
	विकल्प (peekchr())
	{
	    पक्ष NUL:
	    पक्ष Magic('|'):
	    पक्ष Magic('&'):
	    पक्ष Magic(')'):
			    cont = FALSE;
			    भङ्ग;
	    पक्ष Magic('Z'):
			    regflags |= RF_ICOMBINE;
			    skipchr_keepstart();
			    भङ्ग;
	    पक्ष Magic('c'):
			    regflags |= RF_ICASE;
			    skipchr_keepstart();
			    भङ्ग;
	    पक्ष Magic('C'):
			    regflags |= RF_NOICASE;
			    skipchr_keepstart();
			    भङ्ग;
	    पक्ष Magic('v'):
			    reg_magic = MAGIC_ALL;
			    skipchr_keepstart();
			    curchr = -1;
			    भङ्ग;
	    पक्ष Magic('m'):
			    reg_magic = MAGIC_ON;
			    skipchr_keepstart();
			    curchr = -1;
			    भङ्ग;
	    पक्ष Magic('M'):
			    reg_magic = MAGIC_OFF;
			    skipchr_keepstart();
			    curchr = -1;
			    भङ्ग;
	    पक्ष Magic('V'):
			    reg_magic = MAGIC_NONE;
			    skipchr_keepstart();
			    curchr = -1;
			    भङ्ग;
	    स्वभाव:
			    latest = regpiece(&flags);
			    जइ (latest == रिक्त || reg_toolong)
				प्रत्यागम रिक्त;
			    *flagp |= flags & (HASWIDTH | HASNL | HASLOOKBH);
			    जइ (chain == रिक्त)	// First piece.
				*flagp |= flags & SPSTART;
			    अण्णहा
				regtail(chain, latest);
			    chain = latest;
			    जइ (first == रिक्त)
				first = latest;
			    भङ्ग;
	}
    }
    जइ (first == रिक्त)		// Loop ran zero times.
	first = regnode(NOTHING);
    प्रत्यागम first;
}

/*
 * Parse one alternative of an | operator.
 * Implements the & operator.
 */
    स्थिर char_u *
regbranch(पूर्णाङ्क *flagp)
{
    char_u	*ret;
    char_u	*chain = रिक्त;
    char_u	*latest;
    पूर्णाङ्क		flags;

    *flagp = WORST | HASNL;		// Tentatively.

    ret = regnode(BRANCH);
    प्रति (;;)
    {
	latest = regconcat(&flags);
	जइ (latest == रिक्त)
	    प्रत्यागम रिक्त;
	// If one of the branches has width, the whole thing has.  If one of
	// the branches anchors at start-of-line, the whole thing does.
	// If one of the branches uses look-behind, the whole thing does.
	*flagp |= flags & (HASWIDTH | SPSTART | HASLOOKBH);
	// If one of the branches doesn't match a line-break, the whole thing
	// doesn't.
	*flagp &= ~HASNL | (flags & HASNL);
	जइ (chain != रिक्त)
	    regtail(chain, latest);
	जइ (peekchr() != Magic('&'))
	    भङ्ग;
	skipchr();
	regtail(latest, regnode(END)); // operand ends
	जइ (reg_toolong)
	    भङ्ग;
	reginsert(MATCH, latest);
	chain = latest;
    }

    प्रत्यागम ret;
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
    स्थिर char_u *
reg(
    पूर्णाङ्क		paren,	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
    पूर्णाङ्क		*flagp)
{
    char_u	*ret;
    char_u	*br;
    char_u	*ender;
    पूर्णाङ्क		parno = 0;
    पूर्णाङ्क		flags;

    *flagp = HASWIDTH;		// Tentatively.

#ifdef FEAT_SYN_HL
    जइ (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	जइ (regnzpar >= NSUBEXP)
	    EMSG_RET_NULL(_(e_too_many_z));
	parno = regnzpar;
	regnzpar++;
	ret = regnode(ZOPEN + parno);
    }
    अण्णहा
#endif
	जइ (paren == REG_PAREN)
    {
	// Make a MOPEN node.
	जइ (regnpar >= NSUBEXP)
	    EMSG2_RET_NULL(_(e_too_many_str_open), reg_magic == MAGIC_ALL);
	parno = regnpar;
	++regnpar;
	ret = regnode(MOPEN + parno);
    }
    अण्णहा जइ (paren == REG_NPAREN)
    {
	// Make a NOPEN node.
	ret = regnode(NOPEN);
    }
    अण्णहा
	ret = रिक्त;

    जइ (bt_reg_parse_depth >= REG_MAX_PAREN_DEPTH)
	EMSG_RET_NULL(_(e_command_too_complex));
    ++bt_reg_parse_depth;

    // Pick up the branches, linking them together.
    br = regbranch(&flags);
    जइ (br == रिक्त)
    {
	ret = रिक्त;
	गच्छ theend;
    }
    जइ (ret != रिक्त)
	regtail(ret, br);	// [MZ]OPEN -> first.
    अण्णहा
	ret = br;
    // If one of the branches can be zero-width, the whole thing can.
    // If one of the branches has * at start or matches a line-break, the
    // whole thing can.
    जइ (!(flags & HASWIDTH))
	*flagp &= ~HASWIDTH;
    *flagp |= flags & (SPSTART | HASNL | HASLOOKBH);
    जाव (peekchr() == Magic('|'))
    {
	skipchr();
	br = regbranch(&flags);
	जइ (br == रिक्त || reg_toolong)
	{
	    ret = रिक्त;
	    गच्छ theend;
	}
	regtail(ret, br);	// BRANCH -> BRANCH.
	जइ (!(flags & HASWIDTH))
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
    प्रति (br = ret; br != रिक्त; br = regnext(br))
	regoptail(br, ender);

    // Check for proper termination.
    जइ (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
#ifdef FEAT_SYN_HL
	जइ (paren == REG_ZPAREN)
	{
	    emsg(_(e_unmatched_z));
	    rc_did_emsg = TRUE;
	    ret = रिक्त;
	    गच्छ theend;
	}
	अण्णहा
#endif
	जइ (paren == REG_NPAREN)
	{
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = रिक्त;
	    गच्छ theend;
	}
	अण्णहा
	{
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = रिक्त;
	    गच्छ theend;
	}
    }
    अण्णहा जइ (paren == REG_NOPAREN && peekchr() != NUL)
    {
	जइ (curchr == Magic(')'))
	{
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = रिक्त;
	    गच्छ theend;
	}
	अण्णहा
	{
	    emsg(_(e_trailing_characters));	// "Can't happen".
	    rc_did_emsg = TRUE;
	    ret = रिक्त;
	    गच्छ theend;
	}
    }
    // Here we set the flag allowing back references to this set of
    // parentheses.
    जइ (paren == REG_PAREN)
	had_endbrace[parno] = TRUE;	// have seen the close paren

theend:
    --bt_reg_parse_depth;
    प्रत्यागम ret;
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
    स्थिर regprog_T *
bt_regcomp(char_u *व्यञ्जकम्, पूर्णाङ्क re_flags)
{
    bt_regprog_T    *r;
    char_u	*scan;
    char_u	*longest;
    पूर्णाङ्क		len;
    पूर्णाङ्क		flags;

    जइ (व्यञ्जकम् == रिक्त)
	IEMSG_RET_NULL(e_null_argument);

    init_class_tab();

    // First pass: determine size, legality.
    regcomp_start(व्यञ्जकम्, re_flags);
    regcode = JUST_CALC_SIZE;
    regc(REGMAGIC);
    जइ (reg(REG_NOPAREN, &flags) == रिक्त)
	प्रत्यागम रिक्त;

    // Allocate space.
    r = alloc(offsetof(bt_regprog_T, program) + regsize);
    जइ (r == रिक्त)
	प्रत्यागम रिक्त;
    r->re_in_use = FALSE;
#ifdef DEBUG
    r->regsz = regsize;
#endif

    // Second pass: emit code.
    regcomp_start(व्यञ्जकम्, re_flags);
    regcode = r->program;
    regc(REGMAGIC);
    जइ (reg(REG_NOPAREN, &flags) == रिक्त || reg_toolong)
    {
	vim_free(r);
	जइ (reg_toolong)
	    EMSG_RET_NULL(_(e_pattern_too_long));
	प्रत्यागम रिक्त;
    }

    // Dig out information for optimizations.
    r->regstart = NUL;		// Worst-case defaults.
    r->reganch = 0;
    r->regmust = रिक्त;
    r->regmlen = 0;
    r->regflags = regflags;
    जइ (flags & HASNL)
	r->regflags |= RF_HASNL;
    जइ (flags & HASLOOKBH)
	r->regflags |= RF_LOOKBH;
#ifdef FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    r->reghasz = re_has_z;
#endif
    scan = r->program + 1;	// First BRANCH.
    जइ (OP(regnext(scan)) == END)   // Only one top-level choice.
    {
	scan = OPERAND(scan);

	// Starting-point info.
	जइ (OP(scan) == BOL || OP(scan) == RE_BOF)
	{
	    r->reganch++;
	    scan = regnext(scan);
	}

	जइ (OP(scan) == EXACTLY)
	{
	    जइ (has_mbyte)
		r->regstart = (*mb_ptr2char)(OPERAND(scan));
	    अण्णहा
		r->regstart = *OPERAND(scan);
	}
	अण्णहा जइ ((OP(scan) == BOW
		    || OP(scan) == EOW
		    || OP(scan) == NOTHING
		    || OP(scan) == MOPEN + 0 || OP(scan) == NOPEN
		    || OP(scan) == MCLOSE + 0 || OP(scan) == NCLOSE)
		 && OP(regnext(scan)) == EXACTLY)
	{
	    जइ (has_mbyte)
		r->regstart = (*mb_ptr2char)(OPERAND(regnext(scan)));
	    अण्णहा
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
	जइ ((flags & SPSTART || OP(scan) == BOW || OP(scan) == EOW)
							  && !(flags & HASNL))
	{
	    size_t  scanlen;

	    longest = रिक्त;
	    len = 0;
	    प्रति (; scan != रिक्त; scan = regnext(scan))
	    {
		जइ (OP(scan) == EXACTLY)
		{
		    scanlen = STRLEN(OPERAND(scan));
		    जइ (scanlen >= (size_t)len)
		    {
			longest = OPERAND(scan);
			len = (पूर्णाङ्क)scanlen;
		    }
		}
	    }
	    r->regmust = longest;
	    r->regmlen = len;
	}
    }
#ifdef BT_REGEXP_DUMP
    regdump(व्यञ्जकम्, r);
#endif
    r->engine = &bt_regengine;
    प्रत्यागम (regprog_T *)r;
}

#जइ defined(FEAT_SYN_HL)
/*
 * Check if during the previous call to vim_regcomp the EOL item "$" has been
 * found.  This is messy, but it works fine.
 */
    पूर्णाङ्क
vim_regcomp_had_eol(शून्य)
{
    प्रत्यागम had_eol;
}
#endif

/*
 * Get a number after a backslash that is inside [].
 * When nothing is recognized return a backslash.
 */
    स्थिर पूर्णाङ्क
coll_get_char(शून्य)
{
    vimlong_T	nr = -1;

    विकल्प (*regparse++)
    {
	पक्ष 'd': nr = getdecchrs(); भङ्ग;
	पक्ष 'o': nr = getoctchrs(); भङ्ग;
	पक्ष 'x': nr = gethexchrs(2); भङ्ग;
	पक्ष 'u': nr = gethexchrs(4); भङ्ग;
	पक्ष 'U': nr = gethexchrs(8); भङ्ग;
    }
    जइ (nr < 0)
    {
	// If getting the number fails be backwards compatible: the character
	// is a backslash.
	--regparse;
	nr = '\\';
    }
    जइ (nr > INT_MAX)
	nr = INT_MAX;
    प्रत्यागम nr;
}

/*
 * Free a compiled regexp program, returned by bt_regcomp().
 */
    स्थिर शून्य
bt_regfree(regprog_T *prog)
{
    vim_free(prog);
}

#define ADVANCE_REGINPUT() MB_PTR_ADV(rex.निवेशः)

/*
 * The arguments from BRACE_LIMITS are stored here.  They are actually local
 * to regmatch(), but they are here to reduce the amount of stack space used
 * (it can be called recursively many times).
 */
स्थिर long	bl_minval;
स्थिर long	bl_maxval;

/*
 * Save the input line and position in a regsave_T.
 */
    स्थिर शून्य
reg_save(regsave_T *save, garray_T *gap)
{
    जइ (REG_MULTI)
    {
	save->rs_u.pos.col = (colnr_T)(rex.निवेशः - rex.पङ्क्तिः);
	save->rs_u.pos.lnum = rex.lnum;
    }
    अण्णहा
	save->rs_u.ptr = rex.निवेशः;
    save->rs_len = gap->ga_len;
}

/*
 * Restore the input line and position from a regsave_T.
 */
    स्थिर शून्य
reg_restore(regsave_T *save, garray_T *gap)
{
    जइ (REG_MULTI)
    {
	जइ (rex.lnum != save->rs_u.pos.lnum)
	{
	    // only call reg_getline() when the line number changed to save
	    // a bit of time
	    rex.lnum = save->rs_u.pos.lnum;
	    rex.पङ्क्तिः = reg_getline(rex.lnum);
	}
	rex.निवेशः = rex.पङ्क्तिः + save->rs_u.pos.col;
    }
    अण्णहा
	rex.निवेशः = save->rs_u.ptr;
    gap->ga_len = save->rs_len;
}

/*
 * Return TRUE if current position is equal to saved position.
 */
    स्थिर पूर्णाङ्क
reg_save_equal(regsave_T *save)
{
    जइ (REG_MULTI)
	प्रत्यागम rex.lnum == save->rs_u.pos.lnum
				  && rex.निवेशः == rex.पङ्क्तिः + save->rs_u.pos.col;
    प्रत्यागम rex.निवेशः == save->rs_u.ptr;
}

// Save the sub-expressions before attempting a match.
#define save_se(savep, posp, pp) \
    REG_MULTI ? save_se_multi((savep), (posp)) : save_se_one((savep), (pp))

// After a failed match restore the sub-expressions.
#define restore_se(savep, posp, pp) \
{ \
    जइ (REG_MULTI) \
	*(posp) = (savep)->se_u.pos; \
    अण्णहा \
	*(pp) = (savep)->se_u.ptr; \
}

/*
 * Tentatively set the sub-expression start to the current position (after
 * calling regmatch() they will have changed).  Need to save the existing
 * values for when there is no match.
 * Use se_save() to use pointer (save_se_multi()) or position (save_se_one()),
 * depending on REG_MULTI.
 */
    स्थिर शून्य
save_se_multi(save_se_T *savep, lpos_T *posp)
{
    savep->se_u.pos = *posp;
    posp->lnum = rex.lnum;
    posp->col = (colnr_T)(rex.निवेशः - rex.पङ्क्तिः);
}

    स्थिर शून्य
save_se_one(save_se_T *savep, char_u **pp)
{
    savep->se_u.ptr = *pp;
    *pp = rex.निवेशः;
}

/*
 * regrepeat - repeatedly match something simple, return how many.
 * Advances rex.input (and rex.lnum) to just after the matched chars.
 */
    स्थिर पूर्णाङ्क
regrepeat(
    char_u	*p,
    long	maxcount)   // maximum number of matches allowed
{
    long	गणनम् = 0;
    char_u	*scan;
    char_u	*opnd;
    पूर्णाङ्क		mask;
    पूर्णाङ्क		testval = 0;

    scan = rex.निवेशः;	    // Make local copy of rex.input for speed.
    opnd = OPERAND(p);
    विकल्प (OP(p))
    {
      पक्ष ANY:
      पक्ष ANY + ADD_NL:
	जाव (गणनम् < maxcount)
	{
	    // Matching anything means we continue until end-of-line (or
	    // end-of-file for ANY + ADD_NL), only limited by maxcount.
	    जाव (*scan != NUL && गणनम् < maxcount)
	    {
		++गणनम्;
		MB_PTR_ADV(scan);
	    }
	    जइ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
				      || rex.reg_पङ्क्तिः_lbr || गणनम् == maxcount)
		भङ्ग;
	    ++गणनम्;		// count the line-break
	    reg_nextline();
	    scan = rex.निवेशः;
	    जइ (got_int)
		भङ्ग;
	}
	भङ्ग;

      पक्ष IDENT:
      पक्ष IDENT + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      पक्ष SIDENT:
      पक्ष SIDENT + ADD_NL:
	जाव (गणनम् < maxcount)
	{
	    जइ (vim_isIDc(PTR2CHAR(scan)) && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    अण्णहा जइ (*scan == NUL)
	    {
		जइ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_पङ्क्तिः_lbr)
		    भङ्ग;
		reg_nextline();
		scan = rex.निवेशः;
		जइ (got_int)
		    भङ्ग;
	    }
	    अण्णहा जइ (rex.reg_पङ्क्तिः_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    अण्णहा
		भङ्ग;
	    ++गणनम्;
	}
	भङ्ग;

      पक्ष KWORD:
      पक्ष KWORD + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      पक्ष SKWORD:
      पक्ष SKWORD + ADD_NL:
	जाव (गणनम् < maxcount)
	{
	    जइ (vim_iswordp_buf(scan, rex.reg_buf)
					  && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    अण्णहा जइ (*scan == NUL)
	    {
		जइ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_पङ्क्तिः_lbr)
		    भङ्ग;
		reg_nextline();
		scan = rex.निवेशः;
		जइ (got_int)
		    भङ्ग;
	    }
	    अण्णहा जइ (rex.reg_पङ्क्तिः_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    अण्णहा
		भङ्ग;
	    ++गणनम्;
	}
	भङ्ग;

      पक्ष FNAME:
      पक्ष FNAME + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      पक्ष SFNAME:
      पक्ष SFNAME + ADD_NL:
	जाव (गणनम् < maxcount)
	{
	    जइ (vim_isfilec(PTR2CHAR(scan)) && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    अण्णहा जइ (*scan == NUL)
	    {
		जइ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_पङ्क्तिः_lbr)
		    भङ्ग;
		reg_nextline();
		scan = rex.निवेशः;
		जइ (got_int)
		    भङ्ग;
	    }
	    अण्णहा जइ (rex.reg_पङ्क्तिः_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    अण्णहा
		भङ्ग;
	    ++गणनम्;
	}
	भङ्ग;

      पक्ष PRINT:
      पक्ष PRINT + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      पक्ष SPRINT:
      पक्ष SPRINT + ADD_NL:
	जाव (गणनम् < maxcount)
	{
	    जइ (*scan == NUL)
	    {
		जइ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_पङ्क्तिः_lbr)
		    भङ्ग;
		reg_nextline();
		scan = rex.निवेशः;
		जइ (got_int)
		    भङ्ग;
	    }
	    अण्णहा जइ (vim_isprintc(PTR2CHAR(scan)) == 1
					  && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    अण्णहा जइ (rex.reg_पङ्क्तिः_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    अण्णहा
		भङ्ग;
	    ++गणनम्;
	}
	भङ्ग;

      पक्ष WHITE:
      पक्ष WHITE + ADD_NL:
	testval = mask = RI_WHITE;
do_class:
	जाव (गणनम् < maxcount)
	{
	    पूर्णाङ्क		l;

	    जइ (*scan == NUL)
	    {
		जइ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_पङ्क्तिः_lbr)
		    भङ्ग;
		reg_nextline();
		scan = rex.निवेशः;
		जइ (got_int)
		    भङ्ग;
	    }
	    अण्णहा जइ (has_mbyte && (l = (*mb_ptr2len)(scan)) > 1)
	    {
		जइ (testval != 0)
		    भङ्ग;
		scan += l;
	    }
	    अण्णहा जइ ((class_tab[*scan] & mask) == testval)
		++scan;
	    अण्णहा जइ (rex.reg_पङ्क्तिः_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    अण्णहा
		भङ्ग;
	    ++गणनम्;
	}
	भङ्ग;

      पक्ष NWHITE:
      पक्ष NWHITE + ADD_NL:
	mask = RI_WHITE;
	गच्छ do_class;
      पक्ष DIGIT:
      पक्ष DIGIT + ADD_NL:
	testval = mask = RI_DIGIT;
	गच्छ do_class;
      पक्ष NDIGIT:
      पक्ष NDIGIT + ADD_NL:
	mask = RI_DIGIT;
	गच्छ do_class;
      पक्ष HEX:
      पक्ष HEX + ADD_NL:
	testval = mask = RI_HEX;
	गच्छ do_class;
      पक्ष NHEX:
      पक्ष NHEX + ADD_NL:
	mask = RI_HEX;
	गच्छ do_class;
      पक्ष OCTAL:
      पक्ष OCTAL + ADD_NL:
	testval = mask = RI_OCTAL;
	गच्छ do_class;
      पक्ष NOCTAL:
      पक्ष NOCTAL + ADD_NL:
	mask = RI_OCTAL;
	गच्छ do_class;
      पक्ष WORD:
      पक्ष WORD + ADD_NL:
	testval = mask = RI_WORD;
	गच्छ do_class;
      पक्ष NWORD:
      पक्ष NWORD + ADD_NL:
	mask = RI_WORD;
	गच्छ do_class;
      पक्ष HEAD:
      पक्ष HEAD + ADD_NL:
	testval = mask = RI_HEAD;
	गच्छ do_class;
      पक्ष NHEAD:
      पक्ष NHEAD + ADD_NL:
	mask = RI_HEAD;
	गच्छ do_class;
      पक्ष ALPHA:
      पक्ष ALPHA + ADD_NL:
	testval = mask = RI_ALPHA;
	गच्छ do_class;
      पक्ष NALPHA:
      पक्ष NALPHA + ADD_NL:
	mask = RI_ALPHA;
	गच्छ do_class;
      पक्ष LOWER:
      पक्ष LOWER + ADD_NL:
	testval = mask = RI_LOWER;
	गच्छ do_class;
      पक्ष NLOWER:
      पक्ष NLOWER + ADD_NL:
	mask = RI_LOWER;
	गच्छ do_class;
      पक्ष UPPER:
      पक्ष UPPER + ADD_NL:
	testval = mask = RI_UPPER;
	गच्छ do_class;
      पक्ष NUPPER:
      पक्ष NUPPER + ADD_NL:
	mask = RI_UPPER;
	गच्छ do_class;

      पक्ष EXACTLY:
	{
	    पूर्णाङ्क	    cu, cl;

	    // This doesn't do a multi-byte character, because a MULTIBYTECODE
	    // would have been used for it.  It does handle single-byte
	    // characters, such as latin1.
	    जइ (rex.reg_ic)
	    {
		cu = MB_TOUPPER(*opnd);
		cl = MB_TOLOWER(*opnd);
		जाव (गणनम् < maxcount && (*scan == cu || *scan == cl))
		{
		    गणनम्++;
		    scan++;
		}
	    }
	    अण्णहा
	    {
		cu = *opnd;
		जाव (गणनम् < maxcount && *scan == cu)
		{
		    गणनम्++;
		    scan++;
		}
	    }
	    भङ्ग;
	}

      पक्ष MULTIBYTECODE:
	{
	    पूर्णाङ्क		i, len, cf = 0;

	    // Safety check (just in case 'encoding' was changed since
	    // compiling the program).
	    जइ ((len = (*mb_ptr2len)(opnd)) > 1)
	    {
		जइ (rex.reg_ic && enc_utf8)
		    cf = utf_fold(utf_ptr2char(opnd));
		जाव (गणनम् < maxcount && (*mb_ptr2len)(scan) >= len)
		{
		    प्रति (i = 0; i < len; ++i)
			जइ (opnd[i] != scan[i])
			    भङ्ग;
		    जइ (i < len && (!rex.reg_ic || !enc_utf8
					|| utf_fold(utf_ptr2char(scan)) != cf))
			भङ्ग;
		    scan += len;
		    ++गणनम्;
		}
	    }
	}
	भङ्ग;

      पक्ष ANYOF:
      पक्ष ANYOF + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH

      पक्ष ANYBUT:
      पक्ष ANYBUT + ADD_NL:
	जाव (गणनम् < maxcount)
	{
	    पूर्णाङ्क len;

	    जइ (*scan == NUL)
	    {
		जइ (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_पङ्क्तिः_lbr)
		    भङ्ग;
		reg_nextline();
		scan = rex.निवेशः;
		जइ (got_int)
		    भङ्ग;
	    }
	    अण्णहा जइ (rex.reg_पङ्क्तिः_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    अण्णहा जइ (has_mbyte && (len = (*mb_ptr2len)(scan)) > 1)
	    {
		जइ ((cstrchr(opnd, (*mb_ptr2char)(scan)) == रिक्त) == testval)
		    भङ्ग;
		scan += len;
	    }
	    अण्णहा
	    {
		जइ ((cstrchr(opnd, *scan) == रिक्त) == testval)
		    भङ्ग;
		++scan;
	    }
	    ++गणनम्;
	}
	भङ्ग;

      पक्ष NEWL:
	जाव (गणनम् < maxcount
		&& ((*scan == NUL && rex.lnum <= rex.reg_maxline
				       && !rex.reg_पङ्क्तिः_lbr && REG_MULTI)
		    || (*scan == '\n' && rex.reg_पङ्क्तिः_lbr)))
	{
	    गणनम्++;
	    जइ (rex.reg_पङ्क्तिः_lbr)
		ADVANCE_REGINPUT();
	    अण्णहा
		reg_nextline();
	    scan = rex.निवेशः;
	    जइ (got_int)
		भङ्ग;
	}
	भङ्ग;

      स्वभाव:			// Oh dear.  Called inappropriately.
	iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	printf("Called regrepeat with op code %d\n", OP(p));
#endif
	भङ्ग;
    }

    rex.निवेशः = scan;

    प्रत्यागम (पूर्णाङ्क)गणनम्;
}

/*
 * Push an item onto the regstack.
 * Returns pointer to new item.  Returns NULL when out of memory.
 */
    स्थिर regitem_T *
regstack_प्रक्षिप(regstate_T state, char_u *scan)
{
    regitem_T	*rp;

    जइ ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
    {
	emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
	प्रत्यागम रिक्त;
    }
    जइ (GA_GROW_FAILS(&regstack, (पूर्णाङ्क)परिमाण(regitem_T)))
	प्रत्यागम रिक्त;

    rp = (regitem_T *)((वर्ण *)regstack.ga_data + regstack.ga_len);
    rp->rs_state = state;
    rp->rs_scan = scan;

    regstack.ga_len += परिमाण(regitem_T);
    प्रत्यागम rp;
}

/*
 * Pop an item from the regstack.
 */
    स्थिर शून्य
regstack_निष्कासय(char_u **scan)
{
    regitem_T	*rp;

    rp = (regitem_T *)((वर्ण *)regstack.ga_data + regstack.ga_len) - 1;
    *scan = rp->rs_scan;

    regstack.ga_len -= परिमाण(regitem_T);
}

#ifdef FEAT_RELTIME
/*
 * Check if the timer expired, return TRUE if so.
 */
    स्थिर पूर्णाङ्क
bt_did_time_out(पूर्णाङ्क *timed_out)
{
    जइ (*timeout_flag)
    {
	जइ (timed_out != रिक्त)
	{
# ifdef FEAT_EVAL
	    जइ (!*timed_out)
		ch_log(रिक्त, "BT regexp timed out");
# endif
	    *timed_out = TRUE;
	}
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}
#endif

/*
 * Save the current subexpr to "bp", so that they can be restored
 * later by restore_subexpr().
 */
    स्थिर शून्य
save_subexpr(regbehind_T *bp)
{
    पूर्णाङ्क i;

    // When "rex.need_clear_subexpr" is set we don't need to save the values,
    // only remember that this flag needs to be set again when restoring.
    bp->save_need_clear_subexpr = rex.need_clear_subexpr;
    जइ (rex.need_clear_subexpr)
	प्रत्यागम;

    प्रति (i = 0; i < NSUBEXP; ++i)
    {
	जइ (REG_MULTI)
	{
	    bp->save_start[i].se_u.pos = rex.reg_startpos[i];
	    bp->save_end[i].se_u.pos = rex.reg_endpos[i];
	}
	अण्णहा
	{
	    bp->save_start[i].se_u.ptr = rex.reg_startp[i];
	    bp->save_end[i].se_u.ptr = rex.reg_endp[i];
	}
    }
}

/*
 * Restore the subexpr from "bp".
 */
    स्थिर शून्य
restore_subexpr(regbehind_T *bp)
{
    पूर्णाङ्क i;

    // Only need to restore saved values when they are not to be cleared.
    rex.need_clear_subexpr = bp->save_need_clear_subexpr;
    जइ (rex.need_clear_subexpr)
	प्रत्यागम;

    प्रति (i = 0; i < NSUBEXP; ++i)
    {
	जइ (REG_MULTI)
	{
	    rex.reg_startpos[i] = bp->save_start[i].se_u.pos;
	    rex.reg_endpos[i] = bp->save_end[i].se_u.pos;
	}
	अण्णहा
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
    स्थिर पूर्णाङ्क
regmatch(
    char_u	*scan,		    // Current node.
    पूर्णाङ्क		*timed_out UNUSED)  // flag set on timeout or NULL
{
  char_u	*next;		// Next node.
  पूर्णाङ्क		op;
  पूर्णाङ्क		c;
  regitem_T	*rp;
  पूर्णाङ्क		no;
  पूर्णाङ्क		status;		// one of the RA_ values:

  // Make "regstack" and "backpos" empty.  They are allocated and freed in
  // bt_regexec_both() to reduce malloc()/free() calls.
  regstack.ga_len = 0;
  backpos.ga_len = 0;

  // Repeat until "regstack" is empty.
  प्रति (;;)
  {
    // Some patterns may take a long time to match, e.g., "\([a-z]\+\)\+Q".
    // Allow interrupting them with CTRL-C.
    fast_breakcheck();

#ifdef DEBUG
    जइ (scan != रिक्त && regnarrate)
    {
	mch_errmsg((वर्ण *)regprop(scan));
	mch_errmsg("(\n");
    }
#endif

    // Repeat for items that can be matched sequentially, without using the
    // regstack.
    प्रति (;;)
    {
	जइ (got_int || scan == रिक्त)
	{
	    status = RA_FAIL;
	    भङ्ग;
	}
#ifdef FEAT_RELTIME
	जइ (bt_did_time_out(timed_out))
	{
	    status = RA_FAIL;
	    भङ्ग;
	}
#endif
	status = RA_CONT;

#ifdef DEBUG
	जइ (regnarrate)
	{
	    mch_errmsg((वर्ण *)regprop(scan));
	    mch_errmsg("...\n");
# ifdef FEAT_SYN_HL
	    जइ (re_extmatch_in != रिक्त)
	    {
		पूर्णाङ्क i;

		mch_errmsg(_("External submatches:\n"));
		प्रति (i = 0; i < NSUBEXP; i++)
		{
		    mch_errmsg("    \"");
		    जइ (re_extmatch_in->matches[i] != रिक्त)
			mch_errmsg((वर्ण *)re_extmatch_in->matches[i]);
		    mch_errmsg("\"\n");
		}
	    }
# endif
	}
#endif
	next = regnext(scan);

	op = OP(scan);
	// Check for character class with NL added.
	जइ (!rex.reg_पङ्क्तिः_lbr && WITH_NL(op) && REG_MULTI
			     && *rex.निवेशः == NUL && rex.lnum <= rex.reg_maxline)
	{
	    reg_nextline();
	}
	अण्णहा जइ (rex.reg_पङ्क्तिः_lbr && WITH_NL(op) && *rex.निवेशः == '\n')
	{
	    ADVANCE_REGINPUT();
	}
	अण्णहा
	{
	  जइ (WITH_NL(op))
	      op -= ADD_NL;
	  जइ (has_mbyte)
	      c = (*mb_ptr2char)(rex.निवेशः);
	  अण्णहा
	      c = *rex.निवेशः;
	  विकल्प (op)
	  {
	  पक्ष BOL:
	    जइ (rex.निवेशः != rex.पङ्क्तिः)
		status = RA_NOMATCH;
	    भङ्ग;

	  पक्ष EOL:
	    जइ (c != NUL)
		status = RA_NOMATCH;
	    भङ्ग;

	  पक्ष RE_BOF:
	    // We're not at the beginning of the file when below the first
	    // line where we started, not at the start of the line or we
	    // didn't start at the first line of the buffer.
	    जइ (rex.lnum != 0 || rex.निवेशः != rex.पङ्क्तिः
				       || (REG_MULTI && rex.reg_firstlnum > 1))
		status = RA_NOMATCH;
	    भङ्ग;

	  पक्ष RE_EOF:
	    जइ (rex.lnum != rex.reg_maxline || c != NUL)
		status = RA_NOMATCH;
	    भङ्ग;

	  पक्ष CURSOR:
	    // Check if the buffer is in a window and compare the
	    // rex.reg_win->w_cursor position to the match position.
	    जइ (rex.reg_win == रिक्त
		    || (rex.lnum + rex.reg_firstlnum
						 != rex.reg_win->w_cursor.lnum)
		    || ((colnr_T)(rex.निवेशः - rex.पङ्क्तिः)
						 != rex.reg_win->w_cursor.col))
		status = RA_NOMATCH;
	    भङ्ग;

	  पक्ष RE_MARK:
	    // Compare the mark position to the match position.
	    {
		पूर्णाङ्क	mark = OPERAND(scan)[0];
		पूर्णाङ्क	cmp = OPERAND(scan)[1];
		pos_T	*pos;
		size_t	col = REG_MULTI ? rex.निवेशः - rex.पङ्क्तिः : 0;

		pos = getmark_buf(rex.reg_buf, mark, FALSE);

		// Line may have been freed, get it again.
		जइ (REG_MULTI)
		{
		    rex.पङ्क्तिः = reg_getline(rex.lnum);
		    rex.निवेशः = rex.पङ्क्तिः + col;
		}

		जइ (pos == रिक्त		     // mark doesn't exist
			|| pos->lnum <= 0)   // mark isn't set in reg_buf
		{
		    status = RA_NOMATCH;
		}
		अण्णहा
		{
		    colnr_T pos_col = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->col == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->col;

		    जइ ((pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_col == (colnr_T)(rex.निवेशः - rex.पङ्क्तिः)
				    ? (cmp == '<' || cmp == '>')
				    : (pos_col < (colnr_T)(rex.निवेशः - rex.पङ्क्तिः)
					? cmp != '>'
					: cmp != '<'))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? cmp != '>'
				    : cmp != '<')))
		    status = RA_NOMATCH;
		}
	    }
	    भङ्ग;

	  पक्ष RE_VISUAL:
	    जइ (!reg_match_visual())
		status = RA_NOMATCH;
	    भङ्ग;

	  पक्ष RE_LNUM:
	    जइ (!REG_MULTI || !re_अङ्कः_cmp((long_u)(rex.lnum + rex.reg_firstlnum),
									scan))
		status = RA_NOMATCH;
	    भङ्ग;

	  पक्ष RE_COL:
	    जइ (!re_अङ्कः_cmp((long_u)(rex.निवेशः - rex.पङ्क्तिः) + 1, scan))
		status = RA_NOMATCH;
	    भङ्ग;

	  पक्ष RE_VCOL:
	    {
		win_T	    *wp = rex.reg_win == रिक्त ? curwin : rex.reg_win;
		linenr_T    lnum = REG_MULTI ? rex.reg_firstlnum + rex.lnum : 1;
		long_u	    vcol;

		जइ (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्))
		    lnum = 1;
		vcol = (long_u)win_linetabsize(wp, lnum, rex.पङ्क्तिः,
					      (colnr_T)(rex.निवेशः - rex.पङ्क्तिः));
		जइ (!re_अङ्कः_cmp(vcol + 1, scan))
		    status = RA_NOMATCH;
	    }
	    भङ्ग;

	  पक्ष BOW:	// \<word; rex.input points to w
	    जइ (c == NUL)	// Can't match at end of line
		status = RA_NOMATCH;
	    अण्णहा जइ (has_mbyte)
	    {
		पूर्णाङ्क this_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.निवेशः, rex.reg_buf);
		जइ (this_class <= 1)
		    status = RA_NOMATCH;  // not on a word at all
		अण्णहा जइ (reg_prev_class() == this_class)
		    status = RA_NOMATCH;  // previous char is in same word
	    }
	    अण्णहा
	    {
		जइ (!vim_iswordc_buf(c, rex.reg_buf) || (rex.निवेशः > rex.पङ्क्तिः
				&& vim_iswordc_buf(rex.निवेशः[-1], rex.reg_buf)))
		    status = RA_NOMATCH;
	    }
	    भङ्ग;

	  पक्ष EOW:	// word\>; rex.input points after d
	    जइ (rex.निवेशः == rex.पङ्क्तिः)    // Can't match at start of line
		status = RA_NOMATCH;
	    अण्णहा जइ (has_mbyte)
	    {
		पूर्णाङ्क this_class, prev_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.निवेशः, rex.reg_buf);
		prev_class = reg_prev_class();
		जइ (this_class == prev_class
			|| prev_class == 0 || prev_class == 1)
		    status = RA_NOMATCH;
	    }
	    अण्णहा
	    {
		जइ (!vim_iswordc_buf(rex.निवेशः[-1], rex.reg_buf)
			|| (rex.निवेशः[0] != NUL
					   && vim_iswordc_buf(c, rex.reg_buf)))
		    status = RA_NOMATCH;
	    }
	    भङ्ग; // Matched with EOW

	  पक्ष ANY:
	    // ANY does not match new lines.
	    जइ (c == NUL)
		status = RA_NOMATCH;
	    अण्णहा
		ADVANCE_REGINPUT();
	    भङ्ग;

	  पक्ष IDENT:
	    जइ (!vim_isIDc(c))
		status = RA_NOMATCH;
	    अण्णहा
		ADVANCE_REGINPUT();
	    भङ्ग;

	  पक्ष SIDENT:
	    जइ (VIM_ISDIGIT(*rex.निवेशः) || !vim_isIDc(c))
		status = RA_NOMATCH;
	    अण्णहा
		ADVANCE_REGINPUT();
	    भङ्ग;

	  पक्ष KWORD:
	    जइ (!vim_iswordp_buf(rex.निवेशः, rex.reg_buf))
		status = RA_NOMATCH;
	    अण्णहा
		ADVANCE_REGINPUT();
	    भङ्ग;

	  पक्ष SKWORD:
	    जइ (VIM_ISDIGIT(*rex.निवेशः)
				    || !vim_iswordp_buf(rex.निवेशः, rex.reg_buf))
		status = RA_NOMATCH;
	    अण्णहा
		ADVANCE_REGINPUT();
	    भङ्ग;

	  पक्ष FNAME:
	    जइ (!vim_isfilec(c))
		status = RA_NOMATCH;
	    अण्णहा
		ADVANCE_REGINPUT();
	    भङ्ग;

	  पक्ष SFNAME:
	    जइ (VIM_ISDIGIT(*rex.निवेशः) || !vim_isfilec(c))
		status = RA_NOMATCH;
	    अण्णहा
		ADVANCE_REGINPUT();
	    भङ्ग;

	  पक्ष PRINT:
	    जइ (!vim_isprintc(PTR2CHAR(rex.निवेशः)))
		status = RA_NOMATCH;
	    अण्णहा
		ADVANCE_REGINPUT();
	    भङ्ग;

	  पक्ष SPRINT:
	    जइ (VIM_ISDIGIT(*rex.निवेशः) || !vim_isprintc(PTR2CHAR(rex.निवेशः)))
		status = RA_NOMATCH;
	    अण्णहा
		ADVANCE_REGINPUT();
	    भङ्ग;

	  पक्ष WHITE:
	    जइ (!VIM_ISWHITE(c))
		status = RA_NOMATCH;
	    अण्णहा
		ADVANCE_REGINPUT();
	    भङ्ग;

	  पक्ष NWHITE:
	    जइ (c == NUL || VIM_ISWHITE(c))
		status = RA_NOMATCH;
	    अण्णहा
		ADVANCE_REGINPUT();
	    भङ्ग;

	  पक्ष DIGIT:
	    जइ (!ri_digit(c))
		status = RA_NOMATCH;
	    अण्णहा
		ADVANCE_REGINPUT();
	    भङ्ग;

	  पक्ष NDIGIT:
	    जइ (c == NUL || ri_digit(c))
		status = RA_NOMATCH;
	    अण्णहा
		ADVANCE_REGINPUT();
	    भङ्ग;

	  पक्ष HEX:
	    जइ (!ri_hex(c))
		status = RA_NOMATCH;
	    अण्णहा
		ADVANCE_REGINPUT();
	    भङ्ग;

	  पक्ष NHEX:
	    जइ (c == NUL || ri_hex(c))
		status = RA_NOMATCH;
	    अण्णहा
		ADVANCE_REGINPUT();
	    भङ्ग;

	  पक्ष OCTAL:
	    जइ (!ri_octal(c))
		status = RA_NOMATCH;
	    अण्णहा
		ADVANCE_REGINPUT();
	    भङ्ग;

	  पक्ष NOCTAL:
	    जइ (c == NUL || ri_octal(c))
		status = RA_NOMATCH;
	    अण्णहा
		ADVANCE_REGINPUT();
	    भङ्ग;

	  पक्ष WORD:
	    जइ (!ri_word(c))
		status = RA_NOMATCH;
	    अण्णहा
		ADVANCE_REGINPUT();
	    भङ्ग;

	  पक्ष NWORD:
	    जइ (c == NUL || ri_word(c))
		status = RA_NOMATCH;
	    अण्णहा
		ADVANCE_REGINPUT();
	    भङ्ग;

	  पक्ष HEAD:
	    जइ (!ri_head(c))
		status = RA_NOMATCH;
	    अण्णहा
		ADVANCE_REGINPUT();
	    भङ्ग;

	  पक्ष NHEAD:
	    जइ (c == NUL || ri_head(c))
		status = RA_NOMATCH;
	    अण्णहा
		ADVANCE_REGINPUT();
	    भङ्ग;

	  पक्ष ALPHA:
	    जइ (!ri_alpha(c))
		status = RA_NOMATCH;
	    अण्णहा
		ADVANCE_REGINPUT();
	    भङ्ग;

	  पक्ष NALPHA:
	    जइ (c == NUL || ri_alpha(c))
		status = RA_NOMATCH;
	    अण्णहा
		ADVANCE_REGINPUT();
	    भङ्ग;

	  पक्ष LOWER:
	    जइ (!ri_lower(c))
		status = RA_NOMATCH;
	    अण्णहा
		ADVANCE_REGINPUT();
	    भङ्ग;

	  पक्ष NLOWER:
	    जइ (c == NUL || ri_lower(c))
		status = RA_NOMATCH;
	    अण्णहा
		ADVANCE_REGINPUT();
	    भङ्ग;

	  पक्ष UPPER:
	    जइ (!ri_upper(c))
		status = RA_NOMATCH;
	    अण्णहा
		ADVANCE_REGINPUT();
	    भङ्ग;

	  पक्ष NUPPER:
	    जइ (c == NUL || ri_upper(c))
		status = RA_NOMATCH;
	    अण्णहा
		ADVANCE_REGINPUT();
	    भङ्ग;

	  पक्ष EXACTLY:
	    {
		पूर्णाङ्क	len;
		char_u	*opnd;

		opnd = OPERAND(scan);
		// Inline the first byte, for speed.
		जइ (*opnd != *rex.निवेशः
			&& (!rex.reg_ic
			    || (!enc_utf8
			      && MB_TOLOWER(*opnd) != MB_TOLOWER(*rex.निवेशः))))
		    status = RA_NOMATCH;
		अण्णहा जइ (*opnd == NUL)
		{
		    // match empty string always works; happens when "~" is
		    // empty.
		}
		अण्णहा
		{
		    जइ (opnd[1] == NUL && !(enc_utf8 && rex.reg_ic))
		    {
			len = 1;	// matched a single byte above
		    }
		    अण्णहा
		    {
			// Need to match first byte again for multi-byte.
			len = (पूर्णाङ्क)STRLEN(opnd);
			जइ (cstrncmp(opnd, rex.निवेशः, &len) != 0)
			    status = RA_NOMATCH;
		    }
		    // Check for following composing character, unless %C
		    // follows (skips over all composing chars).
		    जइ (status != RA_NOMATCH
			    && enc_utf8
			    && UTF_COMPOSINGLIKE(rex.निवेशः, rex.निवेशः + len)
			    && !rex.reg_icombine
			    && OP(next) != RE_COMPOSING)
		    {
			// raaron: This code makes a composing character get
			// ignored, which is the correct behavior (sometimes)
			// for voweled Hebrew texts.
			status = RA_NOMATCH;
		    }
		    जइ (status != RA_NOMATCH)
			rex.निवेशः += len;
		}
	    }
	    भङ्ग;

	  पक्ष ANYOF:
	  पक्ष ANYBUT:
	    {
		char_u  *q = OPERAND(scan);

		जइ (c == NUL)
		    status = RA_NOMATCH;
		अण्णहा जइ ((cstrchr(q, c) == रिक्त) == (op == ANYOF))
		    status = RA_NOMATCH;
		अण्णहा
		{
		    // Check following combining characters
		    पूर्णाङ्क	len = 0;
		    पूर्णाङ्क i;

		    जइ (enc_utf8)
			len = utfc_ptr2len(q) - utf_ptr2len(q);

		    MB_CPTR_ADV(rex.निवेशः);
		    MB_CPTR_ADV(q);

		    जइ (!enc_utf8 || len == 0)
			भङ्ग;

		    प्रति (i = 0; i < len; ++i)
			जइ (q[i] != rex.निवेशः[i])
			{
			    status = RA_NOMATCH;
			    भङ्ग;
			}
		    rex.निवेशः += len;
		}
		भङ्ग;
	    }

	  पक्ष MULTIBYTECODE:
	    जइ (has_mbyte)
	    {
		पूर्णाङ्क	i, len;
		char_u	*opnd;
		पूर्णाङ्क	opndc = 0, inpc;

		opnd = OPERAND(scan);
		// Safety check (just in case 'encoding' was changed since
		// compiling the program).
		जइ ((len = (*mb_ptr2len)(opnd)) < 2)
		{
		    status = RA_NOMATCH;
		    भङ्ग;
		}
		जइ (enc_utf8)
		    opndc = utf_ptr2char(opnd);
		जइ (enc_utf8 && utf_iscomposing(opndc))
		{
		    // When only a composing char is given match at any
		    // position where that composing char appears.
		    status = RA_NOMATCH;
		    प्रति (i = 0; rex.निवेशः[i] != NUL;
						i += utf_ptr2len(rex.निवेशः + i))
		    {
			inpc = utf_ptr2char(rex.निवेशः + i);
			जइ (!utf_iscomposing(inpc))
			{
			    जइ (i > 0)
				भङ्ग;
			}
			अण्णहा जइ (opndc == inpc)
			{
			    // Include all following composing chars.
			    len = i + utfc_ptr2len(rex.निवेशः + i);
			    status = RA_MATCH;
			    भङ्ग;
			}
		    }
		}
		अण्णहा जइ (enc_utf8)
		{
		    जइ (cstrncmp(opnd, rex.निवेशः, &len) != 0)
		    {
			status = RA_NOMATCH;
			भङ्ग;
		    }
		}
		अण्णहा
		    प्रति (i = 0; i < len; ++i)
			जइ (opnd[i] != rex.निवेशः[i])
			{
			    status = RA_NOMATCH;
			    भङ्ग;
			}
		rex.निवेशः += len;
	    }
	    अण्णहा
		status = RA_NOMATCH;
	    भङ्ग;
	  पक्ष RE_COMPOSING:
	    जइ (enc_utf8)
	    {
		// Skip composing characters.
		जाव (utf_iscomposing(utf_ptr2char(rex.निवेशः)))
		    MB_CPTR_ADV(rex.निवेशः);
	    }
	    भङ्ग;

	  पक्ष NOTHING:
	    भङ्ग;

	  पक्ष BACK:
	    {
		पूर्णाङ्क		i;
		backpos_T	*bp;

		// When we run into BACK we need to check if we don't keep
		// looping without matching any input.  The second and later
		// times a BACK is encountered it fails if the input is still
		// at the same position as the previous time.
		// The positions are stored in "backpos" and found by the
		// current value of "scan", the position in the RE program.
		bp = (backpos_T *)backpos.ga_data;
		प्रति (i = 0; i < backpos.ga_len; ++i)
		    जइ (bp[i].bp_scan == scan)
			भङ्ग;
		जइ (i == backpos.ga_len)
		{
		    // First time at this BACK, make room to store the pos.
		    जइ (GA_GROW_FAILS(&backpos, 1))
			status = RA_FAIL;
		    अण्णहा
		    {
			// get "ga_data" again, it may have changed
			bp = (backpos_T *)backpos.ga_data;
			bp[i].bp_scan = scan;
			++backpos.ga_len;
		    }
		}
		अण्णहा जइ (reg_save_equal(&bp[i].bp_pos))
		    // Still at same position as last time, fail.
		    status = RA_NOMATCH;

		जइ (status != RA_FAIL && status != RA_NOMATCH)
		    reg_save(&bp[i].bp_pos, &backpos);
	    }
	    भङ्ग;

	  पक्ष MOPEN + 0:   // Match start: \zs
	  पक्ष MOPEN + 1:   // \(
	  पक्ष MOPEN + 2:
	  पक्ष MOPEN + 3:
	  पक्ष MOPEN + 4:
	  पक्ष MOPEN + 5:
	  पक्ष MOPEN + 6:
	  पक्ष MOPEN + 7:
	  पक्ष MOPEN + 8:
	  पक्ष MOPEN + 9:
	    {
		no = op - MOPEN;
		cleanup_subexpr();
		rp = regstack_प्रक्षिप(RS_MOPEN, scan);
		जइ (rp == रिक्त)
		    status = RA_FAIL;
		अण्णहा
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_startpos[no],
							  &rex.reg_startp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    भङ्ग;

	  पक्ष NOPEN:	    // \%(
	  पक्ष NCLOSE:	    // \) after \%(
		जइ (regstack_प्रक्षिप(RS_NOPEN, scan) == रिक्त)
		    status = RA_FAIL;
		// We simply continue and handle the result when done.
		भङ्ग;

#ifdef FEAT_SYN_HL
	  पक्ष ZOPEN + 1:
	  पक्ष ZOPEN + 2:
	  पक्ष ZOPEN + 3:
	  पक्ष ZOPEN + 4:
	  पक्ष ZOPEN + 5:
	  पक्ष ZOPEN + 6:
	  पक्ष ZOPEN + 7:
	  पक्ष ZOPEN + 8:
	  पक्ष ZOPEN + 9:
	    {
		no = op - ZOPEN;
		cleanup_zsubexpr();
		rp = regstack_प्रक्षिप(RS_ZOPEN, scan);
		जइ (rp == रिक्त)
		    status = RA_FAIL;
		अण्णहा
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_startzpos[no],
							     &reg_startzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    भङ्ग;
#endif

	  पक्ष MCLOSE + 0:  // Match end: \ze
	  पक्ष MCLOSE + 1:  // \)
	  पक्ष MCLOSE + 2:
	  पक्ष MCLOSE + 3:
	  पक्ष MCLOSE + 4:
	  पक्ष MCLOSE + 5:
	  पक्ष MCLOSE + 6:
	  पक्ष MCLOSE + 7:
	  पक्ष MCLOSE + 8:
	  पक्ष MCLOSE + 9:
	    {
		no = op - MCLOSE;
		cleanup_subexpr();
		rp = regstack_प्रक्षिप(RS_MCLOSE, scan);
		जइ (rp == रिक्त)
		    status = RA_FAIL;
		अण्णहा
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_endpos[no],
							    &rex.reg_endp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    भङ्ग;

#ifdef FEAT_SYN_HL
	  पक्ष ZCLOSE + 1:  // \) after \z(
	  पक्ष ZCLOSE + 2:
	  पक्ष ZCLOSE + 3:
	  पक्ष ZCLOSE + 4:
	  पक्ष ZCLOSE + 5:
	  पक्ष ZCLOSE + 6:
	  पक्ष ZCLOSE + 7:
	  पक्ष ZCLOSE + 8:
	  पक्ष ZCLOSE + 9:
	    {
		no = op - ZCLOSE;
		cleanup_zsubexpr();
		rp = regstack_प्रक्षिप(RS_ZCLOSE, scan);
		जइ (rp == रिक्त)
		    status = RA_FAIL;
		अण्णहा
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_endzpos[no],
							      &reg_endzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    भङ्ग;
#endif

	  पक्ष BACKREF + 1:
	  पक्ष BACKREF + 2:
	  पक्ष BACKREF + 3:
	  पक्ष BACKREF + 4:
	  पक्ष BACKREF + 5:
	  पक्ष BACKREF + 6:
	  पक्ष BACKREF + 7:
	  पक्ष BACKREF + 8:
	  पक्ष BACKREF + 9:
	    {
		पूर्णाङ्क		len;

		no = op - BACKREF;
		cleanup_subexpr();
		जइ (!REG_MULTI)		// Single-line regexp
		{
		    जइ (rex.reg_startp[no] == रिक्त || rex.reg_endp[no] == रिक्त)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    अण्णहा
		    {
			// Compare current input with back-ref in the same
			// line.
			len = (पूर्णाङ्क)(rex.reg_endp[no] - rex.reg_startp[no]);
			जइ (cstrncmp(rex.reg_startp[no], rex.निवेशः, &len) != 0)
			    status = RA_NOMATCH;
		    }
		}
		अण्णहा				// Multi-line regexp
		{
		    जइ (rex.reg_startpos[no].lnum < 0
						|| rex.reg_endpos[no].lnum < 0)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    अण्णहा
		    {
			जइ (rex.reg_startpos[no].lnum == rex.lnum
				&& rex.reg_endpos[no].lnum == rex.lnum)
			{
			    // Compare back-ref within the current line.
			    len = rex.reg_endpos[no].col
						    - rex.reg_startpos[no].col;
			    जइ (cstrncmp(rex.पङ्क्तिः + rex.reg_startpos[no].col,
							  rex.निवेशः, &len) != 0)
				status = RA_NOMATCH;
			}
			अण्णहा
			{
			    // Messy situation: Need to compare between two
			    // lines.
			    पूर्णाङ्क r = match_with_backref(
					    rex.reg_startpos[no].lnum,
					    rex.reg_startpos[no].col,
					    rex.reg_endpos[no].lnum,
					    rex.reg_endpos[no].col,
					    &len);

			    जइ (r != RA_MATCH)
				status = r;
			}
		    }
		}

		// Matched the backref, skip over it.
		rex.निवेशः += len;
	    }
	    भङ्ग;

#ifdef FEAT_SYN_HL
	  पक्ष ZREF + 1:
	  पक्ष ZREF + 2:
	  पक्ष ZREF + 3:
	  पक्ष ZREF + 4:
	  पक्ष ZREF + 5:
	  पक्ष ZREF + 6:
	  पक्ष ZREF + 7:
	  पक्ष ZREF + 8:
	  पक्ष ZREF + 9:
	    {
		पूर्णाङ्क	len;

		cleanup_zsubexpr();
		no = op - ZREF;
		जइ (re_extmatch_in != रिक्त
			&& re_extmatch_in->matches[no] != रिक्त)
		{
		    len = (पूर्णाङ्क)STRLEN(re_extmatch_in->matches[no]);
		    जइ (cstrncmp(re_extmatch_in->matches[no],
							  rex.निवेशः, &len) != 0)
			status = RA_NOMATCH;
		    अण्णहा
			rex.निवेशः += len;
		}
		अण्णहा
		{
		    // Backref was not set: Match an empty string.
		}
	    }
	    भङ्ग;
#endif

	  पक्ष BRANCH:
	    {
		जइ (OP(next) != BRANCH) // No choice.
		    next = OPERAND(scan);	// Avoid recursion.
		अण्णहा
		{
		    rp = regstack_प्रक्षिप(RS_BRANCH, scan);
		    जइ (rp == रिक्त)
			status = RA_FAIL;
		    अण्णहा
			status = RA_BREAK;	// rest is below
		}
	    }
	    भङ्ग;

	  पक्ष BRACE_LIMITS:
	    {
		जइ (OP(next) == BRACE_SIMPLE)
		{
		    bl_minval = OPERAND_MIN(scan);
		    bl_maxval = OPERAND_MAX(scan);
		}
		अण्णहा जइ (OP(next) >= BRACE_COMPLEX
			&& OP(next) < BRACE_COMPLEX + 10)
		{
		    no = OP(next) - BRACE_COMPLEX;
		    brace_min[no] = OPERAND_MIN(scan);
		    brace_max[no] = OPERAND_MAX(scan);
		    brace_गणनम्[no] = 0;
		}
		अण्णहा
		{
		    internal_error("BRACE_LIMITS");
		    status = RA_FAIL;
		}
	    }
	    भङ्ग;

	  पक्ष BRACE_COMPLEX + 0:
	  पक्ष BRACE_COMPLEX + 1:
	  पक्ष BRACE_COMPLEX + 2:
	  पक्ष BRACE_COMPLEX + 3:
	  पक्ष BRACE_COMPLEX + 4:
	  पक्ष BRACE_COMPLEX + 5:
	  पक्ष BRACE_COMPLEX + 6:
	  पक्ष BRACE_COMPLEX + 7:
	  पक्ष BRACE_COMPLEX + 8:
	  पक्ष BRACE_COMPLEX + 9:
	    {
		no = op - BRACE_COMPLEX;
		++brace_गणनम्[no];

		// If not matched enough times yet, try one more
		जइ (brace_गणनम्[no] <= (brace_min[no] <= brace_max[no]
					     ? brace_min[no] : brace_max[no]))
		{
		    rp = regstack_प्रक्षिप(RS_BRCPLX_MORE, scan);
		    जइ (rp == रिक्त)
			status = RA_FAIL;
		    अण्णहा
		    {
			rp->rs_no = no;
			reg_save(&rp->rs_un.regsave, &backpos);
			next = OPERAND(scan);
			// We continue and handle the result when done.
		    }
		    भङ्ग;
		}

		// If matched enough times, may try matching some more
		जइ (brace_min[no] <= brace_max[no])
		{
		    // Range is the normal way around, use longest match
		    जइ (brace_गणनम्[no] <= brace_max[no])
		    {
			rp = regstack_प्रक्षिप(RS_BRCPLX_LONG, scan);
			जइ (rp == रिक्त)
			    status = RA_FAIL;
			अण्णहा
			{
			    rp->rs_no = no;
			    reg_save(&rp->rs_un.regsave, &backpos);
			    next = OPERAND(scan);
			    // We continue and handle the result when done.
			}
		    }
		}
		अण्णहा
		{
		    // Range is backwards, use shortest match first
		    जइ (brace_गणनम्[no] <= brace_min[no])
		    {
			rp = regstack_प्रक्षिप(RS_BRCPLX_SHORT, scan);
			जइ (rp == रिक्त)
			    status = RA_FAIL;
			अण्णहा
			{
			    reg_save(&rp->rs_un.regsave, &backpos);
			    // We continue and handle the result when done.
			}
		    }
		}
	    }
	    भङ्ग;

	  पक्ष BRACE_SIMPLE:
	  पक्ष STAR:
	  पक्ष PLUS:
	    {
		regstar_T	rst;

		// Lookahead to avoid useless match attempts when we know
		// what character comes next.
		जइ (OP(next) == EXACTLY)
		{
		    rst.nextb = *OPERAND(next);
		    जइ (rex.reg_ic)
		    {
			जइ (MB_ISUPPER(rst.nextb))
			    rst.nextb_ic = MB_TOLOWER(rst.nextb);
			अण्णहा
			    rst.nextb_ic = MB_TOUPPER(rst.nextb);
		    }
		    अण्णहा
			rst.nextb_ic = rst.nextb;
		}
		अण्णहा
		{
		    rst.nextb = NUL;
		    rst.nextb_ic = NUL;
		}
		जइ (op != BRACE_SIMPLE)
		{
		    rst.minval = (op == STAR) ? 0 : 1;
		    rst.maxval = MAX_LIMIT;
		}
		अण्णहा
		{
		    rst.minval = bl_minval;
		    rst.maxval = bl_maxval;
		}

		// When maxval > minval, try matching as much as possible, up
		// to maxval.  When maxval < minval, try matching at least the
		// minimal number (since the range is backwards, that's also
		// maxval!).
		rst.गणनम् = regrepeat(OPERAND(scan), rst.maxval);
		जइ (got_int)
		{
		    status = RA_FAIL;
		    भङ्ग;
		}
		जइ (rst.minval <= rst.maxval
			  ? rst.गणनम् >= rst.minval : rst.गणनम् >= rst.maxval)
		{
		    // It could match.  Prepare for trying to match what
		    // follows.  The code is below.  Parameters are stored in
		    // a regstar_T on the regstack.
		    जइ ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
		    {
			emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
			status = RA_FAIL;
		    }
		    अण्णहा जइ (GA_GROW_FAILS(&regstack, (पूर्णाङ्क)परिमाण(regstar_T)))
			status = RA_FAIL;
		    अण्णहा
		    {
			regstack.ga_len += परिमाण(regstar_T);
			rp = regstack_प्रक्षिप(rst.minval <= rst.maxval
					? RS_STAR_LONG : RS_STAR_SHORT, scan);
			जइ (rp == रिक्त)
			    status = RA_FAIL;
			अण्णहा
			{
			    *(((regstar_T *)rp) - 1) = rst;
			    status = RA_BREAK;	    // skip the restore bits
			}
		    }
		}
		अण्णहा
		    status = RA_NOMATCH;

	    }
	    भङ्ग;

	  पक्ष NOMATCH:
	  पक्ष MATCH:
	  पक्ष SUBPAT:
	    rp = regstack_प्रक्षिप(RS_NOMATCH, scan);
	    जइ (rp == रिक्त)
		status = RA_FAIL;
	    अण्णहा
	    {
		rp->rs_no = op;
		reg_save(&rp->rs_un.regsave, &backpos);
		next = OPERAND(scan);
		// We continue and handle the result when done.
	    }
	    भङ्ग;

	  पक्ष BEHIND:
	  पक्ष NOBEHIND:
	    // Need a bit of room to store extra positions.
	    जइ ((long)((unsigned)regstack.ga_len >> 10) >= p_mmp)
	    {
		emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		status = RA_FAIL;
	    }
	    अण्णहा जइ (GA_GROW_FAILS(&regstack, (पूर्णाङ्क)परिमाण(regbehind_T)))
		status = RA_FAIL;
	    अण्णहा
	    {
		regstack.ga_len += परिमाण(regbehind_T);
		rp = regstack_प्रक्षिप(RS_BEHIND1, scan);
		जइ (rp == रिक्त)
		    status = RA_FAIL;
		अण्णहा
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
	    भङ्ग;

	  पक्ष BHPOS:
	    जइ (REG_MULTI)
	    {
		जइ (behind_pos.rs_u.pos.col != (colnr_T)(rex.निवेशः - rex.पङ्क्तिः)
			|| behind_pos.rs_u.pos.lnum != rex.lnum)
		    status = RA_NOMATCH;
	    }
	    अण्णहा जइ (behind_pos.rs_u.ptr != rex.निवेशः)
		status = RA_NOMATCH;
	    भङ्ग;

	  पक्ष NEWL:
	    जइ ((c != NUL || !REG_MULTI || rex.lnum > rex.reg_maxline
			     || rex.reg_पङ्क्तिः_lbr)
					   && (c != '\n' || !rex.reg_पङ्क्तिः_lbr))
		status = RA_NOMATCH;
	    अण्णहा जइ (rex.reg_पङ्क्तिः_lbr)
		ADVANCE_REGINPUT();
	    अण्णहा
		reg_nextline();
	    भङ्ग;

	  पक्ष END:
	    status = RA_MATCH;	// Success!
	    भङ्ग;

	  स्वभाव:
	    iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	    printf("Illegal op code %d\n", op);
#endif
	    status = RA_FAIL;
	    भङ्ग;
	  }
	}

	// If we can't continue sequentially, break the inner loop.
	जइ (status != RA_CONT)
	    भङ्ग;

	// Continue in inner loop, advance to next item.
	scan = next;

    } // end of inner loop

    // If there is something on the regstack execute the code for the state.
    // If the state is popped then loop and use the older state.
    जाव (regstack.ga_len > 0 && status != RA_FAIL)
    {
	rp = (regitem_T *)((वर्ण *)regstack.ga_data + regstack.ga_len) - 1;
	विकल्प (rp->rs_state)
	{
	  पक्ष RS_NOPEN:
	    // Result is passed on as-is, simply pop the state.
	    regstack_निष्कासय(&scan);
	    भङ्ग;

	  पक्ष RS_MOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    जइ (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_startpos[rp->rs_no],
						  &rex.reg_startp[rp->rs_no]);
	    regstack_निष्कासय(&scan);
	    भङ्ग;

#ifdef FEAT_SYN_HL
	  पक्ष RS_ZOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    जइ (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_startzpos[rp->rs_no],
						 &reg_startzp[rp->rs_no]);
	    regstack_निष्कासय(&scan);
	    भङ्ग;
#endif

	  पक्ष RS_MCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    जइ (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_endpos[rp->rs_no],
						    &rex.reg_endp[rp->rs_no]);
	    regstack_निष्कासय(&scan);
	    भङ्ग;

#ifdef FEAT_SYN_HL
	  पक्ष RS_ZCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    जइ (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_endzpos[rp->rs_no],
						   &reg_endzp[rp->rs_no]);
	    regstack_निष्कासय(&scan);
	    भङ्ग;
#endif

	  पक्ष RS_BRANCH:
	    जइ (status == RA_MATCH)
		// this branch matched, use it
		regstack_निष्कासय(&scan);
	    अण्णहा
	    {
		जइ (status != RA_BREAK)
		{
		    // After a non-matching branch: try next one.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    scan = rp->rs_scan;
		}
		जइ (scan == रिक्त || OP(scan) != BRANCH)
		{
		    // no more branches, didn't find a match
		    status = RA_NOMATCH;
		    regstack_निष्कासय(&scan);
		}
		अण्णहा
		{
		    // Prepare to try a branch.
		    rp->rs_scan = regnext(scan);
		    reg_save(&rp->rs_un.regsave, &backpos);
		    scan = OPERAND(scan);
		}
	    }
	    भङ्ग;

	  पक्ष RS_BRCPLX_MORE:
	    // Pop the state.  Restore pointers when there is no match.
	    जइ (status == RA_NOMATCH)
	    {
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_गणनम्[rp->rs_no];	// decrement match count
	    }
	    regstack_निष्कासय(&scan);
	    भङ्ग;

	  पक्ष RS_BRCPLX_LONG:
	    // Pop the state.  Restore pointers when there is no match.
	    जइ (status == RA_NOMATCH)
	    {
		// There was no match, but we did find enough matches.
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_गणनम्[rp->rs_no];
		// continue with the items after "\{}"
		status = RA_CONT;
	    }
	    regstack_निष्कासय(&scan);
	    जइ (status == RA_CONT)
		scan = regnext(scan);
	    भङ्ग;

	  पक्ष RS_BRCPLX_SHORT:
	    // Pop the state.  Restore pointers when there is no match.
	    जइ (status == RA_NOMATCH)
		// There was no match, try to match one more item.
		reg_restore(&rp->rs_un.regsave, &backpos);
	    regstack_निष्कासय(&scan);
	    जइ (status == RA_NOMATCH)
	    {
		scan = OPERAND(scan);
		status = RA_CONT;
	    }
	    भङ्ग;

	  पक्ष RS_NOMATCH:
	    // Pop the state.  If the operand matches for NOMATCH or
	    // doesn't match for MATCH/SUBPAT, we fail.  Otherwise backup,
	    // except for SUBPAT, and continue with the next item.
	    जइ (status == (rp->rs_no == NOMATCH ? RA_MATCH : RA_NOMATCH))
		status = RA_NOMATCH;
	    अण्णहा
	    {
		status = RA_CONT;
		जइ (rp->rs_no != SUBPAT)	// zero-width
		    reg_restore(&rp->rs_un.regsave, &backpos);
	    }
	    regstack_निष्कासय(&scan);
	    जइ (status == RA_CONT)
		scan = regnext(scan);
	    भङ्ग;

	  पक्ष RS_BEHIND1:
	    जइ (status == RA_NOMATCH)
	    {
		regstack_निष्कासय(&scan);
		regstack.ga_len -= परिमाण(regbehind_T);
	    }
	    अण्णहा
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
	    भङ्ग;

	  पक्ष RS_BEHIND2:
	    // Looping for BEHIND / NOBEHIND match.
	    जइ (status == RA_MATCH && reg_save_equal(&behind_pos))
	    {
		// found a match that ends where "next" started
		behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		जइ (rp->rs_no == BEHIND)
		    reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
		अण्णहा
		{
		    // But we didn't want a match.  Need to restore the
		    // subexpr, because what follows matched, so they have
		    // been set.
		    status = RA_NOMATCH;
		    restore_subexpr(((regbehind_T *)rp) - 1);
		}
		regstack_निष्कासय(&scan);
		regstack.ga_len -= परिमाण(regbehind_T);
	    }
	    अण्णहा
	    {
		long limit;

		// No match or a match that doesn't end where we want it: Go
		// back one character.  May go to previous line once.
		no = OK;
		limit = OPERAND_MIN(rp->rs_scan);
		जइ (REG_MULTI)
		{
		    जइ (limit > 0
			    && ((rp->rs_un.regsave.rs_u.pos.lnum
						    < behind_pos.rs_u.pos.lnum
				    ? (colnr_T)STRLEN(rex.पङ्क्तिः)
				    : behind_pos.rs_u.pos.col)
				- rp->rs_un.regsave.rs_u.pos.col >= limit))
			no = FAIL;
		    अण्णहा जइ (rp->rs_un.regsave.rs_u.pos.col == 0)
		    {
			जइ (rp->rs_un.regsave.rs_u.pos.lnum
					< behind_pos.rs_u.pos.lnum
				|| reg_getline(
					--rp->rs_un.regsave.rs_u.pos.lnum)
								  == रिक्त)
			    no = FAIL;
			अण्णहा
			{
			    reg_restore(&rp->rs_un.regsave, &backpos);
			    rp->rs_un.regsave.rs_u.pos.col =
						 (colnr_T)STRLEN(rex.पङ्क्तिः);
			}
		    }
		    अण्णहा
		    {
			जइ (has_mbyte)
			{
			    char_u *पङ्क्तिः =
				  reg_getline(rp->rs_un.regsave.rs_u.pos.lnum);

			    rp->rs_un.regsave.rs_u.pos.col -=
				(*mb_head_off)(पङ्क्तिः, पङ्क्तिः
				    + rp->rs_un.regsave.rs_u.pos.col - 1) + 1;
			}
			अण्णहा
			    --rp->rs_un.regsave.rs_u.pos.col;
		    }
		}
		अण्णहा
		{
		    जइ (rp->rs_un.regsave.rs_u.ptr == rex.पङ्क्तिः)
			no = FAIL;
		    अण्णहा
		    {
			MB_PTR_BACK(rex.पङ्क्तिः, rp->rs_un.regsave.rs_u.ptr);
			जइ (limit > 0 && (long)(behind_pos.rs_u.ptr
				     - rp->rs_un.regsave.rs_u.ptr) > limit)
			    no = FAIL;
		    }
		}
		जइ (no == OK)
		{
		    // Advanced, prepare for finding match again.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    scan = OPERAND(rp->rs_scan) + 4;
		    जइ (status == RA_MATCH)
		    {
			// We did match, so subexpr may have been changed,
			// need to restore them for the next try.
			status = RA_NOMATCH;
			restore_subexpr(((regbehind_T *)rp) - 1);
		    }
		}
		अण्णहा
		{
		    // Can't advance.  For NOBEHIND that's a match.
		    behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		    जइ (rp->rs_no == NOBEHIND)
		    {
			reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
			status = RA_MATCH;
		    }
		    अण्णहा
		    {
			// We do want a proper match.  Need to restore the
			// subexpr if we had a match, because they may have
			// been set.
			जइ (status == RA_MATCH)
			{
			    status = RA_NOMATCH;
			    restore_subexpr(((regbehind_T *)rp) - 1);
			}
		    }
		    regstack_निष्कासय(&scan);
		    regstack.ga_len -= परिमाण(regbehind_T);
		}
	    }
	    भङ्ग;

	  पक्ष RS_STAR_LONG:
	  पक्ष RS_STAR_SHORT:
	    {
		regstar_T	    *rst = ((regstar_T *)rp) - 1;

		जइ (status == RA_MATCH)
		{
		    regstack_निष्कासय(&scan);
		    regstack.ga_len -= परिमाण(regstar_T);
		    भङ्ग;
		}

		// Tried once already, restore input pointers.
		जइ (status != RA_BREAK)
		    reg_restore(&rp->rs_un.regsave, &backpos);

		// Repeat until we found a position where it could match.
		प्रति (;;)
		{
		    जइ (status != RA_BREAK)
		    {
			// Tried first position already, advance.
			जइ (rp->rs_state == RS_STAR_LONG)
			{
			    // Trying for longest match, but couldn't or
			    // didn't match -- back up one char.
			    जइ (--rst->गणनम् < rst->minval)
				भङ्ग;
			    जइ (rex.निवेशः == rex.पङ्क्तिः)
			    {
				// backup to last char of previous line
				जइ (rex.lnum == 0)
				{
				    status = RA_NOMATCH;
				    भङ्ग;
				}
				--rex.lnum;
				rex.पङ्क्तिः = reg_getline(rex.lnum);
				// Just in case regrepeat() didn't count
				// right.
				जइ (rex.पङ्क्तिः == रिक्त)
				    भङ्ग;
				rex.निवेशः = rex.पङ्क्तिः + reg_getline_len(rex.lnum);
				fast_breakcheck();
			    }
			    अण्णहा
				MB_PTR_BACK(rex.पङ्क्तिः, rex.निवेशः);
			}
			अण्णहा
			{
			    // Range is backwards, use shortest match first.
			    // Careful: maxval and minval are exchanged!
			    // Couldn't or didn't match: try advancing one
			    // char.
			    जइ (rst->गणनम् == rst->minval
				  || regrepeat(OPERAND(rp->rs_scan), 1L) == 0)
				भङ्ग;
			    ++rst->गणनम्;
			}
			जइ (got_int)
			    भङ्ग;
		    }
		    अण्णहा
			status = RA_NOMATCH;

		    // If it could match, try it.
		    जइ (rst->nextb == NUL || *rex.निवेशः == rst->nextb
					     || *rex.निवेशः == rst->nextb_ic)
		    {
			reg_save(&rp->rs_un.regsave, &backpos);
			scan = regnext(rp->rs_scan);
			status = RA_CONT;
			भङ्ग;
		    }
		}
		जइ (status != RA_CONT)
		{
		    // Failed.
		    regstack_निष्कासय(&scan);
		    regstack.ga_len -= परिमाण(regstar_T);
		    status = RA_NOMATCH;
		}
	    }
	    भङ्ग;
	}

	// If we want to continue the inner loop or didn't pop a state
	// continue matching loop
	जइ (status == RA_CONT || rp == (regitem_T *)
			     ((वर्ण *)regstack.ga_data + regstack.ga_len) - 1)
	    भङ्ग;

#ifdef FEAT_RELTIME
	जइ (bt_did_time_out(timed_out))
	{
	    status = RA_FAIL;
	    भङ्ग;
	}
#endif
    }

    // May need to continue with the inner loop, starting at "scan".
    जइ (status == RA_CONT)
	अनुवर्त;

    // If the regstack is empty or something failed we are done.
    जइ (regstack.ga_len == 0 || status == RA_FAIL)
    {
	जइ (scan == रिक्त)
	{
	    // We get here only if there's trouble -- normally "case END" is
	    // the terminating point.
	    iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	    printf("Premature EOL\n");
#endif
	}
	प्रत्यागम (status == RA_MATCH);
    }

  } // End of loop until the regstack is empty.

  // NOTREACHED
}

/*
 * regtry - try match of "prog" with at rex.line["col"].
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    स्थिर long
regtry(
    bt_regprog_T	*prog,
    colnr_T		col,
    पूर्णाङ्क			*timed_out)	// flag set on timeout or NULL
{
    rex.निवेशः = rex.पङ्क्तिः + col;
    rex.need_clear_subexpr = TRUE;
#ifdef FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    rex.need_clear_zsubexpr = (prog->reghasz == REX_SET);
#endif

    जइ (regmatch(prog->program + 1, timed_out) == 0)
	प्रत्यागम 0;

    cleanup_subexpr();
    जइ (REG_MULTI)
    {
	जइ (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].col = col;
	}
	जइ (rex.reg_endpos[0].lnum < 0)
	{
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].col = (पूर्णाङ्क)(rex.निवेशः - rex.पङ्क्तिः);
	}
	अण्णहा
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    अण्णहा
    {
	जइ (rex.reg_startp[0] == रिक्त)
	    rex.reg_startp[0] = rex.पङ्क्तिः + col;
	जइ (rex.reg_endp[0] == रिक्त)
	    rex.reg_endp[0] = rex.निवेशः;
    }
#ifdef FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = रिक्त;

    जइ (prog->reghasz == REX_SET)
    {
	पूर्णाङ्क		i;

	cleanup_zsubexpr();
	re_extmatch_out = make_extmatch();
	जइ (re_extmatch_out == रिक्त)
	    प्रत्यागम 0;
	प्रति (i = 0; i < NSUBEXP; i++)
	{
	    जइ (REG_MULTI)
	    {
		// Only accept single line matches.
		जइ (reg_startzpos[i].lnum >= 0
			&& reg_endzpos[i].lnum == reg_startzpos[i].lnum
			&& reg_endzpos[i].col >= reg_startzpos[i].col)
		    re_extmatch_out->matches[i] =
			vim_strnsave(reg_getline(reg_startzpos[i].lnum)
						       + reg_startzpos[i].col,
				   reg_endzpos[i].col - reg_startzpos[i].col);
	    }
	    अण्णहा
	    {
		जइ (reg_startzp[i] != रिक्त && reg_endzp[i] != रिक्त)
		    re_extmatch_out->matches[i] =
			    vim_strnsave(reg_startzp[i],
						reg_endzp[i] - reg_startzp[i]);
	    }
	}
    }
#endif
    प्रत्यागम 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    स्थिर long
bt_regexec_both(
    char_u	*पङ्क्तिः,
    colnr_T	startcol,	// column to start looking for match
    पूर्णाङ्क		*timed_out)	// flag set on timeout or NULL
{
    bt_regprog_T    *prog;
    char_u	    *s;
    colnr_T	    col = startcol;
    long	    retval = 0L;

    // Create "regstack" and "backpos" if they are not allocated yet.
    // We allocate *_INITIAL amount of bytes first and then set the grow size
    // to much bigger value to avoid many malloc calls in case of deep regular
    // expressions.
    जइ (regstack.ga_data == रिक्त)
    {
	// Use an item size of 1 byte, since we push different things
	// onto the regstack.
	ga_init2(&regstack, 1, REGSTACK_INITIAL);
	(शून्य)ga_grow(&regstack, REGSTACK_INITIAL);
	regstack.ga_growsize = REGSTACK_INITIAL * 8;
    }

    जइ (backpos.ga_data == रिक्त)
    {
	ga_init2(&backpos, परिमाण(backpos_T), BACKPOS_INITIAL);
	(शून्य)ga_grow(&backpos, BACKPOS_INITIAL);
	backpos.ga_growsize = BACKPOS_INITIAL * 8;
    }

    जइ (REG_MULTI)
    {
	prog = (bt_regprog_T *)rex.reg_mmatch->regprog;
	पङ्क्तिः = reg_getline((linenr_T)0);
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    अण्णहा
    {
	prog = (bt_regprog_T *)rex.reg_match->regprog;
	rex.reg_startp = rex.reg_match->startp;
	rex.reg_endp = rex.reg_match->endp;
    }

    // Be paranoid...
    जइ (prog == रिक्त || पङ्क्तिः == रिक्त)
    {
	iemsg(e_null_argument);
	गच्छ theend;
    }

    // Check validity of program.
    जइ (prog_magic_wrong())
	गच्छ theend;

    // If the start column is past the maximum column: no need to try.
    जइ (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	गच्छ theend;

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    जइ (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    अण्णहा जइ (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    जइ (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    // If there is a "must appear" string, look for it.
    जइ (prog->regmust != रिक्त)
    {
	पूर्णाङ्क c;

	जइ (has_mbyte)
	    c = (*mb_ptr2char)(prog->regmust);
	अण्णहा
	    c = *prog->regmust;
	s = पङ्क्तिः + col;

	// This is used very often, esp. for ":global".  Use three versions of
	// the loop to avoid overhead of conditions.
	जइ (!rex.reg_ic && !has_mbyte)
	    जाव ((s = vim_strbyte(s, c)) != रिक्त)
	    {
		पूर्णाङ्क len = prog->regmlen;
		जइ (cstrncmp(s, prog->regmust, &len) == 0)
		    भङ्ग;		// Found it.
		++s;
	    }
	अण्णहा जइ (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	    जाव ((s = vim_strchr(s, c)) != रिक्त)
	    {
		पूर्णाङ्क len = prog->regmlen;
		जइ (cstrncmp(s, prog->regmust, &len) == 0)
		    भङ्ग;		// Found it.
		MB_PTR_ADV(s);
	    }
	अण्णहा
	    जाव ((s = cstrchr(s, c)) != रिक्त)
	    {
		पूर्णाङ्क len = prog->regmlen;
		जइ (cstrncmp(s, prog->regmust, &len) == 0)
		    भङ्ग;		// Found it.
		MB_PTR_ADV(s);
	    }
	जइ (s == रिक्त)		// Not present.
	    गच्छ theend;
    }

    rex.पङ्क्तिः = पङ्क्तिः;
    rex.lnum = 0;
    reg_toolong = FALSE;

    // Simplest case: Anchored match need be tried only once.
    जइ (prog->reganch)
    {
	पूर्णाङ्क	c;

	जइ (has_mbyte)
	    c = (*mb_ptr2char)(rex.पङ्क्तिः + col);
	अण्णहा
	    c = rex.पङ्क्तिः[col];
	जइ (prog->regstart == NUL
		|| prog->regstart == c
		|| (rex.reg_ic
		    && (((enc_utf8 && utf_fold(prog->regstart) == utf_fold(c)))
			|| (c < 255 && prog->regstart < 255 &&
			    MB_TOLOWER(prog->regstart) == MB_TOLOWER(c)))))
	    retval = regtry(prog, col, timed_out);
	अण्णहा
	    retval = 0;
    }
    अण्णहा
    {
	// Messy cases:  unanchored match.
	जाव (!got_int)
	{
	    जइ (prog->regstart != NUL)
	    {
		// Skip until the char we know it must start with.
		// Used often, do some work to avoid call overhead.
		जइ (!rex.reg_ic && !has_mbyte)
		    s = vim_strbyte(rex.पङ्क्तिः + col, prog->regstart);
		अण्णहा
		    s = cstrchr(rex.पङ्क्तिः + col, prog->regstart);
		जइ (s == रिक्त)
		{
		    retval = 0;
		    भङ्ग;
		}
		col = (पूर्णाङ्क)(s - rex.पङ्क्तिः);
	    }

	    // Check for maximum column to try.
	    जइ (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	    {
		retval = 0;
		भङ्ग;
	    }

	    retval = regtry(prog, col, timed_out);
	    जइ (retval > 0)
		भङ्ग;

	    // if not currently on the first line, get it again
	    जइ (rex.lnum != 0)
	    {
		rex.lnum = 0;
		rex.पङ्क्तिः = reg_getline((linenr_T)0);
	    }
	    जइ (rex.पङ्क्तिः[col] == NUL)
		भङ्ग;
	    जइ (has_mbyte)
		col += (*mb_ptr2len)(rex.पङ्क्तिः + col);
	    अण्णहा
		++col;
#ifdef FEAT_RELTIME
	    जइ (bt_did_time_out(timed_out))
		भङ्ग;
#endif
	}
    }

theend:
    // Free "reg_tofree" when it's a bit big.
    // Free regstack and backpos if they are bigger than their initial size.
    जइ (reg_tofreelen > 400)
	VIM_CLEAR(reg_tofree);
    जइ (regstack.ga_maxlen > REGSTACK_INITIAL)
	ga_clear(&regstack);
    जइ (backpos.ga_maxlen > BACKPOS_INITIAL)
	ga_clear(&backpos);

    जइ (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs
	// and \ze are used.
	जइ (REG_MULTI)
	{
	    lpos_T *start = &rex.reg_mmatch->startpos[0];
	    lpos_T *end = &rex.reg_mmatch->endpos[0];

	    जइ (end->lnum < start->lnum
			|| (end->lnum == start->lnum && end->col < start->col))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_mmatch->rmm_matchcol = col;
	}
	अण्णहा
	{
	    जइ (rex.reg_match->endp[0] < rex.reg_match->startp[0])
		rex.reg_match->endp[0] = rex.reg_match->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_match->rm_matchcol = col;
	}
    }

    प्रत्यागम retval;
}

/*
 * Match a regexp against a string.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 * if "line_lbr" is TRUE  consider a "\n" in "line" to be a line break.
 *
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    स्थिर पूर्णाङ्क
bt_regexec_nl(
    regmatch_T	*rmp,
    char_u	*पङ्क्तिः,	// string to match against
    colnr_T	col,	// column to start looking for match
    पूर्णाङ्क		पङ्क्तिः_lbr)
{
    rex.reg_match = rmp;
    rex.reg_mmatch = रिक्त;
    rex.reg_maxline = 0;
    rex.reg_पङ्क्तिः_lbr = पङ्क्तिः_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = रिक्त;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;

    प्रत्यागम bt_regexec_both(पङ्क्तिः, col, रिक्त);
}

/*
 * Match a regexp against multiple lines.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 *
 * Return zero if there is no match.  Return number of lines contained in the
 * match otherwise.
 */
    स्थिर long
bt_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	col,		// column to start looking for match
    पूर्णाङ्क		*timed_out)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    प्रत्यागम bt_regexec_both(रिक्त, col, timed_out);
}

/*
 * Compare a number with the operand of RE_LNUM, RE_COL or RE_VCOL.
 */
    स्थिर पूर्णाङ्क
re_अङ्कः_cmp(long_u val, char_u *scan)
{
    long_u  n = OPERAND_MIN(scan);

    जइ (OPERAND_CMP(scan) == '>')
	प्रत्यागम val > n;
    जइ (OPERAND_CMP(scan) == '<')
	प्रत्यागम val < n;
    प्रत्यागम val == n;
}

#ifdef BT_REGEXP_DUMP

/*
 * regdump - dump a regexp onto stdout in vaguely comprehensible form
 */
    स्थिर शून्य
regdump(char_u *pattern, bt_regprog_T *r)
{
    char_u  *s;
    पूर्णाङ्क	    op = EXACTLY;	// Arbitrary non-END op.
    char_u  *next;
    char_u  *end = रिक्त;
    FILE    *f;

# ifdef BT_REGEXP_LOG
    f = fopen("bt_regexp_log.log", "a");
# अण्णहा
    f = stdout;
# endif
    जइ (f == रिक्त)
	प्रत्यागम;
    fprintf(f, "-------------------------------------\n\r\nregcomp(%s):\r\n", pattern);

    s = r->program + 1;
    // Loop until we find the END that isn't before a referred next (an END
    // can also appear in a NOMATCH operand).
    जाव ((op != END || s <= end) && s < r->program + r->regsz)
    {
	op = OP(s);
	fprintf(f, "%2d%s", (पूर्णाङ्क)(s - r->program), regprop(s)); // Where, what.
	next = (s + 3 <= r->program + r->regsz) ? regnext(s) : रिक्त;
	जइ (next == रिक्त)	// Next ptr.
	    fprintf(f, "(0)");
	अण्णहा
	    fprintf(f, "(%d)", (पूर्णाङ्क)((s - r->program) + (next - s)));
	जइ (end < next)
	    end = next;
	जइ (op == BRACE_LIMITS)
	{
	    // Two ints
	    fprintf(f, " minval %ld, maxval %ld", OPERAND_MIN(s), OPERAND_MAX(s));
	    s += 8;
	}
	अण्णहा जइ (op == BEHIND || op == NOBEHIND)
	{
	    // one int
	    fprintf(f, " count %ld", OPERAND_MIN(s));
	    s += 4;
	}
	अण्णहा जइ (op == RE_LNUM || op == RE_COL || op == RE_VCOL)
	{
	    // one int plus comparator
	    fprintf(f, " count %ld", OPERAND_MIN(s));
	    s += 5;
	}
	s += 3;
	जइ (op == MULTIBYTECODE)
	{
	    fprintf(f, " mbc=%d", utf_ptr2char(s));
	    s += utfc_ptr2len(s);
	}
	जइ (op == ANYOF || op == ANYOF + ADD_NL
		|| op == ANYBUT || op == ANYBUT + ADD_NL
		|| op == EXACTLY)
	{
	    // Literal string, where present.
	    fprintf(f, "\nxxxxxxxxx\n");
	    जाव (*s != NUL && s < r->program + r->regsz)
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
    जइ (r->regstart != NUL)
	fprintf(f, "start `%s' 0x%x; ", r->regstart < 256
		? (वर्ण *)transchar(r->regstart)
		: "multibyte", r->regstart);
    जइ (r->reganch)
	fprintf(f, "anchored; ");
    जइ (r->regmust != रिक्त)
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
    स्थिर char_u *
regprop(char_u *op)
{
    वर्ण	    *p;
    स्थिर वर्ण	    buf[50];
    स्थिर size_t   buflen = 0;

    STRCPY(buf, ":");
    buflen = 1;

    विकल्प ((पूर्णाङ्क) OP(op))
    {
      पक्ष BOL:
	p = "BOL";
	भङ्ग;
      पक्ष EOL:
	p = "EOL";
	भङ्ग;
      पक्ष RE_BOF:
	p = "BOF";
	भङ्ग;
      पक्ष RE_EOF:
	p = "EOF";
	भङ्ग;
      पक्ष CURSOR:
	p = "CURSOR";
	भङ्ग;
      पक्ष RE_VISUAL:
	p = "RE_VISUAL";
	भङ्ग;
      पक्ष RE_LNUM:
	p = "RE_LNUM";
	भङ्ग;
      पक्ष RE_MARK:
	p = "RE_MARK";
	भङ्ग;
      पक्ष RE_COL:
	p = "RE_COL";
	भङ्ग;
      पक्ष RE_VCOL:
	p = "RE_VCOL";
	भङ्ग;
      पक्ष BOW:
	p = "BOW";
	भङ्ग;
      पक्ष EOW:
	p = "EOW";
	भङ्ग;
      पक्ष ANY:
	p = "ANY";
	भङ्ग;
      पक्ष ANY + ADD_NL:
	p = "ANY+NL";
	भङ्ग;
      पक्ष ANYOF:
	p = "ANYOF";
	भङ्ग;
      पक्ष ANYOF + ADD_NL:
	p = "ANYOF+NL";
	भङ्ग;
      पक्ष ANYBUT:
	p = "ANYBUT";
	भङ्ग;
      पक्ष ANYBUT + ADD_NL:
	p = "ANYBUT+NL";
	भङ्ग;
      पक्ष IDENT:
	p = "IDENT";
	भङ्ग;
      पक्ष IDENT + ADD_NL:
	p = "IDENT+NL";
	भङ्ग;
      पक्ष SIDENT:
	p = "SIDENT";
	भङ्ग;
      पक्ष SIDENT + ADD_NL:
	p = "SIDENT+NL";
	भङ्ग;
      पक्ष KWORD:
	p = "KWORD";
	भङ्ग;
      पक्ष KWORD + ADD_NL:
	p = "KWORD+NL";
	भङ्ग;
      पक्ष SKWORD:
	p = "SKWORD";
	भङ्ग;
      पक्ष SKWORD + ADD_NL:
	p = "SKWORD+NL";
	भङ्ग;
      पक्ष FNAME:
	p = "FNAME";
	भङ्ग;
      पक्ष FNAME + ADD_NL:
	p = "FNAME+NL";
	भङ्ग;
      पक्ष SFNAME:
	p = "SFNAME";
	भङ्ग;
      पक्ष SFNAME + ADD_NL:
	p = "SFNAME+NL";
	भङ्ग;
      पक्ष PRINT:
	p = "PRINT";
	भङ्ग;
      पक्ष PRINT + ADD_NL:
	p = "PRINT+NL";
	भङ्ग;
      पक्ष SPRINT:
	p = "SPRINT";
	भङ्ग;
      पक्ष SPRINT + ADD_NL:
	p = "SPRINT+NL";
	भङ्ग;
      पक्ष WHITE:
	p = "WHITE";
	भङ्ग;
      पक्ष WHITE + ADD_NL:
	p = "WHITE+NL";
	भङ्ग;
      पक्ष NWHITE:
	p = "NWHITE";
	भङ्ग;
      पक्ष NWHITE + ADD_NL:
	p = "NWHITE+NL";
	भङ्ग;
      पक्ष DIGIT:
	p = "DIGIT";
	भङ्ग;
      पक्ष DIGIT + ADD_NL:
	p = "DIGIT+NL";
	भङ्ग;
      पक्ष NDIGIT:
	p = "NDIGIT";
	भङ्ग;
      पक्ष NDIGIT + ADD_NL:
	p = "NDIGIT+NL";
	भङ्ग;
      पक्ष HEX:
	p = "HEX";
	भङ्ग;
      पक्ष HEX + ADD_NL:
	p = "HEX+NL";
	भङ्ग;
      पक्ष NHEX:
	p = "NHEX";
	भङ्ग;
      पक्ष NHEX + ADD_NL:
	p = "NHEX+NL";
	भङ्ग;
      पक्ष OCTAL:
	p = "OCTAL";
	भङ्ग;
      पक्ष OCTAL + ADD_NL:
	p = "OCTAL+NL";
	भङ्ग;
      पक्ष NOCTAL:
	p = "NOCTAL";
	भङ्ग;
      पक्ष NOCTAL + ADD_NL:
	p = "NOCTAL+NL";
	भङ्ग;
      पक्ष WORD:
	p = "WORD";
	भङ्ग;
      पक्ष WORD + ADD_NL:
	p = "WORD+NL";
	भङ्ग;
      पक्ष NWORD:
	p = "NWORD";
	भङ्ग;
      पक्ष NWORD + ADD_NL:
	p = "NWORD+NL";
	भङ्ग;
      पक्ष HEAD:
	p = "HEAD";
	भङ्ग;
      पक्ष HEAD + ADD_NL:
	p = "HEAD+NL";
	भङ्ग;
      पक्ष NHEAD:
	p = "NHEAD";
	भङ्ग;
      पक्ष NHEAD + ADD_NL:
	p = "NHEAD+NL";
	भङ्ग;
      पक्ष ALPHA:
	p = "ALPHA";
	भङ्ग;
      पक्ष ALPHA + ADD_NL:
	p = "ALPHA+NL";
	भङ्ग;
      पक्ष NALPHA:
	p = "NALPHA";
	भङ्ग;
      पक्ष NALPHA + ADD_NL:
	p = "NALPHA+NL";
	भङ्ग;
      पक्ष LOWER:
	p = "LOWER";
	भङ्ग;
      पक्ष LOWER + ADD_NL:
	p = "LOWER+NL";
	भङ्ग;
      पक्ष NLOWER:
	p = "NLOWER";
	भङ्ग;
      पक्ष NLOWER + ADD_NL:
	p = "NLOWER+NL";
	भङ्ग;
      पक्ष UPPER:
	p = "UPPER";
	भङ्ग;
      पक्ष UPPER + ADD_NL:
	p = "UPPER+NL";
	भङ्ग;
      पक्ष NUPPER:
	p = "NUPPER";
	भङ्ग;
      पक्ष NUPPER + ADD_NL:
	p = "NUPPER+NL";
	भङ्ग;
      पक्ष BRANCH:
	p = "BRANCH";
	भङ्ग;
      पक्ष EXACTLY:
	p = "EXACTLY";
	भङ्ग;
      पक्ष NOTHING:
	p = "NOTHING";
	भङ्ग;
      पक्ष BACK:
	p = "BACK";
	भङ्ग;
      पक्ष END:
	p = "END";
	भङ्ग;
      पक्ष MOPEN + 0:
	p = "MATCH START";
	भङ्ग;
      पक्ष MOPEN + 1:
      पक्ष MOPEN + 2:
      पक्ष MOPEN + 3:
      पक्ष MOPEN + 4:
      पक्ष MOPEN + 5:
      पक्ष MOPEN + 6:
      पक्ष MOPEN + 7:
      पक्ष MOPEN + 8:
      पक्ष MOPEN + 9:
	buflen += vim_snprintf(buf + buflen, परिमाण(buf) - buflen, "MOPEN%d", OP(op) - MOPEN);
	p = रिक्त;
	भङ्ग;
      पक्ष MCLOSE + 0:
	p = "MATCH END";
	भङ्ग;
      पक्ष MCLOSE + 1:
      पक्ष MCLOSE + 2:
      पक्ष MCLOSE + 3:
      पक्ष MCLOSE + 4:
      पक्ष MCLOSE + 5:
      पक्ष MCLOSE + 6:
      पक्ष MCLOSE + 7:
      पक्ष MCLOSE + 8:
      पक्ष MCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, परिमाण(buf) - buflen, "MCLOSE%d", OP(op) - MCLOSE);
	p = रिक्त;
	भङ्ग;
      पक्ष BACKREF + 1:
      पक्ष BACKREF + 2:
      पक्ष BACKREF + 3:
      पक्ष BACKREF + 4:
      पक्ष BACKREF + 5:
      पक्ष BACKREF + 6:
      पक्ष BACKREF + 7:
      पक्ष BACKREF + 8:
      पक्ष BACKREF + 9:
	buflen += vim_snprintf(buf + buflen, परिमाण(buf) - buflen, "BACKREF%d", OP(op) - BACKREF);
	p = रिक्त;
	भङ्ग;
      पक्ष NOPEN:
	p = "NOPEN";
	भङ्ग;
      पक्ष NCLOSE:
	p = "NCLOSE";
	भङ्ग;
# ifdef FEAT_SYN_HL
      पक्ष ZOPEN + 1:
      पक्ष ZOPEN + 2:
      पक्ष ZOPEN + 3:
      पक्ष ZOPEN + 4:
      पक्ष ZOPEN + 5:
      पक्ष ZOPEN + 6:
      पक्ष ZOPEN + 7:
      पक्ष ZOPEN + 8:
      पक्ष ZOPEN + 9:
	buflen += vim_snprintf(buf + buflen, परिमाण(buf) - buflen, "ZOPEN%d", OP(op) - ZOPEN);
	p = रिक्त;
	भङ्ग;
      पक्ष ZCLOSE + 1:
      पक्ष ZCLOSE + 2:
      पक्ष ZCLOSE + 3:
      पक्ष ZCLOSE + 4:
      पक्ष ZCLOSE + 5:
      पक्ष ZCLOSE + 6:
      पक्ष ZCLOSE + 7:
      पक्ष ZCLOSE + 8:
      पक्ष ZCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, परिमाण(buf) - buflen, "ZCLOSE%d", OP(op) - ZCLOSE);
	p = रिक्त;
	भङ्ग;
      पक्ष ZREF + 1:
      पक्ष ZREF + 2:
      पक्ष ZREF + 3:
      पक्ष ZREF + 4:
      पक्ष ZREF + 5:
      पक्ष ZREF + 6:
      पक्ष ZREF + 7:
      पक्ष ZREF + 8:
      पक्ष ZREF + 9:
	buflen += vim_snprintf(buf + buflen, परिमाण(buf) - buflen, "ZREF%d", OP(op) - ZREF);
	p = रिक्त;
	भङ्ग;
# endif
      पक्ष STAR:
	p = "STAR";
	भङ्ग;
      पक्ष PLUS:
	p = "PLUS";
	भङ्ग;
      पक्ष NOMATCH:
	p = "NOMATCH";
	भङ्ग;
      पक्ष MATCH:
	p = "MATCH";
	भङ्ग;
      पक्ष BEHIND:
	p = "BEHIND";
	भङ्ग;
      पक्ष NOBEHIND:
	p = "NOBEHIND";
	भङ्ग;
      पक्ष SUBPAT:
	p = "SUBPAT";
	भङ्ग;
      पक्ष BRACE_LIMITS:
	p = "BRACE_LIMITS";
	भङ्ग;
      पक्ष BRACE_SIMPLE:
	p = "BRACE_SIMPLE";
	भङ्ग;
      पक्ष BRACE_COMPLEX + 0:
      पक्ष BRACE_COMPLEX + 1:
      पक्ष BRACE_COMPLEX + 2:
      पक्ष BRACE_COMPLEX + 3:
      पक्ष BRACE_COMPLEX + 4:
      पक्ष BRACE_COMPLEX + 5:
      पक्ष BRACE_COMPLEX + 6:
      पक्ष BRACE_COMPLEX + 7:
      पक्ष BRACE_COMPLEX + 8:
      पक्ष BRACE_COMPLEX + 9:
	buflen += vim_snprintf(buf + buflen, परिमाण(buf) - buflen, "BRACE_COMPLEX%d", OP(op) - BRACE_COMPLEX);
	p = रिक्त;
	भङ्ग;
      पक्ष MULTIBYTECODE:
	p = "MULTIBYTECODE";
	भङ्ग;
      पक्ष NEWL:
	p = "NEWL";
	भङ्ग;
      स्वभाव:
	buflen += vim_snprintf(buf + buflen, परिमाण(buf) - buflen, "corrupt %d", OP(op));
	p = रिक्त;
	भङ्ग;
    }
    जइ (p != रिक्त)
	vim_strncpy((char_u *)buf + buflen, (char_u *)p, परिमाण(buf) - buflen - 1);
    प्रत्यागम (char_u *)buf;
}
#endif	    // DEBUG
