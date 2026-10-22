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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_SYN_HL)

// different types of offsets that are possible
#నిర్వచించు SPO_MS_OFF	0	// match  start offset
#నిర్వచించు SPO_ME_OFF	1	// match  end	offset
#నిర్వచించు SPO_HS_OFF	2	// highl. start offset
#నిర్వచించు SPO_HE_OFF	3	// highl. end	offset
#నిర్వచించు SPO_RS_OFF	4	// region start offset
#నిర్వచించు SPO_RE_OFF	5	// region end	offset
#నిర్వచించు SPO_LC_OFF	6	// leading context offset
#నిర్వచించు SPO_లెక్క	7

స్థిర అక్షరం *(spo_name_tab[SPO_లెక్క]) =
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
రకనిర్వచనం నిర్మాణం syn_నమూనా
{
    అక్షరం	 sp_type;		// see SPTYPE_ defines below
    అక్షరం	 sp_syncing;		// this item used for syncing
    పొట్టి	 sp_syn_సరిపోలిక_id;	// highlight group ID of pattern
    పొట్టి	 sp_off_flags;		// see below
    పూర్ణాంకం		 sp_offsets[SPO_లెక్క];	// offsets
    పూర్ణాంకం		 sp_flags;		// see HL_ defines below
#నిర్వచితమైతే FEAT_CONCEAL
    పూర్ణాంకం		 sp_cchar;		// conceal substitute character
#అయితే_ముగింపు
    పూర్ణాంకం		 sp_ic;			// ignore-case flag for sp_prog
    పూర్ణాంకం		 sp_sync_idx;		// sync item index (syncing only)
    పూర్ణాంకం		 sp_పంక్తి_id;		// ID of last line where tried
    పూర్ణాంకం		 sp_startcol;		// next match in sp_line_id line
    పొట్టి	*sp_cont_list;		// cont. group IDs, if non-zero
    పొట్టి	*sp_తదుపరి_list;		// next group IDs, if non-zero
    నిర్మాణం sp_syn sp_syn;		// struct passed to in_id_list()
    char_u	*sp_నమూనా;		// regexp to match, pattern
    regprog_T	*sp_prog;		// regexp to match, program
#నిర్వచితమైతే FEAT_PROFILE
    syn_సమయం_T	 sp_సమయం;
#అయితే_ముగింపు
} synpat_T;

// The sp_off_flags are computed like this:
// offset from the start of the matched text: (1 << SPO_XX_OFF)
// offset from the end	 of the matched text: (1 << (SPO_XX_OFF + SPO_COUNT))
// When both are present, only one is used.

#నిర్వచించు SPTYPE_సరిపోలిక	1	// match keyword with this group ID
#నిర్వచించు SPTYPE_ప్రారంభం	2	// match a regexp, start of item
#నిర్వచించు SPTYPE_చివర	3	// match a regexp, end of item
#నిర్వచించు SPTYPE_SKIP	4	// match a regexp, skip within item


#నిర్వచించు SYN_వస్తువులు(buf)	((synpat_T *)((buf)->b_syn_patterns.ga_దత్తాంశం))

#నిర్వచించు NONE_IDX	(-2)	// value of sp_sync_idx for "NONE"

/*
 * Flags for b_syn_sync_flags:
 */
#నిర్వచించు SF_CCOMMENT	0x01	// sync on a C-style comment
#నిర్వచించు SF_సరిపోలిక	0x02	// sync by matching a pattern

#నిర్వచించు SYN_STATE_P(ssp)    ((bufstate_T *)((ssp)->ga_దత్తాంశం))

#నిర్వచించు MAXKEYWLEN	80	    // maximum length of a keyword

/*
 * The attributes of the syntax item that has been recognized.
 */
స్థిర పూర్ణాంకం current_attr = 0;	    // attr of current syntax word
#నిర్వచితమైతే FEAT_EVAL
స్థిర పూర్ణాంకం current_id = 0;	    // ID of current char for syn_get_id()
స్థిర పూర్ణాంకం current_trans_id = 0;    // idem, transparency removed
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CONCEAL
స్థిర పూర్ణాంకం current_flags = 0;
స్థిర పూర్ణాంకం current_seqnr = 0;
స్థిర పూర్ణాంకం current_sub_char = 0;
#అయితే_ముగింపు

రకనిర్వచనం నిర్మాణం syn_cluster_S
{
    char_u	    *scl_name;	    // syntax cluster name
    char_u	    *scl_name_u;    // uppercase of scl_name
    పొట్టి	    *scl_list;	    // IDs in this syntax cluster
} syn_cluster_T;

/*
 * Methods of combining two clusters
 */
#నిర్వచించు CLUSTER_REPLACE	    1	// replace first list with second
#నిర్వచించు CLUSTER_కూడు	    2	// add second list to first
#నిర్వచించు CLUSTER_వ్యవకలనం    3	// subtract second list from first

#నిర్వచించు SYN_CLSTR(buf)	((syn_cluster_T *)((buf)->b_syn_clusters.ga_దత్తాంశం))

/*
 * Syntax group IDs have different types:
 *     0 - 19999  normal syntax groups
 * 20000 - 20999  ALLBUT indicator (current_syn_inc_tag added)
 * 21000 - 21999  TOP indicator (current_syn_inc_tag added)
 * 22000 - 22999  CONTAINED indicator (current_syn_inc_tag added)
 * 23000 - 32767  cluster IDs (subtract SYNID_CLUSTER for the cluster ID)
 */
#నిర్వచించు SYNID_ALLBUT	MAX_HL_ID   // syntax group ID for contains=ALLBUT
#నిర్వచించు SYNID_శిఖరం	21000	    // syntax group ID for contains=TOP
#నిర్వచించు SYNID_CONTAINED	22000	    // syntax group ID for contains=CONTAINED
#నిర్వచించు SYNID_CLUSTER	23000	    // first syntax group ID for clusters

#నిర్వచించు MAX_SYN_INC_ట్యాగ్	999	    // maximum before the above overflow
#నిర్వచించు MAX_CLUSTER_ID  (32767 - SYNID_CLUSTER)

/*
 * Annoying Hack(TM):  ":syn include" needs this pointer to pass to
 * expand_filename().  Most of the other syntax commands don't need it, so
 * instead of passing it to them, we stow it here.
 */
స్థిర char_u **syn_cmdlinep;

/*
 * Another Annoying Hack(TM):  To prevent rules from other ":syn include"'d
 * files from leaking into ALLBUT lists, we assign a unique ID to the
 * rules in each ":syn include"'d file.
 */
స్థిర పూర్ణాంకం current_syn_inc_ట్యాగ్ = 0;
స్థిర పూర్ణాంకం running_syn_inc_ట్యాగ్ = 0;

/*
 * In a hashtable item "hi_key" points to "keyword" in a keyentry.
 * This avoids adding a pointer to the hashtable item.
 * KE2HIKEY() converts a var pointer to a hashitem key pointer.
 * HIKEY2KE() converts a hashitem key pointer to a var pointer.
 * HI2KE() converts a hashitem pointer to a var pointer.
 */
స్థిర keyentry_T dumkey;
#నిర్వచించు KE2HIKEY(kp)  ((kp)->keyword)
#నిర్వచించు HIKEY2KE(p)   ((keyentry_T *)((p) - (dumkey.keyword - (char_u *)&dumkey)))
#నిర్వచించు HI2KE(hi)      HIKEY2KE((hi)->hi_కీలకం)

/*
 * To reduce the time spent in keepend(), remember at which level in the state
 * stack the first item with "keepend" is present.  When "-1", there is no
 * "keepend" on the stack.
 */
స్థిర పూర్ణాంకం keepend_level = -1;

స్థిర అక్షరం సందేశం_no_వస్తువులు[] = N_("No Syntax items defined for this buffer");

/*
 * For the current state we need to remember more than just the idx.
 * When si_m_endpos.lnum is 0, the items other than si_idx are unknown.
 * (The end positions have the column number of the next char)
 */
రకనిర్వచనం నిర్మాణం state_అంశం
{
    పూర్ణాంకం		si_idx;			// index of syntax pattern or
					// KEYWORD_IDX
    పూర్ణాంకం		si_id;			// highlight group ID for keywords
    పూర్ణాంకం		si_trans_id;		// idem, transparency removed
    పూర్ణాంకం		si_m_lnum;		// lnum of the match
    పూర్ణాంకం		si_m_startcol;		// starting column of the match
    lpos_T	si_m_endpos;		// just after end posn of the match
    lpos_T	si_h_startpos;		// start position of the highlighting
    lpos_T	si_h_endpos;		// end position of the highlighting
    lpos_T	si_eoe_pos;		// end position of end pattern
    పూర్ణాంకం		si_చివర_idx;		// group ID for end pattern or zero
    పూర్ణాంకం		si_చివర్లు;		// if match ends before si_m_endpos
    పూర్ణాంకం		si_attr;		// attributes in this state
    దీర్ఘ	si_flags;		// HL_HAS_EOL flag in this state, and
					// HL_SKIP* for si_next_list
#నిర్వచితమైతే FEAT_CONCEAL
    పూర్ణాంకం		si_seqnr;		// sequence number
    పూర్ణాంకం		si_cchar;		// substitution character for conceal
#అయితే_ముగింపు
    పొట్టి	*si_cont_list;		// list of contained groups
    పొట్టి	*si_తదుపరి_list;		// nextgroup IDs after this item ends
    reg_extmatch_T *si_extmatch;	// \z(...\) matches from start
					//నమూనా
} stateitem_T;

#నిర్వచించు KEYWORD_IDX	(-1)	    // value of si_idx for keywords
#నిర్వచించు ID_LIST_ALL	((పొట్టి *)-1) // valid of si_cont_list for containing all
				    // but contained groups

#నిర్వచితమైతే FEAT_CONCEAL
స్థిర పూర్ణాంకం తదుపరి_seqnr = 1;		// value to use for si_seqnr
#అయితే_ముగింపు

/*
 * Struct to reduce the number of arguments to get_syn_options(), it's used
 * very often.
 */
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం		flags;		// flags for contained and transparent
    పూర్ణాంకం		keyword;	// TRUE for ":syn keyword"
    పూర్ణాంకం		*sync_idx;	// syntax item for "grouphere" argument, NULL
				// if not allowed
    అక్షరం	has_cont_list;	// TRUE if "cont_list" can be used
    పొట్టి	*cont_list;	// group IDs for "contains" argument
    పొట్టి	*cont_లోపల_list;	// group IDs for "containedin" argument
    పొట్టి	*తదుపరి_list;	// group IDs for "nextgroup" argument
} syn_opt_arg_T;

/*
 * The next possible match in the current line for any pattern is remembered,
 * to avoid having to try for a match in each column.
 * If next_match_idx == -1, not tried (in this line) yet.
 * If next_match_col == MAXCOL, no match found in this line.
 * (All end positions have the column of the char after the end)
 */
స్థిర పూర్ణాంకం తదుపరి_సరిపోలిక_నిలువు;		// column for start of next match
స్థిర lpos_T తదుపరి_సరిపోలిక_m_endpos;	// position for end of next match
స్థిర lpos_T తదుపరి_సరిపోలిక_h_startpos;	// pos. for highl. start of next match
స్థిర lpos_T తదుపరి_సరిపోలిక_h_endpos;	// pos. for highl. end of next match
స్థిర పూర్ణాంకం తదుపరి_సరిపోలిక_idx;		// index of matched item
స్థిర దీర్ఘ తదుపరి_సరిపోలిక_flags;		// flags for next match
స్థిర lpos_T తదుపరి_సరిపోలిక_eos_pos;	// end of start pattn (start region)
స్థిర lpos_T తదుపరి_సరిపోలిక_eoe_pos;	// pos. for end of end pattern
స్థిర పూర్ణాంకం తదుపరి_సరిపోలిక_చివర_idx;		// ID of group for end pattn or zero
స్థిర reg_extmatch_T *తదుపరి_సరిపోలిక_extmatch = ఖాళీ;

/*
 * A state stack is an array of integers or stateitem_T, stored in a
 * garray_T.  A state stack is invalid if its itemsize entry is zero.
 */
#నిర్వచించు INVALID_STATE(ssp)  ((ssp)->ga_itemsize == 0)
#నిర్వచించు VALID_STATE(ssp)    ((ssp)->ga_itemsize != 0)

#నిర్వచించు FOR_ALL_SYNSTATES(sb, sst) \
    కొరకు ((sst) = (sb)->b_sst_మొదటి; (sst) != ఖాళీ; (sst) = (sst)->sst_తదుపరి)

/*
 * The current state (within the line) of the recognition engine.
 * When current_state.ga_itemsize is 0 the current state is invalid.
 */
స్థిర win_T	*syn_win;		// current window for highlighting
స్థిర buf_T	*syn_buf;		// current buffer for highlighting
స్థిర synblock_T *syn_ఖండం;		// current buffer for highlighting
స్థిర linenr_T current_lnum = 0;	// lnum of current state
స్థిర colnr_T	current_నిలువు = 0;	// column of current state
స్థిర పూర్ణాంకం	current_state_stored = 0; // TRUE if stored current state
					  // after setting current_finished
స్థిర పూర్ణాంకం	current_ముగిసిన = 0;	// current line has been finished
స్థిర garray_T current_state		// current stack of state_items
		= {0, 0, 0, 0, ఖాళీ};
స్థిర పొట్టి	*current_తదుపరి_list = ఖాళీ; // when non-zero, nextgroup list
స్థిర పూర్ణాంకం	current_తదుపరి_flags = 0; // flags for current_next_list
స్థిర పూర్ణాంకం	current_పంక్తి_id = 0;	// unique number for current line

#నిర్వచించు CUR_STATE(idx)	((stateitem_T *)(current_state.ga_దత్తాంశం))[idx]

స్థిర శూన్య syn_sync(win_T *wp, linenr_T lnum, synstate_T *చివరి_valid);
స్థిర పూర్ణాంకం syn_సరిపోలిక_linecont(linenr_T lnum);
స్థిర శూన్య syn_ప్రారంభం_పంక్తి(శూన్య);
స్థిర శూన్య syn_update_చివర్లు(పూర్ణాంకం startofline);
స్థిర శూన్య syn_దొంతర_alloc(శూన్య);
స్థిర పూర్ణాంకం syn_దొంతర_cleanup(శూన్య);
స్థిర శూన్య syn_దొంతర_free_నమోదు(synblock_T *ఖండం, synstate_T *p);
స్థిర synstate_T *syn_దొంతర_కనుగొను_నమోదు(linenr_T lnum);
స్థిర synstate_T *store_current_state(శూన్య);
స్థిర శూన్య ఎక్కించు_current_state(synstate_T *from);
స్థిర శూన్య invalidate_current_state(శూన్య);
స్థిర పూర్ణాంకం syn_దొంతర_equal(synstate_T *sp);
స్థిర శూన్య validate_current_state(శూన్య);
స్థిర పూర్ణాంకం syn_ముగింపు_పంక్తి(పూర్ణాంకం syncing);
స్థిర పూర్ణాంకం syn_current_attr(పూర్ణాంకం syncing, పూర్ణాంకం displaying, పూర్ణాంకం *can_spell, పూర్ణాంకం keep_state);
స్థిర పూర్ణాంకం did_సరిపోలిక_already(పూర్ణాంకం idx, garray_T *gap);
స్థిర stateitem_T *నెట్టు_తదుపరి_సరిపోలిక(stateitem_T *cur_si);
స్థిర శూన్య check_state_చివర్లు(శూన్య);
స్థిర శూన్య update_si_attr(పూర్ణాంకం idx);
స్థిర శూన్య check_keepend(శూన్య);
స్థిర శూన్య update_si_చివర(stateitem_T *sip, పూర్ణాంకం startcol, పూర్ణాంకం force);
స్థిర పొట్టి *copy_id_list(పొట్టి *list);
స్థిర పూర్ణాంకం లోపల_id_list(stateitem_T *అంశం, పొట్టి *cont_list, నిర్మాణం sp_syn *ssp, పూర్ణాంకం flags);
స్థిర పూర్ణాంకం నెట్టు_current_state(పూర్ణాంకం idx);
స్థిర శూన్య తీయి_current_state(శూన్య);
#నిర్వచితమైతే FEAT_PROFILE
స్థిర శూన్య syn_clear_సమయం(syn_సమయం_T *tt);
స్థిర శూన్య syntime_clear(శూన్య);
స్థిర శూన్య syntime_report(శూన్య);
స్థిర పూర్ణాంకం syn_సమయం_on = FALSE;
# define IF_SYN_సమయం(p) (p)
#లేకపోతే
# define IF_SYN_సమయం(p) ఖాళీ
రకనిర్వచనం పూర్ణాంకం syn_సమయం_T;
#అయితే_ముగింపు

స్థిర శూన్య syn_దొంతర_apply_changes_ఖండం(synblock_T *ఖండం, buf_T *buf);
స్థిర శూన్య కనుగొను_endpos(పూర్ణాంకం idx, lpos_T *startpos, lpos_T *m_endpos, lpos_T *hl_endpos, దీర్ఘ *flagsp, lpos_T *చివర_endpos, పూర్ణాంకం *చివర_idx, reg_extmatch_T *ప్రారంభం_ext);

స్థిర శూన్య హద్దు_pos(lpos_T *pos, lpos_T *హద్దు);
స్థిర శూన్య హద్దు_pos_zero(lpos_T *pos, lpos_T *హద్దు);
స్థిర శూన్య syn_కూడు_చివర_off(lpos_T *ఫలితం, regmmatch_T *regmatch, synpat_T *spp, పూర్ణాంకం idx, పూర్ణాంకం extra);
స్థిర శూన్య syn_కూడు_ప్రారంభం_off(lpos_T *ఫలితం, regmmatch_T *regmatch, synpat_T *spp, పూర్ణాంకం idx, పూర్ణాంకం extra);
స్థిర char_u *syn_getcurline(శూన్య);
స్థిర colnr_T syn_getcurline_len(శూన్య);
స్థిర పూర్ణాంకం syn_regexec(regmmatch_T *rmp, linenr_T lnum, colnr_T నిలువు, syn_సమయం_T *st);
స్థిర పూర్ణాంకం check_keyword_id(char_u *పంక్తి, పూర్ణాంకం startcol, పూర్ణాంకం *endcol, దీర్ఘ *flags, పొట్టి **తదుపరి_list, stateitem_T *cur_si, పూర్ణాంకం *ccharp);
స్థిర శూన్య syn_remove_నమూనా(synblock_T *ఖండం, పూర్ణాంకం idx);
స్థిర శూన్య syn_clear_నమూనా(synblock_T *ఖండం, పూర్ణాంకం i);
స్థిర శూన్య syn_clear_cluster(synblock_T *ఖండం, పూర్ణాంకం i);
స్థిర శూన్య syn_clear_one(పూర్ణాంకం id, పూర్ణాంకం syncing);
స్థిర శూన్య syn_cmd_onoff(exarg_T *eap, అక్షరం *name);
స్థిర శూన్య syn_పంక్తులు_సందేశం(శూన్య);
స్థిర శూన్య syn_సరిపోలిక_సందేశం(శూన్య);
స్థిర శూన్య syn_list_one(పూర్ణాంకం id, పూర్ణాంకం syncing, పూర్ణాంకం link_only);
స్థిర శూన్య syn_list_cluster(పూర్ణాంకం id);
స్థిర శూన్య put_id_list(char_u *name, పొట్టి *list, పూర్ణాంకం attr);
స్థిర శూన్య put_నమూనా(అక్షరం *s, పూర్ణాంకం c, synpat_T *spp, పూర్ణాంకం attr);
స్థిర పూర్ణాంకం syn_list_keywords(పూర్ణాంకం id, hashtab_T *ht, పూర్ణాంకం did_శీర్షిక, పూర్ణాంకం attr);
స్థిర శూన్య syn_clear_keyword(పూర్ణాంకం id, hashtab_T *ht);
స్థిర శూన్య clear_keywtab(hashtab_T *ht);
స్థిర పూర్ణాంకం syn_scl_namen2id(char_u *linep, పూర్ణాంకం len);
స్థిర పూర్ణాంకం syn_check_cluster(char_u *pp, పూర్ణాంకం len);
స్థిర పూర్ణాంకం syn_కూడు_cluster(char_u *name);
స్థిర శూన్య init_syn_patterns(శూన్య);
స్థిర char_u *get_syn_నమూనా(char_u *arg, synpat_T *ci);
స్థిర పూర్ణాంకం get_id_list(char_u **arg, పూర్ణాంకం keylen, పొట్టి **list, పూర్ణాంకం skip);
స్థిర శూన్య syn_combine_list(పొట్టి **clstr1, పొట్టి **clstr2, పూర్ణాంకం list_op);
స్థిర శూన్య idl_cache_clear(synblock_T *ఖండం);

/*
 * Start the syntax recognition for a line.  This function is normally called
 * from the screen updating, once for each displayed line.
 * The buffer is remembered in syn_buf, because get_syntax_attr() doesn't get
 * it.	Careful: curbuf and curwin are likely to point to another buffer and
 * window.
 */
    శూన్య
syntax_ప్రారంభం(win_T *wp, linenr_T lnum)
{
    synstate_T	*p;
    synstate_T	*చివరి_valid = ఖాళీ;
    synstate_T	*చివరి_min_valid = ఖాళీ;
    synstate_T	*sp, *prev = ఖాళీ;
    linenr_T	parsed_lnum;
    linenr_T	మొదటి_stored;
    పూర్ణాంకం		dist;
    స్థిర varnumber_T changedtick = 0;	// remember the last change ID

#నిర్వచితమైతే FEAT_CONCEAL
    current_sub_char = NUL;
#అయితే_ముగింపు

    /*
     * After switching buffers, invalidate current_state.
     * Also do this when a change was made, the current state may be invalid
     * then.
     */
    ఒకవేళ (syn_ఖండం != wp->w_s
	    || syn_buf != wp->w_బఫరు
	    || changedtick != CHANGEDTICK(syn_buf))
    {
	invalidate_current_state();
	syn_buf = wp->w_బఫరు;
	syn_ఖండం = wp->w_s;
    }
    changedtick = CHANGEDTICK(syn_buf);
    syn_win = wp;

    /*
     * Allocate syntax stack when needed.
     */
    syn_దొంతర_alloc();
    ఒకవేళ (syn_ఖండం->b_sst_array == ఖాళీ)
	తిరిగి;		// out of memory
    syn_ఖండం->b_sst_lasttick = display_tick;

    /*
     * If the state of the end of the previous line is useful, store it.
     */
    ఒకవేళ (VALID_STATE(&current_state)
	    && current_lnum < lnum
	    && current_lnum < syn_buf->b_ml.ml_పంక్తి_లెక్క)
    {
	(శూన్య)syn_ముగింపు_పంక్తి(FALSE);
	ఒకవేళ (!current_state_stored)
	{
	    ++current_lnum;
	    (శూన్య)store_current_state();
	}

	/*
	 * If the current_lnum is now the same as "lnum", keep the current
	 * state (this happens very often!).  Otherwise invalidate
	 * current_state and figure it out below.
	 */
	ఒకవేళ (current_lnum != lnum)
	    invalidate_current_state();
    }
    లేకపోతే
	invalidate_current_state();

    /*
     * Try to synchronize from a saved state in b_sst_array[].
     * Only do this if lnum is not before and not to far beyond a saved state.
     */
    ఒకవేళ (INVALID_STATE(&current_state) && syn_ఖండం->b_sst_array != ఖాళీ)
    {
	// Find last valid saved state before start_lnum.
	FOR_ALL_SYNSTATES(syn_ఖండం, p)
	{
	    ఒకవేళ (p->sst_lnum > lnum)
		ఆపు;
	    ఒకవేళ (p->sst_lnum <= lnum && p->sst_change_lnum == 0)
	    {
		చివరి_valid = p;
		ఒకవేళ (p->sst_lnum >= lnum - syn_ఖండం->b_syn_sync_minlines)
		    చివరి_min_valid = p;
	    }
	}
	ఒకవేళ (చివరి_min_valid != ఖాళీ)
	    ఎక్కించు_current_state(చివరి_min_valid);
    }

    /*
     * If "lnum" is before or far beyond a line with a saved state, need to
     * re-synchronize.
     */
    ఒకవేళ (INVALID_STATE(&current_state))
    {
	syn_sync(wp, lnum, చివరి_valid);
	ఒకవేళ (current_lnum == 1)
	    // First line is always valid, no matter "minlines".
	    మొదటి_stored = 1;
	లేకపోతే
	    // Need to parse "minlines" lines before state can be considered
	    // valid to store.
	    మొదటి_stored = current_lnum + syn_ఖండం->b_syn_sync_minlines;
    }
    లేకపోతే
	మొదటి_stored = current_lnum;

    /*
     * Advance from the sync point or saved state until the current line.
     * Save some entries for syncing with later on.
     */
    ఒకవేళ (syn_ఖండం->b_sst_len <= అడ్డువరుసలు)
	dist = 999999;
    లేకపోతే
	dist = syn_buf->b_ml.ml_పంక్తి_లెక్క / (syn_ఖండం->b_sst_len - అడ్డువరుసలు) + 1;
    అయితే (current_lnum < lnum)
    {
	syn_ప్రారంభం_పంక్తి();
	(శూన్య)syn_ముగింపు_పంక్తి(FALSE);
	++current_lnum;

	// If we parsed at least "minlines" lines or started at a valid
	// state, the current state is considered valid.
	ఒకవేళ (current_lnum >= మొదటి_stored)
	{
	    // Check if the saved state entry is for the current line and is
	    // equal to the current state.  If so, then validate all saved
	    // states that depended on a change before the parsed line.
	    ఒకవేళ (prev == ఖాళీ)
		prev = syn_దొంతర_కనుగొను_నమోదు(current_lnum - 1);
	    ఒకవేళ (prev == ఖాళీ)
		sp = syn_ఖండం->b_sst_మొదటి;
	    లేకపోతే
		sp = prev;
	    అయితే (sp != ఖాళీ && sp->sst_lnum < current_lnum)
		sp = sp->sst_తదుపరి;
	    ఒకవేళ (sp != ఖాళీ
		    && sp->sst_lnum == current_lnum
		    && syn_దొంతర_equal(sp))
	    {
		parsed_lnum = current_lnum;
		prev = sp;
		అయితే (sp != ఖాళీ && sp->sst_change_lnum <= parsed_lnum)
		{
		    ఒకవేళ (sp->sst_lnum <= lnum)
			// valid state before desired line, use this one
			prev = sp;
		    లేకపోతే ఒకవేళ (sp->sst_change_lnum == 0)
			// past saved states depending on change, break here.
			ఆపు;
		    sp->sst_change_lnum = 0;
		    sp = sp->sst_తదుపరి;
		}
		ఎక్కించు_current_state(prev);
	    }
	    // Store the state at this line when it's the first one, the line
	    // where we start parsing, or some distance from the previously
	    // saved state.  But only when parsed at least 'minlines'.
	    లేకపోతే ఒకవేళ (prev == ఖాళీ
			|| current_lnum == lnum
			|| current_lnum >= prev->sst_lnum + dist)
		prev = store_current_state();
	}

	// This can take a long time: break when CTRL-C pressed.  The current
	// state will be wrong then.
	పంక్తి_breakcheck();
	ఒకవేళ (got_int)
	{
	    current_lnum = lnum;
	    ఆపు;
	}
    }

    syn_ప్రారంభం_పంక్తి();
}

/*
 * We cannot simply discard growarrays full of state_items or buf_states; we
 * have to manually release their extmatch pointers first.
 */
    స్థిర శూన్య
clear_syn_state(synstate_T *p)
{
    పూర్ణాంకం		i;
    garray_T	*gap;

    ఒకవేళ (p->sst_stacksize > SST_FIX_స్థితులు)
    {
	gap = &(p->sst_union.sst_ga);
	కొరకు (i = 0; i < gap->ga_len; i++)
	    unref_extmatch(SYN_STATE_P(gap)[i].bs_extmatch);
	ga_clear(gap);
    }
    లేకపోతే
    {
	కొరకు (i = 0; i < p->sst_stacksize; i++)
	    unref_extmatch(p->sst_union.sst_దొంతర[i].bs_extmatch);
    }
}

/*
 * Cleanup the current_state stack.
 */
    స్థిర శూన్య
clear_current_state(శూన్య)
{
    పూర్ణాంకం		i;
    stateitem_T	*sip;

    sip = (stateitem_T *)(current_state.ga_దత్తాంశం);
    కొరకు (i = 0; i < current_state.ga_len; i++)
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
    స్థిర శూన్య
syn_sync(
    win_T	*wp,
    linenr_T	ప్రారంభం_lnum,
    synstate_T	*చివరి_valid)
{
    buf_T	*curbuf_save;
    win_T	*curwin_save;
    pos_T	cursor_save;
    పూర్ణాంకం		idx;
    linenr_T	lnum;
    linenr_T	చివర_lnum;
    linenr_T	break_lnum;
    పూర్ణాంకం		had_sync_బిందువు;
    stateitem_T	*cur_si;
    synpat_T	*spp;
    పూర్ణాంకం		దొరికింది_flags = 0;
    పూర్ణాంకం		దొరికింది_సరిపోలిక_idx = 0;
    linenr_T	దొరికింది_current_lnum = 0;
    పూర్ణాంకం		దొరికింది_current_నిలువు = 0;
    lpos_T	దొరికింది_m_endpos;
    colnr_T	prev_current_నిలువు;

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
    ఒకవేళ (syn_ఖండం->b_syn_sync_minlines > ప్రారంభం_lnum)
	ప్రారంభం_lnum = 1;
    లేకపోతే
    {
	ఒకవేళ (syn_ఖండం->b_syn_sync_minlines == 1)
	    lnum = 1;
	లేకపోతే ఒకవేళ (syn_ఖండం->b_syn_sync_minlines < 10)
	    lnum = syn_ఖండం->b_syn_sync_minlines * 2;
	లేకపోతే
	    lnum = syn_ఖండం->b_syn_sync_minlines * 3 / 2;
	ఒకవేళ (syn_ఖండం->b_syn_sync_maxlines != 0
				     && lnum > syn_ఖండం->b_syn_sync_maxlines)
	    lnum = syn_ఖండం->b_syn_sync_maxlines;
	ఒకవేళ (lnum >= ప్రారంభం_lnum)
	    ప్రారంభం_lnum = 1;
	లేకపోతే
	    ప్రారంభం_lnum -= lnum;
    }
    current_lnum = ప్రారంభం_lnum;

    /*
     * 1. Search backwards for the end of a C-style comment.
     */
    ఒకవేళ (syn_ఖండం->b_syn_sync_flags & SF_CCOMMENT)
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
	కొరకు ( ; ప్రారంభం_lnum > 1; --ప్రారంభం_lnum)
	{
	    char_u	*l = ml_get(ప్రారంభం_lnum - 1);

	    ఒకవేళ (*l == NUL || *(l + ml_get_len(ప్రారంభం_lnum - 1) - 1) != '\\')
		ఆపు;
	}
	current_lnum = ప్రారంభం_lnum;

	// set cursor to start of search
	cursor_save = wp->w_cursor;
	wp->w_cursor.lnum = ప్రారంభం_lnum;
	wp->w_cursor.నిలువు = 0;

	/*
	 * If the line is inside a comment, need to find the syntax item that
	 * defines the comment.
	 * Restrict the search for the end of a comment to b_syn_sync_maxlines.
	 */
	ఒకవేళ (కనుగొను_ప్రారంభం_comment((పూర్ణాంకం)syn_ఖండం->b_syn_sync_maxlines) != ఖాళీ)
	{
	    కొరకు (idx = syn_ఖండం->b_syn_patterns.ga_len; --idx >= 0; )
		ఒకవేళ (SYN_వస్తువులు(syn_ఖండం)[idx].sp_syn.id
						   == syn_ఖండం->b_syn_sync_id
			&& SYN_వస్తువులు(syn_ఖండం)[idx].sp_type == SPTYPE_ప్రారంభం)
		{
		    validate_current_state();
		    ఒకవేళ (నెట్టు_current_state(idx) == OK)
			update_si_attr(current_state.ga_len - 1);
		    ఆపు;
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
    లేకపోతే ఒకవేళ (syn_ఖండం->b_syn_sync_flags & SF_సరిపోలిక)
    {
	ఒకవేళ (syn_ఖండం->b_syn_sync_maxlines != 0
			       && ప్రారంభం_lnum > syn_ఖండం->b_syn_sync_maxlines)
	    break_lnum = ప్రారంభం_lnum - syn_ఖండం->b_syn_sync_maxlines;
	లేకపోతే
	    break_lnum = 0;

	దొరికింది_m_endpos.lnum = 0;
	దొరికింది_m_endpos.నిలువు = 0;
	చివర_lnum = ప్రారంభం_lnum;
	lnum = ప్రారంభం_lnum;
	అయితే (--lnum > break_lnum)
	{
	    // This can take a long time: break when CTRL-C pressed.
	    పంక్తి_breakcheck();
	    ఒకవేళ (got_int)
	    {
		invalidate_current_state();
		current_lnum = ప్రారంభం_lnum;
		ఆపు;
	    }

	    // Check if we have run into a valid saved state stack now.
	    ఒకవేళ (చివరి_valid != ఖాళీ && lnum == చివరి_valid->sst_lnum)
	    {
		ఎక్కించు_current_state(చివరి_valid);
		ఆపు;
	    }

	    /*
	     * Check if the previous line has the line-continuation pattern.
	     */
	    ఒకవేళ (lnum > 1 && syn_సరిపోలిక_linecont(lnum - 1))
		కొనసాగు;

	    /*
	     * Start with nothing on the state stack
	     */
	    validate_current_state();

	    కొరకు (current_lnum = lnum; current_lnum < చివర_lnum; ++current_lnum)
	    {
		syn_ప్రారంభం_పంక్తి();
		కొరకు (;;)
		{
		    had_sync_బిందువు = syn_ముగింపు_పంక్తి(TRUE);
		    /*
		     * When a sync point has been found, remember where, and
		     * continue to look for another one, further on in the line.
		     */
		    ఒకవేళ (had_sync_బిందువు && current_state.ga_len)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			ఒకవేళ (cur_si->si_m_endpos.lnum > ప్రారంభం_lnum)
			{
			    // ignore match that goes to after where started
			    current_lnum = చివర_lnum;
			    ఆపు;
			}
			ఒకవేళ (cur_si->si_idx < 0)
			{
			    // Cannot happen?
			    దొరికింది_flags = 0;
			    దొరికింది_సరిపోలిక_idx = KEYWORD_IDX;
			}
			లేకపోతే
			{
			    spp = &(SYN_వస్తువులు(syn_ఖండం)[cur_si->si_idx]);
			    దొరికింది_flags = spp->sp_flags;
			    దొరికింది_సరిపోలిక_idx = spp->sp_sync_idx;
			}
			దొరికింది_current_lnum = current_lnum;
			దొరికింది_current_నిలువు = current_నిలువు;
			దొరికింది_m_endpos = cur_si->si_m_endpos;
			/*
			 * Continue after the match (be aware of a zero-length
			 * match).
			 */
			ఒకవేళ (దొరికింది_m_endpos.lnum > current_lnum)
			{
			    current_lnum = దొరికింది_m_endpos.lnum;
			    current_నిలువు = దొరికింది_m_endpos.నిలువు;
			    ఒకవేళ (current_lnum >= చివర_lnum)
				ఆపు;
			}
			లేకపోతే ఒకవేళ (దొరికింది_m_endpos.నిలువు > current_నిలువు)
			    current_నిలువు = దొరికింది_m_endpos.నిలువు;
			లేకపోతే
			    ++current_నిలువు;

			// syn_current_attr() will have skipped the check for
			// an item that ends here, need to do that now.  Be
			// careful not to go past the NUL.
			prev_current_నిలువు = current_నిలువు;
			ఒకవేళ (syn_getcurline()[current_నిలువు] != NUL)
			    ++current_నిలువు;
			check_state_చివర్లు();
			current_నిలువు = prev_current_నిలువు;
		    }
		    లేకపోతే
			ఆపు;
		}
	    }

	    /*
	     * If a sync point was encountered, break here.
	     */
	    ఒకవేళ (దొరికింది_flags)
	    {
		/*
		 * Put the item that was specified by the sync point on the
		 * state stack.  If there was no item specified, make the
		 * state stack empty.
		 */
		clear_current_state();
		ఒకవేళ (దొరికింది_సరిపోలిక_idx >= 0
			&& నెట్టు_current_state(దొరికింది_సరిపోలిక_idx) == OK)
		    update_si_attr(current_state.ga_len - 1);

		/*
		 * When using "grouphere", continue from the sync point
		 * match, until the end of the line.  Parsing starts at
		 * the next line.
		 * For "groupthere" the parsing starts at start_lnum.
		 */
		ఒకవేళ (దొరికింది_flags & HL_SYNC_ఇక్కడ)
		{
		    current_lnum = దొరికింది_m_endpos.lnum;
		    current_నిలువు = దొరికింది_m_endpos.నిలువు;
		    ఒకవేళ (current_state.ga_len)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			cur_si->si_h_startpos.lnum = దొరికింది_current_lnum;
			cur_si->si_h_startpos.నిలువు = దొరికింది_current_నిలువు;
			update_si_చివర(cur_si, (పూర్ణాంకం)current_నిలువు, TRUE);
			check_keepend();
		    }
		    (శూన్య)syn_ముగింపు_పంక్తి(FALSE);
		    ++current_lnum;
		}
		లేకపోతే
		    current_lnum = ప్రారంభం_lnum;

		ఆపు;
	    }

	    చివర_lnum = lnum;
	    invalidate_current_state();
	}

	// Ran into start of the file or exceeded maximum number of lines
	ఒకవేళ (lnum <= break_lnum)
	{
	    invalidate_current_state();
	    current_lnum = break_lnum + 1;
	}
    }

    validate_current_state();
}

    స్థిర శూన్య
save_chartab(char_u *chartab)
{
    ఒకవేళ (syn_ఖండం->b_syn_isk == ఖాళీలు_option)
	తిరిగి;

    mch_memmove(chartab, syn_buf->b_chartab, (size_t)32);
    mch_memmove(syn_buf->b_chartab, syn_win->w_s->b_syn_chartab, (size_t)32);
}

    స్థిర శూన్య
restore_chartab(char_u *chartab)
{
    ఒకవేళ (syn_win->w_s->b_syn_isk != ఖాళీలు_option)
	mch_memmove(syn_buf->b_chartab, chartab, (size_t)32);
}

/*
 * Return TRUE if the line-continuation pattern matches in line "lnum".
 */
    స్థిర పూర్ణాంకం
syn_సరిపోలిక_linecont(linenr_T lnum)
{
    regmmatch_T regmatch;
    పూర్ణాంకం r;
    char_u	buf_chartab[32];  // chartab array for syn iskyeyword

    ఒకవేళ (syn_ఖండం->b_syn_linecont_prog == ఖాళీ)
	తిరిగి FALSE;

    // use syntax iskeyword option
    save_chartab(buf_chartab);
    regmatch.rmm_ic = syn_ఖండం->b_syn_linecont_ic;
    regmatch.regprog = syn_ఖండం->b_syn_linecont_prog;
    r = syn_regexec(&regmatch, lnum, (colnr_T)0,
	    IF_SYN_సమయం(&syn_ఖండం->b_syn_linecont_సమయం));
    syn_ఖండం->b_syn_linecont_prog = regmatch.regprog;
    restore_chartab(buf_chartab);
    తిరిగి r;
}

/*
 * Prepare the current state for the start of a line.
 */
    స్థిర శూన్య
syn_ప్రారంభం_పంక్తి(శూన్య)
{
    current_ముగిసిన = FALSE;
    current_నిలువు = 0;

    /*
     * Need to update the end of a start/skip/end that continues from the
     * previous line and regions that have "keepend".
     */
    ఒకవేళ (current_state.ga_len > 0)
    {
	syn_update_చివర్లు(TRUE);
	check_state_చివర్లు();
    }

    తదుపరి_సరిపోలిక_idx = -1;
    ++current_పంక్తి_id;
#నిర్వచితమైతే FEAT_CONCEAL
    తదుపరి_seqnr = 1;
#అయితే_ముగింపు
}

/*
 * Check for items in the stack that need their end updated.
 * When "startofline" is TRUE the last item is always updated.
 * When "startofline" is FALSE the item with "keepend" is forcefully updated.
 */
    స్థిర శూన్య
syn_update_చివర్లు(పూర్ణాంకం startofline)
{
    stateitem_T	*cur_si;
    పూర్ణాంకం		i;
    పూర్ణాంకం		చూసిన_keepend;

    ఒకవేళ (startofline)
    {
	// Check for a match carried over from a previous line with a
	// contained region.  The match ends as soon as the region ends.
	కొరకు (i = 0; i < current_state.ga_len; ++i)
	{
	    cur_si = &CUR_STATE(i);
	    ఒకవేళ (cur_si->si_idx >= 0
		    && (SYN_వస్తువులు(syn_ఖండం)[cur_si->si_idx]).sp_type
							       == SPTYPE_సరిపోలిక
		    && cur_si->si_m_endpos.lnum < current_lnum)
	    {
		cur_si->si_flags |= HL_MATCHCONT;
		cur_si->si_m_endpos.lnum = 0;
		cur_si->si_m_endpos.నిలువు = 0;
		cur_si->si_h_endpos = cur_si->si_m_endpos;
		cur_si->si_చివర్లు = TRUE;
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
    ఒకవేళ (keepend_level >= 0)
	కొరకు ( ; i > keepend_level; --i)
	    ఒకవేళ (CUR_STATE(i).si_flags & HL_EXTEND)
		ఆపు;

    చూసిన_keepend = FALSE;
    కొరకు ( ; i < current_state.ga_len; ++i)
    {
	cur_si = &CUR_STATE(i);
	ఒకవేళ ((cur_si->si_flags & HL_KEEPEND)
			    || (చూసిన_keepend && !startofline)
			    || (i == current_state.ga_len - 1 && startofline))
	{
	    cur_si->si_h_startpos.నిలువు = 0;	// start highl. in col 0
	    cur_si->si_h_startpos.lnum = current_lnum;

	    ఒకవేళ (!(cur_si->si_flags & HL_MATCHCONT))
		update_si_చివర(cur_si, (పూర్ణాంకం)current_నిలువు, !startofline);

	    ఒకవేళ (!startofline && (cur_si->si_flags & HL_KEEPEND))
		చూసిన_keepend = TRUE;
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

    స్థిర శూన్య
syn_దొంతర_free_ఖండం(synblock_T *ఖండం)
{
    synstate_T	*p;

    // Syntax definitions may have changed: drop the in_id_list() cache.
    idl_cache_clear(ఖండం);

    ఒకవేళ (ఖండం->b_sst_array == ఖాళీ)
	తిరిగి;

    FOR_ALL_SYNSTATES(ఖండం, p)
	clear_syn_state(p);
    VIM_CLEAR(ఖండం->b_sst_array);
    ఖండం->b_sst_మొదటి = ఖాళీ;
    ఖండం->b_sst_శోధన = ఖాళీ;
    ఖండం->b_sst_len = 0;
}
/*
 * Free b_sst_array[] for buffer "buf".
 * Used when syntax items changed to force resyncing everywhere.
 */
    శూన్య
syn_దొంతర_free_all(synblock_T *ఖండం)
{
#నిర్వచితమైతే FEAT_FOLDING
    win_T	*wp;
#అయితే_ముగింపు

    syn_దొంతర_free_ఖండం(ఖండం);

#నిర్వచితమైతే FEAT_FOLDING
    // When using "syntax" fold method, must update all folds.
    FOR_ALL_WINDOWS(wp)
    {
	ఒకవేళ (wp->w_s == ఖండం && foldmethodIsSyntax(wp))
	    foldUpdateAll(wp);
    }
#అయితే_ముగింపు
}

/*
 * Allocate the syntax state stack for syn_buf when needed.
 * If the number of entries in b_sst_array[] is much too big or a bit too
 * small, reallocate it.
 * Also used to allocate b_sst_array[] for the first time.
 */
    స్థిర శూన్య
syn_దొంతర_alloc(శూన్య)
{
    దీర్ఘ	len;
    synstate_T	*గమ్యం, *from;
    synstate_T	*sstp;

    len = syn_buf->b_ml.ml_పంక్తి_లెక్క / SST_DIST + అడ్డువరుసలు * 2;
    ఒకవేళ (len < SST_MIN_ENTRIES)
	len = SST_MIN_ENTRIES;
    లేకపోతే ఒకవేళ (len > SST_MAX_ENTRIES)
	len = SST_MAX_ENTRIES;
    ఒకవేళ (syn_ఖండం->b_sst_len > len * 2 || syn_ఖండం->b_sst_len < len)
    {
	// Allocate 50% too much, to avoid reallocating too often.
	len = syn_buf->b_ml.ml_పంక్తి_లెక్క;
	len = (len + len / 2) / SST_DIST + అడ్డువరుసలు * 2;
	ఒకవేళ (len < SST_MIN_ENTRIES)
	    len = SST_MIN_ENTRIES;
	లేకపోతే ఒకవేళ (len > SST_MAX_ENTRIES)
	    len = SST_MAX_ENTRIES;

	ఒకవేళ (syn_ఖండం->b_sst_array != ఖాళీ)
	{
	    // When shrinking the array, cleanup the existing stack.
	    // Make sure that all valid entries fit in the new array.
	    అయితే (syn_ఖండం->b_sst_len - syn_ఖండం->b_sst_freecount + 2 > len
		    && syn_దొంతర_cleanup())
		;
	    ఒకవేళ (len < syn_ఖండం->b_sst_len - syn_ఖండం->b_sst_freecount + 2)
		len = syn_ఖండం->b_sst_len - syn_ఖండం->b_sst_freecount + 2;
	}

	sstp = ALLOC_CLEAR_MULT(synstate_T, len);
	ఒకవేళ (sstp == ఖాళీ)	// out of memory!
	    తిరిగి;

	గమ్యం = sstp - 1;
	ఒకవేళ (syn_ఖండం->b_sst_array != ఖాళీ)
	{
	    // Move the states from the old array to the new one.
	    కొరకు (from = syn_ఖండం->b_sst_మొదటి; from != ఖాళీ;
							from = from->sst_తదుపరి)
	    {
		++గమ్యం;
		*గమ్యం = *from;
		గమ్యం->sst_తదుపరి = గమ్యం + 1;
	    }
	}
	ఒకవేళ (గమ్యం != sstp - 1)
	{
	    గమ్యం->sst_తదుపరి = ఖాళీ;
	    syn_ఖండం->b_sst_మొదటి = sstp;
	    syn_ఖండం->b_sst_freecount = len - (పూర్ణాంకం)(గమ్యం - sstp) - 1;
	}
	లేకపోతే
	{
	    syn_ఖండం->b_sst_మొదటి = ఖాళీ;
	    syn_ఖండం->b_sst_freecount = len;
	}

	// Create the list of free entries.
	syn_ఖండం->b_sst_firstfree = గమ్యం + 1;
	అయితే (++గమ్యం < sstp + len)
	    గమ్యం->sst_తదుపరి = గమ్యం + 1;
	(sstp + len - 1)->sst_తదుపరి = ఖాళీ;

	vim_free(syn_ఖండం->b_sst_array);
	syn_ఖండం->b_sst_array = sstp;
	syn_ఖండం->b_sst_len = len;
	// The entries were moved to a new array, drop the stale pointer.
	syn_ఖండం->b_sst_శోధన = ఖాళీ;
    }
}

/*
 * Check for changes in a buffer to affect stored syntax states.  Uses the
 * b_mod_* fields.
 * Called from update_screen(), before screen is being updated, once for each
 * displayed buffer.
 */
    శూన్య
syn_దొంతర_apply_changes(buf_T *buf)
{
    win_T	*wp;

    syn_దొంతర_apply_changes_ఖండం(&buf->b_s, buf);

    FOR_ALL_WINDOWS(wp)
    {
	ఒకవేళ ((wp->w_బఫరు == buf) && (wp->w_s != &buf->b_s))
	    syn_దొంతర_apply_changes_ఖండం(wp->w_s, buf);
    }
}

    స్థిర శూన్య
syn_దొంతర_apply_changes_ఖండం(synblock_T *ఖండం, buf_T *buf)
{
    synstate_T	*p, *prev, *np;
    linenr_T	n;

    prev = ఖాళీ;
    కొరకు (p = ఖండం->b_sst_మొదటి; p != ఖాళీ; )
    {
	ఒకవేళ (p->sst_lnum + ఖండం->b_syn_sync_linebreaks > buf->b_mod_శిఖరం)
	{
	    n = p->sst_lnum + buf->b_mod_xlines;
	    ఒకవేళ (n <= buf->b_mod_bot)
	    {
		// this state is inside the changed area, remove it
		np = p->sst_తదుపరి;
		ఒకవేళ (prev == ఖాళీ)
		    ఖండం->b_sst_మొదటి = np;
		లేకపోతే
		    prev->sst_తదుపరి = np;
		syn_దొంతర_free_నమోదు(ఖండం, p);
		p = np;
		కొనసాగు;
	    }
	    // This state is below the changed area.  Remember the line
	    // that needs to be parsed before this entry can be made valid
	    // again.
	    ఒకవేళ (p->sst_change_lnum != 0 && p->sst_change_lnum > buf->b_mod_శిఖరం)
	    {
		ఒకవేళ (p->sst_change_lnum + buf->b_mod_xlines > buf->b_mod_శిఖరం)
		    p->sst_change_lnum += buf->b_mod_xlines;
		లేకపోతే
		    p->sst_change_lnum = buf->b_mod_శిఖరం;
	    }
	    ఒకవేళ (p->sst_change_lnum == 0
		    || p->sst_change_lnum < buf->b_mod_bot)
		p->sst_change_lnum = buf->b_mod_bot;

	    p->sst_lnum = n;
	}
	prev = p;
	p = p->sst_తదుపరి;
    }
}

/*
 * Reduce the number of entries in the state stack for syn_buf.
 * Returns TRUE if at least one entry was freed.
 */
    స్థిర పూర్ణాంకం
syn_దొంతర_cleanup(శూన్య)
{
    synstate_T	*p, *prev;
    disptick_T	tick;
    పూర్ణాంకం		above;
    పూర్ణాంకం		dist;
    పూర్ణాంకం		retval = FALSE;

    ఒకవేళ (syn_ఖండం->b_sst_మొదటి == ఖాళీ)
	తిరిగి retval;

    // Compute normal distance between non-displayed entries.
    ఒకవేళ (syn_ఖండం->b_sst_len <= అడ్డువరుసలు)
	dist = 999999;
    లేకపోతే
	dist = syn_buf->b_ml.ml_పంక్తి_లెక్క / (syn_ఖండం->b_sst_len - అడ్డువరుసలు) + 1;

    /*
     * Go through the list to find the "tick" for the oldest entry that can
     * be removed.  Set "above" when the "tick" for the oldest entry is above
     * "b_sst_lasttick" (the display tick wraps around).
     */
    tick = syn_ఖండం->b_sst_lasttick;
    above = FALSE;
    prev = syn_ఖండం->b_sst_మొదటి;
    కొరకు (p = prev->sst_తదుపరి; p != ఖాళీ; prev = p, p = p->sst_తదుపరి)
    {
	ఒకవేళ (prev->sst_lnum + dist > p->sst_lnum)
	{
	    ఒకవేళ (p->sst_tick > syn_ఖండం->b_sst_lasttick)
	    {
		ఒకవేళ (!above || p->sst_tick < tick)
		    tick = p->sst_tick;
		above = TRUE;
	    }
	    లేకపోతే ఒకవేళ (!above && p->sst_tick < tick)
		tick = p->sst_tick;
	}
    }

    /*
     * Go through the list to make the entries for the oldest tick at an
     * interval of several lines.
     */
    prev = syn_ఖండం->b_sst_మొదటి;
    కొరకు (p = prev->sst_తదుపరి; p != ఖాళీ; prev = p, p = p->sst_తదుపరి)
    {
	ఒకవేళ (p->sst_tick == tick && prev->sst_lnum + dist > p->sst_lnum)
	{
	    // Move this entry from used list to free list
	    prev->sst_తదుపరి = p->sst_తదుపరి;
	    syn_దొంతర_free_నమోదు(syn_ఖండం, p);
	    p = prev;
	    retval = TRUE;
	}
    }
    తిరిగి retval;
}

/*
 * Free the allocated memory for a syn_state item.
 * Move the entry into the free list.
 */
    స్థిర శూన్య
syn_దొంతర_free_నమోదు(synblock_T *ఖండం, synstate_T *p)
{
    ఒకవేళ (ఖండం->b_sst_శోధన == p)
	ఖండం->b_sst_శోధన = ఖాళీ;
    clear_syn_state(p);
    p->sst_తదుపరి = ఖండం->b_sst_firstfree;
    ఖండం->b_sst_firstfree = p;
    ++ఖండం->b_sst_freecount;
}

/*
 * Find an entry in the list of state stacks at or before "lnum".
 * Returns NULL when there is no entry or the first entry is after "lnum".
 */
    స్థిర synstate_T *
syn_దొంతర_కనుగొను_నమోదు(linenr_T lnum)
{
    synstate_T	*p, *prev;

    prev = ఖాళీ;
    p = syn_ఖండం->b_sst_మొదటి;

    // The list is sorted by line number and lookups while parsing advance
    // monotonically, so resume from the last returned entry instead of
    // rescanning from the start whenever it is at or before "lnum".
    ఒకవేళ (syn_ఖండం->b_sst_శోధన != ఖాళీ
				 && syn_ఖండం->b_sst_శోధన->sst_lnum <= lnum)
	p = syn_ఖండం->b_sst_శోధన;

    కొరకు ( ; p != ఖాళీ; prev = p, p = p->sst_తదుపరి)
    {
	ఒకవేళ (p->sst_lnum == lnum)
	{
	    syn_ఖండం->b_sst_శోధన = p;
	    తిరిగి p;
	}
	ఒకవేళ (p->sst_lnum > lnum)
	    ఆపు;
    }
    ఒకవేళ (prev != ఖాళీ)
	syn_ఖండం->b_sst_శోధన = prev;
    తిరిగి prev;
}

/*
 * Try saving the current state in b_sst_array[].
 * The current state must be valid for the start of the current_lnum line!
 */
    స్థిర synstate_T *
store_current_state(శూన్య)
{
    పూర్ణాంకం		i;
    synstate_T	*p;
    bufstate_T	*bp;
    stateitem_T	*cur_si;
    synstate_T	*sp = syn_దొంతర_కనుగొను_నమోదు(current_lnum);

    /*
     * If the current state contains a start or end pattern that continues
     * from the previous line, we can't use it.  Don't store it then.
     */
    కొరకు (i = current_state.ga_len - 1; i >= 0; --i)
    {
	cur_si = &CUR_STATE(i);
	ఒకవేళ (cur_si->si_h_startpos.lnum >= current_lnum
		|| cur_si->si_m_endpos.lnum >= current_lnum
		|| cur_si->si_h_endpos.lnum >= current_lnum
		|| (cur_si->si_చివర_idx
		    && cur_si->si_eoe_pos.lnum >= current_lnum))
	    ఆపు;
    }
    ఒకవేళ (i >= 0)
    {
	ఒకవేళ (sp != ఖాళీ)
	{
	    // find "sp" in the list and remove it
	    ఒకవేళ (syn_ఖండం->b_sst_మొదటి == sp)
		// it's the first entry
		syn_ఖండం->b_sst_మొదటి = sp->sst_తదుపరి;
	    లేకపోతే
	    {
		// find the entry just before this one to adjust sst_next
		FOR_ALL_SYNSTATES(syn_ఖండం, p)
		    ఒకవేళ (p->sst_తదుపరి == sp)
			ఆపు;
		ఒకవేళ (p != ఖాళీ)	// just in case
		    p->sst_తదుపరి = sp->sst_తదుపరి;
	    }
	    syn_దొంతర_free_నమోదు(syn_ఖండం, sp);
	    sp = ఖాళీ;
	}
    }
    లేకపోతే ఒకవేళ (sp == ఖాళీ || sp->sst_lnum != current_lnum)
    {
	/*
	 * Add a new entry
	 */
	// If no free items, cleanup the array first.
	ఒకవేళ (syn_ఖండం->b_sst_freecount == 0)
	{
	    (శూన్య)syn_దొంతర_cleanup();
	    // "sp" may have been moved to the freelist now
	    sp = syn_దొంతర_కనుగొను_నమోదు(current_lnum);
	}
	// Still no free items?  Must be a strange problem...
	ఒకవేళ (syn_ఖండం->b_sst_freecount == 0)
	    sp = ఖాళీ;
	లేకపోతే
	{
	    // Take the first item from the free list and put it in the used
	    // list, after *sp
	    p = syn_ఖండం->b_sst_firstfree;
	    syn_ఖండం->b_sst_firstfree = p->sst_తదుపరి;
	    --syn_ఖండం->b_sst_freecount;
	    ఒకవేళ (sp == ఖాళీ)
	    {
		// Insert in front of the list
		p->sst_తదుపరి = syn_ఖండం->b_sst_మొదటి;
		syn_ఖండం->b_sst_మొదటి = p;
	    }
	    లేకపోతే
	    {
		// insert in list after *sp
		p->sst_తదుపరి = sp->sst_తదుపరి;
		sp->sst_తదుపరి = p;
	    }
	    sp = p;
	    sp->sst_stacksize = 0;
	    sp->sst_lnum = current_lnum;
	    // Resume the next forward lookup from the entry just stored.
	    syn_ఖండం->b_sst_శోధన = sp;
	}
    }
    ఒకవేళ (sp != ఖాళీ)
    {
	// When overwriting an existing state stack, clear it first
	clear_syn_state(sp);
	sp->sst_stacksize = current_state.ga_len;
	ఒకవేళ (current_state.ga_len > SST_FIX_స్థితులు)
	{
	    // Need to clear it, might be something remaining from when the
	    // length was less than SST_FIX_STATES.
	    ga_init2(&sp->sst_union.sst_ga, పరిమాణం(bufstate_T), 1);
	    ఒకవేళ (ga_grow(&sp->sst_union.sst_ga, current_state.ga_len) == FAIL)
		sp->sst_stacksize = 0;
	    లేకపోతే
		sp->sst_union.sst_ga.ga_len = current_state.ga_len;
	    bp = SYN_STATE_P(&(sp->sst_union.sst_ga));
	}
	లేకపోతే
	    bp = sp->sst_union.sst_దొంతర;
	కొరకు (i = 0; i < sp->sst_stacksize; ++i)
	{
	    bp[i].bs_idx = CUR_STATE(i).si_idx;
	    bp[i].bs_flags = CUR_STATE(i).si_flags;
#నిర్వచితమైతే FEAT_CONCEAL
	    bp[i].bs_seqnr = CUR_STATE(i).si_seqnr;
	    bp[i].bs_cchar = CUR_STATE(i).si_cchar;
#అయితే_ముగింపు
	    bp[i].bs_extmatch = ref_extmatch(CUR_STATE(i).si_extmatch);
	}
	sp->sst_తదుపరి_flags = current_తదుపరి_flags;
	sp->sst_తదుపరి_list = current_తదుపరి_list;
	sp->sst_tick = display_tick;
	sp->sst_change_lnum = 0;
    }
    current_state_stored = TRUE;
    తిరిగి sp;
}

/*
 * Copy a state stack from "from" in b_sst_array[] to current_state;
 */
    స్థిర శూన్య
ఎక్కించు_current_state(synstate_T *from)
{
    పూర్ణాంకం		i;
    bufstate_T	*bp;

    clear_current_state();
    validate_current_state();
    keepend_level = -1;
    ఒకవేళ (from->sst_stacksize
	    && ga_grow(&current_state, from->sst_stacksize) == OK)
    {
	ఒకవేళ (from->sst_stacksize > SST_FIX_స్థితులు)
	    bp = SYN_STATE_P(&(from->sst_union.sst_ga));
	లేకపోతే
	    bp = from->sst_union.sst_దొంతర;
	కొరకు (i = 0; i < from->sst_stacksize; ++i)
	{
	    CUR_STATE(i).si_idx = bp[i].bs_idx;
	    CUR_STATE(i).si_flags = bp[i].bs_flags;
#నిర్వచితమైతే FEAT_CONCEAL
	    CUR_STATE(i).si_seqnr = bp[i].bs_seqnr;
	    CUR_STATE(i).si_cchar = bp[i].bs_cchar;
#అయితే_ముగింపు
	    CUR_STATE(i).si_extmatch = ref_extmatch(bp[i].bs_extmatch);
	    ఒకవేళ (keepend_level < 0 && (CUR_STATE(i).si_flags & HL_KEEPEND))
		keepend_level = i;
	    CUR_STATE(i).si_చివర్లు = FALSE;
	    CUR_STATE(i).si_m_lnum = 0;
	    ఒకవేళ (CUR_STATE(i).si_idx >= 0)
		CUR_STATE(i).si_తదుపరి_list =
		     (SYN_వస్తువులు(syn_ఖండం)[CUR_STATE(i).si_idx]).sp_తదుపరి_list;
	    లేకపోతే
		CUR_STATE(i).si_తదుపరి_list = ఖాళీ;
	    update_si_attr(i);
	}
	current_state.ga_len = from->sst_stacksize;
    }
    current_తదుపరి_list = from->sst_తదుపరి_list;
    current_తదుపరి_flags = from->sst_తదుపరి_flags;
    current_lnum = from->sst_lnum;
}

/*
 * Compare saved state stack "*sp" with the current state.
 * Return TRUE when they are equal.
 */
    స్థిర పూర్ణాంకం
syn_దొంతర_equal(synstate_T *sp)
{
    పూర్ణాంకం		i, j;
    bufstate_T	*bp;
    reg_extmatch_T	*six, *bsx;

    // First a quick check if the stacks have the same size end nextlist.
    ఒకవేళ (sp->sst_stacksize != current_state.ga_len
	    || sp->sst_తదుపరి_list != current_తదుపరి_list)
	తిరిగి FALSE;

    // Need to compare all states on both stacks.
    ఒకవేళ (sp->sst_stacksize > SST_FIX_స్థితులు)
	bp = SYN_STATE_P(&(sp->sst_union.sst_ga));
    లేకపోతే
	bp = sp->sst_union.sst_దొంతర;

    కొరకు (i = current_state.ga_len; --i >= 0; )
    {
	// If the item has another index the state is different.
	ఒకవేళ (bp[i].bs_idx != CUR_STATE(i).si_idx)
	    ఆపు;
	ఒకవేళ (bp[i].bs_extmatch == CUR_STATE(i).si_extmatch)
	    కొనసాగు;
	// When the extmatch pointers are different, the strings in them can
	// still be the same.  Check if the extmatch references are equal.
	bsx = bp[i].bs_extmatch;
	six = CUR_STATE(i).si_extmatch;
	// If one of the extmatch pointers is NULL the states are different.
	ఒకవేళ (bsx == ఖాళీ || six == ఖాళీ)
	    ఆపు;
	కొరకు (j = 0; j < NSUBEXP; ++j)
	{
	    // Check each referenced match string. They must all be equal.
	    ఒకవేళ (bsx->matches[j] != six->matches[j])
	    {
		// If the pointer is different it can still be the same text.
		// Compare the strings, ignore case when the start item has the
		// sp_ic flag set.
		ఒకవేళ (bsx->matches[j] == ఖాళీ || six->matches[j] == ఖాళీ)
		    ఆపు;
		ఒకవేళ ((SYN_వస్తువులు(syn_ఖండం)[CUR_STATE(i).si_idx]).sp_ic
			? MB_STRICMP(bsx->matches[j], six->matches[j]) != 0
			: STRCMP(bsx->matches[j], six->matches[j]) != 0)
		    ఆపు;
	    }
	}
	ఒకవేళ (j != NSUBEXP)
	    ఆపు;
    }
    తిరిగి i < 0 ? TRUE : FALSE;
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
    శూన్య
syntax_చివర_parsing(win_T *wp, linenr_T lnum)
{
    synstate_T	*sp;

    ఒకవేళ (syn_ఖండం != wp->w_s)
	తిరిగి;  // not the right window
    sp = syn_దొంతర_కనుగొను_నమోదు(lnum);
    ఒకవేళ (sp != ఖాళీ && sp->sst_lnum < lnum)
	sp = sp->sst_తదుపరి;

    ఒకవేళ (sp != ఖాళీ && sp->sst_change_lnum != 0)
	sp->sst_change_lnum = lnum;
}

/*
 * End of handling of the state stack.
 ****************************************/

    స్థిర శూన్య
invalidate_current_state(శూన్య)
{
    clear_current_state();
    current_state.ga_itemsize = 0;	// mark current_state invalid
    current_తదుపరి_list = ఖాళీ;
    keepend_level = -1;
}

    స్థిర శూన్య
validate_current_state(శూన్య)
{
    current_state.ga_itemsize = పరిమాణం(stateitem_T);
    current_state.ga_growsize = 3;
}

/*
 * Return TRUE if the syntax at start of lnum changed since last time.
 * This will only be called just after get_syntax_attr() for the previous
 * line, to check if the next line needs to be redrawn too.
 */
    పూర్ణాంకం
syntax_check_మారింది(linenr_T lnum)
{
    పూర్ణాంకం		retval = TRUE;
    synstate_T	*sp;

    /*
     * Check the state stack when:
     * - lnum is just below the previously syntaxed line.
     * - lnum is not before the lines with saved states.
     * - lnum is not past the lines with saved states.
     * - lnum is at or before the last changed line.
     */
    ఒకవేళ (VALID_STATE(&current_state) && lnum == current_lnum + 1)
    {
	sp = syn_దొంతర_కనుగొను_నమోదు(lnum);
	ఒకవేళ (sp != ఖాళీ && sp->sst_lnum == lnum)
	{
	    /*
	     * finish the previous line (needed when not all of the line was
	     * drawn)
	     */
	    (శూన్య)syn_ముగింపు_పంక్తి(FALSE);

	    /*
	     * Compare the current state with the previously saved state of
	     * the line.
	     */
	    ఒకవేళ (syn_దొంతర_equal(sp))
		retval = FALSE;

	    /*
	     * Store the current state in b_sst_array[] for later use.
	     */
	    ++current_lnum;
	    (శూన్య)store_current_state();
	}
    }

    తిరిగి retval;
}

/*
 * Finish the current line.
 * This doesn't return any attributes, it only gets the state at the end of
 * the line.  It can start anywhere in the line, as long as the current state
 * is valid.
 */
    స్థిర పూర్ణాంకం
syn_ముగింపు_పంక్తి(
    పూర్ణాంకం	    syncing)		// called for syncing
{
    stateitem_T	*cur_si;
    colnr_T	prev_current_నిలువు;

    అయితే (!current_ముగిసిన)
    {
	(శూన్య)syn_current_attr(syncing, FALSE, ఖాళీ, FALSE);
	/*
	 * When syncing, and found some item, need to check the item.
	 */
	ఒకవేళ (syncing && current_state.ga_len)
	{
	    /*
	     * Check for match with sync item.
	     */
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	    ఒకవేళ (cur_si->si_idx >= 0
		    && (SYN_వస్తువులు(syn_ఖండం)[cur_si->si_idx].sp_flags
					  & (HL_SYNC_ఇక్కడ|HL_SYNC_THERE)))
		తిరిగి TRUE;

	    // syn_current_attr() will have skipped the check for an item
	    // that ends here, need to do that now.  Be careful not to go
	    // past the NUL.
	    prev_current_నిలువు = current_నిలువు;
	    ఒకవేళ (syn_getcurline()[current_నిలువు] != NUL)
		++current_నిలువు;
	    check_state_చివర్లు();
	    current_నిలువు = prev_current_నిలువు;
	}
	++current_నిలువు;
    }
    తిరిగి FALSE;
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
    పూర్ణాంకం
get_syntax_attr(
    colnr_T	నిలువు,
    పూర్ణాంకం		*can_spell,
    పూర్ణాంకం		keep_state)	// keep state of char at "col"
{
    పూర్ణాంకం	    attr = 0;

    ఒకవేళ (can_spell != ఖాళీ)
	// Default: Only do spelling when there is no @Spell cluster or when
	// ":syn spell toplevel" was used.
	*can_spell = syn_ఖండం->b_syn_spell == SYNSPL_DEFAULT
		    ? (syn_ఖండం->b_spell_cluster_id == 0)
		    : (syn_ఖండం->b_syn_spell == SYNSPL_శిఖరం);

    // check for out of memory situation
    ఒకవేళ (syn_ఖండం->b_sst_array == ఖాళీ)
	తిరిగి 0;

    // After 'synmaxcol' the attribute is always zero.
    ఒకవేళ (syn_buf->b_p_smc > 0 && నిలువు >= (colnr_T)syn_buf->b_p_smc)
    {
	clear_current_state();
#నిర్వచితమైతే FEAT_EVAL
	current_id = 0;
	current_trans_id = 0;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CONCEAL
	current_flags = 0;
	current_seqnr = 0;
#అయితే_ముగింపు
	తిరిగి 0;
    }

    // Make sure current_state is valid
    ఒకవేళ (INVALID_STATE(&current_state))
	validate_current_state();

    /*
     * Skip from the current column to "col", get the attributes for "col".
     */
    అయితే (current_నిలువు <= నిలువు)
    {
	attr = syn_current_attr(FALSE, TRUE, can_spell,
				     current_నిలువు == నిలువు ? keep_state : FALSE);
	++current_నిలువు;
    }

    తిరిగి attr;
}

/*
 * Get syntax attributes for current_lnum, current_col.
 */
    స్థిర పూర్ణాంకం
syn_current_attr(
    పూర్ణాంకం		syncing,		// When 1: called for syncing
    పూర్ణాంకం		displaying,		// result will be displayed
    పూర్ణాంకం		*can_spell,		// return: do spell checking
    పూర్ణాంకం		keep_state)		// keep syntax stack afterwards
{
    పూర్ణాంకం		syn_id;
    lpos_T	endpos;		// was: char_u *endp;
    lpos_T	hl_startpos;	// was: int hl_startcol;
    lpos_T	hl_endpos;
    lpos_T	eos_pos;	// end-of-start match (start region)
    lpos_T	eoe_pos;	// end-of-end pattern
    పూర్ణాంకం		చివర_idx;	// group ID for end pattern
    పూర్ణాంకం		idx;
    synpat_T	*spp;
    stateitem_T	*cur_si, *sip = ఖాళీ;
    పూర్ణాంకం		startcol;
    పూర్ణాంకం		endcol;
    దీర్ఘ	flags;
    పూర్ణాంకం		cchar;
    పొట్టి	*తదుపరి_list;
    పూర్ణాంకం		దొరికింది_సరిపోలిక;		    // found usable match
    స్థిర పూర్ణాంకం	try_తదుపరి_స్తంభం = FALSE;    // must try in next col
    పూర్ణాంకం		do_keywords;
    regmmatch_T	regmatch;
    lpos_T	pos;
    పూర్ణాంకం		lc_నిలువు;
    reg_extmatch_T *cur_extmatch = ఖాళీ;
    char_u	buf_chartab[32];  // chartab array for syn iskyeyword
    char_u	*పంక్తి;		// current line.  NOTE: becomes invalid after
				// looking for a pattern match!

    // variables for zero-width matches that have a "nextgroup" argument
    పూర్ణాంకం		keep_తదుపరి_list;
    పూర్ణాంకం		zero_వెడల్పు_తదుపరి_list = FALSE;
    garray_T	zero_వెడల్పు_తదుపరి_ga;

    /*
     * No character, no attributes!  Past end of line?
     * Do try matching with an empty line (could be the start of a region).
     */
    పంక్తి = syn_getcurline();
    ఒకవేళ (పంక్తి[current_నిలువు] == NUL && current_నిలువు != 0)
    {
	/*
	 * If we found a match after the last column, use it.
	 */
	ఒకవేళ (తదుపరి_సరిపోలిక_idx >= 0 && తదుపరి_సరిపోలిక_నిలువు >= (పూర్ణాంకం)current_నిలువు
						  && తదుపరి_సరిపోలిక_నిలువు != MAXCOL)
	    (శూన్య)నెట్టు_తదుపరి_సరిపోలిక(ఖాళీ);

	current_ముగిసిన = TRUE;
	current_state_stored = FALSE;
	తిరిగి 0;
    }

    // if the current or next character is NUL, we will finish the line now
    ఒకవేళ (పంక్తి[current_నిలువు] == NUL || పంక్తి[current_నిలువు + 1] == NUL)
    {
	current_ముగిసిన = TRUE;
	current_state_stored = FALSE;
    }

    /*
     * When in the previous column there was a match but it could not be used
     * (empty match or already matched in this column) need to try again in
     * the next column.
     */
    ఒకవేళ (try_తదుపరి_స్తంభం)
    {
	తదుపరి_సరిపోలిక_idx = -1;
	try_తదుపరి_స్తంభం = FALSE;
    }

    // Only check for keywords when not syncing and there are some.
    do_keywords = !syncing
		    && (syn_ఖండం->b_keywtab.ht_వాడిన > 0
			    || syn_ఖండం->b_keywtab_ic.ht_వాడిన > 0);

    // Init the list of zero-width matches with a nextlist.  This is used to
    // avoid matching the same item in the same position twice.
    ga_init2(&zero_వెడల్పు_తదుపరి_ga, పరిమాణం(పూర్ణాంకం), 10);

    // use syntax iskeyword option
    save_chartab(buf_chartab);

    /*
     * Repeat matching keywords and patterns, to find contained items at the
     * same column.  This stops when there are no extra matches at the current
     * column.
     */
    చేయి
    {
	దొరికింది_సరిపోలిక = FALSE;
	keep_తదుపరి_list = FALSE;
	syn_id = 0;


	/*
	 * 1. Check for a current state.
	 *    Only when there is no current state, or if the current state may
	 *    contain other things, we need to check for keywords and patterns.
	 *    Always need to check for contained items if some item has the
	 *    "containedin" argument (takes extra time!).
	 */
	ఒకవేళ (current_state.ga_len)
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	లేకపోతే
	    cur_si = ఖాళీ;

	ఒకవేళ (syn_ఖండం->b_syn_containedin || cur_si == ఖాళీ
					      || cur_si->si_cont_list != ఖాళీ)
	{
	    /*
	     * 2. Check for keywords, if on a keyword char after a non-keyword
	     *	  char.  Don't do this when syncing.
	     */
	    ఒకవేళ (do_keywords)
	    {
	      పంక్తి = syn_getcurline();
	      ఒకవేళ (vim_iswordp_buf(పంక్తి + current_నిలువు, syn_buf)
		      && (current_నిలువు == 0
			  || !vim_iswordp_buf(పంక్తి + current_నిలువు - 1
			      - (has_mbyte
				  ? (*mb_తల_off)(పంక్తి, పంక్తి + current_నిలువు - 1)
				  : 0) , syn_buf)))
	      {
		syn_id = check_keyword_id(పంక్తి, (పూర్ణాంకం)current_నిలువు,
					 &endcol, &flags, &తదుపరి_list, cur_si,
					 &cchar);
		ఒకవేళ (syn_id != 0)
		{
		    ఒకవేళ (నెట్టు_current_state(KEYWORD_IDX) == OK)
		    {
			cur_si = &CUR_STATE(current_state.ga_len - 1);
			cur_si->si_m_startcol = current_నిలువు;
			cur_si->si_h_startpos.lnum = current_lnum;
			cur_si->si_h_startpos.నిలువు = 0;	// starts right away
			cur_si->si_m_endpos.lnum = current_lnum;
			cur_si->si_m_endpos.నిలువు = endcol;
			cur_si->si_h_endpos.lnum = current_lnum;
			cur_si->si_h_endpos.నిలువు = endcol;
			cur_si->si_చివర్లు = TRUE;
			cur_si->si_చివర_idx = 0;
			cur_si->si_flags = flags;
#నిర్వచితమైతే FEAT_CONCEAL
			cur_si->si_seqnr = తదుపరి_seqnr++;
			cur_si->si_cchar = cchar;
			ఒకవేళ (current_state.ga_len > 1)
			    cur_si->si_flags |=
				  CUR_STATE(current_state.ga_len - 2).si_flags
								 & HL_CONCEAL;
#అయితే_ముగింపు
			cur_si->si_id = syn_id;
			cur_si->si_trans_id = syn_id;
			ఒకవేళ (flags & HL_TRANSP)
			{
			    ఒకవేళ (current_state.ga_len < 2)
			    {
				cur_si->si_attr = 0;
				cur_si->si_trans_id = 0;
			    }
			    లేకపోతే
			    {
				cur_si->si_attr = CUR_STATE(
					current_state.ga_len - 2).si_attr;
				cur_si->si_trans_id = CUR_STATE(
					current_state.ga_len - 2).si_trans_id;
			    }
			}
			లేకపోతే
			    cur_si->si_attr = syn_id2attr(syn_id);
			cur_si->si_cont_list = ఖాళీ;
			cur_si->si_తదుపరి_list = తదుపరి_list;
			check_keepend();
		    }
		    లేకపోతే
			vim_free(తదుపరి_list);
		}
	      }
	    }

	    /*
	     * 3. Check for patterns (only if no keyword found).
	     */
	    ఒకవేళ (syn_id == 0 && syn_ఖండం->b_syn_patterns.ga_len)
	    {
		/*
		 * If we didn't check for a match yet, or we are past it, check
		 * for any match with a pattern.
		 */
		ఒకవేళ (తదుపరి_సరిపోలిక_idx < 0 || తదుపరి_సరిపోలిక_నిలువు < (పూర్ణాంకం)current_నిలువు)
		{
		    /*
		     * Check all relevant patterns for a match at this
		     * position.  This is complicated, because matching with a
		     * pattern takes quite a bit of time, thus we want to
		     * avoid doing it when it's not needed.
		     */
		    తదుపరి_సరిపోలిక_idx = 0;		// no match in this line yet
		    తదుపరి_సరిపోలిక_నిలువు = MAXCOL;
		    కొరకు (idx = syn_ఖండం->b_syn_patterns.ga_len; --idx >= 0; )
		    {
			spp = &(SYN_వస్తువులు(syn_ఖండం)[idx]);
			ఒకవేళ (	   spp->sp_syncing == syncing
				&& (displaying || !(spp->sp_flags & HL_DISPLAY))
				&& (spp->sp_type == SPTYPE_సరిపోలిక
				    || spp->sp_type == SPTYPE_ప్రారంభం)
				&& (current_తదుపరి_list != ఖాళీ
				    ? లోపల_id_list(ఖాళీ, current_తదుపరి_list,
							      &spp->sp_syn, 0)
				    : (cur_si == ఖాళీ
					? !(spp->sp_flags & HL_CONTAINED)
					: లోపల_id_list(cur_si,
					    cur_si->si_cont_list, &spp->sp_syn,
					    spp->sp_flags))))
			{
			    పూర్ణాంకం r;

			    // If we already tried matching in this line, and
			    // there isn't a match before next_match_col, skip
			    // this item.
			    ఒకవేళ (spp->sp_పంక్తి_id == current_పంక్తి_id
				    && spp->sp_startcol >= తదుపరి_సరిపోలిక_నిలువు)
				కొనసాగు;
			    spp->sp_పంక్తి_id = current_పంక్తి_id;

			    lc_నిలువు = current_నిలువు - spp->sp_offsets[SPO_LC_OFF];
			    ఒకవేళ (lc_నిలువు < 0)
				lc_నిలువు = 0;

			    regmatch.rmm_ic = spp->sp_ic;
			    regmatch.regprog = spp->sp_prog;
			    r = syn_regexec(&regmatch,
					     current_lnum,
					     (colnr_T)lc_నిలువు,
					     IF_SYN_సమయం(&spp->sp_సమయం));
			    spp->sp_prog = regmatch.regprog;
			    ఒకవేళ (!r)
			    {
				// no match in this line, try another one
				spp->sp_startcol = MAXCOL;
				కొనసాగు;
			    }

			    /*
			     * Compute the first column of the match.
			     */
			    syn_కూడు_ప్రారంభం_off(&pos, &regmatch,
							 spp, SPO_MS_OFF, -1);
			    ఒకవేళ (pos.lnum > current_lnum)
			    {
				// must have used end of match in a next line,
				// we can't handle that
				spp->sp_startcol = MAXCOL;
				కొనసాగు;
			    }
			    startcol = pos.నిలువు;

			    // remember the next column where this pattern
			    // matches in the current line
			    spp->sp_startcol = startcol;

			    /*
			     * If a previously found match starts at a lower
			     * column number, don't use this one.
			     */
			    ఒకవేళ (startcol >= తదుపరి_సరిపోలిక_నిలువు)
				కొనసాగు;

			    /*
			     * If we matched this pattern at this position
			     * before, skip it.  Must retry in the next
			     * column, because it may match from there.
			     */
			    ఒకవేళ (did_సరిపోలిక_already(idx, &zero_వెడల్పు_తదుపరి_ga))
			    {
				try_తదుపరి_స్తంభం = TRUE;
				కొనసాగు;
			    }

			    endpos.lnum = regmatch.endpos[0].lnum;
			    endpos.నిలువు = regmatch.endpos[0].నిలువు;

			    // Compute the highlight start.
			    syn_కూడు_ప్రారంభం_off(&hl_startpos, &regmatch,
							 spp, SPO_HS_OFF, -1);

			    // Compute the region start.
			    // Default is to use the end of the match.
			    syn_కూడు_చివర_off(&eos_pos, &regmatch,
							 spp, SPO_RS_OFF, 0);

			    /*
			     * Grab the external submatches before they get
			     * overwritten.  Reference count doesn't change.
			     */
			    unref_extmatch(cur_extmatch);
			    cur_extmatch = re_extmatch_బహిర్గతం;
			    re_extmatch_బహిర్గతం = ఖాళీ;

			    flags = 0;
			    eoe_pos.lnum = 0;	// avoid warning
			    eoe_pos.నిలువు = 0;
			    చివర_idx = 0;
			    hl_endpos.lnum = 0;

			    /*
			     * For a "oneline" the end must be found in the
			     * same line too.  Search for it after the end of
			     * the match with the start pattern.  Set the
			     * resulting end positions at the same time.
			     */
			    ఒకవేళ (spp->sp_type == SPTYPE_ప్రారంభం
					      && (spp->sp_flags & HL_ONELINE))
			    {
				lpos_T	startpos;

				startpos = endpos;
				కనుగొను_endpos(idx, &startpos, &endpos, &hl_endpos,
				    &flags, &eoe_pos, &చివర_idx, cur_extmatch);
				ఒకవేళ (endpos.lnum == 0)
				    కొనసాగు;	    // not found
			    }

			    /*
			     * For a "match" the size must be > 0 after the
			     * end offset needs has been added.  Except when
			     * syncing.
			     */
			    లేకపోతే ఒకవేళ (spp->sp_type == SPTYPE_సరిపోలిక)
			    {
				syn_కూడు_చివర_off(&hl_endpos, &regmatch, spp,
							       SPO_HE_OFF, 0);
				syn_కూడు_చివర_off(&endpos, &regmatch, spp,
							       SPO_ME_OFF, 0);
				ఒకవేళ (endpos.lnum == current_lnum
				      && (పూర్ణాంకం)endpos.నిలువు + syncing < startcol)
				{
				    /*
				     * If an empty string is matched, may need
				     * to try matching again at next column.
				     */
				    ఒకవేళ (regmatch.startpos[0].నిలువు
						    == regmatch.endpos[0].నిలువు)
					try_తదుపరి_స్తంభం = TRUE;
				    కొనసాగు;
				}
			    }

			    /*
			     * keep the best match so far in next_match_*
			     */
			    // Highlighting must start after startpos and end
			    // before endpos.
			    ఒకవేళ (hl_startpos.lnum == current_lnum
					   && (పూర్ణాంకం)hl_startpos.నిలువు < startcol)
				hl_startpos.నిలువు = startcol;
			    హద్దు_pos_zero(&hl_endpos, &endpos);

			    తదుపరి_సరిపోలిక_idx = idx;
			    తదుపరి_సరిపోలిక_నిలువు = startcol;
			    తదుపరి_సరిపోలిక_m_endpos = endpos;
			    తదుపరి_సరిపోలిక_h_endpos = hl_endpos;
			    తదుపరి_సరిపోలిక_h_startpos = hl_startpos;
			    తదుపరి_సరిపోలిక_flags = flags;
			    తదుపరి_సరిపోలిక_eos_pos = eos_pos;
			    తదుపరి_సరిపోలిక_eoe_pos = eoe_pos;
			    తదుపరి_సరిపోలిక_చివర_idx = చివర_idx;
			    unref_extmatch(తదుపరి_సరిపోలిక_extmatch);
			    తదుపరి_సరిపోలిక_extmatch = cur_extmatch;
			    cur_extmatch = ఖాళీ;
			}
		    }
		}

		/*
		 * If we found a match at the current column, use it.
		 */
		ఒకవేళ (తదుపరి_సరిపోలిక_idx >= 0 && తదుపరి_సరిపోలిక_నిలువు == (పూర్ణాంకం)current_నిలువు)
		{
		    synpat_T	*lspp;

		    // When a zero-width item matched which has a nextgroup,
		    // don't push the item but set nextgroup.
		    lspp = &(SYN_వస్తువులు(syn_ఖండం)[తదుపరి_సరిపోలిక_idx]);
		    ఒకవేళ (తదుపరి_సరిపోలిక_m_endpos.lnum == current_lnum
			    && తదుపరి_సరిపోలిక_m_endpos.నిలువు == current_నిలువు
			    && lspp->sp_తదుపరి_list != ఖాళీ)
		    {
			current_తదుపరి_list = lspp->sp_తదుపరి_list;
			current_తదుపరి_flags = lspp->sp_flags;
			keep_తదుపరి_list = TRUE;
			zero_వెడల్పు_తదుపరి_list = TRUE;

			// Add the index to a list, so that we can check
			// later that we don't match it again (and cause an
			// endless loop).
			ఒకవేళ (ga_grow(&zero_వెడల్పు_తదుపరి_ga, 1) == OK)
			{
			    ((పూర్ణాంకం *)(zero_వెడల్పు_తదుపరి_ga.ga_దత్తాంశం))
				[zero_వెడల్పు_తదుపరి_ga.ga_len++] = తదుపరి_సరిపోలిక_idx;
			}
			తదుపరి_సరిపోలిక_idx = -1;
		    }
		    లేకపోతే
			cur_si = నెట్టు_తదుపరి_సరిపోలిక(cur_si);
		    దొరికింది_సరిపోలిక = TRUE;
		}
	    }
	}

	/*
	 * Handle searching for nextgroup match.
	 */
	ఒకవేళ (current_తదుపరి_list != ఖాళీ && !keep_తదుపరి_list)
	{
	    /*
	     * If a nextgroup was not found, continue looking for one if:
	     * - this is an empty line and the "skipempty" option was given
	     * - we are on white space and the "skipwhite" option was given
	     */
	    ఒకవేళ (!దొరికింది_సరిపోలిక)
	    {
		పంక్తి = syn_getcurline();
		ఒకవేళ (((current_తదుపరి_flags & HL_SKIPWHITE)
			    && VIM_ISWHITE(పంక్తి[current_నిలువు]))
			|| ((current_తదుపరి_flags & HL_SKIPEMPTY)
			    && *పంక్తి == NUL))
		    ఆపు;
	    }

	    /*
	     * If a nextgroup was found: Use it, and continue looking for
	     * contained matches.
	     * If a nextgroup was not found: Continue looking for a normal
	     * match.
	     * When did set current_next_list for a zero-width item and no
	     * match was found don't loop (would get stuck).
	     */
	    current_తదుపరి_list = ఖాళీ;
	    తదుపరి_సరిపోలిక_idx = -1;
	    ఒకవేళ (!zero_వెడల్పు_తదుపరి_list)
		దొరికింది_సరిపోలిక = TRUE;
	}

    } అయితే (దొరికింది_సరిపోలిక);

    restore_chartab(buf_chartab);

    /*
     * Use attributes from the current state, if within its highlighting.
     * If not, use attributes from the current-but-one state, etc.
     */
    current_attr = 0;
#నిర్వచితమైతే FEAT_EVAL
    current_id = 0;
    current_trans_id = 0;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CONCEAL
    current_flags = 0;
    current_seqnr = 0;
#అయితే_ముగింపు
    ఒకవేళ (cur_si != ఖాళీ)
    {
#నిర్వచితం_కాకపోతే FEAT_EVAL
	పూర్ణాంకం	current_trans_id = 0;
#అయితే_ముగింపు
	కొరకు (idx = current_state.ga_len - 1; idx >= 0; --idx)
	{
	    sip = &CUR_STATE(idx);
	    ఒకవేళ ((current_lnum > sip->si_h_startpos.lnum
			|| (current_lnum == sip->si_h_startpos.lnum
			    && current_నిలువు >= sip->si_h_startpos.నిలువు))
		    && (sip->si_h_endpos.lnum == 0
			|| current_lnum < sip->si_h_endpos.lnum
			|| (current_lnum == sip->si_h_endpos.lnum
			    && current_నిలువు < sip->si_h_endpos.నిలువు)))
	    {
		current_attr = sip->si_attr;
#నిర్వచితమైతే FEAT_EVAL
		current_id = sip->si_id;
#అయితే_ముగింపు
		current_trans_id = sip->si_trans_id;
#నిర్వచితమైతే FEAT_CONCEAL
		current_flags = sip->si_flags;
		current_seqnr = sip->si_seqnr;
		current_sub_char = sip->si_cchar;
#అయితే_ముగింపు
		ఆపు;
	    }
	}

	ఒకవేళ (can_spell != ఖాళీ)
	{
	    నిర్మాణం sp_syn   sps;

	    /*
	     * set "can_spell" to TRUE if spell checking is supposed to be
	     * done in the current item.
	     */
	    ఒకవేళ (syn_ఖండం->b_spell_cluster_id == 0)
	    {
		// There is no @Spell cluster: Do spelling for items without
		// @NoSpell cluster.
		ఒకవేళ (syn_ఖండం->b_nospell_cluster_id == 0
						     || current_trans_id == 0)
		    *can_spell = (syn_ఖండం->b_syn_spell != SYNSPL_NOTOP);
		లేకపోతే
		{
		    sps.inc_ట్యాగ్ = 0;
		    sps.id = syn_ఖండం->b_nospell_cluster_id;
		    sps.cont_లోపల_list = ఖాళీ;
		    *can_spell = !లోపల_id_list(sip, sip->si_cont_list, &sps, 0);
		}
	    }
	    లేకపోతే
	    {
		// The @Spell cluster is defined: Do spelling in items with
		// the @Spell cluster.  But not when @NoSpell is also there.
		// At the toplevel only spell check when ":syn spell toplevel"
		// was used.
		ఒకవేళ (current_trans_id == 0)
		    *can_spell = (syn_ఖండం->b_syn_spell == SYNSPL_శిఖరం);
		లేకపోతే
		{
		    sps.inc_ట్యాగ్ = 0;
		    sps.id = syn_ఖండం->b_spell_cluster_id;
		    sps.cont_లోపల_list = ఖాళీ;
		    *can_spell = లోపల_id_list(sip, sip->si_cont_list, &sps, 0);

		    ఒకవేళ (syn_ఖండం->b_nospell_cluster_id != 0)
		    {
			sps.id = syn_ఖండం->b_nospell_cluster_id;
			ఒకవేళ (లోపల_id_list(sip, sip->si_cont_list, &sps, 0))
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
	ఒకవేళ (!syncing && !keep_state)
	{
	    check_state_చివర్లు();
	    ఒకవేళ (current_state.ga_len > 0
				      && syn_getcurline()[current_నిలువు] != NUL)
	    {
		++current_నిలువు;
		check_state_చివర్లు();
		--current_నిలువు;
	    }
	}
    }
    లేకపోతే ఒకవేళ (can_spell != ఖాళీ)
	// Default: Only do spelling when there is no @Spell cluster or when
	// ":syn spell toplevel" was used.
	*can_spell = syn_ఖండం->b_syn_spell == SYNSPL_DEFAULT
		    ? (syn_ఖండం->b_spell_cluster_id == 0)
		    : (syn_ఖండం->b_syn_spell == SYNSPL_శిఖరం);

    // nextgroup ends at end of line, unless "skipnl" or "skipempty" present
    ఒకవేళ (current_తదుపరి_list != ఖాళీ
	    && (పంక్తి = syn_getcurline())[current_నిలువు] != NUL
	    && పంక్తి[current_నిలువు + 1] == NUL
	    && !(current_తదుపరి_flags & (HL_SKIPNL | HL_SKIPEMPTY)))
	current_తదుపరి_list = ఖాళీ;

    ఒకవేళ (zero_వెడల్పు_తదుపరి_ga.ga_len > 0)
	ga_clear(&zero_వెడల్పు_తదుపరి_ga);

    // No longer need external matches.  But keep next_match_extmatch.
    unref_extmatch(re_extmatch_బహిర్గతం);
    re_extmatch_బహిర్గతం = ఖాళీ;
    unref_extmatch(cur_extmatch);

    తిరిగి current_attr;
}


/*
 * Check if we already matched pattern "idx" at the current column.
 */
    స్థిర పూర్ణాంకం
did_సరిపోలిక_already(పూర్ణాంకం idx, garray_T *gap)
{
    పూర్ణాంకం		i;

    కొరకు (i = current_state.ga_len; --i >= 0; )
	ఒకవేళ (CUR_STATE(i).si_m_startcol == (పూర్ణాంకం)current_నిలువు
		&& CUR_STATE(i).si_m_lnum == (పూర్ణాంకం)current_lnum
		&& CUR_STATE(i).si_idx == idx)
	    తిరిగి TRUE;

    // Zero-width matches with a nextgroup argument are not put on the syntax
    // stack, and can only be matched once anyway.
    కొరకు (i = gap->ga_len; --i >= 0; )
	ఒకవేళ (((పూర్ణాంకం *)(gap->ga_దత్తాంశం))[i] == idx)
	    తిరిగి TRUE;

    తిరిగి FALSE;
}

/*
 * Push the next match onto the stack.
 */
    స్థిర stateitem_T *
నెట్టు_తదుపరి_సరిపోలిక(stateitem_T *cur_si)
{
    synpat_T	*spp;
#నిర్వచితమైతే FEAT_CONCEAL
    పూర్ణాంకం		 save_flags;
#అయితే_ముగింపు

    spp = &(SYN_వస్తువులు(syn_ఖండం)[తదుపరి_సరిపోలిక_idx]);

    /*
     * Push the item in current_state stack;
     */
    ఒకవేళ (నెట్టు_current_state(తదుపరి_సరిపోలిక_idx) == OK)
    {
	/*
	 * If it's a start-skip-end type that crosses lines, figure out how
	 * much it continues in this line.  Otherwise just fill in the length.
	 */
	cur_si = &CUR_STATE(current_state.ga_len - 1);
	cur_si->si_h_startpos = తదుపరి_సరిపోలిక_h_startpos;
	cur_si->si_m_startcol = current_నిలువు;
	cur_si->si_m_lnum = current_lnum;
	cur_si->si_flags = spp->sp_flags;
#నిర్వచితమైతే FEAT_CONCEAL
	cur_si->si_seqnr = తదుపరి_seqnr++;
	cur_si->si_cchar = spp->sp_cchar;
	ఒకవేళ (current_state.ga_len > 1)
	    cur_si->si_flags |=
		    CUR_STATE(current_state.ga_len - 2).si_flags & HL_CONCEAL;
#అయితే_ముగింపు
	cur_si->si_తదుపరి_list = spp->sp_తదుపరి_list;
	cur_si->si_extmatch = ref_extmatch(తదుపరి_సరిపోలిక_extmatch);
	ఒకవేళ (spp->sp_type == SPTYPE_ప్రారంభం && !(spp->sp_flags & HL_ONELINE))
	{
	    // Try to find the end pattern in the current line
	    update_si_చివర(cur_si, (పూర్ణాంకం)(తదుపరి_సరిపోలిక_m_endpos.నిలువు), TRUE);
	    check_keepend();
	}
	లేకపోతే
	{
	    cur_si->si_m_endpos = తదుపరి_సరిపోలిక_m_endpos;
	    cur_si->si_h_endpos = తదుపరి_సరిపోలిక_h_endpos;
	    cur_si->si_చివర్లు = TRUE;
	    cur_si->si_flags |= తదుపరి_సరిపోలిక_flags;
	    cur_si->si_eoe_pos = తదుపరి_సరిపోలిక_eoe_pos;
	    cur_si->si_చివర_idx = తదుపరి_సరిపోలిక_చివర_idx;
	}
	ఒకవేళ (keepend_level < 0 && (cur_si->si_flags & HL_KEEPEND))
	    keepend_level = current_state.ga_len - 1;
	check_keepend();
	update_si_attr(current_state.ga_len - 1);

#నిర్వచితమైతే FEAT_CONCEAL
	save_flags = cur_si->si_flags & (HL_CONCEAL | HL_CONCEALENDS);
#అయితే_ముగింపు
	/*
	 * If the start pattern has another highlight group, push another item
	 * on the stack for the start pattern.
	 */
	ఒకవేళ (	   spp->sp_type == SPTYPE_ప్రారంభం
		&& spp->sp_syn_సరిపోలిక_id != 0
		&& నెట్టు_current_state(తదుపరి_సరిపోలిక_idx) == OK)
	{
	    cur_si = &CUR_STATE(current_state.ga_len - 1);
	    cur_si->si_h_startpos = తదుపరి_సరిపోలిక_h_startpos;
	    cur_si->si_m_startcol = current_నిలువు;
	    cur_si->si_m_lnum = current_lnum;
	    cur_si->si_m_endpos = తదుపరి_సరిపోలిక_eos_pos;
	    cur_si->si_h_endpos = తదుపరి_సరిపోలిక_eos_pos;
	    cur_si->si_చివర్లు = TRUE;
	    cur_si->si_చివర_idx = 0;
	    cur_si->si_flags = HL_సరిపోలిక;
#నిర్వచితమైతే FEAT_CONCEAL
	    cur_si->si_seqnr = తదుపరి_seqnr++;
	    cur_si->si_flags |= save_flags;
	    ఒకవేళ (cur_si->si_flags & HL_CONCEALENDS)
		cur_si->si_flags |= HL_CONCEAL;
#అయితే_ముగింపు
	    cur_si->si_తదుపరి_list = ఖాళీ;
	    check_keepend();
	    update_si_attr(current_state.ga_len - 1);
	}
    }

    తదుపరి_సరిపోలిక_idx = -1;	// try other match next time

    తిరిగి cur_si;
}

/*
 * Check for end of current state (and the states before it).
 */
    స్థిర శూన్య
check_state_చివర్లు(శూన్య)
{
    stateitem_T	*cur_si;
    పూర్ణాంకం		had_extend;

    cur_si = &CUR_STATE(current_state.ga_len - 1);
    కొరకు (;;)
    {
	ఒకవేళ (cur_si->si_చివర్లు
		&& (cur_si->si_m_endpos.lnum < current_lnum
		    || (cur_si->si_m_endpos.lnum == current_lnum
			&& cur_si->si_m_endpos.నిలువు <= current_నిలువు)))
	{
	    /*
	     * If there is an end pattern group ID, highlight the end pattern
	     * now.  No need to pop the current item from the stack.
	     * Only do this if the end pattern continues beyond the current
	     * position.
	     */
	    ఒకవేళ (cur_si->si_చివర_idx
		    && (cur_si->si_eoe_pos.lnum > current_lnum
			|| (cur_si->si_eoe_pos.lnum == current_lnum
			    && cur_si->si_eoe_pos.నిలువు > current_నిలువు)))
	    {
		cur_si->si_idx = cur_si->si_చివర_idx;
		cur_si->si_చివర_idx = 0;
		cur_si->si_m_endpos = cur_si->si_eoe_pos;
		cur_si->si_h_endpos = cur_si->si_eoe_pos;
		cur_si->si_flags |= HL_సరిపోలిక;
#నిర్వచితమైతే FEAT_CONCEAL
		cur_si->si_seqnr = తదుపరి_seqnr++;
		ఒకవేళ (cur_si->si_flags & HL_CONCEALENDS)
		    cur_si->si_flags |= HL_CONCEAL;
#అయితే_ముగింపు
		update_si_attr(current_state.ga_len - 1);

		// nextgroup= should not match in the end pattern
		current_తదుపరి_list = ఖాళీ;

		// what matches next may be different now, clear it
		తదుపరి_సరిపోలిక_idx = 0;
		తదుపరి_సరిపోలిక_నిలువు = MAXCOL;
		ఆపు;
	    }

	    // handle next_list, unless at end of line and no "skipnl" or
	    // "skipempty"
	    current_తదుపరి_list = cur_si->si_తదుపరి_list;
	    current_తదుపరి_flags = cur_si->si_flags;
	    ఒకవేళ (!(current_తదుపరి_flags & (HL_SKIPNL | HL_SKIPEMPTY))
		    && syn_getcurline()[current_నిలువు] == NUL)
		current_తదుపరి_list = ఖాళీ;

	    // When the ended item has "extend", another item with
	    // "keepend" now needs to check for its end.
	    had_extend = (cur_si->si_flags & HL_EXTEND);

	    తీయి_current_state();

	    ఒకవేళ (current_state.ga_len == 0)
		ఆపు;

	    ఒకవేళ (had_extend && keepend_level >= 0)
	    {
		syn_update_చివర్లు(FALSE);
		ఒకవేళ (current_state.ga_len == 0)
		    ఆపు;
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
	    ఒకవేళ (cur_si->si_idx >= 0
		    && SYN_వస్తువులు(syn_ఖండం)[cur_si->si_idx].sp_type
								== SPTYPE_ప్రారంభం
		    && !(cur_si->si_flags & (HL_సరిపోలిక | HL_KEEPEND)))
	    {
		update_si_చివర(cur_si, (పూర్ణాంకం)current_నిలువు, TRUE);
		check_keepend();
		ఒకవేళ ((current_తదుపరి_flags & HL_HAS_EOL)
			&& keepend_level < 0
			&& syn_getcurline()[current_నిలువు] == NUL)
		    ఆపు;
	    }
	}
	లేకపోతే
	    ఆపు;
    }
}

/*
 * Update an entry in the current_state stack for a match or region.  This
 * fills in si_attr, si_next_list and si_cont_list.
 */
    స్థిర శూన్య
update_si_attr(పూర్ణాంకం idx)
{
    stateitem_T	*sip = &CUR_STATE(idx);
    synpat_T	*spp;

    // This should not happen...
    ఒకవేళ (sip->si_idx < 0)
	తిరిగి;

    spp = &(SYN_వస్తువులు(syn_ఖండం)[sip->si_idx]);
    ఒకవేళ (sip->si_flags & HL_సరిపోలిక)
	sip->si_id = spp->sp_syn_సరిపోలిక_id;
    లేకపోతే
	sip->si_id = spp->sp_syn.id;
    sip->si_attr = syn_id2attr(sip->si_id);
    sip->si_trans_id = sip->si_id;
    ఒకవేళ (sip->si_flags & HL_సరిపోలిక)
	sip->si_cont_list = ఖాళీ;
    లేకపోతే
	sip->si_cont_list = spp->sp_cont_list;

    /*
     * For transparent items, take attr from outer item.
     * Also take cont_list, if there is none.
     * Don't do this for the matchgroup of a start or end pattern.
     */
    ఒకవేళ ((spp->sp_flags & HL_TRANSP) && !(sip->si_flags & HL_సరిపోలిక))
    {
	ఒకవేళ (idx == 0)
	{
	    sip->si_attr = 0;
	    sip->si_trans_id = 0;
	    ఒకవేళ (sip->si_cont_list == ఖాళీ)
		sip->si_cont_list = ID_LIST_ALL;
	}
	లేకపోతే
	{
	    sip->si_attr = CUR_STATE(idx - 1).si_attr;
	    sip->si_trans_id = CUR_STATE(idx - 1).si_trans_id;
	    ఒకవేళ (sip->si_cont_list == ఖాళీ)
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
    స్థిర శూన్య
check_keepend(శూన్య)
{
    పూర్ణాంకం		i;
    lpos_T	maxpos;
    lpos_T	maxpos_h;
    stateitem_T	*sip;

    /*
     * This check can consume a lot of time; only do it from the level where
     * there really is a keepend.
     */
    ఒకవేళ (keepend_level < 0)
	తిరిగి;

    /*
     * Find the last index of an "extend" item.  "keepend" items before that
     * won't do anything.  If there is no "extend" item "i" will be
     * "keepend_level" and all "keepend" items will work normally.
     */
    కొరకు (i = current_state.ga_len - 1; i > keepend_level; --i)
	ఒకవేళ (CUR_STATE(i).si_flags & HL_EXTEND)
	    ఆపు;

    maxpos.lnum = 0;
    maxpos.నిలువు = 0;
    maxpos_h.lnum = 0;
    maxpos_h.నిలువు = 0;
    కొరకు ( ; i < current_state.ga_len; ++i)
    {
	sip = &CUR_STATE(i);
	ఒకవేళ (maxpos.lnum != 0)
	{
	    హద్దు_pos_zero(&sip->si_m_endpos, &maxpos);
	    హద్దు_pos_zero(&sip->si_h_endpos, &maxpos_h);
	    హద్దు_pos_zero(&sip->si_eoe_pos, &maxpos);
	    sip->si_చివర్లు = TRUE;
	}
	ఒకవేళ (sip->si_చివర్లు && (sip->si_flags & HL_KEEPEND))
	{
	    ఒకవేళ (maxpos.lnum == 0
		    || maxpos.lnum > sip->si_m_endpos.lnum
		    || (maxpos.lnum == sip->si_m_endpos.lnum
			&& maxpos.నిలువు > sip->si_m_endpos.నిలువు))
		maxpos = sip->si_m_endpos;
	    ఒకవేళ (maxpos_h.lnum == 0
		    || maxpos_h.lnum > sip->si_h_endpos.lnum
		    || (maxpos_h.lnum == sip->si_h_endpos.lnum
			&& maxpos_h.నిలువు > sip->si_h_endpos.నిలువు))
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
    స్థిర శూన్య
update_si_చివర(
    stateitem_T	*sip,
    పూర్ణాంకం		startcol,   // where to start searching for the end
    పూర్ణాంకం		force)	    // when TRUE overrule a previous end
{
    lpos_T	startpos;
    lpos_T	endpos;
    lpos_T	hl_endpos;
    lpos_T	చివర_endpos;
    పూర్ణాంకం		చివర_idx;

    // return quickly for a keyword
    ఒకవేళ (sip->si_idx < 0)
	తిరిగి;

    // Don't update when it's already done.  Can be a match of an end pattern
    // that started in a previous line.  Watch out: can also be a "keepend"
    // from a containing item.
    ఒకవేళ (!force && sip->si_m_endpos.lnum >= current_lnum)
	తిరిగి;

    /*
     * We need to find the end of the region.  It may continue in the next
     * line.
     */
    చివర_idx = 0;
    startpos.lnum = current_lnum;
    startpos.నిలువు = startcol;
    కనుగొను_endpos(sip->si_idx, &startpos, &endpos, &hl_endpos,
		   &(sip->si_flags), &చివర_endpos, &చివర_idx, sip->si_extmatch);

    ఒకవేళ (endpos.lnum == 0)
    {
	// No end pattern matched.
	ఒకవేళ (SYN_వస్తువులు(syn_ఖండం)[sip->si_idx].sp_flags & HL_ONELINE)
	{
	    // a "oneline" never continues in the next line
	    sip->si_చివర్లు = TRUE;
	    sip->si_m_endpos.lnum = current_lnum;
	    sip->si_m_endpos.నిలువు = syn_getcurline_len();
	}
	లేకపోతే
	{
	    // continues in the next line
	    sip->si_చివర్లు = FALSE;
	    sip->si_m_endpos.lnum = 0;
	}
	sip->si_h_endpos = sip->si_m_endpos;
    }
    లేకపోతే
    {
	// match within this line
	sip->si_m_endpos = endpos;
	sip->si_h_endpos = hl_endpos;
	sip->si_eoe_pos = చివర_endpos;
	sip->si_చివర్లు = TRUE;
	sip->si_చివర_idx = చివర_idx;
    }
}

/*
 * Add a new state to the current state stack.
 * It is cleared and the index set to "idx".
 * Return FAIL if it's not possible (out of memory).
 */
    స్థిర పూర్ణాంకం
నెట్టు_current_state(పూర్ణాంకం idx)
{
    ఒకవేళ (ga_grow(&current_state, 1) == FAIL)
	తిరిగి FAIL;
    CLEAR_సూచిక(&CUR_STATE(current_state.ga_len));
    CUR_STATE(current_state.ga_len).si_idx = idx;
    ++current_state.ga_len;
    తిరిగి OK;
}

/*
 * Remove a state from the current_state stack.
 */
    స్థిర శూన్య
తీయి_current_state(శూన్య)
{
    ఒకవేళ (current_state.ga_len)
    {
	unref_extmatch(CUR_STATE(current_state.ga_len - 1).si_extmatch);
	--current_state.ga_len;
    }
    // after the end of a pattern, try matching a keyword or pattern
    తదుపరి_సరిపోలిక_idx = -1;

    // if first state with "keepend" is popped, reset keepend_level
    ఒకవేళ (keepend_level >= current_state.ga_len)
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
    స్థిర శూన్య
కనుగొను_endpos(
    పూర్ణాంకం		idx,		// index of the pattern
    lpos_T	*startpos,	// where to start looking for an END match
    lpos_T	*m_endpos,	// return: end of match
    lpos_T	*hl_endpos,	// return: end of highlighting
    దీర్ఘ	*flagsp,	// return: flags of matching END
    lpos_T	*చివర_endpos,	// return: end of end pattern match
    పూర్ణాంకం		*చివర_idx,	// return: group ID for end pat. match, or 0
    reg_extmatch_T *ప్రారంభం_ext)	// submatches from the start pattern
{
    colnr_T	matchcol;
    synpat_T	*spp, *spp_skip;
    పూర్ణాంకం		ప్రారంభం_idx;
    పూర్ణాంకం		ఉత్తమ_idx;
    regmmatch_T	regmatch;
    regmmatch_T	ఉత్తమ_regmatch;	    // startpos/endpos of best match
    lpos_T	pos;
    పూర్ణాంకం		had_సరిపోలిక = FALSE;
    char_u	buf_chartab[32];  // chartab array for syn option iskyeyword

    // just in case we are invoked for a keyword
    ఒకవేళ (idx < 0)
	తిరిగి;

    /*
     * Check for being called with a START pattern.
     * Can happen with a match that continues to the next line, because it
     * contained a region.
     */
    spp = &(SYN_వస్తువులు(syn_ఖండం)[idx]);
    ఒకవేళ (spp->sp_type != SPTYPE_ప్రారంభం)
    {
	*hl_endpos = *startpos;
	తిరిగి;
    }

    /*
     * Find the SKIP or first END pattern after the last START pattern.
     */
    కొరకు (;;)
    {
	spp = &(SYN_వస్తువులు(syn_ఖండం)[idx]);
	ఒకవేళ (spp->sp_type != SPTYPE_ప్రారంభం)
	    ఆపు;
	++idx;
    }

    /*
     *	Lookup the SKIP pattern (if present)
     */
    ఒకవేళ (spp->sp_type == SPTYPE_SKIP)
    {
	spp_skip = spp;
	++idx;
    }
    లేకపోతే
	spp_skip = ఖాళీ;

    // Setup external matches for syn_regexec().
    unref_extmatch(re_extmatch_లోపల);
    re_extmatch_లోపల = ref_extmatch(ప్రారంభం_ext);

    matchcol = startpos->నిలువు;	// start looking for a match at sstart
    ప్రారంభం_idx = idx;		// remember the first END pattern.
    ఉత్తమ_regmatch.startpos[0].నిలువు = 0;		// avoid compiler warning

    // use syntax iskeyword option
    save_chartab(buf_chartab);

    కొరకు (;;)
    {
	/*
	 * Find end pattern that matches first after "matchcol".
	 */
	ఉత్తమ_idx = -1;
	కొరకు (idx = ప్రారంభం_idx; idx < syn_ఖండం->b_syn_patterns.ga_len; ++idx)
	{
	    పూర్ణాంకం lc_నిలువు = matchcol;
	    పూర్ణాంకం r;

	    spp = &(SYN_వస్తువులు(syn_ఖండం)[idx]);
	    ఒకవేళ (spp->sp_type != SPTYPE_చివర)	// past last END pattern
		ఆపు;
	    lc_నిలువు -= spp->sp_offsets[SPO_LC_OFF];
	    ఒకవేళ (lc_నిలువు < 0)
		lc_నిలువు = 0;

	    regmatch.rmm_ic = spp->sp_ic;
	    regmatch.regprog = spp->sp_prog;
	    r = syn_regexec(&regmatch, startpos->lnum, lc_నిలువు,
						  IF_SYN_సమయం(&spp->sp_సమయం));
	    spp->sp_prog = regmatch.regprog;
	    ఒకవేళ (r)
	    {
		ఒకవేళ (ఉత్తమ_idx == -1 || regmatch.startpos[0].నిలువు
					      < ఉత్తమ_regmatch.startpos[0].నిలువు)
		{
		    ఉత్తమ_idx = idx;
		    ఉత్తమ_regmatch.startpos[0] = regmatch.startpos[0];
		    ఉత్తమ_regmatch.endpos[0] = regmatch.endpos[0];
		}
	    }
	}

	/*
	 * If all end patterns have been tried, and there is no match, the
	 * item continues until end-of-line.
	 */
	ఒకవేళ (ఉత్తమ_idx == -1)
	    ఆపు;

	/*
	 * If the skip pattern matches before the end pattern,
	 * continue searching after the skip pattern.
	 */
	ఒకవేళ (spp_skip != ఖాళీ)
	{
	    పూర్ణాంకం lc_నిలువు = matchcol - spp_skip->sp_offsets[SPO_LC_OFF];
	    పూర్ణాంకం r;

	    ఒకవేళ (lc_నిలువు < 0)
		lc_నిలువు = 0;
	    regmatch.rmm_ic = spp_skip->sp_ic;
	    regmatch.regprog = spp_skip->sp_prog;
	    r = syn_regexec(&regmatch, startpos->lnum, lc_నిలువు,
					      IF_SYN_సమయం(&spp_skip->sp_సమయం));
	    spp_skip->sp_prog = regmatch.regprog;
	    ఒకవేళ (r && regmatch.startpos[0].నిలువు
					     <= ఉత్తమ_regmatch.startpos[0].నిలువు)
	    {
		పూర్ణాంకం పంక్తి_len;

		// Add offset to skip pattern match
		syn_కూడు_చివర_off(&pos, &regmatch, spp_skip, SPO_ME_OFF, 1);

		// If the skip pattern goes on to the next line, there is no
		// match with an end pattern in this line.
		ఒకవేళ (pos.lnum > startpos->lnum)
		    ఆపు;

		పంక్తి_len = ml_get_buf_len(syn_buf, startpos->lnum);

		// take care of an empty match or negative offset
		ఒకవేళ (pos.నిలువు <= matchcol)
		    ++matchcol;
		లేకపోతే ఒకవేళ (pos.నిలువు <= regmatch.endpos[0].నిలువు)
		    matchcol = pos.నిలువు;
		లేకపోతే
		    // Be careful not to jump over the NUL at the end-of-line
		    కొరకు (matchcol = regmatch.endpos[0].నిలువు;
			    matchcol < పంక్తి_len && matchcol < pos.నిలువు;
								   ++matchcol)
			;

		// if the skip pattern includes end-of-line, break here
		ఒకవేళ (matchcol >= పంక్తి_len)
		    ఆపు;

		కొనసాగు;	    // start with first end pattern again
	    }
	}

	/*
	 * Match from start pattern to end pattern.
	 * Correct for match and highlight offset of end pattern.
	 */
	spp = &(SYN_వస్తువులు(syn_ఖండం)[ఉత్తమ_idx]);
	syn_కూడు_చివర_off(m_endpos, &ఉత్తమ_regmatch, spp, SPO_ME_OFF, 1);
	// can't end before the start
	ఒకవేళ (m_endpos->lnum == startpos->lnum && m_endpos->నిలువు < startpos->నిలువు)
	    m_endpos->నిలువు = startpos->నిలువు;

	syn_కూడు_చివర_off(చివర_endpos, &ఉత్తమ_regmatch, spp, SPO_HE_OFF, 1);
	// can't end before the start
	ఒకవేళ (చివర_endpos->lnum == startpos->lnum
					   && చివర_endpos->నిలువు < startpos->నిలువు)
	    చివర_endpos->నిలువు = startpos->నిలువు;
	// can't end after the match
	హద్దు_pos(చివర_endpos, m_endpos);

	/*
	 * If the end group is highlighted differently, adjust the pointers.
	 */
	ఒకవేళ (spp->sp_syn_సరిపోలిక_id != spp->sp_syn.id && spp->sp_syn_సరిపోలిక_id != 0)
	{
	    *చివర_idx = ఉత్తమ_idx;
	    ఒకవేళ (spp->sp_off_flags & (1 << (SPO_RE_OFF + SPO_లెక్క)))
	    {
		hl_endpos->lnum = ఉత్తమ_regmatch.endpos[0].lnum;
		hl_endpos->నిలువు = ఉత్తమ_regmatch.endpos[0].నిలువు;
	    }
	    లేకపోతే
	    {
		hl_endpos->lnum = ఉత్తమ_regmatch.startpos[0].lnum;
		hl_endpos->నిలువు = ఉత్తమ_regmatch.startpos[0].నిలువు;
	    }
	    hl_endpos->నిలువు += spp->sp_offsets[SPO_RE_OFF];

	    // can't end before the start
	    ఒకవేళ (hl_endpos->lnum == startpos->lnum
					    && hl_endpos->నిలువు < startpos->నిలువు)
		hl_endpos->నిలువు = startpos->నిలువు;
	    హద్దు_pos(hl_endpos, m_endpos);

	    // now the match ends where the highlighting ends, it is turned
	    // into the matchgroup for the end
	    *m_endpos = *hl_endpos;
	}
	లేకపోతే
	{
	    *చివర_idx = 0;
	    *hl_endpos = *చివర_endpos;
	}

	*flagsp = spp->sp_flags;

	had_సరిపోలిక = TRUE;
	ఆపు;
    }

    // no match for an END pattern in this line
    ఒకవేళ (!had_సరిపోలిక)
	m_endpos->lnum = 0;

    restore_chartab(buf_chartab);

    // Remove external matches.
    unref_extmatch(re_extmatch_లోపల);
    re_extmatch_లోపల = ఖాళీ;
}

/*
 * Limit "pos" not to be after "limit".
 */
    స్థిర శూన్య
హద్దు_pos(lpos_T *pos, lpos_T *హద్దు)
{
    ఒకవేళ (pos->lnum > హద్దు->lnum)
	*pos = *హద్దు;
    లేకపోతే ఒకవేళ (pos->lnum == హద్దు->lnum && pos->నిలువు > హద్దు->నిలువు)
	pos->నిలువు = హద్దు->నిలువు;
}

/*
 * Limit "pos" not to be after "limit", unless pos->lnum is zero.
 */
    స్థిర శూన్య
హద్దు_pos_zero(
    lpos_T	*pos,
    lpos_T	*హద్దు)
{
    ఒకవేళ (pos->lnum == 0)
	*pos = *హద్దు;
    లేకపోతే
	హద్దు_pos(pos, హద్దు);
}

/*
 * Add offset to matched text for end of match or highlight.
 */
    స్థిర శూన్య
syn_కూడు_చివర_off(
    lpos_T	*ఫలితం,	// returned position
    regmmatch_T	*regmatch,	// start/end of match
    synpat_T	*spp,		// matched pattern
    పూర్ణాంకం		idx,		// index of offset
    పూర్ణాంకం		extra)		// extra chars for offset to start
{
    పూర్ణాంకం		నిలువు;
    పూర్ణాంకం		off;
    char_u	*పునాది;
    char_u	*p;

    ఒకవేళ (spp->sp_off_flags & (1 << idx))
    {
	ఫలితం->lnum = regmatch->startpos[0].lnum;
	నిలువు = regmatch->startpos[0].నిలువు;
	off = spp->sp_offsets[idx] + extra;
    }
    లేకపోతే
    {
	ఫలితం->lnum = regmatch->endpos[0].lnum;
	నిలువు = regmatch->endpos[0].నిలువు;
	off = spp->sp_offsets[idx];
    }
    // Don't go past the end of the line.  Matters for "rs=e+2" when there
    // is a matchgroup. Watch out for match with last NL in the buffer.
    ఒకవేళ (ఫలితం->lnum > syn_buf->b_ml.ml_పంక్తి_లెక్క)
	నిలువు = 0;
    లేకపోతే ఒకవేళ (off != 0)
    {
	పునాది = ml_get_buf(syn_buf, ఫలితం->lnum, FALSE);
	p = పునాది + నిలువు;
	ఒకవేళ (off > 0)
	{
	    అయితే (off-- > 0 && *p != NUL)
		MB_PTR_ADV(p);
	}
	లేకపోతే ఒకవేళ (off < 0)
	{
	    అయితే (off++ < 0 && పునాది < p)
		MB_PTR_వెనుక(పునాది, p);
	}
	నిలువు = (పూర్ణాంకం)(p - పునాది);
    }
    ఫలితం->నిలువు = నిలువు;
}

/*
 * Add offset to matched text for start of match or highlight.
 * Avoid resulting column to become negative.
 */
    స్థిర శూన్య
syn_కూడు_ప్రారంభం_off(
    lpos_T	*ఫలితం,	// returned position
    regmmatch_T	*regmatch,	// start/end of match
    synpat_T	*spp,
    పూర్ణాంకం		idx,
    పూర్ణాంకం		extra)	    // extra chars for offset to end
{
    పూర్ణాంకం		నిలువు;
    పూర్ణాంకం		off;
    char_u	*పునాది;
    char_u	*p;

    ఒకవేళ (spp->sp_off_flags & (1 << (idx + SPO_లెక్క)))
    {
	ఫలితం->lnum = regmatch->endpos[0].lnum;
	నిలువు = regmatch->endpos[0].నిలువు;
	off = spp->sp_offsets[idx] + extra;
    }
    లేకపోతే
    {
	ఫలితం->lnum = regmatch->startpos[0].lnum;
	నిలువు = regmatch->startpos[0].నిలువు;
	off = spp->sp_offsets[idx];
    }
    ఒకవేళ (ఫలితం->lnum > syn_buf->b_ml.ml_పంక్తి_లెక్క)
    {
	// a "\n" at the end of the pattern may take us below the last line
	ఫలితం->lnum = syn_buf->b_ml.ml_పంక్తి_లెక్క;
	నిలువు = ml_get_buf_len(syn_buf, ఫలితం->lnum);
    }
    ఒకవేళ (off != 0)
    {
	పునాది = ml_get_buf(syn_buf, ఫలితం->lnum, FALSE);
	p = పునాది + నిలువు;
	ఒకవేళ (off > 0)
	{
	    అయితే (off-- && *p != NUL)
		MB_PTR_ADV(p);
	}
	లేకపోతే ఒకవేళ (off < 0)
	{
	    అయితే (off++ && పునాది < p)
		MB_PTR_వెనుక(పునాది, p);
	}
	నిలువు = (పూర్ణాంకం)(p - పునాది);
    }
    ఫలితం->నిలువు = నిలువు;
}

/*
 * Get current line in syntax buffer.
 */
    స్థిర char_u *
syn_getcurline(శూన్య)
{
    తిరిగి ml_get_buf(syn_buf, current_lnum, FALSE);
}

/*
 * Get length of current line in syntax buffer.
 */
    స్థిర colnr_T
syn_getcurline_len(శూన్య)
{
    తిరిగి ml_get_buf_len(syn_buf, current_lnum);
}

/*
 * Call vim_regexec() to find a match with "rmp" in "syn_buf".
 * Returns TRUE when there is a match.
 */
    స్థిర పూర్ణాంకం
syn_regexec(
    regmmatch_T	*rmp,
    linenr_T	lnum,
    colnr_T	నిలువు,
    syn_సమయం_T  *st వాడని)
{
    పూర్ణాంకం		r;
    పూర్ణాంకం		timed_బహిర్గతం = FALSE;
#నిర్వచితమైతే FEAT_PROFILE
    proftime_T	pt;

    ఒకవేళ (syn_సమయం_on)
	profile_ప్రారంభం(&pt);
#అయితే_ముగింపు

    ఒకవేళ (rmp->regprog == ఖాళీ)
	// This can happen if a previous call to vim_regexec_multi() tried to
	// use the NFA engine, which resulted in NFA_TOO_EXPENSIVE, and
	// compiling the pattern with the other engine fails.
	తిరిగి FALSE;

    rmp->rmm_maxcol = syn_buf->b_p_smc;
    r = vim_regexec_multi(rmp, syn_win, syn_buf, lnum, నిలువు, &timed_బహిర్గతం);

#నిర్వచితమైతే FEAT_PROFILE
    ఒకవేళ (syn_సమయం_on)
    {
	profile_చివర(&pt);
	profile_కూడు(&st->మొత్తం, &pt);
	ఒకవేళ (profile_cmp(&pt, &st->slowest) < 0)
	    st->slowest = pt;
	++st->లెక్క;
	ఒకవేళ (r > 0)
	    ++st->సరిపోలిక;
    }
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_RELTIME
    ఒకవేళ (timed_బహిర్గతం && redrawtime_హద్దు_సమితి && !syn_win->w_s->b_syn_slow)
    {
	syn_win->w_s->b_syn_slow = TRUE;
	సందేశం(_("'redrawtime' exceeded, syntax highlighting disabled"));
    }
#అయితే_ముగింపు

    ఒకవేళ (r > 0)
    {
	rmp->startpos[0].lnum += lnum;
	rmp->endpos[0].lnum += lnum;
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Check one position in a line for a matching keyword.
 * The caller must check if a keyword can start at startcol.
 * Return its ID if found, 0 otherwise.
 */
    స్థిర పూర్ణాంకం
check_keyword_id(
    char_u	*పంక్తి,
    పూర్ణాంకం		startcol,	// position in line to check for keyword
    పూర్ణాంకం		*endcolp,	// return: character after found keyword
    దీర్ఘ	*flagsp,	// return: flags of matching keyword
    పొట్టి	**తదుపరి_listp,	// return: next_list of matching keyword
    stateitem_T	*cur_si,	// item at the top of the stack
    పూర్ణాంకం		*ccharp వాడని)	// conceal substitution char
{
    keyentry_T	*kp;
    char_u	*kwp;
    పూర్ణాంకం		విడత;
    పూర్ణాంకం		kwlen;
    char_u	keyword[MAXKEYWLEN + 1]; // assume max. keyword len is 80
    hashtab_T	*ht;
    hashitem_T	*hi;

    // Find first character after the keyword.  First character was already
    // checked.
    kwp = పంక్తి + startcol;
    kwlen = 0;
    చేయి
    {
	ఒకవేళ (has_mbyte)
	    kwlen += (*mb_ptr2len)(kwp + kwlen);
	లేకపోతే
	    ++kwlen;
    }
    అయితే (vim_iswordp_buf(kwp + kwlen, syn_buf));

    ఒకవేళ (kwlen > MAXKEYWLEN)
	తిరిగి 0;

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
    కొరకు (విడత = 1; విడత <= 2; ++విడత)
    {
	ht = విడత == 1 ? &syn_ఖండం->b_keywtab : &syn_ఖండం->b_keywtab_ic;
	ఒకవేళ (ht->ht_వాడిన == 0)
	    కొనసాగు;
	ఒకవేళ (విడత == 2)	// ignore case
	    (శూన్య)str_foldcase(kwp, kwlen, keyword, MAXKEYWLEN + 1);

	/*
	 * Find keywords that match.  There can be several with different
	 * attributes.
	 * When current_next_list is non-zero accept only that group, otherwise:
	 *  Accept a not-contained keyword at toplevel.
	 *  Accept a keyword at other levels only if it is in the contains list.
	 */
	hi = hash_కనుగొను(ht, keyword);
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	    కొరకు (kp = HI2KE(hi); kp != ఖాళీ; kp = kp->ke_తదుపరి)
	    {
		ఒకవేళ (current_తదుపరి_list != 0
			? లోపల_id_list(ఖాళీ, current_తదుపరి_list, &kp->k_syn, 0)
			: (cur_si == ఖాళీ
			    ? !(kp->flags & HL_CONTAINED)
			    : లోపల_id_list(cur_si, cur_si->si_cont_list,
				      &kp->k_syn, kp->flags)))
		{
		    *endcolp = startcol + kwlen;
		    *flagsp = kp->flags;
		    *తదుపరి_listp = kp->తదుపరి_list;
#నిర్వచితమైతే FEAT_CONCEAL
		    *ccharp = kp->k_char;
#అయితే_ముగింపు
		    తిరిగి kp->k_syn.id;
		}
	    }
    }
    తిరిగి 0;
}

/*
 * Handle ":syntax conceal" command.
 */
    స్థిర శూన్య
syn_cmd_conceal(exarg_T *eap వాడని, పూర్ణాంకం syncing వాడని)
{
#నిర్వచితమైతే FEAT_CONCEAL
    char_u	*arg = eap->arg;
    char_u	*తదుపరి;

    eap->nextcmd = కనుగొను_nextcmd(arg);
    ఒకవేళ (eap->skip)
	తిరిగి;

    తదుపరి = skiptowhite(arg);
    ఒకవేళ (*arg == NUL)
    {
	ఒకవేళ (curwin->w_s->b_syn_conceal)
	    సందేశం("syntax conceal on");
	లేకపోతే
	    సందేశం("syntax conceal off");
    }
    లేకపోతే ఒకవేళ (STRNICMP(arg, "on", 2) == 0 && తదుపరి - arg == 2)
	curwin->w_s->b_syn_conceal = TRUE;
    లేకపోతే ఒకవేళ (STRNICMP(arg, "off", 3) == 0 && తదుపరి - arg == 3)
	curwin->w_s->b_syn_conceal = FALSE;
    లేకపోతే
	semsg(_(e_illegal_వాదం_str_2), arg);
#అయితే_ముగింపు
}

/*
 * Handle ":syntax case" command.
 */
    స్థిర శూన్య
syn_cmd_case(exarg_T *eap, పూర్ణాంకం syncing వాడని)
{
    char_u	*arg = eap->arg;
    char_u	*తదుపరి;

    eap->nextcmd = కనుగొను_nextcmd(arg);
    ఒకవేళ (eap->skip)
	తిరిగి;

    తదుపరి = skiptowhite(arg);
    ఒకవేళ (*arg == NUL)
    {
	ఒకవేళ (curwin->w_s->b_syn_ic)
	    సందేశం("syntax case ignore");
	లేకపోతే
	    సందేశం("syntax case match");
    }
    లేకపోతే ఒకవేళ (STRNICMP(arg, "match", 5) == 0 && తదుపరి - arg == 5)
	curwin->w_s->b_syn_ic = FALSE;
    లేకపోతే ఒకవేళ (STRNICMP(arg, "ignore", 6) == 0 && తదుపరి - arg == 6)
	curwin->w_s->b_syn_ic = TRUE;
    లేకపోతే
	semsg(_(e_illegal_వాదం_str_2), arg);
}

/*
 * Handle ":syntax foldlevel" command.
 */
    స్థిర శూన్య
syn_cmd_foldlevel(exarg_T *eap, పూర్ణాంకం syncing వాడని)
{
    char_u *arg = eap->arg;
    char_u *arg_చివర;

    eap->nextcmd = కనుగొను_nextcmd(arg);
    ఒకవేళ (eap->skip)
	తిరిగి;

    ఒకవేళ (*arg == NUL)
    {
	ఎంపిక (curwin->w_s->b_syn_foldlevel)
	{
	    స్థితి SYNFLD_ప్రారంభం:   సందేశం("syntax foldlevel start");   ఆపు;
	    స్థితి SYNFLD_MINIMUM: సందేశం("syntax foldlevel minimum"); ఆపు;
	    అప్రమేయ: ఆపు;
	}
	తిరిగి;
    }

    arg_చివర = skiptowhite(arg);
    ఒకవేళ (STRNICMP(arg, "start", 5) == 0 && arg_చివర - arg == 5)
	curwin->w_s->b_syn_foldlevel = SYNFLD_ప్రారంభం;
    లేకపోతే ఒకవేళ (STRNICMP(arg, "minimum", 7) == 0 && arg_చివర - arg == 7)
	curwin->w_s->b_syn_foldlevel = SYNFLD_MINIMUM;
    లేకపోతే
    {
	semsg(_(e_illegal_వాదం_str_2), arg);
	తిరిగి;
    }

    arg = skipwhite(arg_చివర);
    ఒకవేళ (*arg != NUL)
    {
	semsg(_(e_illegal_వాదం_str_2), arg);
    }
}

/*
 * Handle ":syntax spell" command.
 */
    స్థిర శూన్య
syn_cmd_spell(exarg_T *eap, పూర్ణాంకం syncing వాడని)
{
    char_u	*arg = eap->arg;
    char_u	*తదుపరి;

    eap->nextcmd = కనుగొను_nextcmd(arg);
    ఒకవేళ (eap->skip)
	తిరిగి;

    తదుపరి = skiptowhite(arg);
    ఒకవేళ (*arg == NUL)
    {
	ఒకవేళ (curwin->w_s->b_syn_spell == SYNSPL_శిఖరం)
	    సందేశం("syntax spell toplevel");
	లేకపోతే ఒకవేళ (curwin->w_s->b_syn_spell == SYNSPL_NOTOP)
	    సందేశం("syntax spell notoplevel");
	లేకపోతే
	    సందేశం("syntax spell default");
    }
    లేకపోతే ఒకవేళ (STRNICMP(arg, "toplevel", 8) == 0 && తదుపరి - arg == 8)
	curwin->w_s->b_syn_spell = SYNSPL_శిఖరం;
    లేకపోతే ఒకవేళ (STRNICMP(arg, "notoplevel", 10) == 0 && తదుపరి - arg == 10)
	curwin->w_s->b_syn_spell = SYNSPL_NOTOP;
    లేకపోతే ఒకవేళ (STRNICMP(arg, "default", 7) == 0 && తదుపరి - arg == 7)
	curwin->w_s->b_syn_spell = SYNSPL_DEFAULT;
    లేకపోతే
    {
	semsg(_(e_illegal_వాదం_str_2), arg);
	తిరిగి;
    }

    // assume spell checking changed, force a redraw
    redraw_win_later(curwin, UPD_NOT_VALID);
}

/*
 * Handle ":syntax iskeyword" command.
 */
    స్థిర శూన్య
syn_cmd_iskeyword(exarg_T *eap, పూర్ణాంకం syncing వాడని)
{
    char_u	*arg = eap->arg;
    char_u	save_chartab[32];
    char_u	*save_isk;

    ఒకవేళ (eap->skip)
	తిరిగి;

    arg = skipwhite(arg);
    ఒకవేళ (*arg == NUL)
    {
	సందేశం_puts("\n");
	ఒకవేళ (curwin->w_s->b_syn_isk != ఖాళీలు_option)
	{
	    సందేశం_puts("syntax iskeyword ");
	    సందేశం_outtrans(curwin->w_s->b_syn_isk);
	}
	లేకపోతే
	    సందేశం_outtrans((char_u *)_("syntax iskeyword not set"));
    }
    లేకపోతే
    {
	ఒకవేళ (STRNICMP(arg, "clear", 5) == 0)
	{
	    mch_memmove(curwin->w_s->b_syn_chartab, curbuf->b_chartab,
								  (size_t)32);
	    clear_string_option(&curwin->w_s->b_syn_isk);
	}
	లేకపోతే
	{
	    ఒకవేళ (check_isopt(arg) == FAIL)
	    {
		emsg(_(e_invalid_వాదం));
		తిరిగి;
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
    శూన్య
syntax_clear(synblock_T *ఖండం)
{
    పూర్ణాంకం i;

    ఖండం->b_syn_error = FALSE;	    // clear previous error
#నిర్వచితమైతే FEAT_RELTIME
    ఖండం->b_syn_slow = FALSE;	    // clear previous timeout
#అయితే_ముగింపు
    ఖండం->b_syn_ic = FALSE;	    // Use case, by default
    ఖండం->b_syn_foldlevel = SYNFLD_ప్రారంభం;
    ఖండం->b_syn_spell = SYNSPL_DEFAULT; // default spell checking
    ఖండం->b_syn_containedin = FALSE;
#నిర్వచితమైతే FEAT_CONCEAL
    ఖండం->b_syn_conceal = FALSE;
#అయితే_ముగింపు

    // free the keywords
    clear_keywtab(&ఖండం->b_keywtab);
    clear_keywtab(&ఖండం->b_keywtab_ic);

    // free the syntax patterns
    కొరకు (i = ఖండం->b_syn_patterns.ga_len; --i >= 0; )
	syn_clear_నమూనా(ఖండం, i);
    ga_clear(&ఖండం->b_syn_patterns);

    // free the syntax clusters
    కొరకు (i = ఖండం->b_syn_clusters.ga_len; --i >= 0; )
	syn_clear_cluster(ఖండం, i);
    ga_clear(&ఖండం->b_syn_clusters);
    ఖండం->b_spell_cluster_id = 0;
    ఖండం->b_nospell_cluster_id = 0;

    ఖండం->b_syn_sync_flags = 0;
    ఖండం->b_syn_sync_minlines = 0;
    ఖండం->b_syn_sync_maxlines = 0;
    ఖండం->b_syn_sync_linebreaks = 0;

    vim_regfree(ఖండం->b_syn_linecont_prog);
    ఖండం->b_syn_linecont_prog = ఖాళీ;
    VIM_CLEAR(ఖండం->b_syn_linecont_pat);
#నిర్వచితమైతే FEAT_FOLDING
    ఖండం->b_syn_folditems = 0;
#అయితే_ముగింపు
    clear_string_option(&ఖండం->b_syn_isk);

    // free the stored states
    syn_దొంతర_free_all(ఖండం);
    invalidate_current_state();

    // Reset the counter for ":syn include"
    running_syn_inc_ట్యాగ్ = 0;
}

/*
 * Get rid of ownsyntax for window "wp".
 */
    శూన్య
reset_synblock(win_T *wp)
{
    ఒకవేళ (wp->w_s != &wp->w_బఫరు->b_s)
    {
	syntax_clear(wp->w_s);
	vim_free(wp->w_s);
	wp->w_s = &wp->w_బఫరు->b_s;
    }
}

/*
 * Clear syncing info for one buffer.
 */
    స్థిర శూన్య
syntax_sync_clear(శూన్య)
{
    పూర్ణాంకం i;

    // free the syntax patterns
    కొరకు (i = curwin->w_s->b_syn_patterns.ga_len; --i >= 0; )
	ఒకవేళ (SYN_వస్తువులు(curwin->w_s)[i].sp_syncing)
	    syn_remove_నమూనా(curwin->w_s, i);

    curwin->w_s->b_syn_sync_flags = 0;
    curwin->w_s->b_syn_sync_minlines = 0;
    curwin->w_s->b_syn_sync_maxlines = 0;
    curwin->w_s->b_syn_sync_linebreaks = 0;

    vim_regfree(curwin->w_s->b_syn_linecont_prog);
    curwin->w_s->b_syn_linecont_prog = ఖాళీ;
    VIM_CLEAR(curwin->w_s->b_syn_linecont_pat);
    clear_string_option(&curwin->w_s->b_syn_isk);

    syn_దొంతర_free_all(curwin->w_s);	// Need to recompute all syntax.
}

/*
 * Remove one pattern from the buffer's pattern list.
 */
    స్థిర శూన్య
syn_remove_నమూనా(
    synblock_T	*ఖండం,
    పూర్ణాంకం		idx)
{
    synpat_T	*spp;

    spp = &(SYN_వస్తువులు(ఖండం)[idx]);
#నిర్వచితమైతే FEAT_FOLDING
    ఒకవేళ (spp->sp_flags & HL_FOLD)
	--ఖండం->b_syn_folditems;
#అయితే_ముగింపు
    syn_clear_నమూనా(ఖండం, idx);
    mch_memmove(spp, spp + 1,
		   పరిమాణం(synpat_T) * (ఖండం->b_syn_patterns.ga_len - idx - 1));
    --ఖండం->b_syn_patterns.ga_len;
}

/*
 * Clear and free one syntax pattern.  When clearing all, must be called from
 * last to first!
 */
    స్థిర శూన్య
syn_clear_నమూనా(synblock_T *ఖండం, పూర్ణాంకం i)
{
    vim_free(SYN_వస్తువులు(ఖండం)[i].sp_నమూనా);
    vim_regfree(SYN_వస్తువులు(ఖండం)[i].sp_prog);
    // Only free sp_cont_list and sp_next_list of first start pattern
    ఒకవేళ (i == 0 || SYN_వస్తువులు(ఖండం)[i - 1].sp_type != SPTYPE_ప్రారంభం)
    {
	vim_free(SYN_వస్తువులు(ఖండం)[i].sp_cont_list);
	vim_free(SYN_వస్తువులు(ఖండం)[i].sp_తదుపరి_list);
	vim_free(SYN_వస్తువులు(ఖండం)[i].sp_syn.cont_లోపల_list);
    }
}

/*
 * Clear and free one syntax cluster.
 */
    స్థిర శూన్య
syn_clear_cluster(synblock_T *ఖండం, పూర్ణాంకం i)
{
    vim_free(SYN_CLSTR(ఖండం)[i].scl_name);
    vim_free(SYN_CLSTR(ఖండం)[i].scl_name_u);
    vim_free(SYN_CLSTR(ఖండం)[i].scl_list);
}

/*
 * Handle ":syntax clear" command.
 */
    స్థిర శూన్య
syn_cmd_clear(exarg_T *eap, పూర్ణాంకం syncing)
{
    char_u	*arg = eap->arg;
    char_u	*arg_చివర;
    పూర్ణాంకం		id;

    eap->nextcmd = కనుగొను_nextcmd(arg);
    ఒకవేళ (eap->skip)
	తిరిగి;

    /*
     * We have to disable this within ":syn include @group filename",
     * because otherwise @group would get deleted.
     * Only required for Vim 5.x syntax files, 6.0 ones don't contain ":syn
     * clear".
     */
    ఒకవేళ (curwin->w_s->b_syn_topgrp != 0)
	తిరిగి;

    ఒకవేళ (చివర్లు_excmd2(eap->cmd, arg))
    {
	/*
	 * No argument: Clear all syntax items.
	 */
	ఒకవేళ (syncing)
	    syntax_sync_clear();
	లేకపోతే
	{
	    syntax_clear(curwin->w_s);
	    ఒకవేళ (curwin->w_s == &curwin->w_బఫరు->b_s)
		do_unlet((char_u *)"b:current_syntax", TRUE);
	    do_unlet((char_u *)"w:current_syntax", TRUE);
	}
    }
    లేకపోతే
    {
	/*
	 * Clear the group IDs that are in the argument.
	 */
	అయితే (!చివర్లు_excmd2(eap->cmd, arg))
	{
	    arg_చివర = skiptowhite(arg);
	    ఒకవేళ (*arg == '@')
	    {
		id = syn_scl_namen2id(arg + 1, (పూర్ణాంకం)(arg_చివర - arg - 1));
		ఒకవేళ (id == 0)
		{
		    semsg(_(e_no_such_syntax_cluster_str_1), arg);
		    ఆపు;
		}
		లేకపోతే
		{
		    /*
		     * We can't physically delete a cluster without changing
		     * the IDs of other clusters, so we do the next best thing
		     * and make it empty.
		     */
		    పొట్టి scl_id = id - SYNID_CLUSTER;

		    VIM_CLEAR(SYN_CLSTR(curwin->w_s)[scl_id].scl_list);
		}
	    }
	    లేకపోతే
	    {
		id = syn_namen2id(arg, (పూర్ణాంకం)(arg_చివర - arg));
		ఒకవేళ (id == 0)
		{
		    semsg(_(e_no_such_highlight_సమూహం_name_str), arg);
		    ఆపు;
		}
		లేకపోతే
		    syn_clear_one(id, syncing);
	    }
	    arg = skipwhite(arg_చివర);
	}
    }
    redraw_curbuf_later(UPD_SOME_VALID);
    syn_దొంతర_free_all(curwin->w_s);		// Need to recompute all syntax.
}

/*
 * Clear one syntax group for the current buffer.
 */
    స్థిర శూన్య
syn_clear_one(పూర్ణాంకం id, పూర్ణాంకం syncing)
{
    synpat_T	*spp;
    పూర్ణాంకం		idx;

    // Clear keywords only when not ":syn sync clear group-name"
    ఒకవేళ (!syncing)
    {
	(శూన్య)syn_clear_keyword(id, &curwin->w_s->b_keywtab);
	(శూన్య)syn_clear_keyword(id, &curwin->w_s->b_keywtab_ic);
    }

    // clear the patterns for "id"
    కొరకు (idx = curwin->w_s->b_syn_patterns.ga_len; --idx >= 0; )
    {
	spp = &(SYN_వస్తువులు(curwin->w_s)[idx]);
	ఒకవేళ (spp->sp_syn.id != id || spp->sp_syncing != syncing)
	    కొనసాగు;
	syn_remove_నమూనా(curwin->w_s, idx);
    }
}

/*
 * Handle ":syntax on" command.
 */
    స్థిర శూన్య
syn_cmd_on(exarg_T *eap, పూర్ణాంకం syncing వాడని)
{
    syn_cmd_onoff(eap, "syntax");
}

/*
 * Handle ":syntax enable" command.
 */
    స్థిర శూన్య
syn_cmd_enable(exarg_T *eap, పూర్ణాంకం syncing వాడని)
{
    సమితి_అంతర్గత_string_var((char_u *)"g:syntax_cmd", (char_u *)"enable");
    syn_cmd_onoff(eap, "syntax");
    do_unlet((char_u *)"g:syntax_cmd", TRUE);
}

/*
 * Handle ":syntax reset" command.
 * It actually resets highlighting, not syntax.
 */
    స్థిర శూన్య
syn_cmd_reset(exarg_T *eap, పూర్ణాంకం syncing వాడని)
{
    సమితి_nextcmd(eap, eap->arg);
    ఒకవేళ (!eap->skip)
    {
	సమితి_అంతర్గత_string_var((char_u *)"g:syntax_cmd", (char_u *)"reset");
	do_cmdline_cmd((char_u *)"runtime! syntax/syncolor.vim");
	do_unlet((char_u *)"g:syntax_cmd", TRUE);
    }
}

/*
 * Handle ":syntax manual" command.
 */
    స్థిర శూన్య
syn_cmd_manual(exarg_T *eap, పూర్ణాంకం syncing వాడని)
{
    syn_cmd_onoff(eap, "manual");
}

/*
 * Handle ":syntax off" command.
 */
    స్థిర శూన్య
syn_cmd_off(exarg_T *eap, పూర్ణాంకం syncing వాడని)
{
    syn_cmd_onoff(eap, "nosyntax");
}

    స్థిర శూన్య
syn_cmd_onoff(exarg_T *eap, అక్షరం *name)
{
    char_u	buf[100];

    సమితి_nextcmd(eap, eap->arg);
    ఒకవేళ (!eap->skip)
    {
	STRCPY(buf, "so ");
	vim_snprintf((అక్షరం *)buf + 3, పరిమాణం(buf) - 3, SYNTAX_FNAME, name);
	do_cmdline_cmd(buf);
    }
}

/*
 * Handle ":syntax [list]" command: list current syntax words.
 */
    స్థిర శూన్య
syn_cmd_list(
    exarg_T	*eap,
    పూర్ణాంకం		syncing)	    // when TRUE: list syncing items
{
    char_u	*arg = eap->arg;
    పూర్ణాంకం		id;
    char_u	*arg_చివర;

    eap->nextcmd = కనుగొను_nextcmd(arg);
    ఒకవేళ (eap->skip)
	తిరిగి;

    ఒకవేళ (!syntax_present(curwin))
    {
	సందేశం(_(సందేశం_no_వస్తువులు));
	తిరిగి;
    }

    ఒకవేళ (syncing)
    {
	ఒకవేళ (curwin->w_s->b_syn_sync_flags & SF_CCOMMENT)
	{
	    సందేశం_puts(_("syncing on C-style comments"));
	    syn_పంక్తులు_సందేశం();
	    syn_సరిపోలిక_సందేశం();
	}
	లేకపోతే ఒకవేళ (curwin->w_s->b_syn_sync_flags & SF_సరిపోలిక)
	{
	    సందేశం_puts_title(_("\n--- Syntax sync items ---"));
	    ఒకవేళ (curwin->w_s->b_syn_sync_minlines > 0
		|| curwin->w_s->b_syn_sync_maxlines > 0
		|| curwin->w_s->b_syn_sync_linebreaks > 0)
	    {
		సందేశం_puts(_("\nsyncing on items"));
		syn_పంక్తులు_సందేశం();
		syn_సరిపోలిక_సందేశం();
	    }
	    కొరకు (id = 1; id <= highlight_అంకె_groups() && !got_int; ++id)
		syn_list_one(id, syncing, FALSE);
	}
	లేకపోతే
	{
	    ఒకవేళ (curwin->w_s->b_syn_sync_minlines == 0)
		సందేశం_puts(_("no syncing"));
	    లేకపోతే
	    {
		ఒకవేళ (curwin->w_s->b_syn_sync_minlines == MAXLNUM)
		    సందేశం_puts(_("syncing starts at the first line"));
		లేకపోతే
		{
		    సందేశం_puts(_("syncing starts "));
		    సందేశం_outnum(curwin->w_s->b_syn_sync_minlines);
		    సందేశం_puts(_(" lines before top line"));
		}
		syn_సరిపోలిక_సందేశం();
	    }
	}
	తిరిగి;
    }

    సందేశం_puts_title(_("\n--- Syntax items ---"));
    ఒకవేళ (చివర్లు_excmd2(eap->cmd, arg))
    {
	/*
	 * No argument: List all group IDs and all syntax clusters.
	 */
	కొరకు (id = 1; id <= highlight_అంకె_groups() && !got_int; ++id)
	    syn_list_one(id, syncing, FALSE);
	కొరకు (id = 0; id < curwin->w_s->b_syn_clusters.ga_len && !got_int; ++id)
	    syn_list_cluster(id);
    }
    లేకపోతే
    {
	/*
	 * List the group IDs and syntax clusters that are in the argument.
	 */
	అయితే (!చివర్లు_excmd2(eap->cmd, arg) && !got_int)
	{
	    arg_చివర = skiptowhite(arg);
	    ఒకవేళ (*arg == '@')
	    {
		id = syn_scl_namen2id(arg + 1, (పూర్ణాంకం)(arg_చివర - arg - 1));
		ఒకవేళ (id == 0)
		    semsg(_(e_no_such_syntax_cluster_str_2), arg);
		లేకపోతే
		    syn_list_cluster(id - SYNID_CLUSTER);
	    }
	    లేకపోతే
	    {
		id = syn_namen2id(arg, (పూర్ణాంకం)(arg_చివర - arg));
		ఒకవేళ (id == 0)
		    semsg(_(e_no_such_highlight_సమూహం_name_str), arg);
		లేకపోతే
		    syn_list_one(id, syncing, TRUE);
	    }
	    arg = skipwhite(arg_చివర);
	}
    }
    సమితి_nextcmd(eap, arg);
}

    స్థిర శూన్య
syn_పంక్తులు_సందేశం(శూన్య)
{
    ఒకవేళ (curwin->w_s->b_syn_sync_maxlines > 0
				      || curwin->w_s->b_syn_sync_minlines > 0)
    {
	సందేశం_puts("; ");
	ఒకవేళ (curwin->w_s->b_syn_sync_minlines == MAXLNUM)
	    సందేశం_puts(_("from the first line"));
	లేకపోతే
	{
	    ఒకవేళ (curwin->w_s->b_syn_sync_minlines > 0)
	    {
		సందేశం_puts(_("minimal "));
		సందేశం_outnum(curwin->w_s->b_syn_sync_minlines);
		ఒకవేళ (curwin->w_s->b_syn_sync_maxlines)
		    సందేశం_puts(", ");
	    }
	    ఒకవేళ (curwin->w_s->b_syn_sync_maxlines > 0)
	    {
		సందేశం_puts(_("maximal "));
		సందేశం_outnum(curwin->w_s->b_syn_sync_maxlines);
	    }
	    సందేశం_puts(_(" lines before top line"));
	}
    }
}

    స్థిర శూన్య
syn_సరిపోలిక_సందేశం(శూన్య)
{
    ఒకవేళ (curwin->w_s->b_syn_sync_linebreaks > 0)
    {
	సందేశం_puts(_("; match "));
	సందేశం_outnum(curwin->w_s->b_syn_sync_linebreaks);
	సందేశం_puts(_(" line breaks"));
    }
}

స్థిర పూర్ణాంకం  చివరి_matchgroup;

స్థిర శూన్య syn_list_flags(keyvalue_T *nlist, పూర్ణాంకం nr_entries, పూర్ణాంకం flags, పూర్ణాంకం attr);

/*
 * List one syntax item, for ":syntax" or "syntax list syntax_name".
 */
    స్థిర శూన్య
syn_list_one(
    పూర్ణాంకం		id,
    పూర్ణాంకం		syncing,	    // when TRUE: list syncing items
    పూర్ణాంకం		link_only)	    // when TRUE; list link-only too
{
    పూర్ణాంకం		attr;
    పూర్ణాంకం		idx;
    పూర్ణాంకం		did_శీర్షిక = FALSE;
    synpat_T	*spp;
    స్థిర keyvalue_T namelist1[] =
		{
		    KEYVALUE_నమోదు(HL_DISPLAY, "display"),
		    KEYVALUE_నమోదు(HL_CONTAINED, "contained"),
		    KEYVALUE_నమోదు(HL_ONELINE, "oneline"),
		    KEYVALUE_నమోదు(HL_KEEPEND, "keepend"),
		    KEYVALUE_నమోదు(HL_EXTEND, "extend"),
		    KEYVALUE_నమోదు(HL_EXCLUDENL, "excludenl"),
		    KEYVALUE_నమోదు(HL_TRANSP, "transparent"),
		    KEYVALUE_నమోదు(HL_FOLD, "fold")
#నిర్వచితమైతే FEAT_CONCEAL
		    ,
		    KEYVALUE_నమోదు(HL_CONCEAL, "conceal"),
		    KEYVALUE_నమోదు(HL_CONCEALENDS, "concealends")
#అయితే_ముగింపు
		};
    స్థిర keyvalue_T namelist2[] =
		{
		    KEYVALUE_నమోదు(HL_SKIPWHITE, "skipwhite"),
		    KEYVALUE_నమోదు(HL_SKIPNL, "skipnl"),
		    KEYVALUE_నమోదు(HL_SKIPEMPTY, "skipempty")
		};

    attr = HL_ATTR(HLF_D);		// highlight like directories

    // list the keywords for "id"
    ఒకవేళ (!syncing)
    {
	did_శీర్షిక = syn_list_keywords(id, &curwin->w_s->b_keywtab, FALSE, attr);
	did_శీర్షిక = syn_list_keywords(id, &curwin->w_s->b_keywtab_ic,
							    did_శీర్షిక, attr);
    }

    // list the patterns for "id"
    కొరకు (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len && !got_int; ++idx)
    {
	spp = &(SYN_వస్తువులు(curwin->w_s)[idx]);
	ఒకవేళ (spp->sp_syn.id != id || spp->sp_syncing != syncing)
	    కొనసాగు;

	(శూన్య)syn_list_శీర్షిక(did_శీర్షిక, 999, id);
	did_శీర్షిక = TRUE;
	చివరి_matchgroup = 0;
	ఒకవేళ (spp->sp_type == SPTYPE_సరిపోలిక)
	{
	    put_నమూనా("match", ' ', spp, attr);
	    సందేశం_putchar(' ');
	}
	లేకపోతే ఒకవేళ (spp->sp_type == SPTYPE_ప్రారంభం)
	{
	    అయితే (SYN_వస్తువులు(curwin->w_s)[idx].sp_type == SPTYPE_ప్రారంభం)
		put_నమూనా("start", '=', &SYN_వస్తువులు(curwin->w_s)[idx++], attr);
	    ఒకవేళ (SYN_వస్తువులు(curwin->w_s)[idx].sp_type == SPTYPE_SKIP)
		put_నమూనా("skip", '=', &SYN_వస్తువులు(curwin->w_s)[idx++], attr);
	    అయితే (idx < curwin->w_s->b_syn_patterns.ga_len
			  && SYN_వస్తువులు(curwin->w_s)[idx].sp_type == SPTYPE_చివర)
		put_నమూనా("end", '=', &SYN_వస్తువులు(curwin->w_s)[idx++], attr);
	    --idx;
	    సందేశం_putchar(' ');
	}
	syn_list_flags(namelist1, (పూర్ణాంకం)ARRAY_నిడివి(namelist1), spp->sp_flags, attr);

	ఒకవేళ (spp->sp_cont_list != ఖాళీ)
	    put_id_list((char_u *)"contains", spp->sp_cont_list, attr);

	ఒకవేళ (spp->sp_syn.cont_లోపల_list != ఖాళీ)
	    put_id_list((char_u *)"containedin",
					      spp->sp_syn.cont_లోపల_list, attr);

	ఒకవేళ (spp->sp_తదుపరి_list != ఖాళీ)
	{
	    put_id_list((char_u *)"nextgroup", spp->sp_తదుపరి_list, attr);
	    syn_list_flags(namelist2, (పూర్ణాంకం)ARRAY_నిడివి(namelist2), spp->sp_flags, attr);
	}
	ఒకవేళ (spp->sp_flags & (HL_SYNC_ఇక్కడ|HL_SYNC_THERE))
	{
	    ఒకవేళ (spp->sp_flags & HL_SYNC_ఇక్కడ)
		సందేశం_puts_attr("grouphere", attr);
	    లేకపోతే
		సందేశం_puts_attr("groupthere", attr);
	    సందేశం_putchar(' ');
	    ఒకవేళ (spp->sp_sync_idx >= 0)
		సందేశం_outtrans(highlight_సమూహం_name(SYN_వస్తువులు(curwin->w_s)
				   [spp->sp_sync_idx].sp_syn.id - 1));
	    లేకపోతే
		సందేశం_puts("NONE");
	    సందేశం_putchar(' ');
	}
    }

    // list the link, if there is one
    ఒకవేళ (highlight_link_id(id - 1) && (did_శీర్షిక || link_only) && !got_int)
    {
	(శూన్య)syn_list_శీర్షిక(did_శీర్షిక, 999, id);
	సందేశం_puts_attr("links to", attr);
	సందేశం_putchar(' ');
	సందేశం_outtrans(highlight_సమూహం_name(highlight_link_id(id - 1) - 1));
    }
}

    స్థిర శూన్య
syn_list_flags(keyvalue_T *nlist, పూర్ణాంకం nr_entries, పూర్ణాంకం flags, పూర్ణాంకం attr)
{
    పూర్ణాంకం		i;

    కొరకు (i = 0; i < nr_entries; ++i)
	ఒకవేళ (flags & nlist[i].కీలకం)
	{
	    సందేశం_puts_attr((అక్షరం *)nlist[i].విలువ.string, attr);
	    సందేశం_putchar(' ');
	}
}

/*
 * List one syntax cluster, for ":syntax" or "syntax list syntax_name".
 */
    స్థిర శూన్య
syn_list_cluster(పూర్ణాంకం id)
{
    పూర్ణాంకం	    endcol = 15;

    // slight hack:  roughly duplicate the guts of syn_list_header()
    సందేశం_putchar('\n');
    సందేశం_outtrans(SYN_CLSTR(curwin->w_s)[id].scl_name);

    ఒకవేళ (సందేశం_నిలువు >= endcol)	// output at least one space
	endcol = సందేశం_నిలువు + 1;
    ఒకవేళ (cmdline_వెడల్పు <= endcol)	// avoid hang for tiny window
	endcol = cmdline_వెడల్పు - 1;

    సందేశం_advance(endcol);
    ఒకవేళ (SYN_CLSTR(curwin->w_s)[id].scl_list != ఖాళీ)
    {
	put_id_list((char_u *)"cluster", SYN_CLSTR(curwin->w_s)[id].scl_list,
		    HL_ATTR(HLF_D));
    }
    లేకపోతే
    {
	సందేశం_puts_attr("cluster", HL_ATTR(HLF_D));
	సందేశం_puts("=NONE");
    }
}

    స్థిర శూన్య
put_id_list(char_u *name, పొట్టి *list, పూర్ణాంకం attr)
{
    పొట్టి		*p;

    సందేశం_puts_attr((అక్షరం *)name, attr);
    సందేశం_putchar('=');
    కొరకు (p = list; *p; ++p)
    {
	ఒకవేళ (*p >= SYNID_ALLBUT && *p < SYNID_శిఖరం)
	{
	    ఒకవేళ (p[1])
		సందేశం_puts("ALLBUT");
	    లేకపోతే
		సందేశం_puts("ALL");
	}
	లేకపోతే ఒకవేళ (*p >= SYNID_శిఖరం && *p < SYNID_CONTAINED)
	{
	    సందేశం_puts("TOP");
	}
	లేకపోతే ఒకవేళ (*p >= SYNID_CONTAINED && *p < SYNID_CLUSTER)
	{
	    సందేశం_puts("CONTAINED");
	}
	లేకపోతే ఒకవేళ (*p >= SYNID_CLUSTER)
	{
	    పొట్టి scl_id = *p - SYNID_CLUSTER;

	    సందేశం_putchar('@');
	    సందేశం_outtrans(SYN_CLSTR(curwin->w_s)[scl_id].scl_name);
	}
	లేకపోతే
	    సందేశం_outtrans(highlight_సమూహం_name(*p - 1));
	ఒకవేళ (p[1])
	    సందేశం_putchar(',');
    }
    సందేశం_putchar(' ');
}

    స్థిర శూన్య
put_నమూనా(
    అక్షరం	*s,
    పూర్ణాంకం		c,
    synpat_T	*spp,
    పూర్ణాంకం		attr)
{
    దీర్ఘ	n;
    పూర్ణాంకం		ముసుగు;
    పూర్ణాంకం		మొదటి;
    స్థిర అక్షరం	*sepchars = "/+=-#@\"|'^&";
    పూర్ణాంకం		i;

    // May have to write "matchgroup=group"
    ఒకవేళ (చివరి_matchgroup != spp->sp_syn_సరిపోలిక_id)
    {
	చివరి_matchgroup = spp->sp_syn_సరిపోలిక_id;
	సందేశం_puts_attr("matchgroup", attr);
	సందేశం_putchar('=');
	ఒకవేళ (చివరి_matchgroup == 0)
	    సందేశం_outtrans((char_u *)"NONE");
	లేకపోతే
	    సందేశం_outtrans(highlight_సమూహం_name(చివరి_matchgroup - 1));
	సందేశం_putchar(' ');
    }

    // output the name of the pattern and an '=' or ' '
    సందేశం_puts_attr(s, attr);
    సందేశం_putchar(c);

    // output the pattern, in between a char that is not in the pattern
    కొరకు (i = 0; vim_strchr(spp->sp_నమూనా, sepchars[i]) != ఖాళీ; )
	ఒకవేళ (sepchars[++i] == NUL)
	{
	    i = 0;	// no good char found, just use the first one
	    ఆపు;
	}
    సందేశం_putchar(sepchars[i]);
    సందేశం_outtrans(spp->sp_నమూనా);
    సందేశం_putchar(sepchars[i]);

    // output any pattern options
    మొదటి = TRUE;
    కొరకు (i = 0; i < SPO_లెక్క; ++i)
    {
	ముసుగు = (1 << i);
	ఒకవేళ (spp->sp_off_flags & (ముసుగు + (ముసుగు << SPO_లెక్క)))
	{
	    ఒకవేళ (!మొదటి)
		సందేశం_putchar(',');	// separate with commas
	    సందేశం_puts(spo_name_tab[i]);
	    n = spp->sp_offsets[i];
	    ఒకవేళ (i != SPO_LC_OFF)
	    {
		ఒకవేళ (spp->sp_off_flags & ముసుగు)
		    సందేశం_putchar('s');
		లేకపోతే
		    సందేశం_putchar('e');
		ఒకవేళ (n > 0)
		    సందేశం_putchar('+');
	    }
	    ఒకవేళ (n || i == SPO_LC_OFF)
		సందేశం_outnum(n);
	    మొదటి = FALSE;
	}
    }
    సందేశం_putchar(' ');
}

/*
 * List or clear the keywords for one syntax group.
 * Return TRUE if the header has been printed.
 */
    స్థిర పూర్ణాంకం
syn_list_keywords(
    పూర్ణాంకం		id,
    hashtab_T	*ht,
    పూర్ణాంకం		did_శీర్షిక,		// header has already been printed
    పూర్ణాంకం		attr)
{
    పూర్ణాంకం		outlen;
    hashitem_T	*hi;
    keyentry_T	*kp;
    పూర్ణాంకం		todo;
    పూర్ణాంకం		prev_contained = 0;
    పొట్టి	*prev_తదుపరి_list = ఖాళీ;
    పొట్టి	*prev_cont_లోపల_list = ఖాళీ;
    పూర్ణాంకం		prev_skipnl = 0;
    పూర్ణాంకం		prev_skipwhite = 0;
    పూర్ణాంకం		prev_skipempty = 0;

    /*
     * Unfortunately, this list of keywords is not sorted on alphabet but on
     * hash value...
     */
    todo = (పూర్ణాంకం)ht->ht_వాడిన;
    కొరకు (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    --todo;
	    కొరకు (kp = HI2KE(hi); kp != ఖాళీ && !got_int; kp = kp->ke_తదుపరి)
	    {
		ఒకవేళ (kp->k_syn.id == id)
		{
		    ఒకవేళ (prev_contained != (kp->flags & HL_CONTAINED)
			    || prev_skipnl != (kp->flags & HL_SKIPNL)
			    || prev_skipwhite != (kp->flags & HL_SKIPWHITE)
			    || prev_skipempty != (kp->flags & HL_SKIPEMPTY)
			    || prev_cont_లోపల_list != kp->k_syn.cont_లోపల_list
			    || prev_తదుపరి_list != kp->తదుపరి_list)
			outlen = 9999;
		    లేకపోతే
			outlen = (పూర్ణాంకం)STRLEN(kp->keyword);
		    // output "contained" and "nextgroup" on each line
		    ఒకవేళ (syn_list_శీర్షిక(did_శీర్షిక, outlen, id))
		    {
			prev_contained = 0;
			prev_తదుపరి_list = ఖాళీ;
			prev_cont_లోపల_list = ఖాళీ;
			prev_skipnl = 0;
			prev_skipwhite = 0;
			prev_skipempty = 0;
		    }
		    did_శీర్షిక = TRUE;
		    ఒకవేళ (prev_contained != (kp->flags & HL_CONTAINED))
		    {
			సందేశం_puts_attr("contained", attr);
			సందేశం_putchar(' ');
			prev_contained = (kp->flags & HL_CONTAINED);
		    }
		    ఒకవేళ (kp->k_syn.cont_లోపల_list != prev_cont_లోపల_list)
		    {
			put_id_list((char_u *)"containedin",
						kp->k_syn.cont_లోపల_list, attr);
			సందేశం_putchar(' ');
			prev_cont_లోపల_list = kp->k_syn.cont_లోపల_list;
		    }
		    ఒకవేళ (kp->తదుపరి_list != prev_తదుపరి_list)
		    {
			put_id_list((char_u *)"nextgroup", kp->తదుపరి_list, attr);
			సందేశం_putchar(' ');
			prev_తదుపరి_list = kp->తదుపరి_list;
			ఒకవేళ (kp->flags & HL_SKIPNL)
			{
			    సందేశం_puts_attr("skipnl", attr);
			    సందేశం_putchar(' ');
			    prev_skipnl = (kp->flags & HL_SKIPNL);
			}
			ఒకవేళ (kp->flags & HL_SKIPWHITE)
			{
			    సందేశం_puts_attr("skipwhite", attr);
			    సందేశం_putchar(' ');
			    prev_skipwhite = (kp->flags & HL_SKIPWHITE);
			}
			ఒకవేళ (kp->flags & HL_SKIPEMPTY)
			{
			    సందేశం_puts_attr("skipempty", attr);
			    సందేశం_putchar(' ');
			    prev_skipempty = (kp->flags & HL_SKIPEMPTY);
			}
		    }
		    సందేశం_outtrans(kp->keyword);
		}
	    }
	}
    }

    తిరిగి did_శీర్షిక;
}

    స్థిర శూన్య
syn_clear_keyword(పూర్ణాంకం id, hashtab_T *ht)
{
    hashitem_T	*hi;
    keyentry_T	*kp;
    keyentry_T	*kp_prev;
    keyentry_T	*kp_తదుపరి;
    పూర్ణాంకం		todo;

    hash_తాళం(ht);
    todo = (పూర్ణాంకం)ht->ht_వాడిన;
    FOR_ALL_HASHTAB_వస్తువులు(ht, hi, todo)
    {
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    --todo;
	    kp_prev = ఖాళీ;
	    కొరకు (kp = HI2KE(hi); kp != ఖాళీ; )
	    {
		ఒకవేళ (kp->k_syn.id == id)
		{
		    kp_తదుపరి = kp->ke_తదుపరి;
		    ఒకవేళ (kp_prev == ఖాళీ)
		    {
			ఒకవేళ (kp_తదుపరి == ఖాళీ)
			    hash_remove(ht, hi, "syntax clear keyword");
			లేకపోతే
			    hi->hi_కీలకం = KE2HIKEY(kp_తదుపరి);
		    }
		    లేకపోతే
			kp_prev->ke_తదుపరి = kp_తదుపరి;
		    vim_free(kp->తదుపరి_list);
		    vim_free(kp->k_syn.cont_లోపల_list);
		    vim_free(kp);
		    kp = kp_తదుపరి;
		}
		లేకపోతే
		{
		    kp_prev = kp;
		    kp = kp->ke_తదుపరి;
		}
	    }
	}
    }
    hash_unlock(ht);
}

/*
 * Clear a whole keyword table.
 */
    స్థిర శూన్య
clear_keywtab(hashtab_T *ht)
{
    hashitem_T	*hi;
    పూర్ణాంకం		todo;
    keyentry_T	*kp;
    keyentry_T	*kp_తదుపరి;

    todo = (పూర్ణాంకం)ht->ht_వాడిన;
    FOR_ALL_HASHTAB_వస్తువులు(ht, hi, todo)
    {
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    --todo;
	    కొరకు (kp = HI2KE(hi); kp != ఖాళీ; kp = kp_తదుపరి)
	    {
		kp_తదుపరి = kp->ke_తదుపరి;
		vim_free(kp->తదుపరి_list);
		vim_free(kp->k_syn.cont_లోపల_list);
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
    స్థిర శూన్య
కూడు_keyword(
    char_u	*name,	    // name of keyword
    size_t	namelen,    // length of keyword (excluding the NUL)
    పూర్ణాంకం		id,	    // group ID for this keyword
    పూర్ణాంకం		flags,	    // flags for this keyword
    పొట్టి	*cont_లోపల_list, // containedin for this keyword
    పొట్టి	*తదుపరి_list, // nextgroup for this keyword
    పూర్ణాంకం		conceal_char)
{
    keyentry_T	*kp;
    hashtab_T	*ht;
    hashitem_T	*hi;
    char_u	*name_ic;
    size_t	name_iclen;
    long_u	hash;
    char_u	name_folded[MAXKEYWLEN + 1];

    ఒకవేళ (curwin->w_s->b_syn_ic)
    {
	name_ic = str_foldcase(name, (పూర్ణాంకం)namelen,
						 name_folded, MAXKEYWLEN + 1);
	name_iclen = STRLEN(name_ic);
    }
    లేకపోతే
    {
	name_ic = name;
	name_iclen = namelen;
    }
    kp = alloc(offsetof(keyentry_T, keyword) + name_iclen + 1);
    ఒకవేళ (kp == ఖాళీ)
	తిరిగి;
    STRCPY(kp->keyword, name_ic);
    kp->k_syn.id = id;
    kp->k_syn.inc_ట్యాగ్ = current_syn_inc_ట్యాగ్;
    kp->flags = flags;
    kp->k_char = conceal_char;
    kp->k_syn.cont_లోపల_list = copy_id_list(cont_లోపల_list);
    ఒకవేళ (cont_లోపల_list != ఖాళీ)
	curwin->w_s->b_syn_containedin = TRUE;
    kp->తదుపరి_list = copy_id_list(తదుపరి_list);

    ఒకవేళ (curwin->w_s->b_syn_ic)
	ht = &curwin->w_s->b_keywtab_ic;
    లేకపోతే
	ht = &curwin->w_s->b_keywtab;

    hash = hash_hash(kp->keyword);
    hi = hash_lookup(ht, kp->keyword, hash);
    ఒకవేళ (HASHITEM_ఖాళీలు(hi))
    {
	// new keyword, add to hashtable
	kp->ke_తదుపరి = ఖాళీ;
	hash_కూడు_అంశం(ht, hi, kp->keyword, hash);
    }
    లేకపోతే
    {
	// keyword already exists, prepend to list
	kp->ke_తదుపరి = HI2KE(hi);
	hi->hi_కీలకం = KE2HIKEY(kp);
    }
}

/*
 * Get the start and end of the group name argument.
 * Return a pointer to the first argument.
 * Return NULL if the end of the command was found instead of further args.
 */
    స్థిర char_u *
get_సమూహం_name(
    char_u	*arg,		// start of the argument
    char_u	**name_చివర)	// pointer to end of the name
{
    char_u	*rest;

    *name_చివర = skiptowhite(arg);
    rest = skipwhite(*name_చివర);

    /*
     * Check if there are enough arguments.  The first argument may be a
     * pattern, where '|' is allowed, so only check for NUL.
     */
    ఒకవేళ (చివర్లు_excmd(*arg) || *rest == NUL)
	తిరిగి ఖాళీ;
    తిరిగి rest;
}

/*
 * Check for syntax command option arguments.
 * This can be called at any place in the list of arguments, and just picks
 * out the arguments that are known.  Can be called several times in a row to
 * collect all options in between other arguments.
 * Return a pointer to the next argument (which isn't an option).
 * Return NULL for any error;
 */
    స్థిర char_u *
get_syn_options(
    char_u	    *ప్రారంభం,		// next argument to be checked
    syn_opt_arg_T   *opt,		// various things
    పూర్ణాంకం		    *conceal_char వాడని,
    పూర్ణాంకం		    skip)		// TRUE if skipping over command
{
    char_u	*arg = ప్రారంభం;
    char_u	*gname_ప్రారంభం, *gname;
    పూర్ణాంకం		syn_id;
    పూర్ణాంకం		len;
    అక్షరం	*p;
    పూర్ణాంకం		i;
    పూర్ణాంకం		fidx;
    స్థిర నిర్మాణం flag
    {
	అక్షరం	*name;
	పూర్ణాంకం	argtype;
	పూర్ణాంకం	flags;
    } flagtab[] = { {"cCoOnNtTaAiInNeEdD",	0,	HL_CONTAINED},
		    {"oOnNeElLiInNeE",		0,	HL_ONELINE},
		    {"kKeEeEpPeEnNdD",		0,	HL_KEEPEND},
		    {"eExXtTeEnNdD",		0,	HL_EXTEND},
		    {"eExXcClLuUdDeEnNlL",	0,	HL_EXCLUDENL},
		    {"tTrRaAnNsSpPaArReEnNtT",	0,	HL_TRANSP},
		    {"sSkKiIpPnNlL",		0,	HL_SKIPNL},
		    {"sSkKiIpPwWhHiItTeE",	0,	HL_SKIPWHITE},
		    {"sSkKiIpPeEmMpPtTyY",	0,	HL_SKIPEMPTY},
		    {"gGrRoOuUpPhHeErReE",	0,	HL_SYNC_ఇక్కడ},
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
    స్థిర అక్షరం *మొదటి_letters = "cCoOkKeEtTsSgGdDfFnN";

    ఒకవేళ (arg == ఖాళీ)		// already detected error
	తిరిగి ఖాళీ;

#నిర్వచితమైతే FEAT_CONCEAL
    ఒకవేళ (curwin->w_s->b_syn_conceal)
	opt->flags |= HL_CONCEAL;
#అయితే_ముగింపు

    కొరకు (;;)
    {
	/*
	 * This is used very often when a large number of keywords is defined.
	 * Need to skip quickly when no option name is found.
	 * Also avoid tolower(), it's slow.
	 */
	ఒకవేళ (strchr(మొదటి_letters, *arg) == ఖాళీ)
	    ఆపు;

	కొరకు (fidx = ARRAY_నిడివి(flagtab); --fidx >= 0; )
	{
	    p = flagtab[fidx].name;
	    కొరకు (i = 0, len = 0; p[i] != NUL; i += 2, ++len)
		ఒకవేళ (arg[len] != p[i] && arg[len] != p[i + 1])
		    ఆపు;
	    ఒకవేళ (p[i] == NUL && (VIM_ISWHITE(arg[len])
				    || (flagtab[fidx].argtype > 0
					 ? arg[len] == '='
					 : చివర్లు_excmd2(ప్రారంభం, arg + len))))
	    {
		ఒకవేళ (opt->keyword
			&& (flagtab[fidx].flags == HL_DISPLAY
			    || flagtab[fidx].flags == HL_FOLD
			    || flagtab[fidx].flags == HL_EXTEND))
		    // treat "display", "fold" and "extend" as a keyword
		    fidx = -1;
		ఆపు;
	    }
	}
	ఒకవేళ (fidx < 0)	    // no match found
	    ఆపు;

	ఒకవేళ (flagtab[fidx].argtype == 1)
	{
	    ఒకవేళ (!opt->has_cont_list)
	    {
		emsg(_(e_కలిగియుంది_వాదం_not_accepted_ఇక్కడ));
		తిరిగి ఖాళీ;
	    }
	    ఒకవేళ (get_id_list(&arg, 8, &opt->cont_list, skip) == FAIL)
		తిరిగి ఖాళీ;
	}
	లేకపోతే ఒకవేళ (flagtab[fidx].argtype == 2)
	{
	    ఒకవేళ (get_id_list(&arg, 11, &opt->cont_లోపల_list, skip) == FAIL)
		తిరిగి ఖాళీ;
	}
	లేకపోతే ఒకవేళ (flagtab[fidx].argtype == 3)
	{
	    ఒకవేళ (get_id_list(&arg, 9, &opt->తదుపరి_list, skip) == FAIL)
		తిరిగి ఖాళీ;
	}
	లేకపోతే ఒకవేళ (flagtab[fidx].argtype == 11 && arg[5] == '=')
	{
	    // cchar=?
	    ఒకవేళ (has_mbyte)
	    {
#నిర్వచితమైతే FEAT_CONCEAL
		*conceal_char = mb_ptr2char(arg + 6);
#అయితే_ముగింపు
		arg += mb_ptr2len(arg + 6) - 1;
	    }
	    లేకపోతే
	    {
#నిర్వచితమైతే FEAT_CONCEAL
		*conceal_char = arg[6];
#లేకపోతే
		;
#అయితే_ముగింపు
	    }
#నిర్వచితమైతే FEAT_CONCEAL
	    ఒకవేళ (!vim_isprintc_strict(*conceal_char))
	    {
		emsg(_(e_invalid_cchar_విలువ));
		తిరిగి ఖాళీ;
	    }
#అయితే_ముగింపు
	    arg = skipwhite(arg + 7);
	}
	లేకపోతే
	{
	    opt->flags |= flagtab[fidx].flags;
	    arg = skipwhite(arg + len);

	    ఒకవేళ (flagtab[fidx].flags == HL_SYNC_ఇక్కడ
		    || flagtab[fidx].flags == HL_SYNC_THERE)
	    {
		ఒకవేళ (opt->sync_idx == ఖాళీ)
		{
		    emsg(_(e_groupthere_not_accepted_ఇక్కడ));
		    తిరిగి ఖాళీ;
		}
		gname_ప్రారంభం = arg;
		arg = skiptowhite(arg);
		ఒకవేళ (gname_ప్రారంభం == arg)
		    తిరిగి ఖాళీ;
		gname = vim_strnsave(gname_ప్రారంభం, arg - gname_ప్రారంభం);
		ఒకవేళ (gname == ఖాళీ)
		    తిరిగి ఖాళీ;
		ఒకవేళ (STRCMP(gname, "NONE") == 0)
		    *opt->sync_idx = NONE_IDX;
		లేకపోతే
		{
		    syn_id = syn_name2id(gname);
		    కొరకు (i = curwin->w_s->b_syn_patterns.ga_len; --i >= 0; )
			ఒకవేళ (SYN_వస్తువులు(curwin->w_s)[i].sp_syn.id == syn_id
			      && SYN_వస్తువులు(curwin->w_s)[i].sp_type
							       == SPTYPE_ప్రారంభం)
			{
			    *opt->sync_idx = i;
			    ఆపు;
			}
		    ఒకవేళ (i < 0)
		    {
			semsg(_(e_didnt_కనుగొను_region_అంశం_for_str), gname);
			vim_free(gname);
			తిరిగి ఖాళీ;
		    }
		}

		vim_free(gname);
		arg = skipwhite(arg);
	    }
#నిర్వచితమైతే FEAT_FOLDING
	    లేకపోతే ఒకవేళ (flagtab[fidx].flags == HL_FOLD
						&& foldmethodIsSyntax(curwin))
		// Need to update folds later.
		foldUpdateAll(curwin);
#అయితే_ముగింపు
	}
    }

    తిరిగి arg;
}

/*
 * Adjustments to syntax item when declared in a ":syn include"'d file.
 * Set the contained flag, and if the item is not already contained, add it
 * to the specified top-level group, if any.
 */
    స్థిర శూన్య
syn_incl_toplevel(పూర్ణాంకం id, పూర్ణాంకం *flagsp)
{
    ఒకవేళ ((*flagsp & HL_CONTAINED) || curwin->w_s->b_syn_topgrp == 0)
	తిరిగి;
    *flagsp |= HL_CONTAINED | HL_INCLUDED_TOPLEVEL;
    ఒకవేళ (curwin->w_s->b_syn_topgrp >= SYNID_CLUSTER)
    {
	// We have to alloc this, because syn_combine_list() will free it.
	పొట్టి	    *grp_list = ALLOC_MULT(పొట్టి, 2);
	పూర్ణాంకం	    tlg_id = curwin->w_s->b_syn_topgrp - SYNID_CLUSTER;

	ఒకవేళ (grp_list != ఖాళీ)
	{
	    grp_list[0] = id;
	    grp_list[1] = 0;
	    syn_combine_list(&SYN_CLSTR(curwin->w_s)[tlg_id].scl_list,
						       &grp_list, CLUSTER_కూడు);
	}
    }
}

/*
 * Handle ":syntax include [@{group-name}] filename" command.
 */
    స్థిర శూన్య
syn_cmd_include(exarg_T *eap, పూర్ణాంకం syncing వాడని)
{
    char_u	*arg = eap->arg;
    పూర్ణాంకం		sgl_id = 1;
    char_u	*సమూహం_name_చివర;
    char_u	*rest;
    అక్షరం	*errormsg = ఖాళీ;
    పూర్ణాంకం		prev_toplvl_grp;
    పూర్ణాంకం		prev_syn_inc_ట్యాగ్;
    పూర్ణాంకం		ఆధారం = FALSE;

    eap->nextcmd = కనుగొను_nextcmd(arg);
    ఒకవేళ (eap->skip)
	తిరిగి;

    ఒకవేళ (arg[0] == '@')
    {
	++arg;
	rest = get_సమూహం_name(arg, &సమూహం_name_చివర);
	ఒకవేళ (rest == ఖాళీ)
	{
	    emsg(_(e_filename_required));
	    తిరిగి;
	}
	sgl_id = syn_check_cluster(arg, (పూర్ణాంకం)(సమూహం_name_చివర - arg));
	ఒకవేళ (sgl_id == 0)
	    తిరిగి;
	// separate_nextcmd() and expand_filename() depend on this
	eap->arg = rest;
    }

    /*
     * Everything that's left, up to the next command, should be the
     * filename to include.
     */
    eap->argt |= (EX_XFILE | EX_NOSPC);
    separate_nextcmd(eap, FALSE);
    ఒకవేళ (*eap->arg == '<' || *eap->arg == '$' || mch_is_నిండిన_Name(eap->arg))
    {
	// For an absolute path, "$VIM/..." or "<sfile>.." we ":source" the
	// file.  Need to expand the file name first.  In other cases
	// ":runtime!" is used.
	ఆధారం = TRUE;
	ఒకవేళ (expand_filename(eap, syn_cmdlinep, &errormsg) == FAIL)
	{
	    ఒకవేళ (errormsg != ఖాళీ)
		emsg(errormsg);
	    తిరిగి;
	}
    }

    /*
     * Save and restore the existing top-level grouplist id and ":syn
     * include" tag around the actual inclusion.
     */
    ఒకవేళ (running_syn_inc_ట్యాగ్ >= MAX_SYN_INC_ట్యాగ్)
    {
	emsg(_(e_too_many_syntax_includes));
	తిరిగి;
    }
    prev_syn_inc_ట్యాగ్ = current_syn_inc_ట్యాగ్;
    current_syn_inc_ట్యాగ్ = ++running_syn_inc_ట్యాగ్;
    prev_toplvl_grp = curwin->w_s->b_syn_topgrp;
    curwin->w_s->b_syn_topgrp = sgl_id;
    ఒకవేళ (ఆధారం ? do_ఆధారం(eap->arg, FALSE, DOSO_NONE, ఖాళీ) == FAIL
				: ఆధారం_runtime(eap->arg, DIP_ALL) == FAIL)
	semsg(_(e_cant_open_file_str), eap->arg);
    curwin->w_s->b_syn_topgrp = prev_toplvl_grp;
    current_syn_inc_ట్యాగ్ = prev_syn_inc_ట్యాగ్;
}

/*
 * Handle ":syntax keyword {group-name} [{option}] keyword .." command.
 */
    స్థిర శూన్య
syn_cmd_keyword(exarg_T *eap, పూర్ణాంకం syncing వాడని)
{
    char_u	*arg = eap->arg;
    char_u	*సమూహం_name_చివర;
    పూర్ణాంకం		syn_id;
    char_u	*rest;
    char_u	*keyword_copy = ఖాళీ;
    char_u	*p;
    char_u	*kw;
    syn_opt_arg_T syn_opt_arg;
    పూర్ణాంకం		cnt;
    పూర్ణాంకం		conceal_char = NUL;

    rest = get_సమూహం_name(arg, &సమూహం_name_చివర);

    ఒకవేళ (rest != ఖాళీ)
    {
	ఒకవేళ (eap->skip)
	    syn_id = -1;
	లేకపోతే
	    syn_id = syn_check_సమూహం(arg, (పూర్ణాంకం)(సమూహం_name_చివర - arg));
	ఒకవేళ (syn_id != 0)
	    // allocate a buffer, for removing backslashes in the keyword
	    keyword_copy = alloc(STRLEN(rest) + 1);
	ఒకవేళ (keyword_copy != ఖాళీ)
	{
	    syn_opt_arg.flags = 0;
	    syn_opt_arg.keyword = TRUE;
	    syn_opt_arg.sync_idx = ఖాళీ;
	    syn_opt_arg.has_cont_list = FALSE;
	    syn_opt_arg.cont_లోపల_list = ఖాళీ;
	    syn_opt_arg.తదుపరి_list = ఖాళీ;

	    /*
	     * The options given apply to ALL keywords, so all options must be
	     * found before keywords can be created.
	     * 1: collect the options and copy the keywords to keyword_copy.
	     */
	    cnt = 0;
	    p = keyword_copy;
	    కొరకు ( ; rest != ఖాళీ && !చివర్లు_excmd2(eap->arg, rest);
							rest = skipwhite(rest))
	    {
		rest = get_syn_options(rest, &syn_opt_arg, &conceal_char,
								    eap->skip);
		ఒకవేళ (rest == ఖాళీ || చివర్లు_excmd2(eap->arg, rest))
		    ఆపు;
		// Copy the keyword, removing backslashes, and add a NUL.
		అయితే (*rest != NUL && !VIM_ISWHITE(*rest))
		{
		    ఒకవేళ (*rest == '\\' && rest[1] != NUL)
			++rest;
		    *p++ = *rest++;
		}
		*p++ = NUL;
		++cnt;
	    }

	    ఒకవేళ (!eap->skip)
	    {
		size_t	kwlen = 0;

		// Adjust flags for use of ":syn include".
		syn_incl_toplevel(syn_id, &syn_opt_arg.flags);

		/*
		 * 2: Add an entry for each keyword.
		 */
		కొరకు (kw = keyword_copy; --cnt >= 0; kw += kwlen + 1)
		{
		    కొరకు (p = vim_strchr(kw, '['); ; )
		    {
			ఒకవేళ (p == ఖాళీ)
			    kwlen = STRLEN(kw);
			లేకపోతే
			{
			    *p = NUL;
			    kwlen = (size_t)(p - kw);
			}
			కూడు_keyword(kw, kwlen, syn_id, syn_opt_arg.flags,
				syn_opt_arg.cont_లోపల_list,
					 syn_opt_arg.తదుపరి_list, conceal_char);
			ఒకవేళ (p == ఖాళీ)
			    ఆపు;
			ఒకవేళ (p[1] == NUL)
			{
			    semsg(_(e_error_missing_rsb_str), kw);
			    వెళ్ళు error;
			}
			ఒకవేళ (p[1] == ']')
			{
			    ఒకవేళ (p[2] != NUL)
			    {
				semsg(_(e_trailing_char_after_rsb_str_str),
								    kw, &p[2]);
				వెళ్ళు error;
			    }
			    kw = p + 1;		// skip over the "]"
			    kwlen = 1;
			    ఆపు;
			}
			ఒకవేళ (has_mbyte)
			{
			    పూర్ణాంకం l = (*mb_ptr2len)(p + 1);

			    mch_memmove(p, p + 1, l);
			    p += l;
			}
			లేకపోతే
			{
			    p[0] = p[1];
			    ++p;
			}
		    }
		}
	    }
error:
	    vim_free(keyword_copy);
	    vim_free(syn_opt_arg.cont_లోపల_list);
	    vim_free(syn_opt_arg.తదుపరి_list);
	}
    }

    ఒకవేళ (rest != ఖాళీ)
	సమితి_nextcmd(eap, rest);
    లేకపోతే
	semsg(_(e_invalid_వాదం_str), arg);

    redraw_curbuf_later(UPD_SOME_VALID);
    syn_దొంతర_free_all(curwin->w_s);		// Need to recompute all syntax.
}

/*
 * Handle ":syntax match {name} [{options}] {pattern} [{options}]".
 *
 * Also ":syntax sync match {name} [[grouphere | groupthere] {group-name}] .."
 */
    స్థిర శూన్య
syn_cmd_సరిపోలిక(
    exarg_T	*eap,
    పూర్ణాంకం		syncing)	    // TRUE for ":syntax sync match .. "
{
    char_u	*arg = eap->arg;
    char_u	*సమూహం_name_చివర;
    char_u	*rest;
    synpat_T	అంశం;		// the item found in the line
    పూర్ణాంకం		syn_id;
    పూర్ణాంకం		idx;
    syn_opt_arg_T syn_opt_arg;
    పూర్ణాంకం		sync_idx = 0;
    పూర్ణాంకం		conceal_char = NUL;
    పూర్ణాంకం		orig_called_emsg = called_emsg;

    // Isolate the group name, check for validity
    rest = get_సమూహం_name(arg, &సమూహం_name_చివర);

    // Get options before the pattern
    syn_opt_arg.flags = 0;
    syn_opt_arg.keyword = FALSE;
    syn_opt_arg.sync_idx = syncing ? &sync_idx : ఖాళీ;
    syn_opt_arg.has_cont_list = TRUE;
    syn_opt_arg.cont_list = ఖాళీ;
    syn_opt_arg.cont_లోపల_list = ఖాళీ;
    syn_opt_arg.తదుపరి_list = ఖాళీ;
    rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);

    // get the pattern.
    init_syn_patterns();
    CLEAR_క్షేత్రం(అంశం);
    rest = get_syn_నమూనా(rest, &అంశం);
    ఒకవేళ (vim_regcomp_had_eol() && !(syn_opt_arg.flags & HL_EXCLUDENL))
	syn_opt_arg.flags |= HL_HAS_EOL;

    // Get options after the pattern
    rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);

    ఒకవేళ (rest != ఖాళీ)		// all arguments are valid
    {
	/*
	 * Check for trailing command and illegal trailing arguments.
	 */
	సమితి_nextcmd(eap, rest);
	ఒకవేళ (!చివర్లు_excmd2(eap->cmd, rest) || eap->skip)
	    rest = ఖాళీ;
	లేకపోతే ఒకవేళ (ga_grow(&curwin->w_s->b_syn_patterns, 1) == OK
		&& (syn_id = syn_check_సమూహం(arg,
					   (పూర్ణాంకం)(సమూహం_name_చివర - arg))) != 0)
	{
	    syn_incl_toplevel(syn_id, &syn_opt_arg.flags);
	    /*
	     * Store the pattern in the syn_items list
	     */
	    idx = curwin->w_s->b_syn_patterns.ga_len;
	    SYN_వస్తువులు(curwin->w_s)[idx] = అంశం;
	    SYN_వస్తువులు(curwin->w_s)[idx].sp_syncing = syncing;
	    SYN_వస్తువులు(curwin->w_s)[idx].sp_type = SPTYPE_సరిపోలిక;
	    SYN_వస్తువులు(curwin->w_s)[idx].sp_syn.id = syn_id;
	    SYN_వస్తువులు(curwin->w_s)[idx].sp_syn.inc_ట్యాగ్ = current_syn_inc_ట్యాగ్;
	    SYN_వస్తువులు(curwin->w_s)[idx].sp_flags = syn_opt_arg.flags;
	    SYN_వస్తువులు(curwin->w_s)[idx].sp_sync_idx = sync_idx;
	    SYN_వస్తువులు(curwin->w_s)[idx].sp_cont_list = syn_opt_arg.cont_list;
	    SYN_వస్తువులు(curwin->w_s)[idx].sp_syn.cont_లోపల_list =
						     syn_opt_arg.cont_లోపల_list;
#నిర్వచితమైతే FEAT_CONCEAL
	    SYN_వస్తువులు(curwin->w_s)[idx].sp_cchar = conceal_char;
#అయితే_ముగింపు
	    ఒకవేళ (syn_opt_arg.cont_లోపల_list != ఖాళీ)
		curwin->w_s->b_syn_containedin = TRUE;
	    SYN_వస్తువులు(curwin->w_s)[idx].sp_తదుపరి_list = syn_opt_arg.తదుపరి_list;
	    ++curwin->w_s->b_syn_patterns.ga_len;

	    // remember that we found a match for syncing on
	    ఒకవేళ (syn_opt_arg.flags & (HL_SYNC_ఇక్కడ|HL_SYNC_THERE))
		curwin->w_s->b_syn_sync_flags |= SF_సరిపోలిక;
#నిర్వచితమైతే FEAT_FOLDING
	    ఒకవేళ (syn_opt_arg.flags & HL_FOLD)
		++curwin->w_s->b_syn_folditems;
#అయితే_ముగింపు

	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_దొంతర_free_all(curwin->w_s);	// Need to recompute all syntax.
	    తిరిగి;	// don't free the progs and patterns now
	}
    }

    /*
     * Something failed, free the allocated memory.
     */
    vim_regfree(అంశం.sp_prog);
    vim_free(అంశం.sp_నమూనా);
    vim_free(syn_opt_arg.cont_list);
    vim_free(syn_opt_arg.cont_లోపల_list);
    vim_free(syn_opt_arg.తదుపరి_list);

    ఒకవేళ (rest == ఖాళీ && called_emsg == orig_called_emsg)
	semsg(_(e_invalid_వాదం_str), arg);
}

/*
 * Handle ":syntax region {group-name} [matchgroup={group-name}]
 *		start {start} .. [skip {skip}] end {end} .. [{options}]".
 */
    స్థిర శూన్య
syn_cmd_region(
    exarg_T	*eap,
    పూర్ణాంకం		syncing)	    // TRUE for ":syntax sync region .."
{
    char_u		*arg = eap->arg;
    char_u		*సమూహం_name_చివర;
    char_u		*rest;			// next arg, NULL on error
    char_u		*కీలకం_చివర;
    char_u		*కీలకం = ఖాళీ;
    char_u		*p;
    పూర్ణాంకం			అంశం;
#నిర్వచించు అంశం_ప్రారంభం_2	    0
#నిర్వచించు అంశం_SKIP	    1
#నిర్వచించు అంశం_చివర_2	    2
#నిర్వచించు అంశం_MATCHGROUP	    3
    నిర్మాణం pat_ptr
    {
	synpat_T	*pp_synp;		// pointer to syn_pattern
	పూర్ణాంకం		pp_matchgroup_id;	// matchgroup ID
	నిర్మాణం pat_ptr	*pp_తదుపరి;		// pointer to next pat_ptr
    }			*(pat_ptrs[3]);
					// patterns found in the line
    నిర్మాణం pat_ptr	*ppp;
    నిర్మాణం pat_ptr	*ppp_తదుపరి;
    పూర్ణాంకం			pat_లెక్క = 0;		// nr of syn_patterns found
    పూర్ణాంకం			syn_id;
    పూర్ణాంకం			matchgroup_id = 0;
    పూర్ణాంకం			not_enough = FALSE;	// not enough arguments
    పూర్ణాంకం			illegal = FALSE;	// illegal arguments
    పూర్ణాంకం			success = FALSE;
    పూర్ణాంకం			idx;
    syn_opt_arg_T	syn_opt_arg;
    పూర్ణాంకం			conceal_char = NUL;

    // Isolate the group name, check for validity
    rest = get_సమూహం_name(arg, &సమూహం_name_చివర);

    pat_ptrs[0] = ఖాళీ;
    pat_ptrs[1] = ఖాళీ;
    pat_ptrs[2] = ఖాళీ;

    init_syn_patterns();

    syn_opt_arg.flags = 0;
    syn_opt_arg.keyword = FALSE;
    syn_opt_arg.sync_idx = ఖాళీ;
    syn_opt_arg.has_cont_list = TRUE;
    syn_opt_arg.cont_list = ఖాళీ;
    syn_opt_arg.cont_లోపల_list = ఖాళీ;
    syn_opt_arg.తదుపరి_list = ఖాళీ;

    /*
     * get the options, patterns and matchgroup.
     */
    అయితే (rest != ఖాళీ && !చివర్లు_excmd2(eap->cmd, rest))
    {
	// Check for option arguments
	rest = get_syn_options(rest, &syn_opt_arg, &conceal_char, eap->skip);
	ఒకవేళ (rest == ఖాళీ || చివర్లు_excmd2(eap->cmd, rest))
	    ఆపు;

	// must be a pattern or matchgroup then
	కీలకం_చివర = rest;
	అయితే (*కీలకం_చివర && !VIM_ISWHITE(*కీలకం_చివర) && *కీలకం_చివర != '=')
	    ++కీలకం_చివర;
	vim_free(కీలకం);
	కీలకం = vim_strnsave_పైకి(rest, కీలకం_చివర - rest);
	ఒకవేళ (కీలకం == ఖాళీ)			// out of memory
	{
	    rest = ఖాళీ;
	    ఆపు;
	}
	ఒకవేళ (STRCMP(కీలకం, "MATCHGROUP") == 0)
	    అంశం = అంశం_MATCHGROUP;
	లేకపోతే ఒకవేళ (STRCMP(కీలకం, "START") == 0)
	    అంశం = అంశం_ప్రారంభం_2;
	లేకపోతే ఒకవేళ (STRCMP(కీలకం, "END") == 0)
	    అంశం = అంశం_చివర_2;
	లేకపోతే ఒకవేళ (STRCMP(కీలకం, "SKIP") == 0)
	{
	    ఒకవేళ (pat_ptrs[అంశం_SKIP] != ఖాళీ)	// one skip pattern allowed
	    {
		illegal = TRUE;
		ఆపు;
	    }
	    అంశం = అంశం_SKIP;
	}
	లేకపోతే
	    ఆపు;
	rest = skipwhite(కీలకం_చివర);
	ఒకవేళ (*rest != '=')
	{
	    rest = ఖాళీ;
	    semsg(_(e_missing_equal_str), arg);
	    ఆపు;
	}
	rest = skipwhite(rest + 1);
	ఒకవేళ (*rest == NUL)
	{
	    not_enough = TRUE;
	    ఆపు;
	}

	ఒకవేళ (అంశం == అంశం_MATCHGROUP)
	{
	    p = skiptowhite(rest);
	    ఒకవేళ ((p - rest == 4 && STRNCMP(rest, "NONE", 4) == 0) || eap->skip)
		matchgroup_id = 0;
	    లేకపోతే
	    {
		matchgroup_id = syn_check_సమూహం(rest, (పూర్ణాంకం)(p - rest));
		ఒకవేళ (matchgroup_id == 0)
		{
		    illegal = TRUE;
		    ఆపు;
		}
	    }
	    rest = skipwhite(p);
	}
	లేకపోతే
	{
	    /*
	     * Allocate room for a syn_pattern, and link it in the list of
	     * syn_patterns for this item, at the start (because the list is
	     * used from end to start).
	     */
	    ppp = ALLOC_ONE(నిర్మాణం pat_ptr);
	    ఒకవేళ (ppp == ఖాళీ)
	    {
		rest = ఖాళీ;
		ఆపు;
	    }
	    ppp->pp_తదుపరి = pat_ptrs[అంశం];
	    pat_ptrs[అంశం] = ppp;
	    ppp->pp_synp = ALLOC_CLEAR_ONE(synpat_T);
	    ఒకవేళ (ppp->pp_synp == ఖాళీ)
	    {
		rest = ఖాళీ;
		ఆపు;
	    }

	    /*
	     * Get the syntax pattern and the following offset(s).
	     */
	    // Enable the appropriate \z specials.
	    ఒకవేళ (అంశం == అంశం_ప్రారంభం_2)
		reg_do_extmatch = REX_సమితి;
	    లేకపోతే ఒకవేళ (అంశం == అంశం_SKIP || అంశం == అంశం_చివర_2)
		reg_do_extmatch = REX_USE;
	    rest = get_syn_నమూనా(rest, ppp->pp_synp);
	    reg_do_extmatch = 0;
	    ఒకవేళ (అంశం == అంశం_చివర_2 && vim_regcomp_had_eol()
				       && !(syn_opt_arg.flags & HL_EXCLUDENL))
		ppp->pp_synp->sp_flags |= HL_HAS_EOL;
	    ppp->pp_matchgroup_id = matchgroup_id;
	    ++pat_లెక్క;
	}
    }
    vim_free(కీలకం);
    ఒకవేళ (illegal || not_enough)
	rest = ఖాళీ;

    /*
     * Must have a "start" and "end" pattern.
     */
    ఒకవేళ (rest != ఖాళీ && (pat_ptrs[అంశం_ప్రారంభం_2] == ఖాళీ ||
						  pat_ptrs[అంశం_చివర_2] == ఖాళీ))
    {
	not_enough = TRUE;
	rest = ఖాళీ;
    }

    ఒకవేళ (rest != ఖాళీ)
    {
	/*
	 * Check for trailing garbage or command.
	 * If OK, add the item.
	 */
	సమితి_nextcmd(eap, rest);
	ఒకవేళ (!చివర్లు_excmd(*rest) || eap->skip)
	    rest = ఖాళీ;
	లేకపోతే ఒకవేళ (ga_grow(&(curwin->w_s->b_syn_patterns), pat_లెక్క) == OK
		&& (syn_id = syn_check_సమూహం(arg,
					   (పూర్ణాంకం)(సమూహం_name_చివర - arg))) != 0)
	{
	    syn_incl_toplevel(syn_id, &syn_opt_arg.flags);
	    /*
	     * Store the start/skip/end in the syn_items list
	     */
	    idx = curwin->w_s->b_syn_patterns.ga_len;
	    కొరకు (అంశం = అంశం_ప్రారంభం_2; అంశం <= అంశం_చివర_2; ++అంశం)
	    {
		కొరకు (ppp = pat_ptrs[అంశం]; ppp != ఖాళీ; ppp = ppp->pp_తదుపరి)
		{
		    SYN_వస్తువులు(curwin->w_s)[idx] = *(ppp->pp_synp);
		    SYN_వస్తువులు(curwin->w_s)[idx].sp_syncing = syncing;
		    SYN_వస్తువులు(curwin->w_s)[idx].sp_type =
			    (అంశం == అంశం_ప్రారంభం_2) ? SPTYPE_ప్రారంభం :
			    (అంశం == అంశం_SKIP) ? SPTYPE_SKIP : SPTYPE_చివర;
		    SYN_వస్తువులు(curwin->w_s)[idx].sp_flags |= syn_opt_arg.flags;
		    SYN_వస్తువులు(curwin->w_s)[idx].sp_syn.id = syn_id;
		    SYN_వస్తువులు(curwin->w_s)[idx].sp_syn.inc_ట్యాగ్ =
							  current_syn_inc_ట్యాగ్;
		    SYN_వస్తువులు(curwin->w_s)[idx].sp_syn_సరిపోలిక_id =
							ppp->pp_matchgroup_id;
#నిర్వచితమైతే FEAT_CONCEAL
		    SYN_వస్తువులు(curwin->w_s)[idx].sp_cchar = conceal_char;
#అయితే_ముగింపు
		    ఒకవేళ (అంశం == అంశం_ప్రారంభం_2)
		    {
			SYN_వస్తువులు(curwin->w_s)[idx].sp_cont_list =
							syn_opt_arg.cont_list;
			SYN_వస్తువులు(curwin->w_s)[idx].sp_syn.cont_లోపల_list =
						     syn_opt_arg.cont_లోపల_list;
			ఒకవేళ (syn_opt_arg.cont_లోపల_list != ఖాళీ)
			    curwin->w_s->b_syn_containedin = TRUE;
			SYN_వస్తువులు(curwin->w_s)[idx].sp_తదుపరి_list =
							syn_opt_arg.తదుపరి_list;
		    }
		    ++curwin->w_s->b_syn_patterns.ga_len;
		    ++idx;
#నిర్వచితమైతే FEAT_FOLDING
		    ఒకవేళ (syn_opt_arg.flags & HL_FOLD)
			++curwin->w_s->b_syn_folditems;
#అయితే_ముగింపు
		}
	    }

	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_దొంతర_free_all(curwin->w_s);	// Need to recompute all syntax.
	    success = TRUE;	    // don't free the progs and patterns now
	}
    }

    /*
     * Free the allocated memory.
     */
    కొరకు (అంశం = అంశం_ప్రారంభం_2; అంశం <= అంశం_చివర_2; ++అంశం)
	కొరకు (ppp = pat_ptrs[అంశం]; ppp != ఖాళీ; ppp = ppp_తదుపరి)
	{
	    ఒకవేళ (!success && ppp->pp_synp != ఖాళీ)
	    {
		vim_regfree(ppp->pp_synp->sp_prog);
		vim_free(ppp->pp_synp->sp_నమూనా);
	    }
	    vim_free(ppp->pp_synp);
	    ppp_తదుపరి = ppp->pp_తదుపరి;
	    vim_free(ppp);
	}

    ఒకవేళ (!success)
    {
	vim_free(syn_opt_arg.cont_list);
	vim_free(syn_opt_arg.cont_లోపల_list);
	vim_free(syn_opt_arg.తదుపరి_list);
	ఒకవేళ (not_enough)
	    semsg(_(e_not_enough_arguments_syntax_region_str), arg);
	లేకపోతే ఒకవేళ (illegal || rest == ఖాళీ)
	    semsg(_(e_invalid_వాదం_str), arg);
    }
}

/*
 * A simple syntax group ID comparison function suitable for use in qsort()
 */
    స్థిర పూర్ణాంకం
syn_compare_stub(స్థిరరాశి శూన్య *v1, స్థిరరాశి శూన్య *v2)
{
    స్థిరరాశి పొట్టి	*s1 = v1;
    స్థిరరాశి పొట్టి	*s2 = v2;

    తిరిగి (*s1 > *s2 ? 1 : *s1 < *s2 ? -1 : 0);
}

/*
 * Combines lists of syntax clusters.
 * *clstr1 and *clstr2 must both be allocated memory; they will be consumed.
 */
    స్థిర శూన్య
syn_combine_list(పొట్టి **clstr1, పొట్టి **clstr2, పూర్ణాంకం list_op)
{
    పూర్ణాంకం		count1 = 0;
    పూర్ణాంకం		count2 = 0;
    పొట్టి	*g1;
    పొట్టి	*g2;
    పొట్టి	*clstr = ఖాళీ;
    పూర్ణాంకం		లెక్క;
    పూర్ణాంకం		విడత;

    /*
     * Handle degenerate cases.
     */
    ఒకవేళ (*clstr2 == ఖాళీ)
	తిరిగి;
    ఒకవేళ (*clstr1 == ఖాళీ || list_op == CLUSTER_REPLACE)
    {
	ఒకవేళ (list_op == CLUSTER_REPLACE)
	    vim_free(*clstr1);
	ఒకవేళ (list_op == CLUSTER_REPLACE || list_op == CLUSTER_కూడు)
	    *clstr1 = *clstr2;
	లేకపోతే
	    vim_free(*clstr2);
	తిరిగి;
    }

    కొరకు (g1 = *clstr1; *g1; g1++)
	++count1;
    కొరకు (g2 = *clstr2; *g2; g2++)
	++count2;

    /*
     * For speed purposes, sort both lists.
     */
    qsort(*clstr1, (size_t)count1, పరిమాణం(పొట్టి), syn_compare_stub);
    qsort(*clstr2, (size_t)count2, పరిమాణం(పొట్టి), syn_compare_stub);

    /*
     * We proceed in two passes; in round 1, we count the elements to place
     * in the new list, and in round 2, we allocate and populate the new
     * list.  For speed, we use a mergesort-like method, adding the smaller
     * of the current elements in each list to the new list.
     */
    కొరకు (విడత = 1; విడత <= 2; విడత++)
    {
	g1 = *clstr1;
	g2 = *clstr2;
	లెక్క = 0;

	/*
	 * First, loop through the lists until one of them is empty.
	 */
	అయితే (*g1 && *g2)
	{
	    /*
	     * We always want to add from the first list.
	     */
	    ఒకవేళ (*g1 < *g2)
	    {
		ఒకవేళ (విడత == 2)
		    clstr[లెక్క] = *g1;
		లెక్క++;
		g1++;
		కొనసాగు;
	    }
	    /*
	     * We only want to add from the second list if we're adding the
	     * lists.
	     */
	    ఒకవేళ (list_op == CLUSTER_కూడు)
	    {
		ఒకవేళ (విడత == 2)
		    clstr[లెక్క] = *g2;
		లెక్క++;
	    }
	    ఒకవేళ (*g1 == *g2)
		g1++;
	    g2++;
	}

	/*
	 * Now add the leftovers from whichever list didn't get finished
	 * first.  As before, we only want to add from the second list if
	 * we're adding the lists.
	 */
	కొరకు (; *g1; g1++, లెక్క++)
	    ఒకవేళ (విడత == 2)
		clstr[లెక్క] = *g1;
	ఒకవేళ (list_op == CLUSTER_కూడు)
	    కొరకు (; *g2; g2++, లెక్క++)
		ఒకవేళ (విడత == 2)
		    clstr[లెక్క] = *g2;

	ఒకవేళ (విడత == 1)
	{
	    /*
	     * If the group ended up empty, we don't need to allocate any
	     * space for it.
	     */
	    ఒకవేళ (లెక్క == 0)
	    {
		clstr = ఖాళీ;
		ఆపు;
	    }
	    clstr = ALLOC_MULT(పొట్టి, లెక్క + 1);
	    ఒకవేళ (clstr == ఖాళీ)
		ఆపు;
	    clstr[లెక్క] = 0;
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
    స్థిర పూర్ణాంకం
syn_scl_name2id(char_u *name)
{
    పూర్ణాంకం		i;
    char_u	*name_u;

    // Avoid using stricmp() too much, it's slow on some systems
    name_u = vim_strsave_పైకి(name);
    ఒకవేళ (name_u == ఖాళీ)
	తిరిగి 0;
    కొరకు (i = curwin->w_s->b_syn_clusters.ga_len; --i >= 0; )
	ఒకవేళ (SYN_CLSTR(curwin->w_s)[i].scl_name_u != ఖాళీ
		&& STRCMP(name_u, SYN_CLSTR(curwin->w_s)[i].scl_name_u) == 0)
	    ఆపు;
    vim_free(name_u);
    తిరిగి (i < 0 ? 0 : i + SYNID_CLUSTER);
}

/*
 * Like syn_scl_name2id(), but take a pointer + length argument.
 */
    స్థిర పూర్ణాంకం
syn_scl_namen2id(char_u *linep, పూర్ణాంకం len)
{
    char_u  *name;
    పూర్ణాంకం	    id = 0;

    name = vim_strnsave(linep, len);
    ఒకవేళ (name == ఖాళీ)
	తిరిగి 0;

    id = syn_scl_name2id(name);
    vim_free(name);
    తిరిగి id;
}

/*
 * Find syntax cluster name in the table and return its ID.
 * The argument is a pointer to the name and the length of the name.
 * If it doesn't exist yet, a new entry is created.
 * Return 0 for failure.
 */
    స్థిర పూర్ణాంకం
syn_check_cluster(char_u *pp, పూర్ణాంకం len)
{
    పూర్ణాంకం		id;
    char_u	*name;

    name = vim_strnsave(pp, len);
    ఒకవేళ (name == ఖాళీ)
	తిరిగి 0;

    id = syn_scl_name2id(name);
    ఒకవేళ (id == 0)			// doesn't exist yet
	id = syn_కూడు_cluster(name);
    లేకపోతే
	vim_free(name);
    తిరిగి id;
}

/*
 * Add new syntax cluster and return its ID.
 * "name" must be an allocated string, it will be consumed.
 * Return 0 for failure.
 */
    స్థిర పూర్ణాంకం
syn_కూడు_cluster(char_u *name)
{
    పూర్ణాంకం		len;

    /*
     * First call for this growarray: init growing array.
     */
    ఒకవేళ (curwin->w_s->b_syn_clusters.ga_దత్తాంశం == ఖాళీ)
    {
	curwin->w_s->b_syn_clusters.ga_itemsize = పరిమాణం(syn_cluster_T);
	curwin->w_s->b_syn_clusters.ga_growsize = 10;
    }

    len = curwin->w_s->b_syn_clusters.ga_len;
    ఒకవేళ (len >= MAX_CLUSTER_ID)
    {
	emsg(_(e_too_many_syntax_clusters));
	vim_free(name);
	తిరిగి 0;
    }

    /*
     * Make room for at least one other cluster entry.
     */
    ఒకవేళ (ga_grow(&curwin->w_s->b_syn_clusters, 1) == FAIL)
    {
	vim_free(name);
	తిరిగి 0;
    }

    CLEAR_సూచిక(&(SYN_CLSTR(curwin->w_s)[len]));
    SYN_CLSTR(curwin->w_s)[len].scl_name = name;
    SYN_CLSTR(curwin->w_s)[len].scl_name_u = vim_strsave_పైకి(name);
    SYN_CLSTR(curwin->w_s)[len].scl_list = ఖాళీ;
    ++curwin->w_s->b_syn_clusters.ga_len;

    ఒకవేళ (STRICMP(name, "Spell") == 0)
	curwin->w_s->b_spell_cluster_id = len + SYNID_CLUSTER;
    ఒకవేళ (STRICMP(name, "NoSpell") == 0)
	curwin->w_s->b_nospell_cluster_id = len + SYNID_CLUSTER;

    తిరిగి len + SYNID_CLUSTER;
}

/*
 * Handle ":syntax cluster {cluster-name} [contains={groupname},..]
 *		[add={groupname},..] [remove={groupname},..]".
 */
    స్థిర శూన్య
syn_cmd_cluster(exarg_T *eap, పూర్ణాంకం syncing వాడని)
{
    char_u	*arg = eap->arg;
    char_u	*సమూహం_name_చివర;
    char_u	*rest;
    పూర్ణాంకం		scl_id;
    పొట్టి	*clstr_list;
    పూర్ణాంకం		got_clstr = FALSE;
    పూర్ణాంకం		opt_len;
    పూర్ణాంకం		list_op;

    eap->nextcmd = కనుగొను_nextcmd(arg);
    ఒకవేళ (eap->skip)
	తిరిగి;

    rest = get_సమూహం_name(arg, &సమూహం_name_చివర);

    ఒకవేళ (rest != ఖాళీ)
    {
	scl_id = syn_check_cluster(arg, (పూర్ణాంకం)(సమూహం_name_చివర - arg));
	ఒకవేళ (scl_id == 0)
	    తిరిగి;
	scl_id -= SYNID_CLUSTER;

	కొరకు (;;)
	{
	    ఒకవేళ (STRNICMP(rest, "add", 3) == 0
		    && (VIM_ISWHITE(rest[3]) || rest[3] == '='))
	    {
		opt_len = 3;
		list_op = CLUSTER_కూడు;
	    }
	    లేకపోతే ఒకవేళ (STRNICMP(rest, "remove", 6) == 0
		    && (VIM_ISWHITE(rest[6]) || rest[6] == '='))
	    {
		opt_len = 6;
		list_op = CLUSTER_వ్యవకలనం;
	    }
	    లేకపోతే ఒకవేళ (STRNICMP(rest, "contains", 8) == 0
			&& (VIM_ISWHITE(rest[8]) || rest[8] == '='))
	    {
		opt_len = 8;
		list_op = CLUSTER_REPLACE;
	    }
	    లేకపోతే
		ఆపు;

	    clstr_list = ఖాళీ;
	    ఒకవేళ (get_id_list(&rest, opt_len, &clstr_list, eap->skip) == FAIL)
	    {
		semsg(_(e_invalid_వాదం_str), rest);
		ఆపు;
	    }
	    ఒకవేళ (scl_id >= 0)
		syn_combine_list(&SYN_CLSTR(curwin->w_s)[scl_id].scl_list,
			     &clstr_list, list_op);
	    లేకపోతే
		vim_free(clstr_list);
	    got_clstr = TRUE;
	}

	ఒకవేళ (got_clstr)
	{
	    redraw_curbuf_later(UPD_SOME_VALID);
	    syn_దొంతర_free_all(curwin->w_s);	// Need to recompute all.
	}
    }

    ఒకవేళ (!got_clstr)
	emsg(_(e_no_cluster_specified));
    ఒకవేళ (rest == ఖాళీ || !చివర్లు_excmd2(eap->cmd, rest))
	semsg(_(e_invalid_వాదం_str), arg);
}

/*
 * On first call for current buffer: Init growing array.
 */
    స్థిర శూన్య
init_syn_patterns(శూన్య)
{
    curwin->w_s->b_syn_patterns.ga_itemsize = పరిమాణం(synpat_T);
    curwin->w_s->b_syn_patterns.ga_growsize = 10;
}

/*
 * Get one pattern for a ":syntax match" or ":syntax region" command.
 * Stores the pattern and program in a synpat_T.
 * Returns a pointer to the next argument, or NULL in case of an error.
 */
    స్థిర char_u *
get_syn_నమూనా(char_u *arg, synpat_T *ci)
{
    char_u	*చివర;
    పూర్ణాంకం		*p;
    పూర్ణాంకం		idx;
    char_u	*cpo_save;

    // need at least three chars
    ఒకవేళ (arg == ఖాళీ || arg[0] == NUL || arg[1] == NUL || arg[2] == NUL)
	తిరిగి ఖాళీ;

    చివర = skip_regexp(arg + 1, *arg, TRUE);
    ఒకవేళ (*చివర != *arg)			    // end delimiter not found
    {
	semsg(_(e_నమూనా_delimiter_not_దొరికింది_str), arg);
	తిరిగి ఖాళీ;
    }
    // store the pattern and compiled regexp program
    ఒకవేళ ((ci->sp_నమూనా = vim_strnsave(arg + 1, చివర - arg - 1)) == ఖాళీ)
	తిరిగి ఖాళీ;

    // Make 'cpoptions' empty, to avoid the 'l' flag
    cpo_save = p_cpo;
    p_cpo = ఖాళీలు_option;
    ci->sp_prog = vim_regcomp(ci->sp_నమూనా, RE_MAGIC);
    p_cpo = cpo_save;

    ఒకవేళ (ci->sp_prog == ఖాళీ)
	తిరిగి ఖాళీ;
    ci->sp_ic = curwin->w_s->b_syn_ic;
#నిర్వచితమైతే FEAT_PROFILE
    syn_clear_సమయం(&ci->sp_సమయం);
#అయితే_ముగింపు

    /*
     * Check for a match, highlight or region offset.
     */
    ++చివర;
    చేయి
    {
	కొరకు (idx = SPO_లెక్క; --idx >= 0; )
	    ఒకవేళ (STRNCMP(చివర, spo_name_tab[idx], 3) == 0)
		ఆపు;
	ఒకవేళ (idx >= 0)
	{
	    p = &(ci->sp_offsets[idx]);
	    ఒకవేళ (idx != SPO_LC_OFF)
		ఎంపిక (చివర[3])
		{
		    స్థితి 's':   ఆపు;
		    స్థితి 'b':   ఆపు;
		    స్థితి 'e':   idx += SPO_లెక్క; ఆపు;
		    అప్రమేయ:    idx = -1; ఆపు;
		}
	    ఒకవేళ (idx >= 0)
	    {
		ci->sp_off_flags |= (1 << idx);
		ఒకవేళ (idx == SPO_LC_OFF)	    // lc=99
		{
		    చివర += 3;
		    *p = getdigits(&చివర);

		    // "lc=" offset automatically sets "ms=" offset
		    ఒకవేళ (!(ci->sp_off_flags & (1 << SPO_MS_OFF)))
		    {
			ci->sp_off_flags |= (1 << SPO_MS_OFF);
			ci->sp_offsets[SPO_MS_OFF] = *p;
		    }
		}
		లేకపోతే			    // yy=x+99
		{
		    చివర += 4;
		    ఒకవేళ (*చివర == '+')
		    {
			++చివర;
			*p = getdigits(&చివర);		// positive offset
		    }
		    లేకపోతే ఒకవేళ (*చివర == '-')
		    {
			++చివర;
			*p = -getdigits(&చివర);		// negative offset
		    }
		}
		ఒకవేళ (*చివర != ',')
		    ఆపు;
		++చివర;
	    }
	}
    } అయితే (idx >= 0);

    ఒకవేళ (!చివర్లు_excmd2(arg, చివర) && !VIM_ISWHITE(*చివర))
    {
	semsg(_(e_garbage_after_నమూనా_str), arg);
	తిరిగి ఖాళీ;
    }
    తిరిగి skipwhite(చివర);
}

/*
 * Handle ":syntax sync .." command.
 */
    స్థిర శూన్య
syn_cmd_sync(exarg_T *eap, పూర్ణాంకం syncing వాడని)
{
    char_u	*arg_ప్రారంభం = eap->arg;
    char_u	*arg_చివర;
    char_u	*కీలకం = ఖాళీ;
    char_u	*తదుపరి_arg;
    పూర్ణాంకం		illegal = FALSE;
    పూర్ణాంకం		ముగిసిన = FALSE;
    దీర్ఘ	n;
    char_u	*cpo_save;

    ఒకవేళ (చివర్లు_excmd2(eap->cmd, arg_ప్రారంభం))
    {
	syn_cmd_list(eap, TRUE);
	తిరిగి;
    }

    అయితే (!చివర్లు_excmd2(eap->cmd, arg_ప్రారంభం))
    {
	arg_చివర = skiptowhite(arg_ప్రారంభం);
	తదుపరి_arg = skipwhite(arg_చివర);
	vim_free(కీలకం);
	కీలకం = vim_strnsave_పైకి(arg_ప్రారంభం, arg_చివర - arg_ప్రారంభం);
	ఒకవేళ (కీలకం == ఖాళీ)
	    ఆపు;
	ఒకవేళ (STRCMP(కీలకం, "CCOMMENT") == 0)
	{
	    ఒకవేళ (!eap->skip)
		curwin->w_s->b_syn_sync_flags |= SF_CCOMMENT;
	    ఒకవేళ (!చివర్లు_excmd2(eap->cmd, తదుపరి_arg))
	    {
		arg_చివర = skiptowhite(తదుపరి_arg);
		ఒకవేళ (!eap->skip)
		    curwin->w_s->b_syn_sync_id = syn_check_సమూహం(తదుపరి_arg,
						   (పూర్ణాంకం)(arg_చివర - తదుపరి_arg));
		తదుపరి_arg = skipwhite(arg_చివర);
	    }
	    లేకపోతే ఒకవేళ (!eap->skip)
		curwin->w_s->b_syn_sync_id = syn_name2id((char_u *)"Comment");
	}
	లేకపోతే ఒకవేళ (  STRNCMP(కీలకం, "LINES", 5) == 0
		|| STRNCMP(కీలకం, "MINLINES", 8) == 0
		|| STRNCMP(కీలకం, "MAXLINES", 8) == 0
		|| STRNCMP(కీలకం, "LINEBREAKS", 10) == 0)
	{
	    ఒకవేళ (కీలకం[4] == 'S')
		arg_చివర = కీలకం + 6;
	    లేకపోతే ఒకవేళ (కీలకం[0] == 'L')
		arg_చివర = కీలకం + 11;
	    లేకపోతే
		arg_చివర = కీలకం + 9;
	    ఒకవేళ (arg_చివర[-1] != '=' || !VIM_ISDIGIT(*arg_చివర))
	    {
		illegal = TRUE;
		ఆపు;
	    }
	    n = getdigits(&arg_చివర);
	    ఒకవేళ (!eap->skip)
	    {
		ఒకవేళ (కీలకం[4] == 'B')
		    curwin->w_s->b_syn_sync_linebreaks = n;
		లేకపోతే ఒకవేళ (కీలకం[1] == 'A')
		    curwin->w_s->b_syn_sync_maxlines = n;
		లేకపోతే
		    curwin->w_s->b_syn_sync_minlines = n;
	    }
	}
	లేకపోతే ఒకవేళ (STRCMP(కీలకం, "FROMSTART") == 0)
	{
	    ఒకవేళ (!eap->skip)
	    {
		curwin->w_s->b_syn_sync_minlines = MAXLNUM;
		curwin->w_s->b_syn_sync_maxlines = 0;
	    }
	}
	లేకపోతే ఒకవేళ (STRCMP(కీలకం, "LINECONT") == 0)
	{
	    ఒకవేళ (*తదుపరి_arg == NUL)	   // missing pattern
	    {
		illegal = TRUE;
		ఆపు;
	    }
	    ఒకవేళ (curwin->w_s->b_syn_linecont_pat != ఖాళీ)
	    {
		emsg(_(e_syntax_sync_పంక్తి_continuations_నమూనా_specified_twice));
		ముగిసిన = TRUE;
		ఆపు;
	    }
	    arg_చివర = skip_regexp(తదుపరి_arg + 1, *తదుపరి_arg, TRUE);
	    ఒకవేళ (*arg_చివర != *తదుపరి_arg)	    // end delimiter not found
	    {
		illegal = TRUE;
		ఆపు;
	    }

	    ఒకవేళ (!eap->skip)
	    {
		// store the pattern and compiled regexp program
		ఒకవేళ ((curwin->w_s->b_syn_linecont_pat =
			    vim_strnsave(తదుపరి_arg + 1,
				      arg_చివర - తదుపరి_arg - 1)) == ఖాళీ)
		{
		    ముగిసిన = TRUE;
		    ఆపు;
		}
		curwin->w_s->b_syn_linecont_ic = curwin->w_s->b_syn_ic;

		// Make 'cpoptions' empty, to avoid the 'l' flag
		cpo_save = p_cpo;
		p_cpo = ఖాళీలు_option;
		curwin->w_s->b_syn_linecont_prog =
		       vim_regcomp(curwin->w_s->b_syn_linecont_pat, RE_MAGIC);
		p_cpo = cpo_save;
#నిర్వచితమైతే FEAT_PROFILE
		syn_clear_సమయం(&curwin->w_s->b_syn_linecont_సమయం);
#అయితే_ముగింపు

		ఒకవేళ (curwin->w_s->b_syn_linecont_prog == ఖాళీ)
		{
		    VIM_CLEAR(curwin->w_s->b_syn_linecont_pat);
		    ముగిసిన = TRUE;
		    ఆపు;
		}
	    }
	    తదుపరి_arg = skipwhite(arg_చివర + 1);
	}
	లేకపోతే
	{
	    eap->arg = తదుపరి_arg;
	    ఒకవేళ (STRCMP(కీలకం, "MATCH") == 0)
		syn_cmd_సరిపోలిక(eap, TRUE);
	    లేకపోతే ఒకవేళ (STRCMP(కీలకం, "REGION") == 0)
		syn_cmd_region(eap, TRUE);
	    లేకపోతే ఒకవేళ (STRCMP(కీలకం, "CLEAR") == 0)
		syn_cmd_clear(eap, TRUE);
	    లేకపోతే
		illegal = TRUE;
	    ముగిసిన = TRUE;
	    ఆపు;
	}
	arg_ప్రారంభం = తదుపరి_arg;
    }
    vim_free(కీలకం);
    ఒకవేళ (illegal)
	semsg(_(e_illegal_arguments_str), arg_ప్రారంభం);
    లేకపోతే ఒకవేళ (!ముగిసిన)
    {
	సమితి_nextcmd(eap, arg_ప్రారంభం);
	redraw_curbuf_later(UPD_SOME_VALID);
	syn_దొంతర_free_all(curwin->w_s);	// Need to recompute all syntax.
    }
}

/*
 * Convert a line of highlight group names into a list of group ID numbers.
 * "arg" should point to the "contains" or "nextgroup" keyword.
 * "arg" is advanced to after the last group name.
 * Careful: the argument is modified (NULs added).
 * returns FAIL for some error, OK for success.
 */
    స్థిర పూర్ణాంకం
get_id_list(
    char_u	**arg,
    పూర్ణాంకం		keylen,		// length of keyword
    పొట్టి	**list,		// where to store the resulting list, if not
				// NULL, the list is silently skipped!
    పూర్ణాంకం		skip)
{
    char_u	*p = ఖాళీ;
    char_u	*చివర;
    పూర్ణాంకం		విడత;
    పూర్ణాంకం		లెక్క;
    పూర్ణాంకం		మొత్తం_లెక్క = 0;
    పొట్టి	*retval = ఖాళీ;
    char_u	*name;
    regmatch_T	regmatch;
    పూర్ణాంకం		id;
    పూర్ణాంకం		i;
    పూర్ణాంకం		failed = FALSE;

    /*
     * We parse the list twice:
     * round == 1: count the number of items, allocate the array.
     * round == 2: fill the array with the items.
     * In round 1 new groups may be added, causing the number of items to
     * grow when a regexp is used.  In that case round 1 is done once again.
     */
    కొరకు (విడత = 1; విడత <= 2; ++విడత)
    {
	/*
	 * skip "contains"
	 */
	p = skipwhite(*arg + keylen);
	ఒకవేళ (*p != '=')
	{
	    semsg(_(e_missing_equal_sign_str), *arg);
	    ఆపు;
	}
	p = skipwhite(p + 1);
	ఒకవేళ (చివర్లు_excmd2(*arg, p))
	{
	    semsg(_(e_ఖాళీలు_వాదం_str), *arg);
	    ఆపు;
	}

	/*
	 * parse the arguments after "contains"
	 */
	లెక్క = 0;
	అయితే (!చివర్లు_excmd2(*arg, p))
	{
	    కొరకు (చివర = p; *చివర && !VIM_ISWHITE(*చివర) && *చివర != ','; ++చివర)
		;
	    name = alloc(చివర - p + 3);	    // leave room for "^$"
	    ఒకవేళ (name == ఖాళీ)
	    {
		failed = TRUE;
		ఆపు;
	    }
	    vim_strncpy(name + 1, p, చివర - p);
	    ఒకవేళ (       STRCMP(name + 1, "ALLBUT") == 0
		    || STRCMP(name + 1, "ALL") == 0
		    || STRCMP(name + 1, "TOP") == 0
		    || STRCMP(name + 1, "CONTAINED") == 0)
	    {
		ఒకవేళ (TOUPPER_ASC(**arg) != 'C')
		{
		    semsg(_(e_str_not_allowed_ఇక్కడ), name + 1);
		    failed = TRUE;
		    vim_free(name);
		    ఆపు;
		}
		ఒకవేళ (లెక్క != 0)
		{
		    semsg(_(e_str_must_be_మొదటి_లోపల_కలిగియుంది_list), name + 1);
		    failed = TRUE;
		    vim_free(name);
		    ఆపు;
		}
		ఒకవేళ (name[1] == 'A')
		    id = SYNID_ALLBUT;
		లేకపోతే ఒకవేళ (name[1] == 'T')
		    id = SYNID_శిఖరం;
		లేకపోతే
		    id = SYNID_CONTAINED;
		id += current_syn_inc_ట్యాగ్;
	    }
	    లేకపోతే ఒకవేళ (name[1] == '@')
	    {
		ఒకవేళ (skip)
		    id = -1;
		లేకపోతే
		    id = syn_check_cluster(name + 2, (పూర్ణాంకం)(చివర - p - 1));
	    }
	    లేకపోతే
	    {
		/*
		 * Handle full group name.
		 */
		ఒకవేళ (vim_strpbrk(name + 1, (char_u *)"\\.*^$~[") == ఖాళీ)
		    id = syn_check_సమూహం(name + 1, (పూర్ణాంకం)(చివర - p));
		లేకపోతే
		{
		    /*
		     * Handle match of regexp with group names.
		     */
		    *name = '^';
		    STRCAT(name, "$");
		    regmatch.regprog = vim_regcomp(name, RE_MAGIC);
		    ఒకవేళ (regmatch.regprog == ఖాళీ)
		    {
			failed = TRUE;
			vim_free(name);
			ఆపు;
		    }

		    regmatch.rm_ic = TRUE;
		    id = 0;
		    కొరకు (i = highlight_అంకె_groups(); --i >= 0; )
		    {
			ఒకవేళ (vim_regexec(&regmatch, highlight_సమూహం_name(i),
								  (colnr_T)0))
			{
			    ఒకవేళ (విడత == 2)
			    {
				// Got more items than expected; can happen
				// when adding items that match:
				// "contains=a.*b,axb".
				// Go back to first round
				ఒకవేళ (లెక్క >= మొత్తం_లెక్క)
				{
				    vim_free(retval);
				    విడత = 1;
				}
				లేకపోతే
				    retval[లెక్క] = i + 1;
			    }
			    ++లెక్క;
			    id = -1;	    // remember that we found one
			}
		    }
		    vim_regfree(regmatch.regprog);
		}
	    }
	    vim_free(name);
	    ఒకవేళ (id == 0)
	    {
		semsg(_(e_unknown_సమూహం_name_str), p);
		failed = TRUE;
		ఆపు;
	    }
	    ఒకవేళ (id > 0)
	    {
		ఒకవేళ (విడత == 2)
		{
		    // Got more items than expected, go back to first round
		    ఒకవేళ (లెక్క >= మొత్తం_లెక్క)
		    {
			vim_free(retval);
			విడత = 1;
		    }
		    లేకపోతే
			retval[లెక్క] = id;
		}
		++లెక్క;
	    }
	    p = skipwhite(చివర);
	    ఒకవేళ (*p != ',')
		ఆపు;
	    p = skipwhite(p + 1);	// skip comma in between arguments
	}
	ఒకవేళ (failed)
	    ఆపు;
	ఒకవేళ (విడత == 1)
	{
	    retval = ALLOC_MULT(పొట్టి, లెక్క + 1);
	    ఒకవేళ (retval == ఖాళీ)
		ఆపు;
	    retval[లెక్క] = 0;	    // zero means end of the list
	    మొత్తం_లెక్క = లెక్క;
	}
    }

    *arg = p;
    ఒకవేళ (failed || retval == ఖాళీ)
    {
	vim_free(retval);
	తిరిగి FAIL;
    }

    ఒకవేళ (*list == ఖాళీ)
	*list = retval;
    లేకపోతే
	vim_free(retval);	// list already found, don't overwrite it

    తిరిగి OK;
}

/*
 * Make a copy of an ID list.
 */
    స్థిర పొట్టి *
copy_id_list(పొట్టి *list)
{
    పూర్ణాంకం	    len;
    పూర్ణాంకం	    లెక్క;
    పొట్టి   *retval;

    ఒకవేళ (list == ఖాళీ)
	తిరిగి ఖాళీ;

    కొరకు (లెక్క = 0; list[లెక్క]; ++లెక్క)
	;
    len = (లెక్క + 1) * పరిమాణం(పొట్టి);
    retval = alloc(len);
    ఒకవేళ (retval != ఖాళీ)
	mch_memmove(retval, list, (size_t)len);

    తిరిగి retval;
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
#నిర్వచించు IDL_CACHE_కొలత 16

రకనిర్వచనం నిర్మాణం
{
    పొట్టి	*idl_కీలకం;	// the list this entry resolves (NULL when empty)
    పొట్టి	*idl_ids;	// sorted, cluster-expanded group IDs (allocated)
    పూర్ణాంకం		idl_లెక్క;	// number of IDs in idl_ids
    పొట్టి	idl_marker;	// leading ALLBUT/TOP/CONTAINED item, or 0
    bool	idl_usable;	// false: not cacheable, use the slow path
} idl_నమోదు_T;

రకనిర్వచనం నిర్మాణం
{
    idl_నమోదు_T	ent[IDL_CACHE_కొలత];
    పూర్ణాంకం		విడత;		// round-robin replacement index
    పూర్ణాంకం		చివరి;		// index of the last returned entry (fast path)
} idl_cache_T;

/*
 * Collect the explicit group IDs of "list" into "ga", expanding clusters.
 * "seen" records visited clusters to break cycles and avoid duplicate work.
 * Return false when the list cannot be represented as a plain set (a nested
 * ALLBUT/TOP/CONTAINED marker) or on out of memory.
 */
    స్థిర bool
idl_collect(పొట్టి *list, garray_T *ga, garray_T *చూసిన)
{
    ఒకవేళ (list == ఖాళీ || list == ID_LIST_ALL)
	తిరిగి false;
    కొరకు (పొట్టి అంశం = *list; అంశం != 0; అంశం = *++list)
    {
	ఒకవేళ (అంశం >= SYNID_ALLBUT && అంశం < SYNID_CLUSTER)
	    తిరిగి false;	// nested ALLBUT/TOP/CONTAINED: not cacheable
	ఒకవేళ (అంశం >= SYNID_CLUSTER)
	{
	    పొట్టి	*scl_list;
	    bool	చూసిన_it = false;

	    కొరకు (పూర్ణాంకం i = 0; i < చూసిన->ga_len; ++i)
		ఒకవేళ (((పొట్టి *)చూసిన->ga_దత్తాంశం)[i] == అంశం)
		{
		    చూసిన_it = true;
		    ఆపు;
		}
	    ఒకవేళ (చూసిన_it)
		కొనసాగు;
	    ఒకవేళ (ga_grow(చూసిన, 1) == FAIL)
		తిరిగి false;
	    ((పొట్టి *)చూసిన->ga_దత్తాంశం)[చూసిన->ga_len++] = అంశం;
	    // Record the cluster id itself, not just its members: the slow
	    // path matches "item == id" for a cluster id present in the list.
	    ఒకవేళ (ga_grow(ga, 1) == FAIL)
		తిరిగి false;
	    ((పొట్టి *)ga->ga_దత్తాంశం)[ga->ga_len++] = అంశం;
	    scl_list = SYN_CLSTR(syn_ఖండం)[అంశం - SYNID_CLUSTER].scl_list;
	    ఒకవేళ (scl_list != ఖాళీ && !idl_collect(scl_list, ga, చూసిన))
		తిరిగి false;
	}
	లేకపోతే
	{
	    ఒకవేళ (ga_grow(ga, 1) == FAIL)
		తిరిగి false;
	    ((పొట్టి *)ga->ga_దత్తాంశం)[ga->ga_len++] = అంశం;
	}
    }
    తిరిగి true;
}

    స్థిర పూర్ణాంకం
idl_id_cmp(స్థిరరాశి శూన్య *a, స్థిరరాశి శూన్య *b)
{
    తిరిగి (పూర్ణాంకం)*(స్థిరరాశి పొట్టి *)a - (పూర్ణాంకం)*(స్థిరరాశి పొట్టి *)b;
}

/*
 * Get the cache entry that resolves "list", building it if needed.
 * Returns NULL only on allocation failure.
 */
    స్థిర idl_నమోదు_T *
idl_get_నమోదు(పొట్టి *list)
{
    idl_cache_T	*cache = (idl_cache_T *)syn_ఖండం->b_idlist_cache;
    idl_నమోదు_T	*e;
    garray_T	ga, చూసిన;

    ఒకవేళ (cache == ఖాళీ)
    {
	cache = ALLOC_CLEAR_ONE(idl_cache_T);
	ఒకవేళ (cache == ఖాళీ)
	    తిరిగి ఖాళీ;
	syn_ఖండం->b_idlist_cache = cache;
    }
    // Fast path: the same list is usually queried many times in a row.
    ఒకవేళ (cache->ent[cache->చివరి].idl_కీలకం == list)
	తిరిగి &cache->ent[cache->చివరి];
    కొరకు (పూర్ణాంకం i = 0; i < IDL_CACHE_కొలత; ++i)
	ఒకవేళ (cache->ent[i].idl_కీలకం == list)
	{
	    cache->చివరి = i;
	    తిరిగి &cache->ent[i];
	}

    // Build a new entry, replacing one in round-robin order.
    cache->చివరి = cache->విడత;
    e = &cache->ent[cache->విడత];
    cache->విడత = (cache->విడత + 1) % IDL_CACHE_కొలత;
    VIM_CLEAR(e->idl_ids);
    e->idl_లెక్క = 0;
    e->idl_కీలకం = list;
    e->idl_marker = 0;
    e->idl_usable = true;

    // A leading ALLBUT/TOP/CONTAINED marker is kept aside; the rest is the set.
    ఒకవేళ (*list >= SYNID_ALLBUT && *list < SYNID_CLUSTER)
    {
	e->idl_marker = *list;
	++list;
    }

    ga_init2(&ga, పరిమాణం(పొట్టి), 50);
    ga_init2(&చూసిన, పరిమాణం(పొట్టి), 10);
    ఒకవేళ (!idl_collect(list, &ga, &చూసిన))
    {
	e->idl_usable = false;
	ga_clear(&ga);
	ga_clear(&చూసిన);
	తిరిగి e;
    }
    ga_clear(&చూసిన);
    ఒకవేళ (ga.ga_len > 0)
    {
	qsort(ga.ga_దత్తాంశం, (size_t)ga.ga_len, పరిమాణం(పొట్టి), idl_id_cmp);
	e->idl_ids = (పొట్టి *)ga.ga_దత్తాంశం;	// take ownership
	e->idl_లెక్క = ga.ga_len;
    }
    లేకపోతే
	ga_clear(&ga);
    తిరిగి e;
}

/*
 * Return true if "id" is in the resolved set of cache entry "e".
 */
    స్థిర bool
idl_కలిగియుంది(idl_నమోదు_T *e, పొట్టి id)
{
    పూర్ణాంకం	lo = 0;
    పూర్ణాంకం	hi = e->idl_లెక్క - 1;

    అయితే (lo <= hi)
    {
	పూర్ణాంకం	mid = (lo + hi) / 2;
	పొట్టి	v = e->idl_ids[mid];

	ఒకవేళ (v == id)
	    తిరిగి true;
	ఒకవేళ (v < id)
	    lo = mid + 1;
	లేకపోతే
	    hi = mid - 1;
    }
    తిరిగి false;
}

    స్థిర శూన్య
idl_cache_clear(synblock_T *ఖండం)
{
    idl_cache_T	*cache = (idl_cache_T *)ఖండం->b_idlist_cache;

    ఒకవేళ (cache == ఖాళీ)
	తిరిగి;
    కొరకు (పూర్ణాంకం i = 0; i < IDL_CACHE_కొలత; ++i)
	vim_free(cache->ent[i].idl_ids);
    vim_free(cache);
    ఖండం->b_idlist_cache = ఖాళీ;
}
// }}}

/*
 * Check if syntax group "ssp" is in the ID list "list" of "cur_si".
 * "cur_si" can be NULL if not checking the "containedin" list.
 * Used to check if a syntax item is in the "contains" or "nextgroup" list of
 * the current item.
 * This function is called very often, keep it fast!!
 */
    స్థిర పూర్ణాంకం
లోపల_id_list(
    stateitem_T	*cur_si,	// current item or NULL
    పొట్టి	*list,		// id list
    నిర్మాణం sp_syn *ssp,		// group id and ":syn include" tag of group
    పూర్ణాంకం		flags)		// group flags
{
    పూర్ణాంకం		retval;
    పొట్టి	*scl_list;
    పొట్టి	అంశం;
    పొట్టి	id = ssp->id;
    స్థిర పూర్ణాంకం	లోతు = 0;
    పూర్ణాంకం		r;
    పూర్ణాంకం		toplevel;
    idl_నమోదు_T	*e;

    // If ssp has a "containedin" list and "cur_si" is in it, return TRUE.
    ఒకవేళ (cur_si != ఖాళీ && ssp->cont_లోపల_list != ఖాళీ
					    && !(cur_si->si_flags & HL_సరిపోలిక))
    {
	// Ignore transparent items without a contains argument.  Double check
	// that we don't go back past the first one.
	అయితే ((cur_si->si_flags & HL_TRANS_CONT)
		&& cur_si > (stateitem_T *)(current_state.ga_దత్తాంశం))
	    --cur_si;
	// cur_si->si_idx is -1 for keywords, these never contain anything.
	ఒకవేళ (cur_si->si_idx >= 0 && లోపల_id_list(ఖాళీ, ssp->cont_లోపల_list,
		&(SYN_వస్తువులు(syn_ఖండం)[cur_si->si_idx].sp_syn),
		  SYN_వస్తువులు(syn_ఖండం)[cur_si->si_idx].sp_flags))
	    తిరిగి TRUE;
    }

    ఒకవేళ (list == ఖాళీ)
	తిరిగి FALSE;

    /*
     * If list is ID_LIST_ALL, we are in a transparent item that isn't
     * inside anything.  Only allow not-contained groups.
     */
    ఒకవేళ (list == ID_LIST_ALL)
	తిరిగి !(flags & HL_CONTAINED);

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
    e = disable_syn_idlist_cache_for_testing ? ఖాళీ : idl_get_నమోదు(list);
    ఒకవేళ (e != ఖాళీ && e->idl_usable)
    {
	ఒకవేళ (e->idl_marker != 0)
	{
	    అంశం = e->idl_marker;
	    ఒకవేళ (అంశం < SYNID_శిఖరం)
	    {
		ఒకవేళ (అంశం - SYNID_ALLBUT != ssp->inc_ట్యాగ్)
		    తిరిగి FALSE;
	    }
	    లేకపోతే ఒకవేళ (అంశం < SYNID_CONTAINED)
	    {
		ఒకవేళ (అంశం - SYNID_శిఖరం != ssp->inc_ట్యాగ్ || !toplevel)
		    తిరిగి FALSE;
	    }
	    లేకపోతే
	    {
		ఒకవేళ (అంశం - SYNID_CONTAINED != ssp->inc_ట్యాగ్ || toplevel)
		    తిరిగి FALSE;
	    }
	    retval = FALSE;
	}
	లేకపోతే
	    retval = TRUE;
	తిరిగి idl_కలిగియుంది(e, id) ? retval : !retval;
    }

    /*
     * Slow path (uncacheable list, e.g. a nested ALLBUT).
     * If the first item is "ALLBUT", return TRUE if "id" is NOT in the
     * contains list.  We also require that "id" is at the same ":syn include"
     * level as the list.
     */
    అంశం = *list;
    ఒకవేళ (అంశం >= SYNID_ALLBUT && అంశం < SYNID_CLUSTER)
    {
	ఒకవేళ (అంశం < SYNID_శిఖరం)
	{
	    // ALL or ALLBUT: accept all groups in the same file
	    ఒకవేళ (అంశం - SYNID_ALLBUT != ssp->inc_ట్యాగ్)
		తిరిగి FALSE;
	}
	లేకపోతే ఒకవేళ (అంశం < SYNID_CONTAINED)
	{
	    // TOP: accept all not-contained groups in the same file
	    ఒకవేళ (అంశం - SYNID_శిఖరం != ssp->inc_ట్యాగ్ || !toplevel)
		తిరిగి FALSE;
	}
	లేకపోతే
	{
	    // CONTAINED: accept all contained groups in the same file
	    ఒకవేళ (అంశం - SYNID_CONTAINED != ssp->inc_ట్యాగ్ || toplevel)
		తిరిగి FALSE;
	}
	అంశం = *++list;
	retval = FALSE;
    }
    లేకపోతే
	retval = TRUE;

    /*
     * Return "retval" if id is in the contains list.
     */
    అయితే (అంశం != 0)
    {
	ఒకవేళ (అంశం == id)
	    తిరిగి retval;
	ఒకవేళ (అంశం >= SYNID_CLUSTER)
	{
	    scl_list = SYN_CLSTR(syn_ఖండం)[అంశం - SYNID_CLUSTER].scl_list;
	    // restrict recursiveness to 30 to avoid an endless loop for a
	    // cluster that includes itself (indirectly)
	    ఒకవేళ (scl_list != ఖాళీ && లోతు < 30)
	    {
		++లోతు;
		r = లోపల_id_list(ఖాళీ, scl_list, ssp, flags);
		--లోతు;
		ఒకవేళ (r)
		    తిరిగి retval;
	    }
	}
	అంశం = *++list;
    }
    తిరిగి !retval;
}

నిర్మాణం subcommand
{
    అక్షరం    *name;			// subcommand name
    శూన్య    (*func)(exarg_T *, పూర్ణాంకం);	// function to call
};

స్థిర నిర్మాణం subcommand subcommands[] =
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
    {"match",		syn_cmd_సరిపోలిక},
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
    శూన్య
ex_syntax(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*subcmd_చివర;
    char_u	*subcmd_name;
    పూర్ణాంకం		i;

    syn_cmdlinep = eap->cmdlinep;

    // isolate subcommand name
    కొరకు (subcmd_చివర = arg; ASCII_ISALPHA(*subcmd_చివర); ++subcmd_చివర)
	;
    subcmd_name = vim_strnsave(arg, subcmd_చివర - arg);
    ఒకవేళ (subcmd_name == ఖాళీ)
	తిరిగి;

    ఒకవేళ (eap->skip)		// skip error messages for all subcommands
	++emsg_skip;
    కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(subcommands); ++i)
    {
	ఒకవేళ (STRCMP(subcmd_name, (char_u *)subcommands[i].name) == 0)
	{
	    eap->arg = skipwhite(subcmd_చివర);
	    (subcommands[i].func)(eap, FALSE);
	    ఆపు;
	}
    }

    ఒకవేళ (i == (పూర్ణాంకం)ARRAY_నిడివి(subcommands))
	semsg(_(e_invalid_syntax_subcommand_str), subcmd_name);

    vim_free(subcmd_name);
    ఒకవేళ (eap->skip)
	--emsg_skip;
}

    శూన్య
ex_ownsyntax(exarg_T *eap)
{
    char_u	*old_విలువ;
    char_u	*new_విలువ_2;

    ఒకవేళ (curwin->w_s == &curwin->w_బఫరు->b_s)
    {
	curwin->w_s = ALLOC_ONE(synblock_T);
	CLEAR_సూచిక(curwin->w_s);
	hash_init(&curwin->w_s->b_keywtab);
	hash_init(&curwin->w_s->b_keywtab_ic);
#నిర్వచితమైతే FEAT_SPELL
	// TODO: keep the spell checking as it was.
	curwin->w_p_spell = FALSE;	// No spell checking
	// make sure option values are "empty_option" instead of NULL
	clear_string_option(&curwin->w_s->b_p_spc);
	clear_string_option(&curwin->w_s->b_p_spf);
	clear_string_option(&curwin->w_s->b_p_spl);
	clear_string_option(&curwin->w_s->b_p_spo);
#అయితే_ముగింపు
	clear_string_option(&curwin->w_s->b_syn_isk);
    }

    // save value of b:current_syntax
    old_విలువ = get_var_విలువ((char_u *)"b:current_syntax");
    ఒకవేళ (old_విలువ != ఖాళీ)
	old_విలువ = vim_strsave(old_విలువ);

    // Apply the "syntax" autocommand event, this finds and loads the syntax
    // file.
    apply_autocmds(EVENT_SYNTAX, eap->arg, curbuf->b_fname, TRUE, curbuf);

    // move value of b:current_syntax to w:current_syntax
    new_విలువ_2 = get_var_విలువ((char_u *)"b:current_syntax");
    ఒకవేళ (new_విలువ_2 != ఖాళీ)
	సమితి_అంతర్గత_string_var((char_u *)"w:current_syntax", new_విలువ_2);

    // restore value of b:current_syntax
    ఒకవేళ (old_విలువ == ఖాళీ)
	do_unlet((char_u *)"b:current_syntax", TRUE);
    లేకపోతే
    {
	సమితి_అంతర్గత_string_var((char_u *)"b:current_syntax", old_విలువ);
	vim_free(old_విలువ);
    }
}

    పూర్ణాంకం
syntax_present(win_T *win)
{
    తిరిగి (win->w_s->b_syn_patterns.ga_len != 0
	    || win->w_s->b_syn_clusters.ga_len != 0
	    || win->w_s->b_keywtab.ht_వాడిన > 0
	    || win->w_s->b_keywtab_ic.ht_వాడిన > 0);
}


స్థిర గణన
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
    శూన్య
reset_expand_highlight(శూన్య)
{
    include_link = include_default = include_none = 0;
}

/*
 * Handle command line completion for :match and :echohl command: Add "NONE"
 * as highlight group.
 */
    శూన్య
సమితి_context_లోపల_echohl_cmd(expand_T *xp, char_u *arg)
{
    xp->xp_context = EXPAND_HIGHLIGHT;
    xp->xp_నమూనా = arg;
    include_none = 1;
}

/*
 * Handle command line completion for :syntax command.
 */
    శూన్య
సమితి_context_లోపల_syntax_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    // Default: expand subcommands
    xp->xp_context = EXPAND_SYNTAX;
    expand_what = EXP_SUBCMD;
    xp->xp_నమూనా = arg;
    include_link = 0;
    include_default = 0;

    ఒకవేళ (*arg == NUL)
	తిరిగి;

    // (part of) subcommand already typed
    p = skiptowhite(arg);
    ఒకవేళ (*p == NUL)
	తిరిగి;

    // past first word
    xp->xp_నమూనా = skipwhite(p);
    ఒకవేళ (*skiptowhite(xp->xp_నమూనా) != NUL)
	xp->xp_context = EXPAND_NOTHING;
    లేకపోతే ఒకవేళ (STRNICMP(arg, "case", p - arg) == 0)
	expand_what = EXP_CASE;
    లేకపోతే ఒకవేళ (STRNICMP(arg, "spell", p - arg) == 0)
	expand_what = EXP_SPELL;
    లేకపోతే ఒకవేళ (STRNICMP(arg, "sync", p - arg) == 0)
	expand_what = EXP_SYNC;
    లేకపోతే ఒకవేళ (STRNICMP(arg, "list", p - arg) == 0)
    {
	p = skipwhite(p);
	ఒకవేళ (*p == '@')
	    expand_what = EXP_CLUSTER;
	లేకపోతే
	    xp->xp_context = EXPAND_HIGHLIGHT;
    }
    లేకపోతే ఒకవేళ (STRNICMP(arg, "keyword", p - arg) == 0
	    || STRNICMP(arg, "region", p - arg) == 0
	    || STRNICMP(arg, "match", p - arg) == 0)
	xp->xp_context = EXPAND_HIGHLIGHT;
    లేకపోతే
	xp->xp_context = EXPAND_NOTHING;
}

/*
 * Function given to ExpandGeneric() to obtain the list syntax names for
 * expansion.
 */
    char_u *
get_syntax_name(expand_T *xp, పూర్ణాంకం idx)
{
    ఎంపిక (expand_what)
    {
	స్థితి EXP_SUBCMD:
	    ఒకవేళ (idx < 0 || idx >= (పూర్ణాంకం)ARRAY_నిడివి(subcommands))
		తిరిగి ఖాళీ;
	    తిరిగి (char_u *)subcommands[idx].name;
	స్థితి EXP_CASE:
	{
	    స్థిర అక్షరం *case_వాదనలు[] = {"match", "ignore", ఖాళీ};
	    తిరిగి (char_u *)case_వాదనలు[idx];
	}
	స్థితి EXP_SPELL:
	{
	    స్థిర అక్షరం *spell_వాదనలు[] =
		{"toplevel", "notoplevel", "default", ఖాళీ};
	    తిరిగి (char_u *)spell_వాదనలు[idx];
	}
	స్థితి EXP_SYNC:
	{
	    స్థిర అక్షరం *sync_వాదనలు[] =
		{"ccomment", "clear", "fromstart",
		 "linebreaks=", "linecont", "lines=", "match",
		 "maxlines=", "minlines=", "region", ఖాళీ};
	    తిరిగి (char_u *)sync_వాదనలు[idx];
	}
	స్థితి EXP_CLUSTER:
	{
	    ఒకవేళ (idx < curwin->w_s->b_syn_clusters.ga_len)
	    {
		vim_snprintf((అక్షరం *)xp->xp_buf, EXPAND_BUF_LEN, "@%s",
					 SYN_CLSTR(curwin->w_s)[idx].scl_name);
		తిరిగి xp->xp_buf;
	    }
	    లేకపోతే
		తిరిగి ఖాళీ;
	}
    }
    తిరిగి ఖాళీ;
}


/*
 * Function called for expression evaluation: get syntax ID at file position.
 */
    పూర్ణాంకం
syn_get_id(
    win_T	*wp,
    దీర్ఘ	lnum,
    colnr_T	నిలువు,
    పూర్ణాంకం		trans,	     // remove transparency
    పూర్ణాంకం		*spellp,     // return: can do spell checking
    పూర్ణాంకం		keep_state)  // keep state of char at "col"
{
    // When the position is not after the current position and in the same
    // line of the same window with the same buffer, need to restart parsing.
    ఒకవేళ (wp != syn_win
	    || wp->w_బఫరు != syn_buf
	    || lnum != current_lnum
	    || నిలువు < current_నిలువు)
	syntax_ప్రారంభం(wp, lnum);
    లేకపోతే ఒకవేళ (wp->w_బఫరు == syn_buf
	    && lnum == current_lnum
	    && నిలువు > current_నిలువు)
	// next_match may not be correct when moving around, e.g. with the
	// "skip" expression in searchpair()
	తదుపరి_సరిపోలిక_idx = -1;

    (శూన్య)get_syntax_attr(నిలువు, spellp, keep_state);

    తిరిగి (trans ? current_trans_id : current_id);
}

#ఒకవేళ defined(FEAT_CONCEAL)
/*
 * Get extra information about the syntax item.  Must be called right after
 * get_syntax_attr().
 * Stores the current item sequence nr in "*seqnrp".
 * Returns the current flags.
 */
    పూర్ణాంకం
get_syntax_info(పూర్ణాంకం *seqnrp)
{
    *seqnrp = current_seqnr;
    తిరిగి current_flags;
}

/*
 * Return conceal substitution character
 */
    పూర్ణాంకం
syn_get_sub_char(శూన్య)
{
    తిరిగి current_sub_char;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Return the syntax ID at position "i" in the current stack.
 * The caller must have called syn_get_id() before to fill the stack.
 * Returns -1 when "i" is out of range.
 */
    పూర్ణాంకం
syn_get_దొంతర_అంశం(పూర్ణాంకం i)
{
    ఒకవేళ (i >= current_state.ga_len)
    {
	// Need to invalidate the state, because we didn't properly finish it
	// for the last character, "keep_state" was TRUE.
	invalidate_current_state();
	current_నిలువు = MAXCOL;
	తిరిగి -1;
    }
    తిరిగి CUR_STATE(i).si_id;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_FOLDING)
    స్థిర పూర్ణాంకం
syn_cur_foldlevel(శూన్య)
{
    పూర్ణాంకం		level = 0;
    పూర్ణాంకం		i;

    కొరకు (i = 0; i < current_state.ga_len; ++i)
	ఒకవేళ (CUR_STATE(i).si_flags & HL_FOLD)
	    ++level;
    తిరిగి level;
}

/*
 * Function called to get folding level for line "lnum" in window "wp".
 */
    పూర్ణాంకం
syn_get_foldlevel(win_T *wp, దీర్ఘ lnum)
{
    పూర్ణాంకం		level = 0;
    పూర్ణాంకం		దిగువ_level;
    పూర్ణాంకం		cur_level;

    // Return quickly when there are no fold items at all.
    ఒకవేళ (wp->w_s->b_syn_folditems != 0
	    && !wp->w_s->b_syn_error
# ifdef SYN_సమయం_హద్దు
	    && !wp->w_s->b_syn_slow
# endif
	    )
    {
	syntax_ప్రారంభం(wp, lnum);

	// Start with the fold level at the start of the line.
	level = syn_cur_foldlevel();

	ఒకవేళ (wp->w_s->b_syn_foldlevel == SYNFLD_MINIMUM)
	{
	    // Find the lowest fold level that is followed by a higher one.
	    cur_level = level;
	    దిగువ_level = cur_level;
	    అయితే (!current_ముగిసిన)
	    {
		(శూన్య)syn_current_attr(FALSE, FALSE, ఖాళీ, FALSE);
		cur_level = syn_cur_foldlevel();
		ఒకవేళ (cur_level < దిగువ_level)
		    దిగువ_level = cur_level;
		లేకపోతే ఒకవేళ (cur_level > దిగువ_level)
		    level = దిగువ_level;
		++current_నిలువు;
	    }
	}
    }
    ఒకవేళ (level > wp->w_p_fdn)
    {
	level = wp->w_p_fdn;
	ఒకవేళ (level < 0)
	    level = 0;
    }
    తిరిగి level;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_PROFILE)
/*
 * ":syntime".
 */
    శూన్య
ex_syntime(exarg_T *eap)
{
    ఒకవేళ (STRCMP(eap->arg, "on") == 0)
	syn_సమయం_on = TRUE;
    లేకపోతే ఒకవేళ (STRCMP(eap->arg, "off") == 0)
	syn_సమయం_on = FALSE;
    లేకపోతే ఒకవేళ (STRCMP(eap->arg, "clear") == 0)
	syntime_clear();
    లేకపోతే ఒకవేళ (STRCMP(eap->arg, "report") == 0)
	syntime_report();
    లేకపోతే
	semsg(_(e_invalid_వాదం_str), eap->arg);
}

    స్థిర శూన్య
syn_clear_సమయం(syn_సమయం_T *st)
{
    profile_zero(&st->మొత్తం);
    profile_zero(&st->slowest);
    st->లెక్క = 0;
    st->సరిపోలిక = 0;
}

/*
 * Clear the syntax timing for the current buffer.
 */
    స్థిర శూన్య
syntime_clear(శూన్య)
{
    పూర్ణాంకం		idx;
    synpat_T	*spp;

    ఒకవేళ (!syntax_present(curwin))
    {
	సందేశం(_(సందేశం_no_వస్తువులు));
	తిరిగి;
    }
    కొరకు (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len; ++idx)
    {
	spp = &(SYN_వస్తువులు(curwin->w_s)[idx]);
	syn_clear_సమయం(&spp->sp_సమయం);
    }
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":syntime {on,off,clear,report}" command.
 */
    char_u *
get_syntime_arg(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    ఎంపిక (idx)
    {
	స్థితి 0: తిరిగి (char_u *)"on";
	స్థితి 1: తిరిగి (char_u *)"off";
	స్థితి 2: తిరిగి (char_u *)"clear";
	స్థితి 3: తిరిగి (char_u *)"report";
    }
    తిరిగి ఖాళీ;
}

రకనిర్వచనం నిర్మాణం
{
    proftime_T	మొత్తం;
    పూర్ణాంకం		లెక్క;
    పూర్ణాంకం		సరిపోలిక;
    proftime_T	slowest;
    proftime_T	సరాసరి;
    పూర్ణాంకం		id;
    char_u	*నమూనా;
} సమయం_నమోదు_T;

    స్థిర పూర్ణాంకం
syn_compare_syntime(స్థిరరాశి శూన్య *v1, స్థిరరాశి శూన్య *v2)
{
    స్థిరరాశి సమయం_నమోదు_T	*s1 = v1;
    స్థిరరాశి సమయం_నమోదు_T	*s2 = v2;

    తిరిగి profile_cmp(&s1->మొత్తం, &s2->మొత్తం);
}

/*
 * Clear the syntax timing for the current buffer.
 */
    స్థిర శూన్య
syntime_report(శూన్య)
{
    పూర్ణాంకం		idx;
    synpat_T	*spp;
# ఒకవేళ defined(FEAT_RELTIME)
    proftime_T	tm;
# endif
    పూర్ణాంకం		len;
    పూర్ణాంకం		patlen;
    proftime_T	మొత్తం_మొత్తం;
    పూర్ణాంకం		మొత్తం_లెక్క = 0;
    garray_T    ga;
    సమయం_నమోదు_T *p;

    ఒకవేళ (!syntax_present(curwin))
    {
	సందేశం(_(సందేశం_no_వస్తువులు));
	తిరిగి;
    }

    ga_init2(&ga, పరిమాణం(సమయం_నమోదు_T), 50);
    profile_zero(&మొత్తం_మొత్తం);
    కొరకు (idx = 0; idx < curwin->w_s->b_syn_patterns.ga_len; ++idx)
    {
	spp = &(SYN_వస్తువులు(curwin->w_s)[idx]);
	ఒకవేళ (spp->sp_సమయం.లెక్క > 0)
	{
	    (శూన్య)ga_grow(&ga, 1);
	    p = ((సమయం_నమోదు_T *)ga.ga_దత్తాంశం) + ga.ga_len;
	    p->మొత్తం = spp->sp_సమయం.మొత్తం;
	    profile_కూడు(&మొత్తం_మొత్తం, &spp->sp_సమయం.మొత్తం);
	    p->లెక్క = spp->sp_సమయం.లెక్క;
	    p->సరిపోలిక = spp->sp_సమయం.సరిపోలిక;
	    మొత్తం_లెక్క += spp->sp_సమయం.లెక్క;
	    p->slowest = spp->sp_సమయం.slowest;
# ఒకవేళ defined(FEAT_RELTIME)
	    profile_divide(&spp->sp_సమయం.మొత్తం, spp->sp_సమయం.లెక్క, &tm);
	    p->సరాసరి = tm;
# endif
	    p->id = spp->sp_syn.id;
	    p->నమూనా = spp->sp_నమూనా;
	    ++ga.ga_len;
	}
    }

    // Sort on total time. Skip if there are no items to avoid passing NULL
    // pointer to qsort().
    ఒకవేళ (ga.ga_len > 1)
	qsort(ga.ga_దత్తాంశం, (size_t)ga.ga_len, పరిమాణం(సమయం_నమోదు_T),
							 syn_compare_syntime);

    సందేశం_puts_title(_("  TOTAL      COUNT  MATCH   SLOWEST     AVERAGE   NAME               PATTERN"));
    సందేశం_puts("\n");
    కొరకు (idx = 0; idx < ga.ga_len && !got_int; ++idx)
    {
	p = ((సమయం_నమోదు_T *)ga.ga_దత్తాంశం) + idx;

	సందేశం_puts(profile_సందేశం(&p->మొత్తం));
	సందేశం_puts(" "); // make sure there is always a separating space
	సందేశం_advance(13);
	సందేశం_outnum(p->లెక్క);
	సందేశం_puts(" ");
	సందేశం_advance(20);
	సందేశం_outnum(p->సరిపోలిక);
	సందేశం_puts(" ");
	సందేశం_advance(26);
	సందేశం_puts(profile_సందేశం(&p->slowest));
	సందేశం_puts(" ");
	సందేశం_advance(38);
	సందేశం_puts(profile_సందేశం(&p->సరాసరి));
	సందేశం_puts(" ");
	సందేశం_advance(50);
	సందేశం_outtrans(highlight_సమూహం_name(p->id - 1));
	సందేశం_puts(" ");

	సందేశం_advance(69);
	ఒకవేళ (Columns < 80)
	    len = 20; // will wrap anyway
	లేకపోతే
	    len = Columns - 70;
	patlen = (పూర్ణాంకం)STRLEN(p->నమూనా);
	ఒకవేళ (len > patlen)
	    len = patlen;
	సందేశం_outtrans_len(p->నమూనా, len);
	సందేశం_puts("\n");
    }
    ga_clear(&ga);
    ఒకవేళ (!got_int)
    {
	సందేశం_puts("\n");
	సందేశం_puts(profile_సందేశం(&మొత్తం_మొత్తం));
	సందేశం_advance(13);
	సందేశం_outnum(మొత్తం_లెక్క);
	సందేశం_puts("\n");
    }
}
#అయితే_ముగింపు

#అయితే_ముగింపు // FEAT_SYN_HL
