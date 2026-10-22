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

#অন্তর্ভাৱ "vim.h"

#যদি defined(FEAT_SYN_HL)

// different types of offsets that are possible
#define SPO_MS_OFF	0	// match  start offset
#define SPO_ME_OFF	1	// match  end	offset
#define SPO_HS_OFF	2	// highl. start offset
#define SPO_HE_OFF	3	// highl. end	offset
#define SPO_RS_OFF	4	// region start offset
#define SPO_RE_OFF	5	// region end	offset
#define SPO_LC_OFF	6	// leading context offset
#define SPO_গুনতি	7

স্থির ৱর্ণ *(spo_name_tab[SPO_গুনতি]) =
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
প্রকারনির্দেশ সংরচনা syn_pattern
{
    ৱর্ণ	 sp_type;		// see SPTYPE_ defines below
    ৱর্ণ	 sp_syncing;		// this item used for syncing
    short	 sp_syn_match_id;	// highlight group ID of pattern
    short	 sp_off_flags;		// see below
    পূর্ণাঙ্ক		 sp_offsets[SPO_গুনতি];	// offsets
    পূর্ণাঙ্ক		 sp_flags;		// see HL_ defines below
#ifdef FEAT_CONCEAL
    পূর্ণাঙ্ক		 sp_cchar;		// conceal substitute character
#endif
    পূর্ণাঙ্ক		 sp_ic;			// ignore-case flag for sp_prog
    পূর্ণাঙ্ক		 sp_sync_idx;		// sync item index (syncing only)
    পূর্ণাঙ্ক		 sp_সাৰি_id;		// ID of last line where tried
    পূর্ণাঙ্ক		 sp_startcol;		// next match in sp_line_id line
    short	*sp_cont_list;		// cont. group IDs, if non-zero
    short	*sp_next_list;		// next group IDs, if non-zero
    সংরচনা sp_syn sp_syn;		// struct passed to in_id_list()
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


#define SYN_বস্তুসমূহ(buf)	((synpat_T *)((buf)->b_syn_patterns.ga_data))

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
স্থির পূর্ণাঙ্ক current_attr = 0;	    // attr of current syntax word
#ifdef FEAT_EVAL
স্থির পূর্ণাঙ্ক current_id = 0;	    // ID of current char for syn_get_id()
স্থির পূর্ণাঙ্ক current_trans_id = 0;    // idem, transparency removed
#endif
#ifdef FEAT_CONCEAL
স্থির পূর্ণাঙ্ক current_flags = 0;
স্থির পূর্ণাঙ্ক current_seqnr = 0;
স্থির পূর্ণাঙ্ক current_sub_char = 0;
#endif

প্রকারনির্দেশ সংরচনা syn_cluster_S
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
#define SYNID_শীৰ্ষ	21000	    // syntax group ID for contains=TOP
#define SYNID_CONTAINED	22000	    // syntax group ID for contains=CONTAINED
#define SYNID_CLUSTER	23000	    // first syntax group ID for clusters

#define MAX_SYN_INC_TAG	999	    // maximum before the above overflow
#define MAX_CLUSTER_ID  (32767 - SYNID_CLUSTER)

/*
 * Annoying Hack(TM):  ":syn include" needs this pointer to pass to
 * expand_filename().  Most of the other syntax commands don't need it, so
 * instead of passing it to them, we stow it here.
 */
স্থির char_u **syn_cmdlinep;

/*
 * Another Annoying Hack(TM):  To prevent rules from other ":syn include"'d
 * files from leaking into ALLBUT lists, we assign a unique ID to the
 * rules in each ":syn include"'d file.
 */
স্থির পূর্ণাঙ্ক current_syn_inc_tag = 0;
স্থির পূর্ণাঙ্ক running_syn_inc_tag = 0;

/*
 * In a hashtable item "hi_key" points to "keyword" in a keyentry.
 * This avoids adding a pointer to the hashtable item.
 * KE2HIKEY() converts a var pointer to a hashitem key pointer.
 * HIKEY2KE() converts a hashitem key pointer to a var pointer.
 * HI2KE() converts a hashitem pointer to a var pointer.
 */
স্থির keyentry_T dumkey;
#define KE2HIKEY(kp)  ((kp)->keyword)
#define HIKEY2KE(p)   ((keyentry_T *)((p) - (dumkey.keyword - (char_u *)&dumkey)))
#define HI2KE(hi)      HIKEY2KE((hi)->hi_চাবি)

/*
 * To reduce the time spent in keepend(), remember at which level in the state
 * stack the first item with "keepend" is present.  When "-1", there is no
 * "keepend" on the stack.
 */
স্থির পূর্ণাঙ্ক keepend_level = -1;

স্থির ৱর্ণ বাৰ্তা_no_বস্তুসমূহ[] = N_("No Syntax items defined for this buffer");

/*
 * For the current state we need to remember more than just the idx.
 * When si_m_endpos.lnum is 0, the items other than si_idx are unknown.
 * (The end positions have the column number of the next char)
 */
প্রকারনির্দেশ সংরচনা state_item
{
    পূর্ণাঙ্ক		si_idx;			// index of syntax pattern or
					// KEYWORD_IDX
    পূর্ণাঙ্ক		si_id;			// highlight group ID for keywords
    পূর্ণাঙ্ক		si_trans_id;		// idem, transparency removed
    পূর্ণাঙ্ক		si_m_lnum;		// lnum of the match
    পূর্ণাঙ্ক		si_m_startcol;		// starting column of the match
    lpos_T	si_m_endpos;		// just after end posn of the match
    lpos_T	si_h_startpos;		// start position of the highlighting
    lpos_T	si_h_endpos;		// end position of the highlighting
    lpos_T	si_eoe_pos;		// end position of end pattern
    পূর্ণাঙ্ক		si_end_idx;		// group ID for end pattern or zero
    পূর্ণাঙ্ক		si_ends;		// if match ends before si_m_endpos
    পূর্ণাঙ্ক		si_attr;		// attributes in this state
    long	si_flags;		// HL_HAS_EOL flag in this state, and
					// HL_SKIP* for si_next_list
#ifdef FEAT_CONCEAL
    পূর্ণাঙ্ক		si_seqnr;		// sequence number
    পূর্ণাঙ্ক		si_cchar;		// substitution character for conceal
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
স্থির পূর্ণাঙ্ক next_seqnr = 1;		// value to use for si_seqnr
#endif

/*
 * Struct to reduce the number of arguments to get_syn_options(), it's used
 * very often.
 */
প্রকারনির্দেশ সংরচনা
{
    পূর্ণাঙ্ক		flags;		// flags for contained and transparent
    পূর্ণাঙ্ক		keyword;	// TRUE for ":syn keyword"
    পূর্ণাঙ্ক		*sync_idx;	// syntax item for "grouphere" argument, NULL
				// if not allowed
    ৱর্ণ	has_cont_list;	// TRUE if "cont_list" can be used
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
স্থির পূর্ণাঙ্ক next_match_col;		// column for start of next match
স্থির lpos_T next_match_m_endpos;	// position for end of next match
স্থির lpos_T next_match_h_startpos;	// pos. for highl. start of next match
স্থির lpos_T next_match_h_endpos;	// pos. for highl. end of next match
স্থির পূর্ণাঙ্ক next_match_idx;		// index of matched item
স্থির long next_match_flags;		// flags for next match
স্থির lpos_T next_match_eos_pos;	// end of start pattn (start region)
স্থির lpos_T next_match_eoe_pos;	// pos. for end of end pattern
স্থির পূর্ণাঙ্ক next_match_end_idx;		// ID of group for end pattn or zero
স্থির reg_extmatch_T *next_match_extmatch = রিক্ত;

/*
 * A state stack is an array of integers or stateitem_T, stored in a
 * garray_T.  A state stack is invalid if its itemsize entry is zero.
 */
#define INVALID_STATE(ssp)  ((ssp)->ga_itemsize == 0)
#define VALID_STATE(ssp)    ((ssp)->ga_itemsize != 0)

#define FOR_ALL_SYNSTATES(sb, sst) \
    বাবে ((sst) = (sb)->b_sst_first; (sst) != রিক্ত; (sst) = (sst)->sst_next)

/*
 * The current state (within the line) of the recognition engine.
 * When current_state.ga_itemsize is 0 the current state is invalid.
 */
স্থির win_T	*syn_win;		// current window for highlighting
স্থির buf_T	*syn_buf;		// current buffer for highlighting
স্থির synblock_T *syn_block;		// current buffer for highlighting
স্থির linenr_T current_lnum = 0;	// lnum of current state
স্থির colnr_T	current_col = 0;	// column of current state
স্থির পূর্ণাঙ্ক	current_state_stored = 0; // TRUE if stored current state
					  // after setting current_finished
স্থির পূর্ণাঙ্ক	current_finished = 0;	// current line has been finished
স্থির garray_T current_state		// current stack of state_items
		= {0, 0, 0, 0, রিক্ত};
স্থির short	*current_next_list = রিক্ত; // when non-zero, nextgroup list
স্থির পূর্ণাঙ্ক	current_next_flags = 0; // flags for current_next_list
স্থির পূর্ণাঙ্ক	current_সাৰি_id = 0;	// unique number for current line

#define CUR_STATE(idx)	((stateitem_T *)(current_state.ga_data))[idx]

স্থির শূন্য syn_sync(win_T *wp, linenr_T lnum, synstate_T *last_valid);
স্থির পূর্ণাঙ্ক syn_match_linecont(linenr_T lnum);
স্থির শূন্য syn_start_সাৰি(শূন্য);
স্থির শূন্য syn_update_ends(পূর্ণাঙ্ক startofline);
স্থির শূন্য syn_স্তূপ_alloc(শূন্য);
স্থির পূর্ণাঙ্ক syn_স্তূপ_cleanup(শূন্য);
স্থির শূন্য syn_স্তূপ_free_entry(synblock_T *block, synstate_T *p);
স্থির synstate_T *syn_স্তূপ_find_entry(linenr_T lnum);
স্থির synstate_T *store_current_state(শূন্য);
স্থির শূন্য load_current_state(synstate_T *from);
স্থির শূন্য invalidate_current_state(শূন্য);
স্থির পূর্ণাঙ্ক syn_স্তূপ_equal(synstate_T *sp);
স্থির শূন্য validate_current_state(শূন্য);
স্থির পূর্ণাঙ্ক syn_finish_সাৰি(পূর্ণাঙ্ক syncing);
স্থির পূর্ণাঙ্ক syn_current_attr(পূর্ণাঙ্ক syncing, পূর্ণাঙ্ক displaying, পূর্ণাঙ্ক *can_spell, পূর্ণাঙ্ক keep_state);
স্থির পূর্ণাঙ্ক did_match_already(পূর্ণাঙ্ক idx, garray_T *gap);
স্থির stateitem_T *ঠেলো_next_match(stateitem_T *cur_si);
স্থির শূন্য check_state_ends(শূন্য);
স্থির শূন্য update_si_attr(পূর্ণাঙ্ক idx);
স্থির শূন্য check_keepend(শূন্য);
স্থির শূন্য update_si_end(stateitem_T *sip, পূর্ণাঙ্ক startcol, পূর্ণাঙ্ক force);
স্থির short *copy_id_list(short *list);
স্থির পূর্ণাঙ্ক in_id_list(stateitem_T *item, short *cont_list, সংরচনা sp_syn *ssp, পূর্ণাঙ্ক flags);
স্থির পূর্ণাঙ্ক ঠেলো_current_state(পূর্ণাঙ্ক idx);
স্থির শূন্য বেৰ_কৰো_current_state(শূন্য);
#ifdef FEAT_PROFILE
স্থির শূন্য syn_clear_time(syn_time_T *tt);
স্থির শূন্য syntime_clear(শূন্য);
স্থির শূন্য syntime_report(শূন্য);
স্থির পূর্ণাঙ্ক syn_time_on = FALSE;
# define IF_SYN_TIME(p) (p)
#অন্যথা
# define IF_SYN_TIME(p) রিক্ত
প্রকারনির্দেশ পূর্ণাঙ্ক syn_time_T;
#endif

স্থির শূন্য syn_স্তূপ_apply_changes_block(synblock_T *block, buf_T *buf);
স্থির শূন্য find_endpos(পূর্ণাঙ্ক idx, lpos_T *startpos, lpos_T *m_endpos, lpos_T *hl_endpos, long *flagsp, lpos_T *end_endpos, পূর্ণাঙ্ক *end_idx, reg_extmatch_T *start_ext);

স্থির শূন্য limit_pos(lpos_T *pos, lpos_T *limit);
স্থির শূন্য limit_pos_zero(lpos_T *pos, lpos_T *limit);
স্থির শূন্য syn_add_end_off(lpos_T *result, regmmatch_T *regmatch, synpat_T *spp, পূর্ণাঙ্ক idx, পূর্ণাঙ্ক extra);
স্থির শূন্য syn_add_start_off(lpos_T *result, regmmatch_T *regmatch, synpat_T *spp, পূর্ণাঙ্ক idx, পূর্ণাঙ্ক extra);
স্থির char_u *syn_getcurline(শূন্য);
স্থির colnr_T syn_getcurline_len(শূন্য);
স্থির পূর্ণাঙ্ক syn_regexec(regmmatch_T *rmp, linenr_T lnum, colnr_T col, syn_time_T *st);
স্থির পূর্ণাঙ্ক check_keyword_id(char_u *সাৰি, পূর্ণাঙ্ক startcol, পূর্ণাঙ্ক *endcol, long *flags, short **next_list, stateitem_T *cur_si, পূর্ণাঙ্ক *ccharp);
স্থির শূন্য syn_remove_pattern(synblock_T *block, পূর্ণাঙ্ক idx);
স্থির শূন্য syn_clear_pattern(synblock_T *block, পূর্ণাঙ্ক i);
স্থির শূন্য syn_clear_cluster(synblock_T *block, পূর্ণাঙ্ক i);
স্থির শূন্য syn_clear_one(পূর্ণাঙ্ক id, পূর্ণাঙ্ক syncing);
স্থির শূন্য syn_cmd_onoff(exarg_T *eap, ৱর্ণ *name);
স্থির শূন্য syn_সাৰিগুলি_বাৰ্তা(শূন্য);
স্থির শূন্য syn_match_বাৰ্তা(শূন্য);
স্থির শূন্য syn_list_one(পূর্ণাঙ্ক id, পূর্ণাঙ্ক syncing, পূর্ণাঙ্ক link_only);
স্থির শূন্য syn_list_cluster(পূর্ণাঙ্ক id);
স্থির শূন্য put_id_list(char_u *name, short *list, পূর্ণাঙ্ক attr);
স্থির শূন্য put_pattern(ৱর্ণ *s, পূর্ণাঙ্ক c, synpat_T *spp, পূর্ণাঙ্ক attr);
স্থির পূর্ণাঙ্ক syn_list_keywords(পূর্ণাঙ্ক id, hashtab_T *ht, পূর্ণাঙ্ক did_header, পূর্ণাঙ্ক attr);
স্থির শূন্য syn_clear_keyword(পূর্ণাঙ্ক id, hashtab_T *ht);
স্থির শূন্য clear_keywtab(hashtab_T *ht);
স্থির পূর্ণাঙ্ক syn_scl_namen2id(char_u *linep, পূর্ণাঙ্ক len);
স্থির পূর্ণাঙ্ক syn_check_cluster(char_u *pp, পূর্ণাঙ্ক len);
স্থির পূর্ণাঙ্ক syn_add_cluster(char_u *name);
স্থির শূন্য init_syn_patterns(শূন্য);
স্থির char_u *get_syn_pattern(char_u *arg, synpat_T *ci);
স্থির পূর্ণাঙ্ক get_id_list(char_u **arg, পূর্ণাঙ্ক keylen, short **list, পূর্ণাঙ্ক skip);
স্থির শূন্য syn_combine_list(short **clstr1, short **clstr2, পূর্ণাঙ্ক list_op);
স্থির শূন্য idl_cache_clear(synblock_T *block);

/*
 * Start the syntax recognition for a line.  This function is normally called
 * from the screen updating, once for each displayed line.
 * The buffer is remembered in syn_buf, because get_syntax_attr() doesn't get
 * it.	Careful: curbuf and curwin are likely to point to another buffer and
 * window.
 */
    শূন্য
syntax_start(win_T *wp, linenr_T lnum)
{
    synstate_T	*p;
    synstate_T	*last_valid = রিক্ত;
    synstate_T	*last_min_valid = রিক্ত;
    synstate_T	*sp, *prev = রিক্ত;
    linenr_T	parsed_lnum;
    linenr_T	first_stored;
    পূর্ণাঙ্ক		dist;
    স্থির varnumber_T changedtick = 0;	// remember the last change ID

#ifdef FEAT_CONCEAL
    current_sub_char = NUL;
#endif

    /*
     * After switching buffers, invalidate current_state.
     * Also do this when a change was made, the current state may be invalid
     * then.
     */
    যদি (syn_block != wp->w_s
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
    syn_স্তূপ_alloc();
    যদি (syn_block->b_sst_array == রিক্ত)
	ঘূৰাই;		// out of memory
    syn_block->b_sst_lasttick = display_tick;

    /*
     * If the state of the end of the previous line is useful, store it.
     */
    যদি (VALID_STATE(&current_state)
	    && current_lnum < lnum
	    && current_lnum < syn_buf->b_ml.ml_সাৰি_গুনতি)
    {
	(শূন্য)syn_finish_সাৰি(FALSE);
	যদি (!current_state_stored)
	{
	    ++current_lnum;
	    (শূন্য)store_current_state();
	}

	/*
	 * If the current_lnum is now the same as "lnum", keep the current
	 * state (this happens very often!).  Otherwise invalidate
	 * current_state and figure it out below.
	 */
	যদি (current_lnum != lnum)
	    invalidate_current_state();
    }
    অন্যথা
	invalidate_current_state();

    /*
     * Try to synchronize from a saved state in b_sst_array[].
     * Only do this if lnum is not before and not to far beyond a saved state.
     */
    যদি (INVALID_STATE(&current_state) && syn_block->b_sst_array != রিক্ত)
    {
	// Find last valid saved state before start_lnum.
	FOR_ALL_SYNSTATES(syn_block, p)
	{
	    যদি (p->sst_lnum > lnum)
		ভঙ্গ;
	    যদি (p->sst_lnum <= lnum && p->sst_change_lnum == 0)
	    {
		last_valid = p;
		যদি (p->sst_lnum >= lnum - syn_block->b_syn_sync_minlines)
		    last_min_valid = p;
	    }
	}
	যদি (last_min_valid != রিক্ত)
	    load_current_state(last_min_valid);
    }

    /*
     * If "lnum" is before or far beyond a line with a saved state, need to
     * re-synchronize.
     */
    যদি (INVALID_STATE(&current_state))
    {
	syn_sync(wp, lnum, last_valid);
	যদি (current_lnum == 1)
	    // First line is always valid, no matter "minlines".
	    first_stored = 1;
	অন্যথা
	    // Need to parse "minlines" lines before state can be considered
	    // valid to store.
	    first_stored = current_lnum + syn_block->b_syn_sync_minlines;
    }
    অন্যথা
	first_stored = current_lnum;

    /*
     * Advance from the sync point or saved state until the current line.
     * Save some entries for syncing with later on.
     */
    যদি (syn_block->b_sst_len <= Rows)
	dist = 999999;
    অন্যথা
	dist = syn_buf->b_ml.ml_সাৰি_গুনতি / (syn_block->b_sst_len - Rows) + 1;
    যেতিয়ালৈ (current_lnum < lnum)
    {
	syn_start_সাৰি();
	(শূন্য)syn_finish_সাৰি(FALSE);
	++current_lnum;

	// If we parsed at least "minlines" lines or started at a valid
	// state, the current state is considered valid.
	যদি (current_lnum >= first_stored)
	{
	    // Check if the saved state entry is for the current line and is
	    // equal to the current state.  If so, then validate all saved
	    // states that depended on a change before the parsed line.
	    যদি (prev == রিক্ত)
		prev = syn_স্তূপ_find_entry(current_lnum - 1);
	    যদি (prev == রিক্ত)
		sp = syn_block->b_sst_first;
	    অন্যথা
		sp = prev;
	    যেতিয়ালৈ (sp != রিক্ত && sp->sst_lnum < current_lnum)
		sp = sp->sst_next;
	    যদি (sp != রিক্ত
		    && sp->sst_lnum == current_lnum
		    && syn_স্তূপ_equal(sp))
	    {
		parsed_lnum = current_lnum;
		prev = sp;
		যেতিয়ালৈ (sp != রিক্ত && sp->sst_change_lnum <= parsed_lnum)
		{
		    যদি (sp->sst_lnum <= lnum)
			// valid state before desired line, use this one
			prev = sp;
		    অন্যথা যদি (sp->sst_change_lnum == 0)
			// past saved states depending on change, break here.
			ভঙ্গ;
		    sp->sst_change_lnum = 0;
		    sp = sp->sst_next;
		}
		load_current_state(prev);
	    }
	    // Store the state at this line when it's the first one, the line
	    // where we start parsing, or some distance from the previously
	    // saved state.  But only when parsed at least 'minlines'.
	    অন্যথা যদি (prev == রিক্ত
			|| current_lnum == lnum
			|| current_lnum >= prev->sst_lnum + dist)
		prev = store_current_state();
	}

	// This can take a long time: break when CTRL-C pressed.  The current
	// state will be wrong then.
	সাৰি_breakcheck();
	যদি (got_int)
	{
	    current_lnum = lnum;
	    ভঙ্গ;
	}
    }

    syn_start_সাৰি();
}

/*
 * We cannot simply discard growarrays full of state_items or buf_states; we
 * have to manually release their extmatch pointers first.
 */
    স্থির শূন্য
clear_syn_state(synstate_T *p)
{
    পূর্ণাঙ্ক		i;
    garray_T	*gap;

    যদি (p->sst_stacksize > SST_FIX_STATES)
    {
	gap = &(p->sst_union.sst_ga);
	বাবে (i = 0; i < gap->ga_len; i++)
	    unref_extmatch(SYN_STATE_P(gap)[i].bs_extmatch);
	ga_clear(gap);
    }
    অন্যথা
    {
	বাবে (i = 0; i < p->sst_stacksize; i++)
	    unref_extmatch(p->sst_union.sst_স্তূপ[i].bs_extmatch);
    }
}

/*
 * Cleanup the current_state stack.
 */
    স্থির শূন্য
clear_current_state(শূন্য)
{
    পূর্ণাঙ্ক		i;
    stateitem_T	*sip;

    sip = (stateitem_T *)(current_state.ga_data);
    বাবে (i = 0; i < current_state.ga_len; i++)
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
    স্থির শূন্য
syn_sync(
    win_T	*wp,
    linenr_T	start_lnum,
    synstate_T	*last_valid)
{
    buf_T	*curbuf_save;
    win_T	*curwin_save;
    pos_T	cursor_save;
    পূর্ণাঙ্ক		idx;
    linenr_T	lnum;
    linenr_T	end_lnum;
    linenr_T	break_lnum;
    পূর্ণাঙ্ক		had_sync_point;
    stateitem_T	*cur_si;
    synpat_T	*spp;
    পূর্ণাঙ্ক		found_flags = 0;
    পূর্ণাঙ্ক		found_match_idx = 0;
    linenr_T	found_current_lnum = 0;
    পূর্ণাঙ্ক		found_current_col = 0;
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
    যদি (syn_block->b_syn_sync_minlines > start_lnum)
	start_lnum = 1;
    অন্যথা
    {
	যদি (syn_block->b_syn_sync_minlines == 1)
	    lnum = 1;
	অন্যথা যদি (syn_block->b_syn_sync_minlines < 10)
	    lnum = syn_block->b_syn_sync_minlines * 2;
	অন্যথা
	    lnum = syn_block->b_syn_sync_minlines * 3 / 2;
	যদি (syn_block->b_syn_sync_maxlines != 0
				     && lnum > syn_block->b_syn_sync_maxlines)
	    lnum = syn_block->b_syn_sync_maxlines;
	যদি (lnum >= start_lnum)
	    start_lnum = 1;
	অন্যথা
	    start_lnum -= lnum;
    }
    current_lnum = start_lnum;

    /*
     * 1. Search backwards for the end of a C-style comment.
     */
    যদি (syn_block->b_syn_sync_flags & SF_CCOMMENT)
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
	বাবে ( ; start_lnum > 1; --start_lnum)
	{
	    char_u	*l = ml_get(start_lnum - 1);

	    যদি (*l == NUL || *(l + ml_get_len(start_lnum - 1) - 1) != '\\')
		ভঙ্গ;
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
	যদি (find_start_comment((পূর্ণাঙ্ক)syn_block->b_syn_sync_maxlines) != রিক্ত)
	{
	    বাবে (idx = syn_block->b_syn_patterns.ga_len; --idx >= 0; )
		যদি (SYN_বস্তুসমূহ(syn_block)[idx].sp_syn.id
						   == syn_block->b_syn_sync_id
			&& SYN_বস্তুসমূহ(syn_block)[idx].sp_type == SPTYPE_START)
		{
		    validate_current_state();
		    যদি (ঠেলো_current_state(idx) == OK)
			update_si_attr(current_state.ga_len - 1);
		    ভঙ্গ;
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
    অন্যথা যদি (syn_block->b_syn_sync_flags & SF_MATCH)
    {
	যদি (syn_block->b_syn_sync_maxlines != 0
			       && start_lnum > syn_block->b_syn_sync_maxlines)
	    break_lnum = start_lnum - syn_block->b_syn_sync_maxlines;
	অন্যথা
	    break_lnum = 0;

	found_m_endpos.lnum = 0;
	found_m_endpos.col = 0;
	end_lnum = start_lnum;
	lnum = start_lnum;
	যেতিয়ালৈ (--lnum > break_lnum)
	{
	    // This can take a long time: break when CTRL-C pressed.
	    সাৰি_breakcheck();
	    যদি (got_int)
	    {
		invalidate_current_state();
		current_lnum = start_lnum;
		ভঙ্গ;
	    }

	    // Check if we have run into a valid saved state stack now.
	    যদি (last_valid != রিক্ত && lnum == last_valid->sst_lnum)
	    {
		load_current_state(last_valid);
		ভঙ্গ;
	    }

	    /*
	     * Check if the previous line has the line-continuation pattern.
	     */
	    যদি (lnum > 1 && syn_match_linecont(lnum - 1))
		অনুৱর্ত;

	    /*
	     * Start with nothing on the state stack
	     */
	    validate_current_state();

	    বাবে (current_lnum = lnum; current_lnum < end_lnum; ++current_lnum)
	    {
		syn_start_সাৰি();
		বাবে (;;)
		{
		    had_sync_point = syn_finish_সাৰি(TRUE);
		    /*
		     * When a sync point has been found, remember where, and
		     * continue to look for another one, further on in the line.
		     */
		    যদি (had_sync_point && current_state.ga_len)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			যদি (cur_si->si_m_endpos.lnum > start_lnum)
			{
			    // ignore match that goes to after where started
			    current_lnum = end_lnum;
			    ভঙ্গ;
			}
			যদি (cur_si->si_idx < 0)
			{
			    // Cannot happen?
			    found_flags = 0;
			    found_match_idx = KEYWORD_IDX;
			}
			অন্যথা
			{
			    spp = &(SYN_বস্তুসমূহ(syn_block)[cur_si->si_idx]);
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
			যদি (found_m_endpos.lnum > current_lnum)
			{
			    current_lnum = found_m_endpos.lnum;
			    current_col = found_m_endpos.col;
			    যদি (current_lnum >= end_lnum)
				ভঙ্গ;
			}
			অন্যথা যদি (found_m_endpos.col > current_col)
			    current_col = found_m_endpos.col;
			অন্যথা
			    ++current_col;

			// syn_current_attr() will have skipped the check for
			// an item that ends here, need to do that now.  Be
			// careful not to go past the NUL.
			prev_current_col = current_col;
			যদি (syn_getcurline()[current_col] != NUL)
			    ++current_col;
			check_state_ends();
			current_col = prev_current_col;
		    }
		    অন্যথা
			ভঙ্গ;
		}
	    }

	    /*
	     * If a sync point was encountered, break here.
	     */
	    যদি (found_flags)
	    {
		/*
		 * Put the item that was specified by the sync point on the
		 * state stack.  If there was no item specified, make the
		 * state stack empty.
		 */
		clear_current_state();
		যদি (found_match_idx >= 0
			&& ঠেলো_current_state(found_match_idx) == OK)
		    update_si_attr(current_state.ga_len - 1);

		/*
		 * When using "grouphere", continue from the sync point
		 * match, until the end of the line.  Parsing starts at
		 * the next line.
		 * For "groupthere" the parsing starts at start_lnum.
		 */
		যদি (found_flags & HL_SYNC_HERE)
		{
		    current_lnum = found_m_endpos.lnum;
		    current_col = found_m_endpos.col;
		    যদি (current_state.ga_len)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			cur_si->si_h_startpos.lnum = found_current_lnum;
			cur_si->si_h_startpos.col = found_current_col;
			update_si_end(cur_si, (পূর্ণাঙ্ক)current_col, TRUE);
			check_keepend();
		    }
		    (শূন্য)syn_finish_সাৰি(FALSE);
		    ++current_lnum;
		}
		অন্যথা
		    current_lnum = start_lnum;

		ভঙ্গ;
	    }

	    end_lnum = lnum;
	    invalidate_current_state();
	}

	// Ran into start of the file or exceeded maximum number of lines
	যদি (lnum <= break_lnum)
	{
	    invalidate_current_state();
	    current_lnum = break_lnum + 1;
	}
    }

    validate_current_state();
}

    স্থির শূন্য
save_chartab(char_u *chartab)
{
    যদি (syn_block->b_syn_isk == empty_option)
	ঘূৰাই;

    mch_memmove(chartab, syn_buf->b_chartab, (size_t)32);
    mch_memmove(syn_buf->b_chartab, syn_win->w_s->b_syn_chartab, (size_t)32);
}

    স্থির শূন্য
restore_chartab(char_u *chartab)
{
    যদি (syn_win->w_s->b_syn_isk != empty_option)
	mch_memmove(syn_buf->b_chartab, chartab, (size_t)32);
}

/*
 * Return TRUE if the line-continuation pattern matches in line "lnum".
 */
    স্থির পূর্ণাঙ্ক
syn_match_linecont(linenr_T lnum)
{
    regmmatch_T regmatch;
    পূর্ণাঙ্ক r;
    char_u	buf_chartab[32];  // chartab array for syn iskyeyword

    যদি (syn_block->b_syn_linecont_prog == রিক্ত)
	ঘূৰাই FALSE;

    // use syntax iskeyword option
    save_chartab(buf_chartab);
    regmatch.rmm_ic = syn_block->b_syn_linecont_ic;
    regmatch.regprog = syn_block->b_syn_linecont_prog;
    r = syn_regexec(&regmatch, lnum, (colnr_T)0,
	    IF_SYN_TIME(&syn_block->b_syn_linecont_time));
    syn_block->b_syn_linecont_prog = regmatch.regprog;
    restore_chartab(buf_chartab);
    ঘূৰাই r;
}

/*
 * Prepare the current state for the start of a line.
 */
    স্থির শূন্য
syn_start_সাৰি(শূন্য)
{
    current_finished = FALSE;
    current_col = 0;

    /*
     * Need to update the end of a start/skip/end that continues from the
     * previous line and regions that have "keepend".
     */
    যদি (current_state.ga_len > 0)
    {
	syn_update_ends(TRUE);
	check_state_ends();
    }

    next_match_idx = -1;
    ++current_সাৰি_id;
#ifdef FEAT_CONCEAL
    next_seqnr = 1;
#endif
}

/*
 * Check for items in the stack that need their end updated.
 * When "startofline" is TRUE the last item is always updated.
 * When "startofline" is FALSE the item with "keepend" is forcefully updated.
 */
    স্থির শূন্য
syn_update_ends(পূর্ণাঙ্ক startofline)
{
    stateitem_T	*cur_si;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		seen_keepend;

    যদি (startofline)
    {
	// Check for a match carried over from a previous line with a
	// contained region.  The match ends as soon as the region ends.
	বাবে (i = 0; i < current_state.ga_len; ++i)
	{
	    cur_si = &CUR_STATE(i);
	    যদি (cur_si->si_idx >= 0
		    && (SYN_বস্তুসমূহ(syn_block)[cur_si->si_idx]).sp_type
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
    যদি (keepend_level >= 0)
	বাবে ( ; i > keepend_level; --i)
	    যদি (CUR_STATE(i).si_flags & HL_EXTEND)
		ভঙ্গ;

    seen_keepend = FALSE;
    বাবে ( ; i < current_state.ga_len; ++i)
    {
	cur_si = &CUR_STATE(i);
	যদি ((cur_si->si_flags & HL_KEEPEND)
			    || (seen_keepend && !startofline)
			    || (i == current_state.ga_len - 1 && startofline))
	{
	    cur_si->si_h_startpos.col = 0;	// start highl. in col 0
	    cur_si->si_h_startpos.lnum = current_lnum;

	    যদি (!(cur_si->si_flags & HL_MATCHCONT))
		update_si_end(cur_si, (পূর্ণাঙ্ক)current_col, !startofline);

	    যদি (!startofline && (cur_si->si_flags & HL_KEEPEND))
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

    স্থির শূন্য
syn_স্তূপ_free_block(synblock_T *block)
{
    synstate_T	*p;

    // Syntax definitions may have changed: drop the in_id_list() cache.
    idl_cache_clear(block);

    যদি (block->b_sst_array == রিক্ত)
	ঘূৰাই;

    FOR_ALL_SYNSTATES(block, p)
	clear_syn_state(p);
    VIM_CLEAR(block->b_sst_array);
    block->b_sst_first = রিক্ত;
    block->b_sst_অনুসন্ধান = রিক্ত;
    block->b_sst_len = 0;
}
/*
 * Free b_sst_array[] for buffer "buf".
 * Used when syntax items changed to force resyncing everywhere.
 */
    শূন্য
syn_স্তূপ_free_all(synblock_T *block)
{
#ifdef FEAT_FOLDING
    win_T	*wp;
#endif

    syn_স্তূপ_free_block(block);

#ifdef FEAT_FOLDING
    // When using "syntax" fold method, must update all folds.
    FOR_ALL_WINDOWS(wp)
    {
	যদি (wp->w_s == block && foldmethodIsSyntax(wp))
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
    স্থির শূন্য
syn_স্তূপ_alloc(শূন্য)
{
    long	len;
    synstate_T	*to, *from;
    synstate_T	*sstp;

    len = syn_buf->b_ml.ml_সাৰি_গুনতি / SST_DIST + Rows * 2;
    যদি (len < SST_MIN_ENTRIES)
	len = SST_MIN_ENTRIES;
    অন্যথা যদি (len > SST_MAX_ENTRIES)
	len = SST_MAX_ENTRIES;
    যদি (syn_block->b_sst_len > len * 2 || syn_block->b_sst_len < len)
    {
	// Allocate 50% too much, to avoid reallocating too often.
	len = syn_buf->b_ml.ml_সাৰি_গুনতি;
	len = (len + len / 2) / SST_DIST + Rows * 2;
	যদি (len < SST_MIN_ENTRIES)
	    len = SST_MIN_ENTRIES;
	অন্যথা যদি (len > SST_MAX_ENTRIES)
	    len = SST_MAX_ENTRIES;

	যদি (syn_block->b_sst_array != রিক্ত)
	{
	    // When shrinking the array, cleanup the existing stack.
	    // Make sure that all valid entries fit in the new array.
	    যেতিয়ালৈ (syn_block->b_sst_len - syn_block->b_sst_freecount + 2 > len
		    && syn_স্তূপ_cleanup())
		;
	    যদি (len < syn_block->b_sst_len - syn_block->b_sst_freecount + 2)
		len = syn_block->b_sst_len - syn_block->b_sst_freecount + 2;
	}

	sstp = ALLOC_CLEAR_MULT(synstate_T, len);
	যদি (sstp == রিক্ত)	// out of memory!
	    ঘূৰাই;

	to = sstp - 1;
	যদি (syn_block->b_sst_array != রিক্ত)
	{
	    // Move the states from the old array to the new one.
	    বাবে (from = syn_block->b_sst_first; from != রিক্ত;
							from = from->sst_next)
	    {
		++to;
		*to = *from;
		to->sst_next = to + 1;
	    }
	}
	যদি (to != sstp - 1)
	{
	    to->sst_next = রিক্ত;
	    syn_block->b_sst_first = sstp;
	    syn_block->b_sst_freecount = len - (পূর্ণাঙ্ক)(to - sstp) - 1;
	}
	অন্যথা
	{
	    syn_block->b_sst_first = রিক্ত;
	    syn_block->b_sst_freecount = len;
	}

	// Create the list of free entries.
	syn_block->b_sst_firstfree = to + 1;
	যেতিয়ালৈ (++to < sstp + len)
	    to->sst_next = to + 1;
	(sstp + len - 1)->sst_next = রিক্ত;

	vim_free(syn_block->b_sst_array);
	syn_block->b_sst_array = sstp;
	syn_block->b_sst_len = len;
	// The entries were moved to a new array, drop the stale pointer.
	syn_block->b_sst_অনুসন্ধান = রিক্ত;
    }
}

/*
 * Check for changes in a buffer to affect stored syntax states.  Uses the
 * b_mod_* fields.
 * Called from update_screen(), before screen is being updated, once for each
 * displayed buffer.
 */
    শূন্য
syn_স্তূপ_apply_changes(buf_T *buf)
{
    win_T	*wp;

    syn_স্তূপ_apply_changes_block(&buf->b_s, buf);

    FOR_ALL_WINDOWS(wp)
    {
	যদি ((wp->w_buffer == buf) && (wp->w_s != &buf->b_s))
	    syn_স্তূপ_apply_changes_block(wp->w_s, buf);
    }
}

    স্থির শূন্য
syn_স্তূপ_apply_changes_block(synblock_T *block, buf_T *buf)
{
    synstate_T	*p, *prev, *np;
    linenr_T	n;

    prev = রিক্ত;
    বাবে (p = block->b_sst_first; p != রিক্ত; )
    {
	যদি (p->sst_lnum + block->b_syn_sync_linebreaks > buf->b_mod_শীৰ্ষ)
	{
	    n = p->sst_lnum + buf->b_mod_xlines;
	    যদি (n <= buf->b_mod_bot)
	    {
		// this state is inside the changed area, remove it
		np = p->sst_next;
		যদি (prev == রিক্ত)
		    block->b_sst_first = np;
		অন্যথা
		    prev->sst_next = np;
		syn_স্তূপ_free_entry(block, p);
		p = np;
		অনুৱর্ত;
	    }
	    // This state is below the changed area.  Remember the line
	    // that needs to be parsed before this entry can be made valid
	    // again.
	    যদি (p->sst_change_lnum != 0 && p->sst_change_lnum > buf->b_mod_শীৰ্ষ)
	    {
		যদি (p->sst_change_lnum + buf->b_mod_xlines > buf->b_mod_শীৰ্ষ)
		    p->sst_change_lnum += buf->b_mod_xlines;
		অন্যথা
		    p->sst_change_lnum = buf->b_mod_শীৰ্ষ;
	    }
	    যদি (p->sst_change_lnum == 0
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
    স্থির পূর্ণাঙ্ক
syn_স্তূপ_cleanup(শূন্য)
{
    synstate_T	*p, *prev;
    disptick_T	tick;
    পূর্ণাঙ্ক		above;
    পূর্ণাঙ্ক		dist;
    পূর্ণাঙ্ক		retval = FALSE;

    যদি (syn_block->b_sst_first == রিক্ত)
	ঘূৰাই retval;

    // Compute normal distance between non-displayed entries.
    যদি (syn_block->b_sst_len <= Rows)
	dist = 999999;
    অন্যথা
	dist = syn_buf->b_ml.ml_সাৰি_গুনতি / (syn_block->b_sst_len - Rows) + 1;

    /*
     * Go through the list to find the "tick" for the oldest entry that can
     * be removed.  Set "above" when the "tick" for the oldest entry is above
     * "b_sst_lasttick" (the display tick wraps around).
     */
    tick = syn_block->b_sst_lasttick;
    above = FALSE;
    prev = syn_block->b_sst_first;
    বাবে (p = prev->sst_next; p != রিক্ত; prev = p, p = p->sst_next)
    {
	যদি (prev->sst_lnum + dist > p->sst_lnum)
	{
	    যদি (p->sst_tick > syn_block->b_sst_lasttick)
	    {
		যদি (!above || p->sst_tick < tick)
		    tick = p->sst_tick;
		above = TRUE;
	    }
	    অন্যথা যদি (!above && p->sst_tick < tick)
		tick = p->sst_tick;
	}
    }

    /*
     * Go through the list to make the entries for the oldest tick at an
     * interval of several lines.
     */
    prev = syn_block->b_sst_first;
    বাবে (p = prev->sst_next; p != রিক্ত; prev = p, p = p->sst_next)
    {
	যদি (p->sst_tick == tick && prev->sst_lnum + dist > p->sst_lnum)
	{
	    // Move this entry from used list to free list
	    prev->sst_next = p->sst_next;
	    syn_স্তূপ_free_entry(syn_block, p);
	    p = prev;
	    retval = TRUE;
	}
    }
    ঘূৰাই retval;
}

/*
 * Free the allocated memory for a syn_state item.
 * Move the entry into the free list.
 */
    স্থির শূন্য
syn_স্তূপ_free_entry(synblock_T *block, synstate_T *p)
{
    যদি (block->b_sst_অনুসন্ধান == p)
	block->b_sst_অনুসন্ধান = রিক্ত;
    clear_syn_state(p);
    p->sst_next = block->b_sst_firstfree;
    block->b_sst_firstfree = p;
    ++block->b_sst_freecount;
}

/*
 * Find an entry in the list of state stacks at or before "lnum".
 * Returns NULL when there is no entry or the first entry is after "lnum".
 */
    স্থির synstate_T *
syn_স্তূপ_find_entry(linenr_T lnum)
{
    synstate_T	*p, *prev;

    prev = রিক্ত;
    p = syn_block->b_sst_first;

    // The list is sorted by line number and lookups while parsing advance
    // monotonically, so resume from the last returned entry instead of
    // rescanning from the start whenever it is at or before "lnum".
    যদি (syn_block->b_sst_অনুসন্ধান != রিক্ত
				 && syn_block->b_sst_অনুসন্ধান->sst_lnum <= lnum)
	p = syn_block->b_sst_অনুসন্ধান;

    বাবে ( ; p != রিক্ত; prev = p, p = p->sst_next)
    {
	যদি (p->sst_lnum == lnum)
	{
	    syn_block->b_sst_অনুসন্ধান = p;
	    ঘূৰাই p;
	}
	যদি (p->sst_lnum > lnum)
	    ভঙ্গ;
    }
    যদি (prev != রিক্ত)
	syn_block->b_sst_অনুসন্ধান = prev;
    ঘূৰাই prev;
}

/*
 * Try saving the current state in b_sst_array[].
 * The current state must be valid for the start of the current_lnum line!
 */
    স্থির synstate_T *
store_current_state(শূন্য)
{
    পূর্ণাঙ্ক		i;
    synstate_T	*p;
    bufstate_T	*bp;
    stateitem_T	*cur_si;
    synstate_T	*sp = syn_স্তূপ_find_entry(current_lnum);

    /*
     * If the current state contains a start or end pattern that continues
     * from the previous line, we can't use it.  Don't store it then.
     */
    বাবে (i = current_state.ga_len - 1; i >= 0; --i)
    {
	cur_si = &CUR_STATE(i);
	যদি (cur_si->si_h_startpos.lnum >= current_lnum
		|| cur_si->si_m_endpos.lnum >= current_lnum
		|| cur_si->si_h_endpos.lnum >= current_lnum
		|| (cur_si->si_end_idx
		    && cur_si->si_eoe_pos.lnum >= current_lnum))
	    ভঙ্গ;
    }
    যদি (i >= 0)
    {
	যদি (sp != রিক্ত)
	{
	    // find "sp" in the list and remove it
	    যদি (syn_block->b_sst_first == sp)
		// it's the first entry
		syn_block->b_sst_first = sp->sst_next;
	    অন্যথা
	    {
		// find the entry just before this one to adjust sst_next
		FOR_ALL_SYNSTATES(syn_block, p)
		    যদি (p->sst_next == sp)
			ভঙ্গ;
		যদি (p != রিক্ত)	// just in case
		    p->sst_next = sp->sst_next;
	    }
	    syn_স্তূপ_free_entry(syn_block, sp);
	    sp = রিক্ত;
	}
    }
    অন্যথা যদি (sp == রিক্ত || sp->sst_lnum != current_lnum)
    {
	/*
	 * Add a new entry
	 */
	// If no free items, cleanup the array first.
	যদি (syn_block->b_sst_freecount == 0)
	{
	    (শূন্য)syn_স্তূপ_cleanup();
	    // "sp" may have been moved to the freelist now
	    sp = syn_স্তূপ_find_entry(current_lnum);
	}
	// Still no free items?  Must be a strange problem...
	যদি (syn_block->b_sst_freecount == 0)
	    sp = রিক্ত;
	অন্যথা
	{
	    // Take the first item from the free list and put it in the used
	    // list, after *sp
	    p = syn_block->b_sst_firstfree;
	    syn_block->b_sst_firstfree = p->sst_next;
	    --syn_block->b_sst_freecount;
	    যদি (sp == রিক্ত)
	    {
		// Insert in front of the list
		p->sst_next = syn_block->b_sst_first;
		syn_block->b_sst_first = p;
	    }
	    অন্যথা
	    {
		// insert in list after *sp
		p->sst_next = sp->sst_next;
		sp->sst_next = p;
	    }
	    sp = p;
	    sp->sst_stacksize = 0;
	    sp->sst_lnum = current_lnum;
	    // Resume the next forward lookup from the entry just stored.
	    syn_block->b_sst_অনুসন্ধান = sp;
	}
    }
    যদি (sp != রিক্ত)
    {
	// When overwriting an existing state stack, clear it first
	clear_syn_state(sp);
	sp->sst_stacksize = current_state.ga_len;
	যদি (current_state.ga_len > SST_FIX_STATES)
	{
	    // Need to clear it, might be something remaining from when the
	    // length was less than SST_FIX_STATES.
	    ga_init2(&sp->sst_union.sst_ga, পরিমাণ(bufstate_T), 1);
	    যদি (ga_grow(&sp->sst_union.sst_ga, current_state.ga_len) == FAIL)
		sp->sst_stacksize = 0;
	    অন্যথা
		sp->sst_union.sst_ga.ga_len = current_state.ga_len;
	    bp = SYN_STATE_P(&(sp->sst_union.sst_ga));
	}
	অন্যথা
	    bp = sp->sst_union.sst_স্তূপ;
	বাবে (i = 0; i < sp->sst_stacksize; ++i)
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
    ঘূৰাই sp;
}

/*
 * Copy a state stack from "from" in b_sst_array[] to current_state;
 */
    স্থির শূন্য
load_current_state(synstate_T *from)
{
    পূর্ণাঙ্ক		i;
    bufstate_T	*bp;

    clear_current_state();
    validate_current_state();
    keepend_level = -1;
    যদি (from->sst_stacksize
	    && ga_grow(&current_state, from->sst_stacksize) == OK)
    {
	যদি (from->sst_stacksize > SST_FIX_STATES)
	    bp = SYN_STATE_P(&(from->sst_union.sst_ga));
	অন্যথা
	    bp = from->sst_union.sst_স্তূপ;
	বাবে (i = 0; i < from->sst_stacksize; ++i)
	{
	    CUR_STATE(i).si_idx = bp[i].bs_idx;
	    CUR_STATE(i).si_flags = bp[i].bs_flags;
#ifdef FEAT_CONCEAL
	    CUR_STATE(i).si_seqnr = bp[i].bs_seqnr;
	    CUR_STATE(i).si_cchar = bp[i].bs_cchar;
#endif
	    CUR_STATE(i).si_extmatch = ref_extmatch(bp[i].bs_extmatch);
	    যদি (keepend_level < 0 && (CUR_STATE(i).si_flags & HL_KEEPEND))
		keepend_level = i;
	    CUR_STATE(i).si_ends = FALSE;
	    CUR_STATE(i).si_m_lnum = 0;
	    যদি (CUR_STATE(i).si_idx >= 0)
		CUR_STATE(i).si_next_list =
		     (SYN_বস্তুসমূহ(syn_block)[CUR_STATE(i).si_idx]).sp_next_list;
	    অন্যথা
		CUR_STATE(i).si_next_list = রিক্ত;
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
    স্থির পূর্ণাঙ্ক
syn_স্তূপ_equal(synstate_T *sp)
{
    পূর্ণাঙ্ক		i, j;
    bufstate_T	*bp;
    reg_extmatch_T	*six, *bsx;

    // First a quick check if the stacks have the same size end nextlist.
    যদি (sp->sst_stacksize != current_state.ga_len
	    || sp->sst_next_list != current_next_list)
	ঘূৰাই FALSE;

    // Need to compare all states on both stacks.
    যদি (sp->sst_stacksize > SST_FIX_STATES)
	bp = SYN_STATE_P(&(sp->sst_union.sst_ga));
    অন্যথা
	bp = sp->sst_union.sst_স্তূপ;

    বাবে (i = current_state.ga_len; --i >= 0; )
    {
	// If the item has another index the state is different.
	যদি (bp[i].bs_idx != CUR_STATE(i).si_idx)
	    ভঙ্গ;
	যদি (bp[i].bs_extmatch == CUR_STATE(i).si_extmatch)
	    অনুৱর্ত;
	// When the extmatch pointers are different, the strings in them can
	// still be the same.  Check if the extmatch references are equal.
	bsx = bp[i].bs_extmatch;
	six = CUR_STATE(i).si_extmatch;
	// If one of the extmatch pointers is NULL the states are different.
	যদি (bsx == রিক্ত || six == রিক্ত)
	    ভঙ্গ;
	বাবে (j = 0; j < NSUBEXP; ++j)
	{
	    // Check each referenced match string. They must all be equal.
	    যদি (bsx->matches[j] != six->matches[j])
	    {
		// If the pointer is different it can still be the same text.
		// Compare the strings, ignore case when the start item has the
		// sp_ic flag set.
		যদি (bsx->matches[j] == রিক্ত || six->matches[j] == রিক্ত)
		    ভঙ্গ;
		যদি ((SYN_বস্তুসমূহ(syn_block)[CUR_STATE(i).si_idx]).sp_ic
			? MB_STRICMP(bsx->matches[j], six->matches[j]) != 0
			: STRCMP(bsx->matches[j], six->matches[j]) != 0)
		    ভঙ্গ;
	    }
	}
	যদি (j != NSUBEXP)
	    ভঙ্গ;
    }
    ঘূৰাই i < 0 ? TRUE : FALSE;
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
    শূন্য
syntax_end_parsing(win_T *wp, linenr_T lnum)
{
    synstate_T	*sp;

    যদি (syn_block != wp->w_s)
	ঘূৰাই;  // not the right window
    sp = syn_স্তূপ_find_entry(lnum);
    যদি (sp != রিক্ত && sp->sst_lnum < lnum)
	sp = sp->sst_next;

    যদি (sp != রিক্ত && sp->sst_change_lnum != 0)
	sp->sst_change_lnum = lnum;
}

/*
 * End of handling of the state stack.
 ****************************************/

    স্থির শূন্য
invalidate_current_state(শূন্য)
{
    clear_current_state();
    current_state.ga_itemsize = 0;	// mark current_state invalid
    current_next_list = রিক্ত;
    keepend_level = -1;
}

    স্থির শূন্য
validate_current_state(শূন্য)
{
    current_state.ga_itemsize = পরিমাণ(stateitem_T);
    current_state.ga_growsize = 3;
}

/*
 * Return TRUE if the syntax at start of lnum changed since last time.
 * This will only be called just after get_syntax_attr() for the previous
 * line, to check if the next line needs to be redrawn too.
 */
    পূর্ণাঙ্ক
syntax_check_changed(linenr_T lnum)
{
    পূর্ণাঙ্ক		retval = TRUE;
    synstate_T	*sp;

    /*
     * Check the state stack when:
     * - lnum is just below the previously syntaxed line.
     * - lnum is not before the lines with saved states.
     * - lnum is not past the lines with saved states.
     * - lnum is at or before the last changed line.
     */
    যদি (VALID_STATE(&current_state) && lnum == current_lnum + 1)
    {
	sp = syn_স্তূপ_find_entry(lnum);
	যদি (sp != রিক্ত && sp->sst_lnum == lnum)
	{
	    /*
	     * finish the previous line (needed when not all of the line was
	     * drawn)
	     */
	    (শূন্য)syn_finish_সাৰি(FALSE);

	    /*
	     * Compare the current state with the previously saved state of
	     * the line.
	     */
	    যদি (syn_স্তূপ_equal(sp))
		retval = FALSE;

	    /*
	     * Store the current state in b_sst_array[] for later use.
	     */
	    ++current_lnum;
	    (শূন্য)store_current_state();
	}
    }

    ঘূৰাই retval;
}

/*
 * Finish the current line.
 * This doesn't return any attributes, it only gets the state at the end of
 * the line.  It can start anywhere in the line, as long as the current state
 * is valid.
 */
    স্থির পূর্ণাঙ্ক
syn_finish_সাৰি(
    পূর্ণাঙ্ক	    syncing)		// called for syncing
{
    stateitem_T	*cur_si;
    colnr_T	prev_current_col;

    যেতিয়ালৈ (!current_finished)
    {
	(শূন্য)syn_current_attr(syncing, FALSE, রিক্ত, FALSE);
	/*
	 * When syncing, and found some item, need to check the item.
	 */
	যদি (syncing && current_state.ga_len)
	{
	    /*
	     * Check for match with sync item.
	     */
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	    যদি (cur_si->si_idx >= 0
		    && (SYN_বস্তুসমূহ(syn_block)[cur_si->si_idx].sp_flags
					  & (HL_SYNC_HERE|HL_SYNC_THERE)))
		ঘূৰাই TRUE;

	    // syn_current_attr() will have skipped the check for an item
	    // that ends here, need to do that now.  Be careful not to go
	    // past the NUL.
	    prev_current_col = current_col;
	    যদি (syn_getcurline()[current_col] != NUL)
		++current_col;
	    check_state_ends();
	    current_col = prev_current_col;
	}
	++current_col;
    }
    ঘূৰাই FALSE;
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
    পূর্ণাঙ্ক
get_syntax_attr(
    colnr_T	col,
    পূর্ণাঙ্ক		*can_spell,
    পূর্ণাঙ্ক		keep_state)	// keep state of char at "col"
{
    পূর্ণাঙ্ক	    attr = 0;

    যদি (can_spell != রিক্ত)
	// Default: Only do spelling when there is no @Spell cluster or when
	// ":syn spell toplevel" was used.
	*can_spell = syn_block->b_syn_spell == SYNSPL_DEFAULT
		    ? (syn_block->b_spell_cluster_id == 0)
		    : (syn_block->b_syn_spell == SYNSPL_শীৰ্ষ);

    // check for out of memory situation
    যদি (syn_block->b_sst_array == রিক্ত)
	ঘূৰাই 0;

    // After 'synmaxcol' the attribute is always zero.
    যদি (syn_buf->b_p_smc > 0 && col >= (colnr_T)syn_buf->b_p_smc)
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
	ঘূৰাই 0;
    }

    // Make sure current_state is valid
    যদি (INVALID_STATE(&current_state))
	validate_current_state();

    /*
     * Skip from the current column to "col", get the attributes for "col".
     */
    যেতিয়ালৈ (current_col <= col)
    {
	attr = syn_current_attr(FALSE, TRUE, can_spell,
				     current_col == col ? keep_state : FALSE);
	++current_col;
    }

    ঘূৰাই attr;
}

/*
 * Get syntax attributes for current_lnum, current_col.
 */
    স্থির পূর্ণাঙ্ক
syn_current_attr(
    পূর্ণাঙ্ক		syncing,		// When 1: called for syncing
    পূর্ণাঙ্ক		displaying,		// result will be displayed
    পূর্ণাঙ্ক		*can_spell,		// return: do spell checking
    পূর্ণাঙ্ক		keep_state)		// keep syntax stack afterwards
{
    পূর্ণাঙ্ক		syn_id;
    lpos_T	endpos;		// was: char_u *endp;
    lpos_T	hl_startpos;	// was: int hl_startcol;
    lpos_T	hl_endpos;
    lpos_T	eos_pos;	// end-of-start match (start region)
    lpos_T	eoe_pos;	// end-of-end pattern
    পূর্ণাঙ্ক		end_idx;	// group ID for end pattern
    পূর্ণাঙ্ক		idx;
    synpat_T	*spp;
    stateitem_T	*cur_si, *sip = রিক্ত;
    পূর্ণাঙ্ক		startcol;
    পূর্ণাঙ্ক		endcol;
    long	flags;
    পূর্ণাঙ্ক		cchar;
    short	*next_list;
    পূর্ণাঙ্ক		found_match;		    // found usable match
    স্থির পূর্ণাঙ্ক	try_next_column = FALSE;    // must try in next col
    পূর্ণাঙ্ক		do_keywords;
    regmmatch_T	regmatch;
    lpos_T	pos;
    পূর্ণাঙ্ক		lc_col;
    reg_extmatch_T *cur_extmatch = রিক্ত;
    char_u	buf_chartab[32];  // chartab array for syn iskyeyword
    char_u	*সাৰি;		// current line.  NOTE: becomes invalid after
				// looking for a pattern match!

    // variables for zero-width matches that have a "nextgroup" argument
    পূর্ণাঙ্ক		keep_next_list;
    পূর্ণাঙ্ক		zero_width_next_list = FALSE;
    garray_T	zero_width_next_ga;

    /*
     * No character, no attributes!  Past end of line?
     * Do try matching with an empty line (could be the start of a region).
     */
    সাৰি = syn_getcurline();
    যদি (সাৰি[current_col] == NUL && current_col != 0)
    {
	/*
	 * If we found a match after the last column, use it.
	 */
	যদি (next_match_idx >= 0 && next_match_col >= (পূর্ণাঙ্ক)current_col
						  && next_match_col != MAXCOL)
	    (শূন্য)ঠেলো_next_match(রিক্ত);

	current_finished = TRUE;
	current_state_stored = FALSE;
	ঘূৰাই 0;
    }

    // if the current or next character is NUL, we will finish the line now
    যদি (সাৰি[current_col] == NUL || সাৰি[current_col + 1] == NUL)
    {
	current_finished = TRUE;
	current_state_stored = FALSE;
    }

    /*
     * When in the previous column there was a match but it could not be used
     * (empty match or already matched in this column) need to try again in
     * the next column.
     */
    যদি (try_next_column)
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
    ga_init2(&zero_width_next_ga, পরিমাণ(পূর্ণাঙ্ক), 10);

    // use syntax iskeyword option
    save_chartab(buf_chartab);

    /*
     * Repeat matching keywords and patterns, to find contained items at the
     * same column.  This stops when there are no extra matches at the current
     * column.
     */
    কৰা
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
	যদি (current_state.ga_len)
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	অন্যথা
	    cur_si = রিক্ত;

	যদি (syn_block->b_syn_containedin || cur_si == রিক্ত
					      || cur_si->si_cont_list != রিক্ত)
	{
	    /*
	     * 2. Check for keywords, if on a keyword char after a non-keyword
	     *	  char.  Don't do this when syncing.
	     */
	    যদি (do_keywords)
	    {
	      সাৰি = syn_getcurline();
	      যদি (vim_iswordp_buf(সাৰি + current_col, syn_buf)
		      && (current_col == 0
			  || !vim_iswordp_buf(সাৰি + current_col - 1
			      - (has_mbyte
				  ? (*mb_head_off)(সাৰি, সাৰি + current_col - 1)
				  : 0) , syn_buf)))
	      {
		syn_id = check_keyword_id(সাৰি, (পূর্ণাঙ্ক)current_col,
					 &endcol, &flags, &next_list, cur_si,
					 &cchar);
		যদি (syn_id != 0)
		{
		    যদি (ঠেলো_current_state(KEYWORD_IDX) == OK)
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
			যদি (current_state.ga_len > 1)
			    cur_si->si_flags |=
				  CUR_STATE(current_state.ga_len - 2).si_flags
								 & HL_CONCEAL;
#endif
			cur_si->si_id = syn_id;
			cur_si->si_trans_id = syn_id;
			যদি (flags & HL_TRANSP)
			{
			    যদি (current_state.ga_len < 2)
			    {
				cur_si->si_attr = 0;
				cur_si->si_trans_id = 0;
			    }
			    অন্যথা
			    {
				cur_si->si_attr = CUR_STATE(
					current_state.ga_len - 2).si_attr;
				cur_si->si_trans_id = CUR_STATE(
					current_state.ga_len - 2).si_trans_id;
			    }
			}
			অন্যথা
			    cur_si->si_attr = syn_id2attr(syn_id);
			cur_si->si_cont_list = রিক্ত;
			cur_si->si_next_list = next_list;
			check_keepend();
		    }
		    অন্যথা
			vim_free(next_list);
		}
	      }
	    }

	    /*
	     * 3. Check for patterns (only if no keyword found).
	     */
	    যদি (syn_id == 0 && syn_block->b_syn_patterns.ga_len)
	    {
		/*
		 * If we didn't check for a match yet, or we are past it, check
		 * for any match with a pattern.
		 */
		যদি (next_match_idx < 0 || next_match_col < (পূর্ণাঙ্ক)current_col)
		{
		    /*
		     * Check all relevant patterns for a match at this
		     * position.  This is complicated, because matching with a
		     * pattern takes quite a bit of time, thus we want to
		     * avoid doing it when it's not needed.
		     */
		    next_match_idx = 0;		// no match in this line yet
		    next_match_col = MAXCOL;
		    বাবে (idx = syn_block->b_syn_patterns.ga_len; --idx >= 0; )
		    {
			spp = &(SYN_বস্তুসমূহ(syn_block)[idx]);
			যদি (	   spp->sp_syncing == syncing
				&& (displaying || !(spp->sp_flags & HL_DISPLAY))
				&& (spp->sp_type == SPTYPE_MATCH
				    || spp->sp_type == SPTYPE_START)
				&& (current_next_list != রিক্ত
				    ? in_id_list(রিক্ত, current_next_list,
							      &spp->sp_syn, 0)
				    : (cur_si == রিক্ত
					? !(spp->sp_flags & HL_CONTAINED)
					: in_id_list(cur_si,
					    cur_si->si_cont_list, &spp->sp_syn,
					    spp->sp_flags))))
			{
			    পূর্ণাঙ্ক r;

			    // If we already tried matching in this line, and
			    // there isn't a match before next_match_col, skip
			    // this item.
			    যদি (spp->sp_সাৰি_id == current_সাৰি_id
				    && spp->sp_startcol >= next_match_col)
				অনুৱর্ত;
			    spp->sp_সাৰি_id = current_সাৰি_id;

			    lc_col = current_col - spp->sp_offsets[SPO_LC_OFF];
			    যদি (lc_col < 0)
				lc_col = 0;

			    regmatch.rmm_ic = spp->sp_ic;
			    regmatch.regprog = spp->sp_prog;
			    r = syn_regexec(&regmatch,
					     current_lnum,
					     (colnr_T)lc_col,
					     IF_SYN_TIME(&spp->sp_time));
			    spp->sp_prog = regmatch.regprog;
			    যদি (!r)
			    {
				// no match in this line, try another one
				spp->sp_startcol = MAXCOL;
				অনুৱর্ত;
			    }

			    /*
			     * Compute the first column of the match.
			     */
			    syn_add_start_off(&pos, &regmatch,
							 spp, SPO_MS_OFF, -1);
			    যদি (pos.lnum > current_lnum)
			    {
				// must have used end of match in a next line,
				// we can't handle that
				spp->sp_startcol = MAXCOL;
				অনুৱর্ত;
			    }
			    startcol = pos.col;

			    // remember the next column where this pattern
			    // matches in the current line
			    spp->sp_startcol = startcol;

			    /*
			     * If a previously found match starts at a lower
			     * column number, don't use this one.
			     */
			    যদি (startcol >= next_match_col)
				অনুৱর্ত;

			    /*
			     * If we matched this pattern at this position
			     * before, skip it.  Must retry in the next
			     * column, because it may match from there.
			     */
			    যদি (did_match_already(idx, &zero_width_next_ga))
			    {
				try_next_column = TRUE;
				অনুৱর্ত;
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
			    re_extmatch_out = রিক্ত;

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
			    যদি (spp->sp_type == SPTYPE_START
					      && (spp->sp_flags & HL_ONELINE))
			    {
				lpos_T	startpos;

				startpos = endpos;
				find_endpos(idx, &startpos, &endpos, &hl_endpos,
				    &flags, &eoe_pos, &end_idx, cur_extmatch);
				যদি (endpos.lnum == 0)
				    অনুৱর্ত;	    // not found
			    }

			    /*
			     * For a "match" the size must be > 0 after the
			     * end offset needs has been added.  Except when
			     * syncing.
			     */
			    অন্যথা যদি (spp->sp_type == SPTYPE_MATCH)
			    {
				syn_add_end_off(&hl_endpos, &regmatch, spp,
							       SPO_HE_OFF, 0);
				syn_add_end_off(&endpos, &regmatch, spp,
							       SPO_ME_OFF, 0);
				যদি (endpos.lnum == current_lnum
				      && (পূর্ণাঙ্ক)endpos.col + syncing < startcol)
				{
				    /*
				     * If an empty string is matched, may need
				     * to try matching again at next column.
				     */
				    যদি (regmatch.startpos[0].col
						    == regmatch.endpos[0].col)
					try_next_column = TRUE;
				    অনুৱর্ত;
				}
			    }

			    /*
			     * keep the best match so far in next_match_*
			     */
			    // Highlighting must start after startpos and end
			    // before endpos.
			    যদি (hl_startpos.lnum == current_lnum
					   && (পূর্ণাঙ্ক)hl_startpos.col < startcol)
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
			    cur_extmatch = রিক্ত;
			}
		    }
		}

		/*
		 * If we found a match at the current column, use it.
		 */
		যদি (next_match_idx >= 0 && next_match_col == (পূর্ণাঙ্ক)current_col)
		{
		    synpat_T	*lspp;

		    // When a zero-width item matched which has a nextgroup,
		    // don't push the item but set nextgroup.
		    lspp = &(SYN_বস্তুসমূহ(syn_block)[next_match_idx]);
		    যদি (next_match_m_endpos.lnum == current_lnum
			    && next_match_m_endpos.col == current_col
			    && lspp->sp_next_list != রিক্ত)
		    {
			current_next_list = lspp->sp_next_list;
			current_next_flags = lspp->sp_flags;
			keep_next_list = TRUE;
			zero_width_next_list = TRUE;

			// Add the index to a list, so that we can check
			// later that we don't match it again (and cause an
			// endless loop).
			যদি (ga_grow(&zero_width_next_ga, 1) == OK)
			{
			    ((পূর্ণাঙ্ক *)(zero_width_next_ga.ga_data))
				[zero_width_next_ga.ga_len++] = next_match_idx;
			}
			next_match_idx = -1;
		    }
		    অন্যথা
			cur_si = ঠেলো_next_match(cur_si);
		    found_match = TRUE;
		}
	    }
	}

	/*
	 * Handle searching for nextgroup match.
	 */
	যদি (current_next_list != রিক্ত && !keep_next_list)
	{
	    /*
	     * If a nextgroup was not found, continue looking for one if:
	     * - this is an empty line and the "skipempty" option was given
	     * - we are on white space and the "skipwhite" option was given
	     */
	    যদি (!found_match)
	    {
		সাৰি = syn_getcurline();
		যদি (((current_next_flags & HL_SKIPWHITE)
			    && VIM_ISWHITE(সাৰি[current_col]))
			|| ((current_next_flags & HL_SKIPEMPTY)
			    && *সাৰি == NUL))
		    ভঙ্গ;
	    }

	    /*
	     * If a nextgroup was found: Use it, and continue looking for
	     * contained matches.
	     * If a nextgroup was not found: Continue looking for a normal
	     * match.
	     * When did set current_next_list for a zero-width item and no
	     * match was found don't loop (would get stuck).
	     */
	    current_next_list = রিক্ত;
	    next_match_idx = -1;
	    যদি (!zero_width_next_list)
		found_match = TRUE;
	}

    } যেতিয়ালৈ (found_match);

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
    যদি (cur_si != রিক্ত)
    {
#ifndef FEAT_EVAL
	পূর্ণাঙ্ক	current_trans_id = 0;
#endif
	বাবে (idx = current_state.ga_len - 1; idx >= 0; --idx)
	{
	    sip = &CUR_STATE(idx);
	    যদি ((current_lnum > sip->si_h_startpos.lnum
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
		ভঙ্গ;
	    }
	}

	যদি (can_spell != রিক্ত)
	{
	    সংরচনা sp_syn   sps;

	    /*
	     * set "can_spell" to TRUE if spell checking is supposed to be
	     * done in the current item.
	     */
	    যদি (syn_block->b_spell_cluster_id == 0)
	    {
		// There is no @Spell cluster: Do spelling for items without
		// @NoSpell cluster.
		যদি (syn_block->b_nospell_cluster_id == 0
						     || current_trans_id == 0)
		    *can_spell = (syn_block->b_syn_spell != SYNSPL_NOTOP);
		অন্যথা
		{
		    sps.inc_tag = 0;
		    sps.id = syn_block->b_nospell_cluster_id;
		    sps.cont_in_list = রিক্ত;
		    *can_spell = !in_id_list(sip, sip->si_cont_list, &sps, 0);
		}
	    }
	    অন্যথা
	    {
		// The @Spell cluster is defined: Do spelling in items with
		// the @Spell cluster.  But not when @NoSpell is also there.
		// At the toplevel only spell check when ":syn spell toplevel"
		// was used.
		যদি (current_trans_id == 0)
		    *can_spell = (syn_block->b_syn_spell == SYNSPL_শীৰ্ষ);
		অন্যথা
		{
		    sps.inc_tag = 0;
		    sps.id = syn_block->b_spell_cluster_id;
		    sps.cont_in_list = রিক্ত;
		    *can_spell = in_id_list(sip, sip->si_cont_list, &sps, 0);

		    যদি (syn_block->b_nospell_cluster_id != 0)
		    {
			sps.id = syn_block->b_nospell_cluster_id;
			যদি (in_id_list(sip, sip->si_cont_list, &sps, 0))
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
	যদি (!syncing && !keep_state)
	{
	    check_state_ends();
	    যদি (current_state.ga_len > 0
				      && syn_getcurline()[current_col] != NUL)
	    {
		++current_col;
		check_state_ends();
		--current_col;
	    }
	}
    }
    অন্যথা যদি (can_spell != রিক্ত)
	// Default: Only do spelling when there is no @Spell cluster or when
	// ":syn spell toplevel" was used.
	*can_spell = syn_block->b_syn_spell == SYNSPL_DEFAULT
		    ? (syn_block->b_spell_cluster_id == 0)
		    : (syn_block->b_syn_spell == SYNSPL_শীৰ্ষ);

    // nextgroup ends at end of line, unless "skipnl" or "skipempty" present
    যদি (current_next_list != রিক্ত
	    && (সাৰি = syn_getcurline())[current_col] != NUL
	    && সাৰি[current_col + 1] == NUL
	    && !(current_next_flags & (HL_SKIPNL | HL_SKIPEMPTY)))
	current_next_list = রিক্ত;

    যদি (zero_width_next_ga.ga_len > 0)
	ga_clear(&zero_width_next_ga);

    // No longer need external matches.  But keep next_match_extmatch.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = রিক্ত;
    unref_extmatch(cur_extmatch);

    ঘূৰাই current_attr;
}


/*
 * Check if we already matched pattern "idx" at the current column.
 */
    স্থির পূর্ণাঙ্ক
did_match_already(পূর্ণাঙ্ক idx, garray_T *gap)
{
    পূর্ণাঙ্ক		i;

    বাবে (i = current_state.ga_len; --i >= 0; )
	যদি (CUR_STATE(i).si_m_startcol == (পূর্ণাঙ্ক)current_col
		&& CUR_STATE(i).si_m_lnum == (পূর্ণাঙ্ক)current_lnum
		&& CUR_STATE(i).si_idx == idx)
	    ঘূৰাই TRUE;

    // Zero-width matches with a nextgroup argument are not put on the syntax
    // stack, and can only be matched once anyway.
    বাবে (i = gap->ga_len; --i >= 0; )
	যদি (((পূর্ণাঙ্ক *)(gap->ga_data))[i] == idx)
	    ঘূৰাই TRUE;

    ঘূৰাই FALSE;
}

/*
 * Push the next match onto the stack.
 */
    স্থির stateitem_T *
ঠেলো_next_match(stateitem_T *cur_si)
{
    synpat_T	*spp;
#ifdef FEAT_CONCEAL
    পূর্ণাঙ্ক		 save_flags;
#endif

    spp = &(SYN_বস্তুসমূহ(syn_block)[next_match_idx]);

    /*
     * Push the item in current_state stack;
     */
    যদি (ঠেলো_current_state(next_match_idx) == OK)
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
	যদি (current_state.ga_len > 1)
	    cur_si->si_flags |=
		    CUR_STATE(current_state.ga_len - 2).si_flags & HL_CONCEAL;
#endif
	cur_si->si_next_list = spp->sp_next_list;
	cur_si->si_extmatch = ref_extmatch(next_match_extmatch);
	যদি (spp->sp_type == SPTYPE_START && !(spp->sp_flags & HL_ONELINE))
	{
	    // Try to find the end pattern in the current line
	    update_si_end(cur_si, (পূর্ণাঙ্ক)(next_match_m_endpos.col), TRUE);
	    check_keepend();
	}
	অন্যথা
	{
	    cur_si->si_m_endpos = next_match_m_endpos;
	    cur_si->si_h_endpos = next_match_h_endpos;
	    cur_si->si_ends = TRUE;
	    cur_si->si_flags |= next_match_flags;
	    cur_si->si_eoe_pos = next_match_eoe_pos;
	    cur_si->si_end_idx = next_match_end_idx;
	}
	যদি (keepend_level < 0 && (cur_si->si_flags & HL_KEEPEND))
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
	যদি (	   spp->sp_type == SPTYPE_START
		&& spp->sp_syn_match_id != 0
		&& ঠেলো_current_state(next_match_idx) == OK)
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
	    যদি (cur_si->si_flags & HL_CONCEALENDS)
		cur_si->si_flags |= HL_CONCEAL;
#endif
	    cur_si->si_next_list = রিক্ত;
	    check_keepend();
	    update_si_attr(current_state.ga_len - 1);
	}
    }

    next_match_idx = -1;	// try other match next time

    ঘূৰাই cur_si;
}

/*
 * Check for end of current state (and the states before it).
 */
    স্থির শূন্য
check_state_ends(শূন্য)
{
    stateitem_T	*cur_si;
    পূর্ণাঙ্ক		had_extend;

    cur_si = &CUR_STATE(current_state.ga_len - 1);
    বাবে (;;)
    {
	যদি (cur_si->si_ends
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
	    যদি (cur_si->si_end_idx
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
		যদি (cur_si->si_flags & HL_CONCEALENDS)
		    cur_si->si_flags |= HL_CONCEAL;
#endif
		update_si_attr(current_state.ga_len - 1);

		// nextgroup= should not match in the end pattern
		current_next_list = রিক্ত;

		// what matches next may be different now, clear it
		next_match_idx = 0;
		next_match_col = MAXCOL;
		ভঙ্গ;
	    }

	    // handle next_list, unless at end of line and no "skipnl" or
	    // "skipempty"
	    current_next_list = cur_si->si_next_list;
	    current_next_flags = cur_si->si_flags;
	    যদি (!(current_next_flags & (HL_SKIPNL | HL_SKIPEMPTY))
		    && syn_getcurline()[current_col] == NUL)
		current_next_list = রিক্ত;

	    // When the ended item has "extend", another item with
	    // "keepend" now needs to check for its end.
	    had_extend = (cur_si->si_flags & HL_EXTEND);

	    বেৰ_কৰো_current_state();

	    যদি (current_state.ga_len == 0)
		ভঙ্গ;

	    যদি (had_extend && keepend_level >= 0)
	    {
		syn_update_ends(FALSE);
		যদি (current_state.ga_len == 0)
		    ভঙ্গ;
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
	    যদি (cur_si->si_idx >= 0
		    && SYN_বস্তুসমূহ(syn_block)[cur_si->si_idx].sp_type
								== SPTYPE_START
		    && !(cur_si->si_flags & (HL_MATCH | HL_KEEPEND)))
	    {
		update_si_end(cur_si, (পূর্ণাঙ্ক)current_col, TRUE);
		check_keepend();
		যদি ((current_next_flags & HL_HAS_EOL)
			&& keepend_level < 0
			&& syn_getcurline()[current_col] == NUL)
		    ভঙ্গ;
	    }
	}
	অন্যথা
	    ভঙ্গ;
    }
}

/*
 * Update an entry in the current_state stack for a match or region.  This
 * fills in si_attr, si_next_list and si_cont_list.
 */
    স্থির শূন্য
update_si_attr(পূর্ণাঙ্ক idx)
{
    stateitem_T	*sip = &CUR_STATE(idx);
    synpat_T	*spp;

    // This should not happen...
    যদি (sip->si_idx < 0)
	ঘূৰাই;

    spp = &(SYN_বস্তুসমূহ(syn_block)[sip->si_idx]);
    যদি (sip->si_flags & HL_MATCH)
	sip->si_id = spp->sp_syn_match_id;
    অন্যথা
	sip->si_id = spp->sp_syn.id;
    sip->si_attr = syn_id2attr(sip->si_id);
    sip->si_trans_id = sip->si_id;
    যদি (sip->si_flags & HL_MATCH)
	sip->si_cont_list = রিক্ত;
    অন্যথা
	sip->si_cont_list = spp->sp_cont_list;

    /*
     * For transparent items, take attr from outer item.
     * Also take cont_list, if there is none.
     * Don't do this for the matchgroup of a start or end pattern.
     */
    যদি ((spp->sp_flags & HL_TRANSP) && !(sip->si_flags & HL_MATCH))
    {
	যদি (idx == 0)
	{
	    sip->si_attr = 0;
	    sip->si_trans_id = 0;
	    যদি (sip->si_cont_list == রিক্ত)
		sip->si_cont_list = ID_LIST_ALL;
	}
	অন্যথা
	{
	    sip->si_attr = CUR_STATE(idx - 1).si_attr;
	    sip->si_trans_id = CUR_STATE(idx - 1).si_trans_id;
	    যদি (sip->si_cont_list == রিক্ত)
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
    স্থির শূন্য
check_keepend(শূন্য)
{
    পূর্ণাঙ্ক		i;
    lpos_T	maxpos;
    lpos_T	maxpos_h;
    stateitem_T	*sip;

    /*
     * This check can consume a lot of time; only do it from the level where
     * there really is a keepend.
     */
    যদি (keepend_level < 0)
	ঘূৰাই;

    /*
     * Find the last index of an "extend" item.  "keepend" items before that
     * won't do anything.  If there is no "extend" item "i" will be
     * "keepend_level" and all "keepend" items will work normally.
     */
    বাবে (i = current_state.ga_len - 1; i > keepend_level; --i)
	যদি (CUR_STATE(i).si_flags & HL_EXTEND)
	    ভঙ্গ;

    maxpos.lnum = 0;
    maxpos.col = 0;
    maxpos_h.lnum = 0;
    maxpos_h.col = 0;
    বাবে ( ; i < current_state.ga_len; ++i)
    {
	sip = &CUR_STATE(i);
	যদি (maxpos.lnum != 0)
	{
	    limit_pos_zero(&sip->si_m_endpos, &maxpos);
	    limit_pos_zero(&sip->si_h_endpos, &maxpos_h);
	    limit_pos_zero(&sip->si_eoe_pos, &maxpos);
	    sip->si_ends = TRUE;
	}
	যদি (sip->si_ends && (sip->si_flags & HL_KEEPEND))
	{
	    যদি (maxpos.lnum == 0
		    || maxpos.lnum > sip->si_m_endpos.lnum
		    || (maxpos.lnum == sip->si_m_endpos.lnum
			&& maxpos.col > sip->si_m_endpos.col))
		maxpos = sip->si_m_endpos;
	    যদি (maxpos_h.lnum == 0
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
    স্থির শূন্য
update_si_end(
    stateitem_T	*sip,
    পূর্ণাঙ্ক		startcol,   // where to start searching for the end
    পূর্ণাঙ্ক		force)	    // when TRUE overrule a previous end
{
    lpos_T	startpos;
    lpos_T	endpos;
    lpos_T	hl_endpos;
    lpos_T	end_endpos;
    পূর্ণাঙ্ক		end_idx;

    // return quickly for a keyword
    যদি (sip->si_idx < 0)
	ঘূৰাই;

    // Don't update when it's already done.  Can be a match of an end pattern
    // that started in a previous line.  Watch out: can also be a "keepend"
    // from a containing item.
    যদি (!force && sip->si_m_endpos.lnum >= current_lnum)
	ঘূৰাই;

    /*
     * We need to find the end of the region.  It may continue in the next
     * line.
     */
    end_idx = 0;
    startpos.lnum = current_lnum;
    startpos.col = startcol;
    find_endpos(sip->si_idx, &startpos, &endpos, &hl_endpos,
		   &(sip->si_flags), &end_endpos, &end_idx, sip->si_extmatch);

    যদি (endpos.lnum == 0)
    {
	// No end pattern matched.
	যদি (SYN_বস্তুসমূহ(syn_block)[sip->si_idx].sp_flags & HL_ONELINE)
	{
	    // a "oneline" never continues in the next line
	    sip->si_ends = TRUE;
	    sip->si_m_endpos.lnum = current_lnum;
	    sip->si_m_endpos.col = syn_getcurline_len();
	}
	অন্যথা
	{
	    // continues in the next line
	    sip->si_ends = FALSE;
	    sip->si_m_endpos.lnum = 0;
	}
	sip->si_h_endpos = sip->si_m_endpos;
    }
    অন্যথা
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
    স্থির পূর্ণাঙ্ক
ঠেলো_current_state(পূর্ণাঙ্ক idx)
{
    যদি (ga_grow(&current_state, 1) == FAIL)
	ঘূৰাই FAIL;
    CLEAR_POINTER(&CUR_STATE(current_state.ga_len));
    CUR_STATE(current_state.ga_len).si_idx = idx;
    ++current_state.ga_len;
    ঘূৰাই OK;
}

/*
 * Remove a state from the current_state stack.
 */
    স্থির শূন্য
বেৰ_কৰো_current_state(শূন্য)
{
    যদি (current_state.ga_len)
    {
	unref_extmatch(CUR_STATE(current_state.ga_len - 1).si_extmatch);
	--current_state.ga_len;
    }
    // after the end of a pattern, try matching a keyword or pattern
    next_match_idx = -1;

    // if first state with "keepend" is popped, reset keepend_level
    যদি (keepend_level >= current_state.ga_len)
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
    স্থির শূন্য
find_endpos(
    পূর্ণাঙ্ক		idx,		// index of the pattern
    lpos_T	*startpos,	// where to start looking for an END match
    lpos_T	*m_endpos,	// return: end of match
    lpos_T	*hl_endpos,	// return: end of highlighting
    long	*flagsp,	// return: flags of matching END
    lpos_T	*end_endpos,	// return: end of end pattern match
    পূর্ণাঙ্ক		*end_idx,	// return: group ID for end pat. match, or 0
    reg_extmatch_T *start_ext)	// submatches from the start pattern
{
    colnr_T	matchcol;
    synpat_T	*spp, *spp_skip;
    পূর্ণাঙ্ক		start_idx;
    পূর্ণাঙ্ক		best_idx;
    regmmatch_T	regmatch;
    regmmatch_T	best_regmatch;	    // startpos/endpos of best match
    lpos_T	pos;
    পূর্ণাঙ্ক		had_match = FALSE;
    char_u	buf_chartab[32];  // chartab array for syn option iskyeyword

    // just in case we are invoked for a keyword
    যদি (idx < 0)
	ঘূৰাই;

    /*
     * Check for being called with a START pattern.
     * Can happen with a match that continues to the next line, because it
     * contained a region.
     */
    spp = &(SYN_বস্তুসমূহ(syn_block)[idx]);
    যদি (spp->sp_type != SPTYPE_START)
    {
	*hl_endpos = *startpos;
	ঘূৰাই;
    }

    /*
     * Find the SKIP or first END pattern after the last START pattern.
     */
    বাবে (;;)
    {
	spp = &(SYN_বস্তুসমূহ(syn_block)[idx]);
	যদি (spp->sp_type != SPTYPE_START)
	    ভঙ্গ;
	++idx;
    }

    /*
     *	Lookup the SKIP pattern (if present)
     */
    যদি (spp->sp_type == SPTYPE_SKIP)
    {
	spp_skip = spp;
	++idx;
    }
    অন্যথা
	spp_skip = রিক্ত;

    // Setup external matches for syn_regexec().
    unref_extmatch(re_extmatch_in);
    re_extmatch_in = ref_extmatch(start_ext);

    matchcol = startpos->col;	// start looking for a match at sstart
    start_idx = idx;		// remember the first END pattern.
    best_regmatch.startpos[0].col = 0;		// avoid compiler warning

    // use syntax iskeyword option
    save_chartab(buf_chartab);

    বাবে (;;)
    {
	/*
	 * Find end pattern that matches first after "matchcol".
	 */
	best_idx = -1;
	বাবে (idx = start_idx; idx < syn_block->b_syn_patterns.ga_len; ++idx)
	{
	    পূর্ণাঙ্ক lc_col = matchcol;
	    পূর্ণাঙ্ক r;

	    spp = &(SYN_বস্তুসমূহ(syn_block)[idx]);
	    যদি (spp->sp_type != SPTYPE_END)	// past last END pattern
		ভঙ্গ;
	    lc_col -= spp->sp_offsets[SPO_LC_OFF];
	    যদি (lc_col < 0)
		lc_col = 0;

	    regmatch.rmm_ic = spp->sp_ic;
	    regmatch.regprog = spp->sp_prog;
	    r = syn_regexec(&regmatch, startpos->lnum, lc_col,
						  IF_SYN_TIME(&spp->sp_time));
	    spp->sp_prog = regmatch.regprog;
	    যদি (r)
	    {
		যদি (best_idx == -1 || regmatch.startpos[0].col
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
	যদি (best_idx == -1)
	    ভঙ্গ;

	/*
	 * If the skip pattern matches before the end pattern,
	 * continue searching after the skip pattern.
	 */
	যদি (spp_skip != রিক্ত)
	{
	    পূর্ণাঙ্ক lc_col = matchcol - spp_skip->sp_offsets[SPO_LC_OFF];
	    পূর্ণাঙ্ক r;

	    যদি (lc_col < 0)
		lc_col = 0;
	    regmatch.rmm_ic = spp_skip->sp_ic;
	    regmatch.regprog = spp_skip->sp_prog;
	    r = syn_regexec(&regmatch, startpos->lnum, lc_col,
					      IF_SYN_TIME(&spp_skip->sp_time));
	    spp_skip->sp_prog = regmatch.regprog;
	    যদি (r && regmatch.startpos[0].col
					     <= best_regmatch.startpos[0].col)
	    {
		পূর্ণাঙ্ক সাৰি_len;

		// Add offset to skip pattern match
		syn_add_end_off(&pos, &regmatch, spp_skip, SPO_ME_OFF, 1);

		// If the skip pattern goes on to the next line, there is no
		// match with an end pattern in this line.
		যদি (pos.lnum > startpos->lnum)
		    ভঙ্গ;

		সাৰি_len = ml_get_buf_len(syn_buf, startpos->lnum);

		// take care of an empty match or negative offset
		যদি (pos.col <= matchcol)
		    ++matchcol;
		অন্যথা যদি (pos.col <= regmatch.endpos[0].col)
		    matchcol = pos.col;
		অন্যথা
		    // Be careful not to jump over the NUL at the end-of-line
		    বাবে (matchcol = regmatch.endpos[0].col;
			    matchcol < সাৰি_len && matchcol < pos.col;
								   ++matchcol)
			;

		// if the skip pattern includes end-of-line, break here
		যদি (matchcol >= সাৰি_len)
		    ভঙ্গ;

		অনুৱর্ত;	    // start with first end pattern again
	    }
	}

	/*
	 * Match from start pattern to end pattern.
	 * Correct for match and highlight offset of end pattern.
	 */
	spp = &(SYN_বস্তুসমূহ(syn_block)[best_idx]);
	syn_add_end_off(m_endpos, &best_regmatch, spp, SPO_ME_OFF, 1);
	// can't end before the start
	যদি (m_endpos->lnum == startpos->lnum && m_endpos->col < startpos->col)
	    m_endpos->col = startpos->col;

	syn_add_end_off(end_endpos, &best_regmatch, spp, SPO_HE_OFF, 1);
	// can't end before the start
	যদি (end_endpos->lnum == startpos->lnum
					   && end_endpos->col < startpos->col)
	    end_endpos->col = startpos->col;
	// can't end after the match
	limit_pos(end_endpos, m_endpos);

	/*
	 * If the end group is highlighted differently, adjust the pointers.
	 */
	যদি (spp->sp_syn_match_id != spp->sp_syn.id && spp->sp_syn_match_id != 0)
	{
	    *end_idx = best_idx;
	    যদি (spp->sp_off_flags & (1 << (SPO_RE_OFF + SPO_গুনতি)))
	    {
		hl_endpos->lnum = best_regmatch.endpos[0].lnum;
		hl_endpos->col = best_regmatch.endpos[0].col;
	    }
	    অন্যথা
	    {
		hl_endpos->lnum = best_regmatch.startpos[0].lnum;
		hl_endpos->col = best_regmatch.startpos[0].col;
	    }
	    hl_endpos->col += spp->sp_offsets[SPO_RE_OFF];

	    // can't end before the start
	    যদি (hl_endpos->lnum == startpos->lnum
					    && hl_endpos->col < startpos->col)
		hl_endpos->col = startpos->col;
	    limit_pos(hl_endpos, m_endpos);

	    // now the match ends where the highlighting ends, it is turned
	    // into the matchgroup for the end
	    *m_endpos = *hl_endpos;
	}
	অন্যথা
	{
	    *end_idx = 0;
	    *hl_endpos = *end_endpos;
	}

	*flagsp = spp->sp_flags;

	had_match = TRUE;
	ভঙ্গ;
    }

    // no match for an END pattern in this line
    যদি (!had_match)
	m_endpos->lnum = 0;

    restore_chartab(buf_chartab);

    // Remove external matches.
    unref_extmatch(re_extmatch_in);
    re_extmatch_in = রিক্ত;
}

/*
 * Limit "pos" not to be after "limit".
 */
    স্থির শূন্য
limit_pos(lpos_T *pos, lpos_T *limit)
{
    যদি (pos->lnum > limit->lnum)
	*pos = *limit;
    অন্যথা যদি (pos->lnum == limit->lnum && pos->col > limit->col)
	pos->col = limit->col;
}

/*
 * Limit "pos" not to be after "limit", unless pos->lnum is zero.
 */
    স্থির শূন্য
limit_pos_zero(
    lpos_T	*pos,
    lpos_T	*limit)
{
    যদি (pos->lnum == 0)
	*pos = *limit;
    অন্যথা
	limit_pos(pos, limit);
}

/*
 * Add offset to matched text for end of match or highlight.
 */
    স্থির শূন্য
syn_add_end_off(
    lpos_T	*result,	// returned position
    regmmatch_T	*regmatch,	// start/end of match
    synpat_T	*spp,		// matched pattern
    পূর্ণাঙ্ক		idx,		// index of offset
    পূর্ণাঙ্ক		extra)		// extra chars for offset to start
{
    পূর্ণাঙ্ক		col;
    পূর্ণাঙ্ক		off;
    char_u	*base;
    char_u	*p;

    যদি (spp->sp_off_flags & (1 << idx))
    {
	result->lnum = regmatch->startpos[0].lnum;
	col = regmatch->startpos[0].col;
	off = spp->sp_offsets[idx] + extra;
    }
    অন্যথা
    {
	result->lnum = regmatch->endpos[0].lnum;
	col = regmatch->endpos[0].col;
	off = spp->sp_offsets[idx];
    }
    // Don't go past the end of the line.  Matters for "rs=e+2" when there
    // is a matchgroup. Watch out for match with last NL in the buffer.
    যদি (result->lnum > syn_buf->b_ml.ml_সাৰি_গুনতি)
	col = 0;
    অন্যথা যদি (off != 0)
    {
	base = ml_get_buf(syn_buf, result->lnum, FALSE);
	p = base + col;
	যদি (off > 0)
	{
	    যেতিয়ালৈ (off-- > 0 && *p != NUL)
		MB_PTR_ADV(p);
	}
	অন্যথা যদি (off < 0)
	{
	    যেতিয়ালৈ (off++ < 0 && base < p)
		MB_PTR_BACK(base, p);
	}
	col = (পূর্ণাঙ্ক)(p - base);
    }
    result->col = col;
}

/*
 * Add offset to matched text for start of match or highlight.
 * Avoid resulting column to become negative.
 */
    স্থির শূন্য
syn_add_start_off(
    lpos_T	*result,	// returned position
    regmmatch_T	*regmatch,	// start/end of match
    synpat_T	*spp,
    পূর্ণাঙ্ক		idx,
    পূর্ণাঙ্ক		extra)	    // extra chars for offset to end
{
    পূর্ণাঙ্ক		col;
    পূর্ণাঙ্ক		off;
    char_u	*base;
    char_u	*p;

    যদি (spp->sp_off_flags & (1 << (idx + SPO_গুনতি)))
    {
	result->lnum = regmatch->endpos[0].lnum;
	col = regmatch->endpos[0].col;
	off = spp->sp_offsets[idx] + extra;
    }
    অন্যথা
    {
	result->lnum = regmatch->startpos[0].lnum;
	col = regmatch->startpos[0].col;
	off = spp->sp_offsets[idx];
    }
    যদি (result->lnum > syn_buf->b_ml.ml_সাৰি_গুনতি)
    {
	// a "\n" at the end of the pattern may take us below the last line
	result->lnum = syn_buf->b_ml.ml_সাৰি_গুনতি;
	col = ml_get_buf_len(syn_buf, result->lnum);
    }
    যদি (off != 0)
    {
	base = ml_get_buf(syn_buf, result->lnum, FALSE);
	p = base + col;
	যদি (off > 0)
	{
	    যেতিয়ালৈ (off-- && *p != NUL)
		MB_PTR_ADV(p);
	}
	অন্যথা যদি (off < 0)
	{
	    যেতিয়ালৈ (off++ && base < p)
		MB_PTR_BACK(base, p);
	}
	col = (পূর্ণাঙ্ক)(p - base);
    }
    result->col = col;
}

/*
 * Get current line in syntax buffer.
 */
    স্থির char_u *
syn_getcurline(শূন্য)
{
    ঘূৰাই ml_get_buf(syn_buf, current_lnum, FALSE);
}

/*
 * Get length of current line in syntax buffer.
 */
    স্থির colnr_T
syn_getcurline_len(শূন্য)
{
    ঘূৰাই ml_get_buf_len(syn_buf, current_lnum);
}

/*
 * Call vim_regexec() to find a match with "rmp" in "syn_buf".
 * Returns TRUE when there is a match.
 */
    স্থির পূর্ণাঙ্ক
syn_regexec(
    regmmatch_T	*rmp,
    linenr_T	lnum,
    colnr_T	col,
    syn_time_T  *st UNUSED)
{
    পূর্ণাঙ্ক		r;
    পূর্ণাঙ্ক		timed_out = FALSE;
#ifdef FEAT_PROFILE
    proftime_T	pt;

    যদি (syn_time_on)
	profile_start(&pt);
#endif

    যদি (rmp->regprog == রিক্ত)
	// This can happen if a previous call to vim_regexec_multi() tried to
	// use the NFA engine, which resulted in NFA_TOO_EXPENSIVE, and
	// compiling the pattern with the other engine fails.
	ঘূৰাই FALSE;

    rmp->rmm_maxcol = syn_buf->b_p_smc;
    r = vim_regexec_multi(rmp, syn_win, syn_buf, lnum, col, &timed_out);

#ifdef FEAT_PROFILE
    যদি (syn_time_on)
    {
	profile_end(&pt);
	profile_add(&st->total, &pt);
	যদি (profile_cmp(&pt, &st->slowest) < 0)
	    st->slowest = pt;
	++st->গুনতি;
	যদি (r > 0)
	    ++st->match;
    }
#endif
#ifdef FEAT_RELTIME
    যদি (timed_out && redrawtime_limit_set && !syn_win->w_s->b_syn_slow)
    {
	syn_win->w_s->b_syn_slow = TRUE;
	বাৰ্তা(_("'redrawtime' exceeded, syntax highlighting disabled"));
    }
#endif

    যদি (r > 0)
    {
	rmp->startpos[0].lnum += lnum;
	rmp->endpos[0].lnum += lnum;
	ঘূৰাই TRUE;
    }
    ঘূৰাই FALSE;
}

/*
 * Check one position in a line for a matching keyword.
 * The caller must check if a keyword can start at startcol.
 * Return its ID if found, 0 otherwise.
 */
    স্থির পূর্ণাঙ্ক
check_keyword_id(
    char_u	*সাৰি,
    পূর্ণাঙ্ক		startcol,	// position in line to check for keyword
    পূর্ণাঙ্ক		*endcolp,	// return: character after found keyword
    long	*flagsp,	// return: flags of matching keyword
    short	**next_listp,	// return: next_list of matching keyword
    stateitem_T	*cur_si,	// item at the top of the stack
    পূর্ণাঙ্ক		*ccharp UNUSED)	// conceal substitution char
{
    keyentry_T	*kp;
    char_u	*kwp;
    পূর্ণাঙ্ক		round;
    পূর্ণাঙ্ক		kwlen;
    char_u	keyword[MAXKEYWLEN + 1]; // assume max. keyword len is 80
    hashtab_T	*ht;
    hashitem_T	*hi;

    // Find first character after the keyword.  First character was already
    // checked.
    kwp = সাৰি + startcol;
    kwlen = 0;
    কৰা
    {
	যদি (has_mbyte)
	    kwlen += (*mb_ptr2len)(kwp + kwlen);
	অন্যথা
	    ++kwlen;
    }
    যেতিয়ালৈ (vim_iswordp_buf(kwp + kwlen, syn_buf));

    যদি (kwlen > MAXKEYWLEN)
	ঘূৰাই 0;

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
    বাবে (round = 1; round <= 2; ++round)
    {
	ht = round == 1 ? &syn_block->b_keywtab : &syn_block->b_keywtab_ic;
	যদি (ht->ht_used == 0)
	    অনুৱর্ত;
	যদি (round == 2)	// ignore case
	    (শূন্য)str_foldcase(kwp, kwlen, keyword, MAXKEYWLEN + 1);

	/*
	 * Find keywords that match.  There can be several with different
	 * attributes.
	 * When current_next_list is non-zero accept only that group, otherwise:
	 *  Accept a not-contained keyword at toplevel.
	 *  Accept a keyword at other levels only if it is in the contains list.
	 */
	hi = hash_find(ht, keyword);
	যদি (!HASHITEM_EMPTY(hi))
	    বাবে (kp = HI2KE(hi); kp != রিক্ত; kp = kp->ke_next)
	    {
		যদি (current_next_list != 0
			? in_id_list(রিক্ত, current_next_list, &kp->k_syn, 0)
			: (cur_si == রিক্ত
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
		    ঘূৰাই kp->k_syn.id;
		}
	    }
    }
    ঘূৰাই 0;
}

/*
 * Handle ":syntax conceal" command.
 */
    স্থির শূন্য
syn_cmd_conceal(exarg_T *eap UNUSED, পূর্ণাঙ্ক syncing UNUSED)
{
#ifdef FEAT_CONCEAL
    char_u	*arg = eap->arg;
    char_u	*next;

    eap->nextcmd = find_nextcmd(arg);
    যদি (eap->skip)
	ঘূৰাই;

    next = skiptowhite(arg);
    যদি (*arg == NUL)
    {
	যদি (curwin->w_s->b_syn_conceal)
	    বাৰ্তা("syntax conceal on");
	অন্যথা
	    বাৰ্তা("syntax conceal off");
    }
    অন্যথা যদি (STRNICMP(arg, "on", 2) == 0 && next - arg == 2)
	curwin->w_s->b_syn_conceal = TRUE;
    অন্যথা যদি (STRNICMP(arg, "off", 3) == 0 && next - arg == 3)
	curwin->w_s->b_syn_conceal = FALSE;
    অন্যথা
	semsg(_(e_illegal_argument_str_2), arg);
#endif
}

/*
 * Handle ":syntax case" command.
 */
    স্থির শূন্য
syn_cmd_case(exarg_T *eap, পূর্ণাঙ্ক syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*next;

    eap->nextcmd = find_nextcmd(arg);
    যদি (eap->skip)
	ঘূৰাই;

    next = skiptowhite(arg);
    যদি (*arg == NUL)
    {
	যদি (curwin->w_s->b_syn_ic)
	    বাৰ্তা("syntax case ignore");
	অন্যথা
	    বাৰ্তা("syntax case match");
    }
    অন্যথা যদি (STRNICMP(arg, "match", 5) == 0 && next - arg == 5)
	curwin->w_s->b_syn_ic = FALSE;
    অন্যথা যদি (STRNICMP(arg, "ignore", 6) == 0 && next - arg == 6)
	curwin->w_s->b_syn_ic = TRUE;
    অন্যথা
	semsg(_(e_illegal_argument_str_2), arg);
}

/*
 * Handle ":syntax foldlevel" command.
 */
    স্থির শূন্য
syn_cmd_foldlevel(exarg_T *eap, পূর্ণাঙ্ক syncing UNUSED)
{
    char_u *arg = eap->arg;
    char_u *arg_end;

    eap->nextcmd = find_nextcmd(arg);
    যদি (eap->skip)
	ঘূৰাই;

    যদি (*arg == NUL)
    {
	ৱিকল্প (curwin->w_s->b_syn_foldlevel)
	{
	    পক্ষ SYNFLD_START:   বাৰ্তা("syntax foldlevel start");   ভঙ্গ;
	    পক্ষ SYNFLD_MINIMUM: বাৰ্তা("syntax foldlevel minimum"); ভঙ্গ;
	    স্ৱভাৱ: ভঙ্গ;
	}
	ঘূৰাই;
    }

    arg_end = skiptowhite(arg);
    যদি (STRNICMP(arg, "start", 5) == 0 && arg_end - arg == 5)
	curwin->w_s->b_syn_foldlevel = SYNFLD_START;
    অন্যথা যদি (STRNICMP(arg, "minimum", 7) == 0 && arg_end - arg == 7)
	curwin->w_s->b_syn_foldlevel = SYNFLD_MINIMUM;
    অন্যথা
    {
	semsg(_(e_illegal_argument_str_2), arg);
	ঘূৰাই;
    }

    arg = skipwhite(arg_end);
    যদি (*arg != NUL)
    {
	semsg(_(e_illegal_argument_str_2), arg);
    }
}

/*
 * Handle ":syntax spell" command.
 */
    স্থির শূন্য
syn_cmd_spell(exarg_T *eap, পূর্ণাঙ্ক syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*next;

    eap->nextcmd = find_nextcmd(arg);
    যদি (eap->skip)
	ঘূৰাই;

    next = skiptowhite(arg);
    যদি (*arg == NUL)
    {
	যদি (curwin->w_s->b_syn_spell == SYNSPL_শীৰ্ষ)
	    বাৰ্তা("syntax spell toplevel");
	অন্যথা যদি (curwin->w_s->b_syn_spell == SYNSPL_NOTOP)
	    বাৰ্তা("syntax spell notoplevel");
	অন্যথা
	    বাৰ্তা("syntax spell default");
    }
    অন্যথা যদি (STRNICMP(arg, "toplevel", 8) == 0 && next - arg == 8)
	curwin->w_s->b_syn_spell = SYNSPL_শীৰ্ষ;
    অন্যথা যদি (STRNICMP(arg, "notoplevel", 10) == 0 && next - arg == 10)
	curwin->w_s->b_syn_spell = SYNSPL_NOTOP;
    অন্যথা যদি (STRNICMP(arg, "default", 7) == 0 && next - arg == 7)
	curwin->w_s->b_syn_spell = SYNSPL_DEFAULT;
    অন্যথা
    {
	semsg(_(e_illegal_argument_str_2), arg);
	ঘূৰাই;
    }

    // assume spell checking changed, force a redraw
    redraw_win_later(curwin, UPD_NOT_VALID);
}

/*
 * Handle ":syntax iskeyword" command.
 */
    স্থির শূন্য
syn_cmd_iskeyword(exarg_T *eap, পূর্ণাঙ্ক syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	save_chartab[32];
    char_u	*save_isk;

    যদি (eap->skip)
	ঘূৰাই;

    arg = skipwhite(arg);
    যদি (*arg == NUL)
    {
	বাৰ্তা_puts("\n");
	যদি (curwin->w_s->b_syn_isk != empty_option)
	{
	    বাৰ্তা_puts("syntax iskeyword ");
	    বাৰ্তা_outtrans(curwin->w_s->b_syn_isk);
	}
	অন্যথা
	    বাৰ্তা_outtrans((char_u *)_("syntax iskeyword not set"));
    }
    অন্যথা
    {
	যদি (STRNICMP(arg, "clear", 5) == 0)
	{
	    mch_memmove(curwin->w_s->b_syn_chartab, curbuf->b_chartab,
								  (size_t)32);
	    clear_string_option(&curwin->w_s->b_syn_isk);
	}
	অন্যথা
	{
	    যদি (check_isopt(arg) == FAIL)
	    {
		emsg(_(e_invalid_argument));
		ঘূৰাই;
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
    শূন্য
syntax_clear(synblock_T *block)
{
    পূর্ণাঙ্ক i;

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
    বাবে (i = block->b_syn_patterns.ga_len; --i >= 0; )
	syn_clear_pattern(block, i);
    ga_clear(&block->b_syn_patterns);

    // free the syntax clusters
    বাবে (i = block->b_syn_clusters.ga_len; --i >= 0; )
	syn_clear_cluster(block, i);
    ga_clear(&block->b_syn_clusters);
    block->b_spell_cluster_id = 0;
    block->b_nospell_cluster_id = 0;

    block->b_syn_sync_flags = 0;
    block->b_syn_sync_minlines = 0;
    block->b_syn_sync_maxlines = 0;
    block->b_syn_sync_linebreaks = 0;

    vim_regfree(block->b_syn_linecont_prog);
    block->b_syn_linecont_prog = রিক্ত;
    VIM_CLEAR(block->b_syn_linecont_pat);
#ifdef FEAT_FOLDING
    block->b_syn_folditems = 0;
#endif
    clear_string_option(&block->b_syn_isk);

    // free the stored states
    syn_স্তূপ_free_all(block);
    invalidate_current_state();

    // Reset the counter for ":syn include"
    running_syn_inc_tag = 0;
}

/*
 * Get rid of ownsyntax for window "wp".
 */
    শূন্য
reset_synblock(win_T *wp)
{
    যদি (wp->w_s != &wp->w_buffer->b_s)
    {
	syntax_clear(wp->w_s);
	vim_free(wp->w_s);
	wp->w_s = &wp->w_buffer->b_s;
    }
}

/*
 * Clear syncing info for one buffer.
 */
    স্থির শূন্য
syntax_sync_clear(শূন্য)
{
    পূর্ণাঙ্ক i;

    // free the syntax patterns
    বাবে (i = curwin->w_s->b_syn_patterns.ga_len; --i >= 0; )
	যদি (SYN_বস্তুসমূহ(curwin->w_s)[i].sp_syncing)
	    syn_remove_pattern(curwin->w_s, i);

    curwin->w_s->b_syn_sync_flags = 0;
    curwin->w_s->b_syn_sync_minlines = 0;
    curwin->w_s->b_syn_sync_maxlines = 0;
    curwin->w_s->b_syn_sync_linebreaks = 0;

    vim_regfree(curwin->w_s->b_syn_linecont_prog);
    curwin->w_s->b_syn_linecont_prog = রিক্ত;
    VIM_CLEAR(curwin->w_s->b_syn_linecont_pat);
    clear_string_option(&curwin->w_s->b_syn_isk);

    syn_স্তূপ_free_all(curwin->w_s);	// Need to recompute all syntax.
}

/*
 * Remove one pattern from the buffer's pattern list.
 */
    স্থির শূন্য
syn_remove_pattern(
    synblock_T	*block,
    পূর্ণাঙ্ক		idx)
{
    synpat_T	*spp;

    spp = &(SYN_বস্তুসমূহ(block)[idx]);
#ifdef FEAT_FOLDING
    যদি (spp->sp_flags & HL_FOLD)
	--block->b_syn_folditems;
#endif
    syn_clear_pattern(block, idx);
    mch_memmove(spp, spp + 1,
		   পরিমাণ(synpat_T) * (block->b_syn_patterns.ga_len - idx - 1));
    --block->b_syn_patterns.ga_len;
}

/*
 * Clear and free one syntax pattern.  When clearing all, must be called from
 * last to first!
 */
    স্থির শূন্য
syn_clear_pattern(synblock_T *block, পূর্ণাঙ্ক i)
{
    vim_free(SYN_বস্তুসমূহ(block)[i].sp_pattern);
    vim_regfree(SYN_বস্তুসমূহ(block)[i].sp_prog);
    // Only free sp_cont_list and sp_next_list of first start pattern
    যদি (i == 0 || SYN_বস্তুসমূহ(block)[i - 1].sp_type != SPTYPE_START)
    {
	vim_free(SYN_বস্তুসমূহ(block)[i].sp_cont_list);
	vim_free(SYN_বস্তুসমূহ(block)[i].sp_next_list);
	vim_free(SYN_বস্তুসমূহ(block)[i].sp_syn.cont_in_list);
    }
}

/*
 * Clear and free one syntax cluster.
 */
    স্থির শূন্য
syn_clear_cluster(synblock_T *block, পূর্ণাঙ্ক i)
{
    vim_free(SYN_CLSTR(block)[i].scl_name);
    vim_free(SYN_CLSTR(block)[i].scl_name_u);
    vim_free(SYN_CLSTR(block)[i].scl_list);
}

/*
 * Handle ":syntax clear" command.
 */
    স্থির শূন্য
syn_cmd_clear(exarg_T *eap, পূর্ণাঙ্ক syncing)
{
    char_u	*arg = eap->arg;
    char_u	*arg_end;
    পূর্ণাঙ্ক		id;

    eap->nextcmd = find_nextcmd(arg);
    যদি (eap->skip)
	ঘূৰাই;

    /*
     * We have to disable this within ":syn include @group filename",
     * because otherwise @group would get deleted.
     * Only required for Vim 5.x syntax files, 6.0 ones don't contain ":syn
     * clear".
     */
    যদি (curwin->w_s->b_syn_topgrp != 0)
	ঘূৰাই;

    যদি (ends_excmd2(eap->cmd, arg))
    {
	/*
	 * No argument: Clear all syntax items.
	 */
	যদি (syncing)
	    syntax_sync_clear();
	অন্যথা
	{
	    syntax_clear(curwin->w_s);
	    যদি (curwin->w_s == &curwin->w_buffer->b_s)
		do_unlet((char_u *)"b:current_syntax", TRUE);
	    do_unlet((char_u *)"w:current_syntax", TRUE);
	}
    }
    অন্যথা
    {
	/*
	 * Clear the group IDs that are in the argument.
	 */
	যেতিয়ালৈ (!ends_excmd2(eap->cmd, arg))
	{
	    arg_end = skiptowhite(arg);
	    যদি (*arg == '@')
	    {
		id = syn_scl_namen2id(arg + 1, (পূর্ণাঙ্ক)(arg_end - arg - 1));
		যদি (id == 0)
		{
		    semsg(_(e_no_such_syntax_cluster_str_1), arg);
		    ভঙ্গ;
		}
		অন্যথা
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
	    অন্যথা
	    {
		id = syn_namen2id(arg, (পূর্ণাঙ্ক)(arg_end - arg));
		যদি (id == 0)
		{
		    semsg(_(e_no_such_highlight_group_name_str), arg);
		    ভঙ্গ;
		}
		অন্যথা
		    syn_clear_one(id, syncing);
	    }
	    arg = skipwhite(arg_end);
	}
    }
    redraw_curbuf_later(UPD_SOME_VALID);
    syn_স্তূপ_free_all(curwin->w_s);		// Need to recompute all syntax.
}

/*
 * Clear one syntax group for the current buffer.
 */
    স্থির শূন্য
syn_clear_one(পূর্ণাঙ্ক id, পূর্ণাঙ্ক syncing)
{
    synpat_T	*spp;
    পূর্ণাঙ্ক		idx;

    // Clear keywords only when not ":syn sync clear group-name"
    যদি (!syncing)
    {
	(শূন্য)syn_clear_keyword(id, &curwin->w_s->b_keywtab);
	(শূন্য)syn_clear_keyword(id, &curwin->w_s->b_keywtab_ic);
    }

    // clear the patterns for "id"
    বাবে (idx = curwin->w_s->b_syn_patterns.ga_len; --idx >= 0; )
    {
	spp = &(SYN_বস্তুসমূহ(curwin->w_s)[idx]);
	যদি (spp->sp_syn.id != id || spp->sp_syncing != syncing)
	    অনুৱর্ত;
	syn_remove_pattern(curwin->w_s, idx);
    }
}

/*
 * Handle ":syntax on" command.
 */
    স্থির শূন্য
syn_cmd_on(exarg_T *eap, পূর্ণাঙ্ক syncing UNUSED)
{
    syn_cmd_onoff(eap, "syntax");
}

/*
 * Handle ":syntax enable" command.
 */
    স্থির শূন্য
syn_cmd_enable(exarg_T *eap, পূর্ণাঙ্ক syncing UNUSED)
{
    set_internal_string_var((char_u *)"g:syntax_cmd", (char_u *)"enable");
    syn_cmd_onoff(eap, "syntax");
    do_unlet((char_u *)"g:syntax_cmd", TRUE);
}

/*
 * Handle ":syntax reset" command.
 * It actually resets highlighting, not syntax.
 */
    স্থির শূন্য
syn_cmd_reset(exarg_T *eap, পূর্ণাঙ্ক syncing UNUSED)
{
    set_nextcmd(eap, eap->arg);
    যদি (!eap->skip)
    {
	set_internal_string_var((char_u *)"g:syntax_cmd", (char_u *)"reset");
	do_cmdline_cmd((char_u *)"runtime! syntax/syncolor.vim");
	do_unlet((char_u *)"g:syntax_cmd", TRUE);
    }
}

/*
 * Handle ":syntax manual" command.
 */
    স্থির শূন্য
syn_cmd_manual(exarg_T *eap, পূর্ণাঙ্ক syncing UNUSED)
{
    syn_cmd_onoff(eap, "manual");
}

/*
 * Handle ":syntax off" command.
 */
    স্থির শূন্য
syn_cmd_off(exarg_T *eap, পূর্ণাঙ্ক syncing UNUSED)
{
    syn_cmd_onoff(eap, "nosyntax");
}

    স্থির শূন্য
syn_cmd_onoff(exarg_T *eap, ৱর্ণ *name)
{
    char_u	buf[100];

    set_nextcmd(eap, eap->arg);
    যদি (!eap->skip)
    {
	STRCPY(buf, "so ");
	vim_snprintf((ৱর্ণ *)buf + 3, পরিমাণ(buf) - 3, SYNTAX_FNAME, name);
	do_cmdline_cmd(buf);
    }
}

/*
 * Handle ":syntax [list]" command: list current syntax words.
 */
    স্থির শূন্য
syn_cmd_list(
    exarg_T	*eap,
    পূর্ণাঙ্ক		syncing)	    // when TRUE: list syncing items
{
    char_u	*arg = eap->arg;
    পূর্ণাঙ্ক		id;
    char_u	*arg_end;

    eap->nextcmd = find_nextcmd(arg);
    যদি (eap->skip)
	ঘূৰাই;

    যদি (!syntax_present(curwin))
    {
	বাৰ্তা(_(বাৰ্তা_no_বস্তুসমূহ));
	ঘূৰাই;
    }

    যদি (syncing)
    {
	যদি (curwin->w_s->b_syn_sync_flags & SF_CCOMMENT)
	{
	    বাৰ্তা_puts(_("syncing on C-style comments"));
	    syn_সাৰিগুলি_বাৰ্তা();
	    syn_match_বাৰ্তা();
	}
	অন্যথা যদি (curwin->w_s->b_syn_sync_flags & SF_MATCH)
	{
	    বাৰ্তা_puts_title(_("\n--- Syntax sync items ---"));
	    যদি (curwin->w_s->b_syn_sync_minlines > 0
		|| curwin->w_s->b_syn_sync_maxlines > 0
		|| curwin->w_s->b_syn_sync_linebreaks > 0)
	    {
		বাৰ্তা_puts(_("\nsyncing on items"));
		syn_সাৰিগুলি_বাৰ্তা();
		syn_match_বাৰ্তা();
	    }
	    বাবে (id = 1; id <= highlight_অঙ্ক_groups() && !got_int; ++id)
		syn_list_one(id, syncing, FALSE);
	}
	অন্যথা
	{
	    যদি (curwin->w_s->b_syn_sync_minlines == 0)
		বাৰ্তা_puts(_("no syncing"));
	    অন্যথা
	    {
		যদি (curwin->w_s->b_syn_sync_minlines == MAXLNUM)
		    বাৰ্তা_puts(_("syncing starts at the first line"));
		অন্যথা
		{
		    বাৰ্তা_puts(_("syncing starts "));
		    বাৰ্তা_outnum(curwin->w_s->b_syn_sync_minlines);
		    বাৰ্তা_puts(_(" lines before top line"));
		}
		syn_match_বাৰ্তা();
	    }
	}
	ঘূৰাই;
    }

    বাৰ্তা_puts_title(_("\n--- Syntax items ---"));
    যদি (ends_excmd2(eap->cmd, arg))
    {
	/*
	 * No argument: List all group IDs and all syntax clusters.
	 */
	বাবে (id = 1; id <= highlight_অঙ্ক_groups() && !got_int; ++id)
	    syn_list_one(id, syncing, FALSE);
	বাবে (id = 0; id < curwin->w_s->b_syn_clusters.ga_len && !got_int; ++id)
	    syn_list_cluster(id);
    }
    অন্যথা
    {
	/*
	 * List the group IDs and syntax clusters that are in the argument.
	 */
	যেতিয়ালৈ (!ends_excmd2(eap->cmd, arg) && !got_int)
	{
	    arg_end = skiptowhite(arg);
	    যদি (*arg == '@')
	    {
		id = syn_scl_namen2id(arg + 1, (পূর্ণাঙ্ক)(arg_end - arg - 1));
		যদি (id == 0)
		    semsg(_(e_no_such_syntax_cluster_str_2), arg);
		অন্যথা
		    syn_list_cluster(id - SYNID_CLUSTER);
	    }
	    অন্যথা
	    {
		id = syn_namen2id(arg, (পূর্ণাঙ্ক)(arg_end - arg));
		যদি (id == 0)
		    semsg(_(e_no_such_highlight_group_name_str), arg);
		অন্যথা
		    syn_list_one(id, syncing, TRUE);
	    }
	    arg = skipwhite(arg_end);
	}
    }
    set_nextcmd(eap, arg);
}

    স্থির শূন্য
syn_সাৰিগুলি_বাৰ্তা(শূন্য)
{
    যদি (curwin->w_s->b_syn_sync_maxlines > 0
				      || curwin->w_s->b_syn_sync_minlines > 0)
    {
	বাৰ্তা_puts("; ");
	যদি (curwin->w_s->b_syn_sync_minlines == MAXLNUM)
	    বাৰ্তা_puts(_("from the first line"));
	অন্যথা
	{
	    যদি (curwin->w_s->b_syn_sync_minlines > 0)
	    {
		বাৰ্তা_puts(_("minimal "));
		বাৰ্তা_outnum(curwin->w_s->b_syn_sync_minlines);
		যদি (curwin->w_s->b_syn_sync_maxlines)
		    বাৰ্তা_puts(", ");
	    }
	    যদি (curwin->w_s->b_syn_sync_maxlines > 0)
	    {
		বাৰ্তা_puts(_("maximal "));
		বাৰ্তা_outnum(curwin->w_s->b_syn_sync_maxlines);
	    }
	    বাৰ্তা_puts(_(" lines before top line"));
	}
    }
}

    স্থির শূন্য
syn_match_বাৰ্তা(শূন্য)
{
    যদি (curwin->w_s->b_syn_sync_linebreaks > 0)
    {
	বাৰ্তা_puts(_("; match "));
	বাৰ্তা_outnum(curwin->w_s->b_syn_sync_linebreaks);
	বাৰ্তা_puts(_(" line breaks"));
    }
}

স্থির পূর্ণাঙ্ক  last_matchgroup;

স্থির শূন্য syn_list_flags(keyvalue_T *nlist, পূর্ণাঙ্ক nr_entries, পূর্ণাঙ্ক flags, পূর্ণাঙ্ক attr);

/*
 * List one syntax item, for ":syntax" or "syntax list syntax_name".
 */
    স্থির শূন্য
syn_list_one(
    পূর্ণাঙ্ক		id,
    পূর্ণাঙ্ক		syncing,	    // when TRUE: list syncing items
    পূর্ণাঙ্ক		link_only)	    // when TRUE; list link-only too
{
    পূর্ণাঙ্ক		attr;
    পূর্ণাঙ্ক		idx;
    পূর্ণাঙ্ক		did_header = FALSE;
    synpat_T	*spp;
    স্থির keyvalue_T namelist1[] =
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
    স্থির keyvalue_T namelist2[] =
		{
		    KEYVALUE_ENTRY(HL_SKIPWHITE, "skipwhite"),
		    KEYVALUE_ENTRY(HL_SKIPNL, "skipnl"),
		    KEYVALUE_ENTRY(HL_SKIPEMPTY, "skipempty")
		};

    attr = HL_ATTR(HLF_D);		// highlight like directories

    // list the keywords for "id"
    যদি (!syncing)
    {
	did_header = syn_list_keywords(id, &curwin->w_s->b_keywtab, FALSE, attr);
	did_header = syn_list_keywords(id, &curwin->w_s->b_keywtab_ic,
							    did_header, attr);
    }

    // list the patterns for "id"
    বাবে (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len && !got_int; ++idx)
    {
	spp = &(SYN_বস্তুসমূহ(curwin->w_s)[idx]);
	যদি (spp->sp_syn.id != id || spp->sp_syncing != syncing)
	    অনুৱর্ত;

	(শূন্য)syn_list_header(did_header, 999, id);
	did_header = TRUE;
	last_matchgroup = 0;
	যদি (spp->sp_type == SPTYPE_MATCH)
	{
	    put_pattern("match", ' ', spp, attr);
	    বাৰ্তা_putchar(' ');
	}
	অন্যথা যদি (spp->sp_type == SPTYPE_START)
	{
	    যেতিয়ালৈ (SYN_বস্তুসমূহ(curwin->w_s)[idx].sp_type == SPTYPE_START)
		put_pattern("start", '=', &SYN_বস্তুসমূহ(curwin->w_s)[idx++], attr);
	    যদি (SYN_বস্তুসমূহ(curwin->w_s)[idx].sp_type == SPTYPE_SKIP)
		put_pattern("skip", '=', &SYN_বস্তুসমূহ(curwin->w_s)[idx++], attr);
	    যেতিয়ালৈ (idx < curwin->w_s->b_syn_patterns.ga_len
			  && SYN_বস্তুসমূহ(curwin->w_s)[idx].sp_type == SPTYPE_END)
		put_pattern("end", '=', &SYN_বস্তুসমূহ(curwin->w_s)[idx++], attr);
	    --idx;
	    বাৰ্তা_putchar(' ');
	}
	syn_list_flags(namelist1, (পূর্ণাঙ্ক)ARRAY_LENGTH(namelist1), spp->sp_flags, attr);

	যদি (spp->sp_cont_list != রিক্ত)
	    put_id_list((char_u *)"contains", spp->sp_cont_list, attr);

	যদি (spp->sp_syn.cont_in_list != রিক্ত)
	    put_id_list((char_u *)"containedin",
					      spp->sp_syn.cont_in_list, attr);

	যদি (spp->sp_next_list != রিক্ত)
	{
	    put_id_list((char_u *)"nextgroup", spp->sp_next_list, attr);
	    syn_list_flags(namelist2, (পূর্ণাঙ্ক)ARRAY_LENGTH(namelist2), spp->sp_flags, attr);
	}
	যদি (spp->sp_flags & (HL_SYNC_HERE|HL_SYNC_THERE))
	{
	    যদি (spp->sp_flags & HL_SYNC_HERE)
		বাৰ্তা_puts_attr("grouphere", attr);
	    অন্যথা
		বাৰ্তা_puts_attr("groupthere", attr);
	    বাৰ্তা_putchar(' ');
	    যদি (spp->sp_sync_idx >= 0)
		বাৰ্তা_outtrans(highlight_group_name(SYN_বস্তুসমূহ(curwin->w_s)
				   [spp->sp_sync_idx].sp_syn.id - 1));
	    অন্যথা
		বাৰ্তা_puts("NONE");
	    বাৰ্তা_putchar(' ');
	}
    }

    // list the link, if there is one
    যদি (highlight_link_id(id - 1) && (did_header || link_only) && !got_int)
    {
	(শূন্য)syn_list_header(did_header, 999, id);
	বাৰ্তা_puts_attr("links to", attr);
	বাৰ্তা_putchar(' ');
	বাৰ্তা_outtrans(highlight_group_name(highlight_link_id(id - 1) - 1));
    }
}

    স্থির শূন্য
syn_list_flags(keyvalue_T *nlist, পূর্ণাঙ্ক nr_entries, পূর্ণাঙ্ক flags, পূর্ণাঙ্ক attr)
{
    পূর্ণাঙ্ক		i;

    বাবে (i = 0; i < nr_entries; ++i)
	যদি (flags & nlist[i].চাবি)
	{
	    বাৰ্তা_puts_attr((ৱর্ণ *)nlist[i].মান.string, attr);
	    বাৰ্তা_putchar(' ');
	}
}

/*
 * List one syntax cluster, for ":syntax" or "syntax list syntax_name".
 */
    স্থির শূন্য
syn_list_cluster(পূর্ণাঙ্ক id)
{
    পূর্ণাঙ্ক	    endcol = 15;

    // slight hack:  roughly duplicate the guts of syn_list_header()
    বাৰ্তা_putchar('\n');
    বাৰ্তা_outtrans(SYN_CLSTR(curwin->w_s)[id].scl_name);

    যদি (বাৰ্তা_col >= endcol)	// output at least one space
	endcol = বাৰ্তা_col + 1;
    যদি (cmdline_width <= endcol)	// avoid hang for tiny window
	endcol = cmdline_width - 1;

    বাৰ্তা_advance(endcol);
    যদি (SYN_CLSTR(curwin->w_s)[id].scl_list != রিক্ত)
    {
	put_id_list((char_u *)"cluster", SYN_CLSTR(curwin->w_s)[id].scl_list,
		    HL_ATTR(HLF_D));
    }
    অন্যথা
    {
	বাৰ্তা_puts_attr("cluster", HL_ATTR(HLF_D));
	বাৰ্তা_puts("=NONE");
    }
}

    স্থির শূন্য
put_id_list(char_u *name, short *list, পূর্ণাঙ্ক attr)
{
    short		*p;

    বাৰ্তা_puts_attr((ৱর্ণ *)name, attr);
    বাৰ্তা_putchar('=');
    বাবে (p = list; *p; ++p)
    {
	যদি (*p >= SYNID_ALLBUT && *p < SYNID_শীৰ্ষ)
	{
	    যদি (p[1])
		বাৰ্তা_puts("ALLBUT");
	    অন্যথা
		বাৰ্তা_puts("ALL");
	}
	অন্যথা যদি (*p >= SYNID_শীৰ্ষ && *p < SYNID_CONTAINED)
	{
	    বাৰ্তা_puts("TOP");
	}
	অন্যথা যদি (*p >= SYNID_CONTAINED && *p < SYNID_CLUSTER)
	{
	    বাৰ্তা_puts("CONTAINED");
	}
	অন্যথা যদি (*p >= SYNID_CLUSTER)
	{
	    short scl_id = *p - SYNID_CLUSTER;

	    বাৰ্তা_putchar('@');
	    বাৰ্তা_outtrans(SYN_CLSTR(curwin->w_s)[scl_id].scl_name);
	}
	অন্যথা
	    বাৰ্তা_outtrans(highlight_group_name(*p - 1));
	যদি (p[1])
	    বাৰ্তা_putchar(',');
    }
    বাৰ্তা_putchar(' ');
}

    স্থির শূন্য
put_pattern(
    ৱর্ণ	*s,
    পূর্ণাঙ্ক		c,
    synpat_T	*spp,
    পূর্ণাঙ্ক		attr)
{
    long	n;
    পূর্ণাঙ্ক		mask;
    পূর্ণাঙ্ক		first;
    স্থির ৱর্ণ	*sepchars = "/+=-#@\"|'^&";
    পূর্ণাঙ্ক		i;

    // May have to write "matchgroup=group"
    যদি (last_matchgroup != spp->sp_syn_match_id)
    {
	last_matchgroup = spp->sp_syn_match_id;
	বাৰ্তা_puts_attr("matchgroup", attr);
	বাৰ্তা_putchar('=');
	যদি (last_matchgroup == 0)
	    বাৰ্তা_outtrans((char_u *)"NONE");
	অন্যথা
	    বাৰ্তা_outtrans(highlight_group_name(last_matchgroup - 1));
	বাৰ্তা_putchar(' ');
    }

    // output the name of the pattern and an '=' or ' '
    বাৰ্তা_puts_attr(s, attr);
    বাৰ্তা_putchar(c);

    // output the pattern, in between a char that is not in the pattern
    বাবে (i = 0; vim_strchr(spp->sp_pattern, sepchars[i]) != রিক্ত; )
	যদি (sepchars[++i] == NUL)
	{
	    i = 0;	// no good char found, just use the first one
	    ভঙ্গ;
	}
    বাৰ্তা_putchar(sepchars[i]);
    বাৰ্তা_outtrans(spp->sp_pattern);
    বাৰ্তা_putchar(sepchars[i]);

    // output any pattern options
    first = TRUE;
    বাবে (i = 0; i < SPO_গুনতি; ++i)
    {
	mask = (1 << i);
	যদি (spp->sp_off_flags & (mask + (mask << SPO_গুনতি)))
	{
	    যদি (!first)
		বাৰ্তা_putchar(',');	// separate with commas
	    বাৰ্তা_puts(spo_name_tab[i]);
	    n = spp->sp_offsets[i];
	    যদি (i != SPO_LC_OFF)
	    {
		যদি (spp->sp_off_flags & mask)
		    বাৰ্তা_putchar('s');
		অন্যথা
		    বাৰ্তা_putchar('e');
		যদি (n > 0)
		    বাৰ্তা_putchar('+');
	    }
	    যদি (n || i == SPO_LC_OFF)
		বাৰ্তা_outnum(n);
	    first = FALSE;
	}
    }
    বাৰ্তা_putchar(' ');
}

/*
 * List or clear the keywords for one syntax group.
 * Return TRUE if the header has been printed.
 */
    স্থির পূর্ণাঙ্ক
syn_list_keywords(
    পূর্ণাঙ্ক		id,
    hashtab_T	*ht,
    পূর্ণাঙ্ক		did_header,		// header has already been printed
    পূর্ণাঙ্ক		attr)
{
    পূর্ণাঙ্ক		outlen;
    hashitem_T	*hi;
    keyentry_T	*kp;
    পূর্ণাঙ্ক		todo;
    পূর্ণাঙ্ক		prev_contained = 0;
    short	*prev_next_list = রিক্ত;
    short	*prev_cont_in_list = রিক্ত;
    পূর্ণাঙ্ক		prev_skipnl = 0;
    পূর্ণাঙ্ক		prev_skipwhite = 0;
    পূর্ণাঙ্ক		prev_skipempty = 0;

    /*
     * Unfortunately, this list of keywords is not sorted on alphabet but on
     * hash value...
     */
    todo = (পূর্ণাঙ্ক)ht->ht_used;
    বাবে (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    বাবে (kp = HI2KE(hi); kp != রিক্ত && !got_int; kp = kp->ke_next)
	    {
		যদি (kp->k_syn.id == id)
		{
		    যদি (prev_contained != (kp->flags & HL_CONTAINED)
			    || prev_skipnl != (kp->flags & HL_SKIPNL)
			    || prev_skipwhite != (kp->flags & HL_SKIPWHITE)
			    || prev_skipempty != (kp->flags & HL_SKIPEMPTY)
			    || prev_cont_in_list != kp->k_syn.cont_in_list
			    || prev_next_list != kp->next_list)
			outlen = 9999;
		    অন্যথা
			outlen = (পূর্ণাঙ্ক)STRLEN(kp->keyword);
		    // output "contained" and "nextgroup" on each line
		    যদি (syn_list_header(did_header, outlen, id))
		    {
			prev_contained = 0;
			prev_next_list = রিক্ত;
			prev_cont_in_list = রিক্ত;
			prev_skipnl = 0;
			prev_skipwhite = 0;
			prev_skipempty = 0;
		    }
		    did_header = TRUE;
		    যদি (prev_contained != (kp->flags & HL_CONTAINED))
		    {
			বাৰ্তা_puts_attr("contained", attr);
			বাৰ্তা_putchar(' ');
			prev_contained = (kp->flags & HL_CONTAINED);
		    }
		    যদি (kp->k_syn.cont_in_list != prev_cont_in_list)
		    {
			put_id_list((char_u *)"containedin",
						kp->k_syn.cont_in_list, attr);
			বাৰ্তা_putchar(' ');
			prev_cont_in_list = kp->k_syn.cont_in_list;
		    }
		    যদি (kp->next_list != prev_next_list)
		    {
			put_id_list((char_u *)"nextgroup", kp->next_list, attr);
			বাৰ্তা_putchar(' ');
			prev_next_list = kp->next_list;
			যদি (kp->flags & HL_SKIPNL)
			{
			    বাৰ্তা_puts_attr("skipnl", attr);
			    বাৰ্তা_putchar(' ');
			    prev_skipnl = (kp->flags & HL_SKIPNL);
			}
			যদি (kp->flags & HL_SKIPWHITE)
			{
			    বাৰ্তা_puts_attr("skipwhite", attr);
			    বাৰ্তা_putchar(' ');
			    prev_skipwhite = (kp->flags & HL_SKIPWHITE);
			}
			যদি (kp->flags & HL_SKIPEMPTY)
			{
			    বাৰ্তা_puts_attr("skipempty", attr);
			    বাৰ্তা_putchar(' ');
			    prev_skipempty = (kp->flags & HL_SKIPEMPTY);
			}
		    }
		    বাৰ্তা_outtrans(kp->keyword);
		}
	    }
	}
    }

    ঘূৰাই did_header;
}

    স্থির শূন্য
syn_clear_keyword(পূর্ণাঙ্ক id, hashtab_T *ht)
{
    hashitem_T	*hi;
    keyentry_T	*kp;
    keyentry_T	*kp_prev;
    keyentry_T	*kp_next;
    পূর্ণাঙ্ক		todo;

    hash_lock(ht);
    todo = (পূর্ণাঙ্ক)ht->ht_used;
    FOR_ALL_HASHTAB_বস্তুসমূহ(ht, hi, todo)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    kp_prev = রিক্ত;
	    বাবে (kp = HI2KE(hi); kp != রিক্ত; )
	    {
		যদি (kp->k_syn.id == id)
		{
		    kp_next = kp->ke_next;
		    যদি (kp_prev == রিক্ত)
		    {
			যদি (kp_next == রিক্ত)
			    hash_remove(ht, hi, "syntax clear keyword");
			অন্যথা
			    hi->hi_চাবি = KE2HIKEY(kp_next);
		    }
		    অন্যথা
			kp_prev->ke_next = kp_next;
		    vim_free(kp->next_list);
		    vim_free(kp->k_syn.cont_in_list);
		    vim_free(kp);
		    kp = kp_next;
		}
		অন্যথা
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
    স্থির শূন্য
clear_keywtab(hashtab_T *ht)
{
    hashitem_T	*hi;
    পূর্ণাঙ্ক		todo;
    keyentry_T	*kp;
    keyentry_T	*kp_next;

    todo = (পূর্ণাঙ্ক)ht->ht_used;
    FOR_ALL_HASHTAB_বস্তুসমূহ(ht, hi, todo)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    বাবে (kp = HI2KE(hi); kp != রিক্ত; kp = kp_next)
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
    স্থির শূন্য
add_keyword(
    char_u	*name,	    // name of keyword
    size_t	namelen,    // length of keyword (excluding the NUL)
    পূর্ণাঙ্ক		id,	    // group ID for this keyword
    পূর্ণাঙ্ক		flags,	    // flags for this keyword
    short	*cont_in_list, // containedin for this keyword
    short	*next_list, // nextgroup for this keyword
    পূর্ণাঙ্ক		conceal_char)
{
    keyentry_T	*kp;
    hashtab_T	*ht;
    hashitem_T	*hi;
    char_u	*name_ic;
    size_t	name_iclen;
    long_u	hash;
    char_u	name_folded[MAXKEYWLEN + 1];

    যদি (curwin->w_s->b_syn_ic)
    {
	name_ic = str_foldcase(name, (পূর্ণাঙ্ক)namelen,
						 name_folded, MAXKEYWLEN + 1);
	name_iclen = STRLEN(name_ic);
    }
    অন্যথা
    {
	name_ic = name;
	name_iclen = namelen;
    }
    kp = alloc(offsetof(keyentry_T, keyword) + name_iclen + 1);
    যদি (kp == রিক্ত)
	ঘূৰাই;
    STRCPY(kp->keyword, name_ic);
    kp->k_syn.id = id;
    kp->k_syn.inc_tag = current_syn_inc_tag;
    kp->flags = flags;
    kp->k_char = conceal_char;
    kp->k_syn.cont_in_list = copy_id_list(cont_in_list);
    যদি (cont_in_list != রিক্ত)
	curwin->w_s->b_syn_containedin = TRUE;
    kp->next_list = copy_id_list(next_list);

    যদি (curwin->w_s->b_syn_ic)
	ht = &curwin->w_s->b_keywtab_ic;
    অন্যথা
	ht = &curwin->w_s->b_keywtab;

    hash = hash_hash(kp->keyword);
    hi = hash_lookup(ht, kp->keyword, hash);
    যদি (HASHITEM_EMPTY(hi))
    {
	// new keyword, add to hashtable
	kp->ke_next = রিক্ত;
	hash_add_item(ht, hi, kp->keyword, hash);
    }
    অন্যথা
    {
	// keyword already exists, prepend to list
	kp->ke_next = HI2KE(hi);
	hi->hi_চাবি = KE2HIKEY(kp);
    }
}

/*
 * Get the start and end of the group name argument.
 * Return a pointer to the first argument.
 * Return NULL if the end of the command was found instead of further args.
 */
    স্থির char_u *
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
    যদি (ends_excmd(*arg) || *rest == NUL)
	ঘূৰাই রিক্ত;
    ঘূৰাই rest;
}

/*
 * Check for syntax command option arguments.
 * This can be called at any place in the list of arguments, and just picks
 * out the arguments that are known.  Can be called several times in a row to
 * collect all options in between other arguments.
 * Return a pointer to the next argument (which isn't an option).
 * Return NULL for any error;
 */
    স্থির char_u *
get_syn_options(
    char_u	    *start,		// next argument to be checked
    syn_opt_arg_T   *opt,		// various things
    পূর্ণাঙ্ক		    *conceal_char UNUSED,
    পূর্ণাঙ্ক		    skip)		// TRUE if skipping over command
{
    char_u	*arg = start;
    char_u	*gname_start, *gname;
    পূর্ণাঙ্ক		syn_id;
    পূর্ণাঙ্ক		len;
    ৱর্ণ	*p;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		fidx;
    স্থির সংরচনা flag
    {
	ৱর্ণ	*name;
	পূর্ণাঙ্ক	argtype;
	পূর্ণাঙ্ক	flags;
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
    স্থির ৱর্ণ *first_letters = "cCoOkKeEtTsSgGdDfFnN";

    যদি (arg == রিক্ত)		// already detected error
	ঘূৰাই রিক্ত;

#ifdef FEAT_CONCEAL
    যদি (curwin->w_s->b_syn_conceal)
	opt->flags |= HL_CONCEAL;
#endif

    বাবে (;;)
    {
	/*
	 * This is used very often when a large number of keywords is defined.
	 * Need to skip quickly when no option name is found.
	 * Also avoid tolower(), it's slow.
	 */
	যদি (strchr(first_letters, *arg) == রিক্ত)
	    ভঙ্গ;

	বাবে (fidx = ARRAY_LENGTH(flagtab); --fidx >= 0; )
	{
	    p = flagtab[fidx].name;
	    বাবে (i = 0, len = 0; p[i] != NUL; i += 2, ++len)
		যদি (arg[len] != p[i] && arg[len] != p[i + 1])
		    ভঙ্গ;
	    যদি (p[i] == NUL && (VIM_ISWHITE(arg[len])
				    || (flagtab[fidx].argtype > 0
					 ? arg[len] == '='
					 : ends_excmd2(start, arg + len))))
	    {
		যদি (opt->keyword
			&& (flagtab[fidx].flags == HL_DISPLAY
			    || flagtab[fidx].flags == HL_FOLD
			    || flagtab[fidx].flags == HL_EXTEND))
		    // treat "display", "fold" and "extend" as a keyword
		    fidx = -1;
		ভঙ্গ;
	    }
	}
	যদি (fidx < 0)	    // no match found
	    ভঙ্গ;

	যদি (flagtab[fidx].argtype == 1)
	{
	    যদি (!opt->has_cont_list)
	    {
		emsg(_(e_contains_argument_not_accepted_here));
		ঘূৰাই রিক্ত;
	    }
	    যদি (get_id_list(&arg, 8, &opt->cont_list, skip) == FAIL)
		ঘূৰাই রিক্ত;
	}
	অন্যথা যদি (flagtab[fidx].argtype == 2)
	{
	    যদি (get_id_list(&arg, 11, &opt->cont_in_list, skip) == FAIL)
		ঘূৰাই রিক্ত;
	}
	অন্যথা যদি (flagtab[fidx].argtype == 3)
	{
	    যদি (get_id_list(&arg, 9, &opt->next_list, skip) == FAIL)
		ঘূৰাই রিক্ত;
	}
	অন্যথা যদি (flagtab[fidx].argtype == 11 && arg[5] == '=')
	{
	    // cchar=?
	    যদি (has_mbyte)
	    {
#ifdef FEAT_CONCEAL
		*conceal_char = mb_ptr2char(arg + 6);
#endif
		arg += mb_ptr2len(arg + 6) - 1;
	    }
	    অন্যথা
	    {
#ifdef FEAT_CONCEAL
		*conceal_char = arg[6];
#অন্যথা
		;
#endif
	    }
#ifdef FEAT_CONCEAL
	    যদি (!vim_isprintc_strict(*conceal_char))
	    {
		emsg(_(e_invalid_cchar_মান));
		ঘূৰাই রিক্ত;
	    }
#endif
	    arg = skipwhite(arg + 7);
	}
	অন্যথা
	{
	    opt->flags |= flagtab[fidx].flags;
	    arg = skipwhite(arg + len);

	    যদি (flagtab[fidx].flags == HL_SYNC_HERE
		    || flagtab[fidx].flags == HL_SYNC_THERE)
	    {
		যদি (opt->sync_idx == রিক্ত)
		{
		    emsg(_(e_groupthere_not_accepted_here));
		    ঘূৰাই রিক্ত;
		}
		gname_start = arg;
		arg = skiptowhite(arg);
		যদি (gname_start == arg)
		    ঘূৰাই রিক্ত;
		gname = vim_strnsave(gname_start, arg - gname_start);
		যদি (gname == রিক্ত)
		    ঘূৰাই রিক্ত;
		যদি (STRCMP(gname, "NONE") == 0)
		    *opt->sync_idx = NONE_IDX;
		অন্যথা
		{
		    syn_id = syn_name2id(gname);
		    বাবে (i = curwin->w_s->b_syn_patterns.ga_len; --i >= 0; )
			যদি (SYN_বস্তুসমূহ(curwin->w_s)[i].sp_syn.id == syn_id
			      && SYN_বস্তুসমূহ(curwin->w_s)[i].sp_type
							       == SPTYPE_START)
			{
			    *opt->sync_idx = i;
			    ভঙ্গ;
			}
		    যদি (i < 0)
		    {
			semsg(_(e_didnt_find_region_item_for_str), gname);
			vim_free(gname);
			ঘূৰাই রিক্ত;
		    }
		}

		vim_free(gname);
		arg = skipwhite(arg);
	    }
#ifdef FEAT_FOLDING
	    অন্যথা যদি (flagtab[fidx].flags == HL_FOLD
						&& foldmethodIsSyntax(curwin))
		// Need to update folds later.
		foldUpdateAll(curwin);
#endif
	}
    }

    ঘূৰাই arg;
}

/*
 * Adjustments to syntax item when declared in a ":syn include"'d file.
 * Set the contained flag, and if the item is not already contained, add it
 * to the specified top-level group, if any.
 */
    স্থির শূন্য
syn_incl_toplevel(পূর্ণাঙ্ক id, পূর্ণাঙ্ক *flagsp)
{
    যদি ((*flagsp & HL_CONTAINED) || curwin->w_s->b_syn_topgrp == 0)
	ঘূৰাই;
    *flagsp |= HL_CONTAINED | HL_INCLUDED_TOPLEVEL;
    যদি (curwin->w_s->b_syn_topgrp >= SYNID_CLUSTER)
    {
	// We have to alloc this, because syn_combine_list() will free it.
	short	    *grp_list = ALLOC_MULT(short, 2);
	পূর্ণাঙ্ক	    tlg_id = curwin->w_s->b_syn_topgrp - SYNID_CLUSTER;

	যদি (grp_list != রিক্ত)
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
    স্থির শূন্য
syn_cmd_include(exarg_T *eap, পূর্ণাঙ্ক syncing UNUSED)
{
    char_u	*arg = eap->arg;
    পূর্ণাঙ্ক		sgl_id = 1;
    char_u	*group_name_end;
    char_u	*rest;
    ৱর্ণ	*errormsg = রিক্ত;
    পূর্ণাঙ্ক		prev_toplvl_grp;
    পূর্ণাঙ্ক		prev_syn_inc_tag;
    পূর্ণাঙ্ক		source = FALSE;

    eap->nextcmd = find_nextcmd(arg);
    যদি (eap->skip)
	ঘূৰাই;

    যদি (arg[0] == '@')
    {
	++arg;
	rest = get_group_name(arg, &group_name_end);
	যদি (rest == রিক্ত)
	{
	    emsg(_(e_filename_required));
	    ঘূৰাই;
	}
	sgl_id = syn_check_cluster(arg, (পূর্ণাঙ্ক)(group_name_end - arg));
	যদি (sgl_id == 0)
	    ঘূৰাই;
	// separate_nextcmd() and expand_filename() depend on this
	eap->arg = rest;
    }

    /*
     * Everything that's left, up to the next command, should be the
     * filename to include.
     */
    eap->argt |= (EX_XFILE | EX_NOSPC);
    separate_nextcmd(eap, FALSE);
    যদি (*eap->arg == '<' || *eap->arg == '$' || mch_isFullName(eap->arg))
    {
	// For an absolute path, "$VIM/..." or "<sfile>.." we ":source" the
	// file.  Need to expand the file name first.  In other cases
	// ":runtime!" is used.
	source = TRUE;
	যদি (expand_filename(eap, syn_cmdlinep, &errormsg) == FAIL)
	{
	    যদি (errormsg != রিক্ত)
		emsg(errormsg);
	    ঘূৰাই;
	}
    }

    /*
     * Save and restore the existing top-level grouplist id and ":syn
     * include" tag around the actual inclusion.
     */
    যদি (running_syn_inc_tag >= MAX_SYN_INC_TAG)
    {
	emsg(_(e_too_many_syntax_includes));
	ঘূৰাই;
    }
    prev_syn_inc_tag = current_syn_inc_tag;
    current_syn_inc_tag = ++running_syn_inc_tag;
    prev_toplvl_grp = curwin->w_s->b_syn_topgrp;
    curwin->w_s->b_syn_topgrp = sgl_id;
    যদি (source ? do_source(eap->arg, FALSE, DOSO_NONE, রিক্ত) == FAIL
				: source_runtime(eap->arg, DIP_ALL) == FAIL)
	semsg(_(e_cant_open_file_str), eap->arg);
    curwin->w_s->b_syn_topgrp = prev_toplvl_grp;
    current_syn_inc_tag = prev_syn_inc_tag;
}

/*
 * Handle ":syntax keyword {group-name} [{option}] keyword .." command.
 */
    স্থির শূন্য
syn_cmd_keyword(exarg_T *eap, পূর্ণাঙ্ক syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*group_name_end;
    পূর্ণাঙ্ক		syn_id;
    char_u	*rest;
    char_u	*keyword_copy = রিক্ত;
    char_u	*p;
    char_u	*kw;
    syn_opt_arg_T syn_opt_arg;
    পূর্ণাঙ্ক		cnt;
    পূর্ণাঙ্ক		conceal_char = NUL;

    rest = get_group_name(arg, &group_name_end);

    যদি (rest != রিক্ত)
    {
	যদি (eap->skip)
	    syn_id = -1;
	অন্যথা
	    syn_id = syn_check_group(arg, (পূর্ণাঙ্ক)(group_name_end - arg));
	যদি (syn_id != 0)
	    // allocate a buffer, for removing backslashes in the keyword
	    keyword_copy = alloc(STRLEN(rest) + 1);
	যদি (keyword_copy != রিক্ত)
	{
	    syn_opt_arg.flags = 0;
	    syn_opt_arg.keyword = TRUE;
	    syn_opt_arg.sync_idx = রিক্ত;
	    syn_opt_arg.has_cont_list = FALSE;
	    syn_opt_arg.cont_in_list = রিক্ত;
	    syn_opt_arg.next_list = রিক্ত;

	    /*
	     * The options given apply to ALL keywords, so all options must be
	     * found before keywords can be created.
	     * 1: collect the options and copy the keywords to keyword_copy.
	     */
	    cnt = 0;
	    p = keyword_copy;
	    বাবে ( ; rest != রিক্ত && !ends_excmd2(eap->arg, rest);
							rest = skipwhite(rest))
	    {
		rest = get_syn_options(rest, &syn_opt_arg, &conceal_char,
								    eap->skip);
		যদি (rest == রিক্ত || ends_excmd2(eap->arg, rest))
		    ভঙ্গ;
		// Copy the keyword, removing backslashes, and add a NUL.
		যেতিয়ালৈ (*rest != NUL && !VIM_ISWHITE(*rest))
		{
		    যদি (*rest == '\\' && rest[1] != NUL)
			++rest;
		    *p++ = *rest++;
		}
		*p++ = NUL;
		++cnt;
	    }

	    যদি (!eap->skip)
	    {
		size_t	kwlen = 0;

		// Adjust flags for use of ":syn include".
		syn_incl_toplevel(syn_id, &syn_opt_arg.flags);

		/*
		 * 2: Add an entry for each keyword.
		 */
		বাবে (kw = keyword_copy; --cnt >= 0; kw += kwlen + 1)
		{
		    বাবে (p = vim_strchr(kw, '['); ; )
		    {
			যদি (p == রিক্ত)
			    kwlen = STRLEN(kw);
			অন্যথা
			{
			    *p = NUL;
			    kwlen = (size_t)(p - kw);
			}
			add_keyword(kw, kwlen, syn_id, syn_opt_arg.flags,
				syn_opt_arg.cont_in_list,
					 syn_opt_arg.next_list, conceal_char);
			যদি (p == রিক্ত)
			    ভঙ্গ;
			যদি (p[1] == NUL)
			{
			    semsg(_(e_error_missing_rsb_str), kw);
			    গচ্ছ error;
			}
			যদি (p[1] == ']')
			{
			    যদি (p[2] != NUL)
			    {
				semsg(_(e_trailing_char_after_rsb_str_str),
								    kw, &p[2]);
				গচ্ছ error;
			    }
			    kw = p + 1;		// skip over the "]"
			    kwlen = 1;
			    ভঙ্গ;
			}
			যদি (has_mbyte)
			{
			    পূর্ণাঙ্ক l = (*mb_ptr2len)(p + 1);

			    mch_memmove(p, p + 1, l);
			    p += l;
			}
			অন্যথা
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

    যদি (rest != রিক্ত)
	set_nextcmd(eap, rest);
    অন্যথা
	semsg(_(e_invalid_argument_str), arg);

    redraw_curbuf_later(UPD_SOME_VALID);
    syn_স্তূপ_free_all(curwin->w_s);		// Need to recompute all syntax.
}

/*
 * Handle ":syntax match {name} [{options}] {pattern} [{options}]".
 *
 * Also ":syntax sync match {name} [[grouphere | groupthere] {group-name}] .."
 */
    স্থির শূন্য
syn_cmd_match(
    exarg_T	*eap,
    পূর্ণাঙ্ক		syncing)	    // TRUE for ":syntax sync match .. "
{
    char_u	*arg = eap->arg;
    char_u	*group_name_end;
    char_u	*rest;
    synpat_T	item;		// the item found in the line
    পূর্ণাঙ্ক		syn_id;
    পূর্ণাঙ্ক		idx;
    syn_opt_arg_T syn_opt_arg;
    পূর্ণাঙ্ক		sync_idx = 0;
    পূর্ণাঙ্ক		conceal_char = NUL;
    পূর্ণাঙ্ক		orig_called_emsg = called_emsg;

    // Isolate the group name, check for validity
    rest = get_group_name(arg, &group_name_end);

    // Get options before the pattern
    syn_opt_arg.flags = 0;
    syn_opt_arg.keyword = FALSE;
    syn_opt_arg.sync_idx = syncing ? &sync_idx : রিক্ত;
    syn_opt_arg.has_cont_list = TRUE;
    syn_opt_arg.cont_list = রিক্ত;
    syn_opt_arg.cont_in_list = রিক্ত;
    syn_opt_arg.next_list = রিক্ত;
    rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);

    // get the pattern.
    init_syn_patterns();
    CLEAR_FIELD(item);
    rest = get_syn_pattern(rest, &item);
    যদি (vim_regcomp_had_eol() && !(syn_opt_arg.flags & HL_EXCLUDENL))
	syn_opt_arg.flags |= HL_HAS_EOL;

    // Get options after the pattern
    rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);

    যদি (rest != রিক্ত)		// all arguments are valid
    {
	/*
	 * Check for trailing command and illegal trailing arguments.
	 */
	set_nextcmd(eap, rest);
	যদি (!ends_excmd2(eap->cmd, rest) || eap->skip)
	    rest = রিক্ত;
	অন্যথা যদি (ga_grow(&curwin->w_s->b_syn_patterns, 1) == OK
		&& (syn_id = syn_check_group(arg,
					   (পূর্ণাঙ্ক)(group_name_end - arg))) != 0)
	{
	    syn_incl_toplevel(syn_id, &syn_opt_arg.flags);
	    /*
	     * Store the pattern in the syn_items list
	     */
	    idx = curwin->w_s->b_syn_patterns.ga_len;
	    SYN_বস্তুসমূহ(curwin->w_s)[idx] = item;
	    SYN_বস্তুসমূহ(curwin->w_s)[idx].sp_syncing = syncing;
	    SYN_বস্তুসমূহ(curwin->w_s)[idx].sp_type = SPTYPE_MATCH;
	    SYN_বস্তুসমূহ(curwin->w_s)[idx].sp_syn.id = syn_id;
	    SYN_বস্তুসমূহ(curwin->w_s)[idx].sp_syn.inc_tag = current_syn_inc_tag;
	    SYN_বস্তুসমূহ(curwin->w_s)[idx].sp_flags = syn_opt_arg.flags;
	    SYN_বস্তুসমূহ(curwin->w_s)[idx].sp_sync_idx = sync_idx;
	    SYN_বস্তুসমূহ(curwin->w_s)[idx].sp_cont_list = syn_opt_arg.cont_list;
	    SYN_বস্তুসমূহ(curwin->w_s)[idx].sp_syn.cont_in_list =
						     syn_opt_arg.cont_in_list;
#ifdef FEAT_CONCEAL
	    SYN_বস্তুসমূহ(curwin->w_s)[idx].sp_cchar = conceal_char;
#endif
	    যদি (syn_opt_arg.cont_in_list != রিক্ত)
		curwin->w_s->b_syn_containedin = TRUE;
	    SYN_বস্তুসমূহ(curwin->w_s)[idx].sp_next_list = syn_opt_arg.next_list;
	    ++curwin->w_s->b_syn_patterns.ga_len;

	    // remember that we found a match for syncing on
	    যদি (syn_opt_arg.flags & (HL_SYNC_HERE|HL_SYNC_THERE))
		curwin->w_s->b_syn_sync_flags |= SF_MATCH;
#ifdef FEAT_FOLDING
	    যদি (syn_opt_arg.flags & HL_FOLD)
		++curwin->w_s->b_syn_folditems;
#endif

	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_স্তূপ_free_all(curwin->w_s);	// Need to recompute all syntax.
	    ঘূৰাই;	// don't free the progs and patterns now
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

    যদি (rest == রিক্ত && called_emsg == orig_called_emsg)
	semsg(_(e_invalid_argument_str), arg);
}

/*
 * Handle ":syntax region {group-name} [matchgroup={group-name}]
 *		start {start} .. [skip {skip}] end {end} .. [{options}]".
 */
    স্থির শূন্য
syn_cmd_region(
    exarg_T	*eap,
    পূর্ণাঙ্ক		syncing)	    // TRUE for ":syntax sync region .."
{
    char_u		*arg = eap->arg;
    char_u		*group_name_end;
    char_u		*rest;			// next arg, NULL on error
    char_u		*চাবি_end;
    char_u		*চাবি = রিক্ত;
    char_u		*p;
    পূর্ণাঙ্ক			item;
#define ITEM_START	    0
#define ITEM_SKIP	    1
#define ITEM_END	    2
#define ITEM_MATCHGROUP	    3
    সংরচনা pat_ptr
    {
	synpat_T	*pp_synp;		// pointer to syn_pattern
	পূর্ণাঙ্ক		pp_matchgroup_id;	// matchgroup ID
	সংরচনা pat_ptr	*pp_next;		// pointer to next pat_ptr
    }			*(pat_ptrs[3]);
					// patterns found in the line
    সংরচনা pat_ptr	*ppp;
    সংরচনা pat_ptr	*ppp_next;
    পূর্ণাঙ্ক			pat_গুনতি = 0;		// nr of syn_patterns found
    পূর্ণাঙ্ক			syn_id;
    পূর্ণাঙ্ক			matchgroup_id = 0;
    পূর্ণাঙ্ক			not_enough = FALSE;	// not enough arguments
    পূর্ণাঙ্ক			illegal = FALSE;	// illegal arguments
    পূর্ণাঙ্ক			success = FALSE;
    পূর্ণাঙ্ক			idx;
    syn_opt_arg_T	syn_opt_arg;
    পূর্ণাঙ্ক			conceal_char = NUL;

    // Isolate the group name, check for validity
    rest = get_group_name(arg, &group_name_end);

    pat_ptrs[0] = রিক্ত;
    pat_ptrs[1] = রিক্ত;
    pat_ptrs[2] = রিক্ত;

    init_syn_patterns();

    syn_opt_arg.flags = 0;
    syn_opt_arg.keyword = FALSE;
    syn_opt_arg.sync_idx = রিক্ত;
    syn_opt_arg.has_cont_list = TRUE;
    syn_opt_arg.cont_list = রিক্ত;
    syn_opt_arg.cont_in_list = রিক্ত;
    syn_opt_arg.next_list = রিক্ত;

    /*
     * get the options, patterns and matchgroup.
     */
    যেতিয়ালৈ (rest != রিক্ত && !ends_excmd2(eap->cmd, rest))
    {
	// Check for option arguments
	rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);
	যদি (rest == রিক্ত || ends_excmd2(eap->cmd, rest))
	    ভঙ্গ;

	// must be a pattern or matchgroup then
	চাবি_end = rest;
	যেতিয়ালৈ (*চাবি_end && !VIM_ISWHITE(*চাবি_end) && *চাবি_end != '=')
	    ++চাবি_end;
	vim_free(চাবি);
	চাবি = vim_strnsave_up(rest, চাবি_end - rest);
	যদি (চাবি == রিক্ত)			// out of memory
	{
	    rest = রিক্ত;
	    ভঙ্গ;
	}
	যদি (STRCMP(চাবি, "MATCHGROUP") == 0)
	    item = ITEM_MATCHGROUP;
	অন্যথা যদি (STRCMP(চাবি, "START") == 0)
	    item = ITEM_START;
	অন্যথা যদি (STRCMP(চাবি, "END") == 0)
	    item = ITEM_END;
	অন্যথা যদি (STRCMP(চাবি, "SKIP") == 0)
	{
	    যদি (pat_ptrs[ITEM_SKIP] != রিক্ত)	// one skip pattern allowed
	    {
		illegal = TRUE;
		ভঙ্গ;
	    }
	    item = ITEM_SKIP;
	}
	অন্যথা
	    ভঙ্গ;
	rest = skipwhite(চাবি_end);
	যদি (*rest != '=')
	{
	    rest = রিক্ত;
	    semsg(_(e_missing_equal_str), arg);
	    ভঙ্গ;
	}
	rest = skipwhite(rest + 1);
	যদি (*rest == NUL)
	{
	    not_enough = TRUE;
	    ভঙ্গ;
	}

	যদি (item == ITEM_MATCHGROUP)
	{
	    p = skiptowhite(rest);
	    যদি ((p - rest == 4 && STRNCMP(rest, "NONE", 4) == 0) || eap->skip)
		matchgroup_id = 0;
	    অন্যথা
	    {
		matchgroup_id = syn_check_group(rest, (পূর্ণাঙ্ক)(p - rest));
		যদি (matchgroup_id == 0)
		{
		    illegal = TRUE;
		    ভঙ্গ;
		}
	    }
	    rest = skipwhite(p);
	}
	অন্যথা
	{
	    /*
	     * Allocate room for a syn_pattern, and link it in the list of
	     * syn_patterns for this item, at the start (because the list is
	     * used from end to start).
	     */
	    ppp = ALLOC_ONE(সংরচনা pat_ptr);
	    যদি (ppp == রিক্ত)
	    {
		rest = রিক্ত;
		ভঙ্গ;
	    }
	    ppp->pp_next = pat_ptrs[item];
	    pat_ptrs[item] = ppp;
	    ppp->pp_synp = ALLOC_CLEAR_ONE(synpat_T);
	    যদি (ppp->pp_synp == রিক্ত)
	    {
		rest = রিক্ত;
		ভঙ্গ;
	    }

	    /*
	     * Get the syntax pattern and the following offset(s).
	     */
	    // Enable the appropriate \z specials.
	    যদি (item == ITEM_START)
		reg_do_extmatch = REX_SET;
	    অন্যথা যদি (item == ITEM_SKIP || item == ITEM_END)
		reg_do_extmatch = REX_USE;
	    rest = get_syn_pattern(rest, ppp->pp_synp);
	    reg_do_extmatch = 0;
	    যদি (item == ITEM_END && vim_regcomp_had_eol()
				       && !(syn_opt_arg.flags & HL_EXCLUDENL))
		ppp->pp_synp->sp_flags |= HL_HAS_EOL;
	    ppp->pp_matchgroup_id = matchgroup_id;
	    ++pat_গুনতি;
	}
    }
    vim_free(চাবি);
    যদি (illegal || not_enough)
	rest = রিক্ত;

    /*
     * Must have a "start" and "end" pattern.
     */
    যদি (rest != রিক্ত && (pat_ptrs[ITEM_START] == রিক্ত ||
						  pat_ptrs[ITEM_END] == রিক্ত))
    {
	not_enough = TRUE;
	rest = রিক্ত;
    }

    যদি (rest != রিক্ত)
    {
	/*
	 * Check for trailing garbage or command.
	 * If OK, add the item.
	 */
	set_nextcmd(eap, rest);
	যদি (!ends_excmd(*rest) || eap->skip)
	    rest = রিক্ত;
	অন্যথা যদি (ga_grow(&(curwin->w_s->b_syn_patterns), pat_গুনতি) == OK
		&& (syn_id = syn_check_group(arg,
					   (পূর্ণাঙ্ক)(group_name_end - arg))) != 0)
	{
	    syn_incl_toplevel(syn_id, &syn_opt_arg.flags);
	    /*
	     * Store the start/skip/end in the syn_items list
	     */
	    idx = curwin->w_s->b_syn_patterns.ga_len;
	    বাবে (item = ITEM_START; item <= ITEM_END; ++item)
	    {
		বাবে (ppp = pat_ptrs[item]; ppp != রিক্ত; ppp = ppp->pp_next)
		{
		    SYN_বস্তুসমূহ(curwin->w_s)[idx] = *(ppp->pp_synp);
		    SYN_বস্তুসমূহ(curwin->w_s)[idx].sp_syncing = syncing;
		    SYN_বস্তুসমূহ(curwin->w_s)[idx].sp_type =
			    (item == ITEM_START) ? SPTYPE_START :
			    (item == ITEM_SKIP) ? SPTYPE_SKIP : SPTYPE_END;
		    SYN_বস্তুসমূহ(curwin->w_s)[idx].sp_flags |= syn_opt_arg.flags;
		    SYN_বস্তুসমূহ(curwin->w_s)[idx].sp_syn.id = syn_id;
		    SYN_বস্তুসমূহ(curwin->w_s)[idx].sp_syn.inc_tag =
							  current_syn_inc_tag;
		    SYN_বস্তুসমূহ(curwin->w_s)[idx].sp_syn_match_id =
							ppp->pp_matchgroup_id;
#ifdef FEAT_CONCEAL
		    SYN_বস্তুসমূহ(curwin->w_s)[idx].sp_cchar = conceal_char;
#endif
		    যদি (item == ITEM_START)
		    {
			SYN_বস্তুসমূহ(curwin->w_s)[idx].sp_cont_list =
							syn_opt_arg.cont_list;
			SYN_বস্তুসমূহ(curwin->w_s)[idx].sp_syn.cont_in_list =
						     syn_opt_arg.cont_in_list;
			যদি (syn_opt_arg.cont_in_list != রিক্ত)
			    curwin->w_s->b_syn_containedin = TRUE;
			SYN_বস্তুসমূহ(curwin->w_s)[idx].sp_next_list =
							syn_opt_arg.next_list;
		    }
		    ++curwin->w_s->b_syn_patterns.ga_len;
		    ++idx;
#ifdef FEAT_FOLDING
		    যদি (syn_opt_arg.flags & HL_FOLD)
			++curwin->w_s->b_syn_folditems;
#endif
		}
	    }

	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_স্তূপ_free_all(curwin->w_s);	// Need to recompute all syntax.
	    success = TRUE;	    // don't free the progs and patterns now
	}
    }

    /*
     * Free the allocated memory.
     */
    বাবে (item = ITEM_START; item <= ITEM_END; ++item)
	বাবে (ppp = pat_ptrs[item]; ppp != রিক্ত; ppp = ppp_next)
	{
	    যদি (!success && ppp->pp_synp != রিক্ত)
	    {
		vim_regfree(ppp->pp_synp->sp_prog);
		vim_free(ppp->pp_synp->sp_pattern);
	    }
	    vim_free(ppp->pp_synp);
	    ppp_next = ppp->pp_next;
	    vim_free(ppp);
	}

    যদি (!success)
    {
	vim_free(syn_opt_arg.cont_list);
	vim_free(syn_opt_arg.cont_in_list);
	vim_free(syn_opt_arg.next_list);
	যদি (not_enough)
	    semsg(_(e_not_enough_arguments_syntax_region_str), arg);
	অন্যথা যদি (illegal || rest == রিক্ত)
	    semsg(_(e_invalid_argument_str), arg);
    }
}

/*
 * A simple syntax group ID comparison function suitable for use in qsort()
 */
    স্থির পূর্ণাঙ্ক
syn_compare_stub(ধ্রুৱ শূন্য *v1, ধ্রুৱ শূন্য *v2)
{
    ধ্রুৱ short	*s1 = v1;
    ধ্রুৱ short	*s2 = v2;

    ঘূৰাই (*s1 > *s2 ? 1 : *s1 < *s2 ? -1 : 0);
}

/*
 * Combines lists of syntax clusters.
 * *clstr1 and *clstr2 must both be allocated memory; they will be consumed.
 */
    স্থির শূন্য
syn_combine_list(short **clstr1, short **clstr2, পূর্ণাঙ্ক list_op)
{
    পূর্ণাঙ্ক		count1 = 0;
    পূর্ণাঙ্ক		count2 = 0;
    short	*g1;
    short	*g2;
    short	*clstr = রিক্ত;
    পূর্ণাঙ্ক		গুনতি;
    পূর্ণাঙ্ক		round;

    /*
     * Handle degenerate cases.
     */
    যদি (*clstr2 == রিক্ত)
	ঘূৰাই;
    যদি (*clstr1 == রিক্ত || list_op == CLUSTER_REPLACE)
    {
	যদি (list_op == CLUSTER_REPLACE)
	    vim_free(*clstr1);
	যদি (list_op == CLUSTER_REPLACE || list_op == CLUSTER_ADD)
	    *clstr1 = *clstr2;
	অন্যথা
	    vim_free(*clstr2);
	ঘূৰাই;
    }

    বাবে (g1 = *clstr1; *g1; g1++)
	++count1;
    বাবে (g2 = *clstr2; *g2; g2++)
	++count2;

    /*
     * For speed purposes, sort both lists.
     */
    qsort(*clstr1, (size_t)count1, পরিমাণ(short), syn_compare_stub);
    qsort(*clstr2, (size_t)count2, পরিমাণ(short), syn_compare_stub);

    /*
     * We proceed in two passes; in round 1, we count the elements to place
     * in the new list, and in round 2, we allocate and populate the new
     * list.  For speed, we use a mergesort-like method, adding the smaller
     * of the current elements in each list to the new list.
     */
    বাবে (round = 1; round <= 2; round++)
    {
	g1 = *clstr1;
	g2 = *clstr2;
	গুনতি = 0;

	/*
	 * First, loop through the lists until one of them is empty.
	 */
	যেতিয়ালৈ (*g1 && *g2)
	{
	    /*
	     * We always want to add from the first list.
	     */
	    যদি (*g1 < *g2)
	    {
		যদি (round == 2)
		    clstr[গুনতি] = *g1;
		গুনতি++;
		g1++;
		অনুৱর্ত;
	    }
	    /*
	     * We only want to add from the second list if we're adding the
	     * lists.
	     */
	    যদি (list_op == CLUSTER_ADD)
	    {
		যদি (round == 2)
		    clstr[গুনতি] = *g2;
		গুনতি++;
	    }
	    যদি (*g1 == *g2)
		g1++;
	    g2++;
	}

	/*
	 * Now add the leftovers from whichever list didn't get finished
	 * first.  As before, we only want to add from the second list if
	 * we're adding the lists.
	 */
	বাবে (; *g1; g1++, গুনতি++)
	    যদি (round == 2)
		clstr[গুনতি] = *g1;
	যদি (list_op == CLUSTER_ADD)
	    বাবে (; *g2; g2++, গুনতি++)
		যদি (round == 2)
		    clstr[গুনতি] = *g2;

	যদি (round == 1)
	{
	    /*
	     * If the group ended up empty, we don't need to allocate any
	     * space for it.
	     */
	    যদি (গুনতি == 0)
	    {
		clstr = রিক্ত;
		ভঙ্গ;
	    }
	    clstr = ALLOC_MULT(short, গুনতি + 1);
	    যদি (clstr == রিক্ত)
		ভঙ্গ;
	    clstr[গুনতি] = 0;
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
    স্থির পূর্ণাঙ্ক
syn_scl_name2id(char_u *name)
{
    পূর্ণাঙ্ক		i;
    char_u	*name_u;

    // Avoid using stricmp() too much, it's slow on some systems
    name_u = vim_strsave_up(name);
    যদি (name_u == রিক্ত)
	ঘূৰাই 0;
    বাবে (i = curwin->w_s->b_syn_clusters.ga_len; --i >= 0; )
	যদি (SYN_CLSTR(curwin->w_s)[i].scl_name_u != রিক্ত
		&& STRCMP(name_u, SYN_CLSTR(curwin->w_s)[i].scl_name_u) == 0)
	    ভঙ্গ;
    vim_free(name_u);
    ঘূৰাই (i < 0 ? 0 : i + SYNID_CLUSTER);
}

/*
 * Like syn_scl_name2id(), but take a pointer + length argument.
 */
    স্থির পূর্ণাঙ্ক
syn_scl_namen2id(char_u *linep, পূর্ণাঙ্ক len)
{
    char_u  *name;
    পূর্ণাঙ্ক	    id = 0;

    name = vim_strnsave(linep, len);
    যদি (name == রিক্ত)
	ঘূৰাই 0;

    id = syn_scl_name2id(name);
    vim_free(name);
    ঘূৰাই id;
}

/*
 * Find syntax cluster name in the table and return its ID.
 * The argument is a pointer to the name and the length of the name.
 * If it doesn't exist yet, a new entry is created.
 * Return 0 for failure.
 */
    স্থির পূর্ণাঙ্ক
syn_check_cluster(char_u *pp, পূর্ণাঙ্ক len)
{
    পূর্ণাঙ্ক		id;
    char_u	*name;

    name = vim_strnsave(pp, len);
    যদি (name == রিক্ত)
	ঘূৰাই 0;

    id = syn_scl_name2id(name);
    যদি (id == 0)			// doesn't exist yet
	id = syn_add_cluster(name);
    অন্যথা
	vim_free(name);
    ঘূৰাই id;
}

/*
 * Add new syntax cluster and return its ID.
 * "name" must be an allocated string, it will be consumed.
 * Return 0 for failure.
 */
    স্থির পূর্ণাঙ্ক
syn_add_cluster(char_u *name)
{
    পূর্ণাঙ্ক		len;

    /*
     * First call for this growarray: init growing array.
     */
    যদি (curwin->w_s->b_syn_clusters.ga_data == রিক্ত)
    {
	curwin->w_s->b_syn_clusters.ga_itemsize = পরিমাণ(syn_cluster_T);
	curwin->w_s->b_syn_clusters.ga_growsize = 10;
    }

    len = curwin->w_s->b_syn_clusters.ga_len;
    যদি (len >= MAX_CLUSTER_ID)
    {
	emsg(_(e_too_many_syntax_clusters));
	vim_free(name);
	ঘূৰাই 0;
    }

    /*
     * Make room for at least one other cluster entry.
     */
    যদি (ga_grow(&curwin->w_s->b_syn_clusters, 1) == FAIL)
    {
	vim_free(name);
	ঘূৰাই 0;
    }

    CLEAR_POINTER(&(SYN_CLSTR(curwin->w_s)[len]));
    SYN_CLSTR(curwin->w_s)[len].scl_name = name;
    SYN_CLSTR(curwin->w_s)[len].scl_name_u = vim_strsave_up(name);
    SYN_CLSTR(curwin->w_s)[len].scl_list = রিক্ত;
    ++curwin->w_s->b_syn_clusters.ga_len;

    যদি (STRICMP(name, "Spell") == 0)
	curwin->w_s->b_spell_cluster_id = len + SYNID_CLUSTER;
    যদি (STRICMP(name, "NoSpell") == 0)
	curwin->w_s->b_nospell_cluster_id = len + SYNID_CLUSTER;

    ঘূৰাই len + SYNID_CLUSTER;
}

/*
 * Handle ":syntax cluster {cluster-name} [contains={groupname},..]
 *		[add={groupname},..] [remove={groupname},..]".
 */
    স্থির শূন্য
syn_cmd_cluster(exarg_T *eap, পূর্ণাঙ্ক syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*group_name_end;
    char_u	*rest;
    পূর্ণাঙ্ক		scl_id;
    short	*clstr_list;
    পূর্ণাঙ্ক		got_clstr = FALSE;
    পূর্ণাঙ্ক		opt_len;
    পূর্ণাঙ্ক		list_op;

    eap->nextcmd = find_nextcmd(arg);
    যদি (eap->skip)
	ঘূৰাই;

    rest = get_group_name(arg, &group_name_end);

    যদি (rest != রিক্ত)
    {
	scl_id = syn_check_cluster(arg, (পূর্ণাঙ্ক)(group_name_end - arg));
	যদি (scl_id == 0)
	    ঘূৰাই;
	scl_id -= SYNID_CLUSTER;

	বাবে (;;)
	{
	    যদি (STRNICMP(rest, "add", 3) == 0
		    && (VIM_ISWHITE(rest[3]) || rest[3] == '='))
	    {
		opt_len = 3;
		list_op = CLUSTER_ADD;
	    }
	    অন্যথা যদি (STRNICMP(rest, "remove", 6) == 0
		    && (VIM_ISWHITE(rest[6]) || rest[6] == '='))
	    {
		opt_len = 6;
		list_op = CLUSTER_SUBTRACT;
	    }
	    অন্যথা যদি (STRNICMP(rest, "contains", 8) == 0
			&& (VIM_ISWHITE(rest[8]) || rest[8] == '='))
	    {
		opt_len = 8;
		list_op = CLUSTER_REPLACE;
	    }
	    অন্যথা
		ভঙ্গ;

	    clstr_list = রিক্ত;
	    যদি (get_id_list(&rest, opt_len, &clstr_list, eap->skip) == FAIL)
	    {
		semsg(_(e_invalid_argument_str), rest);
		ভঙ্গ;
	    }
	    যদি (scl_id >= 0)
		syn_combine_list(&SYN_CLSTR(curwin->w_s)[scl_id].scl_list,
			     &clstr_list, list_op);
	    অন্যথা
		vim_free(clstr_list);
	    got_clstr = TRUE;
	}

	যদি (got_clstr)
	{
	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_স্তূপ_free_all(curwin->w_s);	// Need to recompute all.
	}
    }

    যদি (!got_clstr)
	emsg(_(e_no_cluster_specified));
    যদি (rest == রিক্ত || !ends_excmd2(eap->cmd, rest))
	semsg(_(e_invalid_argument_str), arg);
}

/*
 * On first call for current buffer: Init growing array.
 */
    স্থির শূন্য
init_syn_patterns(শূন্য)
{
    curwin->w_s->b_syn_patterns.ga_itemsize = পরিমাণ(synpat_T);
    curwin->w_s->b_syn_patterns.ga_growsize = 10;
}

/*
 * Get one pattern for a ":syntax match" or ":syntax region" command.
 * Stores the pattern and program in a synpat_T.
 * Returns a pointer to the next argument, or NULL in case of an error.
 */
    স্থির char_u *
get_syn_pattern(char_u *arg, synpat_T *ci)
{
    char_u	*end;
    পূর্ণাঙ্ক		*p;
    পূর্ণাঙ্ক		idx;
    char_u	*cpo_save;

    // need at least three chars
    যদি (arg == রিক্ত || arg[0] == NUL || arg[1] == NUL || arg[2] == NUL)
	ঘূৰাই রিক্ত;

    end = skip_regexp(arg + 1, *arg, TRUE);
    যদি (*end != *arg)			    // end delimiter not found
    {
	semsg(_(e_pattern_delimiter_not_found_str), arg);
	ঘূৰাই রিক্ত;
    }
    // store the pattern and compiled regexp program
    যদি ((ci->sp_pattern = vim_strnsave(arg + 1, end - arg - 1)) == রিক্ত)
	ঘূৰাই রিক্ত;

    // Make 'cpoptions' empty, to avoid the 'l' flag
    cpo_save = p_cpo;
    p_cpo = empty_option;
    ci->sp_prog = vim_regcomp(ci->sp_pattern, RE_MAGIC);
    p_cpo = cpo_save;

    যদি (ci->sp_prog == রিক্ত)
	ঘূৰাই রিক্ত;
    ci->sp_ic = curwin->w_s->b_syn_ic;
#ifdef FEAT_PROFILE
    syn_clear_time(&ci->sp_time);
#endif

    /*
     * Check for a match, highlight or region offset.
     */
    ++end;
    কৰা
    {
	বাবে (idx = SPO_গুনতি; --idx >= 0; )
	    যদি (STRNCMP(end, spo_name_tab[idx], 3) == 0)
		ভঙ্গ;
	যদি (idx >= 0)
	{
	    p = &(ci->sp_offsets[idx]);
	    যদি (idx != SPO_LC_OFF)
		ৱিকল্প (end[3])
		{
		    পক্ষ 's':   ভঙ্গ;
		    পক্ষ 'b':   ভঙ্গ;
		    পক্ষ 'e':   idx += SPO_গুনতি; ভঙ্গ;
		    স্ৱভাৱ:    idx = -1; ভঙ্গ;
		}
	    যদি (idx >= 0)
	    {
		ci->sp_off_flags |= (1 << idx);
		যদি (idx == SPO_LC_OFF)	    // lc=99
		{
		    end += 3;
		    *p = getdigits(&end);

		    // "lc=" offset automatically sets "ms=" offset
		    যদি (!(ci->sp_off_flags & (1 << SPO_MS_OFF)))
		    {
			ci->sp_off_flags |= (1 << SPO_MS_OFF);
			ci->sp_offsets[SPO_MS_OFF] = *p;
		    }
		}
		অন্যথা			    // yy=x+99
		{
		    end += 4;
		    যদি (*end == '+')
		    {
			++end;
			*p = getdigits(&end);		// positive offset
		    }
		    অন্যথা যদি (*end == '-')
		    {
			++end;
			*p = -getdigits(&end);		// negative offset
		    }
		}
		যদি (*end != ',')
		    ভঙ্গ;
		++end;
	    }
	}
    } যেতিয়ালৈ (idx >= 0);

    যদি (!ends_excmd2(arg, end) && !VIM_ISWHITE(*end))
    {
	semsg(_(e_garbage_after_pattern_str), arg);
	ঘূৰাই রিক্ত;
    }
    ঘূৰাই skipwhite(end);
}

/*
 * Handle ":syntax sync .." command.
 */
    স্থির শূন্য
syn_cmd_sync(exarg_T *eap, পূর্ণাঙ্ক syncing UNUSED)
{
    char_u	*arg_start = eap->arg;
    char_u	*arg_end;
    char_u	*চাবি = রিক্ত;
    char_u	*next_arg;
    পূর্ণাঙ্ক		illegal = FALSE;
    পূর্ণাঙ্ক		finished = FALSE;
    long	n;
    char_u	*cpo_save;

    যদি (ends_excmd2(eap->cmd, arg_start))
    {
	syn_cmd_list(eap, TRUE);
	ঘূৰাই;
    }

    যেতিয়ালৈ (!ends_excmd2(eap->cmd, arg_start))
    {
	arg_end = skiptowhite(arg_start);
	next_arg = skipwhite(arg_end);
	vim_free(চাবি);
	চাবি = vim_strnsave_up(arg_start, arg_end - arg_start);
	যদি (চাবি == রিক্ত)
	    ভঙ্গ;
	যদি (STRCMP(চাবি, "CCOMMENT") == 0)
	{
	    যদি (!eap->skip)
		curwin->w_s->b_syn_sync_flags |= SF_CCOMMENT;
	    যদি (!ends_excmd2(eap->cmd, next_arg))
	    {
		arg_end = skiptowhite(next_arg);
		যদি (!eap->skip)
		    curwin->w_s->b_syn_sync_id = syn_check_group(next_arg,
						   (পূর্ণাঙ্ক)(arg_end - next_arg));
		next_arg = skipwhite(arg_end);
	    }
	    অন্যথা যদি (!eap->skip)
		curwin->w_s->b_syn_sync_id = syn_name2id((char_u *)"Comment");
	}
	অন্যথা যদি (  STRNCMP(চাবি, "LINES", 5) == 0
		|| STRNCMP(চাবি, "MINLINES", 8) == 0
		|| STRNCMP(চাবি, "MAXLINES", 8) == 0
		|| STRNCMP(চাবি, "LINEBREAKS", 10) == 0)
	{
	    যদি (চাবি[4] == 'S')
		arg_end = চাবি + 6;
	    অন্যথা যদি (চাবি[0] == 'L')
		arg_end = চাবি + 11;
	    অন্যথা
		arg_end = চাবি + 9;
	    যদি (arg_end[-1] != '=' || !VIM_ISDIGIT(*arg_end))
	    {
		illegal = TRUE;
		ভঙ্গ;
	    }
	    n = getdigits(&arg_end);
	    যদি (!eap->skip)
	    {
		যদি (চাবি[4] == 'B')
		    curwin->w_s->b_syn_sync_linebreaks = n;
		অন্যথা যদি (চাবি[1] == 'A')
		    curwin->w_s->b_syn_sync_maxlines = n;
		অন্যথা
		    curwin->w_s->b_syn_sync_minlines = n;
	    }
	}
	অন্যথা যদি (STRCMP(চাবি, "FROMSTART") == 0)
	{
	    যদি (!eap->skip)
	    {
		curwin->w_s->b_syn_sync_minlines = MAXLNUM;
		curwin->w_s->b_syn_sync_maxlines = 0;
	    }
	}
	অন্যথা যদি (STRCMP(চাবি, "LINECONT") == 0)
	{
	    যদি (*next_arg == NUL)	   // missing pattern
	    {
		illegal = TRUE;
		ভঙ্গ;
	    }
	    যদি (curwin->w_s->b_syn_linecont_pat != রিক্ত)
	    {
		emsg(_(e_syntax_sync_সাৰি_continuations_pattern_specified_twice));
		finished = TRUE;
		ভঙ্গ;
	    }
	    arg_end = skip_regexp(next_arg + 1, *next_arg, TRUE);
	    যদি (*arg_end != *next_arg)	    // end delimiter not found
	    {
		illegal = TRUE;
		ভঙ্গ;
	    }

	    যদি (!eap->skip)
	    {
		// store the pattern and compiled regexp program
		যদি ((curwin->w_s->b_syn_linecont_pat =
			    vim_strnsave(next_arg + 1,
				      arg_end - next_arg - 1)) == রিক্ত)
		{
		    finished = TRUE;
		    ভঙ্গ;
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

		যদি (curwin->w_s->b_syn_linecont_prog == রিক্ত)
		{
		    VIM_CLEAR(curwin->w_s->b_syn_linecont_pat);
		    finished = TRUE;
		    ভঙ্গ;
		}
	    }
	    next_arg = skipwhite(arg_end + 1);
	}
	অন্যথা
	{
	    eap->arg = next_arg;
	    যদি (STRCMP(চাবি, "MATCH") == 0)
		syn_cmd_match(eap, TRUE);
	    অন্যথা যদি (STRCMP(চাবি, "REGION") == 0)
		syn_cmd_region(eap, TRUE);
	    অন্যথা যদি (STRCMP(চাবি, "CLEAR") == 0)
		syn_cmd_clear(eap, TRUE);
	    অন্যথা
		illegal = TRUE;
	    finished = TRUE;
	    ভঙ্গ;
	}
	arg_start = next_arg;
    }
    vim_free(চাবি);
    যদি (illegal)
	semsg(_(e_illegal_arguments_str), arg_start);
    অন্যথা যদি (!finished)
    {
	set_nextcmd(eap, arg_start);
	redraw_curbuf_later(UPD_SOME_VALID);
	syn_স্তূপ_free_all(curwin->w_s);	// Need to recompute all syntax.
    }
}

/*
 * Convert a line of highlight group names into a list of group ID numbers.
 * "arg" should point to the "contains" or "nextgroup" keyword.
 * "arg" is advanced to after the last group name.
 * Careful: the argument is modified (NULs added).
 * returns FAIL for some error, OK for success.
 */
    স্থির পূর্ণাঙ্ক
get_id_list(
    char_u	**arg,
    পূর্ণাঙ্ক		keylen,		// length of keyword
    short	**list,		// where to store the resulting list, if not
				// NULL, the list is silently skipped!
    পূর্ণাঙ্ক		skip)
{
    char_u	*p = রিক্ত;
    char_u	*end;
    পূর্ণাঙ্ক		round;
    পূর্ণাঙ্ক		গুনতি;
    পূর্ণাঙ্ক		total_গুনতি = 0;
    short	*retval = রিক্ত;
    char_u	*name;
    regmatch_T	regmatch;
    পূর্ণাঙ্ক		id;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		failed = FALSE;

    /*
     * We parse the list twice:
     * round == 1: count the number of items, allocate the array.
     * round == 2: fill the array with the items.
     * In round 1 new groups may be added, causing the number of items to
     * grow when a regexp is used.  In that case round 1 is done once again.
     */
    বাবে (round = 1; round <= 2; ++round)
    {
	/*
	 * skip "contains"
	 */
	p = skipwhite(*arg + keylen);
	যদি (*p != '=')
	{
	    semsg(_(e_missing_equal_sign_str), *arg);
	    ভঙ্গ;
	}
	p = skipwhite(p + 1);
	যদি (ends_excmd2(*arg, p))
	{
	    semsg(_(e_empty_argument_str), *arg);
	    ভঙ্গ;
	}

	/*
	 * parse the arguments after "contains"
	 */
	গুনতি = 0;
	যেতিয়ালৈ (!ends_excmd2(*arg, p))
	{
	    বাবে (end = p; *end && !VIM_ISWHITE(*end) && *end != ','; ++end)
		;
	    name = alloc(end - p + 3);	    // leave room for "^$"
	    যদি (name == রিক্ত)
	    {
		failed = TRUE;
		ভঙ্গ;
	    }
	    vim_strncpy(name + 1, p, end - p);
	    যদি (       STRCMP(name + 1, "ALLBUT") == 0
		    || STRCMP(name + 1, "ALL") == 0
		    || STRCMP(name + 1, "TOP") == 0
		    || STRCMP(name + 1, "CONTAINED") == 0)
	    {
		যদি (TOUPPER_ASC(**arg) != 'C')
		{
		    semsg(_(e_str_not_allowed_here), name + 1);
		    failed = TRUE;
		    vim_free(name);
		    ভঙ্গ;
		}
		যদি (গুনতি != 0)
		{
		    semsg(_(e_str_must_be_first_in_contains_list), name + 1);
		    failed = TRUE;
		    vim_free(name);
		    ভঙ্গ;
		}
		যদি (name[1] == 'A')
		    id = SYNID_ALLBUT;
		অন্যথা যদি (name[1] == 'T')
		    id = SYNID_শীৰ্ষ;
		অন্যথা
		    id = SYNID_CONTAINED;
		id += current_syn_inc_tag;
	    }
	    অন্যথা যদি (name[1] == '@')
	    {
		যদি (skip)
		    id = -1;
		অন্যথা
		    id = syn_check_cluster(name + 2, (পূর্ণাঙ্ক)(end - p - 1));
	    }
	    অন্যথা
	    {
		/*
		 * Handle full group name.
		 */
		যদি (vim_strpbrk(name + 1, (char_u *)"\\.*^$~[") == রিক্ত)
		    id = syn_check_group(name + 1, (পূর্ণাঙ্ক)(end - p));
		অন্যথা
		{
		    /*
		     * Handle match of regexp with group names.
		     */
		    *name = '^';
		    STRCAT(name, "$");
		    regmatch.regprog = vim_regcomp(name, RE_MAGIC);
		    যদি (regmatch.regprog == রিক্ত)
		    {
			failed = TRUE;
			vim_free(name);
			ভঙ্গ;
		    }

		    regmatch.rm_ic = TRUE;
		    id = 0;
		    বাবে (i = highlight_অঙ্ক_groups(); --i >= 0; )
		    {
			যদি (vim_regexec(&regmatch, highlight_group_name(i),
								  (colnr_T)0))
			{
			    যদি (round == 2)
			    {
				// Got more items than expected; can happen
				// when adding items that match:
				// "contains=a.*b,axb".
				// Go back to first round
				যদি (গুনতি >= total_গুনতি)
				{
				    vim_free(retval);
				    round = 1;
				}
				অন্যথা
				    retval[গুনতি] = i + 1;
			    }
			    ++গুনতি;
			    id = -1;	    // remember that we found one
			}
		    }
		    vim_regfree(regmatch.regprog);
		}
	    }
	    vim_free(name);
	    যদি (id == 0)
	    {
		semsg(_(e_unknown_group_name_str), p);
		failed = TRUE;
		ভঙ্গ;
	    }
	    যদি (id > 0)
	    {
		যদি (round == 2)
		{
		    // Got more items than expected, go back to first round
		    যদি (গুনতি >= total_গুনতি)
		    {
			vim_free(retval);
			round = 1;
		    }
		    অন্যথা
			retval[গুনতি] = id;
		}
		++গুনতি;
	    }
	    p = skipwhite(end);
	    যদি (*p != ',')
		ভঙ্গ;
	    p = skipwhite(p + 1);	// skip comma in between arguments
	}
	যদি (failed)
	    ভঙ্গ;
	যদি (round == 1)
	{
	    retval = ALLOC_MULT(short, গুনতি + 1);
	    যদি (retval == রিক্ত)
		ভঙ্গ;
	    retval[গুনতি] = 0;	    // zero means end of the list
	    total_গুনতি = গুনতি;
	}
    }

    *arg = p;
    যদি (failed || retval == রিক্ত)
    {
	vim_free(retval);
	ঘূৰাই FAIL;
    }

    যদি (*list == রিক্ত)
	*list = retval;
    অন্যথা
	vim_free(retval);	// list already found, don't overwrite it

    ঘূৰাই OK;
}

/*
 * Make a copy of an ID list.
 */
    স্থির short *
copy_id_list(short *list)
{
    পূর্ণাঙ্ক	    len;
    পূর্ণাঙ্ক	    গুনতি;
    short   *retval;

    যদি (list == রিক্ত)
	ঘূৰাই রিক্ত;

    বাবে (গুনতি = 0; list[গুনতি]; ++গুনতি)
	;
    len = (গুনতি + 1) * পরিমাণ(short);
    retval = alloc(len);
    যদি (retval != রিক্ত)
	mch_memmove(retval, list, (size_t)len);

    ঘূৰাই retval;
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

প্রকারনির্দেশ সংরচনা
{
    short	*idl_চাবি;	// the list this entry resolves (NULL when empty)
    short	*idl_ids;	// sorted, cluster-expanded group IDs (allocated)
    পূর্ণাঙ্ক		idl_গুনতি;	// number of IDs in idl_ids
    short	idl_marker;	// leading ALLBUT/TOP/CONTAINED item, or 0
    bool	idl_usable;	// false: not cacheable, use the slow path
} idl_entry_T;

প্রকারনির্দেশ সংরচনা
{
    idl_entry_T	ent[IDL_CACHE_SIZE];
    পূর্ণাঙ্ক		round;		// round-robin replacement index
    পূর্ণাঙ্ক		last;		// index of the last returned entry (fast path)
} idl_cache_T;

/*
 * Collect the explicit group IDs of "list" into "ga", expanding clusters.
 * "seen" records visited clusters to break cycles and avoid duplicate work.
 * Return false when the list cannot be represented as a plain set (a nested
 * ALLBUT/TOP/CONTAINED marker) or on out of memory.
 */
    স্থির bool
idl_collect(short *list, garray_T *ga, garray_T *seen)
{
    যদি (list == রিক্ত || list == ID_LIST_ALL)
	ঘূৰাই false;
    বাবে (short item = *list; item != 0; item = *++list)
    {
	যদি (item >= SYNID_ALLBUT && item < SYNID_CLUSTER)
	    ঘূৰাই false;	// nested ALLBUT/TOP/CONTAINED: not cacheable
	যদি (item >= SYNID_CLUSTER)
	{
	    short	*scl_list;
	    bool	seen_it = false;

	    বাবে (পূর্ণাঙ্ক i = 0; i < seen->ga_len; ++i)
		যদি (((short *)seen->ga_data)[i] == item)
		{
		    seen_it = true;
		    ভঙ্গ;
		}
	    যদি (seen_it)
		অনুৱর্ত;
	    যদি (ga_grow(seen, 1) == FAIL)
		ঘূৰাই false;
	    ((short *)seen->ga_data)[seen->ga_len++] = item;
	    // Record the cluster id itself, not just its members: the slow
	    // path matches "item == id" for a cluster id present in the list.
	    যদি (ga_grow(ga, 1) == FAIL)
		ঘূৰাই false;
	    ((short *)ga->ga_data)[ga->ga_len++] = item;
	    scl_list = SYN_CLSTR(syn_block)[item - SYNID_CLUSTER].scl_list;
	    যদি (scl_list != রিক্ত && !idl_collect(scl_list, ga, seen))
		ঘূৰাই false;
	}
	অন্যথা
	{
	    যদি (ga_grow(ga, 1) == FAIL)
		ঘূৰাই false;
	    ((short *)ga->ga_data)[ga->ga_len++] = item;
	}
    }
    ঘূৰাই true;
}

    স্থির পূর্ণাঙ্ক
idl_id_cmp(ধ্রুৱ শূন্য *a, ধ্রুৱ শূন্য *b)
{
    ঘূৰাই (পূর্ণাঙ্ক)*(ধ্রুৱ short *)a - (পূর্ণাঙ্ক)*(ধ্রুৱ short *)b;
}

/*
 * Get the cache entry that resolves "list", building it if needed.
 * Returns NULL only on allocation failure.
 */
    স্থির idl_entry_T *
idl_get_entry(short *list)
{
    idl_cache_T	*cache = (idl_cache_T *)syn_block->b_idlist_cache;
    idl_entry_T	*e;
    garray_T	ga, seen;

    যদি (cache == রিক্ত)
    {
	cache = ALLOC_CLEAR_ONE(idl_cache_T);
	যদি (cache == রিক্ত)
	    ঘূৰাই রিক্ত;
	syn_block->b_idlist_cache = cache;
    }
    // Fast path: the same list is usually queried many times in a row.
    যদি (cache->ent[cache->last].idl_চাবি == list)
	ঘূৰাই &cache->ent[cache->last];
    বাবে (পূর্ণাঙ্ক i = 0; i < IDL_CACHE_SIZE; ++i)
	যদি (cache->ent[i].idl_চাবি == list)
	{
	    cache->last = i;
	    ঘূৰাই &cache->ent[i];
	}

    // Build a new entry, replacing one in round-robin order.
    cache->last = cache->round;
    e = &cache->ent[cache->round];
    cache->round = (cache->round + 1) % IDL_CACHE_SIZE;
    VIM_CLEAR(e->idl_ids);
    e->idl_গুনতি = 0;
    e->idl_চাবি = list;
    e->idl_marker = 0;
    e->idl_usable = true;

    // A leading ALLBUT/TOP/CONTAINED marker is kept aside; the rest is the set.
    যদি (*list >= SYNID_ALLBUT && *list < SYNID_CLUSTER)
    {
	e->idl_marker = *list;
	++list;
    }

    ga_init2(&ga, পরিমাণ(short), 50);
    ga_init2(&seen, পরিমাণ(short), 10);
    যদি (!idl_collect(list, &ga, &seen))
    {
	e->idl_usable = false;
	ga_clear(&ga);
	ga_clear(&seen);
	ঘূৰাই e;
    }
    ga_clear(&seen);
    যদি (ga.ga_len > 0)
    {
	qsort(ga.ga_data, (size_t)ga.ga_len, পরিমাণ(short), idl_id_cmp);
	e->idl_ids = (short *)ga.ga_data;	// take ownership
	e->idl_গুনতি = ga.ga_len;
    }
    অন্যথা
	ga_clear(&ga);
    ঘূৰাই e;
}

/*
 * Return true if "id" is in the resolved set of cache entry "e".
 */
    স্থির bool
idl_contains(idl_entry_T *e, short id)
{
    পূর্ণাঙ্ক	lo = 0;
    পূর্ণাঙ্ক	hi = e->idl_গুনতি - 1;

    যেতিয়ালৈ (lo <= hi)
    {
	পূর্ণাঙ্ক	mid = (lo + hi) / 2;
	short	v = e->idl_ids[mid];

	যদি (v == id)
	    ঘূৰাই true;
	যদি (v < id)
	    lo = mid + 1;
	অন্যথা
	    hi = mid - 1;
    }
    ঘূৰাই false;
}

    স্থির শূন্য
idl_cache_clear(synblock_T *block)
{
    idl_cache_T	*cache = (idl_cache_T *)block->b_idlist_cache;

    যদি (cache == রিক্ত)
	ঘূৰাই;
    বাবে (পূর্ণাঙ্ক i = 0; i < IDL_CACHE_SIZE; ++i)
	vim_free(cache->ent[i].idl_ids);
    vim_free(cache);
    block->b_idlist_cache = রিক্ত;
}
// }}}

/*
 * Check if syntax group "ssp" is in the ID list "list" of "cur_si".
 * "cur_si" can be NULL if not checking the "containedin" list.
 * Used to check if a syntax item is in the "contains" or "nextgroup" list of
 * the current item.
 * This function is called very often, keep it fast!!
 */
    স্থির পূর্ণাঙ্ক
in_id_list(
    stateitem_T	*cur_si,	// current item or NULL
    short	*list,		// id list
    সংরচনা sp_syn *ssp,		// group id and ":syn include" tag of group
    পূর্ণাঙ্ক		flags)		// group flags
{
    পূর্ণাঙ্ক		retval;
    short	*scl_list;
    short	item;
    short	id = ssp->id;
    স্থির পূর্ণাঙ্ক	depth = 0;
    পূর্ণাঙ্ক		r;
    পূর্ণাঙ্ক		toplevel;
    idl_entry_T	*e;

    // If ssp has a "containedin" list and "cur_si" is in it, return TRUE.
    যদি (cur_si != রিক্ত && ssp->cont_in_list != রিক্ত
					    && !(cur_si->si_flags & HL_MATCH))
    {
	// Ignore transparent items without a contains argument.  Double check
	// that we don't go back past the first one.
	যেতিয়ালৈ ((cur_si->si_flags & HL_TRANS_CONT)
		&& cur_si > (stateitem_T *)(current_state.ga_data))
	    --cur_si;
	// cur_si->si_idx is -1 for keywords, these never contain anything.
	যদি (cur_si->si_idx >= 0 && in_id_list(রিক্ত, ssp->cont_in_list,
		&(SYN_বস্তুসমূহ(syn_block)[cur_si->si_idx].sp_syn),
		  SYN_বস্তুসমূহ(syn_block)[cur_si->si_idx].sp_flags))
	    ঘূৰাই TRUE;
    }

    যদি (list == রিক্ত)
	ঘূৰাই FALSE;

    /*
     * If list is ID_LIST_ALL, we are in a transparent item that isn't
     * inside anything.  Only allow not-contained groups.
     */
    যদি (list == ID_LIST_ALL)
	ঘূৰাই !(flags & HL_CONTAINED);

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
    e = disable_syn_idlist_cache_for_testing ? রিক্ত : idl_get_entry(list);
    যদি (e != রিক্ত && e->idl_usable)
    {
	যদি (e->idl_marker != 0)
	{
	    item = e->idl_marker;
	    যদি (item < SYNID_শীৰ্ষ)
	    {
		যদি (item - SYNID_ALLBUT != ssp->inc_tag)
		    ঘূৰাই FALSE;
	    }
	    অন্যথা যদি (item < SYNID_CONTAINED)
	    {
		যদি (item - SYNID_শীৰ্ষ != ssp->inc_tag || !toplevel)
		    ঘূৰাই FALSE;
	    }
	    অন্যথা
	    {
		যদি (item - SYNID_CONTAINED != ssp->inc_tag || toplevel)
		    ঘূৰাই FALSE;
	    }
	    retval = FALSE;
	}
	অন্যথা
	    retval = TRUE;
	ঘূৰাই idl_contains(e, id) ? retval : !retval;
    }

    /*
     * Slow path (uncacheable list, e.g. a nested ALLBUT).
     * If the first item is "ALLBUT", return TRUE if "id" is NOT in the
     * contains list.  We also require that "id" is at the same ":syn include"
     * level as the list.
     */
    item = *list;
    যদি (item >= SYNID_ALLBUT && item < SYNID_CLUSTER)
    {
	যদি (item < SYNID_শীৰ্ষ)
	{
	    // ALL or ALLBUT: accept all groups in the same file
	    যদি (item - SYNID_ALLBUT != ssp->inc_tag)
		ঘূৰাই FALSE;
	}
	অন্যথা যদি (item < SYNID_CONTAINED)
	{
	    // TOP: accept all not-contained groups in the same file
	    যদি (item - SYNID_শীৰ্ষ != ssp->inc_tag || !toplevel)
		ঘূৰাই FALSE;
	}
	অন্যথা
	{
	    // CONTAINED: accept all contained groups in the same file
	    যদি (item - SYNID_CONTAINED != ssp->inc_tag || toplevel)
		ঘূৰাই FALSE;
	}
	item = *++list;
	retval = FALSE;
    }
    অন্যথা
	retval = TRUE;

    /*
     * Return "retval" if id is in the contains list.
     */
    যেতিয়ালৈ (item != 0)
    {
	যদি (item == id)
	    ঘূৰাই retval;
	যদি (item >= SYNID_CLUSTER)
	{
	    scl_list = SYN_CLSTR(syn_block)[item - SYNID_CLUSTER].scl_list;
	    // restrict recursiveness to 30 to avoid an endless loop for a
	    // cluster that includes itself (indirectly)
	    যদি (scl_list != রিক্ত && depth < 30)
	    {
		++depth;
		r = in_id_list(রিক্ত, scl_list, ssp, flags);
		--depth;
		যদি (r)
		    ঘূৰাই retval;
	    }
	}
	item = *++list;
    }
    ঘূৰাই !retval;
}

সংরচনা subcommand
{
    ৱর্ণ    *name;			// subcommand name
    শূন্য    (*func)(exarg_T *, পূর্ণাঙ্ক);	// function to call
};

স্থির সংরচনা subcommand subcommands[] =
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
    শূন্য
ex_syntax(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*subcmd_end;
    char_u	*subcmd_name;
    পূর্ণাঙ্ক		i;

    syn_cmdlinep = eap->cmdlinep;

    // isolate subcommand name
    বাবে (subcmd_end = arg; ASCII_ISALPHA(*subcmd_end); ++subcmd_end)
	;
    subcmd_name = vim_strnsave(arg, subcmd_end - arg);
    যদি (subcmd_name == রিক্ত)
	ঘূৰাই;

    যদি (eap->skip)		// skip error messages for all subcommands
	++emsg_skip;
    বাবে (i = 0; i < (পূর্ণাঙ্ক)ARRAY_LENGTH(subcommands); ++i)
    {
	যদি (STRCMP(subcmd_name, (char_u *)subcommands[i].name) == 0)
	{
	    eap->arg = skipwhite(subcmd_end);
	    (subcommands[i].func)(eap, FALSE);
	    ভঙ্গ;
	}
    }

    যদি (i == (পূর্ণাঙ্ক)ARRAY_LENGTH(subcommands))
	semsg(_(e_invalid_syntax_subcommand_str), subcmd_name);

    vim_free(subcmd_name);
    যদি (eap->skip)
	--emsg_skip;
}

    শূন্য
ex_ownsyntax(exarg_T *eap)
{
    char_u	*old_মান;
    char_u	*new_মান_2;

    যদি (curwin->w_s == &curwin->w_buffer->b_s)
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
    old_মান = get_var_মান((char_u *)"b:current_syntax");
    যদি (old_মান != রিক্ত)
	old_মান = vim_strsave(old_মান);

    // Apply the "syntax" autocommand event, this finds and loads the syntax
    // file.
    apply_autocmds(EVENT_SYNTAX, eap->arg, curbuf->b_fname, TRUE, curbuf);

    // move value of b:current_syntax to w:current_syntax
    new_মান_2 = get_var_মান((char_u *)"b:current_syntax");
    যদি (new_মান_2 != রিক্ত)
	set_internal_string_var((char_u *)"w:current_syntax", new_মান_2);

    // restore value of b:current_syntax
    যদি (old_মান == রিক্ত)
	do_unlet((char_u *)"b:current_syntax", TRUE);
    অন্যথা
    {
	set_internal_string_var((char_u *)"b:current_syntax", old_মান);
	vim_free(old_মান);
    }
}

    পূর্ণাঙ্ক
syntax_present(win_T *win)
{
    ঘূৰাই (win->w_s->b_syn_patterns.ga_len != 0
	    || win->w_s->b_syn_clusters.ga_len != 0
	    || win->w_s->b_keywtab.ht_used > 0
	    || win->w_s->b_keywtab_ic.ht_used > 0);
}


স্থির গণনা
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
    শূন্য
reset_expand_highlight(শূন্য)
{
    include_link = include_default = include_none = 0;
}

/*
 * Handle command line completion for :match and :echohl command: Add "NONE"
 * as highlight group.
 */
    শূন্য
set_context_in_echohl_cmd(expand_T *xp, char_u *arg)
{
    xp->xp_context = EXPAND_HIGHLIGHT;
    xp->xp_pattern = arg;
    include_none = 1;
}

/*
 * Handle command line completion for :syntax command.
 */
    শূন্য
set_context_in_syntax_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    // Default: expand subcommands
    xp->xp_context = EXPAND_SYNTAX;
    expand_what = EXP_SUBCMD;
    xp->xp_pattern = arg;
    include_link = 0;
    include_default = 0;

    যদি (*arg == NUL)
	ঘূৰাই;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    যদি (*p == NUL)
	ঘূৰাই;

    // past first word
    xp->xp_pattern = skipwhite(p);
    যদি (*skiptowhite(xp->xp_pattern) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    অন্যথা যদি (STRNICMP(arg, "case", p - arg) == 0)
	expand_what = EXP_CASE;
    অন্যথা যদি (STRNICMP(arg, "spell", p - arg) == 0)
	expand_what = EXP_SPELL;
    অন্যথা যদি (STRNICMP(arg, "sync", p - arg) == 0)
	expand_what = EXP_SYNC;
    অন্যথা যদি (STRNICMP(arg, "list", p - arg) == 0)
    {
	p = skipwhite(p);
	যদি (*p == '@')
	    expand_what = EXP_CLUSTER;
	অন্যথা
	    xp->xp_context = EXPAND_HIGHLIGHT;
    }
    অন্যথা যদি (STRNICMP(arg, "keyword", p - arg) == 0
	    || STRNICMP(arg, "region", p - arg) == 0
	    || STRNICMP(arg, "match", p - arg) == 0)
	xp->xp_context = EXPAND_HIGHLIGHT;
    অন্যথা
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Function given to ExpandGeneric() to obtain the list syntax names for
 * expansion.
 */
    char_u *
get_syntax_name(expand_T *xp, পূর্ণাঙ্ক idx)
{
    ৱিকল্প (expand_what)
    {
	পক্ষ EXP_SUBCMD:
	    যদি (idx < 0 || idx >= (পূর্ণাঙ্ক)ARRAY_LENGTH(subcommands))
		ঘূৰাই রিক্ত;
	    ঘূৰাই (char_u *)subcommands[idx].name;
	পক্ষ EXP_CASE:
	{
	    স্থির ৱর্ণ *case_args[] = {"match", "ignore", রিক্ত};
	    ঘূৰাই (char_u *)case_args[idx];
	}
	পক্ষ EXP_SPELL:
	{
	    স্থির ৱর্ণ *spell_args[] =
		{"toplevel", "notoplevel", "default", রিক্ত};
	    ঘূৰাই (char_u *)spell_args[idx];
	}
	পক্ষ EXP_SYNC:
	{
	    স্থির ৱর্ণ *sync_args[] =
		{"ccomment", "clear", "fromstart",
		 "linebreaks=", "linecont", "lines=", "match",
		 "maxlines=", "minlines=", "region", রিক্ত};
	    ঘূৰাই (char_u *)sync_args[idx];
	}
	পক্ষ EXP_CLUSTER:
	{
	    যদি (idx < curwin->w_s->b_syn_clusters.ga_len)
	    {
		vim_snprintf((ৱর্ণ *)xp->xp_buf, EXPAND_BUF_LEN, "@%s",
					 SYN_CLSTR(curwin->w_s)[idx].scl_name);
		ঘূৰাই xp->xp_buf;
	    }
	    অন্যথা
		ঘূৰাই রিক্ত;
	}
    }
    ঘূৰাই রিক্ত;
}


/*
 * Function called for expression evaluation: get syntax ID at file position.
 */
    পূর্ণাঙ্ক
syn_get_id(
    win_T	*wp,
    long	lnum,
    colnr_T	col,
    পূর্ণাঙ্ক		trans,	     // remove transparency
    পূর্ণাঙ্ক		*spellp,     // return: can do spell checking
    পূর্ণাঙ্ক		keep_state)  // keep state of char at "col"
{
    // When the position is not after the current position and in the same
    // line of the same window with the same buffer, need to restart parsing.
    যদি (wp != syn_win
	    || wp->w_buffer != syn_buf
	    || lnum != current_lnum
	    || col < current_col)
	syntax_start(wp, lnum);
    অন্যথা যদি (wp->w_buffer == syn_buf
	    && lnum == current_lnum
	    && col > current_col)
	// next_match may not be correct when moving around, e.g. with the
	// "skip" expression in searchpair()
	next_match_idx = -1;

    (শূন্য)get_syntax_attr(col, spellp, keep_state);

    ঘূৰাই (trans ? current_trans_id : current_id);
}

#যদি defined(FEAT_CONCEAL)
/*
 * Get extra information about the syntax item.  Must be called right after
 * get_syntax_attr().
 * Stores the current item sequence nr in "*seqnrp".
 * Returns the current flags.
 */
    পূর্ণাঙ্ক
get_syntax_info(পূর্ণাঙ্ক *seqnrp)
{
    *seqnrp = current_seqnr;
    ঘূৰাই current_flags;
}

/*
 * Return conceal substitution character
 */
    পূর্ণাঙ্ক
syn_get_sub_char(শূন্য)
{
    ঘূৰাই current_sub_char;
}
#endif

#যদি defined(FEAT_EVAL)
/*
 * Return the syntax ID at position "i" in the current stack.
 * The caller must have called syn_get_id() before to fill the stack.
 * Returns -1 when "i" is out of range.
 */
    পূর্ণাঙ্ক
syn_get_স্তূপ_item(পূর্ণাঙ্ক i)
{
    যদি (i >= current_state.ga_len)
    {
	// Need to invalidate the state, because we didn't properly finish it
	// for the last character, "keep_state" was TRUE.
	invalidate_current_state();
	current_col = MAXCOL;
	ঘূৰাই -1;
    }
    ঘূৰাই CUR_STATE(i).si_id;
}
#endif

#যদি defined(FEAT_FOLDING)
    স্থির পূর্ণাঙ্ক
syn_cur_foldlevel(শূন্য)
{
    পূর্ণাঙ্ক		level = 0;
    পূর্ণাঙ্ক		i;

    বাবে (i = 0; i < current_state.ga_len; ++i)
	যদি (CUR_STATE(i).si_flags & HL_FOLD)
	    ++level;
    ঘূৰাই level;
}

/*
 * Function called to get folding level for line "lnum" in window "wp".
 */
    পূর্ণাঙ্ক
syn_get_foldlevel(win_T *wp, long lnum)
{
    পূর্ণাঙ্ক		level = 0;
    পূর্ণাঙ্ক		নিচেৰ_level;
    পূর্ণাঙ্ক		cur_level;

    // Return quickly when there are no fold items at all.
    যদি (wp->w_s->b_syn_folditems != 0
	    && !wp->w_s->b_syn_error
# ifdef SYN_TIME_LIMIT
	    && !wp->w_s->b_syn_slow
# endif
	    )
    {
	syntax_start(wp, lnum);

	// Start with the fold level at the start of the line.
	level = syn_cur_foldlevel();

	যদি (wp->w_s->b_syn_foldlevel == SYNFLD_MINIMUM)
	{
	    // Find the lowest fold level that is followed by a higher one.
	    cur_level = level;
	    নিচেৰ_level = cur_level;
	    যেতিয়ালৈ (!current_finished)
	    {
		(শূন্য)syn_current_attr(FALSE, FALSE, রিক্ত, FALSE);
		cur_level = syn_cur_foldlevel();
		যদি (cur_level < নিচেৰ_level)
		    নিচেৰ_level = cur_level;
		অন্যথা যদি (cur_level > নিচেৰ_level)
		    level = নিচেৰ_level;
		++current_col;
	    }
	}
    }
    যদি (level > wp->w_p_fdn)
    {
	level = wp->w_p_fdn;
	যদি (level < 0)
	    level = 0;
    }
    ঘূৰাই level;
}
#endif

#যদি defined(FEAT_PROFILE)
/*
 * ":syntime".
 */
    শূন্য
ex_syntime(exarg_T *eap)
{
    যদি (STRCMP(eap->arg, "on") == 0)
	syn_time_on = TRUE;
    অন্যথা যদি (STRCMP(eap->arg, "off") == 0)
	syn_time_on = FALSE;
    অন্যথা যদি (STRCMP(eap->arg, "clear") == 0)
	syntime_clear();
    অন্যথা যদি (STRCMP(eap->arg, "report") == 0)
	syntime_report();
    অন্যথা
	semsg(_(e_invalid_argument_str), eap->arg);
}

    স্থির শূন্য
syn_clear_time(syn_time_T *st)
{
    profile_zero(&st->total);
    profile_zero(&st->slowest);
    st->গুনতি = 0;
    st->match = 0;
}

/*
 * Clear the syntax timing for the current buffer.
 */
    স্থির শূন্য
syntime_clear(শূন্য)
{
    পূর্ণাঙ্ক		idx;
    synpat_T	*spp;

    যদি (!syntax_present(curwin))
    {
	বাৰ্তা(_(বাৰ্তা_no_বস্তুসমূহ));
	ঘূৰাই;
    }
    বাবে (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len; ++idx)
    {
	spp = &(SYN_বস্তুসমূহ(curwin->w_s)[idx]);
	syn_clear_time(&spp->sp_time);
    }
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":syntime {on,off,clear,report}" command.
 */
    char_u *
get_syntime_arg(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    ৱিকল্প (idx)
    {
	পক্ষ 0: ঘূৰাই (char_u *)"on";
	পক্ষ 1: ঘূৰাই (char_u *)"off";
	পক্ষ 2: ঘূৰাই (char_u *)"clear";
	পক্ষ 3: ঘূৰাই (char_u *)"report";
    }
    ঘূৰাই রিক্ত;
}

প্রকারনির্দেশ সংরচনা
{
    proftime_T	total;
    পূর্ণাঙ্ক		গুনতি;
    পূর্ণাঙ্ক		match;
    proftime_T	slowest;
    proftime_T	average;
    পূর্ণাঙ্ক		id;
    char_u	*pattern;
} time_entry_T;

    স্থির পূর্ণাঙ্ক
syn_compare_syntime(ধ্রুৱ শূন্য *v1, ধ্রুৱ শূন্য *v2)
{
    ধ্রুৱ time_entry_T	*s1 = v1;
    ধ্রুৱ time_entry_T	*s2 = v2;

    ঘূৰাই profile_cmp(&s1->total, &s2->total);
}

/*
 * Clear the syntax timing for the current buffer.
 */
    স্থির শূন্য
syntime_report(শূন্য)
{
    পূর্ণাঙ্ক		idx;
    synpat_T	*spp;
# যদি defined(FEAT_RELTIME)
    proftime_T	tm;
# endif
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		patlen;
    proftime_T	total_total;
    পূর্ণাঙ্ক		total_গুনতি = 0;
    garray_T    ga;
    time_entry_T *p;

    যদি (!syntax_present(curwin))
    {
	বাৰ্তা(_(বাৰ্তা_no_বস্তুসমূহ));
	ঘূৰাই;
    }

    ga_init2(&ga, পরিমাণ(time_entry_T), 50);
    profile_zero(&total_total);
    বাবে (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len; ++idx)
    {
	spp = &(SYN_বস্তুসমূহ(curwin->w_s)[idx]);
	যদি (spp->sp_time.গুনতি > 0)
	{
	    (শূন্য)ga_grow(&ga, 1);
	    p = ((time_entry_T *)ga.ga_data) + ga.ga_len;
	    p->total = spp->sp_time.total;
	    profile_add(&total_total, &spp->sp_time.total);
	    p->গুনতি = spp->sp_time.গুনতি;
	    p->match = spp->sp_time.match;
	    total_গুনতি += spp->sp_time.গুনতি;
	    p->slowest = spp->sp_time.slowest;
# যদি defined(FEAT_RELTIME)
	    profile_divide(&spp->sp_time.total, spp->sp_time.গুনতি, &tm);
	    p->average = tm;
# endif
	    p->id = spp->sp_syn.id;
	    p->pattern = spp->sp_pattern;
	    ++ga.ga_len;
	}
    }

    // Sort on total time. Skip if there are no items to avoid passing NULL
    // pointer to qsort().
    যদি (ga.ga_len > 1)
	qsort(ga.ga_data, (size_t)ga.ga_len, পরিমাণ(time_entry_T),
							 syn_compare_syntime);

    বাৰ্তা_puts_title(_("  TOTAL      COUNT  MATCH   SLOWEST     AVERAGE   NAME               PATTERN"));
    বাৰ্তা_puts("\n");
    বাবে (idx = 0; idx < ga.ga_len && !got_int; ++idx)
    {
	p = ((time_entry_T *)ga.ga_data) + idx;

	বাৰ্তা_puts(profile_বাৰ্তা(&p->total));
	বাৰ্তা_puts(" "); // make sure there is always a separating space
	বাৰ্তা_advance(13);
	বাৰ্তা_outnum(p->গুনতি);
	বাৰ্তা_puts(" ");
	বাৰ্তা_advance(20);
	বাৰ্তা_outnum(p->match);
	বাৰ্তা_puts(" ");
	বাৰ্তা_advance(26);
	বাৰ্তা_puts(profile_বাৰ্তা(&p->slowest));
	বাৰ্তা_puts(" ");
	বাৰ্তা_advance(38);
	বাৰ্তা_puts(profile_বাৰ্তা(&p->average));
	বাৰ্তা_puts(" ");
	বাৰ্তা_advance(50);
	বাৰ্তা_outtrans(highlight_group_name(p->id - 1));
	বাৰ্তা_puts(" ");

	বাৰ্তা_advance(69);
	যদি (Columns < 80)
	    len = 20; // will wrap anyway
	অন্যথা
	    len = Columns - 70;
	patlen = (পূর্ণাঙ্ক)STRLEN(p->pattern);
	যদি (len > patlen)
	    len = patlen;
	বাৰ্তা_outtrans_len(p->pattern, len);
	বাৰ্তা_puts("\n");
    }
    ga_clear(&ga);
    যদি (!got_int)
    {
	বাৰ্তা_puts("\n");
	বাৰ্তা_puts(profile_বাৰ্তা(&total_total));
	বাৰ্তা_advance(13);
	বাৰ্তা_outnum(total_গুনতি);
	বাৰ্তা_puts("\n");
    }
}
#endif

#endif // FEAT_SYN_HL
