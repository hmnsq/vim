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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _REGEXP_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _REGEXP_H

/*
 * The number of sub-matches is limited to 10.
 * The first one (index 0) is the whole match, referenced with "\0".
 * The second one (index 1) is the first sub-match, referenced with "\1".
 * This goes up to the tenth (index 9), referenced with "\9".
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NSUBEXP  10

/*
 * In the NFA engine: how many states are allowed
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NFA_MAX_ਹਾਲਤਾਂ 100000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NFA_TOO_EXPENSIVE (-1)

// Which regexp engine to use? Needed for vim_regcomp().
// Must match with 'regexpengine'.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	    AUTOMATIC_ENGINE	0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	    BACKTRACKING_ENGINE	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	    NFA_ENGINE		2

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ regengine regengine_T;

/*
 * Structure returned by vim_regcomp() to pass on to vim_regexec().
 * This is the general structure. For the actual matcher, two specific
 * structures are used. See code below.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ regprog
{
    regengine_T		*engine;
    ਬਿਨਾਨਿਸ਼ਾਨੀ		regflags;
    ਬਿਨਾਨਿਸ਼ਾਨੀ		re_engine;   // automatic, backtracking or nfa engine
    ਬਿਨਾਨਿਸ਼ਾਨੀ		re_flags;    // second argument for vim_regcomp()
    ਪੂਰਨ			re_ਵਿੱਚ_use;   // prog is being executed
} regprog_T;

/*
 * Structure used by the back track matcher.
 * These fields are only to be used in regexp.c!
 * See regexp.c for an explanation.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    // These four members implement regprog_T
    regengine_T		*engine;
    ਬਿਨਾਨਿਸ਼ਾਨੀ		regflags;
    ਬਿਨਾਨਿਸ਼ਾਨੀ		re_engine;
    ਬਿਨਾਨਿਸ਼ਾਨੀ		re_flags;
    ਪੂਰਨ			re_ਵਿੱਚ_use;

    ਪੂਰਨ			regstart;
    char_u		reganch;
    char_u		*regmust;
    ਪੂਰਨ			regmlen;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
    ਪੂਰਨ			regsz;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    char_u		reghasz;
#ਜੇ_ਅੰਤ
    char_u		ਪ੍ਰੋਗਰਾਮ[1];	// actually longer..
} bt_regprog_T;

/*
 * Structure representing a NFA state.
 * An NFA state may have no outgoing edge, when it is a NFA_MATCH state.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ nfa_state nfa_state_T;
ਬਣਤਰ nfa_state
{
    ਪੂਰਨ			c;
    nfa_state_T		*ਬਾਹਰਲਾ;
    nfa_state_T		*out1;
    ਪੂਰਨ			id;
    ਪੂਰਨ			lastlist[2]; // 0: normal, 1: recursive
    ਪੂਰਨ			val;
};

/*
 * Structure used by the NFA matcher.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    // These three members implement regprog_T
    regengine_T		*engine;
    ਬਿਨਾਨਿਸ਼ਾਨੀ		regflags;
    ਬਿਨਾਨਿਸ਼ਾਨੀ		re_engine;
    ਬਿਨਾਨਿਸ਼ਾਨੀ		re_flags;
    ਪੂਰਨ			re_ਵਿੱਚ_use;

    nfa_state_T		*ਸ਼ੁਰੂ;		// points into state[]

    ਪੂਰਨ			reganch;	// pattern starts with ^
    ਪੂਰਨ			regstart;	// char at start of pattern
    char_u		*ਮੇਲ_ਪਾਠ;	// plain text to match with

    ਪੂਰਨ			has_zend;	// pattern contains \ze
    ਪੂਰਨ			has_backref;	// pattern contains \1 .. \9
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ਪੂਰਨ			reghasz;
#ਜੇ_ਅੰਤ
    char_u		*ਨਮੂਨਾ;
    ਪੂਰਨ			nsubexp;	// number of ()
    ਪੂਰਨ			nstate;
    ਖਾਲੀ		*listbuf[2];	// cached list buffers for
					// nfa_regmatch()
    nfa_state_T		state[1];	// actually longer..
} nfa_regprog_T;

/*
 * Structure to be used for single-line matching.
 * Sub-match "no" starts at "startp[no]" and ends just before "endp[no]".
 * When there is no match, the pointer is NULL.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    regprog_T		*regprog;
    char_u		*startp[NSUBEXP];
    char_u		*endp[NSUBEXP];

    colnr_T		rm_matchcol;   // match start without "\zs"
    ਪੂਰਨ			rm_ic;
} regmatch_T;

/*
 * Structure to be used for multi-line matching.
 * Sub-match "no" starts in line "startpos[no].lnum" column "startpos[no].col"
 * and ends in line "endpos[no].lnum" just before column "endpos[no].col".
 * The line numbers are relative to the first line, thus startpos[0].lnum is
 * always 0.
 * When there is no match, the line number is -1.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    regprog_T		*regprog;
    lpos_T		startpos[NSUBEXP];
    lpos_T		endpos[NSUBEXP];

    colnr_T		rmm_matchcol;   // match start without "\zs"
    ਪੂਰਨ			rmm_ic;
    colnr_T		rmm_maxcol;	// when not zero: maximum column
} regmmatch_T;

/*
 * Structure used to store external references: "\z\(\)" to "\z\1".
 * Use a reference count to avoid the need to copy this around.  When it goes
 * from 1 to zero the matches need to be freed.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਛੋਟਾ		refcnt;
    char_u		*matches[NSUBEXP];
} reg_extmatch_T;

ਬਣਤਰ regengine
{
    // bt_regcomp or nfa_regcomp
    regprog_T	*(*regcomp)(char_u*, ਪੂਰਨ);
    // bt_regfree or nfa_regfree
    ਖਾਲੀ	(*regfree)(regprog_T *);
    // bt_regexec_nl or nfa_regexec_nl
    ਪੂਰਨ		(*regexec_nl)(regmatch_T *, char_u *, colnr_T, ਪੂਰਨ);
    // bt_regexec_mult or nfa_regexec_mult
    ਲੰਮਾ	(*regexec_multi)(regmmatch_T *, win_T *, buf_T *, linenr_T, colnr_T, ਪੂਰਨ *);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
    char_u	*ਵਿਅੰਜਕ;
#ਜੇ_ਅੰਤ
};

// Flags used by vim_regsub() and vim_regsub_both()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REGSUB_COPY	    1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REGSUB_MAGIC	    2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REGSUB_BACKSLASH    4

#ਜੇ_ਅੰਤ	// _REGEXP_H
