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

#شامل "vim.h"

#اگر defined(FEAT_SYN_HL)

// different types of offsets that are possible
#define SPO_MS_OFF	0	// match  start offset
#define SPO_ME_OFF	1	// match  end	offset
#define SPO_HS_OFF	2	// highl. start offset
#define SPO_HE_OFF	3	// highl. end	offset
#define SPO_RS_OFF	4	// region start offset
#define SPO_RE_OFF	5	// region end	offset
#define SPO_LC_OFF	6	// leading context offset
#define SPO_شمار	7

ایستا نویسه *(spo_name_tab[SPO_شمار]) =
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
تعریفنوع ساختار syn_pattern
{
    نویسه	 sp_type;		// see SPTYPE_ defines below
    نویسه	 sp_syncing;		// this item used for syncing
    short	 sp_syn_match_id;	// highlight group ID of pattern
    short	 sp_off_flags;		// see below
    صحیح		 sp_offsets[SPO_شمار];	// offsets
    صحیح		 sp_flags;		// see HL_ defines below
#ifdef FEAT_CONCEAL
    صحیح		 sp_cchar;		// conceal substitute character
#endif
    صحیح		 sp_ic;			// ignore-case flag for sp_prog
    صحیح		 sp_sync_idx;		// sync item index (syncing only)
    صحیح		 sp_سطر_id;		// ID of last line where tried
    صحیح		 sp_startcol;		// next match in sp_line_id line
    short	*sp_cont_list;		// cont. group IDs, if non-zero
    short	*sp_next_list;		// next group IDs, if non-zero
    ساختار sp_syn sp_syn;		// struct passed to in_id_list()
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


#define SYN_اقلام(buf)	((synpat_T *)((buf)->b_syn_patterns.ga_data))

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
ایستا صحیح current_attr = 0;	    // attr of current syntax word
#ifdef FEAT_EVAL
ایستا صحیح current_id = 0;	    // ID of current char for syn_get_id()
ایستا صحیح current_trans_id = 0;    // idem, transparency removed
#endif
#ifdef FEAT_CONCEAL
ایستا صحیح current_flags = 0;
ایستا صحیح current_seqnr = 0;
ایستا صحیح current_sub_char = 0;
#endif

تعریفنوع ساختار syn_cluster_S
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
#define SYNID_بالاترین	21000	    // syntax group ID for contains=TOP
#define SYNID_CONTAINED	22000	    // syntax group ID for contains=CONTAINED
#define SYNID_CLUSTER	23000	    // first syntax group ID for clusters

#define MAX_SYN_INC_TAG	999	    // maximum before the above overflow
#define MAX_CLUSTER_ID  (32767 - SYNID_CLUSTER)

/*
 * Annoying Hack(TM):  ":syn include" needs this pointer to pass to
 * expand_filename().  Most of the other syntax commands don't need it, so
 * instead of passing it to them, we stow it here.
 */
ایستا char_u **syn_cmdlinep;

/*
 * Another Annoying Hack(TM):  To prevent rules from other ":syn include"'d
 * files from leaking into ALLBUT lists, we assign a unique ID to the
 * rules in each ":syn include"'d file.
 */
ایستا صحیح current_syn_inc_tag = 0;
ایستا صحیح running_syn_inc_tag = 0;

/*
 * In a hashtable item "hi_key" points to "keyword" in a keyentry.
 * This avoids adding a pointer to the hashtable item.
 * KE2HIKEY() converts a var pointer to a hashitem key pointer.
 * HIKEY2KE() converts a hashitem key pointer to a var pointer.
 * HI2KE() converts a hashitem pointer to a var pointer.
 */
ایستا keyentry_T dumkey;
#define KE2HIKEY(kp)  ((kp)->keyword)
#define HIKEY2KE(p)   ((keyentry_T *)((p) - (dumkey.keyword - (char_u *)&dumkey)))
#define HI2KE(hi)      HIKEY2KE((hi)->hi_کلید)

/*
 * To reduce the time spent in keepend(), remember at which level in the state
 * stack the first item with "keepend" is present.  When "-1", there is no
 * "keepend" on the stack.
 */
ایستا صحیح keepend_level = -1;

ایستا نویسه پیام_no_اقلام[] = N_("No Syntax items defined for this buffer");

/*
 * For the current state we need to remember more than just the idx.
 * When si_m_endpos.lnum is 0, the items other than si_idx are unknown.
 * (The end positions have the column number of the next char)
 */
تعریفنوع ساختار state_item
{
    صحیح		si_idx;			// index of syntax pattern or
					// KEYWORD_IDX
    صحیح		si_id;			// highlight group ID for keywords
    صحیح		si_trans_id;		// idem, transparency removed
    صحیح		si_m_lnum;		// lnum of the match
    صحیح		si_m_startcol;		// starting column of the match
    lpos_T	si_m_endpos;		// just after end posn of the match
    lpos_T	si_h_startpos;		// start position of the highlighting
    lpos_T	si_h_endpos;		// end position of the highlighting
    lpos_T	si_eoe_pos;		// end position of end pattern
    صحیح		si_end_idx;		// group ID for end pattern or zero
    صحیح		si_ends;		// if match ends before si_m_endpos
    صحیح		si_attr;		// attributes in this state
    long	si_flags;		// HL_HAS_EOL flag in this state, and
					// HL_SKIP* for si_next_list
#ifdef FEAT_CONCEAL
    صحیح		si_seqnr;		// sequence number
    صحیح		si_cchar;		// substitution character for conceal
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
ایستا صحیح next_seqnr = 1;		// value to use for si_seqnr
#endif

/*
 * Struct to reduce the number of arguments to get_syn_options(), it's used
 * very often.
 */
تعریفنوع ساختار
{
    صحیح		flags;		// flags for contained and transparent
    صحیح		keyword;	// TRUE for ":syn keyword"
    صحیح		*sync_idx;	// syntax item for "grouphere" argument, NULL
				// if not allowed
    نویسه	has_cont_list;	// TRUE if "cont_list" can be used
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
ایستا صحیح next_match_col;		// column for start of next match
ایستا lpos_T next_match_m_endpos;	// position for end of next match
ایستا lpos_T next_match_h_startpos;	// pos. for highl. start of next match
ایستا lpos_T next_match_h_endpos;	// pos. for highl. end of next match
ایستا صحیح next_match_idx;		// index of matched item
ایستا long next_match_flags;		// flags for next match
ایستا lpos_T next_match_eos_pos;	// end of start pattn (start region)
ایستا lpos_T next_match_eoe_pos;	// pos. for end of end pattern
ایستا صحیح next_match_end_idx;		// ID of group for end pattn or zero
ایستا reg_extmatch_T *next_match_extmatch = پوچ;

/*
 * A state stack is an array of integers or stateitem_T, stored in a
 * garray_T.  A state stack is invalid if its itemsize entry is zero.
 */
#define INVALID_STATE(ssp)  ((ssp)->ga_itemsize == 0)
#define VALID_STATE(ssp)    ((ssp)->ga_itemsize != 0)

#define FOR_ALL_SYNSTATES(sb, sst) \
    برای ((sst) = (sb)->b_sst_first; (sst) != پوچ; (sst) = (sst)->sst_next)

/*
 * The current state (within the line) of the recognition engine.
 * When current_state.ga_itemsize is 0 the current state is invalid.
 */
ایستا win_T	*syn_win;		// current window for highlighting
ایستا buf_T	*syn_buf;		// current buffer for highlighting
ایستا synblock_T *syn_block;		// current buffer for highlighting
ایستا linenr_T current_lnum = 0;	// lnum of current state
ایستا colnr_T	current_col = 0;	// column of current state
ایستا صحیح	current_state_stored = 0; // TRUE if stored current state
					  // after setting current_finished
ایستا صحیح	current_finished = 0;	// current line has been finished
ایستا garray_T current_state		// current stack of state_items
		= {0, 0, 0, 0, پوچ};
ایستا short	*current_next_list = پوچ; // when non-zero, nextgroup list
ایستا صحیح	current_next_flags = 0; // flags for current_next_list
ایستا صحیح	current_سطر_id = 0;	// unique number for current line

#define CUR_STATE(idx)	((stateitem_T *)(current_state.ga_data))[idx]

ایستا تهی syn_sync(win_T *wp, linenr_T lnum, synstate_T *last_valid);
ایستا صحیح syn_match_linecont(linenr_T lnum);
ایستا تهی syn_start_سطر(تهی);
ایستا تهی syn_update_ends(صحیح startofline);
ایستا تهی syn_پشته_alloc(تهی);
ایستا صحیح syn_پشته_cleanup(تهی);
ایستا تهی syn_پشته_free_entry(synblock_T *block, synstate_T *p);
ایستا synstate_T *syn_پشته_find_entry(linenr_T lnum);
ایستا synstate_T *store_current_state(تهی);
ایستا تهی load_current_state(synstate_T *from);
ایستا تهی invalidate_current_state(تهی);
ایستا صحیح syn_پشته_equal(synstate_T *sp);
ایستا تهی validate_current_state(تهی);
ایستا صحیح syn_finish_سطر(صحیح syncing);
ایستا صحیح syn_current_attr(صحیح syncing, صحیح displaying, صحیح *can_spell, صحیح keep_state);
ایستا صحیح did_match_already(صحیح idx, garray_T *gap);
ایستا stateitem_T *افزودن_next_match(stateitem_T *cur_si);
ایستا تهی check_state_ends(تهی);
ایستا تهی update_si_attr(صحیح idx);
ایستا تهی check_keepend(تهی);
ایستا تهی update_si_end(stateitem_T *sip, صحیح startcol, صحیح force);
ایستا short *copy_id_list(short *list);
ایستا صحیح in_id_list(stateitem_T *item, short *cont_list, ساختار sp_syn *ssp, صحیح flags);
ایستا صحیح افزودن_current_state(صحیح idx);
ایستا تهی برداشت_current_state(تهی);
#ifdef FEAT_PROFILE
ایستا تهی syn_clear_time(syn_time_T *tt);
ایستا تهی syntime_clear(تهی);
ایستا تهی syntime_report(تهی);
ایستا صحیح syn_time_on = FALSE;
# define IF_SYN_TIME(p) (p)
#وگرنه
# define IF_SYN_TIME(p) پوچ
تعریفنوع صحیح syn_time_T;
#endif

ایستا تهی syn_پشته_apply_changes_block(synblock_T *block, buf_T *buf);
ایستا تهی find_endpos(صحیح idx, lpos_T *startpos, lpos_T *m_endpos, lpos_T *hl_endpos, long *flagsp, lpos_T *end_endpos, صحیح *end_idx, reg_extmatch_T *start_ext);

ایستا تهی limit_pos(lpos_T *pos, lpos_T *limit);
ایستا تهی limit_pos_zero(lpos_T *pos, lpos_T *limit);
ایستا تهی syn_add_end_off(lpos_T *result, regmmatch_T *regmatch, synpat_T *spp, صحیح idx, صحیح extra);
ایستا تهی syn_add_start_off(lpos_T *result, regmmatch_T *regmatch, synpat_T *spp, صحیح idx, صحیح extra);
ایستا char_u *syn_getcurline(تهی);
ایستا colnr_T syn_getcurline_len(تهی);
ایستا صحیح syn_regexec(regmmatch_T *rmp, linenr_T lnum, colnr_T col, syn_time_T *st);
ایستا صحیح check_keyword_id(char_u *سطر, صحیح startcol, صحیح *endcol, long *flags, short **next_list, stateitem_T *cur_si, صحیح *ccharp);
ایستا تهی syn_remove_pattern(synblock_T *block, صحیح idx);
ایستا تهی syn_clear_pattern(synblock_T *block, صحیح i);
ایستا تهی syn_clear_cluster(synblock_T *block, صحیح i);
ایستا تهی syn_clear_one(صحیح id, صحیح syncing);
ایستا تهی syn_cmd_onoff(exarg_T *eap, نویسه *name);
ایستا تهی syn_سطرها_پیام(تهی);
ایستا تهی syn_match_پیام(تهی);
ایستا تهی syn_list_one(صحیح id, صحیح syncing, صحیح link_only);
ایستا تهی syn_list_cluster(صحیح id);
ایستا تهی put_id_list(char_u *name, short *list, صحیح attr);
ایستا تهی put_pattern(نویسه *s, صحیح c, synpat_T *spp, صحیح attr);
ایستا صحیح syn_list_keywords(صحیح id, hashtab_T *ht, صحیح did_header, صحیح attr);
ایستا تهی syn_clear_keyword(صحیح id, hashtab_T *ht);
ایستا تهی clear_keywtab(hashtab_T *ht);
ایستا صحیح syn_scl_namen2id(char_u *linep, صحیح len);
ایستا صحیح syn_check_cluster(char_u *pp, صحیح len);
ایستا صحیح syn_add_cluster(char_u *name);
ایستا تهی init_syn_patterns(تهی);
ایستا char_u *get_syn_pattern(char_u *arg, synpat_T *ci);
ایستا صحیح get_id_list(char_u **arg, صحیح keylen, short **list, صحیح skip);
ایستا تهی syn_combine_list(short **clstr1, short **clstr2, صحیح list_op);
ایستا تهی idl_cache_clear(synblock_T *block);

/*
 * Start the syntax recognition for a line.  This function is normally called
 * from the screen updating, once for each displayed line.
 * The buffer is remembered in syn_buf, because get_syntax_attr() doesn't get
 * it.	Careful: curbuf and curwin are likely to point to another buffer and
 * window.
 */
    تهی
syntax_start(win_T *wp, linenr_T lnum)
{
    synstate_T	*p;
    synstate_T	*last_valid = پوچ;
    synstate_T	*last_min_valid = پوچ;
    synstate_T	*sp, *prev = پوچ;
    linenr_T	parsed_lnum;
    linenr_T	first_stored;
    صحیح		dist;
    ایستا varnumber_T changedtick = 0;	// remember the last change ID

#ifdef FEAT_CONCEAL
    current_sub_char = NUL;
#endif

    /*
     * After switching buffers, invalidate current_state.
     * Also do this when a change was made, the current state may be invalid
     * then.
     */
    اگر (syn_block != wp->w_s
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
    syn_پشته_alloc();
    اگر (syn_block->b_sst_array == پوچ)
	بازگشت;		// out of memory
    syn_block->b_sst_lasttick = display_tick;

    /*
     * If the state of the end of the previous line is useful, store it.
     */
    اگر (VALID_STATE(&current_state)
	    && current_lnum < lnum
	    && current_lnum < syn_buf->b_ml.ml_سطر_شمار)
    {
	(تهی)syn_finish_سطر(FALSE);
	اگر (!current_state_stored)
	{
	    ++current_lnum;
	    (تهی)store_current_state();
	}

	/*
	 * If the current_lnum is now the same as "lnum", keep the current
	 * state (this happens very often!).  Otherwise invalidate
	 * current_state and figure it out below.
	 */
	اگر (current_lnum != lnum)
	    invalidate_current_state();
    }
    وگرنه
	invalidate_current_state();

    /*
     * Try to synchronize from a saved state in b_sst_array[].
     * Only do this if lnum is not before and not to far beyond a saved state.
     */
    اگر (INVALID_STATE(&current_state) && syn_block->b_sst_array != پوچ)
    {
	// Find last valid saved state before start_lnum.
	FOR_ALL_SYNSTATES(syn_block, p)
	{
	    اگر (p->sst_lnum > lnum)
		بشکن;
	    اگر (p->sst_lnum <= lnum && p->sst_change_lnum == 0)
	    {
		last_valid = p;
		اگر (p->sst_lnum >= lnum - syn_block->b_syn_sync_minlines)
		    last_min_valid = p;
	    }
	}
	اگر (last_min_valid != پوچ)
	    load_current_state(last_min_valid);
    }

    /*
     * If "lnum" is before or far beyond a line with a saved state, need to
     * re-synchronize.
     */
    اگر (INVALID_STATE(&current_state))
    {
	syn_sync(wp, lnum, last_valid);
	اگر (current_lnum == 1)
	    // First line is always valid, no matter "minlines".
	    first_stored = 1;
	وگرنه
	    // Need to parse "minlines" lines before state can be considered
	    // valid to store.
	    first_stored = current_lnum + syn_block->b_syn_sync_minlines;
    }
    وگرنه
	first_stored = current_lnum;

    /*
     * Advance from the sync point or saved state until the current line.
     * Save some entries for syncing with later on.
     */
    اگر (syn_block->b_sst_len <= Rows)
	dist = 999999;
    وگرنه
	dist = syn_buf->b_ml.ml_سطر_شمار / (syn_block->b_sst_len - Rows) + 1;
    وقتی (current_lnum < lnum)
    {
	syn_start_سطر();
	(تهی)syn_finish_سطر(FALSE);
	++current_lnum;

	// If we parsed at least "minlines" lines or started at a valid
	// state, the current state is considered valid.
	اگر (current_lnum >= first_stored)
	{
	    // Check if the saved state entry is for the current line and is
	    // equal to the current state.  If so, then validate all saved
	    // states that depended on a change before the parsed line.
	    اگر (prev == پوچ)
		prev = syn_پشته_find_entry(current_lnum - 1);
	    اگر (prev == پوچ)
		sp = syn_block->b_sst_first;
	    وگرنه
		sp = prev;
	    وقتی (sp != پوچ && sp->sst_lnum < current_lnum)
		sp = sp->sst_next;
	    اگر (sp != پوچ
		    && sp->sst_lnum == current_lnum
		    && syn_پشته_equal(sp))
	    {
		parsed_lnum = current_lnum;
		prev = sp;
		وقتی (sp != پوچ && sp->sst_change_lnum <= parsed_lnum)
		{
		    اگر (sp->sst_lnum <= lnum)
			// valid state before desired line, use this one
			prev = sp;
		    وگرنه اگر (sp->sst_change_lnum == 0)
			// past saved states depending on change, break here.
			بشکن;
		    sp->sst_change_lnum = 0;
		    sp = sp->sst_next;
		}
		load_current_state(prev);
	    }
	    // Store the state at this line when it's the first one, the line
	    // where we start parsing, or some distance from the previously
	    // saved state.  But only when parsed at least 'minlines'.
	    وگرنه اگر (prev == پوچ
			|| current_lnum == lnum
			|| current_lnum >= prev->sst_lnum + dist)
		prev = store_current_state();
	}

	// This can take a long time: break when CTRL-C pressed.  The current
	// state will be wrong then.
	سطر_breakcheck();
	اگر (got_int)
	{
	    current_lnum = lnum;
	    بشکن;
	}
    }

    syn_start_سطر();
}

/*
 * We cannot simply discard growarrays full of state_items or buf_states; we
 * have to manually release their extmatch pointers first.
 */
    ایستا تهی
clear_syn_state(synstate_T *p)
{
    صحیح		i;
    garray_T	*gap;

    اگر (p->sst_stacksize > SST_FIX_STATES)
    {
	gap = &(p->sst_union.sst_ga);
	برای (i = 0; i < gap->ga_len; i++)
	    unref_extmatch(SYN_STATE_P(gap)[i].bs_extmatch);
	ga_clear(gap);
    }
    وگرنه
    {
	برای (i = 0; i < p->sst_stacksize; i++)
	    unref_extmatch(p->sst_union.sst_پشته[i].bs_extmatch);
    }
}

/*
 * Cleanup the current_state stack.
 */
    ایستا تهی
clear_current_state(تهی)
{
    صحیح		i;
    stateitem_T	*sip;

    sip = (stateitem_T *)(current_state.ga_data);
    برای (i = 0; i < current_state.ga_len; i++)
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
    ایستا تهی
syn_sync(
    win_T	*wp,
    linenr_T	start_lnum,
    synstate_T	*last_valid)
{
    buf_T	*curbuf_save;
    win_T	*curwin_save;
    pos_T	cursor_save;
    صحیح		idx;
    linenr_T	lnum;
    linenr_T	end_lnum;
    linenr_T	break_lnum;
    صحیح		had_sync_point;
    stateitem_T	*cur_si;
    synpat_T	*spp;
    صحیح		found_flags = 0;
    صحیح		found_match_idx = 0;
    linenr_T	found_current_lnum = 0;
    صحیح		found_current_col = 0;
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
    اگر (syn_block->b_syn_sync_minlines > start_lnum)
	start_lnum = 1;
    وگرنه
    {
	اگر (syn_block->b_syn_sync_minlines == 1)
	    lnum = 1;
	وگرنه اگر (syn_block->b_syn_sync_minlines < 10)
	    lnum = syn_block->b_syn_sync_minlines * 2;
	وگرنه
	    lnum = syn_block->b_syn_sync_minlines * 3 / 2;
	اگر (syn_block->b_syn_sync_maxlines != 0
				     && lnum > syn_block->b_syn_sync_maxlines)
	    lnum = syn_block->b_syn_sync_maxlines;
	اگر (lnum >= start_lnum)
	    start_lnum = 1;
	وگرنه
	    start_lnum -= lnum;
    }
    current_lnum = start_lnum;

    /*
     * 1. Search backwards for the end of a C-style comment.
     */
    اگر (syn_block->b_syn_sync_flags & SF_CCOMMENT)
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
	برای ( ; start_lnum > 1; --start_lnum)
	{
	    char_u	*l = ml_get(start_lnum - 1);

	    اگر (*l == NUL || *(l + ml_get_len(start_lnum - 1) - 1) != '\\')
		بشکن;
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
	اگر (find_start_comment((صحیح)syn_block->b_syn_sync_maxlines) != پوچ)
	{
	    برای (idx = syn_block->b_syn_patterns.ga_len; --idx >= 0; )
		اگر (SYN_اقلام(syn_block)[idx].sp_syn.id
						   == syn_block->b_syn_sync_id
			&& SYN_اقلام(syn_block)[idx].sp_type == SPTYPE_START)
		{
		    validate_current_state();
		    اگر (افزودن_current_state(idx) == OK)
			update_si_attr(current_state.ga_len - 1);
		    بشکن;
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
    وگرنه اگر (syn_block->b_syn_sync_flags & SF_MATCH)
    {
	اگر (syn_block->b_syn_sync_maxlines != 0
			       && start_lnum > syn_block->b_syn_sync_maxlines)
	    break_lnum = start_lnum - syn_block->b_syn_sync_maxlines;
	وگرنه
	    break_lnum = 0;

	found_m_endpos.lnum = 0;
	found_m_endpos.col = 0;
	end_lnum = start_lnum;
	lnum = start_lnum;
	وقتی (--lnum > break_lnum)
	{
	    // This can take a long time: break when CTRL-C pressed.
	    سطر_breakcheck();
	    اگر (got_int)
	    {
		invalidate_current_state();
		current_lnum = start_lnum;
		بشکن;
	    }

	    // Check if we have run into a valid saved state stack now.
	    اگر (last_valid != پوچ && lnum == last_valid->sst_lnum)
	    {
		load_current_state(last_valid);
		بشکن;
	    }

	    /*
	     * Check if the previous line has the line-continuation pattern.
	     */
	    اگر (lnum > 1 && syn_match_linecont(lnum - 1))
		ادامه;

	    /*
	     * Start with nothing on the state stack
	     */
	    validate_current_state();

	    برای (current_lnum = lnum; current_lnum < end_lnum; ++current_lnum)
	    {
		syn_start_سطر();
		برای (;;)
		{
		    had_sync_point = syn_finish_سطر(TRUE);
		    /*
		     * When a sync point has been found, remember where, and
		     * continue to look for another one, further on in the line.
		     */
		    اگر (had_sync_point && current_state.ga_len)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			اگر (cur_si->si_m_endpos.lnum > start_lnum)
			{
			    // ignore match that goes to after where started
			    current_lnum = end_lnum;
			    بشکن;
			}
			اگر (cur_si->si_idx < 0)
			{
			    // Cannot happen?
			    found_flags = 0;
			    found_match_idx = KEYWORD_IDX;
			}
			وگرنه
			{
			    spp = &(SYN_اقلام(syn_block)[cur_si->si_idx]);
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
			اگر (found_m_endpos.lnum > current_lnum)
			{
			    current_lnum = found_m_endpos.lnum;
			    current_col = found_m_endpos.col;
			    اگر (current_lnum >= end_lnum)
				بشکن;
			}
			وگرنه اگر (found_m_endpos.col > current_col)
			    current_col = found_m_endpos.col;
			وگرنه
			    ++current_col;

			// syn_current_attr() will have skipped the check for
			// an item that ends here, need to do that now.  Be
			// careful not to go past the NUL.
			prev_current_col = current_col;
			اگر (syn_getcurline()[current_col] != NUL)
			    ++current_col;
			check_state_ends();
			current_col = prev_current_col;
		    }
		    وگرنه
			بشکن;
		}
	    }

	    /*
	     * If a sync point was encountered, break here.
	     */
	    اگر (found_flags)
	    {
		/*
		 * Put the item that was specified by the sync point on the
		 * state stack.  If there was no item specified, make the
		 * state stack empty.
		 */
		clear_current_state();
		اگر (found_match_idx >= 0
			&& افزودن_current_state(found_match_idx) == OK)
		    update_si_attr(current_state.ga_len - 1);

		/*
		 * When using "grouphere", continue from the sync point
		 * match, until the end of the line.  Parsing starts at
		 * the next line.
		 * For "groupthere" the parsing starts at start_lnum.
		 */
		اگر (found_flags & HL_SYNC_HERE)
		{
		    current_lnum = found_m_endpos.lnum;
		    current_col = found_m_endpos.col;
		    اگر (current_state.ga_len)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			cur_si->si_h_startpos.lnum = found_current_lnum;
			cur_si->si_h_startpos.col = found_current_col;
			update_si_end(cur_si, (صحیح)current_col, TRUE);
			check_keepend();
		    }
		    (تهی)syn_finish_سطر(FALSE);
		    ++current_lnum;
		}
		وگرنه
		    current_lnum = start_lnum;

		بشکن;
	    }

	    end_lnum = lnum;
	    invalidate_current_state();
	}

	// Ran into start of the file or exceeded maximum number of lines
	اگر (lnum <= break_lnum)
	{
	    invalidate_current_state();
	    current_lnum = break_lnum + 1;
	}
    }

    validate_current_state();
}

    ایستا تهی
save_chartab(char_u *chartab)
{
    اگر (syn_block->b_syn_isk == empty_option)
	بازگشت;

    mch_memmove(chartab, syn_buf->b_chartab, (size_t)32);
    mch_memmove(syn_buf->b_chartab, syn_win->w_s->b_syn_chartab, (size_t)32);
}

    ایستا تهی
restore_chartab(char_u *chartab)
{
    اگر (syn_win->w_s->b_syn_isk != empty_option)
	mch_memmove(syn_buf->b_chartab, chartab, (size_t)32);
}

/*
 * Return TRUE if the line-continuation pattern matches in line "lnum".
 */
    ایستا صحیح
syn_match_linecont(linenr_T lnum)
{
    regmmatch_T regmatch;
    صحیح r;
    char_u	buf_chartab[32];  // chartab array for syn iskyeyword

    اگر (syn_block->b_syn_linecont_prog == پوچ)
	بازگشت FALSE;

    // use syntax iskeyword option
    save_chartab(buf_chartab);
    regmatch.rmm_ic = syn_block->b_syn_linecont_ic;
    regmatch.regprog = syn_block->b_syn_linecont_prog;
    r = syn_regexec(&regmatch, lnum, (colnr_T)0,
	    IF_SYN_TIME(&syn_block->b_syn_linecont_time));
    syn_block->b_syn_linecont_prog = regmatch.regprog;
    restore_chartab(buf_chartab);
    بازگشت r;
}

/*
 * Prepare the current state for the start of a line.
 */
    ایستا تهی
syn_start_سطر(تهی)
{
    current_finished = FALSE;
    current_col = 0;

    /*
     * Need to update the end of a start/skip/end that continues from the
     * previous line and regions that have "keepend".
     */
    اگر (current_state.ga_len > 0)
    {
	syn_update_ends(TRUE);
	check_state_ends();
    }

    next_match_idx = -1;
    ++current_سطر_id;
#ifdef FEAT_CONCEAL
    next_seqnr = 1;
#endif
}

/*
 * Check for items in the stack that need their end updated.
 * When "startofline" is TRUE the last item is always updated.
 * When "startofline" is FALSE the item with "keepend" is forcefully updated.
 */
    ایستا تهی
syn_update_ends(صحیح startofline)
{
    stateitem_T	*cur_si;
    صحیح		i;
    صحیح		seen_keepend;

    اگر (startofline)
    {
	// Check for a match carried over from a previous line with a
	// contained region.  The match ends as soon as the region ends.
	برای (i = 0; i < current_state.ga_len; ++i)
	{
	    cur_si = &CUR_STATE(i);
	    اگر (cur_si->si_idx >= 0
		    && (SYN_اقلام(syn_block)[cur_si->si_idx]).sp_type
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
    اگر (keepend_level >= 0)
	برای ( ; i > keepend_level; --i)
	    اگر (CUR_STATE(i).si_flags & HL_EXTEND)
		بشکن;

    seen_keepend = FALSE;
    برای ( ; i < current_state.ga_len; ++i)
    {
	cur_si = &CUR_STATE(i);
	اگر ((cur_si->si_flags & HL_KEEPEND)
			    || (seen_keepend && !startofline)
			    || (i == current_state.ga_len - 1 && startofline))
	{
	    cur_si->si_h_startpos.col = 0;	// start highl. in col 0
	    cur_si->si_h_startpos.lnum = current_lnum;

	    اگر (!(cur_si->si_flags & HL_MATCHCONT))
		update_si_end(cur_si, (صحیح)current_col, !startofline);

	    اگر (!startofline && (cur_si->si_flags & HL_KEEPEND))
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

    ایستا تهی
syn_پشته_free_block(synblock_T *block)
{
    synstate_T	*p;

    // Syntax definitions may have changed: drop the in_id_list() cache.
    idl_cache_clear(block);

    اگر (block->b_sst_array == پوچ)
	بازگشت;

    FOR_ALL_SYNSTATES(block, p)
	clear_syn_state(p);
    VIM_CLEAR(block->b_sst_array);
    block->b_sst_first = پوچ;
    block->b_sst_جستجو = پوچ;
    block->b_sst_len = 0;
}
/*
 * Free b_sst_array[] for buffer "buf".
 * Used when syntax items changed to force resyncing everywhere.
 */
    تهی
syn_پشته_free_all(synblock_T *block)
{
#ifdef FEAT_FOLDING
    win_T	*wp;
#endif

    syn_پشته_free_block(block);

#ifdef FEAT_FOLDING
    // When using "syntax" fold method, must update all folds.
    FOR_ALL_WINDOWS(wp)
    {
	اگر (wp->w_s == block && foldmethodIsSyntax(wp))
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
    ایستا تهی
syn_پشته_alloc(تهی)
{
    long	len;
    synstate_T	*to, *from;
    synstate_T	*sstp;

    len = syn_buf->b_ml.ml_سطر_شمار / SST_DIST + Rows * 2;
    اگر (len < SST_MIN_ENTRIES)
	len = SST_MIN_ENTRIES;
    وگرنه اگر (len > SST_MAX_ENTRIES)
	len = SST_MAX_ENTRIES;
    اگر (syn_block->b_sst_len > len * 2 || syn_block->b_sst_len < len)
    {
	// Allocate 50% too much, to avoid reallocating too often.
	len = syn_buf->b_ml.ml_سطر_شمار;
	len = (len + len / 2) / SST_DIST + Rows * 2;
	اگر (len < SST_MIN_ENTRIES)
	    len = SST_MIN_ENTRIES;
	وگرنه اگر (len > SST_MAX_ENTRIES)
	    len = SST_MAX_ENTRIES;

	اگر (syn_block->b_sst_array != پوچ)
	{
	    // When shrinking the array, cleanup the existing stack.
	    // Make sure that all valid entries fit in the new array.
	    وقتی (syn_block->b_sst_len - syn_block->b_sst_freecount + 2 > len
		    && syn_پشته_cleanup())
		;
	    اگر (len < syn_block->b_sst_len - syn_block->b_sst_freecount + 2)
		len = syn_block->b_sst_len - syn_block->b_sst_freecount + 2;
	}

	sstp = ALLOC_CLEAR_MULT(synstate_T, len);
	اگر (sstp == پوچ)	// out of memory!
	    بازگشت;

	to = sstp - 1;
	اگر (syn_block->b_sst_array != پوچ)
	{
	    // Move the states from the old array to the new one.
	    برای (from = syn_block->b_sst_first; from != پوچ;
							from = from->sst_next)
	    {
		++to;
		*to = *from;
		to->sst_next = to + 1;
	    }
	}
	اگر (to != sstp - 1)
	{
	    to->sst_next = پوچ;
	    syn_block->b_sst_first = sstp;
	    syn_block->b_sst_freecount = len - (صحیح)(to - sstp) - 1;
	}
	وگرنه
	{
	    syn_block->b_sst_first = پوچ;
	    syn_block->b_sst_freecount = len;
	}

	// Create the list of free entries.
	syn_block->b_sst_firstfree = to + 1;
	وقتی (++to < sstp + len)
	    to->sst_next = to + 1;
	(sstp + len - 1)->sst_next = پوچ;

	vim_free(syn_block->b_sst_array);
	syn_block->b_sst_array = sstp;
	syn_block->b_sst_len = len;
	// The entries were moved to a new array, drop the stale pointer.
	syn_block->b_sst_جستجو = پوچ;
    }
}

/*
 * Check for changes in a buffer to affect stored syntax states.  Uses the
 * b_mod_* fields.
 * Called from update_screen(), before screen is being updated, once for each
 * displayed buffer.
 */
    تهی
syn_پشته_apply_changes(buf_T *buf)
{
    win_T	*wp;

    syn_پشته_apply_changes_block(&buf->b_s, buf);

    FOR_ALL_WINDOWS(wp)
    {
	اگر ((wp->w_buffer == buf) && (wp->w_s != &buf->b_s))
	    syn_پشته_apply_changes_block(wp->w_s, buf);
    }
}

    ایستا تهی
syn_پشته_apply_changes_block(synblock_T *block, buf_T *buf)
{
    synstate_T	*p, *prev, *np;
    linenr_T	n;

    prev = پوچ;
    برای (p = block->b_sst_first; p != پوچ; )
    {
	اگر (p->sst_lnum + block->b_syn_sync_linebreaks > buf->b_mod_بالاترین)
	{
	    n = p->sst_lnum + buf->b_mod_xlines;
	    اگر (n <= buf->b_mod_bot)
	    {
		// this state is inside the changed area, remove it
		np = p->sst_next;
		اگر (prev == پوچ)
		    block->b_sst_first = np;
		وگرنه
		    prev->sst_next = np;
		syn_پشته_free_entry(block, p);
		p = np;
		ادامه;
	    }
	    // This state is below the changed area.  Remember the line
	    // that needs to be parsed before this entry can be made valid
	    // again.
	    اگر (p->sst_change_lnum != 0 && p->sst_change_lnum > buf->b_mod_بالاترین)
	    {
		اگر (p->sst_change_lnum + buf->b_mod_xlines > buf->b_mod_بالاترین)
		    p->sst_change_lnum += buf->b_mod_xlines;
		وگرنه
		    p->sst_change_lnum = buf->b_mod_بالاترین;
	    }
	    اگر (p->sst_change_lnum == 0
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
    ایستا صحیح
syn_پشته_cleanup(تهی)
{
    synstate_T	*p, *prev;
    disptick_T	tick;
    صحیح		above;
    صحیح		dist;
    صحیح		retval = FALSE;

    اگر (syn_block->b_sst_first == پوچ)
	بازگشت retval;

    // Compute normal distance between non-displayed entries.
    اگر (syn_block->b_sst_len <= Rows)
	dist = 999999;
    وگرنه
	dist = syn_buf->b_ml.ml_سطر_شمار / (syn_block->b_sst_len - Rows) + 1;

    /*
     * Go through the list to find the "tick" for the oldest entry that can
     * be removed.  Set "above" when the "tick" for the oldest entry is above
     * "b_sst_lasttick" (the display tick wraps around).
     */
    tick = syn_block->b_sst_lasttick;
    above = FALSE;
    prev = syn_block->b_sst_first;
    برای (p = prev->sst_next; p != پوچ; prev = p, p = p->sst_next)
    {
	اگر (prev->sst_lnum + dist > p->sst_lnum)
	{
	    اگر (p->sst_tick > syn_block->b_sst_lasttick)
	    {
		اگر (!above || p->sst_tick < tick)
		    tick = p->sst_tick;
		above = TRUE;
	    }
	    وگرنه اگر (!above && p->sst_tick < tick)
		tick = p->sst_tick;
	}
    }

    /*
     * Go through the list to make the entries for the oldest tick at an
     * interval of several lines.
     */
    prev = syn_block->b_sst_first;
    برای (p = prev->sst_next; p != پوچ; prev = p, p = p->sst_next)
    {
	اگر (p->sst_tick == tick && prev->sst_lnum + dist > p->sst_lnum)
	{
	    // Move this entry from used list to free list
	    prev->sst_next = p->sst_next;
	    syn_پشته_free_entry(syn_block, p);
	    p = prev;
	    retval = TRUE;
	}
    }
    بازگشت retval;
}

/*
 * Free the allocated memory for a syn_state item.
 * Move the entry into the free list.
 */
    ایستا تهی
syn_پشته_free_entry(synblock_T *block, synstate_T *p)
{
    اگر (block->b_sst_جستجو == p)
	block->b_sst_جستجو = پوچ;
    clear_syn_state(p);
    p->sst_next = block->b_sst_firstfree;
    block->b_sst_firstfree = p;
    ++block->b_sst_freecount;
}

/*
 * Find an entry in the list of state stacks at or before "lnum".
 * Returns NULL when there is no entry or the first entry is after "lnum".
 */
    ایستا synstate_T *
syn_پشته_find_entry(linenr_T lnum)
{
    synstate_T	*p, *prev;

    prev = پوچ;
    p = syn_block->b_sst_first;

    // The list is sorted by line number and lookups while parsing advance
    // monotonically, so resume from the last returned entry instead of
    // rescanning from the start whenever it is at or before "lnum".
    اگر (syn_block->b_sst_جستجو != پوچ
				 && syn_block->b_sst_جستجو->sst_lnum <= lnum)
	p = syn_block->b_sst_جستجو;

    برای ( ; p != پوچ; prev = p, p = p->sst_next)
    {
	اگر (p->sst_lnum == lnum)
	{
	    syn_block->b_sst_جستجو = p;
	    بازگشت p;
	}
	اگر (p->sst_lnum > lnum)
	    بشکن;
    }
    اگر (prev != پوچ)
	syn_block->b_sst_جستجو = prev;
    بازگشت prev;
}

/*
 * Try saving the current state in b_sst_array[].
 * The current state must be valid for the start of the current_lnum line!
 */
    ایستا synstate_T *
store_current_state(تهی)
{
    صحیح		i;
    synstate_T	*p;
    bufstate_T	*bp;
    stateitem_T	*cur_si;
    synstate_T	*sp = syn_پشته_find_entry(current_lnum);

    /*
     * If the current state contains a start or end pattern that continues
     * from the previous line, we can't use it.  Don't store it then.
     */
    برای (i = current_state.ga_len - 1; i >= 0; --i)
    {
	cur_si = &CUR_STATE(i);
	اگر (cur_si->si_h_startpos.lnum >= current_lnum
		|| cur_si->si_m_endpos.lnum >= current_lnum
		|| cur_si->si_h_endpos.lnum >= current_lnum
		|| (cur_si->si_end_idx
		    && cur_si->si_eoe_pos.lnum >= current_lnum))
	    بشکن;
    }
    اگر (i >= 0)
    {
	اگر (sp != پوچ)
	{
	    // find "sp" in the list and remove it
	    اگر (syn_block->b_sst_first == sp)
		// it's the first entry
		syn_block->b_sst_first = sp->sst_next;
	    وگرنه
	    {
		// find the entry just before this one to adjust sst_next
		FOR_ALL_SYNSTATES(syn_block, p)
		    اگر (p->sst_next == sp)
			بشکن;
		اگر (p != پوچ)	// just in case
		    p->sst_next = sp->sst_next;
	    }
	    syn_پشته_free_entry(syn_block, sp);
	    sp = پوچ;
	}
    }
    وگرنه اگر (sp == پوچ || sp->sst_lnum != current_lnum)
    {
	/*
	 * Add a new entry
	 */
	// If no free items, cleanup the array first.
	اگر (syn_block->b_sst_freecount == 0)
	{
	    (تهی)syn_پشته_cleanup();
	    // "sp" may have been moved to the freelist now
	    sp = syn_پشته_find_entry(current_lnum);
	}
	// Still no free items?  Must be a strange problem...
	اگر (syn_block->b_sst_freecount == 0)
	    sp = پوچ;
	وگرنه
	{
	    // Take the first item from the free list and put it in the used
	    // list, after *sp
	    p = syn_block->b_sst_firstfree;
	    syn_block->b_sst_firstfree = p->sst_next;
	    --syn_block->b_sst_freecount;
	    اگر (sp == پوچ)
	    {
		// Insert in front of the list
		p->sst_next = syn_block->b_sst_first;
		syn_block->b_sst_first = p;
	    }
	    وگرنه
	    {
		// insert in list after *sp
		p->sst_next = sp->sst_next;
		sp->sst_next = p;
	    }
	    sp = p;
	    sp->sst_stacksize = 0;
	    sp->sst_lnum = current_lnum;
	    // Resume the next forward lookup from the entry just stored.
	    syn_block->b_sst_جستجو = sp;
	}
    }
    اگر (sp != پوچ)
    {
	// When overwriting an existing state stack, clear it first
	clear_syn_state(sp);
	sp->sst_stacksize = current_state.ga_len;
	اگر (current_state.ga_len > SST_FIX_STATES)
	{
	    // Need to clear it, might be something remaining from when the
	    // length was less than SST_FIX_STATES.
	    ga_init2(&sp->sst_union.sst_ga, اندازه(bufstate_T), 1);
	    اگر (ga_grow(&sp->sst_union.sst_ga, current_state.ga_len) == FAIL)
		sp->sst_stacksize = 0;
	    وگرنه
		sp->sst_union.sst_ga.ga_len = current_state.ga_len;
	    bp = SYN_STATE_P(&(sp->sst_union.sst_ga));
	}
	وگرنه
	    bp = sp->sst_union.sst_پشته;
	برای (i = 0; i < sp->sst_stacksize; ++i)
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
    بازگشت sp;
}

/*
 * Copy a state stack from "from" in b_sst_array[] to current_state;
 */
    ایستا تهی
load_current_state(synstate_T *from)
{
    صحیح		i;
    bufstate_T	*bp;

    clear_current_state();
    validate_current_state();
    keepend_level = -1;
    اگر (from->sst_stacksize
	    && ga_grow(&current_state, from->sst_stacksize) == OK)
    {
	اگر (from->sst_stacksize > SST_FIX_STATES)
	    bp = SYN_STATE_P(&(from->sst_union.sst_ga));
	وگرنه
	    bp = from->sst_union.sst_پشته;
	برای (i = 0; i < from->sst_stacksize; ++i)
	{
	    CUR_STATE(i).si_idx = bp[i].bs_idx;
	    CUR_STATE(i).si_flags = bp[i].bs_flags;
#ifdef FEAT_CONCEAL
	    CUR_STATE(i).si_seqnr = bp[i].bs_seqnr;
	    CUR_STATE(i).si_cchar = bp[i].bs_cchar;
#endif
	    CUR_STATE(i).si_extmatch = ref_extmatch(bp[i].bs_extmatch);
	    اگر (keepend_level < 0 && (CUR_STATE(i).si_flags & HL_KEEPEND))
		keepend_level = i;
	    CUR_STATE(i).si_ends = FALSE;
	    CUR_STATE(i).si_m_lnum = 0;
	    اگر (CUR_STATE(i).si_idx >= 0)
		CUR_STATE(i).si_next_list =
		     (SYN_اقلام(syn_block)[CUR_STATE(i).si_idx]).sp_next_list;
	    وگرنه
		CUR_STATE(i).si_next_list = پوچ;
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
    ایستا صحیح
syn_پشته_equal(synstate_T *sp)
{
    صحیح		i, j;
    bufstate_T	*bp;
    reg_extmatch_T	*six, *bsx;

    // First a quick check if the stacks have the same size end nextlist.
    اگر (sp->sst_stacksize != current_state.ga_len
	    || sp->sst_next_list != current_next_list)
	بازگشت FALSE;

    // Need to compare all states on both stacks.
    اگر (sp->sst_stacksize > SST_FIX_STATES)
	bp = SYN_STATE_P(&(sp->sst_union.sst_ga));
    وگرنه
	bp = sp->sst_union.sst_پشته;

    برای (i = current_state.ga_len; --i >= 0; )
    {
	// If the item has another index the state is different.
	اگر (bp[i].bs_idx != CUR_STATE(i).si_idx)
	    بشکن;
	اگر (bp[i].bs_extmatch == CUR_STATE(i).si_extmatch)
	    ادامه;
	// When the extmatch pointers are different, the strings in them can
	// still be the same.  Check if the extmatch references are equal.
	bsx = bp[i].bs_extmatch;
	six = CUR_STATE(i).si_extmatch;
	// If one of the extmatch pointers is NULL the states are different.
	اگر (bsx == پوچ || six == پوچ)
	    بشکن;
	برای (j = 0; j < NSUBEXP; ++j)
	{
	    // Check each referenced match string. They must all be equal.
	    اگر (bsx->matches[j] != six->matches[j])
	    {
		// If the pointer is different it can still be the same text.
		// Compare the strings, ignore case when the start item has the
		// sp_ic flag set.
		اگر (bsx->matches[j] == پوچ || six->matches[j] == پوچ)
		    بشکن;
		اگر ((SYN_اقلام(syn_block)[CUR_STATE(i).si_idx]).sp_ic
			? MB_STRICMP(bsx->matches[j], six->matches[j]) != 0
			: STRCMP(bsx->matches[j], six->matches[j]) != 0)
		    بشکن;
	    }
	}
	اگر (j != NSUBEXP)
	    بشکن;
    }
    بازگشت i < 0 ? TRUE : FALSE;
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
    تهی
syntax_end_parsing(win_T *wp, linenr_T lnum)
{
    synstate_T	*sp;

    اگر (syn_block != wp->w_s)
	بازگشت;  // not the right window
    sp = syn_پشته_find_entry(lnum);
    اگر (sp != پوچ && sp->sst_lnum < lnum)
	sp = sp->sst_next;

    اگر (sp != پوچ && sp->sst_change_lnum != 0)
	sp->sst_change_lnum = lnum;
}

/*
 * End of handling of the state stack.
 ****************************************/

    ایستا تهی
invalidate_current_state(تهی)
{
    clear_current_state();
    current_state.ga_itemsize = 0;	// mark current_state invalid
    current_next_list = پوچ;
    keepend_level = -1;
}

    ایستا تهی
validate_current_state(تهی)
{
    current_state.ga_itemsize = اندازه(stateitem_T);
    current_state.ga_growsize = 3;
}

/*
 * Return TRUE if the syntax at start of lnum changed since last time.
 * This will only be called just after get_syntax_attr() for the previous
 * line, to check if the next line needs to be redrawn too.
 */
    صحیح
syntax_check_changed(linenr_T lnum)
{
    صحیح		retval = TRUE;
    synstate_T	*sp;

    /*
     * Check the state stack when:
     * - lnum is just below the previously syntaxed line.
     * - lnum is not before the lines with saved states.
     * - lnum is not past the lines with saved states.
     * - lnum is at or before the last changed line.
     */
    اگر (VALID_STATE(&current_state) && lnum == current_lnum + 1)
    {
	sp = syn_پشته_find_entry(lnum);
	اگر (sp != پوچ && sp->sst_lnum == lnum)
	{
	    /*
	     * finish the previous line (needed when not all of the line was
	     * drawn)
	     */
	    (تهی)syn_finish_سطر(FALSE);

	    /*
	     * Compare the current state with the previously saved state of
	     * the line.
	     */
	    اگر (syn_پشته_equal(sp))
		retval = FALSE;

	    /*
	     * Store the current state in b_sst_array[] for later use.
	     */
	    ++current_lnum;
	    (تهی)store_current_state();
	}
    }

    بازگشت retval;
}

/*
 * Finish the current line.
 * This doesn't return any attributes, it only gets the state at the end of
 * the line.  It can start anywhere in the line, as long as the current state
 * is valid.
 */
    ایستا صحیح
syn_finish_سطر(
    صحیح	    syncing)		// called for syncing
{
    stateitem_T	*cur_si;
    colnr_T	prev_current_col;

    وقتی (!current_finished)
    {
	(تهی)syn_current_attr(syncing, FALSE, پوچ, FALSE);
	/*
	 * When syncing, and found some item, need to check the item.
	 */
	اگر (syncing && current_state.ga_len)
	{
	    /*
	     * Check for match with sync item.
	     */
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	    اگر (cur_si->si_idx >= 0
		    && (SYN_اقلام(syn_block)[cur_si->si_idx].sp_flags
					  & (HL_SYNC_HERE|HL_SYNC_THERE)))
		بازگشت TRUE;

	    // syn_current_attr() will have skipped the check for an item
	    // that ends here, need to do that now.  Be careful not to go
	    // past the NUL.
	    prev_current_col = current_col;
	    اگر (syn_getcurline()[current_col] != NUL)
		++current_col;
	    check_state_ends();
	    current_col = prev_current_col;
	}
	++current_col;
    }
    بازگشت FALSE;
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
    صحیح
get_syntax_attr(
    colnr_T	col,
    صحیح		*can_spell,
    صحیح		keep_state)	// keep state of char at "col"
{
    صحیح	    attr = 0;

    اگر (can_spell != پوچ)
	// Default: Only do spelling when there is no @Spell cluster or when
	// ":syn spell toplevel" was used.
	*can_spell = syn_block->b_syn_spell == SYNSPL_DEFAULT
		    ? (syn_block->b_spell_cluster_id == 0)
		    : (syn_block->b_syn_spell == SYNSPL_بالاترین);

    // check for out of memory situation
    اگر (syn_block->b_sst_array == پوچ)
	بازگشت 0;

    // After 'synmaxcol' the attribute is always zero.
    اگر (syn_buf->b_p_smc > 0 && col >= (colnr_T)syn_buf->b_p_smc)
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
	بازگشت 0;
    }

    // Make sure current_state is valid
    اگر (INVALID_STATE(&current_state))
	validate_current_state();

    /*
     * Skip from the current column to "col", get the attributes for "col".
     */
    وقتی (current_col <= col)
    {
	attr = syn_current_attr(FALSE, TRUE, can_spell,
				     current_col == col ? keep_state : FALSE);
	++current_col;
    }

    بازگشت attr;
}

/*
 * Get syntax attributes for current_lnum, current_col.
 */
    ایستا صحیح
syn_current_attr(
    صحیح		syncing,		// When 1: called for syncing
    صحیح		displaying,		// result will be displayed
    صحیح		*can_spell,		// return: do spell checking
    صحیح		keep_state)		// keep syntax stack afterwards
{
    صحیح		syn_id;
    lpos_T	endpos;		// was: char_u *endp;
    lpos_T	hl_startpos;	// was: int hl_startcol;
    lpos_T	hl_endpos;
    lpos_T	eos_pos;	// end-of-start match (start region)
    lpos_T	eoe_pos;	// end-of-end pattern
    صحیح		end_idx;	// group ID for end pattern
    صحیح		idx;
    synpat_T	*spp;
    stateitem_T	*cur_si, *sip = پوچ;
    صحیح		startcol;
    صحیح		endcol;
    long	flags;
    صحیح		cchar;
    short	*next_list;
    صحیح		found_match;		    // found usable match
    ایستا صحیح	try_next_column = FALSE;    // must try in next col
    صحیح		do_keywords;
    regmmatch_T	regmatch;
    lpos_T	pos;
    صحیح		lc_col;
    reg_extmatch_T *cur_extmatch = پوچ;
    char_u	buf_chartab[32];  // chartab array for syn iskyeyword
    char_u	*سطر;		// current line.  NOTE: becomes invalid after
				// looking for a pattern match!

    // variables for zero-width matches that have a "nextgroup" argument
    صحیح		keep_next_list;
    صحیح		zero_width_next_list = FALSE;
    garray_T	zero_width_next_ga;

    /*
     * No character, no attributes!  Past end of line?
     * Do try matching with an empty line (could be the start of a region).
     */
    سطر = syn_getcurline();
    اگر (سطر[current_col] == NUL && current_col != 0)
    {
	/*
	 * If we found a match after the last column, use it.
	 */
	اگر (next_match_idx >= 0 && next_match_col >= (صحیح)current_col
						  && next_match_col != MAXCOL)
	    (تهی)افزودن_next_match(پوچ);

	current_finished = TRUE;
	current_state_stored = FALSE;
	بازگشت 0;
    }

    // if the current or next character is NUL, we will finish the line now
    اگر (سطر[current_col] == NUL || سطر[current_col + 1] == NUL)
    {
	current_finished = TRUE;
	current_state_stored = FALSE;
    }

    /*
     * When in the previous column there was a match but it could not be used
     * (empty match or already matched in this column) need to try again in
     * the next column.
     */
    اگر (try_next_column)
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
    ga_init2(&zero_width_next_ga, اندازه(صحیح), 10);

    // use syntax iskeyword option
    save_chartab(buf_chartab);

    /*
     * Repeat matching keywords and patterns, to find contained items at the
     * same column.  This stops when there are no extra matches at the current
     * column.
     */
    انجامبده
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
	اگر (current_state.ga_len)
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	وگرنه
	    cur_si = پوچ;

	اگر (syn_block->b_syn_containedin || cur_si == پوچ
					      || cur_si->si_cont_list != پوچ)
	{
	    /*
	     * 2. Check for keywords, if on a keyword char after a non-keyword
	     *	  char.  Don't do this when syncing.
	     */
	    اگر (do_keywords)
	    {
	      سطر = syn_getcurline();
	      اگر (vim_iswordp_buf(سطر + current_col, syn_buf)
		      && (current_col == 0
			  || !vim_iswordp_buf(سطر + current_col - 1
			      - (has_mbyte
				  ? (*mb_head_off)(سطر, سطر + current_col - 1)
				  : 0) , syn_buf)))
	      {
		syn_id = check_keyword_id(سطر, (صحیح)current_col,
					 &endcol, &flags, &next_list, cur_si,
					 &cchar);
		اگر (syn_id != 0)
		{
		    اگر (افزودن_current_state(KEYWORD_IDX) == OK)
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
			اگر (current_state.ga_len > 1)
			    cur_si->si_flags |=
				  CUR_STATE(current_state.ga_len - 2).si_flags
								 & HL_CONCEAL;
#endif
			cur_si->si_id = syn_id;
			cur_si->si_trans_id = syn_id;
			اگر (flags & HL_TRANSP)
			{
			    اگر (current_state.ga_len < 2)
			    {
				cur_si->si_attr = 0;
				cur_si->si_trans_id = 0;
			    }
			    وگرنه
			    {
				cur_si->si_attr = CUR_STATE(
					current_state.ga_len - 2).si_attr;
				cur_si->si_trans_id = CUR_STATE(
					current_state.ga_len - 2).si_trans_id;
			    }
			}
			وگرنه
			    cur_si->si_attr = syn_id2attr(syn_id);
			cur_si->si_cont_list = پوچ;
			cur_si->si_next_list = next_list;
			check_keepend();
		    }
		    وگرنه
			vim_free(next_list);
		}
	      }
	    }

	    /*
	     * 3. Check for patterns (only if no keyword found).
	     */
	    اگر (syn_id == 0 && syn_block->b_syn_patterns.ga_len)
	    {
		/*
		 * If we didn't check for a match yet, or we are past it, check
		 * for any match with a pattern.
		 */
		اگر (next_match_idx < 0 || next_match_col < (صحیح)current_col)
		{
		    /*
		     * Check all relevant patterns for a match at this
		     * position.  This is complicated, because matching with a
		     * pattern takes quite a bit of time, thus we want to
		     * avoid doing it when it's not needed.
		     */
		    next_match_idx = 0;		// no match in this line yet
		    next_match_col = MAXCOL;
		    برای (idx = syn_block->b_syn_patterns.ga_len; --idx >= 0; )
		    {
			spp = &(SYN_اقلام(syn_block)[idx]);
			اگر (	   spp->sp_syncing == syncing
				&& (displaying || !(spp->sp_flags & HL_DISPLAY))
				&& (spp->sp_type == SPTYPE_MATCH
				    || spp->sp_type == SPTYPE_START)
				&& (current_next_list != پوچ
				    ? in_id_list(پوچ, current_next_list,
							      &spp->sp_syn, 0)
				    : (cur_si == پوچ
					? !(spp->sp_flags & HL_CONTAINED)
					: in_id_list(cur_si,
					    cur_si->si_cont_list, &spp->sp_syn,
					    spp->sp_flags))))
			{
			    صحیح r;

			    // If we already tried matching in this line, and
			    // there isn't a match before next_match_col, skip
			    // this item.
			    اگر (spp->sp_سطر_id == current_سطر_id
				    && spp->sp_startcol >= next_match_col)
				ادامه;
			    spp->sp_سطر_id = current_سطر_id;

			    lc_col = current_col - spp->sp_offsets[SPO_LC_OFF];
			    اگر (lc_col < 0)
				lc_col = 0;

			    regmatch.rmm_ic = spp->sp_ic;
			    regmatch.regprog = spp->sp_prog;
			    r = syn_regexec(&regmatch,
					     current_lnum,
					     (colnr_T)lc_col,
					     IF_SYN_TIME(&spp->sp_time));
			    spp->sp_prog = regmatch.regprog;
			    اگر (!r)
			    {
				// no match in this line, try another one
				spp->sp_startcol = MAXCOL;
				ادامه;
			    }

			    /*
			     * Compute the first column of the match.
			     */
			    syn_add_start_off(&pos, &regmatch,
							 spp, SPO_MS_OFF, -1);
			    اگر (pos.lnum > current_lnum)
			    {
				// must have used end of match in a next line,
				// we can't handle that
				spp->sp_startcol = MAXCOL;
				ادامه;
			    }
			    startcol = pos.col;

			    // remember the next column where this pattern
			    // matches in the current line
			    spp->sp_startcol = startcol;

			    /*
			     * If a previously found match starts at a lower
			     * column number, don't use this one.
			     */
			    اگر (startcol >= next_match_col)
				ادامه;

			    /*
			     * If we matched this pattern at this position
			     * before, skip it.  Must retry in the next
			     * column, because it may match from there.
			     */
			    اگر (did_match_already(idx, &zero_width_next_ga))
			    {
				try_next_column = TRUE;
				ادامه;
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
			    re_extmatch_out = پوچ;

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
			    اگر (spp->sp_type == SPTYPE_START
					      && (spp->sp_flags & HL_ONELINE))
			    {
				lpos_T	startpos;

				startpos = endpos;
				find_endpos(idx, &startpos, &endpos, &hl_endpos,
				    &flags, &eoe_pos, &end_idx, cur_extmatch);
				اگر (endpos.lnum == 0)
				    ادامه;	    // not found
			    }

			    /*
			     * For a "match" the size must be > 0 after the
			     * end offset needs has been added.  Except when
			     * syncing.
			     */
			    وگرنه اگر (spp->sp_type == SPTYPE_MATCH)
			    {
				syn_add_end_off(&hl_endpos, &regmatch, spp,
							       SPO_HE_OFF, 0);
				syn_add_end_off(&endpos, &regmatch, spp,
							       SPO_ME_OFF, 0);
				اگر (endpos.lnum == current_lnum
				      && (صحیح)endpos.col + syncing < startcol)
				{
				    /*
				     * If an empty string is matched, may need
				     * to try matching again at next column.
				     */
				    اگر (regmatch.startpos[0].col
						    == regmatch.endpos[0].col)
					try_next_column = TRUE;
				    ادامه;
				}
			    }

			    /*
			     * keep the best match so far in next_match_*
			     */
			    // Highlighting must start after startpos and end
			    // before endpos.
			    اگر (hl_startpos.lnum == current_lnum
					   && (صحیح)hl_startpos.col < startcol)
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
			    cur_extmatch = پوچ;
			}
		    }
		}

		/*
		 * If we found a match at the current column, use it.
		 */
		اگر (next_match_idx >= 0 && next_match_col == (صحیح)current_col)
		{
		    synpat_T	*lspp;

		    // When a zero-width item matched which has a nextgroup,
		    // don't push the item but set nextgroup.
		    lspp = &(SYN_اقلام(syn_block)[next_match_idx]);
		    اگر (next_match_m_endpos.lnum == current_lnum
			    && next_match_m_endpos.col == current_col
			    && lspp->sp_next_list != پوچ)
		    {
			current_next_list = lspp->sp_next_list;
			current_next_flags = lspp->sp_flags;
			keep_next_list = TRUE;
			zero_width_next_list = TRUE;

			// Add the index to a list, so that we can check
			// later that we don't match it again (and cause an
			// endless loop).
			اگر (ga_grow(&zero_width_next_ga, 1) == OK)
			{
			    ((صحیح *)(zero_width_next_ga.ga_data))
				[zero_width_next_ga.ga_len++] = next_match_idx;
			}
			next_match_idx = -1;
		    }
		    وگرنه
			cur_si = افزودن_next_match(cur_si);
		    found_match = TRUE;
		}
	    }
	}

	/*
	 * Handle searching for nextgroup match.
	 */
	اگر (current_next_list != پوچ && !keep_next_list)
	{
	    /*
	     * If a nextgroup was not found, continue looking for one if:
	     * - this is an empty line and the "skipempty" option was given
	     * - we are on white space and the "skipwhite" option was given
	     */
	    اگر (!found_match)
	    {
		سطر = syn_getcurline();
		اگر (((current_next_flags & HL_SKIPWHITE)
			    && VIM_ISWHITE(سطر[current_col]))
			|| ((current_next_flags & HL_SKIPEMPTY)
			    && *سطر == NUL))
		    بشکن;
	    }

	    /*
	     * If a nextgroup was found: Use it, and continue looking for
	     * contained matches.
	     * If a nextgroup was not found: Continue looking for a normal
	     * match.
	     * When did set current_next_list for a zero-width item and no
	     * match was found don't loop (would get stuck).
	     */
	    current_next_list = پوچ;
	    next_match_idx = -1;
	    اگر (!zero_width_next_list)
		found_match = TRUE;
	}

    } وقتی (found_match);

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
    اگر (cur_si != پوچ)
    {
#ifndef FEAT_EVAL
	صحیح	current_trans_id = 0;
#endif
	برای (idx = current_state.ga_len - 1; idx >= 0; --idx)
	{
	    sip = &CUR_STATE(idx);
	    اگر ((current_lnum > sip->si_h_startpos.lnum
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
		بشکن;
	    }
	}

	اگر (can_spell != پوچ)
	{
	    ساختار sp_syn   sps;

	    /*
	     * set "can_spell" to TRUE if spell checking is supposed to be
	     * done in the current item.
	     */
	    اگر (syn_block->b_spell_cluster_id == 0)
	    {
		// There is no @Spell cluster: Do spelling for items without
		// @NoSpell cluster.
		اگر (syn_block->b_nospell_cluster_id == 0
						     || current_trans_id == 0)
		    *can_spell = (syn_block->b_syn_spell != SYNSPL_NOTOP);
		وگرنه
		{
		    sps.inc_tag = 0;
		    sps.id = syn_block->b_nospell_cluster_id;
		    sps.cont_in_list = پوچ;
		    *can_spell = !in_id_list(sip, sip->si_cont_list, &sps, 0);
		}
	    }
	    وگرنه
	    {
		// The @Spell cluster is defined: Do spelling in items with
		// the @Spell cluster.  But not when @NoSpell is also there.
		// At the toplevel only spell check when ":syn spell toplevel"
		// was used.
		اگر (current_trans_id == 0)
		    *can_spell = (syn_block->b_syn_spell == SYNSPL_بالاترین);
		وگرنه
		{
		    sps.inc_tag = 0;
		    sps.id = syn_block->b_spell_cluster_id;
		    sps.cont_in_list = پوچ;
		    *can_spell = in_id_list(sip, sip->si_cont_list, &sps, 0);

		    اگر (syn_block->b_nospell_cluster_id != 0)
		    {
			sps.id = syn_block->b_nospell_cluster_id;
			اگر (in_id_list(sip, sip->si_cont_list, &sps, 0))
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
	اگر (!syncing && !keep_state)
	{
	    check_state_ends();
	    اگر (current_state.ga_len > 0
				      && syn_getcurline()[current_col] != NUL)
	    {
		++current_col;
		check_state_ends();
		--current_col;
	    }
	}
    }
    وگرنه اگر (can_spell != پوچ)
	// Default: Only do spelling when there is no @Spell cluster or when
	// ":syn spell toplevel" was used.
	*can_spell = syn_block->b_syn_spell == SYNSPL_DEFAULT
		    ? (syn_block->b_spell_cluster_id == 0)
		    : (syn_block->b_syn_spell == SYNSPL_بالاترین);

    // nextgroup ends at end of line, unless "skipnl" or "skipempty" present
    اگر (current_next_list != پوچ
	    && (سطر = syn_getcurline())[current_col] != NUL
	    && سطر[current_col + 1] == NUL
	    && !(current_next_flags & (HL_SKIPNL | HL_SKIPEMPTY)))
	current_next_list = پوچ;

    اگر (zero_width_next_ga.ga_len > 0)
	ga_clear(&zero_width_next_ga);

    // No longer need external matches.  But keep next_match_extmatch.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = پوچ;
    unref_extmatch(cur_extmatch);

    بازگشت current_attr;
}


/*
 * Check if we already matched pattern "idx" at the current column.
 */
    ایستا صحیح
did_match_already(صحیح idx, garray_T *gap)
{
    صحیح		i;

    برای (i = current_state.ga_len; --i >= 0; )
	اگر (CUR_STATE(i).si_m_startcol == (صحیح)current_col
		&& CUR_STATE(i).si_m_lnum == (صحیح)current_lnum
		&& CUR_STATE(i).si_idx == idx)
	    بازگشت TRUE;

    // Zero-width matches with a nextgroup argument are not put on the syntax
    // stack, and can only be matched once anyway.
    برای (i = gap->ga_len; --i >= 0; )
	اگر (((صحیح *)(gap->ga_data))[i] == idx)
	    بازگشت TRUE;

    بازگشت FALSE;
}

/*
 * Push the next match onto the stack.
 */
    ایستا stateitem_T *
افزودن_next_match(stateitem_T *cur_si)
{
    synpat_T	*spp;
#ifdef FEAT_CONCEAL
    صحیح		 save_flags;
#endif

    spp = &(SYN_اقلام(syn_block)[next_match_idx]);

    /*
     * Push the item in current_state stack;
     */
    اگر (افزودن_current_state(next_match_idx) == OK)
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
	اگر (current_state.ga_len > 1)
	    cur_si->si_flags |=
		    CUR_STATE(current_state.ga_len - 2).si_flags & HL_CONCEAL;
#endif
	cur_si->si_next_list = spp->sp_next_list;
	cur_si->si_extmatch = ref_extmatch(next_match_extmatch);
	اگر (spp->sp_type == SPTYPE_START && !(spp->sp_flags & HL_ONELINE))
	{
	    // Try to find the end pattern in the current line
	    update_si_end(cur_si, (صحیح)(next_match_m_endpos.col), TRUE);
	    check_keepend();
	}
	وگرنه
	{
	    cur_si->si_m_endpos = next_match_m_endpos;
	    cur_si->si_h_endpos = next_match_h_endpos;
	    cur_si->si_ends = TRUE;
	    cur_si->si_flags |= next_match_flags;
	    cur_si->si_eoe_pos = next_match_eoe_pos;
	    cur_si->si_end_idx = next_match_end_idx;
	}
	اگر (keepend_level < 0 && (cur_si->si_flags & HL_KEEPEND))
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
	اگر (	   spp->sp_type == SPTYPE_START
		&& spp->sp_syn_match_id != 0
		&& افزودن_current_state(next_match_idx) == OK)
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
	    اگر (cur_si->si_flags & HL_CONCEALENDS)
		cur_si->si_flags |= HL_CONCEAL;
#endif
	    cur_si->si_next_list = پوچ;
	    check_keepend();
	    update_si_attr(current_state.ga_len - 1);
	}
    }

    next_match_idx = -1;	// try other match next time

    بازگشت cur_si;
}

/*
 * Check for end of current state (and the states before it).
 */
    ایستا تهی
check_state_ends(تهی)
{
    stateitem_T	*cur_si;
    صحیح		had_extend;

    cur_si = &CUR_STATE(current_state.ga_len - 1);
    برای (;;)
    {
	اگر (cur_si->si_ends
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
	    اگر (cur_si->si_end_idx
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
		اگر (cur_si->si_flags & HL_CONCEALENDS)
		    cur_si->si_flags |= HL_CONCEAL;
#endif
		update_si_attr(current_state.ga_len - 1);

		// nextgroup= should not match in the end pattern
		current_next_list = پوچ;

		// what matches next may be different now, clear it
		next_match_idx = 0;
		next_match_col = MAXCOL;
		بشکن;
	    }

	    // handle next_list, unless at end of line and no "skipnl" or
	    // "skipempty"
	    current_next_list = cur_si->si_next_list;
	    current_next_flags = cur_si->si_flags;
	    اگر (!(current_next_flags & (HL_SKIPNL | HL_SKIPEMPTY))
		    && syn_getcurline()[current_col] == NUL)
		current_next_list = پوچ;

	    // When the ended item has "extend", another item with
	    // "keepend" now needs to check for its end.
	    had_extend = (cur_si->si_flags & HL_EXTEND);

	    برداشت_current_state();

	    اگر (current_state.ga_len == 0)
		بشکن;

	    اگر (had_extend && keepend_level >= 0)
	    {
		syn_update_ends(FALSE);
		اگر (current_state.ga_len == 0)
		    بشکن;
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
	    اگر (cur_si->si_idx >= 0
		    && SYN_اقلام(syn_block)[cur_si->si_idx].sp_type
								== SPTYPE_START
		    && !(cur_si->si_flags & (HL_MATCH | HL_KEEPEND)))
	    {
		update_si_end(cur_si, (صحیح)current_col, TRUE);
		check_keepend();
		اگر ((current_next_flags & HL_HAS_EOL)
			&& keepend_level < 0
			&& syn_getcurline()[current_col] == NUL)
		    بشکن;
	    }
	}
	وگرنه
	    بشکن;
    }
}

/*
 * Update an entry in the current_state stack for a match or region.  This
 * fills in si_attr, si_next_list and si_cont_list.
 */
    ایستا تهی
update_si_attr(صحیح idx)
{
    stateitem_T	*sip = &CUR_STATE(idx);
    synpat_T	*spp;

    // This should not happen...
    اگر (sip->si_idx < 0)
	بازگشت;

    spp = &(SYN_اقلام(syn_block)[sip->si_idx]);
    اگر (sip->si_flags & HL_MATCH)
	sip->si_id = spp->sp_syn_match_id;
    وگرنه
	sip->si_id = spp->sp_syn.id;
    sip->si_attr = syn_id2attr(sip->si_id);
    sip->si_trans_id = sip->si_id;
    اگر (sip->si_flags & HL_MATCH)
	sip->si_cont_list = پوچ;
    وگرنه
	sip->si_cont_list = spp->sp_cont_list;

    /*
     * For transparent items, take attr from outer item.
     * Also take cont_list, if there is none.
     * Don't do this for the matchgroup of a start or end pattern.
     */
    اگر ((spp->sp_flags & HL_TRANSP) && !(sip->si_flags & HL_MATCH))
    {
	اگر (idx == 0)
	{
	    sip->si_attr = 0;
	    sip->si_trans_id = 0;
	    اگر (sip->si_cont_list == پوچ)
		sip->si_cont_list = ID_LIST_ALL;
	}
	وگرنه
	{
	    sip->si_attr = CUR_STATE(idx - 1).si_attr;
	    sip->si_trans_id = CUR_STATE(idx - 1).si_trans_id;
	    اگر (sip->si_cont_list == پوچ)
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
    ایستا تهی
check_keepend(تهی)
{
    صحیح		i;
    lpos_T	maxpos;
    lpos_T	maxpos_h;
    stateitem_T	*sip;

    /*
     * This check can consume a lot of time; only do it from the level where
     * there really is a keepend.
     */
    اگر (keepend_level < 0)
	بازگشت;

    /*
     * Find the last index of an "extend" item.  "keepend" items before that
     * won't do anything.  If there is no "extend" item "i" will be
     * "keepend_level" and all "keepend" items will work normally.
     */
    برای (i = current_state.ga_len - 1; i > keepend_level; --i)
	اگر (CUR_STATE(i).si_flags & HL_EXTEND)
	    بشکن;

    maxpos.lnum = 0;
    maxpos.col = 0;
    maxpos_h.lnum = 0;
    maxpos_h.col = 0;
    برای ( ; i < current_state.ga_len; ++i)
    {
	sip = &CUR_STATE(i);
	اگر (maxpos.lnum != 0)
	{
	    limit_pos_zero(&sip->si_m_endpos, &maxpos);
	    limit_pos_zero(&sip->si_h_endpos, &maxpos_h);
	    limit_pos_zero(&sip->si_eoe_pos, &maxpos);
	    sip->si_ends = TRUE;
	}
	اگر (sip->si_ends && (sip->si_flags & HL_KEEPEND))
	{
	    اگر (maxpos.lnum == 0
		    || maxpos.lnum > sip->si_m_endpos.lnum
		    || (maxpos.lnum == sip->si_m_endpos.lnum
			&& maxpos.col > sip->si_m_endpos.col))
		maxpos = sip->si_m_endpos;
	    اگر (maxpos_h.lnum == 0
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
    ایستا تهی
update_si_end(
    stateitem_T	*sip,
    صحیح		startcol,   // where to start searching for the end
    صحیح		force)	    // when TRUE overrule a previous end
{
    lpos_T	startpos;
    lpos_T	endpos;
    lpos_T	hl_endpos;
    lpos_T	end_endpos;
    صحیح		end_idx;

    // return quickly for a keyword
    اگر (sip->si_idx < 0)
	بازگشت;

    // Don't update when it's already done.  Can be a match of an end pattern
    // that started in a previous line.  Watch out: can also be a "keepend"
    // from a containing item.
    اگر (!force && sip->si_m_endpos.lnum >= current_lnum)
	بازگشت;

    /*
     * We need to find the end of the region.  It may continue in the next
     * line.
     */
    end_idx = 0;
    startpos.lnum = current_lnum;
    startpos.col = startcol;
    find_endpos(sip->si_idx, &startpos, &endpos, &hl_endpos,
		   &(sip->si_flags), &end_endpos, &end_idx, sip->si_extmatch);

    اگر (endpos.lnum == 0)
    {
	// No end pattern matched.
	اگر (SYN_اقلام(syn_block)[sip->si_idx].sp_flags & HL_ONELINE)
	{
	    // a "oneline" never continues in the next line
	    sip->si_ends = TRUE;
	    sip->si_m_endpos.lnum = current_lnum;
	    sip->si_m_endpos.col = syn_getcurline_len();
	}
	وگرنه
	{
	    // continues in the next line
	    sip->si_ends = FALSE;
	    sip->si_m_endpos.lnum = 0;
	}
	sip->si_h_endpos = sip->si_m_endpos;
    }
    وگرنه
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
    ایستا صحیح
افزودن_current_state(صحیح idx)
{
    اگر (ga_grow(&current_state, 1) == FAIL)
	بازگشت FAIL;
    CLEAR_POINTER(&CUR_STATE(current_state.ga_len));
    CUR_STATE(current_state.ga_len).si_idx = idx;
    ++current_state.ga_len;
    بازگشت OK;
}

/*
 * Remove a state from the current_state stack.
 */
    ایستا تهی
برداشت_current_state(تهی)
{
    اگر (current_state.ga_len)
    {
	unref_extmatch(CUR_STATE(current_state.ga_len - 1).si_extmatch);
	--current_state.ga_len;
    }
    // after the end of a pattern, try matching a keyword or pattern
    next_match_idx = -1;

    // if first state with "keepend" is popped, reset keepend_level
    اگر (keepend_level >= current_state.ga_len)
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
    ایستا تهی
find_endpos(
    صحیح		idx,		// index of the pattern
    lpos_T	*startpos,	// where to start looking for an END match
    lpos_T	*m_endpos,	// return: end of match
    lpos_T	*hl_endpos,	// return: end of highlighting
    long	*flagsp,	// return: flags of matching END
    lpos_T	*end_endpos,	// return: end of end pattern match
    صحیح		*end_idx,	// return: group ID for end pat. match, or 0
    reg_extmatch_T *start_ext)	// submatches from the start pattern
{
    colnr_T	matchcol;
    synpat_T	*spp, *spp_skip;
    صحیح		start_idx;
    صحیح		best_idx;
    regmmatch_T	regmatch;
    regmmatch_T	best_regmatch;	    // startpos/endpos of best match
    lpos_T	pos;
    صحیح		had_match = FALSE;
    char_u	buf_chartab[32];  // chartab array for syn option iskyeyword

    // just in case we are invoked for a keyword
    اگر (idx < 0)
	بازگشت;

    /*
     * Check for being called with a START pattern.
     * Can happen with a match that continues to the next line, because it
     * contained a region.
     */
    spp = &(SYN_اقلام(syn_block)[idx]);
    اگر (spp->sp_type != SPTYPE_START)
    {
	*hl_endpos = *startpos;
	بازگشت;
    }

    /*
     * Find the SKIP or first END pattern after the last START pattern.
     */
    برای (;;)
    {
	spp = &(SYN_اقلام(syn_block)[idx]);
	اگر (spp->sp_type != SPTYPE_START)
	    بشکن;
	++idx;
    }

    /*
     *	Lookup the SKIP pattern (if present)
     */
    اگر (spp->sp_type == SPTYPE_SKIP)
    {
	spp_skip = spp;
	++idx;
    }
    وگرنه
	spp_skip = پوچ;

    // Setup external matches for syn_regexec().
    unref_extmatch(re_extmatch_in);
    re_extmatch_in = ref_extmatch(start_ext);

    matchcol = startpos->col;	// start looking for a match at sstart
    start_idx = idx;		// remember the first END pattern.
    best_regmatch.startpos[0].col = 0;		// avoid compiler warning

    // use syntax iskeyword option
    save_chartab(buf_chartab);

    برای (;;)
    {
	/*
	 * Find end pattern that matches first after "matchcol".
	 */
	best_idx = -1;
	برای (idx = start_idx; idx < syn_block->b_syn_patterns.ga_len; ++idx)
	{
	    صحیح lc_col = matchcol;
	    صحیح r;

	    spp = &(SYN_اقلام(syn_block)[idx]);
	    اگر (spp->sp_type != SPTYPE_END)	// past last END pattern
		بشکن;
	    lc_col -= spp->sp_offsets[SPO_LC_OFF];
	    اگر (lc_col < 0)
		lc_col = 0;

	    regmatch.rmm_ic = spp->sp_ic;
	    regmatch.regprog = spp->sp_prog;
	    r = syn_regexec(&regmatch, startpos->lnum, lc_col,
						  IF_SYN_TIME(&spp->sp_time));
	    spp->sp_prog = regmatch.regprog;
	    اگر (r)
	    {
		اگر (best_idx == -1 || regmatch.startpos[0].col
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
	اگر (best_idx == -1)
	    بشکن;

	/*
	 * If the skip pattern matches before the end pattern,
	 * continue searching after the skip pattern.
	 */
	اگر (spp_skip != پوچ)
	{
	    صحیح lc_col = matchcol - spp_skip->sp_offsets[SPO_LC_OFF];
	    صحیح r;

	    اگر (lc_col < 0)
		lc_col = 0;
	    regmatch.rmm_ic = spp_skip->sp_ic;
	    regmatch.regprog = spp_skip->sp_prog;
	    r = syn_regexec(&regmatch, startpos->lnum, lc_col,
					      IF_SYN_TIME(&spp_skip->sp_time));
	    spp_skip->sp_prog = regmatch.regprog;
	    اگر (r && regmatch.startpos[0].col
					     <= best_regmatch.startpos[0].col)
	    {
		صحیح سطر_len;

		// Add offset to skip pattern match
		syn_add_end_off(&pos, &regmatch, spp_skip, SPO_ME_OFF, 1);

		// If the skip pattern goes on to the next line, there is no
		// match with an end pattern in this line.
		اگر (pos.lnum > startpos->lnum)
		    بشکن;

		سطر_len = ml_get_buf_len(syn_buf, startpos->lnum);

		// take care of an empty match or negative offset
		اگر (pos.col <= matchcol)
		    ++matchcol;
		وگرنه اگر (pos.col <= regmatch.endpos[0].col)
		    matchcol = pos.col;
		وگرنه
		    // Be careful not to jump over the NUL at the end-of-line
		    برای (matchcol = regmatch.endpos[0].col;
			    matchcol < سطر_len && matchcol < pos.col;
								   ++matchcol)
			;

		// if the skip pattern includes end-of-line, break here
		اگر (matchcol >= سطر_len)
		    بشکن;

		ادامه;	    // start with first end pattern again
	    }
	}

	/*
	 * Match from start pattern to end pattern.
	 * Correct for match and highlight offset of end pattern.
	 */
	spp = &(SYN_اقلام(syn_block)[best_idx]);
	syn_add_end_off(m_endpos, &best_regmatch, spp, SPO_ME_OFF, 1);
	// can't end before the start
	اگر (m_endpos->lnum == startpos->lnum && m_endpos->col < startpos->col)
	    m_endpos->col = startpos->col;

	syn_add_end_off(end_endpos, &best_regmatch, spp, SPO_HE_OFF, 1);
	// can't end before the start
	اگر (end_endpos->lnum == startpos->lnum
					   && end_endpos->col < startpos->col)
	    end_endpos->col = startpos->col;
	// can't end after the match
	limit_pos(end_endpos, m_endpos);

	/*
	 * If the end group is highlighted differently, adjust the pointers.
	 */
	اگر (spp->sp_syn_match_id != spp->sp_syn.id && spp->sp_syn_match_id != 0)
	{
	    *end_idx = best_idx;
	    اگر (spp->sp_off_flags & (1 << (SPO_RE_OFF + SPO_شمار)))
	    {
		hl_endpos->lnum = best_regmatch.endpos[0].lnum;
		hl_endpos->col = best_regmatch.endpos[0].col;
	    }
	    وگرنه
	    {
		hl_endpos->lnum = best_regmatch.startpos[0].lnum;
		hl_endpos->col = best_regmatch.startpos[0].col;
	    }
	    hl_endpos->col += spp->sp_offsets[SPO_RE_OFF];

	    // can't end before the start
	    اگر (hl_endpos->lnum == startpos->lnum
					    && hl_endpos->col < startpos->col)
		hl_endpos->col = startpos->col;
	    limit_pos(hl_endpos, m_endpos);

	    // now the match ends where the highlighting ends, it is turned
	    // into the matchgroup for the end
	    *m_endpos = *hl_endpos;
	}
	وگرنه
	{
	    *end_idx = 0;
	    *hl_endpos = *end_endpos;
	}

	*flagsp = spp->sp_flags;

	had_match = TRUE;
	بشکن;
    }

    // no match for an END pattern in this line
    اگر (!had_match)
	m_endpos->lnum = 0;

    restore_chartab(buf_chartab);

    // Remove external matches.
    unref_extmatch(re_extmatch_in);
    re_extmatch_in = پوچ;
}

/*
 * Limit "pos" not to be after "limit".
 */
    ایستا تهی
limit_pos(lpos_T *pos, lpos_T *limit)
{
    اگر (pos->lnum > limit->lnum)
	*pos = *limit;
    وگرنه اگر (pos->lnum == limit->lnum && pos->col > limit->col)
	pos->col = limit->col;
}

/*
 * Limit "pos" not to be after "limit", unless pos->lnum is zero.
 */
    ایستا تهی
limit_pos_zero(
    lpos_T	*pos,
    lpos_T	*limit)
{
    اگر (pos->lnum == 0)
	*pos = *limit;
    وگرنه
	limit_pos(pos, limit);
}

/*
 * Add offset to matched text for end of match or highlight.
 */
    ایستا تهی
syn_add_end_off(
    lpos_T	*result,	// returned position
    regmmatch_T	*regmatch,	// start/end of match
    synpat_T	*spp,		// matched pattern
    صحیح		idx,		// index of offset
    صحیح		extra)		// extra chars for offset to start
{
    صحیح		col;
    صحیح		off;
    char_u	*base;
    char_u	*p;

    اگر (spp->sp_off_flags & (1 << idx))
    {
	result->lnum = regmatch->startpos[0].lnum;
	col = regmatch->startpos[0].col;
	off = spp->sp_offsets[idx] + extra;
    }
    وگرنه
    {
	result->lnum = regmatch->endpos[0].lnum;
	col = regmatch->endpos[0].col;
	off = spp->sp_offsets[idx];
    }
    // Don't go past the end of the line.  Matters for "rs=e+2" when there
    // is a matchgroup. Watch out for match with last NL in the buffer.
    اگر (result->lnum > syn_buf->b_ml.ml_سطر_شمار)
	col = 0;
    وگرنه اگر (off != 0)
    {
	base = ml_get_buf(syn_buf, result->lnum, FALSE);
	p = base + col;
	اگر (off > 0)
	{
	    وقتی (off-- > 0 && *p != NUL)
		MB_PTR_ADV(p);
	}
	وگرنه اگر (off < 0)
	{
	    وقتی (off++ < 0 && base < p)
		MB_PTR_BACK(base, p);
	}
	col = (صحیح)(p - base);
    }
    result->col = col;
}

/*
 * Add offset to matched text for start of match or highlight.
 * Avoid resulting column to become negative.
 */
    ایستا تهی
syn_add_start_off(
    lpos_T	*result,	// returned position
    regmmatch_T	*regmatch,	// start/end of match
    synpat_T	*spp,
    صحیح		idx,
    صحیح		extra)	    // extra chars for offset to end
{
    صحیح		col;
    صحیح		off;
    char_u	*base;
    char_u	*p;

    اگر (spp->sp_off_flags & (1 << (idx + SPO_شمار)))
    {
	result->lnum = regmatch->endpos[0].lnum;
	col = regmatch->endpos[0].col;
	off = spp->sp_offsets[idx] + extra;
    }
    وگرنه
    {
	result->lnum = regmatch->startpos[0].lnum;
	col = regmatch->startpos[0].col;
	off = spp->sp_offsets[idx];
    }
    اگر (result->lnum > syn_buf->b_ml.ml_سطر_شمار)
    {
	// a "\n" at the end of the pattern may take us below the last line
	result->lnum = syn_buf->b_ml.ml_سطر_شمار;
	col = ml_get_buf_len(syn_buf, result->lnum);
    }
    اگر (off != 0)
    {
	base = ml_get_buf(syn_buf, result->lnum, FALSE);
	p = base + col;
	اگر (off > 0)
	{
	    وقتی (off-- && *p != NUL)
		MB_PTR_ADV(p);
	}
	وگرنه اگر (off < 0)
	{
	    وقتی (off++ && base < p)
		MB_PTR_BACK(base, p);
	}
	col = (صحیح)(p - base);
    }
    result->col = col;
}

/*
 * Get current line in syntax buffer.
 */
    ایستا char_u *
syn_getcurline(تهی)
{
    بازگشت ml_get_buf(syn_buf, current_lnum, FALSE);
}

/*
 * Get length of current line in syntax buffer.
 */
    ایستا colnr_T
syn_getcurline_len(تهی)
{
    بازگشت ml_get_buf_len(syn_buf, current_lnum);
}

/*
 * Call vim_regexec() to find a match with "rmp" in "syn_buf".
 * Returns TRUE when there is a match.
 */
    ایستا صحیح
syn_regexec(
    regmmatch_T	*rmp,
    linenr_T	lnum,
    colnr_T	col,
    syn_time_T  *st UNUSED)
{
    صحیح		r;
    صحیح		timed_out = FALSE;
#ifdef FEAT_PROFILE
    proftime_T	pt;

    اگر (syn_time_on)
	profile_start(&pt);
#endif

    اگر (rmp->regprog == پوچ)
	// This can happen if a previous call to vim_regexec_multi() tried to
	// use the NFA engine, which resulted in NFA_TOO_EXPENSIVE, and
	// compiling the pattern with the other engine fails.
	بازگشت FALSE;

    rmp->rmm_maxcol = syn_buf->b_p_smc;
    r = vim_regexec_multi(rmp, syn_win, syn_buf, lnum, col, &timed_out);

#ifdef FEAT_PROFILE
    اگر (syn_time_on)
    {
	profile_end(&pt);
	profile_add(&st->total, &pt);
	اگر (profile_cmp(&pt, &st->slowest) < 0)
	    st->slowest = pt;
	++st->شمار;
	اگر (r > 0)
	    ++st->match;
    }
#endif
#ifdef FEAT_RELTIME
    اگر (timed_out && redrawtime_limit_set && !syn_win->w_s->b_syn_slow)
    {
	syn_win->w_s->b_syn_slow = TRUE;
	پیام(_("'redrawtime' exceeded, syntax highlighting disabled"));
    }
#endif

    اگر (r > 0)
    {
	rmp->startpos[0].lnum += lnum;
	rmp->endpos[0].lnum += lnum;
	بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * Check one position in a line for a matching keyword.
 * The caller must check if a keyword can start at startcol.
 * Return its ID if found, 0 otherwise.
 */
    ایستا صحیح
check_keyword_id(
    char_u	*سطر,
    صحیح		startcol,	// position in line to check for keyword
    صحیح		*endcolp,	// return: character after found keyword
    long	*flagsp,	// return: flags of matching keyword
    short	**next_listp,	// return: next_list of matching keyword
    stateitem_T	*cur_si,	// item at the top of the stack
    صحیح		*ccharp UNUSED)	// conceal substitution char
{
    keyentry_T	*kp;
    char_u	*kwp;
    صحیح		round;
    صحیح		kwlen;
    char_u	keyword[MAXKEYWLEN + 1]; // assume max. keyword len is 80
    hashtab_T	*ht;
    hashitem_T	*hi;

    // Find first character after the keyword.  First character was already
    // checked.
    kwp = سطر + startcol;
    kwlen = 0;
    انجامبده
    {
	اگر (has_mbyte)
	    kwlen += (*mb_ptr2len)(kwp + kwlen);
	وگرنه
	    ++kwlen;
    }
    وقتی (vim_iswordp_buf(kwp + kwlen, syn_buf));

    اگر (kwlen > MAXKEYWLEN)
	بازگشت 0;

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
    برای (round = 1; round <= 2; ++round)
    {
	ht = round == 1 ? &syn_block->b_keywtab : &syn_block->b_keywtab_ic;
	اگر (ht->ht_used == 0)
	    ادامه;
	اگر (round == 2)	// ignore case
	    (تهی)str_foldcase(kwp, kwlen, keyword, MAXKEYWLEN + 1);

	/*
	 * Find keywords that match.  There can be several with different
	 * attributes.
	 * When current_next_list is non-zero accept only that group, otherwise:
	 *  Accept a not-contained keyword at toplevel.
	 *  Accept a keyword at other levels only if it is in the contains list.
	 */
	hi = hash_find(ht, keyword);
	اگر (!HASHITEM_EMPTY(hi))
	    برای (kp = HI2KE(hi); kp != پوچ; kp = kp->ke_next)
	    {
		اگر (current_next_list != 0
			? in_id_list(پوچ, current_next_list, &kp->k_syn, 0)
			: (cur_si == پوچ
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
		    بازگشت kp->k_syn.id;
		}
	    }
    }
    بازگشت 0;
}

/*
 * Handle ":syntax conceal" command.
 */
    ایستا تهی
syn_cmd_conceal(exarg_T *eap UNUSED, صحیح syncing UNUSED)
{
#ifdef FEAT_CONCEAL
    char_u	*arg = eap->arg;
    char_u	*next;

    eap->nextcmd = find_nextcmd(arg);
    اگر (eap->skip)
	بازگشت;

    next = skiptowhite(arg);
    اگر (*arg == NUL)
    {
	اگر (curwin->w_s->b_syn_conceal)
	    پیام("syntax conceal on");
	وگرنه
	    پیام("syntax conceal off");
    }
    وگرنه اگر (STRNICMP(arg, "on", 2) == 0 && next - arg == 2)
	curwin->w_s->b_syn_conceal = TRUE;
    وگرنه اگر (STRNICMP(arg, "off", 3) == 0 && next - arg == 3)
	curwin->w_s->b_syn_conceal = FALSE;
    وگرنه
	semsg(_(e_illegal_argument_str_2), arg);
#endif
}

/*
 * Handle ":syntax case" command.
 */
    ایستا تهی
syn_cmd_case(exarg_T *eap, صحیح syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*next;

    eap->nextcmd = find_nextcmd(arg);
    اگر (eap->skip)
	بازگشت;

    next = skiptowhite(arg);
    اگر (*arg == NUL)
    {
	اگر (curwin->w_s->b_syn_ic)
	    پیام("syntax case ignore");
	وگرنه
	    پیام("syntax case match");
    }
    وگرنه اگر (STRNICMP(arg, "match", 5) == 0 && next - arg == 5)
	curwin->w_s->b_syn_ic = FALSE;
    وگرنه اگر (STRNICMP(arg, "ignore", 6) == 0 && next - arg == 6)
	curwin->w_s->b_syn_ic = TRUE;
    وگرنه
	semsg(_(e_illegal_argument_str_2), arg);
}

/*
 * Handle ":syntax foldlevel" command.
 */
    ایستا تهی
syn_cmd_foldlevel(exarg_T *eap, صحیح syncing UNUSED)
{
    char_u *arg = eap->arg;
    char_u *arg_end;

    eap->nextcmd = find_nextcmd(arg);
    اگر (eap->skip)
	بازگشت;

    اگر (*arg == NUL)
    {
	انتخاب (curwin->w_s->b_syn_foldlevel)
	{
	    حالت SYNFLD_START:   پیام("syntax foldlevel start");   بشکن;
	    حالت SYNFLD_MINIMUM: پیام("syntax foldlevel minimum"); بشکن;
	    پیشفرض: بشکن;
	}
	بازگشت;
    }

    arg_end = skiptowhite(arg);
    اگر (STRNICMP(arg, "start", 5) == 0 && arg_end - arg == 5)
	curwin->w_s->b_syn_foldlevel = SYNFLD_START;
    وگرنه اگر (STRNICMP(arg, "minimum", 7) == 0 && arg_end - arg == 7)
	curwin->w_s->b_syn_foldlevel = SYNFLD_MINIMUM;
    وگرنه
    {
	semsg(_(e_illegal_argument_str_2), arg);
	بازگشت;
    }

    arg = skipwhite(arg_end);
    اگر (*arg != NUL)
    {
	semsg(_(e_illegal_argument_str_2), arg);
    }
}

/*
 * Handle ":syntax spell" command.
 */
    ایستا تهی
syn_cmd_spell(exarg_T *eap, صحیح syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*next;

    eap->nextcmd = find_nextcmd(arg);
    اگر (eap->skip)
	بازگشت;

    next = skiptowhite(arg);
    اگر (*arg == NUL)
    {
	اگر (curwin->w_s->b_syn_spell == SYNSPL_بالاترین)
	    پیام("syntax spell toplevel");
	وگرنه اگر (curwin->w_s->b_syn_spell == SYNSPL_NOTOP)
	    پیام("syntax spell notoplevel");
	وگرنه
	    پیام("syntax spell default");
    }
    وگرنه اگر (STRNICMP(arg, "toplevel", 8) == 0 && next - arg == 8)
	curwin->w_s->b_syn_spell = SYNSPL_بالاترین;
    وگرنه اگر (STRNICMP(arg, "notoplevel", 10) == 0 && next - arg == 10)
	curwin->w_s->b_syn_spell = SYNSPL_NOTOP;
    وگرنه اگر (STRNICMP(arg, "default", 7) == 0 && next - arg == 7)
	curwin->w_s->b_syn_spell = SYNSPL_DEFAULT;
    وگرنه
    {
	semsg(_(e_illegal_argument_str_2), arg);
	بازگشت;
    }

    // assume spell checking changed, force a redraw
    redraw_win_later(curwin, UPD_NOT_VALID);
}

/*
 * Handle ":syntax iskeyword" command.
 */
    ایستا تهی
syn_cmd_iskeyword(exarg_T *eap, صحیح syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	save_chartab[32];
    char_u	*save_isk;

    اگر (eap->skip)
	بازگشت;

    arg = skipwhite(arg);
    اگر (*arg == NUL)
    {
	پیام_puts("\n");
	اگر (curwin->w_s->b_syn_isk != empty_option)
	{
	    پیام_puts("syntax iskeyword ");
	    پیام_outtrans(curwin->w_s->b_syn_isk);
	}
	وگرنه
	    پیام_outtrans((char_u *)_("syntax iskeyword not set"));
    }
    وگرنه
    {
	اگر (STRNICMP(arg, "clear", 5) == 0)
	{
	    mch_memmove(curwin->w_s->b_syn_chartab, curbuf->b_chartab,
								  (size_t)32);
	    clear_string_option(&curwin->w_s->b_syn_isk);
	}
	وگرنه
	{
	    اگر (check_isopt(arg) == FAIL)
	    {
		emsg(_(e_invalid_argument));
		بازگشت;
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
    تهی
syntax_clear(synblock_T *block)
{
    صحیح i;

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
    برای (i = block->b_syn_patterns.ga_len; --i >= 0; )
	syn_clear_pattern(block, i);
    ga_clear(&block->b_syn_patterns);

    // free the syntax clusters
    برای (i = block->b_syn_clusters.ga_len; --i >= 0; )
	syn_clear_cluster(block, i);
    ga_clear(&block->b_syn_clusters);
    block->b_spell_cluster_id = 0;
    block->b_nospell_cluster_id = 0;

    block->b_syn_sync_flags = 0;
    block->b_syn_sync_minlines = 0;
    block->b_syn_sync_maxlines = 0;
    block->b_syn_sync_linebreaks = 0;

    vim_regfree(block->b_syn_linecont_prog);
    block->b_syn_linecont_prog = پوچ;
    VIM_CLEAR(block->b_syn_linecont_pat);
#ifdef FEAT_FOLDING
    block->b_syn_folditems = 0;
#endif
    clear_string_option(&block->b_syn_isk);

    // free the stored states
    syn_پشته_free_all(block);
    invalidate_current_state();

    // Reset the counter for ":syn include"
    running_syn_inc_tag = 0;
}

/*
 * Get rid of ownsyntax for window "wp".
 */
    تهی
reset_synblock(win_T *wp)
{
    اگر (wp->w_s != &wp->w_buffer->b_s)
    {
	syntax_clear(wp->w_s);
	vim_free(wp->w_s);
	wp->w_s = &wp->w_buffer->b_s;
    }
}

/*
 * Clear syncing info for one buffer.
 */
    ایستا تهی
syntax_sync_clear(تهی)
{
    صحیح i;

    // free the syntax patterns
    برای (i = curwin->w_s->b_syn_patterns.ga_len; --i >= 0; )
	اگر (SYN_اقلام(curwin->w_s)[i].sp_syncing)
	    syn_remove_pattern(curwin->w_s, i);

    curwin->w_s->b_syn_sync_flags = 0;
    curwin->w_s->b_syn_sync_minlines = 0;
    curwin->w_s->b_syn_sync_maxlines = 0;
    curwin->w_s->b_syn_sync_linebreaks = 0;

    vim_regfree(curwin->w_s->b_syn_linecont_prog);
    curwin->w_s->b_syn_linecont_prog = پوچ;
    VIM_CLEAR(curwin->w_s->b_syn_linecont_pat);
    clear_string_option(&curwin->w_s->b_syn_isk);

    syn_پشته_free_all(curwin->w_s);	// Need to recompute all syntax.
}

/*
 * Remove one pattern from the buffer's pattern list.
 */
    ایستا تهی
syn_remove_pattern(
    synblock_T	*block,
    صحیح		idx)
{
    synpat_T	*spp;

    spp = &(SYN_اقلام(block)[idx]);
#ifdef FEAT_FOLDING
    اگر (spp->sp_flags & HL_FOLD)
	--block->b_syn_folditems;
#endif
    syn_clear_pattern(block, idx);
    mch_memmove(spp, spp + 1,
		   اندازه(synpat_T) * (block->b_syn_patterns.ga_len - idx - 1));
    --block->b_syn_patterns.ga_len;
}

/*
 * Clear and free one syntax pattern.  When clearing all, must be called from
 * last to first!
 */
    ایستا تهی
syn_clear_pattern(synblock_T *block, صحیح i)
{
    vim_free(SYN_اقلام(block)[i].sp_pattern);
    vim_regfree(SYN_اقلام(block)[i].sp_prog);
    // Only free sp_cont_list and sp_next_list of first start pattern
    اگر (i == 0 || SYN_اقلام(block)[i - 1].sp_type != SPTYPE_START)
    {
	vim_free(SYN_اقلام(block)[i].sp_cont_list);
	vim_free(SYN_اقلام(block)[i].sp_next_list);
	vim_free(SYN_اقلام(block)[i].sp_syn.cont_in_list);
    }
}

/*
 * Clear and free one syntax cluster.
 */
    ایستا تهی
syn_clear_cluster(synblock_T *block, صحیح i)
{
    vim_free(SYN_CLSTR(block)[i].scl_name);
    vim_free(SYN_CLSTR(block)[i].scl_name_u);
    vim_free(SYN_CLSTR(block)[i].scl_list);
}

/*
 * Handle ":syntax clear" command.
 */
    ایستا تهی
syn_cmd_clear(exarg_T *eap, صحیح syncing)
{
    char_u	*arg = eap->arg;
    char_u	*arg_end;
    صحیح		id;

    eap->nextcmd = find_nextcmd(arg);
    اگر (eap->skip)
	بازگشت;

    /*
     * We have to disable this within ":syn include @group filename",
     * because otherwise @group would get deleted.
     * Only required for Vim 5.x syntax files, 6.0 ones don't contain ":syn
     * clear".
     */
    اگر (curwin->w_s->b_syn_topgrp != 0)
	بازگشت;

    اگر (ends_excmd2(eap->cmd, arg))
    {
	/*
	 * No argument: Clear all syntax items.
	 */
	اگر (syncing)
	    syntax_sync_clear();
	وگرنه
	{
	    syntax_clear(curwin->w_s);
	    اگر (curwin->w_s == &curwin->w_buffer->b_s)
		do_unlet((char_u *)"b:current_syntax", TRUE);
	    do_unlet((char_u *)"w:current_syntax", TRUE);
	}
    }
    وگرنه
    {
	/*
	 * Clear the group IDs that are in the argument.
	 */
	وقتی (!ends_excmd2(eap->cmd, arg))
	{
	    arg_end = skiptowhite(arg);
	    اگر (*arg == '@')
	    {
		id = syn_scl_namen2id(arg + 1, (صحیح)(arg_end - arg - 1));
		اگر (id == 0)
		{
		    semsg(_(e_no_such_syntax_cluster_str_1), arg);
		    بشکن;
		}
		وگرنه
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
	    وگرنه
	    {
		id = syn_namen2id(arg, (صحیح)(arg_end - arg));
		اگر (id == 0)
		{
		    semsg(_(e_no_such_highlight_group_name_str), arg);
		    بشکن;
		}
		وگرنه
		    syn_clear_one(id, syncing);
	    }
	    arg = skipwhite(arg_end);
	}
    }
    redraw_curbuf_later(UPD_SOME_VALID);
    syn_پشته_free_all(curwin->w_s);		// Need to recompute all syntax.
}

/*
 * Clear one syntax group for the current buffer.
 */
    ایستا تهی
syn_clear_one(صحیح id, صحیح syncing)
{
    synpat_T	*spp;
    صحیح		idx;

    // Clear keywords only when not ":syn sync clear group-name"
    اگر (!syncing)
    {
	(تهی)syn_clear_keyword(id, &curwin->w_s->b_keywtab);
	(تهی)syn_clear_keyword(id, &curwin->w_s->b_keywtab_ic);
    }

    // clear the patterns for "id"
    برای (idx = curwin->w_s->b_syn_patterns.ga_len; --idx >= 0; )
    {
	spp = &(SYN_اقلام(curwin->w_s)[idx]);
	اگر (spp->sp_syn.id != id || spp->sp_syncing != syncing)
	    ادامه;
	syn_remove_pattern(curwin->w_s, idx);
    }
}

/*
 * Handle ":syntax on" command.
 */
    ایستا تهی
syn_cmd_on(exarg_T *eap, صحیح syncing UNUSED)
{
    syn_cmd_onoff(eap, "syntax");
}

/*
 * Handle ":syntax enable" command.
 */
    ایستا تهی
syn_cmd_enable(exarg_T *eap, صحیح syncing UNUSED)
{
    set_internal_string_var((char_u *)"g:syntax_cmd", (char_u *)"enable");
    syn_cmd_onoff(eap, "syntax");
    do_unlet((char_u *)"g:syntax_cmd", TRUE);
}

/*
 * Handle ":syntax reset" command.
 * It actually resets highlighting, not syntax.
 */
    ایستا تهی
syn_cmd_reset(exarg_T *eap, صحیح syncing UNUSED)
{
    set_nextcmd(eap, eap->arg);
    اگر (!eap->skip)
    {
	set_internal_string_var((char_u *)"g:syntax_cmd", (char_u *)"reset");
	do_cmdline_cmd((char_u *)"runtime! syntax/syncolor.vim");
	do_unlet((char_u *)"g:syntax_cmd", TRUE);
    }
}

/*
 * Handle ":syntax manual" command.
 */
    ایستا تهی
syn_cmd_manual(exarg_T *eap, صحیح syncing UNUSED)
{
    syn_cmd_onoff(eap, "manual");
}

/*
 * Handle ":syntax off" command.
 */
    ایستا تهی
syn_cmd_off(exarg_T *eap, صحیح syncing UNUSED)
{
    syn_cmd_onoff(eap, "nosyntax");
}

    ایستا تهی
syn_cmd_onoff(exarg_T *eap, نویسه *name)
{
    char_u	buf[100];

    set_nextcmd(eap, eap->arg);
    اگر (!eap->skip)
    {
	STRCPY(buf, "so ");
	vim_snprintf((نویسه *)buf + 3, اندازه(buf) - 3, SYNTAX_FNAME, name);
	do_cmdline_cmd(buf);
    }
}

/*
 * Handle ":syntax [list]" command: list current syntax words.
 */
    ایستا تهی
syn_cmd_list(
    exarg_T	*eap,
    صحیح		syncing)	    // when TRUE: list syncing items
{
    char_u	*arg = eap->arg;
    صحیح		id;
    char_u	*arg_end;

    eap->nextcmd = find_nextcmd(arg);
    اگر (eap->skip)
	بازگشت;

    اگر (!syntax_present(curwin))
    {
	پیام(_(پیام_no_اقلام));
	بازگشت;
    }

    اگر (syncing)
    {
	اگر (curwin->w_s->b_syn_sync_flags & SF_CCOMMENT)
	{
	    پیام_puts(_("syncing on C-style comments"));
	    syn_سطرها_پیام();
	    syn_match_پیام();
	}
	وگرنه اگر (curwin->w_s->b_syn_sync_flags & SF_MATCH)
	{
	    پیام_puts_title(_("\n--- Syntax sync items ---"));
	    اگر (curwin->w_s->b_syn_sync_minlines > 0
		|| curwin->w_s->b_syn_sync_maxlines > 0
		|| curwin->w_s->b_syn_sync_linebreaks > 0)
	    {
		پیام_puts(_("\nsyncing on items"));
		syn_سطرها_پیام();
		syn_match_پیام();
	    }
	    برای (id = 1; id <= highlight_رقم_groups() && !got_int; ++id)
		syn_list_one(id, syncing, FALSE);
	}
	وگرنه
	{
	    اگر (curwin->w_s->b_syn_sync_minlines == 0)
		پیام_puts(_("no syncing"));
	    وگرنه
	    {
		اگر (curwin->w_s->b_syn_sync_minlines == MAXLNUM)
		    پیام_puts(_("syncing starts at the first line"));
		وگرنه
		{
		    پیام_puts(_("syncing starts "));
		    پیام_outnum(curwin->w_s->b_syn_sync_minlines);
		    پیام_puts(_(" lines before top line"));
		}
		syn_match_پیام();
	    }
	}
	بازگشت;
    }

    پیام_puts_title(_("\n--- Syntax items ---"));
    اگر (ends_excmd2(eap->cmd, arg))
    {
	/*
	 * No argument: List all group IDs and all syntax clusters.
	 */
	برای (id = 1; id <= highlight_رقم_groups() && !got_int; ++id)
	    syn_list_one(id, syncing, FALSE);
	برای (id = 0; id < curwin->w_s->b_syn_clusters.ga_len && !got_int; ++id)
	    syn_list_cluster(id);
    }
    وگرنه
    {
	/*
	 * List the group IDs and syntax clusters that are in the argument.
	 */
	وقتی (!ends_excmd2(eap->cmd, arg) && !got_int)
	{
	    arg_end = skiptowhite(arg);
	    اگر (*arg == '@')
	    {
		id = syn_scl_namen2id(arg + 1, (صحیح)(arg_end - arg - 1));
		اگر (id == 0)
		    semsg(_(e_no_such_syntax_cluster_str_2), arg);
		وگرنه
		    syn_list_cluster(id - SYNID_CLUSTER);
	    }
	    وگرنه
	    {
		id = syn_namen2id(arg, (صحیح)(arg_end - arg));
		اگر (id == 0)
		    semsg(_(e_no_such_highlight_group_name_str), arg);
		وگرنه
		    syn_list_one(id, syncing, TRUE);
	    }
	    arg = skipwhite(arg_end);
	}
    }
    set_nextcmd(eap, arg);
}

    ایستا تهی
syn_سطرها_پیام(تهی)
{
    اگر (curwin->w_s->b_syn_sync_maxlines > 0
				      || curwin->w_s->b_syn_sync_minlines > 0)
    {
	پیام_puts("; ");
	اگر (curwin->w_s->b_syn_sync_minlines == MAXLNUM)
	    پیام_puts(_("from the first line"));
	وگرنه
	{
	    اگر (curwin->w_s->b_syn_sync_minlines > 0)
	    {
		پیام_puts(_("minimal "));
		پیام_outnum(curwin->w_s->b_syn_sync_minlines);
		اگر (curwin->w_s->b_syn_sync_maxlines)
		    پیام_puts(", ");
	    }
	    اگر (curwin->w_s->b_syn_sync_maxlines > 0)
	    {
		پیام_puts(_("maximal "));
		پیام_outnum(curwin->w_s->b_syn_sync_maxlines);
	    }
	    پیام_puts(_(" lines before top line"));
	}
    }
}

    ایستا تهی
syn_match_پیام(تهی)
{
    اگر (curwin->w_s->b_syn_sync_linebreaks > 0)
    {
	پیام_puts(_("; match "));
	پیام_outnum(curwin->w_s->b_syn_sync_linebreaks);
	پیام_puts(_(" line breaks"));
    }
}

ایستا صحیح  last_matchgroup;

ایستا تهی syn_list_flags(keyvalue_T *nlist, صحیح nr_entries, صحیح flags, صحیح attr);

/*
 * List one syntax item, for ":syntax" or "syntax list syntax_name".
 */
    ایستا تهی
syn_list_one(
    صحیح		id,
    صحیح		syncing,	    // when TRUE: list syncing items
    صحیح		link_only)	    // when TRUE; list link-only too
{
    صحیح		attr;
    صحیح		idx;
    صحیح		did_header = FALSE;
    synpat_T	*spp;
    ایستا keyvalue_T namelist1[] =
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
    ایستا keyvalue_T namelist2[] =
		{
		    KEYVALUE_ENTRY(HL_SKIPWHITE, "skipwhite"),
		    KEYVALUE_ENTRY(HL_SKIPNL, "skipnl"),
		    KEYVALUE_ENTRY(HL_SKIPEMPTY, "skipempty")
		};

    attr = HL_ATTR(HLF_D);		// highlight like directories

    // list the keywords for "id"
    اگر (!syncing)
    {
	did_header = syn_list_keywords(id, &curwin->w_s->b_keywtab, FALSE, attr);
	did_header = syn_list_keywords(id, &curwin->w_s->b_keywtab_ic,
							    did_header, attr);
    }

    // list the patterns for "id"
    برای (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len && !got_int; ++idx)
    {
	spp = &(SYN_اقلام(curwin->w_s)[idx]);
	اگر (spp->sp_syn.id != id || spp->sp_syncing != syncing)
	    ادامه;

	(تهی)syn_list_header(did_header, 999, id);
	did_header = TRUE;
	last_matchgroup = 0;
	اگر (spp->sp_type == SPTYPE_MATCH)
	{
	    put_pattern("match", ' ', spp, attr);
	    پیام_putchar(' ');
	}
	وگرنه اگر (spp->sp_type == SPTYPE_START)
	{
	    وقتی (SYN_اقلام(curwin->w_s)[idx].sp_type == SPTYPE_START)
		put_pattern("start", '=', &SYN_اقلام(curwin->w_s)[idx++], attr);
	    اگر (SYN_اقلام(curwin->w_s)[idx].sp_type == SPTYPE_SKIP)
		put_pattern("skip", '=', &SYN_اقلام(curwin->w_s)[idx++], attr);
	    وقتی (idx < curwin->w_s->b_syn_patterns.ga_len
			  && SYN_اقلام(curwin->w_s)[idx].sp_type == SPTYPE_END)
		put_pattern("end", '=', &SYN_اقلام(curwin->w_s)[idx++], attr);
	    --idx;
	    پیام_putchar(' ');
	}
	syn_list_flags(namelist1, (صحیح)ARRAY_LENGTH(namelist1), spp->sp_flags, attr);

	اگر (spp->sp_cont_list != پوچ)
	    put_id_list((char_u *)"contains", spp->sp_cont_list, attr);

	اگر (spp->sp_syn.cont_in_list != پوچ)
	    put_id_list((char_u *)"containedin",
					      spp->sp_syn.cont_in_list, attr);

	اگر (spp->sp_next_list != پوچ)
	{
	    put_id_list((char_u *)"nextgroup", spp->sp_next_list, attr);
	    syn_list_flags(namelist2, (صحیح)ARRAY_LENGTH(namelist2), spp->sp_flags, attr);
	}
	اگر (spp->sp_flags & (HL_SYNC_HERE|HL_SYNC_THERE))
	{
	    اگر (spp->sp_flags & HL_SYNC_HERE)
		پیام_puts_attr("grouphere", attr);
	    وگرنه
		پیام_puts_attr("groupthere", attr);
	    پیام_putchar(' ');
	    اگر (spp->sp_sync_idx >= 0)
		پیام_outtrans(highlight_group_name(SYN_اقلام(curwin->w_s)
				   [spp->sp_sync_idx].sp_syn.id - 1));
	    وگرنه
		پیام_puts("NONE");
	    پیام_putchar(' ');
	}
    }

    // list the link, if there is one
    اگر (highlight_link_id(id - 1) && (did_header || link_only) && !got_int)
    {
	(تهی)syn_list_header(did_header, 999, id);
	پیام_puts_attr("links to", attr);
	پیام_putchar(' ');
	پیام_outtrans(highlight_group_name(highlight_link_id(id - 1) - 1));
    }
}

    ایستا تهی
syn_list_flags(keyvalue_T *nlist, صحیح nr_entries, صحیح flags, صحیح attr)
{
    صحیح		i;

    برای (i = 0; i < nr_entries; ++i)
	اگر (flags & nlist[i].کلید)
	{
	    پیام_puts_attr((نویسه *)nlist[i].مقدار.string, attr);
	    پیام_putchar(' ');
	}
}

/*
 * List one syntax cluster, for ":syntax" or "syntax list syntax_name".
 */
    ایستا تهی
syn_list_cluster(صحیح id)
{
    صحیح	    endcol = 15;

    // slight hack:  roughly duplicate the guts of syn_list_header()
    پیام_putchar('\n');
    پیام_outtrans(SYN_CLSTR(curwin->w_s)[id].scl_name);

    اگر (پیام_col >= endcol)	// output at least one space
	endcol = پیام_col + 1;
    اگر (cmdline_width <= endcol)	// avoid hang for tiny window
	endcol = cmdline_width - 1;

    پیام_advance(endcol);
    اگر (SYN_CLSTR(curwin->w_s)[id].scl_list != پوچ)
    {
	put_id_list((char_u *)"cluster", SYN_CLSTR(curwin->w_s)[id].scl_list,
		    HL_ATTR(HLF_D));
    }
    وگرنه
    {
	پیام_puts_attr("cluster", HL_ATTR(HLF_D));
	پیام_puts("=NONE");
    }
}

    ایستا تهی
put_id_list(char_u *name, short *list, صحیح attr)
{
    short		*p;

    پیام_puts_attr((نویسه *)name, attr);
    پیام_putchar('=');
    برای (p = list; *p; ++p)
    {
	اگر (*p >= SYNID_ALLBUT && *p < SYNID_بالاترین)
	{
	    اگر (p[1])
		پیام_puts("ALLBUT");
	    وگرنه
		پیام_puts("ALL");
	}
	وگرنه اگر (*p >= SYNID_بالاترین && *p < SYNID_CONTAINED)
	{
	    پیام_puts("TOP");
	}
	وگرنه اگر (*p >= SYNID_CONTAINED && *p < SYNID_CLUSTER)
	{
	    پیام_puts("CONTAINED");
	}
	وگرنه اگر (*p >= SYNID_CLUSTER)
	{
	    short scl_id = *p - SYNID_CLUSTER;

	    پیام_putchar('@');
	    پیام_outtrans(SYN_CLSTR(curwin->w_s)[scl_id].scl_name);
	}
	وگرنه
	    پیام_outtrans(highlight_group_name(*p - 1));
	اگر (p[1])
	    پیام_putchar(',');
    }
    پیام_putchar(' ');
}

    ایستا تهی
put_pattern(
    نویسه	*s,
    صحیح		c,
    synpat_T	*spp,
    صحیح		attr)
{
    long	n;
    صحیح		mask;
    صحیح		first;
    ایستا نویسه	*sepchars = "/+=-#@\"|'^&";
    صحیح		i;

    // May have to write "matchgroup=group"
    اگر (last_matchgroup != spp->sp_syn_match_id)
    {
	last_matchgroup = spp->sp_syn_match_id;
	پیام_puts_attr("matchgroup", attr);
	پیام_putchar('=');
	اگر (last_matchgroup == 0)
	    پیام_outtrans((char_u *)"NONE");
	وگرنه
	    پیام_outtrans(highlight_group_name(last_matchgroup - 1));
	پیام_putchar(' ');
    }

    // output the name of the pattern and an '=' or ' '
    پیام_puts_attr(s, attr);
    پیام_putchar(c);

    // output the pattern, in between a char that is not in the pattern
    برای (i = 0; vim_strchr(spp->sp_pattern, sepchars[i]) != پوچ; )
	اگر (sepchars[++i] == NUL)
	{
	    i = 0;	// no good char found, just use the first one
	    بشکن;
	}
    پیام_putchar(sepchars[i]);
    پیام_outtrans(spp->sp_pattern);
    پیام_putchar(sepchars[i]);

    // output any pattern options
    first = TRUE;
    برای (i = 0; i < SPO_شمار; ++i)
    {
	mask = (1 << i);
	اگر (spp->sp_off_flags & (mask + (mask << SPO_شمار)))
	{
	    اگر (!first)
		پیام_putchar(',');	// separate with commas
	    پیام_puts(spo_name_tab[i]);
	    n = spp->sp_offsets[i];
	    اگر (i != SPO_LC_OFF)
	    {
		اگر (spp->sp_off_flags & mask)
		    پیام_putchar('s');
		وگرنه
		    پیام_putchar('e');
		اگر (n > 0)
		    پیام_putchar('+');
	    }
	    اگر (n || i == SPO_LC_OFF)
		پیام_outnum(n);
	    first = FALSE;
	}
    }
    پیام_putchar(' ');
}

/*
 * List or clear the keywords for one syntax group.
 * Return TRUE if the header has been printed.
 */
    ایستا صحیح
syn_list_keywords(
    صحیح		id,
    hashtab_T	*ht,
    صحیح		did_header,		// header has already been printed
    صحیح		attr)
{
    صحیح		outlen;
    hashitem_T	*hi;
    keyentry_T	*kp;
    صحیح		todo;
    صحیح		prev_contained = 0;
    short	*prev_next_list = پوچ;
    short	*prev_cont_in_list = پوچ;
    صحیح		prev_skipnl = 0;
    صحیح		prev_skipwhite = 0;
    صحیح		prev_skipempty = 0;

    /*
     * Unfortunately, this list of keywords is not sorted on alphabet but on
     * hash value...
     */
    todo = (صحیح)ht->ht_used;
    برای (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	اگر (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    برای (kp = HI2KE(hi); kp != پوچ && !got_int; kp = kp->ke_next)
	    {
		اگر (kp->k_syn.id == id)
		{
		    اگر (prev_contained != (kp->flags & HL_CONTAINED)
			    || prev_skipnl != (kp->flags & HL_SKIPNL)
			    || prev_skipwhite != (kp->flags & HL_SKIPWHITE)
			    || prev_skipempty != (kp->flags & HL_SKIPEMPTY)
			    || prev_cont_in_list != kp->k_syn.cont_in_list
			    || prev_next_list != kp->next_list)
			outlen = 9999;
		    وگرنه
			outlen = (صحیح)STRLEN(kp->keyword);
		    // output "contained" and "nextgroup" on each line
		    اگر (syn_list_header(did_header, outlen, id))
		    {
			prev_contained = 0;
			prev_next_list = پوچ;
			prev_cont_in_list = پوچ;
			prev_skipnl = 0;
			prev_skipwhite = 0;
			prev_skipempty = 0;
		    }
		    did_header = TRUE;
		    اگر (prev_contained != (kp->flags & HL_CONTAINED))
		    {
			پیام_puts_attr("contained", attr);
			پیام_putchar(' ');
			prev_contained = (kp->flags & HL_CONTAINED);
		    }
		    اگر (kp->k_syn.cont_in_list != prev_cont_in_list)
		    {
			put_id_list((char_u *)"containedin",
						kp->k_syn.cont_in_list, attr);
			پیام_putchar(' ');
			prev_cont_in_list = kp->k_syn.cont_in_list;
		    }
		    اگر (kp->next_list != prev_next_list)
		    {
			put_id_list((char_u *)"nextgroup", kp->next_list, attr);
			پیام_putchar(' ');
			prev_next_list = kp->next_list;
			اگر (kp->flags & HL_SKIPNL)
			{
			    پیام_puts_attr("skipnl", attr);
			    پیام_putchar(' ');
			    prev_skipnl = (kp->flags & HL_SKIPNL);
			}
			اگر (kp->flags & HL_SKIPWHITE)
			{
			    پیام_puts_attr("skipwhite", attr);
			    پیام_putchar(' ');
			    prev_skipwhite = (kp->flags & HL_SKIPWHITE);
			}
			اگر (kp->flags & HL_SKIPEMPTY)
			{
			    پیام_puts_attr("skipempty", attr);
			    پیام_putchar(' ');
			    prev_skipempty = (kp->flags & HL_SKIPEMPTY);
			}
		    }
		    پیام_outtrans(kp->keyword);
		}
	    }
	}
    }

    بازگشت did_header;
}

    ایستا تهی
syn_clear_keyword(صحیح id, hashtab_T *ht)
{
    hashitem_T	*hi;
    keyentry_T	*kp;
    keyentry_T	*kp_prev;
    keyentry_T	*kp_next;
    صحیح		todo;

    hash_lock(ht);
    todo = (صحیح)ht->ht_used;
    FOR_ALL_HASHTAB_اقلام(ht, hi, todo)
    {
	اگر (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    kp_prev = پوچ;
	    برای (kp = HI2KE(hi); kp != پوچ; )
	    {
		اگر (kp->k_syn.id == id)
		{
		    kp_next = kp->ke_next;
		    اگر (kp_prev == پوچ)
		    {
			اگر (kp_next == پوچ)
			    hash_remove(ht, hi, "syntax clear keyword");
			وگرنه
			    hi->hi_کلید = KE2HIKEY(kp_next);
		    }
		    وگرنه
			kp_prev->ke_next = kp_next;
		    vim_free(kp->next_list);
		    vim_free(kp->k_syn.cont_in_list);
		    vim_free(kp);
		    kp = kp_next;
		}
		وگرنه
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
    ایستا تهی
clear_keywtab(hashtab_T *ht)
{
    hashitem_T	*hi;
    صحیح		todo;
    keyentry_T	*kp;
    keyentry_T	*kp_next;

    todo = (صحیح)ht->ht_used;
    FOR_ALL_HASHTAB_اقلام(ht, hi, todo)
    {
	اگر (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    برای (kp = HI2KE(hi); kp != پوچ; kp = kp_next)
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
    ایستا تهی
add_keyword(
    char_u	*name,	    // name of keyword
    size_t	namelen,    // length of keyword (excluding the NUL)
    صحیح		id,	    // group ID for this keyword
    صحیح		flags,	    // flags for this keyword
    short	*cont_in_list, // containedin for this keyword
    short	*next_list, // nextgroup for this keyword
    صحیح		conceal_char)
{
    keyentry_T	*kp;
    hashtab_T	*ht;
    hashitem_T	*hi;
    char_u	*name_ic;
    size_t	name_iclen;
    long_u	hash;
    char_u	name_folded[MAXKEYWLEN + 1];

    اگر (curwin->w_s->b_syn_ic)
    {
	name_ic = str_foldcase(name, (صحیح)namelen,
						 name_folded, MAXKEYWLEN + 1);
	name_iclen = STRLEN(name_ic);
    }
    وگرنه
    {
	name_ic = name;
	name_iclen = namelen;
    }
    kp = alloc(offsetof(keyentry_T, keyword) + name_iclen + 1);
    اگر (kp == پوچ)
	بازگشت;
    STRCPY(kp->keyword, name_ic);
    kp->k_syn.id = id;
    kp->k_syn.inc_tag = current_syn_inc_tag;
    kp->flags = flags;
    kp->k_char = conceal_char;
    kp->k_syn.cont_in_list = copy_id_list(cont_in_list);
    اگر (cont_in_list != پوچ)
	curwin->w_s->b_syn_containedin = TRUE;
    kp->next_list = copy_id_list(next_list);

    اگر (curwin->w_s->b_syn_ic)
	ht = &curwin->w_s->b_keywtab_ic;
    وگرنه
	ht = &curwin->w_s->b_keywtab;

    hash = hash_hash(kp->keyword);
    hi = hash_lookup(ht, kp->keyword, hash);
    اگر (HASHITEM_EMPTY(hi))
    {
	// new keyword, add to hashtable
	kp->ke_next = پوچ;
	hash_add_item(ht, hi, kp->keyword, hash);
    }
    وگرنه
    {
	// keyword already exists, prepend to list
	kp->ke_next = HI2KE(hi);
	hi->hi_کلید = KE2HIKEY(kp);
    }
}

/*
 * Get the start and end of the group name argument.
 * Return a pointer to the first argument.
 * Return NULL if the end of the command was found instead of further args.
 */
    ایستا char_u *
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
    اگر (ends_excmd(*arg) || *rest == NUL)
	بازگشت پوچ;
    بازگشت rest;
}

/*
 * Check for syntax command option arguments.
 * This can be called at any place in the list of arguments, and just picks
 * out the arguments that are known.  Can be called several times in a row to
 * collect all options in between other arguments.
 * Return a pointer to the next argument (which isn't an option).
 * Return NULL for any error;
 */
    ایستا char_u *
get_syn_options(
    char_u	    *start,		// next argument to be checked
    syn_opt_arg_T   *opt,		// various things
    صحیح		    *conceal_char UNUSED,
    صحیح		    skip)		// TRUE if skipping over command
{
    char_u	*arg = start;
    char_u	*gname_start, *gname;
    صحیح		syn_id;
    صحیح		len;
    نویسه	*p;
    صحیح		i;
    صحیح		fidx;
    ایستا ساختار flag
    {
	نویسه	*name;
	صحیح	argtype;
	صحیح	flags;
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
    ایستا نویسه *first_letters = "cCoOkKeEtTsSgGdDfFnN";

    اگر (arg == پوچ)		// already detected error
	بازگشت پوچ;

#ifdef FEAT_CONCEAL
    اگر (curwin->w_s->b_syn_conceal)
	opt->flags |= HL_CONCEAL;
#endif

    برای (;;)
    {
	/*
	 * This is used very often when a large number of keywords is defined.
	 * Need to skip quickly when no option name is found.
	 * Also avoid tolower(), it's slow.
	 */
	اگر (strchr(first_letters, *arg) == پوچ)
	    بشکن;

	برای (fidx = ARRAY_LENGTH(flagtab); --fidx >= 0; )
	{
	    p = flagtab[fidx].name;
	    برای (i = 0, len = 0; p[i] != NUL; i += 2, ++len)
		اگر (arg[len] != p[i] && arg[len] != p[i + 1])
		    بشکن;
	    اگر (p[i] == NUL && (VIM_ISWHITE(arg[len])
				    || (flagtab[fidx].argtype > 0
					 ? arg[len] == '='
					 : ends_excmd2(start, arg + len))))
	    {
		اگر (opt->keyword
			&& (flagtab[fidx].flags == HL_DISPLAY
			    || flagtab[fidx].flags == HL_FOLD
			    || flagtab[fidx].flags == HL_EXTEND))
		    // treat "display", "fold" and "extend" as a keyword
		    fidx = -1;
		بشکن;
	    }
	}
	اگر (fidx < 0)	    // no match found
	    بشکن;

	اگر (flagtab[fidx].argtype == 1)
	{
	    اگر (!opt->has_cont_list)
	    {
		emsg(_(e_contains_argument_not_accepted_here));
		بازگشت پوچ;
	    }
	    اگر (get_id_list(&arg, 8, &opt->cont_list, skip) == FAIL)
		بازگشت پوچ;
	}
	وگرنه اگر (flagtab[fidx].argtype == 2)
	{
	    اگر (get_id_list(&arg, 11, &opt->cont_in_list, skip) == FAIL)
		بازگشت پوچ;
	}
	وگرنه اگر (flagtab[fidx].argtype == 3)
	{
	    اگر (get_id_list(&arg, 9, &opt->next_list, skip) == FAIL)
		بازگشت پوچ;
	}
	وگرنه اگر (flagtab[fidx].argtype == 11 && arg[5] == '=')
	{
	    // cchar=?
	    اگر (has_mbyte)
	    {
#ifdef FEAT_CONCEAL
		*conceal_char = mb_ptr2char(arg + 6);
#endif
		arg += mb_ptr2len(arg + 6) - 1;
	    }
	    وگرنه
	    {
#ifdef FEAT_CONCEAL
		*conceal_char = arg[6];
#وگرنه
		;
#endif
	    }
#ifdef FEAT_CONCEAL
	    اگر (!vim_isprintc_strict(*conceal_char))
	    {
		emsg(_(e_invalid_cchar_مقدار));
		بازگشت پوچ;
	    }
#endif
	    arg = skipwhite(arg + 7);
	}
	وگرنه
	{
	    opt->flags |= flagtab[fidx].flags;
	    arg = skipwhite(arg + len);

	    اگر (flagtab[fidx].flags == HL_SYNC_HERE
		    || flagtab[fidx].flags == HL_SYNC_THERE)
	    {
		اگر (opt->sync_idx == پوچ)
		{
		    emsg(_(e_groupthere_not_accepted_here));
		    بازگشت پوچ;
		}
		gname_start = arg;
		arg = skiptowhite(arg);
		اگر (gname_start == arg)
		    بازگشت پوچ;
		gname = vim_strnsave(gname_start, arg - gname_start);
		اگر (gname == پوچ)
		    بازگشت پوچ;
		اگر (STRCMP(gname, "NONE") == 0)
		    *opt->sync_idx = NONE_IDX;
		وگرنه
		{
		    syn_id = syn_name2id(gname);
		    برای (i = curwin->w_s->b_syn_patterns.ga_len; --i >= 0; )
			اگر (SYN_اقلام(curwin->w_s)[i].sp_syn.id == syn_id
			      && SYN_اقلام(curwin->w_s)[i].sp_type
							       == SPTYPE_START)
			{
			    *opt->sync_idx = i;
			    بشکن;
			}
		    اگر (i < 0)
		    {
			semsg(_(e_didnt_find_region_item_for_str), gname);
			vim_free(gname);
			بازگشت پوچ;
		    }
		}

		vim_free(gname);
		arg = skipwhite(arg);
	    }
#ifdef FEAT_FOLDING
	    وگرنه اگر (flagtab[fidx].flags == HL_FOLD
						&& foldmethodIsSyntax(curwin))
		// Need to update folds later.
		foldUpdateAll(curwin);
#endif
	}
    }

    بازگشت arg;
}

/*
 * Adjustments to syntax item when declared in a ":syn include"'d file.
 * Set the contained flag, and if the item is not already contained, add it
 * to the specified top-level group, if any.
 */
    ایستا تهی
syn_incl_toplevel(صحیح id, صحیح *flagsp)
{
    اگر ((*flagsp & HL_CONTAINED) || curwin->w_s->b_syn_topgrp == 0)
	بازگشت;
    *flagsp |= HL_CONTAINED | HL_INCLUDED_TOPLEVEL;
    اگر (curwin->w_s->b_syn_topgrp >= SYNID_CLUSTER)
    {
	// We have to alloc this, because syn_combine_list() will free it.
	short	    *grp_list = ALLOC_MULT(short, 2);
	صحیح	    tlg_id = curwin->w_s->b_syn_topgrp - SYNID_CLUSTER;

	اگر (grp_list != پوچ)
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
    ایستا تهی
syn_cmd_include(exarg_T *eap, صحیح syncing UNUSED)
{
    char_u	*arg = eap->arg;
    صحیح		sgl_id = 1;
    char_u	*group_name_end;
    char_u	*rest;
    نویسه	*errormsg = پوچ;
    صحیح		prev_toplvl_grp;
    صحیح		prev_syn_inc_tag;
    صحیح		source = FALSE;

    eap->nextcmd = find_nextcmd(arg);
    اگر (eap->skip)
	بازگشت;

    اگر (arg[0] == '@')
    {
	++arg;
	rest = get_group_name(arg, &group_name_end);
	اگر (rest == پوچ)
	{
	    emsg(_(e_filename_required));
	    بازگشت;
	}
	sgl_id = syn_check_cluster(arg, (صحیح)(group_name_end - arg));
	اگر (sgl_id == 0)
	    بازگشت;
	// separate_nextcmd() and expand_filename() depend on this
	eap->arg = rest;
    }

    /*
     * Everything that's left, up to the next command, should be the
     * filename to include.
     */
    eap->argt |= (EX_XFILE | EX_NOSPC);
    separate_nextcmd(eap, FALSE);
    اگر (*eap->arg == '<' || *eap->arg == '$' || mch_isFullName(eap->arg))
    {
	// For an absolute path, "$VIM/..." or "<sfile>.." we ":source" the
	// file.  Need to expand the file name first.  In other cases
	// ":runtime!" is used.
	source = TRUE;
	اگر (expand_filename(eap, syn_cmdlinep, &errormsg) == FAIL)
	{
	    اگر (errormsg != پوچ)
		emsg(errormsg);
	    بازگشت;
	}
    }

    /*
     * Save and restore the existing top-level grouplist id and ":syn
     * include" tag around the actual inclusion.
     */
    اگر (running_syn_inc_tag >= MAX_SYN_INC_TAG)
    {
	emsg(_(e_too_many_syntax_includes));
	بازگشت;
    }
    prev_syn_inc_tag = current_syn_inc_tag;
    current_syn_inc_tag = ++running_syn_inc_tag;
    prev_toplvl_grp = curwin->w_s->b_syn_topgrp;
    curwin->w_s->b_syn_topgrp = sgl_id;
    اگر (source ? do_source(eap->arg, FALSE, DOSO_NONE, پوچ) == FAIL
				: source_runtime(eap->arg, DIP_ALL) == FAIL)
	semsg(_(e_cant_open_file_str), eap->arg);
    curwin->w_s->b_syn_topgrp = prev_toplvl_grp;
    current_syn_inc_tag = prev_syn_inc_tag;
}

/*
 * Handle ":syntax keyword {group-name} [{option}] keyword .." command.
 */
    ایستا تهی
syn_cmd_keyword(exarg_T *eap, صحیح syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*group_name_end;
    صحیح		syn_id;
    char_u	*rest;
    char_u	*keyword_copy = پوچ;
    char_u	*p;
    char_u	*kw;
    syn_opt_arg_T syn_opt_arg;
    صحیح		cnt;
    صحیح		conceal_char = NUL;

    rest = get_group_name(arg, &group_name_end);

    اگر (rest != پوچ)
    {
	اگر (eap->skip)
	    syn_id = -1;
	وگرنه
	    syn_id = syn_check_group(arg, (صحیح)(group_name_end - arg));
	اگر (syn_id != 0)
	    // allocate a buffer, for removing backslashes in the keyword
	    keyword_copy = alloc(STRLEN(rest) + 1);
	اگر (keyword_copy != پوچ)
	{
	    syn_opt_arg.flags = 0;
	    syn_opt_arg.keyword = TRUE;
	    syn_opt_arg.sync_idx = پوچ;
	    syn_opt_arg.has_cont_list = FALSE;
	    syn_opt_arg.cont_in_list = پوچ;
	    syn_opt_arg.next_list = پوچ;

	    /*
	     * The options given apply to ALL keywords, so all options must be
	     * found before keywords can be created.
	     * 1: collect the options and copy the keywords to keyword_copy.
	     */
	    cnt = 0;
	    p = keyword_copy;
	    برای ( ; rest != پوچ && !ends_excmd2(eap->arg, rest);
							rest = skipwhite(rest))
	    {
		rest = get_syn_options(rest, &syn_opt_arg, &conceal_char,
								    eap->skip);
		اگر (rest == پوچ || ends_excmd2(eap->arg, rest))
		    بشکن;
		// Copy the keyword, removing backslashes, and add a NUL.
		وقتی (*rest != NUL && !VIM_ISWHITE(*rest))
		{
		    اگر (*rest == '\\' && rest[1] != NUL)
			++rest;
		    *p++ = *rest++;
		}
		*p++ = NUL;
		++cnt;
	    }

	    اگر (!eap->skip)
	    {
		size_t	kwlen = 0;

		// Adjust flags for use of ":syn include".
		syn_incl_toplevel(syn_id, &syn_opt_arg.flags);

		/*
		 * 2: Add an entry for each keyword.
		 */
		برای (kw = keyword_copy; --cnt >= 0; kw += kwlen + 1)
		{
		    برای (p = vim_strchr(kw, '['); ; )
		    {
			اگر (p == پوچ)
			    kwlen = STRLEN(kw);
			وگرنه
			{
			    *p = NUL;
			    kwlen = (size_t)(p - kw);
			}
			add_keyword(kw, kwlen, syn_id, syn_opt_arg.flags,
				syn_opt_arg.cont_in_list,
					 syn_opt_arg.next_list, conceal_char);
			اگر (p == پوچ)
			    بشکن;
			اگر (p[1] == NUL)
			{
			    semsg(_(e_error_missing_rsb_str), kw);
			    برو error;
			}
			اگر (p[1] == ']')
			{
			    اگر (p[2] != NUL)
			    {
				semsg(_(e_trailing_char_after_rsb_str_str),
								    kw, &p[2]);
				برو error;
			    }
			    kw = p + 1;		// skip over the "]"
			    kwlen = 1;
			    بشکن;
			}
			اگر (has_mbyte)
			{
			    صحیح l = (*mb_ptr2len)(p + 1);

			    mch_memmove(p, p + 1, l);
			    p += l;
			}
			وگرنه
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

    اگر (rest != پوچ)
	set_nextcmd(eap, rest);
    وگرنه
	semsg(_(e_invalid_argument_str), arg);

    redraw_curbuf_later(UPD_SOME_VALID);
    syn_پشته_free_all(curwin->w_s);		// Need to recompute all syntax.
}

/*
 * Handle ":syntax match {name} [{options}] {pattern} [{options}]".
 *
 * Also ":syntax sync match {name} [[grouphere | groupthere] {group-name}] .."
 */
    ایستا تهی
syn_cmd_match(
    exarg_T	*eap,
    صحیح		syncing)	    // TRUE for ":syntax sync match .. "
{
    char_u	*arg = eap->arg;
    char_u	*group_name_end;
    char_u	*rest;
    synpat_T	item;		// the item found in the line
    صحیح		syn_id;
    صحیح		idx;
    syn_opt_arg_T syn_opt_arg;
    صحیح		sync_idx = 0;
    صحیح		conceal_char = NUL;
    صحیح		orig_called_emsg = called_emsg;

    // Isolate the group name, check for validity
    rest = get_group_name(arg, &group_name_end);

    // Get options before the pattern
    syn_opt_arg.flags = 0;
    syn_opt_arg.keyword = FALSE;
    syn_opt_arg.sync_idx = syncing ? &sync_idx : پوچ;
    syn_opt_arg.has_cont_list = TRUE;
    syn_opt_arg.cont_list = پوچ;
    syn_opt_arg.cont_in_list = پوچ;
    syn_opt_arg.next_list = پوچ;
    rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);

    // get the pattern.
    init_syn_patterns();
    CLEAR_FIELD(item);
    rest = get_syn_pattern(rest, &item);
    اگر (vim_regcomp_had_eol() && !(syn_opt_arg.flags & HL_EXCLUDENL))
	syn_opt_arg.flags |= HL_HAS_EOL;

    // Get options after the pattern
    rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);

    اگر (rest != پوچ)		// all arguments are valid
    {
	/*
	 * Check for trailing command and illegal trailing arguments.
	 */
	set_nextcmd(eap, rest);
	اگر (!ends_excmd2(eap->cmd, rest) || eap->skip)
	    rest = پوچ;
	وگرنه اگر (ga_grow(&curwin->w_s->b_syn_patterns, 1) == OK
		&& (syn_id = syn_check_group(arg,
					   (صحیح)(group_name_end - arg))) != 0)
	{
	    syn_incl_toplevel(syn_id, &syn_opt_arg.flags);
	    /*
	     * Store the pattern in the syn_items list
	     */
	    idx = curwin->w_s->b_syn_patterns.ga_len;
	    SYN_اقلام(curwin->w_s)[idx] = item;
	    SYN_اقلام(curwin->w_s)[idx].sp_syncing = syncing;
	    SYN_اقلام(curwin->w_s)[idx].sp_type = SPTYPE_MATCH;
	    SYN_اقلام(curwin->w_s)[idx].sp_syn.id = syn_id;
	    SYN_اقلام(curwin->w_s)[idx].sp_syn.inc_tag = current_syn_inc_tag;
	    SYN_اقلام(curwin->w_s)[idx].sp_flags = syn_opt_arg.flags;
	    SYN_اقلام(curwin->w_s)[idx].sp_sync_idx = sync_idx;
	    SYN_اقلام(curwin->w_s)[idx].sp_cont_list = syn_opt_arg.cont_list;
	    SYN_اقلام(curwin->w_s)[idx].sp_syn.cont_in_list =
						     syn_opt_arg.cont_in_list;
#ifdef FEAT_CONCEAL
	    SYN_اقلام(curwin->w_s)[idx].sp_cchar = conceal_char;
#endif
	    اگر (syn_opt_arg.cont_in_list != پوچ)
		curwin->w_s->b_syn_containedin = TRUE;
	    SYN_اقلام(curwin->w_s)[idx].sp_next_list = syn_opt_arg.next_list;
	    ++curwin->w_s->b_syn_patterns.ga_len;

	    // remember that we found a match for syncing on
	    اگر (syn_opt_arg.flags & (HL_SYNC_HERE|HL_SYNC_THERE))
		curwin->w_s->b_syn_sync_flags |= SF_MATCH;
#ifdef FEAT_FOLDING
	    اگر (syn_opt_arg.flags & HL_FOLD)
		++curwin->w_s->b_syn_folditems;
#endif

	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_پشته_free_all(curwin->w_s);	// Need to recompute all syntax.
	    بازگشت;	// don't free the progs and patterns now
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

    اگر (rest == پوچ && called_emsg == orig_called_emsg)
	semsg(_(e_invalid_argument_str), arg);
}

/*
 * Handle ":syntax region {group-name} [matchgroup={group-name}]
 *		start {start} .. [skip {skip}] end {end} .. [{options}]".
 */
    ایستا تهی
syn_cmd_region(
    exarg_T	*eap,
    صحیح		syncing)	    // TRUE for ":syntax sync region .."
{
    char_u		*arg = eap->arg;
    char_u		*group_name_end;
    char_u		*rest;			// next arg, NULL on error
    char_u		*کلید_end;
    char_u		*کلید = پوچ;
    char_u		*p;
    صحیح			item;
#define ITEM_START	    0
#define ITEM_SKIP	    1
#define ITEM_END	    2
#define ITEM_MATCHGROUP	    3
    ساختار pat_ptr
    {
	synpat_T	*pp_synp;		// pointer to syn_pattern
	صحیح		pp_matchgroup_id;	// matchgroup ID
	ساختار pat_ptr	*pp_next;		// pointer to next pat_ptr
    }			*(pat_ptrs[3]);
					// patterns found in the line
    ساختار pat_ptr	*ppp;
    ساختار pat_ptr	*ppp_next;
    صحیح			pat_شمار = 0;		// nr of syn_patterns found
    صحیح			syn_id;
    صحیح			matchgroup_id = 0;
    صحیح			not_enough = FALSE;	// not enough arguments
    صحیح			illegal = FALSE;	// illegal arguments
    صحیح			success = FALSE;
    صحیح			idx;
    syn_opt_arg_T	syn_opt_arg;
    صحیح			conceal_char = NUL;

    // Isolate the group name, check for validity
    rest = get_group_name(arg, &group_name_end);

    pat_ptrs[0] = پوچ;
    pat_ptrs[1] = پوچ;
    pat_ptrs[2] = پوچ;

    init_syn_patterns();

    syn_opt_arg.flags = 0;
    syn_opt_arg.keyword = FALSE;
    syn_opt_arg.sync_idx = پوچ;
    syn_opt_arg.has_cont_list = TRUE;
    syn_opt_arg.cont_list = پوچ;
    syn_opt_arg.cont_in_list = پوچ;
    syn_opt_arg.next_list = پوچ;

    /*
     * get the options, patterns and matchgroup.
     */
    وقتی (rest != پوچ && !ends_excmd2(eap->cmd, rest))
    {
	// Check for option arguments
	rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);
	اگر (rest == پوچ || ends_excmd2(eap->cmd, rest))
	    بشکن;

	// must be a pattern or matchgroup then
	کلید_end = rest;
	وقتی (*کلید_end && !VIM_ISWHITE(*کلید_end) && *کلید_end != '=')
	    ++کلید_end;
	vim_free(کلید);
	کلید = vim_strnsave_up(rest, کلید_end - rest);
	اگر (کلید == پوچ)			// out of memory
	{
	    rest = پوچ;
	    بشکن;
	}
	اگر (STRCMP(کلید, "MATCHGROUP") == 0)
	    item = ITEM_MATCHGROUP;
	وگرنه اگر (STRCMP(کلید, "START") == 0)
	    item = ITEM_START;
	وگرنه اگر (STRCMP(کلید, "END") == 0)
	    item = ITEM_END;
	وگرنه اگر (STRCMP(کلید, "SKIP") == 0)
	{
	    اگر (pat_ptrs[ITEM_SKIP] != پوچ)	// one skip pattern allowed
	    {
		illegal = TRUE;
		بشکن;
	    }
	    item = ITEM_SKIP;
	}
	وگرنه
	    بشکن;
	rest = skipwhite(کلید_end);
	اگر (*rest != '=')
	{
	    rest = پوچ;
	    semsg(_(e_missing_equal_str), arg);
	    بشکن;
	}
	rest = skipwhite(rest + 1);
	اگر (*rest == NUL)
	{
	    not_enough = TRUE;
	    بشکن;
	}

	اگر (item == ITEM_MATCHGROUP)
	{
	    p = skiptowhite(rest);
	    اگر ((p - rest == 4 && STRNCMP(rest, "NONE", 4) == 0) || eap->skip)
		matchgroup_id = 0;
	    وگرنه
	    {
		matchgroup_id = syn_check_group(rest, (صحیح)(p - rest));
		اگر (matchgroup_id == 0)
		{
		    illegal = TRUE;
		    بشکن;
		}
	    }
	    rest = skipwhite(p);
	}
	وگرنه
	{
	    /*
	     * Allocate room for a syn_pattern, and link it in the list of
	     * syn_patterns for this item, at the start (because the list is
	     * used from end to start).
	     */
	    ppp = ALLOC_ONE(ساختار pat_ptr);
	    اگر (ppp == پوچ)
	    {
		rest = پوچ;
		بشکن;
	    }
	    ppp->pp_next = pat_ptrs[item];
	    pat_ptrs[item] = ppp;
	    ppp->pp_synp = ALLOC_CLEAR_ONE(synpat_T);
	    اگر (ppp->pp_synp == پوچ)
	    {
		rest = پوچ;
		بشکن;
	    }

	    /*
	     * Get the syntax pattern and the following offset(s).
	     */
	    // Enable the appropriate \z specials.
	    اگر (item == ITEM_START)
		reg_do_extmatch = REX_SET;
	    وگرنه اگر (item == ITEM_SKIP || item == ITEM_END)
		reg_do_extmatch = REX_USE;
	    rest = get_syn_pattern(rest, ppp->pp_synp);
	    reg_do_extmatch = 0;
	    اگر (item == ITEM_END && vim_regcomp_had_eol()
				       && !(syn_opt_arg.flags & HL_EXCLUDENL))
		ppp->pp_synp->sp_flags |= HL_HAS_EOL;
	    ppp->pp_matchgroup_id = matchgroup_id;
	    ++pat_شمار;
	}
    }
    vim_free(کلید);
    اگر (illegal || not_enough)
	rest = پوچ;

    /*
     * Must have a "start" and "end" pattern.
     */
    اگر (rest != پوچ && (pat_ptrs[ITEM_START] == پوچ ||
						  pat_ptrs[ITEM_END] == پوچ))
    {
	not_enough = TRUE;
	rest = پوچ;
    }

    اگر (rest != پوچ)
    {
	/*
	 * Check for trailing garbage or command.
	 * If OK, add the item.
	 */
	set_nextcmd(eap, rest);
	اگر (!ends_excmd(*rest) || eap->skip)
	    rest = پوچ;
	وگرنه اگر (ga_grow(&(curwin->w_s->b_syn_patterns), pat_شمار) == OK
		&& (syn_id = syn_check_group(arg,
					   (صحیح)(group_name_end - arg))) != 0)
	{
	    syn_incl_toplevel(syn_id, &syn_opt_arg.flags);
	    /*
	     * Store the start/skip/end in the syn_items list
	     */
	    idx = curwin->w_s->b_syn_patterns.ga_len;
	    برای (item = ITEM_START; item <= ITEM_END; ++item)
	    {
		برای (ppp = pat_ptrs[item]; ppp != پوچ; ppp = ppp->pp_next)
		{
		    SYN_اقلام(curwin->w_s)[idx] = *(ppp->pp_synp);
		    SYN_اقلام(curwin->w_s)[idx].sp_syncing = syncing;
		    SYN_اقلام(curwin->w_s)[idx].sp_type =
			    (item == ITEM_START) ? SPTYPE_START :
			    (item == ITEM_SKIP) ? SPTYPE_SKIP : SPTYPE_END;
		    SYN_اقلام(curwin->w_s)[idx].sp_flags |= syn_opt_arg.flags;
		    SYN_اقلام(curwin->w_s)[idx].sp_syn.id = syn_id;
		    SYN_اقلام(curwin->w_s)[idx].sp_syn.inc_tag =
							  current_syn_inc_tag;
		    SYN_اقلام(curwin->w_s)[idx].sp_syn_match_id =
							ppp->pp_matchgroup_id;
#ifdef FEAT_CONCEAL
		    SYN_اقلام(curwin->w_s)[idx].sp_cchar = conceal_char;
#endif
		    اگر (item == ITEM_START)
		    {
			SYN_اقلام(curwin->w_s)[idx].sp_cont_list =
							syn_opt_arg.cont_list;
			SYN_اقلام(curwin->w_s)[idx].sp_syn.cont_in_list =
						     syn_opt_arg.cont_in_list;
			اگر (syn_opt_arg.cont_in_list != پوچ)
			    curwin->w_s->b_syn_containedin = TRUE;
			SYN_اقلام(curwin->w_s)[idx].sp_next_list =
							syn_opt_arg.next_list;
		    }
		    ++curwin->w_s->b_syn_patterns.ga_len;
		    ++idx;
#ifdef FEAT_FOLDING
		    اگر (syn_opt_arg.flags & HL_FOLD)
			++curwin->w_s->b_syn_folditems;
#endif
		}
	    }

	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_پشته_free_all(curwin->w_s);	// Need to recompute all syntax.
	    success = TRUE;	    // don't free the progs and patterns now
	}
    }

    /*
     * Free the allocated memory.
     */
    برای (item = ITEM_START; item <= ITEM_END; ++item)
	برای (ppp = pat_ptrs[item]; ppp != پوچ; ppp = ppp_next)
	{
	    اگر (!success && ppp->pp_synp != پوچ)
	    {
		vim_regfree(ppp->pp_synp->sp_prog);
		vim_free(ppp->pp_synp->sp_pattern);
	    }
	    vim_free(ppp->pp_synp);
	    ppp_next = ppp->pp_next;
	    vim_free(ppp);
	}

    اگر (!success)
    {
	vim_free(syn_opt_arg.cont_list);
	vim_free(syn_opt_arg.cont_in_list);
	vim_free(syn_opt_arg.next_list);
	اگر (not_enough)
	    semsg(_(e_not_enough_arguments_syntax_region_str), arg);
	وگرنه اگر (illegal || rest == پوچ)
	    semsg(_(e_invalid_argument_str), arg);
    }
}

/*
 * A simple syntax group ID comparison function suitable for use in qsort()
 */
    ایستا صحیح
syn_compare_stub(ثابت تهی *v1, ثابت تهی *v2)
{
    ثابت short	*s1 = v1;
    ثابت short	*s2 = v2;

    بازگشت (*s1 > *s2 ? 1 : *s1 < *s2 ? -1 : 0);
}

/*
 * Combines lists of syntax clusters.
 * *clstr1 and *clstr2 must both be allocated memory; they will be consumed.
 */
    ایستا تهی
syn_combine_list(short **clstr1, short **clstr2, صحیح list_op)
{
    صحیح		count1 = 0;
    صحیح		count2 = 0;
    short	*g1;
    short	*g2;
    short	*clstr = پوچ;
    صحیح		شمار;
    صحیح		round;

    /*
     * Handle degenerate cases.
     */
    اگر (*clstr2 == پوچ)
	بازگشت;
    اگر (*clstr1 == پوچ || list_op == CLUSTER_REPLACE)
    {
	اگر (list_op == CLUSTER_REPLACE)
	    vim_free(*clstr1);
	اگر (list_op == CLUSTER_REPLACE || list_op == CLUSTER_ADD)
	    *clstr1 = *clstr2;
	وگرنه
	    vim_free(*clstr2);
	بازگشت;
    }

    برای (g1 = *clstr1; *g1; g1++)
	++count1;
    برای (g2 = *clstr2; *g2; g2++)
	++count2;

    /*
     * For speed purposes, sort both lists.
     */
    qsort(*clstr1, (size_t)count1, اندازه(short), syn_compare_stub);
    qsort(*clstr2, (size_t)count2, اندازه(short), syn_compare_stub);

    /*
     * We proceed in two passes; in round 1, we count the elements to place
     * in the new list, and in round 2, we allocate and populate the new
     * list.  For speed, we use a mergesort-like method, adding the smaller
     * of the current elements in each list to the new list.
     */
    برای (round = 1; round <= 2; round++)
    {
	g1 = *clstr1;
	g2 = *clstr2;
	شمار = 0;

	/*
	 * First, loop through the lists until one of them is empty.
	 */
	وقتی (*g1 && *g2)
	{
	    /*
	     * We always want to add from the first list.
	     */
	    اگر (*g1 < *g2)
	    {
		اگر (round == 2)
		    clstr[شمار] = *g1;
		شمار++;
		g1++;
		ادامه;
	    }
	    /*
	     * We only want to add from the second list if we're adding the
	     * lists.
	     */
	    اگر (list_op == CLUSTER_ADD)
	    {
		اگر (round == 2)
		    clstr[شمار] = *g2;
		شمار++;
	    }
	    اگر (*g1 == *g2)
		g1++;
	    g2++;
	}

	/*
	 * Now add the leftovers from whichever list didn't get finished
	 * first.  As before, we only want to add from the second list if
	 * we're adding the lists.
	 */
	برای (; *g1; g1++, شمار++)
	    اگر (round == 2)
		clstr[شمار] = *g1;
	اگر (list_op == CLUSTER_ADD)
	    برای (; *g2; g2++, شمار++)
		اگر (round == 2)
		    clstr[شمار] = *g2;

	اگر (round == 1)
	{
	    /*
	     * If the group ended up empty, we don't need to allocate any
	     * space for it.
	     */
	    اگر (شمار == 0)
	    {
		clstr = پوچ;
		بشکن;
	    }
	    clstr = ALLOC_MULT(short, شمار + 1);
	    اگر (clstr == پوچ)
		بشکن;
	    clstr[شمار] = 0;
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
    ایستا صحیح
syn_scl_name2id(char_u *name)
{
    صحیح		i;
    char_u	*name_u;

    // Avoid using stricmp() too much, it's slow on some systems
    name_u = vim_strsave_up(name);
    اگر (name_u == پوچ)
	بازگشت 0;
    برای (i = curwin->w_s->b_syn_clusters.ga_len; --i >= 0; )
	اگر (SYN_CLSTR(curwin->w_s)[i].scl_name_u != پوچ
		&& STRCMP(name_u, SYN_CLSTR(curwin->w_s)[i].scl_name_u) == 0)
	    بشکن;
    vim_free(name_u);
    بازگشت (i < 0 ? 0 : i + SYNID_CLUSTER);
}

/*
 * Like syn_scl_name2id(), but take a pointer + length argument.
 */
    ایستا صحیح
syn_scl_namen2id(char_u *linep, صحیح len)
{
    char_u  *name;
    صحیح	    id = 0;

    name = vim_strnsave(linep, len);
    اگر (name == پوچ)
	بازگشت 0;

    id = syn_scl_name2id(name);
    vim_free(name);
    بازگشت id;
}

/*
 * Find syntax cluster name in the table and return its ID.
 * The argument is a pointer to the name and the length of the name.
 * If it doesn't exist yet, a new entry is created.
 * Return 0 for failure.
 */
    ایستا صحیح
syn_check_cluster(char_u *pp, صحیح len)
{
    صحیح		id;
    char_u	*name;

    name = vim_strnsave(pp, len);
    اگر (name == پوچ)
	بازگشت 0;

    id = syn_scl_name2id(name);
    اگر (id == 0)			// doesn't exist yet
	id = syn_add_cluster(name);
    وگرنه
	vim_free(name);
    بازگشت id;
}

/*
 * Add new syntax cluster and return its ID.
 * "name" must be an allocated string, it will be consumed.
 * Return 0 for failure.
 */
    ایستا صحیح
syn_add_cluster(char_u *name)
{
    صحیح		len;

    /*
     * First call for this growarray: init growing array.
     */
    اگر (curwin->w_s->b_syn_clusters.ga_data == پوچ)
    {
	curwin->w_s->b_syn_clusters.ga_itemsize = اندازه(syn_cluster_T);
	curwin->w_s->b_syn_clusters.ga_growsize = 10;
    }

    len = curwin->w_s->b_syn_clusters.ga_len;
    اگر (len >= MAX_CLUSTER_ID)
    {
	emsg(_(e_too_many_syntax_clusters));
	vim_free(name);
	بازگشت 0;
    }

    /*
     * Make room for at least one other cluster entry.
     */
    اگر (ga_grow(&curwin->w_s->b_syn_clusters, 1) == FAIL)
    {
	vim_free(name);
	بازگشت 0;
    }

    CLEAR_POINTER(&(SYN_CLSTR(curwin->w_s)[len]));
    SYN_CLSTR(curwin->w_s)[len].scl_name = name;
    SYN_CLSTR(curwin->w_s)[len].scl_name_u = vim_strsave_up(name);
    SYN_CLSTR(curwin->w_s)[len].scl_list = پوچ;
    ++curwin->w_s->b_syn_clusters.ga_len;

    اگر (STRICMP(name, "Spell") == 0)
	curwin->w_s->b_spell_cluster_id = len + SYNID_CLUSTER;
    اگر (STRICMP(name, "NoSpell") == 0)
	curwin->w_s->b_nospell_cluster_id = len + SYNID_CLUSTER;

    بازگشت len + SYNID_CLUSTER;
}

/*
 * Handle ":syntax cluster {cluster-name} [contains={groupname},..]
 *		[add={groupname},..] [remove={groupname},..]".
 */
    ایستا تهی
syn_cmd_cluster(exarg_T *eap, صحیح syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*group_name_end;
    char_u	*rest;
    صحیح		scl_id;
    short	*clstr_list;
    صحیح		got_clstr = FALSE;
    صحیح		opt_len;
    صحیح		list_op;

    eap->nextcmd = find_nextcmd(arg);
    اگر (eap->skip)
	بازگشت;

    rest = get_group_name(arg, &group_name_end);

    اگر (rest != پوچ)
    {
	scl_id = syn_check_cluster(arg, (صحیح)(group_name_end - arg));
	اگر (scl_id == 0)
	    بازگشت;
	scl_id -= SYNID_CLUSTER;

	برای (;;)
	{
	    اگر (STRNICMP(rest, "add", 3) == 0
		    && (VIM_ISWHITE(rest[3]) || rest[3] == '='))
	    {
		opt_len = 3;
		list_op = CLUSTER_ADD;
	    }
	    وگرنه اگر (STRNICMP(rest, "remove", 6) == 0
		    && (VIM_ISWHITE(rest[6]) || rest[6] == '='))
	    {
		opt_len = 6;
		list_op = CLUSTER_SUBTRACT;
	    }
	    وگرنه اگر (STRNICMP(rest, "contains", 8) == 0
			&& (VIM_ISWHITE(rest[8]) || rest[8] == '='))
	    {
		opt_len = 8;
		list_op = CLUSTER_REPLACE;
	    }
	    وگرنه
		بشکن;

	    clstr_list = پوچ;
	    اگر (get_id_list(&rest, opt_len, &clstr_list, eap->skip) == FAIL)
	    {
		semsg(_(e_invalid_argument_str), rest);
		بشکن;
	    }
	    اگر (scl_id >= 0)
		syn_combine_list(&SYN_CLSTR(curwin->w_s)[scl_id].scl_list,
			     &clstr_list, list_op);
	    وگرنه
		vim_free(clstr_list);
	    got_clstr = TRUE;
	}

	اگر (got_clstr)
	{
	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_پشته_free_all(curwin->w_s);	// Need to recompute all.
	}
    }

    اگر (!got_clstr)
	emsg(_(e_no_cluster_specified));
    اگر (rest == پوچ || !ends_excmd2(eap->cmd, rest))
	semsg(_(e_invalid_argument_str), arg);
}

/*
 * On first call for current buffer: Init growing array.
 */
    ایستا تهی
init_syn_patterns(تهی)
{
    curwin->w_s->b_syn_patterns.ga_itemsize = اندازه(synpat_T);
    curwin->w_s->b_syn_patterns.ga_growsize = 10;
}

/*
 * Get one pattern for a ":syntax match" or ":syntax region" command.
 * Stores the pattern and program in a synpat_T.
 * Returns a pointer to the next argument, or NULL in case of an error.
 */
    ایستا char_u *
get_syn_pattern(char_u *arg, synpat_T *ci)
{
    char_u	*end;
    صحیح		*p;
    صحیح		idx;
    char_u	*cpo_save;

    // need at least three chars
    اگر (arg == پوچ || arg[0] == NUL || arg[1] == NUL || arg[2] == NUL)
	بازگشت پوچ;

    end = skip_regexp(arg + 1, *arg, TRUE);
    اگر (*end != *arg)			    // end delimiter not found
    {
	semsg(_(e_pattern_delimiter_not_found_str), arg);
	بازگشت پوچ;
    }
    // store the pattern and compiled regexp program
    اگر ((ci->sp_pattern = vim_strnsave(arg + 1, end - arg - 1)) == پوچ)
	بازگشت پوچ;

    // Make 'cpoptions' empty, to avoid the 'l' flag
    cpo_save = p_cpo;
    p_cpo = empty_option;
    ci->sp_prog = vim_regcomp(ci->sp_pattern, RE_MAGIC);
    p_cpo = cpo_save;

    اگر (ci->sp_prog == پوچ)
	بازگشت پوچ;
    ci->sp_ic = curwin->w_s->b_syn_ic;
#ifdef FEAT_PROFILE
    syn_clear_time(&ci->sp_time);
#endif

    /*
     * Check for a match, highlight or region offset.
     */
    ++end;
    انجامبده
    {
	برای (idx = SPO_شمار; --idx >= 0; )
	    اگر (STRNCMP(end, spo_name_tab[idx], 3) == 0)
		بشکن;
	اگر (idx >= 0)
	{
	    p = &(ci->sp_offsets[idx]);
	    اگر (idx != SPO_LC_OFF)
		انتخاب (end[3])
		{
		    حالت 's':   بشکن;
		    حالت 'b':   بشکن;
		    حالت 'e':   idx += SPO_شمار; بشکن;
		    پیشفرض:    idx = -1; بشکن;
		}
	    اگر (idx >= 0)
	    {
		ci->sp_off_flags |= (1 << idx);
		اگر (idx == SPO_LC_OFF)	    // lc=99
		{
		    end += 3;
		    *p = getdigits(&end);

		    // "lc=" offset automatically sets "ms=" offset
		    اگر (!(ci->sp_off_flags & (1 << SPO_MS_OFF)))
		    {
			ci->sp_off_flags |= (1 << SPO_MS_OFF);
			ci->sp_offsets[SPO_MS_OFF] = *p;
		    }
		}
		وگرنه			    // yy=x+99
		{
		    end += 4;
		    اگر (*end == '+')
		    {
			++end;
			*p = getdigits(&end);		// positive offset
		    }
		    وگرنه اگر (*end == '-')
		    {
			++end;
			*p = -getdigits(&end);		// negative offset
		    }
		}
		اگر (*end != ',')
		    بشکن;
		++end;
	    }
	}
    } وقتی (idx >= 0);

    اگر (!ends_excmd2(arg, end) && !VIM_ISWHITE(*end))
    {
	semsg(_(e_garbage_after_pattern_str), arg);
	بازگشت پوچ;
    }
    بازگشت skipwhite(end);
}

/*
 * Handle ":syntax sync .." command.
 */
    ایستا تهی
syn_cmd_sync(exarg_T *eap, صحیح syncing UNUSED)
{
    char_u	*arg_start = eap->arg;
    char_u	*arg_end;
    char_u	*کلید = پوچ;
    char_u	*next_arg;
    صحیح		illegal = FALSE;
    صحیح		finished = FALSE;
    long	n;
    char_u	*cpo_save;

    اگر (ends_excmd2(eap->cmd, arg_start))
    {
	syn_cmd_list(eap, TRUE);
	بازگشت;
    }

    وقتی (!ends_excmd2(eap->cmd, arg_start))
    {
	arg_end = skiptowhite(arg_start);
	next_arg = skipwhite(arg_end);
	vim_free(کلید);
	کلید = vim_strnsave_up(arg_start, arg_end - arg_start);
	اگر (کلید == پوچ)
	    بشکن;
	اگر (STRCMP(کلید, "CCOMMENT") == 0)
	{
	    اگر (!eap->skip)
		curwin->w_s->b_syn_sync_flags |= SF_CCOMMENT;
	    اگر (!ends_excmd2(eap->cmd, next_arg))
	    {
		arg_end = skiptowhite(next_arg);
		اگر (!eap->skip)
		    curwin->w_s->b_syn_sync_id = syn_check_group(next_arg,
						   (صحیح)(arg_end - next_arg));
		next_arg = skipwhite(arg_end);
	    }
	    وگرنه اگر (!eap->skip)
		curwin->w_s->b_syn_sync_id = syn_name2id((char_u *)"Comment");
	}
	وگرنه اگر (  STRNCMP(کلید, "LINES", 5) == 0
		|| STRNCMP(کلید, "MINLINES", 8) == 0
		|| STRNCMP(کلید, "MAXLINES", 8) == 0
		|| STRNCMP(کلید, "LINEBREAKS", 10) == 0)
	{
	    اگر (کلید[4] == 'S')
		arg_end = کلید + 6;
	    وگرنه اگر (کلید[0] == 'L')
		arg_end = کلید + 11;
	    وگرنه
		arg_end = کلید + 9;
	    اگر (arg_end[-1] != '=' || !VIM_ISDIGIT(*arg_end))
	    {
		illegal = TRUE;
		بشکن;
	    }
	    n = getdigits(&arg_end);
	    اگر (!eap->skip)
	    {
		اگر (کلید[4] == 'B')
		    curwin->w_s->b_syn_sync_linebreaks = n;
		وگرنه اگر (کلید[1] == 'A')
		    curwin->w_s->b_syn_sync_maxlines = n;
		وگرنه
		    curwin->w_s->b_syn_sync_minlines = n;
	    }
	}
	وگرنه اگر (STRCMP(کلید, "FROMSTART") == 0)
	{
	    اگر (!eap->skip)
	    {
		curwin->w_s->b_syn_sync_minlines = MAXLNUM;
		curwin->w_s->b_syn_sync_maxlines = 0;
	    }
	}
	وگرنه اگر (STRCMP(کلید, "LINECONT") == 0)
	{
	    اگر (*next_arg == NUL)	   // missing pattern
	    {
		illegal = TRUE;
		بشکن;
	    }
	    اگر (curwin->w_s->b_syn_linecont_pat != پوچ)
	    {
		emsg(_(e_syntax_sync_سطر_continuations_pattern_specified_twice));
		finished = TRUE;
		بشکن;
	    }
	    arg_end = skip_regexp(next_arg + 1, *next_arg, TRUE);
	    اگر (*arg_end != *next_arg)	    // end delimiter not found
	    {
		illegal = TRUE;
		بشکن;
	    }

	    اگر (!eap->skip)
	    {
		// store the pattern and compiled regexp program
		اگر ((curwin->w_s->b_syn_linecont_pat =
			    vim_strnsave(next_arg + 1,
				      arg_end - next_arg - 1)) == پوچ)
		{
		    finished = TRUE;
		    بشکن;
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

		اگر (curwin->w_s->b_syn_linecont_prog == پوچ)
		{
		    VIM_CLEAR(curwin->w_s->b_syn_linecont_pat);
		    finished = TRUE;
		    بشکن;
		}
	    }
	    next_arg = skipwhite(arg_end + 1);
	}
	وگرنه
	{
	    eap->arg = next_arg;
	    اگر (STRCMP(کلید, "MATCH") == 0)
		syn_cmd_match(eap, TRUE);
	    وگرنه اگر (STRCMP(کلید, "REGION") == 0)
		syn_cmd_region(eap, TRUE);
	    وگرنه اگر (STRCMP(کلید, "CLEAR") == 0)
		syn_cmd_clear(eap, TRUE);
	    وگرنه
		illegal = TRUE;
	    finished = TRUE;
	    بشکن;
	}
	arg_start = next_arg;
    }
    vim_free(کلید);
    اگر (illegal)
	semsg(_(e_illegal_arguments_str), arg_start);
    وگرنه اگر (!finished)
    {
	set_nextcmd(eap, arg_start);
	redraw_curbuf_later(UPD_SOME_VALID);
	syn_پشته_free_all(curwin->w_s);	// Need to recompute all syntax.
    }
}

/*
 * Convert a line of highlight group names into a list of group ID numbers.
 * "arg" should point to the "contains" or "nextgroup" keyword.
 * "arg" is advanced to after the last group name.
 * Careful: the argument is modified (NULs added).
 * returns FAIL for some error, OK for success.
 */
    ایستا صحیح
get_id_list(
    char_u	**arg,
    صحیح		keylen,		// length of keyword
    short	**list,		// where to store the resulting list, if not
				// NULL, the list is silently skipped!
    صحیح		skip)
{
    char_u	*p = پوچ;
    char_u	*end;
    صحیح		round;
    صحیح		شمار;
    صحیح		total_شمار = 0;
    short	*retval = پوچ;
    char_u	*name;
    regmatch_T	regmatch;
    صحیح		id;
    صحیح		i;
    صحیح		failed = FALSE;

    /*
     * We parse the list twice:
     * round == 1: count the number of items, allocate the array.
     * round == 2: fill the array with the items.
     * In round 1 new groups may be added, causing the number of items to
     * grow when a regexp is used.  In that case round 1 is done once again.
     */
    برای (round = 1; round <= 2; ++round)
    {
	/*
	 * skip "contains"
	 */
	p = skipwhite(*arg + keylen);
	اگر (*p != '=')
	{
	    semsg(_(e_missing_equal_sign_str), *arg);
	    بشکن;
	}
	p = skipwhite(p + 1);
	اگر (ends_excmd2(*arg, p))
	{
	    semsg(_(e_empty_argument_str), *arg);
	    بشکن;
	}

	/*
	 * parse the arguments after "contains"
	 */
	شمار = 0;
	وقتی (!ends_excmd2(*arg, p))
	{
	    برای (end = p; *end && !VIM_ISWHITE(*end) && *end != ','; ++end)
		;
	    name = alloc(end - p + 3);	    // leave room for "^$"
	    اگر (name == پوچ)
	    {
		failed = TRUE;
		بشکن;
	    }
	    vim_strncpy(name + 1, p, end - p);
	    اگر (       STRCMP(name + 1, "ALLBUT") == 0
		    || STRCMP(name + 1, "ALL") == 0
		    || STRCMP(name + 1, "TOP") == 0
		    || STRCMP(name + 1, "CONTAINED") == 0)
	    {
		اگر (TOUPPER_ASC(**arg) != 'C')
		{
		    semsg(_(e_str_not_allowed_here), name + 1);
		    failed = TRUE;
		    vim_free(name);
		    بشکن;
		}
		اگر (شمار != 0)
		{
		    semsg(_(e_str_must_be_first_in_contains_list), name + 1);
		    failed = TRUE;
		    vim_free(name);
		    بشکن;
		}
		اگر (name[1] == 'A')
		    id = SYNID_ALLBUT;
		وگرنه اگر (name[1] == 'T')
		    id = SYNID_بالاترین;
		وگرنه
		    id = SYNID_CONTAINED;
		id += current_syn_inc_tag;
	    }
	    وگرنه اگر (name[1] == '@')
	    {
		اگر (skip)
		    id = -1;
		وگرنه
		    id = syn_check_cluster(name + 2, (صحیح)(end - p - 1));
	    }
	    وگرنه
	    {
		/*
		 * Handle full group name.
		 */
		اگر (vim_strpbrk(name + 1, (char_u *)"\\.*^$~[") == پوچ)
		    id = syn_check_group(name + 1, (صحیح)(end - p));
		وگرنه
		{
		    /*
		     * Handle match of regexp with group names.
		     */
		    *name = '^';
		    STRCAT(name, "$");
		    regmatch.regprog = vim_regcomp(name, RE_MAGIC);
		    اگر (regmatch.regprog == پوچ)
		    {
			failed = TRUE;
			vim_free(name);
			بشکن;
		    }

		    regmatch.rm_ic = TRUE;
		    id = 0;
		    برای (i = highlight_رقم_groups(); --i >= 0; )
		    {
			اگر (vim_regexec(&regmatch, highlight_group_name(i),
								  (colnr_T)0))
			{
			    اگر (round == 2)
			    {
				// Got more items than expected; can happen
				// when adding items that match:
				// "contains=a.*b,axb".
				// Go back to first round
				اگر (شمار >= total_شمار)
				{
				    vim_free(retval);
				    round = 1;
				}
				وگرنه
				    retval[شمار] = i + 1;
			    }
			    ++شمار;
			    id = -1;	    // remember that we found one
			}
		    }
		    vim_regfree(regmatch.regprog);
		}
	    }
	    vim_free(name);
	    اگر (id == 0)
	    {
		semsg(_(e_unknown_group_name_str), p);
		failed = TRUE;
		بشکن;
	    }
	    اگر (id > 0)
	    {
		اگر (round == 2)
		{
		    // Got more items than expected, go back to first round
		    اگر (شمار >= total_شمار)
		    {
			vim_free(retval);
			round = 1;
		    }
		    وگرنه
			retval[شمار] = id;
		}
		++شمار;
	    }
	    p = skipwhite(end);
	    اگر (*p != ',')
		بشکن;
	    p = skipwhite(p + 1);	// skip comma in between arguments
	}
	اگر (failed)
	    بشکن;
	اگر (round == 1)
	{
	    retval = ALLOC_MULT(short, شمار + 1);
	    اگر (retval == پوچ)
		بشکن;
	    retval[شمار] = 0;	    // zero means end of the list
	    total_شمار = شمار;
	}
    }

    *arg = p;
    اگر (failed || retval == پوچ)
    {
	vim_free(retval);
	بازگشت FAIL;
    }

    اگر (*list == پوچ)
	*list = retval;
    وگرنه
	vim_free(retval);	// list already found, don't overwrite it

    بازگشت OK;
}

/*
 * Make a copy of an ID list.
 */
    ایستا short *
copy_id_list(short *list)
{
    صحیح	    len;
    صحیح	    شمار;
    short   *retval;

    اگر (list == پوچ)
	بازگشت پوچ;

    برای (شمار = 0; list[شمار]; ++شمار)
	;
    len = (شمار + 1) * اندازه(short);
    retval = alloc(len);
    اگر (retval != پوچ)
	mch_memmove(retval, list, (size_t)len);

    بازگشت retval;
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

تعریفنوع ساختار
{
    short	*idl_کلید;	// the list this entry resolves (NULL when empty)
    short	*idl_ids;	// sorted, cluster-expanded group IDs (allocated)
    صحیح		idl_شمار;	// number of IDs in idl_ids
    short	idl_marker;	// leading ALLBUT/TOP/CONTAINED item, or 0
    bool	idl_usable;	// false: not cacheable, use the slow path
} idl_entry_T;

تعریفنوع ساختار
{
    idl_entry_T	ent[IDL_CACHE_SIZE];
    صحیح		round;		// round-robin replacement index
    صحیح		last;		// index of the last returned entry (fast path)
} idl_cache_T;

/*
 * Collect the explicit group IDs of "list" into "ga", expanding clusters.
 * "seen" records visited clusters to break cycles and avoid duplicate work.
 * Return false when the list cannot be represented as a plain set (a nested
 * ALLBUT/TOP/CONTAINED marker) or on out of memory.
 */
    ایستا bool
idl_collect(short *list, garray_T *ga, garray_T *seen)
{
    اگر (list == پوچ || list == ID_LIST_ALL)
	بازگشت false;
    برای (short item = *list; item != 0; item = *++list)
    {
	اگر (item >= SYNID_ALLBUT && item < SYNID_CLUSTER)
	    بازگشت false;	// nested ALLBUT/TOP/CONTAINED: not cacheable
	اگر (item >= SYNID_CLUSTER)
	{
	    short	*scl_list;
	    bool	seen_it = false;

	    برای (صحیح i = 0; i < seen->ga_len; ++i)
		اگر (((short *)seen->ga_data)[i] == item)
		{
		    seen_it = true;
		    بشکن;
		}
	    اگر (seen_it)
		ادامه;
	    اگر (ga_grow(seen, 1) == FAIL)
		بازگشت false;
	    ((short *)seen->ga_data)[seen->ga_len++] = item;
	    // Record the cluster id itself, not just its members: the slow
	    // path matches "item == id" for a cluster id present in the list.
	    اگر (ga_grow(ga, 1) == FAIL)
		بازگشت false;
	    ((short *)ga->ga_data)[ga->ga_len++] = item;
	    scl_list = SYN_CLSTR(syn_block)[item - SYNID_CLUSTER].scl_list;
	    اگر (scl_list != پوچ && !idl_collect(scl_list, ga, seen))
		بازگشت false;
	}
	وگرنه
	{
	    اگر (ga_grow(ga, 1) == FAIL)
		بازگشت false;
	    ((short *)ga->ga_data)[ga->ga_len++] = item;
	}
    }
    بازگشت true;
}

    ایستا صحیح
idl_id_cmp(ثابت تهی *a, ثابت تهی *b)
{
    بازگشت (صحیح)*(ثابت short *)a - (صحیح)*(ثابت short *)b;
}

/*
 * Get the cache entry that resolves "list", building it if needed.
 * Returns NULL only on allocation failure.
 */
    ایستا idl_entry_T *
idl_get_entry(short *list)
{
    idl_cache_T	*cache = (idl_cache_T *)syn_block->b_idlist_cache;
    idl_entry_T	*e;
    garray_T	ga, seen;

    اگر (cache == پوچ)
    {
	cache = ALLOC_CLEAR_ONE(idl_cache_T);
	اگر (cache == پوچ)
	    بازگشت پوچ;
	syn_block->b_idlist_cache = cache;
    }
    // Fast path: the same list is usually queried many times in a row.
    اگر (cache->ent[cache->last].idl_کلید == list)
	بازگشت &cache->ent[cache->last];
    برای (صحیح i = 0; i < IDL_CACHE_SIZE; ++i)
	اگر (cache->ent[i].idl_کلید == list)
	{
	    cache->last = i;
	    بازگشت &cache->ent[i];
	}

    // Build a new entry, replacing one in round-robin order.
    cache->last = cache->round;
    e = &cache->ent[cache->round];
    cache->round = (cache->round + 1) % IDL_CACHE_SIZE;
    VIM_CLEAR(e->idl_ids);
    e->idl_شمار = 0;
    e->idl_کلید = list;
    e->idl_marker = 0;
    e->idl_usable = true;

    // A leading ALLBUT/TOP/CONTAINED marker is kept aside; the rest is the set.
    اگر (*list >= SYNID_ALLBUT && *list < SYNID_CLUSTER)
    {
	e->idl_marker = *list;
	++list;
    }

    ga_init2(&ga, اندازه(short), 50);
    ga_init2(&seen, اندازه(short), 10);
    اگر (!idl_collect(list, &ga, &seen))
    {
	e->idl_usable = false;
	ga_clear(&ga);
	ga_clear(&seen);
	بازگشت e;
    }
    ga_clear(&seen);
    اگر (ga.ga_len > 0)
    {
	qsort(ga.ga_data, (size_t)ga.ga_len, اندازه(short), idl_id_cmp);
	e->idl_ids = (short *)ga.ga_data;	// take ownership
	e->idl_شمار = ga.ga_len;
    }
    وگرنه
	ga_clear(&ga);
    بازگشت e;
}

/*
 * Return true if "id" is in the resolved set of cache entry "e".
 */
    ایستا bool
idl_contains(idl_entry_T *e, short id)
{
    صحیح	lo = 0;
    صحیح	hi = e->idl_شمار - 1;

    وقتی (lo <= hi)
    {
	صحیح	mid = (lo + hi) / 2;
	short	v = e->idl_ids[mid];

	اگر (v == id)
	    بازگشت true;
	اگر (v < id)
	    lo = mid + 1;
	وگرنه
	    hi = mid - 1;
    }
    بازگشت false;
}

    ایستا تهی
idl_cache_clear(synblock_T *block)
{
    idl_cache_T	*cache = (idl_cache_T *)block->b_idlist_cache;

    اگر (cache == پوچ)
	بازگشت;
    برای (صحیح i = 0; i < IDL_CACHE_SIZE; ++i)
	vim_free(cache->ent[i].idl_ids);
    vim_free(cache);
    block->b_idlist_cache = پوچ;
}
// }}}

/*
 * Check if syntax group "ssp" is in the ID list "list" of "cur_si".
 * "cur_si" can be NULL if not checking the "containedin" list.
 * Used to check if a syntax item is in the "contains" or "nextgroup" list of
 * the current item.
 * This function is called very often, keep it fast!!
 */
    ایستا صحیح
in_id_list(
    stateitem_T	*cur_si,	// current item or NULL
    short	*list,		// id list
    ساختار sp_syn *ssp,		// group id and ":syn include" tag of group
    صحیح		flags)		// group flags
{
    صحیح		retval;
    short	*scl_list;
    short	item;
    short	id = ssp->id;
    ایستا صحیح	depth = 0;
    صحیح		r;
    صحیح		toplevel;
    idl_entry_T	*e;

    // If ssp has a "containedin" list and "cur_si" is in it, return TRUE.
    اگر (cur_si != پوچ && ssp->cont_in_list != پوچ
					    && !(cur_si->si_flags & HL_MATCH))
    {
	// Ignore transparent items without a contains argument.  Double check
	// that we don't go back past the first one.
	وقتی ((cur_si->si_flags & HL_TRANS_CONT)
		&& cur_si > (stateitem_T *)(current_state.ga_data))
	    --cur_si;
	// cur_si->si_idx is -1 for keywords, these never contain anything.
	اگر (cur_si->si_idx >= 0 && in_id_list(پوچ, ssp->cont_in_list,
		&(SYN_اقلام(syn_block)[cur_si->si_idx].sp_syn),
		  SYN_اقلام(syn_block)[cur_si->si_idx].sp_flags))
	    بازگشت TRUE;
    }

    اگر (list == پوچ)
	بازگشت FALSE;

    /*
     * If list is ID_LIST_ALL, we are in a transparent item that isn't
     * inside anything.  Only allow not-contained groups.
     */
    اگر (list == ID_LIST_ALL)
	بازگشت !(flags & HL_CONTAINED);

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
    e = disable_syn_idlist_cache_for_testing ? پوچ : idl_get_entry(list);
    اگر (e != پوچ && e->idl_usable)
    {
	اگر (e->idl_marker != 0)
	{
	    item = e->idl_marker;
	    اگر (item < SYNID_بالاترین)
	    {
		اگر (item - SYNID_ALLBUT != ssp->inc_tag)
		    بازگشت FALSE;
	    }
	    وگرنه اگر (item < SYNID_CONTAINED)
	    {
		اگر (item - SYNID_بالاترین != ssp->inc_tag || !toplevel)
		    بازگشت FALSE;
	    }
	    وگرنه
	    {
		اگر (item - SYNID_CONTAINED != ssp->inc_tag || toplevel)
		    بازگشت FALSE;
	    }
	    retval = FALSE;
	}
	وگرنه
	    retval = TRUE;
	بازگشت idl_contains(e, id) ? retval : !retval;
    }

    /*
     * Slow path (uncacheable list, e.g. a nested ALLBUT).
     * If the first item is "ALLBUT", return TRUE if "id" is NOT in the
     * contains list.  We also require that "id" is at the same ":syn include"
     * level as the list.
     */
    item = *list;
    اگر (item >= SYNID_ALLBUT && item < SYNID_CLUSTER)
    {
	اگر (item < SYNID_بالاترین)
	{
	    // ALL or ALLBUT: accept all groups in the same file
	    اگر (item - SYNID_ALLBUT != ssp->inc_tag)
		بازگشت FALSE;
	}
	وگرنه اگر (item < SYNID_CONTAINED)
	{
	    // TOP: accept all not-contained groups in the same file
	    اگر (item - SYNID_بالاترین != ssp->inc_tag || !toplevel)
		بازگشت FALSE;
	}
	وگرنه
	{
	    // CONTAINED: accept all contained groups in the same file
	    اگر (item - SYNID_CONTAINED != ssp->inc_tag || toplevel)
		بازگشت FALSE;
	}
	item = *++list;
	retval = FALSE;
    }
    وگرنه
	retval = TRUE;

    /*
     * Return "retval" if id is in the contains list.
     */
    وقتی (item != 0)
    {
	اگر (item == id)
	    بازگشت retval;
	اگر (item >= SYNID_CLUSTER)
	{
	    scl_list = SYN_CLSTR(syn_block)[item - SYNID_CLUSTER].scl_list;
	    // restrict recursiveness to 30 to avoid an endless loop for a
	    // cluster that includes itself (indirectly)
	    اگر (scl_list != پوچ && depth < 30)
	    {
		++depth;
		r = in_id_list(پوچ, scl_list, ssp, flags);
		--depth;
		اگر (r)
		    بازگشت retval;
	    }
	}
	item = *++list;
    }
    بازگشت !retval;
}

ساختار subcommand
{
    نویسه    *name;			// subcommand name
    تهی    (*func)(exarg_T *, صحیح);	// function to call
};

ایستا ساختار subcommand subcommands[] =
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
    تهی
ex_syntax(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*subcmd_end;
    char_u	*subcmd_name;
    صحیح		i;

    syn_cmdlinep = eap->cmdlinep;

    // isolate subcommand name
    برای (subcmd_end = arg; ASCII_ISALPHA(*subcmd_end); ++subcmd_end)
	;
    subcmd_name = vim_strnsave(arg, subcmd_end - arg);
    اگر (subcmd_name == پوچ)
	بازگشت;

    اگر (eap->skip)		// skip error messages for all subcommands
	++emsg_skip;
    برای (i = 0; i < (صحیح)ARRAY_LENGTH(subcommands); ++i)
    {
	اگر (STRCMP(subcmd_name, (char_u *)subcommands[i].name) == 0)
	{
	    eap->arg = skipwhite(subcmd_end);
	    (subcommands[i].func)(eap, FALSE);
	    بشکن;
	}
    }

    اگر (i == (صحیح)ARRAY_LENGTH(subcommands))
	semsg(_(e_invalid_syntax_subcommand_str), subcmd_name);

    vim_free(subcmd_name);
    اگر (eap->skip)
	--emsg_skip;
}

    تهی
ex_ownsyntax(exarg_T *eap)
{
    char_u	*old_مقدار;
    char_u	*new_مقدار_2;

    اگر (curwin->w_s == &curwin->w_buffer->b_s)
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
    old_مقدار = get_var_مقدار((char_u *)"b:current_syntax");
    اگر (old_مقدار != پوچ)
	old_مقدار = vim_strsave(old_مقدار);

    // Apply the "syntax" autocommand event, this finds and loads the syntax
    // file.
    apply_autocmds(EVENT_SYNTAX, eap->arg, curbuf->b_fname, TRUE, curbuf);

    // move value of b:current_syntax to w:current_syntax
    new_مقدار_2 = get_var_مقدار((char_u *)"b:current_syntax");
    اگر (new_مقدار_2 != پوچ)
	set_internal_string_var((char_u *)"w:current_syntax", new_مقدار_2);

    // restore value of b:current_syntax
    اگر (old_مقدار == پوچ)
	do_unlet((char_u *)"b:current_syntax", TRUE);
    وگرنه
    {
	set_internal_string_var((char_u *)"b:current_syntax", old_مقدار);
	vim_free(old_مقدار);
    }
}

    صحیح
syntax_present(win_T *win)
{
    بازگشت (win->w_s->b_syn_patterns.ga_len != 0
	    || win->w_s->b_syn_clusters.ga_len != 0
	    || win->w_s->b_keywtab.ht_used > 0
	    || win->w_s->b_keywtab_ic.ht_used > 0);
}


ایستا شمارش
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
    تهی
reset_expand_highlight(تهی)
{
    include_link = include_default = include_none = 0;
}

/*
 * Handle command line completion for :match and :echohl command: Add "NONE"
 * as highlight group.
 */
    تهی
set_context_in_echohl_cmd(expand_T *xp, char_u *arg)
{
    xp->xp_context = EXPAND_HIGHLIGHT;
    xp->xp_pattern = arg;
    include_none = 1;
}

/*
 * Handle command line completion for :syntax command.
 */
    تهی
set_context_in_syntax_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    // Default: expand subcommands
    xp->xp_context = EXPAND_SYNTAX;
    expand_what = EXP_SUBCMD;
    xp->xp_pattern = arg;
    include_link = 0;
    include_default = 0;

    اگر (*arg == NUL)
	بازگشت;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    اگر (*p == NUL)
	بازگشت;

    // past first word
    xp->xp_pattern = skipwhite(p);
    اگر (*skiptowhite(xp->xp_pattern) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    وگرنه اگر (STRNICMP(arg, "case", p - arg) == 0)
	expand_what = EXP_CASE;
    وگرنه اگر (STRNICMP(arg, "spell", p - arg) == 0)
	expand_what = EXP_SPELL;
    وگرنه اگر (STRNICMP(arg, "sync", p - arg) == 0)
	expand_what = EXP_SYNC;
    وگرنه اگر (STRNICMP(arg, "list", p - arg) == 0)
    {
	p = skipwhite(p);
	اگر (*p == '@')
	    expand_what = EXP_CLUSTER;
	وگرنه
	    xp->xp_context = EXPAND_HIGHLIGHT;
    }
    وگرنه اگر (STRNICMP(arg, "keyword", p - arg) == 0
	    || STRNICMP(arg, "region", p - arg) == 0
	    || STRNICMP(arg, "match", p - arg) == 0)
	xp->xp_context = EXPAND_HIGHLIGHT;
    وگرنه
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Function given to ExpandGeneric() to obtain the list syntax names for
 * expansion.
 */
    char_u *
get_syntax_name(expand_T *xp, صحیح idx)
{
    انتخاب (expand_what)
    {
	حالت EXP_SUBCMD:
	    اگر (idx < 0 || idx >= (صحیح)ARRAY_LENGTH(subcommands))
		بازگشت پوچ;
	    بازگشت (char_u *)subcommands[idx].name;
	حالت EXP_CASE:
	{
	    ایستا نویسه *case_args[] = {"match", "ignore", پوچ};
	    بازگشت (char_u *)case_args[idx];
	}
	حالت EXP_SPELL:
	{
	    ایستا نویسه *spell_args[] =
		{"toplevel", "notoplevel", "default", پوچ};
	    بازگشت (char_u *)spell_args[idx];
	}
	حالت EXP_SYNC:
	{
	    ایستا نویسه *sync_args[] =
		{"ccomment", "clear", "fromstart",
		 "linebreaks=", "linecont", "lines=", "match",
		 "maxlines=", "minlines=", "region", پوچ};
	    بازگشت (char_u *)sync_args[idx];
	}
	حالت EXP_CLUSTER:
	{
	    اگر (idx < curwin->w_s->b_syn_clusters.ga_len)
	    {
		vim_snprintf((نویسه *)xp->xp_buf, EXPAND_BUF_LEN, "@%s",
					 SYN_CLSTR(curwin->w_s)[idx].scl_name);
		بازگشت xp->xp_buf;
	    }
	    وگرنه
		بازگشت پوچ;
	}
    }
    بازگشت پوچ;
}


/*
 * Function called for expression evaluation: get syntax ID at file position.
 */
    صحیح
syn_get_id(
    win_T	*wp,
    long	lnum,
    colnr_T	col,
    صحیح		trans,	     // remove transparency
    صحیح		*spellp,     // return: can do spell checking
    صحیح		keep_state)  // keep state of char at "col"
{
    // When the position is not after the current position and in the same
    // line of the same window with the same buffer, need to restart parsing.
    اگر (wp != syn_win
	    || wp->w_buffer != syn_buf
	    || lnum != current_lnum
	    || col < current_col)
	syntax_start(wp, lnum);
    وگرنه اگر (wp->w_buffer == syn_buf
	    && lnum == current_lnum
	    && col > current_col)
	// next_match may not be correct when moving around, e.g. with the
	// "skip" expression in searchpair()
	next_match_idx = -1;

    (تهی)get_syntax_attr(col, spellp, keep_state);

    بازگشت (trans ? current_trans_id : current_id);
}

#اگر defined(FEAT_CONCEAL)
/*
 * Get extra information about the syntax item.  Must be called right after
 * get_syntax_attr().
 * Stores the current item sequence nr in "*seqnrp".
 * Returns the current flags.
 */
    صحیح
get_syntax_info(صحیح *seqnrp)
{
    *seqnrp = current_seqnr;
    بازگشت current_flags;
}

/*
 * Return conceal substitution character
 */
    صحیح
syn_get_sub_char(تهی)
{
    بازگشت current_sub_char;
}
#endif

#اگر defined(FEAT_EVAL)
/*
 * Return the syntax ID at position "i" in the current stack.
 * The caller must have called syn_get_id() before to fill the stack.
 * Returns -1 when "i" is out of range.
 */
    صحیح
syn_get_پشته_item(صحیح i)
{
    اگر (i >= current_state.ga_len)
    {
	// Need to invalidate the state, because we didn't properly finish it
	// for the last character, "keep_state" was TRUE.
	invalidate_current_state();
	current_col = MAXCOL;
	بازگشت -1;
    }
    بازگشت CUR_STATE(i).si_id;
}
#endif

#اگر defined(FEAT_FOLDING)
    ایستا صحیح
syn_cur_foldlevel(تهی)
{
    صحیح		level = 0;
    صحیح		i;

    برای (i = 0; i < current_state.ga_len; ++i)
	اگر (CUR_STATE(i).si_flags & HL_FOLD)
	    ++level;
    بازگشت level;
}

/*
 * Function called to get folding level for line "lnum" in window "wp".
 */
    صحیح
syn_get_foldlevel(win_T *wp, long lnum)
{
    صحیح		level = 0;
    صحیح		کم_level;
    صحیح		cur_level;

    // Return quickly when there are no fold items at all.
    اگر (wp->w_s->b_syn_folditems != 0
	    && !wp->w_s->b_syn_error
# ifdef SYN_TIME_LIMIT
	    && !wp->w_s->b_syn_slow
# endif
	    )
    {
	syntax_start(wp, lnum);

	// Start with the fold level at the start of the line.
	level = syn_cur_foldlevel();

	اگر (wp->w_s->b_syn_foldlevel == SYNFLD_MINIMUM)
	{
	    // Find the lowest fold level that is followed by a higher one.
	    cur_level = level;
	    کم_level = cur_level;
	    وقتی (!current_finished)
	    {
		(تهی)syn_current_attr(FALSE, FALSE, پوچ, FALSE);
		cur_level = syn_cur_foldlevel();
		اگر (cur_level < کم_level)
		    کم_level = cur_level;
		وگرنه اگر (cur_level > کم_level)
		    level = کم_level;
		++current_col;
	    }
	}
    }
    اگر (level > wp->w_p_fdn)
    {
	level = wp->w_p_fdn;
	اگر (level < 0)
	    level = 0;
    }
    بازگشت level;
}
#endif

#اگر defined(FEAT_PROFILE)
/*
 * ":syntime".
 */
    تهی
ex_syntime(exarg_T *eap)
{
    اگر (STRCMP(eap->arg, "on") == 0)
	syn_time_on = TRUE;
    وگرنه اگر (STRCMP(eap->arg, "off") == 0)
	syn_time_on = FALSE;
    وگرنه اگر (STRCMP(eap->arg, "clear") == 0)
	syntime_clear();
    وگرنه اگر (STRCMP(eap->arg, "report") == 0)
	syntime_report();
    وگرنه
	semsg(_(e_invalid_argument_str), eap->arg);
}

    ایستا تهی
syn_clear_time(syn_time_T *st)
{
    profile_zero(&st->total);
    profile_zero(&st->slowest);
    st->شمار = 0;
    st->match = 0;
}

/*
 * Clear the syntax timing for the current buffer.
 */
    ایستا تهی
syntime_clear(تهی)
{
    صحیح		idx;
    synpat_T	*spp;

    اگر (!syntax_present(curwin))
    {
	پیام(_(پیام_no_اقلام));
	بازگشت;
    }
    برای (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len; ++idx)
    {
	spp = &(SYN_اقلام(curwin->w_s)[idx]);
	syn_clear_time(&spp->sp_time);
    }
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":syntime {on,off,clear,report}" command.
 */
    char_u *
get_syntime_arg(expand_T *xp UNUSED, صحیح idx)
{
    انتخاب (idx)
    {
	حالت 0: بازگشت (char_u *)"on";
	حالت 1: بازگشت (char_u *)"off";
	حالت 2: بازگشت (char_u *)"clear";
	حالت 3: بازگشت (char_u *)"report";
    }
    بازگشت پوچ;
}

تعریفنوع ساختار
{
    proftime_T	total;
    صحیح		شمار;
    صحیح		match;
    proftime_T	slowest;
    proftime_T	average;
    صحیح		id;
    char_u	*pattern;
} time_entry_T;

    ایستا صحیح
syn_compare_syntime(ثابت تهی *v1, ثابت تهی *v2)
{
    ثابت time_entry_T	*s1 = v1;
    ثابت time_entry_T	*s2 = v2;

    بازگشت profile_cmp(&s1->total, &s2->total);
}

/*
 * Clear the syntax timing for the current buffer.
 */
    ایستا تهی
syntime_report(تهی)
{
    صحیح		idx;
    synpat_T	*spp;
# اگر defined(FEAT_RELTIME)
    proftime_T	tm;
# endif
    صحیح		len;
    صحیح		patlen;
    proftime_T	total_total;
    صحیح		total_شمار = 0;
    garray_T    ga;
    time_entry_T *p;

    اگر (!syntax_present(curwin))
    {
	پیام(_(پیام_no_اقلام));
	بازگشت;
    }

    ga_init2(&ga, اندازه(time_entry_T), 50);
    profile_zero(&total_total);
    برای (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len; ++idx)
    {
	spp = &(SYN_اقلام(curwin->w_s)[idx]);
	اگر (spp->sp_time.شمار > 0)
	{
	    (تهی)ga_grow(&ga, 1);
	    p = ((time_entry_T *)ga.ga_data) + ga.ga_len;
	    p->total = spp->sp_time.total;
	    profile_add(&total_total, &spp->sp_time.total);
	    p->شمار = spp->sp_time.شمار;
	    p->match = spp->sp_time.match;
	    total_شمار += spp->sp_time.شمار;
	    p->slowest = spp->sp_time.slowest;
# اگر defined(FEAT_RELTIME)
	    profile_divide(&spp->sp_time.total, spp->sp_time.شمار, &tm);
	    p->average = tm;
# endif
	    p->id = spp->sp_syn.id;
	    p->pattern = spp->sp_pattern;
	    ++ga.ga_len;
	}
    }

    // Sort on total time. Skip if there are no items to avoid passing NULL
    // pointer to qsort().
    اگر (ga.ga_len > 1)
	qsort(ga.ga_data, (size_t)ga.ga_len, اندازه(time_entry_T),
							 syn_compare_syntime);

    پیام_puts_title(_("  TOTAL      COUNT  MATCH   SLOWEST     AVERAGE   NAME               PATTERN"));
    پیام_puts("\n");
    برای (idx = 0; idx < ga.ga_len && !got_int; ++idx)
    {
	p = ((time_entry_T *)ga.ga_data) + idx;

	پیام_puts(profile_پیام(&p->total));
	پیام_puts(" "); // make sure there is always a separating space
	پیام_advance(13);
	پیام_outnum(p->شمار);
	پیام_puts(" ");
	پیام_advance(20);
	پیام_outnum(p->match);
	پیام_puts(" ");
	پیام_advance(26);
	پیام_puts(profile_پیام(&p->slowest));
	پیام_puts(" ");
	پیام_advance(38);
	پیام_puts(profile_پیام(&p->average));
	پیام_puts(" ");
	پیام_advance(50);
	پیام_outtrans(highlight_group_name(p->id - 1));
	پیام_puts(" ");

	پیام_advance(69);
	اگر (Columns < 80)
	    len = 20; // will wrap anyway
	وگرنه
	    len = Columns - 70;
	patlen = (صحیح)STRLEN(p->pattern);
	اگر (len > patlen)
	    len = patlen;
	پیام_outtrans_len(p->pattern, len);
	پیام_puts("\n");
    }
    ga_clear(&ga);
    اگر (!got_int)
    {
	پیام_puts("\n");
	پیام_puts(profile_پیام(&total_total));
	پیام_advance(13);
	پیام_outnum(total_شمار);
	پیام_puts("\n");
    }
}
#endif

#endif // FEAT_SYN_HL
