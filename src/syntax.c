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

#अन्तर्भाव "vim.h"

#यदि defined(FEAT_SYN_HL)

// different types of offsets that are possible
#define SPO_MS_OFF	0	// match  start offset
#define SPO_ME_OFF	1	// match  end	offset
#define SPO_HS_OFF	2	// highl. start offset
#define SPO_HE_OFF	3	// highl. end	offset
#define SPO_RS_OFF	4	// region start offset
#define SPO_RE_OFF	5	// region end	offset
#define SPO_LC_OFF	6	// leading context offset
#define SPO_गन्ती	7

स्थिर वर्ण *(spo_name_tab[SPO_गन्ती]) =
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
प्रकारनिर्देश संरचना syn_pattern
{
    वर्ण	 sp_type;		// see SPTYPE_ defines below
    वर्ण	 sp_syncing;		// this item used for syncing
    लघु	 sp_syn_match_id;	// highlight group ID of pattern
    लघु	 sp_off_flags;		// see below
    पूर्णाङ्क		 sp_offsets[SPO_गन्ती];	// offsets
    पूर्णाङ्क		 sp_flags;		// see HL_ defines below
#ifdef FEAT_CONCEAL
    पूर्णाङ्क		 sp_cchar;		// conceal substitute character
#endif
    पूर्णाङ्क		 sp_ic;			// ignore-case flag for sp_prog
    पूर्णाङ्क		 sp_sync_idx;		// sync item index (syncing only)
    पूर्णाङ्क		 sp_पङ्क्ति_id;		// ID of last line where tried
    पूर्णाङ्क		 sp_startcol;		// next match in sp_line_id line
    लघु	*sp_cont_list;		// cont. group IDs, if non-zero
    लघु	*sp_next_list;		// next group IDs, if non-zero
    संरचना sp_syn sp_syn;		// struct passed to in_id_list()
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


#define SYN_वस्तुहरू(buf)	((synpat_T *)((buf)->b_syn_patterns.ga_data))

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
स्थिर पूर्णाङ्क current_attr = 0;	    // attr of current syntax word
#ifdef FEAT_EVAL
स्थिर पूर्णाङ्क current_id = 0;	    // ID of current char for syn_get_id()
स्थिर पूर्णाङ्क current_trans_id = 0;    // idem, transparency removed
#endif
#ifdef FEAT_CONCEAL
स्थिर पूर्णाङ्क current_flags = 0;
स्थिर पूर्णाङ्क current_seqnr = 0;
स्थिर पूर्णाङ्क current_sub_char = 0;
#endif

प्रकारनिर्देश संरचना syn_cluster_S
{
    char_u	    *scl_name;	    // syntax cluster name
    char_u	    *scl_name_u;    // uppercase of scl_name
    लघु	    *scl_list;	    // IDs in this syntax cluster
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
#define SYNID_शीर्ष	21000	    // syntax group ID for contains=TOP
#define SYNID_CONTAINED	22000	    // syntax group ID for contains=CONTAINED
#define SYNID_CLUSTER	23000	    // first syntax group ID for clusters

#define MAX_SYN_INC_TAG	999	    // maximum before the above overflow
#define MAX_CLUSTER_ID  (32767 - SYNID_CLUSTER)

/*
 * Annoying Hack(TM):  ":syn include" needs this pointer to pass to
 * expand_filename().  Most of the other syntax commands don't need it, so
 * instead of passing it to them, we stow it here.
 */
स्थिर char_u **syn_cmdlinep;

/*
 * Another Annoying Hack(TM):  To prevent rules from other ":syn include"'d
 * files from leaking into ALLBUT lists, we assign a unique ID to the
 * rules in each ":syn include"'d file.
 */
स्थिर पूर्णाङ्क current_syn_inc_tag = 0;
स्थिर पूर्णाङ्क running_syn_inc_tag = 0;

/*
 * In a hashtable item "hi_key" points to "keyword" in a keyentry.
 * This avoids adding a pointer to the hashtable item.
 * KE2HIKEY() converts a var pointer to a hashitem key pointer.
 * HIKEY2KE() converts a hashitem key pointer to a var pointer.
 * HI2KE() converts a hashitem pointer to a var pointer.
 */
स्थिर keyentry_T dumkey;
#define KE2HIKEY(kp)  ((kp)->keyword)
#define HIKEY2KE(p)   ((keyentry_T *)((p) - (dumkey.keyword - (char_u *)&dumkey)))
#define HI2KE(hi)      HIKEY2KE((hi)->hi_साँचो)

/*
 * To reduce the time spent in keepend(), remember at which level in the state
 * stack the first item with "keepend" is present.  When "-1", there is no
 * "keepend" on the stack.
 */
स्थिर पूर्णाङ्क keepend_level = -1;

स्थिर वर्ण सन्देश_no_वस्तुहरू[] = N_("No Syntax items defined for this buffer");

/*
 * For the current state we need to remember more than just the idx.
 * When si_m_endpos.lnum is 0, the items other than si_idx are unknown.
 * (The end positions have the column number of the next char)
 */
प्रकारनिर्देश संरचना state_item
{
    पूर्णाङ्क		si_idx;			// index of syntax pattern or
					// KEYWORD_IDX
    पूर्णाङ्क		si_id;			// highlight group ID for keywords
    पूर्णाङ्क		si_trans_id;		// idem, transparency removed
    पूर्णाङ्क		si_m_lnum;		// lnum of the match
    पूर्णाङ्क		si_m_startcol;		// starting column of the match
    lpos_T	si_m_endpos;		// just after end posn of the match
    lpos_T	si_h_startpos;		// start position of the highlighting
    lpos_T	si_h_endpos;		// end position of the highlighting
    lpos_T	si_eoe_pos;		// end position of end pattern
    पूर्णाङ्क		si_end_idx;		// group ID for end pattern or zero
    पूर्णाङ्क		si_ends;		// if match ends before si_m_endpos
    पूर्णाङ्क		si_attr;		// attributes in this state
    दीर्घ	si_flags;		// HL_HAS_EOL flag in this state, and
					// HL_SKIP* for si_next_list
#ifdef FEAT_CONCEAL
    पूर्णाङ्क		si_seqnr;		// sequence number
    पूर्णाङ्क		si_cchar;		// substitution character for conceal
#endif
    लघु	*si_cont_list;		// list of contained groups
    लघु	*si_next_list;		// nextgroup IDs after this item ends
    reg_extmatch_T *si_extmatch;	// \z(...\) matches from start
					// pattern
} stateitem_T;

#define KEYWORD_IDX	(-1)	    // value of si_idx for keywords
#define ID_LIST_ALL	((लघु *)-1) // valid of si_cont_list for containing all
				    // but contained groups

#ifdef FEAT_CONCEAL
स्थिर पूर्णाङ्क next_seqnr = 1;		// value to use for si_seqnr
#endif

/*
 * Struct to reduce the number of arguments to get_syn_options(), it's used
 * very often.
 */
प्रकारनिर्देश संरचना
{
    पूर्णाङ्क		flags;		// flags for contained and transparent
    पूर्णाङ्क		keyword;	// TRUE for ":syn keyword"
    पूर्णाङ्क		*sync_idx;	// syntax item for "grouphere" argument, NULL
				// if not allowed
    वर्ण	has_cont_list;	// TRUE if "cont_list" can be used
    लघु	*cont_list;	// group IDs for "contains" argument
    लघु	*cont_in_list;	// group IDs for "containedin" argument
    लघु	*next_list;	// group IDs for "nextgroup" argument
} syn_opt_arg_T;

/*
 * The next possible match in the current line for any pattern is remembered,
 * to avoid having to try for a match in each column.
 * If next_match_idx == -1, not tried (in this line) yet.
 * If next_match_col == MAXCOL, no match found in this line.
 * (All end positions have the column of the char after the end)
 */
स्थिर पूर्णाङ्क next_match_col;		// column for start of next match
स्थिर lpos_T next_match_m_endpos;	// position for end of next match
स्थिर lpos_T next_match_h_startpos;	// pos. for highl. start of next match
स्थिर lpos_T next_match_h_endpos;	// pos. for highl. end of next match
स्थिर पूर्णाङ्क next_match_idx;		// index of matched item
स्थिर दीर्घ next_match_flags;		// flags for next match
स्थिर lpos_T next_match_eos_pos;	// end of start pattn (start region)
स्थिर lpos_T next_match_eoe_pos;	// pos. for end of end pattern
स्थिर पूर्णाङ्क next_match_end_idx;		// ID of group for end pattn or zero
स्थिर reg_extmatch_T *next_match_extmatch = रिक्त;

/*
 * A state stack is an array of integers or stateitem_T, stored in a
 * garray_T.  A state stack is invalid if its itemsize entry is zero.
 */
#define INVALID_STATE(ssp)  ((ssp)->ga_itemsize == 0)
#define VALID_STATE(ssp)    ((ssp)->ga_itemsize != 0)

#define FOR_ALL_SYNSTATES(sb, sst) \
    लागि ((sst) = (sb)->b_sst_first; (sst) != रिक्त; (sst) = (sst)->sst_next)

/*
 * The current state (within the line) of the recognition engine.
 * When current_state.ga_itemsize is 0 the current state is invalid.
 */
स्थिर win_T	*syn_win;		// current window for highlighting
स्थिर buf_T	*syn_buf;		// current buffer for highlighting
स्थिर synblock_T *syn_block;		// current buffer for highlighting
स्थिर linenr_T current_lnum = 0;	// lnum of current state
स्थिर colnr_T	current_col = 0;	// column of current state
स्थिर पूर्णाङ्क	current_state_stored = 0; // TRUE if stored current state
					  // after setting current_finished
स्थिर पूर्णाङ्क	current_finished = 0;	// current line has been finished
स्थिर garray_T current_state		// current stack of state_items
		= {0, 0, 0, 0, रिक्त};
स्थिर लघु	*current_next_list = रिक्त; // when non-zero, nextgroup list
स्थिर पूर्णाङ्क	current_next_flags = 0; // flags for current_next_list
स्थिर पूर्णाङ्क	current_पङ्क्ति_id = 0;	// unique number for current line

#define CUR_STATE(idx)	((stateitem_T *)(current_state.ga_data))[idx]

स्थिर शून्य syn_sync(win_T *wp, linenr_T lnum, synstate_T *last_valid);
स्थिर पूर्णाङ्क syn_match_linecont(linenr_T lnum);
स्थिर शून्य syn_start_पङ्क्ति(शून्य);
स्थिर शून्य syn_update_ends(पूर्णाङ्क startofline);
स्थिर शून्य syn_थाक_alloc(शून्य);
स्थिर पूर्णाङ्क syn_थाक_cleanup(शून्य);
स्थिर शून्य syn_थाक_free_entry(synblock_T *block, synstate_T *p);
स्थिर synstate_T *syn_थाक_find_entry(linenr_T lnum);
स्थिर synstate_T *store_current_state(शून्य);
स्थिर शून्य load_current_state(synstate_T *from);
स्थिर शून्य invalidate_current_state(शून्य);
स्थिर पूर्णाङ्क syn_थाक_equal(synstate_T *sp);
स्थिर शून्य validate_current_state(शून्य);
स्थिर पूर्णाङ्क syn_finish_पङ्क्ति(पूर्णाङ्क syncing);
स्थिर पूर्णाङ्क syn_current_attr(पूर्णाङ्क syncing, पूर्णाङ्क displaying, पूर्णाङ्क *can_spell, पूर्णाङ्क keep_state);
स्थिर पूर्णाङ्क did_match_already(पूर्णाङ्क idx, garray_T *gap);
स्थिर stateitem_T *थप_next_match(stateitem_T *cur_si);
स्थिर शून्य check_state_ends(शून्य);
स्थिर शून्य update_si_attr(पूर्णाङ्क idx);
स्थिर शून्य check_keepend(शून्य);
स्थिर शून्य update_si_end(stateitem_T *sip, पूर्णाङ्क startcol, पूर्णाङ्क force);
स्थिर लघु *copy_id_list(लघु *list);
स्थिर पूर्णाङ्क in_id_list(stateitem_T *item, लघु *cont_list, संरचना sp_syn *ssp, पूर्णाङ्क flags);
स्थिर पूर्णाङ्क थप_current_state(पूर्णाङ्क idx);
स्थिर शून्य झिक_current_state(शून्य);
#ifdef FEAT_PROFILE
स्थिर शून्य syn_clear_time(syn_time_T *tt);
स्थिर शून्य syntime_clear(शून्य);
स्थिर शून्य syntime_report(शून्य);
स्थिर पूर्णाङ्क syn_time_on = FALSE;
# define IF_SYN_TIME(p) (p)
#अन्यथा
# define IF_SYN_TIME(p) रिक्त
प्रकारनिर्देश पूर्णाङ्क syn_time_T;
#endif

स्थिर शून्य syn_थाक_apply_changes_block(synblock_T *block, buf_T *buf);
स्थिर शून्य find_endpos(पूर्णाङ्क idx, lpos_T *startpos, lpos_T *m_endpos, lpos_T *hl_endpos, दीर्घ *flagsp, lpos_T *end_endpos, पूर्णाङ्क *end_idx, reg_extmatch_T *start_ext);

स्थिर शून्य limit_pos(lpos_T *pos, lpos_T *limit);
स्थिर शून्य limit_pos_zero(lpos_T *pos, lpos_T *limit);
स्थिर शून्य syn_add_end_off(lpos_T *result, regmmatch_T *regmatch, synpat_T *spp, पूर्णाङ्क idx, पूर्णाङ्क extra);
स्थिर शून्य syn_add_start_off(lpos_T *result, regmmatch_T *regmatch, synpat_T *spp, पूर्णाङ्क idx, पूर्णाङ्क extra);
स्थिर char_u *syn_getcurline(शून्य);
स्थिर colnr_T syn_getcurline_len(शून्य);
स्थिर पूर्णाङ्क syn_regexec(regmmatch_T *rmp, linenr_T lnum, colnr_T col, syn_time_T *st);
स्थिर पूर्णाङ्क check_keyword_id(char_u *पङ्क्ति, पूर्णाङ्क startcol, पूर्णाङ्क *endcol, दीर्घ *flags, लघु **next_list, stateitem_T *cur_si, पूर्णाङ्क *ccharp);
स्थिर शून्य syn_remove_pattern(synblock_T *block, पूर्णाङ्क idx);
स्थिर शून्य syn_clear_pattern(synblock_T *block, पूर्णाङ्क i);
स्थिर शून्य syn_clear_cluster(synblock_T *block, पूर्णाङ्क i);
स्थिर शून्य syn_clear_one(पूर्णाङ्क id, पूर्णाङ्क syncing);
स्थिर शून्य syn_cmd_onoff(exarg_T *eap, वर्ण *name);
स्थिर शून्य syn_पङ्क्तिहरू_सन्देश(शून्य);
स्थिर शून्य syn_match_सन्देश(शून्य);
स्थिर शून्य syn_list_one(पूर्णाङ्क id, पूर्णाङ्क syncing, पूर्णाङ्क link_only);
स्थिर शून्य syn_list_cluster(पूर्णाङ्क id);
स्थिर शून्य put_id_list(char_u *name, लघु *list, पूर्णाङ्क attr);
स्थिर शून्य put_pattern(वर्ण *s, पूर्णाङ्क c, synpat_T *spp, पूर्णाङ्क attr);
स्थिर पूर्णाङ्क syn_list_keywords(पूर्णाङ्क id, hashtab_T *ht, पूर्णाङ्क did_header, पूर्णाङ्क attr);
स्थिर शून्य syn_clear_keyword(पूर्णाङ्क id, hashtab_T *ht);
स्थिर शून्य clear_keywtab(hashtab_T *ht);
स्थिर पूर्णाङ्क syn_scl_namen2id(char_u *linep, पूर्णाङ्क len);
स्थिर पूर्णाङ्क syn_check_cluster(char_u *pp, पूर्णाङ्क len);
स्थिर पूर्णाङ्क syn_add_cluster(char_u *name);
स्थिर शून्य init_syn_patterns(शून्य);
स्थिर char_u *get_syn_pattern(char_u *arg, synpat_T *ci);
स्थिर पूर्णाङ्क get_id_list(char_u **arg, पूर्णाङ्क keylen, लघु **list, पूर्णाङ्क skip);
स्थिर शून्य syn_combine_list(लघु **clstr1, लघु **clstr2, पूर्णाङ्क list_op);
स्थिर शून्य idl_cache_clear(synblock_T *block);

/*
 * Start the syntax recognition for a line.  This function is normally called
 * from the screen updating, once for each displayed line.
 * The buffer is remembered in syn_buf, because get_syntax_attr() doesn't get
 * it.	Careful: curbuf and curwin are likely to point to another buffer and
 * window.
 */
    शून्य
syntax_start(win_T *wp, linenr_T lnum)
{
    synstate_T	*p;
    synstate_T	*last_valid = रिक्त;
    synstate_T	*last_min_valid = रिक्त;
    synstate_T	*sp, *prev = रिक्त;
    linenr_T	parsed_lnum;
    linenr_T	first_stored;
    पूर्णाङ्क		dist;
    स्थिर varnumber_T changedtick = 0;	// remember the last change ID

#ifdef FEAT_CONCEAL
    current_sub_char = NUL;
#endif

    /*
     * After switching buffers, invalidate current_state.
     * Also do this when a change was made, the current state may be invalid
     * then.
     */
    यदि (syn_block != wp->w_s
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
    syn_थाक_alloc();
    यदि (syn_block->b_sst_array == रिक्त)
	फर्काऊ;		// out of memory
    syn_block->b_sst_lasttick = display_tick;

    /*
     * If the state of the end of the previous line is useful, store it.
     */
    यदि (VALID_STATE(&current_state)
	    && current_lnum < lnum
	    && current_lnum < syn_buf->b_ml.ml_पङ्क्ति_गन्ती)
    {
	(शून्य)syn_finish_पङ्क्ति(FALSE);
	यदि (!current_state_stored)
	{
	    ++current_lnum;
	    (शून्य)store_current_state();
	}

	/*
	 * If the current_lnum is now the same as "lnum", keep the current
	 * state (this happens very often!).  Otherwise invalidate
	 * current_state and figure it out below.
	 */
	यदि (current_lnum != lnum)
	    invalidate_current_state();
    }
    अन्यथा
	invalidate_current_state();

    /*
     * Try to synchronize from a saved state in b_sst_array[].
     * Only do this if lnum is not before and not to far beyond a saved state.
     */
    यदि (INVALID_STATE(&current_state) && syn_block->b_sst_array != रिक्त)
    {
	// Find last valid saved state before start_lnum.
	FOR_ALL_SYNSTATES(syn_block, p)
	{
	    यदि (p->sst_lnum > lnum)
		रोक;
	    यदि (p->sst_lnum <= lnum && p->sst_change_lnum == 0)
	    {
		last_valid = p;
		यदि (p->sst_lnum >= lnum - syn_block->b_syn_sync_minlines)
		    last_min_valid = p;
	    }
	}
	यदि (last_min_valid != रिक्त)
	    load_current_state(last_min_valid);
    }

    /*
     * If "lnum" is before or far beyond a line with a saved state, need to
     * re-synchronize.
     */
    यदि (INVALID_STATE(&current_state))
    {
	syn_sync(wp, lnum, last_valid);
	यदि (current_lnum == 1)
	    // First line is always valid, no matter "minlines".
	    first_stored = 1;
	अन्यथा
	    // Need to parse "minlines" lines before state can be considered
	    // valid to store.
	    first_stored = current_lnum + syn_block->b_syn_sync_minlines;
    }
    अन्यथा
	first_stored = current_lnum;

    /*
     * Advance from the sync point or saved state until the current line.
     * Save some entries for syncing with later on.
     */
    यदि (syn_block->b_sst_len <= Rows)
	dist = 999999;
    अन्यथा
	dist = syn_buf->b_ml.ml_पङ्क्ति_गन्ती / (syn_block->b_sst_len - Rows) + 1;
    जबसम्म (current_lnum < lnum)
    {
	syn_start_पङ्क्ति();
	(शून्य)syn_finish_पङ्क्ति(FALSE);
	++current_lnum;

	// If we parsed at least "minlines" lines or started at a valid
	// state, the current state is considered valid.
	यदि (current_lnum >= first_stored)
	{
	    // Check if the saved state entry is for the current line and is
	    // equal to the current state.  If so, then validate all saved
	    // states that depended on a change before the parsed line.
	    यदि (prev == रिक्त)
		prev = syn_थाक_find_entry(current_lnum - 1);
	    यदि (prev == रिक्त)
		sp = syn_block->b_sst_first;
	    अन्यथा
		sp = prev;
	    जबसम्म (sp != रिक्त && sp->sst_lnum < current_lnum)
		sp = sp->sst_next;
	    यदि (sp != रिक्त
		    && sp->sst_lnum == current_lnum
		    && syn_थाक_equal(sp))
	    {
		parsed_lnum = current_lnum;
		prev = sp;
		जबसम्म (sp != रिक्त && sp->sst_change_lnum <= parsed_lnum)
		{
		    यदि (sp->sst_lnum <= lnum)
			// valid state before desired line, use this one
			prev = sp;
		    अन्यथा यदि (sp->sst_change_lnum == 0)
			// past saved states depending on change, break here.
			रोक;
		    sp->sst_change_lnum = 0;
		    sp = sp->sst_next;
		}
		load_current_state(prev);
	    }
	    // Store the state at this line when it's the first one, the line
	    // where we start parsing, or some distance from the previously
	    // saved state.  But only when parsed at least 'minlines'.
	    अन्यथा यदि (prev == रिक्त
			|| current_lnum == lnum
			|| current_lnum >= prev->sst_lnum + dist)
		prev = store_current_state();
	}

	// This can take a long time: break when CTRL-C pressed.  The current
	// state will be wrong then.
	पङ्क्ति_breakcheck();
	यदि (got_int)
	{
	    current_lnum = lnum;
	    रोक;
	}
    }

    syn_start_पङ्क्ति();
}

/*
 * We cannot simply discard growarrays full of state_items or buf_states; we
 * have to manually release their extmatch pointers first.
 */
    स्थिर शून्य
clear_syn_state(synstate_T *p)
{
    पूर्णाङ्क		i;
    garray_T	*gap;

    यदि (p->sst_stacksize > SST_FIX_STATES)
    {
	gap = &(p->sst_union.sst_ga);
	लागि (i = 0; i < gap->ga_len; i++)
	    unref_extmatch(SYN_STATE_P(gap)[i].bs_extmatch);
	ga_clear(gap);
    }
    अन्यथा
    {
	लागि (i = 0; i < p->sst_stacksize; i++)
	    unref_extmatch(p->sst_union.sst_थाक[i].bs_extmatch);
    }
}

/*
 * Cleanup the current_state stack.
 */
    स्थिर शून्य
clear_current_state(शून्य)
{
    पूर्णाङ्क		i;
    stateitem_T	*sip;

    sip = (stateitem_T *)(current_state.ga_data);
    लागि (i = 0; i < current_state.ga_len; i++)
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
    स्थिर शून्य
syn_sync(
    win_T	*wp,
    linenr_T	start_lnum,
    synstate_T	*last_valid)
{
    buf_T	*curbuf_save;
    win_T	*curwin_save;
    pos_T	cursor_save;
    पूर्णाङ्क		idx;
    linenr_T	lnum;
    linenr_T	end_lnum;
    linenr_T	break_lnum;
    पूर्णाङ्क		had_sync_point;
    stateitem_T	*cur_si;
    synpat_T	*spp;
    पूर्णाङ्क		found_flags = 0;
    पूर्णाङ्क		found_match_idx = 0;
    linenr_T	found_current_lnum = 0;
    पूर्णाङ्क		found_current_col = 0;
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
    यदि (syn_block->b_syn_sync_minlines > start_lnum)
	start_lnum = 1;
    अन्यथा
    {
	यदि (syn_block->b_syn_sync_minlines == 1)
	    lnum = 1;
	अन्यथा यदि (syn_block->b_syn_sync_minlines < 10)
	    lnum = syn_block->b_syn_sync_minlines * 2;
	अन्यथा
	    lnum = syn_block->b_syn_sync_minlines * 3 / 2;
	यदि (syn_block->b_syn_sync_maxlines != 0
				     && lnum > syn_block->b_syn_sync_maxlines)
	    lnum = syn_block->b_syn_sync_maxlines;
	यदि (lnum >= start_lnum)
	    start_lnum = 1;
	अन्यथा
	    start_lnum -= lnum;
    }
    current_lnum = start_lnum;

    /*
     * 1. Search backwards for the end of a C-style comment.
     */
    यदि (syn_block->b_syn_sync_flags & SF_CCOMMENT)
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
	लागि ( ; start_lnum > 1; --start_lnum)
	{
	    char_u	*l = ml_get(start_lnum - 1);

	    यदि (*l == NUL || *(l + ml_get_len(start_lnum - 1) - 1) != '\\')
		रोक;
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
	यदि (find_start_comment((पूर्णाङ्क)syn_block->b_syn_sync_maxlines) != रिक्त)
	{
	    लागि (idx = syn_block->b_syn_patterns.ga_len; --idx >= 0; )
		यदि (SYN_वस्तुहरू(syn_block)[idx].sp_syn.id
						   == syn_block->b_syn_sync_id
			&& SYN_वस्तुहरू(syn_block)[idx].sp_type == SPTYPE_START)
		{
		    validate_current_state();
		    यदि (थप_current_state(idx) == OK)
			update_si_attr(current_state.ga_len - 1);
		    रोक;
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
    अन्यथा यदि (syn_block->b_syn_sync_flags & SF_MATCH)
    {
	यदि (syn_block->b_syn_sync_maxlines != 0
			       && start_lnum > syn_block->b_syn_sync_maxlines)
	    break_lnum = start_lnum - syn_block->b_syn_sync_maxlines;
	अन्यथा
	    break_lnum = 0;

	found_m_endpos.lnum = 0;
	found_m_endpos.col = 0;
	end_lnum = start_lnum;
	lnum = start_lnum;
	जबसम्म (--lnum > break_lnum)
	{
	    // This can take a long time: break when CTRL-C pressed.
	    पङ्क्ति_breakcheck();
	    यदि (got_int)
	    {
		invalidate_current_state();
		current_lnum = start_lnum;
		रोक;
	    }

	    // Check if we have run into a valid saved state stack now.
	    यदि (last_valid != रिक्त && lnum == last_valid->sst_lnum)
	    {
		load_current_state(last_valid);
		रोक;
	    }

	    /*
	     * Check if the previous line has the line-continuation pattern.
	     */
	    यदि (lnum > 1 && syn_match_linecont(lnum - 1))
		जारी;

	    /*
	     * Start with nothing on the state stack
	     */
	    validate_current_state();

	    लागि (current_lnum = lnum; current_lnum < end_lnum; ++current_lnum)
	    {
		syn_start_पङ्क्ति();
		लागि (;;)
		{
		    had_sync_point = syn_finish_पङ्क्ति(TRUE);
		    /*
		     * When a sync point has been found, remember where, and
		     * continue to look for another one, further on in the line.
		     */
		    यदि (had_sync_point && current_state.ga_len)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			यदि (cur_si->si_m_endpos.lnum > start_lnum)
			{
			    // ignore match that goes to after where started
			    current_lnum = end_lnum;
			    रोक;
			}
			यदि (cur_si->si_idx < 0)
			{
			    // Cannot happen?
			    found_flags = 0;
			    found_match_idx = KEYWORD_IDX;
			}
			अन्यथा
			{
			    spp = &(SYN_वस्तुहरू(syn_block)[cur_si->si_idx]);
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
			यदि (found_m_endpos.lnum > current_lnum)
			{
			    current_lnum = found_m_endpos.lnum;
			    current_col = found_m_endpos.col;
			    यदि (current_lnum >= end_lnum)
				रोक;
			}
			अन्यथा यदि (found_m_endpos.col > current_col)
			    current_col = found_m_endpos.col;
			अन्यथा
			    ++current_col;

			// syn_current_attr() will have skipped the check for
			// an item that ends here, need to do that now.  Be
			// careful not to go past the NUL.
			prev_current_col = current_col;
			यदि (syn_getcurline()[current_col] != NUL)
			    ++current_col;
			check_state_ends();
			current_col = prev_current_col;
		    }
		    अन्यथा
			रोक;
		}
	    }

	    /*
	     * If a sync point was encountered, break here.
	     */
	    यदि (found_flags)
	    {
		/*
		 * Put the item that was specified by the sync point on the
		 * state stack.  If there was no item specified, make the
		 * state stack empty.
		 */
		clear_current_state();
		यदि (found_match_idx >= 0
			&& थप_current_state(found_match_idx) == OK)
		    update_si_attr(current_state.ga_len - 1);

		/*
		 * When using "grouphere", continue from the sync point
		 * match, until the end of the line.  Parsing starts at
		 * the next line.
		 * For "groupthere" the parsing starts at start_lnum.
		 */
		यदि (found_flags & HL_SYNC_HERE)
		{
		    current_lnum = found_m_endpos.lnum;
		    current_col = found_m_endpos.col;
		    यदि (current_state.ga_len)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			cur_si->si_h_startpos.lnum = found_current_lnum;
			cur_si->si_h_startpos.col = found_current_col;
			update_si_end(cur_si, (पूर्णाङ्क)current_col, TRUE);
			check_keepend();
		    }
		    (शून्य)syn_finish_पङ्क्ति(FALSE);
		    ++current_lnum;
		}
		अन्यथा
		    current_lnum = start_lnum;

		रोक;
	    }

	    end_lnum = lnum;
	    invalidate_current_state();
	}

	// Ran into start of the file or exceeded maximum number of lines
	यदि (lnum <= break_lnum)
	{
	    invalidate_current_state();
	    current_lnum = break_lnum + 1;
	}
    }

    validate_current_state();
}

    स्थिर शून्य
save_chartab(char_u *chartab)
{
    यदि (syn_block->b_syn_isk == empty_option)
	फर्काऊ;

    mch_memmove(chartab, syn_buf->b_chartab, (माप_प्रकार)32);
    mch_memmove(syn_buf->b_chartab, syn_win->w_s->b_syn_chartab, (माप_प्रकार)32);
}

    स्थिर शून्य
restore_chartab(char_u *chartab)
{
    यदि (syn_win->w_s->b_syn_isk != empty_option)
	mch_memmove(syn_buf->b_chartab, chartab, (माप_प्रकार)32);
}

/*
 * Return TRUE if the line-continuation pattern matches in line "lnum".
 */
    स्थिर पूर्णाङ्क
syn_match_linecont(linenr_T lnum)
{
    regmmatch_T regmatch;
    पूर्णाङ्क r;
    char_u	buf_chartab[32];  // chartab array for syn iskyeyword

    यदि (syn_block->b_syn_linecont_prog == रिक्त)
	फर्काऊ FALSE;

    // use syntax iskeyword option
    save_chartab(buf_chartab);
    regmatch.rmm_ic = syn_block->b_syn_linecont_ic;
    regmatch.regprog = syn_block->b_syn_linecont_prog;
    r = syn_regexec(&regmatch, lnum, (colnr_T)0,
	    IF_SYN_TIME(&syn_block->b_syn_linecont_time));
    syn_block->b_syn_linecont_prog = regmatch.regprog;
    restore_chartab(buf_chartab);
    फर्काऊ r;
}

/*
 * Prepare the current state for the start of a line.
 */
    स्थिर शून्य
syn_start_पङ्क्ति(शून्य)
{
    current_finished = FALSE;
    current_col = 0;

    /*
     * Need to update the end of a start/skip/end that continues from the
     * previous line and regions that have "keepend".
     */
    यदि (current_state.ga_len > 0)
    {
	syn_update_ends(TRUE);
	check_state_ends();
    }

    next_match_idx = -1;
    ++current_पङ्क्ति_id;
#ifdef FEAT_CONCEAL
    next_seqnr = 1;
#endif
}

/*
 * Check for items in the stack that need their end updated.
 * When "startofline" is TRUE the last item is always updated.
 * When "startofline" is FALSE the item with "keepend" is forcefully updated.
 */
    स्थिर शून्य
syn_update_ends(पूर्णाङ्क startofline)
{
    stateitem_T	*cur_si;
    पूर्णाङ्क		i;
    पूर्णाङ्क		seen_keepend;

    यदि (startofline)
    {
	// Check for a match carried over from a previous line with a
	// contained region.  The match ends as soon as the region ends.
	लागि (i = 0; i < current_state.ga_len; ++i)
	{
	    cur_si = &CUR_STATE(i);
	    यदि (cur_si->si_idx >= 0
		    && (SYN_वस्तुहरू(syn_block)[cur_si->si_idx]).sp_type
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
    यदि (keepend_level >= 0)
	लागि ( ; i > keepend_level; --i)
	    यदि (CUR_STATE(i).si_flags & HL_EXTEND)
		रोक;

    seen_keepend = FALSE;
    लागि ( ; i < current_state.ga_len; ++i)
    {
	cur_si = &CUR_STATE(i);
	यदि ((cur_si->si_flags & HL_KEEPEND)
			    || (seen_keepend && !startofline)
			    || (i == current_state.ga_len - 1 && startofline))
	{
	    cur_si->si_h_startpos.col = 0;	// start highl. in col 0
	    cur_si->si_h_startpos.lnum = current_lnum;

	    यदि (!(cur_si->si_flags & HL_MATCHCONT))
		update_si_end(cur_si, (पूर्णाङ्क)current_col, !startofline);

	    यदि (!startofline && (cur_si->si_flags & HL_KEEPEND))
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

    स्थिर शून्य
syn_थाक_free_block(synblock_T *block)
{
    synstate_T	*p;

    // Syntax definitions may have changed: drop the in_id_list() cache.
    idl_cache_clear(block);

    यदि (block->b_sst_array == रिक्त)
	फर्काऊ;

    FOR_ALL_SYNSTATES(block, p)
	clear_syn_state(p);
    VIM_CLEAR(block->b_sst_array);
    block->b_sst_first = रिक्त;
    block->b_sst_खोज = रिक्त;
    block->b_sst_len = 0;
}
/*
 * Free b_sst_array[] for buffer "buf".
 * Used when syntax items changed to force resyncing everywhere.
 */
    शून्य
syn_थाक_free_all(synblock_T *block)
{
#ifdef FEAT_FOLDING
    win_T	*wp;
#endif

    syn_थाक_free_block(block);

#ifdef FEAT_FOLDING
    // When using "syntax" fold method, must update all folds.
    FOR_ALL_WINDOWS(wp)
    {
	यदि (wp->w_s == block && foldmethodIsSyntax(wp))
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
    स्थिर शून्य
syn_थाक_alloc(शून्य)
{
    दीर्घ	len;
    synstate_T	*to, *from;
    synstate_T	*sstp;

    len = syn_buf->b_ml.ml_पङ्क्ति_गन्ती / SST_DIST + Rows * 2;
    यदि (len < SST_MIN_ENTRIES)
	len = SST_MIN_ENTRIES;
    अन्यथा यदि (len > SST_MAX_ENTRIES)
	len = SST_MAX_ENTRIES;
    यदि (syn_block->b_sst_len > len * 2 || syn_block->b_sst_len < len)
    {
	// Allocate 50% too much, to avoid reallocating too often.
	len = syn_buf->b_ml.ml_पङ्क्ति_गन्ती;
	len = (len + len / 2) / SST_DIST + Rows * 2;
	यदि (len < SST_MIN_ENTRIES)
	    len = SST_MIN_ENTRIES;
	अन्यथा यदि (len > SST_MAX_ENTRIES)
	    len = SST_MAX_ENTRIES;

	यदि (syn_block->b_sst_array != रिक्त)
	{
	    // When shrinking the array, cleanup the existing stack.
	    // Make sure that all valid entries fit in the new array.
	    जबसम्म (syn_block->b_sst_len - syn_block->b_sst_freecount + 2 > len
		    && syn_थाक_cleanup())
		;
	    यदि (len < syn_block->b_sst_len - syn_block->b_sst_freecount + 2)
		len = syn_block->b_sst_len - syn_block->b_sst_freecount + 2;
	}

	sstp = ALLOC_CLEAR_MULT(synstate_T, len);
	यदि (sstp == रिक्त)	// out of memory!
	    फर्काऊ;

	to = sstp - 1;
	यदि (syn_block->b_sst_array != रिक्त)
	{
	    // Move the states from the old array to the new one.
	    लागि (from = syn_block->b_sst_first; from != रिक्त;
							from = from->sst_next)
	    {
		++to;
		*to = *from;
		to->sst_next = to + 1;
	    }
	}
	यदि (to != sstp - 1)
	{
	    to->sst_next = रिक्त;
	    syn_block->b_sst_first = sstp;
	    syn_block->b_sst_freecount = len - (पूर्णाङ्क)(to - sstp) - 1;
	}
	अन्यथा
	{
	    syn_block->b_sst_first = रिक्त;
	    syn_block->b_sst_freecount = len;
	}

	// Create the list of free entries.
	syn_block->b_sst_firstfree = to + 1;
	जबसम्म (++to < sstp + len)
	    to->sst_next = to + 1;
	(sstp + len - 1)->sst_next = रिक्त;

	vim_free(syn_block->b_sst_array);
	syn_block->b_sst_array = sstp;
	syn_block->b_sst_len = len;
	// The entries were moved to a new array, drop the stale pointer.
	syn_block->b_sst_खोज = रिक्त;
    }
}

/*
 * Check for changes in a buffer to affect stored syntax states.  Uses the
 * b_mod_* fields.
 * Called from update_screen(), before screen is being updated, once for each
 * displayed buffer.
 */
    शून्य
syn_थाक_apply_changes(buf_T *buf)
{
    win_T	*wp;

    syn_थाक_apply_changes_block(&buf->b_s, buf);

    FOR_ALL_WINDOWS(wp)
    {
	यदि ((wp->w_buffer == buf) && (wp->w_s != &buf->b_s))
	    syn_थाक_apply_changes_block(wp->w_s, buf);
    }
}

    स्थिर शून्य
syn_थाक_apply_changes_block(synblock_T *block, buf_T *buf)
{
    synstate_T	*p, *prev, *np;
    linenr_T	n;

    prev = रिक्त;
    लागि (p = block->b_sst_first; p != रिक्त; )
    {
	यदि (p->sst_lnum + block->b_syn_sync_linebreaks > buf->b_mod_शीर्ष)
	{
	    n = p->sst_lnum + buf->b_mod_xlines;
	    यदि (n <= buf->b_mod_bot)
	    {
		// this state is inside the changed area, remove it
		np = p->sst_next;
		यदि (prev == रिक्त)
		    block->b_sst_first = np;
		अन्यथा
		    prev->sst_next = np;
		syn_थाक_free_entry(block, p);
		p = np;
		जारी;
	    }
	    // This state is below the changed area.  Remember the line
	    // that needs to be parsed before this entry can be made valid
	    // again.
	    यदि (p->sst_change_lnum != 0 && p->sst_change_lnum > buf->b_mod_शीर्ष)
	    {
		यदि (p->sst_change_lnum + buf->b_mod_xlines > buf->b_mod_शीर्ष)
		    p->sst_change_lnum += buf->b_mod_xlines;
		अन्यथा
		    p->sst_change_lnum = buf->b_mod_शीर्ष;
	    }
	    यदि (p->sst_change_lnum == 0
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
    स्थिर पूर्णाङ्क
syn_थाक_cleanup(शून्य)
{
    synstate_T	*p, *prev;
    disptick_T	tick;
    पूर्णाङ्क		above;
    पूर्णाङ्क		dist;
    पूर्णाङ्क		retval = FALSE;

    यदि (syn_block->b_sst_first == रिक्त)
	फर्काऊ retval;

    // Compute normal distance between non-displayed entries.
    यदि (syn_block->b_sst_len <= Rows)
	dist = 999999;
    अन्यथा
	dist = syn_buf->b_ml.ml_पङ्क्ति_गन्ती / (syn_block->b_sst_len - Rows) + 1;

    /*
     * Go through the list to find the "tick" for the oldest entry that can
     * be removed.  Set "above" when the "tick" for the oldest entry is above
     * "b_sst_lasttick" (the display tick wraps around).
     */
    tick = syn_block->b_sst_lasttick;
    above = FALSE;
    prev = syn_block->b_sst_first;
    लागि (p = prev->sst_next; p != रिक्त; prev = p, p = p->sst_next)
    {
	यदि (prev->sst_lnum + dist > p->sst_lnum)
	{
	    यदि (p->sst_tick > syn_block->b_sst_lasttick)
	    {
		यदि (!above || p->sst_tick < tick)
		    tick = p->sst_tick;
		above = TRUE;
	    }
	    अन्यथा यदि (!above && p->sst_tick < tick)
		tick = p->sst_tick;
	}
    }

    /*
     * Go through the list to make the entries for the oldest tick at an
     * interval of several lines.
     */
    prev = syn_block->b_sst_first;
    लागि (p = prev->sst_next; p != रिक्त; prev = p, p = p->sst_next)
    {
	यदि (p->sst_tick == tick && prev->sst_lnum + dist > p->sst_lnum)
	{
	    // Move this entry from used list to free list
	    prev->sst_next = p->sst_next;
	    syn_थाक_free_entry(syn_block, p);
	    p = prev;
	    retval = TRUE;
	}
    }
    फर्काऊ retval;
}

/*
 * Free the allocated memory for a syn_state item.
 * Move the entry into the free list.
 */
    स्थिर शून्य
syn_थाक_free_entry(synblock_T *block, synstate_T *p)
{
    यदि (block->b_sst_खोज == p)
	block->b_sst_खोज = रिक्त;
    clear_syn_state(p);
    p->sst_next = block->b_sst_firstfree;
    block->b_sst_firstfree = p;
    ++block->b_sst_freecount;
}

/*
 * Find an entry in the list of state stacks at or before "lnum".
 * Returns NULL when there is no entry or the first entry is after "lnum".
 */
    स्थिर synstate_T *
syn_थाक_find_entry(linenr_T lnum)
{
    synstate_T	*p, *prev;

    prev = रिक्त;
    p = syn_block->b_sst_first;

    // The list is sorted by line number and lookups while parsing advance
    // monotonically, so resume from the last returned entry instead of
    // rescanning from the start whenever it is at or before "lnum".
    यदि (syn_block->b_sst_खोज != रिक्त
				 && syn_block->b_sst_खोज->sst_lnum <= lnum)
	p = syn_block->b_sst_खोज;

    लागि ( ; p != रिक्त; prev = p, p = p->sst_next)
    {
	यदि (p->sst_lnum == lnum)
	{
	    syn_block->b_sst_खोज = p;
	    फर्काऊ p;
	}
	यदि (p->sst_lnum > lnum)
	    रोक;
    }
    यदि (prev != रिक्त)
	syn_block->b_sst_खोज = prev;
    फर्काऊ prev;
}

/*
 * Try saving the current state in b_sst_array[].
 * The current state must be valid for the start of the current_lnum line!
 */
    स्थिर synstate_T *
store_current_state(शून्य)
{
    पूर्णाङ्क		i;
    synstate_T	*p;
    bufstate_T	*bp;
    stateitem_T	*cur_si;
    synstate_T	*sp = syn_थाक_find_entry(current_lnum);

    /*
     * If the current state contains a start or end pattern that continues
     * from the previous line, we can't use it.  Don't store it then.
     */
    लागि (i = current_state.ga_len - 1; i >= 0; --i)
    {
	cur_si = &CUR_STATE(i);
	यदि (cur_si->si_h_startpos.lnum >= current_lnum
		|| cur_si->si_m_endpos.lnum >= current_lnum
		|| cur_si->si_h_endpos.lnum >= current_lnum
		|| (cur_si->si_end_idx
		    && cur_si->si_eoe_pos.lnum >= current_lnum))
	    रोक;
    }
    यदि (i >= 0)
    {
	यदि (sp != रिक्त)
	{
	    // find "sp" in the list and remove it
	    यदि (syn_block->b_sst_first == sp)
		// it's the first entry
		syn_block->b_sst_first = sp->sst_next;
	    अन्यथा
	    {
		// find the entry just before this one to adjust sst_next
		FOR_ALL_SYNSTATES(syn_block, p)
		    यदि (p->sst_next == sp)
			रोक;
		यदि (p != रिक्त)	// just in case
		    p->sst_next = sp->sst_next;
	    }
	    syn_थाक_free_entry(syn_block, sp);
	    sp = रिक्त;
	}
    }
    अन्यथा यदि (sp == रिक्त || sp->sst_lnum != current_lnum)
    {
	/*
	 * Add a new entry
	 */
	// If no free items, cleanup the array first.
	यदि (syn_block->b_sst_freecount == 0)
	{
	    (शून्य)syn_थाक_cleanup();
	    // "sp" may have been moved to the freelist now
	    sp = syn_थाक_find_entry(current_lnum);
	}
	// Still no free items?  Must be a strange problem...
	यदि (syn_block->b_sst_freecount == 0)
	    sp = रिक्त;
	अन्यथा
	{
	    // Take the first item from the free list and put it in the used
	    // list, after *sp
	    p = syn_block->b_sst_firstfree;
	    syn_block->b_sst_firstfree = p->sst_next;
	    --syn_block->b_sst_freecount;
	    यदि (sp == रिक्त)
	    {
		// Insert in front of the list
		p->sst_next = syn_block->b_sst_first;
		syn_block->b_sst_first = p;
	    }
	    अन्यथा
	    {
		// insert in list after *sp
		p->sst_next = sp->sst_next;
		sp->sst_next = p;
	    }
	    sp = p;
	    sp->sst_stacksize = 0;
	    sp->sst_lnum = current_lnum;
	    // Resume the next forward lookup from the entry just stored.
	    syn_block->b_sst_खोज = sp;
	}
    }
    यदि (sp != रिक्त)
    {
	// When overwriting an existing state stack, clear it first
	clear_syn_state(sp);
	sp->sst_stacksize = current_state.ga_len;
	यदि (current_state.ga_len > SST_FIX_STATES)
	{
	    // Need to clear it, might be something remaining from when the
	    // length was less than SST_FIX_STATES.
	    ga_init2(&sp->sst_union.sst_ga, परिमाण(bufstate_T), 1);
	    यदि (ga_grow(&sp->sst_union.sst_ga, current_state.ga_len) == FAIL)
		sp->sst_stacksize = 0;
	    अन्यथा
		sp->sst_union.sst_ga.ga_len = current_state.ga_len;
	    bp = SYN_STATE_P(&(sp->sst_union.sst_ga));
	}
	अन्यथा
	    bp = sp->sst_union.sst_थाक;
	लागि (i = 0; i < sp->sst_stacksize; ++i)
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
    फर्काऊ sp;
}

/*
 * Copy a state stack from "from" in b_sst_array[] to current_state;
 */
    स्थिर शून्य
load_current_state(synstate_T *from)
{
    पूर्णाङ्क		i;
    bufstate_T	*bp;

    clear_current_state();
    validate_current_state();
    keepend_level = -1;
    यदि (from->sst_stacksize
	    && ga_grow(&current_state, from->sst_stacksize) == OK)
    {
	यदि (from->sst_stacksize > SST_FIX_STATES)
	    bp = SYN_STATE_P(&(from->sst_union.sst_ga));
	अन्यथा
	    bp = from->sst_union.sst_थाक;
	लागि (i = 0; i < from->sst_stacksize; ++i)
	{
	    CUR_STATE(i).si_idx = bp[i].bs_idx;
	    CUR_STATE(i).si_flags = bp[i].bs_flags;
#ifdef FEAT_CONCEAL
	    CUR_STATE(i).si_seqnr = bp[i].bs_seqnr;
	    CUR_STATE(i).si_cchar = bp[i].bs_cchar;
#endif
	    CUR_STATE(i).si_extmatch = ref_extmatch(bp[i].bs_extmatch);
	    यदि (keepend_level < 0 && (CUR_STATE(i).si_flags & HL_KEEPEND))
		keepend_level = i;
	    CUR_STATE(i).si_ends = FALSE;
	    CUR_STATE(i).si_m_lnum = 0;
	    यदि (CUR_STATE(i).si_idx >= 0)
		CUR_STATE(i).si_next_list =
		     (SYN_वस्तुहरू(syn_block)[CUR_STATE(i).si_idx]).sp_next_list;
	    अन्यथा
		CUR_STATE(i).si_next_list = रिक्त;
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
    स्थिर पूर्णाङ्क
syn_थाक_equal(synstate_T *sp)
{
    पूर्णाङ्क		i, j;
    bufstate_T	*bp;
    reg_extmatch_T	*six, *bsx;

    // First a quick check if the stacks have the same size end nextlist.
    यदि (sp->sst_stacksize != current_state.ga_len
	    || sp->sst_next_list != current_next_list)
	फर्काऊ FALSE;

    // Need to compare all states on both stacks.
    यदि (sp->sst_stacksize > SST_FIX_STATES)
	bp = SYN_STATE_P(&(sp->sst_union.sst_ga));
    अन्यथा
	bp = sp->sst_union.sst_थाक;

    लागि (i = current_state.ga_len; --i >= 0; )
    {
	// If the item has another index the state is different.
	यदि (bp[i].bs_idx != CUR_STATE(i).si_idx)
	    रोक;
	यदि (bp[i].bs_extmatch == CUR_STATE(i).si_extmatch)
	    जारी;
	// When the extmatch pointers are different, the strings in them can
	// still be the same.  Check if the extmatch references are equal.
	bsx = bp[i].bs_extmatch;
	six = CUR_STATE(i).si_extmatch;
	// If one of the extmatch pointers is NULL the states are different.
	यदि (bsx == रिक्त || six == रिक्त)
	    रोक;
	लागि (j = 0; j < NSUBEXP; ++j)
	{
	    // Check each referenced match string. They must all be equal.
	    यदि (bsx->matches[j] != six->matches[j])
	    {
		// If the pointer is different it can still be the same text.
		// Compare the strings, ignore case when the start item has the
		// sp_ic flag set.
		यदि (bsx->matches[j] == रिक्त || six->matches[j] == रिक्त)
		    रोक;
		यदि ((SYN_वस्तुहरू(syn_block)[CUR_STATE(i).si_idx]).sp_ic
			? MB_STRICMP(bsx->matches[j], six->matches[j]) != 0
			: STRCMP(bsx->matches[j], six->matches[j]) != 0)
		    रोक;
	    }
	}
	यदि (j != NSUBEXP)
	    रोक;
    }
    फर्काऊ i < 0 ? TRUE : FALSE;
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
    शून्य
syntax_end_parsing(win_T *wp, linenr_T lnum)
{
    synstate_T	*sp;

    यदि (syn_block != wp->w_s)
	फर्काऊ;  // not the right window
    sp = syn_थाक_find_entry(lnum);
    यदि (sp != रिक्त && sp->sst_lnum < lnum)
	sp = sp->sst_next;

    यदि (sp != रिक्त && sp->sst_change_lnum != 0)
	sp->sst_change_lnum = lnum;
}

/*
 * End of handling of the state stack.
 ****************************************/

    स्थिर शून्य
invalidate_current_state(शून्य)
{
    clear_current_state();
    current_state.ga_itemsize = 0;	// mark current_state invalid
    current_next_list = रिक्त;
    keepend_level = -1;
}

    स्थिर शून्य
validate_current_state(शून्य)
{
    current_state.ga_itemsize = परिमाण(stateitem_T);
    current_state.ga_growsize = 3;
}

/*
 * Return TRUE if the syntax at start of lnum changed since last time.
 * This will only be called just after get_syntax_attr() for the previous
 * line, to check if the next line needs to be redrawn too.
 */
    पूर्णाङ्क
syntax_check_changed(linenr_T lnum)
{
    पूर्णाङ्क		retval = TRUE;
    synstate_T	*sp;

    /*
     * Check the state stack when:
     * - lnum is just below the previously syntaxed line.
     * - lnum is not before the lines with saved states.
     * - lnum is not past the lines with saved states.
     * - lnum is at or before the last changed line.
     */
    यदि (VALID_STATE(&current_state) && lnum == current_lnum + 1)
    {
	sp = syn_थाक_find_entry(lnum);
	यदि (sp != रिक्त && sp->sst_lnum == lnum)
	{
	    /*
	     * finish the previous line (needed when not all of the line was
	     * drawn)
	     */
	    (शून्य)syn_finish_पङ्क्ति(FALSE);

	    /*
	     * Compare the current state with the previously saved state of
	     * the line.
	     */
	    यदि (syn_थाक_equal(sp))
		retval = FALSE;

	    /*
	     * Store the current state in b_sst_array[] for later use.
	     */
	    ++current_lnum;
	    (शून्य)store_current_state();
	}
    }

    फर्काऊ retval;
}

/*
 * Finish the current line.
 * This doesn't return any attributes, it only gets the state at the end of
 * the line.  It can start anywhere in the line, as long as the current state
 * is valid.
 */
    स्थिर पूर्णाङ्क
syn_finish_पङ्क्ति(
    पूर्णाङ्क	    syncing)		// called for syncing
{
    stateitem_T	*cur_si;
    colnr_T	prev_current_col;

    जबसम्म (!current_finished)
    {
	(शून्य)syn_current_attr(syncing, FALSE, रिक्त, FALSE);
	/*
	 * When syncing, and found some item, need to check the item.
	 */
	यदि (syncing && current_state.ga_len)
	{
	    /*
	     * Check for match with sync item.
	     */
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	    यदि (cur_si->si_idx >= 0
		    && (SYN_वस्तुहरू(syn_block)[cur_si->si_idx].sp_flags
					  & (HL_SYNC_HERE|HL_SYNC_THERE)))
		फर्काऊ TRUE;

	    // syn_current_attr() will have skipped the check for an item
	    // that ends here, need to do that now.  Be careful not to go
	    // past the NUL.
	    prev_current_col = current_col;
	    यदि (syn_getcurline()[current_col] != NUL)
		++current_col;
	    check_state_ends();
	    current_col = prev_current_col;
	}
	++current_col;
    }
    फर्काऊ FALSE;
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
    पूर्णाङ्क
get_syntax_attr(
    colnr_T	col,
    पूर्णाङ्क		*can_spell,
    पूर्णाङ्क		keep_state)	// keep state of char at "col"
{
    पूर्णाङ्क	    attr = 0;

    यदि (can_spell != रिक्त)
	// Default: Only do spelling when there is no @Spell cluster or when
	// ":syn spell toplevel" was used.
	*can_spell = syn_block->b_syn_spell == SYNSPL_DEFAULT
		    ? (syn_block->b_spell_cluster_id == 0)
		    : (syn_block->b_syn_spell == SYNSPL_शीर्ष);

    // check for out of memory situation
    यदि (syn_block->b_sst_array == रिक्त)
	फर्काऊ 0;

    // After 'synmaxcol' the attribute is always zero.
    यदि (syn_buf->b_p_smc > 0 && col >= (colnr_T)syn_buf->b_p_smc)
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
	फर्काऊ 0;
    }

    // Make sure current_state is valid
    यदि (INVALID_STATE(&current_state))
	validate_current_state();

    /*
     * Skip from the current column to "col", get the attributes for "col".
     */
    जबसम्म (current_col <= col)
    {
	attr = syn_current_attr(FALSE, TRUE, can_spell,
				     current_col == col ? keep_state : FALSE);
	++current_col;
    }

    फर्काऊ attr;
}

/*
 * Get syntax attributes for current_lnum, current_col.
 */
    स्थिर पूर्णाङ्क
syn_current_attr(
    पूर्णाङ्क		syncing,		// When 1: called for syncing
    पूर्णाङ्क		displaying,		// result will be displayed
    पूर्णाङ्क		*can_spell,		// return: do spell checking
    पूर्णाङ्क		keep_state)		// keep syntax stack afterwards
{
    पूर्णाङ्क		syn_id;
    lpos_T	endpos;		// was: char_u *endp;
    lpos_T	hl_startpos;	// was: int hl_startcol;
    lpos_T	hl_endpos;
    lpos_T	eos_pos;	// end-of-start match (start region)
    lpos_T	eoe_pos;	// end-of-end pattern
    पूर्णाङ्क		end_idx;	// group ID for end pattern
    पूर्णाङ्क		idx;
    synpat_T	*spp;
    stateitem_T	*cur_si, *sip = रिक्त;
    पूर्णाङ्क		startcol;
    पूर्णाङ्क		endcol;
    दीर्घ	flags;
    पूर्णाङ्क		cchar;
    लघु	*next_list;
    पूर्णाङ्क		found_match;		    // found usable match
    स्थिर पूर्णाङ्क	try_next_column = FALSE;    // must try in next col
    पूर्णाङ्क		do_keywords;
    regmmatch_T	regmatch;
    lpos_T	pos;
    पूर्णाङ्क		lc_col;
    reg_extmatch_T *cur_extmatch = रिक्त;
    char_u	buf_chartab[32];  // chartab array for syn iskyeyword
    char_u	*पङ्क्ति;		// current line.  NOTE: becomes invalid after
				// looking for a pattern match!

    // variables for zero-width matches that have a "nextgroup" argument
    पूर्णाङ्क		keep_next_list;
    पूर्णाङ्क		zero_width_next_list = FALSE;
    garray_T	zero_width_next_ga;

    /*
     * No character, no attributes!  Past end of line?
     * Do try matching with an empty line (could be the start of a region).
     */
    पङ्क्ति = syn_getcurline();
    यदि (पङ्क्ति[current_col] == NUL && current_col != 0)
    {
	/*
	 * If we found a match after the last column, use it.
	 */
	यदि (next_match_idx >= 0 && next_match_col >= (पूर्णाङ्क)current_col
						  && next_match_col != MAXCOL)
	    (शून्य)थप_next_match(रिक्त);

	current_finished = TRUE;
	current_state_stored = FALSE;
	फर्काऊ 0;
    }

    // if the current or next character is NUL, we will finish the line now
    यदि (पङ्क्ति[current_col] == NUL || पङ्क्ति[current_col + 1] == NUL)
    {
	current_finished = TRUE;
	current_state_stored = FALSE;
    }

    /*
     * When in the previous column there was a match but it could not be used
     * (empty match or already matched in this column) need to try again in
     * the next column.
     */
    यदि (try_next_column)
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
    ga_init2(&zero_width_next_ga, परिमाण(पूर्णाङ्क), 10);

    // use syntax iskeyword option
    save_chartab(buf_chartab);

    /*
     * Repeat matching keywords and patterns, to find contained items at the
     * same column.  This stops when there are no extra matches at the current
     * column.
     */
    गर
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
	यदि (current_state.ga_len)
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	अन्यथा
	    cur_si = रिक्त;

	यदि (syn_block->b_syn_containedin || cur_si == रिक्त
					      || cur_si->si_cont_list != रिक्त)
	{
	    /*
	     * 2. Check for keywords, if on a keyword char after a non-keyword
	     *	  char.  Don't do this when syncing.
	     */
	    यदि (do_keywords)
	    {
	      पङ्क्ति = syn_getcurline();
	      यदि (vim_iswordp_buf(पङ्क्ति + current_col, syn_buf)
		      && (current_col == 0
			  || !vim_iswordp_buf(पङ्क्ति + current_col - 1
			      - (has_mbyte
				  ? (*mb_head_off)(पङ्क्ति, पङ्क्ति + current_col - 1)
				  : 0) , syn_buf)))
	      {
		syn_id = check_keyword_id(पङ्क्ति, (पूर्णाङ्क)current_col,
					 &endcol, &flags, &next_list, cur_si,
					 &cchar);
		यदि (syn_id != 0)
		{
		    यदि (थप_current_state(KEYWORD_IDX) == OK)
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
			यदि (current_state.ga_len > 1)
			    cur_si->si_flags |=
				  CUR_STATE(current_state.ga_len - 2).si_flags
								 & HL_CONCEAL;
#endif
			cur_si->si_id = syn_id;
			cur_si->si_trans_id = syn_id;
			यदि (flags & HL_TRANSP)
			{
			    यदि (current_state.ga_len < 2)
			    {
				cur_si->si_attr = 0;
				cur_si->si_trans_id = 0;
			    }
			    अन्यथा
			    {
				cur_si->si_attr = CUR_STATE(
					current_state.ga_len - 2).si_attr;
				cur_si->si_trans_id = CUR_STATE(
					current_state.ga_len - 2).si_trans_id;
			    }
			}
			अन्यथा
			    cur_si->si_attr = syn_id2attr(syn_id);
			cur_si->si_cont_list = रिक्त;
			cur_si->si_next_list = next_list;
			check_keepend();
		    }
		    अन्यथा
			vim_free(next_list);
		}
	      }
	    }

	    /*
	     * 3. Check for patterns (only if no keyword found).
	     */
	    यदि (syn_id == 0 && syn_block->b_syn_patterns.ga_len)
	    {
		/*
		 * If we didn't check for a match yet, or we are past it, check
		 * for any match with a pattern.
		 */
		यदि (next_match_idx < 0 || next_match_col < (पूर्णाङ्क)current_col)
		{
		    /*
		     * Check all relevant patterns for a match at this
		     * position.  This is complicated, because matching with a
		     * pattern takes quite a bit of time, thus we want to
		     * avoid doing it when it's not needed.
		     */
		    next_match_idx = 0;		// no match in this line yet
		    next_match_col = MAXCOL;
		    लागि (idx = syn_block->b_syn_patterns.ga_len; --idx >= 0; )
		    {
			spp = &(SYN_वस्तुहरू(syn_block)[idx]);
			यदि (	   spp->sp_syncing == syncing
				&& (displaying || !(spp->sp_flags & HL_DISPLAY))
				&& (spp->sp_type == SPTYPE_MATCH
				    || spp->sp_type == SPTYPE_START)
				&& (current_next_list != रिक्त
				    ? in_id_list(रिक्त, current_next_list,
							      &spp->sp_syn, 0)
				    : (cur_si == रिक्त
					? !(spp->sp_flags & HL_CONTAINED)
					: in_id_list(cur_si,
					    cur_si->si_cont_list, &spp->sp_syn,
					    spp->sp_flags))))
			{
			    पूर्णाङ्क r;

			    // If we already tried matching in this line, and
			    // there isn't a match before next_match_col, skip
			    // this item.
			    यदि (spp->sp_पङ्क्ति_id == current_पङ्क्ति_id
				    && spp->sp_startcol >= next_match_col)
				जारी;
			    spp->sp_पङ्क्ति_id = current_पङ्क्ति_id;

			    lc_col = current_col - spp->sp_offsets[SPO_LC_OFF];
			    यदि (lc_col < 0)
				lc_col = 0;

			    regmatch.rmm_ic = spp->sp_ic;
			    regmatch.regprog = spp->sp_prog;
			    r = syn_regexec(&regmatch,
					     current_lnum,
					     (colnr_T)lc_col,
					     IF_SYN_TIME(&spp->sp_time));
			    spp->sp_prog = regmatch.regprog;
			    यदि (!r)
			    {
				// no match in this line, try another one
				spp->sp_startcol = MAXCOL;
				जारी;
			    }

			    /*
			     * Compute the first column of the match.
			     */
			    syn_add_start_off(&pos, &regmatch,
							 spp, SPO_MS_OFF, -1);
			    यदि (pos.lnum > current_lnum)
			    {
				// must have used end of match in a next line,
				// we can't handle that
				spp->sp_startcol = MAXCOL;
				जारी;
			    }
			    startcol = pos.col;

			    // remember the next column where this pattern
			    // matches in the current line
			    spp->sp_startcol = startcol;

			    /*
			     * If a previously found match starts at a lower
			     * column number, don't use this one.
			     */
			    यदि (startcol >= next_match_col)
				जारी;

			    /*
			     * If we matched this pattern at this position
			     * before, skip it.  Must retry in the next
			     * column, because it may match from there.
			     */
			    यदि (did_match_already(idx, &zero_width_next_ga))
			    {
				try_next_column = TRUE;
				जारी;
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
			    re_extmatch_out = रिक्त;

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
			    यदि (spp->sp_type == SPTYPE_START
					      && (spp->sp_flags & HL_ONELINE))
			    {
				lpos_T	startpos;

				startpos = endpos;
				find_endpos(idx, &startpos, &endpos, &hl_endpos,
				    &flags, &eoe_pos, &end_idx, cur_extmatch);
				यदि (endpos.lnum == 0)
				    जारी;	    // not found
			    }

			    /*
			     * For a "match" the size must be > 0 after the
			     * end offset needs has been added.  Except when
			     * syncing.
			     */
			    अन्यथा यदि (spp->sp_type == SPTYPE_MATCH)
			    {
				syn_add_end_off(&hl_endpos, &regmatch, spp,
							       SPO_HE_OFF, 0);
				syn_add_end_off(&endpos, &regmatch, spp,
							       SPO_ME_OFF, 0);
				यदि (endpos.lnum == current_lnum
				      && (पूर्णाङ्क)endpos.col + syncing < startcol)
				{
				    /*
				     * If an empty string is matched, may need
				     * to try matching again at next column.
				     */
				    यदि (regmatch.startpos[0].col
						    == regmatch.endpos[0].col)
					try_next_column = TRUE;
				    जारी;
				}
			    }

			    /*
			     * keep the best match so far in next_match_*
			     */
			    // Highlighting must start after startpos and end
			    // before endpos.
			    यदि (hl_startpos.lnum == current_lnum
					   && (पूर्णाङ्क)hl_startpos.col < startcol)
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
			    cur_extmatch = रिक्त;
			}
		    }
		}

		/*
		 * If we found a match at the current column, use it.
		 */
		यदि (next_match_idx >= 0 && next_match_col == (पूर्णाङ्क)current_col)
		{
		    synpat_T	*lspp;

		    // When a zero-width item matched which has a nextgroup,
		    // don't push the item but set nextgroup.
		    lspp = &(SYN_वस्तुहरू(syn_block)[next_match_idx]);
		    यदि (next_match_m_endpos.lnum == current_lnum
			    && next_match_m_endpos.col == current_col
			    && lspp->sp_next_list != रिक्त)
		    {
			current_next_list = lspp->sp_next_list;
			current_next_flags = lspp->sp_flags;
			keep_next_list = TRUE;
			zero_width_next_list = TRUE;

			// Add the index to a list, so that we can check
			// later that we don't match it again (and cause an
			// endless loop).
			यदि (ga_grow(&zero_width_next_ga, 1) == OK)
			{
			    ((पूर्णाङ्क *)(zero_width_next_ga.ga_data))
				[zero_width_next_ga.ga_len++] = next_match_idx;
			}
			next_match_idx = -1;
		    }
		    अन्यथा
			cur_si = थप_next_match(cur_si);
		    found_match = TRUE;
		}
	    }
	}

	/*
	 * Handle searching for nextgroup match.
	 */
	यदि (current_next_list != रिक्त && !keep_next_list)
	{
	    /*
	     * If a nextgroup was not found, continue looking for one if:
	     * - this is an empty line and the "skipempty" option was given
	     * - we are on white space and the "skipwhite" option was given
	     */
	    यदि (!found_match)
	    {
		पङ्क्ति = syn_getcurline();
		यदि (((current_next_flags & HL_SKIPWHITE)
			    && VIM_ISWHITE(पङ्क्ति[current_col]))
			|| ((current_next_flags & HL_SKIPEMPTY)
			    && *पङ्क्ति == NUL))
		    रोक;
	    }

	    /*
	     * If a nextgroup was found: Use it, and continue looking for
	     * contained matches.
	     * If a nextgroup was not found: Continue looking for a normal
	     * match.
	     * When did set current_next_list for a zero-width item and no
	     * match was found don't loop (would get stuck).
	     */
	    current_next_list = रिक्त;
	    next_match_idx = -1;
	    यदि (!zero_width_next_list)
		found_match = TRUE;
	}

    } जबसम्म (found_match);

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
    यदि (cur_si != रिक्त)
    {
#ifndef FEAT_EVAL
	पूर्णाङ्क	current_trans_id = 0;
#endif
	लागि (idx = current_state.ga_len - 1; idx >= 0; --idx)
	{
	    sip = &CUR_STATE(idx);
	    यदि ((current_lnum > sip->si_h_startpos.lnum
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
		रोक;
	    }
	}

	यदि (can_spell != रिक्त)
	{
	    संरचना sp_syn   sps;

	    /*
	     * set "can_spell" to TRUE if spell checking is supposed to be
	     * done in the current item.
	     */
	    यदि (syn_block->b_spell_cluster_id == 0)
	    {
		// There is no @Spell cluster: Do spelling for items without
		// @NoSpell cluster.
		यदि (syn_block->b_nospell_cluster_id == 0
						     || current_trans_id == 0)
		    *can_spell = (syn_block->b_syn_spell != SYNSPL_NOTOP);
		अन्यथा
		{
		    sps.inc_tag = 0;
		    sps.id = syn_block->b_nospell_cluster_id;
		    sps.cont_in_list = रिक्त;
		    *can_spell = !in_id_list(sip, sip->si_cont_list, &sps, 0);
		}
	    }
	    अन्यथा
	    {
		// The @Spell cluster is defined: Do spelling in items with
		// the @Spell cluster.  But not when @NoSpell is also there.
		// At the toplevel only spell check when ":syn spell toplevel"
		// was used.
		यदि (current_trans_id == 0)
		    *can_spell = (syn_block->b_syn_spell == SYNSPL_शीर्ष);
		अन्यथा
		{
		    sps.inc_tag = 0;
		    sps.id = syn_block->b_spell_cluster_id;
		    sps.cont_in_list = रिक्त;
		    *can_spell = in_id_list(sip, sip->si_cont_list, &sps, 0);

		    यदि (syn_block->b_nospell_cluster_id != 0)
		    {
			sps.id = syn_block->b_nospell_cluster_id;
			यदि (in_id_list(sip, sip->si_cont_list, &sps, 0))
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
	यदि (!syncing && !keep_state)
	{
	    check_state_ends();
	    यदि (current_state.ga_len > 0
				      && syn_getcurline()[current_col] != NUL)
	    {
		++current_col;
		check_state_ends();
		--current_col;
	    }
	}
    }
    अन्यथा यदि (can_spell != रिक्त)
	// Default: Only do spelling when there is no @Spell cluster or when
	// ":syn spell toplevel" was used.
	*can_spell = syn_block->b_syn_spell == SYNSPL_DEFAULT
		    ? (syn_block->b_spell_cluster_id == 0)
		    : (syn_block->b_syn_spell == SYNSPL_शीर्ष);

    // nextgroup ends at end of line, unless "skipnl" or "skipempty" present
    यदि (current_next_list != रिक्त
	    && (पङ्क्ति = syn_getcurline())[current_col] != NUL
	    && पङ्क्ति[current_col + 1] == NUL
	    && !(current_next_flags & (HL_SKIPNL | HL_SKIPEMPTY)))
	current_next_list = रिक्त;

    यदि (zero_width_next_ga.ga_len > 0)
	ga_clear(&zero_width_next_ga);

    // No longer need external matches.  But keep next_match_extmatch.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = रिक्त;
    unref_extmatch(cur_extmatch);

    फर्काऊ current_attr;
}


/*
 * Check if we already matched pattern "idx" at the current column.
 */
    स्थिर पूर्णाङ्क
did_match_already(पूर्णाङ्क idx, garray_T *gap)
{
    पूर्णाङ्क		i;

    लागि (i = current_state.ga_len; --i >= 0; )
	यदि (CUR_STATE(i).si_m_startcol == (पूर्णाङ्क)current_col
		&& CUR_STATE(i).si_m_lnum == (पूर्णाङ्क)current_lnum
		&& CUR_STATE(i).si_idx == idx)
	    फर्काऊ TRUE;

    // Zero-width matches with a nextgroup argument are not put on the syntax
    // stack, and can only be matched once anyway.
    लागि (i = gap->ga_len; --i >= 0; )
	यदि (((पूर्णाङ्क *)(gap->ga_data))[i] == idx)
	    फर्काऊ TRUE;

    फर्काऊ FALSE;
}

/*
 * Push the next match onto the stack.
 */
    स्थिर stateitem_T *
थप_next_match(stateitem_T *cur_si)
{
    synpat_T	*spp;
#ifdef FEAT_CONCEAL
    पूर्णाङ्क		 save_flags;
#endif

    spp = &(SYN_वस्तुहरू(syn_block)[next_match_idx]);

    /*
     * Push the item in current_state stack;
     */
    यदि (थप_current_state(next_match_idx) == OK)
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
	यदि (current_state.ga_len > 1)
	    cur_si->si_flags |=
		    CUR_STATE(current_state.ga_len - 2).si_flags & HL_CONCEAL;
#endif
	cur_si->si_next_list = spp->sp_next_list;
	cur_si->si_extmatch = ref_extmatch(next_match_extmatch);
	यदि (spp->sp_type == SPTYPE_START && !(spp->sp_flags & HL_ONELINE))
	{
	    // Try to find the end pattern in the current line
	    update_si_end(cur_si, (पूर्णाङ्क)(next_match_m_endpos.col), TRUE);
	    check_keepend();
	}
	अन्यथा
	{
	    cur_si->si_m_endpos = next_match_m_endpos;
	    cur_si->si_h_endpos = next_match_h_endpos;
	    cur_si->si_ends = TRUE;
	    cur_si->si_flags |= next_match_flags;
	    cur_si->si_eoe_pos = next_match_eoe_pos;
	    cur_si->si_end_idx = next_match_end_idx;
	}
	यदि (keepend_level < 0 && (cur_si->si_flags & HL_KEEPEND))
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
	यदि (	   spp->sp_type == SPTYPE_START
		&& spp->sp_syn_match_id != 0
		&& थप_current_state(next_match_idx) == OK)
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
	    यदि (cur_si->si_flags & HL_CONCEALENDS)
		cur_si->si_flags |= HL_CONCEAL;
#endif
	    cur_si->si_next_list = रिक्त;
	    check_keepend();
	    update_si_attr(current_state.ga_len - 1);
	}
    }

    next_match_idx = -1;	// try other match next time

    फर्काऊ cur_si;
}

/*
 * Check for end of current state (and the states before it).
 */
    स्थिर शून्य
check_state_ends(शून्य)
{
    stateitem_T	*cur_si;
    पूर्णाङ्क		had_extend;

    cur_si = &CUR_STATE(current_state.ga_len - 1);
    लागि (;;)
    {
	यदि (cur_si->si_ends
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
	    यदि (cur_si->si_end_idx
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
		यदि (cur_si->si_flags & HL_CONCEALENDS)
		    cur_si->si_flags |= HL_CONCEAL;
#endif
		update_si_attr(current_state.ga_len - 1);

		// nextgroup= should not match in the end pattern
		current_next_list = रिक्त;

		// what matches next may be different now, clear it
		next_match_idx = 0;
		next_match_col = MAXCOL;
		रोक;
	    }

	    // handle next_list, unless at end of line and no "skipnl" or
	    // "skipempty"
	    current_next_list = cur_si->si_next_list;
	    current_next_flags = cur_si->si_flags;
	    यदि (!(current_next_flags & (HL_SKIPNL | HL_SKIPEMPTY))
		    && syn_getcurline()[current_col] == NUL)
		current_next_list = रिक्त;

	    // When the ended item has "extend", another item with
	    // "keepend" now needs to check for its end.
	    had_extend = (cur_si->si_flags & HL_EXTEND);

	    झिक_current_state();

	    यदि (current_state.ga_len == 0)
		रोक;

	    यदि (had_extend && keepend_level >= 0)
	    {
		syn_update_ends(FALSE);
		यदि (current_state.ga_len == 0)
		    रोक;
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
	    यदि (cur_si->si_idx >= 0
		    && SYN_वस्तुहरू(syn_block)[cur_si->si_idx].sp_type
								== SPTYPE_START
		    && !(cur_si->si_flags & (HL_MATCH | HL_KEEPEND)))
	    {
		update_si_end(cur_si, (पूर्णाङ्क)current_col, TRUE);
		check_keepend();
		यदि ((current_next_flags & HL_HAS_EOL)
			&& keepend_level < 0
			&& syn_getcurline()[current_col] == NUL)
		    रोक;
	    }
	}
	अन्यथा
	    रोक;
    }
}

/*
 * Update an entry in the current_state stack for a match or region.  This
 * fills in si_attr, si_next_list and si_cont_list.
 */
    स्थिर शून्य
update_si_attr(पूर्णाङ्क idx)
{
    stateitem_T	*sip = &CUR_STATE(idx);
    synpat_T	*spp;

    // This should not happen...
    यदि (sip->si_idx < 0)
	फर्काऊ;

    spp = &(SYN_वस्तुहरू(syn_block)[sip->si_idx]);
    यदि (sip->si_flags & HL_MATCH)
	sip->si_id = spp->sp_syn_match_id;
    अन्यथा
	sip->si_id = spp->sp_syn.id;
    sip->si_attr = syn_id2attr(sip->si_id);
    sip->si_trans_id = sip->si_id;
    यदि (sip->si_flags & HL_MATCH)
	sip->si_cont_list = रिक्त;
    अन्यथा
	sip->si_cont_list = spp->sp_cont_list;

    /*
     * For transparent items, take attr from outer item.
     * Also take cont_list, if there is none.
     * Don't do this for the matchgroup of a start or end pattern.
     */
    यदि ((spp->sp_flags & HL_TRANSP) && !(sip->si_flags & HL_MATCH))
    {
	यदि (idx == 0)
	{
	    sip->si_attr = 0;
	    sip->si_trans_id = 0;
	    यदि (sip->si_cont_list == रिक्त)
		sip->si_cont_list = ID_LIST_ALL;
	}
	अन्यथा
	{
	    sip->si_attr = CUR_STATE(idx - 1).si_attr;
	    sip->si_trans_id = CUR_STATE(idx - 1).si_trans_id;
	    यदि (sip->si_cont_list == रिक्त)
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
    स्थिर शून्य
check_keepend(शून्य)
{
    पूर्णाङ्क		i;
    lpos_T	maxpos;
    lpos_T	maxpos_h;
    stateitem_T	*sip;

    /*
     * This check can consume a lot of time; only do it from the level where
     * there really is a keepend.
     */
    यदि (keepend_level < 0)
	फर्काऊ;

    /*
     * Find the last index of an "extend" item.  "keepend" items before that
     * won't do anything.  If there is no "extend" item "i" will be
     * "keepend_level" and all "keepend" items will work normally.
     */
    लागि (i = current_state.ga_len - 1; i > keepend_level; --i)
	यदि (CUR_STATE(i).si_flags & HL_EXTEND)
	    रोक;

    maxpos.lnum = 0;
    maxpos.col = 0;
    maxpos_h.lnum = 0;
    maxpos_h.col = 0;
    लागि ( ; i < current_state.ga_len; ++i)
    {
	sip = &CUR_STATE(i);
	यदि (maxpos.lnum != 0)
	{
	    limit_pos_zero(&sip->si_m_endpos, &maxpos);
	    limit_pos_zero(&sip->si_h_endpos, &maxpos_h);
	    limit_pos_zero(&sip->si_eoe_pos, &maxpos);
	    sip->si_ends = TRUE;
	}
	यदि (sip->si_ends && (sip->si_flags & HL_KEEPEND))
	{
	    यदि (maxpos.lnum == 0
		    || maxpos.lnum > sip->si_m_endpos.lnum
		    || (maxpos.lnum == sip->si_m_endpos.lnum
			&& maxpos.col > sip->si_m_endpos.col))
		maxpos = sip->si_m_endpos;
	    यदि (maxpos_h.lnum == 0
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
    स्थिर शून्य
update_si_end(
    stateitem_T	*sip,
    पूर्णाङ्क		startcol,   // where to start searching for the end
    पूर्णाङ्क		force)	    // when TRUE overrule a previous end
{
    lpos_T	startpos;
    lpos_T	endpos;
    lpos_T	hl_endpos;
    lpos_T	end_endpos;
    पूर्णाङ्क		end_idx;

    // return quickly for a keyword
    यदि (sip->si_idx < 0)
	फर्काऊ;

    // Don't update when it's already done.  Can be a match of an end pattern
    // that started in a previous line.  Watch out: can also be a "keepend"
    // from a containing item.
    यदि (!force && sip->si_m_endpos.lnum >= current_lnum)
	फर्काऊ;

    /*
     * We need to find the end of the region.  It may continue in the next
     * line.
     */
    end_idx = 0;
    startpos.lnum = current_lnum;
    startpos.col = startcol;
    find_endpos(sip->si_idx, &startpos, &endpos, &hl_endpos,
		   &(sip->si_flags), &end_endpos, &end_idx, sip->si_extmatch);

    यदि (endpos.lnum == 0)
    {
	// No end pattern matched.
	यदि (SYN_वस्तुहरू(syn_block)[sip->si_idx].sp_flags & HL_ONELINE)
	{
	    // a "oneline" never continues in the next line
	    sip->si_ends = TRUE;
	    sip->si_m_endpos.lnum = current_lnum;
	    sip->si_m_endpos.col = syn_getcurline_len();
	}
	अन्यथा
	{
	    // continues in the next line
	    sip->si_ends = FALSE;
	    sip->si_m_endpos.lnum = 0;
	}
	sip->si_h_endpos = sip->si_m_endpos;
    }
    अन्यथा
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
    स्थिर पूर्णाङ्क
थप_current_state(पूर्णाङ्क idx)
{
    यदि (ga_grow(&current_state, 1) == FAIL)
	फर्काऊ FAIL;
    CLEAR_POINTER(&CUR_STATE(current_state.ga_len));
    CUR_STATE(current_state.ga_len).si_idx = idx;
    ++current_state.ga_len;
    फर्काऊ OK;
}

/*
 * Remove a state from the current_state stack.
 */
    स्थिर शून्य
झिक_current_state(शून्य)
{
    यदि (current_state.ga_len)
    {
	unref_extmatch(CUR_STATE(current_state.ga_len - 1).si_extmatch);
	--current_state.ga_len;
    }
    // after the end of a pattern, try matching a keyword or pattern
    next_match_idx = -1;

    // if first state with "keepend" is popped, reset keepend_level
    यदि (keepend_level >= current_state.ga_len)
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
    स्थिर शून्य
find_endpos(
    पूर्णाङ्क		idx,		// index of the pattern
    lpos_T	*startpos,	// where to start looking for an END match
    lpos_T	*m_endpos,	// return: end of match
    lpos_T	*hl_endpos,	// return: end of highlighting
    दीर्घ	*flagsp,	// return: flags of matching END
    lpos_T	*end_endpos,	// return: end of end pattern match
    पूर्णाङ्क		*end_idx,	// return: group ID for end pat. match, or 0
    reg_extmatch_T *start_ext)	// submatches from the start pattern
{
    colnr_T	matchcol;
    synpat_T	*spp, *spp_skip;
    पूर्णाङ्क		start_idx;
    पूर्णाङ्क		best_idx;
    regmmatch_T	regmatch;
    regmmatch_T	best_regmatch;	    // startpos/endpos of best match
    lpos_T	pos;
    पूर्णाङ्क		had_match = FALSE;
    char_u	buf_chartab[32];  // chartab array for syn option iskyeyword

    // just in case we are invoked for a keyword
    यदि (idx < 0)
	फर्काऊ;

    /*
     * Check for being called with a START pattern.
     * Can happen with a match that continues to the next line, because it
     * contained a region.
     */
    spp = &(SYN_वस्तुहरू(syn_block)[idx]);
    यदि (spp->sp_type != SPTYPE_START)
    {
	*hl_endpos = *startpos;
	फर्काऊ;
    }

    /*
     * Find the SKIP or first END pattern after the last START pattern.
     */
    लागि (;;)
    {
	spp = &(SYN_वस्तुहरू(syn_block)[idx]);
	यदि (spp->sp_type != SPTYPE_START)
	    रोक;
	++idx;
    }

    /*
     *	Lookup the SKIP pattern (if present)
     */
    यदि (spp->sp_type == SPTYPE_SKIP)
    {
	spp_skip = spp;
	++idx;
    }
    अन्यथा
	spp_skip = रिक्त;

    // Setup external matches for syn_regexec().
    unref_extmatch(re_extmatch_in);
    re_extmatch_in = ref_extmatch(start_ext);

    matchcol = startpos->col;	// start looking for a match at sstart
    start_idx = idx;		// remember the first END pattern.
    best_regmatch.startpos[0].col = 0;		// avoid compiler warning

    // use syntax iskeyword option
    save_chartab(buf_chartab);

    लागि (;;)
    {
	/*
	 * Find end pattern that matches first after "matchcol".
	 */
	best_idx = -1;
	लागि (idx = start_idx; idx < syn_block->b_syn_patterns.ga_len; ++idx)
	{
	    पूर्णाङ्क lc_col = matchcol;
	    पूर्णाङ्क r;

	    spp = &(SYN_वस्तुहरू(syn_block)[idx]);
	    यदि (spp->sp_type != SPTYPE_END)	// past last END pattern
		रोक;
	    lc_col -= spp->sp_offsets[SPO_LC_OFF];
	    यदि (lc_col < 0)
		lc_col = 0;

	    regmatch.rmm_ic = spp->sp_ic;
	    regmatch.regprog = spp->sp_prog;
	    r = syn_regexec(&regmatch, startpos->lnum, lc_col,
						  IF_SYN_TIME(&spp->sp_time));
	    spp->sp_prog = regmatch.regprog;
	    यदि (r)
	    {
		यदि (best_idx == -1 || regmatch.startpos[0].col
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
	यदि (best_idx == -1)
	    रोक;

	/*
	 * If the skip pattern matches before the end pattern,
	 * continue searching after the skip pattern.
	 */
	यदि (spp_skip != रिक्त)
	{
	    पूर्णाङ्क lc_col = matchcol - spp_skip->sp_offsets[SPO_LC_OFF];
	    पूर्णाङ्क r;

	    यदि (lc_col < 0)
		lc_col = 0;
	    regmatch.rmm_ic = spp_skip->sp_ic;
	    regmatch.regprog = spp_skip->sp_prog;
	    r = syn_regexec(&regmatch, startpos->lnum, lc_col,
					      IF_SYN_TIME(&spp_skip->sp_time));
	    spp_skip->sp_prog = regmatch.regprog;
	    यदि (r && regmatch.startpos[0].col
					     <= best_regmatch.startpos[0].col)
	    {
		पूर्णाङ्क पङ्क्ति_len;

		// Add offset to skip pattern match
		syn_add_end_off(&pos, &regmatch, spp_skip, SPO_ME_OFF, 1);

		// If the skip pattern goes on to the next line, there is no
		// match with an end pattern in this line.
		यदि (pos.lnum > startpos->lnum)
		    रोक;

		पङ्क्ति_len = ml_get_buf_len(syn_buf, startpos->lnum);

		// take care of an empty match or negative offset
		यदि (pos.col <= matchcol)
		    ++matchcol;
		अन्यथा यदि (pos.col <= regmatch.endpos[0].col)
		    matchcol = pos.col;
		अन्यथा
		    // Be careful not to jump over the NUL at the end-of-line
		    लागि (matchcol = regmatch.endpos[0].col;
			    matchcol < पङ्क्ति_len && matchcol < pos.col;
								   ++matchcol)
			;

		// if the skip pattern includes end-of-line, break here
		यदि (matchcol >= पङ्क्ति_len)
		    रोक;

		जारी;	    // start with first end pattern again
	    }
	}

	/*
	 * Match from start pattern to end pattern.
	 * Correct for match and highlight offset of end pattern.
	 */
	spp = &(SYN_वस्तुहरू(syn_block)[best_idx]);
	syn_add_end_off(m_endpos, &best_regmatch, spp, SPO_ME_OFF, 1);
	// can't end before the start
	यदि (m_endpos->lnum == startpos->lnum && m_endpos->col < startpos->col)
	    m_endpos->col = startpos->col;

	syn_add_end_off(end_endpos, &best_regmatch, spp, SPO_HE_OFF, 1);
	// can't end before the start
	यदि (end_endpos->lnum == startpos->lnum
					   && end_endpos->col < startpos->col)
	    end_endpos->col = startpos->col;
	// can't end after the match
	limit_pos(end_endpos, m_endpos);

	/*
	 * If the end group is highlighted differently, adjust the pointers.
	 */
	यदि (spp->sp_syn_match_id != spp->sp_syn.id && spp->sp_syn_match_id != 0)
	{
	    *end_idx = best_idx;
	    यदि (spp->sp_off_flags & (1 << (SPO_RE_OFF + SPO_गन्ती)))
	    {
		hl_endpos->lnum = best_regmatch.endpos[0].lnum;
		hl_endpos->col = best_regmatch.endpos[0].col;
	    }
	    अन्यथा
	    {
		hl_endpos->lnum = best_regmatch.startpos[0].lnum;
		hl_endpos->col = best_regmatch.startpos[0].col;
	    }
	    hl_endpos->col += spp->sp_offsets[SPO_RE_OFF];

	    // can't end before the start
	    यदि (hl_endpos->lnum == startpos->lnum
					    && hl_endpos->col < startpos->col)
		hl_endpos->col = startpos->col;
	    limit_pos(hl_endpos, m_endpos);

	    // now the match ends where the highlighting ends, it is turned
	    // into the matchgroup for the end
	    *m_endpos = *hl_endpos;
	}
	अन्यथा
	{
	    *end_idx = 0;
	    *hl_endpos = *end_endpos;
	}

	*flagsp = spp->sp_flags;

	had_match = TRUE;
	रोक;
    }

    // no match for an END pattern in this line
    यदि (!had_match)
	m_endpos->lnum = 0;

    restore_chartab(buf_chartab);

    // Remove external matches.
    unref_extmatch(re_extmatch_in);
    re_extmatch_in = रिक्त;
}

/*
 * Limit "pos" not to be after "limit".
 */
    स्थिर शून्य
limit_pos(lpos_T *pos, lpos_T *limit)
{
    यदि (pos->lnum > limit->lnum)
	*pos = *limit;
    अन्यथा यदि (pos->lnum == limit->lnum && pos->col > limit->col)
	pos->col = limit->col;
}

/*
 * Limit "pos" not to be after "limit", unless pos->lnum is zero.
 */
    स्थिर शून्य
limit_pos_zero(
    lpos_T	*pos,
    lpos_T	*limit)
{
    यदि (pos->lnum == 0)
	*pos = *limit;
    अन्यथा
	limit_pos(pos, limit);
}

/*
 * Add offset to matched text for end of match or highlight.
 */
    स्थिर शून्य
syn_add_end_off(
    lpos_T	*result,	// returned position
    regmmatch_T	*regmatch,	// start/end of match
    synpat_T	*spp,		// matched pattern
    पूर्णाङ्क		idx,		// index of offset
    पूर्णाङ्क		extra)		// extra chars for offset to start
{
    पूर्णाङ्क		col;
    पूर्णाङ्क		off;
    char_u	*base;
    char_u	*p;

    यदि (spp->sp_off_flags & (1 << idx))
    {
	result->lnum = regmatch->startpos[0].lnum;
	col = regmatch->startpos[0].col;
	off = spp->sp_offsets[idx] + extra;
    }
    अन्यथा
    {
	result->lnum = regmatch->endpos[0].lnum;
	col = regmatch->endpos[0].col;
	off = spp->sp_offsets[idx];
    }
    // Don't go past the end of the line.  Matters for "rs=e+2" when there
    // is a matchgroup. Watch out for match with last NL in the buffer.
    यदि (result->lnum > syn_buf->b_ml.ml_पङ्क्ति_गन्ती)
	col = 0;
    अन्यथा यदि (off != 0)
    {
	base = ml_get_buf(syn_buf, result->lnum, FALSE);
	p = base + col;
	यदि (off > 0)
	{
	    जबसम्म (off-- > 0 && *p != NUL)
		MB_PTR_ADV(p);
	}
	अन्यथा यदि (off < 0)
	{
	    जबसम्म (off++ < 0 && base < p)
		MB_PTR_BACK(base, p);
	}
	col = (पूर्णाङ्क)(p - base);
    }
    result->col = col;
}

/*
 * Add offset to matched text for start of match or highlight.
 * Avoid resulting column to become negative.
 */
    स्थिर शून्य
syn_add_start_off(
    lpos_T	*result,	// returned position
    regmmatch_T	*regmatch,	// start/end of match
    synpat_T	*spp,
    पूर्णाङ्क		idx,
    पूर्णाङ्क		extra)	    // extra chars for offset to end
{
    पूर्णाङ्क		col;
    पूर्णाङ्क		off;
    char_u	*base;
    char_u	*p;

    यदि (spp->sp_off_flags & (1 << (idx + SPO_गन्ती)))
    {
	result->lnum = regmatch->endpos[0].lnum;
	col = regmatch->endpos[0].col;
	off = spp->sp_offsets[idx] + extra;
    }
    अन्यथा
    {
	result->lnum = regmatch->startpos[0].lnum;
	col = regmatch->startpos[0].col;
	off = spp->sp_offsets[idx];
    }
    यदि (result->lnum > syn_buf->b_ml.ml_पङ्क्ति_गन्ती)
    {
	// a "\n" at the end of the pattern may take us below the last line
	result->lnum = syn_buf->b_ml.ml_पङ्क्ति_गन्ती;
	col = ml_get_buf_len(syn_buf, result->lnum);
    }
    यदि (off != 0)
    {
	base = ml_get_buf(syn_buf, result->lnum, FALSE);
	p = base + col;
	यदि (off > 0)
	{
	    जबसम्म (off-- && *p != NUL)
		MB_PTR_ADV(p);
	}
	अन्यथा यदि (off < 0)
	{
	    जबसम्म (off++ && base < p)
		MB_PTR_BACK(base, p);
	}
	col = (पूर्णाङ्क)(p - base);
    }
    result->col = col;
}

/*
 * Get current line in syntax buffer.
 */
    स्थिर char_u *
syn_getcurline(शून्य)
{
    फर्काऊ ml_get_buf(syn_buf, current_lnum, FALSE);
}

/*
 * Get length of current line in syntax buffer.
 */
    स्थिर colnr_T
syn_getcurline_len(शून्य)
{
    फर्काऊ ml_get_buf_len(syn_buf, current_lnum);
}

/*
 * Call vim_regexec() to find a match with "rmp" in "syn_buf".
 * Returns TRUE when there is a match.
 */
    स्थिर पूर्णाङ्क
syn_regexec(
    regmmatch_T	*rmp,
    linenr_T	lnum,
    colnr_T	col,
    syn_time_T  *st UNUSED)
{
    पूर्णाङ्क		r;
    पूर्णाङ्क		timed_out = FALSE;
#ifdef FEAT_PROFILE
    proftime_T	pt;

    यदि (syn_time_on)
	profile_start(&pt);
#endif

    यदि (rmp->regprog == रिक्त)
	// This can happen if a previous call to vim_regexec_multi() tried to
	// use the NFA engine, which resulted in NFA_TOO_EXPENSIVE, and
	// compiling the pattern with the other engine fails.
	फर्काऊ FALSE;

    rmp->rmm_maxcol = syn_buf->b_p_smc;
    r = vim_regexec_multi(rmp, syn_win, syn_buf, lnum, col, &timed_out);

#ifdef FEAT_PROFILE
    यदि (syn_time_on)
    {
	profile_end(&pt);
	profile_add(&st->total, &pt);
	यदि (profile_cmp(&pt, &st->slowest) < 0)
	    st->slowest = pt;
	++st->गन्ती;
	यदि (r > 0)
	    ++st->match;
    }
#endif
#ifdef FEAT_RELTIME
    यदि (timed_out && redrawtime_limit_set && !syn_win->w_s->b_syn_slow)
    {
	syn_win->w_s->b_syn_slow = TRUE;
	सन्देश(_("'redrawtime' exceeded, syntax highlighting disabled"));
    }
#endif

    यदि (r > 0)
    {
	rmp->startpos[0].lnum += lnum;
	rmp->endpos[0].lnum += lnum;
	फर्काऊ TRUE;
    }
    फर्काऊ FALSE;
}

/*
 * Check one position in a line for a matching keyword.
 * The caller must check if a keyword can start at startcol.
 * Return its ID if found, 0 otherwise.
 */
    स्थिर पूर्णाङ्क
check_keyword_id(
    char_u	*पङ्क्ति,
    पूर्णाङ्क		startcol,	// position in line to check for keyword
    पूर्णाङ्क		*endcolp,	// return: character after found keyword
    दीर्घ	*flagsp,	// return: flags of matching keyword
    लघु	**next_listp,	// return: next_list of matching keyword
    stateitem_T	*cur_si,	// item at the top of the stack
    पूर्णाङ्क		*ccharp UNUSED)	// conceal substitution char
{
    keyentry_T	*kp;
    char_u	*kwp;
    पूर्णाङ्क		round;
    पूर्णाङ्क		kwlen;
    char_u	keyword[MAXKEYWLEN + 1]; // assume max. keyword len is 80
    hashtab_T	*ht;
    hashitem_T	*hi;

    // Find first character after the keyword.  First character was already
    // checked.
    kwp = पङ्क्ति + startcol;
    kwlen = 0;
    गर
    {
	यदि (has_mbyte)
	    kwlen += (*mb_ptr2len)(kwp + kwlen);
	अन्यथा
	    ++kwlen;
    }
    जबसम्म (vim_iswordp_buf(kwp + kwlen, syn_buf));

    यदि (kwlen > MAXKEYWLEN)
	फर्काऊ 0;

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
    लागि (round = 1; round <= 2; ++round)
    {
	ht = round == 1 ? &syn_block->b_keywtab : &syn_block->b_keywtab_ic;
	यदि (ht->ht_used == 0)
	    जारी;
	यदि (round == 2)	// ignore case
	    (शून्य)str_foldcase(kwp, kwlen, keyword, MAXKEYWLEN + 1);

	/*
	 * Find keywords that match.  There can be several with different
	 * attributes.
	 * When current_next_list is non-zero accept only that group, otherwise:
	 *  Accept a not-contained keyword at toplevel.
	 *  Accept a keyword at other levels only if it is in the contains list.
	 */
	hi = hash_find(ht, keyword);
	यदि (!HASHITEM_EMPTY(hi))
	    लागि (kp = HI2KE(hi); kp != रिक्त; kp = kp->ke_next)
	    {
		यदि (current_next_list != 0
			? in_id_list(रिक्त, current_next_list, &kp->k_syn, 0)
			: (cur_si == रिक्त
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
		    फर्काऊ kp->k_syn.id;
		}
	    }
    }
    फर्काऊ 0;
}

/*
 * Handle ":syntax conceal" command.
 */
    स्थिर शून्य
syn_cmd_conceal(exarg_T *eap UNUSED, पूर्णाङ्क syncing UNUSED)
{
#ifdef FEAT_CONCEAL
    char_u	*arg = eap->arg;
    char_u	*next;

    eap->nextcmd = find_nextcmd(arg);
    यदि (eap->skip)
	फर्काऊ;

    next = skiptowhite(arg);
    यदि (*arg == NUL)
    {
	यदि (curwin->w_s->b_syn_conceal)
	    सन्देश("syntax conceal on");
	अन्यथा
	    सन्देश("syntax conceal off");
    }
    अन्यथा यदि (STRNICMP(arg, "on", 2) == 0 && next - arg == 2)
	curwin->w_s->b_syn_conceal = TRUE;
    अन्यथा यदि (STRNICMP(arg, "off", 3) == 0 && next - arg == 3)
	curwin->w_s->b_syn_conceal = FALSE;
    अन्यथा
	semsg(_(e_illegal_argument_str_2), arg);
#endif
}

/*
 * Handle ":syntax case" command.
 */
    स्थिर शून्य
syn_cmd_case(exarg_T *eap, पूर्णाङ्क syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*next;

    eap->nextcmd = find_nextcmd(arg);
    यदि (eap->skip)
	फर्काऊ;

    next = skiptowhite(arg);
    यदि (*arg == NUL)
    {
	यदि (curwin->w_s->b_syn_ic)
	    सन्देश("syntax case ignore");
	अन्यथा
	    सन्देश("syntax case match");
    }
    अन्यथा यदि (STRNICMP(arg, "match", 5) == 0 && next - arg == 5)
	curwin->w_s->b_syn_ic = FALSE;
    अन्यथा यदि (STRNICMP(arg, "ignore", 6) == 0 && next - arg == 6)
	curwin->w_s->b_syn_ic = TRUE;
    अन्यथा
	semsg(_(e_illegal_argument_str_2), arg);
}

/*
 * Handle ":syntax foldlevel" command.
 */
    स्थिर शून्य
syn_cmd_foldlevel(exarg_T *eap, पूर्णाङ्क syncing UNUSED)
{
    char_u *arg = eap->arg;
    char_u *arg_end;

    eap->nextcmd = find_nextcmd(arg);
    यदि (eap->skip)
	फर्काऊ;

    यदि (*arg == NUL)
    {
	विकल्प (curwin->w_s->b_syn_foldlevel)
	{
	    पक्ष SYNFLD_START:   सन्देश("syntax foldlevel start");   रोक;
	    पक्ष SYNFLD_MINIMUM: सन्देश("syntax foldlevel minimum"); रोक;
	    स्वभाव: रोक;
	}
	फर्काऊ;
    }

    arg_end = skiptowhite(arg);
    यदि (STRNICMP(arg, "start", 5) == 0 && arg_end - arg == 5)
	curwin->w_s->b_syn_foldlevel = SYNFLD_START;
    अन्यथा यदि (STRNICMP(arg, "minimum", 7) == 0 && arg_end - arg == 7)
	curwin->w_s->b_syn_foldlevel = SYNFLD_MINIMUM;
    अन्यथा
    {
	semsg(_(e_illegal_argument_str_2), arg);
	फर्काऊ;
    }

    arg = skipwhite(arg_end);
    यदि (*arg != NUL)
    {
	semsg(_(e_illegal_argument_str_2), arg);
    }
}

/*
 * Handle ":syntax spell" command.
 */
    स्थिर शून्य
syn_cmd_spell(exarg_T *eap, पूर्णाङ्क syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*next;

    eap->nextcmd = find_nextcmd(arg);
    यदि (eap->skip)
	फर्काऊ;

    next = skiptowhite(arg);
    यदि (*arg == NUL)
    {
	यदि (curwin->w_s->b_syn_spell == SYNSPL_शीर्ष)
	    सन्देश("syntax spell toplevel");
	अन्यथा यदि (curwin->w_s->b_syn_spell == SYNSPL_NOTOP)
	    सन्देश("syntax spell notoplevel");
	अन्यथा
	    सन्देश("syntax spell default");
    }
    अन्यथा यदि (STRNICMP(arg, "toplevel", 8) == 0 && next - arg == 8)
	curwin->w_s->b_syn_spell = SYNSPL_शीर्ष;
    अन्यथा यदि (STRNICMP(arg, "notoplevel", 10) == 0 && next - arg == 10)
	curwin->w_s->b_syn_spell = SYNSPL_NOTOP;
    अन्यथा यदि (STRNICMP(arg, "default", 7) == 0 && next - arg == 7)
	curwin->w_s->b_syn_spell = SYNSPL_DEFAULT;
    अन्यथा
    {
	semsg(_(e_illegal_argument_str_2), arg);
	फर्काऊ;
    }

    // assume spell checking changed, force a redraw
    redraw_win_later(curwin, UPD_NOT_VALID);
}

/*
 * Handle ":syntax iskeyword" command.
 */
    स्थिर शून्य
syn_cmd_iskeyword(exarg_T *eap, पूर्णाङ्क syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	save_chartab[32];
    char_u	*save_isk;

    यदि (eap->skip)
	फर्काऊ;

    arg = skipwhite(arg);
    यदि (*arg == NUL)
    {
	सन्देश_puts("\n");
	यदि (curwin->w_s->b_syn_isk != empty_option)
	{
	    सन्देश_puts("syntax iskeyword ");
	    सन्देश_outtrans(curwin->w_s->b_syn_isk);
	}
	अन्यथा
	    सन्देश_outtrans((char_u *)_("syntax iskeyword not set"));
    }
    अन्यथा
    {
	यदि (STRNICMP(arg, "clear", 5) == 0)
	{
	    mch_memmove(curwin->w_s->b_syn_chartab, curbuf->b_chartab,
								  (माप_प्रकार)32);
	    clear_string_option(&curwin->w_s->b_syn_isk);
	}
	अन्यथा
	{
	    यदि (check_isopt(arg) == FAIL)
	    {
		emsg(_(e_invalid_argument));
		फर्काऊ;
	    }
	    mch_memmove(save_chartab, curbuf->b_chartab, (माप_प्रकार)32);
	    save_isk = curbuf->b_p_isk;
	    curbuf->b_p_isk = vim_strsave(arg);

	    buf_init_chartab(curbuf, FALSE);
	    mch_memmove(curwin->w_s->b_syn_chartab, curbuf->b_chartab,
								  (माप_प्रकार)32);
	    mch_memmove(curbuf->b_chartab, save_chartab, (माप_प्रकार)32);
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
    शून्य
syntax_clear(synblock_T *block)
{
    पूर्णाङ्क i;

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
    लागि (i = block->b_syn_patterns.ga_len; --i >= 0; )
	syn_clear_pattern(block, i);
    ga_clear(&block->b_syn_patterns);

    // free the syntax clusters
    लागि (i = block->b_syn_clusters.ga_len; --i >= 0; )
	syn_clear_cluster(block, i);
    ga_clear(&block->b_syn_clusters);
    block->b_spell_cluster_id = 0;
    block->b_nospell_cluster_id = 0;

    block->b_syn_sync_flags = 0;
    block->b_syn_sync_minlines = 0;
    block->b_syn_sync_maxlines = 0;
    block->b_syn_sync_linebreaks = 0;

    vim_regfree(block->b_syn_linecont_prog);
    block->b_syn_linecont_prog = रिक्त;
    VIM_CLEAR(block->b_syn_linecont_pat);
#ifdef FEAT_FOLDING
    block->b_syn_folditems = 0;
#endif
    clear_string_option(&block->b_syn_isk);

    // free the stored states
    syn_थाक_free_all(block);
    invalidate_current_state();

    // Reset the counter for ":syn include"
    running_syn_inc_tag = 0;
}

/*
 * Get rid of ownsyntax for window "wp".
 */
    शून्य
reset_synblock(win_T *wp)
{
    यदि (wp->w_s != &wp->w_buffer->b_s)
    {
	syntax_clear(wp->w_s);
	vim_free(wp->w_s);
	wp->w_s = &wp->w_buffer->b_s;
    }
}

/*
 * Clear syncing info for one buffer.
 */
    स्थिर शून्य
syntax_sync_clear(शून्य)
{
    पूर्णाङ्क i;

    // free the syntax patterns
    लागि (i = curwin->w_s->b_syn_patterns.ga_len; --i >= 0; )
	यदि (SYN_वस्तुहरू(curwin->w_s)[i].sp_syncing)
	    syn_remove_pattern(curwin->w_s, i);

    curwin->w_s->b_syn_sync_flags = 0;
    curwin->w_s->b_syn_sync_minlines = 0;
    curwin->w_s->b_syn_sync_maxlines = 0;
    curwin->w_s->b_syn_sync_linebreaks = 0;

    vim_regfree(curwin->w_s->b_syn_linecont_prog);
    curwin->w_s->b_syn_linecont_prog = रिक्त;
    VIM_CLEAR(curwin->w_s->b_syn_linecont_pat);
    clear_string_option(&curwin->w_s->b_syn_isk);

    syn_थाक_free_all(curwin->w_s);	// Need to recompute all syntax.
}

/*
 * Remove one pattern from the buffer's pattern list.
 */
    स्थिर शून्य
syn_remove_pattern(
    synblock_T	*block,
    पूर्णाङ्क		idx)
{
    synpat_T	*spp;

    spp = &(SYN_वस्तुहरू(block)[idx]);
#ifdef FEAT_FOLDING
    यदि (spp->sp_flags & HL_FOLD)
	--block->b_syn_folditems;
#endif
    syn_clear_pattern(block, idx);
    mch_memmove(spp, spp + 1,
		   परिमाण(synpat_T) * (block->b_syn_patterns.ga_len - idx - 1));
    --block->b_syn_patterns.ga_len;
}

/*
 * Clear and free one syntax pattern.  When clearing all, must be called from
 * last to first!
 */
    स्थिर शून्य
syn_clear_pattern(synblock_T *block, पूर्णाङ्क i)
{
    vim_free(SYN_वस्तुहरू(block)[i].sp_pattern);
    vim_regfree(SYN_वस्तुहरू(block)[i].sp_prog);
    // Only free sp_cont_list and sp_next_list of first start pattern
    यदि (i == 0 || SYN_वस्तुहरू(block)[i - 1].sp_type != SPTYPE_START)
    {
	vim_free(SYN_वस्तुहरू(block)[i].sp_cont_list);
	vim_free(SYN_वस्तुहरू(block)[i].sp_next_list);
	vim_free(SYN_वस्तुहरू(block)[i].sp_syn.cont_in_list);
    }
}

/*
 * Clear and free one syntax cluster.
 */
    स्थिर शून्य
syn_clear_cluster(synblock_T *block, पूर्णाङ्क i)
{
    vim_free(SYN_CLSTR(block)[i].scl_name);
    vim_free(SYN_CLSTR(block)[i].scl_name_u);
    vim_free(SYN_CLSTR(block)[i].scl_list);
}

/*
 * Handle ":syntax clear" command.
 */
    स्थिर शून्य
syn_cmd_clear(exarg_T *eap, पूर्णाङ्क syncing)
{
    char_u	*arg = eap->arg;
    char_u	*arg_end;
    पूर्णाङ्क		id;

    eap->nextcmd = find_nextcmd(arg);
    यदि (eap->skip)
	फर्काऊ;

    /*
     * We have to disable this within ":syn include @group filename",
     * because otherwise @group would get deleted.
     * Only required for Vim 5.x syntax files, 6.0 ones don't contain ":syn
     * clear".
     */
    यदि (curwin->w_s->b_syn_topgrp != 0)
	फर्काऊ;

    यदि (ends_excmd2(eap->cmd, arg))
    {
	/*
	 * No argument: Clear all syntax items.
	 */
	यदि (syncing)
	    syntax_sync_clear();
	अन्यथा
	{
	    syntax_clear(curwin->w_s);
	    यदि (curwin->w_s == &curwin->w_buffer->b_s)
		do_unlet((char_u *)"b:current_syntax", TRUE);
	    do_unlet((char_u *)"w:current_syntax", TRUE);
	}
    }
    अन्यथा
    {
	/*
	 * Clear the group IDs that are in the argument.
	 */
	जबसम्म (!ends_excmd2(eap->cmd, arg))
	{
	    arg_end = skiptowhite(arg);
	    यदि (*arg == '@')
	    {
		id = syn_scl_namen2id(arg + 1, (पूर्णाङ्क)(arg_end - arg - 1));
		यदि (id == 0)
		{
		    semsg(_(e_no_such_syntax_cluster_str_1), arg);
		    रोक;
		}
		अन्यथा
		{
		    /*
		     * We can't physically delete a cluster without changing
		     * the IDs of other clusters, so we do the next best thing
		     * and make it empty.
		     */
		    लघु scl_id = id - SYNID_CLUSTER;

		    VIM_CLEAR(SYN_CLSTR(curwin->w_s)[scl_id].scl_list);
		}
	    }
	    अन्यथा
	    {
		id = syn_namen2id(arg, (पूर्णाङ्क)(arg_end - arg));
		यदि (id == 0)
		{
		    semsg(_(e_no_such_highlight_group_name_str), arg);
		    रोक;
		}
		अन्यथा
		    syn_clear_one(id, syncing);
	    }
	    arg = skipwhite(arg_end);
	}
    }
    redraw_curbuf_later(UPD_SOME_VALID);
    syn_थाक_free_all(curwin->w_s);		// Need to recompute all syntax.
}

/*
 * Clear one syntax group for the current buffer.
 */
    स्थिर शून्य
syn_clear_one(पूर्णाङ्क id, पूर्णाङ्क syncing)
{
    synpat_T	*spp;
    पूर्णाङ्क		idx;

    // Clear keywords only when not ":syn sync clear group-name"
    यदि (!syncing)
    {
	(शून्य)syn_clear_keyword(id, &curwin->w_s->b_keywtab);
	(शून्य)syn_clear_keyword(id, &curwin->w_s->b_keywtab_ic);
    }

    // clear the patterns for "id"
    लागि (idx = curwin->w_s->b_syn_patterns.ga_len; --idx >= 0; )
    {
	spp = &(SYN_वस्तुहरू(curwin->w_s)[idx]);
	यदि (spp->sp_syn.id != id || spp->sp_syncing != syncing)
	    जारी;
	syn_remove_pattern(curwin->w_s, idx);
    }
}

/*
 * Handle ":syntax on" command.
 */
    स्थिर शून्य
syn_cmd_on(exarg_T *eap, पूर्णाङ्क syncing UNUSED)
{
    syn_cmd_onoff(eap, "syntax");
}

/*
 * Handle ":syntax enable" command.
 */
    स्थिर शून्य
syn_cmd_enable(exarg_T *eap, पूर्णाङ्क syncing UNUSED)
{
    set_internal_string_var((char_u *)"g:syntax_cmd", (char_u *)"enable");
    syn_cmd_onoff(eap, "syntax");
    do_unlet((char_u *)"g:syntax_cmd", TRUE);
}

/*
 * Handle ":syntax reset" command.
 * It actually resets highlighting, not syntax.
 */
    स्थिर शून्य
syn_cmd_reset(exarg_T *eap, पूर्णाङ्क syncing UNUSED)
{
    set_nextcmd(eap, eap->arg);
    यदि (!eap->skip)
    {
	set_internal_string_var((char_u *)"g:syntax_cmd", (char_u *)"reset");
	do_cmdline_cmd((char_u *)"runtime! syntax/syncolor.vim");
	do_unlet((char_u *)"g:syntax_cmd", TRUE);
    }
}

/*
 * Handle ":syntax manual" command.
 */
    स्थिर शून्य
syn_cmd_manual(exarg_T *eap, पूर्णाङ्क syncing UNUSED)
{
    syn_cmd_onoff(eap, "manual");
}

/*
 * Handle ":syntax off" command.
 */
    स्थिर शून्य
syn_cmd_off(exarg_T *eap, पूर्णाङ्क syncing UNUSED)
{
    syn_cmd_onoff(eap, "nosyntax");
}

    स्थिर शून्य
syn_cmd_onoff(exarg_T *eap, वर्ण *name)
{
    char_u	buf[100];

    set_nextcmd(eap, eap->arg);
    यदि (!eap->skip)
    {
	STRCPY(buf, "so ");
	vim_snprintf((वर्ण *)buf + 3, परिमाण(buf) - 3, SYNTAX_FNAME, name);
	do_cmdline_cmd(buf);
    }
}

/*
 * Handle ":syntax [list]" command: list current syntax words.
 */
    स्थिर शून्य
syn_cmd_list(
    exarg_T	*eap,
    पूर्णाङ्क		syncing)	    // when TRUE: list syncing items
{
    char_u	*arg = eap->arg;
    पूर्णाङ्क		id;
    char_u	*arg_end;

    eap->nextcmd = find_nextcmd(arg);
    यदि (eap->skip)
	फर्काऊ;

    यदि (!syntax_present(curwin))
    {
	सन्देश(_(सन्देश_no_वस्तुहरू));
	फर्काऊ;
    }

    यदि (syncing)
    {
	यदि (curwin->w_s->b_syn_sync_flags & SF_CCOMMENT)
	{
	    सन्देश_puts(_("syncing on C-style comments"));
	    syn_पङ्क्तिहरू_सन्देश();
	    syn_match_सन्देश();
	}
	अन्यथा यदि (curwin->w_s->b_syn_sync_flags & SF_MATCH)
	{
	    सन्देश_puts_title(_("\n--- Syntax sync items ---"));
	    यदि (curwin->w_s->b_syn_sync_minlines > 0
		|| curwin->w_s->b_syn_sync_maxlines > 0
		|| curwin->w_s->b_syn_sync_linebreaks > 0)
	    {
		सन्देश_puts(_("\nsyncing on items"));
		syn_पङ्क्तिहरू_सन्देश();
		syn_match_सन्देश();
	    }
	    लागि (id = 1; id <= highlight_अङ्क_groups() && !got_int; ++id)
		syn_list_one(id, syncing, FALSE);
	}
	अन्यथा
	{
	    यदि (curwin->w_s->b_syn_sync_minlines == 0)
		सन्देश_puts(_("no syncing"));
	    अन्यथा
	    {
		यदि (curwin->w_s->b_syn_sync_minlines == MAXLNUM)
		    सन्देश_puts(_("syncing starts at the first line"));
		अन्यथा
		{
		    सन्देश_puts(_("syncing starts "));
		    सन्देश_outnum(curwin->w_s->b_syn_sync_minlines);
		    सन्देश_puts(_(" lines before top line"));
		}
		syn_match_सन्देश();
	    }
	}
	फर्काऊ;
    }

    सन्देश_puts_title(_("\n--- Syntax items ---"));
    यदि (ends_excmd2(eap->cmd, arg))
    {
	/*
	 * No argument: List all group IDs and all syntax clusters.
	 */
	लागि (id = 1; id <= highlight_अङ्क_groups() && !got_int; ++id)
	    syn_list_one(id, syncing, FALSE);
	लागि (id = 0; id < curwin->w_s->b_syn_clusters.ga_len && !got_int; ++id)
	    syn_list_cluster(id);
    }
    अन्यथा
    {
	/*
	 * List the group IDs and syntax clusters that are in the argument.
	 */
	जबसम्म (!ends_excmd2(eap->cmd, arg) && !got_int)
	{
	    arg_end = skiptowhite(arg);
	    यदि (*arg == '@')
	    {
		id = syn_scl_namen2id(arg + 1, (पूर्णाङ्क)(arg_end - arg - 1));
		यदि (id == 0)
		    semsg(_(e_no_such_syntax_cluster_str_2), arg);
		अन्यथा
		    syn_list_cluster(id - SYNID_CLUSTER);
	    }
	    अन्यथा
	    {
		id = syn_namen2id(arg, (पूर्णाङ्क)(arg_end - arg));
		यदि (id == 0)
		    semsg(_(e_no_such_highlight_group_name_str), arg);
		अन्यथा
		    syn_list_one(id, syncing, TRUE);
	    }
	    arg = skipwhite(arg_end);
	}
    }
    set_nextcmd(eap, arg);
}

    स्थिर शून्य
syn_पङ्क्तिहरू_सन्देश(शून्य)
{
    यदि (curwin->w_s->b_syn_sync_maxlines > 0
				      || curwin->w_s->b_syn_sync_minlines > 0)
    {
	सन्देश_puts("; ");
	यदि (curwin->w_s->b_syn_sync_minlines == MAXLNUM)
	    सन्देश_puts(_("from the first line"));
	अन्यथा
	{
	    यदि (curwin->w_s->b_syn_sync_minlines > 0)
	    {
		सन्देश_puts(_("minimal "));
		सन्देश_outnum(curwin->w_s->b_syn_sync_minlines);
		यदि (curwin->w_s->b_syn_sync_maxlines)
		    सन्देश_puts(", ");
	    }
	    यदि (curwin->w_s->b_syn_sync_maxlines > 0)
	    {
		सन्देश_puts(_("maximal "));
		सन्देश_outnum(curwin->w_s->b_syn_sync_maxlines);
	    }
	    सन्देश_puts(_(" lines before top line"));
	}
    }
}

    स्थिर शून्य
syn_match_सन्देश(शून्य)
{
    यदि (curwin->w_s->b_syn_sync_linebreaks > 0)
    {
	सन्देश_puts(_("; match "));
	सन्देश_outnum(curwin->w_s->b_syn_sync_linebreaks);
	सन्देश_puts(_(" line breaks"));
    }
}

स्थिर पूर्णाङ्क  last_matchgroup;

स्थिर शून्य syn_list_flags(keyvalue_T *nlist, पूर्णाङ्क nr_entries, पूर्णाङ्क flags, पूर्णाङ्क attr);

/*
 * List one syntax item, for ":syntax" or "syntax list syntax_name".
 */
    स्थिर शून्य
syn_list_one(
    पूर्णाङ्क		id,
    पूर्णाङ्क		syncing,	    // when TRUE: list syncing items
    पूर्णाङ्क		link_only)	    // when TRUE; list link-only too
{
    पूर्णाङ्क		attr;
    पूर्णाङ्क		idx;
    पूर्णाङ्क		did_header = FALSE;
    synpat_T	*spp;
    स्थिर keyvalue_T namelist1[] =
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
    स्थिर keyvalue_T namelist2[] =
		{
		    KEYVALUE_ENTRY(HL_SKIPWHITE, "skipwhite"),
		    KEYVALUE_ENTRY(HL_SKIPNL, "skipnl"),
		    KEYVALUE_ENTRY(HL_SKIPEMPTY, "skipempty")
		};

    attr = HL_ATTR(HLF_D);		// highlight like directories

    // list the keywords for "id"
    यदि (!syncing)
    {
	did_header = syn_list_keywords(id, &curwin->w_s->b_keywtab, FALSE, attr);
	did_header = syn_list_keywords(id, &curwin->w_s->b_keywtab_ic,
							    did_header, attr);
    }

    // list the patterns for "id"
    लागि (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len && !got_int; ++idx)
    {
	spp = &(SYN_वस्तुहरू(curwin->w_s)[idx]);
	यदि (spp->sp_syn.id != id || spp->sp_syncing != syncing)
	    जारी;

	(शून्य)syn_list_header(did_header, 999, id);
	did_header = TRUE;
	last_matchgroup = 0;
	यदि (spp->sp_type == SPTYPE_MATCH)
	{
	    put_pattern("match", ' ', spp, attr);
	    सन्देश_putchar(' ');
	}
	अन्यथा यदि (spp->sp_type == SPTYPE_START)
	{
	    जबसम्म (SYN_वस्तुहरू(curwin->w_s)[idx].sp_type == SPTYPE_START)
		put_pattern("start", '=', &SYN_वस्तुहरू(curwin->w_s)[idx++], attr);
	    यदि (SYN_वस्तुहरू(curwin->w_s)[idx].sp_type == SPTYPE_SKIP)
		put_pattern("skip", '=', &SYN_वस्तुहरू(curwin->w_s)[idx++], attr);
	    जबसम्म (idx < curwin->w_s->b_syn_patterns.ga_len
			  && SYN_वस्तुहरू(curwin->w_s)[idx].sp_type == SPTYPE_END)
		put_pattern("end", '=', &SYN_वस्तुहरू(curwin->w_s)[idx++], attr);
	    --idx;
	    सन्देश_putchar(' ');
	}
	syn_list_flags(namelist1, (पूर्णाङ्क)ARRAY_LENGTH(namelist1), spp->sp_flags, attr);

	यदि (spp->sp_cont_list != रिक्त)
	    put_id_list((char_u *)"contains", spp->sp_cont_list, attr);

	यदि (spp->sp_syn.cont_in_list != रिक्त)
	    put_id_list((char_u *)"containedin",
					      spp->sp_syn.cont_in_list, attr);

	यदि (spp->sp_next_list != रिक्त)
	{
	    put_id_list((char_u *)"nextgroup", spp->sp_next_list, attr);
	    syn_list_flags(namelist2, (पूर्णाङ्क)ARRAY_LENGTH(namelist2), spp->sp_flags, attr);
	}
	यदि (spp->sp_flags & (HL_SYNC_HERE|HL_SYNC_THERE))
	{
	    यदि (spp->sp_flags & HL_SYNC_HERE)
		सन्देश_puts_attr("grouphere", attr);
	    अन्यथा
		सन्देश_puts_attr("groupthere", attr);
	    सन्देश_putchar(' ');
	    यदि (spp->sp_sync_idx >= 0)
		सन्देश_outtrans(highlight_group_name(SYN_वस्तुहरू(curwin->w_s)
				   [spp->sp_sync_idx].sp_syn.id - 1));
	    अन्यथा
		सन्देश_puts("NONE");
	    सन्देश_putchar(' ');
	}
    }

    // list the link, if there is one
    यदि (highlight_link_id(id - 1) && (did_header || link_only) && !got_int)
    {
	(शून्य)syn_list_header(did_header, 999, id);
	सन्देश_puts_attr("links to", attr);
	सन्देश_putchar(' ');
	सन्देश_outtrans(highlight_group_name(highlight_link_id(id - 1) - 1));
    }
}

    स्थिर शून्य
syn_list_flags(keyvalue_T *nlist, पूर्णाङ्क nr_entries, पूर्णाङ्क flags, पूर्णाङ्क attr)
{
    पूर्णाङ्क		i;

    लागि (i = 0; i < nr_entries; ++i)
	यदि (flags & nlist[i].साँचो)
	{
	    सन्देश_puts_attr((वर्ण *)nlist[i].मान.string, attr);
	    सन्देश_putchar(' ');
	}
}

/*
 * List one syntax cluster, for ":syntax" or "syntax list syntax_name".
 */
    स्थिर शून्य
syn_list_cluster(पूर्णाङ्क id)
{
    पूर्णाङ्क	    endcol = 15;

    // slight hack:  roughly duplicate the guts of syn_list_header()
    सन्देश_putchar('\n');
    सन्देश_outtrans(SYN_CLSTR(curwin->w_s)[id].scl_name);

    यदि (सन्देश_col >= endcol)	// output at least one space
	endcol = सन्देश_col + 1;
    यदि (cmdline_width <= endcol)	// avoid hang for tiny window
	endcol = cmdline_width - 1;

    सन्देश_advance(endcol);
    यदि (SYN_CLSTR(curwin->w_s)[id].scl_list != रिक्त)
    {
	put_id_list((char_u *)"cluster", SYN_CLSTR(curwin->w_s)[id].scl_list,
		    HL_ATTR(HLF_D));
    }
    अन्यथा
    {
	सन्देश_puts_attr("cluster", HL_ATTR(HLF_D));
	सन्देश_puts("=NONE");
    }
}

    स्थिर शून्य
put_id_list(char_u *name, लघु *list, पूर्णाङ्क attr)
{
    लघु		*p;

    सन्देश_puts_attr((वर्ण *)name, attr);
    सन्देश_putchar('=');
    लागि (p = list; *p; ++p)
    {
	यदि (*p >= SYNID_ALLBUT && *p < SYNID_शीर्ष)
	{
	    यदि (p[1])
		सन्देश_puts("ALLBUT");
	    अन्यथा
		सन्देश_puts("ALL");
	}
	अन्यथा यदि (*p >= SYNID_शीर्ष && *p < SYNID_CONTAINED)
	{
	    सन्देश_puts("TOP");
	}
	अन्यथा यदि (*p >= SYNID_CONTAINED && *p < SYNID_CLUSTER)
	{
	    सन्देश_puts("CONTAINED");
	}
	अन्यथा यदि (*p >= SYNID_CLUSTER)
	{
	    लघु scl_id = *p - SYNID_CLUSTER;

	    सन्देश_putchar('@');
	    सन्देश_outtrans(SYN_CLSTR(curwin->w_s)[scl_id].scl_name);
	}
	अन्यथा
	    सन्देश_outtrans(highlight_group_name(*p - 1));
	यदि (p[1])
	    सन्देश_putchar(',');
    }
    सन्देश_putchar(' ');
}

    स्थिर शून्य
put_pattern(
    वर्ण	*s,
    पूर्णाङ्क		c,
    synpat_T	*spp,
    पूर्णाङ्क		attr)
{
    दीर्घ	n;
    पूर्णाङ्क		mask;
    पूर्णाङ्क		first;
    स्थिर वर्ण	*sepchars = "/+=-#@\"|'^&";
    पूर्णाङ्क		i;

    // May have to write "matchgroup=group"
    यदि (last_matchgroup != spp->sp_syn_match_id)
    {
	last_matchgroup = spp->sp_syn_match_id;
	सन्देश_puts_attr("matchgroup", attr);
	सन्देश_putchar('=');
	यदि (last_matchgroup == 0)
	    सन्देश_outtrans((char_u *)"NONE");
	अन्यथा
	    सन्देश_outtrans(highlight_group_name(last_matchgroup - 1));
	सन्देश_putchar(' ');
    }

    // output the name of the pattern and an '=' or ' '
    सन्देश_puts_attr(s, attr);
    सन्देश_putchar(c);

    // output the pattern, in between a char that is not in the pattern
    लागि (i = 0; vim_strchr(spp->sp_pattern, sepchars[i]) != रिक्त; )
	यदि (sepchars[++i] == NUL)
	{
	    i = 0;	// no good char found, just use the first one
	    रोक;
	}
    सन्देश_putchar(sepchars[i]);
    सन्देश_outtrans(spp->sp_pattern);
    सन्देश_putchar(sepchars[i]);

    // output any pattern options
    first = TRUE;
    लागि (i = 0; i < SPO_गन्ती; ++i)
    {
	mask = (1 << i);
	यदि (spp->sp_off_flags & (mask + (mask << SPO_गन्ती)))
	{
	    यदि (!first)
		सन्देश_putchar(',');	// separate with commas
	    सन्देश_puts(spo_name_tab[i]);
	    n = spp->sp_offsets[i];
	    यदि (i != SPO_LC_OFF)
	    {
		यदि (spp->sp_off_flags & mask)
		    सन्देश_putchar('s');
		अन्यथा
		    सन्देश_putchar('e');
		यदि (n > 0)
		    सन्देश_putchar('+');
	    }
	    यदि (n || i == SPO_LC_OFF)
		सन्देश_outnum(n);
	    first = FALSE;
	}
    }
    सन्देश_putchar(' ');
}

/*
 * List or clear the keywords for one syntax group.
 * Return TRUE if the header has been printed.
 */
    स्थिर पूर्णाङ्क
syn_list_keywords(
    पूर्णाङ्क		id,
    hashtab_T	*ht,
    पूर्णाङ्क		did_header,		// header has already been printed
    पूर्णाङ्क		attr)
{
    पूर्णाङ्क		outlen;
    hashitem_T	*hi;
    keyentry_T	*kp;
    पूर्णाङ्क		todo;
    पूर्णाङ्क		prev_contained = 0;
    लघु	*prev_next_list = रिक्त;
    लघु	*prev_cont_in_list = रिक्त;
    पूर्णाङ्क		prev_skipnl = 0;
    पूर्णाङ्क		prev_skipwhite = 0;
    पूर्णाङ्क		prev_skipempty = 0;

    /*
     * Unfortunately, this list of keywords is not sorted on alphabet but on
     * hash value...
     */
    todo = (पूर्णाङ्क)ht->ht_used;
    लागि (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	यदि (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    लागि (kp = HI2KE(hi); kp != रिक्त && !got_int; kp = kp->ke_next)
	    {
		यदि (kp->k_syn.id == id)
		{
		    यदि (prev_contained != (kp->flags & HL_CONTAINED)
			    || prev_skipnl != (kp->flags & HL_SKIPNL)
			    || prev_skipwhite != (kp->flags & HL_SKIPWHITE)
			    || prev_skipempty != (kp->flags & HL_SKIPEMPTY)
			    || prev_cont_in_list != kp->k_syn.cont_in_list
			    || prev_next_list != kp->next_list)
			outlen = 9999;
		    अन्यथा
			outlen = (पूर्णाङ्क)STRLEN(kp->keyword);
		    // output "contained" and "nextgroup" on each line
		    यदि (syn_list_header(did_header, outlen, id))
		    {
			prev_contained = 0;
			prev_next_list = रिक्त;
			prev_cont_in_list = रिक्त;
			prev_skipnl = 0;
			prev_skipwhite = 0;
			prev_skipempty = 0;
		    }
		    did_header = TRUE;
		    यदि (prev_contained != (kp->flags & HL_CONTAINED))
		    {
			सन्देश_puts_attr("contained", attr);
			सन्देश_putchar(' ');
			prev_contained = (kp->flags & HL_CONTAINED);
		    }
		    यदि (kp->k_syn.cont_in_list != prev_cont_in_list)
		    {
			put_id_list((char_u *)"containedin",
						kp->k_syn.cont_in_list, attr);
			सन्देश_putchar(' ');
			prev_cont_in_list = kp->k_syn.cont_in_list;
		    }
		    यदि (kp->next_list != prev_next_list)
		    {
			put_id_list((char_u *)"nextgroup", kp->next_list, attr);
			सन्देश_putchar(' ');
			prev_next_list = kp->next_list;
			यदि (kp->flags & HL_SKIPNL)
			{
			    सन्देश_puts_attr("skipnl", attr);
			    सन्देश_putchar(' ');
			    prev_skipnl = (kp->flags & HL_SKIPNL);
			}
			यदि (kp->flags & HL_SKIPWHITE)
			{
			    सन्देश_puts_attr("skipwhite", attr);
			    सन्देश_putchar(' ');
			    prev_skipwhite = (kp->flags & HL_SKIPWHITE);
			}
			यदि (kp->flags & HL_SKIPEMPTY)
			{
			    सन्देश_puts_attr("skipempty", attr);
			    सन्देश_putchar(' ');
			    prev_skipempty = (kp->flags & HL_SKIPEMPTY);
			}
		    }
		    सन्देश_outtrans(kp->keyword);
		}
	    }
	}
    }

    फर्काऊ did_header;
}

    स्थिर शून्य
syn_clear_keyword(पूर्णाङ्क id, hashtab_T *ht)
{
    hashitem_T	*hi;
    keyentry_T	*kp;
    keyentry_T	*kp_prev;
    keyentry_T	*kp_next;
    पूर्णाङ्क		todo;

    hash_lock(ht);
    todo = (पूर्णाङ्क)ht->ht_used;
    FOR_ALL_HASHTAB_वस्तुहरू(ht, hi, todo)
    {
	यदि (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    kp_prev = रिक्त;
	    लागि (kp = HI2KE(hi); kp != रिक्त; )
	    {
		यदि (kp->k_syn.id == id)
		{
		    kp_next = kp->ke_next;
		    यदि (kp_prev == रिक्त)
		    {
			यदि (kp_next == रिक्त)
			    hash_remove(ht, hi, "syntax clear keyword");
			अन्यथा
			    hi->hi_साँचो = KE2HIKEY(kp_next);
		    }
		    अन्यथा
			kp_prev->ke_next = kp_next;
		    vim_free(kp->next_list);
		    vim_free(kp->k_syn.cont_in_list);
		    vim_free(kp);
		    kp = kp_next;
		}
		अन्यथा
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
    स्थिर शून्य
clear_keywtab(hashtab_T *ht)
{
    hashitem_T	*hi;
    पूर्णाङ्क		todo;
    keyentry_T	*kp;
    keyentry_T	*kp_next;

    todo = (पूर्णाङ्क)ht->ht_used;
    FOR_ALL_HASHTAB_वस्तुहरू(ht, hi, todo)
    {
	यदि (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    लागि (kp = HI2KE(hi); kp != रिक्त; kp = kp_next)
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
    स्थिर शून्य
add_keyword(
    char_u	*name,	    // name of keyword
    माप_प्रकार	namelen,    // length of keyword (excluding the NUL)
    पूर्णाङ्क		id,	    // group ID for this keyword
    पूर्णाङ्क		flags,	    // flags for this keyword
    लघु	*cont_in_list, // containedin for this keyword
    लघु	*next_list, // nextgroup for this keyword
    पूर्णाङ्क		conceal_char)
{
    keyentry_T	*kp;
    hashtab_T	*ht;
    hashitem_T	*hi;
    char_u	*name_ic;
    माप_प्रकार	name_iclen;
    long_u	hash;
    char_u	name_folded[MAXKEYWLEN + 1];

    यदि (curwin->w_s->b_syn_ic)
    {
	name_ic = str_foldcase(name, (पूर्णाङ्क)namelen,
						 name_folded, MAXKEYWLEN + 1);
	name_iclen = STRLEN(name_ic);
    }
    अन्यथा
    {
	name_ic = name;
	name_iclen = namelen;
    }
    kp = alloc(दुरत्व(keyentry_T, keyword) + name_iclen + 1);
    यदि (kp == रिक्त)
	फर्काऊ;
    STRCPY(kp->keyword, name_ic);
    kp->k_syn.id = id;
    kp->k_syn.inc_tag = current_syn_inc_tag;
    kp->flags = flags;
    kp->k_char = conceal_char;
    kp->k_syn.cont_in_list = copy_id_list(cont_in_list);
    यदि (cont_in_list != रिक्त)
	curwin->w_s->b_syn_containedin = TRUE;
    kp->next_list = copy_id_list(next_list);

    यदि (curwin->w_s->b_syn_ic)
	ht = &curwin->w_s->b_keywtab_ic;
    अन्यथा
	ht = &curwin->w_s->b_keywtab;

    hash = hash_hash(kp->keyword);
    hi = hash_lookup(ht, kp->keyword, hash);
    यदि (HASHITEM_EMPTY(hi))
    {
	// new keyword, add to hashtable
	kp->ke_next = रिक्त;
	hash_add_item(ht, hi, kp->keyword, hash);
    }
    अन्यथा
    {
	// keyword already exists, prepend to list
	kp->ke_next = HI2KE(hi);
	hi->hi_साँचो = KE2HIKEY(kp);
    }
}

/*
 * Get the start and end of the group name argument.
 * Return a pointer to the first argument.
 * Return NULL if the end of the command was found instead of further args.
 */
    स्थिर char_u *
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
    यदि (ends_excmd(*arg) || *rest == NUL)
	फर्काऊ रिक्त;
    फर्काऊ rest;
}

/*
 * Check for syntax command option arguments.
 * This can be called at any place in the list of arguments, and just picks
 * out the arguments that are known.  Can be called several times in a row to
 * collect all options in between other arguments.
 * Return a pointer to the next argument (which isn't an option).
 * Return NULL for any error;
 */
    स्थिर char_u *
get_syn_options(
    char_u	    *start,		// next argument to be checked
    syn_opt_arg_T   *opt,		// various things
    पूर्णाङ्क		    *conceal_char UNUSED,
    पूर्णाङ्क		    skip)		// TRUE if skipping over command
{
    char_u	*arg = start;
    char_u	*gname_start, *gname;
    पूर्णाङ्क		syn_id;
    पूर्णाङ्क		len;
    वर्ण	*p;
    पूर्णाङ्क		i;
    पूर्णाङ्क		fidx;
    स्थिर संरचना flag
    {
	वर्ण	*name;
	पूर्णाङ्क	argtype;
	पूर्णाङ्क	flags;
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
    स्थिर वर्ण *first_letters = "cCoOkKeEtTsSgGdDfFnN";

    यदि (arg == रिक्त)		// already detected error
	फर्काऊ रिक्त;

#ifdef FEAT_CONCEAL
    यदि (curwin->w_s->b_syn_conceal)
	opt->flags |= HL_CONCEAL;
#endif

    लागि (;;)
    {
	/*
	 * This is used very often when a large number of keywords is defined.
	 * Need to skip quickly when no option name is found.
	 * Also avoid tolower(), it's slow.
	 */
	यदि (म_अक्षर(first_letters, *arg) == रिक्त)
	    रोक;

	लागि (fidx = ARRAY_LENGTH(flagtab); --fidx >= 0; )
	{
	    p = flagtab[fidx].name;
	    लागि (i = 0, len = 0; p[i] != NUL; i += 2, ++len)
		यदि (arg[len] != p[i] && arg[len] != p[i + 1])
		    रोक;
	    यदि (p[i] == NUL && (VIM_ISWHITE(arg[len])
				    || (flagtab[fidx].argtype > 0
					 ? arg[len] == '='
					 : ends_excmd2(start, arg + len))))
	    {
		यदि (opt->keyword
			&& (flagtab[fidx].flags == HL_DISPLAY
			    || flagtab[fidx].flags == HL_FOLD
			    || flagtab[fidx].flags == HL_EXTEND))
		    // treat "display", "fold" and "extend" as a keyword
		    fidx = -1;
		रोक;
	    }
	}
	यदि (fidx < 0)	    // no match found
	    रोक;

	यदि (flagtab[fidx].argtype == 1)
	{
	    यदि (!opt->has_cont_list)
	    {
		emsg(_(e_contains_argument_not_accepted_here));
		फर्काऊ रिक्त;
	    }
	    यदि (get_id_list(&arg, 8, &opt->cont_list, skip) == FAIL)
		फर्काऊ रिक्त;
	}
	अन्यथा यदि (flagtab[fidx].argtype == 2)
	{
	    यदि (get_id_list(&arg, 11, &opt->cont_in_list, skip) == FAIL)
		फर्काऊ रिक्त;
	}
	अन्यथा यदि (flagtab[fidx].argtype == 3)
	{
	    यदि (get_id_list(&arg, 9, &opt->next_list, skip) == FAIL)
		फर्काऊ रिक्त;
	}
	अन्यथा यदि (flagtab[fidx].argtype == 11 && arg[5] == '=')
	{
	    // cchar=?
	    यदि (has_mbyte)
	    {
#ifdef FEAT_CONCEAL
		*conceal_char = mb_ptr2char(arg + 6);
#endif
		arg += mb_ptr2len(arg + 6) - 1;
	    }
	    अन्यथा
	    {
#ifdef FEAT_CONCEAL
		*conceal_char = arg[6];
#अन्यथा
		;
#endif
	    }
#ifdef FEAT_CONCEAL
	    यदि (!vim_isprintc_strict(*conceal_char))
	    {
		emsg(_(e_invalid_cchar_मान));
		फर्काऊ रिक्त;
	    }
#endif
	    arg = skipwhite(arg + 7);
	}
	अन्यथा
	{
	    opt->flags |= flagtab[fidx].flags;
	    arg = skipwhite(arg + len);

	    यदि (flagtab[fidx].flags == HL_SYNC_HERE
		    || flagtab[fidx].flags == HL_SYNC_THERE)
	    {
		यदि (opt->sync_idx == रिक्त)
		{
		    emsg(_(e_groupthere_not_accepted_here));
		    फर्काऊ रिक्त;
		}
		gname_start = arg;
		arg = skiptowhite(arg);
		यदि (gname_start == arg)
		    फर्काऊ रिक्त;
		gname = vim_strnsave(gname_start, arg - gname_start);
		यदि (gname == रिक्त)
		    फर्काऊ रिक्त;
		यदि (STRCMP(gname, "NONE") == 0)
		    *opt->sync_idx = NONE_IDX;
		अन्यथा
		{
		    syn_id = syn_name2id(gname);
		    लागि (i = curwin->w_s->b_syn_patterns.ga_len; --i >= 0; )
			यदि (SYN_वस्तुहरू(curwin->w_s)[i].sp_syn.id == syn_id
			      && SYN_वस्तुहरू(curwin->w_s)[i].sp_type
							       == SPTYPE_START)
			{
			    *opt->sync_idx = i;
			    रोक;
			}
		    यदि (i < 0)
		    {
			semsg(_(e_didnt_find_region_item_for_str), gname);
			vim_free(gname);
			फर्काऊ रिक्त;
		    }
		}

		vim_free(gname);
		arg = skipwhite(arg);
	    }
#ifdef FEAT_FOLDING
	    अन्यथा यदि (flagtab[fidx].flags == HL_FOLD
						&& foldmethodIsSyntax(curwin))
		// Need to update folds later.
		foldUpdateAll(curwin);
#endif
	}
    }

    फर्काऊ arg;
}

/*
 * Adjustments to syntax item when declared in a ":syn include"'d file.
 * Set the contained flag, and if the item is not already contained, add it
 * to the specified top-level group, if any.
 */
    स्थिर शून्य
syn_incl_toplevel(पूर्णाङ्क id, पूर्णाङ्क *flagsp)
{
    यदि ((*flagsp & HL_CONTAINED) || curwin->w_s->b_syn_topgrp == 0)
	फर्काऊ;
    *flagsp |= HL_CONTAINED | HL_INCLUDED_TOPLEVEL;
    यदि (curwin->w_s->b_syn_topgrp >= SYNID_CLUSTER)
    {
	// We have to alloc this, because syn_combine_list() will free it.
	लघु	    *grp_list = ALLOC_MULT(लघु, 2);
	पूर्णाङ्क	    tlg_id = curwin->w_s->b_syn_topgrp - SYNID_CLUSTER;

	यदि (grp_list != रिक्त)
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
    स्थिर शून्य
syn_cmd_include(exarg_T *eap, पूर्णाङ्क syncing UNUSED)
{
    char_u	*arg = eap->arg;
    पूर्णाङ्क		sgl_id = 1;
    char_u	*group_name_end;
    char_u	*rest;
    वर्ण	*errormsg = रिक्त;
    पूर्णाङ्क		prev_toplvl_grp;
    पूर्णाङ्क		prev_syn_inc_tag;
    पूर्णाङ्क		source = FALSE;

    eap->nextcmd = find_nextcmd(arg);
    यदि (eap->skip)
	फर्काऊ;

    यदि (arg[0] == '@')
    {
	++arg;
	rest = get_group_name(arg, &group_name_end);
	यदि (rest == रिक्त)
	{
	    emsg(_(e_filename_required));
	    फर्काऊ;
	}
	sgl_id = syn_check_cluster(arg, (पूर्णाङ्क)(group_name_end - arg));
	यदि (sgl_id == 0)
	    फर्काऊ;
	// separate_nextcmd() and expand_filename() depend on this
	eap->arg = rest;
    }

    /*
     * Everything that's left, up to the next command, should be the
     * filename to include.
     */
    eap->argt |= (EX_XFILE | EX_NOSPC);
    separate_nextcmd(eap, FALSE);
    यदि (*eap->arg == '<' || *eap->arg == '$' || mch_isFullName(eap->arg))
    {
	// For an absolute path, "$VIM/..." or "<sfile>.." we ":source" the
	// file.  Need to expand the file name first.  In other cases
	// ":runtime!" is used.
	source = TRUE;
	यदि (expand_filename(eap, syn_cmdlinep, &errormsg) == FAIL)
	{
	    यदि (errormsg != रिक्त)
		emsg(errormsg);
	    फर्काऊ;
	}
    }

    /*
     * Save and restore the existing top-level grouplist id and ":syn
     * include" tag around the actual inclusion.
     */
    यदि (running_syn_inc_tag >= MAX_SYN_INC_TAG)
    {
	emsg(_(e_too_many_syntax_includes));
	फर्काऊ;
    }
    prev_syn_inc_tag = current_syn_inc_tag;
    current_syn_inc_tag = ++running_syn_inc_tag;
    prev_toplvl_grp = curwin->w_s->b_syn_topgrp;
    curwin->w_s->b_syn_topgrp = sgl_id;
    यदि (source ? do_source(eap->arg, FALSE, DOSO_NONE, रिक्त) == FAIL
				: source_runtime(eap->arg, DIP_ALL) == FAIL)
	semsg(_(e_cant_open_file_str), eap->arg);
    curwin->w_s->b_syn_topgrp = prev_toplvl_grp;
    current_syn_inc_tag = prev_syn_inc_tag;
}

/*
 * Handle ":syntax keyword {group-name} [{option}] keyword .." command.
 */
    स्थिर शून्य
syn_cmd_keyword(exarg_T *eap, पूर्णाङ्क syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*group_name_end;
    पूर्णाङ्क		syn_id;
    char_u	*rest;
    char_u	*keyword_copy = रिक्त;
    char_u	*p;
    char_u	*kw;
    syn_opt_arg_T syn_opt_arg;
    पूर्णाङ्क		cnt;
    पूर्णाङ्क		conceal_char = NUL;

    rest = get_group_name(arg, &group_name_end);

    यदि (rest != रिक्त)
    {
	यदि (eap->skip)
	    syn_id = -1;
	अन्यथा
	    syn_id = syn_check_group(arg, (पूर्णाङ्क)(group_name_end - arg));
	यदि (syn_id != 0)
	    // allocate a buffer, for removing backslashes in the keyword
	    keyword_copy = alloc(STRLEN(rest) + 1);
	यदि (keyword_copy != रिक्त)
	{
	    syn_opt_arg.flags = 0;
	    syn_opt_arg.keyword = TRUE;
	    syn_opt_arg.sync_idx = रिक्त;
	    syn_opt_arg.has_cont_list = FALSE;
	    syn_opt_arg.cont_in_list = रिक्त;
	    syn_opt_arg.next_list = रिक्त;

	    /*
	     * The options given apply to ALL keywords, so all options must be
	     * found before keywords can be created.
	     * 1: collect the options and copy the keywords to keyword_copy.
	     */
	    cnt = 0;
	    p = keyword_copy;
	    लागि ( ; rest != रिक्त && !ends_excmd2(eap->arg, rest);
							rest = skipwhite(rest))
	    {
		rest = get_syn_options(rest, &syn_opt_arg, &conceal_char,
								    eap->skip);
		यदि (rest == रिक्त || ends_excmd2(eap->arg, rest))
		    रोक;
		// Copy the keyword, removing backslashes, and add a NUL.
		जबसम्म (*rest != NUL && !VIM_ISWHITE(*rest))
		{
		    यदि (*rest == '\\' && rest[1] != NUL)
			++rest;
		    *p++ = *rest++;
		}
		*p++ = NUL;
		++cnt;
	    }

	    यदि (!eap->skip)
	    {
		माप_प्रकार	kwlen = 0;

		// Adjust flags for use of ":syn include".
		syn_incl_toplevel(syn_id, &syn_opt_arg.flags);

		/*
		 * 2: Add an entry for each keyword.
		 */
		लागि (kw = keyword_copy; --cnt >= 0; kw += kwlen + 1)
		{
		    लागि (p = vim_strchr(kw, '['); ; )
		    {
			यदि (p == रिक्त)
			    kwlen = STRLEN(kw);
			अन्यथा
			{
			    *p = NUL;
			    kwlen = (माप_प्रकार)(p - kw);
			}
			add_keyword(kw, kwlen, syn_id, syn_opt_arg.flags,
				syn_opt_arg.cont_in_list,
					 syn_opt_arg.next_list, conceal_char);
			यदि (p == रिक्त)
			    रोक;
			यदि (p[1] == NUL)
			{
			    semsg(_(e_error_missing_rsb_str), kw);
			    गच्छ error;
			}
			यदि (p[1] == ']')
			{
			    यदि (p[2] != NUL)
			    {
				semsg(_(e_trailing_char_after_rsb_str_str),
								    kw, &p[2]);
				गच्छ error;
			    }
			    kw = p + 1;		// skip over the "]"
			    kwlen = 1;
			    रोक;
			}
			यदि (has_mbyte)
			{
			    पूर्णाङ्क l = (*mb_ptr2len)(p + 1);

			    mch_memmove(p, p + 1, l);
			    p += l;
			}
			अन्यथा
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

    यदि (rest != रिक्त)
	set_nextcmd(eap, rest);
    अन्यथा
	semsg(_(e_invalid_argument_str), arg);

    redraw_curbuf_later(UPD_SOME_VALID);
    syn_थाक_free_all(curwin->w_s);		// Need to recompute all syntax.
}

/*
 * Handle ":syntax match {name} [{options}] {pattern} [{options}]".
 *
 * Also ":syntax sync match {name} [[grouphere | groupthere] {group-name}] .."
 */
    स्थिर शून्य
syn_cmd_match(
    exarg_T	*eap,
    पूर्णाङ्क		syncing)	    // TRUE for ":syntax sync match .. "
{
    char_u	*arg = eap->arg;
    char_u	*group_name_end;
    char_u	*rest;
    synpat_T	item;		// the item found in the line
    पूर्णाङ्क		syn_id;
    पूर्णाङ्क		idx;
    syn_opt_arg_T syn_opt_arg;
    पूर्णाङ्क		sync_idx = 0;
    पूर्णाङ्क		conceal_char = NUL;
    पूर्णाङ्क		orig_called_emsg = called_emsg;

    // Isolate the group name, check for validity
    rest = get_group_name(arg, &group_name_end);

    // Get options before the pattern
    syn_opt_arg.flags = 0;
    syn_opt_arg.keyword = FALSE;
    syn_opt_arg.sync_idx = syncing ? &sync_idx : रिक्त;
    syn_opt_arg.has_cont_list = TRUE;
    syn_opt_arg.cont_list = रिक्त;
    syn_opt_arg.cont_in_list = रिक्त;
    syn_opt_arg.next_list = रिक्त;
    rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);

    // get the pattern.
    init_syn_patterns();
    CLEAR_FIELD(item);
    rest = get_syn_pattern(rest, &item);
    यदि (vim_regcomp_had_eol() && !(syn_opt_arg.flags & HL_EXCLUDENL))
	syn_opt_arg.flags |= HL_HAS_EOL;

    // Get options after the pattern
    rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);

    यदि (rest != रिक्त)		// all arguments are valid
    {
	/*
	 * Check for trailing command and illegal trailing arguments.
	 */
	set_nextcmd(eap, rest);
	यदि (!ends_excmd2(eap->cmd, rest) || eap->skip)
	    rest = रिक्त;
	अन्यथा यदि (ga_grow(&curwin->w_s->b_syn_patterns, 1) == OK
		&& (syn_id = syn_check_group(arg,
					   (पूर्णाङ्क)(group_name_end - arg))) != 0)
	{
	    syn_incl_toplevel(syn_id, &syn_opt_arg.flags);
	    /*
	     * Store the pattern in the syn_items list
	     */
	    idx = curwin->w_s->b_syn_patterns.ga_len;
	    SYN_वस्तुहरू(curwin->w_s)[idx] = item;
	    SYN_वस्तुहरू(curwin->w_s)[idx].sp_syncing = syncing;
	    SYN_वस्तुहरू(curwin->w_s)[idx].sp_type = SPTYPE_MATCH;
	    SYN_वस्तुहरू(curwin->w_s)[idx].sp_syn.id = syn_id;
	    SYN_वस्तुहरू(curwin->w_s)[idx].sp_syn.inc_tag = current_syn_inc_tag;
	    SYN_वस्तुहरू(curwin->w_s)[idx].sp_flags = syn_opt_arg.flags;
	    SYN_वस्तुहरू(curwin->w_s)[idx].sp_sync_idx = sync_idx;
	    SYN_वस्तुहरू(curwin->w_s)[idx].sp_cont_list = syn_opt_arg.cont_list;
	    SYN_वस्तुहरू(curwin->w_s)[idx].sp_syn.cont_in_list =
						     syn_opt_arg.cont_in_list;
#ifdef FEAT_CONCEAL
	    SYN_वस्तुहरू(curwin->w_s)[idx].sp_cchar = conceal_char;
#endif
	    यदि (syn_opt_arg.cont_in_list != रिक्त)
		curwin->w_s->b_syn_containedin = TRUE;
	    SYN_वस्तुहरू(curwin->w_s)[idx].sp_next_list = syn_opt_arg.next_list;
	    ++curwin->w_s->b_syn_patterns.ga_len;

	    // remember that we found a match for syncing on
	    यदि (syn_opt_arg.flags & (HL_SYNC_HERE|HL_SYNC_THERE))
		curwin->w_s->b_syn_sync_flags |= SF_MATCH;
#ifdef FEAT_FOLDING
	    यदि (syn_opt_arg.flags & HL_FOLD)
		++curwin->w_s->b_syn_folditems;
#endif

	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_थाक_free_all(curwin->w_s);	// Need to recompute all syntax.
	    फर्काऊ;	// don't free the progs and patterns now
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

    यदि (rest == रिक्त && called_emsg == orig_called_emsg)
	semsg(_(e_invalid_argument_str), arg);
}

/*
 * Handle ":syntax region {group-name} [matchgroup={group-name}]
 *		start {start} .. [skip {skip}] end {end} .. [{options}]".
 */
    स्थिर शून्य
syn_cmd_region(
    exarg_T	*eap,
    पूर्णाङ्क		syncing)	    // TRUE for ":syntax sync region .."
{
    char_u		*arg = eap->arg;
    char_u		*group_name_end;
    char_u		*rest;			// next arg, NULL on error
    char_u		*साँचो_end;
    char_u		*साँचो = रिक्त;
    char_u		*p;
    पूर्णाङ्क			item;
#define ITEM_START	    0
#define ITEM_SKIP	    1
#define ITEM_END	    2
#define ITEM_MATCHGROUP	    3
    संरचना pat_ptr
    {
	synpat_T	*pp_synp;		// pointer to syn_pattern
	पूर्णाङ्क		pp_matchgroup_id;	// matchgroup ID
	संरचना pat_ptr	*pp_next;		// pointer to next pat_ptr
    }			*(pat_ptrs[3]);
					// patterns found in the line
    संरचना pat_ptr	*ppp;
    संरचना pat_ptr	*ppp_next;
    पूर्णाङ्क			pat_गन्ती = 0;		// nr of syn_patterns found
    पूर्णाङ्क			syn_id;
    पूर्णाङ्क			matchgroup_id = 0;
    पूर्णाङ्क			not_enough = FALSE;	// not enough arguments
    पूर्णाङ्क			illegal = FALSE;	// illegal arguments
    पूर्णाङ्क			success = FALSE;
    पूर्णाङ्क			idx;
    syn_opt_arg_T	syn_opt_arg;
    पूर्णाङ्क			conceal_char = NUL;

    // Isolate the group name, check for validity
    rest = get_group_name(arg, &group_name_end);

    pat_ptrs[0] = रिक्त;
    pat_ptrs[1] = रिक्त;
    pat_ptrs[2] = रिक्त;

    init_syn_patterns();

    syn_opt_arg.flags = 0;
    syn_opt_arg.keyword = FALSE;
    syn_opt_arg.sync_idx = रिक्त;
    syn_opt_arg.has_cont_list = TRUE;
    syn_opt_arg.cont_list = रिक्त;
    syn_opt_arg.cont_in_list = रिक्त;
    syn_opt_arg.next_list = रिक्त;

    /*
     * get the options, patterns and matchgroup.
     */
    जबसम्म (rest != रिक्त && !ends_excmd2(eap->cmd, rest))
    {
	// Check for option arguments
	rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);
	यदि (rest == रिक्त || ends_excmd2(eap->cmd, rest))
	    रोक;

	// must be a pattern or matchgroup then
	साँचो_end = rest;
	जबसम्म (*साँचो_end && !VIM_ISWHITE(*साँचो_end) && *साँचो_end != '=')
	    ++साँचो_end;
	vim_free(साँचो);
	साँचो = vim_strnsave_up(rest, साँचो_end - rest);
	यदि (साँचो == रिक्त)			// out of memory
	{
	    rest = रिक्त;
	    रोक;
	}
	यदि (STRCMP(साँचो, "MATCHGROUP") == 0)
	    item = ITEM_MATCHGROUP;
	अन्यथा यदि (STRCMP(साँचो, "START") == 0)
	    item = ITEM_START;
	अन्यथा यदि (STRCMP(साँचो, "END") == 0)
	    item = ITEM_END;
	अन्यथा यदि (STRCMP(साँचो, "SKIP") == 0)
	{
	    यदि (pat_ptrs[ITEM_SKIP] != रिक्त)	// one skip pattern allowed
	    {
		illegal = TRUE;
		रोक;
	    }
	    item = ITEM_SKIP;
	}
	अन्यथा
	    रोक;
	rest = skipwhite(साँचो_end);
	यदि (*rest != '=')
	{
	    rest = रिक्त;
	    semsg(_(e_missing_equal_str), arg);
	    रोक;
	}
	rest = skipwhite(rest + 1);
	यदि (*rest == NUL)
	{
	    not_enough = TRUE;
	    रोक;
	}

	यदि (item == ITEM_MATCHGROUP)
	{
	    p = skiptowhite(rest);
	    यदि ((p - rest == 4 && STRNCMP(rest, "NONE", 4) == 0) || eap->skip)
		matchgroup_id = 0;
	    अन्यथा
	    {
		matchgroup_id = syn_check_group(rest, (पूर्णाङ्क)(p - rest));
		यदि (matchgroup_id == 0)
		{
		    illegal = TRUE;
		    रोक;
		}
	    }
	    rest = skipwhite(p);
	}
	अन्यथा
	{
	    /*
	     * Allocate room for a syn_pattern, and link it in the list of
	     * syn_patterns for this item, at the start (because the list is
	     * used from end to start).
	     */
	    ppp = ALLOC_ONE(संरचना pat_ptr);
	    यदि (ppp == रिक्त)
	    {
		rest = रिक्त;
		रोक;
	    }
	    ppp->pp_next = pat_ptrs[item];
	    pat_ptrs[item] = ppp;
	    ppp->pp_synp = ALLOC_CLEAR_ONE(synpat_T);
	    यदि (ppp->pp_synp == रिक्त)
	    {
		rest = रिक्त;
		रोक;
	    }

	    /*
	     * Get the syntax pattern and the following offset(s).
	     */
	    // Enable the appropriate \z specials.
	    यदि (item == ITEM_START)
		reg_do_extmatch = REX_SET;
	    अन्यथा यदि (item == ITEM_SKIP || item == ITEM_END)
		reg_do_extmatch = REX_USE;
	    rest = get_syn_pattern(rest, ppp->pp_synp);
	    reg_do_extmatch = 0;
	    यदि (item == ITEM_END && vim_regcomp_had_eol()
				       && !(syn_opt_arg.flags & HL_EXCLUDENL))
		ppp->pp_synp->sp_flags |= HL_HAS_EOL;
	    ppp->pp_matchgroup_id = matchgroup_id;
	    ++pat_गन्ती;
	}
    }
    vim_free(साँचो);
    यदि (illegal || not_enough)
	rest = रिक्त;

    /*
     * Must have a "start" and "end" pattern.
     */
    यदि (rest != रिक्त && (pat_ptrs[ITEM_START] == रिक्त ||
						  pat_ptrs[ITEM_END] == रिक्त))
    {
	not_enough = TRUE;
	rest = रिक्त;
    }

    यदि (rest != रिक्त)
    {
	/*
	 * Check for trailing garbage or command.
	 * If OK, add the item.
	 */
	set_nextcmd(eap, rest);
	यदि (!ends_excmd(*rest) || eap->skip)
	    rest = रिक्त;
	अन्यथा यदि (ga_grow(&(curwin->w_s->b_syn_patterns), pat_गन्ती) == OK
		&& (syn_id = syn_check_group(arg,
					   (पूर्णाङ्क)(group_name_end - arg))) != 0)
	{
	    syn_incl_toplevel(syn_id, &syn_opt_arg.flags);
	    /*
	     * Store the start/skip/end in the syn_items list
	     */
	    idx = curwin->w_s->b_syn_patterns.ga_len;
	    लागि (item = ITEM_START; item <= ITEM_END; ++item)
	    {
		लागि (ppp = pat_ptrs[item]; ppp != रिक्त; ppp = ppp->pp_next)
		{
		    SYN_वस्तुहरू(curwin->w_s)[idx] = *(ppp->pp_synp);
		    SYN_वस्तुहरू(curwin->w_s)[idx].sp_syncing = syncing;
		    SYN_वस्तुहरू(curwin->w_s)[idx].sp_type =
			    (item == ITEM_START) ? SPTYPE_START :
			    (item == ITEM_SKIP) ? SPTYPE_SKIP : SPTYPE_END;
		    SYN_वस्तुहरू(curwin->w_s)[idx].sp_flags |= syn_opt_arg.flags;
		    SYN_वस्तुहरू(curwin->w_s)[idx].sp_syn.id = syn_id;
		    SYN_वस्तुहरू(curwin->w_s)[idx].sp_syn.inc_tag =
							  current_syn_inc_tag;
		    SYN_वस्तुहरू(curwin->w_s)[idx].sp_syn_match_id =
							ppp->pp_matchgroup_id;
#ifdef FEAT_CONCEAL
		    SYN_वस्तुहरू(curwin->w_s)[idx].sp_cchar = conceal_char;
#endif
		    यदि (item == ITEM_START)
		    {
			SYN_वस्तुहरू(curwin->w_s)[idx].sp_cont_list =
							syn_opt_arg.cont_list;
			SYN_वस्तुहरू(curwin->w_s)[idx].sp_syn.cont_in_list =
						     syn_opt_arg.cont_in_list;
			यदि (syn_opt_arg.cont_in_list != रिक्त)
			    curwin->w_s->b_syn_containedin = TRUE;
			SYN_वस्तुहरू(curwin->w_s)[idx].sp_next_list =
							syn_opt_arg.next_list;
		    }
		    ++curwin->w_s->b_syn_patterns.ga_len;
		    ++idx;
#ifdef FEAT_FOLDING
		    यदि (syn_opt_arg.flags & HL_FOLD)
			++curwin->w_s->b_syn_folditems;
#endif
		}
	    }

	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_थाक_free_all(curwin->w_s);	// Need to recompute all syntax.
	    success = TRUE;	    // don't free the progs and patterns now
	}
    }

    /*
     * Free the allocated memory.
     */
    लागि (item = ITEM_START; item <= ITEM_END; ++item)
	लागि (ppp = pat_ptrs[item]; ppp != रिक्त; ppp = ppp_next)
	{
	    यदि (!success && ppp->pp_synp != रिक्त)
	    {
		vim_regfree(ppp->pp_synp->sp_prog);
		vim_free(ppp->pp_synp->sp_pattern);
	    }
	    vim_free(ppp->pp_synp);
	    ppp_next = ppp->pp_next;
	    vim_free(ppp);
	}

    यदि (!success)
    {
	vim_free(syn_opt_arg.cont_list);
	vim_free(syn_opt_arg.cont_in_list);
	vim_free(syn_opt_arg.next_list);
	यदि (not_enough)
	    semsg(_(e_not_enough_arguments_syntax_region_str), arg);
	अन्यथा यदि (illegal || rest == रिक्त)
	    semsg(_(e_invalid_argument_str), arg);
    }
}

/*
 * A simple syntax group ID comparison function suitable for use in qsort()
 */
    स्थिर पूर्णाङ्क
syn_compare_stub(अचर शून्य *v1, अचर शून्य *v2)
{
    अचर लघु	*s1 = v1;
    अचर लघु	*s2 = v2;

    फर्काऊ (*s1 > *s2 ? 1 : *s1 < *s2 ? -1 : 0);
}

/*
 * Combines lists of syntax clusters.
 * *clstr1 and *clstr2 must both be allocated memory; they will be consumed.
 */
    स्थिर शून्य
syn_combine_list(लघु **clstr1, लघु **clstr2, पूर्णाङ्क list_op)
{
    पूर्णाङ्क		count1 = 0;
    पूर्णाङ्क		count2 = 0;
    लघु	*g1;
    लघु	*g2;
    लघु	*clstr = रिक्त;
    पूर्णाङ्क		गन्ती;
    पूर्णाङ्क		round;

    /*
     * Handle degenerate cases.
     */
    यदि (*clstr2 == रिक्त)
	फर्काऊ;
    यदि (*clstr1 == रिक्त || list_op == CLUSTER_REPLACE)
    {
	यदि (list_op == CLUSTER_REPLACE)
	    vim_free(*clstr1);
	यदि (list_op == CLUSTER_REPLACE || list_op == CLUSTER_ADD)
	    *clstr1 = *clstr2;
	अन्यथा
	    vim_free(*clstr2);
	फर्काऊ;
    }

    लागि (g1 = *clstr1; *g1; g1++)
	++count1;
    लागि (g2 = *clstr2; *g2; g2++)
	++count2;

    /*
     * For speed purposes, sort both lists.
     */
    क्विक(*clstr1, (माप_प्रकार)count1, परिमाण(लघु), syn_compare_stub);
    क्विक(*clstr2, (माप_प्रकार)count2, परिमाण(लघु), syn_compare_stub);

    /*
     * We proceed in two passes; in round 1, we count the elements to place
     * in the new list, and in round 2, we allocate and populate the new
     * list.  For speed, we use a mergesort-like method, adding the smaller
     * of the current elements in each list to the new list.
     */
    लागि (round = 1; round <= 2; round++)
    {
	g1 = *clstr1;
	g2 = *clstr2;
	गन्ती = 0;

	/*
	 * First, loop through the lists until one of them is empty.
	 */
	जबसम्म (*g1 && *g2)
	{
	    /*
	     * We always want to add from the first list.
	     */
	    यदि (*g1 < *g2)
	    {
		यदि (round == 2)
		    clstr[गन्ती] = *g1;
		गन्ती++;
		g1++;
		जारी;
	    }
	    /*
	     * We only want to add from the second list if we're adding the
	     * lists.
	     */
	    यदि (list_op == CLUSTER_ADD)
	    {
		यदि (round == 2)
		    clstr[गन्ती] = *g2;
		गन्ती++;
	    }
	    यदि (*g1 == *g2)
		g1++;
	    g2++;
	}

	/*
	 * Now add the leftovers from whichever list didn't get finished
	 * first.  As before, we only want to add from the second list if
	 * we're adding the lists.
	 */
	लागि (; *g1; g1++, गन्ती++)
	    यदि (round == 2)
		clstr[गन्ती] = *g1;
	यदि (list_op == CLUSTER_ADD)
	    लागि (; *g2; g2++, गन्ती++)
		यदि (round == 2)
		    clstr[गन्ती] = *g2;

	यदि (round == 1)
	{
	    /*
	     * If the group ended up empty, we don't need to allocate any
	     * space for it.
	     */
	    यदि (गन्ती == 0)
	    {
		clstr = रिक्त;
		रोक;
	    }
	    clstr = ALLOC_MULT(लघु, गन्ती + 1);
	    यदि (clstr == रिक्त)
		रोक;
	    clstr[गन्ती] = 0;
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
    स्थिर पूर्णाङ्क
syn_scl_name2id(char_u *name)
{
    पूर्णाङ्क		i;
    char_u	*name_u;

    // Avoid using stricmp() too much, it's slow on some systems
    name_u = vim_strsave_up(name);
    यदि (name_u == रिक्त)
	फर्काऊ 0;
    लागि (i = curwin->w_s->b_syn_clusters.ga_len; --i >= 0; )
	यदि (SYN_CLSTR(curwin->w_s)[i].scl_name_u != रिक्त
		&& STRCMP(name_u, SYN_CLSTR(curwin->w_s)[i].scl_name_u) == 0)
	    रोक;
    vim_free(name_u);
    फर्काऊ (i < 0 ? 0 : i + SYNID_CLUSTER);
}

/*
 * Like syn_scl_name2id(), but take a pointer + length argument.
 */
    स्थिर पूर्णाङ्क
syn_scl_namen2id(char_u *linep, पूर्णाङ्क len)
{
    char_u  *name;
    पूर्णाङ्क	    id = 0;

    name = vim_strnsave(linep, len);
    यदि (name == रिक्त)
	फर्काऊ 0;

    id = syn_scl_name2id(name);
    vim_free(name);
    फर्काऊ id;
}

/*
 * Find syntax cluster name in the table and return its ID.
 * The argument is a pointer to the name and the length of the name.
 * If it doesn't exist yet, a new entry is created.
 * Return 0 for failure.
 */
    स्थिर पूर्णाङ्क
syn_check_cluster(char_u *pp, पूर्णाङ्क len)
{
    पूर्णाङ्क		id;
    char_u	*name;

    name = vim_strnsave(pp, len);
    यदि (name == रिक्त)
	फर्काऊ 0;

    id = syn_scl_name2id(name);
    यदि (id == 0)			// doesn't exist yet
	id = syn_add_cluster(name);
    अन्यथा
	vim_free(name);
    फर्काऊ id;
}

/*
 * Add new syntax cluster and return its ID.
 * "name" must be an allocated string, it will be consumed.
 * Return 0 for failure.
 */
    स्थिर पूर्णाङ्क
syn_add_cluster(char_u *name)
{
    पूर्णाङ्क		len;

    /*
     * First call for this growarray: init growing array.
     */
    यदि (curwin->w_s->b_syn_clusters.ga_data == रिक्त)
    {
	curwin->w_s->b_syn_clusters.ga_itemsize = परिमाण(syn_cluster_T);
	curwin->w_s->b_syn_clusters.ga_growsize = 10;
    }

    len = curwin->w_s->b_syn_clusters.ga_len;
    यदि (len >= MAX_CLUSTER_ID)
    {
	emsg(_(e_too_many_syntax_clusters));
	vim_free(name);
	फर्काऊ 0;
    }

    /*
     * Make room for at least one other cluster entry.
     */
    यदि (ga_grow(&curwin->w_s->b_syn_clusters, 1) == FAIL)
    {
	vim_free(name);
	फर्काऊ 0;
    }

    CLEAR_POINTER(&(SYN_CLSTR(curwin->w_s)[len]));
    SYN_CLSTR(curwin->w_s)[len].scl_name = name;
    SYN_CLSTR(curwin->w_s)[len].scl_name_u = vim_strsave_up(name);
    SYN_CLSTR(curwin->w_s)[len].scl_list = रिक्त;
    ++curwin->w_s->b_syn_clusters.ga_len;

    यदि (STRICMP(name, "Spell") == 0)
	curwin->w_s->b_spell_cluster_id = len + SYNID_CLUSTER;
    यदि (STRICMP(name, "NoSpell") == 0)
	curwin->w_s->b_nospell_cluster_id = len + SYNID_CLUSTER;

    फर्काऊ len + SYNID_CLUSTER;
}

/*
 * Handle ":syntax cluster {cluster-name} [contains={groupname},..]
 *		[add={groupname},..] [remove={groupname},..]".
 */
    स्थिर शून्य
syn_cmd_cluster(exarg_T *eap, पूर्णाङ्क syncing UNUSED)
{
    char_u	*arg = eap->arg;
    char_u	*group_name_end;
    char_u	*rest;
    पूर्णाङ्क		scl_id;
    लघु	*clstr_list;
    पूर्णाङ्क		got_clstr = FALSE;
    पूर्णाङ्क		opt_len;
    पूर्णाङ्क		list_op;

    eap->nextcmd = find_nextcmd(arg);
    यदि (eap->skip)
	फर्काऊ;

    rest = get_group_name(arg, &group_name_end);

    यदि (rest != रिक्त)
    {
	scl_id = syn_check_cluster(arg, (पूर्णाङ्क)(group_name_end - arg));
	यदि (scl_id == 0)
	    फर्काऊ;
	scl_id -= SYNID_CLUSTER;

	लागि (;;)
	{
	    यदि (STRNICMP(rest, "add", 3) == 0
		    && (VIM_ISWHITE(rest[3]) || rest[3] == '='))
	    {
		opt_len = 3;
		list_op = CLUSTER_ADD;
	    }
	    अन्यथा यदि (STRNICMP(rest, "remove", 6) == 0
		    && (VIM_ISWHITE(rest[6]) || rest[6] == '='))
	    {
		opt_len = 6;
		list_op = CLUSTER_SUBTRACT;
	    }
	    अन्यथा यदि (STRNICMP(rest, "contains", 8) == 0
			&& (VIM_ISWHITE(rest[8]) || rest[8] == '='))
	    {
		opt_len = 8;
		list_op = CLUSTER_REPLACE;
	    }
	    अन्यथा
		रोक;

	    clstr_list = रिक्त;
	    यदि (get_id_list(&rest, opt_len, &clstr_list, eap->skip) == FAIL)
	    {
		semsg(_(e_invalid_argument_str), rest);
		रोक;
	    }
	    यदि (scl_id >= 0)
		syn_combine_list(&SYN_CLSTR(curwin->w_s)[scl_id].scl_list,
			     &clstr_list, list_op);
	    अन्यथा
		vim_free(clstr_list);
	    got_clstr = TRUE;
	}

	यदि (got_clstr)
	{
	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_थाक_free_all(curwin->w_s);	// Need to recompute all.
	}
    }

    यदि (!got_clstr)
	emsg(_(e_no_cluster_specified));
    यदि (rest == रिक्त || !ends_excmd2(eap->cmd, rest))
	semsg(_(e_invalid_argument_str), arg);
}

/*
 * On first call for current buffer: Init growing array.
 */
    स्थिर शून्य
init_syn_patterns(शून्य)
{
    curwin->w_s->b_syn_patterns.ga_itemsize = परिमाण(synpat_T);
    curwin->w_s->b_syn_patterns.ga_growsize = 10;
}

/*
 * Get one pattern for a ":syntax match" or ":syntax region" command.
 * Stores the pattern and program in a synpat_T.
 * Returns a pointer to the next argument, or NULL in case of an error.
 */
    स्थिर char_u *
get_syn_pattern(char_u *arg, synpat_T *ci)
{
    char_u	*end;
    पूर्णाङ्क		*p;
    पूर्णाङ्क		idx;
    char_u	*cpo_save;

    // need at least three chars
    यदि (arg == रिक्त || arg[0] == NUL || arg[1] == NUL || arg[2] == NUL)
	फर्काऊ रिक्त;

    end = skip_regexp(arg + 1, *arg, TRUE);
    यदि (*end != *arg)			    // end delimiter not found
    {
	semsg(_(e_pattern_delimiter_not_found_str), arg);
	फर्काऊ रिक्त;
    }
    // store the pattern and compiled regexp program
    यदि ((ci->sp_pattern = vim_strnsave(arg + 1, end - arg - 1)) == रिक्त)
	फर्काऊ रिक्त;

    // Make 'cpoptions' empty, to avoid the 'l' flag
    cpo_save = p_cpo;
    p_cpo = empty_option;
    ci->sp_prog = vim_regcomp(ci->sp_pattern, RE_MAGIC);
    p_cpo = cpo_save;

    यदि (ci->sp_prog == रिक्त)
	फर्काऊ रिक्त;
    ci->sp_ic = curwin->w_s->b_syn_ic;
#ifdef FEAT_PROFILE
    syn_clear_time(&ci->sp_time);
#endif

    /*
     * Check for a match, highlight or region offset.
     */
    ++end;
    गर
    {
	लागि (idx = SPO_गन्ती; --idx >= 0; )
	    यदि (STRNCMP(end, spo_name_tab[idx], 3) == 0)
		रोक;
	यदि (idx >= 0)
	{
	    p = &(ci->sp_offsets[idx]);
	    यदि (idx != SPO_LC_OFF)
		विकल्प (end[3])
		{
		    पक्ष 's':   रोक;
		    पक्ष 'b':   रोक;
		    पक्ष 'e':   idx += SPO_गन्ती; रोक;
		    स्वभाव:    idx = -1; रोक;
		}
	    यदि (idx >= 0)
	    {
		ci->sp_off_flags |= (1 << idx);
		यदि (idx == SPO_LC_OFF)	    // lc=99
		{
		    end += 3;
		    *p = getdigits(&end);

		    // "lc=" offset automatically sets "ms=" offset
		    यदि (!(ci->sp_off_flags & (1 << SPO_MS_OFF)))
		    {
			ci->sp_off_flags |= (1 << SPO_MS_OFF);
			ci->sp_offsets[SPO_MS_OFF] = *p;
		    }
		}
		अन्यथा			    // yy=x+99
		{
		    end += 4;
		    यदि (*end == '+')
		    {
			++end;
			*p = getdigits(&end);		// positive offset
		    }
		    अन्यथा यदि (*end == '-')
		    {
			++end;
			*p = -getdigits(&end);		// negative offset
		    }
		}
		यदि (*end != ',')
		    रोक;
		++end;
	    }
	}
    } जबसम्म (idx >= 0);

    यदि (!ends_excmd2(arg, end) && !VIM_ISWHITE(*end))
    {
	semsg(_(e_garbage_after_pattern_str), arg);
	फर्काऊ रिक्त;
    }
    फर्काऊ skipwhite(end);
}

/*
 * Handle ":syntax sync .." command.
 */
    स्थिर शून्य
syn_cmd_sync(exarg_T *eap, पूर्णाङ्क syncing UNUSED)
{
    char_u	*arg_start = eap->arg;
    char_u	*arg_end;
    char_u	*साँचो = रिक्त;
    char_u	*next_arg;
    पूर्णाङ्क		illegal = FALSE;
    पूर्णाङ्क		finished = FALSE;
    दीर्घ	n;
    char_u	*cpo_save;

    यदि (ends_excmd2(eap->cmd, arg_start))
    {
	syn_cmd_list(eap, TRUE);
	फर्काऊ;
    }

    जबसम्म (!ends_excmd2(eap->cmd, arg_start))
    {
	arg_end = skiptowhite(arg_start);
	next_arg = skipwhite(arg_end);
	vim_free(साँचो);
	साँचो = vim_strnsave_up(arg_start, arg_end - arg_start);
	यदि (साँचो == रिक्त)
	    रोक;
	यदि (STRCMP(साँचो, "CCOMMENT") == 0)
	{
	    यदि (!eap->skip)
		curwin->w_s->b_syn_sync_flags |= SF_CCOMMENT;
	    यदि (!ends_excmd2(eap->cmd, next_arg))
	    {
		arg_end = skiptowhite(next_arg);
		यदि (!eap->skip)
		    curwin->w_s->b_syn_sync_id = syn_check_group(next_arg,
						   (पूर्णाङ्क)(arg_end - next_arg));
		next_arg = skipwhite(arg_end);
	    }
	    अन्यथा यदि (!eap->skip)
		curwin->w_s->b_syn_sync_id = syn_name2id((char_u *)"Comment");
	}
	अन्यथा यदि (  STRNCMP(साँचो, "LINES", 5) == 0
		|| STRNCMP(साँचो, "MINLINES", 8) == 0
		|| STRNCMP(साँचो, "MAXLINES", 8) == 0
		|| STRNCMP(साँचो, "LINEBREAKS", 10) == 0)
	{
	    यदि (साँचो[4] == 'S')
		arg_end = साँचो + 6;
	    अन्यथा यदि (साँचो[0] == 'L')
		arg_end = साँचो + 11;
	    अन्यथा
		arg_end = साँचो + 9;
	    यदि (arg_end[-1] != '=' || !VIM_ISDIGIT(*arg_end))
	    {
		illegal = TRUE;
		रोक;
	    }
	    n = getdigits(&arg_end);
	    यदि (!eap->skip)
	    {
		यदि (साँचो[4] == 'B')
		    curwin->w_s->b_syn_sync_linebreaks = n;
		अन्यथा यदि (साँचो[1] == 'A')
		    curwin->w_s->b_syn_sync_maxlines = n;
		अन्यथा
		    curwin->w_s->b_syn_sync_minlines = n;
	    }
	}
	अन्यथा यदि (STRCMP(साँचो, "FROMSTART") == 0)
	{
	    यदि (!eap->skip)
	    {
		curwin->w_s->b_syn_sync_minlines = MAXLNUM;
		curwin->w_s->b_syn_sync_maxlines = 0;
	    }
	}
	अन्यथा यदि (STRCMP(साँचो, "LINECONT") == 0)
	{
	    यदि (*next_arg == NUL)	   // missing pattern
	    {
		illegal = TRUE;
		रोक;
	    }
	    यदि (curwin->w_s->b_syn_linecont_pat != रिक्त)
	    {
		emsg(_(e_syntax_sync_पङ्क्ति_continuations_pattern_specified_twice));
		finished = TRUE;
		रोक;
	    }
	    arg_end = skip_regexp(next_arg + 1, *next_arg, TRUE);
	    यदि (*arg_end != *next_arg)	    // end delimiter not found
	    {
		illegal = TRUE;
		रोक;
	    }

	    यदि (!eap->skip)
	    {
		// store the pattern and compiled regexp program
		यदि ((curwin->w_s->b_syn_linecont_pat =
			    vim_strnsave(next_arg + 1,
				      arg_end - next_arg - 1)) == रिक्त)
		{
		    finished = TRUE;
		    रोक;
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

		यदि (curwin->w_s->b_syn_linecont_prog == रिक्त)
		{
		    VIM_CLEAR(curwin->w_s->b_syn_linecont_pat);
		    finished = TRUE;
		    रोक;
		}
	    }
	    next_arg = skipwhite(arg_end + 1);
	}
	अन्यथा
	{
	    eap->arg = next_arg;
	    यदि (STRCMP(साँचो, "MATCH") == 0)
		syn_cmd_match(eap, TRUE);
	    अन्यथा यदि (STRCMP(साँचो, "REGION") == 0)
		syn_cmd_region(eap, TRUE);
	    अन्यथा यदि (STRCMP(साँचो, "CLEAR") == 0)
		syn_cmd_clear(eap, TRUE);
	    अन्यथा
		illegal = TRUE;
	    finished = TRUE;
	    रोक;
	}
	arg_start = next_arg;
    }
    vim_free(साँचो);
    यदि (illegal)
	semsg(_(e_illegal_arguments_str), arg_start);
    अन्यथा यदि (!finished)
    {
	set_nextcmd(eap, arg_start);
	redraw_curbuf_later(UPD_SOME_VALID);
	syn_थाक_free_all(curwin->w_s);	// Need to recompute all syntax.
    }
}

/*
 * Convert a line of highlight group names into a list of group ID numbers.
 * "arg" should point to the "contains" or "nextgroup" keyword.
 * "arg" is advanced to after the last group name.
 * Careful: the argument is modified (NULs added).
 * returns FAIL for some error, OK for success.
 */
    स्थिर पूर्णाङ्क
get_id_list(
    char_u	**arg,
    पूर्णाङ्क		keylen,		// length of keyword
    लघु	**list,		// where to store the resulting list, if not
				// NULL, the list is silently skipped!
    पूर्णाङ्क		skip)
{
    char_u	*p = रिक्त;
    char_u	*end;
    पूर्णाङ्क		round;
    पूर्णाङ्क		गन्ती;
    पूर्णाङ्क		total_गन्ती = 0;
    लघु	*retval = रिक्त;
    char_u	*name;
    regmatch_T	regmatch;
    पूर्णाङ्क		id;
    पूर्णाङ्क		i;
    पूर्णाङ्क		failed = FALSE;

    /*
     * We parse the list twice:
     * round == 1: count the number of items, allocate the array.
     * round == 2: fill the array with the items.
     * In round 1 new groups may be added, causing the number of items to
     * grow when a regexp is used.  In that case round 1 is done once again.
     */
    लागि (round = 1; round <= 2; ++round)
    {
	/*
	 * skip "contains"
	 */
	p = skipwhite(*arg + keylen);
	यदि (*p != '=')
	{
	    semsg(_(e_missing_equal_sign_str), *arg);
	    रोक;
	}
	p = skipwhite(p + 1);
	यदि (ends_excmd2(*arg, p))
	{
	    semsg(_(e_empty_argument_str), *arg);
	    रोक;
	}

	/*
	 * parse the arguments after "contains"
	 */
	गन्ती = 0;
	जबसम्म (!ends_excmd2(*arg, p))
	{
	    लागि (end = p; *end && !VIM_ISWHITE(*end) && *end != ','; ++end)
		;
	    name = alloc(end - p + 3);	    // leave room for "^$"
	    यदि (name == रिक्त)
	    {
		failed = TRUE;
		रोक;
	    }
	    vim_strncpy(name + 1, p, end - p);
	    यदि (       STRCMP(name + 1, "ALLBUT") == 0
		    || STRCMP(name + 1, "ALL") == 0
		    || STRCMP(name + 1, "TOP") == 0
		    || STRCMP(name + 1, "CONTAINED") == 0)
	    {
		यदि (TOUPPER_ASC(**arg) != 'C')
		{
		    semsg(_(e_str_not_allowed_here), name + 1);
		    failed = TRUE;
		    vim_free(name);
		    रोक;
		}
		यदि (गन्ती != 0)
		{
		    semsg(_(e_str_must_be_first_in_contains_list), name + 1);
		    failed = TRUE;
		    vim_free(name);
		    रोक;
		}
		यदि (name[1] == 'A')
		    id = SYNID_ALLBUT;
		अन्यथा यदि (name[1] == 'T')
		    id = SYNID_शीर्ष;
		अन्यथा
		    id = SYNID_CONTAINED;
		id += current_syn_inc_tag;
	    }
	    अन्यथा यदि (name[1] == '@')
	    {
		यदि (skip)
		    id = -1;
		अन्यथा
		    id = syn_check_cluster(name + 2, (पूर्णाङ्क)(end - p - 1));
	    }
	    अन्यथा
	    {
		/*
		 * Handle full group name.
		 */
		यदि (vim_strpbrk(name + 1, (char_u *)"\\.*^$~[") == रिक्त)
		    id = syn_check_group(name + 1, (पूर्णाङ्क)(end - p));
		अन्यथा
		{
		    /*
		     * Handle match of regexp with group names.
		     */
		    *name = '^';
		    STRCAT(name, "$");
		    regmatch.regprog = vim_regcomp(name, RE_MAGIC);
		    यदि (regmatch.regprog == रिक्त)
		    {
			failed = TRUE;
			vim_free(name);
			रोक;
		    }

		    regmatch.rm_ic = TRUE;
		    id = 0;
		    लागि (i = highlight_अङ्क_groups(); --i >= 0; )
		    {
			यदि (vim_regexec(&regmatch, highlight_group_name(i),
								  (colnr_T)0))
			{
			    यदि (round == 2)
			    {
				// Got more items than expected; can happen
				// when adding items that match:
				// "contains=a.*b,axb".
				// Go back to first round
				यदि (गन्ती >= total_गन्ती)
				{
				    vim_free(retval);
				    round = 1;
				}
				अन्यथा
				    retval[गन्ती] = i + 1;
			    }
			    ++गन्ती;
			    id = -1;	    // remember that we found one
			}
		    }
		    vim_regfree(regmatch.regprog);
		}
	    }
	    vim_free(name);
	    यदि (id == 0)
	    {
		semsg(_(e_unknown_group_name_str), p);
		failed = TRUE;
		रोक;
	    }
	    यदि (id > 0)
	    {
		यदि (round == 2)
		{
		    // Got more items than expected, go back to first round
		    यदि (गन्ती >= total_गन्ती)
		    {
			vim_free(retval);
			round = 1;
		    }
		    अन्यथा
			retval[गन्ती] = id;
		}
		++गन्ती;
	    }
	    p = skipwhite(end);
	    यदि (*p != ',')
		रोक;
	    p = skipwhite(p + 1);	// skip comma in between arguments
	}
	यदि (failed)
	    रोक;
	यदि (round == 1)
	{
	    retval = ALLOC_MULT(लघु, गन्ती + 1);
	    यदि (retval == रिक्त)
		रोक;
	    retval[गन्ती] = 0;	    // zero means end of the list
	    total_गन्ती = गन्ती;
	}
    }

    *arg = p;
    यदि (failed || retval == रिक्त)
    {
	vim_free(retval);
	फर्काऊ FAIL;
    }

    यदि (*list == रिक्त)
	*list = retval;
    अन्यथा
	vim_free(retval);	// list already found, don't overwrite it

    फर्काऊ OK;
}

/*
 * Make a copy of an ID list.
 */
    स्थिर लघु *
copy_id_list(लघु *list)
{
    पूर्णाङ्क	    len;
    पूर्णाङ्क	    गन्ती;
    लघु   *retval;

    यदि (list == रिक्त)
	फर्काऊ रिक्त;

    लागि (गन्ती = 0; list[गन्ती]; ++गन्ती)
	;
    len = (गन्ती + 1) * परिमाण(लघु);
    retval = alloc(len);
    यदि (retval != रिक्त)
	mch_memmove(retval, list, (माप_प्रकार)len);

    फर्काऊ retval;
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

प्रकारनिर्देश संरचना
{
    लघु	*idl_साँचो;	// the list this entry resolves (NULL when empty)
    लघु	*idl_ids;	// sorted, cluster-expanded group IDs (allocated)
    पूर्णाङ्क		idl_गन्ती;	// number of IDs in idl_ids
    लघु	idl_marker;	// leading ALLBUT/TOP/CONTAINED item, or 0
    bool	idl_usable;	// false: not cacheable, use the slow path
} idl_entry_T;

प्रकारनिर्देश संरचना
{
    idl_entry_T	ent[IDL_CACHE_SIZE];
    पूर्णाङ्क		round;		// round-robin replacement index
    पूर्णाङ्क		last;		// index of the last returned entry (fast path)
} idl_cache_T;

/*
 * Collect the explicit group IDs of "list" into "ga", expanding clusters.
 * "seen" records visited clusters to break cycles and avoid duplicate work.
 * Return false when the list cannot be represented as a plain set (a nested
 * ALLBUT/TOP/CONTAINED marker) or on out of memory.
 */
    स्थिर bool
idl_collect(लघु *list, garray_T *ga, garray_T *seen)
{
    यदि (list == रिक्त || list == ID_LIST_ALL)
	फर्काऊ false;
    लागि (लघु item = *list; item != 0; item = *++list)
    {
	यदि (item >= SYNID_ALLBUT && item < SYNID_CLUSTER)
	    फर्काऊ false;	// nested ALLBUT/TOP/CONTAINED: not cacheable
	यदि (item >= SYNID_CLUSTER)
	{
	    लघु	*scl_list;
	    bool	seen_it = false;

	    लागि (पूर्णाङ्क i = 0; i < seen->ga_len; ++i)
		यदि (((लघु *)seen->ga_data)[i] == item)
		{
		    seen_it = true;
		    रोक;
		}
	    यदि (seen_it)
		जारी;
	    यदि (ga_grow(seen, 1) == FAIL)
		फर्काऊ false;
	    ((लघु *)seen->ga_data)[seen->ga_len++] = item;
	    // Record the cluster id itself, not just its members: the slow
	    // path matches "item == id" for a cluster id present in the list.
	    यदि (ga_grow(ga, 1) == FAIL)
		फर्काऊ false;
	    ((लघु *)ga->ga_data)[ga->ga_len++] = item;
	    scl_list = SYN_CLSTR(syn_block)[item - SYNID_CLUSTER].scl_list;
	    यदि (scl_list != रिक्त && !idl_collect(scl_list, ga, seen))
		फर्काऊ false;
	}
	अन्यथा
	{
	    यदि (ga_grow(ga, 1) == FAIL)
		फर्काऊ false;
	    ((लघु *)ga->ga_data)[ga->ga_len++] = item;
	}
    }
    फर्काऊ true;
}

    स्थिर पूर्णाङ्क
idl_id_cmp(अचर शून्य *a, अचर शून्य *b)
{
    फर्काऊ (पूर्णाङ्क)*(अचर लघु *)a - (पूर्णाङ्क)*(अचर लघु *)b;
}

/*
 * Get the cache entry that resolves "list", building it if needed.
 * Returns NULL only on allocation failure.
 */
    स्थिर idl_entry_T *
idl_get_entry(लघु *list)
{
    idl_cache_T	*cache = (idl_cache_T *)syn_block->b_idlist_cache;
    idl_entry_T	*e;
    garray_T	ga, seen;

    यदि (cache == रिक्त)
    {
	cache = ALLOC_CLEAR_ONE(idl_cache_T);
	यदि (cache == रिक्त)
	    फर्काऊ रिक्त;
	syn_block->b_idlist_cache = cache;
    }
    // Fast path: the same list is usually queried many times in a row.
    यदि (cache->ent[cache->last].idl_साँचो == list)
	फर्काऊ &cache->ent[cache->last];
    लागि (पूर्णाङ्क i = 0; i < IDL_CACHE_SIZE; ++i)
	यदि (cache->ent[i].idl_साँचो == list)
	{
	    cache->last = i;
	    फर्काऊ &cache->ent[i];
	}

    // Build a new entry, replacing one in round-robin order.
    cache->last = cache->round;
    e = &cache->ent[cache->round];
    cache->round = (cache->round + 1) % IDL_CACHE_SIZE;
    VIM_CLEAR(e->idl_ids);
    e->idl_गन्ती = 0;
    e->idl_साँचो = list;
    e->idl_marker = 0;
    e->idl_usable = true;

    // A leading ALLBUT/TOP/CONTAINED marker is kept aside; the rest is the set.
    यदि (*list >= SYNID_ALLBUT && *list < SYNID_CLUSTER)
    {
	e->idl_marker = *list;
	++list;
    }

    ga_init2(&ga, परिमाण(लघु), 50);
    ga_init2(&seen, परिमाण(लघु), 10);
    यदि (!idl_collect(list, &ga, &seen))
    {
	e->idl_usable = false;
	ga_clear(&ga);
	ga_clear(&seen);
	फर्काऊ e;
    }
    ga_clear(&seen);
    यदि (ga.ga_len > 0)
    {
	क्विक(ga.ga_data, (माप_प्रकार)ga.ga_len, परिमाण(लघु), idl_id_cmp);
	e->idl_ids = (लघु *)ga.ga_data;	// take ownership
	e->idl_गन्ती = ga.ga_len;
    }
    अन्यथा
	ga_clear(&ga);
    फर्काऊ e;
}

/*
 * Return true if "id" is in the resolved set of cache entry "e".
 */
    स्थिर bool
idl_contains(idl_entry_T *e, लघु id)
{
    पूर्णाङ्क	lo = 0;
    पूर्णाङ्क	hi = e->idl_गन्ती - 1;

    जबसम्म (lo <= hi)
    {
	पूर्णाङ्क	mid = (lo + hi) / 2;
	लघु	v = e->idl_ids[mid];

	यदि (v == id)
	    फर्काऊ true;
	यदि (v < id)
	    lo = mid + 1;
	अन्यथा
	    hi = mid - 1;
    }
    फर्काऊ false;
}

    स्थिर शून्य
idl_cache_clear(synblock_T *block)
{
    idl_cache_T	*cache = (idl_cache_T *)block->b_idlist_cache;

    यदि (cache == रिक्त)
	फर्काऊ;
    लागि (पूर्णाङ्क i = 0; i < IDL_CACHE_SIZE; ++i)
	vim_free(cache->ent[i].idl_ids);
    vim_free(cache);
    block->b_idlist_cache = रिक्त;
}
// }}}

/*
 * Check if syntax group "ssp" is in the ID list "list" of "cur_si".
 * "cur_si" can be NULL if not checking the "containedin" list.
 * Used to check if a syntax item is in the "contains" or "nextgroup" list of
 * the current item.
 * This function is called very often, keep it fast!!
 */
    स्थिर पूर्णाङ्क
in_id_list(
    stateitem_T	*cur_si,	// current item or NULL
    लघु	*list,		// id list
    संरचना sp_syn *ssp,		// group id and ":syn include" tag of group
    पूर्णाङ्क		flags)		// group flags
{
    पूर्णाङ्क		retval;
    लघु	*scl_list;
    लघु	item;
    लघु	id = ssp->id;
    स्थिर पूर्णाङ्क	depth = 0;
    पूर्णाङ्क		r;
    पूर्णाङ्क		toplevel;
    idl_entry_T	*e;

    // If ssp has a "containedin" list and "cur_si" is in it, return TRUE.
    यदि (cur_si != रिक्त && ssp->cont_in_list != रिक्त
					    && !(cur_si->si_flags & HL_MATCH))
    {
	// Ignore transparent items without a contains argument.  Double check
	// that we don't go back past the first one.
	जबसम्म ((cur_si->si_flags & HL_TRANS_CONT)
		&& cur_si > (stateitem_T *)(current_state.ga_data))
	    --cur_si;
	// cur_si->si_idx is -1 for keywords, these never contain anything.
	यदि (cur_si->si_idx >= 0 && in_id_list(रिक्त, ssp->cont_in_list,
		&(SYN_वस्तुहरू(syn_block)[cur_si->si_idx].sp_syn),
		  SYN_वस्तुहरू(syn_block)[cur_si->si_idx].sp_flags))
	    फर्काऊ TRUE;
    }

    यदि (list == रिक्त)
	फर्काऊ FALSE;

    /*
     * If list is ID_LIST_ALL, we are in a transparent item that isn't
     * inside anything.  Only allow not-contained groups.
     */
    यदि (list == ID_LIST_ALL)
	फर्काऊ !(flags & HL_CONTAINED);

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
    e = disable_syn_idlist_cache_for_testing ? रिक्त : idl_get_entry(list);
    यदि (e != रिक्त && e->idl_usable)
    {
	यदि (e->idl_marker != 0)
	{
	    item = e->idl_marker;
	    यदि (item < SYNID_शीर्ष)
	    {
		यदि (item - SYNID_ALLBUT != ssp->inc_tag)
		    फर्काऊ FALSE;
	    }
	    अन्यथा यदि (item < SYNID_CONTAINED)
	    {
		यदि (item - SYNID_शीर्ष != ssp->inc_tag || !toplevel)
		    फर्काऊ FALSE;
	    }
	    अन्यथा
	    {
		यदि (item - SYNID_CONTAINED != ssp->inc_tag || toplevel)
		    फर्काऊ FALSE;
	    }
	    retval = FALSE;
	}
	अन्यथा
	    retval = TRUE;
	फर्काऊ idl_contains(e, id) ? retval : !retval;
    }

    /*
     * Slow path (uncacheable list, e.g. a nested ALLBUT).
     * If the first item is "ALLBUT", return TRUE if "id" is NOT in the
     * contains list.  We also require that "id" is at the same ":syn include"
     * level as the list.
     */
    item = *list;
    यदि (item >= SYNID_ALLBUT && item < SYNID_CLUSTER)
    {
	यदि (item < SYNID_शीर्ष)
	{
	    // ALL or ALLBUT: accept all groups in the same file
	    यदि (item - SYNID_ALLBUT != ssp->inc_tag)
		फर्काऊ FALSE;
	}
	अन्यथा यदि (item < SYNID_CONTAINED)
	{
	    // TOP: accept all not-contained groups in the same file
	    यदि (item - SYNID_शीर्ष != ssp->inc_tag || !toplevel)
		फर्काऊ FALSE;
	}
	अन्यथा
	{
	    // CONTAINED: accept all contained groups in the same file
	    यदि (item - SYNID_CONTAINED != ssp->inc_tag || toplevel)
		फर्काऊ FALSE;
	}
	item = *++list;
	retval = FALSE;
    }
    अन्यथा
	retval = TRUE;

    /*
     * Return "retval" if id is in the contains list.
     */
    जबसम्म (item != 0)
    {
	यदि (item == id)
	    फर्काऊ retval;
	यदि (item >= SYNID_CLUSTER)
	{
	    scl_list = SYN_CLSTR(syn_block)[item - SYNID_CLUSTER].scl_list;
	    // restrict recursiveness to 30 to avoid an endless loop for a
	    // cluster that includes itself (indirectly)
	    यदि (scl_list != रिक्त && depth < 30)
	    {
		++depth;
		r = in_id_list(रिक्त, scl_list, ssp, flags);
		--depth;
		यदि (r)
		    फर्काऊ retval;
	    }
	}
	item = *++list;
    }
    फर्काऊ !retval;
}

संरचना subcommand
{
    वर्ण    *name;			// subcommand name
    शून्य    (*func)(exarg_T *, पूर्णाङ्क);	// function to call
};

स्थिर संरचना subcommand subcommands[] =
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
    शून्य
ex_syntax(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*subcmd_end;
    char_u	*subcmd_name;
    पूर्णाङ्क		i;

    syn_cmdlinep = eap->cmdlinep;

    // isolate subcommand name
    लागि (subcmd_end = arg; ASCII_ISALPHA(*subcmd_end); ++subcmd_end)
	;
    subcmd_name = vim_strnsave(arg, subcmd_end - arg);
    यदि (subcmd_name == रिक्त)
	फर्काऊ;

    यदि (eap->skip)		// skip error messages for all subcommands
	++emsg_skip;
    लागि (i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(subcommands); ++i)
    {
	यदि (STRCMP(subcmd_name, (char_u *)subcommands[i].name) == 0)
	{
	    eap->arg = skipwhite(subcmd_end);
	    (subcommands[i].func)(eap, FALSE);
	    रोक;
	}
    }

    यदि (i == (पूर्णाङ्क)ARRAY_LENGTH(subcommands))
	semsg(_(e_invalid_syntax_subcommand_str), subcmd_name);

    vim_free(subcmd_name);
    यदि (eap->skip)
	--emsg_skip;
}

    शून्य
ex_ownsyntax(exarg_T *eap)
{
    char_u	*old_मान;
    char_u	*new_मान_2;

    यदि (curwin->w_s == &curwin->w_buffer->b_s)
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
    old_मान = get_var_मान((char_u *)"b:current_syntax");
    यदि (old_मान != रिक्त)
	old_मान = vim_strsave(old_मान);

    // Apply the "syntax" autocommand event, this finds and loads the syntax
    // file.
    apply_autocmds(EVENT_SYNTAX, eap->arg, curbuf->b_fname, TRUE, curbuf);

    // move value of b:current_syntax to w:current_syntax
    new_मान_2 = get_var_मान((char_u *)"b:current_syntax");
    यदि (new_मान_2 != रिक्त)
	set_internal_string_var((char_u *)"w:current_syntax", new_मान_2);

    // restore value of b:current_syntax
    यदि (old_मान == रिक्त)
	do_unlet((char_u *)"b:current_syntax", TRUE);
    अन्यथा
    {
	set_internal_string_var((char_u *)"b:current_syntax", old_मान);
	vim_free(old_मान);
    }
}

    पूर्णाङ्क
syntax_present(win_T *win)
{
    फर्काऊ (win->w_s->b_syn_patterns.ga_len != 0
	    || win->w_s->b_syn_clusters.ga_len != 0
	    || win->w_s->b_keywtab.ht_used > 0
	    || win->w_s->b_keywtab_ic.ht_used > 0);
}


स्थिर गणना
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
    शून्य
reset_expand_highlight(शून्य)
{
    include_link = include_default = include_none = 0;
}

/*
 * Handle command line completion for :match and :echohl command: Add "NONE"
 * as highlight group.
 */
    शून्य
set_context_in_echohl_cmd(expand_T *xp, char_u *arg)
{
    xp->xp_context = EXPAND_HIGHLIGHT;
    xp->xp_pattern = arg;
    include_none = 1;
}

/*
 * Handle command line completion for :syntax command.
 */
    शून्य
set_context_in_syntax_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    // Default: expand subcommands
    xp->xp_context = EXPAND_SYNTAX;
    expand_what = EXP_SUBCMD;
    xp->xp_pattern = arg;
    include_link = 0;
    include_default = 0;

    यदि (*arg == NUL)
	फर्काऊ;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    यदि (*p == NUL)
	फर्काऊ;

    // past first word
    xp->xp_pattern = skipwhite(p);
    यदि (*skiptowhite(xp->xp_pattern) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    अन्यथा यदि (STRNICMP(arg, "case", p - arg) == 0)
	expand_what = EXP_CASE;
    अन्यथा यदि (STRNICMP(arg, "spell", p - arg) == 0)
	expand_what = EXP_SPELL;
    अन्यथा यदि (STRNICMP(arg, "sync", p - arg) == 0)
	expand_what = EXP_SYNC;
    अन्यथा यदि (STRNICMP(arg, "list", p - arg) == 0)
    {
	p = skipwhite(p);
	यदि (*p == '@')
	    expand_what = EXP_CLUSTER;
	अन्यथा
	    xp->xp_context = EXPAND_HIGHLIGHT;
    }
    अन्यथा यदि (STRNICMP(arg, "keyword", p - arg) == 0
	    || STRNICMP(arg, "region", p - arg) == 0
	    || STRNICMP(arg, "match", p - arg) == 0)
	xp->xp_context = EXPAND_HIGHLIGHT;
    अन्यथा
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Function given to ExpandGeneric() to obtain the list syntax names for
 * expansion.
 */
    char_u *
get_syntax_name(expand_T *xp, पूर्णाङ्क idx)
{
    विकल्प (expand_what)
    {
	पक्ष EXP_SUBCMD:
	    यदि (idx < 0 || idx >= (पूर्णाङ्क)ARRAY_LENGTH(subcommands))
		फर्काऊ रिक्त;
	    फर्काऊ (char_u *)subcommands[idx].name;
	पक्ष EXP_CASE:
	{
	    स्थिर वर्ण *case_args[] = {"match", "ignore", रिक्त};
	    फर्काऊ (char_u *)case_args[idx];
	}
	पक्ष EXP_SPELL:
	{
	    स्थिर वर्ण *spell_args[] =
		{"toplevel", "notoplevel", "default", रिक्त};
	    फर्काऊ (char_u *)spell_args[idx];
	}
	पक्ष EXP_SYNC:
	{
	    स्थिर वर्ण *sync_args[] =
		{"ccomment", "clear", "fromstart",
		 "linebreaks=", "linecont", "lines=", "match",
		 "maxlines=", "minlines=", "region", रिक्त};
	    फर्काऊ (char_u *)sync_args[idx];
	}
	पक्ष EXP_CLUSTER:
	{
	    यदि (idx < curwin->w_s->b_syn_clusters.ga_len)
	    {
		vim_snprintf((वर्ण *)xp->xp_buf, EXPAND_BUF_LEN, "@%s",
					 SYN_CLSTR(curwin->w_s)[idx].scl_name);
		फर्काऊ xp->xp_buf;
	    }
	    अन्यथा
		फर्काऊ रिक्त;
	}
    }
    फर्काऊ रिक्त;
}


/*
 * Function called for expression evaluation: get syntax ID at file position.
 */
    पूर्णाङ्क
syn_get_id(
    win_T	*wp,
    दीर्घ	lnum,
    colnr_T	col,
    पूर्णाङ्क		trans,	     // remove transparency
    पूर्णाङ्क		*spellp,     // return: can do spell checking
    पूर्णाङ्क		keep_state)  // keep state of char at "col"
{
    // When the position is not after the current position and in the same
    // line of the same window with the same buffer, need to restart parsing.
    यदि (wp != syn_win
	    || wp->w_buffer != syn_buf
	    || lnum != current_lnum
	    || col < current_col)
	syntax_start(wp, lnum);
    अन्यथा यदि (wp->w_buffer == syn_buf
	    && lnum == current_lnum
	    && col > current_col)
	// next_match may not be correct when moving around, e.g. with the
	// "skip" expression in searchpair()
	next_match_idx = -1;

    (शून्य)get_syntax_attr(col, spellp, keep_state);

    फर्काऊ (trans ? current_trans_id : current_id);
}

#यदि defined(FEAT_CONCEAL)
/*
 * Get extra information about the syntax item.  Must be called right after
 * get_syntax_attr().
 * Stores the current item sequence nr in "*seqnrp".
 * Returns the current flags.
 */
    पूर्णाङ्क
get_syntax_info(पूर्णाङ्क *seqnrp)
{
    *seqnrp = current_seqnr;
    फर्काऊ current_flags;
}

/*
 * Return conceal substitution character
 */
    पूर्णाङ्क
syn_get_sub_char(शून्य)
{
    फर्काऊ current_sub_char;
}
#endif

#यदि defined(FEAT_EVAL)
/*
 * Return the syntax ID at position "i" in the current stack.
 * The caller must have called syn_get_id() before to fill the stack.
 * Returns -1 when "i" is out of range.
 */
    पूर्णाङ्क
syn_get_थाक_item(पूर्णाङ्क i)
{
    यदि (i >= current_state.ga_len)
    {
	// Need to invalidate the state, because we didn't properly finish it
	// for the last character, "keep_state" was TRUE.
	invalidate_current_state();
	current_col = MAXCOL;
	फर्काऊ -1;
    }
    फर्काऊ CUR_STATE(i).si_id;
}
#endif

#यदि defined(FEAT_FOLDING)
    स्थिर पूर्णाङ्क
syn_cur_foldlevel(शून्य)
{
    पूर्णाङ्क		level = 0;
    पूर्णाङ्क		i;

    लागि (i = 0; i < current_state.ga_len; ++i)
	यदि (CUR_STATE(i).si_flags & HL_FOLD)
	    ++level;
    फर्काऊ level;
}

/*
 * Function called to get folding level for line "lnum" in window "wp".
 */
    पूर्णाङ्क
syn_get_foldlevel(win_T *wp, दीर्घ lnum)
{
    पूर्णाङ्क		level = 0;
    पूर्णाङ्क		तल्लो_level;
    पूर्णाङ्क		cur_level;

    // Return quickly when there are no fold items at all.
    यदि (wp->w_s->b_syn_folditems != 0
	    && !wp->w_s->b_syn_error
# ifdef SYN_TIME_LIMIT
	    && !wp->w_s->b_syn_slow
# endif
	    )
    {
	syntax_start(wp, lnum);

	// Start with the fold level at the start of the line.
	level = syn_cur_foldlevel();

	यदि (wp->w_s->b_syn_foldlevel == SYNFLD_MINIMUM)
	{
	    // Find the lowest fold level that is followed by a higher one.
	    cur_level = level;
	    तल्लो_level = cur_level;
	    जबसम्म (!current_finished)
	    {
		(शून्य)syn_current_attr(FALSE, FALSE, रिक्त, FALSE);
		cur_level = syn_cur_foldlevel();
		यदि (cur_level < तल्लो_level)
		    तल्लो_level = cur_level;
		अन्यथा यदि (cur_level > तल्लो_level)
		    level = तल्लो_level;
		++current_col;
	    }
	}
    }
    यदि (level > wp->w_p_fdn)
    {
	level = wp->w_p_fdn;
	यदि (level < 0)
	    level = 0;
    }
    फर्काऊ level;
}
#endif

#यदि defined(FEAT_PROFILE)
/*
 * ":syntime".
 */
    शून्य
ex_syntime(exarg_T *eap)
{
    यदि (STRCMP(eap->arg, "on") == 0)
	syn_time_on = TRUE;
    अन्यथा यदि (STRCMP(eap->arg, "off") == 0)
	syn_time_on = FALSE;
    अन्यथा यदि (STRCMP(eap->arg, "clear") == 0)
	syntime_clear();
    अन्यथा यदि (STRCMP(eap->arg, "report") == 0)
	syntime_report();
    अन्यथा
	semsg(_(e_invalid_argument_str), eap->arg);
}

    स्थिर शून्य
syn_clear_time(syn_time_T *st)
{
    profile_zero(&st->total);
    profile_zero(&st->slowest);
    st->गन्ती = 0;
    st->match = 0;
}

/*
 * Clear the syntax timing for the current buffer.
 */
    स्थिर शून्य
syntime_clear(शून्य)
{
    पूर्णाङ्क		idx;
    synpat_T	*spp;

    यदि (!syntax_present(curwin))
    {
	सन्देश(_(सन्देश_no_वस्तुहरू));
	फर्काऊ;
    }
    लागि (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len; ++idx)
    {
	spp = &(SYN_वस्तुहरू(curwin->w_s)[idx]);
	syn_clear_time(&spp->sp_time);
    }
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":syntime {on,off,clear,report}" command.
 */
    char_u *
get_syntime_arg(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    विकल्प (idx)
    {
	पक्ष 0: फर्काऊ (char_u *)"on";
	पक्ष 1: फर्काऊ (char_u *)"off";
	पक्ष 2: फर्काऊ (char_u *)"clear";
	पक्ष 3: फर्काऊ (char_u *)"report";
    }
    फर्काऊ रिक्त;
}

प्रकारनिर्देश संरचना
{
    proftime_T	total;
    पूर्णाङ्क		गन्ती;
    पूर्णाङ्क		match;
    proftime_T	slowest;
    proftime_T	average;
    पूर्णाङ्क		id;
    char_u	*pattern;
} time_entry_T;

    स्थिर पूर्णाङ्क
syn_compare_syntime(अचर शून्य *v1, अचर शून्य *v2)
{
    अचर time_entry_T	*s1 = v1;
    अचर time_entry_T	*s2 = v2;

    फर्काऊ profile_cmp(&s1->total, &s2->total);
}

/*
 * Clear the syntax timing for the current buffer.
 */
    स्थिर शून्य
syntime_report(शून्य)
{
    पूर्णाङ्क		idx;
    synpat_T	*spp;
# यदि defined(FEAT_RELTIME)
    proftime_T	पंचांग;
# endif
    पूर्णाङ्क		len;
    पूर्णाङ्क		patlen;
    proftime_T	total_total;
    पूर्णाङ्क		total_गन्ती = 0;
    garray_T    ga;
    time_entry_T *p;

    यदि (!syntax_present(curwin))
    {
	सन्देश(_(सन्देश_no_वस्तुहरू));
	फर्काऊ;
    }

    ga_init2(&ga, परिमाण(time_entry_T), 50);
    profile_zero(&total_total);
    लागि (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len; ++idx)
    {
	spp = &(SYN_वस्तुहरू(curwin->w_s)[idx]);
	यदि (spp->sp_time.गन्ती > 0)
	{
	    (शून्य)ga_grow(&ga, 1);
	    p = ((time_entry_T *)ga.ga_data) + ga.ga_len;
	    p->total = spp->sp_time.total;
	    profile_add(&total_total, &spp->sp_time.total);
	    p->गन्ती = spp->sp_time.गन्ती;
	    p->match = spp->sp_time.match;
	    total_गन्ती += spp->sp_time.गन्ती;
	    p->slowest = spp->sp_time.slowest;
# यदि defined(FEAT_RELTIME)
	    profile_divide(&spp->sp_time.total, spp->sp_time.गन्ती, &पंचांग);
	    p->average = पंचांग;
# endif
	    p->id = spp->sp_syn.id;
	    p->pattern = spp->sp_pattern;
	    ++ga.ga_len;
	}
    }

    // Sort on total time. Skip if there are no items to avoid passing NULL
    // pointer to qsort().
    यदि (ga.ga_len > 1)
	क्विक(ga.ga_data, (माप_प्रकार)ga.ga_len, परिमाण(time_entry_T),
							 syn_compare_syntime);

    सन्देश_puts_title(_("  TOTAL      COUNT  MATCH   SLOWEST     AVERAGE   NAME               PATTERN"));
    सन्देश_puts("\n");
    लागि (idx = 0; idx < ga.ga_len && !got_int; ++idx)
    {
	p = ((time_entry_T *)ga.ga_data) + idx;

	सन्देश_puts(profile_सन्देश(&p->total));
	सन्देश_puts(" "); // make sure there is always a separating space
	सन्देश_advance(13);
	सन्देश_outnum(p->गन्ती);
	सन्देश_puts(" ");
	सन्देश_advance(20);
	सन्देश_outnum(p->match);
	सन्देश_puts(" ");
	सन्देश_advance(26);
	सन्देश_puts(profile_सन्देश(&p->slowest));
	सन्देश_puts(" ");
	सन्देश_advance(38);
	सन्देश_puts(profile_सन्देश(&p->average));
	सन्देश_puts(" ");
	सन्देश_advance(50);
	सन्देश_outtrans(highlight_group_name(p->id - 1));
	सन्देश_puts(" ");

	सन्देश_advance(69);
	यदि (Columns < 80)
	    len = 20; // will wrap anyway
	अन्यथा
	    len = Columns - 70;
	patlen = (पूर्णाङ्क)STRLEN(p->pattern);
	यदि (len > patlen)
	    len = patlen;
	सन्देश_outtrans_len(p->pattern, len);
	सन्देश_puts("\n");
    }
    ga_clear(&ga);
    यदि (!got_int)
    {
	सन्देश_puts("\n");
	सन्देश_puts(profile_सन्देश(&total_total));
	सन्देश_advance(13);
	सन्देश_outnum(total_गन्ती);
	सन्देश_puts("\n");
    }
}
#endif

#endif // FEAT_SYN_HL
