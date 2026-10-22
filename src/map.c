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

#അന്തര്ഭാവ "vim.h"

/*
 * List used for abbreviations.
 */
സ്ഥിതം mapblock_T	*first_abbr = ശൂന്യ; // first entry in abbrlist

/*
 * Each mapping is put in one of the 256 hash lists, to speed up finding it.
 */
സ്ഥിതം mapblock_T	*(maphash[256]);
സ്ഥിതം പൂർണ്ണസംഖ്യ		maphash_valid = FALSE;

// When non-zero then no mappings can be added or removed.  Prevents mappings
// to change while listing them.
സ്ഥിതം പൂർണ്ണസംഖ്യ		map_locked = 0;

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
get_maphash_list(പൂർണ്ണസംഖ്യ state, പൂർണ്ണസംഖ്യ c)
{
    തിരികെ maphash[MAP_HASH(state, c)];
}

/*
 * Get the buffer-local hashed map list for "state" and first character "c".
 */
    mapblock_T *
get_buf_maphash_list(പൂർണ്ണസംഖ്യ state, പൂർണ്ണസംഖ്യ c)
{
    തിരികെ curbuf->b_maphash[MAP_HASH(state, c)];
}

    പൂർണ്ണസംഖ്യ
is_maphash_valid(ശൂന്യം)
{
    തിരികെ maphash_valid;
}

/*
 * Initialize maphash[] for first use.
 */
    സ്ഥിതം ശൂന്യം
validate_maphash(ശൂന്യം)
{
    എങ്കിൽ (maphash_valid)
	തിരികെ;

    CLEAR_FIELD(maphash);
    maphash_valid = TRUE;
}

/*
 * Delete one entry from the abbrlist or maphash[].
 * "mpp" is a pointer to the m_next field of the PREVIOUS entry!
 */
    സ്ഥിതം ശൂന്യം
map_free(mapblock_T **mpp)
{
    mapblock_T	*mp;

    mp = *mpp;
    vim_free(mp->m_താക്കോലുകൾ);
    എങ്കിൽ (mp->m_alt != ശൂന്യ)
	mp->m_alt->m_alt = ശൂന്യ;
    vim_free(mp->m_str);
    vim_free(mp->m_orig_str);
    *mpp = mp->m_next;
#ifdef FEAT_EVAL
    reset_last_used_map(mp);
#endif
    vim_free(mp);
}

/*
 * Return characters to represent the map mode in an allocated string.
 * Returns NULL when out of memory.
 */
    സ്ഥിതം char_u *
map_mode_to_chars(പൂർണ്ണസംഖ്യ mode)
{
    garray_T    mapmode;

    ga_init2(&mapmode, 1, 7);

    എങ്കിൽ ((mode & (MODE_തിരുകുക | MODE_CMDLINE)) == (MODE_തിരുകുക | MODE_CMDLINE))
	ga_append(&mapmode, '!');			// :map!
    അല്ലെങ്കിൽ എങ്കിൽ (mode & MODE_തിരുകുക)
	ga_append(&mapmode, 'i');			// :imap
    അല്ലെങ്കിൽ എങ്കിൽ (mode & MODE_LANGMAP)
	ga_append(&mapmode, 'l');			// :lmap
    അല്ലെങ്കിൽ എങ്കിൽ (mode & MODE_CMDLINE)
	ga_append(&mapmode, 'c');			// :cmap
    അല്ലെങ്കിൽ എങ്കിൽ ((mode
		 & (MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING))
		== (MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING))
	ga_append(&mapmode, ' ');			// :map
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (mode & MODE_NORMAL)
	    ga_append(&mapmode, 'n');			// :nmap
	എങ്കിൽ (mode & MODE_OP_PENDING)
	    ga_append(&mapmode, 'o');			// :omap
	എങ്കിൽ (mode & MODE_TERMINAL)
	    ga_append(&mapmode, 't');			// :tmap
	എങ്കിൽ ((mode & (MODE_VISUAL | MODE_SELECT)) == (MODE_VISUAL | MODE_SELECT))
	    ga_append(&mapmode, 'v');			// :vmap
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (mode & MODE_VISUAL)
		ga_append(&mapmode, 'x');		// :xmap
	    എങ്കിൽ (mode & MODE_SELECT)
		ga_append(&mapmode, 's');		// :smap
	}
    }

    ga_append(&mapmode, NUL);
    തിരികെ (char_u *)mapmode.ga_data;
}

/*
 * Output a line for one mapping.
 */
    സ്ഥിതം ശൂന്യം
showmap(
    mapblock_T	*mp,
    പൂർണ്ണസംഖ്യ		local)	    // TRUE for buffer-local map
{
    പൂർണ്ണസംഖ്യ		len = 1;
    char_u	*mapchars;

    എങ്കിൽ (message_filtered(mp->m_താക്കോലുകൾ) && message_filtered(mp->m_str))
	തിരികെ;

    // Prevent mappings to be cleared while at the more prompt.
    // Must jump to "theend" instead of returning.
    ++map_locked;

    എങ്കിൽ (സന്ദേശം_didout || സന്ദേശം_silent != 0)
    {
	സന്ദേശം_putchar('\n');
	എങ്കിൽ (got_int)	    // 'q' typed at MORE prompt
	    പോകുക theend;
    }

    mapchars = map_mode_to_chars(mp->m_mode);
    എങ്കിൽ (mapchars != ശൂന്യ)
    {
	സന്ദേശം_puts((അക്ഷരം *)mapchars);
	len = (പൂർണ്ണസംഖ്യ)STRLEN(mapchars);
	vim_free(mapchars);
    }

    ആകുമ്പോൾ (++len <= 3)
	സന്ദേശം_putchar(' ');

    // Display the LHS.  Get length of what we write.
    len = സന്ദേശം_outtrans_special(mp->m_താക്കോലുകൾ, TRUE, 0);
    ചെയ്യുക
    {
	സന്ദേശം_putchar(' ');		// pad with blanks
	++len;
    } ആകുമ്പോൾ (len < 12);

    എങ്കിൽ (mp->m_noremap == REMAP_NONE)
	സന്ദേശം_puts_attr("*", HL_ATTR(HLF_8));
    അല്ലെങ്കിൽ എങ്കിൽ (mp->m_noremap == REMAP_SCRIPT)
	സന്ദേശം_puts_attr("&", HL_ATTR(HLF_8));
    അല്ലെങ്കിൽ
	സന്ദേശം_putchar(' ');

    എങ്കിൽ (local)
	സന്ദേശം_putchar('@');
    അല്ലെങ്കിൽ
	സന്ദേശം_putchar(' ');

    // Use FALSE below if we only want things like <Up> to show up as such on
    // the rhs, and not M-x etc, TRUE gets both -- webb
    എങ്കിൽ (*mp->m_str == NUL)
	സന്ദേശം_puts_attr("<Nop>", HL_ATTR(HLF_8));
    അല്ലെങ്കിൽ
	സന്ദേശം_outtrans_special(mp->m_str, FALSE, 0);
#ifdef FEAT_EVAL
    എങ്കിൽ (p_verbose > 0)
	last_set_സന്ദേശം(mp->m_script_ctx);
#endif
    സന്ദേശം_clr_eos();
    out_flush();			// show one line at a time

theend:
    --map_locked;
}

    സ്ഥിതം mapblock_T *
map_add(
	mapblock_T  **map_table,
	mapblock_T  **abbr_table,
	char_u	    *താക്കോലുകൾ,
	char_u	    *rhs,
	char_u	    *orig_rhs,
	പൂർണ്ണസംഖ്യ	    noremap,
	പൂർണ്ണസംഖ്യ	    nowait,
	പൂർണ്ണസംഖ്യ	    silent,
	പൂർണ്ണസംഖ്യ	    mode,
	പൂർണ്ണസംഖ്യ	    is_abbr,
#ifdef FEAT_EVAL
	പൂർണ്ണസംഖ്യ	    വ്യഞ്ജകം,
	scid_T	    sid,	    // 0 to use current_sctx
	പൂർണ്ണസംഖ്യ	    scriptversion,
	linenr_T    lnum,
#endif
	പൂർണ്ണസംഖ്യ	    simplified)
{
    mapblock_T	*mp = ALLOC_CLEAR_ONE(mapblock_T);

    എങ്കിൽ (mp == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // If CTRL-C has been mapped, don't always use it for Interrupting.
    എങ്കിൽ (*താക്കോലുകൾ == Ctrl_C)
    {
	എങ്കിൽ (map_table == curbuf->b_maphash)
	    curbuf->b_mapped_ctrl_c |= mode;
	അല്ലെങ്കിൽ
	    mapped_ctrl_c |= mode;
    }

    mp->m_താക്കോലുകൾ = vim_strsave(താക്കോലുകൾ);
    mp->m_str = vim_strsave(rhs);
    mp->m_orig_str = vim_strsave(orig_rhs);
    എങ്കിൽ (mp->m_താക്കോലുകൾ == ശൂന്യ || mp->m_str == ശൂന്യ)
    {
	vim_free(mp->m_താക്കോലുകൾ);
	vim_free(mp->m_str);
	vim_free(mp->m_orig_str);
	vim_free(mp);
	തിരികെ ശൂന്യ;
    }
    mp->m_keylen = (പൂർണ്ണസംഖ്യ)STRLEN(mp->m_താക്കോലുകൾ);
    mp->m_noremap = noremap;
    mp->m_nowait = nowait;
    mp->m_silent = silent;
    mp->m_mode = mode;
    mp->m_simplified = simplified;
#ifdef FEAT_EVAL
    mp->m_വ്യഞ്ജകം = വ്യഞ്ജകം;
    എങ്കിൽ (sid != 0)
    {
	mp->m_script_ctx.sc_sid = sid;
	mp->m_script_ctx.sc_lnum = lnum;
	mp->m_script_ctx.sc_version = scriptversion;
    }
    അല്ലെങ്കിൽ
    {
	mp->m_script_ctx = current_sctx;
	എങ്കിൽ (cmdmod.cmod_flags & CMOD_LEGACY)
	    mp->m_script_ctx.sc_version = 1;
	mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
    }
#endif

    // add the new entry in front of the abbrlist or maphash[] list
    എങ്കിൽ (is_abbr)
    {
	mp->m_next = *abbr_table;
	*abbr_table = mp;
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ n = MAP_HASH(mp->m_mode, mp->m_താക്കോലുകൾ[0]);

	mp->m_next = map_table[n];
	map_table[n] = mp;
    }
    തിരികെ mp;
}

/*
 * List mappings.  When "haskey" is FALSE all mappings, otherwise mappings that
 * match "keys[keys_len]".
 */
    സ്ഥിതം ശൂന്യം
list_mappings(
	പൂർണ്ണസംഖ്യ	keyround,
	പൂർണ്ണസംഖ്യ	abbrev,
	പൂർണ്ണസംഖ്യ	haskey,
	char_u	*താക്കോലുകൾ,
	പൂർണ്ണസംഖ്യ	താക്കോലുകൾ_len,
	പൂർണ്ണസംഖ്യ	mode,
	പൂർണ്ണസംഖ്യ	*did_local)
{
    // Prevent mappings to be cleared while at the more prompt.
    ++map_locked;

    എങ്കിൽ (p_verbose > 0 && keyround == 1)
    {
	എങ്കിൽ (seen_Modify_Other_താക്കോലുകൾ)
	    സന്ദേശം_puts(_("Seen modifyOtherKeys: true\n"));

	എങ്കിൽ (modify_otherkeys_state != MOKS_INITIAL)
	{
	    അക്ഷരം *name = _("Unknown");
	    തിരഞ്ഞെടുപ്പ് (modify_otherkeys_state)
	    {
		അവസ്ഥ MOKS_INITIAL: നിർത്തുക;
		അവസ്ഥ MOKS_OFF: name = _("Off"); നിർത്തുക;
		അവസ്ഥ MOKS_ENABLED: name = _("On"); നിർത്തുക;
		അവസ്ഥ MOKS_DISABLED: name = _("Disabled"); നിർത്തുക;
		അവസ്ഥ MOKS_AFTER_T_TE: name = _("Cleared"); നിർത്തുക;
	    }

	    അക്ഷരം buf[200];
	    vim_snprintf(buf, വലുപ്പം(buf),
				    _("modifyOtherKeys detected: %s\n"), name);
	    സന്ദേശം_puts(buf);
	}

	എങ്കിൽ (kitty_protocol_state != KKPS_INITIAL)
	{
	    അക്ഷരം *name = _("Unknown");
	    തിരഞ്ഞെടുപ്പ് (kitty_protocol_state)
	    {
		അവസ്ഥ KKPS_INITIAL: നിർത്തുക;
		അവസ്ഥ KKPS_OFF: name = _("Off"); നിർത്തുക;
		അവസ്ഥ KKPS_ENABLED: name = _("On"); നിർത്തുക;
		അവസ്ഥ KKPS_DISABLED: name = _("Disabled"); നിർത്തുക;
		അവസ്ഥ KKPS_AFTER_T_TE: name = _("Cleared"); നിർത്തുക;
	    }

	    അക്ഷരം buf[200];
	    vim_snprintf(buf, വലുപ്പം(buf),
				     _("Kitty keyboard protocol: %s\n"), name);
	    സന്ദേശം_puts(buf);
	}
    }

    // need to loop over all global hash lists
    വേണ്ടി (പൂർണ്ണസംഖ്യ hash = 0; hash < 256 && !got_int; ++hash)
    {
	mapblock_T	*mp;

	എങ്കിൽ (abbrev)
	{
	    എങ്കിൽ (hash != 0)	// there is only one abbreviation list
		നിർത്തുക;
	    mp = curbuf->b_first_abbr;
	}
	അല്ലെങ്കിൽ
	    mp = curbuf->b_maphash[hash];
	വേണ്ടി ( ; mp != ശൂന്യ && !got_int; mp = mp->m_next)
	{
	    // check entries with the same mode
	    എങ്കിൽ (!mp->m_simplified && (mp->m_mode & mode) != 0)
	    {
		എങ്കിൽ (!haskey)		    // show all entries
		{
		    showmap(mp, TRUE);
		    *did_local = TRUE;
		}
		അല്ലെങ്കിൽ
		{
		    പൂർണ്ണസംഖ്യ n = mp->m_keylen;
		    എങ്കിൽ (STRNCMP(mp->m_താക്കോലുകൾ, താക്കോലുകൾ,
				   (size_t)(n < താക്കോലുകൾ_len ? n : താക്കോലുകൾ_len)) == 0)
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
    പൂർണ്ണസംഖ്യ
do_map(
    പൂർണ്ണസംഖ്യ		maptype,
    char_u	*arg,
    പൂർണ്ണസംഖ്യ		mode,
    പൂർണ്ണസംഖ്യ		abbrev)		// not a mapping but an abbreviation
{
    char_u	*താക്കോലുകൾ;
    mapblock_T	*mp, **mpp;
    mapblock_T	*mp_result[2] = {ശൂന്യ, ശൂന്യ};
    char_u	*rhs;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		n;
    പൂർണ്ണസംഖ്യ		len = 0;	// init for GCC
    പൂർണ്ണസംഖ്യ		hasarg;
    പൂർണ്ണസംഖ്യ		haskey;
    പൂർണ്ണസംഖ്യ		do_print;
    പൂർണ്ണസംഖ്യ		keyround;
    char_u	*താക്കോലുകൾ_buf = ശൂന്യ;
    char_u	*alt_താക്കോലുകൾ_buf = ശൂന്യ;
    char_u	*arg_buf = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		retval = 0;
    പൂർണ്ണസംഖ്യ		do_backslash;
    mapblock_T	**abbr_table;
    mapblock_T	**map_table;
    പൂർണ്ണസംഖ്യ		unique = FALSE;
    പൂർണ്ണസംഖ്യ		nowait = FALSE;
    പൂർണ്ണസംഖ്യ		silent = FALSE;
    പൂർണ്ണസംഖ്യ		special = FALSE;
#ifdef FEAT_EVAL
    പൂർണ്ണസംഖ്യ		വ്യഞ്ജകം = FALSE;
#endif
    പൂർണ്ണസംഖ്യ		did_simplify = FALSE;
    പൂർണ്ണസംഖ്യ		unmap_lhs_only = FALSE;
    പൂർണ്ണസംഖ്യ		noremap;
    char_u      *orig_rhs;

    താക്കോലുകൾ = arg;
    map_table = maphash;
    abbr_table = &first_abbr;

    എങ്കിൽ (maptype == MAPTYPE_UNMAP_LHS)
    {
	unmap_lhs_only = TRUE;
	maptype = MAPTYPE_UNMAP;
    }

    // For ":noremap" don't remap, otherwise do remap.
    എങ്കിൽ (maptype == MAPTYPE_NOREMAP)
	noremap = REMAP_NONE;
    അല്ലെങ്കിൽ
	noremap = REMAP_YES;

    // Accept <buffer>, <nowait>, <silent>, <expr> <script> and <unique> in
    // any order.
    വേണ്ടി (;;)
    {
	// Check for "<buffer>": mapping local to buffer.
	എങ്കിൽ (STRNCMP(താക്കോലുകൾ, "<buffer>", 8) == 0)
	{
	    താക്കോലുകൾ = skipwhite(താക്കോലുകൾ + 8);
	    map_table = curbuf->b_maphash;
	    abbr_table = &curbuf->b_first_abbr;
	    തുടരുക;
	}

	// Check for "<nowait>": don't wait for more characters.
	എങ്കിൽ (STRNCMP(താക്കോലുകൾ, "<nowait>", 8) == 0)
	{
	    താക്കോലുകൾ = skipwhite(താക്കോലുകൾ + 8);
	    nowait = TRUE;
	    തുടരുക;
	}

	// Check for "<silent>": don't echo commands.
	എങ്കിൽ (STRNCMP(താക്കോലുകൾ, "<silent>", 8) == 0)
	{
	    താക്കോലുകൾ = skipwhite(താക്കോലുകൾ + 8);
	    silent = TRUE;
	    തുടരുക;
	}

	// Check for "<special>": accept special keys in <>
	എങ്കിൽ (STRNCMP(താക്കോലുകൾ, "<special>", 9) == 0)
	{
	    താക്കോലുകൾ = skipwhite(താക്കോലുകൾ + 9);
	    special = TRUE;
	    തുടരുക;
	}

#ifdef FEAT_EVAL
	// Check for "<script>": remap script-local mappings only
	എങ്കിൽ (STRNCMP(താക്കോലുകൾ, "<script>", 8) == 0)
	{
	    താക്കോലുകൾ = skipwhite(താക്കോലുകൾ + 8);
	    noremap = REMAP_SCRIPT;
	    തുടരുക;
	}

	// Check for "<expr>": {rhs} is an expression.
	എങ്കിൽ (STRNCMP(താക്കോലുകൾ, "<expr>", 6) == 0)
	{
	    താക്കോലുകൾ = skipwhite(താക്കോലുകൾ + 6);
	    വ്യഞ്ജകം = TRUE;
	    തുടരുക;
	}
#endif
	// Check for "<unique>": don't overwrite an existing mapping.
	എങ്കിൽ (STRNCMP(താക്കോലുകൾ, "<unique>", 8) == 0)
	{
	    താക്കോലുകൾ = skipwhite(താക്കോലുകൾ + 8);
	    unique = TRUE;
	    തുടരുക;
	}
	നിർത്തുക;
    }

    validate_maphash();

    // Find end of keys and skip CTRL-Vs (and backslashes) in it.
    // Accept backslash like CTRL-V when 'cpoptions' does not contain 'B'.
    // with :unmap white space is included in the keys, no argument possible.
    p = താക്കോലുകൾ;
    do_backslash = (vim_strchr(p_cpo, CPO_BSLASH) == ശൂന്യ);
    ആകുമ്പോൾ (*p && (maptype == MAPTYPE_UNMAP || !VIM_ISWHITE(*p)))
    {
	എങ്കിൽ ((p[0] == Ctrl_V || (do_backslash && p[0] == '\\')) &&
								  p[1] != NUL)
	    ++p;		// skip CTRL-V or backslash
	++p;
    }
    എങ്കിൽ (*p != NUL)
	*p++ = NUL;

    p = skipwhite(p);
    rhs = p;
    hasarg = (*rhs != NUL);
    haskey = (*താക്കോലുകൾ != NUL);
    do_print = !haskey || (maptype != MAPTYPE_UNMAP && !hasarg);

    // check for :unmap without argument
    എങ്കിൽ (maptype == MAPTYPE_UNMAP && !haskey)
    {
	retval = 1;
	പോകുക theend;
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
    എങ്കിൽ (haskey)
    {
	char_u	*new_താക്കോലുകൾ;
	പൂർണ്ണസംഖ്യ	flags = REPTERM_FROM_PART | REPTERM_DO_LT;

	എങ്കിൽ (special)
	    flags |= REPTERM_SPECIAL;
	new_താക്കോലുകൾ = replace_termcodes(താക്കോലുകൾ, &താക്കോലുകൾ_buf, 0, flags, &did_simplify);
	എങ്കിൽ (did_simplify)
	    (ശൂന്യം)replace_termcodes(താക്കോലുകൾ, &alt_താക്കോലുകൾ_buf, 0,
					    flags | REPTERM_NO_SIMPLIFY, ശൂന്യ);
	താക്കോലുകൾ = new_താക്കോലുകൾ;
    }
    orig_rhs = rhs;
    എങ്കിൽ (hasarg)
    {
	എങ്കിൽ (STRICMP(rhs, "<nop>") == 0)	    // "<Nop>" means nothing
	    rhs = (char_u *)"";
	അല്ലെങ്കിൽ
	    rhs = replace_termcodes(rhs, &arg_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), ശൂന്യ);
    }

    /*
     * The following is done twice if we have two versions of keys:
     * "alt_keys_buf" is not NULL.
     */
    വേണ്ടി (keyround = 1; keyround <= 2; ++keyround)
    {
	പൂർണ്ണസംഖ്യ	did_it = FALSE;
	പൂർണ്ണസംഖ്യ	did_local = FALSE;
	പൂർണ്ണസംഖ്യ	keyround1_simplified = keyround == 1 && did_simplify;
	പൂർണ്ണസംഖ്യ	round;
	പൂർണ്ണസംഖ്യ	അക്കം_rounds;

	എങ്കിൽ (keyround == 2)
	{
	    എങ്കിൽ (alt_താക്കോലുകൾ_buf == ശൂന്യ)
		നിർത്തുക;
	    താക്കോലുകൾ = alt_താക്കോലുകൾ_buf;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (alt_താക്കോലുകൾ_buf != ശൂന്യ && do_print)
	    // when printing always use the not-simplified map
	    താക്കോലുകൾ = alt_താക്കോലുകൾ_buf;

	// check arguments and translate function keys
	എങ്കിൽ (haskey)
	{
	    len = (പൂർണ്ണസംഖ്യ)STRLEN(താക്കോലുകൾ);
	    എങ്കിൽ (len > MAXMAPLEN)	// maximum length of MAXMAPLEN chars
	    {
		retval = 1;
		പോകുക theend;
	    }

	    എങ്കിൽ (abbrev && maptype != MAPTYPE_UNMAP)
	    {
		// If an abbreviation ends in a keyword character, the
		// rest must be all keyword-char or all non-keyword-char.
		// Otherwise we won't be able to find the start of it in a
		// vi-compatible way.
		എങ്കിൽ (has_mbyte)
		{
		    പൂർണ്ണസംഖ്യ		first, last;
		    പൂർണ്ണസംഖ്യ		same = -1;
		    char_u	താക്കോലുകൾ_unescaped[MAXMAPLEN + 1];
		    size_t	താക്കോലുകൾ_unescaped_len;

		    mch_memmove(താക്കോലുകൾ_unescaped, താക്കോലുകൾ, (size_t)(len + 1));
		    താക്കോലുകൾ_unescaped_len = vim_unescape_csi(താക്കോലുകൾ_unescaped);
		    p = താക്കോലുകൾ_unescaped;

		    first = vim_iswordp(p);
		    last = first;
		    MB_PTR_ADV(p);
		    n = 1;
		    ആകുമ്പോൾ (p < താക്കോലുകൾ_unescaped + താക്കോലുകൾ_unescaped_len)
		    {
			++n;			// nr of (multi-byte) chars
			last = vim_iswordp(p);	// type of last char
			എങ്കിൽ (same == -1 && last != first)
			    same = n - 1;	// count of same char type
			MB_PTR_ADV(p);
		    }
		    എങ്കിൽ (last && n > 2 && same >= 0 && same < n - 1)
		    {
			retval = 1;
			പോകുക theend;
		    }
		}
		അല്ലെങ്കിൽ എങ്കിൽ (vim_iswordc(താക്കോലുകൾ[len - 1]))
		    // ends in keyword char
		    വേണ്ടി (n = 0; n < len - 2; ++n)
			എങ്കിൽ (vim_iswordc(താക്കോലുകൾ[n]) != vim_iswordc(താക്കോലുകൾ[len - 2]))
			{
			    retval = 1;
			    പോകുക theend;
			}
		// An abbreviation cannot contain white space.
		വേണ്ടി (n = 0; n < len; ++n)
		    എങ്കിൽ (VIM_ISWHITE(താക്കോലുകൾ[n]))
		    {
			retval = 1;
			പോകുക theend;
		    }
	    }
	}

	എങ്കിൽ (haskey && hasarg && abbrev)	// if we will add an abbreviation
	    no_abbr = FALSE;		// reset flag that indicates there are
					// no abbreviations

	എങ്കിൽ (do_print)
	    സന്ദേശം_start();

	// Check if a new local mapping wasn't already defined globally.
	എങ്കിൽ (unique && map_table == curbuf->b_maphash
			       && haskey && hasarg && maptype != MAPTYPE_UNMAP)
	{
	    // need to loop over all global hash lists
	    വേണ്ടി (പൂർണ്ണസംഖ്യ hash = 0; hash < 256 && !got_int; ++hash)
	    {
		എങ്കിൽ (abbrev)
		{
		    എങ്കിൽ (hash != 0)	// there is only one abbreviation list
			നിർത്തുക;
		    mp = first_abbr;
		}
		അല്ലെങ്കിൽ
		    mp = maphash[hash];
		വേണ്ടി ( ; mp != ശൂന്യ && !got_int; mp = mp->m_next)
		{
		    // check entries with the same mode
		    എങ്കിൽ ((mp->m_mode & mode) != 0
			    && mp->m_keylen == len
			    && STRNCMP(mp->m_താക്കോലുകൾ, താക്കോലുകൾ, (size_t)len) == 0)
		    {
			എങ്കിൽ (abbrev)
			    semsg(
			       _(e_global_abbreviation_already_exists_for_str),
				    mp->m_താക്കോലുകൾ);
			അല്ലെങ്കിൽ
			    semsg(_(e_global_mapping_already_exists_for_str),
				    mp->m_താക്കോലുകൾ);
			retval = 5;
			പോകുക theend;
		    }
		}
	    }
	}

	// When listing global mappings, also list buffer-local ones here.
	എങ്കിൽ (map_table != curbuf->b_maphash && !hasarg
						   && maptype != MAPTYPE_UNMAP)
	    list_mappings(keyround, abbrev, haskey, താക്കോലുകൾ, len,
							     mode, &did_local);

	// Find an entry in the maphash[] list that matches.
	// For :unmap we may loop two times: once to try to unmap an entry with
	// a matching 'from' part, a second time, if the first fails, to unmap
	// an entry with a matching 'to' part. This was done to allow
	// ":ab foo bar" to be unmapped by typing ":unab foo", where "foo" will
	// be replaced by "bar" because of the abbreviation.
	അക്കം_rounds = maptype == MAPTYPE_UNMAP && !unmap_lhs_only ? 2 : 1;
	വേണ്ടി (round = 0; round < അക്കം_rounds && !did_it && !got_int; ++round)
	{
	    // need to loop over all hash lists
	    വേണ്ടി (പൂർണ്ണസംഖ്യ hash = 0; hash < 256 && !got_int; ++hash)
	    {
		എങ്കിൽ (abbrev)
		{
		    എങ്കിൽ (hash > 0)	// there is only one abbreviation list
			നിർത്തുക;
		    mpp = abbr_table;
		}
		അല്ലെങ്കിൽ
		    mpp = &(map_table[hash]);
		വേണ്ടി (mp = *mpp; mp != ശൂന്യ && !got_int; mp = *mpp)
		{

		    എങ്കിൽ ((mp->m_mode & mode) == 0)
		    {
			// skip entries with wrong mode
			mpp = &(mp->m_next);
			തുടരുക;
		    }
		    എങ്കിൽ (!haskey)	// show all entries
		    {
			എങ്കിൽ (!mp->m_simplified)
			{
			    showmap(mp, map_table != maphash);
			    did_it = TRUE;
			}
		    }
		    അല്ലെങ്കിൽ	// do we have a match?
		    {
			എങ്കിൽ (round)	// second round: Try unmap "rhs" string
			{
			    n = (പൂർണ്ണസംഖ്യ)STRLEN(mp->m_str);
			    p = mp->m_str;
			}
			അല്ലെങ്കിൽ
			{
			    n = mp->m_keylen;
			    p = mp->m_താക്കോലുകൾ;
			}
			എങ്കിൽ (STRNCMP(p, താക്കോലുകൾ, (size_t)(n < len ? n : len)) == 0)
			{
			    എങ്കിൽ (maptype == MAPTYPE_UNMAP)
			    {
				// Delete entry.
				// Only accept a full match.  For abbreviations
				// we ignore trailing space when matching with
				// the "lhs", since an abbreviation can't have
				// trailing space.
				എങ്കിൽ (n != len && (!abbrev || round || n > len
					       || *skipwhite(താക്കോലുകൾ + n) != NUL))
				{
				    mpp = &(mp->m_next);
				    തുടരുക;
				}
				// In keyround for simplified keys, don't unmap
				// a mapping without m_simplified flag.
				എങ്കിൽ (keyround1_simplified && !mp->m_simplified)
				    നിർത്തുക;
				// We reset the indicated mode bits. If nothing
				// is left the entry is deleted below.
				mp->m_mode &= ~mode;
				did_it = TRUE;	// remember we did something
			    }
			    അല്ലെങ്കിൽ എങ്കിൽ (!hasarg)	// show matching entry
			    {
				എങ്കിൽ (!mp->m_simplified)
				{
				    showmap(mp, map_table != maphash);
				    did_it = TRUE;
				}
			    }
			    അല്ലെങ്കിൽ എങ്കിൽ (n != len)	// new entry is ambiguous
			    {
				mpp = &(mp->m_next);
				തുടരുക;
			    }
			    അല്ലെങ്കിൽ എങ്കിൽ (unique)
			    {
				എങ്കിൽ (abbrev)
				    semsg(
				      _(e_abbreviation_already_exists_for_str),
					    p);
				അല്ലെങ്കിൽ
				    semsg(_(e_mapping_already_exists_for_str),
					    p);
				retval = 5;
				പോകുക theend;
			    }
			    അല്ലെങ്കിൽ
			    {
				// new rhs for existing entry
				mp->m_mode &= ~mode;	// remove mode bits
				എങ്കിൽ (mp->m_mode == 0 && !did_it) // reuse entry
				{
				    char_u *newstr = vim_strsave(rhs);

				    എങ്കിൽ (newstr == ശൂന്യ)
				    {
					retval = 4;		// no mem
					പോകുക theend;
				    }
				    എങ്കിൽ (mp->m_alt != ശൂന്യ)
					mp->m_alt = mp->m_alt->m_alt = ശൂന്യ;
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
				    mp->m_വ്യഞ്ജകം = വ്യഞ്ജകം;
				    mp->m_script_ctx = current_sctx;
				    എങ്കിൽ (cmdmod.cmod_flags & CMOD_LEGACY)
					mp->m_script_ctx.sc_version = 1;
				    mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
#endif
				    mp_result[keyround - 1] = mp;
				    did_it = TRUE;
				}
			    }
			    എങ്കിൽ (mp->m_mode == 0)  // entry can be deleted
			    {
				map_free(mpp);
				തുടരുക;	// continue with *mpp
			    }

			    // May need to put this entry into another hash
			    // list.
			    പൂർണ്ണസംഖ്യ new_hash = MAP_HASH(mp->m_mode, mp->m_താക്കോലുകൾ[0]);
			    എങ്കിൽ (!abbrev && new_hash != hash)
			    {
				*mpp = mp->m_next;
				mp->m_next = map_table[new_hash];
				map_table[new_hash] = mp;

				തുടരുക;	// continue with *mpp
			    }
			}
		    }
		    mpp = &(mp->m_next);
		}
	    }
	}

	എങ്കിൽ (maptype == MAPTYPE_UNMAP)
	{
	    // delete entry
	    എങ്കിൽ (!did_it)
	    {
		എങ്കിൽ (!keyround1_simplified)
		    retval = 2;		// no match
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (*താക്കോലുകൾ == Ctrl_C)
	    {
		// If CTRL-C has been unmapped, reuse it for Interrupting.
		എങ്കിൽ (map_table == curbuf->b_maphash)
		    curbuf->b_mapped_ctrl_c &= ~mode;
		അല്ലെങ്കിൽ
		    mapped_ctrl_c &= ~mode;
	    }
	    തുടരുക;
	}

	എങ്കിൽ (!haskey || !hasarg)
	{
	    // print entries
	    എങ്കിൽ (!did_it && !did_local)
	    {
		എങ്കിൽ (abbrev)
		    സന്ദേശം(_("No abbreviation found"));
		അല്ലെങ്കിൽ
		    സന്ദേശം(_("No mapping found"));
	    }
	    പോകുക theend;    // listing finished
	}

	എങ്കിൽ (did_it)
	    തുടരുക;	// have added the new entry already

	// Get here when adding a new entry to the maphash[] list or abbrlist.
	mp_result[keyround - 1] = map_add(map_table, abbr_table, താക്കോലുകൾ,
		    rhs, orig_rhs, noremap, nowait, silent, mode, abbrev,
#ifdef FEAT_EVAL
		    വ്യഞ്ജകം, /* sid */ 0, /* scriptversion */ 0, /* lnum */ 0,
#endif
		    keyround1_simplified);
	എങ്കിൽ (mp_result[keyround - 1] == ശൂന്യ)
	{
	    retval = 4;	    // no mem
	    പോകുക theend;
	}
    }

    എങ്കിൽ (mp_result[0] != ശൂന്യ && mp_result[1] != ശൂന്യ)
    {
	mp_result[0]->m_alt = mp_result[1];
	mp_result[1]->m_alt = mp_result[0];
    }

theend:
    vim_free(താക്കോലുകൾ_buf);
    vim_free(alt_താക്കോലുകൾ_buf);
    vim_free(arg_buf);
    തിരികെ retval;
}

/*
 * Get the mapping mode from the command name.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_map_mode(char_u **cmdp, പൂർണ്ണസംഖ്യ forceit)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		modec;
    പൂർണ്ണസംഖ്യ		mode;

    p = *cmdp;
    modec = *p++;
    എങ്കിൽ (modec == 'i')
	mode = MODE_തിരുകുക;				// :imap
    അല്ലെങ്കിൽ എങ്കിൽ (modec == 'l')
	mode = MODE_LANGMAP;				// :lmap
    അല്ലെങ്കിൽ എങ്കിൽ (modec == 'c')
	mode = MODE_CMDLINE;				// :cmap
    അല്ലെങ്കിൽ എങ്കിൽ (modec == 'n' && *p != 'o')		    // avoid :noremap
	mode = MODE_NORMAL;				// :nmap
    അല്ലെങ്കിൽ എങ്കിൽ (modec == 'v')
	mode = MODE_VISUAL | MODE_SELECT;		// :vmap
    അല്ലെങ്കിൽ എങ്കിൽ (modec == 'x')
	mode = MODE_VISUAL;				// :xmap
    അല്ലെങ്കിൽ എങ്കിൽ (modec == 's')
	mode = MODE_SELECT;			// :smap
    അല്ലെങ്കിൽ എങ്കിൽ (modec == 'o')
	mode = MODE_OP_PENDING;			// :omap
    അല്ലെങ്കിൽ എങ്കിൽ (modec == 't')
	mode = MODE_TERMINAL;			// :tmap
    അല്ലെങ്കിൽ
    {
	--p;
	എങ്കിൽ (forceit)
	    mode = MODE_തിരുകുക | MODE_CMDLINE;		// :map !
	അല്ലെങ്കിൽ
	    mode = MODE_VISUAL | MODE_SELECT | MODE_NORMAL | MODE_OP_PENDING;
							// :map
    }

    *cmdp = p;
    തിരികെ mode;
}

/*
 * Clear all mappings (":mapclear") or abbreviations (":abclear").
 * "abbr" should be FALSE for mappings, TRUE for abbreviations.
 */
    സ്ഥിതം ശൂന്യം
map_clear(
    char_u	*cmdp,
    char_u	*arg,
    പൂർണ്ണസംഖ്യ		forceit,
    പൂർണ്ണസംഖ്യ		abbr)
{
    പൂർണ്ണസംഖ്യ		mode;
    പൂർണ്ണസംഖ്യ		local;

    local = (STRCMP(arg, "<buffer>") == 0);
    എങ്കിൽ (!local && *arg != NUL)
    {
	emsg(_(e_invalid_argument));
	തിരികെ;
    }

    mode = get_map_mode(&cmdp, forceit);
    map_clear_mode(curbuf, mode, local, abbr);
}

/*
 * If "map_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_map_locked(ശൂന്യം)
{
    എങ്കിൽ (map_locked > 0)
    {
	emsg(_(e_cannot_change_mappings_while_listing));
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Clear all mappings in "mode".
 */
    ശൂന്യം
map_clear_mode(
    buf_T	*buf,		// buffer for local mappings
    പൂർണ്ണസംഖ്യ		mode,		// mode in which to delete
    പൂർണ്ണസംഖ്യ		local,		// TRUE for buffer-local mappings
    പൂർണ്ണസംഖ്യ		abbr)		// TRUE for abbreviations
{
    mapblock_T	*mp, **mpp;
    പൂർണ്ണസംഖ്യ		hash;
    പൂർണ്ണസംഖ്യ		new_hash;

    എങ്കിൽ (is_map_locked())
	തിരികെ;

    validate_maphash();

    വേണ്ടി (hash = 0; hash < 256; ++hash)
    {
	എങ്കിൽ (abbr)
	{
	    എങ്കിൽ (hash > 0)	// there is only one abbrlist
		നിർത്തുക;
	    എങ്കിൽ (local)
		mpp = &buf->b_first_abbr;
	    അല്ലെങ്കിൽ
		mpp = &first_abbr;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (local)
		mpp = &buf->b_maphash[hash];
	    അല്ലെങ്കിൽ
		mpp = &maphash[hash];
	}
	ആകുമ്പോൾ (*mpp != ശൂന്യ)
	{
	    mp = *mpp;
	    എങ്കിൽ (mp->m_mode & mode)
	    {
		mp->m_mode &= ~mode;
		എങ്കിൽ (mp->m_mode == 0) // entry can be deleted
		{
		    map_free(mpp);
		    തുടരുക;
		}
		// May need to put this entry into another hash list.
		new_hash = MAP_HASH(mp->m_mode, mp->m_താക്കോലുകൾ[0]);
		എങ്കിൽ (!abbr && new_hash != hash)
		{
		    *mpp = mp->m_next;
		    എങ്കിൽ (local)
		    {
			mp->m_next = buf->b_maphash[new_hash];
			buf->b_maphash[new_hash] = mp;
		    }
		    അല്ലെങ്കിൽ
		    {
			mp->m_next = maphash[new_hash];
			maphash[new_hash] = mp;
		    }
		    തുടരുക;		// continue with *mpp
		}
	    }
	    mpp = &(mp->m_next);
	}
    }
}

#എങ്കിൽ defined(FEAT_EVAL)
    പൂർണ്ണസംഖ്യ
mode_str2flags(char_u *modechars)
{
    പൂർണ്ണസംഖ്യ		mode = 0;

    എങ്കിൽ (vim_strchr(modechars, 'n') != ശൂന്യ)
	mode |= MODE_NORMAL;
    എങ്കിൽ (vim_strchr(modechars, 'v') != ശൂന്യ)
	mode |= MODE_VISUAL | MODE_SELECT;
    എങ്കിൽ (vim_strchr(modechars, 'x') != ശൂന്യ)
	mode |= MODE_VISUAL;
    എങ്കിൽ (vim_strchr(modechars, 's') != ശൂന്യ)
	mode |= MODE_SELECT;
    എങ്കിൽ (vim_strchr(modechars, 'o') != ശൂന്യ)
	mode |= MODE_OP_PENDING;
    എങ്കിൽ (vim_strchr(modechars, 'i') != ശൂന്യ)
	mode |= MODE_തിരുകുക;
    എങ്കിൽ (vim_strchr(modechars, 'l') != ശൂന്യ)
	mode |= MODE_LANGMAP;
    എങ്കിൽ (vim_strchr(modechars, 'c') != ശൂന്യ)
	mode |= MODE_CMDLINE;

    തിരികെ mode;
}

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "modechars".
 * Recognize termcap codes in "str".
 * Also checks mappings local to the current buffer.
 */
    പൂർണ്ണസംഖ്യ
map_to_exists(char_u *str, char_u *modechars, പൂർണ്ണസംഖ്യ abbr)
{
    char_u	*rhs;
    char_u	*buf;
    പൂർണ്ണസംഖ്യ		retval;

    rhs = replace_termcodes(str, &buf, 0, REPTERM_DO_LT, ശൂന്യ);

    retval = map_to_exists_mode(rhs, mode_str2flags(modechars), abbr);
    vim_free(buf);

    തിരികെ retval;
}
#endif

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "mode".
 * Also checks mappings local to the current buffer.
 */
    പൂർണ്ണസംഖ്യ
map_to_exists_mode(char_u *rhs, പൂർണ്ണസംഖ്യ mode, പൂർണ്ണസംഖ്യ abbr)
{
    mapblock_T	*mp;
    പൂർണ്ണസംഖ്യ		hash;
    പൂർണ്ണസംഖ്യ		exp_buffer = FALSE;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    വേണ്ടി (;;)
    {
	വേണ്ടി (hash = 0; hash < 256; ++hash)
	{
	    എങ്കിൽ (abbr)
	    {
		എങ്കിൽ (hash > 0)		// there is only one abbr list
		    നിർത്തുക;
		എങ്കിൽ (exp_buffer)
		    mp = curbuf->b_first_abbr;
		അല്ലെങ്കിൽ
		    mp = first_abbr;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (exp_buffer)
		mp = curbuf->b_maphash[hash];
	    അല്ലെങ്കിൽ
		mp = maphash[hash];
	    വേണ്ടി (; mp; mp = mp->m_next)
	    {
		എങ്കിൽ ((mp->m_mode & mode)
			&& strstr((അക്ഷരം *)mp->m_str, (അക്ഷരം *)rhs) != ശൂന്യ)
		    തിരികെ TRUE;
	    }
	}
	എങ്കിൽ (exp_buffer)
	    നിർത്തുക;
	exp_buffer = TRUE;
    }

    തിരികെ FALSE;
}

/*
 * Used below when expanding mapping/abbreviation names.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ	expand_mapmodes = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ	expand_isabbrev = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ	expand_buffer = FALSE;

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
    സ്ഥിതം char_u *
translate_mapping(char_u *str)
{
    garray_T	ga;
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		modifiers;
    പൂർണ്ണസംഖ്യ		cpo_bslash;
    പൂർണ്ണസംഖ്യ		cpo_special;

    ga_init(&ga);
    ga.ga_itemsize = 1;
    ga.ga_growsize = 40;

    cpo_bslash = (vim_strchr(p_cpo, CPO_BSLASH) != ശൂന്യ);
    cpo_special = (vim_strchr(p_cpo, CPO_SPECI) != ശൂന്യ);

    വേണ്ടി (; *str; ++str)
    {
	c = *str;
	എങ്കിൽ (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	{
	    modifiers = 0;
	    എങ്കിൽ (str[1] == KS_MODIFIER)
	    {
		str++;
		modifiers = *++str;
		c = *++str;
	    }
	    എങ്കിൽ (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	    {
		എങ്കിൽ (cpo_special)
		{
		    ga_clear(&ga);
		    തിരികെ ശൂന്യ;
		}
		c = TO_SPECIAL(str[1], str[2]);
		എങ്കിൽ (c == K_ZERO)	// display <Nul> as ^@
		    c = NUL;
		str += 2;
	    }
	    എങ്കിൽ (IS_SPECIAL(c) || modifiers)	// special key
	    {
		എങ്കിൽ (cpo_special)
		{
		    ga_clear(&ga);
		    തിരികെ ശൂന്യ;
		}
		ga_concat(&ga, get_special_താക്കോൽ_name(c, modifiers));
		തുടരുക; // for (str)
	    }
	}
	എങ്കിൽ (c == ' ' || c == '\t' || c == Ctrl_J || c == Ctrl_V
	    || (c == '<' && !cpo_special) || (c == '\\' && !cpo_bslash))
	    ga_append(&ga, cpo_bslash ? Ctrl_V : '\\');
	എങ്കിൽ (c)
	    ga_append(&ga, c);
    }
    ga_append(&ga, NUL);
    തിരികെ (char_u *)(ga.ga_data);
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
    പൂർണ്ണസംഖ്യ		forceit,	// TRUE if '!' given
    പൂർണ്ണസംഖ്യ		isabbrev,	// TRUE if abbreviation
    പൂർണ്ണസംഖ്യ		isunmap,	// TRUE if unmap/unabbrev command
    cmdidx_T	cmdidx)
{
    എങ്കിൽ (forceit && cmdidx != CMD_map && cmdidx != CMD_unmap)
	xp->xp_context = EXPAND_NOTHING;
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (isunmap)
	    expand_mapmodes = get_map_mode(&cmd, forceit || isabbrev);
	അല്ലെങ്കിൽ
	{
	    expand_mapmodes = MODE_തിരുകുക | MODE_CMDLINE;
	    എങ്കിൽ (!isabbrev)
		expand_mapmodes += MODE_VISUAL | MODE_SELECT | MODE_NORMAL
							     | MODE_OP_PENDING;
	}
	expand_isabbrev = isabbrev;
	xp->xp_context = EXPAND_MAPPINGS;
	expand_buffer = FALSE;
	വേണ്ടി (;;)
	{
	    എങ്കിൽ (STRNCMP(arg, "<buffer>", 8) == 0)
	    {
		expand_buffer = TRUE;
		arg = skipwhite(arg + 8);
		തുടരുക;
	    }
	    എങ്കിൽ (STRNCMP(arg, "<unique>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		തുടരുക;
	    }
	    എങ്കിൽ (STRNCMP(arg, "<nowait>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		തുടരുക;
	    }
	    എങ്കിൽ (STRNCMP(arg, "<silent>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		തുടരുക;
	    }
	    എങ്കിൽ (STRNCMP(arg, "<special>", 9) == 0)
	    {
		arg = skipwhite(arg + 9);
		തുടരുക;
	    }
#ifdef FEAT_EVAL
	    എങ്കിൽ (STRNCMP(arg, "<script>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		തുടരുക;
	    }
	    എങ്കിൽ (STRNCMP(arg, "<expr>", 6) == 0)
	    {
		arg = skipwhite(arg + 6);
		തുടരുക;
	    }
#endif
	    നിർത്തുക;
	}
	xp->xp_pattern = arg;
    }

    തിരികെ ശൂന്യ;
}

/*
 * Find all mapping/abbreviation names that match regexp "regmatch"'.
 * For command line expansion of ":[un]map" and ":[un]abbrev" in all modes.
 * Return OK if matches found, FAIL otherwise.
 */
    പൂർണ്ണസംഖ്യ
ExpandMappings(
    char_u	*pat,
    regmatch_T	*regmatch,
    പൂർണ്ണസംഖ്യ		*അക്കം_Matches,
    char_u	***matches)
{
    mapblock_T	*mp;
    garray_T	ga;
    പൂർണ്ണസംഖ്യ		hash;
    പൂർണ്ണസംഖ്യ		എണ്ണം;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		fuzzy;
    പൂർണ്ണസംഖ്യ		match;
    പൂർണ്ണസംഖ്യ		score = 0;
    fuzmatch_str_T  *fuzmatch;

    fuzzy = cmdline_fuzzy_complete(pat);

    validate_maphash();

    *അക്കം_Matches = 0;		    // return values in case of FAIL
    *matches = ശൂന്യ;

    എങ്കിൽ (!fuzzy)
	ga_init2(&ga, വലുപ്പം(അക്ഷരം *), 3);
    അല്ലെങ്കിൽ
	ga_init2(&ga, വലുപ്പം(fuzmatch_str_T), 3);

    // First search in map modifier arguments
    വേണ്ടി (i = 0; i < 7; ++i)
    {
	എങ്കിൽ (i == 0)
	    p = (char_u *)"<silent>";
	അല്ലെങ്കിൽ എങ്കിൽ (i == 1)
	    p = (char_u *)"<unique>";
#ifdef FEAT_EVAL
	അല്ലെങ്കിൽ എങ്കിൽ (i == 2)
	    p = (char_u *)"<script>";
	അല്ലെങ്കിൽ എങ്കിൽ (i == 3)
	    p = (char_u *)"<expr>";
#endif
	അല്ലെങ്കിൽ എങ്കിൽ (i == 4 && !expand_buffer)
	    p = (char_u *)"<buffer>";
	അല്ലെങ്കിൽ എങ്കിൽ (i == 5)
	    p = (char_u *)"<nowait>";
	അല്ലെങ്കിൽ എങ്കിൽ (i == 6)
	    p = (char_u *)"<special>";
	അല്ലെങ്കിൽ
	    തുടരുക;

	എങ്കിൽ (!fuzzy)
	    match = vim_regexec(regmatch, p, (colnr_T)0);
	അല്ലെങ്കിൽ
	{
	    score = fuzzy_match_str(p, pat);
	    match = (score != FUZZY_SCORE_NONE);
	}

	എങ്കിൽ (!match)
	    തുടരുക;

	എങ്കിൽ (ga_grow(&ga, 1) == FAIL)
	    നിർത്തുക;

	എങ്കിൽ (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = vim_strsave(p);
	    fuzmatch->score = score;
	}
	അല്ലെങ്കിൽ
	    ((char_u **)ga.ga_data)[ga.ga_len] = vim_strsave(p);
	++ga.ga_len;
    }

    വേണ്ടി (hash = 0; hash < 256; ++hash)
    {
	എങ്കിൽ (expand_isabbrev)
	{
	    എങ്കിൽ (hash > 0)	// only one abbrev list
		നിർത്തുക; // for (hash)
	    mp = first_abbr;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (expand_buffer)
	    mp = curbuf->b_maphash[hash];
	അല്ലെങ്കിൽ
	    mp = maphash[hash];
	വേണ്ടി (; mp; mp = mp->m_next)
	{
	    എങ്കിൽ (mp->m_simplified || !(mp->m_mode & expand_mapmodes))
		തുടരുക;

	    p = translate_mapping(mp->m_താക്കോലുകൾ);
	    എങ്കിൽ (p == ശൂന്യ)
		തുടരുക;

	    എങ്കിൽ (!fuzzy)
		match = vim_regexec(regmatch, p, (colnr_T)0);
	    അല്ലെങ്കിൽ
	    {
		score = fuzzy_match_str(p, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }

	    എങ്കിൽ (!match)
	    {
		vim_free(p);
		തുടരുക;
	    }

	    എങ്കിൽ (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		നിർത്തുക;
	    }

	    എങ്കിൽ (fuzzy)
	    {
		fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    അല്ലെങ്കിൽ
		((char_u **)ga.ga_data)[ga.ga_len] = p;

	    ++ga.ga_len;
	} // for (mp)
    } // for (hash)

    എങ്കിൽ (ga.ga_len == 0)
	തിരികെ FAIL;

    എങ്കിൽ (!fuzzy)
    {
	*matches = ga.ga_data;
	*അക്കം_Matches = ga.ga_len;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
							FALSE) == FAIL)
	    തിരികെ FAIL;
	*അക്കം_Matches = ga.ga_len;
    }

    എണ്ണം = *അക്കം_Matches;
    എങ്കിൽ (എണ്ണം > 1)
    {
	char_u	**ptr1;
	char_u	**ptr2;
	char_u	**ptr3;

	// Sort the matches
	// Fuzzy matching already sorts the matches
	എങ്കിൽ (!fuzzy)
	    sort_strings(*matches, എണ്ണം);

	// Remove multiple entries
	ptr1 = *matches;
	ptr2 = ptr1 + 1;
	ptr3 = ptr1 + എണ്ണം;

	ആകുമ്പോൾ (ptr2 < ptr3)
	{
	    എങ്കിൽ (STRCMP(*ptr1, *ptr2) != 0)
		*++ptr1 = *ptr2++;
	    അല്ലെങ്കിൽ
	    {
		vim_free(*ptr2++);
		എണ്ണം--;
	    }
	}
    }

    *അക്കം_Matches = എണ്ണം;
    തിരികെ (എണ്ണം == 0 ? FAIL : OK);
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
    പൂർണ്ണസംഖ്യ
check_abbr(
    പൂർണ്ണസംഖ്യ		c,
    char_u	*ptr,
    പൂർണ്ണസംഖ്യ		col,
    പൂർണ്ണസംഖ്യ		mincol)
{
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		scol;		// starting column of the abbr.
    പൂർണ്ണസംഖ്യ		j;
    char_u	*s;
    char_u	tb[MB_MAXBYTES + 4];
    mapblock_T	*mp;
    mapblock_T	*mp2;
    പൂർണ്ണസംഖ്യ		clen = 0;	// length in characters
    പൂർണ്ണസംഖ്യ		is_id = TRUE;
    പൂർണ്ണസംഖ്യ		vim_abbr;

    എങ്കിൽ (typebuf.tb_no_abbr_cnt)	// abbrev. are not recursive
	തിരികെ FALSE;

    // no remapping implies no abbreviation, except for CTRL-]
    എങ്കിൽ (noremap_താക്കോലുകൾ() && c != Ctrl_RSB)
	തിരികെ FALSE;

    // Check for word before the cursor: If it ends in a keyword char all
    // chars before it must be keyword chars or non-keyword chars, but not
    // white space. If it ends in a non-keyword char we accept any characters
    // before it except white space.
    എങ്കിൽ (col == 0)				// cannot be an abbr.
	തിരികെ FALSE;

    എങ്കിൽ (has_mbyte)
    {
	char_u *p;

	p = mb_prevptr(ptr, ptr + col);
	എങ്കിൽ (!vim_iswordp(p))
	    vim_abbr = TRUE;			// Vim added abbr.
	അല്ലെങ്കിൽ
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    എങ്കിൽ (p > ptr)
		is_id = vim_iswordp(mb_prevptr(ptr, p));
	}
	clen = 1;
	ആകുമ്പോൾ (p > ptr + mincol)
	{
	    p = mb_prevptr(ptr, p);
	    എങ്കിൽ (vim_isspace(*p) || (!vim_abbr && is_id != vim_iswordp(p)))
	    {
		p += (*mb_ptr2len)(p);
		നിർത്തുക;
	    }
	    ++clen;
	}
	scol = (പൂർണ്ണസംഖ്യ)(p - ptr);
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (!vim_iswordc(ptr[col - 1]))
	    vim_abbr = TRUE;			// Vim added abbr.
	അല്ലെങ്കിൽ
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    എങ്കിൽ (col > 1)
		is_id = vim_iswordc(ptr[col - 2]);
	}
	വേണ്ടി (scol = col - 1; scol > 0 && !vim_isspace(ptr[scol - 1])
		&& (vim_abbr || is_id == vim_iswordc(ptr[scol - 1])); --scol)
	    ;
    }

    എങ്കിൽ (scol < mincol)
	scol = mincol;
    എങ്കിൽ (scol < col)		// there is a word in front of the cursor
    {
	ptr += scol;
	len = col - scol;
	mp = curbuf->b_first_abbr;
	mp2 = first_abbr;
	എങ്കിൽ (mp == ശൂന്യ)
	{
	    mp = mp2;
	    mp2 = ശൂന്യ;
	}
	വേണ്ടി ( ; mp; mp->m_next == ശൂന്യ
				  ? (mp = mp2, mp2 = ശൂന്യ) : (mp = mp->m_next))
	{
	    പൂർണ്ണസംഖ്യ		qlen = mp->m_keylen;
	    char_u	*q = mp->m_താക്കോലുകൾ;
	    പൂർണ്ണസംഖ്യ		match;

	    എങ്കിൽ (vim_strbyte(mp->m_താക്കോലുകൾ, K_SPECIAL) != ശൂന്യ)
	    {
		char_u *qe = vim_strsave(mp->m_താക്കോലുകൾ);

		// might have CSI escaped mp->m_keys
		എങ്കിൽ (qe != ശൂന്യ)
		{
		    q = qe;
		    qlen = (പൂർണ്ണസംഖ്യ)vim_unescape_csi(q);
		}
	    }

	    // find entries with right mode and keys
	    match =    (mp->m_mode & State)
		    && qlen == len
		    && STRNCMP(q, ptr, (size_t)len) == 0;
	    എങ്കിൽ (q != mp->m_താക്കോലുകൾ)
		vim_free(q);
	    എങ്കിൽ (match)
		നിർത്തുക;
	}
	എങ്കിൽ (mp != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ	noremap;
	    പൂർണ്ണസംഖ്യ silent;
#ifdef FEAT_EVAL
	    പൂർണ്ണസംഖ്യ വ്യഞ്ജകം;
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
	    എങ്കിൽ (c != Ctrl_RSB)
	    {
					// special key code, split up
		എങ്കിൽ (IS_SPECIAL(c) || c == K_SPECIAL)
		{
		    tb[j++] = K_SPECIAL;
		    tb[j++] = K_SECOND(c);
		    tb[j++] = K_THIRD(c);
		}
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (c < ABBR_OFF && (c < ' ' || c > '~'))
			tb[j++] = Ctrl_V;	// special char needs CTRL-V
		    എങ്കിൽ (has_mbyte)
		    {
			പൂർണ്ണസംഖ്യ	newlen;
			char_u	*escaped;

			// if ABBR_OFF has been added, remove it here
			എങ്കിൽ (c >= ABBR_OFF)
			    c -= ABBR_OFF;
			newlen = (*mb_char2bytes)(c, tb + j);
			tb[j + newlen] = NUL;
			// Need to escape K_SPECIAL.
			escaped = vim_strsave_escape_csi(tb + j);
			എങ്കിൽ (escaped != ശൂന്യ)
			{
			    newlen = (പൂർണ്ണസംഖ്യ)STRLEN(escaped);
			    mch_memmove(tb + j, escaped, newlen);
			    j += newlen;
			    vim_free(escaped);
			}
		    }
		    അല്ലെങ്കിൽ
			tb[j++] = c;
		}
		tb[j] = NUL;
					// insert the last typed char
		(ശൂന്യം)ins_typebuf(tb, 1, 0, TRUE, mp->m_silent);
	    }

	    // copy values here, calling eval_map_expr() may make "mp" invalid!
	    noremap = mp->m_noremap;
	    silent = mp->m_silent;
#ifdef FEAT_EVAL
	    വ്യഞ്ജകം = mp->m_വ്യഞ്ജകം;

	    എങ്കിൽ (വ്യഞ്ജകം)
		s = eval_map_വ്യഞ്ജകം(mp, c);
	    അല്ലെങ്കിൽ
#endif
		s = mp->m_str;
	    എങ്കിൽ (s != ശൂന്യ)
	    {
					// insert the to string
		(ശൂന്യം)ins_typebuf(s, noremap, 0, TRUE, silent);
					// no abbrev. for these chars
		typebuf.tb_no_abbr_cnt += (പൂർണ്ണസംഖ്യ)STRLEN(s) + j + 1;
#ifdef FEAT_EVAL
		എങ്കിൽ (വ്യഞ്ജകം)
		    vim_free(s);
#endif
	    }

	    tb[0] = Ctrl_H;
	    tb[1] = NUL;
	    എങ്കിൽ (has_mbyte)
		len = clen;	// Delete characters instead of bytes
	    ആകുമ്പോൾ (len-- > 0)		// delete the from string
		(ശൂന്യം)ins_typebuf(tb, 1, 0, TRUE, silent);
	    തിരികെ TRUE;
	}
    }
    തിരികെ FALSE;
}

#ifdef FEAT_EVAL
/*
 * Evaluate the RHS of a mapping or abbreviations and take care of escaping
 * special characters.
 * Careful: after this "mp" will be invalid if the mapping was deleted.
 */
    char_u *
eval_map_വ്യഞ്ജകം(
    mapblock_T	*mp,
    പൂർണ്ണസംഖ്യ		c)	    // NUL or typed character for abbreviation
{
    char_u	*res;
    char_u	*p;
    char_u	*വ്യഞ്ജകം;
    pos_T	save_cursor;
    പൂർണ്ണസംഖ്യ		save_സന്ദേശം_col;
    പൂർണ്ണസംഖ്യ		save_സന്ദേശം_row;
    scid_T	save_sctx_sid = current_sctx.sc_sid;
    പൂർണ്ണസംഖ്യ		save_sctx_version = current_sctx.sc_version;

    // Remove escaping of CSI, because "str" is in a format to be used as
    // typeahead.
    വ്യഞ്ജകം = vim_strsave(mp->m_str);
    എങ്കിൽ (വ്യഞ്ജകം == ശൂന്യ)
	തിരികെ ശൂന്യ;
    vim_unescape_csi(വ്യഞ്ജകം);

    // Forbid changing text or using ":normal" to avoid most of the bad side
    // effects.  Also restore the cursor position.
    ++textlock;
    ++ex_normal_lock;
    set_vim_var_char(c);  // set v:char to the typed character
    save_cursor = curwin->w_cursor;
    save_സന്ദേശം_col = സന്ദേശം_col;
    save_സന്ദേശം_row = സന്ദേശം_row;

    current_sctx.sc_version = mp->m_script_ctx.sc_version;
    എങ്കിൽ (mp->m_script_ctx.sc_version == SCRIPT_VERSION_VIM9)
	current_sctx.sc_sid = mp->m_script_ctx.sc_sid;

    // Note: the evaluation may make "mp" invalid.
    p = eval_to_string(വ്യഞ്ജകം, FALSE, FALSE);

    --textlock;
    --ex_normal_lock;
    curwin->w_cursor = save_cursor;
    സന്ദേശം_col = save_സന്ദേശം_col;
    സന്ദേശം_row = save_സന്ദേശം_row;
    current_sctx.sc_sid = save_sctx_sid;
    current_sctx.sc_version = save_sctx_version;

    vim_free(വ്യഞ്ജകം);

    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ ശൂന്യ;
    // Escape CSI in the result to be able to use the string as typeahead.
    res = vim_strsave_escape_csi(p);
    vim_free(p);

    തിരികെ res;
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
    എങ്കിൽ (res == ശൂന്യ)
	തിരികെ ശൂന്യ;

    d = res;
    വേണ്ടി (s = p; *s != NUL; )
    {
	എങ്കിൽ ((s[0] == K_SPECIAL
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
	അല്ലെങ്കിൽ
	{
	    // Add character, possibly multi-byte to destination, escaping
	    // CSI and K_SPECIAL. Be careful, it can be an illegal byte!
	    d = add_char2buf(PTR2CHAR(s), d);
	    s += MB_CPTR2LEN(s);
	}
    }
    *d = NUL;

    തിരികെ res;
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

    ആകുമ്പോൾ (*s != NUL)
    {
	എങ്കിൽ (s[0] == K_SPECIAL && s[1] == KS_SPECIAL && s[2] == KE_FILLER)
	{
	    *d++ = K_SPECIAL;
	    s += 3;
	}
	അല്ലെങ്കിൽ എങ്കിൽ ((s[0] == K_SPECIAL || s[0] == CSI)
				   && s[1] == KS_EXTRA && s[2] == (പൂർണ്ണസംഖ്യ)KE_CSI)
	{
	    *d++ = CSI;
	    s += 3;
	}
	അല്ലെങ്കിൽ
	    *d++ = *s++;
    }
    *d = NUL;
    തിരികെ (size_t)(d - p);
}

/*
 * Write map commands for the current mappings to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    പൂർണ്ണസംഖ്യ
makemap(
    FILE	*fd,
    buf_T	*buf)	    // buffer for local mappings or NULL
{
    mapblock_T	*mp;
    char_u	c1, c2, c3;
    char_u	*p;
    അക്ഷരം	*cmd;
    പൂർണ്ണസംഖ്യ		abbr;
    പൂർണ്ണസംഖ്യ		hash;
    പൂർണ്ണസംഖ്യ		did_cpo = FALSE;
    പൂർണ്ണസംഖ്യ		i;

    validate_maphash();

    // Do the loop twice: Once for mappings, once for abbreviations.
    // Then loop over all map hash lists.
    വേണ്ടി (abbr = 0; abbr < 2; ++abbr)
	വേണ്ടി (hash = 0; hash < 256; ++hash)
	{
	    എങ്കിൽ (abbr)
	    {
		എങ്കിൽ (hash > 0)		// there is only one abbr list
		    നിർത്തുക;
		എങ്കിൽ (buf != ശൂന്യ)
		    mp = buf->b_first_abbr;
		അല്ലെങ്കിൽ
		    mp = first_abbr;
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (buf != ശൂന്യ)
		    mp = buf->b_maphash[hash];
		അല്ലെങ്കിൽ
		    mp = maphash[hash];
	    }

	    വേണ്ടി ( ; mp; mp = mp->m_next)
	    {
		// simplified map blocks are not created explicitly
		എങ്കിൽ (mp->m_simplified)
		    തുടരുക;

		// skip script-local mappings
		എങ്കിൽ (mp->m_noremap == REMAP_SCRIPT)
		    തുടരുക;

		// skip mappings that contain a <SNR> (script-local thing),
		// they probably don't work when loaded again
		വേണ്ടി (p = mp->m_str; *p != NUL; ++p)
		    എങ്കിൽ (p[0] == K_SPECIAL && p[1] == KS_EXTRA
						       && p[2] == (പൂർണ്ണസംഖ്യ)KE_SNR)
			നിർത്തുക;
		എങ്കിൽ (*p != NUL)
		    തുടരുക;

		// It's possible to create a mapping and then ":unmap" certain
		// modes.  We recreate this here by mapping the individual
		// modes, which requires up to three of them.
		c1 = NUL;
		c2 = NUL;
		c3 = NUL;
		എങ്കിൽ (abbr)
		    cmd = "abbr";
		അല്ലെങ്കിൽ
		    cmd = "map";
		തിരഞ്ഞെടുപ്പ് (mp->m_mode)
		{
		    അവസ്ഥ MODE_NORMAL | MODE_VISUAL | MODE_SELECT
							     | MODE_OP_PENDING:
			നിർത്തുക;
		    അവസ്ഥ MODE_NORMAL:
			c1 = 'n';
			നിർത്തുക;
		    അവസ്ഥ MODE_VISUAL:
			c1 = 'x';
			നിർത്തുക;
		    അവസ്ഥ MODE_SELECT:
			c1 = 's';
			നിർത്തുക;
		    അവസ്ഥ MODE_OP_PENDING:
			c1 = 'o';
			നിർത്തുക;
		    അവസ്ഥ MODE_NORMAL | MODE_VISUAL:
			c1 = 'n';
			c2 = 'x';
			നിർത്തുക;
		    അവസ്ഥ MODE_NORMAL | MODE_SELECT:
			c1 = 'n';
			c2 = 's';
			നിർത്തുക;
		    അവസ്ഥ MODE_NORMAL | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 'o';
			നിർത്തുക;
		    അവസ്ഥ MODE_VISUAL | MODE_SELECT:
			c1 = 'v';
			നിർത്തുക;
		    അവസ്ഥ MODE_VISUAL | MODE_OP_PENDING:
			c1 = 'x';
			c2 = 'o';
			നിർത്തുക;
		    അവസ്ഥ MODE_SELECT | MODE_OP_PENDING:
			c1 = 's';
			c2 = 'o';
			നിർത്തുക;
		    അവസ്ഥ MODE_NORMAL | MODE_VISUAL | MODE_SELECT:
			c1 = 'n';
			c2 = 'v';
			നിർത്തുക;
		    അവസ്ഥ MODE_NORMAL | MODE_VISUAL | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 'x';
			c3 = 'o';
			നിർത്തുക;
		    അവസ്ഥ MODE_NORMAL | MODE_SELECT | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 's';
			c3 = 'o';
			നിർത്തുക;
		    അവസ്ഥ MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING:
			c1 = 'v';
			c2 = 'o';
			നിർത്തുക;
		    അവസ്ഥ MODE_CMDLINE | MODE_തിരുകുക:
			എങ്കിൽ (!abbr)
			    cmd = "map!";
			നിർത്തുക;
		    അവസ്ഥ MODE_CMDLINE:
			c1 = 'c';
			നിർത്തുക;
		    അവസ്ഥ MODE_തിരുകുക:
			c1 = 'i';
			നിർത്തുക;
		    അവസ്ഥ MODE_LANGMAP:
			c1 = 'l';
			നിർത്തുക;
		    അവസ്ഥ MODE_TERMINAL:
			c1 = 't';
			നിർത്തുക;
		    സ്ഥിരസ്ഥിതി:
			iemsg(e_makemap_illegal_mode);
			തിരികെ FAIL;
		}
		ചെയ്യുക	// do this twice if c2 is set, 3 times with c3
		{
		    // When outputting <> form, need to make sure that 'cpo'
		    // is set to the Vim default.
		    എങ്കിൽ (!did_cpo)
		    {
			എങ്കിൽ (*mp->m_str == NUL)		// will use <Nop>
			    did_cpo = TRUE;
			അല്ലെങ്കിൽ
			    വേണ്ടി (i = 0; i < 2; ++i)
				വേണ്ടി (p = (i ? mp->m_str : mp->m_താക്കോലുകൾ); *p; ++p)
				    എങ്കിൽ (*p == K_SPECIAL || *p == NL)
					did_cpo = TRUE;
			എങ്കിൽ (did_cpo)
			{
			    എങ്കിൽ (fprintf(fd, "cpo_save = &cpo") < 0
				    || put_eol(fd) < 0
				    || fprintf(fd, "set cpo&vim") < 0
				    || put_eol(fd) < 0)
				തിരികെ FAIL;
			}
		    }
#ifdef FEAT_EVAL
		    // If it is not vim9 use legacy
		    എങ്കിൽ (mp->m_വ്യഞ്ജകം && mp->m_script_ctx.sc_version < SCRIPT_VERSION_VIM9
			    && fputs("legacy ", fd) < 0)
			തിരികെ FAIL;
#endif
		    എങ്കിൽ (c1 && putc(c1, fd) < 0)
			തിരികെ FAIL;
		    എങ്കിൽ (mp->m_noremap != REMAP_YES && fprintf(fd, "nore") < 0)
			തിരികെ FAIL;
		    എങ്കിൽ (fputs(cmd, fd) < 0)
			തിരികെ FAIL;
		    എങ്കിൽ (buf != ശൂന്യ && fputs(" <buffer>", fd) < 0)
			തിരികെ FAIL;
		    എങ്കിൽ (mp->m_nowait && fputs(" <nowait>", fd) < 0)
			തിരികെ FAIL;
		    എങ്കിൽ (mp->m_silent && fputs(" <silent>", fd) < 0)
			തിരികെ FAIL;
#ifdef FEAT_EVAL
		    എങ്കിൽ (mp->m_noremap == REMAP_SCRIPT
						 && fputs("<script>", fd) < 0)
			തിരികെ FAIL;
		    എങ്കിൽ (mp->m_വ്യഞ്ജകം && fputs(" <expr>", fd) < 0)
			തിരികെ FAIL;
#endif

		    എങ്കിൽ (       putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_താക്കോലുകൾ, 0) == FAIL
			    || putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_str, 1) == FAIL
			    || put_eol(fd) < 0)
			തിരികെ FAIL;
		    c1 = c2;
		    c2 = c3;
		    c3 = NUL;
		} ആകുമ്പോൾ (c1 != NUL);
	    }
	}

    എങ്കിൽ (did_cpo)
	എങ്കിൽ (fprintf(fd, "&cpo = cpo_save") < 0
		|| put_eol(fd) < 0)
	    തിരികെ FAIL;
    തിരികെ OK;
}

/*
 * write escape string to file
 * "what": 0 for :map lhs, 1 for :map rhs, 2 for :set
 *
 * return FAIL for failure, OK otherwise
 */
    പൂർണ്ണസംഖ്യ
put_escstr(FILE *fd, char_u *strstart, പൂർണ്ണസംഖ്യ what)
{
    char_u	*str = strstart;
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		modifiers;

    // :map xx <Nop>
    എങ്കിൽ (*str == NUL && what == 1)
    {
	എങ്കിൽ (fprintf(fd, "<Nop>") < 0)
	    തിരികെ FAIL;
	തിരികെ OK;
    }

    വേണ്ടി ( ; *str != NUL; ++str)
    {
	char_u	*p;

	// Check for a multi-byte character, which may contain escaped
	// K_SPECIAL and CSI bytes
	p = mb_unescape(&str);
	എങ്കിൽ (p != ശൂന്യ)
	{
	    ആകുമ്പോൾ (*p != NUL)
		എങ്കിൽ (fputc(*p++, fd) < 0)
		    തിരികെ FAIL;
	    --str;
	    തുടരുക;
	}

	c = *str;
	// Special key codes have to be translated to be able to make sense
	// when they are read back.
	എങ്കിൽ (c == K_SPECIAL && what != 2)
	{
	    modifiers = 0;
	    എങ്കിൽ (str[1] == KS_MODIFIER)
	    {
		modifiers = str[2];
		str += 3;

		// Modifiers can be applied too to multi-byte characters.
		p = mb_unescape(&str);

		എങ്കിൽ (p == ശൂന്യ)
		    c = *str;
		അല്ലെങ്കിൽ
		{
		    // retrieve codepoint (character number) from unescaped string
		    c = (*mb_ptr2char)(p);
		    --str;
		}
	    }
	    എങ്കിൽ (c == K_SPECIAL)
	    {
		c = TO_SPECIAL(str[1], str[2]);
		str += 2;
	    }
	    എങ്കിൽ (IS_SPECIAL(c) || modifiers)	// special key
	    {
		എങ്കിൽ (fputs((അക്ഷരം *)get_special_താക്കോൽ_name(c, modifiers), fd) < 0)
		    തിരികെ FAIL;
		തുടരുക;
	    }
	}

	// A '\n' in a map command should be written as <NL>.
	// A '\n' in a set command should be written as \^V^J.
	എങ്കിൽ (c == NL)
	{
	    എങ്കിൽ (what == 2)
	    {
		എങ്കിൽ (fprintf(fd, "\\\026\n") < 0)
		    തിരികെ FAIL;
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (fprintf(fd, "<NL>") < 0)
		    തിരികെ FAIL;
	    }
	    തുടരുക;
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
	എങ്കിൽ (what == 2 && (VIM_ISWHITE(c) || c == '"' || c == '\\'))
	{
	    എങ്കിൽ (putc('\\', fd) < 0)
		തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (c < ' ' || c > '~' || c == '|'
		|| (what == 0 && c == ' ')
		|| (what == 1 && str == strstart && c == ' ')
		|| (what != 2 && c == '<'))
	{
	    എങ്കിൽ (putc(Ctrl_V, fd) < 0)
		തിരികെ FAIL;
	}
	എങ്കിൽ (putc(c, fd) < 0)
	    തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Check all mappings for the presence of special key codes.
 * Used after ":set term=xxx".
 */
    ശൂന്യം
check_map_keycodes(ശൂന്യം)
{
    mapblock_T	*mp;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		i;
    char_u	buf[3];
    പൂർണ്ണസംഖ്യ		abbr;
    പൂർണ്ണസംഖ്യ		hash;
    buf_T	*bp;
    ESTACK_CHECK_DECLARATION;

    validate_maphash();
    // avoids giving error messages
    estack_തള്ളുക(ETYPE_INTERNAL, (char_u *)"mappings", 0);
    ESTACK_CHECK_SETUP;

    // Do this once for each buffer, and then once for global
    // mappings/abbreviations with bp == NULL
    വേണ്ടി (bp = firstbuf; ; bp = bp->b_next)
    {
	// Do the loop twice: Once for mappings, once for abbreviations.
	// Then loop over all map hash lists.
	വേണ്ടി (abbr = 0; abbr <= 1; ++abbr)
	    വേണ്ടി (hash = 0; hash < 256; ++hash)
	    {
		എങ്കിൽ (abbr)
		{
		    എങ്കിൽ (hash)	    // there is only one abbr list
			നിർത്തുക;
		    എങ്കിൽ (bp != ശൂന്യ)
			mp = bp->b_first_abbr;
		    അല്ലെങ്കിൽ
			mp = first_abbr;
		}
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (bp != ശൂന്യ)
			mp = bp->b_maphash[hash];
		    അല്ലെങ്കിൽ
			mp = maphash[hash];
		}
		വേണ്ടി ( ; mp != ശൂന്യ; mp = mp->m_next)
		{
		    വേണ്ടി (i = 0; i <= 1; ++i)	// do this twice
		    {
			എങ്കിൽ (i == 0)
			    p = mp->m_താക്കോലുകൾ;	// once for the "from" part
			അല്ലെങ്കിൽ
			    p = mp->m_str;	// and once for the "to" part
			ആകുമ്പോൾ (*p)
			{
			    എങ്കിൽ (*p == K_SPECIAL)
			    {
				++p;
				എങ്കിൽ (*p < 128)   // for "normal" tcap entries
				{
				    buf[0] = p[0];
				    buf[1] = p[1];
				    buf[2] = NUL;
				    (ശൂന്യം)add_termcap_entry(buf, FALSE);
				}
				++p;
			    }
			    ++p;
			}
		    }
		}
	    }
	എങ്കിൽ (bp == ശൂന്യ)
	    നിർത്തുക;
    }
    ESTACK_CHECK_NOW;
    estack_എടുക്കുക();
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Check the string "keys" against the lhs of all mappings.
 * Return pointer to rhs of mapping (mapblock->m_str).
 * NULL when no mapping found.
 */
    char_u *
check_map(
    char_u	*താക്കോലുകൾ,
    പൂർണ്ണസംഖ്യ		mode,
    പൂർണ്ണസംഖ്യ		exact,		// require exact match
    പൂർണ്ണസംഖ്യ		ign_mod,	// ignore preceding modifier
    പൂർണ്ണസംഖ്യ		abbr,		// do abbreviations
    mapblock_T	**mp_ptr,	// return: pointer to mapblock or NULL
    പൂർണ്ണസംഖ്യ		*local_ptr)	// return: buffer-local mapping or NULL
{
    പൂർണ്ണസംഖ്യ		hash;
    പൂർണ്ണസംഖ്യ		len, minlen;
    mapblock_T	*mp;
    char_u	*s;
    പൂർണ്ണസംഖ്യ		local;

    validate_maphash();

    len = (പൂർണ്ണസംഖ്യ)STRLEN(താക്കോലുകൾ);
    വേണ്ടി (local = 1; local >= 0; --local)
	// loop over all hash lists
	വേണ്ടി (hash = 0; hash < 256; ++hash)
	{
	    എങ്കിൽ (abbr)
	    {
		എങ്കിൽ (hash > 0)		// there is only one list.
		    നിർത്തുക;
		എങ്കിൽ (local)
		    mp = curbuf->b_first_abbr;
		അല്ലെങ്കിൽ
		    mp = first_abbr;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (local)
		mp = curbuf->b_maphash[hash];
	    അല്ലെങ്കിൽ
		mp = maphash[hash];
	    വേണ്ടി ( ; mp != ശൂന്യ; mp = mp->m_next)
	    {
		// skip entries with wrong mode, wrong length and not matching
		// ones
		എങ്കിൽ ((mp->m_mode & mode) && (!exact || mp->m_keylen == len))
		{
		    എങ്കിൽ (len > mp->m_keylen)
			minlen = mp->m_keylen;
		    അല്ലെങ്കിൽ
			minlen = len;
		    s = mp->m_താക്കോലുകൾ;
		    എങ്കിൽ (ign_mod && s[0] == K_SPECIAL && s[1] == KS_MODIFIER
							       && s[2] != NUL)
		    {
			s += 3;
			എങ്കിൽ (len > mp->m_keylen - 3)
			    minlen = mp->m_keylen - 3;
		    }
		    എങ്കിൽ (STRNCMP(s, താക്കോലുകൾ, minlen) == 0)
		    {
			എങ്കിൽ (mp_ptr != ശൂന്യ)
			    *mp_ptr = mp;
			എങ്കിൽ (local_ptr != ശൂന്യ)
			    *local_ptr = local;
			തിരികെ mp->m_str;
		    }
		}
	    }
	}

    തിരികെ ശൂന്യ;
}

/*
 * "hasmapto()" function
 */
    ശൂന്യം
f_hasmapto(typval_T *argvars, typval_T *rettv)
{
    char_u	*name;
    char_u	*mode;
    char_u	buf[NUMBUFLEN];
    പൂർണ്ണസംഖ്യ		abbr = FALSE;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	തിരികെ;

    name = tv_get_string(&argvars[0]);
    എങ്കിൽ (argvars[1].v_type == VAR_UNKNOWN)
	mode = (char_u *)"nvo";
    അല്ലെങ്കിൽ
    {
	mode = tv_get_string_buf(&argvars[1], buf);
	എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
	    abbr = (പൂർണ്ണസംഖ്യ)tv_get_bool(&argvars[2]);
    }

    എങ്കിൽ (map_to_exists(name, mode, abbr))
	rettv->vval.v_സംഖ്യ = TRUE;
    അല്ലെങ്കിൽ
	rettv->vval.v_സംഖ്യ = FALSE;
}

/*
 * Fill in the empty dictionary with items as defined by maparg builtin.
 */
    സ്ഥിതം ശൂന്യം
mapblock2dict(
	mapblock_T  *mp,
	dict_T	    *dict,
	char_u	    *lhsrawalt,	    // may be NULL
	പൂർണ്ണസംഖ്യ	    buffer_local,   // false if not buffer local mapping
	പൂർണ്ണസംഖ്യ	    abbr)	    // true if abbreviation
{
    char_u	    *lhs = str2special_save(mp->m_താക്കോലുകൾ, TRUE, FALSE);
    char_u	    *mapmode = map_mode_to_chars(mp->m_mode);

    dict_add_string(dict, "lhs", lhs);
    vim_free(lhs);
    dict_add_string(dict, "lhsraw", mp->m_താക്കോലുകൾ);
    എങ്കിൽ (lhsrawalt)
	// Also add the value for the simplified entry.
	dict_add_string(dict, "lhsrawalt", lhsrawalt);
    dict_add_string(dict, "rhs", mp->m_orig_str);
    dict_add_സംഖ്യ(dict, "noremap", mp->m_noremap ? 1L : 0L);
    dict_add_സംഖ്യ(dict, "script", mp->m_noremap == REMAP_SCRIPT
		    ? 1L : 0L);
    dict_add_സംഖ്യ(dict, "expr", mp->m_വ്യഞ്ജകം ? 1L : 0L);
    dict_add_സംഖ്യ(dict, "silent", mp->m_silent ? 1L : 0L);
    dict_add_സംഖ്യ(dict, "sid", (long)mp->m_script_ctx.sc_sid);
    dict_add_സംഖ്യ(dict, "scriptversion",
		    (long)mp->m_script_ctx.sc_version);
    dict_add_സംഖ്യ(dict, "lnum", (long)mp->m_script_ctx.sc_lnum);
    dict_add_സംഖ്യ(dict, "buffer", (long)buffer_local);
    dict_add_സംഖ്യ(dict, "nowait", mp->m_nowait ? 1L : 0L);
    dict_add_string(dict, "mode", mapmode);
    dict_add_സംഖ്യ(dict, "abbr", abbr ? 1L : 0L);
    dict_add_സംഖ്യ(dict, "mode_bits", mp->m_mode);

    vim_free(mapmode);
}

    സ്ഥിതം ശൂന്യം
get_maparg(typval_T *argvars, typval_T *rettv, പൂർണ്ണസംഖ്യ exact)
{
    char_u	*താക്കോലുകൾ;
    char_u	*താക്കോലുകൾ_simplified;
    char_u	*which;
    char_u	buf[NUMBUFLEN];
    char_u	*താക്കോലുകൾ_buf = ശൂന്യ;
    char_u	*alt_താക്കോലുകൾ_buf = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		did_simplify = FALSE;
    char_u	*rhs;
    പൂർണ്ണസംഖ്യ		mode;
    പൂർണ്ണസംഖ്യ		abbr = FALSE;
    പൂർണ്ണസംഖ്യ		get_dict = FALSE;
    mapblock_T	*mp = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		buffer_local;
    പൂർണ്ണസംഖ്യ		flags = REPTERM_FROM_PART | REPTERM_DO_LT;

    // return empty string for failure
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;

    താക്കോലുകൾ = tv_get_string(&argvars[0]);
    എങ്കിൽ (*താക്കോലുകൾ == NUL)
	തിരികെ;

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	which = tv_get_string_buf_chk(&argvars[1], buf);
	എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    abbr = (പൂർണ്ണസംഖ്യ)tv_get_bool(&argvars[2]);
	    എങ്കിൽ (argvars[3].v_type != VAR_UNKNOWN)
		get_dict = (പൂർണ്ണസംഖ്യ)tv_get_bool(&argvars[3]);
	}
    }
    അല്ലെങ്കിൽ
	which = (char_u *)"";
    എങ്കിൽ (which == ശൂന്യ)
	തിരികെ;

    mode = get_map_mode(&which, 0);

    താക്കോലുകൾ_simplified = replace_termcodes(താക്കോലുകൾ, &താക്കോലുകൾ_buf, 0, flags,
								&did_simplify);
    rhs = check_map(താക്കോലുകൾ_simplified, mode, exact, FALSE, abbr,
							   &mp, &buffer_local);
    എങ്കിൽ (did_simplify)
    {
	// When the lhs is being simplified the not-simplified keys are
	// preferred for printing, like in do_map().
	(ശൂന്യം)replace_termcodes(താക്കോലുകൾ, &alt_താക്കോലുകൾ_buf, 0,
					flags | REPTERM_NO_SIMPLIFY, ശൂന്യ);
	rhs = check_map(alt_താക്കോലുകൾ_buf, mode, exact, FALSE, abbr, &mp,
								&buffer_local);
    }

    എങ്കിൽ (!get_dict)
    {
	// Return a string.
	എങ്കിൽ (rhs != ശൂന്യ)
	{
	    എങ്കിൽ (*rhs == NUL)
		rettv->vval.v_string = vim_strsave((char_u *)"<Nop>");
	    അല്ലെങ്കിൽ
		rettv->vval.v_string = str2special_save(rhs, FALSE, FALSE);
	}

    }
    അല്ലെങ്കിൽ എങ്കിൽ (rettv_dict_alloc(rettv) == OK && rhs != ശൂന്യ)
	mapblock2dict(mp, rettv->vval.v_dict,
			  did_simplify ? താക്കോലുകൾ_simplified : ശൂന്യ,
			  buffer_local, abbr);

    vim_free(താക്കോലുകൾ_buf);
    vim_free(alt_താക്കോലുകൾ_buf);
}

/*
 * "maplist()" function
 */
    ശൂന്യം
f_maplist(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    mapblock_T	*mp;
    പൂർണ്ണസംഖ്യ		buffer_local;
    char_u	*താക്കോലുകൾ_buf;
    പൂർണ്ണസംഖ്യ		did_simplify;
    പൂർണ്ണസംഖ്യ		hash;
    char_u	*lhs;
    സ്ഥിരം പൂർണ്ണസംഖ്യ	flags = REPTERM_FROM_PART | REPTERM_DO_LT;
    പൂർണ്ണസംഖ്യ		abbr = FALSE;

    എങ്കിൽ (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	തിരികെ;
    എങ്കിൽ (argvars[0].v_type != VAR_UNKNOWN)
	abbr = tv_get_bool(&argvars[0]);

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    വേണ്ടി (buffer_local = 0; buffer_local <= 1; ++buffer_local)
    {
	വേണ്ടി (hash = 0; hash < 256; ++hash)
	{
	    എങ്കിൽ (abbr)
	    {
		എങ്കിൽ (hash > 0)		// there is only one abbr list
		    നിർത്തുക;
		എങ്കിൽ (buffer_local)
		    mp = curbuf->b_first_abbr;
		അല്ലെങ്കിൽ
		    mp = first_abbr;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (buffer_local)
		mp = curbuf->b_maphash[hash];
	    അല്ലെങ്കിൽ
		mp = maphash[hash];
	    വേണ്ടി (; mp; mp = mp->m_next)
	    {
		എങ്കിൽ (mp->m_simplified)
		    തുടരുക;
		എങ്കിൽ ((d = dict_alloc()) == ശൂന്യ)
		    തിരികെ;
		എങ്കിൽ (list_append_dict(rettv->vval.v_list, d) == FAIL)
		{
		    dict_unref(d);
		    തിരികെ;
		}

		താക്കോലുകൾ_buf = ശൂന്യ;
		did_simplify = FALSE;

		lhs = str2special_save(mp->m_താക്കോലുകൾ, TRUE, FALSE);
		(ശൂന്യം)replace_termcodes(lhs, &താക്കോലുകൾ_buf, 0, flags,
								&did_simplify);
		vim_free(lhs);

		mapblock2dict(mp, d,
				 did_simplify ? താക്കോലുകൾ_buf : ശൂന്യ,
				 buffer_local, abbr);
		vim_free(താക്കോലുകൾ_buf);
	    }
	}
    }
}

/*
 * "maparg()" function
 */
    ശൂന്യം
f_maparg(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
		തിരികെ;

    get_maparg(argvars, rettv, TRUE);
}

/*
 * "mapcheck()" function
 */
    ശൂന്യം
f_mapcheck(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	തിരികെ;

    get_maparg(argvars, rettv, FALSE);
}

/*
 * Get the mapping mode from the mode string.
 * It may contain multiple characters, eg "nox", or "!", or ' '
 * Return 0 if there is an error.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_map_mode_string(char_u *mode_string, പൂർണ്ണസംഖ്യ abbr)
{
    char_u	*p = mode_string;
    പൂർണ്ണസംഖ്യ		mode = 0;
    പൂർണ്ണസംഖ്യ		tmode;
    പൂർണ്ണസംഖ്യ		modec;
    സ്ഥിരം പൂർണ്ണസംഖ്യ	MASK_V = MODE_VISUAL | MODE_SELECT;
    സ്ഥിരം പൂർണ്ണസംഖ്യ	MASK_MAP = MODE_VISUAL | MODE_SELECT | MODE_NORMAL
							     | MODE_OP_PENDING;
    സ്ഥിരം പൂർണ്ണസംഖ്യ	MASK_BANG = MODE_തിരുകുക | MODE_CMDLINE;

    എങ്കിൽ (*p == NUL)
	p = (char_u *)" ";	// compatibility
    ആകുമ്പോൾ ((modec = *p++))
    {
	തിരഞ്ഞെടുപ്പ് (modec)
	{
	    അവസ്ഥ 'i': tmode = MODE_തിരുകുക;	നിർത്തുക;
	    അവസ്ഥ 'l': tmode = MODE_LANGMAP;	നിർത്തുക;
	    അവസ്ഥ 'c': tmode = MODE_CMDLINE;	നിർത്തുക;
	    അവസ്ഥ 'n': tmode = MODE_NORMAL;	നിർത്തുക;
	    അവസ്ഥ 'x': tmode = MODE_VISUAL;	നിർത്തുക;
	    അവസ്ഥ 's': tmode = MODE_SELECT;	നിർത്തുക;
	    അവസ്ഥ 'o': tmode = MODE_OP_PENDING;	നിർത്തുക;
	    അവസ്ഥ 't': tmode = MODE_TERMINAL;	നിർത്തുക;
	    അവസ്ഥ 'v': tmode = MASK_V;		നിർത്തുക;
	    അവസ്ഥ '!': tmode = MASK_BANG;	നിർത്തുക;
	    അവസ്ഥ ' ': tmode = MASK_MAP;		നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:
		      തിരികെ 0; // error, unknown mode character
	}
	mode |= tmode;
    }
    എങ്കിൽ ((abbr && (mode & ~MASK_BANG) != 0)
	|| (!abbr && (mode & (mode-1)) != 0 // more than one bit set
	    && (
		// false if multiple bits set in mode and mode is fully
		// contained in one mask
		!(((mode & MASK_BANG) != 0 && (mode & ~MASK_BANG) == 0)
		    || ((mode & MASK_MAP) != 0 && (mode & ~MASK_MAP) == 0)))))
	തിരികെ 0;

    തിരികെ mode;
}

/*
 * "mapset()" function
 */
    ശൂന്യം
f_mapset(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*which;
    പൂർണ്ണസംഖ്യ		mode;
    char_u	buf[NUMBUFLEN];
    പൂർണ്ണസംഖ്യ		is_abbr;
    dict_T	*d;
    char_u	*lhs;
    char_u	*lhsraw;
    char_u	*lhsrawalt;
    char_u	*rhs;
    char_u	*orig_rhs;
    char_u	*arg_buf = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		noremap;
    പൂർണ്ണസംഖ്യ		വ്യഞ്ജകം;
    പൂർണ്ണസംഖ്യ		silent;
    പൂർണ്ണസംഖ്യ		buffer;
    scid_T	sid;
    പൂർണ്ണസംഖ്യ		scriptversion;
    linenr_T	lnum;
    mapblock_T	**map_table = maphash;
    mapblock_T  **abbr_table = &first_abbr;
    പൂർണ്ണസംഖ്യ		nowait;
    char_u	*arg;
    പൂർണ്ണസംഖ്യ		dict_only;
    mapblock_T	*mp_result[2] = {ശൂന്യ, ശൂന്യ};

    എങ്കിൽ (check_secure())
	തിരികെ;

    // If first arg is a dict, then that's the only arg permitted.
    dict_only = argvars[0].v_type == VAR_DICT;
    എങ്കിൽ (in_vim9script()
	    && (check_for_string_or_dict_arg(argvars, 0) == FAIL
		|| (dict_only && check_for_unknown_arg(argvars, 1) == FAIL)
		|| (!dict_only
		    && (check_for_string_arg(argvars, 0) == FAIL
			|| check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_dict_arg(argvars, 2) == FAIL))))
	തിരികെ;

    എങ്കിൽ (dict_only)
    {
	d = argvars[0].vval.v_dict;
	which = dict_get_string(d, "mode", FALSE);
	is_abbr = dict_get_bool(d, "abbr", -1);
	എങ്കിൽ (which == ശൂന്യ || is_abbr < 0)
	{
	    emsg(_(e_entries_missing_in_mapset_dict_argument));
	    തിരികെ;
	}
    }
    അല്ലെങ്കിൽ
    {
	which = tv_get_string_buf_chk(&argvars[0], buf);
	എങ്കിൽ (which == ശൂന്യ)
	    തിരികെ;
	is_abbr = (പൂർണ്ണസംഖ്യ)tv_get_bool(&argvars[1]);

	എങ്കിൽ (check_for_dict_arg(argvars, 2) == FAIL)
	    തിരികെ;
	d = argvars[2].vval.v_dict;
    }
    mode = get_map_mode_string(which, is_abbr);
    എങ്കിൽ (mode == 0)
    {
	semsg(_(e_illegal_map_mode_string_str), which);
	തിരികെ;
    }


    // Get the values in the same order as above in get_maparg().
    lhs = dict_get_string(d, "lhs", FALSE);
    lhsraw = dict_get_string(d, "lhsraw", FALSE);
    lhsrawalt = dict_get_string(d, "lhsrawalt", FALSE);
    rhs = dict_get_string(d, "rhs", FALSE);
    എങ്കിൽ (lhs == ശൂന്യ || lhsraw == ശൂന്യ || rhs == ശൂന്യ)
    {
	emsg(_(e_entries_missing_in_mapset_dict_argument));
	തിരികെ;
    }
    orig_rhs = rhs;

    noremap = dict_get_സംഖ്യ(d, "noremap") ? REMAP_NONE: 0;
    എങ്കിൽ (dict_get_സംഖ്യ(d, "script") != 0)
	noremap = REMAP_SCRIPT;
    വ്യഞ്ജകം = dict_get_സംഖ്യ(d, "expr") != 0;
    silent = dict_get_സംഖ്യ(d, "silent") != 0;
    sid = dict_get_സംഖ്യ(d, "sid");
    scriptversion = dict_get_സംഖ്യ(d, "scriptversion");
    lnum = dict_get_സംഖ്യ(d, "lnum");
    buffer = dict_get_സംഖ്യ(d, "buffer");
    nowait = dict_get_സംഖ്യ(d, "nowait") != 0;
    // mode from the dict is not used

    എങ്കിൽ (STRICMP(rhs, "<nop>") == 0)	// "<Nop>" means nothing
	rhs = (char_u *)"";
    അല്ലെങ്കിൽ
	rhs = replace_termcodes(rhs, &arg_buf, sid,
					REPTERM_DO_LT | REPTERM_SPECIAL, ശൂന്യ);

    എങ്കിൽ (buffer)
    {
	map_table = curbuf->b_maphash;
	abbr_table = &curbuf->b_first_abbr;
    }

    // Delete any existing mapping for this lhs and mode.
    എങ്കിൽ (buffer)
    {
	arg = alloc(STRLEN(lhs) + STRLEN("<buffer>") + 1);
	എങ്കിൽ (arg == ശൂന്യ)
	    തിരികെ;
	STRCPY(arg, "<buffer>");
	STRCPY(arg + 8, lhs);
    }
    അല്ലെങ്കിൽ
    {
	arg = vim_strsave(lhs);
	എങ്കിൽ (arg == ശൂന്യ)
	    തിരികെ;
    }
    do_map(MAPTYPE_UNMAP_LHS, arg, mode, is_abbr);
    vim_free(arg);

    mp_result[0] = map_add(map_table, abbr_table, lhsraw, rhs, orig_rhs,
			    noremap, nowait, silent, mode, is_abbr, വ്യഞ്ജകം, sid,
			    scriptversion, lnum, 0);
    എങ്കിൽ (lhsrawalt != ശൂന്യ)
	mp_result[1] = map_add(map_table, abbr_table, lhsrawalt, rhs, orig_rhs,
			    noremap, nowait, silent, mode, is_abbr, വ്യഞ്ജകം, sid,
			    scriptversion, lnum, 1);

    എങ്കിൽ (mp_result[0] != ശൂന്യ && mp_result[1] != ശൂന്യ)
    {
	mp_result[0]->m_alt = mp_result[1];
	mp_result[1]->m_alt = mp_result[0];
    }

    vim_free(arg_buf);
}
#endif


#എങ്കിൽ defined(MSWIN) || defined(MACOS_X)

# define VIS_SEL	(MODE_VISUAL | MODE_SELECT)	// abbreviation

/*
 * Default mappings for some often used keys.
 */
ഘടന initmap
{
    char_u	*arg;
    പൂർണ്ണസംഖ്യ		mode;
};

# ifdef FEAT_GUI_MSWIN
// Use the Windows (CUA) keybindings. (GUI)
സ്ഥിതം ഘടന initmap initmappings[] =
{
	// paste, copy and cut
	{(char_u *)"<S-Insert> \"*P", MODE_NORMAL},
	{(char_u *)"<S-Insert> \"-d\"*P", VIS_SEL},
	{(char_u *)"<S-Insert> <C-R><C-O>*", MODE_തിരുകുക | MODE_CMDLINE},
	{(char_u *)"<C-Insert> \"*y", VIS_SEL},
	{(char_u *)"<S-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-X> \"*d", VIS_SEL},
	// Missing: CTRL-C (cancel) and CTRL-V (block selection)
};
# endif

# എങ്കിൽ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
// Use the Windows (CUA) keybindings. (Console)
സ്ഥിതം ഘടന initmap cinitmappings[] =
{
	{(char_u *)"\316w <C-Home>", MODE_NORMAL | VIS_SEL},
	{(char_u *)"\316w <C-Home>", MODE_തിരുകുക | MODE_CMDLINE},
	{(char_u *)"\316u <C-End>", MODE_NORMAL | VIS_SEL},
	{(char_u *)"\316u <C-End>", MODE_തിരുകുക | MODE_CMDLINE},

	// paste, copy and cut
#  ifdef FEAT_CLIPBOARD
	{(char_u *)"\316\324 \"*P", MODE_NORMAL},   // SHIFT-Insert is "*P
	{(char_u *)"\316\324 \"-d\"*P", VIS_SEL},   // SHIFT-Insert is "-d"*P
	{(char_u *)"\316\324 \022\017*", MODE_തിരുകുക},  // SHIFT-Insert is ^R^O*
	{(char_u *)"\316\325 \"*y", VIS_SEL},	    // CTRL-Insert is "*y
	{(char_u *)"\316\327 \"*d", VIS_SEL},	    // SHIFT-Del is "*d
	{(char_u *)"\316\330 \"*d", VIS_SEL},	    // CTRL-Del is "*d
	{(char_u *)"\030 \"*d", VIS_SEL},	    // CTRL-X is "*d
#  അല്ലെങ്കിൽ
	{(char_u *)"\316\324 P", MODE_NORMAL},	    // SHIFT-Insert is P
	{(char_u *)"\316\324 \"-dP", VIS_SEL},	    // SHIFT-Insert is "-dP
	{(char_u *)"\316\324 \022\017\"", MODE_തിരുകുക}, // SHIFT-Insert is ^R^O"
	{(char_u *)"\316\325 y", VIS_SEL},	    // CTRL-Insert is y
	{(char_u *)"\316\327 d", VIS_SEL},	    // SHIFT-Del is d
	{(char_u *)"\316\330 d", VIS_SEL},	    // CTRL-Del is d
#  endif
};
# endif

# എങ്കിൽ defined(MACOS_X)
സ്ഥിതം ഘടന initmap initmappings[] =
{
	// Use the Standard MacOS binding.
	// paste, copy and cut
	{(char_u *)"<D-v> \"*P", MODE_NORMAL},
	{(char_u *)"<D-v> \"-d\"*P", VIS_SEL},
	{(char_u *)"<D-v> <C-R>*", MODE_തിരുകുക | MODE_CMDLINE},
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
    ശൂന്യം
init_mappings(ശൂന്യം)
{
#എങ്കിൽ defined(MSWIN) || defined(MACOS_X)
    പൂർണ്ണസംഖ്യ		i;

# എങ്കിൽ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    എങ്കിൽ (!gui.starting)
#  endif
    {
	വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(cinitmappings); ++i)
	    add_map(cinitmappings[i].arg, cinitmappings[i].mode, FALSE);
    }
# endif
# എങ്കിൽ defined(FEAT_GUI_MSWIN) || defined(MACOS_X)
    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(initmappings); ++i)
	add_map(initmappings[i].arg, initmappings[i].mode, FALSE);
# endif
#endif
}

/*
 * Add a mapping "map" for mode "mode".
 * When "nore" is TRUE use MAPTYPE_NOREMAP.
 * Need to put string in allocated memory, because do_map() will modify it.
 */
    ശൂന്യം
add_map(char_u *map, പൂർണ്ണസംഖ്യ mode, പൂർണ്ണസംഖ്യ nore)
{
    char_u	*s;
    char_u	*cpo_save = p_cpo;

    p_cpo = empty_option;	// Allow <> notation
    s = vim_strsave(map);
    എങ്കിൽ (s != ശൂന്യ)
    {
	(ശൂന്യം)do_map(nore ? MAPTYPE_NOREMAP : MAPTYPE_MAP, s, mode, FALSE);
	vim_free(s);
    }
    p_cpo = cpo_save;
}

#എങ്കിൽ defined(FEAT_LANGMAP)
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
തരനിർവചനം ഘടന
{
    പൂർണ്ണസംഖ്യ	    from;
    പൂർണ്ണസംഖ്യ     to;
} langmap_entry_T;

സ്ഥിതം garray_T langmap_mapga;

/*
 * Search for an entry in "langmap_mapga" for "from".  If found set the "to"
 * field.  If not found insert a new entry at the appropriate location.
 */
    സ്ഥിതം ശൂന്യം
langmap_set_entry(പൂർണ്ണസംഖ്യ from, പൂർണ്ണസംഖ്യ to)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_data);
    പൂർണ്ണസംഖ്യ		    a = 0;
    പൂർണ്ണസംഖ്യ		    b = langmap_mapga.ga_len;

    // Do a binary search for an existing entry.
    ആകുമ്പോൾ (a != b)
    {
	പൂർണ്ണസംഖ്യ i = (a + b) / 2;
	പൂർണ്ണസംഖ്യ d = entries[i].from - from;

	എങ്കിൽ (d == 0)
	{
	    entries[i].to = to;
	    തിരികെ;
	}
	എങ്കിൽ (d < 0)
	    a = i + 1;
	അല്ലെങ്കിൽ
	    b = i;
    }

    എങ്കിൽ (ga_grow(&langmap_mapga, 1) == FAIL)
	തിരികെ;  // out of memory

    // insert new entry at position "a"
    entries = (langmap_entry_T *)(langmap_mapga.ga_data) + a;
    mch_memmove(entries + 1, entries,
			(langmap_mapga.ga_len - a) * വലുപ്പം(langmap_entry_T));
    ++langmap_mapga.ga_len;
    entries[0].from = from;
    entries[0].to = to;
}

/*
 * Apply 'langmap' to multi-byte character "c" and return the result.
 */
    പൂർണ്ണസംഖ്യ
langmap_adjust_mb(പൂർണ്ണസംഖ്യ c)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_data);
    പൂർണ്ണസംഖ്യ a = 0;
    പൂർണ്ണസംഖ്യ b = langmap_mapga.ga_len;

    ആകുമ്പോൾ (a != b)
    {
	പൂർണ്ണസംഖ്യ i = (a + b) / 2;
	പൂർണ്ണസംഖ്യ d = entries[i].from - c;

	എങ്കിൽ (d == 0)
	    തിരികെ entries[i].to;  // found matching entry
	എങ്കിൽ (d < 0)
	    a = i + 1;
	അല്ലെങ്കിൽ
	    b = i;
    }
    തിരികെ c;  // no entry found, return "c" unmodified
}

    ശൂന്യം
langmap_init(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ i;

    വേണ്ടി (i = 0; i < 256; i++)
	langmap_mapchar[i] = i;	 // we init with a one-to-one map
    ga_init2(&langmap_mapga, വലുപ്പം(langmap_entry_T), 8);
}

/*
 * Called when langmap option is set; the language map can be
 * changed at any time!
 */
    അക്ഷരം *
did_set_langmap(optset_T *args UNUSED)
{
    char_u  *p;
    char_u  *p2;
    പൂർണ്ണസംഖ്യ	    from, to;

    ga_clear(&langmap_mapga);		    // clear the previous map first
    langmap_init();			    // back to one-to-one map

    വേണ്ടി (p = p_langmap; p[0] != NUL; )
    {
	വേണ്ടി (p2 = p; p2[0] != NUL && p2[0] != ',' && p2[0] != ';';
							       MB_PTR_ADV(p2))
	{
	    എങ്കിൽ (p2[0] == '\\' && p2[1] != NUL)
		++p2;
	}
	എങ്കിൽ (p2[0] == ';')
	    ++p2;	    // abcd;ABCD form, p2 points to A
	അല്ലെങ്കിൽ
	    p2 = ശൂന്യ;	    // aAbBcCdD form, p2 is NULL
	ആകുമ്പോൾ (p[0])
	{
	    എങ്കിൽ (p[0] == ',')
	    {
		++p;
		നിർത്തുക;
	    }
	    എങ്കിൽ (p[0] == '\\' && p[1] != NUL)
		++p;
	    from = (*mb_ptr2char)(p);
	    to = NUL;
	    എങ്കിൽ (p2 == ശൂന്യ)
	    {
		MB_PTR_ADV(p);
		എങ്കിൽ (p[0] != ',')
		{
		    എങ്കിൽ (p[0] == '\\')
			++p;
		    to = (*mb_ptr2char)(p);
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (p2[0] != ',')
		{
		    എങ്കിൽ (p2[0] == '\\')
			++p2;
		    to = (*mb_ptr2char)(p2);
		}
	    }
	    എങ്കിൽ (to == NUL)
	    {
		sprintf(args->os_errbuf,
			_(e_langmap_matching_character_missing_for_str),
			transchar(from));
		തിരികെ args->os_errbuf;
	    }

	    എങ്കിൽ (from >= 256)
		langmap_set_entry(from, to);
	    അല്ലെങ്കിൽ
		langmap_mapchar[from & 255] = to;

	    // Advance to next pair
	    MB_PTR_ADV(p);
	    എങ്കിൽ (p2 != ശൂന്യ)
	    {
		MB_PTR_ADV(p2);
		എങ്കിൽ (*p == ';')
		{
		    p = p2;
		    എങ്കിൽ (p[0] != NUL)
		    {
			എങ്കിൽ (p[0] != ',')
			{
			    vim_snprintf(args->os_errbuf, args->os_errbuflen,
				    _(e_langmap_extra_characters_after_semicolon_str),
				    p);
			    തിരികെ args->os_errbuf;
			}
			++p;
		    }
		    നിർത്തുക;
		}
	    }
	}
    }

    തിരികെ ശൂന്യ;
}
#endif

    സ്ഥിതം ശൂന്യം
do_exmap(exarg_T *eap, പൂർണ്ണസംഖ്യ isabbrev)
{
    പൂർണ്ണസംഖ്യ	    mode;
    char_u  *cmdp;

    cmdp = eap->cmd;
    mode = get_map_mode(&cmdp, eap->forceit || isabbrev);

    തിരഞ്ഞെടുപ്പ് (do_map(*cmdp == 'n' ? MAPTYPE_NOREMAP
				: *cmdp == 'u' ? MAPTYPE_UNMAP : MAPTYPE_MAP,
						    eap->arg, mode, isabbrev))
    {
	അവസ്ഥ 1: emsg(_(e_invalid_argument));
		നിർത്തുക;
	അവസ്ഥ 2: emsg((isabbrev ? _(e_no_such_abbreviation)
						      : _(e_no_such_mapping)));
		നിർത്തുക;
    }
}

/*
 * ":abbreviate" and friends.
 */
    ശൂന്യം
ex_abbreviate(exarg_T *eap)
{
    do_exmap(eap, TRUE);	// almost the same as mapping
}

/*
 * ":map" and friends.
 */
    ശൂന്യം
ex_map(exarg_T *eap)
{
    // If we are sourcing .exrc or .vimrc in current directory we
    // print the mappings for security reasons.
    എങ്കിൽ (secure)
    {
	secure = 2;
	സന്ദേശം_outtrans(eap->cmd);
	സന്ദേശം_putchar('\n');
    }
    do_exmap(eap, FALSE);
}

/*
 * ":unmap" and friends.
 */
    ശൂന്യം
ex_unmap(exarg_T *eap)
{
    do_exmap(eap, FALSE);
}

/*
 * ":mapclear" and friends.
 */
    ശൂന്യം
ex_mapclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, eap->forceit, FALSE);
}

/*
 * ":abclear" and friends.
 */
    ശൂന്യം
ex_abclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, TRUE, TRUE);
}
