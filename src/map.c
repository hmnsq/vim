/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * map.c: Code for mappings and abbreviations.
 */

#অন্তর্ভাব "vim.h"

/*
 * List used for abbreviations.
 */
স্থির mapblock_T	*first_abbr = ফাঁকা; // first entry in abbrlist

/*
 * Each mapping is put in one of the 256 hash lists, to speed up finding it.
 */
স্থির mapblock_T	*(maphash[256]);
স্থির পূর্ণসংখ্যা		maphash_valid = FALSE;

// When non-zero then no mappings can be added or removed.  Prevents mappings
// to change while listing them.
স্থির পূর্ণসংখ্যা		map_locked = 0;

/*
 * Make a hash value for a mapping.
 * "mode" is the lower 4 bits of the State for the mapping.
 * "c1" is the first character of the "lhs".
 * Returns a value between 0 and 255, index in maphash.
 * Put Normal/Visual mode mappings mostly separately from Insert/Cmdline mode.
 */
#define MAP_HASH(mode, c1) (((mode) & (MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING | MODE_TERMINAL)) ? (c1) : ((c1) ^ 0x80))

/*
 * Get the start of the hashed map list for "state" and first character "c".
 */
    mapblock_T *
get_maphash_list(পূর্ণসংখ্যা state, পূর্ণসংখ্যা c)
{
    ফেরত maphash[MAP_HASH(state, c)];
}

/*
 * Get the buffer-local hashed map list for "state" and first character "c".
 */
    mapblock_T *
get_buf_maphash_list(পূর্ণসংখ্যা state, পূর্ণসংখ্যা c)
{
    ফেরত curbuf->b_maphash[MAP_HASH(state, c)];
}

    পূর্ণসংখ্যা
is_maphash_valid(শূন্য)
{
    ফেরত maphash_valid;
}

/*
 * Initialize maphash[] for first use.
 */
    স্থির শূন্য
validate_maphash(শূন্য)
{
    যদি (maphash_valid)
	ফেরত;

    CLEAR_FIELD(maphash);
    maphash_valid = TRUE;
}

/*
 * Delete one entry from the abbrlist or maphash[].
 * "mpp" is a pointer to the m_next field of the PREVIOUS entry!
 */
    স্থির শূন্য
map_free(mapblock_T **mpp)
{
    mapblock_T	*mp;

    mp = *mpp;
    vim_free(mp->m_চাবিগুলি);
    যদি (mp->m_alt != ফাঁকা)
	mp->m_alt->m_alt = ফাঁকা;
    vim_free(mp->m_str);
    vim_free(mp->m_orig_str);
    *mpp = mp->m_পরবর্তী;
#ifdef FEAT_EVAL
    reset_last_used_map(mp);
#endif
    vim_free(mp);
}

/*
 * Return characters to represent the map mode in an allocated string.
 * Returns NULL when out of memory.
 */
    স্থির char_u *
map_mode_to_chars(পূর্ণসংখ্যা mode)
{
    garray_T    mapmode;

    ga_init2(&mapmode, 1, 7);

    যদি ((mode & (MODE_ঢোকাও | MODE_CMDLINE)) == (MODE_ঢোকাও | MODE_CMDLINE))
	ga_append(&mapmode, '!');			// :map!
    অন্যথা যদি (mode & MODE_ঢোকাও)
	ga_append(&mapmode, 'i');			// :imap
    অন্যথা যদি (mode & MODE_LANGMAP)
	ga_append(&mapmode, 'l');			// :lmap
    অন্যথা যদি (mode & MODE_CMDLINE)
	ga_append(&mapmode, 'c');			// :cmap
    অন্যথা যদি ((mode
		 & (MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING))
		== (MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING))
	ga_append(&mapmode, ' ');			// :map
    অন্যথা
    {
	যদি (mode & MODE_NORMAL)
	    ga_append(&mapmode, 'n');			// :nmap
	যদি (mode & MODE_OP_PENDING)
	    ga_append(&mapmode, 'o');			// :omap
	যদি (mode & MODE_TERMINAL)
	    ga_append(&mapmode, 't');			// :tmap
	যদি ((mode & (MODE_VISUAL | MODE_SELECT)) == (MODE_VISUAL | MODE_SELECT))
	    ga_append(&mapmode, 'v');			// :vmap
	অন্যথা
	{
	    যদি (mode & MODE_VISUAL)
		ga_append(&mapmode, 'x');		// :xmap
	    যদি (mode & MODE_SELECT)
		ga_append(&mapmode, 's');		// :smap
	}
    }

    ga_append(&mapmode, NUL);
    ফেরত (char_u *)mapmode.ga_তথ্য;
}

/*
 * Output a line for one mapping.
 */
    স্থির শূন্য
showmap(
    mapblock_T	*mp,
    পূর্ণসংখ্যা		local)	    // TRUE for buffer-local map
{
    পূর্ণসংখ্যা		len = 1;
    char_u	*mapchars;

    যদি (message_filtered(mp->m_চাবিগুলি) && message_filtered(mp->m_str))
	ফেরত;

    // Prevent mappings to be cleared while at the more prompt.
    // Must jump to "theend" instead of returning.
    ++map_locked;

    যদি (বার্তা_didout || বার্তা_silent != 0)
    {
	বার্তা_putchar('\n');
	যদি (got_int)	    // 'q' typed at MORE prompt
	    যাও theend;
    }

    mapchars = map_mode_to_chars(mp->m_mode);
    যদি (mapchars != ফাঁকা)
    {
	বার্তা_puts((অক্ষর *)mapchars);
	len = (পূর্ণসংখ্যা)STRLEN(mapchars);
	vim_free(mapchars);
    }

    যখন (++len <= 3)
	বার্তা_putchar(' ');

    // Display the LHS.  Get length of what we write.
    len = বার্তা_outtrans_special(mp->m_চাবিগুলি, TRUE, 0);
    করো
    {
	বার্তা_putchar(' ');		// pad with blanks
	++len;
    } যখন (len < 12);

    যদি (mp->m_noremap == REMAP_NONE)
	বার্তা_puts_attr("*", HL_ATTR(HLF_8));
    অন্যথা যদি (mp->m_noremap == REMAP_SCRIPT)
	বার্তা_puts_attr("&", HL_ATTR(HLF_8));
    অন্যথা
	বার্তা_putchar(' ');

    যদি (local)
	বার্তা_putchar('@');
    অন্যথা
	বার্তা_putchar(' ');

    // Use FALSE below if we only want things like <Up> to show up as such on
    // the rhs, and not M-x etc, TRUE gets both -- webb
    যদি (*mp->m_str == NUL)
	বার্তা_puts_attr("<Nop>", HL_ATTR(HLF_8));
    অন্যথা
	বার্তা_outtrans_special(mp->m_str, FALSE, 0);
#ifdef FEAT_EVAL
    যদি (p_verbose > 0)
	last_set_বার্তা(mp->m_script_ctx);
#endif
    বার্তা_clr_eos();
    out_flush();			// show one line at a time

theend:
    --map_locked;
}

    স্থির mapblock_T *
map_add(
	mapblock_T  **map_table,
	mapblock_T  **abbr_table,
	char_u	    *চাবিগুলি,
	char_u	    *rhs,
	char_u	    *orig_rhs,
	পূর্ণসংখ্যা	    noremap,
	পূর্ণসংখ্যা	    nowait,
	পূর্ণসংখ্যা	    silent,
	পূর্ণসংখ্যা	    mode,
	পূর্ণসংখ্যা	    is_abbr,
#ifdef FEAT_EVAL
	পূর্ণসংখ্যা	    রাশিমালা,
	scid_T	    sid,	    // 0 to use current_sctx
	পূর্ণসংখ্যা	    scriptversion,
	linenr_T    lnum,
#endif
	পূর্ণসংখ্যা	    simplified)
{
    mapblock_T	*mp = ALLOC_CLEAR_ONE(mapblock_T);

    যদি (mp == ফাঁকা)
	ফেরত ফাঁকা;

    // If CTRL-C has been mapped, don't always use it for Interrupting.
    যদি (*চাবিগুলি == Ctrl_C)
    {
	যদি (map_table == curbuf->b_maphash)
	    curbuf->b_mapped_ctrl_c |= mode;
	অন্যথা
	    mapped_ctrl_c |= mode;
    }

    mp->m_চাবিগুলি = vim_strsave(চাবিগুলি);
    mp->m_str = vim_strsave(rhs);
    mp->m_orig_str = vim_strsave(orig_rhs);
    যদি (mp->m_চাবিগুলি == ফাঁকা || mp->m_str == ফাঁকা)
    {
	vim_free(mp->m_চাবিগুলি);
	vim_free(mp->m_str);
	vim_free(mp->m_orig_str);
	vim_free(mp);
	ফেরত ফাঁকা;
    }
    mp->m_keylen = (পূর্ণসংখ্যা)STRLEN(mp->m_চাবিগুলি);
    mp->m_noremap = noremap;
    mp->m_nowait = nowait;
    mp->m_silent = silent;
    mp->m_mode = mode;
    mp->m_simplified = simplified;
#ifdef FEAT_EVAL
    mp->m_রাশিমালা = রাশিমালা;
    যদি (sid != 0)
    {
	mp->m_script_ctx.sc_sid = sid;
	mp->m_script_ctx.sc_lnum = lnum;
	mp->m_script_ctx.sc_version = scriptversion;
    }
    অন্যথা
    {
	mp->m_script_ctx = current_sctx;
	যদি (cmdmod.cmod_flags & CMOD_LEGACY)
	    mp->m_script_ctx.sc_version = 1;
	mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
    }
#endif

    // add the new entry in front of the abbrlist or maphash[] list
    যদি (is_abbr)
    {
	mp->m_পরবর্তী = *abbr_table;
	*abbr_table = mp;
    }
    অন্যথা
    {
	পূর্ণসংখ্যা n = MAP_HASH(mp->m_mode, mp->m_চাবিগুলি[0]);

	mp->m_পরবর্তী = map_table[n];
	map_table[n] = mp;
    }
    ফেরত mp;
}

/*
 * List mappings.  When "haskey" is FALSE all mappings, otherwise mappings that
 * match "keys[keys_len]".
 */
    স্থির শূন্য
list_mappings(
	পূর্ণসংখ্যা	keyround,
	পূর্ণসংখ্যা	abbrev,
	পূর্ণসংখ্যা	haskey,
	char_u	*চাবিগুলি,
	পূর্ণসংখ্যা	চাবিগুলি_len,
	পূর্ণসংখ্যা	mode,
	পূর্ণসংখ্যা	*did_local)
{
    // Prevent mappings to be cleared while at the more prompt.
    ++map_locked;

    যদি (p_verbose > 0 && keyround == 1)
    {
	যদি (seen_Modify_Other_চাবিগুলি)
	    বার্তা_puts(_("Seen modifyOtherKeys: true\n"));

	যদি (modify_otherkeys_state != MOKS_INITIAL)
	{
	    অক্ষর *name = _("Unknown");
	    নির্বাচন (modify_otherkeys_state)
	    {
		ক্ষেত্র MOKS_INITIAL: বিরতি;
		ক্ষেত্র MOKS_OFF: name = _("Off"); বিরতি;
		ক্ষেত্র MOKS_ENABLED: name = _("On"); বিরতি;
		ক্ষেত্র MOKS_DISABLED: name = _("Disabled"); বিরতি;
		ক্ষেত্র MOKS_AFTER_T_TE: name = _("Cleared"); বিরতি;
	    }

	    অক্ষর buf[200];
	    vim_snprintf(buf, আকার(buf),
				    _("modifyOtherKeys detected: %s\n"), name);
	    বার্তা_puts(buf);
	}

	যদি (kitty_protocol_state != KKPS_INITIAL)
	{
	    অক্ষর *name = _("Unknown");
	    নির্বাচন (kitty_protocol_state)
	    {
		ক্ষেত্র KKPS_INITIAL: বিরতি;
		ক্ষেত্র KKPS_OFF: name = _("Off"); বিরতি;
		ক্ষেত্র KKPS_ENABLED: name = _("On"); বিরতি;
		ক্ষেত্র KKPS_DISABLED: name = _("Disabled"); বিরতি;
		ক্ষেত্র KKPS_AFTER_T_TE: name = _("Cleared"); বিরতি;
	    }

	    অক্ষর buf[200];
	    vim_snprintf(buf, আকার(buf),
				     _("Kitty keyboard protocol: %s\n"), name);
	    বার্তা_puts(buf);
	}
    }

    // need to loop over all global hash lists
    জন্য (পূর্ণসংখ্যা hash = 0; hash < 256 && !got_int; ++hash)
    {
	mapblock_T	*mp;

	যদি (abbrev)
	{
	    যদি (hash != 0)	// there is only one abbreviation list
		বিরতি;
	    mp = curbuf->b_first_abbr;
	}
	অন্যথা
	    mp = curbuf->b_maphash[hash];
	জন্য ( ; mp != ফাঁকা && !got_int; mp = mp->m_পরবর্তী)
	{
	    // check entries with the same mode
	    যদি (!mp->m_simplified && (mp->m_mode & mode) != 0)
	    {
		যদি (!haskey)		    // show all entries
		{
		    showmap(mp, TRUE);
		    *did_local = TRUE;
		}
		অন্যথা
		{
		    পূর্ণসংখ্যা n = mp->m_keylen;
		    যদি (STRNCMP(mp->m_চাবিগুলি, চাবিগুলি,
				   (size_t)(n < চাবিগুলি_len ? n : চাবিগুলি_len)) == 0)
		    {
			showmap(mp, TRUE);
			*did_local = TRUE;
		    }
		}
	    }
	}
    }

    --map_locked;
}

/*
 * map[!]		    : show all key mappings
 * map[!] {lhs}		    : show key mapping for {lhs}
 * map[!] {lhs} {rhs}	    : set key mapping for {lhs} to {rhs}
 * noremap[!] {lhs} {rhs}   : same, but no remapping for {rhs}
 * unmap[!] {lhs}	    : remove key mapping for {lhs}
 * abbr			    : show all abbreviations
 * abbr {lhs}		    : show abbreviations for {lhs}
 * abbr {lhs} {rhs}	    : set abbreviation for {lhs} to {rhs}
 * noreabbr {lhs} {rhs}	    : same, but no remapping for {rhs}
 * unabbr {lhs}		    : remove abbreviation for {lhs}
 *
 * maptype: MAPTYPE_MAP for :map or :abbr
 *	    MAPTYPE_UNMAP for :unmap or :unabbr
 *	    MAPTYPE_NOREMAP for :noremap or :noreabbr
 *	    MAPTYPE_UNMAP_LHS is like MAPTYPE_UNMAP, but doesn't try to match
 *	    with {rhs} if there is no match with {lhs}.
 *
 * arg is pointer to any arguments. Note: arg cannot be a read-only string,
 * it will be modified.
 *
 * for :map   mode is MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING
 * for :map!  mode is MODE_INSERT | MODE_CMDLINE
 * for :cmap  mode is MODE_CMDLINE
 * for :imap  mode is MODE_INSERT
 * for :lmap  mode is MODE_LANGMAP
 * for :nmap  mode is MODE_NORMAL
 * for :vmap  mode is MODE_VISUAL | MODE_SELECT
 * for :xmap  mode is MODE_VISUAL
 * for :smap  mode is MODE_SELECT
 * for :omap  mode is MODE_OP_PENDING
 * for :tmap  mode is MODE_TERMINAL
 *
 * for :abbr  mode is MODE_INSERT | MODE_CMDLINE
 * for :iabbr mode is MODE_INSERT
 * for :cabbr mode is MODE_CMDLINE
 *
 * Return 0 for success
 *	  1 for invalid arguments
 *	  2 for no match
 *	  4 for out of mem
 *	  5 for entry not unique
 */
    পূর্ণসংখ্যা
do_map(
    পূর্ণসংখ্যা		maptype,
    char_u	*arg,
    পূর্ণসংখ্যা		mode,
    পূর্ণসংখ্যা		abbrev)		// not a mapping but an abbreviation
{
    char_u	*চাবিগুলি;
    mapblock_T	*mp, **mpp;
    mapblock_T	*mp_ফলাফল[2] = {ফাঁকা, ফাঁকা};
    char_u	*rhs;
    char_u	*p;
    পূর্ণসংখ্যা		n;
    পূর্ণসংখ্যা		len = 0;	// init for GCC
    পূর্ণসংখ্যা		hasarg;
    পূর্ণসংখ্যা		haskey;
    পূর্ণসংখ্যা		do_print;
    পূর্ণসংখ্যা		keyround;
    char_u	*চাবিগুলি_buf = ফাঁকা;
    char_u	*alt_চাবিগুলি_buf = ফাঁকা;
    char_u	*arg_buf = ফাঁকা;
    পূর্ণসংখ্যা		retval = 0;
    পূর্ণসংখ্যা		do_backslash;
    mapblock_T	**abbr_table;
    mapblock_T	**map_table;
    পূর্ণসংখ্যা		unique = FALSE;
    পূর্ণসংখ্যা		nowait = FALSE;
    পূর্ণসংখ্যা		silent = FALSE;
    পূর্ণসংখ্যা		special = FALSE;
#ifdef FEAT_EVAL
    পূর্ণসংখ্যা		রাশিমালা = FALSE;
#endif
    পূর্ণসংখ্যা		did_simplify = FALSE;
    পূর্ণসংখ্যা		unmap_lhs_only = FALSE;
    পূর্ণসংখ্যা		noremap;
    char_u      *orig_rhs;

    চাবিগুলি = arg;
    map_table = maphash;
    abbr_table = &first_abbr;

    যদি (maptype == MAPTYPE_UNMAP_LHS)
    {
	unmap_lhs_only = TRUE;
	maptype = MAPTYPE_UNMAP;
    }

    // For ":noremap" don't remap, otherwise do remap.
    যদি (maptype == MAPTYPE_NOREMAP)
	noremap = REMAP_NONE;
    অন্যথা
	noremap = REMAP_YES;

    // Accept <buffer>, <nowait>, <silent>, <expr> <script> and <unique> in
    // any order.
    জন্য (;;)
    {
	// Check for "<buffer>": mapping local to buffer.
	যদি (STRNCMP(চাবিগুলি, "<buffer>", 8) == 0)
	{
	    চাবিগুলি = skipwhite(চাবিগুলি + 8);
	    map_table = curbuf->b_maphash;
	    abbr_table = &curbuf->b_first_abbr;
	    চালু;
	}

	// Check for "<nowait>": don't wait for more characters.
	যদি (STRNCMP(চাবিগুলি, "<nowait>", 8) == 0)
	{
	    চাবিগুলি = skipwhite(চাবিগুলি + 8);
	    nowait = TRUE;
	    চালু;
	}

	// Check for "<silent>": don't echo commands.
	যদি (STRNCMP(চাবিগুলি, "<silent>", 8) == 0)
	{
	    চাবিগুলি = skipwhite(চাবিগুলি + 8);
	    silent = TRUE;
	    চালু;
	}

	// Check for "<special>": accept special keys in <>
	যদি (STRNCMP(চাবিগুলি, "<special>", 9) == 0)
	{
	    চাবিগুলি = skipwhite(চাবিগুলি + 9);
	    special = TRUE;
	    চালু;
	}

#ifdef FEAT_EVAL
	// Check for "<script>": remap script-local mappings only
	যদি (STRNCMP(চাবিগুলি, "<script>", 8) == 0)
	{
	    চাবিগুলি = skipwhite(চাবিগুলি + 8);
	    noremap = REMAP_SCRIPT;
	    চালু;
	}

	// Check for "<expr>": {rhs} is an expression.
	যদি (STRNCMP(চাবিগুলি, "<expr>", 6) == 0)
	{
	    চাবিগুলি = skipwhite(চাবিগুলি + 6);
	    রাশিমালা = TRUE;
	    চালু;
	}
#endif
	// Check for "<unique>": don't overwrite an existing mapping.
	যদি (STRNCMP(চাবিগুলি, "<unique>", 8) == 0)
	{
	    চাবিগুলি = skipwhite(চাবিগুলি + 8);
	    unique = TRUE;
	    চালু;
	}
	বিরতি;
    }

    validate_maphash();

    // Find end of keys and skip CTRL-Vs (and backslashes) in it.
    // Accept backslash like CTRL-V when 'cpoptions' does not contain 'B'.
    // with :unmap white space is included in the keys, no argument possible.
    p = চাবিগুলি;
    do_backslash = (vim_strchr(p_cpo, CPO_BSLASH) == ফাঁকা);
    যখন (*p && (maptype == MAPTYPE_UNMAP || !VIM_ISWHITE(*p)))
    {
	যদি ((p[0] == Ctrl_V || (do_backslash && p[0] == '\\')) &&
								  p[1] != NUL)
	    ++p;		// skip CTRL-V or backslash
	++p;
    }
    যদি (*p != NUL)
	*p++ = NUL;

    p = skipwhite(p);
    rhs = p;
    hasarg = (*rhs != NUL);
    haskey = (*চাবিগুলি != NUL);
    do_print = !haskey || (maptype != MAPTYPE_UNMAP && !hasarg);

    // check for :unmap without argument
    যদি (maptype == MAPTYPE_UNMAP && !haskey)
    {
	retval = 1;
	যাও theend;
    }

    // If mapping has been given as ^V<C_UP> say, then replace the term codes
    // with the appropriate two bytes. If it is a shifted special key, unshift
    // it too, giving another two bytes.
    // replace_termcodes() may move the result to allocated memory, which
    // needs to be freed later (*keys_buf and *arg_buf).
    // replace_termcodes() also removes CTRL-Vs and sometimes backslashes.
    // If something like <C-H> is simplified to 0x08 then mark it as simplified
    // and also add an entry with a modifier, which will work when using a key
    // protocol.
    যদি (haskey)
    {
	char_u	*new_চাবিগুলি;
	পূর্ণসংখ্যা	flags = REPTERM_FROM_PART | REPTERM_DO_LT;

	যদি (special)
	    flags |= REPTERM_SPECIAL;
	new_চাবিগুলি = replace_termcodes(চাবিগুলি, &চাবিগুলি_buf, 0, flags, &did_simplify);
	যদি (did_simplify)
	    (শূন্য)replace_termcodes(চাবিগুলি, &alt_চাবিগুলি_buf, 0,
					    flags | REPTERM_NO_SIMPLIFY, ফাঁকা);
	চাবিগুলি = new_চাবিগুলি;
    }
    orig_rhs = rhs;
    যদি (hasarg)
    {
	যদি (STRICMP(rhs, "<nop>") == 0)	    // "<Nop>" means nothing
	    rhs = (char_u *)"";
	অন্যথা
	    rhs = replace_termcodes(rhs, &arg_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), ফাঁকা);
    }

    /*
     * The following is done twice if we have two versions of keys:
     * "alt_keys_buf" is not NULL.
     */
    জন্য (keyround = 1; keyround <= 2; ++keyround)
    {
	পূর্ণসংখ্যা	did_it = FALSE;
	পূর্ণসংখ্যা	did_local = FALSE;
	পূর্ণসংখ্যা	keyround1_simplified = keyround == 1 && did_simplify;
	পূর্ণসংখ্যা	round;
	পূর্ণসংখ্যা	অঙ্ক_rounds;

	যদি (keyround == 2)
	{
	    যদি (alt_চাবিগুলি_buf == ফাঁকা)
		বিরতি;
	    চাবিগুলি = alt_চাবিগুলি_buf;
	}
	অন্যথা যদি (alt_চাবিগুলি_buf != ফাঁকা && do_print)
	    // when printing always use the not-simplified map
	    চাবিগুলি = alt_চাবিগুলি_buf;

	// check arguments and translate function keys
	যদি (haskey)
	{
	    len = (পূর্ণসংখ্যা)STRLEN(চাবিগুলি);
	    যদি (len > MAXMAPLEN)	// maximum length of MAXMAPLEN chars
	    {
		retval = 1;
		যাও theend;
	    }

	    যদি (abbrev && maptype != MAPTYPE_UNMAP)
	    {
		// If an abbreviation ends in a keyword character, the
		// rest must be all keyword-char or all non-keyword-char.
		// Otherwise we won't be able to find the start of it in a
		// vi-compatible way.
		যদি (has_mbyte)
		{
		    পূর্ণসংখ্যা		first, last;
		    পূর্ণসংখ্যা		same = -1;
		    char_u	চাবিগুলি_unescaped[MAXMAPLEN + 1];
		    size_t	চাবিগুলি_unescaped_len;

		    mch_memmove(চাবিগুলি_unescaped, চাবিগুলি, (size_t)(len + 1));
		    চাবিগুলি_unescaped_len = vim_unescape_csi(চাবিগুলি_unescaped);
		    p = চাবিগুলি_unescaped;

		    first = vim_iswordp(p);
		    last = first;
		    MB_PTR_ADV(p);
		    n = 1;
		    যখন (p < চাবিগুলি_unescaped + চাবিগুলি_unescaped_len)
		    {
			++n;			// nr of (multi-byte) chars
			last = vim_iswordp(p);	// type of last char
			যদি (same == -1 && last != first)
			    same = n - 1;	// count of same char type
			MB_PTR_ADV(p);
		    }
		    যদি (last && n > 2 && same >= 0 && same < n - 1)
		    {
			retval = 1;
			যাও theend;
		    }
		}
		অন্যথা যদি (vim_iswordc(চাবিগুলি[len - 1]))
		    // ends in keyword char
		    জন্য (n = 0; n < len - 2; ++n)
			যদি (vim_iswordc(চাবিগুলি[n]) != vim_iswordc(চাবিগুলি[len - 2]))
			{
			    retval = 1;
			    যাও theend;
			}
		// An abbreviation cannot contain white space.
		জন্য (n = 0; n < len; ++n)
		    যদি (VIM_ISWHITE(চাবিগুলি[n]))
		    {
			retval = 1;
			যাও theend;
		    }
	    }
	}

	যদি (haskey && hasarg && abbrev)	// if we will add an abbreviation
	    no_abbr = FALSE;		// reset flag that indicates there are
					// no abbreviations

	যদি (do_print)
	    বার্তা_start();

	// Check if a new local mapping wasn't already defined globally.
	যদি (unique && map_table == curbuf->b_maphash
			       && haskey && hasarg && maptype != MAPTYPE_UNMAP)
	{
	    // need to loop over all global hash lists
	    জন্য (পূর্ণসংখ্যা hash = 0; hash < 256 && !got_int; ++hash)
	    {
		যদি (abbrev)
		{
		    যদি (hash != 0)	// there is only one abbreviation list
			বিরতি;
		    mp = first_abbr;
		}
		অন্যথা
		    mp = maphash[hash];
		জন্য ( ; mp != ফাঁকা && !got_int; mp = mp->m_পরবর্তী)
		{
		    // check entries with the same mode
		    যদি ((mp->m_mode & mode) != 0
			    && mp->m_keylen == len
			    && STRNCMP(mp->m_চাবিগুলি, চাবিগুলি, (size_t)len) == 0)
		    {
			যদি (abbrev)
			    semsg(
			       _(e_global_abbreviation_already_exists_for_str),
				    mp->m_চাবিগুলি);
			অন্যথা
			    semsg(_(e_global_mapping_already_exists_for_str),
				    mp->m_চাবিগুলি);
			retval = 5;
			যাও theend;
		    }
		}
	    }
	}

	// When listing global mappings, also list buffer-local ones here.
	যদি (map_table != curbuf->b_maphash && !hasarg
						   && maptype != MAPTYPE_UNMAP)
	    list_mappings(keyround, abbrev, haskey, চাবিগুলি, len,
							     mode, &did_local);

	// Find an entry in the maphash[] list that matches.
	// For :unmap we may loop two times: once to try to unmap an entry with
	// a matching 'from' part, a second time, if the first fails, to unmap
	// an entry with a matching 'to' part. This was done to allow
	// ":ab foo bar" to be unmapped by typing ":unab foo", where "foo" will
	// be replaced by "bar" because of the abbreviation.
	অঙ্ক_rounds = maptype == MAPTYPE_UNMAP && !unmap_lhs_only ? 2 : 1;
	জন্য (round = 0; round < অঙ্ক_rounds && !did_it && !got_int; ++round)
	{
	    // need to loop over all hash lists
	    জন্য (পূর্ণসংখ্যা hash = 0; hash < 256 && !got_int; ++hash)
	    {
		যদি (abbrev)
		{
		    যদি (hash > 0)	// there is only one abbreviation list
			বিরতি;
		    mpp = abbr_table;
		}
		অন্যথা
		    mpp = &(map_table[hash]);
		জন্য (mp = *mpp; mp != ফাঁকা && !got_int; mp = *mpp)
		{

		    যদি ((mp->m_mode & mode) == 0)
		    {
			// skip entries with wrong mode
			mpp = &(mp->m_পরবর্তী);
			চালু;
		    }
		    যদি (!haskey)	// show all entries
		    {
			যদি (!mp->m_simplified)
			{
			    showmap(mp, map_table != maphash);
			    did_it = TRUE;
			}
		    }
		    অন্যথা	// do we have a match?
		    {
			যদি (round)	// second round: Try unmap "rhs" string
			{
			    n = (পূর্ণসংখ্যা)STRLEN(mp->m_str);
			    p = mp->m_str;
			}
			অন্যথা
			{
			    n = mp->m_keylen;
			    p = mp->m_চাবিগুলি;
			}
			যদি (STRNCMP(p, চাবিগুলি, (size_t)(n < len ? n : len)) == 0)
			{
			    যদি (maptype == MAPTYPE_UNMAP)
			    {
				// Delete entry.
				// Only accept a full match.  For abbreviations
				// we ignore trailing space when matching with
				// the "lhs", since an abbreviation can't have
				// trailing space.
				যদি (n != len && (!abbrev || round || n > len
					       || *skipwhite(চাবিগুলি + n) != NUL))
				{
				    mpp = &(mp->m_পরবর্তী);
				    চালু;
				}
				// In keyround for simplified keys, don't unmap
				// a mapping without m_simplified flag.
				যদি (keyround1_simplified && !mp->m_simplified)
				    বিরতি;
				// We reset the indicated mode bits. If nothing
				// is left the entry is deleted below.
				mp->m_mode &= ~mode;
				did_it = TRUE;	// remember we did something
			    }
			    অন্যথা যদি (!hasarg)	// show matching entry
			    {
				যদি (!mp->m_simplified)
				{
				    showmap(mp, map_table != maphash);
				    did_it = TRUE;
				}
			    }
			    অন্যথা যদি (n != len)	// new entry is ambiguous
			    {
				mpp = &(mp->m_পরবর্তী);
				চালু;
			    }
			    অন্যথা যদি (unique)
			    {
				যদি (abbrev)
				    semsg(
				      _(e_abbreviation_already_exists_for_str),
					    p);
				অন্যথা
				    semsg(_(e_mapping_already_exists_for_str),
					    p);
				retval = 5;
				যাও theend;
			    }
			    অন্যথা
			    {
				// new rhs for existing entry
				mp->m_mode &= ~mode;	// remove mode bits
				যদি (mp->m_mode == 0 && !did_it) // reuse entry
				{
				    char_u *newstr = vim_strsave(rhs);

				    যদি (newstr == ফাঁকা)
				    {
					retval = 4;		// no mem
					যাও theend;
				    }
				    যদি (mp->m_alt != ফাঁকা)
					mp->m_alt = mp->m_alt->m_alt = ফাঁকা;
				    vim_free(mp->m_str);
				    mp->m_str = newstr;
				    vim_free(mp->m_orig_str);
				    mp->m_orig_str = vim_strsave(orig_rhs);
				    mp->m_noremap = noremap;
				    mp->m_nowait = nowait;
				    mp->m_silent = silent;
				    mp->m_mode = mode;
				    mp->m_simplified = keyround1_simplified;
#ifdef FEAT_EVAL
				    mp->m_রাশিমালা = রাশিমালা;
				    mp->m_script_ctx = current_sctx;
				    যদি (cmdmod.cmod_flags & CMOD_LEGACY)
					mp->m_script_ctx.sc_version = 1;
				    mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
#endif
				    mp_ফলাফল[keyround - 1] = mp;
				    did_it = TRUE;
				}
			    }
			    যদি (mp->m_mode == 0)  // entry can be deleted
			    {
				map_free(mpp);
				চালু;	// continue with *mpp
			    }

			    // May need to put this entry into another hash
			    // list.
			    পূর্ণসংখ্যা new_hash = MAP_HASH(mp->m_mode, mp->m_চাবিগুলি[0]);
			    যদি (!abbrev && new_hash != hash)
			    {
				*mpp = mp->m_পরবর্তী;
				mp->m_পরবর্তী = map_table[new_hash];
				map_table[new_hash] = mp;

				চালু;	// continue with *mpp
			    }
			}
		    }
		    mpp = &(mp->m_পরবর্তী);
		}
	    }
	}

	যদি (maptype == MAPTYPE_UNMAP)
	{
	    // delete entry
	    যদি (!did_it)
	    {
		যদি (!keyround1_simplified)
		    retval = 2;		// no match
	    }
	    অন্যথা যদি (*চাবিগুলি == Ctrl_C)
	    {
		// If CTRL-C has been unmapped, reuse it for Interrupting.
		যদি (map_table == curbuf->b_maphash)
		    curbuf->b_mapped_ctrl_c &= ~mode;
		অন্যথা
		    mapped_ctrl_c &= ~mode;
	    }
	    চালু;
	}

	যদি (!haskey || !hasarg)
	{
	    // print entries
	    যদি (!did_it && !did_local)
	    {
		যদি (abbrev)
		    বার্তা(_("No abbreviation found"));
		অন্যথা
		    বার্তা(_("No mapping found"));
	    }
	    যাও theend;    // listing finished
	}

	যদি (did_it)
	    চালু;	// have added the new entry already

	// Get here when adding a new entry to the maphash[] list or abbrlist.
	mp_ফলাফল[keyround - 1] = map_add(map_table, abbr_table, চাবিগুলি,
		    rhs, orig_rhs, noremap, nowait, silent, mode, abbrev,
#ifdef FEAT_EVAL
		    রাশিমালা, /* sid */ 0, /* scriptversion */ 0, /* lnum */ 0,
#endif
		    keyround1_simplified);
	যদি (mp_ফলাফল[keyround - 1] == ফাঁকা)
	{
	    retval = 4;	    // no mem
	    যাও theend;
	}
    }

    যদি (mp_ফলাফল[0] != ফাঁকা && mp_ফলাফল[1] != ফাঁকা)
    {
	mp_ফলাফল[0]->m_alt = mp_ফলাফল[1];
	mp_ফলাফল[1]->m_alt = mp_ফলাফল[0];
    }

theend:
    vim_free(চাবিগুলি_buf);
    vim_free(alt_চাবিগুলি_buf);
    vim_free(arg_buf);
    ফেরত retval;
}

/*
 * Get the mapping mode from the command name.
 */
    স্থির পূর্ণসংখ্যা
get_map_mode(char_u **cmdp, পূর্ণসংখ্যা forceit)
{
    char_u	*p;
    পূর্ণসংখ্যা		modec;
    পূর্ণসংখ্যা		mode;

    p = *cmdp;
    modec = *p++;
    যদি (modec == 'i')
	mode = MODE_ঢোকাও;				// :imap
    অন্যথা যদি (modec == 'l')
	mode = MODE_LANGMAP;				// :lmap
    অন্যথা যদি (modec == 'c')
	mode = MODE_CMDLINE;				// :cmap
    অন্যথা যদি (modec == 'n' && *p != 'o')		    // avoid :noremap
	mode = MODE_NORMAL;				// :nmap
    অন্যথা যদি (modec == 'v')
	mode = MODE_VISUAL | MODE_SELECT;		// :vmap
    অন্যথা যদি (modec == 'x')
	mode = MODE_VISUAL;				// :xmap
    অন্যথা যদি (modec == 's')
	mode = MODE_SELECT;			// :smap
    অন্যথা যদি (modec == 'o')
	mode = MODE_OP_PENDING;			// :omap
    অন্যথা যদি (modec == 't')
	mode = MODE_TERMINAL;			// :tmap
    অন্যথা
    {
	--p;
	যদি (forceit)
	    mode = MODE_ঢোকাও | MODE_CMDLINE;		// :map !
	অন্যথা
	    mode = MODE_VISUAL | MODE_SELECT | MODE_NORMAL | MODE_OP_PENDING;
							// :map
    }

    *cmdp = p;
    ফেরত mode;
}

/*
 * Clear all mappings (":mapclear") or abbreviations (":abclear").
 * "abbr" should be FALSE for mappings, TRUE for abbreviations.
 */
    স্থির শূন্য
map_clear(
    char_u	*cmdp,
    char_u	*arg,
    পূর্ণসংখ্যা		forceit,
    পূর্ণসংখ্যা		abbr)
{
    পূর্ণসংখ্যা		mode;
    পূর্ণসংখ্যা		local;

    local = (STRCMP(arg, "<buffer>") == 0);
    যদি (!local && *arg != NUL)
    {
	emsg(_(e_invalid_argument));
	ফেরত;
    }

    mode = get_map_mode(&cmdp, forceit);
    map_clear_mode(curbuf, mode, local, abbr);
}

/*
 * If "map_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    স্থির পূর্ণসংখ্যা
is_map_locked(শূন্য)
{
    যদি (map_locked > 0)
    {
	emsg(_(e_cannot_change_mappings_while_listing));
	ফেরত TRUE;
    }
    ফেরত FALSE;
}

/*
 * Clear all mappings in "mode".
 */
    শূন্য
map_clear_mode(
    buf_T	*buf,		// buffer for local mappings
    পূর্ণসংখ্যা		mode,		// mode in which to delete
    পূর্ণসংখ্যা		local,		// TRUE for buffer-local mappings
    পূর্ণসংখ্যা		abbr)		// TRUE for abbreviations
{
    mapblock_T	*mp, **mpp;
    পূর্ণসংখ্যা		hash;
    পূর্ণসংখ্যা		new_hash;

    যদি (is_map_locked())
	ফেরত;

    validate_maphash();

    জন্য (hash = 0; hash < 256; ++hash)
    {
	যদি (abbr)
	{
	    যদি (hash > 0)	// there is only one abbrlist
		বিরতি;
	    যদি (local)
		mpp = &buf->b_first_abbr;
	    অন্যথা
		mpp = &first_abbr;
	}
	অন্যথা
	{
	    যদি (local)
		mpp = &buf->b_maphash[hash];
	    অন্যথা
		mpp = &maphash[hash];
	}
	যখন (*mpp != ফাঁকা)
	{
	    mp = *mpp;
	    যদি (mp->m_mode & mode)
	    {
		mp->m_mode &= ~mode;
		যদি (mp->m_mode == 0) // entry can be deleted
		{
		    map_free(mpp);
		    চালু;
		}
		// May need to put this entry into another hash list.
		new_hash = MAP_HASH(mp->m_mode, mp->m_চাবিগুলি[0]);
		যদি (!abbr && new_hash != hash)
		{
		    *mpp = mp->m_পরবর্তী;
		    যদি (local)
		    {
			mp->m_পরবর্তী = buf->b_maphash[new_hash];
			buf->b_maphash[new_hash] = mp;
		    }
		    অন্যথা
		    {
			mp->m_পরবর্তী = maphash[new_hash];
			maphash[new_hash] = mp;
		    }
		    চালু;		// continue with *mpp
		}
	    }
	    mpp = &(mp->m_পরবর্তী);
	}
    }
}

#যদি defined(FEAT_EVAL)
    পূর্ণসংখ্যা
mode_str2flags(char_u *modechars)
{
    পূর্ণসংখ্যা		mode = 0;

    যদি (vim_strchr(modechars, 'n') != ফাঁকা)
	mode |= MODE_NORMAL;
    যদি (vim_strchr(modechars, 'v') != ফাঁকা)
	mode |= MODE_VISUAL | MODE_SELECT;
    যদি (vim_strchr(modechars, 'x') != ফাঁকা)
	mode |= MODE_VISUAL;
    যদি (vim_strchr(modechars, 's') != ফাঁকা)
	mode |= MODE_SELECT;
    যদি (vim_strchr(modechars, 'o') != ফাঁকা)
	mode |= MODE_OP_PENDING;
    যদি (vim_strchr(modechars, 'i') != ফাঁকা)
	mode |= MODE_ঢোকাও;
    যদি (vim_strchr(modechars, 'l') != ফাঁকা)
	mode |= MODE_LANGMAP;
    যদি (vim_strchr(modechars, 'c') != ফাঁকা)
	mode |= MODE_CMDLINE;

    ফেরত mode;
}

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "modechars".
 * Recognize termcap codes in "str".
 * Also checks mappings local to the current buffer.
 */
    পূর্ণসংখ্যা
map_to_exists(char_u *str, char_u *modechars, পূর্ণসংখ্যা abbr)
{
    char_u	*rhs;
    char_u	*buf;
    পূর্ণসংখ্যা		retval;

    rhs = replace_termcodes(str, &buf, 0, REPTERM_DO_LT, ফাঁকা);

    retval = map_to_exists_mode(rhs, mode_str2flags(modechars), abbr);
    vim_free(buf);

    ফেরত retval;
}
#endif

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "mode".
 * Also checks mappings local to the current buffer.
 */
    পূর্ণসংখ্যা
map_to_exists_mode(char_u *rhs, পূর্ণসংখ্যা mode, পূর্ণসংখ্যা abbr)
{
    mapblock_T	*mp;
    পূর্ণসংখ্যা		hash;
    পূর্ণসংখ্যা		exp_বাফার = FALSE;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    জন্য (;;)
    {
	জন্য (hash = 0; hash < 256; ++hash)
	{
	    যদি (abbr)
	    {
		যদি (hash > 0)		// there is only one abbr list
		    বিরতি;
		যদি (exp_বাফার)
		    mp = curbuf->b_first_abbr;
		অন্যথা
		    mp = first_abbr;
	    }
	    অন্যথা যদি (exp_বাফার)
		mp = curbuf->b_maphash[hash];
	    অন্যথা
		mp = maphash[hash];
	    জন্য (; mp; mp = mp->m_পরবর্তী)
	    {
		যদি ((mp->m_mode & mode)
			&& strstr((অক্ষর *)mp->m_str, (অক্ষর *)rhs) != ফাঁকা)
		    ফেরত TRUE;
	    }
	}
	যদি (exp_বাফার)
	    বিরতি;
	exp_বাফার = TRUE;
    }

    ফেরত FALSE;
}

/*
 * Used below when expanding mapping/abbreviation names.
 */
স্থির পূর্ণসংখ্যা	expand_mapmodes = 0;
স্থির পূর্ণসংখ্যা	expand_isabbrev = 0;
স্থির পূর্ণসংখ্যা	expand_বাফার = FALSE;

/*
 * Translate an internal mapping/abbreviation representation into the
 * corresponding external one recognized by :map/:abbrev commands.
 * Respects the current B/k/< settings of 'cpoption'.
 *
 * This function is called when expanding mappings/abbreviations on the
 * command-line.
 *
 * It uses a growarray to build the translation string since the latter can be
 * wider than the original description. The caller has to free the string
 * afterwards.
 *
 * Returns NULL when there is a problem.
 */
    স্থির char_u *
translate_mapping(char_u *str)
{
    garray_T	ga;
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		modifiers;
    পূর্ণসংখ্যা		cpo_bslash;
    পূর্ণসংখ্যা		cpo_special;

    ga_init(&ga);
    ga.ga_itemsize = 1;
    ga.ga_growsize = 40;

    cpo_bslash = (vim_strchr(p_cpo, CPO_BSLASH) != ফাঁকা);
    cpo_special = (vim_strchr(p_cpo, CPO_SPECI) != ফাঁকা);

    জন্য (; *str; ++str)
    {
	c = *str;
	যদি (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	{
	    modifiers = 0;
	    যদি (str[1] == KS_MODIFIER)
	    {
		str++;
		modifiers = *++str;
		c = *++str;
	    }
	    যদি (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	    {
		যদি (cpo_special)
		{
		    ga_clear(&ga);
		    ফেরত ফাঁকা;
		}
		c = TO_SPECIAL(str[1], str[2]);
		যদি (c == K_ZERO)	// display <Nul> as ^@
		    c = NUL;
		str += 2;
	    }
	    যদি (IS_SPECIAL(c) || modifiers)	// special key
	    {
		যদি (cpo_special)
		{
		    ga_clear(&ga);
		    ফেরত ফাঁকা;
		}
		ga_concat(&ga, get_special_চাবি_name(c, modifiers));
		চালু; // for (str)
	    }
	}
	যদি (c == ' ' || c == '\t' || c == Ctrl_J || c == Ctrl_V
	    || (c == '<' && !cpo_special) || (c == '\\' && !cpo_bslash))
	    ga_append(&ga, cpo_bslash ? Ctrl_V : '\\');
	যদি (c)
	    ga_append(&ga, c);
    }
    ga_append(&ga, NUL);
    ফেরত (char_u *)(ga.ga_তথ্য);
}

/*
 * Work out what to complete when doing command line completion of mapping
 * or abbreviation names.
 */
    char_u *
set_context_in_map_cmd(
    expand_T	*xp,
    char_u	*cmd,
    char_u	*arg,
    পূর্ণসংখ্যা		forceit,	// TRUE if '!' given
    পূর্ণসংখ্যা		isabbrev,	// TRUE if abbreviation
    পূর্ণসংখ্যা		isunmap,	// TRUE if unmap/unabbrev command
    cmdidx_T	cmdidx)
{
    যদি (forceit && cmdidx != CMD_map && cmdidx != CMD_unmap)
	xp->xp_context = EXPAND_NOTHING;
    অন্যথা
    {
	যদি (isunmap)
	    expand_mapmodes = get_map_mode(&cmd, forceit || isabbrev);
	অন্যথা
	{
	    expand_mapmodes = MODE_ঢোকাও | MODE_CMDLINE;
	    যদি (!isabbrev)
		expand_mapmodes += MODE_VISUAL | MODE_SELECT | MODE_NORMAL
							     | MODE_OP_PENDING;
	}
	expand_isabbrev = isabbrev;
	xp->xp_context = EXPAND_MAPPINGS;
	expand_বাফার = FALSE;
	জন্য (;;)
	{
	    যদি (STRNCMP(arg, "<buffer>", 8) == 0)
	    {
		expand_বাফার = TRUE;
		arg = skipwhite(arg + 8);
		চালু;
	    }
	    যদি (STRNCMP(arg, "<unique>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		চালু;
	    }
	    যদি (STRNCMP(arg, "<nowait>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		চালু;
	    }
	    যদি (STRNCMP(arg, "<silent>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		চালু;
	    }
	    যদি (STRNCMP(arg, "<special>", 9) == 0)
	    {
		arg = skipwhite(arg + 9);
		চালু;
	    }
#ifdef FEAT_EVAL
	    যদি (STRNCMP(arg, "<script>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		চালু;
	    }
	    যদি (STRNCMP(arg, "<expr>", 6) == 0)
	    {
		arg = skipwhite(arg + 6);
		চালু;
	    }
#endif
	    বিরতি;
	}
	xp->xp_pattern = arg;
    }

    ফেরত ফাঁকা;
}

/*
 * Find all mapping/abbreviation names that match regexp "regmatch"'.
 * For command line expansion of ":[un]map" and ":[un]abbrev" in all modes.
 * Return OK if matches found, FAIL otherwise.
 */
    পূর্ণসংখ্যা
ExpandMappings(
    char_u	*pat,
    regmatch_T	*regmatch,
    পূর্ণসংখ্যা		*অঙ্ক_Matches,
    char_u	***matches)
{
    mapblock_T	*mp;
    garray_T	ga;
    পূর্ণসংখ্যা		hash;
    পূর্ণসংখ্যা		গুনতি;
    char_u	*p;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		fuzzy;
    পূর্ণসংখ্যা		match;
    পূর্ণসংখ্যা		score = 0;
    fuzmatch_str_T  *fuzmatch;

    fuzzy = cmdline_fuzzy_complete(pat);

    validate_maphash();

    *অঙ্ক_Matches = 0;		    // return values in case of FAIL
    *matches = ফাঁকা;

    যদি (!fuzzy)
	ga_init2(&ga, আকার(অক্ষর *), 3);
    অন্যথা
	ga_init2(&ga, আকার(fuzmatch_str_T), 3);

    // First search in map modifier arguments
    জন্য (i = 0; i < 7; ++i)
    {
	যদি (i == 0)
	    p = (char_u *)"<silent>";
	অন্যথা যদি (i == 1)
	    p = (char_u *)"<unique>";
#ifdef FEAT_EVAL
	অন্যথা যদি (i == 2)
	    p = (char_u *)"<script>";
	অন্যথা যদি (i == 3)
	    p = (char_u *)"<expr>";
#endif
	অন্যথা যদি (i == 4 && !expand_বাফার)
	    p = (char_u *)"<buffer>";
	অন্যথা যদি (i == 5)
	    p = (char_u *)"<nowait>";
	অন্যথা যদি (i == 6)
	    p = (char_u *)"<special>";
	অন্যথা
	    চালু;

	যদি (!fuzzy)
	    match = vim_regexec(regmatch, p, (colnr_T)0);
	অন্যথা
	{
	    score = fuzzy_match_str(p, pat);
	    match = (score != FUZZY_SCORE_NONE);
	}

	যদি (!match)
	    চালু;

	যদি (ga_grow(&ga, 1) == FAIL)
	    বিরতি;

	যদি (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_তথ্য)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = vim_strsave(p);
	    fuzmatch->score = score;
	}
	অন্যথা
	    ((char_u **)ga.ga_তথ্য)[ga.ga_len] = vim_strsave(p);
	++ga.ga_len;
    }

    জন্য (hash = 0; hash < 256; ++hash)
    {
	যদি (expand_isabbrev)
	{
	    যদি (hash > 0)	// only one abbrev list
		বিরতি; // for (hash)
	    mp = first_abbr;
	}
	অন্যথা যদি (expand_বাফার)
	    mp = curbuf->b_maphash[hash];
	অন্যথা
	    mp = maphash[hash];
	জন্য (; mp; mp = mp->m_পরবর্তী)
	{
	    যদি (mp->m_simplified || !(mp->m_mode & expand_mapmodes))
		চালু;

	    p = translate_mapping(mp->m_চাবিগুলি);
	    যদি (p == ফাঁকা)
		চালু;

	    যদি (!fuzzy)
		match = vim_regexec(regmatch, p, (colnr_T)0);
	    অন্যথা
	    {
		score = fuzzy_match_str(p, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }

	    যদি (!match)
	    {
		vim_free(p);
		চালু;
	    }

	    যদি (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		বিরতি;
	    }

	    যদি (fuzzy)
	    {
		fuzmatch = &((fuzmatch_str_T *)ga.ga_তথ্য)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    অন্যথা
		((char_u **)ga.ga_তথ্য)[ga.ga_len] = p;

	    ++ga.ga_len;
	} // for (mp)
    } // for (hash)

    যদি (ga.ga_len == 0)
	ফেরত FAIL;

    যদি (!fuzzy)
    {
	*matches = ga.ga_তথ্য;
	*অঙ্ক_Matches = ga.ga_len;
    }
    অন্যথা
    {
	যদি (fuzzymatches_to_strmatches(ga.ga_তথ্য, matches, ga.ga_len,
							FALSE) == FAIL)
	    ফেরত FAIL;
	*অঙ্ক_Matches = ga.ga_len;
    }

    গুনতি = *অঙ্ক_Matches;
    যদি (গুনতি > 1)
    {
	char_u	**ptr1;
	char_u	**ptr2;
	char_u	**ptr3;

	// Sort the matches
	// Fuzzy matching already sorts the matches
	যদি (!fuzzy)
	    sort_strings(*matches, গুনতি);

	// Remove multiple entries
	ptr1 = *matches;
	ptr2 = ptr1 + 1;
	ptr3 = ptr1 + গুনতি;

	যখন (ptr2 < ptr3)
	{
	    যদি (STRCMP(*ptr1, *ptr2) != 0)
		*++ptr1 = *ptr2++;
	    অন্যথা
	    {
		vim_free(*ptr2++);
		গুনতি--;
	    }
	}
    }

    *অঙ্ক_Matches = গুনতি;
    ফেরত (গুনতি == 0 ? FAIL : OK);
}

/*
 * Check for an abbreviation.
 * Cursor is at ptr[col].
 * When inserting, mincol is where insert started.
 * For the command line, mincol is what is to be skipped over.
 * "c" is the character typed before check_abbr was called.  It may have
 * ABBR_OFF added to avoid prepending a CTRL-V to it.
 *
 * Historic vi practice: The last character of an abbreviation must be an id
 * character ([a-zA-Z0-9_]). The characters in front of it must be all id
 * characters or all non-id characters. This allows for abbr. "#i" to
 * "#include".
 *
 * Vim addition: Allow for abbreviations that end in a non-keyword character.
 * Then there must be white space before the abbr.
 *
 * return TRUE if there is an abbreviation, FALSE if not
 */
    পূর্ণসংখ্যা
check_abbr(
    পূর্ণসংখ্যা		c,
    char_u	*ptr,
    পূর্ণসংখ্যা		col,
    পূর্ণসংখ্যা		mincol)
{
    পূর্ণসংখ্যা		len;
    পূর্ণসংখ্যা		scol;		// starting column of the abbr.
    পূর্ণসংখ্যা		j;
    char_u	*s;
    char_u	tb[MB_MAXBYTES + 4];
    mapblock_T	*mp;
    mapblock_T	*mp2;
    পূর্ণসংখ্যা		clen = 0;	// length in characters
    পূর্ণসংখ্যা		is_id = TRUE;
    পূর্ণসংখ্যা		vim_abbr;

    যদি (typebuf.tb_no_abbr_cnt)	// abbrev. are not recursive
	ফেরত FALSE;

    // no remapping implies no abbreviation, except for CTRL-]
    যদি (noremap_চাবিগুলি() && c != Ctrl_RSB)
	ফেরত FALSE;

    // Check for word before the cursor: If it ends in a keyword char all
    // chars before it must be keyword chars or non-keyword chars, but not
    // white space. If it ends in a non-keyword char we accept any characters
    // before it except white space.
    যদি (col == 0)				// cannot be an abbr.
	ফেরত FALSE;

    যদি (has_mbyte)
    {
	char_u *p;

	p = mb_prevptr(ptr, ptr + col);
	যদি (!vim_iswordp(p))
	    vim_abbr = TRUE;			// Vim added abbr.
	অন্যথা
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    যদি (p > ptr)
		is_id = vim_iswordp(mb_prevptr(ptr, p));
	}
	clen = 1;
	যখন (p > ptr + mincol)
	{
	    p = mb_prevptr(ptr, p);
	    যদি (vim_isspace(*p) || (!vim_abbr && is_id != vim_iswordp(p)))
	    {
		p += (*mb_ptr2len)(p);
		বিরতি;
	    }
	    ++clen;
	}
	scol = (পূর্ণসংখ্যা)(p - ptr);
    }
    অন্যথা
    {
	যদি (!vim_iswordc(ptr[col - 1]))
	    vim_abbr = TRUE;			// Vim added abbr.
	অন্যথা
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    যদি (col > 1)
		is_id = vim_iswordc(ptr[col - 2]);
	}
	জন্য (scol = col - 1; scol > 0 && !vim_isspace(ptr[scol - 1])
		&& (vim_abbr || is_id == vim_iswordc(ptr[scol - 1])); --scol)
	    ;
    }

    যদি (scol < mincol)
	scol = mincol;
    যদি (scol < col)		// there is a word in front of the cursor
    {
	ptr += scol;
	len = col - scol;
	mp = curbuf->b_first_abbr;
	mp2 = first_abbr;
	যদি (mp == ফাঁকা)
	{
	    mp = mp2;
	    mp2 = ফাঁকা;
	}
	জন্য ( ; mp; mp->m_পরবর্তী == ফাঁকা
				  ? (mp = mp2, mp2 = ফাঁকা) : (mp = mp->m_পরবর্তী))
	{
	    পূর্ণসংখ্যা		qlen = mp->m_keylen;
	    char_u	*q = mp->m_চাবিগুলি;
	    পূর্ণসংখ্যা		match;

	    যদি (vim_strbyte(mp->m_চাবিগুলি, K_SPECIAL) != ফাঁকা)
	    {
		char_u *qe = vim_strsave(mp->m_চাবিগুলি);

		// might have CSI escaped mp->m_keys
		যদি (qe != ফাঁকা)
		{
		    q = qe;
		    qlen = (পূর্ণসংখ্যা)vim_unescape_csi(q);
		}
	    }

	    // find entries with right mode and keys
	    match =    (mp->m_mode & State)
		    && qlen == len
		    && STRNCMP(q, ptr, (size_t)len) == 0;
	    যদি (q != mp->m_চাবিগুলি)
		vim_free(q);
	    যদি (match)
		বিরতি;
	}
	যদি (mp != ফাঁকা)
	{
	    পূর্ণসংখ্যা	noremap;
	    পূর্ণসংখ্যা silent;
#ifdef FEAT_EVAL
	    পূর্ণসংখ্যা রাশিমালা;
#endif

	    // Found a match:
	    // Insert the rest of the abbreviation in typebuf.tb_buf[].
	    // This goes from end to start.
	    //
	    // Characters 0x000 - 0x100: normal chars, may need CTRL-V,
	    // except K_SPECIAL: Becomes K_SPECIAL KS_SPECIAL KE_FILLER
	    // Characters where IS_SPECIAL() == TRUE: key codes, need
	    // K_SPECIAL. Other characters (with ABBR_OFF): don't use CTRL-V.
	    //
	    // Character CTRL-] is treated specially - it completes the
	    // abbreviation, but is not inserted into the input stream.
	    j = 0;
	    যদি (c != Ctrl_RSB)
	    {
					// special key code, split up
		যদি (IS_SPECIAL(c) || c == K_SPECIAL)
		{
		    tb[j++] = K_SPECIAL;
		    tb[j++] = K_SECOND(c);
		    tb[j++] = K_THIRD(c);
		}
		অন্যথা
		{
		    যদি (c < ABBR_OFF && (c < ' ' || c > '~'))
			tb[j++] = Ctrl_V;	// special char needs CTRL-V
		    যদি (has_mbyte)
		    {
			পূর্ণসংখ্যা	newlen;
			char_u	*escaped;

			// if ABBR_OFF has been added, remove it here
			যদি (c >= ABBR_OFF)
			    c -= ABBR_OFF;
			newlen = (*mb_char2bytes)(c, tb + j);
			tb[j + newlen] = NUL;
			// Need to escape K_SPECIAL.
			escaped = vim_strsave_escape_csi(tb + j);
			যদি (escaped != ফাঁকা)
			{
			    newlen = (পূর্ণসংখ্যা)STRLEN(escaped);
			    mch_memmove(tb + j, escaped, newlen);
			    j += newlen;
			    vim_free(escaped);
			}
		    }
		    অন্যথা
			tb[j++] = c;
		}
		tb[j] = NUL;
					// insert the last typed char
		(শূন্য)ins_typebuf(tb, 1, 0, TRUE, mp->m_silent);
	    }

	    // copy values here, calling eval_map_expr() may make "mp" invalid!
	    noremap = mp->m_noremap;
	    silent = mp->m_silent;
#ifdef FEAT_EVAL
	    রাশিমালা = mp->m_রাশিমালা;

	    যদি (রাশিমালা)
		s = eval_map_রাশিমালা(mp, c);
	    অন্যথা
#endif
		s = mp->m_str;
	    যদি (s != ফাঁকা)
	    {
					// insert the to string
		(শূন্য)ins_typebuf(s, noremap, 0, TRUE, silent);
					// no abbrev. for these chars
		typebuf.tb_no_abbr_cnt += (পূর্ণসংখ্যা)STRLEN(s) + j + 1;
#ifdef FEAT_EVAL
		যদি (রাশিমালা)
		    vim_free(s);
#endif
	    }

	    tb[0] = Ctrl_H;
	    tb[1] = NUL;
	    যদি (has_mbyte)
		len = clen;	// Delete characters instead of bytes
	    যখন (len-- > 0)		// delete the from string
		(শূন্য)ins_typebuf(tb, 1, 0, TRUE, silent);
	    ফেরত TRUE;
	}
    }
    ফেরত FALSE;
}

#ifdef FEAT_EVAL
/*
 * Evaluate the RHS of a mapping or abbreviations and take care of escaping
 * special characters.
 * Careful: after this "mp" will be invalid if the mapping was deleted.
 */
    char_u *
eval_map_রাশিমালা(
    mapblock_T	*mp,
    পূর্ণসংখ্যা		c)	    // NUL or typed character for abbreviation
{
    char_u	*res;
    char_u	*p;
    char_u	*রাশিমালা;
    pos_T	save_cursor;
    পূর্ণসংখ্যা		save_বার্তা_col;
    পূর্ণসংখ্যা		save_বার্তা_row;
    scid_T	save_sctx_sid = current_sctx.sc_sid;
    পূর্ণসংখ্যা		save_sctx_version = current_sctx.sc_version;

    // Remove escaping of CSI, because "str" is in a format to be used as
    // typeahead.
    রাশিমালা = vim_strsave(mp->m_str);
    যদি (রাশিমালা == ফাঁকা)
	ফেরত ফাঁকা;
    vim_unescape_csi(রাশিমালা);

    // Forbid changing text or using ":normal" to avoid most of the bad side
    // effects.  Also restore the cursor position.
    ++textlock;
    ++ex_normal_lock;
    set_vim_var_char(c);  // set v:char to the typed character
    save_cursor = curwin->w_cursor;
    save_বার্তা_col = বার্তা_col;
    save_বার্তা_row = বার্তা_row;

    current_sctx.sc_version = mp->m_script_ctx.sc_version;
    যদি (mp->m_script_ctx.sc_version == SCRIPT_VERSION_VIM9)
	current_sctx.sc_sid = mp->m_script_ctx.sc_sid;

    // Note: the evaluation may make "mp" invalid.
    p = eval_to_string(রাশিমালা, FALSE, FALSE);

    --textlock;
    --ex_normal_lock;
    curwin->w_cursor = save_cursor;
    বার্তা_col = save_বার্তা_col;
    বার্তা_row = save_বার্তা_row;
    current_sctx.sc_sid = save_sctx_sid;
    current_sctx.sc_version = save_sctx_version;

    vim_free(রাশিমালা);

    যদি (p == ফাঁকা)
	ফেরত ফাঁকা;
    // Escape CSI in the result to be able to use the string as typeahead.
    res = vim_strsave_escape_csi(p);
    vim_free(p);

    ফেরত res;
}
#endif

/*
 * Copy "p" to allocated memory, escaping K_SPECIAL and CSI so that the result
 * can be put in the typeahead buffer.
 * Returns NULL when out of memory.
 */
    char_u *
vim_strsave_escape_csi(char_u *p)
{
    char_u	*res;
    char_u	*s, *d;

    // Need a buffer to hold up to three times as much.  Four in case of an
    // illegal utf-8 byte:
    // 0xc0 -> 0xc3 0x80 -> 0xc3 K_SPECIAL KS_SPECIAL KE_FILLER
    res = alloc(STRLEN(p) * 4 + 1);
    যদি (res == ফাঁকা)
	ফেরত ফাঁকা;

    d = res;
    জন্য (s = p; *s != NUL; )
    {
	যদি ((s[0] == K_SPECIAL
#ifdef FEAT_GUI
		    || (gui.in_use && s[0] == CSI)
#endif
	    ) && s[1] != NUL && s[2] != NUL)
	{
	    // Copy special key unmodified.
	    *d++ = *s++;
	    *d++ = *s++;
	    *d++ = *s++;
	}
	অন্যথা
	{
	    // Add character, possibly multi-byte to destination, escaping
	    // CSI and K_SPECIAL. Be careful, it can be an illegal byte!
	    d = add_char2buf(PTR2CHAR(s), d);
	    s += MB_CPTR2LEN(s);
	}
    }
    *d = NUL;

    ফেরত res;
}

/*
 * Remove escaping from CSI and K_SPECIAL characters.  Reverse of
 * vim_strsave_escape_csi().  Works in-place.
 * Returns the number of bytes in the unescaped string.
 */
    size_t
vim_unescape_csi(char_u *p)
{
    char_u	*s = p, *d = p;

    যখন (*s != NUL)
    {
	যদি (s[0] == K_SPECIAL && s[1] == KS_SPECIAL && s[2] == KE_FILLER)
	{
	    *d++ = K_SPECIAL;
	    s += 3;
	}
	অন্যথা যদি ((s[0] == K_SPECIAL || s[0] == CSI)
				   && s[1] == KS_EXTRA && s[2] == (পূর্ণসংখ্যা)KE_CSI)
	{
	    *d++ = CSI;
	    s += 3;
	}
	অন্যথা
	    *d++ = *s++;
    }
    *d = NUL;
    ফেরত (size_t)(d - p);
}

/*
 * Write map commands for the current mappings to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    পূর্ণসংখ্যা
makemap(
    FILE	*fd,
    buf_T	*buf)	    // buffer for local mappings or NULL
{
    mapblock_T	*mp;
    char_u	c1, c2, c3;
    char_u	*p;
    অক্ষর	*cmd;
    পূর্ণসংখ্যা		abbr;
    পূর্ণসংখ্যা		hash;
    পূর্ণসংখ্যা		did_cpo = FALSE;
    পূর্ণসংখ্যা		i;

    validate_maphash();

    // Do the loop twice: Once for mappings, once for abbreviations.
    // Then loop over all map hash lists.
    জন্য (abbr = 0; abbr < 2; ++abbr)
	জন্য (hash = 0; hash < 256; ++hash)
	{
	    যদি (abbr)
	    {
		যদি (hash > 0)		// there is only one abbr list
		    বিরতি;
		যদি (buf != ফাঁকা)
		    mp = buf->b_first_abbr;
		অন্যথা
		    mp = first_abbr;
	    }
	    অন্যথা
	    {
		যদি (buf != ফাঁকা)
		    mp = buf->b_maphash[hash];
		অন্যথা
		    mp = maphash[hash];
	    }

	    জন্য ( ; mp; mp = mp->m_পরবর্তী)
	    {
		// simplified map blocks are not created explicitly
		যদি (mp->m_simplified)
		    চালু;

		// skip script-local mappings
		যদি (mp->m_noremap == REMAP_SCRIPT)
		    চালু;

		// skip mappings that contain a <SNR> (script-local thing),
		// they probably don't work when loaded again
		জন্য (p = mp->m_str; *p != NUL; ++p)
		    যদি (p[0] == K_SPECIAL && p[1] == KS_EXTRA
						       && p[2] == (পূর্ণসংখ্যা)KE_SNR)
			বিরতি;
		যদি (*p != NUL)
		    চালু;

		// It's possible to create a mapping and then ":unmap" certain
		// modes.  We recreate this here by mapping the individual
		// modes, which requires up to three of them.
		c1 = NUL;
		c2 = NUL;
		c3 = NUL;
		যদি (abbr)
		    cmd = "abbr";
		অন্যথা
		    cmd = "map";
		নির্বাচন (mp->m_mode)
		{
		    ক্ষেত্র MODE_NORMAL | MODE_VISUAL | MODE_SELECT
							     | MODE_OP_PENDING:
			বিরতি;
		    ক্ষেত্র MODE_NORMAL:
			c1 = 'n';
			বিরতি;
		    ক্ষেত্র MODE_VISUAL:
			c1 = 'x';
			বিরতি;
		    ক্ষেত্র MODE_SELECT:
			c1 = 's';
			বিরতি;
		    ক্ষেত্র MODE_OP_PENDING:
			c1 = 'o';
			বিরতি;
		    ক্ষেত্র MODE_NORMAL | MODE_VISUAL:
			c1 = 'n';
			c2 = 'x';
			বিরতি;
		    ক্ষেত্র MODE_NORMAL | MODE_SELECT:
			c1 = 'n';
			c2 = 's';
			বিরতি;
		    ক্ষেত্র MODE_NORMAL | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 'o';
			বিরতি;
		    ক্ষেত্র MODE_VISUAL | MODE_SELECT:
			c1 = 'v';
			বিরতি;
		    ক্ষেত্র MODE_VISUAL | MODE_OP_PENDING:
			c1 = 'x';
			c2 = 'o';
			বিরতি;
		    ক্ষেত্র MODE_SELECT | MODE_OP_PENDING:
			c1 = 's';
			c2 = 'o';
			বিরতি;
		    ক্ষেত্র MODE_NORMAL | MODE_VISUAL | MODE_SELECT:
			c1 = 'n';
			c2 = 'v';
			বিরতি;
		    ক্ষেত্র MODE_NORMAL | MODE_VISUAL | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 'x';
			c3 = 'o';
			বিরতি;
		    ক্ষেত্র MODE_NORMAL | MODE_SELECT | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 's';
			c3 = 'o';
			বিরতি;
		    ক্ষেত্র MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING:
			c1 = 'v';
			c2 = 'o';
			বিরতি;
		    ক্ষেত্র MODE_CMDLINE | MODE_ঢোকাও:
			যদি (!abbr)
			    cmd = "map!";
			বিরতি;
		    ক্ষেত্র MODE_CMDLINE:
			c1 = 'c';
			বিরতি;
		    ক্ষেত্র MODE_ঢোকাও:
			c1 = 'i';
			বিরতি;
		    ক্ষেত্র MODE_LANGMAP:
			c1 = 'l';
			বিরতি;
		    ক্ষেত্র MODE_TERMINAL:
			c1 = 't';
			বিরতি;
		    পূর্বনির্ধারিত:
			iemsg(e_makemap_illegal_mode);
			ফেরত FAIL;
		}
		করো	// do this twice if c2 is set, 3 times with c3
		{
		    // When outputting <> form, need to make sure that 'cpo'
		    // is set to the Vim default.
		    যদি (!did_cpo)
		    {
			যদি (*mp->m_str == NUL)		// will use <Nop>
			    did_cpo = TRUE;
			অন্যথা
			    জন্য (i = 0; i < 2; ++i)
				জন্য (p = (i ? mp->m_str : mp->m_চাবিগুলি); *p; ++p)
				    যদি (*p == K_SPECIAL || *p == NL)
					did_cpo = TRUE;
			যদি (did_cpo)
			{
			    যদি (fprintf(fd, "cpo_save = &cpo") < 0
				    || put_eol(fd) < 0
				    || fprintf(fd, "set cpo&vim") < 0
				    || put_eol(fd) < 0)
				ফেরত FAIL;
			}
		    }
#ifdef FEAT_EVAL
		    // If it is not vim9 use legacy
		    যদি (mp->m_রাশিমালা && mp->m_script_ctx.sc_version < SCRIPT_VERSION_VIM9
			    && fputs("legacy ", fd) < 0)
			ফেরত FAIL;
#endif
		    যদি (c1 && putc(c1, fd) < 0)
			ফেরত FAIL;
		    যদি (mp->m_noremap != REMAP_YES && fprintf(fd, "nore") < 0)
			ফেরত FAIL;
		    যদি (fputs(cmd, fd) < 0)
			ফেরত FAIL;
		    যদি (buf != ফাঁকা && fputs(" <buffer>", fd) < 0)
			ফেরত FAIL;
		    যদি (mp->m_nowait && fputs(" <nowait>", fd) < 0)
			ফেরত FAIL;
		    যদি (mp->m_silent && fputs(" <silent>", fd) < 0)
			ফেরত FAIL;
#ifdef FEAT_EVAL
		    যদি (mp->m_noremap == REMAP_SCRIPT
						 && fputs("<script>", fd) < 0)
			ফেরত FAIL;
		    যদি (mp->m_রাশিমালা && fputs(" <expr>", fd) < 0)
			ফেরত FAIL;
#endif

		    যদি (       putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_চাবিগুলি, 0) == FAIL
			    || putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_str, 1) == FAIL
			    || put_eol(fd) < 0)
			ফেরত FAIL;
		    c1 = c2;
		    c2 = c3;
		    c3 = NUL;
		} যখন (c1 != NUL);
	    }
	}

    যদি (did_cpo)
	যদি (fprintf(fd, "&cpo = cpo_save") < 0
		|| put_eol(fd) < 0)
	    ফেরত FAIL;
    ফেরত OK;
}

/*
 * write escape string to file
 * "what": 0 for :map lhs, 1 for :map rhs, 2 for :set
 *
 * return FAIL for failure, OK otherwise
 */
    পূর্ণসংখ্যা
put_escstr(FILE *fd, char_u *strstart, পূর্ণসংখ্যা what)
{
    char_u	*str = strstart;
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		modifiers;

    // :map xx <Nop>
    যদি (*str == NUL && what == 1)
    {
	যদি (fprintf(fd, "<Nop>") < 0)
	    ফেরত FAIL;
	ফেরত OK;
    }

    জন্য ( ; *str != NUL; ++str)
    {
	char_u	*p;

	// Check for a multi-byte character, which may contain escaped
	// K_SPECIAL and CSI bytes
	p = mb_unescape(&str);
	যদি (p != ফাঁকা)
	{
	    যখন (*p != NUL)
		যদি (fputc(*p++, fd) < 0)
		    ফেরত FAIL;
	    --str;
	    চালু;
	}

	c = *str;
	// Special key codes have to be translated to be able to make sense
	// when they are read back.
	যদি (c == K_SPECIAL && what != 2)
	{
	    modifiers = 0;
	    যদি (str[1] == KS_MODIFIER)
	    {
		modifiers = str[2];
		str += 3;

		// Modifiers can be applied too to multi-byte characters.
		p = mb_unescape(&str);

		যদি (p == ফাঁকা)
		    c = *str;
		অন্যথা
		{
		    // retrieve codepoint (character number) from unescaped string
		    c = (*mb_ptr2char)(p);
		    --str;
		}
	    }
	    যদি (c == K_SPECIAL)
	    {
		c = TO_SPECIAL(str[1], str[2]);
		str += 2;
	    }
	    যদি (IS_SPECIAL(c) || modifiers)	// special key
	    {
		যদি (fputs((অক্ষর *)get_special_চাবি_name(c, modifiers), fd) < 0)
		    ফেরত FAIL;
		চালু;
	    }
	}

	// A '\n' in a map command should be written as <NL>.
	// A '\n' in a set command should be written as \^V^J.
	যদি (c == NL)
	{
	    যদি (what == 2)
	    {
		যদি (fprintf(fd, "\\\026\n") < 0)
		    ফেরত FAIL;
	    }
	    অন্যথা
	    {
		যদি (fprintf(fd, "<NL>") < 0)
		    ফেরত FAIL;
	    }
	    চালু;
	}

	// Some characters have to be escaped with CTRL-V to
	// prevent them from misinterpreted in DoOneCmd().
	// A space, Tab and '"' has to be escaped with a backslash to
	// prevent it to be misinterpreted in do_set().
	// A space has to be escaped with a CTRL-V when it's at the start of a
	// ":map" rhs.
	// A '<' has to be escaped with a CTRL-V to prevent it being
	// interpreted as the start of a special key name.
	// A space in the lhs of a :map needs a CTRL-V.
	যদি (what == 2 && (VIM_ISWHITE(c) || c == '"' || c == '\\'))
	{
	    যদি (putc('\\', fd) < 0)
		ফেরত FAIL;
	}
	অন্যথা যদি (c < ' ' || c > '~' || c == '|'
		|| (what == 0 && c == ' ')
		|| (what == 1 && str == strstart && c == ' ')
		|| (what != 2 && c == '<'))
	{
	    যদি (putc(Ctrl_V, fd) < 0)
		ফেরত FAIL;
	}
	যদি (putc(c, fd) < 0)
	    ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Check all mappings for the presence of special key codes.
 * Used after ":set term=xxx".
 */
    শূন্য
check_map_keycodes(শূন্য)
{
    mapblock_T	*mp;
    char_u	*p;
    পূর্ণসংখ্যা		i;
    char_u	buf[3];
    পূর্ণসংখ্যা		abbr;
    পূর্ণসংখ্যা		hash;
    buf_T	*bp;
    ESTACK_CHECK_DECLARATION;

    validate_maphash();
    // avoids giving error messages
    estack_ঠেলো(ETYPE_INTERNAL, (char_u *)"mappings", 0);
    ESTACK_CHECK_SETUP;

    // Do this once for each buffer, and then once for global
    // mappings/abbreviations with bp == NULL
    জন্য (bp = firstbuf; ; bp = bp->b_পরবর্তী)
    {
	// Do the loop twice: Once for mappings, once for abbreviations.
	// Then loop over all map hash lists.
	জন্য (abbr = 0; abbr <= 1; ++abbr)
	    জন্য (hash = 0; hash < 256; ++hash)
	    {
		যদি (abbr)
		{
		    যদি (hash)	    // there is only one abbr list
			বিরতি;
		    যদি (bp != ফাঁকা)
			mp = bp->b_first_abbr;
		    অন্যথা
			mp = first_abbr;
		}
		অন্যথা
		{
		    যদি (bp != ফাঁকা)
			mp = bp->b_maphash[hash];
		    অন্যথা
			mp = maphash[hash];
		}
		জন্য ( ; mp != ফাঁকা; mp = mp->m_পরবর্তী)
		{
		    জন্য (i = 0; i <= 1; ++i)	// do this twice
		    {
			যদি (i == 0)
			    p = mp->m_চাবিগুলি;	// once for the "from" part
			অন্যথা
			    p = mp->m_str;	// and once for the "to" part
			যখন (*p)
			{
			    যদি (*p == K_SPECIAL)
			    {
				++p;
				যদি (*p < 128)   // for "normal" tcap entries
				{
				    buf[0] = p[0];
				    buf[1] = p[1];
				    buf[2] = NUL;
				    (শূন্য)add_termcap_entry(buf, FALSE);
				}
				++p;
			    }
			    ++p;
			}
		    }
		}
	    }
	যদি (bp == ফাঁকা)
	    বিরতি;
    }
    ESTACK_CHECK_NOW;
    estack_বের_করো();
}

#যদি defined(FEAT_EVAL)
/*
 * Check the string "keys" against the lhs of all mappings.
 * Return pointer to rhs of mapping (mapblock->m_str).
 * NULL when no mapping found.
 */
    char_u *
check_map(
    char_u	*চাবিগুলি,
    পূর্ণসংখ্যা		mode,
    পূর্ণসংখ্যা		exact,		// require exact match
    পূর্ণসংখ্যা		ign_mod,	// ignore preceding modifier
    পূর্ণসংখ্যা		abbr,		// do abbreviations
    mapblock_T	**mp_ptr,	// return: pointer to mapblock or NULL
    পূর্ণসংখ্যা		*local_ptr)	// return: buffer-local mapping or NULL
{
    পূর্ণসংখ্যা		hash;
    পূর্ণসংখ্যা		len, minlen;
    mapblock_T	*mp;
    char_u	*s;
    পূর্ণসংখ্যা		local;

    validate_maphash();

    len = (পূর্ণসংখ্যা)STRLEN(চাবিগুলি);
    জন্য (local = 1; local >= 0; --local)
	// loop over all hash lists
	জন্য (hash = 0; hash < 256; ++hash)
	{
	    যদি (abbr)
	    {
		যদি (hash > 0)		// there is only one list.
		    বিরতি;
		যদি (local)
		    mp = curbuf->b_first_abbr;
		অন্যথা
		    mp = first_abbr;
	    }
	    অন্যথা যদি (local)
		mp = curbuf->b_maphash[hash];
	    অন্যথা
		mp = maphash[hash];
	    জন্য ( ; mp != ফাঁকা; mp = mp->m_পরবর্তী)
	    {
		// skip entries with wrong mode, wrong length and not matching
		// ones
		যদি ((mp->m_mode & mode) && (!exact || mp->m_keylen == len))
		{
		    যদি (len > mp->m_keylen)
			minlen = mp->m_keylen;
		    অন্যথা
			minlen = len;
		    s = mp->m_চাবিগুলি;
		    যদি (ign_mod && s[0] == K_SPECIAL && s[1] == KS_MODIFIER
							       && s[2] != NUL)
		    {
			s += 3;
			যদি (len > mp->m_keylen - 3)
			    minlen = mp->m_keylen - 3;
		    }
		    যদি (STRNCMP(s, চাবিগুলি, minlen) == 0)
		    {
			যদি (mp_ptr != ফাঁকা)
			    *mp_ptr = mp;
			যদি (local_ptr != ফাঁকা)
			    *local_ptr = local;
			ফেরত mp->m_str;
		    }
		}
	    }
	}

    ফেরত ফাঁকা;
}

/*
 * "hasmapto()" function
 */
    শূন্য
f_hasmapto(typval_T *argvars, typval_T *rettv)
{
    char_u	*name;
    char_u	*mode;
    char_u	buf[NUMBUFLEN];
    পূর্ণসংখ্যা		abbr = FALSE;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ফেরত;

    name = tv_get_string(&argvars[0]);
    যদি (argvars[1].v_type == VAR_UNKNOWN)
	mode = (char_u *)"nvo";
    অন্যথা
    {
	mode = tv_get_string_buf(&argvars[1], buf);
	যদি (argvars[2].v_type != VAR_UNKNOWN)
	    abbr = (পূর্ণসংখ্যা)tv_get_bool(&argvars[2]);
    }

    যদি (map_to_exists(name, mode, abbr))
	rettv->vval.v_সংখ্যা = TRUE;
    অন্যথা
	rettv->vval.v_সংখ্যা = FALSE;
}

/*
 * Fill in the empty dictionary with items as defined by maparg builtin.
 */
    স্থির শূন্য
mapblock2dict(
	mapblock_T  *mp,
	dict_T	    *dict,
	char_u	    *lhsrawalt,	    // may be NULL
	পূর্ণসংখ্যা	    বাফার_local,   // false if not buffer local mapping
	পূর্ণসংখ্যা	    abbr)	    // true if abbreviation
{
    char_u	    *lhs = str2special_save(mp->m_চাবিগুলি, TRUE, FALSE);
    char_u	    *mapmode = map_mode_to_chars(mp->m_mode);

    dict_add_string(dict, "lhs", lhs);
    vim_free(lhs);
    dict_add_string(dict, "lhsraw", mp->m_চাবিগুলি);
    যদি (lhsrawalt)
	// Also add the value for the simplified entry.
	dict_add_string(dict, "lhsrawalt", lhsrawalt);
    dict_add_string(dict, "rhs", mp->m_orig_str);
    dict_add_সংখ্যা(dict, "noremap", mp->m_noremap ? 1L : 0L);
    dict_add_সংখ্যা(dict, "script", mp->m_noremap == REMAP_SCRIPT
		    ? 1L : 0L);
    dict_add_সংখ্যা(dict, "expr", mp->m_রাশিমালা ? 1L : 0L);
    dict_add_সংখ্যা(dict, "silent", mp->m_silent ? 1L : 0L);
    dict_add_সংখ্যা(dict, "sid", (long)mp->m_script_ctx.sc_sid);
    dict_add_সংখ্যা(dict, "scriptversion",
		    (long)mp->m_script_ctx.sc_version);
    dict_add_সংখ্যা(dict, "lnum", (long)mp->m_script_ctx.sc_lnum);
    dict_add_সংখ্যা(dict, "buffer", (long)বাফার_local);
    dict_add_সংখ্যা(dict, "nowait", mp->m_nowait ? 1L : 0L);
    dict_add_string(dict, "mode", mapmode);
    dict_add_সংখ্যা(dict, "abbr", abbr ? 1L : 0L);
    dict_add_সংখ্যা(dict, "mode_bits", mp->m_mode);

    vim_free(mapmode);
}

    স্থির শূন্য
get_maparg(typval_T *argvars, typval_T *rettv, পূর্ণসংখ্যা exact)
{
    char_u	*চাবিগুলি;
    char_u	*চাবিগুলি_simplified;
    char_u	*which;
    char_u	buf[NUMBUFLEN];
    char_u	*চাবিগুলি_buf = ফাঁকা;
    char_u	*alt_চাবিগুলি_buf = ফাঁকা;
    পূর্ণসংখ্যা		did_simplify = FALSE;
    char_u	*rhs;
    পূর্ণসংখ্যা		mode;
    পূর্ণসংখ্যা		abbr = FALSE;
    পূর্ণসংখ্যা		get_dict = FALSE;
    mapblock_T	*mp = ফাঁকা;
    পূর্ণসংখ্যা		বাফার_local;
    পূর্ণসংখ্যা		flags = REPTERM_FROM_PART | REPTERM_DO_LT;

    // return empty string for failure
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;

    চাবিগুলি = tv_get_string(&argvars[0]);
    যদি (*চাবিগুলি == NUL)
	ফেরত;

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	which = tv_get_string_buf_chk(&argvars[1], buf);
	যদি (argvars[2].v_type != VAR_UNKNOWN)
	{
	    abbr = (পূর্ণসংখ্যা)tv_get_bool(&argvars[2]);
	    যদি (argvars[3].v_type != VAR_UNKNOWN)
		get_dict = (পূর্ণসংখ্যা)tv_get_bool(&argvars[3]);
	}
    }
    অন্যথা
	which = (char_u *)"";
    যদি (which == ফাঁকা)
	ফেরত;

    mode = get_map_mode(&which, 0);

    চাবিগুলি_simplified = replace_termcodes(চাবিগুলি, &চাবিগুলি_buf, 0, flags,
								&did_simplify);
    rhs = check_map(চাবিগুলি_simplified, mode, exact, FALSE, abbr,
							   &mp, &বাফার_local);
    যদি (did_simplify)
    {
	// When the lhs is being simplified the not-simplified keys are
	// preferred for printing, like in do_map().
	(শূন্য)replace_termcodes(চাবিগুলি, &alt_চাবিগুলি_buf, 0,
					flags | REPTERM_NO_SIMPLIFY, ফাঁকা);
	rhs = check_map(alt_চাবিগুলি_buf, mode, exact, FALSE, abbr, &mp,
								&বাফার_local);
    }

    যদি (!get_dict)
    {
	// Return a string.
	যদি (rhs != ফাঁকা)
	{
	    যদি (*rhs == NUL)
		rettv->vval.v_string = vim_strsave((char_u *)"<Nop>");
	    অন্যথা
		rettv->vval.v_string = str2special_save(rhs, FALSE, FALSE);
	}

    }
    অন্যথা যদি (rettv_dict_alloc(rettv) == OK && rhs != ফাঁকা)
	mapblock2dict(mp, rettv->vval.v_dict,
			  did_simplify ? চাবিগুলি_simplified : ফাঁকা,
			  বাফার_local, abbr);

    vim_free(চাবিগুলি_buf);
    vim_free(alt_চাবিগুলি_buf);
}

/*
 * "maplist()" function
 */
    শূন্য
f_maplist(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    mapblock_T	*mp;
    পূর্ণসংখ্যা		বাফার_local;
    char_u	*চাবিগুলি_buf;
    পূর্ণসংখ্যা		did_simplify;
    পূর্ণসংখ্যা		hash;
    char_u	*lhs;
    ধ্রুবক পূর্ণসংখ্যা	flags = REPTERM_FROM_PART | REPTERM_DO_LT;
    পূর্ণসংখ্যা		abbr = FALSE;

    যদি (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ফেরত;
    যদি (argvars[0].v_type != VAR_UNKNOWN)
	abbr = tv_get_bool(&argvars[0]);

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    জন্য (বাফার_local = 0; বাফার_local <= 1; ++বাফার_local)
    {
	জন্য (hash = 0; hash < 256; ++hash)
	{
	    যদি (abbr)
	    {
		যদি (hash > 0)		// there is only one abbr list
		    বিরতি;
		যদি (বাফার_local)
		    mp = curbuf->b_first_abbr;
		অন্যথা
		    mp = first_abbr;
	    }
	    অন্যথা যদি (বাফার_local)
		mp = curbuf->b_maphash[hash];
	    অন্যথা
		mp = maphash[hash];
	    জন্য (; mp; mp = mp->m_পরবর্তী)
	    {
		যদি (mp->m_simplified)
		    চালু;
		যদি ((d = dict_alloc()) == ফাঁকা)
		    ফেরত;
		যদি (list_append_dict(rettv->vval.v_list, d) == FAIL)
		{
		    dict_unref(d);
		    ফেরত;
		}

		চাবিগুলি_buf = ফাঁকা;
		did_simplify = FALSE;

		lhs = str2special_save(mp->m_চাবিগুলি, TRUE, FALSE);
		(শূন্য)replace_termcodes(lhs, &চাবিগুলি_buf, 0, flags,
								&did_simplify);
		vim_free(lhs);

		mapblock2dict(mp, d,
				 did_simplify ? চাবিগুলি_buf : ফাঁকা,
				 বাফার_local, abbr);
		vim_free(চাবিগুলি_buf);
	    }
	}
    }
}

/*
 * "maparg()" function
 */
    শূন্য
f_maparg(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
		ফেরত;

    get_maparg(argvars, rettv, TRUE);
}

/*
 * "mapcheck()" function
 */
    শূন্য
f_mapcheck(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ফেরত;

    get_maparg(argvars, rettv, FALSE);
}

/*
 * Get the mapping mode from the mode string.
 * It may contain multiple characters, eg "nox", or "!", or ' '
 * Return 0 if there is an error.
 */
    স্থির পূর্ণসংখ্যা
get_map_mode_string(char_u *mode_string, পূর্ণসংখ্যা abbr)
{
    char_u	*p = mode_string;
    পূর্ণসংখ্যা		mode = 0;
    পূর্ণসংখ্যা		tmode;
    পূর্ণসংখ্যা		modec;
    ধ্রুবক পূর্ণসংখ্যা	MASK_V = MODE_VISUAL | MODE_SELECT;
    ধ্রুবক পূর্ণসংখ্যা	MASK_MAP = MODE_VISUAL | MODE_SELECT | MODE_NORMAL
							     | MODE_OP_PENDING;
    ধ্রুবক পূর্ণসংখ্যা	MASK_BANG = MODE_ঢোকাও | MODE_CMDLINE;

    যদি (*p == NUL)
	p = (char_u *)" ";	// compatibility
    যখন ((modec = *p++))
    {
	নির্বাচন (modec)
	{
	    ক্ষেত্র 'i': tmode = MODE_ঢোকাও;	বিরতি;
	    ক্ষেত্র 'l': tmode = MODE_LANGMAP;	বিরতি;
	    ক্ষেত্র 'c': tmode = MODE_CMDLINE;	বিরতি;
	    ক্ষেত্র 'n': tmode = MODE_NORMAL;	বিরতি;
	    ক্ষেত্র 'x': tmode = MODE_VISUAL;	বিরতি;
	    ক্ষেত্র 's': tmode = MODE_SELECT;	বিরতি;
	    ক্ষেত্র 'o': tmode = MODE_OP_PENDING;	বিরতি;
	    ক্ষেত্র 't': tmode = MODE_TERMINAL;	বিরতি;
	    ক্ষেত্র 'v': tmode = MASK_V;		বিরতি;
	    ক্ষেত্র '!': tmode = MASK_BANG;	বিরতি;
	    ক্ষেত্র ' ': tmode = MASK_MAP;		বিরতি;
	    পূর্বনির্ধারিত:
		      ফেরত 0; // error, unknown mode character
	}
	mode |= tmode;
    }
    যদি ((abbr && (mode & ~MASK_BANG) != 0)
	|| (!abbr && (mode & (mode-1)) != 0 // more than one bit set
	    && (
		// false if multiple bits set in mode and mode is fully
		// contained in one mask
		!(((mode & MASK_BANG) != 0 && (mode & ~MASK_BANG) == 0)
		    || ((mode & MASK_MAP) != 0 && (mode & ~MASK_MAP) == 0)))))
	ফেরত 0;

    ফেরত mode;
}

/*
 * "mapset()" function
 */
    শূন্য
f_mapset(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*which;
    পূর্ণসংখ্যা		mode;
    char_u	buf[NUMBUFLEN];
    পূর্ণসংখ্যা		is_abbr;
    dict_T	*d;
    char_u	*lhs;
    char_u	*lhsraw;
    char_u	*lhsrawalt;
    char_u	*rhs;
    char_u	*orig_rhs;
    char_u	*arg_buf = ফাঁকা;
    পূর্ণসংখ্যা		noremap;
    পূর্ণসংখ্যা		রাশিমালা;
    পূর্ণসংখ্যা		silent;
    পূর্ণসংখ্যা		বাফার;
    scid_T	sid;
    পূর্ণসংখ্যা		scriptversion;
    linenr_T	lnum;
    mapblock_T	**map_table = maphash;
    mapblock_T  **abbr_table = &first_abbr;
    পূর্ণসংখ্যা		nowait;
    char_u	*arg;
    পূর্ণসংখ্যা		dict_only;
    mapblock_T	*mp_ফলাফল[2] = {ফাঁকা, ফাঁকা};

    যদি (check_secure())
	ফেরত;

    // If first arg is a dict, then that's the only arg permitted.
    dict_only = argvars[0].v_type == VAR_DICT;
    যদি (in_vim9script()
	    && (check_for_string_or_dict_arg(argvars, 0) == FAIL
		|| (dict_only && check_for_unknown_arg(argvars, 1) == FAIL)
		|| (!dict_only
		    && (check_for_string_arg(argvars, 0) == FAIL
			|| check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_dict_arg(argvars, 2) == FAIL))))
	ফেরত;

    যদি (dict_only)
    {
	d = argvars[0].vval.v_dict;
	which = dict_get_string(d, "mode", FALSE);
	is_abbr = dict_get_bool(d, "abbr", -1);
	যদি (which == ফাঁকা || is_abbr < 0)
	{
	    emsg(_(e_entries_missing_in_mapset_dict_argument));
	    ফেরত;
	}
    }
    অন্যথা
    {
	which = tv_get_string_buf_chk(&argvars[0], buf);
	যদি (which == ফাঁকা)
	    ফেরত;
	is_abbr = (পূর্ণসংখ্যা)tv_get_bool(&argvars[1]);

	যদি (check_for_dict_arg(argvars, 2) == FAIL)
	    ফেরত;
	d = argvars[2].vval.v_dict;
    }
    mode = get_map_mode_string(which, is_abbr);
    যদি (mode == 0)
    {
	semsg(_(e_illegal_map_mode_string_str), which);
	ফেরত;
    }


    // Get the values in the same order as above in get_maparg().
    lhs = dict_get_string(d, "lhs", FALSE);
    lhsraw = dict_get_string(d, "lhsraw", FALSE);
    lhsrawalt = dict_get_string(d, "lhsrawalt", FALSE);
    rhs = dict_get_string(d, "rhs", FALSE);
    যদি (lhs == ফাঁকা || lhsraw == ফাঁকা || rhs == ফাঁকা)
    {
	emsg(_(e_entries_missing_in_mapset_dict_argument));
	ফেরত;
    }
    orig_rhs = rhs;

    noremap = dict_get_সংখ্যা(d, "noremap") ? REMAP_NONE: 0;
    যদি (dict_get_সংখ্যা(d, "script") != 0)
	noremap = REMAP_SCRIPT;
    রাশিমালা = dict_get_সংখ্যা(d, "expr") != 0;
    silent = dict_get_সংখ্যা(d, "silent") != 0;
    sid = dict_get_সংখ্যা(d, "sid");
    scriptversion = dict_get_সংখ্যা(d, "scriptversion");
    lnum = dict_get_সংখ্যা(d, "lnum");
    বাফার = dict_get_সংখ্যা(d, "buffer");
    nowait = dict_get_সংখ্যা(d, "nowait") != 0;
    // mode from the dict is not used

    যদি (STRICMP(rhs, "<nop>") == 0)	// "<Nop>" means nothing
	rhs = (char_u *)"";
    অন্যথা
	rhs = replace_termcodes(rhs, &arg_buf, sid,
					REPTERM_DO_LT | REPTERM_SPECIAL, ফাঁকা);

    যদি (বাফার)
    {
	map_table = curbuf->b_maphash;
	abbr_table = &curbuf->b_first_abbr;
    }

    // Delete any existing mapping for this lhs and mode.
    যদি (বাফার)
    {
	arg = alloc(STRLEN(lhs) + STRLEN("<buffer>") + 1);
	যদি (arg == ফাঁকা)
	    ফেরত;
	STRCPY(arg, "<buffer>");
	STRCPY(arg + 8, lhs);
    }
    অন্যথা
    {
	arg = vim_strsave(lhs);
	যদি (arg == ফাঁকা)
	    ফেরত;
    }
    do_map(MAPTYPE_UNMAP_LHS, arg, mode, is_abbr);
    vim_free(arg);

    mp_ফলাফল[0] = map_add(map_table, abbr_table, lhsraw, rhs, orig_rhs,
			    noremap, nowait, silent, mode, is_abbr, রাশিমালা, sid,
			    scriptversion, lnum, 0);
    যদি (lhsrawalt != ফাঁকা)
	mp_ফলাফল[1] = map_add(map_table, abbr_table, lhsrawalt, rhs, orig_rhs,
			    noremap, nowait, silent, mode, is_abbr, রাশিমালা, sid,
			    scriptversion, lnum, 1);

    যদি (mp_ফলাফল[0] != ফাঁকা && mp_ফলাফল[1] != ফাঁকা)
    {
	mp_ফলাফল[0]->m_alt = mp_ফলাফল[1];
	mp_ফলাফল[1]->m_alt = mp_ফলাফল[0];
    }

    vim_free(arg_buf);
}
#endif


#যদি defined(MSWIN) || defined(MACOS_X)

# define VIS_SEL	(MODE_VISUAL | MODE_SELECT)	// abbreviation

/*
 * Default mappings for some often used keys.
 */
কাঠামো initmap
{
    char_u	*arg;
    পূর্ণসংখ্যা		mode;
};

# ifdef FEAT_GUI_MSWIN
// Use the Windows (CUA) keybindings. (GUI)
স্থির কাঠামো initmap initmappings[] =
{
	// paste, copy and cut
	{(char_u *)"<S-Insert> \"*P", MODE_NORMAL},
	{(char_u *)"<S-Insert> \"-d\"*P", VIS_SEL},
	{(char_u *)"<S-Insert> <C-R><C-O>*", MODE_ঢোকাও | MODE_CMDLINE},
	{(char_u *)"<C-Insert> \"*y", VIS_SEL},
	{(char_u *)"<S-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-X> \"*d", VIS_SEL},
	// Missing: CTRL-C (cancel) and CTRL-V (block selection)
};
# endif

# যদি defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
// Use the Windows (CUA) keybindings. (Console)
স্থির কাঠামো initmap cinitmappings[] =
{
	{(char_u *)"\316w <C-Home>", MODE_NORMAL | VIS_SEL},
	{(char_u *)"\316w <C-Home>", MODE_ঢোকাও | MODE_CMDLINE},
	{(char_u *)"\316u <C-End>", MODE_NORMAL | VIS_SEL},
	{(char_u *)"\316u <C-End>", MODE_ঢোকাও | MODE_CMDLINE},

	// paste, copy and cut
#  ifdef FEAT_CLIPBOARD
	{(char_u *)"\316\324 \"*P", MODE_NORMAL},   // SHIFT-Insert is "*P
	{(char_u *)"\316\324 \"-d\"*P", VIS_SEL},   // SHIFT-Insert is "-d"*P
	{(char_u *)"\316\324 \022\017*", MODE_ঢোকাও},  // SHIFT-Insert is ^R^O*
	{(char_u *)"\316\325 \"*y", VIS_SEL},	    // CTRL-Insert is "*y
	{(char_u *)"\316\327 \"*d", VIS_SEL},	    // SHIFT-Del is "*d
	{(char_u *)"\316\330 \"*d", VIS_SEL},	    // CTRL-Del is "*d
	{(char_u *)"\030 \"*d", VIS_SEL},	    // CTRL-X is "*d
#  অন্যথা
	{(char_u *)"\316\324 P", MODE_NORMAL},	    // SHIFT-Insert is P
	{(char_u *)"\316\324 \"-dP", VIS_SEL},	    // SHIFT-Insert is "-dP
	{(char_u *)"\316\324 \022\017\"", MODE_ঢোকাও}, // SHIFT-Insert is ^R^O"
	{(char_u *)"\316\325 y", VIS_SEL},	    // CTRL-Insert is y
	{(char_u *)"\316\327 d", VIS_SEL},	    // SHIFT-Del is d
	{(char_u *)"\316\330 d", VIS_SEL},	    // CTRL-Del is d
#  endif
};
# endif

# যদি defined(MACOS_X)
স্থির কাঠামো initmap initmappings[] =
{
	// Use the Standard MacOS binding.
	// paste, copy and cut
	{(char_u *)"<D-v> \"*P", MODE_NORMAL},
	{(char_u *)"<D-v> \"-d\"*P", VIS_SEL},
	{(char_u *)"<D-v> <C-R>*", MODE_ঢোকাও | MODE_CMDLINE},
	{(char_u *)"<D-c> \"*y", VIS_SEL},
	{(char_u *)"<D-x> \"*d", VIS_SEL},
	{(char_u *)"<Backspace> \"-d", VIS_SEL},
};
# endif

# undef VIS_SEL
#endif

/*
 * Set up default mappings.
 */
    শূন্য
init_mappings(শূন্য)
{
#যদি defined(MSWIN) || defined(MACOS_X)
    পূর্ণসংখ্যা		i;

# যদি defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    যদি (!gui.starting)
#  endif
    {
	জন্য (i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(cinitmappings); ++i)
	    add_map(cinitmappings[i].arg, cinitmappings[i].mode, FALSE);
    }
# endif
# যদি defined(FEAT_GUI_MSWIN) || defined(MACOS_X)
    জন্য (i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(initmappings); ++i)
	add_map(initmappings[i].arg, initmappings[i].mode, FALSE);
# endif
#endif
}

/*
 * Add a mapping "map" for mode "mode".
 * When "nore" is TRUE use MAPTYPE_NOREMAP.
 * Need to put string in allocated memory, because do_map() will modify it.
 */
    শূন্য
add_map(char_u *map, পূর্ণসংখ্যা mode, পূর্ণসংখ্যা nore)
{
    char_u	*s;
    char_u	*cpo_save = p_cpo;

    p_cpo = empty_option;	// Allow <> notation
    s = vim_strsave(map);
    যদি (s != ফাঁকা)
    {
	(শূন্য)do_map(nore ? MAPTYPE_NOREMAP : MAPTYPE_MAP, s, mode, FALSE);
	vim_free(s);
    }
    p_cpo = cpo_save;
}

#যদি defined(FEAT_LANGMAP)
/*
 * Any character has an equivalent 'langmap' character.  This is used for
 * keyboards that have a special language mode that sends characters above
 * 128 (although other characters can be translated too).  The "to" field is a
 * Vim command character.  This avoids having to switch the keyboard back to
 * ASCII mode when leaving Insert mode.
 *
 * langmap_mapchar[] maps any of 256 chars to an ASCII char used for Vim
 * commands.
 * langmap_mapga.ga_data is a sorted table of langmap_entry_T.  This does the
 * same as langmap_mapchar[] for characters >= 256.
 *
 * Use growarray for 'langmap' chars >= 256
 */
প্রকারলেখ কাঠামো
{
    পূর্ণসংখ্যা	    from;
    পূর্ণসংখ্যা     to;
} langmap_entry_T;

স্থির garray_T langmap_mapga;

/*
 * Search for an entry in "langmap_mapga" for "from".  If found set the "to"
 * field.  If not found insert a new entry at the appropriate location.
 */
    স্থির শূন্য
langmap_set_entry(পূর্ণসংখ্যা from, পূর্ণসংখ্যা to)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_তথ্য);
    পূর্ণসংখ্যা		    a = 0;
    পূর্ণসংখ্যা		    b = langmap_mapga.ga_len;

    // Do a binary search for an existing entry.
    যখন (a != b)
    {
	পূর্ণসংখ্যা i = (a + b) / 2;
	পূর্ণসংখ্যা d = entries[i].from - from;

	যদি (d == 0)
	{
	    entries[i].to = to;
	    ফেরত;
	}
	যদি (d < 0)
	    a = i + 1;
	অন্যথা
	    b = i;
    }

    যদি (ga_grow(&langmap_mapga, 1) == FAIL)
	ফেরত;  // out of memory

    // insert new entry at position "a"
    entries = (langmap_entry_T *)(langmap_mapga.ga_তথ্য) + a;
    mch_memmove(entries + 1, entries,
			(langmap_mapga.ga_len - a) * আকার(langmap_entry_T));
    ++langmap_mapga.ga_len;
    entries[0].from = from;
    entries[0].to = to;
}

/*
 * Apply 'langmap' to multi-byte character "c" and return the result.
 */
    পূর্ণসংখ্যা
langmap_adjust_mb(পূর্ণসংখ্যা c)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_তথ্য);
    পূর্ণসংখ্যা a = 0;
    পূর্ণসংখ্যা b = langmap_mapga.ga_len;

    যখন (a != b)
    {
	পূর্ণসংখ্যা i = (a + b) / 2;
	পূর্ণসংখ্যা d = entries[i].from - c;

	যদি (d == 0)
	    ফেরত entries[i].to;  // found matching entry
	যদি (d < 0)
	    a = i + 1;
	অন্যথা
	    b = i;
    }
    ফেরত c;  // no entry found, return "c" unmodified
}

    শূন্য
langmap_init(শূন্য)
{
    পূর্ণসংখ্যা i;

    জন্য (i = 0; i < 256; i++)
	langmap_mapchar[i] = i;	 // we init with a one-to-one map
    ga_init2(&langmap_mapga, আকার(langmap_entry_T), 8);
}

/*
 * Called when langmap option is set; the language map can be
 * changed at any time!
 */
    অক্ষর *
did_set_langmap(optset_T *args UNUSED)
{
    char_u  *p;
    char_u  *p2;
    পূর্ণসংখ্যা	    from, to;

    ga_clear(&langmap_mapga);		    // clear the previous map first
    langmap_init();			    // back to one-to-one map

    জন্য (p = p_langmap; p[0] != NUL; )
    {
	জন্য (p2 = p; p2[0] != NUL && p2[0] != ',' && p2[0] != ';';
							       MB_PTR_ADV(p2))
	{
	    যদি (p2[0] == '\\' && p2[1] != NUL)
		++p2;
	}
	যদি (p2[0] == ';')
	    ++p2;	    // abcd;ABCD form, p2 points to A
	অন্যথা
	    p2 = ফাঁকা;	    // aAbBcCdD form, p2 is NULL
	যখন (p[0])
	{
	    যদি (p[0] == ',')
	    {
		++p;
		বিরতি;
	    }
	    যদি (p[0] == '\\' && p[1] != NUL)
		++p;
	    from = (*mb_ptr2char)(p);
	    to = NUL;
	    যদি (p2 == ফাঁকা)
	    {
		MB_PTR_ADV(p);
		যদি (p[0] != ',')
		{
		    যদি (p[0] == '\\')
			++p;
		    to = (*mb_ptr2char)(p);
		}
	    }
	    অন্যথা
	    {
		যদি (p2[0] != ',')
		{
		    যদি (p2[0] == '\\')
			++p2;
		    to = (*mb_ptr2char)(p2);
		}
	    }
	    যদি (to == NUL)
	    {
		sprintf(args->os_errbuf,
			_(e_langmap_matching_character_missing_for_str),
			transchar(from));
		ফেরত args->os_errbuf;
	    }

	    যদি (from >= 256)
		langmap_set_entry(from, to);
	    অন্যথা
		langmap_mapchar[from & 255] = to;

	    // Advance to next pair
	    MB_PTR_ADV(p);
	    যদি (p2 != ফাঁকা)
	    {
		MB_PTR_ADV(p2);
		যদি (*p == ';')
		{
		    p = p2;
		    যদি (p[0] != NUL)
		    {
			যদি (p[0] != ',')
			{
			    vim_snprintf(args->os_errbuf, args->os_errbuflen,
				    _(e_langmap_extra_characters_after_semicolon_str),
				    p);
			    ফেরত args->os_errbuf;
			}
			++p;
		    }
		    বিরতি;
		}
	    }
	}
    }

    ফেরত ফাঁকা;
}
#endif

    স্থির শূন্য
do_exmap(exarg_T *eap, পূর্ণসংখ্যা isabbrev)
{
    পূর্ণসংখ্যা	    mode;
    char_u  *cmdp;

    cmdp = eap->cmd;
    mode = get_map_mode(&cmdp, eap->forceit || isabbrev);

    নির্বাচন (do_map(*cmdp == 'n' ? MAPTYPE_NOREMAP
				: *cmdp == 'u' ? MAPTYPE_UNMAP : MAPTYPE_MAP,
						    eap->arg, mode, isabbrev))
    {
	ক্ষেত্র 1: emsg(_(e_invalid_argument));
		বিরতি;
	ক্ষেত্র 2: emsg((isabbrev ? _(e_no_such_abbreviation)
						      : _(e_no_such_mapping)));
		বিরতি;
    }
}

/*
 * ":abbreviate" and friends.
 */
    শূন্য
ex_abbreviate(exarg_T *eap)
{
    do_exmap(eap, TRUE);	// almost the same as mapping
}

/*
 * ":map" and friends.
 */
    শূন্য
ex_map(exarg_T *eap)
{
    // If we are sourcing .exrc or .vimrc in current directory we
    // print the mappings for security reasons.
    যদি (secure)
    {
	secure = 2;
	বার্তা_outtrans(eap->cmd);
	বার্তা_putchar('\n');
    }
    do_exmap(eap, FALSE);
}

/*
 * ":unmap" and friends.
 */
    শূন্য
ex_unmap(exarg_T *eap)
{
    do_exmap(eap, FALSE);
}

/*
 * ":mapclear" and friends.
 */
    শূন্য
ex_mapclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, eap->forceit, FALSE);
}

/*
 * ":abclear" and friends.
 */
    শূন্য
ex_abclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, TRUE, TRUE);
}
