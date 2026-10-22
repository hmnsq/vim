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

#اگر_غیر_تعریف_شدہ _REGEXP_H
#تعریف_کرو _REGEXP_H

/*
 * The number of sub-matches is limited to 10.
 * The first one (index 0) is the whole match, referenced with "\0".
 * The second one (index 1) is the first sub-match, referenced with "\1".
 * This goes up to the tenth (index 9), referenced with "\9".
 */
#تعریف_کرو NSUBEXP  10

/*
 * In the NFA engine: how many states are allowed
 */
#تعریف_کرو NFA_MAX_حالتیں 100000
#تعریف_کرو NFA_TOO_EXPENSIVE (-1)

// Which regexp engine to use? Needed for vim_regcomp().
// Must match with 'regexpengine'.
#تعریف_کرو	    AUTOMATIC_ENGINE	0
#تعریف_کرو	    BACKTRACKING_ENGINE	1
#تعریف_کرو	    NFA_ENGINE		2

قسمتعریف ساخت regengine regengine_T;

/*
 * Structure returned by vim_regcomp() to pass on to vim_regexec().
 * This is the general structure. For the actual matcher, two specific
 * structures are used. See code below.
 */
قسمتعریف ساخت regprog
{
    regengine_T		*engine;
    غیرعلامتی		regflags;
    غیرعلامتی		re_engine;   // automatic, backtracking or nfa engine
    غیرعلامتی		re_flags;    // second argument for vim_regcomp()
    صحیح			re_اندر_use;   // prog is being executed
} regprog_T;

/*
 * Structure used by the back track matcher.
 * These fields are only to be used in regexp.c!
 * See regexp.c for an explanation.
 */
قسمتعریف ساخت
{
    // These four members implement regprog_T
    regengine_T		*engine;
    غیرعلامتی		regflags;
    غیرعلامتی		re_engine;
    غیرعلامتی		re_flags;
    صحیح			re_اندر_use;

    صحیح			regstart;
    char_u		reganch;
    char_u		*regmust;
    صحیح			regmlen;
#اگر_تعریف_شدہ DEBUG
    صحیح			regsz;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SYN_HL
    char_u		reghasz;
#اگر_ختم
    char_u		پروگرام[1];	// actually longer..
} bt_regprog_T;

/*
 * Structure representing a NFA state.
 * An NFA state may have no outgoing edge, when it is a NFA_MATCH state.
 */
قسمتعریف ساخت nfa_state nfa_state_T;
ساخت nfa_state
{
    صحیح			c;
    nfa_state_T		*برآمد;
    nfa_state_T		*out1;
    صحیح			id;
    صحیح			lastlist[2]; // 0: normal, 1: recursive
    صحیح			val;
};

/*
 * Structure used by the NFA matcher.
 */
قسمتعریف ساخت
{
    // These three members implement regprog_T
    regengine_T		*engine;
    غیرعلامتی		regflags;
    غیرعلامتی		re_engine;
    غیرعلامتی		re_flags;
    صحیح			re_اندر_use;

    nfa_state_T		*آغاز;		// points into state[]

    صحیح			reganch;	// pattern starts with ^
    صحیح			regstart;	// char at start of pattern
    char_u		*مطابقت_متن;	// plain text to match with

    صحیح			has_zend;	// pattern contains \ze
    صحیح			has_backref;	// pattern contains \1 .. \9
#اگر_تعریف_شدہ FEAT_SYN_HL
    صحیح			reghasz;
#اگر_ختم
    char_u		*نمونہ;
    صحیح			nsubexp;	// number of ()
    صحیح			nstate;
    خالی		*listbuf[2];	// cached list buffers for
					// nfa_regmatch()
    nfa_state_T		state[1];	// actually longer..
} nfa_regprog_T;

/*
 * Structure to be used for single-line matching.
 * Sub-match "no" starts at "startp[no]" and ends just before "endp[no]".
 * When there is no match, the pointer is NULL.
 */
قسمتعریف ساخت
{
    regprog_T		*regprog;
    char_u		*startp[NSUBEXP];
    char_u		*endp[NSUBEXP];

    colnr_T		rm_matchcol;   // match start without "\zs"
    صحیح			rm_ic;
} regmatch_T;

/*
 * Structure to be used for multi-line matching.
 * Sub-match "no" starts in line "startpos[no].lnum" column "startpos[no].col"
 * and ends in line "endpos[no].lnum" just before column "endpos[no].col".
 * The line numbers are relative to the first line, thus startpos[0].lnum is
 * always 0.
 * When there is no match, the line number is -1.
 */
قسمتعریف ساخت
{
    regprog_T		*regprog;
    lpos_T		startpos[NSUBEXP];
    lpos_T		endpos[NSUBEXP];

    colnr_T		rmm_matchcol;   // match start without "\zs"
    صحیح			rmm_ic;
    colnr_T		rmm_maxcol;	// when not zero: maximum column
} regmmatch_T;

/*
 * Structure used to store external references: "\z\(\)" to "\z\1".
 * Use a reference count to avoid the need to copy this around.  When it goes
 * from 1 to zero the matches need to be freed.
 */
قسمتعریف ساخت
{
    مختصر		refcnt;
    char_u		*matches[NSUBEXP];
} reg_extmatch_T;

ساخت regengine
{
    // bt_regcomp or nfa_regcomp
    regprog_T	*(*regcomp)(char_u*, صحیح);
    // bt_regfree or nfa_regfree
    خالی	(*regfree)(regprog_T *);
    // bt_regexec_nl or nfa_regexec_nl
    صحیح		(*regexec_nl)(regmatch_T *, char_u *, colnr_T, صحیح);
    // bt_regexec_mult or nfa_regexec_mult
    طویل	(*regexec_multi)(regmmatch_T *, win_T *, buf_T *, linenr_T, colnr_T, صحیح *);
#اگر_تعریف_شدہ DEBUG
    char_u	*اظہاریہ;
#اگر_ختم
};

// Flags used by vim_regsub() and vim_regsub_both()
#تعریف_کرو REGSUB_COPY	    1
#تعریف_کرو REGSUB_MAGIC	    2
#تعریف_کرو REGSUB_BACKSLASH    4

#اگر_ختم	// _REGEXP_H
