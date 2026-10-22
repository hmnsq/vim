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
static ចំនួនគត់		maphash_valid = FALSE;

// When non-zero then no mappings can be added or removed.  Prevents mappings
// to change while listing them.
static ចំនួនគត់		map_locked = 0;

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
get_maphash_list(ចំនួនគត់ state, ចំនួនគត់ c)
{
    ត្រឡប់ maphash[MAP_HASH(state, c)];
}

/*
 * Get the buffer-local hashed map list for "state" and first character "c".
 */
    mapblock_T *
get_buf_maphash_list(ចំនួនគត់ state, ចំនួនគត់ c)
{
    ត្រឡប់ curbuf->b_maphash[MAP_HASH(state, c)];
}

    ចំនួនគត់
is_maphash_valid(void)
{
    ត្រឡប់ maphash_valid;
}

/*
 * Initialize maphash[] for first use.
 */
    static void
validate_maphash(void)
{
    ប្រសិនបើ (maphash_valid)
	ត្រឡប់;

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
    ប្រសិនបើ (mp->m_alt != NULL)
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
map_mode_to_chars(ចំនួនគត់ mode)
{
    garray_T    mapmode;

    ga_init2(&mapmode, 1, 7);

    ប្រសិនបើ ((mode & (MODE_INSERT | MODE_CMDLINE)) == (MODE_INSERT | MODE_CMDLINE))
	ga_append(&mapmode, '!');			// :map!
    បើមិនដូច្នេះ ប្រសិនបើ (mode & MODE_INSERT)
	ga_append(&mapmode, 'i');			// :imap
    បើមិនដូច្នេះ ប្រសិនបើ (mode & MODE_LANGMAP)
	ga_append(&mapmode, 'l');			// :lmap
    បើមិនដូច្នេះ ប្រសិនបើ (mode & MODE_CMDLINE)
	ga_append(&mapmode, 'c');			// :cmap
    បើមិនដូច្នេះ ប្រសិនបើ ((mode
		 & (MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING))
		== (MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING))
	ga_append(&mapmode, ' ');			// :map
    បើមិនដូច្នេះ
    {
	ប្រសិនបើ (mode & MODE_NORMAL)
	    ga_append(&mapmode, 'n');			// :nmap
	ប្រសិនបើ (mode & MODE_OP_PENDING)
	    ga_append(&mapmode, 'o');			// :omap
	ប្រសិនបើ (mode & MODE_TERMINAL)
	    ga_append(&mapmode, 't');			// :tmap
	ប្រសិនបើ ((mode & (MODE_VISUAL | MODE_SELECT)) == (MODE_VISUAL | MODE_SELECT))
	    ga_append(&mapmode, 'v');			// :vmap
	បើមិនដូច្នេះ
	{
	    ប្រសិនបើ (mode & MODE_VISUAL)
		ga_append(&mapmode, 'x');		// :xmap
	    ប្រសិនបើ (mode & MODE_SELECT)
		ga_append(&mapmode, 's');		// :smap
	}
    }

    ga_append(&mapmode, NUL);
    ត្រឡប់ (char_u *)mapmode.ga_data;
}

/*
 * Output a line for one mapping.
 */
    static void
showmap(
    mapblock_T	*mp,
    ចំនួនគត់		local)	    // TRUE for buffer-local map
{
    ចំនួនគត់		len = 1;
    char_u	*mapchars;

    ប្រសិនបើ (message_filtered(mp->m_keys) && message_filtered(mp->m_str))
	ត្រឡប់;

    // Prevent mappings to be cleared while at the more prompt.
    // Must jump to "theend" instead of returning.
    ++map_locked;

    ប្រសិនបើ (msg_didout || msg_silent != 0)
    {
	msg_putchar('\n');
	ប្រសិនបើ (got_int)	    // 'q' typed at MORE prompt
	    goto theend;
    }

    mapchars = map_mode_to_chars(mp->m_mode);
    ប្រសិនបើ (mapchars != NULL)
    {
	msg_puts((var_na *)mapchars);
	len = (ចំនួនគត់)STRLEN(mapchars);
	vim_free(mapchars);
    }

    ខណៈពេល (++len <= 3)
	msg_putchar(' ');

    // Display the LHS.  Get length of what we write.
    len = msg_outtrans_special(mp->m_keys, TRUE, 0);
    do
    {
	msg_putchar(' ');		// pad with blanks
	++len;
    } ខណៈពេល (len < 12);

    ប្រសិនបើ (mp->m_noremap == REMAP_NONE)
	msg_puts_attr("*", HL_ATTR(HLF_8));
    បើមិនដូច្នេះ ប្រសិនបើ (mp->m_noremap == REMAP_SCRIPT)
	msg_puts_attr("&", HL_ATTR(HLF_8));
    បើមិនដូច្នេះ
	msg_putchar(' ');

    ប្រសិនបើ (local)
	msg_putchar('@');
    បើមិនដូច្នេះ
	msg_putchar(' ');

    // Use FALSE below if we only want things like <Up> to show up as such on
    // the rhs, and not M-x etc, TRUE gets both -- webb
    ប្រសិនបើ (*mp->m_str == NUL)
	msg_puts_attr("<Nop>", HL_ATTR(HLF_8));
    បើមិនដូច្នេះ
	msg_outtrans_special(mp->m_str, FALSE, 0);
#ifdef FEAT_EVAL
    ប្រសិនបើ (p_verbose > 0)
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
	ចំនួនគត់	    noremap,
	ចំនួនគត់	    nowait,
	ចំនួនគត់	    silent,
	ចំនួនគត់	    mode,
	ចំនួនគត់	    is_abbr,
#ifdef FEAT_EVAL
	ចំនួនគត់	    expr,
	scid_T	    sid,	    // 0 to use current_sctx
	ចំនួនគត់	    scriptversion,
	linenr_T    lnum,
#endif
	ចំនួនគត់	    simplified)
{
    mapblock_T	*mp = ALLOC_CLEAR_ONE(mapblock_T);

    ប្រសិនបើ (mp == NULL)
	ត្រឡប់ NULL;

    // If CTRL-C has been mapped, don't always use it for Interrupting.
    ប្រសិនបើ (*keys == Ctrl_C)
    {
	ប្រសិនបើ (map_table == curbuf->b_maphash)
	    curbuf->b_mapped_ctrl_c |= mode;
	បើមិនដូច្នេះ
	    mapped_ctrl_c |= mode;
    }

    mp->m_keys = vim_strsave(keys);
    mp->m_str = vim_strsave(rhs);
    mp->m_orig_str = vim_strsave(orig_rhs);
    ប្រសិនបើ (mp->m_keys == NULL || mp->m_str == NULL)
    {
	vim_free(mp->m_keys);
	vim_free(mp->m_str);
	vim_free(mp->m_orig_str);
	vim_free(mp);
	ត្រឡប់ NULL;
    }
    mp->m_keylen = (ចំនួនគត់)STRLEN(mp->m_keys);
    mp->m_noremap = noremap;
    mp->m_nowait = nowait;
    mp->m_silent = silent;
    mp->m_mode = mode;
    mp->m_simplified = simplified;
#ifdef FEAT_EVAL
    mp->m_expr = expr;
    ប្រសិនបើ (sid != 0)
    {
	mp->m_script_ctx.sc_sid = sid;
	mp->m_script_ctx.sc_lnum = lnum;
	mp->m_script_ctx.sc_version = scriptversion;
    }
    បើមិនដូច្នេះ
    {
	mp->m_script_ctx = current_sctx;
	ប្រសិនបើ (cmdmod.cmod_flags & CMOD_LEGACY)
	    mp->m_script_ctx.sc_version = 1;
	mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
    }
#endif

    // add the new entry in front of the abbrlist or maphash[] list
    ប្រសិនបើ (is_abbr)
    {
	mp->m_next = *abbr_table;
	*abbr_table = mp;
    }
    បើមិនដូច្នេះ
    {
	ចំនួនគត់ n = MAP_HASH(mp->m_mode, mp->m_keys[0]);

	mp->m_next = map_table[n];
	map_table[n] = mp;
    }
    ត្រឡប់ mp;
}

/*
 * List mappings.  When "haskey" is FALSE all mappings, otherwise mappings that
 * match "keys[keys_len]".
 */
    static void
list_mappings(
	ចំនួនគត់	keyround,
	ចំនួនគត់	abbrev,
	ចំនួនគត់	haskey,
	char_u	*keys,
	ចំនួនគត់	keys_len,
	ចំនួនគត់	mode,
	ចំនួនគត់	*did_local)
{
    // Prevent mappings to be cleared while at the more prompt.
    ++map_locked;

    ប្រសិនបើ (p_verbose > 0 && keyround == 1)
    {
	ប្រសិនបើ (seenModifyOtherKeys)
	    msg_puts(_("Seen modifyOtherKeys: true\n"));

	ប្រសិនបើ (modify_otherkeys_state != MOKS_INITIAL)
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

	ប្រសិនបើ (kitty_protocol_state != KKPS_INITIAL)
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
    សម្រាប់ (ចំនួនគត់ hash = 0; hash < 256 && !got_int; ++hash)
    {
	mapblock_T	*mp;

	ប្រសិនបើ (abbrev)
	{
	    ប្រសិនបើ (hash != 0)	// there is only one abbreviation list
		bha_nga;
	    mp = curbuf->b_first_abbr;
	}
	បើមិនដូច្នេះ
	    mp = curbuf->b_maphash[hash];
	សម្រាប់ ( ; mp != NULL && !got_int; mp = mp->m_next)
	{
	    // check entries with the same mode
	    ប្រសិនបើ (!mp->m_simplified && (mp->m_mode & mode) != 0)
	    {
		ប្រសិនបើ (!haskey)		    // show all entries
		{
		    showmap(mp, TRUE);
		    *did_local = TRUE;
		}
		បើមិនដូច្នេះ
		{
		    ចំនួនគត់ n = mp->m_keylen;
		    ប្រសិនបើ (STRNCMP(mp->m_keys, keys,
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
    ចំនួនគត់
do_map(
    ចំនួនគត់		maptype,
    char_u	*arg,
    ចំនួនគត់		mode,
    ចំនួនគត់		abbrev)		// not a mapping but an abbreviation
{
    char_u	*keys;
    mapblock_T	*mp, **mpp;
    mapblock_T	*mp_result[2] = {NULL, NULL};
    char_u	*rhs;
    char_u	*p;
    ចំនួនគត់		n;
    ចំនួនគត់		len = 0;	// init for GCC
    ចំនួនគត់		hasarg;
    ចំនួនគត់		haskey;
    ចំនួនគត់		do_print;
    ចំនួនគត់		keyround;
    char_u	*keys_buf = NULL;
    char_u	*alt_keys_buf = NULL;
    char_u	*arg_buf = NULL;
    ចំនួនគត់		retval = 0;
    ចំនួនគត់		do_backslash;
    mapblock_T	**abbr_table;
    mapblock_T	**map_table;
    ចំនួនគត់		unique = FALSE;
    ចំនួនគត់		nowait = FALSE;
    ចំនួនគត់		silent = FALSE;
    ចំនួនគត់		special = FALSE;
#ifdef FEAT_EVAL
    ចំនួនគត់		expr = FALSE;
#endif
    ចំនួនគត់		did_simplify = FALSE;
    ចំនួនគត់		unmap_lhs_only = FALSE;
    ចំនួនគត់		noremap;
    char_u      *orig_rhs;

    keys = arg;
    map_table = maphash;
    abbr_table = &first_abbr;

    ប្រសិនបើ (maptype == MAPTYPE_UNMAP_LHS)
    {
	unmap_lhs_only = TRUE;
	maptype = MAPTYPE_UNMAP;
    }

    // For ":noremap" don't remap, otherwise do remap.
    ប្រសិនបើ (maptype == MAPTYPE_NOREMAP)
	noremap = REMAP_NONE;
    បើមិនដូច្នេះ
	noremap = REMAP_YES;

    // Accept <buffer>, <nowait>, <silent>, <expr> <script> and <unique> in
    // any order.
    សម្រាប់ (;;)
    {
	// Check for "<buffer>": mapping local to buffer.
	ប្រសិនបើ (STRNCMP(keys, "<buffer>", 8) == 0)
	{
	    keys = skipwhite(keys + 8);
	    map_table = curbuf->b_maphash;
	    abbr_table = &curbuf->b_first_abbr;
	    continue;
	}

	// Check for "<nowait>": don't wait for more characters.
	ប្រសិនបើ (STRNCMP(keys, "<nowait>", 8) == 0)
	{
	    keys = skipwhite(keys + 8);
	    nowait = TRUE;
	    continue;
	}

	// Check for "<silent>": don't echo commands.
	ប្រសិនបើ (STRNCMP(keys, "<silent>", 8) == 0)
	{
	    keys = skipwhite(keys + 8);
	    silent = TRUE;
	    continue;
	}

	// Check for "<special>": accept special keys in <>
	ប្រសិនបើ (STRNCMP(keys, "<special>", 9) == 0)
	{
	    keys = skipwhite(keys + 9);
	    special = TRUE;
	    continue;
	}

#ifdef FEAT_EVAL
	// Check for "<script>": remap script-local mappings only
	ប្រសិនបើ (STRNCMP(keys, "<script>", 8) == 0)
	{
	    keys = skipwhite(keys + 8);
	    noremap = REMAP_SCRIPT;
	    continue;
	}

	// Check for "<expr>": {rhs} is an expression.
	ប្រសិនបើ (STRNCMP(keys, "<expr>", 6) == 0)
	{
	    keys = skipwhite(keys + 6);
	    expr = TRUE;
	    continue;
	}
#endif
	// Check for "<unique>": don't overwrite an existing mapping.
	ប្រសិនបើ (STRNCMP(keys, "<unique>", 8) == 0)
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
    ខណៈពេល (*p && (maptype == MAPTYPE_UNMAP || !VIM_ISWHITE(*p)))
    {
	ប្រសិនបើ ((p[0] == Ctrl_V || (do_backslash && p[0] == '\\')) &&
								  p[1] != NUL)
	    ++p;		// skip CTRL-V or backslash
	++p;
    }
    ប្រសិនបើ (*p != NUL)
	*p++ = NUL;

    p = skipwhite(p);
    rhs = p;
    hasarg = (*rhs != NUL);
    haskey = (*keys != NUL);
    do_print = !haskey || (maptype != MAPTYPE_UNMAP && !hasarg);

    // check for :unmap without argument
    ប្រសិនបើ (maptype == MAPTYPE_UNMAP && !haskey)
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
    ប្រសិនបើ (haskey)
    {
	char_u	*new_keys;
	ចំនួនគត់	flags = REPTERM_FROM_PART | REPTERM_DO_LT;

	ប្រសិនបើ (special)
	    flags |= REPTERM_SPECIAL;
	new_keys = replace_termcodes(keys, &keys_buf, 0, flags, &did_simplify);
	ប្រសិនបើ (did_simplify)
	    (void)replace_termcodes(keys, &alt_keys_buf, 0,
					    flags | REPTERM_NO_SIMPLIFY, NULL);
	keys = new_keys;
    }
    orig_rhs = rhs;
    ប្រសិនបើ (hasarg)
    {
	ប្រសិនបើ (STRICMP(rhs, "<nop>") == 0)	    // "<Nop>" means nothing
	    rhs = (char_u *)"";
	បើមិនដូច្នេះ
	    rhs = replace_termcodes(rhs, &arg_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), NULL);
    }

    /*
     * The following is done twice if we have two versions of keys:
     * "alt_keys_buf" is not NULL.
     */
    សម្រាប់ (keyround = 1; keyround <= 2; ++keyround)
    {
	ចំនួនគត់	did_it = FALSE;
	ចំនួនគត់	did_local = FALSE;
	ចំនួនគត់	keyround1_simplified = keyround == 1 && did_simplify;
	ចំនួនគត់	round;
	ចំនួនគត់	num_rounds;

	ប្រសិនបើ (keyround == 2)
	{
	    ប្រសិនបើ (alt_keys_buf == NULL)
		bha_nga;
	    keys = alt_keys_buf;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (alt_keys_buf != NULL && do_print)
	    // when printing always use the not-simplified map
	    keys = alt_keys_buf;

	// check arguments and translate function keys
	ប្រសិនបើ (haskey)
	{
	    len = (ចំនួនគត់)STRLEN(keys);
	    ប្រសិនបើ (len > MAXMAPLEN)	// maximum length of MAXMAPLEN chars
	    {
		retval = 1;
		goto theend;
	    }

	    ប្រសិនបើ (abbrev && maptype != MAPTYPE_UNMAP)
	    {
		// If an abbreviation ends in a keyword character, the
		// rest must be all keyword-char or all non-keyword-char.
		// Otherwise we won't be able to find the start of it in a
		// vi-compatible way.
		ប្រសិនបើ (has_mbyte)
		{
		    ចំនួនគត់		first, last;
		    ចំនួនគត់		same = -1;
		    char_u	keys_unescaped[MAXMAPLEN + 1];
		    size_t	keys_unescaped_len;

		    mch_memmove(keys_unescaped, keys, (size_t)(len + 1));
		    keys_unescaped_len = vim_unescape_csi(keys_unescaped);
		    p = keys_unescaped;

		    first = vim_iswordp(p);
		    last = first;
		    MB_PTR_ADV(p);
		    n = 1;
		    ខណៈពេល (p < keys_unescaped + keys_unescaped_len)
		    {
			++n;			// nr of (multi-byte) chars
			last = vim_iswordp(p);	// type of last char
			ប្រសិនបើ (same == -1 && last != first)
			    same = n - 1;	// count of same char type
			MB_PTR_ADV(p);
		    }
		    ប្រសិនបើ (last && n > 2 && same >= 0 && same < n - 1)
		    {
			retval = 1;
			goto theend;
		    }
		}
		បើមិនដូច្នេះ ប្រសិនបើ (vim_iswordc(keys[len - 1]))
		    // ends in keyword char
		    សម្រាប់ (n = 0; n < len - 2; ++n)
			ប្រសិនបើ (vim_iswordc(keys[n]) != vim_iswordc(keys[len - 2]))
			{
			    retval = 1;
			    goto theend;
			}
		// An abbreviation cannot contain white space.
		សម្រាប់ (n = 0; n < len; ++n)
		    ប្រសិនបើ (VIM_ISWHITE(keys[n]))
		    {
			retval = 1;
			goto theend;
		    }
	    }
	}

	ប្រសិនបើ (haskey && hasarg && abbrev)	// if we will add an abbreviation
	    no_abbr = FALSE;		// reset flag that indicates there are
					// no abbreviations

	ប្រសិនបើ (do_print)
	    msg_start();

	// Check if a new local mapping wasn't already defined globally.
	ប្រសិនបើ (unique && map_table == curbuf->b_maphash
			       && haskey && hasarg && maptype != MAPTYPE_UNMAP)
	{
	    // need to loop over all global hash lists
	    សម្រាប់ (ចំនួនគត់ hash = 0; hash < 256 && !got_int; ++hash)
	    {
		ប្រសិនបើ (abbrev)
		{
		    ប្រសិនបើ (hash != 0)	// there is only one abbreviation list
			bha_nga;
		    mp = first_abbr;
		}
		បើមិនដូច្នេះ
		    mp = maphash[hash];
		សម្រាប់ ( ; mp != NULL && !got_int; mp = mp->m_next)
		{
		    // check entries with the same mode
		    ប្រសិនបើ ((mp->m_mode & mode) != 0
			    && mp->m_keylen == len
			    && STRNCMP(mp->m_keys, keys, (size_t)len) == 0)
		    {
			ប្រសិនបើ (abbrev)
			    semsg(
			       _(e_global_abbreviation_already_exists_for_str),
				    mp->m_keys);
			បើមិនដូច្នេះ
			    semsg(_(e_global_mapping_already_exists_for_str),
				    mp->m_keys);
			retval = 5;
			goto theend;
		    }
		}
	    }
	}

	// When listing global mappings, also list buffer-local ones here.
	ប្រសិនបើ (map_table != curbuf->b_maphash && !hasarg
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
	សម្រាប់ (round = 0; round < num_rounds && !did_it && !got_int; ++round)
	{
	    // need to loop over all hash lists
	    សម្រាប់ (ចំនួនគត់ hash = 0; hash < 256 && !got_int; ++hash)
	    {
		ប្រសិនបើ (abbrev)
		{
		    ប្រសិនបើ (hash > 0)	// there is only one abbreviation list
			bha_nga;
		    mpp = abbr_table;
		}
		បើមិនដូច្នេះ
		    mpp = &(map_table[hash]);
		សម្រាប់ (mp = *mpp; mp != NULL && !got_int; mp = *mpp)
		{

		    ប្រសិនបើ ((mp->m_mode & mode) == 0)
		    {
			// skip entries with wrong mode
			mpp = &(mp->m_next);
			continue;
		    }
		    ប្រសិនបើ (!haskey)	// show all entries
		    {
			ប្រសិនបើ (!mp->m_simplified)
			{
			    showmap(mp, map_table != maphash);
			    did_it = TRUE;
			}
		    }
		    បើមិនដូច្នេះ	// do we have a match?
		    {
			ប្រសិនបើ (round)	// second round: Try unmap "rhs" string
			{
			    n = (ចំនួនគត់)STRLEN(mp->m_str);
			    p = mp->m_str;
			}
			បើមិនដូច្នេះ
			{
			    n = mp->m_keylen;
			    p = mp->m_keys;
			}
			ប្រសិនបើ (STRNCMP(p, keys, (size_t)(n < len ? n : len)) == 0)
			{
			    ប្រសិនបើ (maptype == MAPTYPE_UNMAP)
			    {
				// Delete entry.
				// Only accept a full match.  For abbreviations
				// we ignore trailing space when matching with
				// the "lhs", since an abbreviation can't have
				// trailing space.
				ប្រសិនបើ (n != len && (!abbrev || round || n > len
					       || *skipwhite(keys + n) != NUL))
				{
				    mpp = &(mp->m_next);
				    continue;
				}
				// In keyround for simplified keys, don't unmap
				// a mapping without m_simplified flag.
				ប្រសិនបើ (keyround1_simplified && !mp->m_simplified)
				    bha_nga;
				// We reset the indicated mode bits. If nothing
				// is left the entry is deleted below.
				mp->m_mode &= ~mode;
				did_it = TRUE;	// remember we did something
			    }
			    បើមិនដូច្នេះ ប្រសិនបើ (!hasarg)	// show matching entry
			    {
				ប្រសិនបើ (!mp->m_simplified)
				{
				    showmap(mp, map_table != maphash);
				    did_it = TRUE;
				}
			    }
			    បើមិនដូច្នេះ ប្រសិនបើ (n != len)	// new entry is ambiguous
			    {
				mpp = &(mp->m_next);
				continue;
			    }
			    បើមិនដូច្នេះ ប្រសិនបើ (unique)
			    {
				ប្រសិនបើ (abbrev)
				    semsg(
				      _(e_abbreviation_already_exists_for_str),
					    p);
				បើមិនដូច្នេះ
				    semsg(_(e_mapping_already_exists_for_str),
					    p);
				retval = 5;
				goto theend;
			    }
			    បើមិនដូច្នេះ
			    {
				// new rhs for existing entry
				mp->m_mode &= ~mode;	// remove mode bits
				ប្រសិនបើ (mp->m_mode == 0 && !did_it) // reuse entry
				{
				    char_u *newstr = vim_strsave(rhs);

				    ប្រសិនបើ (newstr == NULL)
				    {
					retval = 4;		// no mem
					goto theend;
				    }
				    ប្រសិនបើ (mp->m_alt != NULL)
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
				    ប្រសិនបើ (cmdmod.cmod_flags & CMOD_LEGACY)
					mp->m_script_ctx.sc_version = 1;
				    mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
#endif
				    mp_result[keyround - 1] = mp;
				    did_it = TRUE;
				}
			    }
			    ប្រសិនបើ (mp->m_mode == 0)  // entry can be deleted
			    {
				map_free(mpp);
				continue;	// continue with *mpp
			    }

			    // May need to put this entry into another hash
			    // list.
			    ចំនួនគត់ new_hash = MAP_HASH(mp->m_mode, mp->m_keys[0]);
			    ប្រសិនបើ (!abbrev && new_hash != hash)
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

	ប្រសិនបើ (maptype == MAPTYPE_UNMAP)
	{
	    // delete entry
	    ប្រសិនបើ (!did_it)
	    {
		ប្រសិនបើ (!keyround1_simplified)
		    retval = 2;		// no match
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (*keys == Ctrl_C)
	    {
		// If CTRL-C has been unmapped, reuse it for Interrupting.
		ប្រសិនបើ (map_table == curbuf->b_maphash)
		    curbuf->b_mapped_ctrl_c &= ~mode;
		បើមិនដូច្នេះ
		    mapped_ctrl_c &= ~mode;
	    }
	    continue;
	}

	ប្រសិនបើ (!haskey || !hasarg)
	{
	    // print entries
	    ប្រសិនបើ (!did_it && !did_local)
	    {
		ប្រសិនបើ (abbrev)
		    msg(_("No abbreviation found"));
		បើមិនដូច្នេះ
		    msg(_("No mapping found"));
	    }
	    goto theend;    // listing finished
	}

	ប្រសិនបើ (did_it)
	    continue;	// have added the new entry already

	// Get here when adding a new entry to the maphash[] list or abbrlist.
	mp_result[keyround - 1] = map_add(map_table, abbr_table, keys,
		    rhs, orig_rhs, noremap, nowait, silent, mode, abbrev,
#ifdef FEAT_EVAL
		    expr, /* sid */ 0, /* scriptversion */ 0, /* lnum */ 0,
#endif
		    keyround1_simplified);
	ប្រសិនបើ (mp_result[keyround - 1] == NULL)
	{
	    retval = 4;	    // no mem
	    goto theend;
	}
    }

    ប្រសិនបើ (mp_result[0] != NULL && mp_result[1] != NULL)
    {
	mp_result[0]->m_alt = mp_result[1];
	mp_result[1]->m_alt = mp_result[0];
    }

theend:
    vim_free(keys_buf);
    vim_free(alt_keys_buf);
    vim_free(arg_buf);
    ត្រឡប់ retval;
}

/*
 * Get the mapping mode from the command name.
 */
    static ចំនួនគត់
get_map_mode(char_u **cmdp, ចំនួនគត់ forceit)
{
    char_u	*p;
    ចំនួនគត់		modec;
    ចំនួនគត់		mode;

    p = *cmdp;
    modec = *p++;
    ប្រសិនបើ (modec == 'i')
	mode = MODE_INSERT;				// :imap
    បើមិនដូច្នេះ ប្រសិនបើ (modec == 'l')
	mode = MODE_LANGMAP;				// :lmap
    បើមិនដូច្នេះ ប្រសិនបើ (modec == 'c')
	mode = MODE_CMDLINE;				// :cmap
    បើមិនដូច្នេះ ប្រសិនបើ (modec == 'n' && *p != 'o')		    // avoid :noremap
	mode = MODE_NORMAL;				// :nmap
    បើមិនដូច្នេះ ប្រសិនបើ (modec == 'v')
	mode = MODE_VISUAL | MODE_SELECT;		// :vmap
    បើមិនដូច្នេះ ប្រសិនបើ (modec == 'x')
	mode = MODE_VISUAL;				// :xmap
    បើមិនដូច្នេះ ប្រសិនបើ (modec == 's')
	mode = MODE_SELECT;			// :smap
    បើមិនដូច្នេះ ប្រសិនបើ (modec == 'o')
	mode = MODE_OP_PENDING;			// :omap
    បើមិនដូច្នេះ ប្រសិនបើ (modec == 't')
	mode = MODE_TERMINAL;			// :tmap
    បើមិនដូច្នេះ
    {
	--p;
	ប្រសិនបើ (forceit)
	    mode = MODE_INSERT | MODE_CMDLINE;		// :map !
	បើមិនដូច្នេះ
	    mode = MODE_VISUAL | MODE_SELECT | MODE_NORMAL | MODE_OP_PENDING;
							// :map
    }

    *cmdp = p;
    ត្រឡប់ mode;
}

/*
 * Clear all mappings (":mapclear") or abbreviations (":abclear").
 * "abbr" should be FALSE for mappings, TRUE for abbreviations.
 */
    static void
map_clear(
    char_u	*cmdp,
    char_u	*arg,
    ចំនួនគត់		forceit,
    ចំនួនគត់		abbr)
{
    ចំនួនគត់		mode;
    ចំនួនគត់		local;

    local = (STRCMP(arg, "<buffer>") == 0);
    ប្រសិនបើ (!local && *arg != NUL)
    {
	emsg(_(e_invalid_argument));
	ត្រឡប់;
    }

    mode = get_map_mode(&cmdp, forceit);
    map_clear_mode(curbuf, mode, local, abbr);
}

/*
 * If "map_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    static ចំនួនគត់
is_map_locked(void)
{
    ប្រសិនបើ (map_locked > 0)
    {
	emsg(_(e_cannot_change_mappings_while_listing));
	ត្រឡប់ TRUE;
    }
    ត្រឡប់ FALSE;
}

/*
 * Clear all mappings in "mode".
 */
    void
map_clear_mode(
    buf_T	*buf,		// buffer for local mappings
    ចំនួនគត់		mode,		// mode in which to delete
    ចំនួនគត់		local,		// TRUE for buffer-local mappings
    ចំនួនគត់		abbr)		// TRUE for abbreviations
{
    mapblock_T	*mp, **mpp;
    ចំនួនគត់		hash;
    ចំនួនគត់		new_hash;

    ប្រសិនបើ (is_map_locked())
	ត្រឡប់;

    validate_maphash();

    សម្រាប់ (hash = 0; hash < 256; ++hash)
    {
	ប្រសិនបើ (abbr)
	{
	    ប្រសិនបើ (hash > 0)	// there is only one abbrlist
		bha_nga;
	    ប្រសិនបើ (local)
		mpp = &buf->b_first_abbr;
	    បើមិនដូច្នេះ
		mpp = &first_abbr;
	}
	បើមិនដូច្នេះ
	{
	    ប្រសិនបើ (local)
		mpp = &buf->b_maphash[hash];
	    បើមិនដូច្នេះ
		mpp = &maphash[hash];
	}
	ខណៈពេល (*mpp != NULL)
	{
	    mp = *mpp;
	    ប្រសិនបើ (mp->m_mode & mode)
	    {
		mp->m_mode &= ~mode;
		ប្រសិនបើ (mp->m_mode == 0) // entry can be deleted
		{
		    map_free(mpp);
		    continue;
		}
		// May need to put this entry into another hash list.
		new_hash = MAP_HASH(mp->m_mode, mp->m_keys[0]);
		ប្រសិនបើ (!abbr && new_hash != hash)
		{
		    *mpp = mp->m_next;
		    ប្រសិនបើ (local)
		    {
			mp->m_next = buf->b_maphash[new_hash];
			buf->b_maphash[new_hash] = mp;
		    }
		    បើមិនដូច្នេះ
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

#ប្រសិនបើ defined(FEAT_EVAL)
    ចំនួនគត់
mode_str2flags(char_u *modechars)
{
    ចំនួនគត់		mode = 0;

    ប្រសិនបើ (vim_strchr(modechars, 'n') != NULL)
	mode |= MODE_NORMAL;
    ប្រសិនបើ (vim_strchr(modechars, 'v') != NULL)
	mode |= MODE_VISUAL | MODE_SELECT;
    ប្រសិនបើ (vim_strchr(modechars, 'x') != NULL)
	mode |= MODE_VISUAL;
    ប្រសិនបើ (vim_strchr(modechars, 's') != NULL)
	mode |= MODE_SELECT;
    ប្រសិនបើ (vim_strchr(modechars, 'o') != NULL)
	mode |= MODE_OP_PENDING;
    ប្រសិនបើ (vim_strchr(modechars, 'i') != NULL)
	mode |= MODE_INSERT;
    ប្រសិនបើ (vim_strchr(modechars, 'l') != NULL)
	mode |= MODE_LANGMAP;
    ប្រសិនបើ (vim_strchr(modechars, 'c') != NULL)
	mode |= MODE_CMDLINE;

    ត្រឡប់ mode;
}

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "modechars".
 * Recognize termcap codes in "str".
 * Also checks mappings local to the current buffer.
 */
    ចំនួនគត់
map_to_exists(char_u *str, char_u *modechars, ចំនួនគត់ abbr)
{
    char_u	*rhs;
    char_u	*buf;
    ចំនួនគត់		retval;

    rhs = replace_termcodes(str, &buf, 0, REPTERM_DO_LT, NULL);

    retval = map_to_exists_mode(rhs, mode_str2flags(modechars), abbr);
    vim_free(buf);

    ត្រឡប់ retval;
}
#endif

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "mode".
 * Also checks mappings local to the current buffer.
 */
    ចំនួនគត់
map_to_exists_mode(char_u *rhs, ចំនួនគត់ mode, ចំនួនគត់ abbr)
{
    mapblock_T	*mp;
    ចំនួនគត់		hash;
    ចំនួនគត់		exp_buffer = FALSE;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    សម្រាប់ (;;)
    {
	សម្រាប់ (hash = 0; hash < 256; ++hash)
	{
	    ប្រសិនបើ (abbr)
	    {
		ប្រសិនបើ (hash > 0)		// there is only one abbr list
		    bha_nga;
		ប្រសិនបើ (exp_buffer)
		    mp = curbuf->b_first_abbr;
		បើមិនដូច្នេះ
		    mp = first_abbr;
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (exp_buffer)
		mp = curbuf->b_maphash[hash];
	    បើមិនដូច្នេះ
		mp = maphash[hash];
	    សម្រាប់ (; mp; mp = mp->m_next)
	    {
		ប្រសិនបើ ((mp->m_mode & mode)
			&& strstr((var_na *)mp->m_str, (var_na *)rhs) != NULL)
		    ត្រឡប់ TRUE;
	    }
	}
	ប្រសិនបើ (exp_buffer)
	    bha_nga;
	exp_buffer = TRUE;
    }

    ត្រឡប់ FALSE;
}

/*
 * Used below when expanding mapping/abbreviation names.
 */
static ចំនួនគត់	expand_mapmodes = 0;
static ចំនួនគត់	expand_isabbrev = 0;
static ចំនួនគត់	expand_buffer = FALSE;

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
    ចំនួនគត់		c;
    ចំនួនគត់		modifiers;
    ចំនួនគត់		cpo_bslash;
    ចំនួនគត់		cpo_special;

    ga_init(&ga);
    ga.ga_itemsize = 1;
    ga.ga_growsize = 40;

    cpo_bslash = (vim_strchr(p_cpo, CPO_BSLASH) != NULL);
    cpo_special = (vim_strchr(p_cpo, CPO_SPECI) != NULL);

    សម្រាប់ (; *str; ++str)
    {
	c = *str;
	ប្រសិនបើ (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	{
	    modifiers = 0;
	    ប្រសិនបើ (str[1] == KS_MODIFIER)
	    {
		str++;
		modifiers = *++str;
		c = *++str;
	    }
	    ប្រសិនបើ (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	    {
		ប្រសិនបើ (cpo_special)
		{
		    ga_clear(&ga);
		    ត្រឡប់ NULL;
		}
		c = TO_SPECIAL(str[1], str[2]);
		ប្រសិនបើ (c == K_ZERO)	// display <Nul> as ^@
		    c = NUL;
		str += 2;
	    }
	    ប្រសិនបើ (IS_SPECIAL(c) || modifiers)	// special key
	    {
		ប្រសិនបើ (cpo_special)
		{
		    ga_clear(&ga);
		    ត្រឡប់ NULL;
		}
		ga_concat(&ga, get_special_key_name(c, modifiers));
		continue; // for (str)
	    }
	}
	ប្រសិនបើ (c == ' ' || c == '\t' || c == Ctrl_J || c == Ctrl_V
	    || (c == '<' && !cpo_special) || (c == '\\' && !cpo_bslash))
	    ga_append(&ga, cpo_bslash ? Ctrl_V : '\\');
	ប្រសិនបើ (c)
	    ga_append(&ga, c);
    }
    ga_append(&ga, NUL);
    ត្រឡប់ (char_u *)(ga.ga_data);
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
    ចំនួនគត់		forceit,	// TRUE if '!' given
    ចំនួនគត់		isabbrev,	// TRUE if abbreviation
    ចំនួនគត់		isunmap,	// TRUE if unmap/unabbrev command
    cmdidx_T	cmdidx)
{
    ប្រសិនបើ (forceit && cmdidx != CMD_map && cmdidx != CMD_unmap)
	xp->xp_context = EXPAND_NOTHING;
    បើមិនដូច្នេះ
    {
	ប្រសិនបើ (isunmap)
	    expand_mapmodes = get_map_mode(&cmd, forceit || isabbrev);
	បើមិនដូច្នេះ
	{
	    expand_mapmodes = MODE_INSERT | MODE_CMDLINE;
	    ប្រសិនបើ (!isabbrev)
		expand_mapmodes += MODE_VISUAL | MODE_SELECT | MODE_NORMAL
							     | MODE_OP_PENDING;
	}
	expand_isabbrev = isabbrev;
	xp->xp_context = EXPAND_MAPPINGS;
	expand_buffer = FALSE;
	សម្រាប់ (;;)
	{
	    ប្រសិនបើ (STRNCMP(arg, "<buffer>", 8) == 0)
	    {
		expand_buffer = TRUE;
		arg = skipwhite(arg + 8);
		continue;
	    }
	    ប្រសិនបើ (STRNCMP(arg, "<unique>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		continue;
	    }
	    ប្រសិនបើ (STRNCMP(arg, "<nowait>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		continue;
	    }
	    ប្រសិនបើ (STRNCMP(arg, "<silent>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		continue;
	    }
	    ប្រសិនបើ (STRNCMP(arg, "<special>", 9) == 0)
	    {
		arg = skipwhite(arg + 9);
		continue;
	    }
#ifdef FEAT_EVAL
	    ប្រសិនបើ (STRNCMP(arg, "<script>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		continue;
	    }
	    ប្រសិនបើ (STRNCMP(arg, "<expr>", 6) == 0)
	    {
		arg = skipwhite(arg + 6);
		continue;
	    }
#endif
	    bha_nga;
	}
	xp->xp_pattern = arg;
    }

    ត្រឡប់ NULL;
}

/*
 * Find all mapping/abbreviation names that match regexp "regmatch"'.
 * For command line expansion of ":[un]map" and ":[un]abbrev" in all modes.
 * Return OK if matches found, FAIL otherwise.
 */
    ចំនួនគត់
ExpandMappings(
    char_u	*pat,
    regmatch_T	*regmatch,
    ចំនួនគត់		*numMatches,
    char_u	***matches)
{
    mapblock_T	*mp;
    garray_T	ga;
    ចំនួនគត់		hash;
    ចំនួនគត់		count;
    char_u	*p;
    ចំនួនគត់		i;
    ចំនួនគត់		fuzzy;
    ចំនួនគត់		match;
    ចំនួនគត់		score = 0;
    fuzmatch_str_T  *fuzmatch;

    fuzzy = cmdline_fuzzy_complete(pat);

    validate_maphash();

    *numMatches = 0;		    // return values in case of FAIL
    *matches = NULL;

    ប្រសិនបើ (!fuzzy)
	ga_init2(&ga, parimaa_na(var_na *), 3);
    បើមិនដូច្នេះ
	ga_init2(&ga, parimaa_na(fuzmatch_str_T), 3);

    // First search in map modifier arguments
    សម្រាប់ (i = 0; i < 7; ++i)
    {
	ប្រសិនបើ (i == 0)
	    p = (char_u *)"<silent>";
	បើមិនដូច្នេះ ប្រសិនបើ (i == 1)
	    p = (char_u *)"<unique>";
#ifdef FEAT_EVAL
	បើមិនដូច្នេះ ប្រសិនបើ (i == 2)
	    p = (char_u *)"<script>";
	បើមិនដូច្នេះ ប្រសិនបើ (i == 3)
	    p = (char_u *)"<expr>";
#endif
	បើមិនដូច្នេះ ប្រសិនបើ (i == 4 && !expand_buffer)
	    p = (char_u *)"<buffer>";
	បើមិនដូច្នេះ ប្រសិនបើ (i == 5)
	    p = (char_u *)"<nowait>";
	បើមិនដូច្នេះ ប្រសិនបើ (i == 6)
	    p = (char_u *)"<special>";
	បើមិនដូច្នេះ
	    continue;

	ប្រសិនបើ (!fuzzy)
	    match = vim_regexec(regmatch, p, (colnr_T)0);
	បើមិនដូច្នេះ
	{
	    score = fuzzy_match_str(p, pat);
	    match = (score != FUZZY_SCORE_NONE);
	}

	ប្រសិនបើ (!match)
	    continue;

	ប្រសិនបើ (ga_grow(&ga, 1) == FAIL)
	    bha_nga;

	ប្រសិនបើ (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = vim_strsave(p);
	    fuzmatch->score = score;
	}
	បើមិនដូច្នេះ
	    ((char_u **)ga.ga_data)[ga.ga_len] = vim_strsave(p);
	++ga.ga_len;
    }

    សម្រាប់ (hash = 0; hash < 256; ++hash)
    {
	ប្រសិនបើ (expand_isabbrev)
	{
	    ប្រសិនបើ (hash > 0)	// only one abbrev list
		bha_nga; // for (hash)
	    mp = first_abbr;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (expand_buffer)
	    mp = curbuf->b_maphash[hash];
	បើមិនដូច្នេះ
	    mp = maphash[hash];
	សម្រាប់ (; mp; mp = mp->m_next)
	{
	    ប្រសិនបើ (mp->m_simplified || !(mp->m_mode & expand_mapmodes))
		continue;

	    p = translate_mapping(mp->m_keys);
	    ប្រសិនបើ (p == NULL)
		continue;

	    ប្រសិនបើ (!fuzzy)
		match = vim_regexec(regmatch, p, (colnr_T)0);
	    បើមិនដូច្នេះ
	    {
		score = fuzzy_match_str(p, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }

	    ប្រសិនបើ (!match)
	    {
		vim_free(p);
		continue;
	    }

	    ប្រសិនបើ (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		bha_nga;
	    }

	    ប្រសិនបើ (fuzzy)
	    {
		fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    បើមិនដូច្នេះ
		((char_u **)ga.ga_data)[ga.ga_len] = p;

	    ++ga.ga_len;
	} // for (mp)
    } // for (hash)

    ប្រសិនបើ (ga.ga_len == 0)
	ត្រឡប់ FAIL;

    ប្រសិនបើ (!fuzzy)
    {
	*matches = ga.ga_data;
	*numMatches = ga.ga_len;
    }
    បើមិនដូច្នេះ
    {
	ប្រសិនបើ (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
							FALSE) == FAIL)
	    ត្រឡប់ FAIL;
	*numMatches = ga.ga_len;
    }

    count = *numMatches;
    ប្រសិនបើ (count > 1)
    {
	char_u	**ptr1;
	char_u	**ptr2;
	char_u	**ptr3;

	// Sort the matches
	// Fuzzy matching already sorts the matches
	ប្រសិនបើ (!fuzzy)
	    sort_strings(*matches, count);

	// Remove multiple entries
	ptr1 = *matches;
	ptr2 = ptr1 + 1;
	ptr3 = ptr1 + count;

	ខណៈពេល (ptr2 < ptr3)
	{
	    ប្រសិនបើ (STRCMP(*ptr1, *ptr2) != 0)
		*++ptr1 = *ptr2++;
	    បើមិនដូច្នេះ
	    {
		vim_free(*ptr2++);
		count--;
	    }
	}
    }

    *numMatches = count;
    ត្រឡប់ (count == 0 ? FAIL : OK);
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
    ចំនួនគត់
check_abbr(
    ចំនួនគត់		c,
    char_u	*ptr,
    ចំនួនគត់		col,
    ចំនួនគត់		mincol)
{
    ចំនួនគត់		len;
    ចំនួនគត់		scol;		// starting column of the abbr.
    ចំនួនគត់		j;
    char_u	*s;
    char_u	tb[MB_MAXBYTES + 4];
    mapblock_T	*mp;
    mapblock_T	*mp2;
    ចំនួនគត់		clen = 0;	// length in characters
    ចំនួនគត់		is_id = TRUE;
    ចំនួនគត់		vim_abbr;

    ប្រសិនបើ (typebuf.tb_no_abbr_cnt)	// abbrev. are not recursive
	ត្រឡប់ FALSE;

    // no remapping implies no abbreviation, except for CTRL-]
    ប្រសិនបើ (noremap_keys() && c != Ctrl_RSB)
	ត្រឡប់ FALSE;

    // Check for word before the cursor: If it ends in a keyword char all
    // chars before it must be keyword chars or non-keyword chars, but not
    // white space. If it ends in a non-keyword char we accept any characters
    // before it except white space.
    ប្រសិនបើ (col == 0)				// cannot be an abbr.
	ត្រឡប់ FALSE;

    ប្រសិនបើ (has_mbyte)
    {
	char_u *p;

	p = mb_prevptr(ptr, ptr + col);
	ប្រសិនបើ (!vim_iswordp(p))
	    vim_abbr = TRUE;			// Vim added abbr.
	បើមិនដូច្នេះ
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    ប្រសិនបើ (p > ptr)
		is_id = vim_iswordp(mb_prevptr(ptr, p));
	}
	clen = 1;
	ខណៈពេល (p > ptr + mincol)
	{
	    p = mb_prevptr(ptr, p);
	    ប្រសិនបើ (vim_isspace(*p) || (!vim_abbr && is_id != vim_iswordp(p)))
	    {
		p += (*mb_ptr2len)(p);
		bha_nga;
	    }
	    ++clen;
	}
	scol = (ចំនួនគត់)(p - ptr);
    }
    បើមិនដូច្នេះ
    {
	ប្រសិនបើ (!vim_iswordc(ptr[col - 1]))
	    vim_abbr = TRUE;			// Vim added abbr.
	បើមិនដូច្នេះ
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    ប្រសិនបើ (col > 1)
		is_id = vim_iswordc(ptr[col - 2]);
	}
	សម្រាប់ (scol = col - 1; scol > 0 && !vim_isspace(ptr[scol - 1])
		&& (vim_abbr || is_id == vim_iswordc(ptr[scol - 1])); --scol)
	    ;
    }

    ប្រសិនបើ (scol < mincol)
	scol = mincol;
    ប្រសិនបើ (scol < col)		// there is a word in front of the cursor
    {
	ptr += scol;
	len = col - scol;
	mp = curbuf->b_first_abbr;
	mp2 = first_abbr;
	ប្រសិនបើ (mp == NULL)
	{
	    mp = mp2;
	    mp2 = NULL;
	}
	សម្រាប់ ( ; mp; mp->m_next == NULL
				  ? (mp = mp2, mp2 = NULL) : (mp = mp->m_next))
	{
	    ចំនួនគត់		qlen = mp->m_keylen;
	    char_u	*q = mp->m_keys;
	    ចំនួនគត់		match;

	    ប្រសិនបើ (vim_strbyte(mp->m_keys, K_SPECIAL) != NULL)
	    {
		char_u *qe = vim_strsave(mp->m_keys);

		// might have CSI escaped mp->m_keys
		ប្រសិនបើ (qe != NULL)
		{
		    q = qe;
		    qlen = (ចំនួនគត់)vim_unescape_csi(q);
		}
	    }

	    // find entries with right mode and keys
	    match =    (mp->m_mode & State)
		    && qlen == len
		    && STRNCMP(q, ptr, (size_t)len) == 0;
	    ប្រសិនបើ (q != mp->m_keys)
		vim_free(q);
	    ប្រសិនបើ (match)
		bha_nga;
	}
	ប្រសិនបើ (mp != NULL)
	{
	    ចំនួនគត់	noremap;
	    ចំនួនគត់ silent;
#ifdef FEAT_EVAL
	    ចំនួនគត់ expr;
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
	    ប្រសិនបើ (c != Ctrl_RSB)
	    {
					// special key code, split up
		ប្រសិនបើ (IS_SPECIAL(c) || c == K_SPECIAL)
		{
		    tb[j++] = K_SPECIAL;
		    tb[j++] = K_SECOND(c);
		    tb[j++] = K_THIRD(c);
		}
		បើមិនដូច្នេះ
		{
		    ប្រសិនបើ (c < ABBR_OFF && (c < ' ' || c > '~'))
			tb[j++] = Ctrl_V;	// special char needs CTRL-V
		    ប្រសិនបើ (has_mbyte)
		    {
			ចំនួនគត់	newlen;
			char_u	*escaped;

			// if ABBR_OFF has been added, remove it here
			ប្រសិនបើ (c >= ABBR_OFF)
			    c -= ABBR_OFF;
			newlen = (*mb_char2bytes)(c, tb + j);
			tb[j + newlen] = NUL;
			// Need to escape K_SPECIAL.
			escaped = vim_strsave_escape_csi(tb + j);
			ប្រសិនបើ (escaped != NULL)
			{
			    newlen = (ចំនួនគត់)STRLEN(escaped);
			    mch_memmove(tb + j, escaped, newlen);
			    j += newlen;
			    vim_free(escaped);
			}
		    }
		    បើមិនដូច្នេះ
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

	    ប្រសិនបើ (expr)
		s = eval_map_expr(mp, c);
	    បើមិនដូច្នេះ
#endif
		s = mp->m_str;
	    ប្រសិនបើ (s != NULL)
	    {
					// insert the to string
		(void)ins_typebuf(s, noremap, 0, TRUE, silent);
					// no abbrev. for these chars
		typebuf.tb_no_abbr_cnt += (ចំនួនគត់)STRLEN(s) + j + 1;
#ifdef FEAT_EVAL
		ប្រសិនបើ (expr)
		    vim_free(s);
#endif
	    }

	    tb[0] = Ctrl_H;
	    tb[1] = NUL;
	    ប្រសិនបើ (has_mbyte)
		len = clen;	// Delete characters instead of bytes
	    ខណៈពេល (len-- > 0)		// delete the from string
		(void)ins_typebuf(tb, 1, 0, TRUE, silent);
	    ត្រឡប់ TRUE;
	}
    }
    ត្រឡប់ FALSE;
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
    ចំនួនគត់		c)	    // NUL or typed character for abbreviation
{
    char_u	*res;
    char_u	*p;
    char_u	*expr;
    pos_T	save_cursor;
    ចំនួនគត់		save_msg_col;
    ចំនួនគត់		save_msg_row;
    scid_T	save_sctx_sid = current_sctx.sc_sid;
    ចំនួនគត់		save_sctx_version = current_sctx.sc_version;

    // Remove escaping of CSI, because "str" is in a format to be used as
    // typeahead.
    expr = vim_strsave(mp->m_str);
    ប្រសិនបើ (expr == NULL)
	ត្រឡប់ NULL;
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
    ប្រសិនបើ (mp->m_script_ctx.sc_version == SCRIPT_VERSION_VIM9)
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

    ប្រសិនបើ (p == NULL)
	ត្រឡប់ NULL;
    // Escape CSI in the result to be able to use the string as typeahead.
    res = vim_strsave_escape_csi(p);
    vim_free(p);

    ត្រឡប់ res;
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
    ប្រសិនបើ (res == NULL)
	ត្រឡប់ NULL;

    d = res;
    សម្រាប់ (s = p; *s != NUL; )
    {
	ប្រសិនបើ ((s[0] == K_SPECIAL
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
	បើមិនដូច្នេះ
	{
	    // Add character, possibly multi-byte to destination, escaping
	    // CSI and K_SPECIAL. Be careful, it can be an illegal byte!
	    d = add_char2buf(PTR2CHAR(s), d);
	    s += MB_CPTR2LEN(s);
	}
    }
    *d = NUL;

    ត្រឡប់ res;
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

    ខណៈពេល (*s != NUL)
    {
	ប្រសិនបើ (s[0] == K_SPECIAL && s[1] == KS_SPECIAL && s[2] == KE_FILLER)
	{
	    *d++ = K_SPECIAL;
	    s += 3;
	}
	បើមិនដូច្នេះ ប្រសិនបើ ((s[0] == K_SPECIAL || s[0] == CSI)
				   && s[1] == KS_EXTRA && s[2] == (ចំនួនគត់)KE_CSI)
	{
	    *d++ = CSI;
	    s += 3;
	}
	បើមិនដូច្នេះ
	    *d++ = *s++;
    }
    *d = NUL;
    ត្រឡប់ (size_t)(d - p);
}

/*
 * Write map commands for the current mappings to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    ចំនួនគត់
makemap(
    FILE	*fd,
    buf_T	*buf)	    // buffer for local mappings or NULL
{
    mapblock_T	*mp;
    char_u	c1, c2, c3;
    char_u	*p;
    var_na	*cmd;
    ចំនួនគត់		abbr;
    ចំនួនគត់		hash;
    ចំនួនគត់		did_cpo = FALSE;
    ចំនួនគត់		i;

    validate_maphash();

    // Do the loop twice: Once for mappings, once for abbreviations.
    // Then loop over all map hash lists.
    សម្រាប់ (abbr = 0; abbr < 2; ++abbr)
	សម្រាប់ (hash = 0; hash < 256; ++hash)
	{
	    ប្រសិនបើ (abbr)
	    {
		ប្រសិនបើ (hash > 0)		// there is only one abbr list
		    bha_nga;
		ប្រសិនបើ (buf != NULL)
		    mp = buf->b_first_abbr;
		បើមិនដូច្នេះ
		    mp = first_abbr;
	    }
	    បើមិនដូច្នេះ
	    {
		ប្រសិនបើ (buf != NULL)
		    mp = buf->b_maphash[hash];
		បើមិនដូច្នេះ
		    mp = maphash[hash];
	    }

	    សម្រាប់ ( ; mp; mp = mp->m_next)
	    {
		// simplified map blocks are not created explicitly
		ប្រសិនបើ (mp->m_simplified)
		    continue;

		// skip script-local mappings
		ប្រសិនបើ (mp->m_noremap == REMAP_SCRIPT)
		    continue;

		// skip mappings that contain a <SNR> (script-local thing),
		// they probably don't work when loaded again
		សម្រាប់ (p = mp->m_str; *p != NUL; ++p)
		    ប្រសិនបើ (p[0] == K_SPECIAL && p[1] == KS_EXTRA
						       && p[2] == (ចំនួនគត់)KE_SNR)
			bha_nga;
		ប្រសិនបើ (*p != NUL)
		    continue;

		// It's possible to create a mapping and then ":unmap" certain
		// modes.  We recreate this here by mapping the individual
		// modes, which requires up to three of them.
		c1 = NUL;
		c2 = NUL;
		c3 = NUL;
		ប្រសិនបើ (abbr)
		    cmd = "abbr";
		បើមិនដូច្នេះ
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
			ប្រសិនបើ (!abbr)
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
			ត្រឡប់ FAIL;
		}
		do	// do this twice if c2 is set, 3 times with c3
		{
		    // When outputting <> form, need to make sure that 'cpo'
		    // is set to the Vim default.
		    ប្រសិនបើ (!did_cpo)
		    {
			ប្រសិនបើ (*mp->m_str == NUL)		// will use <Nop>
			    did_cpo = TRUE;
			បើមិនដូច្នេះ
			    សម្រាប់ (i = 0; i < 2; ++i)
				សម្រាប់ (p = (i ? mp->m_str : mp->m_keys); *p; ++p)
				    ប្រសិនបើ (*p == K_SPECIAL || *p == NL)
					did_cpo = TRUE;
			ប្រសិនបើ (did_cpo)
			{
			    ប្រសិនបើ (fprintf(fd, "cpo_save = &cpo") < 0
				    || put_eol(fd) < 0
				    || fprintf(fd, "set cpo&vim") < 0
				    || put_eol(fd) < 0)
				ត្រឡប់ FAIL;
			}
		    }
#ifdef FEAT_EVAL
		    // If it is not vim9 use legacy
		    ប្រសិនបើ (mp->m_expr && mp->m_script_ctx.sc_version < SCRIPT_VERSION_VIM9
			    && fputs("legacy ", fd) < 0)
			ត្រឡប់ FAIL;
#endif
		    ប្រសិនបើ (c1 && putc(c1, fd) < 0)
			ត្រឡប់ FAIL;
		    ប្រសិនបើ (mp->m_noremap != REMAP_YES && fprintf(fd, "nore") < 0)
			ត្រឡប់ FAIL;
		    ប្រសិនបើ (fputs(cmd, fd) < 0)
			ត្រឡប់ FAIL;
		    ប្រសិនបើ (buf != NULL && fputs(" <buffer>", fd) < 0)
			ត្រឡប់ FAIL;
		    ប្រសិនបើ (mp->m_nowait && fputs(" <nowait>", fd) < 0)
			ត្រឡប់ FAIL;
		    ប្រសិនបើ (mp->m_silent && fputs(" <silent>", fd) < 0)
			ត្រឡប់ FAIL;
#ifdef FEAT_EVAL
		    ប្រសិនបើ (mp->m_noremap == REMAP_SCRIPT
						 && fputs("<script>", fd) < 0)
			ត្រឡប់ FAIL;
		    ប្រសិនបើ (mp->m_expr && fputs(" <expr>", fd) < 0)
			ត្រឡប់ FAIL;
#endif

		    ប្រសិនបើ (       putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_keys, 0) == FAIL
			    || putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_str, 1) == FAIL
			    || put_eol(fd) < 0)
			ត្រឡប់ FAIL;
		    c1 = c2;
		    c2 = c3;
		    c3 = NUL;
		} ខណៈពេល (c1 != NUL);
	    }
	}

    ប្រសិនបើ (did_cpo)
	ប្រសិនបើ (fprintf(fd, "&cpo = cpo_save") < 0
		|| put_eol(fd) < 0)
	    ត្រឡប់ FAIL;
    ត្រឡប់ OK;
}

/*
 * write escape string to file
 * "what": 0 for :map lhs, 1 for :map rhs, 2 for :set
 *
 * return FAIL for failure, OK otherwise
 */
    ចំនួនគត់
put_escstr(FILE *fd, char_u *strstart, ចំនួនគត់ what)
{
    char_u	*str = strstart;
    ចំនួនគត់		c;
    ចំនួនគត់		modifiers;

    // :map xx <Nop>
    ប្រសិនបើ (*str == NUL && what == 1)
    {
	ប្រសិនបើ (fprintf(fd, "<Nop>") < 0)
	    ត្រឡប់ FAIL;
	ត្រឡប់ OK;
    }

    សម្រាប់ ( ; *str != NUL; ++str)
    {
	char_u	*p;

	// Check for a multi-byte character, which may contain escaped
	// K_SPECIAL and CSI bytes
	p = mb_unescape(&str);
	ប្រសិនបើ (p != NULL)
	{
	    ខណៈពេល (*p != NUL)
		ប្រសិនបើ (fputc(*p++, fd) < 0)
		    ត្រឡប់ FAIL;
	    --str;
	    continue;
	}

	c = *str;
	// Special key codes have to be translated to be able to make sense
	// when they are read back.
	ប្រសិនបើ (c == K_SPECIAL && what != 2)
	{
	    modifiers = 0;
	    ប្រសិនបើ (str[1] == KS_MODIFIER)
	    {
		modifiers = str[2];
		str += 3;

		// Modifiers can be applied too to multi-byte characters.
		p = mb_unescape(&str);

		ប្រសិនបើ (p == NULL)
		    c = *str;
		បើមិនដូច្នេះ
		{
		    // retrieve codepoint (character number) from unescaped string
		    c = (*mb_ptr2char)(p);
		    --str;
		}
	    }
	    ប្រសិនបើ (c == K_SPECIAL)
	    {
		c = TO_SPECIAL(str[1], str[2]);
		str += 2;
	    }
	    ប្រសិនបើ (IS_SPECIAL(c) || modifiers)	// special key
	    {
		ប្រសិនបើ (fputs((var_na *)get_special_key_name(c, modifiers), fd) < 0)
		    ត្រឡប់ FAIL;
		continue;
	    }
	}

	// A '\n' in a map command should be written as <NL>.
	// A '\n' in a set command should be written as \^V^J.
	ប្រសិនបើ (c == NL)
	{
	    ប្រសិនបើ (what == 2)
	    {
		ប្រសិនបើ (fprintf(fd, "\\\026\n") < 0)
		    ត្រឡប់ FAIL;
	    }
	    បើមិនដូច្នេះ
	    {
		ប្រសិនបើ (fprintf(fd, "<NL>") < 0)
		    ត្រឡប់ FAIL;
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
	ប្រសិនបើ (what == 2 && (VIM_ISWHITE(c) || c == '"' || c == '\\'))
	{
	    ប្រសិនបើ (putc('\\', fd) < 0)
		ត្រឡប់ FAIL;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (c < ' ' || c > '~' || c == '|'
		|| (what == 0 && c == ' ')
		|| (what == 1 && str == strstart && c == ' ')
		|| (what != 2 && c == '<'))
	{
	    ប្រសិនបើ (putc(Ctrl_V, fd) < 0)
		ត្រឡប់ FAIL;
	}
	ប្រសិនបើ (putc(c, fd) < 0)
	    ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
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
    ចំនួនគត់		i;
    char_u	buf[3];
    ចំនួនគត់		abbr;
    ចំនួនគត់		hash;
    buf_T	*bp;
    ESTACK_CHECK_DECLARATION;

    validate_maphash();
    // avoids giving error messages
    estack_push(ETYPE_INTERNAL, (char_u *)"mappings", 0);
    ESTACK_CHECK_SETUP;

    // Do this once for each buffer, and then once for global
    // mappings/abbreviations with bp == NULL
    សម្រាប់ (bp = firstbuf; ; bp = bp->b_next)
    {
	// Do the loop twice: Once for mappings, once for abbreviations.
	// Then loop over all map hash lists.
	សម្រាប់ (abbr = 0; abbr <= 1; ++abbr)
	    សម្រាប់ (hash = 0; hash < 256; ++hash)
	    {
		ប្រសិនបើ (abbr)
		{
		    ប្រសិនបើ (hash)	    // there is only one abbr list
			bha_nga;
		    ប្រសិនបើ (bp != NULL)
			mp = bp->b_first_abbr;
		    បើមិនដូច្នេះ
			mp = first_abbr;
		}
		បើមិនដូច្នេះ
		{
		    ប្រសិនបើ (bp != NULL)
			mp = bp->b_maphash[hash];
		    បើមិនដូច្នេះ
			mp = maphash[hash];
		}
		សម្រាប់ ( ; mp != NULL; mp = mp->m_next)
		{
		    សម្រាប់ (i = 0; i <= 1; ++i)	// do this twice
		    {
			ប្រសិនបើ (i == 0)
			    p = mp->m_keys;	// once for the "from" part
			បើមិនដូច្នេះ
			    p = mp->m_str;	// and once for the "to" part
			ខណៈពេល (*p)
			{
			    ប្រសិនបើ (*p == K_SPECIAL)
			    {
				++p;
				ប្រសិនបើ (*p < 128)   // for "normal" tcap entries
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
	ប្រសិនបើ (bp == NULL)
	    bha_nga;
    }
    ESTACK_CHECK_NOW;
    estack_pop();
}

#ប្រសិនបើ defined(FEAT_EVAL)
/*
 * Check the string "keys" against the lhs of all mappings.
 * Return pointer to rhs of mapping (mapblock->m_str).
 * NULL when no mapping found.
 */
    char_u *
check_map(
    char_u	*keys,
    ចំនួនគត់		mode,
    ចំនួនគត់		exact,		// require exact match
    ចំនួនគត់		ign_mod,	// ignore preceding modifier
    ចំនួនគត់		abbr,		// do abbreviations
    mapblock_T	**mp_ptr,	// return: pointer to mapblock or NULL
    ចំនួនគត់		*local_ptr)	// return: buffer-local mapping or NULL
{
    ចំនួនគត់		hash;
    ចំនួនគត់		len, minlen;
    mapblock_T	*mp;
    char_u	*s;
    ចំនួនគត់		local;

    validate_maphash();

    len = (ចំនួនគត់)STRLEN(keys);
    សម្រាប់ (local = 1; local >= 0; --local)
	// loop over all hash lists
	សម្រាប់ (hash = 0; hash < 256; ++hash)
	{
	    ប្រសិនបើ (abbr)
	    {
		ប្រសិនបើ (hash > 0)		// there is only one list.
		    bha_nga;
		ប្រសិនបើ (local)
		    mp = curbuf->b_first_abbr;
		បើមិនដូច្នេះ
		    mp = first_abbr;
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (local)
		mp = curbuf->b_maphash[hash];
	    បើមិនដូច្នេះ
		mp = maphash[hash];
	    សម្រាប់ ( ; mp != NULL; mp = mp->m_next)
	    {
		// skip entries with wrong mode, wrong length and not matching
		// ones
		ប្រសិនបើ ((mp->m_mode & mode) && (!exact || mp->m_keylen == len))
		{
		    ប្រសិនបើ (len > mp->m_keylen)
			minlen = mp->m_keylen;
		    បើមិនដូច្នេះ
			minlen = len;
		    s = mp->m_keys;
		    ប្រសិនបើ (ign_mod && s[0] == K_SPECIAL && s[1] == KS_MODIFIER
							       && s[2] != NUL)
		    {
			s += 3;
			ប្រសិនបើ (len > mp->m_keylen - 3)
			    minlen = mp->m_keylen - 3;
		    }
		    ប្រសិនបើ (STRNCMP(s, keys, minlen) == 0)
		    {
			ប្រសិនបើ (mp_ptr != NULL)
			    *mp_ptr = mp;
			ប្រសិនបើ (local_ptr != NULL)
			    *local_ptr = local;
			ត្រឡប់ mp->m_str;
		    }
		}
	    }
	}

    ត្រឡប់ NULL;
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
    ចំនួនគត់		abbr = FALSE;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ត្រឡប់;

    name = tv_get_string(&argvars[0]);
    ប្រសិនបើ (argvars[1].v_type == VAR_UNKNOWN)
	mode = (char_u *)"nvo";
    បើមិនដូច្នេះ
    {
	mode = tv_get_string_buf(&argvars[1], buf);
	ប្រសិនបើ (argvars[2].v_type != VAR_UNKNOWN)
	    abbr = (ចំនួនគត់)tv_get_bool(&argvars[2]);
    }

    ប្រសិនបើ (map_to_exists(name, mode, abbr))
	rettv->vval.v_number = TRUE;
    បើមិនដូច្នេះ
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
	ចំនួនគត់	    buffer_local,   // false if not buffer local mapping
	ចំនួនគត់	    abbr)	    // true if abbreviation
{
    char_u	    *lhs = str2special_save(mp->m_keys, TRUE, FALSE);
    char_u	    *mapmode = map_mode_to_chars(mp->m_mode);

    dict_add_string(dict, "lhs", lhs);
    vim_free(lhs);
    dict_add_string(dict, "lhsraw", mp->m_keys);
    ប្រសិនបើ (lhsrawalt)
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
get_maparg(typval_T *argvars, typval_T *rettv, ចំនួនគត់ exact)
{
    char_u	*keys;
    char_u	*keys_simplified;
    char_u	*which;
    char_u	buf[NUMBUFLEN];
    char_u	*keys_buf = NULL;
    char_u	*alt_keys_buf = NULL;
    ចំនួនគត់		did_simplify = FALSE;
    char_u	*rhs;
    ចំនួនគត់		mode;
    ចំនួនគត់		abbr = FALSE;
    ចំនួនគត់		get_dict = FALSE;
    mapblock_T	*mp = NULL;
    ចំនួនគត់		buffer_local;
    ចំនួនគត់		flags = REPTERM_FROM_PART | REPTERM_DO_LT;

    // return empty string for failure
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    keys = tv_get_string(&argvars[0]);
    ប្រសិនបើ (*keys == NUL)
	ត្រឡប់;

    ប្រសិនបើ (argvars[1].v_type != VAR_UNKNOWN)
    {
	which = tv_get_string_buf_chk(&argvars[1], buf);
	ប្រសិនបើ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    abbr = (ចំនួនគត់)tv_get_bool(&argvars[2]);
	    ប្រសិនបើ (argvars[3].v_type != VAR_UNKNOWN)
		get_dict = (ចំនួនគត់)tv_get_bool(&argvars[3]);
	}
    }
    បើមិនដូច្នេះ
	which = (char_u *)"";
    ប្រសិនបើ (which == NULL)
	ត្រឡប់;

    mode = get_map_mode(&which, 0);

    keys_simplified = replace_termcodes(keys, &keys_buf, 0, flags,
								&did_simplify);
    rhs = check_map(keys_simplified, mode, exact, FALSE, abbr,
							   &mp, &buffer_local);
    ប្រសិនបើ (did_simplify)
    {
	// When the lhs is being simplified the not-simplified keys are
	// preferred for printing, like in do_map().
	(void)replace_termcodes(keys, &alt_keys_buf, 0,
					flags | REPTERM_NO_SIMPLIFY, NULL);
	rhs = check_map(alt_keys_buf, mode, exact, FALSE, abbr, &mp,
								&buffer_local);
    }

    ប្រសិនបើ (!get_dict)
    {
	// Return a string.
	ប្រសិនបើ (rhs != NULL)
	{
	    ប្រសិនបើ (*rhs == NUL)
		rettv->vval.v_string = vim_strsave((char_u *)"<Nop>");
	    បើមិនដូច្នេះ
		rettv->vval.v_string = str2special_save(rhs, FALSE, FALSE);
	}

    }
    បើមិនដូច្នេះ ប្រសិនបើ (rettv_dict_alloc(rettv) == OK && rhs != NULL)
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
    ចំនួនគត់		buffer_local;
    char_u	*keys_buf;
    ចំនួនគត់		did_simplify;
    ចំនួនគត់		hash;
    char_u	*lhs;
    const ចំនួនគត់	flags = REPTERM_FROM_PART | REPTERM_DO_LT;
    ចំនួនគត់		abbr = FALSE;

    ប្រសិនបើ (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ត្រឡប់;
    ប្រសិនបើ (argvars[0].v_type != VAR_UNKNOWN)
	abbr = tv_get_bool(&argvars[0]);

    ប្រសិនបើ (rettv_list_alloc(rettv) == FAIL)
	ត្រឡប់;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    សម្រាប់ (buffer_local = 0; buffer_local <= 1; ++buffer_local)
    {
	សម្រាប់ (hash = 0; hash < 256; ++hash)
	{
	    ប្រសិនបើ (abbr)
	    {
		ប្រសិនបើ (hash > 0)		// there is only one abbr list
		    bha_nga;
		ប្រសិនបើ (buffer_local)
		    mp = curbuf->b_first_abbr;
		បើមិនដូច្នេះ
		    mp = first_abbr;
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (buffer_local)
		mp = curbuf->b_maphash[hash];
	    បើមិនដូច្នេះ
		mp = maphash[hash];
	    សម្រាប់ (; mp; mp = mp->m_next)
	    {
		ប្រសិនបើ (mp->m_simplified)
		    continue;
		ប្រសិនបើ ((d = dict_alloc()) == NULL)
		    ត្រឡប់;
		ប្រសិនបើ (list_append_dict(rettv->vval.v_list, d) == FAIL)
		{
		    dict_unref(d);
		    ត្រឡប់;
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
    ប្រសិនបើ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
		ត្រឡប់;

    get_maparg(argvars, rettv, TRUE);
}

/*
 * "mapcheck()" function
 */
    void
f_mapcheck(typval_T *argvars, typval_T *rettv)
{
    ប្រសិនបើ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	ត្រឡប់;

    get_maparg(argvars, rettv, FALSE);
}

/*
 * Get the mapping mode from the mode string.
 * It may contain multiple characters, eg "nox", or "!", or ' '
 * Return 0 if there is an error.
 */
    static ចំនួនគត់
get_map_mode_string(char_u *mode_string, ចំនួនគត់ abbr)
{
    char_u	*p = mode_string;
    ចំនួនគត់		mode = 0;
    ចំនួនគត់		tmode;
    ចំនួនគត់		modec;
    const ចំនួនគត់	MASK_V = MODE_VISUAL | MODE_SELECT;
    const ចំនួនគត់	MASK_MAP = MODE_VISUAL | MODE_SELECT | MODE_NORMAL
							     | MODE_OP_PENDING;
    const ចំនួនគត់	MASK_BANG = MODE_INSERT | MODE_CMDLINE;

    ប្រសិនបើ (*p == NUL)
	p = (char_u *)" ";	// compatibility
    ខណៈពេល ((modec = *p++))
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
		      ត្រឡប់ 0; // error, unknown mode character
	}
	mode |= tmode;
    }
    ប្រសិនបើ ((abbr && (mode & ~MASK_BANG) != 0)
	|| (!abbr && (mode & (mode-1)) != 0 // more than one bit set
	    && (
		// false if multiple bits set in mode and mode is fully
		// contained in one mask
		!(((mode & MASK_BANG) != 0 && (mode & ~MASK_BANG) == 0)
		    || ((mode & MASK_MAP) != 0 && (mode & ~MASK_MAP) == 0)))))
	ត្រឡប់ 0;

    ត្រឡប់ mode;
}

/*
 * "mapset()" function
 */
    void
f_mapset(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*which;
    ចំនួនគត់		mode;
    char_u	buf[NUMBUFLEN];
    ចំនួនគត់		is_abbr;
    dict_T	*d;
    char_u	*lhs;
    char_u	*lhsraw;
    char_u	*lhsrawalt;
    char_u	*rhs;
    char_u	*orig_rhs;
    char_u	*arg_buf = NULL;
    ចំនួនគត់		noremap;
    ចំនួនគត់		expr;
    ចំនួនគត់		silent;
    ចំនួនគត់		buffer;
    scid_T	sid;
    ចំនួនគត់		scriptversion;
    linenr_T	lnum;
    mapblock_T	**map_table = maphash;
    mapblock_T  **abbr_table = &first_abbr;
    ចំនួនគត់		nowait;
    char_u	*arg;
    ចំនួនគត់		dict_only;
    mapblock_T	*mp_result[2] = {NULL, NULL};

    ប្រសិនបើ (check_secure())
	ត្រឡប់;

    // If first arg is a dict, then that's the only arg permitted.
    dict_only = argvars[0].v_type == VAR_DICT;
    ប្រសិនបើ (in_vim9script()
	    && (check_for_string_or_dict_arg(argvars, 0) == FAIL
		|| (dict_only && check_for_unknown_arg(argvars, 1) == FAIL)
		|| (!dict_only
		    && (check_for_string_arg(argvars, 0) == FAIL
			|| check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_dict_arg(argvars, 2) == FAIL))))
	ត្រឡប់;

    ប្រសិនបើ (dict_only)
    {
	d = argvars[0].vval.v_dict;
	which = dict_get_string(d, "mode", FALSE);
	is_abbr = dict_get_bool(d, "abbr", -1);
	ប្រសិនបើ (which == NULL || is_abbr < 0)
	{
	    emsg(_(e_entries_missing_in_mapset_dict_argument));
	    ត្រឡប់;
	}
    }
    បើមិនដូច្នេះ
    {
	which = tv_get_string_buf_chk(&argvars[0], buf);
	ប្រសិនបើ (which == NULL)
	    ត្រឡប់;
	is_abbr = (ចំនួនគត់)tv_get_bool(&argvars[1]);

	ប្រសិនបើ (check_for_dict_arg(argvars, 2) == FAIL)
	    ត្រឡប់;
	d = argvars[2].vval.v_dict;
    }
    mode = get_map_mode_string(which, is_abbr);
    ប្រសិនបើ (mode == 0)
    {
	semsg(_(e_illegal_map_mode_string_str), which);
	ត្រឡប់;
    }


    // Get the values in the same order as above in get_maparg().
    lhs = dict_get_string(d, "lhs", FALSE);
    lhsraw = dict_get_string(d, "lhsraw", FALSE);
    lhsrawalt = dict_get_string(d, "lhsrawalt", FALSE);
    rhs = dict_get_string(d, "rhs", FALSE);
    ប្រសិនបើ (lhs == NULL || lhsraw == NULL || rhs == NULL)
    {
	emsg(_(e_entries_missing_in_mapset_dict_argument));
	ត្រឡប់;
    }
    orig_rhs = rhs;

    noremap = dict_get_number(d, "noremap") ? REMAP_NONE: 0;
    ប្រសិនបើ (dict_get_number(d, "script") != 0)
	noremap = REMAP_SCRIPT;
    expr = dict_get_number(d, "expr") != 0;
    silent = dict_get_number(d, "silent") != 0;
    sid = dict_get_number(d, "sid");
    scriptversion = dict_get_number(d, "scriptversion");
    lnum = dict_get_number(d, "lnum");
    buffer = dict_get_number(d, "buffer");
    nowait = dict_get_number(d, "nowait") != 0;
    // mode from the dict is not used

    ប្រសិនបើ (STRICMP(rhs, "<nop>") == 0)	// "<Nop>" means nothing
	rhs = (char_u *)"";
    បើមិនដូច្នេះ
	rhs = replace_termcodes(rhs, &arg_buf, sid,
					REPTERM_DO_LT | REPTERM_SPECIAL, NULL);

    ប្រសិនបើ (buffer)
    {
	map_table = curbuf->b_maphash;
	abbr_table = &curbuf->b_first_abbr;
    }

    // Delete any existing mapping for this lhs and mode.
    ប្រសិនបើ (buffer)
    {
	arg = alloc(STRLEN(lhs) + STRLEN("<buffer>") + 1);
	ប្រសិនបើ (arg == NULL)
	    ត្រឡប់;
	STRCPY(arg, "<buffer>");
	STRCPY(arg + 8, lhs);
    }
    បើមិនដូច្នេះ
    {
	arg = vim_strsave(lhs);
	ប្រសិនបើ (arg == NULL)
	    ត្រឡប់;
    }
    do_map(MAPTYPE_UNMAP_LHS, arg, mode, is_abbr);
    vim_free(arg);

    mp_result[0] = map_add(map_table, abbr_table, lhsraw, rhs, orig_rhs,
			    noremap, nowait, silent, mode, is_abbr, expr, sid,
			    scriptversion, lnum, 0);
    ប្រសិនបើ (lhsrawalt != NULL)
	mp_result[1] = map_add(map_table, abbr_table, lhsrawalt, rhs, orig_rhs,
			    noremap, nowait, silent, mode, is_abbr, expr, sid,
			    scriptversion, lnum, 1);

    ប្រសិនបើ (mp_result[0] != NULL && mp_result[1] != NULL)
    {
	mp_result[0]->m_alt = mp_result[1];
	mp_result[1]->m_alt = mp_result[0];
    }

    vim_free(arg_buf);
}
#endif


#ប្រសិនបើ defined(MSWIN) || defined(MACOS_X)

# define VIS_SEL	(MODE_VISUAL | MODE_SELECT)	// abbreviation

/*
 * Default mappings for some often used keys.
 */
sa_nracanaa initmap
{
    char_u	*arg;
    ចំនួនគត់		mode;
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

# ប្រសិនបើ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
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
#  បើមិនដូច្នេះ
	{(char_u *)"\316\324 P", MODE_NORMAL},	    // SHIFT-Insert is P
	{(char_u *)"\316\324 \"-dP", VIS_SEL},	    // SHIFT-Insert is "-dP
	{(char_u *)"\316\324 \022\017\"", MODE_INSERT}, // SHIFT-Insert is ^R^O"
	{(char_u *)"\316\325 y", VIS_SEL},	    // CTRL-Insert is y
	{(char_u *)"\316\327 d", VIS_SEL},	    // SHIFT-Del is d
	{(char_u *)"\316\330 d", VIS_SEL},	    // CTRL-Del is d
#  endif
};
# endif

# ប្រសិនបើ defined(MACOS_X)
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
#ប្រសិនបើ defined(MSWIN) || defined(MACOS_X)
    ចំនួនគត់		i;

# ប្រសិនបើ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    ប្រសិនបើ (!gui.starting)
#  endif
    {
	សម្រាប់ (i = 0; i < (ចំនួនគត់)ARRAY_LENGTH(cinitmappings); ++i)
	    add_map(cinitmappings[i].arg, cinitmappings[i].mode, FALSE);
    }
# endif
# ប្រសិនបើ defined(FEAT_GUI_MSWIN) || defined(MACOS_X)
    សម្រាប់ (i = 0; i < (ចំនួនគត់)ARRAY_LENGTH(initmappings); ++i)
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
add_map(char_u *map, ចំនួនគត់ mode, ចំនួនគត់ nore)
{
    char_u	*s;
    char_u	*cpo_save = p_cpo;

    p_cpo = empty_option;	// Allow <> notation
    s = vim_strsave(map);
    ប្រសិនបើ (s != NULL)
    {
	(void)do_map(nore ? MAPTYPE_NOREMAP : MAPTYPE_MAP, s, mode, FALSE);
	vim_free(s);
    }
    p_cpo = cpo_save;
}

#ប្រសិនបើ defined(FEAT_LANGMAP)
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
    ចំនួនគត់	    from;
    ចំនួនគត់     to;
} langmap_entry_T;

static garray_T langmap_mapga;

/*
 * Search for an entry in "langmap_mapga" for "from".  If found set the "to"
 * field.  If not found insert a new entry at the appropriate location.
 */
    static void
langmap_set_entry(ចំនួនគត់ from, ចំនួនគត់ to)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_data);
    ចំនួនគត់		    a = 0;
    ចំនួនគត់		    b = langmap_mapga.ga_len;

    // Do a binary search for an existing entry.
    ខណៈពេល (a != b)
    {
	ចំនួនគត់ i = (a + b) / 2;
	ចំនួនគត់ d = entries[i].from - from;

	ប្រសិនបើ (d == 0)
	{
	    entries[i].to = to;
	    ត្រឡប់;
	}
	ប្រសិនបើ (d < 0)
	    a = i + 1;
	បើមិនដូច្នេះ
	    b = i;
    }

    ប្រសិនបើ (ga_grow(&langmap_mapga, 1) == FAIL)
	ត្រឡប់;  // out of memory

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
    ចំនួនគត់
langmap_adjust_mb(ចំនួនគត់ c)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_data);
    ចំនួនគត់ a = 0;
    ចំនួនគត់ b = langmap_mapga.ga_len;

    ខណៈពេល (a != b)
    {
	ចំនួនគត់ i = (a + b) / 2;
	ចំនួនគត់ d = entries[i].from - c;

	ប្រសិនបើ (d == 0)
	    ត្រឡប់ entries[i].to;  // found matching entry
	ប្រសិនបើ (d < 0)
	    a = i + 1;
	បើមិនដូច្នេះ
	    b = i;
    }
    ត្រឡប់ c;  // no entry found, return "c" unmodified
}

    void
langmap_init(void)
{
    ចំនួនគត់ i;

    សម្រាប់ (i = 0; i < 256; i++)
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
    ចំនួនគត់	    from, to;

    ga_clear(&langmap_mapga);		    // clear the previous map first
    langmap_init();			    // back to one-to-one map

    សម្រាប់ (p = p_langmap; p[0] != NUL; )
    {
	សម្រាប់ (p2 = p; p2[0] != NUL && p2[0] != ',' && p2[0] != ';';
							       MB_PTR_ADV(p2))
	{
	    ប្រសិនបើ (p2[0] == '\\' && p2[1] != NUL)
		++p2;
	}
	ប្រសិនបើ (p2[0] == ';')
	    ++p2;	    // abcd;ABCD form, p2 points to A
	បើមិនដូច្នេះ
	    p2 = NULL;	    // aAbBcCdD form, p2 is NULL
	ខណៈពេល (p[0])
	{
	    ប្រសិនបើ (p[0] == ',')
	    {
		++p;
		bha_nga;
	    }
	    ប្រសិនបើ (p[0] == '\\' && p[1] != NUL)
		++p;
	    from = (*mb_ptr2char)(p);
	    to = NUL;
	    ប្រសិនបើ (p2 == NULL)
	    {
		MB_PTR_ADV(p);
		ប្រសិនបើ (p[0] != ',')
		{
		    ប្រសិនបើ (p[0] == '\\')
			++p;
		    to = (*mb_ptr2char)(p);
		}
	    }
	    បើមិនដូច្នេះ
	    {
		ប្រសិនបើ (p2[0] != ',')
		{
		    ប្រសិនបើ (p2[0] == '\\')
			++p2;
		    to = (*mb_ptr2char)(p2);
		}
	    }
	    ប្រសិនបើ (to == NUL)
	    {
		sprintf(args->os_errbuf,
			_(e_langmap_matching_character_missing_for_str),
			transchar(from));
		ត្រឡប់ args->os_errbuf;
	    }

	    ប្រសិនបើ (from >= 256)
		langmap_set_entry(from, to);
	    បើមិនដូច្នេះ
		langmap_mapchar[from & 255] = to;

	    // Advance to next pair
	    MB_PTR_ADV(p);
	    ប្រសិនបើ (p2 != NULL)
	    {
		MB_PTR_ADV(p2);
		ប្រសិនបើ (*p == ';')
		{
		    p = p2;
		    ប្រសិនបើ (p[0] != NUL)
		    {
			ប្រសិនបើ (p[0] != ',')
			{
			    vim_snprintf(args->os_errbuf, args->os_errbuflen,
				    _(e_langmap_extra_characters_after_semicolon_str),
				    p);
			    ត្រឡប់ args->os_errbuf;
			}
			++p;
		    }
		    bha_nga;
		}
	    }
	}
    }

    ត្រឡប់ NULL;
}
#endif

    static void
do_exmap(exarg_T *eap, ចំនួនគត់ isabbrev)
{
    ចំនួនគត់	    mode;
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
    ប្រសិនបើ (secure)
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
