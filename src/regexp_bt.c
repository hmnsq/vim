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
#وصف_ڪريو پڇاڙي_3		0	//	End of program or NOMATCH operand.
#وصف_ڪريو BOL		1	//	Match "" at beginning of line.
#وصف_ڪريو EOL		2	//	Match "" at end of line.
#وصف_ڪريو BRANCH		3	// node Match this alternative, or the
				//	next...
#وصف_ڪريو پٺ_2		4	//	Match "", "next" ptr points backward.
#وصف_ڪريو EXACTLY		5	// str	Match this string.
#وصف_ڪريو NOTHING		6	//	Match empty string.
#وصف_ڪريو تارو		7	// node Match this (simple) thing 0 or more
				//	times.
#وصف_ڪريو PLUS		8	// node Match this (simple) thing 1 or more
				//	times.
#وصف_ڪريو ميلاپ_2		9	// node match the operand zero-width
#وصف_ڪريو NOMATCH		10	// node check for no match with operand
#وصف_ڪريو BEHIND		11	// node look behind for a match with operand
#وصف_ڪريو NOBEHIND	12	// node look behind for no match with operand
#وصف_ڪريو SUBPAT		13	// node match the operand here
#وصف_ڪريو BRACE_SIMPLE	14	// node Match this (simple) thing between m and
				//	n times (\{m,n\}).
#وصف_ڪريو BOW		15	//	Match "" after [^a-zA-Z0-9_]
#وصف_ڪريو EOW		16	//	Match "" at    [^a-zA-Z0-9_]
#وصف_ڪريو BRACE_LIMITS	17	// nr nr  define the min & max for BRACE_SIMPLE
				//	and BRACE_COMPLEX.
#وصف_ڪريو NEWL		18	//	Match line-break
#وصف_ڪريو BHPOS		19	//	End position for BEHIND or NOBEHIND


// character classes: 20-48 normal, 50-78 include a line-break
#وصف_ڪريو جوڙيو_NL		30
#وصف_ڪريو پهريون_NL	ANY + جوڙيو_NL
#وصف_ڪريو ANY		20	//	Match any one character.
#وصف_ڪريو ANYOF		21	// str	Match any character in this string.
#وصف_ڪريو ANYBUT		22	// str	Match any character not in this
				//	string.
#وصف_ڪريو IDENT		23	//	Match identifier char
#وصف_ڪريو SIDENT		24	//	Match identifier char but no digit
#وصف_ڪريو KWORD		25	//	Match keyword char
#وصف_ڪريو SKWORD		26	//	Match word char but no digit
#وصف_ڪريو FNAME		27	//	Match file name char
#وصف_ڪريو SFNAME		28	//	Match file name char but no digit
#وصف_ڪريو PRINT		29	//	Match printable char
#وصف_ڪريو SPRINT		30	//	Match printable char but no digit
#وصف_ڪريو WHITE		31	//	Match whitespace char
#وصف_ڪريو NWHITE		32	//	Match non-whitespace char
#وصف_ڪريو DIGIT		33	//	Match digit char
#وصف_ڪريو NDIGIT		34	//	Match non-digit char
#وصف_ڪريو HEX		35	//	Match hex char
#وصف_ڪريو NHEX		36	//	Match non-hex char
#وصف_ڪريو OCTAL		37	//	Match octal char
#وصف_ڪريو NOCTAL		38	//	Match non-octal char
#وصف_ڪريو لفظ_2		39	//	Match word char
#وصف_ڪريو NWORD		40	//	Match non-word char
#وصف_ڪريو سر_2		41	//	Match head char
#وصف_ڪريو NHEAD		42	//	Match non-head char
#وصف_ڪريو ALPHA		43	//	Match alpha char
#وصف_ڪريو NALPHA		44	//	Match non-alpha char
#وصف_ڪريو هيٺاهون_2		45	//	Match lowercase char
#وصف_ڪريو NLOWER		46	//	Match non-lowercase char
#وصف_ڪريو مٿانهون		47	//	Match uppercase char
#وصف_ڪريو NUPPER		48	//	Match non-uppercase char
#وصف_ڪريو آخري_NL		NUPPER + جوڙيو_NL
#وصف_ڪريو WITH_NL(op)	((op) >= پهريون_NL && (op) <= آخري_NL)

#وصف_ڪريو MOPEN		80  // -89	 Mark this point in input as start of
				//	 \( subexpr.  MOPEN + 0 marks start of
				//	 match.
#وصف_ڪريو MCLOSE		90  // -99	 Analogous to MOPEN.  MCLOSE + 0 marks
				//	 end of match.
#وصف_ڪريو BACKREF		100 // -109 node Match same string again \1-\9

#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
# define ZOPEN		110 // -119	 Mark this point in input as start of
				//	 \z( subexpr.
# define ZCLOSE		120 // -129	 Analogous to ZOPEN.
# define ZREF		130 // -139 node Match external submatch \z1-\z9
#جيڪڏهن_پورو

#وصف_ڪريو BRACE_COMPLEX	140 // -149 node Match nodes between m & n times

#وصف_ڪريو NOPEN		150	//	Mark this point in input as start of
				//	\%( subexpr.
#وصف_ڪريو NCLOSE		151	//	Analogous to NOPEN.

#وصف_ڪريو MULTIBYTECODE	200	// mbc	Match one multi-byte character
#وصف_ڪريو RE_BOF		201	//	Match "" at beginning of file.
#وصف_ڪريو RE_EOF		202	//	Match "" at end of file.
#وصف_ڪريو CURSOR		203	//	Match location of cursor.

#وصف_ڪريو RE_LNUM		204	// nr cmp  Match line number
#وصف_ڪريو RE_کانو		205	// nr cmp  Match column number
#وصف_ڪريو RE_VCOL		206	// nr cmp  Match virtual column number

#وصف_ڪريو RE_MARK		207	// mark cmp  Match mark position
#وصف_ڪريو RE_VISUAL	208	//	Match Visual area
#وصف_ڪريو RE_COMPOSING	209	// any composing characters

/*
 * Flags to be passed up and down.
 */
#وصف_ڪريو HASWIDTH	0x1	// Known never to match null string.
#وصف_ڪريو SIMPLE		0x2	// Simple enough to be STAR/PLUS operand.
#وصف_ڪريو SPSTART		0x4	// Starts with * or +.
#وصف_ڪريو HASNL		0x8	// Contains some \n.
#وصف_ڪريو HASLOOKBH	0x10	// Contains "\@<=" or "\@<!".
#وصف_ڪريو WORST		0	// Worst case.

ساكن صحيح	انگ_complex_braces; // Complex \{...} count
ساكن char_u	*regcode;	// Code-emit pointer, or JUST_CALC_SIZE
ساكن ڊگهو	regsize;	// Code size.
ساكن صحيح	reg_toolong;	// TRUE when offset out of range
ساكن صحيح	bt_reg_parse_اونهائي;	// nesting depth in reg()
ساكن char_u	had_endbrace[NSUBEXP];	// flags, TRUE if end of () found
ساكن ڊگهو	brace_min[10];	// Minimums for complex brace repeats
ساكن ڊگهو	brace_max[10];	// Maximums for complex brace repeats
ساكن صحيح	brace_ڳڻپ[10]; // Current counts for complex brace repeats
ساكن صحيح	one_exactly = FALSE;	// only do one char for EXACTLY

// When making changes to classchars also change nfa_classcodes.
ساكن char_u	*classchars = (char_u *)".iIkKfFpPsSdDxXoOwWhHaAlLuU";
ساكن ثابت صحيح classcodes[] = {
    ANY, IDENT, SIDENT, KWORD, SKWORD,
    FNAME, SFNAME, PRINT, SPRINT,
    WHITE, NWHITE, DIGIT, NDIGIT,
    HEX, NHEX, OCTAL, NOCTAL,
    لفظ_2, NWORD, سر_2, NHEAD,
    ALPHA, NALPHA, هيٺاهون_2, NLOWER,
    مٿانهون, NUPPER
};

/*
 * Search between "start" (the first char of the range) and "end" (the closing
 * "]") and try to recognize a character class in expanded form, for example
 * [0-9].  On success, return the atom to be emitted, on failure 0.
 */
    ساكن صحيح
bt_recognize_char_class(char_u *شروعات, char_u *پڇاڙي)
{
    صحيح		newl;
    صحيح		config = get_char_class_بٽ(شروعات, پڇاڙي, &newl);

    // A newline would need the ADD_NL variant, don't bother with it here.
    إذا (config < 0 || newl)
	أرجع 0;

	// The old engine has no case-insensitive class opcode, so [a-z] and [A-Z]
	// are left as collections.  The classes below are case-independent.
    بدل (config)
    {
	حالة CLASS_o9:
	    أرجع DIGIT;
	حالة CLASS_not | CLASS_o9:
	    أرجع NDIGIT;
	حالة CLASS_af | CLASS_AF | CLASS_o9:
	    أرجع HEX;
	حالة CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    أرجع NHEX;
	حالة CLASS_o7:
	    أرجع OCTAL;
	حالة CLASS_not | CLASS_o7:
	    أرجع NOCTAL;
	حالة CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    أرجع لفظ_2;
	حالة CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    أرجع NWORD;
	حالة CLASS_az | CLASS_AZ | CLASS_underscore:
	    أرجع سر_2;
	حالة CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    أرجع NHEAD;
	حالة CLASS_az | CLASS_AZ:
	    أرجع ALPHA;
	حالة CLASS_not | CLASS_az | CLASS_AZ:
	    أرجع NALPHA;
    }
    أرجع 0;
}

/*
 * When regcode is set to this value, code is not emitted and size is computed
 * instead.
 */
#وصف_ڪريو JUST_CALC_سائيز	((char_u *) -1)

// Values for rs_state in regitem_T.
تعريفنوع تعداد regstate_E
{
    RS_NOPEN = 0	// NOPEN and NCLOSE
    , RS_MOPEN		// MOPEN + [0-9]
    , RS_MCLOSE		// MCLOSE + [0-9]
#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
    , RS_ZOPEN		// ZOPEN + [0-9]
    , RS_ZCLOSE		// ZCLOSE + [0-9]
#جيڪڏهن_پورو
    , RS_BRANCH		// BRANCH
    , RS_BRCPLX_وڌيڪ	// BRACE_COMPLEX and trying one more match
    , RS_BRCPLX_LONG	// BRACE_COMPLEX and trying longest match
    , RS_BRCPLX_SHORT	// BRACE_COMPLEX and trying shortest match
    , RS_NOMATCH	// NOMATCH
    , RS_BEHIND1	// BEHIND / NOBEHIND matching rest
    , RS_BEHIND2	// BEHIND / NOBEHIND matching behind part
    , RS_تارو_LONG	// STAR/PLUS/BRACE_SIMPLE longest match
    , RS_تارو_SHORT	// STAR/PLUS/BRACE_SIMPLE shortest match
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
    save_se_T   save_شروعات[NSUBEXP];
    save_se_T   save_پڇاڙي[NSUBEXP];
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
    ننڍو	rs_no;		// submatch nr or BEHIND/NOBEHIND
    char_u	*rs_جاچ;	// current node in program
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
    ڊگهو	ڳڻپ;
    ڊگهو	minval;
    ڊگهو	maxval;
} regstar_T;

// used to store input position when a BACK was encountered, so that we now if
// we made any progress since the last time.
تعريفنوع بنية backpos_S
{
    char_u	*bp_جاچ;	// "scan" where BACK was encountered
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
#وصف_ڪريو REGSTACK_INITIAL	2048
#وصف_ڪريو BACKPOS_INITIAL		64

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
#وصف_ڪريو OP(p)		((صحيح)*(p))
#وصف_ڪريو ايندڙ_2(p)		(((*((p) + 1) & 0377) << 8) + (*((p) + 2) & 0377))
#وصف_ڪريو عمليو(p)	((p) + 3)
// Obtain an operand that was stored as four bytes, MSB first.
#وصف_ڪريو عمليو_MIN(p)	(((ڊگهو)(p)[3] << 24) + ((ڊگهو)(p)[4] << 16) \
			+ ((ڊگهو)(p)[5] << 8) + (ڊگهو)(p)[6])
// Obtain a second operand stored as four bytes.
#وصف_ڪريو عمليو_MAX(p)	عمليو_MIN((p) + 4)
// Obtain a second single-byte operand stored after a four bytes operand.
#وصف_ڪريو عمليو_CMP(p)	(p)[7]

ساكن char_u *reg(صحيح paren, صحيح *flagp);

#جيڪڏهن_وصف_ٿيل BT_REGEXP_DUMP
ساكن فراغ	regdump(char_u *, bt_regprog_T *);
#جيڪڏهن_پورو

ساكن صحيح	re_انگ_cmp(long_u val, char_u *جاچ);

#جيڪڏهن_وصف_ٿيل DEBUG
ساكن char_u	*regprop(char_u *);

ساكن صحيح	regnarrate = 0;
#جيڪڏهن_پورو


/*
 * Setup to parse the regexp.  Used once to get the length and once to do it.
 */
    ساكن فراغ
regcomp_شروعات(
    char_u	*اظهار,
    صحيح		re_flags)	    // see vim_regcomp()
{
    initchr(اظهار);
    إذا (re_flags & RE_MAGIC)
	reg_magic = MAGIC_ON;
    وإلا
	reg_magic = MAGIC_OFF;
    reg_string = (re_flags & RE_STRING);
    reg_strict = (re_flags & RE_STRICT);
    get_cpo_flags();

    انگ_complex_braces = 0;
    regnpar = 1;
    CLEAR_ميدان(had_endbrace);
#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
    regnzpar = 1;
    re_has_z = 0;
#جيڪڏهن_پورو
    regsize = 0L;
    reg_toolong = FALSE;
    bt_reg_parse_اونهائي = 0;
    regflags = 0;
#إذا defined(FEAT_SYN_HL)
    had_eol = FALSE;
#جيڪڏهن_پورو
}

/*
 * Return TRUE if MULTIBYTECODE should be used instead of EXACTLY for
 * character "c".
 */
    ساكن صحيح
use_multibytecode(صحيح c)
{
    أرجع has_mbyte && (*mb_char2len)(c) > 1
		     && (re_multi_type(peekchr()) != NOT_MULTI
			     || (enc_utf8 && utf_iscomposing(c)));
}

/*
 * Emit (if appropriate) a byte of code
 */
    ساكن فراغ
regc(صحيح b)
{
    إذا (regcode == JUST_CALC_سائيز)
	regsize++;
    وإلا
	*regcode++ = b;
}

/*
 * Emit (if appropriate) a multi-byte character of code
 */
    ساكن فراغ
regmbc(صحيح c)
{
    إذا (!has_mbyte && c > 0xff)
	أرجع;
    إذا (regcode == JUST_CALC_سائيز)
	regsize += (*mb_char2len)(c);
    وإلا
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
    إذا (enc_utf8 || STRCMP(p_enc, "latin1") == 0
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
		      أرجع;
	    حالة 'B': حالة 0x181: حالة 0x243: حالة 0x1e02:
	    حالة 0x1e04: حالة 0x1e06:
		      regmbc('B');
		      regmbc(0x181); regmbc(0x243); regmbc(0x1e02);
		      regmbc(0x1e04); regmbc(0x1e06);
		      أرجع;
	    حالة 'C': حالة 0xc7:
	    حالة 0x106: حالة 0x108: حالة 0x10a: حالة 0x10c: حالة 0x187:
	    حالة 0x23b: حالة 0x1e08: حالة 0xa792:
		      regmbc('C'); regmbc(0xc7);
		      regmbc(0x106); regmbc(0x108); regmbc(0x10a);
		      regmbc(0x10c); regmbc(0x187); regmbc(0x23b);
		      regmbc(0x1e08); regmbc(0xa792);
		      أرجع;
	    حالة 'D': حالة 0x10e: حالة 0x110: حالة 0x18a:
	    حالة 0x1e0a: حالة 0x1e0c: حالة 0x1e0e: حالة 0x1e10:
	    حالة 0x1e12:
		      regmbc('D'); regmbc(0x10e); regmbc(0x110);
		      regmbc(0x18a); regmbc(0x1e0a); regmbc(0x1e0c);
		      regmbc(0x1e0e); regmbc(0x1e10); regmbc(0x1e12);
		      أرجع;
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
		      أرجع;
	    حالة 'F': حالة 0x191: حالة 0x1e1e: حالة 0xa798:
		      regmbc('F'); regmbc(0x191); regmbc(0x1e1e);
		      regmbc(0xa798);
		      أرجع;
	    حالة 'G': حالة 0x11c: حالة 0x11e: حالة 0x120:
	    حالة 0x122: حالة 0x193: حالة 0x1e4: حالة 0x1e6:
	    حالة 0x1f4: حالة 0x1e20: حالة 0xa7a0:
		      regmbc('G'); regmbc(0x11c); regmbc(0x11e);
		      regmbc(0x120); regmbc(0x122); regmbc(0x193);
		      regmbc(0x1e4); regmbc(0x1e6); regmbc(0x1f4);
		      regmbc(0x1e20); regmbc(0xa7a0);
		      أرجع;
	    حالة 'H': حالة 0x124: حالة 0x126: حالة 0x21e:
	    حالة 0x1e22: حالة 0x1e24: حالة 0x1e26:
	    حالة 0x1e28: حالة 0x1e2a: حالة 0x2c67:
		      regmbc('H'); regmbc(0x124); regmbc(0x126);
		      regmbc(0x21e); regmbc(0x1e22); regmbc(0x1e24);
		      regmbc(0x1e26); regmbc(0x1e28); regmbc(0x1e2a);
		      regmbc(0x2c67);
		      أرجع;
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
		      أرجع;
	    حالة 'J': حالة 0x134: حالة 0x248:
		      regmbc('J'); regmbc(0x134); regmbc(0x248);
		      أرجع;
	    حالة 'K': حالة 0x136: حالة 0x198: حالة 0x1e8: حالة 0x1e30:
	    حالة 0x1e32: حالة 0x1e34: حالة 0x2c69: حالة 0xa740:
		      regmbc('K'); regmbc(0x136); regmbc(0x198);
		      regmbc(0x1e8); regmbc(0x1e30); regmbc(0x1e32);
		      regmbc(0x1e34); regmbc(0x2c69); regmbc(0xa740);
		      أرجع;
	    حالة 'L': حالة 0x139: حالة 0x13b: حالة 0x13d: حالة 0x13f:
	    حالة 0x141: حالة 0x23d: حالة 0x1e36: حالة 0x1e38:
	    حالة 0x1e3a: حالة 0x1e3c: حالة 0x2c60:
		      regmbc('L'); regmbc(0x139); regmbc(0x13b);
		      regmbc(0x13d); regmbc(0x13f); regmbc(0x141);
		      regmbc(0x23d); regmbc(0x1e36); regmbc(0x1e38);
		      regmbc(0x1e3a); regmbc(0x1e3c); regmbc(0x2c60);
		      أرجع;
	    حالة 'M': حالة 0x1e3e: حالة 0x1e40: حالة 0x1e42:
		      regmbc('M'); regmbc(0x1e3e); regmbc(0x1e40);
		      regmbc(0x1e42);
		      أرجع;
	    حالة 'N': حالة 0xd1:
	    حالة 0x143: حالة 0x145: حالة 0x147: حالة 0x1f8:
	    حالة 0x1e44: حالة 0x1e46: حالة 0x1e48: حالة 0x1e4a:
	    حالة 0xa7a4:
		      regmbc('N'); regmbc(0xd1);
		      regmbc(0x143); regmbc(0x145); regmbc(0x147);
		      regmbc(0x1f8); regmbc(0x1e44); regmbc(0x1e46);
		      regmbc(0x1e48); regmbc(0x1e4a); regmbc(0xa7a4);
		      أرجع;
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
		      أرجع;
	    حالة 'P': حالة 0x1a4: حالة 0x1e54: حالة 0x1e56: حالة 0x2c63:
		      regmbc('P'); regmbc(0x1a4); regmbc(0x1e54);
		      regmbc(0x1e56); regmbc(0x2c63);
		      أرجع;
	    حالة 'Q': حالة 0x24a:
		      regmbc('Q'); regmbc(0x24a);
		      أرجع;
	    حالة 'R': حالة 0x154: حالة 0x156: حالة 0x158: حالة 0x210:
	    حالة 0x212: حالة 0x24c: حالة 0x1e58: حالة 0x1e5a:
	    حالة 0x1e5c: حالة 0x1e5e: حالة 0x2c64: حالة 0xa7a6:
		      regmbc('R'); regmbc(0x154); regmbc(0x156);
		      regmbc(0x210); regmbc(0x212); regmbc(0x158);
		      regmbc(0x24c); regmbc(0x1e58); regmbc(0x1e5a);
		      regmbc(0x1e5c); regmbc(0x1e5e); regmbc(0x2c64);
		      regmbc(0xa7a6);
		      أرجع;
	    حالة 'S': حالة 0x15a: حالة 0x15c: حالة 0x15e: حالة 0x160:
	    حالة 0x218: حالة 0x1e60: حالة 0x1e62: حالة 0x1e64:
	    حالة 0x1e66: حالة 0x1e68: حالة 0x2c7e: حالة 0xa7a8:
		      regmbc('S'); regmbc(0x15a); regmbc(0x15c);
		      regmbc(0x15e); regmbc(0x160); regmbc(0x218);
		      regmbc(0x1e60); regmbc(0x1e62); regmbc(0x1e64);
		      regmbc(0x1e66); regmbc(0x1e68); regmbc(0x2c7e);
			  regmbc(0xa7a8);
		      أرجع;
	    حالة 'T': حالة 0x162: حالة 0x164: حالة 0x166: حالة 0x1ac:
	    حالة 0x1ae: حالة 0x21a: حالة 0x23e: حالة 0x1e6a: حالة 0x1e6c:
	    حالة 0x1e6e: حالة 0x1e70:
		      regmbc('T'); regmbc(0x162); regmbc(0x164);
		      regmbc(0x166); regmbc(0x1ac); regmbc(0x23e);
		      regmbc(0x1ae); regmbc(0x21a); regmbc(0x1e6a);
		      regmbc(0x1e6c); regmbc(0x1e6e); regmbc(0x1e70);
		      أرجع;
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
		      أرجع;
	    حالة 'V': حالة 0x1b2: حالة 0x1e7c: حالة 0x1e7e:
		      regmbc('V'); regmbc(0x1b2); regmbc(0x1e7c);
		      regmbc(0x1e7e);
		      أرجع;
	    حالة 'W': حالة 0x174: حالة 0x1e80: حالة 0x1e82:
	    حالة 0x1e84: حالة 0x1e86: حالة 0x1e88:
		      regmbc('W'); regmbc(0x174); regmbc(0x1e80);
		      regmbc(0x1e82); regmbc(0x1e84); regmbc(0x1e86);
		      regmbc(0x1e88);
		      أرجع;
	    حالة 'X': حالة 0x1e8a: حالة 0x1e8c:
		      regmbc('X'); regmbc(0x1e8a); regmbc(0x1e8c);
		      أرجع;
	    حالة 'Y': حالة 0xdd:
	    حالة 0x176: حالة 0x178: حالة 0x1b3: حالة 0x232: حالة 0x24e:
	    حالة 0x1e8e: حالة 0x1ef2: حالة 0x1ef6: حالة 0x1ef4: حالة 0x1ef8:
		      regmbc('Y'); regmbc(0xdd); regmbc(0x176);
		      regmbc(0x178); regmbc(0x1b3); regmbc(0x232);
		      regmbc(0x24e); regmbc(0x1e8e); regmbc(0x1ef2);
		      regmbc(0x1ef4); regmbc(0x1ef6); regmbc(0x1ef8);
		      أرجع;
	    حالة 'Z': حالة 0x179: حالة 0x17b: حالة 0x17d: حالة 0x1b5:
	    حالة 0x1e90: حالة 0x1e92: حالة 0x1e94: حالة 0x2c6b:
		      regmbc('Z'); regmbc(0x179); regmbc(0x17b);
		      regmbc(0x17d); regmbc(0x1b5); regmbc(0x1e90);
		      regmbc(0x1e92); regmbc(0x1e94); regmbc(0x2c6b);
		      أرجع;
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
		      أرجع;
	    حالة 'b': حالة 0x180: حالة 0x253: حالة 0x1d6c: حالة 0x1d80:
	    حالة 0x1e03: حالة 0x1e05: حالة 0x1e07:
		      regmbc('b');
		      regmbc(0x180); regmbc(0x253); regmbc(0x1d6c);
		      regmbc(0x1d80); regmbc(0x1e03); regmbc(0x1e05);
		      regmbc(0x1e07);
		      أرجع;
	    حالة 'c': حالة 0xe7:
	    حالة 0x107: حالة 0x109: حالة 0x10b: حالة 0x10d: حالة 0x188:
	    حالة 0x23c: حالة 0x1e09: حالة 0xa793: حالة 0xa794:
		      regmbc('c'); regmbc(0xe7); regmbc(0x107);
		      regmbc(0x109); regmbc(0x10b); regmbc(0x10d);
		      regmbc(0x188); regmbc(0x23c); regmbc(0x1e09);
		      regmbc(0xa793); regmbc(0xa794);
		      أرجع;
	    حالة 'd': حالة 0x10f: حالة 0x111: حالة 0x257: حالة 0x1d6d:
	    حالة 0x1d81: حالة 0x1d91: حالة 0x1e0b: حالة 0x1e0d:
	    حالة 0x1e0f: حالة 0x1e11: حالة 0x1e13:
		      regmbc('d'); regmbc(0x10f); regmbc(0x111);
		      regmbc(0x257); regmbc(0x1d6d); regmbc(0x1d81);
		      regmbc(0x1d91); regmbc(0x1e0b); regmbc(0x1e0d);
		      regmbc(0x1e0f); regmbc(0x1e11); regmbc(0x1e13);
		      أرجع;
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
		      أرجع;
	    حالة 'f': حالة 0x192: حالة 0x1d6e: حالة 0x1d82:
	    حالة 0x1e1f: حالة 0xa799:
		     regmbc('f'); regmbc(0x192); regmbc(0x1d6e);
		     regmbc(0x1d82); regmbc(0x1e1f); regmbc(0xa799);
		     أرجع;
	    حالة 'g': حالة 0x11d: حالة 0x11f: حالة 0x121: حالة 0x123:
	    حالة 0x1e5: حالة 0x1e7: حالة 0x260: حالة 0x1f5: حالة 0x1d83:
	    حالة 0x1e21: حالة 0xa7a1:
		      regmbc('g'); regmbc(0x11d); regmbc(0x11f);
		      regmbc(0x121); regmbc(0x123); regmbc(0x1e5);
		      regmbc(0x1e7); regmbc(0x1f5); regmbc(0x260);
		      regmbc(0x1d83); regmbc(0x1e21); regmbc(0xa7a1);
		      أرجع;
	    حالة 'h': حالة 0x125: حالة 0x127: حالة 0x21f: حالة 0x1e23:
	    حالة 0x1e25: حالة 0x1e27: حالة 0x1e29: حالة 0x1e2b:
	    حالة 0x1e96: حالة 0x2c68: حالة 0xa795:
		      regmbc('h'); regmbc(0x125); regmbc(0x127);
		      regmbc(0x21f); regmbc(0x1e23); regmbc(0x1e25);
		      regmbc(0x1e27); regmbc(0x1e29); regmbc(0x1e2b);
		      regmbc(0x1e96); regmbc(0x2c68); regmbc(0xa795);
		      أرجع;
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
		      أرجع;
	    حالة 'j': حالة 0x135: حالة 0x1f0: حالة 0x249:
		      regmbc('j'); regmbc(0x135); regmbc(0x1f0);
		      regmbc(0x249);
		      أرجع;
	    حالة 'k': حالة 0x137: حالة 0x199: حالة 0x1e9:
	    حالة 0x1d84: حالة 0x1e31: حالة 0x1e33: حالة 0x1e35:
	    حالة 0x2c6a: حالة 0xa741:
		      regmbc('k'); regmbc(0x137); regmbc(0x199);
		      regmbc(0x1e9); regmbc(0x1d84); regmbc(0x1e31);
		      regmbc(0x1e33); regmbc(0x1e35); regmbc(0x2c6a);
		      regmbc(0xa741);
		      أرجع;
	    حالة 'l': حالة 0x13a: حالة 0x13c: حالة 0x13e:
	    حالة 0x140: حالة 0x142: حالة 0x19a: حالة 0x1e37:
	    حالة 0x1e39: حالة 0x1e3b: حالة 0x1e3d: حالة 0x2c61:
		      regmbc('l'); regmbc(0x13a); regmbc(0x13c);
		      regmbc(0x13e); regmbc(0x140); regmbc(0x142);
		      regmbc(0x19a); regmbc(0x1e37); regmbc(0x1e39);
		      regmbc(0x1e3b); regmbc(0x1e3d); regmbc(0x2c61);
		      أرجع;
	    حالة 'm': حالة 0x1d6f: حالة 0x1e3f: حالة 0x1e41: حالة 0x1e43:
		      regmbc('m'); regmbc(0x1d6f); regmbc(0x1e3f);
		      regmbc(0x1e41); regmbc(0x1e43);
		      أرجع;
	    حالة 'n': حالة 0xf1: حالة 0x144: حالة 0x146: حالة 0x148:
	    حالة 0x149: حالة 0x1f9: حالة 0x1d70: حالة 0x1d87:
	    حالة 0x1e45: حالة 0x1e47: حالة 0x1e49: حالة 0x1e4b:
	    حالة 0xa7a5:
		      regmbc('n'); regmbc(0xf1); regmbc(0x144);
		      regmbc(0x146); regmbc(0x148); regmbc(0x149);
		      regmbc(0x1f9); regmbc(0x1d70); regmbc(0x1d87);
		      regmbc(0x1e45); regmbc(0x1e47); regmbc(0x1e49);
		      regmbc(0x1e4b); regmbc(0xa7a5);
		      أرجع;
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
		      أرجع;
	    حالة 'p': حالة 0x1a5: حالة 0x1d71: حالة 0x1d88: حالة 0x1d7d:
	    حالة 0x1e55: حالة 0x1e57:
		      regmbc('p'); regmbc(0x1a5); regmbc(0x1d71);
		      regmbc(0x1d7d); regmbc(0x1d88); regmbc(0x1e55);
		      regmbc(0x1e57);
		      أرجع;
	    حالة 'q': حالة 0x24b: حالة 0x2a0:
		      regmbc('q'); regmbc(0x24b); regmbc(0x2a0);
		      أرجع;
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
		      أرجع;
	    حالة 's': حالة 0x15b: حالة 0x15d: حالة 0x15f: حالة 0x161:
	    حالة 0x1e61: حالة 0x219: حالة 0x23f: حالة 0x1d74: حالة 0x1d8a:
	    حالة 0x1e63: حالة 0x1e65: حالة 0x1e67: حالة 0x1e69: حالة 0xa7a9:
		      regmbc('s'); regmbc(0x15b); regmbc(0x15d);
		      regmbc(0x15f); regmbc(0x161); regmbc(0x23f);
		      regmbc(0x219); regmbc(0x1d74); regmbc(0x1d8a);
		      regmbc(0x1e61); regmbc(0x1e63); regmbc(0x1e65);
		      regmbc(0x1e67); regmbc(0x1e69); regmbc(0xa7a9);
		      أرجع;
	    حالة 't': حالة 0x163: حالة 0x165: حالة 0x167: حالة 0x1ab:
	    حالة 0x1ad: حالة 0x21b: حالة 0x288: حالة 0x1d75: حالة 0x1e6b:
	    حالة 0x1e6d: حالة 0x1e6f: حالة 0x1e71: حالة 0x1e97: حالة 0x2c66:
		      regmbc('t'); regmbc(0x163); regmbc(0x165);
		      regmbc(0x167); regmbc(0x1ab); regmbc(0x21b);
		      regmbc(0x1ad); regmbc(0x288); regmbc(0x1d75);
		      regmbc(0x1e6b); regmbc(0x1e6d); regmbc(0x1e6f);
		      regmbc(0x1e71); regmbc(0x1e97); regmbc(0x2c66);
		      أرجع;
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
		      أرجع;
	    حالة 'v': حالة 0x28b: حالة 0x1d8c: حالة 0x1e7d: حالة 0x1e7f:
		      regmbc('v'); regmbc(0x28b); regmbc(0x1d8c);
		      regmbc(0x1e7d); regmbc(0x1e7f);
		      أرجع;
	    حالة 'w': حالة 0x175: حالة 0x1e81: حالة 0x1e83:
	    حالة 0x1e85: حالة 0x1e87: حالة 0x1e89: حالة 0x1e98:
		      regmbc('w'); regmbc(0x175); regmbc(0x1e81);
		      regmbc(0x1e83); regmbc(0x1e85); regmbc(0x1e87);
		      regmbc(0x1e89); regmbc(0x1e98);
		      أرجع;
	    حالة 'x': حالة 0x1e8b: حالة 0x1e8d:
		      regmbc('x'); regmbc(0x1e8b); regmbc(0x1e8d);
		      أرجع;
	    حالة 'y': حالة 0xfd: حالة 0xff: حالة 0x177: حالة 0x1b4:
	    حالة 0x233: حالة 0x24f: حالة 0x1e8f: حالة 0x1e99: حالة 0x1ef3:
	    حالة 0x1ef5: حالة 0x1ef7: حالة 0x1ef9:
		      regmbc('y'); regmbc(0xfd); regmbc(0xff);
		      regmbc(0x177); regmbc(0x1b4); regmbc(0x233);
		      regmbc(0x24f); regmbc(0x1e8f); regmbc(0x1e99);
		      regmbc(0x1ef3); regmbc(0x1ef5); regmbc(0x1ef7);
		      regmbc(0x1ef9);
		      أرجع;
	    حالة 'z': حالة 0x17a: حالة 0x17c: حالة 0x17e: حالة 0x1b6:
	    حالة 0x1d76: حالة 0x1d8e: حالة 0x1e91: حالة 0x1e93:
	    حالة 0x1e95: حالة 0x2c6c:
		      regmbc('z'); regmbc(0x17a); regmbc(0x17c);
		      regmbc(0x17e); regmbc(0x1b6); regmbc(0x1d76);
		      regmbc(0x1d8e); regmbc(0x1e91); regmbc(0x1e93);
		      regmbc(0x1e95); regmbc(0x2c6c);
		      أرجع;
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
    إذا (ret == JUST_CALC_سائيز)
	regsize += 3;
    وإلا
    {
	*regcode++ = op;
	*regcode++ = NUL;		// Null "next" pointer.
	*regcode++ = NUL;
    }
    أرجع ret;
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
    أرجع p;
}

/*
 * regnext - dig the "next" pointer out of a node
 * Returns NULL when calculating size, when there is no next item and when
 * there is an error.
 */
    ساكن char_u *
regnext(char_u *p)
{
    صحيح	    وٿي;

    إذا (p == JUST_CALC_سائيز || reg_toolong)
	أرجع عدم;

    وٿي = ايندڙ_2(p);
    إذا (وٿي == 0)
	أرجع عدم;

    إذا (OP(p) == پٺ_2)
	أرجع p - وٿي;
    وإلا
	أرجع p + وٿي;
}

/*
 * Set the next-pointer at the end of a node chain.
 */
    ساكن فراغ
regtail(char_u *p, char_u *val)
{
    char_u	*جاچ;
    char_u	*عارضي;
    صحيح		وٿي;

    إذا (p == JUST_CALC_سائيز)
	أرجع;

    // Find last node.
    جاچ = p;
    لكل (;;)
    {
	عارضي = regnext(جاچ);
	إذا (عارضي == عدم)
	    اكسر;
	جاچ = عارضي;
    }

    إذا (OP(جاچ) == پٺ_2)
	وٿي = (صحيح)(جاچ - val);
    وإلا
	وٿي = (صحيح)(val - جاچ);
    // When the offset uses more than 16 bits it can no longer fit in the two
    // bytes available.  Use a global flag to avoid having to check return
    // values in too many places.
    إذا (وٿي > 0xffff)
	reg_toolong = TRUE;
    وإلا
    {
	*(جاچ + 1) = (char_u) (((بنانشان)وٿي >> 8) & 0377);
	*(جاچ + 2) = (char_u) (وٿي & 0377);
    }
}

/*
 * Like regtail, on item after a BRANCH; nop if none.
 */
    ساكن فراغ
regoptail(char_u *p, char_u *val)
{
    // When op is neither BRANCH nor BRACE_COMPLEX0-9, it is "operandless"
    إذا (p == عدم || p == JUST_CALC_سائيز
	    || (OP(p) != BRANCH
		&& (OP(p) < BRACE_COMPLEX || OP(p) > BRACE_COMPLEX + 9)))
	أرجع;
    regtail(عمليو(p), val);
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
    char_u	*رکو;

    إذا (regcode == JUST_CALC_سائيز)
    {
	regsize += 3;
	أرجع;
    }
    src = regcode;
    regcode += 3;
    dst = regcode;
    طالما (src > opnd)
	*--dst = *--src;

    رکو = opnd;		// Op node, where operand used to be.
    *رکو++ = op;
    *رکو++ = NUL;
    *رکو = NUL;
}

/*
 * Insert an operator in front of already-emitted operand.
 * Add a number to the operator.
 */
    ساكن فراغ
reginsert_nr(صحيح op, ڊگهو val, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*رکو;

    إذا (regcode == JUST_CALC_سائيز)
    {
	regsize += 7;
	أرجع;
    }
    src = regcode;
    regcode += 7;
    dst = regcode;
    طالما (src > opnd)
	*--dst = *--src;

    رکو = opnd;		// Op node, where operand used to be.
    *رکو++ = op;
    *رکو++ = NUL;
    *رکو++ = NUL;
    re_put_long(رکو, (long_u)val);
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
    ڊگهو	minval,
    ڊگهو	maxval,
    char_u	*opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*رکو;

    إذا (regcode == JUST_CALC_سائيز)
    {
	regsize += 11;
	أرجع;
    }
    src = regcode;
    regcode += 11;
    dst = regcode;
    طالما (src > opnd)
	*--dst = *--src;

    رکو = opnd;		// Op node, where operand used to be.
    *رکو++ = op;
    *رکو++ = NUL;
    *رکو++ = NUL;
    رکو = re_put_long(رکو, (long_u)minval);
    رکو = re_put_long(رکو, (long_u)maxval);
    regtail(opnd, رکو);
}

/*
 * Return TRUE if the back reference is legal. We must have seen the close
 * brace.
 * TODO: Should also check that we don't refer to something that is repeated
 * (+*=): what instance of the repetition should we match?
 */
    ساكن صحيح
ڏٺل_endbrace(صحيح refnum)
{
    إذا (!had_endbrace[refnum])
    {
	char_u *p;

	// Trick: check if "@<=" or "@<!" follows, in which case
	// the \1 can appear before the referenced match.
	لكل (p = regparse; *p != NUL; ++p)
	    إذا (p[0] == '@' && p[1] == '<' && (p[2] == '!' || p[2] == '='))
		اكسر;
	إذا (*p == NUL)
	{
	    emsg(_(e_illegal_پٺ_reference));
	    rc_did_emsg = TRUE;
	    أرجع FALSE;
	}
    }
    أرجع TRUE;
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
    صحيح		    save_prev_at_شروعات = prev_at_شروعات;

    *flagp = WORST;		// Tentatively.

    c = getchr();
    بدل (c)
    {
      حالة Magic('^'):
	ret = regnode(BOL);
	اكسر;

      حالة Magic('$'):
	ret = regnode(EOL);
#إذا defined(FEAT_SYN_HL)
	had_eol = TRUE;
#جيڪڏهن_پورو
	اكسر;

      حالة Magic('<'):
	ret = regnode(BOW);
	اكسر;

      حالة Magic('>'):
	ret = regnode(EOW);
	اكسر;

      حالة Magic('_'):
	c = no_Magic(getchr());
	إذا (c == '^')		// "\_^" is start-of-line
	{
	    ret = regnode(BOL);
	    اكسر;
	}
	إذا (c == '$')		// "\_$" is end-of-line
	{
	    ret = regnode(EOL);
#إذا defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#جيڪڏهن_پورو
	    اكسر;
	}

	extra = جوڙيو_NL;
	*flagp |= HASNL;

	// "\_[" is character range plus newline
	إذا (c == '[')
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
	إذا (p == عدم)
	    EMSG_RET_NULL(_(e_invalid_use_جو_underscore));

	// When '.' is followed by a composing char ignore the dot, so that
	// the composing char is matched here.
	إذا (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	{
	    c = getchr();
	    اذهب do_multibyte;
	}
	ret = regnode(classcodes[p - classchars] + extra);
	*flagp |= HASWIDTH | SIMPLE;
	اكسر;

      حالة Magic('n'):
	إذا (reg_string)
	{
	    // In a string "\n" matches a newline character.
	    ret = regnode(EXACTLY);
	    regc(NL);
	    regc(NUL);
	    *flagp |= HASWIDTH | SIMPLE;
	}
	وإلا
	{
	    // In buffer text "\n" matches the end of a line.
	    ret = regnode(NEWL);
	    *flagp |= HASWIDTH | HASNL;
	}
	اكسر;

      حالة Magic('('):
	إذا (one_exactly)
	    EMSG_ONE_RET_NULL;
	ret = reg(REG_PAREN, &flags);
	إذا (ret == عدم)
	    أرجع عدم;
	*flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
	اكسر;

      حالة NUL:
      حالة Magic('|'):
      حالة Magic('&'):
      حالة Magic(')'):
	إذا (one_exactly)
	    EMSG_ONE_RET_NULL;
	// Supposed to be caught earlier.
	IEMSG_RET_NULL(e_اندروني_error_اندريون_regexp);
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
	    إذا (reg_prev_sub != عدم)
	    {
		char_u	    *lp;

		ret = regnode(EXACTLY);
		lp = reg_prev_sub;
		طالما (*lp != NUL)
		    regc(*lp++);
		regc(NUL);
		إذا (*reg_prev_sub != NUL)
		{
		    *flagp |= HASWIDTH;
		    إذا ((lp - reg_prev_sub) == 1)
			*flagp |= SIMPLE;
		}
	    }
	    وإلا
		EMSG_RET_NULL(_(e_no_اڳيون_substitute_regular_اظهار));
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
		إذا (!ڏٺل_endbrace(refnum))
		    أرجع عدم;
		ret = regnode(BACKREF + refnum);
	    }
	    اكسر;

      حالة Magic('z'):
	{
	    c = no_Magic(getchr());
	    بدل (c)
	    {
#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
		حالة '(': إذا ((reg_do_extmatch & REX_سيٽ) == 0)
			      EMSG_RET_NULL(_(e_z_not_allowed_هتي));
			  إذا (one_exactly)
			      EMSG_ONE_RET_NULL;
			  ret = reg(REG_ZPAREN, &flags);
			  إذا (ret == عدم)
			      أرجع عدم;
			  *flagp |= flags & (HASWIDTH|SPSTART|HASNL|HASLOOKBH);
			  re_has_z = REX_سيٽ;
			  اكسر;

		حالة '1':
		حالة '2':
		حالة '3':
		حالة '4':
		حالة '5':
		حالة '6':
		حالة '7':
		حالة '8':
		حالة '9': إذا ((reg_do_extmatch & REX_USE) == 0)
			      EMSG_RET_NULL(_(e_z1_z9_not_allowed_هتي));
			  ret = regnode(ZREF + c - '0');
			  re_has_z = REX_USE;
			  اكسر;
#جيڪڏهن_پورو

		حالة 's': ret = regnode(MOPEN + 0);
			  إذا (re_mult_ايندڙ("\\zs") == FAIL)
			      أرجع عدم;
			  اكسر;

		حالة 'e': ret = regnode(MCLOSE + 0);
			  إذا (re_mult_ايندڙ("\\ze") == FAIL)
			      أرجع عدم;
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
		    إذا (one_exactly)
			EMSG_ONE_RET_NULL;
		    ret = reg(REG_NPAREN, &flags);
		    إذا (ret == عدم)
			أرجع عدم;
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
		    إذا (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_شروعات_جو_نمونو),
								  regparse[1]);
			أرجع FAIL;
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
			  إذا (one_exactly)	// doesn't nest
			      EMSG_ONE_RET_NULL;
			  {
			      char_u	*lastbranch;
			      char_u	*lastnode = عدم;
			      char_u	*br;

			      ret = عدم;
			      طالما ((c = getchr()) != ']')
			      {
				  إذا (c == NUL)
				      EMSG2_RET_NULL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
				  br = regnode(BRANCH);
				  إذا (ret == عدم)
				      ret = br;
				  وإلا
				  {
				      regtail(lastnode, br);
				      إذا (reg_toolong)
					  أرجع عدم;
				  }

				  ungetchr();
				  one_exactly = TRUE;
				  lastnode = regatom(flagp);
				  one_exactly = FALSE;
				  إذا (lastnode == عدم)
				      أرجع عدم;
			      }
			      إذا (ret == عدم)
				  EMSG2_RET_NULL(_(e_خالي_جايون_str_brackets),
						      reg_magic == MAGIC_ALL);
			      lastbranch = regnode(BRANCH);
			      br = regnode(NOTHING);
			      إذا (ret != JUST_CALC_سائيز)
			      {
				  regtail(lastnode, br);
				  regtail(lastbranch, br);
				  // connect all branches to the NOTHING
				  // branch at the end
				  لكل (br = ret; br != lastnode; )
				  {
				      إذا (OP(br) == BRANCH)
				      {
					  regtail(br, lastbranch);
					  إذا (reg_toolong)
					      أرجع عدم;
					  br = عمليو(br);
				      }
				      وإلا
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

			      إذا (i < 0 || i > وڌ_کان_وڌ_عدد)
				  EMSG2_RET_NULL(
					    _(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			      إذا (use_multibytecode(i))
				  ret = regnode(MULTIBYTECODE);
			      وإلا
				  ret = regnode(EXACTLY);
			      إذا (i == 0)
				  regc(0x0a);
			      وإلا
				  regmbc((صحيح)i);
			      regc(NUL);
			      *flagp |= HASWIDTH;
			      اكسر;
			  }

		افتراضي:
			  إذا (VIM_ISDIGIT(c) || c == '<' || c == '>'
						|| c == '\'' || c == '.')
			  {
			      long_u	n = 0;
			      صحيح	cmp;
			      صحيح	cur = FALSE;
			      صحيح	got_digit = FALSE;

			      cmp = c;
			      إذا (cmp == '<' || cmp == '>')
				  c = getchr();
			      إذا (no_Magic(c) == '.')
			      {
				  cur = TRUE;
				  c = getchr();
			      }
			      طالما (VIM_ISDIGIT(c))
			      {
				  got_digit = TRUE;
				  n = n * 10 + (c - '0');
				  c = getchr();
			      }
			      إذا (no_Magic(c) == '\'' && n == 0)
			      {
				  // "\%'m", "\%<'m" and "\%>'m": Mark
				  c = getchr();
				  ret = regnode(RE_MARK);
				  إذا (ret == JUST_CALC_سائيز)
				      regsize += 2;
				  وإلا
				  {
				      *regcode++ = c;
				      *regcode++ = cmp;
				  }
				  اكسر;
			      }
			      وإلا إذا ((c == 'l' || c == 'c' || c == 'v')
					  && (cur || got_digit))
			      {
				  إذا (cur && n)
				  {
				    semsg(_(e_regexp_عدد_after_dot_pos_ڳولا_chr),
								  no_Magic(c));
				    rc_did_emsg = TRUE;
				    أرجع عدم;
				  }
				  إذا (c == 'l')
				  {
				      إذا (cur)
					  n = curwin->w_cursor.lnum;
				      ret = regnode(RE_LNUM);
				      إذا (save_prev_at_شروعات)
					  at_شروعات = TRUE;
				  }
				  وإلا إذا (c == 'c')
				  {
				      إذا (cur)
				      {
					  n = curwin->w_cursor.کانو;
					  n++;
				      }
				      ret = regnode(RE_کانو);
				  }
				  وإلا
				  {
				      إذا (cur)
				      {
					  colnr_T vcol = 0;

					  getvvcol(curwin, &curwin->w_cursor,
							 عدم, عدم, &vcol, 0);
					  ++vcol;
					  n = vcol;
				      }
				      ret = regnode(RE_VCOL);
				  }
				  إذا (ret == JUST_CALC_سائيز)
				      regsize += 5;
				  وإلا
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
	    إذا (*lp == ']')	// there is a matching ']'
	    {
		صحيح	startc = -1;	// > 0 when next '-' is a range
		صحيح	endc;

		إذا (extra == 0)
		{
		    صحيح	cl = bt_recognize_char_class(regparse, lp);

		    إذا (cl != 0)
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
		إذا (*regparse == '^')	    // Complement of range.
		{
		    ret = regnode(ANYBUT + extra);
		    regparse++;
		}
		وإلا
		    ret = regnode(ANYOF + extra);

		// At the start ']' and '-' mean the literal character.
		إذا (*regparse == ']' || *regparse == '-')
		{
		    startc = *regparse;
		    regc(*regparse++);
		}

		طالما (*regparse != NUL && *regparse != ']')
		{
		    إذا (*regparse == '-')
		    {
			++regparse;
			// The '-' is not used for a range at the end and
			// after or before a '\n'.
			إذا (*regparse == ']' || *regparse == NUL
				|| startc == -1
				|| (regparse[0] == '\\' && regparse[1] == 'n'))
			{
			    regc('-');
			    startc = '-';	// [--x] is a range
			}
			وإلا
			{
			    // Also accept "a-[.z.]"
			    endc = 0;
			    إذا (*regparse == '[')
				endc = get_coll_element(&regparse);
			    إذا (endc == 0)
			    {
				إذا (has_mbyte)
				    endc = mb_ptr2char_adv(&regparse);
				وإلا
				    endc = *regparse++;
			    }

			    // Handle \o40, \x20 and \u20AC style sequences
			    إذا (endc == '\\' && !reg_cpo_lit && !reg_cpo_bsl)
				endc = coll_get_char();

			    إذا (startc > endc)
				EMSG_RET_NULL(_(e_ابتو_range_اندريون_character_class));
			    إذا (has_mbyte && ((*mb_char2len)(startc) > 1
						 || (*mb_char2len)(endc) > 1))
			    {
				// Limit to a range of 256 chars.
				إذا (endc > startc + 256)
				    EMSG_RET_NULL(_(e_range_too_large_اندريون_character_class));
				طالما (++startc <= endc)
				    regmbc(startc);
			    }
			    وإلا
			    {
				طالما (++startc <= endc)
				    regc(startc);
			    }
			    startc = -1;
			}
		    }
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    وإلا إذا (*regparse == '\\'
			    && !reg_cpo_bsl
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != عدم
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR,
						       regparse[1]) != عدم)))
		    {
			regparse++;
			إذا (*regparse == 'n')
			{
			    // '\n' in range: also match NL
			    إذا (ret != JUST_CALC_سائيز)
			    {
				// Using \n inside [^] does not change what
				// matches. "[^\n]" is the same as ".".
				إذا (*ret == ANYOF)
				{
				    *ret = ANYOF + جوڙيو_NL;
				    *flagp |= HASNL;
				}
				// else: must have had a \n already
			    }
			    regparse++;
			    startc = -1;
			}
			وإلا إذا (*regparse == 'd'
				|| *regparse == 'o'
				|| *regparse == 'x'
				|| *regparse == 'u'
				|| *regparse == 'U')
			{
			    startc = coll_get_char();
			    // max UTF-8 Codepoint is U+10FFFF,
			    // but allow values until INT_MAX
			    إذا (startc == وڌ_کان_وڌ_عدد)
				EMSG_RET_NULL(_(e_unicode_val_too_large));
			    إذا (startc == 0)
				regc(0x0a);
			    وإلا
				regmbc(startc);
			}
			وإلا
			{
			    startc = backslash_trans(*regparse++);
			    regc(startc);
			}
		    }
		    وإلا إذا (*regparse == '[')
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
				إذا (c_class != 0)
				{
				    // produce equivalence class
				    reg_equi_class(c_class);
				}
				وإلا إذا ((c_class =
					    get_coll_element(&regparse)) != 0)
				{
				    // produce a collating element
				    regmbc(c_class);
				}
				وإلا
				{
				    // literal '[', allow [[-x] as a range
				    startc = *regparse++;
				    regc(startc);
				}
				اكسر;
			    حالة CLASS_ALNUM:
				لكل (cu = 1; cu < 128; cu++)
				    إذا (اکر_يا_انگ(cu))
					regmbc(cu);
				اكسر;
			    حالة CLASS_ALPHA:
				لكل (cu = 1; cu < 128; cu++)
				    إذا (اکر_آهي(cu))
					regmbc(cu);
				اكسر;
			    حالة CLASS_BLANK:
				regc(' ');
				regc('\t');
				اكسر;
			    حالة CLASS_CNTRL:
				لكل (cu = 1; cu <= 127; cu++)
				    إذا (iscntrl(cu))
					regmbc(cu);
				اكسر;
			    حالة CLASS_DIGIT:
				لكل (cu = 1; cu <= 127; cu++)
				    إذا (VIM_ISDIGIT(cu))
					regmbc(cu);
				اكسر;
			    حالة CLASS_گراف:
				لكل (cu = 1; cu <= 127; cu++)
				    إذا (isgraph(cu))
					regmbc(cu);
				اكسر;
			    حالة CLASS_هيٺاهون:
				لكل (cu = 1; cu <= 255; cu++)
				    إذا (MB_ISLOWER(cu) && cu != 170
								 && cu != 186)
					regmbc(cu);
				اكسر;
			    حالة CLASS_PRINT:
				لكل (cu = 1; cu <= 255; cu++)
				    إذا (vim_isprintc(cu))
					regmbc(cu);
				اكسر;
			    حالة CLASS_PUNCT:
				لكل (cu = 1; cu < 128; cu++)
				    إذا (ispunct(cu))
					regmbc(cu);
				اكسر;
			    حالة CLASS_SPACE:
				لكل (cu = 9; cu <= 13; cu++)
				    regc(cu);
				regc(' ');
				اكسر;
			    حالة CLASS_مٿانهون:
				لكل (cu = 1; cu <= 255; cu++)
				    إذا (MB_ISUPPER(cu))
					regmbc(cu);
				اكسر;
			    حالة CLASS_XDIGIT:
				لكل (cu = 1; cu <= 255; cu++)
				    إذا (vim_isxdigit(cu))
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
				لكل (cu = 1; cu <= 255; cu++)
				    إذا (vim_isIDc(cu))
					regmbc(cu);
				اكسر;
			    حالة CLASS_KEYWORD:
				لكل (cu = 1; cu <= 255; cu++)
				    إذا (reg_iswordc(cu))
					regmbc(cu);
				اكسر;
			    حالة CLASS_FNAME:
				لكل (cu = 1; cu <= 255; cu++)
				    إذا (vim_isfilec(cu))
					regmbc(cu);
				اكسر;
			}
		    }
		    وإلا
		    {
			إذا (has_mbyte)
			{
			    صحيح	len;

			    // produce a multibyte character, including any
			    // following composing characters
			    startc = mb_ptr2char(regparse);
			    len = (*mb_ptr2len)(regparse);
			    إذا (enc_utf8 && utf_char2len(startc) != len)
				startc = -1;	// composing chars
			    طالما (--len >= 0)
				regc(*regparse++);
			}
			وإلا
			{
			    startc = *regparse++;
			    regc(startc);
			}
		    }
		}
		regc(NUL);
		prevchr_len = 1;	// last char was the ']'
		إذا (*regparse != ']')
		    EMSG_RET_NULL(_(e_too_many_brackets));  // Cannot happen?
		skipchr();	    // let's be friends with the lexer again
		*flagp |= HASWIDTH | SIMPLE;
		اكسر;
	    }
	    وإلا إذا (reg_strict)
		EMSG2_RET_NULL(_(e_missing_rsb_after_str_lsb),
							reg_magic > MAGIC_OFF);
	}
	// FALLTHROUGH

      افتراضي:
	{
	    صحيح		len;

	    // A multi-byte character is handled as a separate atom if it's
	    // before a multi and when it's a composing char.
	    إذا (use_multibytecode(c))
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
	    لكل (len = 0; c != NUL && (len == 0
			|| (re_multi_type(peekchr()) == NOT_MULTI
			    && !one_exactly
			    && !is_Magic(c))); ++len)
	    {
		c = no_Magic(c);
		إذا (has_mbyte)
		{
		    regmbc(c);
		    إذا (enc_utf8)
		    {
			صحيح	l;

			// Need to get composing character too.
			لكل (;;)
			{
			    l = utf_ptr2len(regparse);
			    إذا (!UTF_COMPOSINGLIKE(regparse, regparse + l))
				اكسر;
			    regmbc(utf_ptr2char(regparse));
			    skipchr();
			}
		    }
		}
		وإلا
		    regc(c);
		c = getchr();
	    }
	    ungetchr();

	    regc(NUL);
	    *flagp |= HASWIDTH;
	    إذا (len == 1)
		*flagp |= SIMPLE;
	}
	اكسر;
    }

    أرجع ret;
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
    char_u	    *ايندڙ;
    صحيح		    flags;
    ڊگهو	    minval;
    ڊگهو	    maxval;

    ret = regatom(&flags);
    إذا (ret == عدم)
	أرجع عدم;

    op = peekchr();
    إذا (re_multi_type(op) == NOT_MULTI)
    {
	*flagp = flags;
	أرجع ret;
    }
    // default flags
    *flagp = (WORST | SPSTART | (flags & (HASNL | HASLOOKBH)));

    skipchr();
    بدل (op)
    {
	حالة Magic('*'):
	    إذا (flags & SIMPLE)
		reginsert(تارو, ret);
	    وإلا
	    {
		// Emit x* as (x&|), where & means "self".
		reginsert(BRANCH, ret); // Either x
		regoptail(ret, regnode(پٺ_2));	// and loop
		regoptail(ret, ret);	//پٺ
		regtail(ret, regnode(BRANCH));	// or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    اكسر;

	حالة Magic('+'):
	    إذا (flags & SIMPLE)
		reginsert(PLUS, ret);
	    وإلا
	    {
		// Emit x+ as x(&|), where & means "self".
		ايندڙ = regnode(BRANCH); // Either
		regtail(ret, ايندڙ);
		regtail(regnode(پٺ_2), ret);	// loop back
		regtail(ايندڙ, regnode(BRANCH)); // or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    *flagp = (WORST | HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    اكسر;

	حالة Magic('@'):
	    {
		صحيح	lop = پڇاڙي_3;
		ڊگهو	nr;

		nr = (ڊگهو)getdecchrs();
		بدل (no_Magic(getchr()))
		{
		    حالة '=': lop = ميلاپ_2; اكسر;		  // \@=
		    حالة '!': lop = NOMATCH; اكسر;		  // \@!
		    حالة '>': lop = SUBPAT; اكسر;		  // \@>
		    حالة '<': بدل (no_Magic(getchr()))
			      {
				  حالة '=': lop = BEHIND; اكسر;   // \@<=
				  حالة '!': lop = NOBEHIND; اكسر; // \@<!
			      }
		}
		إذا (lop == پڇاڙي_3)
		    EMSG2_RET_NULL(_(e_invalid_character_after_str_at),
						      reg_magic == MAGIC_ALL);
		// Look behind must match with behind_pos.
		إذا (lop == BEHIND || lop == NOBEHIND)
		{
		    regtail(ret, regnode(BHPOS));
		    *flagp |= HASLOOKBH;
		}
		regtail(ret, regnode(پڇاڙي_3)); // operand ends
		إذا (lop == BEHIND || lop == NOBEHIND)
		{
		    إذا (nr < 0)
			nr = 0; // no limit is same as zero limit
		    reginsert_nr(lop, nr, ret);
		}
		وإلا
		    reginsert(lop, ret);
		اكسر;
	    }

	حالة Magic('?'):
	حالة Magic('='):
	    // Emit x= as (x|)
	    reginsert(BRANCH, ret);		// Either x
	    regtail(ret, regnode(BRANCH));	// or
	    ايندڙ = regnode(NOTHING);		// null.
	    regtail(ret, ايندڙ);
	    regoptail(ret, ايندڙ);
	    اكسر;

	حالة Magic('{'):
	    إذا (!read_limits(&minval, &maxval))
		أرجع عدم;
	    إذا (flags & SIMPLE)
	    {
		reginsert(BRACE_SIMPLE, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
	    }
	    وإلا
	    {
		إذا (انگ_complex_braces >= 10)
		    EMSG2_RET_NULL(_(e_too_many_complex_str_curly),
						      reg_magic == MAGIC_ALL);
		reginsert(BRACE_COMPLEX + انگ_complex_braces, ret);
		regoptail(ret, regnode(پٺ_2));
		regoptail(ret, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
		++انگ_complex_braces;
	    }
	    إذا (minval > 0 && maxval > 0)
		*flagp = (HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    اكسر;
    }
    إذا (re_multi_type(peekchr()) != NOT_MULTI)
    {
	// Can't have a multi follow a multi.
	إذا (peekchr() == Magic('*'))
	    EMSG2_RET_NULL(_(e_nested_str), reg_magic >= MAGIC_ON);
	EMSG3_RET_NULL(_(e_nested_str_chr), reg_magic == MAGIC_ALL,
							  no_Magic(peekchr()));
    }

    أرجع ret;
}

/*
 * Parse one alternative of an | or & operator.
 * Implements the concatenation operator.
 */
    ساكن char_u *
regconcat(صحيح *flagp)
{
    char_u	*پهريون = عدم;
    char_u	*chain = عدم;
    char_u	*latest;
    صحيح		flags;
    صحيح		cont = TRUE;

    *flagp = WORST;		// Tentatively.

    طالما (cont)
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
			    إذا (latest == عدم || reg_toolong)
				أرجع عدم;
			    *flagp |= flags & (HASWIDTH | HASNL | HASLOOKBH);
			    إذا (chain == عدم)	// First piece.
				*flagp |= flags & SPSTART;
			    وإلا
				regtail(chain, latest);
			    chain = latest;
			    إذا (پهريون == عدم)
				پهريون = latest;
			    اكسر;
	}
    }
    إذا (پهريون == عدم)		// Loop ran zero times.
	پهريون = regnode(NOTHING);
    أرجع پهريون;
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
    لكل (;;)
    {
	latest = regconcat(&flags);
	إذا (latest == عدم)
	    أرجع عدم;
	// If one of the branches has width, the whole thing has.  If one of
	// the branches anchors at start-of-line, the whole thing does.
	// If one of the branches uses look-behind, the whole thing does.
	*flagp |= flags & (HASWIDTH | SPSTART | HASLOOKBH);
	// If one of the branches doesn't match a line-break, the whole thing
	// doesn't.
	*flagp &= ~HASNL | (flags & HASNL);
	إذا (chain != عدم)
	    regtail(chain, latest);
	إذا (peekchr() != Magic('&'))
	    اكسر;
	skipchr();
	regtail(latest, regnode(پڇاڙي_3)); // operand ends
	إذا (reg_toolong)
	    اكسر;
	reginsert(ميلاپ_2, latest);
	chain = latest;
    }

    أرجع ret;
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

#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
    إذا (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	إذا (regnzpar >= NSUBEXP)
	    EMSG_RET_NULL(_(e_too_many_z));
	parno = regnzpar;
	regnzpar++;
	ret = regnode(ZOPEN + parno);
    }
    وإلا
#جيڪڏهن_پورو
	إذا (paren == REG_PAREN)
    {
	// Make a MOPEN node.
	إذا (regnpar >= NSUBEXP)
	    EMSG2_RET_NULL(_(e_too_many_str_open), reg_magic == MAGIC_ALL);
	parno = regnpar;
	++regnpar;
	ret = regnode(MOPEN + parno);
    }
    وإلا إذا (paren == REG_NPAREN)
    {
	// Make a NOPEN node.
	ret = regnode(NOPEN);
    }
    وإلا
	ret = عدم;

    إذا (bt_reg_parse_اونهائي >= REG_MAX_PAREN_اونهائي)
	EMSG_RET_NULL(_(e_command_too_complex));
    ++bt_reg_parse_اونهائي;

    // Pick up the branches, linking them together.
    br = regbranch(&flags);
    إذا (br == عدم)
    {
	ret = عدم;
	اذهب theend;
    }
    إذا (ret != عدم)
	regtail(ret, br);	// [MZ]OPEN -> first.
    وإلا
	ret = br;
    // If one of the branches can be zero-width, the whole thing can.
    // If one of the branches has * at start or matches a line-break, the
    // whole thing can.
    إذا (!(flags & HASWIDTH))
	*flagp &= ~HASWIDTH;
    *flagp |= flags & (SPSTART | HASNL | HASLOOKBH);
    طالما (peekchr() == Magic('|'))
    {
	skipchr();
	br = regbranch(&flags);
	إذا (br == عدم || reg_toolong)
	{
	    ret = عدم;
	    اذهب theend;
	}
	regtail(ret, br);	// BRANCH -> BRANCH.
	إذا (!(flags & HASWIDTH))
	    *flagp &= ~HASWIDTH;
	*flagp |= flags & (SPSTART | HASNL | HASLOOKBH);
    }

    // Make a closing node, and hook it on the end.
    ender = regnode(
#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
	    paren == REG_ZPAREN ? ZCLOSE + parno :
#جيڪڏهن_پورو
	    paren == REG_PAREN ? MCLOSE + parno :
	    paren == REG_NPAREN ? NCLOSE : پڇاڙي_3);
    regtail(ret, ender);

    // Hook the tails of the branches to the closing node.
    لكل (br = ret; br != عدم; br = regnext(br))
	regoptail(br, ender);

    // Check for proper termination.
    إذا (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
	إذا (paren == REG_ZPAREN)
	{
	    emsg(_(e_unmatched_z));
	    rc_did_emsg = TRUE;
	    ret = عدم;
	    اذهب theend;
	}
	وإلا
#جيڪڏهن_پورو
	إذا (paren == REG_NPAREN)
	{
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = عدم;
	    اذهب theend;
	}
	وإلا
	{
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = عدم;
	    اذهب theend;
	}
    }
    وإلا إذا (paren == REG_NOPAREN && peekchr() != NUL)
    {
	إذا (curchr == Magic(')'))
	{
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = عدم;
	    اذهب theend;
	}
	وإلا
	{
	    emsg(_(e_trailing_characters));	// "Can't happen".
	    rc_did_emsg = TRUE;
	    ret = عدم;
	    اذهب theend;
	}
    }
    // Here we set the flag allowing back references to this set of
    // parentheses.
    إذا (paren == REG_PAREN)
	had_endbrace[parno] = TRUE;	// have seen the close paren

theend:
    --bt_reg_parse_اونهائي;
    أرجع ret;
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
bt_regcomp(char_u *اظهار, صحيح re_flags)
{
    bt_regprog_T    *r;
    char_u	*جاچ;
    char_u	*longest;
    صحيح		len;
    صحيح		flags;

    إذا (اظهار == عدم)
	IEMSG_RET_NULL(e_null_دليل);

    init_class_tab();

    // First pass: determine size, legality.
    regcomp_شروعات(اظهار, re_flags);
    regcode = JUST_CALC_سائيز;
    regc(REGMAGIC);
    إذا (reg(REG_NOPAREN, &flags) == عدم)
	أرجع عدم;

    // Allocate space.
    r = alloc(offsetof(bt_regprog_T, پروگرام) + regsize);
    إذا (r == عدم)
	أرجع عدم;
    r->re_اندريون_use = FALSE;
#جيڪڏهن_وصف_ٿيل DEBUG
    r->regsz = regsize;
#جيڪڏهن_پورو

    // Second pass: emit code.
    regcomp_شروعات(اظهار, re_flags);
    regcode = r->پروگرام;
    regc(REGMAGIC);
    إذا (reg(REG_NOPAREN, &flags) == عدم || reg_toolong)
    {
	vim_free(r);
	إذا (reg_toolong)
	    EMSG_RET_NULL(_(e_نمونو_too_long));
	أرجع عدم;
    }

    // Dig out information for optimizations.
    r->regstart = NUL;		// Worst-case defaults.
    r->reganch = 0;
    r->regmust = عدم;
    r->regmlen = 0;
    r->regflags = regflags;
    إذا (flags & HASNL)
	r->regflags |= RF_HASNL;
    إذا (flags & HASLOOKBH)
	r->regflags |= RF_LOOKBH;
#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    r->reghasz = re_has_z;
#جيڪڏهن_پورو
    جاچ = r->پروگرام + 1;	// First BRANCH.
    إذا (OP(regnext(جاچ)) == پڇاڙي_3)   // Only one top-level choice.
    {
	جاچ = عمليو(جاچ);

	// Starting-point info.
	إذا (OP(جاچ) == BOL || OP(جاچ) == RE_BOF)
	{
	    r->reganch++;
	    جاچ = regnext(جاچ);
	}

	إذا (OP(جاچ) == EXACTLY)
	{
	    إذا (has_mbyte)
		r->regstart = (*mb_ptr2char)(عمليو(جاچ));
	    وإلا
		r->regstart = *عمليو(جاچ);
	}
	وإلا إذا ((OP(جاچ) == BOW
		    || OP(جاچ) == EOW
		    || OP(جاچ) == NOTHING
		    || OP(جاچ) == MOPEN + 0 || OP(جاچ) == NOPEN
		    || OP(جاچ) == MCLOSE + 0 || OP(جاچ) == NCLOSE)
		 && OP(regnext(جاچ)) == EXACTLY)
	{
	    إذا (has_mbyte)
		r->regstart = (*mb_ptr2char)(عمليو(regnext(جاچ)));
	    وإلا
		r->regstart = *عمليو(regnext(جاچ));
	}

	// If there's something expensive in the r.e., find the longest
	// literal string that must appear and make it the regmust.  Resolve
	// ties in favor of later strings, since the regstart check works
	// with the beginning of the r.e. and avoiding duplication
	// strengthens checking.  Not a strong reason, but sufficient in the
	// absence of others.

	// When the r.e. starts with BOW, it is faster to look for a regmust
	// first. Used a lot for "#" and "*" commands. (Added by mool).
	إذا ((flags & SPSTART || OP(جاچ) == BOW || OP(جاچ) == EOW)
							  && !(flags & HASNL))
	{
	    size_t  scanlen;

	    longest = عدم;
	    len = 0;
	    لكل (; جاچ != عدم; جاچ = regnext(جاچ))
	    {
		إذا (OP(جاچ) == EXACTLY)
		{
		    scanlen = STRLEN(عمليو(جاچ));
		    إذا (scanlen >= (size_t)len)
		    {
			longest = عمليو(جاچ);
			len = (صحيح)scanlen;
		    }
		}
	    }
	    r->regmust = longest;
	    r->regmlen = len;
	}
    }
#جيڪڏهن_وصف_ٿيل BT_REGEXP_DUMP
    regdump(اظهار, r);
#جيڪڏهن_پورو
    r->engine = &bt_regengine;
    أرجع (regprog_T *)r;
}

#إذا defined(FEAT_SYN_HL)
/*
 * Check if during the previous call to vim_regcomp the EOL item "$" has been
 * found.  This is messy, but it works fine.
 */
    صحيح
vim_regcomp_had_eol(فراغ)
{
    أرجع had_eol;
}
#جيڪڏهن_پورو

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
    إذا (nr < 0)
    {
	// If getting the number fails be backwards compatible: the character
	// is a backslash.
	--regparse;
	nr = '\\';
    }
    إذا (nr > وڌ_کان_وڌ_عدد)
	nr = وڌ_کان_وڌ_عدد;
    أرجع nr;
}

/*
 * Free a compiled regexp program, returned by bt_regcomp().
 */
    ساكن فراغ
bt_regfree(regprog_T *prog)
{
    vim_free(prog);
}

#وصف_ڪريو ADVANCE_REGINPUT() MB_PTR_ADV(rex.داخلا_2)

/*
 * The arguments from BRACE_LIMITS are stored here.  They are actually local
 * to regmatch(), but they are here to reduce the amount of stack space used
 * (it can be called recursively many times).
 */
ساكن ڊگهو	bl_minval;
ساكن ڊگهو	bl_maxval;

/*
 * Save the input line and position in a regsave_T.
 */
    ساكن فراغ
reg_save(regsave_T *save, garray_T *gap)
{
    إذا (REG_MULTI)
    {
	save->rs_u.pos.کانو = (colnr_T)(rex.داخلا_2 - rex.سٽ);
	save->rs_u.pos.lnum = rex.lnum;
    }
    وإلا
	save->rs_u.ptr = rex.داخلا_2;
    save->rs_len = gap->ga_len;
}

/*
 * Restore the input line and position from a regsave_T.
 */
    ساكن فراغ
reg_restore(regsave_T *save, garray_T *gap)
{
    إذا (REG_MULTI)
    {
	إذا (rex.lnum != save->rs_u.pos.lnum)
	{
	    // only call reg_getline() when the line number changed to save
	    // a bit of time
	    rex.lnum = save->rs_u.pos.lnum;
	    rex.سٽ = reg_getline(rex.lnum);
	}
	rex.داخلا_2 = rex.سٽ + save->rs_u.pos.کانو;
    }
    وإلا
	rex.داخلا_2 = save->rs_u.ptr;
    gap->ga_len = save->rs_len;
}

/*
 * Return TRUE if current position is equal to saved position.
 */
    ساكن صحيح
reg_save_equal(regsave_T *save)
{
    إذا (REG_MULTI)
	أرجع rex.lnum == save->rs_u.pos.lnum
				  && rex.داخلا_2 == rex.سٽ + save->rs_u.pos.کانو;
    أرجع rex.داخلا_2 == save->rs_u.ptr;
}

// Save the sub-expressions before attempting a match.
#وصف_ڪريو save_se(savep, posp, pp) \
    REG_MULTI ? save_se_multi((savep), (posp)) : save_se_one((savep), (pp))

// After a failed match restore the sub-expressions.
#وصف_ڪريو restore_se(savep, posp, pp) \
{ \
    إذا (REG_MULTI) \
	*(posp) = (savep)->se_u.pos; \
    وإلا \
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
    posp->کانو = (colnr_T)(rex.داخلا_2 - rex.سٽ);
}

    ساكن فراغ
save_se_one(save_se_T *savep, char_u **pp)
{
    savep->se_u.ptr = *pp;
    *pp = rex.داخلا_2;
}

/*
 * regrepeat - repeatedly match something simple, return how many.
 * Advances rex.input (and rex.lnum) to just after the matched chars.
 */
    ساكن صحيح
regrepeat(
    char_u	*p,
    ڊگهو	maxcount)   // maximum number of matches allowed
{
    ڊگهو	ڳڻپ = 0;
    char_u	*جاچ;
    char_u	*opnd;
    صحيح		نقاب;
    صحيح		testval = 0;

    جاچ = rex.داخلا_2;	    // Make local copy of rex.input for speed.
    opnd = عمليو(p);
    بدل (OP(p))
    {
      حالة ANY:
      حالة ANY + جوڙيو_NL:
	طالما (ڳڻپ < maxcount)
	{
	    // Matching anything means we continue until end-of-line (or
	    // end-of-file for ANY + ADD_NL), only limited by maxcount.
	    طالما (*جاچ != NUL && ڳڻپ < maxcount)
	    {
		++ڳڻپ;
		MB_PTR_ADV(جاچ);
	    }
	    إذا (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
				      || rex.reg_سٽ_lbr || ڳڻپ == maxcount)
		اكسر;
	    ++ڳڻپ;		// count the line-break
	    reg_nextline();
	    جاچ = rex.داخلا_2;
	    إذا (got_int)
		اكسر;
	}
	اكسر;

      حالة IDENT:
      حالة IDENT + جوڙيو_NL:
	testval = TRUE;
	// FALLTHROUGH
      حالة SIDENT:
      حالة SIDENT + جوڙيو_NL:
	طالما (ڳڻپ < maxcount)
	{
	    إذا (vim_isIDc(PTR2CHAR(جاچ)) && (testval || !VIM_ISDIGIT(*جاچ)))
	    {
		MB_PTR_ADV(جاچ);
	    }
	    وإلا إذا (*جاچ == NUL)
	    {
		إذا (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_سٽ_lbr)
		    اكسر;
		reg_nextline();
		جاچ = rex.داخلا_2;
		إذا (got_int)
		    اكسر;
	    }
	    وإلا إذا (rex.reg_سٽ_lbr && *جاچ == '\n' && WITH_NL(OP(p)))
		++جاچ;
	    وإلا
		اكسر;
	    ++ڳڻپ;
	}
	اكسر;

      حالة KWORD:
      حالة KWORD + جوڙيو_NL:
	testval = TRUE;
	// FALLTHROUGH
      حالة SKWORD:
      حالة SKWORD + جوڙيو_NL:
	طالما (ڳڻپ < maxcount)
	{
	    إذا (vim_iswordp_buf(جاچ, rex.reg_buf)
					  && (testval || !VIM_ISDIGIT(*جاچ)))
	    {
		MB_PTR_ADV(جاچ);
	    }
	    وإلا إذا (*جاچ == NUL)
	    {
		إذا (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_سٽ_lbr)
		    اكسر;
		reg_nextline();
		جاچ = rex.داخلا_2;
		إذا (got_int)
		    اكسر;
	    }
	    وإلا إذا (rex.reg_سٽ_lbr && *جاچ == '\n' && WITH_NL(OP(p)))
		++جاچ;
	    وإلا
		اكسر;
	    ++ڳڻپ;
	}
	اكسر;

      حالة FNAME:
      حالة FNAME + جوڙيو_NL:
	testval = TRUE;
	// FALLTHROUGH
      حالة SFNAME:
      حالة SFNAME + جوڙيو_NL:
	طالما (ڳڻپ < maxcount)
	{
	    إذا (vim_isfilec(PTR2CHAR(جاچ)) && (testval || !VIM_ISDIGIT(*جاچ)))
	    {
		MB_PTR_ADV(جاچ);
	    }
	    وإلا إذا (*جاچ == NUL)
	    {
		إذا (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_سٽ_lbr)
		    اكسر;
		reg_nextline();
		جاچ = rex.داخلا_2;
		إذا (got_int)
		    اكسر;
	    }
	    وإلا إذا (rex.reg_سٽ_lbr && *جاچ == '\n' && WITH_NL(OP(p)))
		++جاچ;
	    وإلا
		اكسر;
	    ++ڳڻپ;
	}
	اكسر;

      حالة PRINT:
      حالة PRINT + جوڙيو_NL:
	testval = TRUE;
	// FALLTHROUGH
      حالة SPRINT:
      حالة SPRINT + جوڙيو_NL:
	طالما (ڳڻپ < maxcount)
	{
	    إذا (*جاچ == NUL)
	    {
		إذا (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_سٽ_lbr)
		    اكسر;
		reg_nextline();
		جاچ = rex.داخلا_2;
		إذا (got_int)
		    اكسر;
	    }
	    وإلا إذا (vim_isprintc(PTR2CHAR(جاچ)) == 1
					  && (testval || !VIM_ISDIGIT(*جاچ)))
	    {
		MB_PTR_ADV(جاچ);
	    }
	    وإلا إذا (rex.reg_سٽ_lbr && *جاچ == '\n' && WITH_NL(OP(p)))
		++جاچ;
	    وإلا
		اكسر;
	    ++ڳڻپ;
	}
	اكسر;

      حالة WHITE:
      حالة WHITE + جوڙيو_NL:
	testval = نقاب = RI_WHITE;
do_class:
	طالما (ڳڻپ < maxcount)
	{
	    صحيح		l;

	    إذا (*جاچ == NUL)
	    {
		إذا (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_سٽ_lbr)
		    اكسر;
		reg_nextline();
		جاچ = rex.داخلا_2;
		إذا (got_int)
		    اكسر;
	    }
	    وإلا إذا (has_mbyte && (l = (*mb_ptr2len)(جاچ)) > 1)
	    {
		إذا (testval != 0)
		    اكسر;
		جاچ += l;
	    }
	    وإلا إذا ((class_tab[*جاچ] & نقاب) == testval)
		++جاچ;
	    وإلا إذا (rex.reg_سٽ_lbr && *جاچ == '\n' && WITH_NL(OP(p)))
		++جاچ;
	    وإلا
		اكسر;
	    ++ڳڻپ;
	}
	اكسر;

      حالة NWHITE:
      حالة NWHITE + جوڙيو_NL:
	نقاب = RI_WHITE;
	اذهب do_class;
      حالة DIGIT:
      حالة DIGIT + جوڙيو_NL:
	testval = نقاب = RI_DIGIT;
	اذهب do_class;
      حالة NDIGIT:
      حالة NDIGIT + جوڙيو_NL:
	نقاب = RI_DIGIT;
	اذهب do_class;
      حالة HEX:
      حالة HEX + جوڙيو_NL:
	testval = نقاب = RI_HEX;
	اذهب do_class;
      حالة NHEX:
      حالة NHEX + جوڙيو_NL:
	نقاب = RI_HEX;
	اذهب do_class;
      حالة OCTAL:
      حالة OCTAL + جوڙيو_NL:
	testval = نقاب = RI_OCTAL;
	اذهب do_class;
      حالة NOCTAL:
      حالة NOCTAL + جوڙيو_NL:
	نقاب = RI_OCTAL;
	اذهب do_class;
      حالة لفظ_2:
      حالة لفظ_2 + جوڙيو_NL:
	testval = نقاب = RI_لفظ;
	اذهب do_class;
      حالة NWORD:
      حالة NWORD + جوڙيو_NL:
	نقاب = RI_لفظ;
	اذهب do_class;
      حالة سر_2:
      حالة سر_2 + جوڙيو_NL:
	testval = نقاب = RI_سر;
	اذهب do_class;
      حالة NHEAD:
      حالة NHEAD + جوڙيو_NL:
	نقاب = RI_سر;
	اذهب do_class;
      حالة ALPHA:
      حالة ALPHA + جوڙيو_NL:
	testval = نقاب = RI_ALPHA;
	اذهب do_class;
      حالة NALPHA:
      حالة NALPHA + جوڙيو_NL:
	نقاب = RI_ALPHA;
	اذهب do_class;
      حالة هيٺاهون_2:
      حالة هيٺاهون_2 + جوڙيو_NL:
	testval = نقاب = RI_هيٺاهون;
	اذهب do_class;
      حالة NLOWER:
      حالة NLOWER + جوڙيو_NL:
	نقاب = RI_هيٺاهون;
	اذهب do_class;
      حالة مٿانهون:
      حالة مٿانهون + جوڙيو_NL:
	testval = نقاب = RI_مٿانهون;
	اذهب do_class;
      حالة NUPPER:
      حالة NUPPER + جوڙيو_NL:
	نقاب = RI_مٿانهون;
	اذهب do_class;

      حالة EXACTLY:
	{
	    صحيح	    cu, cl;

	    // This doesn't do a multi-byte character, because a MULTIBYTECODE
	    // would have been used for it.  It does handle single-byte
	    // characters, such as latin1.
	    إذا (rex.reg_ic)
	    {
		cu = MB_TOUPPER(*opnd);
		cl = MB_TOLOWER(*opnd);
		طالما (ڳڻپ < maxcount && (*جاچ == cu || *جاچ == cl))
		{
		    ڳڻپ++;
		    جاچ++;
		}
	    }
	    وإلا
	    {
		cu = *opnd;
		طالما (ڳڻپ < maxcount && *جاچ == cu)
		{
		    ڳڻپ++;
		    جاچ++;
		}
	    }
	    اكسر;
	}

      حالة MULTIBYTECODE:
	{
	    صحيح		i, len, cf = 0;

	    // Safety check (just in case 'encoding' was changed since
	    // compiling the program).
	    إذا ((len = (*mb_ptr2len)(opnd)) > 1)
	    {
		إذا (rex.reg_ic && enc_utf8)
		    cf = utf_fold(utf_ptr2char(opnd));
		طالما (ڳڻپ < maxcount && (*mb_ptr2len)(جاچ) >= len)
		{
		    لكل (i = 0; i < len; ++i)
			إذا (opnd[i] != جاچ[i])
			    اكسر;
		    إذا (i < len && (!rex.reg_ic || !enc_utf8
					|| utf_fold(utf_ptr2char(جاچ)) != cf))
			اكسر;
		    جاچ += len;
		    ++ڳڻپ;
		}
	    }
	}
	اكسر;

      حالة ANYOF:
      حالة ANYOF + جوڙيو_NL:
	testval = TRUE;
	// FALLTHROUGH

      حالة ANYBUT:
      حالة ANYBUT + جوڙيو_NL:
	طالما (ڳڻپ < maxcount)
	{
	    صحيح len;

	    إذا (*جاچ == NUL)
	    {
		إذا (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_سٽ_lbr)
		    اكسر;
		reg_nextline();
		جاچ = rex.داخلا_2;
		إذا (got_int)
		    اكسر;
	    }
	    وإلا إذا (rex.reg_سٽ_lbr && *جاچ == '\n' && WITH_NL(OP(p)))
		++جاچ;
	    وإلا إذا (has_mbyte && (len = (*mb_ptr2len)(جاچ)) > 1)
	    {
		إذا ((cstrchr(opnd, (*mb_ptr2char)(جاچ)) == عدم) == testval)
		    اكسر;
		جاچ += len;
	    }
	    وإلا
	    {
		إذا ((cstrchr(opnd, *جاچ) == عدم) == testval)
		    اكسر;
		++جاچ;
	    }
	    ++ڳڻپ;
	}
	اكسر;

      حالة NEWL:
	طالما (ڳڻپ < maxcount
		&& ((*جاچ == NUL && rex.lnum <= rex.reg_maxline
				       && !rex.reg_سٽ_lbr && REG_MULTI)
		    || (*جاچ == '\n' && rex.reg_سٽ_lbr)))
	{
	    ڳڻپ++;
	    إذا (rex.reg_سٽ_lbr)
		ADVANCE_REGINPUT();
	    وإلا
		reg_nextline();
	    جاچ = rex.داخلا_2;
	    إذا (got_int)
		اكسر;
	}
	اكسر;

      افتراضي:			// Oh dear.  Called inappropriately.
	iemsg(e_corrupted_regexp_پروگرام);
#جيڪڏهن_وصف_ٿيل DEBUG
	لکو("Called regrepeat with op code %d\n", OP(p));
#جيڪڏهن_پورو
	اكسر;
    }

    rex.داخلا_2 = جاچ;

    أرجع (صحيح)ڳڻپ;
}

/*
 * Push an item onto the regstack.
 * Returns pointer to new item.  Returns NULL when out of memory.
 */
    ساكن regitem_T *
regstack_ڌڪو(regstate_T state, char_u *جاچ)
{
    regitem_T	*rp;

    إذا ((ڊگهو)((بنانشان)regstack.ga_len >> 10) >= p_mmp)
    {
	emsg(_(e_نمونو_استعمال_ڪري_وڌيڪ_يادگيري_than_maxmempattern));
	أرجع عدم;
    }
    إذا (GA_GROW_FAILS(&regstack, (صحيح)حجم(regitem_T)))
	أرجع عدم;

    rp = (regitem_T *)((حرف *)regstack.ga_معلومات + regstack.ga_len);
    rp->rs_state = state;
    rp->rs_جاچ = جاچ;

    regstack.ga_len += حجم(regitem_T);
    أرجع rp;
}

/*
 * Pop an item from the regstack.
 */
    ساكن فراغ
regstack_ڪڍو(char_u **جاچ)
{
    regitem_T	*rp;

    rp = (regitem_T *)((حرف *)regstack.ga_معلومات + regstack.ga_len) - 1;
    *جاچ = rp->rs_جاچ;

    regstack.ga_len -= حجم(regitem_T);
}

#جيڪڏهن_وصف_ٿيل FEAT_RELTIME
/*
 * Check if the timer expired, return TRUE if so.
 */
    ساكن صحيح
bt_did_وقت_ٻاهريون(صحيح *timed_ٻاهريون)
{
    إذا (*timeout_flag)
    {
	إذا (timed_ٻاهريون != عدم)
	{
# ifdef FEAT_EVAL
	    إذا (!*timed_ٻاهريون)
		ch_log(عدم, "BT regexp timed out");
# endif
	    *timed_ٻاهريون = TRUE;
	}
	أرجع TRUE;
    }
    أرجع FALSE;
}
#جيڪڏهن_پورو

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
    إذا (rex.need_clear_subexpr)
	أرجع;

    لكل (i = 0; i < NSUBEXP; ++i)
    {
	إذا (REG_MULTI)
	{
	    bp->save_شروعات[i].se_u.pos = rex.reg_startpos[i];
	    bp->save_پڇاڙي[i].se_u.pos = rex.reg_endpos[i];
	}
	وإلا
	{
	    bp->save_شروعات[i].se_u.ptr = rex.reg_startp[i];
	    bp->save_پڇاڙي[i].se_u.ptr = rex.reg_endp[i];
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
    إذا (rex.need_clear_subexpr)
	أرجع;

    لكل (i = 0; i < NSUBEXP; ++i)
    {
	إذا (REG_MULTI)
	{
	    rex.reg_startpos[i] = bp->save_شروعات[i].se_u.pos;
	    rex.reg_endpos[i] = bp->save_پڇاڙي[i].se_u.pos;
	}
	وإلا
	{
	    rex.reg_startp[i] = bp->save_شروعات[i].se_u.ptr;
	    rex.reg_endp[i] = bp->save_پڇاڙي[i].se_u.ptr;
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
    char_u	*جاچ,		    // Current node.
    صحيح		*timed_ٻاهريون اڻ_استعمال_ٿيل)  // flag set on timeout or NULL
{
  char_u	*ايندڙ;		// Next node.
  صحيح		op;
  صحيح		c;
  regitem_T	*rp;
  صحيح		no;
  صحيح		حالت;		// one of the RA_ values:

  // Make "regstack" and "backpos" empty.  They are allocated and freed in
  // bt_regexec_both() to reduce malloc()/free() calls.
  regstack.ga_len = 0;
  backpos.ga_len = 0;

  // Repeat until "regstack" is empty.
  لكل (;;)
  {
    // Some patterns may take a long time to match, e.g., "\([a-z]\+\)\+Q".
    // Allow interrupting them with CTRL-C.
    fast_breakcheck();

#جيڪڏهن_وصف_ٿيل DEBUG
    إذا (جاچ != عدم && regnarrate)
    {
	mch_errmsg((حرف *)regprop(جاچ));
	mch_errmsg("(\n");
    }
#جيڪڏهن_پورو

    // Repeat for items that can be matched sequentially, without using the
    // regstack.
    لكل (;;)
    {
	إذا (got_int || جاچ == عدم)
	{
	    حالت = RA_FAIL;
	    اكسر;
	}
#جيڪڏهن_وصف_ٿيل FEAT_RELTIME
	إذا (bt_did_وقت_ٻاهريون(timed_ٻاهريون))
	{
	    حالت = RA_FAIL;
	    اكسر;
	}
#جيڪڏهن_پورو
	حالت = RA_CONT;

#جيڪڏهن_وصف_ٿيل DEBUG
	إذا (regnarrate)
	{
	    mch_errmsg((حرف *)regprop(جاچ));
	    mch_errmsg("...\n");
# ifdef FEAT_SYN_HL
	    إذا (re_extmatch_اندريون != عدم)
	    {
		صحيح i;

		mch_errmsg(_("External submatches:\n"));
		لكل (i = 0; i < NSUBEXP; i++)
		{
		    mch_errmsg("    \"");
		    إذا (re_extmatch_اندريون->matches[i] != عدم)
			mch_errmsg((حرف *)re_extmatch_اندريون->matches[i]);
		    mch_errmsg("\"\n");
		}
	    }
# endif
	}
#جيڪڏهن_پورو
	ايندڙ = regnext(جاچ);

	op = OP(جاچ);
	// Check for character class with NL added.
	إذا (!rex.reg_سٽ_lbr && WITH_NL(op) && REG_MULTI
			     && *rex.داخلا_2 == NUL && rex.lnum <= rex.reg_maxline)
	{
	    reg_nextline();
	}
	وإلا إذا (rex.reg_سٽ_lbr && WITH_NL(op) && *rex.داخلا_2 == '\n')
	{
	    ADVANCE_REGINPUT();
	}
	وإلا
	{
	  إذا (WITH_NL(op))
	      op -= جوڙيو_NL;
	  إذا (has_mbyte)
	      c = (*mb_ptr2char)(rex.داخلا_2);
	  وإلا
	      c = *rex.داخلا_2;
	  بدل (op)
	  {
	  حالة BOL:
	    إذا (rex.داخلا_2 != rex.سٽ)
		حالت = RA_NOMATCH;
	    اكسر;

	  حالة EOL:
	    إذا (c != NUL)
		حالت = RA_NOMATCH;
	    اكسر;

	  حالة RE_BOF:
	    // We're not at the beginning of the file when below the first
	    // line where we started, not at the start of the line or we
	    // didn't start at the first line of the buffer.
	    إذا (rex.lnum != 0 || rex.داخلا_2 != rex.سٽ
				       || (REG_MULTI && rex.reg_firstlnum > 1))
		حالت = RA_NOMATCH;
	    اكسر;

	  حالة RE_EOF:
	    إذا (rex.lnum != rex.reg_maxline || c != NUL)
		حالت = RA_NOMATCH;
	    اكسر;

	  حالة CURSOR:
	    // Check if the buffer is in a window and compare the
	    // rex.reg_win->w_cursor position to the match position.
	    إذا (rex.reg_win == عدم
		    || (rex.lnum + rex.reg_firstlnum
						 != rex.reg_win->w_cursor.lnum)
		    || ((colnr_T)(rex.داخلا_2 - rex.سٽ)
						 != rex.reg_win->w_cursor.کانو))
		حالت = RA_NOMATCH;
	    اكسر;

	  حالة RE_MARK:
	    // Compare the mark position to the match position.
	    {
		صحيح	mark = عمليو(جاچ)[0];
		صحيح	cmp = عمليو(جاچ)[1];
		pos_T	*pos;
		size_t	کانو = REG_MULTI ? rex.داخلا_2 - rex.سٽ : 0;

		pos = getmark_buf(rex.reg_buf, mark, FALSE);

		// Line may have been freed, get it again.
		إذا (REG_MULTI)
		{
		    rex.سٽ = reg_getline(rex.lnum);
		    rex.داخلا_2 = rex.سٽ + کانو;
		}

		إذا (pos == عدم		     // mark doesn't exist
			|| pos->lnum <= 0)   // mark isn't set in reg_buf
		{
		    حالت = RA_NOMATCH;
		}
		وإلا
		{
		    colnr_T pos_کانو = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->کانو == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->کانو;

		    إذا ((pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_کانو == (colnr_T)(rex.داخلا_2 - rex.سٽ)
				    ? (cmp == '<' || cmp == '>')
				    : (pos_کانو < (colnr_T)(rex.داخلا_2 - rex.سٽ)
					? cmp != '>'
					: cmp != '<'))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? cmp != '>'
				    : cmp != '<')))
		    حالت = RA_NOMATCH;
		}
	    }
	    اكسر;

	  حالة RE_VISUAL:
	    إذا (!reg_ميلاپ_visual())
		حالت = RA_NOMATCH;
	    اكسر;

	  حالة RE_LNUM:
	    إذا (!REG_MULTI || !re_انگ_cmp((long_u)(rex.lnum + rex.reg_firstlnum),
									جاچ))
		حالت = RA_NOMATCH;
	    اكسر;

	  حالة RE_کانو:
	    إذا (!re_انگ_cmp((long_u)(rex.داخلا_2 - rex.سٽ) + 1, جاچ))
		حالت = RA_NOMATCH;
	    اكسر;

	  حالة RE_VCOL:
	    {
		win_T	    *wp = rex.reg_win == عدم ? curwin : rex.reg_win;
		linenr_T    lnum = REG_MULTI ? rex.reg_firstlnum + rex.lnum : 1;
		long_u	    vcol;

		إذا (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_بفر->b_ml.ml_سٽ_ڳڻپ))
		    lnum = 1;
		vcol = (long_u)win_linetabsize(wp, lnum, rex.سٽ,
					      (colnr_T)(rex.داخلا_2 - rex.سٽ));
		إذا (!re_انگ_cmp(vcol + 1, جاچ))
		    حالت = RA_NOMATCH;
	    }
	    اكسر;

	  حالة BOW:	// \<word; rex.input points to w
	    إذا (c == NUL)	// Can't match at end of line
		حالت = RA_NOMATCH;
	    وإلا إذا (has_mbyte)
	    {
		صحيح this_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.داخلا_2, rex.reg_buf);
		إذا (this_class <= 1)
		    حالت = RA_NOMATCH;  // not on a word at all
		وإلا إذا (reg_prev_class() == this_class)
		    حالت = RA_NOMATCH;  // previous char is in same word
	    }
	    وإلا
	    {
		إذا (!vim_iswordc_buf(c, rex.reg_buf) || (rex.داخلا_2 > rex.سٽ
				&& vim_iswordc_buf(rex.داخلا_2[-1], rex.reg_buf)))
		    حالت = RA_NOMATCH;
	    }
	    اكسر;

	  حالة EOW:	// word\>; rex.input points after d
	    إذا (rex.داخلا_2 == rex.سٽ)    // Can't match at start of line
		حالت = RA_NOMATCH;
	    وإلا إذا (has_mbyte)
	    {
		صحيح this_class, prev_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.داخلا_2, rex.reg_buf);
		prev_class = reg_prev_class();
		إذا (this_class == prev_class
			|| prev_class == 0 || prev_class == 1)
		    حالت = RA_NOMATCH;
	    }
	    وإلا
	    {
		إذا (!vim_iswordc_buf(rex.داخلا_2[-1], rex.reg_buf)
			|| (rex.داخلا_2[0] != NUL
					   && vim_iswordc_buf(c, rex.reg_buf)))
		    حالت = RA_NOMATCH;
	    }
	    اكسر; // Matched with EOW

	  حالة ANY:
	    // ANY does not match new lines.
	    إذا (c == NUL)
		حالت = RA_NOMATCH;
	    وإلا
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة IDENT:
	    إذا (!vim_isIDc(c))
		حالت = RA_NOMATCH;
	    وإلا
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة SIDENT:
	    إذا (VIM_ISDIGIT(*rex.داخلا_2) || !vim_isIDc(c))
		حالت = RA_NOMATCH;
	    وإلا
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة KWORD:
	    إذا (!vim_iswordp_buf(rex.داخلا_2, rex.reg_buf))
		حالت = RA_NOMATCH;
	    وإلا
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة SKWORD:
	    إذا (VIM_ISDIGIT(*rex.داخلا_2)
				    || !vim_iswordp_buf(rex.داخلا_2, rex.reg_buf))
		حالت = RA_NOMATCH;
	    وإلا
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة FNAME:
	    إذا (!vim_isfilec(c))
		حالت = RA_NOMATCH;
	    وإلا
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة SFNAME:
	    إذا (VIM_ISDIGIT(*rex.داخلا_2) || !vim_isfilec(c))
		حالت = RA_NOMATCH;
	    وإلا
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة PRINT:
	    إذا (!vim_isprintc(PTR2CHAR(rex.داخلا_2)))
		حالت = RA_NOMATCH;
	    وإلا
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة SPRINT:
	    إذا (VIM_ISDIGIT(*rex.داخلا_2) || !vim_isprintc(PTR2CHAR(rex.داخلا_2)))
		حالت = RA_NOMATCH;
	    وإلا
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة WHITE:
	    إذا (!VIM_ISWHITE(c))
		حالت = RA_NOMATCH;
	    وإلا
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة NWHITE:
	    إذا (c == NUL || VIM_ISWHITE(c))
		حالت = RA_NOMATCH;
	    وإلا
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة DIGIT:
	    إذا (!ri_digit(c))
		حالت = RA_NOMATCH;
	    وإلا
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة NDIGIT:
	    إذا (c == NUL || ri_digit(c))
		حالت = RA_NOMATCH;
	    وإلا
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة HEX:
	    إذا (!ri_hex(c))
		حالت = RA_NOMATCH;
	    وإلا
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة NHEX:
	    إذا (c == NUL || ri_hex(c))
		حالت = RA_NOMATCH;
	    وإلا
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة OCTAL:
	    إذا (!ri_octal(c))
		حالت = RA_NOMATCH;
	    وإلا
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة NOCTAL:
	    إذا (c == NUL || ri_octal(c))
		حالت = RA_NOMATCH;
	    وإلا
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة لفظ_2:
	    إذا (!ri_لفظ(c))
		حالت = RA_NOMATCH;
	    وإلا
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة NWORD:
	    إذا (c == NUL || ri_لفظ(c))
		حالت = RA_NOMATCH;
	    وإلا
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة سر_2:
	    إذا (!ri_سر(c))
		حالت = RA_NOMATCH;
	    وإلا
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة NHEAD:
	    إذا (c == NUL || ri_سر(c))
		حالت = RA_NOMATCH;
	    وإلا
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة ALPHA:
	    إذا (!ri_alpha(c))
		حالت = RA_NOMATCH;
	    وإلا
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة NALPHA:
	    إذا (c == NUL || ri_alpha(c))
		حالت = RA_NOMATCH;
	    وإلا
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة هيٺاهون_2:
	    إذا (!ri_هيٺاهون(c))
		حالت = RA_NOMATCH;
	    وإلا
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة NLOWER:
	    إذا (c == NUL || ri_هيٺاهون(c))
		حالت = RA_NOMATCH;
	    وإلا
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة مٿانهون:
	    إذا (!ri_مٿانهون(c))
		حالت = RA_NOMATCH;
	    وإلا
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة NUPPER:
	    إذا (c == NUL || ri_مٿانهون(c))
		حالت = RA_NOMATCH;
	    وإلا
		ADVANCE_REGINPUT();
	    اكسر;

	  حالة EXACTLY:
	    {
		صحيح	len;
		char_u	*opnd;

		opnd = عمليو(جاچ);
		// Inline the first byte, for speed.
		إذا (*opnd != *rex.داخلا_2
			&& (!rex.reg_ic
			    || (!enc_utf8
			      && MB_TOLOWER(*opnd) != MB_TOLOWER(*rex.داخلا_2))))
		    حالت = RA_NOMATCH;
		وإلا إذا (*opnd == NUL)
		{
		    // match empty string always works; happens when "~" is
		    // empty.
		}
		وإلا
		{
		    إذا (opnd[1] == NUL && !(enc_utf8 && rex.reg_ic))
		    {
			len = 1;	// matched a single byte above
		    }
		    وإلا
		    {
			// Need to match first byte again for multi-byte.
			len = (صحيح)STRLEN(opnd);
			إذا (cstrncmp(opnd, rex.داخلا_2, &len) != 0)
			    حالت = RA_NOMATCH;
		    }
		    // Check for following composing character, unless %C
		    // follows (skips over all composing chars).
		    إذا (حالت != RA_NOMATCH
			    && enc_utf8
			    && UTF_COMPOSINGLIKE(rex.داخلا_2, rex.داخلا_2 + len)
			    && !rex.reg_icombine
			    && OP(ايندڙ) != RE_COMPOSING)
		    {
			// raaron: This code makes a composing character get
			// ignored, which is the correct behavior (sometimes)
			// for voweled Hebrew texts.
			حالت = RA_NOMATCH;
		    }
		    إذا (حالت != RA_NOMATCH)
			rex.داخلا_2 += len;
		}
	    }
	    اكسر;

	  حالة ANYOF:
	  حالة ANYBUT:
	    {
		char_u  *q = عمليو(جاچ);

		إذا (c == NUL)
		    حالت = RA_NOMATCH;
		وإلا إذا ((cstrchr(q, c) == عدم) == (op == ANYOF))
		    حالت = RA_NOMATCH;
		وإلا
		{
		    // Check following combining characters
		    صحيح	len = 0;
		    صحيح i;

		    إذا (enc_utf8)
			len = utfc_ptr2len(q) - utf_ptr2len(q);

		    MB_CPTR_ADV(rex.داخلا_2);
		    MB_CPTR_ADV(q);

		    إذا (!enc_utf8 || len == 0)
			اكسر;

		    لكل (i = 0; i < len; ++i)
			إذا (q[i] != rex.داخلا_2[i])
			{
			    حالت = RA_NOMATCH;
			    اكسر;
			}
		    rex.داخلا_2 += len;
		}
		اكسر;
	    }

	  حالة MULTIBYTECODE:
	    إذا (has_mbyte)
	    {
		صحيح	i, len;
		char_u	*opnd;
		صحيح	opndc = 0, inpc;

		opnd = عمليو(جاچ);
		// Safety check (just in case 'encoding' was changed since
		// compiling the program).
		إذا ((len = (*mb_ptr2len)(opnd)) < 2)
		{
		    حالت = RA_NOMATCH;
		    اكسر;
		}
		إذا (enc_utf8)
		    opndc = utf_ptr2char(opnd);
		إذا (enc_utf8 && utf_iscomposing(opndc))
		{
		    // When only a composing char is given match at any
		    // position where that composing char appears.
		    حالت = RA_NOMATCH;
		    لكل (i = 0; rex.داخلا_2[i] != NUL;
						i += utf_ptr2len(rex.داخلا_2 + i))
		    {
			inpc = utf_ptr2char(rex.داخلا_2 + i);
			إذا (!utf_iscomposing(inpc))
			{
			    إذا (i > 0)
				اكسر;
			}
			وإلا إذا (opndc == inpc)
			{
			    // Include all following composing chars.
			    len = i + utfc_ptr2len(rex.داخلا_2 + i);
			    حالت = RA_ميلاپ;
			    اكسر;
			}
		    }
		}
		وإلا إذا (enc_utf8)
		{
		    إذا (cstrncmp(opnd, rex.داخلا_2, &len) != 0)
		    {
			حالت = RA_NOMATCH;
			اكسر;
		    }
		}
		وإلا
		    لكل (i = 0; i < len; ++i)
			إذا (opnd[i] != rex.داخلا_2[i])
			{
			    حالت = RA_NOMATCH;
			    اكسر;
			}
		rex.داخلا_2 += len;
	    }
	    وإلا
		حالت = RA_NOMATCH;
	    اكسر;
	  حالة RE_COMPOSING:
	    إذا (enc_utf8)
	    {
		// Skip composing characters.
		طالما (utf_iscomposing(utf_ptr2char(rex.داخلا_2)))
		    MB_CPTR_ADV(rex.داخلا_2);
	    }
	    اكسر;

	  حالة NOTHING:
	    اكسر;

	  حالة پٺ_2:
	    {
		صحيح		i;
		backpos_T	*bp;

		// When we run into BACK we need to check if we don't keep
		// looping without matching any input.  The second and later
		// times a BACK is encountered it fails if the input is still
		// at the same position as the previous time.
		// The positions are stored in "backpos" and found by the
		// current value of "scan", the position in the RE program.
		bp = (backpos_T *)backpos.ga_معلومات;
		لكل (i = 0; i < backpos.ga_len; ++i)
		    إذا (bp[i].bp_جاچ == جاچ)
			اكسر;
		إذا (i == backpos.ga_len)
		{
		    // First time at this BACK, make room to store the pos.
		    إذا (GA_GROW_FAILS(&backpos, 1))
			حالت = RA_FAIL;
		    وإلا
		    {
			// get "ga_data" again, it may have changed
			bp = (backpos_T *)backpos.ga_معلومات;
			bp[i].bp_جاچ = جاچ;
			++backpos.ga_len;
		    }
		}
		وإلا إذا (reg_save_equal(&bp[i].bp_pos))
		    // Still at same position as last time, fail.
		    حالت = RA_NOMATCH;

		إذا (حالت != RA_FAIL && حالت != RA_NOMATCH)
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
		rp = regstack_ڌڪو(RS_MOPEN, جاچ);
		إذا (rp == عدم)
		    حالت = RA_FAIL;
		وإلا
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
		إذا (regstack_ڌڪو(RS_NOPEN, جاچ) == عدم)
		    حالت = RA_FAIL;
		// We simply continue and handle the result when done.
		اكسر;

#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
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
		rp = regstack_ڌڪو(RS_ZOPEN, جاچ);
		إذا (rp == عدم)
		    حالت = RA_FAIL;
		وإلا
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_startzpos[no],
							     &reg_startzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    اكسر;
#جيڪڏهن_پورو

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
		rp = regstack_ڌڪو(RS_MCLOSE, جاچ);
		إذا (rp == عدم)
		    حالت = RA_FAIL;
		وإلا
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_endpos[no],
							    &rex.reg_endp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    اكسر;

#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
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
		rp = regstack_ڌڪو(RS_ZCLOSE, جاچ);
		إذا (rp == عدم)
		    حالت = RA_FAIL;
		وإلا
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_endzpos[no],
							      &reg_endzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    اكسر;
#جيڪڏهن_پورو

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
		إذا (!REG_MULTI)		// Single-line regexp
		{
		    إذا (rex.reg_startp[no] == عدم || rex.reg_endp[no] == عدم)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    وإلا
		    {
			// Compare current input with back-ref in the same
			// line.
			len = (صحيح)(rex.reg_endp[no] - rex.reg_startp[no]);
			إذا (cstrncmp(rex.reg_startp[no], rex.داخلا_2, &len) != 0)
			    حالت = RA_NOMATCH;
		    }
		}
		وإلا				// Multi-line regexp
		{
		    إذا (rex.reg_startpos[no].lnum < 0
						|| rex.reg_endpos[no].lnum < 0)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    وإلا
		    {
			إذا (rex.reg_startpos[no].lnum == rex.lnum
				&& rex.reg_endpos[no].lnum == rex.lnum)
			{
			    // Compare back-ref within the current line.
			    len = rex.reg_endpos[no].کانو
						    - rex.reg_startpos[no].کانو;
			    إذا (cstrncmp(rex.سٽ + rex.reg_startpos[no].کانو,
							  rex.داخلا_2, &len) != 0)
				حالت = RA_NOMATCH;
			}
			وإلا
			{
			    // Messy situation: Need to compare between two
			    // lines.
			    صحيح r = ميلاپ_with_backref(
					    rex.reg_startpos[no].lnum,
					    rex.reg_startpos[no].کانو,
					    rex.reg_endpos[no].lnum,
					    rex.reg_endpos[no].کانو,
					    &len);

			    إذا (r != RA_ميلاپ)
				حالت = r;
			}
		    }
		}

		// Matched the backref, skip over it.
		rex.داخلا_2 += len;
	    }
	    اكسر;

#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
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
		إذا (re_extmatch_اندريون != عدم
			&& re_extmatch_اندريون->matches[no] != عدم)
		{
		    len = (صحيح)STRLEN(re_extmatch_اندريون->matches[no]);
		    إذا (cstrncmp(re_extmatch_اندريون->matches[no],
							  rex.داخلا_2, &len) != 0)
			حالت = RA_NOMATCH;
		    وإلا
			rex.داخلا_2 += len;
		}
		وإلا
		{
		    // Backref was not set: Match an empty string.
		}
	    }
	    اكسر;
#جيڪڏهن_پورو

	  حالة BRANCH:
	    {
		إذا (OP(ايندڙ) != BRANCH) // No choice.
		    ايندڙ = عمليو(جاچ);	// Avoid recursion.
		وإلا
		{
		    rp = regstack_ڌڪو(RS_BRANCH, جاچ);
		    إذا (rp == عدم)
			حالت = RA_FAIL;
		    وإلا
			حالت = RA_BREAK;	// rest is below
		}
	    }
	    اكسر;

	  حالة BRACE_LIMITS:
	    {
		إذا (OP(ايندڙ) == BRACE_SIMPLE)
		{
		    bl_minval = عمليو_MIN(جاچ);
		    bl_maxval = عمليو_MAX(جاچ);
		}
		وإلا إذا (OP(ايندڙ) >= BRACE_COMPLEX
			&& OP(ايندڙ) < BRACE_COMPLEX + 10)
		{
		    no = OP(ايندڙ) - BRACE_COMPLEX;
		    brace_min[no] = عمليو_MIN(جاچ);
		    brace_max[no] = عمليو_MAX(جاچ);
		    brace_ڳڻپ[no] = 0;
		}
		وإلا
		{
		    اندروني_error("BRACE_LIMITS");
		    حالت = RA_FAIL;
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
		++brace_ڳڻپ[no];

		// If not matched enough times yet, try one more
		إذا (brace_ڳڻپ[no] <= (brace_min[no] <= brace_max[no]
					     ? brace_min[no] : brace_max[no]))
		{
		    rp = regstack_ڌڪو(RS_BRCPLX_وڌيڪ, جاچ);
		    إذا (rp == عدم)
			حالت = RA_FAIL;
		    وإلا
		    {
			rp->rs_no = no;
			reg_save(&rp->rs_un.regsave, &backpos);
			ايندڙ = عمليو(جاچ);
			// We continue and handle the result when done.
		    }
		    اكسر;
		}

		// If matched enough times, may try matching some more
		إذا (brace_min[no] <= brace_max[no])
		{
		    // Range is the normal way around, use longest match
		    إذا (brace_ڳڻپ[no] <= brace_max[no])
		    {
			rp = regstack_ڌڪو(RS_BRCPLX_LONG, جاچ);
			إذا (rp == عدم)
			    حالت = RA_FAIL;
			وإلا
			{
			    rp->rs_no = no;
			    reg_save(&rp->rs_un.regsave, &backpos);
			    ايندڙ = عمليو(جاچ);
			    // We continue and handle the result when done.
			}
		    }
		}
		وإلا
		{
		    // Range is backwards, use shortest match first
		    إذا (brace_ڳڻپ[no] <= brace_min[no])
		    {
			rp = regstack_ڌڪو(RS_BRCPLX_SHORT, جاچ);
			إذا (rp == عدم)
			    حالت = RA_FAIL;
			وإلا
			{
			    reg_save(&rp->rs_un.regsave, &backpos);
			    // We continue and handle the result when done.
			}
		    }
		}
	    }
	    اكسر;

	  حالة BRACE_SIMPLE:
	  حالة تارو:
	  حالة PLUS:
	    {
		regstar_T	rst;

		// Lookahead to avoid useless match attempts when we know
		// what character comes next.
		إذا (OP(ايندڙ) == EXACTLY)
		{
		    rst.nextb = *عمليو(ايندڙ);
		    إذا (rex.reg_ic)
		    {
			إذا (MB_ISUPPER(rst.nextb))
			    rst.nextb_ic = MB_TOLOWER(rst.nextb);
			وإلا
			    rst.nextb_ic = MB_TOUPPER(rst.nextb);
		    }
		    وإلا
			rst.nextb_ic = rst.nextb;
		}
		وإلا
		{
		    rst.nextb = NUL;
		    rst.nextb_ic = NUL;
		}
		إذا (op != BRACE_SIMPLE)
		{
		    rst.minval = (op == تارو) ? 0 : 1;
		    rst.maxval = MAX_حد_بندي;
		}
		وإلا
		{
		    rst.minval = bl_minval;
		    rst.maxval = bl_maxval;
		}

		// When maxval > minval, try matching as much as possible, up
		// to maxval.  When maxval < minval, try matching at least the
		// minimal number (since the range is backwards, that's also
		// maxval!).
		rst.ڳڻپ = regrepeat(عمليو(جاچ), rst.maxval);
		إذا (got_int)
		{
		    حالت = RA_FAIL;
		    اكسر;
		}
		إذا (rst.minval <= rst.maxval
			  ? rst.ڳڻپ >= rst.minval : rst.ڳڻپ >= rst.maxval)
		{
		    // It could match.  Prepare for trying to match what
		    // follows.  The code is below.  Parameters are stored in
		    // a regstar_T on the regstack.
		    إذا ((ڊگهو)((بنانشان)regstack.ga_len >> 10) >= p_mmp)
		    {
			emsg(_(e_نمونو_استعمال_ڪري_وڌيڪ_يادگيري_than_maxmempattern));
			حالت = RA_FAIL;
		    }
		    وإلا إذا (GA_GROW_FAILS(&regstack, (صحيح)حجم(regstar_T)))
			حالت = RA_FAIL;
		    وإلا
		    {
			regstack.ga_len += حجم(regstar_T);
			rp = regstack_ڌڪو(rst.minval <= rst.maxval
					? RS_تارو_LONG : RS_تارو_SHORT, جاچ);
			إذا (rp == عدم)
			    حالت = RA_FAIL;
			وإلا
			{
			    *(((regstar_T *)rp) - 1) = rst;
			    حالت = RA_BREAK;	    // skip the restore bits
			}
		    }
		}
		وإلا
		    حالت = RA_NOMATCH;

	    }
	    اكسر;

	  حالة NOMATCH:
	  حالة ميلاپ_2:
	  حالة SUBPAT:
	    rp = regstack_ڌڪو(RS_NOMATCH, جاچ);
	    إذا (rp == عدم)
		حالت = RA_FAIL;
	    وإلا
	    {
		rp->rs_no = op;
		reg_save(&rp->rs_un.regsave, &backpos);
		ايندڙ = عمليو(جاچ);
		// We continue and handle the result when done.
	    }
	    اكسر;

	  حالة BEHIND:
	  حالة NOBEHIND:
	    // Need a bit of room to store extra positions.
	    إذا ((ڊگهو)((بنانشان)regstack.ga_len >> 10) >= p_mmp)
	    {
		emsg(_(e_نمونو_استعمال_ڪري_وڌيڪ_يادگيري_than_maxmempattern));
		حالت = RA_FAIL;
	    }
	    وإلا إذا (GA_GROW_FAILS(&regstack, (صحيح)حجم(regbehind_T)))
		حالت = RA_FAIL;
	    وإلا
	    {
		regstack.ga_len += حجم(regbehind_T);
		rp = regstack_ڌڪو(RS_BEHIND1, جاچ);
		إذا (rp == عدم)
		    حالت = RA_FAIL;
		وإلا
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
	    إذا (REG_MULTI)
	    {
		إذا (behind_pos.rs_u.pos.کانو != (colnr_T)(rex.داخلا_2 - rex.سٽ)
			|| behind_pos.rs_u.pos.lnum != rex.lnum)
		    حالت = RA_NOMATCH;
	    }
	    وإلا إذا (behind_pos.rs_u.ptr != rex.داخلا_2)
		حالت = RA_NOMATCH;
	    اكسر;

	  حالة NEWL:
	    إذا ((c != NUL || !REG_MULTI || rex.lnum > rex.reg_maxline
			     || rex.reg_سٽ_lbr)
					   && (c != '\n' || !rex.reg_سٽ_lbr))
		حالت = RA_NOMATCH;
	    وإلا إذا (rex.reg_سٽ_lbr)
		ADVANCE_REGINPUT();
	    وإلا
		reg_nextline();
	    اكسر;

	  حالة پڇاڙي_3:
	    حالت = RA_ميلاپ;	// Success!
	    اكسر;

	  افتراضي:
	    iemsg(e_corrupted_regexp_پروگرام);
#جيڪڏهن_وصف_ٿيل DEBUG
	    لکو("Illegal op code %d\n", op);
#جيڪڏهن_پورو
	    حالت = RA_FAIL;
	    اكسر;
	  }
	}

	// If we can't continue sequentially, break the inner loop.
	إذا (حالت != RA_CONT)
	    اكسر;

	// Continue in inner loop, advance to next item.
	جاچ = ايندڙ;

    } // end of inner loop

    // If there is something on the regstack execute the code for the state.
    // If the state is popped then loop and use the older state.
    طالما (regstack.ga_len > 0 && حالت != RA_FAIL)
    {
	rp = (regitem_T *)((حرف *)regstack.ga_معلومات + regstack.ga_len) - 1;
	بدل (rp->rs_state)
	{
	  حالة RS_NOPEN:
	    // Result is passed on as-is, simply pop the state.
	    regstack_ڪڍو(&جاچ);
	    اكسر;

	  حالة RS_MOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    إذا (حالت == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_startpos[rp->rs_no],
						  &rex.reg_startp[rp->rs_no]);
	    regstack_ڪڍو(&جاچ);
	    اكسر;

#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
	  حالة RS_ZOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    إذا (حالت == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_startzpos[rp->rs_no],
						 &reg_startzp[rp->rs_no]);
	    regstack_ڪڍو(&جاچ);
	    اكسر;
#جيڪڏهن_پورو

	  حالة RS_MCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    إذا (حالت == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_endpos[rp->rs_no],
						    &rex.reg_endp[rp->rs_no]);
	    regstack_ڪڍو(&جاچ);
	    اكسر;

#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
	  حالة RS_ZCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    إذا (حالت == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_endzpos[rp->rs_no],
						   &reg_endzp[rp->rs_no]);
	    regstack_ڪڍو(&جاچ);
	    اكسر;
#جيڪڏهن_پورو

	  حالة RS_BRANCH:
	    إذا (حالت == RA_ميلاپ)
		// this branch matched, use it
		regstack_ڪڍو(&جاچ);
	    وإلا
	    {
		إذا (حالت != RA_BREAK)
		{
		    // After a non-matching branch: try next one.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    جاچ = rp->rs_جاچ;
		}
		إذا (جاچ == عدم || OP(جاچ) != BRANCH)
		{
		    // no more branches, didn't find a match
		    حالت = RA_NOMATCH;
		    regstack_ڪڍو(&جاچ);
		}
		وإلا
		{
		    // Prepare to try a branch.
		    rp->rs_جاچ = regnext(جاچ);
		    reg_save(&rp->rs_un.regsave, &backpos);
		    جاچ = عمليو(جاچ);
		}
	    }
	    اكسر;

	  حالة RS_BRCPLX_وڌيڪ:
	    // Pop the state.  Restore pointers when there is no match.
	    إذا (حالت == RA_NOMATCH)
	    {
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_ڳڻپ[rp->rs_no];	// decrement match count
	    }
	    regstack_ڪڍو(&جاچ);
	    اكسر;

	  حالة RS_BRCPLX_LONG:
	    // Pop the state.  Restore pointers when there is no match.
	    إذا (حالت == RA_NOMATCH)
	    {
		// There was no match, but we did find enough matches.
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_ڳڻپ[rp->rs_no];
		// continue with the items after "\{}"
		حالت = RA_CONT;
	    }
	    regstack_ڪڍو(&جاچ);
	    إذا (حالت == RA_CONT)
		جاچ = regnext(جاچ);
	    اكسر;

	  حالة RS_BRCPLX_SHORT:
	    // Pop the state.  Restore pointers when there is no match.
	    إذا (حالت == RA_NOMATCH)
		// There was no match, try to match one more item.
		reg_restore(&rp->rs_un.regsave, &backpos);
	    regstack_ڪڍو(&جاچ);
	    إذا (حالت == RA_NOMATCH)
	    {
		جاچ = عمليو(جاچ);
		حالت = RA_CONT;
	    }
	    اكسر;

	  حالة RS_NOMATCH:
	    // Pop the state.  If the operand matches for NOMATCH or
	    // doesn't match for MATCH/SUBPAT, we fail.  Otherwise backup,
	    // except for SUBPAT, and continue with the next item.
	    إذا (حالت == (rp->rs_no == NOMATCH ? RA_ميلاپ : RA_NOMATCH))
		حالت = RA_NOMATCH;
	    وإلا
	    {
		حالت = RA_CONT;
		إذا (rp->rs_no != SUBPAT)	// zero-width
		    reg_restore(&rp->rs_un.regsave, &backpos);
	    }
	    regstack_ڪڍو(&جاچ);
	    إذا (حالت == RA_CONT)
		جاچ = regnext(جاچ);
	    اكسر;

	  حالة RS_BEHIND1:
	    إذا (حالت == RA_NOMATCH)
	    {
		regstack_ڪڍو(&جاچ);
		regstack.ga_len -= حجم(regbehind_T);
	    }
	    وإلا
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
		جاچ = عمليو(rp->rs_جاچ) + 4;
	    }
	    اكسر;

	  حالة RS_BEHIND2:
	    // Looping for BEHIND / NOBEHIND match.
	    إذا (حالت == RA_ميلاپ && reg_save_equal(&behind_pos))
	    {
		// found a match that ends where "next" started
		behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		إذا (rp->rs_no == BEHIND)
		    reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
		وإلا
		{
		    // But we didn't want a match.  Need to restore the
		    // subexpr, because what follows matched, so they have
		    // been set.
		    حالت = RA_NOMATCH;
		    restore_subexpr(((regbehind_T *)rp) - 1);
		}
		regstack_ڪڍو(&جاچ);
		regstack.ga_len -= حجم(regbehind_T);
	    }
	    وإلا
	    {
		ڊگهو حد_بندي;

		// No match or a match that doesn't end where we want it: Go
		// back one character.  May go to previous line once.
		no = OK;
		حد_بندي = عمليو_MIN(rp->rs_جاچ);
		إذا (REG_MULTI)
		{
		    إذا (حد_بندي > 0
			    && ((rp->rs_un.regsave.rs_u.pos.lnum
						    < behind_pos.rs_u.pos.lnum
				    ? (colnr_T)STRLEN(rex.سٽ)
				    : behind_pos.rs_u.pos.کانو)
				- rp->rs_un.regsave.rs_u.pos.کانو >= حد_بندي))
			no = FAIL;
		    وإلا إذا (rp->rs_un.regsave.rs_u.pos.کانو == 0)
		    {
			إذا (rp->rs_un.regsave.rs_u.pos.lnum
					< behind_pos.rs_u.pos.lnum
				|| reg_getline(
					--rp->rs_un.regsave.rs_u.pos.lnum)
								  == عدم)
			    no = FAIL;
			وإلا
			{
			    reg_restore(&rp->rs_un.regsave, &backpos);
			    rp->rs_un.regsave.rs_u.pos.کانو =
						 (colnr_T)STRLEN(rex.سٽ);
			}
		    }
		    وإلا
		    {
			إذا (has_mbyte)
			{
			    char_u *سٽ =
				  reg_getline(rp->rs_un.regsave.rs_u.pos.lnum);

			    rp->rs_un.regsave.rs_u.pos.کانو -=
				(*mb_سر_off)(سٽ, سٽ
				    + rp->rs_un.regsave.rs_u.pos.کانو - 1) + 1;
			}
			وإلا
			    --rp->rs_un.regsave.rs_u.pos.کانو;
		    }
		}
		وإلا
		{
		    إذا (rp->rs_un.regsave.rs_u.ptr == rex.سٽ)
			no = FAIL;
		    وإلا
		    {
			MB_PTR_پٺ(rex.سٽ, rp->rs_un.regsave.rs_u.ptr);
			إذا (حد_بندي > 0 && (ڊگهو)(behind_pos.rs_u.ptr
				     - rp->rs_un.regsave.rs_u.ptr) > حد_بندي)
			    no = FAIL;
		    }
		}
		إذا (no == OK)
		{
		    // Advanced, prepare for finding match again.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    جاچ = عمليو(rp->rs_جاچ) + 4;
		    إذا (حالت == RA_ميلاپ)
		    {
			// We did match, so subexpr may have been changed,
			// need to restore them for the next try.
			حالت = RA_NOMATCH;
			restore_subexpr(((regbehind_T *)rp) - 1);
		    }
		}
		وإلا
		{
		    // Can't advance.  For NOBEHIND that's a match.
		    behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		    إذا (rp->rs_no == NOBEHIND)
		    {
			reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
			حالت = RA_ميلاپ;
		    }
		    وإلا
		    {
			// We do want a proper match.  Need to restore the
			// subexpr if we had a match, because they may have
			// been set.
			إذا (حالت == RA_ميلاپ)
			{
			    حالت = RA_NOMATCH;
			    restore_subexpr(((regbehind_T *)rp) - 1);
			}
		    }
		    regstack_ڪڍو(&جاچ);
		    regstack.ga_len -= حجم(regbehind_T);
		}
	    }
	    اكسر;

	  حالة RS_تارو_LONG:
	  حالة RS_تارو_SHORT:
	    {
		regstar_T	    *rst = ((regstar_T *)rp) - 1;

		إذا (حالت == RA_ميلاپ)
		{
		    regstack_ڪڍو(&جاچ);
		    regstack.ga_len -= حجم(regstar_T);
		    اكسر;
		}

		// Tried once already, restore input pointers.
		إذا (حالت != RA_BREAK)
		    reg_restore(&rp->rs_un.regsave, &backpos);

		// Repeat until we found a position where it could match.
		لكل (;;)
		{
		    إذا (حالت != RA_BREAK)
		    {
			// Tried first position already, advance.
			إذا (rp->rs_state == RS_تارو_LONG)
			{
			    // Trying for longest match, but couldn't or
			    // didn't match -- back up one char.
			    إذا (--rst->ڳڻپ < rst->minval)
				اكسر;
			    إذا (rex.داخلا_2 == rex.سٽ)
			    {
				// backup to last char of previous line
				إذا (rex.lnum == 0)
				{
				    حالت = RA_NOMATCH;
				    اكسر;
				}
				--rex.lnum;
				rex.سٽ = reg_getline(rex.lnum);
				// Just in case regrepeat() didn't count
				// right.
				إذا (rex.سٽ == عدم)
				    اكسر;
				rex.داخلا_2 = rex.سٽ + reg_getline_len(rex.lnum);
				fast_breakcheck();
			    }
			    وإلا
				MB_PTR_پٺ(rex.سٽ, rex.داخلا_2);
			}
			وإلا
			{
			    // Range is backwards, use shortest match first.
			    // Careful: maxval and minval are exchanged!
			    // Couldn't or didn't match: try advancing one
			    // char.
			    إذا (rst->ڳڻپ == rst->minval
				  || regrepeat(عمليو(rp->rs_جاچ), 1L) == 0)
				اكسر;
			    ++rst->ڳڻپ;
			}
			إذا (got_int)
			    اكسر;
		    }
		    وإلا
			حالت = RA_NOMATCH;

		    // If it could match, try it.
		    إذا (rst->nextb == NUL || *rex.داخلا_2 == rst->nextb
					     || *rex.داخلا_2 == rst->nextb_ic)
		    {
			reg_save(&rp->rs_un.regsave, &backpos);
			جاچ = regnext(rp->rs_جاچ);
			حالت = RA_CONT;
			اكسر;
		    }
		}
		إذا (حالت != RA_CONT)
		{
		    // Failed.
		    regstack_ڪڍو(&جاچ);
		    regstack.ga_len -= حجم(regstar_T);
		    حالت = RA_NOMATCH;
		}
	    }
	    اكسر;
	}

	// If we want to continue the inner loop or didn't pop a state
	// continue matching loop
	إذا (حالت == RA_CONT || rp == (regitem_T *)
			     ((حرف *)regstack.ga_معلومات + regstack.ga_len) - 1)
	    اكسر;

#جيڪڏهن_وصف_ٿيل FEAT_RELTIME
	إذا (bt_did_وقت_ٻاهريون(timed_ٻاهريون))
	{
	    حالت = RA_FAIL;
	    اكسر;
	}
#جيڪڏهن_پورو
    }

    // May need to continue with the inner loop, starting at "scan".
    إذا (حالت == RA_CONT)
	استمر;

    // If the regstack is empty or something failed we are done.
    إذا (regstack.ga_len == 0 || حالت == RA_FAIL)
    {
	إذا (جاچ == عدم)
	{
	    // We get here only if there's trouble -- normally "case END" is
	    // the terminating point.
	    iemsg(e_corrupted_regexp_پروگرام);
#جيڪڏهن_وصف_ٿيل DEBUG
	    لکو("Premature EOL\n");
#جيڪڏهن_پورو
	}
	أرجع (حالت == RA_ميلاپ);
    }

  } // End of loop until the regstack is empty.

  // NOTREACHED
}

/*
 * regtry - try match of "prog" with at rex.line["col"].
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    ساكن ڊگهو
regtry(
    bt_regprog_T	*prog,
    colnr_T		کانو,
    صحيح			*timed_ٻاهريون)	// flag set on timeout or NULL
{
    rex.داخلا_2 = rex.سٽ + کانو;
    rex.need_clear_subexpr = TRUE;
#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    rex.need_clear_zsubexpr = (prog->reghasz == REX_سيٽ);
#جيڪڏهن_پورو

    إذا (regmatch(prog->پروگرام + 1, timed_ٻاهريون) == 0)
	أرجع 0;

    cleanup_subexpr();
    إذا (REG_MULTI)
    {
	إذا (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].کانو = کانو;
	}
	إذا (rex.reg_endpos[0].lnum < 0)
	{
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].کانو = (صحيح)(rex.داخلا_2 - rex.سٽ);
	}
	وإلا
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    وإلا
    {
	إذا (rex.reg_startp[0] == عدم)
	    rex.reg_startp[0] = rex.سٽ + کانو;
	إذا (rex.reg_endp[0] == عدم)
	    rex.reg_endp[0] = rex.داخلا_2;
    }
#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_ٻاهريون);
    re_extmatch_ٻاهريون = عدم;

    إذا (prog->reghasz == REX_سيٽ)
    {
	صحيح		i;

	cleanup_zsubexpr();
	re_extmatch_ٻاهريون = make_extmatch();
	إذا (re_extmatch_ٻاهريون == عدم)
	    أرجع 0;
	لكل (i = 0; i < NSUBEXP; i++)
	{
	    إذا (REG_MULTI)
	    {
		// Only accept single line matches.
		إذا (reg_startzpos[i].lnum >= 0
			&& reg_endzpos[i].lnum == reg_startzpos[i].lnum
			&& reg_endzpos[i].کانو >= reg_startzpos[i].کانو)
		    re_extmatch_ٻاهريون->matches[i] =
			vim_strnsave(reg_getline(reg_startzpos[i].lnum)
						       + reg_startzpos[i].کانو,
				   reg_endzpos[i].کانو - reg_startzpos[i].کانو);
	    }
	    وإلا
	    {
		إذا (reg_startzp[i] != عدم && reg_endzp[i] != عدم)
		    re_extmatch_ٻاهريون->matches[i] =
			    vim_strnsave(reg_startzp[i],
						reg_endzp[i] - reg_startzp[i]);
	    }
	}
    }
#جيڪڏهن_پورو
    أرجع 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    ساكن ڊگهو
bt_regexec_both(
    char_u	*سٽ,
    colnr_T	startcol,	// column to start looking for match
    صحيح		*timed_ٻاهريون)	// flag set on timeout or NULL
{
    bt_regprog_T    *prog;
    char_u	    *s;
    colnr_T	    کانو = startcol;
    ڊگهو	    retval = 0L;

    // Create "regstack" and "backpos" if they are not allocated yet.
    // We allocate *_INITIAL amount of bytes first and then set the grow size
    // to much bigger value to avoid many malloc calls in case of deep regular
    // expressions.
    إذا (regstack.ga_معلومات == عدم)
    {
	// Use an item size of 1 byte, since we push different things
	// onto the regstack.
	ga_init2(&regstack, 1, REGSTACK_INITIAL);
	(فراغ)ga_grow(&regstack, REGSTACK_INITIAL);
	regstack.ga_growsize = REGSTACK_INITIAL * 8;
    }

    إذا (backpos.ga_معلومات == عدم)
    {
	ga_init2(&backpos, حجم(backpos_T), BACKPOS_INITIAL);
	(فراغ)ga_grow(&backpos, BACKPOS_INITIAL);
	backpos.ga_growsize = BACKPOS_INITIAL * 8;
    }

    إذا (REG_MULTI)
    {
	prog = (bt_regprog_T *)rex.reg_mmatch->regprog;
	سٽ = reg_getline((linenr_T)0);
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    وإلا
    {
	prog = (bt_regprog_T *)rex.reg_ميلاپ->regprog;
	rex.reg_startp = rex.reg_ميلاپ->startp;
	rex.reg_endp = rex.reg_ميلاپ->endp;
    }

    // Be paranoid...
    إذا (prog == عدم || سٽ == عدم)
    {
	iemsg(e_null_دليل);
	اذهب theend;
    }

    // Check validity of program.
    إذا (prog_magic_wrong())
	اذهب theend;

    // If the start column is past the maximum column: no need to try.
    إذا (rex.reg_maxcol > 0 && کانو >= rex.reg_maxcol)
	اذهب theend;

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    إذا (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    وإلا إذا (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    إذا (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    // If there is a "must appear" string, look for it.
    إذا (prog->regmust != عدم)
    {
	صحيح c;

	إذا (has_mbyte)
	    c = (*mb_ptr2char)(prog->regmust);
	وإلا
	    c = *prog->regmust;
	s = سٽ + کانو;

	// This is used very often, esp. for ":global".  Use three versions of
	// the loop to avoid overhead of conditions.
	إذا (!rex.reg_ic && !has_mbyte)
	    طالما ((s = vim_strbyte(s, c)) != عدم)
	    {
		صحيح len = prog->regmlen;
		إذا (cstrncmp(s, prog->regmust, &len) == 0)
		    اكسر;		// Found it.
		++s;
	    }
	وإلا إذا (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	    طالما ((s = vim_strchr(s, c)) != عدم)
	    {
		صحيح len = prog->regmlen;
		إذا (cstrncmp(s, prog->regmust, &len) == 0)
		    اكسر;		// Found it.
		MB_PTR_ADV(s);
	    }
	وإلا
	    طالما ((s = cstrchr(s, c)) != عدم)
	    {
		صحيح len = prog->regmlen;
		إذا (cstrncmp(s, prog->regmust, &len) == 0)
		    اكسر;		// Found it.
		MB_PTR_ADV(s);
	    }
	إذا (s == عدم)		// Not present.
	    اذهب theend;
    }

    rex.سٽ = سٽ;
    rex.lnum = 0;
    reg_toolong = FALSE;

    // Simplest case: Anchored match need be tried only once.
    إذا (prog->reganch)
    {
	صحيح	c;

	إذا (has_mbyte)
	    c = (*mb_ptr2char)(rex.سٽ + کانو);
	وإلا
	    c = rex.سٽ[کانو];
	إذا (prog->regstart == NUL
		|| prog->regstart == c
		|| (rex.reg_ic
		    && (((enc_utf8 && utf_fold(prog->regstart) == utf_fold(c)))
			|| (c < 255 && prog->regstart < 255 &&
			    MB_TOLOWER(prog->regstart) == MB_TOLOWER(c)))))
	    retval = regtry(prog, کانو, timed_ٻاهريون);
	وإلا
	    retval = 0;
    }
    وإلا
    {
	// Messy cases:  unanchored match.
	طالما (!got_int)
	{
	    إذا (prog->regstart != NUL)
	    {
		// Skip until the char we know it must start with.
		// Used often, do some work to avoid call overhead.
		إذا (!rex.reg_ic && !has_mbyte)
		    s = vim_strbyte(rex.سٽ + کانو, prog->regstart);
		وإلا
		    s = cstrchr(rex.سٽ + کانو, prog->regstart);
		إذا (s == عدم)
		{
		    retval = 0;
		    اكسر;
		}
		کانو = (صحيح)(s - rex.سٽ);
	    }

	    // Check for maximum column to try.
	    إذا (rex.reg_maxcol > 0 && کانو >= rex.reg_maxcol)
	    {
		retval = 0;
		اكسر;
	    }

	    retval = regtry(prog, کانو, timed_ٻاهريون);
	    إذا (retval > 0)
		اكسر;

	    // if not currently on the first line, get it again
	    إذا (rex.lnum != 0)
	    {
		rex.lnum = 0;
		rex.سٽ = reg_getline((linenr_T)0);
	    }
	    إذا (rex.سٽ[کانو] == NUL)
		اكسر;
	    إذا (has_mbyte)
		کانو += (*mb_ptr2len)(rex.سٽ + کانو);
	    وإلا
		++کانو;
#جيڪڏهن_وصف_ٿيل FEAT_RELTIME
	    إذا (bt_did_وقت_ٻاهريون(timed_ٻاهريون))
		اكسر;
#جيڪڏهن_پورو
	}
    }

theend:
    // Free "reg_tofree" when it's a bit big.
    // Free regstack and backpos if they are bigger than their initial size.
    إذا (reg_tofreelen > 400)
	VIM_CLEAR(reg_tofree);
    إذا (regstack.ga_maxlen > REGSTACK_INITIAL)
	ga_clear(&regstack);
    إذا (backpos.ga_maxlen > BACKPOS_INITIAL)
	ga_clear(&backpos);

    إذا (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs
	// and \ze are used.
	إذا (REG_MULTI)
	{
	    lpos_T *شروعات = &rex.reg_mmatch->startpos[0];
	    lpos_T *پڇاڙي = &rex.reg_mmatch->endpos[0];

	    إذا (پڇاڙي->lnum < شروعات->lnum
			|| (پڇاڙي->lnum == شروعات->lnum && پڇاڙي->کانو < شروعات->کانو))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_mmatch->rmm_matchcol = کانو;
	}
	وإلا
	{
	    إذا (rex.reg_ميلاپ->endp[0] < rex.reg_ميلاپ->startp[0])
		rex.reg_ميلاپ->endp[0] = rex.reg_ميلاپ->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_ميلاپ->rm_matchcol = کانو;
	}
    }

    أرجع retval;
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
    char_u	*سٽ,	// string to match against
    colnr_T	کانو,	// column to start looking for match
    صحيح		سٽ_lbr)
{
    rex.reg_ميلاپ = rmp;
    rex.reg_mmatch = عدم;
    rex.reg_maxline = 0;
    rex.reg_سٽ_lbr = سٽ_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = عدم;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;

    أرجع bt_regexec_both(سٽ, کانو, عدم);
}

/*
 * Match a regexp against multiple lines.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 *
 * Return zero if there is no match.  Return number of lines contained in the
 * match otherwise.
 */
    ساكن ڊگهو
bt_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	کانو,		// column to start looking for match
    صحيح		*timed_ٻاهريون)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    أرجع bt_regexec_both(عدم, کانو, timed_ٻاهريون);
}

/*
 * Compare a number with the operand of RE_LNUM, RE_COL or RE_VCOL.
 */
    ساكن صحيح
re_انگ_cmp(long_u val, char_u *جاچ)
{
    long_u  n = عمليو_MIN(جاچ);

    إذا (عمليو_CMP(جاچ) == '>')
	أرجع val > n;
    إذا (عمليو_CMP(جاچ) == '<')
	أرجع val < n;
    أرجع val == n;
}

#جيڪڏهن_وصف_ٿيل BT_REGEXP_DUMP

/*
 * regdump - dump a regexp onto stdout in vaguely comprehensible form
 */
    ساكن فراغ
regdump(char_u *نمونو, bt_regprog_T *r)
{
    char_u  *s;
    صحيح	    op = EXACTLY;	// Arbitrary non-END op.
    char_u  *ايندڙ;
    char_u  *پڇاڙي = عدم;
    FILE    *f;

# ifdef BT_REGEXP_LOG
    f = fopen("bt_regexp_log.log", "a");
# وإلا
    f = stdout;
# endif
    إذا (f == عدم)
	أرجع;
    fprintf(f, "-------------------------------------\n\r\nregcomp(%s):\r\n", نمونو);

    s = r->پروگرام + 1;
    // Loop until we find the END that isn't before a referred next (an END
    // can also appear in a NOMATCH operand).
    طالما ((op != پڇاڙي_3 || s <= پڇاڙي) && s < r->پروگرام + r->regsz)
    {
	op = OP(s);
	fprintf(f, "%2d%s", (صحيح)(s - r->پروگرام), regprop(s)); // Where, what.
	ايندڙ = (s + 3 <= r->پروگرام + r->regsz) ? regnext(s) : عدم;
	إذا (ايندڙ == عدم)	// Next ptr.
	    fprintf(f, "(0)");
	وإلا
	    fprintf(f, "(%d)", (صحيح)((s - r->پروگرام) + (ايندڙ - s)));
	إذا (پڇاڙي < ايندڙ)
	    پڇاڙي = ايندڙ;
	إذا (op == BRACE_LIMITS)
	{
	    // Two ints
	    fprintf(f, " minval %ld, maxval %ld", عمليو_MIN(s), عمليو_MAX(s));
	    s += 8;
	}
	وإلا إذا (op == BEHIND || op == NOBEHIND)
	{
	    // one int
	    fprintf(f, " count %ld", عمليو_MIN(s));
	    s += 4;
	}
	وإلا إذا (op == RE_LNUM || op == RE_کانو || op == RE_VCOL)
	{
	    // one int plus comparator
	    fprintf(f, " count %ld", عمليو_MIN(s));
	    s += 5;
	}
	s += 3;
	إذا (op == MULTIBYTECODE)
	{
	    fprintf(f, " mbc=%d", utf_ptr2char(s));
	    s += utfc_ptr2len(s);
	}
	إذا (op == ANYOF || op == ANYOF + جوڙيو_NL
		|| op == ANYBUT || op == ANYBUT + جوڙيو_NL
		|| op == EXACTLY)
	{
	    // Literal string, where present.
	    fprintf(f, "\nxxxxxxxxx\n");
	    طالما (*s != NUL && s < r->پروگرام + r->regsz)
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
    إذا (r->regstart != NUL)
	fprintf(f, "start `%s' 0x%x; ", r->regstart < 256
		? (حرف *)transchar(r->regstart)
		: "multibyte", r->regstart);
    إذا (r->reganch)
	fprintf(f, "anchored; ");
    إذا (r->regmust != عدم)
	fprintf(f, "must have \"%s\"", r->regmust);
    fprintf(f, "\r\n");

# ifdef BT_REGEXP_LOG
    fclose(f);
# endif
}
#جيڪڏهن_پورو	    // BT_REGEXP_DUMP

#جيڪڏهن_وصف_ٿيل DEBUG
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
      حالة RE_کانو:
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
      حالة ANY + جوڙيو_NL:
	p = "ANY+NL";
	اكسر;
      حالة ANYOF:
	p = "ANYOF";
	اكسر;
      حالة ANYOF + جوڙيو_NL:
	p = "ANYOF+NL";
	اكسر;
      حالة ANYBUT:
	p = "ANYBUT";
	اكسر;
      حالة ANYBUT + جوڙيو_NL:
	p = "ANYBUT+NL";
	اكسر;
      حالة IDENT:
	p = "IDENT";
	اكسر;
      حالة IDENT + جوڙيو_NL:
	p = "IDENT+NL";
	اكسر;
      حالة SIDENT:
	p = "SIDENT";
	اكسر;
      حالة SIDENT + جوڙيو_NL:
	p = "SIDENT+NL";
	اكسر;
      حالة KWORD:
	p = "KWORD";
	اكسر;
      حالة KWORD + جوڙيو_NL:
	p = "KWORD+NL";
	اكسر;
      حالة SKWORD:
	p = "SKWORD";
	اكسر;
      حالة SKWORD + جوڙيو_NL:
	p = "SKWORD+NL";
	اكسر;
      حالة FNAME:
	p = "FNAME";
	اكسر;
      حالة FNAME + جوڙيو_NL:
	p = "FNAME+NL";
	اكسر;
      حالة SFNAME:
	p = "SFNAME";
	اكسر;
      حالة SFNAME + جوڙيو_NL:
	p = "SFNAME+NL";
	اكسر;
      حالة PRINT:
	p = "PRINT";
	اكسر;
      حالة PRINT + جوڙيو_NL:
	p = "PRINT+NL";
	اكسر;
      حالة SPRINT:
	p = "SPRINT";
	اكسر;
      حالة SPRINT + جوڙيو_NL:
	p = "SPRINT+NL";
	اكسر;
      حالة WHITE:
	p = "WHITE";
	اكسر;
      حالة WHITE + جوڙيو_NL:
	p = "WHITE+NL";
	اكسر;
      حالة NWHITE:
	p = "NWHITE";
	اكسر;
      حالة NWHITE + جوڙيو_NL:
	p = "NWHITE+NL";
	اكسر;
      حالة DIGIT:
	p = "DIGIT";
	اكسر;
      حالة DIGIT + جوڙيو_NL:
	p = "DIGIT+NL";
	اكسر;
      حالة NDIGIT:
	p = "NDIGIT";
	اكسر;
      حالة NDIGIT + جوڙيو_NL:
	p = "NDIGIT+NL";
	اكسر;
      حالة HEX:
	p = "HEX";
	اكسر;
      حالة HEX + جوڙيو_NL:
	p = "HEX+NL";
	اكسر;
      حالة NHEX:
	p = "NHEX";
	اكسر;
      حالة NHEX + جوڙيو_NL:
	p = "NHEX+NL";
	اكسر;
      حالة OCTAL:
	p = "OCTAL";
	اكسر;
      حالة OCTAL + جوڙيو_NL:
	p = "OCTAL+NL";
	اكسر;
      حالة NOCTAL:
	p = "NOCTAL";
	اكسر;
      حالة NOCTAL + جوڙيو_NL:
	p = "NOCTAL+NL";
	اكسر;
      حالة لفظ_2:
	p = "WORD";
	اكسر;
      حالة لفظ_2 + جوڙيو_NL:
	p = "WORD+NL";
	اكسر;
      حالة NWORD:
	p = "NWORD";
	اكسر;
      حالة NWORD + جوڙيو_NL:
	p = "NWORD+NL";
	اكسر;
      حالة سر_2:
	p = "HEAD";
	اكسر;
      حالة سر_2 + جوڙيو_NL:
	p = "HEAD+NL";
	اكسر;
      حالة NHEAD:
	p = "NHEAD";
	اكسر;
      حالة NHEAD + جوڙيو_NL:
	p = "NHEAD+NL";
	اكسر;
      حالة ALPHA:
	p = "ALPHA";
	اكسر;
      حالة ALPHA + جوڙيو_NL:
	p = "ALPHA+NL";
	اكسر;
      حالة NALPHA:
	p = "NALPHA";
	اكسر;
      حالة NALPHA + جوڙيو_NL:
	p = "NALPHA+NL";
	اكسر;
      حالة هيٺاهون_2:
	p = "LOWER";
	اكسر;
      حالة هيٺاهون_2 + جوڙيو_NL:
	p = "LOWER+NL";
	اكسر;
      حالة NLOWER:
	p = "NLOWER";
	اكسر;
      حالة NLOWER + جوڙيو_NL:
	p = "NLOWER+NL";
	اكسر;
      حالة مٿانهون:
	p = "UPPER";
	اكسر;
      حالة مٿانهون + جوڙيو_NL:
	p = "UPPER+NL";
	اكسر;
      حالة NUPPER:
	p = "NUPPER";
	اكسر;
      حالة NUPPER + جوڙيو_NL:
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
      حالة پٺ_2:
	p = "BACK";
	اكسر;
      حالة پڇاڙي_3:
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
      حالة تارو:
	p = "STAR";
	اكسر;
      حالة PLUS:
	p = "PLUS";
	اكسر;
      حالة NOMATCH:
	p = "NOMATCH";
	اكسر;
      حالة ميلاپ_2:
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
    إذا (p != عدم)
	vim_strncpy((char_u *)buf + buflen, (char_u *)p, حجم(buf) - buflen - 1);
    أرجع (char_u *)buf;
}
#جيڪڏهن_پورو	    // DEBUG
