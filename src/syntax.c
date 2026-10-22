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

#ಯದಿ defined(FEAT_SYN_HL)

// different types of offsets that are possible
#define SPO_MS_OFF	0	// match  start offset
#define SPO_ME_OFF	1	// match  end	offset
#define SPO_HS_OFF	2	// highl. start offset
#define SPO_HE_OFF	3	// highl. end	offset
#define SPO_RS_OFF	4	// region start offset
#define SPO_RE_OFF	5	// region end	offset
#define SPO_LC_OFF	6	// leading context offset
#define SPO_ಎಣಿಕೆ	7

ಸ್ಥಿರ ವರ್ಣ *(spo_name_tab[SPO_ಎಣಿಕೆ]) =
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
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ syn_pattern
{
    ವರ್ಣ	 sp_type;		// see SPTYPE_ defines below
    ವರ್ಣ	 sp_syncing;		// this item used for syncing
    short	 sp_syn_match_id;	// highlight group ID of pattern
    short	 sp_off_flags;		// see below
    ಪೂರ್ಣಾಙ್ಕ		 sp_offsets[SPO_ಎಣಿಕೆ];	// offsets
    ಪೂರ್ಣಾಙ್ಕ		 sp_flags;		// see HL_ defines below
#ifdef FEAT_CONCEAL
    ಪೂರ್ಣಾಙ್ಕ		 sp_cchar;		// conceal substitute character
#endif
    ಪೂರ್ಣಾಙ್ಕ		 sp_ic;			// ignore-case flag for sp_prog
    ಪೂರ್ಣಾಙ್ಕ		 sp_sync_idx;		// sync item index (syncing only)
    ಪೂರ್ಣಾಙ್ಕ		 sp_ಸಾಲು_id;		// ID of last line where tried
    ಪೂರ್ಣಾಙ್ಕ		 sp_startcol;		// next match in sp_line_id line
    short	*sp_cont_list;		// cont. group IDs, if non-zero
    short	*sp_next_list;		// next group IDs, if non-zero
    ಸಂರಚನಾ sp_syn sp_syn;		// struct passed to in_id_list()
    char_u	*sp_pattern;		// regexp to match, pattern
    regprog_T	*sp_prog;		// regexp to match, program
#ifdef FEAT_PROFILE
    syn_time_T	 sp_time;
#endif
} synpat_T;

// The sp_off_flags are computed like this:
// offset from the start of the matched text: (1 << SPO_XX_OFF)
// offset from the end	 of the matched text: (1 << (SPO_XX_OFF + SPO_COUNT))
// When both are present, only one is used.

#define SPTYPE_MATCH	1	// match keyword with this group ID
#define SPTYPE_START	2	// match a regexp, start of item
#define SPTYPE_END	3	// match a regexp, end of item
#define SPTYPE_SKIP	4	// match a regexp, skip within item


#define SYN_ವಸ್ತುಗಳು(buf)	((synpat_T *)((buf)->b_syn_patterns.ga_data))

#define NONE_IDX	(-2)	// value of sp_sync_idx for "NONE"

/*
 * Flags for b_syn_sync_flags:
 */
#define SF_CCOMMENT	0x01	// sync on a C-style comment
#define SF_MATCH	0x02	// sync by matching a pattern

#define SYN_STATE_P(ssp)    ((bufstate_T *)((ssp)->ga_data))

#define MAXKEYWLEN	80	    // maximum length of a keyword

/*
 * The attributes of the syntax item that has been recognized.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ current_attr = 0;	    // attr of current syntax word
#ifdef FEAT_EVAL
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ current_id = 0;	    // ID of current char for syn_get_id()
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ current_trans_id = 0;    // idem, transparency removed
#endif
#ifdef FEAT_CONCEAL
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ current_flags = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ current_seqnr = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ current_sub_char = 0;
#endif

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ syn_cluster_S
{
    char_u	    *scl_name;	    // syntax cluster name
    char_u	    *scl_name_u;    // uppercase of scl_name
    short	    *scl_list;	    // IDs in this syntax cluster
} syn_cluster_T;

/*
 * Methods of combining two clusters
 */
#define CLUSTER_REPLACE	    1	// replace first list with second
#define CLUSTER_ADD	    2	// add second list to first
#define CLUSTER_SUBTRACT    3	// subtract second list from first

#define SYN_CLSTR(buf)	((syn_cluster_T *)((buf)->b_syn_clusters.ga_data))

/*
 * Syntax group IDs have different types:
 *     0 - 19999  normal syntax groups
 * 20000 - 20999  ALLBUT indicator (current_syn_inc_tag added)
 * 21000 - 21999  TOP indicator (current_syn_inc_tag added)
 * 22000 - 22999  CONTAINED indicator (current_syn_inc_tag added)
 * 23000 - 32767  cluster IDs (subtract SYNID_CLUSTER for the cluster ID)
 */
#define SYNID_ALLBUT	MAX_HL_ID   // syntax group ID for contains=ALLBUT
#define SYNID_ತುದಿ	21000	    // syntax group ID for contains=TOP
#define SYNID_CONTAINED	22000	    // syntax group ID for contains=CONTAINED
#define SYNID_CLUSTER	23000	    // first syntax group ID for clusters

#define MAX_SYN_INC_TAG	999	    // maximum before the above overflow
#define MAX_CLUSTER_ID  (32767 - SYNID_CLUSTER)

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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ current_syn_inc_tag = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ running_syn_inc_tag = 0;

/*
 * In a hashtable item "hi_key" points to "keyword" in a keyentry.
 * This avoids adding a pointer to the hashtable item.
 * KE2HIKEY() converts a var pointer to a hashitem key pointer.
 * HIKEY2KE() converts a hashitem key pointer to a var pointer.
 * HI2KE() converts a hashitem pointer to a var pointer.
 */
ಸ್ಥಿರ keyentry_T dumkey;
#define KE2HIKEY(kp)  ((kp)->keyword)
#define HIKEY2KE(p)   ((keyentry_T *)((p) - (dumkey.keyword - (char_u *)&dumkey)))
#define HI2KE(hi)      HIKEY2KE((hi)->hi_ಕೀಲಿ)

/*
 * To reduce the time spent in keepend(), remember at which level in the state
 * stack the first item with "keepend" is present.  When "-1", there is no
 * "keepend" on the stack.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ keepend_level = -1;

ಸ್ಥಿರ ವರ್ಣ ಸಂದೇಶ_no_ವಸ್ತುಗಳು[] = N_("No Syntax items defined for this buffer");

/*
 * For the current state we need to remember more than just the idx.
 * When si_m_endpos.lnum is 0, the items other than si_idx are unknown.
 * (The end positions have the column number of the next char)
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ state_item
{
    ಪೂರ್ಣಾಙ್ಕ		si_idx;			// index of syntax pattern or
					// KEYWORD_IDX
    ಪೂರ್ಣಾಙ್ಕ		si_id;			// highlight group ID for keywords
    ಪೂರ್ಣಾಙ್ಕ		si_trans_id;		// idem, transparency removed
    ಪೂರ್ಣಾಙ್ಕ		si_m_lnum;		// lnum of the match
    ಪೂರ್ಣಾಙ್ಕ		si_m_startcol;		// starting column of the match
    lpos_T	si_m_endpos;		// just after end posn of the match
    lpos_T	si_h_startpos;		// start position of the highlighting
    lpos_T	si_h_endpos;		// end position of the highlighting
    lpos_T	si_eoe_pos;		// end position of end pattern
    ಪೂರ್ಣಾಙ್ಕ		si_end_idx;		// group ID for end pattern or zero
    ಪೂರ್ಣಾಙ್ಕ		si_ends;		// if match ends before si_m_endpos
    ಪೂರ್ಣಾಙ್ಕ		si_attr;		// attributes in this state
    long	si_flags;		// HL_HAS_EOL flag in this state, and
					// HL_SKIP* for si_next_list
#ifdef FEAT_CONCEAL
    ಪೂರ್ಣಾಙ್ಕ		si_seqnr;		// sequence number
    ಪೂರ್ಣಾಙ್ಕ		si_cchar;		// substitution character for conceal
#endif
    short	*si_cont_list;		// list of contained groups
    short	*si_next_list;		// nextgroup IDs after this item ends
    reg_extmatch_T *si_extmatch;	// \z(...\) matches from start
					// pattern
} stateitem_T;

#define KEYWORD_IDX	(-1)	    // value of si_idx for keywords
#define ID_LIST_ALL	((short *)-1) // valid of si_cont_list for containing all
				    // but contained groups

#ifdef FEAT_CONCEAL
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ next_seqnr = 1;		// value to use for si_seqnr
#endif

/*
 * Struct to reduce the number of arguments to get_syn_options(), it's used
 * very often.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ಪೂರ್ಣಾಙ್ಕ		flags;		// flags for contained and transparent
    ಪೂರ್ಣಾಙ್ಕ		keyword;	// TRUE for ":syn keyword"
    ಪೂರ್ಣಾಙ್ಕ		*sync_idx;	// syntax item for "grouphere" argument, NULL
				// if not allowed
    ವರ್ಣ	has_cont_list;	// TRUE if "cont_list" can be used
    short	*cont_list;	// group IDs for "contains" argument
    short	*cont_in_list;	// group IDs for "containedin" argument
    short	*next_list;	// group IDs for "nextgroup" argument
} syn_opt_arg_T;

/*
 * The next possible match in the current line for any pattern is remembered,
 * to avoid having to try for a match in each column.
 * If next_match_idx == -1, not tried (in this line) yet.
 * If next_match_col == MAXCOL, no match found in this line.
 * (All end positions have the column of the char after the end)
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ next_match_col;		// column for start of next match
ಸ್ಥಿರ lpos_T next_match_m_endpos;	// position for end of next match
ಸ್ಥಿರ lpos_T next_match_h_startpos;	// pos. for highl. start of next match
ಸ್ಥಿರ lpos_T next_match_h_endpos;	// pos. for highl. end of next match
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ next_match_idx;		// index of matched item
ಸ್ಥಿರ long next_match_flags;		// flags for next match
ಸ್ಥಿರ lpos_T next_match_eos_pos;	// end of start pattn (start region)
ಸ್ಥಿರ lpos_T next_match_eoe_pos;	// pos. for end of end pattern
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ next_match_end_idx;		// ID of group for end pattn or zero
ಸ್ಥಿರ reg_extmatch_T *next_match_extmatch = ರಿಕ್ತ;

/*
 * A state stack is an array of integers or stateitem_T, stored in a
 * garray_T.  A state stack is invalid if its itemsize entry is zero.
 */
#define INVALID_STATE(ssp)  ((ssp)->ga_itemsize == 0)
#define VALID_STATE(ssp)    ((ssp)->ga_itemsize != 0)

#define FOR_ALL_SYNSTATES(sb, sst) \
    ಪ್ರತಿ ((sst) = (sb)->b_sst_first; (sst) != ರಿಕ್ತ; (sst) = (sst)->sst_next)

/*
 * The current state (within the line) of the recognition engine.
 * When current_state.ga_itemsize is 0 the current state is invalid.
 */
ಸ್ಥಿರ win_T	*syn_win;		// current window for highlighting
ಸ್ಥಿರ buf_T	*syn_buf;		// current buffer for highlighting
ಸ್ಥಿರ synblock_T *syn_block;		// current buffer for highlighting
ಸ್ಥಿರ linenr_T current_lnum = 0;	// lnum of current state
ಸ್ಥಿರ colnr_T	current_col = 0;	// column of current state
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	current_state_stored = 0; // TRUE if stored current state
					  // after setting current_finished
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	current_finished = 0;	// current line has been finished
ಸ್ಥಿರ garray_T current_state		// current stack of state_items
		= {0, 0, 0, 0, ರಿಕ್ತ};
ಸ್ಥಿರ short	*current_next_list = ರಿಕ್ತ; // when non-zero, nextgroup list
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	current_next_flags = 0; // flags for current_next_list
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	current_ಸಾಲು_id = 0;	// unique number for current line

#define CUR_STATE(idx)	((stateitem_T *)(current_state.ga_data))[idx]

ಸ್ಥಿರ ಶೂನ್ಯ syn_sync(win_T *wp, linenr_T lnum, synstate_T *last_valid);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ syn_match_linecont(linenr_T lnum);
ಸ್ಥಿರ ಶೂನ್ಯ syn_start_ಸಾಲು(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ syn_update_ends(ಪೂರ್ಣಾಙ್ಕ startofline);
ಸ್ಥಿರ ಶೂನ್ಯ syn_ರಾಶಿ_alloc(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ syn_ರಾಶಿ_cleanup(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ syn_ರಾಶಿ_free_entry(synblock_T *block, synstate_T *p);
ಸ್ಥಿರ synstate_T *syn_ರಾಶಿ_find_entry(linenr_T lnum);
ಸ್ಥಿರ synstate_T *store_current_state(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ load_current_state(synstate_T *from);
ಸ್ಥಿರ ಶೂನ್ಯ invalidate_current_state(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ syn_ರಾಶಿ_equal(synstate_T *sp);
ಸ್ಥಿರ ಶೂನ್ಯ validate_current_state(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ syn_finish_ಸಾಲು(ಪೂರ್ಣಾಙ್ಕ syncing);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ syn_current_attr(ಪೂರ್ಣಾಙ್ಕ syncing, ಪೂರ್ಣಾಙ್ಕ displaying, ಪೂರ್ಣಾಙ್ಕ *can_spell, ಪೂರ್ಣಾಙ್ಕ keep_state);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ did_match_already(ಪೂರ್ಣಾಙ್ಕ idx, garray_T *gap);
ಸ್ಥಿರ stateitem_T *ತಳ್ಳು_next_match(stateitem_T *cur_si);
ಸ್ಥಿರ ಶೂನ್ಯ check_state_ends(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ update_si_attr(ಪೂರ್ಣಾಙ್ಕ idx);
ಸ್ಥಿರ ಶೂನ್ಯ check_keepend(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ update_si_end(stateitem_T *sip, ಪೂರ್ಣಾಙ್ಕ startcol, ಪೂರ್ಣಾಙ್ಕ force);
ಸ್ಥಿರ short *copy_id_list(short *list);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ in_id_list(stateitem_T *item, short *cont_list, ಸಂರಚನಾ sp_syn *ssp, ಪೂರ್ಣಾಙ್ಕ flags);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ತಳ್ಳು_current_state(ಪೂರ್ಣಾಙ್ಕ idx);
ಸ್ಥಿರ ಶೂನ್ಯ ತೆಗೆ_current_state(ಶೂನ್ಯ);
#ifdef FEAT_PROFILE
ಸ್ಥಿರ ಶೂನ್ಯ syn_clear_time(syn_time_T *tt);
ಸ್ಥಿರ ಶೂನ್ಯ syntime_clear(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ syntime_report(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ syn_time_on = FALSE;
# define IF_SYN_TIME(p) (p)
#ಅನ್ಯಥಾ
# define IF_SYN_TIME(p) ರಿಕ್ತ
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ syn_time_T;
#endif

ಸ್ಥಿರ ಶೂನ್ಯ syn_ರಾಶಿ_apply_changes_block(synblock_T *block, buf_T *buf);
ಸ್ಥಿರ ಶೂನ್ಯ find_endpos(ಪೂರ್ಣಾಙ್ಕ idx, lpos_T *startpos, lpos_T *m_endpos, lpos_T *hl_endpos, long *flagsp, lpos_T *end_endpos, ಪೂರ್ಣಾಙ್ಕ *end_idx, reg_extmatch_T *start_ext);

ಸ್ಥಿರ ಶೂನ್ಯ limit_pos(lpos_T *pos, lpos_T *limit);
ಸ್ಥಿರ ಶೂನ್ಯ limit_pos_zero(lpos_T *pos, lpos_T *limit);
ಸ್ಥಿರ ಶೂನ್ಯ syn_add_end_off(lpos_T *result, regmmatch_T *regmatch, synpat_T *spp, ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ extra);
ಸ್ಥಿರ ಶೂನ್ಯ syn_add_start_off(lpos_T *result, regmmatch_T *regmatch, synpat_T *spp, ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ extra);
ಸ್ಥಿರ char_u *syn_getcurline(ಶೂನ್ಯ);
ಸ್ಥಿರ colnr_T syn_getcurline_len(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ syn_regexec(regmmatch_T *rmp, linenr_T lnum, colnr_T col, syn_time_T *st);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ check_keyword_id(char_u *ಸಾಲು, ಪೂರ್ಣಾಙ್ಕ startcol, ಪೂರ್ಣಾಙ್ಕ *endcol, long *flags, short **next_list, stateitem_T *cur_si, ಪೂರ್ಣಾಙ್ಕ *ccharp);
ಸ್ಥಿರ ಶೂನ್ಯ syn_remove_pattern(synblock_T *block, ಪೂರ್ಣಾಙ್ಕ idx);
ಸ್ಥಿರ ಶೂನ್ಯ syn_clear_pattern(synblock_T *block, ಪೂರ್ಣಾಙ್ಕ i);
ಸ್ಥಿರ ಶೂನ್ಯ syn_clear_cluster(synblock_T *block, ಪೂರ್ಣಾಙ್ಕ i);
ಸ್ಥಿರ ಶೂನ್ಯ syn_clear_one(ಪೂರ್ಣಾಙ್ಕ id, ಪೂರ್ಣಾಙ್ಕ syncing);
ಸ್ಥಿರ ಶೂನ್ಯ syn_cmd_onoff(exarg_T *eap, ವರ್ಣ *name);
ಸ್ಥಿರ ಶೂನ್ಯ syn_ಸಾಲುಗಳು_ಸಂದೇಶ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ syn_match_ಸಂದೇಶ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ syn_list_one(ಪೂರ್ಣಾಙ್ಕ id, ಪೂರ್ಣಾಙ್ಕ syncing, ಪೂರ್ಣಾಙ್ಕ link_only);
ಸ್ಥಿರ ಶೂನ್ಯ syn_list_cluster(ಪೂರ್ಣಾಙ್ಕ id);
ಸ್ಥಿರ ಶೂನ್ಯ put_id_list(char_u *name, short *list, ಪೂರ್ಣಾಙ್ಕ attr);
ಸ್ಥಿರ ಶೂನ್ಯ put_pattern(ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ c, synpat_T *spp, ಪೂರ್ಣಾಙ್ಕ attr);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ syn_list_keywords(ಪೂರ್ಣಾಙ್ಕ id, hashtab_T *ht, ಪೂರ್ಣಾಙ್ಕ did_header, ಪೂರ್ಣಾಙ್ಕ attr);
ಸ್ಥಿರ ಶೂನ್ಯ syn_clear_keyword(ಪೂರ್ಣಾಙ್ಕ id, hashtab_T *ht);
ಸ್ಥಿರ ಶೂನ್ಯ clear_keywtab(hashtab_T *ht);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ syn_scl_namen2id(char_u *linep, ಪೂರ್ಣಾಙ್ಕ len);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ syn_check_cluster(char_u *pp, ಪೂರ್ಣಾಙ್ಕ len);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ syn_add_cluster(char_u *name);
ಸ್ಥಿರ ಶೂನ್ಯ init_syn_patterns(ಶೂನ್ಯ);
ಸ್ಥಿರ char_u *get_syn_pattern(char_u *arg, synpat_T *ci);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ get_id_list(char_u **arg, ಪೂರ್ಣಾಙ್ಕ keylen, short **list, ಪೂರ್ಣಾಙ್ಕ skip);
ಸ್ಥಿರ ಶೂನ್ಯ syn_combine_list(short **clstr1, short **clstr2, ಪೂರ್ಣಾಙ್ಕ list_op);
ಸ್ಥಿರ ಶೂನ್ಯ idl_cache_clear(synblock_T *block);

/*
 * Start the syntax recognition for a line.  This function is normally called
 * from the screen updating, once for each displayed line.
 * The buffer is remembered in syn_buf, because get_syntax_attr() doesn't get
 * it.	Careful: curbuf and curwin are likely to point to another buffer and
 * window.
 */
    ಶೂನ್ಯ
syntax_start(win_T *wp, linenr_T lnum)
{
    synstate_T	*p;
    synstate_T	*last_valid = ರಿಕ್ತ;
    synstate_T	*last_min_valid = ರಿಕ್ತ;
    synstate_T	*sp, *prev = ರಿಕ್ತ;
    linenr_T	parsed_lnum;
    linenr_T	first_stored;
    ಪೂರ್ಣಾಙ್ಕ		dist;
    ಸ್ಥಿರ varnumber_T changedtick = 0;	// remember the last change ID

#ifdef FEAT_CONCEAL
    current_sub_char = NUL;
#endif

    /*
     * After switching buffers, invalidate current_state.
     * Also do this when a change was made, the current state may be invalid
     * then.
     */
    ಯದಿ (syn_block != wp->w_s
	    || syn_buf != wp->w_buffer
	    || changedtick != CHANGEDTICK(syn_buf))
    {
	invalidate_current_state();
	syn_buf = wp->w_buffer;
	syn_block = wp->w_s;
    }
    changedtick = CHANGEDTICK(syn_buf);
    syn_win = wp;

    /*
     * Allocate syntax stack when needed.
     */
    syn_ರಾಶಿ_alloc();
    ಯದಿ (syn_block->b_sst_array == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;		// out of memory
    syn_block->b_sst_lasttick = display_tick;

    /*
     * If the state of the end of the previous line is useful, store it.
     */
    ಯದಿ (VALID_STATE(&current_state)
	    && current_lnum < lnum
	    && current_lnum < syn_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	(ಶೂನ್ಯ)syn_finish_ಸಾಲು(FALSE);
	ಯದಿ (!current_state_stored)
	{
	    ++current_lnum;
	    (ಶೂನ್ಯ)store_current_state();
	}

	/*
	 * If the current_lnum is now the same as "lnum", keep the current
	 * state (this happens very often!).  Otherwise invalidate
	 * current_state and figure it out below.
	 */
	ಯದಿ (current_lnum != lnum)
	    invalidate_current_state();
    }
    ಅನ್ಯಥಾ
	invalidate_current_state();

    /*
     * Try to synchronize from a saved state in b_sst_array[].
     * Only do this if lnum is not before and not to far beyond a saved state.
     */
    ಯದಿ (INVALID_STATE(&current_state) && syn_block->b_sst_array != ರಿಕ್ತ)
    {
	// Find last valid saved state before start_lnum.
	FOR_ALL_SYNSTATES(syn_block, p)
	{
	    ಯದಿ (p->sst_lnum > lnum)
		ಭಙ್ಗ;
	    ಯದಿ (p->sst_lnum <= lnum && p->sst_change_lnum == 0)
	    {
		last_valid = p;
		ಯದಿ (p->sst_lnum >= lnum - syn_block->b_syn_sync_minlines)
		    last_min_valid = p;
	    }
	}
	ಯದಿ (last_min_valid != ರಿಕ್ತ)
	    load_current_state(last_min_valid);
    }

    /*
     * If "lnum" is before or far beyond a line with a saved state, need to
     * re-synchronize.
     */
    ಯದಿ (INVALID_STATE(&current_state))
    {
	syn_sync(wp, lnum, last_valid);
	ಯದಿ (current_lnum == 1)
	    // First line is always valid, no matter "minlines".
	    first_stored = 1;
	ಅನ್ಯಥಾ
	    // Need to parse "minlines" lines before state can be considered
	    // valid to store.
	    first_stored = current_lnum + syn_block->b_syn_sync_minlines;
    }
    ಅನ್ಯಥಾ
	first_stored = current_lnum;

    /*
     * Advance from the sync point or saved state until the current line.
     * Save some entries for syncing with later on.
     */
    ಯದಿ (syn_block->b_sst_len <= Rows)
	dist = 999999;
    ಅನ್ಯಥಾ
	dist = syn_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ / (syn_block->b_sst_len - Rows) + 1;
    ಯಾವತ್ (current_lnum < lnum)
    {
	syn_start_ಸಾಲು();
	(ಶೂನ್ಯ)syn_finish_ಸಾಲು(FALSE);
	++current_lnum;

	// If we parsed at least "minlines" lines or started at a valid
	// state, the current state is considered valid.
	ಯದಿ (current_lnum >= first_stored)
	{
	    // Check if the saved state entry is for the current line and is
	    // equal to the current state.  If so, then validate all saved
	    // states that depended on a change before the parsed line.
	    ಯದಿ (prev == ರಿಕ್ತ)
		prev = syn_ರಾಶಿ_find_entry(current_lnum - 1);
	    ಯದಿ (prev == ರಿಕ್ತ)
		sp = syn_block->b_sst_first;
	    ಅನ್ಯಥಾ
		sp = prev;
	    ಯಾವತ್ (sp != ರಿಕ್ತ && sp->sst_lnum < current_lnum)
		sp = sp->sst_next;
	    ಯದಿ (sp != ರಿಕ್ತ
		    && sp->sst_lnum == current_lnum
		    && syn_ರಾಶಿ_equal(sp))
	    {
		parsed_lnum = current_lnum;
		prev = sp;
		ಯಾವತ್ (sp != ರಿಕ್ತ && sp->sst_change_lnum <= parsed_lnum)
		{
		    ಯದಿ (sp->sst_lnum <= lnum)
			// valid state before desired line, use this one
			prev = sp;
		    ಅನ್ಯಥಾ ಯದಿ (sp->sst_change_lnum == 0)
			// past saved states depending on change, break here.
			ಭಙ್ಗ;
		    sp->sst_change_lnum = 0;
		    sp = sp->sst_next;
		}
		load_current_state(prev);
	    }
	    // Store the state at this line when it's the first one, the line
	    // where we start parsing, or some distance from the previously
	    // saved state.  But only when parsed at least 'minlines'.
	    ಅನ್ಯಥಾ ಯದಿ (prev == ರಿಕ್ತ
			|| current_lnum == lnum
			|| current_lnum >= prev->sst_lnum + dist)
		prev = store_current_state();
	}

	// This can take a long time: break when CTRL-C pressed.  The current
	// state will be wrong then.
	ಸಾಲು_breakcheck();
	ಯದಿ (got_int)
	{
	    current_lnum = lnum;
	    ಭಙ್ಗ;
	}
    }

    syn_start_ಸಾಲು();
}

/*
 * We cannot simply discard growarrays full of state_items or buf_states; we
 * have to manually release their extmatch pointers first.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clear_syn_state(synstate_T *p)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    garray_T	*gap;

    ಯದಿ (p->sst_stacksize > SST_FIX_STATES)
    {
	gap = &(p->sst_union.sst_ga);
	ಪ್ರತಿ (i = 0; i < gap->ga_len; i++)
	    unref_extmatch(SYN_STATE_P(gap)[i].bs_extmatch);
	ga_clear(gap);
    }
    ಅನ್ಯಥಾ
    {
	ಪ್ರತಿ (i = 0; i < p->sst_stacksize; i++)
	    unref_extmatch(p->sst_union.sst_ರಾಶಿ[i].bs_extmatch);
    }
}

/*
 * Cleanup the current_state stack.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clear_current_state(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    stateitem_T	*sip;

    sip = (stateitem_T *)(current_state.ga_data);
    ಪ್ರತಿ (i = 0; i < current_state.ga_len; i++)
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
    linenr_T	start_lnum,
    synstate_T	*last_valid)
{
    buf_T	*curbuf_save;
    win_T	*curwin_save;
    pos_T	cursor_save;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    linenr_T	lnum;
    linenr_T	end_lnum;
    linenr_T	break_lnum;
    ಪೂರ್ಣಾಙ್ಕ		had_sync_point;
    stateitem_T	*cur_si;
    synpat_T	*spp;
    ಪೂರ್ಣಾಙ್ಕ		found_flags = 0;
    ಪೂರ್ಣಾಙ್ಕ		found_match_idx = 0;
    linenr_T	found_current_lnum = 0;
    ಪೂರ್ಣಾಙ್ಕ		found_current_col = 0;
    lpos_T	found_m_endpos;
    colnr_T	prev_current_col;

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
    ಯದಿ (syn_block->b_syn_sync_minlines > start_lnum)
	start_lnum = 1;
    ಅನ್ಯಥಾ
    {
	ಯದಿ (syn_block->b_syn_sync_minlines == 1)
	    lnum = 1;
	ಅನ್ಯಥಾ ಯದಿ (syn_block->b_syn_sync_minlines < 10)
	    lnum = syn_block->b_syn_sync_minlines * 2;
	ಅನ್ಯಥಾ
	    lnum = syn_block->b_syn_sync_minlines * 3 / 2;
	ಯದಿ (syn_block->b_syn_sync_maxlines != 0
				     && lnum > syn_block->b_syn_sync_maxlines)
	    lnum = syn_block->b_syn_sync_maxlines;
	ಯದಿ (lnum >= start_lnum)
	    start_lnum = 1;
	ಅನ್ಯಥಾ
	    start_lnum -= lnum;
    }
    current_lnum = start_lnum;

    /*
     * 1. Search backwards for the end of a C-style comment.
     */
    ಯದಿ (syn_block->b_syn_sync_flags & SF_CCOMMENT)
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
	ಪ್ರತಿ ( ; start_lnum > 1; --start_lnum)
	{
	    char_u	*l = ml_get(start_lnum - 1);

	    ಯದಿ (*l == NUL || *(l + ml_get_len(start_lnum - 1) - 1) != '\\')
		ಭಙ್ಗ;
	}
	current_lnum = start_lnum;

	// set cursor to start of search
	cursor_save = wp->w_cursor;
	wp->w_cursor.lnum = start_lnum;
	wp->w_cursor.col = 0;

	/*
	 * If the line is inside a comment, need to find the syntax item that
	 * defines the comment.
	 * Restrict the search for the end of a comment to b_syn_sync_maxlines.
	 */
	ಯದಿ (find_start_comment((ಪೂರ್ಣಾಙ್ಕ)syn_block->b_syn_sync_maxlines) != ರಿಕ್ತ)
	{
	    ಪ್ರತಿ (idx = syn_block->b_syn_patterns.ga_len; --idx >= 0; )
		ಯದಿ (SYN_ವಸ್ತುಗಳು(syn_block)[idx].sp_syn.id
						   == syn_block->b_syn_sync_id
			&& SYN_ವಸ್ತುಗಳು(syn_block)[idx].sp_type == SPTYPE_START)
		{
		    validate_current_state();
		    ಯದಿ (ತಳ್ಳು_current_state(idx) == OK)
			update_si_attr(current_state.ga_len - 1);
		    ಭಙ್ಗ;
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
    ಅನ್ಯಥಾ ಯದಿ (syn_block->b_syn_sync_flags & SF_MATCH)
    {
	ಯದಿ (syn_block->b_syn_sync_maxlines != 0
			       && start_lnum > syn_block->b_syn_sync_maxlines)
	    break_lnum = start_lnum - syn_block->b_syn_sync_maxlines;
	ಅನ್ಯಥಾ
	    break_lnum = 0;

	found_m_endpos.lnum = 0;
	found_m_endpos.col = 0;
	end_lnum = start_lnum;
	lnum = start_lnum;
	ಯಾವತ್ (--lnum > break_lnum)
	{
	    // This can take a long time: break when CTRL-C pressed.
	    ಸಾಲು_breakcheck();
	    ಯದಿ (got_int)
	    {
		invalidate_current_state();
		current_lnum = start_lnum;
		ಭಙ್ಗ;
	    }

	    // Check if we have run into a valid saved state stack now.
	    ಯದಿ (last_valid != ರಿಕ್ತ && lnum == last_valid->sst_lnum)
	    {
		load_current_state(last_valid);
		ಭಙ್ಗ;
	    }

	    /*
	     * Check if the previous line has the line-continuation pattern.
	     */
	    ಯದಿ (lnum > 1 && syn_match_linecont(lnum - 1))
		ಅನುವರ್ತ;

	    /*
	     * Start with nothing on the state stack
	     */
	    validate_current_state();

	    ಪ್ರತಿ (current_lnum = lnum; current_lnum < end_lnum; ++current_lnum)
	    {
		syn_start_ಸಾಲು();
		ಪ್ರತಿ (;;)
		{
		    had_sync_point = syn_finish_ಸಾಲು(TRUE);
		    /*
		     * When a sync point has been found, remember where, and
		     * continue to look for another one, further on in the line.
		     */
		    ಯದಿ (had_sync_point && current_state.ga_len)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			ಯದಿ (cur_si->si_m_endpos.lnum > start_lnum)
			{
			    // ignore match that goes to after where started
			    current_lnum = end_lnum;
			    ಭಙ್ಗ;
			}
			ಯದಿ (cur_si->si_idx < 0)
			{
			    // Cannot happen?
			    found_flags = 0;
			    found_match_idx = KEYWORD_IDX;
			}
			ಅನ್ಯಥಾ
			{
			    spp = &(SYN_ವಸ್ತುಗಳು(syn_block)[cur_si->si_idx]);
			    found_flags = spp->sp_flags;
			    found_match_idx = spp->sp_sync_idx;
			}
			found_current_lnum = current_lnum;
			found_current_col = current_col;
			found_m_endpos = cur_si->si_m_endpos;
			/*
			 * Continue after the match (be aware of a zero-length
			 * match).
			 */
			ಯದಿ (found_m_endpos.lnum > current_lnum)
			{
			    current_lnum = found_m_endpos.lnum;
			    current_col = found_m_endpos.col;
			    ಯದಿ (current_lnum >= end_lnum)
				ಭಙ್ಗ;
			}
			ಅನ್ಯಥಾ ಯದಿ (found_m_endpos.col > current_col)
			    current_col = found_m_endpos.col;
			ಅನ್ಯಥಾ
			    ++current_col;

			// syn_current_attr() will have skipped the check for
			// an item that ends here, need to do that now.  Be
			// careful not to go past the NUL.
			prev_current_col = current_col;
			ಯದಿ (syn_getcurline()[current_col] != NUL)
			    ++current_col;
			check_state_ends();
			current_col = prev_current_col;
		    }
		    ಅನ್ಯಥಾ
			ಭಙ್ಗ;
		}
	    }

	    /*
	     * If a sync point was encountered, break here.
	     */
	    ಯದಿ (found_flags)
	    {
		/*
		 * Put the item that was specified by the sync point on the
		 * state stack.  If there was no item specified, make the
		 * state stack empty.
		 */
		clear_current_state();
		ಯದಿ (found_match_idx >= 0
			&& ತಳ್ಳು_current_state(found_match_idx) == OK)
		    update_si_attr(current_state.ga_len - 1);

		/*
		 * When using "grouphere", continue from the sync point
		 * match, until the end of the line.  Parsing starts at
		 * the next line.
		 * For "groupthere" the parsing starts at start_lnum.
		 */
		ಯದಿ (found_flags & HL_SYNC_HERE)
		{
		    current_lnum = found_m_endpos.lnum;
		    current_col = found_m_endpos.col;
		    ಯದಿ (current_state.ga_len)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			cur_si->si_h_startpos.lnum = found_current_lnum;
			cur_si->si_h_startpos.col = found_current_col;
			update_si_end(cur_si, (ಪೂರ್ಣಾಙ್ಕ)current_col, TRUE);
			check_keepend();
		    }
		    (ಶೂನ್ಯ)syn_finish_ಸಾಲು(FALSE);
		    ++current_lnum;
		}
		ಅನ್ಯಥಾ
		    current_lnum = start_lnum;

		ಭಙ್ಗ;
	    }

	    end_lnum = lnum;
	    invalidate_current_state();
	}

	// Ran into start of the file or exceeded maximum number of lines
	ಯದಿ (lnum <= break_lnum)
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
    ಯದಿ (syn_block->b_syn_isk == empty_option)
	ಪ್ರತ್ಯಾಗಮ;

    mch_memmove(chartab, syn_buf->b_chartab, (size_t)32);
    mch_memmove(syn_buf->b_chartab, syn_win->w_s->b_syn_chartab, (size_t)32);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
restore_chartab(char_u *chartab)
{
    ಯದಿ (syn_win->w_s->b_syn_isk != empty_option)
	mch_memmove(syn_buf->b_chartab, chartab, (size_t)32);
}

/*
 * Return TRUE if the line-continuation pattern matches in line "lnum".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
syn_match_linecont(linenr_T lnum)
{
    regmmatch_T regmatch;
    ಪೂರ್ಣಾಙ್ಕ r;
    char_u	buf_chartab[32];  // chartab array for syn iskyeyword

    ಯದಿ (syn_block->b_syn_linecont_prog == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // use syntax iskeyword option
    save_chartab(buf_chartab);
    regmatch.rmm_ic = syn_block->b_syn_linecont_ic;
    regmatch.regprog = syn_block->b_syn_linecont_prog;
    r = syn_regexec(&regmatch, lnum, (colnr_T)0,
	    IF_SYN_TIME(&syn_block->b_syn_linecont_time));
    syn_block->b_syn_linecont_prog = regmatch.regprog;
    restore_chartab(buf_chartab);
    ಪ್ರತ್ಯಾಗಮ r;
}

/*
 * Prepare the current state for the start of a line.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_start_ಸಾಲು(ಶೂನ್ಯ)
{
    current_finished = FALSE;
    current_col = 0;

    /*
     * Need to update the end of a start/skip/end that continues from the
     * previous line and regions that have "keepend".
     */
    ಯದಿ (current_state.ga_len > 0)
    {
	syn_update_ends(TRUE);
	check_state_ends();
    }

    next_match_idx = -1;
    ++current_ಸಾಲು_id;
#ifdef FEAT_CONCEAL
    next_seqnr = 1;
#endif
}

/*
 * Check for items in the stack that need their end updated.
 * When "startofline" is TRUE the last item is always updated.
 * When "startofline" is FALSE the item with "keepend" is forcefully updated.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_update_ends(ಪೂರ್ಣಾಙ್ಕ startofline)
{
    stateitem_T	*cur_si;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		seen_keepend;

    ಯದಿ (startofline)
    {
	// Check for a match carried over from a previous line with a
	// contained region.  The match ends as soon as the region ends.
	ಪ್ರತಿ (i = 0; i < current_state.ga_len; ++i)
	{
	    cur_si = &CUR_STATE(i);
	    ಯದಿ (cur_si->si_idx >= 0
		    && (SYN_ವಸ್ತುಗಳು(syn_block)[cur_si->si_idx]).sp_type
							       == SPTYPE_MATCH
		    && cur_si->si_m_endpos.lnum < current_lnum)
	    {
		cur_si->si_flags |= HL_MATCHCONT;
		cur_si->si_m_endpos.lnum = 0;
		cur_si->si_m_endpos.col = 0;
		cur_si->si_h_endpos = cur_si->si_m_endpos;
		cur_si->si_ends = TRUE;
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
    ಯದಿ (keepend_level >= 0)
	ಪ್ರತಿ ( ; i > keepend_level; --i)
	    ಯದಿ (CUR_STATE(i).si_flags & HL_EXTEND)
		ಭಙ್ಗ;

    seen_keepend = FALSE;
    ಪ್ರತಿ ( ; i < current_state.ga_len; ++i)
    {
	cur_si = &CUR_STATE(i);
	ಯದಿ ((cur_si->si_flags & HL_KEEPEND)
			    || (seen_keepend && !startofline)
			    || (i == current_state.ga_len - 1 && startofline))
	{
	    cur_si->si_h_startpos.col = 0;	// start highl. in col 0
	    cur_si->si_h_startpos.lnum = current_lnum;

	    ಯದಿ (!(cur_si->si_flags & HL_MATCHCONT))
		update_si_end(cur_si, (ಪೂರ್ಣಾಙ್ಕ)current_col, !startofline);

	    ಯದಿ (!startofline && (cur_si->si_flags & HL_KEEPEND))
		seen_keepend = TRUE;
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
syn_ರಾಶಿ_free_block(synblock_T *block)
{
    synstate_T	*p;

    // Syntax definitions may have changed: drop the in_id_list() cache.
    idl_cache_clear(block);

    ಯದಿ (block->b_sst_array == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    FOR_ALL_SYNSTATES(block, p)
	clear_syn_state(p);
    VIM_CLEAR(block->b_sst_array);
    block->b_sst_first = ರಿಕ್ತ;
    block->b_sst_ಹುಡುಕು = ರಿಕ್ತ;
    block->b_sst_len = 0;
}
/*
 * Free b_sst_array[] for buffer "buf".
 * Used when syntax items changed to force resyncing everywhere.
 */
    ಶೂನ್ಯ
syn_ರಾಶಿ_free_all(synblock_T *block)
{
#ifdef FEAT_FOLDING
    win_T	*wp;
#endif

    syn_ರಾಶಿ_free_block(block);

#ifdef FEAT_FOLDING
    // When using "syntax" fold method, must update all folds.
    FOR_ALL_WINDOWS(wp)
    {
	ಯದಿ (wp->w_s == block && foldmethodIsSyntax(wp))
	    foldUpdateAll(wp);
    }
#endif
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
    long	len;
    synstate_T	*to, *from;
    synstate_T	*sstp;

    len = syn_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ / SST_DIST + Rows * 2;
    ಯದಿ (len < SST_MIN_ENTRIES)
	len = SST_MIN_ENTRIES;
    ಅನ್ಯಥಾ ಯದಿ (len > SST_MAX_ENTRIES)
	len = SST_MAX_ENTRIES;
    ಯದಿ (syn_block->b_sst_len > len * 2 || syn_block->b_sst_len < len)
    {
	// Allocate 50% too much, to avoid reallocating too often.
	len = syn_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	len = (len + len / 2) / SST_DIST + Rows * 2;
	ಯದಿ (len < SST_MIN_ENTRIES)
	    len = SST_MIN_ENTRIES;
	ಅನ್ಯಥಾ ಯದಿ (len > SST_MAX_ENTRIES)
	    len = SST_MAX_ENTRIES;

	ಯದಿ (syn_block->b_sst_array != ರಿಕ್ತ)
	{
	    // When shrinking the array, cleanup the existing stack.
	    // Make sure that all valid entries fit in the new array.
	    ಯಾವತ್ (syn_block->b_sst_len - syn_block->b_sst_freecount + 2 > len
		    && syn_ರಾಶಿ_cleanup())
		;
	    ಯದಿ (len < syn_block->b_sst_len - syn_block->b_sst_freecount + 2)
		len = syn_block->b_sst_len - syn_block->b_sst_freecount + 2;
	}

	sstp = ALLOC_CLEAR_MULT(synstate_T, len);
	ಯದಿ (sstp == ರಿಕ್ತ)	// out of memory!
	    ಪ್ರತ್ಯಾಗಮ;

	to = sstp - 1;
	ಯದಿ (syn_block->b_sst_array != ರಿಕ್ತ)
	{
	    // Move the states from the old array to the new one.
	    ಪ್ರತಿ (from = syn_block->b_sst_first; from != ರಿಕ್ತ;
							from = from->sst_next)
	    {
		++to;
		*to = *from;
		to->sst_next = to + 1;
	    }
	}
	ಯದಿ (to != sstp - 1)
	{
	    to->sst_next = ರಿಕ್ತ;
	    syn_block->b_sst_first = sstp;
	    syn_block->b_sst_freecount = len - (ಪೂರ್ಣಾಙ್ಕ)(to - sstp) - 1;
	}
	ಅನ್ಯಥಾ
	{
	    syn_block->b_sst_first = ರಿಕ್ತ;
	    syn_block->b_sst_freecount = len;
	}

	// Create the list of free entries.
	syn_block->b_sst_firstfree = to + 1;
	ಯಾವತ್ (++to < sstp + len)
	    to->sst_next = to + 1;
	(sstp + len - 1)->sst_next = ರಿಕ್ತ;

	vim_free(syn_block->b_sst_array);
	syn_block->b_sst_array = sstp;
	syn_block->b_sst_len = len;
	// The entries were moved to a new array, drop the stale pointer.
	syn_block->b_sst_ಹುಡುಕು = ರಿಕ್ತ;
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

    syn_ರಾಶಿ_apply_changes_block(&buf->b_s, buf);

    FOR_ALL_WINDOWS(wp)
    {
	ಯದಿ ((wp->w_buffer == buf) && (wp->w_s != &buf->b_s))
	    syn_ರಾಶಿ_apply_changes_block(wp->w_s, buf);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
syn_ರಾಶಿ_apply_changes_block(synblock_T *block, buf_T *buf)
{
    synstate_T	*p, *prev, *np;
    linenr_T	n;

    prev = ರಿಕ್ತ;
    ಪ್ರತಿ (p = block->b_sst_first; p != ರಿಕ್ತ; )
    {
	ಯದಿ (p->sst_lnum + block->b_syn_sync_linebreaks > buf->b_mod_ತುದಿ)
	{
	    n = p->sst_lnum + buf->b_mod_xlines;
	    ಯದಿ (n <= buf->b_mod_bot)
	    {
		// this state is inside the changed area, remove it
		np = p->sst_next;
		ಯದಿ (prev == ರಿಕ್ತ)
		    block->b_sst_first = np;
		ಅನ್ಯಥಾ
		    prev->sst_next = np;
		syn_ರಾಶಿ_free_entry(block, p);
		p = np;
		ಅನುವರ್ತ;
	    }
	    // This state is below the changed area.  Remember the line
	    // that needs to be parsed before this entry can be made valid
	    // again.
	    ಯದಿ (p->sst_change_lnum != 0 && p->sst_change_lnum > buf->b_mod_ತುದಿ)
	    {
		ಯದಿ (p->sst_change_lnum + buf->b_mod_xlines > buf->b_mod_ತುದಿ)
		    p->sst_change_lnum += buf->b_mod_xlines;
		ಅನ್ಯಥಾ
		    p->sst_change_lnum = buf->b_mod_ತುದಿ;
	    }
	    ಯದಿ (p->sst_change_lnum == 0
		    || p->sst_change_lnum < buf->b_mod_bot)
		p->sst_change_lnum = buf->b_mod_bot;

	    p->sst_lnum = n;
	}
	prev = p;
	p = p->sst_next;
    }
}

/*
 * Reduce the number of entries in the state stack for syn_buf.
 * Returns TRUE if at least one entry was freed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
syn_ರಾಶಿ_cleanup(ಶೂನ್ಯ)
{
    synstate_T	*p, *prev;
    disptick_T	tick;
    ಪೂರ್ಣಾಙ್ಕ		above;
    ಪೂರ್ಣಾಙ್ಕ		dist;
    ಪೂರ್ಣಾಙ್ಕ		retval = FALSE;

    ಯದಿ (syn_block->b_sst_first == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ retval;

    // Compute normal distance between non-displayed entries.
    ಯದಿ (syn_block->b_sst_len <= Rows)
	dist = 999999;
    ಅನ್ಯಥಾ
	dist = syn_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ / (syn_block->b_sst_len - Rows) + 1;

    /*
     * Go through the list to find the "tick" for the oldest entry that can
     * be removed.  Set "above" when the "tick" for the oldest entry is above
     * "b_sst_lasttick" (the display tick wraps around).
     */
    tick = syn_block->b_sst_lasttick;
    above = FALSE;
    prev = syn_block->b_sst_first;
    ಪ್ರತಿ (p = prev->sst_next; p != ರಿಕ್ತ; prev = p, p = p->sst_next)
    {
	ಯದಿ (prev->sst_lnum + dist > p->sst_lnum)
	{
	    ಯದಿ (p->sst_tick > syn_block->b_sst_lasttick)
	    {
		ಯದಿ (!above || p->sst_tick < tick)
		    tick = p->sst_tick;
		above = TRUE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (!above && p->sst_tick < tick)
		tick = p->sst_tick;
	}
    }

    /*
     * Go through the list to make the entries for the oldest tick at an
     * interval of several lines.
     */
    prev = syn_block->b_sst_first;
    ಪ್ರತಿ (p = prev->sst_next; p != ರಿಕ್ತ; prev = p, p = p->sst_next)
    {
	ಯದಿ (p->sst_tick == tick && prev->sst_lnum + dist > p->sst_lnum)
	{
	    // Move this entry from used list to free list
	    prev->sst_next = p->sst_next;
	    syn_ರಾಶಿ_free_entry(syn_block, p);
	    p = prev;
	    retval = TRUE;
	}
    }
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Free the allocated memory for a syn_state item.
 * Move the entry into the free list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_ರಾಶಿ_free_entry(synblock_T *block, synstate_T *p)
{
    ಯದಿ (block->b_sst_ಹುಡುಕು == p)
	block->b_sst_ಹುಡುಕು = ರಿಕ್ತ;
    clear_syn_state(p);
    p->sst_next = block->b_sst_firstfree;
    block->b_sst_firstfree = p;
    ++block->b_sst_freecount;
}

/*
 * Find an entry in the list of state stacks at or before "lnum".
 * Returns NULL when there is no entry or the first entry is after "lnum".
 */
    ಸ್ಥಿರ synstate_T *
syn_ರಾಶಿ_find_entry(linenr_T lnum)
{
    synstate_T	*p, *prev;

    prev = ರಿಕ್ತ;
    p = syn_block->b_sst_first;

    // The list is sorted by line number and lookups while parsing advance
    // monotonically, so resume from the last returned entry instead of
    // rescanning from the start whenever it is at or before "lnum".
    ಯದಿ (syn_block->b_sst_ಹುಡುಕು != ರಿಕ್ತ
				 && syn_block->b_sst_ಹುಡುಕು->sst_lnum <= lnum)
	p = syn_block->b_sst_ಹುಡುಕು;

    ಪ್ರತಿ ( ; p != ರಿಕ್ತ; prev = p, p = p->sst_next)
    {
	ಯದಿ (p->sst_lnum == lnum)
	{
	    syn_block->b_sst_ಹುಡುಕು = p;
	    ಪ್ರತ್ಯಾಗಮ p;
	}
	ಯದಿ (p->sst_lnum > lnum)
	    ಭಙ್ಗ;
    }
    ಯದಿ (prev != ರಿಕ್ತ)
	syn_block->b_sst_ಹುಡುಕು = prev;
    ಪ್ರತ್ಯಾಗಮ prev;
}

/*
 * Try saving the current state in b_sst_array[].
 * The current state must be valid for the start of the current_lnum line!
 */
    ಸ್ಥಿರ synstate_T *
store_current_state(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    synstate_T	*p;
    bufstate_T	*bp;
    stateitem_T	*cur_si;
    synstate_T	*sp = syn_ರಾಶಿ_find_entry(current_lnum);

    /*
     * If the current state contains a start or end pattern that continues
     * from the previous line, we can't use it.  Don't store it then.
     */
    ಪ್ರತಿ (i = current_state.ga_len - 1; i >= 0; --i)
    {
	cur_si = &CUR_STATE(i);
	ಯದಿ (cur_si->si_h_startpos.lnum >= current_lnum
		|| cur_si->si_m_endpos.lnum >= current_lnum
		|| cur_si->si_h_endpos.lnum >= current_lnum
		|| (cur_si->si_end_idx
		    && cur_si->si_eoe_pos.lnum >= current_lnum))
	    ಭಙ್ಗ;
    }
    ಯದಿ (i >= 0)
    {
	ಯದಿ (sp != ರಿಕ್ತ)
	{
	    // find "sp" in the list and remove it
	    ಯದಿ (syn_block->b_sst_first == sp)
		// it's the first entry
		syn_block->b_sst_first = sp->sst_next;
	    ಅನ್ಯಥಾ
	    {
		// find the entry just before this one to adjust sst_next
		FOR_ALL_SYNSTATES(syn_block, p)
		    ಯದಿ (p->sst_next == sp)
			ಭಙ್ಗ;
		ಯದಿ (p != ರಿಕ್ತ)	// just in case
		    p->sst_next = sp->sst_next;
	    }
	    syn_ರಾಶಿ_free_entry(syn_block, sp);
	    sp = ರಿಕ್ತ;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (sp == ರಿಕ್ತ || sp->sst_lnum != current_lnum)
    {
	/*
	 * Add a new entry
	 */
	// If no free items, cleanup the array first.
	ಯದಿ (syn_block->b_sst_freecount == 0)
	{
	    (ಶೂನ್ಯ)syn_ರಾಶಿ_cleanup();
	    // "sp" may have been moved to the freelist now
	    sp = syn_ರಾಶಿ_find_entry(current_lnum);
	}
	// Still no free items?  Must be a strange problem...
	ಯದಿ (syn_block->b_sst_freecount == 0)
	    sp = ರಿಕ್ತ;
	ಅನ್ಯಥಾ
	{
	    // Take the first item from the free list and put it in the used
	    // list, after *sp
	    p = syn_block->b_sst_firstfree;
	    syn_block->b_sst_firstfree = p->sst_next;
	    --syn_block->b_sst_freecount;
	    ಯದಿ (sp == ರಿಕ್ತ)
	    {
		// Insert in front of the list
		p->sst_next = syn_block->b_sst_first;
		syn_block->b_sst_first = p;
	    }
	    ಅನ್ಯಥಾ
	    {
		// insert in list after *sp
		p->sst_next = sp->sst_next;
		sp->sst_next = p;
	    }
	    sp = p;
	    sp->sst_stacksize = 0;
	    sp->sst_lnum = current_lnum;
	    // Resume the next forward lookup from the entry just stored.
	    syn_block->b_sst_ಹುಡುಕು = sp;
	}
    }
    ಯದಿ (sp != ರಿಕ್ತ)
    {
	// When overwriting an existing state stack, clear it first
	clear_syn_state(sp);
	sp->sst_stacksize = current_state.ga_len;
	ಯದಿ (current_state.ga_len > SST_FIX_STATES)
	{
	    // Need to clear it, might be something remaining from when the
	    // length was less than SST_FIX_STATES.
	    ga_init2(&sp->sst_union.sst_ga, ಪರಿಮಾಣ(bufstate_T), 1);
	    ಯದಿ (ga_grow(&sp->sst_union.sst_ga, current_state.ga_len) == FAIL)
		sp->sst_stacksize = 0;
	    ಅನ್ಯಥಾ
		sp->sst_union.sst_ga.ga_len = current_state.ga_len;
	    bp = SYN_STATE_P(&(sp->sst_union.sst_ga));
	}
	ಅನ್ಯಥಾ
	    bp = sp->sst_union.sst_ರಾಶಿ;
	ಪ್ರತಿ (i = 0; i < sp->sst_stacksize; ++i)
	{
	    bp[i].bs_idx = CUR_STATE(i).si_idx;
	    bp[i].bs_flags = CUR_STATE(i).si_flags;
#ifdef FEAT_CONCEAL
	    bp[i].bs_seqnr = CUR_STATE(i).si_seqnr;
	    bp[i].bs_cchar = CUR_STATE(i).si_cchar;
#endif
	    bp[i].bs_extmatch = ref_extmatch(CUR_STATE(i).si_extmatch);
	}
	sp->sst_next_flags = current_next_flags;
	sp->sst_next_list = current_next_list;
	sp->sst_tick = display_tick;
	sp->sst_change_lnum = 0;
    }
    current_state_stored = TRUE;
    ಪ್ರತ್ಯಾಗಮ sp;
}

/*
 * Copy a state stack from "from" in b_sst_array[] to current_state;
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
load_current_state(synstate_T *from)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    bufstate_T	*bp;

    clear_current_state();
    validate_current_state();
    keepend_level = -1;
    ಯದಿ (from->sst_stacksize
	    && ga_grow(&current_state, from->sst_stacksize) == OK)
    {
	ಯದಿ (from->sst_stacksize > SST_FIX_STATES)
	    bp = SYN_STATE_P(&(from->sst_union.sst_ga));
	ಅನ್ಯಥಾ
	    bp = from->sst_union.sst_ರಾಶಿ;
	ಪ್ರತಿ (i = 0; i < from->sst_stacksize; ++i)
	{
	    CUR_STATE(i).si_idx = bp[i].bs_idx;
	    CUR_STATE(i).si_flags = bp[i].bs_flags;
#ifdef FEAT_CONCEAL
	    CUR_STATE(i).si_seqnr = bp[i].bs_seqnr;
	    CUR_STATE(i).si_cchar = bp[i].bs_cchar;
#endif
	    CUR_STATE(i).si_extmatch = ref_extmatch(bp[i].bs_extmatch);
	    ಯದಿ (keepend_level < 0 && (CUR_STATE(i).si_flags & HL_KEEPEND))
		keepend_level = i;
	    CUR_STATE(i).si_ends = FALSE;
	    CUR_STATE(i).si_m_lnum = 0;
	    ಯದಿ (CUR_STATE(i).si_idx >= 0)
		CUR_STATE(i).si_next_list =
		     (SYN_ವಸ್ತುಗಳು(syn_block)[CUR_STATE(i).si_idx]).sp_next_list;
	    ಅನ್ಯಥಾ
		CUR_STATE(i).si_next_list = ರಿಕ್ತ;
	    update_si_attr(i);
	}
	current_state.ga_len = from->sst_stacksize;
    }
    current_next_list = from->sst_next_list;
    current_next_flags = from->sst_next_flags;
    current_lnum = from->sst_lnum;
}

/*
 * Compare saved state stack "*sp" with the current state.
 * Return TRUE when they are equal.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
syn_ರಾಶಿ_equal(synstate_T *sp)
{
    ಪೂರ್ಣಾಙ್ಕ		i, j;
    bufstate_T	*bp;
    reg_extmatch_T	*six, *bsx;

    // First a quick check if the stacks have the same size end nextlist.
    ಯದಿ (sp->sst_stacksize != current_state.ga_len
	    || sp->sst_next_list != current_next_list)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // Need to compare all states on both stacks.
    ಯದಿ (sp->sst_stacksize > SST_FIX_STATES)
	bp = SYN_STATE_P(&(sp->sst_union.sst_ga));
    ಅನ್ಯಥಾ
	bp = sp->sst_union.sst_ರಾಶಿ;

    ಪ್ರತಿ (i = current_state.ga_len; --i >= 0; )
    {
	// If the item has another index the state is different.
	ಯದಿ (bp[i].bs_idx != CUR_STATE(i).si_idx)
	    ಭಙ್ಗ;
	ಯದಿ (bp[i].bs_extmatch == CUR_STATE(i).si_extmatch)
	    ಅನುವರ್ತ;
	// When the extmatch pointers are different, the strings in them can
	// still be the same.  Check if the extmatch references are equal.
	bsx = bp[i].bs_extmatch;
	six = CUR_STATE(i).si_extmatch;
	// If one of the extmatch pointers is NULL the states are different.
	ಯದಿ (bsx == ರಿಕ್ತ || six == ರಿಕ್ತ)
	    ಭಙ್ಗ;
	ಪ್ರತಿ (j = 0; j < NSUBEXP; ++j)
	{
	    // Check each referenced match string. They must all be equal.
	    ಯದಿ (bsx->matches[j] != six->matches[j])
	    {
		// If the pointer is different it can still be the same text.
		// Compare the strings, ignore case when the start item has the
		// sp_ic flag set.
		ಯದಿ (bsx->matches[j] == ರಿಕ್ತ || six->matches[j] == ರಿಕ್ತ)
		    ಭಙ್ಗ;
		ಯದಿ ((SYN_ವಸ್ತುಗಳು(syn_block)[CUR_STATE(i).si_idx]).sp_ic
			? MB_STRICMP(bsx->matches[j], six->matches[j]) != 0
			: STRCMP(bsx->matches[j], six->matches[j]) != 0)
		    ಭಙ್ಗ;
	    }
	}
	ಯದಿ (j != NSUBEXP)
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ i < 0 ? TRUE : FALSE;
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
syntax_end_parsing(win_T *wp, linenr_T lnum)
{
    synstate_T	*sp;

    ಯದಿ (syn_block != wp->w_s)
	ಪ್ರತ್ಯಾಗಮ;  // not the right window
    sp = syn_ರಾಶಿ_find_entry(lnum);
    ಯದಿ (sp != ರಿಕ್ತ && sp->sst_lnum < lnum)
	sp = sp->sst_next;

    ಯದಿ (sp != ರಿಕ್ತ && sp->sst_change_lnum != 0)
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
    current_next_list = ರಿಕ್ತ;
    keepend_level = -1;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
validate_current_state(ಶೂನ್ಯ)
{
    current_state.ga_itemsize = ಪರಿಮಾಣ(stateitem_T);
    current_state.ga_growsize = 3;
}

/*
 * Return TRUE if the syntax at start of lnum changed since last time.
 * This will only be called just after get_syntax_attr() for the previous
 * line, to check if the next line needs to be redrawn too.
 */
    ಪೂರ್ಣಾಙ್ಕ
syntax_check_changed(linenr_T lnum)
{
    ಪೂರ್ಣಾಙ್ಕ		retval = TRUE;
    synstate_T	*sp;

    /*
     * Check the state stack when:
     * - lnum is just below the previously syntaxed line.
     * - lnum is not before the lines with saved states.
     * - lnum is not past the lines with saved states.
     * - lnum is at or before the last changed line.
     */
    ಯದಿ (VALID_STATE(&current_state) && lnum == current_lnum + 1)
    {
	sp = syn_ರಾಶಿ_find_entry(lnum);
	ಯದಿ (sp != ರಿಕ್ತ && sp->sst_lnum == lnum)
	{
	    /*
	     * finish the previous line (needed when not all of the line was
	     * drawn)
	     */
	    (ಶೂನ್ಯ)syn_finish_ಸಾಲು(FALSE);

	    /*
	     * Compare the current state with the previously saved state of
	     * the line.
	     */
	    ಯದಿ (syn_ರಾಶಿ_equal(sp))
		retval = FALSE;

	    /*
	     * Store the current state in b_sst_array[] for later use.
	     */
	    ++current_lnum;
	    (ಶೂನ್ಯ)store_current_state();
	}
    }

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Finish the current line.
 * This doesn't return any attributes, it only gets the state at the end of
 * the line.  It can start anywhere in the line, as long as the current state
 * is valid.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
syn_finish_ಸಾಲು(
    ಪೂರ್ಣಾಙ್ಕ	    syncing)		// called for syncing
{
    stateitem_T	*cur_si;
    colnr_T	prev_current_col;

    ಯಾವತ್ (!current_finished)
    {
	(ಶೂನ್ಯ)syn_current_attr(syncing, FALSE, ರಿಕ್ತ, FALSE);
	/*
	 * When syncing, and found some item, need to check the item.
	 */
	ಯದಿ (syncing && current_state.ga_len)
	{
	    /*
	     * Check for match with sync item.
	     */
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	    ಯದಿ (cur_si->si_idx >= 0
		    && (SYN_ವಸ್ತುಗಳು(syn_block)[cur_si->si_idx].sp_flags
					  & (HL_SYNC_HERE|HL_SYNC_THERE)))
		ಪ್ರತ್ಯಾಗಮ TRUE;

	    // syn_current_attr() will have skipped the check for an item
	    // that ends here, need to do that now.  Be careful not to go
	    // past the NUL.
	    prev_current_col = current_col;
	    ಯದಿ (syn_getcurline()[current_col] != NUL)
		++current_col;
	    check_state_ends();
	    current_col = prev_current_col;
	}
	++current_col;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
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
    ಪೂರ್ಣಾಙ್ಕ
get_syntax_attr(
    colnr_T	col,
    ಪೂರ್ಣಾಙ್ಕ		*can_spell,
    ಪೂರ್ಣಾಙ್ಕ		keep_state)	// keep state of char at "col"
{
    ಪೂರ್ಣಾಙ್ಕ	    attr = 0;

    ಯದಿ (can_spell != ರಿಕ್ತ)
	// Default: Only do spelling when there is no @Spell cluster or when
	// ":syn spell toplevel" was used.
	*can_spell = syn_block->b_syn_spell == SYNSPL_DEFAULT
		    ? (syn_block->b_spell_cluster_id == 0)
		    : (syn_block->b_syn_spell == SYNSPL_ತುದಿ);

    // check for out of memory situation
    ಯದಿ (syn_block->b_sst_array == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    // After 'synmaxcol' the attribute is always zero.
    ಯದಿ (syn_buf->b_p_smc > 0 && col >= (colnr_T)syn_buf->b_p_smc)
    {
	clear_current_state();
#ifdef FEAT_EVAL
	current_id = 0;
	current_trans_id = 0;
#endif
#ifdef FEAT_CONCEAL
	current_flags = 0;
	current_seqnr = 0;
#endif
	ಪ್ರತ್ಯಾಗಮ 0;
    }

    // Make sure current_state is valid
    ಯದಿ (INVALID_STATE(&current_state))
	validate_current_state();

    /*
     * Skip from the current column to "col", get the attributes for "col".
     */
    ಯಾವತ್ (current_col <= col)
    {
	attr = syn_current_attr(FALSE, TRUE, can_spell,
				     current_col == col ? keep_state : FALSE);
	++current_col;
    }

    ಪ್ರತ್ಯಾಗಮ attr;
}

/*
 * Get syntax attributes for current_lnum, current_col.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
syn_current_attr(
    ಪೂರ್ಣಾಙ್ಕ		syncing,		// When 1: called for syncing
    ಪೂರ್ಣಾಙ್ಕ		displaying,		// result will be displayed
    ಪೂರ್ಣಾಙ್ಕ		*can_spell,		// return: do spell checking
    ಪೂರ್ಣಾಙ್ಕ		keep_state)		// keep syntax stack afterwards
{
    ಪೂರ್ಣಾಙ್ಕ		syn_id;
    lpos_T	endpos;		// was: char_u *endp;
    lpos_T	hl_startpos;	// was: int hl_startcol;
    lpos_T	hl_endpos;
    lpos_T	eos_pos;	// end-of-start match (start region)
    lpos_T	eoe_pos;	// end-of-end pattern
    ಪೂರ್ಣಾಙ್ಕ		end_idx;	// group ID for end pattern
    ಪೂರ್ಣಾಙ್ಕ		idx;
    synpat_T	*spp;
    stateitem_T	*cur_si, *sip = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		startcol;
    ಪೂರ್ಣಾಙ್ಕ		endcol;
    long	flags;
    ಪೂರ್ಣಾಙ್ಕ		cchar;
    short	*next_list;
    ಪೂರ್ಣಾಙ್ಕ		found_match;		    // found usable match
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	try_next_column = FALSE;    // must try in next col
    ಪೂರ್ಣಾಙ್ಕ		do_keywords;
    regmmatch_T	regmatch;
    lpos_T	pos;
    ಪೂರ್ಣಾಙ್ಕ		lc_col;
    reg_extmatch_T *cur_extmatch = ರಿಕ್ತ;
    char_u	buf_chartab[32];  // chartab array for syn iskyeyword
    char_u	*ಸಾಲು;		// current line.  NOTE: becomes invalid after
				// looking for a pattern match!

    // variables for zero-width matches that have a "nextgroup" argument
    ಪೂರ್ಣಾಙ್ಕ		keep_next_list;
    ಪೂರ್ಣಾಙ್ಕ		zero_width_next_list = FALSE;
    garray_T	zero_width_next_ga;

    /*
     * No character, no attributes!  Past end of line?
     * Do try matching with an empty line (could be the start of a region).
     */
    ಸಾಲು = syn_getcurline();
    ಯದಿ (ಸಾಲು[current_col] == NUL && current_col != 0)
    {
	/*
	 * If we found a match after the last column, use it.
	 */
	ಯದಿ (next_match_idx >= 0 && next_match_col >= (ಪೂರ್ಣಾಙ್ಕ)current_col
						  && next_match_col != MAXCOL)
	    (ಶೂನ್ಯ)ತಳ್ಳು_next_match(ರಿಕ್ತ);

	current_finished = TRUE;
	current_state_stored = FALSE;
	ಪ್ರತ್ಯಾಗಮ 0;
    }

    // if the current or next character is NUL, we will finish the line now
    ಯದಿ (ಸಾಲು[current_col] == NUL || ಸಾಲು[current_col + 1] == NUL)
    {
	current_finished = TRUE;
	current_state_stored = FALSE;
    }

    /*
     * When in the previous column there was a match but it could not be used
     * (empty match or already matched in this column) need to try again in
     * the next column.
     */
    ಯದಿ (try_next_column)
    {
	next_match_idx = -1;
	try_next_column = FALSE;
    }

    // Only check for keywords when not syncing and there are some.
    do_keywords = !syncing
		    && (syn_block->b_keywtab.ht_used > 0
			    || syn_block->b_keywtab_ic.ht_used > 0);

    // Init the list of zero-width matches with a nextlist.  This is used to
    // avoid matching the same item in the same position twice.
    ga_init2(&zero_width_next_ga, ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ), 10);

    // use syntax iskeyword option
    save_chartab(buf_chartab);

    /*
     * Repeat matching keywords and patterns, to find contained items at the
     * same column.  This stops when there are no extra matches at the current
     * column.
     */
    ಕುರು
    {
	found_match = FALSE;
	keep_next_list = FALSE;
	syn_id = 0;


	/*
	 * 1. Check for a current state.
	 *    Only when there is no current state, or if the current state may
	 *    contain other things, we need to check for keywords and patterns.
	 *    Always need to check for contained items if some item has the
	 *    "containedin" argument (takes extra time!).
	 */
	ಯದಿ (current_state.ga_len)
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	ಅನ್ಯಥಾ
	    cur_si = ರಿಕ್ತ;

	ಯದಿ (syn_block->b_syn_containedin || cur_si == ರಿಕ್ತ
					      || cur_si->si_cont_list != ರಿಕ್ತ)
	{
	    /*
	     * 2. Check for keywords, if on a keyword char after a non-keyword
	     *	  char.  Don't do this when syncing.
	     */
	    ಯದಿ (do_keywords)
	    {
	      ಸಾಲು = syn_getcurline();
	      ಯದಿ (vim_iswordp_buf(ಸಾಲು + current_col, syn_buf)
		      && (current_col == 0
			  || !vim_iswordp_buf(ಸಾಲು + current_col - 1
			      - (has_mbyte
				  ? (*mb_head_off)(ಸಾಲು, ಸಾಲು + current_col - 1)
				  : 0) , syn_buf)))
	      {
		syn_id = check_keyword_id(ಸಾಲು, (ಪೂರ್ಣಾಙ್ಕ)current_col,
					 &endcol, &flags, &next_list, cur_si,
					 &cchar);
		ಯದಿ (syn_id != 0)
		{
		    ಯದಿ (ತಳ್ಳು_current_state(KEYWORD_IDX) == OK)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			cur_si->si_m_startcol = current_col;
			cur_si->si_h_startpos.lnum = current_lnum;
			cur_si->si_h_startpos.col = 0;	// starts right away
			cur_si->si_m_endpos.lnum = current_lnum;
			cur_si->si_m_endpos.col = endcol;
			cur_si->si_h_endpos.lnum = current_lnum;
			cur_si->si_h_endpos.col = endcol;
			cur_si->si_ends = TRUE;
			cur_si->si_end_idx = 0;
			cur_si->si_flags = flags;
#ifdef FEAT_CONCEAL
			cur_si->si_seqnr = next_seqnr++;
			cur_si->si_cchar = cchar;
			ಯದಿ (current_state.ga_len > 1)
			    cur_si->si_flags |=
				  CUR_STATE(current_state.ga_len - 2).si_flags
								 & HL_CONCEAL;
#endif
			cur_si->si_id = syn_id;
			cur_si->si_trans_id = syn_id;
			ಯದಿ (flags & HL_TRANSP)
			{
			    ಯದಿ (current_state.ga_len < 2)
			    {
				cur_si->si_attr = 0;
				cur_si->si_trans_id = 0;
			    }
			    ಅನ್ಯಥಾ
			    {
				cur_si->si_attr = CUR_STATE(
					current_state.ga_len - 2).si_attr;
				cur_si->si_trans_id = CUR_STATE(
					current_state.ga_len - 2).si_trans_id;
			    }
			}
			ಅನ್ಯಥಾ
			    cur_si->si_attr = syn_id2attr(syn_id);
			cur_si->si_cont_list = ರಿಕ್ತ;
			cur_si->si_next_list = next_list;
			check_keepend();
		    }
		    ಅನ್ಯಥಾ
			vim_free(next_list);
		}
	      }
	    }

	    /*
	     * 3. Check for patterns (only if no keyword found).
	     */
	    ಯದಿ (syn_id == 0 && syn_block->b_syn_patterns.ga_len)
	    {
		/*
		 * If we didn't check for a match yet, or we are past it, check
		 * for any match with a pattern.
		 */
		ಯದಿ (next_match_idx < 0 || next_match_col < (ಪೂರ್ಣಾಙ್ಕ)current_col)
		{
		    /*
		     * Check all relevant patterns for a match at this
		     * position.  This is complicated, because matching with a
		     * pattern takes quite a bit of time, thus we want to
		     * avoid doing it when it's not needed.
		     */
		    next_match_idx = 0;		// no match in this line yet
		    next_match_col = MAXCOL;
		    ಪ್ರತಿ (idx = syn_block->b_syn_patterns.ga_len; --idx >= 0; )
		    {
			spp = &(SYN_ವಸ್ತುಗಳು(syn_block)[idx]);
			ಯದಿ (	   spp->sp_syncing == syncing
				&& (displaying || !(spp->sp_flags & HL_DISPLAY))
				&& (spp->sp_type == SPTYPE_MATCH
				    || spp->sp_type == SPTYPE_START)
				&& (current_next_list != ರಿಕ್ತ
				    ? in_id_list(ರಿಕ್ತ, current_next_list,
							      &spp->sp_syn, 0)
				    : (cur_si == ರಿಕ್ತ
					? !(spp->sp_flags & HL_CONTAINED)
					: in_id_list(cur_si,
					    cur_si->si_cont_list, &spp->sp_syn,
					    spp->sp_flags))))
			{
			    ಪೂರ್ಣಾಙ್ಕ r;

			    // If we already tried matching in this line, and
			    // there isn't a match before next_match_col, skip
			    // this item.
			    ಯದಿ (spp->sp_ಸಾಲು_id == current_ಸಾಲು_id
				    && spp->sp_startcol >= next_match_col)
				ಅನುವರ್ತ;
			    spp->sp_ಸಾಲು_id = current_ಸಾಲು_id;

			    lc_col = current_col - spp->sp_offsets[SPO_LC_OFF];
			    ಯದಿ (lc_col < 0)
				lc_col = 0;

			    regmatch.rmm_ic = spp->sp_ic;
			    regmatch.regprog = spp->sp_prog;
			    r = syn_regexec(&regmatch,
					     current_lnum,
					     (colnr_T)lc_col,
					     IF_SYN_TIME(&spp->sp_time));
			    spp->sp_prog = regmatch.regprog;
			    ಯದಿ (!r)
			    {
				// no match in this line, try another one
				spp->sp_startcol = MAXCOL;
				ಅನುವರ್ತ;
			    }

			    /*
			     * Compute the first column of the match.
			     */
			    syn_add_start_off(&pos, &regmatch,
							 spp, SPO_MS_OFF, -1);
			    ಯದಿ (pos.lnum > current_lnum)
			    {
				// must have used end of match in a next line,
				// we can't handle that
				spp->sp_startcol = MAXCOL;
				ಅನುವರ್ತ;
			    }
			    startcol = pos.col;

			    // remember the next column where this pattern
			    // matches in the current line
			    spp->sp_startcol = startcol;

			    /*
			     * If a previously found match starts at a lower
			     * column number, don't use this one.
			     */
			    ಯದಿ (startcol >= next_match_col)
				ಅನುವರ್ತ;

			    /*
			     * If we matched this pattern at this position
			     * before, skip it.  Must retry in the next
			     * column, because it may match from there.
			     */
			    ಯದಿ (did_match_already(idx, &zero_width_next_ga))
			    {
				try_next_column = TRUE;
				ಅನುವರ್ತ;
			    }

			    endpos.lnum = regmatch.endpos[0].lnum;
			    endpos.col = regmatch.endpos[0].col;

			    // Compute the highlight start.
			    syn_add_start_off(&hl_startpos, &regmatch,
							 spp, SPO_HS_OFF, -1);

			    // Compute the region start.
			    // Default is to use the end of the match.
			    syn_add_end_off(&eos_pos, &regmatch,
							 spp, SPO_RS_OFF, 0);

			    /*
			     * Grab the external submatches before they get
			     * overwritten.  Reference count doesn't change.
			     */
			    unref_extmatch(cur_extmatch);
			    cur_extmatch = re_extmatch_out;
			    re_extmatch_out = ರಿಕ್ತ;

			    flags = 0;
			    eoe_pos.lnum = 0;	// avoid warning
			    eoe_pos.col = 0;
			    end_idx = 0;
			    hl_endpos.lnum = 0;

			    /*
			     * For a "oneline" the end must be found in the
			     * same line too.  Search for it after the end of
			     * the match with the start pattern.  Set the
			     * resulting end positions at the same time.
			     */
			    ಯದಿ (spp->sp_type == SPTYPE_START
					      && (spp->sp_flags & HL_ONELINE))
			    {
				lpos_T	startpos;

				startpos = endpos;
				find_endpos(idx, &startpos, &endpos, &hl_endpos,
				    &flags, &eoe_pos, &end_idx, cur_extmatch);
				ಯದಿ (endpos.lnum == 0)
				    ಅನುವರ್ತ;	    // not found
			    }

			    /*
			     * For a "match" the size must be > 0 after the
			     * end offset needs has been added.  Except when
			     * syncing.
			     */
			    ಅನ್ಯಥಾ ಯದಿ (spp->sp_type == SPTYPE_MATCH)
			    {
				syn_add_end_off(&hl_endpos, &regmatch, spp,
							       SPO_HE_OFF, 0);
				syn_add_end_off(&endpos, &regmatch, spp,
							       SPO_ME_OFF, 0);
				ಯದಿ (endpos.lnum == current_lnum
				      && (ಪೂರ್ಣಾಙ್ಕ)endpos.col + syncing < startcol)
				{
				    /*
				     * If an empty string is matched, may need
				     * to try matching again at next column.
				     */
				    ಯದಿ (regmatch.startpos[0].col
						    == regmatch.endpos[0].col)
					try_next_column = TRUE;
				    ಅನುವರ್ತ;
				}
			    }

			    /*
			     * keep the best match so far in next_match_*
			     */
			    // Highlighting must start after startpos and end
			    // before endpos.
			    ಯದಿ (hl_startpos.lnum == current_lnum
					   && (ಪೂರ್ಣಾಙ್ಕ)hl_startpos.col < startcol)
				hl_startpos.col = startcol;
			    limit_pos_zero(&hl_endpos, &endpos);

			    next_match_idx = idx;
			    next_match_col = startcol;
			    next_match_m_endpos = endpos;
			    next_match_h_endpos = hl_endpos;
			    next_match_h_startpos = hl_startpos;
			    next_match_flags = flags;
			    next_match_eos_pos = eos_pos;
			    next_match_eoe_pos = eoe_pos;
			    next_match_end_idx = end_idx;
			    unref_extmatch(next_match_extmatch);
			    next_match_extmatch = cur_extmatch;
			    cur_extmatch = ರಿಕ್ತ;
			}
		    }
		}

		/*
		 * If we found a match at the current column, use it.
		 */
		ಯದಿ (next_match_idx >= 0 && next_match_col == (ಪೂರ್ಣಾಙ್ಕ)current_col)
		{
		    synpat_T	*lspp;

		    // When a zero-width item matched which has a nextgroup,
		    // don't push the item but set nextgroup.
		    lspp = &(SYN_ವಸ್ತುಗಳು(syn_block)[next_match_idx]);
		    ಯದಿ (next_match_m_endpos.lnum == current_lnum
			    && next_match_m_endpos.col == current_col
			    && lspp->sp_next_list != ರಿಕ್ತ)
		    {
			current_next_list = lspp->sp_next_list;
			current_next_flags = lspp->sp_flags;
			keep_next_list = TRUE;
			zero_width_next_list = TRUE;

			// Add the index to a list, so that we can check
			// later that we don't match it again (and cause an
			// endless loop).
			ಯದಿ (ga_grow(&zero_width_next_ga, 1) == OK)
			{
			    ((ಪೂರ್ಣಾಙ್ಕ *)(zero_width_next_ga.ga_data))
				[zero_width_next_ga.ga_len++] = next_match_idx;
			}
			next_match_idx = -1;
		    }
		    ಅನ್ಯಥಾ
			cur_si = ತಳ್ಳು_next_match(cur_si);
		    found_match = TRUE;
		}
	    }
	}

	/*
	 * Handle searching for nextgroup match.
	 */
	ಯದಿ (current_next_list != ರಿಕ್ತ && !keep_next_list)
	{
	    /*
	     * If a nextgroup was not found, continue looking for one if:
	     * - this is an empty line and the "skipempty" option was given
	     * - we are on white space and the "skipwhite" option was given
	     */
	    ಯದಿ (!found_match)
	    {
		ಸಾಲು = syn_getcurline();
		ಯದಿ (((current_next_flags & HL_SKIPWHITE)
			    && VIM_ISWHITE(ಸಾಲು[current_col]))
			|| ((current_next_flags & HL_SKIPEMPTY)
			    && *ಸಾಲು == NUL))
		    ಭಙ್ಗ;
	    }

	    /*
	     * If a nextgroup was found: Use it, and continue looking for
	     * contained matches.
	     * If a nextgroup was not found: Continue looking for a normal
	     * match.
	     * When did set current_next_list for a zero-width item and no
	     * match was found don't loop (would get stuck).
	     */
	    current_next_list = ರಿಕ್ತ;
	    next_match_idx = -1;
	    ಯದಿ (!zero_width_next_list)
		found_match = TRUE;
	}

    } ಯಾವತ್ (found_match);

    restore_chartab(buf_chartab);

    /*
     * Use attributes from the current state, if within its highlighting.
     * If not, use attributes from the current-but-one state, etc.
     */
    current_attr = 0;
#ifdef FEAT_EVAL
    current_id = 0;
    current_trans_id = 0;
#endif
#ifdef FEAT_CONCEAL
    current_flags = 0;
    current_seqnr = 0;
#endif
    ಯದಿ (cur_si != ರಿಕ್ತ)
    {
#ifndef FEAT_EVAL
	ಪೂರ್ಣಾಙ್ಕ	current_trans_id = 0;
#endif
	ಪ್ರತಿ (idx = current_state.ga_len - 1; idx >= 0; --idx)
	{
	    sip = &CUR_STATE(idx);
	    ಯದಿ ((current_lnum > sip->si_h_startpos.lnum
			|| (current_lnum == sip->si_h_startpos.lnum
			    && current_col >= sip->si_h_startpos.col))
		    && (sip->si_h_endpos.lnum == 0
			|| current_lnum < sip->si_h_endpos.lnum
			|| (current_lnum == sip->si_h_endpos.lnum
			    && current_col < sip->si_h_endpos.col)))
	    {
		current_attr = sip->si_attr;
#ifdef FEAT_EVAL
		current_id = sip->si_id;
#endif
		current_trans_id = sip->si_trans_id;
#ifdef FEAT_CONCEAL
		current_flags = sip->si_flags;
		current_seqnr = sip->si_seqnr;
		current_sub_char = sip->si_cchar;
#endif
		ಭಙ್ಗ;
	    }
	}

	ಯದಿ (can_spell != ರಿಕ್ತ)
	{
	    ಸಂರಚನಾ sp_syn   sps;

	    /*
	     * set "can_spell" to TRUE if spell checking is supposed to be
	     * done in the current item.
	     */
	    ಯದಿ (syn_block->b_spell_cluster_id == 0)
	    {
		// There is no @Spell cluster: Do spelling for items without
		// @NoSpell cluster.
		ಯದಿ (syn_block->b_nospell_cluster_id == 0
						     || current_trans_id == 0)
		    *can_spell = (syn_block->b_syn_spell != SYNSPL_NOTOP);
		ಅನ್ಯಥಾ
		{
		    sps.inc_tag = 0;
		    sps.id = syn_block->b_nospell_cluster_id;
		    sps.cont_in_list = ರಿಕ್ತ;
		    *can_spell = !in_id_list(sip, sip->si_cont_list, &sps, 0);
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		// The @Spell cluster is defined: Do spelling in items with
		// the @Spell cluster.  But not when @NoSpell is also there.
		// At the toplevel only spell check when ":syn spell toplevel"
		// was used.
		ಯದಿ (current_trans_id == 0)
		    *can_spell = (syn_block->b_syn_spell == SYNSPL_ತುದಿ);
		ಅನ್ಯಥಾ
		{
		    sps.inc_tag = 0;
		    sps.id = syn_block->b_spell_cluster_id;
		    sps.cont_in_list = ರಿಕ್ತ;
		    *can_spell = in_id_list(sip, sip->si_cont_list, &sps, 0);

		    ಯದಿ (syn_block->b_nospell_cluster_id != 0)
		    {
			sps.id = syn_block->b_nospell_cluster_id;
			ಯದಿ (in_id_list(sip, sip->si_cont_list, &sps, 0))
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
	ಯದಿ (!syncing && !keep_state)
	{
	    check_state_ends();
	    ಯದಿ (current_state.ga_len > 0
				      && syn_getcurline()[current_col] != NUL)
	    {
		++current_col;
		check_state_ends();
		--current_col;
	    }
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (can_spell != ರಿಕ್ತ)
	// Default: Only do spelling when there is no @Spell cluster or when
	// ":syn spell toplevel" was used.
	*can_spell = syn_block->b_syn_spell == SYNSPL_DEFAULT
		    ? (syn_block->b_spell_cluster_id == 0)
		    : (syn_block->b_syn_spell == SYNSPL_ತುದಿ);

    // nextgroup ends at end of line, unless "skipnl" or "skipempty" present
    ಯದಿ (current_next_list != ರಿಕ್ತ
	    && (ಸಾಲು = syn_getcurline())[current_col] != NUL
	    && ಸಾಲು[current_col + 1] == NUL
	    && !(current_next_flags & (HL_SKIPNL | HL_SKIPEMPTY)))
	current_next_list = ರಿಕ್ತ;

    ಯದಿ (zero_width_next_ga.ga_len > 0)
	ga_clear(&zero_width_next_ga);

    // No longer need external matches.  But keep next_match_extmatch.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = ರಿಕ್ತ;
    unref_extmatch(cur_extmatch);

    ಪ್ರತ್ಯಾಗಮ current_attr;
}


/*
 * Check if we already matched pattern "idx" at the current column.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
did_match_already(ಪೂರ್ಣಾಙ್ಕ idx, garray_T *gap)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = current_state.ga_len; --i >= 0; )
	ಯದಿ (CUR_STATE(i).si_m_startcol == (ಪೂರ್ಣಾಙ್ಕ)current_col
		&& CUR_STATE(i).si_m_lnum == (ಪೂರ್ಣಾಙ್ಕ)current_lnum
		&& CUR_STATE(i).si_idx == idx)
	    ಪ್ರತ್ಯಾಗಮ TRUE;

    // Zero-width matches with a nextgroup argument are not put on the syntax
    // stack, and can only be matched once anyway.
    ಪ್ರತಿ (i = gap->ga_len; --i >= 0; )
	ಯದಿ (((ಪೂರ್ಣಾಙ್ಕ *)(gap->ga_data))[i] == idx)
	    ಪ್ರತ್ಯಾಗಮ TRUE;

    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Push the next match onto the stack.
 */
    ಸ್ಥಿರ stateitem_T *
ತಳ್ಳು_next_match(stateitem_T *cur_si)
{
    synpat_T	*spp;
#ifdef FEAT_CONCEAL
    ಪೂರ್ಣಾಙ್ಕ		 save_flags;
#endif

    spp = &(SYN_ವಸ್ತುಗಳು(syn_block)[next_match_idx]);

    /*
     * Push the item in current_state stack;
     */
    ಯದಿ (ತಳ್ಳು_current_state(next_match_idx) == OK)
    {
	/*
	 * If it's a start-skip-end type that crosses lines, figure out how
	 * much it continues in this line.  Otherwise just fill in the length.
	 */
	cur_si = &CUR_STATE(current_state.ga_len - 1);
	cur_si->si_h_startpos = next_match_h_startpos;
	cur_si->si_m_startcol = current_col;
	cur_si->si_m_lnum = current_lnum;
	cur_si->si_flags = spp->sp_flags;
#ifdef FEAT_CONCEAL
	cur_si->si_seqnr = next_seqnr++;
	cur_si->si_cchar = spp->sp_cchar;
	ಯದಿ (current_state.ga_len > 1)
	    cur_si->si_flags |=
		    CUR_STATE(current_state.ga_len - 2).si_flags & HL_CONCEAL;
#endif
	cur_si->si_next_list = spp->sp_next_list;
	cur_si->si_extmatch = ref_extmatch(next_match_extmatch);
	ಯದಿ (spp->sp_type == SPTYPE_START && !(spp->sp_flags & HL_ONELINE))
	{
	    // Try to find the end pattern in the current line
	    update_si_end(cur_si, (ಪೂರ್ಣಾಙ್ಕ)(next_match_m_endpos.col), TRUE);
	    check_keepend();
	}
	ಅನ್ಯಥಾ
	{
	    cur_si->si_m_endpos = next_match_m_endpos;
	    cur_si->si_h_endpos = next_match_h_endpos;
	    cur_si->si_ends = TRUE;
	    cur_si->si_flags |= next_match_flags;
	    cur_si->si_eoe_pos = next_match_eoe_pos;
	    cur_si->si_end_idx = next_match_end_idx;
	}
	ಯದಿ (keepend_level < 0 && (cur_si->si_flags & HL_KEEPEND))
	    keepend_level = current_state.ga_len - 1;
	check_keepend();
	update_si_attr(current_state.ga_len - 1);

#ifdef FEAT_CONCEAL
	save_flags = cur_si->si_flags & (HL_CONCEAL | HL_CONCEALENDS);
#endif
	/*
	 * If the start pattern has another highlight group, push another item
	 * on the stack for the start pattern.
	 */
	ಯದಿ (	   spp->sp_type == SPTYPE_START
		&& spp->sp_syn_match_id != 0
		&& ತಳ್ಳು_current_state(next_match_idx) == OK)
	{
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	    cur_si->si_h_startpos = next_match_h_startpos;
	    cur_si->si_m_startcol = current_col;
	    cur_si->si_m_lnum = current_lnum;
	    cur_si->si_m_endpos = next_match_eos_pos;
	    cur_si->si_h_endpos = next_match_eos_pos;
	    cur_si->si_ends = TRUE;
	    cur_si->si_end_idx = 0;
	    cur_si->si_flags = HL_MATCH;
#ifdef FEAT_CONCEAL
	    cur_si->si_seqnr = next_seqnr++;
	    cur_si->si_flags |= save_flags;
	    ಯದಿ (cur_si->si_flags & HL_CONCEALENDS)
		cur_si->si_flags |= HL_CONCEAL;
#endif
	    cur_si->si_next_list = ರಿಕ್ತ;
	    check_keepend();
	    update_si_attr(current_state.ga_len - 1);
	}
    }

    next_match_idx = -1;	// try other match next time

    ಪ್ರತ್ಯಾಗಮ cur_si;
}

/*
 * Check for end of current state (and the states before it).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
check_state_ends(ಶೂನ್ಯ)
{
    stateitem_T	*cur_si;
    ಪೂರ್ಣಾಙ್ಕ		had_extend;

    cur_si = &CUR_STATE(current_state.ga_len - 1);
    ಪ್ರತಿ (;;)
    {
	ಯದಿ (cur_si->si_ends
		&& (cur_si->si_m_endpos.lnum < current_lnum
		    || (cur_si->si_m_endpos.lnum == current_lnum
			&& cur_si->si_m_endpos.col <= current_col)))
	{
	    /*
	     * If there is an end pattern group ID, highlight the end pattern
	     * now.  No need to pop the current item from the stack.
	     * Only do this if the end pattern continues beyond the current
	     * position.
	     */
	    ಯದಿ (cur_si->si_end_idx
		    && (cur_si->si_eoe_pos.lnum > current_lnum
			|| (cur_si->si_eoe_pos.lnum == current_lnum
			    && cur_si->si_eoe_pos.col > current_col)))
	    {
		cur_si->si_idx = cur_si->si_end_idx;
		cur_si->si_end_idx = 0;
		cur_si->si_m_endpos = cur_si->si_eoe_pos;
		cur_si->si_h_endpos = cur_si->si_eoe_pos;
		cur_si->si_flags |= HL_MATCH;
#ifdef FEAT_CONCEAL
		cur_si->si_seqnr = next_seqnr++;
		ಯದಿ (cur_si->si_flags & HL_CONCEALENDS)
		    cur_si->si_flags |= HL_CONCEAL;
#endif
		update_si_attr(current_state.ga_len - 1);

		// nextgroup= should not match in the end pattern
		current_next_list = ರಿಕ್ತ;

		// what matches next may be different now, clear it
		next_match_idx = 0;
		next_match_col = MAXCOL;
		ಭಙ್ಗ;
	    }

	    // handle next_list, unless at end of line and no "skipnl" or
	    // "skipempty"
	    current_next_list = cur_si->si_next_list;
	    current_next_flags = cur_si->si_flags;
	    ಯದಿ (!(current_next_flags & (HL_SKIPNL | HL_SKIPEMPTY))
		    && syn_getcurline()[current_col] == NUL)
		current_next_list = ರಿಕ್ತ;

	    // When the ended item has "extend", another item with
	    // "keepend" now needs to check for its end.
	    had_extend = (cur_si->si_flags & HL_EXTEND);

	    ತೆಗೆ_current_state();

	    ಯದಿ (current_state.ga_len == 0)
		ಭಙ್ಗ;

	    ಯದಿ (had_extend && keepend_level >= 0)
	    {
		syn_update_ends(FALSE);
		ಯದಿ (current_state.ga_len == 0)
		    ಭಙ್ಗ;
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
	    ಯದಿ (cur_si->si_idx >= 0
		    && SYN_ವಸ್ತುಗಳು(syn_block)[cur_si->si_idx].sp_type
								== SPTYPE_START
		    && !(cur_si->si_flags & (HL_MATCH | HL_KEEPEND)))
	    {
		update_si_end(cur_si, (ಪೂರ್ಣಾಙ್ಕ)current_col, TRUE);
		check_keepend();
		ಯದಿ ((current_next_flags & HL_HAS_EOL)
			&& keepend_level < 0
			&& syn_getcurline()[current_col] == NUL)
		    ಭಙ್ಗ;
	    }
	}
	ಅನ್ಯಥಾ
	    ಭಙ್ಗ;
    }
}

/*
 * Update an entry in the current_state stack for a match or region.  This
 * fills in si_attr, si_next_list and si_cont_list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
update_si_attr(ಪೂರ್ಣಾಙ್ಕ idx)
{
    stateitem_T	*sip = &CUR_STATE(idx);
    synpat_T	*spp;

    // This should not happen...
    ಯದಿ (sip->si_idx < 0)
	ಪ್ರತ್ಯಾಗಮ;

    spp = &(SYN_ವಸ್ತುಗಳು(syn_block)[sip->si_idx]);
    ಯದಿ (sip->si_flags & HL_MATCH)
	sip->si_id = spp->sp_syn_match_id;
    ಅನ್ಯಥಾ
	sip->si_id = spp->sp_syn.id;
    sip->si_attr = syn_id2attr(sip->si_id);
    sip->si_trans_id = sip->si_id;
    ಯದಿ (sip->si_flags & HL_MATCH)
	sip->si_cont_list = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
	sip->si_cont_list = spp->sp_cont_list;

    /*
     * For transparent items, take attr from outer item.
     * Also take cont_list, if there is none.
     * Don't do this for the matchgroup of a start or end pattern.
     */
    ಯದಿ ((spp->sp_flags & HL_TRANSP) && !(sip->si_flags & HL_MATCH))
    {
	ಯದಿ (idx == 0)
	{
	    sip->si_attr = 0;
	    sip->si_trans_id = 0;
	    ಯದಿ (sip->si_cont_list == ರಿಕ್ತ)
		sip->si_cont_list = ID_LIST_ALL;
	}
	ಅನ್ಯಥಾ
	{
	    sip->si_attr = CUR_STATE(idx - 1).si_attr;
	    sip->si_trans_id = CUR_STATE(idx - 1).si_trans_id;
	    ಯದಿ (sip->si_cont_list == ರಿಕ್ತ)
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
    ಪೂರ್ಣಾಙ್ಕ		i;
    lpos_T	maxpos;
    lpos_T	maxpos_h;
    stateitem_T	*sip;

    /*
     * This check can consume a lot of time; only do it from the level where
     * there really is a keepend.
     */
    ಯದಿ (keepend_level < 0)
	ಪ್ರತ್ಯಾಗಮ;

    /*
     * Find the last index of an "extend" item.  "keepend" items before that
     * won't do anything.  If there is no "extend" item "i" will be
     * "keepend_level" and all "keepend" items will work normally.
     */
    ಪ್ರತಿ (i = current_state.ga_len - 1; i > keepend_level; --i)
	ಯದಿ (CUR_STATE(i).si_flags & HL_EXTEND)
	    ಭಙ್ಗ;

    maxpos.lnum = 0;
    maxpos.col = 0;
    maxpos_h.lnum = 0;
    maxpos_h.col = 0;
    ಪ್ರತಿ ( ; i < current_state.ga_len; ++i)
    {
	sip = &CUR_STATE(i);
	ಯದಿ (maxpos.lnum != 0)
	{
	    limit_pos_zero(&sip->si_m_endpos, &maxpos);
	    limit_pos_zero(&sip->si_h_endpos, &maxpos_h);
	    limit_pos_zero(&sip->si_eoe_pos, &maxpos);
	    sip->si_ends = TRUE;
	}
	ಯದಿ (sip->si_ends && (sip->si_flags & HL_KEEPEND))
	{
	    ಯದಿ (maxpos.lnum == 0
		    || maxpos.lnum > sip->si_m_endpos.lnum
		    || (maxpos.lnum == sip->si_m_endpos.lnum
			&& maxpos.col > sip->si_m_endpos.col))
		maxpos = sip->si_m_endpos;
	    ಯದಿ (maxpos_h.lnum == 0
		    || maxpos_h.lnum > sip->si_h_endpos.lnum
		    || (maxpos_h.lnum == sip->si_h_endpos.lnum
			&& maxpos_h.col > sip->si_h_endpos.col))
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
update_si_end(
    stateitem_T	*sip,
    ಪೂರ್ಣಾಙ್ಕ		startcol,   // where to start searching for the end
    ಪೂರ್ಣಾಙ್ಕ		force)	    // when TRUE overrule a previous end
{
    lpos_T	startpos;
    lpos_T	endpos;
    lpos_T	hl_endpos;
    lpos_T	end_endpos;
    ಪೂರ್ಣಾಙ್ಕ		end_idx;

    // return quickly for a keyword
    ಯದಿ (sip->si_idx < 0)
	ಪ್ರತ್ಯಾಗಮ;

    // Don't update when it's already done.  Can be a match of an end pattern
    // that started in a previous line.  Watch out: can also be a "keepend"
    // from a containing item.
    ಯದಿ (!force && sip->si_m_endpos.lnum >= current_lnum)
	ಪ್ರತ್ಯಾಗಮ;

    /*
     * We need to find the end of the region.  It may continue in the next
     * line.
     */
    end_idx = 0;
    startpos.lnum = current_lnum;
    startpos.col = startcol;
    find_endpos(sip->si_idx, &startpos, &endpos, &hl_endpos,
		   &(sip->si_flags), &end_endpos, &end_idx, sip->si_extmatch);

    ಯದಿ (endpos.lnum == 0)
    {
	// No end pattern matched.
	ಯದಿ (SYN_ವಸ್ತುಗಳು(syn_block)[sip->si_idx].sp_flags & HL_ONELINE)
	{
	    // a "oneline" never continues in the next line
	    sip->si_ends = TRUE;
	    sip->si_m_endpos.lnum = current_lnum;
	    sip->si_m_endpos.col = syn_getcurline_len();
	}
	ಅನ್ಯಥಾ
	{
	    // continues in the next line
	    sip->si_ends = FALSE;
	    sip->si_m_endpos.lnum = 0;
	}
	sip->si_h_endpos = sip->si_m_endpos;
    }
    ಅನ್ಯಥಾ
    {
	// match within this line
	sip->si_m_endpos = endpos;
	sip->si_h_endpos = hl_endpos;
	sip->si_eoe_pos = end_endpos;
	sip->si_ends = TRUE;
	sip->si_end_idx = end_idx;
    }
}

/*
 * Add a new state to the current state stack.
 * It is cleared and the index set to "idx".
 * Return FAIL if it's not possible (out of memory).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ತಳ್ಳು_current_state(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (ga_grow(&current_state, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    CLEAR_POINTER(&CUR_STATE(current_state.ga_len));
    CUR_STATE(current_state.ga_len).si_idx = idx;
    ++current_state.ga_len;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Remove a state from the current_state stack.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ತೆಗೆ_current_state(ಶೂನ್ಯ)
{
    ಯದಿ (current_state.ga_len)
    {
	unref_extmatch(CUR_STATE(current_state.ga_len - 1).si_extmatch);
	--current_state.ga_len;
    }
    // after the end of a pattern, try matching a keyword or pattern
    next_match_idx = -1;

    // if first state with "keepend" is popped, reset keepend_level
    ಯದಿ (keepend_level >= current_state.ga_len)
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
find_endpos(
    ಪೂರ್ಣಾಙ್ಕ		idx,		// index of the pattern
    lpos_T	*startpos,	// where to start looking for an END match
    lpos_T	*m_endpos,	// return: end of match
    lpos_T	*hl_endpos,	// return: end of highlighting
    long	*flagsp,	// return: flags of matching END
    lpos_T	*end_endpos,	// return: end of end pattern match
    ಪೂರ್ಣಾಙ್ಕ		*end_idx,	// return: group ID for end pat. match, or 0
    reg_extmatch_T *start_ext)	// submatches from the start pattern
{
    colnr_T	matchcol;
    synpat_T	*spp, *spp_skip;
    ಪೂರ್ಣಾಙ್ಕ		start_idx;
    ಪೂರ್ಣಾಙ್ಕ		best_idx;
    regmmatch_T	regmatch;
    regmmatch_T	best_regmatch;	    // startpos/endpos of best match
    lpos_T	pos;
    ಪೂರ್ಣಾಙ್ಕ		had_match = FALSE;
    char_u	buf_chartab[32];  // chartab array for syn option iskyeyword

    // just in case we are invoked for a keyword
    ಯದಿ (idx < 0)
	ಪ್ರತ್ಯಾಗಮ;

    /*
     * Check for being called with a START pattern.
     * Can happen with a match that continues to the next line, because it
     * contained a region.
     */
    spp = &(SYN_ವಸ್ತುಗಳು(syn_block)[idx]);
    ಯದಿ (spp->sp_type != SPTYPE_START)
    {
	*hl_endpos = *startpos;
	ಪ್ರತ್ಯಾಗಮ;
    }

    /*
     * Find the SKIP or first END pattern after the last START pattern.
     */
    ಪ್ರತಿ (;;)
    {
	spp = &(SYN_ವಸ್ತುಗಳು(syn_block)[idx]);
	ಯದಿ (spp->sp_type != SPTYPE_START)
	    ಭಙ್ಗ;
	++idx;
    }

    /*
     *	Lookup the SKIP pattern (if present)
     */
    ಯದಿ (spp->sp_type == SPTYPE_SKIP)
    {
	spp_skip = spp;
	++idx;
    }
    ಅನ್ಯಥಾ
	spp_skip = ರಿಕ್ತ;

    // Setup external matches for syn_regexec().
    unref_extmatch(re_extmatch_in);
    re_extmatch_in = ref_extmatch(start_ext);

    matchcol = startpos->col;	// start looking for a match at sstart
    start_idx = idx;		// remember the first END pattern.
    best_regmatch.startpos[0].col = 0;		// avoid compiler warning

    // use syntax iskeyword option
    save_chartab(buf_chartab);

    ಪ್ರತಿ (;;)
    {
	/*
	 * Find end pattern that matches first after "matchcol".
	 */
	best_idx = -1;
	ಪ್ರತಿ (idx = start_idx; idx < syn_block->b_syn_patterns.ga_len; ++idx)
	{
	    ಪೂರ್ಣಾಙ್ಕ lc_col = matchcol;
	    ಪೂರ್ಣಾಙ್ಕ r;

	    spp = &(SYN_ವಸ್ತುಗಳು(syn_block)[idx]);
	    ಯದಿ (spp->sp_type != SPTYPE_END)	// past last END pattern
		ಭಙ್ಗ;
	    lc_col -= spp->sp_offsets[SPO_LC_OFF];
	    ಯದಿ (lc_col < 0)
		lc_col = 0;

	    regmatch.rmm_ic = spp->sp_ic;
	    regmatch.regprog = spp->sp_prog;
	    r = syn_regexec(&regmatch, startpos->lnum, lc_col,
						  IF_SYN_TIME(&spp->sp_time));
	    spp->sp_prog = regmatch.regprog;
	    ಯದಿ (r)
	    {
		ಯದಿ (best_idx == -1 || regmatch.startpos[0].col
					      < best_regmatch.startpos[0].col)
		{
		    best_idx = idx;
		    best_regmatch.startpos[0] = regmatch.startpos[0];
		    best_regmatch.endpos[0] = regmatch.endpos[0];
		}
	    }
	}

	/*
	 * If all end patterns have been tried, and there is no match, the
	 * item continues until end-of-line.
	 */
	ಯದಿ (best_idx == -1)
	    ಭಙ್ಗ;

	/*
	 * If the skip pattern matches before the end pattern,
	 * continue searching after the skip pattern.
	 */
	ಯದಿ (spp_skip != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ lc_col = matchcol - spp_skip->sp_offsets[SPO_LC_OFF];
	    ಪೂರ್ಣಾಙ್ಕ r;

	    ಯದಿ (lc_col < 0)
		lc_col = 0;
	    regmatch.rmm_ic = spp_skip->sp_ic;
	    regmatch.regprog = spp_skip->sp_prog;
	    r = syn_regexec(&regmatch, startpos->lnum, lc_col,
					      IF_SYN_TIME(&spp_skip->sp_time));
	    spp_skip->sp_prog = regmatch.regprog;
	    ಯದಿ (r && regmatch.startpos[0].col
					     <= best_regmatch.startpos[0].col)
	    {
		ಪೂರ್ಣಾಙ್ಕ ಸಾಲು_len;

		// Add offset to skip pattern match
		syn_add_end_off(&pos, &regmatch, spp_skip, SPO_ME_OFF, 1);

		// If the skip pattern goes on to the next line, there is no
		// match with an end pattern in this line.
		ಯದಿ (pos.lnum > startpos->lnum)
		    ಭಙ್ಗ;

		ಸಾಲು_len = ml_get_buf_len(syn_buf, startpos->lnum);

		// take care of an empty match or negative offset
		ಯದಿ (pos.col <= matchcol)
		    ++matchcol;
		ಅನ್ಯಥಾ ಯದಿ (pos.col <= regmatch.endpos[0].col)
		    matchcol = pos.col;
		ಅನ್ಯಥಾ
		    // Be careful not to jump over the NUL at the end-of-line
		    ಪ್ರತಿ (matchcol = regmatch.endpos[0].col;
			    matchcol < ಸಾಲು_len && matchcol < pos.col;
								   ++matchcol)
			;

		// if the skip pattern includes end-of-line, break here
		ಯದಿ (matchcol >= ಸಾಲು_len)
		    ಭಙ್ಗ;

		ಅನುವರ್ತ;	    // start with first end pattern again
	    }
	}

	/*
	 * Match from start pattern to end pattern.
	 * Correct for match and highlight offset of end pattern.
	 */
	spp = &(SYN_ವಸ್ತುಗಳು(syn_block)[best_idx]);
	syn_add_end_off(m_endpos, &best_regmatch, spp, SPO_ME_OFF, 1);
	// can't end before the start
	ಯದಿ (m_endpos->lnum == startpos->lnum && m_endpos->col < startpos->col)
	    m_endpos->col = startpos->col;

	syn_add_end_off(end_endpos, &best_regmatch, spp, SPO_HE_OFF, 1);
	// can't end before the start
	ಯದಿ (end_endpos->lnum == startpos->lnum
					   && end_endpos->col < startpos->col)
	    end_endpos->col = startpos->col;
	// can't end after the match
	limit_pos(end_endpos, m_endpos);

	/*
	 * If the end group is highlighted differently, adjust the pointers.
	 */
	ಯದಿ (spp->sp_syn_match_id != spp->sp_syn.id && spp->sp_syn_match_id != 0)
	{
	    *end_idx = best_idx;
	    ಯದಿ (spp->sp_off_flags & (1 << (SPO_RE_OFF + SPO_ಎಣಿಕೆ)))
	    {
		hl_endpos->lnum = best_regmatch.endpos[0].lnum;
		hl_endpos->col = best_regmatch.endpos[0].col;
	    }
	    ಅನ್ಯಥಾ
	    {
		hl_endpos->lnum = best_regmatch.startpos[0].lnum;
		hl_endpos->col = best_regmatch.startpos[0].col;
	    }
	    hl_endpos->col += spp->sp_offsets[SPO_RE_OFF];

	    // can't end before the start
	    ಯದಿ (hl_endpos->lnum == startpos->lnum
					    && hl_endpos->col < startpos->col)
		hl_endpos->col = startpos->col;
	    limit_pos(hl_endpos, m_endpos);

	    // now the match ends where the highlighting ends, it is turned
	    // into the matchgroup for the end
	    *m_endpos = *hl_endpos;
	}
	ಅನ್ಯಥಾ
	{
	    *end_idx = 0;
	    *hl_endpos = *end_endpos;
	}

	*flagsp = spp->sp_flags;

	had_match = TRUE;
	ಭಙ್ಗ;
    }

    // no match for an END pattern in this line
    ಯದಿ (!had_match)
	m_endpos->lnum = 0;

    restore_chartab(buf_chartab);

    // Remove external matches.
    unref_extmatch(re_extmatch_in);
    re_extmatch_in = ರಿಕ್ತ;
}

/*
 * Limit "pos" not to be after "limit".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
limit_pos(lpos_T *pos, lpos_T *limit)
{
    ಯದಿ (pos->lnum > limit->lnum)
	*pos = *limit;
    ಅನ್ಯಥಾ ಯದಿ (pos->lnum == limit->lnum && pos->col > limit->col)
	pos->col = limit->col;
}

/*
 * Limit "pos" not to be after "limit", unless pos->lnum is zero.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
limit_pos_zero(
    lpos_T	*pos,
    lpos_T	*limit)
{
    ಯದಿ (pos->lnum == 0)
	*pos = *limit;
    ಅನ್ಯಥಾ
	limit_pos(pos, limit);
}

/*
 * Add offset to matched text for end of match or highlight.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_add_end_off(
    lpos_T	*result,	// returned position
    regmmatch_T	*regmatch,	// start/end of match
    synpat_T	*spp,		// matched pattern
    ಪೂರ್ಣಾಙ್ಕ		idx,		// index of offset
    ಪೂರ್ಣಾಙ್ಕ		extra)		// extra chars for offset to start
{
    ಪೂರ್ಣಾಙ್ಕ		col;
    ಪೂರ್ಣಾಙ್ಕ		off;
    char_u	*base;
    char_u	*p;

    ಯದಿ (spp->sp_off_flags & (1 << idx))
    {
	result->lnum = regmatch->startpos[0].lnum;
	col = regmatch->startpos[0].col;
	off = spp->sp_offsets[idx] + extra;
    }
    ಅನ್ಯಥಾ
    {
	result->lnum = regmatch->endpos[0].lnum;
	col = regmatch->endpos[0].col;
	off = spp->sp_offsets[idx];
    }
    // Don't go past the end of the line.  Matters for "rs=e+2" when there
    // is a matchgroup. Watch out for match with last NL in the buffer.
    ಯದಿ (result->lnum > syn_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	col = 0;
    ಅನ್ಯಥಾ ಯದಿ (off != 0)
    {
	base = ml_get_buf(syn_buf, result->lnum, FALSE);
	p = base + col;
	ಯದಿ (off > 0)
	{
	    ಯಾವತ್ (off-- > 0 && *p != NUL)
		MB_PTR_ADV(p);
	}
	ಅನ್ಯಥಾ ಯದಿ (off < 0)
	{
	    ಯಾವತ್ (off++ < 0 && base < p)
		MB_PTR_BACK(base, p);
	}
	col = (ಪೂರ್ಣಾಙ್ಕ)(p - base);
    }
    result->col = col;
}

/*
 * Add offset to matched text for start of match or highlight.
 * Avoid resulting column to become negative.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_add_start_off(
    lpos_T	*result,	// returned position
    regmmatch_T	*regmatch,	// start/end of match
    synpat_T	*spp,
    ಪೂರ್ಣಾಙ್ಕ		idx,
    ಪೂರ್ಣಾಙ್ಕ		extra)	    // extra chars for offset to end
{
    ಪೂರ್ಣಾಙ್ಕ		col;
    ಪೂರ್ಣಾಙ್ಕ		off;
    char_u	*base;
    char_u	*p;

    ಯದಿ (spp->sp_off_flags & (1 << (idx + SPO_ಎಣಿಕೆ)))
    {
	result->lnum = regmatch->endpos[0].lnum;
	col = regmatch->endpos[0].col;
	off = spp->sp_offsets[idx] + extra;
    }
    ಅನ್ಯಥಾ
    {
	result->lnum = regmatch->startpos[0].lnum;
	col = regmatch->startpos[0].col;
	off = spp->sp_offsets[idx];
    }
    ಯದಿ (result->lnum > syn_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	// a "\n" at the end of the pattern may take us below the last line
	result->lnum = syn_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	col = ml_get_buf_len(syn_buf, result->lnum);
    }
    ಯದಿ (off != 0)
    {
	base = ml_get_buf(syn_buf, result->lnum, FALSE);
	p = base + col;
	ಯದಿ (off > 0)
	{
	    ಯಾವತ್ (off-- && *p != NUL)
		MB_PTR_ADV(p);
	}
	ಅನ್ಯಥಾ ಯದಿ (off < 0)
	{
	    ಯಾವತ್ (off++ && base < p)
		MB_PTR_BACK(base, p);
	}
	col = (ಪೂರ್ಣಾಙ್ಕ)(p - base);
    }
    result->col = col;
}

/*
 * Get current line in syntax buffer.
 */
    ಸ್ಥಿರ char_u *
syn_getcurline(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ ml_get_buf(syn_buf, current_lnum, FALSE);
}

/*
 * Get length of current line in syntax buffer.
 */
    ಸ್ಥಿರ colnr_T
syn_getcurline_len(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ ml_get_buf_len(syn_buf, current_lnum);
}

/*
 * Call vim_regexec() to find a match with "rmp" in "syn_buf".
 * Returns TRUE when there is a match.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
syn_regexec(
    regmmatch_T	*rmp,
    linenr_T	lnum,
    colnr_T	col,
    syn_time_T  *st UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		r;
    ಪೂರ್ಣಾಙ್ಕ		timed_out = FALSE;
#ifdef FEAT_PROFILE
    proftime_T	pt;

    ಯದಿ (syn_time_on)
	profile_start(&pt);
#endif

    ಯದಿ (rmp->regprog == ರಿಕ್ತ)
	// This can happen if a previous call to vim_regexec_multi() tried to
	// use the NFA engine, which resulted in NFA_TOO_EXPENSIVE, and
	// compiling the pattern with the other engine fails.
	ಪ್ರತ್ಯಾಗಮ FALSE;

    rmp->rmm_maxcol = syn_buf->b_p_smc;
    r = vim_regexec_multi(rmp, syn_win, syn_buf, lnum, col, &timed_out);

#ifdef FEAT_PROFILE
    ಯದಿ (syn_time_on)
    {
	profile_end(&pt);
	profile_add(&st->total, &pt);
	ಯದಿ (profile_cmp(&pt, &st->slowest) < 0)
	    st->slowest = pt;
	++st->ಎಣಿಕೆ;
	ಯದಿ (r > 0)
	    ++st->match;
    }
#endif
#ifdef FEAT_RELTIME
    ಯದಿ (timed_out && redrawtime_limit_set && !syn_win->w_s->b_syn_slow)
    {
	syn_win->w_s->b_syn_slow = TRUE;
	ಸಂದೇಶ(_("'redrawtime' exceeded, syntax highlighting disabled"));
    }
#endif

    ಯದಿ (r > 0)
    {
	rmp->startpos[0].lnum += lnum;
	rmp->endpos[0].lnum += lnum;
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Check one position in a line for a matching keyword.
 * The caller must check if a keyword can start at startcol.
 * Return its ID if found, 0 otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_keyword_id(
    char_u	*ಸಾಲು,
    ಪೂರ್ಣಾಙ್ಕ		startcol,	// position in line to check for keyword
    ಪೂರ್ಣಾಙ್ಕ		*endcolp,	// return: character after found keyword
    long	*flagsp,	// return: flags of matching keyword
    short	**next_listp,	// return: next_list of matching keyword
    stateitem_T	*cur_si,	// item at the top of the stack
    ಪೂರ್ಣಾಙ್ಕ		*ccharp UNUSED)	// conceal substitution char
{
    keyentry_T	*kp;
    char_u	*kwp;
    ಪೂರ್ಣಾಙ್ಕ		round;
    ಪೂರ್ಣಾಙ್ಕ		kwlen;
    char_u	keyword[MAXKEYWLEN + 1]; // assume max. keyword len is 80
    hashtab_T	*ht;
    hashitem_T	*hi;

    // Find first character after the keyword.  First character was already
    // checked.
    kwp = ಸಾಲು + startcol;
    kwlen = 0;
    ಕುರು
    {
	ಯದಿ (has_mbyte)
	    kwlen += (*mb_ptr2len)(kwp + kwlen);
	ಅನ್ಯಥಾ
	    ++kwlen;
    }
    ಯಾವತ್ (vim_iswordp_buf(kwp + kwlen, syn_buf));

    ಯದಿ (kwlen > MAXKEYWLEN)
	ಪ್ರತ್ಯಾಗಮ 0;

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
    ಪ್ರತಿ (round = 1; round <= 2; ++round)
    {
	ht = round == 1 ? &syn_block->b_keywtab : &syn_block->b_keywtab_ic;
	ಯದಿ (ht->ht_used == 0)
	    ಅನುವರ್ತ;
	ಯದಿ (round == 2)	// ignore case
	    (ಶೂನ್ಯ)str_foldcase(kwp, kwlen, keyword, MAXKEYWLEN + 1);

	/*
	 * Find keywords that match.  There can be several with different
	 * attributes.
	 * When current_next_list is non-zero accept only that group, otherwise:
	 *  Accept a not-contained keyword at toplevel.
	 *  Accept a keyword at other levels only if it is in the contains list.
	 */
	hi = hash_find(ht, keyword);
	ಯದಿ (!HASHITEM_EMPTY(hi))
	    ಪ್ರತಿ (kp = HI2KE(hi); kp != ರಿಕ್ತ; kp = kp->ke_next)
	    {
		ಯದಿ (current_next_list != 0
			? in_id_list(ರಿಕ್ತ, current_next_list, &kp->k_syn, 0)
			: (cur_si == ರಿಕ್ತ
			    ? !(kp->flags & HL_CONTAINED)
			    : in_id_list(cur_si, cur_si->si_cont_list,
				      &kp->k_syn, kp->flags)))
		{
		    *endcolp = startcol + kwlen;
		    *flagsp = kp->flags;
		    *next_listp = kp->next_list;
#ifdef FEAT_CONCEAL
		    *ccharp = kp->k_char;
#endif
		    ಪ್ರತ್ಯಾಗಮ kp->k_syn.id;
		}
	    }
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Handle ":syntax conceal" command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_conceal(exarg_T *eap UNUSED, ಪೂರ್ಣಾಙ್ಕ syncing UNUSED)
{
#ifdef FEAT_CONCEAL
    char_u	*arg = eap->arg;
    char_u	*next;

    eap->nextcmd = find_nextcmd(arg);
    ಯದಿ (eap->skip)
	ಪ್ರತ್ಯಾಗಮ;

    next = skiptowhite(arg);
    ಯದಿ (*arg == NUL)
    {
	ಯದಿ (curwin->w_s->b_syn_conceal)
	    ಸಂದೇಶ("syntax conceal on");
	ಅನ್ಯಥಾ
	    ಸಂದೇಶ("syntax conceal off");
    }
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(arg, "on", 2) == 0 && next - arg == 2)
	curwin->w_s->b_syn_conceal = TRUE;
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(arg, "off", 3) == 0 && next - arg == 3)
	curwin->w_s->b_syn_conceal = FALSE;
    ಅನ್ಯಥಾ
	semsg(_(e_illegal_argument_str_2), arg);
#endif
}

/*
 * Handle ":syntax case" command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_case(exarg_T *eap, ಪೂರ್ಣಾಙ್ಕ syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*next;

    eap->nextcmd = find_nextcmd(arg);
    ಯದಿ (eap->skip)
	ಪ್ರತ್ಯಾಗಮ;

    next = skiptowhite(arg);
    ಯದಿ (*arg == NUL)
    {
	ಯದಿ (curwin->w_s->b_syn_ic)
	    ಸಂದೇಶ("syntax case ignore");
	ಅನ್ಯಥಾ
	    ಸಂದೇಶ("syntax case match");
    }
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(arg, "match", 5) == 0 && next - arg == 5)
	curwin->w_s->b_syn_ic = FALSE;
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(arg, "ignore", 6) == 0 && next - arg == 6)
	curwin->w_s->b_syn_ic = TRUE;
    ಅನ್ಯಥಾ
	semsg(_(e_illegal_argument_str_2), arg);
}

/*
 * Handle ":syntax foldlevel" command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_foldlevel(exarg_T *eap, ಪೂರ್ಣಾಙ್ಕ syncing UNUSED)
{
    char_u *arg = eap->arg;
    char_u *arg_end;

    eap->nextcmd = find_nextcmd(arg);
    ಯದಿ (eap->skip)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (*arg == NUL)
    {
	ವಿಕಲ್ಪ (curwin->w_s->b_syn_foldlevel)
	{
	    ಪಕ್ಷ SYNFLD_START:   ಸಂದೇಶ("syntax foldlevel start");   ಭಙ್ಗ;
	    ಪಕ್ಷ SYNFLD_MINIMUM: ಸಂದೇಶ("syntax foldlevel minimum"); ಭಙ್ಗ;
	    ಸ್ವಭಾವ: ಭಙ್ಗ;
	}
	ಪ್ರತ್ಯಾಗಮ;
    }

    arg_end = skiptowhite(arg);
    ಯದಿ (STRNICMP(arg, "start", 5) == 0 && arg_end - arg == 5)
	curwin->w_s->b_syn_foldlevel = SYNFLD_START;
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(arg, "minimum", 7) == 0 && arg_end - arg == 7)
	curwin->w_s->b_syn_foldlevel = SYNFLD_MINIMUM;
    ಅನ್ಯಥಾ
    {
	semsg(_(e_illegal_argument_str_2), arg);
	ಪ್ರತ್ಯಾಗಮ;
    }

    arg = skipwhite(arg_end);
    ಯದಿ (*arg != NUL)
    {
	semsg(_(e_illegal_argument_str_2), arg);
    }
}

/*
 * Handle ":syntax spell" command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_spell(exarg_T *eap, ಪೂರ್ಣಾಙ್ಕ syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*next;

    eap->nextcmd = find_nextcmd(arg);
    ಯದಿ (eap->skip)
	ಪ್ರತ್ಯಾಗಮ;

    next = skiptowhite(arg);
    ಯದಿ (*arg == NUL)
    {
	ಯದಿ (curwin->w_s->b_syn_spell == SYNSPL_ತುದಿ)
	    ಸಂದೇಶ("syntax spell toplevel");
	ಅನ್ಯಥಾ ಯದಿ (curwin->w_s->b_syn_spell == SYNSPL_NOTOP)
	    ಸಂದೇಶ("syntax spell notoplevel");
	ಅನ್ಯಥಾ
	    ಸಂದೇಶ("syntax spell default");
    }
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(arg, "toplevel", 8) == 0 && next - arg == 8)
	curwin->w_s->b_syn_spell = SYNSPL_ತುದಿ;
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(arg, "notoplevel", 10) == 0 && next - arg == 10)
	curwin->w_s->b_syn_spell = SYNSPL_NOTOP;
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(arg, "default", 7) == 0 && next - arg == 7)
	curwin->w_s->b_syn_spell = SYNSPL_DEFAULT;
    ಅನ್ಯಥಾ
    {
	semsg(_(e_illegal_argument_str_2), arg);
	ಪ್ರತ್ಯಾಗಮ;
    }

    // assume spell checking changed, force a redraw
    redraw_win_later(curwin, UPD_NOT_VALID);
}

/*
 * Handle ":syntax iskeyword" command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_iskeyword(exarg_T *eap, ಪೂರ್ಣಾಙ್ಕ syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	save_chartab[32];
    char_u	*save_isk;

    ಯದಿ (eap->skip)
	ಪ್ರತ್ಯಾಗಮ;

    arg = skipwhite(arg);
    ಯದಿ (*arg == NUL)
    {
	ಸಂದೇಶ_puts("\n");
	ಯದಿ (curwin->w_s->b_syn_isk != empty_option)
	{
	    ಸಂದೇಶ_puts("syntax iskeyword ");
	    ಸಂದೇಶ_outtrans(curwin->w_s->b_syn_isk);
	}
	ಅನ್ಯಥಾ
	    ಸಂದೇಶ_outtrans((char_u *)_("syntax iskeyword not set"));
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (STRNICMP(arg, "clear", 5) == 0)
	{
	    mch_memmove(curwin->w_s->b_syn_chartab, curbuf->b_chartab,
								  (size_t)32);
	    clear_string_option(&curwin->w_s->b_syn_isk);
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (check_isopt(arg) == FAIL)
	    {
		emsg(_(e_invalid_argument));
		ಪ್ರತ್ಯಾಗಮ;
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
syntax_clear(synblock_T *block)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    block->b_syn_error = FALSE;	    // clear previous error
#ifdef FEAT_RELTIME
    block->b_syn_slow = FALSE;	    // clear previous timeout
#endif
    block->b_syn_ic = FALSE;	    // Use case, by default
    block->b_syn_foldlevel = SYNFLD_START;
    block->b_syn_spell = SYNSPL_DEFAULT; // default spell checking
    block->b_syn_containedin = FALSE;
#ifdef FEAT_CONCEAL
    block->b_syn_conceal = FALSE;
#endif

    // free the keywords
    clear_keywtab(&block->b_keywtab);
    clear_keywtab(&block->b_keywtab_ic);

    // free the syntax patterns
    ಪ್ರತಿ (i = block->b_syn_patterns.ga_len; --i >= 0; )
	syn_clear_pattern(block, i);
    ga_clear(&block->b_syn_patterns);

    // free the syntax clusters
    ಪ್ರತಿ (i = block->b_syn_clusters.ga_len; --i >= 0; )
	syn_clear_cluster(block, i);
    ga_clear(&block->b_syn_clusters);
    block->b_spell_cluster_id = 0;
    block->b_nospell_cluster_id = 0;

    block->b_syn_sync_flags = 0;
    block->b_syn_sync_minlines = 0;
    block->b_syn_sync_maxlines = 0;
    block->b_syn_sync_linebreaks = 0;

    vim_regfree(block->b_syn_linecont_prog);
    block->b_syn_linecont_prog = ರಿಕ್ತ;
    VIM_CLEAR(block->b_syn_linecont_pat);
#ifdef FEAT_FOLDING
    block->b_syn_folditems = 0;
#endif
    clear_string_option(&block->b_syn_isk);

    // free the stored states
    syn_ರಾಶಿ_free_all(block);
    invalidate_current_state();

    // Reset the counter for ":syn include"
    running_syn_inc_tag = 0;
}

/*
 * Get rid of ownsyntax for window "wp".
 */
    ಶೂನ್ಯ
reset_synblock(win_T *wp)
{
    ಯದಿ (wp->w_s != &wp->w_buffer->b_s)
    {
	syntax_clear(wp->w_s);
	vim_free(wp->w_s);
	wp->w_s = &wp->w_buffer->b_s;
    }
}

/*
 * Clear syncing info for one buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syntax_sync_clear(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    // free the syntax patterns
    ಪ್ರತಿ (i = curwin->w_s->b_syn_patterns.ga_len; --i >= 0; )
	ಯದಿ (SYN_ವಸ್ತುಗಳು(curwin->w_s)[i].sp_syncing)
	    syn_remove_pattern(curwin->w_s, i);

    curwin->w_s->b_syn_sync_flags = 0;
    curwin->w_s->b_syn_sync_minlines = 0;
    curwin->w_s->b_syn_sync_maxlines = 0;
    curwin->w_s->b_syn_sync_linebreaks = 0;

    vim_regfree(curwin->w_s->b_syn_linecont_prog);
    curwin->w_s->b_syn_linecont_prog = ರಿಕ್ತ;
    VIM_CLEAR(curwin->w_s->b_syn_linecont_pat);
    clear_string_option(&curwin->w_s->b_syn_isk);

    syn_ರಾಶಿ_free_all(curwin->w_s);	// Need to recompute all syntax.
}

/*
 * Remove one pattern from the buffer's pattern list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_remove_pattern(
    synblock_T	*block,
    ಪೂರ್ಣಾಙ್ಕ		idx)
{
    synpat_T	*spp;

    spp = &(SYN_ವಸ್ತುಗಳು(block)[idx]);
#ifdef FEAT_FOLDING
    ಯದಿ (spp->sp_flags & HL_FOLD)
	--block->b_syn_folditems;
#endif
    syn_clear_pattern(block, idx);
    mch_memmove(spp, spp + 1,
		   ಪರಿಮಾಣ(synpat_T) * (block->b_syn_patterns.ga_len - idx - 1));
    --block->b_syn_patterns.ga_len;
}

/*
 * Clear and free one syntax pattern.  When clearing all, must be called from
 * last to first!
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_clear_pattern(synblock_T *block, ಪೂರ್ಣಾಙ್ಕ i)
{
    vim_free(SYN_ವಸ್ತುಗಳು(block)[i].sp_pattern);
    vim_regfree(SYN_ವಸ್ತುಗಳು(block)[i].sp_prog);
    // Only free sp_cont_list and sp_next_list of first start pattern
    ಯದಿ (i == 0 || SYN_ವಸ್ತುಗಳು(block)[i - 1].sp_type != SPTYPE_START)
    {
	vim_free(SYN_ವಸ್ತುಗಳು(block)[i].sp_cont_list);
	vim_free(SYN_ವಸ್ತುಗಳು(block)[i].sp_next_list);
	vim_free(SYN_ವಸ್ತುಗಳು(block)[i].sp_syn.cont_in_list);
    }
}

/*
 * Clear and free one syntax cluster.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_clear_cluster(synblock_T *block, ಪೂರ್ಣಾಙ್ಕ i)
{
    vim_free(SYN_CLSTR(block)[i].scl_name);
    vim_free(SYN_CLSTR(block)[i].scl_name_u);
    vim_free(SYN_CLSTR(block)[i].scl_list);
}

/*
 * Handle ":syntax clear" command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_clear(exarg_T *eap, ಪೂರ್ಣಾಙ್ಕ syncing)
{
    char_u	*arg = eap->arg;
    char_u	*arg_end;
    ಪೂರ್ಣಾಙ್ಕ		id;

    eap->nextcmd = find_nextcmd(arg);
    ಯದಿ (eap->skip)
	ಪ್ರತ್ಯಾಗಮ;

    /*
     * We have to disable this within ":syn include @group filename",
     * because otherwise @group would get deleted.
     * Only required for Vim 5.x syntax files, 6.0 ones don't contain ":syn
     * clear".
     */
    ಯದಿ (curwin->w_s->b_syn_topgrp != 0)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (ends_excmd2(eap->cmd, arg))
    {
	/*
	 * No argument: Clear all syntax items.
	 */
	ಯದಿ (syncing)
	    syntax_sync_clear();
	ಅನ್ಯಥಾ
	{
	    syntax_clear(curwin->w_s);
	    ಯದಿ (curwin->w_s == &curwin->w_buffer->b_s)
		do_unlet((char_u *)"b:current_syntax", TRUE);
	    do_unlet((char_u *)"w:current_syntax", TRUE);
	}
    }
    ಅನ್ಯಥಾ
    {
	/*
	 * Clear the group IDs that are in the argument.
	 */
	ಯಾವತ್ (!ends_excmd2(eap->cmd, arg))
	{
	    arg_end = skiptowhite(arg);
	    ಯದಿ (*arg == '@')
	    {
		id = syn_scl_namen2id(arg + 1, (ಪೂರ್ಣಾಙ್ಕ)(arg_end - arg - 1));
		ಯದಿ (id == 0)
		{
		    semsg(_(e_no_such_syntax_cluster_str_1), arg);
		    ಭಙ್ಗ;
		}
		ಅನ್ಯಥಾ
		{
		    /*
		     * We can't physically delete a cluster without changing
		     * the IDs of other clusters, so we do the next best thing
		     * and make it empty.
		     */
		    short scl_id = id - SYNID_CLUSTER;

		    VIM_CLEAR(SYN_CLSTR(curwin->w_s)[scl_id].scl_list);
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		id = syn_namen2id(arg, (ಪೂರ್ಣಾಙ್ಕ)(arg_end - arg));
		ಯದಿ (id == 0)
		{
		    semsg(_(e_no_such_highlight_group_name_str), arg);
		    ಭಙ್ಗ;
		}
		ಅನ್ಯಥಾ
		    syn_clear_one(id, syncing);
	    }
	    arg = skipwhite(arg_end);
	}
    }
    redraw_curbuf_later(UPD_SOME_VALID);
    syn_ರಾಶಿ_free_all(curwin->w_s);		// Need to recompute all syntax.
}

/*
 * Clear one syntax group for the current buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_clear_one(ಪೂರ್ಣಾಙ್ಕ id, ಪೂರ್ಣಾಙ್ಕ syncing)
{
    synpat_T	*spp;
    ಪೂರ್ಣಾಙ್ಕ		idx;

    // Clear keywords only when not ":syn sync clear group-name"
    ಯದಿ (!syncing)
    {
	(ಶೂನ್ಯ)syn_clear_keyword(id, &curwin->w_s->b_keywtab);
	(ಶೂನ್ಯ)syn_clear_keyword(id, &curwin->w_s->b_keywtab_ic);
    }

    // clear the patterns for "id"
    ಪ್ರತಿ (idx = curwin->w_s->b_syn_patterns.ga_len; --idx >= 0; )
    {
	spp = &(SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx]);
	ಯದಿ (spp->sp_syn.id != id || spp->sp_syncing != syncing)
	    ಅನುವರ್ತ;
	syn_remove_pattern(curwin->w_s, idx);
    }
}

/*
 * Handle ":syntax on" command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_on(exarg_T *eap, ಪೂರ್ಣಾಙ್ಕ syncing UNUSED)
{
    syn_cmd_onoff(eap, "syntax");
}

/*
 * Handle ":syntax enable" command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_enable(exarg_T *eap, ಪೂರ್ಣಾಙ್ಕ syncing UNUSED)
{
    set_internal_string_var((char_u *)"g:syntax_cmd", (char_u *)"enable");
    syn_cmd_onoff(eap, "syntax");
    do_unlet((char_u *)"g:syntax_cmd", TRUE);
}

/*
 * Handle ":syntax reset" command.
 * It actually resets highlighting, not syntax.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_reset(exarg_T *eap, ಪೂರ್ಣಾಙ್ಕ syncing UNUSED)
{
    set_nextcmd(eap, eap->arg);
    ಯದಿ (!eap->skip)
    {
	set_internal_string_var((char_u *)"g:syntax_cmd", (char_u *)"reset");
	do_cmdline_cmd((char_u *)"runtime! syntax/syncolor.vim");
	do_unlet((char_u *)"g:syntax_cmd", TRUE);
    }
}

/*
 * Handle ":syntax manual" command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_manual(exarg_T *eap, ಪೂರ್ಣಾಙ್ಕ syncing UNUSED)
{
    syn_cmd_onoff(eap, "manual");
}

/*
 * Handle ":syntax off" command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_off(exarg_T *eap, ಪೂರ್ಣಾಙ್ಕ syncing UNUSED)
{
    syn_cmd_onoff(eap, "nosyntax");
}

    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_onoff(exarg_T *eap, ವರ್ಣ *name)
{
    char_u	buf[100];

    set_nextcmd(eap, eap->arg);
    ಯದಿ (!eap->skip)
    {
	STRCPY(buf, "so ");
	vim_snprintf((ವರ್ಣ *)buf + 3, ಪರಿಮಾಣ(buf) - 3, SYNTAX_FNAME, name);
	do_cmdline_cmd(buf);
    }
}

/*
 * Handle ":syntax [list]" command: list current syntax words.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_list(
    exarg_T	*eap,
    ಪೂರ್ಣಾಙ್ಕ		syncing)	    // when TRUE: list syncing items
{
    char_u	*arg = eap->arg;
    ಪೂರ್ಣಾಙ್ಕ		id;
    char_u	*arg_end;

    eap->nextcmd = find_nextcmd(arg);
    ಯದಿ (eap->skip)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (!syntax_present(curwin))
    {
	ಸಂದೇಶ(_(ಸಂದೇಶ_no_ವಸ್ತುಗಳು));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (syncing)
    {
	ಯದಿ (curwin->w_s->b_syn_sync_flags & SF_CCOMMENT)
	{
	    ಸಂದೇಶ_puts(_("syncing on C-style comments"));
	    syn_ಸಾಲುಗಳು_ಸಂದೇಶ();
	    syn_match_ಸಂದೇಶ();
	}
	ಅನ್ಯಥಾ ಯದಿ (curwin->w_s->b_syn_sync_flags & SF_MATCH)
	{
	    ಸಂದೇಶ_puts_title(_("\n--- Syntax sync items ---"));
	    ಯದಿ (curwin->w_s->b_syn_sync_minlines > 0
		|| curwin->w_s->b_syn_sync_maxlines > 0
		|| curwin->w_s->b_syn_sync_linebreaks > 0)
	    {
		ಸಂದೇಶ_puts(_("\nsyncing on items"));
		syn_ಸಾಲುಗಳು_ಸಂದೇಶ();
		syn_match_ಸಂದೇಶ();
	    }
	    ಪ್ರತಿ (id = 1; id <= highlight_ಅಂಕೆ_groups() && !got_int; ++id)
		syn_list_one(id, syncing, FALSE);
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (curwin->w_s->b_syn_sync_minlines == 0)
		ಸಂದೇಶ_puts(_("no syncing"));
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (curwin->w_s->b_syn_sync_minlines == MAXLNUM)
		    ಸಂದೇಶ_puts(_("syncing starts at the first line"));
		ಅನ್ಯಥಾ
		{
		    ಸಂದೇಶ_puts(_("syncing starts "));
		    ಸಂದೇಶ_outnum(curwin->w_s->b_syn_sync_minlines);
		    ಸಂದೇಶ_puts(_(" lines before top line"));
		}
		syn_match_ಸಂದೇಶ();
	    }
	}
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಸಂದೇಶ_puts_title(_("\n--- Syntax items ---"));
    ಯದಿ (ends_excmd2(eap->cmd, arg))
    {
	/*
	 * No argument: List all group IDs and all syntax clusters.
	 */
	ಪ್ರತಿ (id = 1; id <= highlight_ಅಂಕೆ_groups() && !got_int; ++id)
	    syn_list_one(id, syncing, FALSE);
	ಪ್ರತಿ (id = 0; id < curwin->w_s->b_syn_clusters.ga_len && !got_int; ++id)
	    syn_list_cluster(id);
    }
    ಅನ್ಯಥಾ
    {
	/*
	 * List the group IDs and syntax clusters that are in the argument.
	 */
	ಯಾವತ್ (!ends_excmd2(eap->cmd, arg) && !got_int)
	{
	    arg_end = skiptowhite(arg);
	    ಯದಿ (*arg == '@')
	    {
		id = syn_scl_namen2id(arg + 1, (ಪೂರ್ಣಾಙ್ಕ)(arg_end - arg - 1));
		ಯದಿ (id == 0)
		    semsg(_(e_no_such_syntax_cluster_str_2), arg);
		ಅನ್ಯಥಾ
		    syn_list_cluster(id - SYNID_CLUSTER);
	    }
	    ಅನ್ಯಥಾ
	    {
		id = syn_namen2id(arg, (ಪೂರ್ಣಾಙ್ಕ)(arg_end - arg));
		ಯದಿ (id == 0)
		    semsg(_(e_no_such_highlight_group_name_str), arg);
		ಅನ್ಯಥಾ
		    syn_list_one(id, syncing, TRUE);
	    }
	    arg = skipwhite(arg_end);
	}
    }
    set_nextcmd(eap, arg);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
syn_ಸಾಲುಗಳು_ಸಂದೇಶ(ಶೂನ್ಯ)
{
    ಯದಿ (curwin->w_s->b_syn_sync_maxlines > 0
				      || curwin->w_s->b_syn_sync_minlines > 0)
    {
	ಸಂದೇಶ_puts("; ");
	ಯದಿ (curwin->w_s->b_syn_sync_minlines == MAXLNUM)
	    ಸಂದೇಶ_puts(_("from the first line"));
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (curwin->w_s->b_syn_sync_minlines > 0)
	    {
		ಸಂದೇಶ_puts(_("minimal "));
		ಸಂದೇಶ_outnum(curwin->w_s->b_syn_sync_minlines);
		ಯದಿ (curwin->w_s->b_syn_sync_maxlines)
		    ಸಂದೇಶ_puts(", ");
	    }
	    ಯದಿ (curwin->w_s->b_syn_sync_maxlines > 0)
	    {
		ಸಂದೇಶ_puts(_("maximal "));
		ಸಂದೇಶ_outnum(curwin->w_s->b_syn_sync_maxlines);
	    }
	    ಸಂದೇಶ_puts(_(" lines before top line"));
	}
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
syn_match_ಸಂದೇಶ(ಶೂನ್ಯ)
{
    ಯದಿ (curwin->w_s->b_syn_sync_linebreaks > 0)
    {
	ಸಂದೇಶ_puts(_("; match "));
	ಸಂದೇಶ_outnum(curwin->w_s->b_syn_sync_linebreaks);
	ಸಂದೇಶ_puts(_(" line breaks"));
    }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  last_matchgroup;

ಸ್ಥಿರ ಶೂನ್ಯ syn_list_flags(keyvalue_T *nlist, ಪೂರ್ಣಾಙ್ಕ nr_entries, ಪೂರ್ಣಾಙ್ಕ flags, ಪೂರ್ಣಾಙ್ಕ attr);

/*
 * List one syntax item, for ":syntax" or "syntax list syntax_name".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_list_one(
    ಪೂರ್ಣಾಙ್ಕ		id,
    ಪೂರ್ಣಾಙ್ಕ		syncing,	    // when TRUE: list syncing items
    ಪೂರ್ಣಾಙ್ಕ		link_only)	    // when TRUE; list link-only too
{
    ಪೂರ್ಣಾಙ್ಕ		attr;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    ಪೂರ್ಣಾಙ್ಕ		did_header = FALSE;
    synpat_T	*spp;
    ಸ್ಥಿರ keyvalue_T namelist1[] =
		{
		    KEYVALUE_ENTRY(HL_DISPLAY, "display"),
		    KEYVALUE_ENTRY(HL_CONTAINED, "contained"),
		    KEYVALUE_ENTRY(HL_ONELINE, "oneline"),
		    KEYVALUE_ENTRY(HL_KEEPEND, "keepend"),
		    KEYVALUE_ENTRY(HL_EXTEND, "extend"),
		    KEYVALUE_ENTRY(HL_EXCLUDENL, "excludenl"),
		    KEYVALUE_ENTRY(HL_TRANSP, "transparent"),
		    KEYVALUE_ENTRY(HL_FOLD, "fold")
#ifdef FEAT_CONCEAL
		    ,
		    KEYVALUE_ENTRY(HL_CONCEAL, "conceal"),
		    KEYVALUE_ENTRY(HL_CONCEALENDS, "concealends")
#endif
		};
    ಸ್ಥಿರ keyvalue_T namelist2[] =
		{
		    KEYVALUE_ENTRY(HL_SKIPWHITE, "skipwhite"),
		    KEYVALUE_ENTRY(HL_SKIPNL, "skipnl"),
		    KEYVALUE_ENTRY(HL_SKIPEMPTY, "skipempty")
		};

    attr = HL_ATTR(HLF_D);		// highlight like directories

    // list the keywords for "id"
    ಯದಿ (!syncing)
    {
	did_header = syn_list_keywords(id, &curwin->w_s->b_keywtab, FALSE, attr);
	did_header = syn_list_keywords(id, &curwin->w_s->b_keywtab_ic,
							    did_header, attr);
    }

    // list the patterns for "id"
    ಪ್ರತಿ (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len && !got_int; ++idx)
    {
	spp = &(SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx]);
	ಯದಿ (spp->sp_syn.id != id || spp->sp_syncing != syncing)
	    ಅನುವರ್ತ;

	(ಶೂನ್ಯ)syn_list_header(did_header, 999, id);
	did_header = TRUE;
	last_matchgroup = 0;
	ಯದಿ (spp->sp_type == SPTYPE_MATCH)
	{
	    put_pattern("match", ' ', spp, attr);
	    ಸಂದೇಶ_putchar(' ');
	}
	ಅನ್ಯಥಾ ಯದಿ (spp->sp_type == SPTYPE_START)
	{
	    ಯಾವತ್ (SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_type == SPTYPE_START)
		put_pattern("start", '=', &SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx++], attr);
	    ಯದಿ (SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_type == SPTYPE_SKIP)
		put_pattern("skip", '=', &SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx++], attr);
	    ಯಾವತ್ (idx < curwin->w_s->b_syn_patterns.ga_len
			  && SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_type == SPTYPE_END)
		put_pattern("end", '=', &SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx++], attr);
	    --idx;
	    ಸಂದೇಶ_putchar(' ');
	}
	syn_list_flags(namelist1, (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(namelist1), spp->sp_flags, attr);

	ಯದಿ (spp->sp_cont_list != ರಿಕ್ತ)
	    put_id_list((char_u *)"contains", spp->sp_cont_list, attr);

	ಯದಿ (spp->sp_syn.cont_in_list != ರಿಕ್ತ)
	    put_id_list((char_u *)"containedin",
					      spp->sp_syn.cont_in_list, attr);

	ಯದಿ (spp->sp_next_list != ರಿಕ್ತ)
	{
	    put_id_list((char_u *)"nextgroup", spp->sp_next_list, attr);
	    syn_list_flags(namelist2, (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(namelist2), spp->sp_flags, attr);
	}
	ಯದಿ (spp->sp_flags & (HL_SYNC_HERE|HL_SYNC_THERE))
	{
	    ಯದಿ (spp->sp_flags & HL_SYNC_HERE)
		ಸಂದೇಶ_puts_attr("grouphere", attr);
	    ಅನ್ಯಥಾ
		ಸಂದೇಶ_puts_attr("groupthere", attr);
	    ಸಂದೇಶ_putchar(' ');
	    ಯದಿ (spp->sp_sync_idx >= 0)
		ಸಂದೇಶ_outtrans(highlight_group_name(SYN_ವಸ್ತುಗಳು(curwin->w_s)
				   [spp->sp_sync_idx].sp_syn.id - 1));
	    ಅನ್ಯಥಾ
		ಸಂದೇಶ_puts("NONE");
	    ಸಂದೇಶ_putchar(' ');
	}
    }

    // list the link, if there is one
    ಯದಿ (highlight_link_id(id - 1) && (did_header || link_only) && !got_int)
    {
	(ಶೂನ್ಯ)syn_list_header(did_header, 999, id);
	ಸಂದೇಶ_puts_attr("links to", attr);
	ಸಂದೇಶ_putchar(' ');
	ಸಂದೇಶ_outtrans(highlight_group_name(highlight_link_id(id - 1) - 1));
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
syn_list_flags(keyvalue_T *nlist, ಪೂರ್ಣಾಙ್ಕ nr_entries, ಪೂರ್ಣಾಙ್ಕ flags, ಪೂರ್ಣಾಙ್ಕ attr)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = 0; i < nr_entries; ++i)
	ಯದಿ (flags & nlist[i].ಕೀಲಿ)
	{
	    ಸಂದೇಶ_puts_attr((ವರ್ಣ *)nlist[i].ಮೌಲ್ಯ.string, attr);
	    ಸಂದೇಶ_putchar(' ');
	}
}

/*
 * List one syntax cluster, for ":syntax" or "syntax list syntax_name".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_list_cluster(ಪೂರ್ಣಾಙ್ಕ id)
{
    ಪೂರ್ಣಾಙ್ಕ	    endcol = 15;

    // slight hack:  roughly duplicate the guts of syn_list_header()
    ಸಂದೇಶ_putchar('\n');
    ಸಂದೇಶ_outtrans(SYN_CLSTR(curwin->w_s)[id].scl_name);

    ಯದಿ (ಸಂದೇಶ_col >= endcol)	// output at least one space
	endcol = ಸಂದೇಶ_col + 1;
    ಯದಿ (cmdline_width <= endcol)	// avoid hang for tiny window
	endcol = cmdline_width - 1;

    ಸಂದೇಶ_advance(endcol);
    ಯದಿ (SYN_CLSTR(curwin->w_s)[id].scl_list != ರಿಕ್ತ)
    {
	put_id_list((char_u *)"cluster", SYN_CLSTR(curwin->w_s)[id].scl_list,
		    HL_ATTR(HLF_D));
    }
    ಅನ್ಯಥಾ
    {
	ಸಂದೇಶ_puts_attr("cluster", HL_ATTR(HLF_D));
	ಸಂದೇಶ_puts("=NONE");
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
put_id_list(char_u *name, short *list, ಪೂರ್ಣಾಙ್ಕ attr)
{
    short		*p;

    ಸಂದೇಶ_puts_attr((ವರ್ಣ *)name, attr);
    ಸಂದೇಶ_putchar('=');
    ಪ್ರತಿ (p = list; *p; ++p)
    {
	ಯದಿ (*p >= SYNID_ALLBUT && *p < SYNID_ತುದಿ)
	{
	    ಯದಿ (p[1])
		ಸಂದೇಶ_puts("ALLBUT");
	    ಅನ್ಯಥಾ
		ಸಂದೇಶ_puts("ALL");
	}
	ಅನ್ಯಥಾ ಯದಿ (*p >= SYNID_ತುದಿ && *p < SYNID_CONTAINED)
	{
	    ಸಂದೇಶ_puts("TOP");
	}
	ಅನ್ಯಥಾ ಯದಿ (*p >= SYNID_CONTAINED && *p < SYNID_CLUSTER)
	{
	    ಸಂದೇಶ_puts("CONTAINED");
	}
	ಅನ್ಯಥಾ ಯದಿ (*p >= SYNID_CLUSTER)
	{
	    short scl_id = *p - SYNID_CLUSTER;

	    ಸಂದೇಶ_putchar('@');
	    ಸಂದೇಶ_outtrans(SYN_CLSTR(curwin->w_s)[scl_id].scl_name);
	}
	ಅನ್ಯಥಾ
	    ಸಂದೇಶ_outtrans(highlight_group_name(*p - 1));
	ಯದಿ (p[1])
	    ಸಂದೇಶ_putchar(',');
    }
    ಸಂದೇಶ_putchar(' ');
}

    ಸ್ಥಿರ ಶೂನ್ಯ
put_pattern(
    ವರ್ಣ	*s,
    ಪೂರ್ಣಾಙ್ಕ		c,
    synpat_T	*spp,
    ಪೂರ್ಣಾಙ್ಕ		attr)
{
    long	n;
    ಪೂರ್ಣಾಙ್ಕ		mask;
    ಪೂರ್ಣಾಙ್ಕ		first;
    ಸ್ಥಿರ ವರ್ಣ	*sepchars = "/+=-#@\"|'^&";
    ಪೂರ್ಣಾಙ್ಕ		i;

    // May have to write "matchgroup=group"
    ಯದಿ (last_matchgroup != spp->sp_syn_match_id)
    {
	last_matchgroup = spp->sp_syn_match_id;
	ಸಂದೇಶ_puts_attr("matchgroup", attr);
	ಸಂದೇಶ_putchar('=');
	ಯದಿ (last_matchgroup == 0)
	    ಸಂದೇಶ_outtrans((char_u *)"NONE");
	ಅನ್ಯಥಾ
	    ಸಂದೇಶ_outtrans(highlight_group_name(last_matchgroup - 1));
	ಸಂದೇಶ_putchar(' ');
    }

    // output the name of the pattern and an '=' or ' '
    ಸಂದೇಶ_puts_attr(s, attr);
    ಸಂದೇಶ_putchar(c);

    // output the pattern, in between a char that is not in the pattern
    ಪ್ರತಿ (i = 0; vim_strchr(spp->sp_pattern, sepchars[i]) != ರಿಕ್ತ; )
	ಯದಿ (sepchars[++i] == NUL)
	{
	    i = 0;	// no good char found, just use the first one
	    ಭಙ್ಗ;
	}
    ಸಂದೇಶ_putchar(sepchars[i]);
    ಸಂದೇಶ_outtrans(spp->sp_pattern);
    ಸಂದೇಶ_putchar(sepchars[i]);

    // output any pattern options
    first = TRUE;
    ಪ್ರತಿ (i = 0; i < SPO_ಎಣಿಕೆ; ++i)
    {
	mask = (1 << i);
	ಯದಿ (spp->sp_off_flags & (mask + (mask << SPO_ಎಣಿಕೆ)))
	{
	    ಯದಿ (!first)
		ಸಂದೇಶ_putchar(',');	// separate with commas
	    ಸಂದೇಶ_puts(spo_name_tab[i]);
	    n = spp->sp_offsets[i];
	    ಯದಿ (i != SPO_LC_OFF)
	    {
		ಯದಿ (spp->sp_off_flags & mask)
		    ಸಂದೇಶ_putchar('s');
		ಅನ್ಯಥಾ
		    ಸಂದೇಶ_putchar('e');
		ಯದಿ (n > 0)
		    ಸಂದೇಶ_putchar('+');
	    }
	    ಯದಿ (n || i == SPO_LC_OFF)
		ಸಂದೇಶ_outnum(n);
	    first = FALSE;
	}
    }
    ಸಂದೇಶ_putchar(' ');
}

/*
 * List or clear the keywords for one syntax group.
 * Return TRUE if the header has been printed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
syn_list_keywords(
    ಪೂರ್ಣಾಙ್ಕ		id,
    hashtab_T	*ht,
    ಪೂರ್ಣಾಙ್ಕ		did_header,		// header has already been printed
    ಪೂರ್ಣಾಙ್ಕ		attr)
{
    ಪೂರ್ಣಾಙ್ಕ		outlen;
    hashitem_T	*hi;
    keyentry_T	*kp;
    ಪೂರ್ಣಾಙ್ಕ		todo;
    ಪೂರ್ಣಾಙ್ಕ		prev_contained = 0;
    short	*prev_next_list = ರಿಕ್ತ;
    short	*prev_cont_in_list = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		prev_skipnl = 0;
    ಪೂರ್ಣಾಙ್ಕ		prev_skipwhite = 0;
    ಪೂರ್ಣಾಙ್ಕ		prev_skipempty = 0;

    /*
     * Unfortunately, this list of keywords is not sorted on alphabet but on
     * hash value...
     */
    todo = (ಪೂರ್ಣಾಙ್ಕ)ht->ht_used;
    ಪ್ರತಿ (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	ಯದಿ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ಪ್ರತಿ (kp = HI2KE(hi); kp != ರಿಕ್ತ && !got_int; kp = kp->ke_next)
	    {
		ಯದಿ (kp->k_syn.id == id)
		{
		    ಯದಿ (prev_contained != (kp->flags & HL_CONTAINED)
			    || prev_skipnl != (kp->flags & HL_SKIPNL)
			    || prev_skipwhite != (kp->flags & HL_SKIPWHITE)
			    || prev_skipempty != (kp->flags & HL_SKIPEMPTY)
			    || prev_cont_in_list != kp->k_syn.cont_in_list
			    || prev_next_list != kp->next_list)
			outlen = 9999;
		    ಅನ್ಯಥಾ
			outlen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(kp->keyword);
		    // output "contained" and "nextgroup" on each line
		    ಯದಿ (syn_list_header(did_header, outlen, id))
		    {
			prev_contained = 0;
			prev_next_list = ರಿಕ್ತ;
			prev_cont_in_list = ರಿಕ್ತ;
			prev_skipnl = 0;
			prev_skipwhite = 0;
			prev_skipempty = 0;
		    }
		    did_header = TRUE;
		    ಯದಿ (prev_contained != (kp->flags & HL_CONTAINED))
		    {
			ಸಂದೇಶ_puts_attr("contained", attr);
			ಸಂದೇಶ_putchar(' ');
			prev_contained = (kp->flags & HL_CONTAINED);
		    }
		    ಯದಿ (kp->k_syn.cont_in_list != prev_cont_in_list)
		    {
			put_id_list((char_u *)"containedin",
						kp->k_syn.cont_in_list, attr);
			ಸಂದೇಶ_putchar(' ');
			prev_cont_in_list = kp->k_syn.cont_in_list;
		    }
		    ಯದಿ (kp->next_list != prev_next_list)
		    {
			put_id_list((char_u *)"nextgroup", kp->next_list, attr);
			ಸಂದೇಶ_putchar(' ');
			prev_next_list = kp->next_list;
			ಯದಿ (kp->flags & HL_SKIPNL)
			{
			    ಸಂದೇಶ_puts_attr("skipnl", attr);
			    ಸಂದೇಶ_putchar(' ');
			    prev_skipnl = (kp->flags & HL_SKIPNL);
			}
			ಯದಿ (kp->flags & HL_SKIPWHITE)
			{
			    ಸಂದೇಶ_puts_attr("skipwhite", attr);
			    ಸಂದೇಶ_putchar(' ');
			    prev_skipwhite = (kp->flags & HL_SKIPWHITE);
			}
			ಯದಿ (kp->flags & HL_SKIPEMPTY)
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

    ಪ್ರತ್ಯಾಗಮ did_header;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
syn_clear_keyword(ಪೂರ್ಣಾಙ್ಕ id, hashtab_T *ht)
{
    hashitem_T	*hi;
    keyentry_T	*kp;
    keyentry_T	*kp_prev;
    keyentry_T	*kp_next;
    ಪೂರ್ಣಾಙ್ಕ		todo;

    hash_lock(ht);
    todo = (ಪೂರ್ಣಾಙ್ಕ)ht->ht_used;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(ht, hi, todo)
    {
	ಯದಿ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    kp_prev = ರಿಕ್ತ;
	    ಪ್ರತಿ (kp = HI2KE(hi); kp != ರಿಕ್ತ; )
	    {
		ಯದಿ (kp->k_syn.id == id)
		{
		    kp_next = kp->ke_next;
		    ಯದಿ (kp_prev == ರಿಕ್ತ)
		    {
			ಯದಿ (kp_next == ರಿಕ್ತ)
			    hash_remove(ht, hi, "syntax clear keyword");
			ಅನ್ಯಥಾ
			    hi->hi_ಕೀಲಿ = KE2HIKEY(kp_next);
		    }
		    ಅನ್ಯಥಾ
			kp_prev->ke_next = kp_next;
		    vim_free(kp->next_list);
		    vim_free(kp->k_syn.cont_in_list);
		    vim_free(kp);
		    kp = kp_next;
		}
		ಅನ್ಯಥಾ
		{
		    kp_prev = kp;
		    kp = kp->ke_next;
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
    ಪೂರ್ಣಾಙ್ಕ		todo;
    keyentry_T	*kp;
    keyentry_T	*kp_next;

    todo = (ಪೂರ್ಣಾಙ್ಕ)ht->ht_used;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(ht, hi, todo)
    {
	ಯದಿ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ಪ್ರತಿ (kp = HI2KE(hi); kp != ರಿಕ್ತ; kp = kp_next)
	    {
		kp_next = kp->ke_next;
		vim_free(kp->next_list);
		vim_free(kp->k_syn.cont_in_list);
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
add_keyword(
    char_u	*name,	    // name of keyword
    size_t	namelen,    // length of keyword (excluding the NUL)
    ಪೂರ್ಣಾಙ್ಕ		id,	    // group ID for this keyword
    ಪೂರ್ಣಾಙ್ಕ		flags,	    // flags for this keyword
    short	*cont_in_list, // containedin for this keyword
    short	*next_list, // nextgroup for this keyword
    ಪೂರ್ಣಾಙ್ಕ		conceal_char)
{
    keyentry_T	*kp;
    hashtab_T	*ht;
    hashitem_T	*hi;
    char_u	*name_ic;
    size_t	name_iclen;
    long_u	hash;
    char_u	name_folded[MAXKEYWLEN + 1];

    ಯದಿ (curwin->w_s->b_syn_ic)
    {
	name_ic = str_foldcase(name, (ಪೂರ್ಣಾಙ್ಕ)namelen,
						 name_folded, MAXKEYWLEN + 1);
	name_iclen = STRLEN(name_ic);
    }
    ಅನ್ಯಥಾ
    {
	name_ic = name;
	name_iclen = namelen;
    }
    kp = alloc(offsetof(keyentry_T, keyword) + name_iclen + 1);
    ಯದಿ (kp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    STRCPY(kp->keyword, name_ic);
    kp->k_syn.id = id;
    kp->k_syn.inc_tag = current_syn_inc_tag;
    kp->flags = flags;
    kp->k_char = conceal_char;
    kp->k_syn.cont_in_list = copy_id_list(cont_in_list);
    ಯದಿ (cont_in_list != ರಿಕ್ತ)
	curwin->w_s->b_syn_containedin = TRUE;
    kp->next_list = copy_id_list(next_list);

    ಯದಿ (curwin->w_s->b_syn_ic)
	ht = &curwin->w_s->b_keywtab_ic;
    ಅನ್ಯಥಾ
	ht = &curwin->w_s->b_keywtab;

    hash = hash_hash(kp->keyword);
    hi = hash_lookup(ht, kp->keyword, hash);
    ಯದಿ (HASHITEM_EMPTY(hi))
    {
	// new keyword, add to hashtable
	kp->ke_next = ರಿಕ್ತ;
	hash_add_item(ht, hi, kp->keyword, hash);
    }
    ಅನ್ಯಥಾ
    {
	// keyword already exists, prepend to list
	kp->ke_next = HI2KE(hi);
	hi->hi_ಕೀಲಿ = KE2HIKEY(kp);
    }
}

/*
 * Get the start and end of the group name argument.
 * Return a pointer to the first argument.
 * Return NULL if the end of the command was found instead of further args.
 */
    ಸ್ಥಿರ char_u *
get_group_name(
    char_u	*arg,		// start of the argument
    char_u	**name_end)	// pointer to end of the name
{
    char_u	*rest;

    *name_end = skiptowhite(arg);
    rest = skipwhite(*name_end);

    /*
     * Check if there are enough arguments.  The first argument may be a
     * pattern, where '|' is allowed, so only check for NUL.
     */
    ಯದಿ (ends_excmd(*arg) || *rest == NUL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ rest;
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
    char_u	    *start,		// next argument to be checked
    syn_opt_arg_T   *opt,		// various things
    ಪೂರ್ಣಾಙ್ಕ		    *conceal_char UNUSED,
    ಪೂರ್ಣಾಙ್ಕ		    skip)		// TRUE if skipping over command
{
    char_u	*arg = start;
    char_u	*gname_start, *gname;
    ಪೂರ್ಣಾಙ್ಕ		syn_id;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ವರ್ಣ	*p;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		fidx;
    ಸ್ಥಿರ ಸಂರಚನಾ flag
    {
	ವರ್ಣ	*name;
	ಪೂರ್ಣಾಙ್ಕ	argtype;
	ಪೂರ್ಣಾಙ್ಕ	flags;
    } flagtab[] = { {"cCoOnNtTaAiInNeEdD",	0,	HL_CONTAINED},
		    {"oOnNeElLiInNeE",		0,	HL_ONELINE},
		    {"kKeEeEpPeEnNdD",		0,	HL_KEEPEND},
		    {"eExXtTeEnNdD",		0,	HL_EXTEND},
		    {"eExXcClLuUdDeEnNlL",	0,	HL_EXCLUDENL},
		    {"tTrRaAnNsSpPaArReEnNtT",	0,	HL_TRANSP},
		    {"sSkKiIpPnNlL",		0,	HL_SKIPNL},
		    {"sSkKiIpPwWhHiItTeE",	0,	HL_SKIPWHITE},
		    {"sSkKiIpPeEmMpPtTyY",	0,	HL_SKIPEMPTY},
		    {"gGrRoOuUpPhHeErReE",	0,	HL_SYNC_HERE},
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
    ಸ್ಥಿರ ವರ್ಣ *first_letters = "cCoOkKeEtTsSgGdDfFnN";

    ಯದಿ (arg == ರಿಕ್ತ)		// already detected error
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

#ifdef FEAT_CONCEAL
    ಯದಿ (curwin->w_s->b_syn_conceal)
	opt->flags |= HL_CONCEAL;
#endif

    ಪ್ರತಿ (;;)
    {
	/*
	 * This is used very often when a large number of keywords is defined.
	 * Need to skip quickly when no option name is found.
	 * Also avoid tolower(), it's slow.
	 */
	ಯದಿ (strchr(first_letters, *arg) == ರಿಕ್ತ)
	    ಭಙ್ಗ;

	ಪ್ರತಿ (fidx = ARRAY_LENGTH(flagtab); --fidx >= 0; )
	{
	    p = flagtab[fidx].name;
	    ಪ್ರತಿ (i = 0, len = 0; p[i] != NUL; i += 2, ++len)
		ಯದಿ (arg[len] != p[i] && arg[len] != p[i + 1])
		    ಭಙ್ಗ;
	    ಯದಿ (p[i] == NUL && (VIM_ISWHITE(arg[len])
				    || (flagtab[fidx].argtype > 0
					 ? arg[len] == '='
					 : ends_excmd2(start, arg + len))))
	    {
		ಯದಿ (opt->keyword
			&& (flagtab[fidx].flags == HL_DISPLAY
			    || flagtab[fidx].flags == HL_FOLD
			    || flagtab[fidx].flags == HL_EXTEND))
		    // treat "display", "fold" and "extend" as a keyword
		    fidx = -1;
		ಭಙ್ಗ;
	    }
	}
	ಯದಿ (fidx < 0)	    // no match found
	    ಭಙ್ಗ;

	ಯದಿ (flagtab[fidx].argtype == 1)
	{
	    ಯದಿ (!opt->has_cont_list)
	    {
		emsg(_(e_contains_argument_not_accepted_here));
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	    ಯದಿ (get_id_list(&arg, 8, &opt->cont_list, skip) == FAIL)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ ಯದಿ (flagtab[fidx].argtype == 2)
	{
	    ಯದಿ (get_id_list(&arg, 11, &opt->cont_in_list, skip) == FAIL)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ ಯದಿ (flagtab[fidx].argtype == 3)
	{
	    ಯದಿ (get_id_list(&arg, 9, &opt->next_list, skip) == FAIL)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ ಯದಿ (flagtab[fidx].argtype == 11 && arg[5] == '=')
	{
	    // cchar=?
	    ಯದಿ (has_mbyte)
	    {
#ifdef FEAT_CONCEAL
		*conceal_char = mb_ptr2char(arg + 6);
#endif
		arg += mb_ptr2len(arg + 6) - 1;
	    }
	    ಅನ್ಯಥಾ
	    {
#ifdef FEAT_CONCEAL
		*conceal_char = arg[6];
#ಅನ್ಯಥಾ
		;
#endif
	    }
#ifdef FEAT_CONCEAL
	    ಯದಿ (!vim_isprintc_strict(*conceal_char))
	    {
		emsg(_(e_invalid_cchar_ಮೌಲ್ಯ));
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
#endif
	    arg = skipwhite(arg + 7);
	}
	ಅನ್ಯಥಾ
	{
	    opt->flags |= flagtab[fidx].flags;
	    arg = skipwhite(arg + len);

	    ಯದಿ (flagtab[fidx].flags == HL_SYNC_HERE
		    || flagtab[fidx].flags == HL_SYNC_THERE)
	    {
		ಯದಿ (opt->sync_idx == ರಿಕ್ತ)
		{
		    emsg(_(e_groupthere_not_accepted_here));
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		gname_start = arg;
		arg = skiptowhite(arg);
		ಯದಿ (gname_start == arg)
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		gname = vim_strnsave(gname_start, arg - gname_start);
		ಯದಿ (gname == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		ಯದಿ (STRCMP(gname, "NONE") == 0)
		    *opt->sync_idx = NONE_IDX;
		ಅನ್ಯಥಾ
		{
		    syn_id = syn_name2id(gname);
		    ಪ್ರತಿ (i = curwin->w_s->b_syn_patterns.ga_len; --i >= 0; )
			ಯದಿ (SYN_ವಸ್ತುಗಳು(curwin->w_s)[i].sp_syn.id == syn_id
			      && SYN_ವಸ್ತುಗಳು(curwin->w_s)[i].sp_type
							       == SPTYPE_START)
			{
			    *opt->sync_idx = i;
			    ಭಙ್ಗ;
			}
		    ಯದಿ (i < 0)
		    {
			semsg(_(e_didnt_find_region_item_for_str), gname);
			vim_free(gname);
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		    }
		}

		vim_free(gname);
		arg = skipwhite(arg);
	    }
#ifdef FEAT_FOLDING
	    ಅನ್ಯಥಾ ಯದಿ (flagtab[fidx].flags == HL_FOLD
						&& foldmethodIsSyntax(curwin))
		// Need to update folds later.
		foldUpdateAll(curwin);
#endif
	}
    }

    ಪ್ರತ್ಯಾಗಮ arg;
}

/*
 * Adjustments to syntax item when declared in a ":syn include"'d file.
 * Set the contained flag, and if the item is not already contained, add it
 * to the specified top-level group, if any.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_incl_toplevel(ಪೂರ್ಣಾಙ್ಕ id, ಪೂರ್ಣಾಙ್ಕ *flagsp)
{
    ಯದಿ ((*flagsp & HL_CONTAINED) || curwin->w_s->b_syn_topgrp == 0)
	ಪ್ರತ್ಯಾಗಮ;
    *flagsp |= HL_CONTAINED | HL_INCLUDED_TOPLEVEL;
    ಯದಿ (curwin->w_s->b_syn_topgrp >= SYNID_CLUSTER)
    {
	// We have to alloc this, because syn_combine_list() will free it.
	short	    *grp_list = ALLOC_MULT(short, 2);
	ಪೂರ್ಣಾಙ್ಕ	    tlg_id = curwin->w_s->b_syn_topgrp - SYNID_CLUSTER;

	ಯದಿ (grp_list != ರಿಕ್ತ)
	{
	    grp_list[0] = id;
	    grp_list[1] = 0;
	    syn_combine_list(&SYN_CLSTR(curwin->w_s)[tlg_id].scl_list,
						       &grp_list, CLUSTER_ADD);
	}
    }
}

/*
 * Handle ":syntax include [@{group-name}] filename" command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_include(exarg_T *eap, ಪೂರ್ಣಾಙ್ಕ syncing UNUSED)
{
    char_u	*arg = eap->arg;
    ಪೂರ್ಣಾಙ್ಕ		sgl_id = 1;
    char_u	*group_name_end;
    char_u	*rest;
    ವರ್ಣ	*errormsg = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		prev_toplvl_grp;
    ಪೂರ್ಣಾಙ್ಕ		prev_syn_inc_tag;
    ಪೂರ್ಣಾಙ್ಕ		source = FALSE;

    eap->nextcmd = find_nextcmd(arg);
    ಯದಿ (eap->skip)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (arg[0] == '@')
    {
	++arg;
	rest = get_group_name(arg, &group_name_end);
	ಯದಿ (rest == ರಿಕ್ತ)
	{
	    emsg(_(e_filename_required));
	    ಪ್ರತ್ಯಾಗಮ;
	}
	sgl_id = syn_check_cluster(arg, (ಪೂರ್ಣಾಙ್ಕ)(group_name_end - arg));
	ಯದಿ (sgl_id == 0)
	    ಪ್ರತ್ಯಾಗಮ;
	// separate_nextcmd() and expand_filename() depend on this
	eap->arg = rest;
    }

    /*
     * Everything that's left, up to the next command, should be the
     * filename to include.
     */
    eap->argt |= (EX_XFILE | EX_NOSPC);
    separate_nextcmd(eap, FALSE);
    ಯದಿ (*eap->arg == '<' || *eap->arg == '$' || mch_isFullName(eap->arg))
    {
	// For an absolute path, "$VIM/..." or "<sfile>.." we ":source" the
	// file.  Need to expand the file name first.  In other cases
	// ":runtime!" is used.
	source = TRUE;
	ಯದಿ (expand_filename(eap, syn_cmdlinep, &errormsg) == FAIL)
	{
	    ಯದಿ (errormsg != ರಿಕ್ತ)
		emsg(errormsg);
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    /*
     * Save and restore the existing top-level grouplist id and ":syn
     * include" tag around the actual inclusion.
     */
    ಯದಿ (running_syn_inc_tag >= MAX_SYN_INC_TAG)
    {
	emsg(_(e_too_many_syntax_includes));
	ಪ್ರತ್ಯಾಗಮ;
    }
    prev_syn_inc_tag = current_syn_inc_tag;
    current_syn_inc_tag = ++running_syn_inc_tag;
    prev_toplvl_grp = curwin->w_s->b_syn_topgrp;
    curwin->w_s->b_syn_topgrp = sgl_id;
    ಯದಿ (source ? do_source(eap->arg, FALSE, DOSO_NONE, ರಿಕ್ತ) == FAIL
				: source_runtime(eap->arg, DIP_ALL) == FAIL)
	semsg(_(e_cant_open_file_str), eap->arg);
    curwin->w_s->b_syn_topgrp = prev_toplvl_grp;
    current_syn_inc_tag = prev_syn_inc_tag;
}

/*
 * Handle ":syntax keyword {group-name} [{option}] keyword .." command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_keyword(exarg_T *eap, ಪೂರ್ಣಾಙ್ಕ syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*group_name_end;
    ಪೂರ್ಣಾಙ್ಕ		syn_id;
    char_u	*rest;
    char_u	*keyword_copy = ರಿಕ್ತ;
    char_u	*p;
    char_u	*kw;
    syn_opt_arg_T syn_opt_arg;
    ಪೂರ್ಣಾಙ್ಕ		cnt;
    ಪೂರ್ಣಾಙ್ಕ		conceal_char = NUL;

    rest = get_group_name(arg, &group_name_end);

    ಯದಿ (rest != ರಿಕ್ತ)
    {
	ಯದಿ (eap->skip)
	    syn_id = -1;
	ಅನ್ಯಥಾ
	    syn_id = syn_check_group(arg, (ಪೂರ್ಣಾಙ್ಕ)(group_name_end - arg));
	ಯದಿ (syn_id != 0)
	    // allocate a buffer, for removing backslashes in the keyword
	    keyword_copy = alloc(STRLEN(rest) + 1);
	ಯದಿ (keyword_copy != ರಿಕ್ತ)
	{
	    syn_opt_arg.flags = 0;
	    syn_opt_arg.keyword = TRUE;
	    syn_opt_arg.sync_idx = ರಿಕ್ತ;
	    syn_opt_arg.has_cont_list = FALSE;
	    syn_opt_arg.cont_in_list = ರಿಕ್ತ;
	    syn_opt_arg.next_list = ರಿಕ್ತ;

	    /*
	     * The options given apply to ALL keywords, so all options must be
	     * found before keywords can be created.
	     * 1: collect the options and copy the keywords to keyword_copy.
	     */
	    cnt = 0;
	    p = keyword_copy;
	    ಪ್ರತಿ ( ; rest != ರಿಕ್ತ && !ends_excmd2(eap->arg, rest);
							rest = skipwhite(rest))
	    {
		rest = get_syn_options(rest, &syn_opt_arg, &conceal_char,
								    eap->skip);
		ಯದಿ (rest == ರಿಕ್ತ || ends_excmd2(eap->arg, rest))
		    ಭಙ್ಗ;
		// Copy the keyword, removing backslashes, and add a NUL.
		ಯಾವತ್ (*rest != NUL && !VIM_ISWHITE(*rest))
		{
		    ಯದಿ (*rest == '\\' && rest[1] != NUL)
			++rest;
		    *p++ = *rest++;
		}
		*p++ = NUL;
		++cnt;
	    }

	    ಯದಿ (!eap->skip)
	    {
		size_t	kwlen = 0;

		// Adjust flags for use of ":syn include".
		syn_incl_toplevel(syn_id, &syn_opt_arg.flags);

		/*
		 * 2: Add an entry for each keyword.
		 */
		ಪ್ರತಿ (kw = keyword_copy; --cnt >= 0; kw += kwlen + 1)
		{
		    ಪ್ರತಿ (p = vim_strchr(kw, '['); ; )
		    {
			ಯದಿ (p == ರಿಕ್ತ)
			    kwlen = STRLEN(kw);
			ಅನ್ಯಥಾ
			{
			    *p = NUL;
			    kwlen = (size_t)(p - kw);
			}
			add_keyword(kw, kwlen, syn_id, syn_opt_arg.flags,
				syn_opt_arg.cont_in_list,
					 syn_opt_arg.next_list, conceal_char);
			ಯದಿ (p == ರಿಕ್ತ)
			    ಭಙ್ಗ;
			ಯದಿ (p[1] == NUL)
			{
			    semsg(_(e_error_missing_rsb_str), kw);
			    ಗಚ್ಛ error;
			}
			ಯದಿ (p[1] == ']')
			{
			    ಯದಿ (p[2] != NUL)
			    {
				semsg(_(e_trailing_char_after_rsb_str_str),
								    kw, &p[2]);
				ಗಚ್ಛ error;
			    }
			    kw = p + 1;		// skip over the "]"
			    kwlen = 1;
			    ಭಙ್ಗ;
			}
			ಯದಿ (has_mbyte)
			{
			    ಪೂರ್ಣಾಙ್ಕ l = (*mb_ptr2len)(p + 1);

			    mch_memmove(p, p + 1, l);
			    p += l;
			}
			ಅನ್ಯಥಾ
			{
			    p[0] = p[1];
			    ++p;
			}
		    }
		}
	    }
error:
	    vim_free(keyword_copy);
	    vim_free(syn_opt_arg.cont_in_list);
	    vim_free(syn_opt_arg.next_list);
	}
    }

    ಯದಿ (rest != ರಿಕ್ತ)
	set_nextcmd(eap, rest);
    ಅನ್ಯಥಾ
	semsg(_(e_invalid_argument_str), arg);

    redraw_curbuf_later(UPD_SOME_VALID);
    syn_ರಾಶಿ_free_all(curwin->w_s);		// Need to recompute all syntax.
}

/*
 * Handle ":syntax match {name} [{options}] {pattern} [{options}]".
 *
 * Also ":syntax sync match {name} [[grouphere | groupthere] {group-name}] .."
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_match(
    exarg_T	*eap,
    ಪೂರ್ಣಾಙ್ಕ		syncing)	    // TRUE for ":syntax sync match .. "
{
    char_u	*arg = eap->arg;
    char_u	*group_name_end;
    char_u	*rest;
    synpat_T	item;		// the item found in the line
    ಪೂರ್ಣಾಙ್ಕ		syn_id;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    syn_opt_arg_T syn_opt_arg;
    ಪೂರ್ಣಾಙ್ಕ		sync_idx = 0;
    ಪೂರ್ಣಾಙ್ಕ		conceal_char = NUL;
    ಪೂರ್ಣಾಙ್ಕ		orig_called_emsg = called_emsg;

    // Isolate the group name, check for validity
    rest = get_group_name(arg, &group_name_end);

    // Get options before the pattern
    syn_opt_arg.flags = 0;
    syn_opt_arg.keyword = FALSE;
    syn_opt_arg.sync_idx = syncing ? &sync_idx : ರಿಕ್ತ;
    syn_opt_arg.has_cont_list = TRUE;
    syn_opt_arg.cont_list = ರಿಕ್ತ;
    syn_opt_arg.cont_in_list = ರಿಕ್ತ;
    syn_opt_arg.next_list = ರಿಕ್ತ;
    rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);

    // get the pattern.
    init_syn_patterns();
    CLEAR_FIELD(item);
    rest = get_syn_pattern(rest, &item);
    ಯದಿ (vim_regcomp_had_eol() && !(syn_opt_arg.flags & HL_EXCLUDENL))
	syn_opt_arg.flags |= HL_HAS_EOL;

    // Get options after the pattern
    rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);

    ಯದಿ (rest != ರಿಕ್ತ)		// all arguments are valid
    {
	/*
	 * Check for trailing command and illegal trailing arguments.
	 */
	set_nextcmd(eap, rest);
	ಯದಿ (!ends_excmd2(eap->cmd, rest) || eap->skip)
	    rest = ರಿಕ್ತ;
	ಅನ್ಯಥಾ ಯದಿ (ga_grow(&curwin->w_s->b_syn_patterns, 1) == OK
		&& (syn_id = syn_check_group(arg,
					   (ಪೂರ್ಣಾಙ್ಕ)(group_name_end - arg))) != 0)
	{
	    syn_incl_toplevel(syn_id, &syn_opt_arg.flags);
	    /*
	     * Store the pattern in the syn_items list
	     */
	    idx = curwin->w_s->b_syn_patterns.ga_len;
	    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx] = item;
	    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_syncing = syncing;
	    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_type = SPTYPE_MATCH;
	    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_syn.id = syn_id;
	    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_syn.inc_tag = current_syn_inc_tag;
	    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_flags = syn_opt_arg.flags;
	    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_sync_idx = sync_idx;
	    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_cont_list = syn_opt_arg.cont_list;
	    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_syn.cont_in_list =
						     syn_opt_arg.cont_in_list;
#ifdef FEAT_CONCEAL
	    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_cchar = conceal_char;
#endif
	    ಯದಿ (syn_opt_arg.cont_in_list != ರಿಕ್ತ)
		curwin->w_s->b_syn_containedin = TRUE;
	    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_next_list = syn_opt_arg.next_list;
	    ++curwin->w_s->b_syn_patterns.ga_len;

	    // remember that we found a match for syncing on
	    ಯದಿ (syn_opt_arg.flags & (HL_SYNC_HERE|HL_SYNC_THERE))
		curwin->w_s->b_syn_sync_flags |= SF_MATCH;
#ifdef FEAT_FOLDING
	    ಯದಿ (syn_opt_arg.flags & HL_FOLD)
		++curwin->w_s->b_syn_folditems;
#endif

	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_ರಾಶಿ_free_all(curwin->w_s);	// Need to recompute all syntax.
	    ಪ್ರತ್ಯಾಗಮ;	// don't free the progs and patterns now
	}
    }

    /*
     * Something failed, free the allocated memory.
     */
    vim_regfree(item.sp_prog);
    vim_free(item.sp_pattern);
    vim_free(syn_opt_arg.cont_list);
    vim_free(syn_opt_arg.cont_in_list);
    vim_free(syn_opt_arg.next_list);

    ಯದಿ (rest == ರಿಕ್ತ && called_emsg == orig_called_emsg)
	semsg(_(e_invalid_argument_str), arg);
}

/*
 * Handle ":syntax region {group-name} [matchgroup={group-name}]
 *		start {start} .. [skip {skip}] end {end} .. [{options}]".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_region(
    exarg_T	*eap,
    ಪೂರ್ಣಾಙ್ಕ		syncing)	    // TRUE for ":syntax sync region .."
{
    char_u		*arg = eap->arg;
    char_u		*group_name_end;
    char_u		*rest;			// next arg, NULL on error
    char_u		*ಕೀಲಿ_end;
    char_u		*ಕೀಲಿ = ರಿಕ್ತ;
    char_u		*p;
    ಪೂರ್ಣಾಙ್ಕ			item;
#define ITEM_START	    0
#define ITEM_SKIP	    1
#define ITEM_END	    2
#define ITEM_MATCHGROUP	    3
    ಸಂರಚನಾ pat_ptr
    {
	synpat_T	*pp_synp;		// pointer to syn_pattern
	ಪೂರ್ಣಾಙ್ಕ		pp_matchgroup_id;	// matchgroup ID
	ಸಂರಚನಾ pat_ptr	*pp_next;		// pointer to next pat_ptr
    }			*(pat_ptrs[3]);
					// patterns found in the line
    ಸಂರಚನಾ pat_ptr	*ppp;
    ಸಂರಚನಾ pat_ptr	*ppp_next;
    ಪೂರ್ಣಾಙ್ಕ			pat_ಎಣಿಕೆ = 0;		// nr of syn_patterns found
    ಪೂರ್ಣಾಙ್ಕ			syn_id;
    ಪೂರ್ಣಾಙ್ಕ			matchgroup_id = 0;
    ಪೂರ್ಣಾಙ್ಕ			not_enough = FALSE;	// not enough arguments
    ಪೂರ್ಣಾಙ್ಕ			illegal = FALSE;	// illegal arguments
    ಪೂರ್ಣಾಙ್ಕ			success = FALSE;
    ಪೂರ್ಣಾಙ್ಕ			idx;
    syn_opt_arg_T	syn_opt_arg;
    ಪೂರ್ಣಾಙ್ಕ			conceal_char = NUL;

    // Isolate the group name, check for validity
    rest = get_group_name(arg, &group_name_end);

    pat_ptrs[0] = ರಿಕ್ತ;
    pat_ptrs[1] = ರಿಕ್ತ;
    pat_ptrs[2] = ರಿಕ್ತ;

    init_syn_patterns();

    syn_opt_arg.flags = 0;
    syn_opt_arg.keyword = FALSE;
    syn_opt_arg.sync_idx = ರಿಕ್ತ;
    syn_opt_arg.has_cont_list = TRUE;
    syn_opt_arg.cont_list = ರಿಕ್ತ;
    syn_opt_arg.cont_in_list = ರಿಕ್ತ;
    syn_opt_arg.next_list = ರಿಕ್ತ;

    /*
     * get the options, patterns and matchgroup.
     */
    ಯಾವತ್ (rest != ರಿಕ್ತ && !ends_excmd2(eap->cmd, rest))
    {
	// Check for option arguments
	rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);
	ಯದಿ (rest == ರಿಕ್ತ || ends_excmd2(eap->cmd, rest))
	    ಭಙ್ಗ;

	// must be a pattern or matchgroup then
	ಕೀಲಿ_end = rest;
	ಯಾವತ್ (*ಕೀಲಿ_end && !VIM_ISWHITE(*ಕೀಲಿ_end) && *ಕೀಲಿ_end != '=')
	    ++ಕೀಲಿ_end;
	vim_free(ಕೀಲಿ);
	ಕೀಲಿ = vim_strnsave_up(rest, ಕೀಲಿ_end - rest);
	ಯದಿ (ಕೀಲಿ == ರಿಕ್ತ)			// out of memory
	{
	    rest = ರಿಕ್ತ;
	    ಭಙ್ಗ;
	}
	ಯದಿ (STRCMP(ಕೀಲಿ, "MATCHGROUP") == 0)
	    item = ITEM_MATCHGROUP;
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(ಕೀಲಿ, "START") == 0)
	    item = ITEM_START;
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(ಕೀಲಿ, "END") == 0)
	    item = ITEM_END;
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(ಕೀಲಿ, "SKIP") == 0)
	{
	    ಯದಿ (pat_ptrs[ITEM_SKIP] != ರಿಕ್ತ)	// one skip pattern allowed
	    {
		illegal = TRUE;
		ಭಙ್ಗ;
	    }
	    item = ITEM_SKIP;
	}
	ಅನ್ಯಥಾ
	    ಭಙ್ಗ;
	rest = skipwhite(ಕೀಲಿ_end);
	ಯದಿ (*rest != '=')
	{
	    rest = ರಿಕ್ತ;
	    semsg(_(e_missing_equal_str), arg);
	    ಭಙ್ಗ;
	}
	rest = skipwhite(rest + 1);
	ಯದಿ (*rest == NUL)
	{
	    not_enough = TRUE;
	    ಭಙ್ಗ;
	}

	ಯದಿ (item == ITEM_MATCHGROUP)
	{
	    p = skiptowhite(rest);
	    ಯದಿ ((p - rest == 4 && STRNCMP(rest, "NONE", 4) == 0) || eap->skip)
		matchgroup_id = 0;
	    ಅನ್ಯಥಾ
	    {
		matchgroup_id = syn_check_group(rest, (ಪೂರ್ಣಾಙ್ಕ)(p - rest));
		ಯದಿ (matchgroup_id == 0)
		{
		    illegal = TRUE;
		    ಭಙ್ಗ;
		}
	    }
	    rest = skipwhite(p);
	}
	ಅನ್ಯಥಾ
	{
	    /*
	     * Allocate room for a syn_pattern, and link it in the list of
	     * syn_patterns for this item, at the start (because the list is
	     * used from end to start).
	     */
	    ppp = ALLOC_ONE(ಸಂರಚನಾ pat_ptr);
	    ಯದಿ (ppp == ರಿಕ್ತ)
	    {
		rest = ರಿಕ್ತ;
		ಭಙ್ಗ;
	    }
	    ppp->pp_next = pat_ptrs[item];
	    pat_ptrs[item] = ppp;
	    ppp->pp_synp = ALLOC_CLEAR_ONE(synpat_T);
	    ಯದಿ (ppp->pp_synp == ರಿಕ್ತ)
	    {
		rest = ರಿಕ್ತ;
		ಭಙ್ಗ;
	    }

	    /*
	     * Get the syntax pattern and the following offset(s).
	     */
	    // Enable the appropriate \z specials.
	    ಯದಿ (item == ITEM_START)
		reg_do_extmatch = REX_SET;
	    ಅನ್ಯಥಾ ಯದಿ (item == ITEM_SKIP || item == ITEM_END)
		reg_do_extmatch = REX_USE;
	    rest = get_syn_pattern(rest, ppp->pp_synp);
	    reg_do_extmatch = 0;
	    ಯದಿ (item == ITEM_END && vim_regcomp_had_eol()
				       && !(syn_opt_arg.flags & HL_EXCLUDENL))
		ppp->pp_synp->sp_flags |= HL_HAS_EOL;
	    ppp->pp_matchgroup_id = matchgroup_id;
	    ++pat_ಎಣಿಕೆ;
	}
    }
    vim_free(ಕೀಲಿ);
    ಯದಿ (illegal || not_enough)
	rest = ರಿಕ್ತ;

    /*
     * Must have a "start" and "end" pattern.
     */
    ಯದಿ (rest != ರಿಕ್ತ && (pat_ptrs[ITEM_START] == ರಿಕ್ತ ||
						  pat_ptrs[ITEM_END] == ರಿಕ್ತ))
    {
	not_enough = TRUE;
	rest = ರಿಕ್ತ;
    }

    ಯದಿ (rest != ರಿಕ್ತ)
    {
	/*
	 * Check for trailing garbage or command.
	 * If OK, add the item.
	 */
	set_nextcmd(eap, rest);
	ಯದಿ (!ends_excmd(*rest) || eap->skip)
	    rest = ರಿಕ್ತ;
	ಅನ್ಯಥಾ ಯದಿ (ga_grow(&(curwin->w_s->b_syn_patterns), pat_ಎಣಿಕೆ) == OK
		&& (syn_id = syn_check_group(arg,
					   (ಪೂರ್ಣಾಙ್ಕ)(group_name_end - arg))) != 0)
	{
	    syn_incl_toplevel(syn_id, &syn_opt_arg.flags);
	    /*
	     * Store the start/skip/end in the syn_items list
	     */
	    idx = curwin->w_s->b_syn_patterns.ga_len;
	    ಪ್ರತಿ (item = ITEM_START; item <= ITEM_END; ++item)
	    {
		ಪ್ರತಿ (ppp = pat_ptrs[item]; ppp != ರಿಕ್ತ; ppp = ppp->pp_next)
		{
		    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx] = *(ppp->pp_synp);
		    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_syncing = syncing;
		    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_type =
			    (item == ITEM_START) ? SPTYPE_START :
			    (item == ITEM_SKIP) ? SPTYPE_SKIP : SPTYPE_END;
		    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_flags |= syn_opt_arg.flags;
		    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_syn.id = syn_id;
		    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_syn.inc_tag =
							  current_syn_inc_tag;
		    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_syn_match_id =
							ppp->pp_matchgroup_id;
#ifdef FEAT_CONCEAL
		    SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_cchar = conceal_char;
#endif
		    ಯದಿ (item == ITEM_START)
		    {
			SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_cont_list =
							syn_opt_arg.cont_list;
			SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_syn.cont_in_list =
						     syn_opt_arg.cont_in_list;
			ಯದಿ (syn_opt_arg.cont_in_list != ರಿಕ್ತ)
			    curwin->w_s->b_syn_containedin = TRUE;
			SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx].sp_next_list =
							syn_opt_arg.next_list;
		    }
		    ++curwin->w_s->b_syn_patterns.ga_len;
		    ++idx;
#ifdef FEAT_FOLDING
		    ಯದಿ (syn_opt_arg.flags & HL_FOLD)
			++curwin->w_s->b_syn_folditems;
#endif
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
    ಪ್ರತಿ (item = ITEM_START; item <= ITEM_END; ++item)
	ಪ್ರತಿ (ppp = pat_ptrs[item]; ppp != ರಿಕ್ತ; ppp = ppp_next)
	{
	    ಯದಿ (!success && ppp->pp_synp != ರಿಕ್ತ)
	    {
		vim_regfree(ppp->pp_synp->sp_prog);
		vim_free(ppp->pp_synp->sp_pattern);
	    }
	    vim_free(ppp->pp_synp);
	    ppp_next = ppp->pp_next;
	    vim_free(ppp);
	}

    ಯದಿ (!success)
    {
	vim_free(syn_opt_arg.cont_list);
	vim_free(syn_opt_arg.cont_in_list);
	vim_free(syn_opt_arg.next_list);
	ಯದಿ (not_enough)
	    semsg(_(e_not_enough_arguments_syntax_region_str), arg);
	ಅನ್ಯಥಾ ಯದಿ (illegal || rest == ರಿಕ್ತ)
	    semsg(_(e_invalid_argument_str), arg);
    }
}

/*
 * A simple syntax group ID comparison function suitable for use in qsort()
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
syn_compare_stub(ಧ್ರುವ ಶೂನ್ಯ *v1, ಧ್ರುವ ಶೂನ್ಯ *v2)
{
    ಧ್ರುವ short	*s1 = v1;
    ಧ್ರುವ short	*s2 = v2;

    ಪ್ರತ್ಯಾಗಮ (*s1 > *s2 ? 1 : *s1 < *s2 ? -1 : 0);
}

/*
 * Combines lists of syntax clusters.
 * *clstr1 and *clstr2 must both be allocated memory; they will be consumed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_combine_list(short **clstr1, short **clstr2, ಪೂರ್ಣಾಙ್ಕ list_op)
{
    ಪೂರ್ಣಾಙ್ಕ		count1 = 0;
    ಪೂರ್ಣಾಙ್ಕ		count2 = 0;
    short	*g1;
    short	*g2;
    short	*clstr = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		round;

    /*
     * Handle degenerate cases.
     */
    ಯದಿ (*clstr2 == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (*clstr1 == ರಿಕ್ತ || list_op == CLUSTER_REPLACE)
    {
	ಯದಿ (list_op == CLUSTER_REPLACE)
	    vim_free(*clstr1);
	ಯದಿ (list_op == CLUSTER_REPLACE || list_op == CLUSTER_ADD)
	    *clstr1 = *clstr2;
	ಅನ್ಯಥಾ
	    vim_free(*clstr2);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಪ್ರತಿ (g1 = *clstr1; *g1; g1++)
	++count1;
    ಪ್ರತಿ (g2 = *clstr2; *g2; g2++)
	++count2;

    /*
     * For speed purposes, sort both lists.
     */
    qsort(*clstr1, (size_t)count1, ಪರಿಮಾಣ(short), syn_compare_stub);
    qsort(*clstr2, (size_t)count2, ಪರಿಮಾಣ(short), syn_compare_stub);

    /*
     * We proceed in two passes; in round 1, we count the elements to place
     * in the new list, and in round 2, we allocate and populate the new
     * list.  For speed, we use a mergesort-like method, adding the smaller
     * of the current elements in each list to the new list.
     */
    ಪ್ರತಿ (round = 1; round <= 2; round++)
    {
	g1 = *clstr1;
	g2 = *clstr2;
	ಎಣಿಕೆ = 0;

	/*
	 * First, loop through the lists until one of them is empty.
	 */
	ಯಾವತ್ (*g1 && *g2)
	{
	    /*
	     * We always want to add from the first list.
	     */
	    ಯದಿ (*g1 < *g2)
	    {
		ಯದಿ (round == 2)
		    clstr[ಎಣಿಕೆ] = *g1;
		ಎಣಿಕೆ++;
		g1++;
		ಅನುವರ್ತ;
	    }
	    /*
	     * We only want to add from the second list if we're adding the
	     * lists.
	     */
	    ಯದಿ (list_op == CLUSTER_ADD)
	    {
		ಯದಿ (round == 2)
		    clstr[ಎಣಿಕೆ] = *g2;
		ಎಣಿಕೆ++;
	    }
	    ಯದಿ (*g1 == *g2)
		g1++;
	    g2++;
	}

	/*
	 * Now add the leftovers from whichever list didn't get finished
	 * first.  As before, we only want to add from the second list if
	 * we're adding the lists.
	 */
	ಪ್ರತಿ (; *g1; g1++, ಎಣಿಕೆ++)
	    ಯದಿ (round == 2)
		clstr[ಎಣಿಕೆ] = *g1;
	ಯದಿ (list_op == CLUSTER_ADD)
	    ಪ್ರತಿ (; *g2; g2++, ಎಣಿಕೆ++)
		ಯದಿ (round == 2)
		    clstr[ಎಣಿಕೆ] = *g2;

	ಯದಿ (round == 1)
	{
	    /*
	     * If the group ended up empty, we don't need to allocate any
	     * space for it.
	     */
	    ಯದಿ (ಎಣಿಕೆ == 0)
	    {
		clstr = ರಿಕ್ತ;
		ಭಙ್ಗ;
	    }
	    clstr = ALLOC_MULT(short, ಎಣಿಕೆ + 1);
	    ಯದಿ (clstr == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    clstr[ಎಣಿಕೆ] = 0;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
syn_scl_name2id(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*name_u;

    // Avoid using stricmp() too much, it's slow on some systems
    name_u = vim_strsave_up(name);
    ಯದಿ (name_u == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಪ್ರತಿ (i = curwin->w_s->b_syn_clusters.ga_len; --i >= 0; )
	ಯದಿ (SYN_CLSTR(curwin->w_s)[i].scl_name_u != ರಿಕ್ತ
		&& STRCMP(name_u, SYN_CLSTR(curwin->w_s)[i].scl_name_u) == 0)
	    ಭಙ್ಗ;
    vim_free(name_u);
    ಪ್ರತ್ಯಾಗಮ (i < 0 ? 0 : i + SYNID_CLUSTER);
}

/*
 * Like syn_scl_name2id(), but take a pointer + length argument.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
syn_scl_namen2id(char_u *linep, ಪೂರ್ಣಾಙ್ಕ len)
{
    char_u  *name;
    ಪೂರ್ಣಾಙ್ಕ	    id = 0;

    name = vim_strnsave(linep, len);
    ಯದಿ (name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    id = syn_scl_name2id(name);
    vim_free(name);
    ಪ್ರತ್ಯಾಗಮ id;
}

/*
 * Find syntax cluster name in the table and return its ID.
 * The argument is a pointer to the name and the length of the name.
 * If it doesn't exist yet, a new entry is created.
 * Return 0 for failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
syn_check_cluster(char_u *pp, ಪೂರ್ಣಾಙ್ಕ len)
{
    ಪೂರ್ಣಾಙ್ಕ		id;
    char_u	*name;

    name = vim_strnsave(pp, len);
    ಯದಿ (name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    id = syn_scl_name2id(name);
    ಯದಿ (id == 0)			// doesn't exist yet
	id = syn_add_cluster(name);
    ಅನ್ಯಥಾ
	vim_free(name);
    ಪ್ರತ್ಯಾಗಮ id;
}

/*
 * Add new syntax cluster and return its ID.
 * "name" must be an allocated string, it will be consumed.
 * Return 0 for failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
syn_add_cluster(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ		len;

    /*
     * First call for this growarray: init growing array.
     */
    ಯದಿ (curwin->w_s->b_syn_clusters.ga_data == ರಿಕ್ತ)
    {
	curwin->w_s->b_syn_clusters.ga_itemsize = ಪರಿಮಾಣ(syn_cluster_T);
	curwin->w_s->b_syn_clusters.ga_growsize = 10;
    }

    len = curwin->w_s->b_syn_clusters.ga_len;
    ಯದಿ (len >= MAX_CLUSTER_ID)
    {
	emsg(_(e_too_many_syntax_clusters));
	vim_free(name);
	ಪ್ರತ್ಯಾಗಮ 0;
    }

    /*
     * Make room for at least one other cluster entry.
     */
    ಯದಿ (ga_grow(&curwin->w_s->b_syn_clusters, 1) == FAIL)
    {
	vim_free(name);
	ಪ್ರತ್ಯಾಗಮ 0;
    }

    CLEAR_POINTER(&(SYN_CLSTR(curwin->w_s)[len]));
    SYN_CLSTR(curwin->w_s)[len].scl_name = name;
    SYN_CLSTR(curwin->w_s)[len].scl_name_u = vim_strsave_up(name);
    SYN_CLSTR(curwin->w_s)[len].scl_list = ರಿಕ್ತ;
    ++curwin->w_s->b_syn_clusters.ga_len;

    ಯದಿ (STRICMP(name, "Spell") == 0)
	curwin->w_s->b_spell_cluster_id = len + SYNID_CLUSTER;
    ಯದಿ (STRICMP(name, "NoSpell") == 0)
	curwin->w_s->b_nospell_cluster_id = len + SYNID_CLUSTER;

    ಪ್ರತ್ಯಾಗಮ len + SYNID_CLUSTER;
}

/*
 * Handle ":syntax cluster {cluster-name} [contains={groupname},..]
 *		[add={groupname},..] [remove={groupname},..]".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_cluster(exarg_T *eap, ಪೂರ್ಣಾಙ್ಕ syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*group_name_end;
    char_u	*rest;
    ಪೂರ್ಣಾಙ್ಕ		scl_id;
    short	*clstr_list;
    ಪೂರ್ಣಾಙ್ಕ		got_clstr = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		opt_len;
    ಪೂರ್ಣಾಙ್ಕ		list_op;

    eap->nextcmd = find_nextcmd(arg);
    ಯದಿ (eap->skip)
	ಪ್ರತ್ಯಾಗಮ;

    rest = get_group_name(arg, &group_name_end);

    ಯದಿ (rest != ರಿಕ್ತ)
    {
	scl_id = syn_check_cluster(arg, (ಪೂರ್ಣಾಙ್ಕ)(group_name_end - arg));
	ಯದಿ (scl_id == 0)
	    ಪ್ರತ್ಯಾಗಮ;
	scl_id -= SYNID_CLUSTER;

	ಪ್ರತಿ (;;)
	{
	    ಯದಿ (STRNICMP(rest, "add", 3) == 0
		    && (VIM_ISWHITE(rest[3]) || rest[3] == '='))
	    {
		opt_len = 3;
		list_op = CLUSTER_ADD;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(rest, "remove", 6) == 0
		    && (VIM_ISWHITE(rest[6]) || rest[6] == '='))
	    {
		opt_len = 6;
		list_op = CLUSTER_SUBTRACT;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(rest, "contains", 8) == 0
			&& (VIM_ISWHITE(rest[8]) || rest[8] == '='))
	    {
		opt_len = 8;
		list_op = CLUSTER_REPLACE;
	    }
	    ಅನ್ಯಥಾ
		ಭಙ್ಗ;

	    clstr_list = ರಿಕ್ತ;
	    ಯದಿ (get_id_list(&rest, opt_len, &clstr_list, eap->skip) == FAIL)
	    {
		semsg(_(e_invalid_argument_str), rest);
		ಭಙ್ಗ;
	    }
	    ಯದಿ (scl_id >= 0)
		syn_combine_list(&SYN_CLSTR(curwin->w_s)[scl_id].scl_list,
			     &clstr_list, list_op);
	    ಅನ್ಯಥಾ
		vim_free(clstr_list);
	    got_clstr = TRUE;
	}

	ಯದಿ (got_clstr)
	{
	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_ರಾಶಿ_free_all(curwin->w_s);	// Need to recompute all.
	}
    }

    ಯದಿ (!got_clstr)
	emsg(_(e_no_cluster_specified));
    ಯದಿ (rest == ರಿಕ್ತ || !ends_excmd2(eap->cmd, rest))
	semsg(_(e_invalid_argument_str), arg);
}

/*
 * On first call for current buffer: Init growing array.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_syn_patterns(ಶೂನ್ಯ)
{
    curwin->w_s->b_syn_patterns.ga_itemsize = ಪರಿಮಾಣ(synpat_T);
    curwin->w_s->b_syn_patterns.ga_growsize = 10;
}

/*
 * Get one pattern for a ":syntax match" or ":syntax region" command.
 * Stores the pattern and program in a synpat_T.
 * Returns a pointer to the next argument, or NULL in case of an error.
 */
    ಸ್ಥಿರ char_u *
get_syn_pattern(char_u *arg, synpat_T *ci)
{
    char_u	*end;
    ಪೂರ್ಣಾಙ್ಕ		*p;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    char_u	*cpo_save;

    // need at least three chars
    ಯದಿ (arg == ರಿಕ್ತ || arg[0] == NUL || arg[1] == NUL || arg[2] == NUL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    end = skip_regexp(arg + 1, *arg, TRUE);
    ಯದಿ (*end != *arg)			    // end delimiter not found
    {
	semsg(_(e_pattern_delimiter_not_found_str), arg);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    // store the pattern and compiled regexp program
    ಯದಿ ((ci->sp_pattern = vim_strnsave(arg + 1, end - arg - 1)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Make 'cpoptions' empty, to avoid the 'l' flag
    cpo_save = p_cpo;
    p_cpo = empty_option;
    ci->sp_prog = vim_regcomp(ci->sp_pattern, RE_MAGIC);
    p_cpo = cpo_save;

    ಯದಿ (ci->sp_prog == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ci->sp_ic = curwin->w_s->b_syn_ic;
#ifdef FEAT_PROFILE
    syn_clear_time(&ci->sp_time);
#endif

    /*
     * Check for a match, highlight or region offset.
     */
    ++end;
    ಕುರು
    {
	ಪ್ರತಿ (idx = SPO_ಎಣಿಕೆ; --idx >= 0; )
	    ಯದಿ (STRNCMP(end, spo_name_tab[idx], 3) == 0)
		ಭಙ್ಗ;
	ಯದಿ (idx >= 0)
	{
	    p = &(ci->sp_offsets[idx]);
	    ಯದಿ (idx != SPO_LC_OFF)
		ವಿಕಲ್ಪ (end[3])
		{
		    ಪಕ್ಷ 's':   ಭಙ್ಗ;
		    ಪಕ್ಷ 'b':   ಭಙ್ಗ;
		    ಪಕ್ಷ 'e':   idx += SPO_ಎಣಿಕೆ; ಭಙ್ಗ;
		    ಸ್ವಭಾವ:    idx = -1; ಭಙ್ಗ;
		}
	    ಯದಿ (idx >= 0)
	    {
		ci->sp_off_flags |= (1 << idx);
		ಯದಿ (idx == SPO_LC_OFF)	    // lc=99
		{
		    end += 3;
		    *p = getdigits(&end);

		    // "lc=" offset automatically sets "ms=" offset
		    ಯದಿ (!(ci->sp_off_flags & (1 << SPO_MS_OFF)))
		    {
			ci->sp_off_flags |= (1 << SPO_MS_OFF);
			ci->sp_offsets[SPO_MS_OFF] = *p;
		    }
		}
		ಅನ್ಯಥಾ			    // yy=x+99
		{
		    end += 4;
		    ಯದಿ (*end == '+')
		    {
			++end;
			*p = getdigits(&end);		// positive offset
		    }
		    ಅನ್ಯಥಾ ಯದಿ (*end == '-')
		    {
			++end;
			*p = -getdigits(&end);		// negative offset
		    }
		}
		ಯದಿ (*end != ',')
		    ಭಙ್ಗ;
		++end;
	    }
	}
    } ಯಾವತ್ (idx >= 0);

    ಯದಿ (!ends_excmd2(arg, end) && !VIM_ISWHITE(*end))
    {
	semsg(_(e_garbage_after_pattern_str), arg);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ skipwhite(end);
}

/*
 * Handle ":syntax sync .." command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syn_cmd_sync(exarg_T *eap, ಪೂರ್ಣಾಙ್ಕ syncing UNUSED)
{
    char_u	*arg_start = eap->arg;
    char_u	*arg_end;
    char_u	*ಕೀಲಿ = ರಿಕ್ತ;
    char_u	*next_arg;
    ಪೂರ್ಣಾಙ್ಕ		illegal = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		finished = FALSE;
    long	n;
    char_u	*cpo_save;

    ಯದಿ (ends_excmd2(eap->cmd, arg_start))
    {
	syn_cmd_list(eap, TRUE);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯಾವತ್ (!ends_excmd2(eap->cmd, arg_start))
    {
	arg_end = skiptowhite(arg_start);
	next_arg = skipwhite(arg_end);
	vim_free(ಕೀಲಿ);
	ಕೀಲಿ = vim_strnsave_up(arg_start, arg_end - arg_start);
	ಯದಿ (ಕೀಲಿ == ರಿಕ್ತ)
	    ಭಙ್ಗ;
	ಯದಿ (STRCMP(ಕೀಲಿ, "CCOMMENT") == 0)
	{
	    ಯದಿ (!eap->skip)
		curwin->w_s->b_syn_sync_flags |= SF_CCOMMENT;
	    ಯದಿ (!ends_excmd2(eap->cmd, next_arg))
	    {
		arg_end = skiptowhite(next_arg);
		ಯದಿ (!eap->skip)
		    curwin->w_s->b_syn_sync_id = syn_check_group(next_arg,
						   (ಪೂರ್ಣಾಙ್ಕ)(arg_end - next_arg));
		next_arg = skipwhite(arg_end);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (!eap->skip)
		curwin->w_s->b_syn_sync_id = syn_name2id((char_u *)"Comment");
	}
	ಅನ್ಯಥಾ ಯದಿ (  STRNCMP(ಕೀಲಿ, "LINES", 5) == 0
		|| STRNCMP(ಕೀಲಿ, "MINLINES", 8) == 0
		|| STRNCMP(ಕೀಲಿ, "MAXLINES", 8) == 0
		|| STRNCMP(ಕೀಲಿ, "LINEBREAKS", 10) == 0)
	{
	    ಯದಿ (ಕೀಲಿ[4] == 'S')
		arg_end = ಕೀಲಿ + 6;
	    ಅನ್ಯಥಾ ಯದಿ (ಕೀಲಿ[0] == 'L')
		arg_end = ಕೀಲಿ + 11;
	    ಅನ್ಯಥಾ
		arg_end = ಕೀಲಿ + 9;
	    ಯದಿ (arg_end[-1] != '=' || !VIM_ISDIGIT(*arg_end))
	    {
		illegal = TRUE;
		ಭಙ್ಗ;
	    }
	    n = getdigits(&arg_end);
	    ಯದಿ (!eap->skip)
	    {
		ಯದಿ (ಕೀಲಿ[4] == 'B')
		    curwin->w_s->b_syn_sync_linebreaks = n;
		ಅನ್ಯಥಾ ಯದಿ (ಕೀಲಿ[1] == 'A')
		    curwin->w_s->b_syn_sync_maxlines = n;
		ಅನ್ಯಥಾ
		    curwin->w_s->b_syn_sync_minlines = n;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(ಕೀಲಿ, "FROMSTART") == 0)
	{
	    ಯದಿ (!eap->skip)
	    {
		curwin->w_s->b_syn_sync_minlines = MAXLNUM;
		curwin->w_s->b_syn_sync_maxlines = 0;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(ಕೀಲಿ, "LINECONT") == 0)
	{
	    ಯದಿ (*next_arg == NUL)	   // missing pattern
	    {
		illegal = TRUE;
		ಭಙ್ಗ;
	    }
	    ಯದಿ (curwin->w_s->b_syn_linecont_pat != ರಿಕ್ತ)
	    {
		emsg(_(e_syntax_sync_ಸಾಲು_continuations_pattern_specified_twice));
		finished = TRUE;
		ಭಙ್ಗ;
	    }
	    arg_end = skip_regexp(next_arg + 1, *next_arg, TRUE);
	    ಯದಿ (*arg_end != *next_arg)	    // end delimiter not found
	    {
		illegal = TRUE;
		ಭಙ್ಗ;
	    }

	    ಯದಿ (!eap->skip)
	    {
		// store the pattern and compiled regexp program
		ಯದಿ ((curwin->w_s->b_syn_linecont_pat =
			    vim_strnsave(next_arg + 1,
				      arg_end - next_arg - 1)) == ರಿಕ್ತ)
		{
		    finished = TRUE;
		    ಭಙ್ಗ;
		}
		curwin->w_s->b_syn_linecont_ic = curwin->w_s->b_syn_ic;

		// Make 'cpoptions' empty, to avoid the 'l' flag
		cpo_save = p_cpo;
		p_cpo = empty_option;
		curwin->w_s->b_syn_linecont_prog =
		       vim_regcomp(curwin->w_s->b_syn_linecont_pat, RE_MAGIC);
		p_cpo = cpo_save;
#ifdef FEAT_PROFILE
		syn_clear_time(&curwin->w_s->b_syn_linecont_time);
#endif

		ಯದಿ (curwin->w_s->b_syn_linecont_prog == ರಿಕ್ತ)
		{
		    VIM_CLEAR(curwin->w_s->b_syn_linecont_pat);
		    finished = TRUE;
		    ಭಙ್ಗ;
		}
	    }
	    next_arg = skipwhite(arg_end + 1);
	}
	ಅನ್ಯಥಾ
	{
	    eap->arg = next_arg;
	    ಯದಿ (STRCMP(ಕೀಲಿ, "MATCH") == 0)
		syn_cmd_match(eap, TRUE);
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(ಕೀಲಿ, "REGION") == 0)
		syn_cmd_region(eap, TRUE);
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(ಕೀಲಿ, "CLEAR") == 0)
		syn_cmd_clear(eap, TRUE);
	    ಅನ್ಯಥಾ
		illegal = TRUE;
	    finished = TRUE;
	    ಭಙ್ಗ;
	}
	arg_start = next_arg;
    }
    vim_free(ಕೀಲಿ);
    ಯದಿ (illegal)
	semsg(_(e_illegal_arguments_str), arg_start);
    ಅನ್ಯಥಾ ಯದಿ (!finished)
    {
	set_nextcmd(eap, arg_start);
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_id_list(
    char_u	**arg,
    ಪೂರ್ಣಾಙ್ಕ		keylen,		// length of keyword
    short	**list,		// where to store the resulting list, if not
				// NULL, the list is silently skipped!
    ಪೂರ್ಣಾಙ್ಕ		skip)
{
    char_u	*p = ರಿಕ್ತ;
    char_u	*end;
    ಪೂರ್ಣಾಙ್ಕ		round;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		total_ಎಣಿಕೆ = 0;
    short	*retval = ರಿಕ್ತ;
    char_u	*name;
    regmatch_T	regmatch;
    ಪೂರ್ಣಾಙ್ಕ		id;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		failed = FALSE;

    /*
     * We parse the list twice:
     * round == 1: count the number of items, allocate the array.
     * round == 2: fill the array with the items.
     * In round 1 new groups may be added, causing the number of items to
     * grow when a regexp is used.  In that case round 1 is done once again.
     */
    ಪ್ರತಿ (round = 1; round <= 2; ++round)
    {
	/*
	 * skip "contains"
	 */
	p = skipwhite(*arg + keylen);
	ಯದಿ (*p != '=')
	{
	    semsg(_(e_missing_equal_sign_str), *arg);
	    ಭಙ್ಗ;
	}
	p = skipwhite(p + 1);
	ಯದಿ (ends_excmd2(*arg, p))
	{
	    semsg(_(e_empty_argument_str), *arg);
	    ಭಙ್ಗ;
	}

	/*
	 * parse the arguments after "contains"
	 */
	ಎಣಿಕೆ = 0;
	ಯಾವತ್ (!ends_excmd2(*arg, p))
	{
	    ಪ್ರತಿ (end = p; *end && !VIM_ISWHITE(*end) && *end != ','; ++end)
		;
	    name = alloc(end - p + 3);	    // leave room for "^$"
	    ಯದಿ (name == ರಿಕ್ತ)
	    {
		failed = TRUE;
		ಭಙ್ಗ;
	    }
	    vim_strncpy(name + 1, p, end - p);
	    ಯದಿ (       STRCMP(name + 1, "ALLBUT") == 0
		    || STRCMP(name + 1, "ALL") == 0
		    || STRCMP(name + 1, "TOP") == 0
		    || STRCMP(name + 1, "CONTAINED") == 0)
	    {
		ಯದಿ (TOUPPER_ASC(**arg) != 'C')
		{
		    semsg(_(e_str_not_allowed_here), name + 1);
		    failed = TRUE;
		    vim_free(name);
		    ಭಙ್ಗ;
		}
		ಯದಿ (ಎಣಿಕೆ != 0)
		{
		    semsg(_(e_str_must_be_first_in_contains_list), name + 1);
		    failed = TRUE;
		    vim_free(name);
		    ಭಙ್ಗ;
		}
		ಯದಿ (name[1] == 'A')
		    id = SYNID_ALLBUT;
		ಅನ್ಯಥಾ ಯದಿ (name[1] == 'T')
		    id = SYNID_ತುದಿ;
		ಅನ್ಯಥಾ
		    id = SYNID_CONTAINED;
		id += current_syn_inc_tag;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (name[1] == '@')
	    {
		ಯದಿ (skip)
		    id = -1;
		ಅನ್ಯಥಾ
		    id = syn_check_cluster(name + 2, (ಪೂರ್ಣಾಙ್ಕ)(end - p - 1));
	    }
	    ಅನ್ಯಥಾ
	    {
		/*
		 * Handle full group name.
		 */
		ಯದಿ (vim_strpbrk(name + 1, (char_u *)"\\.*^$~[") == ರಿಕ್ತ)
		    id = syn_check_group(name + 1, (ಪೂರ್ಣಾಙ್ಕ)(end - p));
		ಅನ್ಯಥಾ
		{
		    /*
		     * Handle match of regexp with group names.
		     */
		    *name = '^';
		    STRCAT(name, "$");
		    regmatch.regprog = vim_regcomp(name, RE_MAGIC);
		    ಯದಿ (regmatch.regprog == ರಿಕ್ತ)
		    {
			failed = TRUE;
			vim_free(name);
			ಭಙ್ಗ;
		    }

		    regmatch.rm_ic = TRUE;
		    id = 0;
		    ಪ್ರತಿ (i = highlight_ಅಂಕೆ_groups(); --i >= 0; )
		    {
			ಯದಿ (vim_regexec(&regmatch, highlight_group_name(i),
								  (colnr_T)0))
			{
			    ಯದಿ (round == 2)
			    {
				// Got more items than expected; can happen
				// when adding items that match:
				// "contains=a.*b,axb".
				// Go back to first round
				ಯದಿ (ಎಣಿಕೆ >= total_ಎಣಿಕೆ)
				{
				    vim_free(retval);
				    round = 1;
				}
				ಅನ್ಯಥಾ
				    retval[ಎಣಿಕೆ] = i + 1;
			    }
			    ++ಎಣಿಕೆ;
			    id = -1;	    // remember that we found one
			}
		    }
		    vim_regfree(regmatch.regprog);
		}
	    }
	    vim_free(name);
	    ಯದಿ (id == 0)
	    {
		semsg(_(e_unknown_group_name_str), p);
		failed = TRUE;
		ಭಙ್ಗ;
	    }
	    ಯದಿ (id > 0)
	    {
		ಯದಿ (round == 2)
		{
		    // Got more items than expected, go back to first round
		    ಯದಿ (ಎಣಿಕೆ >= total_ಎಣಿಕೆ)
		    {
			vim_free(retval);
			round = 1;
		    }
		    ಅನ್ಯಥಾ
			retval[ಎಣಿಕೆ] = id;
		}
		++ಎಣಿಕೆ;
	    }
	    p = skipwhite(end);
	    ಯದಿ (*p != ',')
		ಭಙ್ಗ;
	    p = skipwhite(p + 1);	// skip comma in between arguments
	}
	ಯದಿ (failed)
	    ಭಙ್ಗ;
	ಯದಿ (round == 1)
	{
	    retval = ALLOC_MULT(short, ಎಣಿಕೆ + 1);
	    ಯದಿ (retval == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    retval[ಎಣಿಕೆ] = 0;	    // zero means end of the list
	    total_ಎಣಿಕೆ = ಎಣಿಕೆ;
	}
    }

    *arg = p;
    ಯದಿ (failed || retval == ರಿಕ್ತ)
    {
	vim_free(retval);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (*list == ರಿಕ್ತ)
	*list = retval;
    ಅನ್ಯಥಾ
	vim_free(retval);	// list already found, don't overwrite it

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Make a copy of an ID list.
 */
    ಸ್ಥಿರ short *
copy_id_list(short *list)
{
    ಪೂರ್ಣಾಙ್ಕ	    len;
    ಪೂರ್ಣಾಙ್ಕ	    ಎಣಿಕೆ;
    short   *retval;

    ಯದಿ (list == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತಿ (ಎಣಿಕೆ = 0; list[ಎಣಿಕೆ]; ++ಎಣಿಕೆ)
	;
    len = (ಎಣಿಕೆ + 1) * ಪರಿಮಾಣ(short);
    retval = alloc(len);
    ಯದಿ (retval != ರಿಕ್ತ)
	mch_memmove(retval, list, (size_t)len);

    ಪ್ರತ್ಯಾಗಮ retval;
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
#define IDL_CACHE_SIZE 16

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    short	*idl_ಕೀಲಿ;	// the list this entry resolves (NULL when empty)
    short	*idl_ids;	// sorted, cluster-expanded group IDs (allocated)
    ಪೂರ್ಣಾಙ್ಕ		idl_ಎಣಿಕೆ;	// number of IDs in idl_ids
    short	idl_marker;	// leading ALLBUT/TOP/CONTAINED item, or 0
    bool	idl_usable;	// false: not cacheable, use the slow path
} idl_entry_T;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    idl_entry_T	ent[IDL_CACHE_SIZE];
    ಪೂರ್ಣಾಙ್ಕ		round;		// round-robin replacement index
    ಪೂರ್ಣಾಙ್ಕ		last;		// index of the last returned entry (fast path)
} idl_cache_T;

/*
 * Collect the explicit group IDs of "list" into "ga", expanding clusters.
 * "seen" records visited clusters to break cycles and avoid duplicate work.
 * Return false when the list cannot be represented as a plain set (a nested
 * ALLBUT/TOP/CONTAINED marker) or on out of memory.
 */
    ಸ್ಥಿರ bool
idl_collect(short *list, garray_T *ga, garray_T *seen)
{
    ಯದಿ (list == ರಿಕ್ತ || list == ID_LIST_ALL)
	ಪ್ರತ್ಯಾಗಮ false;
    ಪ್ರತಿ (short item = *list; item != 0; item = *++list)
    {
	ಯದಿ (item >= SYNID_ALLBUT && item < SYNID_CLUSTER)
	    ಪ್ರತ್ಯಾಗಮ false;	// nested ALLBUT/TOP/CONTAINED: not cacheable
	ಯದಿ (item >= SYNID_CLUSTER)
	{
	    short	*scl_list;
	    bool	seen_it = false;

	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < seen->ga_len; ++i)
		ಯದಿ (((short *)seen->ga_data)[i] == item)
		{
		    seen_it = true;
		    ಭಙ್ಗ;
		}
	    ಯದಿ (seen_it)
		ಅನುವರ್ತ;
	    ಯದಿ (ga_grow(seen, 1) == FAIL)
		ಪ್ರತ್ಯಾಗಮ false;
	    ((short *)seen->ga_data)[seen->ga_len++] = item;
	    // Record the cluster id itself, not just its members: the slow
	    // path matches "item == id" for a cluster id present in the list.
	    ಯದಿ (ga_grow(ga, 1) == FAIL)
		ಪ್ರತ್ಯಾಗಮ false;
	    ((short *)ga->ga_data)[ga->ga_len++] = item;
	    scl_list = SYN_CLSTR(syn_block)[item - SYNID_CLUSTER].scl_list;
	    ಯದಿ (scl_list != ರಿಕ್ತ && !idl_collect(scl_list, ga, seen))
		ಪ್ರತ್ಯಾಗಮ false;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (ga_grow(ga, 1) == FAIL)
		ಪ್ರತ್ಯಾಗಮ false;
	    ((short *)ga->ga_data)[ga->ga_len++] = item;
	}
    }
    ಪ್ರತ್ಯಾಗಮ true;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
idl_id_cmp(ಧ್ರುವ ಶೂನ್ಯ *a, ಧ್ರುವ ಶೂನ್ಯ *b)
{
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)*(ಧ್ರುವ short *)a - (ಪೂರ್ಣಾಙ್ಕ)*(ಧ್ರುವ short *)b;
}

/*
 * Get the cache entry that resolves "list", building it if needed.
 * Returns NULL only on allocation failure.
 */
    ಸ್ಥಿರ idl_entry_T *
idl_get_entry(short *list)
{
    idl_cache_T	*cache = (idl_cache_T *)syn_block->b_idlist_cache;
    idl_entry_T	*e;
    garray_T	ga, seen;

    ಯದಿ (cache == ರಿಕ್ತ)
    {
	cache = ALLOC_CLEAR_ONE(idl_cache_T);
	ಯದಿ (cache == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	syn_block->b_idlist_cache = cache;
    }
    // Fast path: the same list is usually queried many times in a row.
    ಯದಿ (cache->ent[cache->last].idl_ಕೀಲಿ == list)
	ಪ್ರತ್ಯಾಗಮ &cache->ent[cache->last];
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < IDL_CACHE_SIZE; ++i)
	ಯದಿ (cache->ent[i].idl_ಕೀಲಿ == list)
	{
	    cache->last = i;
	    ಪ್ರತ್ಯಾಗಮ &cache->ent[i];
	}

    // Build a new entry, replacing one in round-robin order.
    cache->last = cache->round;
    e = &cache->ent[cache->round];
    cache->round = (cache->round + 1) % IDL_CACHE_SIZE;
    VIM_CLEAR(e->idl_ids);
    e->idl_ಎಣಿಕೆ = 0;
    e->idl_ಕೀಲಿ = list;
    e->idl_marker = 0;
    e->idl_usable = true;

    // A leading ALLBUT/TOP/CONTAINED marker is kept aside; the rest is the set.
    ಯದಿ (*list >= SYNID_ALLBUT && *list < SYNID_CLUSTER)
    {
	e->idl_marker = *list;
	++list;
    }

    ga_init2(&ga, ಪರಿಮಾಣ(short), 50);
    ga_init2(&seen, ಪರಿಮಾಣ(short), 10);
    ಯದಿ (!idl_collect(list, &ga, &seen))
    {
	e->idl_usable = false;
	ga_clear(&ga);
	ga_clear(&seen);
	ಪ್ರತ್ಯಾಗಮ e;
    }
    ga_clear(&seen);
    ಯದಿ (ga.ga_len > 0)
    {
	qsort(ga.ga_data, (size_t)ga.ga_len, ಪರಿಮಾಣ(short), idl_id_cmp);
	e->idl_ids = (short *)ga.ga_data;	// take ownership
	e->idl_ಎಣಿಕೆ = ga.ga_len;
    }
    ಅನ್ಯಥಾ
	ga_clear(&ga);
    ಪ್ರತ್ಯಾಗಮ e;
}

/*
 * Return true if "id" is in the resolved set of cache entry "e".
 */
    ಸ್ಥಿರ bool
idl_contains(idl_entry_T *e, short id)
{
    ಪೂರ್ಣಾಙ್ಕ	lo = 0;
    ಪೂರ್ಣಾಙ್ಕ	hi = e->idl_ಎಣಿಕೆ - 1;

    ಯಾವತ್ (lo <= hi)
    {
	ಪೂರ್ಣಾಙ್ಕ	mid = (lo + hi) / 2;
	short	v = e->idl_ids[mid];

	ಯದಿ (v == id)
	    ಪ್ರತ್ಯಾಗಮ true;
	ಯದಿ (v < id)
	    lo = mid + 1;
	ಅನ್ಯಥಾ
	    hi = mid - 1;
    }
    ಪ್ರತ್ಯಾಗಮ false;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
idl_cache_clear(synblock_T *block)
{
    idl_cache_T	*cache = (idl_cache_T *)block->b_idlist_cache;

    ಯದಿ (cache == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < IDL_CACHE_SIZE; ++i)
	vim_free(cache->ent[i].idl_ids);
    vim_free(cache);
    block->b_idlist_cache = ರಿಕ್ತ;
}
// }}}

/*
 * Check if syntax group "ssp" is in the ID list "list" of "cur_si".
 * "cur_si" can be NULL if not checking the "containedin" list.
 * Used to check if a syntax item is in the "contains" or "nextgroup" list of
 * the current item.
 * This function is called very often, keep it fast!!
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
in_id_list(
    stateitem_T	*cur_si,	// current item or NULL
    short	*list,		// id list
    ಸಂರಚನಾ sp_syn *ssp,		// group id and ":syn include" tag of group
    ಪೂರ್ಣಾಙ್ಕ		flags)		// group flags
{
    ಪೂರ್ಣಾಙ್ಕ		retval;
    short	*scl_list;
    short	item;
    short	id = ssp->id;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	depth = 0;
    ಪೂರ್ಣಾಙ್ಕ		r;
    ಪೂರ್ಣಾಙ್ಕ		toplevel;
    idl_entry_T	*e;

    // If ssp has a "containedin" list and "cur_si" is in it, return TRUE.
    ಯದಿ (cur_si != ರಿಕ್ತ && ssp->cont_in_list != ರಿಕ್ತ
					    && !(cur_si->si_flags & HL_MATCH))
    {
	// Ignore transparent items without a contains argument.  Double check
	// that we don't go back past the first one.
	ಯಾವತ್ ((cur_si->si_flags & HL_TRANS_CONT)
		&& cur_si > (stateitem_T *)(current_state.ga_data))
	    --cur_si;
	// cur_si->si_idx is -1 for keywords, these never contain anything.
	ಯದಿ (cur_si->si_idx >= 0 && in_id_list(ರಿಕ್ತ, ssp->cont_in_list,
		&(SYN_ವಸ್ತುಗಳು(syn_block)[cur_si->si_idx].sp_syn),
		  SYN_ವಸ್ತುಗಳು(syn_block)[cur_si->si_idx].sp_flags))
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }

    ಯದಿ (list == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    /*
     * If list is ID_LIST_ALL, we are in a transparent item that isn't
     * inside anything.  Only allow not-contained groups.
     */
    ಯದಿ (list == ID_LIST_ALL)
	ಪ್ರತ್ಯಾಗಮ !(flags & HL_CONTAINED);

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
    e = disable_syn_idlist_cache_for_testing ? ರಿಕ್ತ : idl_get_entry(list);
    ಯದಿ (e != ರಿಕ್ತ && e->idl_usable)
    {
	ಯದಿ (e->idl_marker != 0)
	{
	    item = e->idl_marker;
	    ಯದಿ (item < SYNID_ತುದಿ)
	    {
		ಯದಿ (item - SYNID_ALLBUT != ssp->inc_tag)
		    ಪ್ರತ್ಯಾಗಮ FALSE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (item < SYNID_CONTAINED)
	    {
		ಯದಿ (item - SYNID_ತುದಿ != ssp->inc_tag || !toplevel)
		    ಪ್ರತ್ಯಾಗಮ FALSE;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (item - SYNID_CONTAINED != ssp->inc_tag || toplevel)
		    ಪ್ರತ್ಯಾಗಮ FALSE;
	    }
	    retval = FALSE;
	}
	ಅನ್ಯಥಾ
	    retval = TRUE;
	ಪ್ರತ್ಯಾಗಮ idl_contains(e, id) ? retval : !retval;
    }

    /*
     * Slow path (uncacheable list, e.g. a nested ALLBUT).
     * If the first item is "ALLBUT", return TRUE if "id" is NOT in the
     * contains list.  We also require that "id" is at the same ":syn include"
     * level as the list.
     */
    item = *list;
    ಯದಿ (item >= SYNID_ALLBUT && item < SYNID_CLUSTER)
    {
	ಯದಿ (item < SYNID_ತುದಿ)
	{
	    // ALL or ALLBUT: accept all groups in the same file
	    ಯದಿ (item - SYNID_ALLBUT != ssp->inc_tag)
		ಪ್ರತ್ಯಾಗಮ FALSE;
	}
	ಅನ್ಯಥಾ ಯದಿ (item < SYNID_CONTAINED)
	{
	    // TOP: accept all not-contained groups in the same file
	    ಯದಿ (item - SYNID_ತುದಿ != ssp->inc_tag || !toplevel)
		ಪ್ರತ್ಯಾಗಮ FALSE;
	}
	ಅನ್ಯಥಾ
	{
	    // CONTAINED: accept all contained groups in the same file
	    ಯದಿ (item - SYNID_CONTAINED != ssp->inc_tag || toplevel)
		ಪ್ರತ್ಯಾಗಮ FALSE;
	}
	item = *++list;
	retval = FALSE;
    }
    ಅನ್ಯಥಾ
	retval = TRUE;

    /*
     * Return "retval" if id is in the contains list.
     */
    ಯಾವತ್ (item != 0)
    {
	ಯದಿ (item == id)
	    ಪ್ರತ್ಯಾಗಮ retval;
	ಯದಿ (item >= SYNID_CLUSTER)
	{
	    scl_list = SYN_CLSTR(syn_block)[item - SYNID_CLUSTER].scl_list;
	    // restrict recursiveness to 30 to avoid an endless loop for a
	    // cluster that includes itself (indirectly)
	    ಯದಿ (scl_list != ರಿಕ್ತ && depth < 30)
	    {
		++depth;
		r = in_id_list(ರಿಕ್ತ, scl_list, ssp, flags);
		--depth;
		ಯದಿ (r)
		    ಪ್ರತ್ಯಾಗಮ retval;
	    }
	}
	item = *++list;
    }
    ಪ್ರತ್ಯಾಗಮ !retval;
}

ಸಂರಚನಾ subcommand
{
    ವರ್ಣ    *name;			// subcommand name
    ಶೂನ್ಯ    (*func)(exarg_T *, ಪೂರ್ಣಾಙ್ಕ);	// function to call
};

ಸ್ಥಿರ ಸಂರಚನಾ subcommand subcommands[] =
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
    {"match",		syn_cmd_match},
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
    char_u	*subcmd_end;
    char_u	*subcmd_name;
    ಪೂರ್ಣಾಙ್ಕ		i;

    syn_cmdlinep = eap->cmdlinep;

    // isolate subcommand name
    ಪ್ರತಿ (subcmd_end = arg; ASCII_ISALPHA(*subcmd_end); ++subcmd_end)
	;
    subcmd_name = vim_strnsave(arg, subcmd_end - arg);
    ಯದಿ (subcmd_name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (eap->skip)		// skip error messages for all subcommands
	++emsg_skip;
    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(subcommands); ++i)
    {
	ಯದಿ (STRCMP(subcmd_name, (char_u *)subcommands[i].name) == 0)
	{
	    eap->arg = skipwhite(subcmd_end);
	    (subcommands[i].func)(eap, FALSE);
	    ಭಙ್ಗ;
	}
    }

    ಯದಿ (i == (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(subcommands))
	semsg(_(e_invalid_syntax_subcommand_str), subcmd_name);

    vim_free(subcmd_name);
    ಯದಿ (eap->skip)
	--emsg_skip;
}

    ಶೂನ್ಯ
ex_ownsyntax(exarg_T *eap)
{
    char_u	*old_ಮೌಲ್ಯ;
    char_u	*new_ಮೌಲ್ಯ_2;

    ಯದಿ (curwin->w_s == &curwin->w_buffer->b_s)
    {
	curwin->w_s = ALLOC_ONE(synblock_T);
	CLEAR_POINTER(curwin->w_s);
	hash_init(&curwin->w_s->b_keywtab);
	hash_init(&curwin->w_s->b_keywtab_ic);
#ifdef FEAT_SPELL
	// TODO: keep the spell checking as it was.
	curwin->w_p_spell = FALSE;	// No spell checking
	// make sure option values are "empty_option" instead of NULL
	clear_string_option(&curwin->w_s->b_p_spc);
	clear_string_option(&curwin->w_s->b_p_spf);
	clear_string_option(&curwin->w_s->b_p_spl);
	clear_string_option(&curwin->w_s->b_p_spo);
#endif
	clear_string_option(&curwin->w_s->b_syn_isk);
    }

    // save value of b:current_syntax
    old_ಮೌಲ್ಯ = get_var_ಮೌಲ್ಯ((char_u *)"b:current_syntax");
    ಯದಿ (old_ಮೌಲ್ಯ != ರಿಕ್ತ)
	old_ಮೌಲ್ಯ = vim_strsave(old_ಮೌಲ್ಯ);

    // Apply the "syntax" autocommand event, this finds and loads the syntax
    // file.
    apply_autocmds(EVENT_SYNTAX, eap->arg, curbuf->b_fname, TRUE, curbuf);

    // move value of b:current_syntax to w:current_syntax
    new_ಮೌಲ್ಯ_2 = get_var_ಮೌಲ್ಯ((char_u *)"b:current_syntax");
    ಯದಿ (new_ಮೌಲ್ಯ_2 != ರಿಕ್ತ)
	set_internal_string_var((char_u *)"w:current_syntax", new_ಮೌಲ್ಯ_2);

    // restore value of b:current_syntax
    ಯದಿ (old_ಮೌಲ್ಯ == ರಿಕ್ತ)
	do_unlet((char_u *)"b:current_syntax", TRUE);
    ಅನ್ಯಥಾ
    {
	set_internal_string_var((char_u *)"b:current_syntax", old_ಮೌಲ್ಯ);
	vim_free(old_ಮೌಲ್ಯ);
    }
}

    ಪೂರ್ಣಾಙ್ಕ
syntax_present(win_T *win)
{
    ಪ್ರತ್ಯಾಗಮ (win->w_s->b_syn_patterns.ga_len != 0
	    || win->w_s->b_syn_clusters.ga_len != 0
	    || win->w_s->b_keywtab.ht_used > 0
	    || win->w_s->b_keywtab_ic.ht_used > 0);
}


ಸ್ಥಿರ ಗಣನಾ
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
set_context_in_echohl_cmd(expand_T *xp, char_u *arg)
{
    xp->xp_context = EXPAND_HIGHLIGHT;
    xp->xp_pattern = arg;
    include_none = 1;
}

/*
 * Handle command line completion for :syntax command.
 */
    ಶೂನ್ಯ
set_context_in_syntax_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    // Default: expand subcommands
    xp->xp_context = EXPAND_SYNTAX;
    expand_what = EXP_SUBCMD;
    xp->xp_pattern = arg;
    include_link = 0;
    include_default = 0;

    ಯದಿ (*arg == NUL)
	ಪ್ರತ್ಯಾಗಮ;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    ಯದಿ (*p == NUL)
	ಪ್ರತ್ಯಾಗಮ;

    // past first word
    xp->xp_pattern = skipwhite(p);
    ಯದಿ (*skiptowhite(xp->xp_pattern) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(arg, "case", p - arg) == 0)
	expand_what = EXP_CASE;
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(arg, "spell", p - arg) == 0)
	expand_what = EXP_SPELL;
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(arg, "sync", p - arg) == 0)
	expand_what = EXP_SYNC;
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(arg, "list", p - arg) == 0)
    {
	p = skipwhite(p);
	ಯದಿ (*p == '@')
	    expand_what = EXP_CLUSTER;
	ಅನ್ಯಥಾ
	    xp->xp_context = EXPAND_HIGHLIGHT;
    }
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(arg, "keyword", p - arg) == 0
	    || STRNICMP(arg, "region", p - arg) == 0
	    || STRNICMP(arg, "match", p - arg) == 0)
	xp->xp_context = EXPAND_HIGHLIGHT;
    ಅನ್ಯಥಾ
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Function given to ExpandGeneric() to obtain the list syntax names for
 * expansion.
 */
    char_u *
get_syntax_name(expand_T *xp, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ವಿಕಲ್ಪ (expand_what)
    {
	ಪಕ್ಷ EXP_SUBCMD:
	    ಯದಿ (idx < 0 || idx >= (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(subcommands))
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    ಪ್ರತ್ಯಾಗಮ (char_u *)subcommands[idx].name;
	ಪಕ್ಷ EXP_CASE:
	{
	    ಸ್ಥಿರ ವರ್ಣ *case_args[] = {"match", "ignore", ರಿಕ್ತ};
	    ಪ್ರತ್ಯಾಗಮ (char_u *)case_args[idx];
	}
	ಪಕ್ಷ EXP_SPELL:
	{
	    ಸ್ಥಿರ ವರ್ಣ *spell_args[] =
		{"toplevel", "notoplevel", "default", ರಿಕ್ತ};
	    ಪ್ರತ್ಯಾಗಮ (char_u *)spell_args[idx];
	}
	ಪಕ್ಷ EXP_SYNC:
	{
	    ಸ್ಥಿರ ವರ್ಣ *sync_args[] =
		{"ccomment", "clear", "fromstart",
		 "linebreaks=", "linecont", "lines=", "match",
		 "maxlines=", "minlines=", "region", ರಿಕ್ತ};
	    ಪ್ರತ್ಯಾಗಮ (char_u *)sync_args[idx];
	}
	ಪಕ್ಷ EXP_CLUSTER:
	{
	    ಯದಿ (idx < curwin->w_s->b_syn_clusters.ga_len)
	    {
		vim_snprintf((ವರ್ಣ *)xp->xp_buf, EXPAND_BUF_LEN, "@%s",
					 SYN_CLSTR(curwin->w_s)[idx].scl_name);
		ಪ್ರತ್ಯಾಗಮ xp->xp_buf;
	    }
	    ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}


/*
 * Function called for expression evaluation: get syntax ID at file position.
 */
    ಪೂರ್ಣಾಙ್ಕ
syn_get_id(
    win_T	*wp,
    long	lnum,
    colnr_T	col,
    ಪೂರ್ಣಾಙ್ಕ		trans,	     // remove transparency
    ಪೂರ್ಣಾಙ್ಕ		*spellp,     // return: can do spell checking
    ಪೂರ್ಣಾಙ್ಕ		keep_state)  // keep state of char at "col"
{
    // When the position is not after the current position and in the same
    // line of the same window with the same buffer, need to restart parsing.
    ಯದಿ (wp != syn_win
	    || wp->w_buffer != syn_buf
	    || lnum != current_lnum
	    || col < current_col)
	syntax_start(wp, lnum);
    ಅನ್ಯಥಾ ಯದಿ (wp->w_buffer == syn_buf
	    && lnum == current_lnum
	    && col > current_col)
	// next_match may not be correct when moving around, e.g. with the
	// "skip" expression in searchpair()
	next_match_idx = -1;

    (ಶೂನ್ಯ)get_syntax_attr(col, spellp, keep_state);

    ಪ್ರತ್ಯಾಗಮ (trans ? current_trans_id : current_id);
}

#ಯದಿ defined(FEAT_CONCEAL)
/*
 * Get extra information about the syntax item.  Must be called right after
 * get_syntax_attr().
 * Stores the current item sequence nr in "*seqnrp".
 * Returns the current flags.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_syntax_info(ಪೂರ್ಣಾಙ್ಕ *seqnrp)
{
    *seqnrp = current_seqnr;
    ಪ್ರತ್ಯಾಗಮ current_flags;
}

/*
 * Return conceal substitution character
 */
    ಪೂರ್ಣಾಙ್ಕ
syn_get_sub_char(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ current_sub_char;
}
#endif

#ಯದಿ defined(FEAT_EVAL)
/*
 * Return the syntax ID at position "i" in the current stack.
 * The caller must have called syn_get_id() before to fill the stack.
 * Returns -1 when "i" is out of range.
 */
    ಪೂರ್ಣಾಙ್ಕ
syn_get_ರಾಶಿ_item(ಪೂರ್ಣಾಙ್ಕ i)
{
    ಯದಿ (i >= current_state.ga_len)
    {
	// Need to invalidate the state, because we didn't properly finish it
	// for the last character, "keep_state" was TRUE.
	invalidate_current_state();
	current_col = MAXCOL;
	ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಪ್ರತ್ಯಾಗಮ CUR_STATE(i).si_id;
}
#endif

#ಯದಿ defined(FEAT_FOLDING)
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
syn_cur_foldlevel(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		level = 0;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = 0; i < current_state.ga_len; ++i)
	ಯದಿ (CUR_STATE(i).si_flags & HL_FOLD)
	    ++level;
    ಪ್ರತ್ಯಾಗಮ level;
}

/*
 * Function called to get folding level for line "lnum" in window "wp".
 */
    ಪೂರ್ಣಾಙ್ಕ
syn_get_foldlevel(win_T *wp, long lnum)
{
    ಪೂರ್ಣಾಙ್ಕ		level = 0;
    ಪೂರ್ಣಾಙ್ಕ		ಕೆಳಗಿನ_level;
    ಪೂರ್ಣಾಙ್ಕ		cur_level;

    // Return quickly when there are no fold items at all.
    ಯದಿ (wp->w_s->b_syn_folditems != 0
	    && !wp->w_s->b_syn_error
# ifdef SYN_TIME_LIMIT
	    && !wp->w_s->b_syn_slow
# endif
	    )
    {
	syntax_start(wp, lnum);

	// Start with the fold level at the start of the line.
	level = syn_cur_foldlevel();

	ಯದಿ (wp->w_s->b_syn_foldlevel == SYNFLD_MINIMUM)
	{
	    // Find the lowest fold level that is followed by a higher one.
	    cur_level = level;
	    ಕೆಳಗಿನ_level = cur_level;
	    ಯಾವತ್ (!current_finished)
	    {
		(ಶೂನ್ಯ)syn_current_attr(FALSE, FALSE, ರಿಕ್ತ, FALSE);
		cur_level = syn_cur_foldlevel();
		ಯದಿ (cur_level < ಕೆಳಗಿನ_level)
		    ಕೆಳಗಿನ_level = cur_level;
		ಅನ್ಯಥಾ ಯದಿ (cur_level > ಕೆಳಗಿನ_level)
		    level = ಕೆಳಗಿನ_level;
		++current_col;
	    }
	}
    }
    ಯದಿ (level > wp->w_p_fdn)
    {
	level = wp->w_p_fdn;
	ಯದಿ (level < 0)
	    level = 0;
    }
    ಪ್ರತ್ಯಾಗಮ level;
}
#endif

#ಯದಿ defined(FEAT_PROFILE)
/*
 * ":syntime".
 */
    ಶೂನ್ಯ
ex_syntime(exarg_T *eap)
{
    ಯದಿ (STRCMP(eap->arg, "on") == 0)
	syn_time_on = TRUE;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(eap->arg, "off") == 0)
	syn_time_on = FALSE;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(eap->arg, "clear") == 0)
	syntime_clear();
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(eap->arg, "report") == 0)
	syntime_report();
    ಅನ್ಯಥಾ
	semsg(_(e_invalid_argument_str), eap->arg);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
syn_clear_time(syn_time_T *st)
{
    profile_zero(&st->total);
    profile_zero(&st->slowest);
    st->ಎಣಿಕೆ = 0;
    st->match = 0;
}

/*
 * Clear the syntax timing for the current buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syntime_clear(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		idx;
    synpat_T	*spp;

    ಯದಿ (!syntax_present(curwin))
    {
	ಸಂದೇಶ(_(ಸಂದೇಶ_no_ವಸ್ತುಗಳು));
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಪ್ರತಿ (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len; ++idx)
    {
	spp = &(SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx]);
	syn_clear_time(&spp->sp_time);
    }
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":syntime {on,off,clear,report}" command.
 */
    char_u *
get_syntime_arg(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ವಿಕಲ್ಪ (idx)
    {
	ಪಕ್ಷ 0: ಪ್ರತ್ಯಾಗಮ (char_u *)"on";
	ಪಕ್ಷ 1: ಪ್ರತ್ಯಾಗಮ (char_u *)"off";
	ಪಕ್ಷ 2: ಪ್ರತ್ಯಾಗಮ (char_u *)"clear";
	ಪಕ್ಷ 3: ಪ್ರತ್ಯಾಗಮ (char_u *)"report";
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    proftime_T	total;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		match;
    proftime_T	slowest;
    proftime_T	average;
    ಪೂರ್ಣಾಙ್ಕ		id;
    char_u	*pattern;
} time_entry_T;

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
syn_compare_syntime(ಧ್ರುವ ಶೂನ್ಯ *v1, ಧ್ರುವ ಶೂನ್ಯ *v2)
{
    ಧ್ರುವ time_entry_T	*s1 = v1;
    ಧ್ರುವ time_entry_T	*s2 = v2;

    ಪ್ರತ್ಯಾಗಮ profile_cmp(&s1->total, &s2->total);
}

/*
 * Clear the syntax timing for the current buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
syntime_report(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		idx;
    synpat_T	*spp;
# ಯದಿ defined(FEAT_RELTIME)
    proftime_T	tm;
# endif
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		patlen;
    proftime_T	total_total;
    ಪೂರ್ಣಾಙ್ಕ		total_ಎಣಿಕೆ = 0;
    garray_T    ga;
    time_entry_T *p;

    ಯದಿ (!syntax_present(curwin))
    {
	ಸಂದೇಶ(_(ಸಂದೇಶ_no_ವಸ್ತುಗಳು));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ga_init2(&ga, ಪರಿಮಾಣ(time_entry_T), 50);
    profile_zero(&total_total);
    ಪ್ರತಿ (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len; ++idx)
    {
	spp = &(SYN_ವಸ್ತುಗಳು(curwin->w_s)[idx]);
	ಯದಿ (spp->sp_time.ಎಣಿಕೆ > 0)
	{
	    (ಶೂನ್ಯ)ga_grow(&ga, 1);
	    p = ((time_entry_T *)ga.ga_data) + ga.ga_len;
	    p->total = spp->sp_time.total;
	    profile_add(&total_total, &spp->sp_time.total);
	    p->ಎಣಿಕೆ = spp->sp_time.ಎಣಿಕೆ;
	    p->match = spp->sp_time.match;
	    total_ಎಣಿಕೆ += spp->sp_time.ಎಣಿಕೆ;
	    p->slowest = spp->sp_time.slowest;
# ಯದಿ defined(FEAT_RELTIME)
	    profile_divide(&spp->sp_time.total, spp->sp_time.ಎಣಿಕೆ, &tm);
	    p->average = tm;
# endif
	    p->id = spp->sp_syn.id;
	    p->pattern = spp->sp_pattern;
	    ++ga.ga_len;
	}
    }

    // Sort on total time. Skip if there are no items to avoid passing NULL
    // pointer to qsort().
    ಯದಿ (ga.ga_len > 1)
	qsort(ga.ga_data, (size_t)ga.ga_len, ಪರಿಮಾಣ(time_entry_T),
							 syn_compare_syntime);

    ಸಂದೇಶ_puts_title(_("  TOTAL      COUNT  MATCH   SLOWEST     AVERAGE   NAME               PATTERN"));
    ಸಂದೇಶ_puts("\n");
    ಪ್ರತಿ (idx = 0; idx < ga.ga_len && !got_int; ++idx)
    {
	p = ((time_entry_T *)ga.ga_data) + idx;

	ಸಂದೇಶ_puts(profile_ಸಂದೇಶ(&p->total));
	ಸಂದೇಶ_puts(" "); // make sure there is always a separating space
	ಸಂದೇಶ_advance(13);
	ಸಂದೇಶ_outnum(p->ಎಣಿಕೆ);
	ಸಂದೇಶ_puts(" ");
	ಸಂದೇಶ_advance(20);
	ಸಂದೇಶ_outnum(p->match);
	ಸಂದೇಶ_puts(" ");
	ಸಂದೇಶ_advance(26);
	ಸಂದೇಶ_puts(profile_ಸಂದೇಶ(&p->slowest));
	ಸಂದೇಶ_puts(" ");
	ಸಂದೇಶ_advance(38);
	ಸಂದೇಶ_puts(profile_ಸಂದೇಶ(&p->average));
	ಸಂದೇಶ_puts(" ");
	ಸಂದೇಶ_advance(50);
	ಸಂದೇಶ_outtrans(highlight_group_name(p->id - 1));
	ಸಂದೇಶ_puts(" ");

	ಸಂದೇಶ_advance(69);
	ಯದಿ (Columns < 80)
	    len = 20; // will wrap anyway
	ಅನ್ಯಥಾ
	    len = Columns - 70;
	patlen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p->pattern);
	ಯದಿ (len > patlen)
	    len = patlen;
	ಸಂದೇಶ_outtrans_len(p->pattern, len);
	ಸಂದೇಶ_puts("\n");
    }
    ga_clear(&ga);
    ಯದಿ (!got_int)
    {
	ಸಂದೇಶ_puts("\n");
	ಸಂದೇಶ_puts(profile_ಸಂದೇಶ(&total_total));
	ಸಂದೇಶ_advance(13);
	ಸಂದೇಶ_outnum(total_ಎಣಿಕೆ);
	ಸಂದೇಶ_puts("\n");
    }
}
#endif

#endif // FEAT_SYN_HL
