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

#include "vim.h"

/*
 * List used for abbreviations.
 */
static mapblock_T	*first_abbr = NULL; // first entry in abbrlist

/*
 * Each mapping is put in one of the 256 hash lists, to speed up finding it.
 */
static mapblock_T	*(maphash[256]);
static puur_naa_nka		maphash_valid = FALSE;

// When non-zero then no mappings can be added or removed.  Prevents mappings
// to change while listing them.
static puur_naa_nka		map_locked = 0;

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
get_maphash_list(puur_naa_nka state, puur_naa_nka c)
{
    ꦮꦭꦶ maphash[MAP_HASH(state, c)];
}

/*
 * Get the buffer-local hashed map list for "state" and first character "c".
 */
    mapblock_T *
get_buf_maphash_list(puur_naa_nka state, puur_naa_nka c)
{
    ꦮꦭꦶ curbuf->b_maphash[MAP_HASH(state, c)];
}

    puur_naa_nka
is_maphash_valid(void)
{
    ꦮꦭꦶ maphash_valid;
}

/*
 * Initialize maphash[] for first use.
 */
    static void
validate_maphash(void)
{
    ꦪꦼꦤ꧀ (maphash_valid)
	ꦮꦭꦶ;

    CLEAR_FIELD(maphash);
    maphash_valid = TRUE;
}

/*
 * Delete one entry from the abbrlist or maphash[].
 * "mpp" is a pointer to the m_next field of the PREVIOUS entry!
 */
    static void
map_free(mapblock_T **mpp)
{
    mapblock_T	*mp;

    mp = *mpp;
    vim_free(mp->m_keys);
    ꦪꦼꦤ꧀ (mp->m_alt != NULL)
	mp->m_alt->m_alt = NULL;
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
    static char_u *
map_mode_to_chars(puur_naa_nka mode)
{
    garray_T    mapmode;

    ga_init2(&mapmode, 1, 7);

    ꦪꦼꦤ꧀ ((mode & (MODE_INSERT | MODE_CMDLINE)) == (MODE_INSERT | MODE_CMDLINE))
	ga_append(&mapmode, '!');			// :map!
    ꦭꦶꦪ ꦪꦼꦤ꧀ (mode & MODE_INSERT)
	ga_append(&mapmode, 'i');			// :imap
    ꦭꦶꦪ ꦪꦼꦤ꧀ (mode & MODE_LANGMAP)
	ga_append(&mapmode, 'l');			// :lmap
    ꦭꦶꦪ ꦪꦼꦤ꧀ (mode & MODE_CMDLINE)
	ga_append(&mapmode, 'c');			// :cmap
    ꦭꦶꦪ ꦪꦼꦤ꧀ ((mode
		 & (MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING))
		== (MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING))
	ga_append(&mapmode, ' ');			// :map
    ꦭꦶꦪ
    {
	ꦪꦼꦤ꧀ (mode & MODE_NORMAL)
	    ga_append(&mapmode, 'n');			// :nmap
	ꦪꦼꦤ꧀ (mode & MODE_OP_PENDING)
	    ga_append(&mapmode, 'o');			// :omap
	ꦪꦼꦤ꧀ (mode & MODE_TERMINAL)
	    ga_append(&mapmode, 't');			// :tmap
	ꦪꦼꦤ꧀ ((mode & (MODE_VISUAL | MODE_SELECT)) == (MODE_VISUAL | MODE_SELECT))
	    ga_append(&mapmode, 'v');			// :vmap
	ꦭꦶꦪ
	{
	    ꦪꦼꦤ꧀ (mode & MODE_VISUAL)
		ga_append(&mapmode, 'x');		// :xmap
	    ꦪꦼꦤ꧀ (mode & MODE_SELECT)
		ga_append(&mapmode, 's');		// :smap
	}
    }

    ga_append(&mapmode, NUL);
    ꦮꦭꦶ (char_u *)mapmode.ga_data;
}

/*
 * Output a line for one mapping.
 */
    static void
showmap(
    mapblock_T	*mp,
    puur_naa_nka		local)	    // TRUE for buffer-local map
{
    puur_naa_nka		len = 1;
    char_u	*mapchars;

    ꦪꦼꦤ꧀ (message_filtered(mp->m_keys) && message_filtered(mp->m_str))
	ꦮꦭꦶ;

    // Prevent mappings to be cleared while at the more prompt.
    // Must jump to "theend" instead of returning.
    ++map_locked;

    ꦪꦼꦤ꧀ (msg_didout || msg_silent != 0)
    {
	msg_putchar('\n');
	ꦪꦼꦤ꧀ (got_int)	    // 'q' typed at MORE prompt
	    goto theend;
    }

    mapchars = map_mode_to_chars(mp->m_mode);
    ꦪꦼꦤ꧀ (mapchars != NULL)
    {
	msg_puts((var_na *)mapchars);
	len = (puur_naa_nka)STRLEN(mapchars);
	vim_free(mapchars);
    }

    ꦱꦼꦢꦼꦭ (++len <= 3)
	msg_putchar(' ');

    // Display the LHS.  Get length of what we write.
    len = msg_outtrans_special(mp->m_keys, TRUE, 0);
    do
    {
	msg_putchar(' ');		// pad with blanks
	++len;
    } ꦱꦼꦢꦼꦭ (len < 12);

    ꦪꦼꦤ꧀ (mp->m_noremap == REMAP_NONE)
	msg_puts_attr("*", HL_ATTR(HLF_8));
    ꦭꦶꦪ ꦪꦼꦤ꧀ (mp->m_noremap == REMAP_SCRIPT)
	msg_puts_attr("&", HL_ATTR(HLF_8));
    ꦭꦶꦪ
	msg_putchar(' ');

    ꦪꦼꦤ꧀ (local)
	msg_putchar('@');
    ꦭꦶꦪ
	msg_putchar(' ');

    // Use FALSE below if we only want things like <Up> to show up as such on
    // the rhs, and not M-x etc, TRUE gets both -- webb
    ꦪꦼꦤ꧀ (*mp->m_str == NUL)
	msg_puts_attr("<Nop>", HL_ATTR(HLF_8));
    ꦭꦶꦪ
	msg_outtrans_special(mp->m_str, FALSE, 0);
#ifdef FEAT_EVAL
    ꦪꦼꦤ꧀ (p_verbose > 0)
	last_set_msg(mp->m_script_ctx);
#endif
    msg_clr_eos();
    out_flush();			// show one line at a time

theend:
    --map_locked;
}

    static mapblock_T *
map_add(
	mapblock_T  **map_table,
	mapblock_T  **abbr_table,
	char_u	    *keys,
	char_u	    *rhs,
	char_u	    *orig_rhs,
	puur_naa_nka	    noremap,
	puur_naa_nka	    nowait,
	puur_naa_nka	    silent,
	puur_naa_nka	    mode,
	puur_naa_nka	    is_abbr,
#ifdef FEAT_EVAL
	puur_naa_nka	    expr,
	scid_T	    sid,	    // 0 to use current_sctx
	puur_naa_nka	    scriptversion,
	linenr_T    lnum,
#endif
	puur_naa_nka	    simplified)
{
    mapblock_T	*mp = ALLOC_CLEAR_ONE(mapblock_T);

    ꦪꦼꦤ꧀ (mp == NULL)
	ꦮꦭꦶ NULL;

    // If CTRL-C has been mapped, don't always use it for Interrupting.
    ꦪꦼꦤ꧀ (*keys == Ctrl_C)
    {
	ꦪꦼꦤ꧀ (map_table == curbuf->b_maphash)
	    curbuf->b_mapped_ctrl_c |= mode;
	ꦭꦶꦪ
	    mapped_ctrl_c |= mode;
    }

    mp->m_keys = vim_strsave(keys);
    mp->m_str = vim_strsave(rhs);
    mp->m_orig_str = vim_strsave(orig_rhs);
    ꦪꦼꦤ꧀ (mp->m_keys == NULL || mp->m_str == NULL)
    {
	vim_free(mp->m_keys);
	vim_free(mp->m_str);
	vim_free(mp->m_orig_str);
	vim_free(mp);
	ꦮꦭꦶ NULL;
    }
    mp->m_keylen = (puur_naa_nka)STRLEN(mp->m_keys);
    mp->m_noremap = noremap;
    mp->m_nowait = nowait;
    mp->m_silent = silent;
    mp->m_mode = mode;
    mp->m_simplified = simplified;
#ifdef FEAT_EVAL
    mp->m_expr = expr;
    ꦪꦼꦤ꧀ (sid != 0)
    {
	mp->m_script_ctx.sc_sid = sid;
	mp->m_script_ctx.sc_lnum = lnum;
	mp->m_script_ctx.sc_version = scriptversion;
    }
    ꦭꦶꦪ
    {
	mp->m_script_ctx = current_sctx;
	ꦪꦼꦤ꧀ (cmdmod.cmod_flags & CMOD_LEGACY)
	    mp->m_script_ctx.sc_version = 1;
	mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
    }
#endif

    // add the new entry in front of the abbrlist or maphash[] list
    ꦪꦼꦤ꧀ (is_abbr)
    {
	mp->m_next = *abbr_table;
	*abbr_table = mp;
    }
    ꦭꦶꦪ
    {
	puur_naa_nka n = MAP_HASH(mp->m_mode, mp->m_keys[0]);

	mp->m_next = map_table[n];
	map_table[n] = mp;
    }
    ꦮꦭꦶ mp;
}

/*
 * List mappings.  When "haskey" is FALSE all mappings, otherwise mappings that
 * match "keys[keys_len]".
 */
    static void
list_mappings(
	puur_naa_nka	keyround,
	puur_naa_nka	abbrev,
	puur_naa_nka	haskey,
	char_u	*keys,
	puur_naa_nka	keys_len,
	puur_naa_nka	mode,
	puur_naa_nka	*did_local)
{
    // Prevent mappings to be cleared while at the more prompt.
    ++map_locked;

    ꦪꦼꦤ꧀ (p_verbose > 0 && keyround == 1)
    {
	ꦪꦼꦤ꧀ (seenModifyOtherKeys)
	    msg_puts(_("Seen modifyOtherKeys: true\n"));

	ꦪꦼꦤ꧀ (modify_otherkeys_state != MOKS_INITIAL)
	{
	    var_na *name = _("Unknown");
	    switch (modify_otherkeys_state)
	    {
		pak_sa MOKS_INITIAL: bha_nga;
		pak_sa MOKS_OFF: name = _("Off"); bha_nga;
		pak_sa MOKS_ENABLED: name = _("On"); bha_nga;
		pak_sa MOKS_DISABLED: name = _("Disabled"); bha_nga;
		pak_sa MOKS_AFTER_T_TE: name = _("Cleared"); bha_nga;
	    }

	    var_na buf[200];
	    vim_snprintf(buf, parimaa_na(buf),
				    _("modifyOtherKeys detected: %s\n"), name);
	    msg_puts(buf);
	}

	ꦪꦼꦤ꧀ (kitty_protocol_state != KKPS_INITIAL)
	{
	    var_na *name = _("Unknown");
	    switch (kitty_protocol_state)
	    {
		pak_sa KKPS_INITIAL: bha_nga;
		pak_sa KKPS_OFF: name = _("Off"); bha_nga;
		pak_sa KKPS_ENABLED: name = _("On"); bha_nga;
		pak_sa KKPS_DISABLED: name = _("Disabled"); bha_nga;
		pak_sa KKPS_AFTER_T_TE: name = _("Cleared"); bha_nga;
	    }

	    var_na buf[200];
	    vim_snprintf(buf, parimaa_na(buf),
				     _("Kitty keyboard protocol: %s\n"), name);
	    msg_puts(buf);
	}
    }

    // need to loop over all global hash lists
    ꦏꦁꦒꦺꦴ (puur_naa_nka hash = 0; hash < 256 && !got_int; ++hash)
    {
	mapblock_T	*mp;

	ꦪꦼꦤ꧀ (abbrev)
	{
	    ꦪꦼꦤ꧀ (hash != 0)	// there is only one abbreviation list
		bha_nga;
	    mp = curbuf->b_first_abbr;
	}
	ꦭꦶꦪ
	    mp = curbuf->b_maphash[hash];
	ꦏꦁꦒꦺꦴ ( ; mp != NULL && !got_int; mp = mp->m_next)
	{
	    // check entries with the same mode
	    ꦪꦼꦤ꧀ (!mp->m_simplified && (mp->m_mode & mode) != 0)
	    {
		ꦪꦼꦤ꧀ (!haskey)		    // show all entries
		{
		    showmap(mp, TRUE);
		    *did_local = TRUE;
		}
		ꦭꦶꦪ
		{
		    puur_naa_nka n = mp->m_keylen;
		    ꦪꦼꦤ꧀ (STRNCMP(mp->m_keys, keys,
				   (size_t)(n < keys_len ? n : keys_len)) == 0)
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
    puur_naa_nka
do_map(
    puur_naa_nka		maptype,
    char_u	*arg,
    puur_naa_nka		mode,
    puur_naa_nka		abbrev)		// not a mapping but an abbreviation
{
    char_u	*keys;
    mapblock_T	*mp, **mpp;
    mapblock_T	*mp_result[2] = {NULL, NULL};
    char_u	*rhs;
    char_u	*p;
    puur_naa_nka		n;
    puur_naa_nka		len = 0;	// init for GCC
    puur_naa_nka		hasarg;
    puur_naa_nka		haskey;
    puur_naa_nka		do_print;
    puur_naa_nka		keyround;
    char_u	*keys_buf = NULL;
    char_u	*alt_keys_buf = NULL;
    char_u	*arg_buf = NULL;
    puur_naa_nka		retval = 0;
    puur_naa_nka		do_backslash;
    mapblock_T	**abbr_table;
    mapblock_T	**map_table;
    puur_naa_nka		unique = FALSE;
    puur_naa_nka		nowait = FALSE;
    puur_naa_nka		silent = FALSE;
    puur_naa_nka		special = FALSE;
#ifdef FEAT_EVAL
    puur_naa_nka		expr = FALSE;
#endif
    puur_naa_nka		did_simplify = FALSE;
    puur_naa_nka		unmap_lhs_only = FALSE;
    puur_naa_nka		noremap;
    char_u      *orig_rhs;

    keys = arg;
    map_table = maphash;
    abbr_table = &first_abbr;

    ꦪꦼꦤ꧀ (maptype == MAPTYPE_UNMAP_LHS)
    {
	unmap_lhs_only = TRUE;
	maptype = MAPTYPE_UNMAP;
    }

    // For ":noremap" don't remap, otherwise do remap.
    ꦪꦼꦤ꧀ (maptype == MAPTYPE_NOREMAP)
	noremap = REMAP_NONE;
    ꦭꦶꦪ
	noremap = REMAP_YES;

    // Accept <buffer>, <nowait>, <silent>, <expr> <script> and <unique> in
    // any order.
    ꦏꦁꦒꦺꦴ (;;)
    {
	// Check for "<buffer>": mapping local to buffer.
	ꦪꦼꦤ꧀ (STRNCMP(keys, "<buffer>", 8) == 0)
	{
	    keys = skipwhite(keys + 8);
	    map_table = curbuf->b_maphash;
	    abbr_table = &curbuf->b_first_abbr;
	    continue;
	}

	// Check for "<nowait>": don't wait for more characters.
	ꦪꦼꦤ꧀ (STRNCMP(keys, "<nowait>", 8) == 0)
	{
	    keys = skipwhite(keys + 8);
	    nowait = TRUE;
	    continue;
	}

	// Check for "<silent>": don't echo commands.
	ꦪꦼꦤ꧀ (STRNCMP(keys, "<silent>", 8) == 0)
	{
	    keys = skipwhite(keys + 8);
	    silent = TRUE;
	    continue;
	}

	// Check for "<special>": accept special keys in <>
	ꦪꦼꦤ꧀ (STRNCMP(keys, "<special>", 9) == 0)
	{
	    keys = skipwhite(keys + 9);
	    special = TRUE;
	    continue;
	}

#ifdef FEAT_EVAL
	// Check for "<script>": remap script-local mappings only
	ꦪꦼꦤ꧀ (STRNCMP(keys, "<script>", 8) == 0)
	{
	    keys = skipwhite(keys + 8);
	    noremap = REMAP_SCRIPT;
	    continue;
	}

	// Check for "<expr>": {rhs} is an expression.
	ꦪꦼꦤ꧀ (STRNCMP(keys, "<expr>", 6) == 0)
	{
	    keys = skipwhite(keys + 6);
	    expr = TRUE;
	    continue;
	}
#endif
	// Check for "<unique>": don't overwrite an existing mapping.
	ꦪꦼꦤ꧀ (STRNCMP(keys, "<unique>", 8) == 0)
	{
	    keys = skipwhite(keys + 8);
	    unique = TRUE;
	    continue;
	}
	bha_nga;
    }

    validate_maphash();

    // Find end of keys and skip CTRL-Vs (and backslashes) in it.
    // Accept backslash like CTRL-V when 'cpoptions' does not contain 'B'.
    // with :unmap white space is included in the keys, no argument possible.
    p = keys;
    do_backslash = (vim_strchr(p_cpo, CPO_BSLASH) == NULL);
    ꦱꦼꦢꦼꦭ (*p && (maptype == MAPTYPE_UNMAP || !VIM_ISWHITE(*p)))
    {
	ꦪꦼꦤ꧀ ((p[0] == Ctrl_V || (do_backslash && p[0] == '\\')) &&
								  p[1] != NUL)
	    ++p;		// skip CTRL-V or backslash
	++p;
    }
    ꦪꦼꦤ꧀ (*p != NUL)
	*p++ = NUL;

    p = skipwhite(p);
    rhs = p;
    hasarg = (*rhs != NUL);
    haskey = (*keys != NUL);
    do_print = !haskey || (maptype != MAPTYPE_UNMAP && !hasarg);

    // check for :unmap without argument
    ꦪꦼꦤ꧀ (maptype == MAPTYPE_UNMAP && !haskey)
    {
	retval = 1;
	goto theend;
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
    ꦪꦼꦤ꧀ (haskey)
    {
	char_u	*new_keys;
	puur_naa_nka	flags = REPTERM_FROM_PART | REPTERM_DO_LT;

	ꦪꦼꦤ꧀ (special)
	    flags |= REPTERM_SPECIAL;
	new_keys = replace_termcodes(keys, &keys_buf, 0, flags, &did_simplify);
	ꦪꦼꦤ꧀ (did_simplify)
	    (void)replace_termcodes(keys, &alt_keys_buf, 0,
					    flags | REPTERM_NO_SIMPLIFY, NULL);
	keys = new_keys;
    }
    orig_rhs = rhs;
    ꦪꦼꦤ꧀ (hasarg)
    {
	ꦪꦼꦤ꧀ (STRICMP(rhs, "<nop>") == 0)	    // "<Nop>" means nothing
	    rhs = (char_u *)"";
	ꦭꦶꦪ
	    rhs = replace_termcodes(rhs, &arg_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), NULL);
    }

    /*
     * The following is done twice if we have two versions of keys:
     * "alt_keys_buf" is not NULL.
     */
    ꦏꦁꦒꦺꦴ (keyround = 1; keyround <= 2; ++keyround)
    {
	puur_naa_nka	did_it = FALSE;
	puur_naa_nka	did_local = FALSE;
	puur_naa_nka	keyround1_simplified = keyround == 1 && did_simplify;
	puur_naa_nka	round;
	puur_naa_nka	num_rounds;

	ꦪꦼꦤ꧀ (keyround == 2)
	{
	    ꦪꦼꦤ꧀ (alt_keys_buf == NULL)
		bha_nga;
	    keys = alt_keys_buf;
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (alt_keys_buf != NULL && do_print)
	    // when printing always use the not-simplified map
	    keys = alt_keys_buf;

	// check arguments and translate function keys
	ꦪꦼꦤ꧀ (haskey)
	{
	    len = (puur_naa_nka)STRLEN(keys);
	    ꦪꦼꦤ꧀ (len > MAXMAPLEN)	// maximum length of MAXMAPLEN chars
	    {
		retval = 1;
		goto theend;
	    }

	    ꦪꦼꦤ꧀ (abbrev && maptype != MAPTYPE_UNMAP)
	    {
		// If an abbreviation ends in a keyword character, the
		// rest must be all keyword-char or all non-keyword-char.
		// Otherwise we won't be able to find the start of it in a
		// vi-compatible way.
		ꦪꦼꦤ꧀ (has_mbyte)
		{
		    puur_naa_nka		first, last;
		    puur_naa_nka		same = -1;
		    char_u	keys_unescaped[MAXMAPLEN + 1];
		    size_t	keys_unescaped_len;

		    mch_memmove(keys_unescaped, keys, (size_t)(len + 1));
		    keys_unescaped_len = vim_unescape_csi(keys_unescaped);
		    p = keys_unescaped;

		    first = vim_iswordp(p);
		    last = first;
		    MB_PTR_ADV(p);
		    n = 1;
		    ꦱꦼꦢꦼꦭ (p < keys_unescaped + keys_unescaped_len)
		    {
			++n;			// nr of (multi-byte) chars
			last = vim_iswordp(p);	// type of last char
			ꦪꦼꦤ꧀ (same == -1 && last != first)
			    same = n - 1;	// count of same char type
			MB_PTR_ADV(p);
		    }
		    ꦪꦼꦤ꧀ (last && n > 2 && same >= 0 && same < n - 1)
		    {
			retval = 1;
			goto theend;
		    }
		}
		ꦭꦶꦪ ꦪꦼꦤ꧀ (vim_iswordc(keys[len - 1]))
		    // ends in keyword char
		    ꦏꦁꦒꦺꦴ (n = 0; n < len - 2; ++n)
			ꦪꦼꦤ꧀ (vim_iswordc(keys[n]) != vim_iswordc(keys[len - 2]))
			{
			    retval = 1;
			    goto theend;
			}
		// An abbreviation cannot contain white space.
		ꦏꦁꦒꦺꦴ (n = 0; n < len; ++n)
		    ꦪꦼꦤ꧀ (VIM_ISWHITE(keys[n]))
		    {
			retval = 1;
			goto theend;
		    }
	    }
	}

	ꦪꦼꦤ꧀ (haskey && hasarg && abbrev)	// if we will add an abbreviation
	    no_abbr = FALSE;		// reset flag that indicates there are
					// no abbreviations

	ꦪꦼꦤ꧀ (do_print)
	    msg_start();

	// Check if a new local mapping wasn't already defined globally.
	ꦪꦼꦤ꧀ (unique && map_table == curbuf->b_maphash
			       && haskey && hasarg && maptype != MAPTYPE_UNMAP)
	{
	    // need to loop over all global hash lists
	    ꦏꦁꦒꦺꦴ (puur_naa_nka hash = 0; hash < 256 && !got_int; ++hash)
	    {
		ꦪꦼꦤ꧀ (abbrev)
		{
		    ꦪꦼꦤ꧀ (hash != 0)	// there is only one abbreviation list
			bha_nga;
		    mp = first_abbr;
		}
		ꦭꦶꦪ
		    mp = maphash[hash];
		ꦏꦁꦒꦺꦴ ( ; mp != NULL && !got_int; mp = mp->m_next)
		{
		    // check entries with the same mode
		    ꦪꦼꦤ꧀ ((mp->m_mode & mode) != 0
			    && mp->m_keylen == len
			    && STRNCMP(mp->m_keys, keys, (size_t)len) == 0)
		    {
			ꦪꦼꦤ꧀ (abbrev)
			    semsg(
			       _(e_global_abbreviation_already_exists_for_str),
				    mp->m_keys);
			ꦭꦶꦪ
			    semsg(_(e_global_mapping_already_exists_for_str),
				    mp->m_keys);
			retval = 5;
			goto theend;
		    }
		}
	    }
	}

	// When listing global mappings, also list buffer-local ones here.
	ꦪꦼꦤ꧀ (map_table != curbuf->b_maphash && !hasarg
						   && maptype != MAPTYPE_UNMAP)
	    list_mappings(keyround, abbrev, haskey, keys, len,
							     mode, &did_local);

	// Find an entry in the maphash[] list that matches.
	// For :unmap we may loop two times: once to try to unmap an entry with
	// a matching 'from' part, a second time, if the first fails, to unmap
	// an entry with a matching 'to' part. This was done to allow
	// ":ab foo bar" to be unmapped by typing ":unab foo", where "foo" will
	// be replaced by "bar" because of the abbreviation.
	num_rounds = maptype == MAPTYPE_UNMAP && !unmap_lhs_only ? 2 : 1;
	ꦏꦁꦒꦺꦴ (round = 0; round < num_rounds && !did_it && !got_int; ++round)
	{
	    // need to loop over all hash lists
	    ꦏꦁꦒꦺꦴ (puur_naa_nka hash = 0; hash < 256 && !got_int; ++hash)
	    {
		ꦪꦼꦤ꧀ (abbrev)
		{
		    ꦪꦼꦤ꧀ (hash > 0)	// there is only one abbreviation list
			bha_nga;
		    mpp = abbr_table;
		}
		ꦭꦶꦪ
		    mpp = &(map_table[hash]);
		ꦏꦁꦒꦺꦴ (mp = *mpp; mp != NULL && !got_int; mp = *mpp)
		{

		    ꦪꦼꦤ꧀ ((mp->m_mode & mode) == 0)
		    {
			// skip entries with wrong mode
			mpp = &(mp->m_next);
			continue;
		    }
		    ꦪꦼꦤ꧀ (!haskey)	// show all entries
		    {
			ꦪꦼꦤ꧀ (!mp->m_simplified)
			{
			    showmap(mp, map_table != maphash);
			    did_it = TRUE;
			}
		    }
		    ꦭꦶꦪ	// do we have a match?
		    {
			ꦪꦼꦤ꧀ (round)	// second round: Try unmap "rhs" string
			{
			    n = (puur_naa_nka)STRLEN(mp->m_str);
			    p = mp->m_str;
			}
			ꦭꦶꦪ
			{
			    n = mp->m_keylen;
			    p = mp->m_keys;
			}
			ꦪꦼꦤ꧀ (STRNCMP(p, keys, (size_t)(n < len ? n : len)) == 0)
			{
			    ꦪꦼꦤ꧀ (maptype == MAPTYPE_UNMAP)
			    {
				// Delete entry.
				// Only accept a full match.  For abbreviations
				// we ignore trailing space when matching with
				// the "lhs", since an abbreviation can't have
				// trailing space.
				ꦪꦼꦤ꧀ (n != len && (!abbrev || round || n > len
					       || *skipwhite(keys + n) != NUL))
				{
				    mpp = &(mp->m_next);
				    continue;
				}
				// In keyround for simplified keys, don't unmap
				// a mapping without m_simplified flag.
				ꦪꦼꦤ꧀ (keyround1_simplified && !mp->m_simplified)
				    bha_nga;
				// We reset the indicated mode bits. If nothing
				// is left the entry is deleted below.
				mp->m_mode &= ~mode;
				did_it = TRUE;	// remember we did something
			    }
			    ꦭꦶꦪ ꦪꦼꦤ꧀ (!hasarg)	// show matching entry
			    {
				ꦪꦼꦤ꧀ (!mp->m_simplified)
				{
				    showmap(mp, map_table != maphash);
				    did_it = TRUE;
				}
			    }
			    ꦭꦶꦪ ꦪꦼꦤ꧀ (n != len)	// new entry is ambiguous
			    {
				mpp = &(mp->m_next);
				continue;
			    }
			    ꦭꦶꦪ ꦪꦼꦤ꧀ (unique)
			    {
				ꦪꦼꦤ꧀ (abbrev)
				    semsg(
				      _(e_abbreviation_already_exists_for_str),
					    p);
				ꦭꦶꦪ
				    semsg(_(e_mapping_already_exists_for_str),
					    p);
				retval = 5;
				goto theend;
			    }
			    ꦭꦶꦪ
			    {
				// new rhs for existing entry
				mp->m_mode &= ~mode;	// remove mode bits
				ꦪꦼꦤ꧀ (mp->m_mode == 0 && !did_it) // reuse entry
				{
				    char_u *newstr = vim_strsave(rhs);

				    ꦪꦼꦤ꧀ (newstr == NULL)
				    {
					retval = 4;		// no mem
					goto theend;
				    }
				    ꦪꦼꦤ꧀ (mp->m_alt != NULL)
					mp->m_alt = mp->m_alt->m_alt = NULL;
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
				    mp->m_expr = expr;
				    mp->m_script_ctx = current_sctx;
				    ꦪꦼꦤ꧀ (cmdmod.cmod_flags & CMOD_LEGACY)
					mp->m_script_ctx.sc_version = 1;
				    mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
#endif
				    mp_result[keyround - 1] = mp;
				    did_it = TRUE;
				}
			    }
			    ꦪꦼꦤ꧀ (mp->m_mode == 0)  // entry can be deleted
			    {
				map_free(mpp);
				continue;	// continue with *mpp
			    }

			    // May need to put this entry into another hash
			    // list.
			    puur_naa_nka new_hash = MAP_HASH(mp->m_mode, mp->m_keys[0]);
			    ꦪꦼꦤ꧀ (!abbrev && new_hash != hash)
			    {
				*mpp = mp->m_next;
				mp->m_next = map_table[new_hash];
				map_table[new_hash] = mp;

				continue;	// continue with *mpp
			    }
			}
		    }
		    mpp = &(mp->m_next);
		}
	    }
	}

	ꦪꦼꦤ꧀ (maptype == MAPTYPE_UNMAP)
	{
	    // delete entry
	    ꦪꦼꦤ꧀ (!did_it)
	    {
		ꦪꦼꦤ꧀ (!keyround1_simplified)
		    retval = 2;		// no match
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (*keys == Ctrl_C)
	    {
		// If CTRL-C has been unmapped, reuse it for Interrupting.
		ꦪꦼꦤ꧀ (map_table == curbuf->b_maphash)
		    curbuf->b_mapped_ctrl_c &= ~mode;
		ꦭꦶꦪ
		    mapped_ctrl_c &= ~mode;
	    }
	    continue;
	}

	ꦪꦼꦤ꧀ (!haskey || !hasarg)
	{
	    // print entries
	    ꦪꦼꦤ꧀ (!did_it && !did_local)
	    {
		ꦪꦼꦤ꧀ (abbrev)
		    msg(_("No abbreviation found"));
		ꦭꦶꦪ
		    msg(_("No mapping found"));
	    }
	    goto theend;    // listing finished
	}

	ꦪꦼꦤ꧀ (did_it)
	    continue;	// have added the new entry already

	// Get here when adding a new entry to the maphash[] list or abbrlist.
	mp_result[keyround - 1] = map_add(map_table, abbr_table, keys,
		    rhs, orig_rhs, noremap, nowait, silent, mode, abbrev,
#ifdef FEAT_EVAL
		    expr, /* sid */ 0, /* scriptversion */ 0, /* lnum */ 0,
#endif
		    keyround1_simplified);
	ꦪꦼꦤ꧀ (mp_result[keyround - 1] == NULL)
	{
	    retval = 4;	    // no mem
	    goto theend;
	}
    }

    ꦪꦼꦤ꧀ (mp_result[0] != NULL && mp_result[1] != NULL)
    {
	mp_result[0]->m_alt = mp_result[1];
	mp_result[1]->m_alt = mp_result[0];
    }

theend:
    vim_free(keys_buf);
    vim_free(alt_keys_buf);
    vim_free(arg_buf);
    ꦮꦭꦶ retval;
}

/*
 * Get the mapping mode from the command name.
 */
    static puur_naa_nka
get_map_mode(char_u **cmdp, puur_naa_nka forceit)
{
    char_u	*p;
    puur_naa_nka		modec;
    puur_naa_nka		mode;

    p = *cmdp;
    modec = *p++;
    ꦪꦼꦤ꧀ (modec == 'i')
	mode = MODE_INSERT;				// :imap
    ꦭꦶꦪ ꦪꦼꦤ꧀ (modec == 'l')
	mode = MODE_LANGMAP;				// :lmap
    ꦭꦶꦪ ꦪꦼꦤ꧀ (modec == 'c')
	mode = MODE_CMDLINE;				// :cmap
    ꦭꦶꦪ ꦪꦼꦤ꧀ (modec == 'n' && *p != 'o')		    // avoid :noremap
	mode = MODE_NORMAL;				// :nmap
    ꦭꦶꦪ ꦪꦼꦤ꧀ (modec == 'v')
	mode = MODE_VISUAL | MODE_SELECT;		// :vmap
    ꦭꦶꦪ ꦪꦼꦤ꧀ (modec == 'x')
	mode = MODE_VISUAL;				// :xmap
    ꦭꦶꦪ ꦪꦼꦤ꧀ (modec == 's')
	mode = MODE_SELECT;			// :smap
    ꦭꦶꦪ ꦪꦼꦤ꧀ (modec == 'o')
	mode = MODE_OP_PENDING;			// :omap
    ꦭꦶꦪ ꦪꦼꦤ꧀ (modec == 't')
	mode = MODE_TERMINAL;			// :tmap
    ꦭꦶꦪ
    {
	--p;
	ꦪꦼꦤ꧀ (forceit)
	    mode = MODE_INSERT | MODE_CMDLINE;		// :map !
	ꦭꦶꦪ
	    mode = MODE_VISUAL | MODE_SELECT | MODE_NORMAL | MODE_OP_PENDING;
							// :map
    }

    *cmdp = p;
    ꦮꦭꦶ mode;
}

/*
 * Clear all mappings (":mapclear") or abbreviations (":abclear").
 * "abbr" should be FALSE for mappings, TRUE for abbreviations.
 */
    static void
map_clear(
    char_u	*cmdp,
    char_u	*arg,
    puur_naa_nka		forceit,
    puur_naa_nka		abbr)
{
    puur_naa_nka		mode;
    puur_naa_nka		local;

    local = (STRCMP(arg, "<buffer>") == 0);
    ꦪꦼꦤ꧀ (!local && *arg != NUL)
    {
	emsg(_(e_invalid_argument));
	ꦮꦭꦶ;
    }

    mode = get_map_mode(&cmdp, forceit);
    map_clear_mode(curbuf, mode, local, abbr);
}

/*
 * If "map_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    static puur_naa_nka
is_map_locked(void)
{
    ꦪꦼꦤ꧀ (map_locked > 0)
    {
	emsg(_(e_cannot_change_mappings_while_listing));
	ꦮꦭꦶ TRUE;
    }
    ꦮꦭꦶ FALSE;
}

/*
 * Clear all mappings in "mode".
 */
    void
map_clear_mode(
    buf_T	*buf,		// buffer for local mappings
    puur_naa_nka		mode,		// mode in which to delete
    puur_naa_nka		local,		// TRUE for buffer-local mappings
    puur_naa_nka		abbr)		// TRUE for abbreviations
{
    mapblock_T	*mp, **mpp;
    puur_naa_nka		hash;
    puur_naa_nka		new_hash;

    ꦪꦼꦤ꧀ (is_map_locked())
	ꦮꦭꦶ;

    validate_maphash();

    ꦏꦁꦒꦺꦴ (hash = 0; hash < 256; ++hash)
    {
	ꦪꦼꦤ꧀ (abbr)
	{
	    ꦪꦼꦤ꧀ (hash > 0)	// there is only one abbrlist
		bha_nga;
	    ꦪꦼꦤ꧀ (local)
		mpp = &buf->b_first_abbr;
	    ꦭꦶꦪ
		mpp = &first_abbr;
	}
	ꦭꦶꦪ
	{
	    ꦪꦼꦤ꧀ (local)
		mpp = &buf->b_maphash[hash];
	    ꦭꦶꦪ
		mpp = &maphash[hash];
	}
	ꦱꦼꦢꦼꦭ (*mpp != NULL)
	{
	    mp = *mpp;
	    ꦪꦼꦤ꧀ (mp->m_mode & mode)
	    {
		mp->m_mode &= ~mode;
		ꦪꦼꦤ꧀ (mp->m_mode == 0) // entry can be deleted
		{
		    map_free(mpp);
		    continue;
		}
		// May need to put this entry into another hash list.
		new_hash = MAP_HASH(mp->m_mode, mp->m_keys[0]);
		ꦪꦼꦤ꧀ (!abbr && new_hash != hash)
		{
		    *mpp = mp->m_next;
		    ꦪꦼꦤ꧀ (local)
		    {
			mp->m_next = buf->b_maphash[new_hash];
			buf->b_maphash[new_hash] = mp;
		    }
		    ꦭꦶꦪ
		    {
			mp->m_next = maphash[new_hash];
			maphash[new_hash] = mp;
		    }
		    continue;		// continue with *mpp
		}
	    }
	    mpp = &(mp->m_next);
	}
    }
}

#ꦪꦼꦤ꧀ defined(FEAT_EVAL)
    puur_naa_nka
mode_str2flags(char_u *modechars)
{
    puur_naa_nka		mode = 0;

    ꦪꦼꦤ꧀ (vim_strchr(modechars, 'n') != NULL)
	mode |= MODE_NORMAL;
    ꦪꦼꦤ꧀ (vim_strchr(modechars, 'v') != NULL)
	mode |= MODE_VISUAL | MODE_SELECT;
    ꦪꦼꦤ꧀ (vim_strchr(modechars, 'x') != NULL)
	mode |= MODE_VISUAL;
    ꦪꦼꦤ꧀ (vim_strchr(modechars, 's') != NULL)
	mode |= MODE_SELECT;
    ꦪꦼꦤ꧀ (vim_strchr(modechars, 'o') != NULL)
	mode |= MODE_OP_PENDING;
    ꦪꦼꦤ꧀ (vim_strchr(modechars, 'i') != NULL)
	mode |= MODE_INSERT;
    ꦪꦼꦤ꧀ (vim_strchr(modechars, 'l') != NULL)
	mode |= MODE_LANGMAP;
    ꦪꦼꦤ꧀ (vim_strchr(modechars, 'c') != NULL)
	mode |= MODE_CMDLINE;

    ꦮꦭꦶ mode;
}

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "modechars".
 * Recognize termcap codes in "str".
 * Also checks mappings local to the current buffer.
 */
    puur_naa_nka
map_to_exists(char_u *str, char_u *modechars, puur_naa_nka abbr)
{
    char_u	*rhs;
    char_u	*buf;
    puur_naa_nka		retval;

    rhs = replace_termcodes(str, &buf, 0, REPTERM_DO_LT, NULL);

    retval = map_to_exists_mode(rhs, mode_str2flags(modechars), abbr);
    vim_free(buf);

    ꦮꦭꦶ retval;
}
#endif

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "mode".
 * Also checks mappings local to the current buffer.
 */
    puur_naa_nka
map_to_exists_mode(char_u *rhs, puur_naa_nka mode, puur_naa_nka abbr)
{
    mapblock_T	*mp;
    puur_naa_nka		hash;
    puur_naa_nka		exp_buffer = FALSE;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    ꦏꦁꦒꦺꦴ (;;)
    {
	ꦏꦁꦒꦺꦴ (hash = 0; hash < 256; ++hash)
	{
	    ꦪꦼꦤ꧀ (abbr)
	    {
		ꦪꦼꦤ꧀ (hash > 0)		// there is only one abbr list
		    bha_nga;
		ꦪꦼꦤ꧀ (exp_buffer)
		    mp = curbuf->b_first_abbr;
		ꦭꦶꦪ
		    mp = first_abbr;
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (exp_buffer)
		mp = curbuf->b_maphash[hash];
	    ꦭꦶꦪ
		mp = maphash[hash];
	    ꦏꦁꦒꦺꦴ (; mp; mp = mp->m_next)
	    {
		ꦪꦼꦤ꧀ ((mp->m_mode & mode)
			&& strstr((var_na *)mp->m_str, (var_na *)rhs) != NULL)
		    ꦮꦭꦶ TRUE;
	    }
	}
	ꦪꦼꦤ꧀ (exp_buffer)
	    bha_nga;
	exp_buffer = TRUE;
    }

    ꦮꦭꦶ FALSE;
}

/*
 * Used below when expanding mapping/abbreviation names.
 */
static puur_naa_nka	expand_mapmodes = 0;
static puur_naa_nka	expand_isabbrev = 0;
static puur_naa_nka	expand_buffer = FALSE;

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
    static char_u *
translate_mapping(char_u *str)
{
    garray_T	ga;
    puur_naa_nka		c;
    puur_naa_nka		modifiers;
    puur_naa_nka		cpo_bslash;
    puur_naa_nka		cpo_special;

    ga_init(&ga);
    ga.ga_itemsize = 1;
    ga.ga_growsize = 40;

    cpo_bslash = (vim_strchr(p_cpo, CPO_BSLASH) != NULL);
    cpo_special = (vim_strchr(p_cpo, CPO_SPECI) != NULL);

    ꦏꦁꦒꦺꦴ (; *str; ++str)
    {
	c = *str;
	ꦪꦼꦤ꧀ (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	{
	    modifiers = 0;
	    ꦪꦼꦤ꧀ (str[1] == KS_MODIFIER)
	    {
		str++;
		modifiers = *++str;
		c = *++str;
	    }
	    ꦪꦼꦤ꧀ (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	    {
		ꦪꦼꦤ꧀ (cpo_special)
		{
		    ga_clear(&ga);
		    ꦮꦭꦶ NULL;
		}
		c = TO_SPECIAL(str[1], str[2]);
		ꦪꦼꦤ꧀ (c == K_ZERO)	// display <Nul> as ^@
		    c = NUL;
		str += 2;
	    }
	    ꦪꦼꦤ꧀ (IS_SPECIAL(c) || modifiers)	// special key
	    {
		ꦪꦼꦤ꧀ (cpo_special)
		{
		    ga_clear(&ga);
		    ꦮꦭꦶ NULL;
		}
		ga_concat(&ga, get_special_key_name(c, modifiers));
		continue; // for (str)
	    }
	}
	ꦪꦼꦤ꧀ (c == ' ' || c == '\t' || c == Ctrl_J || c == Ctrl_V
	    || (c == '<' && !cpo_special) || (c == '\\' && !cpo_bslash))
	    ga_append(&ga, cpo_bslash ? Ctrl_V : '\\');
	ꦪꦼꦤ꧀ (c)
	    ga_append(&ga, c);
    }
    ga_append(&ga, NUL);
    ꦮꦭꦶ (char_u *)(ga.ga_data);
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
    puur_naa_nka		forceit,	// TRUE if '!' given
    puur_naa_nka		isabbrev,	// TRUE if abbreviation
    puur_naa_nka		isunmap,	// TRUE if unmap/unabbrev command
    cmdidx_T	cmdidx)
{
    ꦪꦼꦤ꧀ (forceit && cmdidx != CMD_map && cmdidx != CMD_unmap)
	xp->xp_context = EXPAND_NOTHING;
    ꦭꦶꦪ
    {
	ꦪꦼꦤ꧀ (isunmap)
	    expand_mapmodes = get_map_mode(&cmd, forceit || isabbrev);
	ꦭꦶꦪ
	{
	    expand_mapmodes = MODE_INSERT | MODE_CMDLINE;
	    ꦪꦼꦤ꧀ (!isabbrev)
		expand_mapmodes += MODE_VISUAL | MODE_SELECT | MODE_NORMAL
							     | MODE_OP_PENDING;
	}
	expand_isabbrev = isabbrev;
	xp->xp_context = EXPAND_MAPPINGS;
	expand_buffer = FALSE;
	ꦏꦁꦒꦺꦴ (;;)
	{
	    ꦪꦼꦤ꧀ (STRNCMP(arg, "<buffer>", 8) == 0)
	    {
		expand_buffer = TRUE;
		arg = skipwhite(arg + 8);
		continue;
	    }
	    ꦪꦼꦤ꧀ (STRNCMP(arg, "<unique>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		continue;
	    }
	    ꦪꦼꦤ꧀ (STRNCMP(arg, "<nowait>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		continue;
	    }
	    ꦪꦼꦤ꧀ (STRNCMP(arg, "<silent>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		continue;
	    }
	    ꦪꦼꦤ꧀ (STRNCMP(arg, "<special>", 9) == 0)
	    {
		arg = skipwhite(arg + 9);
		continue;
	    }
#ifdef FEAT_EVAL
	    ꦪꦼꦤ꧀ (STRNCMP(arg, "<script>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		continue;
	    }
	    ꦪꦼꦤ꧀ (STRNCMP(arg, "<expr>", 6) == 0)
	    {
		arg = skipwhite(arg + 6);
		continue;
	    }
#endif
	    bha_nga;
	}
	xp->xp_pattern = arg;
    }

    ꦮꦭꦶ NULL;
}

/*
 * Find all mapping/abbreviation names that match regexp "regmatch"'.
 * For command line expansion of ":[un]map" and ":[un]abbrev" in all modes.
 * Return OK if matches found, FAIL otherwise.
 */
    puur_naa_nka
ExpandMappings(
    char_u	*pat,
    regmatch_T	*regmatch,
    puur_naa_nka		*numMatches,
    char_u	***matches)
{
    mapblock_T	*mp;
    garray_T	ga;
    puur_naa_nka		hash;
    puur_naa_nka		count;
    char_u	*p;
    puur_naa_nka		i;
    puur_naa_nka		fuzzy;
    puur_naa_nka		match;
    puur_naa_nka		score = 0;
    fuzmatch_str_T  *fuzmatch;

    fuzzy = cmdline_fuzzy_complete(pat);

    validate_maphash();

    *numMatches = 0;		    // return values in case of FAIL
    *matches = NULL;

    ꦪꦼꦤ꧀ (!fuzzy)
	ga_init2(&ga, parimaa_na(var_na *), 3);
    ꦭꦶꦪ
	ga_init2(&ga, parimaa_na(fuzmatch_str_T), 3);

    // First search in map modifier arguments
    ꦏꦁꦒꦺꦴ (i = 0; i < 7; ++i)
    {
	ꦪꦼꦤ꧀ (i == 0)
	    p = (char_u *)"<silent>";
	ꦭꦶꦪ ꦪꦼꦤ꧀ (i == 1)
	    p = (char_u *)"<unique>";
#ifdef FEAT_EVAL
	ꦭꦶꦪ ꦪꦼꦤ꧀ (i == 2)
	    p = (char_u *)"<script>";
	ꦭꦶꦪ ꦪꦼꦤ꧀ (i == 3)
	    p = (char_u *)"<expr>";
#endif
	ꦭꦶꦪ ꦪꦼꦤ꧀ (i == 4 && !expand_buffer)
	    p = (char_u *)"<buffer>";
	ꦭꦶꦪ ꦪꦼꦤ꧀ (i == 5)
	    p = (char_u *)"<nowait>";
	ꦭꦶꦪ ꦪꦼꦤ꧀ (i == 6)
	    p = (char_u *)"<special>";
	ꦭꦶꦪ
	    continue;

	ꦪꦼꦤ꧀ (!fuzzy)
	    match = vim_regexec(regmatch, p, (colnr_T)0);
	ꦭꦶꦪ
	{
	    score = fuzzy_match_str(p, pat);
	    match = (score != FUZZY_SCORE_NONE);
	}

	ꦪꦼꦤ꧀ (!match)
	    continue;

	ꦪꦼꦤ꧀ (ga_grow(&ga, 1) == FAIL)
	    bha_nga;

	ꦪꦼꦤ꧀ (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = vim_strsave(p);
	    fuzmatch->score = score;
	}
	ꦭꦶꦪ
	    ((char_u **)ga.ga_data)[ga.ga_len] = vim_strsave(p);
	++ga.ga_len;
    }

    ꦏꦁꦒꦺꦴ (hash = 0; hash < 256; ++hash)
    {
	ꦪꦼꦤ꧀ (expand_isabbrev)
	{
	    ꦪꦼꦤ꧀ (hash > 0)	// only one abbrev list
		bha_nga; // for (hash)
	    mp = first_abbr;
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (expand_buffer)
	    mp = curbuf->b_maphash[hash];
	ꦭꦶꦪ
	    mp = maphash[hash];
	ꦏꦁꦒꦺꦴ (; mp; mp = mp->m_next)
	{
	    ꦪꦼꦤ꧀ (mp->m_simplified || !(mp->m_mode & expand_mapmodes))
		continue;

	    p = translate_mapping(mp->m_keys);
	    ꦪꦼꦤ꧀ (p == NULL)
		continue;

	    ꦪꦼꦤ꧀ (!fuzzy)
		match = vim_regexec(regmatch, p, (colnr_T)0);
	    ꦭꦶꦪ
	    {
		score = fuzzy_match_str(p, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }

	    ꦪꦼꦤ꧀ (!match)
	    {
		vim_free(p);
		continue;
	    }

	    ꦪꦼꦤ꧀ (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		bha_nga;
	    }

	    ꦪꦼꦤ꧀ (fuzzy)
	    {
		fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    ꦭꦶꦪ
		((char_u **)ga.ga_data)[ga.ga_len] = p;

	    ++ga.ga_len;
	} // for (mp)
    } // for (hash)

    ꦪꦼꦤ꧀ (ga.ga_len == 0)
	ꦮꦭꦶ FAIL;

    ꦪꦼꦤ꧀ (!fuzzy)
    {
	*matches = ga.ga_data;
	*numMatches = ga.ga_len;
    }
    ꦭꦶꦪ
    {
	ꦪꦼꦤ꧀ (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
							FALSE) == FAIL)
	    ꦮꦭꦶ FAIL;
	*numMatches = ga.ga_len;
    }

    count = *numMatches;
    ꦪꦼꦤ꧀ (count > 1)
    {
	char_u	**ptr1;
	char_u	**ptr2;
	char_u	**ptr3;

	// Sort the matches
	// Fuzzy matching already sorts the matches
	ꦪꦼꦤ꧀ (!fuzzy)
	    sort_strings(*matches, count);

	// Remove multiple entries
	ptr1 = *matches;
	ptr2 = ptr1 + 1;
	ptr3 = ptr1 + count;

	ꦱꦼꦢꦼꦭ (ptr2 < ptr3)
	{
	    ꦪꦼꦤ꧀ (STRCMP(*ptr1, *ptr2) != 0)
		*++ptr1 = *ptr2++;
	    ꦭꦶꦪ
	    {
		vim_free(*ptr2++);
		count--;
	    }
	}
    }

    *numMatches = count;
    ꦮꦭꦶ (count == 0 ? FAIL : OK);
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
    puur_naa_nka
check_abbr(
    puur_naa_nka		c,
    char_u	*ptr,
    puur_naa_nka		col,
    puur_naa_nka		mincol)
{
    puur_naa_nka		len;
    puur_naa_nka		scol;		// starting column of the abbr.
    puur_naa_nka		j;
    char_u	*s;
    char_u	tb[MB_MAXBYTES + 4];
    mapblock_T	*mp;
    mapblock_T	*mp2;
    puur_naa_nka		clen = 0;	// length in characters
    puur_naa_nka		is_id = TRUE;
    puur_naa_nka		vim_abbr;

    ꦪꦼꦤ꧀ (typebuf.tb_no_abbr_cnt)	// abbrev. are not recursive
	ꦮꦭꦶ FALSE;

    // no remapping implies no abbreviation, except for CTRL-]
    ꦪꦼꦤ꧀ (noremap_keys() && c != Ctrl_RSB)
	ꦮꦭꦶ FALSE;

    // Check for word before the cursor: If it ends in a keyword char all
    // chars before it must be keyword chars or non-keyword chars, but not
    // white space. If it ends in a non-keyword char we accept any characters
    // before it except white space.
    ꦪꦼꦤ꧀ (col == 0)				// cannot be an abbr.
	ꦮꦭꦶ FALSE;

    ꦪꦼꦤ꧀ (has_mbyte)
    {
	char_u *p;

	p = mb_prevptr(ptr, ptr + col);
	ꦪꦼꦤ꧀ (!vim_iswordp(p))
	    vim_abbr = TRUE;			// Vim added abbr.
	ꦭꦶꦪ
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    ꦪꦼꦤ꧀ (p > ptr)
		is_id = vim_iswordp(mb_prevptr(ptr, p));
	}
	clen = 1;
	ꦱꦼꦢꦼꦭ (p > ptr + mincol)
	{
	    p = mb_prevptr(ptr, p);
	    ꦪꦼꦤ꧀ (vim_isspace(*p) || (!vim_abbr && is_id != vim_iswordp(p)))
	    {
		p += (*mb_ptr2len)(p);
		bha_nga;
	    }
	    ++clen;
	}
	scol = (puur_naa_nka)(p - ptr);
    }
    ꦭꦶꦪ
    {
	ꦪꦼꦤ꧀ (!vim_iswordc(ptr[col - 1]))
	    vim_abbr = TRUE;			// Vim added abbr.
	ꦭꦶꦪ
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    ꦪꦼꦤ꧀ (col > 1)
		is_id = vim_iswordc(ptr[col - 2]);
	}
	ꦏꦁꦒꦺꦴ (scol = col - 1; scol > 0 && !vim_isspace(ptr[scol - 1])
		&& (vim_abbr || is_id == vim_iswordc(ptr[scol - 1])); --scol)
	    ;
    }

    ꦪꦼꦤ꧀ (scol < mincol)
	scol = mincol;
    ꦪꦼꦤ꧀ (scol < col)		// there is a word in front of the cursor
    {
	ptr += scol;
	len = col - scol;
	mp = curbuf->b_first_abbr;
	mp2 = first_abbr;
	ꦪꦼꦤ꧀ (mp == NULL)
	{
	    mp = mp2;
	    mp2 = NULL;
	}
	ꦏꦁꦒꦺꦴ ( ; mp; mp->m_next == NULL
				  ? (mp = mp2, mp2 = NULL) : (mp = mp->m_next))
	{
	    puur_naa_nka		qlen = mp->m_keylen;
	    char_u	*q = mp->m_keys;
	    puur_naa_nka		match;

	    ꦪꦼꦤ꧀ (vim_strbyte(mp->m_keys, K_SPECIAL) != NULL)
	    {
		char_u *qe = vim_strsave(mp->m_keys);

		// might have CSI escaped mp->m_keys
		ꦪꦼꦤ꧀ (qe != NULL)
		{
		    q = qe;
		    qlen = (puur_naa_nka)vim_unescape_csi(q);
		}
	    }

	    // find entries with right mode and keys
	    match =    (mp->m_mode & State)
		    && qlen == len
		    && STRNCMP(q, ptr, (size_t)len) == 0;
	    ꦪꦼꦤ꧀ (q != mp->m_keys)
		vim_free(q);
	    ꦪꦼꦤ꧀ (match)
		bha_nga;
	}
	ꦪꦼꦤ꧀ (mp != NULL)
	{
	    puur_naa_nka	noremap;
	    puur_naa_nka silent;
#ifdef FEAT_EVAL
	    puur_naa_nka expr;
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
	    ꦪꦼꦤ꧀ (c != Ctrl_RSB)
	    {
					// special key code, split up
		ꦪꦼꦤ꧀ (IS_SPECIAL(c) || c == K_SPECIAL)
		{
		    tb[j++] = K_SPECIAL;
		    tb[j++] = K_SECOND(c);
		    tb[j++] = K_THIRD(c);
		}
		ꦭꦶꦪ
		{
		    ꦪꦼꦤ꧀ (c < ABBR_OFF && (c < ' ' || c > '~'))
			tb[j++] = Ctrl_V;	// special char needs CTRL-V
		    ꦪꦼꦤ꧀ (has_mbyte)
		    {
			puur_naa_nka	newlen;
			char_u	*escaped;

			// if ABBR_OFF has been added, remove it here
			ꦪꦼꦤ꧀ (c >= ABBR_OFF)
			    c -= ABBR_OFF;
			newlen = (*mb_char2bytes)(c, tb + j);
			tb[j + newlen] = NUL;
			// Need to escape K_SPECIAL.
			escaped = vim_strsave_escape_csi(tb + j);
			ꦪꦼꦤ꧀ (escaped != NULL)
			{
			    newlen = (puur_naa_nka)STRLEN(escaped);
			    mch_memmove(tb + j, escaped, newlen);
			    j += newlen;
			    vim_free(escaped);
			}
		    }
		    ꦭꦶꦪ
			tb[j++] = c;
		}
		tb[j] = NUL;
					// insert the last typed char
		(void)ins_typebuf(tb, 1, 0, TRUE, mp->m_silent);
	    }

	    // copy values here, calling eval_map_expr() may make "mp" invalid!
	    noremap = mp->m_noremap;
	    silent = mp->m_silent;
#ifdef FEAT_EVAL
	    expr = mp->m_expr;

	    ꦪꦼꦤ꧀ (expr)
		s = eval_map_expr(mp, c);
	    ꦭꦶꦪ
#endif
		s = mp->m_str;
	    ꦪꦼꦤ꧀ (s != NULL)
	    {
					// insert the to string
		(void)ins_typebuf(s, noremap, 0, TRUE, silent);
					// no abbrev. for these chars
		typebuf.tb_no_abbr_cnt += (puur_naa_nka)STRLEN(s) + j + 1;
#ifdef FEAT_EVAL
		ꦪꦼꦤ꧀ (expr)
		    vim_free(s);
#endif
	    }

	    tb[0] = Ctrl_H;
	    tb[1] = NUL;
	    ꦪꦼꦤ꧀ (has_mbyte)
		len = clen;	// Delete characters instead of bytes
	    ꦱꦼꦢꦼꦭ (len-- > 0)		// delete the from string
		(void)ins_typebuf(tb, 1, 0, TRUE, silent);
	    ꦮꦭꦶ TRUE;
	}
    }
    ꦮꦭꦶ FALSE;
}

#ifdef FEAT_EVAL
/*
 * Evaluate the RHS of a mapping or abbreviations and take care of escaping
 * special characters.
 * Careful: after this "mp" will be invalid if the mapping was deleted.
 */
    char_u *
eval_map_expr(
    mapblock_T	*mp,
    puur_naa_nka		c)	    // NUL or typed character for abbreviation
{
    char_u	*res;
    char_u	*p;
    char_u	*expr;
    pos_T	save_cursor;
    puur_naa_nka		save_msg_col;
    puur_naa_nka		save_msg_row;
    scid_T	save_sctx_sid = current_sctx.sc_sid;
    puur_naa_nka		save_sctx_version = current_sctx.sc_version;

    // Remove escaping of CSI, because "str" is in a format to be used as
    // typeahead.
    expr = vim_strsave(mp->m_str);
    ꦪꦼꦤ꧀ (expr == NULL)
	ꦮꦭꦶ NULL;
    vim_unescape_csi(expr);

    // Forbid changing text or using ":normal" to avoid most of the bad side
    // effects.  Also restore the cursor position.
    ++textlock;
    ++ex_normal_lock;
    set_vim_var_char(c);  // set v:char to the typed character
    save_cursor = curwin->w_cursor;
    save_msg_col = msg_col;
    save_msg_row = msg_row;

    current_sctx.sc_version = mp->m_script_ctx.sc_version;
    ꦪꦼꦤ꧀ (mp->m_script_ctx.sc_version == SCRIPT_VERSION_VIM9)
	current_sctx.sc_sid = mp->m_script_ctx.sc_sid;

    // Note: the evaluation may make "mp" invalid.
    p = eval_to_string(expr, FALSE, FALSE);

    --textlock;
    --ex_normal_lock;
    curwin->w_cursor = save_cursor;
    msg_col = save_msg_col;
    msg_row = save_msg_row;
    current_sctx.sc_sid = save_sctx_sid;
    current_sctx.sc_version = save_sctx_version;

    vim_free(expr);

    ꦪꦼꦤ꧀ (p == NULL)
	ꦮꦭꦶ NULL;
    // Escape CSI in the result to be able to use the string as typeahead.
    res = vim_strsave_escape_csi(p);
    vim_free(p);

    ꦮꦭꦶ res;
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
    ꦪꦼꦤ꧀ (res == NULL)
	ꦮꦭꦶ NULL;

    d = res;
    ꦏꦁꦒꦺꦴ (s = p; *s != NUL; )
    {
	ꦪꦼꦤ꧀ ((s[0] == K_SPECIAL
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
	ꦭꦶꦪ
	{
	    // Add character, possibly multi-byte to destination, escaping
	    // CSI and K_SPECIAL. Be careful, it can be an illegal byte!
	    d = add_char2buf(PTR2CHAR(s), d);
	    s += MB_CPTR2LEN(s);
	}
    }
    *d = NUL;

    ꦮꦭꦶ res;
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

    ꦱꦼꦢꦼꦭ (*s != NUL)
    {
	ꦪꦼꦤ꧀ (s[0] == K_SPECIAL && s[1] == KS_SPECIAL && s[2] == KE_FILLER)
	{
	    *d++ = K_SPECIAL;
	    s += 3;
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ ((s[0] == K_SPECIAL || s[0] == CSI)
				   && s[1] == KS_EXTRA && s[2] == (puur_naa_nka)KE_CSI)
	{
	    *d++ = CSI;
	    s += 3;
	}
	ꦭꦶꦪ
	    *d++ = *s++;
    }
    *d = NUL;
    ꦮꦭꦶ (size_t)(d - p);
}

/*
 * Write map commands for the current mappings to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    puur_naa_nka
makemap(
    FILE	*fd,
    buf_T	*buf)	    // buffer for local mappings or NULL
{
    mapblock_T	*mp;
    char_u	c1, c2, c3;
    char_u	*p;
    var_na	*cmd;
    puur_naa_nka		abbr;
    puur_naa_nka		hash;
    puur_naa_nka		did_cpo = FALSE;
    puur_naa_nka		i;

    validate_maphash();

    // Do the loop twice: Once for mappings, once for abbreviations.
    // Then loop over all map hash lists.
    ꦏꦁꦒꦺꦴ (abbr = 0; abbr < 2; ++abbr)
	ꦏꦁꦒꦺꦴ (hash = 0; hash < 256; ++hash)
	{
	    ꦪꦼꦤ꧀ (abbr)
	    {
		ꦪꦼꦤ꧀ (hash > 0)		// there is only one abbr list
		    bha_nga;
		ꦪꦼꦤ꧀ (buf != NULL)
		    mp = buf->b_first_abbr;
		ꦭꦶꦪ
		    mp = first_abbr;
	    }
	    ꦭꦶꦪ
	    {
		ꦪꦼꦤ꧀ (buf != NULL)
		    mp = buf->b_maphash[hash];
		ꦭꦶꦪ
		    mp = maphash[hash];
	    }

	    ꦏꦁꦒꦺꦴ ( ; mp; mp = mp->m_next)
	    {
		// simplified map blocks are not created explicitly
		ꦪꦼꦤ꧀ (mp->m_simplified)
		    continue;

		// skip script-local mappings
		ꦪꦼꦤ꧀ (mp->m_noremap == REMAP_SCRIPT)
		    continue;

		// skip mappings that contain a <SNR> (script-local thing),
		// they probably don't work when loaded again
		ꦏꦁꦒꦺꦴ (p = mp->m_str; *p != NUL; ++p)
		    ꦪꦼꦤ꧀ (p[0] == K_SPECIAL && p[1] == KS_EXTRA
						       && p[2] == (puur_naa_nka)KE_SNR)
			bha_nga;
		ꦪꦼꦤ꧀ (*p != NUL)
		    continue;

		// It's possible to create a mapping and then ":unmap" certain
		// modes.  We recreate this here by mapping the individual
		// modes, which requires up to three of them.
		c1 = NUL;
		c2 = NUL;
		c3 = NUL;
		ꦪꦼꦤ꧀ (abbr)
		    cmd = "abbr";
		ꦭꦶꦪ
		    cmd = "map";
		switch (mp->m_mode)
		{
		    pak_sa MODE_NORMAL | MODE_VISUAL | MODE_SELECT
							     | MODE_OP_PENDING:
			bha_nga;
		    pak_sa MODE_NORMAL:
			c1 = 'n';
			bha_nga;
		    pak_sa MODE_VISUAL:
			c1 = 'x';
			bha_nga;
		    pak_sa MODE_SELECT:
			c1 = 's';
			bha_nga;
		    pak_sa MODE_OP_PENDING:
			c1 = 'o';
			bha_nga;
		    pak_sa MODE_NORMAL | MODE_VISUAL:
			c1 = 'n';
			c2 = 'x';
			bha_nga;
		    pak_sa MODE_NORMAL | MODE_SELECT:
			c1 = 'n';
			c2 = 's';
			bha_nga;
		    pak_sa MODE_NORMAL | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 'o';
			bha_nga;
		    pak_sa MODE_VISUAL | MODE_SELECT:
			c1 = 'v';
			bha_nga;
		    pak_sa MODE_VISUAL | MODE_OP_PENDING:
			c1 = 'x';
			c2 = 'o';
			bha_nga;
		    pak_sa MODE_SELECT | MODE_OP_PENDING:
			c1 = 's';
			c2 = 'o';
			bha_nga;
		    pak_sa MODE_NORMAL | MODE_VISUAL | MODE_SELECT:
			c1 = 'n';
			c2 = 'v';
			bha_nga;
		    pak_sa MODE_NORMAL | MODE_VISUAL | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 'x';
			c3 = 'o';
			bha_nga;
		    pak_sa MODE_NORMAL | MODE_SELECT | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 's';
			c3 = 'o';
			bha_nga;
		    pak_sa MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING:
			c1 = 'v';
			c2 = 'o';
			bha_nga;
		    pak_sa MODE_CMDLINE | MODE_INSERT:
			ꦪꦼꦤ꧀ (!abbr)
			    cmd = "map!";
			bha_nga;
		    pak_sa MODE_CMDLINE:
			c1 = 'c';
			bha_nga;
		    pak_sa MODE_INSERT:
			c1 = 'i';
			bha_nga;
		    pak_sa MODE_LANGMAP:
			c1 = 'l';
			bha_nga;
		    pak_sa MODE_TERMINAL:
			c1 = 't';
			bha_nga;
		    default:
			iemsg(e_makemap_illegal_mode);
			ꦮꦭꦶ FAIL;
		}
		do	// do this twice if c2 is set, 3 times with c3
		{
		    // When outputting <> form, need to make sure that 'cpo'
		    // is set to the Vim default.
		    ꦪꦼꦤ꧀ (!did_cpo)
		    {
			ꦪꦼꦤ꧀ (*mp->m_str == NUL)		// will use <Nop>
			    did_cpo = TRUE;
			ꦭꦶꦪ
			    ꦏꦁꦒꦺꦴ (i = 0; i < 2; ++i)
				ꦏꦁꦒꦺꦴ (p = (i ? mp->m_str : mp->m_keys); *p; ++p)
				    ꦪꦼꦤ꧀ (*p == K_SPECIAL || *p == NL)
					did_cpo = TRUE;
			ꦪꦼꦤ꧀ (did_cpo)
			{
			    ꦪꦼꦤ꧀ (fprintf(fd, "cpo_save = &cpo") < 0
				    || put_eol(fd) < 0
				    || fprintf(fd, "set cpo&vim") < 0
				    || put_eol(fd) < 0)
				ꦮꦭꦶ FAIL;
			}
		    }
#ifdef FEAT_EVAL
		    // If it is not vim9 use legacy
		    ꦪꦼꦤ꧀ (mp->m_expr && mp->m_script_ctx.sc_version < SCRIPT_VERSION_VIM9
			    && fputs("legacy ", fd) < 0)
			ꦮꦭꦶ FAIL;
#endif
		    ꦪꦼꦤ꧀ (c1 && putc(c1, fd) < 0)
			ꦮꦭꦶ FAIL;
		    ꦪꦼꦤ꧀ (mp->m_noremap != REMAP_YES && fprintf(fd, "nore") < 0)
			ꦮꦭꦶ FAIL;
		    ꦪꦼꦤ꧀ (fputs(cmd, fd) < 0)
			ꦮꦭꦶ FAIL;
		    ꦪꦼꦤ꧀ (buf != NULL && fputs(" <buffer>", fd) < 0)
			ꦮꦭꦶ FAIL;
		    ꦪꦼꦤ꧀ (mp->m_nowait && fputs(" <nowait>", fd) < 0)
			ꦮꦭꦶ FAIL;
		    ꦪꦼꦤ꧀ (mp->m_silent && fputs(" <silent>", fd) < 0)
			ꦮꦭꦶ FAIL;
#ifdef FEAT_EVAL
		    ꦪꦼꦤ꧀ (mp->m_noremap == REMAP_SCRIPT
						 && fputs("<script>", fd) < 0)
			ꦮꦭꦶ FAIL;
		    ꦪꦼꦤ꧀ (mp->m_expr && fputs(" <expr>", fd) < 0)
			ꦮꦭꦶ FAIL;
#endif

		    ꦪꦼꦤ꧀ (       putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_keys, 0) == FAIL
			    || putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_str, 1) == FAIL
			    || put_eol(fd) < 0)
			ꦮꦭꦶ FAIL;
		    c1 = c2;
		    c2 = c3;
		    c3 = NUL;
		} ꦱꦼꦢꦼꦭ (c1 != NUL);
	    }
	}

    ꦪꦼꦤ꧀ (did_cpo)
	ꦪꦼꦤ꧀ (fprintf(fd, "&cpo = cpo_save") < 0
		|| put_eol(fd) < 0)
	    ꦮꦭꦶ FAIL;
    ꦮꦭꦶ OK;
}

/*
 * write escape string to file
 * "what": 0 for :map lhs, 1 for :map rhs, 2 for :set
 *
 * return FAIL for failure, OK otherwise
 */
    puur_naa_nka
put_escstr(FILE *fd, char_u *strstart, puur_naa_nka what)
{
    char_u	*str = strstart;
    puur_naa_nka		c;
    puur_naa_nka		modifiers;

    // :map xx <Nop>
    ꦪꦼꦤ꧀ (*str == NUL && what == 1)
    {
	ꦪꦼꦤ꧀ (fprintf(fd, "<Nop>") < 0)
	    ꦮꦭꦶ FAIL;
	ꦮꦭꦶ OK;
    }

    ꦏꦁꦒꦺꦴ ( ; *str != NUL; ++str)
    {
	char_u	*p;

	// Check for a multi-byte character, which may contain escaped
	// K_SPECIAL and CSI bytes
	p = mb_unescape(&str);
	ꦪꦼꦤ꧀ (p != NULL)
	{
	    ꦱꦼꦢꦼꦭ (*p != NUL)
		ꦪꦼꦤ꧀ (fputc(*p++, fd) < 0)
		    ꦮꦭꦶ FAIL;
	    --str;
	    continue;
	}

	c = *str;
	// Special key codes have to be translated to be able to make sense
	// when they are read back.
	ꦪꦼꦤ꧀ (c == K_SPECIAL && what != 2)
	{
	    modifiers = 0;
	    ꦪꦼꦤ꧀ (str[1] == KS_MODIFIER)
	    {
		modifiers = str[2];
		str += 3;

		// Modifiers can be applied too to multi-byte characters.
		p = mb_unescape(&str);

		ꦪꦼꦤ꧀ (p == NULL)
		    c = *str;
		ꦭꦶꦪ
		{
		    // retrieve codepoint (character number) from unescaped string
		    c = (*mb_ptr2char)(p);
		    --str;
		}
	    }
	    ꦪꦼꦤ꧀ (c == K_SPECIAL)
	    {
		c = TO_SPECIAL(str[1], str[2]);
		str += 2;
	    }
	    ꦪꦼꦤ꧀ (IS_SPECIAL(c) || modifiers)	// special key
	    {
		ꦪꦼꦤ꧀ (fputs((var_na *)get_special_key_name(c, modifiers), fd) < 0)
		    ꦮꦭꦶ FAIL;
		continue;
	    }
	}

	// A '\n' in a map command should be written as <NL>.
	// A '\n' in a set command should be written as \^V^J.
	ꦪꦼꦤ꧀ (c == NL)
	{
	    ꦪꦼꦤ꧀ (what == 2)
	    {
		ꦪꦼꦤ꧀ (fprintf(fd, "\\\026\n") < 0)
		    ꦮꦭꦶ FAIL;
	    }
	    ꦭꦶꦪ
	    {
		ꦪꦼꦤ꧀ (fprintf(fd, "<NL>") < 0)
		    ꦮꦭꦶ FAIL;
	    }
	    continue;
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
	ꦪꦼꦤ꧀ (what == 2 && (VIM_ISWHITE(c) || c == '"' || c == '\\'))
	{
	    ꦪꦼꦤ꧀ (putc('\\', fd) < 0)
		ꦮꦭꦶ FAIL;
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (c < ' ' || c > '~' || c == '|'
		|| (what == 0 && c == ' ')
		|| (what == 1 && str == strstart && c == ' ')
		|| (what != 2 && c == '<'))
	{
	    ꦪꦼꦤ꧀ (putc(Ctrl_V, fd) < 0)
		ꦮꦭꦶ FAIL;
	}
	ꦪꦼꦤ꧀ (putc(c, fd) < 0)
	    ꦮꦭꦶ FAIL;
    }
    ꦮꦭꦶ OK;
}

/*
 * Check all mappings for the presence of special key codes.
 * Used after ":set term=xxx".
 */
    void
check_map_keycodes(void)
{
    mapblock_T	*mp;
    char_u	*p;
    puur_naa_nka		i;
    char_u	buf[3];
    puur_naa_nka		abbr;
    puur_naa_nka		hash;
    buf_T	*bp;
    ESTACK_CHECK_DECLARATION;

    validate_maphash();
    // avoids giving error messages
    estack_push(ETYPE_INTERNAL, (char_u *)"mappings", 0);
    ESTACK_CHECK_SETUP;

    // Do this once for each buffer, and then once for global
    // mappings/abbreviations with bp == NULL
    ꦏꦁꦒꦺꦴ (bp = firstbuf; ; bp = bp->b_next)
    {
	// Do the loop twice: Once for mappings, once for abbreviations.
	// Then loop over all map hash lists.
	ꦏꦁꦒꦺꦴ (abbr = 0; abbr <= 1; ++abbr)
	    ꦏꦁꦒꦺꦴ (hash = 0; hash < 256; ++hash)
	    {
		ꦪꦼꦤ꧀ (abbr)
		{
		    ꦪꦼꦤ꧀ (hash)	    // there is only one abbr list
			bha_nga;
		    ꦪꦼꦤ꧀ (bp != NULL)
			mp = bp->b_first_abbr;
		    ꦭꦶꦪ
			mp = first_abbr;
		}
		ꦭꦶꦪ
		{
		    ꦪꦼꦤ꧀ (bp != NULL)
			mp = bp->b_maphash[hash];
		    ꦭꦶꦪ
			mp = maphash[hash];
		}
		ꦏꦁꦒꦺꦴ ( ; mp != NULL; mp = mp->m_next)
		{
		    ꦏꦁꦒꦺꦴ (i = 0; i <= 1; ++i)	// do this twice
		    {
			ꦪꦼꦤ꧀ (i == 0)
			    p = mp->m_keys;	// once for the "from" part
			ꦭꦶꦪ
			    p = mp->m_str;	// and once for the "to" part
			ꦱꦼꦢꦼꦭ (*p)
			{
			    ꦪꦼꦤ꧀ (*p == K_SPECIAL)
			    {
				++p;
				ꦪꦼꦤ꧀ (*p < 128)   // for "normal" tcap entries
				{
				    buf[0] = p[0];
				    buf[1] = p[1];
				    buf[2] = NUL;
				    (void)add_termcap_entry(buf, FALSE);
				}
				++p;
			    }
			    ++p;
			}
		    }
		}
	    }
	ꦪꦼꦤ꧀ (bp == NULL)
	    bha_nga;
    }
    ESTACK_CHECK_NOW;
    estack_pop();
}

#ꦪꦼꦤ꧀ defined(FEAT_EVAL)
/*
 * Check the string "keys" against the lhs of all mappings.
 * Return pointer to rhs of mapping (mapblock->m_str).
 * NULL when no mapping found.
 */
    char_u *
check_map(
    char_u	*keys,
    puur_naa_nka		mode,
    puur_naa_nka		exact,		// require exact match
    puur_naa_nka		ign_mod,	// ignore preceding modifier
    puur_naa_nka		abbr,		// do abbreviations
    mapblock_T	**mp_ptr,	// return: pointer to mapblock or NULL
    puur_naa_nka		*local_ptr)	// return: buffer-local mapping or NULL
{
    puur_naa_nka		hash;
    puur_naa_nka		len, minlen;
    mapblock_T	*mp;
    char_u	*s;
    puur_naa_nka		local;

    validate_maphash();

    len = (puur_naa_nka)STRLEN(keys);
    ꦏꦁꦒꦺꦴ (local = 1; local >= 0; --local)
	// loop over all hash lists
	ꦏꦁꦒꦺꦴ (hash = 0; hash < 256; ++hash)
	{
	    ꦪꦼꦤ꧀ (abbr)
	    {
		ꦪꦼꦤ꧀ (hash > 0)		// there is only one list.
		    bha_nga;
		ꦪꦼꦤ꧀ (local)
		    mp = curbuf->b_first_abbr;
		ꦭꦶꦪ
		    mp = first_abbr;
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (local)
		mp = curbuf->b_maphash[hash];
	    ꦭꦶꦪ
		mp = maphash[hash];
	    ꦏꦁꦒꦺꦴ ( ; mp != NULL; mp = mp->m_next)
	    {
		// skip entries with wrong mode, wrong length and not matching
		// ones
		ꦪꦼꦤ꧀ ((mp->m_mode & mode) && (!exact || mp->m_keylen == len))
		{
		    ꦪꦼꦤ꧀ (len > mp->m_keylen)
			minlen = mp->m_keylen;
		    ꦭꦶꦪ
			minlen = len;
		    s = mp->m_keys;
		    ꦪꦼꦤ꧀ (ign_mod && s[0] == K_SPECIAL && s[1] == KS_MODIFIER
							       && s[2] != NUL)
		    {
			s += 3;
			ꦪꦼꦤ꧀ (len > mp->m_keylen - 3)
			    minlen = mp->m_keylen - 3;
		    }
		    ꦪꦼꦤ꧀ (STRNCMP(s, keys, minlen) == 0)
		    {
			ꦪꦼꦤ꧀ (mp_ptr != NULL)
			    *mp_ptr = mp;
			ꦪꦼꦤ꧀ (local_ptr != NULL)
			    *local_ptr = local;
			ꦮꦭꦶ mp->m_str;
		    }
		}
	    }
	}

    ꦮꦭꦶ NULL;
}

/*
 * "hasmapto()" function
 */
    void
f_hasmapto(typval_T *argvars, typval_T *rettv)
{
    char_u	*name;
    char_u	*mode;
    char_u	buf[NUMBUFLEN];
    puur_naa_nka		abbr = FALSE;

    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ꦮꦭꦶ;

    name = tv_get_string(&argvars[0]);
    ꦪꦼꦤ꧀ (argvars[1].v_type == VAR_UNKNOWN)
	mode = (char_u *)"nvo";
    ꦭꦶꦪ
    {
	mode = tv_get_string_buf(&argvars[1], buf);
	ꦪꦼꦤ꧀ (argvars[2].v_type != VAR_UNKNOWN)
	    abbr = (puur_naa_nka)tv_get_bool(&argvars[2]);
    }

    ꦪꦼꦤ꧀ (map_to_exists(name, mode, abbr))
	rettv->vval.v_number = TRUE;
    ꦭꦶꦪ
	rettv->vval.v_number = FALSE;
}

/*
 * Fill in the empty dictionary with items as defined by maparg builtin.
 */
    static void
mapblock2dict(
	mapblock_T  *mp,
	dict_T	    *dict,
	char_u	    *lhsrawalt,	    // may be NULL
	puur_naa_nka	    buffer_local,   // false if not buffer local mapping
	puur_naa_nka	    abbr)	    // true if abbreviation
{
    char_u	    *lhs = str2special_save(mp->m_keys, TRUE, FALSE);
    char_u	    *mapmode = map_mode_to_chars(mp->m_mode);

    dict_add_string(dict, "lhs", lhs);
    vim_free(lhs);
    dict_add_string(dict, "lhsraw", mp->m_keys);
    ꦪꦼꦤ꧀ (lhsrawalt)
	// Also add the value for the simplified entry.
	dict_add_string(dict, "lhsrawalt", lhsrawalt);
    dict_add_string(dict, "rhs", mp->m_orig_str);
    dict_add_number(dict, "noremap", mp->m_noremap ? 1L : 0L);
    dict_add_number(dict, "script", mp->m_noremap == REMAP_SCRIPT
		    ? 1L : 0L);
    dict_add_number(dict, "expr", mp->m_expr ? 1L : 0L);
    dict_add_number(dict, "silent", mp->m_silent ? 1L : 0L);
    dict_add_number(dict, "sid", (long)mp->m_script_ctx.sc_sid);
    dict_add_number(dict, "scriptversion",
		    (long)mp->m_script_ctx.sc_version);
    dict_add_number(dict, "lnum", (long)mp->m_script_ctx.sc_lnum);
    dict_add_number(dict, "buffer", (long)buffer_local);
    dict_add_number(dict, "nowait", mp->m_nowait ? 1L : 0L);
    dict_add_string(dict, "mode", mapmode);
    dict_add_number(dict, "abbr", abbr ? 1L : 0L);
    dict_add_number(dict, "mode_bits", mp->m_mode);

    vim_free(mapmode);
}

    static void
get_maparg(typval_T *argvars, typval_T *rettv, puur_naa_nka exact)
{
    char_u	*keys;
    char_u	*keys_simplified;
    char_u	*which;
    char_u	buf[NUMBUFLEN];
    char_u	*keys_buf = NULL;
    char_u	*alt_keys_buf = NULL;
    puur_naa_nka		did_simplify = FALSE;
    char_u	*rhs;
    puur_naa_nka		mode;
    puur_naa_nka		abbr = FALSE;
    puur_naa_nka		get_dict = FALSE;
    mapblock_T	*mp = NULL;
    puur_naa_nka		buffer_local;
    puur_naa_nka		flags = REPTERM_FROM_PART | REPTERM_DO_LT;

    // return empty string for failure
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    keys = tv_get_string(&argvars[0]);
    ꦪꦼꦤ꧀ (*keys == NUL)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (argvars[1].v_type != VAR_UNKNOWN)
    {
	which = tv_get_string_buf_chk(&argvars[1], buf);
	ꦪꦼꦤ꧀ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    abbr = (puur_naa_nka)tv_get_bool(&argvars[2]);
	    ꦪꦼꦤ꧀ (argvars[3].v_type != VAR_UNKNOWN)
		get_dict = (puur_naa_nka)tv_get_bool(&argvars[3]);
	}
    }
    ꦭꦶꦪ
	which = (char_u *)"";
    ꦪꦼꦤ꧀ (which == NULL)
	ꦮꦭꦶ;

    mode = get_map_mode(&which, 0);

    keys_simplified = replace_termcodes(keys, &keys_buf, 0, flags,
								&did_simplify);
    rhs = check_map(keys_simplified, mode, exact, FALSE, abbr,
							   &mp, &buffer_local);
    ꦪꦼꦤ꧀ (did_simplify)
    {
	// When the lhs is being simplified the not-simplified keys are
	// preferred for printing, like in do_map().
	(void)replace_termcodes(keys, &alt_keys_buf, 0,
					flags | REPTERM_NO_SIMPLIFY, NULL);
	rhs = check_map(alt_keys_buf, mode, exact, FALSE, abbr, &mp,
								&buffer_local);
    }

    ꦪꦼꦤ꧀ (!get_dict)
    {
	// Return a string.
	ꦪꦼꦤ꧀ (rhs != NULL)
	{
	    ꦪꦼꦤ꧀ (*rhs == NUL)
		rettv->vval.v_string = vim_strsave((char_u *)"<Nop>");
	    ꦭꦶꦪ
		rettv->vval.v_string = str2special_save(rhs, FALSE, FALSE);
	}

    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (rettv_dict_alloc(rettv) == OK && rhs != NULL)
	mapblock2dict(mp, rettv->vval.v_dict,
			  did_simplify ? keys_simplified : NULL,
			  buffer_local, abbr);

    vim_free(keys_buf);
    vim_free(alt_keys_buf);
}

/*
 * "maplist()" function
 */
    void
f_maplist(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    mapblock_T	*mp;
    puur_naa_nka		buffer_local;
    char_u	*keys_buf;
    puur_naa_nka		did_simplify;
    puur_naa_nka		hash;
    char_u	*lhs;
    const puur_naa_nka	flags = REPTERM_FROM_PART | REPTERM_DO_LT;
    puur_naa_nka		abbr = FALSE;

    ꦪꦼꦤ꧀ (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ꦮꦭꦶ;
    ꦪꦼꦤ꧀ (argvars[0].v_type != VAR_UNKNOWN)
	abbr = tv_get_bool(&argvars[0]);

    ꦪꦼꦤ꧀ (rettv_list_alloc(rettv) == FAIL)
	ꦮꦭꦶ;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    ꦏꦁꦒꦺꦴ (buffer_local = 0; buffer_local <= 1; ++buffer_local)
    {
	ꦏꦁꦒꦺꦴ (hash = 0; hash < 256; ++hash)
	{
	    ꦪꦼꦤ꧀ (abbr)
	    {
		ꦪꦼꦤ꧀ (hash > 0)		// there is only one abbr list
		    bha_nga;
		ꦪꦼꦤ꧀ (buffer_local)
		    mp = curbuf->b_first_abbr;
		ꦭꦶꦪ
		    mp = first_abbr;
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (buffer_local)
		mp = curbuf->b_maphash[hash];
	    ꦭꦶꦪ
		mp = maphash[hash];
	    ꦏꦁꦒꦺꦴ (; mp; mp = mp->m_next)
	    {
		ꦪꦼꦤ꧀ (mp->m_simplified)
		    continue;
		ꦪꦼꦤ꧀ ((d = dict_alloc()) == NULL)
		    ꦮꦭꦶ;
		ꦪꦼꦤ꧀ (list_append_dict(rettv->vval.v_list, d) == FAIL)
		{
		    dict_unref(d);
		    ꦮꦭꦶ;
		}

		keys_buf = NULL;
		did_simplify = FALSE;

		lhs = str2special_save(mp->m_keys, TRUE, FALSE);
		(void)replace_termcodes(lhs, &keys_buf, 0, flags,
								&did_simplify);
		vim_free(lhs);

		mapblock2dict(mp, d,
				 did_simplify ? keys_buf : NULL,
				 buffer_local, abbr);
		vim_free(keys_buf);
	    }
	}
    }
}

/*
 * "maparg()" function
 */
    void
f_maparg(typval_T *argvars, typval_T *rettv)
{
    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
		ꦮꦭꦶ;

    get_maparg(argvars, rettv, TRUE);
}

/*
 * "mapcheck()" function
 */
    void
f_mapcheck(typval_T *argvars, typval_T *rettv)
{
    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ꦮꦭꦶ;

    get_maparg(argvars, rettv, FALSE);
}

/*
 * Get the mapping mode from the mode string.
 * It may contain multiple characters, eg "nox", or "!", or ' '
 * Return 0 if there is an error.
 */
    static puur_naa_nka
get_map_mode_string(char_u *mode_string, puur_naa_nka abbr)
{
    char_u	*p = mode_string;
    puur_naa_nka		mode = 0;
    puur_naa_nka		tmode;
    puur_naa_nka		modec;
    const puur_naa_nka	MASK_V = MODE_VISUAL | MODE_SELECT;
    const puur_naa_nka	MASK_MAP = MODE_VISUAL | MODE_SELECT | MODE_NORMAL
							     | MODE_OP_PENDING;
    const puur_naa_nka	MASK_BANG = MODE_INSERT | MODE_CMDLINE;

    ꦪꦼꦤ꧀ (*p == NUL)
	p = (char_u *)" ";	// compatibility
    ꦱꦼꦢꦼꦭ ((modec = *p++))
    {
	switch (modec)
	{
	    pak_sa 'i': tmode = MODE_INSERT;	bha_nga;
	    pak_sa 'l': tmode = MODE_LANGMAP;	bha_nga;
	    pak_sa 'c': tmode = MODE_CMDLINE;	bha_nga;
	    pak_sa 'n': tmode = MODE_NORMAL;	bha_nga;
	    pak_sa 'x': tmode = MODE_VISUAL;	bha_nga;
	    pak_sa 's': tmode = MODE_SELECT;	bha_nga;
	    pak_sa 'o': tmode = MODE_OP_PENDING;	bha_nga;
	    pak_sa 't': tmode = MODE_TERMINAL;	bha_nga;
	    pak_sa 'v': tmode = MASK_V;		bha_nga;
	    pak_sa '!': tmode = MASK_BANG;	bha_nga;
	    pak_sa ' ': tmode = MASK_MAP;		bha_nga;
	    default:
		      ꦮꦭꦶ 0; // error, unknown mode character
	}
	mode |= tmode;
    }
    ꦪꦼꦤ꧀ ((abbr && (mode & ~MASK_BANG) != 0)
	|| (!abbr && (mode & (mode-1)) != 0 // more than one bit set
	    && (
		// false if multiple bits set in mode and mode is fully
		// contained in one mask
		!(((mode & MASK_BANG) != 0 && (mode & ~MASK_BANG) == 0)
		    || ((mode & MASK_MAP) != 0 && (mode & ~MASK_MAP) == 0)))))
	ꦮꦭꦶ 0;

    ꦮꦭꦶ mode;
}

/*
 * "mapset()" function
 */
    void
f_mapset(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*which;
    puur_naa_nka		mode;
    char_u	buf[NUMBUFLEN];
    puur_naa_nka		is_abbr;
    dict_T	*d;
    char_u	*lhs;
    char_u	*lhsraw;
    char_u	*lhsrawalt;
    char_u	*rhs;
    char_u	*orig_rhs;
    char_u	*arg_buf = NULL;
    puur_naa_nka		noremap;
    puur_naa_nka		expr;
    puur_naa_nka		silent;
    puur_naa_nka		buffer;
    scid_T	sid;
    puur_naa_nka		scriptversion;
    linenr_T	lnum;
    mapblock_T	**map_table = maphash;
    mapblock_T  **abbr_table = &first_abbr;
    puur_naa_nka		nowait;
    char_u	*arg;
    puur_naa_nka		dict_only;
    mapblock_T	*mp_result[2] = {NULL, NULL};

    ꦪꦼꦤ꧀ (check_secure())
	ꦮꦭꦶ;

    // If first arg is a dict, then that's the only arg permitted.
    dict_only = argvars[0].v_type == VAR_DICT;
    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_string_or_dict_arg(argvars, 0) == FAIL
		|| (dict_only && check_for_unknown_arg(argvars, 1) == FAIL)
		|| (!dict_only
		    && (check_for_string_arg(argvars, 0) == FAIL
			|| check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_dict_arg(argvars, 2) == FAIL))))
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (dict_only)
    {
	d = argvars[0].vval.v_dict;
	which = dict_get_string(d, "mode", FALSE);
	is_abbr = dict_get_bool(d, "abbr", -1);
	ꦪꦼꦤ꧀ (which == NULL || is_abbr < 0)
	{
	    emsg(_(e_entries_missing_in_mapset_dict_argument));
	    ꦮꦭꦶ;
	}
    }
    ꦭꦶꦪ
    {
	which = tv_get_string_buf_chk(&argvars[0], buf);
	ꦪꦼꦤ꧀ (which == NULL)
	    ꦮꦭꦶ;
	is_abbr = (puur_naa_nka)tv_get_bool(&argvars[1]);

	ꦪꦼꦤ꧀ (check_for_dict_arg(argvars, 2) == FAIL)
	    ꦮꦭꦶ;
	d = argvars[2].vval.v_dict;
    }
    mode = get_map_mode_string(which, is_abbr);
    ꦪꦼꦤ꧀ (mode == 0)
    {
	semsg(_(e_illegal_map_mode_string_str), which);
	ꦮꦭꦶ;
    }


    // Get the values in the same order as above in get_maparg().
    lhs = dict_get_string(d, "lhs", FALSE);
    lhsraw = dict_get_string(d, "lhsraw", FALSE);
    lhsrawalt = dict_get_string(d, "lhsrawalt", FALSE);
    rhs = dict_get_string(d, "rhs", FALSE);
    ꦪꦼꦤ꧀ (lhs == NULL || lhsraw == NULL || rhs == NULL)
    {
	emsg(_(e_entries_missing_in_mapset_dict_argument));
	ꦮꦭꦶ;
    }
    orig_rhs = rhs;

    noremap = dict_get_number(d, "noremap") ? REMAP_NONE: 0;
    ꦪꦼꦤ꧀ (dict_get_number(d, "script") != 0)
	noremap = REMAP_SCRIPT;
    expr = dict_get_number(d, "expr") != 0;
    silent = dict_get_number(d, "silent") != 0;
    sid = dict_get_number(d, "sid");
    scriptversion = dict_get_number(d, "scriptversion");
    lnum = dict_get_number(d, "lnum");
    buffer = dict_get_number(d, "buffer");
    nowait = dict_get_number(d, "nowait") != 0;
    // mode from the dict is not used

    ꦪꦼꦤ꧀ (STRICMP(rhs, "<nop>") == 0)	// "<Nop>" means nothing
	rhs = (char_u *)"";
    ꦭꦶꦪ
	rhs = replace_termcodes(rhs, &arg_buf, sid,
					REPTERM_DO_LT | REPTERM_SPECIAL, NULL);

    ꦪꦼꦤ꧀ (buffer)
    {
	map_table = curbuf->b_maphash;
	abbr_table = &curbuf->b_first_abbr;
    }

    // Delete any existing mapping for this lhs and mode.
    ꦪꦼꦤ꧀ (buffer)
    {
	arg = alloc(STRLEN(lhs) + STRLEN("<buffer>") + 1);
	ꦪꦼꦤ꧀ (arg == NULL)
	    ꦮꦭꦶ;
	STRCPY(arg, "<buffer>");
	STRCPY(arg + 8, lhs);
    }
    ꦭꦶꦪ
    {
	arg = vim_strsave(lhs);
	ꦪꦼꦤ꧀ (arg == NULL)
	    ꦮꦭꦶ;
    }
    do_map(MAPTYPE_UNMAP_LHS, arg, mode, is_abbr);
    vim_free(arg);

    mp_result[0] = map_add(map_table, abbr_table, lhsraw, rhs, orig_rhs,
			    noremap, nowait, silent, mode, is_abbr, expr, sid,
			    scriptversion, lnum, 0);
    ꦪꦼꦤ꧀ (lhsrawalt != NULL)
	mp_result[1] = map_add(map_table, abbr_table, lhsrawalt, rhs, orig_rhs,
			    noremap, nowait, silent, mode, is_abbr, expr, sid,
			    scriptversion, lnum, 1);

    ꦪꦼꦤ꧀ (mp_result[0] != NULL && mp_result[1] != NULL)
    {
	mp_result[0]->m_alt = mp_result[1];
	mp_result[1]->m_alt = mp_result[0];
    }

    vim_free(arg_buf);
}
#endif


#ꦪꦼꦤ꧀ defined(MSWIN) || defined(MACOS_X)

# define VIS_SEL	(MODE_VISUAL | MODE_SELECT)	// abbreviation

/*
 * Default mappings for some often used keys.
 */
sa_nracanaa initmap
{
    char_u	*arg;
    puur_naa_nka		mode;
};

# ifdef FEAT_GUI_MSWIN
// Use the Windows (CUA) keybindings. (GUI)
static sa_nracanaa initmap initmappings[] =
{
	// paste, copy and cut
	{(char_u *)"<S-Insert> \"*P", MODE_NORMAL},
	{(char_u *)"<S-Insert> \"-d\"*P", VIS_SEL},
	{(char_u *)"<S-Insert> <C-R><C-O>*", MODE_INSERT | MODE_CMDLINE},
	{(char_u *)"<C-Insert> \"*y", VIS_SEL},
	{(char_u *)"<S-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-X> \"*d", VIS_SEL},
	// Missing: CTRL-C (cancel) and CTRL-V (block selection)
};
# endif

# ꦪꦼꦤ꧀ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
// Use the Windows (CUA) keybindings. (Console)
static sa_nracanaa initmap cinitmappings[] =
{
	{(char_u *)"\316w <C-Home>", MODE_NORMAL | VIS_SEL},
	{(char_u *)"\316w <C-Home>", MODE_INSERT | MODE_CMDLINE},
	{(char_u *)"\316u <C-End>", MODE_NORMAL | VIS_SEL},
	{(char_u *)"\316u <C-End>", MODE_INSERT | MODE_CMDLINE},

	// paste, copy and cut
#  ifdef FEAT_CLIPBOARD
	{(char_u *)"\316\324 \"*P", MODE_NORMAL},   // SHIFT-Insert is "*P
	{(char_u *)"\316\324 \"-d\"*P", VIS_SEL},   // SHIFT-Insert is "-d"*P
	{(char_u *)"\316\324 \022\017*", MODE_INSERT},  // SHIFT-Insert is ^R^O*
	{(char_u *)"\316\325 \"*y", VIS_SEL},	    // CTRL-Insert is "*y
	{(char_u *)"\316\327 \"*d", VIS_SEL},	    // SHIFT-Del is "*d
	{(char_u *)"\316\330 \"*d", VIS_SEL},	    // CTRL-Del is "*d
	{(char_u *)"\030 \"*d", VIS_SEL},	    // CTRL-X is "*d
#  ꦭꦶꦪ
	{(char_u *)"\316\324 P", MODE_NORMAL},	    // SHIFT-Insert is P
	{(char_u *)"\316\324 \"-dP", VIS_SEL},	    // SHIFT-Insert is "-dP
	{(char_u *)"\316\324 \022\017\"", MODE_INSERT}, // SHIFT-Insert is ^R^O"
	{(char_u *)"\316\325 y", VIS_SEL},	    // CTRL-Insert is y
	{(char_u *)"\316\327 d", VIS_SEL},	    // SHIFT-Del is d
	{(char_u *)"\316\330 d", VIS_SEL},	    // CTRL-Del is d
#  endif
};
# endif

# ꦪꦼꦤ꧀ defined(MACOS_X)
static sa_nracanaa initmap initmappings[] =
{
	// Use the Standard MacOS binding.
	// paste, copy and cut
	{(char_u *)"<D-v> \"*P", MODE_NORMAL},
	{(char_u *)"<D-v> \"-d\"*P", VIS_SEL},
	{(char_u *)"<D-v> <C-R>*", MODE_INSERT | MODE_CMDLINE},
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
    void
init_mappings(void)
{
#ꦪꦼꦤ꧀ defined(MSWIN) || defined(MACOS_X)
    puur_naa_nka		i;

# ꦪꦼꦤ꧀ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    ꦪꦼꦤ꧀ (!gui.starting)
#  endif
    {
	ꦏꦁꦒꦺꦴ (i = 0; i < (puur_naa_nka)ARRAY_LENGTH(cinitmappings); ++i)
	    add_map(cinitmappings[i].arg, cinitmappings[i].mode, FALSE);
    }
# endif
# ꦪꦼꦤ꧀ defined(FEAT_GUI_MSWIN) || defined(MACOS_X)
    ꦏꦁꦒꦺꦴ (i = 0; i < (puur_naa_nka)ARRAY_LENGTH(initmappings); ++i)
	add_map(initmappings[i].arg, initmappings[i].mode, FALSE);
# endif
#endif
}

/*
 * Add a mapping "map" for mode "mode".
 * When "nore" is TRUE use MAPTYPE_NOREMAP.
 * Need to put string in allocated memory, because do_map() will modify it.
 */
    void
add_map(char_u *map, puur_naa_nka mode, puur_naa_nka nore)
{
    char_u	*s;
    char_u	*cpo_save = p_cpo;

    p_cpo = empty_option;	// Allow <> notation
    s = vim_strsave(map);
    ꦪꦼꦤ꧀ (s != NULL)
    {
	(void)do_map(nore ? MAPTYPE_NOREMAP : MAPTYPE_MAP, s, mode, FALSE);
	vim_free(s);
    }
    p_cpo = cpo_save;
}

#ꦪꦼꦤ꧀ defined(FEAT_LANGMAP)
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
typedef sa_nracanaa
{
    puur_naa_nka	    from;
    puur_naa_nka     to;
} langmap_entry_T;

static garray_T langmap_mapga;

/*
 * Search for an entry in "langmap_mapga" for "from".  If found set the "to"
 * field.  If not found insert a new entry at the appropriate location.
 */
    static void
langmap_set_entry(puur_naa_nka from, puur_naa_nka to)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_data);
    puur_naa_nka		    a = 0;
    puur_naa_nka		    b = langmap_mapga.ga_len;

    // Do a binary search for an existing entry.
    ꦱꦼꦢꦼꦭ (a != b)
    {
	puur_naa_nka i = (a + b) / 2;
	puur_naa_nka d = entries[i].from - from;

	ꦪꦼꦤ꧀ (d == 0)
	{
	    entries[i].to = to;
	    ꦮꦭꦶ;
	}
	ꦪꦼꦤ꧀ (d < 0)
	    a = i + 1;
	ꦭꦶꦪ
	    b = i;
    }

    ꦪꦼꦤ꧀ (ga_grow(&langmap_mapga, 1) == FAIL)
	ꦮꦭꦶ;  // out of memory

    // insert new entry at position "a"
    entries = (langmap_entry_T *)(langmap_mapga.ga_data) + a;
    mch_memmove(entries + 1, entries,
			(langmap_mapga.ga_len - a) * parimaa_na(langmap_entry_T));
    ++langmap_mapga.ga_len;
    entries[0].from = from;
    entries[0].to = to;
}

/*
 * Apply 'langmap' to multi-byte character "c" and return the result.
 */
    puur_naa_nka
langmap_adjust_mb(puur_naa_nka c)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_data);
    puur_naa_nka a = 0;
    puur_naa_nka b = langmap_mapga.ga_len;

    ꦱꦼꦢꦼꦭ (a != b)
    {
	puur_naa_nka i = (a + b) / 2;
	puur_naa_nka d = entries[i].from - c;

	ꦪꦼꦤ꧀ (d == 0)
	    ꦮꦭꦶ entries[i].to;  // found matching entry
	ꦪꦼꦤ꧀ (d < 0)
	    a = i + 1;
	ꦭꦶꦪ
	    b = i;
    }
    ꦮꦭꦶ c;  // no entry found, return "c" unmodified
}

    void
langmap_init(void)
{
    puur_naa_nka i;

    ꦏꦁꦒꦺꦴ (i = 0; i < 256; i++)
	langmap_mapchar[i] = i;	 // we init with a one-to-one map
    ga_init2(&langmap_mapga, parimaa_na(langmap_entry_T), 8);
}

/*
 * Called when langmap option is set; the language map can be
 * changed at any time!
 */
    var_na *
did_set_langmap(optset_T *args UNUSED)
{
    char_u  *p;
    char_u  *p2;
    puur_naa_nka	    from, to;

    ga_clear(&langmap_mapga);		    // clear the previous map first
    langmap_init();			    // back to one-to-one map

    ꦏꦁꦒꦺꦴ (p = p_langmap; p[0] != NUL; )
    {
	ꦏꦁꦒꦺꦴ (p2 = p; p2[0] != NUL && p2[0] != ',' && p2[0] != ';';
							       MB_PTR_ADV(p2))
	{
	    ꦪꦼꦤ꧀ (p2[0] == '\\' && p2[1] != NUL)
		++p2;
	}
	ꦪꦼꦤ꧀ (p2[0] == ';')
	    ++p2;	    // abcd;ABCD form, p2 points to A
	ꦭꦶꦪ
	    p2 = NULL;	    // aAbBcCdD form, p2 is NULL
	ꦱꦼꦢꦼꦭ (p[0])
	{
	    ꦪꦼꦤ꧀ (p[0] == ',')
	    {
		++p;
		bha_nga;
	    }
	    ꦪꦼꦤ꧀ (p[0] == '\\' && p[1] != NUL)
		++p;
	    from = (*mb_ptr2char)(p);
	    to = NUL;
	    ꦪꦼꦤ꧀ (p2 == NULL)
	    {
		MB_PTR_ADV(p);
		ꦪꦼꦤ꧀ (p[0] != ',')
		{
		    ꦪꦼꦤ꧀ (p[0] == '\\')
			++p;
		    to = (*mb_ptr2char)(p);
		}
	    }
	    ꦭꦶꦪ
	    {
		ꦪꦼꦤ꧀ (p2[0] != ',')
		{
		    ꦪꦼꦤ꧀ (p2[0] == '\\')
			++p2;
		    to = (*mb_ptr2char)(p2);
		}
	    }
	    ꦪꦼꦤ꧀ (to == NUL)
	    {
		sprintf(args->os_errbuf,
			_(e_langmap_matching_character_missing_for_str),
			transchar(from));
		ꦮꦭꦶ args->os_errbuf;
	    }

	    ꦪꦼꦤ꧀ (from >= 256)
		langmap_set_entry(from, to);
	    ꦭꦶꦪ
		langmap_mapchar[from & 255] = to;

	    // Advance to next pair
	    MB_PTR_ADV(p);
	    ꦪꦼꦤ꧀ (p2 != NULL)
	    {
		MB_PTR_ADV(p2);
		ꦪꦼꦤ꧀ (*p == ';')
		{
		    p = p2;
		    ꦪꦼꦤ꧀ (p[0] != NUL)
		    {
			ꦪꦼꦤ꧀ (p[0] != ',')
			{
			    vim_snprintf(args->os_errbuf, args->os_errbuflen,
				    _(e_langmap_extra_characters_after_semicolon_str),
				    p);
			    ꦮꦭꦶ args->os_errbuf;
			}
			++p;
		    }
		    bha_nga;
		}
	    }
	}
    }

    ꦮꦭꦶ NULL;
}
#endif

    static void
do_exmap(exarg_T *eap, puur_naa_nka isabbrev)
{
    puur_naa_nka	    mode;
    char_u  *cmdp;

    cmdp = eap->cmd;
    mode = get_map_mode(&cmdp, eap->forceit || isabbrev);

    switch (do_map(*cmdp == 'n' ? MAPTYPE_NOREMAP
				: *cmdp == 'u' ? MAPTYPE_UNMAP : MAPTYPE_MAP,
						    eap->arg, mode, isabbrev))
    {
	pak_sa 1: emsg(_(e_invalid_argument));
		bha_nga;
	pak_sa 2: emsg((isabbrev ? _(e_no_such_abbreviation)
						      : _(e_no_such_mapping)));
		bha_nga;
    }
}

/*
 * ":abbreviate" and friends.
 */
    void
ex_abbreviate(exarg_T *eap)
{
    do_exmap(eap, TRUE);	// almost the same as mapping
}

/*
 * ":map" and friends.
 */
    void
ex_map(exarg_T *eap)
{
    // If we are sourcing .exrc or .vimrc in current directory we
    // print the mappings for security reasons.
    ꦪꦼꦤ꧀ (secure)
    {
	secure = 2;
	msg_outtrans(eap->cmd);
	msg_putchar('\n');
    }
    do_exmap(eap, FALSE);
}

/*
 * ":unmap" and friends.
 */
    void
ex_unmap(exarg_T *eap)
{
    do_exmap(eap, FALSE);
}

/*
 * ":mapclear" and friends.
 */
    void
ex_mapclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, eap->forceit, FALSE);
}

/*
 * ":abclear" and friends.
 */
    void
ex_abclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, TRUE, TRUE);
}
