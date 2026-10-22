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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_SYN_HL)

// different types of offsets that are possible
#define SPO_MS_OFF	0	// match  start offset
#define SPO_ME_OFF	1	// match  end	offset
#define SPO_HS_OFF	2	// highl. start offset
#define SPO_HE_OFF	3	// highl. end	offset
#define SPO_RS_OFF	4	// region start offset
#define SPO_RE_OFF	5	// region end	offset
#define SPO_LC_OFF	6	// leading context offset
#define SPO_மொத்தம்	7

நிலையான எழுத்து *(spo_name_tab[SPO_மொத்தம்]) =
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
வகைவரை கட்டமைப்பு syn_pattern
{
    எழுத்து	 sp_type;		// see SPTYPE_ defines below
    எழுத்து	 sp_syncing;		// this item used for syncing
    short	 sp_syn_match_id;	// highlight group ID of pattern
    short	 sp_off_flags;		// see below
    முழு		 sp_offsets[SPO_மொத்தம்];	// offsets
    முழு		 sp_flags;		// see HL_ defines below
#ifdef FEAT_CONCEAL
    முழு		 sp_cchar;		// conceal substitute character
#endif
    முழு		 sp_ic;			// ignore-case flag for sp_prog
    முழு		 sp_sync_idx;		// sync item index (syncing only)
    முழு		 sp_வரி_id;		// ID of last line where tried
    முழு		 sp_startcol;		// next match in sp_line_id line
    short	*sp_cont_list;		// cont. group IDs, if non-zero
    short	*sp_next_list;		// next group IDs, if non-zero
    கட்டமைப்பு sp_syn sp_syn;		// struct passed to in_id_list()
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


#define SYN_பொருட்கள்(buf)	((synpat_T *)((buf)->b_syn_patterns.ga_data))

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
நிலையான முழு current_attr = 0;	    // attr of current syntax word
#ifdef FEAT_EVAL
நிலையான முழு current_id = 0;	    // ID of current char for syn_get_id()
நிலையான முழு current_trans_id = 0;    // idem, transparency removed
#endif
#ifdef FEAT_CONCEAL
நிலையான முழு current_flags = 0;
நிலையான முழு current_seqnr = 0;
நிலையான முழு current_sub_char = 0;
#endif

வகைவரை கட்டமைப்பு syn_cluster_S
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
#define SYNID_உச்சி	21000	    // syntax group ID for contains=TOP
#define SYNID_CONTAINED	22000	    // syntax group ID for contains=CONTAINED
#define SYNID_CLUSTER	23000	    // first syntax group ID for clusters

#define MAX_SYN_INC_TAG	999	    // maximum before the above overflow
#define MAX_CLUSTER_ID  (32767 - SYNID_CLUSTER)

/*
 * Annoying Hack(TM):  ":syn include" needs this pointer to pass to
 * expand_filename().  Most of the other syntax commands don't need it, so
 * instead of passing it to them, we stow it here.
 */
நிலையான char_u **syn_cmdlinep;

/*
 * Another Annoying Hack(TM):  To prevent rules from other ":syn include"'d
 * files from leaking into ALLBUT lists, we assign a unique ID to the
 * rules in each ":syn include"'d file.
 */
நிலையான முழு current_syn_inc_tag = 0;
நிலையான முழு running_syn_inc_tag = 0;

/*
 * In a hashtable item "hi_key" points to "keyword" in a keyentry.
 * This avoids adding a pointer to the hashtable item.
 * KE2HIKEY() converts a var pointer to a hashitem key pointer.
 * HIKEY2KE() converts a hashitem key pointer to a var pointer.
 * HI2KE() converts a hashitem pointer to a var pointer.
 */
நிலையான keyentry_T dumkey;
#define KE2HIKEY(kp)  ((kp)->keyword)
#define HIKEY2KE(p)   ((keyentry_T *)((p) - (dumkey.keyword - (char_u *)&dumkey)))
#define HI2KE(hi)      HIKEY2KE((hi)->hi_சாவி)

/*
 * To reduce the time spent in keepend(), remember at which level in the state
 * stack the first item with "keepend" is present.  When "-1", there is no
 * "keepend" on the stack.
 */
நிலையான முழு keepend_level = -1;

நிலையான எழுத்து செய்தி_no_பொருட்கள்[] = N_("No Syntax items defined for this buffer");

/*
 * For the current state we need to remember more than just the idx.
 * When si_m_endpos.lnum is 0, the items other than si_idx are unknown.
 * (The end positions have the column number of the next char)
 */
வகைவரை கட்டமைப்பு state_item
{
    முழு		si_idx;			// index of syntax pattern or
					// KEYWORD_IDX
    முழு		si_id;			// highlight group ID for keywords
    முழு		si_trans_id;		// idem, transparency removed
    முழு		si_m_lnum;		// lnum of the match
    முழு		si_m_startcol;		// starting column of the match
    lpos_T	si_m_endpos;		// just after end posn of the match
    lpos_T	si_h_startpos;		// start position of the highlighting
    lpos_T	si_h_endpos;		// end position of the highlighting
    lpos_T	si_eoe_pos;		// end position of end pattern
    முழு		si_end_idx;		// group ID for end pattern or zero
    முழு		si_ends;		// if match ends before si_m_endpos
    முழு		si_attr;		// attributes in this state
    long	si_flags;		// HL_HAS_EOL flag in this state, and
					// HL_SKIP* for si_next_list
#ifdef FEAT_CONCEAL
    முழு		si_seqnr;		// sequence number
    முழு		si_cchar;		// substitution character for conceal
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
நிலையான முழு next_seqnr = 1;		// value to use for si_seqnr
#endif

/*
 * Struct to reduce the number of arguments to get_syn_options(), it's used
 * very often.
 */
வகைவரை கட்டமைப்பு
{
    முழு		flags;		// flags for contained and transparent
    முழு		keyword;	// TRUE for ":syn keyword"
    முழு		*sync_idx;	// syntax item for "grouphere" argument, NULL
				// if not allowed
    எழுத்து	has_cont_list;	// TRUE if "cont_list" can be used
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
நிலையான முழு next_match_col;		// column for start of next match
நிலையான lpos_T next_match_m_endpos;	// position for end of next match
நிலையான lpos_T next_match_h_startpos;	// pos. for highl. start of next match
நிலையான lpos_T next_match_h_endpos;	// pos. for highl. end of next match
நிலையான முழு next_match_idx;		// index of matched item
நிலையான long next_match_flags;		// flags for next match
நிலையான lpos_T next_match_eos_pos;	// end of start pattn (start region)
நிலையான lpos_T next_match_eoe_pos;	// pos. for end of end pattern
நிலையான முழு next_match_end_idx;		// ID of group for end pattn or zero
நிலையான reg_extmatch_T *next_match_extmatch = வெற்று;

/*
 * A state stack is an array of integers or stateitem_T, stored in a
 * garray_T.  A state stack is invalid if its itemsize entry is zero.
 */
#define INVALID_STATE(ssp)  ((ssp)->ga_itemsize == 0)
#define VALID_STATE(ssp)    ((ssp)->ga_itemsize != 0)

#define FOR_ALL_SYNSTATES(sb, sst) \
    க்கு ((sst) = (sb)->b_sst_first; (sst) != வெற்று; (sst) = (sst)->sst_next)

/*
 * The current state (within the line) of the recognition engine.
 * When current_state.ga_itemsize is 0 the current state is invalid.
 */
நிலையான win_T	*syn_win;		// current window for highlighting
நிலையான buf_T	*syn_buf;		// current buffer for highlighting
நிலையான synblock_T *syn_block;		// current buffer for highlighting
நிலையான linenr_T current_lnum = 0;	// lnum of current state
நிலையான colnr_T	current_col = 0;	// column of current state
நிலையான முழு	current_state_stored = 0; // TRUE if stored current state
					  // after setting current_finished
நிலையான முழு	current_finished = 0;	// current line has been finished
நிலையான garray_T current_state		// current stack of state_items
		= {0, 0, 0, 0, வெற்று};
நிலையான short	*current_next_list = வெற்று; // when non-zero, nextgroup list
நிலையான முழு	current_next_flags = 0; // flags for current_next_list
நிலையான முழு	current_வரி_id = 0;	// unique number for current line

#define CUR_STATE(idx)	((stateitem_T *)(current_state.ga_data))[idx]

நிலையான வெறுமை syn_sync(win_T *wp, linenr_T lnum, synstate_T *last_valid);
நிலையான முழு syn_match_linecont(linenr_T lnum);
நிலையான வெறுமை syn_start_வரி(வெறுமை);
நிலையான வெறுமை syn_update_ends(முழு startofline);
நிலையான வெறுமை syn_அடுக்கு_alloc(வெறுமை);
நிலையான முழு syn_அடுக்கு_cleanup(வெறுமை);
நிலையான வெறுமை syn_அடுக்கு_free_entry(synblock_T *block, synstate_T *p);
நிலையான synstate_T *syn_அடுக்கு_find_entry(linenr_T lnum);
நிலையான synstate_T *store_current_state(வெறுமை);
நிலையான வெறுமை load_current_state(synstate_T *from);
நிலையான வெறுமை invalidate_current_state(வெறுமை);
நிலையான முழு syn_அடுக்கு_equal(synstate_T *sp);
நிலையான வெறுமை validate_current_state(வெறுமை);
நிலையான முழு syn_finish_வரி(முழு syncing);
நிலையான முழு syn_current_attr(முழு syncing, முழு displaying, முழு *can_spell, முழு keep_state);
நிலையான முழு did_match_already(முழு idx, garray_T *gap);
நிலையான stateitem_T *தள்ளு_next_match(stateitem_T *cur_si);
நிலையான வெறுமை check_state_ends(வெறுமை);
நிலையான வெறுமை update_si_attr(முழு idx);
நிலையான வெறுமை check_keepend(வெறுமை);
நிலையான வெறுமை update_si_end(stateitem_T *sip, முழு startcol, முழு force);
நிலையான short *copy_id_list(short *list);
நிலையான முழு in_id_list(stateitem_T *item, short *cont_list, கட்டமைப்பு sp_syn *ssp, முழு flags);
நிலையான முழு தள்ளு_current_state(முழு idx);
நிலையான வெறுமை எடு_current_state(வெறுமை);
#ifdef FEAT_PROFILE
நிலையான வெறுமை syn_clear_time(syn_time_T *tt);
நிலையான வெறுமை syntime_clear(வெறுமை);
நிலையான வெறுமை syntime_report(வெறுமை);
நிலையான முழு syn_time_on = FALSE;
# define IF_SYN_TIME(p) (p)
#இல்லையெனில்
# define IF_SYN_TIME(p) வெற்று
வகைவரை முழு syn_time_T;
#endif

நிலையான வெறுமை syn_அடுக்கு_apply_changes_block(synblock_T *block, buf_T *buf);
நிலையான வெறுமை find_endpos(முழு idx, lpos_T *startpos, lpos_T *m_endpos, lpos_T *hl_endpos, long *flagsp, lpos_T *end_endpos, முழு *end_idx, reg_extmatch_T *start_ext);

நிலையான வெறுமை limit_pos(lpos_T *pos, lpos_T *limit);
நிலையான வெறுமை limit_pos_zero(lpos_T *pos, lpos_T *limit);
நிலையான வெறுமை syn_add_end_off(lpos_T *result, regmmatch_T *regmatch, synpat_T *spp, முழு idx, முழு extra);
நிலையான வெறுமை syn_add_start_off(lpos_T *result, regmmatch_T *regmatch, synpat_T *spp, முழு idx, முழு extra);
நிலையான char_u *syn_getcurline(வெறுமை);
நிலையான colnr_T syn_getcurline_len(வெறுமை);
நிலையான முழு syn_regexec(regmmatch_T *rmp, linenr_T lnum, colnr_T col, syn_time_T *st);
நிலையான முழு check_keyword_id(char_u *வரி, முழு startcol, முழு *endcol, long *flags, short **next_list, stateitem_T *cur_si, முழு *ccharp);
நிலையான வெறுமை syn_remove_pattern(synblock_T *block, முழு idx);
நிலையான வெறுமை syn_clear_pattern(synblock_T *block, முழு i);
நிலையான வெறுமை syn_clear_cluster(synblock_T *block, முழு i);
நிலையான வெறுமை syn_clear_one(முழு id, முழு syncing);
நிலையான வெறுமை syn_cmd_onoff(exarg_T *eap, எழுத்து *name);
நிலையான வெறுமை syn_வரிகள்_செய்தி(வெறுமை);
நிலையான வெறுமை syn_match_செய்தி(வெறுமை);
நிலையான வெறுமை syn_list_one(முழு id, முழு syncing, முழு link_only);
நிலையான வெறுமை syn_list_cluster(முழு id);
நிலையான வெறுமை put_id_list(char_u *name, short *list, முழு attr);
நிலையான வெறுமை put_pattern(எழுத்து *s, முழு c, synpat_T *spp, முழு attr);
நிலையான முழு syn_list_keywords(முழு id, hashtab_T *ht, முழு did_header, முழு attr);
நிலையான வெறுமை syn_clear_keyword(முழு id, hashtab_T *ht);
நிலையான வெறுமை clear_keywtab(hashtab_T *ht);
நிலையான முழு syn_scl_namen2id(char_u *linep, முழு len);
நிலையான முழு syn_check_cluster(char_u *pp, முழு len);
நிலையான முழு syn_add_cluster(char_u *name);
நிலையான வெறுமை init_syn_patterns(வெறுமை);
நிலையான char_u *get_syn_pattern(char_u *arg, synpat_T *ci);
நிலையான முழு get_id_list(char_u **arg, முழு keylen, short **list, முழு skip);
நிலையான வெறுமை syn_combine_list(short **clstr1, short **clstr2, முழு list_op);
நிலையான வெறுமை idl_cache_clear(synblock_T *block);

/*
 * Start the syntax recognition for a line.  This function is normally called
 * from the screen updating, once for each displayed line.
 * The buffer is remembered in syn_buf, because get_syntax_attr() doesn't get
 * it.	Careful: curbuf and curwin are likely to point to another buffer and
 * window.
 */
    வெறுமை
syntax_start(win_T *wp, linenr_T lnum)
{
    synstate_T	*p;
    synstate_T	*last_valid = வெற்று;
    synstate_T	*last_min_valid = வெற்று;
    synstate_T	*sp, *prev = வெற்று;
    linenr_T	parsed_lnum;
    linenr_T	first_stored;
    முழு		dist;
    நிலையான varnumber_T changedtick = 0;	// remember the last change ID

#ifdef FEAT_CONCEAL
    current_sub_char = NUL;
#endif

    /*
     * After switching buffers, invalidate current_state.
     * Also do this when a change was made, the current state may be invalid
     * then.
     */
    எனில் (syn_block != wp->w_s
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
    syn_அடுக்கு_alloc();
    எனில் (syn_block->b_sst_array == வெற்று)
	திரும்பு;		// out of memory
    syn_block->b_sst_lasttick = display_tick;

    /*
     * If the state of the end of the previous line is useful, store it.
     */
    எனில் (VALID_STATE(&current_state)
	    && current_lnum < lnum
	    && current_lnum < syn_buf->b_ml.ml_வரி_மொத்தம்)
    {
	(வெறுமை)syn_finish_வரி(FALSE);
	எனில் (!current_state_stored)
	{
	    ++current_lnum;
	    (வெறுமை)store_current_state();
	}

	/*
	 * If the current_lnum is now the same as "lnum", keep the current
	 * state (this happens very often!).  Otherwise invalidate
	 * current_state and figure it out below.
	 */
	எனில் (current_lnum != lnum)
	    invalidate_current_state();
    }
    இல்லையெனில்
	invalidate_current_state();

    /*
     * Try to synchronize from a saved state in b_sst_array[].
     * Only do this if lnum is not before and not to far beyond a saved state.
     */
    எனில் (INVALID_STATE(&current_state) && syn_block->b_sst_array != வெற்று)
    {
	// Find last valid saved state before start_lnum.
	FOR_ALL_SYNSTATES(syn_block, p)
	{
	    எனில் (p->sst_lnum > lnum)
		நிறுத்து;
	    எனில் (p->sst_lnum <= lnum && p->sst_change_lnum == 0)
	    {
		last_valid = p;
		எனில் (p->sst_lnum >= lnum - syn_block->b_syn_sync_minlines)
		    last_min_valid = p;
	    }
	}
	எனில் (last_min_valid != வெற்று)
	    load_current_state(last_min_valid);
    }

    /*
     * If "lnum" is before or far beyond a line with a saved state, need to
     * re-synchronize.
     */
    எனில் (INVALID_STATE(&current_state))
    {
	syn_sync(wp, lnum, last_valid);
	எனில் (current_lnum == 1)
	    // First line is always valid, no matter "minlines".
	    first_stored = 1;
	இல்லையெனில்
	    // Need to parse "minlines" lines before state can be considered
	    // valid to store.
	    first_stored = current_lnum + syn_block->b_syn_sync_minlines;
    }
    இல்லையெனில்
	first_stored = current_lnum;

    /*
     * Advance from the sync point or saved state until the current line.
     * Save some entries for syncing with later on.
     */
    எனில் (syn_block->b_sst_len <= Rows)
	dist = 999999;
    இல்லையெனில்
	dist = syn_buf->b_ml.ml_வரி_மொத்தம் / (syn_block->b_sst_len - Rows) + 1;
    போது (current_lnum < lnum)
    {
	syn_start_வரி();
	(வெறுமை)syn_finish_வரி(FALSE);
	++current_lnum;

	// If we parsed at least "minlines" lines or started at a valid
	// state, the current state is considered valid.
	எனில் (current_lnum >= first_stored)
	{
	    // Check if the saved state entry is for the current line and is
	    // equal to the current state.  If so, then validate all saved
	    // states that depended on a change before the parsed line.
	    எனில் (prev == வெற்று)
		prev = syn_அடுக்கு_find_entry(current_lnum - 1);
	    எனில் (prev == வெற்று)
		sp = syn_block->b_sst_first;
	    இல்லையெனில்
		sp = prev;
	    போது (sp != வெற்று && sp->sst_lnum < current_lnum)
		sp = sp->sst_next;
	    எனில் (sp != வெற்று
		    && sp->sst_lnum == current_lnum
		    && syn_அடுக்கு_equal(sp))
	    {
		parsed_lnum = current_lnum;
		prev = sp;
		போது (sp != வெற்று && sp->sst_change_lnum <= parsed_lnum)
		{
		    எனில் (sp->sst_lnum <= lnum)
			// valid state before desired line, use this one
			prev = sp;
		    இல்லையெனில் எனில் (sp->sst_change_lnum == 0)
			// past saved states depending on change, break here.
			நிறுத்து;
		    sp->sst_change_lnum = 0;
		    sp = sp->sst_next;
		}
		load_current_state(prev);
	    }
	    // Store the state at this line when it's the first one, the line
	    // where we start parsing, or some distance from the previously
	    // saved state.  But only when parsed at least 'minlines'.
	    இல்லையெனில் எனில் (prev == வெற்று
			|| current_lnum == lnum
			|| current_lnum >= prev->sst_lnum + dist)
		prev = store_current_state();
	}

	// This can take a long time: break when CTRL-C pressed.  The current
	// state will be wrong then.
	வரி_breakcheck();
	எனில் (got_int)
	{
	    current_lnum = lnum;
	    நிறுத்து;
	}
    }

    syn_start_வரி();
}

/*
 * We cannot simply discard growarrays full of state_items or buf_states; we
 * have to manually release their extmatch pointers first.
 */
    நிலையான வெறுமை
clear_syn_state(synstate_T *p)
{
    முழு		i;
    garray_T	*gap;

    எனில் (p->sst_stacksize > SST_FIX_STATES)
    {
	gap = &(p->sst_union.sst_ga);
	க்கு (i = 0; i < gap->ga_len; i++)
	    unref_extmatch(SYN_STATE_P(gap)[i].bs_extmatch);
	ga_clear(gap);
    }
    இல்லையெனில்
    {
	க்கு (i = 0; i < p->sst_stacksize; i++)
	    unref_extmatch(p->sst_union.sst_அடுக்கு[i].bs_extmatch);
    }
}

/*
 * Cleanup the current_state stack.
 */
    நிலையான வெறுமை
clear_current_state(வெறுமை)
{
    முழு		i;
    stateitem_T	*sip;

    sip = (stateitem_T *)(current_state.ga_data);
    க்கு (i = 0; i < current_state.ga_len; i++)
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
    நிலையான வெறுமை
syn_sync(
    win_T	*wp,
    linenr_T	start_lnum,
    synstate_T	*last_valid)
{
    buf_T	*curbuf_save;
    win_T	*curwin_save;
    pos_T	cursor_save;
    முழு		idx;
    linenr_T	lnum;
    linenr_T	end_lnum;
    linenr_T	break_lnum;
    முழு		had_sync_point;
    stateitem_T	*cur_si;
    synpat_T	*spp;
    முழு		found_flags = 0;
    முழு		found_match_idx = 0;
    linenr_T	found_current_lnum = 0;
    முழு		found_current_col = 0;
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
    எனில் (syn_block->b_syn_sync_minlines > start_lnum)
	start_lnum = 1;
    இல்லையெனில்
    {
	எனில் (syn_block->b_syn_sync_minlines == 1)
	    lnum = 1;
	இல்லையெனில் எனில் (syn_block->b_syn_sync_minlines < 10)
	    lnum = syn_block->b_syn_sync_minlines * 2;
	இல்லையெனில்
	    lnum = syn_block->b_syn_sync_minlines * 3 / 2;
	எனில் (syn_block->b_syn_sync_maxlines != 0
				     && lnum > syn_block->b_syn_sync_maxlines)
	    lnum = syn_block->b_syn_sync_maxlines;
	எனில் (lnum >= start_lnum)
	    start_lnum = 1;
	இல்லையெனில்
	    start_lnum -= lnum;
    }
    current_lnum = start_lnum;

    /*
     * 1. Search backwards for the end of a C-style comment.
     */
    எனில் (syn_block->b_syn_sync_flags & SF_CCOMMENT)
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
	க்கு ( ; start_lnum > 1; --start_lnum)
	{
	    char_u	*l = ml_get(start_lnum - 1);

	    எனில் (*l == NUL || *(l + ml_get_len(start_lnum - 1) - 1) != '\\')
		நிறுத்து;
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
	எனில் (find_start_comment((முழு)syn_block->b_syn_sync_maxlines) != வெற்று)
	{
	    க்கு (idx = syn_block->b_syn_patterns.ga_len; --idx >= 0; )
		எனில் (SYN_பொருட்கள்(syn_block)[idx].sp_syn.id
						   == syn_block->b_syn_sync_id
			&& SYN_பொருட்கள்(syn_block)[idx].sp_type == SPTYPE_START)
		{
		    validate_current_state();
		    எனில் (தள்ளு_current_state(idx) == OK)
			update_si_attr(current_state.ga_len - 1);
		    நிறுத்து;
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
    இல்லையெனில் எனில் (syn_block->b_syn_sync_flags & SF_MATCH)
    {
	எனில் (syn_block->b_syn_sync_maxlines != 0
			       && start_lnum > syn_block->b_syn_sync_maxlines)
	    break_lnum = start_lnum - syn_block->b_syn_sync_maxlines;
	இல்லையெனில்
	    break_lnum = 0;

	found_m_endpos.lnum = 0;
	found_m_endpos.col = 0;
	end_lnum = start_lnum;
	lnum = start_lnum;
	போது (--lnum > break_lnum)
	{
	    // This can take a long time: break when CTRL-C pressed.
	    வரி_breakcheck();
	    எனில் (got_int)
	    {
		invalidate_current_state();
		current_lnum = start_lnum;
		நிறுத்து;
	    }

	    // Check if we have run into a valid saved state stack now.
	    எனில் (last_valid != வெற்று && lnum == last_valid->sst_lnum)
	    {
		load_current_state(last_valid);
		நிறுத்து;
	    }

	    /*
	     * Check if the previous line has the line-continuation pattern.
	     */
	    எனில் (lnum > 1 && syn_match_linecont(lnum - 1))
		தொடர்;

	    /*
	     * Start with nothing on the state stack
	     */
	    validate_current_state();

	    க்கு (current_lnum = lnum; current_lnum < end_lnum; ++current_lnum)
	    {
		syn_start_வரி();
		க்கு (;;)
		{
		    had_sync_point = syn_finish_வரி(TRUE);
		    /*
		     * When a sync point has been found, remember where, and
		     * continue to look for another one, further on in the line.
		     */
		    எனில் (had_sync_point && current_state.ga_len)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			எனில் (cur_si->si_m_endpos.lnum > start_lnum)
			{
			    // ignore match that goes to after where started
			    current_lnum = end_lnum;
			    நிறுத்து;
			}
			எனில் (cur_si->si_idx < 0)
			{
			    // Cannot happen?
			    found_flags = 0;
			    found_match_idx = KEYWORD_IDX;
			}
			இல்லையெனில்
			{
			    spp = &(SYN_பொருட்கள்(syn_block)[cur_si->si_idx]);
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
			எனில் (found_m_endpos.lnum > current_lnum)
			{
			    current_lnum = found_m_endpos.lnum;
			    current_col = found_m_endpos.col;
			    எனில் (current_lnum >= end_lnum)
				நிறுத்து;
			}
			இல்லையெனில் எனில் (found_m_endpos.col > current_col)
			    current_col = found_m_endpos.col;
			இல்லையெனில்
			    ++current_col;

			// syn_current_attr() will have skipped the check for
			// an item that ends here, need to do that now.  Be
			// careful not to go past the NUL.
			prev_current_col = current_col;
			எனில் (syn_getcurline()[current_col] != NUL)
			    ++current_col;
			check_state_ends();
			current_col = prev_current_col;
		    }
		    இல்லையெனில்
			நிறுத்து;
		}
	    }

	    /*
	     * If a sync point was encountered, break here.
	     */
	    எனில் (found_flags)
	    {
		/*
		 * Put the item that was specified by the sync point on the
		 * state stack.  If there was no item specified, make the
		 * state stack empty.
		 */
		clear_current_state();
		எனில் (found_match_idx >= 0
			&& தள்ளு_current_state(found_match_idx) == OK)
		    update_si_attr(current_state.ga_len - 1);

		/*
		 * When using "grouphere", continue from the sync point
		 * match, until the end of the line.  Parsing starts at
		 * the next line.
		 * For "groupthere" the parsing starts at start_lnum.
		 */
		எனில் (found_flags & HL_SYNC_HERE)
		{
		    current_lnum = found_m_endpos.lnum;
		    current_col = found_m_endpos.col;
		    எனில் (current_state.ga_len)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			cur_si->si_h_startpos.lnum = found_current_lnum;
			cur_si->si_h_startpos.col = found_current_col;
			update_si_end(cur_si, (முழு)current_col, TRUE);
			check_keepend();
		    }
		    (வெறுமை)syn_finish_வரி(FALSE);
		    ++current_lnum;
		}
		இல்லையெனில்
		    current_lnum = start_lnum;

		நிறுத்து;
	    }

	    end_lnum = lnum;
	    invalidate_current_state();
	}

	// Ran into start of the file or exceeded maximum number of lines
	எனில் (lnum <= break_lnum)
	{
	    invalidate_current_state();
	    current_lnum = break_lnum + 1;
	}
    }

    validate_current_state();
}

    நிலையான வெறுமை
save_chartab(char_u *chartab)
{
    எனில் (syn_block->b_syn_isk == empty_option)
	திரும்பு;

    mch_memmove(chartab, syn_buf->b_chartab, (size_t)32);
    mch_memmove(syn_buf->b_chartab, syn_win->w_s->b_syn_chartab, (size_t)32);
}

    நிலையான வெறுமை
restore_chartab(char_u *chartab)
{
    எனில் (syn_win->w_s->b_syn_isk != empty_option)
	mch_memmove(syn_buf->b_chartab, chartab, (size_t)32);
}

/*
 * Return TRUE if the line-continuation pattern matches in line "lnum".
 */
    நிலையான முழு
syn_match_linecont(linenr_T lnum)
{
    regmmatch_T regmatch;
    முழு r;
    char_u	buf_chartab[32];  // chartab array for syn iskyeyword

    எனில் (syn_block->b_syn_linecont_prog == வெற்று)
	திரும்பு FALSE;

    // use syntax iskeyword option
    save_chartab(buf_chartab);
    regmatch.rmm_ic = syn_block->b_syn_linecont_ic;
    regmatch.regprog = syn_block->b_syn_linecont_prog;
    r = syn_regexec(&regmatch, lnum, (colnr_T)0,
	    IF_SYN_TIME(&syn_block->b_syn_linecont_time));
    syn_block->b_syn_linecont_prog = regmatch.regprog;
    restore_chartab(buf_chartab);
    திரும்பு r;
}

/*
 * Prepare the current state for the start of a line.
 */
    நிலையான வெறுமை
syn_start_வரி(வெறுமை)
{
    current_finished = FALSE;
    current_col = 0;

    /*
     * Need to update the end of a start/skip/end that continues from the
     * previous line and regions that have "keepend".
     */
    எனில் (current_state.ga_len > 0)
    {
	syn_update_ends(TRUE);
	check_state_ends();
    }

    next_match_idx = -1;
    ++current_வரி_id;
#ifdef FEAT_CONCEAL
    next_seqnr = 1;
#endif
}

/*
 * Check for items in the stack that need their end updated.
 * When "startofline" is TRUE the last item is always updated.
 * When "startofline" is FALSE the item with "keepend" is forcefully updated.
 */
    நிலையான வெறுமை
syn_update_ends(முழு startofline)
{
    stateitem_T	*cur_si;
    முழு		i;
    முழு		seen_keepend;

    எனில் (startofline)
    {
	// Check for a match carried over from a previous line with a
	// contained region.  The match ends as soon as the region ends.
	க்கு (i = 0; i < current_state.ga_len; ++i)
	{
	    cur_si = &CUR_STATE(i);
	    எனில் (cur_si->si_idx >= 0
		    && (SYN_பொருட்கள்(syn_block)[cur_si->si_idx]).sp_type
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
    எனில் (keepend_level >= 0)
	க்கு ( ; i > keepend_level; --i)
	    எனில் (CUR_STATE(i).si_flags & HL_EXTEND)
		நிறுத்து;

    seen_keepend = FALSE;
    க்கு ( ; i < current_state.ga_len; ++i)
    {
	cur_si = &CUR_STATE(i);
	எனில் ((cur_si->si_flags & HL_KEEPEND)
			    || (seen_keepend && !startofline)
			    || (i == current_state.ga_len - 1 && startofline))
	{
	    cur_si->si_h_startpos.col = 0;	// start highl. in col 0
	    cur_si->si_h_startpos.lnum = current_lnum;

	    எனில் (!(cur_si->si_flags & HL_MATCHCONT))
		update_si_end(cur_si, (முழு)current_col, !startofline);

	    எனில் (!startofline && (cur_si->si_flags & HL_KEEPEND))
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

    நிலையான வெறுமை
syn_அடுக்கு_free_block(synblock_T *block)
{
    synstate_T	*p;

    // Syntax definitions may have changed: drop the in_id_list() cache.
    idl_cache_clear(block);

    எனில் (block->b_sst_array == வெற்று)
	திரும்பு;

    FOR_ALL_SYNSTATES(block, p)
	clear_syn_state(p);
    VIM_CLEAR(block->b_sst_array);
    block->b_sst_first = வெற்று;
    block->b_sst_தேடல் = வெற்று;
    block->b_sst_len = 0;
}
/*
 * Free b_sst_array[] for buffer "buf".
 * Used when syntax items changed to force resyncing everywhere.
 */
    வெறுமை
syn_அடுக்கு_free_all(synblock_T *block)
{
#ifdef FEAT_FOLDING
    win_T	*wp;
#endif

    syn_அடுக்கு_free_block(block);

#ifdef FEAT_FOLDING
    // When using "syntax" fold method, must update all folds.
    FOR_ALL_WINDOWS(wp)
    {
	எனில் (wp->w_s == block && foldmethodIsSyntax(wp))
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
    நிலையான வெறுமை
syn_அடுக்கு_alloc(வெறுமை)
{
    long	len;
    synstate_T	*to, *from;
    synstate_T	*sstp;

    len = syn_buf->b_ml.ml_வரி_மொத்தம் / SST_DIST + Rows * 2;
    எனில் (len < SST_MIN_ENTRIES)
	len = SST_MIN_ENTRIES;
    இல்லையெனில் எனில் (len > SST_MAX_ENTRIES)
	len = SST_MAX_ENTRIES;
    எனில் (syn_block->b_sst_len > len * 2 || syn_block->b_sst_len < len)
    {
	// Allocate 50% too much, to avoid reallocating too often.
	len = syn_buf->b_ml.ml_வரி_மொத்தம்;
	len = (len + len / 2) / SST_DIST + Rows * 2;
	எனில் (len < SST_MIN_ENTRIES)
	    len = SST_MIN_ENTRIES;
	இல்லையெனில் எனில் (len > SST_MAX_ENTRIES)
	    len = SST_MAX_ENTRIES;

	எனில் (syn_block->b_sst_array != வெற்று)
	{
	    // When shrinking the array, cleanup the existing stack.
	    // Make sure that all valid entries fit in the new array.
	    போது (syn_block->b_sst_len - syn_block->b_sst_freecount + 2 > len
		    && syn_அடுக்கு_cleanup())
		;
	    எனில் (len < syn_block->b_sst_len - syn_block->b_sst_freecount + 2)
		len = syn_block->b_sst_len - syn_block->b_sst_freecount + 2;
	}

	sstp = ALLOC_CLEAR_MULT(synstate_T, len);
	எனில் (sstp == வெற்று)	// out of memory!
	    திரும்பு;

	to = sstp - 1;
	எனில் (syn_block->b_sst_array != வெற்று)
	{
	    // Move the states from the old array to the new one.
	    க்கு (from = syn_block->b_sst_first; from != வெற்று;
							from = from->sst_next)
	    {
		++to;
		*to = *from;
		to->sst_next = to + 1;
	    }
	}
	எனில் (to != sstp - 1)
	{
	    to->sst_next = வெற்று;
	    syn_block->b_sst_first = sstp;
	    syn_block->b_sst_freecount = len - (முழு)(to - sstp) - 1;
	}
	இல்லையெனில்
	{
	    syn_block->b_sst_first = வெற்று;
	    syn_block->b_sst_freecount = len;
	}

	// Create the list of free entries.
	syn_block->b_sst_firstfree = to + 1;
	போது (++to < sstp + len)
	    to->sst_next = to + 1;
	(sstp + len - 1)->sst_next = வெற்று;

	vim_free(syn_block->b_sst_array);
	syn_block->b_sst_array = sstp;
	syn_block->b_sst_len = len;
	// The entries were moved to a new array, drop the stale pointer.
	syn_block->b_sst_தேடல் = வெற்று;
    }
}

/*
 * Check for changes in a buffer to affect stored syntax states.  Uses the
 * b_mod_* fields.
 * Called from update_screen(), before screen is being updated, once for each
 * displayed buffer.
 */
    வெறுமை
syn_அடுக்கு_apply_changes(buf_T *buf)
{
    win_T	*wp;

    syn_அடுக்கு_apply_changes_block(&buf->b_s, buf);

    FOR_ALL_WINDOWS(wp)
    {
	எனில் ((wp->w_buffer == buf) && (wp->w_s != &buf->b_s))
	    syn_அடுக்கு_apply_changes_block(wp->w_s, buf);
    }
}

    நிலையான வெறுமை
syn_அடுக்கு_apply_changes_block(synblock_T *block, buf_T *buf)
{
    synstate_T	*p, *prev, *np;
    linenr_T	n;

    prev = வெற்று;
    க்கு (p = block->b_sst_first; p != வெற்று; )
    {
	எனில் (p->sst_lnum + block->b_syn_sync_linebreaks > buf->b_mod_உச்சி)
	{
	    n = p->sst_lnum + buf->b_mod_xlines;
	    எனில் (n <= buf->b_mod_bot)
	    {
		// this state is inside the changed area, remove it
		np = p->sst_next;
		எனில் (prev == வெற்று)
		    block->b_sst_first = np;
		இல்லையெனில்
		    prev->sst_next = np;
		syn_அடுக்கு_free_entry(block, p);
		p = np;
		தொடர்;
	    }
	    // This state is below the changed area.  Remember the line
	    // that needs to be parsed before this entry can be made valid
	    // again.
	    எனில் (p->sst_change_lnum != 0 && p->sst_change_lnum > buf->b_mod_உச்சி)
	    {
		எனில் (p->sst_change_lnum + buf->b_mod_xlines > buf->b_mod_உச்சி)
		    p->sst_change_lnum += buf->b_mod_xlines;
		இல்லையெனில்
		    p->sst_change_lnum = buf->b_mod_உச்சி;
	    }
	    எனில் (p->sst_change_lnum == 0
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
    நிலையான முழு
syn_அடுக்கு_cleanup(வெறுமை)
{
    synstate_T	*p, *prev;
    disptick_T	tick;
    முழு		above;
    முழு		dist;
    முழு		retval = FALSE;

    எனில் (syn_block->b_sst_first == வெற்று)
	திரும்பு retval;

    // Compute normal distance between non-displayed entries.
    எனில் (syn_block->b_sst_len <= Rows)
	dist = 999999;
    இல்லையெனில்
	dist = syn_buf->b_ml.ml_வரி_மொத்தம் / (syn_block->b_sst_len - Rows) + 1;

    /*
     * Go through the list to find the "tick" for the oldest entry that can
     * be removed.  Set "above" when the "tick" for the oldest entry is above
     * "b_sst_lasttick" (the display tick wraps around).
     */
    tick = syn_block->b_sst_lasttick;
    above = FALSE;
    prev = syn_block->b_sst_first;
    க்கு (p = prev->sst_next; p != வெற்று; prev = p, p = p->sst_next)
    {
	எனில் (prev->sst_lnum + dist > p->sst_lnum)
	{
	    எனில் (p->sst_tick > syn_block->b_sst_lasttick)
	    {
		எனில் (!above || p->sst_tick < tick)
		    tick = p->sst_tick;
		above = TRUE;
	    }
	    இல்லையெனில் எனில் (!above && p->sst_tick < tick)
		tick = p->sst_tick;
	}
    }

    /*
     * Go through the list to make the entries for the oldest tick at an
     * interval of several lines.
     */
    prev = syn_block->b_sst_first;
    க்கு (p = prev->sst_next; p != வெற்று; prev = p, p = p->sst_next)
    {
	எனில் (p->sst_tick == tick && prev->sst_lnum + dist > p->sst_lnum)
	{
	    // Move this entry from used list to free list
	    prev->sst_next = p->sst_next;
	    syn_அடுக்கு_free_entry(syn_block, p);
	    p = prev;
	    retval = TRUE;
	}
    }
    திரும்பு retval;
}

/*
 * Free the allocated memory for a syn_state item.
 * Move the entry into the free list.
 */
    நிலையான வெறுமை
syn_அடுக்கு_free_entry(synblock_T *block, synstate_T *p)
{
    எனில் (block->b_sst_தேடல் == p)
	block->b_sst_தேடல் = வெற்று;
    clear_syn_state(p);
    p->sst_next = block->b_sst_firstfree;
    block->b_sst_firstfree = p;
    ++block->b_sst_freecount;
}

/*
 * Find an entry in the list of state stacks at or before "lnum".
 * Returns NULL when there is no entry or the first entry is after "lnum".
 */
    நிலையான synstate_T *
syn_அடுக்கு_find_entry(linenr_T lnum)
{
    synstate_T	*p, *prev;

    prev = வெற்று;
    p = syn_block->b_sst_first;

    // The list is sorted by line number and lookups while parsing advance
    // monotonically, so resume from the last returned entry instead of
    // rescanning from the start whenever it is at or before "lnum".
    எனில் (syn_block->b_sst_தேடல் != வெற்று
				 && syn_block->b_sst_தேடல்->sst_lnum <= lnum)
	p = syn_block->b_sst_தேடல்;

    க்கு ( ; p != வெற்று; prev = p, p = p->sst_next)
    {
	எனில் (p->sst_lnum == lnum)
	{
	    syn_block->b_sst_தேடல் = p;
	    திரும்பு p;
	}
	எனில் (p->sst_lnum > lnum)
	    நிறுத்து;
    }
    எனில் (prev != வெற்று)
	syn_block->b_sst_தேடல் = prev;
    திரும்பு prev;
}

/*
 * Try saving the current state in b_sst_array[].
 * The current state must be valid for the start of the current_lnum line!
 */
    நிலையான synstate_T *
store_current_state(வெறுமை)
{
    முழு		i;
    synstate_T	*p;
    bufstate_T	*bp;
    stateitem_T	*cur_si;
    synstate_T	*sp = syn_அடுக்கு_find_entry(current_lnum);

    /*
     * If the current state contains a start or end pattern that continues
     * from the previous line, we can't use it.  Don't store it then.
     */
    க்கு (i = current_state.ga_len - 1; i >= 0; --i)
    {
	cur_si = &CUR_STATE(i);
	எனில் (cur_si->si_h_startpos.lnum >= current_lnum
		|| cur_si->si_m_endpos.lnum >= current_lnum
		|| cur_si->si_h_endpos.lnum >= current_lnum
		|| (cur_si->si_end_idx
		    && cur_si->si_eoe_pos.lnum >= current_lnum))
	    நிறுத்து;
    }
    எனில் (i >= 0)
    {
	எனில் (sp != வெற்று)
	{
	    // find "sp" in the list and remove it
	    எனில் (syn_block->b_sst_first == sp)
		// it's the first entry
		syn_block->b_sst_first = sp->sst_next;
	    இல்லையெனில்
	    {
		// find the entry just before this one to adjust sst_next
		FOR_ALL_SYNSTATES(syn_block, p)
		    எனில் (p->sst_next == sp)
			நிறுத்து;
		எனில் (p != வெற்று)	// just in case
		    p->sst_next = sp->sst_next;
	    }
	    syn_அடுக்கு_free_entry(syn_block, sp);
	    sp = வெற்று;
	}
    }
    இல்லையெனில் எனில் (sp == வெற்று || sp->sst_lnum != current_lnum)
    {
	/*
	 * Add a new entry
	 */
	// If no free items, cleanup the array first.
	எனில் (syn_block->b_sst_freecount == 0)
	{
	    (வெறுமை)syn_அடுக்கு_cleanup();
	    // "sp" may have been moved to the freelist now
	    sp = syn_அடுக்கு_find_entry(current_lnum);
	}
	// Still no free items?  Must be a strange problem...
	எனில் (syn_block->b_sst_freecount == 0)
	    sp = வெற்று;
	இல்லையெனில்
	{
	    // Take the first item from the free list and put it in the used
	    // list, after *sp
	    p = syn_block->b_sst_firstfree;
	    syn_block->b_sst_firstfree = p->sst_next;
	    --syn_block->b_sst_freecount;
	    எனில் (sp == வெற்று)
	    {
		// Insert in front of the list
		p->sst_next = syn_block->b_sst_first;
		syn_block->b_sst_first = p;
	    }
	    இல்லையெனில்
	    {
		// insert in list after *sp
		p->sst_next = sp->sst_next;
		sp->sst_next = p;
	    }
	    sp = p;
	    sp->sst_stacksize = 0;
	    sp->sst_lnum = current_lnum;
	    // Resume the next forward lookup from the entry just stored.
	    syn_block->b_sst_தேடல் = sp;
	}
    }
    எனில் (sp != வெற்று)
    {
	// When overwriting an existing state stack, clear it first
	clear_syn_state(sp);
	sp->sst_stacksize = current_state.ga_len;
	எனில் (current_state.ga_len > SST_FIX_STATES)
	{
	    // Need to clear it, might be something remaining from when the
	    // length was less than SST_FIX_STATES.
	    ga_init2(&sp->sst_union.sst_ga, அளவு(bufstate_T), 1);
	    எனில் (ga_grow(&sp->sst_union.sst_ga, current_state.ga_len) == FAIL)
		sp->sst_stacksize = 0;
	    இல்லையெனில்
		sp->sst_union.sst_ga.ga_len = current_state.ga_len;
	    bp = SYN_STATE_P(&(sp->sst_union.sst_ga));
	}
	இல்லையெனில்
	    bp = sp->sst_union.sst_அடுக்கு;
	க்கு (i = 0; i < sp->sst_stacksize; ++i)
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
    திரும்பு sp;
}

/*
 * Copy a state stack from "from" in b_sst_array[] to current_state;
 */
    நிலையான வெறுமை
load_current_state(synstate_T *from)
{
    முழு		i;
    bufstate_T	*bp;

    clear_current_state();
    validate_current_state();
    keepend_level = -1;
    எனில் (from->sst_stacksize
	    && ga_grow(&current_state, from->sst_stacksize) == OK)
    {
	எனில் (from->sst_stacksize > SST_FIX_STATES)
	    bp = SYN_STATE_P(&(from->sst_union.sst_ga));
	இல்லையெனில்
	    bp = from->sst_union.sst_அடுக்கு;
	க்கு (i = 0; i < from->sst_stacksize; ++i)
	{
	    CUR_STATE(i).si_idx = bp[i].bs_idx;
	    CUR_STATE(i).si_flags = bp[i].bs_flags;
#ifdef FEAT_CONCEAL
	    CUR_STATE(i).si_seqnr = bp[i].bs_seqnr;
	    CUR_STATE(i).si_cchar = bp[i].bs_cchar;
#endif
	    CUR_STATE(i).si_extmatch = ref_extmatch(bp[i].bs_extmatch);
	    எனில் (keepend_level < 0 && (CUR_STATE(i).si_flags & HL_KEEPEND))
		keepend_level = i;
	    CUR_STATE(i).si_ends = FALSE;
	    CUR_STATE(i).si_m_lnum = 0;
	    எனில் (CUR_STATE(i).si_idx >= 0)
		CUR_STATE(i).si_next_list =
		     (SYN_பொருட்கள்(syn_block)[CUR_STATE(i).si_idx]).sp_next_list;
	    இல்லையெனில்
		CUR_STATE(i).si_next_list = வெற்று;
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
    நிலையான முழு
syn_அடுக்கு_equal(synstate_T *sp)
{
    முழு		i, j;
    bufstate_T	*bp;
    reg_extmatch_T	*six, *bsx;

    // First a quick check if the stacks have the same size end nextlist.
    எனில் (sp->sst_stacksize != current_state.ga_len
	    || sp->sst_next_list != current_next_list)
	திரும்பு FALSE;

    // Need to compare all states on both stacks.
    எனில் (sp->sst_stacksize > SST_FIX_STATES)
	bp = SYN_STATE_P(&(sp->sst_union.sst_ga));
    இல்லையெனில்
	bp = sp->sst_union.sst_அடுக்கு;

    க்கு (i = current_state.ga_len; --i >= 0; )
    {
	// If the item has another index the state is different.
	எனில் (bp[i].bs_idx != CUR_STATE(i).si_idx)
	    நிறுத்து;
	எனில் (bp[i].bs_extmatch == CUR_STATE(i).si_extmatch)
	    தொடர்;
	// When the extmatch pointers are different, the strings in them can
	// still be the same.  Check if the extmatch references are equal.
	bsx = bp[i].bs_extmatch;
	six = CUR_STATE(i).si_extmatch;
	// If one of the extmatch pointers is NULL the states are different.
	எனில் (bsx == வெற்று || six == வெற்று)
	    நிறுத்து;
	க்கு (j = 0; j < NSUBEXP; ++j)
	{
	    // Check each referenced match string. They must all be equal.
	    எனில் (bsx->matches[j] != six->matches[j])
	    {
		// If the pointer is different it can still be the same text.
		// Compare the strings, ignore case when the start item has the
		// sp_ic flag set.
		எனில் (bsx->matches[j] == வெற்று || six->matches[j] == வெற்று)
		    நிறுத்து;
		எனில் ((SYN_பொருட்கள்(syn_block)[CUR_STATE(i).si_idx]).sp_ic
			? MB_STRICMP(bsx->matches[j], six->matches[j]) != 0
			: STRCMP(bsx->matches[j], six->matches[j]) != 0)
		    நிறுத்து;
	    }
	}
	எனில் (j != NSUBEXP)
	    நிறுத்து;
    }
    திரும்பு i < 0 ? TRUE : FALSE;
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
    வெறுமை
syntax_end_parsing(win_T *wp, linenr_T lnum)
{
    synstate_T	*sp;

    எனில் (syn_block != wp->w_s)
	திரும்பு;  // not the right window
    sp = syn_அடுக்கு_find_entry(lnum);
    எனில் (sp != வெற்று && sp->sst_lnum < lnum)
	sp = sp->sst_next;

    எனில் (sp != வெற்று && sp->sst_change_lnum != 0)
	sp->sst_change_lnum = lnum;
}

/*
 * End of handling of the state stack.
 ****************************************/

    நிலையான வெறுமை
invalidate_current_state(வெறுமை)
{
    clear_current_state();
    current_state.ga_itemsize = 0;	// mark current_state invalid
    current_next_list = வெற்று;
    keepend_level = -1;
}

    நிலையான வெறுமை
validate_current_state(வெறுமை)
{
    current_state.ga_itemsize = அளவு(stateitem_T);
    current_state.ga_growsize = 3;
}

/*
 * Return TRUE if the syntax at start of lnum changed since last time.
 * This will only be called just after get_syntax_attr() for the previous
 * line, to check if the next line needs to be redrawn too.
 */
    முழு
syntax_check_changed(linenr_T lnum)
{
    முழு		retval = TRUE;
    synstate_T	*sp;

    /*
     * Check the state stack when:
     * - lnum is just below the previously syntaxed line.
     * - lnum is not before the lines with saved states.
     * - lnum is not past the lines with saved states.
     * - lnum is at or before the last changed line.
     */
    எனில் (VALID_STATE(&current_state) && lnum == current_lnum + 1)
    {
	sp = syn_அடுக்கு_find_entry(lnum);
	எனில் (sp != வெற்று && sp->sst_lnum == lnum)
	{
	    /*
	     * finish the previous line (needed when not all of the line was
	     * drawn)
	     */
	    (வெறுமை)syn_finish_வரி(FALSE);

	    /*
	     * Compare the current state with the previously saved state of
	     * the line.
	     */
	    எனில் (syn_அடுக்கு_equal(sp))
		retval = FALSE;

	    /*
	     * Store the current state in b_sst_array[] for later use.
	     */
	    ++current_lnum;
	    (வெறுமை)store_current_state();
	}
    }

    திரும்பு retval;
}

/*
 * Finish the current line.
 * This doesn't return any attributes, it only gets the state at the end of
 * the line.  It can start anywhere in the line, as long as the current state
 * is valid.
 */
    நிலையான முழு
syn_finish_வரி(
    முழு	    syncing)		// called for syncing
{
    stateitem_T	*cur_si;
    colnr_T	prev_current_col;

    போது (!current_finished)
    {
	(வெறுமை)syn_current_attr(syncing, FALSE, வெற்று, FALSE);
	/*
	 * When syncing, and found some item, need to check the item.
	 */
	எனில் (syncing && current_state.ga_len)
	{
	    /*
	     * Check for match with sync item.
	     */
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	    எனில் (cur_si->si_idx >= 0
		    && (SYN_பொருட்கள்(syn_block)[cur_si->si_idx].sp_flags
					  & (HL_SYNC_HERE|HL_SYNC_THERE)))
		திரும்பு TRUE;

	    // syn_current_attr() will have skipped the check for an item
	    // that ends here, need to do that now.  Be careful not to go
	    // past the NUL.
	    prev_current_col = current_col;
	    எனில் (syn_getcurline()[current_col] != NUL)
		++current_col;
	    check_state_ends();
	    current_col = prev_current_col;
	}
	++current_col;
    }
    திரும்பு FALSE;
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
    முழு
get_syntax_attr(
    colnr_T	col,
    முழு		*can_spell,
    முழு		keep_state)	// keep state of char at "col"
{
    முழு	    attr = 0;

    எனில் (can_spell != வெற்று)
	// Default: Only do spelling when there is no @Spell cluster or when
	// ":syn spell toplevel" was used.
	*can_spell = syn_block->b_syn_spell == SYNSPL_DEFAULT
		    ? (syn_block->b_spell_cluster_id == 0)
		    : (syn_block->b_syn_spell == SYNSPL_உச்சி);

    // check for out of memory situation
    எனில் (syn_block->b_sst_array == வெற்று)
	திரும்பு 0;

    // After 'synmaxcol' the attribute is always zero.
    எனில் (syn_buf->b_p_smc > 0 && col >= (colnr_T)syn_buf->b_p_smc)
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
	திரும்பு 0;
    }

    // Make sure current_state is valid
    எனில் (INVALID_STATE(&current_state))
	validate_current_state();

    /*
     * Skip from the current column to "col", get the attributes for "col".
     */
    போது (current_col <= col)
    {
	attr = syn_current_attr(FALSE, TRUE, can_spell,
				     current_col == col ? keep_state : FALSE);
	++current_col;
    }

    திரும்பு attr;
}

/*
 * Get syntax attributes for current_lnum, current_col.
 */
    நிலையான முழு
syn_current_attr(
    முழு		syncing,		// When 1: called for syncing
    முழு		displaying,		// result will be displayed
    முழு		*can_spell,		// return: do spell checking
    முழு		keep_state)		// keep syntax stack afterwards
{
    முழு		syn_id;
    lpos_T	endpos;		// was: char_u *endp;
    lpos_T	hl_startpos;	// was: int hl_startcol;
    lpos_T	hl_endpos;
    lpos_T	eos_pos;	// end-of-start match (start region)
    lpos_T	eoe_pos;	// end-of-end pattern
    முழு		end_idx;	// group ID for end pattern
    முழு		idx;
    synpat_T	*spp;
    stateitem_T	*cur_si, *sip = வெற்று;
    முழு		startcol;
    முழு		endcol;
    long	flags;
    முழு		cchar;
    short	*next_list;
    முழு		found_match;		    // found usable match
    நிலையான முழு	try_next_column = FALSE;    // must try in next col
    முழு		do_keywords;
    regmmatch_T	regmatch;
    lpos_T	pos;
    முழு		lc_col;
    reg_extmatch_T *cur_extmatch = வெற்று;
    char_u	buf_chartab[32];  // chartab array for syn iskyeyword
    char_u	*வரி;		// current line.  NOTE: becomes invalid after
				// looking for a pattern match!

    // variables for zero-width matches that have a "nextgroup" argument
    முழு		keep_next_list;
    முழு		zero_width_next_list = FALSE;
    garray_T	zero_width_next_ga;

    /*
     * No character, no attributes!  Past end of line?
     * Do try matching with an empty line (could be the start of a region).
     */
    வரி = syn_getcurline();
    எனில் (வரி[current_col] == NUL && current_col != 0)
    {
	/*
	 * If we found a match after the last column, use it.
	 */
	எனில் (next_match_idx >= 0 && next_match_col >= (முழு)current_col
						  && next_match_col != MAXCOL)
	    (வெறுமை)தள்ளு_next_match(வெற்று);

	current_finished = TRUE;
	current_state_stored = FALSE;
	திரும்பு 0;
    }

    // if the current or next character is NUL, we will finish the line now
    எனில் (வரி[current_col] == NUL || வரி[current_col + 1] == NUL)
    {
	current_finished = TRUE;
	current_state_stored = FALSE;
    }

    /*
     * When in the previous column there was a match but it could not be used
     * (empty match or already matched in this column) need to try again in
     * the next column.
     */
    எனில் (try_next_column)
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
    ga_init2(&zero_width_next_ga, அளவு(முழு), 10);

    // use syntax iskeyword option
    save_chartab(buf_chartab);

    /*
     * Repeat matching keywords and patterns, to find contained items at the
     * same column.  This stops when there are no extra matches at the current
     * column.
     */
    செய்
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
	எனில் (current_state.ga_len)
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	இல்லையெனில்
	    cur_si = வெற்று;

	எனில் (syn_block->b_syn_containedin || cur_si == வெற்று
					      || cur_si->si_cont_list != வெற்று)
	{
	    /*
	     * 2. Check for keywords, if on a keyword char after a non-keyword
	     *	  char.  Don't do this when syncing.
	     */
	    எனில் (do_keywords)
	    {
	      வரி = syn_getcurline();
	      எனில் (vim_iswordp_buf(வரி + current_col, syn_buf)
		      && (current_col == 0
			  || !vim_iswordp_buf(வரி + current_col - 1
			      - (has_mbyte
				  ? (*mb_head_off)(வரி, வரி + current_col - 1)
				  : 0) , syn_buf)))
	      {
		syn_id = check_keyword_id(வரி, (முழு)current_col,
					 &endcol, &flags, &next_list, cur_si,
					 &cchar);
		எனில் (syn_id != 0)
		{
		    எனில் (தள்ளு_current_state(KEYWORD_IDX) == OK)
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
			எனில் (current_state.ga_len > 1)
			    cur_si->si_flags |=
				  CUR_STATE(current_state.ga_len - 2).si_flags
								 & HL_CONCEAL;
#endif
			cur_si->si_id = syn_id;
			cur_si->si_trans_id = syn_id;
			எனில் (flags & HL_TRANSP)
			{
			    எனில் (current_state.ga_len < 2)
			    {
				cur_si->si_attr = 0;
				cur_si->si_trans_id = 0;
			    }
			    இல்லையெனில்
			    {
				cur_si->si_attr = CUR_STATE(
					current_state.ga_len - 2).si_attr;
				cur_si->si_trans_id = CUR_STATE(
					current_state.ga_len - 2).si_trans_id;
			    }
			}
			இல்லையெனில்
			    cur_si->si_attr = syn_id2attr(syn_id);
			cur_si->si_cont_list = வெற்று;
			cur_si->si_next_list = next_list;
			check_keepend();
		    }
		    இல்லையெனில்
			vim_free(next_list);
		}
	      }
	    }

	    /*
	     * 3. Check for patterns (only if no keyword found).
	     */
	    எனில் (syn_id == 0 && syn_block->b_syn_patterns.ga_len)
	    {
		/*
		 * If we didn't check for a match yet, or we are past it, check
		 * for any match with a pattern.
		 */
		எனில் (next_match_idx < 0 || next_match_col < (முழு)current_col)
		{
		    /*
		     * Check all relevant patterns for a match at this
		     * position.  This is complicated, because matching with a
		     * pattern takes quite a bit of time, thus we want to
		     * avoid doing it when it's not needed.
		     */
		    next_match_idx = 0;		// no match in this line yet
		    next_match_col = MAXCOL;
		    க்கு (idx = syn_block->b_syn_patterns.ga_len; --idx >= 0; )
		    {
			spp = &(SYN_பொருட்கள்(syn_block)[idx]);
			எனில் (	   spp->sp_syncing == syncing
				&& (displaying || !(spp->sp_flags & HL_DISPLAY))
				&& (spp->sp_type == SPTYPE_MATCH
				    || spp->sp_type == SPTYPE_START)
				&& (current_next_list != வெற்று
				    ? in_id_list(வெற்று, current_next_list,
							      &spp->sp_syn, 0)
				    : (cur_si == வெற்று
					? !(spp->sp_flags & HL_CONTAINED)
					: in_id_list(cur_si,
					    cur_si->si_cont_list, &spp->sp_syn,
					    spp->sp_flags))))
			{
			    முழு r;

			    // If we already tried matching in this line, and
			    // there isn't a match before next_match_col, skip
			    // this item.
			    எனில் (spp->sp_வரி_id == current_வரி_id
				    && spp->sp_startcol >= next_match_col)
				தொடர்;
			    spp->sp_வரி_id = current_வரி_id;

			    lc_col = current_col - spp->sp_offsets[SPO_LC_OFF];
			    எனில் (lc_col < 0)
				lc_col = 0;

			    regmatch.rmm_ic = spp->sp_ic;
			    regmatch.regprog = spp->sp_prog;
			    r = syn_regexec(&regmatch,
					     current_lnum,
					     (colnr_T)lc_col,
					     IF_SYN_TIME(&spp->sp_time));
			    spp->sp_prog = regmatch.regprog;
			    எனில் (!r)
			    {
				// no match in this line, try another one
				spp->sp_startcol = MAXCOL;
				தொடர்;
			    }

			    /*
			     * Compute the first column of the match.
			     */
			    syn_add_start_off(&pos, &regmatch,
							 spp, SPO_MS_OFF, -1);
			    எனில் (pos.lnum > current_lnum)
			    {
				// must have used end of match in a next line,
				// we can't handle that
				spp->sp_startcol = MAXCOL;
				தொடர்;
			    }
			    startcol = pos.col;

			    // remember the next column where this pattern
			    // matches in the current line
			    spp->sp_startcol = startcol;

			    /*
			     * If a previously found match starts at a lower
			     * column number, don't use this one.
			     */
			    எனில் (startcol >= next_match_col)
				தொடர்;

			    /*
			     * If we matched this pattern at this position
			     * before, skip it.  Must retry in the next
			     * column, because it may match from there.
			     */
			    எனில் (did_match_already(idx, &zero_width_next_ga))
			    {
				try_next_column = TRUE;
				தொடர்;
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
			    re_extmatch_out = வெற்று;

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
			    எனில் (spp->sp_type == SPTYPE_START
					      && (spp->sp_flags & HL_ONELINE))
			    {
				lpos_T	startpos;

				startpos = endpos;
				find_endpos(idx, &startpos, &endpos, &hl_endpos,
				    &flags, &eoe_pos, &end_idx, cur_extmatch);
				எனில் (endpos.lnum == 0)
				    தொடர்;	    // not found
			    }

			    /*
			     * For a "match" the size must be > 0 after the
			     * end offset needs has been added.  Except when
			     * syncing.
			     */
			    இல்லையெனில் எனில் (spp->sp_type == SPTYPE_MATCH)
			    {
				syn_add_end_off(&hl_endpos, &regmatch, spp,
							       SPO_HE_OFF, 0);
				syn_add_end_off(&endpos, &regmatch, spp,
							       SPO_ME_OFF, 0);
				எனில் (endpos.lnum == current_lnum
				      && (முழு)endpos.col + syncing < startcol)
				{
				    /*
				     * If an empty string is matched, may need
				     * to try matching again at next column.
				     */
				    எனில் (regmatch.startpos[0].col
						    == regmatch.endpos[0].col)
					try_next_column = TRUE;
				    தொடர்;
				}
			    }

			    /*
			     * keep the best match so far in next_match_*
			     */
			    // Highlighting must start after startpos and end
			    // before endpos.
			    எனில் (hl_startpos.lnum == current_lnum
					   && (முழு)hl_startpos.col < startcol)
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
			    cur_extmatch = வெற்று;
			}
		    }
		}

		/*
		 * If we found a match at the current column, use it.
		 */
		எனில் (next_match_idx >= 0 && next_match_col == (முழு)current_col)
		{
		    synpat_T	*lspp;

		    // When a zero-width item matched which has a nextgroup,
		    // don't push the item but set nextgroup.
		    lspp = &(SYN_பொருட்கள்(syn_block)[next_match_idx]);
		    எனில் (next_match_m_endpos.lnum == current_lnum
			    && next_match_m_endpos.col == current_col
			    && lspp->sp_next_list != வெற்று)
		    {
			current_next_list = lspp->sp_next_list;
			current_next_flags = lspp->sp_flags;
			keep_next_list = TRUE;
			zero_width_next_list = TRUE;

			// Add the index to a list, so that we can check
			// later that we don't match it again (and cause an
			// endless loop).
			எனில் (ga_grow(&zero_width_next_ga, 1) == OK)
			{
			    ((முழு *)(zero_width_next_ga.ga_data))
				[zero_width_next_ga.ga_len++] = next_match_idx;
			}
			next_match_idx = -1;
		    }
		    இல்லையெனில்
			cur_si = தள்ளு_next_match(cur_si);
		    found_match = TRUE;
		}
	    }
	}

	/*
	 * Handle searching for nextgroup match.
	 */
	எனில் (current_next_list != வெற்று && !keep_next_list)
	{
	    /*
	     * If a nextgroup was not found, continue looking for one if:
	     * - this is an empty line and the "skipempty" option was given
	     * - we are on white space and the "skipwhite" option was given
	     */
	    எனில் (!found_match)
	    {
		வரி = syn_getcurline();
		எனில் (((current_next_flags & HL_SKIPWHITE)
			    && VIM_ISWHITE(வரி[current_col]))
			|| ((current_next_flags & HL_SKIPEMPTY)
			    && *வரி == NUL))
		    நிறுத்து;
	    }

	    /*
	     * If a nextgroup was found: Use it, and continue looking for
	     * contained matches.
	     * If a nextgroup was not found: Continue looking for a normal
	     * match.
	     * When did set current_next_list for a zero-width item and no
	     * match was found don't loop (would get stuck).
	     */
	    current_next_list = வெற்று;
	    next_match_idx = -1;
	    எனில் (!zero_width_next_list)
		found_match = TRUE;
	}

    } போது (found_match);

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
    எனில் (cur_si != வெற்று)
    {
#ifndef FEAT_EVAL
	முழு	current_trans_id = 0;
#endif
	க்கு (idx = current_state.ga_len - 1; idx >= 0; --idx)
	{
	    sip = &CUR_STATE(idx);
	    எனில் ((current_lnum > sip->si_h_startpos.lnum
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
		நிறுத்து;
	    }
	}

	எனில் (can_spell != வெற்று)
	{
	    கட்டமைப்பு sp_syn   sps;

	    /*
	     * set "can_spell" to TRUE if spell checking is supposed to be
	     * done in the current item.
	     */
	    எனில் (syn_block->b_spell_cluster_id == 0)
	    {
		// There is no @Spell cluster: Do spelling for items without
		// @NoSpell cluster.
		எனில் (syn_block->b_nospell_cluster_id == 0
						     || current_trans_id == 0)
		    *can_spell = (syn_block->b_syn_spell != SYNSPL_NOTOP);
		இல்லையெனில்
		{
		    sps.inc_tag = 0;
		    sps.id = syn_block->b_nospell_cluster_id;
		    sps.cont_in_list = வெற்று;
		    *can_spell = !in_id_list(sip, sip->si_cont_list, &sps, 0);
		}
	    }
	    இல்லையெனில்
	    {
		// The @Spell cluster is defined: Do spelling in items with
		// the @Spell cluster.  But not when @NoSpell is also there.
		// At the toplevel only spell check when ":syn spell toplevel"
		// was used.
		எனில் (current_trans_id == 0)
		    *can_spell = (syn_block->b_syn_spell == SYNSPL_உச்சி);
		இல்லையெனில்
		{
		    sps.inc_tag = 0;
		    sps.id = syn_block->b_spell_cluster_id;
		    sps.cont_in_list = வெற்று;
		    *can_spell = in_id_list(sip, sip->si_cont_list, &sps, 0);

		    எனில் (syn_block->b_nospell_cluster_id != 0)
		    {
			sps.id = syn_block->b_nospell_cluster_id;
			எனில் (in_id_list(sip, sip->si_cont_list, &sps, 0))
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
	எனில் (!syncing && !keep_state)
	{
	    check_state_ends();
	    எனில் (current_state.ga_len > 0
				      && syn_getcurline()[current_col] != NUL)
	    {
		++current_col;
		check_state_ends();
		--current_col;
	    }
	}
    }
    இல்லையெனில் எனில் (can_spell != வெற்று)
	// Default: Only do spelling when there is no @Spell cluster or when
	// ":syn spell toplevel" was used.
	*can_spell = syn_block->b_syn_spell == SYNSPL_DEFAULT
		    ? (syn_block->b_spell_cluster_id == 0)
		    : (syn_block->b_syn_spell == SYNSPL_உச்சி);

    // nextgroup ends at end of line, unless "skipnl" or "skipempty" present
    எனில் (current_next_list != வெற்று
	    && (வரி = syn_getcurline())[current_col] != NUL
	    && வரி[current_col + 1] == NUL
	    && !(current_next_flags & (HL_SKIPNL | HL_SKIPEMPTY)))
	current_next_list = வெற்று;

    எனில் (zero_width_next_ga.ga_len > 0)
	ga_clear(&zero_width_next_ga);

    // No longer need external matches.  But keep next_match_extmatch.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = வெற்று;
    unref_extmatch(cur_extmatch);

    திரும்பு current_attr;
}


/*
 * Check if we already matched pattern "idx" at the current column.
 */
    நிலையான முழு
did_match_already(முழு idx, garray_T *gap)
{
    முழு		i;

    க்கு (i = current_state.ga_len; --i >= 0; )
	எனில் (CUR_STATE(i).si_m_startcol == (முழு)current_col
		&& CUR_STATE(i).si_m_lnum == (முழு)current_lnum
		&& CUR_STATE(i).si_idx == idx)
	    திரும்பு TRUE;

    // Zero-width matches with a nextgroup argument are not put on the syntax
    // stack, and can only be matched once anyway.
    க்கு (i = gap->ga_len; --i >= 0; )
	எனில் (((முழு *)(gap->ga_data))[i] == idx)
	    திரும்பு TRUE;

    திரும்பு FALSE;
}

/*
 * Push the next match onto the stack.
 */
    நிலையான stateitem_T *
தள்ளு_next_match(stateitem_T *cur_si)
{
    synpat_T	*spp;
#ifdef FEAT_CONCEAL
    முழு		 save_flags;
#endif

    spp = &(SYN_பொருட்கள்(syn_block)[next_match_idx]);

    /*
     * Push the item in current_state stack;
     */
    எனில் (தள்ளு_current_state(next_match_idx) == OK)
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
	எனில் (current_state.ga_len > 1)
	    cur_si->si_flags |=
		    CUR_STATE(current_state.ga_len - 2).si_flags & HL_CONCEAL;
#endif
	cur_si->si_next_list = spp->sp_next_list;
	cur_si->si_extmatch = ref_extmatch(next_match_extmatch);
	எனில் (spp->sp_type == SPTYPE_START && !(spp->sp_flags & HL_ONELINE))
	{
	    // Try to find the end pattern in the current line
	    update_si_end(cur_si, (முழு)(next_match_m_endpos.col), TRUE);
	    check_keepend();
	}
	இல்லையெனில்
	{
	    cur_si->si_m_endpos = next_match_m_endpos;
	    cur_si->si_h_endpos = next_match_h_endpos;
	    cur_si->si_ends = TRUE;
	    cur_si->si_flags |= next_match_flags;
	    cur_si->si_eoe_pos = next_match_eoe_pos;
	    cur_si->si_end_idx = next_match_end_idx;
	}
	எனில் (keepend_level < 0 && (cur_si->si_flags & HL_KEEPEND))
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
	எனில் (	   spp->sp_type == SPTYPE_START
		&& spp->sp_syn_match_id != 0
		&& தள்ளு_current_state(next_match_idx) == OK)
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
	    எனில் (cur_si->si_flags & HL_CONCEALENDS)
		cur_si->si_flags |= HL_CONCEAL;
#endif
	    cur_si->si_next_list = வெற்று;
	    check_keepend();
	    update_si_attr(current_state.ga_len - 1);
	}
    }

    next_match_idx = -1;	// try other match next time

    திரும்பு cur_si;
}

/*
 * Check for end of current state (and the states before it).
 */
    நிலையான வெறுமை
check_state_ends(வெறுமை)
{
    stateitem_T	*cur_si;
    முழு		had_extend;

    cur_si = &CUR_STATE(current_state.ga_len - 1);
    க்கு (;;)
    {
	எனில் (cur_si->si_ends
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
	    எனில் (cur_si->si_end_idx
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
		எனில் (cur_si->si_flags & HL_CONCEALENDS)
		    cur_si->si_flags |= HL_CONCEAL;
#endif
		update_si_attr(current_state.ga_len - 1);

		// nextgroup= should not match in the end pattern
		current_next_list = வெற்று;

		// what matches next may be different now, clear it
		next_match_idx = 0;
		next_match_col = MAXCOL;
		நிறுத்து;
	    }

	    // handle next_list, unless at end of line and no "skipnl" or
	    // "skipempty"
	    current_next_list = cur_si->si_next_list;
	    current_next_flags = cur_si->si_flags;
	    எனில் (!(current_next_flags & (HL_SKIPNL | HL_SKIPEMPTY))
		    && syn_getcurline()[current_col] == NUL)
		current_next_list = வெற்று;

	    // When the ended item has "extend", another item with
	    // "keepend" now needs to check for its end.
	    had_extend = (cur_si->si_flags & HL_EXTEND);

	    எடு_current_state();

	    எனில் (current_state.ga_len == 0)
		நிறுத்து;

	    எனில் (had_extend && keepend_level >= 0)
	    {
		syn_update_ends(FALSE);
		எனில் (current_state.ga_len == 0)
		    நிறுத்து;
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
	    எனில் (cur_si->si_idx >= 0
		    && SYN_பொருட்கள்(syn_block)[cur_si->si_idx].sp_type
								== SPTYPE_START
		    && !(cur_si->si_flags & (HL_MATCH | HL_KEEPEND)))
	    {
		update_si_end(cur_si, (முழு)current_col, TRUE);
		check_keepend();
		எனில் ((current_next_flags & HL_HAS_EOL)
			&& keepend_level < 0
			&& syn_getcurline()[current_col] == NUL)
		    நிறுத்து;
	    }
	}
	இல்லையெனில்
	    நிறுத்து;
    }
}

/*
 * Update an entry in the current_state stack for a match or region.  This
 * fills in si_attr, si_next_list and si_cont_list.
 */
    நிலையான வெறுமை
update_si_attr(முழு idx)
{
    stateitem_T	*sip = &CUR_STATE(idx);
    synpat_T	*spp;

    // This should not happen...
    எனில் (sip->si_idx < 0)
	திரும்பு;

    spp = &(SYN_பொருட்கள்(syn_block)[sip->si_idx]);
    எனில் (sip->si_flags & HL_MATCH)
	sip->si_id = spp->sp_syn_match_id;
    இல்லையெனில்
	sip->si_id = spp->sp_syn.id;
    sip->si_attr = syn_id2attr(sip->si_id);
    sip->si_trans_id = sip->si_id;
    எனில் (sip->si_flags & HL_MATCH)
	sip->si_cont_list = வெற்று;
    இல்லையெனில்
	sip->si_cont_list = spp->sp_cont_list;

    /*
     * For transparent items, take attr from outer item.
     * Also take cont_list, if there is none.
     * Don't do this for the matchgroup of a start or end pattern.
     */
    எனில் ((spp->sp_flags & HL_TRANSP) && !(sip->si_flags & HL_MATCH))
    {
	எனில் (idx == 0)
	{
	    sip->si_attr = 0;
	    sip->si_trans_id = 0;
	    எனில் (sip->si_cont_list == வெற்று)
		sip->si_cont_list = ID_LIST_ALL;
	}
	இல்லையெனில்
	{
	    sip->si_attr = CUR_STATE(idx - 1).si_attr;
	    sip->si_trans_id = CUR_STATE(idx - 1).si_trans_id;
	    எனில் (sip->si_cont_list == வெற்று)
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
    நிலையான வெறுமை
check_keepend(வெறுமை)
{
    முழு		i;
    lpos_T	maxpos;
    lpos_T	maxpos_h;
    stateitem_T	*sip;

    /*
     * This check can consume a lot of time; only do it from the level where
     * there really is a keepend.
     */
    எனில் (keepend_level < 0)
	திரும்பு;

    /*
     * Find the last index of an "extend" item.  "keepend" items before that
     * won't do anything.  If there is no "extend" item "i" will be
     * "keepend_level" and all "keepend" items will work normally.
     */
    க்கு (i = current_state.ga_len - 1; i > keepend_level; --i)
	எனில் (CUR_STATE(i).si_flags & HL_EXTEND)
	    நிறுத்து;

    maxpos.lnum = 0;
    maxpos.col = 0;
    maxpos_h.lnum = 0;
    maxpos_h.col = 0;
    க்கு ( ; i < current_state.ga_len; ++i)
    {
	sip = &CUR_STATE(i);
	எனில் (maxpos.lnum != 0)
	{
	    limit_pos_zero(&sip->si_m_endpos, &maxpos);
	    limit_pos_zero(&sip->si_h_endpos, &maxpos_h);
	    limit_pos_zero(&sip->si_eoe_pos, &maxpos);
	    sip->si_ends = TRUE;
	}
	எனில் (sip->si_ends && (sip->si_flags & HL_KEEPEND))
	{
	    எனில் (maxpos.lnum == 0
		    || maxpos.lnum > sip->si_m_endpos.lnum
		    || (maxpos.lnum == sip->si_m_endpos.lnum
			&& maxpos.col > sip->si_m_endpos.col))
		maxpos = sip->si_m_endpos;
	    எனில் (maxpos_h.lnum == 0
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
    நிலையான வெறுமை
update_si_end(
    stateitem_T	*sip,
    முழு		startcol,   // where to start searching for the end
    முழு		force)	    // when TRUE overrule a previous end
{
    lpos_T	startpos;
    lpos_T	endpos;
    lpos_T	hl_endpos;
    lpos_T	end_endpos;
    முழு		end_idx;

    // return quickly for a keyword
    எனில் (sip->si_idx < 0)
	திரும்பு;

    // Don't update when it's already done.  Can be a match of an end pattern
    // that started in a previous line.  Watch out: can also be a "keepend"
    // from a containing item.
    எனில் (!force && sip->si_m_endpos.lnum >= current_lnum)
	திரும்பு;

    /*
     * We need to find the end of the region.  It may continue in the next
     * line.
     */
    end_idx = 0;
    startpos.lnum = current_lnum;
    startpos.col = startcol;
    find_endpos(sip->si_idx, &startpos, &endpos, &hl_endpos,
		   &(sip->si_flags), &end_endpos, &end_idx, sip->si_extmatch);

    எனில் (endpos.lnum == 0)
    {
	// No end pattern matched.
	எனில் (SYN_பொருட்கள்(syn_block)[sip->si_idx].sp_flags & HL_ONELINE)
	{
	    // a "oneline" never continues in the next line
	    sip->si_ends = TRUE;
	    sip->si_m_endpos.lnum = current_lnum;
	    sip->si_m_endpos.col = syn_getcurline_len();
	}
	இல்லையெனில்
	{
	    // continues in the next line
	    sip->si_ends = FALSE;
	    sip->si_m_endpos.lnum = 0;
	}
	sip->si_h_endpos = sip->si_m_endpos;
    }
    இல்லையெனில்
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
    நிலையான முழு
தள்ளு_current_state(முழு idx)
{
    எனில் (ga_grow(&current_state, 1) == FAIL)
	திரும்பு FAIL;
    CLEAR_POINTER(&CUR_STATE(current_state.ga_len));
    CUR_STATE(current_state.ga_len).si_idx = idx;
    ++current_state.ga_len;
    திரும்பு OK;
}

/*
 * Remove a state from the current_state stack.
 */
    நிலையான வெறுமை
எடு_current_state(வெறுமை)
{
    எனில் (current_state.ga_len)
    {
	unref_extmatch(CUR_STATE(current_state.ga_len - 1).si_extmatch);
	--current_state.ga_len;
    }
    // after the end of a pattern, try matching a keyword or pattern
    next_match_idx = -1;

    // if first state with "keepend" is popped, reset keepend_level
    எனில் (keepend_level >= current_state.ga_len)
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
    நிலையான வெறுமை
find_endpos(
    முழு		idx,		// index of the pattern
    lpos_T	*startpos,	// where to start looking for an END match
    lpos_T	*m_endpos,	// return: end of match
    lpos_T	*hl_endpos,	// return: end of highlighting
    long	*flagsp,	// return: flags of matching END
    lpos_T	*end_endpos,	// return: end of end pattern match
    முழு		*end_idx,	// return: group ID for end pat. match, or 0
    reg_extmatch_T *start_ext)	// submatches from the start pattern
{
    colnr_T	matchcol;
    synpat_T	*spp, *spp_skip;
    முழு		start_idx;
    முழு		best_idx;
    regmmatch_T	regmatch;
    regmmatch_T	best_regmatch;	    // startpos/endpos of best match
    lpos_T	pos;
    முழு		had_match = FALSE;
    char_u	buf_chartab[32];  // chartab array for syn option iskyeyword

    // just in case we are invoked for a keyword
    எனில் (idx < 0)
	திரும்பு;

    /*
     * Check for being called with a START pattern.
     * Can happen with a match that continues to the next line, because it
     * contained a region.
     */
    spp = &(SYN_பொருட்கள்(syn_block)[idx]);
    எனில் (spp->sp_type != SPTYPE_START)
    {
	*hl_endpos = *startpos;
	திரும்பு;
    }

    /*
     * Find the SKIP or first END pattern after the last START pattern.
     */
    க்கு (;;)
    {
	spp = &(SYN_பொருட்கள்(syn_block)[idx]);
	எனில் (spp->sp_type != SPTYPE_START)
	    நிறுத்து;
	++idx;
    }

    /*
     *	Lookup the SKIP pattern (if present)
     */
    எனில் (spp->sp_type == SPTYPE_SKIP)
    {
	spp_skip = spp;
	++idx;
    }
    இல்லையெனில்
	spp_skip = வெற்று;

    // Setup external matches for syn_regexec().
    unref_extmatch(re_extmatch_in);
    re_extmatch_in = ref_extmatch(start_ext);

    matchcol = startpos->col;	// start looking for a match at sstart
    start_idx = idx;		// remember the first END pattern.
    best_regmatch.startpos[0].col = 0;		// avoid compiler warning

    // use syntax iskeyword option
    save_chartab(buf_chartab);

    க்கு (;;)
    {
	/*
	 * Find end pattern that matches first after "matchcol".
	 */
	best_idx = -1;
	க்கு (idx = start_idx; idx < syn_block->b_syn_patterns.ga_len; ++idx)
	{
	    முழு lc_col = matchcol;
	    முழு r;

	    spp = &(SYN_பொருட்கள்(syn_block)[idx]);
	    எனில் (spp->sp_type != SPTYPE_END)	// past last END pattern
		நிறுத்து;
	    lc_col -= spp->sp_offsets[SPO_LC_OFF];
	    எனில் (lc_col < 0)
		lc_col = 0;

	    regmatch.rmm_ic = spp->sp_ic;
	    regmatch.regprog = spp->sp_prog;
	    r = syn_regexec(&regmatch, startpos->lnum, lc_col,
						  IF_SYN_TIME(&spp->sp_time));
	    spp->sp_prog = regmatch.regprog;
	    எனில் (r)
	    {
		எனில் (best_idx == -1 || regmatch.startpos[0].col
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
	எனில் (best_idx == -1)
	    நிறுத்து;

	/*
	 * If the skip pattern matches before the end pattern,
	 * continue searching after the skip pattern.
	 */
	எனில் (spp_skip != வெற்று)
	{
	    முழு lc_col = matchcol - spp_skip->sp_offsets[SPO_LC_OFF];
	    முழு r;

	    எனில் (lc_col < 0)
		lc_col = 0;
	    regmatch.rmm_ic = spp_skip->sp_ic;
	    regmatch.regprog = spp_skip->sp_prog;
	    r = syn_regexec(&regmatch, startpos->lnum, lc_col,
					      IF_SYN_TIME(&spp_skip->sp_time));
	    spp_skip->sp_prog = regmatch.regprog;
	    எனில் (r && regmatch.startpos[0].col
					     <= best_regmatch.startpos[0].col)
	    {
		முழு வரி_len;

		// Add offset to skip pattern match
		syn_add_end_off(&pos, &regmatch, spp_skip, SPO_ME_OFF, 1);

		// If the skip pattern goes on to the next line, there is no
		// match with an end pattern in this line.
		எனில் (pos.lnum > startpos->lnum)
		    நிறுத்து;

		வரி_len = ml_get_buf_len(syn_buf, startpos->lnum);

		// take care of an empty match or negative offset
		எனில் (pos.col <= matchcol)
		    ++matchcol;
		இல்லையெனில் எனில் (pos.col <= regmatch.endpos[0].col)
		    matchcol = pos.col;
		இல்லையெனில்
		    // Be careful not to jump over the NUL at the end-of-line
		    க்கு (matchcol = regmatch.endpos[0].col;
			    matchcol < வரி_len && matchcol < pos.col;
								   ++matchcol)
			;

		// if the skip pattern includes end-of-line, break here
		எனில் (matchcol >= வரி_len)
		    நிறுத்து;

		தொடர்;	    // start with first end pattern again
	    }
	}

	/*
	 * Match from start pattern to end pattern.
	 * Correct for match and highlight offset of end pattern.
	 */
	spp = &(SYN_பொருட்கள்(syn_block)[best_idx]);
	syn_add_end_off(m_endpos, &best_regmatch, spp, SPO_ME_OFF, 1);
	// can't end before the start
	எனில் (m_endpos->lnum == startpos->lnum && m_endpos->col < startpos->col)
	    m_endpos->col = startpos->col;

	syn_add_end_off(end_endpos, &best_regmatch, spp, SPO_HE_OFF, 1);
	// can't end before the start
	எனில் (end_endpos->lnum == startpos->lnum
					   && end_endpos->col < startpos->col)
	    end_endpos->col = startpos->col;
	// can't end after the match
	limit_pos(end_endpos, m_endpos);

	/*
	 * If the end group is highlighted differently, adjust the pointers.
	 */
	எனில் (spp->sp_syn_match_id != spp->sp_syn.id && spp->sp_syn_match_id != 0)
	{
	    *end_idx = best_idx;
	    எனில் (spp->sp_off_flags & (1 << (SPO_RE_OFF + SPO_மொத்தம்)))
	    {
		hl_endpos->lnum = best_regmatch.endpos[0].lnum;
		hl_endpos->col = best_regmatch.endpos[0].col;
	    }
	    இல்லையெனில்
	    {
		hl_endpos->lnum = best_regmatch.startpos[0].lnum;
		hl_endpos->col = best_regmatch.startpos[0].col;
	    }
	    hl_endpos->col += spp->sp_offsets[SPO_RE_OFF];

	    // can't end before the start
	    எனில் (hl_endpos->lnum == startpos->lnum
					    && hl_endpos->col < startpos->col)
		hl_endpos->col = startpos->col;
	    limit_pos(hl_endpos, m_endpos);

	    // now the match ends where the highlighting ends, it is turned
	    // into the matchgroup for the end
	    *m_endpos = *hl_endpos;
	}
	இல்லையெனில்
	{
	    *end_idx = 0;
	    *hl_endpos = *end_endpos;
	}

	*flagsp = spp->sp_flags;

	had_match = TRUE;
	நிறுத்து;
    }

    // no match for an END pattern in this line
    எனில் (!had_match)
	m_endpos->lnum = 0;

    restore_chartab(buf_chartab);

    // Remove external matches.
    unref_extmatch(re_extmatch_in);
    re_extmatch_in = வெற்று;
}

/*
 * Limit "pos" not to be after "limit".
 */
    நிலையான வெறுமை
limit_pos(lpos_T *pos, lpos_T *limit)
{
    எனில் (pos->lnum > limit->lnum)
	*pos = *limit;
    இல்லையெனில் எனில் (pos->lnum == limit->lnum && pos->col > limit->col)
	pos->col = limit->col;
}

/*
 * Limit "pos" not to be after "limit", unless pos->lnum is zero.
 */
    நிலையான வெறுமை
limit_pos_zero(
    lpos_T	*pos,
    lpos_T	*limit)
{
    எனில் (pos->lnum == 0)
	*pos = *limit;
    இல்லையெனில்
	limit_pos(pos, limit);
}

/*
 * Add offset to matched text for end of match or highlight.
 */
    நிலையான வெறுமை
syn_add_end_off(
    lpos_T	*result,	// returned position
    regmmatch_T	*regmatch,	// start/end of match
    synpat_T	*spp,		// matched pattern
    முழு		idx,		// index of offset
    முழு		extra)		// extra chars for offset to start
{
    முழு		col;
    முழு		off;
    char_u	*base;
    char_u	*p;

    எனில் (spp->sp_off_flags & (1 << idx))
    {
	result->lnum = regmatch->startpos[0].lnum;
	col = regmatch->startpos[0].col;
	off = spp->sp_offsets[idx] + extra;
    }
    இல்லையெனில்
    {
	result->lnum = regmatch->endpos[0].lnum;
	col = regmatch->endpos[0].col;
	off = spp->sp_offsets[idx];
    }
    // Don't go past the end of the line.  Matters for "rs=e+2" when there
    // is a matchgroup. Watch out for match with last NL in the buffer.
    எனில் (result->lnum > syn_buf->b_ml.ml_வரி_மொத்தம்)
	col = 0;
    இல்லையெனில் எனில் (off != 0)
    {
	base = ml_get_buf(syn_buf, result->lnum, FALSE);
	p = base + col;
	எனில் (off > 0)
	{
	    போது (off-- > 0 && *p != NUL)
		MB_PTR_ADV(p);
	}
	இல்லையெனில் எனில் (off < 0)
	{
	    போது (off++ < 0 && base < p)
		MB_PTR_BACK(base, p);
	}
	col = (முழு)(p - base);
    }
    result->col = col;
}

/*
 * Add offset to matched text for start of match or highlight.
 * Avoid resulting column to become negative.
 */
    நிலையான வெறுமை
syn_add_start_off(
    lpos_T	*result,	// returned position
    regmmatch_T	*regmatch,	// start/end of match
    synpat_T	*spp,
    முழு		idx,
    முழு		extra)	    // extra chars for offset to end
{
    முழு		col;
    முழு		off;
    char_u	*base;
    char_u	*p;

    எனில் (spp->sp_off_flags & (1 << (idx + SPO_மொத்தம்)))
    {
	result->lnum = regmatch->endpos[0].lnum;
	col = regmatch->endpos[0].col;
	off = spp->sp_offsets[idx] + extra;
    }
    இல்லையெனில்
    {
	result->lnum = regmatch->startpos[0].lnum;
	col = regmatch->startpos[0].col;
	off = spp->sp_offsets[idx];
    }
    எனில் (result->lnum > syn_buf->b_ml.ml_வரி_மொத்தம்)
    {
	// a "\n" at the end of the pattern may take us below the last line
	result->lnum = syn_buf->b_ml.ml_வரி_மொத்தம்;
	col = ml_get_buf_len(syn_buf, result->lnum);
    }
    எனில் (off != 0)
    {
	base = ml_get_buf(syn_buf, result->lnum, FALSE);
	p = base + col;
	எனில் (off > 0)
	{
	    போது (off-- && *p != NUL)
		MB_PTR_ADV(p);
	}
	இல்லையெனில் எனில் (off < 0)
	{
	    போது (off++ && base < p)
		MB_PTR_BACK(base, p);
	}
	col = (முழு)(p - base);
    }
    result->col = col;
}

/*
 * Get current line in syntax buffer.
 */
    நிலையான char_u *
syn_getcurline(வெறுமை)
{
    திரும்பு ml_get_buf(syn_buf, current_lnum, FALSE);
}

/*
 * Get length of current line in syntax buffer.
 */
    நிலையான colnr_T
syn_getcurline_len(வெறுமை)
{
    திரும்பு ml_get_buf_len(syn_buf, current_lnum);
}

/*
 * Call vim_regexec() to find a match with "rmp" in "syn_buf".
 * Returns TRUE when there is a match.
 */
    நிலையான முழு
syn_regexec(
    regmmatch_T	*rmp,
    linenr_T	lnum,
    colnr_T	col,
    syn_time_T  *st UNUSED)
{
    முழு		r;
    முழு		timed_out = FALSE;
#ifdef FEAT_PROFILE
    proftime_T	pt;

    எனில் (syn_time_on)
	profile_start(&pt);
#endif

    எனில் (rmp->regprog == வெற்று)
	// This can happen if a previous call to vim_regexec_multi() tried to
	// use the NFA engine, which resulted in NFA_TOO_EXPENSIVE, and
	// compiling the pattern with the other engine fails.
	திரும்பு FALSE;

    rmp->rmm_maxcol = syn_buf->b_p_smc;
    r = vim_regexec_multi(rmp, syn_win, syn_buf, lnum, col, &timed_out);

#ifdef FEAT_PROFILE
    எனில் (syn_time_on)
    {
	profile_end(&pt);
	profile_add(&st->total, &pt);
	எனில் (profile_cmp(&pt, &st->slowest) < 0)
	    st->slowest = pt;
	++st->மொத்தம்;
	எனில் (r > 0)
	    ++st->match;
    }
#endif
#ifdef FEAT_RELTIME
    எனில் (timed_out && redrawtime_limit_set && !syn_win->w_s->b_syn_slow)
    {
	syn_win->w_s->b_syn_slow = TRUE;
	செய்தி(_("'redrawtime' exceeded, syntax highlighting disabled"));
    }
#endif

    எனில் (r > 0)
    {
	rmp->startpos[0].lnum += lnum;
	rmp->endpos[0].lnum += lnum;
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Check one position in a line for a matching keyword.
 * The caller must check if a keyword can start at startcol.
 * Return its ID if found, 0 otherwise.
 */
    நிலையான முழு
check_keyword_id(
    char_u	*வரி,
    முழு		startcol,	// position in line to check for keyword
    முழு		*endcolp,	// return: character after found keyword
    long	*flagsp,	// return: flags of matching keyword
    short	**next_listp,	// return: next_list of matching keyword
    stateitem_T	*cur_si,	// item at the top of the stack
    முழு		*ccharp UNUSED)	// conceal substitution char
{
    keyentry_T	*kp;
    char_u	*kwp;
    முழு		round;
    முழு		kwlen;
    char_u	keyword[MAXKEYWLEN + 1]; // assume max. keyword len is 80
    hashtab_T	*ht;
    hashitem_T	*hi;

    // Find first character after the keyword.  First character was already
    // checked.
    kwp = வரி + startcol;
    kwlen = 0;
    செய்
    {
	எனில் (has_mbyte)
	    kwlen += (*mb_ptr2len)(kwp + kwlen);
	இல்லையெனில்
	    ++kwlen;
    }
    போது (vim_iswordp_buf(kwp + kwlen, syn_buf));

    எனில் (kwlen > MAXKEYWLEN)
	திரும்பு 0;

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
    க்கு (round = 1; round <= 2; ++round)
    {
	ht = round == 1 ? &syn_block->b_keywtab : &syn_block->b_keywtab_ic;
	எனில் (ht->ht_used == 0)
	    தொடர்;
	எனில் (round == 2)	// ignore case
	    (வெறுமை)str_foldcase(kwp, kwlen, keyword, MAXKEYWLEN + 1);

	/*
	 * Find keywords that match.  There can be several with different
	 * attributes.
	 * When current_next_list is non-zero accept only that group, otherwise:
	 *  Accept a not-contained keyword at toplevel.
	 *  Accept a keyword at other levels only if it is in the contains list.
	 */
	hi = hash_find(ht, keyword);
	எனில் (!HASHITEM_EMPTY(hi))
	    க்கு (kp = HI2KE(hi); kp != வெற்று; kp = kp->ke_next)
	    {
		எனில் (current_next_list != 0
			? in_id_list(வெற்று, current_next_list, &kp->k_syn, 0)
			: (cur_si == வெற்று
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
		    திரும்பு kp->k_syn.id;
		}
	    }
    }
    திரும்பு 0;
}

/*
 * Handle ":syntax conceal" command.
 */
    நிலையான வெறுமை
syn_cmd_conceal(exarg_T *eap UNUSED, முழு syncing UNUSED)
{
#ifdef FEAT_CONCEAL
    char_u	*arg = eap->arg;
    char_u	*next;

    eap->nextcmd = find_nextcmd(arg);
    எனில் (eap->skip)
	திரும்பு;

    next = skiptowhite(arg);
    எனில் (*arg == NUL)
    {
	எனில் (curwin->w_s->b_syn_conceal)
	    செய்தி("syntax conceal on");
	இல்லையெனில்
	    செய்தி("syntax conceal off");
    }
    இல்லையெனில் எனில் (STRNICMP(arg, "on", 2) == 0 && next - arg == 2)
	curwin->w_s->b_syn_conceal = TRUE;
    இல்லையெனில் எனில் (STRNICMP(arg, "off", 3) == 0 && next - arg == 3)
	curwin->w_s->b_syn_conceal = FALSE;
    இல்லையெனில்
	semsg(_(e_illegal_argument_str_2), arg);
#endif
}

/*
 * Handle ":syntax case" command.
 */
    நிலையான வெறுமை
syn_cmd_case(exarg_T *eap, முழு syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*next;

    eap->nextcmd = find_nextcmd(arg);
    எனில் (eap->skip)
	திரும்பு;

    next = skiptowhite(arg);
    எனில் (*arg == NUL)
    {
	எனில் (curwin->w_s->b_syn_ic)
	    செய்தி("syntax case ignore");
	இல்லையெனில்
	    செய்தி("syntax case match");
    }
    இல்லையெனில் எனில் (STRNICMP(arg, "match", 5) == 0 && next - arg == 5)
	curwin->w_s->b_syn_ic = FALSE;
    இல்லையெனில் எனில் (STRNICMP(arg, "ignore", 6) == 0 && next - arg == 6)
	curwin->w_s->b_syn_ic = TRUE;
    இல்லையெனில்
	semsg(_(e_illegal_argument_str_2), arg);
}

/*
 * Handle ":syntax foldlevel" command.
 */
    நிலையான வெறுமை
syn_cmd_foldlevel(exarg_T *eap, முழு syncing UNUSED)
{
    char_u *arg = eap->arg;
    char_u *arg_end;

    eap->nextcmd = find_nextcmd(arg);
    எனில் (eap->skip)
	திரும்பு;

    எனில் (*arg == NUL)
    {
	தேர்வு (curwin->w_s->b_syn_foldlevel)
	{
	    நிலை SYNFLD_START:   செய்தி("syntax foldlevel start");   நிறுத்து;
	    நிலை SYNFLD_MINIMUM: செய்தி("syntax foldlevel minimum"); நிறுத்து;
	    இயல்பு: நிறுத்து;
	}
	திரும்பு;
    }

    arg_end = skiptowhite(arg);
    எனில் (STRNICMP(arg, "start", 5) == 0 && arg_end - arg == 5)
	curwin->w_s->b_syn_foldlevel = SYNFLD_START;
    இல்லையெனில் எனில் (STRNICMP(arg, "minimum", 7) == 0 && arg_end - arg == 7)
	curwin->w_s->b_syn_foldlevel = SYNFLD_MINIMUM;
    இல்லையெனில்
    {
	semsg(_(e_illegal_argument_str_2), arg);
	திரும்பு;
    }

    arg = skipwhite(arg_end);
    எனில் (*arg != NUL)
    {
	semsg(_(e_illegal_argument_str_2), arg);
    }
}

/*
 * Handle ":syntax spell" command.
 */
    நிலையான வெறுமை
syn_cmd_spell(exarg_T *eap, முழு syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*next;

    eap->nextcmd = find_nextcmd(arg);
    எனில் (eap->skip)
	திரும்பு;

    next = skiptowhite(arg);
    எனில் (*arg == NUL)
    {
	எனில் (curwin->w_s->b_syn_spell == SYNSPL_உச்சி)
	    செய்தி("syntax spell toplevel");
	இல்லையெனில் எனில் (curwin->w_s->b_syn_spell == SYNSPL_NOTOP)
	    செய்தி("syntax spell notoplevel");
	இல்லையெனில்
	    செய்தி("syntax spell default");
    }
    இல்லையெனில் எனில் (STRNICMP(arg, "toplevel", 8) == 0 && next - arg == 8)
	curwin->w_s->b_syn_spell = SYNSPL_உச்சி;
    இல்லையெனில் எனில் (STRNICMP(arg, "notoplevel", 10) == 0 && next - arg == 10)
	curwin->w_s->b_syn_spell = SYNSPL_NOTOP;
    இல்லையெனில் எனில் (STRNICMP(arg, "default", 7) == 0 && next - arg == 7)
	curwin->w_s->b_syn_spell = SYNSPL_DEFAULT;
    இல்லையெனில்
    {
	semsg(_(e_illegal_argument_str_2), arg);
	திரும்பு;
    }

    // assume spell checking changed, force a redraw
    redraw_win_later(curwin, UPD_NOT_VALID);
}

/*
 * Handle ":syntax iskeyword" command.
 */
    நிலையான வெறுமை
syn_cmd_iskeyword(exarg_T *eap, முழு syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	save_chartab[32];
    char_u	*save_isk;

    எனில் (eap->skip)
	திரும்பு;

    arg = skipwhite(arg);
    எனில் (*arg == NUL)
    {
	செய்தி_puts("\n");
	எனில் (curwin->w_s->b_syn_isk != empty_option)
	{
	    செய்தி_puts("syntax iskeyword ");
	    செய்தி_outtrans(curwin->w_s->b_syn_isk);
	}
	இல்லையெனில்
	    செய்தி_outtrans((char_u *)_("syntax iskeyword not set"));
    }
    இல்லையெனில்
    {
	எனில் (STRNICMP(arg, "clear", 5) == 0)
	{
	    mch_memmove(curwin->w_s->b_syn_chartab, curbuf->b_chartab,
								  (size_t)32);
	    clear_string_option(&curwin->w_s->b_syn_isk);
	}
	இல்லையெனில்
	{
	    எனில் (check_isopt(arg) == FAIL)
	    {
		emsg(_(e_invalid_argument));
		திரும்பு;
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
    வெறுமை
syntax_clear(synblock_T *block)
{
    முழு i;

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
    க்கு (i = block->b_syn_patterns.ga_len; --i >= 0; )
	syn_clear_pattern(block, i);
    ga_clear(&block->b_syn_patterns);

    // free the syntax clusters
    க்கு (i = block->b_syn_clusters.ga_len; --i >= 0; )
	syn_clear_cluster(block, i);
    ga_clear(&block->b_syn_clusters);
    block->b_spell_cluster_id = 0;
    block->b_nospell_cluster_id = 0;

    block->b_syn_sync_flags = 0;
    block->b_syn_sync_minlines = 0;
    block->b_syn_sync_maxlines = 0;
    block->b_syn_sync_linebreaks = 0;

    vim_regfree(block->b_syn_linecont_prog);
    block->b_syn_linecont_prog = வெற்று;
    VIM_CLEAR(block->b_syn_linecont_pat);
#ifdef FEAT_FOLDING
    block->b_syn_folditems = 0;
#endif
    clear_string_option(&block->b_syn_isk);

    // free the stored states
    syn_அடுக்கு_free_all(block);
    invalidate_current_state();

    // Reset the counter for ":syn include"
    running_syn_inc_tag = 0;
}

/*
 * Get rid of ownsyntax for window "wp".
 */
    வெறுமை
reset_synblock(win_T *wp)
{
    எனில் (wp->w_s != &wp->w_buffer->b_s)
    {
	syntax_clear(wp->w_s);
	vim_free(wp->w_s);
	wp->w_s = &wp->w_buffer->b_s;
    }
}

/*
 * Clear syncing info for one buffer.
 */
    நிலையான வெறுமை
syntax_sync_clear(வெறுமை)
{
    முழு i;

    // free the syntax patterns
    க்கு (i = curwin->w_s->b_syn_patterns.ga_len; --i >= 0; )
	எனில் (SYN_பொருட்கள்(curwin->w_s)[i].sp_syncing)
	    syn_remove_pattern(curwin->w_s, i);

    curwin->w_s->b_syn_sync_flags = 0;
    curwin->w_s->b_syn_sync_minlines = 0;
    curwin->w_s->b_syn_sync_maxlines = 0;
    curwin->w_s->b_syn_sync_linebreaks = 0;

    vim_regfree(curwin->w_s->b_syn_linecont_prog);
    curwin->w_s->b_syn_linecont_prog = வெற்று;
    VIM_CLEAR(curwin->w_s->b_syn_linecont_pat);
    clear_string_option(&curwin->w_s->b_syn_isk);

    syn_அடுக்கு_free_all(curwin->w_s);	// Need to recompute all syntax.
}

/*
 * Remove one pattern from the buffer's pattern list.
 */
    நிலையான வெறுமை
syn_remove_pattern(
    synblock_T	*block,
    முழு		idx)
{
    synpat_T	*spp;

    spp = &(SYN_பொருட்கள்(block)[idx]);
#ifdef FEAT_FOLDING
    எனில் (spp->sp_flags & HL_FOLD)
	--block->b_syn_folditems;
#endif
    syn_clear_pattern(block, idx);
    mch_memmove(spp, spp + 1,
		   அளவு(synpat_T) * (block->b_syn_patterns.ga_len - idx - 1));
    --block->b_syn_patterns.ga_len;
}

/*
 * Clear and free one syntax pattern.  When clearing all, must be called from
 * last to first!
 */
    நிலையான வெறுமை
syn_clear_pattern(synblock_T *block, முழு i)
{
    vim_free(SYN_பொருட்கள்(block)[i].sp_pattern);
    vim_regfree(SYN_பொருட்கள்(block)[i].sp_prog);
    // Only free sp_cont_list and sp_next_list of first start pattern
    எனில் (i == 0 || SYN_பொருட்கள்(block)[i - 1].sp_type != SPTYPE_START)
    {
	vim_free(SYN_பொருட்கள்(block)[i].sp_cont_list);
	vim_free(SYN_பொருட்கள்(block)[i].sp_next_list);
	vim_free(SYN_பொருட்கள்(block)[i].sp_syn.cont_in_list);
    }
}

/*
 * Clear and free one syntax cluster.
 */
    நிலையான வெறுமை
syn_clear_cluster(synblock_T *block, முழு i)
{
    vim_free(SYN_CLSTR(block)[i].scl_name);
    vim_free(SYN_CLSTR(block)[i].scl_name_u);
    vim_free(SYN_CLSTR(block)[i].scl_list);
}

/*
 * Handle ":syntax clear" command.
 */
    நிலையான வெறுமை
syn_cmd_clear(exarg_T *eap, முழு syncing)
{
    char_u	*arg = eap->arg;
    char_u	*arg_end;
    முழு		id;

    eap->nextcmd = find_nextcmd(arg);
    எனில் (eap->skip)
	திரும்பு;

    /*
     * We have to disable this within ":syn include @group filename",
     * because otherwise @group would get deleted.
     * Only required for Vim 5.x syntax files, 6.0 ones don't contain ":syn
     * clear".
     */
    எனில் (curwin->w_s->b_syn_topgrp != 0)
	திரும்பு;

    எனில் (ends_excmd2(eap->cmd, arg))
    {
	/*
	 * No argument: Clear all syntax items.
	 */
	எனில் (syncing)
	    syntax_sync_clear();
	இல்லையெனில்
	{
	    syntax_clear(curwin->w_s);
	    எனில் (curwin->w_s == &curwin->w_buffer->b_s)
		do_unlet((char_u *)"b:current_syntax", TRUE);
	    do_unlet((char_u *)"w:current_syntax", TRUE);
	}
    }
    இல்லையெனில்
    {
	/*
	 * Clear the group IDs that are in the argument.
	 */
	போது (!ends_excmd2(eap->cmd, arg))
	{
	    arg_end = skiptowhite(arg);
	    எனில் (*arg == '@')
	    {
		id = syn_scl_namen2id(arg + 1, (முழு)(arg_end - arg - 1));
		எனில் (id == 0)
		{
		    semsg(_(e_no_such_syntax_cluster_str_1), arg);
		    நிறுத்து;
		}
		இல்லையெனில்
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
	    இல்லையெனில்
	    {
		id = syn_namen2id(arg, (முழு)(arg_end - arg));
		எனில் (id == 0)
		{
		    semsg(_(e_no_such_highlight_group_name_str), arg);
		    நிறுத்து;
		}
		இல்லையெனில்
		    syn_clear_one(id, syncing);
	    }
	    arg = skipwhite(arg_end);
	}
    }
    redraw_curbuf_later(UPD_SOME_VALID);
    syn_அடுக்கு_free_all(curwin->w_s);		// Need to recompute all syntax.
}

/*
 * Clear one syntax group for the current buffer.
 */
    நிலையான வெறுமை
syn_clear_one(முழு id, முழு syncing)
{
    synpat_T	*spp;
    முழு		idx;

    // Clear keywords only when not ":syn sync clear group-name"
    எனில் (!syncing)
    {
	(வெறுமை)syn_clear_keyword(id, &curwin->w_s->b_keywtab);
	(வெறுமை)syn_clear_keyword(id, &curwin->w_s->b_keywtab_ic);
    }

    // clear the patterns for "id"
    க்கு (idx = curwin->w_s->b_syn_patterns.ga_len; --idx >= 0; )
    {
	spp = &(SYN_பொருட்கள்(curwin->w_s)[idx]);
	எனில் (spp->sp_syn.id != id || spp->sp_syncing != syncing)
	    தொடர்;
	syn_remove_pattern(curwin->w_s, idx);
    }
}

/*
 * Handle ":syntax on" command.
 */
    நிலையான வெறுமை
syn_cmd_on(exarg_T *eap, முழு syncing UNUSED)
{
    syn_cmd_onoff(eap, "syntax");
}

/*
 * Handle ":syntax enable" command.
 */
    நிலையான வெறுமை
syn_cmd_enable(exarg_T *eap, முழு syncing UNUSED)
{
    set_internal_string_var((char_u *)"g:syntax_cmd", (char_u *)"enable");
    syn_cmd_onoff(eap, "syntax");
    do_unlet((char_u *)"g:syntax_cmd", TRUE);
}

/*
 * Handle ":syntax reset" command.
 * It actually resets highlighting, not syntax.
 */
    நிலையான வெறுமை
syn_cmd_reset(exarg_T *eap, முழு syncing UNUSED)
{
    set_nextcmd(eap, eap->arg);
    எனில் (!eap->skip)
    {
	set_internal_string_var((char_u *)"g:syntax_cmd", (char_u *)"reset");
	do_cmdline_cmd((char_u *)"runtime! syntax/syncolor.vim");
	do_unlet((char_u *)"g:syntax_cmd", TRUE);
    }
}

/*
 * Handle ":syntax manual" command.
 */
    நிலையான வெறுமை
syn_cmd_manual(exarg_T *eap, முழு syncing UNUSED)
{
    syn_cmd_onoff(eap, "manual");
}

/*
 * Handle ":syntax off" command.
 */
    நிலையான வெறுமை
syn_cmd_off(exarg_T *eap, முழு syncing UNUSED)
{
    syn_cmd_onoff(eap, "nosyntax");
}

    நிலையான வெறுமை
syn_cmd_onoff(exarg_T *eap, எழுத்து *name)
{
    char_u	buf[100];

    set_nextcmd(eap, eap->arg);
    எனில் (!eap->skip)
    {
	STRCPY(buf, "so ");
	vim_snprintf((எழுத்து *)buf + 3, அளவு(buf) - 3, SYNTAX_FNAME, name);
	do_cmdline_cmd(buf);
    }
}

/*
 * Handle ":syntax [list]" command: list current syntax words.
 */
    நிலையான வெறுமை
syn_cmd_list(
    exarg_T	*eap,
    முழு		syncing)	    // when TRUE: list syncing items
{
    char_u	*arg = eap->arg;
    முழு		id;
    char_u	*arg_end;

    eap->nextcmd = find_nextcmd(arg);
    எனில் (eap->skip)
	திரும்பு;

    எனில் (!syntax_present(curwin))
    {
	செய்தி(_(செய்தி_no_பொருட்கள்));
	திரும்பு;
    }

    எனில் (syncing)
    {
	எனில் (curwin->w_s->b_syn_sync_flags & SF_CCOMMENT)
	{
	    செய்தி_puts(_("syncing on C-style comments"));
	    syn_வரிகள்_செய்தி();
	    syn_match_செய்தி();
	}
	இல்லையெனில் எனில் (curwin->w_s->b_syn_sync_flags & SF_MATCH)
	{
	    செய்தி_puts_title(_("\n--- Syntax sync items ---"));
	    எனில் (curwin->w_s->b_syn_sync_minlines > 0
		|| curwin->w_s->b_syn_sync_maxlines > 0
		|| curwin->w_s->b_syn_sync_linebreaks > 0)
	    {
		செய்தி_puts(_("\nsyncing on items"));
		syn_வரிகள்_செய்தி();
		syn_match_செய்தி();
	    }
	    க்கு (id = 1; id <= highlight_இலக்கம்_groups() && !got_int; ++id)
		syn_list_one(id, syncing, FALSE);
	}
	இல்லையெனில்
	{
	    எனில் (curwin->w_s->b_syn_sync_minlines == 0)
		செய்தி_puts(_("no syncing"));
	    இல்லையெனில்
	    {
		எனில் (curwin->w_s->b_syn_sync_minlines == MAXLNUM)
		    செய்தி_puts(_("syncing starts at the first line"));
		இல்லையெனில்
		{
		    செய்தி_puts(_("syncing starts "));
		    செய்தி_outnum(curwin->w_s->b_syn_sync_minlines);
		    செய்தி_puts(_(" lines before top line"));
		}
		syn_match_செய்தி();
	    }
	}
	திரும்பு;
    }

    செய்தி_puts_title(_("\n--- Syntax items ---"));
    எனில் (ends_excmd2(eap->cmd, arg))
    {
	/*
	 * No argument: List all group IDs and all syntax clusters.
	 */
	க்கு (id = 1; id <= highlight_இலக்கம்_groups() && !got_int; ++id)
	    syn_list_one(id, syncing, FALSE);
	க்கு (id = 0; id < curwin->w_s->b_syn_clusters.ga_len && !got_int; ++id)
	    syn_list_cluster(id);
    }
    இல்லையெனில்
    {
	/*
	 * List the group IDs and syntax clusters that are in the argument.
	 */
	போது (!ends_excmd2(eap->cmd, arg) && !got_int)
	{
	    arg_end = skiptowhite(arg);
	    எனில் (*arg == '@')
	    {
		id = syn_scl_namen2id(arg + 1, (முழு)(arg_end - arg - 1));
		எனில் (id == 0)
		    semsg(_(e_no_such_syntax_cluster_str_2), arg);
		இல்லையெனில்
		    syn_list_cluster(id - SYNID_CLUSTER);
	    }
	    இல்லையெனில்
	    {
		id = syn_namen2id(arg, (முழு)(arg_end - arg));
		எனில் (id == 0)
		    semsg(_(e_no_such_highlight_group_name_str), arg);
		இல்லையெனில்
		    syn_list_one(id, syncing, TRUE);
	    }
	    arg = skipwhite(arg_end);
	}
    }
    set_nextcmd(eap, arg);
}

    நிலையான வெறுமை
syn_வரிகள்_செய்தி(வெறுமை)
{
    எனில் (curwin->w_s->b_syn_sync_maxlines > 0
				      || curwin->w_s->b_syn_sync_minlines > 0)
    {
	செய்தி_puts("; ");
	எனில் (curwin->w_s->b_syn_sync_minlines == MAXLNUM)
	    செய்தி_puts(_("from the first line"));
	இல்லையெனில்
	{
	    எனில் (curwin->w_s->b_syn_sync_minlines > 0)
	    {
		செய்தி_puts(_("minimal "));
		செய்தி_outnum(curwin->w_s->b_syn_sync_minlines);
		எனில் (curwin->w_s->b_syn_sync_maxlines)
		    செய்தி_puts(", ");
	    }
	    எனில் (curwin->w_s->b_syn_sync_maxlines > 0)
	    {
		செய்தி_puts(_("maximal "));
		செய்தி_outnum(curwin->w_s->b_syn_sync_maxlines);
	    }
	    செய்தி_puts(_(" lines before top line"));
	}
    }
}

    நிலையான வெறுமை
syn_match_செய்தி(வெறுமை)
{
    எனில் (curwin->w_s->b_syn_sync_linebreaks > 0)
    {
	செய்தி_puts(_("; match "));
	செய்தி_outnum(curwin->w_s->b_syn_sync_linebreaks);
	செய்தி_puts(_(" line breaks"));
    }
}

நிலையான முழு  last_matchgroup;

நிலையான வெறுமை syn_list_flags(keyvalue_T *nlist, முழு nr_entries, முழு flags, முழு attr);

/*
 * List one syntax item, for ":syntax" or "syntax list syntax_name".
 */
    நிலையான வெறுமை
syn_list_one(
    முழு		id,
    முழு		syncing,	    // when TRUE: list syncing items
    முழு		link_only)	    // when TRUE; list link-only too
{
    முழு		attr;
    முழு		idx;
    முழு		did_header = FALSE;
    synpat_T	*spp;
    நிலையான keyvalue_T namelist1[] =
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
    நிலையான keyvalue_T namelist2[] =
		{
		    KEYVALUE_ENTRY(HL_SKIPWHITE, "skipwhite"),
		    KEYVALUE_ENTRY(HL_SKIPNL, "skipnl"),
		    KEYVALUE_ENTRY(HL_SKIPEMPTY, "skipempty")
		};

    attr = HL_ATTR(HLF_D);		// highlight like directories

    // list the keywords for "id"
    எனில் (!syncing)
    {
	did_header = syn_list_keywords(id, &curwin->w_s->b_keywtab, FALSE, attr);
	did_header = syn_list_keywords(id, &curwin->w_s->b_keywtab_ic,
							    did_header, attr);
    }

    // list the patterns for "id"
    க்கு (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len && !got_int; ++idx)
    {
	spp = &(SYN_பொருட்கள்(curwin->w_s)[idx]);
	எனில் (spp->sp_syn.id != id || spp->sp_syncing != syncing)
	    தொடர்;

	(வெறுமை)syn_list_header(did_header, 999, id);
	did_header = TRUE;
	last_matchgroup = 0;
	எனில் (spp->sp_type == SPTYPE_MATCH)
	{
	    put_pattern("match", ' ', spp, attr);
	    செய்தி_putchar(' ');
	}
	இல்லையெனில் எனில் (spp->sp_type == SPTYPE_START)
	{
	    போது (SYN_பொருட்கள்(curwin->w_s)[idx].sp_type == SPTYPE_START)
		put_pattern("start", '=', &SYN_பொருட்கள்(curwin->w_s)[idx++], attr);
	    எனில் (SYN_பொருட்கள்(curwin->w_s)[idx].sp_type == SPTYPE_SKIP)
		put_pattern("skip", '=', &SYN_பொருட்கள்(curwin->w_s)[idx++], attr);
	    போது (idx < curwin->w_s->b_syn_patterns.ga_len
			  && SYN_பொருட்கள்(curwin->w_s)[idx].sp_type == SPTYPE_END)
		put_pattern("end", '=', &SYN_பொருட்கள்(curwin->w_s)[idx++], attr);
	    --idx;
	    செய்தி_putchar(' ');
	}
	syn_list_flags(namelist1, (முழு)ARRAY_LENGTH(namelist1), spp->sp_flags, attr);

	எனில் (spp->sp_cont_list != வெற்று)
	    put_id_list((char_u *)"contains", spp->sp_cont_list, attr);

	எனில் (spp->sp_syn.cont_in_list != வெற்று)
	    put_id_list((char_u *)"containedin",
					      spp->sp_syn.cont_in_list, attr);

	எனில் (spp->sp_next_list != வெற்று)
	{
	    put_id_list((char_u *)"nextgroup", spp->sp_next_list, attr);
	    syn_list_flags(namelist2, (முழு)ARRAY_LENGTH(namelist2), spp->sp_flags, attr);
	}
	எனில் (spp->sp_flags & (HL_SYNC_HERE|HL_SYNC_THERE))
	{
	    எனில் (spp->sp_flags & HL_SYNC_HERE)
		செய்தி_puts_attr("grouphere", attr);
	    இல்லையெனில்
		செய்தி_puts_attr("groupthere", attr);
	    செய்தி_putchar(' ');
	    எனில் (spp->sp_sync_idx >= 0)
		செய்தி_outtrans(highlight_group_name(SYN_பொருட்கள்(curwin->w_s)
				   [spp->sp_sync_idx].sp_syn.id - 1));
	    இல்லையெனில்
		செய்தி_puts("NONE");
	    செய்தி_putchar(' ');
	}
    }

    // list the link, if there is one
    எனில் (highlight_link_id(id - 1) && (did_header || link_only) && !got_int)
    {
	(வெறுமை)syn_list_header(did_header, 999, id);
	செய்தி_puts_attr("links to", attr);
	செய்தி_putchar(' ');
	செய்தி_outtrans(highlight_group_name(highlight_link_id(id - 1) - 1));
    }
}

    நிலையான வெறுமை
syn_list_flags(keyvalue_T *nlist, முழு nr_entries, முழு flags, முழு attr)
{
    முழு		i;

    க்கு (i = 0; i < nr_entries; ++i)
	எனில் (flags & nlist[i].சாவி)
	{
	    செய்தி_puts_attr((எழுத்து *)nlist[i].மதிப்பு.string, attr);
	    செய்தி_putchar(' ');
	}
}

/*
 * List one syntax cluster, for ":syntax" or "syntax list syntax_name".
 */
    நிலையான வெறுமை
syn_list_cluster(முழு id)
{
    முழு	    endcol = 15;

    // slight hack:  roughly duplicate the guts of syn_list_header()
    செய்தி_putchar('\n');
    செய்தி_outtrans(SYN_CLSTR(curwin->w_s)[id].scl_name);

    எனில் (செய்தி_col >= endcol)	// output at least one space
	endcol = செய்தி_col + 1;
    எனில் (cmdline_width <= endcol)	// avoid hang for tiny window
	endcol = cmdline_width - 1;

    செய்தி_advance(endcol);
    எனில் (SYN_CLSTR(curwin->w_s)[id].scl_list != வெற்று)
    {
	put_id_list((char_u *)"cluster", SYN_CLSTR(curwin->w_s)[id].scl_list,
		    HL_ATTR(HLF_D));
    }
    இல்லையெனில்
    {
	செய்தி_puts_attr("cluster", HL_ATTR(HLF_D));
	செய்தி_puts("=NONE");
    }
}

    நிலையான வெறுமை
put_id_list(char_u *name, short *list, முழு attr)
{
    short		*p;

    செய்தி_puts_attr((எழுத்து *)name, attr);
    செய்தி_putchar('=');
    க்கு (p = list; *p; ++p)
    {
	எனில் (*p >= SYNID_ALLBUT && *p < SYNID_உச்சி)
	{
	    எனில் (p[1])
		செய்தி_puts("ALLBUT");
	    இல்லையெனில்
		செய்தி_puts("ALL");
	}
	இல்லையெனில் எனில் (*p >= SYNID_உச்சி && *p < SYNID_CONTAINED)
	{
	    செய்தி_puts("TOP");
	}
	இல்லையெனில் எனில் (*p >= SYNID_CONTAINED && *p < SYNID_CLUSTER)
	{
	    செய்தி_puts("CONTAINED");
	}
	இல்லையெனில் எனில் (*p >= SYNID_CLUSTER)
	{
	    short scl_id = *p - SYNID_CLUSTER;

	    செய்தி_putchar('@');
	    செய்தி_outtrans(SYN_CLSTR(curwin->w_s)[scl_id].scl_name);
	}
	இல்லையெனில்
	    செய்தி_outtrans(highlight_group_name(*p - 1));
	எனில் (p[1])
	    செய்தி_putchar(',');
    }
    செய்தி_putchar(' ');
}

    நிலையான வெறுமை
put_pattern(
    எழுத்து	*s,
    முழு		c,
    synpat_T	*spp,
    முழு		attr)
{
    long	n;
    முழு		mask;
    முழு		first;
    நிலையான எழுத்து	*sepchars = "/+=-#@\"|'^&";
    முழு		i;

    // May have to write "matchgroup=group"
    எனில் (last_matchgroup != spp->sp_syn_match_id)
    {
	last_matchgroup = spp->sp_syn_match_id;
	செய்தி_puts_attr("matchgroup", attr);
	செய்தி_putchar('=');
	எனில் (last_matchgroup == 0)
	    செய்தி_outtrans((char_u *)"NONE");
	இல்லையெனில்
	    செய்தி_outtrans(highlight_group_name(last_matchgroup - 1));
	செய்தி_putchar(' ');
    }

    // output the name of the pattern and an '=' or ' '
    செய்தி_puts_attr(s, attr);
    செய்தி_putchar(c);

    // output the pattern, in between a char that is not in the pattern
    க்கு (i = 0; vim_strchr(spp->sp_pattern, sepchars[i]) != வெற்று; )
	எனில் (sepchars[++i] == NUL)
	{
	    i = 0;	// no good char found, just use the first one
	    நிறுத்து;
	}
    செய்தி_putchar(sepchars[i]);
    செய்தி_outtrans(spp->sp_pattern);
    செய்தி_putchar(sepchars[i]);

    // output any pattern options
    first = TRUE;
    க்கு (i = 0; i < SPO_மொத்தம்; ++i)
    {
	mask = (1 << i);
	எனில் (spp->sp_off_flags & (mask + (mask << SPO_மொத்தம்)))
	{
	    எனில் (!first)
		செய்தி_putchar(',');	// separate with commas
	    செய்தி_puts(spo_name_tab[i]);
	    n = spp->sp_offsets[i];
	    எனில் (i != SPO_LC_OFF)
	    {
		எனில் (spp->sp_off_flags & mask)
		    செய்தி_putchar('s');
		இல்லையெனில்
		    செய்தி_putchar('e');
		எனில் (n > 0)
		    செய்தி_putchar('+');
	    }
	    எனில் (n || i == SPO_LC_OFF)
		செய்தி_outnum(n);
	    first = FALSE;
	}
    }
    செய்தி_putchar(' ');
}

/*
 * List or clear the keywords for one syntax group.
 * Return TRUE if the header has been printed.
 */
    நிலையான முழு
syn_list_keywords(
    முழு		id,
    hashtab_T	*ht,
    முழு		did_header,		// header has already been printed
    முழு		attr)
{
    முழு		outlen;
    hashitem_T	*hi;
    keyentry_T	*kp;
    முழு		todo;
    முழு		prev_contained = 0;
    short	*prev_next_list = வெற்று;
    short	*prev_cont_in_list = வெற்று;
    முழு		prev_skipnl = 0;
    முழு		prev_skipwhite = 0;
    முழு		prev_skipempty = 0;

    /*
     * Unfortunately, this list of keywords is not sorted on alphabet but on
     * hash value...
     */
    todo = (முழு)ht->ht_used;
    க்கு (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	எனில் (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    க்கு (kp = HI2KE(hi); kp != வெற்று && !got_int; kp = kp->ke_next)
	    {
		எனில் (kp->k_syn.id == id)
		{
		    எனில் (prev_contained != (kp->flags & HL_CONTAINED)
			    || prev_skipnl != (kp->flags & HL_SKIPNL)
			    || prev_skipwhite != (kp->flags & HL_SKIPWHITE)
			    || prev_skipempty != (kp->flags & HL_SKIPEMPTY)
			    || prev_cont_in_list != kp->k_syn.cont_in_list
			    || prev_next_list != kp->next_list)
			outlen = 9999;
		    இல்லையெனில்
			outlen = (முழு)STRLEN(kp->keyword);
		    // output "contained" and "nextgroup" on each line
		    எனில் (syn_list_header(did_header, outlen, id))
		    {
			prev_contained = 0;
			prev_next_list = வெற்று;
			prev_cont_in_list = வெற்று;
			prev_skipnl = 0;
			prev_skipwhite = 0;
			prev_skipempty = 0;
		    }
		    did_header = TRUE;
		    எனில் (prev_contained != (kp->flags & HL_CONTAINED))
		    {
			செய்தி_puts_attr("contained", attr);
			செய்தி_putchar(' ');
			prev_contained = (kp->flags & HL_CONTAINED);
		    }
		    எனில் (kp->k_syn.cont_in_list != prev_cont_in_list)
		    {
			put_id_list((char_u *)"containedin",
						kp->k_syn.cont_in_list, attr);
			செய்தி_putchar(' ');
			prev_cont_in_list = kp->k_syn.cont_in_list;
		    }
		    எனில் (kp->next_list != prev_next_list)
		    {
			put_id_list((char_u *)"nextgroup", kp->next_list, attr);
			செய்தி_putchar(' ');
			prev_next_list = kp->next_list;
			எனில் (kp->flags & HL_SKIPNL)
			{
			    செய்தி_puts_attr("skipnl", attr);
			    செய்தி_putchar(' ');
			    prev_skipnl = (kp->flags & HL_SKIPNL);
			}
			எனில் (kp->flags & HL_SKIPWHITE)
			{
			    செய்தி_puts_attr("skipwhite", attr);
			    செய்தி_putchar(' ');
			    prev_skipwhite = (kp->flags & HL_SKIPWHITE);
			}
			எனில் (kp->flags & HL_SKIPEMPTY)
			{
			    செய்தி_puts_attr("skipempty", attr);
			    செய்தி_putchar(' ');
			    prev_skipempty = (kp->flags & HL_SKIPEMPTY);
			}
		    }
		    செய்தி_outtrans(kp->keyword);
		}
	    }
	}
    }

    திரும்பு did_header;
}

    நிலையான வெறுமை
syn_clear_keyword(முழு id, hashtab_T *ht)
{
    hashitem_T	*hi;
    keyentry_T	*kp;
    keyentry_T	*kp_prev;
    keyentry_T	*kp_next;
    முழு		todo;

    hash_lock(ht);
    todo = (முழு)ht->ht_used;
    FOR_ALL_HASHTAB_பொருட்கள்(ht, hi, todo)
    {
	எனில் (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    kp_prev = வெற்று;
	    க்கு (kp = HI2KE(hi); kp != வெற்று; )
	    {
		எனில் (kp->k_syn.id == id)
		{
		    kp_next = kp->ke_next;
		    எனில் (kp_prev == வெற்று)
		    {
			எனில் (kp_next == வெற்று)
			    hash_remove(ht, hi, "syntax clear keyword");
			இல்லையெனில்
			    hi->hi_சாவி = KE2HIKEY(kp_next);
		    }
		    இல்லையெனில்
			kp_prev->ke_next = kp_next;
		    vim_free(kp->next_list);
		    vim_free(kp->k_syn.cont_in_list);
		    vim_free(kp);
		    kp = kp_next;
		}
		இல்லையெனில்
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
    நிலையான வெறுமை
clear_keywtab(hashtab_T *ht)
{
    hashitem_T	*hi;
    முழு		todo;
    keyentry_T	*kp;
    keyentry_T	*kp_next;

    todo = (முழு)ht->ht_used;
    FOR_ALL_HASHTAB_பொருட்கள்(ht, hi, todo)
    {
	எனில் (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    க்கு (kp = HI2KE(hi); kp != வெற்று; kp = kp_next)
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
    நிலையான வெறுமை
add_keyword(
    char_u	*name,	    // name of keyword
    size_t	namelen,    // length of keyword (excluding the NUL)
    முழு		id,	    // group ID for this keyword
    முழு		flags,	    // flags for this keyword
    short	*cont_in_list, // containedin for this keyword
    short	*next_list, // nextgroup for this keyword
    முழு		conceal_char)
{
    keyentry_T	*kp;
    hashtab_T	*ht;
    hashitem_T	*hi;
    char_u	*name_ic;
    size_t	name_iclen;
    long_u	hash;
    char_u	name_folded[MAXKEYWLEN + 1];

    எனில் (curwin->w_s->b_syn_ic)
    {
	name_ic = str_foldcase(name, (முழு)namelen,
						 name_folded, MAXKEYWLEN + 1);
	name_iclen = STRLEN(name_ic);
    }
    இல்லையெனில்
    {
	name_ic = name;
	name_iclen = namelen;
    }
    kp = alloc(offsetof(keyentry_T, keyword) + name_iclen + 1);
    எனில் (kp == வெற்று)
	திரும்பு;
    STRCPY(kp->keyword, name_ic);
    kp->k_syn.id = id;
    kp->k_syn.inc_tag = current_syn_inc_tag;
    kp->flags = flags;
    kp->k_char = conceal_char;
    kp->k_syn.cont_in_list = copy_id_list(cont_in_list);
    எனில் (cont_in_list != வெற்று)
	curwin->w_s->b_syn_containedin = TRUE;
    kp->next_list = copy_id_list(next_list);

    எனில் (curwin->w_s->b_syn_ic)
	ht = &curwin->w_s->b_keywtab_ic;
    இல்லையெனில்
	ht = &curwin->w_s->b_keywtab;

    hash = hash_hash(kp->keyword);
    hi = hash_lookup(ht, kp->keyword, hash);
    எனில் (HASHITEM_EMPTY(hi))
    {
	// new keyword, add to hashtable
	kp->ke_next = வெற்று;
	hash_add_item(ht, hi, kp->keyword, hash);
    }
    இல்லையெனில்
    {
	// keyword already exists, prepend to list
	kp->ke_next = HI2KE(hi);
	hi->hi_சாவி = KE2HIKEY(kp);
    }
}

/*
 * Get the start and end of the group name argument.
 * Return a pointer to the first argument.
 * Return NULL if the end of the command was found instead of further args.
 */
    நிலையான char_u *
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
    எனில் (ends_excmd(*arg) || *rest == NUL)
	திரும்பு வெற்று;
    திரும்பு rest;
}

/*
 * Check for syntax command option arguments.
 * This can be called at any place in the list of arguments, and just picks
 * out the arguments that are known.  Can be called several times in a row to
 * collect all options in between other arguments.
 * Return a pointer to the next argument (which isn't an option).
 * Return NULL for any error;
 */
    நிலையான char_u *
get_syn_options(
    char_u	    *start,		// next argument to be checked
    syn_opt_arg_T   *opt,		// various things
    முழு		    *conceal_char UNUSED,
    முழு		    skip)		// TRUE if skipping over command
{
    char_u	*arg = start;
    char_u	*gname_start, *gname;
    முழு		syn_id;
    முழு		len;
    எழுத்து	*p;
    முழு		i;
    முழு		fidx;
    நிலையான கட்டமைப்பு flag
    {
	எழுத்து	*name;
	முழு	argtype;
	முழு	flags;
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
    நிலையான எழுத்து *first_letters = "cCoOkKeEtTsSgGdDfFnN";

    எனில் (arg == வெற்று)		// already detected error
	திரும்பு வெற்று;

#ifdef FEAT_CONCEAL
    எனில் (curwin->w_s->b_syn_conceal)
	opt->flags |= HL_CONCEAL;
#endif

    க்கு (;;)
    {
	/*
	 * This is used very often when a large number of keywords is defined.
	 * Need to skip quickly when no option name is found.
	 * Also avoid tolower(), it's slow.
	 */
	எனில் (strchr(first_letters, *arg) == வெற்று)
	    நிறுத்து;

	க்கு (fidx = ARRAY_LENGTH(flagtab); --fidx >= 0; )
	{
	    p = flagtab[fidx].name;
	    க்கு (i = 0, len = 0; p[i] != NUL; i += 2, ++len)
		எனில் (arg[len] != p[i] && arg[len] != p[i + 1])
		    நிறுத்து;
	    எனில் (p[i] == NUL && (VIM_ISWHITE(arg[len])
				    || (flagtab[fidx].argtype > 0
					 ? arg[len] == '='
					 : ends_excmd2(start, arg + len))))
	    {
		எனில் (opt->keyword
			&& (flagtab[fidx].flags == HL_DISPLAY
			    || flagtab[fidx].flags == HL_FOLD
			    || flagtab[fidx].flags == HL_EXTEND))
		    // treat "display", "fold" and "extend" as a keyword
		    fidx = -1;
		நிறுத்து;
	    }
	}
	எனில் (fidx < 0)	    // no match found
	    நிறுத்து;

	எனில் (flagtab[fidx].argtype == 1)
	{
	    எனில் (!opt->has_cont_list)
	    {
		emsg(_(e_contains_argument_not_accepted_here));
		திரும்பு வெற்று;
	    }
	    எனில் (get_id_list(&arg, 8, &opt->cont_list, skip) == FAIL)
		திரும்பு வெற்று;
	}
	இல்லையெனில் எனில் (flagtab[fidx].argtype == 2)
	{
	    எனில் (get_id_list(&arg, 11, &opt->cont_in_list, skip) == FAIL)
		திரும்பு வெற்று;
	}
	இல்லையெனில் எனில் (flagtab[fidx].argtype == 3)
	{
	    எனில் (get_id_list(&arg, 9, &opt->next_list, skip) == FAIL)
		திரும்பு வெற்று;
	}
	இல்லையெனில் எனில் (flagtab[fidx].argtype == 11 && arg[5] == '=')
	{
	    // cchar=?
	    எனில் (has_mbyte)
	    {
#ifdef FEAT_CONCEAL
		*conceal_char = mb_ptr2char(arg + 6);
#endif
		arg += mb_ptr2len(arg + 6) - 1;
	    }
	    இல்லையெனில்
	    {
#ifdef FEAT_CONCEAL
		*conceal_char = arg[6];
#இல்லையெனில்
		;
#endif
	    }
#ifdef FEAT_CONCEAL
	    எனில் (!vim_isprintc_strict(*conceal_char))
	    {
		emsg(_(e_invalid_cchar_மதிப்பு));
		திரும்பு வெற்று;
	    }
#endif
	    arg = skipwhite(arg + 7);
	}
	இல்லையெனில்
	{
	    opt->flags |= flagtab[fidx].flags;
	    arg = skipwhite(arg + len);

	    எனில் (flagtab[fidx].flags == HL_SYNC_HERE
		    || flagtab[fidx].flags == HL_SYNC_THERE)
	    {
		எனில் (opt->sync_idx == வெற்று)
		{
		    emsg(_(e_groupthere_not_accepted_here));
		    திரும்பு வெற்று;
		}
		gname_start = arg;
		arg = skiptowhite(arg);
		எனில் (gname_start == arg)
		    திரும்பு வெற்று;
		gname = vim_strnsave(gname_start, arg - gname_start);
		எனில் (gname == வெற்று)
		    திரும்பு வெற்று;
		எனில் (STRCMP(gname, "NONE") == 0)
		    *opt->sync_idx = NONE_IDX;
		இல்லையெனில்
		{
		    syn_id = syn_name2id(gname);
		    க்கு (i = curwin->w_s->b_syn_patterns.ga_len; --i >= 0; )
			எனில் (SYN_பொருட்கள்(curwin->w_s)[i].sp_syn.id == syn_id
			      && SYN_பொருட்கள்(curwin->w_s)[i].sp_type
							       == SPTYPE_START)
			{
			    *opt->sync_idx = i;
			    நிறுத்து;
			}
		    எனில் (i < 0)
		    {
			semsg(_(e_didnt_find_region_item_for_str), gname);
			vim_free(gname);
			திரும்பு வெற்று;
		    }
		}

		vim_free(gname);
		arg = skipwhite(arg);
	    }
#ifdef FEAT_FOLDING
	    இல்லையெனில் எனில் (flagtab[fidx].flags == HL_FOLD
						&& foldmethodIsSyntax(curwin))
		// Need to update folds later.
		foldUpdateAll(curwin);
#endif
	}
    }

    திரும்பு arg;
}

/*
 * Adjustments to syntax item when declared in a ":syn include"'d file.
 * Set the contained flag, and if the item is not already contained, add it
 * to the specified top-level group, if any.
 */
    நிலையான வெறுமை
syn_incl_toplevel(முழு id, முழு *flagsp)
{
    எனில் ((*flagsp & HL_CONTAINED) || curwin->w_s->b_syn_topgrp == 0)
	திரும்பு;
    *flagsp |= HL_CONTAINED | HL_INCLUDED_TOPLEVEL;
    எனில் (curwin->w_s->b_syn_topgrp >= SYNID_CLUSTER)
    {
	// We have to alloc this, because syn_combine_list() will free it.
	short	    *grp_list = ALLOC_MULT(short, 2);
	முழு	    tlg_id = curwin->w_s->b_syn_topgrp - SYNID_CLUSTER;

	எனில் (grp_list != வெற்று)
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
    நிலையான வெறுமை
syn_cmd_include(exarg_T *eap, முழு syncing UNUSED)
{
    char_u	*arg = eap->arg;
    முழு		sgl_id = 1;
    char_u	*group_name_end;
    char_u	*rest;
    எழுத்து	*errormsg = வெற்று;
    முழு		prev_toplvl_grp;
    முழு		prev_syn_inc_tag;
    முழு		source = FALSE;

    eap->nextcmd = find_nextcmd(arg);
    எனில் (eap->skip)
	திரும்பு;

    எனில் (arg[0] == '@')
    {
	++arg;
	rest = get_group_name(arg, &group_name_end);
	எனில் (rest == வெற்று)
	{
	    emsg(_(e_filename_required));
	    திரும்பு;
	}
	sgl_id = syn_check_cluster(arg, (முழு)(group_name_end - arg));
	எனில் (sgl_id == 0)
	    திரும்பு;
	// separate_nextcmd() and expand_filename() depend on this
	eap->arg = rest;
    }

    /*
     * Everything that's left, up to the next command, should be the
     * filename to include.
     */
    eap->argt |= (EX_XFILE | EX_NOSPC);
    separate_nextcmd(eap, FALSE);
    எனில் (*eap->arg == '<' || *eap->arg == '$' || mch_isFullName(eap->arg))
    {
	// For an absolute path, "$VIM/..." or "<sfile>.." we ":source" the
	// file.  Need to expand the file name first.  In other cases
	// ":runtime!" is used.
	source = TRUE;
	எனில் (expand_filename(eap, syn_cmdlinep, &errormsg) == FAIL)
	{
	    எனில் (errormsg != வெற்று)
		emsg(errormsg);
	    திரும்பு;
	}
    }

    /*
     * Save and restore the existing top-level grouplist id and ":syn
     * include" tag around the actual inclusion.
     */
    எனில் (running_syn_inc_tag >= MAX_SYN_INC_TAG)
    {
	emsg(_(e_too_many_syntax_includes));
	திரும்பு;
    }
    prev_syn_inc_tag = current_syn_inc_tag;
    current_syn_inc_tag = ++running_syn_inc_tag;
    prev_toplvl_grp = curwin->w_s->b_syn_topgrp;
    curwin->w_s->b_syn_topgrp = sgl_id;
    எனில் (source ? do_source(eap->arg, FALSE, DOSO_NONE, வெற்று) == FAIL
				: source_runtime(eap->arg, DIP_ALL) == FAIL)
	semsg(_(e_cant_open_file_str), eap->arg);
    curwin->w_s->b_syn_topgrp = prev_toplvl_grp;
    current_syn_inc_tag = prev_syn_inc_tag;
}

/*
 * Handle ":syntax keyword {group-name} [{option}] keyword .." command.
 */
    நிலையான வெறுமை
syn_cmd_keyword(exarg_T *eap, முழு syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*group_name_end;
    முழு		syn_id;
    char_u	*rest;
    char_u	*keyword_copy = வெற்று;
    char_u	*p;
    char_u	*kw;
    syn_opt_arg_T syn_opt_arg;
    முழு		cnt;
    முழு		conceal_char = NUL;

    rest = get_group_name(arg, &group_name_end);

    எனில் (rest != வெற்று)
    {
	எனில் (eap->skip)
	    syn_id = -1;
	இல்லையெனில்
	    syn_id = syn_check_group(arg, (முழு)(group_name_end - arg));
	எனில் (syn_id != 0)
	    // allocate a buffer, for removing backslashes in the keyword
	    keyword_copy = alloc(STRLEN(rest) + 1);
	எனில் (keyword_copy != வெற்று)
	{
	    syn_opt_arg.flags = 0;
	    syn_opt_arg.keyword = TRUE;
	    syn_opt_arg.sync_idx = வெற்று;
	    syn_opt_arg.has_cont_list = FALSE;
	    syn_opt_arg.cont_in_list = வெற்று;
	    syn_opt_arg.next_list = வெற்று;

	    /*
	     * The options given apply to ALL keywords, so all options must be
	     * found before keywords can be created.
	     * 1: collect the options and copy the keywords to keyword_copy.
	     */
	    cnt = 0;
	    p = keyword_copy;
	    க்கு ( ; rest != வெற்று && !ends_excmd2(eap->arg, rest);
							rest = skipwhite(rest))
	    {
		rest = get_syn_options(rest, &syn_opt_arg, &conceal_char,
								    eap->skip);
		எனில் (rest == வெற்று || ends_excmd2(eap->arg, rest))
		    நிறுத்து;
		// Copy the keyword, removing backslashes, and add a NUL.
		போது (*rest != NUL && !VIM_ISWHITE(*rest))
		{
		    எனில் (*rest == '\\' && rest[1] != NUL)
			++rest;
		    *p++ = *rest++;
		}
		*p++ = NUL;
		++cnt;
	    }

	    எனில் (!eap->skip)
	    {
		size_t	kwlen = 0;

		// Adjust flags for use of ":syn include".
		syn_incl_toplevel(syn_id, &syn_opt_arg.flags);

		/*
		 * 2: Add an entry for each keyword.
		 */
		க்கு (kw = keyword_copy; --cnt >= 0; kw += kwlen + 1)
		{
		    க்கு (p = vim_strchr(kw, '['); ; )
		    {
			எனில் (p == வெற்று)
			    kwlen = STRLEN(kw);
			இல்லையெனில்
			{
			    *p = NUL;
			    kwlen = (size_t)(p - kw);
			}
			add_keyword(kw, kwlen, syn_id, syn_opt_arg.flags,
				syn_opt_arg.cont_in_list,
					 syn_opt_arg.next_list, conceal_char);
			எனில் (p == வெற்று)
			    நிறுத்து;
			எனில் (p[1] == NUL)
			{
			    semsg(_(e_error_missing_rsb_str), kw);
			    செல் error;
			}
			எனில் (p[1] == ']')
			{
			    எனில் (p[2] != NUL)
			    {
				semsg(_(e_trailing_char_after_rsb_str_str),
								    kw, &p[2]);
				செல் error;
			    }
			    kw = p + 1;		// skip over the "]"
			    kwlen = 1;
			    நிறுத்து;
			}
			எனில் (has_mbyte)
			{
			    முழு l = (*mb_ptr2len)(p + 1);

			    mch_memmove(p, p + 1, l);
			    p += l;
			}
			இல்லையெனில்
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

    எனில் (rest != வெற்று)
	set_nextcmd(eap, rest);
    இல்லையெனில்
	semsg(_(e_invalid_argument_str), arg);

    redraw_curbuf_later(UPD_SOME_VALID);
    syn_அடுக்கு_free_all(curwin->w_s);		// Need to recompute all syntax.
}

/*
 * Handle ":syntax match {name} [{options}] {pattern} [{options}]".
 *
 * Also ":syntax sync match {name} [[grouphere | groupthere] {group-name}] .."
 */
    நிலையான வெறுமை
syn_cmd_match(
    exarg_T	*eap,
    முழு		syncing)	    // TRUE for ":syntax sync match .. "
{
    char_u	*arg = eap->arg;
    char_u	*group_name_end;
    char_u	*rest;
    synpat_T	item;		// the item found in the line
    முழு		syn_id;
    முழு		idx;
    syn_opt_arg_T syn_opt_arg;
    முழு		sync_idx = 0;
    முழு		conceal_char = NUL;
    முழு		orig_called_emsg = called_emsg;

    // Isolate the group name, check for validity
    rest = get_group_name(arg, &group_name_end);

    // Get options before the pattern
    syn_opt_arg.flags = 0;
    syn_opt_arg.keyword = FALSE;
    syn_opt_arg.sync_idx = syncing ? &sync_idx : வெற்று;
    syn_opt_arg.has_cont_list = TRUE;
    syn_opt_arg.cont_list = வெற்று;
    syn_opt_arg.cont_in_list = வெற்று;
    syn_opt_arg.next_list = வெற்று;
    rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);

    // get the pattern.
    init_syn_patterns();
    CLEAR_FIELD(item);
    rest = get_syn_pattern(rest, &item);
    எனில் (vim_regcomp_had_eol() && !(syn_opt_arg.flags & HL_EXCLUDENL))
	syn_opt_arg.flags |= HL_HAS_EOL;

    // Get options after the pattern
    rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);

    எனில் (rest != வெற்று)		// all arguments are valid
    {
	/*
	 * Check for trailing command and illegal trailing arguments.
	 */
	set_nextcmd(eap, rest);
	எனில் (!ends_excmd2(eap->cmd, rest) || eap->skip)
	    rest = வெற்று;
	இல்லையெனில் எனில் (ga_grow(&curwin->w_s->b_syn_patterns, 1) == OK
		&& (syn_id = syn_check_group(arg,
					   (முழு)(group_name_end - arg))) != 0)
	{
	    syn_incl_toplevel(syn_id, &syn_opt_arg.flags);
	    /*
	     * Store the pattern in the syn_items list
	     */
	    idx = curwin->w_s->b_syn_patterns.ga_len;
	    SYN_பொருட்கள்(curwin->w_s)[idx] = item;
	    SYN_பொருட்கள்(curwin->w_s)[idx].sp_syncing = syncing;
	    SYN_பொருட்கள்(curwin->w_s)[idx].sp_type = SPTYPE_MATCH;
	    SYN_பொருட்கள்(curwin->w_s)[idx].sp_syn.id = syn_id;
	    SYN_பொருட்கள்(curwin->w_s)[idx].sp_syn.inc_tag = current_syn_inc_tag;
	    SYN_பொருட்கள்(curwin->w_s)[idx].sp_flags = syn_opt_arg.flags;
	    SYN_பொருட்கள்(curwin->w_s)[idx].sp_sync_idx = sync_idx;
	    SYN_பொருட்கள்(curwin->w_s)[idx].sp_cont_list = syn_opt_arg.cont_list;
	    SYN_பொருட்கள்(curwin->w_s)[idx].sp_syn.cont_in_list =
						     syn_opt_arg.cont_in_list;
#ifdef FEAT_CONCEAL
	    SYN_பொருட்கள்(curwin->w_s)[idx].sp_cchar = conceal_char;
#endif
	    எனில் (syn_opt_arg.cont_in_list != வெற்று)
		curwin->w_s->b_syn_containedin = TRUE;
	    SYN_பொருட்கள்(curwin->w_s)[idx].sp_next_list = syn_opt_arg.next_list;
	    ++curwin->w_s->b_syn_patterns.ga_len;

	    // remember that we found a match for syncing on
	    எனில் (syn_opt_arg.flags & (HL_SYNC_HERE|HL_SYNC_THERE))
		curwin->w_s->b_syn_sync_flags |= SF_MATCH;
#ifdef FEAT_FOLDING
	    எனில் (syn_opt_arg.flags & HL_FOLD)
		++curwin->w_s->b_syn_folditems;
#endif

	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_அடுக்கு_free_all(curwin->w_s);	// Need to recompute all syntax.
	    திரும்பு;	// don't free the progs and patterns now
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

    எனில் (rest == வெற்று && called_emsg == orig_called_emsg)
	semsg(_(e_invalid_argument_str), arg);
}

/*
 * Handle ":syntax region {group-name} [matchgroup={group-name}]
 *		start {start} .. [skip {skip}] end {end} .. [{options}]".
 */
    நிலையான வெறுமை
syn_cmd_region(
    exarg_T	*eap,
    முழு		syncing)	    // TRUE for ":syntax sync region .."
{
    char_u		*arg = eap->arg;
    char_u		*group_name_end;
    char_u		*rest;			// next arg, NULL on error
    char_u		*சாவி_end;
    char_u		*சாவி = வெற்று;
    char_u		*p;
    முழு			item;
#define ITEM_START	    0
#define ITEM_SKIP	    1
#define ITEM_END	    2
#define ITEM_MATCHGROUP	    3
    கட்டமைப்பு pat_ptr
    {
	synpat_T	*pp_synp;		// pointer to syn_pattern
	முழு		pp_matchgroup_id;	// matchgroup ID
	கட்டமைப்பு pat_ptr	*pp_next;		// pointer to next pat_ptr
    }			*(pat_ptrs[3]);
					// patterns found in the line
    கட்டமைப்பு pat_ptr	*ppp;
    கட்டமைப்பு pat_ptr	*ppp_next;
    முழு			pat_மொத்தம் = 0;		// nr of syn_patterns found
    முழு			syn_id;
    முழு			matchgroup_id = 0;
    முழு			not_enough = FALSE;	// not enough arguments
    முழு			illegal = FALSE;	// illegal arguments
    முழு			success = FALSE;
    முழு			idx;
    syn_opt_arg_T	syn_opt_arg;
    முழு			conceal_char = NUL;

    // Isolate the group name, check for validity
    rest = get_group_name(arg, &group_name_end);

    pat_ptrs[0] = வெற்று;
    pat_ptrs[1] = வெற்று;
    pat_ptrs[2] = வெற்று;

    init_syn_patterns();

    syn_opt_arg.flags = 0;
    syn_opt_arg.keyword = FALSE;
    syn_opt_arg.sync_idx = வெற்று;
    syn_opt_arg.has_cont_list = TRUE;
    syn_opt_arg.cont_list = வெற்று;
    syn_opt_arg.cont_in_list = வெற்று;
    syn_opt_arg.next_list = வெற்று;

    /*
     * get the options, patterns and matchgroup.
     */
    போது (rest != வெற்று && !ends_excmd2(eap->cmd, rest))
    {
	// Check for option arguments
	rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);
	எனில் (rest == வெற்று || ends_excmd2(eap->cmd, rest))
	    நிறுத்து;

	// must be a pattern or matchgroup then
	சாவி_end = rest;
	போது (*சாவி_end && !VIM_ISWHITE(*சாவி_end) && *சாவி_end != '=')
	    ++சாவி_end;
	vim_free(சாவி);
	சாவி = vim_strnsave_up(rest, சாவி_end - rest);
	எனில் (சாவி == வெற்று)			// out of memory
	{
	    rest = வெற்று;
	    நிறுத்து;
	}
	எனில் (STRCMP(சாவி, "MATCHGROUP") == 0)
	    item = ITEM_MATCHGROUP;
	இல்லையெனில் எனில் (STRCMP(சாவி, "START") == 0)
	    item = ITEM_START;
	இல்லையெனில் எனில் (STRCMP(சாவி, "END") == 0)
	    item = ITEM_END;
	இல்லையெனில் எனில் (STRCMP(சாவி, "SKIP") == 0)
	{
	    எனில் (pat_ptrs[ITEM_SKIP] != வெற்று)	// one skip pattern allowed
	    {
		illegal = TRUE;
		நிறுத்து;
	    }
	    item = ITEM_SKIP;
	}
	இல்லையெனில்
	    நிறுத்து;
	rest = skipwhite(சாவி_end);
	எனில் (*rest != '=')
	{
	    rest = வெற்று;
	    semsg(_(e_missing_equal_str), arg);
	    நிறுத்து;
	}
	rest = skipwhite(rest + 1);
	எனில் (*rest == NUL)
	{
	    not_enough = TRUE;
	    நிறுத்து;
	}

	எனில் (item == ITEM_MATCHGROUP)
	{
	    p = skiptowhite(rest);
	    எனில் ((p - rest == 4 && STRNCMP(rest, "NONE", 4) == 0) || eap->skip)
		matchgroup_id = 0;
	    இல்லையெனில்
	    {
		matchgroup_id = syn_check_group(rest, (முழு)(p - rest));
		எனில் (matchgroup_id == 0)
		{
		    illegal = TRUE;
		    நிறுத்து;
		}
	    }
	    rest = skipwhite(p);
	}
	இல்லையெனில்
	{
	    /*
	     * Allocate room for a syn_pattern, and link it in the list of
	     * syn_patterns for this item, at the start (because the list is
	     * used from end to start).
	     */
	    ppp = ALLOC_ONE(கட்டமைப்பு pat_ptr);
	    எனில் (ppp == வெற்று)
	    {
		rest = வெற்று;
		நிறுத்து;
	    }
	    ppp->pp_next = pat_ptrs[item];
	    pat_ptrs[item] = ppp;
	    ppp->pp_synp = ALLOC_CLEAR_ONE(synpat_T);
	    எனில் (ppp->pp_synp == வெற்று)
	    {
		rest = வெற்று;
		நிறுத்து;
	    }

	    /*
	     * Get the syntax pattern and the following offset(s).
	     */
	    // Enable the appropriate \z specials.
	    எனில் (item == ITEM_START)
		reg_do_extmatch = REX_SET;
	    இல்லையெனில் எனில் (item == ITEM_SKIP || item == ITEM_END)
		reg_do_extmatch = REX_USE;
	    rest = get_syn_pattern(rest, ppp->pp_synp);
	    reg_do_extmatch = 0;
	    எனில் (item == ITEM_END && vim_regcomp_had_eol()
				       && !(syn_opt_arg.flags & HL_EXCLUDENL))
		ppp->pp_synp->sp_flags |= HL_HAS_EOL;
	    ppp->pp_matchgroup_id = matchgroup_id;
	    ++pat_மொத்தம்;
	}
    }
    vim_free(சாவி);
    எனில் (illegal || not_enough)
	rest = வெற்று;

    /*
     * Must have a "start" and "end" pattern.
     */
    எனில் (rest != வெற்று && (pat_ptrs[ITEM_START] == வெற்று ||
						  pat_ptrs[ITEM_END] == வெற்று))
    {
	not_enough = TRUE;
	rest = வெற்று;
    }

    எனில் (rest != வெற்று)
    {
	/*
	 * Check for trailing garbage or command.
	 * If OK, add the item.
	 */
	set_nextcmd(eap, rest);
	எனில் (!ends_excmd(*rest) || eap->skip)
	    rest = வெற்று;
	இல்லையெனில் எனில் (ga_grow(&(curwin->w_s->b_syn_patterns), pat_மொத்தம்) == OK
		&& (syn_id = syn_check_group(arg,
					   (முழு)(group_name_end - arg))) != 0)
	{
	    syn_incl_toplevel(syn_id, &syn_opt_arg.flags);
	    /*
	     * Store the start/skip/end in the syn_items list
	     */
	    idx = curwin->w_s->b_syn_patterns.ga_len;
	    க்கு (item = ITEM_START; item <= ITEM_END; ++item)
	    {
		க்கு (ppp = pat_ptrs[item]; ppp != வெற்று; ppp = ppp->pp_next)
		{
		    SYN_பொருட்கள்(curwin->w_s)[idx] = *(ppp->pp_synp);
		    SYN_பொருட்கள்(curwin->w_s)[idx].sp_syncing = syncing;
		    SYN_பொருட்கள்(curwin->w_s)[idx].sp_type =
			    (item == ITEM_START) ? SPTYPE_START :
			    (item == ITEM_SKIP) ? SPTYPE_SKIP : SPTYPE_END;
		    SYN_பொருட்கள்(curwin->w_s)[idx].sp_flags |= syn_opt_arg.flags;
		    SYN_பொருட்கள்(curwin->w_s)[idx].sp_syn.id = syn_id;
		    SYN_பொருட்கள்(curwin->w_s)[idx].sp_syn.inc_tag =
							  current_syn_inc_tag;
		    SYN_பொருட்கள்(curwin->w_s)[idx].sp_syn_match_id =
							ppp->pp_matchgroup_id;
#ifdef FEAT_CONCEAL
		    SYN_பொருட்கள்(curwin->w_s)[idx].sp_cchar = conceal_char;
#endif
		    எனில் (item == ITEM_START)
		    {
			SYN_பொருட்கள்(curwin->w_s)[idx].sp_cont_list =
							syn_opt_arg.cont_list;
			SYN_பொருட்கள்(curwin->w_s)[idx].sp_syn.cont_in_list =
						     syn_opt_arg.cont_in_list;
			எனில் (syn_opt_arg.cont_in_list != வெற்று)
			    curwin->w_s->b_syn_containedin = TRUE;
			SYN_பொருட்கள்(curwin->w_s)[idx].sp_next_list =
							syn_opt_arg.next_list;
		    }
		    ++curwin->w_s->b_syn_patterns.ga_len;
		    ++idx;
#ifdef FEAT_FOLDING
		    எனில் (syn_opt_arg.flags & HL_FOLD)
			++curwin->w_s->b_syn_folditems;
#endif
		}
	    }

	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_அடுக்கு_free_all(curwin->w_s);	// Need to recompute all syntax.
	    success = TRUE;	    // don't free the progs and patterns now
	}
    }

    /*
     * Free the allocated memory.
     */
    க்கு (item = ITEM_START; item <= ITEM_END; ++item)
	க்கு (ppp = pat_ptrs[item]; ppp != வெற்று; ppp = ppp_next)
	{
	    எனில் (!success && ppp->pp_synp != வெற்று)
	    {
		vim_regfree(ppp->pp_synp->sp_prog);
		vim_free(ppp->pp_synp->sp_pattern);
	    }
	    vim_free(ppp->pp_synp);
	    ppp_next = ppp->pp_next;
	    vim_free(ppp);
	}

    எனில் (!success)
    {
	vim_free(syn_opt_arg.cont_list);
	vim_free(syn_opt_arg.cont_in_list);
	vim_free(syn_opt_arg.next_list);
	எனில் (not_enough)
	    semsg(_(e_not_enough_arguments_syntax_region_str), arg);
	இல்லையெனில் எனில் (illegal || rest == வெற்று)
	    semsg(_(e_invalid_argument_str), arg);
    }
}

/*
 * A simple syntax group ID comparison function suitable for use in qsort()
 */
    நிலையான முழு
syn_compare_stub(மாறிலி வெறுமை *v1, மாறிலி வெறுமை *v2)
{
    மாறிலி short	*s1 = v1;
    மாறிலி short	*s2 = v2;

    திரும்பு (*s1 > *s2 ? 1 : *s1 < *s2 ? -1 : 0);
}

/*
 * Combines lists of syntax clusters.
 * *clstr1 and *clstr2 must both be allocated memory; they will be consumed.
 */
    நிலையான வெறுமை
syn_combine_list(short **clstr1, short **clstr2, முழு list_op)
{
    முழு		count1 = 0;
    முழு		count2 = 0;
    short	*g1;
    short	*g2;
    short	*clstr = வெற்று;
    முழு		மொத்தம்;
    முழு		round;

    /*
     * Handle degenerate cases.
     */
    எனில் (*clstr2 == வெற்று)
	திரும்பு;
    எனில் (*clstr1 == வெற்று || list_op == CLUSTER_REPLACE)
    {
	எனில் (list_op == CLUSTER_REPLACE)
	    vim_free(*clstr1);
	எனில் (list_op == CLUSTER_REPLACE || list_op == CLUSTER_ADD)
	    *clstr1 = *clstr2;
	இல்லையெனில்
	    vim_free(*clstr2);
	திரும்பு;
    }

    க்கு (g1 = *clstr1; *g1; g1++)
	++count1;
    க்கு (g2 = *clstr2; *g2; g2++)
	++count2;

    /*
     * For speed purposes, sort both lists.
     */
    qsort(*clstr1, (size_t)count1, அளவு(short), syn_compare_stub);
    qsort(*clstr2, (size_t)count2, அளவு(short), syn_compare_stub);

    /*
     * We proceed in two passes; in round 1, we count the elements to place
     * in the new list, and in round 2, we allocate and populate the new
     * list.  For speed, we use a mergesort-like method, adding the smaller
     * of the current elements in each list to the new list.
     */
    க்கு (round = 1; round <= 2; round++)
    {
	g1 = *clstr1;
	g2 = *clstr2;
	மொத்தம் = 0;

	/*
	 * First, loop through the lists until one of them is empty.
	 */
	போது (*g1 && *g2)
	{
	    /*
	     * We always want to add from the first list.
	     */
	    எனில் (*g1 < *g2)
	    {
		எனில் (round == 2)
		    clstr[மொத்தம்] = *g1;
		மொத்தம்++;
		g1++;
		தொடர்;
	    }
	    /*
	     * We only want to add from the second list if we're adding the
	     * lists.
	     */
	    எனில் (list_op == CLUSTER_ADD)
	    {
		எனில் (round == 2)
		    clstr[மொத்தம்] = *g2;
		மொத்தம்++;
	    }
	    எனில் (*g1 == *g2)
		g1++;
	    g2++;
	}

	/*
	 * Now add the leftovers from whichever list didn't get finished
	 * first.  As before, we only want to add from the second list if
	 * we're adding the lists.
	 */
	க்கு (; *g1; g1++, மொத்தம்++)
	    எனில் (round == 2)
		clstr[மொத்தம்] = *g1;
	எனில் (list_op == CLUSTER_ADD)
	    க்கு (; *g2; g2++, மொத்தம்++)
		எனில் (round == 2)
		    clstr[மொத்தம்] = *g2;

	எனில் (round == 1)
	{
	    /*
	     * If the group ended up empty, we don't need to allocate any
	     * space for it.
	     */
	    எனில் (மொத்தம் == 0)
	    {
		clstr = வெற்று;
		நிறுத்து;
	    }
	    clstr = ALLOC_MULT(short, மொத்தம் + 1);
	    எனில் (clstr == வெற்று)
		நிறுத்து;
	    clstr[மொத்தம்] = 0;
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
    நிலையான முழு
syn_scl_name2id(char_u *name)
{
    முழு		i;
    char_u	*name_u;

    // Avoid using stricmp() too much, it's slow on some systems
    name_u = vim_strsave_up(name);
    எனில் (name_u == வெற்று)
	திரும்பு 0;
    க்கு (i = curwin->w_s->b_syn_clusters.ga_len; --i >= 0; )
	எனில் (SYN_CLSTR(curwin->w_s)[i].scl_name_u != வெற்று
		&& STRCMP(name_u, SYN_CLSTR(curwin->w_s)[i].scl_name_u) == 0)
	    நிறுத்து;
    vim_free(name_u);
    திரும்பு (i < 0 ? 0 : i + SYNID_CLUSTER);
}

/*
 * Like syn_scl_name2id(), but take a pointer + length argument.
 */
    நிலையான முழு
syn_scl_namen2id(char_u *linep, முழு len)
{
    char_u  *name;
    முழு	    id = 0;

    name = vim_strnsave(linep, len);
    எனில் (name == வெற்று)
	திரும்பு 0;

    id = syn_scl_name2id(name);
    vim_free(name);
    திரும்பு id;
}

/*
 * Find syntax cluster name in the table and return its ID.
 * The argument is a pointer to the name and the length of the name.
 * If it doesn't exist yet, a new entry is created.
 * Return 0 for failure.
 */
    நிலையான முழு
syn_check_cluster(char_u *pp, முழு len)
{
    முழு		id;
    char_u	*name;

    name = vim_strnsave(pp, len);
    எனில் (name == வெற்று)
	திரும்பு 0;

    id = syn_scl_name2id(name);
    எனில் (id == 0)			// doesn't exist yet
	id = syn_add_cluster(name);
    இல்லையெனில்
	vim_free(name);
    திரும்பு id;
}

/*
 * Add new syntax cluster and return its ID.
 * "name" must be an allocated string, it will be consumed.
 * Return 0 for failure.
 */
    நிலையான முழு
syn_add_cluster(char_u *name)
{
    முழு		len;

    /*
     * First call for this growarray: init growing array.
     */
    எனில் (curwin->w_s->b_syn_clusters.ga_data == வெற்று)
    {
	curwin->w_s->b_syn_clusters.ga_itemsize = அளவு(syn_cluster_T);
	curwin->w_s->b_syn_clusters.ga_growsize = 10;
    }

    len = curwin->w_s->b_syn_clusters.ga_len;
    எனில் (len >= MAX_CLUSTER_ID)
    {
	emsg(_(e_too_many_syntax_clusters));
	vim_free(name);
	திரும்பு 0;
    }

    /*
     * Make room for at least one other cluster entry.
     */
    எனில் (ga_grow(&curwin->w_s->b_syn_clusters, 1) == FAIL)
    {
	vim_free(name);
	திரும்பு 0;
    }

    CLEAR_POINTER(&(SYN_CLSTR(curwin->w_s)[len]));
    SYN_CLSTR(curwin->w_s)[len].scl_name = name;
    SYN_CLSTR(curwin->w_s)[len].scl_name_u = vim_strsave_up(name);
    SYN_CLSTR(curwin->w_s)[len].scl_list = வெற்று;
    ++curwin->w_s->b_syn_clusters.ga_len;

    எனில் (STRICMP(name, "Spell") == 0)
	curwin->w_s->b_spell_cluster_id = len + SYNID_CLUSTER;
    எனில் (STRICMP(name, "NoSpell") == 0)
	curwin->w_s->b_nospell_cluster_id = len + SYNID_CLUSTER;

    திரும்பு len + SYNID_CLUSTER;
}

/*
 * Handle ":syntax cluster {cluster-name} [contains={groupname},..]
 *		[add={groupname},..] [remove={groupname},..]".
 */
    நிலையான வெறுமை
syn_cmd_cluster(exarg_T *eap, முழு syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*group_name_end;
    char_u	*rest;
    முழு		scl_id;
    short	*clstr_list;
    முழு		got_clstr = FALSE;
    முழு		opt_len;
    முழு		list_op;

    eap->nextcmd = find_nextcmd(arg);
    எனில் (eap->skip)
	திரும்பு;

    rest = get_group_name(arg, &group_name_end);

    எனில் (rest != வெற்று)
    {
	scl_id = syn_check_cluster(arg, (முழு)(group_name_end - arg));
	எனில் (scl_id == 0)
	    திரும்பு;
	scl_id -= SYNID_CLUSTER;

	க்கு (;;)
	{
	    எனில் (STRNICMP(rest, "add", 3) == 0
		    && (VIM_ISWHITE(rest[3]) || rest[3] == '='))
	    {
		opt_len = 3;
		list_op = CLUSTER_ADD;
	    }
	    இல்லையெனில் எனில் (STRNICMP(rest, "remove", 6) == 0
		    && (VIM_ISWHITE(rest[6]) || rest[6] == '='))
	    {
		opt_len = 6;
		list_op = CLUSTER_SUBTRACT;
	    }
	    இல்லையெனில் எனில் (STRNICMP(rest, "contains", 8) == 0
			&& (VIM_ISWHITE(rest[8]) || rest[8] == '='))
	    {
		opt_len = 8;
		list_op = CLUSTER_REPLACE;
	    }
	    இல்லையெனில்
		நிறுத்து;

	    clstr_list = வெற்று;
	    எனில் (get_id_list(&rest, opt_len, &clstr_list, eap->skip) == FAIL)
	    {
		semsg(_(e_invalid_argument_str), rest);
		நிறுத்து;
	    }
	    எனில் (scl_id >= 0)
		syn_combine_list(&SYN_CLSTR(curwin->w_s)[scl_id].scl_list,
			     &clstr_list, list_op);
	    இல்லையெனில்
		vim_free(clstr_list);
	    got_clstr = TRUE;
	}

	எனில் (got_clstr)
	{
	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_அடுக்கு_free_all(curwin->w_s);	// Need to recompute all.
	}
    }

    எனில் (!got_clstr)
	emsg(_(e_no_cluster_specified));
    எனில் (rest == வெற்று || !ends_excmd2(eap->cmd, rest))
	semsg(_(e_invalid_argument_str), arg);
}

/*
 * On first call for current buffer: Init growing array.
 */
    நிலையான வெறுமை
init_syn_patterns(வெறுமை)
{
    curwin->w_s->b_syn_patterns.ga_itemsize = அளவு(synpat_T);
    curwin->w_s->b_syn_patterns.ga_growsize = 10;
}

/*
 * Get one pattern for a ":syntax match" or ":syntax region" command.
 * Stores the pattern and program in a synpat_T.
 * Returns a pointer to the next argument, or NULL in case of an error.
 */
    நிலையான char_u *
get_syn_pattern(char_u *arg, synpat_T *ci)
{
    char_u	*end;
    முழு		*p;
    முழு		idx;
    char_u	*cpo_save;

    // need at least three chars
    எனில் (arg == வெற்று || arg[0] == NUL || arg[1] == NUL || arg[2] == NUL)
	திரும்பு வெற்று;

    end = skip_regexp(arg + 1, *arg, TRUE);
    எனில் (*end != *arg)			    // end delimiter not found
    {
	semsg(_(e_pattern_delimiter_not_found_str), arg);
	திரும்பு வெற்று;
    }
    // store the pattern and compiled regexp program
    எனில் ((ci->sp_pattern = vim_strnsave(arg + 1, end - arg - 1)) == வெற்று)
	திரும்பு வெற்று;

    // Make 'cpoptions' empty, to avoid the 'l' flag
    cpo_save = p_cpo;
    p_cpo = empty_option;
    ci->sp_prog = vim_regcomp(ci->sp_pattern, RE_MAGIC);
    p_cpo = cpo_save;

    எனில் (ci->sp_prog == வெற்று)
	திரும்பு வெற்று;
    ci->sp_ic = curwin->w_s->b_syn_ic;
#ifdef FEAT_PROFILE
    syn_clear_time(&ci->sp_time);
#endif

    /*
     * Check for a match, highlight or region offset.
     */
    ++end;
    செய்
    {
	க்கு (idx = SPO_மொத்தம்; --idx >= 0; )
	    எனில் (STRNCMP(end, spo_name_tab[idx], 3) == 0)
		நிறுத்து;
	எனில் (idx >= 0)
	{
	    p = &(ci->sp_offsets[idx]);
	    எனில் (idx != SPO_LC_OFF)
		தேர்வு (end[3])
		{
		    நிலை 's':   நிறுத்து;
		    நிலை 'b':   நிறுத்து;
		    நிலை 'e':   idx += SPO_மொத்தம்; நிறுத்து;
		    இயல்பு:    idx = -1; நிறுத்து;
		}
	    எனில் (idx >= 0)
	    {
		ci->sp_off_flags |= (1 << idx);
		எனில் (idx == SPO_LC_OFF)	    // lc=99
		{
		    end += 3;
		    *p = getdigits(&end);

		    // "lc=" offset automatically sets "ms=" offset
		    எனில் (!(ci->sp_off_flags & (1 << SPO_MS_OFF)))
		    {
			ci->sp_off_flags |= (1 << SPO_MS_OFF);
			ci->sp_offsets[SPO_MS_OFF] = *p;
		    }
		}
		இல்லையெனில்			    // yy=x+99
		{
		    end += 4;
		    எனில் (*end == '+')
		    {
			++end;
			*p = getdigits(&end);		// positive offset
		    }
		    இல்லையெனில் எனில் (*end == '-')
		    {
			++end;
			*p = -getdigits(&end);		// negative offset
		    }
		}
		எனில் (*end != ',')
		    நிறுத்து;
		++end;
	    }
	}
    } போது (idx >= 0);

    எனில் (!ends_excmd2(arg, end) && !VIM_ISWHITE(*end))
    {
	semsg(_(e_garbage_after_pattern_str), arg);
	திரும்பு வெற்று;
    }
    திரும்பு skipwhite(end);
}

/*
 * Handle ":syntax sync .." command.
 */
    நிலையான வெறுமை
syn_cmd_sync(exarg_T *eap, முழு syncing UNUSED)
{
    char_u	*arg_start = eap->arg;
    char_u	*arg_end;
    char_u	*சாவி = வெற்று;
    char_u	*next_arg;
    முழு		illegal = FALSE;
    முழு		finished = FALSE;
    long	n;
    char_u	*cpo_save;

    எனில் (ends_excmd2(eap->cmd, arg_start))
    {
	syn_cmd_list(eap, TRUE);
	திரும்பு;
    }

    போது (!ends_excmd2(eap->cmd, arg_start))
    {
	arg_end = skiptowhite(arg_start);
	next_arg = skipwhite(arg_end);
	vim_free(சாவி);
	சாவி = vim_strnsave_up(arg_start, arg_end - arg_start);
	எனில் (சாவி == வெற்று)
	    நிறுத்து;
	எனில் (STRCMP(சாவி, "CCOMMENT") == 0)
	{
	    எனில் (!eap->skip)
		curwin->w_s->b_syn_sync_flags |= SF_CCOMMENT;
	    எனில் (!ends_excmd2(eap->cmd, next_arg))
	    {
		arg_end = skiptowhite(next_arg);
		எனில் (!eap->skip)
		    curwin->w_s->b_syn_sync_id = syn_check_group(next_arg,
						   (முழு)(arg_end - next_arg));
		next_arg = skipwhite(arg_end);
	    }
	    இல்லையெனில் எனில் (!eap->skip)
		curwin->w_s->b_syn_sync_id = syn_name2id((char_u *)"Comment");
	}
	இல்லையெனில் எனில் (  STRNCMP(சாவி, "LINES", 5) == 0
		|| STRNCMP(சாவி, "MINLINES", 8) == 0
		|| STRNCMP(சாவி, "MAXLINES", 8) == 0
		|| STRNCMP(சாவி, "LINEBREAKS", 10) == 0)
	{
	    எனில் (சாவி[4] == 'S')
		arg_end = சாவி + 6;
	    இல்லையெனில் எனில் (சாவி[0] == 'L')
		arg_end = சாவி + 11;
	    இல்லையெனில்
		arg_end = சாவி + 9;
	    எனில் (arg_end[-1] != '=' || !VIM_ISDIGIT(*arg_end))
	    {
		illegal = TRUE;
		நிறுத்து;
	    }
	    n = getdigits(&arg_end);
	    எனில் (!eap->skip)
	    {
		எனில் (சாவி[4] == 'B')
		    curwin->w_s->b_syn_sync_linebreaks = n;
		இல்லையெனில் எனில் (சாவி[1] == 'A')
		    curwin->w_s->b_syn_sync_maxlines = n;
		இல்லையெனில்
		    curwin->w_s->b_syn_sync_minlines = n;
	    }
	}
	இல்லையெனில் எனில் (STRCMP(சாவி, "FROMSTART") == 0)
	{
	    எனில் (!eap->skip)
	    {
		curwin->w_s->b_syn_sync_minlines = MAXLNUM;
		curwin->w_s->b_syn_sync_maxlines = 0;
	    }
	}
	இல்லையெனில் எனில் (STRCMP(சாவி, "LINECONT") == 0)
	{
	    எனில் (*next_arg == NUL)	   // missing pattern
	    {
		illegal = TRUE;
		நிறுத்து;
	    }
	    எனில் (curwin->w_s->b_syn_linecont_pat != வெற்று)
	    {
		emsg(_(e_syntax_sync_வரி_continuations_pattern_specified_twice));
		finished = TRUE;
		நிறுத்து;
	    }
	    arg_end = skip_regexp(next_arg + 1, *next_arg, TRUE);
	    எனில் (*arg_end != *next_arg)	    // end delimiter not found
	    {
		illegal = TRUE;
		நிறுத்து;
	    }

	    எனில் (!eap->skip)
	    {
		// store the pattern and compiled regexp program
		எனில் ((curwin->w_s->b_syn_linecont_pat =
			    vim_strnsave(next_arg + 1,
				      arg_end - next_arg - 1)) == வெற்று)
		{
		    finished = TRUE;
		    நிறுத்து;
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

		எனில் (curwin->w_s->b_syn_linecont_prog == வெற்று)
		{
		    VIM_CLEAR(curwin->w_s->b_syn_linecont_pat);
		    finished = TRUE;
		    நிறுத்து;
		}
	    }
	    next_arg = skipwhite(arg_end + 1);
	}
	இல்லையெனில்
	{
	    eap->arg = next_arg;
	    எனில் (STRCMP(சாவி, "MATCH") == 0)
		syn_cmd_match(eap, TRUE);
	    இல்லையெனில் எனில் (STRCMP(சாவி, "REGION") == 0)
		syn_cmd_region(eap, TRUE);
	    இல்லையெனில் எனில் (STRCMP(சாவி, "CLEAR") == 0)
		syn_cmd_clear(eap, TRUE);
	    இல்லையெனில்
		illegal = TRUE;
	    finished = TRUE;
	    நிறுத்து;
	}
	arg_start = next_arg;
    }
    vim_free(சாவி);
    எனில் (illegal)
	semsg(_(e_illegal_arguments_str), arg_start);
    இல்லையெனில் எனில் (!finished)
    {
	set_nextcmd(eap, arg_start);
	redraw_curbuf_later(UPD_SOME_VALID);
	syn_அடுக்கு_free_all(curwin->w_s);	// Need to recompute all syntax.
    }
}

/*
 * Convert a line of highlight group names into a list of group ID numbers.
 * "arg" should point to the "contains" or "nextgroup" keyword.
 * "arg" is advanced to after the last group name.
 * Careful: the argument is modified (NULs added).
 * returns FAIL for some error, OK for success.
 */
    நிலையான முழு
get_id_list(
    char_u	**arg,
    முழு		keylen,		// length of keyword
    short	**list,		// where to store the resulting list, if not
				// NULL, the list is silently skipped!
    முழு		skip)
{
    char_u	*p = வெற்று;
    char_u	*end;
    முழு		round;
    முழு		மொத்தம்;
    முழு		total_மொத்தம் = 0;
    short	*retval = வெற்று;
    char_u	*name;
    regmatch_T	regmatch;
    முழு		id;
    முழு		i;
    முழு		failed = FALSE;

    /*
     * We parse the list twice:
     * round == 1: count the number of items, allocate the array.
     * round == 2: fill the array with the items.
     * In round 1 new groups may be added, causing the number of items to
     * grow when a regexp is used.  In that case round 1 is done once again.
     */
    க்கு (round = 1; round <= 2; ++round)
    {
	/*
	 * skip "contains"
	 */
	p = skipwhite(*arg + keylen);
	எனில் (*p != '=')
	{
	    semsg(_(e_missing_equal_sign_str), *arg);
	    நிறுத்து;
	}
	p = skipwhite(p + 1);
	எனில் (ends_excmd2(*arg, p))
	{
	    semsg(_(e_empty_argument_str), *arg);
	    நிறுத்து;
	}

	/*
	 * parse the arguments after "contains"
	 */
	மொத்தம் = 0;
	போது (!ends_excmd2(*arg, p))
	{
	    க்கு (end = p; *end && !VIM_ISWHITE(*end) && *end != ','; ++end)
		;
	    name = alloc(end - p + 3);	    // leave room for "^$"
	    எனில் (name == வெற்று)
	    {
		failed = TRUE;
		நிறுத்து;
	    }
	    vim_strncpy(name + 1, p, end - p);
	    எனில் (       STRCMP(name + 1, "ALLBUT") == 0
		    || STRCMP(name + 1, "ALL") == 0
		    || STRCMP(name + 1, "TOP") == 0
		    || STRCMP(name + 1, "CONTAINED") == 0)
	    {
		எனில் (TOUPPER_ASC(**arg) != 'C')
		{
		    semsg(_(e_str_not_allowed_here), name + 1);
		    failed = TRUE;
		    vim_free(name);
		    நிறுத்து;
		}
		எனில் (மொத்தம் != 0)
		{
		    semsg(_(e_str_must_be_first_in_contains_list), name + 1);
		    failed = TRUE;
		    vim_free(name);
		    நிறுத்து;
		}
		எனில் (name[1] == 'A')
		    id = SYNID_ALLBUT;
		இல்லையெனில் எனில் (name[1] == 'T')
		    id = SYNID_உச்சி;
		இல்லையெனில்
		    id = SYNID_CONTAINED;
		id += current_syn_inc_tag;
	    }
	    இல்லையெனில் எனில் (name[1] == '@')
	    {
		எனில் (skip)
		    id = -1;
		இல்லையெனில்
		    id = syn_check_cluster(name + 2, (முழு)(end - p - 1));
	    }
	    இல்லையெனில்
	    {
		/*
		 * Handle full group name.
		 */
		எனில் (vim_strpbrk(name + 1, (char_u *)"\\.*^$~[") == வெற்று)
		    id = syn_check_group(name + 1, (முழு)(end - p));
		இல்லையெனில்
		{
		    /*
		     * Handle match of regexp with group names.
		     */
		    *name = '^';
		    STRCAT(name, "$");
		    regmatch.regprog = vim_regcomp(name, RE_MAGIC);
		    எனில் (regmatch.regprog == வெற்று)
		    {
			failed = TRUE;
			vim_free(name);
			நிறுத்து;
		    }

		    regmatch.rm_ic = TRUE;
		    id = 0;
		    க்கு (i = highlight_இலக்கம்_groups(); --i >= 0; )
		    {
			எனில் (vim_regexec(&regmatch, highlight_group_name(i),
								  (colnr_T)0))
			{
			    எனில் (round == 2)
			    {
				// Got more items than expected; can happen
				// when adding items that match:
				// "contains=a.*b,axb".
				// Go back to first round
				எனில் (மொத்தம் >= total_மொத்தம்)
				{
				    vim_free(retval);
				    round = 1;
				}
				இல்லையெனில்
				    retval[மொத்தம்] = i + 1;
			    }
			    ++மொத்தம்;
			    id = -1;	    // remember that we found one
			}
		    }
		    vim_regfree(regmatch.regprog);
		}
	    }
	    vim_free(name);
	    எனில் (id == 0)
	    {
		semsg(_(e_unknown_group_name_str), p);
		failed = TRUE;
		நிறுத்து;
	    }
	    எனில் (id > 0)
	    {
		எனில் (round == 2)
		{
		    // Got more items than expected, go back to first round
		    எனில் (மொத்தம் >= total_மொத்தம்)
		    {
			vim_free(retval);
			round = 1;
		    }
		    இல்லையெனில்
			retval[மொத்தம்] = id;
		}
		++மொத்தம்;
	    }
	    p = skipwhite(end);
	    எனில் (*p != ',')
		நிறுத்து;
	    p = skipwhite(p + 1);	// skip comma in between arguments
	}
	எனில் (failed)
	    நிறுத்து;
	எனில் (round == 1)
	{
	    retval = ALLOC_MULT(short, மொத்தம் + 1);
	    எனில் (retval == வெற்று)
		நிறுத்து;
	    retval[மொத்தம்] = 0;	    // zero means end of the list
	    total_மொத்தம் = மொத்தம்;
	}
    }

    *arg = p;
    எனில் (failed || retval == வெற்று)
    {
	vim_free(retval);
	திரும்பு FAIL;
    }

    எனில் (*list == வெற்று)
	*list = retval;
    இல்லையெனில்
	vim_free(retval);	// list already found, don't overwrite it

    திரும்பு OK;
}

/*
 * Make a copy of an ID list.
 */
    நிலையான short *
copy_id_list(short *list)
{
    முழு	    len;
    முழு	    மொத்தம்;
    short   *retval;

    எனில் (list == வெற்று)
	திரும்பு வெற்று;

    க்கு (மொத்தம் = 0; list[மொத்தம்]; ++மொத்தம்)
	;
    len = (மொத்தம் + 1) * அளவு(short);
    retval = alloc(len);
    எனில் (retval != வெற்று)
	mch_memmove(retval, list, (size_t)len);

    திரும்பு retval;
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

வகைவரை கட்டமைப்பு
{
    short	*idl_சாவி;	// the list this entry resolves (NULL when empty)
    short	*idl_ids;	// sorted, cluster-expanded group IDs (allocated)
    முழு		idl_மொத்தம்;	// number of IDs in idl_ids
    short	idl_marker;	// leading ALLBUT/TOP/CONTAINED item, or 0
    bool	idl_usable;	// false: not cacheable, use the slow path
} idl_entry_T;

வகைவரை கட்டமைப்பு
{
    idl_entry_T	ent[IDL_CACHE_SIZE];
    முழு		round;		// round-robin replacement index
    முழு		last;		// index of the last returned entry (fast path)
} idl_cache_T;

/*
 * Collect the explicit group IDs of "list" into "ga", expanding clusters.
 * "seen" records visited clusters to break cycles and avoid duplicate work.
 * Return false when the list cannot be represented as a plain set (a nested
 * ALLBUT/TOP/CONTAINED marker) or on out of memory.
 */
    நிலையான bool
idl_collect(short *list, garray_T *ga, garray_T *seen)
{
    எனில் (list == வெற்று || list == ID_LIST_ALL)
	திரும்பு false;
    க்கு (short item = *list; item != 0; item = *++list)
    {
	எனில் (item >= SYNID_ALLBUT && item < SYNID_CLUSTER)
	    திரும்பு false;	// nested ALLBUT/TOP/CONTAINED: not cacheable
	எனில் (item >= SYNID_CLUSTER)
	{
	    short	*scl_list;
	    bool	seen_it = false;

	    க்கு (முழு i = 0; i < seen->ga_len; ++i)
		எனில் (((short *)seen->ga_data)[i] == item)
		{
		    seen_it = true;
		    நிறுத்து;
		}
	    எனில் (seen_it)
		தொடர்;
	    எனில் (ga_grow(seen, 1) == FAIL)
		திரும்பு false;
	    ((short *)seen->ga_data)[seen->ga_len++] = item;
	    // Record the cluster id itself, not just its members: the slow
	    // path matches "item == id" for a cluster id present in the list.
	    எனில் (ga_grow(ga, 1) == FAIL)
		திரும்பு false;
	    ((short *)ga->ga_data)[ga->ga_len++] = item;
	    scl_list = SYN_CLSTR(syn_block)[item - SYNID_CLUSTER].scl_list;
	    எனில் (scl_list != வெற்று && !idl_collect(scl_list, ga, seen))
		திரும்பு false;
	}
	இல்லையெனில்
	{
	    எனில் (ga_grow(ga, 1) == FAIL)
		திரும்பு false;
	    ((short *)ga->ga_data)[ga->ga_len++] = item;
	}
    }
    திரும்பு true;
}

    நிலையான முழு
idl_id_cmp(மாறிலி வெறுமை *a, மாறிலி வெறுமை *b)
{
    திரும்பு (முழு)*(மாறிலி short *)a - (முழு)*(மாறிலி short *)b;
}

/*
 * Get the cache entry that resolves "list", building it if needed.
 * Returns NULL only on allocation failure.
 */
    நிலையான idl_entry_T *
idl_get_entry(short *list)
{
    idl_cache_T	*cache = (idl_cache_T *)syn_block->b_idlist_cache;
    idl_entry_T	*e;
    garray_T	ga, seen;

    எனில் (cache == வெற்று)
    {
	cache = ALLOC_CLEAR_ONE(idl_cache_T);
	எனில் (cache == வெற்று)
	    திரும்பு வெற்று;
	syn_block->b_idlist_cache = cache;
    }
    // Fast path: the same list is usually queried many times in a row.
    எனில் (cache->ent[cache->last].idl_சாவி == list)
	திரும்பு &cache->ent[cache->last];
    க்கு (முழு i = 0; i < IDL_CACHE_SIZE; ++i)
	எனில் (cache->ent[i].idl_சாவி == list)
	{
	    cache->last = i;
	    திரும்பு &cache->ent[i];
	}

    // Build a new entry, replacing one in round-robin order.
    cache->last = cache->round;
    e = &cache->ent[cache->round];
    cache->round = (cache->round + 1) % IDL_CACHE_SIZE;
    VIM_CLEAR(e->idl_ids);
    e->idl_மொத்தம் = 0;
    e->idl_சாவி = list;
    e->idl_marker = 0;
    e->idl_usable = true;

    // A leading ALLBUT/TOP/CONTAINED marker is kept aside; the rest is the set.
    எனில் (*list >= SYNID_ALLBUT && *list < SYNID_CLUSTER)
    {
	e->idl_marker = *list;
	++list;
    }

    ga_init2(&ga, அளவு(short), 50);
    ga_init2(&seen, அளவு(short), 10);
    எனில் (!idl_collect(list, &ga, &seen))
    {
	e->idl_usable = false;
	ga_clear(&ga);
	ga_clear(&seen);
	திரும்பு e;
    }
    ga_clear(&seen);
    எனில் (ga.ga_len > 0)
    {
	qsort(ga.ga_data, (size_t)ga.ga_len, அளவு(short), idl_id_cmp);
	e->idl_ids = (short *)ga.ga_data;	// take ownership
	e->idl_மொத்தம் = ga.ga_len;
    }
    இல்லையெனில்
	ga_clear(&ga);
    திரும்பு e;
}

/*
 * Return true if "id" is in the resolved set of cache entry "e".
 */
    நிலையான bool
idl_contains(idl_entry_T *e, short id)
{
    முழு	lo = 0;
    முழு	hi = e->idl_மொத்தம் - 1;

    போது (lo <= hi)
    {
	முழு	mid = (lo + hi) / 2;
	short	v = e->idl_ids[mid];

	எனில் (v == id)
	    திரும்பு true;
	எனில் (v < id)
	    lo = mid + 1;
	இல்லையெனில்
	    hi = mid - 1;
    }
    திரும்பு false;
}

    நிலையான வெறுமை
idl_cache_clear(synblock_T *block)
{
    idl_cache_T	*cache = (idl_cache_T *)block->b_idlist_cache;

    எனில் (cache == வெற்று)
	திரும்பு;
    க்கு (முழு i = 0; i < IDL_CACHE_SIZE; ++i)
	vim_free(cache->ent[i].idl_ids);
    vim_free(cache);
    block->b_idlist_cache = வெற்று;
}
// }}}

/*
 * Check if syntax group "ssp" is in the ID list "list" of "cur_si".
 * "cur_si" can be NULL if not checking the "containedin" list.
 * Used to check if a syntax item is in the "contains" or "nextgroup" list of
 * the current item.
 * This function is called very often, keep it fast!!
 */
    நிலையான முழு
in_id_list(
    stateitem_T	*cur_si,	// current item or NULL
    short	*list,		// id list
    கட்டமைப்பு sp_syn *ssp,		// group id and ":syn include" tag of group
    முழு		flags)		// group flags
{
    முழு		retval;
    short	*scl_list;
    short	item;
    short	id = ssp->id;
    நிலையான முழு	depth = 0;
    முழு		r;
    முழு		toplevel;
    idl_entry_T	*e;

    // If ssp has a "containedin" list and "cur_si" is in it, return TRUE.
    எனில் (cur_si != வெற்று && ssp->cont_in_list != வெற்று
					    && !(cur_si->si_flags & HL_MATCH))
    {
	// Ignore transparent items without a contains argument.  Double check
	// that we don't go back past the first one.
	போது ((cur_si->si_flags & HL_TRANS_CONT)
		&& cur_si > (stateitem_T *)(current_state.ga_data))
	    --cur_si;
	// cur_si->si_idx is -1 for keywords, these never contain anything.
	எனில் (cur_si->si_idx >= 0 && in_id_list(வெற்று, ssp->cont_in_list,
		&(SYN_பொருட்கள்(syn_block)[cur_si->si_idx].sp_syn),
		  SYN_பொருட்கள்(syn_block)[cur_si->si_idx].sp_flags))
	    திரும்பு TRUE;
    }

    எனில் (list == வெற்று)
	திரும்பு FALSE;

    /*
     * If list is ID_LIST_ALL, we are in a transparent item that isn't
     * inside anything.  Only allow not-contained groups.
     */
    எனில் (list == ID_LIST_ALL)
	திரும்பு !(flags & HL_CONTAINED);

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
    e = disable_syn_idlist_cache_for_testing ? வெற்று : idl_get_entry(list);
    எனில் (e != வெற்று && e->idl_usable)
    {
	எனில் (e->idl_marker != 0)
	{
	    item = e->idl_marker;
	    எனில் (item < SYNID_உச்சி)
	    {
		எனில் (item - SYNID_ALLBUT != ssp->inc_tag)
		    திரும்பு FALSE;
	    }
	    இல்லையெனில் எனில் (item < SYNID_CONTAINED)
	    {
		எனில் (item - SYNID_உச்சி != ssp->inc_tag || !toplevel)
		    திரும்பு FALSE;
	    }
	    இல்லையெனில்
	    {
		எனில் (item - SYNID_CONTAINED != ssp->inc_tag || toplevel)
		    திரும்பு FALSE;
	    }
	    retval = FALSE;
	}
	இல்லையெனில்
	    retval = TRUE;
	திரும்பு idl_contains(e, id) ? retval : !retval;
    }

    /*
     * Slow path (uncacheable list, e.g. a nested ALLBUT).
     * If the first item is "ALLBUT", return TRUE if "id" is NOT in the
     * contains list.  We also require that "id" is at the same ":syn include"
     * level as the list.
     */
    item = *list;
    எனில் (item >= SYNID_ALLBUT && item < SYNID_CLUSTER)
    {
	எனில் (item < SYNID_உச்சி)
	{
	    // ALL or ALLBUT: accept all groups in the same file
	    எனில் (item - SYNID_ALLBUT != ssp->inc_tag)
		திரும்பு FALSE;
	}
	இல்லையெனில் எனில் (item < SYNID_CONTAINED)
	{
	    // TOP: accept all not-contained groups in the same file
	    எனில் (item - SYNID_உச்சி != ssp->inc_tag || !toplevel)
		திரும்பு FALSE;
	}
	இல்லையெனில்
	{
	    // CONTAINED: accept all contained groups in the same file
	    எனில் (item - SYNID_CONTAINED != ssp->inc_tag || toplevel)
		திரும்பு FALSE;
	}
	item = *++list;
	retval = FALSE;
    }
    இல்லையெனில்
	retval = TRUE;

    /*
     * Return "retval" if id is in the contains list.
     */
    போது (item != 0)
    {
	எனில் (item == id)
	    திரும்பு retval;
	எனில் (item >= SYNID_CLUSTER)
	{
	    scl_list = SYN_CLSTR(syn_block)[item - SYNID_CLUSTER].scl_list;
	    // restrict recursiveness to 30 to avoid an endless loop for a
	    // cluster that includes itself (indirectly)
	    எனில் (scl_list != வெற்று && depth < 30)
	    {
		++depth;
		r = in_id_list(வெற்று, scl_list, ssp, flags);
		--depth;
		எனில் (r)
		    திரும்பு retval;
	    }
	}
	item = *++list;
    }
    திரும்பு !retval;
}

கட்டமைப்பு subcommand
{
    எழுத்து    *name;			// subcommand name
    வெறுமை    (*func)(exarg_T *, முழு);	// function to call
};

நிலையான கட்டமைப்பு subcommand subcommands[] =
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
    வெறுமை
ex_syntax(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*subcmd_end;
    char_u	*subcmd_name;
    முழு		i;

    syn_cmdlinep = eap->cmdlinep;

    // isolate subcommand name
    க்கு (subcmd_end = arg; ASCII_ISALPHA(*subcmd_end); ++subcmd_end)
	;
    subcmd_name = vim_strnsave(arg, subcmd_end - arg);
    எனில் (subcmd_name == வெற்று)
	திரும்பு;

    எனில் (eap->skip)		// skip error messages for all subcommands
	++emsg_skip;
    க்கு (i = 0; i < (முழு)ARRAY_LENGTH(subcommands); ++i)
    {
	எனில் (STRCMP(subcmd_name, (char_u *)subcommands[i].name) == 0)
	{
	    eap->arg = skipwhite(subcmd_end);
	    (subcommands[i].func)(eap, FALSE);
	    நிறுத்து;
	}
    }

    எனில் (i == (முழு)ARRAY_LENGTH(subcommands))
	semsg(_(e_invalid_syntax_subcommand_str), subcmd_name);

    vim_free(subcmd_name);
    எனில் (eap->skip)
	--emsg_skip;
}

    வெறுமை
ex_ownsyntax(exarg_T *eap)
{
    char_u	*old_மதிப்பு;
    char_u	*new_மதிப்பு_2;

    எனில் (curwin->w_s == &curwin->w_buffer->b_s)
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
    old_மதிப்பு = get_var_மதிப்பு((char_u *)"b:current_syntax");
    எனில் (old_மதிப்பு != வெற்று)
	old_மதிப்பு = vim_strsave(old_மதிப்பு);

    // Apply the "syntax" autocommand event, this finds and loads the syntax
    // file.
    apply_autocmds(EVENT_SYNTAX, eap->arg, curbuf->b_fname, TRUE, curbuf);

    // move value of b:current_syntax to w:current_syntax
    new_மதிப்பு_2 = get_var_மதிப்பு((char_u *)"b:current_syntax");
    எனில் (new_மதிப்பு_2 != வெற்று)
	set_internal_string_var((char_u *)"w:current_syntax", new_மதிப்பு_2);

    // restore value of b:current_syntax
    எனில் (old_மதிப்பு == வெற்று)
	do_unlet((char_u *)"b:current_syntax", TRUE);
    இல்லையெனில்
    {
	set_internal_string_var((char_u *)"b:current_syntax", old_மதிப்பு);
	vim_free(old_மதிப்பு);
    }
}

    முழு
syntax_present(win_T *win)
{
    திரும்பு (win->w_s->b_syn_patterns.ga_len != 0
	    || win->w_s->b_syn_clusters.ga_len != 0
	    || win->w_s->b_keywtab.ht_used > 0
	    || win->w_s->b_keywtab_ic.ht_used > 0);
}


நிலையான எண்ணிக்கை
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
    வெறுமை
reset_expand_highlight(வெறுமை)
{
    include_link = include_default = include_none = 0;
}

/*
 * Handle command line completion for :match and :echohl command: Add "NONE"
 * as highlight group.
 */
    வெறுமை
set_context_in_echohl_cmd(expand_T *xp, char_u *arg)
{
    xp->xp_context = EXPAND_HIGHLIGHT;
    xp->xp_pattern = arg;
    include_none = 1;
}

/*
 * Handle command line completion for :syntax command.
 */
    வெறுமை
set_context_in_syntax_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    // Default: expand subcommands
    xp->xp_context = EXPAND_SYNTAX;
    expand_what = EXP_SUBCMD;
    xp->xp_pattern = arg;
    include_link = 0;
    include_default = 0;

    எனில் (*arg == NUL)
	திரும்பு;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    எனில் (*p == NUL)
	திரும்பு;

    // past first word
    xp->xp_pattern = skipwhite(p);
    எனில் (*skiptowhite(xp->xp_pattern) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    இல்லையெனில் எனில் (STRNICMP(arg, "case", p - arg) == 0)
	expand_what = EXP_CASE;
    இல்லையெனில் எனில் (STRNICMP(arg, "spell", p - arg) == 0)
	expand_what = EXP_SPELL;
    இல்லையெனில் எனில் (STRNICMP(arg, "sync", p - arg) == 0)
	expand_what = EXP_SYNC;
    இல்லையெனில் எனில் (STRNICMP(arg, "list", p - arg) == 0)
    {
	p = skipwhite(p);
	எனில் (*p == '@')
	    expand_what = EXP_CLUSTER;
	இல்லையெனில்
	    xp->xp_context = EXPAND_HIGHLIGHT;
    }
    இல்லையெனில் எனில் (STRNICMP(arg, "keyword", p - arg) == 0
	    || STRNICMP(arg, "region", p - arg) == 0
	    || STRNICMP(arg, "match", p - arg) == 0)
	xp->xp_context = EXPAND_HIGHLIGHT;
    இல்லையெனில்
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Function given to ExpandGeneric() to obtain the list syntax names for
 * expansion.
 */
    char_u *
get_syntax_name(expand_T *xp, முழு idx)
{
    தேர்வு (expand_what)
    {
	நிலை EXP_SUBCMD:
	    எனில் (idx < 0 || idx >= (முழு)ARRAY_LENGTH(subcommands))
		திரும்பு வெற்று;
	    திரும்பு (char_u *)subcommands[idx].name;
	நிலை EXP_CASE:
	{
	    நிலையான எழுத்து *case_args[] = {"match", "ignore", வெற்று};
	    திரும்பு (char_u *)case_args[idx];
	}
	நிலை EXP_SPELL:
	{
	    நிலையான எழுத்து *spell_args[] =
		{"toplevel", "notoplevel", "default", வெற்று};
	    திரும்பு (char_u *)spell_args[idx];
	}
	நிலை EXP_SYNC:
	{
	    நிலையான எழுத்து *sync_args[] =
		{"ccomment", "clear", "fromstart",
		 "linebreaks=", "linecont", "lines=", "match",
		 "maxlines=", "minlines=", "region", வெற்று};
	    திரும்பு (char_u *)sync_args[idx];
	}
	நிலை EXP_CLUSTER:
	{
	    எனில் (idx < curwin->w_s->b_syn_clusters.ga_len)
	    {
		vim_snprintf((எழுத்து *)xp->xp_buf, EXPAND_BUF_LEN, "@%s",
					 SYN_CLSTR(curwin->w_s)[idx].scl_name);
		திரும்பு xp->xp_buf;
	    }
	    இல்லையெனில்
		திரும்பு வெற்று;
	}
    }
    திரும்பு வெற்று;
}


/*
 * Function called for expression evaluation: get syntax ID at file position.
 */
    முழு
syn_get_id(
    win_T	*wp,
    long	lnum,
    colnr_T	col,
    முழு		trans,	     // remove transparency
    முழு		*spellp,     // return: can do spell checking
    முழு		keep_state)  // keep state of char at "col"
{
    // When the position is not after the current position and in the same
    // line of the same window with the same buffer, need to restart parsing.
    எனில் (wp != syn_win
	    || wp->w_buffer != syn_buf
	    || lnum != current_lnum
	    || col < current_col)
	syntax_start(wp, lnum);
    இல்லையெனில் எனில் (wp->w_buffer == syn_buf
	    && lnum == current_lnum
	    && col > current_col)
	// next_match may not be correct when moving around, e.g. with the
	// "skip" expression in searchpair()
	next_match_idx = -1;

    (வெறுமை)get_syntax_attr(col, spellp, keep_state);

    திரும்பு (trans ? current_trans_id : current_id);
}

#எனில் defined(FEAT_CONCEAL)
/*
 * Get extra information about the syntax item.  Must be called right after
 * get_syntax_attr().
 * Stores the current item sequence nr in "*seqnrp".
 * Returns the current flags.
 */
    முழு
get_syntax_info(முழு *seqnrp)
{
    *seqnrp = current_seqnr;
    திரும்பு current_flags;
}

/*
 * Return conceal substitution character
 */
    முழு
syn_get_sub_char(வெறுமை)
{
    திரும்பு current_sub_char;
}
#endif

#எனில் defined(FEAT_EVAL)
/*
 * Return the syntax ID at position "i" in the current stack.
 * The caller must have called syn_get_id() before to fill the stack.
 * Returns -1 when "i" is out of range.
 */
    முழு
syn_get_அடுக்கு_item(முழு i)
{
    எனில் (i >= current_state.ga_len)
    {
	// Need to invalidate the state, because we didn't properly finish it
	// for the last character, "keep_state" was TRUE.
	invalidate_current_state();
	current_col = MAXCOL;
	திரும்பு -1;
    }
    திரும்பு CUR_STATE(i).si_id;
}
#endif

#எனில் defined(FEAT_FOLDING)
    நிலையான முழு
syn_cur_foldlevel(வெறுமை)
{
    முழு		level = 0;
    முழு		i;

    க்கு (i = 0; i < current_state.ga_len; ++i)
	எனில் (CUR_STATE(i).si_flags & HL_FOLD)
	    ++level;
    திரும்பு level;
}

/*
 * Function called to get folding level for line "lnum" in window "wp".
 */
    முழு
syn_get_foldlevel(win_T *wp, long lnum)
{
    முழு		level = 0;
    முழு		கீழ்_level;
    முழு		cur_level;

    // Return quickly when there are no fold items at all.
    எனில் (wp->w_s->b_syn_folditems != 0
	    && !wp->w_s->b_syn_error
# ifdef SYN_TIME_LIMIT
	    && !wp->w_s->b_syn_slow
# endif
	    )
    {
	syntax_start(wp, lnum);

	// Start with the fold level at the start of the line.
	level = syn_cur_foldlevel();

	எனில் (wp->w_s->b_syn_foldlevel == SYNFLD_MINIMUM)
	{
	    // Find the lowest fold level that is followed by a higher one.
	    cur_level = level;
	    கீழ்_level = cur_level;
	    போது (!current_finished)
	    {
		(வெறுமை)syn_current_attr(FALSE, FALSE, வெற்று, FALSE);
		cur_level = syn_cur_foldlevel();
		எனில் (cur_level < கீழ்_level)
		    கீழ்_level = cur_level;
		இல்லையெனில் எனில் (cur_level > கீழ்_level)
		    level = கீழ்_level;
		++current_col;
	    }
	}
    }
    எனில் (level > wp->w_p_fdn)
    {
	level = wp->w_p_fdn;
	எனில் (level < 0)
	    level = 0;
    }
    திரும்பு level;
}
#endif

#எனில் defined(FEAT_PROFILE)
/*
 * ":syntime".
 */
    வெறுமை
ex_syntime(exarg_T *eap)
{
    எனில் (STRCMP(eap->arg, "on") == 0)
	syn_time_on = TRUE;
    இல்லையெனில் எனில் (STRCMP(eap->arg, "off") == 0)
	syn_time_on = FALSE;
    இல்லையெனில் எனில் (STRCMP(eap->arg, "clear") == 0)
	syntime_clear();
    இல்லையெனில் எனில் (STRCMP(eap->arg, "report") == 0)
	syntime_report();
    இல்லையெனில்
	semsg(_(e_invalid_argument_str), eap->arg);
}

    நிலையான வெறுமை
syn_clear_time(syn_time_T *st)
{
    profile_zero(&st->total);
    profile_zero(&st->slowest);
    st->மொத்தம் = 0;
    st->match = 0;
}

/*
 * Clear the syntax timing for the current buffer.
 */
    நிலையான வெறுமை
syntime_clear(வெறுமை)
{
    முழு		idx;
    synpat_T	*spp;

    எனில் (!syntax_present(curwin))
    {
	செய்தி(_(செய்தி_no_பொருட்கள்));
	திரும்பு;
    }
    க்கு (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len; ++idx)
    {
	spp = &(SYN_பொருட்கள்(curwin->w_s)[idx]);
	syn_clear_time(&spp->sp_time);
    }
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":syntime {on,off,clear,report}" command.
 */
    char_u *
get_syntime_arg(expand_T *xp UNUSED, முழு idx)
{
    தேர்வு (idx)
    {
	நிலை 0: திரும்பு (char_u *)"on";
	நிலை 1: திரும்பு (char_u *)"off";
	நிலை 2: திரும்பு (char_u *)"clear";
	நிலை 3: திரும்பு (char_u *)"report";
    }
    திரும்பு வெற்று;
}

வகைவரை கட்டமைப்பு
{
    proftime_T	total;
    முழு		மொத்தம்;
    முழு		match;
    proftime_T	slowest;
    proftime_T	average;
    முழு		id;
    char_u	*pattern;
} time_entry_T;

    நிலையான முழு
syn_compare_syntime(மாறிலி வெறுமை *v1, மாறிலி வெறுமை *v2)
{
    மாறிலி time_entry_T	*s1 = v1;
    மாறிலி time_entry_T	*s2 = v2;

    திரும்பு profile_cmp(&s1->total, &s2->total);
}

/*
 * Clear the syntax timing for the current buffer.
 */
    நிலையான வெறுமை
syntime_report(வெறுமை)
{
    முழு		idx;
    synpat_T	*spp;
# எனில் defined(FEAT_RELTIME)
    proftime_T	tm;
# endif
    முழு		len;
    முழு		patlen;
    proftime_T	total_total;
    முழு		total_மொத்தம் = 0;
    garray_T    ga;
    time_entry_T *p;

    எனில் (!syntax_present(curwin))
    {
	செய்தி(_(செய்தி_no_பொருட்கள்));
	திரும்பு;
    }

    ga_init2(&ga, அளவு(time_entry_T), 50);
    profile_zero(&total_total);
    க்கு (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len; ++idx)
    {
	spp = &(SYN_பொருட்கள்(curwin->w_s)[idx]);
	எனில் (spp->sp_time.மொத்தம் > 0)
	{
	    (வெறுமை)ga_grow(&ga, 1);
	    p = ((time_entry_T *)ga.ga_data) + ga.ga_len;
	    p->total = spp->sp_time.total;
	    profile_add(&total_total, &spp->sp_time.total);
	    p->மொத்தம் = spp->sp_time.மொத்தம்;
	    p->match = spp->sp_time.match;
	    total_மொத்தம் += spp->sp_time.மொத்தம்;
	    p->slowest = spp->sp_time.slowest;
# எனில் defined(FEAT_RELTIME)
	    profile_divide(&spp->sp_time.total, spp->sp_time.மொத்தம், &tm);
	    p->average = tm;
# endif
	    p->id = spp->sp_syn.id;
	    p->pattern = spp->sp_pattern;
	    ++ga.ga_len;
	}
    }

    // Sort on total time. Skip if there are no items to avoid passing NULL
    // pointer to qsort().
    எனில் (ga.ga_len > 1)
	qsort(ga.ga_data, (size_t)ga.ga_len, அளவு(time_entry_T),
							 syn_compare_syntime);

    செய்தி_puts_title(_("  TOTAL      COUNT  MATCH   SLOWEST     AVERAGE   NAME               PATTERN"));
    செய்தி_puts("\n");
    க்கு (idx = 0; idx < ga.ga_len && !got_int; ++idx)
    {
	p = ((time_entry_T *)ga.ga_data) + idx;

	செய்தி_puts(profile_செய்தி(&p->total));
	செய்தி_puts(" "); // make sure there is always a separating space
	செய்தி_advance(13);
	செய்தி_outnum(p->மொத்தம்);
	செய்தி_puts(" ");
	செய்தி_advance(20);
	செய்தி_outnum(p->match);
	செய்தி_puts(" ");
	செய்தி_advance(26);
	செய்தி_puts(profile_செய்தி(&p->slowest));
	செய்தி_puts(" ");
	செய்தி_advance(38);
	செய்தி_puts(profile_செய்தி(&p->average));
	செய்தி_puts(" ");
	செய்தி_advance(50);
	செய்தி_outtrans(highlight_group_name(p->id - 1));
	செய்தி_puts(" ");

	செய்தி_advance(69);
	எனில் (Columns < 80)
	    len = 20; // will wrap anyway
	இல்லையெனில்
	    len = Columns - 70;
	patlen = (முழு)STRLEN(p->pattern);
	எனில் (len > patlen)
	    len = patlen;
	செய்தி_outtrans_len(p->pattern, len);
	செய்தி_puts("\n");
    }
    ga_clear(&ga);
    எனில் (!got_int)
    {
	செய்தி_puts("\n");
	செய்தி_puts(profile_செய்தி(&total_total));
	செய்தி_advance(13);
	செய்தி_outnum(total_மொத்தம்);
	செய்தி_puts("\n");
    }
}
#endif

#endif // FEAT_SYN_HL
