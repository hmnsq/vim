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
#تعریف_کرو انتہا_3		0	//	End of program or NOMATCH operand.
#تعریف_کرو BOL		1	//	Match "" at beginning of line.
#تعریف_کرو EOL		2	//	Match "" at end of line.
#تعریف_کرو BRANCH		3	// node Match this alternative, or the
				//	next...
#تعریف_کرو عقب_2		4	//	Match "", "next" ptr points backward.
#تعریف_کرو EXACTLY		5	// str	Match this string.
#تعریف_کرو NOTHING		6	//	Match empty string.
#تعریف_کرو ستارہ		7	// node Match this (simple) thing 0 or more
				//	times.
#تعریف_کرو PLUS		8	// node Match this (simple) thing 1 or more
				//	times.
#تعریف_کرو مطابقت_2		9	// node match the operand zero-width
#تعریف_کرو NOMATCH		10	// node check for no match with operand
#تعریف_کرو BEHIND		11	// node look behind for a match with operand
#تعریف_کرو NOBEHIND	12	// node look behind for no match with operand
#تعریف_کرو SUBPAT		13	// node match the operand here
#تعریف_کرو BRACE_SIMPLE	14	// node Match this (simple) thing between m and
				//	n times (\{m,n\}).
#تعریف_کرو BOW		15	//	Match "" after [^a-zA-Z0-9_]
#تعریف_کرو EOW		16	//	Match "" at    [^a-zA-Z0-9_]
#تعریف_کرو BRACE_LIMITS	17	// nr nr  define the min & max for BRACE_SIMPLE
				//	and BRACE_COMPLEX.
#تعریف_کرو NEWL		18	//	Match line-break
#تعریف_کرو BHPOS		19	//	End position for BEHIND or NOBEHIND


// character classes: 20-48 normal, 50-78 include a line-break
#تعریف_کرو جوڑو_NL		30
#تعریف_کرو پہلا_NL	ANY + جوڑو_NL
#تعریف_کرو ANY		20	//	Match any one character.
#تعریف_کرو ANYOF		21	// str	Match any character in this string.
#تعریف_کرو ANYBUT		22	// str	Match any character not in this
				//	string.
#تعریف_کرو IDENT		23	//	Match identifier char
#تعریف_کرو SIDENT		24	//	Match identifier char but no digit
#تعریف_کرو KWORD		25	//	Match keyword char
#تعریف_کرو SKWORD		26	//	Match word char but no digit
#تعریف_کرو FNAME		27	//	Match file name char
#تعریف_کرو SFNAME		28	//	Match file name char but no digit
#تعریف_کرو PRINT		29	//	Match printable char
#تعریف_کرو SPRINT		30	//	Match printable char but no digit
#تعریف_کرو WHITE		31	//	Match whitespace char
#تعریف_کرو NWHITE		32	//	Match non-whitespace char
#تعریف_کرو DIGIT		33	//	Match digit char
#تعریف_کرو NDIGIT		34	//	Match non-digit char
#تعریف_کرو HEX		35	//	Match hex char
#تعریف_کرو NHEX		36	//	Match non-hex char
#تعریف_کرو OCTAL		37	//	Match octal char
#تعریف_کرو NOCTAL		38	//	Match non-octal char
#تعریف_کرو لفظ_2		39	//	Match word char
#تعریف_کرو NWORD		40	//	Match non-word char
#تعریف_کرو سر_2		41	//	Match head char
#تعریف_کرو NHEAD		42	//	Match non-head char
#تعریف_کرو ALPHA		43	//	Match alpha char
#تعریف_کرو NALPHA		44	//	Match non-alpha char
#تعریف_کرو زیریں_2		45	//	Match lowercase char
#تعریف_کرو NLOWER		46	//	Match non-lowercase char
#تعریف_کرو بالائی		47	//	Match uppercase char
#تعریف_کرو NUPPER		48	//	Match non-uppercase char
#تعریف_کرو آخری_NL		NUPPER + جوڑو_NL
#تعریف_کرو WITH_NL(op)	((op) >= پہلا_NL && (op) <= آخری_NL)

#تعریف_کرو MOPEN		80  // -89	 Mark this point in input as start of
				//	 \( subexpr.  MOPEN + 0 marks start of
				//	 match.
#تعریف_کرو MCLOSE		90  // -99	 Analogous to MOPEN.  MCLOSE + 0 marks
				//	 end of match.
#تعریف_کرو BACKREF		100 // -109 node Match same string again \1-\9

#اگر_تعریف_شدہ FEAT_SYN_HL
# define ZOPEN		110 // -119	 Mark this point in input as start of
				//	 \z( subexpr.
# define ZCLOSE		120 // -129	 Analogous to ZOPEN.
# define ZREF		130 // -139 node Match external submatch \z1-\z9
#اگر_ختم

#تعریف_کرو BRACE_COMPLEX	140 // -149 node Match nodes between m & n times

#تعریف_کرو NOPEN		150	//	Mark this point in input as start of
				//	\%( subexpr.
#تعریف_کرو NCLOSE		151	//	Analogous to NOPEN.

#تعریف_کرو MULTIBYTECODE	200	// mbc	Match one multi-byte character
#تعریف_کرو RE_BOF		201	//	Match "" at beginning of file.
#تعریف_کرو RE_EOF		202	//	Match "" at end of file.
#تعریف_کرو CURSOR		203	//	Match location of cursor.

#تعریف_کرو RE_LNUM		204	// nr cmp  Match line number
#تعریف_کرو RE_ستون		205	// nr cmp  Match column number
#تعریف_کرو RE_VCOL		206	// nr cmp  Match virtual column number

#تعریف_کرو RE_MARK		207	// mark cmp  Match mark position
#تعریف_کرو RE_VISUAL	208	//	Match Visual area
#تعریف_کرو RE_COMPOSING	209	// any composing characters

/*
 * Flags to be passed up and down.
 */
#تعریف_کرو HASWIDTH	0x1	// Known never to match null string.
#تعریف_کرو SIMPLE		0x2	// Simple enough to be STAR/PLUS operand.
#تعریف_کرو SPSTART		0x4	// Starts with * or +.
#تعریف_کرو HASNL		0x8	// Contains some \n.
#تعریف_کرو HASLOOKBH	0x10	// Contains "\@<=" or "\@<!".
#تعریف_کرو WORST		0	// Worst case.

ساکن صحیح	عدد_complex_braces; // Complex \{...} count
ساکن char_u	*regcode;	// Code-emit pointer, or JUST_CALC_SIZE
ساکن طویل	regsize;	// Code size.
ساکن صحیح	reg_toolong;	// TRUE when offset out of range
ساکن صحیح	bt_reg_parse_گہرائی;	// nesting depth in reg()
ساکن char_u	had_endbrace[NSUBEXP];	// flags, TRUE if end of () found
ساکن طویل	brace_min[10];	// Minimums for complex brace repeats
ساکن طویل	brace_max[10];	// Maximums for complex brace repeats
ساکن صحیح	brace_گنتی[10]; // Current counts for complex brace repeats
ساکن صحیح	one_exactly = FALSE;	// only do one char for EXACTLY

// When making changes to classchars also change nfa_classcodes.
ساکن char_u	*classchars = (char_u *)".iIkKfFpPsSdDxXoOwWhHaAlLuU";
ساکن ثابت صحیح classcodes[] = {
    ANY, IDENT, SIDENT, KWORD, SKWORD,
    FNAME, SFNAME, PRINT, SPRINT,
    WHITE, NWHITE, DIGIT, NDIGIT,
    HEX, NHEX, OCTAL, NOCTAL,
    لفظ_2, NWORD, سر_2, NHEAD,
    ALPHA, NALPHA, زیریں_2, NLOWER,
    بالائی, NUPPER
};

/*
 * Search between "start" (the first char of the range) and "end" (the closing
 * "]") and try to recognize a character class in expanded form, for example
 * [0-9].  On success, return the atom to be emitted, on failure 0.
 */
    ساکن صحیح
bt_recognize_char_class(char_u *آغاز, char_u *انتہا)
{
    صحیح		newl;
    صحیح		config = get_char_class_بٹ(آغاز, انتہا, &newl);

    // A newline would need the ADD_NL variant, don't bother with it here.
    اگر (config < 0 || newl)
	واپس 0;

	// The old engine has no case-insensitive class opcode, so [a-z] and [A-Z]
	// are left as collections.  The classes below are case-independent.
    انتخاب (config)
    {
	حالت CLASS_o9:
	    واپس DIGIT;
	حالت CLASS_not | CLASS_o9:
	    واپس NDIGIT;
	حالت CLASS_af | CLASS_AF | CLASS_o9:
	    واپس HEX;
	حالت CLASS_not | CLASS_af | CLASS_AF | CLASS_o9:
	    واپس NHEX;
	حالت CLASS_o7:
	    واپس OCTAL;
	حالت CLASS_not | CLASS_o7:
	    واپس NOCTAL;
	حالت CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    واپس لفظ_2;
	حالت CLASS_not | CLASS_az | CLASS_AZ | CLASS_o9 | CLASS_underscore:
	    واپس NWORD;
	حالت CLASS_az | CLASS_AZ | CLASS_underscore:
	    واپس سر_2;
	حالت CLASS_not | CLASS_az | CLASS_AZ | CLASS_underscore:
	    واپس NHEAD;
	حالت CLASS_az | CLASS_AZ:
	    واپس ALPHA;
	حالت CLASS_not | CLASS_az | CLASS_AZ:
	    واپس NALPHA;
    }
    واپس 0;
}

/*
 * When regcode is set to this value, code is not emitted and size is computed
 * instead.
 */
#تعریف_کرو JUST_CALC_سائز	((char_u *) -1)

// Values for rs_state in regitem_T.
قسمتعریف شمار regstate_E
{
    RS_NOPEN = 0	// NOPEN and NCLOSE
    , RS_MOPEN		// MOPEN + [0-9]
    , RS_MCLOSE		// MCLOSE + [0-9]
#اگر_تعریف_شدہ FEAT_SYN_HL
    , RS_ZOPEN		// ZOPEN + [0-9]
    , RS_ZCLOSE		// ZCLOSE + [0-9]
#اگر_ختم
    , RS_BRANCH		// BRANCH
    , RS_BRCPLX_مزید	// BRACE_COMPLEX and trying one more match
    , RS_BRCPLX_LONG	// BRACE_COMPLEX and trying longest match
    , RS_BRCPLX_SHORT	// BRACE_COMPLEX and trying shortest match
    , RS_NOMATCH	// NOMATCH
    , RS_BEHIND1	// BEHIND / NOBEHIND matching rest
    , RS_BEHIND2	// BEHIND / NOBEHIND matching behind part
    , RS_ستارہ_LONG	// STAR/PLUS/BRACE_SIMPLE longest match
    , RS_ستارہ_SHORT	// STAR/PLUS/BRACE_SIMPLE shortest match
} regstate_T;

/*
 * Structure used to save the current input state, when it needs to be
 * restored after trying a match.  Used by reg_save() and reg_restore().
 * Also stores the length of "backpos".
 */
قسمتعریف ساخت
{
    اتحاد
    {
	char_u	*ptr;	// rex.input pointer, for single-line regexp
	lpos_T	pos;	// rex.input pos, for multi-line regexp
    } rs_u;
    صحیح		rs_len;
} regsave_T;

// struct to save start/end pointer/position in for \(\)
قسمتعریف ساخت
{
    اتحاد
    {
	char_u	*ptr;
	lpos_T	pos;
    } se_u;
} save_se_T;

// used for BEHIND and NOBEHIND matching
قسمتعریف ساخت regbehind_S
{
    regsave_T	save_after;
    regsave_T	save_behind;
    صحیح		save_need_clear_subexpr;
    save_se_T   save_آغاز[NSUBEXP];
    save_se_T   save_انتہا[NSUBEXP];
} regbehind_T;

/*
 * When there are alternatives a regstate_T is put on the regstack to remember
 * what we are doing.
 * Before it may be another type of item, depending on rs_state, to remember
 * more things.
 */
قسمتعریف ساخت regitem_S
{
    regstate_T	rs_state;	// what we are doing, one of RS_ above
    مختصر	rs_no;		// submatch nr or BEHIND/NOBEHIND
    char_u	*rs_جائزہ;	// current node in program
    اتحاد
    {
	save_se_T  sesave;
	regsave_T  regsave;
    } rs_un;			// room for saving rex.input
} regitem_T;


// used for STAR, PLUS and BRACE_SIMPLE matching
قسمتعریف ساخت regstar_S
{
    صحیح		nextb;		// next byte
    صحیح		nextb_ic;	// next byte reverse case
    طویل	گنتی;
    طویل	minval;
    طویل	maxval;
} regstar_T;

// used to store input position when a BACK was encountered, so that we now if
// we made any progress since the last time.
قسمتعریف ساخت backpos_S
{
    char_u	*bp_جائزہ;	// "scan" where BACK was encountered
    regsave_T	bp_pos;		// last input position
} backpos_T;

/*
 * "regstack" and "backpos" are used by regmatch().  They are kept over calls
 * to avoid invoking malloc() and free() often.
 * "regstack" is a stack with regitem_T items, sometimes preceded by regstar_T
 * or regbehind_T.
 * "backpos_T" is a table with backpos_T for BACK
 */
ساکن garray_T	regstack = {0, 0, 0, 0, تہی};
ساکن garray_T	backpos = {0, 0, 0, 0, تہی};

ساکن regsave_T behind_pos;

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
#تعریف_کرو REGSTACK_INITIAL	2048
#تعریف_کرو BACKPOS_INITIAL		64

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
#تعریف_کرو OP(p)		((صحیح)*(p))
#تعریف_کرو اگلا_2(p)		(((*((p) + 1) & 0377) << 8) + (*((p) + 2) & 0377))
#تعریف_کرو عملیہ(p)	((p) + 3)
// Obtain an operand that was stored as four bytes, MSB first.
#تعریف_کرو عملیہ_MIN(p)	(((طویل)(p)[3] << 24) + ((طویل)(p)[4] << 16) \
			+ ((طویل)(p)[5] << 8) + (طویل)(p)[6])
// Obtain a second operand stored as four bytes.
#تعریف_کرو عملیہ_MAX(p)	عملیہ_MIN((p) + 4)
// Obtain a second single-byte operand stored after a four bytes operand.
#تعریف_کرو عملیہ_CMP(p)	(p)[7]

ساکن char_u *reg(صحیح paren, صحیح *flagp);

#اگر_تعریف_شدہ BT_REGEXP_DUMP
ساکن خالی	regdump(char_u *, bt_regprog_T *);
#اگر_ختم

ساکن صحیح	re_عدد_cmp(long_u val, char_u *جائزہ);

#اگر_تعریف_شدہ DEBUG
ساکن char_u	*regprop(char_u *);

ساکن صحیح	regnarrate = 0;
#اگر_ختم


/*
 * Setup to parse the regexp.  Used once to get the length and once to do it.
 */
    ساکن خالی
regcomp_آغاز(
    char_u	*اظہاریہ,
    صحیح		re_flags)	    // see vim_regcomp()
{
    initchr(اظہاریہ);
    اگر (re_flags & RE_MAGIC)
	reg_magic = MAGIC_ON;
    ورنہ
	reg_magic = MAGIC_OFF;
    reg_string = (re_flags & RE_STRING);
    reg_strict = (re_flags & RE_STRICT);
    get_cpo_flags();

    عدد_complex_braces = 0;
    regnpar = 1;
    CLEAR_میدان(had_endbrace);
#اگر_تعریف_شدہ FEAT_SYN_HL
    regnzpar = 1;
    re_has_z = 0;
#اگر_ختم
    regsize = 0L;
    reg_toolong = FALSE;
    bt_reg_parse_گہرائی = 0;
    regflags = 0;
#اگر defined(FEAT_SYN_HL)
    had_eol = FALSE;
#اگر_ختم
}

/*
 * Return TRUE if MULTIBYTECODE should be used instead of EXACTLY for
 * character "c".
 */
    ساکن صحیح
use_multibytecode(صحیح c)
{
    واپس has_mbyte && (*mb_char2len)(c) > 1
		     && (re_multi_type(peekchr()) != NOT_MULTI
			     || (enc_utf8 && utf_iscomposing(c)));
}

/*
 * Emit (if appropriate) a byte of code
 */
    ساکن خالی
regc(صحیح b)
{
    اگر (regcode == JUST_CALC_سائز)
	regsize++;
    ورنہ
	*regcode++ = b;
}

/*
 * Emit (if appropriate) a multi-byte character of code
 */
    ساکن خالی
regmbc(صحیح c)
{
    اگر (!has_mbyte && c > 0xff)
	واپس;
    اگر (regcode == JUST_CALC_سائز)
	regsize += (*mb_char2len)(c);
    ورنہ
	regcode += (*mb_char2bytes)(c, regcode);
}


/*
 * Produce the bytes for equivalence class "c".
 * Currently only handles latin1, latin9 and utf-8.
 * NOTE: When changing this function, also change nfa_emit_equi_class()
 */
    ساکن خالی
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
		      واپس;
	    حالت 'B': حالت 0x181: حالت 0x243: حالت 0x1e02:
	    حالت 0x1e04: حالت 0x1e06:
		      regmbc('B');
		      regmbc(0x181); regmbc(0x243); regmbc(0x1e02);
		      regmbc(0x1e04); regmbc(0x1e06);
		      واپس;
	    حالت 'C': حالت 0xc7:
	    حالت 0x106: حالت 0x108: حالت 0x10a: حالت 0x10c: حالت 0x187:
	    حالت 0x23b: حالت 0x1e08: حالت 0xa792:
		      regmbc('C'); regmbc(0xc7);
		      regmbc(0x106); regmbc(0x108); regmbc(0x10a);
		      regmbc(0x10c); regmbc(0x187); regmbc(0x23b);
		      regmbc(0x1e08); regmbc(0xa792);
		      واپس;
	    حالت 'D': حالت 0x10e: حالت 0x110: حالت 0x18a:
	    حالت 0x1e0a: حالت 0x1e0c: حالت 0x1e0e: حالت 0x1e10:
	    حالت 0x1e12:
		      regmbc('D'); regmbc(0x10e); regmbc(0x110);
		      regmbc(0x18a); regmbc(0x1e0a); regmbc(0x1e0c);
		      regmbc(0x1e0e); regmbc(0x1e10); regmbc(0x1e12);
		      واپس;
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
		      واپس;
	    حالت 'F': حالت 0x191: حالت 0x1e1e: حالت 0xa798:
		      regmbc('F'); regmbc(0x191); regmbc(0x1e1e);
		      regmbc(0xa798);
		      واپس;
	    حالت 'G': حالت 0x11c: حالت 0x11e: حالت 0x120:
	    حالت 0x122: حالت 0x193: حالت 0x1e4: حالت 0x1e6:
	    حالت 0x1f4: حالت 0x1e20: حالت 0xa7a0:
		      regmbc('G'); regmbc(0x11c); regmbc(0x11e);
		      regmbc(0x120); regmbc(0x122); regmbc(0x193);
		      regmbc(0x1e4); regmbc(0x1e6); regmbc(0x1f4);
		      regmbc(0x1e20); regmbc(0xa7a0);
		      واپس;
	    حالت 'H': حالت 0x124: حالت 0x126: حالت 0x21e:
	    حالت 0x1e22: حالت 0x1e24: حالت 0x1e26:
	    حالت 0x1e28: حالت 0x1e2a: حالت 0x2c67:
		      regmbc('H'); regmbc(0x124); regmbc(0x126);
		      regmbc(0x21e); regmbc(0x1e22); regmbc(0x1e24);
		      regmbc(0x1e26); regmbc(0x1e28); regmbc(0x1e2a);
		      regmbc(0x2c67);
		      واپس;
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
		      واپس;
	    حالت 'J': حالت 0x134: حالت 0x248:
		      regmbc('J'); regmbc(0x134); regmbc(0x248);
		      واپس;
	    حالت 'K': حالت 0x136: حالت 0x198: حالت 0x1e8: حالت 0x1e30:
	    حالت 0x1e32: حالت 0x1e34: حالت 0x2c69: حالت 0xa740:
		      regmbc('K'); regmbc(0x136); regmbc(0x198);
		      regmbc(0x1e8); regmbc(0x1e30); regmbc(0x1e32);
		      regmbc(0x1e34); regmbc(0x2c69); regmbc(0xa740);
		      واپس;
	    حالت 'L': حالت 0x139: حالت 0x13b: حالت 0x13d: حالت 0x13f:
	    حالت 0x141: حالت 0x23d: حالت 0x1e36: حالت 0x1e38:
	    حالت 0x1e3a: حالت 0x1e3c: حالت 0x2c60:
		      regmbc('L'); regmbc(0x139); regmbc(0x13b);
		      regmbc(0x13d); regmbc(0x13f); regmbc(0x141);
		      regmbc(0x23d); regmbc(0x1e36); regmbc(0x1e38);
		      regmbc(0x1e3a); regmbc(0x1e3c); regmbc(0x2c60);
		      واپس;
	    حالت 'M': حالت 0x1e3e: حالت 0x1e40: حالت 0x1e42:
		      regmbc('M'); regmbc(0x1e3e); regmbc(0x1e40);
		      regmbc(0x1e42);
		      واپس;
	    حالت 'N': حالت 0xd1:
	    حالت 0x143: حالت 0x145: حالت 0x147: حالت 0x1f8:
	    حالت 0x1e44: حالت 0x1e46: حالت 0x1e48: حالت 0x1e4a:
	    حالت 0xa7a4:
		      regmbc('N'); regmbc(0xd1);
		      regmbc(0x143); regmbc(0x145); regmbc(0x147);
		      regmbc(0x1f8); regmbc(0x1e44); regmbc(0x1e46);
		      regmbc(0x1e48); regmbc(0x1e4a); regmbc(0xa7a4);
		      واپس;
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
		      واپس;
	    حالت 'P': حالت 0x1a4: حالت 0x1e54: حالت 0x1e56: حالت 0x2c63:
		      regmbc('P'); regmbc(0x1a4); regmbc(0x1e54);
		      regmbc(0x1e56); regmbc(0x2c63);
		      واپس;
	    حالت 'Q': حالت 0x24a:
		      regmbc('Q'); regmbc(0x24a);
		      واپس;
	    حالت 'R': حالت 0x154: حالت 0x156: حالت 0x158: حالت 0x210:
	    حالت 0x212: حالت 0x24c: حالت 0x1e58: حالت 0x1e5a:
	    حالت 0x1e5c: حالت 0x1e5e: حالت 0x2c64: حالت 0xa7a6:
		      regmbc('R'); regmbc(0x154); regmbc(0x156);
		      regmbc(0x210); regmbc(0x212); regmbc(0x158);
		      regmbc(0x24c); regmbc(0x1e58); regmbc(0x1e5a);
		      regmbc(0x1e5c); regmbc(0x1e5e); regmbc(0x2c64);
		      regmbc(0xa7a6);
		      واپس;
	    حالت 'S': حالت 0x15a: حالت 0x15c: حالت 0x15e: حالت 0x160:
	    حالت 0x218: حالت 0x1e60: حالت 0x1e62: حالت 0x1e64:
	    حالت 0x1e66: حالت 0x1e68: حالت 0x2c7e: حالت 0xa7a8:
		      regmbc('S'); regmbc(0x15a); regmbc(0x15c);
		      regmbc(0x15e); regmbc(0x160); regmbc(0x218);
		      regmbc(0x1e60); regmbc(0x1e62); regmbc(0x1e64);
		      regmbc(0x1e66); regmbc(0x1e68); regmbc(0x2c7e);
			  regmbc(0xa7a8);
		      واپس;
	    حالت 'T': حالت 0x162: حالت 0x164: حالت 0x166: حالت 0x1ac:
	    حالت 0x1ae: حالت 0x21a: حالت 0x23e: حالت 0x1e6a: حالت 0x1e6c:
	    حالت 0x1e6e: حالت 0x1e70:
		      regmbc('T'); regmbc(0x162); regmbc(0x164);
		      regmbc(0x166); regmbc(0x1ac); regmbc(0x23e);
		      regmbc(0x1ae); regmbc(0x21a); regmbc(0x1e6a);
		      regmbc(0x1e6c); regmbc(0x1e6e); regmbc(0x1e70);
		      واپس;
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
		      واپس;
	    حالت 'V': حالت 0x1b2: حالت 0x1e7c: حالت 0x1e7e:
		      regmbc('V'); regmbc(0x1b2); regmbc(0x1e7c);
		      regmbc(0x1e7e);
		      واپس;
	    حالت 'W': حالت 0x174: حالت 0x1e80: حالت 0x1e82:
	    حالت 0x1e84: حالت 0x1e86: حالت 0x1e88:
		      regmbc('W'); regmbc(0x174); regmbc(0x1e80);
		      regmbc(0x1e82); regmbc(0x1e84); regmbc(0x1e86);
		      regmbc(0x1e88);
		      واپس;
	    حالت 'X': حالت 0x1e8a: حالت 0x1e8c:
		      regmbc('X'); regmbc(0x1e8a); regmbc(0x1e8c);
		      واپس;
	    حالت 'Y': حالت 0xdd:
	    حالت 0x176: حالت 0x178: حالت 0x1b3: حالت 0x232: حالت 0x24e:
	    حالت 0x1e8e: حالت 0x1ef2: حالت 0x1ef6: حالت 0x1ef4: حالت 0x1ef8:
		      regmbc('Y'); regmbc(0xdd); regmbc(0x176);
		      regmbc(0x178); regmbc(0x1b3); regmbc(0x232);
		      regmbc(0x24e); regmbc(0x1e8e); regmbc(0x1ef2);
		      regmbc(0x1ef4); regmbc(0x1ef6); regmbc(0x1ef8);
		      واپس;
	    حالت 'Z': حالت 0x179: حالت 0x17b: حالت 0x17d: حالت 0x1b5:
	    حالت 0x1e90: حالت 0x1e92: حالت 0x1e94: حالت 0x2c6b:
		      regmbc('Z'); regmbc(0x179); regmbc(0x17b);
		      regmbc(0x17d); regmbc(0x1b5); regmbc(0x1e90);
		      regmbc(0x1e92); regmbc(0x1e94); regmbc(0x2c6b);
		      واپس;
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
		      واپس;
	    حالت 'b': حالت 0x180: حالت 0x253: حالت 0x1d6c: حالت 0x1d80:
	    حالت 0x1e03: حالت 0x1e05: حالت 0x1e07:
		      regmbc('b');
		      regmbc(0x180); regmbc(0x253); regmbc(0x1d6c);
		      regmbc(0x1d80); regmbc(0x1e03); regmbc(0x1e05);
		      regmbc(0x1e07);
		      واپس;
	    حالت 'c': حالت 0xe7:
	    حالت 0x107: حالت 0x109: حالت 0x10b: حالت 0x10d: حالت 0x188:
	    حالت 0x23c: حالت 0x1e09: حالت 0xa793: حالت 0xa794:
		      regmbc('c'); regmbc(0xe7); regmbc(0x107);
		      regmbc(0x109); regmbc(0x10b); regmbc(0x10d);
		      regmbc(0x188); regmbc(0x23c); regmbc(0x1e09);
		      regmbc(0xa793); regmbc(0xa794);
		      واپس;
	    حالت 'd': حالت 0x10f: حالت 0x111: حالت 0x257: حالت 0x1d6d:
	    حالت 0x1d81: حالت 0x1d91: حالت 0x1e0b: حالت 0x1e0d:
	    حالت 0x1e0f: حالت 0x1e11: حالت 0x1e13:
		      regmbc('d'); regmbc(0x10f); regmbc(0x111);
		      regmbc(0x257); regmbc(0x1d6d); regmbc(0x1d81);
		      regmbc(0x1d91); regmbc(0x1e0b); regmbc(0x1e0d);
		      regmbc(0x1e0f); regmbc(0x1e11); regmbc(0x1e13);
		      واپس;
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
		      واپس;
	    حالت 'f': حالت 0x192: حالت 0x1d6e: حالت 0x1d82:
	    حالت 0x1e1f: حالت 0xa799:
		     regmbc('f'); regmbc(0x192); regmbc(0x1d6e);
		     regmbc(0x1d82); regmbc(0x1e1f); regmbc(0xa799);
		     واپس;
	    حالت 'g': حالت 0x11d: حالت 0x11f: حالت 0x121: حالت 0x123:
	    حالت 0x1e5: حالت 0x1e7: حالت 0x260: حالت 0x1f5: حالت 0x1d83:
	    حالت 0x1e21: حالت 0xa7a1:
		      regmbc('g'); regmbc(0x11d); regmbc(0x11f);
		      regmbc(0x121); regmbc(0x123); regmbc(0x1e5);
		      regmbc(0x1e7); regmbc(0x1f5); regmbc(0x260);
		      regmbc(0x1d83); regmbc(0x1e21); regmbc(0xa7a1);
		      واپس;
	    حالت 'h': حالت 0x125: حالت 0x127: حالت 0x21f: حالت 0x1e23:
	    حالت 0x1e25: حالت 0x1e27: حالت 0x1e29: حالت 0x1e2b:
	    حالت 0x1e96: حالت 0x2c68: حالت 0xa795:
		      regmbc('h'); regmbc(0x125); regmbc(0x127);
		      regmbc(0x21f); regmbc(0x1e23); regmbc(0x1e25);
		      regmbc(0x1e27); regmbc(0x1e29); regmbc(0x1e2b);
		      regmbc(0x1e96); regmbc(0x2c68); regmbc(0xa795);
		      واپس;
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
		      واپس;
	    حالت 'j': حالت 0x135: حالت 0x1f0: حالت 0x249:
		      regmbc('j'); regmbc(0x135); regmbc(0x1f0);
		      regmbc(0x249);
		      واپس;
	    حالت 'k': حالت 0x137: حالت 0x199: حالت 0x1e9:
	    حالت 0x1d84: حالت 0x1e31: حالت 0x1e33: حالت 0x1e35:
	    حالت 0x2c6a: حالت 0xa741:
		      regmbc('k'); regmbc(0x137); regmbc(0x199);
		      regmbc(0x1e9); regmbc(0x1d84); regmbc(0x1e31);
		      regmbc(0x1e33); regmbc(0x1e35); regmbc(0x2c6a);
		      regmbc(0xa741);
		      واپس;
	    حالت 'l': حالت 0x13a: حالت 0x13c: حالت 0x13e:
	    حالت 0x140: حالت 0x142: حالت 0x19a: حالت 0x1e37:
	    حالت 0x1e39: حالت 0x1e3b: حالت 0x1e3d: حالت 0x2c61:
		      regmbc('l'); regmbc(0x13a); regmbc(0x13c);
		      regmbc(0x13e); regmbc(0x140); regmbc(0x142);
		      regmbc(0x19a); regmbc(0x1e37); regmbc(0x1e39);
		      regmbc(0x1e3b); regmbc(0x1e3d); regmbc(0x2c61);
		      واپس;
	    حالت 'm': حالت 0x1d6f: حالت 0x1e3f: حالت 0x1e41: حالت 0x1e43:
		      regmbc('m'); regmbc(0x1d6f); regmbc(0x1e3f);
		      regmbc(0x1e41); regmbc(0x1e43);
		      واپس;
	    حالت 'n': حالت 0xf1: حالت 0x144: حالت 0x146: حالت 0x148:
	    حالت 0x149: حالت 0x1f9: حالت 0x1d70: حالت 0x1d87:
	    حالت 0x1e45: حالت 0x1e47: حالت 0x1e49: حالت 0x1e4b:
	    حالت 0xa7a5:
		      regmbc('n'); regmbc(0xf1); regmbc(0x144);
		      regmbc(0x146); regmbc(0x148); regmbc(0x149);
		      regmbc(0x1f9); regmbc(0x1d70); regmbc(0x1d87);
		      regmbc(0x1e45); regmbc(0x1e47); regmbc(0x1e49);
		      regmbc(0x1e4b); regmbc(0xa7a5);
		      واپس;
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
		      واپس;
	    حالت 'p': حالت 0x1a5: حالت 0x1d71: حالت 0x1d88: حالت 0x1d7d:
	    حالت 0x1e55: حالت 0x1e57:
		      regmbc('p'); regmbc(0x1a5); regmbc(0x1d71);
		      regmbc(0x1d7d); regmbc(0x1d88); regmbc(0x1e55);
		      regmbc(0x1e57);
		      واپس;
	    حالت 'q': حالت 0x24b: حالت 0x2a0:
		      regmbc('q'); regmbc(0x24b); regmbc(0x2a0);
		      واپس;
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
		      واپس;
	    حالت 's': حالت 0x15b: حالت 0x15d: حالت 0x15f: حالت 0x161:
	    حالت 0x1e61: حالت 0x219: حالت 0x23f: حالت 0x1d74: حالت 0x1d8a:
	    حالت 0x1e63: حالت 0x1e65: حالت 0x1e67: حالت 0x1e69: حالت 0xa7a9:
		      regmbc('s'); regmbc(0x15b); regmbc(0x15d);
		      regmbc(0x15f); regmbc(0x161); regmbc(0x23f);
		      regmbc(0x219); regmbc(0x1d74); regmbc(0x1d8a);
		      regmbc(0x1e61); regmbc(0x1e63); regmbc(0x1e65);
		      regmbc(0x1e67); regmbc(0x1e69); regmbc(0xa7a9);
		      واپس;
	    حالت 't': حالت 0x163: حالت 0x165: حالت 0x167: حالت 0x1ab:
	    حالت 0x1ad: حالت 0x21b: حالت 0x288: حالت 0x1d75: حالت 0x1e6b:
	    حالت 0x1e6d: حالت 0x1e6f: حالت 0x1e71: حالت 0x1e97: حالت 0x2c66:
		      regmbc('t'); regmbc(0x163); regmbc(0x165);
		      regmbc(0x167); regmbc(0x1ab); regmbc(0x21b);
		      regmbc(0x1ad); regmbc(0x288); regmbc(0x1d75);
		      regmbc(0x1e6b); regmbc(0x1e6d); regmbc(0x1e6f);
		      regmbc(0x1e71); regmbc(0x1e97); regmbc(0x2c66);
		      واپس;
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
		      واپس;
	    حالت 'v': حالت 0x28b: حالت 0x1d8c: حالت 0x1e7d: حالت 0x1e7f:
		      regmbc('v'); regmbc(0x28b); regmbc(0x1d8c);
		      regmbc(0x1e7d); regmbc(0x1e7f);
		      واپس;
	    حالت 'w': حالت 0x175: حالت 0x1e81: حالت 0x1e83:
	    حالت 0x1e85: حالت 0x1e87: حالت 0x1e89: حالت 0x1e98:
		      regmbc('w'); regmbc(0x175); regmbc(0x1e81);
		      regmbc(0x1e83); regmbc(0x1e85); regmbc(0x1e87);
		      regmbc(0x1e89); regmbc(0x1e98);
		      واپس;
	    حالت 'x': حالت 0x1e8b: حالت 0x1e8d:
		      regmbc('x'); regmbc(0x1e8b); regmbc(0x1e8d);
		      واپس;
	    حالت 'y': حالت 0xfd: حالت 0xff: حالت 0x177: حالت 0x1b4:
	    حالت 0x233: حالت 0x24f: حالت 0x1e8f: حالت 0x1e99: حالت 0x1ef3:
	    حالت 0x1ef5: حالت 0x1ef7: حالت 0x1ef9:
		      regmbc('y'); regmbc(0xfd); regmbc(0xff);
		      regmbc(0x177); regmbc(0x1b4); regmbc(0x233);
		      regmbc(0x24f); regmbc(0x1e8f); regmbc(0x1e99);
		      regmbc(0x1ef3); regmbc(0x1ef5); regmbc(0x1ef7);
		      regmbc(0x1ef9);
		      واپس;
	    حالت 'z': حالت 0x17a: حالت 0x17c: حالت 0x17e: حالت 0x1b6:
	    حالت 0x1d76: حالت 0x1d8e: حالت 0x1e91: حالت 0x1e93:
	    حالت 0x1e95: حالت 0x2c6c:
		      regmbc('z'); regmbc(0x17a); regmbc(0x17c);
		      regmbc(0x17e); regmbc(0x1b6); regmbc(0x1d76);
		      regmbc(0x1d8e); regmbc(0x1e91); regmbc(0x1e93);
		      regmbc(0x1e95); regmbc(0x2c6c);
		      واپس;
	}
    }
    regmbc(c);
}

/*
 * Emit a node.
 * Return pointer to generated code.
 */
    ساکن char_u *
regnode(صحیح op)
{
    char_u  *ret;

    ret = regcode;
    اگر (ret == JUST_CALC_سائز)
	regsize += 3;
    ورنہ
    {
	*regcode++ = op;
	*regcode++ = NUL;		// Null "next" pointer.
	*regcode++ = NUL;
    }
    واپس ret;
}

/*
 * Write a long as four bytes at "p" and return pointer to the next char.
 */
    ساکن char_u *
re_put_long(char_u *p, long_u val)
{
    *p++ = (char_u) ((val >> 24) & 0377);
    *p++ = (char_u) ((val >> 16) & 0377);
    *p++ = (char_u) ((val >> 8) & 0377);
    *p++ = (char_u) (val & 0377);
    واپس p;
}

/*
 * regnext - dig the "next" pointer out of a node
 * Returns NULL when calculating size, when there is no next item and when
 * there is an error.
 */
    ساکن char_u *
regnext(char_u *p)
{
    صحیح	    انحراف;

    اگر (p == JUST_CALC_سائز || reg_toolong)
	واپس تہی;

    انحراف = اگلا_2(p);
    اگر (انحراف == 0)
	واپس تہی;

    اگر (OP(p) == عقب_2)
	واپس p - انحراف;
    ورنہ
	واپس p + انحراف;
}

/*
 * Set the next-pointer at the end of a node chain.
 */
    ساکن خالی
regtail(char_u *p, char_u *val)
{
    char_u	*جائزہ;
    char_u	*عارضی;
    صحیح		انحراف;

    اگر (p == JUST_CALC_سائز)
	واپس;

    // Find last node.
    جائزہ = p;
    برائے (;;)
    {
	عارضی = regnext(جائزہ);
	اگر (عارضی == تہی)
	    توڑو;
	جائزہ = عارضی;
    }

    اگر (OP(جائزہ) == عقب_2)
	انحراف = (صحیح)(جائزہ - val);
    ورنہ
	انحراف = (صحیح)(val - جائزہ);
    // When the offset uses more than 16 bits it can no longer fit in the two
    // bytes available.  Use a global flag to avoid having to check return
    // values in too many places.
    اگر (انحراف > 0xffff)
	reg_toolong = TRUE;
    ورنہ
    {
	*(جائزہ + 1) = (char_u) (((غیرعلامتی)انحراف >> 8) & 0377);
	*(جائزہ + 2) = (char_u) (انحراف & 0377);
    }
}

/*
 * Like regtail, on item after a BRANCH; nop if none.
 */
    ساکن خالی
regoptail(char_u *p, char_u *val)
{
    // When op is neither BRANCH nor BRACE_COMPLEX0-9, it is "operandless"
    اگر (p == تہی || p == JUST_CALC_سائز
	    || (OP(p) != BRANCH
		&& (OP(p) < BRACE_COMPLEX || OP(p) > BRACE_COMPLEX + 9)))
	واپس;
    regtail(عملیہ(p), val);
}

/*
 * Insert an operator in front of already-emitted operand
 *
 * Means relocating the operand.
 */
    ساکن خالی
reginsert(صحیح op, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*رکھو;

    اگر (regcode == JUST_CALC_سائز)
    {
	regsize += 3;
	واپس;
    }
    src = regcode;
    regcode += 3;
    dst = regcode;
    جب (src > opnd)
	*--dst = *--src;

    رکھو = opnd;		// Op node, where operand used to be.
    *رکھو++ = op;
    *رکھو++ = NUL;
    *رکھو = NUL;
}

/*
 * Insert an operator in front of already-emitted operand.
 * Add a number to the operator.
 */
    ساکن خالی
reginsert_nr(صحیح op, طویل val, char_u *opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*رکھو;

    اگر (regcode == JUST_CALC_سائز)
    {
	regsize += 7;
	واپس;
    }
    src = regcode;
    regcode += 7;
    dst = regcode;
    جب (src > opnd)
	*--dst = *--src;

    رکھو = opnd;		// Op node, where operand used to be.
    *رکھو++ = op;
    *رکھو++ = NUL;
    *رکھو++ = NUL;
    re_put_long(رکھو, (long_u)val);
}

/*
 * Insert an operator in front of already-emitted operand.
 * The operator has the given limit values as operands.  Also set next pointer.
 *
 * Means relocating the operand.
 */
    ساکن خالی
reginsert_limits(
    صحیح		op,
    طویل	minval,
    طویل	maxval,
    char_u	*opnd)
{
    char_u	*src;
    char_u	*dst;
    char_u	*رکھو;

    اگر (regcode == JUST_CALC_سائز)
    {
	regsize += 11;
	واپس;
    }
    src = regcode;
    regcode += 11;
    dst = regcode;
    جب (src > opnd)
	*--dst = *--src;

    رکھو = opnd;		// Op node, where operand used to be.
    *رکھو++ = op;
    *رکھو++ = NUL;
    *رکھو++ = NUL;
    رکھو = re_put_long(رکھو, (long_u)minval);
    رکھو = re_put_long(رکھو, (long_u)maxval);
    regtail(opnd, رکھو);
}

/*
 * Return TRUE if the back reference is legal. We must have seen the close
 * brace.
 * TODO: Should also check that we don't refer to something that is repeated
 * (+*=): what instance of the repetition should we match?
 */
    ساکن صحیح
نظر_آیا_endbrace(صحیح refnum)
{
    اگر (!had_endbrace[refnum])
    {
	char_u *p;

	// Trick: check if "@<=" or "@<!" follows, in which case
	// the \1 can appear before the referenced match.
	برائے (p = regparse; *p != NUL; ++p)
	    اگر (p[0] == '@' && p[1] == '<' && (p[2] == '!' || p[2] == '='))
		توڑو;
	اگر (*p == NUL)
	{
	    emsg(_(e_illegal_عقب_reference));
	    rc_did_emsg = TRUE;
	    واپس FALSE;
	}
    }
    واپس TRUE;
}

/*
 * Parse the lowest level.
 *
 * Optimization:  gobbles an entire sequence of ordinary characters so that
 * it can turn them into a single node, which is smaller to store and
 * faster to run.  Don't do this when one_exactly is set.
 */
    ساکن char_u *
regatom(صحیح *flagp)
{
    char_u	    *ret;
    صحیح		    flags;
    صحیح		    c;
    char_u	    *p;
    صحیح		    extra = 0;
    صحیح		    save_پچھلا_at_آغاز = پچھلا_at_آغاز;

    *flagp = WORST;		// Tentatively.

    c = getchr();
    انتخاب (c)
    {
      حالت Magic('^'):
	ret = regnode(BOL);
	توڑو;

      حالت Magic('$'):
	ret = regnode(EOL);
#اگر defined(FEAT_SYN_HL)
	had_eol = TRUE;
#اگر_ختم
	توڑو;

      حالت Magic('<'):
	ret = regnode(BOW);
	توڑو;

      حالت Magic('>'):
	ret = regnode(EOW);
	توڑو;

      حالت Magic('_'):
	c = no_Magic(getchr());
	اگر (c == '^')		// "\_^" is start-of-line
	{
	    ret = regnode(BOL);
	    توڑو;
	}
	اگر (c == '$')		// "\_$" is end-of-line
	{
	    ret = regnode(EOL);
#اگر defined(FEAT_SYN_HL)
	    had_eol = TRUE;
#اگر_ختم
	    توڑو;
	}

	extra = جوڑو_NL;
	*flagp |= HASNL;

	// "\_[" is character range plus newline
	اگر (c == '[')
	    جاؤ collection;

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
	اگر (p == تہی)
	    EMSG_RET_NULL(_(e_invalid_use_کا_underscore));

	// When '.' is followed by a composing char ignore the dot, so that
	// the composing char is matched here.
	اگر (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	{
	    c = getchr();
	    جاؤ do_multibyte;
	}
	ret = regnode(classcodes[p - classchars] + extra);
	*flagp |= HASWIDTH | SIMPLE;
	توڑو;

      حالت Magic('n'):
	اگر (reg_string)
	{
	    // In a string "\n" matches a newline character.
	    ret = regnode(EXACTLY);
	    regc(NL);
	    regc(NUL);
	    *flagp |= HASWIDTH | SIMPLE;
	}
	ورنہ
	{
	    // In buffer text "\n" matches the end of a line.
	    ret = regnode(NEWL);
	    *flagp |= HASWIDTH | HASNL;
	}
	توڑو;

      حالت Magic('('):
	اگر (one_exactly)
	    EMSG_ONE_RET_NULL;
	ret = reg(REG_PAREN, &flags);
	اگر (ret == تہی)
	    واپس تہی;
	*flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
	توڑو;

      حالت NUL:
      حالت Magic('|'):
      حالت Magic('&'):
      حالت Magic(')'):
	اگر (one_exactly)
	    EMSG_ONE_RET_NULL;
	// Supposed to be caught earlier.
	IEMSG_RET_NULL(e_اندرونی_error_اندر_regexp);
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
	    اگر (reg_پچھلا_sub != تہی)
	    {
		char_u	    *lp;

		ret = regnode(EXACTLY);
		lp = reg_پچھلا_sub;
		جب (*lp != NUL)
		    regc(*lp++);
		regc(NUL);
		اگر (*reg_پچھلا_sub != NUL)
		{
		    *flagp |= HASWIDTH;
		    اگر ((lp - reg_پچھلا_sub) == 1)
			*flagp |= SIMPLE;
		}
	    }
	    ورنہ
		EMSG_RET_NULL(_(e_no_پچھلا_substitute_regular_عبارت));
	    توڑو;

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
		اگر (!نظر_آیا_endbrace(refnum))
		    واپس تہی;
		ret = regnode(BACKREF + refnum);
	    }
	    توڑو;

      حالت Magic('z'):
	{
	    c = no_Magic(getchr());
	    انتخاب (c)
	    {
#اگر_تعریف_شدہ FEAT_SYN_HL
		حالت '(': اگر ((reg_do_extmatch & REX_سیٹ) == 0)
			      EMSG_RET_NULL(_(e_z_not_allowed_یہاں));
			  اگر (one_exactly)
			      EMSG_ONE_RET_NULL;
			  ret = reg(REG_ZPAREN, &flags);
			  اگر (ret == تہی)
			      واپس تہی;
			  *flagp |= flags & (HASWIDTH|SPSTART|HASNL|HASLOOKBH);
			  re_has_z = REX_سیٹ;
			  توڑو;

		حالت '1':
		حالت '2':
		حالت '3':
		حالت '4':
		حالت '5':
		حالت '6':
		حالت '7':
		حالت '8':
		حالت '9': اگر ((reg_do_extmatch & REX_USE) == 0)
			      EMSG_RET_NULL(_(e_z1_z9_not_allowed_یہاں));
			  ret = regnode(ZREF + c - '0');
			  re_has_z = REX_USE;
			  توڑو;
#اگر_ختم

		حالت 's': ret = regnode(MOPEN + 0);
			  اگر (re_mult_اگلا("\\zs") == FAIL)
			      واپس تہی;
			  توڑو;

		حالت 'e': ret = regnode(MCLOSE + 0);
			  اگر (re_mult_اگلا("\\ze") == FAIL)
			      واپس تہی;
			  توڑو;

		طےشدہ:  EMSG_RET_NULL(_(e_invalid_character_after_bsl_z));
	    }
	}
	توڑو;

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
		    اگر (ret == تہی)
			واپس تہی;
		    *flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
		    توڑو;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		حالت '^':
		    ret = regnode(RE_BOF);
		    توڑو;

		حالت '$':
		    ret = regnode(RE_EOF);
		    توڑو;

		حالت '#':
		    اگر (regparse[0] == '=' && regparse[1] >= 48
							  && regparse[1] <= 50)
		    {
			// misplaced \%#=1
			semsg(_(e_atom_engine_must_be_at_آغاز_کا_نمونہ),
								  regparse[1]);
			واپس FAIL;
		    }
		    ret = regnode(CURSOR);
		    توڑو;

		حالت 'V':
		    ret = regnode(RE_VISUAL);
		    توڑو;

		حالت 'C':
		    ret = regnode(RE_COMPOSING);
		    توڑو;

		// \%[abc]: Emit as a list of branches, all ending at the last
		// branch which matches nothing.
		حالت '[':
			  اگر (one_exactly)	// doesn't nest
			      EMSG_ONE_RET_NULL;
			  {
			      char_u	*lastbranch;
			      char_u	*lastnode = تہی;
			      char_u	*br;

			      ret = تہی;
			      جب ((c = getchr()) != ']')
			      {
				  اگر (c == NUL)
				      EMSG2_RET_NULL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
				  br = regnode(BRANCH);
				  اگر (ret == تہی)
				      ret = br;
				  ورنہ
				  {
				      regtail(lastnode, br);
				      اگر (reg_toolong)
					  واپس تہی;
				  }

				  ungetchr();
				  one_exactly = TRUE;
				  lastnode = regatom(flagp);
				  one_exactly = FALSE;
				  اگر (lastnode == تہی)
				      واپس تہی;
			      }
			      اگر (ret == تہی)
				  EMSG2_RET_NULL(_(e_خالی_خانے_str_brackets),
						      reg_magic == MAGIC_ALL);
			      lastbranch = regnode(BRANCH);
			      br = regnode(NOTHING);
			      اگر (ret != JUST_CALC_سائز)
			      {
				  regtail(lastnode, br);
				  regtail(lastbranch, br);
				  // connect all branches to the NOTHING
				  // branch at the end
				  برائے (br = ret; br != lastnode; )
				  {
				      اگر (OP(br) == BRANCH)
				      {
					  regtail(br, lastbranch);
					  اگر (reg_toolong)
					      واپس تہی;
					  br = عملیہ(br);
				      }
				      ورنہ
					  br = regnext(br);
				  }
			      }
			      *flagp &= ~(HASWIDTH | SIMPLE);
			      توڑو;
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
				  حالت 'd': i = getdecchrs(); توڑو;
				  حالت 'o': i = getoctchrs(); توڑو;
				  حالت 'x': i = gethexchrs(2); توڑو;
				  حالت 'u': i = gethexchrs(4); توڑو;
				  حالت 'U': i = gethexchrs(8); توڑو;
				  طےشدہ:  i = -1; توڑو;
			      }

			      اگر (i < 0 || i > زیادہ_سے_زیادہ_عدد)
				  EMSG2_RET_NULL(
					    _(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			      اگر (use_multibytecode(i))
				  ret = regnode(MULTIBYTECODE);
			      ورنہ
				  ret = regnode(EXACTLY);
			      اگر (i == 0)
				  regc(0x0a);
			      ورنہ
				  regmbc((صحیح)i);
			      regc(NUL);
			      *flagp |= HASWIDTH;
			      توڑو;
			  }

		طےشدہ:
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
			      جب (VIM_ISDIGIT(c))
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
				  اگر (ret == JUST_CALC_سائز)
				      regsize += 2;
				  ورنہ
				  {
				      *regcode++ = c;
				      *regcode++ = cmp;
				  }
				  توڑو;
			      }
			      ورنہ اگر ((c == 'l' || c == 'c' || c == 'v')
					  && (cur || got_digit))
			      {
				  اگر (cur && n)
				  {
				    semsg(_(e_regexp_نمبر_after_dot_pos_تلاش_chr),
								  no_Magic(c));
				    rc_did_emsg = TRUE;
				    واپس تہی;
				  }
				  اگر (c == 'l')
				  {
				      اگر (cur)
					  n = curwin->w_cursor.lnum;
				      ret = regnode(RE_LNUM);
				      اگر (save_پچھلا_at_آغاز)
					  at_آغاز = TRUE;
				  }
				  ورنہ اگر (c == 'c')
				  {
				      اگر (cur)
				      {
					  n = curwin->w_cursor.ستون;
					  n++;
				      }
				      ret = regnode(RE_ستون);
				  }
				  ورنہ
				  {
				      اگر (cur)
				      {
					  colnr_T vcol = 0;

					  getvvcol(curwin, &curwin->w_cursor,
							 تہی, تہی, &vcol, 0);
					  ++vcol;
					  n = vcol;
				      }
				      ret = regnode(RE_VCOL);
				  }
				  اگر (ret == JUST_CALC_سائز)
				      regsize += 5;
				  ورنہ
				  {
				      // put the number and the optional
				      // comparator after the opcode
				      regcode = re_put_long(regcode, n);
				      *regcode++ = cmp;
				  }
				  توڑو;
			      }
			  }

			  EMSG2_RET_NULL(_(e_invalid_character_after_str),
						      reg_magic == MAGIC_ALL);
	    }
	}
	توڑو;

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
			توڑو;
		    }
		}

		// In a character class, different parsing rules apply.
		// Not even \ is special anymore, nothing is.
		اگر (*regparse == '^')	    // Complement of range.
		{
		    ret = regnode(ANYBUT + extra);
		    regparse++;
		}
		ورنہ
		    ret = regnode(ANYOF + extra);

		// At the start ']' and '-' mean the literal character.
		اگر (*regparse == ']' || *regparse == '-')
		{
		    startc = *regparse;
		    regc(*regparse++);
		}

		جب (*regparse != NUL && *regparse != ']')
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
			ورنہ
			{
			    // Also accept "a-[.z.]"
			    endc = 0;
			    اگر (*regparse == '[')
				endc = get_coll_element(&regparse);
			    اگر (endc == 0)
			    {
				اگر (has_mbyte)
				    endc = mb_ptr2char_adv(&regparse);
				ورنہ
				    endc = *regparse++;
			    }

			    // Handle \o40, \x20 and \u20AC style sequences
			    اگر (endc == '\\' && !reg_cpo_lit && !reg_cpo_bsl)
				endc = coll_get_char();

			    اگر (startc > endc)
				EMSG_RET_NULL(_(e_الٹ_range_اندر_character_class));
			    اگر (has_mbyte && ((*mb_char2len)(startc) > 1
						 || (*mb_char2len)(endc) > 1))
			    {
				// Limit to a range of 256 chars.
				اگر (endc > startc + 256)
				    EMSG_RET_NULL(_(e_range_too_large_اندر_character_class));
				جب (++startc <= endc)
				    regmbc(startc);
			    }
			    ورنہ
			    {
				جب (++startc <= endc)
				    regc(startc);
			    }
			    startc = -1;
			}
		    }
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    ورنہ اگر (*regparse == '\\'
			    && !reg_cpo_bsl
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != تہی
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR,
						       regparse[1]) != تہی)))
		    {
			regparse++;
			اگر (*regparse == 'n')
			{
			    // '\n' in range: also match NL
			    اگر (ret != JUST_CALC_سائز)
			    {
				// Using \n inside [^] does not change what
				// matches. "[^\n]" is the same as ".".
				اگر (*ret == ANYOF)
				{
				    *ret = ANYOF + جوڑو_NL;
				    *flagp |= HASNL;
				}
				// else: must have had a \n already
			    }
			    regparse++;
			    startc = -1;
			}
			ورنہ اگر (*regparse == 'd'
				|| *regparse == 'o'
				|| *regparse == 'x'
				|| *regparse == 'u'
				|| *regparse == 'U')
			{
			    startc = coll_get_char();
			    // max UTF-8 Codepoint is U+10FFFF,
			    // but allow values until INT_MAX
			    اگر (startc == زیادہ_سے_زیادہ_عدد)
				EMSG_RET_NULL(_(e_unicode_val_too_large));
			    اگر (startc == 0)
				regc(0x0a);
			    ورنہ
				regmbc(startc);
			}
			ورنہ
			{
			    startc = backslash_trans(*regparse++);
			    regc(startc);
			}
		    }
		    ورنہ اگر (*regparse == '[')
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
				ورنہ اگر ((c_class =
					    get_coll_element(&regparse)) != 0)
				{
				    // produce a collating element
				    regmbc(c_class);
				}
				ورنہ
				{
				    // literal '[', allow [[-x] as a range
				    startc = *regparse++;
				    regc(startc);
				}
				توڑو;
			    حالت CLASS_ALNUM:
				برائے (cu = 1; cu < 128; cu++)
				    اگر (حرف_یا_ہندسہ(cu))
					regmbc(cu);
				توڑو;
			    حالت CLASS_ALPHA:
				برائے (cu = 1; cu < 128; cu++)
				    اگر (حرف_ہے(cu))
					regmbc(cu);
				توڑو;
			    حالت CLASS_BLANK:
				regc(' ');
				regc('\t');
				توڑو;
			    حالت CLASS_CNTRL:
				برائے (cu = 1; cu <= 127; cu++)
				    اگر (iscntrl(cu))
					regmbc(cu);
				توڑو;
			    حالت CLASS_DIGIT:
				برائے (cu = 1; cu <= 127; cu++)
				    اگر (VIM_ISDIGIT(cu))
					regmbc(cu);
				توڑو;
			    حالت CLASS_گراف:
				برائے (cu = 1; cu <= 127; cu++)
				    اگر (isgraph(cu))
					regmbc(cu);
				توڑو;
			    حالت CLASS_زیریں:
				برائے (cu = 1; cu <= 255; cu++)
				    اگر (MB_ISLOWER(cu) && cu != 170
								 && cu != 186)
					regmbc(cu);
				توڑو;
			    حالت CLASS_PRINT:
				برائے (cu = 1; cu <= 255; cu++)
				    اگر (vim_isprintc(cu))
					regmbc(cu);
				توڑو;
			    حالت CLASS_PUNCT:
				برائے (cu = 1; cu < 128; cu++)
				    اگر (ispunct(cu))
					regmbc(cu);
				توڑو;
			    حالت CLASS_SPACE:
				برائے (cu = 9; cu <= 13; cu++)
				    regc(cu);
				regc(' ');
				توڑو;
			    حالت CLASS_بالائی:
				برائے (cu = 1; cu <= 255; cu++)
				    اگر (MB_ISUPPER(cu))
					regmbc(cu);
				توڑو;
			    حالت CLASS_XDIGIT:
				برائے (cu = 1; cu <= 255; cu++)
				    اگر (vim_isxdigit(cu))
					regmbc(cu);
				توڑو;
			    حالت CLASS_TAB:
				regc('\t');
				توڑو;
			    حالت CLASS_RETURN:
				regc('\r');
				توڑو;
			    حالت CLASS_BACKSPACE:
				regc('\b');
				توڑو;
			    حالت CLASS_ESCAPE:
				regc('\033');
				توڑو;
			    حالت CLASS_IDENT:
				برائے (cu = 1; cu <= 255; cu++)
				    اگر (vim_isIDc(cu))
					regmbc(cu);
				توڑو;
			    حالت CLASS_KEYWORD:
				برائے (cu = 1; cu <= 255; cu++)
				    اگر (reg_iswordc(cu))
					regmbc(cu);
				توڑو;
			    حالت CLASS_FNAME:
				برائے (cu = 1; cu <= 255; cu++)
				    اگر (vim_isfilec(cu))
					regmbc(cu);
				توڑو;
			}
		    }
		    ورنہ
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
			    جب (--len >= 0)
				regc(*regparse++);
			}
			ورنہ
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
		توڑو;
	    }
	    ورنہ اگر (reg_strict)
		EMSG2_RET_NULL(_(e_missing_rsb_after_str_lsb),
							reg_magic > MAGIC_OFF);
	}
	// FALLTHROUGH

      طےشدہ:
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
		توڑو;
	    }

	    ret = regnode(EXACTLY);

	    // Append characters as long as:
	    // - there is no following multi, we then need the character in
	    //   front of it as a single character operand
	    // - not running into a Magic character
	    // - "one_exactly" is not set
	    // But always emit at least one character.  Might be a Multi,
	    // e.g., a "[" without matching "]".
	    برائے (len = 0; c != NUL && (len == 0
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
			برائے (;;)
			{
			    l = utf_ptr2len(regparse);
			    اگر (!UTF_COMPOSINGLIKE(regparse, regparse + l))
				توڑو;
			    regmbc(utf_ptr2char(regparse));
			    skipchr();
			}
		    }
		}
		ورنہ
		    regc(c);
		c = getchr();
	    }
	    ungetchr();

	    regc(NUL);
	    *flagp |= HASWIDTH;
	    اگر (len == 1)
		*flagp |= SIMPLE;
	}
	توڑو;
    }

    واپس ret;
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
    ساکن char_u *
regpiece(صحیح *flagp)
{
    char_u	    *ret;
    صحیح		    op;
    char_u	    *اگلا;
    صحیح		    flags;
    طویل	    minval;
    طویل	    maxval;

    ret = regatom(&flags);
    اگر (ret == تہی)
	واپس تہی;

    op = peekchr();
    اگر (re_multi_type(op) == NOT_MULTI)
    {
	*flagp = flags;
	واپس ret;
    }
    // default flags
    *flagp = (WORST | SPSTART | (flags & (HASNL | HASLOOKBH)));

    skipchr();
    انتخاب (op)
    {
	حالت Magic('*'):
	    اگر (flags & SIMPLE)
		reginsert(ستارہ, ret);
	    ورنہ
	    {
		// Emit x* as (x&|), where & means "self".
		reginsert(BRANCH, ret); // Either x
		regoptail(ret, regnode(عقب_2));	// and loop
		regoptail(ret, ret);	//عقب
		regtail(ret, regnode(BRANCH));	// or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    توڑو;

	حالت Magic('+'):
	    اگر (flags & SIMPLE)
		reginsert(PLUS, ret);
	    ورنہ
	    {
		// Emit x+ as x(&|), where & means "self".
		اگلا = regnode(BRANCH); // Either
		regtail(ret, اگلا);
		regtail(regnode(عقب_2), ret);	// loop back
		regtail(اگلا, regnode(BRANCH)); // or
		regtail(ret, regnode(NOTHING)); // null.
	    }
	    *flagp = (WORST | HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    توڑو;

	حالت Magic('@'):
	    {
		صحیح	lop = انتہا_3;
		طویل	nr;

		nr = (طویل)getdecchrs();
		انتخاب (no_Magic(getchr()))
		{
		    حالت '=': lop = مطابقت_2; توڑو;		  // \@=
		    حالت '!': lop = NOMATCH; توڑو;		  // \@!
		    حالت '>': lop = SUBPAT; توڑو;		  // \@>
		    حالت '<': انتخاب (no_Magic(getchr()))
			      {
				  حالت '=': lop = BEHIND; توڑو;   // \@<=
				  حالت '!': lop = NOBEHIND; توڑو; // \@<!
			      }
		}
		اگر (lop == انتہا_3)
		    EMSG2_RET_NULL(_(e_invalid_character_after_str_at),
						      reg_magic == MAGIC_ALL);
		// Look behind must match with behind_pos.
		اگر (lop == BEHIND || lop == NOBEHIND)
		{
		    regtail(ret, regnode(BHPOS));
		    *flagp |= HASLOOKBH;
		}
		regtail(ret, regnode(انتہا_3)); // operand ends
		اگر (lop == BEHIND || lop == NOBEHIND)
		{
		    اگر (nr < 0)
			nr = 0; // no limit is same as zero limit
		    reginsert_nr(lop, nr, ret);
		}
		ورنہ
		    reginsert(lop, ret);
		توڑو;
	    }

	حالت Magic('?'):
	حالت Magic('='):
	    // Emit x= as (x|)
	    reginsert(BRANCH, ret);		// Either x
	    regtail(ret, regnode(BRANCH));	// or
	    اگلا = regnode(NOTHING);		// null.
	    regtail(ret, اگلا);
	    regoptail(ret, اگلا);
	    توڑو;

	حالت Magic('{'):
	    اگر (!read_limits(&minval, &maxval))
		واپس تہی;
	    اگر (flags & SIMPLE)
	    {
		reginsert(BRACE_SIMPLE, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
	    }
	    ورنہ
	    {
		اگر (عدد_complex_braces >= 10)
		    EMSG2_RET_NULL(_(e_too_many_complex_str_curly),
						      reg_magic == MAGIC_ALL);
		reginsert(BRACE_COMPLEX + عدد_complex_braces, ret);
		regoptail(ret, regnode(عقب_2));
		regoptail(ret, ret);
		reginsert_limits(BRACE_LIMITS, minval, maxval, ret);
		++عدد_complex_braces;
	    }
	    اگر (minval > 0 && maxval > 0)
		*flagp = (HASWIDTH | (flags & (HASNL | HASLOOKBH)));
	    توڑو;
    }
    اگر (re_multi_type(peekchr()) != NOT_MULTI)
    {
	// Can't have a multi follow a multi.
	اگر (peekchr() == Magic('*'))
	    EMSG2_RET_NULL(_(e_nested_str), reg_magic >= MAGIC_ON);
	EMSG3_RET_NULL(_(e_nested_str_chr), reg_magic == MAGIC_ALL,
							  no_Magic(peekchr()));
    }

    واپس ret;
}

/*
 * Parse one alternative of an | or & operator.
 * Implements the concatenation operator.
 */
    ساکن char_u *
regconcat(صحیح *flagp)
{
    char_u	*پہلا = تہی;
    char_u	*chain = تہی;
    char_u	*latest;
    صحیح		flags;
    صحیح		cont = TRUE;

    *flagp = WORST;		// Tentatively.

    جب (cont)
    {
	انتخاب (peekchr())
	{
	    حالت NUL:
	    حالت Magic('|'):
	    حالت Magic('&'):
	    حالت Magic(')'):
			    cont = FALSE;
			    توڑو;
	    حالت Magic('Z'):
			    regflags |= RF_ICOMBINE;
			    skipchr_keepstart();
			    توڑو;
	    حالت Magic('c'):
			    regflags |= RF_ICASE;
			    skipchr_keepstart();
			    توڑو;
	    حالت Magic('C'):
			    regflags |= RF_NOICASE;
			    skipchr_keepstart();
			    توڑو;
	    حالت Magic('v'):
			    reg_magic = MAGIC_ALL;
			    skipchr_keepstart();
			    curchr = -1;
			    توڑو;
	    حالت Magic('m'):
			    reg_magic = MAGIC_ON;
			    skipchr_keepstart();
			    curchr = -1;
			    توڑو;
	    حالت Magic('M'):
			    reg_magic = MAGIC_OFF;
			    skipchr_keepstart();
			    curchr = -1;
			    توڑو;
	    حالت Magic('V'):
			    reg_magic = MAGIC_NONE;
			    skipchr_keepstart();
			    curchr = -1;
			    توڑو;
	    طےشدہ:
			    latest = regpiece(&flags);
			    اگر (latest == تہی || reg_toolong)
				واپس تہی;
			    *flagp |= flags & (HASWIDTH | HASNL | HASLOOKBH);
			    اگر (chain == تہی)	// First piece.
				*flagp |= flags & SPSTART;
			    ورنہ
				regtail(chain, latest);
			    chain = latest;
			    اگر (پہلا == تہی)
				پہلا = latest;
			    توڑو;
	}
    }
    اگر (پہلا == تہی)		// Loop ran zero times.
	پہلا = regnode(NOTHING);
    واپس پہلا;
}

/*
 * Parse one alternative of an | operator.
 * Implements the & operator.
 */
    ساکن char_u *
regbranch(صحیح *flagp)
{
    char_u	*ret;
    char_u	*chain = تہی;
    char_u	*latest;
    صحیح		flags;

    *flagp = WORST | HASNL;		// Tentatively.

    ret = regnode(BRANCH);
    برائے (;;)
    {
	latest = regconcat(&flags);
	اگر (latest == تہی)
	    واپس تہی;
	// If one of the branches has width, the whole thing has.  If one of
	// the branches anchors at start-of-line, the whole thing does.
	// If one of the branches uses look-behind, the whole thing does.
	*flagp |= flags & (HASWIDTH | SPSTART | HASLOOKBH);
	// If one of the branches doesn't match a line-break, the whole thing
	// doesn't.
	*flagp &= ~HASNL | (flags & HASNL);
	اگر (chain != تہی)
	    regtail(chain, latest);
	اگر (peekchr() != Magic('&'))
	    توڑو;
	skipchr();
	regtail(latest, regnode(انتہا_3)); // operand ends
	اگر (reg_toolong)
	    توڑو;
	reginsert(مطابقت_2, latest);
	chain = latest;
    }

    واپس ret;
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
    ساکن char_u *
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

#اگر_تعریف_شدہ FEAT_SYN_HL
    اگر (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	اگر (regnzpar >= NSUBEXP)
	    EMSG_RET_NULL(_(e_too_many_z));
	parno = regnzpar;
	regnzpar++;
	ret = regnode(ZOPEN + parno);
    }
    ورنہ
#اگر_ختم
	اگر (paren == REG_PAREN)
    {
	// Make a MOPEN node.
	اگر (regnpar >= NSUBEXP)
	    EMSG2_RET_NULL(_(e_too_many_str_open), reg_magic == MAGIC_ALL);
	parno = regnpar;
	++regnpar;
	ret = regnode(MOPEN + parno);
    }
    ورنہ اگر (paren == REG_NPAREN)
    {
	// Make a NOPEN node.
	ret = regnode(NOPEN);
    }
    ورنہ
	ret = تہی;

    اگر (bt_reg_parse_گہرائی >= REG_MAX_PAREN_گہرائی)
	EMSG_RET_NULL(_(e_command_too_complex));
    ++bt_reg_parse_گہرائی;

    // Pick up the branches, linking them together.
    br = regbranch(&flags);
    اگر (br == تہی)
    {
	ret = تہی;
	جاؤ theend;
    }
    اگر (ret != تہی)
	regtail(ret, br);	// [MZ]OPEN -> first.
    ورنہ
	ret = br;
    // If one of the branches can be zero-width, the whole thing can.
    // If one of the branches has * at start or matches a line-break, the
    // whole thing can.
    اگر (!(flags & HASWIDTH))
	*flagp &= ~HASWIDTH;
    *flagp |= flags & (SPSTART | HASNL | HASLOOKBH);
    جب (peekchr() == Magic('|'))
    {
	skipchr();
	br = regbranch(&flags);
	اگر (br == تہی || reg_toolong)
	{
	    ret = تہی;
	    جاؤ theend;
	}
	regtail(ret, br);	// BRANCH -> BRANCH.
	اگر (!(flags & HASWIDTH))
	    *flagp &= ~HASWIDTH;
	*flagp |= flags & (SPSTART | HASNL | HASLOOKBH);
    }

    // Make a closing node, and hook it on the end.
    ender = regnode(
#اگر_تعریف_شدہ FEAT_SYN_HL
	    paren == REG_ZPAREN ? ZCLOSE + parno :
#اگر_ختم
	    paren == REG_PAREN ? MCLOSE + parno :
	    paren == REG_NPAREN ? NCLOSE : انتہا_3);
    regtail(ret, ender);

    // Hook the tails of the branches to the closing node.
    برائے (br = ret; br != تہی; br = regnext(br))
	regoptail(br, ender);

    // Check for proper termination.
    اگر (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
#اگر_تعریف_شدہ FEAT_SYN_HL
	اگر (paren == REG_ZPAREN)
	{
	    emsg(_(e_unmatched_z));
	    rc_did_emsg = TRUE;
	    ret = تہی;
	    جاؤ theend;
	}
	ورنہ
#اگر_ختم
	اگر (paren == REG_NPAREN)
	{
	    semsg(_(e_unmatched_str_percent_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = تہی;
	    جاؤ theend;
	}
	ورنہ
	{
	    semsg(_(e_unmatched_str_open),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = تہی;
	    جاؤ theend;
	}
    }
    ورنہ اگر (paren == REG_NOPAREN && peekchr() != NUL)
    {
	اگر (curchr == Magic(')'))
	{
	    semsg(_(e_unmatched_str_close),
				       reg_magic == MAGIC_ALL ? "" : "\\");
	    rc_did_emsg = TRUE;
	    ret = تہی;
	    جاؤ theend;
	}
	ورنہ
	{
	    emsg(_(e_trailing_characters));	// "Can't happen".
	    rc_did_emsg = TRUE;
	    ret = تہی;
	    جاؤ theend;
	}
    }
    // Here we set the flag allowing back references to this set of
    // parentheses.
    اگر (paren == REG_PAREN)
	had_endbrace[parno] = TRUE;	// have seen the close paren

theend:
    --bt_reg_parse_گہرائی;
    واپس ret;
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
    ساکن regprog_T *
bt_regcomp(char_u *اظہاریہ, صحیح re_flags)
{
    bt_regprog_T    *r;
    char_u	*جائزہ;
    char_u	*longest;
    صحیح		len;
    صحیح		flags;

    اگر (اظہاریہ == تہی)
	IEMSG_RET_NULL(e_null_دلیل);

    init_class_tab();

    // First pass: determine size, legality.
    regcomp_آغاز(اظہاریہ, re_flags);
    regcode = JUST_CALC_سائز;
    regc(REGMAGIC);
    اگر (reg(REG_NOPAREN, &flags) == تہی)
	واپس تہی;

    // Allocate space.
    r = alloc(offsetof(bt_regprog_T, پروگرام) + regsize);
    اگر (r == تہی)
	واپس تہی;
    r->re_اندر_use = FALSE;
#اگر_تعریف_شدہ DEBUG
    r->regsz = regsize;
#اگر_ختم

    // Second pass: emit code.
    regcomp_آغاز(اظہاریہ, re_flags);
    regcode = r->پروگرام;
    regc(REGMAGIC);
    اگر (reg(REG_NOPAREN, &flags) == تہی || reg_toolong)
    {
	vim_free(r);
	اگر (reg_toolong)
	    EMSG_RET_NULL(_(e_نمونہ_too_long));
	واپس تہی;
    }

    // Dig out information for optimizations.
    r->regstart = NUL;		// Worst-case defaults.
    r->reganch = 0;
    r->regmust = تہی;
    r->regmlen = 0;
    r->regflags = regflags;
    اگر (flags & HASNL)
	r->regflags |= RF_HASNL;
    اگر (flags & HASLOOKBH)
	r->regflags |= RF_LOOKBH;
#اگر_تعریف_شدہ FEAT_SYN_HL
    // Remember whether this pattern has any \z specials in it.
    r->reghasz = re_has_z;
#اگر_ختم
    جائزہ = r->پروگرام + 1;	// First BRANCH.
    اگر (OP(regnext(جائزہ)) == انتہا_3)   // Only one top-level choice.
    {
	جائزہ = عملیہ(جائزہ);

	// Starting-point info.
	اگر (OP(جائزہ) == BOL || OP(جائزہ) == RE_BOF)
	{
	    r->reganch++;
	    جائزہ = regnext(جائزہ);
	}

	اگر (OP(جائزہ) == EXACTLY)
	{
	    اگر (has_mbyte)
		r->regstart = (*mb_ptr2char)(عملیہ(جائزہ));
	    ورنہ
		r->regstart = *عملیہ(جائزہ);
	}
	ورنہ اگر ((OP(جائزہ) == BOW
		    || OP(جائزہ) == EOW
		    || OP(جائزہ) == NOTHING
		    || OP(جائزہ) == MOPEN + 0 || OP(جائزہ) == NOPEN
		    || OP(جائزہ) == MCLOSE + 0 || OP(جائزہ) == NCLOSE)
		 && OP(regnext(جائزہ)) == EXACTLY)
	{
	    اگر (has_mbyte)
		r->regstart = (*mb_ptr2char)(عملیہ(regnext(جائزہ)));
	    ورنہ
		r->regstart = *عملیہ(regnext(جائزہ));
	}

	// If there's something expensive in the r.e., find the longest
	// literal string that must appear and make it the regmust.  Resolve
	// ties in favor of later strings, since the regstart check works
	// with the beginning of the r.e. and avoiding duplication
	// strengthens checking.  Not a strong reason, but sufficient in the
	// absence of others.

	// When the r.e. starts with BOW, it is faster to look for a regmust
	// first. Used a lot for "#" and "*" commands. (Added by mool).
	اگر ((flags & SPSTART || OP(جائزہ) == BOW || OP(جائزہ) == EOW)
							  && !(flags & HASNL))
	{
	    size_t  scanlen;

	    longest = تہی;
	    len = 0;
	    برائے (; جائزہ != تہی; جائزہ = regnext(جائزہ))
	    {
		اگر (OP(جائزہ) == EXACTLY)
		{
		    scanlen = STRLEN(عملیہ(جائزہ));
		    اگر (scanlen >= (size_t)len)
		    {
			longest = عملیہ(جائزہ);
			len = (صحیح)scanlen;
		    }
		}
	    }
	    r->regmust = longest;
	    r->regmlen = len;
	}
    }
#اگر_تعریف_شدہ BT_REGEXP_DUMP
    regdump(اظہاریہ, r);
#اگر_ختم
    r->engine = &bt_regengine;
    واپس (regprog_T *)r;
}

#اگر defined(FEAT_SYN_HL)
/*
 * Check if during the previous call to vim_regcomp the EOL item "$" has been
 * found.  This is messy, but it works fine.
 */
    صحیح
vim_regcomp_had_eol(خالی)
{
    واپس had_eol;
}
#اگر_ختم

/*
 * Get a number after a backslash that is inside [].
 * When nothing is recognized return a backslash.
 */
    ساکن صحیح
coll_get_char(خالی)
{
    vimlong_T	nr = -1;

    انتخاب (*regparse++)
    {
	حالت 'd': nr = getdecchrs(); توڑو;
	حالت 'o': nr = getoctchrs(); توڑو;
	حالت 'x': nr = gethexchrs(2); توڑو;
	حالت 'u': nr = gethexchrs(4); توڑو;
	حالت 'U': nr = gethexchrs(8); توڑو;
    }
    اگر (nr < 0)
    {
	// If getting the number fails be backwards compatible: the character
	// is a backslash.
	--regparse;
	nr = '\\';
    }
    اگر (nr > زیادہ_سے_زیادہ_عدد)
	nr = زیادہ_سے_زیادہ_عدد;
    واپس nr;
}

/*
 * Free a compiled regexp program, returned by bt_regcomp().
 */
    ساکن خالی
bt_regfree(regprog_T *prog)
{
    vim_free(prog);
}

#تعریف_کرو ADVANCE_REGINPUT() MB_PTR_ADV(rex.داخلہ)

/*
 * The arguments from BRACE_LIMITS are stored here.  They are actually local
 * to regmatch(), but they are here to reduce the amount of stack space used
 * (it can be called recursively many times).
 */
ساکن طویل	bl_minval;
ساکن طویل	bl_maxval;

/*
 * Save the input line and position in a regsave_T.
 */
    ساکن خالی
reg_save(regsave_T *save, garray_T *gap)
{
    اگر (REG_MULTI)
    {
	save->rs_u.pos.ستون = (colnr_T)(rex.داخلہ - rex.سطر);
	save->rs_u.pos.lnum = rex.lnum;
    }
    ورنہ
	save->rs_u.ptr = rex.داخلہ;
    save->rs_len = gap->ga_len;
}

/*
 * Restore the input line and position from a regsave_T.
 */
    ساکن خالی
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
	rex.داخلہ = rex.سطر + save->rs_u.pos.ستون;
    }
    ورنہ
	rex.داخلہ = save->rs_u.ptr;
    gap->ga_len = save->rs_len;
}

/*
 * Return TRUE if current position is equal to saved position.
 */
    ساکن صحیح
reg_save_equal(regsave_T *save)
{
    اگر (REG_MULTI)
	واپس rex.lnum == save->rs_u.pos.lnum
				  && rex.داخلہ == rex.سطر + save->rs_u.pos.ستون;
    واپس rex.داخلہ == save->rs_u.ptr;
}

// Save the sub-expressions before attempting a match.
#تعریف_کرو save_se(savep, posp, pp) \
    REG_MULTI ? save_se_multi((savep), (posp)) : save_se_one((savep), (pp))

// After a failed match restore the sub-expressions.
#تعریف_کرو restore_se(savep, posp, pp) \
{ \
    اگر (REG_MULTI) \
	*(posp) = (savep)->se_u.pos; \
    ورنہ \
	*(pp) = (savep)->se_u.ptr; \
}

/*
 * Tentatively set the sub-expression start to the current position (after
 * calling regmatch() they will have changed).  Need to save the existing
 * values for when there is no match.
 * Use se_save() to use pointer (save_se_multi()) or position (save_se_one()),
 * depending on REG_MULTI.
 */
    ساکن خالی
save_se_multi(save_se_T *savep, lpos_T *posp)
{
    savep->se_u.pos = *posp;
    posp->lnum = rex.lnum;
    posp->ستون = (colnr_T)(rex.داخلہ - rex.سطر);
}

    ساکن خالی
save_se_one(save_se_T *savep, char_u **pp)
{
    savep->se_u.ptr = *pp;
    *pp = rex.داخلہ;
}

/*
 * regrepeat - repeatedly match something simple, return how many.
 * Advances rex.input (and rex.lnum) to just after the matched chars.
 */
    ساکن صحیح
regrepeat(
    char_u	*p,
    طویل	maxcount)   // maximum number of matches allowed
{
    طویل	گنتی = 0;
    char_u	*جائزہ;
    char_u	*opnd;
    صحیح		نقاب;
    صحیح		testval = 0;

    جائزہ = rex.داخلہ;	    // Make local copy of rex.input for speed.
    opnd = عملیہ(p);
    انتخاب (OP(p))
    {
      حالت ANY:
      حالت ANY + جوڑو_NL:
	جب (گنتی < maxcount)
	{
	    // Matching anything means we continue until end-of-line (or
	    // end-of-file for ANY + ADD_NL), only limited by maxcount.
	    جب (*جائزہ != NUL && گنتی < maxcount)
	    {
		++گنتی;
		MB_PTR_ADV(جائزہ);
	    }
	    اگر (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
				      || rex.reg_سطر_lbr || گنتی == maxcount)
		توڑو;
	    ++گنتی;		// count the line-break
	    reg_nextline();
	    جائزہ = rex.داخلہ;
	    اگر (got_int)
		توڑو;
	}
	توڑو;

      حالت IDENT:
      حالت IDENT + جوڑو_NL:
	testval = TRUE;
	// FALLTHROUGH
      حالت SIDENT:
      حالت SIDENT + جوڑو_NL:
	جب (گنتی < maxcount)
	{
	    اگر (vim_isIDc(PTR2CHAR(جائزہ)) && (testval || !VIM_ISDIGIT(*جائزہ)))
	    {
		MB_PTR_ADV(جائزہ);
	    }
	    ورنہ اگر (*جائزہ == NUL)
	    {
		اگر (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_سطر_lbr)
		    توڑو;
		reg_nextline();
		جائزہ = rex.داخلہ;
		اگر (got_int)
		    توڑو;
	    }
	    ورنہ اگر (rex.reg_سطر_lbr && *جائزہ == '\n' && WITH_NL(OP(p)))
		++جائزہ;
	    ورنہ
		توڑو;
	    ++گنتی;
	}
	توڑو;

      حالت KWORD:
      حالت KWORD + جوڑو_NL:
	testval = TRUE;
	// FALLTHROUGH
      حالت SKWORD:
      حالت SKWORD + جوڑو_NL:
	جب (گنتی < maxcount)
	{
	    اگر (vim_iswordp_buf(جائزہ, rex.reg_buf)
					  && (testval || !VIM_ISDIGIT(*جائزہ)))
	    {
		MB_PTR_ADV(جائزہ);
	    }
	    ورنہ اگر (*جائزہ == NUL)
	    {
		اگر (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_سطر_lbr)
		    توڑو;
		reg_nextline();
		جائزہ = rex.داخلہ;
		اگر (got_int)
		    توڑو;
	    }
	    ورنہ اگر (rex.reg_سطر_lbr && *جائزہ == '\n' && WITH_NL(OP(p)))
		++جائزہ;
	    ورنہ
		توڑو;
	    ++گنتی;
	}
	توڑو;

      حالت FNAME:
      حالت FNAME + جوڑو_NL:
	testval = TRUE;
	// FALLTHROUGH
      حالت SFNAME:
      حالت SFNAME + جوڑو_NL:
	جب (گنتی < maxcount)
	{
	    اگر (vim_isfilec(PTR2CHAR(جائزہ)) && (testval || !VIM_ISDIGIT(*جائزہ)))
	    {
		MB_PTR_ADV(جائزہ);
	    }
	    ورنہ اگر (*جائزہ == NUL)
	    {
		اگر (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_سطر_lbr)
		    توڑو;
		reg_nextline();
		جائزہ = rex.داخلہ;
		اگر (got_int)
		    توڑو;
	    }
	    ورنہ اگر (rex.reg_سطر_lbr && *جائزہ == '\n' && WITH_NL(OP(p)))
		++جائزہ;
	    ورنہ
		توڑو;
	    ++گنتی;
	}
	توڑو;

      حالت PRINT:
      حالت PRINT + جوڑو_NL:
	testval = TRUE;
	// FALLTHROUGH
      حالت SPRINT:
      حالت SPRINT + جوڑو_NL:
	جب (گنتی < maxcount)
	{
	    اگر (*جائزہ == NUL)
	    {
		اگر (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_سطر_lbr)
		    توڑو;
		reg_nextline();
		جائزہ = rex.داخلہ;
		اگر (got_int)
		    توڑو;
	    }
	    ورنہ اگر (vim_isprintc(PTR2CHAR(جائزہ)) == 1
					  && (testval || !VIM_ISDIGIT(*جائزہ)))
	    {
		MB_PTR_ADV(جائزہ);
	    }
	    ورنہ اگر (rex.reg_سطر_lbr && *جائزہ == '\n' && WITH_NL(OP(p)))
		++جائزہ;
	    ورنہ
		توڑو;
	    ++گنتی;
	}
	توڑو;

      حالت WHITE:
      حالت WHITE + جوڑو_NL:
	testval = نقاب = RI_WHITE;
do_class:
	جب (گنتی < maxcount)
	{
	    صحیح		l;

	    اگر (*جائزہ == NUL)
	    {
		اگر (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_سطر_lbr)
		    توڑو;
		reg_nextline();
		جائزہ = rex.داخلہ;
		اگر (got_int)
		    توڑو;
	    }
	    ورنہ اگر (has_mbyte && (l = (*mb_ptr2len)(جائزہ)) > 1)
	    {
		اگر (testval != 0)
		    توڑو;
		جائزہ += l;
	    }
	    ورنہ اگر ((class_tab[*جائزہ] & نقاب) == testval)
		++جائزہ;
	    ورنہ اگر (rex.reg_سطر_lbr && *جائزہ == '\n' && WITH_NL(OP(p)))
		++جائزہ;
	    ورنہ
		توڑو;
	    ++گنتی;
	}
	توڑو;

      حالت NWHITE:
      حالت NWHITE + جوڑو_NL:
	نقاب = RI_WHITE;
	جاؤ do_class;
      حالت DIGIT:
      حالت DIGIT + جوڑو_NL:
	testval = نقاب = RI_DIGIT;
	جاؤ do_class;
      حالت NDIGIT:
      حالت NDIGIT + جوڑو_NL:
	نقاب = RI_DIGIT;
	جاؤ do_class;
      حالت HEX:
      حالت HEX + جوڑو_NL:
	testval = نقاب = RI_HEX;
	جاؤ do_class;
      حالت NHEX:
      حالت NHEX + جوڑو_NL:
	نقاب = RI_HEX;
	جاؤ do_class;
      حالت OCTAL:
      حالت OCTAL + جوڑو_NL:
	testval = نقاب = RI_OCTAL;
	جاؤ do_class;
      حالت NOCTAL:
      حالت NOCTAL + جوڑو_NL:
	نقاب = RI_OCTAL;
	جاؤ do_class;
      حالت لفظ_2:
      حالت لفظ_2 + جوڑو_NL:
	testval = نقاب = RI_لفظ;
	جاؤ do_class;
      حالت NWORD:
      حالت NWORD + جوڑو_NL:
	نقاب = RI_لفظ;
	جاؤ do_class;
      حالت سر_2:
      حالت سر_2 + جوڑو_NL:
	testval = نقاب = RI_سر;
	جاؤ do_class;
      حالت NHEAD:
      حالت NHEAD + جوڑو_NL:
	نقاب = RI_سر;
	جاؤ do_class;
      حالت ALPHA:
      حالت ALPHA + جوڑو_NL:
	testval = نقاب = RI_ALPHA;
	جاؤ do_class;
      حالت NALPHA:
      حالت NALPHA + جوڑو_NL:
	نقاب = RI_ALPHA;
	جاؤ do_class;
      حالت زیریں_2:
      حالت زیریں_2 + جوڑو_NL:
	testval = نقاب = RI_زیریں;
	جاؤ do_class;
      حالت NLOWER:
      حالت NLOWER + جوڑو_NL:
	نقاب = RI_زیریں;
	جاؤ do_class;
      حالت بالائی:
      حالت بالائی + جوڑو_NL:
	testval = نقاب = RI_بالائی;
	جاؤ do_class;
      حالت NUPPER:
      حالت NUPPER + جوڑو_NL:
	نقاب = RI_بالائی;
	جاؤ do_class;

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
		جب (گنتی < maxcount && (*جائزہ == cu || *جائزہ == cl))
		{
		    گنتی++;
		    جائزہ++;
		}
	    }
	    ورنہ
	    {
		cu = *opnd;
		جب (گنتی < maxcount && *جائزہ == cu)
		{
		    گنتی++;
		    جائزہ++;
		}
	    }
	    توڑو;
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
		جب (گنتی < maxcount && (*mb_ptr2len)(جائزہ) >= len)
		{
		    برائے (i = 0; i < len; ++i)
			اگر (opnd[i] != جائزہ[i])
			    توڑو;
		    اگر (i < len && (!rex.reg_ic || !enc_utf8
					|| utf_fold(utf_ptr2char(جائزہ)) != cf))
			توڑو;
		    جائزہ += len;
		    ++گنتی;
		}
	    }
	}
	توڑو;

      حالت ANYOF:
      حالت ANYOF + جوڑو_NL:
	testval = TRUE;
	// FALLTHROUGH

      حالت ANYBUT:
      حالت ANYBUT + جوڑو_NL:
	جب (گنتی < maxcount)
	{
	    صحیح len;

	    اگر (*جائزہ == NUL)
	    {
		اگر (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_سطر_lbr)
		    توڑو;
		reg_nextline();
		جائزہ = rex.داخلہ;
		اگر (got_int)
		    توڑو;
	    }
	    ورنہ اگر (rex.reg_سطر_lbr && *جائزہ == '\n' && WITH_NL(OP(p)))
		++جائزہ;
	    ورنہ اگر (has_mbyte && (len = (*mb_ptr2len)(جائزہ)) > 1)
	    {
		اگر ((cstrchr(opnd, (*mb_ptr2char)(جائزہ)) == تہی) == testval)
		    توڑو;
		جائزہ += len;
	    }
	    ورنہ
	    {
		اگر ((cstrchr(opnd, *جائزہ) == تہی) == testval)
		    توڑو;
		++جائزہ;
	    }
	    ++گنتی;
	}
	توڑو;

      حالت NEWL:
	جب (گنتی < maxcount
		&& ((*جائزہ == NUL && rex.lnum <= rex.reg_maxline
				       && !rex.reg_سطر_lbr && REG_MULTI)
		    || (*جائزہ == '\n' && rex.reg_سطر_lbr)))
	{
	    گنتی++;
	    اگر (rex.reg_سطر_lbr)
		ADVANCE_REGINPUT();
	    ورنہ
		reg_nextline();
	    جائزہ = rex.داخلہ;
	    اگر (got_int)
		توڑو;
	}
	توڑو;

      طےشدہ:			// Oh dear.  Called inappropriately.
	iemsg(e_corrupted_regexp_پروگرام);
#اگر_تعریف_شدہ DEBUG
	لکھو("Called regrepeat with op code %d\n", OP(p));
#اگر_ختم
	توڑو;
    }

    rex.داخلہ = جائزہ;

    واپس (صحیح)گنتی;
}

/*
 * Push an item onto the regstack.
 * Returns pointer to new item.  Returns NULL when out of memory.
 */
    ساکن regitem_T *
regstack_ڈالو(regstate_T state, char_u *جائزہ)
{
    regitem_T	*rp;

    اگر ((طویل)((غیرعلامتی)regstack.ga_len >> 10) >= p_mmp)
    {
	emsg(_(e_نمونہ_استعمال_کرتا_مزید_حافظہ_than_maxmempattern));
	واپس تہی;
    }
    اگر (GA_GROW_FAILS(&regstack, (صحیح)حجم(regitem_T)))
	واپس تہی;

    rp = (regitem_T *)((حرف *)regstack.ga_ڈیٹا + regstack.ga_len);
    rp->rs_state = state;
    rp->rs_جائزہ = جائزہ;

    regstack.ga_len += حجم(regitem_T);
    واپس rp;
}

/*
 * Pop an item from the regstack.
 */
    ساکن خالی
regstack_نکالو(char_u **جائزہ)
{
    regitem_T	*rp;

    rp = (regitem_T *)((حرف *)regstack.ga_ڈیٹا + regstack.ga_len) - 1;
    *جائزہ = rp->rs_جائزہ;

    regstack.ga_len -= حجم(regitem_T);
}

#اگر_تعریف_شدہ FEAT_RELTIME
/*
 * Check if the timer expired, return TRUE if so.
 */
    ساکن صحیح
bt_did_وقت_برآمد(صحیح *timed_برآمد)
{
    اگر (*timeout_flag)
    {
	اگر (timed_برآمد != تہی)
	{
# ifdef FEAT_EVAL
	    اگر (!*timed_برآمد)
		ch_log(تہی, "BT regexp timed out");
# endif
	    *timed_برآمد = TRUE;
	}
	واپس TRUE;
    }
    واپس FALSE;
}
#اگر_ختم

/*
 * Save the current subexpr to "bp", so that they can be restored
 * later by restore_subexpr().
 */
    ساکن خالی
save_subexpr(regbehind_T *bp)
{
    صحیح i;

    // When "rex.need_clear_subexpr" is set we don't need to save the values,
    // only remember that this flag needs to be set again when restoring.
    bp->save_need_clear_subexpr = rex.need_clear_subexpr;
    اگر (rex.need_clear_subexpr)
	واپس;

    برائے (i = 0; i < NSUBEXP; ++i)
    {
	اگر (REG_MULTI)
	{
	    bp->save_آغاز[i].se_u.pos = rex.reg_startpos[i];
	    bp->save_انتہا[i].se_u.pos = rex.reg_endpos[i];
	}
	ورنہ
	{
	    bp->save_آغاز[i].se_u.ptr = rex.reg_startp[i];
	    bp->save_انتہا[i].se_u.ptr = rex.reg_endp[i];
	}
    }
}

/*
 * Restore the subexpr from "bp".
 */
    ساکن خالی
restore_subexpr(regbehind_T *bp)
{
    صحیح i;

    // Only need to restore saved values when they are not to be cleared.
    rex.need_clear_subexpr = bp->save_need_clear_subexpr;
    اگر (rex.need_clear_subexpr)
	واپس;

    برائے (i = 0; i < NSUBEXP; ++i)
    {
	اگر (REG_MULTI)
	{
	    rex.reg_startpos[i] = bp->save_آغاز[i].se_u.pos;
	    rex.reg_endpos[i] = bp->save_انتہا[i].se_u.pos;
	}
	ورنہ
	{
	    rex.reg_startp[i] = bp->save_آغاز[i].se_u.ptr;
	    rex.reg_endp[i] = bp->save_انتہا[i].se_u.ptr;
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
    ساکن صحیح
regmatch(
    char_u	*جائزہ,		    // Current node.
    صحیح		*timed_برآمد غیر_مستعمل)  // flag set on timeout or NULL
{
  char_u	*اگلا;		// Next node.
  صحیح		op;
  صحیح		c;
  regitem_T	*rp;
  صحیح		no;
  صحیح		حالت_2;		// one of the RA_ values:

  // Make "regstack" and "backpos" empty.  They are allocated and freed in
  // bt_regexec_both() to reduce malloc()/free() calls.
  regstack.ga_len = 0;
  backpos.ga_len = 0;

  // Repeat until "regstack" is empty.
  برائے (;;)
  {
    // Some patterns may take a long time to match, e.g., "\([a-z]\+\)\+Q".
    // Allow interrupting them with CTRL-C.
    fast_breakcheck();

#اگر_تعریف_شدہ DEBUG
    اگر (جائزہ != تہی && regnarrate)
    {
	mch_errmsg((حرف *)regprop(جائزہ));
	mch_errmsg("(\n");
    }
#اگر_ختم

    // Repeat for items that can be matched sequentially, without using the
    // regstack.
    برائے (;;)
    {
	اگر (got_int || جائزہ == تہی)
	{
	    حالت_2 = RA_FAIL;
	    توڑو;
	}
#اگر_تعریف_شدہ FEAT_RELTIME
	اگر (bt_did_وقت_برآمد(timed_برآمد))
	{
	    حالت_2 = RA_FAIL;
	    توڑو;
	}
#اگر_ختم
	حالت_2 = RA_CONT;

#اگر_تعریف_شدہ DEBUG
	اگر (regnarrate)
	{
	    mch_errmsg((حرف *)regprop(جائزہ));
	    mch_errmsg("...\n");
# ifdef FEAT_SYN_HL
	    اگر (re_extmatch_اندر != تہی)
	    {
		صحیح i;

		mch_errmsg(_("External submatches:\n"));
		برائے (i = 0; i < NSUBEXP; i++)
		{
		    mch_errmsg("    \"");
		    اگر (re_extmatch_اندر->matches[i] != تہی)
			mch_errmsg((حرف *)re_extmatch_اندر->matches[i]);
		    mch_errmsg("\"\n");
		}
	    }
# endif
	}
#اگر_ختم
	اگلا = regnext(جائزہ);

	op = OP(جائزہ);
	// Check for character class with NL added.
	اگر (!rex.reg_سطر_lbr && WITH_NL(op) && REG_MULTI
			     && *rex.داخلہ == NUL && rex.lnum <= rex.reg_maxline)
	{
	    reg_nextline();
	}
	ورنہ اگر (rex.reg_سطر_lbr && WITH_NL(op) && *rex.داخلہ == '\n')
	{
	    ADVANCE_REGINPUT();
	}
	ورنہ
	{
	  اگر (WITH_NL(op))
	      op -= جوڑو_NL;
	  اگر (has_mbyte)
	      c = (*mb_ptr2char)(rex.داخلہ);
	  ورنہ
	      c = *rex.داخلہ;
	  انتخاب (op)
	  {
	  حالت BOL:
	    اگر (rex.داخلہ != rex.سطر)
		حالت_2 = RA_NOMATCH;
	    توڑو;

	  حالت EOL:
	    اگر (c != NUL)
		حالت_2 = RA_NOMATCH;
	    توڑو;

	  حالت RE_BOF:
	    // We're not at the beginning of the file when below the first
	    // line where we started, not at the start of the line or we
	    // didn't start at the first line of the buffer.
	    اگر (rex.lnum != 0 || rex.داخلہ != rex.سطر
				       || (REG_MULTI && rex.reg_firstlnum > 1))
		حالت_2 = RA_NOMATCH;
	    توڑو;

	  حالت RE_EOF:
	    اگر (rex.lnum != rex.reg_maxline || c != NUL)
		حالت_2 = RA_NOMATCH;
	    توڑو;

	  حالت CURSOR:
	    // Check if the buffer is in a window and compare the
	    // rex.reg_win->w_cursor position to the match position.
	    اگر (rex.reg_win == تہی
		    || (rex.lnum + rex.reg_firstlnum
						 != rex.reg_win->w_cursor.lnum)
		    || ((colnr_T)(rex.داخلہ - rex.سطر)
						 != rex.reg_win->w_cursor.ستون))
		حالت_2 = RA_NOMATCH;
	    توڑو;

	  حالت RE_MARK:
	    // Compare the mark position to the match position.
	    {
		صحیح	mark = عملیہ(جائزہ)[0];
		صحیح	cmp = عملیہ(جائزہ)[1];
		pos_T	*pos;
		size_t	ستون = REG_MULTI ? rex.داخلہ - rex.سطر : 0;

		pos = getmark_buf(rex.reg_buf, mark, FALSE);

		// Line may have been freed, get it again.
		اگر (REG_MULTI)
		{
		    rex.سطر = reg_getline(rex.lnum);
		    rex.داخلہ = rex.سطر + ستون;
		}

		اگر (pos == تہی		     // mark doesn't exist
			|| pos->lnum <= 0)   // mark isn't set in reg_buf
		{
		    حالت_2 = RA_NOMATCH;
		}
		ورنہ
		{
		    colnr_T pos_ستون = pos->lnum == rex.lnum + rex.reg_firstlnum
							  && pos->ستون == MAXCOL
				      ? reg_getline_len(pos->lnum - rex.reg_firstlnum)
				      : pos->ستون;

		    اگر ((pos->lnum == rex.lnum + rex.reg_firstlnum
				? (pos_ستون == (colnr_T)(rex.داخلہ - rex.سطر)
				    ? (cmp == '<' || cmp == '>')
				    : (pos_ستون < (colnr_T)(rex.داخلہ - rex.سطر)
					? cmp != '>'
					: cmp != '<'))
				: (pos->lnum < rex.lnum + rex.reg_firstlnum
				    ? cmp != '>'
				    : cmp != '<')))
		    حالت_2 = RA_NOMATCH;
		}
	    }
	    توڑو;

	  حالت RE_VISUAL:
	    اگر (!reg_مطابقت_visual())
		حالت_2 = RA_NOMATCH;
	    توڑو;

	  حالت RE_LNUM:
	    اگر (!REG_MULTI || !re_عدد_cmp((long_u)(rex.lnum + rex.reg_firstlnum),
									جائزہ))
		حالت_2 = RA_NOMATCH;
	    توڑو;

	  حالت RE_ستون:
	    اگر (!re_عدد_cmp((long_u)(rex.داخلہ - rex.سطر) + 1, جائزہ))
		حالت_2 = RA_NOMATCH;
	    توڑو;

	  حالت RE_VCOL:
	    {
		win_T	    *wp = rex.reg_win == تہی ? curwin : rex.reg_win;
		linenr_T    lnum = REG_MULTI ? rex.reg_firstlnum + rex.lnum : 1;
		long_u	    vcol;

		اگر (REG_MULTI && (lnum <= 0
				   || lnum > wp->w_بفر->b_ml.ml_سطر_گنتی))
		    lnum = 1;
		vcol = (long_u)win_linetabsize(wp, lnum, rex.سطر,
					      (colnr_T)(rex.داخلہ - rex.سطر));
		اگر (!re_عدد_cmp(vcol + 1, جائزہ))
		    حالت_2 = RA_NOMATCH;
	    }
	    توڑو;

	  حالت BOW:	// \<word; rex.input points to w
	    اگر (c == NUL)	// Can't match at end of line
		حالت_2 = RA_NOMATCH;
	    ورنہ اگر (has_mbyte)
	    {
		صحیح this_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.داخلہ, rex.reg_buf);
		اگر (this_class <= 1)
		    حالت_2 = RA_NOMATCH;  // not on a word at all
		ورنہ اگر (reg_پچھلا_class() == this_class)
		    حالت_2 = RA_NOMATCH;  // previous char is in same word
	    }
	    ورنہ
	    {
		اگر (!vim_iswordc_buf(c, rex.reg_buf) || (rex.داخلہ > rex.سطر
				&& vim_iswordc_buf(rex.داخلہ[-1], rex.reg_buf)))
		    حالت_2 = RA_NOMATCH;
	    }
	    توڑو;

	  حالت EOW:	// word\>; rex.input points after d
	    اگر (rex.داخلہ == rex.سطر)    // Can't match at start of line
		حالت_2 = RA_NOMATCH;
	    ورنہ اگر (has_mbyte)
	    {
		صحیح this_class, پچھلا_class;

		// Get class of current and previous char (if it exists).
		this_class = mb_get_class_buf(rex.داخلہ, rex.reg_buf);
		پچھلا_class = reg_پچھلا_class();
		اگر (this_class == پچھلا_class
			|| پچھلا_class == 0 || پچھلا_class == 1)
		    حالت_2 = RA_NOMATCH;
	    }
	    ورنہ
	    {
		اگر (!vim_iswordc_buf(rex.داخلہ[-1], rex.reg_buf)
			|| (rex.داخلہ[0] != NUL
					   && vim_iswordc_buf(c, rex.reg_buf)))
		    حالت_2 = RA_NOMATCH;
	    }
	    توڑو; // Matched with EOW

	  حالت ANY:
	    // ANY does not match new lines.
	    اگر (c == NUL)
		حالت_2 = RA_NOMATCH;
	    ورنہ
		ADVANCE_REGINPUT();
	    توڑو;

	  حالت IDENT:
	    اگر (!vim_isIDc(c))
		حالت_2 = RA_NOMATCH;
	    ورنہ
		ADVANCE_REGINPUT();
	    توڑو;

	  حالت SIDENT:
	    اگر (VIM_ISDIGIT(*rex.داخلہ) || !vim_isIDc(c))
		حالت_2 = RA_NOMATCH;
	    ورنہ
		ADVANCE_REGINPUT();
	    توڑو;

	  حالت KWORD:
	    اگر (!vim_iswordp_buf(rex.داخلہ, rex.reg_buf))
		حالت_2 = RA_NOMATCH;
	    ورنہ
		ADVANCE_REGINPUT();
	    توڑو;

	  حالت SKWORD:
	    اگر (VIM_ISDIGIT(*rex.داخلہ)
				    || !vim_iswordp_buf(rex.داخلہ, rex.reg_buf))
		حالت_2 = RA_NOMATCH;
	    ورنہ
		ADVANCE_REGINPUT();
	    توڑو;

	  حالت FNAME:
	    اگر (!vim_isfilec(c))
		حالت_2 = RA_NOMATCH;
	    ورنہ
		ADVANCE_REGINPUT();
	    توڑو;

	  حالت SFNAME:
	    اگر (VIM_ISDIGIT(*rex.داخلہ) || !vim_isfilec(c))
		حالت_2 = RA_NOMATCH;
	    ورنہ
		ADVANCE_REGINPUT();
	    توڑو;

	  حالت PRINT:
	    اگر (!vim_isprintc(PTR2CHAR(rex.داخلہ)))
		حالت_2 = RA_NOMATCH;
	    ورنہ
		ADVANCE_REGINPUT();
	    توڑو;

	  حالت SPRINT:
	    اگر (VIM_ISDIGIT(*rex.داخلہ) || !vim_isprintc(PTR2CHAR(rex.داخلہ)))
		حالت_2 = RA_NOMATCH;
	    ورنہ
		ADVANCE_REGINPUT();
	    توڑو;

	  حالت WHITE:
	    اگر (!VIM_ISWHITE(c))
		حالت_2 = RA_NOMATCH;
	    ورنہ
		ADVANCE_REGINPUT();
	    توڑو;

	  حالت NWHITE:
	    اگر (c == NUL || VIM_ISWHITE(c))
		حالت_2 = RA_NOMATCH;
	    ورنہ
		ADVANCE_REGINPUT();
	    توڑو;

	  حالت DIGIT:
	    اگر (!ri_digit(c))
		حالت_2 = RA_NOMATCH;
	    ورنہ
		ADVANCE_REGINPUT();
	    توڑو;

	  حالت NDIGIT:
	    اگر (c == NUL || ri_digit(c))
		حالت_2 = RA_NOMATCH;
	    ورنہ
		ADVANCE_REGINPUT();
	    توڑو;

	  حالت HEX:
	    اگر (!ri_hex(c))
		حالت_2 = RA_NOMATCH;
	    ورنہ
		ADVANCE_REGINPUT();
	    توڑو;

	  حالت NHEX:
	    اگر (c == NUL || ri_hex(c))
		حالت_2 = RA_NOMATCH;
	    ورنہ
		ADVANCE_REGINPUT();
	    توڑو;

	  حالت OCTAL:
	    اگر (!ri_octal(c))
		حالت_2 = RA_NOMATCH;
	    ورنہ
		ADVANCE_REGINPUT();
	    توڑو;

	  حالت NOCTAL:
	    اگر (c == NUL || ri_octal(c))
		حالت_2 = RA_NOMATCH;
	    ورنہ
		ADVANCE_REGINPUT();
	    توڑو;

	  حالت لفظ_2:
	    اگر (!ri_لفظ(c))
		حالت_2 = RA_NOMATCH;
	    ورنہ
		ADVANCE_REGINPUT();
	    توڑو;

	  حالت NWORD:
	    اگر (c == NUL || ri_لفظ(c))
		حالت_2 = RA_NOMATCH;
	    ورنہ
		ADVANCE_REGINPUT();
	    توڑو;

	  حالت سر_2:
	    اگر (!ri_سر(c))
		حالت_2 = RA_NOMATCH;
	    ورنہ
		ADVANCE_REGINPUT();
	    توڑو;

	  حالت NHEAD:
	    اگر (c == NUL || ri_سر(c))
		حالت_2 = RA_NOMATCH;
	    ورنہ
		ADVANCE_REGINPUT();
	    توڑو;

	  حالت ALPHA:
	    اگر (!ri_alpha(c))
		حالت_2 = RA_NOMATCH;
	    ورنہ
		ADVANCE_REGINPUT();
	    توڑو;

	  حالت NALPHA:
	    اگر (c == NUL || ri_alpha(c))
		حالت_2 = RA_NOMATCH;
	    ورنہ
		ADVANCE_REGINPUT();
	    توڑو;

	  حالت زیریں_2:
	    اگر (!ri_زیریں(c))
		حالت_2 = RA_NOMATCH;
	    ورنہ
		ADVANCE_REGINPUT();
	    توڑو;

	  حالت NLOWER:
	    اگر (c == NUL || ri_زیریں(c))
		حالت_2 = RA_NOMATCH;
	    ورنہ
		ADVANCE_REGINPUT();
	    توڑو;

	  حالت بالائی:
	    اگر (!ri_بالائی(c))
		حالت_2 = RA_NOMATCH;
	    ورنہ
		ADVANCE_REGINPUT();
	    توڑو;

	  حالت NUPPER:
	    اگر (c == NUL || ri_بالائی(c))
		حالت_2 = RA_NOMATCH;
	    ورنہ
		ADVANCE_REGINPUT();
	    توڑو;

	  حالت EXACTLY:
	    {
		صحیح	len;
		char_u	*opnd;

		opnd = عملیہ(جائزہ);
		// Inline the first byte, for speed.
		اگر (*opnd != *rex.داخلہ
			&& (!rex.reg_ic
			    || (!enc_utf8
			      && MB_TOLOWER(*opnd) != MB_TOLOWER(*rex.داخلہ))))
		    حالت_2 = RA_NOMATCH;
		ورنہ اگر (*opnd == NUL)
		{
		    // match empty string always works; happens when "~" is
		    // empty.
		}
		ورنہ
		{
		    اگر (opnd[1] == NUL && !(enc_utf8 && rex.reg_ic))
		    {
			len = 1;	// matched a single byte above
		    }
		    ورنہ
		    {
			// Need to match first byte again for multi-byte.
			len = (صحیح)STRLEN(opnd);
			اگر (cstrncmp(opnd, rex.داخلہ, &len) != 0)
			    حالت_2 = RA_NOMATCH;
		    }
		    // Check for following composing character, unless %C
		    // follows (skips over all composing chars).
		    اگر (حالت_2 != RA_NOMATCH
			    && enc_utf8
			    && UTF_COMPOSINGLIKE(rex.داخلہ, rex.داخلہ + len)
			    && !rex.reg_icombine
			    && OP(اگلا) != RE_COMPOSING)
		    {
			// raaron: This code makes a composing character get
			// ignored, which is the correct behavior (sometimes)
			// for voweled Hebrew texts.
			حالت_2 = RA_NOMATCH;
		    }
		    اگر (حالت_2 != RA_NOMATCH)
			rex.داخلہ += len;
		}
	    }
	    توڑو;

	  حالت ANYOF:
	  حالت ANYBUT:
	    {
		char_u  *q = عملیہ(جائزہ);

		اگر (c == NUL)
		    حالت_2 = RA_NOMATCH;
		ورنہ اگر ((cstrchr(q, c) == تہی) == (op == ANYOF))
		    حالت_2 = RA_NOMATCH;
		ورنہ
		{
		    // Check following combining characters
		    صحیح	len = 0;
		    صحیح i;

		    اگر (enc_utf8)
			len = utfc_ptr2len(q) - utf_ptr2len(q);

		    MB_CPTR_ADV(rex.داخلہ);
		    MB_CPTR_ADV(q);

		    اگر (!enc_utf8 || len == 0)
			توڑو;

		    برائے (i = 0; i < len; ++i)
			اگر (q[i] != rex.داخلہ[i])
			{
			    حالت_2 = RA_NOMATCH;
			    توڑو;
			}
		    rex.داخلہ += len;
		}
		توڑو;
	    }

	  حالت MULTIBYTECODE:
	    اگر (has_mbyte)
	    {
		صحیح	i, len;
		char_u	*opnd;
		صحیح	opndc = 0, inpc;

		opnd = عملیہ(جائزہ);
		// Safety check (just in case 'encoding' was changed since
		// compiling the program).
		اگر ((len = (*mb_ptr2len)(opnd)) < 2)
		{
		    حالت_2 = RA_NOMATCH;
		    توڑو;
		}
		اگر (enc_utf8)
		    opndc = utf_ptr2char(opnd);
		اگر (enc_utf8 && utf_iscomposing(opndc))
		{
		    // When only a composing char is given match at any
		    // position where that composing char appears.
		    حالت_2 = RA_NOMATCH;
		    برائے (i = 0; rex.داخلہ[i] != NUL;
						i += utf_ptr2len(rex.داخلہ + i))
		    {
			inpc = utf_ptr2char(rex.داخلہ + i);
			اگر (!utf_iscomposing(inpc))
			{
			    اگر (i > 0)
				توڑو;
			}
			ورنہ اگر (opndc == inpc)
			{
			    // Include all following composing chars.
			    len = i + utfc_ptr2len(rex.داخلہ + i);
			    حالت_2 = RA_مطابقت;
			    توڑو;
			}
		    }
		}
		ورنہ اگر (enc_utf8)
		{
		    اگر (cstrncmp(opnd, rex.داخلہ, &len) != 0)
		    {
			حالت_2 = RA_NOMATCH;
			توڑو;
		    }
		}
		ورنہ
		    برائے (i = 0; i < len; ++i)
			اگر (opnd[i] != rex.داخلہ[i])
			{
			    حالت_2 = RA_NOMATCH;
			    توڑو;
			}
		rex.داخلہ += len;
	    }
	    ورنہ
		حالت_2 = RA_NOMATCH;
	    توڑو;
	  حالت RE_COMPOSING:
	    اگر (enc_utf8)
	    {
		// Skip composing characters.
		جب (utf_iscomposing(utf_ptr2char(rex.داخلہ)))
		    MB_CPTR_ADV(rex.داخلہ);
	    }
	    توڑو;

	  حالت NOTHING:
	    توڑو;

	  حالت عقب_2:
	    {
		صحیح		i;
		backpos_T	*bp;

		// When we run into BACK we need to check if we don't keep
		// looping without matching any input.  The second and later
		// times a BACK is encountered it fails if the input is still
		// at the same position as the previous time.
		// The positions are stored in "backpos" and found by the
		// current value of "scan", the position in the RE program.
		bp = (backpos_T *)backpos.ga_ڈیٹا;
		برائے (i = 0; i < backpos.ga_len; ++i)
		    اگر (bp[i].bp_جائزہ == جائزہ)
			توڑو;
		اگر (i == backpos.ga_len)
		{
		    // First time at this BACK, make room to store the pos.
		    اگر (GA_GROW_FAILS(&backpos, 1))
			حالت_2 = RA_FAIL;
		    ورنہ
		    {
			// get "ga_data" again, it may have changed
			bp = (backpos_T *)backpos.ga_ڈیٹا;
			bp[i].bp_جائزہ = جائزہ;
			++backpos.ga_len;
		    }
		}
		ورنہ اگر (reg_save_equal(&bp[i].bp_pos))
		    // Still at same position as last time, fail.
		    حالت_2 = RA_NOMATCH;

		اگر (حالت_2 != RA_FAIL && حالت_2 != RA_NOMATCH)
		    reg_save(&bp[i].bp_pos, &backpos);
	    }
	    توڑو;

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
		rp = regstack_ڈالو(RS_MOPEN, جائزہ);
		اگر (rp == تہی)
		    حالت_2 = RA_FAIL;
		ورنہ
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_startpos[no],
							  &rex.reg_startp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    توڑو;

	  حالت NOPEN:	    // \%(
	  حالت NCLOSE:	    // \) after \%(
		اگر (regstack_ڈالو(RS_NOPEN, جائزہ) == تہی)
		    حالت_2 = RA_FAIL;
		// We simply continue and handle the result when done.
		توڑو;

#اگر_تعریف_شدہ FEAT_SYN_HL
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
		rp = regstack_ڈالو(RS_ZOPEN, جائزہ);
		اگر (rp == تہی)
		    حالت_2 = RA_FAIL;
		ورنہ
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_startzpos[no],
							     &reg_startzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    توڑو;
#اگر_ختم

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
		rp = regstack_ڈالو(RS_MCLOSE, جائزہ);
		اگر (rp == تہی)
		    حالت_2 = RA_FAIL;
		ورنہ
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &rex.reg_endpos[no],
							    &rex.reg_endp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    توڑو;

#اگر_تعریف_شدہ FEAT_SYN_HL
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
		rp = regstack_ڈالو(RS_ZCLOSE, جائزہ);
		اگر (rp == تہی)
		    حالت_2 = RA_FAIL;
		ورنہ
		{
		    rp->rs_no = no;
		    save_se(&rp->rs_un.sesave, &reg_endzpos[no],
							      &reg_endzp[no]);
		    // We simply continue and handle the result when done.
		}
	    }
	    توڑو;
#اگر_ختم

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
		    اگر (rex.reg_startp[no] == تہی || rex.reg_endp[no] == تہی)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    ورنہ
		    {
			// Compare current input with back-ref in the same
			// line.
			len = (صحیح)(rex.reg_endp[no] - rex.reg_startp[no]);
			اگر (cstrncmp(rex.reg_startp[no], rex.داخلہ, &len) != 0)
			    حالت_2 = RA_NOMATCH;
		    }
		}
		ورنہ				// Multi-line regexp
		{
		    اگر (rex.reg_startpos[no].lnum < 0
						|| rex.reg_endpos[no].lnum < 0)
		    {
			// Backref was not set: Match an empty string.
			len = 0;
		    }
		    ورنہ
		    {
			اگر (rex.reg_startpos[no].lnum == rex.lnum
				&& rex.reg_endpos[no].lnum == rex.lnum)
			{
			    // Compare back-ref within the current line.
			    len = rex.reg_endpos[no].ستون
						    - rex.reg_startpos[no].ستون;
			    اگر (cstrncmp(rex.سطر + rex.reg_startpos[no].ستون,
							  rex.داخلہ, &len) != 0)
				حالت_2 = RA_NOMATCH;
			}
			ورنہ
			{
			    // Messy situation: Need to compare between two
			    // lines.
			    صحیح r = مطابقت_with_backref(
					    rex.reg_startpos[no].lnum,
					    rex.reg_startpos[no].ستون,
					    rex.reg_endpos[no].lnum,
					    rex.reg_endpos[no].ستون,
					    &len);

			    اگر (r != RA_مطابقت)
				حالت_2 = r;
			}
		    }
		}

		// Matched the backref, skip over it.
		rex.داخلہ += len;
	    }
	    توڑو;

#اگر_تعریف_شدہ FEAT_SYN_HL
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
		اگر (re_extmatch_اندر != تہی
			&& re_extmatch_اندر->matches[no] != تہی)
		{
		    len = (صحیح)STRLEN(re_extmatch_اندر->matches[no]);
		    اگر (cstrncmp(re_extmatch_اندر->matches[no],
							  rex.داخلہ, &len) != 0)
			حالت_2 = RA_NOMATCH;
		    ورنہ
			rex.داخلہ += len;
		}
		ورنہ
		{
		    // Backref was not set: Match an empty string.
		}
	    }
	    توڑو;
#اگر_ختم

	  حالت BRANCH:
	    {
		اگر (OP(اگلا) != BRANCH) // No choice.
		    اگلا = عملیہ(جائزہ);	// Avoid recursion.
		ورنہ
		{
		    rp = regstack_ڈالو(RS_BRANCH, جائزہ);
		    اگر (rp == تہی)
			حالت_2 = RA_FAIL;
		    ورنہ
			حالت_2 = RA_BREAK;	// rest is below
		}
	    }
	    توڑو;

	  حالت BRACE_LIMITS:
	    {
		اگر (OP(اگلا) == BRACE_SIMPLE)
		{
		    bl_minval = عملیہ_MIN(جائزہ);
		    bl_maxval = عملیہ_MAX(جائزہ);
		}
		ورنہ اگر (OP(اگلا) >= BRACE_COMPLEX
			&& OP(اگلا) < BRACE_COMPLEX + 10)
		{
		    no = OP(اگلا) - BRACE_COMPLEX;
		    brace_min[no] = عملیہ_MIN(جائزہ);
		    brace_max[no] = عملیہ_MAX(جائزہ);
		    brace_گنتی[no] = 0;
		}
		ورنہ
		{
		    اندرونی_error("BRACE_LIMITS");
		    حالت_2 = RA_FAIL;
		}
	    }
	    توڑو;

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
		++brace_گنتی[no];

		// If not matched enough times yet, try one more
		اگر (brace_گنتی[no] <= (brace_min[no] <= brace_max[no]
					     ? brace_min[no] : brace_max[no]))
		{
		    rp = regstack_ڈالو(RS_BRCPLX_مزید, جائزہ);
		    اگر (rp == تہی)
			حالت_2 = RA_FAIL;
		    ورنہ
		    {
			rp->rs_no = no;
			reg_save(&rp->rs_un.regsave, &backpos);
			اگلا = عملیہ(جائزہ);
			// We continue and handle the result when done.
		    }
		    توڑو;
		}

		// If matched enough times, may try matching some more
		اگر (brace_min[no] <= brace_max[no])
		{
		    // Range is the normal way around, use longest match
		    اگر (brace_گنتی[no] <= brace_max[no])
		    {
			rp = regstack_ڈالو(RS_BRCPLX_LONG, جائزہ);
			اگر (rp == تہی)
			    حالت_2 = RA_FAIL;
			ورنہ
			{
			    rp->rs_no = no;
			    reg_save(&rp->rs_un.regsave, &backpos);
			    اگلا = عملیہ(جائزہ);
			    // We continue and handle the result when done.
			}
		    }
		}
		ورنہ
		{
		    // Range is backwards, use shortest match first
		    اگر (brace_گنتی[no] <= brace_min[no])
		    {
			rp = regstack_ڈالو(RS_BRCPLX_SHORT, جائزہ);
			اگر (rp == تہی)
			    حالت_2 = RA_FAIL;
			ورنہ
			{
			    reg_save(&rp->rs_un.regsave, &backpos);
			    // We continue and handle the result when done.
			}
		    }
		}
	    }
	    توڑو;

	  حالت BRACE_SIMPLE:
	  حالت ستارہ:
	  حالت PLUS:
	    {
		regstar_T	rst;

		// Lookahead to avoid useless match attempts when we know
		// what character comes next.
		اگر (OP(اگلا) == EXACTLY)
		{
		    rst.nextb = *عملیہ(اگلا);
		    اگر (rex.reg_ic)
		    {
			اگر (MB_ISUPPER(rst.nextb))
			    rst.nextb_ic = MB_TOLOWER(rst.nextb);
			ورنہ
			    rst.nextb_ic = MB_TOUPPER(rst.nextb);
		    }
		    ورنہ
			rst.nextb_ic = rst.nextb;
		}
		ورنہ
		{
		    rst.nextb = NUL;
		    rst.nextb_ic = NUL;
		}
		اگر (op != BRACE_SIMPLE)
		{
		    rst.minval = (op == ستارہ) ? 0 : 1;
		    rst.maxval = MAX_حد_بندی;
		}
		ورنہ
		{
		    rst.minval = bl_minval;
		    rst.maxval = bl_maxval;
		}

		// When maxval > minval, try matching as much as possible, up
		// to maxval.  When maxval < minval, try matching at least the
		// minimal number (since the range is backwards, that's also
		// maxval!).
		rst.گنتی = regrepeat(عملیہ(جائزہ), rst.maxval);
		اگر (got_int)
		{
		    حالت_2 = RA_FAIL;
		    توڑو;
		}
		اگر (rst.minval <= rst.maxval
			  ? rst.گنتی >= rst.minval : rst.گنتی >= rst.maxval)
		{
		    // It could match.  Prepare for trying to match what
		    // follows.  The code is below.  Parameters are stored in
		    // a regstar_T on the regstack.
		    اگر ((طویل)((غیرعلامتی)regstack.ga_len >> 10) >= p_mmp)
		    {
			emsg(_(e_نمونہ_استعمال_کرتا_مزید_حافظہ_than_maxmempattern));
			حالت_2 = RA_FAIL;
		    }
		    ورنہ اگر (GA_GROW_FAILS(&regstack, (صحیح)حجم(regstar_T)))
			حالت_2 = RA_FAIL;
		    ورنہ
		    {
			regstack.ga_len += حجم(regstar_T);
			rp = regstack_ڈالو(rst.minval <= rst.maxval
					? RS_ستارہ_LONG : RS_ستارہ_SHORT, جائزہ);
			اگر (rp == تہی)
			    حالت_2 = RA_FAIL;
			ورنہ
			{
			    *(((regstar_T *)rp) - 1) = rst;
			    حالت_2 = RA_BREAK;	    // skip the restore bits
			}
		    }
		}
		ورنہ
		    حالت_2 = RA_NOMATCH;

	    }
	    توڑو;

	  حالت NOMATCH:
	  حالت مطابقت_2:
	  حالت SUBPAT:
	    rp = regstack_ڈالو(RS_NOMATCH, جائزہ);
	    اگر (rp == تہی)
		حالت_2 = RA_FAIL;
	    ورنہ
	    {
		rp->rs_no = op;
		reg_save(&rp->rs_un.regsave, &backpos);
		اگلا = عملیہ(جائزہ);
		// We continue and handle the result when done.
	    }
	    توڑو;

	  حالت BEHIND:
	  حالت NOBEHIND:
	    // Need a bit of room to store extra positions.
	    اگر ((طویل)((غیرعلامتی)regstack.ga_len >> 10) >= p_mmp)
	    {
		emsg(_(e_نمونہ_استعمال_کرتا_مزید_حافظہ_than_maxmempattern));
		حالت_2 = RA_FAIL;
	    }
	    ورنہ اگر (GA_GROW_FAILS(&regstack, (صحیح)حجم(regbehind_T)))
		حالت_2 = RA_FAIL;
	    ورنہ
	    {
		regstack.ga_len += حجم(regbehind_T);
		rp = regstack_ڈالو(RS_BEHIND1, جائزہ);
		اگر (rp == تہی)
		    حالت_2 = RA_FAIL;
		ورنہ
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
	    توڑو;

	  حالت BHPOS:
	    اگر (REG_MULTI)
	    {
		اگر (behind_pos.rs_u.pos.ستون != (colnr_T)(rex.داخلہ - rex.سطر)
			|| behind_pos.rs_u.pos.lnum != rex.lnum)
		    حالت_2 = RA_NOMATCH;
	    }
	    ورنہ اگر (behind_pos.rs_u.ptr != rex.داخلہ)
		حالت_2 = RA_NOMATCH;
	    توڑو;

	  حالت NEWL:
	    اگر ((c != NUL || !REG_MULTI || rex.lnum > rex.reg_maxline
			     || rex.reg_سطر_lbr)
					   && (c != '\n' || !rex.reg_سطر_lbr))
		حالت_2 = RA_NOMATCH;
	    ورنہ اگر (rex.reg_سطر_lbr)
		ADVANCE_REGINPUT();
	    ورنہ
		reg_nextline();
	    توڑو;

	  حالت انتہا_3:
	    حالت_2 = RA_مطابقت;	// Success!
	    توڑو;

	  طےشدہ:
	    iemsg(e_corrupted_regexp_پروگرام);
#اگر_تعریف_شدہ DEBUG
	    لکھو("Illegal op code %d\n", op);
#اگر_ختم
	    حالت_2 = RA_FAIL;
	    توڑو;
	  }
	}

	// If we can't continue sequentially, break the inner loop.
	اگر (حالت_2 != RA_CONT)
	    توڑو;

	// Continue in inner loop, advance to next item.
	جائزہ = اگلا;

    } // end of inner loop

    // If there is something on the regstack execute the code for the state.
    // If the state is popped then loop and use the older state.
    جب (regstack.ga_len > 0 && حالت_2 != RA_FAIL)
    {
	rp = (regitem_T *)((حرف *)regstack.ga_ڈیٹا + regstack.ga_len) - 1;
	انتخاب (rp->rs_state)
	{
	  حالت RS_NOPEN:
	    // Result is passed on as-is, simply pop the state.
	    regstack_نکالو(&جائزہ);
	    توڑو;

	  حالت RS_MOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    اگر (حالت_2 == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_startpos[rp->rs_no],
						  &rex.reg_startp[rp->rs_no]);
	    regstack_نکالو(&جائزہ);
	    توڑو;

#اگر_تعریف_شدہ FEAT_SYN_HL
	  حالت RS_ZOPEN:
	    // Pop the state.  Restore pointers when there is no match.
	    اگر (حالت_2 == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_startzpos[rp->rs_no],
						 &reg_startzp[rp->rs_no]);
	    regstack_نکالو(&جائزہ);
	    توڑو;
#اگر_ختم

	  حالت RS_MCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    اگر (حالت_2 == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &rex.reg_endpos[rp->rs_no],
						    &rex.reg_endp[rp->rs_no]);
	    regstack_نکالو(&جائزہ);
	    توڑو;

#اگر_تعریف_شدہ FEAT_SYN_HL
	  حالت RS_ZCLOSE:
	    // Pop the state.  Restore pointers when there is no match.
	    اگر (حالت_2 == RA_NOMATCH)
		restore_se(&rp->rs_un.sesave, &reg_endzpos[rp->rs_no],
						   &reg_endzp[rp->rs_no]);
	    regstack_نکالو(&جائزہ);
	    توڑو;
#اگر_ختم

	  حالت RS_BRANCH:
	    اگر (حالت_2 == RA_مطابقت)
		// this branch matched, use it
		regstack_نکالو(&جائزہ);
	    ورنہ
	    {
		اگر (حالت_2 != RA_BREAK)
		{
		    // After a non-matching branch: try next one.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    جائزہ = rp->rs_جائزہ;
		}
		اگر (جائزہ == تہی || OP(جائزہ) != BRANCH)
		{
		    // no more branches, didn't find a match
		    حالت_2 = RA_NOMATCH;
		    regstack_نکالو(&جائزہ);
		}
		ورنہ
		{
		    // Prepare to try a branch.
		    rp->rs_جائزہ = regnext(جائزہ);
		    reg_save(&rp->rs_un.regsave, &backpos);
		    جائزہ = عملیہ(جائزہ);
		}
	    }
	    توڑو;

	  حالت RS_BRCPLX_مزید:
	    // Pop the state.  Restore pointers when there is no match.
	    اگر (حالت_2 == RA_NOMATCH)
	    {
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_گنتی[rp->rs_no];	// decrement match count
	    }
	    regstack_نکالو(&جائزہ);
	    توڑو;

	  حالت RS_BRCPLX_LONG:
	    // Pop the state.  Restore pointers when there is no match.
	    اگر (حالت_2 == RA_NOMATCH)
	    {
		// There was no match, but we did find enough matches.
		reg_restore(&rp->rs_un.regsave, &backpos);
		--brace_گنتی[rp->rs_no];
		// continue with the items after "\{}"
		حالت_2 = RA_CONT;
	    }
	    regstack_نکالو(&جائزہ);
	    اگر (حالت_2 == RA_CONT)
		جائزہ = regnext(جائزہ);
	    توڑو;

	  حالت RS_BRCPLX_SHORT:
	    // Pop the state.  Restore pointers when there is no match.
	    اگر (حالت_2 == RA_NOMATCH)
		// There was no match, try to match one more item.
		reg_restore(&rp->rs_un.regsave, &backpos);
	    regstack_نکالو(&جائزہ);
	    اگر (حالت_2 == RA_NOMATCH)
	    {
		جائزہ = عملیہ(جائزہ);
		حالت_2 = RA_CONT;
	    }
	    توڑو;

	  حالت RS_NOMATCH:
	    // Pop the state.  If the operand matches for NOMATCH or
	    // doesn't match for MATCH/SUBPAT, we fail.  Otherwise backup,
	    // except for SUBPAT, and continue with the next item.
	    اگر (حالت_2 == (rp->rs_no == NOMATCH ? RA_مطابقت : RA_NOMATCH))
		حالت_2 = RA_NOMATCH;
	    ورنہ
	    {
		حالت_2 = RA_CONT;
		اگر (rp->rs_no != SUBPAT)	// zero-width
		    reg_restore(&rp->rs_un.regsave, &backpos);
	    }
	    regstack_نکالو(&جائزہ);
	    اگر (حالت_2 == RA_CONT)
		جائزہ = regnext(جائزہ);
	    توڑو;

	  حالت RS_BEHIND1:
	    اگر (حالت_2 == RA_NOMATCH)
	    {
		regstack_نکالو(&جائزہ);
		regstack.ga_len -= حجم(regbehind_T);
	    }
	    ورنہ
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
		جائزہ = عملیہ(rp->rs_جائزہ) + 4;
	    }
	    توڑو;

	  حالت RS_BEHIND2:
	    // Looping for BEHIND / NOBEHIND match.
	    اگر (حالت_2 == RA_مطابقت && reg_save_equal(&behind_pos))
	    {
		// found a match that ends where "next" started
		behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		اگر (rp->rs_no == BEHIND)
		    reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
		ورنہ
		{
		    // But we didn't want a match.  Need to restore the
		    // subexpr, because what follows matched, so they have
		    // been set.
		    حالت_2 = RA_NOMATCH;
		    restore_subexpr(((regbehind_T *)rp) - 1);
		}
		regstack_نکالو(&جائزہ);
		regstack.ga_len -= حجم(regbehind_T);
	    }
	    ورنہ
	    {
		طویل حد_بندی;

		// No match or a match that doesn't end where we want it: Go
		// back one character.  May go to previous line once.
		no = OK;
		حد_بندی = عملیہ_MIN(rp->rs_جائزہ);
		اگر (REG_MULTI)
		{
		    اگر (حد_بندی > 0
			    && ((rp->rs_un.regsave.rs_u.pos.lnum
						    < behind_pos.rs_u.pos.lnum
				    ? (colnr_T)STRLEN(rex.سطر)
				    : behind_pos.rs_u.pos.ستون)
				- rp->rs_un.regsave.rs_u.pos.ستون >= حد_بندی))
			no = FAIL;
		    ورنہ اگر (rp->rs_un.regsave.rs_u.pos.ستون == 0)
		    {
			اگر (rp->rs_un.regsave.rs_u.pos.lnum
					< behind_pos.rs_u.pos.lnum
				|| reg_getline(
					--rp->rs_un.regsave.rs_u.pos.lnum)
								  == تہی)
			    no = FAIL;
			ورنہ
			{
			    reg_restore(&rp->rs_un.regsave, &backpos);
			    rp->rs_un.regsave.rs_u.pos.ستون =
						 (colnr_T)STRLEN(rex.سطر);
			}
		    }
		    ورنہ
		    {
			اگر (has_mbyte)
			{
			    char_u *سطر =
				  reg_getline(rp->rs_un.regsave.rs_u.pos.lnum);

			    rp->rs_un.regsave.rs_u.pos.ستون -=
				(*mb_سر_off)(سطر, سطر
				    + rp->rs_un.regsave.rs_u.pos.ستون - 1) + 1;
			}
			ورنہ
			    --rp->rs_un.regsave.rs_u.pos.ستون;
		    }
		}
		ورنہ
		{
		    اگر (rp->rs_un.regsave.rs_u.ptr == rex.سطر)
			no = FAIL;
		    ورنہ
		    {
			MB_PTR_عقب(rex.سطر, rp->rs_un.regsave.rs_u.ptr);
			اگر (حد_بندی > 0 && (طویل)(behind_pos.rs_u.ptr
				     - rp->rs_un.regsave.rs_u.ptr) > حد_بندی)
			    no = FAIL;
		    }
		}
		اگر (no == OK)
		{
		    // Advanced, prepare for finding match again.
		    reg_restore(&rp->rs_un.regsave, &backpos);
		    جائزہ = عملیہ(rp->rs_جائزہ) + 4;
		    اگر (حالت_2 == RA_مطابقت)
		    {
			// We did match, so subexpr may have been changed,
			// need to restore them for the next try.
			حالت_2 = RA_NOMATCH;
			restore_subexpr(((regbehind_T *)rp) - 1);
		    }
		}
		ورنہ
		{
		    // Can't advance.  For NOBEHIND that's a match.
		    behind_pos = (((regbehind_T *)rp) - 1)->save_behind;
		    اگر (rp->rs_no == NOBEHIND)
		    {
			reg_restore(&(((regbehind_T *)rp) - 1)->save_after,
								    &backpos);
			حالت_2 = RA_مطابقت;
		    }
		    ورنہ
		    {
			// We do want a proper match.  Need to restore the
			// subexpr if we had a match, because they may have
			// been set.
			اگر (حالت_2 == RA_مطابقت)
			{
			    حالت_2 = RA_NOMATCH;
			    restore_subexpr(((regbehind_T *)rp) - 1);
			}
		    }
		    regstack_نکالو(&جائزہ);
		    regstack.ga_len -= حجم(regbehind_T);
		}
	    }
	    توڑو;

	  حالت RS_ستارہ_LONG:
	  حالت RS_ستارہ_SHORT:
	    {
		regstar_T	    *rst = ((regstar_T *)rp) - 1;

		اگر (حالت_2 == RA_مطابقت)
		{
		    regstack_نکالو(&جائزہ);
		    regstack.ga_len -= حجم(regstar_T);
		    توڑو;
		}

		// Tried once already, restore input pointers.
		اگر (حالت_2 != RA_BREAK)
		    reg_restore(&rp->rs_un.regsave, &backpos);

		// Repeat until we found a position where it could match.
		برائے (;;)
		{
		    اگر (حالت_2 != RA_BREAK)
		    {
			// Tried first position already, advance.
			اگر (rp->rs_state == RS_ستارہ_LONG)
			{
			    // Trying for longest match, but couldn't or
			    // didn't match -- back up one char.
			    اگر (--rst->گنتی < rst->minval)
				توڑو;
			    اگر (rex.داخلہ == rex.سطر)
			    {
				// backup to last char of previous line
				اگر (rex.lnum == 0)
				{
				    حالت_2 = RA_NOMATCH;
				    توڑو;
				}
				--rex.lnum;
				rex.سطر = reg_getline(rex.lnum);
				// Just in case regrepeat() didn't count
				// right.
				اگر (rex.سطر == تہی)
				    توڑو;
				rex.داخلہ = rex.سطر + reg_getline_len(rex.lnum);
				fast_breakcheck();
			    }
			    ورنہ
				MB_PTR_عقب(rex.سطر, rex.داخلہ);
			}
			ورنہ
			{
			    // Range is backwards, use shortest match first.
			    // Careful: maxval and minval are exchanged!
			    // Couldn't or didn't match: try advancing one
			    // char.
			    اگر (rst->گنتی == rst->minval
				  || regrepeat(عملیہ(rp->rs_جائزہ), 1L) == 0)
				توڑو;
			    ++rst->گنتی;
			}
			اگر (got_int)
			    توڑو;
		    }
		    ورنہ
			حالت_2 = RA_NOMATCH;

		    // If it could match, try it.
		    اگر (rst->nextb == NUL || *rex.داخلہ == rst->nextb
					     || *rex.داخلہ == rst->nextb_ic)
		    {
			reg_save(&rp->rs_un.regsave, &backpos);
			جائزہ = regnext(rp->rs_جائزہ);
			حالت_2 = RA_CONT;
			توڑو;
		    }
		}
		اگر (حالت_2 != RA_CONT)
		{
		    // Failed.
		    regstack_نکالو(&جائزہ);
		    regstack.ga_len -= حجم(regstar_T);
		    حالت_2 = RA_NOMATCH;
		}
	    }
	    توڑو;
	}

	// If we want to continue the inner loop or didn't pop a state
	// continue matching loop
	اگر (حالت_2 == RA_CONT || rp == (regitem_T *)
			     ((حرف *)regstack.ga_ڈیٹا + regstack.ga_len) - 1)
	    توڑو;

#اگر_تعریف_شدہ FEAT_RELTIME
	اگر (bt_did_وقت_برآمد(timed_برآمد))
	{
	    حالت_2 = RA_FAIL;
	    توڑو;
	}
#اگر_ختم
    }

    // May need to continue with the inner loop, starting at "scan".
    اگر (حالت_2 == RA_CONT)
	جاری;

    // If the regstack is empty or something failed we are done.
    اگر (regstack.ga_len == 0 || حالت_2 == RA_FAIL)
    {
	اگر (جائزہ == تہی)
	{
	    // We get here only if there's trouble -- normally "case END" is
	    // the terminating point.
	    iemsg(e_corrupted_regexp_پروگرام);
#اگر_تعریف_شدہ DEBUG
	    لکھو("Premature EOL\n");
#اگر_ختم
	}
	واپس (حالت_2 == RA_مطابقت);
    }

  } // End of loop until the regstack is empty.

  // NOTREACHED
}

/*
 * regtry - try match of "prog" with at rex.line["col"].
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    ساکن طویل
regtry(
    bt_regprog_T	*prog,
    colnr_T		ستون,
    صحیح			*timed_برآمد)	// flag set on timeout or NULL
{
    rex.داخلہ = rex.سطر + ستون;
    rex.need_clear_subexpr = TRUE;
#اگر_تعریف_شدہ FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    rex.need_clear_zsubexpr = (prog->reghasz == REX_سیٹ);
#اگر_ختم

    اگر (regmatch(prog->پروگرام + 1, timed_برآمد) == 0)
	واپس 0;

    cleanup_subexpr();
    اگر (REG_MULTI)
    {
	اگر (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].ستون = ستون;
	}
	اگر (rex.reg_endpos[0].lnum < 0)
	{
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].ستون = (صحیح)(rex.داخلہ - rex.سطر);
	}
	ورنہ
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    ورنہ
    {
	اگر (rex.reg_startp[0] == تہی)
	    rex.reg_startp[0] = rex.سطر + ستون;
	اگر (rex.reg_endp[0] == تہی)
	    rex.reg_endp[0] = rex.داخلہ;
    }
#اگر_تعریف_شدہ FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_برآمد);
    re_extmatch_برآمد = تہی;

    اگر (prog->reghasz == REX_سیٹ)
    {
	صحیح		i;

	cleanup_zsubexpr();
	re_extmatch_برآمد = make_extmatch();
	اگر (re_extmatch_برآمد == تہی)
	    واپس 0;
	برائے (i = 0; i < NSUBEXP; i++)
	{
	    اگر (REG_MULTI)
	    {
		// Only accept single line matches.
		اگر (reg_startzpos[i].lnum >= 0
			&& reg_endzpos[i].lnum == reg_startzpos[i].lnum
			&& reg_endzpos[i].ستون >= reg_startzpos[i].ستون)
		    re_extmatch_برآمد->matches[i] =
			vim_strnsave(reg_getline(reg_startzpos[i].lnum)
						       + reg_startzpos[i].ستون,
				   reg_endzpos[i].ستون - reg_startzpos[i].ستون);
	    }
	    ورنہ
	    {
		اگر (reg_startzp[i] != تہی && reg_endzp[i] != تہی)
		    re_extmatch_برآمد->matches[i] =
			    vim_strnsave(reg_startzp[i],
						reg_endzp[i] - reg_startzp[i]);
	    }
	}
    }
#اگر_ختم
    واپس 1 + rex.lnum;
}

/*
 * Match a regexp against a string ("line" points to the string) or multiple
 * lines (if "line" is NULL, use reg_getline()).
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    ساکن طویل
bt_regexec_both(
    char_u	*سطر,
    colnr_T	startcol,	// column to start looking for match
    صحیح		*timed_برآمد)	// flag set on timeout or NULL
{
    bt_regprog_T    *prog;
    char_u	    *s;
    colnr_T	    ستون = startcol;
    طویل	    retval = 0L;

    // Create "regstack" and "backpos" if they are not allocated yet.
    // We allocate *_INITIAL amount of bytes first and then set the grow size
    // to much bigger value to avoid many malloc calls in case of deep regular
    // expressions.
    اگر (regstack.ga_ڈیٹا == تہی)
    {
	// Use an item size of 1 byte, since we push different things
	// onto the regstack.
	ga_init2(&regstack, 1, REGSTACK_INITIAL);
	(خالی)ga_grow(&regstack, REGSTACK_INITIAL);
	regstack.ga_growsize = REGSTACK_INITIAL * 8;
    }

    اگر (backpos.ga_ڈیٹا == تہی)
    {
	ga_init2(&backpos, حجم(backpos_T), BACKPOS_INITIAL);
	(خالی)ga_grow(&backpos, BACKPOS_INITIAL);
	backpos.ga_growsize = BACKPOS_INITIAL * 8;
    }

    اگر (REG_MULTI)
    {
	prog = (bt_regprog_T *)rex.reg_mmatch->regprog;
	سطر = reg_getline((linenr_T)0);
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    ورنہ
    {
	prog = (bt_regprog_T *)rex.reg_مطابقت->regprog;
	rex.reg_startp = rex.reg_مطابقت->startp;
	rex.reg_endp = rex.reg_مطابقت->endp;
    }

    // Be paranoid...
    اگر (prog == تہی || سطر == تہی)
    {
	iemsg(e_null_دلیل);
	جاؤ theend;
    }

    // Check validity of program.
    اگر (prog_magic_wrong())
	جاؤ theend;

    // If the start column is past the maximum column: no need to try.
    اگر (rex.reg_maxcol > 0 && ستون >= rex.reg_maxcol)
	جاؤ theend;

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    اگر (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    ورنہ اگر (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    اگر (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    // If there is a "must appear" string, look for it.
    اگر (prog->regmust != تہی)
    {
	صحیح c;

	اگر (has_mbyte)
	    c = (*mb_ptr2char)(prog->regmust);
	ورنہ
	    c = *prog->regmust;
	s = سطر + ستون;

	// This is used very often, esp. for ":global".  Use three versions of
	// the loop to avoid overhead of conditions.
	اگر (!rex.reg_ic && !has_mbyte)
	    جب ((s = vim_strbyte(s, c)) != تہی)
	    {
		صحیح len = prog->regmlen;
		اگر (cstrncmp(s, prog->regmust, &len) == 0)
		    توڑو;		// Found it.
		++s;
	    }
	ورنہ اگر (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	    جب ((s = vim_strchr(s, c)) != تہی)
	    {
		صحیح len = prog->regmlen;
		اگر (cstrncmp(s, prog->regmust, &len) == 0)
		    توڑو;		// Found it.
		MB_PTR_ADV(s);
	    }
	ورنہ
	    جب ((s = cstrchr(s, c)) != تہی)
	    {
		صحیح len = prog->regmlen;
		اگر (cstrncmp(s, prog->regmust, &len) == 0)
		    توڑو;		// Found it.
		MB_PTR_ADV(s);
	    }
	اگر (s == تہی)		// Not present.
	    جاؤ theend;
    }

    rex.سطر = سطر;
    rex.lnum = 0;
    reg_toolong = FALSE;

    // Simplest case: Anchored match need be tried only once.
    اگر (prog->reganch)
    {
	صحیح	c;

	اگر (has_mbyte)
	    c = (*mb_ptr2char)(rex.سطر + ستون);
	ورنہ
	    c = rex.سطر[ستون];
	اگر (prog->regstart == NUL
		|| prog->regstart == c
		|| (rex.reg_ic
		    && (((enc_utf8 && utf_fold(prog->regstart) == utf_fold(c)))
			|| (c < 255 && prog->regstart < 255 &&
			    MB_TOLOWER(prog->regstart) == MB_TOLOWER(c)))))
	    retval = regtry(prog, ستون, timed_برآمد);
	ورنہ
	    retval = 0;
    }
    ورنہ
    {
	// Messy cases:  unanchored match.
	جب (!got_int)
	{
	    اگر (prog->regstart != NUL)
	    {
		// Skip until the char we know it must start with.
		// Used often, do some work to avoid call overhead.
		اگر (!rex.reg_ic && !has_mbyte)
		    s = vim_strbyte(rex.سطر + ستون, prog->regstart);
		ورنہ
		    s = cstrchr(rex.سطر + ستون, prog->regstart);
		اگر (s == تہی)
		{
		    retval = 0;
		    توڑو;
		}
		ستون = (صحیح)(s - rex.سطر);
	    }

	    // Check for maximum column to try.
	    اگر (rex.reg_maxcol > 0 && ستون >= rex.reg_maxcol)
	    {
		retval = 0;
		توڑو;
	    }

	    retval = regtry(prog, ستون, timed_برآمد);
	    اگر (retval > 0)
		توڑو;

	    // if not currently on the first line, get it again
	    اگر (rex.lnum != 0)
	    {
		rex.lnum = 0;
		rex.سطر = reg_getline((linenr_T)0);
	    }
	    اگر (rex.سطر[ستون] == NUL)
		توڑو;
	    اگر (has_mbyte)
		ستون += (*mb_ptr2len)(rex.سطر + ستون);
	    ورنہ
		++ستون;
#اگر_تعریف_شدہ FEAT_RELTIME
	    اگر (bt_did_وقت_برآمد(timed_برآمد))
		توڑو;
#اگر_ختم
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
	    lpos_T *آغاز = &rex.reg_mmatch->startpos[0];
	    lpos_T *انتہا = &rex.reg_mmatch->endpos[0];

	    اگر (انتہا->lnum < آغاز->lnum
			|| (انتہا->lnum == آغاز->lnum && انتہا->ستون < آغاز->ستون))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_mmatch->rmm_matchcol = ستون;
	}
	ورنہ
	{
	    اگر (rex.reg_مطابقت->endp[0] < rex.reg_مطابقت->startp[0])
		rex.reg_مطابقت->endp[0] = rex.reg_مطابقت->startp[0];

	    // startpos[0] may be set by "\zs", also return the column where
	    // the whole pattern matched.
	    rex.reg_مطابقت->rm_matchcol = ستون;
	}
    }

    واپس retval;
}

/*
 * Match a regexp against a string.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 * if "line_lbr" is TRUE  consider a "\n" in "line" to be a line break.
 *
 * Returns 0 for failure, number of lines contained in the match otherwise.
 */
    ساکن صحیح
bt_regexec_nl(
    regmatch_T	*rmp,
    char_u	*سطر,	// string to match against
    colnr_T	ستون,	// column to start looking for match
    صحیح		سطر_lbr)
{
    rex.reg_مطابقت = rmp;
    rex.reg_mmatch = تہی;
    rex.reg_maxline = 0;
    rex.reg_سطر_lbr = سطر_lbr;
    rex.reg_buf = curbuf;
    rex.reg_win = تہی;
    rex.reg_ic = rmp->rm_ic;
    rex.reg_icombine = FALSE;
    rex.reg_maxcol = 0;

    واپس bt_regexec_both(سطر, ستون, تہی);
}

/*
 * Match a regexp against multiple lines.
 * "rmp->regprog" is a compiled regexp as returned by vim_regcomp().
 * Uses curbuf for line count and 'iskeyword'.
 *
 * Return zero if there is no match.  Return number of lines contained in the
 * match otherwise.
 */
    ساکن طویل
bt_regexec_multi(
    regmmatch_T	*rmp,
    win_T	*win,		// window in which to search or NULL
    buf_T	*buf,		// buffer in which to search
    linenr_T	lnum,		// nr of line to start looking for match
    colnr_T	ستون,		// column to start looking for match
    صحیح		*timed_برآمد)	// flag set on timeout or NULL
{
    init_regexec_multi(rmp, win, buf, lnum);
    واپس bt_regexec_both(تہی, ستون, timed_برآمد);
}

/*
 * Compare a number with the operand of RE_LNUM, RE_COL or RE_VCOL.
 */
    ساکن صحیح
re_عدد_cmp(long_u val, char_u *جائزہ)
{
    long_u  n = عملیہ_MIN(جائزہ);

    اگر (عملیہ_CMP(جائزہ) == '>')
	واپس val > n;
    اگر (عملیہ_CMP(جائزہ) == '<')
	واپس val < n;
    واپس val == n;
}

#اگر_تعریف_شدہ BT_REGEXP_DUMP

/*
 * regdump - dump a regexp onto stdout in vaguely comprehensible form
 */
    ساکن خالی
regdump(char_u *نمونہ, bt_regprog_T *r)
{
    char_u  *s;
    صحیح	    op = EXACTLY;	// Arbitrary non-END op.
    char_u  *اگلا;
    char_u  *انتہا = تہی;
    FILE    *f;

# ifdef BT_REGEXP_LOG
    f = fopen("bt_regexp_log.log", "a");
# ورنہ
    f = stdout;
# endif
    اگر (f == تہی)
	واپس;
    fprintf(f, "-------------------------------------\n\r\nregcomp(%s):\r\n", نمونہ);

    s = r->پروگرام + 1;
    // Loop until we find the END that isn't before a referred next (an END
    // can also appear in a NOMATCH operand).
    جب ((op != انتہا_3 || s <= انتہا) && s < r->پروگرام + r->regsz)
    {
	op = OP(s);
	fprintf(f, "%2d%s", (صحیح)(s - r->پروگرام), regprop(s)); // Where, what.
	اگلا = (s + 3 <= r->پروگرام + r->regsz) ? regnext(s) : تہی;
	اگر (اگلا == تہی)	// Next ptr.
	    fprintf(f, "(0)");
	ورنہ
	    fprintf(f, "(%d)", (صحیح)((s - r->پروگرام) + (اگلا - s)));
	اگر (انتہا < اگلا)
	    انتہا = اگلا;
	اگر (op == BRACE_LIMITS)
	{
	    // Two ints
	    fprintf(f, " minval %ld, maxval %ld", عملیہ_MIN(s), عملیہ_MAX(s));
	    s += 8;
	}
	ورنہ اگر (op == BEHIND || op == NOBEHIND)
	{
	    // one int
	    fprintf(f, " count %ld", عملیہ_MIN(s));
	    s += 4;
	}
	ورنہ اگر (op == RE_LNUM || op == RE_ستون || op == RE_VCOL)
	{
	    // one int plus comparator
	    fprintf(f, " count %ld", عملیہ_MIN(s));
	    s += 5;
	}
	s += 3;
	اگر (op == MULTIBYTECODE)
	{
	    fprintf(f, " mbc=%d", utf_ptr2char(s));
	    s += utfc_ptr2len(s);
	}
	اگر (op == ANYOF || op == ANYOF + جوڑو_NL
		|| op == ANYBUT || op == ANYBUT + جوڑو_NL
		|| op == EXACTLY)
	{
	    // Literal string, where present.
	    fprintf(f, "\nxxxxxxxxx\n");
	    جب (*s != NUL && s < r->پروگرام + r->regsz)
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
		? (حرف *)transchar(r->regstart)
		: "multibyte", r->regstart);
    اگر (r->reganch)
	fprintf(f, "anchored; ");
    اگر (r->regmust != تہی)
	fprintf(f, "must have \"%s\"", r->regmust);
    fprintf(f, "\r\n");

# ifdef BT_REGEXP_LOG
    fclose(f);
# endif
}
#اگر_ختم	    // BT_REGEXP_DUMP

#اگر_تعریف_شدہ DEBUG
/*
 * regprop - printable representation of opcode
 */
    ساکن char_u *
regprop(char_u *op)
{
    حرف	    *p;
    ساکن حرف	    buf[50];
    ساکن size_t   buflen = 0;

    STRCPY(buf, ":");
    buflen = 1;

    انتخاب ((صحیح) OP(op))
    {
      حالت BOL:
	p = "BOL";
	توڑو;
      حالت EOL:
	p = "EOL";
	توڑو;
      حالت RE_BOF:
	p = "BOF";
	توڑو;
      حالت RE_EOF:
	p = "EOF";
	توڑو;
      حالت CURSOR:
	p = "CURSOR";
	توڑو;
      حالت RE_VISUAL:
	p = "RE_VISUAL";
	توڑو;
      حالت RE_LNUM:
	p = "RE_LNUM";
	توڑو;
      حالت RE_MARK:
	p = "RE_MARK";
	توڑو;
      حالت RE_ستون:
	p = "RE_COL";
	توڑو;
      حالت RE_VCOL:
	p = "RE_VCOL";
	توڑو;
      حالت BOW:
	p = "BOW";
	توڑو;
      حالت EOW:
	p = "EOW";
	توڑو;
      حالت ANY:
	p = "ANY";
	توڑو;
      حالت ANY + جوڑو_NL:
	p = "ANY+NL";
	توڑو;
      حالت ANYOF:
	p = "ANYOF";
	توڑو;
      حالت ANYOF + جوڑو_NL:
	p = "ANYOF+NL";
	توڑو;
      حالت ANYBUT:
	p = "ANYBUT";
	توڑو;
      حالت ANYBUT + جوڑو_NL:
	p = "ANYBUT+NL";
	توڑو;
      حالت IDENT:
	p = "IDENT";
	توڑو;
      حالت IDENT + جوڑو_NL:
	p = "IDENT+NL";
	توڑو;
      حالت SIDENT:
	p = "SIDENT";
	توڑو;
      حالت SIDENT + جوڑو_NL:
	p = "SIDENT+NL";
	توڑو;
      حالت KWORD:
	p = "KWORD";
	توڑو;
      حالت KWORD + جوڑو_NL:
	p = "KWORD+NL";
	توڑو;
      حالت SKWORD:
	p = "SKWORD";
	توڑو;
      حالت SKWORD + جوڑو_NL:
	p = "SKWORD+NL";
	توڑو;
      حالت FNAME:
	p = "FNAME";
	توڑو;
      حالت FNAME + جوڑو_NL:
	p = "FNAME+NL";
	توڑو;
      حالت SFNAME:
	p = "SFNAME";
	توڑو;
      حالت SFNAME + جوڑو_NL:
	p = "SFNAME+NL";
	توڑو;
      حالت PRINT:
	p = "PRINT";
	توڑو;
      حالت PRINT + جوڑو_NL:
	p = "PRINT+NL";
	توڑو;
      حالت SPRINT:
	p = "SPRINT";
	توڑو;
      حالت SPRINT + جوڑو_NL:
	p = "SPRINT+NL";
	توڑو;
      حالت WHITE:
	p = "WHITE";
	توڑو;
      حالت WHITE + جوڑو_NL:
	p = "WHITE+NL";
	توڑو;
      حالت NWHITE:
	p = "NWHITE";
	توڑو;
      حالت NWHITE + جوڑو_NL:
	p = "NWHITE+NL";
	توڑو;
      حالت DIGIT:
	p = "DIGIT";
	توڑو;
      حالت DIGIT + جوڑو_NL:
	p = "DIGIT+NL";
	توڑو;
      حالت NDIGIT:
	p = "NDIGIT";
	توڑو;
      حالت NDIGIT + جوڑو_NL:
	p = "NDIGIT+NL";
	توڑو;
      حالت HEX:
	p = "HEX";
	توڑو;
      حالت HEX + جوڑو_NL:
	p = "HEX+NL";
	توڑو;
      حالت NHEX:
	p = "NHEX";
	توڑو;
      حالت NHEX + جوڑو_NL:
	p = "NHEX+NL";
	توڑو;
      حالت OCTAL:
	p = "OCTAL";
	توڑو;
      حالت OCTAL + جوڑو_NL:
	p = "OCTAL+NL";
	توڑو;
      حالت NOCTAL:
	p = "NOCTAL";
	توڑو;
      حالت NOCTAL + جوڑو_NL:
	p = "NOCTAL+NL";
	توڑو;
      حالت لفظ_2:
	p = "WORD";
	توڑو;
      حالت لفظ_2 + جوڑو_NL:
	p = "WORD+NL";
	توڑو;
      حالت NWORD:
	p = "NWORD";
	توڑو;
      حالت NWORD + جوڑو_NL:
	p = "NWORD+NL";
	توڑو;
      حالت سر_2:
	p = "HEAD";
	توڑو;
      حالت سر_2 + جوڑو_NL:
	p = "HEAD+NL";
	توڑو;
      حالت NHEAD:
	p = "NHEAD";
	توڑو;
      حالت NHEAD + جوڑو_NL:
	p = "NHEAD+NL";
	توڑو;
      حالت ALPHA:
	p = "ALPHA";
	توڑو;
      حالت ALPHA + جوڑو_NL:
	p = "ALPHA+NL";
	توڑو;
      حالت NALPHA:
	p = "NALPHA";
	توڑو;
      حالت NALPHA + جوڑو_NL:
	p = "NALPHA+NL";
	توڑو;
      حالت زیریں_2:
	p = "LOWER";
	توڑو;
      حالت زیریں_2 + جوڑو_NL:
	p = "LOWER+NL";
	توڑو;
      حالت NLOWER:
	p = "NLOWER";
	توڑو;
      حالت NLOWER + جوڑو_NL:
	p = "NLOWER+NL";
	توڑو;
      حالت بالائی:
	p = "UPPER";
	توڑو;
      حالت بالائی + جوڑو_NL:
	p = "UPPER+NL";
	توڑو;
      حالت NUPPER:
	p = "NUPPER";
	توڑو;
      حالت NUPPER + جوڑو_NL:
	p = "NUPPER+NL";
	توڑو;
      حالت BRANCH:
	p = "BRANCH";
	توڑو;
      حالت EXACTLY:
	p = "EXACTLY";
	توڑو;
      حالت NOTHING:
	p = "NOTHING";
	توڑو;
      حالت عقب_2:
	p = "BACK";
	توڑو;
      حالت انتہا_3:
	p = "END";
	توڑو;
      حالت MOPEN + 0:
	p = "MATCH START";
	توڑو;
      حالت MOPEN + 1:
      حالت MOPEN + 2:
      حالت MOPEN + 3:
      حالت MOPEN + 4:
      حالت MOPEN + 5:
      حالت MOPEN + 6:
      حالت MOPEN + 7:
      حالت MOPEN + 8:
      حالت MOPEN + 9:
	buflen += vim_snprintf(buf + buflen, حجم(buf) - buflen, "MOPEN%d", OP(op) - MOPEN);
	p = تہی;
	توڑو;
      حالت MCLOSE + 0:
	p = "MATCH END";
	توڑو;
      حالت MCLOSE + 1:
      حالت MCLOSE + 2:
      حالت MCLOSE + 3:
      حالت MCLOSE + 4:
      حالت MCLOSE + 5:
      حالت MCLOSE + 6:
      حالت MCLOSE + 7:
      حالت MCLOSE + 8:
      حالت MCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, حجم(buf) - buflen, "MCLOSE%d", OP(op) - MCLOSE);
	p = تہی;
	توڑو;
      حالت BACKREF + 1:
      حالت BACKREF + 2:
      حالت BACKREF + 3:
      حالت BACKREF + 4:
      حالت BACKREF + 5:
      حالت BACKREF + 6:
      حالت BACKREF + 7:
      حالت BACKREF + 8:
      حالت BACKREF + 9:
	buflen += vim_snprintf(buf + buflen, حجم(buf) - buflen, "BACKREF%d", OP(op) - BACKREF);
	p = تہی;
	توڑو;
      حالت NOPEN:
	p = "NOPEN";
	توڑو;
      حالت NCLOSE:
	p = "NCLOSE";
	توڑو;
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
	buflen += vim_snprintf(buf + buflen, حجم(buf) - buflen, "ZOPEN%d", OP(op) - ZOPEN);
	p = تہی;
	توڑو;
      حالت ZCLOSE + 1:
      حالت ZCLOSE + 2:
      حالت ZCLOSE + 3:
      حالت ZCLOSE + 4:
      حالت ZCLOSE + 5:
      حالت ZCLOSE + 6:
      حالت ZCLOSE + 7:
      حالت ZCLOSE + 8:
      حالت ZCLOSE + 9:
	buflen += vim_snprintf(buf + buflen, حجم(buf) - buflen, "ZCLOSE%d", OP(op) - ZCLOSE);
	p = تہی;
	توڑو;
      حالت ZREF + 1:
      حالت ZREF + 2:
      حالت ZREF + 3:
      حالت ZREF + 4:
      حالت ZREF + 5:
      حالت ZREF + 6:
      حالت ZREF + 7:
      حالت ZREF + 8:
      حالت ZREF + 9:
	buflen += vim_snprintf(buf + buflen, حجم(buf) - buflen, "ZREF%d", OP(op) - ZREF);
	p = تہی;
	توڑو;
# endif
      حالت ستارہ:
	p = "STAR";
	توڑو;
      حالت PLUS:
	p = "PLUS";
	توڑو;
      حالت NOMATCH:
	p = "NOMATCH";
	توڑو;
      حالت مطابقت_2:
	p = "MATCH";
	توڑو;
      حالت BEHIND:
	p = "BEHIND";
	توڑو;
      حالت NOBEHIND:
	p = "NOBEHIND";
	توڑو;
      حالت SUBPAT:
	p = "SUBPAT";
	توڑو;
      حالت BRACE_LIMITS:
	p = "BRACE_LIMITS";
	توڑو;
      حالت BRACE_SIMPLE:
	p = "BRACE_SIMPLE";
	توڑو;
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
	buflen += vim_snprintf(buf + buflen, حجم(buf) - buflen, "BRACE_COMPLEX%d", OP(op) - BRACE_COMPLEX);
	p = تہی;
	توڑو;
      حالت MULTIBYTECODE:
	p = "MULTIBYTECODE";
	توڑو;
      حالت NEWL:
	p = "NEWL";
	توڑو;
      طےشدہ:
	buflen += vim_snprintf(buf + buflen, حجم(buf) - buflen, "corrupt %d", OP(op));
	p = تہی;
	توڑو;
    }
    اگر (p != تہی)
	vim_strncpy((char_u *)buf + buflen, (char_u *)p, حجم(buf) - buflen - 1);
    واپس (char_u *)buf;
}
#اگر_ختم	    // DEBUG
