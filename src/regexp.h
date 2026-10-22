/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE
 *
 * This is NOT the original regular expression code as written by Henry
 * Spencer.  This code has been modified specifically for use with Vim, and
 * should not be used apart from compiling Vim.  If you want a good regular
 * expression library, get the original code.
 *
 * NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE
 */

#جيڪڏهن_وصف_نه_ٿيل _REGEXP_H
#وصف_ڪريو _REGEXP_H

/*
 * The number of sub-matches is limited to 10.
 * The first one (index 0) is the whole match, referenced with "\0".
 * The second one (index 1) is the first sub-match, referenced with "\1".
 * This goes up to the tenth (index 9), referenced with "\9".
 */
#وصف_ڪريو NSUBEXP  10

/*
 * In the NFA engine: how many states are allowed
 */
#وصف_ڪريو NFA_MAX_حالتون 100000
#وصف_ڪريو NFA_TOO_EXPENSIVE (-1)

// Which regexp engine to use? Needed for vim_regcomp().
// Must match with 'regexpengine'.
#وصف_ڪريو	    AUTOMATIC_ENGINE	0
#وصف_ڪريو	    BACKTRACKING_ENGINE	1
#وصف_ڪريو	    NFA_ENGINE		2

تعريفنوع بنية regengine regengine_T;

/*
 * Structure returned by vim_regcomp() to pass on to vim_regexec().
 * This is the general structure. For the actual matcher, two specific
 * structures are used. See code below.
 */
تعريفنوع بنية regprog
{
    regengine_T		*engine;
    بنانشان		regflags;
    بنانشان		re_engine;   // automatic, backtracking or nfa engine
    بنانشان		re_flags;    // second argument for vim_regcomp()
    صحيح			re_اندريون_use;   // prog is being executed
} regprog_T;

/*
 * Structure used by the back track matcher.
 * These fields are only to be used in regexp.c!
 * See regexp.c for an explanation.
 */
تعريفنوع بنية
{
    // These four members implement regprog_T
    regengine_T		*engine;
    بنانشان		regflags;
    بنانشان		re_engine;
    بنانشان		re_flags;
    صحيح			re_اندريون_use;

    صحيح			regstart;
    char_u		reganch;
    char_u		*regmust;
    صحيح			regmlen;
#جيڪڏهن_وصف_ٿيل DEBUG
    صحيح			regsz;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
    char_u		reghasz;
#جيڪڏهن_پورو
    char_u		پروگرام[1];	// actually longer..
} bt_regprog_T;

/*
 * Structure representing a NFA state.
 * An NFA state may have no outgoing edge, when it is a NFA_MATCH state.
 */
تعريفنوع بنية nfa_state nfa_state_T;
بنية nfa_state
{
    صحيح			c;
    nfa_state_T		*ٻاهريون;
    nfa_state_T		*out1;
    صحيح			id;
    صحيح			lastlist[2]; // 0: normal, 1: recursive
    صحيح			val;
};

/*
 * Structure used by the NFA matcher.
 */
تعريفنوع بنية
{
    // These three members implement regprog_T
    regengine_T		*engine;
    بنانشان		regflags;
    بنانشان		re_engine;
    بنانشان		re_flags;
    صحيح			re_اندريون_use;

    nfa_state_T		*شروعات;		// points into state[]

    صحيح			reganch;	// pattern starts with ^
    صحيح			regstart;	// char at start of pattern
    char_u		*ميلاپ_متن;	// plain text to match with

    صحيح			has_zend;	// pattern contains \ze
    صحيح			has_backref;	// pattern contains \1 .. \9
#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
    صحيح			reghasz;
#جيڪڏهن_پورو
    char_u		*نمونو;
    صحيح			nsubexp;	// number of ()
    صحيح			nstate;
    فراغ		*listbuf[2];	// cached list buffers for
					// nfa_regmatch()
    nfa_state_T		state[1];	// actually longer..
} nfa_regprog_T;

/*
 * Structure to be used for single-line matching.
 * Sub-match "no" starts at "startp[no]" and ends just before "endp[no]".
 * When there is no match, the pointer is NULL.
 */
تعريفنوع بنية
{
    regprog_T		*regprog;
    char_u		*startp[NSUBEXP];
    char_u		*endp[NSUBEXP];

    colnr_T		rm_matchcol;   // match start without "\zs"
    صحيح			rm_ic;
} regmatch_T;

/*
 * Structure to be used for multi-line matching.
 * Sub-match "no" starts in line "startpos[no].lnum" column "startpos[no].col"
 * and ends in line "endpos[no].lnum" just before column "endpos[no].col".
 * The line numbers are relative to the first line, thus startpos[0].lnum is
 * always 0.
 * When there is no match, the line number is -1.
 */
تعريفنوع بنية
{
    regprog_T		*regprog;
    lpos_T		startpos[NSUBEXP];
    lpos_T		endpos[NSUBEXP];

    colnr_T		rmm_matchcol;   // match start without "\zs"
    صحيح			rmm_ic;
    colnr_T		rmm_maxcol;	// when not zero: maximum column
} regmmatch_T;

/*
 * Structure used to store external references: "\z\(\)" to "\z\1".
 * Use a reference count to avoid the need to copy this around.  When it goes
 * from 1 to zero the matches need to be freed.
 */
تعريفنوع بنية
{
    ننڍو		refcnt;
    char_u		*matches[NSUBEXP];
} reg_extmatch_T;

بنية regengine
{
    // bt_regcomp or nfa_regcomp
    regprog_T	*(*regcomp)(char_u*, صحيح);
    // bt_regfree or nfa_regfree
    فراغ	(*regfree)(regprog_T *);
    // bt_regexec_nl or nfa_regexec_nl
    صحيح		(*regexec_nl)(regmatch_T *, char_u *, colnr_T, صحيح);
    // bt_regexec_mult or nfa_regexec_mult
    ڊگهو	(*regexec_multi)(regmmatch_T *, win_T *, buf_T *, linenr_T, colnr_T, صحيح *);
#جيڪڏهن_وصف_ٿيل DEBUG
    char_u	*اظهار;
#جيڪڏهن_پورو
};

// Flags used by vim_regsub() and vim_regsub_both()
#وصف_ڪريو REGSUB_COPY	    1
#وصف_ڪريو REGSUB_MAGIC	    2
#وصف_ڪريو REGSUB_BACKSLASH    4

#جيڪڏهن_پورو	// _REGEXP_H
