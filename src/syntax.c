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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_SYN_HL)

// different types of offsets that are possible
#define SPO_MS_OFF	0	// match  start offset
#define SPO_ME_OFF	1	// match  end	offset
#define SPO_HS_OFF	2	// highl. start offset
#define SPO_HE_OFF	3	// highl. end	offset
#define SPO_RS_OFF	4	// region start offset
#define SPO_RE_OFF	5	// region end	offset
#define SPO_LC_OFF	6	// leading context offset
#define SPO_ଗଣନା	7

ସ୍ଥିର ଅକ୍ଷର *(spo_name_tab[SPO_ଗଣନା]) =
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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା syn_pattern
{
    ଅକ୍ଷର	 sp_type;		// see SPTYPE_ defines below
    ଅକ୍ଷର	 sp_syncing;		// this item used for syncing
    short	 sp_syn_match_id;	// highlight group ID of pattern
    short	 sp_off_flags;		// see below
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		 sp_offsets[SPO_ଗଣନା];	// offsets
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		 sp_flags;		// see HL_ defines below
#ifdef FEAT_CONCEAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		 sp_cchar;		// conceal substitute character
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		 sp_ic;			// ignore-case flag for sp_prog
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		 sp_sync_idx;		// sync item index (syncing only)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		 sp_ଧାଡ଼ି_id;		// ID of last line where tried
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		 sp_startcol;		// next match in sp_line_id line
    short	*sp_cont_list;		// cont. group IDs, if non-zero
    short	*sp_next_list;		// next group IDs, if non-zero
    ସଂରଚନା sp_syn sp_syn;		// struct passed to in_id_list()
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


#define SYN_ବସ୍ତୁଗୁଡ଼ିକ(buf)	((synpat_T *)((buf)->b_syn_patterns.ga_data))

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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା current_attr = 0;	    // attr of current syntax word
#ifdef FEAT_EVAL
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା current_id = 0;	    // ID of current char for syn_get_id()
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା current_trans_id = 0;    // idem, transparency removed
#endif
#ifdef FEAT_CONCEAL
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା current_flags = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା current_seqnr = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା current_sub_char = 0;
#endif

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା syn_cluster_S
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
#define SYNID_ଶୀର୍ଷ	21000	    // syntax group ID for contains=TOP
#define SYNID_CONTAINED	22000	    // syntax group ID for contains=CONTAINED
#define SYNID_CLUSTER	23000	    // first syntax group ID for clusters

#define MAX_SYN_INC_TAG	999	    // maximum before the above overflow
#define MAX_CLUSTER_ID  (32767 - SYNID_CLUSTER)

/*
 * Annoying Hack(TM):  ":syn include" needs this pointer to pass to
 * expand_filename().  Most of the other syntax commands don't need it, so
 * instead of passing it to them, we stow it here.
 */
ସ୍ଥିର char_u **syn_cmdlinep;

/*
 * Another Annoying Hack(TM):  To prevent rules from other ":syn include"'d
 * files from leaking into ALLBUT lists, we assign a unique ID to the
 * rules in each ":syn include"'d file.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା current_syn_inc_tag = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା running_syn_inc_tag = 0;

/*
 * In a hashtable item "hi_key" points to "keyword" in a keyentry.
 * This avoids adding a pointer to the hashtable item.
 * KE2HIKEY() converts a var pointer to a hashitem key pointer.
 * HIKEY2KE() converts a hashitem key pointer to a var pointer.
 * HI2KE() converts a hashitem pointer to a var pointer.
 */
ସ୍ଥିର keyentry_T dumkey;
#define KE2HIKEY(kp)  ((kp)->keyword)
#define HIKEY2KE(p)   ((keyentry_T *)((p) - (dumkey.keyword - (char_u *)&dumkey)))
#define HI2KE(hi)      HIKEY2KE((hi)->hi_ଚାବି)

/*
 * To reduce the time spent in keepend(), remember at which level in the state
 * stack the first item with "keepend" is present.  When "-1", there is no
 * "keepend" on the stack.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା keepend_level = -1;

ସ୍ଥିର ଅକ୍ଷର ସନ୍ଦେଶ_no_ବସ୍ତୁଗୁଡ଼ିକ[] = N_("No Syntax items defined for this buffer");

/*
 * For the current state we need to remember more than just the idx.
 * When si_m_endpos.lnum is 0, the items other than si_idx are unknown.
 * (The end positions have the column number of the next char)
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା state_item
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_idx;			// index of syntax pattern or
					// KEYWORD_IDX
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_id;			// highlight group ID for keywords
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_trans_id;		// idem, transparency removed
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_m_lnum;		// lnum of the match
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_m_startcol;		// starting column of the match
    lpos_T	si_m_endpos;		// just after end posn of the match
    lpos_T	si_h_startpos;		// start position of the highlighting
    lpos_T	si_h_endpos;		// end position of the highlighting
    lpos_T	si_eoe_pos;		// end position of end pattern
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_end_idx;		// group ID for end pattern or zero
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_ends;		// if match ends before si_m_endpos
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_attr;		// attributes in this state
    long	si_flags;		// HL_HAS_EOL flag in this state, and
					// HL_SKIP* for si_next_list
#ifdef FEAT_CONCEAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_seqnr;		// sequence number
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		si_cchar;		// substitution character for conceal
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା next_seqnr = 1;		// value to use for si_seqnr
#endif

/*
 * Struct to reduce the number of arguments to get_syn_options(), it's used
 * very often.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags;		// flags for contained and transparent
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		keyword;	// TRUE for ":syn keyword"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*sync_idx;	// syntax item for "grouphere" argument, NULL
				// if not allowed
    ଅକ୍ଷର	has_cont_list;	// TRUE if "cont_list" can be used
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା next_match_col;		// column for start of next match
ସ୍ଥିର lpos_T next_match_m_endpos;	// position for end of next match
ସ୍ଥିର lpos_T next_match_h_startpos;	// pos. for highl. start of next match
ସ୍ଥିର lpos_T next_match_h_endpos;	// pos. for highl. end of next match
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା next_match_idx;		// index of matched item
ସ୍ଥିର long next_match_flags;		// flags for next match
ସ୍ଥିର lpos_T next_match_eos_pos;	// end of start pattn (start region)
ସ୍ଥିର lpos_T next_match_eoe_pos;	// pos. for end of end pattern
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା next_match_end_idx;		// ID of group for end pattn or zero
ସ୍ଥିର reg_extmatch_T *next_match_extmatch = ଖାଲି;

/*
 * A state stack is an array of integers or stateitem_T, stored in a
 * garray_T.  A state stack is invalid if its itemsize entry is zero.
 */
#define INVALID_STATE(ssp)  ((ssp)->ga_itemsize == 0)
#define VALID_STATE(ssp)    ((ssp)->ga_itemsize != 0)

#define FOR_ALL_SYNSTATES(sb, sst) \
    ପାଇଁ ((sst) = (sb)->b_sst_first; (sst) != ଖାଲି; (sst) = (sst)->sst_next)

/*
 * The current state (within the line) of the recognition engine.
 * When current_state.ga_itemsize is 0 the current state is invalid.
 */
ସ୍ଥିର win_T	*syn_win;		// current window for highlighting
ସ୍ଥିର buf_T	*syn_buf;		// current buffer for highlighting
ସ୍ଥିର synblock_T *syn_block;		// current buffer for highlighting
ସ୍ଥିର linenr_T current_lnum = 0;	// lnum of current state
ସ୍ଥିର colnr_T	current_col = 0;	// column of current state
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	current_state_stored = 0; // TRUE if stored current state
					  // after setting current_finished
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	current_finished = 0;	// current line has been finished
ସ୍ଥିର garray_T current_state		// current stack of state_items
		= {0, 0, 0, 0, ଖାଲି};
ସ୍ଥିର short	*current_next_list = ଖାଲି; // when non-zero, nextgroup list
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	current_next_flags = 0; // flags for current_next_list
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	current_ଧାଡ଼ି_id = 0;	// unique number for current line

#define CUR_STATE(idx)	((stateitem_T *)(current_state.ga_data))[idx]

ସ୍ଥିର ଶୂନ୍ୟ syn_sync(win_T *wp, linenr_T lnum, synstate_T *last_valid);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା syn_match_linecont(linenr_T lnum);
ସ୍ଥିର ଶୂନ୍ୟ syn_start_ଧାଡ଼ି(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ syn_update_ends(ପୂର୍ଣ୍ଣସଂଖ୍ୟା startofline);
ସ୍ଥିର ଶୂନ୍ୟ syn_ଗଦା_alloc(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା syn_ଗଦା_cleanup(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ syn_ଗଦା_free_entry(synblock_T *block, synstate_T *p);
ସ୍ଥିର synstate_T *syn_ଗଦା_find_entry(linenr_T lnum);
ସ୍ଥିର synstate_T *store_current_state(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ load_current_state(synstate_T *from);
ସ୍ଥିର ଶୂନ୍ୟ invalidate_current_state(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା syn_ଗଦା_equal(synstate_T *sp);
ସ୍ଥିର ଶୂନ୍ୟ validate_current_state(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା syn_finish_ଧାଡ଼ି(ପୂର୍ଣ୍ଣସଂଖ୍ୟା syncing);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା syn_current_attr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା syncing, ପୂର୍ଣ୍ଣସଂଖ୍ୟା displaying, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *can_spell, ପୂର୍ଣ୍ଣସଂଖ୍ୟା keep_state);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା did_match_already(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, garray_T *gap);
ସ୍ଥିର stateitem_T *ଠେଲ_next_match(stateitem_T *cur_si);
ସ୍ଥିର ଶୂନ୍ୟ check_state_ends(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ update_si_attr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
ସ୍ଥିର ଶୂନ୍ୟ check_keepend(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ update_si_end(stateitem_T *sip, ପୂର୍ଣ୍ଣସଂଖ୍ୟା startcol, ପୂର୍ଣ୍ଣସଂଖ୍ୟା force);
ସ୍ଥିର short *copy_id_list(short *list);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_id_list(stateitem_T *item, short *cont_list, ସଂରଚନା sp_syn *ssp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଠେଲ_current_state(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
ସ୍ଥିର ଶୂନ୍ୟ କାଢ଼_current_state(ଶୂନ୍ୟ);
#ifdef FEAT_PROFILE
ସ୍ଥିର ଶୂନ୍ୟ syn_clear_time(syn_time_T *tt);
ସ୍ଥିର ଶୂନ୍ୟ syntime_clear(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ syntime_report(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା syn_time_on = FALSE;
# define IF_SYN_TIME(p) (p)
#ଅନ୍ୟଥା
# define IF_SYN_TIME(p) ଖାଲି
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା syn_time_T;
#endif

ସ୍ଥିର ଶୂନ୍ୟ syn_ଗଦା_apply_changes_block(synblock_T *block, buf_T *buf);
ସ୍ଥିର ଶୂନ୍ୟ find_endpos(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, lpos_T *startpos, lpos_T *m_endpos, lpos_T *hl_endpos, long *flagsp, lpos_T *end_endpos, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *end_idx, reg_extmatch_T *start_ext);

ସ୍ଥିର ଶୂନ୍ୟ limit_pos(lpos_T *pos, lpos_T *limit);
ସ୍ଥିର ଶୂନ୍ୟ limit_pos_zero(lpos_T *pos, lpos_T *limit);
ସ୍ଥିର ଶୂନ୍ୟ syn_add_end_off(lpos_T *result, regmmatch_T *regmatch, synpat_T *spp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା extra);
ସ୍ଥିର ଶୂନ୍ୟ syn_add_start_off(lpos_T *result, regmmatch_T *regmatch, synpat_T *spp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା extra);
ସ୍ଥିର char_u *syn_getcurline(ଶୂନ୍ୟ);
ସ୍ଥିର colnr_T syn_getcurline_len(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା syn_regexec(regmmatch_T *rmp, linenr_T lnum, colnr_T col, syn_time_T *st);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_keyword_id(char_u *ଧାଡ଼ି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା startcol, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *endcol, long *flags, short **next_list, stateitem_T *cur_si, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ccharp);
ସ୍ଥିର ଶୂନ୍ୟ syn_remove_pattern(synblock_T *block, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
ସ୍ଥିର ଶୂନ୍ୟ syn_clear_pattern(synblock_T *block, ପୂର୍ଣ୍ଣସଂଖ୍ୟା i);
ସ୍ଥିର ଶୂନ୍ୟ syn_clear_cluster(synblock_T *block, ପୂର୍ଣ୍ଣସଂଖ୍ୟା i);
ସ୍ଥିର ଶୂନ୍ୟ syn_clear_one(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id, ପୂର୍ଣ୍ଣସଂଖ୍ୟା syncing);
ସ୍ଥିର ଶୂନ୍ୟ syn_cmd_onoff(exarg_T *eap, ଅକ୍ଷର *name);
ସ୍ଥିର ଶୂନ୍ୟ syn_ଧାଡ଼ିଗୁଡ଼ିକ_ସନ୍ଦେଶ(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ syn_match_ସନ୍ଦେଶ(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ syn_list_one(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id, ପୂର୍ଣ୍ଣସଂଖ୍ୟା syncing, ପୂର୍ଣ୍ଣସଂଖ୍ୟା link_only);
ସ୍ଥିର ଶୂନ୍ୟ syn_list_cluster(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id);
ସ୍ଥିର ଶୂନ୍ୟ put_id_list(char_u *name, short *list, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr);
ସ୍ଥିର ଶୂନ୍ୟ put_pattern(ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, synpat_T *spp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା syn_list_keywords(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id, hashtab_T *ht, ପୂର୍ଣ୍ଣସଂଖ୍ୟା did_header, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr);
ସ୍ଥିର ଶୂନ୍ୟ syn_clear_keyword(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id, hashtab_T *ht);
ସ୍ଥିର ଶୂନ୍ୟ clear_keywtab(hashtab_T *ht);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା syn_scl_namen2id(char_u *linep, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା syn_check_cluster(char_u *pp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା syn_add_cluster(char_u *name);
ସ୍ଥିର ଶୂନ୍ୟ init_syn_patterns(ଶୂନ୍ୟ);
ସ୍ଥିର char_u *get_syn_pattern(char_u *arg, synpat_T *ci);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_id_list(char_u **arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା keylen, short **list, ପୂର୍ଣ୍ଣସଂଖ୍ୟା skip);
ସ୍ଥିର ଶୂନ୍ୟ syn_combine_list(short **clstr1, short **clstr2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା list_op);
ସ୍ଥିର ଶୂନ୍ୟ idl_cache_clear(synblock_T *block);

/*
 * Start the syntax recognition for a line.  This function is normally called
 * from the screen updating, once for each displayed line.
 * The buffer is remembered in syn_buf, because get_syntax_attr() doesn't get
 * it.	Careful: curbuf and curwin are likely to point to another buffer and
 * window.
 */
    ଶୂନ୍ୟ
syntax_start(win_T *wp, linenr_T lnum)
{
    synstate_T	*p;
    synstate_T	*last_valid = ଖାଲି;
    synstate_T	*last_min_valid = ଖାଲି;
    synstate_T	*sp, *prev = ଖାଲି;
    linenr_T	parsed_lnum;
    linenr_T	first_stored;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dist;
    ସ୍ଥିର varnumber_T changedtick = 0;	// remember the last change ID

#ifdef FEAT_CONCEAL
    current_sub_char = NUL;
#endif

    /*
     * After switching buffers, invalidate current_state.
     * Also do this when a change was made, the current state may be invalid
     * then.
     */
    ଯଦି (syn_block != wp->w_s
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
    syn_ଗଦା_alloc();
    ଯଦି (syn_block->b_sst_array == ଖାଲି)
	ଫେରନ୍ତୁ;		// out of memory
    syn_block->b_sst_lasttick = display_tick;

    /*
     * If the state of the end of the previous line is useful, store it.
     */
    ଯଦି (VALID_STATE(&current_state)
	    && current_lnum < lnum
	    && current_lnum < syn_buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	(ଶୂନ୍ୟ)syn_finish_ଧାଡ଼ି(FALSE);
	ଯଦି (!current_state_stored)
	{
	    ++current_lnum;
	    (ଶୂନ୍ୟ)store_current_state();
	}

	/*
	 * If the current_lnum is now the same as "lnum", keep the current
	 * state (this happens very often!).  Otherwise invalidate
	 * current_state and figure it out below.
	 */
	ଯଦି (current_lnum != lnum)
	    invalidate_current_state();
    }
    ଅନ୍ୟଥା
	invalidate_current_state();

    /*
     * Try to synchronize from a saved state in b_sst_array[].
     * Only do this if lnum is not before and not to far beyond a saved state.
     */
    ଯଦି (INVALID_STATE(&current_state) && syn_block->b_sst_array != ଖାଲି)
    {
	// Find last valid saved state before start_lnum.
	FOR_ALL_SYNSTATES(syn_block, p)
	{
	    ଯଦି (p->sst_lnum > lnum)
		ଭାଙ୍ଗ;
	    ଯଦି (p->sst_lnum <= lnum && p->sst_change_lnum == 0)
	    {
		last_valid = p;
		ଯଦି (p->sst_lnum >= lnum - syn_block->b_syn_sync_minlines)
		    last_min_valid = p;
	    }
	}
	ଯଦି (last_min_valid != ଖାଲି)
	    load_current_state(last_min_valid);
    }

    /*
     * If "lnum" is before or far beyond a line with a saved state, need to
     * re-synchronize.
     */
    ଯଦି (INVALID_STATE(&current_state))
    {
	syn_sync(wp, lnum, last_valid);
	ଯଦି (current_lnum == 1)
	    // First line is always valid, no matter "minlines".
	    first_stored = 1;
	ଅନ୍ୟଥା
	    // Need to parse "minlines" lines before state can be considered
	    // valid to store.
	    first_stored = current_lnum + syn_block->b_syn_sync_minlines;
    }
    ଅନ୍ୟଥା
	first_stored = current_lnum;

    /*
     * Advance from the sync point or saved state until the current line.
     * Save some entries for syncing with later on.
     */
    ଯଦି (syn_block->b_sst_len <= Rows)
	dist = 999999;
    ଅନ୍ୟଥା
	dist = syn_buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା / (syn_block->b_sst_len - Rows) + 1;
    ଯେତେବେଳେ (current_lnum < lnum)
    {
	syn_start_ଧାଡ଼ି();
	(ଶୂନ୍ୟ)syn_finish_ଧାଡ଼ି(FALSE);
	++current_lnum;

	// If we parsed at least "minlines" lines or started at a valid
	// state, the current state is considered valid.
	ଯଦି (current_lnum >= first_stored)
	{
	    // Check if the saved state entry is for the current line and is
	    // equal to the current state.  If so, then validate all saved
	    // states that depended on a change before the parsed line.
	    ଯଦି (prev == ଖାଲି)
		prev = syn_ଗଦା_find_entry(current_lnum - 1);
	    ଯଦି (prev == ଖାଲି)
		sp = syn_block->b_sst_first;
	    ଅନ୍ୟଥା
		sp = prev;
	    ଯେତେବେଳେ (sp != ଖାଲି && sp->sst_lnum < current_lnum)
		sp = sp->sst_next;
	    ଯଦି (sp != ଖାଲି
		    && sp->sst_lnum == current_lnum
		    && syn_ଗଦା_equal(sp))
	    {
		parsed_lnum = current_lnum;
		prev = sp;
		ଯେତେବେଳେ (sp != ଖାଲି && sp->sst_change_lnum <= parsed_lnum)
		{
		    ଯଦି (sp->sst_lnum <= lnum)
			// valid state before desired line, use this one
			prev = sp;
		    ଅନ୍ୟଥା ଯଦି (sp->sst_change_lnum == 0)
			// past saved states depending on change, break here.
			ଭାଙ୍ଗ;
		    sp->sst_change_lnum = 0;
		    sp = sp->sst_next;
		}
		load_current_state(prev);
	    }
	    // Store the state at this line when it's the first one, the line
	    // where we start parsing, or some distance from the previously
	    // saved state.  But only when parsed at least 'minlines'.
	    ଅନ୍ୟଥା ଯଦି (prev == ଖାଲି
			|| current_lnum == lnum
			|| current_lnum >= prev->sst_lnum + dist)
		prev = store_current_state();
	}

	// This can take a long time: break when CTRL-C pressed.  The current
	// state will be wrong then.
	ଧାଡ଼ି_breakcheck();
	ଯଦି (got_int)
	{
	    current_lnum = lnum;
	    ଭାଙ୍ଗ;
	}
    }

    syn_start_ଧାଡ଼ି();
}

/*
 * We cannot simply discard growarrays full of state_items or buf_states; we
 * have to manually release their extmatch pointers first.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
clear_syn_state(synstate_T *p)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    garray_T	*gap;

    ଯଦି (p->sst_stacksize > SST_FIX_STATES)
    {
	gap = &(p->sst_union.sst_ga);
	ପାଇଁ (i = 0; i < gap->ga_len; i++)
	    unref_extmatch(SYN_STATE_P(gap)[i].bs_extmatch);
	ga_clear(gap);
    }
    ଅନ୍ୟଥା
    {
	ପାଇଁ (i = 0; i < p->sst_stacksize; i++)
	    unref_extmatch(p->sst_union.sst_ଗଦା[i].bs_extmatch);
    }
}

/*
 * Cleanup the current_state stack.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
clear_current_state(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    stateitem_T	*sip;

    sip = (stateitem_T *)(current_state.ga_data);
    ପାଇଁ (i = 0; i < current_state.ga_len; i++)
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
    ସ୍ଥିର ଶୂନ୍ୟ
syn_sync(
    win_T	*wp,
    linenr_T	start_lnum,
    synstate_T	*last_valid)
{
    buf_T	*curbuf_save;
    win_T	*curwin_save;
    pos_T	cursor_save;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    linenr_T	lnum;
    linenr_T	end_lnum;
    linenr_T	break_lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		had_sync_point;
    stateitem_T	*cur_si;
    synpat_T	*spp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found_flags = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found_match_idx = 0;
    linenr_T	found_current_lnum = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found_current_col = 0;
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
    ଯଦି (syn_block->b_syn_sync_minlines > start_lnum)
	start_lnum = 1;
    ଅନ୍ୟଥା
    {
	ଯଦି (syn_block->b_syn_sync_minlines == 1)
	    lnum = 1;
	ଅନ୍ୟଥା ଯଦି (syn_block->b_syn_sync_minlines < 10)
	    lnum = syn_block->b_syn_sync_minlines * 2;
	ଅନ୍ୟଥା
	    lnum = syn_block->b_syn_sync_minlines * 3 / 2;
	ଯଦି (syn_block->b_syn_sync_maxlines != 0
				     && lnum > syn_block->b_syn_sync_maxlines)
	    lnum = syn_block->b_syn_sync_maxlines;
	ଯଦି (lnum >= start_lnum)
	    start_lnum = 1;
	ଅନ୍ୟଥା
	    start_lnum -= lnum;
    }
    current_lnum = start_lnum;

    /*
     * 1. Search backwards for the end of a C-style comment.
     */
    ଯଦି (syn_block->b_syn_sync_flags & SF_CCOMMENT)
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
	ପାଇଁ ( ; start_lnum > 1; --start_lnum)
	{
	    char_u	*l = ml_get(start_lnum - 1);

	    ଯଦି (*l == NUL || *(l + ml_get_len(start_lnum - 1) - 1) != '\\')
		ଭାଙ୍ଗ;
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
	ଯଦି (find_start_comment((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)syn_block->b_syn_sync_maxlines) != ଖାଲି)
	{
	    ପାଇଁ (idx = syn_block->b_syn_patterns.ga_len; --idx >= 0; )
		ଯଦି (SYN_ବସ୍ତୁଗୁଡ଼ିକ(syn_block)[idx].sp_syn.id
						   == syn_block->b_syn_sync_id
			&& SYN_ବସ୍ତୁଗୁଡ଼ିକ(syn_block)[idx].sp_type == SPTYPE_START)
		{
		    validate_current_state();
		    ଯଦି (ଠେଲ_current_state(idx) == OK)
			update_si_attr(current_state.ga_len - 1);
		    ଭାଙ୍ଗ;
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
    ଅନ୍ୟଥା ଯଦି (syn_block->b_syn_sync_flags & SF_MATCH)
    {
	ଯଦି (syn_block->b_syn_sync_maxlines != 0
			       && start_lnum > syn_block->b_syn_sync_maxlines)
	    break_lnum = start_lnum - syn_block->b_syn_sync_maxlines;
	ଅନ୍ୟଥା
	    break_lnum = 0;

	found_m_endpos.lnum = 0;
	found_m_endpos.col = 0;
	end_lnum = start_lnum;
	lnum = start_lnum;
	ଯେତେବେଳେ (--lnum > break_lnum)
	{
	    // This can take a long time: break when CTRL-C pressed.
	    ଧାଡ଼ି_breakcheck();
	    ଯଦି (got_int)
	    {
		invalidate_current_state();
		current_lnum = start_lnum;
		ଭାଙ୍ଗ;
	    }

	    // Check if we have run into a valid saved state stack now.
	    ଯଦି (last_valid != ଖାଲି && lnum == last_valid->sst_lnum)
	    {
		load_current_state(last_valid);
		ଭାଙ୍ଗ;
	    }

	    /*
	     * Check if the previous line has the line-continuation pattern.
	     */
	    ଯଦି (lnum > 1 && syn_match_linecont(lnum - 1))
		ଜାରି;

	    /*
	     * Start with nothing on the state stack
	     */
	    validate_current_state();

	    ପାଇଁ (current_lnum = lnum; current_lnum < end_lnum; ++current_lnum)
	    {
		syn_start_ଧାଡ଼ି();
		ପାଇଁ (;;)
		{
		    had_sync_point = syn_finish_ଧାଡ଼ି(TRUE);
		    /*
		     * When a sync point has been found, remember where, and
		     * continue to look for another one, further on in the line.
		     */
		    ଯଦି (had_sync_point && current_state.ga_len)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			ଯଦି (cur_si->si_m_endpos.lnum > start_lnum)
			{
			    // ignore match that goes to after where started
			    current_lnum = end_lnum;
			    ଭାଙ୍ଗ;
			}
			ଯଦି (cur_si->si_idx < 0)
			{
			    // Cannot happen?
			    found_flags = 0;
			    found_match_idx = KEYWORD_IDX;
			}
			ଅନ୍ୟଥା
			{
			    spp = &(SYN_ବସ୍ତୁଗୁଡ଼ିକ(syn_block)[cur_si->si_idx]);
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
			ଯଦି (found_m_endpos.lnum > current_lnum)
			{
			    current_lnum = found_m_endpos.lnum;
			    current_col = found_m_endpos.col;
			    ଯଦି (current_lnum >= end_lnum)
				ଭାଙ୍ଗ;
			}
			ଅନ୍ୟଥା ଯଦି (found_m_endpos.col > current_col)
			    current_col = found_m_endpos.col;
			ଅନ୍ୟଥା
			    ++current_col;

			// syn_current_attr() will have skipped the check for
			// an item that ends here, need to do that now.  Be
			// careful not to go past the NUL.
			prev_current_col = current_col;
			ଯଦି (syn_getcurline()[current_col] != NUL)
			    ++current_col;
			check_state_ends();
			current_col = prev_current_col;
		    }
		    ଅନ୍ୟଥା
			ଭାଙ୍ଗ;
		}
	    }

	    /*
	     * If a sync point was encountered, break here.
	     */
	    ଯଦି (found_flags)
	    {
		/*
		 * Put the item that was specified by the sync point on the
		 * state stack.  If there was no item specified, make the
		 * state stack empty.
		 */
		clear_current_state();
		ଯଦି (found_match_idx >= 0
			&& ଠେଲ_current_state(found_match_idx) == OK)
		    update_si_attr(current_state.ga_len - 1);

		/*
		 * When using "grouphere", continue from the sync point
		 * match, until the end of the line.  Parsing starts at
		 * the next line.
		 * For "groupthere" the parsing starts at start_lnum.
		 */
		ଯଦି (found_flags & HL_SYNC_HERE)
		{
		    current_lnum = found_m_endpos.lnum;
		    current_col = found_m_endpos.col;
		    ଯଦି (current_state.ga_len)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			cur_si->si_h_startpos.lnum = found_current_lnum;
			cur_si->si_h_startpos.col = found_current_col;
			update_si_end(cur_si, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)current_col, TRUE);
			check_keepend();
		    }
		    (ଶୂନ୍ୟ)syn_finish_ଧାଡ଼ି(FALSE);
		    ++current_lnum;
		}
		ଅନ୍ୟଥା
		    current_lnum = start_lnum;

		ଭାଙ୍ଗ;
	    }

	    end_lnum = lnum;
	    invalidate_current_state();
	}

	// Ran into start of the file or exceeded maximum number of lines
	ଯଦି (lnum <= break_lnum)
	{
	    invalidate_current_state();
	    current_lnum = break_lnum + 1;
	}
    }

    validate_current_state();
}

    ସ୍ଥିର ଶୂନ୍ୟ
save_chartab(char_u *chartab)
{
    ଯଦି (syn_block->b_syn_isk == empty_option)
	ଫେରନ୍ତୁ;

    mch_memmove(chartab, syn_buf->b_chartab, (size_t)32);
    mch_memmove(syn_buf->b_chartab, syn_win->w_s->b_syn_chartab, (size_t)32);
}

    ସ୍ଥିର ଶୂନ୍ୟ
restore_chartab(char_u *chartab)
{
    ଯଦି (syn_win->w_s->b_syn_isk != empty_option)
	mch_memmove(syn_buf->b_chartab, chartab, (size_t)32);
}

/*
 * Return TRUE if the line-continuation pattern matches in line "lnum".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_match_linecont(linenr_T lnum)
{
    regmmatch_T regmatch;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;
    char_u	buf_chartab[32];  // chartab array for syn iskyeyword

    ଯଦି (syn_block->b_syn_linecont_prog == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    // use syntax iskeyword option
    save_chartab(buf_chartab);
    regmatch.rmm_ic = syn_block->b_syn_linecont_ic;
    regmatch.regprog = syn_block->b_syn_linecont_prog;
    r = syn_regexec(&regmatch, lnum, (colnr_T)0,
	    IF_SYN_TIME(&syn_block->b_syn_linecont_time));
    syn_block->b_syn_linecont_prog = regmatch.regprog;
    restore_chartab(buf_chartab);
    ଫେରନ୍ତୁ r;
}

/*
 * Prepare the current state for the start of a line.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_start_ଧାଡ଼ି(ଶୂନ୍ୟ)
{
    current_finished = FALSE;
    current_col = 0;

    /*
     * Need to update the end of a start/skip/end that continues from the
     * previous line and regions that have "keepend".
     */
    ଯଦି (current_state.ga_len > 0)
    {
	syn_update_ends(TRUE);
	check_state_ends();
    }

    next_match_idx = -1;
    ++current_ଧାଡ଼ି_id;
#ifdef FEAT_CONCEAL
    next_seqnr = 1;
#endif
}

/*
 * Check for items in the stack that need their end updated.
 * When "startofline" is TRUE the last item is always updated.
 * When "startofline" is FALSE the item with "keepend" is forcefully updated.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_update_ends(ପୂର୍ଣ୍ଣସଂଖ୍ୟା startofline)
{
    stateitem_T	*cur_si;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		seen_keepend;

    ଯଦି (startofline)
    {
	// Check for a match carried over from a previous line with a
	// contained region.  The match ends as soon as the region ends.
	ପାଇଁ (i = 0; i < current_state.ga_len; ++i)
	{
	    cur_si = &CUR_STATE(i);
	    ଯଦି (cur_si->si_idx >= 0
		    && (SYN_ବସ୍ତୁଗୁଡ଼ିକ(syn_block)[cur_si->si_idx]).sp_type
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
    ଯଦି (keepend_level >= 0)
	ପାଇଁ ( ; i > keepend_level; --i)
	    ଯଦି (CUR_STATE(i).si_flags & HL_EXTEND)
		ଭାଙ୍ଗ;

    seen_keepend = FALSE;
    ପାଇଁ ( ; i < current_state.ga_len; ++i)
    {
	cur_si = &CUR_STATE(i);
	ଯଦି ((cur_si->si_flags & HL_KEEPEND)
			    || (seen_keepend && !startofline)
			    || (i == current_state.ga_len - 1 && startofline))
	{
	    cur_si->si_h_startpos.col = 0;	// start highl. in col 0
	    cur_si->si_h_startpos.lnum = current_lnum;

	    ଯଦି (!(cur_si->si_flags & HL_MATCHCONT))
		update_si_end(cur_si, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)current_col, !startofline);

	    ଯଦି (!startofline && (cur_si->si_flags & HL_KEEPEND))
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

    ସ୍ଥିର ଶୂନ୍ୟ
syn_ଗଦା_free_block(synblock_T *block)
{
    synstate_T	*p;

    // Syntax definitions may have changed: drop the in_id_list() cache.
    idl_cache_clear(block);

    ଯଦି (block->b_sst_array == ଖାଲି)
	ଫେରନ୍ତୁ;

    FOR_ALL_SYNSTATES(block, p)
	clear_syn_state(p);
    VIM_CLEAR(block->b_sst_array);
    block->b_sst_first = ଖାଲି;
    block->b_sst_ସନ୍ଧାନ = ଖାଲି;
    block->b_sst_len = 0;
}
/*
 * Free b_sst_array[] for buffer "buf".
 * Used when syntax items changed to force resyncing everywhere.
 */
    ଶୂନ୍ୟ
syn_ଗଦା_free_all(synblock_T *block)
{
#ifdef FEAT_FOLDING
    win_T	*wp;
#endif

    syn_ଗଦା_free_block(block);

#ifdef FEAT_FOLDING
    // When using "syntax" fold method, must update all folds.
    FOR_ALL_WINDOWS(wp)
    {
	ଯଦି (wp->w_s == block && foldmethodIsSyntax(wp))
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
    ସ୍ଥିର ଶୂନ୍ୟ
syn_ଗଦା_alloc(ଶୂନ୍ୟ)
{
    long	len;
    synstate_T	*to, *from;
    synstate_T	*sstp;

    len = syn_buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା / SST_DIST + Rows * 2;
    ଯଦି (len < SST_MIN_ENTRIES)
	len = SST_MIN_ENTRIES;
    ଅନ୍ୟଥା ଯଦି (len > SST_MAX_ENTRIES)
	len = SST_MAX_ENTRIES;
    ଯଦି (syn_block->b_sst_len > len * 2 || syn_block->b_sst_len < len)
    {
	// Allocate 50% too much, to avoid reallocating too often.
	len = syn_buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	len = (len + len / 2) / SST_DIST + Rows * 2;
	ଯଦି (len < SST_MIN_ENTRIES)
	    len = SST_MIN_ENTRIES;
	ଅନ୍ୟଥା ଯଦି (len > SST_MAX_ENTRIES)
	    len = SST_MAX_ENTRIES;

	ଯଦି (syn_block->b_sst_array != ଖାଲି)
	{
	    // When shrinking the array, cleanup the existing stack.
	    // Make sure that all valid entries fit in the new array.
	    ଯେତେବେଳେ (syn_block->b_sst_len - syn_block->b_sst_freecount + 2 > len
		    && syn_ଗଦା_cleanup())
		;
	    ଯଦି (len < syn_block->b_sst_len - syn_block->b_sst_freecount + 2)
		len = syn_block->b_sst_len - syn_block->b_sst_freecount + 2;
	}

	sstp = ALLOC_CLEAR_MULT(synstate_T, len);
	ଯଦି (sstp == ଖାଲି)	// out of memory!
	    ଫେରନ୍ତୁ;

	to = sstp - 1;
	ଯଦି (syn_block->b_sst_array != ଖାଲି)
	{
	    // Move the states from the old array to the new one.
	    ପାଇଁ (from = syn_block->b_sst_first; from != ଖାଲି;
							from = from->sst_next)
	    {
		++to;
		*to = *from;
		to->sst_next = to + 1;
	    }
	}
	ଯଦି (to != sstp - 1)
	{
	    to->sst_next = ଖାଲି;
	    syn_block->b_sst_first = sstp;
	    syn_block->b_sst_freecount = len - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(to - sstp) - 1;
	}
	ଅନ୍ୟଥା
	{
	    syn_block->b_sst_first = ଖାଲି;
	    syn_block->b_sst_freecount = len;
	}

	// Create the list of free entries.
	syn_block->b_sst_firstfree = to + 1;
	ଯେତେବେଳେ (++to < sstp + len)
	    to->sst_next = to + 1;
	(sstp + len - 1)->sst_next = ଖାଲି;

	vim_free(syn_block->b_sst_array);
	syn_block->b_sst_array = sstp;
	syn_block->b_sst_len = len;
	// The entries were moved to a new array, drop the stale pointer.
	syn_block->b_sst_ସନ୍ଧାନ = ଖାଲି;
    }
}

/*
 * Check for changes in a buffer to affect stored syntax states.  Uses the
 * b_mod_* fields.
 * Called from update_screen(), before screen is being updated, once for each
 * displayed buffer.
 */
    ଶୂନ୍ୟ
syn_ଗଦା_apply_changes(buf_T *buf)
{
    win_T	*wp;

    syn_ଗଦା_apply_changes_block(&buf->b_s, buf);

    FOR_ALL_WINDOWS(wp)
    {
	ଯଦି ((wp->w_buffer == buf) && (wp->w_s != &buf->b_s))
	    syn_ଗଦା_apply_changes_block(wp->w_s, buf);
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
syn_ଗଦା_apply_changes_block(synblock_T *block, buf_T *buf)
{
    synstate_T	*p, *prev, *np;
    linenr_T	n;

    prev = ଖାଲି;
    ପାଇଁ (p = block->b_sst_first; p != ଖାଲି; )
    {
	ଯଦି (p->sst_lnum + block->b_syn_sync_linebreaks > buf->b_mod_ଶୀର୍ଷ)
	{
	    n = p->sst_lnum + buf->b_mod_xlines;
	    ଯଦି (n <= buf->b_mod_bot)
	    {
		// this state is inside the changed area, remove it
		np = p->sst_next;
		ଯଦି (prev == ଖାଲି)
		    block->b_sst_first = np;
		ଅନ୍ୟଥା
		    prev->sst_next = np;
		syn_ଗଦା_free_entry(block, p);
		p = np;
		ଜାରି;
	    }
	    // This state is below the changed area.  Remember the line
	    // that needs to be parsed before this entry can be made valid
	    // again.
	    ଯଦି (p->sst_change_lnum != 0 && p->sst_change_lnum > buf->b_mod_ଶୀର୍ଷ)
	    {
		ଯଦି (p->sst_change_lnum + buf->b_mod_xlines > buf->b_mod_ଶୀର୍ଷ)
		    p->sst_change_lnum += buf->b_mod_xlines;
		ଅନ୍ୟଥା
		    p->sst_change_lnum = buf->b_mod_ଶୀର୍ଷ;
	    }
	    ଯଦି (p->sst_change_lnum == 0
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_ଗଦା_cleanup(ଶୂନ୍ୟ)
{
    synstate_T	*p, *prev;
    disptick_T	tick;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		above;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dist;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FALSE;

    ଯଦି (syn_block->b_sst_first == ଖାଲି)
	ଫେରନ୍ତୁ retval;

    // Compute normal distance between non-displayed entries.
    ଯଦି (syn_block->b_sst_len <= Rows)
	dist = 999999;
    ଅନ୍ୟଥା
	dist = syn_buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା / (syn_block->b_sst_len - Rows) + 1;

    /*
     * Go through the list to find the "tick" for the oldest entry that can
     * be removed.  Set "above" when the "tick" for the oldest entry is above
     * "b_sst_lasttick" (the display tick wraps around).
     */
    tick = syn_block->b_sst_lasttick;
    above = FALSE;
    prev = syn_block->b_sst_first;
    ପାଇଁ (p = prev->sst_next; p != ଖାଲି; prev = p, p = p->sst_next)
    {
	ଯଦି (prev->sst_lnum + dist > p->sst_lnum)
	{
	    ଯଦି (p->sst_tick > syn_block->b_sst_lasttick)
	    {
		ଯଦି (!above || p->sst_tick < tick)
		    tick = p->sst_tick;
		above = TRUE;
	    }
	    ଅନ୍ୟଥା ଯଦି (!above && p->sst_tick < tick)
		tick = p->sst_tick;
	}
    }

    /*
     * Go through the list to make the entries for the oldest tick at an
     * interval of several lines.
     */
    prev = syn_block->b_sst_first;
    ପାଇଁ (p = prev->sst_next; p != ଖାଲି; prev = p, p = p->sst_next)
    {
	ଯଦି (p->sst_tick == tick && prev->sst_lnum + dist > p->sst_lnum)
	{
	    // Move this entry from used list to free list
	    prev->sst_next = p->sst_next;
	    syn_ଗଦା_free_entry(syn_block, p);
	    p = prev;
	    retval = TRUE;
	}
    }
    ଫେରନ୍ତୁ retval;
}

/*
 * Free the allocated memory for a syn_state item.
 * Move the entry into the free list.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_ଗଦା_free_entry(synblock_T *block, synstate_T *p)
{
    ଯଦି (block->b_sst_ସନ୍ଧାନ == p)
	block->b_sst_ସନ୍ଧାନ = ଖାଲି;
    clear_syn_state(p);
    p->sst_next = block->b_sst_firstfree;
    block->b_sst_firstfree = p;
    ++block->b_sst_freecount;
}

/*
 * Find an entry in the list of state stacks at or before "lnum".
 * Returns NULL when there is no entry or the first entry is after "lnum".
 */
    ସ୍ଥିର synstate_T *
syn_ଗଦା_find_entry(linenr_T lnum)
{
    synstate_T	*p, *prev;

    prev = ଖାଲି;
    p = syn_block->b_sst_first;

    // The list is sorted by line number and lookups while parsing advance
    // monotonically, so resume from the last returned entry instead of
    // rescanning from the start whenever it is at or before "lnum".
    ଯଦି (syn_block->b_sst_ସନ୍ଧାନ != ଖାଲି
				 && syn_block->b_sst_ସନ୍ଧାନ->sst_lnum <= lnum)
	p = syn_block->b_sst_ସନ୍ଧାନ;

    ପାଇଁ ( ; p != ଖାଲି; prev = p, p = p->sst_next)
    {
	ଯଦି (p->sst_lnum == lnum)
	{
	    syn_block->b_sst_ସନ୍ଧାନ = p;
	    ଫେରନ୍ତୁ p;
	}
	ଯଦି (p->sst_lnum > lnum)
	    ଭାଙ୍ଗ;
    }
    ଯଦି (prev != ଖାଲି)
	syn_block->b_sst_ସନ୍ଧାନ = prev;
    ଫେରନ୍ତୁ prev;
}

/*
 * Try saving the current state in b_sst_array[].
 * The current state must be valid for the start of the current_lnum line!
 */
    ସ୍ଥିର synstate_T *
store_current_state(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    synstate_T	*p;
    bufstate_T	*bp;
    stateitem_T	*cur_si;
    synstate_T	*sp = syn_ଗଦା_find_entry(current_lnum);

    /*
     * If the current state contains a start or end pattern that continues
     * from the previous line, we can't use it.  Don't store it then.
     */
    ପାଇଁ (i = current_state.ga_len - 1; i >= 0; --i)
    {
	cur_si = &CUR_STATE(i);
	ଯଦି (cur_si->si_h_startpos.lnum >= current_lnum
		|| cur_si->si_m_endpos.lnum >= current_lnum
		|| cur_si->si_h_endpos.lnum >= current_lnum
		|| (cur_si->si_end_idx
		    && cur_si->si_eoe_pos.lnum >= current_lnum))
	    ଭାଙ୍ଗ;
    }
    ଯଦି (i >= 0)
    {
	ଯଦି (sp != ଖାଲି)
	{
	    // find "sp" in the list and remove it
	    ଯଦି (syn_block->b_sst_first == sp)
		// it's the first entry
		syn_block->b_sst_first = sp->sst_next;
	    ଅନ୍ୟଥା
	    {
		// find the entry just before this one to adjust sst_next
		FOR_ALL_SYNSTATES(syn_block, p)
		    ଯଦି (p->sst_next == sp)
			ଭାଙ୍ଗ;
		ଯଦି (p != ଖାଲି)	// just in case
		    p->sst_next = sp->sst_next;
	    }
	    syn_ଗଦା_free_entry(syn_block, sp);
	    sp = ଖାଲି;
	}
    }
    ଅନ୍ୟଥା ଯଦି (sp == ଖାଲି || sp->sst_lnum != current_lnum)
    {
	/*
	 * Add a new entry
	 */
	// If no free items, cleanup the array first.
	ଯଦି (syn_block->b_sst_freecount == 0)
	{
	    (ଶୂନ୍ୟ)syn_ଗଦା_cleanup();
	    // "sp" may have been moved to the freelist now
	    sp = syn_ଗଦା_find_entry(current_lnum);
	}
	// Still no free items?  Must be a strange problem...
	ଯଦି (syn_block->b_sst_freecount == 0)
	    sp = ଖାଲି;
	ଅନ୍ୟଥା
	{
	    // Take the first item from the free list and put it in the used
	    // list, after *sp
	    p = syn_block->b_sst_firstfree;
	    syn_block->b_sst_firstfree = p->sst_next;
	    --syn_block->b_sst_freecount;
	    ଯଦି (sp == ଖାଲି)
	    {
		// Insert in front of the list
		p->sst_next = syn_block->b_sst_first;
		syn_block->b_sst_first = p;
	    }
	    ଅନ୍ୟଥା
	    {
		// insert in list after *sp
		p->sst_next = sp->sst_next;
		sp->sst_next = p;
	    }
	    sp = p;
	    sp->sst_stacksize = 0;
	    sp->sst_lnum = current_lnum;
	    // Resume the next forward lookup from the entry just stored.
	    syn_block->b_sst_ସନ୍ଧାନ = sp;
	}
    }
    ଯଦି (sp != ଖାଲି)
    {
	// When overwriting an existing state stack, clear it first
	clear_syn_state(sp);
	sp->sst_stacksize = current_state.ga_len;
	ଯଦି (current_state.ga_len > SST_FIX_STATES)
	{
	    // Need to clear it, might be something remaining from when the
	    // length was less than SST_FIX_STATES.
	    ga_init2(&sp->sst_union.sst_ga, ଆକାର(bufstate_T), 1);
	    ଯଦି (ga_grow(&sp->sst_union.sst_ga, current_state.ga_len) == FAIL)
		sp->sst_stacksize = 0;
	    ଅନ୍ୟଥା
		sp->sst_union.sst_ga.ga_len = current_state.ga_len;
	    bp = SYN_STATE_P(&(sp->sst_union.sst_ga));
	}
	ଅନ୍ୟଥା
	    bp = sp->sst_union.sst_ଗଦା;
	ପାଇଁ (i = 0; i < sp->sst_stacksize; ++i)
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
    ଫେରନ୍ତୁ sp;
}

/*
 * Copy a state stack from "from" in b_sst_array[] to current_state;
 */
    ସ୍ଥିର ଶୂନ୍ୟ
load_current_state(synstate_T *from)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    bufstate_T	*bp;

    clear_current_state();
    validate_current_state();
    keepend_level = -1;
    ଯଦି (from->sst_stacksize
	    && ga_grow(&current_state, from->sst_stacksize) == OK)
    {
	ଯଦି (from->sst_stacksize > SST_FIX_STATES)
	    bp = SYN_STATE_P(&(from->sst_union.sst_ga));
	ଅନ୍ୟଥା
	    bp = from->sst_union.sst_ଗଦା;
	ପାଇଁ (i = 0; i < from->sst_stacksize; ++i)
	{
	    CUR_STATE(i).si_idx = bp[i].bs_idx;
	    CUR_STATE(i).si_flags = bp[i].bs_flags;
#ifdef FEAT_CONCEAL
	    CUR_STATE(i).si_seqnr = bp[i].bs_seqnr;
	    CUR_STATE(i).si_cchar = bp[i].bs_cchar;
#endif
	    CUR_STATE(i).si_extmatch = ref_extmatch(bp[i].bs_extmatch);
	    ଯଦି (keepend_level < 0 && (CUR_STATE(i).si_flags & HL_KEEPEND))
		keepend_level = i;
	    CUR_STATE(i).si_ends = FALSE;
	    CUR_STATE(i).si_m_lnum = 0;
	    ଯଦି (CUR_STATE(i).si_idx >= 0)
		CUR_STATE(i).si_next_list =
		     (SYN_ବସ୍ତୁଗୁଡ଼ିକ(syn_block)[CUR_STATE(i).si_idx]).sp_next_list;
	    ଅନ୍ୟଥା
		CUR_STATE(i).si_next_list = ଖାଲି;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_ଗଦା_equal(synstate_T *sp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, j;
    bufstate_T	*bp;
    reg_extmatch_T	*six, *bsx;

    // First a quick check if the stacks have the same size end nextlist.
    ଯଦି (sp->sst_stacksize != current_state.ga_len
	    || sp->sst_next_list != current_next_list)
	ଫେରନ୍ତୁ FALSE;

    // Need to compare all states on both stacks.
    ଯଦି (sp->sst_stacksize > SST_FIX_STATES)
	bp = SYN_STATE_P(&(sp->sst_union.sst_ga));
    ଅନ୍ୟଥା
	bp = sp->sst_union.sst_ଗଦା;

    ପାଇଁ (i = current_state.ga_len; --i >= 0; )
    {
	// If the item has another index the state is different.
	ଯଦି (bp[i].bs_idx != CUR_STATE(i).si_idx)
	    ଭାଙ୍ଗ;
	ଯଦି (bp[i].bs_extmatch == CUR_STATE(i).si_extmatch)
	    ଜାରି;
	// When the extmatch pointers are different, the strings in them can
	// still be the same.  Check if the extmatch references are equal.
	bsx = bp[i].bs_extmatch;
	six = CUR_STATE(i).si_extmatch;
	// If one of the extmatch pointers is NULL the states are different.
	ଯଦି (bsx == ଖାଲି || six == ଖାଲି)
	    ଭାଙ୍ଗ;
	ପାଇଁ (j = 0; j < NSUBEXP; ++j)
	{
	    // Check each referenced match string. They must all be equal.
	    ଯଦି (bsx->matches[j] != six->matches[j])
	    {
		// If the pointer is different it can still be the same text.
		// Compare the strings, ignore case when the start item has the
		// sp_ic flag set.
		ଯଦି (bsx->matches[j] == ଖାଲି || six->matches[j] == ଖାଲି)
		    ଭାଙ୍ଗ;
		ଯଦି ((SYN_ବସ୍ତୁଗୁଡ଼ିକ(syn_block)[CUR_STATE(i).si_idx]).sp_ic
			? MB_STRICMP(bsx->matches[j], six->matches[j]) != 0
			: STRCMP(bsx->matches[j], six->matches[j]) != 0)
		    ଭାଙ୍ଗ;
	    }
	}
	ଯଦି (j != NSUBEXP)
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ i < 0 ? TRUE : FALSE;
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
    ଶୂନ୍ୟ
syntax_end_parsing(win_T *wp, linenr_T lnum)
{
    synstate_T	*sp;

    ଯଦି (syn_block != wp->w_s)
	ଫେରନ୍ତୁ;  // not the right window
    sp = syn_ଗଦା_find_entry(lnum);
    ଯଦି (sp != ଖାଲି && sp->sst_lnum < lnum)
	sp = sp->sst_next;

    ଯଦି (sp != ଖାଲି && sp->sst_change_lnum != 0)
	sp->sst_change_lnum = lnum;
}

/*
 * End of handling of the state stack.
 ****************************************/

    ସ୍ଥିର ଶୂନ୍ୟ
invalidate_current_state(ଶୂନ୍ୟ)
{
    clear_current_state();
    current_state.ga_itemsize = 0;	// mark current_state invalid
    current_next_list = ଖାଲି;
    keepend_level = -1;
}

    ସ୍ଥିର ଶୂନ୍ୟ
validate_current_state(ଶୂନ୍ୟ)
{
    current_state.ga_itemsize = ଆକାର(stateitem_T);
    current_state.ga_growsize = 3;
}

/*
 * Return TRUE if the syntax at start of lnum changed since last time.
 * This will only be called just after get_syntax_attr() for the previous
 * line, to check if the next line needs to be redrawn too.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syntax_check_changed(linenr_T lnum)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = TRUE;
    synstate_T	*sp;

    /*
     * Check the state stack when:
     * - lnum is just below the previously syntaxed line.
     * - lnum is not before the lines with saved states.
     * - lnum is not past the lines with saved states.
     * - lnum is at or before the last changed line.
     */
    ଯଦି (VALID_STATE(&current_state) && lnum == current_lnum + 1)
    {
	sp = syn_ଗଦା_find_entry(lnum);
	ଯଦି (sp != ଖାଲି && sp->sst_lnum == lnum)
	{
	    /*
	     * finish the previous line (needed when not all of the line was
	     * drawn)
	     */
	    (ଶୂନ୍ୟ)syn_finish_ଧାଡ଼ି(FALSE);

	    /*
	     * Compare the current state with the previously saved state of
	     * the line.
	     */
	    ଯଦି (syn_ଗଦା_equal(sp))
		retval = FALSE;

	    /*
	     * Store the current state in b_sst_array[] for later use.
	     */
	    ++current_lnum;
	    (ଶୂନ୍ୟ)store_current_state();
	}
    }

    ଫେରନ୍ତୁ retval;
}

/*
 * Finish the current line.
 * This doesn't return any attributes, it only gets the state at the end of
 * the line.  It can start anywhere in the line, as long as the current state
 * is valid.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_finish_ଧାଡ଼ି(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    syncing)		// called for syncing
{
    stateitem_T	*cur_si;
    colnr_T	prev_current_col;

    ଯେତେବେଳେ (!current_finished)
    {
	(ଶୂନ୍ୟ)syn_current_attr(syncing, FALSE, ଖାଲି, FALSE);
	/*
	 * When syncing, and found some item, need to check the item.
	 */
	ଯଦି (syncing && current_state.ga_len)
	{
	    /*
	     * Check for match with sync item.
	     */
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	    ଯଦି (cur_si->si_idx >= 0
		    && (SYN_ବସ୍ତୁଗୁଡ଼ିକ(syn_block)[cur_si->si_idx].sp_flags
					  & (HL_SYNC_HERE|HL_SYNC_THERE)))
		ଫେରନ୍ତୁ TRUE;

	    // syn_current_attr() will have skipped the check for an item
	    // that ends here, need to do that now.  Be careful not to go
	    // past the NUL.
	    prev_current_col = current_col;
	    ଯଦି (syn_getcurline()[current_col] != NUL)
		++current_col;
	    check_state_ends();
	    current_col = prev_current_col;
	}
	++current_col;
    }
    ଫେରନ୍ତୁ FALSE;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_syntax_attr(
    colnr_T	col,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*can_spell,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		keep_state)	// keep state of char at "col"
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    attr = 0;

    ଯଦି (can_spell != ଖାଲି)
	// Default: Only do spelling when there is no @Spell cluster or when
	// ":syn spell toplevel" was used.
	*can_spell = syn_block->b_syn_spell == SYNSPL_DEFAULT
		    ? (syn_block->b_spell_cluster_id == 0)
		    : (syn_block->b_syn_spell == SYNSPL_ଶୀର୍ଷ);

    // check for out of memory situation
    ଯଦି (syn_block->b_sst_array == ଖାଲି)
	ଫେରନ୍ତୁ 0;

    // After 'synmaxcol' the attribute is always zero.
    ଯଦି (syn_buf->b_p_smc > 0 && col >= (colnr_T)syn_buf->b_p_smc)
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
	ଫେରନ୍ତୁ 0;
    }

    // Make sure current_state is valid
    ଯଦି (INVALID_STATE(&current_state))
	validate_current_state();

    /*
     * Skip from the current column to "col", get the attributes for "col".
     */
    ଯେତେବେଳେ (current_col <= col)
    {
	attr = syn_current_attr(FALSE, TRUE, can_spell,
				     current_col == col ? keep_state : FALSE);
	++current_col;
    }

    ଫେରନ୍ତୁ attr;
}

/*
 * Get syntax attributes for current_lnum, current_col.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_current_attr(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		syncing,		// When 1: called for syncing
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		displaying,		// result will be displayed
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*can_spell,		// return: do spell checking
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		keep_state)		// keep syntax stack afterwards
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		syn_id;
    lpos_T	endpos;		// was: char_u *endp;
    lpos_T	hl_startpos;	// was: int hl_startcol;
    lpos_T	hl_endpos;
    lpos_T	eos_pos;	// end-of-start match (start region)
    lpos_T	eoe_pos;	// end-of-end pattern
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		end_idx;	// group ID for end pattern
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    synpat_T	*spp;
    stateitem_T	*cur_si, *sip = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		startcol;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		endcol;
    long	flags;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cchar;
    short	*next_list;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found_match;		    // found usable match
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	try_next_column = FALSE;    // must try in next col
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_keywords;
    regmmatch_T	regmatch;
    lpos_T	pos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lc_col;
    reg_extmatch_T *cur_extmatch = ଖାଲି;
    char_u	buf_chartab[32];  // chartab array for syn iskyeyword
    char_u	*ଧାଡ଼ି;		// current line.  NOTE: becomes invalid after
				// looking for a pattern match!

    // variables for zero-width matches that have a "nextgroup" argument
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		keep_next_list;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		zero_width_next_list = FALSE;
    garray_T	zero_width_next_ga;

    /*
     * No character, no attributes!  Past end of line?
     * Do try matching with an empty line (could be the start of a region).
     */
    ଧାଡ଼ି = syn_getcurline();
    ଯଦି (ଧାଡ଼ି[current_col] == NUL && current_col != 0)
    {
	/*
	 * If we found a match after the last column, use it.
	 */
	ଯଦି (next_match_idx >= 0 && next_match_col >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)current_col
						  && next_match_col != MAXCOL)
	    (ଶୂନ୍ୟ)ଠେଲ_next_match(ଖାଲି);

	current_finished = TRUE;
	current_state_stored = FALSE;
	ଫେରନ୍ତୁ 0;
    }

    // if the current or next character is NUL, we will finish the line now
    ଯଦି (ଧାଡ଼ି[current_col] == NUL || ଧାଡ଼ି[current_col + 1] == NUL)
    {
	current_finished = TRUE;
	current_state_stored = FALSE;
    }

    /*
     * When in the previous column there was a match but it could not be used
     * (empty match or already matched in this column) need to try again in
     * the next column.
     */
    ଯଦି (try_next_column)
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
    ga_init2(&zero_width_next_ga, ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା), 10);

    // use syntax iskeyword option
    save_chartab(buf_chartab);

    /*
     * Repeat matching keywords and patterns, to find contained items at the
     * same column.  This stops when there are no extra matches at the current
     * column.
     */
    କର
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
	ଯଦି (current_state.ga_len)
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	ଅନ୍ୟଥା
	    cur_si = ଖାଲି;

	ଯଦି (syn_block->b_syn_containedin || cur_si == ଖାଲି
					      || cur_si->si_cont_list != ଖାଲି)
	{
	    /*
	     * 2. Check for keywords, if on a keyword char after a non-keyword
	     *	  char.  Don't do this when syncing.
	     */
	    ଯଦି (do_keywords)
	    {
	      ଧାଡ଼ି = syn_getcurline();
	      ଯଦି (vim_iswordp_buf(ଧାଡ଼ି + current_col, syn_buf)
		      && (current_col == 0
			  || !vim_iswordp_buf(ଧାଡ଼ି + current_col - 1
			      - (has_mbyte
				  ? (*mb_head_off)(ଧାଡ଼ି, ଧାଡ଼ି + current_col - 1)
				  : 0) , syn_buf)))
	      {
		syn_id = check_keyword_id(ଧାଡ଼ି, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)current_col,
					 &endcol, &flags, &next_list, cur_si,
					 &cchar);
		ଯଦି (syn_id != 0)
		{
		    ଯଦି (ଠେଲ_current_state(KEYWORD_IDX) == OK)
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
			ଯଦି (current_state.ga_len > 1)
			    cur_si->si_flags |=
				  CUR_STATE(current_state.ga_len - 2).si_flags
								 & HL_CONCEAL;
#endif
			cur_si->si_id = syn_id;
			cur_si->si_trans_id = syn_id;
			ଯଦି (flags & HL_TRANSP)
			{
			    ଯଦି (current_state.ga_len < 2)
			    {
				cur_si->si_attr = 0;
				cur_si->si_trans_id = 0;
			    }
			    ଅନ୍ୟଥା
			    {
				cur_si->si_attr = CUR_STATE(
					current_state.ga_len - 2).si_attr;
				cur_si->si_trans_id = CUR_STATE(
					current_state.ga_len - 2).si_trans_id;
			    }
			}
			ଅନ୍ୟଥା
			    cur_si->si_attr = syn_id2attr(syn_id);
			cur_si->si_cont_list = ଖାଲି;
			cur_si->si_next_list = next_list;
			check_keepend();
		    }
		    ଅନ୍ୟଥା
			vim_free(next_list);
		}
	      }
	    }

	    /*
	     * 3. Check for patterns (only if no keyword found).
	     */
	    ଯଦି (syn_id == 0 && syn_block->b_syn_patterns.ga_len)
	    {
		/*
		 * If we didn't check for a match yet, or we are past it, check
		 * for any match with a pattern.
		 */
		ଯଦି (next_match_idx < 0 || next_match_col < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)current_col)
		{
		    /*
		     * Check all relevant patterns for a match at this
		     * position.  This is complicated, because matching with a
		     * pattern takes quite a bit of time, thus we want to
		     * avoid doing it when it's not needed.
		     */
		    next_match_idx = 0;		// no match in this line yet
		    next_match_col = MAXCOL;
		    ପାଇଁ (idx = syn_block->b_syn_patterns.ga_len; --idx >= 0; )
		    {
			spp = &(SYN_ବସ୍ତୁଗୁଡ଼ିକ(syn_block)[idx]);
			ଯଦି (	   spp->sp_syncing == syncing
				&& (displaying || !(spp->sp_flags & HL_DISPLAY))
				&& (spp->sp_type == SPTYPE_MATCH
				    || spp->sp_type == SPTYPE_START)
				&& (current_next_list != ଖାଲି
				    ? in_id_list(ଖାଲି, current_next_list,
							      &spp->sp_syn, 0)
				    : (cur_si == ଖାଲି
					? !(spp->sp_flags & HL_CONTAINED)
					: in_id_list(cur_si,
					    cur_si->si_cont_list, &spp->sp_syn,
					    spp->sp_flags))))
			{
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

			    // If we already tried matching in this line, and
			    // there isn't a match before next_match_col, skip
			    // this item.
			    ଯଦି (spp->sp_ଧାଡ଼ି_id == current_ଧାଡ଼ି_id
				    && spp->sp_startcol >= next_match_col)
				ଜାରି;
			    spp->sp_ଧାଡ଼ି_id = current_ଧାଡ଼ି_id;

			    lc_col = current_col - spp->sp_offsets[SPO_LC_OFF];
			    ଯଦି (lc_col < 0)
				lc_col = 0;

			    regmatch.rmm_ic = spp->sp_ic;
			    regmatch.regprog = spp->sp_prog;
			    r = syn_regexec(&regmatch,
					     current_lnum,
					     (colnr_T)lc_col,
					     IF_SYN_TIME(&spp->sp_time));
			    spp->sp_prog = regmatch.regprog;
			    ଯଦି (!r)
			    {
				// no match in this line, try another one
				spp->sp_startcol = MAXCOL;
				ଜାରି;
			    }

			    /*
			     * Compute the first column of the match.
			     */
			    syn_add_start_off(&pos, &regmatch,
							 spp, SPO_MS_OFF, -1);
			    ଯଦି (pos.lnum > current_lnum)
			    {
				// must have used end of match in a next line,
				// we can't handle that
				spp->sp_startcol = MAXCOL;
				ଜାରି;
			    }
			    startcol = pos.col;

			    // remember the next column where this pattern
			    // matches in the current line
			    spp->sp_startcol = startcol;

			    /*
			     * If a previously found match starts at a lower
			     * column number, don't use this one.
			     */
			    ଯଦି (startcol >= next_match_col)
				ଜାରି;

			    /*
			     * If we matched this pattern at this position
			     * before, skip it.  Must retry in the next
			     * column, because it may match from there.
			     */
			    ଯଦି (did_match_already(idx, &zero_width_next_ga))
			    {
				try_next_column = TRUE;
				ଜାରି;
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
			    re_extmatch_out = ଖାଲି;

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
			    ଯଦି (spp->sp_type == SPTYPE_START
					      && (spp->sp_flags & HL_ONELINE))
			    {
				lpos_T	startpos;

				startpos = endpos;
				find_endpos(idx, &startpos, &endpos, &hl_endpos,
				    &flags, &eoe_pos, &end_idx, cur_extmatch);
				ଯଦି (endpos.lnum == 0)
				    ଜାରି;	    // not found
			    }

			    /*
			     * For a "match" the size must be > 0 after the
			     * end offset needs has been added.  Except when
			     * syncing.
			     */
			    ଅନ୍ୟଥା ଯଦି (spp->sp_type == SPTYPE_MATCH)
			    {
				syn_add_end_off(&hl_endpos, &regmatch, spp,
							       SPO_HE_OFF, 0);
				syn_add_end_off(&endpos, &regmatch, spp,
							       SPO_ME_OFF, 0);
				ଯଦି (endpos.lnum == current_lnum
				      && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)endpos.col + syncing < startcol)
				{
				    /*
				     * If an empty string is matched, may need
				     * to try matching again at next column.
				     */
				    ଯଦି (regmatch.startpos[0].col
						    == regmatch.endpos[0].col)
					try_next_column = TRUE;
				    ଜାରି;
				}
			    }

			    /*
			     * keep the best match so far in next_match_*
			     */
			    // Highlighting must start after startpos and end
			    // before endpos.
			    ଯଦି (hl_startpos.lnum == current_lnum
					   && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)hl_startpos.col < startcol)
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
			    cur_extmatch = ଖାଲି;
			}
		    }
		}

		/*
		 * If we found a match at the current column, use it.
		 */
		ଯଦି (next_match_idx >= 0 && next_match_col == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)current_col)
		{
		    synpat_T	*lspp;

		    // When a zero-width item matched which has a nextgroup,
		    // don't push the item but set nextgroup.
		    lspp = &(SYN_ବସ୍ତୁଗୁଡ଼ିକ(syn_block)[next_match_idx]);
		    ଯଦି (next_match_m_endpos.lnum == current_lnum
			    && next_match_m_endpos.col == current_col
			    && lspp->sp_next_list != ଖାଲି)
		    {
			current_next_list = lspp->sp_next_list;
			current_next_flags = lspp->sp_flags;
			keep_next_list = TRUE;
			zero_width_next_list = TRUE;

			// Add the index to a list, so that we can check
			// later that we don't match it again (and cause an
			// endless loop).
			ଯଦି (ga_grow(&zero_width_next_ga, 1) == OK)
			{
			    ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)(zero_width_next_ga.ga_data))
				[zero_width_next_ga.ga_len++] = next_match_idx;
			}
			next_match_idx = -1;
		    }
		    ଅନ୍ୟଥା
			cur_si = ଠେଲ_next_match(cur_si);
		    found_match = TRUE;
		}
	    }
	}

	/*
	 * Handle searching for nextgroup match.
	 */
	ଯଦି (current_next_list != ଖାଲି && !keep_next_list)
	{
	    /*
	     * If a nextgroup was not found, continue looking for one if:
	     * - this is an empty line and the "skipempty" option was given
	     * - we are on white space and the "skipwhite" option was given
	     */
	    ଯଦି (!found_match)
	    {
		ଧାଡ଼ି = syn_getcurline();
		ଯଦି (((current_next_flags & HL_SKIPWHITE)
			    && VIM_ISWHITE(ଧାଡ଼ି[current_col]))
			|| ((current_next_flags & HL_SKIPEMPTY)
			    && *ଧାଡ଼ି == NUL))
		    ଭାଙ୍ଗ;
	    }

	    /*
	     * If a nextgroup was found: Use it, and continue looking for
	     * contained matches.
	     * If a nextgroup was not found: Continue looking for a normal
	     * match.
	     * When did set current_next_list for a zero-width item and no
	     * match was found don't loop (would get stuck).
	     */
	    current_next_list = ଖାଲି;
	    next_match_idx = -1;
	    ଯଦି (!zero_width_next_list)
		found_match = TRUE;
	}

    } ଯେତେବେଳେ (found_match);

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
    ଯଦି (cur_si != ଖାଲି)
    {
#ifndef FEAT_EVAL
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	current_trans_id = 0;
#endif
	ପାଇଁ (idx = current_state.ga_len - 1; idx >= 0; --idx)
	{
	    sip = &CUR_STATE(idx);
	    ଯଦି ((current_lnum > sip->si_h_startpos.lnum
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
		ଭାଙ୍ଗ;
	    }
	}

	ଯଦି (can_spell != ଖାଲି)
	{
	    ସଂରଚନା sp_syn   sps;

	    /*
	     * set "can_spell" to TRUE if spell checking is supposed to be
	     * done in the current item.
	     */
	    ଯଦି (syn_block->b_spell_cluster_id == 0)
	    {
		// There is no @Spell cluster: Do spelling for items without
		// @NoSpell cluster.
		ଯଦି (syn_block->b_nospell_cluster_id == 0
						     || current_trans_id == 0)
		    *can_spell = (syn_block->b_syn_spell != SYNSPL_NOTOP);
		ଅନ୍ୟଥା
		{
		    sps.inc_tag = 0;
		    sps.id = syn_block->b_nospell_cluster_id;
		    sps.cont_in_list = ଖାଲି;
		    *can_spell = !in_id_list(sip, sip->si_cont_list, &sps, 0);
		}
	    }
	    ଅନ୍ୟଥା
	    {
		// The @Spell cluster is defined: Do spelling in items with
		// the @Spell cluster.  But not when @NoSpell is also there.
		// At the toplevel only spell check when ":syn spell toplevel"
		// was used.
		ଯଦି (current_trans_id == 0)
		    *can_spell = (syn_block->b_syn_spell == SYNSPL_ଶୀର୍ଷ);
		ଅନ୍ୟଥା
		{
		    sps.inc_tag = 0;
		    sps.id = syn_block->b_spell_cluster_id;
		    sps.cont_in_list = ଖାଲି;
		    *can_spell = in_id_list(sip, sip->si_cont_list, &sps, 0);

		    ଯଦି (syn_block->b_nospell_cluster_id != 0)
		    {
			sps.id = syn_block->b_nospell_cluster_id;
			ଯଦି (in_id_list(sip, sip->si_cont_list, &sps, 0))
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
	ଯଦି (!syncing && !keep_state)
	{
	    check_state_ends();
	    ଯଦି (current_state.ga_len > 0
				      && syn_getcurline()[current_col] != NUL)
	    {
		++current_col;
		check_state_ends();
		--current_col;
	    }
	}
    }
    ଅନ୍ୟଥା ଯଦି (can_spell != ଖାଲି)
	// Default: Only do spelling when there is no @Spell cluster or when
	// ":syn spell toplevel" was used.
	*can_spell = syn_block->b_syn_spell == SYNSPL_DEFAULT
		    ? (syn_block->b_spell_cluster_id == 0)
		    : (syn_block->b_syn_spell == SYNSPL_ଶୀର୍ଷ);

    // nextgroup ends at end of line, unless "skipnl" or "skipempty" present
    ଯଦି (current_next_list != ଖାଲି
	    && (ଧାଡ଼ି = syn_getcurline())[current_col] != NUL
	    && ଧାଡ଼ି[current_col + 1] == NUL
	    && !(current_next_flags & (HL_SKIPNL | HL_SKIPEMPTY)))
	current_next_list = ଖାଲି;

    ଯଦି (zero_width_next_ga.ga_len > 0)
	ga_clear(&zero_width_next_ga);

    // No longer need external matches.  But keep next_match_extmatch.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = ଖାଲି;
    unref_extmatch(cur_extmatch);

    ଫେରନ୍ତୁ current_attr;
}


/*
 * Check if we already matched pattern "idx" at the current column.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
did_match_already(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, garray_T *gap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = current_state.ga_len; --i >= 0; )
	ଯଦି (CUR_STATE(i).si_m_startcol == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)current_col
		&& CUR_STATE(i).si_m_lnum == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)current_lnum
		&& CUR_STATE(i).si_idx == idx)
	    ଫେରନ୍ତୁ TRUE;

    // Zero-width matches with a nextgroup argument are not put on the syntax
    // stack, and can only be matched once anyway.
    ପାଇଁ (i = gap->ga_len; --i >= 0; )
	ଯଦି (((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)(gap->ga_data))[i] == idx)
	    ଫେରନ୍ତୁ TRUE;

    ଫେରନ୍ତୁ FALSE;
}

/*
 * Push the next match onto the stack.
 */
    ସ୍ଥିର stateitem_T *
ଠେଲ_next_match(stateitem_T *cur_si)
{
    synpat_T	*spp;
#ifdef FEAT_CONCEAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		 save_flags;
#endif

    spp = &(SYN_ବସ୍ତୁଗୁଡ଼ିକ(syn_block)[next_match_idx]);

    /*
     * Push the item in current_state stack;
     */
    ଯଦି (ଠେଲ_current_state(next_match_idx) == OK)
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
	ଯଦି (current_state.ga_len > 1)
	    cur_si->si_flags |=
		    CUR_STATE(current_state.ga_len - 2).si_flags & HL_CONCEAL;
#endif
	cur_si->si_next_list = spp->sp_next_list;
	cur_si->si_extmatch = ref_extmatch(next_match_extmatch);
	ଯଦି (spp->sp_type == SPTYPE_START && !(spp->sp_flags & HL_ONELINE))
	{
	    // Try to find the end pattern in the current line
	    update_si_end(cur_si, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(next_match_m_endpos.col), TRUE);
	    check_keepend();
	}
	ଅନ୍ୟଥା
	{
	    cur_si->si_m_endpos = next_match_m_endpos;
	    cur_si->si_h_endpos = next_match_h_endpos;
	    cur_si->si_ends = TRUE;
	    cur_si->si_flags |= next_match_flags;
	    cur_si->si_eoe_pos = next_match_eoe_pos;
	    cur_si->si_end_idx = next_match_end_idx;
	}
	ଯଦି (keepend_level < 0 && (cur_si->si_flags & HL_KEEPEND))
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
	ଯଦି (	   spp->sp_type == SPTYPE_START
		&& spp->sp_syn_match_id != 0
		&& ଠେଲ_current_state(next_match_idx) == OK)
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
	    ଯଦି (cur_si->si_flags & HL_CONCEALENDS)
		cur_si->si_flags |= HL_CONCEAL;
#endif
	    cur_si->si_next_list = ଖାଲି;
	    check_keepend();
	    update_si_attr(current_state.ga_len - 1);
	}
    }

    next_match_idx = -1;	// try other match next time

    ଫେରନ୍ତୁ cur_si;
}

/*
 * Check for end of current state (and the states before it).
 */
    ସ୍ଥିର ଶୂନ୍ୟ
check_state_ends(ଶୂନ୍ୟ)
{
    stateitem_T	*cur_si;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		had_extend;

    cur_si = &CUR_STATE(current_state.ga_len - 1);
    ପାଇଁ (;;)
    {
	ଯଦି (cur_si->si_ends
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
	    ଯଦି (cur_si->si_end_idx
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
		ଯଦି (cur_si->si_flags & HL_CONCEALENDS)
		    cur_si->si_flags |= HL_CONCEAL;
#endif
		update_si_attr(current_state.ga_len - 1);

		// nextgroup= should not match in the end pattern
		current_next_list = ଖାଲି;

		// what matches next may be different now, clear it
		next_match_idx = 0;
		next_match_col = MAXCOL;
		ଭାଙ୍ଗ;
	    }

	    // handle next_list, unless at end of line and no "skipnl" or
	    // "skipempty"
	    current_next_list = cur_si->si_next_list;
	    current_next_flags = cur_si->si_flags;
	    ଯଦି (!(current_next_flags & (HL_SKIPNL | HL_SKIPEMPTY))
		    && syn_getcurline()[current_col] == NUL)
		current_next_list = ଖାଲି;

	    // When the ended item has "extend", another item with
	    // "keepend" now needs to check for its end.
	    had_extend = (cur_si->si_flags & HL_EXTEND);

	    କାଢ଼_current_state();

	    ଯଦି (current_state.ga_len == 0)
		ଭାଙ୍ଗ;

	    ଯଦି (had_extend && keepend_level >= 0)
	    {
		syn_update_ends(FALSE);
		ଯଦି (current_state.ga_len == 0)
		    ଭାଙ୍ଗ;
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
	    ଯଦି (cur_si->si_idx >= 0
		    && SYN_ବସ୍ତୁଗୁଡ଼ିକ(syn_block)[cur_si->si_idx].sp_type
								== SPTYPE_START
		    && !(cur_si->si_flags & (HL_MATCH | HL_KEEPEND)))
	    {
		update_si_end(cur_si, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)current_col, TRUE);
		check_keepend();
		ଯଦି ((current_next_flags & HL_HAS_EOL)
			&& keepend_level < 0
			&& syn_getcurline()[current_col] == NUL)
		    ଭାଙ୍ଗ;
	    }
	}
	ଅନ୍ୟଥା
	    ଭାଙ୍ଗ;
    }
}

/*
 * Update an entry in the current_state stack for a match or region.  This
 * fills in si_attr, si_next_list and si_cont_list.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
update_si_attr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    stateitem_T	*sip = &CUR_STATE(idx);
    synpat_T	*spp;

    // This should not happen...
    ଯଦି (sip->si_idx < 0)
	ଫେରନ୍ତୁ;

    spp = &(SYN_ବସ୍ତୁଗୁଡ଼ିକ(syn_block)[sip->si_idx]);
    ଯଦି (sip->si_flags & HL_MATCH)
	sip->si_id = spp->sp_syn_match_id;
    ଅନ୍ୟଥା
	sip->si_id = spp->sp_syn.id;
    sip->si_attr = syn_id2attr(sip->si_id);
    sip->si_trans_id = sip->si_id;
    ଯଦି (sip->si_flags & HL_MATCH)
	sip->si_cont_list = ଖାଲି;
    ଅନ୍ୟଥା
	sip->si_cont_list = spp->sp_cont_list;

    /*
     * For transparent items, take attr from outer item.
     * Also take cont_list, if there is none.
     * Don't do this for the matchgroup of a start or end pattern.
     */
    ଯଦି ((spp->sp_flags & HL_TRANSP) && !(sip->si_flags & HL_MATCH))
    {
	ଯଦି (idx == 0)
	{
	    sip->si_attr = 0;
	    sip->si_trans_id = 0;
	    ଯଦି (sip->si_cont_list == ଖାଲି)
		sip->si_cont_list = ID_LIST_ALL;
	}
	ଅନ୍ୟଥା
	{
	    sip->si_attr = CUR_STATE(idx - 1).si_attr;
	    sip->si_trans_id = CUR_STATE(idx - 1).si_trans_id;
	    ଯଦି (sip->si_cont_list == ଖାଲି)
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
    ସ୍ଥିର ଶୂନ୍ୟ
check_keepend(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    lpos_T	maxpos;
    lpos_T	maxpos_h;
    stateitem_T	*sip;

    /*
     * This check can consume a lot of time; only do it from the level where
     * there really is a keepend.
     */
    ଯଦି (keepend_level < 0)
	ଫେରନ୍ତୁ;

    /*
     * Find the last index of an "extend" item.  "keepend" items before that
     * won't do anything.  If there is no "extend" item "i" will be
     * "keepend_level" and all "keepend" items will work normally.
     */
    ପାଇଁ (i = current_state.ga_len - 1; i > keepend_level; --i)
	ଯଦି (CUR_STATE(i).si_flags & HL_EXTEND)
	    ଭାଙ୍ଗ;

    maxpos.lnum = 0;
    maxpos.col = 0;
    maxpos_h.lnum = 0;
    maxpos_h.col = 0;
    ପାଇଁ ( ; i < current_state.ga_len; ++i)
    {
	sip = &CUR_STATE(i);
	ଯଦି (maxpos.lnum != 0)
	{
	    limit_pos_zero(&sip->si_m_endpos, &maxpos);
	    limit_pos_zero(&sip->si_h_endpos, &maxpos_h);
	    limit_pos_zero(&sip->si_eoe_pos, &maxpos);
	    sip->si_ends = TRUE;
	}
	ଯଦି (sip->si_ends && (sip->si_flags & HL_KEEPEND))
	{
	    ଯଦି (maxpos.lnum == 0
		    || maxpos.lnum > sip->si_m_endpos.lnum
		    || (maxpos.lnum == sip->si_m_endpos.lnum
			&& maxpos.col > sip->si_m_endpos.col))
		maxpos = sip->si_m_endpos;
	    ଯଦି (maxpos_h.lnum == 0
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
    ସ୍ଥିର ଶୂନ୍ୟ
update_si_end(
    stateitem_T	*sip,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		startcol,   // where to start searching for the end
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		force)	    // when TRUE overrule a previous end
{
    lpos_T	startpos;
    lpos_T	endpos;
    lpos_T	hl_endpos;
    lpos_T	end_endpos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		end_idx;

    // return quickly for a keyword
    ଯଦି (sip->si_idx < 0)
	ଫେରନ୍ତୁ;

    // Don't update when it's already done.  Can be a match of an end pattern
    // that started in a previous line.  Watch out: can also be a "keepend"
    // from a containing item.
    ଯଦି (!force && sip->si_m_endpos.lnum >= current_lnum)
	ଫେରନ୍ତୁ;

    /*
     * We need to find the end of the region.  It may continue in the next
     * line.
     */
    end_idx = 0;
    startpos.lnum = current_lnum;
    startpos.col = startcol;
    find_endpos(sip->si_idx, &startpos, &endpos, &hl_endpos,
		   &(sip->si_flags), &end_endpos, &end_idx, sip->si_extmatch);

    ଯଦି (endpos.lnum == 0)
    {
	// No end pattern matched.
	ଯଦି (SYN_ବସ୍ତୁଗୁଡ଼ିକ(syn_block)[sip->si_idx].sp_flags & HL_ONELINE)
	{
	    // a "oneline" never continues in the next line
	    sip->si_ends = TRUE;
	    sip->si_m_endpos.lnum = current_lnum;
	    sip->si_m_endpos.col = syn_getcurline_len();
	}
	ଅନ୍ୟଥା
	{
	    // continues in the next line
	    sip->si_ends = FALSE;
	    sip->si_m_endpos.lnum = 0;
	}
	sip->si_h_endpos = sip->si_m_endpos;
    }
    ଅନ୍ୟଥା
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ଠେଲ_current_state(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (ga_grow(&current_state, 1) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    CLEAR_POINTER(&CUR_STATE(current_state.ga_len));
    CUR_STATE(current_state.ga_len).si_idx = idx;
    ++current_state.ga_len;
    ଫେରନ୍ତୁ OK;
}

/*
 * Remove a state from the current_state stack.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
କାଢ଼_current_state(ଶୂନ୍ୟ)
{
    ଯଦି (current_state.ga_len)
    {
	unref_extmatch(CUR_STATE(current_state.ga_len - 1).si_extmatch);
	--current_state.ga_len;
    }
    // after the end of a pattern, try matching a keyword or pattern
    next_match_idx = -1;

    // if first state with "keepend" is popped, reset keepend_level
    ଯଦି (keepend_level >= current_state.ga_len)
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
    ସ୍ଥିର ଶୂନ୍ୟ
find_endpos(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx,		// index of the pattern
    lpos_T	*startpos,	// where to start looking for an END match
    lpos_T	*m_endpos,	// return: end of match
    lpos_T	*hl_endpos,	// return: end of highlighting
    long	*flagsp,	// return: flags of matching END
    lpos_T	*end_endpos,	// return: end of end pattern match
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*end_idx,	// return: group ID for end pat. match, or 0
    reg_extmatch_T *start_ext)	// submatches from the start pattern
{
    colnr_T	matchcol;
    synpat_T	*spp, *spp_skip;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start_idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		best_idx;
    regmmatch_T	regmatch;
    regmmatch_T	best_regmatch;	    // startpos/endpos of best match
    lpos_T	pos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		had_match = FALSE;
    char_u	buf_chartab[32];  // chartab array for syn option iskyeyword

    // just in case we are invoked for a keyword
    ଯଦି (idx < 0)
	ଫେରନ୍ତୁ;

    /*
     * Check for being called with a START pattern.
     * Can happen with a match that continues to the next line, because it
     * contained a region.
     */
    spp = &(SYN_ବସ୍ତୁଗୁଡ଼ିକ(syn_block)[idx]);
    ଯଦି (spp->sp_type != SPTYPE_START)
    {
	*hl_endpos = *startpos;
	ଫେରନ୍ତୁ;
    }

    /*
     * Find the SKIP or first END pattern after the last START pattern.
     */
    ପାଇଁ (;;)
    {
	spp = &(SYN_ବସ୍ତୁଗୁଡ଼ିକ(syn_block)[idx]);
	ଯଦି (spp->sp_type != SPTYPE_START)
	    ଭାଙ୍ଗ;
	++idx;
    }

    /*
     *	Lookup the SKIP pattern (if present)
     */
    ଯଦି (spp->sp_type == SPTYPE_SKIP)
    {
	spp_skip = spp;
	++idx;
    }
    ଅନ୍ୟଥା
	spp_skip = ଖାଲି;

    // Setup external matches for syn_regexec().
    unref_extmatch(re_extmatch_in);
    re_extmatch_in = ref_extmatch(start_ext);

    matchcol = startpos->col;	// start looking for a match at sstart
    start_idx = idx;		// remember the first END pattern.
    best_regmatch.startpos[0].col = 0;		// avoid compiler warning

    // use syntax iskeyword option
    save_chartab(buf_chartab);

    ପାଇଁ (;;)
    {
	/*
	 * Find end pattern that matches first after "matchcol".
	 */
	best_idx = -1;
	ପାଇଁ (idx = start_idx; idx < syn_block->b_syn_patterns.ga_len; ++idx)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lc_col = matchcol;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

	    spp = &(SYN_ବସ୍ତୁଗୁଡ଼ିକ(syn_block)[idx]);
	    ଯଦି (spp->sp_type != SPTYPE_END)	// past last END pattern
		ଭାଙ୍ଗ;
	    lc_col -= spp->sp_offsets[SPO_LC_OFF];
	    ଯଦି (lc_col < 0)
		lc_col = 0;

	    regmatch.rmm_ic = spp->sp_ic;
	    regmatch.regprog = spp->sp_prog;
	    r = syn_regexec(&regmatch, startpos->lnum, lc_col,
						  IF_SYN_TIME(&spp->sp_time));
	    spp->sp_prog = regmatch.regprog;
	    ଯଦି (r)
	    {
		ଯଦି (best_idx == -1 || regmatch.startpos[0].col
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
	ଯଦି (best_idx == -1)
	    ଭାଙ୍ଗ;

	/*
	 * If the skip pattern matches before the end pattern,
	 * continue searching after the skip pattern.
	 */
	ଯଦି (spp_skip != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lc_col = matchcol - spp_skip->sp_offsets[SPO_LC_OFF];
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

	    ଯଦି (lc_col < 0)
		lc_col = 0;
	    regmatch.rmm_ic = spp_skip->sp_ic;
	    regmatch.regprog = spp_skip->sp_prog;
	    r = syn_regexec(&regmatch, startpos->lnum, lc_col,
					      IF_SYN_TIME(&spp_skip->sp_time));
	    spp_skip->sp_prog = regmatch.regprog;
	    ଯଦି (r && regmatch.startpos[0].col
					     <= best_regmatch.startpos[0].col)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି_len;

		// Add offset to skip pattern match
		syn_add_end_off(&pos, &regmatch, spp_skip, SPO_ME_OFF, 1);

		// If the skip pattern goes on to the next line, there is no
		// match with an end pattern in this line.
		ଯଦି (pos.lnum > startpos->lnum)
		    ଭାଙ୍ଗ;

		ଧାଡ଼ି_len = ml_get_buf_len(syn_buf, startpos->lnum);

		// take care of an empty match or negative offset
		ଯଦି (pos.col <= matchcol)
		    ++matchcol;
		ଅନ୍ୟଥା ଯଦି (pos.col <= regmatch.endpos[0].col)
		    matchcol = pos.col;
		ଅନ୍ୟଥା
		    // Be careful not to jump over the NUL at the end-of-line
		    ପାଇଁ (matchcol = regmatch.endpos[0].col;
			    matchcol < ଧାଡ଼ି_len && matchcol < pos.col;
								   ++matchcol)
			;

		// if the skip pattern includes end-of-line, break here
		ଯଦି (matchcol >= ଧାଡ଼ି_len)
		    ଭାଙ୍ଗ;

		ଜାରି;	    // start with first end pattern again
	    }
	}

	/*
	 * Match from start pattern to end pattern.
	 * Correct for match and highlight offset of end pattern.
	 */
	spp = &(SYN_ବସ୍ତୁଗୁଡ଼ିକ(syn_block)[best_idx]);
	syn_add_end_off(m_endpos, &best_regmatch, spp, SPO_ME_OFF, 1);
	// can't end before the start
	ଯଦି (m_endpos->lnum == startpos->lnum && m_endpos->col < startpos->col)
	    m_endpos->col = startpos->col;

	syn_add_end_off(end_endpos, &best_regmatch, spp, SPO_HE_OFF, 1);
	// can't end before the start
	ଯଦି (end_endpos->lnum == startpos->lnum
					   && end_endpos->col < startpos->col)
	    end_endpos->col = startpos->col;
	// can't end after the match
	limit_pos(end_endpos, m_endpos);

	/*
	 * If the end group is highlighted differently, adjust the pointers.
	 */
	ଯଦି (spp->sp_syn_match_id != spp->sp_syn.id && spp->sp_syn_match_id != 0)
	{
	    *end_idx = best_idx;
	    ଯଦି (spp->sp_off_flags & (1 << (SPO_RE_OFF + SPO_ଗଣନା)))
	    {
		hl_endpos->lnum = best_regmatch.endpos[0].lnum;
		hl_endpos->col = best_regmatch.endpos[0].col;
	    }
	    ଅନ୍ୟଥା
	    {
		hl_endpos->lnum = best_regmatch.startpos[0].lnum;
		hl_endpos->col = best_regmatch.startpos[0].col;
	    }
	    hl_endpos->col += spp->sp_offsets[SPO_RE_OFF];

	    // can't end before the start
	    ଯଦି (hl_endpos->lnum == startpos->lnum
					    && hl_endpos->col < startpos->col)
		hl_endpos->col = startpos->col;
	    limit_pos(hl_endpos, m_endpos);

	    // now the match ends where the highlighting ends, it is turned
	    // into the matchgroup for the end
	    *m_endpos = *hl_endpos;
	}
	ଅନ୍ୟଥା
	{
	    *end_idx = 0;
	    *hl_endpos = *end_endpos;
	}

	*flagsp = spp->sp_flags;

	had_match = TRUE;
	ଭାଙ୍ଗ;
    }

    // no match for an END pattern in this line
    ଯଦି (!had_match)
	m_endpos->lnum = 0;

    restore_chartab(buf_chartab);

    // Remove external matches.
    unref_extmatch(re_extmatch_in);
    re_extmatch_in = ଖାଲି;
}

/*
 * Limit "pos" not to be after "limit".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
limit_pos(lpos_T *pos, lpos_T *limit)
{
    ଯଦି (pos->lnum > limit->lnum)
	*pos = *limit;
    ଅନ୍ୟଥା ଯଦି (pos->lnum == limit->lnum && pos->col > limit->col)
	pos->col = limit->col;
}

/*
 * Limit "pos" not to be after "limit", unless pos->lnum is zero.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
limit_pos_zero(
    lpos_T	*pos,
    lpos_T	*limit)
{
    ଯଦି (pos->lnum == 0)
	*pos = *limit;
    ଅନ୍ୟଥା
	limit_pos(pos, limit);
}

/*
 * Add offset to matched text for end of match or highlight.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_add_end_off(
    lpos_T	*result,	// returned position
    regmmatch_T	*regmatch,	// start/end of match
    synpat_T	*spp,		// matched pattern
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx,		// index of offset
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		extra)		// extra chars for offset to start
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		off;
    char_u	*base;
    char_u	*p;

    ଯଦି (spp->sp_off_flags & (1 << idx))
    {
	result->lnum = regmatch->startpos[0].lnum;
	col = regmatch->startpos[0].col;
	off = spp->sp_offsets[idx] + extra;
    }
    ଅନ୍ୟଥା
    {
	result->lnum = regmatch->endpos[0].lnum;
	col = regmatch->endpos[0].col;
	off = spp->sp_offsets[idx];
    }
    // Don't go past the end of the line.  Matters for "rs=e+2" when there
    // is a matchgroup. Watch out for match with last NL in the buffer.
    ଯଦି (result->lnum > syn_buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	col = 0;
    ଅନ୍ୟଥା ଯଦି (off != 0)
    {
	base = ml_get_buf(syn_buf, result->lnum, FALSE);
	p = base + col;
	ଯଦି (off > 0)
	{
	    ଯେତେବେଳେ (off-- > 0 && *p != NUL)
		MB_PTR_ADV(p);
	}
	ଅନ୍ୟଥା ଯଦି (off < 0)
	{
	    ଯେତେବେଳେ (off++ < 0 && base < p)
		MB_PTR_BACK(base, p);
	}
	col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - base);
    }
    result->col = col;
}

/*
 * Add offset to matched text for start of match or highlight.
 * Avoid resulting column to become negative.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_add_start_off(
    lpos_T	*result,	// returned position
    regmmatch_T	*regmatch,	// start/end of match
    synpat_T	*spp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		extra)	    // extra chars for offset to end
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		off;
    char_u	*base;
    char_u	*p;

    ଯଦି (spp->sp_off_flags & (1 << (idx + SPO_ଗଣନା)))
    {
	result->lnum = regmatch->endpos[0].lnum;
	col = regmatch->endpos[0].col;
	off = spp->sp_offsets[idx] + extra;
    }
    ଅନ୍ୟଥା
    {
	result->lnum = regmatch->startpos[0].lnum;
	col = regmatch->startpos[0].col;
	off = spp->sp_offsets[idx];
    }
    ଯଦି (result->lnum > syn_buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	// a "\n" at the end of the pattern may take us below the last line
	result->lnum = syn_buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	col = ml_get_buf_len(syn_buf, result->lnum);
    }
    ଯଦି (off != 0)
    {
	base = ml_get_buf(syn_buf, result->lnum, FALSE);
	p = base + col;
	ଯଦି (off > 0)
	{
	    ଯେତେବେଳେ (off-- && *p != NUL)
		MB_PTR_ADV(p);
	}
	ଅନ୍ୟଥା ଯଦି (off < 0)
	{
	    ଯେତେବେଳେ (off++ && base < p)
		MB_PTR_BACK(base, p);
	}
	col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - base);
    }
    result->col = col;
}

/*
 * Get current line in syntax buffer.
 */
    ସ୍ଥିର char_u *
syn_getcurline(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ ml_get_buf(syn_buf, current_lnum, FALSE);
}

/*
 * Get length of current line in syntax buffer.
 */
    ସ୍ଥିର colnr_T
syn_getcurline_len(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ ml_get_buf_len(syn_buf, current_lnum);
}

/*
 * Call vim_regexec() to find a match with "rmp" in "syn_buf".
 * Returns TRUE when there is a match.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_regexec(
    regmmatch_T	*rmp,
    linenr_T	lnum,
    colnr_T	col,
    syn_time_T  *st UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		timed_out = FALSE;
#ifdef FEAT_PROFILE
    proftime_T	pt;

    ଯଦି (syn_time_on)
	profile_start(&pt);
#endif

    ଯଦି (rmp->regprog == ଖାଲି)
	// This can happen if a previous call to vim_regexec_multi() tried to
	// use the NFA engine, which resulted in NFA_TOO_EXPENSIVE, and
	// compiling the pattern with the other engine fails.
	ଫେରନ୍ତୁ FALSE;

    rmp->rmm_maxcol = syn_buf->b_p_smc;
    r = vim_regexec_multi(rmp, syn_win, syn_buf, lnum, col, &timed_out);

#ifdef FEAT_PROFILE
    ଯଦି (syn_time_on)
    {
	profile_end(&pt);
	profile_add(&st->total, &pt);
	ଯଦି (profile_cmp(&pt, &st->slowest) < 0)
	    st->slowest = pt;
	++st->ଗଣନା_2;
	ଯଦି (r > 0)
	    ++st->match;
    }
#endif
#ifdef FEAT_RELTIME
    ଯଦି (timed_out && redrawtime_limit_set && !syn_win->w_s->b_syn_slow)
    {
	syn_win->w_s->b_syn_slow = TRUE;
	ସନ୍ଦେଶ(_("'redrawtime' exceeded, syntax highlighting disabled"));
    }
#endif

    ଯଦି (r > 0)
    {
	rmp->startpos[0].lnum += lnum;
	rmp->endpos[0].lnum += lnum;
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Check one position in a line for a matching keyword.
 * The caller must check if a keyword can start at startcol.
 * Return its ID if found, 0 otherwise.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_keyword_id(
    char_u	*ଧାଡ଼ି,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		startcol,	// position in line to check for keyword
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*endcolp,	// return: character after found keyword
    long	*flagsp,	// return: flags of matching keyword
    short	**next_listp,	// return: next_list of matching keyword
    stateitem_T	*cur_si,	// item at the top of the stack
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ccharp UNUSED)	// conceal substitution char
{
    keyentry_T	*kp;
    char_u	*kwp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		round;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		kwlen;
    char_u	keyword[MAXKEYWLEN + 1]; // assume max. keyword len is 80
    hashtab_T	*ht;
    hashitem_T	*hi;

    // Find first character after the keyword.  First character was already
    // checked.
    kwp = ଧାଡ଼ି + startcol;
    kwlen = 0;
    କର
    {
	ଯଦି (has_mbyte)
	    kwlen += (*mb_ptr2len)(kwp + kwlen);
	ଅନ୍ୟଥା
	    ++kwlen;
    }
    ଯେତେବେଳେ (vim_iswordp_buf(kwp + kwlen, syn_buf));

    ଯଦି (kwlen > MAXKEYWLEN)
	ଫେରନ୍ତୁ 0;

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
    ପାଇଁ (round = 1; round <= 2; ++round)
    {
	ht = round == 1 ? &syn_block->b_keywtab : &syn_block->b_keywtab_ic;
	ଯଦି (ht->ht_used == 0)
	    ଜାରି;
	ଯଦି (round == 2)	// ignore case
	    (ଶୂନ୍ୟ)str_foldcase(kwp, kwlen, keyword, MAXKEYWLEN + 1);

	/*
	 * Find keywords that match.  There can be several with different
	 * attributes.
	 * When current_next_list is non-zero accept only that group, otherwise:
	 *  Accept a not-contained keyword at toplevel.
	 *  Accept a keyword at other levels only if it is in the contains list.
	 */
	hi = hash_find(ht, keyword);
	ଯଦି (!HASHITEM_EMPTY(hi))
	    ପାଇଁ (kp = HI2KE(hi); kp != ଖାଲି; kp = kp->ke_next)
	    {
		ଯଦି (current_next_list != 0
			? in_id_list(ଖାଲି, current_next_list, &kp->k_syn, 0)
			: (cur_si == ଖାଲି
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
		    ଫେରନ୍ତୁ kp->k_syn.id;
		}
	    }
    }
    ଫେରନ୍ତୁ 0;
}

/*
 * Handle ":syntax conceal" command.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_cmd_conceal(exarg_T *eap UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା syncing UNUSED)
{
#ifdef FEAT_CONCEAL
    char_u	*arg = eap->arg;
    char_u	*next;

    eap->nextcmd = find_nextcmd(arg);
    ଯଦି (eap->skip)
	ଫେରନ୍ତୁ;

    next = skiptowhite(arg);
    ଯଦି (*arg == NUL)
    {
	ଯଦି (curwin->w_s->b_syn_conceal)
	    ସନ୍ଦେଶ("syntax conceal on");
	ଅନ୍ୟଥା
	    ସନ୍ଦେଶ("syntax conceal off");
    }
    ଅନ୍ୟଥା ଯଦି (STRNICMP(arg, "on", 2) == 0 && next - arg == 2)
	curwin->w_s->b_syn_conceal = TRUE;
    ଅନ୍ୟଥା ଯଦି (STRNICMP(arg, "off", 3) == 0 && next - arg == 3)
	curwin->w_s->b_syn_conceal = FALSE;
    ଅନ୍ୟଥା
	semsg(_(e_illegal_argument_str_2), arg);
#endif
}

/*
 * Handle ":syntax case" command.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_cmd_case(exarg_T *eap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*next;

    eap->nextcmd = find_nextcmd(arg);
    ଯଦି (eap->skip)
	ଫେରନ୍ତୁ;

    next = skiptowhite(arg);
    ଯଦି (*arg == NUL)
    {
	ଯଦି (curwin->w_s->b_syn_ic)
	    ସନ୍ଦେଶ("syntax case ignore");
	ଅନ୍ୟଥା
	    ସନ୍ଦେଶ("syntax case match");
    }
    ଅନ୍ୟଥା ଯଦି (STRNICMP(arg, "match", 5) == 0 && next - arg == 5)
	curwin->w_s->b_syn_ic = FALSE;
    ଅନ୍ୟଥା ଯଦି (STRNICMP(arg, "ignore", 6) == 0 && next - arg == 6)
	curwin->w_s->b_syn_ic = TRUE;
    ଅନ୍ୟଥା
	semsg(_(e_illegal_argument_str_2), arg);
}

/*
 * Handle ":syntax foldlevel" command.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_cmd_foldlevel(exarg_T *eap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା syncing UNUSED)
{
    char_u *arg = eap->arg;
    char_u *arg_end;

    eap->nextcmd = find_nextcmd(arg);
    ଯଦି (eap->skip)
	ଫେରନ୍ତୁ;

    ଯଦି (*arg == NUL)
    {
	ବାଛ (curwin->w_s->b_syn_foldlevel)
	{
	    ସ୍ଥିତି SYNFLD_START:   ସନ୍ଦେଶ("syntax foldlevel start");   ଭାଙ୍ଗ;
	    ସ୍ଥିତି SYNFLD_MINIMUM: ସନ୍ଦେଶ("syntax foldlevel minimum"); ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଭାଙ୍ଗ;
	}
	ଫେରନ୍ତୁ;
    }

    arg_end = skiptowhite(arg);
    ଯଦି (STRNICMP(arg, "start", 5) == 0 && arg_end - arg == 5)
	curwin->w_s->b_syn_foldlevel = SYNFLD_START;
    ଅନ୍ୟଥା ଯଦି (STRNICMP(arg, "minimum", 7) == 0 && arg_end - arg == 7)
	curwin->w_s->b_syn_foldlevel = SYNFLD_MINIMUM;
    ଅନ୍ୟଥା
    {
	semsg(_(e_illegal_argument_str_2), arg);
	ଫେରନ୍ତୁ;
    }

    arg = skipwhite(arg_end);
    ଯଦି (*arg != NUL)
    {
	semsg(_(e_illegal_argument_str_2), arg);
    }
}

/*
 * Handle ":syntax spell" command.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_cmd_spell(exarg_T *eap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*next;

    eap->nextcmd = find_nextcmd(arg);
    ଯଦି (eap->skip)
	ଫେରନ୍ତୁ;

    next = skiptowhite(arg);
    ଯଦି (*arg == NUL)
    {
	ଯଦି (curwin->w_s->b_syn_spell == SYNSPL_ଶୀର୍ଷ)
	    ସନ୍ଦେଶ("syntax spell toplevel");
	ଅନ୍ୟଥା ଯଦି (curwin->w_s->b_syn_spell == SYNSPL_NOTOP)
	    ସନ୍ଦେଶ("syntax spell notoplevel");
	ଅନ୍ୟଥା
	    ସନ୍ଦେଶ("syntax spell default");
    }
    ଅନ୍ୟଥା ଯଦି (STRNICMP(arg, "toplevel", 8) == 0 && next - arg == 8)
	curwin->w_s->b_syn_spell = SYNSPL_ଶୀର୍ଷ;
    ଅନ୍ୟଥା ଯଦି (STRNICMP(arg, "notoplevel", 10) == 0 && next - arg == 10)
	curwin->w_s->b_syn_spell = SYNSPL_NOTOP;
    ଅନ୍ୟଥା ଯଦି (STRNICMP(arg, "default", 7) == 0 && next - arg == 7)
	curwin->w_s->b_syn_spell = SYNSPL_DEFAULT;
    ଅନ୍ୟଥା
    {
	semsg(_(e_illegal_argument_str_2), arg);
	ଫେରନ୍ତୁ;
    }

    // assume spell checking changed, force a redraw
    redraw_win_later(curwin, UPD_NOT_VALID);
}

/*
 * Handle ":syntax iskeyword" command.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_cmd_iskeyword(exarg_T *eap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	save_chartab[32];
    char_u	*save_isk;

    ଯଦି (eap->skip)
	ଫେରନ୍ତୁ;

    arg = skipwhite(arg);
    ଯଦି (*arg == NUL)
    {
	ସନ୍ଦେଶ_puts("\n");
	ଯଦି (curwin->w_s->b_syn_isk != empty_option)
	{
	    ସନ୍ଦେଶ_puts("syntax iskeyword ");
	    ସନ୍ଦେଶ_outtrans(curwin->w_s->b_syn_isk);
	}
	ଅନ୍ୟଥା
	    ସନ୍ଦେଶ_outtrans((char_u *)_("syntax iskeyword not set"));
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (STRNICMP(arg, "clear", 5) == 0)
	{
	    mch_memmove(curwin->w_s->b_syn_chartab, curbuf->b_chartab,
								  (size_t)32);
	    clear_string_option(&curwin->w_s->b_syn_isk);
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (check_isopt(arg) == FAIL)
	    {
		emsg(_(e_invalid_argument));
		ଫେରନ୍ତୁ;
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
    ଶୂନ୍ୟ
syntax_clear(synblock_T *block)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

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
    ପାଇଁ (i = block->b_syn_patterns.ga_len; --i >= 0; )
	syn_clear_pattern(block, i);
    ga_clear(&block->b_syn_patterns);

    // free the syntax clusters
    ପାଇଁ (i = block->b_syn_clusters.ga_len; --i >= 0; )
	syn_clear_cluster(block, i);
    ga_clear(&block->b_syn_clusters);
    block->b_spell_cluster_id = 0;
    block->b_nospell_cluster_id = 0;

    block->b_syn_sync_flags = 0;
    block->b_syn_sync_minlines = 0;
    block->b_syn_sync_maxlines = 0;
    block->b_syn_sync_linebreaks = 0;

    vim_regfree(block->b_syn_linecont_prog);
    block->b_syn_linecont_prog = ଖାଲି;
    VIM_CLEAR(block->b_syn_linecont_pat);
#ifdef FEAT_FOLDING
    block->b_syn_folditems = 0;
#endif
    clear_string_option(&block->b_syn_isk);

    // free the stored states
    syn_ଗଦା_free_all(block);
    invalidate_current_state();

    // Reset the counter for ":syn include"
    running_syn_inc_tag = 0;
}

/*
 * Get rid of ownsyntax for window "wp".
 */
    ଶୂନ୍ୟ
reset_synblock(win_T *wp)
{
    ଯଦି (wp->w_s != &wp->w_buffer->b_s)
    {
	syntax_clear(wp->w_s);
	vim_free(wp->w_s);
	wp->w_s = &wp->w_buffer->b_s;
    }
}

/*
 * Clear syncing info for one buffer.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syntax_sync_clear(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    // free the syntax patterns
    ପାଇଁ (i = curwin->w_s->b_syn_patterns.ga_len; --i >= 0; )
	ଯଦି (SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[i].sp_syncing)
	    syn_remove_pattern(curwin->w_s, i);

    curwin->w_s->b_syn_sync_flags = 0;
    curwin->w_s->b_syn_sync_minlines = 0;
    curwin->w_s->b_syn_sync_maxlines = 0;
    curwin->w_s->b_syn_sync_linebreaks = 0;

    vim_regfree(curwin->w_s->b_syn_linecont_prog);
    curwin->w_s->b_syn_linecont_prog = ଖାଲି;
    VIM_CLEAR(curwin->w_s->b_syn_linecont_pat);
    clear_string_option(&curwin->w_s->b_syn_isk);

    syn_ଗଦା_free_all(curwin->w_s);	// Need to recompute all syntax.
}

/*
 * Remove one pattern from the buffer's pattern list.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_remove_pattern(
    synblock_T	*block,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx)
{
    synpat_T	*spp;

    spp = &(SYN_ବସ୍ତୁଗୁଡ଼ିକ(block)[idx]);
#ifdef FEAT_FOLDING
    ଯଦି (spp->sp_flags & HL_FOLD)
	--block->b_syn_folditems;
#endif
    syn_clear_pattern(block, idx);
    mch_memmove(spp, spp + 1,
		   ଆକାର(synpat_T) * (block->b_syn_patterns.ga_len - idx - 1));
    --block->b_syn_patterns.ga_len;
}

/*
 * Clear and free one syntax pattern.  When clearing all, must be called from
 * last to first!
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_clear_pattern(synblock_T *block, ପୂର୍ଣ୍ଣସଂଖ୍ୟା i)
{
    vim_free(SYN_ବସ୍ତୁଗୁଡ଼ିକ(block)[i].sp_pattern);
    vim_regfree(SYN_ବସ୍ତୁଗୁଡ଼ିକ(block)[i].sp_prog);
    // Only free sp_cont_list and sp_next_list of first start pattern
    ଯଦି (i == 0 || SYN_ବସ୍ତୁଗୁଡ଼ିକ(block)[i - 1].sp_type != SPTYPE_START)
    {
	vim_free(SYN_ବସ୍ତୁଗୁଡ଼ିକ(block)[i].sp_cont_list);
	vim_free(SYN_ବସ୍ତୁଗୁଡ଼ିକ(block)[i].sp_next_list);
	vim_free(SYN_ବସ୍ତୁଗୁଡ଼ିକ(block)[i].sp_syn.cont_in_list);
    }
}

/*
 * Clear and free one syntax cluster.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_clear_cluster(synblock_T *block, ପୂର୍ଣ୍ଣସଂଖ୍ୟା i)
{
    vim_free(SYN_CLSTR(block)[i].scl_name);
    vim_free(SYN_CLSTR(block)[i].scl_name_u);
    vim_free(SYN_CLSTR(block)[i].scl_list);
}

/*
 * Handle ":syntax clear" command.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_cmd_clear(exarg_T *eap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା syncing)
{
    char_u	*arg = eap->arg;
    char_u	*arg_end;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;

    eap->nextcmd = find_nextcmd(arg);
    ଯଦି (eap->skip)
	ଫେରନ୍ତୁ;

    /*
     * We have to disable this within ":syn include @group filename",
     * because otherwise @group would get deleted.
     * Only required for Vim 5.x syntax files, 6.0 ones don't contain ":syn
     * clear".
     */
    ଯଦି (curwin->w_s->b_syn_topgrp != 0)
	ଫେରନ୍ତୁ;

    ଯଦି (ends_excmd2(eap->cmd, arg))
    {
	/*
	 * No argument: Clear all syntax items.
	 */
	ଯଦି (syncing)
	    syntax_sync_clear();
	ଅନ୍ୟଥା
	{
	    syntax_clear(curwin->w_s);
	    ଯଦି (curwin->w_s == &curwin->w_buffer->b_s)
		do_unlet((char_u *)"b:current_syntax", TRUE);
	    do_unlet((char_u *)"w:current_syntax", TRUE);
	}
    }
    ଅନ୍ୟଥା
    {
	/*
	 * Clear the group IDs that are in the argument.
	 */
	ଯେତେବେଳେ (!ends_excmd2(eap->cmd, arg))
	{
	    arg_end = skiptowhite(arg);
	    ଯଦି (*arg == '@')
	    {
		id = syn_scl_namen2id(arg + 1, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(arg_end - arg - 1));
		ଯଦି (id == 0)
		{
		    semsg(_(e_no_such_syntax_cluster_str_1), arg);
		    ଭାଙ୍ଗ;
		}
		ଅନ୍ୟଥା
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
	    ଅନ୍ୟଥା
	    {
		id = syn_namen2id(arg, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(arg_end - arg));
		ଯଦି (id == 0)
		{
		    semsg(_(e_no_such_highlight_group_name_str), arg);
		    ଭାଙ୍ଗ;
		}
		ଅନ୍ୟଥା
		    syn_clear_one(id, syncing);
	    }
	    arg = skipwhite(arg_end);
	}
    }
    redraw_curbuf_later(UPD_SOME_VALID);
    syn_ଗଦା_free_all(curwin->w_s);		// Need to recompute all syntax.
}

/*
 * Clear one syntax group for the current buffer.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_clear_one(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id, ପୂର୍ଣ୍ଣସଂଖ୍ୟା syncing)
{
    synpat_T	*spp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;

    // Clear keywords only when not ":syn sync clear group-name"
    ଯଦି (!syncing)
    {
	(ଶୂନ୍ୟ)syn_clear_keyword(id, &curwin->w_s->b_keywtab);
	(ଶୂନ୍ୟ)syn_clear_keyword(id, &curwin->w_s->b_keywtab_ic);
    }

    // clear the patterns for "id"
    ପାଇଁ (idx = curwin->w_s->b_syn_patterns.ga_len; --idx >= 0; )
    {
	spp = &(SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx]);
	ଯଦି (spp->sp_syn.id != id || spp->sp_syncing != syncing)
	    ଜାରି;
	syn_remove_pattern(curwin->w_s, idx);
    }
}

/*
 * Handle ":syntax on" command.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_cmd_on(exarg_T *eap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା syncing UNUSED)
{
    syn_cmd_onoff(eap, "syntax");
}

/*
 * Handle ":syntax enable" command.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_cmd_enable(exarg_T *eap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା syncing UNUSED)
{
    set_internal_string_var((char_u *)"g:syntax_cmd", (char_u *)"enable");
    syn_cmd_onoff(eap, "syntax");
    do_unlet((char_u *)"g:syntax_cmd", TRUE);
}

/*
 * Handle ":syntax reset" command.
 * It actually resets highlighting, not syntax.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_cmd_reset(exarg_T *eap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା syncing UNUSED)
{
    set_nextcmd(eap, eap->arg);
    ଯଦି (!eap->skip)
    {
	set_internal_string_var((char_u *)"g:syntax_cmd", (char_u *)"reset");
	do_cmdline_cmd((char_u *)"runtime! syntax/syncolor.vim");
	do_unlet((char_u *)"g:syntax_cmd", TRUE);
    }
}

/*
 * Handle ":syntax manual" command.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_cmd_manual(exarg_T *eap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା syncing UNUSED)
{
    syn_cmd_onoff(eap, "manual");
}

/*
 * Handle ":syntax off" command.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_cmd_off(exarg_T *eap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା syncing UNUSED)
{
    syn_cmd_onoff(eap, "nosyntax");
}

    ସ୍ଥିର ଶୂନ୍ୟ
syn_cmd_onoff(exarg_T *eap, ଅକ୍ଷର *name)
{
    char_u	buf[100];

    set_nextcmd(eap, eap->arg);
    ଯଦି (!eap->skip)
    {
	STRCPY(buf, "so ");
	vim_snprintf((ଅକ୍ଷର *)buf + 3, ଆକାର(buf) - 3, SYNTAX_FNAME, name);
	do_cmdline_cmd(buf);
    }
}

/*
 * Handle ":syntax [list]" command: list current syntax words.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_cmd_list(
    exarg_T	*eap,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		syncing)	    // when TRUE: list syncing items
{
    char_u	*arg = eap->arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
    char_u	*arg_end;

    eap->nextcmd = find_nextcmd(arg);
    ଯଦି (eap->skip)
	ଫେରନ୍ତୁ;

    ଯଦି (!syntax_present(curwin))
    {
	ସନ୍ଦେଶ(_(ସନ୍ଦେଶ_no_ବସ୍ତୁଗୁଡ଼ିକ));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (syncing)
    {
	ଯଦି (curwin->w_s->b_syn_sync_flags & SF_CCOMMENT)
	{
	    ସନ୍ଦେଶ_puts(_("syncing on C-style comments"));
	    syn_ଧାଡ଼ିଗୁଡ଼ିକ_ସନ୍ଦେଶ();
	    syn_match_ସନ୍ଦେଶ();
	}
	ଅନ୍ୟଥା ଯଦି (curwin->w_s->b_syn_sync_flags & SF_MATCH)
	{
	    ସନ୍ଦେଶ_puts_title(_("\n--- Syntax sync items ---"));
	    ଯଦି (curwin->w_s->b_syn_sync_minlines > 0
		|| curwin->w_s->b_syn_sync_maxlines > 0
		|| curwin->w_s->b_syn_sync_linebreaks > 0)
	    {
		ସନ୍ଦେଶ_puts(_("\nsyncing on items"));
		syn_ଧାଡ଼ିଗୁଡ଼ିକ_ସନ୍ଦେଶ();
		syn_match_ସନ୍ଦେଶ();
	    }
	    ପାଇଁ (id = 1; id <= highlight_ଅଙ୍କ_groups() && !got_int; ++id)
		syn_list_one(id, syncing, FALSE);
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (curwin->w_s->b_syn_sync_minlines == 0)
		ସନ୍ଦେଶ_puts(_("no syncing"));
	    ଅନ୍ୟଥା
	    {
		ଯଦି (curwin->w_s->b_syn_sync_minlines == MAXLNUM)
		    ସନ୍ଦେଶ_puts(_("syncing starts at the first line"));
		ଅନ୍ୟଥା
		{
		    ସନ୍ଦେଶ_puts(_("syncing starts "));
		    ସନ୍ଦେଶ_outnum(curwin->w_s->b_syn_sync_minlines);
		    ସନ୍ଦେଶ_puts(_(" lines before top line"));
		}
		syn_match_ସନ୍ଦେଶ();
	    }
	}
	ଫେରନ୍ତୁ;
    }

    ସନ୍ଦେଶ_puts_title(_("\n--- Syntax items ---"));
    ଯଦି (ends_excmd2(eap->cmd, arg))
    {
	/*
	 * No argument: List all group IDs and all syntax clusters.
	 */
	ପାଇଁ (id = 1; id <= highlight_ଅଙ୍କ_groups() && !got_int; ++id)
	    syn_list_one(id, syncing, FALSE);
	ପାଇଁ (id = 0; id < curwin->w_s->b_syn_clusters.ga_len && !got_int; ++id)
	    syn_list_cluster(id);
    }
    ଅନ୍ୟଥା
    {
	/*
	 * List the group IDs and syntax clusters that are in the argument.
	 */
	ଯେତେବେଳେ (!ends_excmd2(eap->cmd, arg) && !got_int)
	{
	    arg_end = skiptowhite(arg);
	    ଯଦି (*arg == '@')
	    {
		id = syn_scl_namen2id(arg + 1, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(arg_end - arg - 1));
		ଯଦି (id == 0)
		    semsg(_(e_no_such_syntax_cluster_str_2), arg);
		ଅନ୍ୟଥା
		    syn_list_cluster(id - SYNID_CLUSTER);
	    }
	    ଅନ୍ୟଥା
	    {
		id = syn_namen2id(arg, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(arg_end - arg));
		ଯଦି (id == 0)
		    semsg(_(e_no_such_highlight_group_name_str), arg);
		ଅନ୍ୟଥା
		    syn_list_one(id, syncing, TRUE);
	    }
	    arg = skipwhite(arg_end);
	}
    }
    set_nextcmd(eap, arg);
}

    ସ୍ଥିର ଶୂନ୍ୟ
syn_ଧାଡ଼ିଗୁଡ଼ିକ_ସନ୍ଦେଶ(ଶୂନ୍ୟ)
{
    ଯଦି (curwin->w_s->b_syn_sync_maxlines > 0
				      || curwin->w_s->b_syn_sync_minlines > 0)
    {
	ସନ୍ଦେଶ_puts("; ");
	ଯଦି (curwin->w_s->b_syn_sync_minlines == MAXLNUM)
	    ସନ୍ଦେଶ_puts(_("from the first line"));
	ଅନ୍ୟଥା
	{
	    ଯଦି (curwin->w_s->b_syn_sync_minlines > 0)
	    {
		ସନ୍ଦେଶ_puts(_("minimal "));
		ସନ୍ଦେଶ_outnum(curwin->w_s->b_syn_sync_minlines);
		ଯଦି (curwin->w_s->b_syn_sync_maxlines)
		    ସନ୍ଦେଶ_puts(", ");
	    }
	    ଯଦି (curwin->w_s->b_syn_sync_maxlines > 0)
	    {
		ସନ୍ଦେଶ_puts(_("maximal "));
		ସନ୍ଦେଶ_outnum(curwin->w_s->b_syn_sync_maxlines);
	    }
	    ସନ୍ଦେଶ_puts(_(" lines before top line"));
	}
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
syn_match_ସନ୍ଦେଶ(ଶୂନ୍ୟ)
{
    ଯଦି (curwin->w_s->b_syn_sync_linebreaks > 0)
    {
	ସନ୍ଦେଶ_puts(_("; match "));
	ସନ୍ଦେଶ_outnum(curwin->w_s->b_syn_sync_linebreaks);
	ସନ୍ଦେଶ_puts(_(" line breaks"));
    }
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  last_matchgroup;

ସ୍ଥିର ଶୂନ୍ୟ syn_list_flags(keyvalue_T *nlist, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr_entries, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr);

/*
 * List one syntax item, for ":syntax" or "syntax list syntax_name".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_list_one(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		syncing,	    // when TRUE: list syncing items
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		link_only)	    // when TRUE; list link-only too
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_header = FALSE;
    synpat_T	*spp;
    ସ୍ଥିର keyvalue_T namelist1[] =
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
    ସ୍ଥିର keyvalue_T namelist2[] =
		{
		    KEYVALUE_ENTRY(HL_SKIPWHITE, "skipwhite"),
		    KEYVALUE_ENTRY(HL_SKIPNL, "skipnl"),
		    KEYVALUE_ENTRY(HL_SKIPEMPTY, "skipempty")
		};

    attr = HL_ATTR(HLF_D);		// highlight like directories

    // list the keywords for "id"
    ଯଦି (!syncing)
    {
	did_header = syn_list_keywords(id, &curwin->w_s->b_keywtab, FALSE, attr);
	did_header = syn_list_keywords(id, &curwin->w_s->b_keywtab_ic,
							    did_header, attr);
    }

    // list the patterns for "id"
    ପାଇଁ (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len && !got_int; ++idx)
    {
	spp = &(SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx]);
	ଯଦି (spp->sp_syn.id != id || spp->sp_syncing != syncing)
	    ଜାରି;

	(ଶୂନ୍ୟ)syn_list_header(did_header, 999, id);
	did_header = TRUE;
	last_matchgroup = 0;
	ଯଦି (spp->sp_type == SPTYPE_MATCH)
	{
	    put_pattern("match", ' ', spp, attr);
	    ସନ୍ଦେଶ_putchar(' ');
	}
	ଅନ୍ୟଥା ଯଦି (spp->sp_type == SPTYPE_START)
	{
	    ଯେତେବେଳେ (SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx].sp_type == SPTYPE_START)
		put_pattern("start", '=', &SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx++], attr);
	    ଯଦି (SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx].sp_type == SPTYPE_SKIP)
		put_pattern("skip", '=', &SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx++], attr);
	    ଯେତେବେଳେ (idx < curwin->w_s->b_syn_patterns.ga_len
			  && SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx].sp_type == SPTYPE_END)
		put_pattern("end", '=', &SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx++], attr);
	    --idx;
	    ସନ୍ଦେଶ_putchar(' ');
	}
	syn_list_flags(namelist1, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(namelist1), spp->sp_flags, attr);

	ଯଦି (spp->sp_cont_list != ଖାଲି)
	    put_id_list((char_u *)"contains", spp->sp_cont_list, attr);

	ଯଦି (spp->sp_syn.cont_in_list != ଖାଲି)
	    put_id_list((char_u *)"containedin",
					      spp->sp_syn.cont_in_list, attr);

	ଯଦି (spp->sp_next_list != ଖାଲି)
	{
	    put_id_list((char_u *)"nextgroup", spp->sp_next_list, attr);
	    syn_list_flags(namelist2, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(namelist2), spp->sp_flags, attr);
	}
	ଯଦି (spp->sp_flags & (HL_SYNC_HERE|HL_SYNC_THERE))
	{
	    ଯଦି (spp->sp_flags & HL_SYNC_HERE)
		ସନ୍ଦେଶ_puts_attr("grouphere", attr);
	    ଅନ୍ୟଥା
		ସନ୍ଦେଶ_puts_attr("groupthere", attr);
	    ସନ୍ଦେଶ_putchar(' ');
	    ଯଦି (spp->sp_sync_idx >= 0)
		ସନ୍ଦେଶ_outtrans(highlight_group_name(SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)
				   [spp->sp_sync_idx].sp_syn.id - 1));
	    ଅନ୍ୟଥା
		ସନ୍ଦେଶ_puts("NONE");
	    ସନ୍ଦେଶ_putchar(' ');
	}
    }

    // list the link, if there is one
    ଯଦି (highlight_link_id(id - 1) && (did_header || link_only) && !got_int)
    {
	(ଶୂନ୍ୟ)syn_list_header(did_header, 999, id);
	ସନ୍ଦେଶ_puts_attr("links to", attr);
	ସନ୍ଦେଶ_putchar(' ');
	ସନ୍ଦେଶ_outtrans(highlight_group_name(highlight_link_id(id - 1) - 1));
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
syn_list_flags(keyvalue_T *nlist, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr_entries, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = 0; i < nr_entries; ++i)
	ଯଦି (flags & nlist[i].ଚାବି)
	{
	    ସନ୍ଦେଶ_puts_attr((ଅକ୍ଷର *)nlist[i].ମୂଲ୍ୟ.string, attr);
	    ସନ୍ଦେଶ_putchar(' ');
	}
}

/*
 * List one syntax cluster, for ":syntax" or "syntax list syntax_name".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_list_cluster(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    endcol = 15;

    // slight hack:  roughly duplicate the guts of syn_list_header()
    ସନ୍ଦେଶ_putchar('\n');
    ସନ୍ଦେଶ_outtrans(SYN_CLSTR(curwin->w_s)[id].scl_name);

    ଯଦି (ସନ୍ଦେଶ_col >= endcol)	// output at least one space
	endcol = ସନ୍ଦେଶ_col + 1;
    ଯଦି (cmdline_width <= endcol)	// avoid hang for tiny window
	endcol = cmdline_width - 1;

    ସନ୍ଦେଶ_advance(endcol);
    ଯଦି (SYN_CLSTR(curwin->w_s)[id].scl_list != ଖାଲି)
    {
	put_id_list((char_u *)"cluster", SYN_CLSTR(curwin->w_s)[id].scl_list,
		    HL_ATTR(HLF_D));
    }
    ଅନ୍ୟଥା
    {
	ସନ୍ଦେଶ_puts_attr("cluster", HL_ATTR(HLF_D));
	ସନ୍ଦେଶ_puts("=NONE");
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
put_id_list(char_u *name, short *list, ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr)
{
    short		*p;

    ସନ୍ଦେଶ_puts_attr((ଅକ୍ଷର *)name, attr);
    ସନ୍ଦେଶ_putchar('=');
    ପାଇଁ (p = list; *p; ++p)
    {
	ଯଦି (*p >= SYNID_ALLBUT && *p < SYNID_ଶୀର୍ଷ)
	{
	    ଯଦି (p[1])
		ସନ୍ଦେଶ_puts("ALLBUT");
	    ଅନ୍ୟଥା
		ସନ୍ଦେଶ_puts("ALL");
	}
	ଅନ୍ୟଥା ଯଦି (*p >= SYNID_ଶୀର୍ଷ && *p < SYNID_CONTAINED)
	{
	    ସନ୍ଦେଶ_puts("TOP");
	}
	ଅନ୍ୟଥା ଯଦି (*p >= SYNID_CONTAINED && *p < SYNID_CLUSTER)
	{
	    ସନ୍ଦେଶ_puts("CONTAINED");
	}
	ଅନ୍ୟଥା ଯଦି (*p >= SYNID_CLUSTER)
	{
	    short scl_id = *p - SYNID_CLUSTER;

	    ସନ୍ଦେଶ_putchar('@');
	    ସନ୍ଦେଶ_outtrans(SYN_CLSTR(curwin->w_s)[scl_id].scl_name);
	}
	ଅନ୍ୟଥା
	    ସନ୍ଦେଶ_outtrans(highlight_group_name(*p - 1));
	ଯଦି (p[1])
	    ସନ୍ଦେଶ_putchar(',');
    }
    ସନ୍ଦେଶ_putchar(' ');
}

    ସ୍ଥିର ଶୂନ୍ୟ
put_pattern(
    ଅକ୍ଷର	*s,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c,
    synpat_T	*spp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr)
{
    long	n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mask;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first;
    ସ୍ଥିର ଅକ୍ଷର	*sepchars = "/+=-#@\"|'^&";
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    // May have to write "matchgroup=group"
    ଯଦି (last_matchgroup != spp->sp_syn_match_id)
    {
	last_matchgroup = spp->sp_syn_match_id;
	ସନ୍ଦେଶ_puts_attr("matchgroup", attr);
	ସନ୍ଦେଶ_putchar('=');
	ଯଦି (last_matchgroup == 0)
	    ସନ୍ଦେଶ_outtrans((char_u *)"NONE");
	ଅନ୍ୟଥା
	    ସନ୍ଦେଶ_outtrans(highlight_group_name(last_matchgroup - 1));
	ସନ୍ଦେଶ_putchar(' ');
    }

    // output the name of the pattern and an '=' or ' '
    ସନ୍ଦେଶ_puts_attr(s, attr);
    ସନ୍ଦେଶ_putchar(c);

    // output the pattern, in between a char that is not in the pattern
    ପାଇଁ (i = 0; vim_strchr(spp->sp_pattern, sepchars[i]) != ଖାଲି; )
	ଯଦି (sepchars[++i] == NUL)
	{
	    i = 0;	// no good char found, just use the first one
	    ଭାଙ୍ଗ;
	}
    ସନ୍ଦେଶ_putchar(sepchars[i]);
    ସନ୍ଦେଶ_outtrans(spp->sp_pattern);
    ସନ୍ଦେଶ_putchar(sepchars[i]);

    // output any pattern options
    first = TRUE;
    ପାଇଁ (i = 0; i < SPO_ଗଣନା; ++i)
    {
	mask = (1 << i);
	ଯଦି (spp->sp_off_flags & (mask + (mask << SPO_ଗଣନା)))
	{
	    ଯଦି (!first)
		ସନ୍ଦେଶ_putchar(',');	// separate with commas
	    ସନ୍ଦେଶ_puts(spo_name_tab[i]);
	    n = spp->sp_offsets[i];
	    ଯଦି (i != SPO_LC_OFF)
	    {
		ଯଦି (spp->sp_off_flags & mask)
		    ସନ୍ଦେଶ_putchar('s');
		ଅନ୍ୟଥା
		    ସନ୍ଦେଶ_putchar('e');
		ଯଦି (n > 0)
		    ସନ୍ଦେଶ_putchar('+');
	    }
	    ଯଦି (n || i == SPO_LC_OFF)
		ସନ୍ଦେଶ_outnum(n);
	    first = FALSE;
	}
    }
    ସନ୍ଦେଶ_putchar(' ');
}

/*
 * List or clear the keywords for one syntax group.
 * Return TRUE if the header has been printed.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_list_keywords(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id,
    hashtab_T	*ht,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_header,		// header has already been printed
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attr)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		outlen;
    hashitem_T	*hi;
    keyentry_T	*kp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_contained = 0;
    short	*prev_next_list = ଖାଲି;
    short	*prev_cont_in_list = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_skipnl = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_skipwhite = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_skipempty = 0;

    /*
     * Unfortunately, this list of keywords is not sorted on alphabet but on
     * hash value...
     */
    todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ht->ht_used;
    ପାଇଁ (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ପାଇଁ (kp = HI2KE(hi); kp != ଖାଲି && !got_int; kp = kp->ke_next)
	    {
		ଯଦି (kp->k_syn.id == id)
		{
		    ଯଦି (prev_contained != (kp->flags & HL_CONTAINED)
			    || prev_skipnl != (kp->flags & HL_SKIPNL)
			    || prev_skipwhite != (kp->flags & HL_SKIPWHITE)
			    || prev_skipempty != (kp->flags & HL_SKIPEMPTY)
			    || prev_cont_in_list != kp->k_syn.cont_in_list
			    || prev_next_list != kp->next_list)
			outlen = 9999;
		    ଅନ୍ୟଥା
			outlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(kp->keyword);
		    // output "contained" and "nextgroup" on each line
		    ଯଦି (syn_list_header(did_header, outlen, id))
		    {
			prev_contained = 0;
			prev_next_list = ଖାଲି;
			prev_cont_in_list = ଖାଲି;
			prev_skipnl = 0;
			prev_skipwhite = 0;
			prev_skipempty = 0;
		    }
		    did_header = TRUE;
		    ଯଦି (prev_contained != (kp->flags & HL_CONTAINED))
		    {
			ସନ୍ଦେଶ_puts_attr("contained", attr);
			ସନ୍ଦେଶ_putchar(' ');
			prev_contained = (kp->flags & HL_CONTAINED);
		    }
		    ଯଦି (kp->k_syn.cont_in_list != prev_cont_in_list)
		    {
			put_id_list((char_u *)"containedin",
						kp->k_syn.cont_in_list, attr);
			ସନ୍ଦେଶ_putchar(' ');
			prev_cont_in_list = kp->k_syn.cont_in_list;
		    }
		    ଯଦି (kp->next_list != prev_next_list)
		    {
			put_id_list((char_u *)"nextgroup", kp->next_list, attr);
			ସନ୍ଦେଶ_putchar(' ');
			prev_next_list = kp->next_list;
			ଯଦି (kp->flags & HL_SKIPNL)
			{
			    ସନ୍ଦେଶ_puts_attr("skipnl", attr);
			    ସନ୍ଦେଶ_putchar(' ');
			    prev_skipnl = (kp->flags & HL_SKIPNL);
			}
			ଯଦି (kp->flags & HL_SKIPWHITE)
			{
			    ସନ୍ଦେଶ_puts_attr("skipwhite", attr);
			    ସନ୍ଦେଶ_putchar(' ');
			    prev_skipwhite = (kp->flags & HL_SKIPWHITE);
			}
			ଯଦି (kp->flags & HL_SKIPEMPTY)
			{
			    ସନ୍ଦେଶ_puts_attr("skipempty", attr);
			    ସନ୍ଦେଶ_putchar(' ');
			    prev_skipempty = (kp->flags & HL_SKIPEMPTY);
			}
		    }
		    ସନ୍ଦେଶ_outtrans(kp->keyword);
		}
	    }
	}
    }

    ଫେରନ୍ତୁ did_header;
}

    ସ୍ଥିର ଶୂନ୍ୟ
syn_clear_keyword(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id, hashtab_T *ht)
{
    hashitem_T	*hi;
    keyentry_T	*kp;
    keyentry_T	*kp_prev;
    keyentry_T	*kp_next;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;

    hash_lock(ht);
    todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ht->ht_used;
    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(ht, hi, todo)
    {
	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    kp_prev = ଖାଲି;
	    ପାଇଁ (kp = HI2KE(hi); kp != ଖାଲି; )
	    {
		ଯଦି (kp->k_syn.id == id)
		{
		    kp_next = kp->ke_next;
		    ଯଦି (kp_prev == ଖାଲି)
		    {
			ଯଦି (kp_next == ଖାଲି)
			    hash_remove(ht, hi, "syntax clear keyword");
			ଅନ୍ୟଥା
			    hi->hi_ଚାବି = KE2HIKEY(kp_next);
		    }
		    ଅନ୍ୟଥା
			kp_prev->ke_next = kp_next;
		    vim_free(kp->next_list);
		    vim_free(kp->k_syn.cont_in_list);
		    vim_free(kp);
		    kp = kp_next;
		}
		ଅନ୍ୟଥା
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
    ସ୍ଥିର ଶୂନ୍ୟ
clear_keywtab(hashtab_T *ht)
{
    hashitem_T	*hi;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;
    keyentry_T	*kp;
    keyentry_T	*kp_next;

    todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ht->ht_used;
    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(ht, hi, todo)
    {
	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ପାଇଁ (kp = HI2KE(hi); kp != ଖାଲି; kp = kp_next)
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
    ସ୍ଥିର ଶୂନ୍ୟ
add_keyword(
    char_u	*name,	    // name of keyword
    size_t	namelen,    // length of keyword (excluding the NUL)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id,	    // group ID for this keyword
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,	    // flags for this keyword
    short	*cont_in_list, // containedin for this keyword
    short	*next_list, // nextgroup for this keyword
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		conceal_char)
{
    keyentry_T	*kp;
    hashtab_T	*ht;
    hashitem_T	*hi;
    char_u	*name_ic;
    size_t	name_iclen;
    long_u	hash;
    char_u	name_folded[MAXKEYWLEN + 1];

    ଯଦି (curwin->w_s->b_syn_ic)
    {
	name_ic = str_foldcase(name, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)namelen,
						 name_folded, MAXKEYWLEN + 1);
	name_iclen = STRLEN(name_ic);
    }
    ଅନ୍ୟଥା
    {
	name_ic = name;
	name_iclen = namelen;
    }
    kp = alloc(offsetof(keyentry_T, keyword) + name_iclen + 1);
    ଯଦି (kp == ଖାଲି)
	ଫେରନ୍ତୁ;
    STRCPY(kp->keyword, name_ic);
    kp->k_syn.id = id;
    kp->k_syn.inc_tag = current_syn_inc_tag;
    kp->flags = flags;
    kp->k_char = conceal_char;
    kp->k_syn.cont_in_list = copy_id_list(cont_in_list);
    ଯଦି (cont_in_list != ଖାଲି)
	curwin->w_s->b_syn_containedin = TRUE;
    kp->next_list = copy_id_list(next_list);

    ଯଦି (curwin->w_s->b_syn_ic)
	ht = &curwin->w_s->b_keywtab_ic;
    ଅନ୍ୟଥା
	ht = &curwin->w_s->b_keywtab;

    hash = hash_hash(kp->keyword);
    hi = hash_lookup(ht, kp->keyword, hash);
    ଯଦି (HASHITEM_EMPTY(hi))
    {
	// new keyword, add to hashtable
	kp->ke_next = ଖାଲି;
	hash_add_item(ht, hi, kp->keyword, hash);
    }
    ଅନ୍ୟଥା
    {
	// keyword already exists, prepend to list
	kp->ke_next = HI2KE(hi);
	hi->hi_ଚାବି = KE2HIKEY(kp);
    }
}

/*
 * Get the start and end of the group name argument.
 * Return a pointer to the first argument.
 * Return NULL if the end of the command was found instead of further args.
 */
    ସ୍ଥିର char_u *
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
    ଯଦି (ends_excmd(*arg) || *rest == NUL)
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ rest;
}

/*
 * Check for syntax command option arguments.
 * This can be called at any place in the list of arguments, and just picks
 * out the arguments that are known.  Can be called several times in a row to
 * collect all options in between other arguments.
 * Return a pointer to the next argument (which isn't an option).
 * Return NULL for any error;
 */
    ସ୍ଥିର char_u *
get_syn_options(
    char_u	    *start,		// next argument to be checked
    syn_opt_arg_T   *opt,		// various things
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    *conceal_char UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    skip)		// TRUE if skipping over command
{
    char_u	*arg = start;
    char_u	*gname_start, *gname;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		syn_id;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ଅକ୍ଷର	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fidx;
    ସ୍ଥିର ସଂରଚନା flag
    {
	ଅକ୍ଷର	*name;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	argtype;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	flags;
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
    ସ୍ଥିର ଅକ୍ଷର *first_letters = "cCoOkKeEtTsSgGdDfFnN";

    ଯଦି (arg == ଖାଲି)		// already detected error
	ଫେରନ୍ତୁ ଖାଲି;

#ifdef FEAT_CONCEAL
    ଯଦି (curwin->w_s->b_syn_conceal)
	opt->flags |= HL_CONCEAL;
#endif

    ପାଇଁ (;;)
    {
	/*
	 * This is used very often when a large number of keywords is defined.
	 * Need to skip quickly when no option name is found.
	 * Also avoid tolower(), it's slow.
	 */
	ଯଦି (strchr(first_letters, *arg) == ଖାଲି)
	    ଭାଙ୍ଗ;

	ପାଇଁ (fidx = ARRAY_LENGTH(flagtab); --fidx >= 0; )
	{
	    p = flagtab[fidx].name;
	    ପାଇଁ (i = 0, len = 0; p[i] != NUL; i += 2, ++len)
		ଯଦି (arg[len] != p[i] && arg[len] != p[i + 1])
		    ଭାଙ୍ଗ;
	    ଯଦି (p[i] == NUL && (VIM_ISWHITE(arg[len])
				    || (flagtab[fidx].argtype > 0
					 ? arg[len] == '='
					 : ends_excmd2(start, arg + len))))
	    {
		ଯଦି (opt->keyword
			&& (flagtab[fidx].flags == HL_DISPLAY
			    || flagtab[fidx].flags == HL_FOLD
			    || flagtab[fidx].flags == HL_EXTEND))
		    // treat "display", "fold" and "extend" as a keyword
		    fidx = -1;
		ଭାଙ୍ଗ;
	    }
	}
	ଯଦି (fidx < 0)	    // no match found
	    ଭାଙ୍ଗ;

	ଯଦି (flagtab[fidx].argtype == 1)
	{
	    ଯଦି (!opt->has_cont_list)
	    {
		emsg(_(e_contains_argument_not_accepted_here));
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    ଯଦି (get_id_list(&arg, 8, &opt->cont_list, skip) == FAIL)
		ଫେରନ୍ତୁ ଖାଲି;
	}
	ଅନ୍ୟଥା ଯଦି (flagtab[fidx].argtype == 2)
	{
	    ଯଦି (get_id_list(&arg, 11, &opt->cont_in_list, skip) == FAIL)
		ଫେରନ୍ତୁ ଖାଲି;
	}
	ଅନ୍ୟଥା ଯଦି (flagtab[fidx].argtype == 3)
	{
	    ଯଦି (get_id_list(&arg, 9, &opt->next_list, skip) == FAIL)
		ଫେରନ୍ତୁ ଖାଲି;
	}
	ଅନ୍ୟଥା ଯଦି (flagtab[fidx].argtype == 11 && arg[5] == '=')
	{
	    // cchar=?
	    ଯଦି (has_mbyte)
	    {
#ifdef FEAT_CONCEAL
		*conceal_char = mb_ptr2char(arg + 6);
#endif
		arg += mb_ptr2len(arg + 6) - 1;
	    }
	    ଅନ୍ୟଥା
	    {
#ifdef FEAT_CONCEAL
		*conceal_char = arg[6];
#ଅନ୍ୟଥା
		;
#endif
	    }
#ifdef FEAT_CONCEAL
	    ଯଦି (!vim_isprintc_strict(*conceal_char))
	    {
		emsg(_(e_invalid_cchar_ମୂଲ୍ୟ));
		ଫେରନ୍ତୁ ଖାଲି;
	    }
#endif
	    arg = skipwhite(arg + 7);
	}
	ଅନ୍ୟଥା
	{
	    opt->flags |= flagtab[fidx].flags;
	    arg = skipwhite(arg + len);

	    ଯଦି (flagtab[fidx].flags == HL_SYNC_HERE
		    || flagtab[fidx].flags == HL_SYNC_THERE)
	    {
		ଯଦି (opt->sync_idx == ଖାଲି)
		{
		    emsg(_(e_groupthere_not_accepted_here));
		    ଫେରନ୍ତୁ ଖାଲି;
		}
		gname_start = arg;
		arg = skiptowhite(arg);
		ଯଦି (gname_start == arg)
		    ଫେରନ୍ତୁ ଖାଲି;
		gname = vim_strnsave(gname_start, arg - gname_start);
		ଯଦି (gname == ଖାଲି)
		    ଫେରନ୍ତୁ ଖାଲି;
		ଯଦି (STRCMP(gname, "NONE") == 0)
		    *opt->sync_idx = NONE_IDX;
		ଅନ୍ୟଥା
		{
		    syn_id = syn_name2id(gname);
		    ପାଇଁ (i = curwin->w_s->b_syn_patterns.ga_len; --i >= 0; )
			ଯଦି (SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[i].sp_syn.id == syn_id
			      && SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[i].sp_type
							       == SPTYPE_START)
			{
			    *opt->sync_idx = i;
			    ଭାଙ୍ଗ;
			}
		    ଯଦି (i < 0)
		    {
			semsg(_(e_didnt_find_region_item_for_str), gname);
			vim_free(gname);
			ଫେରନ୍ତୁ ଖାଲି;
		    }
		}

		vim_free(gname);
		arg = skipwhite(arg);
	    }
#ifdef FEAT_FOLDING
	    ଅନ୍ୟଥା ଯଦି (flagtab[fidx].flags == HL_FOLD
						&& foldmethodIsSyntax(curwin))
		// Need to update folds later.
		foldUpdateAll(curwin);
#endif
	}
    }

    ଫେରନ୍ତୁ arg;
}

/*
 * Adjustments to syntax item when declared in a ":syn include"'d file.
 * Set the contained flag, and if the item is not already contained, add it
 * to the specified top-level group, if any.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_incl_toplevel(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *flagsp)
{
    ଯଦି ((*flagsp & HL_CONTAINED) || curwin->w_s->b_syn_topgrp == 0)
	ଫେରନ୍ତୁ;
    *flagsp |= HL_CONTAINED | HL_INCLUDED_TOPLEVEL;
    ଯଦି (curwin->w_s->b_syn_topgrp >= SYNID_CLUSTER)
    {
	// We have to alloc this, because syn_combine_list() will free it.
	short	    *grp_list = ALLOC_MULT(short, 2);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    tlg_id = curwin->w_s->b_syn_topgrp - SYNID_CLUSTER;

	ଯଦି (grp_list != ଖାଲି)
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
    ସ୍ଥିର ଶୂନ୍ୟ
syn_cmd_include(exarg_T *eap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା syncing UNUSED)
{
    char_u	*arg = eap->arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sgl_id = 1;
    char_u	*group_name_end;
    char_u	*rest;
    ଅକ୍ଷର	*errormsg = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_toplvl_grp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_syn_inc_tag;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		source = FALSE;

    eap->nextcmd = find_nextcmd(arg);
    ଯଦି (eap->skip)
	ଫେରନ୍ତୁ;

    ଯଦି (arg[0] == '@')
    {
	++arg;
	rest = get_group_name(arg, &group_name_end);
	ଯଦି (rest == ଖାଲି)
	{
	    emsg(_(e_filename_required));
	    ଫେରନ୍ତୁ;
	}
	sgl_id = syn_check_cluster(arg, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(group_name_end - arg));
	ଯଦି (sgl_id == 0)
	    ଫେରନ୍ତୁ;
	// separate_nextcmd() and expand_filename() depend on this
	eap->arg = rest;
    }

    /*
     * Everything that's left, up to the next command, should be the
     * filename to include.
     */
    eap->argt |= (EX_XFILE | EX_NOSPC);
    separate_nextcmd(eap, FALSE);
    ଯଦି (*eap->arg == '<' || *eap->arg == '$' || mch_isFullName(eap->arg))
    {
	// For an absolute path, "$VIM/..." or "<sfile>.." we ":source" the
	// file.  Need to expand the file name first.  In other cases
	// ":runtime!" is used.
	source = TRUE;
	ଯଦି (expand_filename(eap, syn_cmdlinep, &errormsg) == FAIL)
	{
	    ଯଦି (errormsg != ଖାଲି)
		emsg(errormsg);
	    ଫେରନ୍ତୁ;
	}
    }

    /*
     * Save and restore the existing top-level grouplist id and ":syn
     * include" tag around the actual inclusion.
     */
    ଯଦି (running_syn_inc_tag >= MAX_SYN_INC_TAG)
    {
	emsg(_(e_too_many_syntax_includes));
	ଫେରନ୍ତୁ;
    }
    prev_syn_inc_tag = current_syn_inc_tag;
    current_syn_inc_tag = ++running_syn_inc_tag;
    prev_toplvl_grp = curwin->w_s->b_syn_topgrp;
    curwin->w_s->b_syn_topgrp = sgl_id;
    ଯଦି (source ? do_source(eap->arg, FALSE, DOSO_NONE, ଖାଲି) == FAIL
				: source_runtime(eap->arg, DIP_ALL) == FAIL)
	semsg(_(e_cant_open_file_str), eap->arg);
    curwin->w_s->b_syn_topgrp = prev_toplvl_grp;
    current_syn_inc_tag = prev_syn_inc_tag;
}

/*
 * Handle ":syntax keyword {group-name} [{option}] keyword .." command.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_cmd_keyword(exarg_T *eap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*group_name_end;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		syn_id;
    char_u	*rest;
    char_u	*keyword_copy = ଖାଲି;
    char_u	*p;
    char_u	*kw;
    syn_opt_arg_T syn_opt_arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cnt;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		conceal_char = NUL;

    rest = get_group_name(arg, &group_name_end);

    ଯଦି (rest != ଖାଲି)
    {
	ଯଦି (eap->skip)
	    syn_id = -1;
	ଅନ୍ୟଥା
	    syn_id = syn_check_group(arg, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(group_name_end - arg));
	ଯଦି (syn_id != 0)
	    // allocate a buffer, for removing backslashes in the keyword
	    keyword_copy = alloc(STRLEN(rest) + 1);
	ଯଦି (keyword_copy != ଖାଲି)
	{
	    syn_opt_arg.flags = 0;
	    syn_opt_arg.keyword = TRUE;
	    syn_opt_arg.sync_idx = ଖାଲି;
	    syn_opt_arg.has_cont_list = FALSE;
	    syn_opt_arg.cont_in_list = ଖାଲି;
	    syn_opt_arg.next_list = ଖାଲି;

	    /*
	     * The options given apply to ALL keywords, so all options must be
	     * found before keywords can be created.
	     * 1: collect the options and copy the keywords to keyword_copy.
	     */
	    cnt = 0;
	    p = keyword_copy;
	    ପାଇଁ ( ; rest != ଖାଲି && !ends_excmd2(eap->arg, rest);
							rest = skipwhite(rest))
	    {
		rest = get_syn_options(rest, &syn_opt_arg, &conceal_char,
								    eap->skip);
		ଯଦି (rest == ଖାଲି || ends_excmd2(eap->arg, rest))
		    ଭାଙ୍ଗ;
		// Copy the keyword, removing backslashes, and add a NUL.
		ଯେତେବେଳେ (*rest != NUL && !VIM_ISWHITE(*rest))
		{
		    ଯଦି (*rest == '\\' && rest[1] != NUL)
			++rest;
		    *p++ = *rest++;
		}
		*p++ = NUL;
		++cnt;
	    }

	    ଯଦି (!eap->skip)
	    {
		size_t	kwlen = 0;

		// Adjust flags for use of ":syn include".
		syn_incl_toplevel(syn_id, &syn_opt_arg.flags);

		/*
		 * 2: Add an entry for each keyword.
		 */
		ପାଇଁ (kw = keyword_copy; --cnt >= 0; kw += kwlen + 1)
		{
		    ପାଇଁ (p = vim_strchr(kw, '['); ; )
		    {
			ଯଦି (p == ଖାଲି)
			    kwlen = STRLEN(kw);
			ଅନ୍ୟଥା
			{
			    *p = NUL;
			    kwlen = (size_t)(p - kw);
			}
			add_keyword(kw, kwlen, syn_id, syn_opt_arg.flags,
				syn_opt_arg.cont_in_list,
					 syn_opt_arg.next_list, conceal_char);
			ଯଦି (p == ଖାଲି)
			    ଭାଙ୍ଗ;
			ଯଦି (p[1] == NUL)
			{
			    semsg(_(e_error_missing_rsb_str), kw);
			    ଯାଅ error;
			}
			ଯଦି (p[1] == ']')
			{
			    ଯଦି (p[2] != NUL)
			    {
				semsg(_(e_trailing_char_after_rsb_str_str),
								    kw, &p[2]);
				ଯାଅ error;
			    }
			    kw = p + 1;		// skip over the "]"
			    kwlen = 1;
			    ଭାଙ୍ଗ;
			}
			ଯଦି (has_mbyte)
			{
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା l = (*mb_ptr2len)(p + 1);

			    mch_memmove(p, p + 1, l);
			    p += l;
			}
			ଅନ୍ୟଥା
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

    ଯଦି (rest != ଖାଲି)
	set_nextcmd(eap, rest);
    ଅନ୍ୟଥା
	semsg(_(e_invalid_argument_str), arg);

    redraw_curbuf_later(UPD_SOME_VALID);
    syn_ଗଦା_free_all(curwin->w_s);		// Need to recompute all syntax.
}

/*
 * Handle ":syntax match {name} [{options}] {pattern} [{options}]".
 *
 * Also ":syntax sync match {name} [[grouphere | groupthere] {group-name}] .."
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_cmd_match(
    exarg_T	*eap,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		syncing)	    // TRUE for ":syntax sync match .. "
{
    char_u	*arg = eap->arg;
    char_u	*group_name_end;
    char_u	*rest;
    synpat_T	item;		// the item found in the line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		syn_id;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    syn_opt_arg_T syn_opt_arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sync_idx = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		conceal_char = NUL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		orig_called_emsg = called_emsg;

    // Isolate the group name, check for validity
    rest = get_group_name(arg, &group_name_end);

    // Get options before the pattern
    syn_opt_arg.flags = 0;
    syn_opt_arg.keyword = FALSE;
    syn_opt_arg.sync_idx = syncing ? &sync_idx : ଖାଲି;
    syn_opt_arg.has_cont_list = TRUE;
    syn_opt_arg.cont_list = ଖାଲି;
    syn_opt_arg.cont_in_list = ଖାଲି;
    syn_opt_arg.next_list = ଖାଲି;
    rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);

    // get the pattern.
    init_syn_patterns();
    CLEAR_FIELD(item);
    rest = get_syn_pattern(rest, &item);
    ଯଦି (vim_regcomp_had_eol() && !(syn_opt_arg.flags & HL_EXCLUDENL))
	syn_opt_arg.flags |= HL_HAS_EOL;

    // Get options after the pattern
    rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);

    ଯଦି (rest != ଖାଲି)		// all arguments are valid
    {
	/*
	 * Check for trailing command and illegal trailing arguments.
	 */
	set_nextcmd(eap, rest);
	ଯଦି (!ends_excmd2(eap->cmd, rest) || eap->skip)
	    rest = ଖାଲି;
	ଅନ୍ୟଥା ଯଦି (ga_grow(&curwin->w_s->b_syn_patterns, 1) == OK
		&& (syn_id = syn_check_group(arg,
					   (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(group_name_end - arg))) != 0)
	{
	    syn_incl_toplevel(syn_id, &syn_opt_arg.flags);
	    /*
	     * Store the pattern in the syn_items list
	     */
	    idx = curwin->w_s->b_syn_patterns.ga_len;
	    SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx] = item;
	    SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx].sp_syncing = syncing;
	    SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx].sp_type = SPTYPE_MATCH;
	    SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx].sp_syn.id = syn_id;
	    SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx].sp_syn.inc_tag = current_syn_inc_tag;
	    SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx].sp_flags = syn_opt_arg.flags;
	    SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx].sp_sync_idx = sync_idx;
	    SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx].sp_cont_list = syn_opt_arg.cont_list;
	    SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx].sp_syn.cont_in_list =
						     syn_opt_arg.cont_in_list;
#ifdef FEAT_CONCEAL
	    SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx].sp_cchar = conceal_char;
#endif
	    ଯଦି (syn_opt_arg.cont_in_list != ଖାଲି)
		curwin->w_s->b_syn_containedin = TRUE;
	    SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx].sp_next_list = syn_opt_arg.next_list;
	    ++curwin->w_s->b_syn_patterns.ga_len;

	    // remember that we found a match for syncing on
	    ଯଦି (syn_opt_arg.flags & (HL_SYNC_HERE|HL_SYNC_THERE))
		curwin->w_s->b_syn_sync_flags |= SF_MATCH;
#ifdef FEAT_FOLDING
	    ଯଦି (syn_opt_arg.flags & HL_FOLD)
		++curwin->w_s->b_syn_folditems;
#endif

	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_ଗଦା_free_all(curwin->w_s);	// Need to recompute all syntax.
	    ଫେରନ୍ତୁ;	// don't free the progs and patterns now
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

    ଯଦି (rest == ଖାଲି && called_emsg == orig_called_emsg)
	semsg(_(e_invalid_argument_str), arg);
}

/*
 * Handle ":syntax region {group-name} [matchgroup={group-name}]
 *		start {start} .. [skip {skip}] end {end} .. [{options}]".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_cmd_region(
    exarg_T	*eap,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		syncing)	    // TRUE for ":syntax sync region .."
{
    char_u		*arg = eap->arg;
    char_u		*group_name_end;
    char_u		*rest;			// next arg, NULL on error
    char_u		*ଚାବି_end;
    char_u		*ଚାବି = ଖାଲି;
    char_u		*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			item;
#define ITEM_START	    0
#define ITEM_SKIP	    1
#define ITEM_END	    2
#define ITEM_MATCHGROUP	    3
    ସଂରଚନା pat_ptr
    {
	synpat_T	*pp_synp;		// pointer to syn_pattern
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pp_matchgroup_id;	// matchgroup ID
	ସଂରଚନା pat_ptr	*pp_next;		// pointer to next pat_ptr
    }			*(pat_ptrs[3]);
					// patterns found in the line
    ସଂରଚନା pat_ptr	*ppp;
    ସଂରଚନା pat_ptr	*ppp_next;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			pat_ଗଣନା = 0;		// nr of syn_patterns found
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			syn_id;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			matchgroup_id = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			not_enough = FALSE;	// not enough arguments
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			illegal = FALSE;	// illegal arguments
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			success = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			idx;
    syn_opt_arg_T	syn_opt_arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			conceal_char = NUL;

    // Isolate the group name, check for validity
    rest = get_group_name(arg, &group_name_end);

    pat_ptrs[0] = ଖାଲି;
    pat_ptrs[1] = ଖାଲି;
    pat_ptrs[2] = ଖାଲି;

    init_syn_patterns();

    syn_opt_arg.flags = 0;
    syn_opt_arg.keyword = FALSE;
    syn_opt_arg.sync_idx = ଖାଲି;
    syn_opt_arg.has_cont_list = TRUE;
    syn_opt_arg.cont_list = ଖାଲି;
    syn_opt_arg.cont_in_list = ଖାଲି;
    syn_opt_arg.next_list = ଖାଲି;

    /*
     * get the options, patterns and matchgroup.
     */
    ଯେତେବେଳେ (rest != ଖାଲି && !ends_excmd2(eap->cmd, rest))
    {
	// Check for option arguments
	rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);
	ଯଦି (rest == ଖାଲି || ends_excmd2(eap->cmd, rest))
	    ଭାଙ୍ଗ;

	// must be a pattern or matchgroup then
	ଚାବି_end = rest;
	ଯେତେବେଳେ (*ଚାବି_end && !VIM_ISWHITE(*ଚାବି_end) && *ଚାବି_end != '=')
	    ++ଚାବି_end;
	vim_free(ଚାବି);
	ଚାବି = vim_strnsave_up(rest, ଚାବି_end - rest);
	ଯଦି (ଚାବି == ଖାଲି)			// out of memory
	{
	    rest = ଖାଲି;
	    ଭାଙ୍ଗ;
	}
	ଯଦି (STRCMP(ଚାବି, "MATCHGROUP") == 0)
	    item = ITEM_MATCHGROUP;
	ଅନ୍ୟଥା ଯଦି (STRCMP(ଚାବି, "START") == 0)
	    item = ITEM_START;
	ଅନ୍ୟଥା ଯଦି (STRCMP(ଚାବି, "END") == 0)
	    item = ITEM_END;
	ଅନ୍ୟଥା ଯଦି (STRCMP(ଚାବି, "SKIP") == 0)
	{
	    ଯଦି (pat_ptrs[ITEM_SKIP] != ଖାଲି)	// one skip pattern allowed
	    {
		illegal = TRUE;
		ଭାଙ୍ଗ;
	    }
	    item = ITEM_SKIP;
	}
	ଅନ୍ୟଥା
	    ଭାଙ୍ଗ;
	rest = skipwhite(ଚାବି_end);
	ଯଦି (*rest != '=')
	{
	    rest = ଖାଲି;
	    semsg(_(e_missing_equal_str), arg);
	    ଭାଙ୍ଗ;
	}
	rest = skipwhite(rest + 1);
	ଯଦି (*rest == NUL)
	{
	    not_enough = TRUE;
	    ଭାଙ୍ଗ;
	}

	ଯଦି (item == ITEM_MATCHGROUP)
	{
	    p = skiptowhite(rest);
	    ଯଦି ((p - rest == 4 && STRNCMP(rest, "NONE", 4) == 0) || eap->skip)
		matchgroup_id = 0;
	    ଅନ୍ୟଥା
	    {
		matchgroup_id = syn_check_group(rest, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - rest));
		ଯଦି (matchgroup_id == 0)
		{
		    illegal = TRUE;
		    ଭାଙ୍ଗ;
		}
	    }
	    rest = skipwhite(p);
	}
	ଅନ୍ୟଥା
	{
	    /*
	     * Allocate room for a syn_pattern, and link it in the list of
	     * syn_patterns for this item, at the start (because the list is
	     * used from end to start).
	     */
	    ppp = ALLOC_ONE(ସଂରଚନା pat_ptr);
	    ଯଦି (ppp == ଖାଲି)
	    {
		rest = ଖାଲି;
		ଭାଙ୍ଗ;
	    }
	    ppp->pp_next = pat_ptrs[item];
	    pat_ptrs[item] = ppp;
	    ppp->pp_synp = ALLOC_CLEAR_ONE(synpat_T);
	    ଯଦି (ppp->pp_synp == ଖାଲି)
	    {
		rest = ଖାଲି;
		ଭାଙ୍ଗ;
	    }

	    /*
	     * Get the syntax pattern and the following offset(s).
	     */
	    // Enable the appropriate \z specials.
	    ଯଦି (item == ITEM_START)
		reg_do_extmatch = REX_SET;
	    ଅନ୍ୟଥା ଯଦି (item == ITEM_SKIP || item == ITEM_END)
		reg_do_extmatch = REX_USE;
	    rest = get_syn_pattern(rest, ppp->pp_synp);
	    reg_do_extmatch = 0;
	    ଯଦି (item == ITEM_END && vim_regcomp_had_eol()
				       && !(syn_opt_arg.flags & HL_EXCLUDENL))
		ppp->pp_synp->sp_flags |= HL_HAS_EOL;
	    ppp->pp_matchgroup_id = matchgroup_id;
	    ++pat_ଗଣନା;
	}
    }
    vim_free(ଚାବି);
    ଯଦି (illegal || not_enough)
	rest = ଖାଲି;

    /*
     * Must have a "start" and "end" pattern.
     */
    ଯଦି (rest != ଖାଲି && (pat_ptrs[ITEM_START] == ଖାଲି ||
						  pat_ptrs[ITEM_END] == ଖାଲି))
    {
	not_enough = TRUE;
	rest = ଖାଲି;
    }

    ଯଦି (rest != ଖାଲି)
    {
	/*
	 * Check for trailing garbage or command.
	 * If OK, add the item.
	 */
	set_nextcmd(eap, rest);
	ଯଦି (!ends_excmd(*rest) || eap->skip)
	    rest = ଖାଲି;
	ଅନ୍ୟଥା ଯଦି (ga_grow(&(curwin->w_s->b_syn_patterns), pat_ଗଣନା) == OK
		&& (syn_id = syn_check_group(arg,
					   (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(group_name_end - arg))) != 0)
	{
	    syn_incl_toplevel(syn_id, &syn_opt_arg.flags);
	    /*
	     * Store the start/skip/end in the syn_items list
	     */
	    idx = curwin->w_s->b_syn_patterns.ga_len;
	    ପାଇଁ (item = ITEM_START; item <= ITEM_END; ++item)
	    {
		ପାଇଁ (ppp = pat_ptrs[item]; ppp != ଖାଲି; ppp = ppp->pp_next)
		{
		    SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx] = *(ppp->pp_synp);
		    SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx].sp_syncing = syncing;
		    SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx].sp_type =
			    (item == ITEM_START) ? SPTYPE_START :
			    (item == ITEM_SKIP) ? SPTYPE_SKIP : SPTYPE_END;
		    SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx].sp_flags |= syn_opt_arg.flags;
		    SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx].sp_syn.id = syn_id;
		    SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx].sp_syn.inc_tag =
							  current_syn_inc_tag;
		    SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx].sp_syn_match_id =
							ppp->pp_matchgroup_id;
#ifdef FEAT_CONCEAL
		    SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx].sp_cchar = conceal_char;
#endif
		    ଯଦି (item == ITEM_START)
		    {
			SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx].sp_cont_list =
							syn_opt_arg.cont_list;
			SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx].sp_syn.cont_in_list =
						     syn_opt_arg.cont_in_list;
			ଯଦି (syn_opt_arg.cont_in_list != ଖାଲି)
			    curwin->w_s->b_syn_containedin = TRUE;
			SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx].sp_next_list =
							syn_opt_arg.next_list;
		    }
		    ++curwin->w_s->b_syn_patterns.ga_len;
		    ++idx;
#ifdef FEAT_FOLDING
		    ଯଦି (syn_opt_arg.flags & HL_FOLD)
			++curwin->w_s->b_syn_folditems;
#endif
		}
	    }

	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_ଗଦା_free_all(curwin->w_s);	// Need to recompute all syntax.
	    success = TRUE;	    // don't free the progs and patterns now
	}
    }

    /*
     * Free the allocated memory.
     */
    ପାଇଁ (item = ITEM_START; item <= ITEM_END; ++item)
	ପାଇଁ (ppp = pat_ptrs[item]; ppp != ଖାଲି; ppp = ppp_next)
	{
	    ଯଦି (!success && ppp->pp_synp != ଖାଲି)
	    {
		vim_regfree(ppp->pp_synp->sp_prog);
		vim_free(ppp->pp_synp->sp_pattern);
	    }
	    vim_free(ppp->pp_synp);
	    ppp_next = ppp->pp_next;
	    vim_free(ppp);
	}

    ଯଦି (!success)
    {
	vim_free(syn_opt_arg.cont_list);
	vim_free(syn_opt_arg.cont_in_list);
	vim_free(syn_opt_arg.next_list);
	ଯଦି (not_enough)
	    semsg(_(e_not_enough_arguments_syntax_region_str), arg);
	ଅନ୍ୟଥା ଯଦି (illegal || rest == ଖାଲି)
	    semsg(_(e_invalid_argument_str), arg);
    }
}

/*
 * A simple syntax group ID comparison function suitable for use in qsort()
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_compare_stub(ଅଚଳ ଶୂନ୍ୟ *v1, ଅଚଳ ଶୂନ୍ୟ *v2)
{
    ଅଚଳ short	*s1 = v1;
    ଅଚଳ short	*s2 = v2;

    ଫେରନ୍ତୁ (*s1 > *s2 ? 1 : *s1 < *s2 ? -1 : 0);
}

/*
 * Combines lists of syntax clusters.
 * *clstr1 and *clstr2 must both be allocated memory; they will be consumed.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_combine_list(short **clstr1, short **clstr2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା list_op)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		count1 = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		count2 = 0;
    short	*g1;
    short	*g2;
    short	*clstr = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		round;

    /*
     * Handle degenerate cases.
     */
    ଯଦି (*clstr2 == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି (*clstr1 == ଖାଲି || list_op == CLUSTER_REPLACE)
    {
	ଯଦି (list_op == CLUSTER_REPLACE)
	    vim_free(*clstr1);
	ଯଦି (list_op == CLUSTER_REPLACE || list_op == CLUSTER_ADD)
	    *clstr1 = *clstr2;
	ଅନ୍ୟଥା
	    vim_free(*clstr2);
	ଫେରନ୍ତୁ;
    }

    ପାଇଁ (g1 = *clstr1; *g1; g1++)
	++count1;
    ପାଇଁ (g2 = *clstr2; *g2; g2++)
	++count2;

    /*
     * For speed purposes, sort both lists.
     */
    qsort(*clstr1, (size_t)count1, ଆକାର(short), syn_compare_stub);
    qsort(*clstr2, (size_t)count2, ଆକାର(short), syn_compare_stub);

    /*
     * We proceed in two passes; in round 1, we count the elements to place
     * in the new list, and in round 2, we allocate and populate the new
     * list.  For speed, we use a mergesort-like method, adding the smaller
     * of the current elements in each list to the new list.
     */
    ପାଇଁ (round = 1; round <= 2; round++)
    {
	g1 = *clstr1;
	g2 = *clstr2;
	ଗଣନା_2 = 0;

	/*
	 * First, loop through the lists until one of them is empty.
	 */
	ଯେତେବେଳେ (*g1 && *g2)
	{
	    /*
	     * We always want to add from the first list.
	     */
	    ଯଦି (*g1 < *g2)
	    {
		ଯଦି (round == 2)
		    clstr[ଗଣନା_2] = *g1;
		ଗଣନା_2++;
		g1++;
		ଜାରି;
	    }
	    /*
	     * We only want to add from the second list if we're adding the
	     * lists.
	     */
	    ଯଦି (list_op == CLUSTER_ADD)
	    {
		ଯଦି (round == 2)
		    clstr[ଗଣନା_2] = *g2;
		ଗଣନା_2++;
	    }
	    ଯଦି (*g1 == *g2)
		g1++;
	    g2++;
	}

	/*
	 * Now add the leftovers from whichever list didn't get finished
	 * first.  As before, we only want to add from the second list if
	 * we're adding the lists.
	 */
	ପାଇଁ (; *g1; g1++, ଗଣନା_2++)
	    ଯଦି (round == 2)
		clstr[ଗଣନା_2] = *g1;
	ଯଦି (list_op == CLUSTER_ADD)
	    ପାଇଁ (; *g2; g2++, ଗଣନା_2++)
		ଯଦି (round == 2)
		    clstr[ଗଣନା_2] = *g2;

	ଯଦି (round == 1)
	{
	    /*
	     * If the group ended up empty, we don't need to allocate any
	     * space for it.
	     */
	    ଯଦି (ଗଣନା_2 == 0)
	    {
		clstr = ଖାଲି;
		ଭାଙ୍ଗ;
	    }
	    clstr = ALLOC_MULT(short, ଗଣନା_2 + 1);
	    ଯଦି (clstr == ଖାଲି)
		ଭାଙ୍ଗ;
	    clstr[ଗଣନା_2] = 0;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_scl_name2id(char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*name_u;

    // Avoid using stricmp() too much, it's slow on some systems
    name_u = vim_strsave_up(name);
    ଯଦି (name_u == ଖାଲି)
	ଫେରନ୍ତୁ 0;
    ପାଇଁ (i = curwin->w_s->b_syn_clusters.ga_len; --i >= 0; )
	ଯଦି (SYN_CLSTR(curwin->w_s)[i].scl_name_u != ଖାଲି
		&& STRCMP(name_u, SYN_CLSTR(curwin->w_s)[i].scl_name_u) == 0)
	    ଭାଙ୍ଗ;
    vim_free(name_u);
    ଫେରନ୍ତୁ (i < 0 ? 0 : i + SYNID_CLUSTER);
}

/*
 * Like syn_scl_name2id(), but take a pointer + length argument.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_scl_namen2id(char_u *linep, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    char_u  *name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    id = 0;

    name = vim_strnsave(linep, len);
    ଯଦି (name == ଖାଲି)
	ଫେରନ୍ତୁ 0;

    id = syn_scl_name2id(name);
    vim_free(name);
    ଫେରନ୍ତୁ id;
}

/*
 * Find syntax cluster name in the table and return its ID.
 * The argument is a pointer to the name and the length of the name.
 * If it doesn't exist yet, a new entry is created.
 * Return 0 for failure.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_check_cluster(char_u *pp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
    char_u	*name;

    name = vim_strnsave(pp, len);
    ଯଦି (name == ଖାଲି)
	ଫେରନ୍ତୁ 0;

    id = syn_scl_name2id(name);
    ଯଦି (id == 0)			// doesn't exist yet
	id = syn_add_cluster(name);
    ଅନ୍ୟଥା
	vim_free(name);
    ଫେରନ୍ତୁ id;
}

/*
 * Add new syntax cluster and return its ID.
 * "name" must be an allocated string, it will be consumed.
 * Return 0 for failure.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_add_cluster(char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    /*
     * First call for this growarray: init growing array.
     */
    ଯଦି (curwin->w_s->b_syn_clusters.ga_data == ଖାଲି)
    {
	curwin->w_s->b_syn_clusters.ga_itemsize = ଆକାର(syn_cluster_T);
	curwin->w_s->b_syn_clusters.ga_growsize = 10;
    }

    len = curwin->w_s->b_syn_clusters.ga_len;
    ଯଦି (len >= MAX_CLUSTER_ID)
    {
	emsg(_(e_too_many_syntax_clusters));
	vim_free(name);
	ଫେରନ୍ତୁ 0;
    }

    /*
     * Make room for at least one other cluster entry.
     */
    ଯଦି (ga_grow(&curwin->w_s->b_syn_clusters, 1) == FAIL)
    {
	vim_free(name);
	ଫେରନ୍ତୁ 0;
    }

    CLEAR_POINTER(&(SYN_CLSTR(curwin->w_s)[len]));
    SYN_CLSTR(curwin->w_s)[len].scl_name = name;
    SYN_CLSTR(curwin->w_s)[len].scl_name_u = vim_strsave_up(name);
    SYN_CLSTR(curwin->w_s)[len].scl_list = ଖାଲି;
    ++curwin->w_s->b_syn_clusters.ga_len;

    ଯଦି (STRICMP(name, "Spell") == 0)
	curwin->w_s->b_spell_cluster_id = len + SYNID_CLUSTER;
    ଯଦି (STRICMP(name, "NoSpell") == 0)
	curwin->w_s->b_nospell_cluster_id = len + SYNID_CLUSTER;

    ଫେରନ୍ତୁ len + SYNID_CLUSTER;
}

/*
 * Handle ":syntax cluster {cluster-name} [contains={groupname},..]
 *		[add={groupname},..] [remove={groupname},..]".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_cmd_cluster(exarg_T *eap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*group_name_end;
    char_u	*rest;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		scl_id;
    short	*clstr_list;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		got_clstr = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		list_op;

    eap->nextcmd = find_nextcmd(arg);
    ଯଦି (eap->skip)
	ଫେରନ୍ତୁ;

    rest = get_group_name(arg, &group_name_end);

    ଯଦି (rest != ଖାଲି)
    {
	scl_id = syn_check_cluster(arg, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(group_name_end - arg));
	ଯଦି (scl_id == 0)
	    ଫେରନ୍ତୁ;
	scl_id -= SYNID_CLUSTER;

	ପାଇଁ (;;)
	{
	    ଯଦି (STRNICMP(rest, "add", 3) == 0
		    && (VIM_ISWHITE(rest[3]) || rest[3] == '='))
	    {
		opt_len = 3;
		list_op = CLUSTER_ADD;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRNICMP(rest, "remove", 6) == 0
		    && (VIM_ISWHITE(rest[6]) || rest[6] == '='))
	    {
		opt_len = 6;
		list_op = CLUSTER_SUBTRACT;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRNICMP(rest, "contains", 8) == 0
			&& (VIM_ISWHITE(rest[8]) || rest[8] == '='))
	    {
		opt_len = 8;
		list_op = CLUSTER_REPLACE;
	    }
	    ଅନ୍ୟଥା
		ଭାଙ୍ଗ;

	    clstr_list = ଖାଲି;
	    ଯଦି (get_id_list(&rest, opt_len, &clstr_list, eap->skip) == FAIL)
	    {
		semsg(_(e_invalid_argument_str), rest);
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (scl_id >= 0)
		syn_combine_list(&SYN_CLSTR(curwin->w_s)[scl_id].scl_list,
			     &clstr_list, list_op);
	    ଅନ୍ୟଥା
		vim_free(clstr_list);
	    got_clstr = TRUE;
	}

	ଯଦି (got_clstr)
	{
	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_ଗଦା_free_all(curwin->w_s);	// Need to recompute all.
	}
    }

    ଯଦି (!got_clstr)
	emsg(_(e_no_cluster_specified));
    ଯଦି (rest == ଖାଲି || !ends_excmd2(eap->cmd, rest))
	semsg(_(e_invalid_argument_str), arg);
}

/*
 * On first call for current buffer: Init growing array.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
init_syn_patterns(ଶୂନ୍ୟ)
{
    curwin->w_s->b_syn_patterns.ga_itemsize = ଆକାର(synpat_T);
    curwin->w_s->b_syn_patterns.ga_growsize = 10;
}

/*
 * Get one pattern for a ":syntax match" or ":syntax region" command.
 * Stores the pattern and program in a synpat_T.
 * Returns a pointer to the next argument, or NULL in case of an error.
 */
    ସ୍ଥିର char_u *
get_syn_pattern(char_u *arg, synpat_T *ci)
{
    char_u	*end;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    char_u	*cpo_save;

    // need at least three chars
    ଯଦି (arg == ଖାଲି || arg[0] == NUL || arg[1] == NUL || arg[2] == NUL)
	ଫେରନ୍ତୁ ଖାଲି;

    end = skip_regexp(arg + 1, *arg, TRUE);
    ଯଦି (*end != *arg)			    // end delimiter not found
    {
	semsg(_(e_pattern_delimiter_not_found_str), arg);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    // store the pattern and compiled regexp program
    ଯଦି ((ci->sp_pattern = vim_strnsave(arg + 1, end - arg - 1)) == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // Make 'cpoptions' empty, to avoid the 'l' flag
    cpo_save = p_cpo;
    p_cpo = empty_option;
    ci->sp_prog = vim_regcomp(ci->sp_pattern, RE_MAGIC);
    p_cpo = cpo_save;

    ଯଦି (ci->sp_prog == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ci->sp_ic = curwin->w_s->b_syn_ic;
#ifdef FEAT_PROFILE
    syn_clear_time(&ci->sp_time);
#endif

    /*
     * Check for a match, highlight or region offset.
     */
    ++end;
    କର
    {
	ପାଇଁ (idx = SPO_ଗଣନା; --idx >= 0; )
	    ଯଦି (STRNCMP(end, spo_name_tab[idx], 3) == 0)
		ଭାଙ୍ଗ;
	ଯଦି (idx >= 0)
	{
	    p = &(ci->sp_offsets[idx]);
	    ଯଦି (idx != SPO_LC_OFF)
		ବାଛ (end[3])
		{
		    ସ୍ଥିତି 's':   ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'b':   ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'e':   idx += SPO_ଗଣନା; ଭାଙ୍ଗ;
		    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:    idx = -1; ଭାଙ୍ଗ;
		}
	    ଯଦି (idx >= 0)
	    {
		ci->sp_off_flags |= (1 << idx);
		ଯଦି (idx == SPO_LC_OFF)	    // lc=99
		{
		    end += 3;
		    *p = getdigits(&end);

		    // "lc=" offset automatically sets "ms=" offset
		    ଯଦି (!(ci->sp_off_flags & (1 << SPO_MS_OFF)))
		    {
			ci->sp_off_flags |= (1 << SPO_MS_OFF);
			ci->sp_offsets[SPO_MS_OFF] = *p;
		    }
		}
		ଅନ୍ୟଥା			    // yy=x+99
		{
		    end += 4;
		    ଯଦି (*end == '+')
		    {
			++end;
			*p = getdigits(&end);		// positive offset
		    }
		    ଅନ୍ୟଥା ଯଦି (*end == '-')
		    {
			++end;
			*p = -getdigits(&end);		// negative offset
		    }
		}
		ଯଦି (*end != ',')
		    ଭାଙ୍ଗ;
		++end;
	    }
	}
    } ଯେତେବେଳେ (idx >= 0);

    ଯଦି (!ends_excmd2(arg, end) && !VIM_ISWHITE(*end))
    {
	semsg(_(e_garbage_after_pattern_str), arg);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ skipwhite(end);
}

/*
 * Handle ":syntax sync .." command.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syn_cmd_sync(exarg_T *eap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା syncing UNUSED)
{
    char_u	*arg_start = eap->arg;
    char_u	*arg_end;
    char_u	*ଚାବି = ଖାଲି;
    char_u	*next_arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		illegal = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		finished = FALSE;
    long	n;
    char_u	*cpo_save;

    ଯଦି (ends_excmd2(eap->cmd, arg_start))
    {
	syn_cmd_list(eap, TRUE);
	ଫେରନ୍ତୁ;
    }

    ଯେତେବେଳେ (!ends_excmd2(eap->cmd, arg_start))
    {
	arg_end = skiptowhite(arg_start);
	next_arg = skipwhite(arg_end);
	vim_free(ଚାବି);
	ଚାବି = vim_strnsave_up(arg_start, arg_end - arg_start);
	ଯଦି (ଚାବି == ଖାଲି)
	    ଭାଙ୍ଗ;
	ଯଦି (STRCMP(ଚାବି, "CCOMMENT") == 0)
	{
	    ଯଦି (!eap->skip)
		curwin->w_s->b_syn_sync_flags |= SF_CCOMMENT;
	    ଯଦି (!ends_excmd2(eap->cmd, next_arg))
	    {
		arg_end = skiptowhite(next_arg);
		ଯଦି (!eap->skip)
		    curwin->w_s->b_syn_sync_id = syn_check_group(next_arg,
						   (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(arg_end - next_arg));
		next_arg = skipwhite(arg_end);
	    }
	    ଅନ୍ୟଥା ଯଦି (!eap->skip)
		curwin->w_s->b_syn_sync_id = syn_name2id((char_u *)"Comment");
	}
	ଅନ୍ୟଥା ଯଦି (  STRNCMP(ଚାବି, "LINES", 5) == 0
		|| STRNCMP(ଚାବି, "MINLINES", 8) == 0
		|| STRNCMP(ଚାବି, "MAXLINES", 8) == 0
		|| STRNCMP(ଚାବି, "LINEBREAKS", 10) == 0)
	{
	    ଯଦି (ଚାବି[4] == 'S')
		arg_end = ଚାବି + 6;
	    ଅନ୍ୟଥା ଯଦି (ଚାବି[0] == 'L')
		arg_end = ଚାବି + 11;
	    ଅନ୍ୟଥା
		arg_end = ଚାବି + 9;
	    ଯଦି (arg_end[-1] != '=' || !VIM_ISDIGIT(*arg_end))
	    {
		illegal = TRUE;
		ଭାଙ୍ଗ;
	    }
	    n = getdigits(&arg_end);
	    ଯଦି (!eap->skip)
	    {
		ଯଦି (ଚାବି[4] == 'B')
		    curwin->w_s->b_syn_sync_linebreaks = n;
		ଅନ୍ୟଥା ଯଦି (ଚାବି[1] == 'A')
		    curwin->w_s->b_syn_sync_maxlines = n;
		ଅନ୍ୟଥା
		    curwin->w_s->b_syn_sync_minlines = n;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (STRCMP(ଚାବି, "FROMSTART") == 0)
	{
	    ଯଦି (!eap->skip)
	    {
		curwin->w_s->b_syn_sync_minlines = MAXLNUM;
		curwin->w_s->b_syn_sync_maxlines = 0;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (STRCMP(ଚାବି, "LINECONT") == 0)
	{
	    ଯଦି (*next_arg == NUL)	   // missing pattern
	    {
		illegal = TRUE;
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (curwin->w_s->b_syn_linecont_pat != ଖାଲି)
	    {
		emsg(_(e_syntax_sync_ଧାଡ଼ି_continuations_pattern_specified_twice));
		finished = TRUE;
		ଭାଙ୍ଗ;
	    }
	    arg_end = skip_regexp(next_arg + 1, *next_arg, TRUE);
	    ଯଦି (*arg_end != *next_arg)	    // end delimiter not found
	    {
		illegal = TRUE;
		ଭାଙ୍ଗ;
	    }

	    ଯଦି (!eap->skip)
	    {
		// store the pattern and compiled regexp program
		ଯଦି ((curwin->w_s->b_syn_linecont_pat =
			    vim_strnsave(next_arg + 1,
				      arg_end - next_arg - 1)) == ଖାଲି)
		{
		    finished = TRUE;
		    ଭାଙ୍ଗ;
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

		ଯଦି (curwin->w_s->b_syn_linecont_prog == ଖାଲି)
		{
		    VIM_CLEAR(curwin->w_s->b_syn_linecont_pat);
		    finished = TRUE;
		    ଭାଙ୍ଗ;
		}
	    }
	    next_arg = skipwhite(arg_end + 1);
	}
	ଅନ୍ୟଥା
	{
	    eap->arg = next_arg;
	    ଯଦି (STRCMP(ଚାବି, "MATCH") == 0)
		syn_cmd_match(eap, TRUE);
	    ଅନ୍ୟଥା ଯଦି (STRCMP(ଚାବି, "REGION") == 0)
		syn_cmd_region(eap, TRUE);
	    ଅନ୍ୟଥା ଯଦି (STRCMP(ଚାବି, "CLEAR") == 0)
		syn_cmd_clear(eap, TRUE);
	    ଅନ୍ୟଥା
		illegal = TRUE;
	    finished = TRUE;
	    ଭାଙ୍ଗ;
	}
	arg_start = next_arg;
    }
    vim_free(ଚାବି);
    ଯଦି (illegal)
	semsg(_(e_illegal_arguments_str), arg_start);
    ଅନ୍ୟଥା ଯଦି (!finished)
    {
	set_nextcmd(eap, arg_start);
	redraw_curbuf_later(UPD_SOME_VALID);
	syn_ଗଦା_free_all(curwin->w_s);	// Need to recompute all syntax.
    }
}

/*
 * Convert a line of highlight group names into a list of group ID numbers.
 * "arg" should point to the "contains" or "nextgroup" keyword.
 * "arg" is advanced to after the last group name.
 * Careful: the argument is modified (NULs added).
 * returns FAIL for some error, OK for success.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_id_list(
    char_u	**arg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		keylen,		// length of keyword
    short	**list,		// where to store the resulting list, if not
				// NULL, the list is silently skipped!
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skip)
{
    char_u	*p = ଖାଲି;
    char_u	*end;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		round;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		total_ଗଣନା = 0;
    short	*retval = ଖାଲି;
    char_u	*name;
    regmatch_T	regmatch;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		failed = FALSE;

    /*
     * We parse the list twice:
     * round == 1: count the number of items, allocate the array.
     * round == 2: fill the array with the items.
     * In round 1 new groups may be added, causing the number of items to
     * grow when a regexp is used.  In that case round 1 is done once again.
     */
    ପାଇଁ (round = 1; round <= 2; ++round)
    {
	/*
	 * skip "contains"
	 */
	p = skipwhite(*arg + keylen);
	ଯଦି (*p != '=')
	{
	    semsg(_(e_missing_equal_sign_str), *arg);
	    ଭାଙ୍ଗ;
	}
	p = skipwhite(p + 1);
	ଯଦି (ends_excmd2(*arg, p))
	{
	    semsg(_(e_empty_argument_str), *arg);
	    ଭାଙ୍ଗ;
	}

	/*
	 * parse the arguments after "contains"
	 */
	ଗଣନା_2 = 0;
	ଯେତେବେଳେ (!ends_excmd2(*arg, p))
	{
	    ପାଇଁ (end = p; *end && !VIM_ISWHITE(*end) && *end != ','; ++end)
		;
	    name = alloc(end - p + 3);	    // leave room for "^$"
	    ଯଦି (name == ଖାଲି)
	    {
		failed = TRUE;
		ଭାଙ୍ଗ;
	    }
	    vim_strncpy(name + 1, p, end - p);
	    ଯଦି (       STRCMP(name + 1, "ALLBUT") == 0
		    || STRCMP(name + 1, "ALL") == 0
		    || STRCMP(name + 1, "TOP") == 0
		    || STRCMP(name + 1, "CONTAINED") == 0)
	    {
		ଯଦି (TOUPPER_ASC(**arg) != 'C')
		{
		    semsg(_(e_str_not_allowed_here), name + 1);
		    failed = TRUE;
		    vim_free(name);
		    ଭାଙ୍ଗ;
		}
		ଯଦି (ଗଣନା_2 != 0)
		{
		    semsg(_(e_str_must_be_first_in_contains_list), name + 1);
		    failed = TRUE;
		    vim_free(name);
		    ଭାଙ୍ଗ;
		}
		ଯଦି (name[1] == 'A')
		    id = SYNID_ALLBUT;
		ଅନ୍ୟଥା ଯଦି (name[1] == 'T')
		    id = SYNID_ଶୀର୍ଷ;
		ଅନ୍ୟଥା
		    id = SYNID_CONTAINED;
		id += current_syn_inc_tag;
	    }
	    ଅନ୍ୟଥା ଯଦି (name[1] == '@')
	    {
		ଯଦି (skip)
		    id = -1;
		ଅନ୍ୟଥା
		    id = syn_check_cluster(name + 2, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(end - p - 1));
	    }
	    ଅନ୍ୟଥା
	    {
		/*
		 * Handle full group name.
		 */
		ଯଦି (vim_strpbrk(name + 1, (char_u *)"\\.*^$~[") == ଖାଲି)
		    id = syn_check_group(name + 1, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(end - p));
		ଅନ୍ୟଥା
		{
		    /*
		     * Handle match of regexp with group names.
		     */
		    *name = '^';
		    STRCAT(name, "$");
		    regmatch.regprog = vim_regcomp(name, RE_MAGIC);
		    ଯଦି (regmatch.regprog == ଖାଲି)
		    {
			failed = TRUE;
			vim_free(name);
			ଭାଙ୍ଗ;
		    }

		    regmatch.rm_ic = TRUE;
		    id = 0;
		    ପାଇଁ (i = highlight_ଅଙ୍କ_groups(); --i >= 0; )
		    {
			ଯଦି (vim_regexec(&regmatch, highlight_group_name(i),
								  (colnr_T)0))
			{
			    ଯଦି (round == 2)
			    {
				// Got more items than expected; can happen
				// when adding items that match:
				// "contains=a.*b,axb".
				// Go back to first round
				ଯଦି (ଗଣନା_2 >= total_ଗଣନା)
				{
				    vim_free(retval);
				    round = 1;
				}
				ଅନ୍ୟଥା
				    retval[ଗଣନା_2] = i + 1;
			    }
			    ++ଗଣନା_2;
			    id = -1;	    // remember that we found one
			}
		    }
		    vim_regfree(regmatch.regprog);
		}
	    }
	    vim_free(name);
	    ଯଦି (id == 0)
	    {
		semsg(_(e_unknown_group_name_str), p);
		failed = TRUE;
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (id > 0)
	    {
		ଯଦି (round == 2)
		{
		    // Got more items than expected, go back to first round
		    ଯଦି (ଗଣନା_2 >= total_ଗଣନା)
		    {
			vim_free(retval);
			round = 1;
		    }
		    ଅନ୍ୟଥା
			retval[ଗଣନା_2] = id;
		}
		++ଗଣନା_2;
	    }
	    p = skipwhite(end);
	    ଯଦି (*p != ',')
		ଭାଙ୍ଗ;
	    p = skipwhite(p + 1);	// skip comma in between arguments
	}
	ଯଦି (failed)
	    ଭାଙ୍ଗ;
	ଯଦି (round == 1)
	{
	    retval = ALLOC_MULT(short, ଗଣନା_2 + 1);
	    ଯଦି (retval == ଖାଲି)
		ଭାଙ୍ଗ;
	    retval[ଗଣନା_2] = 0;	    // zero means end of the list
	    total_ଗଣନା = ଗଣନା_2;
	}
    }

    *arg = p;
    ଯଦି (failed || retval == ଖାଲି)
    {
	vim_free(retval);
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (*list == ଖାଲି)
	*list = retval;
    ଅନ୍ୟଥା
	vim_free(retval);	// list already found, don't overwrite it

    ଫେରନ୍ତୁ OK;
}

/*
 * Make a copy of an ID list.
 */
    ସ୍ଥିର short *
copy_id_list(short *list)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଗଣନା_2;
    short   *retval;

    ଯଦି (list == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ପାଇଁ (ଗଣନା_2 = 0; list[ଗଣନା_2]; ++ଗଣନା_2)
	;
    len = (ଗଣନା_2 + 1) * ଆକାର(short);
    retval = alloc(len);
    ଯଦି (retval != ଖାଲି)
	mch_memmove(retval, list, (size_t)len);

    ଫେରନ୍ତୁ retval;
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

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    short	*idl_ଚାବି;	// the list this entry resolves (NULL when empty)
    short	*idl_ids;	// sorted, cluster-expanded group IDs (allocated)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idl_ଗଣନା;	// number of IDs in idl_ids
    short	idl_marker;	// leading ALLBUT/TOP/CONTAINED item, or 0
    bool	idl_usable;	// false: not cacheable, use the slow path
} idl_entry_T;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    idl_entry_T	ent[IDL_CACHE_SIZE];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		round;		// round-robin replacement index
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		last;		// index of the last returned entry (fast path)
} idl_cache_T;

/*
 * Collect the explicit group IDs of "list" into "ga", expanding clusters.
 * "seen" records visited clusters to break cycles and avoid duplicate work.
 * Return false when the list cannot be represented as a plain set (a nested
 * ALLBUT/TOP/CONTAINED marker) or on out of memory.
 */
    ସ୍ଥିର bool
idl_collect(short *list, garray_T *ga, garray_T *seen)
{
    ଯଦି (list == ଖାଲି || list == ID_LIST_ALL)
	ଫେରନ୍ତୁ false;
    ପାଇଁ (short item = *list; item != 0; item = *++list)
    {
	ଯଦି (item >= SYNID_ALLBUT && item < SYNID_CLUSTER)
	    ଫେରନ୍ତୁ false;	// nested ALLBUT/TOP/CONTAINED: not cacheable
	ଯଦି (item >= SYNID_CLUSTER)
	{
	    short	*scl_list;
	    bool	seen_it = false;

	    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < seen->ga_len; ++i)
		ଯଦି (((short *)seen->ga_data)[i] == item)
		{
		    seen_it = true;
		    ଭାଙ୍ଗ;
		}
	    ଯଦି (seen_it)
		ଜାରି;
	    ଯଦି (ga_grow(seen, 1) == FAIL)
		ଫେରନ୍ତୁ false;
	    ((short *)seen->ga_data)[seen->ga_len++] = item;
	    // Record the cluster id itself, not just its members: the slow
	    // path matches "item == id" for a cluster id present in the list.
	    ଯଦି (ga_grow(ga, 1) == FAIL)
		ଫେରନ୍ତୁ false;
	    ((short *)ga->ga_data)[ga->ga_len++] = item;
	    scl_list = SYN_CLSTR(syn_block)[item - SYNID_CLUSTER].scl_list;
	    ଯଦି (scl_list != ଖାଲି && !idl_collect(scl_list, ga, seen))
		ଫେରନ୍ତୁ false;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (ga_grow(ga, 1) == FAIL)
		ଫେରନ୍ତୁ false;
	    ((short *)ga->ga_data)[ga->ga_len++] = item;
	}
    }
    ଫେରନ୍ତୁ true;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
idl_id_cmp(ଅଚଳ ଶୂନ୍ୟ *a, ଅଚଳ ଶୂନ୍ୟ *b)
{
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)*(ଅଚଳ short *)a - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)*(ଅଚଳ short *)b;
}

/*
 * Get the cache entry that resolves "list", building it if needed.
 * Returns NULL only on allocation failure.
 */
    ସ୍ଥିର idl_entry_T *
idl_get_entry(short *list)
{
    idl_cache_T	*cache = (idl_cache_T *)syn_block->b_idlist_cache;
    idl_entry_T	*e;
    garray_T	ga, seen;

    ଯଦି (cache == ଖାଲି)
    {
	cache = ALLOC_CLEAR_ONE(idl_cache_T);
	ଯଦି (cache == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	syn_block->b_idlist_cache = cache;
    }
    // Fast path: the same list is usually queried many times in a row.
    ଯଦି (cache->ent[cache->last].idl_ଚାବି == list)
	ଫେରନ୍ତୁ &cache->ent[cache->last];
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < IDL_CACHE_SIZE; ++i)
	ଯଦି (cache->ent[i].idl_ଚାବି == list)
	{
	    cache->last = i;
	    ଫେରନ୍ତୁ &cache->ent[i];
	}

    // Build a new entry, replacing one in round-robin order.
    cache->last = cache->round;
    e = &cache->ent[cache->round];
    cache->round = (cache->round + 1) % IDL_CACHE_SIZE;
    VIM_CLEAR(e->idl_ids);
    e->idl_ଗଣନା = 0;
    e->idl_ଚାବି = list;
    e->idl_marker = 0;
    e->idl_usable = true;

    // A leading ALLBUT/TOP/CONTAINED marker is kept aside; the rest is the set.
    ଯଦି (*list >= SYNID_ALLBUT && *list < SYNID_CLUSTER)
    {
	e->idl_marker = *list;
	++list;
    }

    ga_init2(&ga, ଆକାର(short), 50);
    ga_init2(&seen, ଆକାର(short), 10);
    ଯଦି (!idl_collect(list, &ga, &seen))
    {
	e->idl_usable = false;
	ga_clear(&ga);
	ga_clear(&seen);
	ଫେରନ୍ତୁ e;
    }
    ga_clear(&seen);
    ଯଦି (ga.ga_len > 0)
    {
	qsort(ga.ga_data, (size_t)ga.ga_len, ଆକାର(short), idl_id_cmp);
	e->idl_ids = (short *)ga.ga_data;	// take ownership
	e->idl_ଗଣନା = ga.ga_len;
    }
    ଅନ୍ୟଥା
	ga_clear(&ga);
    ଫେରନ୍ତୁ e;
}

/*
 * Return true if "id" is in the resolved set of cache entry "e".
 */
    ସ୍ଥିର bool
idl_contains(idl_entry_T *e, short id)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	lo = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	hi = e->idl_ଗଣନା - 1;

    ଯେତେବେଳେ (lo <= hi)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	mid = (lo + hi) / 2;
	short	v = e->idl_ids[mid];

	ଯଦି (v == id)
	    ଫେରନ୍ତୁ true;
	ଯଦି (v < id)
	    lo = mid + 1;
	ଅନ୍ୟଥା
	    hi = mid - 1;
    }
    ଫେରନ୍ତୁ false;
}

    ସ୍ଥିର ଶୂନ୍ୟ
idl_cache_clear(synblock_T *block)
{
    idl_cache_T	*cache = (idl_cache_T *)block->b_idlist_cache;

    ଯଦି (cache == ଖାଲି)
	ଫେରନ୍ତୁ;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < IDL_CACHE_SIZE; ++i)
	vim_free(cache->ent[i].idl_ids);
    vim_free(cache);
    block->b_idlist_cache = ଖାଲି;
}
// }}}

/*
 * Check if syntax group "ssp" is in the ID list "list" of "cur_si".
 * "cur_si" can be NULL if not checking the "containedin" list.
 * Used to check if a syntax item is in the "contains" or "nextgroup" list of
 * the current item.
 * This function is called very often, keep it fast!!
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
in_id_list(
    stateitem_T	*cur_si,	// current item or NULL
    short	*list,		// id list
    ସଂରଚନା sp_syn *ssp,		// group id and ":syn include" tag of group
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)		// group flags
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval;
    short	*scl_list;
    short	item;
    short	id = ssp->id;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	depth = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		toplevel;
    idl_entry_T	*e;

    // If ssp has a "containedin" list and "cur_si" is in it, return TRUE.
    ଯଦି (cur_si != ଖାଲି && ssp->cont_in_list != ଖାଲି
					    && !(cur_si->si_flags & HL_MATCH))
    {
	// Ignore transparent items without a contains argument.  Double check
	// that we don't go back past the first one.
	ଯେତେବେଳେ ((cur_si->si_flags & HL_TRANS_CONT)
		&& cur_si > (stateitem_T *)(current_state.ga_data))
	    --cur_si;
	// cur_si->si_idx is -1 for keywords, these never contain anything.
	ଯଦି (cur_si->si_idx >= 0 && in_id_list(ଖାଲି, ssp->cont_in_list,
		&(SYN_ବସ୍ତୁଗୁଡ଼ିକ(syn_block)[cur_si->si_idx].sp_syn),
		  SYN_ବସ୍ତୁଗୁଡ଼ିକ(syn_block)[cur_si->si_idx].sp_flags))
	    ଫେରନ୍ତୁ TRUE;
    }

    ଯଦି (list == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    /*
     * If list is ID_LIST_ALL, we are in a transparent item that isn't
     * inside anything.  Only allow not-contained groups.
     */
    ଯଦି (list == ID_LIST_ALL)
	ଫେରନ୍ତୁ !(flags & HL_CONTAINED);

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
    e = disable_syn_idlist_cache_for_testing ? ଖାଲି : idl_get_entry(list);
    ଯଦି (e != ଖାଲି && e->idl_usable)
    {
	ଯଦି (e->idl_marker != 0)
	{
	    item = e->idl_marker;
	    ଯଦି (item < SYNID_ଶୀର୍ଷ)
	    {
		ଯଦି (item - SYNID_ALLBUT != ssp->inc_tag)
		    ଫେରନ୍ତୁ FALSE;
	    }
	    ଅନ୍ୟଥା ଯଦି (item < SYNID_CONTAINED)
	    {
		ଯଦି (item - SYNID_ଶୀର୍ଷ != ssp->inc_tag || !toplevel)
		    ଫେରନ୍ତୁ FALSE;
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (item - SYNID_CONTAINED != ssp->inc_tag || toplevel)
		    ଫେରନ୍ତୁ FALSE;
	    }
	    retval = FALSE;
	}
	ଅନ୍ୟଥା
	    retval = TRUE;
	ଫେରନ୍ତୁ idl_contains(e, id) ? retval : !retval;
    }

    /*
     * Slow path (uncacheable list, e.g. a nested ALLBUT).
     * If the first item is "ALLBUT", return TRUE if "id" is NOT in the
     * contains list.  We also require that "id" is at the same ":syn include"
     * level as the list.
     */
    item = *list;
    ଯଦି (item >= SYNID_ALLBUT && item < SYNID_CLUSTER)
    {
	ଯଦି (item < SYNID_ଶୀର୍ଷ)
	{
	    // ALL or ALLBUT: accept all groups in the same file
	    ଯଦି (item - SYNID_ALLBUT != ssp->inc_tag)
		ଫେରନ୍ତୁ FALSE;
	}
	ଅନ୍ୟଥା ଯଦି (item < SYNID_CONTAINED)
	{
	    // TOP: accept all not-contained groups in the same file
	    ଯଦି (item - SYNID_ଶୀର୍ଷ != ssp->inc_tag || !toplevel)
		ଫେରନ୍ତୁ FALSE;
	}
	ଅନ୍ୟଥା
	{
	    // CONTAINED: accept all contained groups in the same file
	    ଯଦି (item - SYNID_CONTAINED != ssp->inc_tag || toplevel)
		ଫେରନ୍ତୁ FALSE;
	}
	item = *++list;
	retval = FALSE;
    }
    ଅନ୍ୟଥା
	retval = TRUE;

    /*
     * Return "retval" if id is in the contains list.
     */
    ଯେତେବେଳେ (item != 0)
    {
	ଯଦି (item == id)
	    ଫେରନ୍ତୁ retval;
	ଯଦି (item >= SYNID_CLUSTER)
	{
	    scl_list = SYN_CLSTR(syn_block)[item - SYNID_CLUSTER].scl_list;
	    // restrict recursiveness to 30 to avoid an endless loop for a
	    // cluster that includes itself (indirectly)
	    ଯଦି (scl_list != ଖାଲି && depth < 30)
	    {
		++depth;
		r = in_id_list(ଖାଲି, scl_list, ssp, flags);
		--depth;
		ଯଦି (r)
		    ଫେରନ୍ତୁ retval;
	    }
	}
	item = *++list;
    }
    ଫେରନ୍ତୁ !retval;
}

ସଂରଚନା subcommand
{
    ଅକ୍ଷର    *name;			// subcommand name
    ଶୂନ୍ୟ    (*func)(exarg_T *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);	// function to call
};

ସ୍ଥିର ସଂରଚନା subcommand subcommands[] =
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
    ଶୂନ୍ୟ
ex_syntax(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*subcmd_end;
    char_u	*subcmd_name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    syn_cmdlinep = eap->cmdlinep;

    // isolate subcommand name
    ପାଇଁ (subcmd_end = arg; ASCII_ISALPHA(*subcmd_end); ++subcmd_end)
	;
    subcmd_name = vim_strnsave(arg, subcmd_end - arg);
    ଯଦି (subcmd_name == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (eap->skip)		// skip error messages for all subcommands
	++emsg_skip;
    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(subcommands); ++i)
    {
	ଯଦି (STRCMP(subcmd_name, (char_u *)subcommands[i].name) == 0)
	{
	    eap->arg = skipwhite(subcmd_end);
	    (subcommands[i].func)(eap, FALSE);
	    ଭାଙ୍ଗ;
	}
    }

    ଯଦି (i == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(subcommands))
	semsg(_(e_invalid_syntax_subcommand_str), subcmd_name);

    vim_free(subcmd_name);
    ଯଦି (eap->skip)
	--emsg_skip;
}

    ଶୂନ୍ୟ
ex_ownsyntax(exarg_T *eap)
{
    char_u	*old_ମୂଲ୍ୟ;
    char_u	*new_ମୂଲ୍ୟ_2;

    ଯଦି (curwin->w_s == &curwin->w_buffer->b_s)
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
    old_ମୂଲ୍ୟ = get_var_ମୂଲ୍ୟ((char_u *)"b:current_syntax");
    ଯଦି (old_ମୂଲ୍ୟ != ଖାଲି)
	old_ମୂଲ୍ୟ = vim_strsave(old_ମୂଲ୍ୟ);

    // Apply the "syntax" autocommand event, this finds and loads the syntax
    // file.
    apply_autocmds(EVENT_SYNTAX, eap->arg, curbuf->b_fname, TRUE, curbuf);

    // move value of b:current_syntax to w:current_syntax
    new_ମୂଲ୍ୟ_2 = get_var_ମୂଲ୍ୟ((char_u *)"b:current_syntax");
    ଯଦି (new_ମୂଲ୍ୟ_2 != ଖାଲି)
	set_internal_string_var((char_u *)"w:current_syntax", new_ମୂଲ୍ୟ_2);

    // restore value of b:current_syntax
    ଯଦି (old_ମୂଲ୍ୟ == ଖାଲି)
	do_unlet((char_u *)"b:current_syntax", TRUE);
    ଅନ୍ୟଥା
    {
	set_internal_string_var((char_u *)"b:current_syntax", old_ମୂଲ୍ୟ);
	vim_free(old_ମୂଲ୍ୟ);
    }
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syntax_present(win_T *win)
{
    ଫେରନ୍ତୁ (win->w_s->b_syn_patterns.ga_len != 0
	    || win->w_s->b_syn_clusters.ga_len != 0
	    || win->w_s->b_keywtab.ht_used > 0
	    || win->w_s->b_keywtab_ic.ht_used > 0);
}


ସ୍ଥିର ଗଣନା
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
    ଶୂନ୍ୟ
reset_expand_highlight(ଶୂନ୍ୟ)
{
    include_link = include_default = include_none = 0;
}

/*
 * Handle command line completion for :match and :echohl command: Add "NONE"
 * as highlight group.
 */
    ଶୂନ୍ୟ
set_context_in_echohl_cmd(expand_T *xp, char_u *arg)
{
    xp->xp_context = EXPAND_HIGHLIGHT;
    xp->xp_pattern = arg;
    include_none = 1;
}

/*
 * Handle command line completion for :syntax command.
 */
    ଶୂନ୍ୟ
set_context_in_syntax_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    // Default: expand subcommands
    xp->xp_context = EXPAND_SYNTAX;
    expand_what = EXP_SUBCMD;
    xp->xp_pattern = arg;
    include_link = 0;
    include_default = 0;

    ଯଦି (*arg == NUL)
	ଫେରନ୍ତୁ;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    ଯଦି (*p == NUL)
	ଫେରନ୍ତୁ;

    // past first word
    xp->xp_pattern = skipwhite(p);
    ଯଦି (*skiptowhite(xp->xp_pattern) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    ଅନ୍ୟଥା ଯଦି (STRNICMP(arg, "case", p - arg) == 0)
	expand_what = EXP_CASE;
    ଅନ୍ୟଥା ଯଦି (STRNICMP(arg, "spell", p - arg) == 0)
	expand_what = EXP_SPELL;
    ଅନ୍ୟଥା ଯଦି (STRNICMP(arg, "sync", p - arg) == 0)
	expand_what = EXP_SYNC;
    ଅନ୍ୟଥା ଯଦି (STRNICMP(arg, "list", p - arg) == 0)
    {
	p = skipwhite(p);
	ଯଦି (*p == '@')
	    expand_what = EXP_CLUSTER;
	ଅନ୍ୟଥା
	    xp->xp_context = EXPAND_HIGHLIGHT;
    }
    ଅନ୍ୟଥା ଯଦି (STRNICMP(arg, "keyword", p - arg) == 0
	    || STRNICMP(arg, "region", p - arg) == 0
	    || STRNICMP(arg, "match", p - arg) == 0)
	xp->xp_context = EXPAND_HIGHLIGHT;
    ଅନ୍ୟଥା
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Function given to ExpandGeneric() to obtain the list syntax names for
 * expansion.
 */
    char_u *
get_syntax_name(expand_T *xp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ବାଛ (expand_what)
    {
	ସ୍ଥିତି EXP_SUBCMD:
	    ଯଦି (idx < 0 || idx >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(subcommands))
		ଫେରନ୍ତୁ ଖାଲି;
	    ଫେରନ୍ତୁ (char_u *)subcommands[idx].name;
	ସ୍ଥିତି EXP_CASE:
	{
	    ସ୍ଥିର ଅକ୍ଷର *case_args[] = {"match", "ignore", ଖାଲି};
	    ଫେରନ୍ତୁ (char_u *)case_args[idx];
	}
	ସ୍ଥିତି EXP_SPELL:
	{
	    ସ୍ଥିର ଅକ୍ଷର *spell_args[] =
		{"toplevel", "notoplevel", "default", ଖାଲି};
	    ଫେରନ୍ତୁ (char_u *)spell_args[idx];
	}
	ସ୍ଥିତି EXP_SYNC:
	{
	    ସ୍ଥିର ଅକ୍ଷର *sync_args[] =
		{"ccomment", "clear", "fromstart",
		 "linebreaks=", "linecont", "lines=", "match",
		 "maxlines=", "minlines=", "region", ଖାଲି};
	    ଫେରନ୍ତୁ (char_u *)sync_args[idx];
	}
	ସ୍ଥିତି EXP_CLUSTER:
	{
	    ଯଦି (idx < curwin->w_s->b_syn_clusters.ga_len)
	    {
		vim_snprintf((ଅକ୍ଷର *)xp->xp_buf, EXPAND_BUF_LEN, "@%s",
					 SYN_CLSTR(curwin->w_s)[idx].scl_name);
		ଫେରନ୍ତୁ xp->xp_buf;
	    }
	    ଅନ୍ୟଥା
		ଫେରନ୍ତୁ ଖାଲି;
	}
    }
    ଫେରନ୍ତୁ ଖାଲି;
}


/*
 * Function called for expression evaluation: get syntax ID at file position.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_get_id(
    win_T	*wp,
    long	lnum,
    colnr_T	col,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		trans,	     // remove transparency
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*spellp,     // return: can do spell checking
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		keep_state)  // keep state of char at "col"
{
    // When the position is not after the current position and in the same
    // line of the same window with the same buffer, need to restart parsing.
    ଯଦି (wp != syn_win
	    || wp->w_buffer != syn_buf
	    || lnum != current_lnum
	    || col < current_col)
	syntax_start(wp, lnum);
    ଅନ୍ୟଥା ଯଦି (wp->w_buffer == syn_buf
	    && lnum == current_lnum
	    && col > current_col)
	// next_match may not be correct when moving around, e.g. with the
	// "skip" expression in searchpair()
	next_match_idx = -1;

    (ଶୂନ୍ୟ)get_syntax_attr(col, spellp, keep_state);

    ଫେରନ୍ତୁ (trans ? current_trans_id : current_id);
}

#ଯଦି defined(FEAT_CONCEAL)
/*
 * Get extra information about the syntax item.  Must be called right after
 * get_syntax_attr().
 * Stores the current item sequence nr in "*seqnrp".
 * Returns the current flags.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_syntax_info(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *seqnrp)
{
    *seqnrp = current_seqnr;
    ଫେରନ୍ତୁ current_flags;
}

/*
 * Return conceal substitution character
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_get_sub_char(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ current_sub_char;
}
#endif

#ଯଦି defined(FEAT_EVAL)
/*
 * Return the syntax ID at position "i" in the current stack.
 * The caller must have called syn_get_id() before to fill the stack.
 * Returns -1 when "i" is out of range.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_get_ଗଦା_item(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i)
{
    ଯଦି (i >= current_state.ga_len)
    {
	// Need to invalidate the state, because we didn't properly finish it
	// for the last character, "keep_state" was TRUE.
	invalidate_current_state();
	current_col = MAXCOL;
	ଫେରନ୍ତୁ -1;
    }
    ଫେରନ୍ତୁ CUR_STATE(i).si_id;
}
#endif

#ଯଦି defined(FEAT_FOLDING)
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_cur_foldlevel(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		level = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = 0; i < current_state.ga_len; ++i)
	ଯଦି (CUR_STATE(i).si_flags & HL_FOLD)
	    ++level;
    ଫେରନ୍ତୁ level;
}

/*
 * Function called to get folding level for line "lnum" in window "wp".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_get_foldlevel(win_T *wp, long lnum)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		level = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ତଳ_level;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cur_level;

    // Return quickly when there are no fold items at all.
    ଯଦି (wp->w_s->b_syn_folditems != 0
	    && !wp->w_s->b_syn_error
# ifdef SYN_TIME_LIMIT
	    && !wp->w_s->b_syn_slow
# endif
	    )
    {
	syntax_start(wp, lnum);

	// Start with the fold level at the start of the line.
	level = syn_cur_foldlevel();

	ଯଦି (wp->w_s->b_syn_foldlevel == SYNFLD_MINIMUM)
	{
	    // Find the lowest fold level that is followed by a higher one.
	    cur_level = level;
	    ତଳ_level = cur_level;
	    ଯେତେବେଳେ (!current_finished)
	    {
		(ଶୂନ୍ୟ)syn_current_attr(FALSE, FALSE, ଖାଲି, FALSE);
		cur_level = syn_cur_foldlevel();
		ଯଦି (cur_level < ତଳ_level)
		    ତଳ_level = cur_level;
		ଅନ୍ୟଥା ଯଦି (cur_level > ତଳ_level)
		    level = ତଳ_level;
		++current_col;
	    }
	}
    }
    ଯଦି (level > wp->w_p_fdn)
    {
	level = wp->w_p_fdn;
	ଯଦି (level < 0)
	    level = 0;
    }
    ଫେରନ୍ତୁ level;
}
#endif

#ଯଦି defined(FEAT_PROFILE)
/*
 * ":syntime".
 */
    ଶୂନ୍ୟ
ex_syntime(exarg_T *eap)
{
    ଯଦି (STRCMP(eap->arg, "on") == 0)
	syn_time_on = TRUE;
    ଅନ୍ୟଥା ଯଦି (STRCMP(eap->arg, "off") == 0)
	syn_time_on = FALSE;
    ଅନ୍ୟଥା ଯଦି (STRCMP(eap->arg, "clear") == 0)
	syntime_clear();
    ଅନ୍ୟଥା ଯଦି (STRCMP(eap->arg, "report") == 0)
	syntime_report();
    ଅନ୍ୟଥା
	semsg(_(e_invalid_argument_str), eap->arg);
}

    ସ୍ଥିର ଶୂନ୍ୟ
syn_clear_time(syn_time_T *st)
{
    profile_zero(&st->total);
    profile_zero(&st->slowest);
    st->ଗଣନା_2 = 0;
    st->match = 0;
}

/*
 * Clear the syntax timing for the current buffer.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syntime_clear(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    synpat_T	*spp;

    ଯଦି (!syntax_present(curwin))
    {
	ସନ୍ଦେଶ(_(ସନ୍ଦେଶ_no_ବସ୍ତୁଗୁଡ଼ିକ));
	ଫେରନ୍ତୁ;
    }
    ପାଇଁ (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len; ++idx)
    {
	spp = &(SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx]);
	syn_clear_time(&spp->sp_time);
    }
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":syntime {on,off,clear,report}" command.
 */
    char_u *
get_syntime_arg(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ବାଛ (idx)
    {
	ସ୍ଥିତି 0: ଫେରନ୍ତୁ (char_u *)"on";
	ସ୍ଥିତି 1: ଫେରନ୍ତୁ (char_u *)"off";
	ସ୍ଥିତି 2: ଫେରନ୍ତୁ (char_u *)"clear";
	ସ୍ଥିତି 3: ଫେରନ୍ତୁ (char_u *)"report";
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    proftime_T	total;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		match;
    proftime_T	slowest;
    proftime_T	average;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
    char_u	*pattern;
} time_entry_T;

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
syn_compare_syntime(ଅଚଳ ଶୂନ୍ୟ *v1, ଅଚଳ ଶୂନ୍ୟ *v2)
{
    ଅଚଳ time_entry_T	*s1 = v1;
    ଅଚଳ time_entry_T	*s2 = v2;

    ଫେରନ୍ତୁ profile_cmp(&s1->total, &s2->total);
}

/*
 * Clear the syntax timing for the current buffer.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
syntime_report(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    synpat_T	*spp;
# ଯଦି defined(FEAT_RELTIME)
    proftime_T	tm;
# endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		patlen;
    proftime_T	total_total;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		total_ଗଣନା = 0;
    garray_T    ga;
    time_entry_T *p;

    ଯଦି (!syntax_present(curwin))
    {
	ସନ୍ଦେଶ(_(ସନ୍ଦେଶ_no_ବସ୍ତୁଗୁଡ଼ିକ));
	ଫେରନ୍ତୁ;
    }

    ga_init2(&ga, ଆକାର(time_entry_T), 50);
    profile_zero(&total_total);
    ପାଇଁ (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len; ++idx)
    {
	spp = &(SYN_ବସ୍ତୁଗୁଡ଼ିକ(curwin->w_s)[idx]);
	ଯଦି (spp->sp_time.ଗଣନା_2 > 0)
	{
	    (ଶୂନ୍ୟ)ga_grow(&ga, 1);
	    p = ((time_entry_T *)ga.ga_data) + ga.ga_len;
	    p->total = spp->sp_time.total;
	    profile_add(&total_total, &spp->sp_time.total);
	    p->ଗଣନା_2 = spp->sp_time.ଗଣନା_2;
	    p->match = spp->sp_time.match;
	    total_ଗଣନା += spp->sp_time.ଗଣନା_2;
	    p->slowest = spp->sp_time.slowest;
# ଯଦି defined(FEAT_RELTIME)
	    profile_divide(&spp->sp_time.total, spp->sp_time.ଗଣନା_2, &tm);
	    p->average = tm;
# endif
	    p->id = spp->sp_syn.id;
	    p->pattern = spp->sp_pattern;
	    ++ga.ga_len;
	}
    }

    // Sort on total time. Skip if there are no items to avoid passing NULL
    // pointer to qsort().
    ଯଦି (ga.ga_len > 1)
	qsort(ga.ga_data, (size_t)ga.ga_len, ଆକାର(time_entry_T),
							 syn_compare_syntime);

    ସନ୍ଦେଶ_puts_title(_("  TOTAL      COUNT  MATCH   SLOWEST     AVERAGE   NAME               PATTERN"));
    ସନ୍ଦେଶ_puts("\n");
    ପାଇଁ (idx = 0; idx < ga.ga_len && !got_int; ++idx)
    {
	p = ((time_entry_T *)ga.ga_data) + idx;

	ସନ୍ଦେଶ_puts(profile_ସନ୍ଦେଶ(&p->total));
	ସନ୍ଦେଶ_puts(" "); // make sure there is always a separating space
	ସନ୍ଦେଶ_advance(13);
	ସନ୍ଦେଶ_outnum(p->ଗଣନା_2);
	ସନ୍ଦେଶ_puts(" ");
	ସନ୍ଦେଶ_advance(20);
	ସନ୍ଦେଶ_outnum(p->match);
	ସନ୍ଦେଶ_puts(" ");
	ସନ୍ଦେଶ_advance(26);
	ସନ୍ଦେଶ_puts(profile_ସନ୍ଦେଶ(&p->slowest));
	ସନ୍ଦେଶ_puts(" ");
	ସନ୍ଦେଶ_advance(38);
	ସନ୍ଦେଶ_puts(profile_ସନ୍ଦେଶ(&p->average));
	ସନ୍ଦେଶ_puts(" ");
	ସନ୍ଦେଶ_advance(50);
	ସନ୍ଦେଶ_outtrans(highlight_group_name(p->id - 1));
	ସନ୍ଦେଶ_puts(" ");

	ସନ୍ଦେଶ_advance(69);
	ଯଦି (Columns < 80)
	    len = 20; // will wrap anyway
	ଅନ୍ୟଥା
	    len = Columns - 70;
	patlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p->pattern);
	ଯଦି (len > patlen)
	    len = patlen;
	ସନ୍ଦେଶ_outtrans_len(p->pattern, len);
	ସନ୍ଦେଶ_puts("\n");
    }
    ga_clear(&ga);
    ଯଦି (!got_int)
    {
	ସନ୍ଦେଶ_puts("\n");
	ସନ୍ଦେଶ_puts(profile_ସନ୍ଦେଶ(&total_total));
	ସନ୍ଦେଶ_advance(13);
	ସନ୍ଦେଶ_outnum(total_ଗଣନା);
	ସନ୍ଦେଶ_puts("\n");
    }
}
#endif

#endif // FEAT_SYN_HL
