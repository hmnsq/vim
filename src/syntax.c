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

#ضمن "vim.h"

#إذا defined(FEAT_SYN_HL)

// different types of offsets that are possible
#وصف_ڪريو SPO_MS_OFF	0	// match  start offset
#وصف_ڪريو SPO_ME_OFF	1	// match  end	offset
#وصف_ڪريو SPO_HS_OFF	2	// highl. start offset
#وصف_ڪريو SPO_HE_OFF	3	// highl. end	offset
#وصف_ڪريو SPO_RS_OFF	4	// region start offset
#وصف_ڪريو SPO_RE_OFF	5	// region end	offset
#وصف_ڪريو SPO_LC_OFF	6	// leading context offset
#وصف_ڪريو SPO_ڳڻپ	7

ساكن حرف *(spo_name_tab[SPO_ڳڻپ]) =
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
تعريفنوع بنية syn_نمونو
{
    حرف	 sp_type;		// see SPTYPE_ defines below
    حرف	 sp_syncing;		// this item used for syncing
    ننڍو	 sp_syn_ميلاپ_id;	// highlight group ID of pattern
    ننڍو	 sp_off_flags;		// see below
    صحيح		 sp_offsets[SPO_ڳڻپ];	// offsets
    صحيح		 sp_flags;		// see HL_ defines below
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
    صحيح		 sp_cchar;		// conceal substitute character
#جيڪڏهن_پورو
    صحيح		 sp_ic;			// ignore-case flag for sp_prog
    صحيح		 sp_sync_idx;		// sync item index (syncing only)
    صحيح		 sp_سٽ_id;		// ID of last line where tried
    صحيح		 sp_startcol;		// next match in sp_line_id line
    ننڍو	*sp_cont_list;		// cont. group IDs, if non-zero
    ننڍو	*sp_ايندڙ_list;		// next group IDs, if non-zero
    بنية sp_syn sp_syn;		// struct passed to in_id_list()
    char_u	*sp_نمونو;		// regexp to match, pattern
    regprog_T	*sp_prog;		// regexp to match, program
#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
    syn_وقت_T	 sp_وقت;
#جيڪڏهن_پورو
} synpat_T;

// The sp_off_flags are computed like this:
// offset from the start of the matched text: (1 << SPO_XX_OFF)
// offset from the end	 of the matched text: (1 << (SPO_XX_OFF + SPO_COUNT))
// When both are present, only one is used.

#وصف_ڪريو SPTYPE_ميلاپ	1	// match keyword with this group ID
#وصف_ڪريو SPTYPE_شروعات	2	// match a regexp, start of item
#وصف_ڪريو SPTYPE_پڇاڙي	3	// match a regexp, end of item
#وصف_ڪريو SPTYPE_SKIP	4	// match a regexp, skip within item


#وصف_ڪريو SYN_شيون(buf)	((synpat_T *)((buf)->b_syn_patterns.ga_معلومات))

#وصف_ڪريو NONE_IDX	(-2)	// value of sp_sync_idx for "NONE"

/*
 * Flags for b_syn_sync_flags:
 */
#وصف_ڪريو SF_CCOMMENT	0x01	// sync on a C-style comment
#وصف_ڪريو SF_ميلاپ	0x02	// sync by matching a pattern

#وصف_ڪريو SYN_STATE_P(ssp)    ((bufstate_T *)((ssp)->ga_معلومات))

#وصف_ڪريو MAXKEYWLEN	80	    // maximum length of a keyword

/*
 * The attributes of the syntax item that has been recognized.
 */
ساكن صحيح current_attr = 0;	    // attr of current syntax word
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
ساكن صحيح current_id = 0;	    // ID of current char for syn_get_id()
ساكن صحيح current_trans_id = 0;    // idem, transparency removed
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
ساكن صحيح current_flags = 0;
ساكن صحيح current_seqnr = 0;
ساكن صحيح current_sub_char = 0;
#جيڪڏهن_پورو

تعريفنوع بنية syn_cluster_S
{
    char_u	    *scl_name;	    // syntax cluster name
    char_u	    *scl_name_u;    // uppercase of scl_name
    ننڍو	    *scl_list;	    // IDs in this syntax cluster
} syn_cluster_T;

/*
 * Methods of combining two clusters
 */
#وصف_ڪريو CLUSTER_REPLACE	    1	// replace first list with second
#وصف_ڪريو CLUSTER_جوڙيو	    2	// add second list to first
#وصف_ڪريو CLUSTER_ڪٽيو    3	// subtract second list from first

#وصف_ڪريو SYN_CLSTR(buf)	((syn_cluster_T *)((buf)->b_syn_clusters.ga_معلومات))

/*
 * Syntax group IDs have different types:
 *     0 - 19999  normal syntax groups
 * 20000 - 20999  ALLBUT indicator (current_syn_inc_tag added)
 * 21000 - 21999  TOP indicator (current_syn_inc_tag added)
 * 22000 - 22999  CONTAINED indicator (current_syn_inc_tag added)
 * 23000 - 32767  cluster IDs (subtract SYNID_CLUSTER for the cluster ID)
 */
#وصف_ڪريو SYNID_ALLBUT	MAX_HL_ID   // syntax group ID for contains=ALLBUT
#وصف_ڪريو SYNID_چوٽي	21000	    // syntax group ID for contains=TOP
#وصف_ڪريو SYNID_CONTAINED	22000	    // syntax group ID for contains=CONTAINED
#وصف_ڪريو SYNID_CLUSTER	23000	    // first syntax group ID for clusters

#وصف_ڪريو MAX_SYN_INC_ٽيگ	999	    // maximum before the above overflow
#وصف_ڪريو MAX_CLUSTER_ID  (32767 - SYNID_CLUSTER)

/*
 * Annoying Hack(TM):  ":syn include" needs this pointer to pass to
 * expand_filename().  Most of the other syntax commands don't need it, so
 * instead of passing it to them, we stow it here.
 */
ساكن char_u **syn_cmdlinep;

/*
 * Another Annoying Hack(TM):  To prevent rules from other ":syn include"'d
 * files from leaking into ALLBUT lists, we assign a unique ID to the
 * rules in each ":syn include"'d file.
 */
ساكن صحيح current_syn_inc_ٽيگ = 0;
ساكن صحيح running_syn_inc_ٽيگ = 0;

/*
 * In a hashtable item "hi_key" points to "keyword" in a keyentry.
 * This avoids adding a pointer to the hashtable item.
 * KE2HIKEY() converts a var pointer to a hashitem key pointer.
 * HIKEY2KE() converts a hashitem key pointer to a var pointer.
 * HI2KE() converts a hashitem pointer to a var pointer.
 */
ساكن keyentry_T dumkey;
#وصف_ڪريو KE2HIKEY(kp)  ((kp)->keyword)
#وصف_ڪريو HIKEY2KE(p)   ((keyentry_T *)((p) - (dumkey.keyword - (char_u *)&dumkey)))
#وصف_ڪريو HI2KE(hi)      HIKEY2KE((hi)->hi_ڪنجي)

/*
 * To reduce the time spent in keepend(), remember at which level in the state
 * stack the first item with "keepend" is present.  When "-1", there is no
 * "keepend" on the stack.
 */
ساكن صحيح keepend_level = -1;

ساكن حرف نياپو_no_شيون[] = N_("No Syntax items defined for this buffer");

/*
 * For the current state we need to remember more than just the idx.
 * When si_m_endpos.lnum is 0, the items other than si_idx are unknown.
 * (The end positions have the column number of the next char)
 */
تعريفنوع بنية state_شي
{
    صحيح		si_idx;			// index of syntax pattern or
					// KEYWORD_IDX
    صحيح		si_id;			// highlight group ID for keywords
    صحيح		si_trans_id;		// idem, transparency removed
    صحيح		si_m_lnum;		// lnum of the match
    صحيح		si_m_startcol;		// starting column of the match
    lpos_T	si_m_endpos;		// just after end posn of the match
    lpos_T	si_h_startpos;		// start position of the highlighting
    lpos_T	si_h_endpos;		// end position of the highlighting
    lpos_T	si_eoe_pos;		// end position of end pattern
    صحيح		si_پڇاڙي_idx;		// group ID for end pattern or zero
    صحيح		si_پڇاڙيون;		// if match ends before si_m_endpos
    صحيح		si_attr;		// attributes in this state
    ڊگهو	si_flags;		// HL_HAS_EOL flag in this state, and
					// HL_SKIP* for si_next_list
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
    صحيح		si_seqnr;		// sequence number
    صحيح		si_cchar;		// substitution character for conceal
#جيڪڏهن_پورو
    ننڍو	*si_cont_list;		// list of contained groups
    ننڍو	*si_ايندڙ_list;		// nextgroup IDs after this item ends
    reg_extmatch_T *si_extmatch;	// \z(...\) matches from start
					//نمونو
} stateitem_T;

#وصف_ڪريو KEYWORD_IDX	(-1)	    // value of si_idx for keywords
#وصف_ڪريو ID_LIST_ALL	((ننڍو *)-1) // valid of si_cont_list for containing all
				    // but contained groups

#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
ساكن صحيح ايندڙ_seqnr = 1;		// value to use for si_seqnr
#جيڪڏهن_پورو

/*
 * Struct to reduce the number of arguments to get_syn_options(), it's used
 * very often.
 */
تعريفنوع بنية
{
    صحيح		flags;		// flags for contained and transparent
    صحيح		keyword;	// TRUE for ":syn keyword"
    صحيح		*sync_idx;	// syntax item for "grouphere" argument, NULL
				// if not allowed
    حرف	has_cont_list;	// TRUE if "cont_list" can be used
    ننڍو	*cont_list;	// group IDs for "contains" argument
    ننڍو	*cont_اندريون_list;	// group IDs for "containedin" argument
    ننڍو	*ايندڙ_list;	// group IDs for "nextgroup" argument
} syn_opt_arg_T;

/*
 * The next possible match in the current line for any pattern is remembered,
 * to avoid having to try for a match in each column.
 * If next_match_idx == -1, not tried (in this line) yet.
 * If next_match_col == MAXCOL, no match found in this line.
 * (All end positions have the column of the char after the end)
 */
ساكن صحيح ايندڙ_ميلاپ_کانو;		// column for start of next match
ساكن lpos_T ايندڙ_ميلاپ_m_endpos;	// position for end of next match
ساكن lpos_T ايندڙ_ميلاپ_h_startpos;	// pos. for highl. start of next match
ساكن lpos_T ايندڙ_ميلاپ_h_endpos;	// pos. for highl. end of next match
ساكن صحيح ايندڙ_ميلاپ_idx;		// index of matched item
ساكن ڊگهو ايندڙ_ميلاپ_flags;		// flags for next match
ساكن lpos_T ايندڙ_ميلاپ_eos_pos;	// end of start pattn (start region)
ساكن lpos_T ايندڙ_ميلاپ_eoe_pos;	// pos. for end of end pattern
ساكن صحيح ايندڙ_ميلاپ_پڇاڙي_idx;		// ID of group for end pattn or zero
ساكن reg_extmatch_T *ايندڙ_ميلاپ_extmatch = عدم;

/*
 * A state stack is an array of integers or stateitem_T, stored in a
 * garray_T.  A state stack is invalid if its itemsize entry is zero.
 */
#وصف_ڪريو INVALID_STATE(ssp)  ((ssp)->ga_itemsize == 0)
#وصف_ڪريو VALID_STATE(ssp)    ((ssp)->ga_itemsize != 0)

#وصف_ڪريو FOR_ALL_SYNSTATES(sb, sst) \
    لكل ((sst) = (sb)->b_sst_پهريون; (sst) != عدم; (sst) = (sst)->sst_ايندڙ)

/*
 * The current state (within the line) of the recognition engine.
 * When current_state.ga_itemsize is 0 the current state is invalid.
 */
ساكن win_T	*syn_win;		// current window for highlighting
ساكن buf_T	*syn_buf;		// current buffer for highlighting
ساكن synblock_T *syn_بلاڪ;		// current buffer for highlighting
ساكن linenr_T current_lnum = 0;	// lnum of current state
ساكن colnr_T	current_کانو = 0;	// column of current state
ساكن صحيح	current_state_stored = 0; // TRUE if stored current state
					  // after setting current_finished
ساكن صحيح	current_ختم = 0;	// current line has been finished
ساكن garray_T current_state		// current stack of state_items
		= {0, 0, 0, 0, عدم};
ساكن ننڍو	*current_ايندڙ_list = عدم; // when non-zero, nextgroup list
ساكن صحيح	current_ايندڙ_flags = 0; // flags for current_next_list
ساكن صحيح	current_سٽ_id = 0;	// unique number for current line

#وصف_ڪريو CUR_STATE(idx)	((stateitem_T *)(current_state.ga_معلومات))[idx]

ساكن فراغ syn_sync(win_T *wp, linenr_T lnum, synstate_T *آخري_valid);
ساكن صحيح syn_ميلاپ_linecont(linenr_T lnum);
ساكن فراغ syn_شروعات_سٽ(فراغ);
ساكن فراغ syn_update_پڇاڙيون(صحيح startofline);
ساكن فراغ syn_ڍير_alloc(فراغ);
ساكن صحيح syn_ڍير_cleanup(فراغ);
ساكن فراغ syn_ڍير_free_داخلا(synblock_T *بلاڪ, synstate_T *p);
ساكن synstate_T *syn_ڍير_ڳوليو_داخلا(linenr_T lnum);
ساكن synstate_T *store_current_state(فراغ);
ساكن فراغ ڀريو_current_state(synstate_T *from);
ساكن فراغ invalidate_current_state(فراغ);
ساكن صحيح syn_ڍير_equal(synstate_T *sp);
ساكن فراغ validate_current_state(فراغ);
ساكن صحيح syn_پڄاڻي_سٽ(صحيح syncing);
ساكن صحيح syn_current_attr(صحيح syncing, صحيح displaying, صحيح *can_spell, صحيح keep_state);
ساكن صحيح did_ميلاپ_already(صحيح idx, garray_T *gap);
ساكن stateitem_T *ڌڪو_ايندڙ_ميلاپ(stateitem_T *cur_si);
ساكن فراغ check_state_پڇاڙيون(فراغ);
ساكن فراغ update_si_attr(صحيح idx);
ساكن فراغ check_keepend(فراغ);
ساكن فراغ update_si_پڇاڙي(stateitem_T *sip, صحيح startcol, صحيح force);
ساكن ننڍو *copy_id_list(ننڍو *list);
ساكن صحيح اندريون_id_list(stateitem_T *شي, ننڍو *cont_list, بنية sp_syn *ssp, صحيح flags);
ساكن صحيح ڌڪو_current_state(صحيح idx);
ساكن فراغ ڪڍو_current_state(فراغ);
#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
ساكن فراغ syn_clear_وقت(syn_وقت_T *tt);
ساكن فراغ syntime_clear(فراغ);
ساكن فراغ syntime_report(فراغ);
ساكن صحيح syn_وقت_on = FALSE;
# define IF_SYN_وقت(p) (p)
#وإلا
# define IF_SYN_وقت(p) عدم
تعريفنوع صحيح syn_وقت_T;
#جيڪڏهن_پورو

ساكن فراغ syn_ڍير_apply_changes_بلاڪ(synblock_T *بلاڪ, buf_T *buf);
ساكن فراغ ڳوليو_endpos(صحيح idx, lpos_T *startpos, lpos_T *m_endpos, lpos_T *hl_endpos, ڊگهو *flagsp, lpos_T *پڇاڙي_endpos, صحيح *پڇاڙي_idx, reg_extmatch_T *شروعات_ext);

ساكن فراغ حد_بندي_pos(lpos_T *pos, lpos_T *حد_بندي);
ساكن فراغ حد_بندي_pos_zero(lpos_T *pos, lpos_T *حد_بندي);
ساكن فراغ syn_جوڙيو_پڇاڙي_off(lpos_T *نتيجو, regmmatch_T *regmatch, synpat_T *spp, صحيح idx, صحيح extra);
ساكن فراغ syn_جوڙيو_شروعات_off(lpos_T *نتيجو, regmmatch_T *regmatch, synpat_T *spp, صحيح idx, صحيح extra);
ساكن char_u *syn_getcurline(فراغ);
ساكن colnr_T syn_getcurline_len(فراغ);
ساكن صحيح syn_regexec(regmmatch_T *rmp, linenr_T lnum, colnr_T کانو, syn_وقت_T *st);
ساكن صحيح check_keyword_id(char_u *سٽ, صحيح startcol, صحيح *endcol, ڊگهو *flags, ننڍو **ايندڙ_list, stateitem_T *cur_si, صحيح *ccharp);
ساكن فراغ syn_remove_نمونو(synblock_T *بلاڪ, صحيح idx);
ساكن فراغ syn_clear_نمونو(synblock_T *بلاڪ, صحيح i);
ساكن فراغ syn_clear_cluster(synblock_T *بلاڪ, صحيح i);
ساكن فراغ syn_clear_one(صحيح id, صحيح syncing);
ساكن فراغ syn_cmd_onoff(exarg_T *eap, حرف *name);
ساكن فراغ syn_سٽون_نياپو(فراغ);
ساكن فراغ syn_ميلاپ_نياپو(فراغ);
ساكن فراغ syn_list_one(صحيح id, صحيح syncing, صحيح link_only);
ساكن فراغ syn_list_cluster(صحيح id);
ساكن فراغ put_id_list(char_u *name, ننڍو *list, صحيح attr);
ساكن فراغ put_نمونو(حرف *s, صحيح c, synpat_T *spp, صحيح attr);
ساكن صحيح syn_list_keywords(صحيح id, hashtab_T *ht, صحيح did_سرو, صحيح attr);
ساكن فراغ syn_clear_keyword(صحيح id, hashtab_T *ht);
ساكن فراغ clear_keywtab(hashtab_T *ht);
ساكن صحيح syn_scl_namen2id(char_u *linep, صحيح len);
ساكن صحيح syn_check_cluster(char_u *pp, صحيح len);
ساكن صحيح syn_جوڙيو_cluster(char_u *name);
ساكن فراغ init_syn_patterns(فراغ);
ساكن char_u *get_syn_نمونو(char_u *arg, synpat_T *ci);
ساكن صحيح get_id_list(char_u **arg, صحيح keylen, ننڍو **list, صحيح skip);
ساكن فراغ syn_combine_list(ننڍو **clstr1, ننڍو **clstr2, صحيح list_op);
ساكن فراغ idl_cache_clear(synblock_T *بلاڪ);

/*
 * Start the syntax recognition for a line.  This function is normally called
 * from the screen updating, once for each displayed line.
 * The buffer is remembered in syn_buf, because get_syntax_attr() doesn't get
 * it.	Careful: curbuf and curwin are likely to point to another buffer and
 * window.
 */
    فراغ
syntax_شروعات(win_T *wp, linenr_T lnum)
{
    synstate_T	*p;
    synstate_T	*آخري_valid = عدم;
    synstate_T	*آخري_min_valid = عدم;
    synstate_T	*sp, *prev = عدم;
    linenr_T	parsed_lnum;
    linenr_T	پهريون_stored;
    صحيح		dist;
    ساكن varnumber_T changedtick = 0;	// remember the last change ID

#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
    current_sub_char = NUL;
#جيڪڏهن_پورو

    /*
     * After switching buffers, invalidate current_state.
     * Also do this when a change was made, the current state may be invalid
     * then.
     */
    إذا (syn_بلاڪ != wp->w_s
	    || syn_buf != wp->w_بفر
	    || changedtick != CHANGEDTICK(syn_buf))
    {
	invalidate_current_state();
	syn_buf = wp->w_بفر;
	syn_بلاڪ = wp->w_s;
    }
    changedtick = CHANGEDTICK(syn_buf);
    syn_win = wp;

    /*
     * Allocate syntax stack when needed.
     */
    syn_ڍير_alloc();
    إذا (syn_بلاڪ->b_sst_array == عدم)
	أرجع;		// out of memory
    syn_بلاڪ->b_sst_lasttick = display_tick;

    /*
     * If the state of the end of the previous line is useful, store it.
     */
    إذا (VALID_STATE(&current_state)
	    && current_lnum < lnum
	    && current_lnum < syn_buf->b_ml.ml_سٽ_ڳڻپ)
    {
	(فراغ)syn_پڄاڻي_سٽ(FALSE);
	إذا (!current_state_stored)
	{
	    ++current_lnum;
	    (فراغ)store_current_state();
	}

	/*
	 * If the current_lnum is now the same as "lnum", keep the current
	 * state (this happens very often!).  Otherwise invalidate
	 * current_state and figure it out below.
	 */
	إذا (current_lnum != lnum)
	    invalidate_current_state();
    }
    وإلا
	invalidate_current_state();

    /*
     * Try to synchronize from a saved state in b_sst_array[].
     * Only do this if lnum is not before and not to far beyond a saved state.
     */
    إذا (INVALID_STATE(&current_state) && syn_بلاڪ->b_sst_array != عدم)
    {
	// Find last valid saved state before start_lnum.
	FOR_ALL_SYNSTATES(syn_بلاڪ, p)
	{
	    إذا (p->sst_lnum > lnum)
		اكسر;
	    إذا (p->sst_lnum <= lnum && p->sst_change_lnum == 0)
	    {
		آخري_valid = p;
		إذا (p->sst_lnum >= lnum - syn_بلاڪ->b_syn_sync_minlines)
		    آخري_min_valid = p;
	    }
	}
	إذا (آخري_min_valid != عدم)
	    ڀريو_current_state(آخري_min_valid);
    }

    /*
     * If "lnum" is before or far beyond a line with a saved state, need to
     * re-synchronize.
     */
    إذا (INVALID_STATE(&current_state))
    {
	syn_sync(wp, lnum, آخري_valid);
	إذا (current_lnum == 1)
	    // First line is always valid, no matter "minlines".
	    پهريون_stored = 1;
	وإلا
	    // Need to parse "minlines" lines before state can be considered
	    // valid to store.
	    پهريون_stored = current_lnum + syn_بلاڪ->b_syn_sync_minlines;
    }
    وإلا
	پهريون_stored = current_lnum;

    /*
     * Advance from the sync point or saved state until the current line.
     * Save some entries for syncing with later on.
     */
    إذا (syn_بلاڪ->b_sst_len <= صفون)
	dist = 999999;
    وإلا
	dist = syn_buf->b_ml.ml_سٽ_ڳڻپ / (syn_بلاڪ->b_sst_len - صفون) + 1;
    طالما (current_lnum < lnum)
    {
	syn_شروعات_سٽ();
	(فراغ)syn_پڄاڻي_سٽ(FALSE);
	++current_lnum;

	// If we parsed at least "minlines" lines or started at a valid
	// state, the current state is considered valid.
	إذا (current_lnum >= پهريون_stored)
	{
	    // Check if the saved state entry is for the current line and is
	    // equal to the current state.  If so, then validate all saved
	    // states that depended on a change before the parsed line.
	    إذا (prev == عدم)
		prev = syn_ڍير_ڳوليو_داخلا(current_lnum - 1);
	    إذا (prev == عدم)
		sp = syn_بلاڪ->b_sst_پهريون;
	    وإلا
		sp = prev;
	    طالما (sp != عدم && sp->sst_lnum < current_lnum)
		sp = sp->sst_ايندڙ;
	    إذا (sp != عدم
		    && sp->sst_lnum == current_lnum
		    && syn_ڍير_equal(sp))
	    {
		parsed_lnum = current_lnum;
		prev = sp;
		طالما (sp != عدم && sp->sst_change_lnum <= parsed_lnum)
		{
		    إذا (sp->sst_lnum <= lnum)
			// valid state before desired line, use this one
			prev = sp;
		    وإلا إذا (sp->sst_change_lnum == 0)
			// past saved states depending on change, break here.
			اكسر;
		    sp->sst_change_lnum = 0;
		    sp = sp->sst_ايندڙ;
		}
		ڀريو_current_state(prev);
	    }
	    // Store the state at this line when it's the first one, the line
	    // where we start parsing, or some distance from the previously
	    // saved state.  But only when parsed at least 'minlines'.
	    وإلا إذا (prev == عدم
			|| current_lnum == lnum
			|| current_lnum >= prev->sst_lnum + dist)
		prev = store_current_state();
	}

	// This can take a long time: break when CTRL-C pressed.  The current
	// state will be wrong then.
	سٽ_breakcheck();
	إذا (got_int)
	{
	    current_lnum = lnum;
	    اكسر;
	}
    }

    syn_شروعات_سٽ();
}

/*
 * We cannot simply discard growarrays full of state_items or buf_states; we
 * have to manually release their extmatch pointers first.
 */
    ساكن فراغ
clear_syn_state(synstate_T *p)
{
    صحيح		i;
    garray_T	*gap;

    إذا (p->sst_stacksize > SST_FIX_حالتون)
    {
	gap = &(p->sst_union.sst_ga);
	لكل (i = 0; i < gap->ga_len; i++)
	    unref_extmatch(SYN_STATE_P(gap)[i].bs_extmatch);
	ga_clear(gap);
    }
    وإلا
    {
	لكل (i = 0; i < p->sst_stacksize; i++)
	    unref_extmatch(p->sst_union.sst_ڍير[i].bs_extmatch);
    }
}

/*
 * Cleanup the current_state stack.
 */
    ساكن فراغ
clear_current_state(فراغ)
{
    صحيح		i;
    stateitem_T	*sip;

    sip = (stateitem_T *)(current_state.ga_معلومات);
    لكل (i = 0; i < current_state.ga_len; i++)
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
    ساكن فراغ
syn_sync(
    win_T	*wp,
    linenr_T	شروعات_lnum,
    synstate_T	*آخري_valid)
{
    buf_T	*curbuf_save;
    win_T	*curwin_save;
    pos_T	cursor_save;
    صحيح		idx;
    linenr_T	lnum;
    linenr_T	پڇاڙي_lnum;
    linenr_T	break_lnum;
    صحيح		had_sync_نقطو;
    stateitem_T	*cur_si;
    synpat_T	*spp;
    صحيح		مليو_flags = 0;
    صحيح		مليو_ميلاپ_idx = 0;
    linenr_T	مليو_current_lnum = 0;
    صحيح		مليو_current_کانو = 0;
    lpos_T	مليو_m_endpos;
    colnr_T	prev_current_کانو;

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
    إذا (syn_بلاڪ->b_syn_sync_minlines > شروعات_lnum)
	شروعات_lnum = 1;
    وإلا
    {
	إذا (syn_بلاڪ->b_syn_sync_minlines == 1)
	    lnum = 1;
	وإلا إذا (syn_بلاڪ->b_syn_sync_minlines < 10)
	    lnum = syn_بلاڪ->b_syn_sync_minlines * 2;
	وإلا
	    lnum = syn_بلاڪ->b_syn_sync_minlines * 3 / 2;
	إذا (syn_بلاڪ->b_syn_sync_maxlines != 0
				     && lnum > syn_بلاڪ->b_syn_sync_maxlines)
	    lnum = syn_بلاڪ->b_syn_sync_maxlines;
	إذا (lnum >= شروعات_lnum)
	    شروعات_lnum = 1;
	وإلا
	    شروعات_lnum -= lnum;
    }
    current_lnum = شروعات_lnum;

    /*
     * 1. Search backwards for the end of a C-style comment.
     */
    إذا (syn_بلاڪ->b_syn_sync_flags & SF_CCOMMENT)
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
	لكل ( ; شروعات_lnum > 1; --شروعات_lnum)
	{
	    char_u	*l = ml_get(شروعات_lnum - 1);

	    إذا (*l == NUL || *(l + ml_get_len(شروعات_lnum - 1) - 1) != '\\')
		اكسر;
	}
	current_lnum = شروعات_lnum;

	// set cursor to start of search
	cursor_save = wp->w_cursor;
	wp->w_cursor.lnum = شروعات_lnum;
	wp->w_cursor.کانو = 0;

	/*
	 * If the line is inside a comment, need to find the syntax item that
	 * defines the comment.
	 * Restrict the search for the end of a comment to b_syn_sync_maxlines.
	 */
	إذا (ڳوليو_شروعات_comment((صحيح)syn_بلاڪ->b_syn_sync_maxlines) != عدم)
	{
	    لكل (idx = syn_بلاڪ->b_syn_patterns.ga_len; --idx >= 0; )
		إذا (SYN_شيون(syn_بلاڪ)[idx].sp_syn.id
						   == syn_بلاڪ->b_syn_sync_id
			&& SYN_شيون(syn_بلاڪ)[idx].sp_type == SPTYPE_شروعات)
		{
		    validate_current_state();
		    إذا (ڌڪو_current_state(idx) == OK)
			update_si_attr(current_state.ga_len - 1);
		    اكسر;
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
    وإلا إذا (syn_بلاڪ->b_syn_sync_flags & SF_ميلاپ)
    {
	إذا (syn_بلاڪ->b_syn_sync_maxlines != 0
			       && شروعات_lnum > syn_بلاڪ->b_syn_sync_maxlines)
	    break_lnum = شروعات_lnum - syn_بلاڪ->b_syn_sync_maxlines;
	وإلا
	    break_lnum = 0;

	مليو_m_endpos.lnum = 0;
	مليو_m_endpos.کانو = 0;
	پڇاڙي_lnum = شروعات_lnum;
	lnum = شروعات_lnum;
	طالما (--lnum > break_lnum)
	{
	    // This can take a long time: break when CTRL-C pressed.
	    سٽ_breakcheck();
	    إذا (got_int)
	    {
		invalidate_current_state();
		current_lnum = شروعات_lnum;
		اكسر;
	    }

	    // Check if we have run into a valid saved state stack now.
	    إذا (آخري_valid != عدم && lnum == آخري_valid->sst_lnum)
	    {
		ڀريو_current_state(آخري_valid);
		اكسر;
	    }

	    /*
	     * Check if the previous line has the line-continuation pattern.
	     */
	    إذا (lnum > 1 && syn_ميلاپ_linecont(lnum - 1))
		استمر;

	    /*
	     * Start with nothing on the state stack
	     */
	    validate_current_state();

	    لكل (current_lnum = lnum; current_lnum < پڇاڙي_lnum; ++current_lnum)
	    {
		syn_شروعات_سٽ();
		لكل (;;)
		{
		    had_sync_نقطو = syn_پڄاڻي_سٽ(TRUE);
		    /*
		     * When a sync point has been found, remember where, and
		     * continue to look for another one, further on in the line.
		     */
		    إذا (had_sync_نقطو && current_state.ga_len)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			إذا (cur_si->si_m_endpos.lnum > شروعات_lnum)
			{
			    // ignore match that goes to after where started
			    current_lnum = پڇاڙي_lnum;
			    اكسر;
			}
			إذا (cur_si->si_idx < 0)
			{
			    // Cannot happen?
			    مليو_flags = 0;
			    مليو_ميلاپ_idx = KEYWORD_IDX;
			}
			وإلا
			{
			    spp = &(SYN_شيون(syn_بلاڪ)[cur_si->si_idx]);
			    مليو_flags = spp->sp_flags;
			    مليو_ميلاپ_idx = spp->sp_sync_idx;
			}
			مليو_current_lnum = current_lnum;
			مليو_current_کانو = current_کانو;
			مليو_m_endpos = cur_si->si_m_endpos;
			/*
			 * Continue after the match (be aware of a zero-length
			 * match).
			 */
			إذا (مليو_m_endpos.lnum > current_lnum)
			{
			    current_lnum = مليو_m_endpos.lnum;
			    current_کانو = مليو_m_endpos.کانو;
			    إذا (current_lnum >= پڇاڙي_lnum)
				اكسر;
			}
			وإلا إذا (مليو_m_endpos.کانو > current_کانو)
			    current_کانو = مليو_m_endpos.کانو;
			وإلا
			    ++current_کانو;

			// syn_current_attr() will have skipped the check for
			// an item that ends here, need to do that now.  Be
			// careful not to go past the NUL.
			prev_current_کانو = current_کانو;
			إذا (syn_getcurline()[current_کانو] != NUL)
			    ++current_کانو;
			check_state_پڇاڙيون();
			current_کانو = prev_current_کانو;
		    }
		    وإلا
			اكسر;
		}
	    }

	    /*
	     * If a sync point was encountered, break here.
	     */
	    إذا (مليو_flags)
	    {
		/*
		 * Put the item that was specified by the sync point on the
		 * state stack.  If there was no item specified, make the
		 * state stack empty.
		 */
		clear_current_state();
		إذا (مليو_ميلاپ_idx >= 0
			&& ڌڪو_current_state(مليو_ميلاپ_idx) == OK)
		    update_si_attr(current_state.ga_len - 1);

		/*
		 * When using "grouphere", continue from the sync point
		 * match, until the end of the line.  Parsing starts at
		 * the next line.
		 * For "groupthere" the parsing starts at start_lnum.
		 */
		إذا (مليو_flags & HL_SYNC_هتي)
		{
		    current_lnum = مليو_m_endpos.lnum;
		    current_کانو = مليو_m_endpos.کانو;
		    إذا (current_state.ga_len)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			cur_si->si_h_startpos.lnum = مليو_current_lnum;
			cur_si->si_h_startpos.کانو = مليو_current_کانو;
			update_si_پڇاڙي(cur_si, (صحيح)current_کانو, TRUE);
			check_keepend();
		    }
		    (فراغ)syn_پڄاڻي_سٽ(FALSE);
		    ++current_lnum;
		}
		وإلا
		    current_lnum = شروعات_lnum;

		اكسر;
	    }

	    پڇاڙي_lnum = lnum;
	    invalidate_current_state();
	}

	// Ran into start of the file or exceeded maximum number of lines
	إذا (lnum <= break_lnum)
	{
	    invalidate_current_state();
	    current_lnum = break_lnum + 1;
	}
    }

    validate_current_state();
}

    ساكن فراغ
save_chartab(char_u *chartab)
{
    إذا (syn_بلاڪ->b_syn_isk == خالي_جايون_option)
	أرجع;

    mch_memmove(chartab, syn_buf->b_chartab, (size_t)32);
    mch_memmove(syn_buf->b_chartab, syn_win->w_s->b_syn_chartab, (size_t)32);
}

    ساكن فراغ
restore_chartab(char_u *chartab)
{
    إذا (syn_win->w_s->b_syn_isk != خالي_جايون_option)
	mch_memmove(syn_buf->b_chartab, chartab, (size_t)32);
}

/*
 * Return TRUE if the line-continuation pattern matches in line "lnum".
 */
    ساكن صحيح
syn_ميلاپ_linecont(linenr_T lnum)
{
    regmmatch_T regmatch;
    صحيح r;
    char_u	buf_chartab[32];  // chartab array for syn iskyeyword

    إذا (syn_بلاڪ->b_syn_linecont_prog == عدم)
	أرجع FALSE;

    // use syntax iskeyword option
    save_chartab(buf_chartab);
    regmatch.rmm_ic = syn_بلاڪ->b_syn_linecont_ic;
    regmatch.regprog = syn_بلاڪ->b_syn_linecont_prog;
    r = syn_regexec(&regmatch, lnum, (colnr_T)0,
	    IF_SYN_وقت(&syn_بلاڪ->b_syn_linecont_وقت));
    syn_بلاڪ->b_syn_linecont_prog = regmatch.regprog;
    restore_chartab(buf_chartab);
    أرجع r;
}

/*
 * Prepare the current state for the start of a line.
 */
    ساكن فراغ
syn_شروعات_سٽ(فراغ)
{
    current_ختم = FALSE;
    current_کانو = 0;

    /*
     * Need to update the end of a start/skip/end that continues from the
     * previous line and regions that have "keepend".
     */
    إذا (current_state.ga_len > 0)
    {
	syn_update_پڇاڙيون(TRUE);
	check_state_پڇاڙيون();
    }

    ايندڙ_ميلاپ_idx = -1;
    ++current_سٽ_id;
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
    ايندڙ_seqnr = 1;
#جيڪڏهن_پورو
}

/*
 * Check for items in the stack that need their end updated.
 * When "startofline" is TRUE the last item is always updated.
 * When "startofline" is FALSE the item with "keepend" is forcefully updated.
 */
    ساكن فراغ
syn_update_پڇاڙيون(صحيح startofline)
{
    stateitem_T	*cur_si;
    صحيح		i;
    صحيح		ڏٺل_keepend;

    إذا (startofline)
    {
	// Check for a match carried over from a previous line with a
	// contained region.  The match ends as soon as the region ends.
	لكل (i = 0; i < current_state.ga_len; ++i)
	{
	    cur_si = &CUR_STATE(i);
	    إذا (cur_si->si_idx >= 0
		    && (SYN_شيون(syn_بلاڪ)[cur_si->si_idx]).sp_type
							       == SPTYPE_ميلاپ
		    && cur_si->si_m_endpos.lnum < current_lnum)
	    {
		cur_si->si_flags |= HL_MATCHCONT;
		cur_si->si_m_endpos.lnum = 0;
		cur_si->si_m_endpos.کانو = 0;
		cur_si->si_h_endpos = cur_si->si_m_endpos;
		cur_si->si_پڇاڙيون = TRUE;
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
    إذا (keepend_level >= 0)
	لكل ( ; i > keepend_level; --i)
	    إذا (CUR_STATE(i).si_flags & HL_EXTEND)
		اكسر;

    ڏٺل_keepend = FALSE;
    لكل ( ; i < current_state.ga_len; ++i)
    {
	cur_si = &CUR_STATE(i);
	إذا ((cur_si->si_flags & HL_KEEPEND)
			    || (ڏٺل_keepend && !startofline)
			    || (i == current_state.ga_len - 1 && startofline))
	{
	    cur_si->si_h_startpos.کانو = 0;	// start highl. in col 0
	    cur_si->si_h_startpos.lnum = current_lnum;

	    إذا (!(cur_si->si_flags & HL_MATCHCONT))
		update_si_پڇاڙي(cur_si, (صحيح)current_کانو, !startofline);

	    إذا (!startofline && (cur_si->si_flags & HL_KEEPEND))
		ڏٺل_keepend = TRUE;
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

    ساكن فراغ
syn_ڍير_free_بلاڪ(synblock_T *بلاڪ)
{
    synstate_T	*p;

    // Syntax definitions may have changed: drop the in_id_list() cache.
    idl_cache_clear(بلاڪ);

    إذا (بلاڪ->b_sst_array == عدم)
	أرجع;

    FOR_ALL_SYNSTATES(بلاڪ, p)
	clear_syn_state(p);
    VIM_CLEAR(بلاڪ->b_sst_array);
    بلاڪ->b_sst_پهريون = عدم;
    بلاڪ->b_sst_ڳولا = عدم;
    بلاڪ->b_sst_len = 0;
}
/*
 * Free b_sst_array[] for buffer "buf".
 * Used when syntax items changed to force resyncing everywhere.
 */
    فراغ
syn_ڍير_free_all(synblock_T *بلاڪ)
{
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    win_T	*wp;
#جيڪڏهن_پورو

    syn_ڍير_free_بلاڪ(بلاڪ);

#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    // When using "syntax" fold method, must update all folds.
    FOR_ALL_WINDOWS(wp)
    {
	إذا (wp->w_s == بلاڪ && foldmethodIsSyntax(wp))
	    foldUpdateAll(wp);
    }
#جيڪڏهن_پورو
}

/*
 * Allocate the syntax state stack for syn_buf when needed.
 * If the number of entries in b_sst_array[] is much too big or a bit too
 * small, reallocate it.
 * Also used to allocate b_sst_array[] for the first time.
 */
    ساكن فراغ
syn_ڍير_alloc(فراغ)
{
    ڊگهو	len;
    synstate_T	*منزل, *from;
    synstate_T	*sstp;

    len = syn_buf->b_ml.ml_سٽ_ڳڻپ / SST_DIST + صفون * 2;
    إذا (len < SST_MIN_ENTRIES)
	len = SST_MIN_ENTRIES;
    وإلا إذا (len > SST_MAX_ENTRIES)
	len = SST_MAX_ENTRIES;
    إذا (syn_بلاڪ->b_sst_len > len * 2 || syn_بلاڪ->b_sst_len < len)
    {
	// Allocate 50% too much, to avoid reallocating too often.
	len = syn_buf->b_ml.ml_سٽ_ڳڻپ;
	len = (len + len / 2) / SST_DIST + صفون * 2;
	إذا (len < SST_MIN_ENTRIES)
	    len = SST_MIN_ENTRIES;
	وإلا إذا (len > SST_MAX_ENTRIES)
	    len = SST_MAX_ENTRIES;

	إذا (syn_بلاڪ->b_sst_array != عدم)
	{
	    // When shrinking the array, cleanup the existing stack.
	    // Make sure that all valid entries fit in the new array.
	    طالما (syn_بلاڪ->b_sst_len - syn_بلاڪ->b_sst_freecount + 2 > len
		    && syn_ڍير_cleanup())
		;
	    إذا (len < syn_بلاڪ->b_sst_len - syn_بلاڪ->b_sst_freecount + 2)
		len = syn_بلاڪ->b_sst_len - syn_بلاڪ->b_sst_freecount + 2;
	}

	sstp = ALLOC_CLEAR_MULT(synstate_T, len);
	إذا (sstp == عدم)	// out of memory!
	    أرجع;

	منزل = sstp - 1;
	إذا (syn_بلاڪ->b_sst_array != عدم)
	{
	    // Move the states from the old array to the new one.
	    لكل (from = syn_بلاڪ->b_sst_پهريون; from != عدم;
							from = from->sst_ايندڙ)
	    {
		++منزل;
		*منزل = *from;
		منزل->sst_ايندڙ = منزل + 1;
	    }
	}
	إذا (منزل != sstp - 1)
	{
	    منزل->sst_ايندڙ = عدم;
	    syn_بلاڪ->b_sst_پهريون = sstp;
	    syn_بلاڪ->b_sst_freecount = len - (صحيح)(منزل - sstp) - 1;
	}
	وإلا
	{
	    syn_بلاڪ->b_sst_پهريون = عدم;
	    syn_بلاڪ->b_sst_freecount = len;
	}

	// Create the list of free entries.
	syn_بلاڪ->b_sst_firstfree = منزل + 1;
	طالما (++منزل < sstp + len)
	    منزل->sst_ايندڙ = منزل + 1;
	(sstp + len - 1)->sst_ايندڙ = عدم;

	vim_free(syn_بلاڪ->b_sst_array);
	syn_بلاڪ->b_sst_array = sstp;
	syn_بلاڪ->b_sst_len = len;
	// The entries were moved to a new array, drop the stale pointer.
	syn_بلاڪ->b_sst_ڳولا = عدم;
    }
}

/*
 * Check for changes in a buffer to affect stored syntax states.  Uses the
 * b_mod_* fields.
 * Called from update_screen(), before screen is being updated, once for each
 * displayed buffer.
 */
    فراغ
syn_ڍير_apply_changes(buf_T *buf)
{
    win_T	*wp;

    syn_ڍير_apply_changes_بلاڪ(&buf->b_s, buf);

    FOR_ALL_WINDOWS(wp)
    {
	إذا ((wp->w_بفر == buf) && (wp->w_s != &buf->b_s))
	    syn_ڍير_apply_changes_بلاڪ(wp->w_s, buf);
    }
}

    ساكن فراغ
syn_ڍير_apply_changes_بلاڪ(synblock_T *بلاڪ, buf_T *buf)
{
    synstate_T	*p, *prev, *np;
    linenr_T	n;

    prev = عدم;
    لكل (p = بلاڪ->b_sst_پهريون; p != عدم; )
    {
	إذا (p->sst_lnum + بلاڪ->b_syn_sync_linebreaks > buf->b_mod_چوٽي)
	{
	    n = p->sst_lnum + buf->b_mod_xlines;
	    إذا (n <= buf->b_mod_bot)
	    {
		// this state is inside the changed area, remove it
		np = p->sst_ايندڙ;
		إذا (prev == عدم)
		    بلاڪ->b_sst_پهريون = np;
		وإلا
		    prev->sst_ايندڙ = np;
		syn_ڍير_free_داخلا(بلاڪ, p);
		p = np;
		استمر;
	    }
	    // This state is below the changed area.  Remember the line
	    // that needs to be parsed before this entry can be made valid
	    // again.
	    إذا (p->sst_change_lnum != 0 && p->sst_change_lnum > buf->b_mod_چوٽي)
	    {
		إذا (p->sst_change_lnum + buf->b_mod_xlines > buf->b_mod_چوٽي)
		    p->sst_change_lnum += buf->b_mod_xlines;
		وإلا
		    p->sst_change_lnum = buf->b_mod_چوٽي;
	    }
	    إذا (p->sst_change_lnum == 0
		    || p->sst_change_lnum < buf->b_mod_bot)
		p->sst_change_lnum = buf->b_mod_bot;

	    p->sst_lnum = n;
	}
	prev = p;
	p = p->sst_ايندڙ;
    }
}

/*
 * Reduce the number of entries in the state stack for syn_buf.
 * Returns TRUE if at least one entry was freed.
 */
    ساكن صحيح
syn_ڍير_cleanup(فراغ)
{
    synstate_T	*p, *prev;
    disptick_T	tick;
    صحيح		above;
    صحيح		dist;
    صحيح		retval = FALSE;

    إذا (syn_بلاڪ->b_sst_پهريون == عدم)
	أرجع retval;

    // Compute normal distance between non-displayed entries.
    إذا (syn_بلاڪ->b_sst_len <= صفون)
	dist = 999999;
    وإلا
	dist = syn_buf->b_ml.ml_سٽ_ڳڻپ / (syn_بلاڪ->b_sst_len - صفون) + 1;

    /*
     * Go through the list to find the "tick" for the oldest entry that can
     * be removed.  Set "above" when the "tick" for the oldest entry is above
     * "b_sst_lasttick" (the display tick wraps around).
     */
    tick = syn_بلاڪ->b_sst_lasttick;
    above = FALSE;
    prev = syn_بلاڪ->b_sst_پهريون;
    لكل (p = prev->sst_ايندڙ; p != عدم; prev = p, p = p->sst_ايندڙ)
    {
	إذا (prev->sst_lnum + dist > p->sst_lnum)
	{
	    إذا (p->sst_tick > syn_بلاڪ->b_sst_lasttick)
	    {
		إذا (!above || p->sst_tick < tick)
		    tick = p->sst_tick;
		above = TRUE;
	    }
	    وإلا إذا (!above && p->sst_tick < tick)
		tick = p->sst_tick;
	}
    }

    /*
     * Go through the list to make the entries for the oldest tick at an
     * interval of several lines.
     */
    prev = syn_بلاڪ->b_sst_پهريون;
    لكل (p = prev->sst_ايندڙ; p != عدم; prev = p, p = p->sst_ايندڙ)
    {
	إذا (p->sst_tick == tick && prev->sst_lnum + dist > p->sst_lnum)
	{
	    // Move this entry from used list to free list
	    prev->sst_ايندڙ = p->sst_ايندڙ;
	    syn_ڍير_free_داخلا(syn_بلاڪ, p);
	    p = prev;
	    retval = TRUE;
	}
    }
    أرجع retval;
}

/*
 * Free the allocated memory for a syn_state item.
 * Move the entry into the free list.
 */
    ساكن فراغ
syn_ڍير_free_داخلا(synblock_T *بلاڪ, synstate_T *p)
{
    إذا (بلاڪ->b_sst_ڳولا == p)
	بلاڪ->b_sst_ڳولا = عدم;
    clear_syn_state(p);
    p->sst_ايندڙ = بلاڪ->b_sst_firstfree;
    بلاڪ->b_sst_firstfree = p;
    ++بلاڪ->b_sst_freecount;
}

/*
 * Find an entry in the list of state stacks at or before "lnum".
 * Returns NULL when there is no entry or the first entry is after "lnum".
 */
    ساكن synstate_T *
syn_ڍير_ڳوليو_داخلا(linenr_T lnum)
{
    synstate_T	*p, *prev;

    prev = عدم;
    p = syn_بلاڪ->b_sst_پهريون;

    // The list is sorted by line number and lookups while parsing advance
    // monotonically, so resume from the last returned entry instead of
    // rescanning from the start whenever it is at or before "lnum".
    إذا (syn_بلاڪ->b_sst_ڳولا != عدم
				 && syn_بلاڪ->b_sst_ڳولا->sst_lnum <= lnum)
	p = syn_بلاڪ->b_sst_ڳولا;

    لكل ( ; p != عدم; prev = p, p = p->sst_ايندڙ)
    {
	إذا (p->sst_lnum == lnum)
	{
	    syn_بلاڪ->b_sst_ڳولا = p;
	    أرجع p;
	}
	إذا (p->sst_lnum > lnum)
	    اكسر;
    }
    إذا (prev != عدم)
	syn_بلاڪ->b_sst_ڳولا = prev;
    أرجع prev;
}

/*
 * Try saving the current state in b_sst_array[].
 * The current state must be valid for the start of the current_lnum line!
 */
    ساكن synstate_T *
store_current_state(فراغ)
{
    صحيح		i;
    synstate_T	*p;
    bufstate_T	*bp;
    stateitem_T	*cur_si;
    synstate_T	*sp = syn_ڍير_ڳوليو_داخلا(current_lnum);

    /*
     * If the current state contains a start or end pattern that continues
     * from the previous line, we can't use it.  Don't store it then.
     */
    لكل (i = current_state.ga_len - 1; i >= 0; --i)
    {
	cur_si = &CUR_STATE(i);
	إذا (cur_si->si_h_startpos.lnum >= current_lnum
		|| cur_si->si_m_endpos.lnum >= current_lnum
		|| cur_si->si_h_endpos.lnum >= current_lnum
		|| (cur_si->si_پڇاڙي_idx
		    && cur_si->si_eoe_pos.lnum >= current_lnum))
	    اكسر;
    }
    إذا (i >= 0)
    {
	إذا (sp != عدم)
	{
	    // find "sp" in the list and remove it
	    إذا (syn_بلاڪ->b_sst_پهريون == sp)
		// it's the first entry
		syn_بلاڪ->b_sst_پهريون = sp->sst_ايندڙ;
	    وإلا
	    {
		// find the entry just before this one to adjust sst_next
		FOR_ALL_SYNSTATES(syn_بلاڪ, p)
		    إذا (p->sst_ايندڙ == sp)
			اكسر;
		إذا (p != عدم)	// just in case
		    p->sst_ايندڙ = sp->sst_ايندڙ;
	    }
	    syn_ڍير_free_داخلا(syn_بلاڪ, sp);
	    sp = عدم;
	}
    }
    وإلا إذا (sp == عدم || sp->sst_lnum != current_lnum)
    {
	/*
	 * Add a new entry
	 */
	// If no free items, cleanup the array first.
	إذا (syn_بلاڪ->b_sst_freecount == 0)
	{
	    (فراغ)syn_ڍير_cleanup();
	    // "sp" may have been moved to the freelist now
	    sp = syn_ڍير_ڳوليو_داخلا(current_lnum);
	}
	// Still no free items?  Must be a strange problem...
	إذا (syn_بلاڪ->b_sst_freecount == 0)
	    sp = عدم;
	وإلا
	{
	    // Take the first item from the free list and put it in the used
	    // list, after *sp
	    p = syn_بلاڪ->b_sst_firstfree;
	    syn_بلاڪ->b_sst_firstfree = p->sst_ايندڙ;
	    --syn_بلاڪ->b_sst_freecount;
	    إذا (sp == عدم)
	    {
		// Insert in front of the list
		p->sst_ايندڙ = syn_بلاڪ->b_sst_پهريون;
		syn_بلاڪ->b_sst_پهريون = p;
	    }
	    وإلا
	    {
		// insert in list after *sp
		p->sst_ايندڙ = sp->sst_ايندڙ;
		sp->sst_ايندڙ = p;
	    }
	    sp = p;
	    sp->sst_stacksize = 0;
	    sp->sst_lnum = current_lnum;
	    // Resume the next forward lookup from the entry just stored.
	    syn_بلاڪ->b_sst_ڳولا = sp;
	}
    }
    إذا (sp != عدم)
    {
	// When overwriting an existing state stack, clear it first
	clear_syn_state(sp);
	sp->sst_stacksize = current_state.ga_len;
	إذا (current_state.ga_len > SST_FIX_حالتون)
	{
	    // Need to clear it, might be something remaining from when the
	    // length was less than SST_FIX_STATES.
	    ga_init2(&sp->sst_union.sst_ga, حجم(bufstate_T), 1);
	    إذا (ga_grow(&sp->sst_union.sst_ga, current_state.ga_len) == FAIL)
		sp->sst_stacksize = 0;
	    وإلا
		sp->sst_union.sst_ga.ga_len = current_state.ga_len;
	    bp = SYN_STATE_P(&(sp->sst_union.sst_ga));
	}
	وإلا
	    bp = sp->sst_union.sst_ڍير;
	لكل (i = 0; i < sp->sst_stacksize; ++i)
	{
	    bp[i].bs_idx = CUR_STATE(i).si_idx;
	    bp[i].bs_flags = CUR_STATE(i).si_flags;
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
	    bp[i].bs_seqnr = CUR_STATE(i).si_seqnr;
	    bp[i].bs_cchar = CUR_STATE(i).si_cchar;
#جيڪڏهن_پورو
	    bp[i].bs_extmatch = ref_extmatch(CUR_STATE(i).si_extmatch);
	}
	sp->sst_ايندڙ_flags = current_ايندڙ_flags;
	sp->sst_ايندڙ_list = current_ايندڙ_list;
	sp->sst_tick = display_tick;
	sp->sst_change_lnum = 0;
    }
    current_state_stored = TRUE;
    أرجع sp;
}

/*
 * Copy a state stack from "from" in b_sst_array[] to current_state;
 */
    ساكن فراغ
ڀريو_current_state(synstate_T *from)
{
    صحيح		i;
    bufstate_T	*bp;

    clear_current_state();
    validate_current_state();
    keepend_level = -1;
    إذا (from->sst_stacksize
	    && ga_grow(&current_state, from->sst_stacksize) == OK)
    {
	إذا (from->sst_stacksize > SST_FIX_حالتون)
	    bp = SYN_STATE_P(&(from->sst_union.sst_ga));
	وإلا
	    bp = from->sst_union.sst_ڍير;
	لكل (i = 0; i < from->sst_stacksize; ++i)
	{
	    CUR_STATE(i).si_idx = bp[i].bs_idx;
	    CUR_STATE(i).si_flags = bp[i].bs_flags;
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
	    CUR_STATE(i).si_seqnr = bp[i].bs_seqnr;
	    CUR_STATE(i).si_cchar = bp[i].bs_cchar;
#جيڪڏهن_پورو
	    CUR_STATE(i).si_extmatch = ref_extmatch(bp[i].bs_extmatch);
	    إذا (keepend_level < 0 && (CUR_STATE(i).si_flags & HL_KEEPEND))
		keepend_level = i;
	    CUR_STATE(i).si_پڇاڙيون = FALSE;
	    CUR_STATE(i).si_m_lnum = 0;
	    إذا (CUR_STATE(i).si_idx >= 0)
		CUR_STATE(i).si_ايندڙ_list =
		     (SYN_شيون(syn_بلاڪ)[CUR_STATE(i).si_idx]).sp_ايندڙ_list;
	    وإلا
		CUR_STATE(i).si_ايندڙ_list = عدم;
	    update_si_attr(i);
	}
	current_state.ga_len = from->sst_stacksize;
    }
    current_ايندڙ_list = from->sst_ايندڙ_list;
    current_ايندڙ_flags = from->sst_ايندڙ_flags;
    current_lnum = from->sst_lnum;
}

/*
 * Compare saved state stack "*sp" with the current state.
 * Return TRUE when they are equal.
 */
    ساكن صحيح
syn_ڍير_equal(synstate_T *sp)
{
    صحيح		i, j;
    bufstate_T	*bp;
    reg_extmatch_T	*six, *bsx;

    // First a quick check if the stacks have the same size end nextlist.
    إذا (sp->sst_stacksize != current_state.ga_len
	    || sp->sst_ايندڙ_list != current_ايندڙ_list)
	أرجع FALSE;

    // Need to compare all states on both stacks.
    إذا (sp->sst_stacksize > SST_FIX_حالتون)
	bp = SYN_STATE_P(&(sp->sst_union.sst_ga));
    وإلا
	bp = sp->sst_union.sst_ڍير;

    لكل (i = current_state.ga_len; --i >= 0; )
    {
	// If the item has another index the state is different.
	إذا (bp[i].bs_idx != CUR_STATE(i).si_idx)
	    اكسر;
	إذا (bp[i].bs_extmatch == CUR_STATE(i).si_extmatch)
	    استمر;
	// When the extmatch pointers are different, the strings in them can
	// still be the same.  Check if the extmatch references are equal.
	bsx = bp[i].bs_extmatch;
	six = CUR_STATE(i).si_extmatch;
	// If one of the extmatch pointers is NULL the states are different.
	إذا (bsx == عدم || six == عدم)
	    اكسر;
	لكل (j = 0; j < NSUBEXP; ++j)
	{
	    // Check each referenced match string. They must all be equal.
	    إذا (bsx->matches[j] != six->matches[j])
	    {
		// If the pointer is different it can still be the same text.
		// Compare the strings, ignore case when the start item has the
		// sp_ic flag set.
		إذا (bsx->matches[j] == عدم || six->matches[j] == عدم)
		    اكسر;
		إذا ((SYN_شيون(syn_بلاڪ)[CUR_STATE(i).si_idx]).sp_ic
			? MB_STRICMP(bsx->matches[j], six->matches[j]) != 0
			: STRCMP(bsx->matches[j], six->matches[j]) != 0)
		    اكسر;
	    }
	}
	إذا (j != NSUBEXP)
	    اكسر;
    }
    أرجع i < 0 ? TRUE : FALSE;
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
    فراغ
syntax_پڇاڙي_parsing(win_T *wp, linenr_T lnum)
{
    synstate_T	*sp;

    إذا (syn_بلاڪ != wp->w_s)
	أرجع;  // not the right window
    sp = syn_ڍير_ڳوليو_داخلا(lnum);
    إذا (sp != عدم && sp->sst_lnum < lnum)
	sp = sp->sst_ايندڙ;

    إذا (sp != عدم && sp->sst_change_lnum != 0)
	sp->sst_change_lnum = lnum;
}

/*
 * End of handling of the state stack.
 ****************************************/

    ساكن فراغ
invalidate_current_state(فراغ)
{
    clear_current_state();
    current_state.ga_itemsize = 0;	// mark current_state invalid
    current_ايندڙ_list = عدم;
    keepend_level = -1;
}

    ساكن فراغ
validate_current_state(فراغ)
{
    current_state.ga_itemsize = حجم(stateitem_T);
    current_state.ga_growsize = 3;
}

/*
 * Return TRUE if the syntax at start of lnum changed since last time.
 * This will only be called just after get_syntax_attr() for the previous
 * line, to check if the next line needs to be redrawn too.
 */
    صحيح
syntax_check_بدليو(linenr_T lnum)
{
    صحيح		retval = TRUE;
    synstate_T	*sp;

    /*
     * Check the state stack when:
     * - lnum is just below the previously syntaxed line.
     * - lnum is not before the lines with saved states.
     * - lnum is not past the lines with saved states.
     * - lnum is at or before the last changed line.
     */
    إذا (VALID_STATE(&current_state) && lnum == current_lnum + 1)
    {
	sp = syn_ڍير_ڳوليو_داخلا(lnum);
	إذا (sp != عدم && sp->sst_lnum == lnum)
	{
	    /*
	     * finish the previous line (needed when not all of the line was
	     * drawn)
	     */
	    (فراغ)syn_پڄاڻي_سٽ(FALSE);

	    /*
	     * Compare the current state with the previously saved state of
	     * the line.
	     */
	    إذا (syn_ڍير_equal(sp))
		retval = FALSE;

	    /*
	     * Store the current state in b_sst_array[] for later use.
	     */
	    ++current_lnum;
	    (فراغ)store_current_state();
	}
    }

    أرجع retval;
}

/*
 * Finish the current line.
 * This doesn't return any attributes, it only gets the state at the end of
 * the line.  It can start anywhere in the line, as long as the current state
 * is valid.
 */
    ساكن صحيح
syn_پڄاڻي_سٽ(
    صحيح	    syncing)		// called for syncing
{
    stateitem_T	*cur_si;
    colnr_T	prev_current_کانو;

    طالما (!current_ختم)
    {
	(فراغ)syn_current_attr(syncing, FALSE, عدم, FALSE);
	/*
	 * When syncing, and found some item, need to check the item.
	 */
	إذا (syncing && current_state.ga_len)
	{
	    /*
	     * Check for match with sync item.
	     */
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	    إذا (cur_si->si_idx >= 0
		    && (SYN_شيون(syn_بلاڪ)[cur_si->si_idx].sp_flags
					  & (HL_SYNC_هتي|HL_SYNC_THERE)))
		أرجع TRUE;

	    // syn_current_attr() will have skipped the check for an item
	    // that ends here, need to do that now.  Be careful not to go
	    // past the NUL.
	    prev_current_کانو = current_کانو;
	    إذا (syn_getcurline()[current_کانو] != NUL)
		++current_کانو;
	    check_state_پڇاڙيون();
	    current_کانو = prev_current_کانو;
	}
	++current_کانو;
    }
    أرجع FALSE;
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
    صحيح
get_syntax_attr(
    colnr_T	کانو,
    صحيح		*can_spell,
    صحيح		keep_state)	// keep state of char at "col"
{
    صحيح	    attr = 0;

    إذا (can_spell != عدم)
	// Default: Only do spelling when there is no @Spell cluster or when
	// ":syn spell toplevel" was used.
	*can_spell = syn_بلاڪ->b_syn_spell == SYNSPL_DEFAULT
		    ? (syn_بلاڪ->b_spell_cluster_id == 0)
		    : (syn_بلاڪ->b_syn_spell == SYNSPL_چوٽي);

    // check for out of memory situation
    إذا (syn_بلاڪ->b_sst_array == عدم)
	أرجع 0;

    // After 'synmaxcol' the attribute is always zero.
    إذا (syn_buf->b_p_smc > 0 && کانو >= (colnr_T)syn_buf->b_p_smc)
    {
	clear_current_state();
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	current_id = 0;
	current_trans_id = 0;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
	current_flags = 0;
	current_seqnr = 0;
#جيڪڏهن_پورو
	أرجع 0;
    }

    // Make sure current_state is valid
    إذا (INVALID_STATE(&current_state))
	validate_current_state();

    /*
     * Skip from the current column to "col", get the attributes for "col".
     */
    طالما (current_کانو <= کانو)
    {
	attr = syn_current_attr(FALSE, TRUE, can_spell,
				     current_کانو == کانو ? keep_state : FALSE);
	++current_کانو;
    }

    أرجع attr;
}

/*
 * Get syntax attributes for current_lnum, current_col.
 */
    ساكن صحيح
syn_current_attr(
    صحيح		syncing,		// When 1: called for syncing
    صحيح		displaying,		// result will be displayed
    صحيح		*can_spell,		// return: do spell checking
    صحيح		keep_state)		// keep syntax stack afterwards
{
    صحيح		syn_id;
    lpos_T	endpos;		// was: char_u *endp;
    lpos_T	hl_startpos;	// was: int hl_startcol;
    lpos_T	hl_endpos;
    lpos_T	eos_pos;	// end-of-start match (start region)
    lpos_T	eoe_pos;	// end-of-end pattern
    صحيح		پڇاڙي_idx;	// group ID for end pattern
    صحيح		idx;
    synpat_T	*spp;
    stateitem_T	*cur_si, *sip = عدم;
    صحيح		startcol;
    صحيح		endcol;
    ڊگهو	flags;
    صحيح		cchar;
    ننڍو	*ايندڙ_list;
    صحيح		مليو_ميلاپ;		    // found usable match
    ساكن صحيح	try_ايندڙ_ڪالم = FALSE;    // must try in next col
    صحيح		do_keywords;
    regmmatch_T	regmatch;
    lpos_T	pos;
    صحيح		lc_کانو;
    reg_extmatch_T *cur_extmatch = عدم;
    char_u	buf_chartab[32];  // chartab array for syn iskyeyword
    char_u	*سٽ;		// current line.  NOTE: becomes invalid after
				// looking for a pattern match!

    // variables for zero-width matches that have a "nextgroup" argument
    صحيح		keep_ايندڙ_list;
    صحيح		zero_ويڪر_ايندڙ_list = FALSE;
    garray_T	zero_ويڪر_ايندڙ_ga;

    /*
     * No character, no attributes!  Past end of line?
     * Do try matching with an empty line (could be the start of a region).
     */
    سٽ = syn_getcurline();
    إذا (سٽ[current_کانو] == NUL && current_کانو != 0)
    {
	/*
	 * If we found a match after the last column, use it.
	 */
	إذا (ايندڙ_ميلاپ_idx >= 0 && ايندڙ_ميلاپ_کانو >= (صحيح)current_کانو
						  && ايندڙ_ميلاپ_کانو != MAXCOL)
	    (فراغ)ڌڪو_ايندڙ_ميلاپ(عدم);

	current_ختم = TRUE;
	current_state_stored = FALSE;
	أرجع 0;
    }

    // if the current or next character is NUL, we will finish the line now
    إذا (سٽ[current_کانو] == NUL || سٽ[current_کانو + 1] == NUL)
    {
	current_ختم = TRUE;
	current_state_stored = FALSE;
    }

    /*
     * When in the previous column there was a match but it could not be used
     * (empty match or already matched in this column) need to try again in
     * the next column.
     */
    إذا (try_ايندڙ_ڪالم)
    {
	ايندڙ_ميلاپ_idx = -1;
	try_ايندڙ_ڪالم = FALSE;
    }

    // Only check for keywords when not syncing and there are some.
    do_keywords = !syncing
		    && (syn_بلاڪ->b_keywtab.ht_استعمال_ٿيل > 0
			    || syn_بلاڪ->b_keywtab_ic.ht_استعمال_ٿيل > 0);

    // Init the list of zero-width matches with a nextlist.  This is used to
    // avoid matching the same item in the same position twice.
    ga_init2(&zero_ويڪر_ايندڙ_ga, حجم(صحيح), 10);

    // use syntax iskeyword option
    save_chartab(buf_chartab);

    /*
     * Repeat matching keywords and patterns, to find contained items at the
     * same column.  This stops when there are no extra matches at the current
     * column.
     */
    افعل
    {
	مليو_ميلاپ = FALSE;
	keep_ايندڙ_list = FALSE;
	syn_id = 0;


	/*
	 * 1. Check for a current state.
	 *    Only when there is no current state, or if the current state may
	 *    contain other things, we need to check for keywords and patterns.
	 *    Always need to check for contained items if some item has the
	 *    "containedin" argument (takes extra time!).
	 */
	إذا (current_state.ga_len)
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	وإلا
	    cur_si = عدم;

	إذا (syn_بلاڪ->b_syn_containedin || cur_si == عدم
					      || cur_si->si_cont_list != عدم)
	{
	    /*
	     * 2. Check for keywords, if on a keyword char after a non-keyword
	     *	  char.  Don't do this when syncing.
	     */
	    إذا (do_keywords)
	    {
	      سٽ = syn_getcurline();
	      إذا (vim_iswordp_buf(سٽ + current_کانو, syn_buf)
		      && (current_کانو == 0
			  || !vim_iswordp_buf(سٽ + current_کانو - 1
			      - (has_mbyte
				  ? (*mb_سر_off)(سٽ, سٽ + current_کانو - 1)
				  : 0) , syn_buf)))
	      {
		syn_id = check_keyword_id(سٽ, (صحيح)current_کانو,
					 &endcol, &flags, &ايندڙ_list, cur_si,
					 &cchar);
		إذا (syn_id != 0)
		{
		    إذا (ڌڪو_current_state(KEYWORD_IDX) == OK)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			cur_si->si_m_startcol = current_کانو;
			cur_si->si_h_startpos.lnum = current_lnum;
			cur_si->si_h_startpos.کانو = 0;	// starts right away
			cur_si->si_m_endpos.lnum = current_lnum;
			cur_si->si_m_endpos.کانو = endcol;
			cur_si->si_h_endpos.lnum = current_lnum;
			cur_si->si_h_endpos.کانو = endcol;
			cur_si->si_پڇاڙيون = TRUE;
			cur_si->si_پڇاڙي_idx = 0;
			cur_si->si_flags = flags;
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
			cur_si->si_seqnr = ايندڙ_seqnr++;
			cur_si->si_cchar = cchar;
			إذا (current_state.ga_len > 1)
			    cur_si->si_flags |=
				  CUR_STATE(current_state.ga_len - 2).si_flags
								 & HL_CONCEAL;
#جيڪڏهن_پورو
			cur_si->si_id = syn_id;
			cur_si->si_trans_id = syn_id;
			إذا (flags & HL_TRANSP)
			{
			    إذا (current_state.ga_len < 2)
			    {
				cur_si->si_attr = 0;
				cur_si->si_trans_id = 0;
			    }
			    وإلا
			    {
				cur_si->si_attr = CUR_STATE(
					current_state.ga_len - 2).si_attr;
				cur_si->si_trans_id = CUR_STATE(
					current_state.ga_len - 2).si_trans_id;
			    }
			}
			وإلا
			    cur_si->si_attr = syn_id2attr(syn_id);
			cur_si->si_cont_list = عدم;
			cur_si->si_ايندڙ_list = ايندڙ_list;
			check_keepend();
		    }
		    وإلا
			vim_free(ايندڙ_list);
		}
	      }
	    }

	    /*
	     * 3. Check for patterns (only if no keyword found).
	     */
	    إذا (syn_id == 0 && syn_بلاڪ->b_syn_patterns.ga_len)
	    {
		/*
		 * If we didn't check for a match yet, or we are past it, check
		 * for any match with a pattern.
		 */
		إذا (ايندڙ_ميلاپ_idx < 0 || ايندڙ_ميلاپ_کانو < (صحيح)current_کانو)
		{
		    /*
		     * Check all relevant patterns for a match at this
		     * position.  This is complicated, because matching with a
		     * pattern takes quite a bit of time, thus we want to
		     * avoid doing it when it's not needed.
		     */
		    ايندڙ_ميلاپ_idx = 0;		// no match in this line yet
		    ايندڙ_ميلاپ_کانو = MAXCOL;
		    لكل (idx = syn_بلاڪ->b_syn_patterns.ga_len; --idx >= 0; )
		    {
			spp = &(SYN_شيون(syn_بلاڪ)[idx]);
			إذا (	   spp->sp_syncing == syncing
				&& (displaying || !(spp->sp_flags & HL_DISPLAY))
				&& (spp->sp_type == SPTYPE_ميلاپ
				    || spp->sp_type == SPTYPE_شروعات)
				&& (current_ايندڙ_list != عدم
				    ? اندريون_id_list(عدم, current_ايندڙ_list,
							      &spp->sp_syn, 0)
				    : (cur_si == عدم
					? !(spp->sp_flags & HL_CONTAINED)
					: اندريون_id_list(cur_si,
					    cur_si->si_cont_list, &spp->sp_syn,
					    spp->sp_flags))))
			{
			    صحيح r;

			    // If we already tried matching in this line, and
			    // there isn't a match before next_match_col, skip
			    // this item.
			    إذا (spp->sp_سٽ_id == current_سٽ_id
				    && spp->sp_startcol >= ايندڙ_ميلاپ_کانو)
				استمر;
			    spp->sp_سٽ_id = current_سٽ_id;

			    lc_کانو = current_کانو - spp->sp_offsets[SPO_LC_OFF];
			    إذا (lc_کانو < 0)
				lc_کانو = 0;

			    regmatch.rmm_ic = spp->sp_ic;
			    regmatch.regprog = spp->sp_prog;
			    r = syn_regexec(&regmatch,
					     current_lnum,
					     (colnr_T)lc_کانو,
					     IF_SYN_وقت(&spp->sp_وقت));
			    spp->sp_prog = regmatch.regprog;
			    إذا (!r)
			    {
				// no match in this line, try another one
				spp->sp_startcol = MAXCOL;
				استمر;
			    }

			    /*
			     * Compute the first column of the match.
			     */
			    syn_جوڙيو_شروعات_off(&pos, &regmatch,
							 spp, SPO_MS_OFF, -1);
			    إذا (pos.lnum > current_lnum)
			    {
				// must have used end of match in a next line,
				// we can't handle that
				spp->sp_startcol = MAXCOL;
				استمر;
			    }
			    startcol = pos.کانو;

			    // remember the next column where this pattern
			    // matches in the current line
			    spp->sp_startcol = startcol;

			    /*
			     * If a previously found match starts at a lower
			     * column number, don't use this one.
			     */
			    إذا (startcol >= ايندڙ_ميلاپ_کانو)
				استمر;

			    /*
			     * If we matched this pattern at this position
			     * before, skip it.  Must retry in the next
			     * column, because it may match from there.
			     */
			    إذا (did_ميلاپ_already(idx, &zero_ويڪر_ايندڙ_ga))
			    {
				try_ايندڙ_ڪالم = TRUE;
				استمر;
			    }

			    endpos.lnum = regmatch.endpos[0].lnum;
			    endpos.کانو = regmatch.endpos[0].کانو;

			    // Compute the highlight start.
			    syn_جوڙيو_شروعات_off(&hl_startpos, &regmatch,
							 spp, SPO_HS_OFF, -1);

			    // Compute the region start.
			    // Default is to use the end of the match.
			    syn_جوڙيو_پڇاڙي_off(&eos_pos, &regmatch,
							 spp, SPO_RS_OFF, 0);

			    /*
			     * Grab the external submatches before they get
			     * overwritten.  Reference count doesn't change.
			     */
			    unref_extmatch(cur_extmatch);
			    cur_extmatch = re_extmatch_ٻاهريون;
			    re_extmatch_ٻاهريون = عدم;

			    flags = 0;
			    eoe_pos.lnum = 0;	// avoid warning
			    eoe_pos.کانو = 0;
			    پڇاڙي_idx = 0;
			    hl_endpos.lnum = 0;

			    /*
			     * For a "oneline" the end must be found in the
			     * same line too.  Search for it after the end of
			     * the match with the start pattern.  Set the
			     * resulting end positions at the same time.
			     */
			    إذا (spp->sp_type == SPTYPE_شروعات
					      && (spp->sp_flags & HL_ONELINE))
			    {
				lpos_T	startpos;

				startpos = endpos;
				ڳوليو_endpos(idx, &startpos, &endpos, &hl_endpos,
				    &flags, &eoe_pos, &پڇاڙي_idx, cur_extmatch);
				إذا (endpos.lnum == 0)
				    استمر;	    // not found
			    }

			    /*
			     * For a "match" the size must be > 0 after the
			     * end offset needs has been added.  Except when
			     * syncing.
			     */
			    وإلا إذا (spp->sp_type == SPTYPE_ميلاپ)
			    {
				syn_جوڙيو_پڇاڙي_off(&hl_endpos, &regmatch, spp,
							       SPO_HE_OFF, 0);
				syn_جوڙيو_پڇاڙي_off(&endpos, &regmatch, spp,
							       SPO_ME_OFF, 0);
				إذا (endpos.lnum == current_lnum
				      && (صحيح)endpos.کانو + syncing < startcol)
				{
				    /*
				     * If an empty string is matched, may need
				     * to try matching again at next column.
				     */
				    إذا (regmatch.startpos[0].کانو
						    == regmatch.endpos[0].کانو)
					try_ايندڙ_ڪالم = TRUE;
				    استمر;
				}
			    }

			    /*
			     * keep the best match so far in next_match_*
			     */
			    // Highlighting must start after startpos and end
			    // before endpos.
			    إذا (hl_startpos.lnum == current_lnum
					   && (صحيح)hl_startpos.کانو < startcol)
				hl_startpos.کانو = startcol;
			    حد_بندي_pos_zero(&hl_endpos, &endpos);

			    ايندڙ_ميلاپ_idx = idx;
			    ايندڙ_ميلاپ_کانو = startcol;
			    ايندڙ_ميلاپ_m_endpos = endpos;
			    ايندڙ_ميلاپ_h_endpos = hl_endpos;
			    ايندڙ_ميلاپ_h_startpos = hl_startpos;
			    ايندڙ_ميلاپ_flags = flags;
			    ايندڙ_ميلاپ_eos_pos = eos_pos;
			    ايندڙ_ميلاپ_eoe_pos = eoe_pos;
			    ايندڙ_ميلاپ_پڇاڙي_idx = پڇاڙي_idx;
			    unref_extmatch(ايندڙ_ميلاپ_extmatch);
			    ايندڙ_ميلاپ_extmatch = cur_extmatch;
			    cur_extmatch = عدم;
			}
		    }
		}

		/*
		 * If we found a match at the current column, use it.
		 */
		إذا (ايندڙ_ميلاپ_idx >= 0 && ايندڙ_ميلاپ_کانو == (صحيح)current_کانو)
		{
		    synpat_T	*lspp;

		    // When a zero-width item matched which has a nextgroup,
		    // don't push the item but set nextgroup.
		    lspp = &(SYN_شيون(syn_بلاڪ)[ايندڙ_ميلاپ_idx]);
		    إذا (ايندڙ_ميلاپ_m_endpos.lnum == current_lnum
			    && ايندڙ_ميلاپ_m_endpos.کانو == current_کانو
			    && lspp->sp_ايندڙ_list != عدم)
		    {
			current_ايندڙ_list = lspp->sp_ايندڙ_list;
			current_ايندڙ_flags = lspp->sp_flags;
			keep_ايندڙ_list = TRUE;
			zero_ويڪر_ايندڙ_list = TRUE;

			// Add the index to a list, so that we can check
			// later that we don't match it again (and cause an
			// endless loop).
			إذا (ga_grow(&zero_ويڪر_ايندڙ_ga, 1) == OK)
			{
			    ((صحيح *)(zero_ويڪر_ايندڙ_ga.ga_معلومات))
				[zero_ويڪر_ايندڙ_ga.ga_len++] = ايندڙ_ميلاپ_idx;
			}
			ايندڙ_ميلاپ_idx = -1;
		    }
		    وإلا
			cur_si = ڌڪو_ايندڙ_ميلاپ(cur_si);
		    مليو_ميلاپ = TRUE;
		}
	    }
	}

	/*
	 * Handle searching for nextgroup match.
	 */
	إذا (current_ايندڙ_list != عدم && !keep_ايندڙ_list)
	{
	    /*
	     * If a nextgroup was not found, continue looking for one if:
	     * - this is an empty line and the "skipempty" option was given
	     * - we are on white space and the "skipwhite" option was given
	     */
	    إذا (!مليو_ميلاپ)
	    {
		سٽ = syn_getcurline();
		إذا (((current_ايندڙ_flags & HL_SKIPWHITE)
			    && VIM_ISWHITE(سٽ[current_کانو]))
			|| ((current_ايندڙ_flags & HL_SKIPEMPTY)
			    && *سٽ == NUL))
		    اكسر;
	    }

	    /*
	     * If a nextgroup was found: Use it, and continue looking for
	     * contained matches.
	     * If a nextgroup was not found: Continue looking for a normal
	     * match.
	     * When did set current_next_list for a zero-width item and no
	     * match was found don't loop (would get stuck).
	     */
	    current_ايندڙ_list = عدم;
	    ايندڙ_ميلاپ_idx = -1;
	    إذا (!zero_ويڪر_ايندڙ_list)
		مليو_ميلاپ = TRUE;
	}

    } طالما (مليو_ميلاپ);

    restore_chartab(buf_chartab);

    /*
     * Use attributes from the current state, if within its highlighting.
     * If not, use attributes from the current-but-one state, etc.
     */
    current_attr = 0;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    current_id = 0;
    current_trans_id = 0;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
    current_flags = 0;
    current_seqnr = 0;
#جيڪڏهن_پورو
    إذا (cur_si != عدم)
    {
#جيڪڏهن_وصف_نه_ٿيل FEAT_EVAL
	صحيح	current_trans_id = 0;
#جيڪڏهن_پورو
	لكل (idx = current_state.ga_len - 1; idx >= 0; --idx)
	{
	    sip = &CUR_STATE(idx);
	    إذا ((current_lnum > sip->si_h_startpos.lnum
			|| (current_lnum == sip->si_h_startpos.lnum
			    && current_کانو >= sip->si_h_startpos.کانو))
		    && (sip->si_h_endpos.lnum == 0
			|| current_lnum < sip->si_h_endpos.lnum
			|| (current_lnum == sip->si_h_endpos.lnum
			    && current_کانو < sip->si_h_endpos.کانو)))
	    {
		current_attr = sip->si_attr;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
		current_id = sip->si_id;
#جيڪڏهن_پورو
		current_trans_id = sip->si_trans_id;
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
		current_flags = sip->si_flags;
		current_seqnr = sip->si_seqnr;
		current_sub_char = sip->si_cchar;
#جيڪڏهن_پورو
		اكسر;
	    }
	}

	إذا (can_spell != عدم)
	{
	    بنية sp_syn   sps;

	    /*
	     * set "can_spell" to TRUE if spell checking is supposed to be
	     * done in the current item.
	     */
	    إذا (syn_بلاڪ->b_spell_cluster_id == 0)
	    {
		// There is no @Spell cluster: Do spelling for items without
		// @NoSpell cluster.
		إذا (syn_بلاڪ->b_nospell_cluster_id == 0
						     || current_trans_id == 0)
		    *can_spell = (syn_بلاڪ->b_syn_spell != SYNSPL_NOTOP);
		وإلا
		{
		    sps.inc_ٽيگ = 0;
		    sps.id = syn_بلاڪ->b_nospell_cluster_id;
		    sps.cont_اندريون_list = عدم;
		    *can_spell = !اندريون_id_list(sip, sip->si_cont_list, &sps, 0);
		}
	    }
	    وإلا
	    {
		// The @Spell cluster is defined: Do spelling in items with
		// the @Spell cluster.  But not when @NoSpell is also there.
		// At the toplevel only spell check when ":syn spell toplevel"
		// was used.
		إذا (current_trans_id == 0)
		    *can_spell = (syn_بلاڪ->b_syn_spell == SYNSPL_چوٽي);
		وإلا
		{
		    sps.inc_ٽيگ = 0;
		    sps.id = syn_بلاڪ->b_spell_cluster_id;
		    sps.cont_اندريون_list = عدم;
		    *can_spell = اندريون_id_list(sip, sip->si_cont_list, &sps, 0);

		    إذا (syn_بلاڪ->b_nospell_cluster_id != 0)
		    {
			sps.id = syn_بلاڪ->b_nospell_cluster_id;
			إذا (اندريون_id_list(sip, sip->si_cont_list, &sps, 0))
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
	إذا (!syncing && !keep_state)
	{
	    check_state_پڇاڙيون();
	    إذا (current_state.ga_len > 0
				      && syn_getcurline()[current_کانو] != NUL)
	    {
		++current_کانو;
		check_state_پڇاڙيون();
		--current_کانو;
	    }
	}
    }
    وإلا إذا (can_spell != عدم)
	// Default: Only do spelling when there is no @Spell cluster or when
	// ":syn spell toplevel" was used.
	*can_spell = syn_بلاڪ->b_syn_spell == SYNSPL_DEFAULT
		    ? (syn_بلاڪ->b_spell_cluster_id == 0)
		    : (syn_بلاڪ->b_syn_spell == SYNSPL_چوٽي);

    // nextgroup ends at end of line, unless "skipnl" or "skipempty" present
    إذا (current_ايندڙ_list != عدم
	    && (سٽ = syn_getcurline())[current_کانو] != NUL
	    && سٽ[current_کانو + 1] == NUL
	    && !(current_ايندڙ_flags & (HL_SKIPNL | HL_SKIPEMPTY)))
	current_ايندڙ_list = عدم;

    إذا (zero_ويڪر_ايندڙ_ga.ga_len > 0)
	ga_clear(&zero_ويڪر_ايندڙ_ga);

    // No longer need external matches.  But keep next_match_extmatch.
    unref_extmatch(re_extmatch_ٻاهريون);
    re_extmatch_ٻاهريون = عدم;
    unref_extmatch(cur_extmatch);

    أرجع current_attr;
}


/*
 * Check if we already matched pattern "idx" at the current column.
 */
    ساكن صحيح
did_ميلاپ_already(صحيح idx, garray_T *gap)
{
    صحيح		i;

    لكل (i = current_state.ga_len; --i >= 0; )
	إذا (CUR_STATE(i).si_m_startcol == (صحيح)current_کانو
		&& CUR_STATE(i).si_m_lnum == (صحيح)current_lnum
		&& CUR_STATE(i).si_idx == idx)
	    أرجع TRUE;

    // Zero-width matches with a nextgroup argument are not put on the syntax
    // stack, and can only be matched once anyway.
    لكل (i = gap->ga_len; --i >= 0; )
	إذا (((صحيح *)(gap->ga_معلومات))[i] == idx)
	    أرجع TRUE;

    أرجع FALSE;
}

/*
 * Push the next match onto the stack.
 */
    ساكن stateitem_T *
ڌڪو_ايندڙ_ميلاپ(stateitem_T *cur_si)
{
    synpat_T	*spp;
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
    صحيح		 save_flags;
#جيڪڏهن_پورو

    spp = &(SYN_شيون(syn_بلاڪ)[ايندڙ_ميلاپ_idx]);

    /*
     * Push the item in current_state stack;
     */
    إذا (ڌڪو_current_state(ايندڙ_ميلاپ_idx) == OK)
    {
	/*
	 * If it's a start-skip-end type that crosses lines, figure out how
	 * much it continues in this line.  Otherwise just fill in the length.
	 */
	cur_si = &CUR_STATE(current_state.ga_len - 1);
	cur_si->si_h_startpos = ايندڙ_ميلاپ_h_startpos;
	cur_si->si_m_startcol = current_کانو;
	cur_si->si_m_lnum = current_lnum;
	cur_si->si_flags = spp->sp_flags;
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
	cur_si->si_seqnr = ايندڙ_seqnr++;
	cur_si->si_cchar = spp->sp_cchar;
	إذا (current_state.ga_len > 1)
	    cur_si->si_flags |=
		    CUR_STATE(current_state.ga_len - 2).si_flags & HL_CONCEAL;
#جيڪڏهن_پورو
	cur_si->si_ايندڙ_list = spp->sp_ايندڙ_list;
	cur_si->si_extmatch = ref_extmatch(ايندڙ_ميلاپ_extmatch);
	إذا (spp->sp_type == SPTYPE_شروعات && !(spp->sp_flags & HL_ONELINE))
	{
	    // Try to find the end pattern in the current line
	    update_si_پڇاڙي(cur_si, (صحيح)(ايندڙ_ميلاپ_m_endpos.کانو), TRUE);
	    check_keepend();
	}
	وإلا
	{
	    cur_si->si_m_endpos = ايندڙ_ميلاپ_m_endpos;
	    cur_si->si_h_endpos = ايندڙ_ميلاپ_h_endpos;
	    cur_si->si_پڇاڙيون = TRUE;
	    cur_si->si_flags |= ايندڙ_ميلاپ_flags;
	    cur_si->si_eoe_pos = ايندڙ_ميلاپ_eoe_pos;
	    cur_si->si_پڇاڙي_idx = ايندڙ_ميلاپ_پڇاڙي_idx;
	}
	إذا (keepend_level < 0 && (cur_si->si_flags & HL_KEEPEND))
	    keepend_level = current_state.ga_len - 1;
	check_keepend();
	update_si_attr(current_state.ga_len - 1);

#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
	save_flags = cur_si->si_flags & (HL_CONCEAL | HL_CONCEALENDS);
#جيڪڏهن_پورو
	/*
	 * If the start pattern has another highlight group, push another item
	 * on the stack for the start pattern.
	 */
	إذا (	   spp->sp_type == SPTYPE_شروعات
		&& spp->sp_syn_ميلاپ_id != 0
		&& ڌڪو_current_state(ايندڙ_ميلاپ_idx) == OK)
	{
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	    cur_si->si_h_startpos = ايندڙ_ميلاپ_h_startpos;
	    cur_si->si_m_startcol = current_کانو;
	    cur_si->si_m_lnum = current_lnum;
	    cur_si->si_m_endpos = ايندڙ_ميلاپ_eos_pos;
	    cur_si->si_h_endpos = ايندڙ_ميلاپ_eos_pos;
	    cur_si->si_پڇاڙيون = TRUE;
	    cur_si->si_پڇاڙي_idx = 0;
	    cur_si->si_flags = HL_ميلاپ;
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
	    cur_si->si_seqnr = ايندڙ_seqnr++;
	    cur_si->si_flags |= save_flags;
	    إذا (cur_si->si_flags & HL_CONCEALENDS)
		cur_si->si_flags |= HL_CONCEAL;
#جيڪڏهن_پورو
	    cur_si->si_ايندڙ_list = عدم;
	    check_keepend();
	    update_si_attr(current_state.ga_len - 1);
	}
    }

    ايندڙ_ميلاپ_idx = -1;	// try other match next time

    أرجع cur_si;
}

/*
 * Check for end of current state (and the states before it).
 */
    ساكن فراغ
check_state_پڇاڙيون(فراغ)
{
    stateitem_T	*cur_si;
    صحيح		had_extend;

    cur_si = &CUR_STATE(current_state.ga_len - 1);
    لكل (;;)
    {
	إذا (cur_si->si_پڇاڙيون
		&& (cur_si->si_m_endpos.lnum < current_lnum
		    || (cur_si->si_m_endpos.lnum == current_lnum
			&& cur_si->si_m_endpos.کانو <= current_کانو)))
	{
	    /*
	     * If there is an end pattern group ID, highlight the end pattern
	     * now.  No need to pop the current item from the stack.
	     * Only do this if the end pattern continues beyond the current
	     * position.
	     */
	    إذا (cur_si->si_پڇاڙي_idx
		    && (cur_si->si_eoe_pos.lnum > current_lnum
			|| (cur_si->si_eoe_pos.lnum == current_lnum
			    && cur_si->si_eoe_pos.کانو > current_کانو)))
	    {
		cur_si->si_idx = cur_si->si_پڇاڙي_idx;
		cur_si->si_پڇاڙي_idx = 0;
		cur_si->si_m_endpos = cur_si->si_eoe_pos;
		cur_si->si_h_endpos = cur_si->si_eoe_pos;
		cur_si->si_flags |= HL_ميلاپ;
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
		cur_si->si_seqnr = ايندڙ_seqnr++;
		إذا (cur_si->si_flags & HL_CONCEALENDS)
		    cur_si->si_flags |= HL_CONCEAL;
#جيڪڏهن_پورو
		update_si_attr(current_state.ga_len - 1);

		// nextgroup= should not match in the end pattern
		current_ايندڙ_list = عدم;

		// what matches next may be different now, clear it
		ايندڙ_ميلاپ_idx = 0;
		ايندڙ_ميلاپ_کانو = MAXCOL;
		اكسر;
	    }

	    // handle next_list, unless at end of line and no "skipnl" or
	    // "skipempty"
	    current_ايندڙ_list = cur_si->si_ايندڙ_list;
	    current_ايندڙ_flags = cur_si->si_flags;
	    إذا (!(current_ايندڙ_flags & (HL_SKIPNL | HL_SKIPEMPTY))
		    && syn_getcurline()[current_کانو] == NUL)
		current_ايندڙ_list = عدم;

	    // When the ended item has "extend", another item with
	    // "keepend" now needs to check for its end.
	    had_extend = (cur_si->si_flags & HL_EXTEND);

	    ڪڍو_current_state();

	    إذا (current_state.ga_len == 0)
		اكسر;

	    إذا (had_extend && keepend_level >= 0)
	    {
		syn_update_پڇاڙيون(FALSE);
		إذا (current_state.ga_len == 0)
		    اكسر;
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
	    إذا (cur_si->si_idx >= 0
		    && SYN_شيون(syn_بلاڪ)[cur_si->si_idx].sp_type
								== SPTYPE_شروعات
		    && !(cur_si->si_flags & (HL_ميلاپ | HL_KEEPEND)))
	    {
		update_si_پڇاڙي(cur_si, (صحيح)current_کانو, TRUE);
		check_keepend();
		إذا ((current_ايندڙ_flags & HL_HAS_EOL)
			&& keepend_level < 0
			&& syn_getcurline()[current_کانو] == NUL)
		    اكسر;
	    }
	}
	وإلا
	    اكسر;
    }
}

/*
 * Update an entry in the current_state stack for a match or region.  This
 * fills in si_attr, si_next_list and si_cont_list.
 */
    ساكن فراغ
update_si_attr(صحيح idx)
{
    stateitem_T	*sip = &CUR_STATE(idx);
    synpat_T	*spp;

    // This should not happen...
    إذا (sip->si_idx < 0)
	أرجع;

    spp = &(SYN_شيون(syn_بلاڪ)[sip->si_idx]);
    إذا (sip->si_flags & HL_ميلاپ)
	sip->si_id = spp->sp_syn_ميلاپ_id;
    وإلا
	sip->si_id = spp->sp_syn.id;
    sip->si_attr = syn_id2attr(sip->si_id);
    sip->si_trans_id = sip->si_id;
    إذا (sip->si_flags & HL_ميلاپ)
	sip->si_cont_list = عدم;
    وإلا
	sip->si_cont_list = spp->sp_cont_list;

    /*
     * For transparent items, take attr from outer item.
     * Also take cont_list, if there is none.
     * Don't do this for the matchgroup of a start or end pattern.
     */
    إذا ((spp->sp_flags & HL_TRANSP) && !(sip->si_flags & HL_ميلاپ))
    {
	إذا (idx == 0)
	{
	    sip->si_attr = 0;
	    sip->si_trans_id = 0;
	    إذا (sip->si_cont_list == عدم)
		sip->si_cont_list = ID_LIST_ALL;
	}
	وإلا
	{
	    sip->si_attr = CUR_STATE(idx - 1).si_attr;
	    sip->si_trans_id = CUR_STATE(idx - 1).si_trans_id;
	    إذا (sip->si_cont_list == عدم)
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
    ساكن فراغ
check_keepend(فراغ)
{
    صحيح		i;
    lpos_T	maxpos;
    lpos_T	maxpos_h;
    stateitem_T	*sip;

    /*
     * This check can consume a lot of time; only do it from the level where
     * there really is a keepend.
     */
    إذا (keepend_level < 0)
	أرجع;

    /*
     * Find the last index of an "extend" item.  "keepend" items before that
     * won't do anything.  If there is no "extend" item "i" will be
     * "keepend_level" and all "keepend" items will work normally.
     */
    لكل (i = current_state.ga_len - 1; i > keepend_level; --i)
	إذا (CUR_STATE(i).si_flags & HL_EXTEND)
	    اكسر;

    maxpos.lnum = 0;
    maxpos.کانو = 0;
    maxpos_h.lnum = 0;
    maxpos_h.کانو = 0;
    لكل ( ; i < current_state.ga_len; ++i)
    {
	sip = &CUR_STATE(i);
	إذا (maxpos.lnum != 0)
	{
	    حد_بندي_pos_zero(&sip->si_m_endpos, &maxpos);
	    حد_بندي_pos_zero(&sip->si_h_endpos, &maxpos_h);
	    حد_بندي_pos_zero(&sip->si_eoe_pos, &maxpos);
	    sip->si_پڇاڙيون = TRUE;
	}
	إذا (sip->si_پڇاڙيون && (sip->si_flags & HL_KEEPEND))
	{
	    إذا (maxpos.lnum == 0
		    || maxpos.lnum > sip->si_m_endpos.lnum
		    || (maxpos.lnum == sip->si_m_endpos.lnum
			&& maxpos.کانو > sip->si_m_endpos.کانو))
		maxpos = sip->si_m_endpos;
	    إذا (maxpos_h.lnum == 0
		    || maxpos_h.lnum > sip->si_h_endpos.lnum
		    || (maxpos_h.lnum == sip->si_h_endpos.lnum
			&& maxpos_h.کانو > sip->si_h_endpos.کانو))
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
    ساكن فراغ
update_si_پڇاڙي(
    stateitem_T	*sip,
    صحيح		startcol,   // where to start searching for the end
    صحيح		force)	    // when TRUE overrule a previous end
{
    lpos_T	startpos;
    lpos_T	endpos;
    lpos_T	hl_endpos;
    lpos_T	پڇاڙي_endpos;
    صحيح		پڇاڙي_idx;

    // return quickly for a keyword
    إذا (sip->si_idx < 0)
	أرجع;

    // Don't update when it's already done.  Can be a match of an end pattern
    // that started in a previous line.  Watch out: can also be a "keepend"
    // from a containing item.
    إذا (!force && sip->si_m_endpos.lnum >= current_lnum)
	أرجع;

    /*
     * We need to find the end of the region.  It may continue in the next
     * line.
     */
    پڇاڙي_idx = 0;
    startpos.lnum = current_lnum;
    startpos.کانو = startcol;
    ڳوليو_endpos(sip->si_idx, &startpos, &endpos, &hl_endpos,
		   &(sip->si_flags), &پڇاڙي_endpos, &پڇاڙي_idx, sip->si_extmatch);

    إذا (endpos.lnum == 0)
    {
	// No end pattern matched.
	إذا (SYN_شيون(syn_بلاڪ)[sip->si_idx].sp_flags & HL_ONELINE)
	{
	    // a "oneline" never continues in the next line
	    sip->si_پڇاڙيون = TRUE;
	    sip->si_m_endpos.lnum = current_lnum;
	    sip->si_m_endpos.کانو = syn_getcurline_len();
	}
	وإلا
	{
	    // continues in the next line
	    sip->si_پڇاڙيون = FALSE;
	    sip->si_m_endpos.lnum = 0;
	}
	sip->si_h_endpos = sip->si_m_endpos;
    }
    وإلا
    {
	// match within this line
	sip->si_m_endpos = endpos;
	sip->si_h_endpos = hl_endpos;
	sip->si_eoe_pos = پڇاڙي_endpos;
	sip->si_پڇاڙيون = TRUE;
	sip->si_پڇاڙي_idx = پڇاڙي_idx;
    }
}

/*
 * Add a new state to the current state stack.
 * It is cleared and the index set to "idx".
 * Return FAIL if it's not possible (out of memory).
 */
    ساكن صحيح
ڌڪو_current_state(صحيح idx)
{
    إذا (ga_grow(&current_state, 1) == FAIL)
	أرجع FAIL;
    CLEAR_اشارو(&CUR_STATE(current_state.ga_len));
    CUR_STATE(current_state.ga_len).si_idx = idx;
    ++current_state.ga_len;
    أرجع OK;
}

/*
 * Remove a state from the current_state stack.
 */
    ساكن فراغ
ڪڍو_current_state(فراغ)
{
    إذا (current_state.ga_len)
    {
	unref_extmatch(CUR_STATE(current_state.ga_len - 1).si_extmatch);
	--current_state.ga_len;
    }
    // after the end of a pattern, try matching a keyword or pattern
    ايندڙ_ميلاپ_idx = -1;

    // if first state with "keepend" is popped, reset keepend_level
    إذا (keepend_level >= current_state.ga_len)
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
    ساكن فراغ
ڳوليو_endpos(
    صحيح		idx,		// index of the pattern
    lpos_T	*startpos,	// where to start looking for an END match
    lpos_T	*m_endpos,	// return: end of match
    lpos_T	*hl_endpos,	// return: end of highlighting
    ڊگهو	*flagsp,	// return: flags of matching END
    lpos_T	*پڇاڙي_endpos,	// return: end of end pattern match
    صحيح		*پڇاڙي_idx,	// return: group ID for end pat. match, or 0
    reg_extmatch_T *شروعات_ext)	// submatches from the start pattern
{
    colnr_T	matchcol;
    synpat_T	*spp, *spp_skip;
    صحيح		شروعات_idx;
    صحيح		بهترين_idx;
    regmmatch_T	regmatch;
    regmmatch_T	بهترين_regmatch;	    // startpos/endpos of best match
    lpos_T	pos;
    صحيح		had_ميلاپ = FALSE;
    char_u	buf_chartab[32];  // chartab array for syn option iskyeyword

    // just in case we are invoked for a keyword
    إذا (idx < 0)
	أرجع;

    /*
     * Check for being called with a START pattern.
     * Can happen with a match that continues to the next line, because it
     * contained a region.
     */
    spp = &(SYN_شيون(syn_بلاڪ)[idx]);
    إذا (spp->sp_type != SPTYPE_شروعات)
    {
	*hl_endpos = *startpos;
	أرجع;
    }

    /*
     * Find the SKIP or first END pattern after the last START pattern.
     */
    لكل (;;)
    {
	spp = &(SYN_شيون(syn_بلاڪ)[idx]);
	إذا (spp->sp_type != SPTYPE_شروعات)
	    اكسر;
	++idx;
    }

    /*
     *	Lookup the SKIP pattern (if present)
     */
    إذا (spp->sp_type == SPTYPE_SKIP)
    {
	spp_skip = spp;
	++idx;
    }
    وإلا
	spp_skip = عدم;

    // Setup external matches for syn_regexec().
    unref_extmatch(re_extmatch_اندريون);
    re_extmatch_اندريون = ref_extmatch(شروعات_ext);

    matchcol = startpos->کانو;	// start looking for a match at sstart
    شروعات_idx = idx;		// remember the first END pattern.
    بهترين_regmatch.startpos[0].کانو = 0;		// avoid compiler warning

    // use syntax iskeyword option
    save_chartab(buf_chartab);

    لكل (;;)
    {
	/*
	 * Find end pattern that matches first after "matchcol".
	 */
	بهترين_idx = -1;
	لكل (idx = شروعات_idx; idx < syn_بلاڪ->b_syn_patterns.ga_len; ++idx)
	{
	    صحيح lc_کانو = matchcol;
	    صحيح r;

	    spp = &(SYN_شيون(syn_بلاڪ)[idx]);
	    إذا (spp->sp_type != SPTYPE_پڇاڙي)	// past last END pattern
		اكسر;
	    lc_کانو -= spp->sp_offsets[SPO_LC_OFF];
	    إذا (lc_کانو < 0)
		lc_کانو = 0;

	    regmatch.rmm_ic = spp->sp_ic;
	    regmatch.regprog = spp->sp_prog;
	    r = syn_regexec(&regmatch, startpos->lnum, lc_کانو,
						  IF_SYN_وقت(&spp->sp_وقت));
	    spp->sp_prog = regmatch.regprog;
	    إذا (r)
	    {
		إذا (بهترين_idx == -1 || regmatch.startpos[0].کانو
					      < بهترين_regmatch.startpos[0].کانو)
		{
		    بهترين_idx = idx;
		    بهترين_regmatch.startpos[0] = regmatch.startpos[0];
		    بهترين_regmatch.endpos[0] = regmatch.endpos[0];
		}
	    }
	}

	/*
	 * If all end patterns have been tried, and there is no match, the
	 * item continues until end-of-line.
	 */
	إذا (بهترين_idx == -1)
	    اكسر;

	/*
	 * If the skip pattern matches before the end pattern,
	 * continue searching after the skip pattern.
	 */
	إذا (spp_skip != عدم)
	{
	    صحيح lc_کانو = matchcol - spp_skip->sp_offsets[SPO_LC_OFF];
	    صحيح r;

	    إذا (lc_کانو < 0)
		lc_کانو = 0;
	    regmatch.rmm_ic = spp_skip->sp_ic;
	    regmatch.regprog = spp_skip->sp_prog;
	    r = syn_regexec(&regmatch, startpos->lnum, lc_کانو,
					      IF_SYN_وقت(&spp_skip->sp_وقت));
	    spp_skip->sp_prog = regmatch.regprog;
	    إذا (r && regmatch.startpos[0].کانو
					     <= بهترين_regmatch.startpos[0].کانو)
	    {
		صحيح سٽ_len;

		// Add offset to skip pattern match
		syn_جوڙيو_پڇاڙي_off(&pos, &regmatch, spp_skip, SPO_ME_OFF, 1);

		// If the skip pattern goes on to the next line, there is no
		// match with an end pattern in this line.
		إذا (pos.lnum > startpos->lnum)
		    اكسر;

		سٽ_len = ml_get_buf_len(syn_buf, startpos->lnum);

		// take care of an empty match or negative offset
		إذا (pos.کانو <= matchcol)
		    ++matchcol;
		وإلا إذا (pos.کانو <= regmatch.endpos[0].کانو)
		    matchcol = pos.کانو;
		وإلا
		    // Be careful not to jump over the NUL at the end-of-line
		    لكل (matchcol = regmatch.endpos[0].کانو;
			    matchcol < سٽ_len && matchcol < pos.کانو;
								   ++matchcol)
			;

		// if the skip pattern includes end-of-line, break here
		إذا (matchcol >= سٽ_len)
		    اكسر;

		استمر;	    // start with first end pattern again
	    }
	}

	/*
	 * Match from start pattern to end pattern.
	 * Correct for match and highlight offset of end pattern.
	 */
	spp = &(SYN_شيون(syn_بلاڪ)[بهترين_idx]);
	syn_جوڙيو_پڇاڙي_off(m_endpos, &بهترين_regmatch, spp, SPO_ME_OFF, 1);
	// can't end before the start
	إذا (m_endpos->lnum == startpos->lnum && m_endpos->کانو < startpos->کانو)
	    m_endpos->کانو = startpos->کانو;

	syn_جوڙيو_پڇاڙي_off(پڇاڙي_endpos, &بهترين_regmatch, spp, SPO_HE_OFF, 1);
	// can't end before the start
	إذا (پڇاڙي_endpos->lnum == startpos->lnum
					   && پڇاڙي_endpos->کانو < startpos->کانو)
	    پڇاڙي_endpos->کانو = startpos->کانو;
	// can't end after the match
	حد_بندي_pos(پڇاڙي_endpos, m_endpos);

	/*
	 * If the end group is highlighted differently, adjust the pointers.
	 */
	إذا (spp->sp_syn_ميلاپ_id != spp->sp_syn.id && spp->sp_syn_ميلاپ_id != 0)
	{
	    *پڇاڙي_idx = بهترين_idx;
	    إذا (spp->sp_off_flags & (1 << (SPO_RE_OFF + SPO_ڳڻپ)))
	    {
		hl_endpos->lnum = بهترين_regmatch.endpos[0].lnum;
		hl_endpos->کانو = بهترين_regmatch.endpos[0].کانو;
	    }
	    وإلا
	    {
		hl_endpos->lnum = بهترين_regmatch.startpos[0].lnum;
		hl_endpos->کانو = بهترين_regmatch.startpos[0].کانو;
	    }
	    hl_endpos->کانو += spp->sp_offsets[SPO_RE_OFF];

	    // can't end before the start
	    إذا (hl_endpos->lnum == startpos->lnum
					    && hl_endpos->کانو < startpos->کانو)
		hl_endpos->کانو = startpos->کانو;
	    حد_بندي_pos(hl_endpos, m_endpos);

	    // now the match ends where the highlighting ends, it is turned
	    // into the matchgroup for the end
	    *m_endpos = *hl_endpos;
	}
	وإلا
	{
	    *پڇاڙي_idx = 0;
	    *hl_endpos = *پڇاڙي_endpos;
	}

	*flagsp = spp->sp_flags;

	had_ميلاپ = TRUE;
	اكسر;
    }

    // no match for an END pattern in this line
    إذا (!had_ميلاپ)
	m_endpos->lnum = 0;

    restore_chartab(buf_chartab);

    // Remove external matches.
    unref_extmatch(re_extmatch_اندريون);
    re_extmatch_اندريون = عدم;
}

/*
 * Limit "pos" not to be after "limit".
 */
    ساكن فراغ
حد_بندي_pos(lpos_T *pos, lpos_T *حد_بندي)
{
    إذا (pos->lnum > حد_بندي->lnum)
	*pos = *حد_بندي;
    وإلا إذا (pos->lnum == حد_بندي->lnum && pos->کانو > حد_بندي->کانو)
	pos->کانو = حد_بندي->کانو;
}

/*
 * Limit "pos" not to be after "limit", unless pos->lnum is zero.
 */
    ساكن فراغ
حد_بندي_pos_zero(
    lpos_T	*pos,
    lpos_T	*حد_بندي)
{
    إذا (pos->lnum == 0)
	*pos = *حد_بندي;
    وإلا
	حد_بندي_pos(pos, حد_بندي);
}

/*
 * Add offset to matched text for end of match or highlight.
 */
    ساكن فراغ
syn_جوڙيو_پڇاڙي_off(
    lpos_T	*نتيجو,	// returned position
    regmmatch_T	*regmatch,	// start/end of match
    synpat_T	*spp,		// matched pattern
    صحيح		idx,		// index of offset
    صحيح		extra)		// extra chars for offset to start
{
    صحيح		کانو;
    صحيح		off;
    char_u	*بنياد;
    char_u	*p;

    إذا (spp->sp_off_flags & (1 << idx))
    {
	نتيجو->lnum = regmatch->startpos[0].lnum;
	کانو = regmatch->startpos[0].کانو;
	off = spp->sp_offsets[idx] + extra;
    }
    وإلا
    {
	نتيجو->lnum = regmatch->endpos[0].lnum;
	کانو = regmatch->endpos[0].کانو;
	off = spp->sp_offsets[idx];
    }
    // Don't go past the end of the line.  Matters for "rs=e+2" when there
    // is a matchgroup. Watch out for match with last NL in the buffer.
    إذا (نتيجو->lnum > syn_buf->b_ml.ml_سٽ_ڳڻپ)
	کانو = 0;
    وإلا إذا (off != 0)
    {
	بنياد = ml_get_buf(syn_buf, نتيجو->lnum, FALSE);
	p = بنياد + کانو;
	إذا (off > 0)
	{
	    طالما (off-- > 0 && *p != NUL)
		MB_PTR_ADV(p);
	}
	وإلا إذا (off < 0)
	{
	    طالما (off++ < 0 && بنياد < p)
		MB_PTR_پٺ(بنياد, p);
	}
	کانو = (صحيح)(p - بنياد);
    }
    نتيجو->کانو = کانو;
}

/*
 * Add offset to matched text for start of match or highlight.
 * Avoid resulting column to become negative.
 */
    ساكن فراغ
syn_جوڙيو_شروعات_off(
    lpos_T	*نتيجو,	// returned position
    regmmatch_T	*regmatch,	// start/end of match
    synpat_T	*spp,
    صحيح		idx,
    صحيح		extra)	    // extra chars for offset to end
{
    صحيح		کانو;
    صحيح		off;
    char_u	*بنياد;
    char_u	*p;

    إذا (spp->sp_off_flags & (1 << (idx + SPO_ڳڻپ)))
    {
	نتيجو->lnum = regmatch->endpos[0].lnum;
	کانو = regmatch->endpos[0].کانو;
	off = spp->sp_offsets[idx] + extra;
    }
    وإلا
    {
	نتيجو->lnum = regmatch->startpos[0].lnum;
	کانو = regmatch->startpos[0].کانو;
	off = spp->sp_offsets[idx];
    }
    إذا (نتيجو->lnum > syn_buf->b_ml.ml_سٽ_ڳڻپ)
    {
	// a "\n" at the end of the pattern may take us below the last line
	نتيجو->lnum = syn_buf->b_ml.ml_سٽ_ڳڻپ;
	کانو = ml_get_buf_len(syn_buf, نتيجو->lnum);
    }
    إذا (off != 0)
    {
	بنياد = ml_get_buf(syn_buf, نتيجو->lnum, FALSE);
	p = بنياد + کانو;
	إذا (off > 0)
	{
	    طالما (off-- && *p != NUL)
		MB_PTR_ADV(p);
	}
	وإلا إذا (off < 0)
	{
	    طالما (off++ && بنياد < p)
		MB_PTR_پٺ(بنياد, p);
	}
	کانو = (صحيح)(p - بنياد);
    }
    نتيجو->کانو = کانو;
}

/*
 * Get current line in syntax buffer.
 */
    ساكن char_u *
syn_getcurline(فراغ)
{
    أرجع ml_get_buf(syn_buf, current_lnum, FALSE);
}

/*
 * Get length of current line in syntax buffer.
 */
    ساكن colnr_T
syn_getcurline_len(فراغ)
{
    أرجع ml_get_buf_len(syn_buf, current_lnum);
}

/*
 * Call vim_regexec() to find a match with "rmp" in "syn_buf".
 * Returns TRUE when there is a match.
 */
    ساكن صحيح
syn_regexec(
    regmmatch_T	*rmp,
    linenr_T	lnum,
    colnr_T	کانو,
    syn_وقت_T  *st اڻ_استعمال_ٿيل)
{
    صحيح		r;
    صحيح		timed_ٻاهريون = FALSE;
#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
    proftime_T	pt;

    إذا (syn_وقت_on)
	profile_شروعات(&pt);
#جيڪڏهن_پورو

    إذا (rmp->regprog == عدم)
	// This can happen if a previous call to vim_regexec_multi() tried to
	// use the NFA engine, which resulted in NFA_TOO_EXPENSIVE, and
	// compiling the pattern with the other engine fails.
	أرجع FALSE;

    rmp->rmm_maxcol = syn_buf->b_p_smc;
    r = vim_regexec_multi(rmp, syn_win, syn_buf, lnum, کانو, &timed_ٻاهريون);

#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
    إذا (syn_وقت_on)
    {
	profile_پڇاڙي(&pt);
	profile_جوڙيو(&st->ڪل, &pt);
	إذا (profile_cmp(&pt, &st->slowest) < 0)
	    st->slowest = pt;
	++st->ڳڻپ;
	إذا (r > 0)
	    ++st->ميلاپ;
    }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_RELTIME
    إذا (timed_ٻاهريون && redrawtime_حد_بندي_سيٽ && !syn_win->w_s->b_syn_slow)
    {
	syn_win->w_s->b_syn_slow = TRUE;
	نياپو(_("'redrawtime' exceeded, syntax highlighting disabled"));
    }
#جيڪڏهن_پورو

    إذا (r > 0)
    {
	rmp->startpos[0].lnum += lnum;
	rmp->endpos[0].lnum += lnum;
	أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Check one position in a line for a matching keyword.
 * The caller must check if a keyword can start at startcol.
 * Return its ID if found, 0 otherwise.
 */
    ساكن صحيح
check_keyword_id(
    char_u	*سٽ,
    صحيح		startcol,	// position in line to check for keyword
    صحيح		*endcolp,	// return: character after found keyword
    ڊگهو	*flagsp,	// return: flags of matching keyword
    ننڍو	**ايندڙ_listp,	// return: next_list of matching keyword
    stateitem_T	*cur_si,	// item at the top of the stack
    صحيح		*ccharp اڻ_استعمال_ٿيل)	// conceal substitution char
{
    keyentry_T	*kp;
    char_u	*kwp;
    صحيح		ڦيرو;
    صحيح		kwlen;
    char_u	keyword[MAXKEYWLEN + 1]; // assume max. keyword len is 80
    hashtab_T	*ht;
    hashitem_T	*hi;

    // Find first character after the keyword.  First character was already
    // checked.
    kwp = سٽ + startcol;
    kwlen = 0;
    افعل
    {
	إذا (has_mbyte)
	    kwlen += (*mb_ptr2len)(kwp + kwlen);
	وإلا
	    ++kwlen;
    }
    طالما (vim_iswordp_buf(kwp + kwlen, syn_buf));

    إذا (kwlen > MAXKEYWLEN)
	أرجع 0;

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
    لكل (ڦيرو = 1; ڦيرو <= 2; ++ڦيرو)
    {
	ht = ڦيرو == 1 ? &syn_بلاڪ->b_keywtab : &syn_بلاڪ->b_keywtab_ic;
	إذا (ht->ht_استعمال_ٿيل == 0)
	    استمر;
	إذا (ڦيرو == 2)	// ignore case
	    (فراغ)str_foldcase(kwp, kwlen, keyword, MAXKEYWLEN + 1);

	/*
	 * Find keywords that match.  There can be several with different
	 * attributes.
	 * When current_next_list is non-zero accept only that group, otherwise:
	 *  Accept a not-contained keyword at toplevel.
	 *  Accept a keyword at other levels only if it is in the contains list.
	 */
	hi = hash_ڳوليو(ht, keyword);
	إذا (!HASHITEM_خالي_جايون(hi))
	    لكل (kp = HI2KE(hi); kp != عدم; kp = kp->ke_ايندڙ)
	    {
		إذا (current_ايندڙ_list != 0
			? اندريون_id_list(عدم, current_ايندڙ_list, &kp->k_syn, 0)
			: (cur_si == عدم
			    ? !(kp->flags & HL_CONTAINED)
			    : اندريون_id_list(cur_si, cur_si->si_cont_list,
				      &kp->k_syn, kp->flags)))
		{
		    *endcolp = startcol + kwlen;
		    *flagsp = kp->flags;
		    *ايندڙ_listp = kp->ايندڙ_list;
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
		    *ccharp = kp->k_char;
#جيڪڏهن_پورو
		    أرجع kp->k_syn.id;
		}
	    }
    }
    أرجع 0;
}

/*
 * Handle ":syntax conceal" command.
 */
    ساكن فراغ
syn_cmd_conceal(exarg_T *eap اڻ_استعمال_ٿيل, صحيح syncing اڻ_استعمال_ٿيل)
{
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
    char_u	*arg = eap->arg;
    char_u	*ايندڙ;

    eap->nextcmd = ڳوليو_nextcmd(arg);
    إذا (eap->skip)
	أرجع;

    ايندڙ = skiptowhite(arg);
    إذا (*arg == NUL)
    {
	إذا (curwin->w_s->b_syn_conceal)
	    نياپو("syntax conceal on");
	وإلا
	    نياپو("syntax conceal off");
    }
    وإلا إذا (STRNICMP(arg, "on", 2) == 0 && ايندڙ - arg == 2)
	curwin->w_s->b_syn_conceal = TRUE;
    وإلا إذا (STRNICMP(arg, "off", 3) == 0 && ايندڙ - arg == 3)
	curwin->w_s->b_syn_conceal = FALSE;
    وإلا
	semsg(_(e_illegal_دليل_str_2), arg);
#جيڪڏهن_پورو
}

/*
 * Handle ":syntax case" command.
 */
    ساكن فراغ
syn_cmd_case(exarg_T *eap, صحيح syncing اڻ_استعمال_ٿيل)
{
    char_u	*arg = eap->arg;
    char_u	*ايندڙ;

    eap->nextcmd = ڳوليو_nextcmd(arg);
    إذا (eap->skip)
	أرجع;

    ايندڙ = skiptowhite(arg);
    إذا (*arg == NUL)
    {
	إذا (curwin->w_s->b_syn_ic)
	    نياپو("syntax case ignore");
	وإلا
	    نياپو("syntax case match");
    }
    وإلا إذا (STRNICMP(arg, "match", 5) == 0 && ايندڙ - arg == 5)
	curwin->w_s->b_syn_ic = FALSE;
    وإلا إذا (STRNICMP(arg, "ignore", 6) == 0 && ايندڙ - arg == 6)
	curwin->w_s->b_syn_ic = TRUE;
    وإلا
	semsg(_(e_illegal_دليل_str_2), arg);
}

/*
 * Handle ":syntax foldlevel" command.
 */
    ساكن فراغ
syn_cmd_foldlevel(exarg_T *eap, صحيح syncing اڻ_استعمال_ٿيل)
{
    char_u *arg = eap->arg;
    char_u *arg_پڇاڙي;

    eap->nextcmd = ڳوليو_nextcmd(arg);
    إذا (eap->skip)
	أرجع;

    إذا (*arg == NUL)
    {
	بدل (curwin->w_s->b_syn_foldlevel)
	{
	    حالة SYNFLD_شروعات:   نياپو("syntax foldlevel start");   اكسر;
	    حالة SYNFLD_MINIMUM: نياپو("syntax foldlevel minimum"); اكسر;
	    افتراضي: اكسر;
	}
	أرجع;
    }

    arg_پڇاڙي = skiptowhite(arg);
    إذا (STRNICMP(arg, "start", 5) == 0 && arg_پڇاڙي - arg == 5)
	curwin->w_s->b_syn_foldlevel = SYNFLD_شروعات;
    وإلا إذا (STRNICMP(arg, "minimum", 7) == 0 && arg_پڇاڙي - arg == 7)
	curwin->w_s->b_syn_foldlevel = SYNFLD_MINIMUM;
    وإلا
    {
	semsg(_(e_illegal_دليل_str_2), arg);
	أرجع;
    }

    arg = skipwhite(arg_پڇاڙي);
    إذا (*arg != NUL)
    {
	semsg(_(e_illegal_دليل_str_2), arg);
    }
}

/*
 * Handle ":syntax spell" command.
 */
    ساكن فراغ
syn_cmd_spell(exarg_T *eap, صحيح syncing اڻ_استعمال_ٿيل)
{
    char_u	*arg = eap->arg;
    char_u	*ايندڙ;

    eap->nextcmd = ڳوليو_nextcmd(arg);
    إذا (eap->skip)
	أرجع;

    ايندڙ = skiptowhite(arg);
    إذا (*arg == NUL)
    {
	إذا (curwin->w_s->b_syn_spell == SYNSPL_چوٽي)
	    نياپو("syntax spell toplevel");
	وإلا إذا (curwin->w_s->b_syn_spell == SYNSPL_NOTOP)
	    نياپو("syntax spell notoplevel");
	وإلا
	    نياپو("syntax spell default");
    }
    وإلا إذا (STRNICMP(arg, "toplevel", 8) == 0 && ايندڙ - arg == 8)
	curwin->w_s->b_syn_spell = SYNSPL_چوٽي;
    وإلا إذا (STRNICMP(arg, "notoplevel", 10) == 0 && ايندڙ - arg == 10)
	curwin->w_s->b_syn_spell = SYNSPL_NOTOP;
    وإلا إذا (STRNICMP(arg, "default", 7) == 0 && ايندڙ - arg == 7)
	curwin->w_s->b_syn_spell = SYNSPL_DEFAULT;
    وإلا
    {
	semsg(_(e_illegal_دليل_str_2), arg);
	أرجع;
    }

    // assume spell checking changed, force a redraw
    redraw_win_later(curwin, UPD_NOT_VALID);
}

/*
 * Handle ":syntax iskeyword" command.
 */
    ساكن فراغ
syn_cmd_iskeyword(exarg_T *eap, صحيح syncing اڻ_استعمال_ٿيل)
{
    char_u	*arg = eap->arg;
    char_u	save_chartab[32];
    char_u	*save_isk;

    إذا (eap->skip)
	أرجع;

    arg = skipwhite(arg);
    إذا (*arg == NUL)
    {
	نياپو_puts("\n");
	إذا (curwin->w_s->b_syn_isk != خالي_جايون_option)
	{
	    نياپو_puts("syntax iskeyword ");
	    نياپو_outtrans(curwin->w_s->b_syn_isk);
	}
	وإلا
	    نياپو_outtrans((char_u *)_("syntax iskeyword not set"));
    }
    وإلا
    {
	إذا (STRNICMP(arg, "clear", 5) == 0)
	{
	    mch_memmove(curwin->w_s->b_syn_chartab, curbuf->b_chartab,
								  (size_t)32);
	    clear_string_option(&curwin->w_s->b_syn_isk);
	}
	وإلا
	{
	    إذا (check_isopt(arg) == FAIL)
	    {
		emsg(_(e_invalid_دليل));
		أرجع;
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
    فراغ
syntax_clear(synblock_T *بلاڪ)
{
    صحيح i;

    بلاڪ->b_syn_error = FALSE;	    // clear previous error
#جيڪڏهن_وصف_ٿيل FEAT_RELTIME
    بلاڪ->b_syn_slow = FALSE;	    // clear previous timeout
#جيڪڏهن_پورو
    بلاڪ->b_syn_ic = FALSE;	    // Use case, by default
    بلاڪ->b_syn_foldlevel = SYNFLD_شروعات;
    بلاڪ->b_syn_spell = SYNSPL_DEFAULT; // default spell checking
    بلاڪ->b_syn_containedin = FALSE;
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
    بلاڪ->b_syn_conceal = FALSE;
#جيڪڏهن_پورو

    // free the keywords
    clear_keywtab(&بلاڪ->b_keywtab);
    clear_keywtab(&بلاڪ->b_keywtab_ic);

    // free the syntax patterns
    لكل (i = بلاڪ->b_syn_patterns.ga_len; --i >= 0; )
	syn_clear_نمونو(بلاڪ, i);
    ga_clear(&بلاڪ->b_syn_patterns);

    // free the syntax clusters
    لكل (i = بلاڪ->b_syn_clusters.ga_len; --i >= 0; )
	syn_clear_cluster(بلاڪ, i);
    ga_clear(&بلاڪ->b_syn_clusters);
    بلاڪ->b_spell_cluster_id = 0;
    بلاڪ->b_nospell_cluster_id = 0;

    بلاڪ->b_syn_sync_flags = 0;
    بلاڪ->b_syn_sync_minlines = 0;
    بلاڪ->b_syn_sync_maxlines = 0;
    بلاڪ->b_syn_sync_linebreaks = 0;

    vim_regfree(بلاڪ->b_syn_linecont_prog);
    بلاڪ->b_syn_linecont_prog = عدم;
    VIM_CLEAR(بلاڪ->b_syn_linecont_pat);
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    بلاڪ->b_syn_folditems = 0;
#جيڪڏهن_پورو
    clear_string_option(&بلاڪ->b_syn_isk);

    // free the stored states
    syn_ڍير_free_all(بلاڪ);
    invalidate_current_state();

    // Reset the counter for ":syn include"
    running_syn_inc_ٽيگ = 0;
}

/*
 * Get rid of ownsyntax for window "wp".
 */
    فراغ
reset_synblock(win_T *wp)
{
    إذا (wp->w_s != &wp->w_بفر->b_s)
    {
	syntax_clear(wp->w_s);
	vim_free(wp->w_s);
	wp->w_s = &wp->w_بفر->b_s;
    }
}

/*
 * Clear syncing info for one buffer.
 */
    ساكن فراغ
syntax_sync_clear(فراغ)
{
    صحيح i;

    // free the syntax patterns
    لكل (i = curwin->w_s->b_syn_patterns.ga_len; --i >= 0; )
	إذا (SYN_شيون(curwin->w_s)[i].sp_syncing)
	    syn_remove_نمونو(curwin->w_s, i);

    curwin->w_s->b_syn_sync_flags = 0;
    curwin->w_s->b_syn_sync_minlines = 0;
    curwin->w_s->b_syn_sync_maxlines = 0;
    curwin->w_s->b_syn_sync_linebreaks = 0;

    vim_regfree(curwin->w_s->b_syn_linecont_prog);
    curwin->w_s->b_syn_linecont_prog = عدم;
    VIM_CLEAR(curwin->w_s->b_syn_linecont_pat);
    clear_string_option(&curwin->w_s->b_syn_isk);

    syn_ڍير_free_all(curwin->w_s);	// Need to recompute all syntax.
}

/*
 * Remove one pattern from the buffer's pattern list.
 */
    ساكن فراغ
syn_remove_نمونو(
    synblock_T	*بلاڪ,
    صحيح		idx)
{
    synpat_T	*spp;

    spp = &(SYN_شيون(بلاڪ)[idx]);
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    إذا (spp->sp_flags & HL_FOLD)
	--بلاڪ->b_syn_folditems;
#جيڪڏهن_پورو
    syn_clear_نمونو(بلاڪ, idx);
    mch_memmove(spp, spp + 1,
		   حجم(synpat_T) * (بلاڪ->b_syn_patterns.ga_len - idx - 1));
    --بلاڪ->b_syn_patterns.ga_len;
}

/*
 * Clear and free one syntax pattern.  When clearing all, must be called from
 * last to first!
 */
    ساكن فراغ
syn_clear_نمونو(synblock_T *بلاڪ, صحيح i)
{
    vim_free(SYN_شيون(بلاڪ)[i].sp_نمونو);
    vim_regfree(SYN_شيون(بلاڪ)[i].sp_prog);
    // Only free sp_cont_list and sp_next_list of first start pattern
    إذا (i == 0 || SYN_شيون(بلاڪ)[i - 1].sp_type != SPTYPE_شروعات)
    {
	vim_free(SYN_شيون(بلاڪ)[i].sp_cont_list);
	vim_free(SYN_شيون(بلاڪ)[i].sp_ايندڙ_list);
	vim_free(SYN_شيون(بلاڪ)[i].sp_syn.cont_اندريون_list);
    }
}

/*
 * Clear and free one syntax cluster.
 */
    ساكن فراغ
syn_clear_cluster(synblock_T *بلاڪ, صحيح i)
{
    vim_free(SYN_CLSTR(بلاڪ)[i].scl_name);
    vim_free(SYN_CLSTR(بلاڪ)[i].scl_name_u);
    vim_free(SYN_CLSTR(بلاڪ)[i].scl_list);
}

/*
 * Handle ":syntax clear" command.
 */
    ساكن فراغ
syn_cmd_clear(exarg_T *eap, صحيح syncing)
{
    char_u	*arg = eap->arg;
    char_u	*arg_پڇاڙي;
    صحيح		id;

    eap->nextcmd = ڳوليو_nextcmd(arg);
    إذا (eap->skip)
	أرجع;

    /*
     * We have to disable this within ":syn include @group filename",
     * because otherwise @group would get deleted.
     * Only required for Vim 5.x syntax files, 6.0 ones don't contain ":syn
     * clear".
     */
    إذا (curwin->w_s->b_syn_topgrp != 0)
	أرجع;

    إذا (پڇاڙيون_excmd2(eap->cmd, arg))
    {
	/*
	 * No argument: Clear all syntax items.
	 */
	إذا (syncing)
	    syntax_sync_clear();
	وإلا
	{
	    syntax_clear(curwin->w_s);
	    إذا (curwin->w_s == &curwin->w_بفر->b_s)
		do_unlet((char_u *)"b:current_syntax", TRUE);
	    do_unlet((char_u *)"w:current_syntax", TRUE);
	}
    }
    وإلا
    {
	/*
	 * Clear the group IDs that are in the argument.
	 */
	طالما (!پڇاڙيون_excmd2(eap->cmd, arg))
	{
	    arg_پڇاڙي = skiptowhite(arg);
	    إذا (*arg == '@')
	    {
		id = syn_scl_namen2id(arg + 1, (صحيح)(arg_پڇاڙي - arg - 1));
		إذا (id == 0)
		{
		    semsg(_(e_no_such_syntax_cluster_str_1), arg);
		    اكسر;
		}
		وإلا
		{
		    /*
		     * We can't physically delete a cluster without changing
		     * the IDs of other clusters, so we do the next best thing
		     * and make it empty.
		     */
		    ننڍو scl_id = id - SYNID_CLUSTER;

		    VIM_CLEAR(SYN_CLSTR(curwin->w_s)[scl_id].scl_list);
		}
	    }
	    وإلا
	    {
		id = syn_namen2id(arg, (صحيح)(arg_پڇاڙي - arg));
		إذا (id == 0)
		{
		    semsg(_(e_no_such_highlight_گروهه_name_str), arg);
		    اكسر;
		}
		وإلا
		    syn_clear_one(id, syncing);
	    }
	    arg = skipwhite(arg_پڇاڙي);
	}
    }
    redraw_curbuf_later(UPD_SOME_VALID);
    syn_ڍير_free_all(curwin->w_s);		// Need to recompute all syntax.
}

/*
 * Clear one syntax group for the current buffer.
 */
    ساكن فراغ
syn_clear_one(صحيح id, صحيح syncing)
{
    synpat_T	*spp;
    صحيح		idx;

    // Clear keywords only when not ":syn sync clear group-name"
    إذا (!syncing)
    {
	(فراغ)syn_clear_keyword(id, &curwin->w_s->b_keywtab);
	(فراغ)syn_clear_keyword(id, &curwin->w_s->b_keywtab_ic);
    }

    // clear the patterns for "id"
    لكل (idx = curwin->w_s->b_syn_patterns.ga_len; --idx >= 0; )
    {
	spp = &(SYN_شيون(curwin->w_s)[idx]);
	إذا (spp->sp_syn.id != id || spp->sp_syncing != syncing)
	    استمر;
	syn_remove_نمونو(curwin->w_s, idx);
    }
}

/*
 * Handle ":syntax on" command.
 */
    ساكن فراغ
syn_cmd_on(exarg_T *eap, صحيح syncing اڻ_استعمال_ٿيل)
{
    syn_cmd_onoff(eap, "syntax");
}

/*
 * Handle ":syntax enable" command.
 */
    ساكن فراغ
syn_cmd_enable(exarg_T *eap, صحيح syncing اڻ_استعمال_ٿيل)
{
    سيٽ_اندروني_string_var((char_u *)"g:syntax_cmd", (char_u *)"enable");
    syn_cmd_onoff(eap, "syntax");
    do_unlet((char_u *)"g:syntax_cmd", TRUE);
}

/*
 * Handle ":syntax reset" command.
 * It actually resets highlighting, not syntax.
 */
    ساكن فراغ
syn_cmd_reset(exarg_T *eap, صحيح syncing اڻ_استعمال_ٿيل)
{
    سيٽ_nextcmd(eap, eap->arg);
    إذا (!eap->skip)
    {
	سيٽ_اندروني_string_var((char_u *)"g:syntax_cmd", (char_u *)"reset");
	do_cmdline_cmd((char_u *)"runtime! syntax/syncolor.vim");
	do_unlet((char_u *)"g:syntax_cmd", TRUE);
    }
}

/*
 * Handle ":syntax manual" command.
 */
    ساكن فراغ
syn_cmd_manual(exarg_T *eap, صحيح syncing اڻ_استعمال_ٿيل)
{
    syn_cmd_onoff(eap, "manual");
}

/*
 * Handle ":syntax off" command.
 */
    ساكن فراغ
syn_cmd_off(exarg_T *eap, صحيح syncing اڻ_استعمال_ٿيل)
{
    syn_cmd_onoff(eap, "nosyntax");
}

    ساكن فراغ
syn_cmd_onoff(exarg_T *eap, حرف *name)
{
    char_u	buf[100];

    سيٽ_nextcmd(eap, eap->arg);
    إذا (!eap->skip)
    {
	STRCPY(buf, "so ");
	vim_snprintf((حرف *)buf + 3, حجم(buf) - 3, SYNTAX_FNAME, name);
	do_cmdline_cmd(buf);
    }
}

/*
 * Handle ":syntax [list]" command: list current syntax words.
 */
    ساكن فراغ
syn_cmd_list(
    exarg_T	*eap,
    صحيح		syncing)	    // when TRUE: list syncing items
{
    char_u	*arg = eap->arg;
    صحيح		id;
    char_u	*arg_پڇاڙي;

    eap->nextcmd = ڳوليو_nextcmd(arg);
    إذا (eap->skip)
	أرجع;

    إذا (!syntax_present(curwin))
    {
	نياپو(_(نياپو_no_شيون));
	أرجع;
    }

    إذا (syncing)
    {
	إذا (curwin->w_s->b_syn_sync_flags & SF_CCOMMENT)
	{
	    نياپو_puts(_("syncing on C-style comments"));
	    syn_سٽون_نياپو();
	    syn_ميلاپ_نياپو();
	}
	وإلا إذا (curwin->w_s->b_syn_sync_flags & SF_ميلاپ)
	{
	    نياپو_puts_title(_("\n--- Syntax sync items ---"));
	    إذا (curwin->w_s->b_syn_sync_minlines > 0
		|| curwin->w_s->b_syn_sync_maxlines > 0
		|| curwin->w_s->b_syn_sync_linebreaks > 0)
	    {
		نياپو_puts(_("\nsyncing on items"));
		syn_سٽون_نياپو();
		syn_ميلاپ_نياپو();
	    }
	    لكل (id = 1; id <= highlight_انگ_groups() && !got_int; ++id)
		syn_list_one(id, syncing, FALSE);
	}
	وإلا
	{
	    إذا (curwin->w_s->b_syn_sync_minlines == 0)
		نياپو_puts(_("no syncing"));
	    وإلا
	    {
		إذا (curwin->w_s->b_syn_sync_minlines == MAXLNUM)
		    نياپو_puts(_("syncing starts at the first line"));
		وإلا
		{
		    نياپو_puts(_("syncing starts "));
		    نياپو_outnum(curwin->w_s->b_syn_sync_minlines);
		    نياپو_puts(_(" lines before top line"));
		}
		syn_ميلاپ_نياپو();
	    }
	}
	أرجع;
    }

    نياپو_puts_title(_("\n--- Syntax items ---"));
    إذا (پڇاڙيون_excmd2(eap->cmd, arg))
    {
	/*
	 * No argument: List all group IDs and all syntax clusters.
	 */
	لكل (id = 1; id <= highlight_انگ_groups() && !got_int; ++id)
	    syn_list_one(id, syncing, FALSE);
	لكل (id = 0; id < curwin->w_s->b_syn_clusters.ga_len && !got_int; ++id)
	    syn_list_cluster(id);
    }
    وإلا
    {
	/*
	 * List the group IDs and syntax clusters that are in the argument.
	 */
	طالما (!پڇاڙيون_excmd2(eap->cmd, arg) && !got_int)
	{
	    arg_پڇاڙي = skiptowhite(arg);
	    إذا (*arg == '@')
	    {
		id = syn_scl_namen2id(arg + 1, (صحيح)(arg_پڇاڙي - arg - 1));
		إذا (id == 0)
		    semsg(_(e_no_such_syntax_cluster_str_2), arg);
		وإلا
		    syn_list_cluster(id - SYNID_CLUSTER);
	    }
	    وإلا
	    {
		id = syn_namen2id(arg, (صحيح)(arg_پڇاڙي - arg));
		إذا (id == 0)
		    semsg(_(e_no_such_highlight_گروهه_name_str), arg);
		وإلا
		    syn_list_one(id, syncing, TRUE);
	    }
	    arg = skipwhite(arg_پڇاڙي);
	}
    }
    سيٽ_nextcmd(eap, arg);
}

    ساكن فراغ
syn_سٽون_نياپو(فراغ)
{
    إذا (curwin->w_s->b_syn_sync_maxlines > 0
				      || curwin->w_s->b_syn_sync_minlines > 0)
    {
	نياپو_puts("; ");
	إذا (curwin->w_s->b_syn_sync_minlines == MAXLNUM)
	    نياپو_puts(_("from the first line"));
	وإلا
	{
	    إذا (curwin->w_s->b_syn_sync_minlines > 0)
	    {
		نياپو_puts(_("minimal "));
		نياپو_outnum(curwin->w_s->b_syn_sync_minlines);
		إذا (curwin->w_s->b_syn_sync_maxlines)
		    نياپو_puts(", ");
	    }
	    إذا (curwin->w_s->b_syn_sync_maxlines > 0)
	    {
		نياپو_puts(_("maximal "));
		نياپو_outnum(curwin->w_s->b_syn_sync_maxlines);
	    }
	    نياپو_puts(_(" lines before top line"));
	}
    }
}

    ساكن فراغ
syn_ميلاپ_نياپو(فراغ)
{
    إذا (curwin->w_s->b_syn_sync_linebreaks > 0)
    {
	نياپو_puts(_("; match "));
	نياپو_outnum(curwin->w_s->b_syn_sync_linebreaks);
	نياپو_puts(_(" line breaks"));
    }
}

ساكن صحيح  آخري_matchgroup;

ساكن فراغ syn_list_flags(keyvalue_T *nlist, صحيح nr_entries, صحيح flags, صحيح attr);

/*
 * List one syntax item, for ":syntax" or "syntax list syntax_name".
 */
    ساكن فراغ
syn_list_one(
    صحيح		id,
    صحيح		syncing,	    // when TRUE: list syncing items
    صحيح		link_only)	    // when TRUE; list link-only too
{
    صحيح		attr;
    صحيح		idx;
    صحيح		did_سرو = FALSE;
    synpat_T	*spp;
    ساكن keyvalue_T namelist1[] =
		{
		    KEYVALUE_داخلا(HL_DISPLAY, "display"),
		    KEYVALUE_داخلا(HL_CONTAINED, "contained"),
		    KEYVALUE_داخلا(HL_ONELINE, "oneline"),
		    KEYVALUE_داخلا(HL_KEEPEND, "keepend"),
		    KEYVALUE_داخلا(HL_EXTEND, "extend"),
		    KEYVALUE_داخلا(HL_EXCLUDENL, "excludenl"),
		    KEYVALUE_داخلا(HL_TRANSP, "transparent"),
		    KEYVALUE_داخلا(HL_FOLD, "fold")
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
		    ,
		    KEYVALUE_داخلا(HL_CONCEAL, "conceal"),
		    KEYVALUE_داخلا(HL_CONCEALENDS, "concealends")
#جيڪڏهن_پورو
		};
    ساكن keyvalue_T namelist2[] =
		{
		    KEYVALUE_داخلا(HL_SKIPWHITE, "skipwhite"),
		    KEYVALUE_داخلا(HL_SKIPNL, "skipnl"),
		    KEYVALUE_داخلا(HL_SKIPEMPTY, "skipempty")
		};

    attr = HL_ATTR(HLF_D);		// highlight like directories

    // list the keywords for "id"
    إذا (!syncing)
    {
	did_سرو = syn_list_keywords(id, &curwin->w_s->b_keywtab, FALSE, attr);
	did_سرو = syn_list_keywords(id, &curwin->w_s->b_keywtab_ic,
							    did_سرو, attr);
    }

    // list the patterns for "id"
    لكل (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len && !got_int; ++idx)
    {
	spp = &(SYN_شيون(curwin->w_s)[idx]);
	إذا (spp->sp_syn.id != id || spp->sp_syncing != syncing)
	    استمر;

	(فراغ)syn_list_سرو(did_سرو, 999, id);
	did_سرو = TRUE;
	آخري_matchgroup = 0;
	إذا (spp->sp_type == SPTYPE_ميلاپ)
	{
	    put_نمونو("match", ' ', spp, attr);
	    نياپو_putchar(' ');
	}
	وإلا إذا (spp->sp_type == SPTYPE_شروعات)
	{
	    طالما (SYN_شيون(curwin->w_s)[idx].sp_type == SPTYPE_شروعات)
		put_نمونو("start", '=', &SYN_شيون(curwin->w_s)[idx++], attr);
	    إذا (SYN_شيون(curwin->w_s)[idx].sp_type == SPTYPE_SKIP)
		put_نمونو("skip", '=', &SYN_شيون(curwin->w_s)[idx++], attr);
	    طالما (idx < curwin->w_s->b_syn_patterns.ga_len
			  && SYN_شيون(curwin->w_s)[idx].sp_type == SPTYPE_پڇاڙي)
		put_نمونو("end", '=', &SYN_شيون(curwin->w_s)[idx++], attr);
	    --idx;
	    نياپو_putchar(' ');
	}
	syn_list_flags(namelist1, (صحيح)ARRAY_ڊيگهائي(namelist1), spp->sp_flags, attr);

	إذا (spp->sp_cont_list != عدم)
	    put_id_list((char_u *)"contains", spp->sp_cont_list, attr);

	إذا (spp->sp_syn.cont_اندريون_list != عدم)
	    put_id_list((char_u *)"containedin",
					      spp->sp_syn.cont_اندريون_list, attr);

	إذا (spp->sp_ايندڙ_list != عدم)
	{
	    put_id_list((char_u *)"nextgroup", spp->sp_ايندڙ_list, attr);
	    syn_list_flags(namelist2, (صحيح)ARRAY_ڊيگهائي(namelist2), spp->sp_flags, attr);
	}
	إذا (spp->sp_flags & (HL_SYNC_هتي|HL_SYNC_THERE))
	{
	    إذا (spp->sp_flags & HL_SYNC_هتي)
		نياپو_puts_attr("grouphere", attr);
	    وإلا
		نياپو_puts_attr("groupthere", attr);
	    نياپو_putchar(' ');
	    إذا (spp->sp_sync_idx >= 0)
		نياپو_outtrans(highlight_گروهه_name(SYN_شيون(curwin->w_s)
				   [spp->sp_sync_idx].sp_syn.id - 1));
	    وإلا
		نياپو_puts("NONE");
	    نياپو_putchar(' ');
	}
    }

    // list the link, if there is one
    إذا (highlight_link_id(id - 1) && (did_سرو || link_only) && !got_int)
    {
	(فراغ)syn_list_سرو(did_سرو, 999, id);
	نياپو_puts_attr("links to", attr);
	نياپو_putchar(' ');
	نياپو_outtrans(highlight_گروهه_name(highlight_link_id(id - 1) - 1));
    }
}

    ساكن فراغ
syn_list_flags(keyvalue_T *nlist, صحيح nr_entries, صحيح flags, صحيح attr)
{
    صحيح		i;

    لكل (i = 0; i < nr_entries; ++i)
	إذا (flags & nlist[i].ڪنجي)
	{
	    نياپو_puts_attr((حرف *)nlist[i].قدر.string, attr);
	    نياپو_putchar(' ');
	}
}

/*
 * List one syntax cluster, for ":syntax" or "syntax list syntax_name".
 */
    ساكن فراغ
syn_list_cluster(صحيح id)
{
    صحيح	    endcol = 15;

    // slight hack:  roughly duplicate the guts of syn_list_header()
    نياپو_putchar('\n');
    نياپو_outtrans(SYN_CLSTR(curwin->w_s)[id].scl_name);

    إذا (نياپو_کانو >= endcol)	// output at least one space
	endcol = نياپو_کانو + 1;
    إذا (cmdline_ويڪر <= endcol)	// avoid hang for tiny window
	endcol = cmdline_ويڪر - 1;

    نياپو_advance(endcol);
    إذا (SYN_CLSTR(curwin->w_s)[id].scl_list != عدم)
    {
	put_id_list((char_u *)"cluster", SYN_CLSTR(curwin->w_s)[id].scl_list,
		    HL_ATTR(HLF_D));
    }
    وإلا
    {
	نياپو_puts_attr("cluster", HL_ATTR(HLF_D));
	نياپو_puts("=NONE");
    }
}

    ساكن فراغ
put_id_list(char_u *name, ننڍو *list, صحيح attr)
{
    ننڍو		*p;

    نياپو_puts_attr((حرف *)name, attr);
    نياپو_putchar('=');
    لكل (p = list; *p; ++p)
    {
	إذا (*p >= SYNID_ALLBUT && *p < SYNID_چوٽي)
	{
	    إذا (p[1])
		نياپو_puts("ALLBUT");
	    وإلا
		نياپو_puts("ALL");
	}
	وإلا إذا (*p >= SYNID_چوٽي && *p < SYNID_CONTAINED)
	{
	    نياپو_puts("TOP");
	}
	وإلا إذا (*p >= SYNID_CONTAINED && *p < SYNID_CLUSTER)
	{
	    نياپو_puts("CONTAINED");
	}
	وإلا إذا (*p >= SYNID_CLUSTER)
	{
	    ننڍو scl_id = *p - SYNID_CLUSTER;

	    نياپو_putchar('@');
	    نياپو_outtrans(SYN_CLSTR(curwin->w_s)[scl_id].scl_name);
	}
	وإلا
	    نياپو_outtrans(highlight_گروهه_name(*p - 1));
	إذا (p[1])
	    نياپو_putchar(',');
    }
    نياپو_putchar(' ');
}

    ساكن فراغ
put_نمونو(
    حرف	*s,
    صحيح		c,
    synpat_T	*spp,
    صحيح		attr)
{
    ڊگهو	n;
    صحيح		نقاب;
    صحيح		پهريون;
    ساكن حرف	*sepchars = "/+=-#@\"|'^&";
    صحيح		i;

    // May have to write "matchgroup=group"
    إذا (آخري_matchgroup != spp->sp_syn_ميلاپ_id)
    {
	آخري_matchgroup = spp->sp_syn_ميلاپ_id;
	نياپو_puts_attr("matchgroup", attr);
	نياپو_putchar('=');
	إذا (آخري_matchgroup == 0)
	    نياپو_outtrans((char_u *)"NONE");
	وإلا
	    نياپو_outtrans(highlight_گروهه_name(آخري_matchgroup - 1));
	نياپو_putchar(' ');
    }

    // output the name of the pattern and an '=' or ' '
    نياپو_puts_attr(s, attr);
    نياپو_putchar(c);

    // output the pattern, in between a char that is not in the pattern
    لكل (i = 0; vim_strchr(spp->sp_نمونو, sepchars[i]) != عدم; )
	إذا (sepchars[++i] == NUL)
	{
	    i = 0;	// no good char found, just use the first one
	    اكسر;
	}
    نياپو_putchar(sepchars[i]);
    نياپو_outtrans(spp->sp_نمونو);
    نياپو_putchar(sepchars[i]);

    // output any pattern options
    پهريون = TRUE;
    لكل (i = 0; i < SPO_ڳڻپ; ++i)
    {
	نقاب = (1 << i);
	إذا (spp->sp_off_flags & (نقاب + (نقاب << SPO_ڳڻپ)))
	{
	    إذا (!پهريون)
		نياپو_putchar(',');	// separate with commas
	    نياپو_puts(spo_name_tab[i]);
	    n = spp->sp_offsets[i];
	    إذا (i != SPO_LC_OFF)
	    {
		إذا (spp->sp_off_flags & نقاب)
		    نياپو_putchar('s');
		وإلا
		    نياپو_putchar('e');
		إذا (n > 0)
		    نياپو_putchar('+');
	    }
	    إذا (n || i == SPO_LC_OFF)
		نياپو_outnum(n);
	    پهريون = FALSE;
	}
    }
    نياپو_putchar(' ');
}

/*
 * List or clear the keywords for one syntax group.
 * Return TRUE if the header has been printed.
 */
    ساكن صحيح
syn_list_keywords(
    صحيح		id,
    hashtab_T	*ht,
    صحيح		did_سرو,		// header has already been printed
    صحيح		attr)
{
    صحيح		outlen;
    hashitem_T	*hi;
    keyentry_T	*kp;
    صحيح		todo;
    صحيح		prev_contained = 0;
    ننڍو	*prev_ايندڙ_list = عدم;
    ننڍو	*prev_cont_اندريون_list = عدم;
    صحيح		prev_skipnl = 0;
    صحيح		prev_skipwhite = 0;
    صحيح		prev_skipempty = 0;

    /*
     * Unfortunately, this list of keywords is not sorted on alphabet but on
     * hash value...
     */
    todo = (صحيح)ht->ht_استعمال_ٿيل;
    لكل (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	إذا (!HASHITEM_خالي_جايون(hi))
	{
	    --todo;
	    لكل (kp = HI2KE(hi); kp != عدم && !got_int; kp = kp->ke_ايندڙ)
	    {
		إذا (kp->k_syn.id == id)
		{
		    إذا (prev_contained != (kp->flags & HL_CONTAINED)
			    || prev_skipnl != (kp->flags & HL_SKIPNL)
			    || prev_skipwhite != (kp->flags & HL_SKIPWHITE)
			    || prev_skipempty != (kp->flags & HL_SKIPEMPTY)
			    || prev_cont_اندريون_list != kp->k_syn.cont_اندريون_list
			    || prev_ايندڙ_list != kp->ايندڙ_list)
			outlen = 9999;
		    وإلا
			outlen = (صحيح)STRLEN(kp->keyword);
		    // output "contained" and "nextgroup" on each line
		    إذا (syn_list_سرو(did_سرو, outlen, id))
		    {
			prev_contained = 0;
			prev_ايندڙ_list = عدم;
			prev_cont_اندريون_list = عدم;
			prev_skipnl = 0;
			prev_skipwhite = 0;
			prev_skipempty = 0;
		    }
		    did_سرو = TRUE;
		    إذا (prev_contained != (kp->flags & HL_CONTAINED))
		    {
			نياپو_puts_attr("contained", attr);
			نياپو_putchar(' ');
			prev_contained = (kp->flags & HL_CONTAINED);
		    }
		    إذا (kp->k_syn.cont_اندريون_list != prev_cont_اندريون_list)
		    {
			put_id_list((char_u *)"containedin",
						kp->k_syn.cont_اندريون_list, attr);
			نياپو_putchar(' ');
			prev_cont_اندريون_list = kp->k_syn.cont_اندريون_list;
		    }
		    إذا (kp->ايندڙ_list != prev_ايندڙ_list)
		    {
			put_id_list((char_u *)"nextgroup", kp->ايندڙ_list, attr);
			نياپو_putchar(' ');
			prev_ايندڙ_list = kp->ايندڙ_list;
			إذا (kp->flags & HL_SKIPNL)
			{
			    نياپو_puts_attr("skipnl", attr);
			    نياپو_putchar(' ');
			    prev_skipnl = (kp->flags & HL_SKIPNL);
			}
			إذا (kp->flags & HL_SKIPWHITE)
			{
			    نياپو_puts_attr("skipwhite", attr);
			    نياپو_putchar(' ');
			    prev_skipwhite = (kp->flags & HL_SKIPWHITE);
			}
			إذا (kp->flags & HL_SKIPEMPTY)
			{
			    نياپو_puts_attr("skipempty", attr);
			    نياپو_putchar(' ');
			    prev_skipempty = (kp->flags & HL_SKIPEMPTY);
			}
		    }
		    نياپو_outtrans(kp->keyword);
		}
	    }
	}
    }

    أرجع did_سرو;
}

    ساكن فراغ
syn_clear_keyword(صحيح id, hashtab_T *ht)
{
    hashitem_T	*hi;
    keyentry_T	*kp;
    keyentry_T	*kp_prev;
    keyentry_T	*kp_ايندڙ;
    صحيح		todo;

    hash_تالو(ht);
    todo = (صحيح)ht->ht_استعمال_ٿيل;
    FOR_ALL_HASHTAB_شيون(ht, hi, todo)
    {
	إذا (!HASHITEM_خالي_جايون(hi))
	{
	    --todo;
	    kp_prev = عدم;
	    لكل (kp = HI2KE(hi); kp != عدم; )
	    {
		إذا (kp->k_syn.id == id)
		{
		    kp_ايندڙ = kp->ke_ايندڙ;
		    إذا (kp_prev == عدم)
		    {
			إذا (kp_ايندڙ == عدم)
			    hash_remove(ht, hi, "syntax clear keyword");
			وإلا
			    hi->hi_ڪنجي = KE2HIKEY(kp_ايندڙ);
		    }
		    وإلا
			kp_prev->ke_ايندڙ = kp_ايندڙ;
		    vim_free(kp->ايندڙ_list);
		    vim_free(kp->k_syn.cont_اندريون_list);
		    vim_free(kp);
		    kp = kp_ايندڙ;
		}
		وإلا
		{
		    kp_prev = kp;
		    kp = kp->ke_ايندڙ;
		}
	    }
	}
    }
    hash_unlock(ht);
}

/*
 * Clear a whole keyword table.
 */
    ساكن فراغ
clear_keywtab(hashtab_T *ht)
{
    hashitem_T	*hi;
    صحيح		todo;
    keyentry_T	*kp;
    keyentry_T	*kp_ايندڙ;

    todo = (صحيح)ht->ht_استعمال_ٿيل;
    FOR_ALL_HASHTAB_شيون(ht, hi, todo)
    {
	إذا (!HASHITEM_خالي_جايون(hi))
	{
	    --todo;
	    لكل (kp = HI2KE(hi); kp != عدم; kp = kp_ايندڙ)
	    {
		kp_ايندڙ = kp->ke_ايندڙ;
		vim_free(kp->ايندڙ_list);
		vim_free(kp->k_syn.cont_اندريون_list);
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
    ساكن فراغ
جوڙيو_keyword(
    char_u	*name,	    // name of keyword
    size_t	namelen,    // length of keyword (excluding the NUL)
    صحيح		id,	    // group ID for this keyword
    صحيح		flags,	    // flags for this keyword
    ننڍو	*cont_اندريون_list, // containedin for this keyword
    ننڍو	*ايندڙ_list, // nextgroup for this keyword
    صحيح		conceal_char)
{
    keyentry_T	*kp;
    hashtab_T	*ht;
    hashitem_T	*hi;
    char_u	*name_ic;
    size_t	name_iclen;
    long_u	hash;
    char_u	name_folded[MAXKEYWLEN + 1];

    إذا (curwin->w_s->b_syn_ic)
    {
	name_ic = str_foldcase(name, (صحيح)namelen,
						 name_folded, MAXKEYWLEN + 1);
	name_iclen = STRLEN(name_ic);
    }
    وإلا
    {
	name_ic = name;
	name_iclen = namelen;
    }
    kp = alloc(offsetof(keyentry_T, keyword) + name_iclen + 1);
    إذا (kp == عدم)
	أرجع;
    STRCPY(kp->keyword, name_ic);
    kp->k_syn.id = id;
    kp->k_syn.inc_ٽيگ = current_syn_inc_ٽيگ;
    kp->flags = flags;
    kp->k_char = conceal_char;
    kp->k_syn.cont_اندريون_list = copy_id_list(cont_اندريون_list);
    إذا (cont_اندريون_list != عدم)
	curwin->w_s->b_syn_containedin = TRUE;
    kp->ايندڙ_list = copy_id_list(ايندڙ_list);

    إذا (curwin->w_s->b_syn_ic)
	ht = &curwin->w_s->b_keywtab_ic;
    وإلا
	ht = &curwin->w_s->b_keywtab;

    hash = hash_hash(kp->keyword);
    hi = hash_lookup(ht, kp->keyword, hash);
    إذا (HASHITEM_خالي_جايون(hi))
    {
	// new keyword, add to hashtable
	kp->ke_ايندڙ = عدم;
	hash_جوڙيو_شي(ht, hi, kp->keyword, hash);
    }
    وإلا
    {
	// keyword already exists, prepend to list
	kp->ke_ايندڙ = HI2KE(hi);
	hi->hi_ڪنجي = KE2HIKEY(kp);
    }
}

/*
 * Get the start and end of the group name argument.
 * Return a pointer to the first argument.
 * Return NULL if the end of the command was found instead of further args.
 */
    ساكن char_u *
get_گروهه_name(
    char_u	*arg,		// start of the argument
    char_u	**name_پڇاڙي)	// pointer to end of the name
{
    char_u	*rest;

    *name_پڇاڙي = skiptowhite(arg);
    rest = skipwhite(*name_پڇاڙي);

    /*
     * Check if there are enough arguments.  The first argument may be a
     * pattern, where '|' is allowed, so only check for NUL.
     */
    إذا (پڇاڙيون_excmd(*arg) || *rest == NUL)
	أرجع عدم;
    أرجع rest;
}

/*
 * Check for syntax command option arguments.
 * This can be called at any place in the list of arguments, and just picks
 * out the arguments that are known.  Can be called several times in a row to
 * collect all options in between other arguments.
 * Return a pointer to the next argument (which isn't an option).
 * Return NULL for any error;
 */
    ساكن char_u *
get_syn_options(
    char_u	    *شروعات,		// next argument to be checked
    syn_opt_arg_T   *opt,		// various things
    صحيح		    *conceal_char اڻ_استعمال_ٿيل,
    صحيح		    skip)		// TRUE if skipping over command
{
    char_u	*arg = شروعات;
    char_u	*gname_شروعات, *gname;
    صحيح		syn_id;
    صحيح		len;
    حرف	*p;
    صحيح		i;
    صحيح		fidx;
    ساكن بنية flag
    {
	حرف	*name;
	صحيح	argtype;
	صحيح	flags;
    } flagtab[] = { {"cCoOnNtTaAiInNeEdD",	0,	HL_CONTAINED},
		    {"oOnNeElLiInNeE",		0,	HL_ONELINE},
		    {"kKeEeEpPeEnNdD",		0,	HL_KEEPEND},
		    {"eExXtTeEnNdD",		0,	HL_EXTEND},
		    {"eExXcClLuUdDeEnNlL",	0,	HL_EXCLUDENL},
		    {"tTrRaAnNsSpPaArReEnNtT",	0,	HL_TRANSP},
		    {"sSkKiIpPnNlL",		0,	HL_SKIPNL},
		    {"sSkKiIpPwWhHiItTeE",	0,	HL_SKIPWHITE},
		    {"sSkKiIpPeEmMpPtTyY",	0,	HL_SKIPEMPTY},
		    {"gGrRoOuUpPhHeErReE",	0,	HL_SYNC_هتي},
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
    ساكن حرف *پهريون_letters = "cCoOkKeEtTsSgGdDfFnN";

    إذا (arg == عدم)		// already detected error
	أرجع عدم;

#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
    إذا (curwin->w_s->b_syn_conceal)
	opt->flags |= HL_CONCEAL;
#جيڪڏهن_پورو

    لكل (;;)
    {
	/*
	 * This is used very often when a large number of keywords is defined.
	 * Need to skip quickly when no option name is found.
	 * Also avoid tolower(), it's slow.
	 */
	إذا (strchr(پهريون_letters, *arg) == عدم)
	    اكسر;

	لكل (fidx = ARRAY_ڊيگهائي(flagtab); --fidx >= 0; )
	{
	    p = flagtab[fidx].name;
	    لكل (i = 0, len = 0; p[i] != NUL; i += 2, ++len)
		إذا (arg[len] != p[i] && arg[len] != p[i + 1])
		    اكسر;
	    إذا (p[i] == NUL && (VIM_ISWHITE(arg[len])
				    || (flagtab[fidx].argtype > 0
					 ? arg[len] == '='
					 : پڇاڙيون_excmd2(شروعات, arg + len))))
	    {
		إذا (opt->keyword
			&& (flagtab[fidx].flags == HL_DISPLAY
			    || flagtab[fidx].flags == HL_FOLD
			    || flagtab[fidx].flags == HL_EXTEND))
		    // treat "display", "fold" and "extend" as a keyword
		    fidx = -1;
		اكسر;
	    }
	}
	إذا (fidx < 0)	    // no match found
	    اكسر;

	إذا (flagtab[fidx].argtype == 1)
	{
	    إذا (!opt->has_cont_list)
	    {
		emsg(_(e_شامل_آهي_دليل_not_accepted_هتي));
		أرجع عدم;
	    }
	    إذا (get_id_list(&arg, 8, &opt->cont_list, skip) == FAIL)
		أرجع عدم;
	}
	وإلا إذا (flagtab[fidx].argtype == 2)
	{
	    إذا (get_id_list(&arg, 11, &opt->cont_اندريون_list, skip) == FAIL)
		أرجع عدم;
	}
	وإلا إذا (flagtab[fidx].argtype == 3)
	{
	    إذا (get_id_list(&arg, 9, &opt->ايندڙ_list, skip) == FAIL)
		أرجع عدم;
	}
	وإلا إذا (flagtab[fidx].argtype == 11 && arg[5] == '=')
	{
	    // cchar=?
	    إذا (has_mbyte)
	    {
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
		*conceal_char = mb_ptr2char(arg + 6);
#جيڪڏهن_پورو
		arg += mb_ptr2len(arg + 6) - 1;
	    }
	    وإلا
	    {
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
		*conceal_char = arg[6];
#وإلا
		;
#جيڪڏهن_پورو
	    }
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
	    إذا (!vim_isprintc_strict(*conceal_char))
	    {
		emsg(_(e_invalid_cchar_قدر));
		أرجع عدم;
	    }
#جيڪڏهن_پورو
	    arg = skipwhite(arg + 7);
	}
	وإلا
	{
	    opt->flags |= flagtab[fidx].flags;
	    arg = skipwhite(arg + len);

	    إذا (flagtab[fidx].flags == HL_SYNC_هتي
		    || flagtab[fidx].flags == HL_SYNC_THERE)
	    {
		إذا (opt->sync_idx == عدم)
		{
		    emsg(_(e_groupthere_not_accepted_هتي));
		    أرجع عدم;
		}
		gname_شروعات = arg;
		arg = skiptowhite(arg);
		إذا (gname_شروعات == arg)
		    أرجع عدم;
		gname = vim_strnsave(gname_شروعات, arg - gname_شروعات);
		إذا (gname == عدم)
		    أرجع عدم;
		إذا (STRCMP(gname, "NONE") == 0)
		    *opt->sync_idx = NONE_IDX;
		وإلا
		{
		    syn_id = syn_name2id(gname);
		    لكل (i = curwin->w_s->b_syn_patterns.ga_len; --i >= 0; )
			إذا (SYN_شيون(curwin->w_s)[i].sp_syn.id == syn_id
			      && SYN_شيون(curwin->w_s)[i].sp_type
							       == SPTYPE_شروعات)
			{
			    *opt->sync_idx = i;
			    اكسر;
			}
		    إذا (i < 0)
		    {
			semsg(_(e_didnt_ڳوليو_region_شي_for_str), gname);
			vim_free(gname);
			أرجع عدم;
		    }
		}

		vim_free(gname);
		arg = skipwhite(arg);
	    }
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	    وإلا إذا (flagtab[fidx].flags == HL_FOLD
						&& foldmethodIsSyntax(curwin))
		// Need to update folds later.
		foldUpdateAll(curwin);
#جيڪڏهن_پورو
	}
    }

    أرجع arg;
}

/*
 * Adjustments to syntax item when declared in a ":syn include"'d file.
 * Set the contained flag, and if the item is not already contained, add it
 * to the specified top-level group, if any.
 */
    ساكن فراغ
syn_incl_toplevel(صحيح id, صحيح *flagsp)
{
    إذا ((*flagsp & HL_CONTAINED) || curwin->w_s->b_syn_topgrp == 0)
	أرجع;
    *flagsp |= HL_CONTAINED | HL_INCLUDED_TOPLEVEL;
    إذا (curwin->w_s->b_syn_topgrp >= SYNID_CLUSTER)
    {
	// We have to alloc this, because syn_combine_list() will free it.
	ننڍو	    *grp_list = ALLOC_MULT(ننڍو, 2);
	صحيح	    tlg_id = curwin->w_s->b_syn_topgrp - SYNID_CLUSTER;

	إذا (grp_list != عدم)
	{
	    grp_list[0] = id;
	    grp_list[1] = 0;
	    syn_combine_list(&SYN_CLSTR(curwin->w_s)[tlg_id].scl_list,
						       &grp_list, CLUSTER_جوڙيو);
	}
    }
}

/*
 * Handle ":syntax include [@{group-name}] filename" command.
 */
    ساكن فراغ
syn_cmd_include(exarg_T *eap, صحيح syncing اڻ_استعمال_ٿيل)
{
    char_u	*arg = eap->arg;
    صحيح		sgl_id = 1;
    char_u	*گروهه_name_پڇاڙي;
    char_u	*rest;
    حرف	*errormsg = عدم;
    صحيح		prev_toplvl_grp;
    صحيح		prev_syn_inc_ٽيگ;
    صحيح		ذريعو = FALSE;

    eap->nextcmd = ڳوليو_nextcmd(arg);
    إذا (eap->skip)
	أرجع;

    إذا (arg[0] == '@')
    {
	++arg;
	rest = get_گروهه_name(arg, &گروهه_name_پڇاڙي);
	إذا (rest == عدم)
	{
	    emsg(_(e_filename_required));
	    أرجع;
	}
	sgl_id = syn_check_cluster(arg, (صحيح)(گروهه_name_پڇاڙي - arg));
	إذا (sgl_id == 0)
	    أرجع;
	// separate_nextcmd() and expand_filename() depend on this
	eap->arg = rest;
    }

    /*
     * Everything that's left, up to the next command, should be the
     * filename to include.
     */
    eap->argt |= (EX_XFILE | EX_NOSPC);
    separate_nextcmd(eap, FALSE);
    إذا (*eap->arg == '<' || *eap->arg == '$' || mch_is_ڀريل_Name(eap->arg))
    {
	// For an absolute path, "$VIM/..." or "<sfile>.." we ":source" the
	// file.  Need to expand the file name first.  In other cases
	// ":runtime!" is used.
	ذريعو = TRUE;
	إذا (expand_filename(eap, syn_cmdlinep, &errormsg) == FAIL)
	{
	    إذا (errormsg != عدم)
		emsg(errormsg);
	    أرجع;
	}
    }

    /*
     * Save and restore the existing top-level grouplist id and ":syn
     * include" tag around the actual inclusion.
     */
    إذا (running_syn_inc_ٽيگ >= MAX_SYN_INC_ٽيگ)
    {
	emsg(_(e_too_many_syntax_includes));
	أرجع;
    }
    prev_syn_inc_ٽيگ = current_syn_inc_ٽيگ;
    current_syn_inc_ٽيگ = ++running_syn_inc_ٽيگ;
    prev_toplvl_grp = curwin->w_s->b_syn_topgrp;
    curwin->w_s->b_syn_topgrp = sgl_id;
    إذا (ذريعو ? do_ذريعو(eap->arg, FALSE, DOSO_NONE, عدم) == FAIL
				: ذريعو_runtime(eap->arg, DIP_ALL) == FAIL)
	semsg(_(e_cant_open_file_str), eap->arg);
    curwin->w_s->b_syn_topgrp = prev_toplvl_grp;
    current_syn_inc_ٽيگ = prev_syn_inc_ٽيگ;
}

/*
 * Handle ":syntax keyword {group-name} [{option}] keyword .." command.
 */
    ساكن فراغ
syn_cmd_keyword(exarg_T *eap, صحيح syncing اڻ_استعمال_ٿيل)
{
    char_u	*arg = eap->arg;
    char_u	*گروهه_name_پڇاڙي;
    صحيح		syn_id;
    char_u	*rest;
    char_u	*keyword_copy = عدم;
    char_u	*p;
    char_u	*kw;
    syn_opt_arg_T syn_opt_arg;
    صحيح		cnt;
    صحيح		conceal_char = NUL;

    rest = get_گروهه_name(arg, &گروهه_name_پڇاڙي);

    إذا (rest != عدم)
    {
	إذا (eap->skip)
	    syn_id = -1;
	وإلا
	    syn_id = syn_check_گروهه(arg, (صحيح)(گروهه_name_پڇاڙي - arg));
	إذا (syn_id != 0)
	    // allocate a buffer, for removing backslashes in the keyword
	    keyword_copy = alloc(STRLEN(rest) + 1);
	إذا (keyword_copy != عدم)
	{
	    syn_opt_arg.flags = 0;
	    syn_opt_arg.keyword = TRUE;
	    syn_opt_arg.sync_idx = عدم;
	    syn_opt_arg.has_cont_list = FALSE;
	    syn_opt_arg.cont_اندريون_list = عدم;
	    syn_opt_arg.ايندڙ_list = عدم;

	    /*
	     * The options given apply to ALL keywords, so all options must be
	     * found before keywords can be created.
	     * 1: collect the options and copy the keywords to keyword_copy.
	     */
	    cnt = 0;
	    p = keyword_copy;
	    لكل ( ; rest != عدم && !پڇاڙيون_excmd2(eap->arg, rest);
							rest = skipwhite(rest))
	    {
		rest = get_syn_options(rest, &syn_opt_arg, &conceal_char,
								    eap->skip);
		إذا (rest == عدم || پڇاڙيون_excmd2(eap->arg, rest))
		    اكسر;
		// Copy the keyword, removing backslashes, and add a NUL.
		طالما (*rest != NUL && !VIM_ISWHITE(*rest))
		{
		    إذا (*rest == '\\' && rest[1] != NUL)
			++rest;
		    *p++ = *rest++;
		}
		*p++ = NUL;
		++cnt;
	    }

	    إذا (!eap->skip)
	    {
		size_t	kwlen = 0;

		// Adjust flags for use of ":syn include".
		syn_incl_toplevel(syn_id, &syn_opt_arg.flags);

		/*
		 * 2: Add an entry for each keyword.
		 */
		لكل (kw = keyword_copy; --cnt >= 0; kw += kwlen + 1)
		{
		    لكل (p = vim_strchr(kw, '['); ; )
		    {
			إذا (p == عدم)
			    kwlen = STRLEN(kw);
			وإلا
			{
			    *p = NUL;
			    kwlen = (size_t)(p - kw);
			}
			جوڙيو_keyword(kw, kwlen, syn_id, syn_opt_arg.flags,
				syn_opt_arg.cont_اندريون_list,
					 syn_opt_arg.ايندڙ_list, conceal_char);
			إذا (p == عدم)
			    اكسر;
			إذا (p[1] == NUL)
			{
			    semsg(_(e_error_missing_rsb_str), kw);
			    اذهب error;
			}
			إذا (p[1] == ']')
			{
			    إذا (p[2] != NUL)
			    {
				semsg(_(e_trailing_char_after_rsb_str_str),
								    kw, &p[2]);
				اذهب error;
			    }
			    kw = p + 1;		// skip over the "]"
			    kwlen = 1;
			    اكسر;
			}
			إذا (has_mbyte)
			{
			    صحيح l = (*mb_ptr2len)(p + 1);

			    mch_memmove(p, p + 1, l);
			    p += l;
			}
			وإلا
			{
			    p[0] = p[1];
			    ++p;
			}
		    }
		}
	    }
error:
	    vim_free(keyword_copy);
	    vim_free(syn_opt_arg.cont_اندريون_list);
	    vim_free(syn_opt_arg.ايندڙ_list);
	}
    }

    إذا (rest != عدم)
	سيٽ_nextcmd(eap, rest);
    وإلا
	semsg(_(e_invalid_دليل_str), arg);

    redraw_curbuf_later(UPD_SOME_VALID);
    syn_ڍير_free_all(curwin->w_s);		// Need to recompute all syntax.
}

/*
 * Handle ":syntax match {name} [{options}] {pattern} [{options}]".
 *
 * Also ":syntax sync match {name} [[grouphere | groupthere] {group-name}] .."
 */
    ساكن فراغ
syn_cmd_ميلاپ(
    exarg_T	*eap,
    صحيح		syncing)	    // TRUE for ":syntax sync match .. "
{
    char_u	*arg = eap->arg;
    char_u	*گروهه_name_پڇاڙي;
    char_u	*rest;
    synpat_T	شي;		// the item found in the line
    صحيح		syn_id;
    صحيح		idx;
    syn_opt_arg_T syn_opt_arg;
    صحيح		sync_idx = 0;
    صحيح		conceal_char = NUL;
    صحيح		orig_called_emsg = called_emsg;

    // Isolate the group name, check for validity
    rest = get_گروهه_name(arg, &گروهه_name_پڇاڙي);

    // Get options before the pattern
    syn_opt_arg.flags = 0;
    syn_opt_arg.keyword = FALSE;
    syn_opt_arg.sync_idx = syncing ? &sync_idx : عدم;
    syn_opt_arg.has_cont_list = TRUE;
    syn_opt_arg.cont_list = عدم;
    syn_opt_arg.cont_اندريون_list = عدم;
    syn_opt_arg.ايندڙ_list = عدم;
    rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);

    // get the pattern.
    init_syn_patterns();
    CLEAR_ميدان(شي);
    rest = get_syn_نمونو(rest, &شي);
    إذا (vim_regcomp_had_eol() && !(syn_opt_arg.flags & HL_EXCLUDENL))
	syn_opt_arg.flags |= HL_HAS_EOL;

    // Get options after the pattern
    rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);

    إذا (rest != عدم)		// all arguments are valid
    {
	/*
	 * Check for trailing command and illegal trailing arguments.
	 */
	سيٽ_nextcmd(eap, rest);
	إذا (!پڇاڙيون_excmd2(eap->cmd, rest) || eap->skip)
	    rest = عدم;
	وإلا إذا (ga_grow(&curwin->w_s->b_syn_patterns, 1) == OK
		&& (syn_id = syn_check_گروهه(arg,
					   (صحيح)(گروهه_name_پڇاڙي - arg))) != 0)
	{
	    syn_incl_toplevel(syn_id, &syn_opt_arg.flags);
	    /*
	     * Store the pattern in the syn_items list
	     */
	    idx = curwin->w_s->b_syn_patterns.ga_len;
	    SYN_شيون(curwin->w_s)[idx] = شي;
	    SYN_شيون(curwin->w_s)[idx].sp_syncing = syncing;
	    SYN_شيون(curwin->w_s)[idx].sp_type = SPTYPE_ميلاپ;
	    SYN_شيون(curwin->w_s)[idx].sp_syn.id = syn_id;
	    SYN_شيون(curwin->w_s)[idx].sp_syn.inc_ٽيگ = current_syn_inc_ٽيگ;
	    SYN_شيون(curwin->w_s)[idx].sp_flags = syn_opt_arg.flags;
	    SYN_شيون(curwin->w_s)[idx].sp_sync_idx = sync_idx;
	    SYN_شيون(curwin->w_s)[idx].sp_cont_list = syn_opt_arg.cont_list;
	    SYN_شيون(curwin->w_s)[idx].sp_syn.cont_اندريون_list =
						     syn_opt_arg.cont_اندريون_list;
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
	    SYN_شيون(curwin->w_s)[idx].sp_cchar = conceal_char;
#جيڪڏهن_پورو
	    إذا (syn_opt_arg.cont_اندريون_list != عدم)
		curwin->w_s->b_syn_containedin = TRUE;
	    SYN_شيون(curwin->w_s)[idx].sp_ايندڙ_list = syn_opt_arg.ايندڙ_list;
	    ++curwin->w_s->b_syn_patterns.ga_len;

	    // remember that we found a match for syncing on
	    إذا (syn_opt_arg.flags & (HL_SYNC_هتي|HL_SYNC_THERE))
		curwin->w_s->b_syn_sync_flags |= SF_ميلاپ;
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	    إذا (syn_opt_arg.flags & HL_FOLD)
		++curwin->w_s->b_syn_folditems;
#جيڪڏهن_پورو

	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_ڍير_free_all(curwin->w_s);	// Need to recompute all syntax.
	    أرجع;	// don't free the progs and patterns now
	}
    }

    /*
     * Something failed, free the allocated memory.
     */
    vim_regfree(شي.sp_prog);
    vim_free(شي.sp_نمونو);
    vim_free(syn_opt_arg.cont_list);
    vim_free(syn_opt_arg.cont_اندريون_list);
    vim_free(syn_opt_arg.ايندڙ_list);

    إذا (rest == عدم && called_emsg == orig_called_emsg)
	semsg(_(e_invalid_دليل_str), arg);
}

/*
 * Handle ":syntax region {group-name} [matchgroup={group-name}]
 *		start {start} .. [skip {skip}] end {end} .. [{options}]".
 */
    ساكن فراغ
syn_cmd_region(
    exarg_T	*eap,
    صحيح		syncing)	    // TRUE for ":syntax sync region .."
{
    char_u		*arg = eap->arg;
    char_u		*گروهه_name_پڇاڙي;
    char_u		*rest;			// next arg, NULL on error
    char_u		*ڪنجي_پڇاڙي;
    char_u		*ڪنجي = عدم;
    char_u		*p;
    صحيح			شي;
#وصف_ڪريو شي_شروعات_2	    0
#وصف_ڪريو شي_SKIP	    1
#وصف_ڪريو شي_پڇاڙي_2	    2
#وصف_ڪريو شي_MATCHGROUP	    3
    بنية pat_ptr
    {
	synpat_T	*pp_synp;		// pointer to syn_pattern
	صحيح		pp_matchgroup_id;	// matchgroup ID
	بنية pat_ptr	*pp_ايندڙ;		// pointer to next pat_ptr
    }			*(pat_ptrs[3]);
					// patterns found in the line
    بنية pat_ptr	*ppp;
    بنية pat_ptr	*ppp_ايندڙ;
    صحيح			pat_ڳڻپ = 0;		// nr of syn_patterns found
    صحيح			syn_id;
    صحيح			matchgroup_id = 0;
    صحيح			not_enough = FALSE;	// not enough arguments
    صحيح			illegal = FALSE;	// illegal arguments
    صحيح			success = FALSE;
    صحيح			idx;
    syn_opt_arg_T	syn_opt_arg;
    صحيح			conceal_char = NUL;

    // Isolate the group name, check for validity
    rest = get_گروهه_name(arg, &گروهه_name_پڇاڙي);

    pat_ptrs[0] = عدم;
    pat_ptrs[1] = عدم;
    pat_ptrs[2] = عدم;

    init_syn_patterns();

    syn_opt_arg.flags = 0;
    syn_opt_arg.keyword = FALSE;
    syn_opt_arg.sync_idx = عدم;
    syn_opt_arg.has_cont_list = TRUE;
    syn_opt_arg.cont_list = عدم;
    syn_opt_arg.cont_اندريون_list = عدم;
    syn_opt_arg.ايندڙ_list = عدم;

    /*
     * get the options, patterns and matchgroup.
     */
    طالما (rest != عدم && !پڇاڙيون_excmd2(eap->cmd, rest))
    {
	// Check for option arguments
	rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);
	إذا (rest == عدم || پڇاڙيون_excmd2(eap->cmd, rest))
	    اكسر;

	// must be a pattern or matchgroup then
	ڪنجي_پڇاڙي = rest;
	طالما (*ڪنجي_پڇاڙي && !VIM_ISWHITE(*ڪنجي_پڇاڙي) && *ڪنجي_پڇاڙي != '=')
	    ++ڪنجي_پڇاڙي;
	vim_free(ڪنجي);
	ڪنجي = vim_strnsave_مٿي(rest, ڪنجي_پڇاڙي - rest);
	إذا (ڪنجي == عدم)			// out of memory
	{
	    rest = عدم;
	    اكسر;
	}
	إذا (STRCMP(ڪنجي, "MATCHGROUP") == 0)
	    شي = شي_MATCHGROUP;
	وإلا إذا (STRCMP(ڪنجي, "START") == 0)
	    شي = شي_شروعات_2;
	وإلا إذا (STRCMP(ڪنجي, "END") == 0)
	    شي = شي_پڇاڙي_2;
	وإلا إذا (STRCMP(ڪنجي, "SKIP") == 0)
	{
	    إذا (pat_ptrs[شي_SKIP] != عدم)	// one skip pattern allowed
	    {
		illegal = TRUE;
		اكسر;
	    }
	    شي = شي_SKIP;
	}
	وإلا
	    اكسر;
	rest = skipwhite(ڪنجي_پڇاڙي);
	إذا (*rest != '=')
	{
	    rest = عدم;
	    semsg(_(e_missing_equal_str), arg);
	    اكسر;
	}
	rest = skipwhite(rest + 1);
	إذا (*rest == NUL)
	{
	    not_enough = TRUE;
	    اكسر;
	}

	إذا (شي == شي_MATCHGROUP)
	{
	    p = skiptowhite(rest);
	    إذا ((p - rest == 4 && STRNCMP(rest, "NONE", 4) == 0) || eap->skip)
		matchgroup_id = 0;
	    وإلا
	    {
		matchgroup_id = syn_check_گروهه(rest, (صحيح)(p - rest));
		إذا (matchgroup_id == 0)
		{
		    illegal = TRUE;
		    اكسر;
		}
	    }
	    rest = skipwhite(p);
	}
	وإلا
	{
	    /*
	     * Allocate room for a syn_pattern, and link it in the list of
	     * syn_patterns for this item, at the start (because the list is
	     * used from end to start).
	     */
	    ppp = ALLOC_ONE(بنية pat_ptr);
	    إذا (ppp == عدم)
	    {
		rest = عدم;
		اكسر;
	    }
	    ppp->pp_ايندڙ = pat_ptrs[شي];
	    pat_ptrs[شي] = ppp;
	    ppp->pp_synp = ALLOC_CLEAR_ONE(synpat_T);
	    إذا (ppp->pp_synp == عدم)
	    {
		rest = عدم;
		اكسر;
	    }

	    /*
	     * Get the syntax pattern and the following offset(s).
	     */
	    // Enable the appropriate \z specials.
	    إذا (شي == شي_شروعات_2)
		reg_do_extmatch = REX_سيٽ;
	    وإلا إذا (شي == شي_SKIP || شي == شي_پڇاڙي_2)
		reg_do_extmatch = REX_USE;
	    rest = get_syn_نمونو(rest, ppp->pp_synp);
	    reg_do_extmatch = 0;
	    إذا (شي == شي_پڇاڙي_2 && vim_regcomp_had_eol()
				       && !(syn_opt_arg.flags & HL_EXCLUDENL))
		ppp->pp_synp->sp_flags |= HL_HAS_EOL;
	    ppp->pp_matchgroup_id = matchgroup_id;
	    ++pat_ڳڻپ;
	}
    }
    vim_free(ڪنجي);
    إذا (illegal || not_enough)
	rest = عدم;

    /*
     * Must have a "start" and "end" pattern.
     */
    إذا (rest != عدم && (pat_ptrs[شي_شروعات_2] == عدم ||
						  pat_ptrs[شي_پڇاڙي_2] == عدم))
    {
	not_enough = TRUE;
	rest = عدم;
    }

    إذا (rest != عدم)
    {
	/*
	 * Check for trailing garbage or command.
	 * If OK, add the item.
	 */
	سيٽ_nextcmd(eap, rest);
	إذا (!پڇاڙيون_excmd(*rest) || eap->skip)
	    rest = عدم;
	وإلا إذا (ga_grow(&(curwin->w_s->b_syn_patterns), pat_ڳڻپ) == OK
		&& (syn_id = syn_check_گروهه(arg,
					   (صحيح)(گروهه_name_پڇاڙي - arg))) != 0)
	{
	    syn_incl_toplevel(syn_id, &syn_opt_arg.flags);
	    /*
	     * Store the start/skip/end in the syn_items list
	     */
	    idx = curwin->w_s->b_syn_patterns.ga_len;
	    لكل (شي = شي_شروعات_2; شي <= شي_پڇاڙي_2; ++شي)
	    {
		لكل (ppp = pat_ptrs[شي]; ppp != عدم; ppp = ppp->pp_ايندڙ)
		{
		    SYN_شيون(curwin->w_s)[idx] = *(ppp->pp_synp);
		    SYN_شيون(curwin->w_s)[idx].sp_syncing = syncing;
		    SYN_شيون(curwin->w_s)[idx].sp_type =
			    (شي == شي_شروعات_2) ? SPTYPE_شروعات :
			    (شي == شي_SKIP) ? SPTYPE_SKIP : SPTYPE_پڇاڙي;
		    SYN_شيون(curwin->w_s)[idx].sp_flags |= syn_opt_arg.flags;
		    SYN_شيون(curwin->w_s)[idx].sp_syn.id = syn_id;
		    SYN_شيون(curwin->w_s)[idx].sp_syn.inc_ٽيگ =
							  current_syn_inc_ٽيگ;
		    SYN_شيون(curwin->w_s)[idx].sp_syn_ميلاپ_id =
							ppp->pp_matchgroup_id;
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
		    SYN_شيون(curwin->w_s)[idx].sp_cchar = conceal_char;
#جيڪڏهن_پورو
		    إذا (شي == شي_شروعات_2)
		    {
			SYN_شيون(curwin->w_s)[idx].sp_cont_list =
							syn_opt_arg.cont_list;
			SYN_شيون(curwin->w_s)[idx].sp_syn.cont_اندريون_list =
						     syn_opt_arg.cont_اندريون_list;
			إذا (syn_opt_arg.cont_اندريون_list != عدم)
			    curwin->w_s->b_syn_containedin = TRUE;
			SYN_شيون(curwin->w_s)[idx].sp_ايندڙ_list =
							syn_opt_arg.ايندڙ_list;
		    }
		    ++curwin->w_s->b_syn_patterns.ga_len;
		    ++idx;
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
		    إذا (syn_opt_arg.flags & HL_FOLD)
			++curwin->w_s->b_syn_folditems;
#جيڪڏهن_پورو
		}
	    }

	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_ڍير_free_all(curwin->w_s);	// Need to recompute all syntax.
	    success = TRUE;	    // don't free the progs and patterns now
	}
    }

    /*
     * Free the allocated memory.
     */
    لكل (شي = شي_شروعات_2; شي <= شي_پڇاڙي_2; ++شي)
	لكل (ppp = pat_ptrs[شي]; ppp != عدم; ppp = ppp_ايندڙ)
	{
	    إذا (!success && ppp->pp_synp != عدم)
	    {
		vim_regfree(ppp->pp_synp->sp_prog);
		vim_free(ppp->pp_synp->sp_نمونو);
	    }
	    vim_free(ppp->pp_synp);
	    ppp_ايندڙ = ppp->pp_ايندڙ;
	    vim_free(ppp);
	}

    إذا (!success)
    {
	vim_free(syn_opt_arg.cont_list);
	vim_free(syn_opt_arg.cont_اندريون_list);
	vim_free(syn_opt_arg.ايندڙ_list);
	إذا (not_enough)
	    semsg(_(e_not_enough_arguments_syntax_region_str), arg);
	وإلا إذا (illegal || rest == عدم)
	    semsg(_(e_invalid_دليل_str), arg);
    }
}

/*
 * A simple syntax group ID comparison function suitable for use in qsort()
 */
    ساكن صحيح
syn_compare_stub(ثابت فراغ *v1, ثابت فراغ *v2)
{
    ثابت ننڍو	*s1 = v1;
    ثابت ننڍو	*s2 = v2;

    أرجع (*s1 > *s2 ? 1 : *s1 < *s2 ? -1 : 0);
}

/*
 * Combines lists of syntax clusters.
 * *clstr1 and *clstr2 must both be allocated memory; they will be consumed.
 */
    ساكن فراغ
syn_combine_list(ننڍو **clstr1, ننڍو **clstr2, صحيح list_op)
{
    صحيح		count1 = 0;
    صحيح		count2 = 0;
    ننڍو	*g1;
    ننڍو	*g2;
    ننڍو	*clstr = عدم;
    صحيح		ڳڻپ;
    صحيح		ڦيرو;

    /*
     * Handle degenerate cases.
     */
    إذا (*clstr2 == عدم)
	أرجع;
    إذا (*clstr1 == عدم || list_op == CLUSTER_REPLACE)
    {
	إذا (list_op == CLUSTER_REPLACE)
	    vim_free(*clstr1);
	إذا (list_op == CLUSTER_REPLACE || list_op == CLUSTER_جوڙيو)
	    *clstr1 = *clstr2;
	وإلا
	    vim_free(*clstr2);
	أرجع;
    }

    لكل (g1 = *clstr1; *g1; g1++)
	++count1;
    لكل (g2 = *clstr2; *g2; g2++)
	++count2;

    /*
     * For speed purposes, sort both lists.
     */
    qsort(*clstr1, (size_t)count1, حجم(ننڍو), syn_compare_stub);
    qsort(*clstr2, (size_t)count2, حجم(ننڍو), syn_compare_stub);

    /*
     * We proceed in two passes; in round 1, we count the elements to place
     * in the new list, and in round 2, we allocate and populate the new
     * list.  For speed, we use a mergesort-like method, adding the smaller
     * of the current elements in each list to the new list.
     */
    لكل (ڦيرو = 1; ڦيرو <= 2; ڦيرو++)
    {
	g1 = *clstr1;
	g2 = *clstr2;
	ڳڻپ = 0;

	/*
	 * First, loop through the lists until one of them is empty.
	 */
	طالما (*g1 && *g2)
	{
	    /*
	     * We always want to add from the first list.
	     */
	    إذا (*g1 < *g2)
	    {
		إذا (ڦيرو == 2)
		    clstr[ڳڻپ] = *g1;
		ڳڻپ++;
		g1++;
		استمر;
	    }
	    /*
	     * We only want to add from the second list if we're adding the
	     * lists.
	     */
	    إذا (list_op == CLUSTER_جوڙيو)
	    {
		إذا (ڦيرو == 2)
		    clstr[ڳڻپ] = *g2;
		ڳڻپ++;
	    }
	    إذا (*g1 == *g2)
		g1++;
	    g2++;
	}

	/*
	 * Now add the leftovers from whichever list didn't get finished
	 * first.  As before, we only want to add from the second list if
	 * we're adding the lists.
	 */
	لكل (; *g1; g1++, ڳڻپ++)
	    إذا (ڦيرو == 2)
		clstr[ڳڻپ] = *g1;
	إذا (list_op == CLUSTER_جوڙيو)
	    لكل (; *g2; g2++, ڳڻپ++)
		إذا (ڦيرو == 2)
		    clstr[ڳڻپ] = *g2;

	إذا (ڦيرو == 1)
	{
	    /*
	     * If the group ended up empty, we don't need to allocate any
	     * space for it.
	     */
	    إذا (ڳڻپ == 0)
	    {
		clstr = عدم;
		اكسر;
	    }
	    clstr = ALLOC_MULT(ننڍو, ڳڻپ + 1);
	    إذا (clstr == عدم)
		اكسر;
	    clstr[ڳڻپ] = 0;
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
    ساكن صحيح
syn_scl_name2id(char_u *name)
{
    صحيح		i;
    char_u	*name_u;

    // Avoid using stricmp() too much, it's slow on some systems
    name_u = vim_strsave_مٿي(name);
    إذا (name_u == عدم)
	أرجع 0;
    لكل (i = curwin->w_s->b_syn_clusters.ga_len; --i >= 0; )
	إذا (SYN_CLSTR(curwin->w_s)[i].scl_name_u != عدم
		&& STRCMP(name_u, SYN_CLSTR(curwin->w_s)[i].scl_name_u) == 0)
	    اكسر;
    vim_free(name_u);
    أرجع (i < 0 ? 0 : i + SYNID_CLUSTER);
}

/*
 * Like syn_scl_name2id(), but take a pointer + length argument.
 */
    ساكن صحيح
syn_scl_namen2id(char_u *linep, صحيح len)
{
    char_u  *name;
    صحيح	    id = 0;

    name = vim_strnsave(linep, len);
    إذا (name == عدم)
	أرجع 0;

    id = syn_scl_name2id(name);
    vim_free(name);
    أرجع id;
}

/*
 * Find syntax cluster name in the table and return its ID.
 * The argument is a pointer to the name and the length of the name.
 * If it doesn't exist yet, a new entry is created.
 * Return 0 for failure.
 */
    ساكن صحيح
syn_check_cluster(char_u *pp, صحيح len)
{
    صحيح		id;
    char_u	*name;

    name = vim_strnsave(pp, len);
    إذا (name == عدم)
	أرجع 0;

    id = syn_scl_name2id(name);
    إذا (id == 0)			// doesn't exist yet
	id = syn_جوڙيو_cluster(name);
    وإلا
	vim_free(name);
    أرجع id;
}

/*
 * Add new syntax cluster and return its ID.
 * "name" must be an allocated string, it will be consumed.
 * Return 0 for failure.
 */
    ساكن صحيح
syn_جوڙيو_cluster(char_u *name)
{
    صحيح		len;

    /*
     * First call for this growarray: init growing array.
     */
    إذا (curwin->w_s->b_syn_clusters.ga_معلومات == عدم)
    {
	curwin->w_s->b_syn_clusters.ga_itemsize = حجم(syn_cluster_T);
	curwin->w_s->b_syn_clusters.ga_growsize = 10;
    }

    len = curwin->w_s->b_syn_clusters.ga_len;
    إذا (len >= MAX_CLUSTER_ID)
    {
	emsg(_(e_too_many_syntax_clusters));
	vim_free(name);
	أرجع 0;
    }

    /*
     * Make room for at least one other cluster entry.
     */
    إذا (ga_grow(&curwin->w_s->b_syn_clusters, 1) == FAIL)
    {
	vim_free(name);
	أرجع 0;
    }

    CLEAR_اشارو(&(SYN_CLSTR(curwin->w_s)[len]));
    SYN_CLSTR(curwin->w_s)[len].scl_name = name;
    SYN_CLSTR(curwin->w_s)[len].scl_name_u = vim_strsave_مٿي(name);
    SYN_CLSTR(curwin->w_s)[len].scl_list = عدم;
    ++curwin->w_s->b_syn_clusters.ga_len;

    إذا (STRICMP(name, "Spell") == 0)
	curwin->w_s->b_spell_cluster_id = len + SYNID_CLUSTER;
    إذا (STRICMP(name, "NoSpell") == 0)
	curwin->w_s->b_nospell_cluster_id = len + SYNID_CLUSTER;

    أرجع len + SYNID_CLUSTER;
}

/*
 * Handle ":syntax cluster {cluster-name} [contains={groupname},..]
 *		[add={groupname},..] [remove={groupname},..]".
 */
    ساكن فراغ
syn_cmd_cluster(exarg_T *eap, صحيح syncing اڻ_استعمال_ٿيل)
{
    char_u	*arg = eap->arg;
    char_u	*گروهه_name_پڇاڙي;
    char_u	*rest;
    صحيح		scl_id;
    ننڍو	*clstr_list;
    صحيح		got_clstr = FALSE;
    صحيح		opt_len;
    صحيح		list_op;

    eap->nextcmd = ڳوليو_nextcmd(arg);
    إذا (eap->skip)
	أرجع;

    rest = get_گروهه_name(arg, &گروهه_name_پڇاڙي);

    إذا (rest != عدم)
    {
	scl_id = syn_check_cluster(arg, (صحيح)(گروهه_name_پڇاڙي - arg));
	إذا (scl_id == 0)
	    أرجع;
	scl_id -= SYNID_CLUSTER;

	لكل (;;)
	{
	    إذا (STRNICMP(rest, "add", 3) == 0
		    && (VIM_ISWHITE(rest[3]) || rest[3] == '='))
	    {
		opt_len = 3;
		list_op = CLUSTER_جوڙيو;
	    }
	    وإلا إذا (STRNICMP(rest, "remove", 6) == 0
		    && (VIM_ISWHITE(rest[6]) || rest[6] == '='))
	    {
		opt_len = 6;
		list_op = CLUSTER_ڪٽيو;
	    }
	    وإلا إذا (STRNICMP(rest, "contains", 8) == 0
			&& (VIM_ISWHITE(rest[8]) || rest[8] == '='))
	    {
		opt_len = 8;
		list_op = CLUSTER_REPLACE;
	    }
	    وإلا
		اكسر;

	    clstr_list = عدم;
	    إذا (get_id_list(&rest, opt_len, &clstr_list, eap->skip) == FAIL)
	    {
		semsg(_(e_invalid_دليل_str), rest);
		اكسر;
	    }
	    إذا (scl_id >= 0)
		syn_combine_list(&SYN_CLSTR(curwin->w_s)[scl_id].scl_list,
			     &clstr_list, list_op);
	    وإلا
		vim_free(clstr_list);
	    got_clstr = TRUE;
	}

	إذا (got_clstr)
	{
	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_ڍير_free_all(curwin->w_s);	// Need to recompute all.
	}
    }

    إذا (!got_clstr)
	emsg(_(e_no_cluster_specified));
    إذا (rest == عدم || !پڇاڙيون_excmd2(eap->cmd, rest))
	semsg(_(e_invalid_دليل_str), arg);
}

/*
 * On first call for current buffer: Init growing array.
 */
    ساكن فراغ
init_syn_patterns(فراغ)
{
    curwin->w_s->b_syn_patterns.ga_itemsize = حجم(synpat_T);
    curwin->w_s->b_syn_patterns.ga_growsize = 10;
}

/*
 * Get one pattern for a ":syntax match" or ":syntax region" command.
 * Stores the pattern and program in a synpat_T.
 * Returns a pointer to the next argument, or NULL in case of an error.
 */
    ساكن char_u *
get_syn_نمونو(char_u *arg, synpat_T *ci)
{
    char_u	*پڇاڙي;
    صحيح		*p;
    صحيح		idx;
    char_u	*cpo_save;

    // need at least three chars
    إذا (arg == عدم || arg[0] == NUL || arg[1] == NUL || arg[2] == NUL)
	أرجع عدم;

    پڇاڙي = skip_regexp(arg + 1, *arg, TRUE);
    إذا (*پڇاڙي != *arg)			    // end delimiter not found
    {
	semsg(_(e_نمونو_delimiter_not_مليو_str), arg);
	أرجع عدم;
    }
    // store the pattern and compiled regexp program
    إذا ((ci->sp_نمونو = vim_strnsave(arg + 1, پڇاڙي - arg - 1)) == عدم)
	أرجع عدم;

    // Make 'cpoptions' empty, to avoid the 'l' flag
    cpo_save = p_cpo;
    p_cpo = خالي_جايون_option;
    ci->sp_prog = vim_regcomp(ci->sp_نمونو, RE_MAGIC);
    p_cpo = cpo_save;

    إذا (ci->sp_prog == عدم)
	أرجع عدم;
    ci->sp_ic = curwin->w_s->b_syn_ic;
#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
    syn_clear_وقت(&ci->sp_وقت);
#جيڪڏهن_پورو

    /*
     * Check for a match, highlight or region offset.
     */
    ++پڇاڙي;
    افعل
    {
	لكل (idx = SPO_ڳڻپ; --idx >= 0; )
	    إذا (STRNCMP(پڇاڙي, spo_name_tab[idx], 3) == 0)
		اكسر;
	إذا (idx >= 0)
	{
	    p = &(ci->sp_offsets[idx]);
	    إذا (idx != SPO_LC_OFF)
		بدل (پڇاڙي[3])
		{
		    حالة 's':   اكسر;
		    حالة 'b':   اكسر;
		    حالة 'e':   idx += SPO_ڳڻپ; اكسر;
		    افتراضي:    idx = -1; اكسر;
		}
	    إذا (idx >= 0)
	    {
		ci->sp_off_flags |= (1 << idx);
		إذا (idx == SPO_LC_OFF)	    // lc=99
		{
		    پڇاڙي += 3;
		    *p = getdigits(&پڇاڙي);

		    // "lc=" offset automatically sets "ms=" offset
		    إذا (!(ci->sp_off_flags & (1 << SPO_MS_OFF)))
		    {
			ci->sp_off_flags |= (1 << SPO_MS_OFF);
			ci->sp_offsets[SPO_MS_OFF] = *p;
		    }
		}
		وإلا			    // yy=x+99
		{
		    پڇاڙي += 4;
		    إذا (*پڇاڙي == '+')
		    {
			++پڇاڙي;
			*p = getdigits(&پڇاڙي);		// positive offset
		    }
		    وإلا إذا (*پڇاڙي == '-')
		    {
			++پڇاڙي;
			*p = -getdigits(&پڇاڙي);		// negative offset
		    }
		}
		إذا (*پڇاڙي != ',')
		    اكسر;
		++پڇاڙي;
	    }
	}
    } طالما (idx >= 0);

    إذا (!پڇاڙيون_excmd2(arg, پڇاڙي) && !VIM_ISWHITE(*پڇاڙي))
    {
	semsg(_(e_garbage_after_نمونو_str), arg);
	أرجع عدم;
    }
    أرجع skipwhite(پڇاڙي);
}

/*
 * Handle ":syntax sync .." command.
 */
    ساكن فراغ
syn_cmd_sync(exarg_T *eap, صحيح syncing اڻ_استعمال_ٿيل)
{
    char_u	*arg_شروعات = eap->arg;
    char_u	*arg_پڇاڙي;
    char_u	*ڪنجي = عدم;
    char_u	*ايندڙ_arg;
    صحيح		illegal = FALSE;
    صحيح		ختم = FALSE;
    ڊگهو	n;
    char_u	*cpo_save;

    إذا (پڇاڙيون_excmd2(eap->cmd, arg_شروعات))
    {
	syn_cmd_list(eap, TRUE);
	أرجع;
    }

    طالما (!پڇاڙيون_excmd2(eap->cmd, arg_شروعات))
    {
	arg_پڇاڙي = skiptowhite(arg_شروعات);
	ايندڙ_arg = skipwhite(arg_پڇاڙي);
	vim_free(ڪنجي);
	ڪنجي = vim_strnsave_مٿي(arg_شروعات, arg_پڇاڙي - arg_شروعات);
	إذا (ڪنجي == عدم)
	    اكسر;
	إذا (STRCMP(ڪنجي, "CCOMMENT") == 0)
	{
	    إذا (!eap->skip)
		curwin->w_s->b_syn_sync_flags |= SF_CCOMMENT;
	    إذا (!پڇاڙيون_excmd2(eap->cmd, ايندڙ_arg))
	    {
		arg_پڇاڙي = skiptowhite(ايندڙ_arg);
		إذا (!eap->skip)
		    curwin->w_s->b_syn_sync_id = syn_check_گروهه(ايندڙ_arg,
						   (صحيح)(arg_پڇاڙي - ايندڙ_arg));
		ايندڙ_arg = skipwhite(arg_پڇاڙي);
	    }
	    وإلا إذا (!eap->skip)
		curwin->w_s->b_syn_sync_id = syn_name2id((char_u *)"Comment");
	}
	وإلا إذا (  STRNCMP(ڪنجي, "LINES", 5) == 0
		|| STRNCMP(ڪنجي, "MINLINES", 8) == 0
		|| STRNCMP(ڪنجي, "MAXLINES", 8) == 0
		|| STRNCMP(ڪنجي, "LINEBREAKS", 10) == 0)
	{
	    إذا (ڪنجي[4] == 'S')
		arg_پڇاڙي = ڪنجي + 6;
	    وإلا إذا (ڪنجي[0] == 'L')
		arg_پڇاڙي = ڪنجي + 11;
	    وإلا
		arg_پڇاڙي = ڪنجي + 9;
	    إذا (arg_پڇاڙي[-1] != '=' || !VIM_ISDIGIT(*arg_پڇاڙي))
	    {
		illegal = TRUE;
		اكسر;
	    }
	    n = getdigits(&arg_پڇاڙي);
	    إذا (!eap->skip)
	    {
		إذا (ڪنجي[4] == 'B')
		    curwin->w_s->b_syn_sync_linebreaks = n;
		وإلا إذا (ڪنجي[1] == 'A')
		    curwin->w_s->b_syn_sync_maxlines = n;
		وإلا
		    curwin->w_s->b_syn_sync_minlines = n;
	    }
	}
	وإلا إذا (STRCMP(ڪنجي, "FROMSTART") == 0)
	{
	    إذا (!eap->skip)
	    {
		curwin->w_s->b_syn_sync_minlines = MAXLNUM;
		curwin->w_s->b_syn_sync_maxlines = 0;
	    }
	}
	وإلا إذا (STRCMP(ڪنجي, "LINECONT") == 0)
	{
	    إذا (*ايندڙ_arg == NUL)	   // missing pattern
	    {
		illegal = TRUE;
		اكسر;
	    }
	    إذا (curwin->w_s->b_syn_linecont_pat != عدم)
	    {
		emsg(_(e_syntax_sync_سٽ_continuations_نمونو_specified_twice));
		ختم = TRUE;
		اكسر;
	    }
	    arg_پڇاڙي = skip_regexp(ايندڙ_arg + 1, *ايندڙ_arg, TRUE);
	    إذا (*arg_پڇاڙي != *ايندڙ_arg)	    // end delimiter not found
	    {
		illegal = TRUE;
		اكسر;
	    }

	    إذا (!eap->skip)
	    {
		// store the pattern and compiled regexp program
		إذا ((curwin->w_s->b_syn_linecont_pat =
			    vim_strnsave(ايندڙ_arg + 1,
				      arg_پڇاڙي - ايندڙ_arg - 1)) == عدم)
		{
		    ختم = TRUE;
		    اكسر;
		}
		curwin->w_s->b_syn_linecont_ic = curwin->w_s->b_syn_ic;

		// Make 'cpoptions' empty, to avoid the 'l' flag
		cpo_save = p_cpo;
		p_cpo = خالي_جايون_option;
		curwin->w_s->b_syn_linecont_prog =
		       vim_regcomp(curwin->w_s->b_syn_linecont_pat, RE_MAGIC);
		p_cpo = cpo_save;
#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
		syn_clear_وقت(&curwin->w_s->b_syn_linecont_وقت);
#جيڪڏهن_پورو

		إذا (curwin->w_s->b_syn_linecont_prog == عدم)
		{
		    VIM_CLEAR(curwin->w_s->b_syn_linecont_pat);
		    ختم = TRUE;
		    اكسر;
		}
	    }
	    ايندڙ_arg = skipwhite(arg_پڇاڙي + 1);
	}
	وإلا
	{
	    eap->arg = ايندڙ_arg;
	    إذا (STRCMP(ڪنجي, "MATCH") == 0)
		syn_cmd_ميلاپ(eap, TRUE);
	    وإلا إذا (STRCMP(ڪنجي, "REGION") == 0)
		syn_cmd_region(eap, TRUE);
	    وإلا إذا (STRCMP(ڪنجي, "CLEAR") == 0)
		syn_cmd_clear(eap, TRUE);
	    وإلا
		illegal = TRUE;
	    ختم = TRUE;
	    اكسر;
	}
	arg_شروعات = ايندڙ_arg;
    }
    vim_free(ڪنجي);
    إذا (illegal)
	semsg(_(e_illegal_arguments_str), arg_شروعات);
    وإلا إذا (!ختم)
    {
	سيٽ_nextcmd(eap, arg_شروعات);
	redraw_curbuf_later(UPD_SOME_VALID);
	syn_ڍير_free_all(curwin->w_s);	// Need to recompute all syntax.
    }
}

/*
 * Convert a line of highlight group names into a list of group ID numbers.
 * "arg" should point to the "contains" or "nextgroup" keyword.
 * "arg" is advanced to after the last group name.
 * Careful: the argument is modified (NULs added).
 * returns FAIL for some error, OK for success.
 */
    ساكن صحيح
get_id_list(
    char_u	**arg,
    صحيح		keylen,		// length of keyword
    ننڍو	**list,		// where to store the resulting list, if not
				// NULL, the list is silently skipped!
    صحيح		skip)
{
    char_u	*p = عدم;
    char_u	*پڇاڙي;
    صحيح		ڦيرو;
    صحيح		ڳڻپ;
    صحيح		ڪل_ڳڻپ = 0;
    ننڍو	*retval = عدم;
    char_u	*name;
    regmatch_T	regmatch;
    صحيح		id;
    صحيح		i;
    صحيح		failed = FALSE;

    /*
     * We parse the list twice:
     * round == 1: count the number of items, allocate the array.
     * round == 2: fill the array with the items.
     * In round 1 new groups may be added, causing the number of items to
     * grow when a regexp is used.  In that case round 1 is done once again.
     */
    لكل (ڦيرو = 1; ڦيرو <= 2; ++ڦيرو)
    {
	/*
	 * skip "contains"
	 */
	p = skipwhite(*arg + keylen);
	إذا (*p != '=')
	{
	    semsg(_(e_missing_equal_sign_str), *arg);
	    اكسر;
	}
	p = skipwhite(p + 1);
	إذا (پڇاڙيون_excmd2(*arg, p))
	{
	    semsg(_(e_خالي_جايون_دليل_str), *arg);
	    اكسر;
	}

	/*
	 * parse the arguments after "contains"
	 */
	ڳڻپ = 0;
	طالما (!پڇاڙيون_excmd2(*arg, p))
	{
	    لكل (پڇاڙي = p; *پڇاڙي && !VIM_ISWHITE(*پڇاڙي) && *پڇاڙي != ','; ++پڇاڙي)
		;
	    name = alloc(پڇاڙي - p + 3);	    // leave room for "^$"
	    إذا (name == عدم)
	    {
		failed = TRUE;
		اكسر;
	    }
	    vim_strncpy(name + 1, p, پڇاڙي - p);
	    إذا (       STRCMP(name + 1, "ALLBUT") == 0
		    || STRCMP(name + 1, "ALL") == 0
		    || STRCMP(name + 1, "TOP") == 0
		    || STRCMP(name + 1, "CONTAINED") == 0)
	    {
		إذا (TOUPPER_ASC(**arg) != 'C')
		{
		    semsg(_(e_str_not_allowed_هتي), name + 1);
		    failed = TRUE;
		    vim_free(name);
		    اكسر;
		}
		إذا (ڳڻپ != 0)
		{
		    semsg(_(e_str_must_be_پهريون_اندريون_شامل_آهي_list), name + 1);
		    failed = TRUE;
		    vim_free(name);
		    اكسر;
		}
		إذا (name[1] == 'A')
		    id = SYNID_ALLBUT;
		وإلا إذا (name[1] == 'T')
		    id = SYNID_چوٽي;
		وإلا
		    id = SYNID_CONTAINED;
		id += current_syn_inc_ٽيگ;
	    }
	    وإلا إذا (name[1] == '@')
	    {
		إذا (skip)
		    id = -1;
		وإلا
		    id = syn_check_cluster(name + 2, (صحيح)(پڇاڙي - p - 1));
	    }
	    وإلا
	    {
		/*
		 * Handle full group name.
		 */
		إذا (vim_strpbrk(name + 1, (char_u *)"\\.*^$~[") == عدم)
		    id = syn_check_گروهه(name + 1, (صحيح)(پڇاڙي - p));
		وإلا
		{
		    /*
		     * Handle match of regexp with group names.
		     */
		    *name = '^';
		    STRCAT(name, "$");
		    regmatch.regprog = vim_regcomp(name, RE_MAGIC);
		    إذا (regmatch.regprog == عدم)
		    {
			failed = TRUE;
			vim_free(name);
			اكسر;
		    }

		    regmatch.rm_ic = TRUE;
		    id = 0;
		    لكل (i = highlight_انگ_groups(); --i >= 0; )
		    {
			إذا (vim_regexec(&regmatch, highlight_گروهه_name(i),
								  (colnr_T)0))
			{
			    إذا (ڦيرو == 2)
			    {
				// Got more items than expected; can happen
				// when adding items that match:
				// "contains=a.*b,axb".
				// Go back to first round
				إذا (ڳڻپ >= ڪل_ڳڻپ)
				{
				    vim_free(retval);
				    ڦيرو = 1;
				}
				وإلا
				    retval[ڳڻپ] = i + 1;
			    }
			    ++ڳڻپ;
			    id = -1;	    // remember that we found one
			}
		    }
		    vim_regfree(regmatch.regprog);
		}
	    }
	    vim_free(name);
	    إذا (id == 0)
	    {
		semsg(_(e_unknown_گروهه_name_str), p);
		failed = TRUE;
		اكسر;
	    }
	    إذا (id > 0)
	    {
		إذا (ڦيرو == 2)
		{
		    // Got more items than expected, go back to first round
		    إذا (ڳڻپ >= ڪل_ڳڻپ)
		    {
			vim_free(retval);
			ڦيرو = 1;
		    }
		    وإلا
			retval[ڳڻپ] = id;
		}
		++ڳڻپ;
	    }
	    p = skipwhite(پڇاڙي);
	    إذا (*p != ',')
		اكسر;
	    p = skipwhite(p + 1);	// skip comma in between arguments
	}
	إذا (failed)
	    اكسر;
	إذا (ڦيرو == 1)
	{
	    retval = ALLOC_MULT(ننڍو, ڳڻپ + 1);
	    إذا (retval == عدم)
		اكسر;
	    retval[ڳڻپ] = 0;	    // zero means end of the list
	    ڪل_ڳڻپ = ڳڻپ;
	}
    }

    *arg = p;
    إذا (failed || retval == عدم)
    {
	vim_free(retval);
	أرجع FAIL;
    }

    إذا (*list == عدم)
	*list = retval;
    وإلا
	vim_free(retval);	// list already found, don't overwrite it

    أرجع OK;
}

/*
 * Make a copy of an ID list.
 */
    ساكن ننڍو *
copy_id_list(ننڍو *list)
{
    صحيح	    len;
    صحيح	    ڳڻپ;
    ننڍو   *retval;

    إذا (list == عدم)
	أرجع عدم;

    لكل (ڳڻپ = 0; list[ڳڻپ]; ++ڳڻپ)
	;
    len = (ڳڻپ + 1) * حجم(ننڍو);
    retval = alloc(len);
    إذا (retval != عدم)
	mch_memmove(retval, list, (size_t)len);

    أرجع retval;
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
#وصف_ڪريو IDL_CACHE_سائيز 16

تعريفنوع بنية
{
    ننڍو	*idl_ڪنجي;	// the list this entry resolves (NULL when empty)
    ننڍو	*idl_ids;	// sorted, cluster-expanded group IDs (allocated)
    صحيح		idl_ڳڻپ;	// number of IDs in idl_ids
    ننڍو	idl_marker;	// leading ALLBUT/TOP/CONTAINED item, or 0
    bool	idl_usable;	// false: not cacheable, use the slow path
} idl_داخلا_T;

تعريفنوع بنية
{
    idl_داخلا_T	ent[IDL_CACHE_سائيز];
    صحيح		ڦيرو;		// round-robin replacement index
    صحيح		آخري;		// index of the last returned entry (fast path)
} idl_cache_T;

/*
 * Collect the explicit group IDs of "list" into "ga", expanding clusters.
 * "seen" records visited clusters to break cycles and avoid duplicate work.
 * Return false when the list cannot be represented as a plain set (a nested
 * ALLBUT/TOP/CONTAINED marker) or on out of memory.
 */
    ساكن bool
idl_collect(ننڍو *list, garray_T *ga, garray_T *ڏٺل)
{
    إذا (list == عدم || list == ID_LIST_ALL)
	أرجع false;
    لكل (ننڍو شي = *list; شي != 0; شي = *++list)
    {
	إذا (شي >= SYNID_ALLBUT && شي < SYNID_CLUSTER)
	    أرجع false;	// nested ALLBUT/TOP/CONTAINED: not cacheable
	إذا (شي >= SYNID_CLUSTER)
	{
	    ننڍو	*scl_list;
	    bool	ڏٺل_it = false;

	    لكل (صحيح i = 0; i < ڏٺل->ga_len; ++i)
		إذا (((ننڍو *)ڏٺل->ga_معلومات)[i] == شي)
		{
		    ڏٺل_it = true;
		    اكسر;
		}
	    إذا (ڏٺل_it)
		استمر;
	    إذا (ga_grow(ڏٺل, 1) == FAIL)
		أرجع false;
	    ((ننڍو *)ڏٺل->ga_معلومات)[ڏٺل->ga_len++] = شي;
	    // Record the cluster id itself, not just its members: the slow
	    // path matches "item == id" for a cluster id present in the list.
	    إذا (ga_grow(ga, 1) == FAIL)
		أرجع false;
	    ((ننڍو *)ga->ga_معلومات)[ga->ga_len++] = شي;
	    scl_list = SYN_CLSTR(syn_بلاڪ)[شي - SYNID_CLUSTER].scl_list;
	    إذا (scl_list != عدم && !idl_collect(scl_list, ga, ڏٺل))
		أرجع false;
	}
	وإلا
	{
	    إذا (ga_grow(ga, 1) == FAIL)
		أرجع false;
	    ((ننڍو *)ga->ga_معلومات)[ga->ga_len++] = شي;
	}
    }
    أرجع true;
}

    ساكن صحيح
idl_id_cmp(ثابت فراغ *a, ثابت فراغ *b)
{
    أرجع (صحيح)*(ثابت ننڍو *)a - (صحيح)*(ثابت ننڍو *)b;
}

/*
 * Get the cache entry that resolves "list", building it if needed.
 * Returns NULL only on allocation failure.
 */
    ساكن idl_داخلا_T *
idl_get_داخلا(ننڍو *list)
{
    idl_cache_T	*cache = (idl_cache_T *)syn_بلاڪ->b_idlist_cache;
    idl_داخلا_T	*e;
    garray_T	ga, ڏٺل;

    إذا (cache == عدم)
    {
	cache = ALLOC_CLEAR_ONE(idl_cache_T);
	إذا (cache == عدم)
	    أرجع عدم;
	syn_بلاڪ->b_idlist_cache = cache;
    }
    // Fast path: the same list is usually queried many times in a row.
    إذا (cache->ent[cache->آخري].idl_ڪنجي == list)
	أرجع &cache->ent[cache->آخري];
    لكل (صحيح i = 0; i < IDL_CACHE_سائيز; ++i)
	إذا (cache->ent[i].idl_ڪنجي == list)
	{
	    cache->آخري = i;
	    أرجع &cache->ent[i];
	}

    // Build a new entry, replacing one in round-robin order.
    cache->آخري = cache->ڦيرو;
    e = &cache->ent[cache->ڦيرو];
    cache->ڦيرو = (cache->ڦيرو + 1) % IDL_CACHE_سائيز;
    VIM_CLEAR(e->idl_ids);
    e->idl_ڳڻپ = 0;
    e->idl_ڪنجي = list;
    e->idl_marker = 0;
    e->idl_usable = true;

    // A leading ALLBUT/TOP/CONTAINED marker is kept aside; the rest is the set.
    إذا (*list >= SYNID_ALLBUT && *list < SYNID_CLUSTER)
    {
	e->idl_marker = *list;
	++list;
    }

    ga_init2(&ga, حجم(ننڍو), 50);
    ga_init2(&ڏٺل, حجم(ننڍو), 10);
    إذا (!idl_collect(list, &ga, &ڏٺل))
    {
	e->idl_usable = false;
	ga_clear(&ga);
	ga_clear(&ڏٺل);
	أرجع e;
    }
    ga_clear(&ڏٺل);
    إذا (ga.ga_len > 0)
    {
	qsort(ga.ga_معلومات, (size_t)ga.ga_len, حجم(ننڍو), idl_id_cmp);
	e->idl_ids = (ننڍو *)ga.ga_معلومات;	// take ownership
	e->idl_ڳڻپ = ga.ga_len;
    }
    وإلا
	ga_clear(&ga);
    أرجع e;
}

/*
 * Return true if "id" is in the resolved set of cache entry "e".
 */
    ساكن bool
idl_شامل_آهي(idl_داخلا_T *e, ننڍو id)
{
    صحيح	lo = 0;
    صحيح	hi = e->idl_ڳڻپ - 1;

    طالما (lo <= hi)
    {
	صحيح	mid = (lo + hi) / 2;
	ننڍو	v = e->idl_ids[mid];

	إذا (v == id)
	    أرجع true;
	إذا (v < id)
	    lo = mid + 1;
	وإلا
	    hi = mid - 1;
    }
    أرجع false;
}

    ساكن فراغ
idl_cache_clear(synblock_T *بلاڪ)
{
    idl_cache_T	*cache = (idl_cache_T *)بلاڪ->b_idlist_cache;

    إذا (cache == عدم)
	أرجع;
    لكل (صحيح i = 0; i < IDL_CACHE_سائيز; ++i)
	vim_free(cache->ent[i].idl_ids);
    vim_free(cache);
    بلاڪ->b_idlist_cache = عدم;
}
// }}}

/*
 * Check if syntax group "ssp" is in the ID list "list" of "cur_si".
 * "cur_si" can be NULL if not checking the "containedin" list.
 * Used to check if a syntax item is in the "contains" or "nextgroup" list of
 * the current item.
 * This function is called very often, keep it fast!!
 */
    ساكن صحيح
اندريون_id_list(
    stateitem_T	*cur_si,	// current item or NULL
    ننڍو	*list,		// id list
    بنية sp_syn *ssp,		// group id and ":syn include" tag of group
    صحيح		flags)		// group flags
{
    صحيح		retval;
    ننڍو	*scl_list;
    ننڍو	شي;
    ننڍو	id = ssp->id;
    ساكن صحيح	اونهائي = 0;
    صحيح		r;
    صحيح		toplevel;
    idl_داخلا_T	*e;

    // If ssp has a "containedin" list and "cur_si" is in it, return TRUE.
    إذا (cur_si != عدم && ssp->cont_اندريون_list != عدم
					    && !(cur_si->si_flags & HL_ميلاپ))
    {
	// Ignore transparent items without a contains argument.  Double check
	// that we don't go back past the first one.
	طالما ((cur_si->si_flags & HL_TRANS_CONT)
		&& cur_si > (stateitem_T *)(current_state.ga_معلومات))
	    --cur_si;
	// cur_si->si_idx is -1 for keywords, these never contain anything.
	إذا (cur_si->si_idx >= 0 && اندريون_id_list(عدم, ssp->cont_اندريون_list,
		&(SYN_شيون(syn_بلاڪ)[cur_si->si_idx].sp_syn),
		  SYN_شيون(syn_بلاڪ)[cur_si->si_idx].sp_flags))
	    أرجع TRUE;
    }

    إذا (list == عدم)
	أرجع FALSE;

    /*
     * If list is ID_LIST_ALL, we are in a transparent item that isn't
     * inside anything.  Only allow not-contained groups.
     */
    إذا (list == ID_LIST_ALL)
	أرجع !(flags & HL_CONTAINED);

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
    e = disable_syn_idlist_cache_for_testing ? عدم : idl_get_داخلا(list);
    إذا (e != عدم && e->idl_usable)
    {
	إذا (e->idl_marker != 0)
	{
	    شي = e->idl_marker;
	    إذا (شي < SYNID_چوٽي)
	    {
		إذا (شي - SYNID_ALLBUT != ssp->inc_ٽيگ)
		    أرجع FALSE;
	    }
	    وإلا إذا (شي < SYNID_CONTAINED)
	    {
		إذا (شي - SYNID_چوٽي != ssp->inc_ٽيگ || !toplevel)
		    أرجع FALSE;
	    }
	    وإلا
	    {
		إذا (شي - SYNID_CONTAINED != ssp->inc_ٽيگ || toplevel)
		    أرجع FALSE;
	    }
	    retval = FALSE;
	}
	وإلا
	    retval = TRUE;
	أرجع idl_شامل_آهي(e, id) ? retval : !retval;
    }

    /*
     * Slow path (uncacheable list, e.g. a nested ALLBUT).
     * If the first item is "ALLBUT", return TRUE if "id" is NOT in the
     * contains list.  We also require that "id" is at the same ":syn include"
     * level as the list.
     */
    شي = *list;
    إذا (شي >= SYNID_ALLBUT && شي < SYNID_CLUSTER)
    {
	إذا (شي < SYNID_چوٽي)
	{
	    // ALL or ALLBUT: accept all groups in the same file
	    إذا (شي - SYNID_ALLBUT != ssp->inc_ٽيگ)
		أرجع FALSE;
	}
	وإلا إذا (شي < SYNID_CONTAINED)
	{
	    // TOP: accept all not-contained groups in the same file
	    إذا (شي - SYNID_چوٽي != ssp->inc_ٽيگ || !toplevel)
		أرجع FALSE;
	}
	وإلا
	{
	    // CONTAINED: accept all contained groups in the same file
	    إذا (شي - SYNID_CONTAINED != ssp->inc_ٽيگ || toplevel)
		أرجع FALSE;
	}
	شي = *++list;
	retval = FALSE;
    }
    وإلا
	retval = TRUE;

    /*
     * Return "retval" if id is in the contains list.
     */
    طالما (شي != 0)
    {
	إذا (شي == id)
	    أرجع retval;
	إذا (شي >= SYNID_CLUSTER)
	{
	    scl_list = SYN_CLSTR(syn_بلاڪ)[شي - SYNID_CLUSTER].scl_list;
	    // restrict recursiveness to 30 to avoid an endless loop for a
	    // cluster that includes itself (indirectly)
	    إذا (scl_list != عدم && اونهائي < 30)
	    {
		++اونهائي;
		r = اندريون_id_list(عدم, scl_list, ssp, flags);
		--اونهائي;
		إذا (r)
		    أرجع retval;
	    }
	}
	شي = *++list;
    }
    أرجع !retval;
}

بنية subcommand
{
    حرف    *name;			// subcommand name
    فراغ    (*func)(exarg_T *, صحيح);	// function to call
};

ساكن بنية subcommand subcommands[] =
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
    {"match",		syn_cmd_ميلاپ},
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
    فراغ
ex_syntax(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*subcmd_پڇاڙي;
    char_u	*subcmd_name;
    صحيح		i;

    syn_cmdlinep = eap->cmdlinep;

    // isolate subcommand name
    لكل (subcmd_پڇاڙي = arg; ASCII_ISALPHA(*subcmd_پڇاڙي); ++subcmd_پڇاڙي)
	;
    subcmd_name = vim_strnsave(arg, subcmd_پڇاڙي - arg);
    إذا (subcmd_name == عدم)
	أرجع;

    إذا (eap->skip)		// skip error messages for all subcommands
	++emsg_skip;
    لكل (i = 0; i < (صحيح)ARRAY_ڊيگهائي(subcommands); ++i)
    {
	إذا (STRCMP(subcmd_name, (char_u *)subcommands[i].name) == 0)
	{
	    eap->arg = skipwhite(subcmd_پڇاڙي);
	    (subcommands[i].func)(eap, FALSE);
	    اكسر;
	}
    }

    إذا (i == (صحيح)ARRAY_ڊيگهائي(subcommands))
	semsg(_(e_invalid_syntax_subcommand_str), subcmd_name);

    vim_free(subcmd_name);
    إذا (eap->skip)
	--emsg_skip;
}

    فراغ
ex_ownsyntax(exarg_T *eap)
{
    char_u	*old_قدر;
    char_u	*new_قدر_2;

    إذا (curwin->w_s == &curwin->w_بفر->b_s)
    {
	curwin->w_s = ALLOC_ONE(synblock_T);
	CLEAR_اشارو(curwin->w_s);
	hash_init(&curwin->w_s->b_keywtab);
	hash_init(&curwin->w_s->b_keywtab_ic);
#جيڪڏهن_وصف_ٿيل FEAT_SPELL
	// TODO: keep the spell checking as it was.
	curwin->w_p_spell = FALSE;	// No spell checking
	// make sure option values are "empty_option" instead of NULL
	clear_string_option(&curwin->w_s->b_p_spc);
	clear_string_option(&curwin->w_s->b_p_spf);
	clear_string_option(&curwin->w_s->b_p_spl);
	clear_string_option(&curwin->w_s->b_p_spo);
#جيڪڏهن_پورو
	clear_string_option(&curwin->w_s->b_syn_isk);
    }

    // save value of b:current_syntax
    old_قدر = get_var_قدر((char_u *)"b:current_syntax");
    إذا (old_قدر != عدم)
	old_قدر = vim_strsave(old_قدر);

    // Apply the "syntax" autocommand event, this finds and loads the syntax
    // file.
    apply_autocmds(EVENT_SYNTAX, eap->arg, curbuf->b_fname, TRUE, curbuf);

    // move value of b:current_syntax to w:current_syntax
    new_قدر_2 = get_var_قدر((char_u *)"b:current_syntax");
    إذا (new_قدر_2 != عدم)
	سيٽ_اندروني_string_var((char_u *)"w:current_syntax", new_قدر_2);

    // restore value of b:current_syntax
    إذا (old_قدر == عدم)
	do_unlet((char_u *)"b:current_syntax", TRUE);
    وإلا
    {
	سيٽ_اندروني_string_var((char_u *)"b:current_syntax", old_قدر);
	vim_free(old_قدر);
    }
}

    صحيح
syntax_present(win_T *win)
{
    أرجع (win->w_s->b_syn_patterns.ga_len != 0
	    || win->w_s->b_syn_clusters.ga_len != 0
	    || win->w_s->b_keywtab.ht_استعمال_ٿيل > 0
	    || win->w_s->b_keywtab_ic.ht_استعمال_ٿيل > 0);
}


ساكن تعداد
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
    فراغ
reset_expand_highlight(فراغ)
{
    include_link = include_default = include_none = 0;
}

/*
 * Handle command line completion for :match and :echohl command: Add "NONE"
 * as highlight group.
 */
    فراغ
سيٽ_context_اندريون_echohl_cmd(expand_T *xp, char_u *arg)
{
    xp->xp_context = EXPAND_HIGHLIGHT;
    xp->xp_نمونو = arg;
    include_none = 1;
}

/*
 * Handle command line completion for :syntax command.
 */
    فراغ
سيٽ_context_اندريون_syntax_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    // Default: expand subcommands
    xp->xp_context = EXPAND_SYNTAX;
    expand_what = EXP_SUBCMD;
    xp->xp_نمونو = arg;
    include_link = 0;
    include_default = 0;

    إذا (*arg == NUL)
	أرجع;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    إذا (*p == NUL)
	أرجع;

    // past first word
    xp->xp_نمونو = skipwhite(p);
    إذا (*skiptowhite(xp->xp_نمونو) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    وإلا إذا (STRNICMP(arg, "case", p - arg) == 0)
	expand_what = EXP_CASE;
    وإلا إذا (STRNICMP(arg, "spell", p - arg) == 0)
	expand_what = EXP_SPELL;
    وإلا إذا (STRNICMP(arg, "sync", p - arg) == 0)
	expand_what = EXP_SYNC;
    وإلا إذا (STRNICMP(arg, "list", p - arg) == 0)
    {
	p = skipwhite(p);
	إذا (*p == '@')
	    expand_what = EXP_CLUSTER;
	وإلا
	    xp->xp_context = EXPAND_HIGHLIGHT;
    }
    وإلا إذا (STRNICMP(arg, "keyword", p - arg) == 0
	    || STRNICMP(arg, "region", p - arg) == 0
	    || STRNICMP(arg, "match", p - arg) == 0)
	xp->xp_context = EXPAND_HIGHLIGHT;
    وإلا
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Function given to ExpandGeneric() to obtain the list syntax names for
 * expansion.
 */
    char_u *
get_syntax_name(expand_T *xp, صحيح idx)
{
    بدل (expand_what)
    {
	حالة EXP_SUBCMD:
	    إذا (idx < 0 || idx >= (صحيح)ARRAY_ڊيگهائي(subcommands))
		أرجع عدم;
	    أرجع (char_u *)subcommands[idx].name;
	حالة EXP_CASE:
	{
	    ساكن حرف *case_دليلون[] = {"match", "ignore", عدم};
	    أرجع (char_u *)case_دليلون[idx];
	}
	حالة EXP_SPELL:
	{
	    ساكن حرف *spell_دليلون[] =
		{"toplevel", "notoplevel", "default", عدم};
	    أرجع (char_u *)spell_دليلون[idx];
	}
	حالة EXP_SYNC:
	{
	    ساكن حرف *sync_دليلون[] =
		{"ccomment", "clear", "fromstart",
		 "linebreaks=", "linecont", "lines=", "match",
		 "maxlines=", "minlines=", "region", عدم};
	    أرجع (char_u *)sync_دليلون[idx];
	}
	حالة EXP_CLUSTER:
	{
	    إذا (idx < curwin->w_s->b_syn_clusters.ga_len)
	    {
		vim_snprintf((حرف *)xp->xp_buf, EXPAND_BUF_LEN, "@%s",
					 SYN_CLSTR(curwin->w_s)[idx].scl_name);
		أرجع xp->xp_buf;
	    }
	    وإلا
		أرجع عدم;
	}
    }
    أرجع عدم;
}


/*
 * Function called for expression evaluation: get syntax ID at file position.
 */
    صحيح
syn_get_id(
    win_T	*wp,
    ڊگهو	lnum,
    colnr_T	کانو,
    صحيح		trans,	     // remove transparency
    صحيح		*spellp,     // return: can do spell checking
    صحيح		keep_state)  // keep state of char at "col"
{
    // When the position is not after the current position and in the same
    // line of the same window with the same buffer, need to restart parsing.
    إذا (wp != syn_win
	    || wp->w_بفر != syn_buf
	    || lnum != current_lnum
	    || کانو < current_کانو)
	syntax_شروعات(wp, lnum);
    وإلا إذا (wp->w_بفر == syn_buf
	    && lnum == current_lnum
	    && کانو > current_کانو)
	// next_match may not be correct when moving around, e.g. with the
	// "skip" expression in searchpair()
	ايندڙ_ميلاپ_idx = -1;

    (فراغ)get_syntax_attr(کانو, spellp, keep_state);

    أرجع (trans ? current_trans_id : current_id);
}

#إذا defined(FEAT_CONCEAL)
/*
 * Get extra information about the syntax item.  Must be called right after
 * get_syntax_attr().
 * Stores the current item sequence nr in "*seqnrp".
 * Returns the current flags.
 */
    صحيح
get_syntax_info(صحيح *seqnrp)
{
    *seqnrp = current_seqnr;
    أرجع current_flags;
}

/*
 * Return conceal substitution character
 */
    صحيح
syn_get_sub_char(فراغ)
{
    أرجع current_sub_char;
}
#جيڪڏهن_پورو

#إذا defined(FEAT_EVAL)
/*
 * Return the syntax ID at position "i" in the current stack.
 * The caller must have called syn_get_id() before to fill the stack.
 * Returns -1 when "i" is out of range.
 */
    صحيح
syn_get_ڍير_شي(صحيح i)
{
    إذا (i >= current_state.ga_len)
    {
	// Need to invalidate the state, because we didn't properly finish it
	// for the last character, "keep_state" was TRUE.
	invalidate_current_state();
	current_کانو = MAXCOL;
	أرجع -1;
    }
    أرجع CUR_STATE(i).si_id;
}
#جيڪڏهن_پورو

#إذا defined(FEAT_FOLDING)
    ساكن صحيح
syn_cur_foldlevel(فراغ)
{
    صحيح		level = 0;
    صحيح		i;

    لكل (i = 0; i < current_state.ga_len; ++i)
	إذا (CUR_STATE(i).si_flags & HL_FOLD)
	    ++level;
    أرجع level;
}

/*
 * Function called to get folding level for line "lnum" in window "wp".
 */
    صحيح
syn_get_foldlevel(win_T *wp, ڊگهو lnum)
{
    صحيح		level = 0;
    صحيح		هيٺيون_level;
    صحيح		cur_level;

    // Return quickly when there are no fold items at all.
    إذا (wp->w_s->b_syn_folditems != 0
	    && !wp->w_s->b_syn_error
# ifdef SYN_وقت_حد_بندي
	    && !wp->w_s->b_syn_slow
# endif
	    )
    {
	syntax_شروعات(wp, lnum);

	// Start with the fold level at the start of the line.
	level = syn_cur_foldlevel();

	إذا (wp->w_s->b_syn_foldlevel == SYNFLD_MINIMUM)
	{
	    // Find the lowest fold level that is followed by a higher one.
	    cur_level = level;
	    هيٺيون_level = cur_level;
	    طالما (!current_ختم)
	    {
		(فراغ)syn_current_attr(FALSE, FALSE, عدم, FALSE);
		cur_level = syn_cur_foldlevel();
		إذا (cur_level < هيٺيون_level)
		    هيٺيون_level = cur_level;
		وإلا إذا (cur_level > هيٺيون_level)
		    level = هيٺيون_level;
		++current_کانو;
	    }
	}
    }
    إذا (level > wp->w_p_fdn)
    {
	level = wp->w_p_fdn;
	إذا (level < 0)
	    level = 0;
    }
    أرجع level;
}
#جيڪڏهن_پورو

#إذا defined(FEAT_PROFILE)
/*
 * ":syntime".
 */
    فراغ
ex_syntime(exarg_T *eap)
{
    إذا (STRCMP(eap->arg, "on") == 0)
	syn_وقت_on = TRUE;
    وإلا إذا (STRCMP(eap->arg, "off") == 0)
	syn_وقت_on = FALSE;
    وإلا إذا (STRCMP(eap->arg, "clear") == 0)
	syntime_clear();
    وإلا إذا (STRCMP(eap->arg, "report") == 0)
	syntime_report();
    وإلا
	semsg(_(e_invalid_دليل_str), eap->arg);
}

    ساكن فراغ
syn_clear_وقت(syn_وقت_T *st)
{
    profile_zero(&st->ڪل);
    profile_zero(&st->slowest);
    st->ڳڻپ = 0;
    st->ميلاپ = 0;
}

/*
 * Clear the syntax timing for the current buffer.
 */
    ساكن فراغ
syntime_clear(فراغ)
{
    صحيح		idx;
    synpat_T	*spp;

    إذا (!syntax_present(curwin))
    {
	نياپو(_(نياپو_no_شيون));
	أرجع;
    }
    لكل (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len; ++idx)
    {
	spp = &(SYN_شيون(curwin->w_s)[idx]);
	syn_clear_وقت(&spp->sp_وقت);
    }
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":syntime {on,off,clear,report}" command.
 */
    char_u *
get_syntime_arg(expand_T *xp اڻ_استعمال_ٿيل, صحيح idx)
{
    بدل (idx)
    {
	حالة 0: أرجع (char_u *)"on";
	حالة 1: أرجع (char_u *)"off";
	حالة 2: أرجع (char_u *)"clear";
	حالة 3: أرجع (char_u *)"report";
    }
    أرجع عدم;
}

تعريفنوع بنية
{
    proftime_T	ڪل;
    صحيح		ڳڻپ;
    صحيح		ميلاپ;
    proftime_T	slowest;
    proftime_T	اوسط;
    صحيح		id;
    char_u	*نمونو;
} وقت_داخلا_T;

    ساكن صحيح
syn_compare_syntime(ثابت فراغ *v1, ثابت فراغ *v2)
{
    ثابت وقت_داخلا_T	*s1 = v1;
    ثابت وقت_داخلا_T	*s2 = v2;

    أرجع profile_cmp(&s1->ڪل, &s2->ڪل);
}

/*
 * Clear the syntax timing for the current buffer.
 */
    ساكن فراغ
syntime_report(فراغ)
{
    صحيح		idx;
    synpat_T	*spp;
# إذا defined(FEAT_RELTIME)
    proftime_T	tm;
# endif
    صحيح		len;
    صحيح		patlen;
    proftime_T	ڪل_ڪل;
    صحيح		ڪل_ڳڻپ = 0;
    garray_T    ga;
    وقت_داخلا_T *p;

    إذا (!syntax_present(curwin))
    {
	نياپو(_(نياپو_no_شيون));
	أرجع;
    }

    ga_init2(&ga, حجم(وقت_داخلا_T), 50);
    profile_zero(&ڪل_ڪل);
    لكل (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len; ++idx)
    {
	spp = &(SYN_شيون(curwin->w_s)[idx]);
	إذا (spp->sp_وقت.ڳڻپ > 0)
	{
	    (فراغ)ga_grow(&ga, 1);
	    p = ((وقت_داخلا_T *)ga.ga_معلومات) + ga.ga_len;
	    p->ڪل = spp->sp_وقت.ڪل;
	    profile_جوڙيو(&ڪل_ڪل, &spp->sp_وقت.ڪل);
	    p->ڳڻپ = spp->sp_وقت.ڳڻپ;
	    p->ميلاپ = spp->sp_وقت.ميلاپ;
	    ڪل_ڳڻپ += spp->sp_وقت.ڳڻپ;
	    p->slowest = spp->sp_وقت.slowest;
# إذا defined(FEAT_RELTIME)
	    profile_divide(&spp->sp_وقت.ڪل, spp->sp_وقت.ڳڻپ, &tm);
	    p->اوسط = tm;
# endif
	    p->id = spp->sp_syn.id;
	    p->نمونو = spp->sp_نمونو;
	    ++ga.ga_len;
	}
    }

    // Sort on total time. Skip if there are no items to avoid passing NULL
    // pointer to qsort().
    إذا (ga.ga_len > 1)
	qsort(ga.ga_معلومات, (size_t)ga.ga_len, حجم(وقت_داخلا_T),
							 syn_compare_syntime);

    نياپو_puts_title(_("  TOTAL      COUNT  MATCH   SLOWEST     AVERAGE   NAME               PATTERN"));
    نياپو_puts("\n");
    لكل (idx = 0; idx < ga.ga_len && !got_int; ++idx)
    {
	p = ((وقت_داخلا_T *)ga.ga_معلومات) + idx;

	نياپو_puts(profile_نياپو(&p->ڪل));
	نياپو_puts(" "); // make sure there is always a separating space
	نياپو_advance(13);
	نياپو_outnum(p->ڳڻپ);
	نياپو_puts(" ");
	نياپو_advance(20);
	نياپو_outnum(p->ميلاپ);
	نياپو_puts(" ");
	نياپو_advance(26);
	نياپو_puts(profile_نياپو(&p->slowest));
	نياپو_puts(" ");
	نياپو_advance(38);
	نياپو_puts(profile_نياپو(&p->اوسط));
	نياپو_puts(" ");
	نياپو_advance(50);
	نياپو_outtrans(highlight_گروهه_name(p->id - 1));
	نياپو_puts(" ");

	نياپو_advance(69);
	إذا (Columns < 80)
	    len = 20; // will wrap anyway
	وإلا
	    len = Columns - 70;
	patlen = (صحيح)STRLEN(p->نمونو);
	إذا (len > patlen)
	    len = patlen;
	نياپو_outtrans_len(p->نمونو, len);
	نياپو_puts("\n");
    }
    ga_clear(&ga);
    إذا (!got_int)
    {
	نياپو_puts("\n");
	نياپو_puts(profile_نياپو(&ڪل_ڪل));
	نياپو_advance(13);
	نياپو_outnum(ڪل_ڳڻپ);
	نياپو_puts("\n");
    }
}
#جيڪڏهن_پورو

#جيڪڏهن_پورو // FEAT_SYN_HL
