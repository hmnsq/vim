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

स्थिर पूर्णांक	अंक_complex_braces; // Complex \{...} count
स्थिर char_u	*regcode;	// Code-emit pointer, or JUST_CALC_SIZE
स्थिर दीर्घ	regsize;	// Code size.
स्थिर पूर्णांक	reg_toolong;	// TRUE when offset out of range
स्थिर पूर्णांक	bt_reg_parse_depth;	// nesting depth in reg()
स्थिर char_u	had_endbrace[NSUBEXP];	// flags, TRUE if end of () found
स्थिर दीर्घ	brace_min[10];	// Minimums for complex brace repeats
स्थिर दीर्घ	brace_max[10];	// Maximums for complex brace repeats
स्थिर पूर्णांक	brace_मोजणी[10]; // Current counts for complex brace repeats
स्थिर पूर्णांक	one_exactly = FALSE;	// only do one char for EXACTLY

// When making changes to classchars also change nfa_classcodes.
स्थिर char_u	*classchars = (char_u *)".iIkKfFpPsSdDxXoOwWhHaAlLuU";
स्थिर स्थिरांक पूर्णांक classcodes[] = {
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
    स्थिर पूर्णांक
bt_recognize_char_class(char_u *start, char_u *end)
{
    पूर्णांक		newl;
    पूर्णांक		config = get_char_class_bits(start, end, &newl);

    // A newline would need the ADD_NL variant, don't bother with it here.
    जर (config < 0 || newl)
	परत 0;

	// The old engine has no case-insensitive class opcode, so [a-z] and [A-Z]
	// are left as collections.  The classes below are case-independent.
    निवड (config)
    {
	स्थिती CLASS_o9:
	    परत DIGIT;
	स्थिती CLASS_not | CLASS_o9:
	    परत NDIGIT;
	स्थिती CLASS_af | CLASS_AF | CLASS_o9:
	    परत HEX;
	स्थिती CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    परत NHEX;
	स्थिती CLASS_o7:
	    परत OCTAL;
	स्थिती CLASS_not | CLASS_o7:
	    परत NOCTAL;
	स्थिती CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    परत WORD;
	स्थिती CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    परत NWORD;
	स्थिती CLASS_az | CLASS_AZ | CLASS_underscore:
	    परत HEAD;
	स्थिती CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    परत NHEAD;
	स्थिती CLASS_az | CLASS_AZ:
	    परत ALPHA;
	स्थिती CLASS_not | CLASS_az | CLASS_AZ:
	    परत NALPHA;
    }
    परत 0;
}

/*
 * When regcode is set to this value, code is not emitted and size is computed
 * instead.
 */
#define JUST_CALC_SIZE	((char_u *) -1)

// Values for rs_state in regitem_T.
प्रकारव्याख्या गणना regstate_E
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
प्रकारव्याख्या संरचना
{
    संघ
    {
	char_u	*ptr;	// rex.input pointer, for single-line regexp
	lpos_T	pos;	// rex.input pos, for multi-line regexp
    } rs_u;
    पूर्णांक		rs_len;
} regsave_T;

// struct to save start/end pointer/position in for \(\)
प्रकारव्याख्या संरचना
{
    संघ
    {
	char_u	*ptr;
	lpos_T	pos;
    } se_u;
} save_se_T;

// used for BEHIND and NOBEHIND matching
प्रकारव्याख्या संरचना regbehind_S
{
    regsave_T	save_after;
    regsave_T	save_behind;
    पूर्णांक		save_need_clear_subexpr;
    save_se_T   save_start[NSUBEXP];
    save_se_T   save_end[NSUBEXP];
} regbehind_T;

/*
 * When there are alternatives a regstate_T is put on the regstack to remember
 * what we are doing.
 * Before it may be another type of item, depending on rs_state, to remember
 * more things.
 */
प्रकारव्याख्या संरचना regitem_S
{
    regstate_T	rs_state;	// what we are doing, one of RS_ above
    लघु	rs_no;		// submatch nr or BEHIND/NOBEHIND
    char_u	*rs_scan;	// current node in program
    संघ
    {
	save_se_T  sesave;
	regsave_T  regsave;
    } rs_un;			// room for saving rex.input
} regitem_T;


// used for STAR, PLUS and BRACE_SIMPLE matching
प्रकारव्याख्या संरचना regstar_S
{
    पूर्णांक		nextb;		// next byte
    पूर्णांक		nextb_ic;	// next byte reverse case
    दीर्घ	मोजणी;
    दीर्घ	minval;
    दीर्घ	maxval;
} regstar_T;

// used to store input position when a BACK was encountered, so that we now if
// we made any progress since the last time.
प्रकारव्याख्या संरचना backpos_S
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
#define OP(p)		((पूर्णांक)*(p))
#define NEXT(p)		(((*((p) + 1) & 0377) << 8) + (*((p) + 2) & 0377))
#define OPERAND(p)	((p) + 3)
// Obtain an operand that was stored as four bytes, MSB first.
#define OPERAND_MIN(p)	(((दीर्घ)(p)[3] << 24) + ((दीर्घ)(p)[4] << 16) \
			+ ((दीर्घ)(p)[5] << 8) + (दीर्घ)(p)[6])
// Obtain a second operand stored as four bytes.
#define OPERAND_MAX(p)	OPERAND_MIN((p) + 4)
// Obtain a second single-byte operand stored after a four bytes operand.
#define OPERAND_CMP(p)	(p)[7]

स्थिर char_u *reg(पूर्णांक paren, पूर्णांक *flagp);

#ifdef BT_REGEXP_DUMP
स्थिर शून्य	regdump(char_u *, bt_regprog_T *);
#endif

स्थिर पूर्णांक	re_अंक_cmp(long_u val, char_u *scan);

#ifdef DEBUG
स्थिर char_u	*regprop(char_u *);

स्थिर पूर्णांक	regnarrate = 0;
#endif


/*
 * Setup to parse the regexp.  Used once to get the length and once to do it.
 */
    स्थिर शून्य
regcomp_start(
    char_u	*अभिव्यक्ती,
    पूर्णांक		re_flags)	    // see vim_regcomp()
{
    initchr(अभिव्यक्ती);
    जर (re_flags & RE_MAGIC)
	reg_magic = MAGIC_ON;
    अन्यथा
	reg_magic = MAGIC_OFF;
    reg_string = (re_flags & RE_STRING);
    reg_strict = (re_flags & RE_STRICT);
    get_cpo_flags();

    अंक_complex_braces = 0;
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
#जर defined(FEAT_SYN_HL)
    had_eol = FALSE;
#endif
}

/*
 * Return TRUE if MULTIBYTECODE should be used instead of EXACTLY for
 * character "c".
 */
    स्थिर पूर्णांक
use_multibytecode(पूर्णांक c)
{
    परत has_mbyte && (*mb_char2len)(c) > 1
		     && (re_multi_type(peekchr()) != NOT_MULTI
			     || (enc_utf8 && utf_iscomposing(c)));
}

/*
 * Emit (if appropriate) a byte of code
 */
    स्थिर शून्य
regc(पूर्णांक b)
{
    जर (regcode == JUST_CALC_SIZE)
	regsize++;
    अन्यथा
	*regcode++ = b;
}

/*
 * Emit (if appropriate) a multi-byte character of code
 */
    स्थिर शून्य
regmbc(पूर्णांक c)
{
    जर (!has_mbyte && c > 0xff)
	परत;
    जर (regcode == JUST_CALC_SIZE)
	regsize += (*mb_char2len)(c);
    अन्यथा
	regcode += (*mb_char2bytes)(c, regcode);
}


/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * NOTE: When changing this function, also change nfa_emit_equi_class()
 */
    स्थिर शून्य
reg_equi_class(पूर्णांक c)
{
    जर (enc_utf8 || STRCMP(p_enc, "latin1") == 0
					 || STRCMP(p_enc, "iso-8859-15") == 0)
    {
	निवड (c)
	{
	    // Do not use '\300' style, it results in a negative number.
	    स्थिती 'A': स्थिती 0xc0: स्थिती 0xc1: स्थिती 0xc2: स्थिती 0xc3: स्थिती 0xc4:
	    स्थिती 0xc5: स्थिती 0x100: स्थिती 0x102: स्थिती 0x104: स्थिती 0x1cd:
	    स्थिती 0x1de: स्थिती 0x1e0: स्थिती 0x1fa: स्थिती 0x202: स्थिती 0x226:
	    स्थिती 0x23a: स्थिती 0x1e00: स्थिती 0x1ea0: स्थिती 0x1ea2: स्थिती 0x1ea4:
	    स्थिती 0x1ea6: स्थिती 0x1ea8: स्थिती 0x1eaa: स्थिती 0x1eac: स्थिती 0x1eae:
	    स्थिती 0x1eb0: स्थिती 0x1eb2: स्थिती 0x1eb4: स्थिती 0x1eb6:
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
		      परत;
	    स्थिती 'B': स्थिती 0x181: स्थिती 0x243: स्थिती 0x1e02:
	    स्थिती 0x1e04: स्थिती 0x1e06:
		      regmbc('B');
		      regmbc(0x181); regmbc(0x243); regmbc(0x1e02);
		      regmbc(0x1e04); regmbc(0x1e06);
		      परत;
	    स्थिती 'C': स्थिती 0xc7:
	    स्थिती 0x106: स्थिती 0x108: स्थिती 0x10a: स्थिती 0x10c: स्थिती 0x187:
	    स्थिती 0x23b: स्थिती 0x1e08: स्थिती 0xa792:
		      regmbc('C'); regmbc(0xc7);
		      regmbc(0x106); regmbc(0x108); regmbc(0x10a);
		      regmbc(0x10c); regmbc(0x187); regmbc(0x23b);
		      regmbc(0x1e08); regmbc(0xa792);
		      परत;
	    स्थिती 'D': स्थिती 0x10e: स्थिती 0x110: स्थिती 0x18a:
	    स्थिती 0x1e0a: स्थिती 0x1e0c: स्थिती 0x1e0e: स्थिती 0x1e10:
	    स्थिती 0x1e12:
		      regmbc('D'); regmbc(0x10e); regmbc(0x110);
		      regmbc(0x18a); regmbc(0x1e0a); regmbc(0x1e0c);
		      regmbc(0x1e0e); regmbc(0x1e10); regmbc(0x1e12);
		      परत;
	    स्थिती 'E': स्थिती 0xc8: स्थिती 0xc9: स्थिती 0xca: स्थिती 0xcb:
	    स्थिती 0x112: स्थिती 0x114: स्थिती 0x116: स्थिती 0x118: स्थिती 0x11a:
	    स्थिती 0x204: स्थिती 0x206: स्थिती 0x228: स्थिती 0x246: स्थिती 0x1e14:
	    स्थिती 0x1e16: स्थिती 0x1e18: स्थिती 0x1e1a: स्थिती 0x1e1c:
	    स्थिती 0x1eb8: स्थिती 0x1eba: स्थिती 0x1ebc: स्थिती 0x1ebe:
	    स्थिती 0x1ec0: स्थिती 0x1ec2: स्थिती 0x1ec4: स्थिती 0x1ec6:
		      regmbc('E'); regmbc(0xc8); regmbc(0xc9);
		      regmbc(0xca); regmbc(0xcb); regmbc(0x112);
		      regmbc(0x114); regmbc(0x116); regmbc(0x118);
		      regmbc(0x11a); regmbc(0x204); regmbc(0x206);
		      regmbc(0x228); regmbc(0x246); regmbc(0x1e14);
		      regmbc(0x1e16); regmbc(0x1e18); regmbc(0x1e1a);
		      regmbc(0x1e1c); regmbc(0x1eb8); regmbc(0x1eba);
		      regmbc(0x1ebc); regmbc(0x1ebe); regmbc(0x1ec0);
		      regmbc(0x1ec2); regmbc(0x1ec4); regmbc(0x1ec6);
		      परत;
	    स्थिती 'F': स्थिती 0x191: स्थिती 0x1e1e: स्थिती 0xa798:
		      regmbc('F'); regmbc(0x191); regmbc(0x1e1e);
		      regmbc(0xa798);
		      परत;
	    स्थिती 'G': स्थिती 0x11c: स्थिती 0x11e: स्थिती 0x120:
	    स्थिती 0x122: स्थिती 0x193: स्थिती 0x1e4: स्थिती 0x1e6:
	    स्थिती 0x1f4: स्थिती 0x1e20: स्थिती 0xa7a0:
		      regmbc('G'); regmbc(0x11c); regmbc(0x11e);
		      regmbc(0x120); regmbc(0x122); regmbc(0x193);
		      regmbc(0x1e4); regmbc(0x1e6); regmbc(0x1f4);
		      regmbc(0x1e20); regmbc(0xa7a0);
		      परत;
	    स्थिती 'H': स्थिती 0x124: स्थिती 0x126: स्थिती 0x21e:
	    स्थिती 0x1e22: स्थिती 0x1e24: स्थिती 0x1e26:
	    स्थिती 0x1e28: स्थिती 0x1e2a: स्थिती 0x2c67:
		      regmbc('H'); regmbc(0x124); regmbc(0x126);
		      regmbc(0x21e); regmbc(0x1e22); regmbc(0x1e24);
		      regmbc(0x1e26); regmbc(0x1e28); regmbc(0x1e2a);
		      regmbc(0x2c67);
		      परत;
	    स्थिती 'I': स्थिती 0xcc: स्थिती 0xcd: स्थिती 0xce: स्थिती 0xcf:
	    स्थिती 0x128: स्थिती 0x12a: स्थिती 0x12c: स्थिती 0x12e:
	    स्थिती 0x130: स्थिती 0x197: स्थिती 0x1cf: स्थिती 0x208:
	    स्थिती 0x20a: स्थिती 0x1e2c: स्थिती 0x1e2e: स्थिती 0x1ec8:
	    स्थिती 0x1eca:
		      regmbc('I'); regmbc(0xcc); regmbc(0xcd);
		      regmbc(0xce); regmbc(0xcf); regmbc(0x128);
		      regmbc(0x12a); regmbc(0x12c); regmbc(0x12e);
		      regmbc(0x130); regmbc(0x197); regmbc(0x1cf);
		      regmbc(0x208); regmbc(0x20a); regmbc(0x1e2c);
		      regmbc(0x1e2e); regmbc(0x1ec8); regmbc(0x1eca);
		      परत;
	    स्थिती 'J': स्थिती 0x134: स्थिती 0x248:
		      regmbc('J'); regmbc(0x134); regmbc(0x248);
		      परत;
	    स्थिती 'K': स्थिती 0x136: स्थिती 0x198: स्थिती 0x1e8: स्थिती 0x1e30:
	    स्थिती 0x1e32: स्थिती 0x1e34: स्थिती 0x2c69: स्थिती 0xa740:
		      regmbc('K'); regmbc(0x136); regmbc(0x198);
		      regmbc(0x1e8); regmbc(0x1e30); regmbc(0x1e32);
		      regmbc(0x1e34); regmbc(0x2c69); regmbc(0xa740);
		      परत;
	    स्थिती 'L': स्थिती 0x139: स्थिती 0x13b: स्थिती 0x13d: स्थिती 0x13f:
	    स्थिती 0x141: स्थिती 0x23d: स्थिती 0x1e36: स्थिती 0x1e38:
	    स्थिती 0x1e3a: स्थिती 0x1e3c: स्थिती 0x2c60:
		      regmbc('L'); regmbc(0x139); regmbc(0x13b);
		      regmbc(0x13d); regmbc(0x13f); regmbc(0x141);
		      regmbc(0x23d); regmbc(0x1e36); regmbc(0x1e38);
		      regmbc(0x1e3a); regmbc(0x1e3c); regmbc(0x2c60);
		      परत;
	    स्थिती 'M': स्थिती 0x1e3e: स्थिती 0x1e40: स्थिती 0x1e42:
		      regmbc('M'); regmbc(0x1e3e); regmbc(0x1e40);
		      regmbc(0x1e42);
		      परत;
	    स्थिती 'N': स्थिती 0xd1:
	    स्थिती 0x143: स्थिती 0x145: स्थिती 0x147: स्थिती 0x1f8:
	    स्थिती 0x1e44: स्थिती 0x1e46: स्थिती 0x1e48: स्थिती 0x1e4a:
	    स्थिती 0xa7a4:
		      regmbc('N'); regmbc(0xd1);
		      regmbc(0x143); regmbc(0x145); regmbc(0x147);
		      regmbc(0x1f8); regmbc(0x1e44); regmbc(0x1e46);
		      regmbc(0x1e48); regmbc(0x1e4a); regmbc(0xa7a4);
		      परत;
	    स्थिती 'O': स्थिती 0xd2: स्थिती 0xd3: स्थिती 0xd4: स्थिती 0xd5: स्थिती 0xd6:
	    स्थिती 0xd8: स्थिती 0x14c: स्थिती 0x14e: स्थिती 0x150: स्थिती 0x19f:
	    स्थिती 0x1a0: स्थिती 0x1d1: स्थिती 0x1ea: स्थिती 0x1ec: स्थिती 0x1fe:
	    स्थिती 0x20c: स्थिती 0x20e: स्थिती 0x22a: स्थिती 0x22c: स्थिती 0x22e:
	    स्थिती 0x230: स्थिती 0x1e4c: स्थिती 0x1e4e: स्थिती 0x1e50: स्थिती 0x1e52:
	    स्थिती 0x1ecc: स्थिती 0x1ece: स्थिती 0x1ed0: स्थिती 0x1ed2: स्थिती 0x1ed4:
	    स्थिती 0x1ed6: स्थिती 0x1ed8: स्थिती 0x1eda: स्थिती 0x1edc: स्थिती 0x1ede:
	    स्थिती 0x1ee0: स्थिती 0x1ee2:
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
		      परत;
	    स्थिती 'P': स्थिती 0x1a4: स्थिती 0x1e54: स्थिती 0x1e56: स्थिती 0x2c63:
		      regmbc('P'); regmbc(0x1a4); regmbc(0x1e54);
		      regmbc(0x1e56); regmbc(0x2c63);
		      परत;
	    स्थिती 'Q': स्थिती 0x24a:
		      regmbc('Q'); regmbc(0x24a);
		      परत;
	    स्थिती 'R': स्थिती 0x154: स्थिती 0x156: स्थिती 0x158: स्थिती 0x210:
	    स्थिती 0x212: स्थिती 0x24c: स्थिती 0x1e58: स्थिती 0x1e5a:
	    स्थिती 0x1e5c: स्थिती 0x1e5e: स्थिती 0x2c64: स्थिती 0xa7a6:
		      regmbc('R'); regmbc(0x154); regmbc(0x156);
		      regmbc(0x210); regmbc(0x212); regmbc(0x158);
		      regmbc(0x24c); regmbc(0x1e58); regmbc(0x1e5a);
		      regmbc(0x1e5c); regmbc(0x1e5e); regmbc(0x2c64);
		      regmbc(0xa7a6);
		      परत;
	    स्थिती 'S': स्थिती 0x15a: स्थिती 0x15c: स्थिती 0x15e: स्थिती 0x160:
	    स्थिती 0x218: स्थिती 0x1e60: स्थिती 0x1e62: स्थिती 0x1e64:
	    स्थिती 0x1e66: स्थिती 0x1e68: स्थिती 0x2c7e: स्थिती 0xa7a8:
		      regmbc('S'); regmbc(0x15a); regmbc(0x15c);
		      regmbc(0x15e); regmbc(0x160); regmbc(0x218);
		      regmbc(0x1e60); regmbc(0x1e62); regmbc(0x1e64);
		      regmbc(0x1e66); regmbc(0x1e68); regmbc(0x2c7e);
			  regmbc(0xa7a8);
		      परत;
	    स्थिती 'T': स्थिती 0x162: स्थिती 0x164: स्थिती 0x166: स्थिती 0x1ac:
	    स्थिती 0x1ae: स्थिती 0x21a: स्थिती 0x23e: स्थिती 0x1e6a: स्थिती 0x1e6c:
	    स्थिती 0x1e6e: स्थिती 0x1e70:
		      regmbc('T'); regmbc(0x162); regmbc(0x164);
		      regmbc(0x166); regmbc(0x1ac); regmbc(0x23e);
		      regmbc(0x1ae); regmbc(0x21a); regmbc(0x1e6a);
		      regmbc(0x1e6c); regmbc(0x1e6e); regmbc(0x1e70);
		      परत;
	    स्थिती 'U': स्थिती 0xd9: स्थिती 0xda: स्थिती 0xdb: स्थिती 0xdc:
	    स्थिती 0x168: स्थिती 0x16a: स्थिती 0x16c: स्थिती 0x16e:
	    स्थिती 0x170: स्थिती 0x172: स्थिती 0x1af: स्थिती 0x1d3:
	    स्थिती 0x1d5: स्थिती 0x1d7: स्थिती 0x1d9: स्थिती 0x1db:
	    स्थिती 0x214: स्थिती 0x216: स्थिती 0x244: स्थिती 0x1e72:
	    स्थिती 0x1e74: स्थिती 0x1e76: स्थिती 0x1e78: स्थिती 0x1e7a:
	    स्थिती 0x1ee4: स्थिती 0x1ee6: स्थिती 0x1ee8: स्थिती 0x1eea:
	    स्थिती 0x1eec: स्थिती 0x1eee: स्थिती 0x1ef0:
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
		      परत;
	    स्थिती 'V': स्थिती 0x1b2: स्थिती 0x1e7c: स्थिती 0x1e7e:
		      regmbc('V'); regmbc(0x1b2); regmbc(0x1e7c);
		      regmbc(0x1e7e);
		      परत;
	    स्थिती 'W': स्थिती 0x174: स्थिती 0x1e80: स्थिती 0x1e82:
	    स्थिती 0x1e84: स्थिती 0x1e86: स्थिती 0x1e88:
		      regmbc('W'); regmbc(0x174); regmbc(0x1e80);
		      regmbc(0x1e82); regmbc(0x1e84); regmbc(0x1e86);
		      regmbc(0x1e88);
		      परत;
	    स्थिती 'X': स्थिती 0x1e8a: स्थिती 0x1e8c:
		      regmbc('X'); regmbc(0x1e8a); regmbc(0x1e8c);
		      परत;
	    स्थिती 'Y': स्थिती 0xdd:
	    स्थिती 0x176: स्थिती 0x178: स्थिती 0x1b3: स्थिती 0x232: स्थिती 0x24e:
	    स्थिती 0x1e8e: स्थिती 0x1ef2: स्थिती 0x1ef6: स्थिती 0x1ef4: स्थिती 0x1ef8:
		      regmbc('Y'); regmbc(0xdd); regmbc(0x176);
		      regmbc(0x178); regmbc(0x1b3); regmbc(0x232);
		      regmbc(0x24e); regmbc(0x1e8e); regmbc(0x1ef2);
		      regmbc(0x1ef4); regmbc(0x1ef6); regmbc(0x1ef8);
		      परत;
	    स्थिती 'Z': स्थिती 0x179: स्थिती 0x17b: स्थिती 0x17d: स्थिती 0x1b5:
	    स्थिती 0x1e90: स्थिती 0x1e92: स्थिती 0x1e94: स्थिती 0x2c6b:
		      regmbc('Z'); regmbc(0x179); regmbc(0x17b);
		      regmbc(0x17d); regmbc(0x1b5); regmbc(0x1e90);
		      regmbc(0x1e92); regmbc(0x1e94); regmbc(0x2c6b);
		      परत;
	    स्थिती 'a': स्थिती 0xe0: स्थिती 0xe1: स्थिती 0xe2:
	    स्थिती 0xe3: स्थिती 0xe4: स्थिती 0xe5: स्थिती 0x101: स्थिती 0x103:
	    स्थिती 0x105: स्थिती 0x1ce: स्थिती 0x1df: स्थिती 0x1e1: स्थिती 0x1fb:
	    स्थिती 0x201: स्थिती 0x203: स्थिती 0x227: स्थिती 0x1d8f: स्थिती 0x1e01:
	    स्थिती 0x1e9a: स्थिती 0x1ea1: स्थिती 0x1ea3: स्थिती 0x1ea5:
	    स्थिती 0x1ea7: स्थिती 0x1ea9: स्थिती 0x1eab: स्थिती 0x1ead:
	    स्थिती 0x1eaf: स्थिती 0x1eb1: स्थिती 0x1eb3: स्थिती 0x1eb5:
	    स्थिती 0x1eb7: स्थिती 0x2c65:
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
		      परत;
	    स्थिती 'b': स्थिती 0x180: स्थिती 0x253: स्थिती 0x1d6c: स्थिती 0x1d80:
	    स्थिती 0x1e03: स्थिती 0x1e05: स्थिती 0x1e07:
		      regmbc('b');
		      regmbc(0x180); regmbc(0x253); regmbc(0x1d6c);
		      regmbc(0x1d80); regmbc(0x1e03); regmbc(0x1e05);
		      regmbc(0x1e07);
		      परत;
	    स्थिती 'c': स्थिती 0xe7:
	    स्थिती 0x107: स्थिती 0x109: स्थिती 0x10b: स्थिती 0x10d: स्थिती 0x188:
	    स्थिती 0x23c: स्थिती 0x1e09: स्थिती 0xa793: स्थिती 0xa794:
		      regmbc('c'); regmbc(0xe7); regmbc(0x107);
		      regmbc(0x109); regmbc(0x10b); regmbc(0x10d);
		      regmbc(0x188); regmbc(0x23c); regmbc(0x1e09);
		      regmbc(0xa793); regmbc(0xa794);
		      परत;
	    स्थिती 'd': स्थिती 0x10f: स्थिती 0x111: स्थिती 0x257: स्थिती 0x1d6d:
	    स्थिती 0x1d81: स्थिती 0x1d91: स्थिती 0x1e0b: स्थिती 0x1e0d:
	    स्थिती 0x1e0f: स्थिती 0x1e11: स्थिती 0x1e13:
		      regmbc('d'); regmbc(0x10f); regmbc(0x111);
		      regmbc(0x257); regmbc(0x1d6d); regmbc(0x1d81);
		      regmbc(0x1d91); regmbc(0x1e0b); regmbc(0x1e0d);
		      regmbc(0x1e0f); regmbc(0x1e11); regmbc(0x1e13);
		      परत;
	    स्थिती 'e': स्थिती 0xe8: स्थिती 0xe9: स्थिती 0xea: स्थिती 0xeb:
	    स्थिती 0x113: स्थिती 0x115: स्थिती 0x117: स्थिती 0x119:
	    स्थिती 0x11b: स्थिती 0x205: स्थिती 0x207: स्थिती 0x229:
	    स्थिती 0x247: स्थिती 0x1d92: स्थिती 0x1e15: स्थिती 0x1e17:
	    स्थिती 0x1e19: स्थिती 0x1e1b: स्थिती 0x1eb9: स्थिती 0x1ebb:
	    स्थिती 0x1e1d: स्थिती 0x1ebd: स्थिती 0x1ebf: स्थिती 0x1ec1:
	    स्थिती 0x1ec3: स्थिती 0x1ec5: स्थिती 0x1ec7:
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
		      परत;
	    स्थिती 'f': स्थिती 0x192: स्थिती 0x1d6e: स्थिती 0x1d82:
	    स्थिती 0x1e1f: स्थिती 0xa799:
		     regmbc('f'); regmbc(0x192); regmbc(0x1d6e);
		     regmbc(0x1d82); regmbc(0x1e1f); regmbc(0xa799);
		     परत;
	    स्थिती 'g': स्थिती 0x11d: स्थिती 0x11f: स्थिती 0x121: स्थिती 0x123:
	    स्थिती 0x1e5: स्थिती 0x1e7: स्थिती 0x260: स्थिती 0x1f5: स्थिती 0x1d83:
	    स्थिती 0x1e21: स्थिती 0xa7a1:
		      regmbc('g'); regmbc(0x11d); regmbc(0x11f);
		      regmbc(0x121); regmbc(0x123); regmbc(0x1e5);
		      regmbc(0x1e7); regmbc(0x1f5); regmbc(0x260);
		      regmbc(0x1d83); regmbc(0x1e21); regmbc(0xa7a1);
		      परत;
	    स्थिती 'h': स्थिती 0x125: स्थिती 0x127: स्थिती 0x21f: स्थिती 0x1e23:
	    स्थिती 0x1e25: स्थिती 0x1e27: स्थिती 0x1e29: स्थिती 0x1e2b:
	    स्थिती 0x1e96: स्थिती 0x2c68: स्थिती 0xa795:
		      regmbc('h'); regmbc(0x125); regmbc(0x127);
		      regmbc(0x21f); regmbc(0x1e23); regmbc(0x1e25);
		      regmbc(0x1e27); regmbc(0x1e29); regmbc(0x1e2b);
		      regmbc(0x1e96); regmbc(0x2c68); regmbc(0xa795);
		      परत;
	    स्थिती 'i': स्थिती 0xec: स्थिती 0xed: स्थिती 0xee: स्थिती 0xef:
	    स्थिती 0x129: स्थिती 0x12b: स्थिती 0x12d: स्थिती 0x12f:
	    स्थिती 0x1d0: स्थिती 0x209: स्थिती 0x20b: स्थिती 0x268:
	    स्थिती 0x1d96: स्थिती 0x1e2d: स्थिती 0x1e2f: स्थिती 0x1ec9:
	    स्थिती 0x1ecb:
		      regmbc('i'); regmbc(0xec); regmbc(0xed);
		      regmbc(0xee); regmbc(0xef); regmbc(0x129);
		      regmbc(0x12b); regmbc(0x12d); regmbc(0x12f);
		      regmbc(0x1d0); regmbc(0x209); regmbc(0x20b);
		      regmbc(0x268); regmbc(0x1d96); regmbc(0x1e2d);
		      regmbc(0x1e2f); regmbc(0x1ec9); regmbc(0x1ecb);
		      परत;
	    स्थिती 'j': स्थिती 0x135: स्थिती 0x1f0: स्थिती 0x249:
		      regmbc('j'); regmbc(0x135); regmbc(0x1f0);
		      regmbc(0x249);
		      परत;
	    स्थिती 'k': स्थिती 0x137: स्थिती 0x199: स्थिती 0x1e9:
	    स्थिती 0x1d84: स्थिती 0x1e31: स्थिती 0x1e33: स्थिती 0x1e35:
	    स्थिती 0x2c6a: स्थिती 0xa741:
		      regmbc('k'); regmbc(0x137); regmbc(0x199);
		      regmbc(0x1e9); regmbc(0x1d84); regmbc(0x1e31);
		      regmbc(0x1e33); regmbc(0x1e35); regmbc(0x2c6a);
		      regmbc(0xa741);
		      परत;
	    स्थिती 'l': स्थिती 0x13a: स्थिती 0x13c: स्थिती 0x13e:
	    स्थिती 0x140: स्थिती 0x142: स्थिती 0x19a: स्थिती 0x1e37:
	    स्थिती 0x1e39: स्थिती 0x1e3b: स्थिती 0x1e3d: स्थिती 0x2c61:
		      regmbc('l'); regmbc(0x13a); regmbc(0x13c);
		      regmbc(0x13e); regmbc(0x140); regmbc(0x142);
		      regmbc(0x19a); regmbc(0x1e37); regmbc(0x1e39);
		      regmbc(0x1e3b); regmbc(0x1e3d); regmbc(0x2c61);
		      परत;
	    स्थिती 'm': स्थिती 0x1d6f: स्थिती 0x1e3f: स्थिती 0x1e41: स्थिती 0x1e43:
		      regmbc('m'); regmbc(0x1d6f); regmbc(0x1e3f);
		      regmbc(0x1e41); regmbc(0x1e43);
		      परत;
	    स्थिती 'n': स्थिती 0xf1: स्थिती 0x144: स्थिती 0x146: स्थिती 0x148:
	    स्थिती 0x149: स्थिती 0x1f9: स्थिती 0x1d70: स्थिती 0x1d87:
	    स्थिती 0x1e45: स्थिती 0x1e47: स्थिती 0x1e49: स्थिती 0x1e4b:
	    स्थिती 0xa7a5:
		      regmbc('n'); regmbc(0xf1); regmbc(0x144);
		      regmbc(0x146); regmbc(0x148); regmbc(0x149);
		      regmbc(0x1f9); regmbc(0x1d70); regmbc(0x1d87);
		      regmbc(0x1e45); regmbc(0x1e47); regmbc(0x1e49);
		      regmbc(0x1e4b); regmbc(0xa7a5);
		      परत;
	    स्थिती 'o': स्थिती 0xf2: स्थिती 0xf3: स्थिती 0xf4: स्थिती 0xf5:
	    स्थिती 0xf6: स्थिती 0xf8: स्थिती 0x14d: स्थिती 0x14f: स्थिती 0x151:
	    स्थिती 0x1a1: स्थिती 0x1d2: स्थिती 0x1eb: स्थिती 0x1ed: स्थिती 0x1ff:
	    स्थिती 0x20d: स्थिती 0x20f: स्थिती 0x22b: स्थिती 0x22d: स्थिती 0x22f:
	    स्थिती 0x231: स्थिती 0x275: स्थिती 0x1e4d: स्थिती 0x1e4f:
	    स्थिती 0x1e51: स्थिती 0x1e53: स्थिती 0x1ecd: स्थिती 0x1ecf:
	    स्थिती 0x1ed1: स्थिती 0x1ed3: स्थिती 0x1ed5: स्थिती 0x1ed7:
	    स्थिती 0x1ed9: स्थिती 0x1edb: स्थिती 0x1edd: स्थिती 0x1edf:
	    स्थिती 0x1ee1: स्थिती 0x1ee3:
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
		      परत;
	    स्थिती 'p': स्थिती 0x1a5: स्थिती 0x1d71: स्थिती 0x1d88: स्थिती 0x1d7d:
	    स्थिती 0x1e55: स्थिती 0x1e57:
		      regmbc('p'); regmbc(0x1a5); regmbc(0x1d71);
		      regmbc(0x1d7d); regmbc(0x1d88); regmbc(0x1e55);
		      regmbc(0x1e57);
		      परत;
	    स्थिती 'q': स्थिती 0x24b: स्थिती 0x2a0:
		      regmbc('q'); regmbc(0x24b); regmbc(0x2a0);
		      परत;
	    स्थिती 'r': स्थिती 0x155: स्थिती 0x157: स्थिती 0x159: स्थिती 0x211:
	    स्थिती 0x213: स्थिती 0x24d: स्थिती 0x27d: स्थिती 0x1d72: स्थिती 0x1d73:
	    स्थिती 0x1d89: स्थिती 0x1e59: स्थिती 0x1e5b: स्थिती 0x1e5d: स्थिती 0x1e5f:
	    स्थिती 0xa7a7:
		      regmbc('r'); regmbc(0x155); regmbc(0x157);
		      regmbc(0x159); regmbc(0x211); regmbc(0x213);
		      regmbc(0x24d); regmbc(0x1d72); regmbc(0x1d73);
		      regmbc(0x1d89); regmbc(0x1e59); regmbc(0x27d);
		      regmbc(0x1e5b); regmbc(0x1e5d); regmbc(0x1e5f);
		      regmbc(0xa7a7);
		      परत;
	    स्थिती 's': स्थिती 0x15b: स्थिती 0x15d: स्थिती 0x15f: स्थिती 0x161:
	    स्थिती 0x1e61: स्थिती 0x219: स्थिती 0x23f: स्थिती 0x1d74: स्थिती 0x1d8a:
	    स्थिती 0x1e63: स्थिती 0x1e65: स्थिती 0x1e67: स्थिती 0x1e69: स्थिती 0xa7a9:
		      regmbc('s'); regmbc(0x15b); regmbc(0x15d);
		      regmbc(0x15f); regmbc(0x161); regmbc(0x23f);
		      regmbc(0x219); regmbc(0x1d74); regmbc(0x1d8a);
		      regmbc(0x1e61); regmbc(0x1e63); regmbc(0x1e65);
		      regmbc(0x1e67); regmbc(0x1e69); regmbc(0xa7a9);
		      परत;
	    स्थिती 't': स्थिती 0x163: स्थिती 0x165: स्थिती 0x167: स्थिती 0x1ab:
	    स्थिती 0x1ad: स्थिती 0x21b: स्थिती 0x288: स्थिती 0x1d75: स्थिती 0x1e6b:
	    स्थिती 0x1e6d: स्थिती 0x1e6f: स्थिती 0x1e71: स्थिती 0x1e97: स्थिती 0x2c66:
		      regmbc('t'); regmbc(0x163); regmbc(0x165);
		      regmbc(0x167); regmbc(0x1ab); regmbc(0x21b);
		      regmbc(0x1ad); regmbc(0x288); regmbc(0x1d75);
		      regmbc(0x1e6b); regmbc(0x1e6d); regmbc(0x1e6f);
		      regmbc(0x1e71); regmbc(0x1e97); regmbc(0x2c66);
		      परत;
	    स्थिती 'u': स्थिती 0xf9: स्थिती 0xfa: स्थिती 0xfb: स्थिती 0xfc:
	    स्थिती 0x169: स्थिती 0x16b: स्थिती 0x16d: स्थिती 0x16f:
	    स्थिती 0x171: स्थिती 0x173: स्थिती 0x1b0: स्थिती 0x1d4:
	    स्थिती 0x1d6: स्थिती 0x1d8: स्थिती 0x1da: स्थिती 0x1dc:
	    स्थिती 0x215: स्थिती 0x217: स्थिती 0x289: स्थिती 0x1e73:
	    स्थिती 0x1d7e: स्थिती 0x1d99: स्थिती 0x1e75: स्थिती 0x1e77:
	    स्थिती 0x1e79: स्थिती 0x1e7b: स्थिती 0x1ee5: स्थिती 0x1ee7:
	    स्थिती 0x1ee9: स्थिती 0x1eeb: स्थिती 0x1eed: स्थिती 0x1eef:
	    स्थिती 0x1ef1:
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
		      परत;
	    स्थिती 'v': स्थिती 0x28b: स्थिती 0x1d8c: स्थिती 0x1e7d: स्थिती 0x1e7f:
		      regmbc('v'); regmbc(0x28b); regmbc(0x1d8c);
		      regmbc(0x1e7d); regmbc(0x1e7f);
		      परत;
	    स्थिती 'w': स्थिती 0x175: स्थिती 0x1e81: स्थिती 0x1e83:
	    स्थिती 0x1e85: स्थिती 0x1e87: स्थिती 0x1e89: स्थिती 0x1e98:
		      regmbc('w'); regmbc(0x175); regmbc(0x1e81);
		      regmbc(0x1e83); regmbc(0x1e85); regmbc(0x1e87);
		      regmbc(0x1e89); regmbc(0x1e98);
		      परत;
	    स्थिती 'x': स्थिती 0x1e8b: स्थिती 0x1e8d:
		      regmbc('x'); regmbc(0x1e8b); regmbc(0x1e8d);
		      परत;
	    स्थिती 'y': स्थिती 0xfd: स्थिती 0xff: स्थिती 0x177: स्थिती 0x1b4:
	    स्थिती 0x233: स्थिती 0x24f: स्थिती 0x1e8f: स्थिती 0x1e99: स्थिती 0x1ef3:
	    स्थिती 0x1ef5: स्थिती 0x1ef7: स्थिती 0x1ef9:
		      regmbc('y'); regmbc(0xfd); regmbc(0xff);
		      regmbc(0x177); regmbc(0x1b4); regmbc(0x233);
		      regmbc(0x24f); regmbc(0x1e8f); regmbc(0x1e99);
		      regmbc(0x1ef3); regmbc(0x1ef5); regmbc(0x1ef7);
		      regmbc(0x1ef9);
		      परत;
	    स्थिती 'z': स्थिती 0x17a: स्थिती 0x17c: स्थिती 0x17e: स्थिती 0x1b6:
	    स्थिती 0x1d76: स्थिती 0x1d8e: स्थिती 0x1e91: स्थिती 0x1e93:
	    स्थिती 0x1e95: स्थिती 0x2c6c:
		      regmbc('z'); regmbc(0x17a); regmbc(0x17c);
		      regmbc(0x17e); regmbc(0x1b6); regmbc(0x1d76);
		      regmbc(0x1d8e); regmbc(0x1e91); regmbc(0x1e93);
		      regmbc(0x1e95); regmbc(0x2c6c);
		      परत;
	}
    }
    regmbc(c);
}

/*
 * Emit a node.
 * Return pointer to generated code.
 */
    स्थिर char_u *
regnode(पूर्णांक op)
{
    char_u  *ret;

    ret = regcode;
    जर (ret == JUST_CALC_SIZE)
	regsize += 3;
    अन्यथा
    {
	*regcode++ = op;
	*regcode++ = NUL;		// Null "next" pointer.
	*regcode++ = NUL;
    }
    परत ret;
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
    परत p;
}

/*
 * regnext - dig the "next" pointer out of a node
 * Returns NULL when calculating size, when there is no next item and when
 * there is an error.
 */
    स्थिर char_u *
regnext(char_u *p)
{
    पूर्णांक	    offset;

    जर (p == JUST_CALC_SIZE || reg_toolong)
	परत रिक्त;

    offset = NEXT(p);
    जर (offset == 0)
	परत रिक्त;

    जर (OP(p) == BACK)
	परत p - offset;
    अन्यथा
	परत p + offset;
}

/*
 * Set the next-pointer at the end of a node chain.
 */
    स्थिर शून्य
regtail(char_u *p, char_u *val)
{
    char_u	*scan;
    char_u	*temp;
    पूर्णांक		offset;

    जर (p == JUST_CALC_SIZE)
	परत;

    // Find last node.
    scan = p;
    साठी (;;)
    {
	temp = regnext(scan);
	जर (temp == रिक्त)
	    थांबा;
	scan = temp;
    }

    जर (OP(scan) == BACK)
	offset = (पूर्णांक)(scan - val);
    अन्यथा
	offset = (पूर्णांक)(val - scan);
    // When the offset uses more than 16 bits it can no longer fit in the two
    // bytes available.  Use a global flag to avoid having to check return
    // values in too many places.
    जर (offset > 0xffff)
	reg_toolong = TRUE;
    अन्यथा
    {
	*(scan + 1) = (char_u) (((अचिन्हित)offset >> 8) & 0377);
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
    जर (p == रिक्त || p == JUST_CALC_SIZE
	    || (OP(p) != BRANCH
		&& (OP(p) < BRACE_COMPLEX || OP(p) > BRACE_COMPLEX + 9)))
	परत;
    regtail(OPERAND(p), val);
}

/*
 * Insert an operator in front of already-emitted operand
 *
 * Means relocating the operand.
 */
    स्थिर शून्य
reginsert(पूर्णांक op, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    जर (regcode == JUST_CALC_SIZE)
    {
	regsize += 3;
	परत;
    }
    src = regcode;
    regcode += 3;
    dst = regcode;
    पर्यंत (src > opnd)
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
reginsert_nr(पूर्णांक op, दीर्घ val, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    जर (regcode == JUST_CALC_SIZE)
    {
	regsize += 7;
	परत;
    }
    src = regcode;
    regcode += 7;
    dst = regcode;
    पर्यंत (src > opnd)
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
    पूर्णांक		op,
    दीर्घ	minval,
    दीर्घ	maxval,
    char_u	*opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*place;

    जर (regcode == JUST_CALC_SIZE)
    {
	regsize += 11;
	परत;
    }
    src = regcode;
    regcode += 11;
    dst = regcode;
    पर्यंत (src > opnd)
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
    स्थिर पूर्णांक
seen_endbrace(पूर्णांक refnum)
{
    जर (!had_endbrace[refnum])
    {
	char_u *p;

	// Trick: check if "@<=" or "@<!" follows, in which case
	// the \1 can appear before the referenced match.
	साठी (p = regparse; *p != NUL; ++p)
	    जर (p[0] == '@' && p[1] == '<' && (p[2] == '!' || p[2] == '='))
		थांबा;
	जर (*p == NUL)
	{
	    emsg(_(e_illegal_back_reference));
	    rc_did_emsg = TRUE;
	    परत FALSE;
	}
    }
    परत TRUE;
}

/*
 * Parse the lowest level.
 *
 * Optimization:  gobbles an entire sequence of ordinary characters so that
 * it can turn them into a single node, which is smaller to store and
 * faster to run.  Don't do this when one_exactly is set.
 */
    स्थिर char_u *
regatom(पूर्णांक *flagp)
{
    char_u	    *ret;
    पूर्णांक		    flags;
    पूर्णांक		    c;
    char_u	    *p;
    पूर्णांक		    extra = 0;
    पूर्णांक		    save_prev_at_start = prev_at_start;

    *flagp = WORST;		// Tentatively.

    c = getchr();
    निवड (c)
    {
      स्थिती Magic('^'):
	ret = regnode(BOL);
	थांबा;

      स्थिती Magic('$'):
	ret = regnode(EOL);
#जर defined(FEAT_SYN_HL)
	had_eol = TRUE;
#endif
	थांबा;

      स्थिती Magic('<'):
	ret = regnode(BOW);
	थांबा;

      स्थिती Magic('>'):
	ret = regnode(EOW);
	थांबा;

      स्थिती Magic('_'):
	c = no_Magic(getchr());
	जर (c == '^')		// "\_^" is start-of-line
	{
	    ret = regnode(BOL);
	    थांबा;
	}
	जर (c == '$')		// "\_$" is end-of-line
	{
	    ret = regnode(EOL);
#जर defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#endif
	    थांबा;
	}

	extra = ADD_NL;
	*flagp |= HASNL;

	// "\_[" is character range plus newline
	जर (c == '[')
	    जा collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	// Character classes.
      स्थिती Magic('.'):
      स्थिती Magic('i'):
      स्थिती Magic('I'):
      स्थिती Magic('k'):
      स्थिती Magic('K'):
      स्थिती Magic('f'):
      स्थिती Magic('F'):
      स्थिती Magic('p'):
      स्थिती Magic('P'):
      स्थिती Magic('s'):
      स्थिती Magic('S'):
      स्थिती Magic('d'):
      स्थिती Magic('D'):
      स्थिती Magic('x'):
      स्थिती Magic('X'):
      स्थिती Magic('o'):
      स्थिती Magic('O'):
      स्थिती Magic('w'):
      स्थिती Magic('W'):
      स्थिती Magic('h'):
      स्थिती Magic('H'):
      स्थिती Magic('a'):
      स्थिती Magic('A'):
      स्थिती Magic('l'):
      स्थिती Magic('L'):
      स्थिती Magic('u'):
      स्थिती Magic('U'):
	p = vim_strchr(classchars, no_Magic(c));
	जर (p == रिक्त)
	    EMSG_RET_NULL(_(e_invalid_use_of_underscore));

	// When '.' is followed by a composing char ignore the dot, so that
	// the composing char is matched here.
	जर (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	{
	    c = getchr();
	    जा do_multibyte;
	}
	ret = regnode(classcodes[p - classchars] + extra);
	*flagp |= HASWIDTH | SIMPLE;
	थांबा;

      स्थिती Magic('n'):
	जर (reg_string)
	{
	    // In a string "\n" matches a newline character.
	    ret = regnode(EXACTLY);
	    regc(NL);
	    regc(NUL);
	    *flagp |= HASWIDTH | SIMPLE;
	}
	अन्यथा
	{
	    // In buffer text "\n" matches the end of a line.
	    ret = regnode(NEWL);
	    *flagp |= HASWIDTH | HASNL;
	}
	थांबा;

      स्थिती Magic('('):
	जर (one_exactly)
	    EMSG_ONE_RET_NULL;
	ret = reg(REG_PAREN, &flags);
	जर (ret == रिक्त)
	    परत रिक्त;
	*flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
	थांबा;

      स्थिती NUL:
      स्थिती Magic('|'):
      स्थिती Magic('&'):
      स्थिती Magic(')'):
	जर (one_exactly)
	    EMSG_ONE_RET_NULL;
	// Supposed to be caught earlier.
	IEMSG_RET_NULL(e_internal_error_in_regexp);
	// NOTREACHED

      स्थिती Magic('='):
      स्थिती Magic('?'):
      स्थिती Magic('+'):
      स्थिती Magic('@'):
      स्थिती Magic('{'):
      स्थिती Magic('*'):
	c = no_Magic(c);
	EMSG3_RET_NULL(_(e_str_chr_follows_nothing),
		(c == '*' ? reg_magic >= MAGIC_ON : reg_magic == MAGIC_ALL), c);
	// NOTREACHED

      स्थिती Magic('~'):		// previous substitute pattern
	    जर (reg_prev_sub != रिक्त)
	    {
		char_u	    *lp;

		ret = regnode(EXACTLY);
		lp = reg_prev_sub;
		पर्यंत (*lp != NUL)
		    regc(*lp++);
		regc(NUL);
		जर (*reg_prev_sub != NUL)
		{
		    *flagp |= HASWIDTH;
		    जर ((lp - reg_prev_sub) == 1)
			*flagp |= SIMPLE;
		}
	    }
	    अन्यथा
		EMSG_RET_NULL(_(e_no_previous_substitute_regular_expression));
	    थांबा;

      स्थिती Magic('1'):
      स्थिती Magic('2'):
      स्थिती Magic('3'):
      स्थिती Magic('4'):
      स्थिती Magic('5'):
      स्थिती Magic('6'):
      स्थिती Magic('7'):
      स्थिती Magic('8'):
      स्थिती Magic('9'):
	    {
		पूर्णांक		    refnum;

		refnum = c - Magic('0');
		जर (!seen_endbrace(refnum))
		    परत रिक्त;
		ret = regnode(BACKREF + refnum);
	    }
	    थांबा;

      स्थिती Magic('z'):
	{
	    c = no_Magic(getchr());
	    निवड (c)
	    {
#ifdef FEAT_SYN_HL
		स्थिती '(': जर ((reg_do_extmatch & REX_SET) == 0)
			      EMSG_RET_NULL(_(e_z_not_allowed_here));
			  जर (one_exactly)
			      EMSG_ONE_RET_NULL;
			  ret = reg(REG_ZPAREN, &flags);
			  जर (ret == रिक्त)
			      परत रिक्त;
			  *flagp |= flags & (HASWIDTH|SPSTART|HASNL|HASLOOKBH);
			  re_has_z = REX_SET;
			  थांबा;

		स्थिती '1':
		स्थिती '2':
		स्थिती '3':
		स्थिती '4':
		स्थिती '5':
		स्थिती '6':
		स्थिती '7':
		स्थिती '8':
		स्थिती '9': जर ((reg_do_extmatch & REX_USE) == 0)
			      EMSG_RET_NULL(_(e_z1_z9_not_allowed_here));
			  ret = regnode(ZREF + c - '0');
			  re_has_z = REX_USE;
			  थांबा;
#endif

		स्थिती 's': ret = regnode(MOPEN + 0);
			  जर (re_mult_next("\\zs") == FAIL)
			      परत रिक्त;
			  थांबा;

		स्थिती 'e': ret = regnode(MCLOSE + 0);
			  जर (re_mult_next("\\ze") == FAIL)
			      परत रिक्त;
			  थांबा;

		पूर्वनिर्धारित:  EMSG_RET_NULL(_(e_invalid_character_after_bsl_z));
	    }
	}
	थांबा;

      स्थिती Magic('%'):
	{
	    c = no_Magic(getchr());
	    निवड (c)
	    {
		// () without a back reference
		स्थिती '(':
		    जर (one_exactly)
			EMSG_ONE_RET_NULL;
		    ret = reg(REG_NPAREN, &flags);
		    जर (ret == रिक्त)
			परत रिक्त;
		    *flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
		    थांबा;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		स्थिती '^':
		    ret = regnode(RE_BOF);
		    थांबा;

		स्थिती '$':
		    ret = regnode(RE_EOF);
		    थांबा;

		स्थिती '#':
		    जर (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_start_of_pattern),
								  regparse[1]);
			परत FAIL;
		    }
		    ret = regnode(CURSOR);
		    थांबा;

		स्थिती 'V':
		    ret = regnode(RE_VISUAL);
		    थांबा;

		स्थिती 'C':
		    ret = regnode(RE_COMPOSING);
		    थांबा;

		// \%[abc]: Emit as a list of branches, all ending at the last
		// branch which matches nothing.
		स्थिती '[':
			  जर (one_exactly)	// doesn't nest
			      EMSG_ONE_RET_NULL;
			  {
			      char_u	*lastbranch;
			      char_u	*lastnode = रिक्त;
			      char_u	*br;

			      ret = रिक्त;
			      पर्यंत ((c = getchr()) != ']')
			      {
				  जर (c == NUL)
				      EMSG2_RET_NULL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
				  br = regnode(BRANCH);
				  जर (ret == रिक्त)
				      ret = br;
				  अन्यथा
				  {
				      regtail(lastnode, br);
				      जर (reg_toolong)
					  परत रिक्त;
				  }

				  ungetchr();
				  one_exactly = TRUE;
				  lastnode = regatom(flagp);
				  one_exactly = FALSE;
				  जर (lastnode == रिक्त)
				      परत रिक्त;
			      }
			      जर (ret == रिक्त)
				  EMSG2_RET_NULL(_(e_empty_str_brackets),
						      reg_magic == MAGIC_ALL);
			      lastbranch = regnode(BRANCH);
			      br = regnode(NOTHING);
			      जर (ret != JUST_CALC_SIZE)
			      {
				  regtail(lastnode, br);
				  regtail(lastbranch, br);
				  // connect all branches to the NOTHING
				  // branch at the end
				  साठी (br = ret; br != lastnode; )
				  {
				      जर (OP(br) == BRANCH)
				      {
					  regtail(br, lastbranch);
					  जर (reg_toolong)
					      परत रिक्त;
					  br = OPERAND(br);
				      }
				      अन्यथा
					  br = regnext(br);
				  }
			      }
			      *flagp &= ~(HASWIDTH | SIMPLE);
			      थांबा;
			  }

		स्थिती 'd':   // %d123 decimal
		स्थिती 'o':   // %o123 octal
		स्थिती 'x':   // %xab hex 2
		स्थिती 'u':   // %uabcd hex 4
		स्थिती 'U':   // %U1234abcd hex 8
			  {
			      vimlong_T i;

			      निवड (c)
			      {
				  स्थिती 'd': i = getdecchrs(); थांबा;
				  स्थिती 'o': i = getoctchrs(); थांबा;
				  स्थिती 'x': i = gethexchrs(2); थांबा;
				  स्थिती 'u': i = gethexchrs(4); थांबा;
				  स्थिती 'U': i = gethexchrs(8); थांबा;
				  पूर्वनिर्धारित:  i = -1; थांबा;
			      }

			      जर (i < 0 || i > पूर्णांक_उच्च)
				  EMSG2_RET_NULL(
					    _(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			      जर (use_multibytecode(i))
				  ret = regnode(MULTIBYTECODE);
			      अन्यथा
				  ret = regnode(EXACTLY);
			      जर (i == 0)
				  regc(0x0a);
			      अन्यथा
				  regmbc((पूर्णांक)i);
			      regc(NUL);
			      *flagp |= HASWIDTH;
			      थांबा;
			  }

		पूर्वनिर्धारित:
			  जर (VIM_ISDIGIT(c) || c == '<' || c == '>'
						|| c == '\'' || c == '.')
			  {
			      long_u	n = 0;
			      पूर्णांक	cmp;
			      पूर्णांक	cur = FALSE;
			      पूर्णांक	got_digit = FALSE;

			      cmp = c;
			      जर (cmp == '<' || cmp == '>')
				  c = getchr();
			      जर (no_Magic(c) == '.')
			      {
				  cur = TRUE;
				  c = getchr();
			      }
			      पर्यंत (VIM_ISDIGIT(c))
			      {
				  got_digit = TRUE;
				  n = n * 10 + (c - '0');
				  c = getchr();
			      }
			      जर (no_Magic(c) == '\'' && n == 0)
			      {
				  // "\%'m", "\%<'m" and "\%>'m": Mark
				  c = getchr();
				  ret = regnode(RE_MARK);
				  जर (ret == JUST_CALC_SIZE)
				      regsize += 2;
				  अन्यथा
				  {
				      *regcode++ = c;
				      *regcode++ = cmp;
				  }
				  थांबा;
			      }
			      अन्यथा जर ((c == 'l' || c == 'c' || c == 'v')
					  && (cur || got_digit))
			      {
				  जर (cur && n)
				  {
				    semsg(_(e_regexp_संख्या_after_dot_pos_शोध_chr),
								  no_Magic(c));
				    rc_did_emsg = TRUE;
				    परत रिक्त;
				  }
				  जर (c == 'l')
				  {
				      जर (cur)
					  n = curwin->w_cursor.lnum;
				      ret = regnode(RE_LNUM);
				      जर (save_prev_at_start)
					  at_start = TRUE;
				  }
				  अन्यथा जर (c == 'c')
				  {
				      जर (cur)
				      {
					  n = curwin->w_cursor.col;
					  n++;
				      }
				      ret = regnode(RE_COL);
				  }
				  अन्यथा
				  {
				      जर (cur)
				      {
					  colnr_T vcol = 0;

					  getvvcol(curwin, &curwin->w_cursor,
							 रिक्त, रिक्त, &vcol, 0);
					  ++vcol;
					  n = vcol;
				      }
				      ret = regnode(RE_VCOL);
				  }
				  जर (ret == JUST_CALC_SIZE)
				      regsize += 5;
				  अन्यथा
				  {
				      // put the number and the optional
				      // comparator after the opcode
				      regcode = re_put_long(regcode, n);
				      *regcode++ = cmp;
				  }
				  थांबा;
			      }
			  }

			  EMSG2_RET_NULL(_(e_invalid_character_after_str),
						      reg_magic == MAGIC_ALL);
	    }
	}
	थांबा;

      स्थिती Magic('['):
collection:
	{
	    char_u	*lp;

	    // If there is no matching ']', we assume the '[' is a normal
	    // character.  This makes 'incsearch' and ":help [" work.
	    lp = skip_anyof(regparse);
	    जर (*lp == ']')	// there is a matching ']'
	    {
		पूर्णांक	startc = -1;	// > 0 when next '-' is a range
		पूर्णांक	endc;

		जर (extra == 0)
		{
		    पूर्णांक	cl = bt_recognize_char_class(regparse, lp);

		    जर (cl != 0)
		    {
			ret = regnode(cl);
			regparse = lp;
			skipchr();
			*flagp |= HASWIDTH | SIMPLE;
			थांबा;
		    }
		}

		// In a character class, different parsing rules apply.
		// Not even \ is special anymore, nothing is.
		जर (*regparse == '^')	    // Complement of range.
		{
		    ret = regnode(ANYBUT + extra);
		    regparse++;
		}
		अन्यथा
		    ret = regnode(ANYOF + extra);

		// At the start ']' and '-' mean the literal character.
		जर (*regparse == ']' || *regparse == '-')
		{
		    startc = *regparse;
		    regc(*regparse++);
		}

		पर्यंत (*regparse != NUL && *regparse != ']')
		{
		    जर (*regparse == '-')
		    {
			++regparse;
			// The '-' is not used for a range at the end and
			// after or before a '\n'.
			जर (*regparse == ']' || *regparse == NUL
				|| startc == -1
				|| (regparse[0] == '\\' && regparse[1] == 'n'))
			{
			    regc('-');
			    startc = '-';	// [--x] is a range
			}
			अन्यथा
			{
			    // Also accept "a-[.z.]"
			    endc = 0;
			    जर (*regparse == '[')
				endc = get_coll_element(&regparse);
			    जर (endc == 0)
			    {
				जर (has_mbyte)
				    endc = mb_ptr2char_adv(&regparse);
				अन्यथा
				    endc = *regparse++;
			    }

			    // Handle \o40, \x20 and \u20AC style sequences
			    जर (endc == '\\' && !reg_cpo_lit && !reg_cpo_bsl)
				endc = coll_get_char();

			    जर (startc > endc)
				EMSG_RET_NULL(_(e_reverse_range_in_character_class));
			    जर (has_mbyte && ((*mb_char2len)(startc) > 1
						 || (*mb_char2len)(endc) > 1))
			    {
				// Limit to a range of 256 chars.
				जर (endc > startc + 256)
				    EMSG_RET_NULL(_(e_range_too_large_in_character_class));
				पर्यंत (++startc <= endc)
				    regmbc(startc);
			    }
			    अन्यथा
			    {
				पर्यंत (++startc <= endc)
				    regc(startc);
			    }
			    startc = -1;
			}
		    }
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    अन्यथा जर (*regparse == '\\'
			    && !reg_cpo_bsl
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != रिक्त
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR,
						       regparse[1]) != रिक्त)))
		    {
			regparse++;
			जर (*regparse == 'n')
			{
			    // '\n' in range: also match NL
			    जर (ret != JUST_CALC_SIZE)
			    {
				// Using \n inside [^] does not change what
				// matches. "[^\n]" is the same as ".".
				जर (*ret == ANYOF)
				{
				    *ret = ANYOF + ADD_NL;
				    *flagp |= HASNL;
				}
				// else: must have had a \n already
			    }
			    regparse++;
			    startc = -1;
			}
			अन्यथा जर (*regparse == 'd'
				|| *regparse == 'o'
				|| *regparse == 'x'
				|| *regparse == 'u'
				|| *regparse == 'U')
			{
			    startc = coll_get_char();
			    // max UTF-8 Codepoint is U+10FFFF,
			    // but allow values until INT_MAX
			    जर (startc == पूर्णांक_उच्च)
				EMSG_RET_NULL(_(e_unicode_val_too_large));
			    जर (startc == 0)
				regc(0x0a);
			    अन्यथा
				regmbc(startc);
			}
			अन्यथा
			{
			    startc = backslash_trans(*regparse++);
			    regc(startc);
			}
		    }
		    अन्यथा जर (*regparse == '[')
		    {
			पूर्णांक c_class;
			पूर्णांक cu;

			c_class = get_char_class(&regparse);
			startc = -1;
			// Characters assumed to be 8 bits!
			निवड (c_class)
			{
			    स्थिती CLASS_NONE:
				c_class = get_equi_class(&regparse);
				जर (c_class != 0)
				{
				    // produce equivalence class
				    reg_equi_class(c_class);
				}
				अन्यथा जर ((c_class =
					    get_coll_element(&regparse)) != 0)
				{
				    // produce a collating element
				    regmbc(c_class);
				}
				अन्यथा
				{
				    // literal '[', allow [[-x] as a range
				    startc = *regparse++;
				    regc(startc);
				}
				थांबा;
			    स्थिती CLASS_ALNUM:
				साठी (cu = 1; cu < 128; cu++)
				    जर (है_अक्षर_अंक(cu))
					regmbc(cu);
				थांबा;
			    स्थिती CLASS_ALPHA:
				साठी (cu = 1; cu < 128; cu++)
				    जर (है_अक्षर(cu))
					regmbc(cu);
				थांबा;
			    स्थिती CLASS_BLANK:
				regc(' ');
				regc('\t');
				थांबा;
			    स्थिती CLASS_CNTRL:
				साठी (cu = 1; cu <= 127; cu++)
				    जर (है_नियंत्रण(cu))
					regmbc(cu);
				थांबा;
			    स्थिती CLASS_DIGIT:
				साठी (cu = 1; cu <= 127; cu++)
				    जर (VIM_ISDIGIT(cu))
					regmbc(cu);
				थांबा;
			    स्थिती CLASS_GRAPH:
				साठी (cu = 1; cu <= 127; cu++)
				    जर (है_चित्र(cu))
					regmbc(cu);
				थांबा;
			    स्थिती CLASS_LOWER:
				साठी (cu = 1; cu <= 255; cu++)
				    जर (MB_ISLOWER(cu) && cu != 170
								 && cu != 186)
					regmbc(cu);
				थांबा;
			    स्थिती CLASS_PRINT:
				साठी (cu = 1; cu <= 255; cu++)
				    जर (vim_isprintc(cu))
					regmbc(cu);
				थांबा;
			    स्थिती CLASS_PUNCT:
				साठी (cu = 1; cu < 128; cu++)
				    जर (है_विराम(cu))
					regmbc(cu);
				थांबा;
			    स्थिती CLASS_SPACE:
				साठी (cu = 9; cu <= 13; cu++)
				    regc(cu);
				regc(' ');
				थांबा;
			    स्थिती CLASS_UPPER:
				साठी (cu = 1; cu <= 255; cu++)
				    जर (MB_ISUPPER(cu))
					regmbc(cu);
				थांबा;
			    स्थिती CLASS_XDIGIT:
				साठी (cu = 1; cu <= 255; cu++)
				    जर (vim_isxdigit(cu))
					regmbc(cu);
				थांबा;
			    स्थिती CLASS_TAB:
				regc('\t');
				थांबा;
			    स्थिती CLASS_RETURN:
				regc('\r');
				थांबा;
			    स्थिती CLASS_BACKSPACE:
				regc('\b');
				थांबा;
			    स्थिती CLASS_ESCAPE:
				regc('\033');
				थांबा;
			    स्थिती CLASS_IDENT:
				साठी (cu = 1; cu <= 255; cu++)
				    जर (vim_isIDc(cu))
					regmbc(cu);
				थांबा;
			    स्थिती CLASS_KEYWORD:
				साठी (cu = 1; cu <= 255; cu++)
				    जर (reg_iswordc(cu))
					regmbc(cu);
				थांबा;
			    स्थिती CLASS_FNAME:
				साठी (cu = 1; cu <= 255; cu++)
				    जर (vim_isfilec(cu))
					regmbc(cu);
				थांबा;
			}
		    }
		    अन्यथा
		    {
			जर (has_mbyte)
			{
			    पूर्णांक	len;

			    // produce a multibyte character, including any
			    // following composing characters
			    startc = mb_ptr2char(regparse);
			    len = (*mb_ptr2len)(regparse);
			    जर (enc_utf8 && utf_char2len(startc) != len)
				startc = -1;	// composing chars
			    पर्यंत (--len >= 0)
				regc(*regparse++);
			}
			अन्यथा
			{
			    startc = *regparse++;
			    regc(startc);
			}
		    }
		}
		regc(NUL);
		prevchr_len = 1;	// last char was the ']'
		जर (*regparse != ']')
		    EMSG_RET_NULL(_(e_too_many_brackets));  // Cannot happen?
		skipchr();	    // let's be friends with the lexer again
		*flagp |= HASWIDTH | SIMPLE;
		थांबा;
	    }
	    अन्यथा जर (reg_strict)
		EMSG2_RET_NULL(_(e_missing_rsb_after_str_lsb),
							reg_magic > MAGIC_OFF);
	}
	// FALLTHROUGH

      पूर्वनिर्धारित:
	{
	    पूर्णांक		len;

	    // A multi-byte character is handled as a separate atom if it's
	    // before a multi and when it's a composing char.
	    जर (use_multibytecode(c))
	    {
do_multibyte:
		ret = regnode(MULTIBYTECODE);
		regmbc(c);
		*flagp |= HASWIDTH | SIMPLE;
		थांबा;
	    }

	    ret = regnode(EXACTLY);

	    // Append characters as long as:
	    // - there is no following multi, we then need the character in
	    //   front of it as a single character operand
	    // - not running into a Magic character
	    // - "one_exactly" is not set
	    // But always emit at least one character.  Might be a Multi,
	    // e.g., a "[" without matching "]".
	    साठी (len = 0; c != NUL && (len == 0
			|| (re_multi_type(peekchr()) == NOT_MULTI
			    && !one_exactly
			    && !is_Magic(c))); ++len)
	    {
		c = no_Magic(c);
		जर (has_mbyte)
		{
		    regmbc(c);
		    जर (enc_utf8)
		    {
			पूर्णांक	l;

			// Need to get composing character too.
			साठी (;;)
			{
			    l = utf_ptr2len(regparse);
			    जर (!UTF_COMPOSINGLIKE(regparse, regparse + l))
				थांबा;
			    regmbc(utf_ptr2char(regparse));
			    skipchr();
			}
		    }
		}
		अन्यथा
		    regc(c);
		c = getchr();
	    }
	    ungetchr();

	    regc(NUL);
	    *flagp |= HASWIDTH;
	    जर (len == 1)
		*flagp |= SIMPLE;
	}
	थांबा;
    }

    परत ret;
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
regpiece(पूर्णांक *flagp)
{
    char_u	    *ret;
    पूर्णांक		    op;
    char_u	    *next;
    पूर्णांक		    flags;
    दीर्घ	    minval;
    दीर्घ	    maxval;

    ret = regatom(&flags);
    जर (ret == रिक्त)
	परत रिक्त;

    op = peekchr();
    जर (re_multi_type(op) == NOT_MULTI)
    {
	*flagp = flags;
	परत ret;
    }
    // default flags
    *flagp = (WORST | SPSTART | (flags & (HASNL | HASLOOKBH)));

    skipchr();
    निवड (op)
    {
	स्थिती Magic('*'):
	    जर (flags & SIMPLE)
		reginsert(STAR, ret);
	    अन्यथा
	    {
		// Emit x* as (x&|), where & means "self".
		reginsert(BRANCH, ret); // Either x
		regoptail(ret, regnode(BACK));	// and loop
		regoptail(ret, ret);	// back
		regtail(ret, regnode(BRANCH));	// or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    थांबा;

	स्थिती Magic('+'):
	    जर (flags & SIMPLE)
		reginsert(PLUS, ret);
	    अन्यथा
	    {
		// Emit x+ as x(&|), where & means "self".
		next = regnode(BRANCH); // Either
		regtail(ret, next);
		regtail(regnode(BACK), ret);	// loop back
		regtail(next, regnode(BRANCH)); // or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    *flagp = (WORST | HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    थांबा;

	स्थिती Magic('@'):
	    {
		पूर्णांक	lop = END;
		दीर्घ	nr;

		nr = (दीर्घ)getdecchrs();
		निवड (no_Magic(getchr()))
		{
		    स्थिती '=': lop = MATCH; थांबा;		  // \@=
		    स्थिती '!': lop = NOMATCH; थांबा;		  // \@!
		    स्थिती '>': lop = SUBPAT; थांबा;		  // \@>
		    स्थिती '<': निवड (no_Magic(getchr()))
			      {
				  स्थिती '=': lop = BEHIND; थांबा;   // \@<=
				  स्थिती '!': lop = NOBEHIND; थांबा; // \@<!
			      }
		}
		जर (lop == END)
		    EMSG2_RET_NULL(_(e_invalid_character_after_str_at),
						      reg_magic == MAGIC_ALL);
		// Look behind must match with behind_pos.
		जर (lop == BEHIND || lop == NOBEHIND)
		{
		    regtail(ret, regnode(BHPOS));
		    *flagp |= HASLOOKBH;
		}
		regtail(ret, regnode(END)); // operand ends
		जर (lop == BEHIND || lop == NOBEHIND)
		{
		    जर (nr < 0)
			nr = 0; // no limit is same as zero limit
		    reginsert_nr(lop, nr, ret);
		}
		अन्यथा
		    reginsert(lop, ret);
		थांबा;
	    }

	स्थिती Magic('?'):
	स्थिती Magic('='):
	    // Emit x= as (x|)
	    reginsert(BRANCH, ret);		// Either x
	    regtail(ret, regnode(BRANCH));	// or
	    next = regnode(NOTHING);		// null.
	    regtail(ret, next);
	    regoptail(ret, next);
	    थांबा;

	स्थिती Magic('{'):
	    जर (!read_limits(&minval, &maxval))
		परत रिक्त;
	    जर (flags & SIMPLE)
	    {
		reginsert(BRACE_SIMPLE, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
	    }
	    अन्यथा
	    {
		जर (अंक_complex_braces >= 10)
		    EMSG2_RET_NULL(_(e_too_many_complex_str_curly),
						      reg_magic == MAGIC_ALL);
		reginsert(BRACE_COMPLEX + अंक_complex_braces, ret);
		regoptail(ret, regnode(BACK));
		regoptail(ret, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
		++अंक_complex_braces;
	    }
	    जर (minval > 0 && maxval > 0)
		*flagp = (HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    थांबा;
    }
    जर (re_multi_type(peekchr()) != NOT_MULTI)
    {
	// Can't have a multi follow a multi.
	जर (peekchr() == Magic('*'))
	    EMSG2_RET_NULL(_(e_nested_str), reg_magic >= MAGIC_ON);
	EMSG3_RET_NULL(_(e_nested_str_chr), reg_magic == MAGIC_ALL,
							  no_Magic(peekchr()));
    }

    परत ret;
}

/*
 * Parse one alternative of an | or & operator.
 * Implements the concatenation operator.
 */
    स्थिर char_u *
regconcat(पूर्णांक *flagp)
{
    char_u	*first = रिक्त;
    char_u	*chain = रिक्त;
    char_u	*latest;
    पूर्णांक		flags;
    पूर्णांक		cont = TRUE;

    *flagp = WORST;		// Tentatively.

    पर्यंत (cont)
    {
	निवड (peekchr())
	{
	    स्थिती NUL:
	    स्थिती Magic('|'):
	    स्थिती Magic('&'):
	    स्थिती Magic(')'):
			    cont = FALSE;
			    थांबा;
	    स्थिती Magic('Z'):
			    regflags |= RF_ICOMBINE;
			    skipchr_keepstart();
			    थांबा;
	    स्थिती Magic('c'):
			    regflags |= RF_ICASE;
			    skipchr_keepstart();
			    थांबा;
	    स्थिती Magic('C'):
			    regflags |= RF_NOICASE;
			    skipchr_keepstart();
			    थांबा;
	    स्थिती Magic('v'):
			    reg_magic = MAGIC_ALL;
			    skipchr_keepstart();
			    curchr = -1;
			    थांबा;
	    स्थिती Magic('m'):
			    reg_magic = MAGIC_ON;
			    skipchr_keepstart();
			    curchr = -1;
			    थांबा;
	    स्थिती Magic('M'):
			    reg_magic = MAGIC_OFF;
			    skipchr_keepstart();
			    curchr = -1;
			    थांबा;
	    स्थिती Magic('V'):
			    reg_magic = MAGIC_NONE;
			    skipchr_keepstart();
			    curchr = -1;
			    थांबा;
	    पूर्वनिर्धारित:
			    latest = regpiece(&flags);
			    जर (latest == रिक्त || reg_toolong)
				परत रिक्त;
			    *flagp |= flags & (HASWIDTH | HASNL | HASLOOKBH);
			    जर (chain == रिक्त)	// First piece.
				*flagp |= flags & SPSTART;
			    अन्यथा
				regtail(chain, latest);
			    chain = latest;
			    जर (first == रिक्त)
				first = latest;
			    थांबा;
	}
    }
    जर (first == रिक्त)		// Loop ran zero times.
	first = regnode(NOTHING);
    परत first;
}

/*
 * Parse one alternative of an | operator.
 * Implements the & operator.
 */
    स्थिर char_u *
regbranch(पूर्णांक *flagp)
{
    char_u	*ret;
    char_u	*chain = रिक्त;
    char_u	*latest;
    पूर्णांक		flags;

    *flagp = WORST | HASNL;		// Tentatively.

    ret = regnode(BRANCH);
    साठी (;;)
    {
	latest = regconcat(&flags);
	जर (latest == रिक्त)
	    परत रिक्त;
	// If one of the branches has width, the whole thing has.  If one of
	// the branches anchors at start-of-line, the whole thing does.
	// If one of the branches uses look-behind, the whole thing does.
	*flagp |= flags & (HASWIDTH | SPSTART | HASLOOKBH);
	// If one of the branches doesn't match a line-break, the whole thing
	// doesn't.
	*flagp &= ~HASNL | (flags & HASNL);
	जर (chain != रिक्त)
	    regtail(chain, latest);
	जर (peekchr() != Magic('&'))
	    थांबा;
	skipchr();
	regtail(latest, regnode(END)); // operand ends
	जर (reg_toolong)
	    थांबा;
	reginsert(MATCH, latest);
	chain = latest;
    }

    परत ret;
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
    पूर्णांक		paren,	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
    पूर्णांक		*flagp)
{
    char_u	*ret;
    char_u	*br;
    char_u	*ender;
    पूर्णांक		parno = 0;
    पूर्णांक		flags;

    *flagp = HASWIDTH;		// Tentatively.

#ifdef FEAT_SYN_HL
    जर (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	जर (regnzpar >= NSUBEXP)
	    EMSG_RET_NULL(_(e_too_many_z));
	parno = regnzpar;
	regnzpar++;
	ret = regnode(ZOPEN + parno);
    }
    अन्यथा
#endif
	जर (paren == REG_PAREN)
    {
	// Make a MOPEN node.
	जर (regnpar >= NSUBEXP)
	    EMSG2_RET_NULL(_(e_too_many_str_open), reg_magic == MAGIC_ALL);
	parno = regnpar;
	++regnpar;
	ret = regnode(MOPEN + parno);
    }
    अन्यथा जर (paren == REG_NPAREN)
    {
	// Make a NOPEN node.
	ret = regnode(NOPEN);
    }
    अन्यथा
	ret = रिक्त;

    जर (bt_reg_parse_depth >= REG_MAX_PAREN_DEPTH)
	EMSG_RET_NULL(_(e_command_too_complex));
    ++bt_reg_parse_depth;

    // Pick up the branches, linking them together.
    br = regbranch(&flags);
    जर (br == रिक्त)
    {
	ret = रिक्त;
	जा theend;
    }
    जर (ret != रिक्त)
	regtail(ret, br);	// [MZ]OPEN -> first.
    अन्यथा
	ret = br;
    // If one of the branches can be zero-width, the whole thing can.
    // If one of the branches has * at start or matches a line-break, the
    // whole thing can.
    जर (!(flags & HASWIDTH))
	*flagp &= ~HASWIDTH;
    *flagp |= flags & (SPSTART | HASNL | HASLOOKBH);
    पर्यंत (peekchr() == Magic('|'))
    {
	skipchr();
	br = regbranch(&flags);
	जर (br == रिक्त || reg_toolong)
	{
	    ret = रिक्त;
	    जा theend;
	}
	regtail(ret, br);	// BRANCH -> BRANCH.
	जर (!(flags & HASWIDTH))
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
    साठी (br = ret; br != रिक्त; br = regnext(br))
	regoptail(br, ender);

    // Check for proper termination.
    जर (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
#ifdef FEAT_SYN_HL
	जर (paren == REG_ZPAREN)
	{
	    emsg(_(e_unmatched_z));
	    rc_did_emsg = TRUE;
	    ret = रिक्त;
	    जा theend;
	}
	अन्यथा
#endif
	जर (paren == REG_NPAREN)
	{
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = रिक्त;
	    जा theend;
	}
	अन्यथा
	{
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = रिक्त;
	    जा theend;
	}
    }
    अन्यथा जर (paren == REG_NOPAREN && peekchr() != NUL)
    {
	जर (curchr == Magic(')'))
	{
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = रिक्त;
	    जा theend;
	}
	अन्यथा
	{
	    emsg(_(e_trailing_characters));	// "Can't happen".
	    rc_did_emsg = TRUE;
	    ret = रिक्त;
	    जा theend;
	}
    }
    // Here we set the flag allowing back references to this set of
    // parentheses.
    जर (paren == REG_PAREN)
	had_endbrace[parno] = TRUE;	// have seen the close paren

theend:
    --bt_reg_parse_depth;
    परत ret;
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
bt_regcomp(char_u *अभिव्यक्ती, पूर्णांक re_flags)
{
    bt_regprog_T    *r;
    char_u	*scan;
    char_u	*longest;
    पूर्णांक		len;
    पूर्णांक		flags;

    जर (अभिव्यक्ती == रिक्त)
	IEMSG_RET_NULL(e_null_argument);

    init_class_tab();

    // First pass: determine size, legality.
    regcomp_start(अभिव्यक्ती, re_flags);
    regcode = JUST_CALC_SIZE;
    regc(REGMAGIC);
    जर (reg(REG_NOPAREN, &flags) == रिक्त)
	परत रिक्त;

    // Allocate space.
    r = alloc(दुरत्व(bt_regprog_T, program) + regsize);
    जर (r == रिक्त)
	परत रिक्त;
    r->re_in_use = FALSE;
#ifdef DEBUG
    r->regsz = regsize;
#endif

    // Second pass: emit code.
    regcomp_start(अभिव्यक्ती, re_flags);
    regcode = r->program;
    regc(REGMAGIC);
    जर (reg(REG_NOPAREN, &flags) == रिक्त || reg_toolong)
    {
	vim_free(r);
	जर (reg_toolong)
	    EMSG_RET_NULL(_(e_pattern_too_long));
	परत रिक्त;
    }

    // Dig out information for optimizations.
    r->regstart = NUL;		// Worst-case defaults.
    r->reganch = 0;
    r->regmust = रिक्त;
    r->regmlen = 0;
    r->regflags = regflags;
    जर (flags & HASNL)
	r->regflags |= RF_HASNL;
    जर (flags & HASLOOKBH)
	r->regflags |= RF_LOOKBH;
#ifdef FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    r->reghasz = re_has_z;
#endif
    scan = r->program + 1;	// First BRANCH.
    जर (OP(regnext(scan)) == END)   // Only one top-level choice.
    {
	scan = OPERAND(scan);

	// Starting-point info.
	जर (OP(scan) == BOL || OP(scan) == RE_BOF)
	{
	    r->reganch++;
	    scan = regnext(scan);
	}

	जर (OP(scan) == EXACTLY)
	{
	    जर (has_mbyte)
		r->regstart = (*mb_ptr2char)(OPERAND(scan));
	    अन्यथा
		r->regstart = *OPERAND(scan);
	}
	अन्यथा जर ((OP(scan) == BOW
		    || OP(scan) == EOW
		    || OP(scan) == NOTHING
		    || OP(scan) == MOPEN + 0 || OP(scan) == NOPEN
		    || OP(scan) == MCLOSE + 0 || OP(scan) == NCLOSE)
		 && OP(regnext(scan)) == EXACTLY)
	{
	    जर (has_mbyte)
		r->regstart = (*mb_ptr2char)(OPERAND(regnext(scan)));
	    अन्यथा
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
	जर ((flags & SPSTART || OP(scan) == BOW || OP(scan) == EOW)
							  && !(flags & HASNL))
	{
	    माप_प्रकार  scanlen;

	    longest = रिक्त;
	    len = 0;
	    साठी (; scan != रिक्त; scan = regnext(scan))
	    {
		जर (OP(scan) == EXACTLY)
		{
		    scanlen = STRLEN(OPERAND(scan));
		    जर (scanlen >= (माप_प्रकार)len)
		    {
			longest = OPERAND(scan);
			len = (पूर्णांक)scanlen;
		    }
		}
	    }
	    r->regmust = longest;
	    r->regmlen = len;
	}
    }
#ifdef BT_REGEXP_DUMP
    regdump(अभिव्यक्ती, r);
#endif
    r->engine = &bt_regengine;
    परत (regprog_T *)r;
}

#जर defined(FEAT_SYN_HL)
/*
 * Check if during the previous call to vim_regcomp the EOL item "$" has been
 * found.  This is messy, but it works fine.
 */
    पूर्णांक
vim_regcomp_had_eol(शून्य)
{
    परत had_eol;
}
#endif

/*
 * Get a number after a backslash that is inside [].
 * When nothing is recognized return a backslash.
 */
    स्थिर पूर्णांक
coll_get_char(शून्य)
{
    vimlong_T	nr = -1;

    निवड (*regparse++)
    {
	स्थिती 'd': nr = getdecchrs(); थांबा;
	स्थिती 'o': nr = getoctchrs(); थांबा;
	स्थिती 'x': nr = gethexchrs(2); थांबा;
	स्थिती 'u': nr = gethexchrs(4); थांबा;
	स्थिती 'U': nr = gethexchrs(8); थांबा;
    }
    जर (nr < 0)
    {
	// If getting the number fails be backwards compatible: the character
	// is a backslash.
	--regparse;
	nr = '\\';
    }
    जर (nr > पूर्णांक_उच्च)
	nr = पूर्णांक_उच्च;
    परत nr;
}

/*
 * Free a compiled regexp program, returned by bt_regcomp().
 */
    स्थिर शून्य
bt_regfree(regprog_T *prog)
{
    vim_free(prog);
}

#define ADVANCE_REGINPUT() MB_PTR_ADV(rex.आदान)

/*
 * The arguments from BRACE_LIMITS are stored here.  They are actually local
 * to regmatch(), but they are here to reduce the amount of stack space used
 * (it can be called recursively many times).
 */
स्थिर दीर्घ	bl_minval;
स्थिर दीर्घ	bl_maxval;

/*
 * Save the input line and position in a regsave_T.
 */
    स्थिर शून्य
reg_save(regsave_T *save, garray_T *gap)
{
    जर (REG_MULTI)
    {
	save->rs_u.pos.col = (colnr_T)(rex.आदान - rex.ओळ);
	save->rs_u.pos.lnum = rex.lnum;
    }
    अन्यथा
	save->rs_u.ptr = rex.आदान;
    save->rs_len = gap->ga_len;
}

/*
 * Restore the input line and position from a regsave_T.
 */
    स्थिर शून्य
reg_restore(regsave_T *save, garray_T *gap)
{
    जर (REG_MULTI)
    {
	जर (rex.lnum != save->rs_u.pos.lnum)
	{
	    // only call reg_getline() when the line number changed to save
	    // a bit of time
	    rex.lnum = save->rs_u.pos.lnum;
	    rex.ओळ = reg_getline(rex.lnum);
	}
	rex.आदान = rex.ओळ + save->rs_u.pos.col;
    }
    अन्यथा
	rex.आदान = save->rs_u.ptr;
    gap->ga_len = save->rs_len;
}

/*
 * Return TRUE if current position is equal to saved position.
 */
    स्थिर पूर्णांक
reg_save_equal(regsave_T *save)
{
    जर (REG_MULTI)
	परत rex.lnum == save->rs_u.pos.lnum
				  && rex.आदान == rex.ओळ + save->rs_u.pos.col;
    परत rex.आदान == save->rs_u.ptr;
}

// Save the sub-expressions before attempting a match.
#define save_se(savep, posp, pp) \
    REG_MULTI ? save_se_multi((savep), (posp)) : save_se_one((savep), (pp))

// After a failed match restore the sub-expressions.
#define restore_se(savep, posp, pp) \
{ \
    जर (REG_MULTI) \
	*(posp) = (savep)->se_u.pos; \
    अन्यथा \
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
    posp->col = (colnr_T)(rex.आदान - rex.ओळ);
}

    स्थिर शून्य
save_se_one(save_se_T *savep, char_u **pp)
{
    savep->se_u.ptr = *pp;
    *pp = rex.आदान;
}

/*
 * regrepeat - repeatedly match something simple, return how many.
 * Advances rex.input (and rex.lnum) to just after the matched chars.
 */
    स्थिर पूर्णांक
regrepeat(
    char_u	*p,
    दीर्घ	maxcount)   // maximum number of matches allowed
{
    दीर्घ	मोजणी = 0;
    char_u	*scan;
    char_u	*opnd;
    पूर्णांक		mask;
    पूर्णांक		testval = 0;

    scan = rex.आदान;	    // Make local copy of rex.input for speed.
    opnd = OPERAND(p);
    निवड (OP(p))
    {
      स्थिती ANY:
      स्थिती ANY + ADD_NL:
	पर्यंत (मोजणी < maxcount)
	{
	    // Matching anything means we continue until end-of-line (or
	    // end-of-file for ANY + ADD_NL), only limited by maxcount.
	    पर्यंत (*scan != NUL && मोजणी < maxcount)
	    {
		++मोजणी;
		MB_PTR_ADV(scan);
	    }
	    जर (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
				      || rex.reg_ओळ_lbr || मोजणी == maxcount)
		थांबा;
	    ++मोजणी;		// count the line-break
	    reg_nextline();
	    scan = rex.आदान;
	    जर (got_int)
		थांबा;
	}
	थांबा;

      स्थिती IDENT:
      स्थिती IDENT + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      स्थिती SIDENT:
      स्थिती SIDENT + ADD_NL:
	पर्यंत (मोजणी < maxcount)
	{
	    जर (vim_isIDc(PTR2CHAR(scan)) && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    अन्यथा जर (*scan == NUL)
	    {
		जर (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ओळ_lbr)
		    थांबा;
		reg_nextline();
		scan = rex.आदान;
		जर (got_int)
		    थांबा;
	    }
	    अन्यथा जर (rex.reg_ओळ_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    अन्यथा
		थांबा;
	    ++मोजणी;
	}
	थांबा;

      स्थिती KWORD:
      स्थिती KWORD + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      स्थिती SKWORD:
      स्थिती SKWORD + ADD_NL:
	पर्यंत (मोजणी < maxcount)
	{
	    जर (vim_iswordp_buf(scan, rex.reg_buf)
					  && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    अन्यथा जर (*scan == NUL)
	    {
		जर (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ओळ_lbr)
		    थांबा;
		reg_nextline();
		scan = rex.आदान;
		जर (got_int)
		    थांबा;
	    }
	    अन्यथा जर (rex.reg_ओळ_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    अन्यथा
		थांबा;
	    ++मोजणी;
	}
	थांबा;

      स्थिती FNAME:
      स्थिती FNAME + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      स्थिती SFNAME:
      स्थिती SFNAME + ADD_NL:
	पर्यंत (मोजणी < maxcount)
	{
	    जर (vim_isfilec(PTR2CHAR(scan)) && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    अन्यथा जर (*scan == NUL)
	    {
		जर (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ओळ_lbr)
		    थांबा;
		reg_nextline();
		scan = rex.आदान;
		जर (got_int)
		    थांबा;
	    }
	    अन्यथा जर (rex.reg_ओळ_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    अन्यथा
		थांबा;
	    ++मोजणी;
	}
	थांबा;

      स्थिती PRINT:
      स्थिती PRINT + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      स्थिती SPRINT:
      स्थिती SPRINT + ADD_NL:
	पर्यंत (मोजणी < maxcount)
	{
	    जर (*scan == NUL)
	    {
		जर (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ओळ_lbr)
		    थांबा;
		reg_nextline();
		scan = rex.आदान;
		जर (got_int)
		    थांबा;
	    }
	    अन्यथा जर (vim_isprintc(PTR2CHAR(scan)) == 1
					  && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    अन्यथा जर (rex.reg_ओळ_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    अन्यथा
		थांबा;
	    ++मोजणी;
	}
	थांबा;

      स्थिती WHITE:
      स्थिती WHITE + ADD_NL:
	testval = mask = RI_WHITE;
do_class:
	पर्यंत (मोजणी < maxcount)
	{
	    पूर्णांक		l;

	    जर (*scan == NUL)
	    {
		जर (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ओळ_lbr)
		    थांबा;
		reg_nextline();
		scan = rex.आदान;
		जर (got_int)
		    थांबा;
	    }
	    अन्यथा जर (has_mbyte && (l = (*mb_ptr2len)(scan)) > 1)
	    {
		जर (testval != 0)
		    थांबा;
		scan += l;
	    }
	    अन्यथा जर ((class_tab[*scan] & mask) == testval)
		++scan;
	    अन्यथा जर (rex.reg_ओळ_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    अन्यथा
		थांबा;
	    ++मोजणी;
	}
	थांबा;

      स्थिती NWHITE:
      स्थिती NWHITE + ADD_NL:
	mask = RI_WHITE;
	जा do_class;
      स्थिती DIGIT:
      स्थिती DIGIT + ADD_NL:
	testval = mask = RI_DIGIT;
	जा do_class;
      स्थिती NDIGIT:
      स्थिती NDIGIT + ADD_NL:
	mask = RI_DIGIT;
	जा do_class;
      स्थिती HEX:
      स्थिती HEX + ADD_NL:
	testval = mask = RI_HEX;
	जा do_class;
      स्थिती NHEX:
      स्थिती NHEX + ADD_NL:
	mask = RI_HEX;
	जा do_class;
      स्थिती OCTAL:
      स्थिती OCTAL + ADD_NL:
	testval = mask = RI_OCTAL;
	जा do_class;
      स्थिती NOCTAL:
      स्थिती NOCTAL + ADD_NL:
	mask = RI_OCTAL;
	जा do_class;
      स्थिती WORD:
      स्थिती WORD + ADD_NL:
	testval = mask = RI_WORD;
	जा do_class;
      स्थिती NWORD:
      स्थिती NWORD + ADD_NL:
	mask = RI_WORD;
	जा do_class;
      स्थिती HEAD:
      स्थिती HEAD + ADD_NL:
	testval = mask = RI_HEAD;
	जा do_class;
      स्थिती NHEAD:
      स्थिती NHEAD + ADD_NL:
	mask = RI_HEAD;
	जा do_class;
      स्थिती ALPHA:
      स्थिती ALPHA + ADD_NL:
	testval = mask = RI_ALPHA;
	जा do_class;
      स्थिती NALPHA:
      स्थिती NALPHA + ADD_NL:
	mask = RI_ALPHA;
	जा do_class;
      स्थिती LOWER:
      स्थिती LOWER + ADD_NL:
	testval = mask = RI_LOWER;
	जा do_class;
      स्थिती NLOWER:
      स्थिती NLOWER + ADD_NL:
	mask = RI_LOWER;
	जा do_class;
      स्थिती UPPER:
      स्थिती UPPER + ADD_NL:
	testval = mask = RI_UPPER;
	जा do_class;
      स्थिती NUPPER:
      स्थिती NUPPER + ADD_NL:
	mask = RI_UPPER;
	जा do_class;

      स्थिती EXACTLY:
	{
	    पूर्णांक	    cu, cl;

	    // This doesn't do a multi-byte character, because a MULTIBYTECODE
	    // would have been used for it.  It does handle single-byte
	    // characters, such as latin1.
	    जर (rex.reg_ic)
	    {
		cu = MB_TOUPPER(*opnd);
		cl = MB_TOLOWER(*opnd);
		पर्यंत (मोजणी < maxcount && (*scan == cu || *scan == cl))
		{
		    मोजणी++;
		    scan++;
		}
	    }
	    अन्यथा
	    {
		cu = *opnd;
		पर्यंत (मोजणी < maxcount && *scan == cu)
		{
		    मोजणी++;
		    scan++;
		}
	    }
	    थांबा;
	}

      स्थिती MULTIBYTECODE:
	{
	    पूर्णांक		i, len, cf = 0;

	    // Safety check (just in case 'encoding' was changed since
	    // compiling the program).
	    जर ((len = (*mb_ptr2len)(opnd)) > 1)
	    {
		जर (rex.reg_ic && enc_utf8)
		    cf = utf_fold(utf_ptr2char(opnd));
		पर्यंत (मोजणी < maxcount && (*mb_ptr2len)(scan) >= len)
		{
		    साठी (i = 0; i < len; ++i)
			जर (opnd[i] != scan[i])
			    थांबा;
		    जर (i < len && (!rex.reg_ic || !enc_utf8
					|| utf_fold(utf_ptr2char(scan)) != cf))
			थांबा;
		    scan += len;
		    ++मोजणी;
		}
	    }
	}
	थांबा;

      स्थिती ANYOF:
      स्थिती ANYOF + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH

      स्थिती ANYBUT:
      स्थिती ANYBUT + ADD_NL:
	पर्यंत (मोजणी < maxcount)
	{
	    पूर्णांक len;

	    जर (*scan == NUL)
	    {
		जर (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_ओळ_lbr)
		    थांबा;
		reg_nextline();
		scan = rex.आदान;
		जर (got_int)
		    थांबा;
	    }
	    अन्यथा जर (rex.reg_ओळ_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    अन्यथा जर (has_mbyte && (len = (*mb_ptr2len)(scan)) > 1)
	    {
		जर ((cstrchr(opnd, (*mb_ptr2char)(scan)) == रिक्त) == testval)
		    थांबा;
		scan += len;
	    }
	    अन्यथा
	    {
		जर ((cstrchr(opnd, *scan) == रिक्त) == testval)
		    थांबा;
		++scan;
	    }
	    ++मोजणी;
	}
	थांबा;

      स्थिती NEWL:
	पर्यंत (मोजणी < maxcount
		&& ((*scan == NUL && rex.lnum <= rex.reg_maxline
				       && !rex.reg_ओळ_lbr && REG_MULTI)
		    || (*scan == '\n' && rex.reg_ओळ_lbr)))
	{
	    मोजणी++;
	    जर (rex.reg_ओळ_lbr)
		ADVANCE_REGINPUT();
	    अन्यथा
		reg_nextline();
	    scan = rex.आदान;
	    जर (got_int)
		थांबा;
	}
	थांबा;

      पूर्वनिर्धारित:			// Oh dear.  Called inappropriately.
	iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	म_लिखो("Called regrepeat with op code %d\n", OP(p));
#endif
	थांबा;
    }

    rex.आदान = scan;

    परत (पूर्णांक)मोजणी;
}

/*
 * Push an item onto the regstack.
 * Returns pointer to new item.  Returns NULL when out of memory.
 */
    स्थिर regitem_T *
regstack_ढकला(regstate_T state, char_u *scan)
{
    regitem_T	*rp;

    जर ((दीर्घ)((अचिन्हित)regstack.ga_len >> 10) >= p_mmp)
    {
	emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
	परत रिक्त;
    }
    जर (GA_GROW_FAILS(&regstack, (पूर्णांक)आकार(regitem_T)))
	परत रिक्त;

    rp = (regitem_T *)((अक्षर *)regstack.ga_data + regstack.ga_len);
    rp->rs_state = state;
    rp->rs_scan = scan;

    regstack.ga_len += आकार(regitem_T);
    परत rp;
}

/*
 * Pop an item from the regstack.
 */
    स्थिर शून्य
regstack_काढा(char_u **scan)
{
    regitem_T	*rp;

    rp = (regitem_T *)((अक्षर *)regstack.ga_data + regstack.ga_len) - 1;
    *scan = rp->rs_scan;

    regstack.ga_len -= आकार(regitem_T);
}

#ifdef FEAT_RELTIME
/*
 * Check if the timer expired, return TRUE if so.
 */
    स्थिर पूर्णांक
bt_did_time_out(पूर्णांक *timed_out)
{
    जर (*timeout_flag)
    {
	जर (timed_out != रिक्त)
	{
# ifdef FEAT_EVAL
	    जर (!*timed_out)
		ch_log(रिक्त, "BT regexp timed out");
# endif
	    *timed_out = TRUE;
	}
	परत TRUE;
    }
    परत FALSE;
}
#endif

/*
 * Save the current subexpr to "bp", so that they can be restored
 * later by restore_subexpr().
 */
    स्थिर शून्य
save_subexpr(regbehind_T *bp)
{
    पूर्णांक i;

    // When "rex.need_clear_subexpr" is set we don't need to save the values,
    // only remember that this flag needs to be set again when restoring.
    bp->save_need_clear_subexpr = rex.need_clear_subexpr;
    जर (rex.need_clear_subexpr)
	परत;

    साठी (i = 0; i < NSUBEXP; ++i)
    {
	जर (REG_MULTI)
	{
	    bp->save_start[i].se_u.pos = rex.reg_startpos[i];
	    bp->save_end[i].se_u.pos = rex.reg_endpos[i];
	}
	अन्यथा
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
    पूर्णांक i;

    // Only need to restore saved values when they are not to be cleared.
    rex.need_clear_subexpr = bp->save_need_clear_subexpr;
    जर (rex.need_clear_subexpr)
	परत;

    साठी (i = 0; i < NSUBEXP; ++i)
    {
	जर (REG_MULTI)
	{
	    rex.reg_startpos[i] = bp->save_start[i].se_u.pos;
	    rex.reg_endpos[i] = bp->save_end[i].se_u.pos;
	}
	अन्यथा
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
    स्थिर पूर्णांक
regmatch(
    char_u	*scan,		    // Current node.
    पूर्णांक		*timed_out UNUSED)  // flag set on timeout or NULL
{
  char_u	*next;		// Next node.
  पूर्णांक		op;
  पूर्णांक		c;
  regitem_T	*rp;
  पूर्णांक		no;
  पूर्णांक		status;		// one of the RA_ values:

  // Make "regstack" and "backpos" empty.  They are allocated and freed in
  // bt_regexec_both() to reduce malloc()/free() calls.
  regstack.ga_len = 0;
  backpos.ga_len = 0;

  // Repeat until "regstack" is empty.
  साठी (;;)
  {
    // Some patterns may take a long time to match, e.g., "\([a-z]\+\)\+Q".
    // Allow interrupting them with CTRL-C.
    fast_breakcheck();

#ifdef DEBUG
    जर (scan != रिक्त && regnarrate)
    {
	mch_errmsg((अक्षर *)regprop(scan));
	mch_errmsg("(\n");
    }
#endif

    // Repeat for items that can be matched sequentially, without using the
    // regstack.
    साठी (;;)
    {
	जर (got_int || scan == रिक्त)
	{
	    status = RA_FAIL;
	    थांबा;
	}
#ifdef FEAT_RELTIME
	जर (bt_did_time_out(timed_out))
	{
	    status = RA_FAIL;
	    थांबा;
	}
#endif
	status = RA_CONT;

#ifdef DEBUG
	जर (regnarrate)
	{
	    mch_errmsg((अक्षर *)regprop(scan));
	    mch_errmsg("...\n");
# ifdef FEAT_SYN_HL
	    जर (re_extmatch_in != रिक्त)
	    {
		पूर्णांक i;

		mch_errmsg(_("External submatches:\n"));
		साठी (i = 0; i < NSUBEXP; i++)
		{
		    mch_errmsg("    \"");
		    जर (re_extmatch_in->matches[i] != रिक्त)
			mch_errmsg((अक्षर *)re_extmatch_in->matches[i]);
		    mch_errmsg("\"\n");
		}
	    }
# endif
	}
#endif
	next = regnext(scan);

	op = OP(scan);
	// Check for character class with NL added.
	जर (!rex.reg_ओळ_lbr && WITH_NL(op) && REG_MULTI
			     && *rex.आदान == NUL && rex.lnum <= rex.reg_maxline)
	{
	    reg_nextline();
	}
	अन्यथा जर (rex.reg_ओळ_lbr && WITH_NL(op) && *rex.आदान == '\n')
	{
	    ADVANCE_REGINPUT();
	}
	अन्यथा
	{
	  जर (WITH_NL(op))
	      op -= ADD_NL;
	  जर (has_mbyte)
	      c = (*mb_ptr2char)(rex.आदान);
	  अन्यथा
	      c = *rex.आदान;
	  निवड (op)
	  {
	  स्थिती BOL:
	    जर (rex.आदान != rex.ओळ)
		status = RA_NOMATCH;
	    थांबा;

	  स्थिती EOL:
	    जर (c != NUL)
		status = RA_NOMATCH;
	    थांबा;

	  स्थिती RE_BOF:
	    // We're not at the beginning of the file when below the first
	    // line where we started, not at the start of the line or we
	    // didn't start at the first line of the buffer.
	    जर (rex.lnum != 0 || rex.आदान != rex.ओळ
				       || (REG_MULTI && rex.reg_firstlnum > 1))
		status = RA_NOMATCH;
	    थांबा;

	  स्थिती RE_EOF:
	    जर (rex.lnum != rex.reg_maxline || c != NUL)
		status = RA_NOMATCH;
	    थांबा;

	  स्थिती CURSOR:
	    // Check if the buffer is in a window and compare the
	    // rex.reg_win->w_cursor position to the match position.
	    जर (rex.reg_win == रिक्त
		    || (rex.lnum + rex.reg_firstlnum
						 != rex.reg_win->w_cursor.lnum)
		    || ((colnr_T)(rex.आदान - rex.ओळ)
						 != rex.reg_win->w_cursor.col))
		status = RA_NOMATCH;
	    थांबा;

	  स्थिती RE_MARK:
	    // Compare the mark position to the match position.
	    {
		पूर्णांक	mark = OPERAND(scan)[0];
		पूर्णांक	cmp = OPERAND(scan)[1];
		pos_T	*pos;
		माप_प्रकार	col = REG_MULTI ? rex.आदान - rex.ओळ : 0;

		pos = getmark_buf(rex.reg_buf, mark, FALSE);

		// Line may have been freed, get it again.
		जर (REG_MULTI)
		{
		    rex.ओळ = reg_getline(rex.lnum);
		    rex.आदान = rex.ओळ + col;
		}

		जर (pos == रिक्त		     // mark doesn't exist
			|| pos->lnum <= 0)   // mark isn't set in reg_buf
		{
		    status = RA_NOMATCH;
		}
		अन्यथा
		{
		    colnr_T pos_col = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->col == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->col;

		    जर ((pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_col == (colnr_T)(rex.आदान - rex.ओळ)
				    ? (cmp == '<' || cmp == '>')
				    : (pos_col < (colnr_T)(rex.आदान - rex.ओळ)
					? cmp != '>'
					: cmp != '<'))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? cmp != '>'
				    : cmp != '<')))
		    status = RA_NOMATCH;
		}
	    }
	    थांबा;

	  स्थिती RE_VISUAL:
	    जर (!reg_match_visual())
		status = RA_NOMATCH;
	    थांबा;

	  स्थिती RE_LNUM:
	    जर (!REG_MULTI || !re_अंक_cmp((long_u)(rex.lnum + rex.reg_firstlnum),
									scan))
		status = RA_NOMATCH;
	    थांबा;

	  स्थिती RE_COL:
	    जर (!re_अंक_cmp((long_u)(rex.आदान - rex.ओळ) + 1, scan))
		status = RA_NOMATCH;
	    थांबा;

	  स्थिती RE_VCOL:
	    {
		win_T	    *wp = rex.reg_win == रिक्त ? curwin : rex.reg_win;
		linenr_T    lnum = REG_MULTI ? rex.reg_firstlnum + rex.lnum : 1;
		long_u	    vcol;

		जर (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_buffer->b_ml.ml_ओळ_मोजणी))
		    lnum = 1;
		vcol = (long_u)win_linetabsize(wp, lnum, rex.ओळ,
					      (colnr_T)(rex.आदान - rex.ओळ));
		जर (!re_अंक_cmp(vcol + 1, scan))
		    status = RA_NOMATCH;
	    }
	    थांबा;

	  स्थिती BOW:	// \<word; rex.input points to w
	    जर (c == NUL)	// Can't match at end of line
		status = RA_NOMATCH;
	    अन्यथा जर (has_mbyte)
	    {
		पूर्णांक this_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.आदान, rex.reg_buf);
		जर (this_class <= 1)
		    status = RA_NOMATCH;  // not on a word at all
		अन्यथा जर (reg_prev_class() == this_class)
		    status = RA_NOMATCH;  // previous char is in same word
	    }
	    अन्यथा
	    {
		जर (!vim_iswordc_buf(c, rex.reg_buf) || (rex.आदान > rex.ओळ
				&& vim_iswordc_buf(rex.आदान[-1], rex.reg_buf)))
		    status = RA_NOMATCH;
	    }
	    थांबा;

	  स्थिती EOW:	// word\>; rex.input points after d
	    जर (rex.आदान == rex.ओळ)    // Can't match at start of line
		status = RA_NOMATCH;
	    अन्यथा जर (has_mbyte)
	    {
		पूर्णांक this_class, prev_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.आदान, rex.reg_buf);
		prev_class = reg_prev_class();
		जर (this_class == prev_class
			|| prev_class == 0 || prev_class == 1)
		    status = RA_NOMATCH;
	    }
	    अन्यथा
	    {
		जर (!vim_iswordc_buf(rex.आदान[-1], rex.reg_buf)
			|| (rex.आदान[0] != NUL
					   && vim_iswordc_buf(c, rex.reg_buf)))
		    status = RA_NOMATCH;
	    }
	    थांबा; // Matched with EOW

	  स्थिती ANY:
	    // ANY does not match new lines.
	    जर (c == NUL)
		status = RA_NOMATCH;
	    अन्यथा
		ADVANCE_REGINPUT();
	    थांबा;

	  स्थिती IDENT:
	    जर (!vim_isIDc(c))
		status = RA_NOMATCH;
	    अन्यथा
		ADVANCE_REGINPUT();
	    थांबा;

	  स्थिती SIDENT:
	    जर (VIM_ISDIGIT(*rex.आदान) || !vim_isIDc(c))
		status = RA_NOMATCH;
	    अन्यथा
		ADVANCE_REGINPUT();
	    थांबा;

	  स्थिती KWORD:
	    जर (!vim_iswordp_buf(rex.आदान, rex.reg_buf))
		status = RA_NOMATCH;
	    अन्यथा
		ADVANCE_REGINPUT();
	    थांबा;

	  स्थिती SKWORD:
	    जर (VIM_ISDIGIT(*rex.आदान)
				    || !vim_iswordp_buf(rex.आदान, rex.reg_buf))
		status = RA_NOMATCH;
	    अन्यथा
		ADVANCE_REGINPUT();
	    थांबा;

	  स्थिती FNAME:
	    जर (!vim_isfilec(c))
		status = RA_NOMATCH;
	    अन्यथा
		ADVANCE_REGINPUT();
	    थांबा;

	  स्थिती SFNAME:
	    जर (VIM_ISDIGIT(*rex.आदान) || !vim_isfilec(c))
		status = RA_NOMATCH;
	    अन्यथा
		ADVANCE_REGINPUT();
	    थांबा;

	  स्थिती PRINT:
	    जर (!vim_isprintc(PTR2CHAR(rex.आदान)))
		status = RA_NOMATCH;
	    अन्यथा
		ADVANCE_REGINPUT();
	    थांबा;

	  स्थिती SPRINT:
	    जर (VIM_ISDIGIT(*rex.आदान) || !vim_isprintc(PTR2CHAR(rex.आदान)))
		status = RA_NOMATCH;
	    अन्यथा
		ADVANCE_REGINPUT();
	    थांबा;

	  स्थिती WHITE:
	    जर (!VIM_ISWHITE(c))
		status = RA_NOMATCH;
	    अन्यथा
		ADVANCE_REGINPUT();
	    थांबा;

	  स्थिती NWHITE:
	    जर (c == NUL || VIM_ISWHITE(c))
		status = RA_NOMATCH;
	    अन्यथा
		ADVANCE_REGINPUT();
	    थांबा;

	  स्थिती DIGIT:
	    जर (!ri_digit(c))
		status = RA_NOMATCH;
	    अन्यथा
		ADVANCE_REGINPUT();
	    थांबा;

	  स्थिती NDIGIT:
	    जर (c == NUL || ri_digit(c))
		status = RA_NOMATCH;
	    अन्यथा
		ADVANCE_REGINPUT();
	    थांबा;

	  स्थिती HEX:
	    जर (!ri_hex(c))
		status = RA_NOMATCH;
	    अन्यथा
		ADVANCE_REGINPUT();
	    थांबा;

	  स्थिती NHEX:
	    जर (c == NUL || ri_hex(c))
		status = RA_NOMATCH;
	    अन्यथा
		ADVANCE_REGINPUT();
	    थांबा;

	  स्थिती OCTAL:
	    जर (!ri_octal(c))
		status = RA_NOMATCH;
	    अन्यथा
		ADVANCE_REGINPUT();
	    थांबा;

	  स्थिती NOCTAL:
	    जर (c == NUL || ri_octal(c))
		status = RA_NOMATCH;
	    अन्यथा
		ADVANCE_REGINPUT();
	    थांबा;

	  स्थिती WORD:
	    जर (!ri_word(c))
		status = RA_NOMATCH;
	    अन्यथा
		ADVANCE_REGINPUT();
	    थांबा;

	  स्थिती NWORD:
	    जर (c == NUL || ri_word(c))
		status = RA_NOMATCH;
	    अन्यथा
		ADVANCE_REGINPUT();
	    थांबा;

	  स्थिती HEAD:
	    जर (!ri_head(c))
		status = RA_NOMATCH;
	    अन्यथा
		ADVANCE_REGINPUT();
	    थांबा;

	  स्थिती NHEAD:
	    जर (c == NUL || ri_head(c))
		status = RA_NOMATCH;
	    अन्यथा
		ADVANCE_REGINPUT();
	    थांबा;

	  स्थिती ALPHA:
	    जर (!ri_alpha(c))
		status = RA_NOMATCH;
	    अन्यथा
		ADVANCE_REGINPUT();
	    थांबा;

	  स्थिती NALPHA:
	    जर (c == NUL || ri_alpha(c))
		status = RA_NOMATCH;
	    अन्यथा
		ADVANCE_REGINPUT();
	    थांबा;

	  स्थिती LOWER:
	    जर (!ri_lower(c))
		status = RA_NOMATCH;
	    अन्यथा
		ADVANCE_REGINPUT();
	    थांबा;

	  स्थिती NLOWER:
	    जर (c == NUL || ri_lower(c))
		status = RA_NOMATCH;
	    अन्यथा
		ADVANCE_REGINPUT();
	    थांबा;

	  स्थिती UPPER:
	    जर (!ri_upper(c))
		status = RA_NOMATCH;
	    अन्यथा
		ADVANCE_REGINPUT();
	    थांबा;

	  स्थिती NUPPER:
	    जर (c == NUL || ri_upper(c))
		status = RA_NOMATCH;
	    अन्यथा
		ADVANCE_REGINPUT();
	    थांबा;

	  स्थिती EXACTLY:
	    {
		पूर्णांक	len;
		char_u	*opnd;

		opnd = OPERAND(scan);
		// Inline the first byte, for speed.
		जर (*opnd != *rex.आदान
			&& (!rex.reg_ic
			    || (!enc_utf8
			      && MB_TOLOWER(*opnd) != MB_TOLOWER(*rex.आदान))))
		    status = RA_NOMATCH;
		अन्यथा जर (*opnd == NUL)
		{
		    // match empty string always works; happens when "~" is
		    // empty.
		}
		अन्यथा
		{
		    जर (opnd[1] == NUL && !(enc_utf8 && rex.reg_ic))
		    {
			len = 1;	// matched a single byte above
		    }
		    अन्यथा
		    {
			// Need to match first byte again for multi-byte.
			len = (पूर्णांक)STRLEN(opnd);
			जर (cstrncmp(opnd, rex.आदान, &len) != 0)
			    status = RA_NOMATCH;
		    }
		    // Check for following composing character, unless %C
		    // follows (skips over all composing chars).
		    जर (status != RA_NOMATCH
			    && enc_utf8
			    && UTF_COMPOSINGLIKE(rex.आदान, rex.आदान + len)
			    && !rex.reg_icombine
			    && OP(next) != RE_COMPOSING)
		    {
			// raaron: This code makes a composing character get
			// ignored, which is the correct behavior (sometimes)
			// for voweled Hebrew texts.
			status = RA_NOMATCH;
		    }
		    जर (status != RA_NOMATCH)
			rex.आदान += len;
		}
	    }
	    थांबा;

	  स्थिती ANYOF:
	  स्थिती ANYBUT:
	    {
		char_u  *q = OPERAND(scan);

		जर (c == NUL)
		    status = RA_NOMATCH;
		अन्यथा जर ((cstrchr(q, c) == रिक्त) == (op == ANYOF))
		    status = RA_NOMATCH;
		अन्यथा
		{
		    // Check following combining characters
		    पूर्णांक	len = 0;
		    पूर्णांक i;

		    जर (enc_utf8)
			len = utfc_ptr2len(q) - utf_ptr2len(q);

		    MB_CPTR_ADV(rex.आदान);
		    MB_CPTR_ADV(q);

		    जर (!enc_utf8 || len == 0)
			थांबा;

		    साठी (i = 0; i < len; ++i)
			जर (q[i] != rex.आदान[i])
			{
			    status = RA_NOMATCH;
			    थांबा;
			}
		    rex.आदान += len;
		}
		थांबा;
	    }

	  स्थिती MULTIBYTECODE:
	    जर (has_mbyte)
	    {
		पूर्णांक	i, len;
		char_u	*opnd;
		पूर्णांक	opndc = 0, inpc;

		opnd = OPERAND(scan);
		// Safety check (just in case 'encoding' was changed since
		// compiling the program).
		जर ((len = (*mb_ptr2len)(opnd)) < 2)
		{
		    status = RA_NOMATCH;
		    थांबा;
		}
		जर (enc_utf8)
		    opndc = utf_ptr2char(opnd);
		जर (enc_utf8 && utf_iscomposing(opndc))
		{
		    // When only a composing char is given match at any
		    // position where that composing char appears.
		    status = RA_NOMATCH;
		    साठी (i = 0; rex.आदान[i] != NUL;
						i += utf_ptr2len(rex.आदान + i))
		    {
			inpc = utf_ptr2char(rex.आदान + i);
			जर (!utf_iscomposing(inpc))
			{
			    जर (i > 0)
				थांबा;
			}
			अन्यथा जर (opndc == inpc)
			{
			    // Include all following composing chars.
			    len = i + utfc_ptr2len(rex.आदान + i);
			    status = RA_MATCH;
			    थांबा;
			}
		    }
		}
		अन्यथा जर (enc_utf8)
		{
		    जर (cstrncmp(opnd, rex.आदान, &len) != 0)
		    {
			status = RA_NOMATCH;
			थांबा;
		    }
		}
		अन्यथा
		    साठी (i = 0; i < len; ++i)
			जर (opnd[i] != rex.आदान[i])
			{
			    status = RA_NOMATCH;
			    थांबा;
			}
		rex.आदान += len;
	    }
	    अन्यथा
		status = RA_NOMATCH;
	    थांबा;
	  स्थिती RE_COMPOSING:
	    जर (enc_utf8)
	    {
		// Skip composing characters.
		पर्यंत (utf_iscomposing(utf_ptr2char(rex.आदान)))
		    MB_CPTR_ADV(rex.आदान);
	    }
	    थांबा;

	  स्थिती NOTHING:
	    थांबा;

	  स्थिती BACK:
	    {
		पूर्णांक		i;
		backpos_T	*bp;

		// When we run into BACK we need to check if we don't keep
		// looping without matching any input.  The second and later
		// times a BACK is encountered it fails if the input is still
		// at the same position as the previous time.
		// The positions are stored in "backpos" and found by the
		// current value of "scan", the position in the RE program.
		bp = (backpos_T *)backpos.ga_data;
		साठी (i = 0; i < backpos.ga_len; ++i)
		    जर (bp[i].bp_scan == scan)
			थांबा;
		जर (i == backpos.ga_len)
		{
		    // First time at this BACK, make room to store the pos.
		    जर (GA_GROW_FAILS(&backpos, 1))
			status = RA_FAIL;
		    अन्यथा
		    {
			// get "ga_data" again, it may have changed
			bp = (backpos_T *)backpos.ga_data;
			bp[i].bp_scan = scan;
			++backpos.ga_len;
		    }
		}
		अन्यथा जर (reg_save_equal(&bp[i].bp_pos))
		    // Still at same position as last time, fail.
		    status = RA_NOMATCH;

		जर (status != RA_FAIL && status != RA_NOMATCH)
		    reg_save(&bp[i].bp_pos, &backpos);
	    }
	    थांबा;

	  स्थिती MOPEN + 0:   // Match start: \zs
	  स्थिती MOPEN + 1:   // \(
	  स्थिती MOPEN + 2:
	  स्थिती MOPEN + 3:
	  स्थिती MOPEN + 4:
	  स्थिती MOPEN + 5:
	  स्थिती MOPEN + 6:
	  स्थिती MOPEN + 7:
	  स्थिती MOPEN + 8:
	  स्थिती MOPEN + 9:
	    {
		no = op - MOPEN;
		cleanup_subexpr();
		rp = regstack_ढकला(RS_MOPEN, scan);
		जर (rp == रिक्त)
		    status = RA_FAIL;
		अन्यथा
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_startpos[no],
							  &rex.reg_startp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    थांबा;

	  स्थिती NOPEN:	    // \%(
	  स्थिती NCLOSE:	    // \) after \%(
		जर (regstack_ढकला(RS_NOPEN, scan) == रिक्त)
		    status = RA_FAIL;
		// We simply continue and handle the result when done.
		थांबा;

#ifdef FEAT_SYN_HL
	  स्थिती ZOPEN + 1:
	  स्थिती ZOPEN + 2:
	  स्थिती ZOPEN + 3:
	  स्थिती ZOPEN + 4:
	  स्थिती ZOPEN + 5:
	  स्थिती ZOPEN + 6:
	  स्थिती ZOPEN + 7:
	  स्थिती ZOPEN + 8:
	  स्थिती ZOPEN + 9:
	    {
		no = op - ZOPEN;
		cleanup_zsubexpr();
		rp = regstack_ढकला(RS_ZOPEN, scan);
		जर (rp == रिक्त)
		    status = RA_FAIL;
		अन्यथा
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_startzpos[no],
							     &reg_startzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    थांबा;
#endif

	  स्थिती MCLOSE + 0:  // Match end: \ze
	  स्थिती MCLOSE + 1:  // \)
	  स्थिती MCLOSE + 2:
	  स्थिती MCLOSE + 3:
	  स्थिती MCLOSE + 4:
	  स्थिती MCLOSE + 5:
	  स्थिती MCLOSE + 6:
	  स्थिती MCLOSE + 7:
	  स्थिती MCLOSE + 8:
	  स्थिती MCLOSE + 9:
	    {
		no = op - MCLOSE;
		cleanup_subexpr();
		rp = regstack_ढकला(RS_MCLOSE, scan);
		जर (rp == रिक्त)
		    status = RA_FAIL;
		अन्यथा
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_endpos[no],
							    &rex.reg_endp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    थांबा;

#ifdef FEAT_SYN_HL
	  स्थिती ZCLOSE + 1:  // \) after \z(
	  स्थिती ZCLOSE + 2:
	  स्थिती ZCLOSE + 3:
	  स्थिती ZCLOSE + 4:
	  स्थिती ZCLOSE + 5:
	  स्थिती ZCLOSE + 6:
	  स्थिती ZCLOSE + 7:
	  स्थिती ZCLOSE + 8:
	  स्थिती ZCLOSE + 9:
	    {
		no = op - ZCLOSE;
		cleanup_zsubexpr();
		rp = regstack_ढकला(RS_ZCLOSE, scan);
		जर (rp == रिक्त)
		    status = RA_FAIL;
		अन्यथा
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_endzpos[no],
							      &reg_endzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    थांबा;
#endif

	  स्थिती BACKREF + 1:
	  स्थिती BACKREF + 2:
	  स्थिती BACKREF + 3:
	  स्थिती BACKREF + 4:
	  स्थिती BACKREF + 5:
	  स्थिती BACKREF + 6:
	  स्थिती BACKREF + 7:
	  स्थिती BACKREF + 8:
	  स्थिती BACKREF + 9:
	    {
		पूर्णांक		len;

		no = op - BACKREF;
		cleanup_subexpr();
		जर (!REG_MULTI)		// Single-line regexp
		{
		    जर (rex.reg_startp[no] == रिक्त || rex.reg_endp[no] == रिक्त)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    अन्यथा
		    {
			// Compare current input with back-ref in the same
			// line.
			len = (पूर्णांक)(rex.reg_endp[no] - rex.reg_startp[no]);
			जर (cstrncmp(rex.reg_startp[no], rex.आदान, &len) != 0)
			    status = RA_NOMATCH;
		    }
		}
		अन्यथा				// Multi-line regexp
		{
		    जर (rex.reg_startpos[no].lnum < 0
						|| rex.reg_endpos[no].lnum < 0)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    अन्यथा
		    {
			जर (rex.reg_startpos[no].lnum == rex.lnum
				&& rex.reg_endpos[no].lnum == rex.lnum)
			{
			    // Compare back-ref within the current line.
			    len = rex.reg_endpos[no].col
						    - rex.reg_startpos[no].col;
			    जर (cstrncmp(rex.ओळ + rex.reg_startpos[no].col,
							  rex.आदान, &len) != 0)
				status = RA_NOMATCH;
			}
			अन्यथा
			{
			    // Messy situation: Need to compare between two
			    // lines.
			    पूर्णांक r = match_with_backref(
					    rex.reg_startpos[no].lnum,
					    rex.reg_startpos[no].col,
					    rex.reg_endpos[no].lnum,
					    rex.reg_endpos[no].col,
					    &len);

			    जर (r != RA_MATCH)
				status = r;
			}
		    }
		}

		// Matched the backref, skip over it.
		rex.आदान += len;
	    }
	    थांबा;

#ifdef FEAT_SYN_HL
	  स्थिती ZREF + 1:
	  स्थिती ZREF + 2:
	  स्थिती ZREF + 3:
	  स्थिती ZREF + 4:
	  स्थिती ZREF + 5:
	  स्थिती ZREF + 6:
	  स्थिती ZREF + 7:
	  स्थिती ZREF + 8:
	  स्थिती ZREF + 9:
	    {
		पूर्णांक	len;

		cleanup_zsubexpr();
		no = op - ZREF;
		जर (re_extmatch_in != रिक्त
			&& re_extmatch_in->matches[no] != रिक्त)
		{
		    len = (पूर्णांक)STRLEN(re_extmatch_in->matches[no]);
		    जर (cstrncmp(re_extmatch_in->matches[no],
							  rex.आदान, &len) != 0)
			status = RA_NOMATCH;
		    अन्यथा
			rex.आदान += len;
		}
		अन्यथा
		{
		    // Backref was not set: Match an empty string.
		}
	    }
	    थांबा;
#endif

	  स्थिती BRANCH:
	    {
		जर (OP(next) != BRANCH) // No choice.
		    next = OPERAND(scan);	// Avoid recursion.
		अन्यथा
		{
		    rp = regstack_ढकला(RS_BRANCH, scan);
		    जर (rp == रिक्त)
			status = RA_FAIL;
		    अन्यथा
			status = RA_BREAK;	// rest is below
		}
	    }
	    थांबा;

	  स्थिती BRACE_LIMITS:
	    {
		जर (OP(next) == BRACE_SIMPLE)
		{
		    bl_minval = OPERAND_MIN(scan);
		    bl_maxval = OPERAND_MAX(scan);
		}
		अन्यथा जर (OP(next) >= BRACE_COMPLEX
			&& OP(next) < BRACE_COMPLEX + 10)
		{
		    no = OP(next) - BRACE_COMPLEX;
		    brace_min[no] = OPERAND_MIN(scan);
		    brace_max[no] = OPERAND_MAX(scan);
		    brace_मोजणी[no] = 0;
		}
		अन्यथा
		{
		    internal_error("BRACE_LIMITS");
		    status = RA_FAIL;
		}
	    }
	    थांबा;

	  स्थिती BRACE_COMPLEX + 0:
	  स्थिती BRACE_COMPLEX + 1:
	  स्थिती BRACE_COMPLEX + 2:
	  स्थिती BRACE_COMPLEX + 3:
	  स्थिती BRACE_COMPLEX + 4:
	  स्थिती BRACE_COMPLEX + 5:
	  स्थिती BRACE_COMPLEX + 6:
	  स्थिती BRACE_COMPLEX + 7:
	  स्थिती BRACE_COMPLEX + 8:
	  स्थिती BRACE_COMPLEX + 9:
	    {
		no = op - BRACE_COMPLEX;
		++brace_मोजणी[no];

		// If not matched enough times yet, try one more
		जर (brace_मोजणी[no] <= (brace_min[no] <= brace_max[no]
					     ? brace_min[no] : brace_max[no]))
		{
		    rp = regstack_ढकला(RS_BRCPLX_MORE, scan);
		    जर (rp == रिक्त)
			status = RA_FAIL;
		    अन्यथा
		    {
			rp->rs_no = no;
			reg_save(&rp->rs_un.regsave, &backpos);
			next = OPERAND(scan);
			// We continue and handle the result when done.
		    }
		    थांबा;
		}

		// If matched enough times, may try matching some more
		जर (brace_min[no] <= brace_max[no])
		{
		    // Range is the normal way around, use longest match
		    जर (brace_मोजणी[no] <= brace_max[no])
		    {
			rp = regstack_ढकला(RS_BRCPLX_LONG, scan);
			जर (rp == रिक्त)
			    status = RA_FAIL;
			अन्यथा
			{
			    rp->rs_no = no;
			    reg_save(&rp->rs_un.regsave, &backpos);
			    next = OPERAND(scan);
			    // We continue and handle the result when done.
			}
		    }
		}
		अन्यथा
		{
		    // Range is backwards, use shortest match first
		    जर (brace_मोजणी[no] <= brace_min[no])
		    {
			rp = regstack_ढकला(RS_BRCPLX_SHORT, scan);
			जर (rp == रिक्त)
			    status = RA_FAIL;
			अन्यथा
			{
			    reg_save(&rp->rs_un.regsave, &backpos);
			    // We continue and handle the result when done.
			}
		    }
		}
	    }
	    थांबा;

	  स्थिती BRACE_SIMPLE:
	  स्थिती STAR:
	  स्थिती PLUS:
	    {
		regstar_T	rst;

		// Lookahead to avoid useless match attempts when we know
		// what character comes next.
		जर (OP(next) == EXACTLY)
		{
		    rst.nextb = *OPERAND(next);
		    जर (rex.reg_ic)
		    {
			जर (MB_ISUPPER(rst.nextb))
			    rst.nextb_ic = MB_TOLOWER(rst.nextb);
			अन्यथा
			    rst.nextb_ic = MB_TOUPPER(rst.nextb);
		    }
		    अन्यथा
			rst.nextb_ic = rst.nextb;
		}
		अन्यथा
		{
		    rst.nextb = NUL;
		    rst.nextb_ic = NUL;
		}
		जर (op != BRACE_SIMPLE)
		{
		    rst.minval = (op == STAR) ? 0 : 1;
		    rst.maxval = MAX_LIMIT;
		}
		अन्यथा
		{
		    rst.minval = bl_minval;
		    rst.maxval = bl_maxval;
		}

		// When maxval > minval, try matching as much as possible, up
		// to maxval.  When maxval < minval, try matching at least the
		// minimal number (since the range is backwards, that's also
		// maxval!).
		rst.मोजणी = regrepeat(OPERAND(scan), rst.maxval);
		जर (got_int)
		{
		    status = RA_FAIL;
		    थांबा;
		}
		जर (rst.minval <= rst.maxval
			  ? rst.मोजणी >= rst.minval : rst.मोजणी >= rst.maxval)
		{
		    // It could match.  Prepare for trying to match what
		    // follows.  The code is below.  Parameters are stored in
		    // a regstar_T on the regstack.
		    जर ((दीर्घ)((अचिन्हित)regstack.ga_len >> 10) >= p_mmp)
		    {
			emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
			status = RA_FAIL;
		    }
		    अन्यथा जर (GA_GROW_FAILS(&regstack, (पूर्णांक)आकार(regstar_T)))
			status = RA_FAIL;
		    अन्यथा
		    {
			regstack.ga_len += आकार(regstar_T);
			rp = regstack_ढकला(rst.minval <= rst.maxval
					? RS_STAR_LONG : RS_STAR_SHORT, scan);
			जर (rp == रिक्त)
			    status = RA_FAIL;
			अन्यथा
			{
			    *(((regstar_T *)rp) - 1) = rst;
			    status = RA_BREAK;	    // skip the restore bits
			}
		    }
		}
		अन्यथा
		    status = RA_NOMATCH;

	    }
	    थांबा;

	  स्थिती NOMATCH:
	  स्थिती MATCH:
	  स्थिती SUBPAT:
	    rp = regstack_ढकला(RS_NOMATCH, scan);
	    जर (rp == रिक्त)
		status = RA_FAIL;
	    अन्यथा
	    {
		rp->rs_no = op;
		reg_save(&rp->rs_un.regsave, &backpos);
		next = OPERAND(scan);
		// We continue and handle the result when done.
	    }
	    थांबा;

	  स्थिती BEHIND:
	  स्थिती NOBEHIND:
	    // Need a bit of room to store extra positions.
	    जर ((दीर्घ)((अचिन्हित)regstack.ga_len >> 10) >= p_mmp)
	    {
		emsg(_(e_pattern_uses_more_memory_than_maxmempattern));
		status = RA_FAIL;
	    }
	    अन्यथा जर (GA_GROW_FAILS(&regstack, (पूर्णांक)आकार(regbehind_T)))
		status = RA_FAIL;
	    अन्यथा
	    {
		regstack.ga_len += आकार(regbehind_T);
		rp = regstack_ढकला(RS_BEHIND1, scan);
		जर (rp == रिक्त)
		    status = RA_FAIL;
		अन्यथा
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
	    थांबा;

	  स्थिती BHPOS:
	    जर (REG_MULTI)
	    {
		जर (behind_pos.rs_u.pos.col != (colnr_T)(rex.आदान - rex.ओळ)
			|| behind_pos.rs_u.pos.lnum != rex.lnum)
		    status = RA_NOMATCH;
	    }
	    अन्यथा जर (behind_pos.rs_u.ptr != rex.आदान)
		status = RA_NOMATCH;
	    थांबा;

	  स्थिती NEWL:
	    जर ((c != NUL || !REG_MULTI || rex.lnum > rex.reg_maxline
			     || rex.reg_ओळ_lbr)
					   && (c != '\n' || !rex.reg_ओळ_lbr))
		status = RA_NOMATCH;
	    अन्यथा जर (rex.reg_ओळ_lbr)
		ADVANCE_REGINPUT();
	    अन्यथा
		reg_nextline();
	    थांबा;

	  स्थिती END:
	    status = RA_MATCH;	// Success!
	    थांबा;

	  पूर्वनिर्धारित:
	    iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	    म_लिखो("Illegal op code %d\n", op);
#endif
	    status = RA_FAIL;
	    थांबा;
	  }
	}

	// If we can't continue sequentially, break the inner loop.
	जर (status != RA_CONT)
	    थांबा;

	// Continue in inner loop, advance to next item.
	scan = next;

    } // end of inner loop

    // If there is something on the regstack execute the code for the state.
    // If the state is popped then loop and use the older state.
    पर्यंत (regstack.ga_len > 0 && status != RA_FAIL)
    {
	rp = (regitem_T *)((अक्षर *)regstack.ga_data + regstack.ga_len) - 1;
	निवड (rp->rs_state)
	{
	  स्थिती RS_NOPEN:
	    // Result is passed on as-is, simply pop the state.
	    regstack_काढा(&scan);
	    थांबा;

	  स्थिती RS_MOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    जर (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_startpos[rp->rs_no],
						  &rex.reg_startp[rp->rs_no]);
	    regstack_काढा(&scan);
	    थांबा;

#ifdef FEAT_SYN_HL
	  स्थिती RS_ZOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    जर (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_startzpos[rp->rs_no],
						 &reg_startzp[rp->rs_no]);
	    regstack_काढा(&scan);
	    थांबा;
#endif

	  स्थिती RS_MCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    जर (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_endpos[rp->rs_no],
						    &rex.reg_endp[rp->rs_no]);
	    regstack_काढा(&scan);
	    थांबा;

#ifdef FEAT_SYN_HL
	  स्थिती RS_ZCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    जर (status == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_endzpos[rp->rs_no],
						   &reg_endzp[rp->rs_no]);
	    regstack_काढा(&scan);
	    थांबा;
#endif

	  स्थिती RS_BRANCH:
	    जर (status == RA_MATCH)
		// this branch matched, use it
		regstack_काढा(&scan);
	    अन्यथा
	    {
		जर (status != RA_BREAK)
		{
		    // After a non-matching branch: try next one.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    scan = rp->rs_scan;
		}
		जर (scan == रिक्त || OP(scan) != BRANCH)
		{
		    // no more branches, didn't find a match
		    status = RA_NOMATCH;
		    regstack_काढा(&scan);
		}
		अन्यथा
		{
		    // Prepare to try a branch.
		    rp->rs_scan = regnext(scan);
		    reg_save(&rp->rs_un.regsave, &backpos);
		    scan = OPERAND(scan);
		}
	    }
	    थांबा;

	  स्थिती RS_BRCPLX_MORE:
	    // Pop the state.  Restore pointers when there is no match.
	    जर (status == RA_NOMATCH)
	    {
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_मोजणी[rp->rs_no];	// decrement match count
	    }
	    regstack_काढा(&scan);
	    थांबा;

	  स्थिती RS_BRCPLX_LONG:
	    // Pop the state.  Restore pointers when there is no match.
	    जर (status == RA_NOMATCH)
	    {
		// There was no match, but we did find enough matches.
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_मोजणी[rp->rs_no];
		// continue with the items after "\{}"
		status = RA_CONT;
	    }
	    regstack_काढा(&scan);
	    जर (status == RA_CONT)
		scan = regnext(scan);
	    थांबा;

	  स्थिती RS_BRCPLX_SHORT:
	    // Pop the state.  Restore pointers when there is no match.
	    जर (status == RA_NOMATCH)
		// There was no match, try to match one more item.
		reg_restore(&rp->rs_un.regsave, &backpos);
	    regstack_काढा(&scan);
	    जर (status == RA_NOMATCH)
	    {
		scan = OPERAND(scan);
		status = RA_CONT;
	    }
	    थांबा;

	  स्थिती RS_NOMATCH:
	    // Pop the state.  If the operand matches for NOMATCH or
	    // doesn't match for MATCH/SUBPAT, we fail.  Otherwise backup,
	    // except for SUBPAT, and continue with the next item.
	    जर (status == (rp->rs_no == NOMATCH ? RA_MATCH : RA_NOMATCH))
		status = RA_NOMATCH;
	    अन्यथा
	    {
		status = RA_CONT;
		जर (rp->rs_no != SUBPAT)	// zero-width
		    reg_restore(&rp->rs_un.regsave, &backpos);
	    }
	    regstack_काढा(&scan);
	    जर (status == RA_CONT)
		scan = regnext(scan);
	    थांबा;

	  स्थिती RS_BEHIND1:
	    जर (status == RA_NOMATCH)
	    {
		regstack_काढा(&scan);
		regstack.ga_len -= आकार(regbehind_T);
	    }
	    अन्यथा
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
	    थांबा;

	  स्थिती RS_BEHIND2:
	    // Looping for BEHIND / NOBEHIND match.
	    जर (status == RA_MATCH && reg_save_equal(&behind_pos))
	    {
		// found a match that ends where "next" started
		behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		जर (rp->rs_no == BEHIND)
		    reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
		अन्यथा
		{
		    // But we didn't want a match.  Need to restore the
		    // subexpr, because what follows matched, so they have
		    // been set.
		    status = RA_NOMATCH;
		    restore_subexpr(((regbehind_T *)rp) - 1);
		}
		regstack_काढा(&scan);
		regstack.ga_len -= आकार(regbehind_T);
	    }
	    अन्यथा
	    {
		दीर्घ limit;

		// No match or a match that doesn't end where we want it: Go
		// back one character.  May go to previous line once.
		no = OK;
		limit = OPERAND_MIN(rp->rs_scan);
		जर (REG_MULTI)
		{
		    जर (limit > 0
			    && ((rp->rs_un.regsave.rs_u.pos.lnum
						    < behind_pos.rs_u.pos.lnum
				    ? (colnr_T)STRLEN(rex.ओळ)
				    : behind_pos.rs_u.pos.col)
				- rp->rs_un.regsave.rs_u.pos.col >= limit))
			no = FAIL;
		    अन्यथा जर (rp->rs_un.regsave.rs_u.pos.col == 0)
		    {
			जर (rp->rs_un.regsave.rs_u.pos.lnum
					< behind_pos.rs_u.pos.lnum
				|| reg_getline(
					--rp->rs_un.regsave.rs_u.pos.lnum)
								  == रिक्त)
			    no = FAIL;
			अन्यथा
			{
			    reg_restore(&rp->rs_un.regsave, &backpos);
			    rp->rs_un.regsave.rs_u.pos.col =
						 (colnr_T)STRLEN(rex.ओळ);
			}
		    }
		    अन्यथा
		    {
			जर (has_mbyte)
			{
			    char_u *ओळ =
				  reg_getline(rp->rs_un.regsave.rs_u.pos.lnum);

			    rp->rs_un.regsave.rs_u.pos.col -=
				(*mb_head_off)(ओळ, ओळ
				    + rp->rs_un.regsave.rs_u.pos.col - 1) + 1;
			}
			अन्यथा
			    --rp->rs_un.regsave.rs_u.pos.col;
		    }
		}
		अन्यथा
		{
		    जर (rp->rs_un.regsave.rs_u.ptr == rex.ओळ)
			no = FAIL;
		    अन्यथा
		    {
			MB_PTR_BACK(rex.ओळ, rp->rs_un.regsave.rs_u.ptr);
			जर (limit > 0 && (दीर्घ)(behind_pos.rs_u.ptr
				     - rp->rs_un.regsave.rs_u.ptr) > limit)
			    no = FAIL;
		    }
		}
		जर (no == OK)
		{
		    // Advanced, prepare for finding match again.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    scan = OPERAND(rp->rs_scan) + 4;
		    जर (status == RA_MATCH)
		    {
			// We did match, so subexpr may have been changed,
			// need to restore them for the next try.
			status = RA_NOMATCH;
			restore_subexpr(((regbehind_T *)rp) - 1);
		    }
		}
		अन्यथा
		{
		    // Can't advance.  For NOBEHIND that's a match.
		    behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		    जर (rp->rs_no == NOBEHIND)
		    {
			reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
			status = RA_MATCH;
		    }
		    अन्यथा
		    {
			// We do want a proper match.  Need to restore the
			// subexpr if we had a match, because they may have
			// been set.
			जर (status == RA_MATCH)
			{
			    status = RA_NOMATCH;
			    restore_subexpr(((regbehind_T *)rp) - 1);
			}
		    }
		    regstack_काढा(&scan);
		    regstack.ga_len -= आकार(regbehind_T);
		}
	    }
	    थांबा;

	  स्थिती RS_STAR_LONG:
	  स्थिती RS_STAR_SHORT:
	    {
		regstar_T	    *rst = ((regstar_T *)rp) - 1;

		जर (status == RA_MATCH)
		{
		    regstack_काढा(&scan);
		    regstack.ga_len -= आकार(regstar_T);
		    थांबा;
		}

		// Tried once already, restore input pointers.
		जर (status != RA_BREAK)
		    reg_restore(&rp->rs_un.regsave, &backpos);

		// Repeat until we found a position where it could match.
		साठी (;;)
		{
		    जर (status != RA_BREAK)
		    {
			// Tried first position already, advance.
			जर (rp->rs_state == RS_STAR_LONG)
			{
			    // Trying for longest match, but couldn't or
			    // didn't match -- back up one char.
			    जर (--rst->मोजणी < rst->minval)
				थांबा;
			    जर (rex.आदान == rex.ओळ)
			    {
				// backup to last char of previous line
				जर (rex.lnum == 0)
				{
				    status = RA_NOMATCH;
				    थांबा;
				}
				--rex.lnum;
				rex.ओळ = reg_getline(rex.lnum);
				// Just in case regrepeat() didn't count
				// right.
				जर (rex.ओळ == रिक्त)
				    थांबा;
				rex.आदान = rex.ओळ + reg_getline_len(rex.lnum);
				fast_breakcheck();
			    }
			    अन्यथा
				MB_PTR_BACK(rex.ओळ, rex.आदान);
			}
			अन्यथा
			{
			    // Range is backwards, use shortest match first.
			    // Careful: maxval and minval are exchanged!
			    // Couldn't or didn't match: try advancing one
			    // char.
			    जर (rst->मोजणी == rst->minval
				  || regrepeat(OPERAND(rp->rs_scan), 1L) == 0)
				थांबा;
			    ++rst->मोजणी;
			}
			जर (got_int)
			    थांबा;
		    }
		    अन्यथा
			status = RA_NOMATCH;

		    // If it could match, try it.
		    जर (rst->nextb == NUL || *rex.आदान == rst->nextb
					     || *rex.आदान == rst->nextb_ic)
		    {
			reg_save(&rp->rs_un.regsave, &backpos);
			scan = regnext(rp->rs_scan);
			status = RA_CONT;
			थांबा;
		    }
		}
		जर (status != RA_CONT)
		{
		    // Failed.
		    regstack_काढा(&scan);
		    regstack.ga_len -= आकार(regstar_T);
		    status = RA_NOMATCH;
		}
	    }
	    थांबा;
	}

	// If we want to continue the inner loop or didn't pop a state
	// continue matching loop
	जर (status == RA_CONT || rp == (regitem_T *)
			     ((अक्षर *)regstack.ga_data + regstack.ga_len) - 1)
	    थांबा;

#ifdef FEAT_RELTIME
	जर (bt_did_time_out(timed_out))
	{
	    status = RA_FAIL;
	    थांबा;
	}
#endif
    }

    // May need to continue with the inner loop, starting at "scan".
    जर (status == RA_CONT)
	सुरूठेवा;

    // If the regstack is empty or something failed we are done.
    जर (regstack.ga_len == 0 || status == RA_FAIL)
    {
	जर (scan == रिक्त)
	{
	    // We get here only if there's trouble -- normally "case END" is
	    // the terminating point.
	    iemsg(e_corrupted_regexp_program);
#ifdef DEBUG
	    म_लिखो("Premature EOL\n");
#endif
	}
	परत (status == RA_MATCH);
    }

  } // End of loop until the regstack is empty.

  // NOTREACHED
}

/*
 * regtry - try match of "prog" with at rex.line["col"].
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    स्थिर दीर्घ
regtry(
    bt_regprog_T	*prog,
    colnr_T		col,
    पूर्णांक			*timed_out)	// flag set on timeout or NULL
{
    rex.आदान = rex.ओळ + col;
    rex.need_clear_subexpr = TRUE;
#ifdef FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    rex.need_clear_zsubexpr = (prog->reghasz == REX_SET);
#endif

    जर (regmatch(prog->program + 1, timed_out) == 0)
	परत 0;

    cleanup_subexpr();
    जर (REG_MULTI)
    {
	जर (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].col = col;
	}
	जर (rex.reg_endpos[0].lnum < 0)
	{
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].col = (पूर्णांक)(rex.आदान - rex.ओळ);
	}
	अन्यथा
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    अन्यथा
    {
	जर (rex.reg_startp[0] == रिक्त)
	    rex.reg_startp[0] = rex.ओळ + col;
	जर (rex.reg_endp[0] == रिक्त)
	    rex.reg_endp[0] = rex.आदान;
    }
#ifdef FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = रिक्त;

    जर (prog->reghasz == REX_SET)
    {
	पूर्णांक		i;

	cleanup_zsubexpr();
	re_extmatch_out = make_extmatch();
	जर (re_extmatch_out == रिक्त)
	    परत 0;
	साठी (i = 0; i < NSUBEXP; i++)
	{
	    जर (REG_MULTI)
	    {
		// Only accept single line matches.
		जर (reg_startzpos[i].lnum >= 0
			&& reg_endzpos[i].lnum == reg_startzpos[i].lnum
			&& reg_endzpos[i].col >= reg_startzpos[i].col)
		    re_extmatch_out->matches[i] =
			vim_strnsave(reg_getline(reg_startzpos[i].lnum)
						       + reg_startzpos[i].col,
				   reg_endzpos[i].col - reg_startzpos[i].col);
	    }
	    अन्यथा
	    {
		जर (reg_startzp[i] != रिक्त && reg_endzp[i] != रिक्त)
		    re_extmatch_out->matches[i] =
			    vim_strnsave(reg_startzp[i],
						reg_endzp[i] - reg_startzp[i]);
	    }
	}
    }
#endif
    परत 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    स्थिर दीर्घ
bt_regexec_both(
    char_u	*ओळ,
    colnr_T	startcol,	// column to start looking for match
    पूर्णांक		*timed_out)	// flag set on timeout or NULL
{
    bt_regprog_T    *prog;
    char_u	    *s;
    colnr_T	    col = startcol;
    दीर्घ	    retval = 0L;

    // Create "regstack" and "backpos" if they are not allocated yet.
    // We allocate *_INITIAL amount of bytes first and then set the grow size
    // to much bigger value to avoid many malloc calls in case of deep regular
    // expressions.
    जर (regstack.ga_data == रिक्त)
    {
	// Use an item size of 1 byte, since we push different things
	// onto the regstack.
	ga_init2(&regstack, 1, REGSTACK_INITIAL);
	(शून्य)ga_grow(&regstack, REGSTACK_INITIAL);
	regstack.ga_growsize = REGSTACK_INITIAL * 8;
    }

    जर (backpos.ga_data == रिक्त)
    {
	ga_init2(&backpos, आकार(backpos_T), BACKPOS_INITIAL);
	(शून्य)ga_grow(&backpos, BACKPOS_INITIAL);
	backpos.ga_growsize = BACKPOS_INITIAL * 8;
    }

    जर (REG_MULTI)
    {
	prog = (bt_regprog_T *)rex.reg_mmatch->regprog;
	ओळ = reg_getline((linenr_T)0);
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    अन्यथा
    {
	prog = (bt_regprog_T *)rex.reg_match->regprog;
	rex.reg_startp = rex.reg_match->startp;
	rex.reg_endp = rex.reg_match->endp;
    }

    // Be paranoid...
    जर (prog == रिक्त || ओळ == रिक्त)
    {
	iemsg(e_null_argument);
	जा theend;
    }

    // Check validity of program.
    जर (prog_magic_wrong())
	जा theend;

    // If the start column is past the maximum column: no need to try.
    जर (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	जा theend;

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    जर (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    अन्यथा जर (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    जर (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    // If there is a "must appear" string, look for it.
    जर (prog->regmust != रिक्त)
    {
	पूर्णांक c;

	जर (has_mbyte)
	    c = (*mb_ptr2char)(prog->regmust);
	अन्यथा
	    c = *prog->regmust;
	s = ओळ + col;

	// This is used very often, esp. for ":global".  Use three versions of
	// the loop to avoid overhead of conditions.
	जर (!rex.reg_ic && !has_mbyte)
	    पर्यंत ((s = vim_strbyte(s, c)) != रिक्त)
	    {
		पूर्णांक len = prog->regmlen;
		जर (cstrncmp(s, prog->regmust, &len) == 0)
		    थांबा;		// Found it.
		++s;
	    }
	अन्यथा जर (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	    पर्यंत ((s = vim_strchr(s, c)) != रिक्त)
	    {
		पूर्णांक len = prog->regmlen;
		जर (cstrncmp(s, prog->regmust, &len) == 0)
		    थांबा;		// Found it.
		MB_PTR_ADV(s);
	    }
	अन्यथा
	    पर्यंत ((s = cstrchr(s, c)) != रिक्त)
	    {
		पूर्णांक len = prog->regmlen;
		जर (cstrncmp(s, prog->regmust, &len) == 0)
		    थांबा;		// Found it.
		MB_PTR_ADV(s);
	    }
	जर (s == रिक्त)		// Not present.
	    जा theend;
    }

    rex.ओळ = ओळ;
    rex.lnum = 0;
    reg_toolong = FALSE;

    // Simplest case: Anchored match need be tried only once.
    जर (prog->reganch)
    {
	पूर्णांक	c;

	जर (has_mbyte)
	    c = (*mb_ptr2char)(rex.ओळ + col);
	अन्यथा
	    c = rex.ओळ[col];
	जर (prog->regstart == NUL
		|| prog->regstart == c
		|| (rex.reg_ic
		    && (((enc_utf8 && utf_fold(prog->regstart) == utf_fold(c)))
			|| (c < 255 && prog->regstart < 255 &&
			    MB_TOLOWER(prog->regstart) == MB_TOLOWER(c)))))
	    retval = regtry(prog, col, timed_out);
	अन्यथा
	    retval = 0;
    }
    अन्यथा
    {
	// Messy cases:  unanchored match.
	पर्यंत (!got_int)
	{
	    जर (prog->regstart != NUL)
	    {
		// Skip until the char we know it must start with.
		// Used often, do some work to avoid call overhead.
		जर (!rex.reg_ic && !has_mbyte)
		    s = vim_strbyte(rex.ओळ + col, prog->regstart);
		अन्यथा
		    s = cstrchr(rex.ओळ + col, prog->regstart);
		जर (s == रिक्त)
		{
		    retval = 0;
		    थांबा;
		}
		col = (पूर्णांक)(s - rex.ओळ);
	    }

	    // Check for maximum column to try.
	    जर (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	    {
		retval = 0;
		थांबा;
	    }

	    retval = regtry(prog, col, timed_out);
	    जर (retval > 0)
		थांबा;

	    // if not currently on the first line, get it again
	    जर (rex.lnum != 0)
	    {
		rex.lnum = 0;
		rex.ओळ = reg_getline((linenr_T)0);
	    }
	    जर (rex.ओळ[col] == NUL)
		थांबा;
	    जर (has_mbyte)
		col += (*mb_ptr2len)(rex.ओळ + col);
	    अन्यथा
		++col;
#ifdef FEAT_RELTIME
	    जर (bt_did_time_out(timed_out))
		थांबा;
#endif
	}
    }

theend:
    // Free "reg_tofree" when it's a bit big.
    // Free regstack and backpos if they are bigger than their initial size.
    जर (reg_tofreelen > 400)
	VIM_CLEAR(reg_tofree);
    जर (regstack.ga_maxlen > REGSTACK_INITIAL)
	ga_clear(&regstack);
    जर (backpos.ga_maxlen > BACKPOS_INITIAL)
	ga_clear(&backpos);

    जर (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs
	// and \ze are used.
	जर (REG_MULTI)
	{
	    lpos_T *start = &rex.reg_mmatch->startpos[0];
	    lpos_T *end = &rex.reg_mmatch->endpos[0];

	    जर (end->lnum < start->lnum
			|| (end->lnum == start->lnum && end->col < start->col))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_mmatch->rmm_matchcol = col;
	}
	अन्यथा
	{
	    जर (rex.reg_match->endp[0] < rex.reg_match->startp[0])
		rex.reg_match->endp[0] = rex.reg_match->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_match->rm_matchcol = col;
	}
    }

    परत retval;
}

/*
 * Match a regexp against a string.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 * if "line_lbr" is TRUE  consider a "\n" in "line" to be a line break.
 *
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    स्थिर पूर्णांक
bt_regexec_nl(
    regmatch_T	*rmp,
    char_u	*ओळ,	// string to match against
    colnr_T	col,	// column to start looking for match
    पूर्णांक		ओळ_lbr)
{
    rex.reg_match = rmp;
    rex.reg_mmatch = रिक्त;
    rex.reg_maxline = 0;
    rex.reg_ओळ_lbr = ओळ_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = रिक्त;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;

    परत bt_regexec_both(ओळ, col, रिक्त);
}

/*
 * Match a regexp against multiple lines.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 *
 * Return zero if there is no match.  Return number of lines contained in the
 * match otherwise.
 */
    स्थिर दीर्घ
bt_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	col,		// column to start looking for match
    पूर्णांक		*timed_out)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    परत bt_regexec_both(रिक्त, col, timed_out);
}

/*
 * Compare a number with the operand of RE_LNUM, RE_COL or RE_VCOL.
 */
    स्थिर पूर्णांक
re_अंक_cmp(long_u val, char_u *scan)
{
    long_u  n = OPERAND_MIN(scan);

    जर (OPERAND_CMP(scan) == '>')
	परत val > n;
    जर (OPERAND_CMP(scan) == '<')
	परत val < n;
    परत val == n;
}

#ifdef BT_REGEXP_DUMP

/*
 * regdump - dump a regexp onto stdout in vaguely comprehensible form
 */
    स्थिर शून्य
regdump(char_u *pattern, bt_regprog_T *r)
{
    char_u  *s;
    पूर्णांक	    op = EXACTLY;	// Arbitrary non-END op.
    char_u  *next;
    char_u  *end = रिक्त;
    खाता    *f;

# ifdef BT_REGEXP_LOG
    f = ख_खोलो("bt_regexp_log.log", "a");
# अन्यथा
    f = मानक_निकास;
# endif
    जर (f == रिक्त)
	परत;
    ख_लिखो(f, "-------------------------------------\n\r\nregcomp(%s):\r\n", pattern);

    s = r->program + 1;
    // Loop until we find the END that isn't before a referred next (an END
    // can also appear in a NOMATCH operand).
    पर्यंत ((op != END || s <= end) && s < r->program + r->regsz)
    {
	op = OP(s);
	ख_लिखो(f, "%2d%s", (पूर्णांक)(s - r->program), regprop(s)); // Where, what.
	next = (s + 3 <= r->program + r->regsz) ? regnext(s) : रिक्त;
	जर (next == रिक्त)	// Next ptr.
	    ख_लिखो(f, "(0)");
	अन्यथा
	    ख_लिखो(f, "(%d)", (पूर्णांक)((s - r->program) + (next - s)));
	जर (end < next)
	    end = next;
	जर (op == BRACE_LIMITS)
	{
	    // Two ints
	    ख_लिखो(f, " minval %ld, maxval %ld", OPERAND_MIN(s), OPERAND_MAX(s));
	    s += 8;
	}
	अन्यथा जर (op == BEHIND || op == NOBEHIND)
	{
	    // one int
	    ख_लिखो(f, " count %ld", OPERAND_MIN(s));
	    s += 4;
	}
	अन्यथा जर (op == RE_LNUM || op == RE_COL || op == RE_VCOL)
	{
	    // one int plus comparator
	    ख_लिखो(f, " count %ld", OPERAND_MIN(s));
	    s += 5;
	}
	s += 3;
	जर (op == MULTIBYTECODE)
	{
	    ख_लिखो(f, " mbc=%d", utf_ptr2char(s));
	    s += utfc_ptr2len(s);
	}
	जर (op == ANYOF || op == ANYOF + ADD_NL
		|| op == ANYBUT || op == ANYBUT + ADD_NL
		|| op == EXACTLY)
	{
	    // Literal string, where present.
	    ख_लिखो(f, "\nxxxxxxxxx\n");
	    पर्यंत (*s != NUL && s < r->program + r->regsz)
	    {
		ख_लिखो(f, "%c", *s);
		s += utfc_ptr2len(s);  // advance by full char including combining
	    }
	    ख_लिखो(f, "\nxxxxxxxxx\n");
	    s++;
	}
	ख_लिखो(f, "\r\n");
    }

    // Header fields of interest.
    जर (r->regstart != NUL)
	ख_लिखो(f, "start `%s' 0x%x; ", r->regstart < 256
		? (अक्षर *)transchar(r->regstart)
		: "multibyte", r->regstart);
    जर (r->reganch)
	ख_लिखो(f, "anchored; ");
    जर (r->regmust != रिक्त)
	ख_लिखो(f, "must have \"%s\"", r->regmust);
    ख_लिखो(f, "\r\n");

# ifdef BT_REGEXP_LOG
    ख_बंद(f);
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
    अक्षर	    *p;
    स्थिर अक्षर	    buf[50];
    स्थिर माप_प्रकार   buflen = 0;

    STRCPY(buf, ":");
    buflen = 1;

    निवड ((पूर्णांक) OP(op))
    {
      स्थिती BOL:
	p = "BOL";
	थांबा;
      स्थिती EOL:
	p = "EOL";
	थांबा;
      स्थिती RE_BOF:
	p = "BOF";
	थांबा;
      स्थिती RE_EOF:
	p = "EOF";
	थांबा;
      स्थिती CURSOR:
	p = "CURSOR";
	थांबा;
      स्थिती RE_VISUAL:
	p = "RE_VISUAL";
	थांबा;
      स्थिती RE_LNUM:
	p = "RE_LNUM";
	थांबा;
      स्थिती RE_MARK:
	p = "RE_MARK";
	थांबा;
      स्थिती RE_COL:
	p = "RE_COL";
	थांबा;
      स्थिती RE_VCOL:
	p = "RE_VCOL";
	थांबा;
      स्थिती BOW:
	p = "BOW";
	थांबा;
      स्थिती EOW:
	p = "EOW";
	थांबा;
      स्थिती ANY:
	p = "ANY";
	थांबा;
      स्थिती ANY + ADD_NL:
	p = "ANY+NL";
	थांबा;
      स्थिती ANYOF:
	p = "ANYOF";
	थांबा;
      स्थिती ANYOF + ADD_NL:
	p = "ANYOF+NL";
	थांबा;
      स्थिती ANYBUT:
	p = "ANYBUT";
	थांबा;
      स्थिती ANYBUT + ADD_NL:
	p = "ANYBUT+NL";
	थांबा;
      स्थिती IDENT:
	p = "IDENT";
	थांबा;
      स्थिती IDENT + ADD_NL:
	p = "IDENT+NL";
	थांबा;
      स्थिती SIDENT:
	p = "SIDENT";
	थांबा;
      स्थिती SIDENT + ADD_NL:
	p = "SIDENT+NL";
	थांबा;
      स्थिती KWORD:
	p = "KWORD";
	थांबा;
      स्थिती KWORD + ADD_NL:
	p = "KWORD+NL";
	थांबा;
      स्थिती SKWORD:
	p = "SKWORD";
	थांबा;
      स्थिती SKWORD + ADD_NL:
	p = "SKWORD+NL";
	थांबा;
      स्थिती FNAME:
	p = "FNAME";
	थांबा;
      स्थिती FNAME + ADD_NL:
	p = "FNAME+NL";
	थांबा;
      स्थिती SFNAME:
	p = "SFNAME";
	थांबा;
      स्थिती SFNAME + ADD_NL:
	p = "SFNAME+NL";
	थांबा;
      स्थिती PRINT:
	p = "PRINT";
	थांबा;
      स्थिती PRINT + ADD_NL:
	p = "PRINT+NL";
	थांबा;
      स्थिती SPRINT:
	p = "SPRINT";
	थांबा;
      स्थिती SPRINT + ADD_NL:
	p = "SPRINT+NL";
	थांबा;
      स्थिती WHITE:
	p = "WHITE";
	थांबा;
      स्थिती WHITE + ADD_NL:
	p = "WHITE+NL";
	थांबा;
      स्थिती NWHITE:
	p = "NWHITE";
	थांबा;
      स्थिती NWHITE + ADD_NL:
	p = "NWHITE+NL";
	थांबा;
      स्थिती DIGIT:
	p = "DIGIT";
	थांबा;
      स्थिती DIGIT + ADD_NL:
	p = "DIGIT+NL";
	थांबा;
      स्थिती NDIGIT:
	p = "NDIGIT";
	थांबा;
      स्थिती NDIGIT + ADD_NL:
	p = "NDIGIT+NL";
	थांबा;
      स्थिती HEX:
	p = "HEX";
	थांबा;
      स्थिती HEX + ADD_NL:
	p = "HEX+NL";
	थांबा;
      स्थिती NHEX:
	p = "NHEX";
	थांबा;
      स्थिती NHEX + ADD_NL:
	p = "NHEX+NL";
	थांबा;
      स्थिती OCTAL:
	p = "OCTAL";
	थांबा;
      स्थिती OCTAL + ADD_NL:
	p = "OCTAL+NL";
	थांबा;
      स्थिती NOCTAL:
	p = "NOCTAL";
	थांबा;
      स्थिती NOCTAL + ADD_NL:
	p = "NOCTAL+NL";
	थांबा;
      स्थिती WORD:
	p = "WORD";
	थांबा;
      स्थिती WORD + ADD_NL:
	p = "WORD+NL";
	थांबा;
      स्थिती NWORD:
	p = "NWORD";
	थांबा;
      स्थिती NWORD + ADD_NL:
	p = "NWORD+NL";
	थांबा;
      स्थिती HEAD:
	p = "HEAD";
	थांबा;
      स्थिती HEAD + ADD_NL:
	p = "HEAD+NL";
	थांबा;
      स्थिती NHEAD:
	p = "NHEAD";
	थांबा;
      स्थिती NHEAD + ADD_NL:
	p = "NHEAD+NL";
	थांबा;
      स्थिती ALPHA:
	p = "ALPHA";
	थांबा;
      स्थिती ALPHA + ADD_NL:
	p = "ALPHA+NL";
	थांबा;
      स्थिती NALPHA:
	p = "NALPHA";
	थांबा;
      स्थिती NALPHA + ADD_NL:
	p = "NALPHA+NL";
	थांबा;
      स्थिती LOWER:
	p = "LOWER";
	थांबा;
      स्थिती LOWER + ADD_NL:
	p = "LOWER+NL";
	थांबा;
      स्थिती NLOWER:
	p = "NLOWER";
	थांबा;
      स्थिती NLOWER + ADD_NL:
	p = "NLOWER+NL";
	थांबा;
      स्थिती UPPER:
	p = "UPPER";
	थांबा;
      स्थिती UPPER + ADD_NL:
	p = "UPPER+NL";
	थांबा;
      स्थिती NUPPER:
	p = "NUPPER";
	थांबा;
      स्थिती NUPPER + ADD_NL:
	p = "NUPPER+NL";
	थांबा;
      स्थिती BRANCH:
	p = "BRANCH";
	थांबा;
      स्थिती EXACTLY:
	p = "EXACTLY";
	थांबा;
      स्थिती NOTHING:
	p = "NOTHING";
	थांबा;
      स्थिती BACK:
	p = "BACK";
	थांबा;
      स्थिती END:
	p = "END";
	थांबा;
      स्थिती MOPEN + 0:
	p = "MATCH START";
	थांबा;
      स्थिती MOPEN + 1:
      स्थिती MOPEN + 2:
      स्थिती MOPEN + 3:
      स्थिती MOPEN + 4:
      स्थिती MOPEN + 5:
      स्थिती MOPEN + 6:
      स्थिती MOPEN + 7:
      स्थिती MOPEN + 8:
      स्थिती MOPEN + 9:
	buflen += vim_snprintf(buf + buflen, आकार(buf) - buflen, "MOPEN%d", OP(op) - MOPEN);
	p = रिक्त;
	थांबा;
      स्थिती MCLOSE + 0:
	p = "MATCH END";
	थांबा;
      स्थिती MCLOSE + 1:
      स्थिती MCLOSE + 2:
      स्थिती MCLOSE + 3:
      स्थिती MCLOSE + 4:
      स्थिती MCLOSE + 5:
      स्थिती MCLOSE + 6:
      स्थिती MCLOSE + 7:
      स्थिती MCLOSE + 8:
      स्थिती MCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, आकार(buf) - buflen, "MCLOSE%d", OP(op) - MCLOSE);
	p = रिक्त;
	थांबा;
      स्थिती BACKREF + 1:
      स्थिती BACKREF + 2:
      स्थिती BACKREF + 3:
      स्थिती BACKREF + 4:
      स्थिती BACKREF + 5:
      स्थिती BACKREF + 6:
      स्थिती BACKREF + 7:
      स्थिती BACKREF + 8:
      स्थिती BACKREF + 9:
	buflen += vim_snprintf(buf + buflen, आकार(buf) - buflen, "BACKREF%d", OP(op) - BACKREF);
	p = रिक्त;
	थांबा;
      स्थिती NOPEN:
	p = "NOPEN";
	थांबा;
      स्थिती NCLOSE:
	p = "NCLOSE";
	थांबा;
# ifdef FEAT_SYN_HL
      स्थिती ZOPEN + 1:
      स्थिती ZOPEN + 2:
      स्थिती ZOPEN + 3:
      स्थिती ZOPEN + 4:
      स्थिती ZOPEN + 5:
      स्थिती ZOPEN + 6:
      स्थिती ZOPEN + 7:
      स्थिती ZOPEN + 8:
      स्थिती ZOPEN + 9:
	buflen += vim_snprintf(buf + buflen, आकार(buf) - buflen, "ZOPEN%d", OP(op) - ZOPEN);
	p = रिक्त;
	थांबा;
      स्थिती ZCLOSE + 1:
      स्थिती ZCLOSE + 2:
      स्थिती ZCLOSE + 3:
      स्थिती ZCLOSE + 4:
      स्थिती ZCLOSE + 5:
      स्थिती ZCLOSE + 6:
      स्थिती ZCLOSE + 7:
      स्थिती ZCLOSE + 8:
      स्थिती ZCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, आकार(buf) - buflen, "ZCLOSE%d", OP(op) - ZCLOSE);
	p = रिक्त;
	थांबा;
      स्थिती ZREF + 1:
      स्थिती ZREF + 2:
      स्थिती ZREF + 3:
      स्थिती ZREF + 4:
      स्थिती ZREF + 5:
      स्थिती ZREF + 6:
      स्थिती ZREF + 7:
      स्थिती ZREF + 8:
      स्थिती ZREF + 9:
	buflen += vim_snprintf(buf + buflen, आकार(buf) - buflen, "ZREF%d", OP(op) - ZREF);
	p = रिक्त;
	थांबा;
# endif
      स्थिती STAR:
	p = "STAR";
	थांबा;
      स्थिती PLUS:
	p = "PLUS";
	थांबा;
      स्थिती NOMATCH:
	p = "NOMATCH";
	थांबा;
      स्थिती MATCH:
	p = "MATCH";
	थांबा;
      स्थिती BEHIND:
	p = "BEHIND";
	थांबा;
      स्थिती NOBEHIND:
	p = "NOBEHIND";
	थांबा;
      स्थिती SUBPAT:
	p = "SUBPAT";
	थांबा;
      स्थिती BRACE_LIMITS:
	p = "BRACE_LIMITS";
	थांबा;
      स्थिती BRACE_SIMPLE:
	p = "BRACE_SIMPLE";
	थांबा;
      स्थिती BRACE_COMPLEX + 0:
      स्थिती BRACE_COMPLEX + 1:
      स्थिती BRACE_COMPLEX + 2:
      स्थिती BRACE_COMPLEX + 3:
      स्थिती BRACE_COMPLEX + 4:
      स्थिती BRACE_COMPLEX + 5:
      स्थिती BRACE_COMPLEX + 6:
      स्थिती BRACE_COMPLEX + 7:
      स्थिती BRACE_COMPLEX + 8:
      स्थिती BRACE_COMPLEX + 9:
	buflen += vim_snprintf(buf + buflen, आकार(buf) - buflen, "BRACE_COMPLEX%d", OP(op) - BRACE_COMPLEX);
	p = रिक्त;
	थांबा;
      स्थिती MULTIBYTECODE:
	p = "MULTIBYTECODE";
	थांबा;
      स्थिती NEWL:
	p = "NEWL";
	थांबा;
      पूर्वनिर्धारित:
	buflen += vim_snprintf(buf + buflen, आकार(buf) - buflen, "corrupt %d", OP(op));
	p = रिक्त;
	थांबा;
    }
    जर (p != रिक्त)
	vim_strncpy((char_u *)buf + buflen, (char_u *)p, आकार(buf) - buflen - 1);
    परत (char_u *)buf;
}
#endif	    // DEBUG
