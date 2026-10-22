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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_SYN_HL)

// different types of offsets that are possible
#define SPO_MS_OFF	0	// match  start offset
#define SPO_ME_OFF	1	// match  end	offset
#define SPO_HS_OFF	2	// highl. start offset
#define SPO_HE_OFF	3	// highl. end	offset
#define SPO_RS_OFF	4	// region start offset
#define SPO_RE_OFF	5	// region end	offset
#define SPO_LC_OFF	6	// leading context offset
#define SPO_ગણતરી	7

સ્થિર અક્ષર *(spo_name_tab[SPO_ગણતરી]) =
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
પ્રકારવ્યાખ્યા સંરચના syn_pattern
{
    અક્ષર	 sp_type;		// see SPTYPE_ defines below
    અક્ષર	 sp_syncing;		// this item used for syncing
    short	 sp_syn_match_id;	// highlight group ID of pattern
    short	 sp_off_flags;		// see below
    પૂર્ણાંક		 sp_offsets[SPO_ગણતરી];	// offsets
    પૂર્ણાંક		 sp_flags;		// see HL_ defines below
#ifdef FEAT_CONCEAL
    પૂર્ણાંક		 sp_cchar;		// conceal substitute character
#endif
    પૂર્ણાંક		 sp_ic;			// ignore-case flag for sp_prog
    પૂર્ણાંક		 sp_sync_idx;		// sync item index (syncing only)
    પૂર્ણાંક		 sp_લીટી_id;		// ID of last line where tried
    પૂર્ણાંક		 sp_startcol;		// next match in sp_line_id line
    short	*sp_cont_list;		// cont. group IDs, if non-zero
    short	*sp_next_list;		// next group IDs, if non-zero
    સંરચના sp_syn sp_syn;		// struct passed to in_id_list()
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


#define SYN_વસ્તુઓ(buf)	((synpat_T *)((buf)->b_syn_patterns.ga_data))

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
સ્થિર પૂર્ણાંક current_attr = 0;	    // attr of current syntax word
#ifdef FEAT_EVAL
સ્થિર પૂર્ણાંક current_id = 0;	    // ID of current char for syn_get_id()
સ્થિર પૂર્ણાંક current_trans_id = 0;    // idem, transparency removed
#endif
#ifdef FEAT_CONCEAL
સ્થિર પૂર્ણાંક current_flags = 0;
સ્થિર પૂર્ણાંક current_seqnr = 0;
સ્થિર પૂર્ણાંક current_sub_char = 0;
#endif

પ્રકારવ્યાખ્યા સંરચના syn_cluster_S
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
#define SYNID_ટોચ	21000	    // syntax group ID for contains=TOP
#define SYNID_CONTAINED	22000	    // syntax group ID for contains=CONTAINED
#define SYNID_CLUSTER	23000	    // first syntax group ID for clusters

#define MAX_SYN_INC_TAG	999	    // maximum before the above overflow
#define MAX_CLUSTER_ID  (32767 - SYNID_CLUSTER)

/*
 * Annoying Hack(TM):  ":syn include" needs this pointer to pass to
 * expand_filename().  Most of the other syntax commands don't need it, so
 * instead of passing it to them, we stow it here.
 */
સ્થિર char_u **syn_cmdlinep;

/*
 * Another Annoying Hack(TM):  To prevent rules from other ":syn include"'d
 * files from leaking into ALLBUT lists, we assign a unique ID to the
 * rules in each ":syn include"'d file.
 */
સ્થિર પૂર્ણાંક current_syn_inc_tag = 0;
સ્થિર પૂર્ણાંક running_syn_inc_tag = 0;

/*
 * In a hashtable item "hi_key" points to "keyword" in a keyentry.
 * This avoids adding a pointer to the hashtable item.
 * KE2HIKEY() converts a var pointer to a hashitem key pointer.
 * HIKEY2KE() converts a hashitem key pointer to a var pointer.
 * HI2KE() converts a hashitem pointer to a var pointer.
 */
સ્થિર keyentry_T dumkey;
#define KE2HIKEY(kp)  ((kp)->keyword)
#define HIKEY2KE(p)   ((keyentry_T *)((p) - (dumkey.keyword - (char_u *)&dumkey)))
#define HI2KE(hi)      HIKEY2KE((hi)->hi_ચાવી)

/*
 * To reduce the time spent in keepend(), remember at which level in the state
 * stack the first item with "keepend" is present.  When "-1", there is no
 * "keepend" on the stack.
 */
સ્થિર પૂર્ણાંક keepend_level = -1;

સ્થિર અક્ષર સંદેશ_no_વસ્તુઓ[] = N_("No Syntax items defined for this buffer");

/*
 * For the current state we need to remember more than just the idx.
 * When si_m_endpos.lnum is 0, the items other than si_idx are unknown.
 * (The end positions have the column number of the next char)
 */
પ્રકારવ્યાખ્યા સંરચના state_item
{
    પૂર્ણાંક		si_idx;			// index of syntax pattern or
					// KEYWORD_IDX
    પૂર્ણાંક		si_id;			// highlight group ID for keywords
    પૂર્ણાંક		si_trans_id;		// idem, transparency removed
    પૂર્ણાંક		si_m_lnum;		// lnum of the match
    પૂર્ણાંક		si_m_startcol;		// starting column of the match
    lpos_T	si_m_endpos;		// just after end posn of the match
    lpos_T	si_h_startpos;		// start position of the highlighting
    lpos_T	si_h_endpos;		// end position of the highlighting
    lpos_T	si_eoe_pos;		// end position of end pattern
    પૂર્ણાંક		si_end_idx;		// group ID for end pattern or zero
    પૂર્ણાંક		si_ends;		// if match ends before si_m_endpos
    પૂર્ણાંક		si_attr;		// attributes in this state
    long	si_flags;		// HL_HAS_EOL flag in this state, and
					// HL_SKIP* for si_next_list
#ifdef FEAT_CONCEAL
    પૂર્ણાંક		si_seqnr;		// sequence number
    પૂર્ણાંક		si_cchar;		// substitution character for conceal
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
સ્થિર પૂર્ણાંક next_seqnr = 1;		// value to use for si_seqnr
#endif

/*
 * Struct to reduce the number of arguments to get_syn_options(), it's used
 * very often.
 */
પ્રકારવ્યાખ્યા સંરચના
{
    પૂર્ણાંક		flags;		// flags for contained and transparent
    પૂર્ણાંક		keyword;	// TRUE for ":syn keyword"
    પૂર્ણાંક		*sync_idx;	// syntax item for "grouphere" argument, NULL
				// if not allowed
    અક્ષર	has_cont_list;	// TRUE if "cont_list" can be used
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
સ્થિર પૂર્ણાંક next_match_col;		// column for start of next match
સ્થિર lpos_T next_match_m_endpos;	// position for end of next match
સ્થિર lpos_T next_match_h_startpos;	// pos. for highl. start of next match
સ્થિર lpos_T next_match_h_endpos;	// pos. for highl. end of next match
સ્થિર પૂર્ણાંક next_match_idx;		// index of matched item
સ્થિર long next_match_flags;		// flags for next match
સ્થિર lpos_T next_match_eos_pos;	// end of start pattn (start region)
સ્થિર lpos_T next_match_eoe_pos;	// pos. for end of end pattern
સ્થિર પૂર્ણાંક next_match_end_idx;		// ID of group for end pattn or zero
સ્થિર reg_extmatch_T *next_match_extmatch = ખાલી;

/*
 * A state stack is an array of integers or stateitem_T, stored in a
 * garray_T.  A state stack is invalid if its itemsize entry is zero.
 */
#define INVALID_STATE(ssp)  ((ssp)->ga_itemsize == 0)
#define VALID_STATE(ssp)    ((ssp)->ga_itemsize != 0)

#define FOR_ALL_SYNSTATES(sb, sst) \
    માટે ((sst) = (sb)->b_sst_first; (sst) != ખાલી; (sst) = (sst)->sst_next)

/*
 * The current state (within the line) of the recognition engine.
 * When current_state.ga_itemsize is 0 the current state is invalid.
 */
સ્થિર win_T	*syn_win;		// current window for highlighting
સ્થિર buf_T	*syn_buf;		// current buffer for highlighting
સ્થિર synblock_T *syn_block;		// current buffer for highlighting
સ્થિર linenr_T current_lnum = 0;	// lnum of current state
સ્થિર colnr_T	current_col = 0;	// column of current state
સ્થિર પૂર્ણાંક	current_state_stored = 0; // TRUE if stored current state
					  // after setting current_finished
સ્થિર પૂર્ણાંક	current_finished = 0;	// current line has been finished
સ્થિર garray_T current_state		// current stack of state_items
		= {0, 0, 0, 0, ખાલી};
સ્થિર short	*current_next_list = ખાલી; // when non-zero, nextgroup list
સ્થિર પૂર્ણાંક	current_next_flags = 0; // flags for current_next_list
સ્થિર પૂર્ણાંક	current_લીટી_id = 0;	// unique number for current line

#define CUR_STATE(idx)	((stateitem_T *)(current_state.ga_data))[idx]

સ્થિર શૂન્ય syn_sync(win_T *wp, linenr_T lnum, synstate_T *last_valid);
સ્થિર પૂર્ણાંક syn_match_linecont(linenr_T lnum);
સ્થિર શૂન્ય syn_start_લીટી(શૂન્ય);
સ્થિર શૂન્ય syn_update_ends(પૂર્ણાંક startofline);
સ્થિર શૂન્ય syn_ઢગલો_alloc(શૂન્ય);
સ્થિર પૂર્ણાંક syn_ઢગલો_cleanup(શૂન્ય);
સ્થિર શૂન્ય syn_ઢગલો_free_entry(synblock_T *block, synstate_T *p);
સ્થિર synstate_T *syn_ઢગલો_find_entry(linenr_T lnum);
સ્થિર synstate_T *store_current_state(શૂન્ય);
સ્થિર શૂન્ય load_current_state(synstate_T *from);
સ્થિર શૂન્ય invalidate_current_state(શૂન્ય);
સ્થિર પૂર્ણાંક syn_ઢગલો_equal(synstate_T *sp);
સ્થિર શૂન્ય validate_current_state(શૂન્ય);
સ્થિર પૂર્ણાંક syn_finish_લીટી(પૂર્ણાંક syncing);
સ્થિર પૂર્ણાંક syn_current_attr(પૂર્ણાંક syncing, પૂર્ણાંક displaying, પૂર્ણાંક *can_spell, પૂર્ણાંક keep_state);
સ્થિર પૂર્ણાંક did_match_already(પૂર્ણાંક idx, garray_T *gap);
સ્થિર stateitem_T *ધકેલો_next_match(stateitem_T *cur_si);
સ્થિર શૂન્ય check_state_ends(શૂન્ય);
સ્થિર શૂન્ય update_si_attr(પૂર્ણાંક idx);
સ્થિર શૂન્ય check_keepend(શૂન્ય);
સ્થિર શૂન્ય update_si_end(stateitem_T *sip, પૂર્ણાંક startcol, પૂર્ણાંક force);
સ્થિર short *copy_id_list(short *list);
સ્થિર પૂર્ણાંક in_id_list(stateitem_T *item, short *cont_list, સંરચના sp_syn *ssp, પૂર્ણાંક flags);
સ્થિર પૂર્ણાંક ધકેલો_current_state(પૂર્ણાંક idx);
સ્થિર શૂન્ય કાઢો_current_state(શૂન્ય);
#ifdef FEAT_PROFILE
સ્થિર શૂન્ય syn_clear_time(syn_time_T *tt);
સ્થિર શૂન્ય syntime_clear(શૂન્ય);
સ્થિર શૂન્ય syntime_report(શૂન્ય);
સ્થિર પૂર્ણાંક syn_time_on = FALSE;
# define IF_SYN_TIME(p) (p)
#નહીંતર
# define IF_SYN_TIME(p) ખાલી
પ્રકારવ્યાખ્યા પૂર્ણાંક syn_time_T;
#endif

સ્થિર શૂન્ય syn_ઢગલો_apply_changes_block(synblock_T *block, buf_T *buf);
સ્થિર શૂન્ય find_endpos(પૂર્ણાંક idx, lpos_T *startpos, lpos_T *m_endpos, lpos_T *hl_endpos, long *flagsp, lpos_T *end_endpos, પૂર્ણાંક *end_idx, reg_extmatch_T *start_ext);

સ્થિર શૂન્ય limit_pos(lpos_T *pos, lpos_T *limit);
સ્થિર શૂન્ય limit_pos_zero(lpos_T *pos, lpos_T *limit);
સ્થિર શૂન્ય syn_add_end_off(lpos_T *result, regmmatch_T *regmatch, synpat_T *spp, પૂર્ણાંક idx, પૂર્ણાંક extra);
સ્થિર શૂન્ય syn_add_start_off(lpos_T *result, regmmatch_T *regmatch, synpat_T *spp, પૂર્ણાંક idx, પૂર્ણાંક extra);
સ્થિર char_u *syn_getcurline(શૂન્ય);
સ્થિર colnr_T syn_getcurline_len(શૂન્ય);
સ્થિર પૂર્ણાંક syn_regexec(regmmatch_T *rmp, linenr_T lnum, colnr_T col, syn_time_T *st);
સ્થિર પૂર્ણાંક check_keyword_id(char_u *લીટી, પૂર્ણાંક startcol, પૂર્ણાંક *endcol, long *flags, short **next_list, stateitem_T *cur_si, પૂર્ણાંક *ccharp);
સ્થિર શૂન્ય syn_remove_pattern(synblock_T *block, પૂર્ણાંક idx);
સ્થિર શૂન્ય syn_clear_pattern(synblock_T *block, પૂર્ણાંક i);
સ્થિર શૂન્ય syn_clear_cluster(synblock_T *block, પૂર્ણાંક i);
સ્થિર શૂન્ય syn_clear_one(પૂર્ણાંક id, પૂર્ણાંક syncing);
સ્થિર શૂન્ય syn_cmd_onoff(exarg_T *eap, અક્ષર *name);
સ્થિર શૂન્ય syn_લીટીઓ_સંદેશ(શૂન્ય);
સ્થિર શૂન્ય syn_match_સંદેશ(શૂન્ય);
સ્થિર શૂન્ય syn_list_one(પૂર્ણાંક id, પૂર્ણાંક syncing, પૂર્ણાંક link_only);
સ્થિર શૂન્ય syn_list_cluster(પૂર્ણાંક id);
સ્થિર શૂન્ય put_id_list(char_u *name, short *list, પૂર્ણાંક attr);
સ્થિર શૂન્ય put_pattern(અક્ષર *s, પૂર્ણાંક c, synpat_T *spp, પૂર્ણાંક attr);
સ્થિર પૂર્ણાંક syn_list_keywords(પૂર્ણાંક id, hashtab_T *ht, પૂર્ણાંક did_header, પૂર્ણાંક attr);
સ્થિર શૂન્ય syn_clear_keyword(પૂર્ણાંક id, hashtab_T *ht);
સ્થિર શૂન્ય clear_keywtab(hashtab_T *ht);
સ્થિર પૂર્ણાંક syn_scl_namen2id(char_u *linep, પૂર્ણાંક len);
સ્થિર પૂર્ણાંક syn_check_cluster(char_u *pp, પૂર્ણાંક len);
સ્થિર પૂર્ણાંક syn_add_cluster(char_u *name);
સ્થિર શૂન્ય init_syn_patterns(શૂન્ય);
સ્થિર char_u *get_syn_pattern(char_u *arg, synpat_T *ci);
સ્થિર પૂર્ણાંક get_id_list(char_u **arg, પૂર્ણાંક keylen, short **list, પૂર્ણાંક skip);
સ્થિર શૂન્ય syn_combine_list(short **clstr1, short **clstr2, પૂર્ણાંક list_op);
સ્થિર શૂન્ય idl_cache_clear(synblock_T *block);

/*
 * Start the syntax recognition for a line.  This function is normally called
 * from the screen updating, once for each displayed line.
 * The buffer is remembered in syn_buf, because get_syntax_attr() doesn't get
 * it.	Careful: curbuf and curwin are likely to point to another buffer and
 * window.
 */
    શૂન્ય
syntax_start(win_T *wp, linenr_T lnum)
{
    synstate_T	*p;
    synstate_T	*last_valid = ખાલી;
    synstate_T	*last_min_valid = ખાલી;
    synstate_T	*sp, *prev = ખાલી;
    linenr_T	parsed_lnum;
    linenr_T	first_stored;
    પૂર્ણાંક		dist;
    સ્થિર varnumber_T changedtick = 0;	// remember the last change ID

#ifdef FEAT_CONCEAL
    current_sub_char = NUL;
#endif

    /*
     * After switching buffers, invalidate current_state.
     * Also do this when a change was made, the current state may be invalid
     * then.
     */
    જો (syn_block != wp->w_s
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
    syn_ઢગલો_alloc();
    જો (syn_block->b_sst_array == ખાલી)
	પાછા;		// out of memory
    syn_block->b_sst_lasttick = display_tick;

    /*
     * If the state of the end of the previous line is useful, store it.
     */
    જો (VALID_STATE(&current_state)
	    && current_lnum < lnum
	    && current_lnum < syn_buf->b_ml.ml_લીટી_ગણતરી)
    {
	(શૂન્ય)syn_finish_લીટી(FALSE);
	જો (!current_state_stored)
	{
	    ++current_lnum;
	    (શૂન્ય)store_current_state();
	}

	/*
	 * If the current_lnum is now the same as "lnum", keep the current
	 * state (this happens very often!).  Otherwise invalidate
	 * current_state and figure it out below.
	 */
	જો (current_lnum != lnum)
	    invalidate_current_state();
    }
    નહીંતર
	invalidate_current_state();

    /*
     * Try to synchronize from a saved state in b_sst_array[].
     * Only do this if lnum is not before and not to far beyond a saved state.
     */
    જો (INVALID_STATE(&current_state) && syn_block->b_sst_array != ખાલી)
    {
	// Find last valid saved state before start_lnum.
	FOR_ALL_SYNSTATES(syn_block, p)
	{
	    જો (p->sst_lnum > lnum)
		તોડો;
	    જો (p->sst_lnum <= lnum && p->sst_change_lnum == 0)
	    {
		last_valid = p;
		જો (p->sst_lnum >= lnum - syn_block->b_syn_sync_minlines)
		    last_min_valid = p;
	    }
	}
	જો (last_min_valid != ખાલી)
	    load_current_state(last_min_valid);
    }

    /*
     * If "lnum" is before or far beyond a line with a saved state, need to
     * re-synchronize.
     */
    જો (INVALID_STATE(&current_state))
    {
	syn_sync(wp, lnum, last_valid);
	જો (current_lnum == 1)
	    // First line is always valid, no matter "minlines".
	    first_stored = 1;
	નહીંતર
	    // Need to parse "minlines" lines before state can be considered
	    // valid to store.
	    first_stored = current_lnum + syn_block->b_syn_sync_minlines;
    }
    નહીંતર
	first_stored = current_lnum;

    /*
     * Advance from the sync point or saved state until the current line.
     * Save some entries for syncing with later on.
     */
    જો (syn_block->b_sst_len <= Rows)
	dist = 999999;
    નહીંતર
	dist = syn_buf->b_ml.ml_લીટી_ગણતરી / (syn_block->b_sst_len - Rows) + 1;
    જ્યારે (current_lnum < lnum)
    {
	syn_start_લીટી();
	(શૂન્ય)syn_finish_લીટી(FALSE);
	++current_lnum;

	// If we parsed at least "minlines" lines or started at a valid
	// state, the current state is considered valid.
	જો (current_lnum >= first_stored)
	{
	    // Check if the saved state entry is for the current line and is
	    // equal to the current state.  If so, then validate all saved
	    // states that depended on a change before the parsed line.
	    જો (prev == ખાલી)
		prev = syn_ઢગલો_find_entry(current_lnum - 1);
	    જો (prev == ખાલી)
		sp = syn_block->b_sst_first;
	    નહીંતર
		sp = prev;
	    જ્યારે (sp != ખાલી && sp->sst_lnum < current_lnum)
		sp = sp->sst_next;
	    જો (sp != ખાલી
		    && sp->sst_lnum == current_lnum
		    && syn_ઢગલો_equal(sp))
	    {
		parsed_lnum = current_lnum;
		prev = sp;
		જ્યારે (sp != ખાલી && sp->sst_change_lnum <= parsed_lnum)
		{
		    જો (sp->sst_lnum <= lnum)
			// valid state before desired line, use this one
			prev = sp;
		    નહીંતર જો (sp->sst_change_lnum == 0)
			// past saved states depending on change, break here.
			તોડો;
		    sp->sst_change_lnum = 0;
		    sp = sp->sst_next;
		}
		load_current_state(prev);
	    }
	    // Store the state at this line when it's the first one, the line
	    // where we start parsing, or some distance from the previously
	    // saved state.  But only when parsed at least 'minlines'.
	    નહીંતર જો (prev == ખાલી
			|| current_lnum == lnum
			|| current_lnum >= prev->sst_lnum + dist)
		prev = store_current_state();
	}

	// This can take a long time: break when CTRL-C pressed.  The current
	// state will be wrong then.
	લીટી_breakcheck();
	જો (got_int)
	{
	    current_lnum = lnum;
	    તોડો;
	}
    }

    syn_start_લીટી();
}

/*
 * We cannot simply discard growarrays full of state_items or buf_states; we
 * have to manually release their extmatch pointers first.
 */
    સ્થિર શૂન્ય
clear_syn_state(synstate_T *p)
{
    પૂર્ણાંક		i;
    garray_T	*gap;

    જો (p->sst_stacksize > SST_FIX_STATES)
    {
	gap = &(p->sst_union.sst_ga);
	માટે (i = 0; i < gap->ga_len; i++)
	    unref_extmatch(SYN_STATE_P(gap)[i].bs_extmatch);
	ga_clear(gap);
    }
    નહીંતર
    {
	માટે (i = 0; i < p->sst_stacksize; i++)
	    unref_extmatch(p->sst_union.sst_ઢગલો[i].bs_extmatch);
    }
}

/*
 * Cleanup the current_state stack.
 */
    સ્થિર શૂન્ય
clear_current_state(શૂન્ય)
{
    પૂર્ણાંક		i;
    stateitem_T	*sip;

    sip = (stateitem_T *)(current_state.ga_data);
    માટે (i = 0; i < current_state.ga_len; i++)
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
    સ્થિર શૂન્ય
syn_sync(
    win_T	*wp,
    linenr_T	start_lnum,
    synstate_T	*last_valid)
{
    buf_T	*curbuf_save;
    win_T	*curwin_save;
    pos_T	cursor_save;
    પૂર્ણાંક		idx;
    linenr_T	lnum;
    linenr_T	end_lnum;
    linenr_T	break_lnum;
    પૂર્ણાંક		had_sync_point;
    stateitem_T	*cur_si;
    synpat_T	*spp;
    પૂર્ણાંક		found_flags = 0;
    પૂર્ણાંક		found_match_idx = 0;
    linenr_T	found_current_lnum = 0;
    પૂર્ણાંક		found_current_col = 0;
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
    જો (syn_block->b_syn_sync_minlines > start_lnum)
	start_lnum = 1;
    નહીંતર
    {
	જો (syn_block->b_syn_sync_minlines == 1)
	    lnum = 1;
	નહીંતર જો (syn_block->b_syn_sync_minlines < 10)
	    lnum = syn_block->b_syn_sync_minlines * 2;
	નહીંતર
	    lnum = syn_block->b_syn_sync_minlines * 3 / 2;
	જો (syn_block->b_syn_sync_maxlines != 0
				     && lnum > syn_block->b_syn_sync_maxlines)
	    lnum = syn_block->b_syn_sync_maxlines;
	જો (lnum >= start_lnum)
	    start_lnum = 1;
	નહીંતર
	    start_lnum -= lnum;
    }
    current_lnum = start_lnum;

    /*
     * 1. Search backwards for the end of a C-style comment.
     */
    જો (syn_block->b_syn_sync_flags & SF_CCOMMENT)
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
	માટે ( ; start_lnum > 1; --start_lnum)
	{
	    char_u	*l = ml_get(start_lnum - 1);

	    જો (*l == NUL || *(l + ml_get_len(start_lnum - 1) - 1) != '\\')
		તોડો;
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
	જો (find_start_comment((પૂર્ણાંક)syn_block->b_syn_sync_maxlines) != ખાલી)
	{
	    માટે (idx = syn_block->b_syn_patterns.ga_len; --idx >= 0; )
		જો (SYN_વસ્તુઓ(syn_block)[idx].sp_syn.id
						   == syn_block->b_syn_sync_id
			&& SYN_વસ્તુઓ(syn_block)[idx].sp_type == SPTYPE_START)
		{
		    validate_current_state();
		    જો (ધકેલો_current_state(idx) == OK)
			update_si_attr(current_state.ga_len - 1);
		    તોડો;
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
    નહીંતર જો (syn_block->b_syn_sync_flags & SF_MATCH)
    {
	જો (syn_block->b_syn_sync_maxlines != 0
			       && start_lnum > syn_block->b_syn_sync_maxlines)
	    break_lnum = start_lnum - syn_block->b_syn_sync_maxlines;
	નહીંતર
	    break_lnum = 0;

	found_m_endpos.lnum = 0;
	found_m_endpos.col = 0;
	end_lnum = start_lnum;
	lnum = start_lnum;
	જ્યારે (--lnum > break_lnum)
	{
	    // This can take a long time: break when CTRL-C pressed.
	    લીટી_breakcheck();
	    જો (got_int)
	    {
		invalidate_current_state();
		current_lnum = start_lnum;
		તોડો;
	    }

	    // Check if we have run into a valid saved state stack now.
	    જો (last_valid != ખાલી && lnum == last_valid->sst_lnum)
	    {
		load_current_state(last_valid);
		તોડો;
	    }

	    /*
	     * Check if the previous line has the line-continuation pattern.
	     */
	    જો (lnum > 1 && syn_match_linecont(lnum - 1))
		ચાલુ;

	    /*
	     * Start with nothing on the state stack
	     */
	    validate_current_state();

	    માટે (current_lnum = lnum; current_lnum < end_lnum; ++current_lnum)
	    {
		syn_start_લીટી();
		માટે (;;)
		{
		    had_sync_point = syn_finish_લીટી(TRUE);
		    /*
		     * When a sync point has been found, remember where, and
		     * continue to look for another one, further on in the line.
		     */
		    જો (had_sync_point && current_state.ga_len)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			જો (cur_si->si_m_endpos.lnum > start_lnum)
			{
			    // ignore match that goes to after where started
			    current_lnum = end_lnum;
			    તોડો;
			}
			જો (cur_si->si_idx < 0)
			{
			    // Cannot happen?
			    found_flags = 0;
			    found_match_idx = KEYWORD_IDX;
			}
			નહીંતર
			{
			    spp = &(SYN_વસ્તુઓ(syn_block)[cur_si->si_idx]);
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
			જો (found_m_endpos.lnum > current_lnum)
			{
			    current_lnum = found_m_endpos.lnum;
			    current_col = found_m_endpos.col;
			    જો (current_lnum >= end_lnum)
				તોડો;
			}
			નહીંતર જો (found_m_endpos.col > current_col)
			    current_col = found_m_endpos.col;
			નહીંતર
			    ++current_col;

			// syn_current_attr() will have skipped the check for
			// an item that ends here, need to do that now.  Be
			// careful not to go past the NUL.
			prev_current_col = current_col;
			જો (syn_getcurline()[current_col] != NUL)
			    ++current_col;
			check_state_ends();
			current_col = prev_current_col;
		    }
		    નહીંતર
			તોડો;
		}
	    }

	    /*
	     * If a sync point was encountered, break here.
	     */
	    જો (found_flags)
	    {
		/*
		 * Put the item that was specified by the sync point on the
		 * state stack.  If there was no item specified, make the
		 * state stack empty.
		 */
		clear_current_state();
		જો (found_match_idx >= 0
			&& ધકેલો_current_state(found_match_idx) == OK)
		    update_si_attr(current_state.ga_len - 1);

		/*
		 * When using "grouphere", continue from the sync point
		 * match, until the end of the line.  Parsing starts at
		 * the next line.
		 * For "groupthere" the parsing starts at start_lnum.
		 */
		જો (found_flags & HL_SYNC_HERE)
		{
		    current_lnum = found_m_endpos.lnum;
		    current_col = found_m_endpos.col;
		    જો (current_state.ga_len)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			cur_si->si_h_startpos.lnum = found_current_lnum;
			cur_si->si_h_startpos.col = found_current_col;
			update_si_end(cur_si, (પૂર્ણાંક)current_col, TRUE);
			check_keepend();
		    }
		    (શૂન્ય)syn_finish_લીટી(FALSE);
		    ++current_lnum;
		}
		નહીંતર
		    current_lnum = start_lnum;

		તોડો;
	    }

	    end_lnum = lnum;
	    invalidate_current_state();
	}

	// Ran into start of the file or exceeded maximum number of lines
	જો (lnum <= break_lnum)
	{
	    invalidate_current_state();
	    current_lnum = break_lnum + 1;
	}
    }

    validate_current_state();
}

    સ્થિર શૂન્ય
save_chartab(char_u *chartab)
{
    જો (syn_block->b_syn_isk == empty_option)
	પાછા;

    mch_memmove(chartab, syn_buf->b_chartab, (size_t)32);
    mch_memmove(syn_buf->b_chartab, syn_win->w_s->b_syn_chartab, (size_t)32);
}

    સ્થિર શૂન્ય
restore_chartab(char_u *chartab)
{
    જો (syn_win->w_s->b_syn_isk != empty_option)
	mch_memmove(syn_buf->b_chartab, chartab, (size_t)32);
}

/*
 * Return TRUE if the line-continuation pattern matches in line "lnum".
 */
    સ્થિર પૂર્ણાંક
syn_match_linecont(linenr_T lnum)
{
    regmmatch_T regmatch;
    પૂર્ણાંક r;
    char_u	buf_chartab[32];  // chartab array for syn iskyeyword

    જો (syn_block->b_syn_linecont_prog == ખાલી)
	પાછા FALSE;

    // use syntax iskeyword option
    save_chartab(buf_chartab);
    regmatch.rmm_ic = syn_block->b_syn_linecont_ic;
    regmatch.regprog = syn_block->b_syn_linecont_prog;
    r = syn_regexec(&regmatch, lnum, (colnr_T)0,
	    IF_SYN_TIME(&syn_block->b_syn_linecont_time));
    syn_block->b_syn_linecont_prog = regmatch.regprog;
    restore_chartab(buf_chartab);
    પાછા r;
}

/*
 * Prepare the current state for the start of a line.
 */
    સ્થિર શૂન્ય
syn_start_લીટી(શૂન્ય)
{
    current_finished = FALSE;
    current_col = 0;

    /*
     * Need to update the end of a start/skip/end that continues from the
     * previous line and regions that have "keepend".
     */
    જો (current_state.ga_len > 0)
    {
	syn_update_ends(TRUE);
	check_state_ends();
    }

    next_match_idx = -1;
    ++current_લીટી_id;
#ifdef FEAT_CONCEAL
    next_seqnr = 1;
#endif
}

/*
 * Check for items in the stack that need their end updated.
 * When "startofline" is TRUE the last item is always updated.
 * When "startofline" is FALSE the item with "keepend" is forcefully updated.
 */
    સ્થિર શૂન્ય
syn_update_ends(પૂર્ણાંક startofline)
{
    stateitem_T	*cur_si;
    પૂર્ણાંક		i;
    પૂર્ણાંક		seen_keepend;

    જો (startofline)
    {
	// Check for a match carried over from a previous line with a
	// contained region.  The match ends as soon as the region ends.
	માટે (i = 0; i < current_state.ga_len; ++i)
	{
	    cur_si = &CUR_STATE(i);
	    જો (cur_si->si_idx >= 0
		    && (SYN_વસ્તુઓ(syn_block)[cur_si->si_idx]).sp_type
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
    જો (keepend_level >= 0)
	માટે ( ; i > keepend_level; --i)
	    જો (CUR_STATE(i).si_flags & HL_EXTEND)
		તોડો;

    seen_keepend = FALSE;
    માટે ( ; i < current_state.ga_len; ++i)
    {
	cur_si = &CUR_STATE(i);
	જો ((cur_si->si_flags & HL_KEEPEND)
			    || (seen_keepend && !startofline)
			    || (i == current_state.ga_len - 1 && startofline))
	{
	    cur_si->si_h_startpos.col = 0;	// start highl. in col 0
	    cur_si->si_h_startpos.lnum = current_lnum;

	    જો (!(cur_si->si_flags & HL_MATCHCONT))
		update_si_end(cur_si, (પૂર્ણાંક)current_col, !startofline);

	    જો (!startofline && (cur_si->si_flags & HL_KEEPEND))
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

    સ્થિર શૂન્ય
syn_ઢગલો_free_block(synblock_T *block)
{
    synstate_T	*p;

    // Syntax definitions may have changed: drop the in_id_list() cache.
    idl_cache_clear(block);

    જો (block->b_sst_array == ખાલી)
	પાછા;

    FOR_ALL_SYNSTATES(block, p)
	clear_syn_state(p);
    VIM_CLEAR(block->b_sst_array);
    block->b_sst_first = ખાલી;
    block->b_sst_શોધ = ખાલી;
    block->b_sst_len = 0;
}
/*
 * Free b_sst_array[] for buffer "buf".
 * Used when syntax items changed to force resyncing everywhere.
 */
    શૂન્ય
syn_ઢગલો_free_all(synblock_T *block)
{
#ifdef FEAT_FOLDING
    win_T	*wp;
#endif

    syn_ઢગલો_free_block(block);

#ifdef FEAT_FOLDING
    // When using "syntax" fold method, must update all folds.
    FOR_ALL_WINDOWS(wp)
    {
	જો (wp->w_s == block && foldmethodIsSyntax(wp))
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
    સ્થિર શૂન્ય
syn_ઢગલો_alloc(શૂન્ય)
{
    long	len;
    synstate_T	*to, *from;
    synstate_T	*sstp;

    len = syn_buf->b_ml.ml_લીટી_ગણતરી / SST_DIST + Rows * 2;
    જો (len < SST_MIN_ENTRIES)
	len = SST_MIN_ENTRIES;
    નહીંતર જો (len > SST_MAX_ENTRIES)
	len = SST_MAX_ENTRIES;
    જો (syn_block->b_sst_len > len * 2 || syn_block->b_sst_len < len)
    {
	// Allocate 50% too much, to avoid reallocating too often.
	len = syn_buf->b_ml.ml_લીટી_ગણતરી;
	len = (len + len / 2) / SST_DIST + Rows * 2;
	જો (len < SST_MIN_ENTRIES)
	    len = SST_MIN_ENTRIES;
	નહીંતર જો (len > SST_MAX_ENTRIES)
	    len = SST_MAX_ENTRIES;

	જો (syn_block->b_sst_array != ખાલી)
	{
	    // When shrinking the array, cleanup the existing stack.
	    // Make sure that all valid entries fit in the new array.
	    જ્યારે (syn_block->b_sst_len - syn_block->b_sst_freecount + 2 > len
		    && syn_ઢગલો_cleanup())
		;
	    જો (len < syn_block->b_sst_len - syn_block->b_sst_freecount + 2)
		len = syn_block->b_sst_len - syn_block->b_sst_freecount + 2;
	}

	sstp = ALLOC_CLEAR_MULT(synstate_T, len);
	જો (sstp == ખાલી)	// out of memory!
	    પાછા;

	to = sstp - 1;
	જો (syn_block->b_sst_array != ખાલી)
	{
	    // Move the states from the old array to the new one.
	    માટે (from = syn_block->b_sst_first; from != ખાલી;
							from = from->sst_next)
	    {
		++to;
		*to = *from;
		to->sst_next = to + 1;
	    }
	}
	જો (to != sstp - 1)
	{
	    to->sst_next = ખાલી;
	    syn_block->b_sst_first = sstp;
	    syn_block->b_sst_freecount = len - (પૂર્ણાંક)(to - sstp) - 1;
	}
	નહીંતર
	{
	    syn_block->b_sst_first = ખાલી;
	    syn_block->b_sst_freecount = len;
	}

	// Create the list of free entries.
	syn_block->b_sst_firstfree = to + 1;
	જ્યારે (++to < sstp + len)
	    to->sst_next = to + 1;
	(sstp + len - 1)->sst_next = ખાલી;

	vim_free(syn_block->b_sst_array);
	syn_block->b_sst_array = sstp;
	syn_block->b_sst_len = len;
	// The entries were moved to a new array, drop the stale pointer.
	syn_block->b_sst_શોધ = ખાલી;
    }
}

/*
 * Check for changes in a buffer to affect stored syntax states.  Uses the
 * b_mod_* fields.
 * Called from update_screen(), before screen is being updated, once for each
 * displayed buffer.
 */
    શૂન્ય
syn_ઢગલો_apply_changes(buf_T *buf)
{
    win_T	*wp;

    syn_ઢગલો_apply_changes_block(&buf->b_s, buf);

    FOR_ALL_WINDOWS(wp)
    {
	જો ((wp->w_buffer == buf) && (wp->w_s != &buf->b_s))
	    syn_ઢગલો_apply_changes_block(wp->w_s, buf);
    }
}

    સ્થિર શૂન્ય
syn_ઢગલો_apply_changes_block(synblock_T *block, buf_T *buf)
{
    synstate_T	*p, *prev, *np;
    linenr_T	n;

    prev = ખાલી;
    માટે (p = block->b_sst_first; p != ખાલી; )
    {
	જો (p->sst_lnum + block->b_syn_sync_linebreaks > buf->b_mod_ટોચ)
	{
	    n = p->sst_lnum + buf->b_mod_xlines;
	    જો (n <= buf->b_mod_bot)
	    {
		// this state is inside the changed area, remove it
		np = p->sst_next;
		જો (prev == ખાલી)
		    block->b_sst_first = np;
		નહીંતર
		    prev->sst_next = np;
		syn_ઢગલો_free_entry(block, p);
		p = np;
		ચાલુ;
	    }
	    // This state is below the changed area.  Remember the line
	    // that needs to be parsed before this entry can be made valid
	    // again.
	    જો (p->sst_change_lnum != 0 && p->sst_change_lnum > buf->b_mod_ટોચ)
	    {
		જો (p->sst_change_lnum + buf->b_mod_xlines > buf->b_mod_ટોચ)
		    p->sst_change_lnum += buf->b_mod_xlines;
		નહીંતર
		    p->sst_change_lnum = buf->b_mod_ટોચ;
	    }
	    જો (p->sst_change_lnum == 0
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
    સ્થિર પૂર્ણાંક
syn_ઢગલો_cleanup(શૂન્ય)
{
    synstate_T	*p, *prev;
    disptick_T	tick;
    પૂર્ણાંક		above;
    પૂર્ણાંક		dist;
    પૂર્ણાંક		retval = FALSE;

    જો (syn_block->b_sst_first == ખાલી)
	પાછા retval;

    // Compute normal distance between non-displayed entries.
    જો (syn_block->b_sst_len <= Rows)
	dist = 999999;
    નહીંતર
	dist = syn_buf->b_ml.ml_લીટી_ગણતરી / (syn_block->b_sst_len - Rows) + 1;

    /*
     * Go through the list to find the "tick" for the oldest entry that can
     * be removed.  Set "above" when the "tick" for the oldest entry is above
     * "b_sst_lasttick" (the display tick wraps around).
     */
    tick = syn_block->b_sst_lasttick;
    above = FALSE;
    prev = syn_block->b_sst_first;
    માટે (p = prev->sst_next; p != ખાલી; prev = p, p = p->sst_next)
    {
	જો (prev->sst_lnum + dist > p->sst_lnum)
	{
	    જો (p->sst_tick > syn_block->b_sst_lasttick)
	    {
		જો (!above || p->sst_tick < tick)
		    tick = p->sst_tick;
		above = TRUE;
	    }
	    નહીંતર જો (!above && p->sst_tick < tick)
		tick = p->sst_tick;
	}
    }

    /*
     * Go through the list to make the entries for the oldest tick at an
     * interval of several lines.
     */
    prev = syn_block->b_sst_first;
    માટે (p = prev->sst_next; p != ખાલી; prev = p, p = p->sst_next)
    {
	જો (p->sst_tick == tick && prev->sst_lnum + dist > p->sst_lnum)
	{
	    // Move this entry from used list to free list
	    prev->sst_next = p->sst_next;
	    syn_ઢગલો_free_entry(syn_block, p);
	    p = prev;
	    retval = TRUE;
	}
    }
    પાછા retval;
}

/*
 * Free the allocated memory for a syn_state item.
 * Move the entry into the free list.
 */
    સ્થિર શૂન્ય
syn_ઢગલો_free_entry(synblock_T *block, synstate_T *p)
{
    જો (block->b_sst_શોધ == p)
	block->b_sst_શોધ = ખાલી;
    clear_syn_state(p);
    p->sst_next = block->b_sst_firstfree;
    block->b_sst_firstfree = p;
    ++block->b_sst_freecount;
}

/*
 * Find an entry in the list of state stacks at or before "lnum".
 * Returns NULL when there is no entry or the first entry is after "lnum".
 */
    સ્થિર synstate_T *
syn_ઢગલો_find_entry(linenr_T lnum)
{
    synstate_T	*p, *prev;

    prev = ખાલી;
    p = syn_block->b_sst_first;

    // The list is sorted by line number and lookups while parsing advance
    // monotonically, so resume from the last returned entry instead of
    // rescanning from the start whenever it is at or before "lnum".
    જો (syn_block->b_sst_શોધ != ખાલી
				 && syn_block->b_sst_શોધ->sst_lnum <= lnum)
	p = syn_block->b_sst_શોધ;

    માટે ( ; p != ખાલી; prev = p, p = p->sst_next)
    {
	જો (p->sst_lnum == lnum)
	{
	    syn_block->b_sst_શોધ = p;
	    પાછા p;
	}
	જો (p->sst_lnum > lnum)
	    તોડો;
    }
    જો (prev != ખાલી)
	syn_block->b_sst_શોધ = prev;
    પાછા prev;
}

/*
 * Try saving the current state in b_sst_array[].
 * The current state must be valid for the start of the current_lnum line!
 */
    સ્થિર synstate_T *
store_current_state(શૂન્ય)
{
    પૂર્ણાંક		i;
    synstate_T	*p;
    bufstate_T	*bp;
    stateitem_T	*cur_si;
    synstate_T	*sp = syn_ઢગલો_find_entry(current_lnum);

    /*
     * If the current state contains a start or end pattern that continues
     * from the previous line, we can't use it.  Don't store it then.
     */
    માટે (i = current_state.ga_len - 1; i >= 0; --i)
    {
	cur_si = &CUR_STATE(i);
	જો (cur_si->si_h_startpos.lnum >= current_lnum
		|| cur_si->si_m_endpos.lnum >= current_lnum
		|| cur_si->si_h_endpos.lnum >= current_lnum
		|| (cur_si->si_end_idx
		    && cur_si->si_eoe_pos.lnum >= current_lnum))
	    તોડો;
    }
    જો (i >= 0)
    {
	જો (sp != ખાલી)
	{
	    // find "sp" in the list and remove it
	    જો (syn_block->b_sst_first == sp)
		// it's the first entry
		syn_block->b_sst_first = sp->sst_next;
	    નહીંતર
	    {
		// find the entry just before this one to adjust sst_next
		FOR_ALL_SYNSTATES(syn_block, p)
		    જો (p->sst_next == sp)
			તોડો;
		જો (p != ખાલી)	// just in case
		    p->sst_next = sp->sst_next;
	    }
	    syn_ઢગલો_free_entry(syn_block, sp);
	    sp = ખાલી;
	}
    }
    નહીંતર જો (sp == ખાલી || sp->sst_lnum != current_lnum)
    {
	/*
	 * Add a new entry
	 */
	// If no free items, cleanup the array first.
	જો (syn_block->b_sst_freecount == 0)
	{
	    (શૂન્ય)syn_ઢગલો_cleanup();
	    // "sp" may have been moved to the freelist now
	    sp = syn_ઢગલો_find_entry(current_lnum);
	}
	// Still no free items?  Must be a strange problem...
	જો (syn_block->b_sst_freecount == 0)
	    sp = ખાલી;
	નહીંતર
	{
	    // Take the first item from the free list and put it in the used
	    // list, after *sp
	    p = syn_block->b_sst_firstfree;
	    syn_block->b_sst_firstfree = p->sst_next;
	    --syn_block->b_sst_freecount;
	    જો (sp == ખાલી)
	    {
		// Insert in front of the list
		p->sst_next = syn_block->b_sst_first;
		syn_block->b_sst_first = p;
	    }
	    નહીંતર
	    {
		// insert in list after *sp
		p->sst_next = sp->sst_next;
		sp->sst_next = p;
	    }
	    sp = p;
	    sp->sst_stacksize = 0;
	    sp->sst_lnum = current_lnum;
	    // Resume the next forward lookup from the entry just stored.
	    syn_block->b_sst_શોધ = sp;
	}
    }
    જો (sp != ખાલી)
    {
	// When overwriting an existing state stack, clear it first
	clear_syn_state(sp);
	sp->sst_stacksize = current_state.ga_len;
	જો (current_state.ga_len > SST_FIX_STATES)
	{
	    // Need to clear it, might be something remaining from when the
	    // length was less than SST_FIX_STATES.
	    ga_init2(&sp->sst_union.sst_ga, કદ(bufstate_T), 1);
	    જો (ga_grow(&sp->sst_union.sst_ga, current_state.ga_len) == FAIL)
		sp->sst_stacksize = 0;
	    નહીંતર
		sp->sst_union.sst_ga.ga_len = current_state.ga_len;
	    bp = SYN_STATE_P(&(sp->sst_union.sst_ga));
	}
	નહીંતર
	    bp = sp->sst_union.sst_ઢગલો;
	માટે (i = 0; i < sp->sst_stacksize; ++i)
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
    પાછા sp;
}

/*
 * Copy a state stack from "from" in b_sst_array[] to current_state;
 */
    સ્થિર શૂન્ય
load_current_state(synstate_T *from)
{
    પૂર્ણાંક		i;
    bufstate_T	*bp;

    clear_current_state();
    validate_current_state();
    keepend_level = -1;
    જો (from->sst_stacksize
	    && ga_grow(&current_state, from->sst_stacksize) == OK)
    {
	જો (from->sst_stacksize > SST_FIX_STATES)
	    bp = SYN_STATE_P(&(from->sst_union.sst_ga));
	નહીંતર
	    bp = from->sst_union.sst_ઢગલો;
	માટે (i = 0; i < from->sst_stacksize; ++i)
	{
	    CUR_STATE(i).si_idx = bp[i].bs_idx;
	    CUR_STATE(i).si_flags = bp[i].bs_flags;
#ifdef FEAT_CONCEAL
	    CUR_STATE(i).si_seqnr = bp[i].bs_seqnr;
	    CUR_STATE(i).si_cchar = bp[i].bs_cchar;
#endif
	    CUR_STATE(i).si_extmatch = ref_extmatch(bp[i].bs_extmatch);
	    જો (keepend_level < 0 && (CUR_STATE(i).si_flags & HL_KEEPEND))
		keepend_level = i;
	    CUR_STATE(i).si_ends = FALSE;
	    CUR_STATE(i).si_m_lnum = 0;
	    જો (CUR_STATE(i).si_idx >= 0)
		CUR_STATE(i).si_next_list =
		     (SYN_વસ્તુઓ(syn_block)[CUR_STATE(i).si_idx]).sp_next_list;
	    નહીંતર
		CUR_STATE(i).si_next_list = ખાલી;
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
    સ્થિર પૂર્ણાંક
syn_ઢગલો_equal(synstate_T *sp)
{
    પૂર્ણાંક		i, j;
    bufstate_T	*bp;
    reg_extmatch_T	*six, *bsx;

    // First a quick check if the stacks have the same size end nextlist.
    જો (sp->sst_stacksize != current_state.ga_len
	    || sp->sst_next_list != current_next_list)
	પાછા FALSE;

    // Need to compare all states on both stacks.
    જો (sp->sst_stacksize > SST_FIX_STATES)
	bp = SYN_STATE_P(&(sp->sst_union.sst_ga));
    નહીંતર
	bp = sp->sst_union.sst_ઢગલો;

    માટે (i = current_state.ga_len; --i >= 0; )
    {
	// If the item has another index the state is different.
	જો (bp[i].bs_idx != CUR_STATE(i).si_idx)
	    તોડો;
	જો (bp[i].bs_extmatch == CUR_STATE(i).si_extmatch)
	    ચાલુ;
	// When the extmatch pointers are different, the strings in them can
	// still be the same.  Check if the extmatch references are equal.
	bsx = bp[i].bs_extmatch;
	six = CUR_STATE(i).si_extmatch;
	// If one of the extmatch pointers is NULL the states are different.
	જો (bsx == ખાલી || six == ખાલી)
	    તોડો;
	માટે (j = 0; j < NSUBEXP; ++j)
	{
	    // Check each referenced match string. They must all be equal.
	    જો (bsx->matches[j] != six->matches[j])
	    {
		// If the pointer is different it can still be the same text.
		// Compare the strings, ignore case when the start item has the
		// sp_ic flag set.
		જો (bsx->matches[j] == ખાલી || six->matches[j] == ખાલી)
		    તોડો;
		જો ((SYN_વસ્તુઓ(syn_block)[CUR_STATE(i).si_idx]).sp_ic
			? MB_STRICMP(bsx->matches[j], six->matches[j]) != 0
			: STRCMP(bsx->matches[j], six->matches[j]) != 0)
		    તોડો;
	    }
	}
	જો (j != NSUBEXP)
	    તોડો;
    }
    પાછા i < 0 ? TRUE : FALSE;
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
    શૂન્ય
syntax_end_parsing(win_T *wp, linenr_T lnum)
{
    synstate_T	*sp;

    જો (syn_block != wp->w_s)
	પાછા;  // not the right window
    sp = syn_ઢગલો_find_entry(lnum);
    જો (sp != ખાલી && sp->sst_lnum < lnum)
	sp = sp->sst_next;

    જો (sp != ખાલી && sp->sst_change_lnum != 0)
	sp->sst_change_lnum = lnum;
}

/*
 * End of handling of the state stack.
 ****************************************/

    સ્થિર શૂન્ય
invalidate_current_state(શૂન્ય)
{
    clear_current_state();
    current_state.ga_itemsize = 0;	// mark current_state invalid
    current_next_list = ખાલી;
    keepend_level = -1;
}

    સ્થિર શૂન્ય
validate_current_state(શૂન્ય)
{
    current_state.ga_itemsize = કદ(stateitem_T);
    current_state.ga_growsize = 3;
}

/*
 * Return TRUE if the syntax at start of lnum changed since last time.
 * This will only be called just after get_syntax_attr() for the previous
 * line, to check if the next line needs to be redrawn too.
 */
    પૂર્ણાંક
syntax_check_changed(linenr_T lnum)
{
    પૂર્ણાંક		retval = TRUE;
    synstate_T	*sp;

    /*
     * Check the state stack when:
     * - lnum is just below the previously syntaxed line.
     * - lnum is not before the lines with saved states.
     * - lnum is not past the lines with saved states.
     * - lnum is at or before the last changed line.
     */
    જો (VALID_STATE(&current_state) && lnum == current_lnum + 1)
    {
	sp = syn_ઢગલો_find_entry(lnum);
	જો (sp != ખાલી && sp->sst_lnum == lnum)
	{
	    /*
	     * finish the previous line (needed when not all of the line was
	     * drawn)
	     */
	    (શૂન્ય)syn_finish_લીટી(FALSE);

	    /*
	     * Compare the current state with the previously saved state of
	     * the line.
	     */
	    જો (syn_ઢગલો_equal(sp))
		retval = FALSE;

	    /*
	     * Store the current state in b_sst_array[] for later use.
	     */
	    ++current_lnum;
	    (શૂન્ય)store_current_state();
	}
    }

    પાછા retval;
}

/*
 * Finish the current line.
 * This doesn't return any attributes, it only gets the state at the end of
 * the line.  It can start anywhere in the line, as long as the current state
 * is valid.
 */
    સ્થિર પૂર્ણાંક
syn_finish_લીટી(
    પૂર્ણાંક	    syncing)		// called for syncing
{
    stateitem_T	*cur_si;
    colnr_T	prev_current_col;

    જ્યારે (!current_finished)
    {
	(શૂન્ય)syn_current_attr(syncing, FALSE, ખાલી, FALSE);
	/*
	 * When syncing, and found some item, need to check the item.
	 */
	જો (syncing && current_state.ga_len)
	{
	    /*
	     * Check for match with sync item.
	     */
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	    જો (cur_si->si_idx >= 0
		    && (SYN_વસ્તુઓ(syn_block)[cur_si->si_idx].sp_flags
					  & (HL_SYNC_HERE|HL_SYNC_THERE)))
		પાછા TRUE;

	    // syn_current_attr() will have skipped the check for an item
	    // that ends here, need to do that now.  Be careful not to go
	    // past the NUL.
	    prev_current_col = current_col;
	    જો (syn_getcurline()[current_col] != NUL)
		++current_col;
	    check_state_ends();
	    current_col = prev_current_col;
	}
	++current_col;
    }
    પાછા FALSE;
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
    પૂર્ણાંક
get_syntax_attr(
    colnr_T	col,
    પૂર્ણાંક		*can_spell,
    પૂર્ણાંક		keep_state)	// keep state of char at "col"
{
    પૂર્ણાંક	    attr = 0;

    જો (can_spell != ખાલી)
	// Default: Only do spelling when there is no @Spell cluster or when
	// ":syn spell toplevel" was used.
	*can_spell = syn_block->b_syn_spell == SYNSPL_DEFAULT
		    ? (syn_block->b_spell_cluster_id == 0)
		    : (syn_block->b_syn_spell == SYNSPL_ટોચ);

    // check for out of memory situation
    જો (syn_block->b_sst_array == ખાલી)
	પાછા 0;

    // After 'synmaxcol' the attribute is always zero.
    જો (syn_buf->b_p_smc > 0 && col >= (colnr_T)syn_buf->b_p_smc)
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
	પાછા 0;
    }

    // Make sure current_state is valid
    જો (INVALID_STATE(&current_state))
	validate_current_state();

    /*
     * Skip from the current column to "col", get the attributes for "col".
     */
    જ્યારે (current_col <= col)
    {
	attr = syn_current_attr(FALSE, TRUE, can_spell,
				     current_col == col ? keep_state : FALSE);
	++current_col;
    }

    પાછા attr;
}

/*
 * Get syntax attributes for current_lnum, current_col.
 */
    સ્થિર પૂર્ણાંક
syn_current_attr(
    પૂર્ણાંક		syncing,		// When 1: called for syncing
    પૂર્ણાંક		displaying,		// result will be displayed
    પૂર્ણાંક		*can_spell,		// return: do spell checking
    પૂર્ણાંક		keep_state)		// keep syntax stack afterwards
{
    પૂર્ણાંક		syn_id;
    lpos_T	endpos;		// was: char_u *endp;
    lpos_T	hl_startpos;	// was: int hl_startcol;
    lpos_T	hl_endpos;
    lpos_T	eos_pos;	// end-of-start match (start region)
    lpos_T	eoe_pos;	// end-of-end pattern
    પૂર્ણાંક		end_idx;	// group ID for end pattern
    પૂર્ણાંક		idx;
    synpat_T	*spp;
    stateitem_T	*cur_si, *sip = ખાલી;
    પૂર્ણાંક		startcol;
    પૂર્ણાંક		endcol;
    long	flags;
    પૂર્ણાંક		cchar;
    short	*next_list;
    પૂર્ણાંક		found_match;		    // found usable match
    સ્થિર પૂર્ણાંક	try_next_column = FALSE;    // must try in next col
    પૂર્ણાંક		do_keywords;
    regmmatch_T	regmatch;
    lpos_T	pos;
    પૂર્ણાંક		lc_col;
    reg_extmatch_T *cur_extmatch = ખાલી;
    char_u	buf_chartab[32];  // chartab array for syn iskyeyword
    char_u	*લીટી;		// current line.  NOTE: becomes invalid after
				// looking for a pattern match!

    // variables for zero-width matches that have a "nextgroup" argument
    પૂર્ણાંક		keep_next_list;
    પૂર્ણાંક		zero_width_next_list = FALSE;
    garray_T	zero_width_next_ga;

    /*
     * No character, no attributes!  Past end of line?
     * Do try matching with an empty line (could be the start of a region).
     */
    લીટી = syn_getcurline();
    જો (લીટી[current_col] == NUL && current_col != 0)
    {
	/*
	 * If we found a match after the last column, use it.
	 */
	જો (next_match_idx >= 0 && next_match_col >= (પૂર્ણાંક)current_col
						  && next_match_col != MAXCOL)
	    (શૂન્ય)ધકેલો_next_match(ખાલી);

	current_finished = TRUE;
	current_state_stored = FALSE;
	પાછા 0;
    }

    // if the current or next character is NUL, we will finish the line now
    જો (લીટી[current_col] == NUL || લીટી[current_col + 1] == NUL)
    {
	current_finished = TRUE;
	current_state_stored = FALSE;
    }

    /*
     * When in the previous column there was a match but it could not be used
     * (empty match or already matched in this column) need to try again in
     * the next column.
     */
    જો (try_next_column)
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
    ga_init2(&zero_width_next_ga, કદ(પૂર્ણાંક), 10);

    // use syntax iskeyword option
    save_chartab(buf_chartab);

    /*
     * Repeat matching keywords and patterns, to find contained items at the
     * same column.  This stops when there are no extra matches at the current
     * column.
     */
    કરો
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
	જો (current_state.ga_len)
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	નહીંતર
	    cur_si = ખાલી;

	જો (syn_block->b_syn_containedin || cur_si == ખાલી
					      || cur_si->si_cont_list != ખાલી)
	{
	    /*
	     * 2. Check for keywords, if on a keyword char after a non-keyword
	     *	  char.  Don't do this when syncing.
	     */
	    જો (do_keywords)
	    {
	      લીટી = syn_getcurline();
	      જો (vim_iswordp_buf(લીટી + current_col, syn_buf)
		      && (current_col == 0
			  || !vim_iswordp_buf(લીટી + current_col - 1
			      - (has_mbyte
				  ? (*mb_head_off)(લીટી, લીટી + current_col - 1)
				  : 0) , syn_buf)))
	      {
		syn_id = check_keyword_id(લીટી, (પૂર્ણાંક)current_col,
					 &endcol, &flags, &next_list, cur_si,
					 &cchar);
		જો (syn_id != 0)
		{
		    જો (ધકેલો_current_state(KEYWORD_IDX) == OK)
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
			જો (current_state.ga_len > 1)
			    cur_si->si_flags |=
				  CUR_STATE(current_state.ga_len - 2).si_flags
								 & HL_CONCEAL;
#endif
			cur_si->si_id = syn_id;
			cur_si->si_trans_id = syn_id;
			જો (flags & HL_TRANSP)
			{
			    જો (current_state.ga_len < 2)
			    {
				cur_si->si_attr = 0;
				cur_si->si_trans_id = 0;
			    }
			    નહીંતર
			    {
				cur_si->si_attr = CUR_STATE(
					current_state.ga_len - 2).si_attr;
				cur_si->si_trans_id = CUR_STATE(
					current_state.ga_len - 2).si_trans_id;
			    }
			}
			નહીંતર
			    cur_si->si_attr = syn_id2attr(syn_id);
			cur_si->si_cont_list = ખાલી;
			cur_si->si_next_list = next_list;
			check_keepend();
		    }
		    નહીંતર
			vim_free(next_list);
		}
	      }
	    }

	    /*
	     * 3. Check for patterns (only if no keyword found).
	     */
	    જો (syn_id == 0 && syn_block->b_syn_patterns.ga_len)
	    {
		/*
		 * If we didn't check for a match yet, or we are past it, check
		 * for any match with a pattern.
		 */
		જો (next_match_idx < 0 || next_match_col < (પૂર્ણાંક)current_col)
		{
		    /*
		     * Check all relevant patterns for a match at this
		     * position.  This is complicated, because matching with a
		     * pattern takes quite a bit of time, thus we want to
		     * avoid doing it when it's not needed.
		     */
		    next_match_idx = 0;		// no match in this line yet
		    next_match_col = MAXCOL;
		    માટે (idx = syn_block->b_syn_patterns.ga_len; --idx >= 0; )
		    {
			spp = &(SYN_વસ્તુઓ(syn_block)[idx]);
			જો (	   spp->sp_syncing == syncing
				&& (displaying || !(spp->sp_flags & HL_DISPLAY))
				&& (spp->sp_type == SPTYPE_MATCH
				    || spp->sp_type == SPTYPE_START)
				&& (current_next_list != ખાલી
				    ? in_id_list(ખાલી, current_next_list,
							      &spp->sp_syn, 0)
				    : (cur_si == ખાલી
					? !(spp->sp_flags & HL_CONTAINED)
					: in_id_list(cur_si,
					    cur_si->si_cont_list, &spp->sp_syn,
					    spp->sp_flags))))
			{
			    પૂર્ણાંક r;

			    // If we already tried matching in this line, and
			    // there isn't a match before next_match_col, skip
			    // this item.
			    જો (spp->sp_લીટી_id == current_લીટી_id
				    && spp->sp_startcol >= next_match_col)
				ચાલુ;
			    spp->sp_લીટી_id = current_લીટી_id;

			    lc_col = current_col - spp->sp_offsets[SPO_LC_OFF];
			    જો (lc_col < 0)
				lc_col = 0;

			    regmatch.rmm_ic = spp->sp_ic;
			    regmatch.regprog = spp->sp_prog;
			    r = syn_regexec(&regmatch,
					     current_lnum,
					     (colnr_T)lc_col,
					     IF_SYN_TIME(&spp->sp_time));
			    spp->sp_prog = regmatch.regprog;
			    જો (!r)
			    {
				// no match in this line, try another one
				spp->sp_startcol = MAXCOL;
				ચાલુ;
			    }

			    /*
			     * Compute the first column of the match.
			     */
			    syn_add_start_off(&pos, &regmatch,
							 spp, SPO_MS_OFF, -1);
			    જો (pos.lnum > current_lnum)
			    {
				// must have used end of match in a next line,
				// we can't handle that
				spp->sp_startcol = MAXCOL;
				ચાલુ;
			    }
			    startcol = pos.col;

			    // remember the next column where this pattern
			    // matches in the current line
			    spp->sp_startcol = startcol;

			    /*
			     * If a previously found match starts at a lower
			     * column number, don't use this one.
			     */
			    જો (startcol >= next_match_col)
				ચાલુ;

			    /*
			     * If we matched this pattern at this position
			     * before, skip it.  Must retry in the next
			     * column, because it may match from there.
			     */
			    જો (did_match_already(idx, &zero_width_next_ga))
			    {
				try_next_column = TRUE;
				ચાલુ;
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
			    re_extmatch_out = ખાલી;

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
			    જો (spp->sp_type == SPTYPE_START
					      && (spp->sp_flags & HL_ONELINE))
			    {
				lpos_T	startpos;

				startpos = endpos;
				find_endpos(idx, &startpos, &endpos, &hl_endpos,
				    &flags, &eoe_pos, &end_idx, cur_extmatch);
				જો (endpos.lnum == 0)
				    ચાલુ;	    // not found
			    }

			    /*
			     * For a "match" the size must be > 0 after the
			     * end offset needs has been added.  Except when
			     * syncing.
			     */
			    નહીંતર જો (spp->sp_type == SPTYPE_MATCH)
			    {
				syn_add_end_off(&hl_endpos, &regmatch, spp,
							       SPO_HE_OFF, 0);
				syn_add_end_off(&endpos, &regmatch, spp,
							       SPO_ME_OFF, 0);
				જો (endpos.lnum == current_lnum
				      && (પૂર્ણાંક)endpos.col + syncing < startcol)
				{
				    /*
				     * If an empty string is matched, may need
				     * to try matching again at next column.
				     */
				    જો (regmatch.startpos[0].col
						    == regmatch.endpos[0].col)
					try_next_column = TRUE;
				    ચાલુ;
				}
			    }

			    /*
			     * keep the best match so far in next_match_*
			     */
			    // Highlighting must start after startpos and end
			    // before endpos.
			    જો (hl_startpos.lnum == current_lnum
					   && (પૂર્ણાંક)hl_startpos.col < startcol)
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
			    cur_extmatch = ખાલી;
			}
		    }
		}

		/*
		 * If we found a match at the current column, use it.
		 */
		જો (next_match_idx >= 0 && next_match_col == (પૂર્ણાંક)current_col)
		{
		    synpat_T	*lspp;

		    // When a zero-width item matched which has a nextgroup,
		    // don't push the item but set nextgroup.
		    lspp = &(SYN_વસ્તુઓ(syn_block)[next_match_idx]);
		    જો (next_match_m_endpos.lnum == current_lnum
			    && next_match_m_endpos.col == current_col
			    && lspp->sp_next_list != ખાલી)
		    {
			current_next_list = lspp->sp_next_list;
			current_next_flags = lspp->sp_flags;
			keep_next_list = TRUE;
			zero_width_next_list = TRUE;

			// Add the index to a list, so that we can check
			// later that we don't match it again (and cause an
			// endless loop).
			જો (ga_grow(&zero_width_next_ga, 1) == OK)
			{
			    ((પૂર્ણાંક *)(zero_width_next_ga.ga_data))
				[zero_width_next_ga.ga_len++] = next_match_idx;
			}
			next_match_idx = -1;
		    }
		    નહીંતર
			cur_si = ધકેલો_next_match(cur_si);
		    found_match = TRUE;
		}
	    }
	}

	/*
	 * Handle searching for nextgroup match.
	 */
	જો (current_next_list != ખાલી && !keep_next_list)
	{
	    /*
	     * If a nextgroup was not found, continue looking for one if:
	     * - this is an empty line and the "skipempty" option was given
	     * - we are on white space and the "skipwhite" option was given
	     */
	    જો (!found_match)
	    {
		લીટી = syn_getcurline();
		જો (((current_next_flags & HL_SKIPWHITE)
			    && VIM_ISWHITE(લીટી[current_col]))
			|| ((current_next_flags & HL_SKIPEMPTY)
			    && *લીટી == NUL))
		    તોડો;
	    }

	    /*
	     * If a nextgroup was found: Use it, and continue looking for
	     * contained matches.
	     * If a nextgroup was not found: Continue looking for a normal
	     * match.
	     * When did set current_next_list for a zero-width item and no
	     * match was found don't loop (would get stuck).
	     */
	    current_next_list = ખાલી;
	    next_match_idx = -1;
	    જો (!zero_width_next_list)
		found_match = TRUE;
	}

    } જ્યારે (found_match);

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
    જો (cur_si != ખાલી)
    {
#ifndef FEAT_EVAL
	પૂર્ણાંક	current_trans_id = 0;
#endif
	માટે (idx = current_state.ga_len - 1; idx >= 0; --idx)
	{
	    sip = &CUR_STATE(idx);
	    જો ((current_lnum > sip->si_h_startpos.lnum
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
		તોડો;
	    }
	}

	જો (can_spell != ખાલી)
	{
	    સંરચના sp_syn   sps;

	    /*
	     * set "can_spell" to TRUE if spell checking is supposed to be
	     * done in the current item.
	     */
	    જો (syn_block->b_spell_cluster_id == 0)
	    {
		// There is no @Spell cluster: Do spelling for items without
		// @NoSpell cluster.
		જો (syn_block->b_nospell_cluster_id == 0
						     || current_trans_id == 0)
		    *can_spell = (syn_block->b_syn_spell != SYNSPL_NOTOP);
		નહીંતર
		{
		    sps.inc_tag = 0;
		    sps.id = syn_block->b_nospell_cluster_id;
		    sps.cont_in_list = ખાલી;
		    *can_spell = !in_id_list(sip, sip->si_cont_list, &sps, 0);
		}
	    }
	    નહીંતર
	    {
		// The @Spell cluster is defined: Do spelling in items with
		// the @Spell cluster.  But not when @NoSpell is also there.
		// At the toplevel only spell check when ":syn spell toplevel"
		// was used.
		જો (current_trans_id == 0)
		    *can_spell = (syn_block->b_syn_spell == SYNSPL_ટોચ);
		નહીંતર
		{
		    sps.inc_tag = 0;
		    sps.id = syn_block->b_spell_cluster_id;
		    sps.cont_in_list = ખાલી;
		    *can_spell = in_id_list(sip, sip->si_cont_list, &sps, 0);

		    જો (syn_block->b_nospell_cluster_id != 0)
		    {
			sps.id = syn_block->b_nospell_cluster_id;
			જો (in_id_list(sip, sip->si_cont_list, &sps, 0))
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
	જો (!syncing && !keep_state)
	{
	    check_state_ends();
	    જો (current_state.ga_len > 0
				      && syn_getcurline()[current_col] != NUL)
	    {
		++current_col;
		check_state_ends();
		--current_col;
	    }
	}
    }
    નહીંતર જો (can_spell != ખાલી)
	// Default: Only do spelling when there is no @Spell cluster or when
	// ":syn spell toplevel" was used.
	*can_spell = syn_block->b_syn_spell == SYNSPL_DEFAULT
		    ? (syn_block->b_spell_cluster_id == 0)
		    : (syn_block->b_syn_spell == SYNSPL_ટોચ);

    // nextgroup ends at end of line, unless "skipnl" or "skipempty" present
    જો (current_next_list != ખાલી
	    && (લીટી = syn_getcurline())[current_col] != NUL
	    && લીટી[current_col + 1] == NUL
	    && !(current_next_flags & (HL_SKIPNL | HL_SKIPEMPTY)))
	current_next_list = ખાલી;

    જો (zero_width_next_ga.ga_len > 0)
	ga_clear(&zero_width_next_ga);

    // No longer need external matches.  But keep next_match_extmatch.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = ખાલી;
    unref_extmatch(cur_extmatch);

    પાછા current_attr;
}


/*
 * Check if we already matched pattern "idx" at the current column.
 */
    સ્થિર પૂર્ણાંક
did_match_already(પૂર્ણાંક idx, garray_T *gap)
{
    પૂર્ણાંક		i;

    માટે (i = current_state.ga_len; --i >= 0; )
	જો (CUR_STATE(i).si_m_startcol == (પૂર્ણાંક)current_col
		&& CUR_STATE(i).si_m_lnum == (પૂર્ણાંક)current_lnum
		&& CUR_STATE(i).si_idx == idx)
	    પાછા TRUE;

    // Zero-width matches with a nextgroup argument are not put on the syntax
    // stack, and can only be matched once anyway.
    માટે (i = gap->ga_len; --i >= 0; )
	જો (((પૂર્ણાંક *)(gap->ga_data))[i] == idx)
	    પાછા TRUE;

    પાછા FALSE;
}

/*
 * Push the next match onto the stack.
 */
    સ્થિર stateitem_T *
ધકેલો_next_match(stateitem_T *cur_si)
{
    synpat_T	*spp;
#ifdef FEAT_CONCEAL
    પૂર્ણાંક		 save_flags;
#endif

    spp = &(SYN_વસ્તુઓ(syn_block)[next_match_idx]);

    /*
     * Push the item in current_state stack;
     */
    જો (ધકેલો_current_state(next_match_idx) == OK)
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
	જો (current_state.ga_len > 1)
	    cur_si->si_flags |=
		    CUR_STATE(current_state.ga_len - 2).si_flags & HL_CONCEAL;
#endif
	cur_si->si_next_list = spp->sp_next_list;
	cur_si->si_extmatch = ref_extmatch(next_match_extmatch);
	જો (spp->sp_type == SPTYPE_START && !(spp->sp_flags & HL_ONELINE))
	{
	    // Try to find the end pattern in the current line
	    update_si_end(cur_si, (પૂર્ણાંક)(next_match_m_endpos.col), TRUE);
	    check_keepend();
	}
	નહીંતર
	{
	    cur_si->si_m_endpos = next_match_m_endpos;
	    cur_si->si_h_endpos = next_match_h_endpos;
	    cur_si->si_ends = TRUE;
	    cur_si->si_flags |= next_match_flags;
	    cur_si->si_eoe_pos = next_match_eoe_pos;
	    cur_si->si_end_idx = next_match_end_idx;
	}
	જો (keepend_level < 0 && (cur_si->si_flags & HL_KEEPEND))
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
	જો (	   spp->sp_type == SPTYPE_START
		&& spp->sp_syn_match_id != 0
		&& ધકેલો_current_state(next_match_idx) == OK)
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
	    જો (cur_si->si_flags & HL_CONCEALENDS)
		cur_si->si_flags |= HL_CONCEAL;
#endif
	    cur_si->si_next_list = ખાલી;
	    check_keepend();
	    update_si_attr(current_state.ga_len - 1);
	}
    }

    next_match_idx = -1;	// try other match next time

    પાછા cur_si;
}

/*
 * Check for end of current state (and the states before it).
 */
    સ્થિર શૂન્ય
check_state_ends(શૂન્ય)
{
    stateitem_T	*cur_si;
    પૂર્ણાંક		had_extend;

    cur_si = &CUR_STATE(current_state.ga_len - 1);
    માટે (;;)
    {
	જો (cur_si->si_ends
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
	    જો (cur_si->si_end_idx
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
		જો (cur_si->si_flags & HL_CONCEALENDS)
		    cur_si->si_flags |= HL_CONCEAL;
#endif
		update_si_attr(current_state.ga_len - 1);

		// nextgroup= should not match in the end pattern
		current_next_list = ખાલી;

		// what matches next may be different now, clear it
		next_match_idx = 0;
		next_match_col = MAXCOL;
		તોડો;
	    }

	    // handle next_list, unless at end of line and no "skipnl" or
	    // "skipempty"
	    current_next_list = cur_si->si_next_list;
	    current_next_flags = cur_si->si_flags;
	    જો (!(current_next_flags & (HL_SKIPNL | HL_SKIPEMPTY))
		    && syn_getcurline()[current_col] == NUL)
		current_next_list = ખાલી;

	    // When the ended item has "extend", another item with
	    // "keepend" now needs to check for its end.
	    had_extend = (cur_si->si_flags & HL_EXTEND);

	    કાઢો_current_state();

	    જો (current_state.ga_len == 0)
		તોડો;

	    જો (had_extend && keepend_level >= 0)
	    {
		syn_update_ends(FALSE);
		જો (current_state.ga_len == 0)
		    તોડો;
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
	    જો (cur_si->si_idx >= 0
		    && SYN_વસ્તુઓ(syn_block)[cur_si->si_idx].sp_type
								== SPTYPE_START
		    && !(cur_si->si_flags & (HL_MATCH | HL_KEEPEND)))
	    {
		update_si_end(cur_si, (પૂર્ણાંક)current_col, TRUE);
		check_keepend();
		જો ((current_next_flags & HL_HAS_EOL)
			&& keepend_level < 0
			&& syn_getcurline()[current_col] == NUL)
		    તોડો;
	    }
	}
	નહીંતર
	    તોડો;
    }
}

/*
 * Update an entry in the current_state stack for a match or region.  This
 * fills in si_attr, si_next_list and si_cont_list.
 */
    સ્થિર શૂન્ય
update_si_attr(પૂર્ણાંક idx)
{
    stateitem_T	*sip = &CUR_STATE(idx);
    synpat_T	*spp;

    // This should not happen...
    જો (sip->si_idx < 0)
	પાછા;

    spp = &(SYN_વસ્તુઓ(syn_block)[sip->si_idx]);
    જો (sip->si_flags & HL_MATCH)
	sip->si_id = spp->sp_syn_match_id;
    નહીંતર
	sip->si_id = spp->sp_syn.id;
    sip->si_attr = syn_id2attr(sip->si_id);
    sip->si_trans_id = sip->si_id;
    જો (sip->si_flags & HL_MATCH)
	sip->si_cont_list = ખાલી;
    નહીંતર
	sip->si_cont_list = spp->sp_cont_list;

    /*
     * For transparent items, take attr from outer item.
     * Also take cont_list, if there is none.
     * Don't do this for the matchgroup of a start or end pattern.
     */
    જો ((spp->sp_flags & HL_TRANSP) && !(sip->si_flags & HL_MATCH))
    {
	જો (idx == 0)
	{
	    sip->si_attr = 0;
	    sip->si_trans_id = 0;
	    જો (sip->si_cont_list == ખાલી)
		sip->si_cont_list = ID_LIST_ALL;
	}
	નહીંતર
	{
	    sip->si_attr = CUR_STATE(idx - 1).si_attr;
	    sip->si_trans_id = CUR_STATE(idx - 1).si_trans_id;
	    જો (sip->si_cont_list == ખાલી)
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
    સ્થિર શૂન્ય
check_keepend(શૂન્ય)
{
    પૂર્ણાંક		i;
    lpos_T	maxpos;
    lpos_T	maxpos_h;
    stateitem_T	*sip;

    /*
     * This check can consume a lot of time; only do it from the level where
     * there really is a keepend.
     */
    જો (keepend_level < 0)
	પાછા;

    /*
     * Find the last index of an "extend" item.  "keepend" items before that
     * won't do anything.  If there is no "extend" item "i" will be
     * "keepend_level" and all "keepend" items will work normally.
     */
    માટે (i = current_state.ga_len - 1; i > keepend_level; --i)
	જો (CUR_STATE(i).si_flags & HL_EXTEND)
	    તોડો;

    maxpos.lnum = 0;
    maxpos.col = 0;
    maxpos_h.lnum = 0;
    maxpos_h.col = 0;
    માટે ( ; i < current_state.ga_len; ++i)
    {
	sip = &CUR_STATE(i);
	જો (maxpos.lnum != 0)
	{
	    limit_pos_zero(&sip->si_m_endpos, &maxpos);
	    limit_pos_zero(&sip->si_h_endpos, &maxpos_h);
	    limit_pos_zero(&sip->si_eoe_pos, &maxpos);
	    sip->si_ends = TRUE;
	}
	જો (sip->si_ends && (sip->si_flags & HL_KEEPEND))
	{
	    જો (maxpos.lnum == 0
		    || maxpos.lnum > sip->si_m_endpos.lnum
		    || (maxpos.lnum == sip->si_m_endpos.lnum
			&& maxpos.col > sip->si_m_endpos.col))
		maxpos = sip->si_m_endpos;
	    જો (maxpos_h.lnum == 0
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
    સ્થિર શૂન્ય
update_si_end(
    stateitem_T	*sip,
    પૂર્ણાંક		startcol,   // where to start searching for the end
    પૂર્ણાંક		force)	    // when TRUE overrule a previous end
{
    lpos_T	startpos;
    lpos_T	endpos;
    lpos_T	hl_endpos;
    lpos_T	end_endpos;
    પૂર્ણાંક		end_idx;

    // return quickly for a keyword
    જો (sip->si_idx < 0)
	પાછા;

    // Don't update when it's already done.  Can be a match of an end pattern
    // that started in a previous line.  Watch out: can also be a "keepend"
    // from a containing item.
    જો (!force && sip->si_m_endpos.lnum >= current_lnum)
	પાછા;

    /*
     * We need to find the end of the region.  It may continue in the next
     * line.
     */
    end_idx = 0;
    startpos.lnum = current_lnum;
    startpos.col = startcol;
    find_endpos(sip->si_idx, &startpos, &endpos, &hl_endpos,
		   &(sip->si_flags), &end_endpos, &end_idx, sip->si_extmatch);

    જો (endpos.lnum == 0)
    {
	// No end pattern matched.
	જો (SYN_વસ્તુઓ(syn_block)[sip->si_idx].sp_flags & HL_ONELINE)
	{
	    // a "oneline" never continues in the next line
	    sip->si_ends = TRUE;
	    sip->si_m_endpos.lnum = current_lnum;
	    sip->si_m_endpos.col = syn_getcurline_len();
	}
	નહીંતર
	{
	    // continues in the next line
	    sip->si_ends = FALSE;
	    sip->si_m_endpos.lnum = 0;
	}
	sip->si_h_endpos = sip->si_m_endpos;
    }
    નહીંતર
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
    સ્થિર પૂર્ણાંક
ધકેલો_current_state(પૂર્ણાંક idx)
{
    જો (ga_grow(&current_state, 1) == FAIL)
	પાછા FAIL;
    CLEAR_POINTER(&CUR_STATE(current_state.ga_len));
    CUR_STATE(current_state.ga_len).si_idx = idx;
    ++current_state.ga_len;
    પાછા OK;
}

/*
 * Remove a state from the current_state stack.
 */
    સ્થિર શૂન્ય
કાઢો_current_state(શૂન્ય)
{
    જો (current_state.ga_len)
    {
	unref_extmatch(CUR_STATE(current_state.ga_len - 1).si_extmatch);
	--current_state.ga_len;
    }
    // after the end of a pattern, try matching a keyword or pattern
    next_match_idx = -1;

    // if first state with "keepend" is popped, reset keepend_level
    જો (keepend_level >= current_state.ga_len)
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
    સ્થિર શૂન્ય
find_endpos(
    પૂર્ણાંક		idx,		// index of the pattern
    lpos_T	*startpos,	// where to start looking for an END match
    lpos_T	*m_endpos,	// return: end of match
    lpos_T	*hl_endpos,	// return: end of highlighting
    long	*flagsp,	// return: flags of matching END
    lpos_T	*end_endpos,	// return: end of end pattern match
    પૂર્ણાંક		*end_idx,	// return: group ID for end pat. match, or 0
    reg_extmatch_T *start_ext)	// submatches from the start pattern
{
    colnr_T	matchcol;
    synpat_T	*spp, *spp_skip;
    પૂર્ણાંક		start_idx;
    પૂર્ણાંક		best_idx;
    regmmatch_T	regmatch;
    regmmatch_T	best_regmatch;	    // startpos/endpos of best match
    lpos_T	pos;
    પૂર્ણાંક		had_match = FALSE;
    char_u	buf_chartab[32];  // chartab array for syn option iskyeyword

    // just in case we are invoked for a keyword
    જો (idx < 0)
	પાછા;

    /*
     * Check for being called with a START pattern.
     * Can happen with a match that continues to the next line, because it
     * contained a region.
     */
    spp = &(SYN_વસ્તુઓ(syn_block)[idx]);
    જો (spp->sp_type != SPTYPE_START)
    {
	*hl_endpos = *startpos;
	પાછા;
    }

    /*
     * Find the SKIP or first END pattern after the last START pattern.
     */
    માટે (;;)
    {
	spp = &(SYN_વસ્તુઓ(syn_block)[idx]);
	જો (spp->sp_type != SPTYPE_START)
	    તોડો;
	++idx;
    }

    /*
     *	Lookup the SKIP pattern (if present)
     */
    જો (spp->sp_type == SPTYPE_SKIP)
    {
	spp_skip = spp;
	++idx;
    }
    નહીંતર
	spp_skip = ખાલી;

    // Setup external matches for syn_regexec().
    unref_extmatch(re_extmatch_in);
    re_extmatch_in = ref_extmatch(start_ext);

    matchcol = startpos->col;	// start looking for a match at sstart
    start_idx = idx;		// remember the first END pattern.
    best_regmatch.startpos[0].col = 0;		// avoid compiler warning

    // use syntax iskeyword option
    save_chartab(buf_chartab);

    માટે (;;)
    {
	/*
	 * Find end pattern that matches first after "matchcol".
	 */
	best_idx = -1;
	માટે (idx = start_idx; idx < syn_block->b_syn_patterns.ga_len; ++idx)
	{
	    પૂર્ણાંક lc_col = matchcol;
	    પૂર્ણાંક r;

	    spp = &(SYN_વસ્તુઓ(syn_block)[idx]);
	    જો (spp->sp_type != SPTYPE_END)	// past last END pattern
		તોડો;
	    lc_col -= spp->sp_offsets[SPO_LC_OFF];
	    જો (lc_col < 0)
		lc_col = 0;

	    regmatch.rmm_ic = spp->sp_ic;
	    regmatch.regprog = spp->sp_prog;
	    r = syn_regexec(&regmatch, startpos->lnum, lc_col,
						  IF_SYN_TIME(&spp->sp_time));
	    spp->sp_prog = regmatch.regprog;
	    જો (r)
	    {
		જો (best_idx == -1 || regmatch.startpos[0].col
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
	જો (best_idx == -1)
	    તોડો;

	/*
	 * If the skip pattern matches before the end pattern,
	 * continue searching after the skip pattern.
	 */
	જો (spp_skip != ખાલી)
	{
	    પૂર્ણાંક lc_col = matchcol - spp_skip->sp_offsets[SPO_LC_OFF];
	    પૂર્ણાંક r;

	    જો (lc_col < 0)
		lc_col = 0;
	    regmatch.rmm_ic = spp_skip->sp_ic;
	    regmatch.regprog = spp_skip->sp_prog;
	    r = syn_regexec(&regmatch, startpos->lnum, lc_col,
					      IF_SYN_TIME(&spp_skip->sp_time));
	    spp_skip->sp_prog = regmatch.regprog;
	    જો (r && regmatch.startpos[0].col
					     <= best_regmatch.startpos[0].col)
	    {
		પૂર્ણાંક લીટી_len;

		// Add offset to skip pattern match
		syn_add_end_off(&pos, &regmatch, spp_skip, SPO_ME_OFF, 1);

		// If the skip pattern goes on to the next line, there is no
		// match with an end pattern in this line.
		જો (pos.lnum > startpos->lnum)
		    તોડો;

		લીટી_len = ml_get_buf_len(syn_buf, startpos->lnum);

		// take care of an empty match or negative offset
		જો (pos.col <= matchcol)
		    ++matchcol;
		નહીંતર જો (pos.col <= regmatch.endpos[0].col)
		    matchcol = pos.col;
		નહીંતર
		    // Be careful not to jump over the NUL at the end-of-line
		    માટે (matchcol = regmatch.endpos[0].col;
			    matchcol < લીટી_len && matchcol < pos.col;
								   ++matchcol)
			;

		// if the skip pattern includes end-of-line, break here
		જો (matchcol >= લીટી_len)
		    તોડો;

		ચાલુ;	    // start with first end pattern again
	    }
	}

	/*
	 * Match from start pattern to end pattern.
	 * Correct for match and highlight offset of end pattern.
	 */
	spp = &(SYN_વસ્તુઓ(syn_block)[best_idx]);
	syn_add_end_off(m_endpos, &best_regmatch, spp, SPO_ME_OFF, 1);
	// can't end before the start
	જો (m_endpos->lnum == startpos->lnum && m_endpos->col < startpos->col)
	    m_endpos->col = startpos->col;

	syn_add_end_off(end_endpos, &best_regmatch, spp, SPO_HE_OFF, 1);
	// can't end before the start
	જો (end_endpos->lnum == startpos->lnum
					   && end_endpos->col < startpos->col)
	    end_endpos->col = startpos->col;
	// can't end after the match
	limit_pos(end_endpos, m_endpos);

	/*
	 * If the end group is highlighted differently, adjust the pointers.
	 */
	જો (spp->sp_syn_match_id != spp->sp_syn.id && spp->sp_syn_match_id != 0)
	{
	    *end_idx = best_idx;
	    જો (spp->sp_off_flags & (1 << (SPO_RE_OFF + SPO_ગણતરી)))
	    {
		hl_endpos->lnum = best_regmatch.endpos[0].lnum;
		hl_endpos->col = best_regmatch.endpos[0].col;
	    }
	    નહીંતર
	    {
		hl_endpos->lnum = best_regmatch.startpos[0].lnum;
		hl_endpos->col = best_regmatch.startpos[0].col;
	    }
	    hl_endpos->col += spp->sp_offsets[SPO_RE_OFF];

	    // can't end before the start
	    જો (hl_endpos->lnum == startpos->lnum
					    && hl_endpos->col < startpos->col)
		hl_endpos->col = startpos->col;
	    limit_pos(hl_endpos, m_endpos);

	    // now the match ends where the highlighting ends, it is turned
	    // into the matchgroup for the end
	    *m_endpos = *hl_endpos;
	}
	નહીંતર
	{
	    *end_idx = 0;
	    *hl_endpos = *end_endpos;
	}

	*flagsp = spp->sp_flags;

	had_match = TRUE;
	તોડો;
    }

    // no match for an END pattern in this line
    જો (!had_match)
	m_endpos->lnum = 0;

    restore_chartab(buf_chartab);

    // Remove external matches.
    unref_extmatch(re_extmatch_in);
    re_extmatch_in = ખાલી;
}

/*
 * Limit "pos" not to be after "limit".
 */
    સ્થિર શૂન્ય
limit_pos(lpos_T *pos, lpos_T *limit)
{
    જો (pos->lnum > limit->lnum)
	*pos = *limit;
    નહીંતર જો (pos->lnum == limit->lnum && pos->col > limit->col)
	pos->col = limit->col;
}

/*
 * Limit "pos" not to be after "limit", unless pos->lnum is zero.
 */
    સ્થિર શૂન્ય
limit_pos_zero(
    lpos_T	*pos,
    lpos_T	*limit)
{
    જો (pos->lnum == 0)
	*pos = *limit;
    નહીંતર
	limit_pos(pos, limit);
}

/*
 * Add offset to matched text for end of match or highlight.
 */
    સ્થિર શૂન્ય
syn_add_end_off(
    lpos_T	*result,	// returned position
    regmmatch_T	*regmatch,	// start/end of match
    synpat_T	*spp,		// matched pattern
    પૂર્ણાંક		idx,		// index of offset
    પૂર્ણાંક		extra)		// extra chars for offset to start
{
    પૂર્ણાંક		col;
    પૂર્ણાંક		off;
    char_u	*base;
    char_u	*p;

    જો (spp->sp_off_flags & (1 << idx))
    {
	result->lnum = regmatch->startpos[0].lnum;
	col = regmatch->startpos[0].col;
	off = spp->sp_offsets[idx] + extra;
    }
    નહીંતર
    {
	result->lnum = regmatch->endpos[0].lnum;
	col = regmatch->endpos[0].col;
	off = spp->sp_offsets[idx];
    }
    // Don't go past the end of the line.  Matters for "rs=e+2" when there
    // is a matchgroup. Watch out for match with last NL in the buffer.
    જો (result->lnum > syn_buf->b_ml.ml_લીટી_ગણતરી)
	col = 0;
    નહીંતર જો (off != 0)
    {
	base = ml_get_buf(syn_buf, result->lnum, FALSE);
	p = base + col;
	જો (off > 0)
	{
	    જ્યારે (off-- > 0 && *p != NUL)
		MB_PTR_ADV(p);
	}
	નહીંતર જો (off < 0)
	{
	    જ્યારે (off++ < 0 && base < p)
		MB_PTR_BACK(base, p);
	}
	col = (પૂર્ણાંક)(p - base);
    }
    result->col = col;
}

/*
 * Add offset to matched text for start of match or highlight.
 * Avoid resulting column to become negative.
 */
    સ્થિર શૂન્ય
syn_add_start_off(
    lpos_T	*result,	// returned position
    regmmatch_T	*regmatch,	// start/end of match
    synpat_T	*spp,
    પૂર્ણાંક		idx,
    પૂર્ણાંક		extra)	    // extra chars for offset to end
{
    પૂર્ણાંક		col;
    પૂર્ણાંક		off;
    char_u	*base;
    char_u	*p;

    જો (spp->sp_off_flags & (1 << (idx + SPO_ગણતરી)))
    {
	result->lnum = regmatch->endpos[0].lnum;
	col = regmatch->endpos[0].col;
	off = spp->sp_offsets[idx] + extra;
    }
    નહીંતર
    {
	result->lnum = regmatch->startpos[0].lnum;
	col = regmatch->startpos[0].col;
	off = spp->sp_offsets[idx];
    }
    જો (result->lnum > syn_buf->b_ml.ml_લીટી_ગણતરી)
    {
	// a "\n" at the end of the pattern may take us below the last line
	result->lnum = syn_buf->b_ml.ml_લીટી_ગણતરી;
	col = ml_get_buf_len(syn_buf, result->lnum);
    }
    જો (off != 0)
    {
	base = ml_get_buf(syn_buf, result->lnum, FALSE);
	p = base + col;
	જો (off > 0)
	{
	    જ્યારે (off-- && *p != NUL)
		MB_PTR_ADV(p);
	}
	નહીંતર જો (off < 0)
	{
	    જ્યારે (off++ && base < p)
		MB_PTR_BACK(base, p);
	}
	col = (પૂર્ણાંક)(p - base);
    }
    result->col = col;
}

/*
 * Get current line in syntax buffer.
 */
    સ્થિર char_u *
syn_getcurline(શૂન્ય)
{
    પાછા ml_get_buf(syn_buf, current_lnum, FALSE);
}

/*
 * Get length of current line in syntax buffer.
 */
    સ્થિર colnr_T
syn_getcurline_len(શૂન્ય)
{
    પાછા ml_get_buf_len(syn_buf, current_lnum);
}

/*
 * Call vim_regexec() to find a match with "rmp" in "syn_buf".
 * Returns TRUE when there is a match.
 */
    સ્થિર પૂર્ણાંક
syn_regexec(
    regmmatch_T	*rmp,
    linenr_T	lnum,
    colnr_T	col,
    syn_time_T  *st UNUSED)
{
    પૂર્ણાંક		r;
    પૂર્ણાંક		timed_out = FALSE;
#ifdef FEAT_PROFILE
    proftime_T	pt;

    જો (syn_time_on)
	profile_start(&pt);
#endif

    જો (rmp->regprog == ખાલી)
	// This can happen if a previous call to vim_regexec_multi() tried to
	// use the NFA engine, which resulted in NFA_TOO_EXPENSIVE, and
	// compiling the pattern with the other engine fails.
	પાછા FALSE;

    rmp->rmm_maxcol = syn_buf->b_p_smc;
    r = vim_regexec_multi(rmp, syn_win, syn_buf, lnum, col, &timed_out);

#ifdef FEAT_PROFILE
    જો (syn_time_on)
    {
	profile_end(&pt);
	profile_add(&st->total, &pt);
	જો (profile_cmp(&pt, &st->slowest) < 0)
	    st->slowest = pt;
	++st->ગણતરી;
	જો (r > 0)
	    ++st->match;
    }
#endif
#ifdef FEAT_RELTIME
    જો (timed_out && redrawtime_limit_set && !syn_win->w_s->b_syn_slow)
    {
	syn_win->w_s->b_syn_slow = TRUE;
	સંદેશ(_("'redrawtime' exceeded, syntax highlighting disabled"));
    }
#endif

    જો (r > 0)
    {
	rmp->startpos[0].lnum += lnum;
	rmp->endpos[0].lnum += lnum;
	પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Check one position in a line for a matching keyword.
 * The caller must check if a keyword can start at startcol.
 * Return its ID if found, 0 otherwise.
 */
    સ્થિર પૂર્ણાંક
check_keyword_id(
    char_u	*લીટી,
    પૂર્ણાંક		startcol,	// position in line to check for keyword
    પૂર્ણાંક		*endcolp,	// return: character after found keyword
    long	*flagsp,	// return: flags of matching keyword
    short	**next_listp,	// return: next_list of matching keyword
    stateitem_T	*cur_si,	// item at the top of the stack
    પૂર્ણાંક		*ccharp UNUSED)	// conceal substitution char
{
    keyentry_T	*kp;
    char_u	*kwp;
    પૂર્ણાંક		round;
    પૂર્ણાંક		kwlen;
    char_u	keyword[MAXKEYWLEN + 1]; // assume max. keyword len is 80
    hashtab_T	*ht;
    hashitem_T	*hi;

    // Find first character after the keyword.  First character was already
    // checked.
    kwp = લીટી + startcol;
    kwlen = 0;
    કરો
    {
	જો (has_mbyte)
	    kwlen += (*mb_ptr2len)(kwp + kwlen);
	નહીંતર
	    ++kwlen;
    }
    જ્યારે (vim_iswordp_buf(kwp + kwlen, syn_buf));

    જો (kwlen > MAXKEYWLEN)
	પાછા 0;

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
    માટે (round = 1; round <= 2; ++round)
    {
	ht = round == 1 ? &syn_block->b_keywtab : &syn_block->b_keywtab_ic;
	જો (ht->ht_used == 0)
	    ચાલુ;
	જો (round == 2)	// ignore case
	    (શૂન્ય)str_foldcase(kwp, kwlen, keyword, MAXKEYWLEN + 1);

	/*
	 * Find keywords that match.  There can be several with different
	 * attributes.
	 * When current_next_list is non-zero accept only that group, otherwise:
	 *  Accept a not-contained keyword at toplevel.
	 *  Accept a keyword at other levels only if it is in the contains list.
	 */
	hi = hash_find(ht, keyword);
	જો (!HASHITEM_EMPTY(hi))
	    માટે (kp = HI2KE(hi); kp != ખાલી; kp = kp->ke_next)
	    {
		જો (current_next_list != 0
			? in_id_list(ખાલી, current_next_list, &kp->k_syn, 0)
			: (cur_si == ખાલી
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
		    પાછા kp->k_syn.id;
		}
	    }
    }
    પાછા 0;
}

/*
 * Handle ":syntax conceal" command.
 */
    સ્થિર શૂન્ય
syn_cmd_conceal(exarg_T *eap UNUSED, પૂર્ણાંક syncing UNUSED)
{
#ifdef FEAT_CONCEAL
    char_u	*arg = eap->arg;
    char_u	*next;

    eap->nextcmd = find_nextcmd(arg);
    જો (eap->skip)
	પાછા;

    next = skiptowhite(arg);
    જો (*arg == NUL)
    {
	જો (curwin->w_s->b_syn_conceal)
	    સંદેશ("syntax conceal on");
	નહીંતર
	    સંદેશ("syntax conceal off");
    }
    નહીંતર જો (STRNICMP(arg, "on", 2) == 0 && next - arg == 2)
	curwin->w_s->b_syn_conceal = TRUE;
    નહીંતર જો (STRNICMP(arg, "off", 3) == 0 && next - arg == 3)
	curwin->w_s->b_syn_conceal = FALSE;
    નહીંતર
	semsg(_(e_illegal_argument_str_2), arg);
#endif
}

/*
 * Handle ":syntax case" command.
 */
    સ્થિર શૂન્ય
syn_cmd_case(exarg_T *eap, પૂર્ણાંક syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*next;

    eap->nextcmd = find_nextcmd(arg);
    જો (eap->skip)
	પાછા;

    next = skiptowhite(arg);
    જો (*arg == NUL)
    {
	જો (curwin->w_s->b_syn_ic)
	    સંદેશ("syntax case ignore");
	નહીંતર
	    સંદેશ("syntax case match");
    }
    નહીંતર જો (STRNICMP(arg, "match", 5) == 0 && next - arg == 5)
	curwin->w_s->b_syn_ic = FALSE;
    નહીંતર જો (STRNICMP(arg, "ignore", 6) == 0 && next - arg == 6)
	curwin->w_s->b_syn_ic = TRUE;
    નહીંતર
	semsg(_(e_illegal_argument_str_2), arg);
}

/*
 * Handle ":syntax foldlevel" command.
 */
    સ્થિર શૂન્ય
syn_cmd_foldlevel(exarg_T *eap, પૂર્ણાંક syncing UNUSED)
{
    char_u *arg = eap->arg;
    char_u *arg_end;

    eap->nextcmd = find_nextcmd(arg);
    જો (eap->skip)
	પાછા;

    જો (*arg == NUL)
    {
	પસંદગી (curwin->w_s->b_syn_foldlevel)
	{
	    સ્થિતિ SYNFLD_START:   સંદેશ("syntax foldlevel start");   તોડો;
	    સ્થિતિ SYNFLD_MINIMUM: સંદેશ("syntax foldlevel minimum"); તોડો;
	    મૂળભૂત: તોડો;
	}
	પાછા;
    }

    arg_end = skiptowhite(arg);
    જો (STRNICMP(arg, "start", 5) == 0 && arg_end - arg == 5)
	curwin->w_s->b_syn_foldlevel = SYNFLD_START;
    નહીંતર જો (STRNICMP(arg, "minimum", 7) == 0 && arg_end - arg == 7)
	curwin->w_s->b_syn_foldlevel = SYNFLD_MINIMUM;
    નહીંતર
    {
	semsg(_(e_illegal_argument_str_2), arg);
	પાછા;
    }

    arg = skipwhite(arg_end);
    જો (*arg != NUL)
    {
	semsg(_(e_illegal_argument_str_2), arg);
    }
}

/*
 * Handle ":syntax spell" command.
 */
    સ્થિર શૂન્ય
syn_cmd_spell(exarg_T *eap, પૂર્ણાંક syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*next;

    eap->nextcmd = find_nextcmd(arg);
    જો (eap->skip)
	પાછા;

    next = skiptowhite(arg);
    જો (*arg == NUL)
    {
	જો (curwin->w_s->b_syn_spell == SYNSPL_ટોચ)
	    સંદેશ("syntax spell toplevel");
	નહીંતર જો (curwin->w_s->b_syn_spell == SYNSPL_NOTOP)
	    સંદેશ("syntax spell notoplevel");
	નહીંતર
	    સંદેશ("syntax spell default");
    }
    નહીંતર જો (STRNICMP(arg, "toplevel", 8) == 0 && next - arg == 8)
	curwin->w_s->b_syn_spell = SYNSPL_ટોચ;
    નહીંતર જો (STRNICMP(arg, "notoplevel", 10) == 0 && next - arg == 10)
	curwin->w_s->b_syn_spell = SYNSPL_NOTOP;
    નહીંતર જો (STRNICMP(arg, "default", 7) == 0 && next - arg == 7)
	curwin->w_s->b_syn_spell = SYNSPL_DEFAULT;
    નહીંતર
    {
	semsg(_(e_illegal_argument_str_2), arg);
	પાછા;
    }

    // assume spell checking changed, force a redraw
    redraw_win_later(curwin, UPD_NOT_VALID);
}

/*
 * Handle ":syntax iskeyword" command.
 */
    સ્થિર શૂન્ય
syn_cmd_iskeyword(exarg_T *eap, પૂર્ણાંક syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	save_chartab[32];
    char_u	*save_isk;

    જો (eap->skip)
	પાછા;

    arg = skipwhite(arg);
    જો (*arg == NUL)
    {
	સંદેશ_puts("\n");
	જો (curwin->w_s->b_syn_isk != empty_option)
	{
	    સંદેશ_puts("syntax iskeyword ");
	    સંદેશ_outtrans(curwin->w_s->b_syn_isk);
	}
	નહીંતર
	    સંદેશ_outtrans((char_u *)_("syntax iskeyword not set"));
    }
    નહીંતર
    {
	જો (STRNICMP(arg, "clear", 5) == 0)
	{
	    mch_memmove(curwin->w_s->b_syn_chartab, curbuf->b_chartab,
								  (size_t)32);
	    clear_string_option(&curwin->w_s->b_syn_isk);
	}
	નહીંતર
	{
	    જો (check_isopt(arg) == FAIL)
	    {
		emsg(_(e_invalid_argument));
		પાછા;
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
    શૂન્ય
syntax_clear(synblock_T *block)
{
    પૂર્ણાંક i;

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
    માટે (i = block->b_syn_patterns.ga_len; --i >= 0; )
	syn_clear_pattern(block, i);
    ga_clear(&block->b_syn_patterns);

    // free the syntax clusters
    માટે (i = block->b_syn_clusters.ga_len; --i >= 0; )
	syn_clear_cluster(block, i);
    ga_clear(&block->b_syn_clusters);
    block->b_spell_cluster_id = 0;
    block->b_nospell_cluster_id = 0;

    block->b_syn_sync_flags = 0;
    block->b_syn_sync_minlines = 0;
    block->b_syn_sync_maxlines = 0;
    block->b_syn_sync_linebreaks = 0;

    vim_regfree(block->b_syn_linecont_prog);
    block->b_syn_linecont_prog = ખાલી;
    VIM_CLEAR(block->b_syn_linecont_pat);
#ifdef FEAT_FOLDING
    block->b_syn_folditems = 0;
#endif
    clear_string_option(&block->b_syn_isk);

    // free the stored states
    syn_ઢગલો_free_all(block);
    invalidate_current_state();

    // Reset the counter for ":syn include"
    running_syn_inc_tag = 0;
}

/*
 * Get rid of ownsyntax for window "wp".
 */
    શૂન્ય
reset_synblock(win_T *wp)
{
    જો (wp->w_s != &wp->w_buffer->b_s)
    {
	syntax_clear(wp->w_s);
	vim_free(wp->w_s);
	wp->w_s = &wp->w_buffer->b_s;
    }
}

/*
 * Clear syncing info for one buffer.
 */
    સ્થિર શૂન્ય
syntax_sync_clear(શૂન્ય)
{
    પૂર્ણાંક i;

    // free the syntax patterns
    માટે (i = curwin->w_s->b_syn_patterns.ga_len; --i >= 0; )
	જો (SYN_વસ્તુઓ(curwin->w_s)[i].sp_syncing)
	    syn_remove_pattern(curwin->w_s, i);

    curwin->w_s->b_syn_sync_flags = 0;
    curwin->w_s->b_syn_sync_minlines = 0;
    curwin->w_s->b_syn_sync_maxlines = 0;
    curwin->w_s->b_syn_sync_linebreaks = 0;

    vim_regfree(curwin->w_s->b_syn_linecont_prog);
    curwin->w_s->b_syn_linecont_prog = ખાલી;
    VIM_CLEAR(curwin->w_s->b_syn_linecont_pat);
    clear_string_option(&curwin->w_s->b_syn_isk);

    syn_ઢગલો_free_all(curwin->w_s);	// Need to recompute all syntax.
}

/*
 * Remove one pattern from the buffer's pattern list.
 */
    સ્થિર શૂન્ય
syn_remove_pattern(
    synblock_T	*block,
    પૂર્ણાંક		idx)
{
    synpat_T	*spp;

    spp = &(SYN_વસ્તુઓ(block)[idx]);
#ifdef FEAT_FOLDING
    જો (spp->sp_flags & HL_FOLD)
	--block->b_syn_folditems;
#endif
    syn_clear_pattern(block, idx);
    mch_memmove(spp, spp + 1,
		   કદ(synpat_T) * (block->b_syn_patterns.ga_len - idx - 1));
    --block->b_syn_patterns.ga_len;
}

/*
 * Clear and free one syntax pattern.  When clearing all, must be called from
 * last to first!
 */
    સ્થિર શૂન્ય
syn_clear_pattern(synblock_T *block, પૂર્ણાંક i)
{
    vim_free(SYN_વસ્તુઓ(block)[i].sp_pattern);
    vim_regfree(SYN_વસ્તુઓ(block)[i].sp_prog);
    // Only free sp_cont_list and sp_next_list of first start pattern
    જો (i == 0 || SYN_વસ્તુઓ(block)[i - 1].sp_type != SPTYPE_START)
    {
	vim_free(SYN_વસ્તુઓ(block)[i].sp_cont_list);
	vim_free(SYN_વસ્તુઓ(block)[i].sp_next_list);
	vim_free(SYN_વસ્તુઓ(block)[i].sp_syn.cont_in_list);
    }
}

/*
 * Clear and free one syntax cluster.
 */
    સ્થિર શૂન્ય
syn_clear_cluster(synblock_T *block, પૂર્ણાંક i)
{
    vim_free(SYN_CLSTR(block)[i].scl_name);
    vim_free(SYN_CLSTR(block)[i].scl_name_u);
    vim_free(SYN_CLSTR(block)[i].scl_list);
}

/*
 * Handle ":syntax clear" command.
 */
    સ્થિર શૂન્ય
syn_cmd_clear(exarg_T *eap, પૂર્ણાંક syncing)
{
    char_u	*arg = eap->arg;
    char_u	*arg_end;
    પૂર્ણાંક		id;

    eap->nextcmd = find_nextcmd(arg);
    જો (eap->skip)
	પાછા;

    /*
     * We have to disable this within ":syn include @group filename",
     * because otherwise @group would get deleted.
     * Only required for Vim 5.x syntax files, 6.0 ones don't contain ":syn
     * clear".
     */
    જો (curwin->w_s->b_syn_topgrp != 0)
	પાછા;

    જો (ends_excmd2(eap->cmd, arg))
    {
	/*
	 * No argument: Clear all syntax items.
	 */
	જો (syncing)
	    syntax_sync_clear();
	નહીંતર
	{
	    syntax_clear(curwin->w_s);
	    જો (curwin->w_s == &curwin->w_buffer->b_s)
		do_unlet((char_u *)"b:current_syntax", TRUE);
	    do_unlet((char_u *)"w:current_syntax", TRUE);
	}
    }
    નહીંતર
    {
	/*
	 * Clear the group IDs that are in the argument.
	 */
	જ્યારે (!ends_excmd2(eap->cmd, arg))
	{
	    arg_end = skiptowhite(arg);
	    જો (*arg == '@')
	    {
		id = syn_scl_namen2id(arg + 1, (પૂર્ણાંક)(arg_end - arg - 1));
		જો (id == 0)
		{
		    semsg(_(e_no_such_syntax_cluster_str_1), arg);
		    તોડો;
		}
		નહીંતર
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
	    નહીંતર
	    {
		id = syn_namen2id(arg, (પૂર્ણાંક)(arg_end - arg));
		જો (id == 0)
		{
		    semsg(_(e_no_such_highlight_group_name_str), arg);
		    તોડો;
		}
		નહીંતર
		    syn_clear_one(id, syncing);
	    }
	    arg = skipwhite(arg_end);
	}
    }
    redraw_curbuf_later(UPD_SOME_VALID);
    syn_ઢગલો_free_all(curwin->w_s);		// Need to recompute all syntax.
}

/*
 * Clear one syntax group for the current buffer.
 */
    સ્થિર શૂન્ય
syn_clear_one(પૂર્ણાંક id, પૂર્ણાંક syncing)
{
    synpat_T	*spp;
    પૂર્ણાંક		idx;

    // Clear keywords only when not ":syn sync clear group-name"
    જો (!syncing)
    {
	(શૂન્ય)syn_clear_keyword(id, &curwin->w_s->b_keywtab);
	(શૂન્ય)syn_clear_keyword(id, &curwin->w_s->b_keywtab_ic);
    }

    // clear the patterns for "id"
    માટે (idx = curwin->w_s->b_syn_patterns.ga_len; --idx >= 0; )
    {
	spp = &(SYN_વસ્તુઓ(curwin->w_s)[idx]);
	જો (spp->sp_syn.id != id || spp->sp_syncing != syncing)
	    ચાલુ;
	syn_remove_pattern(curwin->w_s, idx);
    }
}

/*
 * Handle ":syntax on" command.
 */
    સ્થિર શૂન્ય
syn_cmd_on(exarg_T *eap, પૂર્ણાંક syncing UNUSED)
{
    syn_cmd_onoff(eap, "syntax");
}

/*
 * Handle ":syntax enable" command.
 */
    સ્થિર શૂન્ય
syn_cmd_enable(exarg_T *eap, પૂર્ણાંક syncing UNUSED)
{
    set_internal_string_var((char_u *)"g:syntax_cmd", (char_u *)"enable");
    syn_cmd_onoff(eap, "syntax");
    do_unlet((char_u *)"g:syntax_cmd", TRUE);
}

/*
 * Handle ":syntax reset" command.
 * It actually resets highlighting, not syntax.
 */
    સ્થિર શૂન્ય
syn_cmd_reset(exarg_T *eap, પૂર્ણાંક syncing UNUSED)
{
    set_nextcmd(eap, eap->arg);
    જો (!eap->skip)
    {
	set_internal_string_var((char_u *)"g:syntax_cmd", (char_u *)"reset");
	do_cmdline_cmd((char_u *)"runtime! syntax/syncolor.vim");
	do_unlet((char_u *)"g:syntax_cmd", TRUE);
    }
}

/*
 * Handle ":syntax manual" command.
 */
    સ્થિર શૂન્ય
syn_cmd_manual(exarg_T *eap, પૂર્ણાંક syncing UNUSED)
{
    syn_cmd_onoff(eap, "manual");
}

/*
 * Handle ":syntax off" command.
 */
    સ્થિર શૂન્ય
syn_cmd_off(exarg_T *eap, પૂર્ણાંક syncing UNUSED)
{
    syn_cmd_onoff(eap, "nosyntax");
}

    સ્થિર શૂન્ય
syn_cmd_onoff(exarg_T *eap, અક્ષર *name)
{
    char_u	buf[100];

    set_nextcmd(eap, eap->arg);
    જો (!eap->skip)
    {
	STRCPY(buf, "so ");
	vim_snprintf((અક્ષર *)buf + 3, કદ(buf) - 3, SYNTAX_FNAME, name);
	do_cmdline_cmd(buf);
    }
}

/*
 * Handle ":syntax [list]" command: list current syntax words.
 */
    સ્થિર શૂન્ય
syn_cmd_list(
    exarg_T	*eap,
    પૂર્ણાંક		syncing)	    // when TRUE: list syncing items
{
    char_u	*arg = eap->arg;
    પૂર્ણાંક		id;
    char_u	*arg_end;

    eap->nextcmd = find_nextcmd(arg);
    જો (eap->skip)
	પાછા;

    જો (!syntax_present(curwin))
    {
	સંદેશ(_(સંદેશ_no_વસ્તુઓ));
	પાછા;
    }

    જો (syncing)
    {
	જો (curwin->w_s->b_syn_sync_flags & SF_CCOMMENT)
	{
	    સંદેશ_puts(_("syncing on C-style comments"));
	    syn_લીટીઓ_સંદેશ();
	    syn_match_સંદેશ();
	}
	નહીંતર જો (curwin->w_s->b_syn_sync_flags & SF_MATCH)
	{
	    સંદેશ_puts_title(_("\n--- Syntax sync items ---"));
	    જો (curwin->w_s->b_syn_sync_minlines > 0
		|| curwin->w_s->b_syn_sync_maxlines > 0
		|| curwin->w_s->b_syn_sync_linebreaks > 0)
	    {
		સંદેશ_puts(_("\nsyncing on items"));
		syn_લીટીઓ_સંદેશ();
		syn_match_સંદેશ();
	    }
	    માટે (id = 1; id <= highlight_અંક_groups() && !got_int; ++id)
		syn_list_one(id, syncing, FALSE);
	}
	નહીંતર
	{
	    જો (curwin->w_s->b_syn_sync_minlines == 0)
		સંદેશ_puts(_("no syncing"));
	    નહીંતર
	    {
		જો (curwin->w_s->b_syn_sync_minlines == MAXLNUM)
		    સંદેશ_puts(_("syncing starts at the first line"));
		નહીંતર
		{
		    સંદેશ_puts(_("syncing starts "));
		    સંદેશ_outnum(curwin->w_s->b_syn_sync_minlines);
		    સંદેશ_puts(_(" lines before top line"));
		}
		syn_match_સંદેશ();
	    }
	}
	પાછા;
    }

    સંદેશ_puts_title(_("\n--- Syntax items ---"));
    જો (ends_excmd2(eap->cmd, arg))
    {
	/*
	 * No argument: List all group IDs and all syntax clusters.
	 */
	માટે (id = 1; id <= highlight_અંક_groups() && !got_int; ++id)
	    syn_list_one(id, syncing, FALSE);
	માટે (id = 0; id < curwin->w_s->b_syn_clusters.ga_len && !got_int; ++id)
	    syn_list_cluster(id);
    }
    નહીંતર
    {
	/*
	 * List the group IDs and syntax clusters that are in the argument.
	 */
	જ્યારે (!ends_excmd2(eap->cmd, arg) && !got_int)
	{
	    arg_end = skiptowhite(arg);
	    જો (*arg == '@')
	    {
		id = syn_scl_namen2id(arg + 1, (પૂર્ણાંક)(arg_end - arg - 1));
		જો (id == 0)
		    semsg(_(e_no_such_syntax_cluster_str_2), arg);
		નહીંતર
		    syn_list_cluster(id - SYNID_CLUSTER);
	    }
	    નહીંતર
	    {
		id = syn_namen2id(arg, (પૂર્ણાંક)(arg_end - arg));
		જો (id == 0)
		    semsg(_(e_no_such_highlight_group_name_str), arg);
		નહીંતર
		    syn_list_one(id, syncing, TRUE);
	    }
	    arg = skipwhite(arg_end);
	}
    }
    set_nextcmd(eap, arg);
}

    સ્થિર શૂન્ય
syn_લીટીઓ_સંદેશ(શૂન્ય)
{
    જો (curwin->w_s->b_syn_sync_maxlines > 0
				      || curwin->w_s->b_syn_sync_minlines > 0)
    {
	સંદેશ_puts("; ");
	જો (curwin->w_s->b_syn_sync_minlines == MAXLNUM)
	    સંદેશ_puts(_("from the first line"));
	નહીંતર
	{
	    જો (curwin->w_s->b_syn_sync_minlines > 0)
	    {
		સંદેશ_puts(_("minimal "));
		સંદેશ_outnum(curwin->w_s->b_syn_sync_minlines);
		જો (curwin->w_s->b_syn_sync_maxlines)
		    સંદેશ_puts(", ");
	    }
	    જો (curwin->w_s->b_syn_sync_maxlines > 0)
	    {
		સંદેશ_puts(_("maximal "));
		સંદેશ_outnum(curwin->w_s->b_syn_sync_maxlines);
	    }
	    સંદેશ_puts(_(" lines before top line"));
	}
    }
}

    સ્થિર શૂન્ય
syn_match_સંદેશ(શૂન્ય)
{
    જો (curwin->w_s->b_syn_sync_linebreaks > 0)
    {
	સંદેશ_puts(_("; match "));
	સંદેશ_outnum(curwin->w_s->b_syn_sync_linebreaks);
	સંદેશ_puts(_(" line breaks"));
    }
}

સ્થિર પૂર્ણાંક  last_matchgroup;

સ્થિર શૂન્ય syn_list_flags(keyvalue_T *nlist, પૂર્ણાંક nr_entries, પૂર્ણાંક flags, પૂર્ણાંક attr);

/*
 * List one syntax item, for ":syntax" or "syntax list syntax_name".
 */
    સ્થિર શૂન્ય
syn_list_one(
    પૂર્ણાંક		id,
    પૂર્ણાંક		syncing,	    // when TRUE: list syncing items
    પૂર્ણાંક		link_only)	    // when TRUE; list link-only too
{
    પૂર્ણાંક		attr;
    પૂર્ણાંક		idx;
    પૂર્ણાંક		did_header = FALSE;
    synpat_T	*spp;
    સ્થિર keyvalue_T namelist1[] =
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
    સ્થિર keyvalue_T namelist2[] =
		{
		    KEYVALUE_ENTRY(HL_SKIPWHITE, "skipwhite"),
		    KEYVALUE_ENTRY(HL_SKIPNL, "skipnl"),
		    KEYVALUE_ENTRY(HL_SKIPEMPTY, "skipempty")
		};

    attr = HL_ATTR(HLF_D);		// highlight like directories

    // list the keywords for "id"
    જો (!syncing)
    {
	did_header = syn_list_keywords(id, &curwin->w_s->b_keywtab, FALSE, attr);
	did_header = syn_list_keywords(id, &curwin->w_s->b_keywtab_ic,
							    did_header, attr);
    }

    // list the patterns for "id"
    માટે (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len && !got_int; ++idx)
    {
	spp = &(SYN_વસ્તુઓ(curwin->w_s)[idx]);
	જો (spp->sp_syn.id != id || spp->sp_syncing != syncing)
	    ચાલુ;

	(શૂન્ય)syn_list_header(did_header, 999, id);
	did_header = TRUE;
	last_matchgroup = 0;
	જો (spp->sp_type == SPTYPE_MATCH)
	{
	    put_pattern("match", ' ', spp, attr);
	    સંદેશ_putchar(' ');
	}
	નહીંતર જો (spp->sp_type == SPTYPE_START)
	{
	    જ્યારે (SYN_વસ્તુઓ(curwin->w_s)[idx].sp_type == SPTYPE_START)
		put_pattern("start", '=', &SYN_વસ્તુઓ(curwin->w_s)[idx++], attr);
	    જો (SYN_વસ્તુઓ(curwin->w_s)[idx].sp_type == SPTYPE_SKIP)
		put_pattern("skip", '=', &SYN_વસ્તુઓ(curwin->w_s)[idx++], attr);
	    જ્યારે (idx < curwin->w_s->b_syn_patterns.ga_len
			  && SYN_વસ્તુઓ(curwin->w_s)[idx].sp_type == SPTYPE_END)
		put_pattern("end", '=', &SYN_વસ્તુઓ(curwin->w_s)[idx++], attr);
	    --idx;
	    સંદેશ_putchar(' ');
	}
	syn_list_flags(namelist1, (પૂર્ણાંક)ARRAY_LENGTH(namelist1), spp->sp_flags, attr);

	જો (spp->sp_cont_list != ખાલી)
	    put_id_list((char_u *)"contains", spp->sp_cont_list, attr);

	જો (spp->sp_syn.cont_in_list != ખાલી)
	    put_id_list((char_u *)"containedin",
					      spp->sp_syn.cont_in_list, attr);

	જો (spp->sp_next_list != ખાલી)
	{
	    put_id_list((char_u *)"nextgroup", spp->sp_next_list, attr);
	    syn_list_flags(namelist2, (પૂર્ણાંક)ARRAY_LENGTH(namelist2), spp->sp_flags, attr);
	}
	જો (spp->sp_flags & (HL_SYNC_HERE|HL_SYNC_THERE))
	{
	    જો (spp->sp_flags & HL_SYNC_HERE)
		સંદેશ_puts_attr("grouphere", attr);
	    નહીંતર
		સંદેશ_puts_attr("groupthere", attr);
	    સંદેશ_putchar(' ');
	    જો (spp->sp_sync_idx >= 0)
		સંદેશ_outtrans(highlight_group_name(SYN_વસ્તુઓ(curwin->w_s)
				   [spp->sp_sync_idx].sp_syn.id - 1));
	    નહીંતર
		સંદેશ_puts("NONE");
	    સંદેશ_putchar(' ');
	}
    }

    // list the link, if there is one
    જો (highlight_link_id(id - 1) && (did_header || link_only) && !got_int)
    {
	(શૂન્ય)syn_list_header(did_header, 999, id);
	સંદેશ_puts_attr("links to", attr);
	સંદેશ_putchar(' ');
	સંદેશ_outtrans(highlight_group_name(highlight_link_id(id - 1) - 1));
    }
}

    સ્થિર શૂન્ય
syn_list_flags(keyvalue_T *nlist, પૂર્ણાંક nr_entries, પૂર્ણાંક flags, પૂર્ણાંક attr)
{
    પૂર્ણાંક		i;

    માટે (i = 0; i < nr_entries; ++i)
	જો (flags & nlist[i].ચાવી)
	{
	    સંદેશ_puts_attr((અક્ષર *)nlist[i].કિંમત.string, attr);
	    સંદેશ_putchar(' ');
	}
}

/*
 * List one syntax cluster, for ":syntax" or "syntax list syntax_name".
 */
    સ્થિર શૂન્ય
syn_list_cluster(પૂર્ણાંક id)
{
    પૂર્ણાંક	    endcol = 15;

    // slight hack:  roughly duplicate the guts of syn_list_header()
    સંદેશ_putchar('\n');
    સંદેશ_outtrans(SYN_CLSTR(curwin->w_s)[id].scl_name);

    જો (સંદેશ_col >= endcol)	// output at least one space
	endcol = સંદેશ_col + 1;
    જો (cmdline_width <= endcol)	// avoid hang for tiny window
	endcol = cmdline_width - 1;

    સંદેશ_advance(endcol);
    જો (SYN_CLSTR(curwin->w_s)[id].scl_list != ખાલી)
    {
	put_id_list((char_u *)"cluster", SYN_CLSTR(curwin->w_s)[id].scl_list,
		    HL_ATTR(HLF_D));
    }
    નહીંતર
    {
	સંદેશ_puts_attr("cluster", HL_ATTR(HLF_D));
	સંદેશ_puts("=NONE");
    }
}

    સ્થિર શૂન્ય
put_id_list(char_u *name, short *list, પૂર્ણાંક attr)
{
    short		*p;

    સંદેશ_puts_attr((અક્ષર *)name, attr);
    સંદેશ_putchar('=');
    માટે (p = list; *p; ++p)
    {
	જો (*p >= SYNID_ALLBUT && *p < SYNID_ટોચ)
	{
	    જો (p[1])
		સંદેશ_puts("ALLBUT");
	    નહીંતર
		સંદેશ_puts("ALL");
	}
	નહીંતર જો (*p >= SYNID_ટોચ && *p < SYNID_CONTAINED)
	{
	    સંદેશ_puts("TOP");
	}
	નહીંતર જો (*p >= SYNID_CONTAINED && *p < SYNID_CLUSTER)
	{
	    સંદેશ_puts("CONTAINED");
	}
	નહીંતર જો (*p >= SYNID_CLUSTER)
	{
	    short scl_id = *p - SYNID_CLUSTER;

	    સંદેશ_putchar('@');
	    સંદેશ_outtrans(SYN_CLSTR(curwin->w_s)[scl_id].scl_name);
	}
	નહીંતર
	    સંદેશ_outtrans(highlight_group_name(*p - 1));
	જો (p[1])
	    સંદેશ_putchar(',');
    }
    સંદેશ_putchar(' ');
}

    સ્થિર શૂન્ય
put_pattern(
    અક્ષર	*s,
    પૂર્ણાંક		c,
    synpat_T	*spp,
    પૂર્ણાંક		attr)
{
    long	n;
    પૂર્ણાંક		mask;
    પૂર્ણાંક		first;
    સ્થિર અક્ષર	*sepchars = "/+=-#@\"|'^&";
    પૂર્ણાંક		i;

    // May have to write "matchgroup=group"
    જો (last_matchgroup != spp->sp_syn_match_id)
    {
	last_matchgroup = spp->sp_syn_match_id;
	સંદેશ_puts_attr("matchgroup", attr);
	સંદેશ_putchar('=');
	જો (last_matchgroup == 0)
	    સંદેશ_outtrans((char_u *)"NONE");
	નહીંતર
	    સંદેશ_outtrans(highlight_group_name(last_matchgroup - 1));
	સંદેશ_putchar(' ');
    }

    // output the name of the pattern and an '=' or ' '
    સંદેશ_puts_attr(s, attr);
    સંદેશ_putchar(c);

    // output the pattern, in between a char that is not in the pattern
    માટે (i = 0; vim_strchr(spp->sp_pattern, sepchars[i]) != ખાલી; )
	જો (sepchars[++i] == NUL)
	{
	    i = 0;	// no good char found, just use the first one
	    તોડો;
	}
    સંદેશ_putchar(sepchars[i]);
    સંદેશ_outtrans(spp->sp_pattern);
    સંદેશ_putchar(sepchars[i]);

    // output any pattern options
    first = TRUE;
    માટે (i = 0; i < SPO_ગણતરી; ++i)
    {
	mask = (1 << i);
	જો (spp->sp_off_flags & (mask + (mask << SPO_ગણતરી)))
	{
	    જો (!first)
		સંદેશ_putchar(',');	// separate with commas
	    સંદેશ_puts(spo_name_tab[i]);
	    n = spp->sp_offsets[i];
	    જો (i != SPO_LC_OFF)
	    {
		જો (spp->sp_off_flags & mask)
		    સંદેશ_putchar('s');
		નહીંતર
		    સંદેશ_putchar('e');
		જો (n > 0)
		    સંદેશ_putchar('+');
	    }
	    જો (n || i == SPO_LC_OFF)
		સંદેશ_outnum(n);
	    first = FALSE;
	}
    }
    સંદેશ_putchar(' ');
}

/*
 * List or clear the keywords for one syntax group.
 * Return TRUE if the header has been printed.
 */
    સ્થિર પૂર્ણાંક
syn_list_keywords(
    પૂર્ણાંક		id,
    hashtab_T	*ht,
    પૂર્ણાંક		did_header,		// header has already been printed
    પૂર્ણાંક		attr)
{
    પૂર્ણાંક		outlen;
    hashitem_T	*hi;
    keyentry_T	*kp;
    પૂર્ણાંક		todo;
    પૂર્ણાંક		prev_contained = 0;
    short	*prev_next_list = ખાલી;
    short	*prev_cont_in_list = ખાલી;
    પૂર્ણાંક		prev_skipnl = 0;
    પૂર્ણાંક		prev_skipwhite = 0;
    પૂર્ણાંક		prev_skipempty = 0;

    /*
     * Unfortunately, this list of keywords is not sorted on alphabet but on
     * hash value...
     */
    todo = (પૂર્ણાંક)ht->ht_used;
    માટે (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	જો (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    માટે (kp = HI2KE(hi); kp != ખાલી && !got_int; kp = kp->ke_next)
	    {
		જો (kp->k_syn.id == id)
		{
		    જો (prev_contained != (kp->flags & HL_CONTAINED)
			    || prev_skipnl != (kp->flags & HL_SKIPNL)
			    || prev_skipwhite != (kp->flags & HL_SKIPWHITE)
			    || prev_skipempty != (kp->flags & HL_SKIPEMPTY)
			    || prev_cont_in_list != kp->k_syn.cont_in_list
			    || prev_next_list != kp->next_list)
			outlen = 9999;
		    નહીંતર
			outlen = (પૂર્ણાંક)STRLEN(kp->keyword);
		    // output "contained" and "nextgroup" on each line
		    જો (syn_list_header(did_header, outlen, id))
		    {
			prev_contained = 0;
			prev_next_list = ખાલી;
			prev_cont_in_list = ખાલી;
			prev_skipnl = 0;
			prev_skipwhite = 0;
			prev_skipempty = 0;
		    }
		    did_header = TRUE;
		    જો (prev_contained != (kp->flags & HL_CONTAINED))
		    {
			સંદેશ_puts_attr("contained", attr);
			સંદેશ_putchar(' ');
			prev_contained = (kp->flags & HL_CONTAINED);
		    }
		    જો (kp->k_syn.cont_in_list != prev_cont_in_list)
		    {
			put_id_list((char_u *)"containedin",
						kp->k_syn.cont_in_list, attr);
			સંદેશ_putchar(' ');
			prev_cont_in_list = kp->k_syn.cont_in_list;
		    }
		    જો (kp->next_list != prev_next_list)
		    {
			put_id_list((char_u *)"nextgroup", kp->next_list, attr);
			સંદેશ_putchar(' ');
			prev_next_list = kp->next_list;
			જો (kp->flags & HL_SKIPNL)
			{
			    સંદેશ_puts_attr("skipnl", attr);
			    સંદેશ_putchar(' ');
			    prev_skipnl = (kp->flags & HL_SKIPNL);
			}
			જો (kp->flags & HL_SKIPWHITE)
			{
			    સંદેશ_puts_attr("skipwhite", attr);
			    સંદેશ_putchar(' ');
			    prev_skipwhite = (kp->flags & HL_SKIPWHITE);
			}
			જો (kp->flags & HL_SKIPEMPTY)
			{
			    સંદેશ_puts_attr("skipempty", attr);
			    સંદેશ_putchar(' ');
			    prev_skipempty = (kp->flags & HL_SKIPEMPTY);
			}
		    }
		    સંદેશ_outtrans(kp->keyword);
		}
	    }
	}
    }

    પાછા did_header;
}

    સ્થિર શૂન્ય
syn_clear_keyword(પૂર્ણાંક id, hashtab_T *ht)
{
    hashitem_T	*hi;
    keyentry_T	*kp;
    keyentry_T	*kp_prev;
    keyentry_T	*kp_next;
    પૂર્ણાંક		todo;

    hash_lock(ht);
    todo = (પૂર્ણાંક)ht->ht_used;
    FOR_ALL_HASHTAB_વસ્તુઓ(ht, hi, todo)
    {
	જો (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    kp_prev = ખાલી;
	    માટે (kp = HI2KE(hi); kp != ખાલી; )
	    {
		જો (kp->k_syn.id == id)
		{
		    kp_next = kp->ke_next;
		    જો (kp_prev == ખાલી)
		    {
			જો (kp_next == ખાલી)
			    hash_remove(ht, hi, "syntax clear keyword");
			નહીંતર
			    hi->hi_ચાવી = KE2HIKEY(kp_next);
		    }
		    નહીંતર
			kp_prev->ke_next = kp_next;
		    vim_free(kp->next_list);
		    vim_free(kp->k_syn.cont_in_list);
		    vim_free(kp);
		    kp = kp_next;
		}
		નહીંતર
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
    સ્થિર શૂન્ય
clear_keywtab(hashtab_T *ht)
{
    hashitem_T	*hi;
    પૂર્ણાંક		todo;
    keyentry_T	*kp;
    keyentry_T	*kp_next;

    todo = (પૂર્ણાંક)ht->ht_used;
    FOR_ALL_HASHTAB_વસ્તુઓ(ht, hi, todo)
    {
	જો (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    માટે (kp = HI2KE(hi); kp != ખાલી; kp = kp_next)
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
    સ્થિર શૂન્ય
add_keyword(
    char_u	*name,	    // name of keyword
    size_t	namelen,    // length of keyword (excluding the NUL)
    પૂર્ણાંક		id,	    // group ID for this keyword
    પૂર્ણાંક		flags,	    // flags for this keyword
    short	*cont_in_list, // containedin for this keyword
    short	*next_list, // nextgroup for this keyword
    પૂર્ણાંક		conceal_char)
{
    keyentry_T	*kp;
    hashtab_T	*ht;
    hashitem_T	*hi;
    char_u	*name_ic;
    size_t	name_iclen;
    long_u	hash;
    char_u	name_folded[MAXKEYWLEN + 1];

    જો (curwin->w_s->b_syn_ic)
    {
	name_ic = str_foldcase(name, (પૂર્ણાંક)namelen,
						 name_folded, MAXKEYWLEN + 1);
	name_iclen = STRLEN(name_ic);
    }
    નહીંતર
    {
	name_ic = name;
	name_iclen = namelen;
    }
    kp = alloc(offsetof(keyentry_T, keyword) + name_iclen + 1);
    જો (kp == ખાલી)
	પાછા;
    STRCPY(kp->keyword, name_ic);
    kp->k_syn.id = id;
    kp->k_syn.inc_tag = current_syn_inc_tag;
    kp->flags = flags;
    kp->k_char = conceal_char;
    kp->k_syn.cont_in_list = copy_id_list(cont_in_list);
    જો (cont_in_list != ખાલી)
	curwin->w_s->b_syn_containedin = TRUE;
    kp->next_list = copy_id_list(next_list);

    જો (curwin->w_s->b_syn_ic)
	ht = &curwin->w_s->b_keywtab_ic;
    નહીંતર
	ht = &curwin->w_s->b_keywtab;

    hash = hash_hash(kp->keyword);
    hi = hash_lookup(ht, kp->keyword, hash);
    જો (HASHITEM_EMPTY(hi))
    {
	// new keyword, add to hashtable
	kp->ke_next = ખાલી;
	hash_add_item(ht, hi, kp->keyword, hash);
    }
    નહીંતર
    {
	// keyword already exists, prepend to list
	kp->ke_next = HI2KE(hi);
	hi->hi_ચાવી = KE2HIKEY(kp);
    }
}

/*
 * Get the start and end of the group name argument.
 * Return a pointer to the first argument.
 * Return NULL if the end of the command was found instead of further args.
 */
    સ્થિર char_u *
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
    જો (ends_excmd(*arg) || *rest == NUL)
	પાછા ખાલી;
    પાછા rest;
}

/*
 * Check for syntax command option arguments.
 * This can be called at any place in the list of arguments, and just picks
 * out the arguments that are known.  Can be called several times in a row to
 * collect all options in between other arguments.
 * Return a pointer to the next argument (which isn't an option).
 * Return NULL for any error;
 */
    સ્થિર char_u *
get_syn_options(
    char_u	    *start,		// next argument to be checked
    syn_opt_arg_T   *opt,		// various things
    પૂર્ણાંક		    *conceal_char UNUSED,
    પૂર્ણાંક		    skip)		// TRUE if skipping over command
{
    char_u	*arg = start;
    char_u	*gname_start, *gname;
    પૂર્ણાંક		syn_id;
    પૂર્ણાંક		len;
    અક્ષર	*p;
    પૂર્ણાંક		i;
    પૂર્ણાંક		fidx;
    સ્થિર સંરચના flag
    {
	અક્ષર	*name;
	પૂર્ણાંક	argtype;
	પૂર્ણાંક	flags;
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
    સ્થિર અક્ષર *first_letters = "cCoOkKeEtTsSgGdDfFnN";

    જો (arg == ખાલી)		// already detected error
	પાછા ખાલી;

#ifdef FEAT_CONCEAL
    જો (curwin->w_s->b_syn_conceal)
	opt->flags |= HL_CONCEAL;
#endif

    માટે (;;)
    {
	/*
	 * This is used very often when a large number of keywords is defined.
	 * Need to skip quickly when no option name is found.
	 * Also avoid tolower(), it's slow.
	 */
	જો (strchr(first_letters, *arg) == ખાલી)
	    તોડો;

	માટે (fidx = ARRAY_LENGTH(flagtab); --fidx >= 0; )
	{
	    p = flagtab[fidx].name;
	    માટે (i = 0, len = 0; p[i] != NUL; i += 2, ++len)
		જો (arg[len] != p[i] && arg[len] != p[i + 1])
		    તોડો;
	    જો (p[i] == NUL && (VIM_ISWHITE(arg[len])
				    || (flagtab[fidx].argtype > 0
					 ? arg[len] == '='
					 : ends_excmd2(start, arg + len))))
	    {
		જો (opt->keyword
			&& (flagtab[fidx].flags == HL_DISPLAY
			    || flagtab[fidx].flags == HL_FOLD
			    || flagtab[fidx].flags == HL_EXTEND))
		    // treat "display", "fold" and "extend" as a keyword
		    fidx = -1;
		તોડો;
	    }
	}
	જો (fidx < 0)	    // no match found
	    તોડો;

	જો (flagtab[fidx].argtype == 1)
	{
	    જો (!opt->has_cont_list)
	    {
		emsg(_(e_contains_argument_not_accepted_here));
		પાછા ખાલી;
	    }
	    જો (get_id_list(&arg, 8, &opt->cont_list, skip) == FAIL)
		પાછા ખાલી;
	}
	નહીંતર જો (flagtab[fidx].argtype == 2)
	{
	    જો (get_id_list(&arg, 11, &opt->cont_in_list, skip) == FAIL)
		પાછા ખાલી;
	}
	નહીંતર જો (flagtab[fidx].argtype == 3)
	{
	    જો (get_id_list(&arg, 9, &opt->next_list, skip) == FAIL)
		પાછા ખાલી;
	}
	નહીંતર જો (flagtab[fidx].argtype == 11 && arg[5] == '=')
	{
	    // cchar=?
	    જો (has_mbyte)
	    {
#ifdef FEAT_CONCEAL
		*conceal_char = mb_ptr2char(arg + 6);
#endif
		arg += mb_ptr2len(arg + 6) - 1;
	    }
	    નહીંતર
	    {
#ifdef FEAT_CONCEAL
		*conceal_char = arg[6];
#નહીંતર
		;
#endif
	    }
#ifdef FEAT_CONCEAL
	    જો (!vim_isprintc_strict(*conceal_char))
	    {
		emsg(_(e_invalid_cchar_કિંમત));
		પાછા ખાલી;
	    }
#endif
	    arg = skipwhite(arg + 7);
	}
	નહીંતર
	{
	    opt->flags |= flagtab[fidx].flags;
	    arg = skipwhite(arg + len);

	    જો (flagtab[fidx].flags == HL_SYNC_HERE
		    || flagtab[fidx].flags == HL_SYNC_THERE)
	    {
		જો (opt->sync_idx == ખાલી)
		{
		    emsg(_(e_groupthere_not_accepted_here));
		    પાછા ખાલી;
		}
		gname_start = arg;
		arg = skiptowhite(arg);
		જો (gname_start == arg)
		    પાછા ખાલી;
		gname = vim_strnsave(gname_start, arg - gname_start);
		જો (gname == ખાલી)
		    પાછા ખાલી;
		જો (STRCMP(gname, "NONE") == 0)
		    *opt->sync_idx = NONE_IDX;
		નહીંતર
		{
		    syn_id = syn_name2id(gname);
		    માટે (i = curwin->w_s->b_syn_patterns.ga_len; --i >= 0; )
			જો (SYN_વસ્તુઓ(curwin->w_s)[i].sp_syn.id == syn_id
			      && SYN_વસ્તુઓ(curwin->w_s)[i].sp_type
							       == SPTYPE_START)
			{
			    *opt->sync_idx = i;
			    તોડો;
			}
		    જો (i < 0)
		    {
			semsg(_(e_didnt_find_region_item_for_str), gname);
			vim_free(gname);
			પાછા ખાલી;
		    }
		}

		vim_free(gname);
		arg = skipwhite(arg);
	    }
#ifdef FEAT_FOLDING
	    નહીંતર જો (flagtab[fidx].flags == HL_FOLD
						&& foldmethodIsSyntax(curwin))
		// Need to update folds later.
		foldUpdateAll(curwin);
#endif
	}
    }

    પાછા arg;
}

/*
 * Adjustments to syntax item when declared in a ":syn include"'d file.
 * Set the contained flag, and if the item is not already contained, add it
 * to the specified top-level group, if any.
 */
    સ્થિર શૂન્ય
syn_incl_toplevel(પૂર્ણાંક id, પૂર્ણાંક *flagsp)
{
    જો ((*flagsp & HL_CONTAINED) || curwin->w_s->b_syn_topgrp == 0)
	પાછા;
    *flagsp |= HL_CONTAINED | HL_INCLUDED_TOPLEVEL;
    જો (curwin->w_s->b_syn_topgrp >= SYNID_CLUSTER)
    {
	// We have to alloc this, because syn_combine_list() will free it.
	short	    *grp_list = ALLOC_MULT(short, 2);
	પૂર્ણાંક	    tlg_id = curwin->w_s->b_syn_topgrp - SYNID_CLUSTER;

	જો (grp_list != ખાલી)
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
    સ્થિર શૂન્ય
syn_cmd_include(exarg_T *eap, પૂર્ણાંક syncing UNUSED)
{
    char_u	*arg = eap->arg;
    પૂર્ણાંક		sgl_id = 1;
    char_u	*group_name_end;
    char_u	*rest;
    અક્ષર	*errormsg = ખાલી;
    પૂર્ણાંક		prev_toplvl_grp;
    પૂર્ણાંક		prev_syn_inc_tag;
    પૂર્ણાંક		source = FALSE;

    eap->nextcmd = find_nextcmd(arg);
    જો (eap->skip)
	પાછા;

    જો (arg[0] == '@')
    {
	++arg;
	rest = get_group_name(arg, &group_name_end);
	જો (rest == ખાલી)
	{
	    emsg(_(e_filename_required));
	    પાછા;
	}
	sgl_id = syn_check_cluster(arg, (પૂર્ણાંક)(group_name_end - arg));
	જો (sgl_id == 0)
	    પાછા;
	// separate_nextcmd() and expand_filename() depend on this
	eap->arg = rest;
    }

    /*
     * Everything that's left, up to the next command, should be the
     * filename to include.
     */
    eap->argt |= (EX_XFILE | EX_NOSPC);
    separate_nextcmd(eap, FALSE);
    જો (*eap->arg == '<' || *eap->arg == '$' || mch_isFullName(eap->arg))
    {
	// For an absolute path, "$VIM/..." or "<sfile>.." we ":source" the
	// file.  Need to expand the file name first.  In other cases
	// ":runtime!" is used.
	source = TRUE;
	જો (expand_filename(eap, syn_cmdlinep, &errormsg) == FAIL)
	{
	    જો (errormsg != ખાલી)
		emsg(errormsg);
	    પાછા;
	}
    }

    /*
     * Save and restore the existing top-level grouplist id and ":syn
     * include" tag around the actual inclusion.
     */
    જો (running_syn_inc_tag >= MAX_SYN_INC_TAG)
    {
	emsg(_(e_too_many_syntax_includes));
	પાછા;
    }
    prev_syn_inc_tag = current_syn_inc_tag;
    current_syn_inc_tag = ++running_syn_inc_tag;
    prev_toplvl_grp = curwin->w_s->b_syn_topgrp;
    curwin->w_s->b_syn_topgrp = sgl_id;
    જો (source ? do_source(eap->arg, FALSE, DOSO_NONE, ખાલી) == FAIL
				: source_runtime(eap->arg, DIP_ALL) == FAIL)
	semsg(_(e_cant_open_file_str), eap->arg);
    curwin->w_s->b_syn_topgrp = prev_toplvl_grp;
    current_syn_inc_tag = prev_syn_inc_tag;
}

/*
 * Handle ":syntax keyword {group-name} [{option}] keyword .." command.
 */
    સ્થિર શૂન્ય
syn_cmd_keyword(exarg_T *eap, પૂર્ણાંક syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*group_name_end;
    પૂર્ણાંક		syn_id;
    char_u	*rest;
    char_u	*keyword_copy = ખાલી;
    char_u	*p;
    char_u	*kw;
    syn_opt_arg_T syn_opt_arg;
    પૂર્ણાંક		cnt;
    પૂર્ણાંક		conceal_char = NUL;

    rest = get_group_name(arg, &group_name_end);

    જો (rest != ખાલી)
    {
	જો (eap->skip)
	    syn_id = -1;
	નહીંતર
	    syn_id = syn_check_group(arg, (પૂર્ણાંક)(group_name_end - arg));
	જો (syn_id != 0)
	    // allocate a buffer, for removing backslashes in the keyword
	    keyword_copy = alloc(STRLEN(rest) + 1);
	જો (keyword_copy != ખાલી)
	{
	    syn_opt_arg.flags = 0;
	    syn_opt_arg.keyword = TRUE;
	    syn_opt_arg.sync_idx = ખાલી;
	    syn_opt_arg.has_cont_list = FALSE;
	    syn_opt_arg.cont_in_list = ખાલી;
	    syn_opt_arg.next_list = ખાલી;

	    /*
	     * The options given apply to ALL keywords, so all options must be
	     * found before keywords can be created.
	     * 1: collect the options and copy the keywords to keyword_copy.
	     */
	    cnt = 0;
	    p = keyword_copy;
	    માટે ( ; rest != ખાલી && !ends_excmd2(eap->arg, rest);
							rest = skipwhite(rest))
	    {
		rest = get_syn_options(rest, &syn_opt_arg, &conceal_char,
								    eap->skip);
		જો (rest == ખાલી || ends_excmd2(eap->arg, rest))
		    તોડો;
		// Copy the keyword, removing backslashes, and add a NUL.
		જ્યારે (*rest != NUL && !VIM_ISWHITE(*rest))
		{
		    જો (*rest == '\\' && rest[1] != NUL)
			++rest;
		    *p++ = *rest++;
		}
		*p++ = NUL;
		++cnt;
	    }

	    જો (!eap->skip)
	    {
		size_t	kwlen = 0;

		// Adjust flags for use of ":syn include".
		syn_incl_toplevel(syn_id, &syn_opt_arg.flags);

		/*
		 * 2: Add an entry for each keyword.
		 */
		માટે (kw = keyword_copy; --cnt >= 0; kw += kwlen + 1)
		{
		    માટે (p = vim_strchr(kw, '['); ; )
		    {
			જો (p == ખાલી)
			    kwlen = STRLEN(kw);
			નહીંતર
			{
			    *p = NUL;
			    kwlen = (size_t)(p - kw);
			}
			add_keyword(kw, kwlen, syn_id, syn_opt_arg.flags,
				syn_opt_arg.cont_in_list,
					 syn_opt_arg.next_list, conceal_char);
			જો (p == ખાલી)
			    તોડો;
			જો (p[1] == NUL)
			{
			    semsg(_(e_error_missing_rsb_str), kw);
			    જાઓ error;
			}
			જો (p[1] == ']')
			{
			    જો (p[2] != NUL)
			    {
				semsg(_(e_trailing_char_after_rsb_str_str),
								    kw, &p[2]);
				જાઓ error;
			    }
			    kw = p + 1;		// skip over the "]"
			    kwlen = 1;
			    તોડો;
			}
			જો (has_mbyte)
			{
			    પૂર્ણાંક l = (*mb_ptr2len)(p + 1);

			    mch_memmove(p, p + 1, l);
			    p += l;
			}
			નહીંતર
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

    જો (rest != ખાલી)
	set_nextcmd(eap, rest);
    નહીંતર
	semsg(_(e_invalid_argument_str), arg);

    redraw_curbuf_later(UPD_SOME_VALID);
    syn_ઢગલો_free_all(curwin->w_s);		// Need to recompute all syntax.
}

/*
 * Handle ":syntax match {name} [{options}] {pattern} [{options}]".
 *
 * Also ":syntax sync match {name} [[grouphere | groupthere] {group-name}] .."
 */
    સ્થિર શૂન્ય
syn_cmd_match(
    exarg_T	*eap,
    પૂર્ણાંક		syncing)	    // TRUE for ":syntax sync match .. "
{
    char_u	*arg = eap->arg;
    char_u	*group_name_end;
    char_u	*rest;
    synpat_T	item;		// the item found in the line
    પૂર્ણાંક		syn_id;
    પૂર્ણાંક		idx;
    syn_opt_arg_T syn_opt_arg;
    પૂર્ણાંક		sync_idx = 0;
    પૂર્ણાંક		conceal_char = NUL;
    પૂર્ણાંક		orig_called_emsg = called_emsg;

    // Isolate the group name, check for validity
    rest = get_group_name(arg, &group_name_end);

    // Get options before the pattern
    syn_opt_arg.flags = 0;
    syn_opt_arg.keyword = FALSE;
    syn_opt_arg.sync_idx = syncing ? &sync_idx : ખાલી;
    syn_opt_arg.has_cont_list = TRUE;
    syn_opt_arg.cont_list = ખાલી;
    syn_opt_arg.cont_in_list = ખાલી;
    syn_opt_arg.next_list = ખાલી;
    rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);

    // get the pattern.
    init_syn_patterns();
    CLEAR_FIELD(item);
    rest = get_syn_pattern(rest, &item);
    જો (vim_regcomp_had_eol() && !(syn_opt_arg.flags & HL_EXCLUDENL))
	syn_opt_arg.flags |= HL_HAS_EOL;

    // Get options after the pattern
    rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);

    જો (rest != ખાલી)		// all arguments are valid
    {
	/*
	 * Check for trailing command and illegal trailing arguments.
	 */
	set_nextcmd(eap, rest);
	જો (!ends_excmd2(eap->cmd, rest) || eap->skip)
	    rest = ખાલી;
	નહીંતર જો (ga_grow(&curwin->w_s->b_syn_patterns, 1) == OK
		&& (syn_id = syn_check_group(arg,
					   (પૂર્ણાંક)(group_name_end - arg))) != 0)
	{
	    syn_incl_toplevel(syn_id, &syn_opt_arg.flags);
	    /*
	     * Store the pattern in the syn_items list
	     */
	    idx = curwin->w_s->b_syn_patterns.ga_len;
	    SYN_વસ્તુઓ(curwin->w_s)[idx] = item;
	    SYN_વસ્તુઓ(curwin->w_s)[idx].sp_syncing = syncing;
	    SYN_વસ્તુઓ(curwin->w_s)[idx].sp_type = SPTYPE_MATCH;
	    SYN_વસ્તુઓ(curwin->w_s)[idx].sp_syn.id = syn_id;
	    SYN_વસ્તુઓ(curwin->w_s)[idx].sp_syn.inc_tag = current_syn_inc_tag;
	    SYN_વસ્તુઓ(curwin->w_s)[idx].sp_flags = syn_opt_arg.flags;
	    SYN_વસ્તુઓ(curwin->w_s)[idx].sp_sync_idx = sync_idx;
	    SYN_વસ્તુઓ(curwin->w_s)[idx].sp_cont_list = syn_opt_arg.cont_list;
	    SYN_વસ્તુઓ(curwin->w_s)[idx].sp_syn.cont_in_list =
						     syn_opt_arg.cont_in_list;
#ifdef FEAT_CONCEAL
	    SYN_વસ્તુઓ(curwin->w_s)[idx].sp_cchar = conceal_char;
#endif
	    જો (syn_opt_arg.cont_in_list != ખાલી)
		curwin->w_s->b_syn_containedin = TRUE;
	    SYN_વસ્તુઓ(curwin->w_s)[idx].sp_next_list = syn_opt_arg.next_list;
	    ++curwin->w_s->b_syn_patterns.ga_len;

	    // remember that we found a match for syncing on
	    જો (syn_opt_arg.flags & (HL_SYNC_HERE|HL_SYNC_THERE))
		curwin->w_s->b_syn_sync_flags |= SF_MATCH;
#ifdef FEAT_FOLDING
	    જો (syn_opt_arg.flags & HL_FOLD)
		++curwin->w_s->b_syn_folditems;
#endif

	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_ઢગલો_free_all(curwin->w_s);	// Need to recompute all syntax.
	    પાછા;	// don't free the progs and patterns now
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

    જો (rest == ખાલી && called_emsg == orig_called_emsg)
	semsg(_(e_invalid_argument_str), arg);
}

/*
 * Handle ":syntax region {group-name} [matchgroup={group-name}]
 *		start {start} .. [skip {skip}] end {end} .. [{options}]".
 */
    સ્થિર શૂન્ય
syn_cmd_region(
    exarg_T	*eap,
    પૂર્ણાંક		syncing)	    // TRUE for ":syntax sync region .."
{
    char_u		*arg = eap->arg;
    char_u		*group_name_end;
    char_u		*rest;			// next arg, NULL on error
    char_u		*ચાવી_end;
    char_u		*ચાવી = ખાલી;
    char_u		*p;
    પૂર્ણાંક			item;
#define ITEM_START	    0
#define ITEM_SKIP	    1
#define ITEM_END	    2
#define ITEM_MATCHGROUP	    3
    સંરચના pat_ptr
    {
	synpat_T	*pp_synp;		// pointer to syn_pattern
	પૂર્ણાંક		pp_matchgroup_id;	// matchgroup ID
	સંરચના pat_ptr	*pp_next;		// pointer to next pat_ptr
    }			*(pat_ptrs[3]);
					// patterns found in the line
    સંરચના pat_ptr	*ppp;
    સંરચના pat_ptr	*ppp_next;
    પૂર્ણાંક			pat_ગણતરી = 0;		// nr of syn_patterns found
    પૂર્ણાંક			syn_id;
    પૂર્ણાંક			matchgroup_id = 0;
    પૂર્ણાંક			not_enough = FALSE;	// not enough arguments
    પૂર્ણાંક			illegal = FALSE;	// illegal arguments
    પૂર્ણાંક			success = FALSE;
    પૂર્ણાંક			idx;
    syn_opt_arg_T	syn_opt_arg;
    પૂર્ણાંક			conceal_char = NUL;

    // Isolate the group name, check for validity
    rest = get_group_name(arg, &group_name_end);

    pat_ptrs[0] = ખાલી;
    pat_ptrs[1] = ખાલી;
    pat_ptrs[2] = ખાલી;

    init_syn_patterns();

    syn_opt_arg.flags = 0;
    syn_opt_arg.keyword = FALSE;
    syn_opt_arg.sync_idx = ખાલી;
    syn_opt_arg.has_cont_list = TRUE;
    syn_opt_arg.cont_list = ખાલી;
    syn_opt_arg.cont_in_list = ખાલી;
    syn_opt_arg.next_list = ખાલી;

    /*
     * get the options, patterns and matchgroup.
     */
    જ્યારે (rest != ખાલી && !ends_excmd2(eap->cmd, rest))
    {
	// Check for option arguments
	rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);
	જો (rest == ખાલી || ends_excmd2(eap->cmd, rest))
	    તોડો;

	// must be a pattern or matchgroup then
	ચાવી_end = rest;
	જ્યારે (*ચાવી_end && !VIM_ISWHITE(*ચાવી_end) && *ચાવી_end != '=')
	    ++ચાવી_end;
	vim_free(ચાવી);
	ચાવી = vim_strnsave_up(rest, ચાવી_end - rest);
	જો (ચાવી == ખાલી)			// out of memory
	{
	    rest = ખાલી;
	    તોડો;
	}
	જો (STRCMP(ચાવી, "MATCHGROUP") == 0)
	    item = ITEM_MATCHGROUP;
	નહીંતર જો (STRCMP(ચાવી, "START") == 0)
	    item = ITEM_START;
	નહીંતર જો (STRCMP(ચાવી, "END") == 0)
	    item = ITEM_END;
	નહીંતર જો (STRCMP(ચાવી, "SKIP") == 0)
	{
	    જો (pat_ptrs[ITEM_SKIP] != ખાલી)	// one skip pattern allowed
	    {
		illegal = TRUE;
		તોડો;
	    }
	    item = ITEM_SKIP;
	}
	નહીંતર
	    તોડો;
	rest = skipwhite(ચાવી_end);
	જો (*rest != '=')
	{
	    rest = ખાલી;
	    semsg(_(e_missing_equal_str), arg);
	    તોડો;
	}
	rest = skipwhite(rest + 1);
	જો (*rest == NUL)
	{
	    not_enough = TRUE;
	    તોડો;
	}

	જો (item == ITEM_MATCHGROUP)
	{
	    p = skiptowhite(rest);
	    જો ((p - rest == 4 && STRNCMP(rest, "NONE", 4) == 0) || eap->skip)
		matchgroup_id = 0;
	    નહીંતર
	    {
		matchgroup_id = syn_check_group(rest, (પૂર્ણાંક)(p - rest));
		જો (matchgroup_id == 0)
		{
		    illegal = TRUE;
		    તોડો;
		}
	    }
	    rest = skipwhite(p);
	}
	નહીંતર
	{
	    /*
	     * Allocate room for a syn_pattern, and link it in the list of
	     * syn_patterns for this item, at the start (because the list is
	     * used from end to start).
	     */
	    ppp = ALLOC_ONE(સંરચના pat_ptr);
	    જો (ppp == ખાલી)
	    {
		rest = ખાલી;
		તોડો;
	    }
	    ppp->pp_next = pat_ptrs[item];
	    pat_ptrs[item] = ppp;
	    ppp->pp_synp = ALLOC_CLEAR_ONE(synpat_T);
	    જો (ppp->pp_synp == ખાલી)
	    {
		rest = ખાલી;
		તોડો;
	    }

	    /*
	     * Get the syntax pattern and the following offset(s).
	     */
	    // Enable the appropriate \z specials.
	    જો (item == ITEM_START)
		reg_do_extmatch = REX_SET;
	    નહીંતર જો (item == ITEM_SKIP || item == ITEM_END)
		reg_do_extmatch = REX_USE;
	    rest = get_syn_pattern(rest, ppp->pp_synp);
	    reg_do_extmatch = 0;
	    જો (item == ITEM_END && vim_regcomp_had_eol()
				       && !(syn_opt_arg.flags & HL_EXCLUDENL))
		ppp->pp_synp->sp_flags |= HL_HAS_EOL;
	    ppp->pp_matchgroup_id = matchgroup_id;
	    ++pat_ગણતરી;
	}
    }
    vim_free(ચાવી);
    જો (illegal || not_enough)
	rest = ખાલી;

    /*
     * Must have a "start" and "end" pattern.
     */
    જો (rest != ખાલી && (pat_ptrs[ITEM_START] == ખાલી ||
						  pat_ptrs[ITEM_END] == ખાલી))
    {
	not_enough = TRUE;
	rest = ખાલી;
    }

    જો (rest != ખાલી)
    {
	/*
	 * Check for trailing garbage or command.
	 * If OK, add the item.
	 */
	set_nextcmd(eap, rest);
	જો (!ends_excmd(*rest) || eap->skip)
	    rest = ખાલી;
	નહીંતર જો (ga_grow(&(curwin->w_s->b_syn_patterns), pat_ગણતરી) == OK
		&& (syn_id = syn_check_group(arg,
					   (પૂર્ણાંક)(group_name_end - arg))) != 0)
	{
	    syn_incl_toplevel(syn_id, &syn_opt_arg.flags);
	    /*
	     * Store the start/skip/end in the syn_items list
	     */
	    idx = curwin->w_s->b_syn_patterns.ga_len;
	    માટે (item = ITEM_START; item <= ITEM_END; ++item)
	    {
		માટે (ppp = pat_ptrs[item]; ppp != ખાલી; ppp = ppp->pp_next)
		{
		    SYN_વસ્તુઓ(curwin->w_s)[idx] = *(ppp->pp_synp);
		    SYN_વસ્તુઓ(curwin->w_s)[idx].sp_syncing = syncing;
		    SYN_વસ્તુઓ(curwin->w_s)[idx].sp_type =
			    (item == ITEM_START) ? SPTYPE_START :
			    (item == ITEM_SKIP) ? SPTYPE_SKIP : SPTYPE_END;
		    SYN_વસ્તુઓ(curwin->w_s)[idx].sp_flags |= syn_opt_arg.flags;
		    SYN_વસ્તુઓ(curwin->w_s)[idx].sp_syn.id = syn_id;
		    SYN_વસ્તુઓ(curwin->w_s)[idx].sp_syn.inc_tag =
							  current_syn_inc_tag;
		    SYN_વસ્તુઓ(curwin->w_s)[idx].sp_syn_match_id =
							ppp->pp_matchgroup_id;
#ifdef FEAT_CONCEAL
		    SYN_વસ્તુઓ(curwin->w_s)[idx].sp_cchar = conceal_char;
#endif
		    જો (item == ITEM_START)
		    {
			SYN_વસ્તુઓ(curwin->w_s)[idx].sp_cont_list =
							syn_opt_arg.cont_list;
			SYN_વસ્તુઓ(curwin->w_s)[idx].sp_syn.cont_in_list =
						     syn_opt_arg.cont_in_list;
			જો (syn_opt_arg.cont_in_list != ખાલી)
			    curwin->w_s->b_syn_containedin = TRUE;
			SYN_વસ્તુઓ(curwin->w_s)[idx].sp_next_list =
							syn_opt_arg.next_list;
		    }
		    ++curwin->w_s->b_syn_patterns.ga_len;
		    ++idx;
#ifdef FEAT_FOLDING
		    જો (syn_opt_arg.flags & HL_FOLD)
			++curwin->w_s->b_syn_folditems;
#endif
		}
	    }

	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_ઢગલો_free_all(curwin->w_s);	// Need to recompute all syntax.
	    success = TRUE;	    // don't free the progs and patterns now
	}
    }

    /*
     * Free the allocated memory.
     */
    માટે (item = ITEM_START; item <= ITEM_END; ++item)
	માટે (ppp = pat_ptrs[item]; ppp != ખાલી; ppp = ppp_next)
	{
	    જો (!success && ppp->pp_synp != ખાલી)
	    {
		vim_regfree(ppp->pp_synp->sp_prog);
		vim_free(ppp->pp_synp->sp_pattern);
	    }
	    vim_free(ppp->pp_synp);
	    ppp_next = ppp->pp_next;
	    vim_free(ppp);
	}

    જો (!success)
    {
	vim_free(syn_opt_arg.cont_list);
	vim_free(syn_opt_arg.cont_in_list);
	vim_free(syn_opt_arg.next_list);
	જો (not_enough)
	    semsg(_(e_not_enough_arguments_syntax_region_str), arg);
	નહીંતર જો (illegal || rest == ખાલી)
	    semsg(_(e_invalid_argument_str), arg);
    }
}

/*
 * A simple syntax group ID comparison function suitable for use in qsort()
 */
    સ્થિર પૂર્ણાંક
syn_compare_stub(અચલ શૂન્ય *v1, અચલ શૂન્ય *v2)
{
    અચલ short	*s1 = v1;
    અચલ short	*s2 = v2;

    પાછા (*s1 > *s2 ? 1 : *s1 < *s2 ? -1 : 0);
}

/*
 * Combines lists of syntax clusters.
 * *clstr1 and *clstr2 must both be allocated memory; they will be consumed.
 */
    સ્થિર શૂન્ય
syn_combine_list(short **clstr1, short **clstr2, પૂર્ણાંક list_op)
{
    પૂર્ણાંક		count1 = 0;
    પૂર્ણાંક		count2 = 0;
    short	*g1;
    short	*g2;
    short	*clstr = ખાલી;
    પૂર્ણાંક		ગણતરી;
    પૂર્ણાંક		round;

    /*
     * Handle degenerate cases.
     */
    જો (*clstr2 == ખાલી)
	પાછા;
    જો (*clstr1 == ખાલી || list_op == CLUSTER_REPLACE)
    {
	જો (list_op == CLUSTER_REPLACE)
	    vim_free(*clstr1);
	જો (list_op == CLUSTER_REPLACE || list_op == CLUSTER_ADD)
	    *clstr1 = *clstr2;
	નહીંતર
	    vim_free(*clstr2);
	પાછા;
    }

    માટે (g1 = *clstr1; *g1; g1++)
	++count1;
    માટે (g2 = *clstr2; *g2; g2++)
	++count2;

    /*
     * For speed purposes, sort both lists.
     */
    qsort(*clstr1, (size_t)count1, કદ(short), syn_compare_stub);
    qsort(*clstr2, (size_t)count2, કદ(short), syn_compare_stub);

    /*
     * We proceed in two passes; in round 1, we count the elements to place
     * in the new list, and in round 2, we allocate and populate the new
     * list.  For speed, we use a mergesort-like method, adding the smaller
     * of the current elements in each list to the new list.
     */
    માટે (round = 1; round <= 2; round++)
    {
	g1 = *clstr1;
	g2 = *clstr2;
	ગણતરી = 0;

	/*
	 * First, loop through the lists until one of them is empty.
	 */
	જ્યારે (*g1 && *g2)
	{
	    /*
	     * We always want to add from the first list.
	     */
	    જો (*g1 < *g2)
	    {
		જો (round == 2)
		    clstr[ગણતરી] = *g1;
		ગણતરી++;
		g1++;
		ચાલુ;
	    }
	    /*
	     * We only want to add from the second list if we're adding the
	     * lists.
	     */
	    જો (list_op == CLUSTER_ADD)
	    {
		જો (round == 2)
		    clstr[ગણતરી] = *g2;
		ગણતરી++;
	    }
	    જો (*g1 == *g2)
		g1++;
	    g2++;
	}

	/*
	 * Now add the leftovers from whichever list didn't get finished
	 * first.  As before, we only want to add from the second list if
	 * we're adding the lists.
	 */
	માટે (; *g1; g1++, ગણતરી++)
	    જો (round == 2)
		clstr[ગણતરી] = *g1;
	જો (list_op == CLUSTER_ADD)
	    માટે (; *g2; g2++, ગણતરી++)
		જો (round == 2)
		    clstr[ગણતરી] = *g2;

	જો (round == 1)
	{
	    /*
	     * If the group ended up empty, we don't need to allocate any
	     * space for it.
	     */
	    જો (ગણતરી == 0)
	    {
		clstr = ખાલી;
		તોડો;
	    }
	    clstr = ALLOC_MULT(short, ગણતરી + 1);
	    જો (clstr == ખાલી)
		તોડો;
	    clstr[ગણતરી] = 0;
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
    સ્થિર પૂર્ણાંક
syn_scl_name2id(char_u *name)
{
    પૂર્ણાંક		i;
    char_u	*name_u;

    // Avoid using stricmp() too much, it's slow on some systems
    name_u = vim_strsave_up(name);
    જો (name_u == ખાલી)
	પાછા 0;
    માટે (i = curwin->w_s->b_syn_clusters.ga_len; --i >= 0; )
	જો (SYN_CLSTR(curwin->w_s)[i].scl_name_u != ખાલી
		&& STRCMP(name_u, SYN_CLSTR(curwin->w_s)[i].scl_name_u) == 0)
	    તોડો;
    vim_free(name_u);
    પાછા (i < 0 ? 0 : i + SYNID_CLUSTER);
}

/*
 * Like syn_scl_name2id(), but take a pointer + length argument.
 */
    સ્થિર પૂર્ણાંક
syn_scl_namen2id(char_u *linep, પૂર્ણાંક len)
{
    char_u  *name;
    પૂર્ણાંક	    id = 0;

    name = vim_strnsave(linep, len);
    જો (name == ખાલી)
	પાછા 0;

    id = syn_scl_name2id(name);
    vim_free(name);
    પાછા id;
}

/*
 * Find syntax cluster name in the table and return its ID.
 * The argument is a pointer to the name and the length of the name.
 * If it doesn't exist yet, a new entry is created.
 * Return 0 for failure.
 */
    સ્થિર પૂર્ણાંક
syn_check_cluster(char_u *pp, પૂર્ણાંક len)
{
    પૂર્ણાંક		id;
    char_u	*name;

    name = vim_strnsave(pp, len);
    જો (name == ખાલી)
	પાછા 0;

    id = syn_scl_name2id(name);
    જો (id == 0)			// doesn't exist yet
	id = syn_add_cluster(name);
    નહીંતર
	vim_free(name);
    પાછા id;
}

/*
 * Add new syntax cluster and return its ID.
 * "name" must be an allocated string, it will be consumed.
 * Return 0 for failure.
 */
    સ્થિર પૂર્ણાંક
syn_add_cluster(char_u *name)
{
    પૂર્ણાંક		len;

    /*
     * First call for this growarray: init growing array.
     */
    જો (curwin->w_s->b_syn_clusters.ga_data == ખાલી)
    {
	curwin->w_s->b_syn_clusters.ga_itemsize = કદ(syn_cluster_T);
	curwin->w_s->b_syn_clusters.ga_growsize = 10;
    }

    len = curwin->w_s->b_syn_clusters.ga_len;
    જો (len >= MAX_CLUSTER_ID)
    {
	emsg(_(e_too_many_syntax_clusters));
	vim_free(name);
	પાછા 0;
    }

    /*
     * Make room for at least one other cluster entry.
     */
    જો (ga_grow(&curwin->w_s->b_syn_clusters, 1) == FAIL)
    {
	vim_free(name);
	પાછા 0;
    }

    CLEAR_POINTER(&(SYN_CLSTR(curwin->w_s)[len]));
    SYN_CLSTR(curwin->w_s)[len].scl_name = name;
    SYN_CLSTR(curwin->w_s)[len].scl_name_u = vim_strsave_up(name);
    SYN_CLSTR(curwin->w_s)[len].scl_list = ખાલી;
    ++curwin->w_s->b_syn_clusters.ga_len;

    જો (STRICMP(name, "Spell") == 0)
	curwin->w_s->b_spell_cluster_id = len + SYNID_CLUSTER;
    જો (STRICMP(name, "NoSpell") == 0)
	curwin->w_s->b_nospell_cluster_id = len + SYNID_CLUSTER;

    પાછા len + SYNID_CLUSTER;
}

/*
 * Handle ":syntax cluster {cluster-name} [contains={groupname},..]
 *		[add={groupname},..] [remove={groupname},..]".
 */
    સ્થિર શૂન્ય
syn_cmd_cluster(exarg_T *eap, પૂર્ણાંક syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*group_name_end;
    char_u	*rest;
    પૂર્ણાંક		scl_id;
    short	*clstr_list;
    પૂર્ણાંક		got_clstr = FALSE;
    પૂર્ણાંક		opt_len;
    પૂર્ણાંક		list_op;

    eap->nextcmd = find_nextcmd(arg);
    જો (eap->skip)
	પાછા;

    rest = get_group_name(arg, &group_name_end);

    જો (rest != ખાલી)
    {
	scl_id = syn_check_cluster(arg, (પૂર્ણાંક)(group_name_end - arg));
	જો (scl_id == 0)
	    પાછા;
	scl_id -= SYNID_CLUSTER;

	માટે (;;)
	{
	    જો (STRNICMP(rest, "add", 3) == 0
		    && (VIM_ISWHITE(rest[3]) || rest[3] == '='))
	    {
		opt_len = 3;
		list_op = CLUSTER_ADD;
	    }
	    નહીંતર જો (STRNICMP(rest, "remove", 6) == 0
		    && (VIM_ISWHITE(rest[6]) || rest[6] == '='))
	    {
		opt_len = 6;
		list_op = CLUSTER_SUBTRACT;
	    }
	    નહીંતર જો (STRNICMP(rest, "contains", 8) == 0
			&& (VIM_ISWHITE(rest[8]) || rest[8] == '='))
	    {
		opt_len = 8;
		list_op = CLUSTER_REPLACE;
	    }
	    નહીંતર
		તોડો;

	    clstr_list = ખાલી;
	    જો (get_id_list(&rest, opt_len, &clstr_list, eap->skip) == FAIL)
	    {
		semsg(_(e_invalid_argument_str), rest);
		તોડો;
	    }
	    જો (scl_id >= 0)
		syn_combine_list(&SYN_CLSTR(curwin->w_s)[scl_id].scl_list,
			     &clstr_list, list_op);
	    નહીંતર
		vim_free(clstr_list);
	    got_clstr = TRUE;
	}

	જો (got_clstr)
	{
	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_ઢગલો_free_all(curwin->w_s);	// Need to recompute all.
	}
    }

    જો (!got_clstr)
	emsg(_(e_no_cluster_specified));
    જો (rest == ખાલી || !ends_excmd2(eap->cmd, rest))
	semsg(_(e_invalid_argument_str), arg);
}

/*
 * On first call for current buffer: Init growing array.
 */
    સ્થિર શૂન્ય
init_syn_patterns(શૂન્ય)
{
    curwin->w_s->b_syn_patterns.ga_itemsize = કદ(synpat_T);
    curwin->w_s->b_syn_patterns.ga_growsize = 10;
}

/*
 * Get one pattern for a ":syntax match" or ":syntax region" command.
 * Stores the pattern and program in a synpat_T.
 * Returns a pointer to the next argument, or NULL in case of an error.
 */
    સ્થિર char_u *
get_syn_pattern(char_u *arg, synpat_T *ci)
{
    char_u	*end;
    પૂર્ણાંક		*p;
    પૂર્ણાંક		idx;
    char_u	*cpo_save;

    // need at least three chars
    જો (arg == ખાલી || arg[0] == NUL || arg[1] == NUL || arg[2] == NUL)
	પાછા ખાલી;

    end = skip_regexp(arg + 1, *arg, TRUE);
    જો (*end != *arg)			    // end delimiter not found
    {
	semsg(_(e_pattern_delimiter_not_found_str), arg);
	પાછા ખાલી;
    }
    // store the pattern and compiled regexp program
    જો ((ci->sp_pattern = vim_strnsave(arg + 1, end - arg - 1)) == ખાલી)
	પાછા ખાલી;

    // Make 'cpoptions' empty, to avoid the 'l' flag
    cpo_save = p_cpo;
    p_cpo = empty_option;
    ci->sp_prog = vim_regcomp(ci->sp_pattern, RE_MAGIC);
    p_cpo = cpo_save;

    જો (ci->sp_prog == ખાલી)
	પાછા ખાલી;
    ci->sp_ic = curwin->w_s->b_syn_ic;
#ifdef FEAT_PROFILE
    syn_clear_time(&ci->sp_time);
#endif

    /*
     * Check for a match, highlight or region offset.
     */
    ++end;
    કરો
    {
	માટે (idx = SPO_ગણતરી; --idx >= 0; )
	    જો (STRNCMP(end, spo_name_tab[idx], 3) == 0)
		તોડો;
	જો (idx >= 0)
	{
	    p = &(ci->sp_offsets[idx]);
	    જો (idx != SPO_LC_OFF)
		પસંદગી (end[3])
		{
		    સ્થિતિ 's':   તોડો;
		    સ્થિતિ 'b':   તોડો;
		    સ્થિતિ 'e':   idx += SPO_ગણતરી; તોડો;
		    મૂળભૂત:    idx = -1; તોડો;
		}
	    જો (idx >= 0)
	    {
		ci->sp_off_flags |= (1 << idx);
		જો (idx == SPO_LC_OFF)	    // lc=99
		{
		    end += 3;
		    *p = getdigits(&end);

		    // "lc=" offset automatically sets "ms=" offset
		    જો (!(ci->sp_off_flags & (1 << SPO_MS_OFF)))
		    {
			ci->sp_off_flags |= (1 << SPO_MS_OFF);
			ci->sp_offsets[SPO_MS_OFF] = *p;
		    }
		}
		નહીંતર			    // yy=x+99
		{
		    end += 4;
		    જો (*end == '+')
		    {
			++end;
			*p = getdigits(&end);		// positive offset
		    }
		    નહીંતર જો (*end == '-')
		    {
			++end;
			*p = -getdigits(&end);		// negative offset
		    }
		}
		જો (*end != ',')
		    તોડો;
		++end;
	    }
	}
    } જ્યારે (idx >= 0);

    જો (!ends_excmd2(arg, end) && !VIM_ISWHITE(*end))
    {
	semsg(_(e_garbage_after_pattern_str), arg);
	પાછા ખાલી;
    }
    પાછા skipwhite(end);
}

/*
 * Handle ":syntax sync .." command.
 */
    સ્થિર શૂન્ય
syn_cmd_sync(exarg_T *eap, પૂર્ણાંક syncing UNUSED)
{
    char_u	*arg_start = eap->arg;
    char_u	*arg_end;
    char_u	*ચાવી = ખાલી;
    char_u	*next_arg;
    પૂર્ણાંક		illegal = FALSE;
    પૂર્ણાંક		finished = FALSE;
    long	n;
    char_u	*cpo_save;

    જો (ends_excmd2(eap->cmd, arg_start))
    {
	syn_cmd_list(eap, TRUE);
	પાછા;
    }

    જ્યારે (!ends_excmd2(eap->cmd, arg_start))
    {
	arg_end = skiptowhite(arg_start);
	next_arg = skipwhite(arg_end);
	vim_free(ચાવી);
	ચાવી = vim_strnsave_up(arg_start, arg_end - arg_start);
	જો (ચાવી == ખાલી)
	    તોડો;
	જો (STRCMP(ચાવી, "CCOMMENT") == 0)
	{
	    જો (!eap->skip)
		curwin->w_s->b_syn_sync_flags |= SF_CCOMMENT;
	    જો (!ends_excmd2(eap->cmd, next_arg))
	    {
		arg_end = skiptowhite(next_arg);
		જો (!eap->skip)
		    curwin->w_s->b_syn_sync_id = syn_check_group(next_arg,
						   (પૂર્ણાંક)(arg_end - next_arg));
		next_arg = skipwhite(arg_end);
	    }
	    નહીંતર જો (!eap->skip)
		curwin->w_s->b_syn_sync_id = syn_name2id((char_u *)"Comment");
	}
	નહીંતર જો (  STRNCMP(ચાવી, "LINES", 5) == 0
		|| STRNCMP(ચાવી, "MINLINES", 8) == 0
		|| STRNCMP(ચાવી, "MAXLINES", 8) == 0
		|| STRNCMP(ચાવી, "LINEBREAKS", 10) == 0)
	{
	    જો (ચાવી[4] == 'S')
		arg_end = ચાવી + 6;
	    નહીંતર જો (ચાવી[0] == 'L')
		arg_end = ચાવી + 11;
	    નહીંતર
		arg_end = ચાવી + 9;
	    જો (arg_end[-1] != '=' || !VIM_ISDIGIT(*arg_end))
	    {
		illegal = TRUE;
		તોડો;
	    }
	    n = getdigits(&arg_end);
	    જો (!eap->skip)
	    {
		જો (ચાવી[4] == 'B')
		    curwin->w_s->b_syn_sync_linebreaks = n;
		નહીંતર જો (ચાવી[1] == 'A')
		    curwin->w_s->b_syn_sync_maxlines = n;
		નહીંતર
		    curwin->w_s->b_syn_sync_minlines = n;
	    }
	}
	નહીંતર જો (STRCMP(ચાવી, "FROMSTART") == 0)
	{
	    જો (!eap->skip)
	    {
		curwin->w_s->b_syn_sync_minlines = MAXLNUM;
		curwin->w_s->b_syn_sync_maxlines = 0;
	    }
	}
	નહીંતર જો (STRCMP(ચાવી, "LINECONT") == 0)
	{
	    જો (*next_arg == NUL)	   // missing pattern
	    {
		illegal = TRUE;
		તોડો;
	    }
	    જો (curwin->w_s->b_syn_linecont_pat != ખાલી)
	    {
		emsg(_(e_syntax_sync_લીટી_continuations_pattern_specified_twice));
		finished = TRUE;
		તોડો;
	    }
	    arg_end = skip_regexp(next_arg + 1, *next_arg, TRUE);
	    જો (*arg_end != *next_arg)	    // end delimiter not found
	    {
		illegal = TRUE;
		તોડો;
	    }

	    જો (!eap->skip)
	    {
		// store the pattern and compiled regexp program
		જો ((curwin->w_s->b_syn_linecont_pat =
			    vim_strnsave(next_arg + 1,
				      arg_end - next_arg - 1)) == ખાલી)
		{
		    finished = TRUE;
		    તોડો;
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

		જો (curwin->w_s->b_syn_linecont_prog == ખાલી)
		{
		    VIM_CLEAR(curwin->w_s->b_syn_linecont_pat);
		    finished = TRUE;
		    તોડો;
		}
	    }
	    next_arg = skipwhite(arg_end + 1);
	}
	નહીંતર
	{
	    eap->arg = next_arg;
	    જો (STRCMP(ચાવી, "MATCH") == 0)
		syn_cmd_match(eap, TRUE);
	    નહીંતર જો (STRCMP(ચાવી, "REGION") == 0)
		syn_cmd_region(eap, TRUE);
	    નહીંતર જો (STRCMP(ચાવી, "CLEAR") == 0)
		syn_cmd_clear(eap, TRUE);
	    નહીંતર
		illegal = TRUE;
	    finished = TRUE;
	    તોડો;
	}
	arg_start = next_arg;
    }
    vim_free(ચાવી);
    જો (illegal)
	semsg(_(e_illegal_arguments_str), arg_start);
    નહીંતર જો (!finished)
    {
	set_nextcmd(eap, arg_start);
	redraw_curbuf_later(UPD_SOME_VALID);
	syn_ઢગલો_free_all(curwin->w_s);	// Need to recompute all syntax.
    }
}

/*
 * Convert a line of highlight group names into a list of group ID numbers.
 * "arg" should point to the "contains" or "nextgroup" keyword.
 * "arg" is advanced to after the last group name.
 * Careful: the argument is modified (NULs added).
 * returns FAIL for some error, OK for success.
 */
    સ્થિર પૂર્ણાંક
get_id_list(
    char_u	**arg,
    પૂર્ણાંક		keylen,		// length of keyword
    short	**list,		// where to store the resulting list, if not
				// NULL, the list is silently skipped!
    પૂર્ણાંક		skip)
{
    char_u	*p = ખાલી;
    char_u	*end;
    પૂર્ણાંક		round;
    પૂર્ણાંક		ગણતરી;
    પૂર્ણાંક		total_ગણતરી = 0;
    short	*retval = ખાલી;
    char_u	*name;
    regmatch_T	regmatch;
    પૂર્ણાંક		id;
    પૂર્ણાંક		i;
    પૂર્ણાંક		failed = FALSE;

    /*
     * We parse the list twice:
     * round == 1: count the number of items, allocate the array.
     * round == 2: fill the array with the items.
     * In round 1 new groups may be added, causing the number of items to
     * grow when a regexp is used.  In that case round 1 is done once again.
     */
    માટે (round = 1; round <= 2; ++round)
    {
	/*
	 * skip "contains"
	 */
	p = skipwhite(*arg + keylen);
	જો (*p != '=')
	{
	    semsg(_(e_missing_equal_sign_str), *arg);
	    તોડો;
	}
	p = skipwhite(p + 1);
	જો (ends_excmd2(*arg, p))
	{
	    semsg(_(e_empty_argument_str), *arg);
	    તોડો;
	}

	/*
	 * parse the arguments after "contains"
	 */
	ગણતરી = 0;
	જ્યારે (!ends_excmd2(*arg, p))
	{
	    માટે (end = p; *end && !VIM_ISWHITE(*end) && *end != ','; ++end)
		;
	    name = alloc(end - p + 3);	    // leave room for "^$"
	    જો (name == ખાલી)
	    {
		failed = TRUE;
		તોડો;
	    }
	    vim_strncpy(name + 1, p, end - p);
	    જો (       STRCMP(name + 1, "ALLBUT") == 0
		    || STRCMP(name + 1, "ALL") == 0
		    || STRCMP(name + 1, "TOP") == 0
		    || STRCMP(name + 1, "CONTAINED") == 0)
	    {
		જો (TOUPPER_ASC(**arg) != 'C')
		{
		    semsg(_(e_str_not_allowed_here), name + 1);
		    failed = TRUE;
		    vim_free(name);
		    તોડો;
		}
		જો (ગણતરી != 0)
		{
		    semsg(_(e_str_must_be_first_in_contains_list), name + 1);
		    failed = TRUE;
		    vim_free(name);
		    તોડો;
		}
		જો (name[1] == 'A')
		    id = SYNID_ALLBUT;
		નહીંતર જો (name[1] == 'T')
		    id = SYNID_ટોચ;
		નહીંતર
		    id = SYNID_CONTAINED;
		id += current_syn_inc_tag;
	    }
	    નહીંતર જો (name[1] == '@')
	    {
		જો (skip)
		    id = -1;
		નહીંતર
		    id = syn_check_cluster(name + 2, (પૂર્ણાંક)(end - p - 1));
	    }
	    નહીંતર
	    {
		/*
		 * Handle full group name.
		 */
		જો (vim_strpbrk(name + 1, (char_u *)"\\.*^$~[") == ખાલી)
		    id = syn_check_group(name + 1, (પૂર્ણાંક)(end - p));
		નહીંતર
		{
		    /*
		     * Handle match of regexp with group names.
		     */
		    *name = '^';
		    STRCAT(name, "$");
		    regmatch.regprog = vim_regcomp(name, RE_MAGIC);
		    જો (regmatch.regprog == ખાલી)
		    {
			failed = TRUE;
			vim_free(name);
			તોડો;
		    }

		    regmatch.rm_ic = TRUE;
		    id = 0;
		    માટે (i = highlight_અંક_groups(); --i >= 0; )
		    {
			જો (vim_regexec(&regmatch, highlight_group_name(i),
								  (colnr_T)0))
			{
			    જો (round == 2)
			    {
				// Got more items than expected; can happen
				// when adding items that match:
				// "contains=a.*b,axb".
				// Go back to first round
				જો (ગણતરી >= total_ગણતરી)
				{
				    vim_free(retval);
				    round = 1;
				}
				નહીંતર
				    retval[ગણતરી] = i + 1;
			    }
			    ++ગણતરી;
			    id = -1;	    // remember that we found one
			}
		    }
		    vim_regfree(regmatch.regprog);
		}
	    }
	    vim_free(name);
	    જો (id == 0)
	    {
		semsg(_(e_unknown_group_name_str), p);
		failed = TRUE;
		તોડો;
	    }
	    જો (id > 0)
	    {
		જો (round == 2)
		{
		    // Got more items than expected, go back to first round
		    જો (ગણતરી >= total_ગણતરી)
		    {
			vim_free(retval);
			round = 1;
		    }
		    નહીંતર
			retval[ગણતરી] = id;
		}
		++ગણતરી;
	    }
	    p = skipwhite(end);
	    જો (*p != ',')
		તોડો;
	    p = skipwhite(p + 1);	// skip comma in between arguments
	}
	જો (failed)
	    તોડો;
	જો (round == 1)
	{
	    retval = ALLOC_MULT(short, ગણતરી + 1);
	    જો (retval == ખાલી)
		તોડો;
	    retval[ગણતરી] = 0;	    // zero means end of the list
	    total_ગણતરી = ગણતરી;
	}
    }

    *arg = p;
    જો (failed || retval == ખાલી)
    {
	vim_free(retval);
	પાછા FAIL;
    }

    જો (*list == ખાલી)
	*list = retval;
    નહીંતર
	vim_free(retval);	// list already found, don't overwrite it

    પાછા OK;
}

/*
 * Make a copy of an ID list.
 */
    સ્થિર short *
copy_id_list(short *list)
{
    પૂર્ણાંક	    len;
    પૂર્ણાંક	    ગણતરી;
    short   *retval;

    જો (list == ખાલી)
	પાછા ખાલી;

    માટે (ગણતરી = 0; list[ગણતરી]; ++ગણતરી)
	;
    len = (ગણતરી + 1) * કદ(short);
    retval = alloc(len);
    જો (retval != ખાલી)
	mch_memmove(retval, list, (size_t)len);

    પાછા retval;
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

પ્રકારવ્યાખ્યા સંરચના
{
    short	*idl_ચાવી;	// the list this entry resolves (NULL when empty)
    short	*idl_ids;	// sorted, cluster-expanded group IDs (allocated)
    પૂર્ણાંક		idl_ગણતરી;	// number of IDs in idl_ids
    short	idl_marker;	// leading ALLBUT/TOP/CONTAINED item, or 0
    bool	idl_usable;	// false: not cacheable, use the slow path
} idl_entry_T;

પ્રકારવ્યાખ્યા સંરચના
{
    idl_entry_T	ent[IDL_CACHE_SIZE];
    પૂર્ણાંક		round;		// round-robin replacement index
    પૂર્ણાંક		last;		// index of the last returned entry (fast path)
} idl_cache_T;

/*
 * Collect the explicit group IDs of "list" into "ga", expanding clusters.
 * "seen" records visited clusters to break cycles and avoid duplicate work.
 * Return false when the list cannot be represented as a plain set (a nested
 * ALLBUT/TOP/CONTAINED marker) or on out of memory.
 */
    સ્થિર bool
idl_collect(short *list, garray_T *ga, garray_T *seen)
{
    જો (list == ખાલી || list == ID_LIST_ALL)
	પાછા false;
    માટે (short item = *list; item != 0; item = *++list)
    {
	જો (item >= SYNID_ALLBUT && item < SYNID_CLUSTER)
	    પાછા false;	// nested ALLBUT/TOP/CONTAINED: not cacheable
	જો (item >= SYNID_CLUSTER)
	{
	    short	*scl_list;
	    bool	seen_it = false;

	    માટે (પૂર્ણાંક i = 0; i < seen->ga_len; ++i)
		જો (((short *)seen->ga_data)[i] == item)
		{
		    seen_it = true;
		    તોડો;
		}
	    જો (seen_it)
		ચાલુ;
	    જો (ga_grow(seen, 1) == FAIL)
		પાછા false;
	    ((short *)seen->ga_data)[seen->ga_len++] = item;
	    // Record the cluster id itself, not just its members: the slow
	    // path matches "item == id" for a cluster id present in the list.
	    જો (ga_grow(ga, 1) == FAIL)
		પાછા false;
	    ((short *)ga->ga_data)[ga->ga_len++] = item;
	    scl_list = SYN_CLSTR(syn_block)[item - SYNID_CLUSTER].scl_list;
	    જો (scl_list != ખાલી && !idl_collect(scl_list, ga, seen))
		પાછા false;
	}
	નહીંતર
	{
	    જો (ga_grow(ga, 1) == FAIL)
		પાછા false;
	    ((short *)ga->ga_data)[ga->ga_len++] = item;
	}
    }
    પાછા true;
}

    સ્થિર પૂર્ણાંક
idl_id_cmp(અચલ શૂન્ય *a, અચલ શૂન્ય *b)
{
    પાછા (પૂર્ણાંક)*(અચલ short *)a - (પૂર્ણાંક)*(અચલ short *)b;
}

/*
 * Get the cache entry that resolves "list", building it if needed.
 * Returns NULL only on allocation failure.
 */
    સ્થિર idl_entry_T *
idl_get_entry(short *list)
{
    idl_cache_T	*cache = (idl_cache_T *)syn_block->b_idlist_cache;
    idl_entry_T	*e;
    garray_T	ga, seen;

    જો (cache == ખાલી)
    {
	cache = ALLOC_CLEAR_ONE(idl_cache_T);
	જો (cache == ખાલી)
	    પાછા ખાલી;
	syn_block->b_idlist_cache = cache;
    }
    // Fast path: the same list is usually queried many times in a row.
    જો (cache->ent[cache->last].idl_ચાવી == list)
	પાછા &cache->ent[cache->last];
    માટે (પૂર્ણાંક i = 0; i < IDL_CACHE_SIZE; ++i)
	જો (cache->ent[i].idl_ચાવી == list)
	{
	    cache->last = i;
	    પાછા &cache->ent[i];
	}

    // Build a new entry, replacing one in round-robin order.
    cache->last = cache->round;
    e = &cache->ent[cache->round];
    cache->round = (cache->round + 1) % IDL_CACHE_SIZE;
    VIM_CLEAR(e->idl_ids);
    e->idl_ગણતરી = 0;
    e->idl_ચાવી = list;
    e->idl_marker = 0;
    e->idl_usable = true;

    // A leading ALLBUT/TOP/CONTAINED marker is kept aside; the rest is the set.
    જો (*list >= SYNID_ALLBUT && *list < SYNID_CLUSTER)
    {
	e->idl_marker = *list;
	++list;
    }

    ga_init2(&ga, કદ(short), 50);
    ga_init2(&seen, કદ(short), 10);
    જો (!idl_collect(list, &ga, &seen))
    {
	e->idl_usable = false;
	ga_clear(&ga);
	ga_clear(&seen);
	પાછા e;
    }
    ga_clear(&seen);
    જો (ga.ga_len > 0)
    {
	qsort(ga.ga_data, (size_t)ga.ga_len, કદ(short), idl_id_cmp);
	e->idl_ids = (short *)ga.ga_data;	// take ownership
	e->idl_ગણતરી = ga.ga_len;
    }
    નહીંતર
	ga_clear(&ga);
    પાછા e;
}

/*
 * Return true if "id" is in the resolved set of cache entry "e".
 */
    સ્થિર bool
idl_contains(idl_entry_T *e, short id)
{
    પૂર્ણાંક	lo = 0;
    પૂર્ણાંક	hi = e->idl_ગણતરી - 1;

    જ્યારે (lo <= hi)
    {
	પૂર્ણાંક	mid = (lo + hi) / 2;
	short	v = e->idl_ids[mid];

	જો (v == id)
	    પાછા true;
	જો (v < id)
	    lo = mid + 1;
	નહીંતર
	    hi = mid - 1;
    }
    પાછા false;
}

    સ્થિર શૂન્ય
idl_cache_clear(synblock_T *block)
{
    idl_cache_T	*cache = (idl_cache_T *)block->b_idlist_cache;

    જો (cache == ખાલી)
	પાછા;
    માટે (પૂર્ણાંક i = 0; i < IDL_CACHE_SIZE; ++i)
	vim_free(cache->ent[i].idl_ids);
    vim_free(cache);
    block->b_idlist_cache = ખાલી;
}
// }}}

/*
 * Check if syntax group "ssp" is in the ID list "list" of "cur_si".
 * "cur_si" can be NULL if not checking the "containedin" list.
 * Used to check if a syntax item is in the "contains" or "nextgroup" list of
 * the current item.
 * This function is called very often, keep it fast!!
 */
    સ્થિર પૂર્ણાંક
in_id_list(
    stateitem_T	*cur_si,	// current item or NULL
    short	*list,		// id list
    સંરચના sp_syn *ssp,		// group id and ":syn include" tag of group
    પૂર્ણાંક		flags)		// group flags
{
    પૂર્ણાંક		retval;
    short	*scl_list;
    short	item;
    short	id = ssp->id;
    સ્થિર પૂર્ણાંક	depth = 0;
    પૂર્ણાંક		r;
    પૂર્ણાંક		toplevel;
    idl_entry_T	*e;

    // If ssp has a "containedin" list and "cur_si" is in it, return TRUE.
    જો (cur_si != ખાલી && ssp->cont_in_list != ખાલી
					    && !(cur_si->si_flags & HL_MATCH))
    {
	// Ignore transparent items without a contains argument.  Double check
	// that we don't go back past the first one.
	જ્યારે ((cur_si->si_flags & HL_TRANS_CONT)
		&& cur_si > (stateitem_T *)(current_state.ga_data))
	    --cur_si;
	// cur_si->si_idx is -1 for keywords, these never contain anything.
	જો (cur_si->si_idx >= 0 && in_id_list(ખાલી, ssp->cont_in_list,
		&(SYN_વસ્તુઓ(syn_block)[cur_si->si_idx].sp_syn),
		  SYN_વસ્તુઓ(syn_block)[cur_si->si_idx].sp_flags))
	    પાછા TRUE;
    }

    જો (list == ખાલી)
	પાછા FALSE;

    /*
     * If list is ID_LIST_ALL, we are in a transparent item that isn't
     * inside anything.  Only allow not-contained groups.
     */
    જો (list == ID_LIST_ALL)
	પાછા !(flags & HL_CONTAINED);

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
    e = disable_syn_idlist_cache_for_testing ? ખાલી : idl_get_entry(list);
    જો (e != ખાલી && e->idl_usable)
    {
	જો (e->idl_marker != 0)
	{
	    item = e->idl_marker;
	    જો (item < SYNID_ટોચ)
	    {
		જો (item - SYNID_ALLBUT != ssp->inc_tag)
		    પાછા FALSE;
	    }
	    નહીંતર જો (item < SYNID_CONTAINED)
	    {
		જો (item - SYNID_ટોચ != ssp->inc_tag || !toplevel)
		    પાછા FALSE;
	    }
	    નહીંતર
	    {
		જો (item - SYNID_CONTAINED != ssp->inc_tag || toplevel)
		    પાછા FALSE;
	    }
	    retval = FALSE;
	}
	નહીંતર
	    retval = TRUE;
	પાછા idl_contains(e, id) ? retval : !retval;
    }

    /*
     * Slow path (uncacheable list, e.g. a nested ALLBUT).
     * If the first item is "ALLBUT", return TRUE if "id" is NOT in the
     * contains list.  We also require that "id" is at the same ":syn include"
     * level as the list.
     */
    item = *list;
    જો (item >= SYNID_ALLBUT && item < SYNID_CLUSTER)
    {
	જો (item < SYNID_ટોચ)
	{
	    // ALL or ALLBUT: accept all groups in the same file
	    જો (item - SYNID_ALLBUT != ssp->inc_tag)
		પાછા FALSE;
	}
	નહીંતર જો (item < SYNID_CONTAINED)
	{
	    // TOP: accept all not-contained groups in the same file
	    જો (item - SYNID_ટોચ != ssp->inc_tag || !toplevel)
		પાછા FALSE;
	}
	નહીંતર
	{
	    // CONTAINED: accept all contained groups in the same file
	    જો (item - SYNID_CONTAINED != ssp->inc_tag || toplevel)
		પાછા FALSE;
	}
	item = *++list;
	retval = FALSE;
    }
    નહીંતર
	retval = TRUE;

    /*
     * Return "retval" if id is in the contains list.
     */
    જ્યારે (item != 0)
    {
	જો (item == id)
	    પાછા retval;
	જો (item >= SYNID_CLUSTER)
	{
	    scl_list = SYN_CLSTR(syn_block)[item - SYNID_CLUSTER].scl_list;
	    // restrict recursiveness to 30 to avoid an endless loop for a
	    // cluster that includes itself (indirectly)
	    જો (scl_list != ખાલી && depth < 30)
	    {
		++depth;
		r = in_id_list(ખાલી, scl_list, ssp, flags);
		--depth;
		જો (r)
		    પાછા retval;
	    }
	}
	item = *++list;
    }
    પાછા !retval;
}

સંરચના subcommand
{
    અક્ષર    *name;			// subcommand name
    શૂન્ય    (*func)(exarg_T *, પૂર્ણાંક);	// function to call
};

સ્થિર સંરચના subcommand subcommands[] =
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
    શૂન્ય
ex_syntax(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*subcmd_end;
    char_u	*subcmd_name;
    પૂર્ણાંક		i;

    syn_cmdlinep = eap->cmdlinep;

    // isolate subcommand name
    માટે (subcmd_end = arg; ASCII_ISALPHA(*subcmd_end); ++subcmd_end)
	;
    subcmd_name = vim_strnsave(arg, subcmd_end - arg);
    જો (subcmd_name == ખાલી)
	પાછા;

    જો (eap->skip)		// skip error messages for all subcommands
	++emsg_skip;
    માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(subcommands); ++i)
    {
	જો (STRCMP(subcmd_name, (char_u *)subcommands[i].name) == 0)
	{
	    eap->arg = skipwhite(subcmd_end);
	    (subcommands[i].func)(eap, FALSE);
	    તોડો;
	}
    }

    જો (i == (પૂર્ણાંક)ARRAY_LENGTH(subcommands))
	semsg(_(e_invalid_syntax_subcommand_str), subcmd_name);

    vim_free(subcmd_name);
    જો (eap->skip)
	--emsg_skip;
}

    શૂન્ય
ex_ownsyntax(exarg_T *eap)
{
    char_u	*old_કિંમત;
    char_u	*new_કિંમત_2;

    જો (curwin->w_s == &curwin->w_buffer->b_s)
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
    old_કિંમત = get_var_કિંમત((char_u *)"b:current_syntax");
    જો (old_કિંમત != ખાલી)
	old_કિંમત = vim_strsave(old_કિંમત);

    // Apply the "syntax" autocommand event, this finds and loads the syntax
    // file.
    apply_autocmds(EVENT_SYNTAX, eap->arg, curbuf->b_fname, TRUE, curbuf);

    // move value of b:current_syntax to w:current_syntax
    new_કિંમત_2 = get_var_કિંમત((char_u *)"b:current_syntax");
    જો (new_કિંમત_2 != ખાલી)
	set_internal_string_var((char_u *)"w:current_syntax", new_કિંમત_2);

    // restore value of b:current_syntax
    જો (old_કિંમત == ખાલી)
	do_unlet((char_u *)"b:current_syntax", TRUE);
    નહીંતર
    {
	set_internal_string_var((char_u *)"b:current_syntax", old_કિંમત);
	vim_free(old_કિંમત);
    }
}

    પૂર્ણાંક
syntax_present(win_T *win)
{
    પાછા (win->w_s->b_syn_patterns.ga_len != 0
	    || win->w_s->b_syn_clusters.ga_len != 0
	    || win->w_s->b_keywtab.ht_used > 0
	    || win->w_s->b_keywtab_ic.ht_used > 0);
}


સ્થિર ગણના
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
    શૂન્ય
reset_expand_highlight(શૂન્ય)
{
    include_link = include_default = include_none = 0;
}

/*
 * Handle command line completion for :match and :echohl command: Add "NONE"
 * as highlight group.
 */
    શૂન્ય
set_context_in_echohl_cmd(expand_T *xp, char_u *arg)
{
    xp->xp_context = EXPAND_HIGHLIGHT;
    xp->xp_pattern = arg;
    include_none = 1;
}

/*
 * Handle command line completion for :syntax command.
 */
    શૂન્ય
set_context_in_syntax_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    // Default: expand subcommands
    xp->xp_context = EXPAND_SYNTAX;
    expand_what = EXP_SUBCMD;
    xp->xp_pattern = arg;
    include_link = 0;
    include_default = 0;

    જો (*arg == NUL)
	પાછા;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    જો (*p == NUL)
	પાછા;

    // past first word
    xp->xp_pattern = skipwhite(p);
    જો (*skiptowhite(xp->xp_pattern) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    નહીંતર જો (STRNICMP(arg, "case", p - arg) == 0)
	expand_what = EXP_CASE;
    નહીંતર જો (STRNICMP(arg, "spell", p - arg) == 0)
	expand_what = EXP_SPELL;
    નહીંતર જો (STRNICMP(arg, "sync", p - arg) == 0)
	expand_what = EXP_SYNC;
    નહીંતર જો (STRNICMP(arg, "list", p - arg) == 0)
    {
	p = skipwhite(p);
	જો (*p == '@')
	    expand_what = EXP_CLUSTER;
	નહીંતર
	    xp->xp_context = EXPAND_HIGHLIGHT;
    }
    નહીંતર જો (STRNICMP(arg, "keyword", p - arg) == 0
	    || STRNICMP(arg, "region", p - arg) == 0
	    || STRNICMP(arg, "match", p - arg) == 0)
	xp->xp_context = EXPAND_HIGHLIGHT;
    નહીંતર
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Function given to ExpandGeneric() to obtain the list syntax names for
 * expansion.
 */
    char_u *
get_syntax_name(expand_T *xp, પૂર્ણાંક idx)
{
    પસંદગી (expand_what)
    {
	સ્થિતિ EXP_SUBCMD:
	    જો (idx < 0 || idx >= (પૂર્ણાંક)ARRAY_LENGTH(subcommands))
		પાછા ખાલી;
	    પાછા (char_u *)subcommands[idx].name;
	સ્થિતિ EXP_CASE:
	{
	    સ્થિર અક્ષર *case_args[] = {"match", "ignore", ખાલી};
	    પાછા (char_u *)case_args[idx];
	}
	સ્થિતિ EXP_SPELL:
	{
	    સ્થિર અક્ષર *spell_args[] =
		{"toplevel", "notoplevel", "default", ખાલી};
	    પાછા (char_u *)spell_args[idx];
	}
	સ્થિતિ EXP_SYNC:
	{
	    સ્થિર અક્ષર *sync_args[] =
		{"ccomment", "clear", "fromstart",
		 "linebreaks=", "linecont", "lines=", "match",
		 "maxlines=", "minlines=", "region", ખાલી};
	    પાછા (char_u *)sync_args[idx];
	}
	સ્થિતિ EXP_CLUSTER:
	{
	    જો (idx < curwin->w_s->b_syn_clusters.ga_len)
	    {
		vim_snprintf((અક્ષર *)xp->xp_buf, EXPAND_BUF_LEN, "@%s",
					 SYN_CLSTR(curwin->w_s)[idx].scl_name);
		પાછા xp->xp_buf;
	    }
	    નહીંતર
		પાછા ખાલી;
	}
    }
    પાછા ખાલી;
}


/*
 * Function called for expression evaluation: get syntax ID at file position.
 */
    પૂર્ણાંક
syn_get_id(
    win_T	*wp,
    long	lnum,
    colnr_T	col,
    પૂર્ણાંક		trans,	     // remove transparency
    પૂર્ણાંક		*spellp,     // return: can do spell checking
    પૂર્ણાંક		keep_state)  // keep state of char at "col"
{
    // When the position is not after the current position and in the same
    // line of the same window with the same buffer, need to restart parsing.
    જો (wp != syn_win
	    || wp->w_buffer != syn_buf
	    || lnum != current_lnum
	    || col < current_col)
	syntax_start(wp, lnum);
    નહીંતર જો (wp->w_buffer == syn_buf
	    && lnum == current_lnum
	    && col > current_col)
	// next_match may not be correct when moving around, e.g. with the
	// "skip" expression in searchpair()
	next_match_idx = -1;

    (શૂન્ય)get_syntax_attr(col, spellp, keep_state);

    પાછા (trans ? current_trans_id : current_id);
}

#જો defined(FEAT_CONCEAL)
/*
 * Get extra information about the syntax item.  Must be called right after
 * get_syntax_attr().
 * Stores the current item sequence nr in "*seqnrp".
 * Returns the current flags.
 */
    પૂર્ણાંક
get_syntax_info(પૂર્ણાંક *seqnrp)
{
    *seqnrp = current_seqnr;
    પાછા current_flags;
}

/*
 * Return conceal substitution character
 */
    પૂર્ણાંક
syn_get_sub_char(શૂન્ય)
{
    પાછા current_sub_char;
}
#endif

#જો defined(FEAT_EVAL)
/*
 * Return the syntax ID at position "i" in the current stack.
 * The caller must have called syn_get_id() before to fill the stack.
 * Returns -1 when "i" is out of range.
 */
    પૂર્ણાંક
syn_get_ઢગલો_item(પૂર્ણાંક i)
{
    જો (i >= current_state.ga_len)
    {
	// Need to invalidate the state, because we didn't properly finish it
	// for the last character, "keep_state" was TRUE.
	invalidate_current_state();
	current_col = MAXCOL;
	પાછા -1;
    }
    પાછા CUR_STATE(i).si_id;
}
#endif

#જો defined(FEAT_FOLDING)
    સ્થિર પૂર્ણાંક
syn_cur_foldlevel(શૂન્ય)
{
    પૂર્ણાંક		level = 0;
    પૂર્ણાંક		i;

    માટે (i = 0; i < current_state.ga_len; ++i)
	જો (CUR_STATE(i).si_flags & HL_FOLD)
	    ++level;
    પાછા level;
}

/*
 * Function called to get folding level for line "lnum" in window "wp".
 */
    પૂર્ણાંક
syn_get_foldlevel(win_T *wp, long lnum)
{
    પૂર્ણાંક		level = 0;
    પૂર્ણાંક		નીચલું_level;
    પૂર્ણાંક		cur_level;

    // Return quickly when there are no fold items at all.
    જો (wp->w_s->b_syn_folditems != 0
	    && !wp->w_s->b_syn_error
# ifdef SYN_TIME_LIMIT
	    && !wp->w_s->b_syn_slow
# endif
	    )
    {
	syntax_start(wp, lnum);

	// Start with the fold level at the start of the line.
	level = syn_cur_foldlevel();

	જો (wp->w_s->b_syn_foldlevel == SYNFLD_MINIMUM)
	{
	    // Find the lowest fold level that is followed by a higher one.
	    cur_level = level;
	    નીચલું_level = cur_level;
	    જ્યારે (!current_finished)
	    {
		(શૂન્ય)syn_current_attr(FALSE, FALSE, ખાલી, FALSE);
		cur_level = syn_cur_foldlevel();
		જો (cur_level < નીચલું_level)
		    નીચલું_level = cur_level;
		નહીંતર જો (cur_level > નીચલું_level)
		    level = નીચલું_level;
		++current_col;
	    }
	}
    }
    જો (level > wp->w_p_fdn)
    {
	level = wp->w_p_fdn;
	જો (level < 0)
	    level = 0;
    }
    પાછા level;
}
#endif

#જો defined(FEAT_PROFILE)
/*
 * ":syntime".
 */
    શૂન્ય
ex_syntime(exarg_T *eap)
{
    જો (STRCMP(eap->arg, "on") == 0)
	syn_time_on = TRUE;
    નહીંતર જો (STRCMP(eap->arg, "off") == 0)
	syn_time_on = FALSE;
    નહીંતર જો (STRCMP(eap->arg, "clear") == 0)
	syntime_clear();
    નહીંતર જો (STRCMP(eap->arg, "report") == 0)
	syntime_report();
    નહીંતર
	semsg(_(e_invalid_argument_str), eap->arg);
}

    સ્થિર શૂન્ય
syn_clear_time(syn_time_T *st)
{
    profile_zero(&st->total);
    profile_zero(&st->slowest);
    st->ગણતરી = 0;
    st->match = 0;
}

/*
 * Clear the syntax timing for the current buffer.
 */
    સ્થિર શૂન્ય
syntime_clear(શૂન્ય)
{
    પૂર્ણાંક		idx;
    synpat_T	*spp;

    જો (!syntax_present(curwin))
    {
	સંદેશ(_(સંદેશ_no_વસ્તુઓ));
	પાછા;
    }
    માટે (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len; ++idx)
    {
	spp = &(SYN_વસ્તુઓ(curwin->w_s)[idx]);
	syn_clear_time(&spp->sp_time);
    }
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":syntime {on,off,clear,report}" command.
 */
    char_u *
get_syntime_arg(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    પસંદગી (idx)
    {
	સ્થિતિ 0: પાછા (char_u *)"on";
	સ્થિતિ 1: પાછા (char_u *)"off";
	સ્થિતિ 2: પાછા (char_u *)"clear";
	સ્થિતિ 3: પાછા (char_u *)"report";
    }
    પાછા ખાલી;
}

પ્રકારવ્યાખ્યા સંરચના
{
    proftime_T	total;
    પૂર્ણાંક		ગણતરી;
    પૂર્ણાંક		match;
    proftime_T	slowest;
    proftime_T	average;
    પૂર્ણાંક		id;
    char_u	*pattern;
} time_entry_T;

    સ્થિર પૂર્ણાંક
syn_compare_syntime(અચલ શૂન્ય *v1, અચલ શૂન્ય *v2)
{
    અચલ time_entry_T	*s1 = v1;
    અચલ time_entry_T	*s2 = v2;

    પાછા profile_cmp(&s1->total, &s2->total);
}

/*
 * Clear the syntax timing for the current buffer.
 */
    સ્થિર શૂન્ય
syntime_report(શૂન્ય)
{
    પૂર્ણાંક		idx;
    synpat_T	*spp;
# જો defined(FEAT_RELTIME)
    proftime_T	tm;
# endif
    પૂર્ણાંક		len;
    પૂર્ણાંક		patlen;
    proftime_T	total_total;
    પૂર્ણાંક		total_ગણતરી = 0;
    garray_T    ga;
    time_entry_T *p;

    જો (!syntax_present(curwin))
    {
	સંદેશ(_(સંદેશ_no_વસ્તુઓ));
	પાછા;
    }

    ga_init2(&ga, કદ(time_entry_T), 50);
    profile_zero(&total_total);
    માટે (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len; ++idx)
    {
	spp = &(SYN_વસ્તુઓ(curwin->w_s)[idx]);
	જો (spp->sp_time.ગણતરી > 0)
	{
	    (શૂન્ય)ga_grow(&ga, 1);
	    p = ((time_entry_T *)ga.ga_data) + ga.ga_len;
	    p->total = spp->sp_time.total;
	    profile_add(&total_total, &spp->sp_time.total);
	    p->ગણતરી = spp->sp_time.ગણતરી;
	    p->match = spp->sp_time.match;
	    total_ગણતરી += spp->sp_time.ગણતરી;
	    p->slowest = spp->sp_time.slowest;
# જો defined(FEAT_RELTIME)
	    profile_divide(&spp->sp_time.total, spp->sp_time.ગણતરી, &tm);
	    p->average = tm;
# endif
	    p->id = spp->sp_syn.id;
	    p->pattern = spp->sp_pattern;
	    ++ga.ga_len;
	}
    }

    // Sort on total time. Skip if there are no items to avoid passing NULL
    // pointer to qsort().
    જો (ga.ga_len > 1)
	qsort(ga.ga_data, (size_t)ga.ga_len, કદ(time_entry_T),
							 syn_compare_syntime);

    સંદેશ_puts_title(_("  TOTAL      COUNT  MATCH   SLOWEST     AVERAGE   NAME               PATTERN"));
    સંદેશ_puts("\n");
    માટે (idx = 0; idx < ga.ga_len && !got_int; ++idx)
    {
	p = ((time_entry_T *)ga.ga_data) + idx;

	સંદેશ_puts(profile_સંદેશ(&p->total));
	સંદેશ_puts(" "); // make sure there is always a separating space
	સંદેશ_advance(13);
	સંદેશ_outnum(p->ગણતરી);
	સંદેશ_puts(" ");
	સંદેશ_advance(20);
	સંદેશ_outnum(p->match);
	સંદેશ_puts(" ");
	સંદેશ_advance(26);
	સંદેશ_puts(profile_સંદેશ(&p->slowest));
	સંદેશ_puts(" ");
	સંદેશ_advance(38);
	સંદેશ_puts(profile_સંદેશ(&p->average));
	સંદેશ_puts(" ");
	સંદેશ_advance(50);
	સંદેશ_outtrans(highlight_group_name(p->id - 1));
	સંદેશ_puts(" ");

	સંદેશ_advance(69);
	જો (Columns < 80)
	    len = 20; // will wrap anyway
	નહીંતર
	    len = Columns - 70;
	patlen = (પૂર્ણાંક)STRLEN(p->pattern);
	જો (len > patlen)
	    len = patlen;
	સંદેશ_outtrans_len(p->pattern, len);
	સંદેશ_puts("\n");
    }
    ga_clear(&ga);
    જો (!got_int)
    {
	સંદેશ_puts("\n");
	સંદેશ_puts(profile_સંદેશ(&total_total));
	સંદેશ_advance(13);
	સંદેશ_outnum(total_ગણતરી);
	સંદેશ_puts("\n");
    }
}
#endif

#endif // FEAT_SYN_HL
