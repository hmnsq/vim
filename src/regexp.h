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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _REGEXP_H
#ವ್ಯಾಖ್ಯಾನಿಸು _REGEXP_H

/*
 * The number of sub-matches is limited to 10.
 * The first one (index 0) is the whole match, referenced with "\0".
 * The second one (index 1) is the first sub-match, referenced with "\1".
 * This goes up to the tenth (index 9), referenced with "\9".
 */
#ವ್ಯಾಖ್ಯಾನಿಸು NSUBEXP  10

/*
 * In the NFA engine: how many states are allowed
 */
#ವ್ಯಾಖ್ಯಾನಿಸು NFA_MAX_ಸ್ಥಿತಿಗಳು 100000
#ವ್ಯಾಖ್ಯಾನಿಸು NFA_TOO_EXPENSIVE (-1)

// Which regexp engine to use? Needed for vim_regcomp().
// Must match with 'regexpengine'.
#ವ್ಯಾಖ್ಯಾನಿಸು	    AUTOMATIC_ENGINE	0
#ವ್ಯಾಖ್ಯಾನಿಸು	    BACKTRACKING_ENGINE	1
#ವ್ಯಾಖ್ಯಾನಿಸು	    NFA_ENGINE		2

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ regengine regengine_T;

/*
 * Structure returned by vim_regcomp() to pass on to vim_regexec().
 * This is the general structure. For the actual matcher, two specific
 * structures are used. See code below.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ regprog
{
    regengine_T		*engine;
    ಚಿಹ್ನೆರಹಿತ		regflags;
    ಚಿಹ್ನೆರಹಿತ		re_engine;   // automatic, backtracking or nfa engine
    ಚಿಹ್ನೆರಹಿತ		re_flags;    // second argument for vim_regcomp()
    ಪೂರ್ಣಾಂಕ			re_ಒಳ_use;   // prog is being executed
} regprog_T;

/*
 * Structure used by the back track matcher.
 * These fields are only to be used in regexp.c!
 * See regexp.c for an explanation.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    // These four members implement regprog_T
    regengine_T		*engine;
    ಚಿಹ್ನೆರಹಿತ		regflags;
    ಚಿಹ್ನೆರಹಿತ		re_engine;
    ಚಿಹ್ನೆರಹಿತ		re_flags;
    ಪೂರ್ಣಾಂಕ			re_ಒಳ_use;

    ಪೂರ್ಣಾಂಕ			regstart;
    char_u		reganch;
    char_u		*regmust;
    ಪೂರ್ಣಾಂಕ			regmlen;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
    ಪೂರ್ಣಾಂಕ			regsz;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    char_u		reghasz;
#ಆದರೆ_ಅಂತ್ಯ
    char_u		ಕಾರ್ಯಕ್ರಮ[1];	// actually longer..
} bt_regprog_T;

/*
 * Structure representing a NFA state.
 * An NFA state may have no outgoing edge, when it is a NFA_MATCH state.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ nfa_state nfa_state_T;
ರಚನೆ nfa_state
{
    ಪೂರ್ಣಾಂಕ			c;
    nfa_state_T		*ಹೊರಹೋಗು;
    nfa_state_T		*out1;
    ಪೂರ್ಣಾಂಕ			id;
    ಪೂರ್ಣಾಂಕ			lastlist[2]; // 0: normal, 1: recursive
    ಪೂರ್ಣಾಂಕ			val;
};

/*
 * Structure used by the NFA matcher.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    // These three members implement regprog_T
    regengine_T		*engine;
    ಚಿಹ್ನೆರಹಿತ		regflags;
    ಚಿಹ್ನೆರಹಿತ		re_engine;
    ಚಿಹ್ನೆರಹಿತ		re_flags;
    ಪೂರ್ಣಾಂಕ			re_ಒಳ_use;

    nfa_state_T		*ಆರಂಭ;		// points into state[]

    ಪೂರ್ಣಾಂಕ			reganch;	// pattern starts with ^
    ಪೂರ್ಣಾಂಕ			regstart;	// char at start of pattern
    char_u		*ಹೊಂದಿಕೆ_ಪಠ್ಯ;	// plain text to match with

    ಪೂರ್ಣಾಂಕ			has_zend;	// pattern contains \ze
    ಪೂರ್ಣಾಂಕ			has_backref;	// pattern contains \1 .. \9
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ಪೂರ್ಣಾಂಕ			reghasz;
#ಆದರೆ_ಅಂತ್ಯ
    char_u		*ನಮೂನೆ;
    ಪೂರ್ಣಾಂಕ			nsubexp;	// number of ()
    ಪೂರ್ಣಾಂಕ			nstate;
    ಶೂನ್ಯ		*listbuf[2];	// cached list buffers for
					// nfa_regmatch()
    nfa_state_T		state[1];	// actually longer..
} nfa_regprog_T;

/*
 * Structure to be used for single-line matching.
 * Sub-match "no" starts at "startp[no]" and ends just before "endp[no]".
 * When there is no match, the pointer is NULL.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    regprog_T		*regprog;
    char_u		*startp[NSUBEXP];
    char_u		*endp[NSUBEXP];

    colnr_T		rm_matchcol;   // match start without "\zs"
    ಪೂರ್ಣಾಂಕ			rm_ic;
} regmatch_T;

/*
 * Structure to be used for multi-line matching.
 * Sub-match "no" starts in line "startpos[no].lnum" column "startpos[no].col"
 * and ends in line "endpos[no].lnum" just before column "endpos[no].col".
 * The line numbers are relative to the first line, thus startpos[0].lnum is
 * always 0.
 * When there is no match, the line number is -1.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    regprog_T		*regprog;
    lpos_T		startpos[NSUBEXP];
    lpos_T		endpos[NSUBEXP];

    colnr_T		rmm_matchcol;   // match start without "\zs"
    ಪೂರ್ಣಾಂಕ			rmm_ic;
    colnr_T		rmm_maxcol;	// when not zero: maximum column
} regmmatch_T;

/*
 * Structure used to store external references: "\z\(\)" to "\z\1".
 * Use a reference count to avoid the need to copy this around.  When it goes
 * from 1 to zero the matches need to be freed.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಗಿಡ್ಡ		refcnt;
    char_u		*matches[NSUBEXP];
} reg_extmatch_T;

ರಚನೆ regengine
{
    // bt_regcomp or nfa_regcomp
    regprog_T	*(*regcomp)(char_u*, ಪೂರ್ಣಾಂಕ);
    // bt_regfree or nfa_regfree
    ಶೂನ್ಯ	(*regfree)(regprog_T *);
    // bt_regexec_nl or nfa_regexec_nl
    ಪೂರ್ಣಾಂಕ		(*regexec_nl)(regmatch_T *, char_u *, colnr_T, ಪೂರ್ಣಾಂಕ);
    // bt_regexec_mult or nfa_regexec_mult
    ದೀರ್ಘ	(*regexec_multi)(regmmatch_T *, win_T *, buf_T *, linenr_T, colnr_T, ಪೂರ್ಣಾಂಕ *);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
    char_u	*ಅಭಿವ್ಯಕ್ತಿ;
#ಆದರೆ_ಅಂತ್ಯ
};

// Flags used by vim_regsub() and vim_regsub_both()
#ವ್ಯಾಖ್ಯಾನಿಸು REGSUB_COPY	    1
#ವ್ಯಾಖ್ಯಾನಿಸು REGSUB_MAGIC	    2
#ವ್ಯಾಖ್ಯಾನಿಸು REGSUB_BACKSLASH    4

#ಆದರೆ_ಅಂತ್ಯ	// _REGEXP_H
