/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * syntax.c: code for syntax highlighting
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_SYN_HL)

// different types of offsets that are possible
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPO_MS_OFF	0	// match  start offset
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPO_ME_OFF	1	// match  end	offset
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPO_HS_OFF	2	// highl. start offset
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPO_HE_OFF	3	// highl. end	offset
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPO_RS_OFF	4	// region start offset
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPO_RE_OFF	5	// region end	offset
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPO_LC_OFF	6	// leading context offset
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPO_ਗਿਣਤੀ	7

ਸਥਿਰ ਅੱਖਰ *(spo_name_tab[SPO_ਗਿਣਤੀ]) =
	    {"ms=", "me=", "hs=", "he=", "rs=", "re=", "lc="};

/*
 * The patterns that are being searched for are stored in a syn_pattern.
 * A match item consists of one pattern.
 * A start/end item consists of n start patterns and m end patterns.
 * A start/skip/end item consists of n start patterns, one skip pattern and m
 * end patterns.
 * For the latter two, the patterns are always consecutive: start-skip-end.
 *
 * A character offset can be given for the matched text (_m_start and _m_end)
 * and for the actually highlighted text (_h_start and _h_end).
 *
 * Note that ordering of members is optimized to reduce padding.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ syn_ਨਮੂਨਾ
{
    ਅੱਖਰ	 sp_type;		// see SPTYPE_ defines below
    ਅੱਖਰ	 sp_syncing;		// this item used for syncing
    ਛੋਟਾ	 sp_syn_ਮੇਲ_id;	// highlight group ID of pattern
    ਛੋਟਾ	 sp_off_flags;		// see below
    ਪੂਰਨ		 sp_offsets[SPO_ਗਿਣਤੀ];	// offsets
    ਪੂਰਨ		 sp_flags;		// see HL_ defines below
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    ਪੂਰਨ		 sp_cchar;		// conceal substitute character
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		 sp_ic;			// ignore-case flag for sp_prog
    ਪੂਰਨ		 sp_sync_idx;		// sync item index (syncing only)
    ਪੂਰਨ		 sp_ਸਤਰ_id;		// ID of last line where tried
    ਪੂਰਨ		 sp_startcol;		// next match in sp_line_id line
    ਛੋਟਾ	*sp_cont_list;		// cont. group IDs, if non-zero
    ਛੋਟਾ	*sp_ਅਗਲਾ_list;		// next group IDs, if non-zero
    ਬਣਤਰ sp_syn sp_syn;		// struct passed to in_id_list()
    char_u	*sp_ਨਮੂਨਾ;		// regexp to match, pattern
    regprog_T	*sp_prog;		// regexp to match, program
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    syn_ਸਮਾਂ_T	 sp_ਸਮਾਂ;
#ਜੇ_ਅੰਤ
} synpat_T;

// The sp_off_flags are computed like this:
// offset from the start of the matched text: (1 << SPO_XX_OFF)
// offset from the end	 of the matched text: (1 << (SPO_XX_OFF + SPO_COUNT))
// When both are present, only one is used.

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPTYPE_ਮੇਲ	1	// match keyword with this group ID
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPTYPE_ਸ਼ੁਰੂ	2	// match a regexp, start of item
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPTYPE_ਸਿਰਾ	3	// match a regexp, end of item
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPTYPE_SKIP	4	// match a regexp, skip within item


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SYN_ਚੀਜ਼ਾਂ(buf)	((synpat_T *)((buf)->b_syn_patterns.ga_ਅੰਕੜੇ))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NONE_IDX	(-2)	// value of sp_sync_idx for "NONE"

/*
 * Flags for b_syn_sync_flags:
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SF_CCOMMENT	0x01	// sync on a C-style comment
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SF_ਮੇਲ	0x02	// sync by matching a pattern

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SYN_STATE_P(ssp)    ((bufstate_T *)((ssp)->ga_ਅੰਕੜੇ))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAXKEYWLEN	80	    // maximum length of a keyword

/*
 * The attributes of the syntax item that has been recognized.
 */
ਸਥਿਰ ਪੂਰਨ current_attr = 0;	    // attr of current syntax word
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
ਸਥਿਰ ਪੂਰਨ current_id = 0;	    // ID of current char for syn_get_id()
ਸਥਿਰ ਪੂਰਨ current_trans_id = 0;    // idem, transparency removed
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
ਸਥਿਰ ਪੂਰਨ current_flags = 0;
ਸਥਿਰ ਪੂਰਨ current_seqnr = 0;
ਸਥਿਰ ਪੂਰਨ current_sub_char = 0;
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ syn_cluster_S
{
    char_u	    *scl_name;	    // syntax cluster name
    char_u	    *scl_name_u;    // uppercase of scl_name
    ਛੋਟਾ	    *scl_list;	    // IDs in this syntax cluster
} syn_cluster_T;

/*
 * Methods of combining two clusters
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLUSTER_REPLACE	    1	// replace first list with second
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLUSTER_ਜੋੜੋ	    2	// add second list to first
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLUSTER_ਘਟਾਓ    3	// subtract second list from first

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SYN_CLSTR(buf)	((syn_cluster_T *)((buf)->b_syn_clusters.ga_ਅੰਕੜੇ))

/*
 * Syntax group IDs have different types:
 *     0 - 19999  normal syntax groups
 * 20000 - 20999  ALLBUT indicator (current_syn_inc_tag added)
 * 21000 - 21999  TOP indicator (current_syn_inc_tag added)
 * 22000 - 22999  CONTAINED indicator (current_syn_inc_tag added)
 * 23000 - 32767  cluster IDs (subtract SYNID_CLUSTER for the cluster ID)
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SYNID_ALLBUT	MAX_HL_ID   // syntax group ID for contains=ALLBUT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SYNID_ਸਿਖਰ	21000	    // syntax group ID for contains=TOP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SYNID_CONTAINED	22000	    // syntax group ID for contains=CONTAINED
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SYNID_CLUSTER	23000	    // first syntax group ID for clusters

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_SYN_INC_ਟੈਗ	999	    // maximum before the above overflow
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_CLUSTER_ID  (32767 - SYNID_CLUSTER)

/*
 * Annoying Hack(TM):  ":syn include" needs this pointer to pass to
 * expand_filename().  Most of the other syntax commands don't need it, so
 * instead of passing it to them, we stow it here.
 */
ਸਥਿਰ char_u **syn_cmdlinep;

/*
 * Another Annoying Hack(TM):  To prevent rules from other ":syn include"'d
 * files from leaking into ALLBUT lists, we assign a unique ID to the
 * rules in each ":syn include"'d file.
 */
ਸਥਿਰ ਪੂਰਨ current_syn_inc_ਟੈਗ = 0;
ਸਥਿਰ ਪੂਰਨ running_syn_inc_ਟੈਗ = 0;

/*
 * In a hashtable item "hi_key" points to "keyword" in a keyentry.
 * This avoids adding a pointer to the hashtable item.
 * KE2HIKEY() converts a var pointer to a hashitem key pointer.
 * HIKEY2KE() converts a hashitem key pointer to a var pointer.
 * HI2KE() converts a hashitem pointer to a var pointer.
 */
ਸਥਿਰ keyentry_T dumkey;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KE2HIKEY(kp)  ((kp)->keyword)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HIKEY2KE(p)   ((keyentry_T *)((p) - (dumkey.keyword - (char_u *)&dumkey)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HI2KE(hi)      HIKEY2KE((hi)->hi_ਕੁੰਜੀ)

/*
 * To reduce the time spent in keepend(), remember at which level in the state
 * stack the first item with "keepend" is present.  When "-1", there is no
 * "keepend" on the stack.
 */
ਸਥਿਰ ਪੂਰਨ keepend_level = -1;

ਸਥਿਰ ਅੱਖਰ ਸੁਨੇਹਾ_no_ਚੀਜ਼ਾਂ[] = N_("No Syntax items defined for this buffer");

/*
 * For the current state we need to remember more than just the idx.
 * When si_m_endpos.lnum is 0, the items other than si_idx are unknown.
 * (The end positions have the column number of the next char)
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ state_ਵਸਤੂ
{
    ਪੂਰਨ		si_idx;			// index of syntax pattern or
					// KEYWORD_IDX
    ਪੂਰਨ		si_id;			// highlight group ID for keywords
    ਪੂਰਨ		si_trans_id;		// idem, transparency removed
    ਪੂਰਨ		si_m_lnum;		// lnum of the match
    ਪੂਰਨ		si_m_startcol;		// starting column of the match
    lpos_T	si_m_endpos;		// just after end posn of the match
    lpos_T	si_h_startpos;		// start position of the highlighting
    lpos_T	si_h_endpos;		// end position of the highlighting
    lpos_T	si_eoe_pos;		// end position of end pattern
    ਪੂਰਨ		si_ਸਿਰਾ_idx;		// group ID for end pattern or zero
    ਪੂਰਨ		si_ਸਿਰੇ;		// if match ends before si_m_endpos
    ਪੂਰਨ		si_attr;		// attributes in this state
    ਲੰਮਾ	si_flags;		// HL_HAS_EOL flag in this state, and
					// HL_SKIP* for si_next_list
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    ਪੂਰਨ		si_seqnr;		// sequence number
    ਪੂਰਨ		si_cchar;		// substitution character for conceal
#ਜੇ_ਅੰਤ
    ਛੋਟਾ	*si_cont_list;		// list of contained groups
    ਛੋਟਾ	*si_ਅਗਲਾ_list;		// nextgroup IDs after this item ends
    reg_extmatch_T *si_extmatch;	// \z(...\) matches from start
					//ਨਮੂਨਾ
} stateitem_T;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KEYWORD_IDX	(-1)	    // value of si_idx for keywords
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ID_LIST_ALL	((ਛੋਟਾ *)-1) // valid of si_cont_list for containing all
				    // but contained groups

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
ਸਥਿਰ ਪੂਰਨ ਅਗਲਾ_seqnr = 1;		// value to use for si_seqnr
#ਜੇ_ਅੰਤ

/*
 * Struct to reduce the number of arguments to get_syn_options(), it's used
 * very often.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ		flags;		// flags for contained and transparent
    ਪੂਰਨ		keyword;	// TRUE for ":syn keyword"
    ਪੂਰਨ		*sync_idx;	// syntax item for "grouphere" argument, NULL
				// if not allowed
    ਅੱਖਰ	has_cont_list;	// TRUE if "cont_list" can be used
    ਛੋਟਾ	*cont_list;	// group IDs for "contains" argument
    ਛੋਟਾ	*cont_ਵਿੱਚ_list;	// group IDs for "containedin" argument
    ਛੋਟਾ	*ਅਗਲਾ_list;	// group IDs for "nextgroup" argument
} syn_opt_arg_T;

/*
 * The next possible match in the current line for any pattern is remembered,
 * to avoid having to try for a match in each column.
 * If next_match_idx == -1, not tried (in this line) yet.
 * If next_match_col == MAXCOL, no match found in this line.
 * (All end positions have the column of the char after the end)
 */
ਸਥਿਰ ਪੂਰਨ ਅਗਲਾ_ਮੇਲ_ਖਾਨਾ;		// column for start of next match
ਸਥਿਰ lpos_T ਅਗਲਾ_ਮੇਲ_m_endpos;	// position for end of next match
ਸਥਿਰ lpos_T ਅਗਲਾ_ਮੇਲ_h_startpos;	// pos. for highl. start of next match
ਸਥਿਰ lpos_T ਅਗਲਾ_ਮੇਲ_h_endpos;	// pos. for highl. end of next match
ਸਥਿਰ ਪੂਰਨ ਅਗਲਾ_ਮੇਲ_idx;		// index of matched item
ਸਥਿਰ ਲੰਮਾ ਅਗਲਾ_ਮੇਲ_flags;		// flags for next match
ਸਥਿਰ lpos_T ਅਗਲਾ_ਮੇਲ_eos_pos;	// end of start pattn (start region)
ਸਥਿਰ lpos_T ਅਗਲਾ_ਮੇਲ_eoe_pos;	// pos. for end of end pattern
ਸਥਿਰ ਪੂਰਨ ਅਗਲਾ_ਮੇਲ_ਸਿਰਾ_idx;		// ID of group for end pattn or zero
ਸਥਿਰ reg_extmatch_T *ਅਗਲਾ_ਮੇਲ_extmatch = ਨਿਰਾ;

/*
 * A state stack is an array of integers or stateitem_T, stored in a
 * garray_T.  A state stack is invalid if its itemsize entry is zero.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INVALID_STATE(ssp)  ((ssp)->ga_itemsize == 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VALID_STATE(ssp)    ((ssp)->ga_itemsize != 0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOR_ALL_SYNSTATES(sb, sst) \
    ਲਈ ((sst) = (sb)->b_sst_ਪਹਿਲਾ; (sst) != ਨਿਰਾ; (sst) = (sst)->sst_ਅਗਲਾ)

/*
 * The current state (within the line) of the recognition engine.
 * When current_state.ga_itemsize is 0 the current state is invalid.
 */
ਸਥਿਰ win_T	*syn_win;		// current window for highlighting
ਸਥਿਰ buf_T	*syn_buf;		// current buffer for highlighting
ਸਥਿਰ synblock_T *syn_ਖੰਡ;		// current buffer for highlighting
ਸਥਿਰ linenr_T current_lnum = 0;	// lnum of current state
ਸਥਿਰ colnr_T	current_ਖਾਨਾ = 0;	// column of current state
ਸਥਿਰ ਪੂਰਨ	current_state_stored = 0; // TRUE if stored current state
					  // after setting current_finished
ਸਥਿਰ ਪੂਰਨ	current_ਮੁਕੰਮਲ = 0;	// current line has been finished
ਸਥਿਰ garray_T current_state		// current stack of state_items
		= {0, 0, 0, 0, ਨਿਰਾ};
ਸਥਿਰ ਛੋਟਾ	*current_ਅਗਲਾ_list = ਨਿਰਾ; // when non-zero, nextgroup list
ਸਥਿਰ ਪੂਰਨ	current_ਅਗਲਾ_flags = 0; // flags for current_next_list
ਸਥਿਰ ਪੂਰਨ	current_ਸਤਰ_id = 0;	// unique number for current line

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CUR_STATE(idx)	((stateitem_T *)(current_state.ga_ਅੰਕੜੇ))[idx]

ਸਥਿਰ ਖਾਲੀ syn_sync(win_T *wp, linenr_T lnum, synstate_T *ਆਖਰੀ_valid);
ਸਥਿਰ ਪੂਰਨ syn_ਮੇਲ_linecont(linenr_T lnum);
ਸਥਿਰ ਖਾਲੀ syn_ਸ਼ੁਰੂ_ਸਤਰ(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ syn_update_ਸਿਰੇ(ਪੂਰਨ startofline);
ਸਥਿਰ ਖਾਲੀ syn_ਢੇਰ_alloc(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ syn_ਢੇਰ_cleanup(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ syn_ਢੇਰ_free_ਇੰਦਰਾਜ(synblock_T *ਖੰਡ, synstate_T *p);
ਸਥਿਰ synstate_T *syn_ਢੇਰ_ਲੱਭੋ_ਇੰਦਰਾਜ(linenr_T lnum);
ਸਥਿਰ synstate_T *store_current_state(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ ਲੱਦੋ_current_state(synstate_T *from);
ਸਥਿਰ ਖਾਲੀ invalidate_current_state(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ syn_ਢੇਰ_equal(synstate_T *sp);
ਸਥਿਰ ਖਾਲੀ validate_current_state(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ syn_ਅੰਤ_ਸਤਰ(ਪੂਰਨ syncing);
ਸਥਿਰ ਪੂਰਨ syn_current_attr(ਪੂਰਨ syncing, ਪੂਰਨ displaying, ਪੂਰਨ *can_spell, ਪੂਰਨ keep_state);
ਸਥਿਰ ਪੂਰਨ did_ਮੇਲ_already(ਪੂਰਨ idx, garray_T *gap);
ਸਥਿਰ stateitem_T *ਧੱਕੋ_ਅਗਲਾ_ਮੇਲ(stateitem_T *cur_si);
ਸਥਿਰ ਖਾਲੀ check_state_ਸਿਰੇ(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ update_si_attr(ਪੂਰਨ idx);
ਸਥਿਰ ਖਾਲੀ check_keepend(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ update_si_ਸਿਰਾ(stateitem_T *sip, ਪੂਰਨ startcol, ਪੂਰਨ force);
ਸਥਿਰ ਛੋਟਾ *copy_id_list(ਛੋਟਾ *list);
ਸਥਿਰ ਪੂਰਨ ਵਿੱਚ_id_list(stateitem_T *ਵਸਤੂ, ਛੋਟਾ *cont_list, ਬਣਤਰ sp_syn *ssp, ਪੂਰਨ flags);
ਸਥਿਰ ਪੂਰਨ ਧੱਕੋ_current_state(ਪੂਰਨ idx);
ਸਥਿਰ ਖਾਲੀ ਕੱਢੋ_current_state(ਖਾਲੀ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
ਸਥਿਰ ਖਾਲੀ syn_clear_ਸਮਾਂ(syn_ਸਮਾਂ_T *tt);
ਸਥਿਰ ਖਾਲੀ syntime_clear(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ syntime_report(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ syn_ਸਮਾਂ_on = FALSE;
# define IF_SYN_ਸਮਾਂ(p) (p)
#ਨਹੀਂਤਾਂ
# define IF_SYN_ਸਮਾਂ(p) ਨਿਰਾ
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ syn_ਸਮਾਂ_T;
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ syn_ਢੇਰ_apply_changes_ਖੰਡ(synblock_T *ਖੰਡ, buf_T *buf);
ਸਥਿਰ ਖਾਲੀ ਲੱਭੋ_endpos(ਪੂਰਨ idx, lpos_T *startpos, lpos_T *m_endpos, lpos_T *hl_endpos, ਲੰਮਾ *flagsp, lpos_T *ਸਿਰਾ_endpos, ਪੂਰਨ *ਸਿਰਾ_idx, reg_extmatch_T *ਸ਼ੁਰੂ_ext);

ਸਥਿਰ ਖਾਲੀ ਹੱਦਬੰਦੀ_pos(lpos_T *pos, lpos_T *ਹੱਦਬੰਦੀ);
ਸਥਿਰ ਖਾਲੀ ਹੱਦਬੰਦੀ_pos_zero(lpos_T *pos, lpos_T *ਹੱਦਬੰਦੀ);
ਸਥਿਰ ਖਾਲੀ syn_ਜੋੜੋ_ਸਿਰਾ_off(lpos_T *ਨਤੀਜਾ, regmmatch_T *regmatch, synpat_T *spp, ਪੂਰਨ idx, ਪੂਰਨ extra);
ਸਥਿਰ ਖਾਲੀ syn_ਜੋੜੋ_ਸ਼ੁਰੂ_off(lpos_T *ਨਤੀਜਾ, regmmatch_T *regmatch, synpat_T *spp, ਪੂਰਨ idx, ਪੂਰਨ extra);
ਸਥਿਰ char_u *syn_getcurline(ਖਾਲੀ);
ਸਥਿਰ colnr_T syn_getcurline_len(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ syn_regexec(regmmatch_T *rmp, linenr_T lnum, colnr_T ਖਾਨਾ, syn_ਸਮਾਂ_T *st);
ਸਥਿਰ ਪੂਰਨ check_keyword_id(char_u *ਸਤਰ, ਪੂਰਨ startcol, ਪੂਰਨ *endcol, ਲੰਮਾ *flags, ਛੋਟਾ **ਅਗਲਾ_list, stateitem_T *cur_si, ਪੂਰਨ *ccharp);
ਸਥਿਰ ਖਾਲੀ syn_remove_ਨਮੂਨਾ(synblock_T *ਖੰਡ, ਪੂਰਨ idx);
ਸਥਿਰ ਖਾਲੀ syn_clear_ਨਮੂਨਾ(synblock_T *ਖੰਡ, ਪੂਰਨ i);
ਸਥਿਰ ਖਾਲੀ syn_clear_cluster(synblock_T *ਖੰਡ, ਪੂਰਨ i);
ਸਥਿਰ ਖਾਲੀ syn_clear_one(ਪੂਰਨ id, ਪੂਰਨ syncing);
ਸਥਿਰ ਖਾਲੀ syn_cmd_onoff(exarg_T *eap, ਅੱਖਰ *name);
ਸਥਿਰ ਖਾਲੀ syn_ਸਤਰਾਂ_ਸੁਨੇਹਾ(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ syn_ਮੇਲ_ਸੁਨੇਹਾ(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ syn_list_one(ਪੂਰਨ id, ਪੂਰਨ syncing, ਪੂਰਨ link_only);
ਸਥਿਰ ਖਾਲੀ syn_list_cluster(ਪੂਰਨ id);
ਸਥਿਰ ਖਾਲੀ put_id_list(char_u *name, ਛੋਟਾ *list, ਪੂਰਨ attr);
ਸਥਿਰ ਖਾਲੀ put_ਨਮੂਨਾ(ਅੱਖਰ *s, ਪੂਰਨ c, synpat_T *spp, ਪੂਰਨ attr);
ਸਥਿਰ ਪੂਰਨ syn_list_keywords(ਪੂਰਨ id, hashtab_T *ht, ਪੂਰਨ did_ਸਿਰਲੇਖ, ਪੂਰਨ attr);
ਸਥਿਰ ਖਾਲੀ syn_clear_keyword(ਪੂਰਨ id, hashtab_T *ht);
ਸਥਿਰ ਖਾਲੀ clear_keywtab(hashtab_T *ht);
ਸਥਿਰ ਪੂਰਨ syn_scl_namen2id(char_u *linep, ਪੂਰਨ len);
ਸਥਿਰ ਪੂਰਨ syn_check_cluster(char_u *pp, ਪੂਰਨ len);
ਸਥਿਰ ਪੂਰਨ syn_ਜੋੜੋ_cluster(char_u *name);
ਸਥਿਰ ਖਾਲੀ init_syn_patterns(ਖਾਲੀ);
ਸਥਿਰ char_u *get_syn_ਨਮੂਨਾ(char_u *arg, synpat_T *ci);
ਸਥਿਰ ਪੂਰਨ get_id_list(char_u **arg, ਪੂਰਨ keylen, ਛੋਟਾ **list, ਪੂਰਨ skip);
ਸਥਿਰ ਖਾਲੀ syn_combine_list(ਛੋਟਾ **clstr1, ਛੋਟਾ **clstr2, ਪੂਰਨ list_op);
ਸਥਿਰ ਖਾਲੀ idl_cache_clear(synblock_T *ਖੰਡ);

/*
 * Start the syntax recognition for a line.  This function is normally called
 * from the screen updating, once for each displayed line.
 * The buffer is remembered in syn_buf, because get_syntax_attr() doesn't get
 * it.	Careful: curbuf and curwin are likely to point to another buffer and
 * window.
 */
    ਖਾਲੀ
syntax_ਸ਼ੁਰੂ(win_T *wp, linenr_T lnum)
{
    synstate_T	*p;
    synstate_T	*ਆਖਰੀ_valid = ਨਿਰਾ;
    synstate_T	*ਆਖਰੀ_min_valid = ਨਿਰਾ;
    synstate_T	*sp, *prev = ਨਿਰਾ;
    linenr_T	parsed_lnum;
    linenr_T	ਪਹਿਲਾ_stored;
    ਪੂਰਨ		dist;
    ਸਥਿਰ varnumber_T changedtick = 0;	// remember the last change ID

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    current_sub_char = NUL;
#ਜੇ_ਅੰਤ

    /*
     * After switching buffers, invalidate current_state.
     * Also do this when a change was made, the current state may be invalid
     * then.
     */
    ਜੇ (syn_ਖੰਡ != wp->w_s
	    || syn_buf != wp->w_ਬਫਰ
	    || changedtick != CHANGEDTICK(syn_buf))
    {
	invalidate_current_state();
	syn_buf = wp->w_ਬਫਰ;
	syn_ਖੰਡ = wp->w_s;
    }
    changedtick = CHANGEDTICK(syn_buf);
    syn_win = wp;

    /*
     * Allocate syntax stack when needed.
     */
    syn_ਢੇਰ_alloc();
    ਜੇ (syn_ਖੰਡ->b_sst_array == ਨਿਰਾ)
	ਵਾਪਸ;		// out of memory
    syn_ਖੰਡ->b_sst_lasttick = display_tick;

    /*
     * If the state of the end of the previous line is useful, store it.
     */
    ਜੇ (VALID_STATE(&current_state)
	    && current_lnum < lnum
	    && current_lnum < syn_buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	(ਖਾਲੀ)syn_ਅੰਤ_ਸਤਰ(FALSE);
	ਜੇ (!current_state_stored)
	{
	    ++current_lnum;
	    (ਖਾਲੀ)store_current_state();
	}

	/*
	 * If the current_lnum is now the same as "lnum", keep the current
	 * state (this happens very often!).  Otherwise invalidate
	 * current_state and figure it out below.
	 */
	ਜੇ (current_lnum != lnum)
	    invalidate_current_state();
    }
    ਨਹੀਂਤਾਂ
	invalidate_current_state();

    /*
     * Try to synchronize from a saved state in b_sst_array[].
     * Only do this if lnum is not before and not to far beyond a saved state.
     */
    ਜੇ (INVALID_STATE(&current_state) && syn_ਖੰਡ->b_sst_array != ਨਿਰਾ)
    {
	// Find last valid saved state before start_lnum.
	FOR_ALL_SYNSTATES(syn_ਖੰਡ, p)
	{
	    ਜੇ (p->sst_lnum > lnum)
		ਰੋਕੋ;
	    ਜੇ (p->sst_lnum <= lnum && p->sst_change_lnum == 0)
	    {
		ਆਖਰੀ_valid = p;
		ਜੇ (p->sst_lnum >= lnum - syn_ਖੰਡ->b_syn_sync_minlines)
		    ਆਖਰੀ_min_valid = p;
	    }
	}
	ਜੇ (ਆਖਰੀ_min_valid != ਨਿਰਾ)
	    ਲੱਦੋ_current_state(ਆਖਰੀ_min_valid);
    }

    /*
     * If "lnum" is before or far beyond a line with a saved state, need to
     * re-synchronize.
     */
    ਜੇ (INVALID_STATE(&current_state))
    {
	syn_sync(wp, lnum, ਆਖਰੀ_valid);
	ਜੇ (current_lnum == 1)
	    // First line is always valid, no matter "minlines".
	    ਪਹਿਲਾ_stored = 1;
	ਨਹੀਂਤਾਂ
	    // Need to parse "minlines" lines before state can be considered
	    // valid to store.
	    ਪਹਿਲਾ_stored = current_lnum + syn_ਖੰਡ->b_syn_sync_minlines;
    }
    ਨਹੀਂਤਾਂ
	ਪਹਿਲਾ_stored = current_lnum;

    /*
     * Advance from the sync point or saved state until the current line.
     * Save some entries for syncing with later on.
     */
    ਜੇ (syn_ਖੰਡ->b_sst_len <= ਪੰਕਤੀਆਂ)
	dist = 999999;
    ਨਹੀਂਤਾਂ
	dist = syn_buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ / (syn_ਖੰਡ->b_sst_len - ਪੰਕਤੀਆਂ) + 1;
    ਜਦੋਂ (current_lnum < lnum)
    {
	syn_ਸ਼ੁਰੂ_ਸਤਰ();
	(ਖਾਲੀ)syn_ਅੰਤ_ਸਤਰ(FALSE);
	++current_lnum;

	// If we parsed at least "minlines" lines or started at a valid
	// state, the current state is considered valid.
	ਜੇ (current_lnum >= ਪਹਿਲਾ_stored)
	{
	    // Check if the saved state entry is for the current line and is
	    // equal to the current state.  If so, then validate all saved
	    // states that depended on a change before the parsed line.
	    ਜੇ (prev == ਨਿਰਾ)
		prev = syn_ਢੇਰ_ਲੱਭੋ_ਇੰਦਰਾਜ(current_lnum - 1);
	    ਜੇ (prev == ਨਿਰਾ)
		sp = syn_ਖੰਡ->b_sst_ਪਹਿਲਾ;
	    ਨਹੀਂਤਾਂ
		sp = prev;
	    ਜਦੋਂ (sp != ਨਿਰਾ && sp->sst_lnum < current_lnum)
		sp = sp->sst_ਅਗਲਾ;
	    ਜੇ (sp != ਨਿਰਾ
		    && sp->sst_lnum == current_lnum
		    && syn_ਢੇਰ_equal(sp))
	    {
		parsed_lnum = current_lnum;
		prev = sp;
		ਜਦੋਂ (sp != ਨਿਰਾ && sp->sst_change_lnum <= parsed_lnum)
		{
		    ਜੇ (sp->sst_lnum <= lnum)
			// valid state before desired line, use this one
			prev = sp;
		    ਨਹੀਂਤਾਂ ਜੇ (sp->sst_change_lnum == 0)
			// past saved states depending on change, break here.
			ਰੋਕੋ;
		    sp->sst_change_lnum = 0;
		    sp = sp->sst_ਅਗਲਾ;
		}
		ਲੱਦੋ_current_state(prev);
	    }
	    // Store the state at this line when it's the first one, the line
	    // where we start parsing, or some distance from the previously
	    // saved state.  But only when parsed at least 'minlines'.
	    ਨਹੀਂਤਾਂ ਜੇ (prev == ਨਿਰਾ
			|| current_lnum == lnum
			|| current_lnum >= prev->sst_lnum + dist)
		prev = store_current_state();
	}

	// This can take a long time: break when CTRL-C pressed.  The current
	// state will be wrong then.
	ਸਤਰ_breakcheck();
	ਜੇ (got_int)
	{
	    current_lnum = lnum;
	    ਰੋਕੋ;
	}
    }

    syn_ਸ਼ੁਰੂ_ਸਤਰ();
}

/*
 * We cannot simply discard growarrays full of state_items or buf_states; we
 * have to manually release their extmatch pointers first.
 */
    ਸਥਿਰ ਖਾਲੀ
clear_syn_state(synstate_T *p)
{
    ਪੂਰਨ		i;
    garray_T	*gap;

    ਜੇ (p->sst_stacksize > SST_FIX_ਹਾਲਤਾਂ)
    {
	gap = &(p->sst_union.sst_ga);
	ਲਈ (i = 0; i < gap->ga_len; i++)
	    unref_extmatch(SYN_STATE_P(gap)[i].bs_extmatch);
	ga_clear(gap);
    }
    ਨਹੀਂਤਾਂ
    {
	ਲਈ (i = 0; i < p->sst_stacksize; i++)
	    unref_extmatch(p->sst_union.sst_ਢੇਰ[i].bs_extmatch);
    }
}

/*
 * Cleanup the current_state stack.
 */
    ਸਥਿਰ ਖਾਲੀ
clear_current_state(ਖਾਲੀ)
{
    ਪੂਰਨ		i;
    stateitem_T	*sip;

    sip = (stateitem_T *)(current_state.ga_ਅੰਕੜੇ);
    ਲਈ (i = 0; i < current_state.ga_len; i++)
	unref_extmatch(sip[i].si_extmatch);
    ga_clear(&current_state);
}

/*
 * Try to find a synchronisation point for line "lnum".
 *
 * This sets current_lnum and the current state.  One of three methods is
 * used:
 * 1. Search backwards for the end of a C-comment.
 * 2. Search backwards for given sync patterns.
 * 3. Simply start on a given number of lines above "lnum".
 */
    ਸਥਿਰ ਖਾਲੀ
syn_sync(
    win_T	*wp,
    linenr_T	ਸ਼ੁਰੂ_lnum,
    synstate_T	*ਆਖਰੀ_valid)
{
    buf_T	*curbuf_save;
    win_T	*curwin_save;
    pos_T	cursor_save;
    ਪੂਰਨ		idx;
    linenr_T	lnum;
    linenr_T	ਸਿਰਾ_lnum;
    linenr_T	break_lnum;
    ਪੂਰਨ		had_sync_ਬਿੰਦੂ;
    stateitem_T	*cur_si;
    synpat_T	*spp;
    ਪੂਰਨ		ਮਿਲਿਆ_flags = 0;
    ਪੂਰਨ		ਮਿਲਿਆ_ਮੇਲ_idx = 0;
    linenr_T	ਮਿਲਿਆ_current_lnum = 0;
    ਪੂਰਨ		ਮਿਲਿਆ_current_ਖਾਨਾ = 0;
    lpos_T	ਮਿਲਿਆ_m_endpos;
    colnr_T	prev_current_ਖਾਨਾ;

    /*
     * Clear any current state that might be hanging around.
     */
    invalidate_current_state();

    /*
     * Start at least "minlines" back.  Default starting point for parsing is
     * there.
     * Start further back, to avoid that scrolling backwards will result in
     * resyncing for every line.  Now it resyncs only one out of N lines,
     * where N is minlines * 1.5, or minlines * 2 if minlines is small.
     * Watch out for overflow when minlines is MAXLNUM.
     */
    ਜੇ (syn_ਖੰਡ->b_syn_sync_minlines > ਸ਼ੁਰੂ_lnum)
	ਸ਼ੁਰੂ_lnum = 1;
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (syn_ਖੰਡ->b_syn_sync_minlines == 1)
	    lnum = 1;
	ਨਹੀਂਤਾਂ ਜੇ (syn_ਖੰਡ->b_syn_sync_minlines < 10)
	    lnum = syn_ਖੰਡ->b_syn_sync_minlines * 2;
	ਨਹੀਂਤਾਂ
	    lnum = syn_ਖੰਡ->b_syn_sync_minlines * 3 / 2;
	ਜੇ (syn_ਖੰਡ->b_syn_sync_maxlines != 0
				     && lnum > syn_ਖੰਡ->b_syn_sync_maxlines)
	    lnum = syn_ਖੰਡ->b_syn_sync_maxlines;
	ਜੇ (lnum >= ਸ਼ੁਰੂ_lnum)
	    ਸ਼ੁਰੂ_lnum = 1;
	ਨਹੀਂਤਾਂ
	    ਸ਼ੁਰੂ_lnum -= lnum;
    }
    current_lnum = ਸ਼ੁਰੂ_lnum;

    /*
     * 1. Search backwards for the end of a C-style comment.
     */
    ਜੇ (syn_ਖੰਡ->b_syn_sync_flags & SF_CCOMMENT)
    {
	// Need to make syn_buf the current buffer for a moment, to be able to
	// use find_start_comment().
	curwin_save = curwin;
	curwin = wp;
	curbuf_save = curbuf;
	curbuf = syn_buf;

	/*
	 * Skip lines that end in a backslash.
	 */
	ਲਈ ( ; ਸ਼ੁਰੂ_lnum > 1; --ਸ਼ੁਰੂ_lnum)
	{
	    char_u	*l = ml_get(ਸ਼ੁਰੂ_lnum - 1);

	    ਜੇ (*l == NUL || *(l + ml_get_len(ਸ਼ੁਰੂ_lnum - 1) - 1) != '\\')
		ਰੋਕੋ;
	}
	current_lnum = ਸ਼ੁਰੂ_lnum;

	// set cursor to start of search
	cursor_save = wp->w_cursor;
	wp->w_cursor.lnum = ਸ਼ੁਰੂ_lnum;
	wp->w_cursor.ਖਾਨਾ = 0;

	/*
	 * If the line is inside a comment, need to find the syntax item that
	 * defines the comment.
	 * Restrict the search for the end of a comment to b_syn_sync_maxlines.
	 */
	ਜੇ (ਲੱਭੋ_ਸ਼ੁਰੂ_comment((ਪੂਰਨ)syn_ਖੰਡ->b_syn_sync_maxlines) != ਨਿਰਾ)
	{
	    ਲਈ (idx = syn_ਖੰਡ->b_syn_patterns.ga_len; --idx >= 0; )
		ਜੇ (SYN_ਚੀਜ਼ਾਂ(syn_ਖੰਡ)[idx].sp_syn.id
						   == syn_ਖੰਡ->b_syn_sync_id
			&& SYN_ਚੀਜ਼ਾਂ(syn_ਖੰਡ)[idx].sp_type == SPTYPE_ਸ਼ੁਰੂ)
		{
		    validate_current_state();
		    ਜੇ (ਧੱਕੋ_current_state(idx) == OK)
			update_si_attr(current_state.ga_len - 1);
		    ਰੋਕੋ;
		}
	}

	// restore cursor and buffer
	wp->w_cursor = cursor_save;
	curwin = curwin_save;
	curbuf = curbuf_save;
    }

    /*
     * 2. Search backwards for given sync patterns.
     */
    ਨਹੀਂਤਾਂ ਜੇ (syn_ਖੰਡ->b_syn_sync_flags & SF_ਮੇਲ)
    {
	ਜੇ (syn_ਖੰਡ->b_syn_sync_maxlines != 0
			       && ਸ਼ੁਰੂ_lnum > syn_ਖੰਡ->b_syn_sync_maxlines)
	    break_lnum = ਸ਼ੁਰੂ_lnum - syn_ਖੰਡ->b_syn_sync_maxlines;
	ਨਹੀਂਤਾਂ
	    break_lnum = 0;

	ਮਿਲਿਆ_m_endpos.lnum = 0;
	ਮਿਲਿਆ_m_endpos.ਖਾਨਾ = 0;
	ਸਿਰਾ_lnum = ਸ਼ੁਰੂ_lnum;
	lnum = ਸ਼ੁਰੂ_lnum;
	ਜਦੋਂ (--lnum > break_lnum)
	{
	    // This can take a long time: break when CTRL-C pressed.
	    ਸਤਰ_breakcheck();
	    ਜੇ (got_int)
	    {
		invalidate_current_state();
		current_lnum = ਸ਼ੁਰੂ_lnum;
		ਰੋਕੋ;
	    }

	    // Check if we have run into a valid saved state stack now.
	    ਜੇ (ਆਖਰੀ_valid != ਨਿਰਾ && lnum == ਆਖਰੀ_valid->sst_lnum)
	    {
		ਲੱਦੋ_current_state(ਆਖਰੀ_valid);
		ਰੋਕੋ;
	    }

	    /*
	     * Check if the previous line has the line-continuation pattern.
	     */
	    ਜੇ (lnum > 1 && syn_ਮੇਲ_linecont(lnum - 1))
		ਜਾਰੀ;

	    /*
	     * Start with nothing on the state stack
	     */
	    validate_current_state();

	    ਲਈ (current_lnum = lnum; current_lnum < ਸਿਰਾ_lnum; ++current_lnum)
	    {
		syn_ਸ਼ੁਰੂ_ਸਤਰ();
		ਲਈ (;;)
		{
		    had_sync_ਬਿੰਦੂ = syn_ਅੰਤ_ਸਤਰ(TRUE);
		    /*
		     * When a sync point has been found, remember where, and
		     * continue to look for another one, further on in the line.
		     */
		    ਜੇ (had_sync_ਬਿੰਦੂ && current_state.ga_len)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			ਜੇ (cur_si->si_m_endpos.lnum > ਸ਼ੁਰੂ_lnum)
			{
			    // ignore match that goes to after where started
			    current_lnum = ਸਿਰਾ_lnum;
			    ਰੋਕੋ;
			}
			ਜੇ (cur_si->si_idx < 0)
			{
			    // Cannot happen?
			    ਮਿਲਿਆ_flags = 0;
			    ਮਿਲਿਆ_ਮੇਲ_idx = KEYWORD_IDX;
			}
			ਨਹੀਂਤਾਂ
			{
			    spp = &(SYN_ਚੀਜ਼ਾਂ(syn_ਖੰਡ)[cur_si->si_idx]);
			    ਮਿਲਿਆ_flags = spp->sp_flags;
			    ਮਿਲਿਆ_ਮੇਲ_idx = spp->sp_sync_idx;
			}
			ਮਿਲਿਆ_current_lnum = current_lnum;
			ਮਿਲਿਆ_current_ਖਾਨਾ = current_ਖਾਨਾ;
			ਮਿਲਿਆ_m_endpos = cur_si->si_m_endpos;
			/*
			 * Continue after the match (be aware of a zero-length
			 * match).
			 */
			ਜੇ (ਮਿਲਿਆ_m_endpos.lnum > current_lnum)
			{
			    current_lnum = ਮਿਲਿਆ_m_endpos.lnum;
			    current_ਖਾਨਾ = ਮਿਲਿਆ_m_endpos.ਖਾਨਾ;
			    ਜੇ (current_lnum >= ਸਿਰਾ_lnum)
				ਰੋਕੋ;
			}
			ਨਹੀਂਤਾਂ ਜੇ (ਮਿਲਿਆ_m_endpos.ਖਾਨਾ > current_ਖਾਨਾ)
			    current_ਖਾਨਾ = ਮਿਲਿਆ_m_endpos.ਖਾਨਾ;
			ਨਹੀਂਤਾਂ
			    ++current_ਖਾਨਾ;

			// syn_current_attr() will have skipped the check for
			// an item that ends here, need to do that now.  Be
			// careful not to go past the NUL.
			prev_current_ਖਾਨਾ = current_ਖਾਨਾ;
			ਜੇ (syn_getcurline()[current_ਖਾਨਾ] != NUL)
			    ++current_ਖਾਨਾ;
			check_state_ਸਿਰੇ();
			current_ਖਾਨਾ = prev_current_ਖਾਨਾ;
		    }
		    ਨਹੀਂਤਾਂ
			ਰੋਕੋ;
		}
	    }

	    /*
	     * If a sync point was encountered, break here.
	     */
	    ਜੇ (ਮਿਲਿਆ_flags)
	    {
		/*
		 * Put the item that was specified by the sync point on the
		 * state stack.  If there was no item specified, make the
		 * state stack empty.
		 */
		clear_current_state();
		ਜੇ (ਮਿਲਿਆ_ਮੇਲ_idx >= 0
			&& ਧੱਕੋ_current_state(ਮਿਲਿਆ_ਮੇਲ_idx) == OK)
		    update_si_attr(current_state.ga_len - 1);

		/*
		 * When using "grouphere", continue from the sync point
		 * match, until the end of the line.  Parsing starts at
		 * the next line.
		 * For "groupthere" the parsing starts at start_lnum.
		 */
		ਜੇ (ਮਿਲਿਆ_flags & HL_SYNC_ਇੱਥੇ)
		{
		    current_lnum = ਮਿਲਿਆ_m_endpos.lnum;
		    current_ਖਾਨਾ = ਮਿਲਿਆ_m_endpos.ਖਾਨਾ;
		    ਜੇ (current_state.ga_len)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			cur_si->si_h_startpos.lnum = ਮਿਲਿਆ_current_lnum;
			cur_si->si_h_startpos.ਖਾਨਾ = ਮਿਲਿਆ_current_ਖਾਨਾ;
			update_si_ਸਿਰਾ(cur_si, (ਪੂਰਨ)current_ਖਾਨਾ, TRUE);
			check_keepend();
		    }
		    (ਖਾਲੀ)syn_ਅੰਤ_ਸਤਰ(FALSE);
		    ++current_lnum;
		}
		ਨਹੀਂਤਾਂ
		    current_lnum = ਸ਼ੁਰੂ_lnum;

		ਰੋਕੋ;
	    }

	    ਸਿਰਾ_lnum = lnum;
	    invalidate_current_state();
	}

	// Ran into start of the file or exceeded maximum number of lines
	ਜੇ (lnum <= break_lnum)
	{
	    invalidate_current_state();
	    current_lnum = break_lnum + 1;
	}
    }

    validate_current_state();
}

    ਸਥਿਰ ਖਾਲੀ
save_chartab(char_u *chartab)
{
    ਜੇ (syn_ਖੰਡ->b_syn_isk == ਖਾਲੀ_ਥਾਵਾਂ_option)
	ਵਾਪਸ;

    mch_memmove(chartab, syn_buf->b_chartab, (size_t)32);
    mch_memmove(syn_buf->b_chartab, syn_win->w_s->b_syn_chartab, (size_t)32);
}

    ਸਥਿਰ ਖਾਲੀ
restore_chartab(char_u *chartab)
{
    ਜੇ (syn_win->w_s->b_syn_isk != ਖਾਲੀ_ਥਾਵਾਂ_option)
	mch_memmove(syn_buf->b_chartab, chartab, (size_t)32);
}

/*
 * Return TRUE if the line-continuation pattern matches in line "lnum".
 */
    ਸਥਿਰ ਪੂਰਨ
syn_ਮੇਲ_linecont(linenr_T lnum)
{
    regmmatch_T regmatch;
    ਪੂਰਨ r;
    char_u	buf_chartab[32];  // chartab array for syn iskyeyword

    ਜੇ (syn_ਖੰਡ->b_syn_linecont_prog == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    // use syntax iskeyword option
    save_chartab(buf_chartab);
    regmatch.rmm_ic = syn_ਖੰਡ->b_syn_linecont_ic;
    regmatch.regprog = syn_ਖੰਡ->b_syn_linecont_prog;
    r = syn_regexec(&regmatch, lnum, (colnr_T)0,
	    IF_SYN_ਸਮਾਂ(&syn_ਖੰਡ->b_syn_linecont_ਸਮਾਂ));
    syn_ਖੰਡ->b_syn_linecont_prog = regmatch.regprog;
    restore_chartab(buf_chartab);
    ਵਾਪਸ r;
}

/*
 * Prepare the current state for the start of a line.
 */
    ਸਥਿਰ ਖਾਲੀ
syn_ਸ਼ੁਰੂ_ਸਤਰ(ਖਾਲੀ)
{
    current_ਮੁਕੰਮਲ = FALSE;
    current_ਖਾਨਾ = 0;

    /*
     * Need to update the end of a start/skip/end that continues from the
     * previous line and regions that have "keepend".
     */
    ਜੇ (current_state.ga_len > 0)
    {
	syn_update_ਸਿਰੇ(TRUE);
	check_state_ਸਿਰੇ();
    }

    ਅਗਲਾ_ਮੇਲ_idx = -1;
    ++current_ਸਤਰ_id;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    ਅਗਲਾ_seqnr = 1;
#ਜੇ_ਅੰਤ
}

/*
 * Check for items in the stack that need their end updated.
 * When "startofline" is TRUE the last item is always updated.
 * When "startofline" is FALSE the item with "keepend" is forcefully updated.
 */
    ਸਥਿਰ ਖਾਲੀ
syn_update_ਸਿਰੇ(ਪੂਰਨ startofline)
{
    stateitem_T	*cur_si;
    ਪੂਰਨ		i;
    ਪੂਰਨ		ਵੇਖਿਆ_keepend;

    ਜੇ (startofline)
    {
	// Check for a match carried over from a previous line with a
	// contained region.  The match ends as soon as the region ends.
	ਲਈ (i = 0; i < current_state.ga_len; ++i)
	{
	    cur_si = &CUR_STATE(i);
	    ਜੇ (cur_si->si_idx >= 0
		    && (SYN_ਚੀਜ਼ਾਂ(syn_ਖੰਡ)[cur_si->si_idx]).sp_type
							       == SPTYPE_ਮੇਲ
		    && cur_si->si_m_endpos.lnum < current_lnum)
	    {
		cur_si->si_flags |= HL_MATCHCONT;
		cur_si->si_m_endpos.lnum = 0;
		cur_si->si_m_endpos.ਖਾਨਾ = 0;
		cur_si->si_h_endpos = cur_si->si_m_endpos;
		cur_si->si_ਸਿਰੇ = TRUE;
	    }
	}
    }

    /*
     * Need to update the end of a start/skip/end that continues from the
     * previous line.  And regions that have "keepend", because they may
     * influence contained items.  If we've just removed "extend"
     * (startofline == 0) then we should update ends of normal regions
     * contained inside "keepend" because "extend" could have extended
     * these "keepend" regions as well as contained normal regions.
     * Then check for items ending in column 0.
     */
    i = current_state.ga_len - 1;
    ਜੇ (keepend_level >= 0)
	ਲਈ ( ; i > keepend_level; --i)
	    ਜੇ (CUR_STATE(i).si_flags & HL_EXTEND)
		ਰੋਕੋ;

    ਵੇਖਿਆ_keepend = FALSE;
    ਲਈ ( ; i < current_state.ga_len; ++i)
    {
	cur_si = &CUR_STATE(i);
	ਜੇ ((cur_si->si_flags & HL_KEEPEND)
			    || (ਵੇਖਿਆ_keepend && !startofline)
			    || (i == current_state.ga_len - 1 && startofline))
	{
	    cur_si->si_h_startpos.ਖਾਨਾ = 0;	// start highl. in col 0
	    cur_si->si_h_startpos.lnum = current_lnum;

	    ਜੇ (!(cur_si->si_flags & HL_MATCHCONT))
		update_si_ਸਿਰਾ(cur_si, (ਪੂਰਨ)current_ਖਾਨਾ, !startofline);

	    ਜੇ (!startofline && (cur_si->si_flags & HL_KEEPEND))
		ਵੇਖਿਆ_keepend = TRUE;
	}
    }
    check_keepend();
}

/////////////////////////////////////////
// Handling of the state stack cache.

/*
 * EXPLANATION OF THE SYNTAX STATE STACK CACHE
 *
 * To speed up syntax highlighting, the state stack for the start of some
 * lines is cached.  These entries can be used to start parsing at that point.
 *
 * The stack is kept in b_sst_array[] for each buffer.  There is a list of
 * valid entries.  b_sst_first points to the first one, then follow sst_next.
 * The entries are sorted on line number.  The first entry is often for line 2
 * (line 1 always starts with an empty stack).
 * There is also a list for free entries.  This construction is used to avoid
 * having to allocate and free memory blocks too often.
 *
 * When making changes to the buffer, this is logged in b_mod_*.  When calling
 * update_screen() to update the display, it will call
 * syn_stack_apply_changes() for each displayed buffer to adjust the cached
 * entries.  The entries which are inside the changed area are removed,
 * because they must be recomputed.  Entries below the changed have their line
 * number adjusted for deleted/inserted lines, and have their sst_change_lnum
 * set to indicate that a check must be made if the changed lines would change
 * the cached entry.
 *
 * When later displaying lines, an entry is stored for each line.  Displayed
 * lines are likely to be displayed again, in which case the state at the
 * start of the line is needed.
 * For not displayed lines, an entry is stored for every so many lines.  These
 * entries will be used e.g., when scrolling backwards.  The distance between
 * entries depends on the number of lines in the buffer.  For small buffers
 * the distance is fixed at SST_DIST, for large buffers there is a fixed
 * number of entries SST_MAX_ENTRIES, and the distance is computed.
 */

    ਸਥਿਰ ਖਾਲੀ
syn_ਢੇਰ_free_ਖੰਡ(synblock_T *ਖੰਡ)
{
    synstate_T	*p;

    // Syntax definitions may have changed: drop the in_id_list() cache.
    idl_cache_clear(ਖੰਡ);

    ਜੇ (ਖੰਡ->b_sst_array == ਨਿਰਾ)
	ਵਾਪਸ;

    FOR_ALL_SYNSTATES(ਖੰਡ, p)
	clear_syn_state(p);
    VIM_CLEAR(ਖੰਡ->b_sst_array);
    ਖੰਡ->b_sst_ਪਹਿਲਾ = ਨਿਰਾ;
    ਖੰਡ->b_sst_ਖੋਜ = ਨਿਰਾ;
    ਖੰਡ->b_sst_len = 0;
}
/*
 * Free b_sst_array[] for buffer "buf".
 * Used when syntax items changed to force resyncing everywhere.
 */
    ਖਾਲੀ
syn_ਢੇਰ_free_all(synblock_T *ਖੰਡ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    win_T	*wp;
#ਜੇ_ਅੰਤ

    syn_ਢੇਰ_free_ਖੰਡ(ਖੰਡ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    // When using "syntax" fold method, must update all folds.
    FOR_ALL_WINDOWS(wp)
    {
	ਜੇ (wp->w_s == ਖੰਡ && foldmethodIsSyntax(wp))
	    foldUpdateAll(wp);
    }
#ਜੇ_ਅੰਤ
}

/*
 * Allocate the syntax state stack for syn_buf when needed.
 * If the number of entries in b_sst_array[] is much too big or a bit too
 * small, reallocate it.
 * Also used to allocate b_sst_array[] for the first time.
 */
    ਸਥਿਰ ਖਾਲੀ
syn_ਢੇਰ_alloc(ਖਾਲੀ)
{
    ਲੰਮਾ	len;
    synstate_T	*ਮੰਜ਼ਿਲ, *from;
    synstate_T	*sstp;

    len = syn_buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ / SST_DIST + ਪੰਕਤੀਆਂ * 2;
    ਜੇ (len < SST_MIN_ENTRIES)
	len = SST_MIN_ENTRIES;
    ਨਹੀਂਤਾਂ ਜੇ (len > SST_MAX_ENTRIES)
	len = SST_MAX_ENTRIES;
    ਜੇ (syn_ਖੰਡ->b_sst_len > len * 2 || syn_ਖੰਡ->b_sst_len < len)
    {
	// Allocate 50% too much, to avoid reallocating too often.
	len = syn_buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	len = (len + len / 2) / SST_DIST + ਪੰਕਤੀਆਂ * 2;
	ਜੇ (len < SST_MIN_ENTRIES)
	    len = SST_MIN_ENTRIES;
	ਨਹੀਂਤਾਂ ਜੇ (len > SST_MAX_ENTRIES)
	    len = SST_MAX_ENTRIES;

	ਜੇ (syn_ਖੰਡ->b_sst_array != ਨਿਰਾ)
	{
	    // When shrinking the array, cleanup the existing stack.
	    // Make sure that all valid entries fit in the new array.
	    ਜਦੋਂ (syn_ਖੰਡ->b_sst_len - syn_ਖੰਡ->b_sst_freecount + 2 > len
		    && syn_ਢੇਰ_cleanup())
		;
	    ਜੇ (len < syn_ਖੰਡ->b_sst_len - syn_ਖੰਡ->b_sst_freecount + 2)
		len = syn_ਖੰਡ->b_sst_len - syn_ਖੰਡ->b_sst_freecount + 2;
	}

	sstp = ALLOC_CLEAR_MULT(synstate_T, len);
	ਜੇ (sstp == ਨਿਰਾ)	// out of memory!
	    ਵਾਪਸ;

	ਮੰਜ਼ਿਲ = sstp - 1;
	ਜੇ (syn_ਖੰਡ->b_sst_array != ਨਿਰਾ)
	{
	    // Move the states from the old array to the new one.
	    ਲਈ (from = syn_ਖੰਡ->b_sst_ਪਹਿਲਾ; from != ਨਿਰਾ;
							from = from->sst_ਅਗਲਾ)
	    {
		++ਮੰਜ਼ਿਲ;
		*ਮੰਜ਼ਿਲ = *from;
		ਮੰਜ਼ਿਲ->sst_ਅਗਲਾ = ਮੰਜ਼ਿਲ + 1;
	    }
	}
	ਜੇ (ਮੰਜ਼ਿਲ != sstp - 1)
	{
	    ਮੰਜ਼ਿਲ->sst_ਅਗਲਾ = ਨਿਰਾ;
	    syn_ਖੰਡ->b_sst_ਪਹਿਲਾ = sstp;
	    syn_ਖੰਡ->b_sst_freecount = len - (ਪੂਰਨ)(ਮੰਜ਼ਿਲ - sstp) - 1;
	}
	ਨਹੀਂਤਾਂ
	{
	    syn_ਖੰਡ->b_sst_ਪਹਿਲਾ = ਨਿਰਾ;
	    syn_ਖੰਡ->b_sst_freecount = len;
	}

	// Create the list of free entries.
	syn_ਖੰਡ->b_sst_firstfree = ਮੰਜ਼ਿਲ + 1;
	ਜਦੋਂ (++ਮੰਜ਼ਿਲ < sstp + len)
	    ਮੰਜ਼ਿਲ->sst_ਅਗਲਾ = ਮੰਜ਼ਿਲ + 1;
	(sstp + len - 1)->sst_ਅਗਲਾ = ਨਿਰਾ;

	vim_free(syn_ਖੰਡ->b_sst_array);
	syn_ਖੰਡ->b_sst_array = sstp;
	syn_ਖੰਡ->b_sst_len = len;
	// The entries were moved to a new array, drop the stale pointer.
	syn_ਖੰਡ->b_sst_ਖੋਜ = ਨਿਰਾ;
    }
}

/*
 * Check for changes in a buffer to affect stored syntax states.  Uses the
 * b_mod_* fields.
 * Called from update_screen(), before screen is being updated, once for each
 * displayed buffer.
 */
    ਖਾਲੀ
syn_ਢੇਰ_apply_changes(buf_T *buf)
{
    win_T	*wp;

    syn_ਢੇਰ_apply_changes_ਖੰਡ(&buf->b_s, buf);

    FOR_ALL_WINDOWS(wp)
    {
	ਜੇ ((wp->w_ਬਫਰ == buf) && (wp->w_s != &buf->b_s))
	    syn_ਢੇਰ_apply_changes_ਖੰਡ(wp->w_s, buf);
    }
}

    ਸਥਿਰ ਖਾਲੀ
syn_ਢੇਰ_apply_changes_ਖੰਡ(synblock_T *ਖੰਡ, buf_T *buf)
{
    synstate_T	*p, *prev, *np;
    linenr_T	n;

    prev = ਨਿਰਾ;
    ਲਈ (p = ਖੰਡ->b_sst_ਪਹਿਲਾ; p != ਨਿਰਾ; )
    {
	ਜੇ (p->sst_lnum + ਖੰਡ->b_syn_sync_linebreaks > buf->b_mod_ਸਿਖਰ)
	{
	    n = p->sst_lnum + buf->b_mod_xlines;
	    ਜੇ (n <= buf->b_mod_bot)
	    {
		// this state is inside the changed area, remove it
		np = p->sst_ਅਗਲਾ;
		ਜੇ (prev == ਨਿਰਾ)
		    ਖੰਡ->b_sst_ਪਹਿਲਾ = np;
		ਨਹੀਂਤਾਂ
		    prev->sst_ਅਗਲਾ = np;
		syn_ਢੇਰ_free_ਇੰਦਰਾਜ(ਖੰਡ, p);
		p = np;
		ਜਾਰੀ;
	    }
	    // This state is below the changed area.  Remember the line
	    // that needs to be parsed before this entry can be made valid
	    // again.
	    ਜੇ (p->sst_change_lnum != 0 && p->sst_change_lnum > buf->b_mod_ਸਿਖਰ)
	    {
		ਜੇ (p->sst_change_lnum + buf->b_mod_xlines > buf->b_mod_ਸਿਖਰ)
		    p->sst_change_lnum += buf->b_mod_xlines;
		ਨਹੀਂਤਾਂ
		    p->sst_change_lnum = buf->b_mod_ਸਿਖਰ;
	    }
	    ਜੇ (p->sst_change_lnum == 0
		    || p->sst_change_lnum < buf->b_mod_bot)
		p->sst_change_lnum = buf->b_mod_bot;

	    p->sst_lnum = n;
	}
	prev = p;
	p = p->sst_ਅਗਲਾ;
    }
}

/*
 * Reduce the number of entries in the state stack for syn_buf.
 * Returns TRUE if at least one entry was freed.
 */
    ਸਥਿਰ ਪੂਰਨ
syn_ਢੇਰ_cleanup(ਖਾਲੀ)
{
    synstate_T	*p, *prev;
    disptick_T	tick;
    ਪੂਰਨ		above;
    ਪੂਰਨ		dist;
    ਪੂਰਨ		retval = FALSE;

    ਜੇ (syn_ਖੰਡ->b_sst_ਪਹਿਲਾ == ਨਿਰਾ)
	ਵਾਪਸ retval;

    // Compute normal distance between non-displayed entries.
    ਜੇ (syn_ਖੰਡ->b_sst_len <= ਪੰਕਤੀਆਂ)
	dist = 999999;
    ਨਹੀਂਤਾਂ
	dist = syn_buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ / (syn_ਖੰਡ->b_sst_len - ਪੰਕਤੀਆਂ) + 1;

    /*
     * Go through the list to find the "tick" for the oldest entry that can
     * be removed.  Set "above" when the "tick" for the oldest entry is above
     * "b_sst_lasttick" (the display tick wraps around).
     */
    tick = syn_ਖੰਡ->b_sst_lasttick;
    above = FALSE;
    prev = syn_ਖੰਡ->b_sst_ਪਹਿਲਾ;
    ਲਈ (p = prev->sst_ਅਗਲਾ; p != ਨਿਰਾ; prev = p, p = p->sst_ਅਗਲਾ)
    {
	ਜੇ (prev->sst_lnum + dist > p->sst_lnum)
	{
	    ਜੇ (p->sst_tick > syn_ਖੰਡ->b_sst_lasttick)
	    {
		ਜੇ (!above || p->sst_tick < tick)
		    tick = p->sst_tick;
		above = TRUE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (!above && p->sst_tick < tick)
		tick = p->sst_tick;
	}
    }

    /*
     * Go through the list to make the entries for the oldest tick at an
     * interval of several lines.
     */
    prev = syn_ਖੰਡ->b_sst_ਪਹਿਲਾ;
    ਲਈ (p = prev->sst_ਅਗਲਾ; p != ਨਿਰਾ; prev = p, p = p->sst_ਅਗਲਾ)
    {
	ਜੇ (p->sst_tick == tick && prev->sst_lnum + dist > p->sst_lnum)
	{
	    // Move this entry from used list to free list
	    prev->sst_ਅਗਲਾ = p->sst_ਅਗਲਾ;
	    syn_ਢੇਰ_free_ਇੰਦਰਾਜ(syn_ਖੰਡ, p);
	    p = prev;
	    retval = TRUE;
	}
    }
    ਵਾਪਸ retval;
}

/*
 * Free the allocated memory for a syn_state item.
 * Move the entry into the free list.
 */
    ਸਥਿਰ ਖਾਲੀ
syn_ਢੇਰ_free_ਇੰਦਰਾਜ(synblock_T *ਖੰਡ, synstate_T *p)
{
    ਜੇ (ਖੰਡ->b_sst_ਖੋਜ == p)
	ਖੰਡ->b_sst_ਖੋਜ = ਨਿਰਾ;
    clear_syn_state(p);
    p->sst_ਅਗਲਾ = ਖੰਡ->b_sst_firstfree;
    ਖੰਡ->b_sst_firstfree = p;
    ++ਖੰਡ->b_sst_freecount;
}

/*
 * Find an entry in the list of state stacks at or before "lnum".
 * Returns NULL when there is no entry or the first entry is after "lnum".
 */
    ਸਥਿਰ synstate_T *
syn_ਢੇਰ_ਲੱਭੋ_ਇੰਦਰਾਜ(linenr_T lnum)
{
    synstate_T	*p, *prev;

    prev = ਨਿਰਾ;
    p = syn_ਖੰਡ->b_sst_ਪਹਿਲਾ;

    // The list is sorted by line number and lookups while parsing advance
    // monotonically, so resume from the last returned entry instead of
    // rescanning from the start whenever it is at or before "lnum".
    ਜੇ (syn_ਖੰਡ->b_sst_ਖੋਜ != ਨਿਰਾ
				 && syn_ਖੰਡ->b_sst_ਖੋਜ->sst_lnum <= lnum)
	p = syn_ਖੰਡ->b_sst_ਖੋਜ;

    ਲਈ ( ; p != ਨਿਰਾ; prev = p, p = p->sst_ਅਗਲਾ)
    {
	ਜੇ (p->sst_lnum == lnum)
	{
	    syn_ਖੰਡ->b_sst_ਖੋਜ = p;
	    ਵਾਪਸ p;
	}
	ਜੇ (p->sst_lnum > lnum)
	    ਰੋਕੋ;
    }
    ਜੇ (prev != ਨਿਰਾ)
	syn_ਖੰਡ->b_sst_ਖੋਜ = prev;
    ਵਾਪਸ prev;
}

/*
 * Try saving the current state in b_sst_array[].
 * The current state must be valid for the start of the current_lnum line!
 */
    ਸਥਿਰ synstate_T *
store_current_state(ਖਾਲੀ)
{
    ਪੂਰਨ		i;
    synstate_T	*p;
    bufstate_T	*bp;
    stateitem_T	*cur_si;
    synstate_T	*sp = syn_ਢੇਰ_ਲੱਭੋ_ਇੰਦਰਾਜ(current_lnum);

    /*
     * If the current state contains a start or end pattern that continues
     * from the previous line, we can't use it.  Don't store it then.
     */
    ਲਈ (i = current_state.ga_len - 1; i >= 0; --i)
    {
	cur_si = &CUR_STATE(i);
	ਜੇ (cur_si->si_h_startpos.lnum >= current_lnum
		|| cur_si->si_m_endpos.lnum >= current_lnum
		|| cur_si->si_h_endpos.lnum >= current_lnum
		|| (cur_si->si_ਸਿਰਾ_idx
		    && cur_si->si_eoe_pos.lnum >= current_lnum))
	    ਰੋਕੋ;
    }
    ਜੇ (i >= 0)
    {
	ਜੇ (sp != ਨਿਰਾ)
	{
	    // find "sp" in the list and remove it
	    ਜੇ (syn_ਖੰਡ->b_sst_ਪਹਿਲਾ == sp)
		// it's the first entry
		syn_ਖੰਡ->b_sst_ਪਹਿਲਾ = sp->sst_ਅਗਲਾ;
	    ਨਹੀਂਤਾਂ
	    {
		// find the entry just before this one to adjust sst_next
		FOR_ALL_SYNSTATES(syn_ਖੰਡ, p)
		    ਜੇ (p->sst_ਅਗਲਾ == sp)
			ਰੋਕੋ;
		ਜੇ (p != ਨਿਰਾ)	// just in case
		    p->sst_ਅਗਲਾ = sp->sst_ਅਗਲਾ;
	    }
	    syn_ਢੇਰ_free_ਇੰਦਰਾਜ(syn_ਖੰਡ, sp);
	    sp = ਨਿਰਾ;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (sp == ਨਿਰਾ || sp->sst_lnum != current_lnum)
    {
	/*
	 * Add a new entry
	 */
	// If no free items, cleanup the array first.
	ਜੇ (syn_ਖੰਡ->b_sst_freecount == 0)
	{
	    (ਖਾਲੀ)syn_ਢੇਰ_cleanup();
	    // "sp" may have been moved to the freelist now
	    sp = syn_ਢੇਰ_ਲੱਭੋ_ਇੰਦਰਾਜ(current_lnum);
	}
	// Still no free items?  Must be a strange problem...
	ਜੇ (syn_ਖੰਡ->b_sst_freecount == 0)
	    sp = ਨਿਰਾ;
	ਨਹੀਂਤਾਂ
	{
	    // Take the first item from the free list and put it in the used
	    // list, after *sp
	    p = syn_ਖੰਡ->b_sst_firstfree;
	    syn_ਖੰਡ->b_sst_firstfree = p->sst_ਅਗਲਾ;
	    --syn_ਖੰਡ->b_sst_freecount;
	    ਜੇ (sp == ਨਿਰਾ)
	    {
		// Insert in front of the list
		p->sst_ਅਗਲਾ = syn_ਖੰਡ->b_sst_ਪਹਿਲਾ;
		syn_ਖੰਡ->b_sst_ਪਹਿਲਾ = p;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// insert in list after *sp
		p->sst_ਅਗਲਾ = sp->sst_ਅਗਲਾ;
		sp->sst_ਅਗਲਾ = p;
	    }
	    sp = p;
	    sp->sst_stacksize = 0;
	    sp->sst_lnum = current_lnum;
	    // Resume the next forward lookup from the entry just stored.
	    syn_ਖੰਡ->b_sst_ਖੋਜ = sp;
	}
    }
    ਜੇ (sp != ਨਿਰਾ)
    {
	// When overwriting an existing state stack, clear it first
	clear_syn_state(sp);
	sp->sst_stacksize = current_state.ga_len;
	ਜੇ (current_state.ga_len > SST_FIX_ਹਾਲਤਾਂ)
	{
	    // Need to clear it, might be something remaining from when the
	    // length was less than SST_FIX_STATES.
	    ga_init2(&sp->sst_union.sst_ga, ਆਕਾਰ(bufstate_T), 1);
	    ਜੇ (ga_grow(&sp->sst_union.sst_ga, current_state.ga_len) == FAIL)
		sp->sst_stacksize = 0;
	    ਨਹੀਂਤਾਂ
		sp->sst_union.sst_ga.ga_len = current_state.ga_len;
	    bp = SYN_STATE_P(&(sp->sst_union.sst_ga));
	}
	ਨਹੀਂਤਾਂ
	    bp = sp->sst_union.sst_ਢੇਰ;
	ਲਈ (i = 0; i < sp->sst_stacksize; ++i)
	{
	    bp[i].bs_idx = CUR_STATE(i).si_idx;
	    bp[i].bs_flags = CUR_STATE(i).si_flags;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
	    bp[i].bs_seqnr = CUR_STATE(i).si_seqnr;
	    bp[i].bs_cchar = CUR_STATE(i).si_cchar;
#ਜੇ_ਅੰਤ
	    bp[i].bs_extmatch = ref_extmatch(CUR_STATE(i).si_extmatch);
	}
	sp->sst_ਅਗਲਾ_flags = current_ਅਗਲਾ_flags;
	sp->sst_ਅਗਲਾ_list = current_ਅਗਲਾ_list;
	sp->sst_tick = display_tick;
	sp->sst_change_lnum = 0;
    }
    current_state_stored = TRUE;
    ਵਾਪਸ sp;
}

/*
 * Copy a state stack from "from" in b_sst_array[] to current_state;
 */
    ਸਥਿਰ ਖਾਲੀ
ਲੱਦੋ_current_state(synstate_T *from)
{
    ਪੂਰਨ		i;
    bufstate_T	*bp;

    clear_current_state();
    validate_current_state();
    keepend_level = -1;
    ਜੇ (from->sst_stacksize
	    && ga_grow(&current_state, from->sst_stacksize) == OK)
    {
	ਜੇ (from->sst_stacksize > SST_FIX_ਹਾਲਤਾਂ)
	    bp = SYN_STATE_P(&(from->sst_union.sst_ga));
	ਨਹੀਂਤਾਂ
	    bp = from->sst_union.sst_ਢੇਰ;
	ਲਈ (i = 0; i < from->sst_stacksize; ++i)
	{
	    CUR_STATE(i).si_idx = bp[i].bs_idx;
	    CUR_STATE(i).si_flags = bp[i].bs_flags;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
	    CUR_STATE(i).si_seqnr = bp[i].bs_seqnr;
	    CUR_STATE(i).si_cchar = bp[i].bs_cchar;
#ਜੇ_ਅੰਤ
	    CUR_STATE(i).si_extmatch = ref_extmatch(bp[i].bs_extmatch);
	    ਜੇ (keepend_level < 0 && (CUR_STATE(i).si_flags & HL_KEEPEND))
		keepend_level = i;
	    CUR_STATE(i).si_ਸਿਰੇ = FALSE;
	    CUR_STATE(i).si_m_lnum = 0;
	    ਜੇ (CUR_STATE(i).si_idx >= 0)
		CUR_STATE(i).si_ਅਗਲਾ_list =
		     (SYN_ਚੀਜ਼ਾਂ(syn_ਖੰਡ)[CUR_STATE(i).si_idx]).sp_ਅਗਲਾ_list;
	    ਨਹੀਂਤਾਂ
		CUR_STATE(i).si_ਅਗਲਾ_list = ਨਿਰਾ;
	    update_si_attr(i);
	}
	current_state.ga_len = from->sst_stacksize;
    }
    current_ਅਗਲਾ_list = from->sst_ਅਗਲਾ_list;
    current_ਅਗਲਾ_flags = from->sst_ਅਗਲਾ_flags;
    current_lnum = from->sst_lnum;
}

/*
 * Compare saved state stack "*sp" with the current state.
 * Return TRUE when they are equal.
 */
    ਸਥਿਰ ਪੂਰਨ
syn_ਢੇਰ_equal(synstate_T *sp)
{
    ਪੂਰਨ		i, j;
    bufstate_T	*bp;
    reg_extmatch_T	*six, *bsx;

    // First a quick check if the stacks have the same size end nextlist.
    ਜੇ (sp->sst_stacksize != current_state.ga_len
	    || sp->sst_ਅਗਲਾ_list != current_ਅਗਲਾ_list)
	ਵਾਪਸ FALSE;

    // Need to compare all states on both stacks.
    ਜੇ (sp->sst_stacksize > SST_FIX_ਹਾਲਤਾਂ)
	bp = SYN_STATE_P(&(sp->sst_union.sst_ga));
    ਨਹੀਂਤਾਂ
	bp = sp->sst_union.sst_ਢੇਰ;

    ਲਈ (i = current_state.ga_len; --i >= 0; )
    {
	// If the item has another index the state is different.
	ਜੇ (bp[i].bs_idx != CUR_STATE(i).si_idx)
	    ਰੋਕੋ;
	ਜੇ (bp[i].bs_extmatch == CUR_STATE(i).si_extmatch)
	    ਜਾਰੀ;
	// When the extmatch pointers are different, the strings in them can
	// still be the same.  Check if the extmatch references are equal.
	bsx = bp[i].bs_extmatch;
	six = CUR_STATE(i).si_extmatch;
	// If one of the extmatch pointers is NULL the states are different.
	ਜੇ (bsx == ਨਿਰਾ || six == ਨਿਰਾ)
	    ਰੋਕੋ;
	ਲਈ (j = 0; j < NSUBEXP; ++j)
	{
	    // Check each referenced match string. They must all be equal.
	    ਜੇ (bsx->matches[j] != six->matches[j])
	    {
		// If the pointer is different it can still be the same text.
		// Compare the strings, ignore case when the start item has the
		// sp_ic flag set.
		ਜੇ (bsx->matches[j] == ਨਿਰਾ || six->matches[j] == ਨਿਰਾ)
		    ਰੋਕੋ;
		ਜੇ ((SYN_ਚੀਜ਼ਾਂ(syn_ਖੰਡ)[CUR_STATE(i).si_idx]).sp_ic
			? MB_STRICMP(bsx->matches[j], six->matches[j]) != 0
			: STRCMP(bsx->matches[j], six->matches[j]) != 0)
		    ਰੋਕੋ;
	    }
	}
	ਜੇ (j != NSUBEXP)
	    ਰੋਕੋ;
    }
    ਵਾਪਸ i < 0 ? TRUE : FALSE;
}

/*
 * We stop parsing syntax above line "lnum".  If the stored state at or below
 * this line depended on a change before it, it now depends on the line below
 * the last parsed line.
 * The window looks like this:
 *	    line which changed
 *	    displayed line
 *	    displayed line
 * lnum ->  line below window
 */
    ਖਾਲੀ
syntax_ਸਿਰਾ_parsing(win_T *wp, linenr_T lnum)
{
    synstate_T	*sp;

    ਜੇ (syn_ਖੰਡ != wp->w_s)
	ਵਾਪਸ;  // not the right window
    sp = syn_ਢੇਰ_ਲੱਭੋ_ਇੰਦਰਾਜ(lnum);
    ਜੇ (sp != ਨਿਰਾ && sp->sst_lnum < lnum)
	sp = sp->sst_ਅਗਲਾ;

    ਜੇ (sp != ਨਿਰਾ && sp->sst_change_lnum != 0)
	sp->sst_change_lnum = lnum;
}

/*
 * End of handling of the state stack.
 ****************************************/

    ਸਥਿਰ ਖਾਲੀ
invalidate_current_state(ਖਾਲੀ)
{
    clear_current_state();
    current_state.ga_itemsize = 0;	// mark current_state invalid
    current_ਅਗਲਾ_list = ਨਿਰਾ;
    keepend_level = -1;
}

    ਸਥਿਰ ਖਾਲੀ
validate_current_state(ਖਾਲੀ)
{
    current_state.ga_itemsize = ਆਕਾਰ(stateitem_T);
    current_state.ga_growsize = 3;
}

/*
 * Return TRUE if the syntax at start of lnum changed since last time.
 * This will only be called just after get_syntax_attr() for the previous
 * line, to check if the next line needs to be redrawn too.
 */
    ਪੂਰਨ
syntax_check_ਬਦਲਿਆ(linenr_T lnum)
{
    ਪੂਰਨ		retval = TRUE;
    synstate_T	*sp;

    /*
     * Check the state stack when:
     * - lnum is just below the previously syntaxed line.
     * - lnum is not before the lines with saved states.
     * - lnum is not past the lines with saved states.
     * - lnum is at or before the last changed line.
     */
    ਜੇ (VALID_STATE(&current_state) && lnum == current_lnum + 1)
    {
	sp = syn_ਢੇਰ_ਲੱਭੋ_ਇੰਦਰਾਜ(lnum);
	ਜੇ (sp != ਨਿਰਾ && sp->sst_lnum == lnum)
	{
	    /*
	     * finish the previous line (needed when not all of the line was
	     * drawn)
	     */
	    (ਖਾਲੀ)syn_ਅੰਤ_ਸਤਰ(FALSE);

	    /*
	     * Compare the current state with the previously saved state of
	     * the line.
	     */
	    ਜੇ (syn_ਢੇਰ_equal(sp))
		retval = FALSE;

	    /*
	     * Store the current state in b_sst_array[] for later use.
	     */
	    ++current_lnum;
	    (ਖਾਲੀ)store_current_state();
	}
    }

    ਵਾਪਸ retval;
}

/*
 * Finish the current line.
 * This doesn't return any attributes, it only gets the state at the end of
 * the line.  It can start anywhere in the line, as long as the current state
 * is valid.
 */
    ਸਥਿਰ ਪੂਰਨ
syn_ਅੰਤ_ਸਤਰ(
    ਪੂਰਨ	    syncing)		// called for syncing
{
    stateitem_T	*cur_si;
    colnr_T	prev_current_ਖਾਨਾ;

    ਜਦੋਂ (!current_ਮੁਕੰਮਲ)
    {
	(ਖਾਲੀ)syn_current_attr(syncing, FALSE, ਨਿਰਾ, FALSE);
	/*
	 * When syncing, and found some item, need to check the item.
	 */
	ਜੇ (syncing && current_state.ga_len)
	{
	    /*
	     * Check for match with sync item.
	     */
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	    ਜੇ (cur_si->si_idx >= 0
		    && (SYN_ਚੀਜ਼ਾਂ(syn_ਖੰਡ)[cur_si->si_idx].sp_flags
					  & (HL_SYNC_ਇੱਥੇ|HL_SYNC_THERE)))
		ਵਾਪਸ TRUE;

	    // syn_current_attr() will have skipped the check for an item
	    // that ends here, need to do that now.  Be careful not to go
	    // past the NUL.
	    prev_current_ਖਾਨਾ = current_ਖਾਨਾ;
	    ਜੇ (syn_getcurline()[current_ਖਾਨਾ] != NUL)
		++current_ਖਾਨਾ;
	    check_state_ਸਿਰੇ();
	    current_ਖਾਨਾ = prev_current_ਖਾਨਾ;
	}
	++current_ਖਾਨਾ;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Return highlight attributes for next character.
 * Must first call syntax_start() once for the line.
 * "col" is normally 0 for the first use in a line, and increments by one each
 * time.  It's allowed to skip characters and to stop before the end of the
 * line.  But only a "col" after a previously used column is allowed.
 * When "can_spell" is not NULL set it to TRUE when spell-checking should be
 * done.
 */
    ਪੂਰਨ
get_syntax_attr(
    colnr_T	ਖਾਨਾ,
    ਪੂਰਨ		*can_spell,
    ਪੂਰਨ		keep_state)	// keep state of char at "col"
{
    ਪੂਰਨ	    attr = 0;

    ਜੇ (can_spell != ਨਿਰਾ)
	// Default: Only do spelling when there is no @Spell cluster or when
	// ":syn spell toplevel" was used.
	*can_spell = syn_ਖੰਡ->b_syn_spell == SYNSPL_DEFAULT
		    ? (syn_ਖੰਡ->b_spell_cluster_id == 0)
		    : (syn_ਖੰਡ->b_syn_spell == SYNSPL_ਸਿਖਰ);

    // check for out of memory situation
    ਜੇ (syn_ਖੰਡ->b_sst_array == ਨਿਰਾ)
	ਵਾਪਸ 0;

    // After 'synmaxcol' the attribute is always zero.
    ਜੇ (syn_buf->b_p_smc > 0 && ਖਾਨਾ >= (colnr_T)syn_buf->b_p_smc)
    {
	clear_current_state();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	current_id = 0;
	current_trans_id = 0;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
	current_flags = 0;
	current_seqnr = 0;
#ਜੇ_ਅੰਤ
	ਵਾਪਸ 0;
    }

    // Make sure current_state is valid
    ਜੇ (INVALID_STATE(&current_state))
	validate_current_state();

    /*
     * Skip from the current column to "col", get the attributes for "col".
     */
    ਜਦੋਂ (current_ਖਾਨਾ <= ਖਾਨਾ)
    {
	attr = syn_current_attr(FALSE, TRUE, can_spell,
				     current_ਖਾਨਾ == ਖਾਨਾ ? keep_state : FALSE);
	++current_ਖਾਨਾ;
    }

    ਵਾਪਸ attr;
}

/*
 * Get syntax attributes for current_lnum, current_col.
 */
    ਸਥਿਰ ਪੂਰਨ
syn_current_attr(
    ਪੂਰਨ		syncing,		// When 1: called for syncing
    ਪੂਰਨ		displaying,		// result will be displayed
    ਪੂਰਨ		*can_spell,		// return: do spell checking
    ਪੂਰਨ		keep_state)		// keep syntax stack afterwards
{
    ਪੂਰਨ		syn_id;
    lpos_T	endpos;		// was: char_u *endp;
    lpos_T	hl_startpos;	// was: int hl_startcol;
    lpos_T	hl_endpos;
    lpos_T	eos_pos;	// end-of-start match (start region)
    lpos_T	eoe_pos;	// end-of-end pattern
    ਪੂਰਨ		ਸਿਰਾ_idx;	// group ID for end pattern
    ਪੂਰਨ		idx;
    synpat_T	*spp;
    stateitem_T	*cur_si, *sip = ਨਿਰਾ;
    ਪੂਰਨ		startcol;
    ਪੂਰਨ		endcol;
    ਲੰਮਾ	flags;
    ਪੂਰਨ		cchar;
    ਛੋਟਾ	*ਅਗਲਾ_list;
    ਪੂਰਨ		ਮਿਲਿਆ_ਮੇਲ;		    // found usable match
    ਸਥਿਰ ਪੂਰਨ	try_ਅਗਲਾ_ਕਾਲਮ = FALSE;    // must try in next col
    ਪੂਰਨ		do_keywords;
    regmmatch_T	regmatch;
    lpos_T	pos;
    ਪੂਰਨ		lc_ਖਾਨਾ;
    reg_extmatch_T *cur_extmatch = ਨਿਰਾ;
    char_u	buf_chartab[32];  // chartab array for syn iskyeyword
    char_u	*ਸਤਰ;		// current line.  NOTE: becomes invalid after
				// looking for a pattern match!

    // variables for zero-width matches that have a "nextgroup" argument
    ਪੂਰਨ		keep_ਅਗਲਾ_list;
    ਪੂਰਨ		zero_ਚੌੜਾਈ_ਅਗਲਾ_list = FALSE;
    garray_T	zero_ਚੌੜਾਈ_ਅਗਲਾ_ga;

    /*
     * No character, no attributes!  Past end of line?
     * Do try matching with an empty line (could be the start of a region).
     */
    ਸਤਰ = syn_getcurline();
    ਜੇ (ਸਤਰ[current_ਖਾਨਾ] == NUL && current_ਖਾਨਾ != 0)
    {
	/*
	 * If we found a match after the last column, use it.
	 */
	ਜੇ (ਅਗਲਾ_ਮੇਲ_idx >= 0 && ਅਗਲਾ_ਮੇਲ_ਖਾਨਾ >= (ਪੂਰਨ)current_ਖਾਨਾ
						  && ਅਗਲਾ_ਮੇਲ_ਖਾਨਾ != MAXCOL)
	    (ਖਾਲੀ)ਧੱਕੋ_ਅਗਲਾ_ਮੇਲ(ਨਿਰਾ);

	current_ਮੁਕੰਮਲ = TRUE;
	current_state_stored = FALSE;
	ਵਾਪਸ 0;
    }

    // if the current or next character is NUL, we will finish the line now
    ਜੇ (ਸਤਰ[current_ਖਾਨਾ] == NUL || ਸਤਰ[current_ਖਾਨਾ + 1] == NUL)
    {
	current_ਮੁਕੰਮਲ = TRUE;
	current_state_stored = FALSE;
    }

    /*
     * When in the previous column there was a match but it could not be used
     * (empty match or already matched in this column) need to try again in
     * the next column.
     */
    ਜੇ (try_ਅਗਲਾ_ਕਾਲਮ)
    {
	ਅਗਲਾ_ਮੇਲ_idx = -1;
	try_ਅਗਲਾ_ਕਾਲਮ = FALSE;
    }

    // Only check for keywords when not syncing and there are some.
    do_keywords = !syncing
		    && (syn_ਖੰਡ->b_keywtab.ht_ਵਰਤਿਆ > 0
			    || syn_ਖੰਡ->b_keywtab_ic.ht_ਵਰਤਿਆ > 0);

    // Init the list of zero-width matches with a nextlist.  This is used to
    // avoid matching the same item in the same position twice.
    ga_init2(&zero_ਚੌੜਾਈ_ਅਗਲਾ_ga, ਆਕਾਰ(ਪੂਰਨ), 10);

    // use syntax iskeyword option
    save_chartab(buf_chartab);

    /*
     * Repeat matching keywords and patterns, to find contained items at the
     * same column.  This stops when there are no extra matches at the current
     * column.
     */
    ਕਰੋ
    {
	ਮਿਲਿਆ_ਮੇਲ = FALSE;
	keep_ਅਗਲਾ_list = FALSE;
	syn_id = 0;


	/*
	 * 1. Check for a current state.
	 *    Only when there is no current state, or if the current state may
	 *    contain other things, we need to check for keywords and patterns.
	 *    Always need to check for contained items if some item has the
	 *    "containedin" argument (takes extra time!).
	 */
	ਜੇ (current_state.ga_len)
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	ਨਹੀਂਤਾਂ
	    cur_si = ਨਿਰਾ;

	ਜੇ (syn_ਖੰਡ->b_syn_containedin || cur_si == ਨਿਰਾ
					      || cur_si->si_cont_list != ਨਿਰਾ)
	{
	    /*
	     * 2. Check for keywords, if on a keyword char after a non-keyword
	     *	  char.  Don't do this when syncing.
	     */
	    ਜੇ (do_keywords)
	    {
	      ਸਤਰ = syn_getcurline();
	      ਜੇ (vim_iswordp_buf(ਸਤਰ + current_ਖਾਨਾ, syn_buf)
		      && (current_ਖਾਨਾ == 0
			  || !vim_iswordp_buf(ਸਤਰ + current_ਖਾਨਾ - 1
			      - (has_mbyte
				  ? (*mb_ਸਿਰ_off)(ਸਤਰ, ਸਤਰ + current_ਖਾਨਾ - 1)
				  : 0) , syn_buf)))
	      {
		syn_id = check_keyword_id(ਸਤਰ, (ਪੂਰਨ)current_ਖਾਨਾ,
					 &endcol, &flags, &ਅਗਲਾ_list, cur_si,
					 &cchar);
		ਜੇ (syn_id != 0)
		{
		    ਜੇ (ਧੱਕੋ_current_state(KEYWORD_IDX) == OK)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			cur_si->si_m_startcol = current_ਖਾਨਾ;
			cur_si->si_h_startpos.lnum = current_lnum;
			cur_si->si_h_startpos.ਖਾਨਾ = 0;	// starts right away
			cur_si->si_m_endpos.lnum = current_lnum;
			cur_si->si_m_endpos.ਖਾਨਾ = endcol;
			cur_si->si_h_endpos.lnum = current_lnum;
			cur_si->si_h_endpos.ਖਾਨਾ = endcol;
			cur_si->si_ਸਿਰੇ = TRUE;
			cur_si->si_ਸਿਰਾ_idx = 0;
			cur_si->si_flags = flags;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
			cur_si->si_seqnr = ਅਗਲਾ_seqnr++;
			cur_si->si_cchar = cchar;
			ਜੇ (current_state.ga_len > 1)
			    cur_si->si_flags |=
				  CUR_STATE(current_state.ga_len - 2).si_flags
								 & HL_CONCEAL;
#ਜੇ_ਅੰਤ
			cur_si->si_id = syn_id;
			cur_si->si_trans_id = syn_id;
			ਜੇ (flags & HL_TRANSP)
			{
			    ਜੇ (current_state.ga_len < 2)
			    {
				cur_si->si_attr = 0;
				cur_si->si_trans_id = 0;
			    }
			    ਨਹੀਂਤਾਂ
			    {
				cur_si->si_attr = CUR_STATE(
					current_state.ga_len - 2).si_attr;
				cur_si->si_trans_id = CUR_STATE(
					current_state.ga_len - 2).si_trans_id;
			    }
			}
			ਨਹੀਂਤਾਂ
			    cur_si->si_attr = syn_id2attr(syn_id);
			cur_si->si_cont_list = ਨਿਰਾ;
			cur_si->si_ਅਗਲਾ_list = ਅਗਲਾ_list;
			check_keepend();
		    }
		    ਨਹੀਂਤਾਂ
			vim_free(ਅਗਲਾ_list);
		}
	      }
	    }

	    /*
	     * 3. Check for patterns (only if no keyword found).
	     */
	    ਜੇ (syn_id == 0 && syn_ਖੰਡ->b_syn_patterns.ga_len)
	    {
		/*
		 * If we didn't check for a match yet, or we are past it, check
		 * for any match with a pattern.
		 */
		ਜੇ (ਅਗਲਾ_ਮੇਲ_idx < 0 || ਅਗਲਾ_ਮੇਲ_ਖਾਨਾ < (ਪੂਰਨ)current_ਖਾਨਾ)
		{
		    /*
		     * Check all relevant patterns for a match at this
		     * position.  This is complicated, because matching with a
		     * pattern takes quite a bit of time, thus we want to
		     * avoid doing it when it's not needed.
		     */
		    ਅਗਲਾ_ਮੇਲ_idx = 0;		// no match in this line yet
		    ਅਗਲਾ_ਮੇਲ_ਖਾਨਾ = MAXCOL;
		    ਲਈ (idx = syn_ਖੰਡ->b_syn_patterns.ga_len; --idx >= 0; )
		    {
			spp = &(SYN_ਚੀਜ਼ਾਂ(syn_ਖੰਡ)[idx]);
			ਜੇ (	   spp->sp_syncing == syncing
				&& (displaying || !(spp->sp_flags & HL_DISPLAY))
				&& (spp->sp_type == SPTYPE_ਮੇਲ
				    || spp->sp_type == SPTYPE_ਸ਼ੁਰੂ)
				&& (current_ਅਗਲਾ_list != ਨਿਰਾ
				    ? ਵਿੱਚ_id_list(ਨਿਰਾ, current_ਅਗਲਾ_list,
							      &spp->sp_syn, 0)
				    : (cur_si == ਨਿਰਾ
					? !(spp->sp_flags & HL_CONTAINED)
					: ਵਿੱਚ_id_list(cur_si,
					    cur_si->si_cont_list, &spp->sp_syn,
					    spp->sp_flags))))
			{
			    ਪੂਰਨ r;

			    // If we already tried matching in this line, and
			    // there isn't a match before next_match_col, skip
			    // this item.
			    ਜੇ (spp->sp_ਸਤਰ_id == current_ਸਤਰ_id
				    && spp->sp_startcol >= ਅਗਲਾ_ਮੇਲ_ਖਾਨਾ)
				ਜਾਰੀ;
			    spp->sp_ਸਤਰ_id = current_ਸਤਰ_id;

			    lc_ਖਾਨਾ = current_ਖਾਨਾ - spp->sp_offsets[SPO_LC_OFF];
			    ਜੇ (lc_ਖਾਨਾ < 0)
				lc_ਖਾਨਾ = 0;

			    regmatch.rmm_ic = spp->sp_ic;
			    regmatch.regprog = spp->sp_prog;
			    r = syn_regexec(&regmatch,
					     current_lnum,
					     (colnr_T)lc_ਖਾਨਾ,
					     IF_SYN_ਸਮਾਂ(&spp->sp_ਸਮਾਂ));
			    spp->sp_prog = regmatch.regprog;
			    ਜੇ (!r)
			    {
				// no match in this line, try another one
				spp->sp_startcol = MAXCOL;
				ਜਾਰੀ;
			    }

			    /*
			     * Compute the first column of the match.
			     */
			    syn_ਜੋੜੋ_ਸ਼ੁਰੂ_off(&pos, &regmatch,
							 spp, SPO_MS_OFF, -1);
			    ਜੇ (pos.lnum > current_lnum)
			    {
				// must have used end of match in a next line,
				// we can't handle that
				spp->sp_startcol = MAXCOL;
				ਜਾਰੀ;
			    }
			    startcol = pos.ਖਾਨਾ;

			    // remember the next column where this pattern
			    // matches in the current line
			    spp->sp_startcol = startcol;

			    /*
			     * If a previously found match starts at a lower
			     * column number, don't use this one.
			     */
			    ਜੇ (startcol >= ਅਗਲਾ_ਮੇਲ_ਖਾਨਾ)
				ਜਾਰੀ;

			    /*
			     * If we matched this pattern at this position
			     * before, skip it.  Must retry in the next
			     * column, because it may match from there.
			     */
			    ਜੇ (did_ਮੇਲ_already(idx, &zero_ਚੌੜਾਈ_ਅਗਲਾ_ga))
			    {
				try_ਅਗਲਾ_ਕਾਲਮ = TRUE;
				ਜਾਰੀ;
			    }

			    endpos.lnum = regmatch.endpos[0].lnum;
			    endpos.ਖਾਨਾ = regmatch.endpos[0].ਖਾਨਾ;

			    // Compute the highlight start.
			    syn_ਜੋੜੋ_ਸ਼ੁਰੂ_off(&hl_startpos, &regmatch,
							 spp, SPO_HS_OFF, -1);

			    // Compute the region start.
			    // Default is to use the end of the match.
			    syn_ਜੋੜੋ_ਸਿਰਾ_off(&eos_pos, &regmatch,
							 spp, SPO_RS_OFF, 0);

			    /*
			     * Grab the external submatches before they get
			     * overwritten.  Reference count doesn't change.
			     */
			    unref_extmatch(cur_extmatch);
			    cur_extmatch = re_extmatch_ਬਾਹਰਲਾ;
			    re_extmatch_ਬਾਹਰਲਾ = ਨਿਰਾ;

			    flags = 0;
			    eoe_pos.lnum = 0;	// avoid warning
			    eoe_pos.ਖਾਨਾ = 0;
			    ਸਿਰਾ_idx = 0;
			    hl_endpos.lnum = 0;

			    /*
			     * For a "oneline" the end must be found in the
			     * same line too.  Search for it after the end of
			     * the match with the start pattern.  Set the
			     * resulting end positions at the same time.
			     */
			    ਜੇ (spp->sp_type == SPTYPE_ਸ਼ੁਰੂ
					      && (spp->sp_flags & HL_ONELINE))
			    {
				lpos_T	startpos;

				startpos = endpos;
				ਲੱਭੋ_endpos(idx, &startpos, &endpos, &hl_endpos,
				    &flags, &eoe_pos, &ਸਿਰਾ_idx, cur_extmatch);
				ਜੇ (endpos.lnum == 0)
				    ਜਾਰੀ;	    // not found
			    }

			    /*
			     * For a "match" the size must be > 0 after the
			     * end offset needs has been added.  Except when
			     * syncing.
			     */
			    ਨਹੀਂਤਾਂ ਜੇ (spp->sp_type == SPTYPE_ਮੇਲ)
			    {
				syn_ਜੋੜੋ_ਸਿਰਾ_off(&hl_endpos, &regmatch, spp,
							       SPO_HE_OFF, 0);
				syn_ਜੋੜੋ_ਸਿਰਾ_off(&endpos, &regmatch, spp,
							       SPO_ME_OFF, 0);
				ਜੇ (endpos.lnum == current_lnum
				      && (ਪੂਰਨ)endpos.ਖਾਨਾ + syncing < startcol)
				{
				    /*
				     * If an empty string is matched, may need
				     * to try matching again at next column.
				     */
				    ਜੇ (regmatch.startpos[0].ਖਾਨਾ
						    == regmatch.endpos[0].ਖਾਨਾ)
					try_ਅਗਲਾ_ਕਾਲਮ = TRUE;
				    ਜਾਰੀ;
				}
			    }

			    /*
			     * keep the best match so far in next_match_*
			     */
			    // Highlighting must start after startpos and end
			    // before endpos.
			    ਜੇ (hl_startpos.lnum == current_lnum
					   && (ਪੂਰਨ)hl_startpos.ਖਾਨਾ < startcol)
				hl_startpos.ਖਾਨਾ = startcol;
			    ਹੱਦਬੰਦੀ_pos_zero(&hl_endpos, &endpos);

			    ਅਗਲਾ_ਮੇਲ_idx = idx;
			    ਅਗਲਾ_ਮੇਲ_ਖਾਨਾ = startcol;
			    ਅਗਲਾ_ਮੇਲ_m_endpos = endpos;
			    ਅਗਲਾ_ਮੇਲ_h_endpos = hl_endpos;
			    ਅਗਲਾ_ਮੇਲ_h_startpos = hl_startpos;
			    ਅਗਲਾ_ਮੇਲ_flags = flags;
			    ਅਗਲਾ_ਮੇਲ_eos_pos = eos_pos;
			    ਅਗਲਾ_ਮੇਲ_eoe_pos = eoe_pos;
			    ਅਗਲਾ_ਮੇਲ_ਸਿਰਾ_idx = ਸਿਰਾ_idx;
			    unref_extmatch(ਅਗਲਾ_ਮੇਲ_extmatch);
			    ਅਗਲਾ_ਮੇਲ_extmatch = cur_extmatch;
			    cur_extmatch = ਨਿਰਾ;
			}
		    }
		}

		/*
		 * If we found a match at the current column, use it.
		 */
		ਜੇ (ਅਗਲਾ_ਮੇਲ_idx >= 0 && ਅਗਲਾ_ਮੇਲ_ਖਾਨਾ == (ਪੂਰਨ)current_ਖਾਨਾ)
		{
		    synpat_T	*lspp;

		    // When a zero-width item matched which has a nextgroup,
		    // don't push the item but set nextgroup.
		    lspp = &(SYN_ਚੀਜ਼ਾਂ(syn_ਖੰਡ)[ਅਗਲਾ_ਮੇਲ_idx]);
		    ਜੇ (ਅਗਲਾ_ਮੇਲ_m_endpos.lnum == current_lnum
			    && ਅਗਲਾ_ਮੇਲ_m_endpos.ਖਾਨਾ == current_ਖਾਨਾ
			    && lspp->sp_ਅਗਲਾ_list != ਨਿਰਾ)
		    {
			current_ਅਗਲਾ_list = lspp->sp_ਅਗਲਾ_list;
			current_ਅਗਲਾ_flags = lspp->sp_flags;
			keep_ਅਗਲਾ_list = TRUE;
			zero_ਚੌੜਾਈ_ਅਗਲਾ_list = TRUE;

			// Add the index to a list, so that we can check
			// later that we don't match it again (and cause an
			// endless loop).
			ਜੇ (ga_grow(&zero_ਚੌੜਾਈ_ਅਗਲਾ_ga, 1) == OK)
			{
			    ((ਪੂਰਨ *)(zero_ਚੌੜਾਈ_ਅਗਲਾ_ga.ga_ਅੰਕੜੇ))
				[zero_ਚੌੜਾਈ_ਅਗਲਾ_ga.ga_len++] = ਅਗਲਾ_ਮੇਲ_idx;
			}
			ਅਗਲਾ_ਮੇਲ_idx = -1;
		    }
		    ਨਹੀਂਤਾਂ
			cur_si = ਧੱਕੋ_ਅਗਲਾ_ਮੇਲ(cur_si);
		    ਮਿਲਿਆ_ਮੇਲ = TRUE;
		}
	    }
	}

	/*
	 * Handle searching for nextgroup match.
	 */
	ਜੇ (current_ਅਗਲਾ_list != ਨਿਰਾ && !keep_ਅਗਲਾ_list)
	{
	    /*
	     * If a nextgroup was not found, continue looking for one if:
	     * - this is an empty line and the "skipempty" option was given
	     * - we are on white space and the "skipwhite" option was given
	     */
	    ਜੇ (!ਮਿਲਿਆ_ਮੇਲ)
	    {
		ਸਤਰ = syn_getcurline();
		ਜੇ (((current_ਅਗਲਾ_flags & HL_SKIPWHITE)
			    && VIM_ISWHITE(ਸਤਰ[current_ਖਾਨਾ]))
			|| ((current_ਅਗਲਾ_flags & HL_SKIPEMPTY)
			    && *ਸਤਰ == NUL))
		    ਰੋਕੋ;
	    }

	    /*
	     * If a nextgroup was found: Use it, and continue looking for
	     * contained matches.
	     * If a nextgroup was not found: Continue looking for a normal
	     * match.
	     * When did set current_next_list for a zero-width item and no
	     * match was found don't loop (would get stuck).
	     */
	    current_ਅਗਲਾ_list = ਨਿਰਾ;
	    ਅਗਲਾ_ਮੇਲ_idx = -1;
	    ਜੇ (!zero_ਚੌੜਾਈ_ਅਗਲਾ_list)
		ਮਿਲਿਆ_ਮੇਲ = TRUE;
	}

    } ਜਦੋਂ (ਮਿਲਿਆ_ਮੇਲ);

    restore_chartab(buf_chartab);

    /*
     * Use attributes from the current state, if within its highlighting.
     * If not, use attributes from the current-but-one state, etc.
     */
    current_attr = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    current_id = 0;
    current_trans_id = 0;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    current_flags = 0;
    current_seqnr = 0;
#ਜੇ_ਅੰਤ
    ਜੇ (cur_si != ਨਿਰਾ)
    {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਪੂਰਨ	current_trans_id = 0;
#ਜੇ_ਅੰਤ
	ਲਈ (idx = current_state.ga_len - 1; idx >= 0; --idx)
	{
	    sip = &CUR_STATE(idx);
	    ਜੇ ((current_lnum > sip->si_h_startpos.lnum
			|| (current_lnum == sip->si_h_startpos.lnum
			    && current_ਖਾਨਾ >= sip->si_h_startpos.ਖਾਨਾ))
		    && (sip->si_h_endpos.lnum == 0
			|| current_lnum < sip->si_h_endpos.lnum
			|| (current_lnum == sip->si_h_endpos.lnum
			    && current_ਖਾਨਾ < sip->si_h_endpos.ਖਾਨਾ)))
	    {
		current_attr = sip->si_attr;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		current_id = sip->si_id;
#ਜੇ_ਅੰਤ
		current_trans_id = sip->si_trans_id;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
		current_flags = sip->si_flags;
		current_seqnr = sip->si_seqnr;
		current_sub_char = sip->si_cchar;
#ਜੇ_ਅੰਤ
		ਰੋਕੋ;
	    }
	}

	ਜੇ (can_spell != ਨਿਰਾ)
	{
	    ਬਣਤਰ sp_syn   sps;

	    /*
	     * set "can_spell" to TRUE if spell checking is supposed to be
	     * done in the current item.
	     */
	    ਜੇ (syn_ਖੰਡ->b_spell_cluster_id == 0)
	    {
		// There is no @Spell cluster: Do spelling for items without
		// @NoSpell cluster.
		ਜੇ (syn_ਖੰਡ->b_nospell_cluster_id == 0
						     || current_trans_id == 0)
		    *can_spell = (syn_ਖੰਡ->b_syn_spell != SYNSPL_NOTOP);
		ਨਹੀਂਤਾਂ
		{
		    sps.inc_ਟੈਗ = 0;
		    sps.id = syn_ਖੰਡ->b_nospell_cluster_id;
		    sps.cont_ਵਿੱਚ_list = ਨਿਰਾ;
		    *can_spell = !ਵਿੱਚ_id_list(sip, sip->si_cont_list, &sps, 0);
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// The @Spell cluster is defined: Do spelling in items with
		// the @Spell cluster.  But not when @NoSpell is also there.
		// At the toplevel only spell check when ":syn spell toplevel"
		// was used.
		ਜੇ (current_trans_id == 0)
		    *can_spell = (syn_ਖੰਡ->b_syn_spell == SYNSPL_ਸਿਖਰ);
		ਨਹੀਂਤਾਂ
		{
		    sps.inc_ਟੈਗ = 0;
		    sps.id = syn_ਖੰਡ->b_spell_cluster_id;
		    sps.cont_ਵਿੱਚ_list = ਨਿਰਾ;
		    *can_spell = ਵਿੱਚ_id_list(sip, sip->si_cont_list, &sps, 0);

		    ਜੇ (syn_ਖੰਡ->b_nospell_cluster_id != 0)
		    {
			sps.id = syn_ਖੰਡ->b_nospell_cluster_id;
			ਜੇ (ਵਿੱਚ_id_list(sip, sip->si_cont_list, &sps, 0))
			    *can_spell = FALSE;
		    }
		}
	    }
	}


	/*
	 * Check for end of current state (and the states before it) at the
	 * next column.  Don't do this for syncing, because we would miss a
	 * single character match.
	 * First check if the current state ends at the current column.  It
	 * may be for an empty match and a containing item might end in the
	 * current column.
	 */
	ਜੇ (!syncing && !keep_state)
	{
	    check_state_ਸਿਰੇ();
	    ਜੇ (current_state.ga_len > 0
				      && syn_getcurline()[current_ਖਾਨਾ] != NUL)
	    {
		++current_ਖਾਨਾ;
		check_state_ਸਿਰੇ();
		--current_ਖਾਨਾ;
	    }
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (can_spell != ਨਿਰਾ)
	// Default: Only do spelling when there is no @Spell cluster or when
	// ":syn spell toplevel" was used.
	*can_spell = syn_ਖੰਡ->b_syn_spell == SYNSPL_DEFAULT
		    ? (syn_ਖੰਡ->b_spell_cluster_id == 0)
		    : (syn_ਖੰਡ->b_syn_spell == SYNSPL_ਸਿਖਰ);

    // nextgroup ends at end of line, unless "skipnl" or "skipempty" present
    ਜੇ (current_ਅਗਲਾ_list != ਨਿਰਾ
	    && (ਸਤਰ = syn_getcurline())[current_ਖਾਨਾ] != NUL
	    && ਸਤਰ[current_ਖਾਨਾ + 1] == NUL
	    && !(current_ਅਗਲਾ_flags & (HL_SKIPNL | HL_SKIPEMPTY)))
	current_ਅਗਲਾ_list = ਨਿਰਾ;

    ਜੇ (zero_ਚੌੜਾਈ_ਅਗਲਾ_ga.ga_len > 0)
	ga_clear(&zero_ਚੌੜਾਈ_ਅਗਲਾ_ga);

    // No longer need external matches.  But keep next_match_extmatch.
    unref_extmatch(re_extmatch_ਬਾਹਰਲਾ);
    re_extmatch_ਬਾਹਰਲਾ = ਨਿਰਾ;
    unref_extmatch(cur_extmatch);

    ਵਾਪਸ current_attr;
}


/*
 * Check if we already matched pattern "idx" at the current column.
 */
    ਸਥਿਰ ਪੂਰਨ
did_ਮੇਲ_already(ਪੂਰਨ idx, garray_T *gap)
{
    ਪੂਰਨ		i;

    ਲਈ (i = current_state.ga_len; --i >= 0; )
	ਜੇ (CUR_STATE(i).si_m_startcol == (ਪੂਰਨ)current_ਖਾਨਾ
		&& CUR_STATE(i).si_m_lnum == (ਪੂਰਨ)current_lnum
		&& CUR_STATE(i).si_idx == idx)
	    ਵਾਪਸ TRUE;

    // Zero-width matches with a nextgroup argument are not put on the syntax
    // stack, and can only be matched once anyway.
    ਲਈ (i = gap->ga_len; --i >= 0; )
	ਜੇ (((ਪੂਰਨ *)(gap->ga_ਅੰਕੜੇ))[i] == idx)
	    ਵਾਪਸ TRUE;

    ਵਾਪਸ FALSE;
}

/*
 * Push the next match onto the stack.
 */
    ਸਥਿਰ stateitem_T *
ਧੱਕੋ_ਅਗਲਾ_ਮੇਲ(stateitem_T *cur_si)
{
    synpat_T	*spp;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    ਪੂਰਨ		 save_flags;
#ਜੇ_ਅੰਤ

    spp = &(SYN_ਚੀਜ਼ਾਂ(syn_ਖੰਡ)[ਅਗਲਾ_ਮੇਲ_idx]);

    /*
     * Push the item in current_state stack;
     */
    ਜੇ (ਧੱਕੋ_current_state(ਅਗਲਾ_ਮੇਲ_idx) == OK)
    {
	/*
	 * If it's a start-skip-end type that crosses lines, figure out how
	 * much it continues in this line.  Otherwise just fill in the length.
	 */
	cur_si = &CUR_STATE(current_state.ga_len - 1);
	cur_si->si_h_startpos = ਅਗਲਾ_ਮੇਲ_h_startpos;
	cur_si->si_m_startcol = current_ਖਾਨਾ;
	cur_si->si_m_lnum = current_lnum;
	cur_si->si_flags = spp->sp_flags;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
	cur_si->si_seqnr = ਅਗਲਾ_seqnr++;
	cur_si->si_cchar = spp->sp_cchar;
	ਜੇ (current_state.ga_len > 1)
	    cur_si->si_flags |=
		    CUR_STATE(current_state.ga_len - 2).si_flags & HL_CONCEAL;
#ਜੇ_ਅੰਤ
	cur_si->si_ਅਗਲਾ_list = spp->sp_ਅਗਲਾ_list;
	cur_si->si_extmatch = ref_extmatch(ਅਗਲਾ_ਮੇਲ_extmatch);
	ਜੇ (spp->sp_type == SPTYPE_ਸ਼ੁਰੂ && !(spp->sp_flags & HL_ONELINE))
	{
	    // Try to find the end pattern in the current line
	    update_si_ਸਿਰਾ(cur_si, (ਪੂਰਨ)(ਅਗਲਾ_ਮੇਲ_m_endpos.ਖਾਨਾ), TRUE);
	    check_keepend();
	}
	ਨਹੀਂਤਾਂ
	{
	    cur_si->si_m_endpos = ਅਗਲਾ_ਮੇਲ_m_endpos;
	    cur_si->si_h_endpos = ਅਗਲਾ_ਮੇਲ_h_endpos;
	    cur_si->si_ਸਿਰੇ = TRUE;
	    cur_si->si_flags |= ਅਗਲਾ_ਮੇਲ_flags;
	    cur_si->si_eoe_pos = ਅਗਲਾ_ਮੇਲ_eoe_pos;
	    cur_si->si_ਸਿਰਾ_idx = ਅਗਲਾ_ਮੇਲ_ਸਿਰਾ_idx;
	}
	ਜੇ (keepend_level < 0 && (cur_si->si_flags & HL_KEEPEND))
	    keepend_level = current_state.ga_len - 1;
	check_keepend();
	update_si_attr(current_state.ga_len - 1);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
	save_flags = cur_si->si_flags & (HL_CONCEAL | HL_CONCEALENDS);
#ਜੇ_ਅੰਤ
	/*
	 * If the start pattern has another highlight group, push another item
	 * on the stack for the start pattern.
	 */
	ਜੇ (	   spp->sp_type == SPTYPE_ਸ਼ੁਰੂ
		&& spp->sp_syn_ਮੇਲ_id != 0
		&& ਧੱਕੋ_current_state(ਅਗਲਾ_ਮੇਲ_idx) == OK)
	{
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	    cur_si->si_h_startpos = ਅਗਲਾ_ਮੇਲ_h_startpos;
	    cur_si->si_m_startcol = current_ਖਾਨਾ;
	    cur_si->si_m_lnum = current_lnum;
	    cur_si->si_m_endpos = ਅਗਲਾ_ਮੇਲ_eos_pos;
	    cur_si->si_h_endpos = ਅਗਲਾ_ਮੇਲ_eos_pos;
	    cur_si->si_ਸਿਰੇ = TRUE;
	    cur_si->si_ਸਿਰਾ_idx = 0;
	    cur_si->si_flags = HL_ਮੇਲ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
	    cur_si->si_seqnr = ਅਗਲਾ_seqnr++;
	    cur_si->si_flags |= save_flags;
	    ਜੇ (cur_si->si_flags & HL_CONCEALENDS)
		cur_si->si_flags |= HL_CONCEAL;
#ਜੇ_ਅੰਤ
	    cur_si->si_ਅਗਲਾ_list = ਨਿਰਾ;
	    check_keepend();
	    update_si_attr(current_state.ga_len - 1);
	}
    }

    ਅਗਲਾ_ਮੇਲ_idx = -1;	// try other match next time

    ਵਾਪਸ cur_si;
}

/*
 * Check for end of current state (and the states before it).
 */
    ਸਥਿਰ ਖਾਲੀ
check_state_ਸਿਰੇ(ਖਾਲੀ)
{
    stateitem_T	*cur_si;
    ਪੂਰਨ		had_extend;

    cur_si = &CUR_STATE(current_state.ga_len - 1);
    ਲਈ (;;)
    {
	ਜੇ (cur_si->si_ਸਿਰੇ
		&& (cur_si->si_m_endpos.lnum < current_lnum
		    || (cur_si->si_m_endpos.lnum == current_lnum
			&& cur_si->si_m_endpos.ਖਾਨਾ <= current_ਖਾਨਾ)))
	{
	    /*
	     * If there is an end pattern group ID, highlight the end pattern
	     * now.  No need to pop the current item from the stack.
	     * Only do this if the end pattern continues beyond the current
	     * position.
	     */
	    ਜੇ (cur_si->si_ਸਿਰਾ_idx
		    && (cur_si->si_eoe_pos.lnum > current_lnum
			|| (cur_si->si_eoe_pos.lnum == current_lnum
			    && cur_si->si_eoe_pos.ਖਾਨਾ > current_ਖਾਨਾ)))
	    {
		cur_si->si_idx = cur_si->si_ਸਿਰਾ_idx;
		cur_si->si_ਸਿਰਾ_idx = 0;
		cur_si->si_m_endpos = cur_si->si_eoe_pos;
		cur_si->si_h_endpos = cur_si->si_eoe_pos;
		cur_si->si_flags |= HL_ਮੇਲ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
		cur_si->si_seqnr = ਅਗਲਾ_seqnr++;
		ਜੇ (cur_si->si_flags & HL_CONCEALENDS)
		    cur_si->si_flags |= HL_CONCEAL;
#ਜੇ_ਅੰਤ
		update_si_attr(current_state.ga_len - 1);

		// nextgroup= should not match in the end pattern
		current_ਅਗਲਾ_list = ਨਿਰਾ;

		// what matches next may be different now, clear it
		ਅਗਲਾ_ਮੇਲ_idx = 0;
		ਅਗਲਾ_ਮੇਲ_ਖਾਨਾ = MAXCOL;
		ਰੋਕੋ;
	    }

	    // handle next_list, unless at end of line and no "skipnl" or
	    // "skipempty"
	    current_ਅਗਲਾ_list = cur_si->si_ਅਗਲਾ_list;
	    current_ਅਗਲਾ_flags = cur_si->si_flags;
	    ਜੇ (!(current_ਅਗਲਾ_flags & (HL_SKIPNL | HL_SKIPEMPTY))
		    && syn_getcurline()[current_ਖਾਨਾ] == NUL)
		current_ਅਗਲਾ_list = ਨਿਰਾ;

	    // When the ended item has "extend", another item with
	    // "keepend" now needs to check for its end.
	    had_extend = (cur_si->si_flags & HL_EXTEND);

	    ਕੱਢੋ_current_state();

	    ਜੇ (current_state.ga_len == 0)
		ਰੋਕੋ;

	    ਜੇ (had_extend && keepend_level >= 0)
	    {
		syn_update_ਸਿਰੇ(FALSE);
		ਜੇ (current_state.ga_len == 0)
		    ਰੋਕੋ;
	    }

	    cur_si = &CUR_STATE(current_state.ga_len - 1);

	    /*
	     * Only for a region the search for the end continues after
	     * the end of the contained item.  If the contained match
	     * included the end-of-line, break here, the region continues.
	     * Don't do this when:
	     * - "keepend" is used for the contained item
	     * - not at the end of the line (could be end="x$"me=e-1).
	     * - "excludenl" is used (HL_HAS_EOL won't be set)
	     */
	    ਜੇ (cur_si->si_idx >= 0
		    && SYN_ਚੀਜ਼ਾਂ(syn_ਖੰਡ)[cur_si->si_idx].sp_type
								== SPTYPE_ਸ਼ੁਰੂ
		    && !(cur_si->si_flags & (HL_ਮੇਲ | HL_KEEPEND)))
	    {
		update_si_ਸਿਰਾ(cur_si, (ਪੂਰਨ)current_ਖਾਨਾ, TRUE);
		check_keepend();
		ਜੇ ((current_ਅਗਲਾ_flags & HL_HAS_EOL)
			&& keepend_level < 0
			&& syn_getcurline()[current_ਖਾਨਾ] == NUL)
		    ਰੋਕੋ;
	    }
	}
	ਨਹੀਂਤਾਂ
	    ਰੋਕੋ;
    }
}

/*
 * Update an entry in the current_state stack for a match or region.  This
 * fills in si_attr, si_next_list and si_cont_list.
 */
    ਸਥਿਰ ਖਾਲੀ
update_si_attr(ਪੂਰਨ idx)
{
    stateitem_T	*sip = &CUR_STATE(idx);
    synpat_T	*spp;

    // This should not happen...
    ਜੇ (sip->si_idx < 0)
	ਵਾਪਸ;

    spp = &(SYN_ਚੀਜ਼ਾਂ(syn_ਖੰਡ)[sip->si_idx]);
    ਜੇ (sip->si_flags & HL_ਮੇਲ)
	sip->si_id = spp->sp_syn_ਮੇਲ_id;
    ਨਹੀਂਤਾਂ
	sip->si_id = spp->sp_syn.id;
    sip->si_attr = syn_id2attr(sip->si_id);
    sip->si_trans_id = sip->si_id;
    ਜੇ (sip->si_flags & HL_ਮੇਲ)
	sip->si_cont_list = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
	sip->si_cont_list = spp->sp_cont_list;

    /*
     * For transparent items, take attr from outer item.
     * Also take cont_list, if there is none.
     * Don't do this for the matchgroup of a start or end pattern.
     */
    ਜੇ ((spp->sp_flags & HL_TRANSP) && !(sip->si_flags & HL_ਮੇਲ))
    {
	ਜੇ (idx == 0)
	{
	    sip->si_attr = 0;
	    sip->si_trans_id = 0;
	    ਜੇ (sip->si_cont_list == ਨਿਰਾ)
		sip->si_cont_list = ID_LIST_ALL;
	}
	ਨਹੀਂਤਾਂ
	{
	    sip->si_attr = CUR_STATE(idx - 1).si_attr;
	    sip->si_trans_id = CUR_STATE(idx - 1).si_trans_id;
	    ਜੇ (sip->si_cont_list == ਨਿਰਾ)
	    {
		sip->si_flags |= HL_TRANS_CONT;
		sip->si_cont_list = CUR_STATE(idx - 1).si_cont_list;
	    }
	}
    }
}

/*
 * Check the current stack for patterns with "keepend" flag.
 * Propagate the match-end to contained items, until a "skipend" item is found.
 */
    ਸਥਿਰ ਖਾਲੀ
check_keepend(ਖਾਲੀ)
{
    ਪੂਰਨ		i;
    lpos_T	maxpos;
    lpos_T	maxpos_h;
    stateitem_T	*sip;

    /*
     * This check can consume a lot of time; only do it from the level where
     * there really is a keepend.
     */
    ਜੇ (keepend_level < 0)
	ਵਾਪਸ;

    /*
     * Find the last index of an "extend" item.  "keepend" items before that
     * won't do anything.  If there is no "extend" item "i" will be
     * "keepend_level" and all "keepend" items will work normally.
     */
    ਲਈ (i = current_state.ga_len - 1; i > keepend_level; --i)
	ਜੇ (CUR_STATE(i).si_flags & HL_EXTEND)
	    ਰੋਕੋ;

    maxpos.lnum = 0;
    maxpos.ਖਾਨਾ = 0;
    maxpos_h.lnum = 0;
    maxpos_h.ਖਾਨਾ = 0;
    ਲਈ ( ; i < current_state.ga_len; ++i)
    {
	sip = &CUR_STATE(i);
	ਜੇ (maxpos.lnum != 0)
	{
	    ਹੱਦਬੰਦੀ_pos_zero(&sip->si_m_endpos, &maxpos);
	    ਹੱਦਬੰਦੀ_pos_zero(&sip->si_h_endpos, &maxpos_h);
	    ਹੱਦਬੰਦੀ_pos_zero(&sip->si_eoe_pos, &maxpos);
	    sip->si_ਸਿਰੇ = TRUE;
	}
	ਜੇ (sip->si_ਸਿਰੇ && (sip->si_flags & HL_KEEPEND))
	{
	    ਜੇ (maxpos.lnum == 0
		    || maxpos.lnum > sip->si_m_endpos.lnum
		    || (maxpos.lnum == sip->si_m_endpos.lnum
			&& maxpos.ਖਾਨਾ > sip->si_m_endpos.ਖਾਨਾ))
		maxpos = sip->si_m_endpos;
	    ਜੇ (maxpos_h.lnum == 0
		    || maxpos_h.lnum > sip->si_h_endpos.lnum
		    || (maxpos_h.lnum == sip->si_h_endpos.lnum
			&& maxpos_h.ਖਾਨਾ > sip->si_h_endpos.ਖਾਨਾ))
		maxpos_h = sip->si_h_endpos;
	}
    }
}

/*
 * Update an entry in the current_state stack for a start-skip-end pattern.
 * This finds the end of the current item, if it's in the current line.
 *
 * Return the flags for the matched END.
 */
    ਸਥਿਰ ਖਾਲੀ
update_si_ਸਿਰਾ(
    stateitem_T	*sip,
    ਪੂਰਨ		startcol,   // where to start searching for the end
    ਪੂਰਨ		force)	    // when TRUE overrule a previous end
{
    lpos_T	startpos;
    lpos_T	endpos;
    lpos_T	hl_endpos;
    lpos_T	ਸਿਰਾ_endpos;
    ਪੂਰਨ		ਸਿਰਾ_idx;

    // return quickly for a keyword
    ਜੇ (sip->si_idx < 0)
	ਵਾਪਸ;

    // Don't update when it's already done.  Can be a match of an end pattern
    // that started in a previous line.  Watch out: can also be a "keepend"
    // from a containing item.
    ਜੇ (!force && sip->si_m_endpos.lnum >= current_lnum)
	ਵਾਪਸ;

    /*
     * We need to find the end of the region.  It may continue in the next
     * line.
     */
    ਸਿਰਾ_idx = 0;
    startpos.lnum = current_lnum;
    startpos.ਖਾਨਾ = startcol;
    ਲੱਭੋ_endpos(sip->si_idx, &startpos, &endpos, &hl_endpos,
		   &(sip->si_flags), &ਸਿਰਾ_endpos, &ਸਿਰਾ_idx, sip->si_extmatch);

    ਜੇ (endpos.lnum == 0)
    {
	// No end pattern matched.
	ਜੇ (SYN_ਚੀਜ਼ਾਂ(syn_ਖੰਡ)[sip->si_idx].sp_flags & HL_ONELINE)
	{
	    // a "oneline" never continues in the next line
	    sip->si_ਸਿਰੇ = TRUE;
	    sip->si_m_endpos.lnum = current_lnum;
	    sip->si_m_endpos.ਖਾਨਾ = syn_getcurline_len();
	}
	ਨਹੀਂਤਾਂ
	{
	    // continues in the next line
	    sip->si_ਸਿਰੇ = FALSE;
	    sip->si_m_endpos.lnum = 0;
	}
	sip->si_h_endpos = sip->si_m_endpos;
    }
    ਨਹੀਂਤਾਂ
    {
	// match within this line
	sip->si_m_endpos = endpos;
	sip->si_h_endpos = hl_endpos;
	sip->si_eoe_pos = ਸਿਰਾ_endpos;
	sip->si_ਸਿਰੇ = TRUE;
	sip->si_ਸਿਰਾ_idx = ਸਿਰਾ_idx;
    }
}

/*
 * Add a new state to the current state stack.
 * It is cleared and the index set to "idx".
 * Return FAIL if it's not possible (out of memory).
 */
    ਸਥਿਰ ਪੂਰਨ
ਧੱਕੋ_current_state(ਪੂਰਨ idx)
{
    ਜੇ (ga_grow(&current_state, 1) == FAIL)
	ਵਾਪਸ FAIL;
    CLEAR_ਸੂਚਕ(&CUR_STATE(current_state.ga_len));
    CUR_STATE(current_state.ga_len).si_idx = idx;
    ++current_state.ga_len;
    ਵਾਪਸ OK;
}

/*
 * Remove a state from the current_state stack.
 */
    ਸਥਿਰ ਖਾਲੀ
ਕੱਢੋ_current_state(ਖਾਲੀ)
{
    ਜੇ (current_state.ga_len)
    {
	unref_extmatch(CUR_STATE(current_state.ga_len - 1).si_extmatch);
	--current_state.ga_len;
    }
    // after the end of a pattern, try matching a keyword or pattern
    ਅਗਲਾ_ਮੇਲ_idx = -1;

    // if first state with "keepend" is popped, reset keepend_level
    ਜੇ (keepend_level >= current_state.ga_len)
	keepend_level = -1;
}

/*
 * Find the end of a start/skip/end syntax region after "startpos".
 * Only checks one line.
 * Also handles a match item that continued from a previous line.
 * If not found, the syntax item continues in the next line.  m_endpos->lnum
 * will be 0.
 * If found, the end of the region and the end of the highlighting is
 * computed.
 */
    ਸਥਿਰ ਖਾਲੀ
ਲੱਭੋ_endpos(
    ਪੂਰਨ		idx,		// index of the pattern
    lpos_T	*startpos,	// where to start looking for an END match
    lpos_T	*m_endpos,	// return: end of match
    lpos_T	*hl_endpos,	// return: end of highlighting
    ਲੰਮਾ	*flagsp,	// return: flags of matching END
    lpos_T	*ਸਿਰਾ_endpos,	// return: end of end pattern match
    ਪੂਰਨ		*ਸਿਰਾ_idx,	// return: group ID for end pat. match, or 0
    reg_extmatch_T *ਸ਼ੁਰੂ_ext)	// submatches from the start pattern
{
    colnr_T	matchcol;
    synpat_T	*spp, *spp_skip;
    ਪੂਰਨ		ਸ਼ੁਰੂ_idx;
    ਪੂਰਨ		ਉੱਤਮ_idx;
    regmmatch_T	regmatch;
    regmmatch_T	ਉੱਤਮ_regmatch;	    // startpos/endpos of best match
    lpos_T	pos;
    ਪੂਰਨ		had_ਮੇਲ = FALSE;
    char_u	buf_chartab[32];  // chartab array for syn option iskyeyword

    // just in case we are invoked for a keyword
    ਜੇ (idx < 0)
	ਵਾਪਸ;

    /*
     * Check for being called with a START pattern.
     * Can happen with a match that continues to the next line, because it
     * contained a region.
     */
    spp = &(SYN_ਚੀਜ਼ਾਂ(syn_ਖੰਡ)[idx]);
    ਜੇ (spp->sp_type != SPTYPE_ਸ਼ੁਰੂ)
    {
	*hl_endpos = *startpos;
	ਵਾਪਸ;
    }

    /*
     * Find the SKIP or first END pattern after the last START pattern.
     */
    ਲਈ (;;)
    {
	spp = &(SYN_ਚੀਜ਼ਾਂ(syn_ਖੰਡ)[idx]);
	ਜੇ (spp->sp_type != SPTYPE_ਸ਼ੁਰੂ)
	    ਰੋਕੋ;
	++idx;
    }

    /*
     *	Lookup the SKIP pattern (if present)
     */
    ਜੇ (spp->sp_type == SPTYPE_SKIP)
    {
	spp_skip = spp;
	++idx;
    }
    ਨਹੀਂਤਾਂ
	spp_skip = ਨਿਰਾ;

    // Setup external matches for syn_regexec().
    unref_extmatch(re_extmatch_ਵਿੱਚ);
    re_extmatch_ਵਿੱਚ = ref_extmatch(ਸ਼ੁਰੂ_ext);

    matchcol = startpos->ਖਾਨਾ;	// start looking for a match at sstart
    ਸ਼ੁਰੂ_idx = idx;		// remember the first END pattern.
    ਉੱਤਮ_regmatch.startpos[0].ਖਾਨਾ = 0;		// avoid compiler warning

    // use syntax iskeyword option
    save_chartab(buf_chartab);

    ਲਈ (;;)
    {
	/*
	 * Find end pattern that matches first after "matchcol".
	 */
	ਉੱਤਮ_idx = -1;
	ਲਈ (idx = ਸ਼ੁਰੂ_idx; idx < syn_ਖੰਡ->b_syn_patterns.ga_len; ++idx)
	{
	    ਪੂਰਨ lc_ਖਾਨਾ = matchcol;
	    ਪੂਰਨ r;

	    spp = &(SYN_ਚੀਜ਼ਾਂ(syn_ਖੰਡ)[idx]);
	    ਜੇ (spp->sp_type != SPTYPE_ਸਿਰਾ)	// past last END pattern
		ਰੋਕੋ;
	    lc_ਖਾਨਾ -= spp->sp_offsets[SPO_LC_OFF];
	    ਜੇ (lc_ਖਾਨਾ < 0)
		lc_ਖਾਨਾ = 0;

	    regmatch.rmm_ic = spp->sp_ic;
	    regmatch.regprog = spp->sp_prog;
	    r = syn_regexec(&regmatch, startpos->lnum, lc_ਖਾਨਾ,
						  IF_SYN_ਸਮਾਂ(&spp->sp_ਸਮਾਂ));
	    spp->sp_prog = regmatch.regprog;
	    ਜੇ (r)
	    {
		ਜੇ (ਉੱਤਮ_idx == -1 || regmatch.startpos[0].ਖਾਨਾ
					      < ਉੱਤਮ_regmatch.startpos[0].ਖਾਨਾ)
		{
		    ਉੱਤਮ_idx = idx;
		    ਉੱਤਮ_regmatch.startpos[0] = regmatch.startpos[0];
		    ਉੱਤਮ_regmatch.endpos[0] = regmatch.endpos[0];
		}
	    }
	}

	/*
	 * If all end patterns have been tried, and there is no match, the
	 * item continues until end-of-line.
	 */
	ਜੇ (ਉੱਤਮ_idx == -1)
	    ਰੋਕੋ;

	/*
	 * If the skip pattern matches before the end pattern,
	 * continue searching after the skip pattern.
	 */
	ਜੇ (spp_skip != ਨਿਰਾ)
	{
	    ਪੂਰਨ lc_ਖਾਨਾ = matchcol - spp_skip->sp_offsets[SPO_LC_OFF];
	    ਪੂਰਨ r;

	    ਜੇ (lc_ਖਾਨਾ < 0)
		lc_ਖਾਨਾ = 0;
	    regmatch.rmm_ic = spp_skip->sp_ic;
	    regmatch.regprog = spp_skip->sp_prog;
	    r = syn_regexec(&regmatch, startpos->lnum, lc_ਖਾਨਾ,
					      IF_SYN_ਸਮਾਂ(&spp_skip->sp_ਸਮਾਂ));
	    spp_skip->sp_prog = regmatch.regprog;
	    ਜੇ (r && regmatch.startpos[0].ਖਾਨਾ
					     <= ਉੱਤਮ_regmatch.startpos[0].ਖਾਨਾ)
	    {
		ਪੂਰਨ ਸਤਰ_len;

		// Add offset to skip pattern match
		syn_ਜੋੜੋ_ਸਿਰਾ_off(&pos, &regmatch, spp_skip, SPO_ME_OFF, 1);

		// If the skip pattern goes on to the next line, there is no
		// match with an end pattern in this line.
		ਜੇ (pos.lnum > startpos->lnum)
		    ਰੋਕੋ;

		ਸਤਰ_len = ml_get_buf_len(syn_buf, startpos->lnum);

		// take care of an empty match or negative offset
		ਜੇ (pos.ਖਾਨਾ <= matchcol)
		    ++matchcol;
		ਨਹੀਂਤਾਂ ਜੇ (pos.ਖਾਨਾ <= regmatch.endpos[0].ਖਾਨਾ)
		    matchcol = pos.ਖਾਨਾ;
		ਨਹੀਂਤਾਂ
		    // Be careful not to jump over the NUL at the end-of-line
		    ਲਈ (matchcol = regmatch.endpos[0].ਖਾਨਾ;
			    matchcol < ਸਤਰ_len && matchcol < pos.ਖਾਨਾ;
								   ++matchcol)
			;

		// if the skip pattern includes end-of-line, break here
		ਜੇ (matchcol >= ਸਤਰ_len)
		    ਰੋਕੋ;

		ਜਾਰੀ;	    // start with first end pattern again
	    }
	}

	/*
	 * Match from start pattern to end pattern.
	 * Correct for match and highlight offset of end pattern.
	 */
	spp = &(SYN_ਚੀਜ਼ਾਂ(syn_ਖੰਡ)[ਉੱਤਮ_idx]);
	syn_ਜੋੜੋ_ਸਿਰਾ_off(m_endpos, &ਉੱਤਮ_regmatch, spp, SPO_ME_OFF, 1);
	// can't end before the start
	ਜੇ (m_endpos->lnum == startpos->lnum && m_endpos->ਖਾਨਾ < startpos->ਖਾਨਾ)
	    m_endpos->ਖਾਨਾ = startpos->ਖਾਨਾ;

	syn_ਜੋੜੋ_ਸਿਰਾ_off(ਸਿਰਾ_endpos, &ਉੱਤਮ_regmatch, spp, SPO_HE_OFF, 1);
	// can't end before the start
	ਜੇ (ਸਿਰਾ_endpos->lnum == startpos->lnum
					   && ਸਿਰਾ_endpos->ਖਾਨਾ < startpos->ਖਾਨਾ)
	    ਸਿਰਾ_endpos->ਖਾਨਾ = startpos->ਖਾਨਾ;
	// can't end after the match
	ਹੱਦਬੰਦੀ_pos(ਸਿਰਾ_endpos, m_endpos);

	/*
	 * If the end group is highlighted differently, adjust the pointers.
	 */
	ਜੇ (spp->sp_syn_ਮੇਲ_id != spp->sp_syn.id && spp->sp_syn_ਮੇਲ_id != 0)
	{
	    *ਸਿਰਾ_idx = ਉੱਤਮ_idx;
	    ਜੇ (spp->sp_off_flags & (1 << (SPO_RE_OFF + SPO_ਗਿਣਤੀ)))
	    {
		hl_endpos->lnum = ਉੱਤਮ_regmatch.endpos[0].lnum;
		hl_endpos->ਖਾਨਾ = ਉੱਤਮ_regmatch.endpos[0].ਖਾਨਾ;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		hl_endpos->lnum = ਉੱਤਮ_regmatch.startpos[0].lnum;
		hl_endpos->ਖਾਨਾ = ਉੱਤਮ_regmatch.startpos[0].ਖਾਨਾ;
	    }
	    hl_endpos->ਖਾਨਾ += spp->sp_offsets[SPO_RE_OFF];

	    // can't end before the start
	    ਜੇ (hl_endpos->lnum == startpos->lnum
					    && hl_endpos->ਖਾਨਾ < startpos->ਖਾਨਾ)
		hl_endpos->ਖਾਨਾ = startpos->ਖਾਨਾ;
	    ਹੱਦਬੰਦੀ_pos(hl_endpos, m_endpos);

	    // now the match ends where the highlighting ends, it is turned
	    // into the matchgroup for the end
	    *m_endpos = *hl_endpos;
	}
	ਨਹੀਂਤਾਂ
	{
	    *ਸਿਰਾ_idx = 0;
	    *hl_endpos = *ਸਿਰਾ_endpos;
	}

	*flagsp = spp->sp_flags;

	had_ਮੇਲ = TRUE;
	ਰੋਕੋ;
    }

    // no match for an END pattern in this line
    ਜੇ (!had_ਮੇਲ)
	m_endpos->lnum = 0;

    restore_chartab(buf_chartab);

    // Remove external matches.
    unref_extmatch(re_extmatch_ਵਿੱਚ);
    re_extmatch_ਵਿੱਚ = ਨਿਰਾ;
}

/*
 * Limit "pos" not to be after "limit".
 */
    ਸਥਿਰ ਖਾਲੀ
ਹੱਦਬੰਦੀ_pos(lpos_T *pos, lpos_T *ਹੱਦਬੰਦੀ)
{
    ਜੇ (pos->lnum > ਹੱਦਬੰਦੀ->lnum)
	*pos = *ਹੱਦਬੰਦੀ;
    ਨਹੀਂਤਾਂ ਜੇ (pos->lnum == ਹੱਦਬੰਦੀ->lnum && pos->ਖਾਨਾ > ਹੱਦਬੰਦੀ->ਖਾਨਾ)
	pos->ਖਾਨਾ = ਹੱਦਬੰਦੀ->ਖਾਨਾ;
}

/*
 * Limit "pos" not to be after "limit", unless pos->lnum is zero.
 */
    ਸਥਿਰ ਖਾਲੀ
ਹੱਦਬੰਦੀ_pos_zero(
    lpos_T	*pos,
    lpos_T	*ਹੱਦਬੰਦੀ)
{
    ਜੇ (pos->lnum == 0)
	*pos = *ਹੱਦਬੰਦੀ;
    ਨਹੀਂਤਾਂ
	ਹੱਦਬੰਦੀ_pos(pos, ਹੱਦਬੰਦੀ);
}

/*
 * Add offset to matched text for end of match or highlight.
 */
    ਸਥਿਰ ਖਾਲੀ
syn_ਜੋੜੋ_ਸਿਰਾ_off(
    lpos_T	*ਨਤੀਜਾ,	// returned position
    regmmatch_T	*regmatch,	// start/end of match
    synpat_T	*spp,		// matched pattern
    ਪੂਰਨ		idx,		// index of offset
    ਪੂਰਨ		extra)		// extra chars for offset to start
{
    ਪੂਰਨ		ਖਾਨਾ;
    ਪੂਰਨ		off;
    char_u	*ਅਧਾਰ;
    char_u	*p;

    ਜੇ (spp->sp_off_flags & (1 << idx))
    {
	ਨਤੀਜਾ->lnum = regmatch->startpos[0].lnum;
	ਖਾਨਾ = regmatch->startpos[0].ਖਾਨਾ;
	off = spp->sp_offsets[idx] + extra;
    }
    ਨਹੀਂਤਾਂ
    {
	ਨਤੀਜਾ->lnum = regmatch->endpos[0].lnum;
	ਖਾਨਾ = regmatch->endpos[0].ਖਾਨਾ;
	off = spp->sp_offsets[idx];
    }
    // Don't go past the end of the line.  Matters for "rs=e+2" when there
    // is a matchgroup. Watch out for match with last NL in the buffer.
    ਜੇ (ਨਤੀਜਾ->lnum > syn_buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	ਖਾਨਾ = 0;
    ਨਹੀਂਤਾਂ ਜੇ (off != 0)
    {
	ਅਧਾਰ = ml_get_buf(syn_buf, ਨਤੀਜਾ->lnum, FALSE);
	p = ਅਧਾਰ + ਖਾਨਾ;
	ਜੇ (off > 0)
	{
	    ਜਦੋਂ (off-- > 0 && *p != NUL)
		MB_PTR_ADV(p);
	}
	ਨਹੀਂਤਾਂ ਜੇ (off < 0)
	{
	    ਜਦੋਂ (off++ < 0 && ਅਧਾਰ < p)
		MB_PTR_ਪਿੱਠ(ਅਧਾਰ, p);
	}
	ਖਾਨਾ = (ਪੂਰਨ)(p - ਅਧਾਰ);
    }
    ਨਤੀਜਾ->ਖਾਨਾ = ਖਾਨਾ;
}

/*
 * Add offset to matched text for start of match or highlight.
 * Avoid resulting column to become negative.
 */
    ਸਥਿਰ ਖਾਲੀ
syn_ਜੋੜੋ_ਸ਼ੁਰੂ_off(
    lpos_T	*ਨਤੀਜਾ,	// returned position
    regmmatch_T	*regmatch,	// start/end of match
    synpat_T	*spp,
    ਪੂਰਨ		idx,
    ਪੂਰਨ		extra)	    // extra chars for offset to end
{
    ਪੂਰਨ		ਖਾਨਾ;
    ਪੂਰਨ		off;
    char_u	*ਅਧਾਰ;
    char_u	*p;

    ਜੇ (spp->sp_off_flags & (1 << (idx + SPO_ਗਿਣਤੀ)))
    {
	ਨਤੀਜਾ->lnum = regmatch->endpos[0].lnum;
	ਖਾਨਾ = regmatch->endpos[0].ਖਾਨਾ;
	off = spp->sp_offsets[idx] + extra;
    }
    ਨਹੀਂਤਾਂ
    {
	ਨਤੀਜਾ->lnum = regmatch->startpos[0].lnum;
	ਖਾਨਾ = regmatch->startpos[0].ਖਾਨਾ;
	off = spp->sp_offsets[idx];
    }
    ਜੇ (ਨਤੀਜਾ->lnum > syn_buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	// a "\n" at the end of the pattern may take us below the last line
	ਨਤੀਜਾ->lnum = syn_buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	ਖਾਨਾ = ml_get_buf_len(syn_buf, ਨਤੀਜਾ->lnum);
    }
    ਜੇ (off != 0)
    {
	ਅਧਾਰ = ml_get_buf(syn_buf, ਨਤੀਜਾ->lnum, FALSE);
	p = ਅਧਾਰ + ਖਾਨਾ;
	ਜੇ (off > 0)
	{
	    ਜਦੋਂ (off-- && *p != NUL)
		MB_PTR_ADV(p);
	}
	ਨਹੀਂਤਾਂ ਜੇ (off < 0)
	{
	    ਜਦੋਂ (off++ && ਅਧਾਰ < p)
		MB_PTR_ਪਿੱਠ(ਅਧਾਰ, p);
	}
	ਖਾਨਾ = (ਪੂਰਨ)(p - ਅਧਾਰ);
    }
    ਨਤੀਜਾ->ਖਾਨਾ = ਖਾਨਾ;
}

/*
 * Get current line in syntax buffer.
 */
    ਸਥਿਰ char_u *
syn_getcurline(ਖਾਲੀ)
{
    ਵਾਪਸ ml_get_buf(syn_buf, current_lnum, FALSE);
}

/*
 * Get length of current line in syntax buffer.
 */
    ਸਥਿਰ colnr_T
syn_getcurline_len(ਖਾਲੀ)
{
    ਵਾਪਸ ml_get_buf_len(syn_buf, current_lnum);
}

/*
 * Call vim_regexec() to find a match with "rmp" in "syn_buf".
 * Returns TRUE when there is a match.
 */
    ਸਥਿਰ ਪੂਰਨ
syn_regexec(
    regmmatch_T	*rmp,
    linenr_T	lnum,
    colnr_T	ਖਾਨਾ,
    syn_ਸਮਾਂ_T  *st ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		r;
    ਪੂਰਨ		timed_ਬਾਹਰਲਾ = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    proftime_T	pt;

    ਜੇ (syn_ਸਮਾਂ_on)
	profile_ਸ਼ੁਰੂ(&pt);
#ਜੇ_ਅੰਤ

    ਜੇ (rmp->regprog == ਨਿਰਾ)
	// This can happen if a previous call to vim_regexec_multi() tried to
	// use the NFA engine, which resulted in NFA_TOO_EXPENSIVE, and
	// compiling the pattern with the other engine fails.
	ਵਾਪਸ FALSE;

    rmp->rmm_maxcol = syn_buf->b_p_smc;
    r = vim_regexec_multi(rmp, syn_win, syn_buf, lnum, ਖਾਨਾ, &timed_ਬਾਹਰਲਾ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    ਜੇ (syn_ਸਮਾਂ_on)
    {
	profile_ਸਿਰਾ(&pt);
	profile_ਜੋੜੋ(&st->ਕੁੱਲ, &pt);
	ਜੇ (profile_cmp(&pt, &st->slowest) < 0)
	    st->slowest = pt;
	++st->ਗਿਣਤੀ_2;
	ਜੇ (r > 0)
	    ++st->ਮੇਲ;
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
    ਜੇ (timed_ਬਾਹਰਲਾ && redrawtime_ਹੱਦਬੰਦੀ_ਸੈੱਟ && !syn_win->w_s->b_syn_slow)
    {
	syn_win->w_s->b_syn_slow = TRUE;
	ਸੁਨੇਹਾ(_("'redrawtime' exceeded, syntax highlighting disabled"));
    }
#ਜੇ_ਅੰਤ

    ਜੇ (r > 0)
    {
	rmp->startpos[0].lnum += lnum;
	rmp->endpos[0].lnum += lnum;
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Check one position in a line for a matching keyword.
 * The caller must check if a keyword can start at startcol.
 * Return its ID if found, 0 otherwise.
 */
    ਸਥਿਰ ਪੂਰਨ
check_keyword_id(
    char_u	*ਸਤਰ,
    ਪੂਰਨ		startcol,	// position in line to check for keyword
    ਪੂਰਨ		*endcolp,	// return: character after found keyword
    ਲੰਮਾ	*flagsp,	// return: flags of matching keyword
    ਛੋਟਾ	**ਅਗਲਾ_listp,	// return: next_list of matching keyword
    stateitem_T	*cur_si,	// item at the top of the stack
    ਪੂਰਨ		*ccharp ਅਣਵਰਤਿਆ)	// conceal substitution char
{
    keyentry_T	*kp;
    char_u	*kwp;
    ਪੂਰਨ		ਗੇੜਾ;
    ਪੂਰਨ		kwlen;
    char_u	keyword[MAXKEYWLEN + 1]; // assume max. keyword len is 80
    hashtab_T	*ht;
    hashitem_T	*hi;

    // Find first character after the keyword.  First character was already
    // checked.
    kwp = ਸਤਰ + startcol;
    kwlen = 0;
    ਕਰੋ
    {
	ਜੇ (has_mbyte)
	    kwlen += (*mb_ptr2len)(kwp + kwlen);
	ਨਹੀਂਤਾਂ
	    ++kwlen;
    }
    ਜਦੋਂ (vim_iswordp_buf(kwp + kwlen, syn_buf));

    ਜੇ (kwlen > MAXKEYWLEN)
	ਵਾਪਸ 0;

    /*
     * Must make a copy of the keyword, so we can add a NUL and make it
     * lowercase.
     */
    vim_strncpy(keyword, kwp, kwlen);

    /*
     * Try twice:
     * 1. matching case
     * 2. ignoring case
     */
    ਲਈ (ਗੇੜਾ = 1; ਗੇੜਾ <= 2; ++ਗੇੜਾ)
    {
	ht = ਗੇੜਾ == 1 ? &syn_ਖੰਡ->b_keywtab : &syn_ਖੰਡ->b_keywtab_ic;
	ਜੇ (ht->ht_ਵਰਤਿਆ == 0)
	    ਜਾਰੀ;
	ਜੇ (ਗੇੜਾ == 2)	// ignore case
	    (ਖਾਲੀ)str_foldcase(kwp, kwlen, keyword, MAXKEYWLEN + 1);

	/*
	 * Find keywords that match.  There can be several with different
	 * attributes.
	 * When current_next_list is non-zero accept only that group, otherwise:
	 *  Accept a not-contained keyword at toplevel.
	 *  Accept a keyword at other levels only if it is in the contains list.
	 */
	hi = hash_ਲੱਭੋ(ht, keyword);
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	    ਲਈ (kp = HI2KE(hi); kp != ਨਿਰਾ; kp = kp->ke_ਅਗਲਾ)
	    {
		ਜੇ (current_ਅਗਲਾ_list != 0
			? ਵਿੱਚ_id_list(ਨਿਰਾ, current_ਅਗਲਾ_list, &kp->k_syn, 0)
			: (cur_si == ਨਿਰਾ
			    ? !(kp->flags & HL_CONTAINED)
			    : ਵਿੱਚ_id_list(cur_si, cur_si->si_cont_list,
				      &kp->k_syn, kp->flags)))
		{
		    *endcolp = startcol + kwlen;
		    *flagsp = kp->flags;
		    *ਅਗਲਾ_listp = kp->ਅਗਲਾ_list;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
		    *ccharp = kp->k_char;
#ਜੇ_ਅੰਤ
		    ਵਾਪਸ kp->k_syn.id;
		}
	    }
    }
    ਵਾਪਸ 0;
}

/*
 * Handle ":syntax conceal" command.
 */
    ਸਥਿਰ ਖਾਲੀ
syn_cmd_conceal(exarg_T *eap ਅਣਵਰਤਿਆ, ਪੂਰਨ syncing ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    char_u	*arg = eap->arg;
    char_u	*ਅਗਲਾ;

    eap->nextcmd = ਲੱਭੋ_nextcmd(arg);
    ਜੇ (eap->skip)
	ਵਾਪਸ;

    ਅਗਲਾ = skiptowhite(arg);
    ਜੇ (*arg == NUL)
    {
	ਜੇ (curwin->w_s->b_syn_conceal)
	    ਸੁਨੇਹਾ("syntax conceal on");
	ਨਹੀਂਤਾਂ
	    ਸੁਨੇਹਾ("syntax conceal off");
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(arg, "on", 2) == 0 && ਅਗਲਾ - arg == 2)
	curwin->w_s->b_syn_conceal = TRUE;
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(arg, "off", 3) == 0 && ਅਗਲਾ - arg == 3)
	curwin->w_s->b_syn_conceal = FALSE;
    ਨਹੀਂਤਾਂ
	semsg(_(e_illegal_ਦਲੀਲ_str_2), arg);
#ਜੇ_ਅੰਤ
}

/*
 * Handle ":syntax case" command.
 */
    ਸਥਿਰ ਖਾਲੀ
syn_cmd_case(exarg_T *eap, ਪੂਰਨ syncing ਅਣਵਰਤਿਆ)
{
    char_u	*arg = eap->arg;
    char_u	*ਅਗਲਾ;

    eap->nextcmd = ਲੱਭੋ_nextcmd(arg);
    ਜੇ (eap->skip)
	ਵਾਪਸ;

    ਅਗਲਾ = skiptowhite(arg);
    ਜੇ (*arg == NUL)
    {
	ਜੇ (curwin->w_s->b_syn_ic)
	    ਸੁਨੇਹਾ("syntax case ignore");
	ਨਹੀਂਤਾਂ
	    ਸੁਨੇਹਾ("syntax case match");
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(arg, "match", 5) == 0 && ਅਗਲਾ - arg == 5)
	curwin->w_s->b_syn_ic = FALSE;
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(arg, "ignore", 6) == 0 && ਅਗਲਾ - arg == 6)
	curwin->w_s->b_syn_ic = TRUE;
    ਨਹੀਂਤਾਂ
	semsg(_(e_illegal_ਦਲੀਲ_str_2), arg);
}

/*
 * Handle ":syntax foldlevel" command.
 */
    ਸਥਿਰ ਖਾਲੀ
syn_cmd_foldlevel(exarg_T *eap, ਪੂਰਨ syncing ਅਣਵਰਤਿਆ)
{
    char_u *arg = eap->arg;
    char_u *arg_ਸਿਰਾ;

    eap->nextcmd = ਲੱਭੋ_nextcmd(arg);
    ਜੇ (eap->skip)
	ਵਾਪਸ;

    ਜੇ (*arg == NUL)
    {
	ਚੋਣ (curwin->w_s->b_syn_foldlevel)
	{
	    ਹਾਲਤ SYNFLD_ਸ਼ੁਰੂ:   ਸੁਨੇਹਾ("syntax foldlevel start");   ਰੋਕੋ;
	    ਹਾਲਤ SYNFLD_MINIMUM: ਸੁਨੇਹਾ("syntax foldlevel minimum"); ਰੋਕੋ;
	    ਮੂਲ: ਰੋਕੋ;
	}
	ਵਾਪਸ;
    }

    arg_ਸਿਰਾ = skiptowhite(arg);
    ਜੇ (STRNICMP(arg, "start", 5) == 0 && arg_ਸਿਰਾ - arg == 5)
	curwin->w_s->b_syn_foldlevel = SYNFLD_ਸ਼ੁਰੂ;
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(arg, "minimum", 7) == 0 && arg_ਸਿਰਾ - arg == 7)
	curwin->w_s->b_syn_foldlevel = SYNFLD_MINIMUM;
    ਨਹੀਂਤਾਂ
    {
	semsg(_(e_illegal_ਦਲੀਲ_str_2), arg);
	ਵਾਪਸ;
    }

    arg = skipwhite(arg_ਸਿਰਾ);
    ਜੇ (*arg != NUL)
    {
	semsg(_(e_illegal_ਦਲੀਲ_str_2), arg);
    }
}

/*
 * Handle ":syntax spell" command.
 */
    ਸਥਿਰ ਖਾਲੀ
syn_cmd_spell(exarg_T *eap, ਪੂਰਨ syncing ਅਣਵਰਤਿਆ)
{
    char_u	*arg = eap->arg;
    char_u	*ਅਗਲਾ;

    eap->nextcmd = ਲੱਭੋ_nextcmd(arg);
    ਜੇ (eap->skip)
	ਵਾਪਸ;

    ਅਗਲਾ = skiptowhite(arg);
    ਜੇ (*arg == NUL)
    {
	ਜੇ (curwin->w_s->b_syn_spell == SYNSPL_ਸਿਖਰ)
	    ਸੁਨੇਹਾ("syntax spell toplevel");
	ਨਹੀਂਤਾਂ ਜੇ (curwin->w_s->b_syn_spell == SYNSPL_NOTOP)
	    ਸੁਨੇਹਾ("syntax spell notoplevel");
	ਨਹੀਂਤਾਂ
	    ਸੁਨੇਹਾ("syntax spell default");
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(arg, "toplevel", 8) == 0 && ਅਗਲਾ - arg == 8)
	curwin->w_s->b_syn_spell = SYNSPL_ਸਿਖਰ;
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(arg, "notoplevel", 10) == 0 && ਅਗਲਾ - arg == 10)
	curwin->w_s->b_syn_spell = SYNSPL_NOTOP;
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(arg, "default", 7) == 0 && ਅਗਲਾ - arg == 7)
	curwin->w_s->b_syn_spell = SYNSPL_DEFAULT;
    ਨਹੀਂਤਾਂ
    {
	semsg(_(e_illegal_ਦਲੀਲ_str_2), arg);
	ਵਾਪਸ;
    }

    // assume spell checking changed, force a redraw
    redraw_win_later(curwin, UPD_NOT_VALID);
}

/*
 * Handle ":syntax iskeyword" command.
 */
    ਸਥਿਰ ਖਾਲੀ
syn_cmd_iskeyword(exarg_T *eap, ਪੂਰਨ syncing ਅਣਵਰਤਿਆ)
{
    char_u	*arg = eap->arg;
    char_u	save_chartab[32];
    char_u	*save_isk;

    ਜੇ (eap->skip)
	ਵਾਪਸ;

    arg = skipwhite(arg);
    ਜੇ (*arg == NUL)
    {
	ਸੁਨੇਹਾ_puts("\n");
	ਜੇ (curwin->w_s->b_syn_isk != ਖਾਲੀ_ਥਾਵਾਂ_option)
	{
	    ਸੁਨੇਹਾ_puts("syntax iskeyword ");
	    ਸੁਨੇਹਾ_outtrans(curwin->w_s->b_syn_isk);
	}
	ਨਹੀਂਤਾਂ
	    ਸੁਨੇਹਾ_outtrans((char_u *)_("syntax iskeyword not set"));
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (STRNICMP(arg, "clear", 5) == 0)
	{
	    mch_memmove(curwin->w_s->b_syn_chartab, curbuf->b_chartab,
								  (size_t)32);
	    clear_string_option(&curwin->w_s->b_syn_isk);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (check_isopt(arg) == FAIL)
	    {
		emsg(_(e_invalid_ਦਲੀਲ));
		ਵਾਪਸ;
	    }
	    mch_memmove(save_chartab, curbuf->b_chartab, (size_t)32);
	    save_isk = curbuf->b_p_isk;
	    curbuf->b_p_isk = vim_strsave(arg);

	    buf_init_chartab(curbuf, FALSE);
	    mch_memmove(curwin->w_s->b_syn_chartab, curbuf->b_chartab,
								  (size_t)32);
	    mch_memmove(curbuf->b_chartab, save_chartab, (size_t)32);
	    clear_string_option(&curwin->w_s->b_syn_isk);
	    curwin->w_s->b_syn_isk = curbuf->b_p_isk;
	    curbuf->b_p_isk = save_isk;
	}
    }
    redraw_win_later(curwin, UPD_NOT_VALID);
}

/*
 * Clear all syntax info for one buffer.
 */
    ਖਾਲੀ
syntax_clear(synblock_T *ਖੰਡ)
{
    ਪੂਰਨ i;

    ਖੰਡ->b_syn_error = FALSE;	    // clear previous error
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
    ਖੰਡ->b_syn_slow = FALSE;	    // clear previous timeout
#ਜੇ_ਅੰਤ
    ਖੰਡ->b_syn_ic = FALSE;	    // Use case, by default
    ਖੰਡ->b_syn_foldlevel = SYNFLD_ਸ਼ੁਰੂ;
    ਖੰਡ->b_syn_spell = SYNSPL_DEFAULT; // default spell checking
    ਖੰਡ->b_syn_containedin = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    ਖੰਡ->b_syn_conceal = FALSE;
#ਜੇ_ਅੰਤ

    // free the keywords
    clear_keywtab(&ਖੰਡ->b_keywtab);
    clear_keywtab(&ਖੰਡ->b_keywtab_ic);

    // free the syntax patterns
    ਲਈ (i = ਖੰਡ->b_syn_patterns.ga_len; --i >= 0; )
	syn_clear_ਨਮੂਨਾ(ਖੰਡ, i);
    ga_clear(&ਖੰਡ->b_syn_patterns);

    // free the syntax clusters
    ਲਈ (i = ਖੰਡ->b_syn_clusters.ga_len; --i >= 0; )
	syn_clear_cluster(ਖੰਡ, i);
    ga_clear(&ਖੰਡ->b_syn_clusters);
    ਖੰਡ->b_spell_cluster_id = 0;
    ਖੰਡ->b_nospell_cluster_id = 0;

    ਖੰਡ->b_syn_sync_flags = 0;
    ਖੰਡ->b_syn_sync_minlines = 0;
    ਖੰਡ->b_syn_sync_maxlines = 0;
    ਖੰਡ->b_syn_sync_linebreaks = 0;

    vim_regfree(ਖੰਡ->b_syn_linecont_prog);
    ਖੰਡ->b_syn_linecont_prog = ਨਿਰਾ;
    VIM_CLEAR(ਖੰਡ->b_syn_linecont_pat);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਖੰਡ->b_syn_folditems = 0;
#ਜੇ_ਅੰਤ
    clear_string_option(&ਖੰਡ->b_syn_isk);

    // free the stored states
    syn_ਢੇਰ_free_all(ਖੰਡ);
    invalidate_current_state();

    // Reset the counter for ":syn include"
    running_syn_inc_ਟੈਗ = 0;
}

/*
 * Get rid of ownsyntax for window "wp".
 */
    ਖਾਲੀ
reset_synblock(win_T *wp)
{
    ਜੇ (wp->w_s != &wp->w_ਬਫਰ->b_s)
    {
	syntax_clear(wp->w_s);
	vim_free(wp->w_s);
	wp->w_s = &wp->w_ਬਫਰ->b_s;
    }
}

/*
 * Clear syncing info for one buffer.
 */
    ਸਥਿਰ ਖਾਲੀ
syntax_sync_clear(ਖਾਲੀ)
{
    ਪੂਰਨ i;

    // free the syntax patterns
    ਲਈ (i = curwin->w_s->b_syn_patterns.ga_len; --i >= 0; )
	ਜੇ (SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[i].sp_syncing)
	    syn_remove_ਨਮੂਨਾ(curwin->w_s, i);

    curwin->w_s->b_syn_sync_flags = 0;
    curwin->w_s->b_syn_sync_minlines = 0;
    curwin->w_s->b_syn_sync_maxlines = 0;
    curwin->w_s->b_syn_sync_linebreaks = 0;

    vim_regfree(curwin->w_s->b_syn_linecont_prog);
    curwin->w_s->b_syn_linecont_prog = ਨਿਰਾ;
    VIM_CLEAR(curwin->w_s->b_syn_linecont_pat);
    clear_string_option(&curwin->w_s->b_syn_isk);

    syn_ਢੇਰ_free_all(curwin->w_s);	// Need to recompute all syntax.
}

/*
 * Remove one pattern from the buffer's pattern list.
 */
    ਸਥਿਰ ਖਾਲੀ
syn_remove_ਨਮੂਨਾ(
    synblock_T	*ਖੰਡ,
    ਪੂਰਨ		idx)
{
    synpat_T	*spp;

    spp = &(SYN_ਚੀਜ਼ਾਂ(ਖੰਡ)[idx]);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਜੇ (spp->sp_flags & HL_FOLD)
	--ਖੰਡ->b_syn_folditems;
#ਜੇ_ਅੰਤ
    syn_clear_ਨਮੂਨਾ(ਖੰਡ, idx);
    mch_memmove(spp, spp + 1,
		   ਆਕਾਰ(synpat_T) * (ਖੰਡ->b_syn_patterns.ga_len - idx - 1));
    --ਖੰਡ->b_syn_patterns.ga_len;
}

/*
 * Clear and free one syntax pattern.  When clearing all, must be called from
 * last to first!
 */
    ਸਥਿਰ ਖਾਲੀ
syn_clear_ਨਮੂਨਾ(synblock_T *ਖੰਡ, ਪੂਰਨ i)
{
    vim_free(SYN_ਚੀਜ਼ਾਂ(ਖੰਡ)[i].sp_ਨਮੂਨਾ);
    vim_regfree(SYN_ਚੀਜ਼ਾਂ(ਖੰਡ)[i].sp_prog);
    // Only free sp_cont_list and sp_next_list of first start pattern
    ਜੇ (i == 0 || SYN_ਚੀਜ਼ਾਂ(ਖੰਡ)[i - 1].sp_type != SPTYPE_ਸ਼ੁਰੂ)
    {
	vim_free(SYN_ਚੀਜ਼ਾਂ(ਖੰਡ)[i].sp_cont_list);
	vim_free(SYN_ਚੀਜ਼ਾਂ(ਖੰਡ)[i].sp_ਅਗਲਾ_list);
	vim_free(SYN_ਚੀਜ਼ਾਂ(ਖੰਡ)[i].sp_syn.cont_ਵਿੱਚ_list);
    }
}

/*
 * Clear and free one syntax cluster.
 */
    ਸਥਿਰ ਖਾਲੀ
syn_clear_cluster(synblock_T *ਖੰਡ, ਪੂਰਨ i)
{
    vim_free(SYN_CLSTR(ਖੰਡ)[i].scl_name);
    vim_free(SYN_CLSTR(ਖੰਡ)[i].scl_name_u);
    vim_free(SYN_CLSTR(ਖੰਡ)[i].scl_list);
}

/*
 * Handle ":syntax clear" command.
 */
    ਸਥਿਰ ਖਾਲੀ
syn_cmd_clear(exarg_T *eap, ਪੂਰਨ syncing)
{
    char_u	*arg = eap->arg;
    char_u	*arg_ਸਿਰਾ;
    ਪੂਰਨ		id;

    eap->nextcmd = ਲੱਭੋ_nextcmd(arg);
    ਜੇ (eap->skip)
	ਵਾਪਸ;

    /*
     * We have to disable this within ":syn include @group filename",
     * because otherwise @group would get deleted.
     * Only required for Vim 5.x syntax files, 6.0 ones don't contain ":syn
     * clear".
     */
    ਜੇ (curwin->w_s->b_syn_topgrp != 0)
	ਵਾਪਸ;

    ਜੇ (ਸਿਰੇ_excmd2(eap->cmd, arg))
    {
	/*
	 * No argument: Clear all syntax items.
	 */
	ਜੇ (syncing)
	    syntax_sync_clear();
	ਨਹੀਂਤਾਂ
	{
	    syntax_clear(curwin->w_s);
	    ਜੇ (curwin->w_s == &curwin->w_ਬਫਰ->b_s)
		do_unlet((char_u *)"b:current_syntax", TRUE);
	    do_unlet((char_u *)"w:current_syntax", TRUE);
	}
    }
    ਨਹੀਂਤਾਂ
    {
	/*
	 * Clear the group IDs that are in the argument.
	 */
	ਜਦੋਂ (!ਸਿਰੇ_excmd2(eap->cmd, arg))
	{
	    arg_ਸਿਰਾ = skiptowhite(arg);
	    ਜੇ (*arg == '@')
	    {
		id = syn_scl_namen2id(arg + 1, (ਪੂਰਨ)(arg_ਸਿਰਾ - arg - 1));
		ਜੇ (id == 0)
		{
		    semsg(_(e_no_such_syntax_cluster_str_1), arg);
		    ਰੋਕੋ;
		}
		ਨਹੀਂਤਾਂ
		{
		    /*
		     * We can't physically delete a cluster without changing
		     * the IDs of other clusters, so we do the next best thing
		     * and make it empty.
		     */
		    ਛੋਟਾ scl_id = id - SYNID_CLUSTER;

		    VIM_CLEAR(SYN_CLSTR(curwin->w_s)[scl_id].scl_list);
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		id = syn_namen2id(arg, (ਪੂਰਨ)(arg_ਸਿਰਾ - arg));
		ਜੇ (id == 0)
		{
		    semsg(_(e_no_such_highlight_ਸਮੂਹ_name_str), arg);
		    ਰੋਕੋ;
		}
		ਨਹੀਂਤਾਂ
		    syn_clear_one(id, syncing);
	    }
	    arg = skipwhite(arg_ਸਿਰਾ);
	}
    }
    redraw_curbuf_later(UPD_SOME_VALID);
    syn_ਢੇਰ_free_all(curwin->w_s);		// Need to recompute all syntax.
}

/*
 * Clear one syntax group for the current buffer.
 */
    ਸਥਿਰ ਖਾਲੀ
syn_clear_one(ਪੂਰਨ id, ਪੂਰਨ syncing)
{
    synpat_T	*spp;
    ਪੂਰਨ		idx;

    // Clear keywords only when not ":syn sync clear group-name"
    ਜੇ (!syncing)
    {
	(ਖਾਲੀ)syn_clear_keyword(id, &curwin->w_s->b_keywtab);
	(ਖਾਲੀ)syn_clear_keyword(id, &curwin->w_s->b_keywtab_ic);
    }

    // clear the patterns for "id"
    ਲਈ (idx = curwin->w_s->b_syn_patterns.ga_len; --idx >= 0; )
    {
	spp = &(SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx]);
	ਜੇ (spp->sp_syn.id != id || spp->sp_syncing != syncing)
	    ਜਾਰੀ;
	syn_remove_ਨਮੂਨਾ(curwin->w_s, idx);
    }
}

/*
 * Handle ":syntax on" command.
 */
    ਸਥਿਰ ਖਾਲੀ
syn_cmd_on(exarg_T *eap, ਪੂਰਨ syncing ਅਣਵਰਤਿਆ)
{
    syn_cmd_onoff(eap, "syntax");
}

/*
 * Handle ":syntax enable" command.
 */
    ਸਥਿਰ ਖਾਲੀ
syn_cmd_enable(exarg_T *eap, ਪੂਰਨ syncing ਅਣਵਰਤਿਆ)
{
    ਸੈੱਟ_ਅੰਦਰੂਨੀ_string_var((char_u *)"g:syntax_cmd", (char_u *)"enable");
    syn_cmd_onoff(eap, "syntax");
    do_unlet((char_u *)"g:syntax_cmd", TRUE);
}

/*
 * Handle ":syntax reset" command.
 * It actually resets highlighting, not syntax.
 */
    ਸਥਿਰ ਖਾਲੀ
syn_cmd_reset(exarg_T *eap, ਪੂਰਨ syncing ਅਣਵਰਤਿਆ)
{
    ਸੈੱਟ_nextcmd(eap, eap->arg);
    ਜੇ (!eap->skip)
    {
	ਸੈੱਟ_ਅੰਦਰੂਨੀ_string_var((char_u *)"g:syntax_cmd", (char_u *)"reset");
	do_cmdline_cmd((char_u *)"runtime! syntax/syncolor.vim");
	do_unlet((char_u *)"g:syntax_cmd", TRUE);
    }
}

/*
 * Handle ":syntax manual" command.
 */
    ਸਥਿਰ ਖਾਲੀ
syn_cmd_manual(exarg_T *eap, ਪੂਰਨ syncing ਅਣਵਰਤਿਆ)
{
    syn_cmd_onoff(eap, "manual");
}

/*
 * Handle ":syntax off" command.
 */
    ਸਥਿਰ ਖਾਲੀ
syn_cmd_off(exarg_T *eap, ਪੂਰਨ syncing ਅਣਵਰਤਿਆ)
{
    syn_cmd_onoff(eap, "nosyntax");
}

    ਸਥਿਰ ਖਾਲੀ
syn_cmd_onoff(exarg_T *eap, ਅੱਖਰ *name)
{
    char_u	buf[100];

    ਸੈੱਟ_nextcmd(eap, eap->arg);
    ਜੇ (!eap->skip)
    {
	STRCPY(buf, "so ");
	vim_snprintf((ਅੱਖਰ *)buf + 3, ਆਕਾਰ(buf) - 3, SYNTAX_FNAME, name);
	do_cmdline_cmd(buf);
    }
}

/*
 * Handle ":syntax [list]" command: list current syntax words.
 */
    ਸਥਿਰ ਖਾਲੀ
syn_cmd_list(
    exarg_T	*eap,
    ਪੂਰਨ		syncing)	    // when TRUE: list syncing items
{
    char_u	*arg = eap->arg;
    ਪੂਰਨ		id;
    char_u	*arg_ਸਿਰਾ;

    eap->nextcmd = ਲੱਭੋ_nextcmd(arg);
    ਜੇ (eap->skip)
	ਵਾਪਸ;

    ਜੇ (!syntax_present(curwin))
    {
	ਸੁਨੇਹਾ(_(ਸੁਨੇਹਾ_no_ਚੀਜ਼ਾਂ));
	ਵਾਪਸ;
    }

    ਜੇ (syncing)
    {
	ਜੇ (curwin->w_s->b_syn_sync_flags & SF_CCOMMENT)
	{
	    ਸੁਨੇਹਾ_puts(_("syncing on C-style comments"));
	    syn_ਸਤਰਾਂ_ਸੁਨੇਹਾ();
	    syn_ਮੇਲ_ਸੁਨੇਹਾ();
	}
	ਨਹੀਂਤਾਂ ਜੇ (curwin->w_s->b_syn_sync_flags & SF_ਮੇਲ)
	{
	    ਸੁਨੇਹਾ_puts_title(_("\n--- Syntax sync items ---"));
	    ਜੇ (curwin->w_s->b_syn_sync_minlines > 0
		|| curwin->w_s->b_syn_sync_maxlines > 0
		|| curwin->w_s->b_syn_sync_linebreaks > 0)
	    {
		ਸੁਨੇਹਾ_puts(_("\nsyncing on items"));
		syn_ਸਤਰਾਂ_ਸੁਨੇਹਾ();
		syn_ਮੇਲ_ਸੁਨੇਹਾ();
	    }
	    ਲਈ (id = 1; id <= highlight_ਅੰਕ_groups() && !got_int; ++id)
		syn_list_one(id, syncing, FALSE);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (curwin->w_s->b_syn_sync_minlines == 0)
		ਸੁਨੇਹਾ_puts(_("no syncing"));
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (curwin->w_s->b_syn_sync_minlines == MAXLNUM)
		    ਸੁਨੇਹਾ_puts(_("syncing starts at the first line"));
		ਨਹੀਂਤਾਂ
		{
		    ਸੁਨੇਹਾ_puts(_("syncing starts "));
		    ਸੁਨੇਹਾ_outnum(curwin->w_s->b_syn_sync_minlines);
		    ਸੁਨੇਹਾ_puts(_(" lines before top line"));
		}
		syn_ਮੇਲ_ਸੁਨੇਹਾ();
	    }
	}
	ਵਾਪਸ;
    }

    ਸੁਨੇਹਾ_puts_title(_("\n--- Syntax items ---"));
    ਜੇ (ਸਿਰੇ_excmd2(eap->cmd, arg))
    {
	/*
	 * No argument: List all group IDs and all syntax clusters.
	 */
	ਲਈ (id = 1; id <= highlight_ਅੰਕ_groups() && !got_int; ++id)
	    syn_list_one(id, syncing, FALSE);
	ਲਈ (id = 0; id < curwin->w_s->b_syn_clusters.ga_len && !got_int; ++id)
	    syn_list_cluster(id);
    }
    ਨਹੀਂਤਾਂ
    {
	/*
	 * List the group IDs and syntax clusters that are in the argument.
	 */
	ਜਦੋਂ (!ਸਿਰੇ_excmd2(eap->cmd, arg) && !got_int)
	{
	    arg_ਸਿਰਾ = skiptowhite(arg);
	    ਜੇ (*arg == '@')
	    {
		id = syn_scl_namen2id(arg + 1, (ਪੂਰਨ)(arg_ਸਿਰਾ - arg - 1));
		ਜੇ (id == 0)
		    semsg(_(e_no_such_syntax_cluster_str_2), arg);
		ਨਹੀਂਤਾਂ
		    syn_list_cluster(id - SYNID_CLUSTER);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		id = syn_namen2id(arg, (ਪੂਰਨ)(arg_ਸਿਰਾ - arg));
		ਜੇ (id == 0)
		    semsg(_(e_no_such_highlight_ਸਮੂਹ_name_str), arg);
		ਨਹੀਂਤਾਂ
		    syn_list_one(id, syncing, TRUE);
	    }
	    arg = skipwhite(arg_ਸਿਰਾ);
	}
    }
    ਸੈੱਟ_nextcmd(eap, arg);
}

    ਸਥਿਰ ਖਾਲੀ
syn_ਸਤਰਾਂ_ਸੁਨੇਹਾ(ਖਾਲੀ)
{
    ਜੇ (curwin->w_s->b_syn_sync_maxlines > 0
				      || curwin->w_s->b_syn_sync_minlines > 0)
    {
	ਸੁਨੇਹਾ_puts("; ");
	ਜੇ (curwin->w_s->b_syn_sync_minlines == MAXLNUM)
	    ਸੁਨੇਹਾ_puts(_("from the first line"));
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (curwin->w_s->b_syn_sync_minlines > 0)
	    {
		ਸੁਨੇਹਾ_puts(_("minimal "));
		ਸੁਨੇਹਾ_outnum(curwin->w_s->b_syn_sync_minlines);
		ਜੇ (curwin->w_s->b_syn_sync_maxlines)
		    ਸੁਨੇਹਾ_puts(", ");
	    }
	    ਜੇ (curwin->w_s->b_syn_sync_maxlines > 0)
	    {
		ਸੁਨੇਹਾ_puts(_("maximal "));
		ਸੁਨੇਹਾ_outnum(curwin->w_s->b_syn_sync_maxlines);
	    }
	    ਸੁਨੇਹਾ_puts(_(" lines before top line"));
	}
    }
}

    ਸਥਿਰ ਖਾਲੀ
syn_ਮੇਲ_ਸੁਨੇਹਾ(ਖਾਲੀ)
{
    ਜੇ (curwin->w_s->b_syn_sync_linebreaks > 0)
    {
	ਸੁਨੇਹਾ_puts(_("; match "));
	ਸੁਨੇਹਾ_outnum(curwin->w_s->b_syn_sync_linebreaks);
	ਸੁਨੇਹਾ_puts(_(" line breaks"));
    }
}

ਸਥਿਰ ਪੂਰਨ  ਆਖਰੀ_matchgroup;

ਸਥਿਰ ਖਾਲੀ syn_list_flags(keyvalue_T *nlist, ਪੂਰਨ nr_entries, ਪੂਰਨ flags, ਪੂਰਨ attr);

/*
 * List one syntax item, for ":syntax" or "syntax list syntax_name".
 */
    ਸਥਿਰ ਖਾਲੀ
syn_list_one(
    ਪੂਰਨ		id,
    ਪੂਰਨ		syncing,	    // when TRUE: list syncing items
    ਪੂਰਨ		link_only)	    // when TRUE; list link-only too
{
    ਪੂਰਨ		attr;
    ਪੂਰਨ		idx;
    ਪੂਰਨ		did_ਸਿਰਲੇਖ = FALSE;
    synpat_T	*spp;
    ਸਥਿਰ keyvalue_T namelist1[] =
		{
		    KEYVALUE_ਇੰਦਰਾਜ(HL_DISPLAY, "display"),
		    KEYVALUE_ਇੰਦਰਾਜ(HL_CONTAINED, "contained"),
		    KEYVALUE_ਇੰਦਰਾਜ(HL_ONELINE, "oneline"),
		    KEYVALUE_ਇੰਦਰਾਜ(HL_KEEPEND, "keepend"),
		    KEYVALUE_ਇੰਦਰਾਜ(HL_EXTEND, "extend"),
		    KEYVALUE_ਇੰਦਰਾਜ(HL_EXCLUDENL, "excludenl"),
		    KEYVALUE_ਇੰਦਰਾਜ(HL_TRANSP, "transparent"),
		    KEYVALUE_ਇੰਦਰਾਜ(HL_FOLD, "fold")
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
		    ,
		    KEYVALUE_ਇੰਦਰਾਜ(HL_CONCEAL, "conceal"),
		    KEYVALUE_ਇੰਦਰਾਜ(HL_CONCEALENDS, "concealends")
#ਜੇ_ਅੰਤ
		};
    ਸਥਿਰ keyvalue_T namelist2[] =
		{
		    KEYVALUE_ਇੰਦਰਾਜ(HL_SKIPWHITE, "skipwhite"),
		    KEYVALUE_ਇੰਦਰਾਜ(HL_SKIPNL, "skipnl"),
		    KEYVALUE_ਇੰਦਰਾਜ(HL_SKIPEMPTY, "skipempty")
		};

    attr = HL_ATTR(HLF_D);		// highlight like directories

    // list the keywords for "id"
    ਜੇ (!syncing)
    {
	did_ਸਿਰਲੇਖ = syn_list_keywords(id, &curwin->w_s->b_keywtab, FALSE, attr);
	did_ਸਿਰਲੇਖ = syn_list_keywords(id, &curwin->w_s->b_keywtab_ic,
							    did_ਸਿਰਲੇਖ, attr);
    }

    // list the patterns for "id"
    ਲਈ (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len && !got_int; ++idx)
    {
	spp = &(SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx]);
	ਜੇ (spp->sp_syn.id != id || spp->sp_syncing != syncing)
	    ਜਾਰੀ;

	(ਖਾਲੀ)syn_list_ਸਿਰਲੇਖ(did_ਸਿਰਲੇਖ, 999, id);
	did_ਸਿਰਲੇਖ = TRUE;
	ਆਖਰੀ_matchgroup = 0;
	ਜੇ (spp->sp_type == SPTYPE_ਮੇਲ)
	{
	    put_ਨਮੂਨਾ("match", ' ', spp, attr);
	    ਸੁਨੇਹਾ_putchar(' ');
	}
	ਨਹੀਂਤਾਂ ਜੇ (spp->sp_type == SPTYPE_ਸ਼ੁਰੂ)
	{
	    ਜਦੋਂ (SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx].sp_type == SPTYPE_ਸ਼ੁਰੂ)
		put_ਨਮੂਨਾ("start", '=', &SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx++], attr);
	    ਜੇ (SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx].sp_type == SPTYPE_SKIP)
		put_ਨਮੂਨਾ("skip", '=', &SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx++], attr);
	    ਜਦੋਂ (idx < curwin->w_s->b_syn_patterns.ga_len
			  && SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx].sp_type == SPTYPE_ਸਿਰਾ)
		put_ਨਮੂਨਾ("end", '=', &SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx++], attr);
	    --idx;
	    ਸੁਨੇਹਾ_putchar(' ');
	}
	syn_list_flags(namelist1, (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(namelist1), spp->sp_flags, attr);

	ਜੇ (spp->sp_cont_list != ਨਿਰਾ)
	    put_id_list((char_u *)"contains", spp->sp_cont_list, attr);

	ਜੇ (spp->sp_syn.cont_ਵਿੱਚ_list != ਨਿਰਾ)
	    put_id_list((char_u *)"containedin",
					      spp->sp_syn.cont_ਵਿੱਚ_list, attr);

	ਜੇ (spp->sp_ਅਗਲਾ_list != ਨਿਰਾ)
	{
	    put_id_list((char_u *)"nextgroup", spp->sp_ਅਗਲਾ_list, attr);
	    syn_list_flags(namelist2, (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(namelist2), spp->sp_flags, attr);
	}
	ਜੇ (spp->sp_flags & (HL_SYNC_ਇੱਥੇ|HL_SYNC_THERE))
	{
	    ਜੇ (spp->sp_flags & HL_SYNC_ਇੱਥੇ)
		ਸੁਨੇਹਾ_puts_attr("grouphere", attr);
	    ਨਹੀਂਤਾਂ
		ਸੁਨੇਹਾ_puts_attr("groupthere", attr);
	    ਸੁਨੇਹਾ_putchar(' ');
	    ਜੇ (spp->sp_sync_idx >= 0)
		ਸੁਨੇਹਾ_outtrans(highlight_ਸਮੂਹ_name(SYN_ਚੀਜ਼ਾਂ(curwin->w_s)
				   [spp->sp_sync_idx].sp_syn.id - 1));
	    ਨਹੀਂਤਾਂ
		ਸੁਨੇਹਾ_puts("NONE");
	    ਸੁਨੇਹਾ_putchar(' ');
	}
    }

    // list the link, if there is one
    ਜੇ (highlight_link_id(id - 1) && (did_ਸਿਰਲੇਖ || link_only) && !got_int)
    {
	(ਖਾਲੀ)syn_list_ਸਿਰਲੇਖ(did_ਸਿਰਲੇਖ, 999, id);
	ਸੁਨੇਹਾ_puts_attr("links to", attr);
	ਸੁਨੇਹਾ_putchar(' ');
	ਸੁਨੇਹਾ_outtrans(highlight_ਸਮੂਹ_name(highlight_link_id(id - 1) - 1));
    }
}

    ਸਥਿਰ ਖਾਲੀ
syn_list_flags(keyvalue_T *nlist, ਪੂਰਨ nr_entries, ਪੂਰਨ flags, ਪੂਰਨ attr)
{
    ਪੂਰਨ		i;

    ਲਈ (i = 0; i < nr_entries; ++i)
	ਜੇ (flags & nlist[i].ਕੁੰਜੀ)
	{
	    ਸੁਨੇਹਾ_puts_attr((ਅੱਖਰ *)nlist[i].ਮੁੱਲ.string, attr);
	    ਸੁਨੇਹਾ_putchar(' ');
	}
}

/*
 * List one syntax cluster, for ":syntax" or "syntax list syntax_name".
 */
    ਸਥਿਰ ਖਾਲੀ
syn_list_cluster(ਪੂਰਨ id)
{
    ਪੂਰਨ	    endcol = 15;

    // slight hack:  roughly duplicate the guts of syn_list_header()
    ਸੁਨੇਹਾ_putchar('\n');
    ਸੁਨੇਹਾ_outtrans(SYN_CLSTR(curwin->w_s)[id].scl_name);

    ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ >= endcol)	// output at least one space
	endcol = ਸੁਨੇਹਾ_ਖਾਨਾ + 1;
    ਜੇ (cmdline_ਚੌੜਾਈ <= endcol)	// avoid hang for tiny window
	endcol = cmdline_ਚੌੜਾਈ - 1;

    ਸੁਨੇਹਾ_advance(endcol);
    ਜੇ (SYN_CLSTR(curwin->w_s)[id].scl_list != ਨਿਰਾ)
    {
	put_id_list((char_u *)"cluster", SYN_CLSTR(curwin->w_s)[id].scl_list,
		    HL_ATTR(HLF_D));
    }
    ਨਹੀਂਤਾਂ
    {
	ਸੁਨੇਹਾ_puts_attr("cluster", HL_ATTR(HLF_D));
	ਸੁਨੇਹਾ_puts("=NONE");
    }
}

    ਸਥਿਰ ਖਾਲੀ
put_id_list(char_u *name, ਛੋਟਾ *list, ਪੂਰਨ attr)
{
    ਛੋਟਾ		*p;

    ਸੁਨੇਹਾ_puts_attr((ਅੱਖਰ *)name, attr);
    ਸੁਨੇਹਾ_putchar('=');
    ਲਈ (p = list; *p; ++p)
    {
	ਜੇ (*p >= SYNID_ALLBUT && *p < SYNID_ਸਿਖਰ)
	{
	    ਜੇ (p[1])
		ਸੁਨੇਹਾ_puts("ALLBUT");
	    ਨਹੀਂਤਾਂ
		ਸੁਨੇਹਾ_puts("ALL");
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p >= SYNID_ਸਿਖਰ && *p < SYNID_CONTAINED)
	{
	    ਸੁਨੇਹਾ_puts("TOP");
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p >= SYNID_CONTAINED && *p < SYNID_CLUSTER)
	{
	    ਸੁਨੇਹਾ_puts("CONTAINED");
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p >= SYNID_CLUSTER)
	{
	    ਛੋਟਾ scl_id = *p - SYNID_CLUSTER;

	    ਸੁਨੇਹਾ_putchar('@');
	    ਸੁਨੇਹਾ_outtrans(SYN_CLSTR(curwin->w_s)[scl_id].scl_name);
	}
	ਨਹੀਂਤਾਂ
	    ਸੁਨੇਹਾ_outtrans(highlight_ਸਮੂਹ_name(*p - 1));
	ਜੇ (p[1])
	    ਸੁਨੇਹਾ_putchar(',');
    }
    ਸੁਨੇਹਾ_putchar(' ');
}

    ਸਥਿਰ ਖਾਲੀ
put_ਨਮੂਨਾ(
    ਅੱਖਰ	*s,
    ਪੂਰਨ		c,
    synpat_T	*spp,
    ਪੂਰਨ		attr)
{
    ਲੰਮਾ	n;
    ਪੂਰਨ		ਨਕਾਬ;
    ਪੂਰਨ		ਪਹਿਲਾ;
    ਸਥਿਰ ਅੱਖਰ	*sepchars = "/+=-#@\"|'^&";
    ਪੂਰਨ		i;

    // May have to write "matchgroup=group"
    ਜੇ (ਆਖਰੀ_matchgroup != spp->sp_syn_ਮੇਲ_id)
    {
	ਆਖਰੀ_matchgroup = spp->sp_syn_ਮੇਲ_id;
	ਸੁਨੇਹਾ_puts_attr("matchgroup", attr);
	ਸੁਨੇਹਾ_putchar('=');
	ਜੇ (ਆਖਰੀ_matchgroup == 0)
	    ਸੁਨੇਹਾ_outtrans((char_u *)"NONE");
	ਨਹੀਂਤਾਂ
	    ਸੁਨੇਹਾ_outtrans(highlight_ਸਮੂਹ_name(ਆਖਰੀ_matchgroup - 1));
	ਸੁਨੇਹਾ_putchar(' ');
    }

    // output the name of the pattern and an '=' or ' '
    ਸੁਨੇਹਾ_puts_attr(s, attr);
    ਸੁਨੇਹਾ_putchar(c);

    // output the pattern, in between a char that is not in the pattern
    ਲਈ (i = 0; vim_strchr(spp->sp_ਨਮੂਨਾ, sepchars[i]) != ਨਿਰਾ; )
	ਜੇ (sepchars[++i] == NUL)
	{
	    i = 0;	// no good char found, just use the first one
	    ਰੋਕੋ;
	}
    ਸੁਨੇਹਾ_putchar(sepchars[i]);
    ਸੁਨੇਹਾ_outtrans(spp->sp_ਨਮੂਨਾ);
    ਸੁਨੇਹਾ_putchar(sepchars[i]);

    // output any pattern options
    ਪਹਿਲਾ = TRUE;
    ਲਈ (i = 0; i < SPO_ਗਿਣਤੀ; ++i)
    {
	ਨਕਾਬ = (1 << i);
	ਜੇ (spp->sp_off_flags & (ਨਕਾਬ + (ਨਕਾਬ << SPO_ਗਿਣਤੀ)))
	{
	    ਜੇ (!ਪਹਿਲਾ)
		ਸੁਨੇਹਾ_putchar(',');	// separate with commas
	    ਸੁਨੇਹਾ_puts(spo_name_tab[i]);
	    n = spp->sp_offsets[i];
	    ਜੇ (i != SPO_LC_OFF)
	    {
		ਜੇ (spp->sp_off_flags & ਨਕਾਬ)
		    ਸੁਨੇਹਾ_putchar('s');
		ਨਹੀਂਤਾਂ
		    ਸੁਨੇਹਾ_putchar('e');
		ਜੇ (n > 0)
		    ਸੁਨੇਹਾ_putchar('+');
	    }
	    ਜੇ (n || i == SPO_LC_OFF)
		ਸੁਨੇਹਾ_outnum(n);
	    ਪਹਿਲਾ = FALSE;
	}
    }
    ਸੁਨੇਹਾ_putchar(' ');
}

/*
 * List or clear the keywords for one syntax group.
 * Return TRUE if the header has been printed.
 */
    ਸਥਿਰ ਪੂਰਨ
syn_list_keywords(
    ਪੂਰਨ		id,
    hashtab_T	*ht,
    ਪੂਰਨ		did_ਸਿਰਲੇਖ,		// header has already been printed
    ਪੂਰਨ		attr)
{
    ਪੂਰਨ		outlen;
    hashitem_T	*hi;
    keyentry_T	*kp;
    ਪੂਰਨ		todo;
    ਪੂਰਨ		prev_contained = 0;
    ਛੋਟਾ	*prev_ਅਗਲਾ_list = ਨਿਰਾ;
    ਛੋਟਾ	*prev_cont_ਵਿੱਚ_list = ਨਿਰਾ;
    ਪੂਰਨ		prev_skipnl = 0;
    ਪੂਰਨ		prev_skipwhite = 0;
    ਪੂਰਨ		prev_skipempty = 0;

    /*
     * Unfortunately, this list of keywords is not sorted on alphabet but on
     * hash value...
     */
    todo = (ਪੂਰਨ)ht->ht_ਵਰਤਿਆ;
    ਲਈ (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    --todo;
	    ਲਈ (kp = HI2KE(hi); kp != ਨਿਰਾ && !got_int; kp = kp->ke_ਅਗਲਾ)
	    {
		ਜੇ (kp->k_syn.id == id)
		{
		    ਜੇ (prev_contained != (kp->flags & HL_CONTAINED)
			    || prev_skipnl != (kp->flags & HL_SKIPNL)
			    || prev_skipwhite != (kp->flags & HL_SKIPWHITE)
			    || prev_skipempty != (kp->flags & HL_SKIPEMPTY)
			    || prev_cont_ਵਿੱਚ_list != kp->k_syn.cont_ਵਿੱਚ_list
			    || prev_ਅਗਲਾ_list != kp->ਅਗਲਾ_list)
			outlen = 9999;
		    ਨਹੀਂਤਾਂ
			outlen = (ਪੂਰਨ)STRLEN(kp->keyword);
		    // output "contained" and "nextgroup" on each line
		    ਜੇ (syn_list_ਸਿਰਲੇਖ(did_ਸਿਰਲੇਖ, outlen, id))
		    {
			prev_contained = 0;
			prev_ਅਗਲਾ_list = ਨਿਰਾ;
			prev_cont_ਵਿੱਚ_list = ਨਿਰਾ;
			prev_skipnl = 0;
			prev_skipwhite = 0;
			prev_skipempty = 0;
		    }
		    did_ਸਿਰਲੇਖ = TRUE;
		    ਜੇ (prev_contained != (kp->flags & HL_CONTAINED))
		    {
			ਸੁਨੇਹਾ_puts_attr("contained", attr);
			ਸੁਨੇਹਾ_putchar(' ');
			prev_contained = (kp->flags & HL_CONTAINED);
		    }
		    ਜੇ (kp->k_syn.cont_ਵਿੱਚ_list != prev_cont_ਵਿੱਚ_list)
		    {
			put_id_list((char_u *)"containedin",
						kp->k_syn.cont_ਵਿੱਚ_list, attr);
			ਸੁਨੇਹਾ_putchar(' ');
			prev_cont_ਵਿੱਚ_list = kp->k_syn.cont_ਵਿੱਚ_list;
		    }
		    ਜੇ (kp->ਅਗਲਾ_list != prev_ਅਗਲਾ_list)
		    {
			put_id_list((char_u *)"nextgroup", kp->ਅਗਲਾ_list, attr);
			ਸੁਨੇਹਾ_putchar(' ');
			prev_ਅਗਲਾ_list = kp->ਅਗਲਾ_list;
			ਜੇ (kp->flags & HL_SKIPNL)
			{
			    ਸੁਨੇਹਾ_puts_attr("skipnl", attr);
			    ਸੁਨੇਹਾ_putchar(' ');
			    prev_skipnl = (kp->flags & HL_SKIPNL);
			}
			ਜੇ (kp->flags & HL_SKIPWHITE)
			{
			    ਸੁਨੇਹਾ_puts_attr("skipwhite", attr);
			    ਸੁਨੇਹਾ_putchar(' ');
			    prev_skipwhite = (kp->flags & HL_SKIPWHITE);
			}
			ਜੇ (kp->flags & HL_SKIPEMPTY)
			{
			    ਸੁਨੇਹਾ_puts_attr("skipempty", attr);
			    ਸੁਨੇਹਾ_putchar(' ');
			    prev_skipempty = (kp->flags & HL_SKIPEMPTY);
			}
		    }
		    ਸੁਨੇਹਾ_outtrans(kp->keyword);
		}
	    }
	}
    }

    ਵਾਪਸ did_ਸਿਰਲੇਖ;
}

    ਸਥਿਰ ਖਾਲੀ
syn_clear_keyword(ਪੂਰਨ id, hashtab_T *ht)
{
    hashitem_T	*hi;
    keyentry_T	*kp;
    keyentry_T	*kp_prev;
    keyentry_T	*kp_ਅਗਲਾ;
    ਪੂਰਨ		todo;

    hash_ਤਾਲਾ(ht);
    todo = (ਪੂਰਨ)ht->ht_ਵਰਤਿਆ;
    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(ht, hi, todo)
    {
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    --todo;
	    kp_prev = ਨਿਰਾ;
	    ਲਈ (kp = HI2KE(hi); kp != ਨਿਰਾ; )
	    {
		ਜੇ (kp->k_syn.id == id)
		{
		    kp_ਅਗਲਾ = kp->ke_ਅਗਲਾ;
		    ਜੇ (kp_prev == ਨਿਰਾ)
		    {
			ਜੇ (kp_ਅਗਲਾ == ਨਿਰਾ)
			    hash_remove(ht, hi, "syntax clear keyword");
			ਨਹੀਂਤਾਂ
			    hi->hi_ਕੁੰਜੀ = KE2HIKEY(kp_ਅਗਲਾ);
		    }
		    ਨਹੀਂਤਾਂ
			kp_prev->ke_ਅਗਲਾ = kp_ਅਗਲਾ;
		    vim_free(kp->ਅਗਲਾ_list);
		    vim_free(kp->k_syn.cont_ਵਿੱਚ_list);
		    vim_free(kp);
		    kp = kp_ਅਗਲਾ;
		}
		ਨਹੀਂਤਾਂ
		{
		    kp_prev = kp;
		    kp = kp->ke_ਅਗਲਾ;
		}
	    }
	}
    }
    hash_unlock(ht);
}

/*
 * Clear a whole keyword table.
 */
    ਸਥਿਰ ਖਾਲੀ
clear_keywtab(hashtab_T *ht)
{
    hashitem_T	*hi;
    ਪੂਰਨ		todo;
    keyentry_T	*kp;
    keyentry_T	*kp_ਅਗਲਾ;

    todo = (ਪੂਰਨ)ht->ht_ਵਰਤਿਆ;
    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(ht, hi, todo)
    {
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    --todo;
	    ਲਈ (kp = HI2KE(hi); kp != ਨਿਰਾ; kp = kp_ਅਗਲਾ)
	    {
		kp_ਅਗਲਾ = kp->ke_ਅਗਲਾ;
		vim_free(kp->ਅਗਲਾ_list);
		vim_free(kp->k_syn.cont_ਵਿੱਚ_list);
		vim_free(kp);
	    }
	}
    }
    hash_clear(ht);
    hash_init(ht);
}

/*
 * Add a keyword to the list of keywords.
 */
    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_keyword(
    char_u	*name,	    // name of keyword
    size_t	namelen,    // length of keyword (excluding the NUL)
    ਪੂਰਨ		id,	    // group ID for this keyword
    ਪੂਰਨ		flags,	    // flags for this keyword
    ਛੋਟਾ	*cont_ਵਿੱਚ_list, // containedin for this keyword
    ਛੋਟਾ	*ਅਗਲਾ_list, // nextgroup for this keyword
    ਪੂਰਨ		conceal_char)
{
    keyentry_T	*kp;
    hashtab_T	*ht;
    hashitem_T	*hi;
    char_u	*name_ic;
    size_t	name_iclen;
    long_u	hash;
    char_u	name_folded[MAXKEYWLEN + 1];

    ਜੇ (curwin->w_s->b_syn_ic)
    {
	name_ic = str_foldcase(name, (ਪੂਰਨ)namelen,
						 name_folded, MAXKEYWLEN + 1);
	name_iclen = STRLEN(name_ic);
    }
    ਨਹੀਂਤਾਂ
    {
	name_ic = name;
	name_iclen = namelen;
    }
    kp = alloc(offsetof(keyentry_T, keyword) + name_iclen + 1);
    ਜੇ (kp == ਨਿਰਾ)
	ਵਾਪਸ;
    STRCPY(kp->keyword, name_ic);
    kp->k_syn.id = id;
    kp->k_syn.inc_ਟੈਗ = current_syn_inc_ਟੈਗ;
    kp->flags = flags;
    kp->k_char = conceal_char;
    kp->k_syn.cont_ਵਿੱਚ_list = copy_id_list(cont_ਵਿੱਚ_list);
    ਜੇ (cont_ਵਿੱਚ_list != ਨਿਰਾ)
	curwin->w_s->b_syn_containedin = TRUE;
    kp->ਅਗਲਾ_list = copy_id_list(ਅਗਲਾ_list);

    ਜੇ (curwin->w_s->b_syn_ic)
	ht = &curwin->w_s->b_keywtab_ic;
    ਨਹੀਂਤਾਂ
	ht = &curwin->w_s->b_keywtab;

    hash = hash_hash(kp->keyword);
    hi = hash_lookup(ht, kp->keyword, hash);
    ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
    {
	// new keyword, add to hashtable
	kp->ke_ਅਗਲਾ = ਨਿਰਾ;
	hash_ਜੋੜੋ_ਵਸਤੂ(ht, hi, kp->keyword, hash);
    }
    ਨਹੀਂਤਾਂ
    {
	// keyword already exists, prepend to list
	kp->ke_ਅਗਲਾ = HI2KE(hi);
	hi->hi_ਕੁੰਜੀ = KE2HIKEY(kp);
    }
}

/*
 * Get the start and end of the group name argument.
 * Return a pointer to the first argument.
 * Return NULL if the end of the command was found instead of further args.
 */
    ਸਥਿਰ char_u *
get_ਸਮੂਹ_name(
    char_u	*arg,		// start of the argument
    char_u	**name_ਸਿਰਾ)	// pointer to end of the name
{
    char_u	*rest;

    *name_ਸਿਰਾ = skiptowhite(arg);
    rest = skipwhite(*name_ਸਿਰਾ);

    /*
     * Check if there are enough arguments.  The first argument may be a
     * pattern, where '|' is allowed, so only check for NUL.
     */
    ਜੇ (ਸਿਰੇ_excmd(*arg) || *rest == NUL)
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ rest;
}

/*
 * Check for syntax command option arguments.
 * This can be called at any place in the list of arguments, and just picks
 * out the arguments that are known.  Can be called several times in a row to
 * collect all options in between other arguments.
 * Return a pointer to the next argument (which isn't an option).
 * Return NULL for any error;
 */
    ਸਥਿਰ char_u *
get_syn_options(
    char_u	    *ਸ਼ੁਰੂ,		// next argument to be checked
    syn_opt_arg_T   *opt,		// various things
    ਪੂਰਨ		    *conceal_char ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		    skip)		// TRUE if skipping over command
{
    char_u	*arg = ਸ਼ੁਰੂ;
    char_u	*gname_ਸ਼ੁਰੂ, *gname;
    ਪੂਰਨ		syn_id;
    ਪੂਰਨ		len;
    ਅੱਖਰ	*p;
    ਪੂਰਨ		i;
    ਪੂਰਨ		fidx;
    ਸਥਿਰ ਬਣਤਰ flag
    {
	ਅੱਖਰ	*name;
	ਪੂਰਨ	argtype;
	ਪੂਰਨ	flags;
    } flagtab[] = { {"cCoOnNtTaAiInNeEdD",	0,	HL_CONTAINED},
		    {"oOnNeElLiInNeE",		0,	HL_ONELINE},
		    {"kKeEeEpPeEnNdD",		0,	HL_KEEPEND},
		    {"eExXtTeEnNdD",		0,	HL_EXTEND},
		    {"eExXcClLuUdDeEnNlL",	0,	HL_EXCLUDENL},
		    {"tTrRaAnNsSpPaArReEnNtT",	0,	HL_TRANSP},
		    {"sSkKiIpPnNlL",		0,	HL_SKIPNL},
		    {"sSkKiIpPwWhHiItTeE",	0,	HL_SKIPWHITE},
		    {"sSkKiIpPeEmMpPtTyY",	0,	HL_SKIPEMPTY},
		    {"gGrRoOuUpPhHeErReE",	0,	HL_SYNC_ਇੱਥੇ},
		    {"gGrRoOuUpPtThHeErReE",	0,	HL_SYNC_THERE},
		    {"dDiIsSpPlLaAyY",		0,	HL_DISPLAY},
		    {"fFoOlLdD",		0,	HL_FOLD},
		    {"cCoOnNcCeEaAlL",		0,	HL_CONCEAL},
		    {"cCoOnNcCeEaAlLeEnNdDsS",	0,	HL_CONCEALENDS},
		    {"cCcChHaArR",		11,	0},
		    {"cCoOnNtTaAiInNsS",	1,	0},
		    {"cCoOnNtTaAiInNeEdDiInN",	2,	0},
		    {"nNeExXtTgGrRoOuUpP",	3,	0},
		};
    ਸਥਿਰ ਅੱਖਰ *ਪਹਿਲਾ_letters = "cCoOkKeEtTsSgGdDfFnN";

    ਜੇ (arg == ਨਿਰਾ)		// already detected error
	ਵਾਪਸ ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    ਜੇ (curwin->w_s->b_syn_conceal)
	opt->flags |= HL_CONCEAL;
#ਜੇ_ਅੰਤ

    ਲਈ (;;)
    {
	/*
	 * This is used very often when a large number of keywords is defined.
	 * Need to skip quickly when no option name is found.
	 * Also avoid tolower(), it's slow.
	 */
	ਜੇ (strchr(ਪਹਿਲਾ_letters, *arg) == ਨਿਰਾ)
	    ਰੋਕੋ;

	ਲਈ (fidx = ARRAY_ਲੰਮਾਈ(flagtab); --fidx >= 0; )
	{
	    p = flagtab[fidx].name;
	    ਲਈ (i = 0, len = 0; p[i] != NUL; i += 2, ++len)
		ਜੇ (arg[len] != p[i] && arg[len] != p[i + 1])
		    ਰੋਕੋ;
	    ਜੇ (p[i] == NUL && (VIM_ISWHITE(arg[len])
				    || (flagtab[fidx].argtype > 0
					 ? arg[len] == '='
					 : ਸਿਰੇ_excmd2(ਸ਼ੁਰੂ, arg + len))))
	    {
		ਜੇ (opt->keyword
			&& (flagtab[fidx].flags == HL_DISPLAY
			    || flagtab[fidx].flags == HL_FOLD
			    || flagtab[fidx].flags == HL_EXTEND))
		    // treat "display", "fold" and "extend" as a keyword
		    fidx = -1;
		ਰੋਕੋ;
	    }
	}
	ਜੇ (fidx < 0)	    // no match found
	    ਰੋਕੋ;

	ਜੇ (flagtab[fidx].argtype == 1)
	{
	    ਜੇ (!opt->has_cont_list)
	    {
		emsg(_(e_ਸ਼ਾਮਲ_ਹੈ_ਦਲੀਲ_not_accepted_ਇੱਥੇ));
		ਵਾਪਸ ਨਿਰਾ;
	    }
	    ਜੇ (get_id_list(&arg, 8, &opt->cont_list, skip) == FAIL)
		ਵਾਪਸ ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (flagtab[fidx].argtype == 2)
	{
	    ਜੇ (get_id_list(&arg, 11, &opt->cont_ਵਿੱਚ_list, skip) == FAIL)
		ਵਾਪਸ ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (flagtab[fidx].argtype == 3)
	{
	    ਜੇ (get_id_list(&arg, 9, &opt->ਅਗਲਾ_list, skip) == FAIL)
		ਵਾਪਸ ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (flagtab[fidx].argtype == 11 && arg[5] == '=')
	{
	    // cchar=?
	    ਜੇ (has_mbyte)
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
		*conceal_char = mb_ptr2char(arg + 6);
#ਜੇ_ਅੰਤ
		arg += mb_ptr2len(arg + 6) - 1;
	    }
	    ਨਹੀਂਤਾਂ
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
		*conceal_char = arg[6];
#ਨਹੀਂਤਾਂ
		;
#ਜੇ_ਅੰਤ
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
	    ਜੇ (!vim_isprintc_strict(*conceal_char))
	    {
		emsg(_(e_invalid_cchar_ਮੁੱਲ));
		ਵਾਪਸ ਨਿਰਾ;
	    }
#ਜੇ_ਅੰਤ
	    arg = skipwhite(arg + 7);
	}
	ਨਹੀਂਤਾਂ
	{
	    opt->flags |= flagtab[fidx].flags;
	    arg = skipwhite(arg + len);

	    ਜੇ (flagtab[fidx].flags == HL_SYNC_ਇੱਥੇ
		    || flagtab[fidx].flags == HL_SYNC_THERE)
	    {
		ਜੇ (opt->sync_idx == ਨਿਰਾ)
		{
		    emsg(_(e_groupthere_not_accepted_ਇੱਥੇ));
		    ਵਾਪਸ ਨਿਰਾ;
		}
		gname_ਸ਼ੁਰੂ = arg;
		arg = skiptowhite(arg);
		ਜੇ (gname_ਸ਼ੁਰੂ == arg)
		    ਵਾਪਸ ਨਿਰਾ;
		gname = vim_strnsave(gname_ਸ਼ੁਰੂ, arg - gname_ਸ਼ੁਰੂ);
		ਜੇ (gname == ਨਿਰਾ)
		    ਵਾਪਸ ਨਿਰਾ;
		ਜੇ (STRCMP(gname, "NONE") == 0)
		    *opt->sync_idx = NONE_IDX;
		ਨਹੀਂਤਾਂ
		{
		    syn_id = syn_name2id(gname);
		    ਲਈ (i = curwin->w_s->b_syn_patterns.ga_len; --i >= 0; )
			ਜੇ (SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[i].sp_syn.id == syn_id
			      && SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[i].sp_type
							       == SPTYPE_ਸ਼ੁਰੂ)
			{
			    *opt->sync_idx = i;
			    ਰੋਕੋ;
			}
		    ਜੇ (i < 0)
		    {
			semsg(_(e_didnt_ਲੱਭੋ_region_ਵਸਤੂ_for_str), gname);
			vim_free(gname);
			ਵਾਪਸ ਨਿਰਾ;
		    }
		}

		vim_free(gname);
		arg = skipwhite(arg);
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    ਨਹੀਂਤਾਂ ਜੇ (flagtab[fidx].flags == HL_FOLD
						&& foldmethodIsSyntax(curwin))
		// Need to update folds later.
		foldUpdateAll(curwin);
#ਜੇ_ਅੰਤ
	}
    }

    ਵਾਪਸ arg;
}

/*
 * Adjustments to syntax item when declared in a ":syn include"'d file.
 * Set the contained flag, and if the item is not already contained, add it
 * to the specified top-level group, if any.
 */
    ਸਥਿਰ ਖਾਲੀ
syn_incl_toplevel(ਪੂਰਨ id, ਪੂਰਨ *flagsp)
{
    ਜੇ ((*flagsp & HL_CONTAINED) || curwin->w_s->b_syn_topgrp == 0)
	ਵਾਪਸ;
    *flagsp |= HL_CONTAINED | HL_INCLUDED_TOPLEVEL;
    ਜੇ (curwin->w_s->b_syn_topgrp >= SYNID_CLUSTER)
    {
	// We have to alloc this, because syn_combine_list() will free it.
	ਛੋਟਾ	    *grp_list = ALLOC_MULT(ਛੋਟਾ, 2);
	ਪੂਰਨ	    tlg_id = curwin->w_s->b_syn_topgrp - SYNID_CLUSTER;

	ਜੇ (grp_list != ਨਿਰਾ)
	{
	    grp_list[0] = id;
	    grp_list[1] = 0;
	    syn_combine_list(&SYN_CLSTR(curwin->w_s)[tlg_id].scl_list,
						       &grp_list, CLUSTER_ਜੋੜੋ);
	}
    }
}

/*
 * Handle ":syntax include [@{group-name}] filename" command.
 */
    ਸਥਿਰ ਖਾਲੀ
syn_cmd_include(exarg_T *eap, ਪੂਰਨ syncing ਅਣਵਰਤਿਆ)
{
    char_u	*arg = eap->arg;
    ਪੂਰਨ		sgl_id = 1;
    char_u	*ਸਮੂਹ_name_ਸਿਰਾ;
    char_u	*rest;
    ਅੱਖਰ	*errormsg = ਨਿਰਾ;
    ਪੂਰਨ		prev_toplvl_grp;
    ਪੂਰਨ		prev_syn_inc_ਟੈਗ;
    ਪੂਰਨ		ਸਰੋਤ = FALSE;

    eap->nextcmd = ਲੱਭੋ_nextcmd(arg);
    ਜੇ (eap->skip)
	ਵਾਪਸ;

    ਜੇ (arg[0] == '@')
    {
	++arg;
	rest = get_ਸਮੂਹ_name(arg, &ਸਮੂਹ_name_ਸਿਰਾ);
	ਜੇ (rest == ਨਿਰਾ)
	{
	    emsg(_(e_filename_required));
	    ਵਾਪਸ;
	}
	sgl_id = syn_check_cluster(arg, (ਪੂਰਨ)(ਸਮੂਹ_name_ਸਿਰਾ - arg));
	ਜੇ (sgl_id == 0)
	    ਵਾਪਸ;
	// separate_nextcmd() and expand_filename() depend on this
	eap->arg = rest;
    }

    /*
     * Everything that's left, up to the next command, should be the
     * filename to include.
     */
    eap->argt |= (EX_XFILE | EX_NOSPC);
    separate_nextcmd(eap, FALSE);
    ਜੇ (*eap->arg == '<' || *eap->arg == '$' || mch_is_ਭਰਿਆ_Name(eap->arg))
    {
	// For an absolute path, "$VIM/..." or "<sfile>.." we ":source" the
	// file.  Need to expand the file name first.  In other cases
	// ":runtime!" is used.
	ਸਰੋਤ = TRUE;
	ਜੇ (expand_filename(eap, syn_cmdlinep, &errormsg) == FAIL)
	{
	    ਜੇ (errormsg != ਨਿਰਾ)
		emsg(errormsg);
	    ਵਾਪਸ;
	}
    }

    /*
     * Save and restore the existing top-level grouplist id and ":syn
     * include" tag around the actual inclusion.
     */
    ਜੇ (running_syn_inc_ਟੈਗ >= MAX_SYN_INC_ਟੈਗ)
    {
	emsg(_(e_too_many_syntax_includes));
	ਵਾਪਸ;
    }
    prev_syn_inc_ਟੈਗ = current_syn_inc_ਟੈਗ;
    current_syn_inc_ਟੈਗ = ++running_syn_inc_ਟੈਗ;
    prev_toplvl_grp = curwin->w_s->b_syn_topgrp;
    curwin->w_s->b_syn_topgrp = sgl_id;
    ਜੇ (ਸਰੋਤ ? do_ਸਰੋਤ(eap->arg, FALSE, DOSO_NONE, ਨਿਰਾ) == FAIL
				: ਸਰੋਤ_runtime(eap->arg, DIP_ALL) == FAIL)
	semsg(_(e_cant_open_file_str), eap->arg);
    curwin->w_s->b_syn_topgrp = prev_toplvl_grp;
    current_syn_inc_ਟੈਗ = prev_syn_inc_ਟੈਗ;
}

/*
 * Handle ":syntax keyword {group-name} [{option}] keyword .." command.
 */
    ਸਥਿਰ ਖਾਲੀ
syn_cmd_keyword(exarg_T *eap, ਪੂਰਨ syncing ਅਣਵਰਤਿਆ)
{
    char_u	*arg = eap->arg;
    char_u	*ਸਮੂਹ_name_ਸਿਰਾ;
    ਪੂਰਨ		syn_id;
    char_u	*rest;
    char_u	*keyword_copy = ਨਿਰਾ;
    char_u	*p;
    char_u	*kw;
    syn_opt_arg_T syn_opt_arg;
    ਪੂਰਨ		cnt;
    ਪੂਰਨ		conceal_char = NUL;

    rest = get_ਸਮੂਹ_name(arg, &ਸਮੂਹ_name_ਸਿਰਾ);

    ਜੇ (rest != ਨਿਰਾ)
    {
	ਜੇ (eap->skip)
	    syn_id = -1;
	ਨਹੀਂਤਾਂ
	    syn_id = syn_check_ਸਮੂਹ(arg, (ਪੂਰਨ)(ਸਮੂਹ_name_ਸਿਰਾ - arg));
	ਜੇ (syn_id != 0)
	    // allocate a buffer, for removing backslashes in the keyword
	    keyword_copy = alloc(STRLEN(rest) + 1);
	ਜੇ (keyword_copy != ਨਿਰਾ)
	{
	    syn_opt_arg.flags = 0;
	    syn_opt_arg.keyword = TRUE;
	    syn_opt_arg.sync_idx = ਨਿਰਾ;
	    syn_opt_arg.has_cont_list = FALSE;
	    syn_opt_arg.cont_ਵਿੱਚ_list = ਨਿਰਾ;
	    syn_opt_arg.ਅਗਲਾ_list = ਨਿਰਾ;

	    /*
	     * The options given apply to ALL keywords, so all options must be
	     * found before keywords can be created.
	     * 1: collect the options and copy the keywords to keyword_copy.
	     */
	    cnt = 0;
	    p = keyword_copy;
	    ਲਈ ( ; rest != ਨਿਰਾ && !ਸਿਰੇ_excmd2(eap->arg, rest);
							rest = skipwhite(rest))
	    {
		rest = get_syn_options(rest, &syn_opt_arg, &conceal_char,
								    eap->skip);
		ਜੇ (rest == ਨਿਰਾ || ਸਿਰੇ_excmd2(eap->arg, rest))
		    ਰੋਕੋ;
		// Copy the keyword, removing backslashes, and add a NUL.
		ਜਦੋਂ (*rest != NUL && !VIM_ISWHITE(*rest))
		{
		    ਜੇ (*rest == '\\' && rest[1] != NUL)
			++rest;
		    *p++ = *rest++;
		}
		*p++ = NUL;
		++cnt;
	    }

	    ਜੇ (!eap->skip)
	    {
		size_t	kwlen = 0;

		// Adjust flags for use of ":syn include".
		syn_incl_toplevel(syn_id, &syn_opt_arg.flags);

		/*
		 * 2: Add an entry for each keyword.
		 */
		ਲਈ (kw = keyword_copy; --cnt >= 0; kw += kwlen + 1)
		{
		    ਲਈ (p = vim_strchr(kw, '['); ; )
		    {
			ਜੇ (p == ਨਿਰਾ)
			    kwlen = STRLEN(kw);
			ਨਹੀਂਤਾਂ
			{
			    *p = NUL;
			    kwlen = (size_t)(p - kw);
			}
			ਜੋੜੋ_keyword(kw, kwlen, syn_id, syn_opt_arg.flags,
				syn_opt_arg.cont_ਵਿੱਚ_list,
					 syn_opt_arg.ਅਗਲਾ_list, conceal_char);
			ਜੇ (p == ਨਿਰਾ)
			    ਰੋਕੋ;
			ਜੇ (p[1] == NUL)
			{
			    semsg(_(e_error_missing_rsb_str), kw);
			    ਜਾਓ error;
			}
			ਜੇ (p[1] == ']')
			{
			    ਜੇ (p[2] != NUL)
			    {
				semsg(_(e_trailing_char_after_rsb_str_str),
								    kw, &p[2]);
				ਜਾਓ error;
			    }
			    kw = p + 1;		// skip over the "]"
			    kwlen = 1;
			    ਰੋਕੋ;
			}
			ਜੇ (has_mbyte)
			{
			    ਪੂਰਨ l = (*mb_ptr2len)(p + 1);

			    mch_memmove(p, p + 1, l);
			    p += l;
			}
			ਨਹੀਂਤਾਂ
			{
			    p[0] = p[1];
			    ++p;
			}
		    }
		}
	    }
error:
	    vim_free(keyword_copy);
	    vim_free(syn_opt_arg.cont_ਵਿੱਚ_list);
	    vim_free(syn_opt_arg.ਅਗਲਾ_list);
	}
    }

    ਜੇ (rest != ਨਿਰਾ)
	ਸੈੱਟ_nextcmd(eap, rest);
    ਨਹੀਂਤਾਂ
	semsg(_(e_invalid_ਦਲੀਲ_str), arg);

    redraw_curbuf_later(UPD_SOME_VALID);
    syn_ਢੇਰ_free_all(curwin->w_s);		// Need to recompute all syntax.
}

/*
 * Handle ":syntax match {name} [{options}] {pattern} [{options}]".
 *
 * Also ":syntax sync match {name} [[grouphere | groupthere] {group-name}] .."
 */
    ਸਥਿਰ ਖਾਲੀ
syn_cmd_ਮੇਲ(
    exarg_T	*eap,
    ਪੂਰਨ		syncing)	    // TRUE for ":syntax sync match .. "
{
    char_u	*arg = eap->arg;
    char_u	*ਸਮੂਹ_name_ਸਿਰਾ;
    char_u	*rest;
    synpat_T	ਵਸਤੂ;		// the item found in the line
    ਪੂਰਨ		syn_id;
    ਪੂਰਨ		idx;
    syn_opt_arg_T syn_opt_arg;
    ਪੂਰਨ		sync_idx = 0;
    ਪੂਰਨ		conceal_char = NUL;
    ਪੂਰਨ		orig_called_emsg = called_emsg;

    // Isolate the group name, check for validity
    rest = get_ਸਮੂਹ_name(arg, &ਸਮੂਹ_name_ਸਿਰਾ);

    // Get options before the pattern
    syn_opt_arg.flags = 0;
    syn_opt_arg.keyword = FALSE;
    syn_opt_arg.sync_idx = syncing ? &sync_idx : ਨਿਰਾ;
    syn_opt_arg.has_cont_list = TRUE;
    syn_opt_arg.cont_list = ਨਿਰਾ;
    syn_opt_arg.cont_ਵਿੱਚ_list = ਨਿਰਾ;
    syn_opt_arg.ਅਗਲਾ_list = ਨਿਰਾ;
    rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);

    // get the pattern.
    init_syn_patterns();
    CLEAR_ਖੇਤਰ(ਵਸਤੂ);
    rest = get_syn_ਨਮੂਨਾ(rest, &ਵਸਤੂ);
    ਜੇ (vim_regcomp_had_eol() && !(syn_opt_arg.flags & HL_EXCLUDENL))
	syn_opt_arg.flags |= HL_HAS_EOL;

    // Get options after the pattern
    rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);

    ਜੇ (rest != ਨਿਰਾ)		// all arguments are valid
    {
	/*
	 * Check for trailing command and illegal trailing arguments.
	 */
	ਸੈੱਟ_nextcmd(eap, rest);
	ਜੇ (!ਸਿਰੇ_excmd2(eap->cmd, rest) || eap->skip)
	    rest = ਨਿਰਾ;
	ਨਹੀਂਤਾਂ ਜੇ (ga_grow(&curwin->w_s->b_syn_patterns, 1) == OK
		&& (syn_id = syn_check_ਸਮੂਹ(arg,
					   (ਪੂਰਨ)(ਸਮੂਹ_name_ਸਿਰਾ - arg))) != 0)
	{
	    syn_incl_toplevel(syn_id, &syn_opt_arg.flags);
	    /*
	     * Store the pattern in the syn_items list
	     */
	    idx = curwin->w_s->b_syn_patterns.ga_len;
	    SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx] = ਵਸਤੂ;
	    SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx].sp_syncing = syncing;
	    SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx].sp_type = SPTYPE_ਮੇਲ;
	    SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx].sp_syn.id = syn_id;
	    SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx].sp_syn.inc_ਟੈਗ = current_syn_inc_ਟੈਗ;
	    SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx].sp_flags = syn_opt_arg.flags;
	    SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx].sp_sync_idx = sync_idx;
	    SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx].sp_cont_list = syn_opt_arg.cont_list;
	    SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx].sp_syn.cont_ਵਿੱਚ_list =
						     syn_opt_arg.cont_ਵਿੱਚ_list;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
	    SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx].sp_cchar = conceal_char;
#ਜੇ_ਅੰਤ
	    ਜੇ (syn_opt_arg.cont_ਵਿੱਚ_list != ਨਿਰਾ)
		curwin->w_s->b_syn_containedin = TRUE;
	    SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx].sp_ਅਗਲਾ_list = syn_opt_arg.ਅਗਲਾ_list;
	    ++curwin->w_s->b_syn_patterns.ga_len;

	    // remember that we found a match for syncing on
	    ਜੇ (syn_opt_arg.flags & (HL_SYNC_ਇੱਥੇ|HL_SYNC_THERE))
		curwin->w_s->b_syn_sync_flags |= SF_ਮੇਲ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    ਜੇ (syn_opt_arg.flags & HL_FOLD)
		++curwin->w_s->b_syn_folditems;
#ਜੇ_ਅੰਤ

	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_ਢੇਰ_free_all(curwin->w_s);	// Need to recompute all syntax.
	    ਵਾਪਸ;	// don't free the progs and patterns now
	}
    }

    /*
     * Something failed, free the allocated memory.
     */
    vim_regfree(ਵਸਤੂ.sp_prog);
    vim_free(ਵਸਤੂ.sp_ਨਮੂਨਾ);
    vim_free(syn_opt_arg.cont_list);
    vim_free(syn_opt_arg.cont_ਵਿੱਚ_list);
    vim_free(syn_opt_arg.ਅਗਲਾ_list);

    ਜੇ (rest == ਨਿਰਾ && called_emsg == orig_called_emsg)
	semsg(_(e_invalid_ਦਲੀਲ_str), arg);
}

/*
 * Handle ":syntax region {group-name} [matchgroup={group-name}]
 *		start {start} .. [skip {skip}] end {end} .. [{options}]".
 */
    ਸਥਿਰ ਖਾਲੀ
syn_cmd_region(
    exarg_T	*eap,
    ਪੂਰਨ		syncing)	    // TRUE for ":syntax sync region .."
{
    char_u		*arg = eap->arg;
    char_u		*ਸਮੂਹ_name_ਸਿਰਾ;
    char_u		*rest;			// next arg, NULL on error
    char_u		*ਕੁੰਜੀ_ਸਿਰਾ;
    char_u		*ਕੁੰਜੀ = ਨਿਰਾ;
    char_u		*p;
    ਪੂਰਨ			ਵਸਤੂ;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਵਸਤੂ_ਸ਼ੁਰੂ_2	    0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਵਸਤੂ_SKIP	    1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਵਸਤੂ_ਸਿਰਾ_2	    2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਵਸਤੂ_MATCHGROUP	    3
    ਬਣਤਰ pat_ptr
    {
	synpat_T	*pp_synp;		// pointer to syn_pattern
	ਪੂਰਨ		pp_matchgroup_id;	// matchgroup ID
	ਬਣਤਰ pat_ptr	*pp_ਅਗਲਾ;		// pointer to next pat_ptr
    }			*(pat_ptrs[3]);
					// patterns found in the line
    ਬਣਤਰ pat_ptr	*ppp;
    ਬਣਤਰ pat_ptr	*ppp_ਅਗਲਾ;
    ਪੂਰਨ			pat_ਗਿਣਤੀ = 0;		// nr of syn_patterns found
    ਪੂਰਨ			syn_id;
    ਪੂਰਨ			matchgroup_id = 0;
    ਪੂਰਨ			not_enough = FALSE;	// not enough arguments
    ਪੂਰਨ			illegal = FALSE;	// illegal arguments
    ਪੂਰਨ			success = FALSE;
    ਪੂਰਨ			idx;
    syn_opt_arg_T	syn_opt_arg;
    ਪੂਰਨ			conceal_char = NUL;

    // Isolate the group name, check for validity
    rest = get_ਸਮੂਹ_name(arg, &ਸਮੂਹ_name_ਸਿਰਾ);

    pat_ptrs[0] = ਨਿਰਾ;
    pat_ptrs[1] = ਨਿਰਾ;
    pat_ptrs[2] = ਨਿਰਾ;

    init_syn_patterns();

    syn_opt_arg.flags = 0;
    syn_opt_arg.keyword = FALSE;
    syn_opt_arg.sync_idx = ਨਿਰਾ;
    syn_opt_arg.has_cont_list = TRUE;
    syn_opt_arg.cont_list = ਨਿਰਾ;
    syn_opt_arg.cont_ਵਿੱਚ_list = ਨਿਰਾ;
    syn_opt_arg.ਅਗਲਾ_list = ਨਿਰਾ;

    /*
     * get the options, patterns and matchgroup.
     */
    ਜਦੋਂ (rest != ਨਿਰਾ && !ਸਿਰੇ_excmd2(eap->cmd, rest))
    {
	// Check for option arguments
	rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);
	ਜੇ (rest == ਨਿਰਾ || ਸਿਰੇ_excmd2(eap->cmd, rest))
	    ਰੋਕੋ;

	// must be a pattern or matchgroup then
	ਕੁੰਜੀ_ਸਿਰਾ = rest;
	ਜਦੋਂ (*ਕੁੰਜੀ_ਸਿਰਾ && !VIM_ISWHITE(*ਕੁੰਜੀ_ਸਿਰਾ) && *ਕੁੰਜੀ_ਸਿਰਾ != '=')
	    ++ਕੁੰਜੀ_ਸਿਰਾ;
	vim_free(ਕੁੰਜੀ);
	ਕੁੰਜੀ = vim_strnsave_ਉੱਪਰ(rest, ਕੁੰਜੀ_ਸਿਰਾ - rest);
	ਜੇ (ਕੁੰਜੀ == ਨਿਰਾ)			// out of memory
	{
	    rest = ਨਿਰਾ;
	    ਰੋਕੋ;
	}
	ਜੇ (STRCMP(ਕੁੰਜੀ, "MATCHGROUP") == 0)
	    ਵਸਤੂ = ਵਸਤੂ_MATCHGROUP;
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(ਕੁੰਜੀ, "START") == 0)
	    ਵਸਤੂ = ਵਸਤੂ_ਸ਼ੁਰੂ_2;
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(ਕੁੰਜੀ, "END") == 0)
	    ਵਸਤੂ = ਵਸਤੂ_ਸਿਰਾ_2;
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(ਕੁੰਜੀ, "SKIP") == 0)
	{
	    ਜੇ (pat_ptrs[ਵਸਤੂ_SKIP] != ਨਿਰਾ)	// one skip pattern allowed
	    {
		illegal = TRUE;
		ਰੋਕੋ;
	    }
	    ਵਸਤੂ = ਵਸਤੂ_SKIP;
	}
	ਨਹੀਂਤਾਂ
	    ਰੋਕੋ;
	rest = skipwhite(ਕੁੰਜੀ_ਸਿਰਾ);
	ਜੇ (*rest != '=')
	{
	    rest = ਨਿਰਾ;
	    semsg(_(e_missing_equal_str), arg);
	    ਰੋਕੋ;
	}
	rest = skipwhite(rest + 1);
	ਜੇ (*rest == NUL)
	{
	    not_enough = TRUE;
	    ਰੋਕੋ;
	}

	ਜੇ (ਵਸਤੂ == ਵਸਤੂ_MATCHGROUP)
	{
	    p = skiptowhite(rest);
	    ਜੇ ((p - rest == 4 && STRNCMP(rest, "NONE", 4) == 0) || eap->skip)
		matchgroup_id = 0;
	    ਨਹੀਂਤਾਂ
	    {
		matchgroup_id = syn_check_ਸਮੂਹ(rest, (ਪੂਰਨ)(p - rest));
		ਜੇ (matchgroup_id == 0)
		{
		    illegal = TRUE;
		    ਰੋਕੋ;
		}
	    }
	    rest = skipwhite(p);
	}
	ਨਹੀਂਤਾਂ
	{
	    /*
	     * Allocate room for a syn_pattern, and link it in the list of
	     * syn_patterns for this item, at the start (because the list is
	     * used from end to start).
	     */
	    ppp = ALLOC_ONE(ਬਣਤਰ pat_ptr);
	    ਜੇ (ppp == ਨਿਰਾ)
	    {
		rest = ਨਿਰਾ;
		ਰੋਕੋ;
	    }
	    ppp->pp_ਅਗਲਾ = pat_ptrs[ਵਸਤੂ];
	    pat_ptrs[ਵਸਤੂ] = ppp;
	    ppp->pp_synp = ALLOC_CLEAR_ONE(synpat_T);
	    ਜੇ (ppp->pp_synp == ਨਿਰਾ)
	    {
		rest = ਨਿਰਾ;
		ਰੋਕੋ;
	    }

	    /*
	     * Get the syntax pattern and the following offset(s).
	     */
	    // Enable the appropriate \z specials.
	    ਜੇ (ਵਸਤੂ == ਵਸਤੂ_ਸ਼ੁਰੂ_2)
		reg_do_extmatch = REX_ਸੈੱਟ;
	    ਨਹੀਂਤਾਂ ਜੇ (ਵਸਤੂ == ਵਸਤੂ_SKIP || ਵਸਤੂ == ਵਸਤੂ_ਸਿਰਾ_2)
		reg_do_extmatch = REX_USE;
	    rest = get_syn_ਨਮੂਨਾ(rest, ppp->pp_synp);
	    reg_do_extmatch = 0;
	    ਜੇ (ਵਸਤੂ == ਵਸਤੂ_ਸਿਰਾ_2 && vim_regcomp_had_eol()
				       && !(syn_opt_arg.flags & HL_EXCLUDENL))
		ppp->pp_synp->sp_flags |= HL_HAS_EOL;
	    ppp->pp_matchgroup_id = matchgroup_id;
	    ++pat_ਗਿਣਤੀ;
	}
    }
    vim_free(ਕੁੰਜੀ);
    ਜੇ (illegal || not_enough)
	rest = ਨਿਰਾ;

    /*
     * Must have a "start" and "end" pattern.
     */
    ਜੇ (rest != ਨਿਰਾ && (pat_ptrs[ਵਸਤੂ_ਸ਼ੁਰੂ_2] == ਨਿਰਾ ||
						  pat_ptrs[ਵਸਤੂ_ਸਿਰਾ_2] == ਨਿਰਾ))
    {
	not_enough = TRUE;
	rest = ਨਿਰਾ;
    }

    ਜੇ (rest != ਨਿਰਾ)
    {
	/*
	 * Check for trailing garbage or command.
	 * If OK, add the item.
	 */
	ਸੈੱਟ_nextcmd(eap, rest);
	ਜੇ (!ਸਿਰੇ_excmd(*rest) || eap->skip)
	    rest = ਨਿਰਾ;
	ਨਹੀਂਤਾਂ ਜੇ (ga_grow(&(curwin->w_s->b_syn_patterns), pat_ਗਿਣਤੀ) == OK
		&& (syn_id = syn_check_ਸਮੂਹ(arg,
					   (ਪੂਰਨ)(ਸਮੂਹ_name_ਸਿਰਾ - arg))) != 0)
	{
	    syn_incl_toplevel(syn_id, &syn_opt_arg.flags);
	    /*
	     * Store the start/skip/end in the syn_items list
	     */
	    idx = curwin->w_s->b_syn_patterns.ga_len;
	    ਲਈ (ਵਸਤੂ = ਵਸਤੂ_ਸ਼ੁਰੂ_2; ਵਸਤੂ <= ਵਸਤੂ_ਸਿਰਾ_2; ++ਵਸਤੂ)
	    {
		ਲਈ (ppp = pat_ptrs[ਵਸਤੂ]; ppp != ਨਿਰਾ; ppp = ppp->pp_ਅਗਲਾ)
		{
		    SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx] = *(ppp->pp_synp);
		    SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx].sp_syncing = syncing;
		    SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx].sp_type =
			    (ਵਸਤੂ == ਵਸਤੂ_ਸ਼ੁਰੂ_2) ? SPTYPE_ਸ਼ੁਰੂ :
			    (ਵਸਤੂ == ਵਸਤੂ_SKIP) ? SPTYPE_SKIP : SPTYPE_ਸਿਰਾ;
		    SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx].sp_flags |= syn_opt_arg.flags;
		    SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx].sp_syn.id = syn_id;
		    SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx].sp_syn.inc_ਟੈਗ =
							  current_syn_inc_ਟੈਗ;
		    SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx].sp_syn_ਮੇਲ_id =
							ppp->pp_matchgroup_id;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
		    SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx].sp_cchar = conceal_char;
#ਜੇ_ਅੰਤ
		    ਜੇ (ਵਸਤੂ == ਵਸਤੂ_ਸ਼ੁਰੂ_2)
		    {
			SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx].sp_cont_list =
							syn_opt_arg.cont_list;
			SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx].sp_syn.cont_ਵਿੱਚ_list =
						     syn_opt_arg.cont_ਵਿੱਚ_list;
			ਜੇ (syn_opt_arg.cont_ਵਿੱਚ_list != ਨਿਰਾ)
			    curwin->w_s->b_syn_containedin = TRUE;
			SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx].sp_ਅਗਲਾ_list =
							syn_opt_arg.ਅਗਲਾ_list;
		    }
		    ++curwin->w_s->b_syn_patterns.ga_len;
		    ++idx;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		    ਜੇ (syn_opt_arg.flags & HL_FOLD)
			++curwin->w_s->b_syn_folditems;
#ਜੇ_ਅੰਤ
		}
	    }

	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_ਢੇਰ_free_all(curwin->w_s);	// Need to recompute all syntax.
	    success = TRUE;	    // don't free the progs and patterns now
	}
    }

    /*
     * Free the allocated memory.
     */
    ਲਈ (ਵਸਤੂ = ਵਸਤੂ_ਸ਼ੁਰੂ_2; ਵਸਤੂ <= ਵਸਤੂ_ਸਿਰਾ_2; ++ਵਸਤੂ)
	ਲਈ (ppp = pat_ptrs[ਵਸਤੂ]; ppp != ਨਿਰਾ; ppp = ppp_ਅਗਲਾ)
	{
	    ਜੇ (!success && ppp->pp_synp != ਨਿਰਾ)
	    {
		vim_regfree(ppp->pp_synp->sp_prog);
		vim_free(ppp->pp_synp->sp_ਨਮੂਨਾ);
	    }
	    vim_free(ppp->pp_synp);
	    ppp_ਅਗਲਾ = ppp->pp_ਅਗਲਾ;
	    vim_free(ppp);
	}

    ਜੇ (!success)
    {
	vim_free(syn_opt_arg.cont_list);
	vim_free(syn_opt_arg.cont_ਵਿੱਚ_list);
	vim_free(syn_opt_arg.ਅਗਲਾ_list);
	ਜੇ (not_enough)
	    semsg(_(e_not_enough_arguments_syntax_region_str), arg);
	ਨਹੀਂਤਾਂ ਜੇ (illegal || rest == ਨਿਰਾ)
	    semsg(_(e_invalid_ਦਲੀਲ_str), arg);
    }
}

/*
 * A simple syntax group ID comparison function suitable for use in qsort()
 */
    ਸਥਿਰ ਪੂਰਨ
syn_compare_stub(ਅਚਲ ਖਾਲੀ *v1, ਅਚਲ ਖਾਲੀ *v2)
{
    ਅਚਲ ਛੋਟਾ	*s1 = v1;
    ਅਚਲ ਛੋਟਾ	*s2 = v2;

    ਵਾਪਸ (*s1 > *s2 ? 1 : *s1 < *s2 ? -1 : 0);
}

/*
 * Combines lists of syntax clusters.
 * *clstr1 and *clstr2 must both be allocated memory; they will be consumed.
 */
    ਸਥਿਰ ਖਾਲੀ
syn_combine_list(ਛੋਟਾ **clstr1, ਛੋਟਾ **clstr2, ਪੂਰਨ list_op)
{
    ਪੂਰਨ		count1 = 0;
    ਪੂਰਨ		count2 = 0;
    ਛੋਟਾ	*g1;
    ਛੋਟਾ	*g2;
    ਛੋਟਾ	*clstr = ਨਿਰਾ;
    ਪੂਰਨ		ਗਿਣਤੀ_2;
    ਪੂਰਨ		ਗੇੜਾ;

    /*
     * Handle degenerate cases.
     */
    ਜੇ (*clstr2 == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (*clstr1 == ਨਿਰਾ || list_op == CLUSTER_REPLACE)
    {
	ਜੇ (list_op == CLUSTER_REPLACE)
	    vim_free(*clstr1);
	ਜੇ (list_op == CLUSTER_REPLACE || list_op == CLUSTER_ਜੋੜੋ)
	    *clstr1 = *clstr2;
	ਨਹੀਂਤਾਂ
	    vim_free(*clstr2);
	ਵਾਪਸ;
    }

    ਲਈ (g1 = *clstr1; *g1; g1++)
	++count1;
    ਲਈ (g2 = *clstr2; *g2; g2++)
	++count2;

    /*
     * For speed purposes, sort both lists.
     */
    qsort(*clstr1, (size_t)count1, ਆਕਾਰ(ਛੋਟਾ), syn_compare_stub);
    qsort(*clstr2, (size_t)count2, ਆਕਾਰ(ਛੋਟਾ), syn_compare_stub);

    /*
     * We proceed in two passes; in round 1, we count the elements to place
     * in the new list, and in round 2, we allocate and populate the new
     * list.  For speed, we use a mergesort-like method, adding the smaller
     * of the current elements in each list to the new list.
     */
    ਲਈ (ਗੇੜਾ = 1; ਗੇੜਾ <= 2; ਗੇੜਾ++)
    {
	g1 = *clstr1;
	g2 = *clstr2;
	ਗਿਣਤੀ_2 = 0;

	/*
	 * First, loop through the lists until one of them is empty.
	 */
	ਜਦੋਂ (*g1 && *g2)
	{
	    /*
	     * We always want to add from the first list.
	     */
	    ਜੇ (*g1 < *g2)
	    {
		ਜੇ (ਗੇੜਾ == 2)
		    clstr[ਗਿਣਤੀ_2] = *g1;
		ਗਿਣਤੀ_2++;
		g1++;
		ਜਾਰੀ;
	    }
	    /*
	     * We only want to add from the second list if we're adding the
	     * lists.
	     */
	    ਜੇ (list_op == CLUSTER_ਜੋੜੋ)
	    {
		ਜੇ (ਗੇੜਾ == 2)
		    clstr[ਗਿਣਤੀ_2] = *g2;
		ਗਿਣਤੀ_2++;
	    }
	    ਜੇ (*g1 == *g2)
		g1++;
	    g2++;
	}

	/*
	 * Now add the leftovers from whichever list didn't get finished
	 * first.  As before, we only want to add from the second list if
	 * we're adding the lists.
	 */
	ਲਈ (; *g1; g1++, ਗਿਣਤੀ_2++)
	    ਜੇ (ਗੇੜਾ == 2)
		clstr[ਗਿਣਤੀ_2] = *g1;
	ਜੇ (list_op == CLUSTER_ਜੋੜੋ)
	    ਲਈ (; *g2; g2++, ਗਿਣਤੀ_2++)
		ਜੇ (ਗੇੜਾ == 2)
		    clstr[ਗਿਣਤੀ_2] = *g2;

	ਜੇ (ਗੇੜਾ == 1)
	{
	    /*
	     * If the group ended up empty, we don't need to allocate any
	     * space for it.
	     */
	    ਜੇ (ਗਿਣਤੀ_2 == 0)
	    {
		clstr = ਨਿਰਾ;
		ਰੋਕੋ;
	    }
	    clstr = ALLOC_MULT(ਛੋਟਾ, ਗਿਣਤੀ_2 + 1);
	    ਜੇ (clstr == ਨਿਰਾ)
		ਰੋਕੋ;
	    clstr[ਗਿਣਤੀ_2] = 0;
	}
    }

    /*
     * Finally, put the new list in place.
     */
    vim_free(*clstr1);
    vim_free(*clstr2);
    *clstr1 = clstr;
}

/*
 * Lookup a syntax cluster name and return its ID.
 * If it is not found, 0 is returned.
 */
    ਸਥਿਰ ਪੂਰਨ
syn_scl_name2id(char_u *name)
{
    ਪੂਰਨ		i;
    char_u	*name_u;

    // Avoid using stricmp() too much, it's slow on some systems
    name_u = vim_strsave_ਉੱਪਰ(name);
    ਜੇ (name_u == ਨਿਰਾ)
	ਵਾਪਸ 0;
    ਲਈ (i = curwin->w_s->b_syn_clusters.ga_len; --i >= 0; )
	ਜੇ (SYN_CLSTR(curwin->w_s)[i].scl_name_u != ਨਿਰਾ
		&& STRCMP(name_u, SYN_CLSTR(curwin->w_s)[i].scl_name_u) == 0)
	    ਰੋਕੋ;
    vim_free(name_u);
    ਵਾਪਸ (i < 0 ? 0 : i + SYNID_CLUSTER);
}

/*
 * Like syn_scl_name2id(), but take a pointer + length argument.
 */
    ਸਥਿਰ ਪੂਰਨ
syn_scl_namen2id(char_u *linep, ਪੂਰਨ len)
{
    char_u  *name;
    ਪੂਰਨ	    id = 0;

    name = vim_strnsave(linep, len);
    ਜੇ (name == ਨਿਰਾ)
	ਵਾਪਸ 0;

    id = syn_scl_name2id(name);
    vim_free(name);
    ਵਾਪਸ id;
}

/*
 * Find syntax cluster name in the table and return its ID.
 * The argument is a pointer to the name and the length of the name.
 * If it doesn't exist yet, a new entry is created.
 * Return 0 for failure.
 */
    ਸਥਿਰ ਪੂਰਨ
syn_check_cluster(char_u *pp, ਪੂਰਨ len)
{
    ਪੂਰਨ		id;
    char_u	*name;

    name = vim_strnsave(pp, len);
    ਜੇ (name == ਨਿਰਾ)
	ਵਾਪਸ 0;

    id = syn_scl_name2id(name);
    ਜੇ (id == 0)			// doesn't exist yet
	id = syn_ਜੋੜੋ_cluster(name);
    ਨਹੀਂਤਾਂ
	vim_free(name);
    ਵਾਪਸ id;
}

/*
 * Add new syntax cluster and return its ID.
 * "name" must be an allocated string, it will be consumed.
 * Return 0 for failure.
 */
    ਸਥਿਰ ਪੂਰਨ
syn_ਜੋੜੋ_cluster(char_u *name)
{
    ਪੂਰਨ		len;

    /*
     * First call for this growarray: init growing array.
     */
    ਜੇ (curwin->w_s->b_syn_clusters.ga_ਅੰਕੜੇ == ਨਿਰਾ)
    {
	curwin->w_s->b_syn_clusters.ga_itemsize = ਆਕਾਰ(syn_cluster_T);
	curwin->w_s->b_syn_clusters.ga_growsize = 10;
    }

    len = curwin->w_s->b_syn_clusters.ga_len;
    ਜੇ (len >= MAX_CLUSTER_ID)
    {
	emsg(_(e_too_many_syntax_clusters));
	vim_free(name);
	ਵਾਪਸ 0;
    }

    /*
     * Make room for at least one other cluster entry.
     */
    ਜੇ (ga_grow(&curwin->w_s->b_syn_clusters, 1) == FAIL)
    {
	vim_free(name);
	ਵਾਪਸ 0;
    }

    CLEAR_ਸੂਚਕ(&(SYN_CLSTR(curwin->w_s)[len]));
    SYN_CLSTR(curwin->w_s)[len].scl_name = name;
    SYN_CLSTR(curwin->w_s)[len].scl_name_u = vim_strsave_ਉੱਪਰ(name);
    SYN_CLSTR(curwin->w_s)[len].scl_list = ਨਿਰਾ;
    ++curwin->w_s->b_syn_clusters.ga_len;

    ਜੇ (STRICMP(name, "Spell") == 0)
	curwin->w_s->b_spell_cluster_id = len + SYNID_CLUSTER;
    ਜੇ (STRICMP(name, "NoSpell") == 0)
	curwin->w_s->b_nospell_cluster_id = len + SYNID_CLUSTER;

    ਵਾਪਸ len + SYNID_CLUSTER;
}

/*
 * Handle ":syntax cluster {cluster-name} [contains={groupname},..]
 *		[add={groupname},..] [remove={groupname},..]".
 */
    ਸਥਿਰ ਖਾਲੀ
syn_cmd_cluster(exarg_T *eap, ਪੂਰਨ syncing ਅਣਵਰਤਿਆ)
{
    char_u	*arg = eap->arg;
    char_u	*ਸਮੂਹ_name_ਸਿਰਾ;
    char_u	*rest;
    ਪੂਰਨ		scl_id;
    ਛੋਟਾ	*clstr_list;
    ਪੂਰਨ		got_clstr = FALSE;
    ਪੂਰਨ		opt_len;
    ਪੂਰਨ		list_op;

    eap->nextcmd = ਲੱਭੋ_nextcmd(arg);
    ਜੇ (eap->skip)
	ਵਾਪਸ;

    rest = get_ਸਮੂਹ_name(arg, &ਸਮੂਹ_name_ਸਿਰਾ);

    ਜੇ (rest != ਨਿਰਾ)
    {
	scl_id = syn_check_cluster(arg, (ਪੂਰਨ)(ਸਮੂਹ_name_ਸਿਰਾ - arg));
	ਜੇ (scl_id == 0)
	    ਵਾਪਸ;
	scl_id -= SYNID_CLUSTER;

	ਲਈ (;;)
	{
	    ਜੇ (STRNICMP(rest, "add", 3) == 0
		    && (VIM_ISWHITE(rest[3]) || rest[3] == '='))
	    {
		opt_len = 3;
		list_op = CLUSTER_ਜੋੜੋ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(rest, "remove", 6) == 0
		    && (VIM_ISWHITE(rest[6]) || rest[6] == '='))
	    {
		opt_len = 6;
		list_op = CLUSTER_ਘਟਾਓ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(rest, "contains", 8) == 0
			&& (VIM_ISWHITE(rest[8]) || rest[8] == '='))
	    {
		opt_len = 8;
		list_op = CLUSTER_REPLACE;
	    }
	    ਨਹੀਂਤਾਂ
		ਰੋਕੋ;

	    clstr_list = ਨਿਰਾ;
	    ਜੇ (get_id_list(&rest, opt_len, &clstr_list, eap->skip) == FAIL)
	    {
		semsg(_(e_invalid_ਦਲੀਲ_str), rest);
		ਰੋਕੋ;
	    }
	    ਜੇ (scl_id >= 0)
		syn_combine_list(&SYN_CLSTR(curwin->w_s)[scl_id].scl_list,
			     &clstr_list, list_op);
	    ਨਹੀਂਤਾਂ
		vim_free(clstr_list);
	    got_clstr = TRUE;
	}

	ਜੇ (got_clstr)
	{
	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_ਢੇਰ_free_all(curwin->w_s);	// Need to recompute all.
	}
    }

    ਜੇ (!got_clstr)
	emsg(_(e_no_cluster_specified));
    ਜੇ (rest == ਨਿਰਾ || !ਸਿਰੇ_excmd2(eap->cmd, rest))
	semsg(_(e_invalid_ਦਲੀਲ_str), arg);
}

/*
 * On first call for current buffer: Init growing array.
 */
    ਸਥਿਰ ਖਾਲੀ
init_syn_patterns(ਖਾਲੀ)
{
    curwin->w_s->b_syn_patterns.ga_itemsize = ਆਕਾਰ(synpat_T);
    curwin->w_s->b_syn_patterns.ga_growsize = 10;
}

/*
 * Get one pattern for a ":syntax match" or ":syntax region" command.
 * Stores the pattern and program in a synpat_T.
 * Returns a pointer to the next argument, or NULL in case of an error.
 */
    ਸਥਿਰ char_u *
get_syn_ਨਮੂਨਾ(char_u *arg, synpat_T *ci)
{
    char_u	*ਸਿਰਾ;
    ਪੂਰਨ		*p;
    ਪੂਰਨ		idx;
    char_u	*cpo_save;

    // need at least three chars
    ਜੇ (arg == ਨਿਰਾ || arg[0] == NUL || arg[1] == NUL || arg[2] == NUL)
	ਵਾਪਸ ਨਿਰਾ;

    ਸਿਰਾ = skip_regexp(arg + 1, *arg, TRUE);
    ਜੇ (*ਸਿਰਾ != *arg)			    // end delimiter not found
    {
	semsg(_(e_ਨਮੂਨਾ_delimiter_not_ਮਿਲਿਆ_str), arg);
	ਵਾਪਸ ਨਿਰਾ;
    }
    // store the pattern and compiled regexp program
    ਜੇ ((ci->sp_ਨਮੂਨਾ = vim_strnsave(arg + 1, ਸਿਰਾ - arg - 1)) == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // Make 'cpoptions' empty, to avoid the 'l' flag
    cpo_save = p_cpo;
    p_cpo = ਖਾਲੀ_ਥਾਵਾਂ_option;
    ci->sp_prog = vim_regcomp(ci->sp_ਨਮੂਨਾ, RE_MAGIC);
    p_cpo = cpo_save;

    ਜੇ (ci->sp_prog == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ci->sp_ic = curwin->w_s->b_syn_ic;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    syn_clear_ਸਮਾਂ(&ci->sp_ਸਮਾਂ);
#ਜੇ_ਅੰਤ

    /*
     * Check for a match, highlight or region offset.
     */
    ++ਸਿਰਾ;
    ਕਰੋ
    {
	ਲਈ (idx = SPO_ਗਿਣਤੀ; --idx >= 0; )
	    ਜੇ (STRNCMP(ਸਿਰਾ, spo_name_tab[idx], 3) == 0)
		ਰੋਕੋ;
	ਜੇ (idx >= 0)
	{
	    p = &(ci->sp_offsets[idx]);
	    ਜੇ (idx != SPO_LC_OFF)
		ਚੋਣ (ਸਿਰਾ[3])
		{
		    ਹਾਲਤ 's':   ਰੋਕੋ;
		    ਹਾਲਤ 'b':   ਰੋਕੋ;
		    ਹਾਲਤ 'e':   idx += SPO_ਗਿਣਤੀ; ਰੋਕੋ;
		    ਮੂਲ:    idx = -1; ਰੋਕੋ;
		}
	    ਜੇ (idx >= 0)
	    {
		ci->sp_off_flags |= (1 << idx);
		ਜੇ (idx == SPO_LC_OFF)	    // lc=99
		{
		    ਸਿਰਾ += 3;
		    *p = getdigits(&ਸਿਰਾ);

		    // "lc=" offset automatically sets "ms=" offset
		    ਜੇ (!(ci->sp_off_flags & (1 << SPO_MS_OFF)))
		    {
			ci->sp_off_flags |= (1 << SPO_MS_OFF);
			ci->sp_offsets[SPO_MS_OFF] = *p;
		    }
		}
		ਨਹੀਂਤਾਂ			    // yy=x+99
		{
		    ਸਿਰਾ += 4;
		    ਜੇ (*ਸਿਰਾ == '+')
		    {
			++ਸਿਰਾ;
			*p = getdigits(&ਸਿਰਾ);		// positive offset
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (*ਸਿਰਾ == '-')
		    {
			++ਸਿਰਾ;
			*p = -getdigits(&ਸਿਰਾ);		// negative offset
		    }
		}
		ਜੇ (*ਸਿਰਾ != ',')
		    ਰੋਕੋ;
		++ਸਿਰਾ;
	    }
	}
    } ਜਦੋਂ (idx >= 0);

    ਜੇ (!ਸਿਰੇ_excmd2(arg, ਸਿਰਾ) && !VIM_ISWHITE(*ਸਿਰਾ))
    {
	semsg(_(e_garbage_after_ਨਮੂਨਾ_str), arg);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ skipwhite(ਸਿਰਾ);
}

/*
 * Handle ":syntax sync .." command.
 */
    ਸਥਿਰ ਖਾਲੀ
syn_cmd_sync(exarg_T *eap, ਪੂਰਨ syncing ਅਣਵਰਤਿਆ)
{
    char_u	*arg_ਸ਼ੁਰੂ = eap->arg;
    char_u	*arg_ਸਿਰਾ;
    char_u	*ਕੁੰਜੀ = ਨਿਰਾ;
    char_u	*ਅਗਲਾ_arg;
    ਪੂਰਨ		illegal = FALSE;
    ਪੂਰਨ		ਮੁਕੰਮਲ = FALSE;
    ਲੰਮਾ	n;
    char_u	*cpo_save;

    ਜੇ (ਸਿਰੇ_excmd2(eap->cmd, arg_ਸ਼ੁਰੂ))
    {
	syn_cmd_list(eap, TRUE);
	ਵਾਪਸ;
    }

    ਜਦੋਂ (!ਸਿਰੇ_excmd2(eap->cmd, arg_ਸ਼ੁਰੂ))
    {
	arg_ਸਿਰਾ = skiptowhite(arg_ਸ਼ੁਰੂ);
	ਅਗਲਾ_arg = skipwhite(arg_ਸਿਰਾ);
	vim_free(ਕੁੰਜੀ);
	ਕੁੰਜੀ = vim_strnsave_ਉੱਪਰ(arg_ਸ਼ੁਰੂ, arg_ਸਿਰਾ - arg_ਸ਼ੁਰੂ);
	ਜੇ (ਕੁੰਜੀ == ਨਿਰਾ)
	    ਰੋਕੋ;
	ਜੇ (STRCMP(ਕੁੰਜੀ, "CCOMMENT") == 0)
	{
	    ਜੇ (!eap->skip)
		curwin->w_s->b_syn_sync_flags |= SF_CCOMMENT;
	    ਜੇ (!ਸਿਰੇ_excmd2(eap->cmd, ਅਗਲਾ_arg))
	    {
		arg_ਸਿਰਾ = skiptowhite(ਅਗਲਾ_arg);
		ਜੇ (!eap->skip)
		    curwin->w_s->b_syn_sync_id = syn_check_ਸਮੂਹ(ਅਗਲਾ_arg,
						   (ਪੂਰਨ)(arg_ਸਿਰਾ - ਅਗਲਾ_arg));
		ਅਗਲਾ_arg = skipwhite(arg_ਸਿਰਾ);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (!eap->skip)
		curwin->w_s->b_syn_sync_id = syn_name2id((char_u *)"Comment");
	}
	ਨਹੀਂਤਾਂ ਜੇ (  STRNCMP(ਕੁੰਜੀ, "LINES", 5) == 0
		|| STRNCMP(ਕੁੰਜੀ, "MINLINES", 8) == 0
		|| STRNCMP(ਕੁੰਜੀ, "MAXLINES", 8) == 0
		|| STRNCMP(ਕੁੰਜੀ, "LINEBREAKS", 10) == 0)
	{
	    ਜੇ (ਕੁੰਜੀ[4] == 'S')
		arg_ਸਿਰਾ = ਕੁੰਜੀ + 6;
	    ਨਹੀਂਤਾਂ ਜੇ (ਕੁੰਜੀ[0] == 'L')
		arg_ਸਿਰਾ = ਕੁੰਜੀ + 11;
	    ਨਹੀਂਤਾਂ
		arg_ਸਿਰਾ = ਕੁੰਜੀ + 9;
	    ਜੇ (arg_ਸਿਰਾ[-1] != '=' || !VIM_ISDIGIT(*arg_ਸਿਰਾ))
	    {
		illegal = TRUE;
		ਰੋਕੋ;
	    }
	    n = getdigits(&arg_ਸਿਰਾ);
	    ਜੇ (!eap->skip)
	    {
		ਜੇ (ਕੁੰਜੀ[4] == 'B')
		    curwin->w_s->b_syn_sync_linebreaks = n;
		ਨਹੀਂਤਾਂ ਜੇ (ਕੁੰਜੀ[1] == 'A')
		    curwin->w_s->b_syn_sync_maxlines = n;
		ਨਹੀਂਤਾਂ
		    curwin->w_s->b_syn_sync_minlines = n;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(ਕੁੰਜੀ, "FROMSTART") == 0)
	{
	    ਜੇ (!eap->skip)
	    {
		curwin->w_s->b_syn_sync_minlines = MAXLNUM;
		curwin->w_s->b_syn_sync_maxlines = 0;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(ਕੁੰਜੀ, "LINECONT") == 0)
	{
	    ਜੇ (*ਅਗਲਾ_arg == NUL)	   // missing pattern
	    {
		illegal = TRUE;
		ਰੋਕੋ;
	    }
	    ਜੇ (curwin->w_s->b_syn_linecont_pat != ਨਿਰਾ)
	    {
		emsg(_(e_syntax_sync_ਸਤਰ_continuations_ਨਮੂਨਾ_specified_twice));
		ਮੁਕੰਮਲ = TRUE;
		ਰੋਕੋ;
	    }
	    arg_ਸਿਰਾ = skip_regexp(ਅਗਲਾ_arg + 1, *ਅਗਲਾ_arg, TRUE);
	    ਜੇ (*arg_ਸਿਰਾ != *ਅਗਲਾ_arg)	    // end delimiter not found
	    {
		illegal = TRUE;
		ਰੋਕੋ;
	    }

	    ਜੇ (!eap->skip)
	    {
		// store the pattern and compiled regexp program
		ਜੇ ((curwin->w_s->b_syn_linecont_pat =
			    vim_strnsave(ਅਗਲਾ_arg + 1,
				      arg_ਸਿਰਾ - ਅਗਲਾ_arg - 1)) == ਨਿਰਾ)
		{
		    ਮੁਕੰਮਲ = TRUE;
		    ਰੋਕੋ;
		}
		curwin->w_s->b_syn_linecont_ic = curwin->w_s->b_syn_ic;

		// Make 'cpoptions' empty, to avoid the 'l' flag
		cpo_save = p_cpo;
		p_cpo = ਖਾਲੀ_ਥਾਵਾਂ_option;
		curwin->w_s->b_syn_linecont_prog =
		       vim_regcomp(curwin->w_s->b_syn_linecont_pat, RE_MAGIC);
		p_cpo = cpo_save;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
		syn_clear_ਸਮਾਂ(&curwin->w_s->b_syn_linecont_ਸਮਾਂ);
#ਜੇ_ਅੰਤ

		ਜੇ (curwin->w_s->b_syn_linecont_prog == ਨਿਰਾ)
		{
		    VIM_CLEAR(curwin->w_s->b_syn_linecont_pat);
		    ਮੁਕੰਮਲ = TRUE;
		    ਰੋਕੋ;
		}
	    }
	    ਅਗਲਾ_arg = skipwhite(arg_ਸਿਰਾ + 1);
	}
	ਨਹੀਂਤਾਂ
	{
	    eap->arg = ਅਗਲਾ_arg;
	    ਜੇ (STRCMP(ਕੁੰਜੀ, "MATCH") == 0)
		syn_cmd_ਮੇਲ(eap, TRUE);
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(ਕੁੰਜੀ, "REGION") == 0)
		syn_cmd_region(eap, TRUE);
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(ਕੁੰਜੀ, "CLEAR") == 0)
		syn_cmd_clear(eap, TRUE);
	    ਨਹੀਂਤਾਂ
		illegal = TRUE;
	    ਮੁਕੰਮਲ = TRUE;
	    ਰੋਕੋ;
	}
	arg_ਸ਼ੁਰੂ = ਅਗਲਾ_arg;
    }
    vim_free(ਕੁੰਜੀ);
    ਜੇ (illegal)
	semsg(_(e_illegal_arguments_str), arg_ਸ਼ੁਰੂ);
    ਨਹੀਂਤਾਂ ਜੇ (!ਮੁਕੰਮਲ)
    {
	ਸੈੱਟ_nextcmd(eap, arg_ਸ਼ੁਰੂ);
	redraw_curbuf_later(UPD_SOME_VALID);
	syn_ਢੇਰ_free_all(curwin->w_s);	// Need to recompute all syntax.
    }
}

/*
 * Convert a line of highlight group names into a list of group ID numbers.
 * "arg" should point to the "contains" or "nextgroup" keyword.
 * "arg" is advanced to after the last group name.
 * Careful: the argument is modified (NULs added).
 * returns FAIL for some error, OK for success.
 */
    ਸਥਿਰ ਪੂਰਨ
get_id_list(
    char_u	**arg,
    ਪੂਰਨ		keylen,		// length of keyword
    ਛੋਟਾ	**list,		// where to store the resulting list, if not
				// NULL, the list is silently skipped!
    ਪੂਰਨ		skip)
{
    char_u	*p = ਨਿਰਾ;
    char_u	*ਸਿਰਾ;
    ਪੂਰਨ		ਗੇੜਾ;
    ਪੂਰਨ		ਗਿਣਤੀ_2;
    ਪੂਰਨ		ਕੁੱਲ_ਗਿਣਤੀ = 0;
    ਛੋਟਾ	*retval = ਨਿਰਾ;
    char_u	*name;
    regmatch_T	regmatch;
    ਪੂਰਨ		id;
    ਪੂਰਨ		i;
    ਪੂਰਨ		failed = FALSE;

    /*
     * We parse the list twice:
     * round == 1: count the number of items, allocate the array.
     * round == 2: fill the array with the items.
     * In round 1 new groups may be added, causing the number of items to
     * grow when a regexp is used.  In that case round 1 is done once again.
     */
    ਲਈ (ਗੇੜਾ = 1; ਗੇੜਾ <= 2; ++ਗੇੜਾ)
    {
	/*
	 * skip "contains"
	 */
	p = skipwhite(*arg + keylen);
	ਜੇ (*p != '=')
	{
	    semsg(_(e_missing_equal_sign_str), *arg);
	    ਰੋਕੋ;
	}
	p = skipwhite(p + 1);
	ਜੇ (ਸਿਰੇ_excmd2(*arg, p))
	{
	    semsg(_(e_ਖਾਲੀ_ਥਾਵਾਂ_ਦਲੀਲ_str), *arg);
	    ਰੋਕੋ;
	}

	/*
	 * parse the arguments after "contains"
	 */
	ਗਿਣਤੀ_2 = 0;
	ਜਦੋਂ (!ਸਿਰੇ_excmd2(*arg, p))
	{
	    ਲਈ (ਸਿਰਾ = p; *ਸਿਰਾ && !VIM_ISWHITE(*ਸਿਰਾ) && *ਸਿਰਾ != ','; ++ਸਿਰਾ)
		;
	    name = alloc(ਸਿਰਾ - p + 3);	    // leave room for "^$"
	    ਜੇ (name == ਨਿਰਾ)
	    {
		failed = TRUE;
		ਰੋਕੋ;
	    }
	    vim_strncpy(name + 1, p, ਸਿਰਾ - p);
	    ਜੇ (       STRCMP(name + 1, "ALLBUT") == 0
		    || STRCMP(name + 1, "ALL") == 0
		    || STRCMP(name + 1, "TOP") == 0
		    || STRCMP(name + 1, "CONTAINED") == 0)
	    {
		ਜੇ (TOUPPER_ASC(**arg) != 'C')
		{
		    semsg(_(e_str_not_allowed_ਇੱਥੇ), name + 1);
		    failed = TRUE;
		    vim_free(name);
		    ਰੋਕੋ;
		}
		ਜੇ (ਗਿਣਤੀ_2 != 0)
		{
		    semsg(_(e_str_must_be_ਪਹਿਲਾ_ਵਿੱਚ_ਸ਼ਾਮਲ_ਹੈ_list), name + 1);
		    failed = TRUE;
		    vim_free(name);
		    ਰੋਕੋ;
		}
		ਜੇ (name[1] == 'A')
		    id = SYNID_ALLBUT;
		ਨਹੀਂਤਾਂ ਜੇ (name[1] == 'T')
		    id = SYNID_ਸਿਖਰ;
		ਨਹੀਂਤਾਂ
		    id = SYNID_CONTAINED;
		id += current_syn_inc_ਟੈਗ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (name[1] == '@')
	    {
		ਜੇ (skip)
		    id = -1;
		ਨਹੀਂਤਾਂ
		    id = syn_check_cluster(name + 2, (ਪੂਰਨ)(ਸਿਰਾ - p - 1));
	    }
	    ਨਹੀਂਤਾਂ
	    {
		/*
		 * Handle full group name.
		 */
		ਜੇ (vim_strpbrk(name + 1, (char_u *)"\\.*^$~[") == ਨਿਰਾ)
		    id = syn_check_ਸਮੂਹ(name + 1, (ਪੂਰਨ)(ਸਿਰਾ - p));
		ਨਹੀਂਤਾਂ
		{
		    /*
		     * Handle match of regexp with group names.
		     */
		    *name = '^';
		    STRCAT(name, "$");
		    regmatch.regprog = vim_regcomp(name, RE_MAGIC);
		    ਜੇ (regmatch.regprog == ਨਿਰਾ)
		    {
			failed = TRUE;
			vim_free(name);
			ਰੋਕੋ;
		    }

		    regmatch.rm_ic = TRUE;
		    id = 0;
		    ਲਈ (i = highlight_ਅੰਕ_groups(); --i >= 0; )
		    {
			ਜੇ (vim_regexec(&regmatch, highlight_ਸਮੂਹ_name(i),
								  (colnr_T)0))
			{
			    ਜੇ (ਗੇੜਾ == 2)
			    {
				// Got more items than expected; can happen
				// when adding items that match:
				// "contains=a.*b,axb".
				// Go back to first round
				ਜੇ (ਗਿਣਤੀ_2 >= ਕੁੱਲ_ਗਿਣਤੀ)
				{
				    vim_free(retval);
				    ਗੇੜਾ = 1;
				}
				ਨਹੀਂਤਾਂ
				    retval[ਗਿਣਤੀ_2] = i + 1;
			    }
			    ++ਗਿਣਤੀ_2;
			    id = -1;	    // remember that we found one
			}
		    }
		    vim_regfree(regmatch.regprog);
		}
	    }
	    vim_free(name);
	    ਜੇ (id == 0)
	    {
		semsg(_(e_unknown_ਸਮੂਹ_name_str), p);
		failed = TRUE;
		ਰੋਕੋ;
	    }
	    ਜੇ (id > 0)
	    {
		ਜੇ (ਗੇੜਾ == 2)
		{
		    // Got more items than expected, go back to first round
		    ਜੇ (ਗਿਣਤੀ_2 >= ਕੁੱਲ_ਗਿਣਤੀ)
		    {
			vim_free(retval);
			ਗੇੜਾ = 1;
		    }
		    ਨਹੀਂਤਾਂ
			retval[ਗਿਣਤੀ_2] = id;
		}
		++ਗਿਣਤੀ_2;
	    }
	    p = skipwhite(ਸਿਰਾ);
	    ਜੇ (*p != ',')
		ਰੋਕੋ;
	    p = skipwhite(p + 1);	// skip comma in between arguments
	}
	ਜੇ (failed)
	    ਰੋਕੋ;
	ਜੇ (ਗੇੜਾ == 1)
	{
	    retval = ALLOC_MULT(ਛੋਟਾ, ਗਿਣਤੀ_2 + 1);
	    ਜੇ (retval == ਨਿਰਾ)
		ਰੋਕੋ;
	    retval[ਗਿਣਤੀ_2] = 0;	    // zero means end of the list
	    ਕੁੱਲ_ਗਿਣਤੀ = ਗਿਣਤੀ_2;
	}
    }

    *arg = p;
    ਜੇ (failed || retval == ਨਿਰਾ)
    {
	vim_free(retval);
	ਵਾਪਸ FAIL;
    }

    ਜੇ (*list == ਨਿਰਾ)
	*list = retval;
    ਨਹੀਂਤਾਂ
	vim_free(retval);	// list already found, don't overwrite it

    ਵਾਪਸ OK;
}

/*
 * Make a copy of an ID list.
 */
    ਸਥਿਰ ਛੋਟਾ *
copy_id_list(ਛੋਟਾ *list)
{
    ਪੂਰਨ	    len;
    ਪੂਰਨ	    ਗਿਣਤੀ_2;
    ਛੋਟਾ   *retval;

    ਜੇ (list == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਲਈ (ਗਿਣਤੀ_2 = 0; list[ਗਿਣਤੀ_2]; ++ਗਿਣਤੀ_2)
	;
    len = (ਗਿਣਤੀ_2 + 1) * ਆਕਾਰ(ਛੋਟਾ);
    retval = alloc(len);
    ਜੇ (retval != ਨਿਰਾ)
	mch_memmove(retval, list, (size_t)len);

    ਵਾਪਸ retval;
}

/*
 * Cache for in_id_list() {{{
 *
 * A "contains"/cluster list is otherwise scanned with recursive cluster
 * expansion on every check.  Each distinct list is resolved once into a
 * cluster-expanded, sorted set of group IDs so membership becomes a binary
 * search.  The cache lives in the synblock and is cleared by
 * syn_stack_free_block() when syntax definitions change (which also covers
 * list pointer reuse).
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IDL_CACHE_ਮਾਪ 16

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਛੋਟਾ	*idl_ਕੁੰਜੀ;	// the list this entry resolves (NULL when empty)
    ਛੋਟਾ	*idl_ids;	// sorted, cluster-expanded group IDs (allocated)
    ਪੂਰਨ		idl_ਗਿਣਤੀ;	// number of IDs in idl_ids
    ਛੋਟਾ	idl_marker;	// leading ALLBUT/TOP/CONTAINED item, or 0
    bool	idl_usable;	// false: not cacheable, use the slow path
} idl_ਇੰਦਰਾਜ_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    idl_ਇੰਦਰਾਜ_T	ent[IDL_CACHE_ਮਾਪ];
    ਪੂਰਨ		ਗੇੜਾ;		// round-robin replacement index
    ਪੂਰਨ		ਆਖਰੀ;		// index of the last returned entry (fast path)
} idl_cache_T;

/*
 * Collect the explicit group IDs of "list" into "ga", expanding clusters.
 * "seen" records visited clusters to break cycles and avoid duplicate work.
 * Return false when the list cannot be represented as a plain set (a nested
 * ALLBUT/TOP/CONTAINED marker) or on out of memory.
 */
    ਸਥਿਰ bool
idl_collect(ਛੋਟਾ *list, garray_T *ga, garray_T *ਵੇਖਿਆ)
{
    ਜੇ (list == ਨਿਰਾ || list == ID_LIST_ALL)
	ਵਾਪਸ false;
    ਲਈ (ਛੋਟਾ ਵਸਤੂ = *list; ਵਸਤੂ != 0; ਵਸਤੂ = *++list)
    {
	ਜੇ (ਵਸਤੂ >= SYNID_ALLBUT && ਵਸਤੂ < SYNID_CLUSTER)
	    ਵਾਪਸ false;	// nested ALLBUT/TOP/CONTAINED: not cacheable
	ਜੇ (ਵਸਤੂ >= SYNID_CLUSTER)
	{
	    ਛੋਟਾ	*scl_list;
	    bool	ਵੇਖਿਆ_it = false;

	    ਲਈ (ਪੂਰਨ i = 0; i < ਵੇਖਿਆ->ga_len; ++i)
		ਜੇ (((ਛੋਟਾ *)ਵੇਖਿਆ->ga_ਅੰਕੜੇ)[i] == ਵਸਤੂ)
		{
		    ਵੇਖਿਆ_it = true;
		    ਰੋਕੋ;
		}
	    ਜੇ (ਵੇਖਿਆ_it)
		ਜਾਰੀ;
	    ਜੇ (ga_grow(ਵੇਖਿਆ, 1) == FAIL)
		ਵਾਪਸ false;
	    ((ਛੋਟਾ *)ਵੇਖਿਆ->ga_ਅੰਕੜੇ)[ਵੇਖਿਆ->ga_len++] = ਵਸਤੂ;
	    // Record the cluster id itself, not just its members: the slow
	    // path matches "item == id" for a cluster id present in the list.
	    ਜੇ (ga_grow(ga, 1) == FAIL)
		ਵਾਪਸ false;
	    ((ਛੋਟਾ *)ga->ga_ਅੰਕੜੇ)[ga->ga_len++] = ਵਸਤੂ;
	    scl_list = SYN_CLSTR(syn_ਖੰਡ)[ਵਸਤੂ - SYNID_CLUSTER].scl_list;
	    ਜੇ (scl_list != ਨਿਰਾ && !idl_collect(scl_list, ga, ਵੇਖਿਆ))
		ਵਾਪਸ false;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (ga_grow(ga, 1) == FAIL)
		ਵਾਪਸ false;
	    ((ਛੋਟਾ *)ga->ga_ਅੰਕੜੇ)[ga->ga_len++] = ਵਸਤੂ;
	}
    }
    ਵਾਪਸ true;
}

    ਸਥਿਰ ਪੂਰਨ
idl_id_cmp(ਅਚਲ ਖਾਲੀ *a, ਅਚਲ ਖਾਲੀ *b)
{
    ਵਾਪਸ (ਪੂਰਨ)*(ਅਚਲ ਛੋਟਾ *)a - (ਪੂਰਨ)*(ਅਚਲ ਛੋਟਾ *)b;
}

/*
 * Get the cache entry that resolves "list", building it if needed.
 * Returns NULL only on allocation failure.
 */
    ਸਥਿਰ idl_ਇੰਦਰਾਜ_T *
idl_get_ਇੰਦਰਾਜ(ਛੋਟਾ *list)
{
    idl_cache_T	*cache = (idl_cache_T *)syn_ਖੰਡ->b_idlist_cache;
    idl_ਇੰਦਰਾਜ_T	*e;
    garray_T	ga, ਵੇਖਿਆ;

    ਜੇ (cache == ਨਿਰਾ)
    {
	cache = ALLOC_CLEAR_ONE(idl_cache_T);
	ਜੇ (cache == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	syn_ਖੰਡ->b_idlist_cache = cache;
    }
    // Fast path: the same list is usually queried many times in a row.
    ਜੇ (cache->ent[cache->ਆਖਰੀ].idl_ਕੁੰਜੀ == list)
	ਵਾਪਸ &cache->ent[cache->ਆਖਰੀ];
    ਲਈ (ਪੂਰਨ i = 0; i < IDL_CACHE_ਮਾਪ; ++i)
	ਜੇ (cache->ent[i].idl_ਕੁੰਜੀ == list)
	{
	    cache->ਆਖਰੀ = i;
	    ਵਾਪਸ &cache->ent[i];
	}

    // Build a new entry, replacing one in round-robin order.
    cache->ਆਖਰੀ = cache->ਗੇੜਾ;
    e = &cache->ent[cache->ਗੇੜਾ];
    cache->ਗੇੜਾ = (cache->ਗੇੜਾ + 1) % IDL_CACHE_ਮਾਪ;
    VIM_CLEAR(e->idl_ids);
    e->idl_ਗਿਣਤੀ = 0;
    e->idl_ਕੁੰਜੀ = list;
    e->idl_marker = 0;
    e->idl_usable = true;

    // A leading ALLBUT/TOP/CONTAINED marker is kept aside; the rest is the set.
    ਜੇ (*list >= SYNID_ALLBUT && *list < SYNID_CLUSTER)
    {
	e->idl_marker = *list;
	++list;
    }

    ga_init2(&ga, ਆਕਾਰ(ਛੋਟਾ), 50);
    ga_init2(&ਵੇਖਿਆ, ਆਕਾਰ(ਛੋਟਾ), 10);
    ਜੇ (!idl_collect(list, &ga, &ਵੇਖਿਆ))
    {
	e->idl_usable = false;
	ga_clear(&ga);
	ga_clear(&ਵੇਖਿਆ);
	ਵਾਪਸ e;
    }
    ga_clear(&ਵੇਖਿਆ);
    ਜੇ (ga.ga_len > 0)
    {
	qsort(ga.ga_ਅੰਕੜੇ, (size_t)ga.ga_len, ਆਕਾਰ(ਛੋਟਾ), idl_id_cmp);
	e->idl_ids = (ਛੋਟਾ *)ga.ga_ਅੰਕੜੇ;	// take ownership
	e->idl_ਗਿਣਤੀ = ga.ga_len;
    }
    ਨਹੀਂਤਾਂ
	ga_clear(&ga);
    ਵਾਪਸ e;
}

/*
 * Return true if "id" is in the resolved set of cache entry "e".
 */
    ਸਥਿਰ bool
idl_ਸ਼ਾਮਲ_ਹੈ(idl_ਇੰਦਰਾਜ_T *e, ਛੋਟਾ id)
{
    ਪੂਰਨ	lo = 0;
    ਪੂਰਨ	hi = e->idl_ਗਿਣਤੀ - 1;

    ਜਦੋਂ (lo <= hi)
    {
	ਪੂਰਨ	mid = (lo + hi) / 2;
	ਛੋਟਾ	v = e->idl_ids[mid];

	ਜੇ (v == id)
	    ਵਾਪਸ true;
	ਜੇ (v < id)
	    lo = mid + 1;
	ਨਹੀਂਤਾਂ
	    hi = mid - 1;
    }
    ਵਾਪਸ false;
}

    ਸਥਿਰ ਖਾਲੀ
idl_cache_clear(synblock_T *ਖੰਡ)
{
    idl_cache_T	*cache = (idl_cache_T *)ਖੰਡ->b_idlist_cache;

    ਜੇ (cache == ਨਿਰਾ)
	ਵਾਪਸ;
    ਲਈ (ਪੂਰਨ i = 0; i < IDL_CACHE_ਮਾਪ; ++i)
	vim_free(cache->ent[i].idl_ids);
    vim_free(cache);
    ਖੰਡ->b_idlist_cache = ਨਿਰਾ;
}
// }}}

/*
 * Check if syntax group "ssp" is in the ID list "list" of "cur_si".
 * "cur_si" can be NULL if not checking the "containedin" list.
 * Used to check if a syntax item is in the "contains" or "nextgroup" list of
 * the current item.
 * This function is called very often, keep it fast!!
 */
    ਸਥਿਰ ਪੂਰਨ
ਵਿੱਚ_id_list(
    stateitem_T	*cur_si,	// current item or NULL
    ਛੋਟਾ	*list,		// id list
    ਬਣਤਰ sp_syn *ssp,		// group id and ":syn include" tag of group
    ਪੂਰਨ		flags)		// group flags
{
    ਪੂਰਨ		retval;
    ਛੋਟਾ	*scl_list;
    ਛੋਟਾ	ਵਸਤੂ;
    ਛੋਟਾ	id = ssp->id;
    ਸਥਿਰ ਪੂਰਨ	ਡੂੰਘਾਈ = 0;
    ਪੂਰਨ		r;
    ਪੂਰਨ		toplevel;
    idl_ਇੰਦਰਾਜ_T	*e;

    // If ssp has a "containedin" list and "cur_si" is in it, return TRUE.
    ਜੇ (cur_si != ਨਿਰਾ && ssp->cont_ਵਿੱਚ_list != ਨਿਰਾ
					    && !(cur_si->si_flags & HL_ਮੇਲ))
    {
	// Ignore transparent items without a contains argument.  Double check
	// that we don't go back past the first one.
	ਜਦੋਂ ((cur_si->si_flags & HL_TRANS_CONT)
		&& cur_si > (stateitem_T *)(current_state.ga_ਅੰਕੜੇ))
	    --cur_si;
	// cur_si->si_idx is -1 for keywords, these never contain anything.
	ਜੇ (cur_si->si_idx >= 0 && ਵਿੱਚ_id_list(ਨਿਰਾ, ssp->cont_ਵਿੱਚ_list,
		&(SYN_ਚੀਜ਼ਾਂ(syn_ਖੰਡ)[cur_si->si_idx].sp_syn),
		  SYN_ਚੀਜ਼ਾਂ(syn_ਖੰਡ)[cur_si->si_idx].sp_flags))
	    ਵਾਪਸ TRUE;
    }

    ਜੇ (list == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    /*
     * If list is ID_LIST_ALL, we are in a transparent item that isn't
     * inside anything.  Only allow not-contained groups.
     */
    ਜੇ (list == ID_LIST_ALL)
	ਵਾਪਸ !(flags & HL_CONTAINED);

    /*
     * Is this top-level (i.e. not 'contained') in the file it was declared in?
     * For included files, this is different from HL_CONTAINED, which is set
     * unconditionally.
     */
    toplevel = !(flags & HL_CONTAINED) || (flags & HL_INCLUDED_TOPLEVEL);

    /*
     * Fast path: use the resolved, sorted set cached for this list.  Only the
     * leading ALLBUT/TOP/CONTAINED gate depends on "ssp"/"flags", so it is
     * applied here; membership itself is a binary search.  Can be turned off
     * with test_override('syn_idlist_cache', 1) to verify the result matches
     * the slow path.
     */
    e = disable_syn_idlist_cache_for_testing ? ਨਿਰਾ : idl_get_ਇੰਦਰਾਜ(list);
    ਜੇ (e != ਨਿਰਾ && e->idl_usable)
    {
	ਜੇ (e->idl_marker != 0)
	{
	    ਵਸਤੂ = e->idl_marker;
	    ਜੇ (ਵਸਤੂ < SYNID_ਸਿਖਰ)
	    {
		ਜੇ (ਵਸਤੂ - SYNID_ALLBUT != ssp->inc_ਟੈਗ)
		    ਵਾਪਸ FALSE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (ਵਸਤੂ < SYNID_CONTAINED)
	    {
		ਜੇ (ਵਸਤੂ - SYNID_ਸਿਖਰ != ssp->inc_ਟੈਗ || !toplevel)
		    ਵਾਪਸ FALSE;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (ਵਸਤੂ - SYNID_CONTAINED != ssp->inc_ਟੈਗ || toplevel)
		    ਵਾਪਸ FALSE;
	    }
	    retval = FALSE;
	}
	ਨਹੀਂਤਾਂ
	    retval = TRUE;
	ਵਾਪਸ idl_ਸ਼ਾਮਲ_ਹੈ(e, id) ? retval : !retval;
    }

    /*
     * Slow path (uncacheable list, e.g. a nested ALLBUT).
     * If the first item is "ALLBUT", return TRUE if "id" is NOT in the
     * contains list.  We also require that "id" is at the same ":syn include"
     * level as the list.
     */
    ਵਸਤੂ = *list;
    ਜੇ (ਵਸਤੂ >= SYNID_ALLBUT && ਵਸਤੂ < SYNID_CLUSTER)
    {
	ਜੇ (ਵਸਤੂ < SYNID_ਸਿਖਰ)
	{
	    // ALL or ALLBUT: accept all groups in the same file
	    ਜੇ (ਵਸਤੂ - SYNID_ALLBUT != ssp->inc_ਟੈਗ)
		ਵਾਪਸ FALSE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਵਸਤੂ < SYNID_CONTAINED)
	{
	    // TOP: accept all not-contained groups in the same file
	    ਜੇ (ਵਸਤੂ - SYNID_ਸਿਖਰ != ssp->inc_ਟੈਗ || !toplevel)
		ਵਾਪਸ FALSE;
	}
	ਨਹੀਂਤਾਂ
	{
	    // CONTAINED: accept all contained groups in the same file
	    ਜੇ (ਵਸਤੂ - SYNID_CONTAINED != ssp->inc_ਟੈਗ || toplevel)
		ਵਾਪਸ FALSE;
	}
	ਵਸਤੂ = *++list;
	retval = FALSE;
    }
    ਨਹੀਂਤਾਂ
	retval = TRUE;

    /*
     * Return "retval" if id is in the contains list.
     */
    ਜਦੋਂ (ਵਸਤੂ != 0)
    {
	ਜੇ (ਵਸਤੂ == id)
	    ਵਾਪਸ retval;
	ਜੇ (ਵਸਤੂ >= SYNID_CLUSTER)
	{
	    scl_list = SYN_CLSTR(syn_ਖੰਡ)[ਵਸਤੂ - SYNID_CLUSTER].scl_list;
	    // restrict recursiveness to 30 to avoid an endless loop for a
	    // cluster that includes itself (indirectly)
	    ਜੇ (scl_list != ਨਿਰਾ && ਡੂੰਘਾਈ < 30)
	    {
		++ਡੂੰਘਾਈ;
		r = ਵਿੱਚ_id_list(ਨਿਰਾ, scl_list, ssp, flags);
		--ਡੂੰਘਾਈ;
		ਜੇ (r)
		    ਵਾਪਸ retval;
	    }
	}
	ਵਸਤੂ = *++list;
    }
    ਵਾਪਸ !retval;
}

ਬਣਤਰ subcommand
{
    ਅੱਖਰ    *name;			// subcommand name
    ਖਾਲੀ    (*func)(exarg_T *, ਪੂਰਨ);	// function to call
};

ਸਥਿਰ ਬਣਤਰ subcommand subcommands[] =
{
    {"case",		syn_cmd_case},
    {"clear",		syn_cmd_clear},
    {"cluster",		syn_cmd_cluster},
    {"conceal",		syn_cmd_conceal},
    {"enable",		syn_cmd_enable},
    {"foldlevel",	syn_cmd_foldlevel},
    {"include",		syn_cmd_include},
    {"iskeyword",	syn_cmd_iskeyword},
    {"keyword",		syn_cmd_keyword},
    {"list",		syn_cmd_list},
    {"manual",		syn_cmd_manual},
    {"match",		syn_cmd_ਮੇਲ},
    {"on",		syn_cmd_on},
    {"off",		syn_cmd_off},
    {"region",		syn_cmd_region},
    {"reset",		syn_cmd_reset},
    {"spell",		syn_cmd_spell},
    {"sync",		syn_cmd_sync},
    {"",		syn_cmd_list}
};

/*
 * ":syntax".
 * This searches the subcommands[] table for the subcommand name, and calls a
 * syntax_subcommand() function to do the rest.
 */
    ਖਾਲੀ
ex_syntax(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*subcmd_ਸਿਰਾ;
    char_u	*subcmd_name;
    ਪੂਰਨ		i;

    syn_cmdlinep = eap->cmdlinep;

    // isolate subcommand name
    ਲਈ (subcmd_ਸਿਰਾ = arg; ASCII_ISALPHA(*subcmd_ਸਿਰਾ); ++subcmd_ਸਿਰਾ)
	;
    subcmd_name = vim_strnsave(arg, subcmd_ਸਿਰਾ - arg);
    ਜੇ (subcmd_name == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (eap->skip)		// skip error messages for all subcommands
	++emsg_skip;
    ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(subcommands); ++i)
    {
	ਜੇ (STRCMP(subcmd_name, (char_u *)subcommands[i].name) == 0)
	{
	    eap->arg = skipwhite(subcmd_ਸਿਰਾ);
	    (subcommands[i].func)(eap, FALSE);
	    ਰੋਕੋ;
	}
    }

    ਜੇ (i == (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(subcommands))
	semsg(_(e_invalid_syntax_subcommand_str), subcmd_name);

    vim_free(subcmd_name);
    ਜੇ (eap->skip)
	--emsg_skip;
}

    ਖਾਲੀ
ex_ownsyntax(exarg_T *eap)
{
    char_u	*old_ਮੁੱਲ;
    char_u	*new_ਮੁੱਲ_2;

    ਜੇ (curwin->w_s == &curwin->w_ਬਫਰ->b_s)
    {
	curwin->w_s = ALLOC_ONE(synblock_T);
	CLEAR_ਸੂਚਕ(curwin->w_s);
	hash_init(&curwin->w_s->b_keywtab);
	hash_init(&curwin->w_s->b_keywtab_ic);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
	// TODO: keep the spell checking as it was.
	curwin->w_p_spell = FALSE;	// No spell checking
	// make sure option values are "empty_option" instead of NULL
	clear_string_option(&curwin->w_s->b_p_spc);
	clear_string_option(&curwin->w_s->b_p_spf);
	clear_string_option(&curwin->w_s->b_p_spl);
	clear_string_option(&curwin->w_s->b_p_spo);
#ਜੇ_ਅੰਤ
	clear_string_option(&curwin->w_s->b_syn_isk);
    }

    // save value of b:current_syntax
    old_ਮੁੱਲ = get_var_ਮੁੱਲ((char_u *)"b:current_syntax");
    ਜੇ (old_ਮੁੱਲ != ਨਿਰਾ)
	old_ਮੁੱਲ = vim_strsave(old_ਮੁੱਲ);

    // Apply the "syntax" autocommand event, this finds and loads the syntax
    // file.
    apply_autocmds(EVENT_SYNTAX, eap->arg, curbuf->b_fname, TRUE, curbuf);

    // move value of b:current_syntax to w:current_syntax
    new_ਮੁੱਲ_2 = get_var_ਮੁੱਲ((char_u *)"b:current_syntax");
    ਜੇ (new_ਮੁੱਲ_2 != ਨਿਰਾ)
	ਸੈੱਟ_ਅੰਦਰੂਨੀ_string_var((char_u *)"w:current_syntax", new_ਮੁੱਲ_2);

    // restore value of b:current_syntax
    ਜੇ (old_ਮੁੱਲ == ਨਿਰਾ)
	do_unlet((char_u *)"b:current_syntax", TRUE);
    ਨਹੀਂਤਾਂ
    {
	ਸੈੱਟ_ਅੰਦਰੂਨੀ_string_var((char_u *)"b:current_syntax", old_ਮੁੱਲ);
	vim_free(old_ਮੁੱਲ);
    }
}

    ਪੂਰਨ
syntax_present(win_T *win)
{
    ਵਾਪਸ (win->w_s->b_syn_patterns.ga_len != 0
	    || win->w_s->b_syn_clusters.ga_len != 0
	    || win->w_s->b_keywtab.ht_ਵਰਤਿਆ > 0
	    || win->w_s->b_keywtab_ic.ht_ਵਰਤਿਆ > 0);
}


ਸਥਿਰ ਗਿਣਤੀ
{
    EXP_SUBCMD,	    // expand ":syn" sub-commands
    EXP_CASE,	    // expand ":syn case" arguments
    EXP_SPELL,	    // expand ":syn spell" arguments
    EXP_SYNC,	    // expand ":syn sync" arguments
    EXP_CLUSTER	    // expand ":syn list @cluster" arguments
} expand_what;

/*
 * Reset include_link, include_default, include_none to 0.
 * Called when we are done expanding.
 */
    ਖਾਲੀ
reset_expand_highlight(ਖਾਲੀ)
{
    include_link = include_default = include_none = 0;
}

/*
 * Handle command line completion for :match and :echohl command: Add "NONE"
 * as highlight group.
 */
    ਖਾਲੀ
ਸੈੱਟ_context_ਵਿੱਚ_echohl_cmd(expand_T *xp, char_u *arg)
{
    xp->xp_context = EXPAND_HIGHLIGHT;
    xp->xp_ਨਮੂਨਾ = arg;
    include_none = 1;
}

/*
 * Handle command line completion for :syntax command.
 */
    ਖਾਲੀ
ਸੈੱਟ_context_ਵਿੱਚ_syntax_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    // Default: expand subcommands
    xp->xp_context = EXPAND_SYNTAX;
    expand_what = EXP_SUBCMD;
    xp->xp_ਨਮੂਨਾ = arg;
    include_link = 0;
    include_default = 0;

    ਜੇ (*arg == NUL)
	ਵਾਪਸ;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    ਜੇ (*p == NUL)
	ਵਾਪਸ;

    // past first word
    xp->xp_ਨਮੂਨਾ = skipwhite(p);
    ਜੇ (*skiptowhite(xp->xp_ਨਮੂਨਾ) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(arg, "case", p - arg) == 0)
	expand_what = EXP_CASE;
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(arg, "spell", p - arg) == 0)
	expand_what = EXP_SPELL;
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(arg, "sync", p - arg) == 0)
	expand_what = EXP_SYNC;
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(arg, "list", p - arg) == 0)
    {
	p = skipwhite(p);
	ਜੇ (*p == '@')
	    expand_what = EXP_CLUSTER;
	ਨਹੀਂਤਾਂ
	    xp->xp_context = EXPAND_HIGHLIGHT;
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(arg, "keyword", p - arg) == 0
	    || STRNICMP(arg, "region", p - arg) == 0
	    || STRNICMP(arg, "match", p - arg) == 0)
	xp->xp_context = EXPAND_HIGHLIGHT;
    ਨਹੀਂਤਾਂ
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Function given to ExpandGeneric() to obtain the list syntax names for
 * expansion.
 */
    char_u *
get_syntax_name(expand_T *xp, ਪੂਰਨ idx)
{
    ਚੋਣ (expand_what)
    {
	ਹਾਲਤ EXP_SUBCMD:
	    ਜੇ (idx < 0 || idx >= (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(subcommands))
		ਵਾਪਸ ਨਿਰਾ;
	    ਵਾਪਸ (char_u *)subcommands[idx].name;
	ਹਾਲਤ EXP_CASE:
	{
	    ਸਥਿਰ ਅੱਖਰ *case_ਦਲੀਲਾਂ[] = {"match", "ignore", ਨਿਰਾ};
	    ਵਾਪਸ (char_u *)case_ਦਲੀਲਾਂ[idx];
	}
	ਹਾਲਤ EXP_SPELL:
	{
	    ਸਥਿਰ ਅੱਖਰ *spell_ਦਲੀਲਾਂ[] =
		{"toplevel", "notoplevel", "default", ਨਿਰਾ};
	    ਵਾਪਸ (char_u *)spell_ਦਲੀਲਾਂ[idx];
	}
	ਹਾਲਤ EXP_SYNC:
	{
	    ਸਥਿਰ ਅੱਖਰ *sync_ਦਲੀਲਾਂ[] =
		{"ccomment", "clear", "fromstart",
		 "linebreaks=", "linecont", "lines=", "match",
		 "maxlines=", "minlines=", "region", ਨਿਰਾ};
	    ਵਾਪਸ (char_u *)sync_ਦਲੀਲਾਂ[idx];
	}
	ਹਾਲਤ EXP_CLUSTER:
	{
	    ਜੇ (idx < curwin->w_s->b_syn_clusters.ga_len)
	    {
		vim_snprintf((ਅੱਖਰ *)xp->xp_buf, EXPAND_BUF_LEN, "@%s",
					 SYN_CLSTR(curwin->w_s)[idx].scl_name);
		ਵਾਪਸ xp->xp_buf;
	    }
	    ਨਹੀਂਤਾਂ
		ਵਾਪਸ ਨਿਰਾ;
	}
    }
    ਵਾਪਸ ਨਿਰਾ;
}


/*
 * Function called for expression evaluation: get syntax ID at file position.
 */
    ਪੂਰਨ
syn_get_id(
    win_T	*wp,
    ਲੰਮਾ	lnum,
    colnr_T	ਖਾਨਾ,
    ਪੂਰਨ		trans,	     // remove transparency
    ਪੂਰਨ		*spellp,     // return: can do spell checking
    ਪੂਰਨ		keep_state)  // keep state of char at "col"
{
    // When the position is not after the current position and in the same
    // line of the same window with the same buffer, need to restart parsing.
    ਜੇ (wp != syn_win
	    || wp->w_ਬਫਰ != syn_buf
	    || lnum != current_lnum
	    || ਖਾਨਾ < current_ਖਾਨਾ)
	syntax_ਸ਼ੁਰੂ(wp, lnum);
    ਨਹੀਂਤਾਂ ਜੇ (wp->w_ਬਫਰ == syn_buf
	    && lnum == current_lnum
	    && ਖਾਨਾ > current_ਖਾਨਾ)
	// next_match may not be correct when moving around, e.g. with the
	// "skip" expression in searchpair()
	ਅਗਲਾ_ਮੇਲ_idx = -1;

    (ਖਾਲੀ)get_syntax_attr(ਖਾਨਾ, spellp, keep_state);

    ਵਾਪਸ (trans ? current_trans_id : current_id);
}

#ਜੇ defined(FEAT_CONCEAL)
/*
 * Get extra information about the syntax item.  Must be called right after
 * get_syntax_attr().
 * Stores the current item sequence nr in "*seqnrp".
 * Returns the current flags.
 */
    ਪੂਰਨ
get_syntax_info(ਪੂਰਨ *seqnrp)
{
    *seqnrp = current_seqnr;
    ਵਾਪਸ current_flags;
}

/*
 * Return conceal substitution character
 */
    ਪੂਰਨ
syn_get_sub_char(ਖਾਲੀ)
{
    ਵਾਪਸ current_sub_char;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL)
/*
 * Return the syntax ID at position "i" in the current stack.
 * The caller must have called syn_get_id() before to fill the stack.
 * Returns -1 when "i" is out of range.
 */
    ਪੂਰਨ
syn_get_ਢੇਰ_ਵਸਤੂ(ਪੂਰਨ i)
{
    ਜੇ (i >= current_state.ga_len)
    {
	// Need to invalidate the state, because we didn't properly finish it
	// for the last character, "keep_state" was TRUE.
	invalidate_current_state();
	current_ਖਾਨਾ = MAXCOL;
	ਵਾਪਸ -1;
    }
    ਵਾਪਸ CUR_STATE(i).si_id;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_FOLDING)
    ਸਥਿਰ ਪੂਰਨ
syn_cur_foldlevel(ਖਾਲੀ)
{
    ਪੂਰਨ		level = 0;
    ਪੂਰਨ		i;

    ਲਈ (i = 0; i < current_state.ga_len; ++i)
	ਜੇ (CUR_STATE(i).si_flags & HL_FOLD)
	    ++level;
    ਵਾਪਸ level;
}

/*
 * Function called to get folding level for line "lnum" in window "wp".
 */
    ਪੂਰਨ
syn_get_foldlevel(win_T *wp, ਲੰਮਾ lnum)
{
    ਪੂਰਨ		level = 0;
    ਪੂਰਨ		ਨੀਵਾਂ_level;
    ਪੂਰਨ		cur_level;

    // Return quickly when there are no fold items at all.
    ਜੇ (wp->w_s->b_syn_folditems != 0
	    && !wp->w_s->b_syn_error
# ifdef SYN_ਸਮਾਂ_ਹੱਦਬੰਦੀ
	    && !wp->w_s->b_syn_slow
# endif
	    )
    {
	syntax_ਸ਼ੁਰੂ(wp, lnum);

	// Start with the fold level at the start of the line.
	level = syn_cur_foldlevel();

	ਜੇ (wp->w_s->b_syn_foldlevel == SYNFLD_MINIMUM)
	{
	    // Find the lowest fold level that is followed by a higher one.
	    cur_level = level;
	    ਨੀਵਾਂ_level = cur_level;
	    ਜਦੋਂ (!current_ਮੁਕੰਮਲ)
	    {
		(ਖਾਲੀ)syn_current_attr(FALSE, FALSE, ਨਿਰਾ, FALSE);
		cur_level = syn_cur_foldlevel();
		ਜੇ (cur_level < ਨੀਵਾਂ_level)
		    ਨੀਵਾਂ_level = cur_level;
		ਨਹੀਂਤਾਂ ਜੇ (cur_level > ਨੀਵਾਂ_level)
		    level = ਨੀਵਾਂ_level;
		++current_ਖਾਨਾ;
	    }
	}
    }
    ਜੇ (level > wp->w_p_fdn)
    {
	level = wp->w_p_fdn;
	ਜੇ (level < 0)
	    level = 0;
    }
    ਵਾਪਸ level;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_PROFILE)
/*
 * ":syntime".
 */
    ਖਾਲੀ
ex_syntime(exarg_T *eap)
{
    ਜੇ (STRCMP(eap->arg, "on") == 0)
	syn_ਸਮਾਂ_on = TRUE;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(eap->arg, "off") == 0)
	syn_ਸਮਾਂ_on = FALSE;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(eap->arg, "clear") == 0)
	syntime_clear();
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(eap->arg, "report") == 0)
	syntime_report();
    ਨਹੀਂਤਾਂ
	semsg(_(e_invalid_ਦਲੀਲ_str), eap->arg);
}

    ਸਥਿਰ ਖਾਲੀ
syn_clear_ਸਮਾਂ(syn_ਸਮਾਂ_T *st)
{
    profile_zero(&st->ਕੁੱਲ);
    profile_zero(&st->slowest);
    st->ਗਿਣਤੀ_2 = 0;
    st->ਮੇਲ = 0;
}

/*
 * Clear the syntax timing for the current buffer.
 */
    ਸਥਿਰ ਖਾਲੀ
syntime_clear(ਖਾਲੀ)
{
    ਪੂਰਨ		idx;
    synpat_T	*spp;

    ਜੇ (!syntax_present(curwin))
    {
	ਸੁਨੇਹਾ(_(ਸੁਨੇਹਾ_no_ਚੀਜ਼ਾਂ));
	ਵਾਪਸ;
    }
    ਲਈ (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len; ++idx)
    {
	spp = &(SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx]);
	syn_clear_ਸਮਾਂ(&spp->sp_ਸਮਾਂ);
    }
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":syntime {on,off,clear,report}" command.
 */
    char_u *
get_syntime_arg(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਚੋਣ (idx)
    {
	ਹਾਲਤ 0: ਵਾਪਸ (char_u *)"on";
	ਹਾਲਤ 1: ਵਾਪਸ (char_u *)"off";
	ਹਾਲਤ 2: ਵਾਪਸ (char_u *)"clear";
	ਹਾਲਤ 3: ਵਾਪਸ (char_u *)"report";
    }
    ਵਾਪਸ ਨਿਰਾ;
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    proftime_T	ਕੁੱਲ;
    ਪੂਰਨ		ਗਿਣਤੀ_2;
    ਪੂਰਨ		ਮੇਲ;
    proftime_T	slowest;
    proftime_T	ਔਸਤਨ;
    ਪੂਰਨ		id;
    char_u	*ਨਮੂਨਾ;
} ਸਮਾਂ_ਇੰਦਰਾਜ_T;

    ਸਥਿਰ ਪੂਰਨ
syn_compare_syntime(ਅਚਲ ਖਾਲੀ *v1, ਅਚਲ ਖਾਲੀ *v2)
{
    ਅਚਲ ਸਮਾਂ_ਇੰਦਰਾਜ_T	*s1 = v1;
    ਅਚਲ ਸਮਾਂ_ਇੰਦਰਾਜ_T	*s2 = v2;

    ਵਾਪਸ profile_cmp(&s1->ਕੁੱਲ, &s2->ਕੁੱਲ);
}

/*
 * Clear the syntax timing for the current buffer.
 */
    ਸਥਿਰ ਖਾਲੀ
syntime_report(ਖਾਲੀ)
{
    ਪੂਰਨ		idx;
    synpat_T	*spp;
# ਜੇ defined(FEAT_RELTIME)
    proftime_T	tm;
# endif
    ਪੂਰਨ		len;
    ਪੂਰਨ		patlen;
    proftime_T	ਕੁੱਲ_ਕੁੱਲ;
    ਪੂਰਨ		ਕੁੱਲ_ਗਿਣਤੀ = 0;
    garray_T    ga;
    ਸਮਾਂ_ਇੰਦਰਾਜ_T *p;

    ਜੇ (!syntax_present(curwin))
    {
	ਸੁਨੇਹਾ(_(ਸੁਨੇਹਾ_no_ਚੀਜ਼ਾਂ));
	ਵਾਪਸ;
    }

    ga_init2(&ga, ਆਕਾਰ(ਸਮਾਂ_ਇੰਦਰਾਜ_T), 50);
    profile_zero(&ਕੁੱਲ_ਕੁੱਲ);
    ਲਈ (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len; ++idx)
    {
	spp = &(SYN_ਚੀਜ਼ਾਂ(curwin->w_s)[idx]);
	ਜੇ (spp->sp_ਸਮਾਂ.ਗਿਣਤੀ_2 > 0)
	{
	    (ਖਾਲੀ)ga_grow(&ga, 1);
	    p = ((ਸਮਾਂ_ਇੰਦਰਾਜ_T *)ga.ga_ਅੰਕੜੇ) + ga.ga_len;
	    p->ਕੁੱਲ = spp->sp_ਸਮਾਂ.ਕੁੱਲ;
	    profile_ਜੋੜੋ(&ਕੁੱਲ_ਕੁੱਲ, &spp->sp_ਸਮਾਂ.ਕੁੱਲ);
	    p->ਗਿਣਤੀ_2 = spp->sp_ਸਮਾਂ.ਗਿਣਤੀ_2;
	    p->ਮੇਲ = spp->sp_ਸਮਾਂ.ਮੇਲ;
	    ਕੁੱਲ_ਗਿਣਤੀ += spp->sp_ਸਮਾਂ.ਗਿਣਤੀ_2;
	    p->slowest = spp->sp_ਸਮਾਂ.slowest;
# ਜੇ defined(FEAT_RELTIME)
	    profile_divide(&spp->sp_ਸਮਾਂ.ਕੁੱਲ, spp->sp_ਸਮਾਂ.ਗਿਣਤੀ_2, &tm);
	    p->ਔਸਤਨ = tm;
# endif
	    p->id = spp->sp_syn.id;
	    p->ਨਮੂਨਾ = spp->sp_ਨਮੂਨਾ;
	    ++ga.ga_len;
	}
    }

    // Sort on total time. Skip if there are no items to avoid passing NULL
    // pointer to qsort().
    ਜੇ (ga.ga_len > 1)
	qsort(ga.ga_ਅੰਕੜੇ, (size_t)ga.ga_len, ਆਕਾਰ(ਸਮਾਂ_ਇੰਦਰਾਜ_T),
							 syn_compare_syntime);

    ਸੁਨੇਹਾ_puts_title(_("  TOTAL      COUNT  MATCH   SLOWEST     AVERAGE   NAME               PATTERN"));
    ਸੁਨੇਹਾ_puts("\n");
    ਲਈ (idx = 0; idx < ga.ga_len && !got_int; ++idx)
    {
	p = ((ਸਮਾਂ_ਇੰਦਰਾਜ_T *)ga.ga_ਅੰਕੜੇ) + idx;

	ਸੁਨੇਹਾ_puts(profile_ਸੁਨੇਹਾ(&p->ਕੁੱਲ));
	ਸੁਨੇਹਾ_puts(" "); // make sure there is always a separating space
	ਸੁਨੇਹਾ_advance(13);
	ਸੁਨੇਹਾ_outnum(p->ਗਿਣਤੀ_2);
	ਸੁਨੇਹਾ_puts(" ");
	ਸੁਨੇਹਾ_advance(20);
	ਸੁਨੇਹਾ_outnum(p->ਮੇਲ);
	ਸੁਨੇਹਾ_puts(" ");
	ਸੁਨੇਹਾ_advance(26);
	ਸੁਨੇਹਾ_puts(profile_ਸੁਨੇਹਾ(&p->slowest));
	ਸੁਨੇਹਾ_puts(" ");
	ਸੁਨੇਹਾ_advance(38);
	ਸੁਨੇਹਾ_puts(profile_ਸੁਨੇਹਾ(&p->ਔਸਤਨ));
	ਸੁਨੇਹਾ_puts(" ");
	ਸੁਨੇਹਾ_advance(50);
	ਸੁਨੇਹਾ_outtrans(highlight_ਸਮੂਹ_name(p->id - 1));
	ਸੁਨੇਹਾ_puts(" ");

	ਸੁਨੇਹਾ_advance(69);
	ਜੇ (Columns < 80)
	    len = 20; // will wrap anyway
	ਨਹੀਂਤਾਂ
	    len = Columns - 70;
	patlen = (ਪੂਰਨ)STRLEN(p->ਨਮੂਨਾ);
	ਜੇ (len > patlen)
	    len = patlen;
	ਸੁਨੇਹਾ_outtrans_len(p->ਨਮੂਨਾ, len);
	ਸੁਨੇਹਾ_puts("\n");
    }
    ga_clear(&ga);
    ਜੇ (!got_int)
    {
	ਸੁਨੇਹਾ_puts("\n");
	ਸੁਨੇਹਾ_puts(profile_ਸੁਨੇਹਾ(&ਕੁੱਲ_ਕੁੱਲ));
	ਸੁਨੇਹਾ_advance(13);
	ਸੁਨੇਹਾ_outnum(ਕੁੱਲ_ਗਿਣਤੀ);
	ਸੁਨੇਹਾ_puts("\n");
    }
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ // FEAT_SYN_HL
