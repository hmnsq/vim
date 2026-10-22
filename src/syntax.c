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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_SYN_HL)

// different types of offsets that are possible
#ವ್ಯಾಖ್ಯಾನಿಸು SPO_MS_OFF	0	// match  start offset
#ವ್ಯಾಖ್ಯಾನಿಸು SPO_ME_OFF	1	// match  end	offset
#ವ್ಯಾಖ್ಯಾನಿಸು SPO_HS_OFF	2	// highl. start offset
#ವ್ಯಾಖ್ಯಾನಿಸು SPO_HE_OFF	3	// highl. end	offset
#ವ್ಯಾಖ್ಯಾನಿಸು SPO_RS_OFF	4	// region start offset
#ವ್ಯಾಖ್ಯಾನಿಸು SPO_RE_OFF	5	// region end	offset
#ವ್ಯಾಖ್ಯಾನಿಸು SPO_LC_OFF	6	// leading context offset
#ವ್ಯಾಖ್ಯಾನಿಸು SPO_ಎಣಿಕೆ	7

ಸ್ಥಿರ ಅಕ್ಷರ *(spo_name_tab[SPO_ಎಣಿಕೆ]) =
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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ syn_ನಮೂನೆ
{
    ಅಕ್ಷರ	 sp_type;		// see SPTYPE_ defines below
    ಅಕ್ಷರ	 sp_syncing;		// this item used for syncing
    ಗಿಡ್ಡ	 sp_syn_ಹೊಂದಿಕೆ_id;	// highlight group ID of pattern
    ಗಿಡ್ಡ	 sp_off_flags;		// see below
    ಪೂರ್ಣಾಂಕ		 sp_offsets[SPO_ಎಣಿಕೆ];	// offsets
    ಪೂರ್ಣಾಂಕ		 sp_flags;		// see HL_ defines below
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    ಪೂರ್ಣಾಂಕ		 sp_cchar;		// conceal substitute character
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		 sp_ic;			// ignore-case flag for sp_prog
    ಪೂರ್ಣಾಂಕ		 sp_sync_idx;		// sync item index (syncing only)
    ಪೂರ್ಣಾಂಕ		 sp_ಸಾಲು_id;		// ID of last line where tried
    ಪೂರ್ಣಾಂಕ		 sp_startcol;		// next match in sp_line_id line
    ಗಿಡ್ಡ	*sp_cont_list;		// cont. group IDs, if non-zero
    ಗಿಡ್ಡ	*sp_ಮುಂದಿನ_list;		// next group IDs, if non-zero
    ರಚನೆ sp_syn sp_syn;		// struct passed to in_id_list()
    char_u	*sp_ನಮೂನೆ;		// regexp to match, pattern
    regprog_T	*sp_prog;		// regexp to match, program
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    syn_ಸಮಯ_T	 sp_ಸಮಯ;
#ಆದರೆ_ಅಂತ್ಯ
} synpat_T;

// The sp_off_flags are computed like this:
// offset from the start of the matched text: (1 << SPO_XX_OFF)
// offset from the end	 of the matched text: (1 << (SPO_XX_OFF + SPO_COUNT))
// When both are present, only one is used.

#ವ್ಯಾಖ್ಯಾನಿಸು SPTYPE_ಹೊಂದಿಕೆ	1	// match keyword with this group ID
#ವ್ಯಾಖ್ಯಾನಿಸು SPTYPE_ಆರಂಭ	2	// match a regexp, start of item
#ವ್ಯಾಖ್ಯಾನಿಸು SPTYPE_ಕೊನೆ	3	// match a regexp, end of item
#ವ್ಯಾಖ್ಯಾನಿಸು SPTYPE_SKIP	4	// match a regexp, skip within item


#ವ್ಯಾಖ್ಯಾನಿಸು SYN_ವಸ್ತುಗಳು(buf)	((synpat_T *)((buf)->b_syn_patterns.ga_ದತ್ತಾಂಶ))

#ವ್ಯಾಖ್ಯಾನಿಸು NONE_IDX	(-2)	// value of sp_sync_idx for "NONE"

/*
 * Flags for b_syn_sync_flags:
 */
#ವ್ಯಾಖ್ಯಾನಿಸು SF_CCOMMENT	0x01	// sync on a C-style comment
#ವ್ಯಾಖ್ಯಾನಿಸು SF_ಹೊಂದಿಕೆ	0x02	// sync by matching a pattern

#ವ್ಯಾಖ್ಯಾನಿಸು SYN_STATE_P(ssp)    ((bufstate_T *)((ssp)->ga_ದತ್ತಾಂಶ))

#ವ್ಯಾಖ್ಯಾನಿಸು MAXKEYWLEN	80	    // maximum length of a keyword

/*
 * The attributes of the syntax item that has been recognized.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ current_attr = 0;	    // attr of current syntax word
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ current_id = 0;	    // ID of current char for syn_get_id()
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ current_trans_id = 0;    // idem, transparency removed
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ current_flags = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ current_seqnr = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ current_sub_char = 0;
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ syn_cluster_S
{
    char_u	    *scl_name;	    // syntax cluster name
    char_u	    *scl_name_u;    // uppercase of scl_name
    ಗಿಡ್ಡ	    *scl_list;	    // IDs in this syntax cluster
} syn_cluster_T;

/*
 * Methods of combining two clusters
 */
#ವ್ಯಾಖ್ಯಾನಿಸು CLUSTER_REPLACE	    1	// replace first list with second
#ವ್ಯಾಖ್ಯಾನಿಸು CLUSTER_ಕೂಡಿಸು	    2	// add second list to first
#ವ್ಯಾಖ್ಯಾನಿಸು CLUSTER_ಕಳೆ    3	// subtract second list from first

#ವ್ಯಾಖ್ಯಾನಿಸು SYN_CLSTR(buf)	((syn_cluster_T *)((buf)->b_syn_clusters.ga_ದತ್ತಾಂಶ))

/*
 * Syntax group IDs have different types:
 *     0 - 19999  normal syntax groups
 * 20000 - 20999  ALLBUT indicator (current_syn_inc_tag added)
 * 21000 - 21999  TOP indicator (current_syn_inc_tag added)
 * 22000 - 22999  CONTAINED indicator (current_syn_inc_tag added)
 * 23000 - 32767  cluster IDs (subtract SYNID_CLUSTER for the cluster ID)
 */
#ವ್ಯಾಖ್ಯಾನಿಸು SYNID_ALLBUT	MAX_HL_ID   // syntax group ID for contains=ALLBUT
#ವ್ಯಾಖ್ಯಾನಿಸು SYNID_ತುದಿ	21000	    // syntax group ID for contains=TOP
#ವ್ಯಾಖ್ಯಾನಿಸು SYNID_CONTAINED	22000	    // syntax group ID for contains=CONTAINED
#ವ್ಯಾಖ್ಯಾನಿಸು SYNID_CLUSTER	23000	    // first syntax group ID for clusters

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_SYN_INC_ಟ್ಯಾಗ್	999	    // maximum before the above overflow
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_CLUSTER_ID  (32767 - SYNID_CLUSTER)

/*
 * Annoying Hack(TM):  ":syn include" needs this pointer to pass to
 * expand_filename().  Most of the other syntax commands don't need it, so
 * instead of passing it to them, we stow it here.
 */
ಸ್ಥಿರ char_u **syn_cmdlinep;

/*
 * Another Annoying Hack(TM):  To prevent rules from other ":syn include"'d
 * files from leaking into ALLBUT lists, we assign a unique ID to the
 * rules in each ":syn include"'d file.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ current_syn_inc_ಟ್ಯಾಗ್ = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ running_syn_inc_ಟ್ಯಾಗ್ = 0;

/*
 * In a hashtable item "hi_key" points to "keyword" in a keyentry.
 * This avoids adding a pointer to the hashtable item.
 * KE2HIKEY() converts a var pointer to a hashitem key pointer.
 * HIKEY2KE() converts a hashitem key pointer to a var pointer.
 * HI2KE() converts a hashitem pointer to a var pointer.
 */
ಸ್ಥಿರ keyentry_T dumkey;
#ವ್ಯಾಖ್ಯಾನಿಸು KE2HIKEY(kp)  ((kp)->keyword)
#ವ್ಯಾಖ್ಯಾನಿಸು HIKEY2KE(p)   ((keyentry_T *)((p) - (dumkey.keyword - (char_u *)&dumkey)))
#ವ್ಯಾಖ್ಯಾನಿಸು HI2KE(hi)      HIKEY2KE((hi)->hi_ಕೀಲಿ)

/*
 * To reduce the time spent in keepend(), remember at which level in the state
 * stack the first item with "keepend" is present.  When "-1", there is no
 * "keepend" on the stack.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ keepend_level = -1;

ಸ್ಥಿರ ಅಕ್ಷರ ಸಂದೇಶ_no_ವಸ್ತುಗಳು[] = N_("No Syntax items defined for this buffer");

/*
 * For the current state we need to remember more than just the idx.
 * When si_m_endpos.lnum is 0, the items other than si_idx are unknown.
 * (The end positions have the column number of the next char)
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ state_ಅಂಶ
{
    ಪೂರ್ಣಾಂಕ		si_idx;			// index of syntax pattern or
					// KEYWORD_IDX
    ಪೂರ್ಣಾಂಕ		si_id;			// highlight group ID for keywords
    ಪೂರ್ಣಾಂಕ		si_trans_id;		// idem, transparency removed
    ಪೂರ್ಣಾಂಕ		si_m_lnum;		// lnum of the match
    ಪೂರ್ಣಾಂಕ		si_m_startcol;		// starting column of the match
    lpos_T	si_m_endpos;		// just after end posn of the match
    lpos_T	si_h_startpos;		// start position of the highlighting
    lpos_T	si_h_endpos;		// end position of the highlighting
    lpos_T	si_eoe_pos;		// end position of end pattern
    ಪೂರ್ಣಾಂಕ		si_ಕೊನೆ_idx;		// group ID for end pattern or zero
    ಪೂರ್ಣಾಂಕ		si_ತುದಿಗಳು;		// if match ends before si_m_endpos
    ಪೂರ್ಣಾಂಕ		si_attr;		// attributes in this state
    ದೀರ್ಘ	si_flags;		// HL_HAS_EOL flag in this state, and
					// HL_SKIP* for si_next_list
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    ಪೂರ್ಣಾಂಕ		si_seqnr;		// sequence number
    ಪೂರ್ಣಾಂಕ		si_cchar;		// substitution character for conceal
#ಆದರೆ_ಅಂತ್ಯ
    ಗಿಡ್ಡ	*si_cont_list;		// list of contained groups
    ಗಿಡ್ಡ	*si_ಮುಂದಿನ_list;		// nextgroup IDs after this item ends
    reg_extmatch_T *si_extmatch;	// \z(...\) matches from start
					//ನಮೂನೆ
} stateitem_T;

#ವ್ಯಾಖ್ಯಾನಿಸು KEYWORD_IDX	(-1)	    // value of si_idx for keywords
#ವ್ಯಾಖ್ಯಾನಿಸು ID_LIST_ALL	((ಗಿಡ್ಡ *)-1) // valid of si_cont_list for containing all
				    // but contained groups

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಮುಂದಿನ_seqnr = 1;		// value to use for si_seqnr
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Struct to reduce the number of arguments to get_syn_options(), it's used
 * very often.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ		flags;		// flags for contained and transparent
    ಪೂರ್ಣಾಂಕ		keyword;	// TRUE for ":syn keyword"
    ಪೂರ್ಣಾಂಕ		*sync_idx;	// syntax item for "grouphere" argument, NULL
				// if not allowed
    ಅಕ್ಷರ	has_cont_list;	// TRUE if "cont_list" can be used
    ಗಿಡ್ಡ	*cont_list;	// group IDs for "contains" argument
    ಗಿಡ್ಡ	*cont_ಒಳ_list;	// group IDs for "containedin" argument
    ಗಿಡ್ಡ	*ಮುಂದಿನ_list;	// group IDs for "nextgroup" argument
} syn_opt_arg_T;

/*
 * The next possible match in the current line for any pattern is remembered,
 * to avoid having to try for a match in each column.
 * If next_match_idx == -1, not tried (in this line) yet.
 * If next_match_col == MAXCOL, no match found in this line.
 * (All end positions have the column of the char after the end)
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಮುಂದಿನ_ಹೊಂದಿಕೆ_ಲಂಬಸಾಲು;		// column for start of next match
ಸ್ಥಿರ lpos_T ಮುಂದಿನ_ಹೊಂದಿಕೆ_m_endpos;	// position for end of next match
ಸ್ಥಿರ lpos_T ಮುಂದಿನ_ಹೊಂದಿಕೆ_h_startpos;	// pos. for highl. start of next match
ಸ್ಥಿರ lpos_T ಮುಂದಿನ_ಹೊಂದಿಕೆ_h_endpos;	// pos. for highl. end of next match
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಮುಂದಿನ_ಹೊಂದಿಕೆ_idx;		// index of matched item
ಸ್ಥಿರ ದೀರ್ಘ ಮುಂದಿನ_ಹೊಂದಿಕೆ_flags;		// flags for next match
ಸ್ಥಿರ lpos_T ಮುಂದಿನ_ಹೊಂದಿಕೆ_eos_pos;	// end of start pattn (start region)
ಸ್ಥಿರ lpos_T ಮುಂದಿನ_ಹೊಂದಿಕೆ_eoe_pos;	// pos. for end of end pattern
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಮುಂದಿನ_ಹೊಂದಿಕೆ_ಕೊನೆ_idx;		// ID of group for end pattn or zero
ಸ್ಥಿರ reg_extmatch_T *ಮುಂದಿನ_ಹೊಂದಿಕೆ_extmatch = ಖಾಲಿ;

/*
 * A state stack is an array of integers or stateitem_T, stored in a
 * garray_T.  A state stack is invalid if its itemsize entry is zero.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು INVALID_STATE(ssp)  ((ssp)->ga_itemsize == 0)
#ವ್ಯಾಖ್ಯಾನಿಸು VALID_STATE(ssp)    ((ssp)->ga_itemsize != 0)

#ವ್ಯಾಖ್ಯಾನಿಸು FOR_ALL_SYNSTATES(sb, sst) \
    ಗಾಗಿ ((sst) = (sb)->b_sst_ಮೊದಲ; (sst) != ಖಾಲಿ; (sst) = (sst)->sst_ಮುಂದಿನ)

/*
 * The current state (within the line) of the recognition engine.
 * When current_state.ga_itemsize is 0 the current state is invalid.
 */
ಸ್ಥಿರ win_T	*syn_win;		// current window for highlighting
ಸ್ಥಿರ buf_T	*syn_buf;		// current buffer for highlighting
ಸ್ಥಿರ synblock_T *syn_ಖಂಡ;		// current buffer for highlighting
ಸ್ಥಿರ linenr_T current_lnum = 0;	// lnum of current state
ಸ್ಥಿರ colnr_T	current_ಲಂಬಸಾಲು = 0;	// column of current state
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	current_state_stored = 0; // TRUE if stored current state
					  // after setting current_finished
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	current_ಪೂರ್ಣಗೊಂಡ = 0;	// current line has been finished
ಸ್ಥಿರ garray_T current_state		// current stack of state_items
		= {0, 0, 0, 0, ಖಾಲಿ};
ಸ್ಥಿರ ಗಿಡ್ಡ	*current_ಮುಂದಿನ_list = ಖಾಲಿ; // when non-zero, nextgroup list
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	current_ಮುಂದಿನ_flags = 0; // flags for current_next_list
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	current_ಸಾಲು_id = 0;	// unique number for current line

#ವ್ಯಾಖ್ಯಾನಿಸು CUR_STATE(idx)	((stateitem_T *)(current_state.ga_ದತ್ತಾಂಶ))[idx]

ಸ್ಥಿರ ಶೂನ್ಯ syn_sync(win_T *wp, linenr_T lnum, synstate_T *ಕೊನೆಯ_valid);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ syn_ಹೊಂದಿಕೆ_linecont(linenr_T lnum);
ಸ್ಥಿರ ಶೂನ್ಯ syn_ಆರಂಭ_ಸಾಲು(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ syn_update_ತುದಿಗಳು(ಪೂರ್ಣಾಂಕ startofline);
ಸ್ಥಿರ ಶೂನ್ಯ syn_ರಾಶಿ_alloc(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ syn_ರಾಶಿ_cleanup(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ syn_ರಾಶಿ_free_ನಮೂದು(synblock_T *ಖಂಡ, synstate_T *p);
ಸ್ಥಿರ synstate_T *syn_ರಾಶಿ_ಕಂಡುಹಿಡಿ_ನಮೂದು(linenr_T lnum);
ಸ್ಥಿರ synstate_T *store_current_state(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ತುಂಬು_current_state(synstate_T *from);
ಸ್ಥಿರ ಶೂನ್ಯ invalidate_current_state(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ syn_ರಾಶಿ_equal(synstate_T *sp);
ಸ್ಥಿರ ಶೂನ್ಯ validate_current_state(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ syn_ಮುಕ್ತಾಯ_ಸಾಲು(ಪೂರ್ಣಾಂಕ syncing);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ syn_current_attr(ಪೂರ್ಣಾಂಕ syncing, ಪೂರ್ಣಾಂಕ displaying, ಪೂರ್ಣಾಂಕ *can_spell, ಪೂರ್ಣಾಂಕ keep_state);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ did_ಹೊಂದಿಕೆ_already(ಪೂರ್ಣಾಂಕ idx, garray_T *gap);
ಸ್ಥಿರ stateitem_T *ತಳ್ಳು_ಮುಂದಿನ_ಹೊಂದಿಕೆ(stateitem_T *cur_si);
ಸ್ಥಿರ ಶೂನ್ಯ check_state_ತುದಿಗಳು(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ update_si_attr(ಪೂರ್ಣಾಂಕ idx);
ಸ್ಥಿರ ಶೂನ್ಯ check_keepend(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ update_si_ಕೊನೆ(stateitem_T *sip, ಪೂರ್ಣಾಂಕ startcol, ಪೂರ್ಣಾಂಕ force);
ಸ್ಥಿರ ಗಿಡ್ಡ *copy_id_list(ಗಿಡ್ಡ *list);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಒಳ_id_list(stateitem_T *ಅಂಶ, ಗಿಡ್ಡ *cont_list, ರಚನೆ sp_syn *ssp, ಪೂರ್ಣಾಂಕ flags);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ತಳ್ಳು_current_state(ಪೂರ್ಣಾಂಕ idx);
ಸ್ಥಿರ ಶೂನ್ಯ ತೆಗೆ_current_state(ಶೂನ್ಯ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
ಸ್ಥಿರ ಶೂನ್ಯ syn_clear_ಸಮಯ(syn_ಸಮಯ_T *tt);
ಸ್ಥಿರ ಶೂನ್ಯ syntime_clear(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ syntime_report(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ syn_ಸಮಯ_on = FALSE;
# define IF_SYN_ಸಮಯ(p) (p)
#ಇಲ್ಲದಿದ್ದರೆ
# define IF_SYN_ಸಮಯ(p) ಖಾಲಿ
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ syn_ಸಮಯ_T;
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ syn_ರಾಶಿ_apply_changes_ಖಂಡ(synblock_T *ಖಂಡ, buf_T *buf);
ಸ್ಥಿರ ಶೂನ್ಯ ಕಂಡುಹಿಡಿ_endpos(ಪೂರ್ಣಾಂಕ idx, lpos_T *startpos, lpos_T *m_endpos, lpos_T *hl_endpos, ದೀರ್ಘ *flagsp, lpos_T *ಕೊನೆ_endpos, ಪೂರ್ಣಾಂಕ *ಕೊನೆ_idx, reg_extmatch_T *ಆರಂಭ_ext);

ಸ್ಥಿರ ಶೂನ್ಯ ಎಲ್ಲೆ_pos(lpos_T *pos, lpos_T *ಎಲ್ಲೆ);
ಸ್ಥಿರ ಶೂನ್ಯ ಎಲ್ಲೆ_pos_zero(lpos_T *pos, lpos_T *ಎಲ್ಲೆ);
ಸ್ಥಿರ ಶೂನ್ಯ syn_ಕೂಡಿಸು_ಕೊನೆ_off(lpos_T *ಫಲಿತಾಂಶ, regmmatch_T *regmatch, synpat_T *spp, ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ extra);
ಸ್ಥಿರ ಶೂನ್ಯ syn_ಕೂಡಿಸು_ಆರಂಭ_off(lpos_T *ಫಲಿತಾಂಶ, regmmatch_T *regmatch, synpat_T *spp, ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ extra);
ಸ್ಥಿರ char_u *syn_getcurline(ಶೂನ್ಯ);
ಸ್ಥಿರ colnr_T syn_getcurline_len(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ syn_regexec(regmmatch_T *rmp, linenr_T lnum, colnr_T ಲಂಬಸಾಲು, syn_ಸಮಯ_T *st);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ check_keyword_id(char_u *ಸಾಲು, ಪೂರ್ಣಾಂಕ startcol, ಪೂರ್ಣಾಂಕ *endcol, ದೀರ್ಘ *flags, ಗಿಡ್ಡ **ಮುಂದಿನ_list, stateitem_T *cur_si, ಪೂರ್ಣಾಂಕ *ccharp);
ಸ್ಥಿರ ಶೂನ್ಯ syn_remove_ನಮೂನೆ(synblock_T *ಖಂಡ, ಪೂರ್ಣಾಂಕ idx);
ಸ್ಥಿರ ಶೂನ್ಯ syn_clear_ನಮೂನೆ(synblock_T *ಖಂಡ, ಪೂರ್ಣಾಂಕ i);
ಸ್ಥಿರ ಶೂನ್ಯ syn_clear_cluster(synblock_T *ಖಂಡ, ಪೂರ್ಣಾಂಕ i);
ಸ್ಥಿರ ಶೂನ್ಯ syn_clear_one(ಪೂರ್ಣಾಂಕ id, ಪೂರ್ಣಾಂಕ syncing);
ಸ್ಥಿರ ಶೂನ್ಯ syn_cmd_onoff(exarg_T *eap, ಅಕ್ಷರ *name);
ಸ್ಥಿರ ಶೂನ್ಯ syn_ಸಾಲುಗಳು_ಸಂದೇಶ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ syn_ಹೊಂದಿಕೆ_ಸಂದೇಶ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ syn_list_one(ಪೂರ್ಣಾಂಕ id, ಪೂರ್ಣಾಂಕ syncing, ಪೂರ್ಣಾಂಕ link_only);
ಸ್ಥಿರ ಶೂನ್ಯ syn_list_cluster(ಪೂರ್ಣಾಂಕ id);
ಸ್ಥಿರ ಶೂನ್ಯ put_id_list(char_u *name, ಗಿಡ್ಡ *list, ಪೂರ್ಣಾಂಕ attr);
ಸ್ಥಿರ ಶೂನ್ಯ put_ನಮೂನೆ(ಅಕ್ಷರ *s, ಪೂರ್ಣಾಂಕ c, synpat_T *spp, ಪೂರ್ಣಾಂಕ attr);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ syn_list_keywords(ಪೂರ್ಣಾಂಕ id, hashtab_T *ht, ಪೂರ್ಣಾಂಕ did_ಶೀರ್ಷಿಕೆ, ಪೂರ್ಣಾಂಕ attr);
ಸ್ಥಿರ ಶೂನ್ಯ syn_clear_keyword(ಪೂರ್ಣಾಂಕ id, hashtab_T *ht);
ಸ್ಥಿರ ಶೂನ್ಯ clear_keywtab(hashtab_T *ht);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ syn_scl_namen2id(char_u *linep, ಪೂರ್ಣಾಂಕ len);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ syn_check_cluster(char_u *pp, ಪೂರ್ಣಾಂಕ len);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ syn_ಕೂಡಿಸು_cluster(char_u *name);
ಸ್ಥಿರ ಶೂನ್ಯ init_syn_patterns(ಶೂನ್ಯ);
ಸ್ಥಿರ char_u *get_syn_ನಮೂನೆ(char_u *arg, synpat_T *ci);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ get_id_list(char_u **arg, ಪೂರ್ಣಾಂಕ keylen, ಗಿಡ್ಡ **list, ಪೂರ್ಣಾಂಕ skip);
ಸ್ಥಿರ ಶೂನ್ಯ syn_combine_list(ಗಿಡ್ಡ **clstr1, ಗಿಡ್ಡ **clstr2, ಪೂರ್ಣಾಂಕ list_op);
ಸ್ಥಿರ ಶೂನ್ಯ idl_cache_clear(synblock_T *ಖಂಡ);

/*
 * Start the syntax recognition for a line.  This function is normally called
 * from the screen updating, once for each displayed line.
 * The buffer is remembered in syn_buf, because get_syntax_attr() doesn't get
 * it.	Careful: curbuf and curwin are likely to point to another buffer and
 * window.
 */
    ಶೂನ್ಯ
syntax_ಆರಂಭ(win_T *wp, linenr_T lnum)
{
    synstate_T	*p;
    synstate_T	*ಕೊನೆಯ_valid = ಖಾಲಿ;
    synstate_T	*ಕೊನೆಯ_min_valid = ಖಾಲಿ;
    synstate_T	*sp, *prev = ಖಾಲಿ;
    linenr_T	parsed_lnum;
    linenr_T	ಮೊದಲ_stored;
    ಪೂರ್ಣಾಂಕ		dist;
    ಸ್ಥಿರ varnumber_T changedtick = 0;	// remember the last change ID

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    current_sub_char = NUL;
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * After switching buffers, invalidate current_state.
     * Also do this when a change was made, the current state may be invalid
     * then.
     */
    ಆದರೆ (syn_ಖಂಡ != wp->w_s
	    || syn_buf != wp->w_ಬಫರ್
	    || changedtick != CHANGEDTICK(syn_buf))
    {
	invalidate_current_state();
	syn_buf = wp->w_ಬಫರ್;
	syn_ಖಂಡ = wp->w_s;
    }
    changedtick = CHANGEDTICK(syn_buf);
    syn_win = wp;

    /*
     * Allocate syntax stack when needed.
     */
    syn_ರಾಶಿ_alloc();
    ಆದರೆ (syn_ಖಂಡ->b_sst_array == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;		// out of memory
    syn_ಖಂಡ->b_sst_lasttick = display_tick;

    /*
     * If the state of the end of the previous line is useful, store it.
     */
    ಆದರೆ (VALID_STATE(&current_state)
	    && current_lnum < lnum
	    && current_lnum < syn_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	(ಶೂನ್ಯ)syn_ಮುಕ್ತಾಯ_ಸಾಲು(FALSE);
	ಆದರೆ (!current_state_stored)
	{
	    ++current_lnum;
	    (ಶೂನ್ಯ)store_current_state();
	}

	/*
	 * If the current_lnum is now the same as "lnum", keep the current
	 * state (this happens very often!).  Otherwise invalidate
	 * current_state and figure it out below.
	 */
	ಆದರೆ (current_lnum != lnum)
	    invalidate_current_state();
    }
    ಇಲ್ಲದಿದ್ದರೆ
	invalidate_current_state();

    /*
     * Try to synchronize from a saved state in b_sst_array[].
     * Only do this if lnum is not before and not to far beyond a saved state.
     */
    ಆದರೆ (INVALID_STATE(&current_state) && syn_ಖಂಡ->b_sst_array != ಖಾಲಿ)
    {
	// Find last valid saved state before start_lnum.
	FOR_ALL_SYNSTATES(syn_ಖಂಡ, p)
	{
	    ಆದರೆ (p->sst_lnum > lnum)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (p->sst_lnum <= lnum && p->sst_change_lnum == 0)
	    {
		ಕೊನೆಯ_valid = p;
		ಆದರೆ (p->sst_lnum >= lnum - syn_ಖಂಡ->b_syn_sync_minlines)
		    ಕೊನೆಯ_min_valid = p;
	    }
	}
	ಆದರೆ (ಕೊನೆಯ_min_valid != ಖಾಲಿ)
	    ತುಂಬು_current_state(ಕೊನೆಯ_min_valid);
    }

    /*
     * If "lnum" is before or far beyond a line with a saved state, need to
     * re-synchronize.
     */
    ಆದರೆ (INVALID_STATE(&current_state))
    {
	syn_sync(wp, lnum, ಕೊನೆಯ_valid);
	ಆದರೆ (current_lnum == 1)
	    // First line is always valid, no matter "minlines".
	    ಮೊದಲ_stored = 1;
	ಇಲ್ಲದಿದ್ದರೆ
	    // Need to parse "minlines" lines before state can be considered
	    // valid to store.
	    ಮೊದಲ_stored = current_lnum + syn_ಖಂಡ->b_syn_sync_minlines;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಮೊದಲ_stored = current_lnum;

    /*
     * Advance from the sync point or saved state until the current line.
     * Save some entries for syncing with later on.
     */
    ಆದರೆ (syn_ಖಂಡ->b_sst_len <= ಅಡ್ಡಸಾಲುಗಳು)
	dist = 999999;
    ಇಲ್ಲದಿದ್ದರೆ
	dist = syn_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ / (syn_ಖಂಡ->b_sst_len - ಅಡ್ಡಸಾಲುಗಳು) + 1;
    ಆಗ (current_lnum < lnum)
    {
	syn_ಆರಂಭ_ಸಾಲು();
	(ಶೂನ್ಯ)syn_ಮುಕ್ತಾಯ_ಸಾಲು(FALSE);
	++current_lnum;

	// If we parsed at least "minlines" lines or started at a valid
	// state, the current state is considered valid.
	ಆದರೆ (current_lnum >= ಮೊದಲ_stored)
	{
	    // Check if the saved state entry is for the current line and is
	    // equal to the current state.  If so, then validate all saved
	    // states that depended on a change before the parsed line.
	    ಆದರೆ (prev == ಖಾಲಿ)
		prev = syn_ರಾಶಿ_ಕಂಡುಹಿಡಿ_ನಮೂದು(current_lnum - 1);
	    ಆದರೆ (prev == ಖಾಲಿ)
		sp = syn_ಖಂಡ->b_sst_ಮೊದಲ;
	    ಇಲ್ಲದಿದ್ದರೆ
		sp = prev;
	    ಆಗ (sp != ಖಾಲಿ && sp->sst_lnum < current_lnum)
		sp = sp->sst_ಮುಂದಿನ;
	    ಆದರೆ (sp != ಖಾಲಿ
		    && sp->sst_lnum == current_lnum
		    && syn_ರಾಶಿ_equal(sp))
	    {
		parsed_lnum = current_lnum;
		prev = sp;
		ಆಗ (sp != ಖಾಲಿ && sp->sst_change_lnum <= parsed_lnum)
		{
		    ಆದರೆ (sp->sst_lnum <= lnum)
			// valid state before desired line, use this one
			prev = sp;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sp->sst_change_lnum == 0)
			// past saved states depending on change, break here.
			ನಿಲ್ಲಿಸು;
		    sp->sst_change_lnum = 0;
		    sp = sp->sst_ಮುಂದಿನ;
		}
		ತುಂಬು_current_state(prev);
	    }
	    // Store the state at this line when it's the first one, the line
	    // where we start parsing, or some distance from the previously
	    // saved state.  But only when parsed at least 'minlines'.
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (prev == ಖಾಲಿ
			|| current_lnum == lnum
			|| current_lnum >= prev->sst_lnum + dist)
		prev = store_current_state();
	}

	// This can take a long time: break when CTRL-C pressed.  The current
	// state will be wrong then.
	ಸಾಲು_breakcheck();
	ಆದರೆ (got_int)
	{
	    current_lnum = lnum;
	    ನಿಲ್ಲಿಸು;
	}
    }

    syn_ಆರಂಭ_ಸಾಲು();
}

/*
 * We cannot simply discard growarrays full of state_items or buf_states; we
 * have to manually release their extmatch pointers first.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clear_syn_state(synstate_T *p)
{
    ಪೂರ್ಣಾಂಕ		i;
    garray_T	*gap;

    ಆದರೆ (p->sst_stacksize > SST_FIX_ಸ್ಥಿತಿಗಳು)
    {
	gap = &(p->sst_union.sst_ga);
	ಗಾಗಿ (i = 0; i < gap->ga_len; i++)
	    unref_extmatch(SYN_STATE_P(gap)[i].bs_extmatch);
	ga_clear(gap);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಾಗಿ (i = 0; i < p->sst_stacksize; i++)
	    unref_extmatch(p->sst_union.sst_ರಾಶಿ[i].bs_extmatch);
    }
}

/*
 * Cleanup the current_state stack.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clear_current_state(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;
    stateitem_T	*sip;

    sip = (stateitem_T *)(current_state.ga_ದತ್ತಾಂಶ);
    ಗಾಗಿ (i = 0; i < current_state.ga_len; i++)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_sync(
    win_T	*wp,
    linenr_T	ಆರಂಭ_lnum,
    synstate_T	*ಕೊನೆಯ_valid)
{
    buf_T	*curbuf_save;
    win_T	*curwin_save;
    pos_T	cursor_save;
    ಪೂರ್ಣಾಂಕ		idx;
    linenr_T	lnum;
    linenr_T	ಕೊನೆ_lnum;
    linenr_T	break_lnum;
    ಪೂರ್ಣಾಂಕ		had_sync_ಬಿಂದು;
    stateitem_T	*cur_si;
    synpat_T	*spp;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು_flags = 0;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು_ಹೊಂದಿಕೆ_idx = 0;
    linenr_T	ಸಿಕ್ಕಿತು_current_lnum = 0;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು_current_ಲಂಬಸಾಲು = 0;
    lpos_T	ಸಿಕ್ಕಿತು_m_endpos;
    colnr_T	prev_current_ಲಂಬಸಾಲು;

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
    ಆದರೆ (syn_ಖಂಡ->b_syn_sync_minlines > ಆರಂಭ_lnum)
	ಆರಂಭ_lnum = 1;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (syn_ಖಂಡ->b_syn_sync_minlines == 1)
	    lnum = 1;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (syn_ಖಂಡ->b_syn_sync_minlines < 10)
	    lnum = syn_ಖಂಡ->b_syn_sync_minlines * 2;
	ಇಲ್ಲದಿದ್ದರೆ
	    lnum = syn_ಖಂಡ->b_syn_sync_minlines * 3 / 2;
	ಆದರೆ (syn_ಖಂಡ->b_syn_sync_maxlines != 0
				     && lnum > syn_ಖಂಡ->b_syn_sync_maxlines)
	    lnum = syn_ಖಂಡ->b_syn_sync_maxlines;
	ಆದರೆ (lnum >= ಆರಂಭ_lnum)
	    ಆರಂಭ_lnum = 1;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಆರಂಭ_lnum -= lnum;
    }
    current_lnum = ಆರಂಭ_lnum;

    /*
     * 1. Search backwards for the end of a C-style comment.
     */
    ಆದರೆ (syn_ಖಂಡ->b_syn_sync_flags & SF_CCOMMENT)
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
	ಗಾಗಿ ( ; ಆರಂಭ_lnum > 1; --ಆರಂಭ_lnum)
	{
	    char_u	*l = ml_get(ಆರಂಭ_lnum - 1);

	    ಆದರೆ (*l == NUL || *(l + ml_get_len(ಆರಂಭ_lnum - 1) - 1) != '\\')
		ನಿಲ್ಲಿಸು;
	}
	current_lnum = ಆರಂಭ_lnum;

	// set cursor to start of search
	cursor_save = wp->w_cursor;
	wp->w_cursor.lnum = ಆರಂಭ_lnum;
	wp->w_cursor.ಲಂಬಸಾಲು = 0;

	/*
	 * If the line is inside a comment, need to find the syntax item that
	 * defines the comment.
	 * Restrict the search for the end of a comment to b_syn_sync_maxlines.
	 */
	ಆದರೆ (ಕಂಡುಹಿಡಿ_ಆರಂಭ_comment((ಪೂರ್ಣಾಂಕ)syn_ಖಂಡ->b_syn_sync_maxlines) != ಖಾಲಿ)
	{
	    ಗಾಗಿ (idx = syn_ಖಂಡ->b_syn_patterns.ga_len; --idx >= 0; )
		ಆದರೆ (SYN_ವಸ್ತುಗಳು(syn_ಖಂಡ)[idx].sp_syn.id
						   == syn_ಖಂಡ->b_syn_sync_id
			&& SYN_ವಸ್ತುಗಳು(syn_ಖಂಡ)[idx].sp_type == SPTYPE_ಆರಂಭ)
		{
		    validate_current_state();
		    ಆದರೆ (ತಳ್ಳು_current_state(idx) == OK)
			update_si_attr(current_state.ga_len - 1);
		    ನಿಲ್ಲಿಸು;
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
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (syn_ಖಂಡ->b_syn_sync_flags & SF_ಹೊಂದಿಕೆ)
    {
	ಆದರೆ (syn_ಖಂಡ->b_syn_sync_maxlines != 0
			       && ಆರಂಭ_lnum > syn_ಖಂಡ->b_syn_sync_maxlines)
	    break_lnum = ಆರಂಭ_lnum - syn_ಖಂಡ->b_syn_sync_maxlines;
	ಇಲ್ಲದಿದ್ದರೆ
	    break_lnum = 0;

	ಸಿಕ್ಕಿತು_m_endpos.lnum = 0;
	ಸಿಕ್ಕಿತು_m_endpos.ಲಂಬಸಾಲು = 0;
	ಕೊನೆ_lnum = ಆರಂಭ_lnum;
	lnum = ಆರಂಭ_lnum;
	ಆಗ (--lnum > break_lnum)
	{
	    // This can take a long time: break when CTRL-C pressed.
	    ಸಾಲು_breakcheck();
	    ಆದರೆ (got_int)
	    {
		invalidate_current_state();
		current_lnum = ಆರಂಭ_lnum;
		ನಿಲ್ಲಿಸು;
	    }

	    // Check if we have run into a valid saved state stack now.
	    ಆದರೆ (ಕೊನೆಯ_valid != ಖಾಲಿ && lnum == ಕೊನೆಯ_valid->sst_lnum)
	    {
		ತುಂಬು_current_state(ಕೊನೆಯ_valid);
		ನಿಲ್ಲಿಸು;
	    }

	    /*
	     * Check if the previous line has the line-continuation pattern.
	     */
	    ಆದರೆ (lnum > 1 && syn_ಹೊಂದಿಕೆ_linecont(lnum - 1))
		ಮುಂದುವರಿ;

	    /*
	     * Start with nothing on the state stack
	     */
	    validate_current_state();

	    ಗಾಗಿ (current_lnum = lnum; current_lnum < ಕೊನೆ_lnum; ++current_lnum)
	    {
		syn_ಆರಂಭ_ಸಾಲು();
		ಗಾಗಿ (;;)
		{
		    had_sync_ಬಿಂದು = syn_ಮುಕ್ತಾಯ_ಸಾಲು(TRUE);
		    /*
		     * When a sync point has been found, remember where, and
		     * continue to look for another one, further on in the line.
		     */
		    ಆದರೆ (had_sync_ಬಿಂದು && current_state.ga_len)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			ಆದರೆ (cur_si->si_m_endpos.lnum > ಆರಂಭ_lnum)
			{
			    // ignore match that goes to after where started
			    current_lnum = ಕೊನೆ_lnum;
			    ನಿಲ್ಲಿಸು;
			}
			ಆದರೆ (cur_si->si_idx < 0)
			{
			    // Cannot happen?
			    ಸಿಕ್ಕಿತು_flags = 0;
			    ಸಿಕ್ಕಿತು_ಹೊಂದಿಕೆ_idx = KEYWORD_IDX;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    spp = &(SYN_ವಸ್ತುಗಳು(syn_ಖಂಡ)[cur_si->si_idx]);
			    ಸಿಕ್ಕಿತು_flags = spp->sp_flags;
			    ಸಿಕ್ಕಿತು_ಹೊಂದಿಕೆ_idx = spp->sp_sync_idx;
			}
			ಸಿಕ್ಕಿತು_current_lnum = current_lnum;
			ಸಿಕ್ಕಿತು_current_ಲಂಬಸಾಲು = current_ಲಂಬಸಾಲು;
			ಸಿಕ್ಕಿತು_m_endpos = cur_si->si_m_endpos;
			/*
			 * Continue after the match (be aware of a zero-length
			 * match).
			 */
			ಆದರೆ (ಸಿಕ್ಕಿತು_m_endpos.lnum > current_lnum)
			{
			    current_lnum = ಸಿಕ್ಕಿತು_m_endpos.lnum;
			    current_ಲಂಬಸಾಲು = ಸಿಕ್ಕಿತು_m_endpos.ಲಂಬಸಾಲು;
			    ಆದರೆ (current_lnum >= ಕೊನೆ_lnum)
				ನಿಲ್ಲಿಸು;
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸಿಕ್ಕಿತು_m_endpos.ಲಂಬಸಾಲು > current_ಲಂಬಸಾಲು)
			    current_ಲಂಬಸಾಲು = ಸಿಕ್ಕಿತು_m_endpos.ಲಂಬಸಾಲು;
			ಇಲ್ಲದಿದ್ದರೆ
			    ++current_ಲಂಬಸಾಲು;

			// syn_current_attr() will have skipped the check for
			// an item that ends here, need to do that now.  Be
			// careful not to go past the NUL.
			prev_current_ಲಂಬಸಾಲು = current_ಲಂಬಸಾಲು;
			ಆದರೆ (syn_getcurline()[current_ಲಂಬಸಾಲು] != NUL)
			    ++current_ಲಂಬಸಾಲು;
			check_state_ತುದಿಗಳು();
			current_ಲಂಬಸಾಲು = prev_current_ಲಂಬಸಾಲು;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			ನಿಲ್ಲಿಸು;
		}
	    }

	    /*
	     * If a sync point was encountered, break here.
	     */
	    ಆದರೆ (ಸಿಕ್ಕಿತು_flags)
	    {
		/*
		 * Put the item that was specified by the sync point on the
		 * state stack.  If there was no item specified, make the
		 * state stack empty.
		 */
		clear_current_state();
		ಆದರೆ (ಸಿಕ್ಕಿತು_ಹೊಂದಿಕೆ_idx >= 0
			&& ತಳ್ಳು_current_state(ಸಿಕ್ಕಿತು_ಹೊಂದಿಕೆ_idx) == OK)
		    update_si_attr(current_state.ga_len - 1);

		/*
		 * When using "grouphere", continue from the sync point
		 * match, until the end of the line.  Parsing starts at
		 * the next line.
		 * For "groupthere" the parsing starts at start_lnum.
		 */
		ಆದರೆ (ಸಿಕ್ಕಿತು_flags & HL_SYNC_ಇಲ್ಲಿ)
		{
		    current_lnum = ಸಿಕ್ಕಿತು_m_endpos.lnum;
		    current_ಲಂಬಸಾಲು = ಸಿಕ್ಕಿತು_m_endpos.ಲಂಬಸಾಲು;
		    ಆದರೆ (current_state.ga_len)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			cur_si->si_h_startpos.lnum = ಸಿಕ್ಕಿತು_current_lnum;
			cur_si->si_h_startpos.ಲಂಬಸಾಲು = ಸಿಕ್ಕಿತು_current_ಲಂಬಸಾಲು;
			update_si_ಕೊನೆ(cur_si, (ಪೂರ್ಣಾಂಕ)current_ಲಂಬಸಾಲು, TRUE);
			check_keepend();
		    }
		    (ಶೂನ್ಯ)syn_ಮುಕ್ತಾಯ_ಸಾಲು(FALSE);
		    ++current_lnum;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    current_lnum = ಆರಂಭ_lnum;

		ನಿಲ್ಲಿಸು;
	    }

	    ಕೊನೆ_lnum = lnum;
	    invalidate_current_state();
	}

	// Ran into start of the file or exceeded maximum number of lines
	ಆದರೆ (lnum <= break_lnum)
	{
	    invalidate_current_state();
	    current_lnum = break_lnum + 1;
	}
    }

    validate_current_state();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
save_chartab(char_u *chartab)
{
    ಆದರೆ (syn_ಖಂಡ->b_syn_isk == ಖಾಲಿಗಳು_option)
	ಹಿಂತಿರುಗಿ;

    mch_memmove(chartab, syn_buf->b_chartab, (size_t)32);
    mch_memmove(syn_buf->b_chartab, syn_win->w_s->b_syn_chartab, (size_t)32);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
restore_chartab(char_u *chartab)
{
    ಆದರೆ (syn_win->w_s->b_syn_isk != ಖಾಲಿಗಳು_option)
	mch_memmove(syn_buf->b_chartab, chartab, (size_t)32);
}

/*
 * Return TRUE if the line-continuation pattern matches in line "lnum".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
syn_ಹೊಂದಿಕೆ_linecont(linenr_T lnum)
{
    regmmatch_T regmatch;
    ಪೂರ್ಣಾಂಕ r;
    char_u	buf_chartab[32];  // chartab array for syn iskyeyword

    ಆದರೆ (syn_ಖಂಡ->b_syn_linecont_prog == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    // use syntax iskeyword option
    save_chartab(buf_chartab);
    regmatch.rmm_ic = syn_ಖಂಡ->b_syn_linecont_ic;
    regmatch.regprog = syn_ಖಂಡ->b_syn_linecont_prog;
    r = syn_regexec(&regmatch, lnum, (colnr_T)0,
	    IF_SYN_ಸಮಯ(&syn_ಖಂಡ->b_syn_linecont_ಸಮಯ));
    syn_ಖಂಡ->b_syn_linecont_prog = regmatch.regprog;
    restore_chartab(buf_chartab);
    ಹಿಂತಿರುಗಿ r;
}

/*
 * Prepare the current state for the start of a line.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_ಆರಂಭ_ಸಾಲು(ಶೂನ್ಯ)
{
    current_ಪೂರ್ಣಗೊಂಡ = FALSE;
    current_ಲಂಬಸಾಲು = 0;

    /*
     * Need to update the end of a start/skip/end that continues from the
     * previous line and regions that have "keepend".
     */
    ಆದರೆ (current_state.ga_len > 0)
    {
	syn_update_ತುದಿಗಳು(TRUE);
	check_state_ತುದಿಗಳು();
    }

    ಮುಂದಿನ_ಹೊಂದಿಕೆ_idx = -1;
    ++current_ಸಾಲು_id;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    ಮುಂದಿನ_seqnr = 1;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Check for items in the stack that need their end updated.
 * When "startofline" is TRUE the last item is always updated.
 * When "startofline" is FALSE the item with "keepend" is forcefully updated.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_update_ತುದಿಗಳು(ಪೂರ್ಣಾಂಕ startofline)
{
    stateitem_T	*cur_si;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ನೋಡಿದ_keepend;

    ಆದರೆ (startofline)
    {
	// Check for a match carried over from a previous line with a
	// contained region.  The match ends as soon as the region ends.
	ಗಾಗಿ (i = 0; i < current_state.ga_len; ++i)
	{
	    cur_si = &CUR_STATE(i);
	    ಆದರೆ (cur_si->si_idx >= 0
		    && (SYN_ವಸ್ತುಗಳು(syn_ಖಂಡ)[cur_si->si_idx]).sp_type
							       == SPTYPE_ಹೊಂದಿಕೆ
		    && cur_si->si_m_endpos.lnum < current_lnum)
	    {
		cur_si->si_flags |= HL_MATCHCONT;
		cur_si->si_m_endpos.lnum = 0;
		cur_si->si_m_endpos.ಲಂಬಸಾಲು = 0;
		cur_si->si_h_endpos = cur_si->si_m_endpos;
		cur_si->si_ತುದಿಗಳು = TRUE;
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
    ಆದರೆ (keepend_level >= 0)
	ಗಾಗಿ ( ; i > keepend_level; --i)
	    ಆದರೆ (CUR_STATE(i).si_flags & HL_EXTEND)
		ನಿಲ್ಲಿಸು;

    ನೋಡಿದ_keepend = FALSE;
    ಗಾಗಿ ( ; i < current_state.ga_len; ++i)
    {
	cur_si = &CUR_STATE(i);
	ಆದರೆ ((cur_si->si_flags & HL_KEEPEND)
			    || (ನೋಡಿದ_keepend && !startofline)
			    || (i == current_state.ga_len - 1 && startofline))
	{
	    cur_si->si_h_startpos.ಲಂಬಸಾಲು = 0;	// start highl. in col 0
	    cur_si->si_h_startpos.lnum = current_lnum;

	    ಆದರೆ (!(cur_si->si_flags & HL_MATCHCONT))
		update_si_ಕೊನೆ(cur_si, (ಪೂರ್ಣಾಂಕ)current_ಲಂಬಸಾಲು, !startofline);

	    ಆದರೆ (!startofline && (cur_si->si_flags & HL_KEEPEND))
		ನೋಡಿದ_keepend = TRUE;
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

    ಸ್ಥಿರ ಶೂನ್ಯ
syn_ರಾಶಿ_free_ಖಂಡ(synblock_T *ಖಂಡ)
{
    synstate_T	*p;

    // Syntax definitions may have changed: drop the in_id_list() cache.
    idl_cache_clear(ಖಂಡ);

    ಆದರೆ (ಖಂಡ->b_sst_array == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    FOR_ALL_SYNSTATES(ಖಂಡ, p)
	clear_syn_state(p);
    VIM_CLEAR(ಖಂಡ->b_sst_array);
    ಖಂಡ->b_sst_ಮೊದಲ = ಖಾಲಿ;
    ಖಂಡ->b_sst_ಹುಡುಕು = ಖಾಲಿ;
    ಖಂಡ->b_sst_len = 0;
}
/*
 * Free b_sst_array[] for buffer "buf".
 * Used when syntax items changed to force resyncing everywhere.
 */
    ಶೂನ್ಯ
syn_ರಾಶಿ_free_all(synblock_T *ಖಂಡ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    win_T	*wp;
#ಆದರೆ_ಅಂತ್ಯ

    syn_ರಾಶಿ_free_ಖಂಡ(ಖಂಡ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    // When using "syntax" fold method, must update all folds.
    FOR_ALL_WINDOWS(wp)
    {
	ಆದರೆ (wp->w_s == ಖಂಡ && foldmethodIsSyntax(wp))
	    foldUpdateAll(wp);
    }
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Allocate the syntax state stack for syn_buf when needed.
 * If the number of entries in b_sst_array[] is much too big or a bit too
 * small, reallocate it.
 * Also used to allocate b_sst_array[] for the first time.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_ರಾಶಿ_alloc(ಶೂನ್ಯ)
{
    ದೀರ್ಘ	len;
    synstate_T	*ಗಮ್ಯ, *from;
    synstate_T	*sstp;

    len = syn_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ / SST_DIST + ಅಡ್ಡಸಾಲುಗಳು * 2;
    ಆದರೆ (len < SST_MIN_ENTRIES)
	len = SST_MIN_ENTRIES;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (len > SST_MAX_ENTRIES)
	len = SST_MAX_ENTRIES;
    ಆದರೆ (syn_ಖಂಡ->b_sst_len > len * 2 || syn_ಖಂಡ->b_sst_len < len)
    {
	// Allocate 50% too much, to avoid reallocating too often.
	len = syn_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	len = (len + len / 2) / SST_DIST + ಅಡ್ಡಸಾಲುಗಳು * 2;
	ಆದರೆ (len < SST_MIN_ENTRIES)
	    len = SST_MIN_ENTRIES;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (len > SST_MAX_ENTRIES)
	    len = SST_MAX_ENTRIES;

	ಆದರೆ (syn_ಖಂಡ->b_sst_array != ಖಾಲಿ)
	{
	    // When shrinking the array, cleanup the existing stack.
	    // Make sure that all valid entries fit in the new array.
	    ಆಗ (syn_ಖಂಡ->b_sst_len - syn_ಖಂಡ->b_sst_freecount + 2 > len
		    && syn_ರಾಶಿ_cleanup())
		;
	    ಆದರೆ (len < syn_ಖಂಡ->b_sst_len - syn_ಖಂಡ->b_sst_freecount + 2)
		len = syn_ಖಂಡ->b_sst_len - syn_ಖಂಡ->b_sst_freecount + 2;
	}

	sstp = ALLOC_CLEAR_MULT(synstate_T, len);
	ಆದರೆ (sstp == ಖಾಲಿ)	// out of memory!
	    ಹಿಂತಿರುಗಿ;

	ಗಮ್ಯ = sstp - 1;
	ಆದರೆ (syn_ಖಂಡ->b_sst_array != ಖಾಲಿ)
	{
	    // Move the states from the old array to the new one.
	    ಗಾಗಿ (from = syn_ಖಂಡ->b_sst_ಮೊದಲ; from != ಖಾಲಿ;
							from = from->sst_ಮುಂದಿನ)
	    {
		++ಗಮ್ಯ;
		*ಗಮ್ಯ = *from;
		ಗಮ್ಯ->sst_ಮುಂದಿನ = ಗಮ್ಯ + 1;
	    }
	}
	ಆದರೆ (ಗಮ್ಯ != sstp - 1)
	{
	    ಗಮ್ಯ->sst_ಮುಂದಿನ = ಖಾಲಿ;
	    syn_ಖಂಡ->b_sst_ಮೊದಲ = sstp;
	    syn_ಖಂಡ->b_sst_freecount = len - (ಪೂರ್ಣಾಂಕ)(ಗಮ್ಯ - sstp) - 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    syn_ಖಂಡ->b_sst_ಮೊದಲ = ಖಾಲಿ;
	    syn_ಖಂಡ->b_sst_freecount = len;
	}

	// Create the list of free entries.
	syn_ಖಂಡ->b_sst_firstfree = ಗಮ್ಯ + 1;
	ಆಗ (++ಗಮ್ಯ < sstp + len)
	    ಗಮ್ಯ->sst_ಮುಂದಿನ = ಗಮ್ಯ + 1;
	(sstp + len - 1)->sst_ಮುಂದಿನ = ಖಾಲಿ;

	vim_free(syn_ಖಂಡ->b_sst_array);
	syn_ಖಂಡ->b_sst_array = sstp;
	syn_ಖಂಡ->b_sst_len = len;
	// The entries were moved to a new array, drop the stale pointer.
	syn_ಖಂಡ->b_sst_ಹುಡುಕು = ಖಾಲಿ;
    }
}

/*
 * Check for changes in a buffer to affect stored syntax states.  Uses the
 * b_mod_* fields.
 * Called from update_screen(), before screen is being updated, once for each
 * displayed buffer.
 */
    ಶೂನ್ಯ
syn_ರಾಶಿ_apply_changes(buf_T *buf)
{
    win_T	*wp;

    syn_ರಾಶಿ_apply_changes_ಖಂಡ(&buf->b_s, buf);

    FOR_ALL_WINDOWS(wp)
    {
	ಆದರೆ ((wp->w_ಬಫರ್ == buf) && (wp->w_s != &buf->b_s))
	    syn_ರಾಶಿ_apply_changes_ಖಂಡ(wp->w_s, buf);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
syn_ರಾಶಿ_apply_changes_ಖಂಡ(synblock_T *ಖಂಡ, buf_T *buf)
{
    synstate_T	*p, *prev, *np;
    linenr_T	n;

    prev = ಖಾಲಿ;
    ಗಾಗಿ (p = ಖಂಡ->b_sst_ಮೊದಲ; p != ಖಾಲಿ; )
    {
	ಆದರೆ (p->sst_lnum + ಖಂಡ->b_syn_sync_linebreaks > buf->b_mod_ತುದಿ)
	{
	    n = p->sst_lnum + buf->b_mod_xlines;
	    ಆದರೆ (n <= buf->b_mod_bot)
	    {
		// this state is inside the changed area, remove it
		np = p->sst_ಮುಂದಿನ;
		ಆದರೆ (prev == ಖಾಲಿ)
		    ಖಂಡ->b_sst_ಮೊದಲ = np;
		ಇಲ್ಲದಿದ್ದರೆ
		    prev->sst_ಮುಂದಿನ = np;
		syn_ರಾಶಿ_free_ನಮೂದು(ಖಂಡ, p);
		p = np;
		ಮುಂದುವರಿ;
	    }
	    // This state is below the changed area.  Remember the line
	    // that needs to be parsed before this entry can be made valid
	    // again.
	    ಆದರೆ (p->sst_change_lnum != 0 && p->sst_change_lnum > buf->b_mod_ತುದಿ)
	    {
		ಆದರೆ (p->sst_change_lnum + buf->b_mod_xlines > buf->b_mod_ತುದಿ)
		    p->sst_change_lnum += buf->b_mod_xlines;
		ಇಲ್ಲದಿದ್ದರೆ
		    p->sst_change_lnum = buf->b_mod_ತುದಿ;
	    }
	    ಆದರೆ (p->sst_change_lnum == 0
		    || p->sst_change_lnum < buf->b_mod_bot)
		p->sst_change_lnum = buf->b_mod_bot;

	    p->sst_lnum = n;
	}
	prev = p;
	p = p->sst_ಮುಂದಿನ;
    }
}

/*
 * Reduce the number of entries in the state stack for syn_buf.
 * Returns TRUE if at least one entry was freed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
syn_ರಾಶಿ_cleanup(ಶೂನ್ಯ)
{
    synstate_T	*p, *prev;
    disptick_T	tick;
    ಪೂರ್ಣಾಂಕ		above;
    ಪೂರ್ಣಾಂಕ		dist;
    ಪೂರ್ಣಾಂಕ		retval = FALSE;

    ಆದರೆ (syn_ಖಂಡ->b_sst_ಮೊದಲ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ retval;

    // Compute normal distance between non-displayed entries.
    ಆದರೆ (syn_ಖಂಡ->b_sst_len <= ಅಡ್ಡಸಾಲುಗಳು)
	dist = 999999;
    ಇಲ್ಲದಿದ್ದರೆ
	dist = syn_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ / (syn_ಖಂಡ->b_sst_len - ಅಡ್ಡಸಾಲುಗಳು) + 1;

    /*
     * Go through the list to find the "tick" for the oldest entry that can
     * be removed.  Set "above" when the "tick" for the oldest entry is above
     * "b_sst_lasttick" (the display tick wraps around).
     */
    tick = syn_ಖಂಡ->b_sst_lasttick;
    above = FALSE;
    prev = syn_ಖಂಡ->b_sst_ಮೊದಲ;
    ಗಾಗಿ (p = prev->sst_ಮುಂದಿನ; p != ಖಾಲಿ; prev = p, p = p->sst_ಮುಂದಿನ)
    {
	ಆದರೆ (prev->sst_lnum + dist > p->sst_lnum)
	{
	    ಆದರೆ (p->sst_tick > syn_ಖಂಡ->b_sst_lasttick)
	    {
		ಆದರೆ (!above || p->sst_tick < tick)
		    tick = p->sst_tick;
		above = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!above && p->sst_tick < tick)
		tick = p->sst_tick;
	}
    }

    /*
     * Go through the list to make the entries for the oldest tick at an
     * interval of several lines.
     */
    prev = syn_ಖಂಡ->b_sst_ಮೊದಲ;
    ಗಾಗಿ (p = prev->sst_ಮುಂದಿನ; p != ಖಾಲಿ; prev = p, p = p->sst_ಮುಂದಿನ)
    {
	ಆದರೆ (p->sst_tick == tick && prev->sst_lnum + dist > p->sst_lnum)
	{
	    // Move this entry from used list to free list
	    prev->sst_ಮುಂದಿನ = p->sst_ಮುಂದಿನ;
	    syn_ರಾಶಿ_free_ನಮೂದು(syn_ಖಂಡ, p);
	    p = prev;
	    retval = TRUE;
	}
    }
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Free the allocated memory for a syn_state item.
 * Move the entry into the free list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_ರಾಶಿ_free_ನಮೂದು(synblock_T *ಖಂಡ, synstate_T *p)
{
    ಆದರೆ (ಖಂಡ->b_sst_ಹುಡುಕು == p)
	ಖಂಡ->b_sst_ಹುಡುಕು = ಖಾಲಿ;
    clear_syn_state(p);
    p->sst_ಮುಂದಿನ = ಖಂಡ->b_sst_firstfree;
    ಖಂಡ->b_sst_firstfree = p;
    ++ಖಂಡ->b_sst_freecount;
}

/*
 * Find an entry in the list of state stacks at or before "lnum".
 * Returns NULL when there is no entry or the first entry is after "lnum".
 */
    ಸ್ಥಿರ synstate_T *
syn_ರಾಶಿ_ಕಂಡುಹಿಡಿ_ನಮೂದು(linenr_T lnum)
{
    synstate_T	*p, *prev;

    prev = ಖಾಲಿ;
    p = syn_ಖಂಡ->b_sst_ಮೊದಲ;

    // The list is sorted by line number and lookups while parsing advance
    // monotonically, so resume from the last returned entry instead of
    // rescanning from the start whenever it is at or before "lnum".
    ಆದರೆ (syn_ಖಂಡ->b_sst_ಹುಡುಕು != ಖಾಲಿ
				 && syn_ಖಂಡ->b_sst_ಹುಡುಕು->sst_lnum <= lnum)
	p = syn_ಖಂಡ->b_sst_ಹುಡುಕು;

    ಗಾಗಿ ( ; p != ಖಾಲಿ; prev = p, p = p->sst_ಮುಂದಿನ)
    {
	ಆದರೆ (p->sst_lnum == lnum)
	{
	    syn_ಖಂಡ->b_sst_ಹುಡುಕು = p;
	    ಹಿಂತಿರುಗಿ p;
	}
	ಆದರೆ (p->sst_lnum > lnum)
	    ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ (prev != ಖಾಲಿ)
	syn_ಖಂಡ->b_sst_ಹುಡುಕು = prev;
    ಹಿಂತಿರುಗಿ prev;
}

/*
 * Try saving the current state in b_sst_array[].
 * The current state must be valid for the start of the current_lnum line!
 */
    ಸ್ಥಿರ synstate_T *
store_current_state(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;
    synstate_T	*p;
    bufstate_T	*bp;
    stateitem_T	*cur_si;
    synstate_T	*sp = syn_ರಾಶಿ_ಕಂಡುಹಿಡಿ_ನಮೂದು(current_lnum);

    /*
     * If the current state contains a start or end pattern that continues
     * from the previous line, we can't use it.  Don't store it then.
     */
    ಗಾಗಿ (i = current_state.ga_len - 1; i >= 0; --i)
    {
	cur_si = &CUR_STATE(i);
	ಆದರೆ (cur_si->si_h_startpos.lnum >= current_lnum
		|| cur_si->si_m_endpos.lnum >= current_lnum
		|| cur_si->si_h_endpos.lnum >= current_lnum
		|| (cur_si->si_ಕೊನೆ_idx
		    && cur_si->si_eoe_pos.lnum >= current_lnum))
	    ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ (i >= 0)
    {
	ಆದರೆ (sp != ಖಾಲಿ)
	{
	    // find "sp" in the list and remove it
	    ಆದರೆ (syn_ಖಂಡ->b_sst_ಮೊದಲ == sp)
		// it's the first entry
		syn_ಖಂಡ->b_sst_ಮೊದಲ = sp->sst_ಮುಂದಿನ;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// find the entry just before this one to adjust sst_next
		FOR_ALL_SYNSTATES(syn_ಖಂಡ, p)
		    ಆದರೆ (p->sst_ಮುಂದಿನ == sp)
			ನಿಲ್ಲಿಸು;
		ಆದರೆ (p != ಖಾಲಿ)	// just in case
		    p->sst_ಮುಂದಿನ = sp->sst_ಮುಂದಿನ;
	    }
	    syn_ರಾಶಿ_free_ನಮೂದು(syn_ಖಂಡ, sp);
	    sp = ಖಾಲಿ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sp == ಖಾಲಿ || sp->sst_lnum != current_lnum)
    {
	/*
	 * Add a new entry
	 */
	// If no free items, cleanup the array first.
	ಆದರೆ (syn_ಖಂಡ->b_sst_freecount == 0)
	{
	    (ಶೂನ್ಯ)syn_ರಾಶಿ_cleanup();
	    // "sp" may have been moved to the freelist now
	    sp = syn_ರಾಶಿ_ಕಂಡುಹಿಡಿ_ನಮೂದು(current_lnum);
	}
	// Still no free items?  Must be a strange problem...
	ಆದರೆ (syn_ಖಂಡ->b_sst_freecount == 0)
	    sp = ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Take the first item from the free list and put it in the used
	    // list, after *sp
	    p = syn_ಖಂಡ->b_sst_firstfree;
	    syn_ಖಂಡ->b_sst_firstfree = p->sst_ಮುಂದಿನ;
	    --syn_ಖಂಡ->b_sst_freecount;
	    ಆದರೆ (sp == ಖಾಲಿ)
	    {
		// Insert in front of the list
		p->sst_ಮುಂದಿನ = syn_ಖಂಡ->b_sst_ಮೊದಲ;
		syn_ಖಂಡ->b_sst_ಮೊದಲ = p;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// insert in list after *sp
		p->sst_ಮುಂದಿನ = sp->sst_ಮುಂದಿನ;
		sp->sst_ಮುಂದಿನ = p;
	    }
	    sp = p;
	    sp->sst_stacksize = 0;
	    sp->sst_lnum = current_lnum;
	    // Resume the next forward lookup from the entry just stored.
	    syn_ಖಂಡ->b_sst_ಹುಡುಕು = sp;
	}
    }
    ಆದರೆ (sp != ಖಾಲಿ)
    {
	// When overwriting an existing state stack, clear it first
	clear_syn_state(sp);
	sp->sst_stacksize = current_state.ga_len;
	ಆದರೆ (current_state.ga_len > SST_FIX_ಸ್ಥಿತಿಗಳು)
	{
	    // Need to clear it, might be something remaining from when the
	    // length was less than SST_FIX_STATES.
	    ga_init2(&sp->sst_union.sst_ga, ಗಾತ್ರ(bufstate_T), 1);
	    ಆದರೆ (ga_grow(&sp->sst_union.sst_ga, current_state.ga_len) == FAIL)
		sp->sst_stacksize = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
		sp->sst_union.sst_ga.ga_len = current_state.ga_len;
	    bp = SYN_STATE_P(&(sp->sst_union.sst_ga));
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    bp = sp->sst_union.sst_ರಾಶಿ;
	ಗಾಗಿ (i = 0; i < sp->sst_stacksize; ++i)
	{
	    bp[i].bs_idx = CUR_STATE(i).si_idx;
	    bp[i].bs_flags = CUR_STATE(i).si_flags;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
	    bp[i].bs_seqnr = CUR_STATE(i).si_seqnr;
	    bp[i].bs_cchar = CUR_STATE(i).si_cchar;
#ಆದರೆ_ಅಂತ್ಯ
	    bp[i].bs_extmatch = ref_extmatch(CUR_STATE(i).si_extmatch);
	}
	sp->sst_ಮುಂದಿನ_flags = current_ಮುಂದಿನ_flags;
	sp->sst_ಮುಂದಿನ_list = current_ಮುಂದಿನ_list;
	sp->sst_tick = display_tick;
	sp->sst_change_lnum = 0;
    }
    current_state_stored = TRUE;
    ಹಿಂತಿರುಗಿ sp;
}

/*
 * Copy a state stack from "from" in b_sst_array[] to current_state;
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ತುಂಬು_current_state(synstate_T *from)
{
    ಪೂರ್ಣಾಂಕ		i;
    bufstate_T	*bp;

    clear_current_state();
    validate_current_state();
    keepend_level = -1;
    ಆದರೆ (from->sst_stacksize
	    && ga_grow(&current_state, from->sst_stacksize) == OK)
    {
	ಆದರೆ (from->sst_stacksize > SST_FIX_ಸ್ಥಿತಿಗಳು)
	    bp = SYN_STATE_P(&(from->sst_union.sst_ga));
	ಇಲ್ಲದಿದ್ದರೆ
	    bp = from->sst_union.sst_ರಾಶಿ;
	ಗಾಗಿ (i = 0; i < from->sst_stacksize; ++i)
	{
	    CUR_STATE(i).si_idx = bp[i].bs_idx;
	    CUR_STATE(i).si_flags = bp[i].bs_flags;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
	    CUR_STATE(i).si_seqnr = bp[i].bs_seqnr;
	    CUR_STATE(i).si_cchar = bp[i].bs_cchar;
#ಆದರೆ_ಅಂತ್ಯ
	    CUR_STATE(i).si_extmatch = ref_extmatch(bp[i].bs_extmatch);
	    ಆದರೆ (keepend_level < 0 && (CUR_STATE(i).si_flags & HL_KEEPEND))
		keepend_level = i;
	    CUR_STATE(i).si_ತುದಿಗಳು = FALSE;
	    CUR_STATE(i).si_m_lnum = 0;
	    ಆದರೆ (CUR_STATE(i).si_idx >= 0)
		CUR_STATE(i).si_ಮುಂದಿನ_list =
		     (SYN_ವಸ್ತುಗಳು(syn_ಖಂಡ)[CUR_STATE(i).si_idx]).sp_ಮುಂದಿನ_list;
	    ಇಲ್ಲದಿದ್ದರೆ
		CUR_STATE(i).si_ಮುಂದಿನ_list = ಖಾಲಿ;
	    update_si_attr(i);
	}
	current_state.ga_len = from->sst_stacksize;
    }
    current_ಮುಂದಿನ_list = from->sst_ಮುಂದಿನ_list;
    current_ಮುಂದಿನ_flags = from->sst_ಮುಂದಿನ_flags;
    current_lnum = from->sst_lnum;
}

/*
 * Compare saved state stack "*sp" with the current state.
 * Return TRUE when they are equal.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
syn_ರಾಶಿ_equal(synstate_T *sp)
{
    ಪೂರ್ಣಾಂಕ		i, j;
    bufstate_T	*bp;
    reg_extmatch_T	*six, *bsx;

    // First a quick check if the stacks have the same size end nextlist.
    ಆದರೆ (sp->sst_stacksize != current_state.ga_len
	    || sp->sst_ಮುಂದಿನ_list != current_ಮುಂದಿನ_list)
	ಹಿಂತಿರುಗಿ FALSE;

    // Need to compare all states on both stacks.
    ಆದರೆ (sp->sst_stacksize > SST_FIX_ಸ್ಥಿತಿಗಳು)
	bp = SYN_STATE_P(&(sp->sst_union.sst_ga));
    ಇಲ್ಲದಿದ್ದರೆ
	bp = sp->sst_union.sst_ರಾಶಿ;

    ಗಾಗಿ (i = current_state.ga_len; --i >= 0; )
    {
	// If the item has another index the state is different.
	ಆದರೆ (bp[i].bs_idx != CUR_STATE(i).si_idx)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (bp[i].bs_extmatch == CUR_STATE(i).si_extmatch)
	    ಮುಂದುವರಿ;
	// When the extmatch pointers are different, the strings in them can
	// still be the same.  Check if the extmatch references are equal.
	bsx = bp[i].bs_extmatch;
	six = CUR_STATE(i).si_extmatch;
	// If one of the extmatch pointers is NULL the states are different.
	ಆದರೆ (bsx == ಖಾಲಿ || six == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	ಗಾಗಿ (j = 0; j < NSUBEXP; ++j)
	{
	    // Check each referenced match string. They must all be equal.
	    ಆದರೆ (bsx->matches[j] != six->matches[j])
	    {
		// If the pointer is different it can still be the same text.
		// Compare the strings, ignore case when the start item has the
		// sp_ic flag set.
		ಆದರೆ (bsx->matches[j] == ಖಾಲಿ || six->matches[j] == ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ ((SYN_ವಸ್ತುಗಳು(syn_ಖಂಡ)[CUR_STATE(i).si_idx]).sp_ic
			? MB_STRICMP(bsx->matches[j], six->matches[j]) != 0
			: STRCMP(bsx->matches[j], six->matches[j]) != 0)
		    ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (j != NSUBEXP)
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ i < 0 ? TRUE : FALSE;
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
    ಶೂನ್ಯ
syntax_ಕೊನೆ_parsing(win_T *wp, linenr_T lnum)
{
    synstate_T	*sp;

    ಆದರೆ (syn_ಖಂಡ != wp->w_s)
	ಹಿಂತಿರುಗಿ;  // not the right window
    sp = syn_ರಾಶಿ_ಕಂಡುಹಿಡಿ_ನಮೂದು(lnum);
    ಆದರೆ (sp != ಖಾಲಿ && sp->sst_lnum < lnum)
	sp = sp->sst_ಮುಂದಿನ;

    ಆದರೆ (sp != ಖಾಲಿ && sp->sst_change_lnum != 0)
	sp->sst_change_lnum = lnum;
}

/*
 * End of handling of the state stack.
 ****************************************/

    ಸ್ಥಿರ ಶೂನ್ಯ
invalidate_current_state(ಶೂನ್ಯ)
{
    clear_current_state();
    current_state.ga_itemsize = 0;	// mark current_state invalid
    current_ಮುಂದಿನ_list = ಖಾಲಿ;
    keepend_level = -1;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
validate_current_state(ಶೂನ್ಯ)
{
    current_state.ga_itemsize = ಗಾತ್ರ(stateitem_T);
    current_state.ga_growsize = 3;
}

/*
 * Return TRUE if the syntax at start of lnum changed since last time.
 * This will only be called just after get_syntax_attr() for the previous
 * line, to check if the next line needs to be redrawn too.
 */
    ಪೂರ್ಣಾಂಕ
syntax_check_ಬದಲಾಯಿತು(linenr_T lnum)
{
    ಪೂರ್ಣಾಂಕ		retval = TRUE;
    synstate_T	*sp;

    /*
     * Check the state stack when:
     * - lnum is just below the previously syntaxed line.
     * - lnum is not before the lines with saved states.
     * - lnum is not past the lines with saved states.
     * - lnum is at or before the last changed line.
     */
    ಆದರೆ (VALID_STATE(&current_state) && lnum == current_lnum + 1)
    {
	sp = syn_ರಾಶಿ_ಕಂಡುಹಿಡಿ_ನಮೂದು(lnum);
	ಆದರೆ (sp != ಖಾಲಿ && sp->sst_lnum == lnum)
	{
	    /*
	     * finish the previous line (needed when not all of the line was
	     * drawn)
	     */
	    (ಶೂನ್ಯ)syn_ಮುಕ್ತಾಯ_ಸಾಲು(FALSE);

	    /*
	     * Compare the current state with the previously saved state of
	     * the line.
	     */
	    ಆದರೆ (syn_ರಾಶಿ_equal(sp))
		retval = FALSE;

	    /*
	     * Store the current state in b_sst_array[] for later use.
	     */
	    ++current_lnum;
	    (ಶೂನ್ಯ)store_current_state();
	}
    }

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Finish the current line.
 * This doesn't return any attributes, it only gets the state at the end of
 * the line.  It can start anywhere in the line, as long as the current state
 * is valid.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
syn_ಮುಕ್ತಾಯ_ಸಾಲು(
    ಪೂರ್ಣಾಂಕ	    syncing)		// called for syncing
{
    stateitem_T	*cur_si;
    colnr_T	prev_current_ಲಂಬಸಾಲು;

    ಆಗ (!current_ಪೂರ್ಣಗೊಂಡ)
    {
	(ಶೂನ್ಯ)syn_current_attr(syncing, FALSE, ಖಾಲಿ, FALSE);
	/*
	 * When syncing, and found some item, need to check the item.
	 */
	ಆದರೆ (syncing && current_state.ga_len)
	{
	    /*
	     * Check for match with sync item.
	     */
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	    ಆದರೆ (cur_si->si_idx >= 0
		    && (SYN_ವಸ್ತುಗಳು(syn_ಖಂಡ)[cur_si->si_idx].sp_flags
					  & (HL_SYNC_ಇಲ್ಲಿ|HL_SYNC_THERE)))
		ಹಿಂತಿರುಗಿ TRUE;

	    // syn_current_attr() will have skipped the check for an item
	    // that ends here, need to do that now.  Be careful not to go
	    // past the NUL.
	    prev_current_ಲಂಬಸಾಲು = current_ಲಂಬಸಾಲು;
	    ಆದರೆ (syn_getcurline()[current_ಲಂಬಸಾಲು] != NUL)
		++current_ಲಂಬಸಾಲು;
	    check_state_ತುದಿಗಳು();
	    current_ಲಂಬಸಾಲು = prev_current_ಲಂಬಸಾಲು;
	}
	++current_ಲಂಬಸಾಲು;
    }
    ಹಿಂತಿರುಗಿ FALSE;
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
    ಪೂರ್ಣಾಂಕ
get_syntax_attr(
    colnr_T	ಲಂಬಸಾಲು,
    ಪೂರ್ಣಾಂಕ		*can_spell,
    ಪೂರ್ಣಾಂಕ		keep_state)	// keep state of char at "col"
{
    ಪೂರ್ಣಾಂಕ	    attr = 0;

    ಆದರೆ (can_spell != ಖಾಲಿ)
	// Default: Only do spelling when there is no @Spell cluster or when
	// ":syn spell toplevel" was used.
	*can_spell = syn_ಖಂಡ->b_syn_spell == SYNSPL_DEFAULT
		    ? (syn_ಖಂಡ->b_spell_cluster_id == 0)
		    : (syn_ಖಂಡ->b_syn_spell == SYNSPL_ತುದಿ);

    // check for out of memory situation
    ಆದರೆ (syn_ಖಂಡ->b_sst_array == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    // After 'synmaxcol' the attribute is always zero.
    ಆದರೆ (syn_buf->b_p_smc > 0 && ಲಂಬಸಾಲು >= (colnr_T)syn_buf->b_p_smc)
    {
	clear_current_state();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	current_id = 0;
	current_trans_id = 0;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
	current_flags = 0;
	current_seqnr = 0;
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ 0;
    }

    // Make sure current_state is valid
    ಆದರೆ (INVALID_STATE(&current_state))
	validate_current_state();

    /*
     * Skip from the current column to "col", get the attributes for "col".
     */
    ಆಗ (current_ಲಂಬಸಾಲು <= ಲಂಬಸಾಲು)
    {
	attr = syn_current_attr(FALSE, TRUE, can_spell,
				     current_ಲಂಬಸಾಲು == ಲಂಬಸಾಲು ? keep_state : FALSE);
	++current_ಲಂಬಸಾಲು;
    }

    ಹಿಂತಿರುಗಿ attr;
}

/*
 * Get syntax attributes for current_lnum, current_col.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
syn_current_attr(
    ಪೂರ್ಣಾಂಕ		syncing,		// When 1: called for syncing
    ಪೂರ್ಣಾಂಕ		displaying,		// result will be displayed
    ಪೂರ್ಣಾಂಕ		*can_spell,		// return: do spell checking
    ಪೂರ್ಣಾಂಕ		keep_state)		// keep syntax stack afterwards
{
    ಪೂರ್ಣಾಂಕ		syn_id;
    lpos_T	endpos;		// was: char_u *endp;
    lpos_T	hl_startpos;	// was: int hl_startcol;
    lpos_T	hl_endpos;
    lpos_T	eos_pos;	// end-of-start match (start region)
    lpos_T	eoe_pos;	// end-of-end pattern
    ಪೂರ್ಣಾಂಕ		ಕೊನೆ_idx;	// group ID for end pattern
    ಪೂರ್ಣಾಂಕ		idx;
    synpat_T	*spp;
    stateitem_T	*cur_si, *sip = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		startcol;
    ಪೂರ್ಣಾಂಕ		endcol;
    ದೀರ್ಘ	flags;
    ಪೂರ್ಣಾಂಕ		cchar;
    ಗಿಡ್ಡ	*ಮುಂದಿನ_list;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು_ಹೊಂದಿಕೆ;		    // found usable match
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	try_ಮುಂದಿನ_ಸ್ತಂಭ = FALSE;    // must try in next col
    ಪೂರ್ಣಾಂಕ		do_keywords;
    regmmatch_T	regmatch;
    lpos_T	pos;
    ಪೂರ್ಣಾಂಕ		lc_ಲಂಬಸಾಲು;
    reg_extmatch_T *cur_extmatch = ಖಾಲಿ;
    char_u	buf_chartab[32];  // chartab array for syn iskyeyword
    char_u	*ಸಾಲು;		// current line.  NOTE: becomes invalid after
				// looking for a pattern match!

    // variables for zero-width matches that have a "nextgroup" argument
    ಪೂರ್ಣಾಂಕ		keep_ಮುಂದಿನ_list;
    ಪೂರ್ಣಾಂಕ		zero_ಅಗಲ_ಮುಂದಿನ_list = FALSE;
    garray_T	zero_ಅಗಲ_ಮುಂದಿನ_ga;

    /*
     * No character, no attributes!  Past end of line?
     * Do try matching with an empty line (could be the start of a region).
     */
    ಸಾಲು = syn_getcurline();
    ಆದರೆ (ಸಾಲು[current_ಲಂಬಸಾಲು] == NUL && current_ಲಂಬಸಾಲು != 0)
    {
	/*
	 * If we found a match after the last column, use it.
	 */
	ಆದರೆ (ಮುಂದಿನ_ಹೊಂದಿಕೆ_idx >= 0 && ಮುಂದಿನ_ಹೊಂದಿಕೆ_ಲಂಬಸಾಲು >= (ಪೂರ್ಣಾಂಕ)current_ಲಂಬಸಾಲು
						  && ಮುಂದಿನ_ಹೊಂದಿಕೆ_ಲಂಬಸಾಲು != MAXCOL)
	    (ಶೂನ್ಯ)ತಳ್ಳು_ಮುಂದಿನ_ಹೊಂದಿಕೆ(ಖಾಲಿ);

	current_ಪೂರ್ಣಗೊಂಡ = TRUE;
	current_state_stored = FALSE;
	ಹಿಂತಿರುಗಿ 0;
    }

    // if the current or next character is NUL, we will finish the line now
    ಆದರೆ (ಸಾಲು[current_ಲಂಬಸಾಲು] == NUL || ಸಾಲು[current_ಲಂಬಸಾಲು + 1] == NUL)
    {
	current_ಪೂರ್ಣಗೊಂಡ = TRUE;
	current_state_stored = FALSE;
    }

    /*
     * When in the previous column there was a match but it could not be used
     * (empty match or already matched in this column) need to try again in
     * the next column.
     */
    ಆದರೆ (try_ಮುಂದಿನ_ಸ್ತಂಭ)
    {
	ಮುಂದಿನ_ಹೊಂದಿಕೆ_idx = -1;
	try_ಮುಂದಿನ_ಸ್ತಂಭ = FALSE;
    }

    // Only check for keywords when not syncing and there are some.
    do_keywords = !syncing
		    && (syn_ಖಂಡ->b_keywtab.ht_ಬಳಸಿದ > 0
			    || syn_ಖಂಡ->b_keywtab_ic.ht_ಬಳಸಿದ > 0);

    // Init the list of zero-width matches with a nextlist.  This is used to
    // avoid matching the same item in the same position twice.
    ga_init2(&zero_ಅಗಲ_ಮುಂದಿನ_ga, ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ), 10);

    // use syntax iskeyword option
    save_chartab(buf_chartab);

    /*
     * Repeat matching keywords and patterns, to find contained items at the
     * same column.  This stops when there are no extra matches at the current
     * column.
     */
    ಮಾಡು
    {
	ಸಿಕ್ಕಿತು_ಹೊಂದಿಕೆ = FALSE;
	keep_ಮುಂದಿನ_list = FALSE;
	syn_id = 0;


	/*
	 * 1. Check for a current state.
	 *    Only when there is no current state, or if the current state may
	 *    contain other things, we need to check for keywords and patterns.
	 *    Always need to check for contained items if some item has the
	 *    "containedin" argument (takes extra time!).
	 */
	ಆದರೆ (current_state.ga_len)
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	ಇಲ್ಲದಿದ್ದರೆ
	    cur_si = ಖಾಲಿ;

	ಆದರೆ (syn_ಖಂಡ->b_syn_containedin || cur_si == ಖಾಲಿ
					      || cur_si->si_cont_list != ಖಾಲಿ)
	{
	    /*
	     * 2. Check for keywords, if on a keyword char after a non-keyword
	     *	  char.  Don't do this when syncing.
	     */
	    ಆದರೆ (do_keywords)
	    {
	      ಸಾಲು = syn_getcurline();
	      ಆದರೆ (vim_iswordp_buf(ಸಾಲು + current_ಲಂಬಸಾಲು, syn_buf)
		      && (current_ಲಂಬಸಾಲು == 0
			  || !vim_iswordp_buf(ಸಾಲು + current_ಲಂಬಸಾಲು - 1
			      - (has_mbyte
				  ? (*mb_ತಲೆ_off)(ಸಾಲು, ಸಾಲು + current_ಲಂಬಸಾಲು - 1)
				  : 0) , syn_buf)))
	      {
		syn_id = check_keyword_id(ಸಾಲು, (ಪೂರ್ಣಾಂಕ)current_ಲಂಬಸಾಲು,
					 &endcol, &flags, &ಮುಂದಿನ_list, cur_si,
					 &cchar);
		ಆದರೆ (syn_id != 0)
		{
		    ಆದರೆ (ತಳ್ಳು_current_state(KEYWORD_IDX) == OK)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			cur_si->si_m_startcol = current_ಲಂಬಸಾಲು;
			cur_si->si_h_startpos.lnum = current_lnum;
			cur_si->si_h_startpos.ಲಂಬಸಾಲು = 0;	// starts right away
			cur_si->si_m_endpos.lnum = current_lnum;
			cur_si->si_m_endpos.ಲಂಬಸಾಲು = endcol;
			cur_si->si_h_endpos.lnum = current_lnum;
			cur_si->si_h_endpos.ಲಂಬಸಾಲು = endcol;
			cur_si->si_ತುದಿಗಳು = TRUE;
			cur_si->si_ಕೊನೆ_idx = 0;
			cur_si->si_flags = flags;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
			cur_si->si_seqnr = ಮುಂದಿನ_seqnr++;
			cur_si->si_cchar = cchar;
			ಆದರೆ (current_state.ga_len > 1)
			    cur_si->si_flags |=
				  CUR_STATE(current_state.ga_len - 2).si_flags
								 & HL_CONCEAL;
#ಆದರೆ_ಅಂತ್ಯ
			cur_si->si_id = syn_id;
			cur_si->si_trans_id = syn_id;
			ಆದರೆ (flags & HL_TRANSP)
			{
			    ಆದರೆ (current_state.ga_len < 2)
			    {
				cur_si->si_attr = 0;
				cur_si->si_trans_id = 0;
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
			    {
				cur_si->si_attr = CUR_STATE(
					current_state.ga_len - 2).si_attr;
				cur_si->si_trans_id = CUR_STATE(
					current_state.ga_len - 2).si_trans_id;
			    }
			}
			ಇಲ್ಲದಿದ್ದರೆ
			    cur_si->si_attr = syn_id2attr(syn_id);
			cur_si->si_cont_list = ಖಾಲಿ;
			cur_si->si_ಮುಂದಿನ_list = ಮುಂದಿನ_list;
			check_keepend();
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			vim_free(ಮುಂದಿನ_list);
		}
	      }
	    }

	    /*
	     * 3. Check for patterns (only if no keyword found).
	     */
	    ಆದರೆ (syn_id == 0 && syn_ಖಂಡ->b_syn_patterns.ga_len)
	    {
		/*
		 * If we didn't check for a match yet, or we are past it, check
		 * for any match with a pattern.
		 */
		ಆದರೆ (ಮುಂದಿನ_ಹೊಂದಿಕೆ_idx < 0 || ಮುಂದಿನ_ಹೊಂದಿಕೆ_ಲಂಬಸಾಲು < (ಪೂರ್ಣಾಂಕ)current_ಲಂಬಸಾಲು)
		{
		    /*
		     * Check all relevant patterns for a match at this
		     * position.  This is complicated, because matching with a
		     * pattern takes quite a bit of time, thus we want to
		     * avoid doing it when it's not needed.
		     */
		    ಮುಂದಿನ_ಹೊಂದಿಕೆ_idx = 0;		// no match in this line yet
		    ಮುಂದಿನ_ಹೊಂದಿಕೆ_ಲಂಬಸಾಲು = MAXCOL;
		    ಗಾಗಿ (idx = syn_ಖಂಡ->b_syn_patterns.ga_len; --idx >= 0; )
		    {
			spp = &(SYN_ವಸ್ತುಗಳು(syn_ಖಂಡ)[idx]);
			ಆದರೆ (	   spp->sp_syncing == syncing
				&& (displaying || !(spp->sp_flags & HL_DISPLAY))
				&& (spp->sp_type == SPTYPE_ಹೊಂದಿಕೆ
				    || spp->sp_type == SPTYPE_ಆರಂಭ)
				&& (current_ಮುಂದಿನ_list != ಖಾಲಿ
				    ? ಒಳ_id_list(ಖಾಲಿ, current_ಮುಂದಿನ_list,
							      &spp->sp_syn, 0)
				    : (cur_si == ಖಾಲಿ
					? !(spp->sp_flags & HL_CONTAINED)
					: ಒಳ_id_list(cur_si,
					    cur_si->si_cont_list, &spp->sp_syn,
					    spp->sp_flags))))
			{
			    ಪೂರ್ಣಾಂಕ r;

			    // If we already tried matching in this line, and
			    // there isn't a match before next_match_col, skip
			    // this item.
			    ಆದರೆ (spp->sp_ಸಾಲು_id == current_ಸಾಲು_id
				    && spp->sp_startcol >= ಮುಂದಿನ_ಹೊಂದಿಕೆ_ಲಂಬಸಾಲು)
				ಮುಂದುವರಿ;
			    spp->sp_ಸಾಲು_id = current_ಸಾಲು_id;

			    lc_ಲಂಬಸಾಲು = current_ಲಂಬಸಾಲು - spp->sp_offsets[SPO_LC_OFF];
			    ಆದರೆ (lc_ಲಂಬಸಾಲು < 0)
				lc_ಲಂಬಸಾಲು = 0;

			    regmatch.rmm_ic = spp->sp_ic;
			    regmatch.regprog = spp->sp_prog;
			    r = syn_regexec(&regmatch,
					     current_lnum,
					     (colnr_T)lc_ಲಂಬಸಾಲು,
					     IF_SYN_ಸಮಯ(&spp->sp_ಸಮಯ));
			    spp->sp_prog = regmatch.regprog;
			    ಆದರೆ (!r)
			    {
				// no match in this line, try another one
				spp->sp_startcol = MAXCOL;
				ಮುಂದುವರಿ;
			    }

			    /*
			     * Compute the first column of the match.
			     */
			    syn_ಕೂಡಿಸು_ಆರಂಭ_off(&pos, &regmatch,
							 spp, SPO_MS_OFF, -1);
			    ಆದರೆ (pos.lnum > current_lnum)
			    {
				// must have used end of match in a next line,
				// we can't handle that
				spp->sp_startcol = MAXCOL;
				ಮುಂದುವರಿ;
			    }
			    startcol = pos.ಲಂಬಸಾಲು;

			    // remember the next column where this pattern
			    // matches in the current line
			    spp->sp_startcol = startcol;

			    /*
			     * If a previously found match starts at a lower
			     * column number, don't use this one.
			     */
			    ಆದರೆ (startcol >= ಮುಂದಿನ_ಹೊಂದಿಕೆ_ಲಂಬಸಾಲು)
				ಮುಂದುವರಿ;

			    /*
			     * If we matched this pattern at this position
			     * before, skip it.  Must retry in the next
			     * column, because it may match from there.
			     */
			    ಆದರೆ (did_ಹೊಂದಿಕೆ_already(idx, &zero_ಅಗಲ_ಮುಂದಿನ_ga))
			    {
				try_ಮುಂದಿನ_ಸ್ತಂಭ = TRUE;
				ಮುಂದುವರಿ;
			    }

			    endpos.lnum = regmatch.endpos[0].lnum;
			    endpos.ಲಂಬಸಾಲು = regmatch.endpos[0].ಲಂಬಸಾಲು;

			    // Compute the highlight start.
			    syn_ಕೂಡಿಸು_ಆರಂಭ_off(&hl_startpos, &regmatch,
							 spp, SPO_HS_OFF, -1);

			    // Compute the region start.
			    // Default is to use the end of the match.
			    syn_ಕೂಡಿಸು_ಕೊನೆ_off(&eos_pos, &regmatch,
							 spp, SPO_RS_OFF, 0);

			    /*
			     * Grab the external submatches before they get
			     * overwritten.  Reference count doesn't change.
			     */
			    unref_extmatch(cur_extmatch);
			    cur_extmatch = re_extmatch_ಹೊರಹೋಗು;
			    re_extmatch_ಹೊರಹೋಗು = ಖಾಲಿ;

			    flags = 0;
			    eoe_pos.lnum = 0;	// avoid warning
			    eoe_pos.ಲಂಬಸಾಲು = 0;
			    ಕೊನೆ_idx = 0;
			    hl_endpos.lnum = 0;

			    /*
			     * For a "oneline" the end must be found in the
			     * same line too.  Search for it after the end of
			     * the match with the start pattern.  Set the
			     * resulting end positions at the same time.
			     */
			    ಆದರೆ (spp->sp_type == SPTYPE_ಆರಂಭ
					      && (spp->sp_flags & HL_ONELINE))
			    {
				lpos_T	startpos;

				startpos = endpos;
				ಕಂಡುಹಿಡಿ_endpos(idx, &startpos, &endpos, &hl_endpos,
				    &flags, &eoe_pos, &ಕೊನೆ_idx, cur_extmatch);
				ಆದರೆ (endpos.lnum == 0)
				    ಮುಂದುವರಿ;	    // not found
			    }

			    /*
			     * For a "match" the size must be > 0 after the
			     * end offset needs has been added.  Except when
			     * syncing.
			     */
			    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (spp->sp_type == SPTYPE_ಹೊಂದಿಕೆ)
			    {
				syn_ಕೂಡಿಸು_ಕೊನೆ_off(&hl_endpos, &regmatch, spp,
							       SPO_HE_OFF, 0);
				syn_ಕೂಡಿಸು_ಕೊನೆ_off(&endpos, &regmatch, spp,
							       SPO_ME_OFF, 0);
				ಆದರೆ (endpos.lnum == current_lnum
				      && (ಪೂರ್ಣಾಂಕ)endpos.ಲಂಬಸಾಲು + syncing < startcol)
				{
				    /*
				     * If an empty string is matched, may need
				     * to try matching again at next column.
				     */
				    ಆದರೆ (regmatch.startpos[0].ಲಂಬಸಾಲು
						    == regmatch.endpos[0].ಲಂಬಸಾಲು)
					try_ಮುಂದಿನ_ಸ್ತಂಭ = TRUE;
				    ಮುಂದುವರಿ;
				}
			    }

			    /*
			     * keep the best match so far in next_match_*
			     */
			    // Highlighting must start after startpos and end
			    // before endpos.
			    ಆದರೆ (hl_startpos.lnum == current_lnum
					   && (ಪೂರ್ಣಾಂಕ)hl_startpos.ಲಂಬಸಾಲು < startcol)
				hl_startpos.ಲಂಬಸಾಲು = startcol;
			    ಎಲ್ಲೆ_pos_zero(&hl_endpos, &endpos);

			    ಮುಂದಿನ_ಹೊಂದಿಕೆ_idx = idx;
			    ಮುಂದಿನ_ಹೊಂದಿಕೆ_ಲಂಬಸಾಲು = startcol;
			    ಮುಂದಿನ_ಹೊಂದಿಕೆ_m_endpos = endpos;
			    ಮುಂದಿನ_ಹೊಂದಿಕೆ_h_endpos = hl_endpos;
			    ಮುಂದಿನ_ಹೊಂದಿಕೆ_h_startpos = hl_startpos;
			    ಮುಂದಿನ_ಹೊಂದಿಕೆ_flags = flags;
			    ಮುಂದಿನ_ಹೊಂದಿಕೆ_eos_pos = eos_pos;
			    ಮುಂದಿನ_ಹೊಂದಿಕೆ_eoe_pos = eoe_pos;
			    ಮುಂದಿನ_ಹೊಂದಿಕೆ_ಕೊನೆ_idx = ಕೊನೆ_idx;
			    unref_extmatch(ಮುಂದಿನ_ಹೊಂದಿಕೆ_extmatch);
			    ಮುಂದಿನ_ಹೊಂದಿಕೆ_extmatch = cur_extmatch;
			    cur_extmatch = ಖಾಲಿ;
			}
		    }
		}

		/*
		 * If we found a match at the current column, use it.
		 */
		ಆದರೆ (ಮುಂದಿನ_ಹೊಂದಿಕೆ_idx >= 0 && ಮುಂದಿನ_ಹೊಂದಿಕೆ_ಲಂಬಸಾಲು == (ಪೂರ್ಣಾಂಕ)current_ಲಂಬಸಾಲು)
		{
		    synpat_T	*lspp;

		    // When a zero-width item matched which has a nextgroup,
		    // don't push the item but set nextgroup.
		    lspp = &(SYN_ವಸ್ತುಗಳು(syn_ಖಂಡ)[ಮುಂದಿನ_ಹೊಂದಿಕೆ_idx]);
		    ಆದರೆ (ಮುಂದಿನ_ಹೊಂದಿಕೆ_m_endpos.lnum == current_lnum
			    && ಮುಂದಿನ_ಹೊಂದಿಕೆ_m_endpos.ಲಂಬಸಾಲು == current_ಲಂಬಸಾಲು
			    && lspp->sp_ಮುಂದಿನ_list != ಖಾಲಿ)
		    {
			current_ಮುಂದಿನ_list = lspp->sp_ಮುಂದಿನ_list;
			current_ಮುಂದಿನ_flags = lspp->sp_flags;
			keep_ಮುಂದಿನ_list = TRUE;
			zero_ಅಗಲ_ಮುಂದಿನ_list = TRUE;

			// Add the index to a list, so that we can check
			// later that we don't match it again (and cause an
			// endless loop).
			ಆದರೆ (ga_grow(&zero_ಅಗಲ_ಮುಂದಿನ_ga, 1) == OK)
			{
			    ((ಪೂರ್ಣಾಂಕ *)(zero_ಅಗಲ_ಮುಂದಿನ_ga.ga_ದತ್ತಾಂಶ))
				[zero_ಅಗಲ_ಮುಂದಿನ_ga.ga_len++] = ಮುಂದಿನ_ಹೊಂದಿಕೆ_idx;
			}
			ಮುಂದಿನ_ಹೊಂದಿಕೆ_idx = -1;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			cur_si = ತಳ್ಳು_ಮುಂದಿನ_ಹೊಂದಿಕೆ(cur_si);
		    ಸಿಕ್ಕಿತು_ಹೊಂದಿಕೆ = TRUE;
		}
	    }
	}

	/*
	 * Handle searching for nextgroup match.
	 */
	ಆದರೆ (current_ಮುಂದಿನ_list != ಖಾಲಿ && !keep_ಮುಂದಿನ_list)
	{
	    /*
	     * If a nextgroup was not found, continue looking for one if:
	     * - this is an empty line and the "skipempty" option was given
	     * - we are on white space and the "skipwhite" option was given
	     */
	    ಆದರೆ (!ಸಿಕ್ಕಿತು_ಹೊಂದಿಕೆ)
	    {
		ಸಾಲು = syn_getcurline();
		ಆದರೆ (((current_ಮುಂದಿನ_flags & HL_SKIPWHITE)
			    && VIM_ISWHITE(ಸಾಲು[current_ಲಂಬಸಾಲು]))
			|| ((current_ಮುಂದಿನ_flags & HL_SKIPEMPTY)
			    && *ಸಾಲು == NUL))
		    ನಿಲ್ಲಿಸು;
	    }

	    /*
	     * If a nextgroup was found: Use it, and continue looking for
	     * contained matches.
	     * If a nextgroup was not found: Continue looking for a normal
	     * match.
	     * When did set current_next_list for a zero-width item and no
	     * match was found don't loop (would get stuck).
	     */
	    current_ಮುಂದಿನ_list = ಖಾಲಿ;
	    ಮುಂದಿನ_ಹೊಂದಿಕೆ_idx = -1;
	    ಆದರೆ (!zero_ಅಗಲ_ಮುಂದಿನ_list)
		ಸಿಕ್ಕಿತು_ಹೊಂದಿಕೆ = TRUE;
	}

    } ಆಗ (ಸಿಕ್ಕಿತು_ಹೊಂದಿಕೆ);

    restore_chartab(buf_chartab);

    /*
     * Use attributes from the current state, if within its highlighting.
     * If not, use attributes from the current-but-one state, etc.
     */
    current_attr = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    current_id = 0;
    current_trans_id = 0;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    current_flags = 0;
    current_seqnr = 0;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (cur_si != ಖಾಲಿ)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_EVAL
	ಪೂರ್ಣಾಂಕ	current_trans_id = 0;
#ಆದರೆ_ಅಂತ್ಯ
	ಗಾಗಿ (idx = current_state.ga_len - 1; idx >= 0; --idx)
	{
	    sip = &CUR_STATE(idx);
	    ಆದರೆ ((current_lnum > sip->si_h_startpos.lnum
			|| (current_lnum == sip->si_h_startpos.lnum
			    && current_ಲಂಬಸಾಲು >= sip->si_h_startpos.ಲಂಬಸಾಲು))
		    && (sip->si_h_endpos.lnum == 0
			|| current_lnum < sip->si_h_endpos.lnum
			|| (current_lnum == sip->si_h_endpos.lnum
			    && current_ಲಂಬಸಾಲು < sip->si_h_endpos.ಲಂಬಸಾಲು)))
	    {
		current_attr = sip->si_attr;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		current_id = sip->si_id;
#ಆದರೆ_ಅಂತ್ಯ
		current_trans_id = sip->si_trans_id;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
		current_flags = sip->si_flags;
		current_seqnr = sip->si_seqnr;
		current_sub_char = sip->si_cchar;
#ಆದರೆ_ಅಂತ್ಯ
		ನಿಲ್ಲಿಸು;
	    }
	}

	ಆದರೆ (can_spell != ಖಾಲಿ)
	{
	    ರಚನೆ sp_syn   sps;

	    /*
	     * set "can_spell" to TRUE if spell checking is supposed to be
	     * done in the current item.
	     */
	    ಆದರೆ (syn_ಖಂಡ->b_spell_cluster_id == 0)
	    {
		// There is no @Spell cluster: Do spelling for items without
		// @NoSpell cluster.
		ಆದರೆ (syn_ಖಂಡ->b_nospell_cluster_id == 0
						     || current_trans_id == 0)
		    *can_spell = (syn_ಖಂಡ->b_syn_spell != SYNSPL_NOTOP);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    sps.inc_ಟ್ಯಾಗ್ = 0;
		    sps.id = syn_ಖಂಡ->b_nospell_cluster_id;
		    sps.cont_ಒಳ_list = ಖಾಲಿ;
		    *can_spell = !ಒಳ_id_list(sip, sip->si_cont_list, &sps, 0);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// The @Spell cluster is defined: Do spelling in items with
		// the @Spell cluster.  But not when @NoSpell is also there.
		// At the toplevel only spell check when ":syn spell toplevel"
		// was used.
		ಆದರೆ (current_trans_id == 0)
		    *can_spell = (syn_ಖಂಡ->b_syn_spell == SYNSPL_ತುದಿ);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    sps.inc_ಟ್ಯಾಗ್ = 0;
		    sps.id = syn_ಖಂಡ->b_spell_cluster_id;
		    sps.cont_ಒಳ_list = ಖಾಲಿ;
		    *can_spell = ಒಳ_id_list(sip, sip->si_cont_list, &sps, 0);

		    ಆದರೆ (syn_ಖಂಡ->b_nospell_cluster_id != 0)
		    {
			sps.id = syn_ಖಂಡ->b_nospell_cluster_id;
			ಆದರೆ (ಒಳ_id_list(sip, sip->si_cont_list, &sps, 0))
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
	ಆದರೆ (!syncing && !keep_state)
	{
	    check_state_ತುದಿಗಳು();
	    ಆದರೆ (current_state.ga_len > 0
				      && syn_getcurline()[current_ಲಂಬಸಾಲು] != NUL)
	    {
		++current_ಲಂಬಸಾಲು;
		check_state_ತುದಿಗಳು();
		--current_ಲಂಬಸಾಲು;
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (can_spell != ಖಾಲಿ)
	// Default: Only do spelling when there is no @Spell cluster or when
	// ":syn spell toplevel" was used.
	*can_spell = syn_ಖಂಡ->b_syn_spell == SYNSPL_DEFAULT
		    ? (syn_ಖಂಡ->b_spell_cluster_id == 0)
		    : (syn_ಖಂಡ->b_syn_spell == SYNSPL_ತುದಿ);

    // nextgroup ends at end of line, unless "skipnl" or "skipempty" present
    ಆದರೆ (current_ಮುಂದಿನ_list != ಖಾಲಿ
	    && (ಸಾಲು = syn_getcurline())[current_ಲಂಬಸಾಲು] != NUL
	    && ಸಾಲು[current_ಲಂಬಸಾಲು + 1] == NUL
	    && !(current_ಮುಂದಿನ_flags & (HL_SKIPNL | HL_SKIPEMPTY)))
	current_ಮುಂದಿನ_list = ಖಾಲಿ;

    ಆದರೆ (zero_ಅಗಲ_ಮುಂದಿನ_ga.ga_len > 0)
	ga_clear(&zero_ಅಗಲ_ಮುಂದಿನ_ga);

    // No longer need external matches.  But keep next_match_extmatch.
    unref_extmatch(re_extmatch_ಹೊರಹೋಗು);
    re_extmatch_ಹೊರಹೋಗು = ಖಾಲಿ;
    unref_extmatch(cur_extmatch);

    ಹಿಂತಿರುಗಿ current_attr;
}


/*
 * Check if we already matched pattern "idx" at the current column.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
did_ಹೊಂದಿಕೆ_already(ಪೂರ್ಣಾಂಕ idx, garray_T *gap)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = current_state.ga_len; --i >= 0; )
	ಆದರೆ (CUR_STATE(i).si_m_startcol == (ಪೂರ್ಣಾಂಕ)current_ಲಂಬಸಾಲು
		&& CUR_STATE(i).si_m_lnum == (ಪೂರ್ಣಾಂಕ)current_lnum
		&& CUR_STATE(i).si_idx == idx)
	    ಹಿಂತಿರುಗಿ TRUE;

    // Zero-width matches with a nextgroup argument are not put on the syntax
    // stack, and can only be matched once anyway.
    ಗಾಗಿ (i = gap->ga_len; --i >= 0; )
	ಆದರೆ (((ಪೂರ್ಣಾಂಕ *)(gap->ga_ದತ್ತಾಂಶ))[i] == idx)
	    ಹಿಂತಿರುಗಿ TRUE;

    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Push the next match onto the stack.
 */
    ಸ್ಥಿರ stateitem_T *
ತಳ್ಳು_ಮುಂದಿನ_ಹೊಂದಿಕೆ(stateitem_T *cur_si)
{
    synpat_T	*spp;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    ಪೂರ್ಣಾಂಕ		 save_flags;
#ಆದರೆ_ಅಂತ್ಯ

    spp = &(SYN_ವಸ್ತುಗಳು(syn_ಖಂಡ)[ಮುಂದಿನ_ಹೊಂದಿಕೆ_idx]);

    /*
     * Push the item in current_state stack;
     */
    ಆದರೆ (ತಳ್ಳು_current_state(ಮುಂದಿನ_ಹೊಂದಿಕೆ_idx) == OK)
    {
	/*
	 * If it's a start-skip-end type that crosses lines, figure out how
	 * much it continues in this line.  Otherwise just fill in the length.
	 */
	cur_si = &CUR_STATE(current_state.ga_len - 1);
	cur_si->si_h_startpos = ಮುಂದಿನ_ಹೊಂದಿಕೆ_h_startpos;
	cur_si->si_m_startcol = current_ಲಂಬಸಾಲು;
	cur_si->si_m_lnum = current_lnum;
	cur_si->si_flags = spp->sp_flags;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
	cur_si->si_seqnr = ಮುಂದಿನ_seqnr++;
	cur_si->si_cchar = spp->sp_cchar;
	ಆದರೆ (current_state.ga_len > 1)
	    cur_si->si_flags |=
		    CUR_STATE(current_state.ga_len - 2).si_flags & HL_CONCEAL;
#ಆದರೆ_ಅಂತ್ಯ
	cur_si->si_ಮುಂದಿನ_list = spp->sp_ಮುಂದಿನ_list;
	cur_si->si_extmatch = ref_extmatch(ಮುಂದಿನ_ಹೊಂದಿಕೆ_extmatch);
	ಆದರೆ (spp->sp_type == SPTYPE_ಆರಂಭ && !(spp->sp_flags & HL_ONELINE))
	{
	    // Try to find the end pattern in the current line
	    update_si_ಕೊನೆ(cur_si, (ಪೂರ್ಣಾಂಕ)(ಮುಂದಿನ_ಹೊಂದಿಕೆ_m_endpos.ಲಂಬಸಾಲು), TRUE);
	    check_keepend();
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    cur_si->si_m_endpos = ಮುಂದಿನ_ಹೊಂದಿಕೆ_m_endpos;
	    cur_si->si_h_endpos = ಮುಂದಿನ_ಹೊಂದಿಕೆ_h_endpos;
	    cur_si->si_ತುದಿಗಳು = TRUE;
	    cur_si->si_flags |= ಮುಂದಿನ_ಹೊಂದಿಕೆ_flags;
	    cur_si->si_eoe_pos = ಮುಂದಿನ_ಹೊಂದಿಕೆ_eoe_pos;
	    cur_si->si_ಕೊನೆ_idx = ಮುಂದಿನ_ಹೊಂದಿಕೆ_ಕೊನೆ_idx;
	}
	ಆದರೆ (keepend_level < 0 && (cur_si->si_flags & HL_KEEPEND))
	    keepend_level = current_state.ga_len - 1;
	check_keepend();
	update_si_attr(current_state.ga_len - 1);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
	save_flags = cur_si->si_flags & (HL_CONCEAL | HL_CONCEALENDS);
#ಆದರೆ_ಅಂತ್ಯ
	/*
	 * If the start pattern has another highlight group, push another item
	 * on the stack for the start pattern.
	 */
	ಆದರೆ (	   spp->sp_type == SPTYPE_ಆರಂಭ
		&& spp->sp_syn_ಹೊಂದಿಕೆ_id != 0
		&& ತಳ್ಳು_current_state(ಮುಂದಿನ_ಹೊಂದಿಕೆ_idx) == OK)
	{
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	    cur_si->si_h_startpos = ಮುಂದಿನ_ಹೊಂದಿಕೆ_h_startpos;
	    cur_si->si_m_startcol = current_ಲಂಬಸಾಲು;
	    cur_si->si_m_lnum = current_lnum;
	    cur_si->si_m_endpos = ಮುಂದಿನ_ಹೊಂದಿಕೆ_eos_pos;
	    cur_si->si_h_endpos = ಮುಂದಿನ_ಹೊಂದಿಕೆ_eos_pos;
	    cur_si->si_ತುದಿಗಳು = TRUE;
	    cur_si->si_ಕೊನೆ_idx = 0;
	    cur_si->si_flags = HL_ಹೊಂದಿಕೆ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
	    cur_si->si_seqnr = ಮುಂದಿನ_seqnr++;
	    cur_si->si_flags |= save_flags;
	    ಆದರೆ (cur_si->si_flags & HL_CONCEALENDS)
		cur_si->si_flags |= HL_CONCEAL;
#ಆದರೆ_ಅಂತ್ಯ
	    cur_si->si_ಮುಂದಿನ_list = ಖಾಲಿ;
	    check_keepend();
	    update_si_attr(current_state.ga_len - 1);
	}
    }

    ಮುಂದಿನ_ಹೊಂದಿಕೆ_idx = -1;	// try other match next time

    ಹಿಂತಿರುಗಿ cur_si;
}

/*
 * Check for end of current state (and the states before it).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
check_state_ತುದಿಗಳು(ಶೂನ್ಯ)
{
    stateitem_T	*cur_si;
    ಪೂರ್ಣಾಂಕ		had_extend;

    cur_si = &CUR_STATE(current_state.ga_len - 1);
    ಗಾಗಿ (;;)
    {
	ಆದರೆ (cur_si->si_ತುದಿಗಳು
		&& (cur_si->si_m_endpos.lnum < current_lnum
		    || (cur_si->si_m_endpos.lnum == current_lnum
			&& cur_si->si_m_endpos.ಲಂಬಸಾಲು <= current_ಲಂಬಸಾಲು)))
	{
	    /*
	     * If there is an end pattern group ID, highlight the end pattern
	     * now.  No need to pop the current item from the stack.
	     * Only do this if the end pattern continues beyond the current
	     * position.
	     */
	    ಆದರೆ (cur_si->si_ಕೊನೆ_idx
		    && (cur_si->si_eoe_pos.lnum > current_lnum
			|| (cur_si->si_eoe_pos.lnum == current_lnum
			    && cur_si->si_eoe_pos.ಲಂಬಸಾಲು > current_ಲಂಬಸಾಲು)))
	    {
		cur_si->si_idx = cur_si->si_ಕೊನೆ_idx;
		cur_si->si_ಕೊನೆ_idx = 0;
		cur_si->si_m_endpos = cur_si->si_eoe_pos;
		cur_si->si_h_endpos = cur_si->si_eoe_pos;
		cur_si->si_flags |= HL_ಹೊಂದಿಕೆ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
		cur_si->si_seqnr = ಮುಂದಿನ_seqnr++;
		ಆದರೆ (cur_si->si_flags & HL_CONCEALENDS)
		    cur_si->si_flags |= HL_CONCEAL;
#ಆದರೆ_ಅಂತ್ಯ
		update_si_attr(current_state.ga_len - 1);

		// nextgroup= should not match in the end pattern
		current_ಮುಂದಿನ_list = ಖಾಲಿ;

		// what matches next may be different now, clear it
		ಮುಂದಿನ_ಹೊಂದಿಕೆ_idx = 0;
		ಮುಂದಿನ_ಹೊಂದಿಕೆ_ಲಂಬಸಾಲು = MAXCOL;
		ನಿಲ್ಲಿಸು;
	    }

	    // handle next_list, unless at end of line and no "skipnl" or
	    // "skipempty"
	    current_ಮುಂದಿನ_list = cur_si->si_ಮುಂದಿನ_list;
	    current_ಮುಂದಿನ_flags = cur_si->si_flags;
	    ಆದರೆ (!(current_ಮುಂದಿನ_flags & (HL_SKIPNL | HL_SKIPEMPTY))
		    && syn_getcurline()[current_ಲಂಬಸಾಲು] == NUL)
		current_ಮುಂದಿನ_list = ಖಾಲಿ;

	    // When the ended item has "extend", another item with
	    // "keepend" now needs to check for its end.
	    had_extend = (cur_si->si_flags & HL_EXTEND);

	    ತೆಗೆ_current_state();

	    ಆದರೆ (current_state.ga_len == 0)
		ನಿಲ್ಲಿಸು;

	    ಆದರೆ (had_extend && keepend_level >= 0)
	    {
		syn_update_ತುದಿಗಳು(FALSE);
		ಆದರೆ (current_state.ga_len == 0)
		    ನಿಲ್ಲಿಸು;
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
	    ಆದರೆ (cur_si->si_idx >= 0
		    && SYN_ವಸ್ತುಗಳು(syn_ಖಂಡ)[cur_si->si_idx].sp_type
								== SPTYPE_ಆರಂಭ
		    && !(cur_si->si_flags & (HL_ಹೊಂದಿಕೆ | HL_KEEPEND)))
	    {
		update_si_ಕೊನೆ(cur_si, (ಪೂರ್ಣಾಂಕ)current_ಲಂಬಸಾಲು, TRUE);
		check_keepend();
		ಆದರೆ ((current_ಮುಂದಿನ_flags & HL_HAS_EOL)
			&& keepend_level < 0
			&& syn_getcurline()[current_ಲಂಬಸಾಲು] == NUL)
		    ನಿಲ್ಲಿಸು;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ನಿಲ್ಲಿಸು;
    }
}

/*
 * Update an entry in the current_state stack for a match or region.  This
 * fills in si_attr, si_next_list and si_cont_list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
update_si_attr(ಪೂರ್ಣಾಂಕ idx)
{
    stateitem_T	*sip = &CUR_STATE(idx);
    synpat_T	*spp;

    // This should not happen...
    ಆದರೆ (sip->si_idx < 0)
	ಹಿಂತಿರುಗಿ;

    spp = &(SYN_ವಸ್ತುಗಳು(syn_ಖಂಡ)[sip->si_idx]);
    ಆದರೆ (sip->si_flags & HL_ಹೊಂದಿಕೆ)
	sip->si_id = spp->sp_syn_ಹೊಂದಿಕೆ_id;
    ಇಲ್ಲದಿದ್ದರೆ
	sip->si_id = spp->sp_syn.id;
    sip->si_attr = syn_id2attr(sip->si_id);
    sip->si_trans_id = sip->si_id;
    ಆದರೆ (sip->si_flags & HL_ಹೊಂದಿಕೆ)
	sip->si_cont_list = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
	sip->si_cont_list = spp->sp_cont_list;

    /*
     * For transparent items, take attr from outer item.
     * Also take cont_list, if there is none.
     * Don't do this for the matchgroup of a start or end pattern.
     */
    ಆದರೆ ((spp->sp_flags & HL_TRANSP) && !(sip->si_flags & HL_ಹೊಂದಿಕೆ))
    {
	ಆದರೆ (idx == 0)
	{
	    sip->si_attr = 0;
	    sip->si_trans_id = 0;
	    ಆದರೆ (sip->si_cont_list == ಖಾಲಿ)
		sip->si_cont_list = ID_LIST_ALL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    sip->si_attr = CUR_STATE(idx - 1).si_attr;
	    sip->si_trans_id = CUR_STATE(idx - 1).si_trans_id;
	    ಆದರೆ (sip->si_cont_list == ಖಾಲಿ)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
check_keepend(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;
    lpos_T	maxpos;
    lpos_T	maxpos_h;
    stateitem_T	*sip;

    /*
     * This check can consume a lot of time; only do it from the level where
     * there really is a keepend.
     */
    ಆದರೆ (keepend_level < 0)
	ಹಿಂತಿರುಗಿ;

    /*
     * Find the last index of an "extend" item.  "keepend" items before that
     * won't do anything.  If there is no "extend" item "i" will be
     * "keepend_level" and all "keepend" items will work normally.
     */
    ಗಾಗಿ (i = current_state.ga_len - 1; i > keepend_level; --i)
	ಆದರೆ (CUR_STATE(i).si_flags & HL_EXTEND)
	    ನಿಲ್ಲಿಸು;

    maxpos.lnum = 0;
    maxpos.ಲಂಬಸಾಲು = 0;
    maxpos_h.lnum = 0;
    maxpos_h.ಲಂಬಸಾಲು = 0;
    ಗಾಗಿ ( ; i < current_state.ga_len; ++i)
    {
	sip = &CUR_STATE(i);
	ಆದರೆ (maxpos.lnum != 0)
	{
	    ಎಲ್ಲೆ_pos_zero(&sip->si_m_endpos, &maxpos);
	    ಎಲ್ಲೆ_pos_zero(&sip->si_h_endpos, &maxpos_h);
	    ಎಲ್ಲೆ_pos_zero(&sip->si_eoe_pos, &maxpos);
	    sip->si_ತುದಿಗಳು = TRUE;
	}
	ಆದರೆ (sip->si_ತುದಿಗಳು && (sip->si_flags & HL_KEEPEND))
	{
	    ಆದರೆ (maxpos.lnum == 0
		    || maxpos.lnum > sip->si_m_endpos.lnum
		    || (maxpos.lnum == sip->si_m_endpos.lnum
			&& maxpos.ಲಂಬಸಾಲು > sip->si_m_endpos.ಲಂಬಸಾಲು))
		maxpos = sip->si_m_endpos;
	    ಆದರೆ (maxpos_h.lnum == 0
		    || maxpos_h.lnum > sip->si_h_endpos.lnum
		    || (maxpos_h.lnum == sip->si_h_endpos.lnum
			&& maxpos_h.ಲಂಬಸಾಲು > sip->si_h_endpos.ಲಂಬಸಾಲು))
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
    ಸ್ಥಿರ ಶೂನ್ಯ
update_si_ಕೊನೆ(
    stateitem_T	*sip,
    ಪೂರ್ಣಾಂಕ		startcol,   // where to start searching for the end
    ಪೂರ್ಣಾಂಕ		force)	    // when TRUE overrule a previous end
{
    lpos_T	startpos;
    lpos_T	endpos;
    lpos_T	hl_endpos;
    lpos_T	ಕೊನೆ_endpos;
    ಪೂರ್ಣಾಂಕ		ಕೊನೆ_idx;

    // return quickly for a keyword
    ಆದರೆ (sip->si_idx < 0)
	ಹಿಂತಿರುಗಿ;

    // Don't update when it's already done.  Can be a match of an end pattern
    // that started in a previous line.  Watch out: can also be a "keepend"
    // from a containing item.
    ಆದರೆ (!force && sip->si_m_endpos.lnum >= current_lnum)
	ಹಿಂತಿರುಗಿ;

    /*
     * We need to find the end of the region.  It may continue in the next
     * line.
     */
    ಕೊನೆ_idx = 0;
    startpos.lnum = current_lnum;
    startpos.ಲಂಬಸಾಲು = startcol;
    ಕಂಡುಹಿಡಿ_endpos(sip->si_idx, &startpos, &endpos, &hl_endpos,
		   &(sip->si_flags), &ಕೊನೆ_endpos, &ಕೊನೆ_idx, sip->si_extmatch);

    ಆದರೆ (endpos.lnum == 0)
    {
	// No end pattern matched.
	ಆದರೆ (SYN_ವಸ್ತುಗಳು(syn_ಖಂಡ)[sip->si_idx].sp_flags & HL_ONELINE)
	{
	    // a "oneline" never continues in the next line
	    sip->si_ತುದಿಗಳು = TRUE;
	    sip->si_m_endpos.lnum = current_lnum;
	    sip->si_m_endpos.ಲಂಬಸಾಲು = syn_getcurline_len();
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // continues in the next line
	    sip->si_ತುದಿಗಳು = FALSE;
	    sip->si_m_endpos.lnum = 0;
	}
	sip->si_h_endpos = sip->si_m_endpos;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// match within this line
	sip->si_m_endpos = endpos;
	sip->si_h_endpos = hl_endpos;
	sip->si_eoe_pos = ಕೊನೆ_endpos;
	sip->si_ತುದಿಗಳು = TRUE;
	sip->si_ಕೊನೆ_idx = ಕೊನೆ_idx;
    }
}

/*
 * Add a new state to the current state stack.
 * It is cleared and the index set to "idx".
 * Return FAIL if it's not possible (out of memory).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ತಳ್ಳು_current_state(ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (ga_grow(&current_state, 1) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    CLEAR_ಸೂಚಕ(&CUR_STATE(current_state.ga_len));
    CUR_STATE(current_state.ga_len).si_idx = idx;
    ++current_state.ga_len;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Remove a state from the current_state stack.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ತೆಗೆ_current_state(ಶೂನ್ಯ)
{
    ಆದರೆ (current_state.ga_len)
    {
	unref_extmatch(CUR_STATE(current_state.ga_len - 1).si_extmatch);
	--current_state.ga_len;
    }
    // after the end of a pattern, try matching a keyword or pattern
    ಮುಂದಿನ_ಹೊಂದಿಕೆ_idx = -1;

    // if first state with "keepend" is popped, reset keepend_level
    ಆದರೆ (keepend_level >= current_state.ga_len)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕಂಡುಹಿಡಿ_endpos(
    ಪೂರ್ಣಾಂಕ		idx,		// index of the pattern
    lpos_T	*startpos,	// where to start looking for an END match
    lpos_T	*m_endpos,	// return: end of match
    lpos_T	*hl_endpos,	// return: end of highlighting
    ದೀರ್ಘ	*flagsp,	// return: flags of matching END
    lpos_T	*ಕೊನೆ_endpos,	// return: end of end pattern match
    ಪೂರ್ಣಾಂಕ		*ಕೊನೆ_idx,	// return: group ID for end pat. match, or 0
    reg_extmatch_T *ಆರಂಭ_ext)	// submatches from the start pattern
{
    colnr_T	matchcol;
    synpat_T	*spp, *spp_skip;
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ_idx;
    ಪೂರ್ಣಾಂಕ		ಉತ್ತಮ_idx;
    regmmatch_T	regmatch;
    regmmatch_T	ಉತ್ತಮ_regmatch;	    // startpos/endpos of best match
    lpos_T	pos;
    ಪೂರ್ಣಾಂಕ		had_ಹೊಂದಿಕೆ = FALSE;
    char_u	buf_chartab[32];  // chartab array for syn option iskyeyword

    // just in case we are invoked for a keyword
    ಆದರೆ (idx < 0)
	ಹಿಂತಿರುಗಿ;

    /*
     * Check for being called with a START pattern.
     * Can happen with a match that continues to the next line, because it
     * contained a region.
     */
    spp = &(SYN_ವಸ್ತುಗಳು(syn_ಖಂಡ)[idx]);
    ಆದರೆ (spp->sp_type != SPTYPE_ಆರಂಭ)
    {
	*hl_endpos = *startpos;
	ಹಿಂತಿರುಗಿ;
    }

    /*
     * Find the SKIP or first END pattern after the last START pattern.
     */
    ಗಾಗಿ (;;)
    {
	spp = &(SYN_ವಸ್ತುಗಳು(syn_ಖಂಡ)[idx]);
	ಆದರೆ (spp->sp_type != SPTYPE_ಆರಂಭ)
	    ನಿಲ್ಲಿಸು;
	++idx;
    }

    /*
     *	Lookup the SKIP pattern (if present)
     */
    ಆದರೆ (spp->sp_type == SPTYPE_SKIP)
    {
	spp_skip = spp;
	++idx;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	spp_skip = ಖಾಲಿ;

    // Setup external matches for syn_regexec().
    unref_extmatch(re_extmatch_ಒಳ);
    re_extmatch_ಒಳ = ref_extmatch(ಆರಂಭ_ext);

    matchcol = startpos->ಲಂಬಸಾಲು;	// start looking for a match at sstart
    ಆರಂಭ_idx = idx;		// remember the first END pattern.
    ಉತ್ತಮ_regmatch.startpos[0].ಲಂಬಸಾಲು = 0;		// avoid compiler warning

    // use syntax iskeyword option
    save_chartab(buf_chartab);

    ಗಾಗಿ (;;)
    {
	/*
	 * Find end pattern that matches first after "matchcol".
	 */
	ಉತ್ತಮ_idx = -1;
	ಗಾಗಿ (idx = ಆರಂಭ_idx; idx < syn_ಖಂಡ->b_syn_patterns.ga_len; ++idx)
	{
	    ಪೂರ್ಣಾಂಕ lc_ಲಂಬಸಾಲು = matchcol;
	    ಪೂರ್ಣಾಂಕ r;

	    spp = &(SYN_ವಸ್ತುಗಳು(syn_ಖಂಡ)[idx]);
	    ಆದರೆ (spp->sp_type != SPTYPE_ಕೊನೆ)	// past last END pattern
		ನಿಲ್ಲಿಸು;
	    lc_ಲಂಬಸಾಲು -= spp->sp_offsets[SPO_LC_OFF];
	    ಆದರೆ (lc_ಲಂಬಸಾಲು < 0)
		lc_ಲಂಬಸಾಲು = 0;

	    regmatch.rmm_ic = spp->sp_ic;
	    regmatch.regprog = spp->sp_prog;
	    r = syn_regexec(&regmatch, startpos->lnum, lc_ಲಂಬಸಾಲು,
						  IF_SYN_ಸಮಯ(&spp->sp_ಸಮಯ));
	    spp->sp_prog = regmatch.regprog;
	    ಆದರೆ (r)
	    {
		ಆದರೆ (ಉತ್ತಮ_idx == -1 || regmatch.startpos[0].ಲಂಬಸಾಲು
					      < ಉತ್ತಮ_regmatch.startpos[0].ಲಂಬಸಾಲು)
		{
		    ಉತ್ತಮ_idx = idx;
		    ಉತ್ತಮ_regmatch.startpos[0] = regmatch.startpos[0];
		    ಉತ್ತಮ_regmatch.endpos[0] = regmatch.endpos[0];
		}
	    }
	}

	/*
	 * If all end patterns have been tried, and there is no match, the
	 * item continues until end-of-line.
	 */
	ಆದರೆ (ಉತ್ತಮ_idx == -1)
	    ನಿಲ್ಲಿಸು;

	/*
	 * If the skip pattern matches before the end pattern,
	 * continue searching after the skip pattern.
	 */
	ಆದರೆ (spp_skip != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ lc_ಲಂಬಸಾಲು = matchcol - spp_skip->sp_offsets[SPO_LC_OFF];
	    ಪೂರ್ಣಾಂಕ r;

	    ಆದರೆ (lc_ಲಂಬಸಾಲು < 0)
		lc_ಲಂಬಸಾಲು = 0;
	    regmatch.rmm_ic = spp_skip->sp_ic;
	    regmatch.regprog = spp_skip->sp_prog;
	    r = syn_regexec(&regmatch, startpos->lnum, lc_ಲಂಬಸಾಲು,
					      IF_SYN_ಸಮಯ(&spp_skip->sp_ಸಮಯ));
	    spp_skip->sp_prog = regmatch.regprog;
	    ಆದರೆ (r && regmatch.startpos[0].ಲಂಬಸಾಲು
					     <= ಉತ್ತಮ_regmatch.startpos[0].ಲಂಬಸಾಲು)
	    {
		ಪೂರ್ಣಾಂಕ ಸಾಲು_len;

		// Add offset to skip pattern match
		syn_ಕೂಡಿಸು_ಕೊನೆ_off(&pos, &regmatch, spp_skip, SPO_ME_OFF, 1);

		// If the skip pattern goes on to the next line, there is no
		// match with an end pattern in this line.
		ಆದರೆ (pos.lnum > startpos->lnum)
		    ನಿಲ್ಲಿಸು;

		ಸಾಲು_len = ml_get_buf_len(syn_buf, startpos->lnum);

		// take care of an empty match or negative offset
		ಆದರೆ (pos.ಲಂಬಸಾಲು <= matchcol)
		    ++matchcol;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pos.ಲಂಬಸಾಲು <= regmatch.endpos[0].ಲಂಬಸಾಲು)
		    matchcol = pos.ಲಂಬಸಾಲು;
		ಇಲ್ಲದಿದ್ದರೆ
		    // Be careful not to jump over the NUL at the end-of-line
		    ಗಾಗಿ (matchcol = regmatch.endpos[0].ಲಂಬಸಾಲು;
			    matchcol < ಸಾಲು_len && matchcol < pos.ಲಂಬಸಾಲು;
								   ++matchcol)
			;

		// if the skip pattern includes end-of-line, break here
		ಆದರೆ (matchcol >= ಸಾಲು_len)
		    ನಿಲ್ಲಿಸು;

		ಮುಂದುವರಿ;	    // start with first end pattern again
	    }
	}

	/*
	 * Match from start pattern to end pattern.
	 * Correct for match and highlight offset of end pattern.
	 */
	spp = &(SYN_ವಸ್ತುಗಳು(syn_ಖಂಡ)[ಉತ್ತಮ_idx]);
	syn_ಕೂಡಿಸು_ಕೊನೆ_off(m_endpos, &ಉತ್ತಮ_regmatch, spp, SPO_ME_OFF, 1);
	// can't end before the start
	ಆದರೆ (m_endpos->lnum == startpos->lnum && m_endpos->ಲಂಬಸಾಲು < startpos->ಲಂಬಸಾಲು)
	    m_endpos->ಲಂಬಸಾಲು = startpos->ಲಂಬಸಾಲು;

	syn_ಕೂಡಿಸು_ಕೊನೆ_off(ಕೊನೆ_endpos, &ಉತ್ತಮ_regmatch, spp, SPO_HE_OFF, 1);
	// can't end before the start
	ಆದರೆ (ಕೊನೆ_endpos->lnum == startpos->lnum
					   && ಕೊನೆ_endpos->ಲಂಬಸಾಲು < startpos->ಲಂಬಸಾಲು)
	    ಕೊನೆ_endpos->ಲಂಬಸಾಲು = startpos->ಲಂಬಸಾಲು;
	// can't end after the match
	ಎಲ್ಲೆ_pos(ಕೊನೆ_endpos, m_endpos);

	/*
	 * If the end group is highlighted differently, adjust the pointers.
	 */
	ಆದರೆ (spp->sp_syn_ಹೊಂದಿಕೆ_id != spp->sp_syn.id && spp->sp_syn_ಹೊಂದಿಕೆ_id != 0)
	{
	    *ಕೊನೆ_idx = ಉತ್ತಮ_idx;
	    ಆದರೆ (spp->sp_off_flags & (1 << (SPO_RE_OFF + SPO_ಎಣಿಕೆ)))
	    {
		hl_endpos->lnum = ಉತ್ತಮ_regmatch.endpos[0].lnum;
		hl_endpos->ಲಂಬಸಾಲು = ಉತ್ತಮ_regmatch.endpos[0].ಲಂಬಸಾಲು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		hl_endpos->lnum = ಉತ್ತಮ_regmatch.startpos[0].lnum;
		hl_endpos->ಲಂಬಸಾಲು = ಉತ್ತಮ_regmatch.startpos[0].ಲಂಬಸಾಲು;
	    }
	    hl_endpos->ಲಂಬಸಾಲು += spp->sp_offsets[SPO_RE_OFF];

	    // can't end before the start
	    ಆದರೆ (hl_endpos->lnum == startpos->lnum
					    && hl_endpos->ಲಂಬಸಾಲು < startpos->ಲಂಬಸಾಲು)
		hl_endpos->ಲಂಬಸಾಲು = startpos->ಲಂಬಸಾಲು;
	    ಎಲ್ಲೆ_pos(hl_endpos, m_endpos);

	    // now the match ends where the highlighting ends, it is turned
	    // into the matchgroup for the end
	    *m_endpos = *hl_endpos;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    *ಕೊನೆ_idx = 0;
	    *hl_endpos = *ಕೊನೆ_endpos;
	}

	*flagsp = spp->sp_flags;

	had_ಹೊಂದಿಕೆ = TRUE;
	ನಿಲ್ಲಿಸು;
    }

    // no match for an END pattern in this line
    ಆದರೆ (!had_ಹೊಂದಿಕೆ)
	m_endpos->lnum = 0;

    restore_chartab(buf_chartab);

    // Remove external matches.
    unref_extmatch(re_extmatch_ಒಳ);
    re_extmatch_ಒಳ = ಖಾಲಿ;
}

/*
 * Limit "pos" not to be after "limit".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಎಲ್ಲೆ_pos(lpos_T *pos, lpos_T *ಎಲ್ಲೆ)
{
    ಆದರೆ (pos->lnum > ಎಲ್ಲೆ->lnum)
	*pos = *ಎಲ್ಲೆ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pos->lnum == ಎಲ್ಲೆ->lnum && pos->ಲಂಬಸಾಲು > ಎಲ್ಲೆ->ಲಂಬಸಾಲು)
	pos->ಲಂಬಸಾಲು = ಎಲ್ಲೆ->ಲಂಬಸಾಲು;
}

/*
 * Limit "pos" not to be after "limit", unless pos->lnum is zero.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಎಲ್ಲೆ_pos_zero(
    lpos_T	*pos,
    lpos_T	*ಎಲ್ಲೆ)
{
    ಆದರೆ (pos->lnum == 0)
	*pos = *ಎಲ್ಲೆ;
    ಇಲ್ಲದಿದ್ದರೆ
	ಎಲ್ಲೆ_pos(pos, ಎಲ್ಲೆ);
}

/*
 * Add offset to matched text for end of match or highlight.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_ಕೂಡಿಸು_ಕೊನೆ_off(
    lpos_T	*ಫಲಿತಾಂಶ,	// returned position
    regmmatch_T	*regmatch,	// start/end of match
    synpat_T	*spp,		// matched pattern
    ಪೂರ್ಣಾಂಕ		idx,		// index of offset
    ಪೂರ್ಣಾಂಕ		extra)		// extra chars for offset to start
{
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		off;
    char_u	*ತಳ;
    char_u	*p;

    ಆದರೆ (spp->sp_off_flags & (1 << idx))
    {
	ಫಲಿತಾಂಶ->lnum = regmatch->startpos[0].lnum;
	ಲಂಬಸಾಲು = regmatch->startpos[0].ಲಂಬಸಾಲು;
	off = spp->sp_offsets[idx] + extra;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಫಲಿತಾಂಶ->lnum = regmatch->endpos[0].lnum;
	ಲಂಬಸಾಲು = regmatch->endpos[0].ಲಂಬಸಾಲು;
	off = spp->sp_offsets[idx];
    }
    // Don't go past the end of the line.  Matters for "rs=e+2" when there
    // is a matchgroup. Watch out for match with last NL in the buffer.
    ಆದರೆ (ಫಲಿತಾಂಶ->lnum > syn_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಲಂಬಸಾಲು = 0;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (off != 0)
    {
	ತಳ = ml_get_buf(syn_buf, ಫಲಿತಾಂಶ->lnum, FALSE);
	p = ತಳ + ಲಂಬಸಾಲು;
	ಆದರೆ (off > 0)
	{
	    ಆಗ (off-- > 0 && *p != NUL)
		MB_PTR_ADV(p);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (off < 0)
	{
	    ಆಗ (off++ < 0 && ತಳ < p)
		MB_PTR_ಹಿಂಭಾಗ(ತಳ, p);
	}
	ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)(p - ತಳ);
    }
    ಫಲಿತಾಂಶ->ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
}

/*
 * Add offset to matched text for start of match or highlight.
 * Avoid resulting column to become negative.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_ಕೂಡಿಸು_ಆರಂಭ_off(
    lpos_T	*ಫಲಿತಾಂಶ,	// returned position
    regmmatch_T	*regmatch,	// start/end of match
    synpat_T	*spp,
    ಪೂರ್ಣಾಂಕ		idx,
    ಪೂರ್ಣಾಂಕ		extra)	    // extra chars for offset to end
{
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		off;
    char_u	*ತಳ;
    char_u	*p;

    ಆದರೆ (spp->sp_off_flags & (1 << (idx + SPO_ಎಣಿಕೆ)))
    {
	ಫಲಿತಾಂಶ->lnum = regmatch->endpos[0].lnum;
	ಲಂಬಸಾಲು = regmatch->endpos[0].ಲಂಬಸಾಲು;
	off = spp->sp_offsets[idx] + extra;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಫಲಿತಾಂಶ->lnum = regmatch->startpos[0].lnum;
	ಲಂಬಸಾಲು = regmatch->startpos[0].ಲಂಬಸಾಲು;
	off = spp->sp_offsets[idx];
    }
    ಆದರೆ (ಫಲಿತಾಂಶ->lnum > syn_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	// a "\n" at the end of the pattern may take us below the last line
	ಫಲಿತಾಂಶ->lnum = syn_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	ಲಂಬಸಾಲು = ml_get_buf_len(syn_buf, ಫಲಿತಾಂಶ->lnum);
    }
    ಆದರೆ (off != 0)
    {
	ತಳ = ml_get_buf(syn_buf, ಫಲಿತಾಂಶ->lnum, FALSE);
	p = ತಳ + ಲಂಬಸಾಲು;
	ಆದರೆ (off > 0)
	{
	    ಆಗ (off-- && *p != NUL)
		MB_PTR_ADV(p);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (off < 0)
	{
	    ಆಗ (off++ && ತಳ < p)
		MB_PTR_ಹಿಂಭಾಗ(ತಳ, p);
	}
	ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)(p - ತಳ);
    }
    ಫಲಿತಾಂಶ->ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
}

/*
 * Get current line in syntax buffer.
 */
    ಸ್ಥಿರ char_u *
syn_getcurline(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ml_get_buf(syn_buf, current_lnum, FALSE);
}

/*
 * Get length of current line in syntax buffer.
 */
    ಸ್ಥಿರ colnr_T
syn_getcurline_len(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ml_get_buf_len(syn_buf, current_lnum);
}

/*
 * Call vim_regexec() to find a match with "rmp" in "syn_buf".
 * Returns TRUE when there is a match.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
syn_regexec(
    regmmatch_T	*rmp,
    linenr_T	lnum,
    colnr_T	ಲಂಬಸಾಲು,
    syn_ಸಮಯ_T  *st ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		r;
    ಪೂರ್ಣಾಂಕ		timed_ಹೊರಹೋಗು = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    proftime_T	pt;

    ಆದರೆ (syn_ಸಮಯ_on)
	profile_ಆರಂಭ(&pt);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (rmp->regprog == ಖಾಲಿ)
	// This can happen if a previous call to vim_regexec_multi() tried to
	// use the NFA engine, which resulted in NFA_TOO_EXPENSIVE, and
	// compiling the pattern with the other engine fails.
	ಹಿಂತಿರುಗಿ FALSE;

    rmp->rmm_maxcol = syn_buf->b_p_smc;
    r = vim_regexec_multi(rmp, syn_win, syn_buf, lnum, ಲಂಬಸಾಲು, &timed_ಹೊರಹೋಗು);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    ಆದರೆ (syn_ಸಮಯ_on)
    {
	profile_ಕೊನೆ(&pt);
	profile_ಕೂಡಿಸು(&st->ಒಟ್ಟು, &pt);
	ಆದರೆ (profile_cmp(&pt, &st->slowest) < 0)
	    st->slowest = pt;
	++st->ಎಣಿಕೆ_2;
	ಆದರೆ (r > 0)
	    ++st->ಹೊಂದಿಕೆ;
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
    ಆದರೆ (timed_ಹೊರಹೋಗು && redrawtime_ಎಲ್ಲೆ_ಗಣ && !syn_win->w_s->b_syn_slow)
    {
	syn_win->w_s->b_syn_slow = TRUE;
	ಸಂದೇಶ(_("'redrawtime' exceeded, syntax highlighting disabled"));
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (r > 0)
    {
	rmp->startpos[0].lnum += lnum;
	rmp->endpos[0].lnum += lnum;
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Check one position in a line for a matching keyword.
 * The caller must check if a keyword can start at startcol.
 * Return its ID if found, 0 otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_keyword_id(
    char_u	*ಸಾಲು,
    ಪೂರ್ಣಾಂಕ		startcol,	// position in line to check for keyword
    ಪೂರ್ಣಾಂಕ		*endcolp,	// return: character after found keyword
    ದೀರ್ಘ	*flagsp,	// return: flags of matching keyword
    ಗಿಡ್ಡ	**ಮುಂದಿನ_listp,	// return: next_list of matching keyword
    stateitem_T	*cur_si,	// item at the top of the stack
    ಪೂರ್ಣಾಂಕ		*ccharp ಬಳಸದ)	// conceal substitution char
{
    keyentry_T	*kp;
    char_u	*kwp;
    ಪೂರ್ಣಾಂಕ		ಸುತ್ತು;
    ಪೂರ್ಣಾಂಕ		kwlen;
    char_u	keyword[MAXKEYWLEN + 1]; // assume max. keyword len is 80
    hashtab_T	*ht;
    hashitem_T	*hi;

    // Find first character after the keyword.  First character was already
    // checked.
    kwp = ಸಾಲು + startcol;
    kwlen = 0;
    ಮಾಡು
    {
	ಆದರೆ (has_mbyte)
	    kwlen += (*mb_ptr2len)(kwp + kwlen);
	ಇಲ್ಲದಿದ್ದರೆ
	    ++kwlen;
    }
    ಆಗ (vim_iswordp_buf(kwp + kwlen, syn_buf));

    ಆದರೆ (kwlen > MAXKEYWLEN)
	ಹಿಂತಿರುಗಿ 0;

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
    ಗಾಗಿ (ಸುತ್ತು = 1; ಸುತ್ತು <= 2; ++ಸುತ್ತು)
    {
	ht = ಸುತ್ತು == 1 ? &syn_ಖಂಡ->b_keywtab : &syn_ಖಂಡ->b_keywtab_ic;
	ಆದರೆ (ht->ht_ಬಳಸಿದ == 0)
	    ಮುಂದುವರಿ;
	ಆದರೆ (ಸುತ್ತು == 2)	// ignore case
	    (ಶೂನ್ಯ)str_foldcase(kwp, kwlen, keyword, MAXKEYWLEN + 1);

	/*
	 * Find keywords that match.  There can be several with different
	 * attributes.
	 * When current_next_list is non-zero accept only that group, otherwise:
	 *  Accept a not-contained keyword at toplevel.
	 *  Accept a keyword at other levels only if it is in the contains list.
	 */
	hi = hash_ಕಂಡುಹಿಡಿ(ht, keyword);
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	    ಗಾಗಿ (kp = HI2KE(hi); kp != ಖಾಲಿ; kp = kp->ke_ಮುಂದಿನ)
	    {
		ಆದರೆ (current_ಮುಂದಿನ_list != 0
			? ಒಳ_id_list(ಖಾಲಿ, current_ಮುಂದಿನ_list, &kp->k_syn, 0)
			: (cur_si == ಖಾಲಿ
			    ? !(kp->flags & HL_CONTAINED)
			    : ಒಳ_id_list(cur_si, cur_si->si_cont_list,
				      &kp->k_syn, kp->flags)))
		{
		    *endcolp = startcol + kwlen;
		    *flagsp = kp->flags;
		    *ಮುಂದಿನ_listp = kp->ಮುಂದಿನ_list;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
		    *ccharp = kp->k_char;
#ಆದರೆ_ಅಂತ್ಯ
		    ಹಿಂತಿರುಗಿ kp->k_syn.id;
		}
	    }
    }
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Handle ":syntax conceal" command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_conceal(exarg_T *eap ಬಳಸದ, ಪೂರ್ಣಾಂಕ syncing ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    char_u	*arg = eap->arg;
    char_u	*ಮುಂದಿನ;

    eap->nextcmd = ಕಂಡುಹಿಡಿ_nextcmd(arg);
    ಆದರೆ (eap->skip)
	ಹಿಂತಿರುಗಿ;

    ಮುಂದಿನ = skiptowhite(arg);
    ಆದರೆ (*arg == NUL)
    {
	ಆದರೆ (curwin->w_s->b_syn_conceal)
	    ಸಂದೇಶ("syntax conceal on");
	ಇಲ್ಲದಿದ್ದರೆ
	    ಸಂದೇಶ("syntax conceal off");
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(arg, "on", 2) == 0 && ಮುಂದಿನ - arg == 2)
	curwin->w_s->b_syn_conceal = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(arg, "off", 3) == 0 && ಮುಂದಿನ - arg == 3)
	curwin->w_s->b_syn_conceal = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ
	semsg(_(e_illegal_ವಾದ_str_2), arg);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Handle ":syntax case" command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_case(exarg_T *eap, ಪೂರ್ಣಾಂಕ syncing ಬಳಸದ)
{
    char_u	*arg = eap->arg;
    char_u	*ಮುಂದಿನ;

    eap->nextcmd = ಕಂಡುಹಿಡಿ_nextcmd(arg);
    ಆದರೆ (eap->skip)
	ಹಿಂತಿರುಗಿ;

    ಮುಂದಿನ = skiptowhite(arg);
    ಆದರೆ (*arg == NUL)
    {
	ಆದರೆ (curwin->w_s->b_syn_ic)
	    ಸಂದೇಶ("syntax case ignore");
	ಇಲ್ಲದಿದ್ದರೆ
	    ಸಂದೇಶ("syntax case match");
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(arg, "match", 5) == 0 && ಮುಂದಿನ - arg == 5)
	curwin->w_s->b_syn_ic = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(arg, "ignore", 6) == 0 && ಮುಂದಿನ - arg == 6)
	curwin->w_s->b_syn_ic = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
	semsg(_(e_illegal_ವಾದ_str_2), arg);
}

/*
 * Handle ":syntax foldlevel" command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_foldlevel(exarg_T *eap, ಪೂರ್ಣಾಂಕ syncing ಬಳಸದ)
{
    char_u *arg = eap->arg;
    char_u *arg_ಕೊನೆ;

    eap->nextcmd = ಕಂಡುಹಿಡಿ_nextcmd(arg);
    ಆದರೆ (eap->skip)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (*arg == NUL)
    {
	ಆಯ್ಕೆ (curwin->w_s->b_syn_foldlevel)
	{
	    ಸ್ಥಿತಿ SYNFLD_ಆರಂಭ:   ಸಂದೇಶ("syntax foldlevel start");   ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ SYNFLD_MINIMUM: ಸಂದೇಶ("syntax foldlevel minimum"); ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ: ನಿಲ್ಲಿಸು;
	}
	ಹಿಂತಿರುಗಿ;
    }

    arg_ಕೊನೆ = skiptowhite(arg);
    ಆದರೆ (STRNICMP(arg, "start", 5) == 0 && arg_ಕೊನೆ - arg == 5)
	curwin->w_s->b_syn_foldlevel = SYNFLD_ಆರಂಭ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(arg, "minimum", 7) == 0 && arg_ಕೊನೆ - arg == 7)
	curwin->w_s->b_syn_foldlevel = SYNFLD_MINIMUM;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	semsg(_(e_illegal_ವಾದ_str_2), arg);
	ಹಿಂತಿರುಗಿ;
    }

    arg = skipwhite(arg_ಕೊನೆ);
    ಆದರೆ (*arg != NUL)
    {
	semsg(_(e_illegal_ವಾದ_str_2), arg);
    }
}

/*
 * Handle ":syntax spell" command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_spell(exarg_T *eap, ಪೂರ್ಣಾಂಕ syncing ಬಳಸದ)
{
    char_u	*arg = eap->arg;
    char_u	*ಮುಂದಿನ;

    eap->nextcmd = ಕಂಡುಹಿಡಿ_nextcmd(arg);
    ಆದರೆ (eap->skip)
	ಹಿಂತಿರುಗಿ;

    ಮುಂದಿನ = skiptowhite(arg);
    ಆದರೆ (*arg == NUL)
    {
	ಆದರೆ (curwin->w_s->b_syn_spell == SYNSPL_ತುದಿ)
	    ಸಂದೇಶ("syntax spell toplevel");
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_s->b_syn_spell == SYNSPL_NOTOP)
	    ಸಂದೇಶ("syntax spell notoplevel");
	ಇಲ್ಲದಿದ್ದರೆ
	    ಸಂದೇಶ("syntax spell default");
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(arg, "toplevel", 8) == 0 && ಮುಂದಿನ - arg == 8)
	curwin->w_s->b_syn_spell = SYNSPL_ತುದಿ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(arg, "notoplevel", 10) == 0 && ಮುಂದಿನ - arg == 10)
	curwin->w_s->b_syn_spell = SYNSPL_NOTOP;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(arg, "default", 7) == 0 && ಮುಂದಿನ - arg == 7)
	curwin->w_s->b_syn_spell = SYNSPL_DEFAULT;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	semsg(_(e_illegal_ವಾದ_str_2), arg);
	ಹಿಂತಿರುಗಿ;
    }

    // assume spell checking changed, force a redraw
    redraw_win_later(curwin, UPD_NOT_VALID);
}

/*
 * Handle ":syntax iskeyword" command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_iskeyword(exarg_T *eap, ಪೂರ್ಣಾಂಕ syncing ಬಳಸದ)
{
    char_u	*arg = eap->arg;
    char_u	save_chartab[32];
    char_u	*save_isk;

    ಆದರೆ (eap->skip)
	ಹಿಂತಿರುಗಿ;

    arg = skipwhite(arg);
    ಆದರೆ (*arg == NUL)
    {
	ಸಂದೇಶ_puts("\n");
	ಆದರೆ (curwin->w_s->b_syn_isk != ಖಾಲಿಗಳು_option)
	{
	    ಸಂದೇಶ_puts("syntax iskeyword ");
	    ಸಂದೇಶ_outtrans(curwin->w_s->b_syn_isk);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಸಂದೇಶ_outtrans((char_u *)_("syntax iskeyword not set"));
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (STRNICMP(arg, "clear", 5) == 0)
	{
	    mch_memmove(curwin->w_s->b_syn_chartab, curbuf->b_chartab,
								  (size_t)32);
	    clear_string_option(&curwin->w_s->b_syn_isk);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (check_isopt(arg) == FAIL)
	    {
		emsg(_(e_invalid_ವಾದ));
		ಹಿಂತಿರುಗಿ;
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
    ಶೂನ್ಯ
syntax_clear(synblock_T *ಖಂಡ)
{
    ಪೂರ್ಣಾಂಕ i;

    ಖಂಡ->b_syn_error = FALSE;	    // clear previous error
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
    ಖಂಡ->b_syn_slow = FALSE;	    // clear previous timeout
#ಆದರೆ_ಅಂತ್ಯ
    ಖಂಡ->b_syn_ic = FALSE;	    // Use case, by default
    ಖಂಡ->b_syn_foldlevel = SYNFLD_ಆರಂಭ;
    ಖಂಡ->b_syn_spell = SYNSPL_DEFAULT; // default spell checking
    ಖಂಡ->b_syn_containedin = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    ಖಂಡ->b_syn_conceal = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    // free the keywords
    clear_keywtab(&ಖಂಡ->b_keywtab);
    clear_keywtab(&ಖಂಡ->b_keywtab_ic);

    // free the syntax patterns
    ಗಾಗಿ (i = ಖಂಡ->b_syn_patterns.ga_len; --i >= 0; )
	syn_clear_ನಮೂನೆ(ಖಂಡ, i);
    ga_clear(&ಖಂಡ->b_syn_patterns);

    // free the syntax clusters
    ಗಾಗಿ (i = ಖಂಡ->b_syn_clusters.ga_len; --i >= 0; )
	syn_clear_cluster(ಖಂಡ, i);
    ga_clear(&ಖಂಡ->b_syn_clusters);
    ಖಂಡ->b_spell_cluster_id = 0;
    ಖಂಡ->b_nospell_cluster_id = 0;

    ಖಂಡ->b_syn_sync_flags = 0;
    ಖಂಡ->b_syn_sync_minlines = 0;
    ಖಂಡ->b_syn_sync_maxlines = 0;
    ಖಂಡ->b_syn_sync_linebreaks = 0;

    vim_regfree(ಖಂಡ->b_syn_linecont_prog);
    ಖಂಡ->b_syn_linecont_prog = ಖಾಲಿ;
    VIM_CLEAR(ಖಂಡ->b_syn_linecont_pat);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಖಂಡ->b_syn_folditems = 0;
#ಆದರೆ_ಅಂತ್ಯ
    clear_string_option(&ಖಂಡ->b_syn_isk);

    // free the stored states
    syn_ರಾಶಿ_free_all(ಖಂಡ);
    invalidate_current_state();

    // Reset the counter for ":syn include"
    running_syn_inc_ಟ್ಯಾಗ್ = 0;
}

/*
 * Get rid of ownsyntax for window "wp".
 */
    ಶೂನ್ಯ
reset_synblock(win_T *wp)
{
    ಆದರೆ (wp->w_s != &wp->w_ಬಫರ್->b_s)
    {
	syntax_clear(wp->w_s);
	vim_free(wp->w_s);
	wp->w_s = &wp->w_ಬಫರ್->b_s;
    }
}

/*
 * Clear syncing info for one buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syntax_sync_clear(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ i;

    // free the syntax patterns
    ಗಾಗಿ (i = curwin->w_s->b_syn_patterns.ga_len; --i >= 0; )
	ಆದರೆ (SYN_ವಸ್ತುಗಳು(curwin->w_s)[i].sp_syncing)
	    syn_remove_ನಮೂನೆ(curwin->w_s, i);

    curwin->w_s->b_syn_sync_flags = 0;
    curwin->w_s->b_syn_sync_minlines = 0;
    curwin->w_s->b_syn_sync_maxlines = 0;
    curwin->w_s->b_syn_sync_linebreaks = 0;

    vim_regfree(curwin->w_s->b_syn_linecont_prog);
    curwin->w_s->b_syn_linecont_prog = ಖಾಲಿ;
    VIM_CLEAR(curwin->w_s->b_syn_linecont_pat);
    clear_string_option(&curwin->w_s->b_syn_isk);

    syn_ರಾಶಿ_free_all(curwin->w_s);	// Need to recompute all syntax.
}

/*
 * Remove one pattern from the buffer's pattern list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_remove_ನಮೂನೆ(
    synblock_T	*ಖಂಡ,
    ಪೂರ್ಣಾಂಕ		idx)
{
    synpat_T	*spp;

    spp = &(SYN_ವಸ್ತುಗಳು(ಖಂಡ)[idx]);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಆದರೆ (spp->sp_flags & HL_FOLD)
	--ಖಂಡ->b_syn_folditems;
#ಆದರೆ_ಅಂತ್ಯ
    syn_clear_ನಮೂನೆ(ಖಂಡ, idx);
    mch_memmove(spp, spp + 1,
		   ಗಾತ್ರ(synpat_T) * (ಖಂಡ->b_syn_patterns.ga_len - idx - 1));
    --ಖಂಡ->b_syn_patterns.ga_len;
}

/*
 * Clear and free one syntax pattern.  When clearing all, must be called from
 * last to first!
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_clear_ನಮೂನೆ(synblock_T *ಖಂಡ, ಪೂರ್ಣಾಂಕ i)
{
    vim_free(SYN_ವಸ್ತುಗಳು(ಖಂಡ)[i].sp_ನಮೂನೆ);
    vim_regfree(SYN_ವಸ್ತುಗಳು(ಖಂಡ)[i].sp_prog);
    // Only free sp_cont_list and sp_next_list of first start pattern
    ಆದರೆ (i == 0 || SYN_ವಸ್ತುಗಳು(ಖಂಡ)[i - 1].sp_type != SPTYPE_ಆರಂಭ)
    {
	vim_free(SYN_ವಸ್ತುಗಳು(ಖಂಡ)[i].sp_cont_list);
	vim_free(SYN_ವಸ್ತುಗಳು(ಖಂಡ)[i].sp_ಮುಂದಿನ_list);
	vim_free(SYN_ವಸ್ತುಗಳು(ಖಂಡ)[i].sp_syn.cont_ಒಳ_list);
    }
}

/*
 * Clear and free one syntax cluster.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_clear_cluster(synblock_T *ಖಂಡ, ಪೂರ್ಣಾಂಕ i)
{
    vim_free(SYN_CLSTR(ಖಂಡ)[i].scl_name);
    vim_free(SYN_CLSTR(ಖಂಡ)[i].scl_name_u);
    vim_free(SYN_CLSTR(ಖಂಡ)[i].scl_list);
}

/*
 * Handle ":syntax clear" command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_clear(exarg_T *eap, ಪೂರ್ಣಾಂಕ syncing)
{
    char_u	*arg = eap->arg;
    char_u	*arg_ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ		id;

    eap->nextcmd = ಕಂಡುಹಿಡಿ_nextcmd(arg);
    ಆದರೆ (eap->skip)
	ಹಿಂತಿರುಗಿ;

    /*
     * We have to disable this within ":syn include @group filename",
     * because otherwise @group would get deleted.
     * Only required for Vim 5.x syntax files, 6.0 ones don't contain ":syn
     * clear".
     */
    ಆದರೆ (curwin->w_s->b_syn_topgrp != 0)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ತುದಿಗಳು_excmd2(eap->cmd, arg))
    {
	/*
	 * No argument: Clear all syntax items.
	 */
	ಆದರೆ (syncing)
	    syntax_sync_clear();
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    syntax_clear(curwin->w_s);
	    ಆದರೆ (curwin->w_s == &curwin->w_ಬಫರ್->b_s)
		do_unlet((char_u *)"b:current_syntax", TRUE);
	    do_unlet((char_u *)"w:current_syntax", TRUE);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	/*
	 * Clear the group IDs that are in the argument.
	 */
	ಆಗ (!ತುದಿಗಳು_excmd2(eap->cmd, arg))
	{
	    arg_ಕೊನೆ = skiptowhite(arg);
	    ಆದರೆ (*arg == '@')
	    {
		id = syn_scl_namen2id(arg + 1, (ಪೂರ್ಣಾಂಕ)(arg_ಕೊನೆ - arg - 1));
		ಆದರೆ (id == 0)
		{
		    semsg(_(e_no_such_syntax_cluster_str_1), arg);
		    ನಿಲ್ಲಿಸು;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    /*
		     * We can't physically delete a cluster without changing
		     * the IDs of other clusters, so we do the next best thing
		     * and make it empty.
		     */
		    ಗಿಡ್ಡ scl_id = id - SYNID_CLUSTER;

		    VIM_CLEAR(SYN_CLSTR(curwin->w_s)[scl_id].scl_list);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		id = syn_namen2id(arg, (ಪೂರ್ಣಾಂಕ)(arg_ಕೊನೆ - arg));
		ಆದರೆ (id == 0)
		{
		    semsg(_(e_no_such_highlight_ಗುಂಪು_name_str), arg);
		    ನಿಲ್ಲಿಸು;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    syn_clear_one(id, syncing);
	    }
	    arg = skipwhite(arg_ಕೊನೆ);
	}
    }
    redraw_curbuf_later(UPD_SOME_VALID);
    syn_ರಾಶಿ_free_all(curwin->w_s);		// Need to recompute all syntax.
}

/*
 * Clear one syntax group for the current buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_clear_one(ಪೂರ್ಣಾಂಕ id, ಪೂರ್ಣಾಂಕ syncing)
{
    synpat_T	*spp;
    ಪೂರ್ಣಾಂಕ		idx;

    // Clear keywords only when not ":syn sync clear group-name"
    ಆದರೆ (!syncing)
    {
	(ಶೂನ್ಯ)syn_clear_keyword(id, &curwin->w_s->b_keywtab);
	(ಶೂನ್ಯ)syn_clear_keyword(id, &curwin->w_s->b_keywtab_ic);
    }

    // clear the patterns for "id"
    ಗಾಗಿ (idx = curwin->w_s->b_syn_patterns.ga_len; --idx >= 0; )
    {
	spp = &(SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx]);
	ಆದರೆ (spp->sp_syn.id != id || spp->sp_syncing != syncing)
	    ಮುಂದುವರಿ;
	syn_remove_ನಮೂನೆ(curwin->w_s, idx);
    }
}

/*
 * Handle ":syntax on" command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_on(exarg_T *eap, ಪೂರ್ಣಾಂಕ syncing ಬಳಸದ)
{
    syn_cmd_onoff(eap, "syntax");
}

/*
 * Handle ":syntax enable" command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_enable(exarg_T *eap, ಪೂರ್ಣಾಂಕ syncing ಬಳಸದ)
{
    ಗಣ_ಆಂತರಿಕ_string_var((char_u *)"g:syntax_cmd", (char_u *)"enable");
    syn_cmd_onoff(eap, "syntax");
    do_unlet((char_u *)"g:syntax_cmd", TRUE);
}

/*
 * Handle ":syntax reset" command.
 * It actually resets highlighting, not syntax.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_reset(exarg_T *eap, ಪೂರ್ಣಾಂಕ syncing ಬಳಸದ)
{
    ಗಣ_nextcmd(eap, eap->arg);
    ಆದರೆ (!eap->skip)
    {
	ಗಣ_ಆಂತರಿಕ_string_var((char_u *)"g:syntax_cmd", (char_u *)"reset");
	do_cmdline_cmd((char_u *)"runtime! syntax/syncolor.vim");
	do_unlet((char_u *)"g:syntax_cmd", TRUE);
    }
}

/*
 * Handle ":syntax manual" command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_manual(exarg_T *eap, ಪೂರ್ಣಾಂಕ syncing ಬಳಸದ)
{
    syn_cmd_onoff(eap, "manual");
}

/*
 * Handle ":syntax off" command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_off(exarg_T *eap, ಪೂರ್ಣಾಂಕ syncing ಬಳಸದ)
{
    syn_cmd_onoff(eap, "nosyntax");
}

    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_onoff(exarg_T *eap, ಅಕ್ಷರ *name)
{
    char_u	buf[100];

    ಗಣ_nextcmd(eap, eap->arg);
    ಆದರೆ (!eap->skip)
    {
	STRCPY(buf, "so ");
	vim_snprintf((ಅಕ್ಷರ *)buf + 3, ಗಾತ್ರ(buf) - 3, SYNTAX_FNAME, name);
	do_cmdline_cmd(buf);
    }
}

/*
 * Handle ":syntax [list]" command: list current syntax words.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_list(
    exarg_T	*eap,
    ಪೂರ್ಣಾಂಕ		syncing)	    // when TRUE: list syncing items
{
    char_u	*arg = eap->arg;
    ಪೂರ್ಣಾಂಕ		id;
    char_u	*arg_ಕೊನೆ;

    eap->nextcmd = ಕಂಡುಹಿಡಿ_nextcmd(arg);
    ಆದರೆ (eap->skip)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (!syntax_present(curwin))
    {
	ಸಂದೇಶ(_(ಸಂದೇಶ_no_ವಸ್ತುಗಳು));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (syncing)
    {
	ಆದರೆ (curwin->w_s->b_syn_sync_flags & SF_CCOMMENT)
	{
	    ಸಂದೇಶ_puts(_("syncing on C-style comments"));
	    syn_ಸಾಲುಗಳು_ಸಂದೇಶ();
	    syn_ಹೊಂದಿಕೆ_ಸಂದೇಶ();
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_s->b_syn_sync_flags & SF_ಹೊಂದಿಕೆ)
	{
	    ಸಂದೇಶ_puts_title(_("\n--- Syntax sync items ---"));
	    ಆದರೆ (curwin->w_s->b_syn_sync_minlines > 0
		|| curwin->w_s->b_syn_sync_maxlines > 0
		|| curwin->w_s->b_syn_sync_linebreaks > 0)
	    {
		ಸಂದೇಶ_puts(_("\nsyncing on items"));
		syn_ಸಾಲುಗಳು_ಸಂದೇಶ();
		syn_ಹೊಂದಿಕೆ_ಸಂದೇಶ();
	    }
	    ಗಾಗಿ (id = 1; id <= highlight_ಅಂಕೆ_groups() && !got_int; ++id)
		syn_list_one(id, syncing, FALSE);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (curwin->w_s->b_syn_sync_minlines == 0)
		ಸಂದೇಶ_puts(_("no syncing"));
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (curwin->w_s->b_syn_sync_minlines == MAXLNUM)
		    ಸಂದೇಶ_puts(_("syncing starts at the first line"));
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಸಂದೇಶ_puts(_("syncing starts "));
		    ಸಂದೇಶ_outnum(curwin->w_s->b_syn_sync_minlines);
		    ಸಂದೇಶ_puts(_(" lines before top line"));
		}
		syn_ಹೊಂದಿಕೆ_ಸಂದೇಶ();
	    }
	}
	ಹಿಂತಿರುಗಿ;
    }

    ಸಂದೇಶ_puts_title(_("\n--- Syntax items ---"));
    ಆದರೆ (ತುದಿಗಳು_excmd2(eap->cmd, arg))
    {
	/*
	 * No argument: List all group IDs and all syntax clusters.
	 */
	ಗಾಗಿ (id = 1; id <= highlight_ಅಂಕೆ_groups() && !got_int; ++id)
	    syn_list_one(id, syncing, FALSE);
	ಗಾಗಿ (id = 0; id < curwin->w_s->b_syn_clusters.ga_len && !got_int; ++id)
	    syn_list_cluster(id);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	/*
	 * List the group IDs and syntax clusters that are in the argument.
	 */
	ಆಗ (!ತುದಿಗಳು_excmd2(eap->cmd, arg) && !got_int)
	{
	    arg_ಕೊನೆ = skiptowhite(arg);
	    ಆದರೆ (*arg == '@')
	    {
		id = syn_scl_namen2id(arg + 1, (ಪೂರ್ಣಾಂಕ)(arg_ಕೊನೆ - arg - 1));
		ಆದರೆ (id == 0)
		    semsg(_(e_no_such_syntax_cluster_str_2), arg);
		ಇಲ್ಲದಿದ್ದರೆ
		    syn_list_cluster(id - SYNID_CLUSTER);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		id = syn_namen2id(arg, (ಪೂರ್ಣಾಂಕ)(arg_ಕೊನೆ - arg));
		ಆದರೆ (id == 0)
		    semsg(_(e_no_such_highlight_ಗುಂಪು_name_str), arg);
		ಇಲ್ಲದಿದ್ದರೆ
		    syn_list_one(id, syncing, TRUE);
	    }
	    arg = skipwhite(arg_ಕೊನೆ);
	}
    }
    ಗಣ_nextcmd(eap, arg);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
syn_ಸಾಲುಗಳು_ಸಂದೇಶ(ಶೂನ್ಯ)
{
    ಆದರೆ (curwin->w_s->b_syn_sync_maxlines > 0
				      || curwin->w_s->b_syn_sync_minlines > 0)
    {
	ಸಂದೇಶ_puts("; ");
	ಆದರೆ (curwin->w_s->b_syn_sync_minlines == MAXLNUM)
	    ಸಂದೇಶ_puts(_("from the first line"));
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (curwin->w_s->b_syn_sync_minlines > 0)
	    {
		ಸಂದೇಶ_puts(_("minimal "));
		ಸಂದೇಶ_outnum(curwin->w_s->b_syn_sync_minlines);
		ಆದರೆ (curwin->w_s->b_syn_sync_maxlines)
		    ಸಂದೇಶ_puts(", ");
	    }
	    ಆದರೆ (curwin->w_s->b_syn_sync_maxlines > 0)
	    {
		ಸಂದೇಶ_puts(_("maximal "));
		ಸಂದೇಶ_outnum(curwin->w_s->b_syn_sync_maxlines);
	    }
	    ಸಂದೇಶ_puts(_(" lines before top line"));
	}
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
syn_ಹೊಂದಿಕೆ_ಸಂದೇಶ(ಶೂನ್ಯ)
{
    ಆದರೆ (curwin->w_s->b_syn_sync_linebreaks > 0)
    {
	ಸಂದೇಶ_puts(_("; match "));
	ಸಂದೇಶ_outnum(curwin->w_s->b_syn_sync_linebreaks);
	ಸಂದೇಶ_puts(_(" line breaks"));
    }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  ಕೊನೆಯ_matchgroup;

ಸ್ಥಿರ ಶೂನ್ಯ syn_list_flags(keyvalue_T *nlist, ಪೂರ್ಣಾಂಕ nr_entries, ಪೂರ್ಣಾಂಕ flags, ಪೂರ್ಣಾಂಕ attr);

/*
 * List one syntax item, for ":syntax" or "syntax list syntax_name".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_list_one(
    ಪೂರ್ಣಾಂಕ		id,
    ಪೂರ್ಣಾಂಕ		syncing,	    // when TRUE: list syncing items
    ಪೂರ್ಣಾಂಕ		link_only)	    // when TRUE; list link-only too
{
    ಪೂರ್ಣಾಂಕ		attr;
    ಪೂರ್ಣಾಂಕ		idx;
    ಪೂರ್ಣಾಂಕ		did_ಶೀರ್ಷಿಕೆ = FALSE;
    synpat_T	*spp;
    ಸ್ಥಿರ keyvalue_T namelist1[] =
		{
		    KEYVALUE_ನಮೂದು(HL_DISPLAY, "display"),
		    KEYVALUE_ನಮೂದು(HL_CONTAINED, "contained"),
		    KEYVALUE_ನಮೂದು(HL_ONELINE, "oneline"),
		    KEYVALUE_ನಮೂದು(HL_KEEPEND, "keepend"),
		    KEYVALUE_ನಮೂದು(HL_EXTEND, "extend"),
		    KEYVALUE_ನಮೂದು(HL_EXCLUDENL, "excludenl"),
		    KEYVALUE_ನಮೂದು(HL_TRANSP, "transparent"),
		    KEYVALUE_ನಮೂದು(HL_FOLD, "fold")
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
		    ,
		    KEYVALUE_ನಮೂದು(HL_CONCEAL, "conceal"),
		    KEYVALUE_ನಮೂದು(HL_CONCEALENDS, "concealends")
#ಆದರೆ_ಅಂತ್ಯ
		};
    ಸ್ಥಿರ keyvalue_T namelist2[] =
		{
		    KEYVALUE_ನಮೂದು(HL_SKIPWHITE, "skipwhite"),
		    KEYVALUE_ನಮೂದು(HL_SKIPNL, "skipnl"),
		    KEYVALUE_ನಮೂದು(HL_SKIPEMPTY, "skipempty")
		};

    attr = HL_ATTR(HLF_D);		// highlight like directories

    // list the keywords for "id"
    ಆದರೆ (!syncing)
    {
	did_ಶೀರ್ಷಿಕೆ = syn_list_keywords(id, &curwin->w_s->b_keywtab, FALSE, attr);
	did_ಶೀರ್ಷಿಕೆ = syn_list_keywords(id, &curwin->w_s->b_keywtab_ic,
							    did_ಶೀರ್ಷಿಕೆ, attr);
    }

    // list the patterns for "id"
    ಗಾಗಿ (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len && !got_int; ++idx)
    {
	spp = &(SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx]);
	ಆದರೆ (spp->sp_syn.id != id || spp->sp_syncing != syncing)
	    ಮುಂದುವರಿ;

	(ಶೂನ್ಯ)syn_list_ಶೀರ್ಷಿಕೆ(did_ಶೀರ್ಷಿಕೆ, 999, id);
	did_ಶೀರ್ಷಿಕೆ = TRUE;
	ಕೊನೆಯ_matchgroup = 0;
	ಆದರೆ (spp->sp_type == SPTYPE_ಹೊಂದಿಕೆ)
	{
	    put_ನಮೂನೆ("match", ' ', spp, attr);
	    ಸಂದೇಶ_putchar(' ');
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (spp->sp_type == SPTYPE_ಆರಂಭ)
	{
	    ಆಗ (SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_type == SPTYPE_ಆರಂಭ)
		put_ನಮೂನೆ("start", '=', &SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx++], attr);
	    ಆದರೆ (SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_type == SPTYPE_SKIP)
		put_ನಮೂನೆ("skip", '=', &SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx++], attr);
	    ಆಗ (idx < curwin->w_s->b_syn_patterns.ga_len
			  && SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_type == SPTYPE_ಕೊನೆ)
		put_ನಮೂನೆ("end", '=', &SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx++], attr);
	    --idx;
	    ಸಂದೇಶ_putchar(' ');
	}
	syn_list_flags(namelist1, (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(namelist1), spp->sp_flags, attr);

	ಆದರೆ (spp->sp_cont_list != ಖಾಲಿ)
	    put_id_list((char_u *)"contains", spp->sp_cont_list, attr);

	ಆದರೆ (spp->sp_syn.cont_ಒಳ_list != ಖಾಲಿ)
	    put_id_list((char_u *)"containedin",
					      spp->sp_syn.cont_ಒಳ_list, attr);

	ಆದರೆ (spp->sp_ಮುಂದಿನ_list != ಖಾಲಿ)
	{
	    put_id_list((char_u *)"nextgroup", spp->sp_ಮುಂದಿನ_list, attr);
	    syn_list_flags(namelist2, (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(namelist2), spp->sp_flags, attr);
	}
	ಆದರೆ (spp->sp_flags & (HL_SYNC_ಇಲ್ಲಿ|HL_SYNC_THERE))
	{
	    ಆದರೆ (spp->sp_flags & HL_SYNC_ಇಲ್ಲಿ)
		ಸಂದೇಶ_puts_attr("grouphere", attr);
	    ಇಲ್ಲದಿದ್ದರೆ
		ಸಂದೇಶ_puts_attr("groupthere", attr);
	    ಸಂದೇಶ_putchar(' ');
	    ಆದರೆ (spp->sp_sync_idx >= 0)
		ಸಂದೇಶ_outtrans(highlight_ಗುಂಪು_name(SYN_ವಸ್ತುಗಳು(curwin->w_s)
				   [spp->sp_sync_idx].sp_syn.id - 1));
	    ಇಲ್ಲದಿದ್ದರೆ
		ಸಂದೇಶ_puts("NONE");
	    ಸಂದೇಶ_putchar(' ');
	}
    }

    // list the link, if there is one
    ಆದರೆ (highlight_link_id(id - 1) && (did_ಶೀರ್ಷಿಕೆ || link_only) && !got_int)
    {
	(ಶೂನ್ಯ)syn_list_ಶೀರ್ಷಿಕೆ(did_ಶೀರ್ಷಿಕೆ, 999, id);
	ಸಂದೇಶ_puts_attr("links to", attr);
	ಸಂದೇಶ_putchar(' ');
	ಸಂದೇಶ_outtrans(highlight_ಗುಂಪು_name(highlight_link_id(id - 1) - 1));
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
syn_list_flags(keyvalue_T *nlist, ಪೂರ್ಣಾಂಕ nr_entries, ಪೂರ್ಣಾಂಕ flags, ಪೂರ್ಣಾಂಕ attr)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 0; i < nr_entries; ++i)
	ಆದರೆ (flags & nlist[i].ಕೀಲಿ)
	{
	    ಸಂದೇಶ_puts_attr((ಅಕ್ಷರ *)nlist[i].ಮೌಲ್ಯ.string, attr);
	    ಸಂದೇಶ_putchar(' ');
	}
}

/*
 * List one syntax cluster, for ":syntax" or "syntax list syntax_name".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_list_cluster(ಪೂರ್ಣಾಂಕ id)
{
    ಪೂರ್ಣಾಂಕ	    endcol = 15;

    // slight hack:  roughly duplicate the guts of syn_list_header()
    ಸಂದೇಶ_putchar('\n');
    ಸಂದೇಶ_outtrans(SYN_CLSTR(curwin->w_s)[id].scl_name);

    ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು >= endcol)	// output at least one space
	endcol = ಸಂದೇಶ_ಲಂಬಸಾಲು + 1;
    ಆದರೆ (cmdline_ಅಗಲ <= endcol)	// avoid hang for tiny window
	endcol = cmdline_ಅಗಲ - 1;

    ಸಂದೇಶ_advance(endcol);
    ಆದರೆ (SYN_CLSTR(curwin->w_s)[id].scl_list != ಖಾಲಿ)
    {
	put_id_list((char_u *)"cluster", SYN_CLSTR(curwin->w_s)[id].scl_list,
		    HL_ATTR(HLF_D));
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಸಂದೇಶ_puts_attr("cluster", HL_ATTR(HLF_D));
	ಸಂದೇಶ_puts("=NONE");
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
put_id_list(char_u *name, ಗಿಡ್ಡ *list, ಪೂರ್ಣಾಂಕ attr)
{
    ಗಿಡ್ಡ		*p;

    ಸಂದೇಶ_puts_attr((ಅಕ್ಷರ *)name, attr);
    ಸಂದೇಶ_putchar('=');
    ಗಾಗಿ (p = list; *p; ++p)
    {
	ಆದರೆ (*p >= SYNID_ALLBUT && *p < SYNID_ತುದಿ)
	{
	    ಆದರೆ (p[1])
		ಸಂದೇಶ_puts("ALLBUT");
	    ಇಲ್ಲದಿದ್ದರೆ
		ಸಂದೇಶ_puts("ALL");
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p >= SYNID_ತುದಿ && *p < SYNID_CONTAINED)
	{
	    ಸಂದೇಶ_puts("TOP");
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p >= SYNID_CONTAINED && *p < SYNID_CLUSTER)
	{
	    ಸಂದೇಶ_puts("CONTAINED");
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p >= SYNID_CLUSTER)
	{
	    ಗಿಡ್ಡ scl_id = *p - SYNID_CLUSTER;

	    ಸಂದೇಶ_putchar('@');
	    ಸಂದೇಶ_outtrans(SYN_CLSTR(curwin->w_s)[scl_id].scl_name);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಸಂದೇಶ_outtrans(highlight_ಗುಂಪು_name(*p - 1));
	ಆದರೆ (p[1])
	    ಸಂದೇಶ_putchar(',');
    }
    ಸಂದೇಶ_putchar(' ');
}

    ಸ್ಥಿರ ಶೂನ್ಯ
put_ನಮೂನೆ(
    ಅಕ್ಷರ	*s,
    ಪೂರ್ಣಾಂಕ		c,
    synpat_T	*spp,
    ಪೂರ್ಣಾಂಕ		attr)
{
    ದೀರ್ಘ	n;
    ಪೂರ್ಣಾಂಕ		ಮುಖವಾಡ;
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ;
    ಸ್ಥಿರ ಅಕ್ಷರ	*sepchars = "/+=-#@\"|'^&";
    ಪೂರ್ಣಾಂಕ		i;

    // May have to write "matchgroup=group"
    ಆದರೆ (ಕೊನೆಯ_matchgroup != spp->sp_syn_ಹೊಂದಿಕೆ_id)
    {
	ಕೊನೆಯ_matchgroup = spp->sp_syn_ಹೊಂದಿಕೆ_id;
	ಸಂದೇಶ_puts_attr("matchgroup", attr);
	ಸಂದೇಶ_putchar('=');
	ಆದರೆ (ಕೊನೆಯ_matchgroup == 0)
	    ಸಂದೇಶ_outtrans((char_u *)"NONE");
	ಇಲ್ಲದಿದ್ದರೆ
	    ಸಂದೇಶ_outtrans(highlight_ಗುಂಪು_name(ಕೊನೆಯ_matchgroup - 1));
	ಸಂದೇಶ_putchar(' ');
    }

    // output the name of the pattern and an '=' or ' '
    ಸಂದೇಶ_puts_attr(s, attr);
    ಸಂದೇಶ_putchar(c);

    // output the pattern, in between a char that is not in the pattern
    ಗಾಗಿ (i = 0; vim_strchr(spp->sp_ನಮೂನೆ, sepchars[i]) != ಖಾಲಿ; )
	ಆದರೆ (sepchars[++i] == NUL)
	{
	    i = 0;	// no good char found, just use the first one
	    ನಿಲ್ಲಿಸು;
	}
    ಸಂದೇಶ_putchar(sepchars[i]);
    ಸಂದೇಶ_outtrans(spp->sp_ನಮೂನೆ);
    ಸಂದೇಶ_putchar(sepchars[i]);

    // output any pattern options
    ಮೊದಲ = TRUE;
    ಗಾಗಿ (i = 0; i < SPO_ಎಣಿಕೆ; ++i)
    {
	ಮುಖವಾಡ = (1 << i);
	ಆದರೆ (spp->sp_off_flags & (ಮುಖವಾಡ + (ಮುಖವಾಡ << SPO_ಎಣಿಕೆ)))
	{
	    ಆದರೆ (!ಮೊದಲ)
		ಸಂದೇಶ_putchar(',');	// separate with commas
	    ಸಂದೇಶ_puts(spo_name_tab[i]);
	    n = spp->sp_offsets[i];
	    ಆದರೆ (i != SPO_LC_OFF)
	    {
		ಆದರೆ (spp->sp_off_flags & ಮುಖವಾಡ)
		    ಸಂದೇಶ_putchar('s');
		ಇಲ್ಲದಿದ್ದರೆ
		    ಸಂದೇಶ_putchar('e');
		ಆದರೆ (n > 0)
		    ಸಂದೇಶ_putchar('+');
	    }
	    ಆದರೆ (n || i == SPO_LC_OFF)
		ಸಂದೇಶ_outnum(n);
	    ಮೊದಲ = FALSE;
	}
    }
    ಸಂದೇಶ_putchar(' ');
}

/*
 * List or clear the keywords for one syntax group.
 * Return TRUE if the header has been printed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
syn_list_keywords(
    ಪೂರ್ಣಾಂಕ		id,
    hashtab_T	*ht,
    ಪೂರ್ಣಾಂಕ		did_ಶೀರ್ಷಿಕೆ,		// header has already been printed
    ಪೂರ್ಣಾಂಕ		attr)
{
    ಪೂರ್ಣಾಂಕ		outlen;
    hashitem_T	*hi;
    keyentry_T	*kp;
    ಪೂರ್ಣಾಂಕ		todo;
    ಪೂರ್ಣಾಂಕ		prev_contained = 0;
    ಗಿಡ್ಡ	*prev_ಮುಂದಿನ_list = ಖಾಲಿ;
    ಗಿಡ್ಡ	*prev_cont_ಒಳ_list = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		prev_skipnl = 0;
    ಪೂರ್ಣಾಂಕ		prev_skipwhite = 0;
    ಪೂರ್ಣಾಂಕ		prev_skipempty = 0;

    /*
     * Unfortunately, this list of keywords is not sorted on alphabet but on
     * hash value...
     */
    todo = (ಪೂರ್ಣಾಂಕ)ht->ht_ಬಳಸಿದ;
    ಗಾಗಿ (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    --todo;
	    ಗಾಗಿ (kp = HI2KE(hi); kp != ಖಾಲಿ && !got_int; kp = kp->ke_ಮುಂದಿನ)
	    {
		ಆದರೆ (kp->k_syn.id == id)
		{
		    ಆದರೆ (prev_contained != (kp->flags & HL_CONTAINED)
			    || prev_skipnl != (kp->flags & HL_SKIPNL)
			    || prev_skipwhite != (kp->flags & HL_SKIPWHITE)
			    || prev_skipempty != (kp->flags & HL_SKIPEMPTY)
			    || prev_cont_ಒಳ_list != kp->k_syn.cont_ಒಳ_list
			    || prev_ಮುಂದಿನ_list != kp->ಮುಂದಿನ_list)
			outlen = 9999;
		    ಇಲ್ಲದಿದ್ದರೆ
			outlen = (ಪೂರ್ಣಾಂಕ)STRLEN(kp->keyword);
		    // output "contained" and "nextgroup" on each line
		    ಆದರೆ (syn_list_ಶೀರ್ಷಿಕೆ(did_ಶೀರ್ಷಿಕೆ, outlen, id))
		    {
			prev_contained = 0;
			prev_ಮುಂದಿನ_list = ಖಾಲಿ;
			prev_cont_ಒಳ_list = ಖಾಲಿ;
			prev_skipnl = 0;
			prev_skipwhite = 0;
			prev_skipempty = 0;
		    }
		    did_ಶೀರ್ಷಿಕೆ = TRUE;
		    ಆದರೆ (prev_contained != (kp->flags & HL_CONTAINED))
		    {
			ಸಂದೇಶ_puts_attr("contained", attr);
			ಸಂದೇಶ_putchar(' ');
			prev_contained = (kp->flags & HL_CONTAINED);
		    }
		    ಆದರೆ (kp->k_syn.cont_ಒಳ_list != prev_cont_ಒಳ_list)
		    {
			put_id_list((char_u *)"containedin",
						kp->k_syn.cont_ಒಳ_list, attr);
			ಸಂದೇಶ_putchar(' ');
			prev_cont_ಒಳ_list = kp->k_syn.cont_ಒಳ_list;
		    }
		    ಆದರೆ (kp->ಮುಂದಿನ_list != prev_ಮುಂದಿನ_list)
		    {
			put_id_list((char_u *)"nextgroup", kp->ಮುಂದಿನ_list, attr);
			ಸಂದೇಶ_putchar(' ');
			prev_ಮುಂದಿನ_list = kp->ಮುಂದಿನ_list;
			ಆದರೆ (kp->flags & HL_SKIPNL)
			{
			    ಸಂದೇಶ_puts_attr("skipnl", attr);
			    ಸಂದೇಶ_putchar(' ');
			    prev_skipnl = (kp->flags & HL_SKIPNL);
			}
			ಆದರೆ (kp->flags & HL_SKIPWHITE)
			{
			    ಸಂದೇಶ_puts_attr("skipwhite", attr);
			    ಸಂದೇಶ_putchar(' ');
			    prev_skipwhite = (kp->flags & HL_SKIPWHITE);
			}
			ಆದರೆ (kp->flags & HL_SKIPEMPTY)
			{
			    ಸಂದೇಶ_puts_attr("skipempty", attr);
			    ಸಂದೇಶ_putchar(' ');
			    prev_skipempty = (kp->flags & HL_SKIPEMPTY);
			}
		    }
		    ಸಂದೇಶ_outtrans(kp->keyword);
		}
	    }
	}
    }

    ಹಿಂತಿರುಗಿ did_ಶೀರ್ಷಿಕೆ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
syn_clear_keyword(ಪೂರ್ಣಾಂಕ id, hashtab_T *ht)
{
    hashitem_T	*hi;
    keyentry_T	*kp;
    keyentry_T	*kp_prev;
    keyentry_T	*kp_ಮುಂದಿನ;
    ಪೂರ್ಣಾಂಕ		todo;

    hash_ಬೀಗ(ht);
    todo = (ಪೂರ್ಣಾಂಕ)ht->ht_ಬಳಸಿದ;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(ht, hi, todo)
    {
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    --todo;
	    kp_prev = ಖಾಲಿ;
	    ಗಾಗಿ (kp = HI2KE(hi); kp != ಖಾಲಿ; )
	    {
		ಆದರೆ (kp->k_syn.id == id)
		{
		    kp_ಮುಂದಿನ = kp->ke_ಮುಂದಿನ;
		    ಆದರೆ (kp_prev == ಖಾಲಿ)
		    {
			ಆದರೆ (kp_ಮುಂದಿನ == ಖಾಲಿ)
			    hash_remove(ht, hi, "syntax clear keyword");
			ಇಲ್ಲದಿದ್ದರೆ
			    hi->hi_ಕೀಲಿ = KE2HIKEY(kp_ಮುಂದಿನ);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			kp_prev->ke_ಮುಂದಿನ = kp_ಮುಂದಿನ;
		    vim_free(kp->ಮುಂದಿನ_list);
		    vim_free(kp->k_syn.cont_ಒಳ_list);
		    vim_free(kp);
		    kp = kp_ಮುಂದಿನ;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    kp_prev = kp;
		    kp = kp->ke_ಮುಂದಿನ;
		}
	    }
	}
    }
    hash_unlock(ht);
}

/*
 * Clear a whole keyword table.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clear_keywtab(hashtab_T *ht)
{
    hashitem_T	*hi;
    ಪೂರ್ಣಾಂಕ		todo;
    keyentry_T	*kp;
    keyentry_T	*kp_ಮುಂದಿನ;

    todo = (ಪೂರ್ಣಾಂಕ)ht->ht_ಬಳಸಿದ;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(ht, hi, todo)
    {
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    --todo;
	    ಗಾಗಿ (kp = HI2KE(hi); kp != ಖಾಲಿ; kp = kp_ಮುಂದಿನ)
	    {
		kp_ಮುಂದಿನ = kp->ke_ಮುಂದಿನ;
		vim_free(kp->ಮುಂದಿನ_list);
		vim_free(kp->k_syn.cont_ಒಳ_list);
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
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_keyword(
    char_u	*name,	    // name of keyword
    size_t	namelen,    // length of keyword (excluding the NUL)
    ಪೂರ್ಣಾಂಕ		id,	    // group ID for this keyword
    ಪೂರ್ಣಾಂಕ		flags,	    // flags for this keyword
    ಗಿಡ್ಡ	*cont_ಒಳ_list, // containedin for this keyword
    ಗಿಡ್ಡ	*ಮುಂದಿನ_list, // nextgroup for this keyword
    ಪೂರ್ಣಾಂಕ		conceal_char)
{
    keyentry_T	*kp;
    hashtab_T	*ht;
    hashitem_T	*hi;
    char_u	*name_ic;
    size_t	name_iclen;
    long_u	hash;
    char_u	name_folded[MAXKEYWLEN + 1];

    ಆದರೆ (curwin->w_s->b_syn_ic)
    {
	name_ic = str_foldcase(name, (ಪೂರ್ಣಾಂಕ)namelen,
						 name_folded, MAXKEYWLEN + 1);
	name_iclen = STRLEN(name_ic);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	name_ic = name;
	name_iclen = namelen;
    }
    kp = alloc(offsetof(keyentry_T, keyword) + name_iclen + 1);
    ಆದರೆ (kp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    STRCPY(kp->keyword, name_ic);
    kp->k_syn.id = id;
    kp->k_syn.inc_ಟ್ಯಾಗ್ = current_syn_inc_ಟ್ಯಾಗ್;
    kp->flags = flags;
    kp->k_char = conceal_char;
    kp->k_syn.cont_ಒಳ_list = copy_id_list(cont_ಒಳ_list);
    ಆದರೆ (cont_ಒಳ_list != ಖಾಲಿ)
	curwin->w_s->b_syn_containedin = TRUE;
    kp->ಮುಂದಿನ_list = copy_id_list(ಮುಂದಿನ_list);

    ಆದರೆ (curwin->w_s->b_syn_ic)
	ht = &curwin->w_s->b_keywtab_ic;
    ಇಲ್ಲದಿದ್ದರೆ
	ht = &curwin->w_s->b_keywtab;

    hash = hash_hash(kp->keyword);
    hi = hash_lookup(ht, kp->keyword, hash);
    ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
    {
	// new keyword, add to hashtable
	kp->ke_ಮುಂದಿನ = ಖಾಲಿ;
	hash_ಕೂಡಿಸು_ಅಂಶ(ht, hi, kp->keyword, hash);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// keyword already exists, prepend to list
	kp->ke_ಮುಂದಿನ = HI2KE(hi);
	hi->hi_ಕೀಲಿ = KE2HIKEY(kp);
    }
}

/*
 * Get the start and end of the group name argument.
 * Return a pointer to the first argument.
 * Return NULL if the end of the command was found instead of further args.
 */
    ಸ್ಥಿರ char_u *
get_ಗುಂಪು_name(
    char_u	*arg,		// start of the argument
    char_u	**name_ಕೊನೆ)	// pointer to end of the name
{
    char_u	*rest;

    *name_ಕೊನೆ = skiptowhite(arg);
    rest = skipwhite(*name_ಕೊನೆ);

    /*
     * Check if there are enough arguments.  The first argument may be a
     * pattern, where '|' is allowed, so only check for NUL.
     */
    ಆದರೆ (ತುದಿಗಳು_excmd(*arg) || *rest == NUL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ rest;
}

/*
 * Check for syntax command option arguments.
 * This can be called at any place in the list of arguments, and just picks
 * out the arguments that are known.  Can be called several times in a row to
 * collect all options in between other arguments.
 * Return a pointer to the next argument (which isn't an option).
 * Return NULL for any error;
 */
    ಸ್ಥಿರ char_u *
get_syn_options(
    char_u	    *ಆರಂಭ,		// next argument to be checked
    syn_opt_arg_T   *opt,		// various things
    ಪೂರ್ಣಾಂಕ		    *conceal_char ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		    skip)		// TRUE if skipping over command
{
    char_u	*arg = ಆರಂಭ;
    char_u	*gname_ಆರಂಭ, *gname;
    ಪೂರ್ಣಾಂಕ		syn_id;
    ಪೂರ್ಣಾಂಕ		len;
    ಅಕ್ಷರ	*p;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		fidx;
    ಸ್ಥಿರ ರಚನೆ flag
    {
	ಅಕ್ಷರ	*name;
	ಪೂರ್ಣಾಂಕ	argtype;
	ಪೂರ್ಣಾಂಕ	flags;
    } flagtab[] = { {"cCoOnNtTaAiInNeEdD",	0,	HL_CONTAINED},
		    {"oOnNeElLiInNeE",		0,	HL_ONELINE},
		    {"kKeEeEpPeEnNdD",		0,	HL_KEEPEND},
		    {"eExXtTeEnNdD",		0,	HL_EXTEND},
		    {"eExXcClLuUdDeEnNlL",	0,	HL_EXCLUDENL},
		    {"tTrRaAnNsSpPaArReEnNtT",	0,	HL_TRANSP},
		    {"sSkKiIpPnNlL",		0,	HL_SKIPNL},
		    {"sSkKiIpPwWhHiItTeE",	0,	HL_SKIPWHITE},
		    {"sSkKiIpPeEmMpPtTyY",	0,	HL_SKIPEMPTY},
		    {"gGrRoOuUpPhHeErReE",	0,	HL_SYNC_ಇಲ್ಲಿ},
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
    ಸ್ಥಿರ ಅಕ್ಷರ *ಮೊದಲ_letters = "cCoOkKeEtTsSgGdDfFnN";

    ಆದರೆ (arg == ಖಾಲಿ)		// already detected error
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    ಆದರೆ (curwin->w_s->b_syn_conceal)
	opt->flags |= HL_CONCEAL;
#ಆದರೆ_ಅಂತ್ಯ

    ಗಾಗಿ (;;)
    {
	/*
	 * This is used very often when a large number of keywords is defined.
	 * Need to skip quickly when no option name is found.
	 * Also avoid tolower(), it's slow.
	 */
	ಆದರೆ (strchr(ಮೊದಲ_letters, *arg) == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;

	ಗಾಗಿ (fidx = ARRAY_ಉದ್ದಳತೆ(flagtab); --fidx >= 0; )
	{
	    p = flagtab[fidx].name;
	    ಗಾಗಿ (i = 0, len = 0; p[i] != NUL; i += 2, ++len)
		ಆದರೆ (arg[len] != p[i] && arg[len] != p[i + 1])
		    ನಿಲ್ಲಿಸು;
	    ಆದರೆ (p[i] == NUL && (VIM_ISWHITE(arg[len])
				    || (flagtab[fidx].argtype > 0
					 ? arg[len] == '='
					 : ತುದಿಗಳು_excmd2(ಆರಂಭ, arg + len))))
	    {
		ಆದರೆ (opt->keyword
			&& (flagtab[fidx].flags == HL_DISPLAY
			    || flagtab[fidx].flags == HL_FOLD
			    || flagtab[fidx].flags == HL_EXTEND))
		    // treat "display", "fold" and "extend" as a keyword
		    fidx = -1;
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (fidx < 0)	    // no match found
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (flagtab[fidx].argtype == 1)
	{
	    ಆದರೆ (!opt->has_cont_list)
	    {
		emsg(_(e_ಒಳಗೊಂಡಿದೆ_ವಾದ_not_accepted_ಇಲ್ಲಿ));
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    ಆದರೆ (get_id_list(&arg, 8, &opt->cont_list, skip) == FAIL)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flagtab[fidx].argtype == 2)
	{
	    ಆದರೆ (get_id_list(&arg, 11, &opt->cont_ಒಳ_list, skip) == FAIL)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flagtab[fidx].argtype == 3)
	{
	    ಆದರೆ (get_id_list(&arg, 9, &opt->ಮುಂದಿನ_list, skip) == FAIL)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flagtab[fidx].argtype == 11 && arg[5] == '=')
	{
	    // cchar=?
	    ಆದರೆ (has_mbyte)
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
		*conceal_char = mb_ptr2char(arg + 6);
#ಆದರೆ_ಅಂತ್ಯ
		arg += mb_ptr2len(arg + 6) - 1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
		*conceal_char = arg[6];
#ಇಲ್ಲದಿದ್ದರೆ
		;
#ಆದರೆ_ಅಂತ್ಯ
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
	    ಆದರೆ (!vim_isprintc_strict(*conceal_char))
	    {
		emsg(_(e_invalid_cchar_ಮೌಲ್ಯ));
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    arg = skipwhite(arg + 7);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    opt->flags |= flagtab[fidx].flags;
	    arg = skipwhite(arg + len);

	    ಆದರೆ (flagtab[fidx].flags == HL_SYNC_ಇಲ್ಲಿ
		    || flagtab[fidx].flags == HL_SYNC_THERE)
	    {
		ಆದರೆ (opt->sync_idx == ಖಾಲಿ)
		{
		    emsg(_(e_groupthere_not_accepted_ಇಲ್ಲಿ));
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		gname_ಆರಂಭ = arg;
		arg = skiptowhite(arg);
		ಆದರೆ (gname_ಆರಂಭ == arg)
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		gname = vim_strnsave(gname_ಆರಂಭ, arg - gname_ಆರಂಭ);
		ಆದರೆ (gname == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		ಆದರೆ (STRCMP(gname, "NONE") == 0)
		    *opt->sync_idx = NONE_IDX;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    syn_id = syn_name2id(gname);
		    ಗಾಗಿ (i = curwin->w_s->b_syn_patterns.ga_len; --i >= 0; )
			ಆದರೆ (SYN_ವಸ್ತುಗಳು(curwin->w_s)[i].sp_syn.id == syn_id
			      && SYN_ವಸ್ತುಗಳು(curwin->w_s)[i].sp_type
							       == SPTYPE_ಆರಂಭ)
			{
			    *opt->sync_idx = i;
			    ನಿಲ್ಲಿಸು;
			}
		    ಆದರೆ (i < 0)
		    {
			semsg(_(e_didnt_ಕಂಡುಹಿಡಿ_region_ಅಂಶ_for_str), gname);
			vim_free(gname);
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		    }
		}

		vim_free(gname);
		arg = skipwhite(arg);
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flagtab[fidx].flags == HL_FOLD
						&& foldmethodIsSyntax(curwin))
		// Need to update folds later.
		foldUpdateAll(curwin);
#ಆದರೆ_ಅಂತ್ಯ
	}
    }

    ಹಿಂತಿರುಗಿ arg;
}

/*
 * Adjustments to syntax item when declared in a ":syn include"'d file.
 * Set the contained flag, and if the item is not already contained, add it
 * to the specified top-level group, if any.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_incl_toplevel(ಪೂರ್ಣಾಂಕ id, ಪೂರ್ಣಾಂಕ *flagsp)
{
    ಆದರೆ ((*flagsp & HL_CONTAINED) || curwin->w_s->b_syn_topgrp == 0)
	ಹಿಂತಿರುಗಿ;
    *flagsp |= HL_CONTAINED | HL_INCLUDED_TOPLEVEL;
    ಆದರೆ (curwin->w_s->b_syn_topgrp >= SYNID_CLUSTER)
    {
	// We have to alloc this, because syn_combine_list() will free it.
	ಗಿಡ್ಡ	    *grp_list = ALLOC_MULT(ಗಿಡ್ಡ, 2);
	ಪೂರ್ಣಾಂಕ	    tlg_id = curwin->w_s->b_syn_topgrp - SYNID_CLUSTER;

	ಆದರೆ (grp_list != ಖಾಲಿ)
	{
	    grp_list[0] = id;
	    grp_list[1] = 0;
	    syn_combine_list(&SYN_CLSTR(curwin->w_s)[tlg_id].scl_list,
						       &grp_list, CLUSTER_ಕೂಡಿಸು);
	}
    }
}

/*
 * Handle ":syntax include [@{group-name}] filename" command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_include(exarg_T *eap, ಪೂರ್ಣಾಂಕ syncing ಬಳಸದ)
{
    char_u	*arg = eap->arg;
    ಪೂರ್ಣಾಂಕ		sgl_id = 1;
    char_u	*ಗುಂಪು_name_ಕೊನೆ;
    char_u	*rest;
    ಅಕ್ಷರ	*errormsg = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		prev_toplvl_grp;
    ಪೂರ್ಣಾಂಕ		prev_syn_inc_ಟ್ಯಾಗ್;
    ಪೂರ್ಣಾಂಕ		ಮೂಲ = FALSE;

    eap->nextcmd = ಕಂಡುಹಿಡಿ_nextcmd(arg);
    ಆದರೆ (eap->skip)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (arg[0] == '@')
    {
	++arg;
	rest = get_ಗುಂಪು_name(arg, &ಗುಂಪು_name_ಕೊನೆ);
	ಆದರೆ (rest == ಖಾಲಿ)
	{
	    emsg(_(e_filename_required));
	    ಹಿಂತಿರುಗಿ;
	}
	sgl_id = syn_check_cluster(arg, (ಪೂರ್ಣಾಂಕ)(ಗುಂಪು_name_ಕೊನೆ - arg));
	ಆದರೆ (sgl_id == 0)
	    ಹಿಂತಿರುಗಿ;
	// separate_nextcmd() and expand_filename() depend on this
	eap->arg = rest;
    }

    /*
     * Everything that's left, up to the next command, should be the
     * filename to include.
     */
    eap->argt |= (EX_XFILE | EX_NOSPC);
    separate_nextcmd(eap, FALSE);
    ಆದರೆ (*eap->arg == '<' || *eap->arg == '$' || mch_is_ತುಂಬಿದ_Name(eap->arg))
    {
	// For an absolute path, "$VIM/..." or "<sfile>.." we ":source" the
	// file.  Need to expand the file name first.  In other cases
	// ":runtime!" is used.
	ಮೂಲ = TRUE;
	ಆದರೆ (expand_filename(eap, syn_cmdlinep, &errormsg) == FAIL)
	{
	    ಆದರೆ (errormsg != ಖಾಲಿ)
		emsg(errormsg);
	    ಹಿಂತಿರುಗಿ;
	}
    }

    /*
     * Save and restore the existing top-level grouplist id and ":syn
     * include" tag around the actual inclusion.
     */
    ಆದರೆ (running_syn_inc_ಟ್ಯಾಗ್ >= MAX_SYN_INC_ಟ್ಯಾಗ್)
    {
	emsg(_(e_too_many_syntax_includes));
	ಹಿಂತಿರುಗಿ;
    }
    prev_syn_inc_ಟ್ಯಾಗ್ = current_syn_inc_ಟ್ಯಾಗ್;
    current_syn_inc_ಟ್ಯಾಗ್ = ++running_syn_inc_ಟ್ಯಾಗ್;
    prev_toplvl_grp = curwin->w_s->b_syn_topgrp;
    curwin->w_s->b_syn_topgrp = sgl_id;
    ಆದರೆ (ಮೂಲ ? do_ಮೂಲ(eap->arg, FALSE, DOSO_NONE, ಖಾಲಿ) == FAIL
				: ಮೂಲ_runtime(eap->arg, DIP_ALL) == FAIL)
	semsg(_(e_cant_open_file_str), eap->arg);
    curwin->w_s->b_syn_topgrp = prev_toplvl_grp;
    current_syn_inc_ಟ್ಯಾಗ್ = prev_syn_inc_ಟ್ಯಾಗ್;
}

/*
 * Handle ":syntax keyword {group-name} [{option}] keyword .." command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_keyword(exarg_T *eap, ಪೂರ್ಣಾಂಕ syncing ಬಳಸದ)
{
    char_u	*arg = eap->arg;
    char_u	*ಗುಂಪು_name_ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ		syn_id;
    char_u	*rest;
    char_u	*keyword_copy = ಖಾಲಿ;
    char_u	*p;
    char_u	*kw;
    syn_opt_arg_T syn_opt_arg;
    ಪೂರ್ಣಾಂಕ		cnt;
    ಪೂರ್ಣಾಂಕ		conceal_char = NUL;

    rest = get_ಗುಂಪು_name(arg, &ಗುಂಪು_name_ಕೊನೆ);

    ಆದರೆ (rest != ಖಾಲಿ)
    {
	ಆದರೆ (eap->skip)
	    syn_id = -1;
	ಇಲ್ಲದಿದ್ದರೆ
	    syn_id = syn_check_ಗುಂಪು(arg, (ಪೂರ್ಣಾಂಕ)(ಗುಂಪು_name_ಕೊನೆ - arg));
	ಆದರೆ (syn_id != 0)
	    // allocate a buffer, for removing backslashes in the keyword
	    keyword_copy = alloc(STRLEN(rest) + 1);
	ಆದರೆ (keyword_copy != ಖಾಲಿ)
	{
	    syn_opt_arg.flags = 0;
	    syn_opt_arg.keyword = TRUE;
	    syn_opt_arg.sync_idx = ಖಾಲಿ;
	    syn_opt_arg.has_cont_list = FALSE;
	    syn_opt_arg.cont_ಒಳ_list = ಖಾಲಿ;
	    syn_opt_arg.ಮುಂದಿನ_list = ಖಾಲಿ;

	    /*
	     * The options given apply to ALL keywords, so all options must be
	     * found before keywords can be created.
	     * 1: collect the options and copy the keywords to keyword_copy.
	     */
	    cnt = 0;
	    p = keyword_copy;
	    ಗಾಗಿ ( ; rest != ಖಾಲಿ && !ತುದಿಗಳು_excmd2(eap->arg, rest);
							rest = skipwhite(rest))
	    {
		rest = get_syn_options(rest, &syn_opt_arg, &conceal_char,
								    eap->skip);
		ಆದರೆ (rest == ಖಾಲಿ || ತುದಿಗಳು_excmd2(eap->arg, rest))
		    ನಿಲ್ಲಿಸು;
		// Copy the keyword, removing backslashes, and add a NUL.
		ಆಗ (*rest != NUL && !VIM_ISWHITE(*rest))
		{
		    ಆದರೆ (*rest == '\\' && rest[1] != NUL)
			++rest;
		    *p++ = *rest++;
		}
		*p++ = NUL;
		++cnt;
	    }

	    ಆದರೆ (!eap->skip)
	    {
		size_t	kwlen = 0;

		// Adjust flags for use of ":syn include".
		syn_incl_toplevel(syn_id, &syn_opt_arg.flags);

		/*
		 * 2: Add an entry for each keyword.
		 */
		ಗಾಗಿ (kw = keyword_copy; --cnt >= 0; kw += kwlen + 1)
		{
		    ಗಾಗಿ (p = vim_strchr(kw, '['); ; )
		    {
			ಆದರೆ (p == ಖಾಲಿ)
			    kwlen = STRLEN(kw);
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    *p = NUL;
			    kwlen = (size_t)(p - kw);
			}
			ಕೂಡಿಸು_keyword(kw, kwlen, syn_id, syn_opt_arg.flags,
				syn_opt_arg.cont_ಒಳ_list,
					 syn_opt_arg.ಮುಂದಿನ_list, conceal_char);
			ಆದರೆ (p == ಖಾಲಿ)
			    ನಿಲ್ಲಿಸು;
			ಆದರೆ (p[1] == NUL)
			{
			    semsg(_(e_error_missing_rsb_str), kw);
			    ಹೋಗು error;
			}
			ಆದರೆ (p[1] == ']')
			{
			    ಆದರೆ (p[2] != NUL)
			    {
				semsg(_(e_trailing_char_after_rsb_str_str),
								    kw, &p[2]);
				ಹೋಗು error;
			    }
			    kw = p + 1;		// skip over the "]"
			    kwlen = 1;
			    ನಿಲ್ಲಿಸು;
			}
			ಆದರೆ (has_mbyte)
			{
			    ಪೂರ್ಣಾಂಕ l = (*mb_ptr2len)(p + 1);

			    mch_memmove(p, p + 1, l);
			    p += l;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    p[0] = p[1];
			    ++p;
			}
		    }
		}
	    }
error:
	    vim_free(keyword_copy);
	    vim_free(syn_opt_arg.cont_ಒಳ_list);
	    vim_free(syn_opt_arg.ಮುಂದಿನ_list);
	}
    }

    ಆದರೆ (rest != ಖಾಲಿ)
	ಗಣ_nextcmd(eap, rest);
    ಇಲ್ಲದಿದ್ದರೆ
	semsg(_(e_invalid_ವಾದ_str), arg);

    redraw_curbuf_later(UPD_SOME_VALID);
    syn_ರಾಶಿ_free_all(curwin->w_s);		// Need to recompute all syntax.
}

/*
 * Handle ":syntax match {name} [{options}] {pattern} [{options}]".
 *
 * Also ":syntax sync match {name} [[grouphere | groupthere] {group-name}] .."
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_ಹೊಂದಿಕೆ(
    exarg_T	*eap,
    ಪೂರ್ಣಾಂಕ		syncing)	    // TRUE for ":syntax sync match .. "
{
    char_u	*arg = eap->arg;
    char_u	*ಗುಂಪು_name_ಕೊನೆ;
    char_u	*rest;
    synpat_T	ಅಂಶ;		// the item found in the line
    ಪೂರ್ಣಾಂಕ		syn_id;
    ಪೂರ್ಣಾಂಕ		idx;
    syn_opt_arg_T syn_opt_arg;
    ಪೂರ್ಣಾಂಕ		sync_idx = 0;
    ಪೂರ್ಣಾಂಕ		conceal_char = NUL;
    ಪೂರ್ಣಾಂಕ		orig_called_emsg = called_emsg;

    // Isolate the group name, check for validity
    rest = get_ಗುಂಪು_name(arg, &ಗುಂಪು_name_ಕೊನೆ);

    // Get options before the pattern
    syn_opt_arg.flags = 0;
    syn_opt_arg.keyword = FALSE;
    syn_opt_arg.sync_idx = syncing ? &sync_idx : ಖಾಲಿ;
    syn_opt_arg.has_cont_list = TRUE;
    syn_opt_arg.cont_list = ಖಾಲಿ;
    syn_opt_arg.cont_ಒಳ_list = ಖಾಲಿ;
    syn_opt_arg.ಮುಂದಿನ_list = ಖಾಲಿ;
    rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);

    // get the pattern.
    init_syn_patterns();
    CLEAR_ಕ್ಷೇತ್ರ(ಅಂಶ);
    rest = get_syn_ನಮೂನೆ(rest, &ಅಂಶ);
    ಆದರೆ (vim_regcomp_had_eol() && !(syn_opt_arg.flags & HL_EXCLUDENL))
	syn_opt_arg.flags |= HL_HAS_EOL;

    // Get options after the pattern
    rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);

    ಆದರೆ (rest != ಖಾಲಿ)		// all arguments are valid
    {
	/*
	 * Check for trailing command and illegal trailing arguments.
	 */
	ಗಣ_nextcmd(eap, rest);
	ಆದರೆ (!ತುದಿಗಳು_excmd2(eap->cmd, rest) || eap->skip)
	    rest = ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ga_grow(&curwin->w_s->b_syn_patterns, 1) == OK
		&& (syn_id = syn_check_ಗುಂಪು(arg,
					   (ಪೂರ್ಣಾಂಕ)(ಗುಂಪು_name_ಕೊನೆ - arg))) != 0)
	{
	    syn_incl_toplevel(syn_id, &syn_opt_arg.flags);
	    /*
	     * Store the pattern in the syn_items list
	     */
	    idx = curwin->w_s->b_syn_patterns.ga_len;
	    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx] = ಅಂಶ;
	    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_syncing = syncing;
	    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_type = SPTYPE_ಹೊಂದಿಕೆ;
	    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_syn.id = syn_id;
	    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_syn.inc_ಟ್ಯಾಗ್ = current_syn_inc_ಟ್ಯಾಗ್;
	    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_flags = syn_opt_arg.flags;
	    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_sync_idx = sync_idx;
	    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_cont_list = syn_opt_arg.cont_list;
	    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_syn.cont_ಒಳ_list =
						     syn_opt_arg.cont_ಒಳ_list;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
	    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_cchar = conceal_char;
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (syn_opt_arg.cont_ಒಳ_list != ಖಾಲಿ)
		curwin->w_s->b_syn_containedin = TRUE;
	    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_ಮುಂದಿನ_list = syn_opt_arg.ಮುಂದಿನ_list;
	    ++curwin->w_s->b_syn_patterns.ga_len;

	    // remember that we found a match for syncing on
	    ಆದರೆ (syn_opt_arg.flags & (HL_SYNC_ಇಲ್ಲಿ|HL_SYNC_THERE))
		curwin->w_s->b_syn_sync_flags |= SF_ಹೊಂದಿಕೆ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    ಆದರೆ (syn_opt_arg.flags & HL_FOLD)
		++curwin->w_s->b_syn_folditems;
#ಆದರೆ_ಅಂತ್ಯ

	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_ರಾಶಿ_free_all(curwin->w_s);	// Need to recompute all syntax.
	    ಹಿಂತಿರುಗಿ;	// don't free the progs and patterns now
	}
    }

    /*
     * Something failed, free the allocated memory.
     */
    vim_regfree(ಅಂಶ.sp_prog);
    vim_free(ಅಂಶ.sp_ನಮೂನೆ);
    vim_free(syn_opt_arg.cont_list);
    vim_free(syn_opt_arg.cont_ಒಳ_list);
    vim_free(syn_opt_arg.ಮುಂದಿನ_list);

    ಆದರೆ (rest == ಖಾಲಿ && called_emsg == orig_called_emsg)
	semsg(_(e_invalid_ವಾದ_str), arg);
}

/*
 * Handle ":syntax region {group-name} [matchgroup={group-name}]
 *		start {start} .. [skip {skip}] end {end} .. [{options}]".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_region(
    exarg_T	*eap,
    ಪೂರ್ಣಾಂಕ		syncing)	    // TRUE for ":syntax sync region .."
{
    char_u		*arg = eap->arg;
    char_u		*ಗುಂಪು_name_ಕೊನೆ;
    char_u		*rest;			// next arg, NULL on error
    char_u		*ಕೀಲಿ_ಕೊನೆ;
    char_u		*ಕೀಲಿ = ಖಾಲಿ;
    char_u		*p;
    ಪೂರ್ಣಾಂಕ			ಅಂಶ;
#ವ್ಯಾಖ್ಯಾನಿಸು ಅಂಶ_ಆರಂಭ_2	    0
#ವ್ಯಾಖ್ಯಾನಿಸು ಅಂಶ_SKIP	    1
#ವ್ಯಾಖ್ಯಾನಿಸು ಅಂಶ_ಕೊನೆ_2	    2
#ವ್ಯಾಖ್ಯಾನಿಸು ಅಂಶ_MATCHGROUP	    3
    ರಚನೆ pat_ptr
    {
	synpat_T	*pp_synp;		// pointer to syn_pattern
	ಪೂರ್ಣಾಂಕ		pp_matchgroup_id;	// matchgroup ID
	ರಚನೆ pat_ptr	*pp_ಮುಂದಿನ;		// pointer to next pat_ptr
    }			*(pat_ptrs[3]);
					// patterns found in the line
    ರಚನೆ pat_ptr	*ppp;
    ರಚನೆ pat_ptr	*ppp_ಮುಂದಿನ;
    ಪೂರ್ಣಾಂಕ			pat_ಎಣಿಕೆ = 0;		// nr of syn_patterns found
    ಪೂರ್ಣಾಂಕ			syn_id;
    ಪೂರ್ಣಾಂಕ			matchgroup_id = 0;
    ಪೂರ್ಣಾಂಕ			not_enough = FALSE;	// not enough arguments
    ಪೂರ್ಣಾಂಕ			illegal = FALSE;	// illegal arguments
    ಪೂರ್ಣಾಂಕ			success = FALSE;
    ಪೂರ್ಣಾಂಕ			idx;
    syn_opt_arg_T	syn_opt_arg;
    ಪೂರ್ಣಾಂಕ			conceal_char = NUL;

    // Isolate the group name, check for validity
    rest = get_ಗುಂಪು_name(arg, &ಗುಂಪು_name_ಕೊನೆ);

    pat_ptrs[0] = ಖಾಲಿ;
    pat_ptrs[1] = ಖಾಲಿ;
    pat_ptrs[2] = ಖಾಲಿ;

    init_syn_patterns();

    syn_opt_arg.flags = 0;
    syn_opt_arg.keyword = FALSE;
    syn_opt_arg.sync_idx = ಖಾಲಿ;
    syn_opt_arg.has_cont_list = TRUE;
    syn_opt_arg.cont_list = ಖಾಲಿ;
    syn_opt_arg.cont_ಒಳ_list = ಖಾಲಿ;
    syn_opt_arg.ಮುಂದಿನ_list = ಖಾಲಿ;

    /*
     * get the options, patterns and matchgroup.
     */
    ಆಗ (rest != ಖಾಲಿ && !ತುದಿಗಳು_excmd2(eap->cmd, rest))
    {
	// Check for option arguments
	rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);
	ಆದರೆ (rest == ಖಾಲಿ || ತುದಿಗಳು_excmd2(eap->cmd, rest))
	    ನಿಲ್ಲಿಸು;

	// must be a pattern or matchgroup then
	ಕೀಲಿ_ಕೊನೆ = rest;
	ಆಗ (*ಕೀಲಿ_ಕೊನೆ && !VIM_ISWHITE(*ಕೀಲಿ_ಕೊನೆ) && *ಕೀಲಿ_ಕೊನೆ != '=')
	    ++ಕೀಲಿ_ಕೊನೆ;
	vim_free(ಕೀಲಿ);
	ಕೀಲಿ = vim_strnsave_ಮೇಲೆ(rest, ಕೀಲಿ_ಕೊನೆ - rest);
	ಆದರೆ (ಕೀಲಿ == ಖಾಲಿ)			// out of memory
	{
	    rest = ಖಾಲಿ;
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (STRCMP(ಕೀಲಿ, "MATCHGROUP") == 0)
	    ಅಂಶ = ಅಂಶ_MATCHGROUP;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(ಕೀಲಿ, "START") == 0)
	    ಅಂಶ = ಅಂಶ_ಆರಂಭ_2;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(ಕೀಲಿ, "END") == 0)
	    ಅಂಶ = ಅಂಶ_ಕೊನೆ_2;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(ಕೀಲಿ, "SKIP") == 0)
	{
	    ಆದರೆ (pat_ptrs[ಅಂಶ_SKIP] != ಖಾಲಿ)	// one skip pattern allowed
	    {
		illegal = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	    ಅಂಶ = ಅಂಶ_SKIP;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ನಿಲ್ಲಿಸು;
	rest = skipwhite(ಕೀಲಿ_ಕೊನೆ);
	ಆದರೆ (*rest != '=')
	{
	    rest = ಖಾಲಿ;
	    semsg(_(e_missing_equal_str), arg);
	    ನಿಲ್ಲಿಸು;
	}
	rest = skipwhite(rest + 1);
	ಆದರೆ (*rest == NUL)
	{
	    not_enough = TRUE;
	    ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (ಅಂಶ == ಅಂಶ_MATCHGROUP)
	{
	    p = skiptowhite(rest);
	    ಆದರೆ ((p - rest == 4 && STRNCMP(rest, "NONE", 4) == 0) || eap->skip)
		matchgroup_id = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		matchgroup_id = syn_check_ಗುಂಪು(rest, (ಪೂರ್ಣಾಂಕ)(p - rest));
		ಆದರೆ (matchgroup_id == 0)
		{
		    illegal = TRUE;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	    rest = skipwhite(p);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    /*
	     * Allocate room for a syn_pattern, and link it in the list of
	     * syn_patterns for this item, at the start (because the list is
	     * used from end to start).
	     */
	    ppp = ALLOC_ONE(ರಚನೆ pat_ptr);
	    ಆದರೆ (ppp == ಖಾಲಿ)
	    {
		rest = ಖಾಲಿ;
		ನಿಲ್ಲಿಸು;
	    }
	    ppp->pp_ಮುಂದಿನ = pat_ptrs[ಅಂಶ];
	    pat_ptrs[ಅಂಶ] = ppp;
	    ppp->pp_synp = ALLOC_CLEAR_ONE(synpat_T);
	    ಆದರೆ (ppp->pp_synp == ಖಾಲಿ)
	    {
		rest = ಖಾಲಿ;
		ನಿಲ್ಲಿಸು;
	    }

	    /*
	     * Get the syntax pattern and the following offset(s).
	     */
	    // Enable the appropriate \z specials.
	    ಆದರೆ (ಅಂಶ == ಅಂಶ_ಆರಂಭ_2)
		reg_do_extmatch = REX_ಗಣ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಂಶ == ಅಂಶ_SKIP || ಅಂಶ == ಅಂಶ_ಕೊನೆ_2)
		reg_do_extmatch = REX_USE;
	    rest = get_syn_ನಮೂನೆ(rest, ppp->pp_synp);
	    reg_do_extmatch = 0;
	    ಆದರೆ (ಅಂಶ == ಅಂಶ_ಕೊನೆ_2 && vim_regcomp_had_eol()
				       && !(syn_opt_arg.flags & HL_EXCLUDENL))
		ppp->pp_synp->sp_flags |= HL_HAS_EOL;
	    ppp->pp_matchgroup_id = matchgroup_id;
	    ++pat_ಎಣಿಕೆ;
	}
    }
    vim_free(ಕೀಲಿ);
    ಆದರೆ (illegal || not_enough)
	rest = ಖಾಲಿ;

    /*
     * Must have a "start" and "end" pattern.
     */
    ಆದರೆ (rest != ಖಾಲಿ && (pat_ptrs[ಅಂಶ_ಆರಂಭ_2] == ಖಾಲಿ ||
						  pat_ptrs[ಅಂಶ_ಕೊನೆ_2] == ಖಾಲಿ))
    {
	not_enough = TRUE;
	rest = ಖಾಲಿ;
    }

    ಆದರೆ (rest != ಖಾಲಿ)
    {
	/*
	 * Check for trailing garbage or command.
	 * If OK, add the item.
	 */
	ಗಣ_nextcmd(eap, rest);
	ಆದರೆ (!ತುದಿಗಳು_excmd(*rest) || eap->skip)
	    rest = ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ga_grow(&(curwin->w_s->b_syn_patterns), pat_ಎಣಿಕೆ) == OK
		&& (syn_id = syn_check_ಗುಂಪು(arg,
					   (ಪೂರ್ಣಾಂಕ)(ಗುಂಪು_name_ಕೊನೆ - arg))) != 0)
	{
	    syn_incl_toplevel(syn_id, &syn_opt_arg.flags);
	    /*
	     * Store the start/skip/end in the syn_items list
	     */
	    idx = curwin->w_s->b_syn_patterns.ga_len;
	    ಗಾಗಿ (ಅಂಶ = ಅಂಶ_ಆರಂಭ_2; ಅಂಶ <= ಅಂಶ_ಕೊನೆ_2; ++ಅಂಶ)
	    {
		ಗಾಗಿ (ppp = pat_ptrs[ಅಂಶ]; ppp != ಖಾಲಿ; ppp = ppp->pp_ಮುಂದಿನ)
		{
		    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx] = *(ppp->pp_synp);
		    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_syncing = syncing;
		    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_type =
			    (ಅಂಶ == ಅಂಶ_ಆರಂಭ_2) ? SPTYPE_ಆರಂಭ :
			    (ಅಂಶ == ಅಂಶ_SKIP) ? SPTYPE_SKIP : SPTYPE_ಕೊನೆ;
		    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_flags |= syn_opt_arg.flags;
		    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_syn.id = syn_id;
		    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_syn.inc_ಟ್ಯಾಗ್ =
							  current_syn_inc_ಟ್ಯಾಗ್;
		    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_syn_ಹೊಂದಿಕೆ_id =
							ppp->pp_matchgroup_id;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
		    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_cchar = conceal_char;
#ಆದರೆ_ಅಂತ್ಯ
		    ಆದರೆ (ಅಂಶ == ಅಂಶ_ಆರಂಭ_2)
		    {
			SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_cont_list =
							syn_opt_arg.cont_list;
			SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_syn.cont_ಒಳ_list =
						     syn_opt_arg.cont_ಒಳ_list;
			ಆದರೆ (syn_opt_arg.cont_ಒಳ_list != ಖಾಲಿ)
			    curwin->w_s->b_syn_containedin = TRUE;
			SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_ಮುಂದಿನ_list =
							syn_opt_arg.ಮುಂದಿನ_list;
		    }
		    ++curwin->w_s->b_syn_patterns.ga_len;
		    ++idx;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		    ಆದರೆ (syn_opt_arg.flags & HL_FOLD)
			++curwin->w_s->b_syn_folditems;
#ಆದರೆ_ಅಂತ್ಯ
		}
	    }

	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_ರಾಶಿ_free_all(curwin->w_s);	// Need to recompute all syntax.
	    success = TRUE;	    // don't free the progs and patterns now
	}
    }

    /*
     * Free the allocated memory.
     */
    ಗಾಗಿ (ಅಂಶ = ಅಂಶ_ಆರಂಭ_2; ಅಂಶ <= ಅಂಶ_ಕೊನೆ_2; ++ಅಂಶ)
	ಗಾಗಿ (ppp = pat_ptrs[ಅಂಶ]; ppp != ಖಾಲಿ; ppp = ppp_ಮುಂದಿನ)
	{
	    ಆದರೆ (!success && ppp->pp_synp != ಖಾಲಿ)
	    {
		vim_regfree(ppp->pp_synp->sp_prog);
		vim_free(ppp->pp_synp->sp_ನಮೂನೆ);
	    }
	    vim_free(ppp->pp_synp);
	    ppp_ಮುಂದಿನ = ppp->pp_ಮುಂದಿನ;
	    vim_free(ppp);
	}

    ಆದರೆ (!success)
    {
	vim_free(syn_opt_arg.cont_list);
	vim_free(syn_opt_arg.cont_ಒಳ_list);
	vim_free(syn_opt_arg.ಮುಂದಿನ_list);
	ಆದರೆ (not_enough)
	    semsg(_(e_not_enough_arguments_syntax_region_str), arg);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (illegal || rest == ಖಾಲಿ)
	    semsg(_(e_invalid_ವಾದ_str), arg);
    }
}

/*
 * A simple syntax group ID comparison function suitable for use in qsort()
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
syn_compare_stub(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *v1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *v2)
{
    ಸ್ಥಿರಾಂಕ ಗಿಡ್ಡ	*s1 = v1;
    ಸ್ಥಿರಾಂಕ ಗಿಡ್ಡ	*s2 = v2;

    ಹಿಂತಿರುಗಿ (*s1 > *s2 ? 1 : *s1 < *s2 ? -1 : 0);
}

/*
 * Combines lists of syntax clusters.
 * *clstr1 and *clstr2 must both be allocated memory; they will be consumed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_combine_list(ಗಿಡ್ಡ **clstr1, ಗಿಡ್ಡ **clstr2, ಪೂರ್ಣಾಂಕ list_op)
{
    ಪೂರ್ಣಾಂಕ		count1 = 0;
    ಪೂರ್ಣಾಂಕ		count2 = 0;
    ಗಿಡ್ಡ	*g1;
    ಗಿಡ್ಡ	*g2;
    ಗಿಡ್ಡ	*clstr = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2;
    ಪೂರ್ಣಾಂಕ		ಸುತ್ತು;

    /*
     * Handle degenerate cases.
     */
    ಆದರೆ (*clstr2 == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (*clstr1 == ಖಾಲಿ || list_op == CLUSTER_REPLACE)
    {
	ಆದರೆ (list_op == CLUSTER_REPLACE)
	    vim_free(*clstr1);
	ಆದರೆ (list_op == CLUSTER_REPLACE || list_op == CLUSTER_ಕೂಡಿಸು)
	    *clstr1 = *clstr2;
	ಇಲ್ಲದಿದ್ದರೆ
	    vim_free(*clstr2);
	ಹಿಂತಿರುಗಿ;
    }

    ಗಾಗಿ (g1 = *clstr1; *g1; g1++)
	++count1;
    ಗಾಗಿ (g2 = *clstr2; *g2; g2++)
	++count2;

    /*
     * For speed purposes, sort both lists.
     */
    qsort(*clstr1, (size_t)count1, ಗಾತ್ರ(ಗಿಡ್ಡ), syn_compare_stub);
    qsort(*clstr2, (size_t)count2, ಗಾತ್ರ(ಗಿಡ್ಡ), syn_compare_stub);

    /*
     * We proceed in two passes; in round 1, we count the elements to place
     * in the new list, and in round 2, we allocate and populate the new
     * list.  For speed, we use a mergesort-like method, adding the smaller
     * of the current elements in each list to the new list.
     */
    ಗಾಗಿ (ಸುತ್ತು = 1; ಸುತ್ತು <= 2; ಸುತ್ತು++)
    {
	g1 = *clstr1;
	g2 = *clstr2;
	ಎಣಿಕೆ_2 = 0;

	/*
	 * First, loop through the lists until one of them is empty.
	 */
	ಆಗ (*g1 && *g2)
	{
	    /*
	     * We always want to add from the first list.
	     */
	    ಆದರೆ (*g1 < *g2)
	    {
		ಆದರೆ (ಸುತ್ತು == 2)
		    clstr[ಎಣಿಕೆ_2] = *g1;
		ಎಣಿಕೆ_2++;
		g1++;
		ಮುಂದುವರಿ;
	    }
	    /*
	     * We only want to add from the second list if we're adding the
	     * lists.
	     */
	    ಆದರೆ (list_op == CLUSTER_ಕೂಡಿಸು)
	    {
		ಆದರೆ (ಸುತ್ತು == 2)
		    clstr[ಎಣಿಕೆ_2] = *g2;
		ಎಣಿಕೆ_2++;
	    }
	    ಆದರೆ (*g1 == *g2)
		g1++;
	    g2++;
	}

	/*
	 * Now add the leftovers from whichever list didn't get finished
	 * first.  As before, we only want to add from the second list if
	 * we're adding the lists.
	 */
	ಗಾಗಿ (; *g1; g1++, ಎಣಿಕೆ_2++)
	    ಆದರೆ (ಸುತ್ತು == 2)
		clstr[ಎಣಿಕೆ_2] = *g1;
	ಆದರೆ (list_op == CLUSTER_ಕೂಡಿಸು)
	    ಗಾಗಿ (; *g2; g2++, ಎಣಿಕೆ_2++)
		ಆದರೆ (ಸುತ್ತು == 2)
		    clstr[ಎಣಿಕೆ_2] = *g2;

	ಆದರೆ (ಸುತ್ತು == 1)
	{
	    /*
	     * If the group ended up empty, we don't need to allocate any
	     * space for it.
	     */
	    ಆದರೆ (ಎಣಿಕೆ_2 == 0)
	    {
		clstr = ಖಾಲಿ;
		ನಿಲ್ಲಿಸು;
	    }
	    clstr = ALLOC_MULT(ಗಿಡ್ಡ, ಎಣಿಕೆ_2 + 1);
	    ಆದರೆ (clstr == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    clstr[ಎಣಿಕೆ_2] = 0;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
syn_scl_name2id(char_u *name)
{
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*name_u;

    // Avoid using stricmp() too much, it's slow on some systems
    name_u = vim_strsave_ಮೇಲೆ(name);
    ಆದರೆ (name_u == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;
    ಗಾಗಿ (i = curwin->w_s->b_syn_clusters.ga_len; --i >= 0; )
	ಆದರೆ (SYN_CLSTR(curwin->w_s)[i].scl_name_u != ಖಾಲಿ
		&& STRCMP(name_u, SYN_CLSTR(curwin->w_s)[i].scl_name_u) == 0)
	    ನಿಲ್ಲಿಸು;
    vim_free(name_u);
    ಹಿಂತಿರುಗಿ (i < 0 ? 0 : i + SYNID_CLUSTER);
}

/*
 * Like syn_scl_name2id(), but take a pointer + length argument.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
syn_scl_namen2id(char_u *linep, ಪೂರ್ಣಾಂಕ len)
{
    char_u  *name;
    ಪೂರ್ಣಾಂಕ	    id = 0;

    name = vim_strnsave(linep, len);
    ಆದರೆ (name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    id = syn_scl_name2id(name);
    vim_free(name);
    ಹಿಂತಿರುಗಿ id;
}

/*
 * Find syntax cluster name in the table and return its ID.
 * The argument is a pointer to the name and the length of the name.
 * If it doesn't exist yet, a new entry is created.
 * Return 0 for failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
syn_check_cluster(char_u *pp, ಪೂರ್ಣಾಂಕ len)
{
    ಪೂರ್ಣಾಂಕ		id;
    char_u	*name;

    name = vim_strnsave(pp, len);
    ಆದರೆ (name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    id = syn_scl_name2id(name);
    ಆದರೆ (id == 0)			// doesn't exist yet
	id = syn_ಕೂಡಿಸು_cluster(name);
    ಇಲ್ಲದಿದ್ದರೆ
	vim_free(name);
    ಹಿಂತಿರುಗಿ id;
}

/*
 * Add new syntax cluster and return its ID.
 * "name" must be an allocated string, it will be consumed.
 * Return 0 for failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
syn_ಕೂಡಿಸು_cluster(char_u *name)
{
    ಪೂರ್ಣಾಂಕ		len;

    /*
     * First call for this growarray: init growing array.
     */
    ಆದರೆ (curwin->w_s->b_syn_clusters.ga_ದತ್ತಾಂಶ == ಖಾಲಿ)
    {
	curwin->w_s->b_syn_clusters.ga_itemsize = ಗಾತ್ರ(syn_cluster_T);
	curwin->w_s->b_syn_clusters.ga_growsize = 10;
    }

    len = curwin->w_s->b_syn_clusters.ga_len;
    ಆದರೆ (len >= MAX_CLUSTER_ID)
    {
	emsg(_(e_too_many_syntax_clusters));
	vim_free(name);
	ಹಿಂತಿರುಗಿ 0;
    }

    /*
     * Make room for at least one other cluster entry.
     */
    ಆದರೆ (ga_grow(&curwin->w_s->b_syn_clusters, 1) == FAIL)
    {
	vim_free(name);
	ಹಿಂತಿರುಗಿ 0;
    }

    CLEAR_ಸೂಚಕ(&(SYN_CLSTR(curwin->w_s)[len]));
    SYN_CLSTR(curwin->w_s)[len].scl_name = name;
    SYN_CLSTR(curwin->w_s)[len].scl_name_u = vim_strsave_ಮೇಲೆ(name);
    SYN_CLSTR(curwin->w_s)[len].scl_list = ಖಾಲಿ;
    ++curwin->w_s->b_syn_clusters.ga_len;

    ಆದರೆ (STRICMP(name, "Spell") == 0)
	curwin->w_s->b_spell_cluster_id = len + SYNID_CLUSTER;
    ಆದರೆ (STRICMP(name, "NoSpell") == 0)
	curwin->w_s->b_nospell_cluster_id = len + SYNID_CLUSTER;

    ಹಿಂತಿರುಗಿ len + SYNID_CLUSTER;
}

/*
 * Handle ":syntax cluster {cluster-name} [contains={groupname},..]
 *		[add={groupname},..] [remove={groupname},..]".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_cluster(exarg_T *eap, ಪೂರ್ಣಾಂಕ syncing ಬಳಸದ)
{
    char_u	*arg = eap->arg;
    char_u	*ಗುಂಪು_name_ಕೊನೆ;
    char_u	*rest;
    ಪೂರ್ಣಾಂಕ		scl_id;
    ಗಿಡ್ಡ	*clstr_list;
    ಪೂರ್ಣಾಂಕ		got_clstr = FALSE;
    ಪೂರ್ಣಾಂಕ		opt_len;
    ಪೂರ್ಣಾಂಕ		list_op;

    eap->nextcmd = ಕಂಡುಹಿಡಿ_nextcmd(arg);
    ಆದರೆ (eap->skip)
	ಹಿಂತಿರುಗಿ;

    rest = get_ಗುಂಪು_name(arg, &ಗುಂಪು_name_ಕೊನೆ);

    ಆದರೆ (rest != ಖಾಲಿ)
    {
	scl_id = syn_check_cluster(arg, (ಪೂರ್ಣಾಂಕ)(ಗುಂಪು_name_ಕೊನೆ - arg));
	ಆದರೆ (scl_id == 0)
	    ಹಿಂತಿರುಗಿ;
	scl_id -= SYNID_CLUSTER;

	ಗಾಗಿ (;;)
	{
	    ಆದರೆ (STRNICMP(rest, "add", 3) == 0
		    && (VIM_ISWHITE(rest[3]) || rest[3] == '='))
	    {
		opt_len = 3;
		list_op = CLUSTER_ಕೂಡಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(rest, "remove", 6) == 0
		    && (VIM_ISWHITE(rest[6]) || rest[6] == '='))
	    {
		opt_len = 6;
		list_op = CLUSTER_ಕಳೆ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(rest, "contains", 8) == 0
			&& (VIM_ISWHITE(rest[8]) || rest[8] == '='))
	    {
		opt_len = 8;
		list_op = CLUSTER_REPLACE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ನಿಲ್ಲಿಸು;

	    clstr_list = ಖಾಲಿ;
	    ಆದರೆ (get_id_list(&rest, opt_len, &clstr_list, eap->skip) == FAIL)
	    {
		semsg(_(e_invalid_ವಾದ_str), rest);
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (scl_id >= 0)
		syn_combine_list(&SYN_CLSTR(curwin->w_s)[scl_id].scl_list,
			     &clstr_list, list_op);
	    ಇಲ್ಲದಿದ್ದರೆ
		vim_free(clstr_list);
	    got_clstr = TRUE;
	}

	ಆದರೆ (got_clstr)
	{
	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_ರಾಶಿ_free_all(curwin->w_s);	// Need to recompute all.
	}
    }

    ಆದರೆ (!got_clstr)
	emsg(_(e_no_cluster_specified));
    ಆದರೆ (rest == ಖಾಲಿ || !ತುದಿಗಳು_excmd2(eap->cmd, rest))
	semsg(_(e_invalid_ವಾದ_str), arg);
}

/*
 * On first call for current buffer: Init growing array.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_syn_patterns(ಶೂನ್ಯ)
{
    curwin->w_s->b_syn_patterns.ga_itemsize = ಗಾತ್ರ(synpat_T);
    curwin->w_s->b_syn_patterns.ga_growsize = 10;
}

/*
 * Get one pattern for a ":syntax match" or ":syntax region" command.
 * Stores the pattern and program in a synpat_T.
 * Returns a pointer to the next argument, or NULL in case of an error.
 */
    ಸ್ಥಿರ char_u *
get_syn_ನಮೂನೆ(char_u *arg, synpat_T *ci)
{
    char_u	*ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ		*p;
    ಪೂರ್ಣಾಂಕ		idx;
    char_u	*cpo_save;

    // need at least three chars
    ಆದರೆ (arg == ಖಾಲಿ || arg[0] == NUL || arg[1] == NUL || arg[2] == NUL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಕೊನೆ = skip_regexp(arg + 1, *arg, TRUE);
    ಆದರೆ (*ಕೊನೆ != *arg)			    // end delimiter not found
    {
	semsg(_(e_ನಮೂನೆ_delimiter_not_ಸಿಕ್ಕಿತು_str), arg);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    // store the pattern and compiled regexp program
    ಆದರೆ ((ci->sp_ನಮೂನೆ = vim_strnsave(arg + 1, ಕೊನೆ - arg - 1)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Make 'cpoptions' empty, to avoid the 'l' flag
    cpo_save = p_cpo;
    p_cpo = ಖಾಲಿಗಳು_option;
    ci->sp_prog = vim_regcomp(ci->sp_ನಮೂನೆ, RE_MAGIC);
    p_cpo = cpo_save;

    ಆದರೆ (ci->sp_prog == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ci->sp_ic = curwin->w_s->b_syn_ic;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    syn_clear_ಸಮಯ(&ci->sp_ಸಮಯ);
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Check for a match, highlight or region offset.
     */
    ++ಕೊನೆ;
    ಮಾಡು
    {
	ಗಾಗಿ (idx = SPO_ಎಣಿಕೆ; --idx >= 0; )
	    ಆದರೆ (STRNCMP(ಕೊನೆ, spo_name_tab[idx], 3) == 0)
		ನಿಲ್ಲಿಸು;
	ಆದರೆ (idx >= 0)
	{
	    p = &(ci->sp_offsets[idx]);
	    ಆದರೆ (idx != SPO_LC_OFF)
		ಆಯ್ಕೆ (ಕೊನೆ[3])
		{
		    ಸ್ಥಿತಿ 's':   ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'b':   ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'e':   idx += SPO_ಎಣಿಕೆ; ನಿಲ್ಲಿಸು;
		    ಪೂರ್ವನಿಯೋಜಿತ:    idx = -1; ನಿಲ್ಲಿಸು;
		}
	    ಆದರೆ (idx >= 0)
	    {
		ci->sp_off_flags |= (1 << idx);
		ಆದರೆ (idx == SPO_LC_OFF)	    // lc=99
		{
		    ಕೊನೆ += 3;
		    *p = getdigits(&ಕೊನೆ);

		    // "lc=" offset automatically sets "ms=" offset
		    ಆದರೆ (!(ci->sp_off_flags & (1 << SPO_MS_OFF)))
		    {
			ci->sp_off_flags |= (1 << SPO_MS_OFF);
			ci->sp_offsets[SPO_MS_OFF] = *p;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ			    // yy=x+99
		{
		    ಕೊನೆ += 4;
		    ಆದರೆ (*ಕೊನೆ == '+')
		    {
			++ಕೊನೆ;
			*p = getdigits(&ಕೊನೆ);		// positive offset
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ಕೊನೆ == '-')
		    {
			++ಕೊನೆ;
			*p = -getdigits(&ಕೊನೆ);		// negative offset
		    }
		}
		ಆದರೆ (*ಕೊನೆ != ',')
		    ನಿಲ್ಲಿಸು;
		++ಕೊನೆ;
	    }
	}
    } ಆಗ (idx >= 0);

    ಆದರೆ (!ತುದಿಗಳು_excmd2(arg, ಕೊನೆ) && !VIM_ISWHITE(*ಕೊನೆ))
    {
	semsg(_(e_garbage_after_ನಮೂನೆ_str), arg);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ skipwhite(ಕೊನೆ);
}

/*
 * Handle ":syntax sync .." command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_sync(exarg_T *eap, ಪೂರ್ಣಾಂಕ syncing ಬಳಸದ)
{
    char_u	*arg_ಆರಂಭ = eap->arg;
    char_u	*arg_ಕೊನೆ;
    char_u	*ಕೀಲಿ = ಖಾಲಿ;
    char_u	*ಮುಂದಿನ_arg;
    ಪೂರ್ಣಾಂಕ		illegal = FALSE;
    ಪೂರ್ಣಾಂಕ		ಪೂರ್ಣಗೊಂಡ = FALSE;
    ದೀರ್ಘ	n;
    char_u	*cpo_save;

    ಆದರೆ (ತುದಿಗಳು_excmd2(eap->cmd, arg_ಆರಂಭ))
    {
	syn_cmd_list(eap, TRUE);
	ಹಿಂತಿರುಗಿ;
    }

    ಆಗ (!ತುದಿಗಳು_excmd2(eap->cmd, arg_ಆರಂಭ))
    {
	arg_ಕೊನೆ = skiptowhite(arg_ಆರಂಭ);
	ಮುಂದಿನ_arg = skipwhite(arg_ಕೊನೆ);
	vim_free(ಕೀಲಿ);
	ಕೀಲಿ = vim_strnsave_ಮೇಲೆ(arg_ಆರಂಭ, arg_ಕೊನೆ - arg_ಆರಂಭ);
	ಆದರೆ (ಕೀಲಿ == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (STRCMP(ಕೀಲಿ, "CCOMMENT") == 0)
	{
	    ಆದರೆ (!eap->skip)
		curwin->w_s->b_syn_sync_flags |= SF_CCOMMENT;
	    ಆದರೆ (!ತುದಿಗಳು_excmd2(eap->cmd, ಮುಂದಿನ_arg))
	    {
		arg_ಕೊನೆ = skiptowhite(ಮುಂದಿನ_arg);
		ಆದರೆ (!eap->skip)
		    curwin->w_s->b_syn_sync_id = syn_check_ಗುಂಪು(ಮುಂದಿನ_arg,
						   (ಪೂರ್ಣಾಂಕ)(arg_ಕೊನೆ - ಮುಂದಿನ_arg));
		ಮುಂದಿನ_arg = skipwhite(arg_ಕೊನೆ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!eap->skip)
		curwin->w_s->b_syn_sync_id = syn_name2id((char_u *)"Comment");
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (  STRNCMP(ಕೀಲಿ, "LINES", 5) == 0
		|| STRNCMP(ಕೀಲಿ, "MINLINES", 8) == 0
		|| STRNCMP(ಕೀಲಿ, "MAXLINES", 8) == 0
		|| STRNCMP(ಕೀಲಿ, "LINEBREAKS", 10) == 0)
	{
	    ಆದರೆ (ಕೀಲಿ[4] == 'S')
		arg_ಕೊನೆ = ಕೀಲಿ + 6;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೀಲಿ[0] == 'L')
		arg_ಕೊನೆ = ಕೀಲಿ + 11;
	    ಇಲ್ಲದಿದ್ದರೆ
		arg_ಕೊನೆ = ಕೀಲಿ + 9;
	    ಆದರೆ (arg_ಕೊನೆ[-1] != '=' || !VIM_ISDIGIT(*arg_ಕೊನೆ))
	    {
		illegal = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	    n = getdigits(&arg_ಕೊನೆ);
	    ಆದರೆ (!eap->skip)
	    {
		ಆದರೆ (ಕೀಲಿ[4] == 'B')
		    curwin->w_s->b_syn_sync_linebreaks = n;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೀಲಿ[1] == 'A')
		    curwin->w_s->b_syn_sync_maxlines = n;
		ಇಲ್ಲದಿದ್ದರೆ
		    curwin->w_s->b_syn_sync_minlines = n;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(ಕೀಲಿ, "FROMSTART") == 0)
	{
	    ಆದರೆ (!eap->skip)
	    {
		curwin->w_s->b_syn_sync_minlines = MAXLNUM;
		curwin->w_s->b_syn_sync_maxlines = 0;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(ಕೀಲಿ, "LINECONT") == 0)
	{
	    ಆದರೆ (*ಮುಂದಿನ_arg == NUL)	   // missing pattern
	    {
		illegal = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (curwin->w_s->b_syn_linecont_pat != ಖಾಲಿ)
	    {
		emsg(_(e_syntax_sync_ಸಾಲು_continuations_ನಮೂನೆ_specified_twice));
		ಪೂರ್ಣಗೊಂಡ = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	    arg_ಕೊನೆ = skip_regexp(ಮುಂದಿನ_arg + 1, *ಮುಂದಿನ_arg, TRUE);
	    ಆದರೆ (*arg_ಕೊನೆ != *ಮುಂದಿನ_arg)	    // end delimiter not found
	    {
		illegal = TRUE;
		ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ (!eap->skip)
	    {
		// store the pattern and compiled regexp program
		ಆದರೆ ((curwin->w_s->b_syn_linecont_pat =
			    vim_strnsave(ಮುಂದಿನ_arg + 1,
				      arg_ಕೊನೆ - ಮುಂದಿನ_arg - 1)) == ಖಾಲಿ)
		{
		    ಪೂರ್ಣಗೊಂಡ = TRUE;
		    ನಿಲ್ಲಿಸು;
		}
		curwin->w_s->b_syn_linecont_ic = curwin->w_s->b_syn_ic;

		// Make 'cpoptions' empty, to avoid the 'l' flag
		cpo_save = p_cpo;
		p_cpo = ಖಾಲಿಗಳು_option;
		curwin->w_s->b_syn_linecont_prog =
		       vim_regcomp(curwin->w_s->b_syn_linecont_pat, RE_MAGIC);
		p_cpo = cpo_save;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
		syn_clear_ಸಮಯ(&curwin->w_s->b_syn_linecont_ಸಮಯ);
#ಆದರೆ_ಅಂತ್ಯ

		ಆದರೆ (curwin->w_s->b_syn_linecont_prog == ಖಾಲಿ)
		{
		    VIM_CLEAR(curwin->w_s->b_syn_linecont_pat);
		    ಪೂರ್ಣಗೊಂಡ = TRUE;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	    ಮುಂದಿನ_arg = skipwhite(arg_ಕೊನೆ + 1);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    eap->arg = ಮುಂದಿನ_arg;
	    ಆದರೆ (STRCMP(ಕೀಲಿ, "MATCH") == 0)
		syn_cmd_ಹೊಂದಿಕೆ(eap, TRUE);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(ಕೀಲಿ, "REGION") == 0)
		syn_cmd_region(eap, TRUE);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(ಕೀಲಿ, "CLEAR") == 0)
		syn_cmd_clear(eap, TRUE);
	    ಇಲ್ಲದಿದ್ದರೆ
		illegal = TRUE;
	    ಪೂರ್ಣಗೊಂಡ = TRUE;
	    ನಿಲ್ಲಿಸು;
	}
	arg_ಆರಂಭ = ಮುಂದಿನ_arg;
    }
    vim_free(ಕೀಲಿ);
    ಆದರೆ (illegal)
	semsg(_(e_illegal_arguments_str), arg_ಆರಂಭ);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ಪೂರ್ಣಗೊಂಡ)
    {
	ಗಣ_nextcmd(eap, arg_ಆರಂಭ);
	redraw_curbuf_later(UPD_SOME_VALID);
	syn_ರಾಶಿ_free_all(curwin->w_s);	// Need to recompute all syntax.
    }
}

/*
 * Convert a line of highlight group names into a list of group ID numbers.
 * "arg" should point to the "contains" or "nextgroup" keyword.
 * "arg" is advanced to after the last group name.
 * Careful: the argument is modified (NULs added).
 * returns FAIL for some error, OK for success.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_id_list(
    char_u	**arg,
    ಪೂರ್ಣಾಂಕ		keylen,		// length of keyword
    ಗಿಡ್ಡ	**list,		// where to store the resulting list, if not
				// NULL, the list is silently skipped!
    ಪೂರ್ಣಾಂಕ		skip)
{
    char_u	*p = ಖಾಲಿ;
    char_u	*ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ		ಸುತ್ತು;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2;
    ಪೂರ್ಣಾಂಕ		ಒಟ್ಟು_ಎಣಿಕೆ = 0;
    ಗಿಡ್ಡ	*retval = ಖಾಲಿ;
    char_u	*name;
    regmatch_T	regmatch;
    ಪೂರ್ಣಾಂಕ		id;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		failed = FALSE;

    /*
     * We parse the list twice:
     * round == 1: count the number of items, allocate the array.
     * round == 2: fill the array with the items.
     * In round 1 new groups may be added, causing the number of items to
     * grow when a regexp is used.  In that case round 1 is done once again.
     */
    ಗಾಗಿ (ಸುತ್ತು = 1; ಸುತ್ತು <= 2; ++ಸುತ್ತು)
    {
	/*
	 * skip "contains"
	 */
	p = skipwhite(*arg + keylen);
	ಆದರೆ (*p != '=')
	{
	    semsg(_(e_missing_equal_sign_str), *arg);
	    ನಿಲ್ಲಿಸು;
	}
	p = skipwhite(p + 1);
	ಆದರೆ (ತುದಿಗಳು_excmd2(*arg, p))
	{
	    semsg(_(e_ಖಾಲಿಗಳು_ವಾದ_str), *arg);
	    ನಿಲ್ಲಿಸು;
	}

	/*
	 * parse the arguments after "contains"
	 */
	ಎಣಿಕೆ_2 = 0;
	ಆಗ (!ತುದಿಗಳು_excmd2(*arg, p))
	{
	    ಗಾಗಿ (ಕೊನೆ = p; *ಕೊನೆ && !VIM_ISWHITE(*ಕೊನೆ) && *ಕೊನೆ != ','; ++ಕೊನೆ)
		;
	    name = alloc(ಕೊನೆ - p + 3);	    // leave room for "^$"
	    ಆದರೆ (name == ಖಾಲಿ)
	    {
		failed = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	    vim_strncpy(name + 1, p, ಕೊನೆ - p);
	    ಆದರೆ (       STRCMP(name + 1, "ALLBUT") == 0
		    || STRCMP(name + 1, "ALL") == 0
		    || STRCMP(name + 1, "TOP") == 0
		    || STRCMP(name + 1, "CONTAINED") == 0)
	    {
		ಆದರೆ (TOUPPER_ASC(**arg) != 'C')
		{
		    semsg(_(e_str_not_allowed_ಇಲ್ಲಿ), name + 1);
		    failed = TRUE;
		    vim_free(name);
		    ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (ಎಣಿಕೆ_2 != 0)
		{
		    semsg(_(e_str_must_be_ಮೊದಲ_ಒಳ_ಒಳಗೊಂಡಿದೆ_list), name + 1);
		    failed = TRUE;
		    vim_free(name);
		    ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (name[1] == 'A')
		    id = SYNID_ALLBUT;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name[1] == 'T')
		    id = SYNID_ತುದಿ;
		ಇಲ್ಲದಿದ್ದರೆ
		    id = SYNID_CONTAINED;
		id += current_syn_inc_ಟ್ಯಾಗ್;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name[1] == '@')
	    {
		ಆದರೆ (skip)
		    id = -1;
		ಇಲ್ಲದಿದ್ದರೆ
		    id = syn_check_cluster(name + 2, (ಪೂರ್ಣಾಂಕ)(ಕೊನೆ - p - 1));
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		/*
		 * Handle full group name.
		 */
		ಆದರೆ (vim_strpbrk(name + 1, (char_u *)"\\.*^$~[") == ಖಾಲಿ)
		    id = syn_check_ಗುಂಪು(name + 1, (ಪೂರ್ಣಾಂಕ)(ಕೊನೆ - p));
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    /*
		     * Handle match of regexp with group names.
		     */
		    *name = '^';
		    STRCAT(name, "$");
		    regmatch.regprog = vim_regcomp(name, RE_MAGIC);
		    ಆದರೆ (regmatch.regprog == ಖಾಲಿ)
		    {
			failed = TRUE;
			vim_free(name);
			ನಿಲ್ಲಿಸು;
		    }

		    regmatch.rm_ic = TRUE;
		    id = 0;
		    ಗಾಗಿ (i = highlight_ಅಂಕೆ_groups(); --i >= 0; )
		    {
			ಆದರೆ (vim_regexec(&regmatch, highlight_ಗುಂಪು_name(i),
								  (colnr_T)0))
			{
			    ಆದರೆ (ಸುತ್ತು == 2)
			    {
				// Got more items than expected; can happen
				// when adding items that match:
				// "contains=a.*b,axb".
				// Go back to first round
				ಆದರೆ (ಎಣಿಕೆ_2 >= ಒಟ್ಟು_ಎಣಿಕೆ)
				{
				    vim_free(retval);
				    ಸುತ್ತು = 1;
				}
				ಇಲ್ಲದಿದ್ದರೆ
				    retval[ಎಣಿಕೆ_2] = i + 1;
			    }
			    ++ಎಣಿಕೆ_2;
			    id = -1;	    // remember that we found one
			}
		    }
		    vim_regfree(regmatch.regprog);
		}
	    }
	    vim_free(name);
	    ಆದರೆ (id == 0)
	    {
		semsg(_(e_unknown_ಗುಂಪು_name_str), p);
		failed = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (id > 0)
	    {
		ಆದರೆ (ಸುತ್ತು == 2)
		{
		    // Got more items than expected, go back to first round
		    ಆದರೆ (ಎಣಿಕೆ_2 >= ಒಟ್ಟು_ಎಣಿಕೆ)
		    {
			vim_free(retval);
			ಸುತ್ತು = 1;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			retval[ಎಣಿಕೆ_2] = id;
		}
		++ಎಣಿಕೆ_2;
	    }
	    p = skipwhite(ಕೊನೆ);
	    ಆದರೆ (*p != ',')
		ನಿಲ್ಲಿಸು;
	    p = skipwhite(p + 1);	// skip comma in between arguments
	}
	ಆದರೆ (failed)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (ಸುತ್ತು == 1)
	{
	    retval = ALLOC_MULT(ಗಿಡ್ಡ, ಎಣಿಕೆ_2 + 1);
	    ಆದರೆ (retval == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    retval[ಎಣಿಕೆ_2] = 0;	    // zero means end of the list
	    ಒಟ್ಟು_ಎಣಿಕೆ = ಎಣಿಕೆ_2;
	}
    }

    *arg = p;
    ಆದರೆ (failed || retval == ಖಾಲಿ)
    {
	vim_free(retval);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (*list == ಖಾಲಿ)
	*list = retval;
    ಇಲ್ಲದಿದ್ದರೆ
	vim_free(retval);	// list already found, don't overwrite it

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Make a copy of an ID list.
 */
    ಸ್ಥಿರ ಗಿಡ್ಡ *
copy_id_list(ಗಿಡ್ಡ *list)
{
    ಪೂರ್ಣಾಂಕ	    len;
    ಪೂರ್ಣಾಂಕ	    ಎಣಿಕೆ_2;
    ಗಿಡ್ಡ   *retval;

    ಆದರೆ (list == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಗಾಗಿ (ಎಣಿಕೆ_2 = 0; list[ಎಣಿಕೆ_2]; ++ಎಣಿಕೆ_2)
	;
    len = (ಎಣಿಕೆ_2 + 1) * ಗಾತ್ರ(ಗಿಡ್ಡ);
    retval = alloc(len);
    ಆದರೆ (retval != ಖಾಲಿ)
	mch_memmove(retval, list, (size_t)len);

    ಹಿಂತಿರುಗಿ retval;
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
#ವ್ಯಾಖ್ಯಾನಿಸು IDL_CACHE_ಗಾತ್ರ 16

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಗಿಡ್ಡ	*idl_ಕೀಲಿ;	// the list this entry resolves (NULL when empty)
    ಗಿಡ್ಡ	*idl_ids;	// sorted, cluster-expanded group IDs (allocated)
    ಪೂರ್ಣಾಂಕ		idl_ಎಣಿಕೆ;	// number of IDs in idl_ids
    ಗಿಡ್ಡ	idl_marker;	// leading ALLBUT/TOP/CONTAINED item, or 0
    bool	idl_usable;	// false: not cacheable, use the slow path
} idl_ನಮೂದು_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    idl_ನಮೂದು_T	ent[IDL_CACHE_ಗಾತ್ರ];
    ಪೂರ್ಣಾಂಕ		ಸುತ್ತು;		// round-robin replacement index
    ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ;		// index of the last returned entry (fast path)
} idl_cache_T;

/*
 * Collect the explicit group IDs of "list" into "ga", expanding clusters.
 * "seen" records visited clusters to break cycles and avoid duplicate work.
 * Return false when the list cannot be represented as a plain set (a nested
 * ALLBUT/TOP/CONTAINED marker) or on out of memory.
 */
    ಸ್ಥಿರ bool
idl_collect(ಗಿಡ್ಡ *list, garray_T *ga, garray_T *ನೋಡಿದ)
{
    ಆದರೆ (list == ಖಾಲಿ || list == ID_LIST_ALL)
	ಹಿಂತಿರುಗಿ false;
    ಗಾಗಿ (ಗಿಡ್ಡ ಅಂಶ = *list; ಅಂಶ != 0; ಅಂಶ = *++list)
    {
	ಆದರೆ (ಅಂಶ >= SYNID_ALLBUT && ಅಂಶ < SYNID_CLUSTER)
	    ಹಿಂತಿರುಗಿ false;	// nested ALLBUT/TOP/CONTAINED: not cacheable
	ಆದರೆ (ಅಂಶ >= SYNID_CLUSTER)
	{
	    ಗಿಡ್ಡ	*scl_list;
	    bool	ನೋಡಿದ_it = false;

	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ನೋಡಿದ->ga_len; ++i)
		ಆದರೆ (((ಗಿಡ್ಡ *)ನೋಡಿದ->ga_ದತ್ತಾಂಶ)[i] == ಅಂಶ)
		{
		    ನೋಡಿದ_it = true;
		    ನಿಲ್ಲಿಸು;
		}
	    ಆದರೆ (ನೋಡಿದ_it)
		ಮುಂದುವರಿ;
	    ಆದರೆ (ga_grow(ನೋಡಿದ, 1) == FAIL)
		ಹಿಂತಿರುಗಿ false;
	    ((ಗಿಡ್ಡ *)ನೋಡಿದ->ga_ದತ್ತಾಂಶ)[ನೋಡಿದ->ga_len++] = ಅಂಶ;
	    // Record the cluster id itself, not just its members: the slow
	    // path matches "item == id" for a cluster id present in the list.
	    ಆದರೆ (ga_grow(ga, 1) == FAIL)
		ಹಿಂತಿರುಗಿ false;
	    ((ಗಿಡ್ಡ *)ga->ga_ದತ್ತಾಂಶ)[ga->ga_len++] = ಅಂಶ;
	    scl_list = SYN_CLSTR(syn_ಖಂಡ)[ಅಂಶ - SYNID_CLUSTER].scl_list;
	    ಆದರೆ (scl_list != ಖಾಲಿ && !idl_collect(scl_list, ga, ನೋಡಿದ))
		ಹಿಂತಿರುಗಿ false;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (ga_grow(ga, 1) == FAIL)
		ಹಿಂತಿರುಗಿ false;
	    ((ಗಿಡ್ಡ *)ga->ga_ದತ್ತಾಂಶ)[ga->ga_len++] = ಅಂಶ;
	}
    }
    ಹಿಂತಿರುಗಿ true;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
idl_id_cmp(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *a, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *b)
{
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)*(ಸ್ಥಿರಾಂಕ ಗಿಡ್ಡ *)a - (ಪೂರ್ಣಾಂಕ)*(ಸ್ಥಿರಾಂಕ ಗಿಡ್ಡ *)b;
}

/*
 * Get the cache entry that resolves "list", building it if needed.
 * Returns NULL only on allocation failure.
 */
    ಸ್ಥಿರ idl_ನಮೂದು_T *
idl_get_ನಮೂದು(ಗಿಡ್ಡ *list)
{
    idl_cache_T	*cache = (idl_cache_T *)syn_ಖಂಡ->b_idlist_cache;
    idl_ನಮೂದು_T	*e;
    garray_T	ga, ನೋಡಿದ;

    ಆದರೆ (cache == ಖಾಲಿ)
    {
	cache = ALLOC_CLEAR_ONE(idl_cache_T);
	ಆದರೆ (cache == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	syn_ಖಂಡ->b_idlist_cache = cache;
    }
    // Fast path: the same list is usually queried many times in a row.
    ಆದರೆ (cache->ent[cache->ಕೊನೆಯ].idl_ಕೀಲಿ == list)
	ಹಿಂತಿರುಗಿ &cache->ent[cache->ಕೊನೆಯ];
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < IDL_CACHE_ಗಾತ್ರ; ++i)
	ಆದರೆ (cache->ent[i].idl_ಕೀಲಿ == list)
	{
	    cache->ಕೊನೆಯ = i;
	    ಹಿಂತಿರುಗಿ &cache->ent[i];
	}

    // Build a new entry, replacing one in round-robin order.
    cache->ಕೊನೆಯ = cache->ಸುತ್ತು;
    e = &cache->ent[cache->ಸುತ್ತು];
    cache->ಸುತ್ತು = (cache->ಸುತ್ತು + 1) % IDL_CACHE_ಗಾತ್ರ;
    VIM_CLEAR(e->idl_ids);
    e->idl_ಎಣಿಕೆ = 0;
    e->idl_ಕೀಲಿ = list;
    e->idl_marker = 0;
    e->idl_usable = true;

    // A leading ALLBUT/TOP/CONTAINED marker is kept aside; the rest is the set.
    ಆದರೆ (*list >= SYNID_ALLBUT && *list < SYNID_CLUSTER)
    {
	e->idl_marker = *list;
	++list;
    }

    ga_init2(&ga, ಗಾತ್ರ(ಗಿಡ್ಡ), 50);
    ga_init2(&ನೋಡಿದ, ಗಾತ್ರ(ಗಿಡ್ಡ), 10);
    ಆದರೆ (!idl_collect(list, &ga, &ನೋಡಿದ))
    {
	e->idl_usable = false;
	ga_clear(&ga);
	ga_clear(&ನೋಡಿದ);
	ಹಿಂತಿರುಗಿ e;
    }
    ga_clear(&ನೋಡಿದ);
    ಆದರೆ (ga.ga_len > 0)
    {
	qsort(ga.ga_ದತ್ತಾಂಶ, (size_t)ga.ga_len, ಗಾತ್ರ(ಗಿಡ್ಡ), idl_id_cmp);
	e->idl_ids = (ಗಿಡ್ಡ *)ga.ga_ದತ್ತಾಂಶ;	// take ownership
	e->idl_ಎಣಿಕೆ = ga.ga_len;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ga_clear(&ga);
    ಹಿಂತಿರುಗಿ e;
}

/*
 * Return true if "id" is in the resolved set of cache entry "e".
 */
    ಸ್ಥಿರ bool
idl_ಒಳಗೊಂಡಿದೆ(idl_ನಮೂದು_T *e, ಗಿಡ್ಡ id)
{
    ಪೂರ್ಣಾಂಕ	lo = 0;
    ಪೂರ್ಣಾಂಕ	hi = e->idl_ಎಣಿಕೆ - 1;

    ಆಗ (lo <= hi)
    {
	ಪೂರ್ಣಾಂಕ	mid = (lo + hi) / 2;
	ಗಿಡ್ಡ	v = e->idl_ids[mid];

	ಆದರೆ (v == id)
	    ಹಿಂತಿರುಗಿ true;
	ಆದರೆ (v < id)
	    lo = mid + 1;
	ಇಲ್ಲದಿದ್ದರೆ
	    hi = mid - 1;
    }
    ಹಿಂತಿರುಗಿ false;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
idl_cache_clear(synblock_T *ಖಂಡ)
{
    idl_cache_T	*cache = (idl_cache_T *)ಖಂಡ->b_idlist_cache;

    ಆದರೆ (cache == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < IDL_CACHE_ಗಾತ್ರ; ++i)
	vim_free(cache->ent[i].idl_ids);
    vim_free(cache);
    ಖಂಡ->b_idlist_cache = ಖಾಲಿ;
}
// }}}

/*
 * Check if syntax group "ssp" is in the ID list "list" of "cur_si".
 * "cur_si" can be NULL if not checking the "containedin" list.
 * Used to check if a syntax item is in the "contains" or "nextgroup" list of
 * the current item.
 * This function is called very often, keep it fast!!
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಒಳ_id_list(
    stateitem_T	*cur_si,	// current item or NULL
    ಗಿಡ್ಡ	*list,		// id list
    ರಚನೆ sp_syn *ssp,		// group id and ":syn include" tag of group
    ಪೂರ್ಣಾಂಕ		flags)		// group flags
{
    ಪೂರ್ಣಾಂಕ		retval;
    ಗಿಡ್ಡ	*scl_list;
    ಗಿಡ್ಡ	ಅಂಶ;
    ಗಿಡ್ಡ	id = ssp->id;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಆಳ = 0;
    ಪೂರ್ಣಾಂಕ		r;
    ಪೂರ್ಣಾಂಕ		toplevel;
    idl_ನಮೂದು_T	*e;

    // If ssp has a "containedin" list and "cur_si" is in it, return TRUE.
    ಆದರೆ (cur_si != ಖಾಲಿ && ssp->cont_ಒಳ_list != ಖಾಲಿ
					    && !(cur_si->si_flags & HL_ಹೊಂದಿಕೆ))
    {
	// Ignore transparent items without a contains argument.  Double check
	// that we don't go back past the first one.
	ಆಗ ((cur_si->si_flags & HL_TRANS_CONT)
		&& cur_si > (stateitem_T *)(current_state.ga_ದತ್ತಾಂಶ))
	    --cur_si;
	// cur_si->si_idx is -1 for keywords, these never contain anything.
	ಆದರೆ (cur_si->si_idx >= 0 && ಒಳ_id_list(ಖಾಲಿ, ssp->cont_ಒಳ_list,
		&(SYN_ವಸ್ತುಗಳು(syn_ಖಂಡ)[cur_si->si_idx].sp_syn),
		  SYN_ವಸ್ತುಗಳು(syn_ಖಂಡ)[cur_si->si_idx].sp_flags))
	    ಹಿಂತಿರುಗಿ TRUE;
    }

    ಆದರೆ (list == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    /*
     * If list is ID_LIST_ALL, we are in a transparent item that isn't
     * inside anything.  Only allow not-contained groups.
     */
    ಆದರೆ (list == ID_LIST_ALL)
	ಹಿಂತಿರುಗಿ !(flags & HL_CONTAINED);

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
    e = disable_syn_idlist_cache_for_testing ? ಖಾಲಿ : idl_get_ನಮೂದು(list);
    ಆದರೆ (e != ಖಾಲಿ && e->idl_usable)
    {
	ಆದರೆ (e->idl_marker != 0)
	{
	    ಅಂಶ = e->idl_marker;
	    ಆದರೆ (ಅಂಶ < SYNID_ತುದಿ)
	    {
		ಆದರೆ (ಅಂಶ - SYNID_ALLBUT != ssp->inc_ಟ್ಯಾಗ್)
		    ಹಿಂತಿರುಗಿ FALSE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಂಶ < SYNID_CONTAINED)
	    {
		ಆದರೆ (ಅಂಶ - SYNID_ತುದಿ != ssp->inc_ಟ್ಯಾಗ್ || !toplevel)
		    ಹಿಂತಿರುಗಿ FALSE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (ಅಂಶ - SYNID_CONTAINED != ssp->inc_ಟ್ಯಾಗ್ || toplevel)
		    ಹಿಂತಿರುಗಿ FALSE;
	    }
	    retval = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    retval = TRUE;
	ಹಿಂತಿರುಗಿ idl_ಒಳಗೊಂಡಿದೆ(e, id) ? retval : !retval;
    }

    /*
     * Slow path (uncacheable list, e.g. a nested ALLBUT).
     * If the first item is "ALLBUT", return TRUE if "id" is NOT in the
     * contains list.  We also require that "id" is at the same ":syn include"
     * level as the list.
     */
    ಅಂಶ = *list;
    ಆದರೆ (ಅಂಶ >= SYNID_ALLBUT && ಅಂಶ < SYNID_CLUSTER)
    {
	ಆದರೆ (ಅಂಶ < SYNID_ತುದಿ)
	{
	    // ALL or ALLBUT: accept all groups in the same file
	    ಆದರೆ (ಅಂಶ - SYNID_ALLBUT != ssp->inc_ಟ್ಯಾಗ್)
		ಹಿಂತಿರುಗಿ FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಂಶ < SYNID_CONTAINED)
	{
	    // TOP: accept all not-contained groups in the same file
	    ಆದರೆ (ಅಂಶ - SYNID_ತುದಿ != ssp->inc_ಟ್ಯಾಗ್ || !toplevel)
		ಹಿಂತಿರುಗಿ FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // CONTAINED: accept all contained groups in the same file
	    ಆದರೆ (ಅಂಶ - SYNID_CONTAINED != ssp->inc_ಟ್ಯಾಗ್ || toplevel)
		ಹಿಂತಿರುಗಿ FALSE;
	}
	ಅಂಶ = *++list;
	retval = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	retval = TRUE;

    /*
     * Return "retval" if id is in the contains list.
     */
    ಆಗ (ಅಂಶ != 0)
    {
	ಆದರೆ (ಅಂಶ == id)
	    ಹಿಂತಿರುಗಿ retval;
	ಆದರೆ (ಅಂಶ >= SYNID_CLUSTER)
	{
	    scl_list = SYN_CLSTR(syn_ಖಂಡ)[ಅಂಶ - SYNID_CLUSTER].scl_list;
	    // restrict recursiveness to 30 to avoid an endless loop for a
	    // cluster that includes itself (indirectly)
	    ಆದರೆ (scl_list != ಖಾಲಿ && ಆಳ < 30)
	    {
		++ಆಳ;
		r = ಒಳ_id_list(ಖಾಲಿ, scl_list, ssp, flags);
		--ಆಳ;
		ಆದರೆ (r)
		    ಹಿಂತಿರುಗಿ retval;
	    }
	}
	ಅಂಶ = *++list;
    }
    ಹಿಂತಿರುಗಿ !retval;
}

ರಚನೆ subcommand
{
    ಅಕ್ಷರ    *name;			// subcommand name
    ಶೂನ್ಯ    (*func)(exarg_T *, ಪೂರ್ಣಾಂಕ);	// function to call
};

ಸ್ಥಿರ ರಚನೆ subcommand subcommands[] =
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
    {"match",		syn_cmd_ಹೊಂದಿಕೆ},
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
    ಶೂನ್ಯ
ex_syntax(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*subcmd_ಕೊನೆ;
    char_u	*subcmd_name;
    ಪೂರ್ಣಾಂಕ		i;

    syn_cmdlinep = eap->cmdlinep;

    // isolate subcommand name
    ಗಾಗಿ (subcmd_ಕೊನೆ = arg; ASCII_ISALPHA(*subcmd_ಕೊನೆ); ++subcmd_ಕೊನೆ)
	;
    subcmd_name = vim_strnsave(arg, subcmd_ಕೊನೆ - arg);
    ಆದರೆ (subcmd_name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (eap->skip)		// skip error messages for all subcommands
	++emsg_skip;
    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(subcommands); ++i)
    {
	ಆದರೆ (STRCMP(subcmd_name, (char_u *)subcommands[i].name) == 0)
	{
	    eap->arg = skipwhite(subcmd_ಕೊನೆ);
	    (subcommands[i].func)(eap, FALSE);
	    ನಿಲ್ಲಿಸು;
	}
    }

    ಆದರೆ (i == (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(subcommands))
	semsg(_(e_invalid_syntax_subcommand_str), subcmd_name);

    vim_free(subcmd_name);
    ಆದರೆ (eap->skip)
	--emsg_skip;
}

    ಶೂನ್ಯ
ex_ownsyntax(exarg_T *eap)
{
    char_u	*old_ಮೌಲ್ಯ;
    char_u	*new_ಮೌಲ್ಯ_2;

    ಆದರೆ (curwin->w_s == &curwin->w_ಬಫರ್->b_s)
    {
	curwin->w_s = ALLOC_ONE(synblock_T);
	CLEAR_ಸೂಚಕ(curwin->w_s);
	hash_init(&curwin->w_s->b_keywtab);
	hash_init(&curwin->w_s->b_keywtab_ic);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
	// TODO: keep the spell checking as it was.
	curwin->w_p_spell = FALSE;	// No spell checking
	// make sure option values are "empty_option" instead of NULL
	clear_string_option(&curwin->w_s->b_p_spc);
	clear_string_option(&curwin->w_s->b_p_spf);
	clear_string_option(&curwin->w_s->b_p_spl);
	clear_string_option(&curwin->w_s->b_p_spo);
#ಆದರೆ_ಅಂತ್ಯ
	clear_string_option(&curwin->w_s->b_syn_isk);
    }

    // save value of b:current_syntax
    old_ಮೌಲ್ಯ = get_var_ಮೌಲ್ಯ((char_u *)"b:current_syntax");
    ಆದರೆ (old_ಮೌಲ್ಯ != ಖಾಲಿ)
	old_ಮೌಲ್ಯ = vim_strsave(old_ಮೌಲ್ಯ);

    // Apply the "syntax" autocommand event, this finds and loads the syntax
    // file.
    apply_autocmds(EVENT_SYNTAX, eap->arg, curbuf->b_fname, TRUE, curbuf);

    // move value of b:current_syntax to w:current_syntax
    new_ಮೌಲ್ಯ_2 = get_var_ಮೌಲ್ಯ((char_u *)"b:current_syntax");
    ಆದರೆ (new_ಮೌಲ್ಯ_2 != ಖಾಲಿ)
	ಗಣ_ಆಂತರಿಕ_string_var((char_u *)"w:current_syntax", new_ಮೌಲ್ಯ_2);

    // restore value of b:current_syntax
    ಆದರೆ (old_ಮೌಲ್ಯ == ಖಾಲಿ)
	do_unlet((char_u *)"b:current_syntax", TRUE);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಣ_ಆಂತರಿಕ_string_var((char_u *)"b:current_syntax", old_ಮೌಲ್ಯ);
	vim_free(old_ಮೌಲ್ಯ);
    }
}

    ಪೂರ್ಣಾಂಕ
syntax_present(win_T *win)
{
    ಹಿಂತಿರುಗಿ (win->w_s->b_syn_patterns.ga_len != 0
	    || win->w_s->b_syn_clusters.ga_len != 0
	    || win->w_s->b_keywtab.ht_ಬಳಸಿದ > 0
	    || win->w_s->b_keywtab_ic.ht_ಬಳಸಿದ > 0);
}


ಸ್ಥಿರ ಎಣಿಕೆ
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
    ಶೂನ್ಯ
reset_expand_highlight(ಶೂನ್ಯ)
{
    include_link = include_default = include_none = 0;
}

/*
 * Handle command line completion for :match and :echohl command: Add "NONE"
 * as highlight group.
 */
    ಶೂನ್ಯ
ಗಣ_context_ಒಳ_echohl_cmd(expand_T *xp, char_u *arg)
{
    xp->xp_context = EXPAND_HIGHLIGHT;
    xp->xp_ನಮೂನೆ = arg;
    include_none = 1;
}

/*
 * Handle command line completion for :syntax command.
 */
    ಶೂನ್ಯ
ಗಣ_context_ಒಳ_syntax_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    // Default: expand subcommands
    xp->xp_context = EXPAND_SYNTAX;
    expand_what = EXP_SUBCMD;
    xp->xp_ನಮೂನೆ = arg;
    include_link = 0;
    include_default = 0;

    ಆದರೆ (*arg == NUL)
	ಹಿಂತಿರುಗಿ;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    ಆದರೆ (*p == NUL)
	ಹಿಂತಿರುಗಿ;

    // past first word
    xp->xp_ನಮೂನೆ = skipwhite(p);
    ಆದರೆ (*skiptowhite(xp->xp_ನಮೂನೆ) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(arg, "case", p - arg) == 0)
	expand_what = EXP_CASE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(arg, "spell", p - arg) == 0)
	expand_what = EXP_SPELL;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(arg, "sync", p - arg) == 0)
	expand_what = EXP_SYNC;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(arg, "list", p - arg) == 0)
    {
	p = skipwhite(p);
	ಆದರೆ (*p == '@')
	    expand_what = EXP_CLUSTER;
	ಇಲ್ಲದಿದ್ದರೆ
	    xp->xp_context = EXPAND_HIGHLIGHT;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(arg, "keyword", p - arg) == 0
	    || STRNICMP(arg, "region", p - arg) == 0
	    || STRNICMP(arg, "match", p - arg) == 0)
	xp->xp_context = EXPAND_HIGHLIGHT;
    ಇಲ್ಲದಿದ್ದರೆ
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Function given to ExpandGeneric() to obtain the list syntax names for
 * expansion.
 */
    char_u *
get_syntax_name(expand_T *xp, ಪೂರ್ಣಾಂಕ idx)
{
    ಆಯ್ಕೆ (expand_what)
    {
	ಸ್ಥಿತಿ EXP_SUBCMD:
	    ಆದರೆ (idx < 0 || idx >= (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(subcommands))
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    ಹಿಂತಿರುಗಿ (char_u *)subcommands[idx].name;
	ಸ್ಥಿತಿ EXP_CASE:
	{
	    ಸ್ಥಿರ ಅಕ್ಷರ *case_ವಾದಗಳು[] = {"match", "ignore", ಖಾಲಿ};
	    ಹಿಂತಿರುಗಿ (char_u *)case_ವಾದಗಳು[idx];
	}
	ಸ್ಥಿತಿ EXP_SPELL:
	{
	    ಸ್ಥಿರ ಅಕ್ಷರ *spell_ವಾದಗಳು[] =
		{"toplevel", "notoplevel", "default", ಖಾಲಿ};
	    ಹಿಂತಿರುಗಿ (char_u *)spell_ವಾದಗಳು[idx];
	}
	ಸ್ಥಿತಿ EXP_SYNC:
	{
	    ಸ್ಥಿರ ಅಕ್ಷರ *sync_ವಾದಗಳು[] =
		{"ccomment", "clear", "fromstart",
		 "linebreaks=", "linecont", "lines=", "match",
		 "maxlines=", "minlines=", "region", ಖಾಲಿ};
	    ಹಿಂತಿರುಗಿ (char_u *)sync_ವಾದಗಳು[idx];
	}
	ಸ್ಥಿತಿ EXP_CLUSTER:
	{
	    ಆದರೆ (idx < curwin->w_s->b_syn_clusters.ga_len)
	    {
		vim_snprintf((ಅಕ್ಷರ *)xp->xp_buf, EXPAND_BUF_LEN, "@%s",
					 SYN_CLSTR(curwin->w_s)[idx].scl_name);
		ಹಿಂತಿರುಗಿ xp->xp_buf;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}


/*
 * Function called for expression evaluation: get syntax ID at file position.
 */
    ಪೂರ್ಣಾಂಕ
syn_get_id(
    win_T	*wp,
    ದೀರ್ಘ	lnum,
    colnr_T	ಲಂಬಸಾಲು,
    ಪೂರ್ಣಾಂಕ		trans,	     // remove transparency
    ಪೂರ್ಣಾಂಕ		*spellp,     // return: can do spell checking
    ಪೂರ್ಣಾಂಕ		keep_state)  // keep state of char at "col"
{
    // When the position is not after the current position and in the same
    // line of the same window with the same buffer, need to restart parsing.
    ಆದರೆ (wp != syn_win
	    || wp->w_ಬಫರ್ != syn_buf
	    || lnum != current_lnum
	    || ಲಂಬಸಾಲು < current_ಲಂಬಸಾಲು)
	syntax_ಆರಂಭ(wp, lnum);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_ಬಫರ್ == syn_buf
	    && lnum == current_lnum
	    && ಲಂಬಸಾಲು > current_ಲಂಬಸಾಲು)
	// next_match may not be correct when moving around, e.g. with the
	// "skip" expression in searchpair()
	ಮುಂದಿನ_ಹೊಂದಿಕೆ_idx = -1;

    (ಶೂನ್ಯ)get_syntax_attr(ಲಂಬಸಾಲು, spellp, keep_state);

    ಹಿಂತಿರುಗಿ (trans ? current_trans_id : current_id);
}

#ಆದರೆ defined(FEAT_CONCEAL)
/*
 * Get extra information about the syntax item.  Must be called right after
 * get_syntax_attr().
 * Stores the current item sequence nr in "*seqnrp".
 * Returns the current flags.
 */
    ಪೂರ್ಣಾಂಕ
get_syntax_info(ಪೂರ್ಣಾಂಕ *seqnrp)
{
    *seqnrp = current_seqnr;
    ಹಿಂತಿರುಗಿ current_flags;
}

/*
 * Return conceal substitution character
 */
    ಪೂರ್ಣಾಂಕ
syn_get_sub_char(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ current_sub_char;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Return the syntax ID at position "i" in the current stack.
 * The caller must have called syn_get_id() before to fill the stack.
 * Returns -1 when "i" is out of range.
 */
    ಪೂರ್ಣಾಂಕ
syn_get_ರಾಶಿ_ಅಂಶ(ಪೂರ್ಣಾಂಕ i)
{
    ಆದರೆ (i >= current_state.ga_len)
    {
	// Need to invalidate the state, because we didn't properly finish it
	// for the last character, "keep_state" was TRUE.
	invalidate_current_state();
	current_ಲಂಬಸಾಲು = MAXCOL;
	ಹಿಂತಿರುಗಿ -1;
    }
    ಹಿಂತಿರುಗಿ CUR_STATE(i).si_id;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_FOLDING)
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
syn_cur_foldlevel(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		level = 0;
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 0; i < current_state.ga_len; ++i)
	ಆದರೆ (CUR_STATE(i).si_flags & HL_FOLD)
	    ++level;
    ಹಿಂತಿರುಗಿ level;
}

/*
 * Function called to get folding level for line "lnum" in window "wp".
 */
    ಪೂರ್ಣಾಂಕ
syn_get_foldlevel(win_T *wp, ದೀರ್ಘ lnum)
{
    ಪೂರ್ಣಾಂಕ		level = 0;
    ಪೂರ್ಣಾಂಕ		ಕೆಳಗಿನ_level;
    ಪೂರ್ಣಾಂಕ		cur_level;

    // Return quickly when there are no fold items at all.
    ಆದರೆ (wp->w_s->b_syn_folditems != 0
	    && !wp->w_s->b_syn_error
# ifdef SYN_ಸಮಯ_ಎಲ್ಲೆ
	    && !wp->w_s->b_syn_slow
# endif
	    )
    {
	syntax_ಆರಂಭ(wp, lnum);

	// Start with the fold level at the start of the line.
	level = syn_cur_foldlevel();

	ಆದರೆ (wp->w_s->b_syn_foldlevel == SYNFLD_MINIMUM)
	{
	    // Find the lowest fold level that is followed by a higher one.
	    cur_level = level;
	    ಕೆಳಗಿನ_level = cur_level;
	    ಆಗ (!current_ಪೂರ್ಣಗೊಂಡ)
	    {
		(ಶೂನ್ಯ)syn_current_attr(FALSE, FALSE, ಖಾಲಿ, FALSE);
		cur_level = syn_cur_foldlevel();
		ಆದರೆ (cur_level < ಕೆಳಗಿನ_level)
		    ಕೆಳಗಿನ_level = cur_level;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cur_level > ಕೆಳಗಿನ_level)
		    level = ಕೆಳಗಿನ_level;
		++current_ಲಂಬಸಾಲು;
	    }
	}
    }
    ಆದರೆ (level > wp->w_p_fdn)
    {
	level = wp->w_p_fdn;
	ಆದರೆ (level < 0)
	    level = 0;
    }
    ಹಿಂತಿರುಗಿ level;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_PROFILE)
/*
 * ":syntime".
 */
    ಶೂನ್ಯ
ex_syntime(exarg_T *eap)
{
    ಆದರೆ (STRCMP(eap->arg, "on") == 0)
	syn_ಸಮಯ_on = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(eap->arg, "off") == 0)
	syn_ಸಮಯ_on = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(eap->arg, "clear") == 0)
	syntime_clear();
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(eap->arg, "report") == 0)
	syntime_report();
    ಇಲ್ಲದಿದ್ದರೆ
	semsg(_(e_invalid_ವಾದ_str), eap->arg);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
syn_clear_ಸಮಯ(syn_ಸಮಯ_T *st)
{
    profile_zero(&st->ಒಟ್ಟು);
    profile_zero(&st->slowest);
    st->ಎಣಿಕೆ_2 = 0;
    st->ಹೊಂದಿಕೆ = 0;
}

/*
 * Clear the syntax timing for the current buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syntime_clear(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		idx;
    synpat_T	*spp;

    ಆದರೆ (!syntax_present(curwin))
    {
	ಸಂದೇಶ(_(ಸಂದೇಶ_no_ವಸ್ತುಗಳು));
	ಹಿಂತಿರುಗಿ;
    }
    ಗಾಗಿ (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len; ++idx)
    {
	spp = &(SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx]);
	syn_clear_ಸಮಯ(&spp->sp_ಸಮಯ);
    }
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":syntime {on,off,clear,report}" command.
 */
    char_u *
get_syntime_arg(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಆಯ್ಕೆ (idx)
    {
	ಸ್ಥಿತಿ 0: ಹಿಂತಿರುಗಿ (char_u *)"on";
	ಸ್ಥಿತಿ 1: ಹಿಂತಿರುಗಿ (char_u *)"off";
	ಸ್ಥಿತಿ 2: ಹಿಂತಿರುಗಿ (char_u *)"clear";
	ಸ್ಥಿತಿ 3: ಹಿಂತಿರುಗಿ (char_u *)"report";
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    proftime_T	ಒಟ್ಟು;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2;
    ಪೂರ್ಣಾಂಕ		ಹೊಂದಿಕೆ;
    proftime_T	slowest;
    proftime_T	ಸರಾಸರಿ_ಮೌಲ್ಯ;
    ಪೂರ್ಣಾಂಕ		id;
    char_u	*ನಮೂನೆ;
} ಸಮಯ_ನಮೂದು_T;

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
syn_compare_syntime(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *v1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *v2)
{
    ಸ್ಥಿರಾಂಕ ಸಮಯ_ನಮೂದು_T	*s1 = v1;
    ಸ್ಥಿರಾಂಕ ಸಮಯ_ನಮೂದು_T	*s2 = v2;

    ಹಿಂತಿರುಗಿ profile_cmp(&s1->ಒಟ್ಟು, &s2->ಒಟ್ಟು);
}

/*
 * Clear the syntax timing for the current buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syntime_report(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		idx;
    synpat_T	*spp;
# ಆದರೆ defined(FEAT_RELTIME)
    proftime_T	tm;
# endif
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		patlen;
    proftime_T	ಒಟ್ಟು_ಒಟ್ಟು;
    ಪೂರ್ಣಾಂಕ		ಒಟ್ಟು_ಎಣಿಕೆ = 0;
    garray_T    ga;
    ಸಮಯ_ನಮೂದು_T *p;

    ಆದರೆ (!syntax_present(curwin))
    {
	ಸಂದೇಶ(_(ಸಂದೇಶ_no_ವಸ್ತುಗಳು));
	ಹಿಂತಿರುಗಿ;
    }

    ga_init2(&ga, ಗಾತ್ರ(ಸಮಯ_ನಮೂದು_T), 50);
    profile_zero(&ಒಟ್ಟು_ಒಟ್ಟು);
    ಗಾಗಿ (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len; ++idx)
    {
	spp = &(SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx]);
	ಆದರೆ (spp->sp_ಸಮಯ.ಎಣಿಕೆ_2 > 0)
	{
	    (ಶೂನ್ಯ)ga_grow(&ga, 1);
	    p = ((ಸಮಯ_ನಮೂದು_T *)ga.ga_ದತ್ತಾಂಶ) + ga.ga_len;
	    p->ಒಟ್ಟು = spp->sp_ಸಮಯ.ಒಟ್ಟು;
	    profile_ಕೂಡಿಸು(&ಒಟ್ಟು_ಒಟ್ಟು, &spp->sp_ಸಮಯ.ಒಟ್ಟು);
	    p->ಎಣಿಕೆ_2 = spp->sp_ಸಮಯ.ಎಣಿಕೆ_2;
	    p->ಹೊಂದಿಕೆ = spp->sp_ಸಮಯ.ಹೊಂದಿಕೆ;
	    ಒಟ್ಟು_ಎಣಿಕೆ += spp->sp_ಸಮಯ.ಎಣಿಕೆ_2;
	    p->slowest = spp->sp_ಸಮಯ.slowest;
# ಆದರೆ defined(FEAT_RELTIME)
	    profile_divide(&spp->sp_ಸಮಯ.ಒಟ್ಟು, spp->sp_ಸಮಯ.ಎಣಿಕೆ_2, &tm);
	    p->ಸರಾಸರಿ_ಮೌಲ್ಯ = tm;
# endif
	    p->id = spp->sp_syn.id;
	    p->ನಮೂನೆ = spp->sp_ನಮೂನೆ;
	    ++ga.ga_len;
	}
    }

    // Sort on total time. Skip if there are no items to avoid passing NULL
    // pointer to qsort().
    ಆದರೆ (ga.ga_len > 1)
	qsort(ga.ga_ದತ್ತಾಂಶ, (size_t)ga.ga_len, ಗಾತ್ರ(ಸಮಯ_ನಮೂದು_T),
							 syn_compare_syntime);

    ಸಂದೇಶ_puts_title(_("  TOTAL      COUNT  MATCH   SLOWEST     AVERAGE   NAME               PATTERN"));
    ಸಂದೇಶ_puts("\n");
    ಗಾಗಿ (idx = 0; idx < ga.ga_len && !got_int; ++idx)
    {
	p = ((ಸಮಯ_ನಮೂದು_T *)ga.ga_ದತ್ತಾಂಶ) + idx;

	ಸಂದೇಶ_puts(profile_ಸಂದೇಶ(&p->ಒಟ್ಟು));
	ಸಂದೇಶ_puts(" "); // make sure there is always a separating space
	ಸಂದೇಶ_advance(13);
	ಸಂದೇಶ_outnum(p->ಎಣಿಕೆ_2);
	ಸಂದೇಶ_puts(" ");
	ಸಂದೇಶ_advance(20);
	ಸಂದೇಶ_outnum(p->ಹೊಂದಿಕೆ);
	ಸಂದೇಶ_puts(" ");
	ಸಂದೇಶ_advance(26);
	ಸಂದೇಶ_puts(profile_ಸಂದೇಶ(&p->slowest));
	ಸಂದೇಶ_puts(" ");
	ಸಂದೇಶ_advance(38);
	ಸಂದೇಶ_puts(profile_ಸಂದೇಶ(&p->ಸರಾಸರಿ_ಮೌಲ್ಯ));
	ಸಂದೇಶ_puts(" ");
	ಸಂದೇಶ_advance(50);
	ಸಂದೇಶ_outtrans(highlight_ಗುಂಪು_name(p->id - 1));
	ಸಂದೇಶ_puts(" ");

	ಸಂದೇಶ_advance(69);
	ಆದರೆ (Columns < 80)
	    len = 20; // will wrap anyway
	ಇಲ್ಲದಿದ್ದರೆ
	    len = Columns - 70;
	patlen = (ಪೂರ್ಣಾಂಕ)STRLEN(p->ನಮೂನೆ);
	ಆದರೆ (len > patlen)
	    len = patlen;
	ಸಂದೇಶ_outtrans_len(p->ನಮೂನೆ, len);
	ಸಂದೇಶ_puts("\n");
    }
    ga_clear(&ga);
    ಆದರೆ (!got_int)
    {
	ಸಂದೇಶ_puts("\n");
	ಸಂದೇಶ_puts(profile_ಸಂದೇಶ(&ಒಟ್ಟು_ಒಟ್ಟು));
	ಸಂದೇಶ_advance(13);
	ಸಂದೇಶ_outnum(ಒಟ್ಟು_ಎಣಿಕೆ);
	ಸಂದೇಶ_puts("\n");
    }
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ // FEAT_SYN_HL
