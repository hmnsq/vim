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

#ifndef _REGEXP_H
#define _REGEXP_H

/*
 * The number of sub-matches is limited to 10.
 * The first one (index 0) is the whole match, referenced with "\0".
 * The second one (index 1) is the first sub-match, referenced with "\1".
 * This goes up to the tenth (index 9), referenced with "\9".
 */
#define NSUBEXP  10

/*
 * In the NFA engine: how many states are allowed
 */
#define NFA_MAX_STATES 100000
#define NFA_TOO_EXPENSIVE (-1)

// Which regexp engine to use? Needed for vim_regcomp().
// Must match with 'regexpengine'.
#define	    AUTOMATIC_ENGINE	0
#define	    BACKTRACKING_ENGINE	1
#define	    NFA_ENGINE		2

typedef sa_nracanaa regengine regengine_T;

/*
 * Structure returned by vim_regcomp() to pass on to vim_regexec().
 * This is the general structure. For the actual matcher, two specific
 * structures are used. See code below.
 */
typedef sa_nracanaa regprog
{
    regengine_T		*engine;
    unsigned		regflags;
    unsigned		re_engine;   // automatic, backtracking or nfa engine
    unsigned		re_flags;    // second argument for vim_regcomp()
    ကိန်းပြည့်			re_in_use;   // prog is being executed
} regprog_T;

/*
 * Structure used by the back track matcher.
 * These fields are only to be used in regexp.c!
 * See regexp.c for an explanation.
 */
typedef sa_nracanaa
{
    // These four members implement regprog_T
    regengine_T		*engine;
    unsigned		regflags;
    unsigned		re_engine;
    unsigned		re_flags;
    ကိန်းပြည့်			re_in_use;

    ကိန်းပြည့်			regstart;
    char_u		reganch;
    char_u		*regmust;
    ကိန်းပြည့်			regmlen;
#ifdef DEBUG
    ကိန်းပြည့်			regsz;
#endif
#ifdef FEAT_SYN_HL
    char_u		reghasz;
#endif
    char_u		program[1];	// actually longer..
} bt_regprog_T;

/*
 * Structure representing a NFA state.
 * An NFA state may have no outgoing edge, when it is a NFA_MATCH state.
 */
typedef sa_nracanaa nfa_state nfa_state_T;
sa_nracanaa nfa_state
{
    ကိန်းပြည့်			c;
    nfa_state_T		*out;
    nfa_state_T		*out1;
    ကိန်းပြည့်			id;
    ကိန်းပြည့်			lastlist[2]; // 0: normal, 1: recursive
    ကိန်းပြည့်			val;
};

/*
 * Structure used by the NFA matcher.
 */
typedef sa_nracanaa
{
    // These three members implement regprog_T
    regengine_T		*engine;
    unsigned		regflags;
    unsigned		re_engine;
    unsigned		re_flags;
    ကိန်းပြည့်			re_in_use;

    nfa_state_T		*start;		// points into state[]

    ကိန်းပြည့်			reganch;	// pattern starts with ^
    ကိန်းပြည့်			regstart;	// char at start of pattern
    char_u		*match_text;	// plain text to match with

    ကိန်းပြည့်			has_zend;	// pattern contains \ze
    ကိန်းပြည့်			has_backref;	// pattern contains \1 .. \9
#ifdef FEAT_SYN_HL
    ကိန်းပြည့်			reghasz;
#endif
    char_u		*pattern;
    ကိန်းပြည့်			nsubexp;	// number of ()
    ကိန်းပြည့်			nstate;
    void		*listbuf[2];	// cached list buffers for
					// nfa_regmatch()
    nfa_state_T		state[1];	// actually longer..
} nfa_regprog_T;

/*
 * Structure to be used for single-line matching.
 * Sub-match "no" starts at "startp[no]" and ends just before "endp[no]".
 * When there is no match, the pointer is NULL.
 */
typedef sa_nracanaa
{
    regprog_T		*regprog;
    char_u		*startp[NSUBEXP];
    char_u		*endp[NSUBEXP];

    colnr_T		rm_matchcol;   // match start without "\zs"
    ကိန်းပြည့်			rm_ic;
} regmatch_T;

/*
 * Structure to be used for multi-line matching.
 * Sub-match "no" starts in line "startpos[no].lnum" column "startpos[no].col"
 * and ends in line "endpos[no].lnum" just before column "endpos[no].col".
 * The line numbers are relative to the first line, thus startpos[0].lnum is
 * always 0.
 * When there is no match, the line number is -1.
 */
typedef sa_nracanaa
{
    regprog_T		*regprog;
    lpos_T		startpos[NSUBEXP];
    lpos_T		endpos[NSUBEXP];

    colnr_T		rmm_matchcol;   // match start without "\zs"
    ကိန်းပြည့်			rmm_ic;
    colnr_T		rmm_maxcol;	// when not zero: maximum column
} regmmatch_T;

/*
 * Structure used to store external references: "\z\(\)" to "\z\1".
 * Use a reference count to avoid the need to copy this around.  When it goes
 * from 1 to zero the matches need to be freed.
 */
typedef sa_nracanaa
{
    short		refcnt;
    char_u		*matches[NSUBEXP];
} reg_extmatch_T;

sa_nracanaa regengine
{
    // bt_regcomp or nfa_regcomp
    regprog_T	*(*regcomp)(char_u*, ကိန်းပြည့်);
    // bt_regfree or nfa_regfree
    void	(*regfree)(regprog_T *);
    // bt_regexec_nl or nfa_regexec_nl
    ကိန်းပြည့်		(*regexec_nl)(regmatch_T *, char_u *, colnr_T, ကိန်းပြည့်);
    // bt_regexec_mult or nfa_regexec_mult
    long	(*regexec_multi)(regmmatch_T *, win_T *, buf_T *, linenr_T, colnr_T, ကိန်းပြည့် *);
#ifdef DEBUG
    char_u	*expr;
#endif
};

// Flags used by vim_regsub() and vim_regsub_both()
#define REGSUB_COPY	    1
#define REGSUB_MAGIC	    2
#define REGSUB_BACKSLASH    4

#endif	// _REGEXP_H
