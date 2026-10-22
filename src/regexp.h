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

#నిర్వచితం_కాకపోతే _REGEXP_H
#నిర్వచించు _REGEXP_H

/*
 * The number of sub-matches is limited to 10.
 * The first one (index 0) is the whole match, referenced with "\0".
 * The second one (index 1) is the first sub-match, referenced with "\1".
 * This goes up to the tenth (index 9), referenced with "\9".
 */
#నిర్వచించు NSUBEXP  10

/*
 * In the NFA engine: how many states are allowed
 */
#నిర్వచించు NFA_MAX_స్థితులు 100000
#నిర్వచించు NFA_TOO_EXPENSIVE (-1)

// Which regexp engine to use? Needed for vim_regcomp().
// Must match with 'regexpengine'.
#నిర్వచించు	    AUTOMATIC_ENGINE	0
#నిర్వచించు	    BACKTRACKING_ENGINE	1
#నిర్వచించు	    NFA_ENGINE		2

రకనిర్వచనం నిర్మాణం regengine regengine_T;

/*
 * Structure returned by vim_regcomp() to pass on to vim_regexec().
 * This is the general structure. For the actual matcher, two specific
 * structures are used. See code below.
 */
రకనిర్వచనం నిర్మాణం regprog
{
    regengine_T		*engine;
    గుర్తులేని		regflags;
    గుర్తులేని		re_engine;   // automatic, backtracking or nfa engine
    గుర్తులేని		re_flags;    // second argument for vim_regcomp()
    పూర్ణాంకం			re_లోపల_use;   // prog is being executed
} regprog_T;

/*
 * Structure used by the back track matcher.
 * These fields are only to be used in regexp.c!
 * See regexp.c for an explanation.
 */
రకనిర్వచనం నిర్మాణం
{
    // These four members implement regprog_T
    regengine_T		*engine;
    గుర్తులేని		regflags;
    గుర్తులేని		re_engine;
    గుర్తులేని		re_flags;
    పూర్ణాంకం			re_లోపల_use;

    పూర్ణాంకం			regstart;
    char_u		reganch;
    char_u		*regmust;
    పూర్ణాంకం			regmlen;
#నిర్వచితమైతే DEBUG
    పూర్ణాంకం			regsz;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SYN_HL
    char_u		reghasz;
#అయితే_ముగింపు
    char_u		కార్యక్రమం[1];	// actually longer..
} bt_regprog_T;

/*
 * Structure representing a NFA state.
 * An NFA state may have no outgoing edge, when it is a NFA_MATCH state.
 */
రకనిర్వచనం నిర్మాణం nfa_state nfa_state_T;
నిర్మాణం nfa_state
{
    పూర్ణాంకం			c;
    nfa_state_T		*బహిర్గతం;
    nfa_state_T		*out1;
    పూర్ణాంకం			id;
    పూర్ణాంకం			lastlist[2]; // 0: normal, 1: recursive
    పూర్ణాంకం			val;
};

/*
 * Structure used by the NFA matcher.
 */
రకనిర్వచనం నిర్మాణం
{
    // These three members implement regprog_T
    regengine_T		*engine;
    గుర్తులేని		regflags;
    గుర్తులేని		re_engine;
    గుర్తులేని		re_flags;
    పూర్ణాంకం			re_లోపల_use;

    nfa_state_T		*ప్రారంభం;		// points into state[]

    పూర్ణాంకం			reganch;	// pattern starts with ^
    పూర్ణాంకం			regstart;	// char at start of pattern
    char_u		*సరిపోలిక_పాఠ్యం;	// plain text to match with

    పూర్ణాంకం			has_zend;	// pattern contains \ze
    పూర్ణాంకం			has_backref;	// pattern contains \1 .. \9
#నిర్వచితమైతే FEAT_SYN_HL
    పూర్ణాంకం			reghasz;
#అయితే_ముగింపు
    char_u		*నమూనా;
    పూర్ణాంకం			nsubexp;	// number of ()
    పూర్ణాంకం			nstate;
    శూన్య		*listbuf[2];	// cached list buffers for
					// nfa_regmatch()
    nfa_state_T		state[1];	// actually longer..
} nfa_regprog_T;

/*
 * Structure to be used for single-line matching.
 * Sub-match "no" starts at "startp[no]" and ends just before "endp[no]".
 * When there is no match, the pointer is NULL.
 */
రకనిర్వచనం నిర్మాణం
{
    regprog_T		*regprog;
    char_u		*startp[NSUBEXP];
    char_u		*endp[NSUBEXP];

    colnr_T		rm_matchcol;   // match start without "\zs"
    పూర్ణాంకం			rm_ic;
} regmatch_T;

/*
 * Structure to be used for multi-line matching.
 * Sub-match "no" starts in line "startpos[no].lnum" column "startpos[no].col"
 * and ends in line "endpos[no].lnum" just before column "endpos[no].col".
 * The line numbers are relative to the first line, thus startpos[0].lnum is
 * always 0.
 * When there is no match, the line number is -1.
 */
రకనిర్వచనం నిర్మాణం
{
    regprog_T		*regprog;
    lpos_T		startpos[NSUBEXP];
    lpos_T		endpos[NSUBEXP];

    colnr_T		rmm_matchcol;   // match start without "\zs"
    పూర్ణాంకం			rmm_ic;
    colnr_T		rmm_maxcol;	// when not zero: maximum column
} regmmatch_T;

/*
 * Structure used to store external references: "\z\(\)" to "\z\1".
 * Use a reference count to avoid the need to copy this around.  When it goes
 * from 1 to zero the matches need to be freed.
 */
రకనిర్వచనం నిర్మాణం
{
    పొట్టి		refcnt;
    char_u		*matches[NSUBEXP];
} reg_extmatch_T;

నిర్మాణం regengine
{
    // bt_regcomp or nfa_regcomp
    regprog_T	*(*regcomp)(char_u*, పూర్ణాంకం);
    // bt_regfree or nfa_regfree
    శూన్య	(*regfree)(regprog_T *);
    // bt_regexec_nl or nfa_regexec_nl
    పూర్ణాంకం		(*regexec_nl)(regmatch_T *, char_u *, colnr_T, పూర్ణాంకం);
    // bt_regexec_mult or nfa_regexec_mult
    దీర్ఘ	(*regexec_multi)(regmmatch_T *, win_T *, buf_T *, linenr_T, colnr_T, పూర్ణాంకం *);
#నిర్వచితమైతే DEBUG
    char_u	*సమాసం;
#అయితే_ముగింపు
};

// Flags used by vim_regsub() and vim_regsub_both()
#నిర్వచించు REGSUB_COPY	    1
#నిర్వచించు REGSUB_MAGIC	    2
#నిర్వచించు REGSUB_BACKSLASH    4

#అయితే_ముగింపు	// _REGEXP_H
