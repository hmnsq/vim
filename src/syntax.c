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
#تعریف_کرو SPO_MS_OFF	0	// match  start offset
#تعریف_کرو SPO_ME_OFF	1	// match  end	offset
#تعریف_کرو SPO_HS_OFF	2	// highl. start offset
#تعریف_کرو SPO_HE_OFF	3	// highl. end	offset
#تعریف_کرو SPO_RS_OFF	4	// region start offset
#تعریف_کرو SPO_RE_OFF	5	// region end	offset
#تعریف_کرو SPO_LC_OFF	6	// leading context offset
#تعریف_کرو SPO_گنتی	7

ساکن حرف *(spo_name_tab[SPO_گنتی]) =
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
قسمتعریف ساخت syn_نمونہ
{
    حرف	 sp_type;		// see SPTYPE_ defines below
    حرف	 sp_syncing;		// this item used for syncing
    مختصر	 sp_syn_مطابقت_id;	// highlight group ID of pattern
    مختصر	 sp_off_flags;		// see below
    صحیح		 sp_offsets[SPO_گنتی];	// offsets
    صحیح		 sp_flags;		// see HL_ defines below
#اگر_تعریف_شدہ FEAT_CONCEAL
    صحیح		 sp_cchar;		// conceal substitute character
#اگر_ختم
    صحیح		 sp_ic;			// ignore-case flag for sp_prog
    صحیح		 sp_sync_idx;		// sync item index (syncing only)
    صحیح		 sp_سطر_id;		// ID of last line where tried
    صحیح		 sp_startcol;		// next match in sp_line_id line
    مختصر	*sp_cont_list;		// cont. group IDs, if non-zero
    مختصر	*sp_اگلا_list;		// next group IDs, if non-zero
    ساخت sp_syn sp_syn;		// struct passed to in_id_list()
    char_u	*sp_نمونہ;		// regexp to match, pattern
    regprog_T	*sp_prog;		// regexp to match, program
#اگر_تعریف_شدہ FEAT_PROFILE
    syn_وقت_T	 sp_وقت;
#اگر_ختم
} synpat_T;

// The sp_off_flags are computed like this:
// offset from the start of the matched text: (1 << SPO_XX_OFF)
// offset from the end	 of the matched text: (1 << (SPO_XX_OFF + SPO_COUNT))
// When both are present, only one is used.

#تعریف_کرو SPTYPE_مطابقت	1	// match keyword with this group ID
#تعریف_کرو SPTYPE_آغاز	2	// match a regexp, start of item
#تعریف_کرو SPTYPE_انتہا	3	// match a regexp, end of item
#تعریف_کرو SPTYPE_SKIP	4	// match a regexp, skip within item


#تعریف_کرو SYN_اشیاء(buf)	((synpat_T *)((buf)->b_syn_patterns.ga_ڈیٹا))

#تعریف_کرو NONE_IDX	(-2)	// value of sp_sync_idx for "NONE"

/*
 * Flags for b_syn_sync_flags:
 */
#تعریف_کرو SF_CCOMMENT	0x01	// sync on a C-style comment
#تعریف_کرو SF_مطابقت	0x02	// sync by matching a pattern

#تعریف_کرو SYN_STATE_P(ssp)    ((bufstate_T *)((ssp)->ga_ڈیٹا))

#تعریف_کرو MAXKEYWLEN	80	    // maximum length of a keyword

/*
 * The attributes of the syntax item that has been recognized.
 */
ساکن صحیح current_attr = 0;	    // attr of current syntax word
#اگر_تعریف_شدہ FEAT_EVAL
ساکن صحیح current_id = 0;	    // ID of current char for syn_get_id()
ساکن صحیح current_trans_id = 0;    // idem, transparency removed
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CONCEAL
ساکن صحیح current_flags = 0;
ساکن صحیح current_seqnr = 0;
ساکن صحیح current_sub_char = 0;
#اگر_ختم

قسمتعریف ساخت syn_cluster_S
{
    char_u	    *scl_name;	    // syntax cluster name
    char_u	    *scl_name_u;    // uppercase of scl_name
    مختصر	    *scl_list;	    // IDs in this syntax cluster
} syn_cluster_T;

/*
 * Methods of combining two clusters
 */
#تعریف_کرو CLUSTER_REPLACE	    1	// replace first list with second
#تعریف_کرو CLUSTER_جوڑو	    2	// add second list to first
#تعریف_کرو CLUSTER_تفریق    3	// subtract second list from first

#تعریف_کرو SYN_CLSTR(buf)	((syn_cluster_T *)((buf)->b_syn_clusters.ga_ڈیٹا))

/*
 * Syntax group IDs have different types:
 *     0 - 19999  normal syntax groups
 * 20000 - 20999  ALLBUT indicator (current_syn_inc_tag added)
 * 21000 - 21999  TOP indicator (current_syn_inc_tag added)
 * 22000 - 22999  CONTAINED indicator (current_syn_inc_tag added)
 * 23000 - 32767  cluster IDs (subtract SYNID_CLUSTER for the cluster ID)
 */
#تعریف_کرو SYNID_ALLBUT	MAX_HL_ID   // syntax group ID for contains=ALLBUT
#تعریف_کرو SYNID_چوٹی	21000	    // syntax group ID for contains=TOP
#تعریف_کرو SYNID_CONTAINED	22000	    // syntax group ID for contains=CONTAINED
#تعریف_کرو SYNID_CLUSTER	23000	    // first syntax group ID for clusters

#تعریف_کرو MAX_SYN_INC_لیبل	999	    // maximum before the above overflow
#تعریف_کرو MAX_CLUSTER_ID  (32767 - SYNID_CLUSTER)

/*
 * Annoying Hack(TM):  ":syn include" needs this pointer to pass to
 * expand_filename().  Most of the other syntax commands don't need it, so
 * instead of passing it to them, we stow it here.
 */
ساکن char_u **syn_cmdlinep;

/*
 * Another Annoying Hack(TM):  To prevent rules from other ":syn include"'d
 * files from leaking into ALLBUT lists, we assign a unique ID to the
 * rules in each ":syn include"'d file.
 */
ساکن صحیح current_syn_inc_لیبل = 0;
ساکن صحیح running_syn_inc_لیبل = 0;

/*
 * In a hashtable item "hi_key" points to "keyword" in a keyentry.
 * This avoids adding a pointer to the hashtable item.
 * KE2HIKEY() converts a var pointer to a hashitem key pointer.
 * HIKEY2KE() converts a hashitem key pointer to a var pointer.
 * HI2KE() converts a hashitem pointer to a var pointer.
 */
ساکن keyentry_T dumkey;
#تعریف_کرو KE2HIKEY(kp)  ((kp)->keyword)
#تعریف_کرو HIKEY2KE(p)   ((keyentry_T *)((p) - (dumkey.keyword - (char_u *)&dumkey)))
#تعریف_کرو HI2KE(hi)      HIKEY2KE((hi)->hi_کلید)

/*
 * To reduce the time spent in keepend(), remember at which level in the state
 * stack the first item with "keepend" is present.  When "-1", there is no
 * "keepend" on the stack.
 */
ساکن صحیح keepend_level = -1;

ساکن حرف پیغام_no_اشیاء[] = N_("No Syntax items defined for this buffer");

/*
 * For the current state we need to remember more than just the idx.
 * When si_m_endpos.lnum is 0, the items other than si_idx are unknown.
 * (The end positions have the column number of the next char)
 */
قسمتعریف ساخت state_شے
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
    صحیح		si_انتہا_idx;		// group ID for end pattern or zero
    صحیح		si_سرے;		// if match ends before si_m_endpos
    صحیح		si_attr;		// attributes in this state
    طویل	si_flags;		// HL_HAS_EOL flag in this state, and
					// HL_SKIP* for si_next_list
#اگر_تعریف_شدہ FEAT_CONCEAL
    صحیح		si_seqnr;		// sequence number
    صحیح		si_cchar;		// substitution character for conceal
#اگر_ختم
    مختصر	*si_cont_list;		// list of contained groups
    مختصر	*si_اگلا_list;		// nextgroup IDs after this item ends
    reg_extmatch_T *si_extmatch;	// \z(...\) matches from start
					//نمونہ
} stateitem_T;

#تعریف_کرو KEYWORD_IDX	(-1)	    // value of si_idx for keywords
#تعریف_کرو ID_LIST_ALL	((مختصر *)-1) // valid of si_cont_list for containing all
				    // but contained groups

#اگر_تعریف_شدہ FEAT_CONCEAL
ساکن صحیح اگلا_seqnr = 1;		// value to use for si_seqnr
#اگر_ختم

/*
 * Struct to reduce the number of arguments to get_syn_options(), it's used
 * very often.
 */
قسمتعریف ساخت
{
    صحیح		flags;		// flags for contained and transparent
    صحیح		keyword;	// TRUE for ":syn keyword"
    صحیح		*sync_idx;	// syntax item for "grouphere" argument, NULL
				// if not allowed
    حرف	has_cont_list;	// TRUE if "cont_list" can be used
    مختصر	*cont_list;	// group IDs for "contains" argument
    مختصر	*cont_اندر_list;	// group IDs for "containedin" argument
    مختصر	*اگلا_list;	// group IDs for "nextgroup" argument
} syn_opt_arg_T;

/*
 * The next possible match in the current line for any pattern is remembered,
 * to avoid having to try for a match in each column.
 * If next_match_idx == -1, not tried (in this line) yet.
 * If next_match_col == MAXCOL, no match found in this line.
 * (All end positions have the column of the char after the end)
 */
ساکن صحیح اگلا_مطابقت_ستون;		// column for start of next match
ساکن lpos_T اگلا_مطابقت_m_endpos;	// position for end of next match
ساکن lpos_T اگلا_مطابقت_h_startpos;	// pos. for highl. start of next match
ساکن lpos_T اگلا_مطابقت_h_endpos;	// pos. for highl. end of next match
ساکن صحیح اگلا_مطابقت_idx;		// index of matched item
ساکن طویل اگلا_مطابقت_flags;		// flags for next match
ساکن lpos_T اگلا_مطابقت_eos_pos;	// end of start pattn (start region)
ساکن lpos_T اگلا_مطابقت_eoe_pos;	// pos. for end of end pattern
ساکن صحیح اگلا_مطابقت_انتہا_idx;		// ID of group for end pattn or zero
ساکن reg_extmatch_T *اگلا_مطابقت_extmatch = تہی;

/*
 * A state stack is an array of integers or stateitem_T, stored in a
 * garray_T.  A state stack is invalid if its itemsize entry is zero.
 */
#تعریف_کرو INVALID_STATE(ssp)  ((ssp)->ga_itemsize == 0)
#تعریف_کرو VALID_STATE(ssp)    ((ssp)->ga_itemsize != 0)

#تعریف_کرو FOR_ALL_SYNSTATES(sb, sst) \
    برائے ((sst) = (sb)->b_sst_پہلا; (sst) != تہی; (sst) = (sst)->sst_اگلا)

/*
 * The current state (within the line) of the recognition engine.
 * When current_state.ga_itemsize is 0 the current state is invalid.
 */
ساکن win_T	*syn_win;		// current window for highlighting
ساکن buf_T	*syn_buf;		// current buffer for highlighting
ساکن synblock_T *syn_بلاک;		// current buffer for highlighting
ساکن linenr_T current_lnum = 0;	// lnum of current state
ساکن colnr_T	current_ستون = 0;	// column of current state
ساکن صحیح	current_state_stored = 0; // TRUE if stored current state
					  // after setting current_finished
ساکن صحیح	current_ختم_شدہ = 0;	// current line has been finished
ساکن garray_T current_state		// current stack of state_items
		= {0, 0, 0, 0, تہی};
ساکن مختصر	*current_اگلا_list = تہی; // when non-zero, nextgroup list
ساکن صحیح	current_اگلا_flags = 0; // flags for current_next_list
ساکن صحیح	current_سطر_id = 0;	// unique number for current line

#تعریف_کرو CUR_STATE(idx)	((stateitem_T *)(current_state.ga_ڈیٹا))[idx]

ساکن خالی syn_sync(win_T *wp, linenr_T lnum, synstate_T *آخری_valid);
ساکن صحیح syn_مطابقت_linecont(linenr_T lnum);
ساکن خالی syn_آغاز_سطر(خالی);
ساکن خالی syn_update_سرے(صحیح startofline);
ساکن خالی syn_ڈھیر_alloc(خالی);
ساکن صحیح syn_ڈھیر_cleanup(خالی);
ساکن خالی syn_ڈھیر_free_اندراج(synblock_T *بلاک, synstate_T *p);
ساکن synstate_T *syn_ڈھیر_ڈھونڈو_اندراج(linenr_T lnum);
ساکن synstate_T *store_current_state(خالی);
ساکن خالی بھرو_current_state(synstate_T *from);
ساکن خالی invalidate_current_state(خالی);
ساکن صحیح syn_ڈھیر_equal(synstate_T *sp);
ساکن خالی validate_current_state(خالی);
ساکن صحیح syn_اختتام_سطر(صحیح syncing);
ساکن صحیح syn_current_attr(صحیح syncing, صحیح displaying, صحیح *can_spell, صحیح keep_state);
ساکن صحیح did_مطابقت_already(صحیح idx, garray_T *gap);
ساکن stateitem_T *ڈالو_اگلا_مطابقت(stateitem_T *cur_si);
ساکن خالی check_state_سرے(خالی);
ساکن خالی update_si_attr(صحیح idx);
ساکن خالی check_keepend(خالی);
ساکن خالی update_si_انتہا(stateitem_T *sip, صحیح startcol, صحیح force);
ساکن مختصر *copy_id_list(مختصر *list);
ساکن صحیح اندر_id_list(stateitem_T *شے, مختصر *cont_list, ساخت sp_syn *ssp, صحیح flags);
ساکن صحیح ڈالو_current_state(صحیح idx);
ساکن خالی نکالو_current_state(خالی);
#اگر_تعریف_شدہ FEAT_PROFILE
ساکن خالی syn_clear_وقت(syn_وقت_T *tt);
ساکن خالی syntime_clear(خالی);
ساکن خالی syntime_report(خالی);
ساکن صحیح syn_وقت_on = FALSE;
# define IF_SYN_وقت(p) (p)
#ورنہ
# define IF_SYN_وقت(p) تہی
قسمتعریف صحیح syn_وقت_T;
#اگر_ختم

ساکن خالی syn_ڈھیر_apply_changes_بلاک(synblock_T *بلاک, buf_T *buf);
ساکن خالی ڈھونڈو_endpos(صحیح idx, lpos_T *startpos, lpos_T *m_endpos, lpos_T *hl_endpos, طویل *flagsp, lpos_T *انتہا_endpos, صحیح *انتہا_idx, reg_extmatch_T *آغاز_ext);

ساکن خالی حد_بندی_pos(lpos_T *pos, lpos_T *حد_بندی);
ساکن خالی حد_بندی_pos_zero(lpos_T *pos, lpos_T *حد_بندی);
ساکن خالی syn_جوڑو_انتہا_off(lpos_T *نتیجہ, regmmatch_T *regmatch, synpat_T *spp, صحیح idx, صحیح extra);
ساکن خالی syn_جوڑو_آغاز_off(lpos_T *نتیجہ, regmmatch_T *regmatch, synpat_T *spp, صحیح idx, صحیح extra);
ساکن char_u *syn_getcurline(خالی);
ساکن colnr_T syn_getcurline_len(خالی);
ساکن صحیح syn_regexec(regmmatch_T *rmp, linenr_T lnum, colnr_T ستون, syn_وقت_T *st);
ساکن صحیح check_keyword_id(char_u *سطر, صحیح startcol, صحیح *endcol, طویل *flags, مختصر **اگلا_list, stateitem_T *cur_si, صحیح *ccharp);
ساکن خالی syn_remove_نمونہ(synblock_T *بلاک, صحیح idx);
ساکن خالی syn_clear_نمونہ(synblock_T *بلاک, صحیح i);
ساکن خالی syn_clear_cluster(synblock_T *بلاک, صحیح i);
ساکن خالی syn_clear_one(صحیح id, صحیح syncing);
ساکن خالی syn_cmd_onoff(exarg_T *eap, حرف *name);
ساکن خالی syn_سطریں_پیغام(خالی);
ساکن خالی syn_مطابقت_پیغام(خالی);
ساکن خالی syn_list_one(صحیح id, صحیح syncing, صحیح link_only);
ساکن خالی syn_list_cluster(صحیح id);
ساکن خالی put_id_list(char_u *name, مختصر *list, صحیح attr);
ساکن خالی put_نمونہ(حرف *s, صحیح c, synpat_T *spp, صحیح attr);
ساکن صحیح syn_list_keywords(صحیح id, hashtab_T *ht, صحیح did_سرخی, صحیح attr);
ساکن خالی syn_clear_keyword(صحیح id, hashtab_T *ht);
ساکن خالی clear_keywtab(hashtab_T *ht);
ساکن صحیح syn_scl_namen2id(char_u *linep, صحیح len);
ساکن صحیح syn_check_cluster(char_u *pp, صحیح len);
ساکن صحیح syn_جوڑو_cluster(char_u *name);
ساکن خالی init_syn_patterns(خالی);
ساکن char_u *get_syn_نمونہ(char_u *arg, synpat_T *ci);
ساکن صحیح get_id_list(char_u **arg, صحیح keylen, مختصر **list, صحیح skip);
ساکن خالی syn_combine_list(مختصر **clstr1, مختصر **clstr2, صحیح list_op);
ساکن خالی idl_cache_clear(synblock_T *بلاک);

/*
 * Start the syntax recognition for a line.  This function is normally called
 * from the screen updating, once for each displayed line.
 * The buffer is remembered in syn_buf, because get_syntax_attr() doesn't get
 * it.	Careful: curbuf and curwin are likely to point to another buffer and
 * window.
 */
    خالی
syntax_آغاز(win_T *wp, linenr_T lnum)
{
    synstate_T	*p;
    synstate_T	*آخری_valid = تہی;
    synstate_T	*آخری_min_valid = تہی;
    synstate_T	*sp, *پچھلا = تہی;
    linenr_T	parsed_lnum;
    linenr_T	پہلا_stored;
    صحیح		dist;
    ساکن varnumber_T changedtick = 0;	// remember the last change ID

#اگر_تعریف_شدہ FEAT_CONCEAL
    current_sub_char = NUL;
#اگر_ختم

    /*
     * After switching buffers, invalidate current_state.
     * Also do this when a change was made, the current state may be invalid
     * then.
     */
    اگر (syn_بلاک != wp->w_s
	    || syn_buf != wp->w_بفر
	    || changedtick != CHANGEDTICK(syn_buf))
    {
	invalidate_current_state();
	syn_buf = wp->w_بفر;
	syn_بلاک = wp->w_s;
    }
    changedtick = CHANGEDTICK(syn_buf);
    syn_win = wp;

    /*
     * Allocate syntax stack when needed.
     */
    syn_ڈھیر_alloc();
    اگر (syn_بلاک->b_sst_array == تہی)
	واپس;		// out of memory
    syn_بلاک->b_sst_lasttick = display_tick;

    /*
     * If the state of the end of the previous line is useful, store it.
     */
    اگر (VALID_STATE(&current_state)
	    && current_lnum < lnum
	    && current_lnum < syn_buf->b_ml.ml_سطر_گنتی)
    {
	(خالی)syn_اختتام_سطر(FALSE);
	اگر (!current_state_stored)
	{
	    ++current_lnum;
	    (خالی)store_current_state();
	}

	/*
	 * If the current_lnum is now the same as "lnum", keep the current
	 * state (this happens very often!).  Otherwise invalidate
	 * current_state and figure it out below.
	 */
	اگر (current_lnum != lnum)
	    invalidate_current_state();
    }
    ورنہ
	invalidate_current_state();

    /*
     * Try to synchronize from a saved state in b_sst_array[].
     * Only do this if lnum is not before and not to far beyond a saved state.
     */
    اگر (INVALID_STATE(&current_state) && syn_بلاک->b_sst_array != تہی)
    {
	// Find last valid saved state before start_lnum.
	FOR_ALL_SYNSTATES(syn_بلاک, p)
	{
	    اگر (p->sst_lnum > lnum)
		توڑو;
	    اگر (p->sst_lnum <= lnum && p->sst_change_lnum == 0)
	    {
		آخری_valid = p;
		اگر (p->sst_lnum >= lnum - syn_بلاک->b_syn_sync_minlines)
		    آخری_min_valid = p;
	    }
	}
	اگر (آخری_min_valid != تہی)
	    بھرو_current_state(آخری_min_valid);
    }

    /*
     * If "lnum" is before or far beyond a line with a saved state, need to
     * re-synchronize.
     */
    اگر (INVALID_STATE(&current_state))
    {
	syn_sync(wp, lnum, آخری_valid);
	اگر (current_lnum == 1)
	    // First line is always valid, no matter "minlines".
	    پہلا_stored = 1;
	ورنہ
	    // Need to parse "minlines" lines before state can be considered
	    // valid to store.
	    پہلا_stored = current_lnum + syn_بلاک->b_syn_sync_minlines;
    }
    ورنہ
	پہلا_stored = current_lnum;

    /*
     * Advance from the sync point or saved state until the current line.
     * Save some entries for syncing with later on.
     */
    اگر (syn_بلاک->b_sst_len <= صفیں)
	dist = 999999;
    ورنہ
	dist = syn_buf->b_ml.ml_سطر_گنتی / (syn_بلاک->b_sst_len - صفیں) + 1;
    جب (current_lnum < lnum)
    {
	syn_آغاز_سطر();
	(خالی)syn_اختتام_سطر(FALSE);
	++current_lnum;

	// If we parsed at least "minlines" lines or started at a valid
	// state, the current state is considered valid.
	اگر (current_lnum >= پہلا_stored)
	{
	    // Check if the saved state entry is for the current line and is
	    // equal to the current state.  If so, then validate all saved
	    // states that depended on a change before the parsed line.
	    اگر (پچھلا == تہی)
		پچھلا = syn_ڈھیر_ڈھونڈو_اندراج(current_lnum - 1);
	    اگر (پچھلا == تہی)
		sp = syn_بلاک->b_sst_پہلا;
	    ورنہ
		sp = پچھلا;
	    جب (sp != تہی && sp->sst_lnum < current_lnum)
		sp = sp->sst_اگلا;
	    اگر (sp != تہی
		    && sp->sst_lnum == current_lnum
		    && syn_ڈھیر_equal(sp))
	    {
		parsed_lnum = current_lnum;
		پچھلا = sp;
		جب (sp != تہی && sp->sst_change_lnum <= parsed_lnum)
		{
		    اگر (sp->sst_lnum <= lnum)
			// valid state before desired line, use this one
			پچھلا = sp;
		    ورنہ اگر (sp->sst_change_lnum == 0)
			// past saved states depending on change, break here.
			توڑو;
		    sp->sst_change_lnum = 0;
		    sp = sp->sst_اگلا;
		}
		بھرو_current_state(پچھلا);
	    }
	    // Store the state at this line when it's the first one, the line
	    // where we start parsing, or some distance from the previously
	    // saved state.  But only when parsed at least 'minlines'.
	    ورنہ اگر (پچھلا == تہی
			|| current_lnum == lnum
			|| current_lnum >= پچھلا->sst_lnum + dist)
		پچھلا = store_current_state();
	}

	// This can take a long time: break when CTRL-C pressed.  The current
	// state will be wrong then.
	سطر_breakcheck();
	اگر (got_int)
	{
	    current_lnum = lnum;
	    توڑو;
	}
    }

    syn_آغاز_سطر();
}

/*
 * We cannot simply discard growarrays full of state_items or buf_states; we
 * have to manually release their extmatch pointers first.
 */
    ساکن خالی
clear_syn_state(synstate_T *p)
{
    صحیح		i;
    garray_T	*gap;

    اگر (p->sst_stacksize > SST_FIX_حالتیں)
    {
	gap = &(p->sst_union.sst_ga);
	برائے (i = 0; i < gap->ga_len; i++)
	    unref_extmatch(SYN_STATE_P(gap)[i].bs_extmatch);
	ga_clear(gap);
    }
    ورنہ
    {
	برائے (i = 0; i < p->sst_stacksize; i++)
	    unref_extmatch(p->sst_union.sst_ڈھیر[i].bs_extmatch);
    }
}

/*
 * Cleanup the current_state stack.
 */
    ساکن خالی
clear_current_state(خالی)
{
    صحیح		i;
    stateitem_T	*sip;

    sip = (stateitem_T *)(current_state.ga_ڈیٹا);
    برائے (i = 0; i < current_state.ga_len; i++)
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
    ساکن خالی
syn_sync(
    win_T	*wp,
    linenr_T	آغاز_lnum,
    synstate_T	*آخری_valid)
{
    buf_T	*curbuf_save;
    win_T	*curwin_save;
    pos_T	cursor_save;
    صحیح		idx;
    linenr_T	lnum;
    linenr_T	انتہا_lnum;
    linenr_T	break_lnum;
    صحیح		had_sync_نقطہ;
    stateitem_T	*cur_si;
    synpat_T	*spp;
    صحیح		ملا_flags = 0;
    صحیح		ملا_مطابقت_idx = 0;
    linenr_T	ملا_current_lnum = 0;
    صحیح		ملا_current_ستون = 0;
    lpos_T	ملا_m_endpos;
    colnr_T	پچھلا_current_ستون;

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
    اگر (syn_بلاک->b_syn_sync_minlines > آغاز_lnum)
	آغاز_lnum = 1;
    ورنہ
    {
	اگر (syn_بلاک->b_syn_sync_minlines == 1)
	    lnum = 1;
	ورنہ اگر (syn_بلاک->b_syn_sync_minlines < 10)
	    lnum = syn_بلاک->b_syn_sync_minlines * 2;
	ورنہ
	    lnum = syn_بلاک->b_syn_sync_minlines * 3 / 2;
	اگر (syn_بلاک->b_syn_sync_maxlines != 0
				     && lnum > syn_بلاک->b_syn_sync_maxlines)
	    lnum = syn_بلاک->b_syn_sync_maxlines;
	اگر (lnum >= آغاز_lnum)
	    آغاز_lnum = 1;
	ورنہ
	    آغاز_lnum -= lnum;
    }
    current_lnum = آغاز_lnum;

    /*
     * 1. Search backwards for the end of a C-style comment.
     */
    اگر (syn_بلاک->b_syn_sync_flags & SF_CCOMMENT)
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
	برائے ( ; آغاز_lnum > 1; --آغاز_lnum)
	{
	    char_u	*l = ml_get(آغاز_lnum - 1);

	    اگر (*l == NUL || *(l + ml_get_len(آغاز_lnum - 1) - 1) != '\\')
		توڑو;
	}
	current_lnum = آغاز_lnum;

	// set cursor to start of search
	cursor_save = wp->w_cursor;
	wp->w_cursor.lnum = آغاز_lnum;
	wp->w_cursor.ستون = 0;

	/*
	 * If the line is inside a comment, need to find the syntax item that
	 * defines the comment.
	 * Restrict the search for the end of a comment to b_syn_sync_maxlines.
	 */
	اگر (ڈھونڈو_آغاز_comment((صحیح)syn_بلاک->b_syn_sync_maxlines) != تہی)
	{
	    برائے (idx = syn_بلاک->b_syn_patterns.ga_len; --idx >= 0; )
		اگر (SYN_اشیاء(syn_بلاک)[idx].sp_syn.id
						   == syn_بلاک->b_syn_sync_id
			&& SYN_اشیاء(syn_بلاک)[idx].sp_type == SPTYPE_آغاز)
		{
		    validate_current_state();
		    اگر (ڈالو_current_state(idx) == OK)
			update_si_attr(current_state.ga_len - 1);
		    توڑو;
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
    ورنہ اگر (syn_بلاک->b_syn_sync_flags & SF_مطابقت)
    {
	اگر (syn_بلاک->b_syn_sync_maxlines != 0
			       && آغاز_lnum > syn_بلاک->b_syn_sync_maxlines)
	    break_lnum = آغاز_lnum - syn_بلاک->b_syn_sync_maxlines;
	ورنہ
	    break_lnum = 0;

	ملا_m_endpos.lnum = 0;
	ملا_m_endpos.ستون = 0;
	انتہا_lnum = آغاز_lnum;
	lnum = آغاز_lnum;
	جب (--lnum > break_lnum)
	{
	    // This can take a long time: break when CTRL-C pressed.
	    سطر_breakcheck();
	    اگر (got_int)
	    {
		invalidate_current_state();
		current_lnum = آغاز_lnum;
		توڑو;
	    }

	    // Check if we have run into a valid saved state stack now.
	    اگر (آخری_valid != تہی && lnum == آخری_valid->sst_lnum)
	    {
		بھرو_current_state(آخری_valid);
		توڑو;
	    }

	    /*
	     * Check if the previous line has the line-continuation pattern.
	     */
	    اگر (lnum > 1 && syn_مطابقت_linecont(lnum - 1))
		جاری;

	    /*
	     * Start with nothing on the state stack
	     */
	    validate_current_state();

	    برائے (current_lnum = lnum; current_lnum < انتہا_lnum; ++current_lnum)
	    {
		syn_آغاز_سطر();
		برائے (;;)
		{
		    had_sync_نقطہ = syn_اختتام_سطر(TRUE);
		    /*
		     * When a sync point has been found, remember where, and
		     * continue to look for another one, further on in the line.
		     */
		    اگر (had_sync_نقطہ && current_state.ga_len)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			اگر (cur_si->si_m_endpos.lnum > آغاز_lnum)
			{
			    // ignore match that goes to after where started
			    current_lnum = انتہا_lnum;
			    توڑو;
			}
			اگر (cur_si->si_idx < 0)
			{
			    // Cannot happen?
			    ملا_flags = 0;
			    ملا_مطابقت_idx = KEYWORD_IDX;
			}
			ورنہ
			{
			    spp = &(SYN_اشیاء(syn_بلاک)[cur_si->si_idx]);
			    ملا_flags = spp->sp_flags;
			    ملا_مطابقت_idx = spp->sp_sync_idx;
			}
			ملا_current_lnum = current_lnum;
			ملا_current_ستون = current_ستون;
			ملا_m_endpos = cur_si->si_m_endpos;
			/*
			 * Continue after the match (be aware of a zero-length
			 * match).
			 */
			اگر (ملا_m_endpos.lnum > current_lnum)
			{
			    current_lnum = ملا_m_endpos.lnum;
			    current_ستون = ملا_m_endpos.ستون;
			    اگر (current_lnum >= انتہا_lnum)
				توڑو;
			}
			ورنہ اگر (ملا_m_endpos.ستون > current_ستون)
			    current_ستون = ملا_m_endpos.ستون;
			ورنہ
			    ++current_ستون;

			// syn_current_attr() will have skipped the check for
			// an item that ends here, need to do that now.  Be
			// careful not to go past the NUL.
			پچھلا_current_ستون = current_ستون;
			اگر (syn_getcurline()[current_ستون] != NUL)
			    ++current_ستون;
			check_state_سرے();
			current_ستون = پچھلا_current_ستون;
		    }
		    ورنہ
			توڑو;
		}
	    }

	    /*
	     * If a sync point was encountered, break here.
	     */
	    اگر (ملا_flags)
	    {
		/*
		 * Put the item that was specified by the sync point on the
		 * state stack.  If there was no item specified, make the
		 * state stack empty.
		 */
		clear_current_state();
		اگر (ملا_مطابقت_idx >= 0
			&& ڈالو_current_state(ملا_مطابقت_idx) == OK)
		    update_si_attr(current_state.ga_len - 1);

		/*
		 * When using "grouphere", continue from the sync point
		 * match, until the end of the line.  Parsing starts at
		 * the next line.
		 * For "groupthere" the parsing starts at start_lnum.
		 */
		اگر (ملا_flags & HL_SYNC_یہاں)
		{
		    current_lnum = ملا_m_endpos.lnum;
		    current_ستون = ملا_m_endpos.ستون;
		    اگر (current_state.ga_len)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			cur_si->si_h_startpos.lnum = ملا_current_lnum;
			cur_si->si_h_startpos.ستون = ملا_current_ستون;
			update_si_انتہا(cur_si, (صحیح)current_ستون, TRUE);
			check_keepend();
		    }
		    (خالی)syn_اختتام_سطر(FALSE);
		    ++current_lnum;
		}
		ورنہ
		    current_lnum = آغاز_lnum;

		توڑو;
	    }

	    انتہا_lnum = lnum;
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

    ساکن خالی
save_chartab(char_u *chartab)
{
    اگر (syn_بلاک->b_syn_isk == خالی_خانے_option)
	واپس;

    mch_memmove(chartab, syn_buf->b_chartab, (size_t)32);
    mch_memmove(syn_buf->b_chartab, syn_win->w_s->b_syn_chartab, (size_t)32);
}

    ساکن خالی
restore_chartab(char_u *chartab)
{
    اگر (syn_win->w_s->b_syn_isk != خالی_خانے_option)
	mch_memmove(syn_buf->b_chartab, chartab, (size_t)32);
}

/*
 * Return TRUE if the line-continuation pattern matches in line "lnum".
 */
    ساکن صحیح
syn_مطابقت_linecont(linenr_T lnum)
{
    regmmatch_T regmatch;
    صحیح r;
    char_u	buf_chartab[32];  // chartab array for syn iskyeyword

    اگر (syn_بلاک->b_syn_linecont_prog == تہی)
	واپس FALSE;

    // use syntax iskeyword option
    save_chartab(buf_chartab);
    regmatch.rmm_ic = syn_بلاک->b_syn_linecont_ic;
    regmatch.regprog = syn_بلاک->b_syn_linecont_prog;
    r = syn_regexec(&regmatch, lnum, (colnr_T)0,
	    IF_SYN_وقت(&syn_بلاک->b_syn_linecont_وقت));
    syn_بلاک->b_syn_linecont_prog = regmatch.regprog;
    restore_chartab(buf_chartab);
    واپس r;
}

/*
 * Prepare the current state for the start of a line.
 */
    ساکن خالی
syn_آغاز_سطر(خالی)
{
    current_ختم_شدہ = FALSE;
    current_ستون = 0;

    /*
     * Need to update the end of a start/skip/end that continues from the
     * previous line and regions that have "keepend".
     */
    اگر (current_state.ga_len > 0)
    {
	syn_update_سرے(TRUE);
	check_state_سرے();
    }

    اگلا_مطابقت_idx = -1;
    ++current_سطر_id;
#اگر_تعریف_شدہ FEAT_CONCEAL
    اگلا_seqnr = 1;
#اگر_ختم
}

/*
 * Check for items in the stack that need their end updated.
 * When "startofline" is TRUE the last item is always updated.
 * When "startofline" is FALSE the item with "keepend" is forcefully updated.
 */
    ساکن خالی
syn_update_سرے(صحیح startofline)
{
    stateitem_T	*cur_si;
    صحیح		i;
    صحیح		نظر_آیا_keepend;

    اگر (startofline)
    {
	// Check for a match carried over from a previous line with a
	// contained region.  The match ends as soon as the region ends.
	برائے (i = 0; i < current_state.ga_len; ++i)
	{
	    cur_si = &CUR_STATE(i);
	    اگر (cur_si->si_idx >= 0
		    && (SYN_اشیاء(syn_بلاک)[cur_si->si_idx]).sp_type
							       == SPTYPE_مطابقت
		    && cur_si->si_m_endpos.lnum < current_lnum)
	    {
		cur_si->si_flags |= HL_MATCHCONT;
		cur_si->si_m_endpos.lnum = 0;
		cur_si->si_m_endpos.ستون = 0;
		cur_si->si_h_endpos = cur_si->si_m_endpos;
		cur_si->si_سرے = TRUE;
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
	برائے ( ; i > keepend_level; --i)
	    اگر (CUR_STATE(i).si_flags & HL_EXTEND)
		توڑو;

    نظر_آیا_keepend = FALSE;
    برائے ( ; i < current_state.ga_len; ++i)
    {
	cur_si = &CUR_STATE(i);
	اگر ((cur_si->si_flags & HL_KEEPEND)
			    || (نظر_آیا_keepend && !startofline)
			    || (i == current_state.ga_len - 1 && startofline))
	{
	    cur_si->si_h_startpos.ستون = 0;	// start highl. in col 0
	    cur_si->si_h_startpos.lnum = current_lnum;

	    اگر (!(cur_si->si_flags & HL_MATCHCONT))
		update_si_انتہا(cur_si, (صحیح)current_ستون, !startofline);

	    اگر (!startofline && (cur_si->si_flags & HL_KEEPEND))
		نظر_آیا_keepend = TRUE;
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

    ساکن خالی
syn_ڈھیر_free_بلاک(synblock_T *بلاک)
{
    synstate_T	*p;

    // Syntax definitions may have changed: drop the in_id_list() cache.
    idl_cache_clear(بلاک);

    اگر (بلاک->b_sst_array == تہی)
	واپس;

    FOR_ALL_SYNSTATES(بلاک, p)
	clear_syn_state(p);
    VIM_CLEAR(بلاک->b_sst_array);
    بلاک->b_sst_پہلا = تہی;
    بلاک->b_sst_تلاش = تہی;
    بلاک->b_sst_len = 0;
}
/*
 * Free b_sst_array[] for buffer "buf".
 * Used when syntax items changed to force resyncing everywhere.
 */
    خالی
syn_ڈھیر_free_all(synblock_T *بلاک)
{
#اگر_تعریف_شدہ FEAT_FOLDING
    win_T	*wp;
#اگر_ختم

    syn_ڈھیر_free_بلاک(بلاک);

#اگر_تعریف_شدہ FEAT_FOLDING
    // When using "syntax" fold method, must update all folds.
    FOR_ALL_WINDOWS(wp)
    {
	اگر (wp->w_s == بلاک && foldmethodIsSyntax(wp))
	    foldUpdateAll(wp);
    }
#اگر_ختم
}

/*
 * Allocate the syntax state stack for syn_buf when needed.
 * If the number of entries in b_sst_array[] is much too big or a bit too
 * small, reallocate it.
 * Also used to allocate b_sst_array[] for the first time.
 */
    ساکن خالی
syn_ڈھیر_alloc(خالی)
{
    طویل	len;
    synstate_T	*مقصد, *from;
    synstate_T	*sstp;

    len = syn_buf->b_ml.ml_سطر_گنتی / SST_DIST + صفیں * 2;
    اگر (len < SST_MIN_ENTRIES)
	len = SST_MIN_ENTRIES;
    ورنہ اگر (len > SST_MAX_ENTRIES)
	len = SST_MAX_ENTRIES;
    اگر (syn_بلاک->b_sst_len > len * 2 || syn_بلاک->b_sst_len < len)
    {
	// Allocate 50% too much, to avoid reallocating too often.
	len = syn_buf->b_ml.ml_سطر_گنتی;
	len = (len + len / 2) / SST_DIST + صفیں * 2;
	اگر (len < SST_MIN_ENTRIES)
	    len = SST_MIN_ENTRIES;
	ورنہ اگر (len > SST_MAX_ENTRIES)
	    len = SST_MAX_ENTRIES;

	اگر (syn_بلاک->b_sst_array != تہی)
	{
	    // When shrinking the array, cleanup the existing stack.
	    // Make sure that all valid entries fit in the new array.
	    جب (syn_بلاک->b_sst_len - syn_بلاک->b_sst_freecount + 2 > len
		    && syn_ڈھیر_cleanup())
		;
	    اگر (len < syn_بلاک->b_sst_len - syn_بلاک->b_sst_freecount + 2)
		len = syn_بلاک->b_sst_len - syn_بلاک->b_sst_freecount + 2;
	}

	sstp = ALLOC_CLEAR_MULT(synstate_T, len);
	اگر (sstp == تہی)	// out of memory!
	    واپس;

	مقصد = sstp - 1;
	اگر (syn_بلاک->b_sst_array != تہی)
	{
	    // Move the states from the old array to the new one.
	    برائے (from = syn_بلاک->b_sst_پہلا; from != تہی;
							from = from->sst_اگلا)
	    {
		++مقصد;
		*مقصد = *from;
		مقصد->sst_اگلا = مقصد + 1;
	    }
	}
	اگر (مقصد != sstp - 1)
	{
	    مقصد->sst_اگلا = تہی;
	    syn_بلاک->b_sst_پہلا = sstp;
	    syn_بلاک->b_sst_freecount = len - (صحیح)(مقصد - sstp) - 1;
	}
	ورنہ
	{
	    syn_بلاک->b_sst_پہلا = تہی;
	    syn_بلاک->b_sst_freecount = len;
	}

	// Create the list of free entries.
	syn_بلاک->b_sst_firstfree = مقصد + 1;
	جب (++مقصد < sstp + len)
	    مقصد->sst_اگلا = مقصد + 1;
	(sstp + len - 1)->sst_اگلا = تہی;

	vim_free(syn_بلاک->b_sst_array);
	syn_بلاک->b_sst_array = sstp;
	syn_بلاک->b_sst_len = len;
	// The entries were moved to a new array, drop the stale pointer.
	syn_بلاک->b_sst_تلاش = تہی;
    }
}

/*
 * Check for changes in a buffer to affect stored syntax states.  Uses the
 * b_mod_* fields.
 * Called from update_screen(), before screen is being updated, once for each
 * displayed buffer.
 */
    خالی
syn_ڈھیر_apply_changes(buf_T *buf)
{
    win_T	*wp;

    syn_ڈھیر_apply_changes_بلاک(&buf->b_s, buf);

    FOR_ALL_WINDOWS(wp)
    {
	اگر ((wp->w_بفر == buf) && (wp->w_s != &buf->b_s))
	    syn_ڈھیر_apply_changes_بلاک(wp->w_s, buf);
    }
}

    ساکن خالی
syn_ڈھیر_apply_changes_بلاک(synblock_T *بلاک, buf_T *buf)
{
    synstate_T	*p, *پچھلا, *np;
    linenr_T	n;

    پچھلا = تہی;
    برائے (p = بلاک->b_sst_پہلا; p != تہی; )
    {
	اگر (p->sst_lnum + بلاک->b_syn_sync_linebreaks > buf->b_mod_چوٹی)
	{
	    n = p->sst_lnum + buf->b_mod_xlines;
	    اگر (n <= buf->b_mod_bot)
	    {
		// this state is inside the changed area, remove it
		np = p->sst_اگلا;
		اگر (پچھلا == تہی)
		    بلاک->b_sst_پہلا = np;
		ورنہ
		    پچھلا->sst_اگلا = np;
		syn_ڈھیر_free_اندراج(بلاک, p);
		p = np;
		جاری;
	    }
	    // This state is below the changed area.  Remember the line
	    // that needs to be parsed before this entry can be made valid
	    // again.
	    اگر (p->sst_change_lnum != 0 && p->sst_change_lnum > buf->b_mod_چوٹی)
	    {
		اگر (p->sst_change_lnum + buf->b_mod_xlines > buf->b_mod_چوٹی)
		    p->sst_change_lnum += buf->b_mod_xlines;
		ورنہ
		    p->sst_change_lnum = buf->b_mod_چوٹی;
	    }
	    اگر (p->sst_change_lnum == 0
		    || p->sst_change_lnum < buf->b_mod_bot)
		p->sst_change_lnum = buf->b_mod_bot;

	    p->sst_lnum = n;
	}
	پچھلا = p;
	p = p->sst_اگلا;
    }
}

/*
 * Reduce the number of entries in the state stack for syn_buf.
 * Returns TRUE if at least one entry was freed.
 */
    ساکن صحیح
syn_ڈھیر_cleanup(خالی)
{
    synstate_T	*p, *پچھلا;
    disptick_T	tick;
    صحیح		above;
    صحیح		dist;
    صحیح		retval = FALSE;

    اگر (syn_بلاک->b_sst_پہلا == تہی)
	واپس retval;

    // Compute normal distance between non-displayed entries.
    اگر (syn_بلاک->b_sst_len <= صفیں)
	dist = 999999;
    ورنہ
	dist = syn_buf->b_ml.ml_سطر_گنتی / (syn_بلاک->b_sst_len - صفیں) + 1;

    /*
     * Go through the list to find the "tick" for the oldest entry that can
     * be removed.  Set "above" when the "tick" for the oldest entry is above
     * "b_sst_lasttick" (the display tick wraps around).
     */
    tick = syn_بلاک->b_sst_lasttick;
    above = FALSE;
    پچھلا = syn_بلاک->b_sst_پہلا;
    برائے (p = پچھلا->sst_اگلا; p != تہی; پچھلا = p, p = p->sst_اگلا)
    {
	اگر (پچھلا->sst_lnum + dist > p->sst_lnum)
	{
	    اگر (p->sst_tick > syn_بلاک->b_sst_lasttick)
	    {
		اگر (!above || p->sst_tick < tick)
		    tick = p->sst_tick;
		above = TRUE;
	    }
	    ورنہ اگر (!above && p->sst_tick < tick)
		tick = p->sst_tick;
	}
    }

    /*
     * Go through the list to make the entries for the oldest tick at an
     * interval of several lines.
     */
    پچھلا = syn_بلاک->b_sst_پہلا;
    برائے (p = پچھلا->sst_اگلا; p != تہی; پچھلا = p, p = p->sst_اگلا)
    {
	اگر (p->sst_tick == tick && پچھلا->sst_lnum + dist > p->sst_lnum)
	{
	    // Move this entry from used list to free list
	    پچھلا->sst_اگلا = p->sst_اگلا;
	    syn_ڈھیر_free_اندراج(syn_بلاک, p);
	    p = پچھلا;
	    retval = TRUE;
	}
    }
    واپس retval;
}

/*
 * Free the allocated memory for a syn_state item.
 * Move the entry into the free list.
 */
    ساکن خالی
syn_ڈھیر_free_اندراج(synblock_T *بلاک, synstate_T *p)
{
    اگر (بلاک->b_sst_تلاش == p)
	بلاک->b_sst_تلاش = تہی;
    clear_syn_state(p);
    p->sst_اگلا = بلاک->b_sst_firstfree;
    بلاک->b_sst_firstfree = p;
    ++بلاک->b_sst_freecount;
}

/*
 * Find an entry in the list of state stacks at or before "lnum".
 * Returns NULL when there is no entry or the first entry is after "lnum".
 */
    ساکن synstate_T *
syn_ڈھیر_ڈھونڈو_اندراج(linenr_T lnum)
{
    synstate_T	*p, *پچھلا;

    پچھلا = تہی;
    p = syn_بلاک->b_sst_پہلا;

    // The list is sorted by line number and lookups while parsing advance
    // monotonically, so resume from the last returned entry instead of
    // rescanning from the start whenever it is at or before "lnum".
    اگر (syn_بلاک->b_sst_تلاش != تہی
				 && syn_بلاک->b_sst_تلاش->sst_lnum <= lnum)
	p = syn_بلاک->b_sst_تلاش;

    برائے ( ; p != تہی; پچھلا = p, p = p->sst_اگلا)
    {
	اگر (p->sst_lnum == lnum)
	{
	    syn_بلاک->b_sst_تلاش = p;
	    واپس p;
	}
	اگر (p->sst_lnum > lnum)
	    توڑو;
    }
    اگر (پچھلا != تہی)
	syn_بلاک->b_sst_تلاش = پچھلا;
    واپس پچھلا;
}

/*
 * Try saving the current state in b_sst_array[].
 * The current state must be valid for the start of the current_lnum line!
 */
    ساکن synstate_T *
store_current_state(خالی)
{
    صحیح		i;
    synstate_T	*p;
    bufstate_T	*bp;
    stateitem_T	*cur_si;
    synstate_T	*sp = syn_ڈھیر_ڈھونڈو_اندراج(current_lnum);

    /*
     * If the current state contains a start or end pattern that continues
     * from the previous line, we can't use it.  Don't store it then.
     */
    برائے (i = current_state.ga_len - 1; i >= 0; --i)
    {
	cur_si = &CUR_STATE(i);
	اگر (cur_si->si_h_startpos.lnum >= current_lnum
		|| cur_si->si_m_endpos.lnum >= current_lnum
		|| cur_si->si_h_endpos.lnum >= current_lnum
		|| (cur_si->si_انتہا_idx
		    && cur_si->si_eoe_pos.lnum >= current_lnum))
	    توڑو;
    }
    اگر (i >= 0)
    {
	اگر (sp != تہی)
	{
	    // find "sp" in the list and remove it
	    اگر (syn_بلاک->b_sst_پہلا == sp)
		// it's the first entry
		syn_بلاک->b_sst_پہلا = sp->sst_اگلا;
	    ورنہ
	    {
		// find the entry just before this one to adjust sst_next
		FOR_ALL_SYNSTATES(syn_بلاک, p)
		    اگر (p->sst_اگلا == sp)
			توڑو;
		اگر (p != تہی)	// just in case
		    p->sst_اگلا = sp->sst_اگلا;
	    }
	    syn_ڈھیر_free_اندراج(syn_بلاک, sp);
	    sp = تہی;
	}
    }
    ورنہ اگر (sp == تہی || sp->sst_lnum != current_lnum)
    {
	/*
	 * Add a new entry
	 */
	// If no free items, cleanup the array first.
	اگر (syn_بلاک->b_sst_freecount == 0)
	{
	    (خالی)syn_ڈھیر_cleanup();
	    // "sp" may have been moved to the freelist now
	    sp = syn_ڈھیر_ڈھونڈو_اندراج(current_lnum);
	}
	// Still no free items?  Must be a strange problem...
	اگر (syn_بلاک->b_sst_freecount == 0)
	    sp = تہی;
	ورنہ
	{
	    // Take the first item from the free list and put it in the used
	    // list, after *sp
	    p = syn_بلاک->b_sst_firstfree;
	    syn_بلاک->b_sst_firstfree = p->sst_اگلا;
	    --syn_بلاک->b_sst_freecount;
	    اگر (sp == تہی)
	    {
		// Insert in front of the list
		p->sst_اگلا = syn_بلاک->b_sst_پہلا;
		syn_بلاک->b_sst_پہلا = p;
	    }
	    ورنہ
	    {
		// insert in list after *sp
		p->sst_اگلا = sp->sst_اگلا;
		sp->sst_اگلا = p;
	    }
	    sp = p;
	    sp->sst_stacksize = 0;
	    sp->sst_lnum = current_lnum;
	    // Resume the next forward lookup from the entry just stored.
	    syn_بلاک->b_sst_تلاش = sp;
	}
    }
    اگر (sp != تہی)
    {
	// When overwriting an existing state stack, clear it first
	clear_syn_state(sp);
	sp->sst_stacksize = current_state.ga_len;
	اگر (current_state.ga_len > SST_FIX_حالتیں)
	{
	    // Need to clear it, might be something remaining from when the
	    // length was less than SST_FIX_STATES.
	    ga_init2(&sp->sst_union.sst_ga, حجم(bufstate_T), 1);
	    اگر (ga_grow(&sp->sst_union.sst_ga, current_state.ga_len) == FAIL)
		sp->sst_stacksize = 0;
	    ورنہ
		sp->sst_union.sst_ga.ga_len = current_state.ga_len;
	    bp = SYN_STATE_P(&(sp->sst_union.sst_ga));
	}
	ورنہ
	    bp = sp->sst_union.sst_ڈھیر;
	برائے (i = 0; i < sp->sst_stacksize; ++i)
	{
	    bp[i].bs_idx = CUR_STATE(i).si_idx;
	    bp[i].bs_flags = CUR_STATE(i).si_flags;
#اگر_تعریف_شدہ FEAT_CONCEAL
	    bp[i].bs_seqnr = CUR_STATE(i).si_seqnr;
	    bp[i].bs_cchar = CUR_STATE(i).si_cchar;
#اگر_ختم
	    bp[i].bs_extmatch = ref_extmatch(CUR_STATE(i).si_extmatch);
	}
	sp->sst_اگلا_flags = current_اگلا_flags;
	sp->sst_اگلا_list = current_اگلا_list;
	sp->sst_tick = display_tick;
	sp->sst_change_lnum = 0;
    }
    current_state_stored = TRUE;
    واپس sp;
}

/*
 * Copy a state stack from "from" in b_sst_array[] to current_state;
 */
    ساکن خالی
بھرو_current_state(synstate_T *from)
{
    صحیح		i;
    bufstate_T	*bp;

    clear_current_state();
    validate_current_state();
    keepend_level = -1;
    اگر (from->sst_stacksize
	    && ga_grow(&current_state, from->sst_stacksize) == OK)
    {
	اگر (from->sst_stacksize > SST_FIX_حالتیں)
	    bp = SYN_STATE_P(&(from->sst_union.sst_ga));
	ورنہ
	    bp = from->sst_union.sst_ڈھیر;
	برائے (i = 0; i < from->sst_stacksize; ++i)
	{
	    CUR_STATE(i).si_idx = bp[i].bs_idx;
	    CUR_STATE(i).si_flags = bp[i].bs_flags;
#اگر_تعریف_شدہ FEAT_CONCEAL
	    CUR_STATE(i).si_seqnr = bp[i].bs_seqnr;
	    CUR_STATE(i).si_cchar = bp[i].bs_cchar;
#اگر_ختم
	    CUR_STATE(i).si_extmatch = ref_extmatch(bp[i].bs_extmatch);
	    اگر (keepend_level < 0 && (CUR_STATE(i).si_flags & HL_KEEPEND))
		keepend_level = i;
	    CUR_STATE(i).si_سرے = FALSE;
	    CUR_STATE(i).si_m_lnum = 0;
	    اگر (CUR_STATE(i).si_idx >= 0)
		CUR_STATE(i).si_اگلا_list =
		     (SYN_اشیاء(syn_بلاک)[CUR_STATE(i).si_idx]).sp_اگلا_list;
	    ورنہ
		CUR_STATE(i).si_اگلا_list = تہی;
	    update_si_attr(i);
	}
	current_state.ga_len = from->sst_stacksize;
    }
    current_اگلا_list = from->sst_اگلا_list;
    current_اگلا_flags = from->sst_اگلا_flags;
    current_lnum = from->sst_lnum;
}

/*
 * Compare saved state stack "*sp" with the current state.
 * Return TRUE when they are equal.
 */
    ساکن صحیح
syn_ڈھیر_equal(synstate_T *sp)
{
    صحیح		i, j;
    bufstate_T	*bp;
    reg_extmatch_T	*six, *bsx;

    // First a quick check if the stacks have the same size end nextlist.
    اگر (sp->sst_stacksize != current_state.ga_len
	    || sp->sst_اگلا_list != current_اگلا_list)
	واپس FALSE;

    // Need to compare all states on both stacks.
    اگر (sp->sst_stacksize > SST_FIX_حالتیں)
	bp = SYN_STATE_P(&(sp->sst_union.sst_ga));
    ورنہ
	bp = sp->sst_union.sst_ڈھیر;

    برائے (i = current_state.ga_len; --i >= 0; )
    {
	// If the item has another index the state is different.
	اگر (bp[i].bs_idx != CUR_STATE(i).si_idx)
	    توڑو;
	اگر (bp[i].bs_extmatch == CUR_STATE(i).si_extmatch)
	    جاری;
	// When the extmatch pointers are different, the strings in them can
	// still be the same.  Check if the extmatch references are equal.
	bsx = bp[i].bs_extmatch;
	six = CUR_STATE(i).si_extmatch;
	// If one of the extmatch pointers is NULL the states are different.
	اگر (bsx == تہی || six == تہی)
	    توڑو;
	برائے (j = 0; j < NSUBEXP; ++j)
	{
	    // Check each referenced match string. They must all be equal.
	    اگر (bsx->matches[j] != six->matches[j])
	    {
		// If the pointer is different it can still be the same text.
		// Compare the strings, ignore case when the start item has the
		// sp_ic flag set.
		اگر (bsx->matches[j] == تہی || six->matches[j] == تہی)
		    توڑو;
		اگر ((SYN_اشیاء(syn_بلاک)[CUR_STATE(i).si_idx]).sp_ic
			? MB_STRICMP(bsx->matches[j], six->matches[j]) != 0
			: STRCMP(bsx->matches[j], six->matches[j]) != 0)
		    توڑو;
	    }
	}
	اگر (j != NSUBEXP)
	    توڑو;
    }
    واپس i < 0 ? TRUE : FALSE;
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
    خالی
syntax_انتہا_parsing(win_T *wp, linenr_T lnum)
{
    synstate_T	*sp;

    اگر (syn_بلاک != wp->w_s)
	واپس;  // not the right window
    sp = syn_ڈھیر_ڈھونڈو_اندراج(lnum);
    اگر (sp != تہی && sp->sst_lnum < lnum)
	sp = sp->sst_اگلا;

    اگر (sp != تہی && sp->sst_change_lnum != 0)
	sp->sst_change_lnum = lnum;
}

/*
 * End of handling of the state stack.
 ****************************************/

    ساکن خالی
invalidate_current_state(خالی)
{
    clear_current_state();
    current_state.ga_itemsize = 0;	// mark current_state invalid
    current_اگلا_list = تہی;
    keepend_level = -1;
}

    ساکن خالی
validate_current_state(خالی)
{
    current_state.ga_itemsize = حجم(stateitem_T);
    current_state.ga_growsize = 3;
}

/*
 * Return TRUE if the syntax at start of lnum changed since last time.
 * This will only be called just after get_syntax_attr() for the previous
 * line, to check if the next line needs to be redrawn too.
 */
    صحیح
syntax_check_بدلا(linenr_T lnum)
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
	sp = syn_ڈھیر_ڈھونڈو_اندراج(lnum);
	اگر (sp != تہی && sp->sst_lnum == lnum)
	{
	    /*
	     * finish the previous line (needed when not all of the line was
	     * drawn)
	     */
	    (خالی)syn_اختتام_سطر(FALSE);

	    /*
	     * Compare the current state with the previously saved state of
	     * the line.
	     */
	    اگر (syn_ڈھیر_equal(sp))
		retval = FALSE;

	    /*
	     * Store the current state in b_sst_array[] for later use.
	     */
	    ++current_lnum;
	    (خالی)store_current_state();
	}
    }

    واپس retval;
}

/*
 * Finish the current line.
 * This doesn't return any attributes, it only gets the state at the end of
 * the line.  It can start anywhere in the line, as long as the current state
 * is valid.
 */
    ساکن صحیح
syn_اختتام_سطر(
    صحیح	    syncing)		// called for syncing
{
    stateitem_T	*cur_si;
    colnr_T	پچھلا_current_ستون;

    جب (!current_ختم_شدہ)
    {
	(خالی)syn_current_attr(syncing, FALSE, تہی, FALSE);
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
		    && (SYN_اشیاء(syn_بلاک)[cur_si->si_idx].sp_flags
					  & (HL_SYNC_یہاں|HL_SYNC_THERE)))
		واپس TRUE;

	    // syn_current_attr() will have skipped the check for an item
	    // that ends here, need to do that now.  Be careful not to go
	    // past the NUL.
	    پچھلا_current_ستون = current_ستون;
	    اگر (syn_getcurline()[current_ستون] != NUL)
		++current_ستون;
	    check_state_سرے();
	    current_ستون = پچھلا_current_ستون;
	}
	++current_ستون;
    }
    واپس FALSE;
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
    colnr_T	ستون,
    صحیح		*can_spell,
    صحیح		keep_state)	// keep state of char at "col"
{
    صحیح	    attr = 0;

    اگر (can_spell != تہی)
	// Default: Only do spelling when there is no @Spell cluster or when
	// ":syn spell toplevel" was used.
	*can_spell = syn_بلاک->b_syn_spell == SYNSPL_DEFAULT
		    ? (syn_بلاک->b_spell_cluster_id == 0)
		    : (syn_بلاک->b_syn_spell == SYNSPL_چوٹی);

    // check for out of memory situation
    اگر (syn_بلاک->b_sst_array == تہی)
	واپس 0;

    // After 'synmaxcol' the attribute is always zero.
    اگر (syn_buf->b_p_smc > 0 && ستون >= (colnr_T)syn_buf->b_p_smc)
    {
	clear_current_state();
#اگر_تعریف_شدہ FEAT_EVAL
	current_id = 0;
	current_trans_id = 0;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CONCEAL
	current_flags = 0;
	current_seqnr = 0;
#اگر_ختم
	واپس 0;
    }

    // Make sure current_state is valid
    اگر (INVALID_STATE(&current_state))
	validate_current_state();

    /*
     * Skip from the current column to "col", get the attributes for "col".
     */
    جب (current_ستون <= ستون)
    {
	attr = syn_current_attr(FALSE, TRUE, can_spell,
				     current_ستون == ستون ? keep_state : FALSE);
	++current_ستون;
    }

    واپس attr;
}

/*
 * Get syntax attributes for current_lnum, current_col.
 */
    ساکن صحیح
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
    صحیح		انتہا_idx;	// group ID for end pattern
    صحیح		idx;
    synpat_T	*spp;
    stateitem_T	*cur_si, *sip = تہی;
    صحیح		startcol;
    صحیح		endcol;
    طویل	flags;
    صحیح		cchar;
    مختصر	*اگلا_list;
    صحیح		ملا_مطابقت;		    // found usable match
    ساکن صحیح	try_اگلا_عمود = FALSE;    // must try in next col
    صحیح		do_keywords;
    regmmatch_T	regmatch;
    lpos_T	pos;
    صحیح		lc_ستون;
    reg_extmatch_T *cur_extmatch = تہی;
    char_u	buf_chartab[32];  // chartab array for syn iskyeyword
    char_u	*سطر;		// current line.  NOTE: becomes invalid after
				// looking for a pattern match!

    // variables for zero-width matches that have a "nextgroup" argument
    صحیح		keep_اگلا_list;
    صحیح		zero_چوڑائی_اگلا_list = FALSE;
    garray_T	zero_چوڑائی_اگلا_ga;

    /*
     * No character, no attributes!  Past end of line?
     * Do try matching with an empty line (could be the start of a region).
     */
    سطر = syn_getcurline();
    اگر (سطر[current_ستون] == NUL && current_ستون != 0)
    {
	/*
	 * If we found a match after the last column, use it.
	 */
	اگر (اگلا_مطابقت_idx >= 0 && اگلا_مطابقت_ستون >= (صحیح)current_ستون
						  && اگلا_مطابقت_ستون != MAXCOL)
	    (خالی)ڈالو_اگلا_مطابقت(تہی);

	current_ختم_شدہ = TRUE;
	current_state_stored = FALSE;
	واپس 0;
    }

    // if the current or next character is NUL, we will finish the line now
    اگر (سطر[current_ستون] == NUL || سطر[current_ستون + 1] == NUL)
    {
	current_ختم_شدہ = TRUE;
	current_state_stored = FALSE;
    }

    /*
     * When in the previous column there was a match but it could not be used
     * (empty match or already matched in this column) need to try again in
     * the next column.
     */
    اگر (try_اگلا_عمود)
    {
	اگلا_مطابقت_idx = -1;
	try_اگلا_عمود = FALSE;
    }

    // Only check for keywords when not syncing and there are some.
    do_keywords = !syncing
		    && (syn_بلاک->b_keywtab.ht_مستعمل > 0
			    || syn_بلاک->b_keywtab_ic.ht_مستعمل > 0);

    // Init the list of zero-width matches with a nextlist.  This is used to
    // avoid matching the same item in the same position twice.
    ga_init2(&zero_چوڑائی_اگلا_ga, حجم(صحیح), 10);

    // use syntax iskeyword option
    save_chartab(buf_chartab);

    /*
     * Repeat matching keywords and patterns, to find contained items at the
     * same column.  This stops when there are no extra matches at the current
     * column.
     */
    کرو
    {
	ملا_مطابقت = FALSE;
	keep_اگلا_list = FALSE;
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
	ورنہ
	    cur_si = تہی;

	اگر (syn_بلاک->b_syn_containedin || cur_si == تہی
					      || cur_si->si_cont_list != تہی)
	{
	    /*
	     * 2. Check for keywords, if on a keyword char after a non-keyword
	     *	  char.  Don't do this when syncing.
	     */
	    اگر (do_keywords)
	    {
	      سطر = syn_getcurline();
	      اگر (vim_iswordp_buf(سطر + current_ستون, syn_buf)
		      && (current_ستون == 0
			  || !vim_iswordp_buf(سطر + current_ستون - 1
			      - (has_mbyte
				  ? (*mb_سر_off)(سطر, سطر + current_ستون - 1)
				  : 0) , syn_buf)))
	      {
		syn_id = check_keyword_id(سطر, (صحیح)current_ستون,
					 &endcol, &flags, &اگلا_list, cur_si,
					 &cchar);
		اگر (syn_id != 0)
		{
		    اگر (ڈالو_current_state(KEYWORD_IDX) == OK)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			cur_si->si_m_startcol = current_ستون;
			cur_si->si_h_startpos.lnum = current_lnum;
			cur_si->si_h_startpos.ستون = 0;	// starts right away
			cur_si->si_m_endpos.lnum = current_lnum;
			cur_si->si_m_endpos.ستون = endcol;
			cur_si->si_h_endpos.lnum = current_lnum;
			cur_si->si_h_endpos.ستون = endcol;
			cur_si->si_سرے = TRUE;
			cur_si->si_انتہا_idx = 0;
			cur_si->si_flags = flags;
#اگر_تعریف_شدہ FEAT_CONCEAL
			cur_si->si_seqnr = اگلا_seqnr++;
			cur_si->si_cchar = cchar;
			اگر (current_state.ga_len > 1)
			    cur_si->si_flags |=
				  CUR_STATE(current_state.ga_len - 2).si_flags
								 & HL_CONCEAL;
#اگر_ختم
			cur_si->si_id = syn_id;
			cur_si->si_trans_id = syn_id;
			اگر (flags & HL_TRANSP)
			{
			    اگر (current_state.ga_len < 2)
			    {
				cur_si->si_attr = 0;
				cur_si->si_trans_id = 0;
			    }
			    ورنہ
			    {
				cur_si->si_attr = CUR_STATE(
					current_state.ga_len - 2).si_attr;
				cur_si->si_trans_id = CUR_STATE(
					current_state.ga_len - 2).si_trans_id;
			    }
			}
			ورنہ
			    cur_si->si_attr = syn_id2attr(syn_id);
			cur_si->si_cont_list = تہی;
			cur_si->si_اگلا_list = اگلا_list;
			check_keepend();
		    }
		    ورنہ
			vim_free(اگلا_list);
		}
	      }
	    }

	    /*
	     * 3. Check for patterns (only if no keyword found).
	     */
	    اگر (syn_id == 0 && syn_بلاک->b_syn_patterns.ga_len)
	    {
		/*
		 * If we didn't check for a match yet, or we are past it, check
		 * for any match with a pattern.
		 */
		اگر (اگلا_مطابقت_idx < 0 || اگلا_مطابقت_ستون < (صحیح)current_ستون)
		{
		    /*
		     * Check all relevant patterns for a match at this
		     * position.  This is complicated, because matching with a
		     * pattern takes quite a bit of time, thus we want to
		     * avoid doing it when it's not needed.
		     */
		    اگلا_مطابقت_idx = 0;		// no match in this line yet
		    اگلا_مطابقت_ستون = MAXCOL;
		    برائے (idx = syn_بلاک->b_syn_patterns.ga_len; --idx >= 0; )
		    {
			spp = &(SYN_اشیاء(syn_بلاک)[idx]);
			اگر (	   spp->sp_syncing == syncing
				&& (displaying || !(spp->sp_flags & HL_DISPLAY))
				&& (spp->sp_type == SPTYPE_مطابقت
				    || spp->sp_type == SPTYPE_آغاز)
				&& (current_اگلا_list != تہی
				    ? اندر_id_list(تہی, current_اگلا_list,
							      &spp->sp_syn, 0)
				    : (cur_si == تہی
					? !(spp->sp_flags & HL_CONTAINED)
					: اندر_id_list(cur_si,
					    cur_si->si_cont_list, &spp->sp_syn,
					    spp->sp_flags))))
			{
			    صحیح r;

			    // If we already tried matching in this line, and
			    // there isn't a match before next_match_col, skip
			    // this item.
			    اگر (spp->sp_سطر_id == current_سطر_id
				    && spp->sp_startcol >= اگلا_مطابقت_ستون)
				جاری;
			    spp->sp_سطر_id = current_سطر_id;

			    lc_ستون = current_ستون - spp->sp_offsets[SPO_LC_OFF];
			    اگر (lc_ستون < 0)
				lc_ستون = 0;

			    regmatch.rmm_ic = spp->sp_ic;
			    regmatch.regprog = spp->sp_prog;
			    r = syn_regexec(&regmatch,
					     current_lnum,
					     (colnr_T)lc_ستون,
					     IF_SYN_وقت(&spp->sp_وقت));
			    spp->sp_prog = regmatch.regprog;
			    اگر (!r)
			    {
				// no match in this line, try another one
				spp->sp_startcol = MAXCOL;
				جاری;
			    }

			    /*
			     * Compute the first column of the match.
			     */
			    syn_جوڑو_آغاز_off(&pos, &regmatch,
							 spp, SPO_MS_OFF, -1);
			    اگر (pos.lnum > current_lnum)
			    {
				// must have used end of match in a next line,
				// we can't handle that
				spp->sp_startcol = MAXCOL;
				جاری;
			    }
			    startcol = pos.ستون;

			    // remember the next column where this pattern
			    // matches in the current line
			    spp->sp_startcol = startcol;

			    /*
			     * If a previously found match starts at a lower
			     * column number, don't use this one.
			     */
			    اگر (startcol >= اگلا_مطابقت_ستون)
				جاری;

			    /*
			     * If we matched this pattern at this position
			     * before, skip it.  Must retry in the next
			     * column, because it may match from there.
			     */
			    اگر (did_مطابقت_already(idx, &zero_چوڑائی_اگلا_ga))
			    {
				try_اگلا_عمود = TRUE;
				جاری;
			    }

			    endpos.lnum = regmatch.endpos[0].lnum;
			    endpos.ستون = regmatch.endpos[0].ستون;

			    // Compute the highlight start.
			    syn_جوڑو_آغاز_off(&hl_startpos, &regmatch,
							 spp, SPO_HS_OFF, -1);

			    // Compute the region start.
			    // Default is to use the end of the match.
			    syn_جوڑو_انتہا_off(&eos_pos, &regmatch,
							 spp, SPO_RS_OFF, 0);

			    /*
			     * Grab the external submatches before they get
			     * overwritten.  Reference count doesn't change.
			     */
			    unref_extmatch(cur_extmatch);
			    cur_extmatch = re_extmatch_برآمد;
			    re_extmatch_برآمد = تہی;

			    flags = 0;
			    eoe_pos.lnum = 0;	// avoid warning
			    eoe_pos.ستون = 0;
			    انتہا_idx = 0;
			    hl_endpos.lnum = 0;

			    /*
			     * For a "oneline" the end must be found in the
			     * same line too.  Search for it after the end of
			     * the match with the start pattern.  Set the
			     * resulting end positions at the same time.
			     */
			    اگر (spp->sp_type == SPTYPE_آغاز
					      && (spp->sp_flags & HL_ONELINE))
			    {
				lpos_T	startpos;

				startpos = endpos;
				ڈھونڈو_endpos(idx, &startpos, &endpos, &hl_endpos,
				    &flags, &eoe_pos, &انتہا_idx, cur_extmatch);
				اگر (endpos.lnum == 0)
				    جاری;	    // not found
			    }

			    /*
			     * For a "match" the size must be > 0 after the
			     * end offset needs has been added.  Except when
			     * syncing.
			     */
			    ورنہ اگر (spp->sp_type == SPTYPE_مطابقت)
			    {
				syn_جوڑو_انتہا_off(&hl_endpos, &regmatch, spp,
							       SPO_HE_OFF, 0);
				syn_جوڑو_انتہا_off(&endpos, &regmatch, spp,
							       SPO_ME_OFF, 0);
				اگر (endpos.lnum == current_lnum
				      && (صحیح)endpos.ستون + syncing < startcol)
				{
				    /*
				     * If an empty string is matched, may need
				     * to try matching again at next column.
				     */
				    اگر (regmatch.startpos[0].ستون
						    == regmatch.endpos[0].ستون)
					try_اگلا_عمود = TRUE;
				    جاری;
				}
			    }

			    /*
			     * keep the best match so far in next_match_*
			     */
			    // Highlighting must start after startpos and end
			    // before endpos.
			    اگر (hl_startpos.lnum == current_lnum
					   && (صحیح)hl_startpos.ستون < startcol)
				hl_startpos.ستون = startcol;
			    حد_بندی_pos_zero(&hl_endpos, &endpos);

			    اگلا_مطابقت_idx = idx;
			    اگلا_مطابقت_ستون = startcol;
			    اگلا_مطابقت_m_endpos = endpos;
			    اگلا_مطابقت_h_endpos = hl_endpos;
			    اگلا_مطابقت_h_startpos = hl_startpos;
			    اگلا_مطابقت_flags = flags;
			    اگلا_مطابقت_eos_pos = eos_pos;
			    اگلا_مطابقت_eoe_pos = eoe_pos;
			    اگلا_مطابقت_انتہا_idx = انتہا_idx;
			    unref_extmatch(اگلا_مطابقت_extmatch);
			    اگلا_مطابقت_extmatch = cur_extmatch;
			    cur_extmatch = تہی;
			}
		    }
		}

		/*
		 * If we found a match at the current column, use it.
		 */
		اگر (اگلا_مطابقت_idx >= 0 && اگلا_مطابقت_ستون == (صحیح)current_ستون)
		{
		    synpat_T	*lspp;

		    // When a zero-width item matched which has a nextgroup,
		    // don't push the item but set nextgroup.
		    lspp = &(SYN_اشیاء(syn_بلاک)[اگلا_مطابقت_idx]);
		    اگر (اگلا_مطابقت_m_endpos.lnum == current_lnum
			    && اگلا_مطابقت_m_endpos.ستون == current_ستون
			    && lspp->sp_اگلا_list != تہی)
		    {
			current_اگلا_list = lspp->sp_اگلا_list;
			current_اگلا_flags = lspp->sp_flags;
			keep_اگلا_list = TRUE;
			zero_چوڑائی_اگلا_list = TRUE;

			// Add the index to a list, so that we can check
			// later that we don't match it again (and cause an
			// endless loop).
			اگر (ga_grow(&zero_چوڑائی_اگلا_ga, 1) == OK)
			{
			    ((صحیح *)(zero_چوڑائی_اگلا_ga.ga_ڈیٹا))
				[zero_چوڑائی_اگلا_ga.ga_len++] = اگلا_مطابقت_idx;
			}
			اگلا_مطابقت_idx = -1;
		    }
		    ورنہ
			cur_si = ڈالو_اگلا_مطابقت(cur_si);
		    ملا_مطابقت = TRUE;
		}
	    }
	}

	/*
	 * Handle searching for nextgroup match.
	 */
	اگر (current_اگلا_list != تہی && !keep_اگلا_list)
	{
	    /*
	     * If a nextgroup was not found, continue looking for one if:
	     * - this is an empty line and the "skipempty" option was given
	     * - we are on white space and the "skipwhite" option was given
	     */
	    اگر (!ملا_مطابقت)
	    {
		سطر = syn_getcurline();
		اگر (((current_اگلا_flags & HL_SKIPWHITE)
			    && VIM_ISWHITE(سطر[current_ستون]))
			|| ((current_اگلا_flags & HL_SKIPEMPTY)
			    && *سطر == NUL))
		    توڑو;
	    }

	    /*
	     * If a nextgroup was found: Use it, and continue looking for
	     * contained matches.
	     * If a nextgroup was not found: Continue looking for a normal
	     * match.
	     * When did set current_next_list for a zero-width item and no
	     * match was found don't loop (would get stuck).
	     */
	    current_اگلا_list = تہی;
	    اگلا_مطابقت_idx = -1;
	    اگر (!zero_چوڑائی_اگلا_list)
		ملا_مطابقت = TRUE;
	}

    } جب (ملا_مطابقت);

    restore_chartab(buf_chartab);

    /*
     * Use attributes from the current state, if within its highlighting.
     * If not, use attributes from the current-but-one state, etc.
     */
    current_attr = 0;
#اگر_تعریف_شدہ FEAT_EVAL
    current_id = 0;
    current_trans_id = 0;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CONCEAL
    current_flags = 0;
    current_seqnr = 0;
#اگر_ختم
    اگر (cur_si != تہی)
    {
#اگر_غیر_تعریف_شدہ FEAT_EVAL
	صحیح	current_trans_id = 0;
#اگر_ختم
	برائے (idx = current_state.ga_len - 1; idx >= 0; --idx)
	{
	    sip = &CUR_STATE(idx);
	    اگر ((current_lnum > sip->si_h_startpos.lnum
			|| (current_lnum == sip->si_h_startpos.lnum
			    && current_ستون >= sip->si_h_startpos.ستون))
		    && (sip->si_h_endpos.lnum == 0
			|| current_lnum < sip->si_h_endpos.lnum
			|| (current_lnum == sip->si_h_endpos.lnum
			    && current_ستون < sip->si_h_endpos.ستون)))
	    {
		current_attr = sip->si_attr;
#اگر_تعریف_شدہ FEAT_EVAL
		current_id = sip->si_id;
#اگر_ختم
		current_trans_id = sip->si_trans_id;
#اگر_تعریف_شدہ FEAT_CONCEAL
		current_flags = sip->si_flags;
		current_seqnr = sip->si_seqnr;
		current_sub_char = sip->si_cchar;
#اگر_ختم
		توڑو;
	    }
	}

	اگر (can_spell != تہی)
	{
	    ساخت sp_syn   sps;

	    /*
	     * set "can_spell" to TRUE if spell checking is supposed to be
	     * done in the current item.
	     */
	    اگر (syn_بلاک->b_spell_cluster_id == 0)
	    {
		// There is no @Spell cluster: Do spelling for items without
		// @NoSpell cluster.
		اگر (syn_بلاک->b_nospell_cluster_id == 0
						     || current_trans_id == 0)
		    *can_spell = (syn_بلاک->b_syn_spell != SYNSPL_NOTOP);
		ورنہ
		{
		    sps.inc_لیبل = 0;
		    sps.id = syn_بلاک->b_nospell_cluster_id;
		    sps.cont_اندر_list = تہی;
		    *can_spell = !اندر_id_list(sip, sip->si_cont_list, &sps, 0);
		}
	    }
	    ورنہ
	    {
		// The @Spell cluster is defined: Do spelling in items with
		// the @Spell cluster.  But not when @NoSpell is also there.
		// At the toplevel only spell check when ":syn spell toplevel"
		// was used.
		اگر (current_trans_id == 0)
		    *can_spell = (syn_بلاک->b_syn_spell == SYNSPL_چوٹی);
		ورنہ
		{
		    sps.inc_لیبل = 0;
		    sps.id = syn_بلاک->b_spell_cluster_id;
		    sps.cont_اندر_list = تہی;
		    *can_spell = اندر_id_list(sip, sip->si_cont_list, &sps, 0);

		    اگر (syn_بلاک->b_nospell_cluster_id != 0)
		    {
			sps.id = syn_بلاک->b_nospell_cluster_id;
			اگر (اندر_id_list(sip, sip->si_cont_list, &sps, 0))
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
	    check_state_سرے();
	    اگر (current_state.ga_len > 0
				      && syn_getcurline()[current_ستون] != NUL)
	    {
		++current_ستون;
		check_state_سرے();
		--current_ستون;
	    }
	}
    }
    ورنہ اگر (can_spell != تہی)
	// Default: Only do spelling when there is no @Spell cluster or when
	// ":syn spell toplevel" was used.
	*can_spell = syn_بلاک->b_syn_spell == SYNSPL_DEFAULT
		    ? (syn_بلاک->b_spell_cluster_id == 0)
		    : (syn_بلاک->b_syn_spell == SYNSPL_چوٹی);

    // nextgroup ends at end of line, unless "skipnl" or "skipempty" present
    اگر (current_اگلا_list != تہی
	    && (سطر = syn_getcurline())[current_ستون] != NUL
	    && سطر[current_ستون + 1] == NUL
	    && !(current_اگلا_flags & (HL_SKIPNL | HL_SKIPEMPTY)))
	current_اگلا_list = تہی;

    اگر (zero_چوڑائی_اگلا_ga.ga_len > 0)
	ga_clear(&zero_چوڑائی_اگلا_ga);

    // No longer need external matches.  But keep next_match_extmatch.
    unref_extmatch(re_extmatch_برآمد);
    re_extmatch_برآمد = تہی;
    unref_extmatch(cur_extmatch);

    واپس current_attr;
}


/*
 * Check if we already matched pattern "idx" at the current column.
 */
    ساکن صحیح
did_مطابقت_already(صحیح idx, garray_T *gap)
{
    صحیح		i;

    برائے (i = current_state.ga_len; --i >= 0; )
	اگر (CUR_STATE(i).si_m_startcol == (صحیح)current_ستون
		&& CUR_STATE(i).si_m_lnum == (صحیح)current_lnum
		&& CUR_STATE(i).si_idx == idx)
	    واپس TRUE;

    // Zero-width matches with a nextgroup argument are not put on the syntax
    // stack, and can only be matched once anyway.
    برائے (i = gap->ga_len; --i >= 0; )
	اگر (((صحیح *)(gap->ga_ڈیٹا))[i] == idx)
	    واپس TRUE;

    واپس FALSE;
}

/*
 * Push the next match onto the stack.
 */
    ساکن stateitem_T *
ڈالو_اگلا_مطابقت(stateitem_T *cur_si)
{
    synpat_T	*spp;
#اگر_تعریف_شدہ FEAT_CONCEAL
    صحیح		 save_flags;
#اگر_ختم

    spp = &(SYN_اشیاء(syn_بلاک)[اگلا_مطابقت_idx]);

    /*
     * Push the item in current_state stack;
     */
    اگر (ڈالو_current_state(اگلا_مطابقت_idx) == OK)
    {
	/*
	 * If it's a start-skip-end type that crosses lines, figure out how
	 * much it continues in this line.  Otherwise just fill in the length.
	 */
	cur_si = &CUR_STATE(current_state.ga_len - 1);
	cur_si->si_h_startpos = اگلا_مطابقت_h_startpos;
	cur_si->si_m_startcol = current_ستون;
	cur_si->si_m_lnum = current_lnum;
	cur_si->si_flags = spp->sp_flags;
#اگر_تعریف_شدہ FEAT_CONCEAL
	cur_si->si_seqnr = اگلا_seqnr++;
	cur_si->si_cchar = spp->sp_cchar;
	اگر (current_state.ga_len > 1)
	    cur_si->si_flags |=
		    CUR_STATE(current_state.ga_len - 2).si_flags & HL_CONCEAL;
#اگر_ختم
	cur_si->si_اگلا_list = spp->sp_اگلا_list;
	cur_si->si_extmatch = ref_extmatch(اگلا_مطابقت_extmatch);
	اگر (spp->sp_type == SPTYPE_آغاز && !(spp->sp_flags & HL_ONELINE))
	{
	    // Try to find the end pattern in the current line
	    update_si_انتہا(cur_si, (صحیح)(اگلا_مطابقت_m_endpos.ستون), TRUE);
	    check_keepend();
	}
	ورنہ
	{
	    cur_si->si_m_endpos = اگلا_مطابقت_m_endpos;
	    cur_si->si_h_endpos = اگلا_مطابقت_h_endpos;
	    cur_si->si_سرے = TRUE;
	    cur_si->si_flags |= اگلا_مطابقت_flags;
	    cur_si->si_eoe_pos = اگلا_مطابقت_eoe_pos;
	    cur_si->si_انتہا_idx = اگلا_مطابقت_انتہا_idx;
	}
	اگر (keepend_level < 0 && (cur_si->si_flags & HL_KEEPEND))
	    keepend_level = current_state.ga_len - 1;
	check_keepend();
	update_si_attr(current_state.ga_len - 1);

#اگر_تعریف_شدہ FEAT_CONCEAL
	save_flags = cur_si->si_flags & (HL_CONCEAL | HL_CONCEALENDS);
#اگر_ختم
	/*
	 * If the start pattern has another highlight group, push another item
	 * on the stack for the start pattern.
	 */
	اگر (	   spp->sp_type == SPTYPE_آغاز
		&& spp->sp_syn_مطابقت_id != 0
		&& ڈالو_current_state(اگلا_مطابقت_idx) == OK)
	{
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	    cur_si->si_h_startpos = اگلا_مطابقت_h_startpos;
	    cur_si->si_m_startcol = current_ستون;
	    cur_si->si_m_lnum = current_lnum;
	    cur_si->si_m_endpos = اگلا_مطابقت_eos_pos;
	    cur_si->si_h_endpos = اگلا_مطابقت_eos_pos;
	    cur_si->si_سرے = TRUE;
	    cur_si->si_انتہا_idx = 0;
	    cur_si->si_flags = HL_مطابقت;
#اگر_تعریف_شدہ FEAT_CONCEAL
	    cur_si->si_seqnr = اگلا_seqnr++;
	    cur_si->si_flags |= save_flags;
	    اگر (cur_si->si_flags & HL_CONCEALENDS)
		cur_si->si_flags |= HL_CONCEAL;
#اگر_ختم
	    cur_si->si_اگلا_list = تہی;
	    check_keepend();
	    update_si_attr(current_state.ga_len - 1);
	}
    }

    اگلا_مطابقت_idx = -1;	// try other match next time

    واپس cur_si;
}

/*
 * Check for end of current state (and the states before it).
 */
    ساکن خالی
check_state_سرے(خالی)
{
    stateitem_T	*cur_si;
    صحیح		had_extend;

    cur_si = &CUR_STATE(current_state.ga_len - 1);
    برائے (;;)
    {
	اگر (cur_si->si_سرے
		&& (cur_si->si_m_endpos.lnum < current_lnum
		    || (cur_si->si_m_endpos.lnum == current_lnum
			&& cur_si->si_m_endpos.ستون <= current_ستون)))
	{
	    /*
	     * If there is an end pattern group ID, highlight the end pattern
	     * now.  No need to pop the current item from the stack.
	     * Only do this if the end pattern continues beyond the current
	     * position.
	     */
	    اگر (cur_si->si_انتہا_idx
		    && (cur_si->si_eoe_pos.lnum > current_lnum
			|| (cur_si->si_eoe_pos.lnum == current_lnum
			    && cur_si->si_eoe_pos.ستون > current_ستون)))
	    {
		cur_si->si_idx = cur_si->si_انتہا_idx;
		cur_si->si_انتہا_idx = 0;
		cur_si->si_m_endpos = cur_si->si_eoe_pos;
		cur_si->si_h_endpos = cur_si->si_eoe_pos;
		cur_si->si_flags |= HL_مطابقت;
#اگر_تعریف_شدہ FEAT_CONCEAL
		cur_si->si_seqnr = اگلا_seqnr++;
		اگر (cur_si->si_flags & HL_CONCEALENDS)
		    cur_si->si_flags |= HL_CONCEAL;
#اگر_ختم
		update_si_attr(current_state.ga_len - 1);

		// nextgroup= should not match in the end pattern
		current_اگلا_list = تہی;

		// what matches next may be different now, clear it
		اگلا_مطابقت_idx = 0;
		اگلا_مطابقت_ستون = MAXCOL;
		توڑو;
	    }

	    // handle next_list, unless at end of line and no "skipnl" or
	    // "skipempty"
	    current_اگلا_list = cur_si->si_اگلا_list;
	    current_اگلا_flags = cur_si->si_flags;
	    اگر (!(current_اگلا_flags & (HL_SKIPNL | HL_SKIPEMPTY))
		    && syn_getcurline()[current_ستون] == NUL)
		current_اگلا_list = تہی;

	    // When the ended item has "extend", another item with
	    // "keepend" now needs to check for its end.
	    had_extend = (cur_si->si_flags & HL_EXTEND);

	    نکالو_current_state();

	    اگر (current_state.ga_len == 0)
		توڑو;

	    اگر (had_extend && keepend_level >= 0)
	    {
		syn_update_سرے(FALSE);
		اگر (current_state.ga_len == 0)
		    توڑو;
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
		    && SYN_اشیاء(syn_بلاک)[cur_si->si_idx].sp_type
								== SPTYPE_آغاز
		    && !(cur_si->si_flags & (HL_مطابقت | HL_KEEPEND)))
	    {
		update_si_انتہا(cur_si, (صحیح)current_ستون, TRUE);
		check_keepend();
		اگر ((current_اگلا_flags & HL_HAS_EOL)
			&& keepend_level < 0
			&& syn_getcurline()[current_ستون] == NUL)
		    توڑو;
	    }
	}
	ورنہ
	    توڑو;
    }
}

/*
 * Update an entry in the current_state stack for a match or region.  This
 * fills in si_attr, si_next_list and si_cont_list.
 */
    ساکن خالی
update_si_attr(صحیح idx)
{
    stateitem_T	*sip = &CUR_STATE(idx);
    synpat_T	*spp;

    // This should not happen...
    اگر (sip->si_idx < 0)
	واپس;

    spp = &(SYN_اشیاء(syn_بلاک)[sip->si_idx]);
    اگر (sip->si_flags & HL_مطابقت)
	sip->si_id = spp->sp_syn_مطابقت_id;
    ورنہ
	sip->si_id = spp->sp_syn.id;
    sip->si_attr = syn_id2attr(sip->si_id);
    sip->si_trans_id = sip->si_id;
    اگر (sip->si_flags & HL_مطابقت)
	sip->si_cont_list = تہی;
    ورنہ
	sip->si_cont_list = spp->sp_cont_list;

    /*
     * For transparent items, take attr from outer item.
     * Also take cont_list, if there is none.
     * Don't do this for the matchgroup of a start or end pattern.
     */
    اگر ((spp->sp_flags & HL_TRANSP) && !(sip->si_flags & HL_مطابقت))
    {
	اگر (idx == 0)
	{
	    sip->si_attr = 0;
	    sip->si_trans_id = 0;
	    اگر (sip->si_cont_list == تہی)
		sip->si_cont_list = ID_LIST_ALL;
	}
	ورنہ
	{
	    sip->si_attr = CUR_STATE(idx - 1).si_attr;
	    sip->si_trans_id = CUR_STATE(idx - 1).si_trans_id;
	    اگر (sip->si_cont_list == تہی)
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
    ساکن خالی
check_keepend(خالی)
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
	واپس;

    /*
     * Find the last index of an "extend" item.  "keepend" items before that
     * won't do anything.  If there is no "extend" item "i" will be
     * "keepend_level" and all "keepend" items will work normally.
     */
    برائے (i = current_state.ga_len - 1; i > keepend_level; --i)
	اگر (CUR_STATE(i).si_flags & HL_EXTEND)
	    توڑو;

    maxpos.lnum = 0;
    maxpos.ستون = 0;
    maxpos_h.lnum = 0;
    maxpos_h.ستون = 0;
    برائے ( ; i < current_state.ga_len; ++i)
    {
	sip = &CUR_STATE(i);
	اگر (maxpos.lnum != 0)
	{
	    حد_بندی_pos_zero(&sip->si_m_endpos, &maxpos);
	    حد_بندی_pos_zero(&sip->si_h_endpos, &maxpos_h);
	    حد_بندی_pos_zero(&sip->si_eoe_pos, &maxpos);
	    sip->si_سرے = TRUE;
	}
	اگر (sip->si_سرے && (sip->si_flags & HL_KEEPEND))
	{
	    اگر (maxpos.lnum == 0
		    || maxpos.lnum > sip->si_m_endpos.lnum
		    || (maxpos.lnum == sip->si_m_endpos.lnum
			&& maxpos.ستون > sip->si_m_endpos.ستون))
		maxpos = sip->si_m_endpos;
	    اگر (maxpos_h.lnum == 0
		    || maxpos_h.lnum > sip->si_h_endpos.lnum
		    || (maxpos_h.lnum == sip->si_h_endpos.lnum
			&& maxpos_h.ستون > sip->si_h_endpos.ستون))
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
    ساکن خالی
update_si_انتہا(
    stateitem_T	*sip,
    صحیح		startcol,   // where to start searching for the end
    صحیح		force)	    // when TRUE overrule a previous end
{
    lpos_T	startpos;
    lpos_T	endpos;
    lpos_T	hl_endpos;
    lpos_T	انتہا_endpos;
    صحیح		انتہا_idx;

    // return quickly for a keyword
    اگر (sip->si_idx < 0)
	واپس;

    // Don't update when it's already done.  Can be a match of an end pattern
    // that started in a previous line.  Watch out: can also be a "keepend"
    // from a containing item.
    اگر (!force && sip->si_m_endpos.lnum >= current_lnum)
	واپس;

    /*
     * We need to find the end of the region.  It may continue in the next
     * line.
     */
    انتہا_idx = 0;
    startpos.lnum = current_lnum;
    startpos.ستون = startcol;
    ڈھونڈو_endpos(sip->si_idx, &startpos, &endpos, &hl_endpos,
		   &(sip->si_flags), &انتہا_endpos, &انتہا_idx, sip->si_extmatch);

    اگر (endpos.lnum == 0)
    {
	// No end pattern matched.
	اگر (SYN_اشیاء(syn_بلاک)[sip->si_idx].sp_flags & HL_ONELINE)
	{
	    // a "oneline" never continues in the next line
	    sip->si_سرے = TRUE;
	    sip->si_m_endpos.lnum = current_lnum;
	    sip->si_m_endpos.ستون = syn_getcurline_len();
	}
	ورنہ
	{
	    // continues in the next line
	    sip->si_سرے = FALSE;
	    sip->si_m_endpos.lnum = 0;
	}
	sip->si_h_endpos = sip->si_m_endpos;
    }
    ورنہ
    {
	// match within this line
	sip->si_m_endpos = endpos;
	sip->si_h_endpos = hl_endpos;
	sip->si_eoe_pos = انتہا_endpos;
	sip->si_سرے = TRUE;
	sip->si_انتہا_idx = انتہا_idx;
    }
}

/*
 * Add a new state to the current state stack.
 * It is cleared and the index set to "idx".
 * Return FAIL if it's not possible (out of memory).
 */
    ساکن صحیح
ڈالو_current_state(صحیح idx)
{
    اگر (ga_grow(&current_state, 1) == FAIL)
	واپس FAIL;
    CLEAR_اشارہ_گر(&CUR_STATE(current_state.ga_len));
    CUR_STATE(current_state.ga_len).si_idx = idx;
    ++current_state.ga_len;
    واپس OK;
}

/*
 * Remove a state from the current_state stack.
 */
    ساکن خالی
نکالو_current_state(خالی)
{
    اگر (current_state.ga_len)
    {
	unref_extmatch(CUR_STATE(current_state.ga_len - 1).si_extmatch);
	--current_state.ga_len;
    }
    // after the end of a pattern, try matching a keyword or pattern
    اگلا_مطابقت_idx = -1;

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
    ساکن خالی
ڈھونڈو_endpos(
    صحیح		idx,		// index of the pattern
    lpos_T	*startpos,	// where to start looking for an END match
    lpos_T	*m_endpos,	// return: end of match
    lpos_T	*hl_endpos,	// return: end of highlighting
    طویل	*flagsp,	// return: flags of matching END
    lpos_T	*انتہا_endpos,	// return: end of end pattern match
    صحیح		*انتہا_idx,	// return: group ID for end pat. match, or 0
    reg_extmatch_T *آغاز_ext)	// submatches from the start pattern
{
    colnr_T	matchcol;
    synpat_T	*spp, *spp_skip;
    صحیح		آغاز_idx;
    صحیح		بہترین_idx;
    regmmatch_T	regmatch;
    regmmatch_T	بہترین_regmatch;	    // startpos/endpos of best match
    lpos_T	pos;
    صحیح		had_مطابقت = FALSE;
    char_u	buf_chartab[32];  // chartab array for syn option iskyeyword

    // just in case we are invoked for a keyword
    اگر (idx < 0)
	واپس;

    /*
     * Check for being called with a START pattern.
     * Can happen with a match that continues to the next line, because it
     * contained a region.
     */
    spp = &(SYN_اشیاء(syn_بلاک)[idx]);
    اگر (spp->sp_type != SPTYPE_آغاز)
    {
	*hl_endpos = *startpos;
	واپس;
    }

    /*
     * Find the SKIP or first END pattern after the last START pattern.
     */
    برائے (;;)
    {
	spp = &(SYN_اشیاء(syn_بلاک)[idx]);
	اگر (spp->sp_type != SPTYPE_آغاز)
	    توڑو;
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
    ورنہ
	spp_skip = تہی;

    // Setup external matches for syn_regexec().
    unref_extmatch(re_extmatch_اندر);
    re_extmatch_اندر = ref_extmatch(آغاز_ext);

    matchcol = startpos->ستون;	// start looking for a match at sstart
    آغاز_idx = idx;		// remember the first END pattern.
    بہترین_regmatch.startpos[0].ستون = 0;		// avoid compiler warning

    // use syntax iskeyword option
    save_chartab(buf_chartab);

    برائے (;;)
    {
	/*
	 * Find end pattern that matches first after "matchcol".
	 */
	بہترین_idx = -1;
	برائے (idx = آغاز_idx; idx < syn_بلاک->b_syn_patterns.ga_len; ++idx)
	{
	    صحیح lc_ستون = matchcol;
	    صحیح r;

	    spp = &(SYN_اشیاء(syn_بلاک)[idx]);
	    اگر (spp->sp_type != SPTYPE_انتہا)	// past last END pattern
		توڑو;
	    lc_ستون -= spp->sp_offsets[SPO_LC_OFF];
	    اگر (lc_ستون < 0)
		lc_ستون = 0;

	    regmatch.rmm_ic = spp->sp_ic;
	    regmatch.regprog = spp->sp_prog;
	    r = syn_regexec(&regmatch, startpos->lnum, lc_ستون,
						  IF_SYN_وقت(&spp->sp_وقت));
	    spp->sp_prog = regmatch.regprog;
	    اگر (r)
	    {
		اگر (بہترین_idx == -1 || regmatch.startpos[0].ستون
					      < بہترین_regmatch.startpos[0].ستون)
		{
		    بہترین_idx = idx;
		    بہترین_regmatch.startpos[0] = regmatch.startpos[0];
		    بہترین_regmatch.endpos[0] = regmatch.endpos[0];
		}
	    }
	}

	/*
	 * If all end patterns have been tried, and there is no match, the
	 * item continues until end-of-line.
	 */
	اگر (بہترین_idx == -1)
	    توڑو;

	/*
	 * If the skip pattern matches before the end pattern,
	 * continue searching after the skip pattern.
	 */
	اگر (spp_skip != تہی)
	{
	    صحیح lc_ستون = matchcol - spp_skip->sp_offsets[SPO_LC_OFF];
	    صحیح r;

	    اگر (lc_ستون < 0)
		lc_ستون = 0;
	    regmatch.rmm_ic = spp_skip->sp_ic;
	    regmatch.regprog = spp_skip->sp_prog;
	    r = syn_regexec(&regmatch, startpos->lnum, lc_ستون,
					      IF_SYN_وقت(&spp_skip->sp_وقت));
	    spp_skip->sp_prog = regmatch.regprog;
	    اگر (r && regmatch.startpos[0].ستون
					     <= بہترین_regmatch.startpos[0].ستون)
	    {
		صحیح سطر_len;

		// Add offset to skip pattern match
		syn_جوڑو_انتہا_off(&pos, &regmatch, spp_skip, SPO_ME_OFF, 1);

		// If the skip pattern goes on to the next line, there is no
		// match with an end pattern in this line.
		اگر (pos.lnum > startpos->lnum)
		    توڑو;

		سطر_len = ml_get_buf_len(syn_buf, startpos->lnum);

		// take care of an empty match or negative offset
		اگر (pos.ستون <= matchcol)
		    ++matchcol;
		ورنہ اگر (pos.ستون <= regmatch.endpos[0].ستون)
		    matchcol = pos.ستون;
		ورنہ
		    // Be careful not to jump over the NUL at the end-of-line
		    برائے (matchcol = regmatch.endpos[0].ستون;
			    matchcol < سطر_len && matchcol < pos.ستون;
								   ++matchcol)
			;

		// if the skip pattern includes end-of-line, break here
		اگر (matchcol >= سطر_len)
		    توڑو;

		جاری;	    // start with first end pattern again
	    }
	}

	/*
	 * Match from start pattern to end pattern.
	 * Correct for match and highlight offset of end pattern.
	 */
	spp = &(SYN_اشیاء(syn_بلاک)[بہترین_idx]);
	syn_جوڑو_انتہا_off(m_endpos, &بہترین_regmatch, spp, SPO_ME_OFF, 1);
	// can't end before the start
	اگر (m_endpos->lnum == startpos->lnum && m_endpos->ستون < startpos->ستون)
	    m_endpos->ستون = startpos->ستون;

	syn_جوڑو_انتہا_off(انتہا_endpos, &بہترین_regmatch, spp, SPO_HE_OFF, 1);
	// can't end before the start
	اگر (انتہا_endpos->lnum == startpos->lnum
					   && انتہا_endpos->ستون < startpos->ستون)
	    انتہا_endpos->ستون = startpos->ستون;
	// can't end after the match
	حد_بندی_pos(انتہا_endpos, m_endpos);

	/*
	 * If the end group is highlighted differently, adjust the pointers.
	 */
	اگر (spp->sp_syn_مطابقت_id != spp->sp_syn.id && spp->sp_syn_مطابقت_id != 0)
	{
	    *انتہا_idx = بہترین_idx;
	    اگر (spp->sp_off_flags & (1 << (SPO_RE_OFF + SPO_گنتی)))
	    {
		hl_endpos->lnum = بہترین_regmatch.endpos[0].lnum;
		hl_endpos->ستون = بہترین_regmatch.endpos[0].ستون;
	    }
	    ورنہ
	    {
		hl_endpos->lnum = بہترین_regmatch.startpos[0].lnum;
		hl_endpos->ستون = بہترین_regmatch.startpos[0].ستون;
	    }
	    hl_endpos->ستون += spp->sp_offsets[SPO_RE_OFF];

	    // can't end before the start
	    اگر (hl_endpos->lnum == startpos->lnum
					    && hl_endpos->ستون < startpos->ستون)
		hl_endpos->ستون = startpos->ستون;
	    حد_بندی_pos(hl_endpos, m_endpos);

	    // now the match ends where the highlighting ends, it is turned
	    // into the matchgroup for the end
	    *m_endpos = *hl_endpos;
	}
	ورنہ
	{
	    *انتہا_idx = 0;
	    *hl_endpos = *انتہا_endpos;
	}

	*flagsp = spp->sp_flags;

	had_مطابقت = TRUE;
	توڑو;
    }

    // no match for an END pattern in this line
    اگر (!had_مطابقت)
	m_endpos->lnum = 0;

    restore_chartab(buf_chartab);

    // Remove external matches.
    unref_extmatch(re_extmatch_اندر);
    re_extmatch_اندر = تہی;
}

/*
 * Limit "pos" not to be after "limit".
 */
    ساکن خالی
حد_بندی_pos(lpos_T *pos, lpos_T *حد_بندی)
{
    اگر (pos->lnum > حد_بندی->lnum)
	*pos = *حد_بندی;
    ورنہ اگر (pos->lnum == حد_بندی->lnum && pos->ستون > حد_بندی->ستون)
	pos->ستون = حد_بندی->ستون;
}

/*
 * Limit "pos" not to be after "limit", unless pos->lnum is zero.
 */
    ساکن خالی
حد_بندی_pos_zero(
    lpos_T	*pos,
    lpos_T	*حد_بندی)
{
    اگر (pos->lnum == 0)
	*pos = *حد_بندی;
    ورنہ
	حد_بندی_pos(pos, حد_بندی);
}

/*
 * Add offset to matched text for end of match or highlight.
 */
    ساکن خالی
syn_جوڑو_انتہا_off(
    lpos_T	*نتیجہ,	// returned position
    regmmatch_T	*regmatch,	// start/end of match
    synpat_T	*spp,		// matched pattern
    صحیح		idx,		// index of offset
    صحیح		extra)		// extra chars for offset to start
{
    صحیح		ستون;
    صحیح		off;
    char_u	*بنیاد;
    char_u	*p;

    اگر (spp->sp_off_flags & (1 << idx))
    {
	نتیجہ->lnum = regmatch->startpos[0].lnum;
	ستون = regmatch->startpos[0].ستون;
	off = spp->sp_offsets[idx] + extra;
    }
    ورنہ
    {
	نتیجہ->lnum = regmatch->endpos[0].lnum;
	ستون = regmatch->endpos[0].ستون;
	off = spp->sp_offsets[idx];
    }
    // Don't go past the end of the line.  Matters for "rs=e+2" when there
    // is a matchgroup. Watch out for match with last NL in the buffer.
    اگر (نتیجہ->lnum > syn_buf->b_ml.ml_سطر_گنتی)
	ستون = 0;
    ورنہ اگر (off != 0)
    {
	بنیاد = ml_get_buf(syn_buf, نتیجہ->lnum, FALSE);
	p = بنیاد + ستون;
	اگر (off > 0)
	{
	    جب (off-- > 0 && *p != NUL)
		MB_PTR_ADV(p);
	}
	ورنہ اگر (off < 0)
	{
	    جب (off++ < 0 && بنیاد < p)
		MB_PTR_عقب(بنیاد, p);
	}
	ستون = (صحیح)(p - بنیاد);
    }
    نتیجہ->ستون = ستون;
}

/*
 * Add offset to matched text for start of match or highlight.
 * Avoid resulting column to become negative.
 */
    ساکن خالی
syn_جوڑو_آغاز_off(
    lpos_T	*نتیجہ,	// returned position
    regmmatch_T	*regmatch,	// start/end of match
    synpat_T	*spp,
    صحیح		idx,
    صحیح		extra)	    // extra chars for offset to end
{
    صحیح		ستون;
    صحیح		off;
    char_u	*بنیاد;
    char_u	*p;

    اگر (spp->sp_off_flags & (1 << (idx + SPO_گنتی)))
    {
	نتیجہ->lnum = regmatch->endpos[0].lnum;
	ستون = regmatch->endpos[0].ستون;
	off = spp->sp_offsets[idx] + extra;
    }
    ورنہ
    {
	نتیجہ->lnum = regmatch->startpos[0].lnum;
	ستون = regmatch->startpos[0].ستون;
	off = spp->sp_offsets[idx];
    }
    اگر (نتیجہ->lnum > syn_buf->b_ml.ml_سطر_گنتی)
    {
	// a "\n" at the end of the pattern may take us below the last line
	نتیجہ->lnum = syn_buf->b_ml.ml_سطر_گنتی;
	ستون = ml_get_buf_len(syn_buf, نتیجہ->lnum);
    }
    اگر (off != 0)
    {
	بنیاد = ml_get_buf(syn_buf, نتیجہ->lnum, FALSE);
	p = بنیاد + ستون;
	اگر (off > 0)
	{
	    جب (off-- && *p != NUL)
		MB_PTR_ADV(p);
	}
	ورنہ اگر (off < 0)
	{
	    جب (off++ && بنیاد < p)
		MB_PTR_عقب(بنیاد, p);
	}
	ستون = (صحیح)(p - بنیاد);
    }
    نتیجہ->ستون = ستون;
}

/*
 * Get current line in syntax buffer.
 */
    ساکن char_u *
syn_getcurline(خالی)
{
    واپس ml_get_buf(syn_buf, current_lnum, FALSE);
}

/*
 * Get length of current line in syntax buffer.
 */
    ساکن colnr_T
syn_getcurline_len(خالی)
{
    واپس ml_get_buf_len(syn_buf, current_lnum);
}

/*
 * Call vim_regexec() to find a match with "rmp" in "syn_buf".
 * Returns TRUE when there is a match.
 */
    ساکن صحیح
syn_regexec(
    regmmatch_T	*rmp,
    linenr_T	lnum,
    colnr_T	ستون,
    syn_وقت_T  *st غیر_مستعمل)
{
    صحیح		r;
    صحیح		timed_برآمد = FALSE;
#اگر_تعریف_شدہ FEAT_PROFILE
    proftime_T	pt;

    اگر (syn_وقت_on)
	profile_آغاز(&pt);
#اگر_ختم

    اگر (rmp->regprog == تہی)
	// This can happen if a previous call to vim_regexec_multi() tried to
	// use the NFA engine, which resulted in NFA_TOO_EXPENSIVE, and
	// compiling the pattern with the other engine fails.
	واپس FALSE;

    rmp->rmm_maxcol = syn_buf->b_p_smc;
    r = vim_regexec_multi(rmp, syn_win, syn_buf, lnum, ستون, &timed_برآمد);

#اگر_تعریف_شدہ FEAT_PROFILE
    اگر (syn_وقت_on)
    {
	profile_انتہا(&pt);
	profile_جوڑو(&st->کل, &pt);
	اگر (profile_cmp(&pt, &st->slowest) < 0)
	    st->slowest = pt;
	++st->گنتی;
	اگر (r > 0)
	    ++st->مطابقت;
    }
#اگر_ختم
#اگر_تعریف_شدہ FEAT_RELTIME
    اگر (timed_برآمد && redrawtime_حد_بندی_سیٹ && !syn_win->w_s->b_syn_slow)
    {
	syn_win->w_s->b_syn_slow = TRUE;
	پیغام(_("'redrawtime' exceeded, syntax highlighting disabled"));
    }
#اگر_ختم

    اگر (r > 0)
    {
	rmp->startpos[0].lnum += lnum;
	rmp->endpos[0].lnum += lnum;
	واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Check one position in a line for a matching keyword.
 * The caller must check if a keyword can start at startcol.
 * Return its ID if found, 0 otherwise.
 */
    ساکن صحیح
check_keyword_id(
    char_u	*سطر,
    صحیح		startcol,	// position in line to check for keyword
    صحیح		*endcolp,	// return: character after found keyword
    طویل	*flagsp,	// return: flags of matching keyword
    مختصر	**اگلا_listp,	// return: next_list of matching keyword
    stateitem_T	*cur_si,	// item at the top of the stack
    صحیح		*ccharp غیر_مستعمل)	// conceal substitution char
{
    keyentry_T	*kp;
    char_u	*kwp;
    صحیح		باری;
    صحیح		kwlen;
    char_u	keyword[MAXKEYWLEN + 1]; // assume max. keyword len is 80
    hashtab_T	*ht;
    hashitem_T	*hi;

    // Find first character after the keyword.  First character was already
    // checked.
    kwp = سطر + startcol;
    kwlen = 0;
    کرو
    {
	اگر (has_mbyte)
	    kwlen += (*mb_ptr2len)(kwp + kwlen);
	ورنہ
	    ++kwlen;
    }
    جب (vim_iswordp_buf(kwp + kwlen, syn_buf));

    اگر (kwlen > MAXKEYWLEN)
	واپس 0;

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
    برائے (باری = 1; باری <= 2; ++باری)
    {
	ht = باری == 1 ? &syn_بلاک->b_keywtab : &syn_بلاک->b_keywtab_ic;
	اگر (ht->ht_مستعمل == 0)
	    جاری;
	اگر (باری == 2)	// ignore case
	    (خالی)str_foldcase(kwp, kwlen, keyword, MAXKEYWLEN + 1);

	/*
	 * Find keywords that match.  There can be several with different
	 * attributes.
	 * When current_next_list is non-zero accept only that group, otherwise:
	 *  Accept a not-contained keyword at toplevel.
	 *  Accept a keyword at other levels only if it is in the contains list.
	 */
	hi = hash_ڈھونڈو(ht, keyword);
	اگر (!HASHITEM_خالی_خانے(hi))
	    برائے (kp = HI2KE(hi); kp != تہی; kp = kp->ke_اگلا)
	    {
		اگر (current_اگلا_list != 0
			? اندر_id_list(تہی, current_اگلا_list, &kp->k_syn, 0)
			: (cur_si == تہی
			    ? !(kp->flags & HL_CONTAINED)
			    : اندر_id_list(cur_si, cur_si->si_cont_list,
				      &kp->k_syn, kp->flags)))
		{
		    *endcolp = startcol + kwlen;
		    *flagsp = kp->flags;
		    *اگلا_listp = kp->اگلا_list;
#اگر_تعریف_شدہ FEAT_CONCEAL
		    *ccharp = kp->k_char;
#اگر_ختم
		    واپس kp->k_syn.id;
		}
	    }
    }
    واپس 0;
}

/*
 * Handle ":syntax conceal" command.
 */
    ساکن خالی
syn_cmd_conceal(exarg_T *eap غیر_مستعمل, صحیح syncing غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_CONCEAL
    char_u	*arg = eap->arg;
    char_u	*اگلا;

    eap->nextcmd = ڈھونڈو_nextcmd(arg);
    اگر (eap->skip)
	واپس;

    اگلا = skiptowhite(arg);
    اگر (*arg == NUL)
    {
	اگر (curwin->w_s->b_syn_conceal)
	    پیغام("syntax conceal on");
	ورنہ
	    پیغام("syntax conceal off");
    }
    ورنہ اگر (STRNICMP(arg, "on", 2) == 0 && اگلا - arg == 2)
	curwin->w_s->b_syn_conceal = TRUE;
    ورنہ اگر (STRNICMP(arg, "off", 3) == 0 && اگلا - arg == 3)
	curwin->w_s->b_syn_conceal = FALSE;
    ورنہ
	semsg(_(e_illegal_دلیل_str_2), arg);
#اگر_ختم
}

/*
 * Handle ":syntax case" command.
 */
    ساکن خالی
syn_cmd_case(exarg_T *eap, صحیح syncing غیر_مستعمل)
{
    char_u	*arg = eap->arg;
    char_u	*اگلا;

    eap->nextcmd = ڈھونڈو_nextcmd(arg);
    اگر (eap->skip)
	واپس;

    اگلا = skiptowhite(arg);
    اگر (*arg == NUL)
    {
	اگر (curwin->w_s->b_syn_ic)
	    پیغام("syntax case ignore");
	ورنہ
	    پیغام("syntax case match");
    }
    ورنہ اگر (STRNICMP(arg, "match", 5) == 0 && اگلا - arg == 5)
	curwin->w_s->b_syn_ic = FALSE;
    ورنہ اگر (STRNICMP(arg, "ignore", 6) == 0 && اگلا - arg == 6)
	curwin->w_s->b_syn_ic = TRUE;
    ورنہ
	semsg(_(e_illegal_دلیل_str_2), arg);
}

/*
 * Handle ":syntax foldlevel" command.
 */
    ساکن خالی
syn_cmd_foldlevel(exarg_T *eap, صحیح syncing غیر_مستعمل)
{
    char_u *arg = eap->arg;
    char_u *arg_انتہا;

    eap->nextcmd = ڈھونڈو_nextcmd(arg);
    اگر (eap->skip)
	واپس;

    اگر (*arg == NUL)
    {
	انتخاب (curwin->w_s->b_syn_foldlevel)
	{
	    حالت SYNFLD_آغاز:   پیغام("syntax foldlevel start");   توڑو;
	    حالت SYNFLD_MINIMUM: پیغام("syntax foldlevel minimum"); توڑو;
	    طےشدہ: توڑو;
	}
	واپس;
    }

    arg_انتہا = skiptowhite(arg);
    اگر (STRNICMP(arg, "start", 5) == 0 && arg_انتہا - arg == 5)
	curwin->w_s->b_syn_foldlevel = SYNFLD_آغاز;
    ورنہ اگر (STRNICMP(arg, "minimum", 7) == 0 && arg_انتہا - arg == 7)
	curwin->w_s->b_syn_foldlevel = SYNFLD_MINIMUM;
    ورنہ
    {
	semsg(_(e_illegal_دلیل_str_2), arg);
	واپس;
    }

    arg = skipwhite(arg_انتہا);
    اگر (*arg != NUL)
    {
	semsg(_(e_illegal_دلیل_str_2), arg);
    }
}

/*
 * Handle ":syntax spell" command.
 */
    ساکن خالی
syn_cmd_spell(exarg_T *eap, صحیح syncing غیر_مستعمل)
{
    char_u	*arg = eap->arg;
    char_u	*اگلا;

    eap->nextcmd = ڈھونڈو_nextcmd(arg);
    اگر (eap->skip)
	واپس;

    اگلا = skiptowhite(arg);
    اگر (*arg == NUL)
    {
	اگر (curwin->w_s->b_syn_spell == SYNSPL_چوٹی)
	    پیغام("syntax spell toplevel");
	ورنہ اگر (curwin->w_s->b_syn_spell == SYNSPL_NOTOP)
	    پیغام("syntax spell notoplevel");
	ورنہ
	    پیغام("syntax spell default");
    }
    ورنہ اگر (STRNICMP(arg, "toplevel", 8) == 0 && اگلا - arg == 8)
	curwin->w_s->b_syn_spell = SYNSPL_چوٹی;
    ورنہ اگر (STRNICMP(arg, "notoplevel", 10) == 0 && اگلا - arg == 10)
	curwin->w_s->b_syn_spell = SYNSPL_NOTOP;
    ورنہ اگر (STRNICMP(arg, "default", 7) == 0 && اگلا - arg == 7)
	curwin->w_s->b_syn_spell = SYNSPL_DEFAULT;
    ورنہ
    {
	semsg(_(e_illegal_دلیل_str_2), arg);
	واپس;
    }

    // assume spell checking changed, force a redraw
    redraw_win_later(curwin, UPD_NOT_VALID);
}

/*
 * Handle ":syntax iskeyword" command.
 */
    ساکن خالی
syn_cmd_iskeyword(exarg_T *eap, صحیح syncing غیر_مستعمل)
{
    char_u	*arg = eap->arg;
    char_u	save_chartab[32];
    char_u	*save_isk;

    اگر (eap->skip)
	واپس;

    arg = skipwhite(arg);
    اگر (*arg == NUL)
    {
	پیغام_puts("\n");
	اگر (curwin->w_s->b_syn_isk != خالی_خانے_option)
	{
	    پیغام_puts("syntax iskeyword ");
	    پیغام_outtrans(curwin->w_s->b_syn_isk);
	}
	ورنہ
	    پیغام_outtrans((char_u *)_("syntax iskeyword not set"));
    }
    ورنہ
    {
	اگر (STRNICMP(arg, "clear", 5) == 0)
	{
	    mch_memmove(curwin->w_s->b_syn_chartab, curbuf->b_chartab,
								  (size_t)32);
	    clear_string_option(&curwin->w_s->b_syn_isk);
	}
	ورنہ
	{
	    اگر (check_isopt(arg) == FAIL)
	    {
		emsg(_(e_invalid_دلیل));
		واپس;
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
    خالی
syntax_clear(synblock_T *بلاک)
{
    صحیح i;

    بلاک->b_syn_error = FALSE;	    // clear previous error
#اگر_تعریف_شدہ FEAT_RELTIME
    بلاک->b_syn_slow = FALSE;	    // clear previous timeout
#اگر_ختم
    بلاک->b_syn_ic = FALSE;	    // Use case, by default
    بلاک->b_syn_foldlevel = SYNFLD_آغاز;
    بلاک->b_syn_spell = SYNSPL_DEFAULT; // default spell checking
    بلاک->b_syn_containedin = FALSE;
#اگر_تعریف_شدہ FEAT_CONCEAL
    بلاک->b_syn_conceal = FALSE;
#اگر_ختم

    // free the keywords
    clear_keywtab(&بلاک->b_keywtab);
    clear_keywtab(&بلاک->b_keywtab_ic);

    // free the syntax patterns
    برائے (i = بلاک->b_syn_patterns.ga_len; --i >= 0; )
	syn_clear_نمونہ(بلاک, i);
    ga_clear(&بلاک->b_syn_patterns);

    // free the syntax clusters
    برائے (i = بلاک->b_syn_clusters.ga_len; --i >= 0; )
	syn_clear_cluster(بلاک, i);
    ga_clear(&بلاک->b_syn_clusters);
    بلاک->b_spell_cluster_id = 0;
    بلاک->b_nospell_cluster_id = 0;

    بلاک->b_syn_sync_flags = 0;
    بلاک->b_syn_sync_minlines = 0;
    بلاک->b_syn_sync_maxlines = 0;
    بلاک->b_syn_sync_linebreaks = 0;

    vim_regfree(بلاک->b_syn_linecont_prog);
    بلاک->b_syn_linecont_prog = تہی;
    VIM_CLEAR(بلاک->b_syn_linecont_pat);
#اگر_تعریف_شدہ FEAT_FOLDING
    بلاک->b_syn_folditems = 0;
#اگر_ختم
    clear_string_option(&بلاک->b_syn_isk);

    // free the stored states
    syn_ڈھیر_free_all(بلاک);
    invalidate_current_state();

    // Reset the counter for ":syn include"
    running_syn_inc_لیبل = 0;
}

/*
 * Get rid of ownsyntax for window "wp".
 */
    خالی
reset_synblock(win_T *wp)
{
    اگر (wp->w_s != &wp->w_بفر->b_s)
    {
	syntax_clear(wp->w_s);
	vim_free(wp->w_s);
	wp->w_s = &wp->w_بفر->b_s;
    }
}

/*
 * Clear syncing info for one buffer.
 */
    ساکن خالی
syntax_sync_clear(خالی)
{
    صحیح i;

    // free the syntax patterns
    برائے (i = curwin->w_s->b_syn_patterns.ga_len; --i >= 0; )
	اگر (SYN_اشیاء(curwin->w_s)[i].sp_syncing)
	    syn_remove_نمونہ(curwin->w_s, i);

    curwin->w_s->b_syn_sync_flags = 0;
    curwin->w_s->b_syn_sync_minlines = 0;
    curwin->w_s->b_syn_sync_maxlines = 0;
    curwin->w_s->b_syn_sync_linebreaks = 0;

    vim_regfree(curwin->w_s->b_syn_linecont_prog);
    curwin->w_s->b_syn_linecont_prog = تہی;
    VIM_CLEAR(curwin->w_s->b_syn_linecont_pat);
    clear_string_option(&curwin->w_s->b_syn_isk);

    syn_ڈھیر_free_all(curwin->w_s);	// Need to recompute all syntax.
}

/*
 * Remove one pattern from the buffer's pattern list.
 */
    ساکن خالی
syn_remove_نمونہ(
    synblock_T	*بلاک,
    صحیح		idx)
{
    synpat_T	*spp;

    spp = &(SYN_اشیاء(بلاک)[idx]);
#اگر_تعریف_شدہ FEAT_FOLDING
    اگر (spp->sp_flags & HL_FOLD)
	--بلاک->b_syn_folditems;
#اگر_ختم
    syn_clear_نمونہ(بلاک, idx);
    mch_memmove(spp, spp + 1,
		   حجم(synpat_T) * (بلاک->b_syn_patterns.ga_len - idx - 1));
    --بلاک->b_syn_patterns.ga_len;
}

/*
 * Clear and free one syntax pattern.  When clearing all, must be called from
 * last to first!
 */
    ساکن خالی
syn_clear_نمونہ(synblock_T *بلاک, صحیح i)
{
    vim_free(SYN_اشیاء(بلاک)[i].sp_نمونہ);
    vim_regfree(SYN_اشیاء(بلاک)[i].sp_prog);
    // Only free sp_cont_list and sp_next_list of first start pattern
    اگر (i == 0 || SYN_اشیاء(بلاک)[i - 1].sp_type != SPTYPE_آغاز)
    {
	vim_free(SYN_اشیاء(بلاک)[i].sp_cont_list);
	vim_free(SYN_اشیاء(بلاک)[i].sp_اگلا_list);
	vim_free(SYN_اشیاء(بلاک)[i].sp_syn.cont_اندر_list);
    }
}

/*
 * Clear and free one syntax cluster.
 */
    ساکن خالی
syn_clear_cluster(synblock_T *بلاک, صحیح i)
{
    vim_free(SYN_CLSTR(بلاک)[i].scl_name);
    vim_free(SYN_CLSTR(بلاک)[i].scl_name_u);
    vim_free(SYN_CLSTR(بلاک)[i].scl_list);
}

/*
 * Handle ":syntax clear" command.
 */
    ساکن خالی
syn_cmd_clear(exarg_T *eap, صحیح syncing)
{
    char_u	*arg = eap->arg;
    char_u	*arg_انتہا;
    صحیح		id;

    eap->nextcmd = ڈھونڈو_nextcmd(arg);
    اگر (eap->skip)
	واپس;

    /*
     * We have to disable this within ":syn include @group filename",
     * because otherwise @group would get deleted.
     * Only required for Vim 5.x syntax files, 6.0 ones don't contain ":syn
     * clear".
     */
    اگر (curwin->w_s->b_syn_topgrp != 0)
	واپس;

    اگر (سرے_excmd2(eap->cmd, arg))
    {
	/*
	 * No argument: Clear all syntax items.
	 */
	اگر (syncing)
	    syntax_sync_clear();
	ورنہ
	{
	    syntax_clear(curwin->w_s);
	    اگر (curwin->w_s == &curwin->w_بفر->b_s)
		do_unlet((char_u *)"b:current_syntax", TRUE);
	    do_unlet((char_u *)"w:current_syntax", TRUE);
	}
    }
    ورنہ
    {
	/*
	 * Clear the group IDs that are in the argument.
	 */
	جب (!سرے_excmd2(eap->cmd, arg))
	{
	    arg_انتہا = skiptowhite(arg);
	    اگر (*arg == '@')
	    {
		id = syn_scl_namen2id(arg + 1, (صحیح)(arg_انتہا - arg - 1));
		اگر (id == 0)
		{
		    semsg(_(e_no_such_syntax_cluster_str_1), arg);
		    توڑو;
		}
		ورنہ
		{
		    /*
		     * We can't physically delete a cluster without changing
		     * the IDs of other clusters, so we do the next best thing
		     * and make it empty.
		     */
		    مختصر scl_id = id - SYNID_CLUSTER;

		    VIM_CLEAR(SYN_CLSTR(curwin->w_s)[scl_id].scl_list);
		}
	    }
	    ورنہ
	    {
		id = syn_namen2id(arg, (صحیح)(arg_انتہا - arg));
		اگر (id == 0)
		{
		    semsg(_(e_no_such_highlight_گروہ_name_str), arg);
		    توڑو;
		}
		ورنہ
		    syn_clear_one(id, syncing);
	    }
	    arg = skipwhite(arg_انتہا);
	}
    }
    redraw_curbuf_later(UPD_SOME_VALID);
    syn_ڈھیر_free_all(curwin->w_s);		// Need to recompute all syntax.
}

/*
 * Clear one syntax group for the current buffer.
 */
    ساکن خالی
syn_clear_one(صحیح id, صحیح syncing)
{
    synpat_T	*spp;
    صحیح		idx;

    // Clear keywords only when not ":syn sync clear group-name"
    اگر (!syncing)
    {
	(خالی)syn_clear_keyword(id, &curwin->w_s->b_keywtab);
	(خالی)syn_clear_keyword(id, &curwin->w_s->b_keywtab_ic);
    }

    // clear the patterns for "id"
    برائے (idx = curwin->w_s->b_syn_patterns.ga_len; --idx >= 0; )
    {
	spp = &(SYN_اشیاء(curwin->w_s)[idx]);
	اگر (spp->sp_syn.id != id || spp->sp_syncing != syncing)
	    جاری;
	syn_remove_نمونہ(curwin->w_s, idx);
    }
}

/*
 * Handle ":syntax on" command.
 */
    ساکن خالی
syn_cmd_on(exarg_T *eap, صحیح syncing غیر_مستعمل)
{
    syn_cmd_onoff(eap, "syntax");
}

/*
 * Handle ":syntax enable" command.
 */
    ساکن خالی
syn_cmd_enable(exarg_T *eap, صحیح syncing غیر_مستعمل)
{
    سیٹ_اندرونی_string_var((char_u *)"g:syntax_cmd", (char_u *)"enable");
    syn_cmd_onoff(eap, "syntax");
    do_unlet((char_u *)"g:syntax_cmd", TRUE);
}

/*
 * Handle ":syntax reset" command.
 * It actually resets highlighting, not syntax.
 */
    ساکن خالی
syn_cmd_reset(exarg_T *eap, صحیح syncing غیر_مستعمل)
{
    سیٹ_nextcmd(eap, eap->arg);
    اگر (!eap->skip)
    {
	سیٹ_اندرونی_string_var((char_u *)"g:syntax_cmd", (char_u *)"reset");
	do_cmdline_cmd((char_u *)"runtime! syntax/syncolor.vim");
	do_unlet((char_u *)"g:syntax_cmd", TRUE);
    }
}

/*
 * Handle ":syntax manual" command.
 */
    ساکن خالی
syn_cmd_manual(exarg_T *eap, صحیح syncing غیر_مستعمل)
{
    syn_cmd_onoff(eap, "manual");
}

/*
 * Handle ":syntax off" command.
 */
    ساکن خالی
syn_cmd_off(exarg_T *eap, صحیح syncing غیر_مستعمل)
{
    syn_cmd_onoff(eap, "nosyntax");
}

    ساکن خالی
syn_cmd_onoff(exarg_T *eap, حرف *name)
{
    char_u	buf[100];

    سیٹ_nextcmd(eap, eap->arg);
    اگر (!eap->skip)
    {
	STRCPY(buf, "so ");
	vim_snprintf((حرف *)buf + 3, حجم(buf) - 3, SYNTAX_FNAME, name);
	do_cmdline_cmd(buf);
    }
}

/*
 * Handle ":syntax [list]" command: list current syntax words.
 */
    ساکن خالی
syn_cmd_list(
    exarg_T	*eap,
    صحیح		syncing)	    // when TRUE: list syncing items
{
    char_u	*arg = eap->arg;
    صحیح		id;
    char_u	*arg_انتہا;

    eap->nextcmd = ڈھونڈو_nextcmd(arg);
    اگر (eap->skip)
	واپس;

    اگر (!syntax_present(curwin))
    {
	پیغام(_(پیغام_no_اشیاء));
	واپس;
    }

    اگر (syncing)
    {
	اگر (curwin->w_s->b_syn_sync_flags & SF_CCOMMENT)
	{
	    پیغام_puts(_("syncing on C-style comments"));
	    syn_سطریں_پیغام();
	    syn_مطابقت_پیغام();
	}
	ورنہ اگر (curwin->w_s->b_syn_sync_flags & SF_مطابقت)
	{
	    پیغام_puts_title(_("\n--- Syntax sync items ---"));
	    اگر (curwin->w_s->b_syn_sync_minlines > 0
		|| curwin->w_s->b_syn_sync_maxlines > 0
		|| curwin->w_s->b_syn_sync_linebreaks > 0)
	    {
		پیغام_puts(_("\nsyncing on items"));
		syn_سطریں_پیغام();
		syn_مطابقت_پیغام();
	    }
	    برائے (id = 1; id <= highlight_عدد_groups() && !got_int; ++id)
		syn_list_one(id, syncing, FALSE);
	}
	ورنہ
	{
	    اگر (curwin->w_s->b_syn_sync_minlines == 0)
		پیغام_puts(_("no syncing"));
	    ورنہ
	    {
		اگر (curwin->w_s->b_syn_sync_minlines == MAXLNUM)
		    پیغام_puts(_("syncing starts at the first line"));
		ورنہ
		{
		    پیغام_puts(_("syncing starts "));
		    پیغام_outnum(curwin->w_s->b_syn_sync_minlines);
		    پیغام_puts(_(" lines before top line"));
		}
		syn_مطابقت_پیغام();
	    }
	}
	واپس;
    }

    پیغام_puts_title(_("\n--- Syntax items ---"));
    اگر (سرے_excmd2(eap->cmd, arg))
    {
	/*
	 * No argument: List all group IDs and all syntax clusters.
	 */
	برائے (id = 1; id <= highlight_عدد_groups() && !got_int; ++id)
	    syn_list_one(id, syncing, FALSE);
	برائے (id = 0; id < curwin->w_s->b_syn_clusters.ga_len && !got_int; ++id)
	    syn_list_cluster(id);
    }
    ورنہ
    {
	/*
	 * List the group IDs and syntax clusters that are in the argument.
	 */
	جب (!سرے_excmd2(eap->cmd, arg) && !got_int)
	{
	    arg_انتہا = skiptowhite(arg);
	    اگر (*arg == '@')
	    {
		id = syn_scl_namen2id(arg + 1, (صحیح)(arg_انتہا - arg - 1));
		اگر (id == 0)
		    semsg(_(e_no_such_syntax_cluster_str_2), arg);
		ورنہ
		    syn_list_cluster(id - SYNID_CLUSTER);
	    }
	    ورنہ
	    {
		id = syn_namen2id(arg, (صحیح)(arg_انتہا - arg));
		اگر (id == 0)
		    semsg(_(e_no_such_highlight_گروہ_name_str), arg);
		ورنہ
		    syn_list_one(id, syncing, TRUE);
	    }
	    arg = skipwhite(arg_انتہا);
	}
    }
    سیٹ_nextcmd(eap, arg);
}

    ساکن خالی
syn_سطریں_پیغام(خالی)
{
    اگر (curwin->w_s->b_syn_sync_maxlines > 0
				      || curwin->w_s->b_syn_sync_minlines > 0)
    {
	پیغام_puts("; ");
	اگر (curwin->w_s->b_syn_sync_minlines == MAXLNUM)
	    پیغام_puts(_("from the first line"));
	ورنہ
	{
	    اگر (curwin->w_s->b_syn_sync_minlines > 0)
	    {
		پیغام_puts(_("minimal "));
		پیغام_outnum(curwin->w_s->b_syn_sync_minlines);
		اگر (curwin->w_s->b_syn_sync_maxlines)
		    پیغام_puts(", ");
	    }
	    اگر (curwin->w_s->b_syn_sync_maxlines > 0)
	    {
		پیغام_puts(_("maximal "));
		پیغام_outnum(curwin->w_s->b_syn_sync_maxlines);
	    }
	    پیغام_puts(_(" lines before top line"));
	}
    }
}

    ساکن خالی
syn_مطابقت_پیغام(خالی)
{
    اگر (curwin->w_s->b_syn_sync_linebreaks > 0)
    {
	پیغام_puts(_("; match "));
	پیغام_outnum(curwin->w_s->b_syn_sync_linebreaks);
	پیغام_puts(_(" line breaks"));
    }
}

ساکن صحیح  آخری_matchgroup;

ساکن خالی syn_list_flags(keyvalue_T *nlist, صحیح nr_entries, صحیح flags, صحیح attr);

/*
 * List one syntax item, for ":syntax" or "syntax list syntax_name".
 */
    ساکن خالی
syn_list_one(
    صحیح		id,
    صحیح		syncing,	    // when TRUE: list syncing items
    صحیح		link_only)	    // when TRUE; list link-only too
{
    صحیح		attr;
    صحیح		idx;
    صحیح		did_سرخی = FALSE;
    synpat_T	*spp;
    ساکن keyvalue_T namelist1[] =
		{
		    KEYVALUE_اندراج(HL_DISPLAY, "display"),
		    KEYVALUE_اندراج(HL_CONTAINED, "contained"),
		    KEYVALUE_اندراج(HL_ONELINE, "oneline"),
		    KEYVALUE_اندراج(HL_KEEPEND, "keepend"),
		    KEYVALUE_اندراج(HL_EXTEND, "extend"),
		    KEYVALUE_اندراج(HL_EXCLUDENL, "excludenl"),
		    KEYVALUE_اندراج(HL_TRANSP, "transparent"),
		    KEYVALUE_اندراج(HL_FOLD, "fold")
#اگر_تعریف_شدہ FEAT_CONCEAL
		    ,
		    KEYVALUE_اندراج(HL_CONCEAL, "conceal"),
		    KEYVALUE_اندراج(HL_CONCEALENDS, "concealends")
#اگر_ختم
		};
    ساکن keyvalue_T namelist2[] =
		{
		    KEYVALUE_اندراج(HL_SKIPWHITE, "skipwhite"),
		    KEYVALUE_اندراج(HL_SKIPNL, "skipnl"),
		    KEYVALUE_اندراج(HL_SKIPEMPTY, "skipempty")
		};

    attr = HL_ATTR(HLF_D);		// highlight like directories

    // list the keywords for "id"
    اگر (!syncing)
    {
	did_سرخی = syn_list_keywords(id, &curwin->w_s->b_keywtab, FALSE, attr);
	did_سرخی = syn_list_keywords(id, &curwin->w_s->b_keywtab_ic,
							    did_سرخی, attr);
    }

    // list the patterns for "id"
    برائے (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len && !got_int; ++idx)
    {
	spp = &(SYN_اشیاء(curwin->w_s)[idx]);
	اگر (spp->sp_syn.id != id || spp->sp_syncing != syncing)
	    جاری;

	(خالی)syn_list_سرخی(did_سرخی, 999, id);
	did_سرخی = TRUE;
	آخری_matchgroup = 0;
	اگر (spp->sp_type == SPTYPE_مطابقت)
	{
	    put_نمونہ("match", ' ', spp, attr);
	    پیغام_putchar(' ');
	}
	ورنہ اگر (spp->sp_type == SPTYPE_آغاز)
	{
	    جب (SYN_اشیاء(curwin->w_s)[idx].sp_type == SPTYPE_آغاز)
		put_نمونہ("start", '=', &SYN_اشیاء(curwin->w_s)[idx++], attr);
	    اگر (SYN_اشیاء(curwin->w_s)[idx].sp_type == SPTYPE_SKIP)
		put_نمونہ("skip", '=', &SYN_اشیاء(curwin->w_s)[idx++], attr);
	    جب (idx < curwin->w_s->b_syn_patterns.ga_len
			  && SYN_اشیاء(curwin->w_s)[idx].sp_type == SPTYPE_انتہا)
		put_نمونہ("end", '=', &SYN_اشیاء(curwin->w_s)[idx++], attr);
	    --idx;
	    پیغام_putchar(' ');
	}
	syn_list_flags(namelist1, (صحیح)ARRAY_لمبائی(namelist1), spp->sp_flags, attr);

	اگر (spp->sp_cont_list != تہی)
	    put_id_list((char_u *)"contains", spp->sp_cont_list, attr);

	اگر (spp->sp_syn.cont_اندر_list != تہی)
	    put_id_list((char_u *)"containedin",
					      spp->sp_syn.cont_اندر_list, attr);

	اگر (spp->sp_اگلا_list != تہی)
	{
	    put_id_list((char_u *)"nextgroup", spp->sp_اگلا_list, attr);
	    syn_list_flags(namelist2, (صحیح)ARRAY_لمبائی(namelist2), spp->sp_flags, attr);
	}
	اگر (spp->sp_flags & (HL_SYNC_یہاں|HL_SYNC_THERE))
	{
	    اگر (spp->sp_flags & HL_SYNC_یہاں)
		پیغام_puts_attr("grouphere", attr);
	    ورنہ
		پیغام_puts_attr("groupthere", attr);
	    پیغام_putchar(' ');
	    اگر (spp->sp_sync_idx >= 0)
		پیغام_outtrans(highlight_گروہ_name(SYN_اشیاء(curwin->w_s)
				   [spp->sp_sync_idx].sp_syn.id - 1));
	    ورنہ
		پیغام_puts("NONE");
	    پیغام_putchar(' ');
	}
    }

    // list the link, if there is one
    اگر (highlight_link_id(id - 1) && (did_سرخی || link_only) && !got_int)
    {
	(خالی)syn_list_سرخی(did_سرخی, 999, id);
	پیغام_puts_attr("links to", attr);
	پیغام_putchar(' ');
	پیغام_outtrans(highlight_گروہ_name(highlight_link_id(id - 1) - 1));
    }
}

    ساکن خالی
syn_list_flags(keyvalue_T *nlist, صحیح nr_entries, صحیح flags, صحیح attr)
{
    صحیح		i;

    برائے (i = 0; i < nr_entries; ++i)
	اگر (flags & nlist[i].کلید)
	{
	    پیغام_puts_attr((حرف *)nlist[i].قدر.string, attr);
	    پیغام_putchar(' ');
	}
}

/*
 * List one syntax cluster, for ":syntax" or "syntax list syntax_name".
 */
    ساکن خالی
syn_list_cluster(صحیح id)
{
    صحیح	    endcol = 15;

    // slight hack:  roughly duplicate the guts of syn_list_header()
    پیغام_putchar('\n');
    پیغام_outtrans(SYN_CLSTR(curwin->w_s)[id].scl_name);

    اگر (پیغام_ستون >= endcol)	// output at least one space
	endcol = پیغام_ستون + 1;
    اگر (cmdline_چوڑائی <= endcol)	// avoid hang for tiny window
	endcol = cmdline_چوڑائی - 1;

    پیغام_advance(endcol);
    اگر (SYN_CLSTR(curwin->w_s)[id].scl_list != تہی)
    {
	put_id_list((char_u *)"cluster", SYN_CLSTR(curwin->w_s)[id].scl_list,
		    HL_ATTR(HLF_D));
    }
    ورنہ
    {
	پیغام_puts_attr("cluster", HL_ATTR(HLF_D));
	پیغام_puts("=NONE");
    }
}

    ساکن خالی
put_id_list(char_u *name, مختصر *list, صحیح attr)
{
    مختصر		*p;

    پیغام_puts_attr((حرف *)name, attr);
    پیغام_putchar('=');
    برائے (p = list; *p; ++p)
    {
	اگر (*p >= SYNID_ALLBUT && *p < SYNID_چوٹی)
	{
	    اگر (p[1])
		پیغام_puts("ALLBUT");
	    ورنہ
		پیغام_puts("ALL");
	}
	ورنہ اگر (*p >= SYNID_چوٹی && *p < SYNID_CONTAINED)
	{
	    پیغام_puts("TOP");
	}
	ورنہ اگر (*p >= SYNID_CONTAINED && *p < SYNID_CLUSTER)
	{
	    پیغام_puts("CONTAINED");
	}
	ورنہ اگر (*p >= SYNID_CLUSTER)
	{
	    مختصر scl_id = *p - SYNID_CLUSTER;

	    پیغام_putchar('@');
	    پیغام_outtrans(SYN_CLSTR(curwin->w_s)[scl_id].scl_name);
	}
	ورنہ
	    پیغام_outtrans(highlight_گروہ_name(*p - 1));
	اگر (p[1])
	    پیغام_putchar(',');
    }
    پیغام_putchar(' ');
}

    ساکن خالی
put_نمونہ(
    حرف	*s,
    صحیح		c,
    synpat_T	*spp,
    صحیح		attr)
{
    طویل	n;
    صحیح		نقاب;
    صحیح		پہلا;
    ساکن حرف	*sepchars = "/+=-#@\"|'^&";
    صحیح		i;

    // May have to write "matchgroup=group"
    اگر (آخری_matchgroup != spp->sp_syn_مطابقت_id)
    {
	آخری_matchgroup = spp->sp_syn_مطابقت_id;
	پیغام_puts_attr("matchgroup", attr);
	پیغام_putchar('=');
	اگر (آخری_matchgroup == 0)
	    پیغام_outtrans((char_u *)"NONE");
	ورنہ
	    پیغام_outtrans(highlight_گروہ_name(آخری_matchgroup - 1));
	پیغام_putchar(' ');
    }

    // output the name of the pattern and an '=' or ' '
    پیغام_puts_attr(s, attr);
    پیغام_putchar(c);

    // output the pattern, in between a char that is not in the pattern
    برائے (i = 0; vim_strchr(spp->sp_نمونہ, sepchars[i]) != تہی; )
	اگر (sepchars[++i] == NUL)
	{
	    i = 0;	// no good char found, just use the first one
	    توڑو;
	}
    پیغام_putchar(sepchars[i]);
    پیغام_outtrans(spp->sp_نمونہ);
    پیغام_putchar(sepchars[i]);

    // output any pattern options
    پہلا = TRUE;
    برائے (i = 0; i < SPO_گنتی; ++i)
    {
	نقاب = (1 << i);
	اگر (spp->sp_off_flags & (نقاب + (نقاب << SPO_گنتی)))
	{
	    اگر (!پہلا)
		پیغام_putchar(',');	// separate with commas
	    پیغام_puts(spo_name_tab[i]);
	    n = spp->sp_offsets[i];
	    اگر (i != SPO_LC_OFF)
	    {
		اگر (spp->sp_off_flags & نقاب)
		    پیغام_putchar('s');
		ورنہ
		    پیغام_putchar('e');
		اگر (n > 0)
		    پیغام_putchar('+');
	    }
	    اگر (n || i == SPO_LC_OFF)
		پیغام_outnum(n);
	    پہلا = FALSE;
	}
    }
    پیغام_putchar(' ');
}

/*
 * List or clear the keywords for one syntax group.
 * Return TRUE if the header has been printed.
 */
    ساکن صحیح
syn_list_keywords(
    صحیح		id,
    hashtab_T	*ht,
    صحیح		did_سرخی,		// header has already been printed
    صحیح		attr)
{
    صحیح		outlen;
    hashitem_T	*hi;
    keyentry_T	*kp;
    صحیح		todo;
    صحیح		پچھلا_contained = 0;
    مختصر	*پچھلا_اگلا_list = تہی;
    مختصر	*پچھلا_cont_اندر_list = تہی;
    صحیح		پچھلا_skipnl = 0;
    صحیح		پچھلا_skipwhite = 0;
    صحیح		پچھلا_skipempty = 0;

    /*
     * Unfortunately, this list of keywords is not sorted on alphabet but on
     * hash value...
     */
    todo = (صحیح)ht->ht_مستعمل;
    برائے (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    --todo;
	    برائے (kp = HI2KE(hi); kp != تہی && !got_int; kp = kp->ke_اگلا)
	    {
		اگر (kp->k_syn.id == id)
		{
		    اگر (پچھلا_contained != (kp->flags & HL_CONTAINED)
			    || پچھلا_skipnl != (kp->flags & HL_SKIPNL)
			    || پچھلا_skipwhite != (kp->flags & HL_SKIPWHITE)
			    || پچھلا_skipempty != (kp->flags & HL_SKIPEMPTY)
			    || پچھلا_cont_اندر_list != kp->k_syn.cont_اندر_list
			    || پچھلا_اگلا_list != kp->اگلا_list)
			outlen = 9999;
		    ورنہ
			outlen = (صحیح)STRLEN(kp->keyword);
		    // output "contained" and "nextgroup" on each line
		    اگر (syn_list_سرخی(did_سرخی, outlen, id))
		    {
			پچھلا_contained = 0;
			پچھلا_اگلا_list = تہی;
			پچھلا_cont_اندر_list = تہی;
			پچھلا_skipnl = 0;
			پچھلا_skipwhite = 0;
			پچھلا_skipempty = 0;
		    }
		    did_سرخی = TRUE;
		    اگر (پچھلا_contained != (kp->flags & HL_CONTAINED))
		    {
			پیغام_puts_attr("contained", attr);
			پیغام_putchar(' ');
			پچھلا_contained = (kp->flags & HL_CONTAINED);
		    }
		    اگر (kp->k_syn.cont_اندر_list != پچھلا_cont_اندر_list)
		    {
			put_id_list((char_u *)"containedin",
						kp->k_syn.cont_اندر_list, attr);
			پیغام_putchar(' ');
			پچھلا_cont_اندر_list = kp->k_syn.cont_اندر_list;
		    }
		    اگر (kp->اگلا_list != پچھلا_اگلا_list)
		    {
			put_id_list((char_u *)"nextgroup", kp->اگلا_list, attr);
			پیغام_putchar(' ');
			پچھلا_اگلا_list = kp->اگلا_list;
			اگر (kp->flags & HL_SKIPNL)
			{
			    پیغام_puts_attr("skipnl", attr);
			    پیغام_putchar(' ');
			    پچھلا_skipnl = (kp->flags & HL_SKIPNL);
			}
			اگر (kp->flags & HL_SKIPWHITE)
			{
			    پیغام_puts_attr("skipwhite", attr);
			    پیغام_putchar(' ');
			    پچھلا_skipwhite = (kp->flags & HL_SKIPWHITE);
			}
			اگر (kp->flags & HL_SKIPEMPTY)
			{
			    پیغام_puts_attr("skipempty", attr);
			    پیغام_putchar(' ');
			    پچھلا_skipempty = (kp->flags & HL_SKIPEMPTY);
			}
		    }
		    پیغام_outtrans(kp->keyword);
		}
	    }
	}
    }

    واپس did_سرخی;
}

    ساکن خالی
syn_clear_keyword(صحیح id, hashtab_T *ht)
{
    hashitem_T	*hi;
    keyentry_T	*kp;
    keyentry_T	*kp_پچھلا;
    keyentry_T	*kp_اگلا;
    صحیح		todo;

    hash_تالا(ht);
    todo = (صحیح)ht->ht_مستعمل;
    FOR_ALL_HASHTAB_اشیاء(ht, hi, todo)
    {
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    --todo;
	    kp_پچھلا = تہی;
	    برائے (kp = HI2KE(hi); kp != تہی; )
	    {
		اگر (kp->k_syn.id == id)
		{
		    kp_اگلا = kp->ke_اگلا;
		    اگر (kp_پچھلا == تہی)
		    {
			اگر (kp_اگلا == تہی)
			    hash_remove(ht, hi, "syntax clear keyword");
			ورنہ
			    hi->hi_کلید = KE2HIKEY(kp_اگلا);
		    }
		    ورنہ
			kp_پچھلا->ke_اگلا = kp_اگلا;
		    vim_free(kp->اگلا_list);
		    vim_free(kp->k_syn.cont_اندر_list);
		    vim_free(kp);
		    kp = kp_اگلا;
		}
		ورنہ
		{
		    kp_پچھلا = kp;
		    kp = kp->ke_اگلا;
		}
	    }
	}
    }
    hash_unlock(ht);
}

/*
 * Clear a whole keyword table.
 */
    ساکن خالی
clear_keywtab(hashtab_T *ht)
{
    hashitem_T	*hi;
    صحیح		todo;
    keyentry_T	*kp;
    keyentry_T	*kp_اگلا;

    todo = (صحیح)ht->ht_مستعمل;
    FOR_ALL_HASHTAB_اشیاء(ht, hi, todo)
    {
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    --todo;
	    برائے (kp = HI2KE(hi); kp != تہی; kp = kp_اگلا)
	    {
		kp_اگلا = kp->ke_اگلا;
		vim_free(kp->اگلا_list);
		vim_free(kp->k_syn.cont_اندر_list);
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
    ساکن خالی
جوڑو_keyword(
    char_u	*name,	    // name of keyword
    size_t	namelen,    // length of keyword (excluding the NUL)
    صحیح		id,	    // group ID for this keyword
    صحیح		flags,	    // flags for this keyword
    مختصر	*cont_اندر_list, // containedin for this keyword
    مختصر	*اگلا_list, // nextgroup for this keyword
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
    ورنہ
    {
	name_ic = name;
	name_iclen = namelen;
    }
    kp = alloc(offsetof(keyentry_T, keyword) + name_iclen + 1);
    اگر (kp == تہی)
	واپس;
    STRCPY(kp->keyword, name_ic);
    kp->k_syn.id = id;
    kp->k_syn.inc_لیبل = current_syn_inc_لیبل;
    kp->flags = flags;
    kp->k_char = conceal_char;
    kp->k_syn.cont_اندر_list = copy_id_list(cont_اندر_list);
    اگر (cont_اندر_list != تہی)
	curwin->w_s->b_syn_containedin = TRUE;
    kp->اگلا_list = copy_id_list(اگلا_list);

    اگر (curwin->w_s->b_syn_ic)
	ht = &curwin->w_s->b_keywtab_ic;
    ورنہ
	ht = &curwin->w_s->b_keywtab;

    hash = hash_hash(kp->keyword);
    hi = hash_lookup(ht, kp->keyword, hash);
    اگر (HASHITEM_خالی_خانے(hi))
    {
	// new keyword, add to hashtable
	kp->ke_اگلا = تہی;
	hash_جوڑو_شے(ht, hi, kp->keyword, hash);
    }
    ورنہ
    {
	// keyword already exists, prepend to list
	kp->ke_اگلا = HI2KE(hi);
	hi->hi_کلید = KE2HIKEY(kp);
    }
}

/*
 * Get the start and end of the group name argument.
 * Return a pointer to the first argument.
 * Return NULL if the end of the command was found instead of further args.
 */
    ساکن char_u *
get_گروہ_name(
    char_u	*arg,		// start of the argument
    char_u	**name_انتہا)	// pointer to end of the name
{
    char_u	*rest;

    *name_انتہا = skiptowhite(arg);
    rest = skipwhite(*name_انتہا);

    /*
     * Check if there are enough arguments.  The first argument may be a
     * pattern, where '|' is allowed, so only check for NUL.
     */
    اگر (سرے_excmd(*arg) || *rest == NUL)
	واپس تہی;
    واپس rest;
}

/*
 * Check for syntax command option arguments.
 * This can be called at any place in the list of arguments, and just picks
 * out the arguments that are known.  Can be called several times in a row to
 * collect all options in between other arguments.
 * Return a pointer to the next argument (which isn't an option).
 * Return NULL for any error;
 */
    ساکن char_u *
get_syn_options(
    char_u	    *آغاز,		// next argument to be checked
    syn_opt_arg_T   *opt,		// various things
    صحیح		    *conceal_char غیر_مستعمل,
    صحیح		    skip)		// TRUE if skipping over command
{
    char_u	*arg = آغاز;
    char_u	*gname_آغاز, *gname;
    صحیح		syn_id;
    صحیح		len;
    حرف	*p;
    صحیح		i;
    صحیح		fidx;
    ساکن ساخت flag
    {
	حرف	*name;
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
		    {"gGrRoOuUpPhHeErReE",	0,	HL_SYNC_یہاں},
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
    ساکن حرف *پہلا_letters = "cCoOkKeEtTsSgGdDfFnN";

    اگر (arg == تہی)		// already detected error
	واپس تہی;

#اگر_تعریف_شدہ FEAT_CONCEAL
    اگر (curwin->w_s->b_syn_conceal)
	opt->flags |= HL_CONCEAL;
#اگر_ختم

    برائے (;;)
    {
	/*
	 * This is used very often when a large number of keywords is defined.
	 * Need to skip quickly when no option name is found.
	 * Also avoid tolower(), it's slow.
	 */
	اگر (strchr(پہلا_letters, *arg) == تہی)
	    توڑو;

	برائے (fidx = ARRAY_لمبائی(flagtab); --fidx >= 0; )
	{
	    p = flagtab[fidx].name;
	    برائے (i = 0, len = 0; p[i] != NUL; i += 2, ++len)
		اگر (arg[len] != p[i] && arg[len] != p[i + 1])
		    توڑو;
	    اگر (p[i] == NUL && (VIM_ISWHITE(arg[len])
				    || (flagtab[fidx].argtype > 0
					 ? arg[len] == '='
					 : سرے_excmd2(آغاز, arg + len))))
	    {
		اگر (opt->keyword
			&& (flagtab[fidx].flags == HL_DISPLAY
			    || flagtab[fidx].flags == HL_FOLD
			    || flagtab[fidx].flags == HL_EXTEND))
		    // treat "display", "fold" and "extend" as a keyword
		    fidx = -1;
		توڑو;
	    }
	}
	اگر (fidx < 0)	    // no match found
	    توڑو;

	اگر (flagtab[fidx].argtype == 1)
	{
	    اگر (!opt->has_cont_list)
	    {
		emsg(_(e_شامل_ہے_دلیل_not_accepted_یہاں));
		واپس تہی;
	    }
	    اگر (get_id_list(&arg, 8, &opt->cont_list, skip) == FAIL)
		واپس تہی;
	}
	ورنہ اگر (flagtab[fidx].argtype == 2)
	{
	    اگر (get_id_list(&arg, 11, &opt->cont_اندر_list, skip) == FAIL)
		واپس تہی;
	}
	ورنہ اگر (flagtab[fidx].argtype == 3)
	{
	    اگر (get_id_list(&arg, 9, &opt->اگلا_list, skip) == FAIL)
		واپس تہی;
	}
	ورنہ اگر (flagtab[fidx].argtype == 11 && arg[5] == '=')
	{
	    // cchar=?
	    اگر (has_mbyte)
	    {
#اگر_تعریف_شدہ FEAT_CONCEAL
		*conceal_char = mb_ptr2char(arg + 6);
#اگر_ختم
		arg += mb_ptr2len(arg + 6) - 1;
	    }
	    ورنہ
	    {
#اگر_تعریف_شدہ FEAT_CONCEAL
		*conceal_char = arg[6];
#ورنہ
		;
#اگر_ختم
	    }
#اگر_تعریف_شدہ FEAT_CONCEAL
	    اگر (!vim_isprintc_strict(*conceal_char))
	    {
		emsg(_(e_invalid_cchar_قدر));
		واپس تہی;
	    }
#اگر_ختم
	    arg = skipwhite(arg + 7);
	}
	ورنہ
	{
	    opt->flags |= flagtab[fidx].flags;
	    arg = skipwhite(arg + len);

	    اگر (flagtab[fidx].flags == HL_SYNC_یہاں
		    || flagtab[fidx].flags == HL_SYNC_THERE)
	    {
		اگر (opt->sync_idx == تہی)
		{
		    emsg(_(e_groupthere_not_accepted_یہاں));
		    واپس تہی;
		}
		gname_آغاز = arg;
		arg = skiptowhite(arg);
		اگر (gname_آغاز == arg)
		    واپس تہی;
		gname = vim_strnsave(gname_آغاز, arg - gname_آغاز);
		اگر (gname == تہی)
		    واپس تہی;
		اگر (STRCMP(gname, "NONE") == 0)
		    *opt->sync_idx = NONE_IDX;
		ورنہ
		{
		    syn_id = syn_name2id(gname);
		    برائے (i = curwin->w_s->b_syn_patterns.ga_len; --i >= 0; )
			اگر (SYN_اشیاء(curwin->w_s)[i].sp_syn.id == syn_id
			      && SYN_اشیاء(curwin->w_s)[i].sp_type
							       == SPTYPE_آغاز)
			{
			    *opt->sync_idx = i;
			    توڑو;
			}
		    اگر (i < 0)
		    {
			semsg(_(e_didnt_ڈھونڈو_region_شے_for_str), gname);
			vim_free(gname);
			واپس تہی;
		    }
		}

		vim_free(gname);
		arg = skipwhite(arg);
	    }
#اگر_تعریف_شدہ FEAT_FOLDING
	    ورنہ اگر (flagtab[fidx].flags == HL_FOLD
						&& foldmethodIsSyntax(curwin))
		// Need to update folds later.
		foldUpdateAll(curwin);
#اگر_ختم
	}
    }

    واپس arg;
}

/*
 * Adjustments to syntax item when declared in a ":syn include"'d file.
 * Set the contained flag, and if the item is not already contained, add it
 * to the specified top-level group, if any.
 */
    ساکن خالی
syn_incl_toplevel(صحیح id, صحیح *flagsp)
{
    اگر ((*flagsp & HL_CONTAINED) || curwin->w_s->b_syn_topgrp == 0)
	واپس;
    *flagsp |= HL_CONTAINED | HL_INCLUDED_TOPLEVEL;
    اگر (curwin->w_s->b_syn_topgrp >= SYNID_CLUSTER)
    {
	// We have to alloc this, because syn_combine_list() will free it.
	مختصر	    *grp_list = ALLOC_MULT(مختصر, 2);
	صحیح	    tlg_id = curwin->w_s->b_syn_topgrp - SYNID_CLUSTER;

	اگر (grp_list != تہی)
	{
	    grp_list[0] = id;
	    grp_list[1] = 0;
	    syn_combine_list(&SYN_CLSTR(curwin->w_s)[tlg_id].scl_list,
						       &grp_list, CLUSTER_جوڑو);
	}
    }
}

/*
 * Handle ":syntax include [@{group-name}] filename" command.
 */
    ساکن خالی
syn_cmd_include(exarg_T *eap, صحیح syncing غیر_مستعمل)
{
    char_u	*arg = eap->arg;
    صحیح		sgl_id = 1;
    char_u	*گروہ_name_انتہا;
    char_u	*rest;
    حرف	*errormsg = تہی;
    صحیح		پچھلا_toplvl_grp;
    صحیح		پچھلا_syn_inc_لیبل;
    صحیح		ماخذ = FALSE;

    eap->nextcmd = ڈھونڈو_nextcmd(arg);
    اگر (eap->skip)
	واپس;

    اگر (arg[0] == '@')
    {
	++arg;
	rest = get_گروہ_name(arg, &گروہ_name_انتہا);
	اگر (rest == تہی)
	{
	    emsg(_(e_filename_required));
	    واپس;
	}
	sgl_id = syn_check_cluster(arg, (صحیح)(گروہ_name_انتہا - arg));
	اگر (sgl_id == 0)
	    واپس;
	// separate_nextcmd() and expand_filename() depend on this
	eap->arg = rest;
    }

    /*
     * Everything that's left, up to the next command, should be the
     * filename to include.
     */
    eap->argt |= (EX_XFILE | EX_NOSPC);
    separate_nextcmd(eap, FALSE);
    اگر (*eap->arg == '<' || *eap->arg == '$' || mch_is_بھرا_Name(eap->arg))
    {
	// For an absolute path, "$VIM/..." or "<sfile>.." we ":source" the
	// file.  Need to expand the file name first.  In other cases
	// ":runtime!" is used.
	ماخذ = TRUE;
	اگر (expand_filename(eap, syn_cmdlinep, &errormsg) == FAIL)
	{
	    اگر (errormsg != تہی)
		emsg(errormsg);
	    واپس;
	}
    }

    /*
     * Save and restore the existing top-level grouplist id and ":syn
     * include" tag around the actual inclusion.
     */
    اگر (running_syn_inc_لیبل >= MAX_SYN_INC_لیبل)
    {
	emsg(_(e_too_many_syntax_includes));
	واپس;
    }
    پچھلا_syn_inc_لیبل = current_syn_inc_لیبل;
    current_syn_inc_لیبل = ++running_syn_inc_لیبل;
    پچھلا_toplvl_grp = curwin->w_s->b_syn_topgrp;
    curwin->w_s->b_syn_topgrp = sgl_id;
    اگر (ماخذ ? do_ماخذ(eap->arg, FALSE, DOSO_NONE, تہی) == FAIL
				: ماخذ_runtime(eap->arg, DIP_ALL) == FAIL)
	semsg(_(e_cant_open_file_str), eap->arg);
    curwin->w_s->b_syn_topgrp = پچھلا_toplvl_grp;
    current_syn_inc_لیبل = پچھلا_syn_inc_لیبل;
}

/*
 * Handle ":syntax keyword {group-name} [{option}] keyword .." command.
 */
    ساکن خالی
syn_cmd_keyword(exarg_T *eap, صحیح syncing غیر_مستعمل)
{
    char_u	*arg = eap->arg;
    char_u	*گروہ_name_انتہا;
    صحیح		syn_id;
    char_u	*rest;
    char_u	*keyword_copy = تہی;
    char_u	*p;
    char_u	*kw;
    syn_opt_arg_T syn_opt_arg;
    صحیح		cnt;
    صحیح		conceal_char = NUL;

    rest = get_گروہ_name(arg, &گروہ_name_انتہا);

    اگر (rest != تہی)
    {
	اگر (eap->skip)
	    syn_id = -1;
	ورنہ
	    syn_id = syn_check_گروہ(arg, (صحیح)(گروہ_name_انتہا - arg));
	اگر (syn_id != 0)
	    // allocate a buffer, for removing backslashes in the keyword
	    keyword_copy = alloc(STRLEN(rest) + 1);
	اگر (keyword_copy != تہی)
	{
	    syn_opt_arg.flags = 0;
	    syn_opt_arg.keyword = TRUE;
	    syn_opt_arg.sync_idx = تہی;
	    syn_opt_arg.has_cont_list = FALSE;
	    syn_opt_arg.cont_اندر_list = تہی;
	    syn_opt_arg.اگلا_list = تہی;

	    /*
	     * The options given apply to ALL keywords, so all options must be
	     * found before keywords can be created.
	     * 1: collect the options and copy the keywords to keyword_copy.
	     */
	    cnt = 0;
	    p = keyword_copy;
	    برائے ( ; rest != تہی && !سرے_excmd2(eap->arg, rest);
							rest = skipwhite(rest))
	    {
		rest = get_syn_options(rest, &syn_opt_arg, &conceal_char,
								    eap->skip);
		اگر (rest == تہی || سرے_excmd2(eap->arg, rest))
		    توڑو;
		// Copy the keyword, removing backslashes, and add a NUL.
		جب (*rest != NUL && !VIM_ISWHITE(*rest))
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
		برائے (kw = keyword_copy; --cnt >= 0; kw += kwlen + 1)
		{
		    برائے (p = vim_strchr(kw, '['); ; )
		    {
			اگر (p == تہی)
			    kwlen = STRLEN(kw);
			ورنہ
			{
			    *p = NUL;
			    kwlen = (size_t)(p - kw);
			}
			جوڑو_keyword(kw, kwlen, syn_id, syn_opt_arg.flags,
				syn_opt_arg.cont_اندر_list,
					 syn_opt_arg.اگلا_list, conceal_char);
			اگر (p == تہی)
			    توڑو;
			اگر (p[1] == NUL)
			{
			    semsg(_(e_error_missing_rsb_str), kw);
			    جاؤ error;
			}
			اگر (p[1] == ']')
			{
			    اگر (p[2] != NUL)
			    {
				semsg(_(e_trailing_char_after_rsb_str_str),
								    kw, &p[2]);
				جاؤ error;
			    }
			    kw = p + 1;		// skip over the "]"
			    kwlen = 1;
			    توڑو;
			}
			اگر (has_mbyte)
			{
			    صحیح l = (*mb_ptr2len)(p + 1);

			    mch_memmove(p, p + 1, l);
			    p += l;
			}
			ورنہ
			{
			    p[0] = p[1];
			    ++p;
			}
		    }
		}
	    }
error:
	    vim_free(keyword_copy);
	    vim_free(syn_opt_arg.cont_اندر_list);
	    vim_free(syn_opt_arg.اگلا_list);
	}
    }

    اگر (rest != تہی)
	سیٹ_nextcmd(eap, rest);
    ورنہ
	semsg(_(e_invalid_دلیل_str), arg);

    redraw_curbuf_later(UPD_SOME_VALID);
    syn_ڈھیر_free_all(curwin->w_s);		// Need to recompute all syntax.
}

/*
 * Handle ":syntax match {name} [{options}] {pattern} [{options}]".
 *
 * Also ":syntax sync match {name} [[grouphere | groupthere] {group-name}] .."
 */
    ساکن خالی
syn_cmd_مطابقت(
    exarg_T	*eap,
    صحیح		syncing)	    // TRUE for ":syntax sync match .. "
{
    char_u	*arg = eap->arg;
    char_u	*گروہ_name_انتہا;
    char_u	*rest;
    synpat_T	شے;		// the item found in the line
    صحیح		syn_id;
    صحیح		idx;
    syn_opt_arg_T syn_opt_arg;
    صحیح		sync_idx = 0;
    صحیح		conceal_char = NUL;
    صحیح		orig_called_emsg = called_emsg;

    // Isolate the group name, check for validity
    rest = get_گروہ_name(arg, &گروہ_name_انتہا);

    // Get options before the pattern
    syn_opt_arg.flags = 0;
    syn_opt_arg.keyword = FALSE;
    syn_opt_arg.sync_idx = syncing ? &sync_idx : تہی;
    syn_opt_arg.has_cont_list = TRUE;
    syn_opt_arg.cont_list = تہی;
    syn_opt_arg.cont_اندر_list = تہی;
    syn_opt_arg.اگلا_list = تہی;
    rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);

    // get the pattern.
    init_syn_patterns();
    CLEAR_میدان(شے);
    rest = get_syn_نمونہ(rest, &شے);
    اگر (vim_regcomp_had_eol() && !(syn_opt_arg.flags & HL_EXCLUDENL))
	syn_opt_arg.flags |= HL_HAS_EOL;

    // Get options after the pattern
    rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);

    اگر (rest != تہی)		// all arguments are valid
    {
	/*
	 * Check for trailing command and illegal trailing arguments.
	 */
	سیٹ_nextcmd(eap, rest);
	اگر (!سرے_excmd2(eap->cmd, rest) || eap->skip)
	    rest = تہی;
	ورنہ اگر (ga_grow(&curwin->w_s->b_syn_patterns, 1) == OK
		&& (syn_id = syn_check_گروہ(arg,
					   (صحیح)(گروہ_name_انتہا - arg))) != 0)
	{
	    syn_incl_toplevel(syn_id, &syn_opt_arg.flags);
	    /*
	     * Store the pattern in the syn_items list
	     */
	    idx = curwin->w_s->b_syn_patterns.ga_len;
	    SYN_اشیاء(curwin->w_s)[idx] = شے;
	    SYN_اشیاء(curwin->w_s)[idx].sp_syncing = syncing;
	    SYN_اشیاء(curwin->w_s)[idx].sp_type = SPTYPE_مطابقت;
	    SYN_اشیاء(curwin->w_s)[idx].sp_syn.id = syn_id;
	    SYN_اشیاء(curwin->w_s)[idx].sp_syn.inc_لیبل = current_syn_inc_لیبل;
	    SYN_اشیاء(curwin->w_s)[idx].sp_flags = syn_opt_arg.flags;
	    SYN_اشیاء(curwin->w_s)[idx].sp_sync_idx = sync_idx;
	    SYN_اشیاء(curwin->w_s)[idx].sp_cont_list = syn_opt_arg.cont_list;
	    SYN_اشیاء(curwin->w_s)[idx].sp_syn.cont_اندر_list =
						     syn_opt_arg.cont_اندر_list;
#اگر_تعریف_شدہ FEAT_CONCEAL
	    SYN_اشیاء(curwin->w_s)[idx].sp_cchar = conceal_char;
#اگر_ختم
	    اگر (syn_opt_arg.cont_اندر_list != تہی)
		curwin->w_s->b_syn_containedin = TRUE;
	    SYN_اشیاء(curwin->w_s)[idx].sp_اگلا_list = syn_opt_arg.اگلا_list;
	    ++curwin->w_s->b_syn_patterns.ga_len;

	    // remember that we found a match for syncing on
	    اگر (syn_opt_arg.flags & (HL_SYNC_یہاں|HL_SYNC_THERE))
		curwin->w_s->b_syn_sync_flags |= SF_مطابقت;
#اگر_تعریف_شدہ FEAT_FOLDING
	    اگر (syn_opt_arg.flags & HL_FOLD)
		++curwin->w_s->b_syn_folditems;
#اگر_ختم

	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_ڈھیر_free_all(curwin->w_s);	// Need to recompute all syntax.
	    واپس;	// don't free the progs and patterns now
	}
    }

    /*
     * Something failed, free the allocated memory.
     */
    vim_regfree(شے.sp_prog);
    vim_free(شے.sp_نمونہ);
    vim_free(syn_opt_arg.cont_list);
    vim_free(syn_opt_arg.cont_اندر_list);
    vim_free(syn_opt_arg.اگلا_list);

    اگر (rest == تہی && called_emsg == orig_called_emsg)
	semsg(_(e_invalid_دلیل_str), arg);
}

/*
 * Handle ":syntax region {group-name} [matchgroup={group-name}]
 *		start {start} .. [skip {skip}] end {end} .. [{options}]".
 */
    ساکن خالی
syn_cmd_region(
    exarg_T	*eap,
    صحیح		syncing)	    // TRUE for ":syntax sync region .."
{
    char_u		*arg = eap->arg;
    char_u		*گروہ_name_انتہا;
    char_u		*rest;			// next arg, NULL on error
    char_u		*کلید_انتہا;
    char_u		*کلید = تہی;
    char_u		*p;
    صحیح			شے;
#تعریف_کرو شے_آغاز_2	    0
#تعریف_کرو شے_SKIP	    1
#تعریف_کرو شے_انتہا_2	    2
#تعریف_کرو شے_MATCHGROUP	    3
    ساخت pat_ptr
    {
	synpat_T	*pp_synp;		// pointer to syn_pattern
	صحیح		pp_matchgroup_id;	// matchgroup ID
	ساخت pat_ptr	*pp_اگلا;		// pointer to next pat_ptr
    }			*(pat_ptrs[3]);
					// patterns found in the line
    ساخت pat_ptr	*ppp;
    ساخت pat_ptr	*ppp_اگلا;
    صحیح			pat_گنتی = 0;		// nr of syn_patterns found
    صحیح			syn_id;
    صحیح			matchgroup_id = 0;
    صحیح			not_enough = FALSE;	// not enough arguments
    صحیح			illegal = FALSE;	// illegal arguments
    صحیح			success = FALSE;
    صحیح			idx;
    syn_opt_arg_T	syn_opt_arg;
    صحیح			conceal_char = NUL;

    // Isolate the group name, check for validity
    rest = get_گروہ_name(arg, &گروہ_name_انتہا);

    pat_ptrs[0] = تہی;
    pat_ptrs[1] = تہی;
    pat_ptrs[2] = تہی;

    init_syn_patterns();

    syn_opt_arg.flags = 0;
    syn_opt_arg.keyword = FALSE;
    syn_opt_arg.sync_idx = تہی;
    syn_opt_arg.has_cont_list = TRUE;
    syn_opt_arg.cont_list = تہی;
    syn_opt_arg.cont_اندر_list = تہی;
    syn_opt_arg.اگلا_list = تہی;

    /*
     * get the options, patterns and matchgroup.
     */
    جب (rest != تہی && !سرے_excmd2(eap->cmd, rest))
    {
	// Check for option arguments
	rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);
	اگر (rest == تہی || سرے_excmd2(eap->cmd, rest))
	    توڑو;

	// must be a pattern or matchgroup then
	کلید_انتہا = rest;
	جب (*کلید_انتہا && !VIM_ISWHITE(*کلید_انتہا) && *کلید_انتہا != '=')
	    ++کلید_انتہا;
	vim_free(کلید);
	کلید = vim_strnsave_اوپر(rest, کلید_انتہا - rest);
	اگر (کلید == تہی)			// out of memory
	{
	    rest = تہی;
	    توڑو;
	}
	اگر (STRCMP(کلید, "MATCHGROUP") == 0)
	    شے = شے_MATCHGROUP;
	ورنہ اگر (STRCMP(کلید, "START") == 0)
	    شے = شے_آغاز_2;
	ورنہ اگر (STRCMP(کلید, "END") == 0)
	    شے = شے_انتہا_2;
	ورنہ اگر (STRCMP(کلید, "SKIP") == 0)
	{
	    اگر (pat_ptrs[شے_SKIP] != تہی)	// one skip pattern allowed
	    {
		illegal = TRUE;
		توڑو;
	    }
	    شے = شے_SKIP;
	}
	ورنہ
	    توڑو;
	rest = skipwhite(کلید_انتہا);
	اگر (*rest != '=')
	{
	    rest = تہی;
	    semsg(_(e_missing_equal_str), arg);
	    توڑو;
	}
	rest = skipwhite(rest + 1);
	اگر (*rest == NUL)
	{
	    not_enough = TRUE;
	    توڑو;
	}

	اگر (شے == شے_MATCHGROUP)
	{
	    p = skiptowhite(rest);
	    اگر ((p - rest == 4 && STRNCMP(rest, "NONE", 4) == 0) || eap->skip)
		matchgroup_id = 0;
	    ورنہ
	    {
		matchgroup_id = syn_check_گروہ(rest, (صحیح)(p - rest));
		اگر (matchgroup_id == 0)
		{
		    illegal = TRUE;
		    توڑو;
		}
	    }
	    rest = skipwhite(p);
	}
	ورنہ
	{
	    /*
	     * Allocate room for a syn_pattern, and link it in the list of
	     * syn_patterns for this item, at the start (because the list is
	     * used from end to start).
	     */
	    ppp = ALLOC_ONE(ساخت pat_ptr);
	    اگر (ppp == تہی)
	    {
		rest = تہی;
		توڑو;
	    }
	    ppp->pp_اگلا = pat_ptrs[شے];
	    pat_ptrs[شے] = ppp;
	    ppp->pp_synp = ALLOC_CLEAR_ONE(synpat_T);
	    اگر (ppp->pp_synp == تہی)
	    {
		rest = تہی;
		توڑو;
	    }

	    /*
	     * Get the syntax pattern and the following offset(s).
	     */
	    // Enable the appropriate \z specials.
	    اگر (شے == شے_آغاز_2)
		reg_do_extmatch = REX_سیٹ;
	    ورنہ اگر (شے == شے_SKIP || شے == شے_انتہا_2)
		reg_do_extmatch = REX_USE;
	    rest = get_syn_نمونہ(rest, ppp->pp_synp);
	    reg_do_extmatch = 0;
	    اگر (شے == شے_انتہا_2 && vim_regcomp_had_eol()
				       && !(syn_opt_arg.flags & HL_EXCLUDENL))
		ppp->pp_synp->sp_flags |= HL_HAS_EOL;
	    ppp->pp_matchgroup_id = matchgroup_id;
	    ++pat_گنتی;
	}
    }
    vim_free(کلید);
    اگر (illegal || not_enough)
	rest = تہی;

    /*
     * Must have a "start" and "end" pattern.
     */
    اگر (rest != تہی && (pat_ptrs[شے_آغاز_2] == تہی ||
						  pat_ptrs[شے_انتہا_2] == تہی))
    {
	not_enough = TRUE;
	rest = تہی;
    }

    اگر (rest != تہی)
    {
	/*
	 * Check for trailing garbage or command.
	 * If OK, add the item.
	 */
	سیٹ_nextcmd(eap, rest);
	اگر (!سرے_excmd(*rest) || eap->skip)
	    rest = تہی;
	ورنہ اگر (ga_grow(&(curwin->w_s->b_syn_patterns), pat_گنتی) == OK
		&& (syn_id = syn_check_گروہ(arg,
					   (صحیح)(گروہ_name_انتہا - arg))) != 0)
	{
	    syn_incl_toplevel(syn_id, &syn_opt_arg.flags);
	    /*
	     * Store the start/skip/end in the syn_items list
	     */
	    idx = curwin->w_s->b_syn_patterns.ga_len;
	    برائے (شے = شے_آغاز_2; شے <= شے_انتہا_2; ++شے)
	    {
		برائے (ppp = pat_ptrs[شے]; ppp != تہی; ppp = ppp->pp_اگلا)
		{
		    SYN_اشیاء(curwin->w_s)[idx] = *(ppp->pp_synp);
		    SYN_اشیاء(curwin->w_s)[idx].sp_syncing = syncing;
		    SYN_اشیاء(curwin->w_s)[idx].sp_type =
			    (شے == شے_آغاز_2) ? SPTYPE_آغاز :
			    (شے == شے_SKIP) ? SPTYPE_SKIP : SPTYPE_انتہا;
		    SYN_اشیاء(curwin->w_s)[idx].sp_flags |= syn_opt_arg.flags;
		    SYN_اشیاء(curwin->w_s)[idx].sp_syn.id = syn_id;
		    SYN_اشیاء(curwin->w_s)[idx].sp_syn.inc_لیبل =
							  current_syn_inc_لیبل;
		    SYN_اشیاء(curwin->w_s)[idx].sp_syn_مطابقت_id =
							ppp->pp_matchgroup_id;
#اگر_تعریف_شدہ FEAT_CONCEAL
		    SYN_اشیاء(curwin->w_s)[idx].sp_cchar = conceal_char;
#اگر_ختم
		    اگر (شے == شے_آغاز_2)
		    {
			SYN_اشیاء(curwin->w_s)[idx].sp_cont_list =
							syn_opt_arg.cont_list;
			SYN_اشیاء(curwin->w_s)[idx].sp_syn.cont_اندر_list =
						     syn_opt_arg.cont_اندر_list;
			اگر (syn_opt_arg.cont_اندر_list != تہی)
			    curwin->w_s->b_syn_containedin = TRUE;
			SYN_اشیاء(curwin->w_s)[idx].sp_اگلا_list =
							syn_opt_arg.اگلا_list;
		    }
		    ++curwin->w_s->b_syn_patterns.ga_len;
		    ++idx;
#اگر_تعریف_شدہ FEAT_FOLDING
		    اگر (syn_opt_arg.flags & HL_FOLD)
			++curwin->w_s->b_syn_folditems;
#اگر_ختم
		}
	    }

	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_ڈھیر_free_all(curwin->w_s);	// Need to recompute all syntax.
	    success = TRUE;	    // don't free the progs and patterns now
	}
    }

    /*
     * Free the allocated memory.
     */
    برائے (شے = شے_آغاز_2; شے <= شے_انتہا_2; ++شے)
	برائے (ppp = pat_ptrs[شے]; ppp != تہی; ppp = ppp_اگلا)
	{
	    اگر (!success && ppp->pp_synp != تہی)
	    {
		vim_regfree(ppp->pp_synp->sp_prog);
		vim_free(ppp->pp_synp->sp_نمونہ);
	    }
	    vim_free(ppp->pp_synp);
	    ppp_اگلا = ppp->pp_اگلا;
	    vim_free(ppp);
	}

    اگر (!success)
    {
	vim_free(syn_opt_arg.cont_list);
	vim_free(syn_opt_arg.cont_اندر_list);
	vim_free(syn_opt_arg.اگلا_list);
	اگر (not_enough)
	    semsg(_(e_not_enough_arguments_syntax_region_str), arg);
	ورنہ اگر (illegal || rest == تہی)
	    semsg(_(e_invalid_دلیل_str), arg);
    }
}

/*
 * A simple syntax group ID comparison function suitable for use in qsort()
 */
    ساکن صحیح
syn_compare_stub(ثابت خالی *v1, ثابت خالی *v2)
{
    ثابت مختصر	*s1 = v1;
    ثابت مختصر	*s2 = v2;

    واپس (*s1 > *s2 ? 1 : *s1 < *s2 ? -1 : 0);
}

/*
 * Combines lists of syntax clusters.
 * *clstr1 and *clstr2 must both be allocated memory; they will be consumed.
 */
    ساکن خالی
syn_combine_list(مختصر **clstr1, مختصر **clstr2, صحیح list_op)
{
    صحیح		count1 = 0;
    صحیح		count2 = 0;
    مختصر	*g1;
    مختصر	*g2;
    مختصر	*clstr = تہی;
    صحیح		گنتی;
    صحیح		باری;

    /*
     * Handle degenerate cases.
     */
    اگر (*clstr2 == تہی)
	واپس;
    اگر (*clstr1 == تہی || list_op == CLUSTER_REPLACE)
    {
	اگر (list_op == CLUSTER_REPLACE)
	    vim_free(*clstr1);
	اگر (list_op == CLUSTER_REPLACE || list_op == CLUSTER_جوڑو)
	    *clstr1 = *clstr2;
	ورنہ
	    vim_free(*clstr2);
	واپس;
    }

    برائے (g1 = *clstr1; *g1; g1++)
	++count1;
    برائے (g2 = *clstr2; *g2; g2++)
	++count2;

    /*
     * For speed purposes, sort both lists.
     */
    qsort(*clstr1, (size_t)count1, حجم(مختصر), syn_compare_stub);
    qsort(*clstr2, (size_t)count2, حجم(مختصر), syn_compare_stub);

    /*
     * We proceed in two passes; in round 1, we count the elements to place
     * in the new list, and in round 2, we allocate and populate the new
     * list.  For speed, we use a mergesort-like method, adding the smaller
     * of the current elements in each list to the new list.
     */
    برائے (باری = 1; باری <= 2; باری++)
    {
	g1 = *clstr1;
	g2 = *clstr2;
	گنتی = 0;

	/*
	 * First, loop through the lists until one of them is empty.
	 */
	جب (*g1 && *g2)
	{
	    /*
	     * We always want to add from the first list.
	     */
	    اگر (*g1 < *g2)
	    {
		اگر (باری == 2)
		    clstr[گنتی] = *g1;
		گنتی++;
		g1++;
		جاری;
	    }
	    /*
	     * We only want to add from the second list if we're adding the
	     * lists.
	     */
	    اگر (list_op == CLUSTER_جوڑو)
	    {
		اگر (باری == 2)
		    clstr[گنتی] = *g2;
		گنتی++;
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
	برائے (; *g1; g1++, گنتی++)
	    اگر (باری == 2)
		clstr[گنتی] = *g1;
	اگر (list_op == CLUSTER_جوڑو)
	    برائے (; *g2; g2++, گنتی++)
		اگر (باری == 2)
		    clstr[گنتی] = *g2;

	اگر (باری == 1)
	{
	    /*
	     * If the group ended up empty, we don't need to allocate any
	     * space for it.
	     */
	    اگر (گنتی == 0)
	    {
		clstr = تہی;
		توڑو;
	    }
	    clstr = ALLOC_MULT(مختصر, گنتی + 1);
	    اگر (clstr == تہی)
		توڑو;
	    clstr[گنتی] = 0;
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
    ساکن صحیح
syn_scl_name2id(char_u *name)
{
    صحیح		i;
    char_u	*name_u;

    // Avoid using stricmp() too much, it's slow on some systems
    name_u = vim_strsave_اوپر(name);
    اگر (name_u == تہی)
	واپس 0;
    برائے (i = curwin->w_s->b_syn_clusters.ga_len; --i >= 0; )
	اگر (SYN_CLSTR(curwin->w_s)[i].scl_name_u != تہی
		&& STRCMP(name_u, SYN_CLSTR(curwin->w_s)[i].scl_name_u) == 0)
	    توڑو;
    vim_free(name_u);
    واپس (i < 0 ? 0 : i + SYNID_CLUSTER);
}

/*
 * Like syn_scl_name2id(), but take a pointer + length argument.
 */
    ساکن صحیح
syn_scl_namen2id(char_u *linep, صحیح len)
{
    char_u  *name;
    صحیح	    id = 0;

    name = vim_strnsave(linep, len);
    اگر (name == تہی)
	واپس 0;

    id = syn_scl_name2id(name);
    vim_free(name);
    واپس id;
}

/*
 * Find syntax cluster name in the table and return its ID.
 * The argument is a pointer to the name and the length of the name.
 * If it doesn't exist yet, a new entry is created.
 * Return 0 for failure.
 */
    ساکن صحیح
syn_check_cluster(char_u *pp, صحیح len)
{
    صحیح		id;
    char_u	*name;

    name = vim_strnsave(pp, len);
    اگر (name == تہی)
	واپس 0;

    id = syn_scl_name2id(name);
    اگر (id == 0)			// doesn't exist yet
	id = syn_جوڑو_cluster(name);
    ورنہ
	vim_free(name);
    واپس id;
}

/*
 * Add new syntax cluster and return its ID.
 * "name" must be an allocated string, it will be consumed.
 * Return 0 for failure.
 */
    ساکن صحیح
syn_جوڑو_cluster(char_u *name)
{
    صحیح		len;

    /*
     * First call for this growarray: init growing array.
     */
    اگر (curwin->w_s->b_syn_clusters.ga_ڈیٹا == تہی)
    {
	curwin->w_s->b_syn_clusters.ga_itemsize = حجم(syn_cluster_T);
	curwin->w_s->b_syn_clusters.ga_growsize = 10;
    }

    len = curwin->w_s->b_syn_clusters.ga_len;
    اگر (len >= MAX_CLUSTER_ID)
    {
	emsg(_(e_too_many_syntax_clusters));
	vim_free(name);
	واپس 0;
    }

    /*
     * Make room for at least one other cluster entry.
     */
    اگر (ga_grow(&curwin->w_s->b_syn_clusters, 1) == FAIL)
    {
	vim_free(name);
	واپس 0;
    }

    CLEAR_اشارہ_گر(&(SYN_CLSTR(curwin->w_s)[len]));
    SYN_CLSTR(curwin->w_s)[len].scl_name = name;
    SYN_CLSTR(curwin->w_s)[len].scl_name_u = vim_strsave_اوپر(name);
    SYN_CLSTR(curwin->w_s)[len].scl_list = تہی;
    ++curwin->w_s->b_syn_clusters.ga_len;

    اگر (STRICMP(name, "Spell") == 0)
	curwin->w_s->b_spell_cluster_id = len + SYNID_CLUSTER;
    اگر (STRICMP(name, "NoSpell") == 0)
	curwin->w_s->b_nospell_cluster_id = len + SYNID_CLUSTER;

    واپس len + SYNID_CLUSTER;
}

/*
 * Handle ":syntax cluster {cluster-name} [contains={groupname},..]
 *		[add={groupname},..] [remove={groupname},..]".
 */
    ساکن خالی
syn_cmd_cluster(exarg_T *eap, صحیح syncing غیر_مستعمل)
{
    char_u	*arg = eap->arg;
    char_u	*گروہ_name_انتہا;
    char_u	*rest;
    صحیح		scl_id;
    مختصر	*clstr_list;
    صحیح		got_clstr = FALSE;
    صحیح		opt_len;
    صحیح		list_op;

    eap->nextcmd = ڈھونڈو_nextcmd(arg);
    اگر (eap->skip)
	واپس;

    rest = get_گروہ_name(arg, &گروہ_name_انتہا);

    اگر (rest != تہی)
    {
	scl_id = syn_check_cluster(arg, (صحیح)(گروہ_name_انتہا - arg));
	اگر (scl_id == 0)
	    واپس;
	scl_id -= SYNID_CLUSTER;

	برائے (;;)
	{
	    اگر (STRNICMP(rest, "add", 3) == 0
		    && (VIM_ISWHITE(rest[3]) || rest[3] == '='))
	    {
		opt_len = 3;
		list_op = CLUSTER_جوڑو;
	    }
	    ورنہ اگر (STRNICMP(rest, "remove", 6) == 0
		    && (VIM_ISWHITE(rest[6]) || rest[6] == '='))
	    {
		opt_len = 6;
		list_op = CLUSTER_تفریق;
	    }
	    ورنہ اگر (STRNICMP(rest, "contains", 8) == 0
			&& (VIM_ISWHITE(rest[8]) || rest[8] == '='))
	    {
		opt_len = 8;
		list_op = CLUSTER_REPLACE;
	    }
	    ورنہ
		توڑو;

	    clstr_list = تہی;
	    اگر (get_id_list(&rest, opt_len, &clstr_list, eap->skip) == FAIL)
	    {
		semsg(_(e_invalid_دلیل_str), rest);
		توڑو;
	    }
	    اگر (scl_id >= 0)
		syn_combine_list(&SYN_CLSTR(curwin->w_s)[scl_id].scl_list,
			     &clstr_list, list_op);
	    ورنہ
		vim_free(clstr_list);
	    got_clstr = TRUE;
	}

	اگر (got_clstr)
	{
	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_ڈھیر_free_all(curwin->w_s);	// Need to recompute all.
	}
    }

    اگر (!got_clstr)
	emsg(_(e_no_cluster_specified));
    اگر (rest == تہی || !سرے_excmd2(eap->cmd, rest))
	semsg(_(e_invalid_دلیل_str), arg);
}

/*
 * On first call for current buffer: Init growing array.
 */
    ساکن خالی
init_syn_patterns(خالی)
{
    curwin->w_s->b_syn_patterns.ga_itemsize = حجم(synpat_T);
    curwin->w_s->b_syn_patterns.ga_growsize = 10;
}

/*
 * Get one pattern for a ":syntax match" or ":syntax region" command.
 * Stores the pattern and program in a synpat_T.
 * Returns a pointer to the next argument, or NULL in case of an error.
 */
    ساکن char_u *
get_syn_نمونہ(char_u *arg, synpat_T *ci)
{
    char_u	*انتہا;
    صحیح		*p;
    صحیح		idx;
    char_u	*cpo_save;

    // need at least three chars
    اگر (arg == تہی || arg[0] == NUL || arg[1] == NUL || arg[2] == NUL)
	واپس تہی;

    انتہا = skip_regexp(arg + 1, *arg, TRUE);
    اگر (*انتہا != *arg)			    // end delimiter not found
    {
	semsg(_(e_نمونہ_delimiter_not_ملا_str), arg);
	واپس تہی;
    }
    // store the pattern and compiled regexp program
    اگر ((ci->sp_نمونہ = vim_strnsave(arg + 1, انتہا - arg - 1)) == تہی)
	واپس تہی;

    // Make 'cpoptions' empty, to avoid the 'l' flag
    cpo_save = p_cpo;
    p_cpo = خالی_خانے_option;
    ci->sp_prog = vim_regcomp(ci->sp_نمونہ, RE_MAGIC);
    p_cpo = cpo_save;

    اگر (ci->sp_prog == تہی)
	واپس تہی;
    ci->sp_ic = curwin->w_s->b_syn_ic;
#اگر_تعریف_شدہ FEAT_PROFILE
    syn_clear_وقت(&ci->sp_وقت);
#اگر_ختم

    /*
     * Check for a match, highlight or region offset.
     */
    ++انتہا;
    کرو
    {
	برائے (idx = SPO_گنتی; --idx >= 0; )
	    اگر (STRNCMP(انتہا, spo_name_tab[idx], 3) == 0)
		توڑو;
	اگر (idx >= 0)
	{
	    p = &(ci->sp_offsets[idx]);
	    اگر (idx != SPO_LC_OFF)
		انتخاب (انتہا[3])
		{
		    حالت 's':   توڑو;
		    حالت 'b':   توڑو;
		    حالت 'e':   idx += SPO_گنتی; توڑو;
		    طےشدہ:    idx = -1; توڑو;
		}
	    اگر (idx >= 0)
	    {
		ci->sp_off_flags |= (1 << idx);
		اگر (idx == SPO_LC_OFF)	    // lc=99
		{
		    انتہا += 3;
		    *p = getdigits(&انتہا);

		    // "lc=" offset automatically sets "ms=" offset
		    اگر (!(ci->sp_off_flags & (1 << SPO_MS_OFF)))
		    {
			ci->sp_off_flags |= (1 << SPO_MS_OFF);
			ci->sp_offsets[SPO_MS_OFF] = *p;
		    }
		}
		ورنہ			    // yy=x+99
		{
		    انتہا += 4;
		    اگر (*انتہا == '+')
		    {
			++انتہا;
			*p = getdigits(&انتہا);		// positive offset
		    }
		    ورنہ اگر (*انتہا == '-')
		    {
			++انتہا;
			*p = -getdigits(&انتہا);		// negative offset
		    }
		}
		اگر (*انتہا != ',')
		    توڑو;
		++انتہا;
	    }
	}
    } جب (idx >= 0);

    اگر (!سرے_excmd2(arg, انتہا) && !VIM_ISWHITE(*انتہا))
    {
	semsg(_(e_garbage_after_نمونہ_str), arg);
	واپس تہی;
    }
    واپس skipwhite(انتہا);
}

/*
 * Handle ":syntax sync .." command.
 */
    ساکن خالی
syn_cmd_sync(exarg_T *eap, صحیح syncing غیر_مستعمل)
{
    char_u	*arg_آغاز = eap->arg;
    char_u	*arg_انتہا;
    char_u	*کلید = تہی;
    char_u	*اگلا_arg;
    صحیح		illegal = FALSE;
    صحیح		ختم_شدہ = FALSE;
    طویل	n;
    char_u	*cpo_save;

    اگر (سرے_excmd2(eap->cmd, arg_آغاز))
    {
	syn_cmd_list(eap, TRUE);
	واپس;
    }

    جب (!سرے_excmd2(eap->cmd, arg_آغاز))
    {
	arg_انتہا = skiptowhite(arg_آغاز);
	اگلا_arg = skipwhite(arg_انتہا);
	vim_free(کلید);
	کلید = vim_strnsave_اوپر(arg_آغاز, arg_انتہا - arg_آغاز);
	اگر (کلید == تہی)
	    توڑو;
	اگر (STRCMP(کلید, "CCOMMENT") == 0)
	{
	    اگر (!eap->skip)
		curwin->w_s->b_syn_sync_flags |= SF_CCOMMENT;
	    اگر (!سرے_excmd2(eap->cmd, اگلا_arg))
	    {
		arg_انتہا = skiptowhite(اگلا_arg);
		اگر (!eap->skip)
		    curwin->w_s->b_syn_sync_id = syn_check_گروہ(اگلا_arg,
						   (صحیح)(arg_انتہا - اگلا_arg));
		اگلا_arg = skipwhite(arg_انتہا);
	    }
	    ورنہ اگر (!eap->skip)
		curwin->w_s->b_syn_sync_id = syn_name2id((char_u *)"Comment");
	}
	ورنہ اگر (  STRNCMP(کلید, "LINES", 5) == 0
		|| STRNCMP(کلید, "MINLINES", 8) == 0
		|| STRNCMP(کلید, "MAXLINES", 8) == 0
		|| STRNCMP(کلید, "LINEBREAKS", 10) == 0)
	{
	    اگر (کلید[4] == 'S')
		arg_انتہا = کلید + 6;
	    ورنہ اگر (کلید[0] == 'L')
		arg_انتہا = کلید + 11;
	    ورنہ
		arg_انتہا = کلید + 9;
	    اگر (arg_انتہا[-1] != '=' || !VIM_ISDIGIT(*arg_انتہا))
	    {
		illegal = TRUE;
		توڑو;
	    }
	    n = getdigits(&arg_انتہا);
	    اگر (!eap->skip)
	    {
		اگر (کلید[4] == 'B')
		    curwin->w_s->b_syn_sync_linebreaks = n;
		ورنہ اگر (کلید[1] == 'A')
		    curwin->w_s->b_syn_sync_maxlines = n;
		ورنہ
		    curwin->w_s->b_syn_sync_minlines = n;
	    }
	}
	ورنہ اگر (STRCMP(کلید, "FROMSTART") == 0)
	{
	    اگر (!eap->skip)
	    {
		curwin->w_s->b_syn_sync_minlines = MAXLNUM;
		curwin->w_s->b_syn_sync_maxlines = 0;
	    }
	}
	ورنہ اگر (STRCMP(کلید, "LINECONT") == 0)
	{
	    اگر (*اگلا_arg == NUL)	   // missing pattern
	    {
		illegal = TRUE;
		توڑو;
	    }
	    اگر (curwin->w_s->b_syn_linecont_pat != تہی)
	    {
		emsg(_(e_syntax_sync_سطر_continuations_نمونہ_specified_twice));
		ختم_شدہ = TRUE;
		توڑو;
	    }
	    arg_انتہا = skip_regexp(اگلا_arg + 1, *اگلا_arg, TRUE);
	    اگر (*arg_انتہا != *اگلا_arg)	    // end delimiter not found
	    {
		illegal = TRUE;
		توڑو;
	    }

	    اگر (!eap->skip)
	    {
		// store the pattern and compiled regexp program
		اگر ((curwin->w_s->b_syn_linecont_pat =
			    vim_strnsave(اگلا_arg + 1,
				      arg_انتہا - اگلا_arg - 1)) == تہی)
		{
		    ختم_شدہ = TRUE;
		    توڑو;
		}
		curwin->w_s->b_syn_linecont_ic = curwin->w_s->b_syn_ic;

		// Make 'cpoptions' empty, to avoid the 'l' flag
		cpo_save = p_cpo;
		p_cpo = خالی_خانے_option;
		curwin->w_s->b_syn_linecont_prog =
		       vim_regcomp(curwin->w_s->b_syn_linecont_pat, RE_MAGIC);
		p_cpo = cpo_save;
#اگر_تعریف_شدہ FEAT_PROFILE
		syn_clear_وقت(&curwin->w_s->b_syn_linecont_وقت);
#اگر_ختم

		اگر (curwin->w_s->b_syn_linecont_prog == تہی)
		{
		    VIM_CLEAR(curwin->w_s->b_syn_linecont_pat);
		    ختم_شدہ = TRUE;
		    توڑو;
		}
	    }
	    اگلا_arg = skipwhite(arg_انتہا + 1);
	}
	ورنہ
	{
	    eap->arg = اگلا_arg;
	    اگر (STRCMP(کلید, "MATCH") == 0)
		syn_cmd_مطابقت(eap, TRUE);
	    ورنہ اگر (STRCMP(کلید, "REGION") == 0)
		syn_cmd_region(eap, TRUE);
	    ورنہ اگر (STRCMP(کلید, "CLEAR") == 0)
		syn_cmd_clear(eap, TRUE);
	    ورنہ
		illegal = TRUE;
	    ختم_شدہ = TRUE;
	    توڑو;
	}
	arg_آغاز = اگلا_arg;
    }
    vim_free(کلید);
    اگر (illegal)
	semsg(_(e_illegal_arguments_str), arg_آغاز);
    ورنہ اگر (!ختم_شدہ)
    {
	سیٹ_nextcmd(eap, arg_آغاز);
	redraw_curbuf_later(UPD_SOME_VALID);
	syn_ڈھیر_free_all(curwin->w_s);	// Need to recompute all syntax.
    }
}

/*
 * Convert a line of highlight group names into a list of group ID numbers.
 * "arg" should point to the "contains" or "nextgroup" keyword.
 * "arg" is advanced to after the last group name.
 * Careful: the argument is modified (NULs added).
 * returns FAIL for some error, OK for success.
 */
    ساکن صحیح
get_id_list(
    char_u	**arg,
    صحیح		keylen,		// length of keyword
    مختصر	**list,		// where to store the resulting list, if not
				// NULL, the list is silently skipped!
    صحیح		skip)
{
    char_u	*p = تہی;
    char_u	*انتہا;
    صحیح		باری;
    صحیح		گنتی;
    صحیح		کل_گنتی = 0;
    مختصر	*retval = تہی;
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
    برائے (باری = 1; باری <= 2; ++باری)
    {
	/*
	 * skip "contains"
	 */
	p = skipwhite(*arg + keylen);
	اگر (*p != '=')
	{
	    semsg(_(e_missing_equal_sign_str), *arg);
	    توڑو;
	}
	p = skipwhite(p + 1);
	اگر (سرے_excmd2(*arg, p))
	{
	    semsg(_(e_خالی_خانے_دلیل_str), *arg);
	    توڑو;
	}

	/*
	 * parse the arguments after "contains"
	 */
	گنتی = 0;
	جب (!سرے_excmd2(*arg, p))
	{
	    برائے (انتہا = p; *انتہا && !VIM_ISWHITE(*انتہا) && *انتہا != ','; ++انتہا)
		;
	    name = alloc(انتہا - p + 3);	    // leave room for "^$"
	    اگر (name == تہی)
	    {
		failed = TRUE;
		توڑو;
	    }
	    vim_strncpy(name + 1, p, انتہا - p);
	    اگر (       STRCMP(name + 1, "ALLBUT") == 0
		    || STRCMP(name + 1, "ALL") == 0
		    || STRCMP(name + 1, "TOP") == 0
		    || STRCMP(name + 1, "CONTAINED") == 0)
	    {
		اگر (TOUPPER_ASC(**arg) != 'C')
		{
		    semsg(_(e_str_not_allowed_یہاں), name + 1);
		    failed = TRUE;
		    vim_free(name);
		    توڑو;
		}
		اگر (گنتی != 0)
		{
		    semsg(_(e_str_must_be_پہلا_اندر_شامل_ہے_list), name + 1);
		    failed = TRUE;
		    vim_free(name);
		    توڑو;
		}
		اگر (name[1] == 'A')
		    id = SYNID_ALLBUT;
		ورنہ اگر (name[1] == 'T')
		    id = SYNID_چوٹی;
		ورنہ
		    id = SYNID_CONTAINED;
		id += current_syn_inc_لیبل;
	    }
	    ورنہ اگر (name[1] == '@')
	    {
		اگر (skip)
		    id = -1;
		ورنہ
		    id = syn_check_cluster(name + 2, (صحیح)(انتہا - p - 1));
	    }
	    ورنہ
	    {
		/*
		 * Handle full group name.
		 */
		اگر (vim_strpbrk(name + 1, (char_u *)"\\.*^$~[") == تہی)
		    id = syn_check_گروہ(name + 1, (صحیح)(انتہا - p));
		ورنہ
		{
		    /*
		     * Handle match of regexp with group names.
		     */
		    *name = '^';
		    STRCAT(name, "$");
		    regmatch.regprog = vim_regcomp(name, RE_MAGIC);
		    اگر (regmatch.regprog == تہی)
		    {
			failed = TRUE;
			vim_free(name);
			توڑو;
		    }

		    regmatch.rm_ic = TRUE;
		    id = 0;
		    برائے (i = highlight_عدد_groups(); --i >= 0; )
		    {
			اگر (vim_regexec(&regmatch, highlight_گروہ_name(i),
								  (colnr_T)0))
			{
			    اگر (باری == 2)
			    {
				// Got more items than expected; can happen
				// when adding items that match:
				// "contains=a.*b,axb".
				// Go back to first round
				اگر (گنتی >= کل_گنتی)
				{
				    vim_free(retval);
				    باری = 1;
				}
				ورنہ
				    retval[گنتی] = i + 1;
			    }
			    ++گنتی;
			    id = -1;	    // remember that we found one
			}
		    }
		    vim_regfree(regmatch.regprog);
		}
	    }
	    vim_free(name);
	    اگر (id == 0)
	    {
		semsg(_(e_unknown_گروہ_name_str), p);
		failed = TRUE;
		توڑو;
	    }
	    اگر (id > 0)
	    {
		اگر (باری == 2)
		{
		    // Got more items than expected, go back to first round
		    اگر (گنتی >= کل_گنتی)
		    {
			vim_free(retval);
			باری = 1;
		    }
		    ورنہ
			retval[گنتی] = id;
		}
		++گنتی;
	    }
	    p = skipwhite(انتہا);
	    اگر (*p != ',')
		توڑو;
	    p = skipwhite(p + 1);	// skip comma in between arguments
	}
	اگر (failed)
	    توڑو;
	اگر (باری == 1)
	{
	    retval = ALLOC_MULT(مختصر, گنتی + 1);
	    اگر (retval == تہی)
		توڑو;
	    retval[گنتی] = 0;	    // zero means end of the list
	    کل_گنتی = گنتی;
	}
    }

    *arg = p;
    اگر (failed || retval == تہی)
    {
	vim_free(retval);
	واپس FAIL;
    }

    اگر (*list == تہی)
	*list = retval;
    ورنہ
	vim_free(retval);	// list already found, don't overwrite it

    واپس OK;
}

/*
 * Make a copy of an ID list.
 */
    ساکن مختصر *
copy_id_list(مختصر *list)
{
    صحیح	    len;
    صحیح	    گنتی;
    مختصر   *retval;

    اگر (list == تہی)
	واپس تہی;

    برائے (گنتی = 0; list[گنتی]; ++گنتی)
	;
    len = (گنتی + 1) * حجم(مختصر);
    retval = alloc(len);
    اگر (retval != تہی)
	mch_memmove(retval, list, (size_t)len);

    واپس retval;
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
#تعریف_کرو IDL_CACHE_سائز 16

قسمتعریف ساخت
{
    مختصر	*idl_کلید;	// the list this entry resolves (NULL when empty)
    مختصر	*idl_ids;	// sorted, cluster-expanded group IDs (allocated)
    صحیح		idl_گنتی;	// number of IDs in idl_ids
    مختصر	idl_marker;	// leading ALLBUT/TOP/CONTAINED item, or 0
    bool	idl_usable;	// false: not cacheable, use the slow path
} idl_اندراج_T;

قسمتعریف ساخت
{
    idl_اندراج_T	ent[IDL_CACHE_سائز];
    صحیح		باری;		// round-robin replacement index
    صحیح		آخری;		// index of the last returned entry (fast path)
} idl_cache_T;

/*
 * Collect the explicit group IDs of "list" into "ga", expanding clusters.
 * "seen" records visited clusters to break cycles and avoid duplicate work.
 * Return false when the list cannot be represented as a plain set (a nested
 * ALLBUT/TOP/CONTAINED marker) or on out of memory.
 */
    ساکن bool
idl_collect(مختصر *list, garray_T *ga, garray_T *نظر_آیا)
{
    اگر (list == تہی || list == ID_LIST_ALL)
	واپس false;
    برائے (مختصر شے = *list; شے != 0; شے = *++list)
    {
	اگر (شے >= SYNID_ALLBUT && شے < SYNID_CLUSTER)
	    واپس false;	// nested ALLBUT/TOP/CONTAINED: not cacheable
	اگر (شے >= SYNID_CLUSTER)
	{
	    مختصر	*scl_list;
	    bool	نظر_آیا_it = false;

	    برائے (صحیح i = 0; i < نظر_آیا->ga_len; ++i)
		اگر (((مختصر *)نظر_آیا->ga_ڈیٹا)[i] == شے)
		{
		    نظر_آیا_it = true;
		    توڑو;
		}
	    اگر (نظر_آیا_it)
		جاری;
	    اگر (ga_grow(نظر_آیا, 1) == FAIL)
		واپس false;
	    ((مختصر *)نظر_آیا->ga_ڈیٹا)[نظر_آیا->ga_len++] = شے;
	    // Record the cluster id itself, not just its members: the slow
	    // path matches "item == id" for a cluster id present in the list.
	    اگر (ga_grow(ga, 1) == FAIL)
		واپس false;
	    ((مختصر *)ga->ga_ڈیٹا)[ga->ga_len++] = شے;
	    scl_list = SYN_CLSTR(syn_بلاک)[شے - SYNID_CLUSTER].scl_list;
	    اگر (scl_list != تہی && !idl_collect(scl_list, ga, نظر_آیا))
		واپس false;
	}
	ورنہ
	{
	    اگر (ga_grow(ga, 1) == FAIL)
		واپس false;
	    ((مختصر *)ga->ga_ڈیٹا)[ga->ga_len++] = شے;
	}
    }
    واپس true;
}

    ساکن صحیح
idl_id_cmp(ثابت خالی *a, ثابت خالی *b)
{
    واپس (صحیح)*(ثابت مختصر *)a - (صحیح)*(ثابت مختصر *)b;
}

/*
 * Get the cache entry that resolves "list", building it if needed.
 * Returns NULL only on allocation failure.
 */
    ساکن idl_اندراج_T *
idl_get_اندراج(مختصر *list)
{
    idl_cache_T	*cache = (idl_cache_T *)syn_بلاک->b_idlist_cache;
    idl_اندراج_T	*e;
    garray_T	ga, نظر_آیا;

    اگر (cache == تہی)
    {
	cache = ALLOC_CLEAR_ONE(idl_cache_T);
	اگر (cache == تہی)
	    واپس تہی;
	syn_بلاک->b_idlist_cache = cache;
    }
    // Fast path: the same list is usually queried many times in a row.
    اگر (cache->ent[cache->آخری].idl_کلید == list)
	واپس &cache->ent[cache->آخری];
    برائے (صحیح i = 0; i < IDL_CACHE_سائز; ++i)
	اگر (cache->ent[i].idl_کلید == list)
	{
	    cache->آخری = i;
	    واپس &cache->ent[i];
	}

    // Build a new entry, replacing one in round-robin order.
    cache->آخری = cache->باری;
    e = &cache->ent[cache->باری];
    cache->باری = (cache->باری + 1) % IDL_CACHE_سائز;
    VIM_CLEAR(e->idl_ids);
    e->idl_گنتی = 0;
    e->idl_کلید = list;
    e->idl_marker = 0;
    e->idl_usable = true;

    // A leading ALLBUT/TOP/CONTAINED marker is kept aside; the rest is the set.
    اگر (*list >= SYNID_ALLBUT && *list < SYNID_CLUSTER)
    {
	e->idl_marker = *list;
	++list;
    }

    ga_init2(&ga, حجم(مختصر), 50);
    ga_init2(&نظر_آیا, حجم(مختصر), 10);
    اگر (!idl_collect(list, &ga, &نظر_آیا))
    {
	e->idl_usable = false;
	ga_clear(&ga);
	ga_clear(&نظر_آیا);
	واپس e;
    }
    ga_clear(&نظر_آیا);
    اگر (ga.ga_len > 0)
    {
	qsort(ga.ga_ڈیٹا, (size_t)ga.ga_len, حجم(مختصر), idl_id_cmp);
	e->idl_ids = (مختصر *)ga.ga_ڈیٹا;	// take ownership
	e->idl_گنتی = ga.ga_len;
    }
    ورنہ
	ga_clear(&ga);
    واپس e;
}

/*
 * Return true if "id" is in the resolved set of cache entry "e".
 */
    ساکن bool
idl_شامل_ہے(idl_اندراج_T *e, مختصر id)
{
    صحیح	lo = 0;
    صحیح	hi = e->idl_گنتی - 1;

    جب (lo <= hi)
    {
	صحیح	mid = (lo + hi) / 2;
	مختصر	v = e->idl_ids[mid];

	اگر (v == id)
	    واپس true;
	اگر (v < id)
	    lo = mid + 1;
	ورنہ
	    hi = mid - 1;
    }
    واپس false;
}

    ساکن خالی
idl_cache_clear(synblock_T *بلاک)
{
    idl_cache_T	*cache = (idl_cache_T *)بلاک->b_idlist_cache;

    اگر (cache == تہی)
	واپس;
    برائے (صحیح i = 0; i < IDL_CACHE_سائز; ++i)
	vim_free(cache->ent[i].idl_ids);
    vim_free(cache);
    بلاک->b_idlist_cache = تہی;
}
// }}}

/*
 * Check if syntax group "ssp" is in the ID list "list" of "cur_si".
 * "cur_si" can be NULL if not checking the "containedin" list.
 * Used to check if a syntax item is in the "contains" or "nextgroup" list of
 * the current item.
 * This function is called very often, keep it fast!!
 */
    ساکن صحیح
اندر_id_list(
    stateitem_T	*cur_si,	// current item or NULL
    مختصر	*list,		// id list
    ساخت sp_syn *ssp,		// group id and ":syn include" tag of group
    صحیح		flags)		// group flags
{
    صحیح		retval;
    مختصر	*scl_list;
    مختصر	شے;
    مختصر	id = ssp->id;
    ساکن صحیح	گہرائی = 0;
    صحیح		r;
    صحیح		toplevel;
    idl_اندراج_T	*e;

    // If ssp has a "containedin" list and "cur_si" is in it, return TRUE.
    اگر (cur_si != تہی && ssp->cont_اندر_list != تہی
					    && !(cur_si->si_flags & HL_مطابقت))
    {
	// Ignore transparent items without a contains argument.  Double check
	// that we don't go back past the first one.
	جب ((cur_si->si_flags & HL_TRANS_CONT)
		&& cur_si > (stateitem_T *)(current_state.ga_ڈیٹا))
	    --cur_si;
	// cur_si->si_idx is -1 for keywords, these never contain anything.
	اگر (cur_si->si_idx >= 0 && اندر_id_list(تہی, ssp->cont_اندر_list,
		&(SYN_اشیاء(syn_بلاک)[cur_si->si_idx].sp_syn),
		  SYN_اشیاء(syn_بلاک)[cur_si->si_idx].sp_flags))
	    واپس TRUE;
    }

    اگر (list == تہی)
	واپس FALSE;

    /*
     * If list is ID_LIST_ALL, we are in a transparent item that isn't
     * inside anything.  Only allow not-contained groups.
     */
    اگر (list == ID_LIST_ALL)
	واپس !(flags & HL_CONTAINED);

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
    e = disable_syn_idlist_cache_for_testing ? تہی : idl_get_اندراج(list);
    اگر (e != تہی && e->idl_usable)
    {
	اگر (e->idl_marker != 0)
	{
	    شے = e->idl_marker;
	    اگر (شے < SYNID_چوٹی)
	    {
		اگر (شے - SYNID_ALLBUT != ssp->inc_لیبل)
		    واپس FALSE;
	    }
	    ورنہ اگر (شے < SYNID_CONTAINED)
	    {
		اگر (شے - SYNID_چوٹی != ssp->inc_لیبل || !toplevel)
		    واپس FALSE;
	    }
	    ورنہ
	    {
		اگر (شے - SYNID_CONTAINED != ssp->inc_لیبل || toplevel)
		    واپس FALSE;
	    }
	    retval = FALSE;
	}
	ورنہ
	    retval = TRUE;
	واپس idl_شامل_ہے(e, id) ? retval : !retval;
    }

    /*
     * Slow path (uncacheable list, e.g. a nested ALLBUT).
     * If the first item is "ALLBUT", return TRUE if "id" is NOT in the
     * contains list.  We also require that "id" is at the same ":syn include"
     * level as the list.
     */
    شے = *list;
    اگر (شے >= SYNID_ALLBUT && شے < SYNID_CLUSTER)
    {
	اگر (شے < SYNID_چوٹی)
	{
	    // ALL or ALLBUT: accept all groups in the same file
	    اگر (شے - SYNID_ALLBUT != ssp->inc_لیبل)
		واپس FALSE;
	}
	ورنہ اگر (شے < SYNID_CONTAINED)
	{
	    // TOP: accept all not-contained groups in the same file
	    اگر (شے - SYNID_چوٹی != ssp->inc_لیبل || !toplevel)
		واپس FALSE;
	}
	ورنہ
	{
	    // CONTAINED: accept all contained groups in the same file
	    اگر (شے - SYNID_CONTAINED != ssp->inc_لیبل || toplevel)
		واپس FALSE;
	}
	شے = *++list;
	retval = FALSE;
    }
    ورنہ
	retval = TRUE;

    /*
     * Return "retval" if id is in the contains list.
     */
    جب (شے != 0)
    {
	اگر (شے == id)
	    واپس retval;
	اگر (شے >= SYNID_CLUSTER)
	{
	    scl_list = SYN_CLSTR(syn_بلاک)[شے - SYNID_CLUSTER].scl_list;
	    // restrict recursiveness to 30 to avoid an endless loop for a
	    // cluster that includes itself (indirectly)
	    اگر (scl_list != تہی && گہرائی < 30)
	    {
		++گہرائی;
		r = اندر_id_list(تہی, scl_list, ssp, flags);
		--گہرائی;
		اگر (r)
		    واپس retval;
	    }
	}
	شے = *++list;
    }
    واپس !retval;
}

ساخت subcommand
{
    حرف    *name;			// subcommand name
    خالی    (*func)(exarg_T *, صحیح);	// function to call
};

ساکن ساخت subcommand subcommands[] =
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
    {"match",		syn_cmd_مطابقت},
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
    خالی
ex_syntax(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*subcmd_انتہا;
    char_u	*subcmd_name;
    صحیح		i;

    syn_cmdlinep = eap->cmdlinep;

    // isolate subcommand name
    برائے (subcmd_انتہا = arg; ASCII_ISALPHA(*subcmd_انتہا); ++subcmd_انتہا)
	;
    subcmd_name = vim_strnsave(arg, subcmd_انتہا - arg);
    اگر (subcmd_name == تہی)
	واپس;

    اگر (eap->skip)		// skip error messages for all subcommands
	++emsg_skip;
    برائے (i = 0; i < (صحیح)ARRAY_لمبائی(subcommands); ++i)
    {
	اگر (STRCMP(subcmd_name, (char_u *)subcommands[i].name) == 0)
	{
	    eap->arg = skipwhite(subcmd_انتہا);
	    (subcommands[i].func)(eap, FALSE);
	    توڑو;
	}
    }

    اگر (i == (صحیح)ARRAY_لمبائی(subcommands))
	semsg(_(e_invalid_syntax_subcommand_str), subcmd_name);

    vim_free(subcmd_name);
    اگر (eap->skip)
	--emsg_skip;
}

    خالی
ex_ownsyntax(exarg_T *eap)
{
    char_u	*old_قدر;
    char_u	*new_قدر_2;

    اگر (curwin->w_s == &curwin->w_بفر->b_s)
    {
	curwin->w_s = ALLOC_ONE(synblock_T);
	CLEAR_اشارہ_گر(curwin->w_s);
	hash_init(&curwin->w_s->b_keywtab);
	hash_init(&curwin->w_s->b_keywtab_ic);
#اگر_تعریف_شدہ FEAT_SPELL
	// TODO: keep the spell checking as it was.
	curwin->w_p_spell = FALSE;	// No spell checking
	// make sure option values are "empty_option" instead of NULL
	clear_string_option(&curwin->w_s->b_p_spc);
	clear_string_option(&curwin->w_s->b_p_spf);
	clear_string_option(&curwin->w_s->b_p_spl);
	clear_string_option(&curwin->w_s->b_p_spo);
#اگر_ختم
	clear_string_option(&curwin->w_s->b_syn_isk);
    }

    // save value of b:current_syntax
    old_قدر = get_var_قدر((char_u *)"b:current_syntax");
    اگر (old_قدر != تہی)
	old_قدر = vim_strsave(old_قدر);

    // Apply the "syntax" autocommand event, this finds and loads the syntax
    // file.
    apply_autocmds(EVENT_SYNTAX, eap->arg, curbuf->b_fname, TRUE, curbuf);

    // move value of b:current_syntax to w:current_syntax
    new_قدر_2 = get_var_قدر((char_u *)"b:current_syntax");
    اگر (new_قدر_2 != تہی)
	سیٹ_اندرونی_string_var((char_u *)"w:current_syntax", new_قدر_2);

    // restore value of b:current_syntax
    اگر (old_قدر == تہی)
	do_unlet((char_u *)"b:current_syntax", TRUE);
    ورنہ
    {
	سیٹ_اندرونی_string_var((char_u *)"b:current_syntax", old_قدر);
	vim_free(old_قدر);
    }
}

    صحیح
syntax_present(win_T *win)
{
    واپس (win->w_s->b_syn_patterns.ga_len != 0
	    || win->w_s->b_syn_clusters.ga_len != 0
	    || win->w_s->b_keywtab.ht_مستعمل > 0
	    || win->w_s->b_keywtab_ic.ht_مستعمل > 0);
}


ساکن شمار
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
    خالی
reset_expand_highlight(خالی)
{
    include_link = include_default = include_none = 0;
}

/*
 * Handle command line completion for :match and :echohl command: Add "NONE"
 * as highlight group.
 */
    خالی
سیٹ_context_اندر_echohl_cmd(expand_T *xp, char_u *arg)
{
    xp->xp_context = EXPAND_HIGHLIGHT;
    xp->xp_نمونہ = arg;
    include_none = 1;
}

/*
 * Handle command line completion for :syntax command.
 */
    خالی
سیٹ_context_اندر_syntax_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    // Default: expand subcommands
    xp->xp_context = EXPAND_SYNTAX;
    expand_what = EXP_SUBCMD;
    xp->xp_نمونہ = arg;
    include_link = 0;
    include_default = 0;

    اگر (*arg == NUL)
	واپس;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    اگر (*p == NUL)
	واپس;

    // past first word
    xp->xp_نمونہ = skipwhite(p);
    اگر (*skiptowhite(xp->xp_نمونہ) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    ورنہ اگر (STRNICMP(arg, "case", p - arg) == 0)
	expand_what = EXP_CASE;
    ورنہ اگر (STRNICMP(arg, "spell", p - arg) == 0)
	expand_what = EXP_SPELL;
    ورنہ اگر (STRNICMP(arg, "sync", p - arg) == 0)
	expand_what = EXP_SYNC;
    ورنہ اگر (STRNICMP(arg, "list", p - arg) == 0)
    {
	p = skipwhite(p);
	اگر (*p == '@')
	    expand_what = EXP_CLUSTER;
	ورنہ
	    xp->xp_context = EXPAND_HIGHLIGHT;
    }
    ورنہ اگر (STRNICMP(arg, "keyword", p - arg) == 0
	    || STRNICMP(arg, "region", p - arg) == 0
	    || STRNICMP(arg, "match", p - arg) == 0)
	xp->xp_context = EXPAND_HIGHLIGHT;
    ورنہ
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
	    اگر (idx < 0 || idx >= (صحیح)ARRAY_لمبائی(subcommands))
		واپس تہی;
	    واپس (char_u *)subcommands[idx].name;
	حالت EXP_CASE:
	{
	    ساکن حرف *case_دلائل[] = {"match", "ignore", تہی};
	    واپس (char_u *)case_دلائل[idx];
	}
	حالت EXP_SPELL:
	{
	    ساکن حرف *spell_دلائل[] =
		{"toplevel", "notoplevel", "default", تہی};
	    واپس (char_u *)spell_دلائل[idx];
	}
	حالت EXP_SYNC:
	{
	    ساکن حرف *sync_دلائل[] =
		{"ccomment", "clear", "fromstart",
		 "linebreaks=", "linecont", "lines=", "match",
		 "maxlines=", "minlines=", "region", تہی};
	    واپس (char_u *)sync_دلائل[idx];
	}
	حالت EXP_CLUSTER:
	{
	    اگر (idx < curwin->w_s->b_syn_clusters.ga_len)
	    {
		vim_snprintf((حرف *)xp->xp_buf, EXPAND_BUF_LEN, "@%s",
					 SYN_CLSTR(curwin->w_s)[idx].scl_name);
		واپس xp->xp_buf;
	    }
	    ورنہ
		واپس تہی;
	}
    }
    واپس تہی;
}


/*
 * Function called for expression evaluation: get syntax ID at file position.
 */
    صحیح
syn_get_id(
    win_T	*wp,
    طویل	lnum,
    colnr_T	ستون,
    صحیح		trans,	     // remove transparency
    صحیح		*spellp,     // return: can do spell checking
    صحیح		keep_state)  // keep state of char at "col"
{
    // When the position is not after the current position and in the same
    // line of the same window with the same buffer, need to restart parsing.
    اگر (wp != syn_win
	    || wp->w_بفر != syn_buf
	    || lnum != current_lnum
	    || ستون < current_ستون)
	syntax_آغاز(wp, lnum);
    ورنہ اگر (wp->w_بفر == syn_buf
	    && lnum == current_lnum
	    && ستون > current_ستون)
	// next_match may not be correct when moving around, e.g. with the
	// "skip" expression in searchpair()
	اگلا_مطابقت_idx = -1;

    (خالی)get_syntax_attr(ستون, spellp, keep_state);

    واپس (trans ? current_trans_id : current_id);
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
    واپس current_flags;
}

/*
 * Return conceal substitution character
 */
    صحیح
syn_get_sub_char(خالی)
{
    واپس current_sub_char;
}
#اگر_ختم

#اگر defined(FEAT_EVAL)
/*
 * Return the syntax ID at position "i" in the current stack.
 * The caller must have called syn_get_id() before to fill the stack.
 * Returns -1 when "i" is out of range.
 */
    صحیح
syn_get_ڈھیر_شے(صحیح i)
{
    اگر (i >= current_state.ga_len)
    {
	// Need to invalidate the state, because we didn't properly finish it
	// for the last character, "keep_state" was TRUE.
	invalidate_current_state();
	current_ستون = MAXCOL;
	واپس -1;
    }
    واپس CUR_STATE(i).si_id;
}
#اگر_ختم

#اگر defined(FEAT_FOLDING)
    ساکن صحیح
syn_cur_foldlevel(خالی)
{
    صحیح		level = 0;
    صحیح		i;

    برائے (i = 0; i < current_state.ga_len; ++i)
	اگر (CUR_STATE(i).si_flags & HL_FOLD)
	    ++level;
    واپس level;
}

/*
 * Function called to get folding level for line "lnum" in window "wp".
 */
    صحیح
syn_get_foldlevel(win_T *wp, طویل lnum)
{
    صحیح		level = 0;
    صحیح		کم_level;
    صحیح		cur_level;

    // Return quickly when there are no fold items at all.
    اگر (wp->w_s->b_syn_folditems != 0
	    && !wp->w_s->b_syn_error
# ifdef SYN_وقت_حد_بندی
	    && !wp->w_s->b_syn_slow
# endif
	    )
    {
	syntax_آغاز(wp, lnum);

	// Start with the fold level at the start of the line.
	level = syn_cur_foldlevel();

	اگر (wp->w_s->b_syn_foldlevel == SYNFLD_MINIMUM)
	{
	    // Find the lowest fold level that is followed by a higher one.
	    cur_level = level;
	    کم_level = cur_level;
	    جب (!current_ختم_شدہ)
	    {
		(خالی)syn_current_attr(FALSE, FALSE, تہی, FALSE);
		cur_level = syn_cur_foldlevel();
		اگر (cur_level < کم_level)
		    کم_level = cur_level;
		ورنہ اگر (cur_level > کم_level)
		    level = کم_level;
		++current_ستون;
	    }
	}
    }
    اگر (level > wp->w_p_fdn)
    {
	level = wp->w_p_fdn;
	اگر (level < 0)
	    level = 0;
    }
    واپس level;
}
#اگر_ختم

#اگر defined(FEAT_PROFILE)
/*
 * ":syntime".
 */
    خالی
ex_syntime(exarg_T *eap)
{
    اگر (STRCMP(eap->arg, "on") == 0)
	syn_وقت_on = TRUE;
    ورنہ اگر (STRCMP(eap->arg, "off") == 0)
	syn_وقت_on = FALSE;
    ورنہ اگر (STRCMP(eap->arg, "clear") == 0)
	syntime_clear();
    ورنہ اگر (STRCMP(eap->arg, "report") == 0)
	syntime_report();
    ورنہ
	semsg(_(e_invalid_دلیل_str), eap->arg);
}

    ساکن خالی
syn_clear_وقت(syn_وقت_T *st)
{
    profile_zero(&st->کل);
    profile_zero(&st->slowest);
    st->گنتی = 0;
    st->مطابقت = 0;
}

/*
 * Clear the syntax timing for the current buffer.
 */
    ساکن خالی
syntime_clear(خالی)
{
    صحیح		idx;
    synpat_T	*spp;

    اگر (!syntax_present(curwin))
    {
	پیغام(_(پیغام_no_اشیاء));
	واپس;
    }
    برائے (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len; ++idx)
    {
	spp = &(SYN_اشیاء(curwin->w_s)[idx]);
	syn_clear_وقت(&spp->sp_وقت);
    }
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":syntime {on,off,clear,report}" command.
 */
    char_u *
get_syntime_arg(expand_T *xp غیر_مستعمل, صحیح idx)
{
    انتخاب (idx)
    {
	حالت 0: واپس (char_u *)"on";
	حالت 1: واپس (char_u *)"off";
	حالت 2: واپس (char_u *)"clear";
	حالت 3: واپس (char_u *)"report";
    }
    واپس تہی;
}

قسمتعریف ساخت
{
    proftime_T	کل;
    صحیح		گنتی;
    صحیح		مطابقت;
    proftime_T	slowest;
    proftime_T	اوسط_قدر;
    صحیح		id;
    char_u	*نمونہ;
} وقت_اندراج_T;

    ساکن صحیح
syn_compare_syntime(ثابت خالی *v1, ثابت خالی *v2)
{
    ثابت وقت_اندراج_T	*s1 = v1;
    ثابت وقت_اندراج_T	*s2 = v2;

    واپس profile_cmp(&s1->کل, &s2->کل);
}

/*
 * Clear the syntax timing for the current buffer.
 */
    ساکن خالی
syntime_report(خالی)
{
    صحیح		idx;
    synpat_T	*spp;
# اگر defined(FEAT_RELTIME)
    proftime_T	tm;
# endif
    صحیح		len;
    صحیح		patlen;
    proftime_T	کل_کل;
    صحیح		کل_گنتی = 0;
    garray_T    ga;
    وقت_اندراج_T *p;

    اگر (!syntax_present(curwin))
    {
	پیغام(_(پیغام_no_اشیاء));
	واپس;
    }

    ga_init2(&ga, حجم(وقت_اندراج_T), 50);
    profile_zero(&کل_کل);
    برائے (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len; ++idx)
    {
	spp = &(SYN_اشیاء(curwin->w_s)[idx]);
	اگر (spp->sp_وقت.گنتی > 0)
	{
	    (خالی)ga_grow(&ga, 1);
	    p = ((وقت_اندراج_T *)ga.ga_ڈیٹا) + ga.ga_len;
	    p->کل = spp->sp_وقت.کل;
	    profile_جوڑو(&کل_کل, &spp->sp_وقت.کل);
	    p->گنتی = spp->sp_وقت.گنتی;
	    p->مطابقت = spp->sp_وقت.مطابقت;
	    کل_گنتی += spp->sp_وقت.گنتی;
	    p->slowest = spp->sp_وقت.slowest;
# اگر defined(FEAT_RELTIME)
	    profile_divide(&spp->sp_وقت.کل, spp->sp_وقت.گنتی, &tm);
	    p->اوسط_قدر = tm;
# endif
	    p->id = spp->sp_syn.id;
	    p->نمونہ = spp->sp_نمونہ;
	    ++ga.ga_len;
	}
    }

    // Sort on total time. Skip if there are no items to avoid passing NULL
    // pointer to qsort().
    اگر (ga.ga_len > 1)
	qsort(ga.ga_ڈیٹا, (size_t)ga.ga_len, حجم(وقت_اندراج_T),
							 syn_compare_syntime);

    پیغام_puts_title(_("  TOTAL      COUNT  MATCH   SLOWEST     AVERAGE   NAME               PATTERN"));
    پیغام_puts("\n");
    برائے (idx = 0; idx < ga.ga_len && !got_int; ++idx)
    {
	p = ((وقت_اندراج_T *)ga.ga_ڈیٹا) + idx;

	پیغام_puts(profile_پیغام(&p->کل));
	پیغام_puts(" "); // make sure there is always a separating space
	پیغام_advance(13);
	پیغام_outnum(p->گنتی);
	پیغام_puts(" ");
	پیغام_advance(20);
	پیغام_outnum(p->مطابقت);
	پیغام_puts(" ");
	پیغام_advance(26);
	پیغام_puts(profile_پیغام(&p->slowest));
	پیغام_puts(" ");
	پیغام_advance(38);
	پیغام_puts(profile_پیغام(&p->اوسط_قدر));
	پیغام_puts(" ");
	پیغام_advance(50);
	پیغام_outtrans(highlight_گروہ_name(p->id - 1));
	پیغام_puts(" ");

	پیغام_advance(69);
	اگر (Columns < 80)
	    len = 20; // will wrap anyway
	ورنہ
	    len = Columns - 70;
	patlen = (صحیح)STRLEN(p->نمونہ);
	اگر (len > patlen)
	    len = patlen;
	پیغام_outtrans_len(p->نمونہ, len);
	پیغام_puts("\n");
    }
    ga_clear(&ga);
    اگر (!got_int)
    {
	پیغام_puts("\n");
	پیغام_puts(profile_پیغام(&کل_کل));
	پیغام_advance(13);
	پیغام_outnum(کل_گنتی);
	پیغام_puts("\n");
    }
}
#اگر_ختم

#اگر_ختم // FEAT_SYN_HL
