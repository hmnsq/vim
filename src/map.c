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

#અન્તર્ભાવ "vim.h"

/*
 * List used for abbreviations.
 */
સ્થિર mapblock_T	*first_abbr = ખાલી; // first entry in abbrlist

/*
 * Each mapping is put in one of the 256 hash lists, to speed up finding it.
 */
સ્થિર mapblock_T	*(maphash[256]);
સ્થિર પૂર્ણાંક		maphash_valid = FALSE;

// When non-zero then no mappings can be added or removed.  Prevents mappings
// to change while listing them.
સ્થિર પૂર્ણાંક		map_locked = 0;

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
get_maphash_list(પૂર્ણાંક state, પૂર્ણાંક c)
{
    પાછા maphash[MAP_HASH(state, c)];
}

/*
 * Get the buffer-local hashed map list for "state" and first character "c".
 */
    mapblock_T *
get_buf_maphash_list(પૂર્ણાંક state, પૂર્ણાંક c)
{
    પાછા curbuf->b_maphash[MAP_HASH(state, c)];
}

    પૂર્ણાંક
is_maphash_valid(શૂન્ય)
{
    પાછા maphash_valid;
}

/*
 * Initialize maphash[] for first use.
 */
    સ્થિર શૂન્ય
validate_maphash(શૂન્ય)
{
    જો (maphash_valid)
	પાછા;

    CLEAR_FIELD(maphash);
    maphash_valid = TRUE;
}

/*
 * Delete one entry from the abbrlist or maphash[].
 * "mpp" is a pointer to the m_next field of the PREVIOUS entry!
 */
    સ્થિર શૂન્ય
map_free(mapblock_T **mpp)
{
    mapblock_T	*mp;

    mp = *mpp;
    vim_free(mp->m_ચાવીઓ);
    જો (mp->m_alt != ખાલી)
	mp->m_alt->m_alt = ખાલી;
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
    સ્થિર char_u *
map_mode_to_chars(પૂર્ણાંક mode)
{
    garray_T    mapmode;

    ga_init2(&mapmode, 1, 7);

    જો ((mode & (MODE_ઉમેરો | MODE_CMDLINE)) == (MODE_ઉમેરો | MODE_CMDLINE))
	ga_append(&mapmode, '!');			// :map!
    નહીંતર જો (mode & MODE_ઉમેરો)
	ga_append(&mapmode, 'i');			// :imap
    નહીંતર જો (mode & MODE_LANGMAP)
	ga_append(&mapmode, 'l');			// :lmap
    નહીંતર જો (mode & MODE_CMDLINE)
	ga_append(&mapmode, 'c');			// :cmap
    નહીંતર જો ((mode
		 & (MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING))
		== (MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING))
	ga_append(&mapmode, ' ');			// :map
    નહીંતર
    {
	જો (mode & MODE_NORMAL)
	    ga_append(&mapmode, 'n');			// :nmap
	જો (mode & MODE_OP_PENDING)
	    ga_append(&mapmode, 'o');			// :omap
	જો (mode & MODE_TERMINAL)
	    ga_append(&mapmode, 't');			// :tmap
	જો ((mode & (MODE_VISUAL | MODE_SELECT)) == (MODE_VISUAL | MODE_SELECT))
	    ga_append(&mapmode, 'v');			// :vmap
	નહીંતર
	{
	    જો (mode & MODE_VISUAL)
		ga_append(&mapmode, 'x');		// :xmap
	    જો (mode & MODE_SELECT)
		ga_append(&mapmode, 's');		// :smap
	}
    }

    ga_append(&mapmode, NUL);
    પાછા (char_u *)mapmode.ga_data;
}

/*
 * Output a line for one mapping.
 */
    સ્થિર શૂન્ય
showmap(
    mapblock_T	*mp,
    પૂર્ણાંક		local)	    // TRUE for buffer-local map
{
    પૂર્ણાંક		len = 1;
    char_u	*mapchars;

    જો (message_filtered(mp->m_ચાવીઓ) && message_filtered(mp->m_str))
	પાછા;

    // Prevent mappings to be cleared while at the more prompt.
    // Must jump to "theend" instead of returning.
    ++map_locked;

    જો (સંદેશ_didout || સંદેશ_silent != 0)
    {
	સંદેશ_putchar('\n');
	જો (got_int)	    // 'q' typed at MORE prompt
	    જાઓ theend;
    }

    mapchars = map_mode_to_chars(mp->m_mode);
    જો (mapchars != ખાલી)
    {
	સંદેશ_puts((અક્ષર *)mapchars);
	len = (પૂર્ણાંક)STRLEN(mapchars);
	vim_free(mapchars);
    }

    જ્યારે (++len <= 3)
	સંદેશ_putchar(' ');

    // Display the LHS.  Get length of what we write.
    len = સંદેશ_outtrans_special(mp->m_ચાવીઓ, TRUE, 0);
    કરો
    {
	સંદેશ_putchar(' ');		// pad with blanks
	++len;
    } જ્યારે (len < 12);

    જો (mp->m_noremap == REMAP_NONE)
	સંદેશ_puts_attr("*", HL_ATTR(HLF_8));
    નહીંતર જો (mp->m_noremap == REMAP_SCRIPT)
	સંદેશ_puts_attr("&", HL_ATTR(HLF_8));
    નહીંતર
	સંદેશ_putchar(' ');

    જો (local)
	સંદેશ_putchar('@');
    નહીંતર
	સંદેશ_putchar(' ');

    // Use FALSE below if we only want things like <Up> to show up as such on
    // the rhs, and not M-x etc, TRUE gets both -- webb
    જો (*mp->m_str == NUL)
	સંદેશ_puts_attr("<Nop>", HL_ATTR(HLF_8));
    નહીંતર
	સંદેશ_outtrans_special(mp->m_str, FALSE, 0);
#ifdef FEAT_EVAL
    જો (p_verbose > 0)
	last_set_સંદેશ(mp->m_script_ctx);
#endif
    સંદેશ_clr_eos();
    out_flush();			// show one line at a time

theend:
    --map_locked;
}

    સ્થિર mapblock_T *
map_add(
	mapblock_T  **map_table,
	mapblock_T  **abbr_table,
	char_u	    *ચાવીઓ,
	char_u	    *rhs,
	char_u	    *orig_rhs,
	પૂર્ણાંક	    noremap,
	પૂર્ણાંક	    nowait,
	પૂર્ણાંક	    silent,
	પૂર્ણાંક	    mode,
	પૂર્ણાંક	    is_abbr,
#ifdef FEAT_EVAL
	પૂર્ણાંક	    પદાવલિ,
	scid_T	    sid,	    // 0 to use current_sctx
	પૂર્ણાંક	    scriptversion,
	linenr_T    lnum,
#endif
	પૂર્ણાંક	    simplified)
{
    mapblock_T	*mp = ALLOC_CLEAR_ONE(mapblock_T);

    જો (mp == ખાલી)
	પાછા ખાલી;

    // If CTRL-C has been mapped, don't always use it for Interrupting.
    જો (*ચાવીઓ == Ctrl_C)
    {
	જો (map_table == curbuf->b_maphash)
	    curbuf->b_mapped_ctrl_c |= mode;
	નહીંતર
	    mapped_ctrl_c |= mode;
    }

    mp->m_ચાવીઓ = vim_strsave(ચાવીઓ);
    mp->m_str = vim_strsave(rhs);
    mp->m_orig_str = vim_strsave(orig_rhs);
    જો (mp->m_ચાવીઓ == ખાલી || mp->m_str == ખાલી)
    {
	vim_free(mp->m_ચાવીઓ);
	vim_free(mp->m_str);
	vim_free(mp->m_orig_str);
	vim_free(mp);
	પાછા ખાલી;
    }
    mp->m_keylen = (પૂર્ણાંક)STRLEN(mp->m_ચાવીઓ);
    mp->m_noremap = noremap;
    mp->m_nowait = nowait;
    mp->m_silent = silent;
    mp->m_mode = mode;
    mp->m_simplified = simplified;
#ifdef FEAT_EVAL
    mp->m_પદાવલિ = પદાવલિ;
    જો (sid != 0)
    {
	mp->m_script_ctx.sc_sid = sid;
	mp->m_script_ctx.sc_lnum = lnum;
	mp->m_script_ctx.sc_version = scriptversion;
    }
    નહીંતર
    {
	mp->m_script_ctx = current_sctx;
	જો (cmdmod.cmod_flags & CMOD_LEGACY)
	    mp->m_script_ctx.sc_version = 1;
	mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
    }
#endif

    // add the new entry in front of the abbrlist or maphash[] list
    જો (is_abbr)
    {
	mp->m_next = *abbr_table;
	*abbr_table = mp;
    }
    નહીંતર
    {
	પૂર્ણાંક n = MAP_HASH(mp->m_mode, mp->m_ચાવીઓ[0]);

	mp->m_next = map_table[n];
	map_table[n] = mp;
    }
    પાછા mp;
}

/*
 * List mappings.  When "haskey" is FALSE all mappings, otherwise mappings that
 * match "keys[keys_len]".
 */
    સ્થિર શૂન્ય
list_mappings(
	પૂર્ણાંક	keyround,
	પૂર્ણાંક	abbrev,
	પૂર્ણાંક	haskey,
	char_u	*ચાવીઓ,
	પૂર્ણાંક	ચાવીઓ_len,
	પૂર્ણાંક	mode,
	પૂર્ણાંક	*did_local)
{
    // Prevent mappings to be cleared while at the more prompt.
    ++map_locked;

    જો (p_verbose > 0 && keyround == 1)
    {
	જો (seen_Modify_Other_ચાવીઓ)
	    સંદેશ_puts(_("Seen modifyOtherKeys: true\n"));

	જો (modify_otherkeys_state != MOKS_INITIAL)
	{
	    અક્ષર *name = _("Unknown");
	    પસંદગી (modify_otherkeys_state)
	    {
		સ્થિતિ MOKS_INITIAL: તોડો;
		સ્થિતિ MOKS_OFF: name = _("Off"); તોડો;
		સ્થિતિ MOKS_ENABLED: name = _("On"); તોડો;
		સ્થિતિ MOKS_DISABLED: name = _("Disabled"); તોડો;
		સ્થિતિ MOKS_AFTER_T_TE: name = _("Cleared"); તોડો;
	    }

	    અક્ષર buf[200];
	    vim_snprintf(buf, કદ(buf),
				    _("modifyOtherKeys detected: %s\n"), name);
	    સંદેશ_puts(buf);
	}

	જો (kitty_protocol_state != KKPS_INITIAL)
	{
	    અક્ષર *name = _("Unknown");
	    પસંદગી (kitty_protocol_state)
	    {
		સ્થિતિ KKPS_INITIAL: તોડો;
		સ્થિતિ KKPS_OFF: name = _("Off"); તોડો;
		સ્થિતિ KKPS_ENABLED: name = _("On"); તોડો;
		સ્થિતિ KKPS_DISABLED: name = _("Disabled"); તોડો;
		સ્થિતિ KKPS_AFTER_T_TE: name = _("Cleared"); તોડો;
	    }

	    અક્ષર buf[200];
	    vim_snprintf(buf, કદ(buf),
				     _("Kitty keyboard protocol: %s\n"), name);
	    સંદેશ_puts(buf);
	}
    }

    // need to loop over all global hash lists
    માટે (પૂર્ણાંક hash = 0; hash < 256 && !got_int; ++hash)
    {
	mapblock_T	*mp;

	જો (abbrev)
	{
	    જો (hash != 0)	// there is only one abbreviation list
		તોડો;
	    mp = curbuf->b_first_abbr;
	}
	નહીંતર
	    mp = curbuf->b_maphash[hash];
	માટે ( ; mp != ખાલી && !got_int; mp = mp->m_next)
	{
	    // check entries with the same mode
	    જો (!mp->m_simplified && (mp->m_mode & mode) != 0)
	    {
		જો (!haskey)		    // show all entries
		{
		    showmap(mp, TRUE);
		    *did_local = TRUE;
		}
		નહીંતર
		{
		    પૂર્ણાંક n = mp->m_keylen;
		    જો (STRNCMP(mp->m_ચાવીઓ, ચાવીઓ,
				   (size_t)(n < ચાવીઓ_len ? n : ચાવીઓ_len)) == 0)
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
    પૂર્ણાંક
do_map(
    પૂર્ણાંક		maptype,
    char_u	*arg,
    પૂર્ણાંક		mode,
    પૂર્ણાંક		abbrev)		// not a mapping but an abbreviation
{
    char_u	*ચાવીઓ;
    mapblock_T	*mp, **mpp;
    mapblock_T	*mp_result[2] = {ખાલી, ખાલી};
    char_u	*rhs;
    char_u	*p;
    પૂર્ણાંક		n;
    પૂર્ણાંક		len = 0;	// init for GCC
    પૂર્ણાંક		hasarg;
    પૂર્ણાંક		haskey;
    પૂર્ણાંક		do_print;
    પૂર્ણાંક		keyround;
    char_u	*ચાવીઓ_buf = ખાલી;
    char_u	*alt_ચાવીઓ_buf = ખાલી;
    char_u	*arg_buf = ખાલી;
    પૂર્ણાંક		retval = 0;
    પૂર્ણાંક		do_backslash;
    mapblock_T	**abbr_table;
    mapblock_T	**map_table;
    પૂર્ણાંક		unique = FALSE;
    પૂર્ણાંક		nowait = FALSE;
    પૂર્ણાંક		silent = FALSE;
    પૂર્ણાંક		special = FALSE;
#ifdef FEAT_EVAL
    પૂર્ણાંક		પદાવલિ = FALSE;
#endif
    પૂર્ણાંક		did_simplify = FALSE;
    પૂર્ણાંક		unmap_lhs_only = FALSE;
    પૂર્ણાંક		noremap;
    char_u      *orig_rhs;

    ચાવીઓ = arg;
    map_table = maphash;
    abbr_table = &first_abbr;

    જો (maptype == MAPTYPE_UNMAP_LHS)
    {
	unmap_lhs_only = TRUE;
	maptype = MAPTYPE_UNMAP;
    }

    // For ":noremap" don't remap, otherwise do remap.
    જો (maptype == MAPTYPE_NOREMAP)
	noremap = REMAP_NONE;
    નહીંતર
	noremap = REMAP_YES;

    // Accept <buffer>, <nowait>, <silent>, <expr> <script> and <unique> in
    // any order.
    માટે (;;)
    {
	// Check for "<buffer>": mapping local to buffer.
	જો (STRNCMP(ચાવીઓ, "<buffer>", 8) == 0)
	{
	    ચાવીઓ = skipwhite(ચાવીઓ + 8);
	    map_table = curbuf->b_maphash;
	    abbr_table = &curbuf->b_first_abbr;
	    ચાલુ;
	}

	// Check for "<nowait>": don't wait for more characters.
	જો (STRNCMP(ચાવીઓ, "<nowait>", 8) == 0)
	{
	    ચાવીઓ = skipwhite(ચાવીઓ + 8);
	    nowait = TRUE;
	    ચાલુ;
	}

	// Check for "<silent>": don't echo commands.
	જો (STRNCMP(ચાવીઓ, "<silent>", 8) == 0)
	{
	    ચાવીઓ = skipwhite(ચાવીઓ + 8);
	    silent = TRUE;
	    ચાલુ;
	}

	// Check for "<special>": accept special keys in <>
	જો (STRNCMP(ચાવીઓ, "<special>", 9) == 0)
	{
	    ચાવીઓ = skipwhite(ચાવીઓ + 9);
	    special = TRUE;
	    ચાલુ;
	}

#ifdef FEAT_EVAL
	// Check for "<script>": remap script-local mappings only
	જો (STRNCMP(ચાવીઓ, "<script>", 8) == 0)
	{
	    ચાવીઓ = skipwhite(ચાવીઓ + 8);
	    noremap = REMAP_SCRIPT;
	    ચાલુ;
	}

	// Check for "<expr>": {rhs} is an expression.
	જો (STRNCMP(ચાવીઓ, "<expr>", 6) == 0)
	{
	    ચાવીઓ = skipwhite(ચાવીઓ + 6);
	    પદાવલિ = TRUE;
	    ચાલુ;
	}
#endif
	// Check for "<unique>": don't overwrite an existing mapping.
	જો (STRNCMP(ચાવીઓ, "<unique>", 8) == 0)
	{
	    ચાવીઓ = skipwhite(ચાવીઓ + 8);
	    unique = TRUE;
	    ચાલુ;
	}
	તોડો;
    }

    validate_maphash();

    // Find end of keys and skip CTRL-Vs (and backslashes) in it.
    // Accept backslash like CTRL-V when 'cpoptions' does not contain 'B'.
    // with :unmap white space is included in the keys, no argument possible.
    p = ચાવીઓ;
    do_backslash = (vim_strchr(p_cpo, CPO_BSLASH) == ખાલી);
    જ્યારે (*p && (maptype == MAPTYPE_UNMAP || !VIM_ISWHITE(*p)))
    {
	જો ((p[0] == Ctrl_V || (do_backslash && p[0] == '\\')) &&
								  p[1] != NUL)
	    ++p;		// skip CTRL-V or backslash
	++p;
    }
    જો (*p != NUL)
	*p++ = NUL;

    p = skipwhite(p);
    rhs = p;
    hasarg = (*rhs != NUL);
    haskey = (*ચાવીઓ != NUL);
    do_print = !haskey || (maptype != MAPTYPE_UNMAP && !hasarg);

    // check for :unmap without argument
    જો (maptype == MAPTYPE_UNMAP && !haskey)
    {
	retval = 1;
	જાઓ theend;
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
    જો (haskey)
    {
	char_u	*new_ચાવીઓ;
	પૂર્ણાંક	flags = REPTERM_FROM_PART | REPTERM_DO_LT;

	જો (special)
	    flags |= REPTERM_SPECIAL;
	new_ચાવીઓ = replace_termcodes(ચાવીઓ, &ચાવીઓ_buf, 0, flags, &did_simplify);
	જો (did_simplify)
	    (શૂન્ય)replace_termcodes(ચાવીઓ, &alt_ચાવીઓ_buf, 0,
					    flags | REPTERM_NO_SIMPLIFY, ખાલી);
	ચાવીઓ = new_ચાવીઓ;
    }
    orig_rhs = rhs;
    જો (hasarg)
    {
	જો (STRICMP(rhs, "<nop>") == 0)	    // "<Nop>" means nothing
	    rhs = (char_u *)"";
	નહીંતર
	    rhs = replace_termcodes(rhs, &arg_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), ખાલી);
    }

    /*
     * The following is done twice if we have two versions of keys:
     * "alt_keys_buf" is not NULL.
     */
    માટે (keyround = 1; keyround <= 2; ++keyround)
    {
	પૂર્ણાંક	did_it = FALSE;
	પૂર્ણાંક	did_local = FALSE;
	પૂર્ણાંક	keyround1_simplified = keyround == 1 && did_simplify;
	પૂર્ણાંક	round;
	પૂર્ણાંક	અંક_rounds;

	જો (keyround == 2)
	{
	    જો (alt_ચાવીઓ_buf == ખાલી)
		તોડો;
	    ચાવીઓ = alt_ચાવીઓ_buf;
	}
	નહીંતર જો (alt_ચાવીઓ_buf != ખાલી && do_print)
	    // when printing always use the not-simplified map
	    ચાવીઓ = alt_ચાવીઓ_buf;

	// check arguments and translate function keys
	જો (haskey)
	{
	    len = (પૂર્ણાંક)STRLEN(ચાવીઓ);
	    જો (len > MAXMAPLEN)	// maximum length of MAXMAPLEN chars
	    {
		retval = 1;
		જાઓ theend;
	    }

	    જો (abbrev && maptype != MAPTYPE_UNMAP)
	    {
		// If an abbreviation ends in a keyword character, the
		// rest must be all keyword-char or all non-keyword-char.
		// Otherwise we won't be able to find the start of it in a
		// vi-compatible way.
		જો (has_mbyte)
		{
		    પૂર્ણાંક		first, last;
		    પૂર્ણાંક		same = -1;
		    char_u	ચાવીઓ_unescaped[MAXMAPLEN + 1];
		    size_t	ચાવીઓ_unescaped_len;

		    mch_memmove(ચાવીઓ_unescaped, ચાવીઓ, (size_t)(len + 1));
		    ચાવીઓ_unescaped_len = vim_unescape_csi(ચાવીઓ_unescaped);
		    p = ચાવીઓ_unescaped;

		    first = vim_iswordp(p);
		    last = first;
		    MB_PTR_ADV(p);
		    n = 1;
		    જ્યારે (p < ચાવીઓ_unescaped + ચાવીઓ_unescaped_len)
		    {
			++n;			// nr of (multi-byte) chars
			last = vim_iswordp(p);	// type of last char
			જો (same == -1 && last != first)
			    same = n - 1;	// count of same char type
			MB_PTR_ADV(p);
		    }
		    જો (last && n > 2 && same >= 0 && same < n - 1)
		    {
			retval = 1;
			જાઓ theend;
		    }
		}
		નહીંતર જો (vim_iswordc(ચાવીઓ[len - 1]))
		    // ends in keyword char
		    માટે (n = 0; n < len - 2; ++n)
			જો (vim_iswordc(ચાવીઓ[n]) != vim_iswordc(ચાવીઓ[len - 2]))
			{
			    retval = 1;
			    જાઓ theend;
			}
		// An abbreviation cannot contain white space.
		માટે (n = 0; n < len; ++n)
		    જો (VIM_ISWHITE(ચાવીઓ[n]))
		    {
			retval = 1;
			જાઓ theend;
		    }
	    }
	}

	જો (haskey && hasarg && abbrev)	// if we will add an abbreviation
	    no_abbr = FALSE;		// reset flag that indicates there are
					// no abbreviations

	જો (do_print)
	    સંદેશ_start();

	// Check if a new local mapping wasn't already defined globally.
	જો (unique && map_table == curbuf->b_maphash
			       && haskey && hasarg && maptype != MAPTYPE_UNMAP)
	{
	    // need to loop over all global hash lists
	    માટે (પૂર્ણાંક hash = 0; hash < 256 && !got_int; ++hash)
	    {
		જો (abbrev)
		{
		    જો (hash != 0)	// there is only one abbreviation list
			તોડો;
		    mp = first_abbr;
		}
		નહીંતર
		    mp = maphash[hash];
		માટે ( ; mp != ખાલી && !got_int; mp = mp->m_next)
		{
		    // check entries with the same mode
		    જો ((mp->m_mode & mode) != 0
			    && mp->m_keylen == len
			    && STRNCMP(mp->m_ચાવીઓ, ચાવીઓ, (size_t)len) == 0)
		    {
			જો (abbrev)
			    semsg(
			       _(e_global_abbreviation_already_exists_for_str),
				    mp->m_ચાવીઓ);
			નહીંતર
			    semsg(_(e_global_mapping_already_exists_for_str),
				    mp->m_ચાવીઓ);
			retval = 5;
			જાઓ theend;
		    }
		}
	    }
	}

	// When listing global mappings, also list buffer-local ones here.
	જો (map_table != curbuf->b_maphash && !hasarg
						   && maptype != MAPTYPE_UNMAP)
	    list_mappings(keyround, abbrev, haskey, ચાવીઓ, len,
							     mode, &did_local);

	// Find an entry in the maphash[] list that matches.
	// For :unmap we may loop two times: once to try to unmap an entry with
	// a matching 'from' part, a second time, if the first fails, to unmap
	// an entry with a matching 'to' part. This was done to allow
	// ":ab foo bar" to be unmapped by typing ":unab foo", where "foo" will
	// be replaced by "bar" because of the abbreviation.
	અંક_rounds = maptype == MAPTYPE_UNMAP && !unmap_lhs_only ? 2 : 1;
	માટે (round = 0; round < અંક_rounds && !did_it && !got_int; ++round)
	{
	    // need to loop over all hash lists
	    માટે (પૂર્ણાંક hash = 0; hash < 256 && !got_int; ++hash)
	    {
		જો (abbrev)
		{
		    જો (hash > 0)	// there is only one abbreviation list
			તોડો;
		    mpp = abbr_table;
		}
		નહીંતર
		    mpp = &(map_table[hash]);
		માટે (mp = *mpp; mp != ખાલી && !got_int; mp = *mpp)
		{

		    જો ((mp->m_mode & mode) == 0)
		    {
			// skip entries with wrong mode
			mpp = &(mp->m_next);
			ચાલુ;
		    }
		    જો (!haskey)	// show all entries
		    {
			જો (!mp->m_simplified)
			{
			    showmap(mp, map_table != maphash);
			    did_it = TRUE;
			}
		    }
		    નહીંતર	// do we have a match?
		    {
			જો (round)	// second round: Try unmap "rhs" string
			{
			    n = (પૂર્ણાંક)STRLEN(mp->m_str);
			    p = mp->m_str;
			}
			નહીંતર
			{
			    n = mp->m_keylen;
			    p = mp->m_ચાવીઓ;
			}
			જો (STRNCMP(p, ચાવીઓ, (size_t)(n < len ? n : len)) == 0)
			{
			    જો (maptype == MAPTYPE_UNMAP)
			    {
				// Delete entry.
				// Only accept a full match.  For abbreviations
				// we ignore trailing space when matching with
				// the "lhs", since an abbreviation can't have
				// trailing space.
				જો (n != len && (!abbrev || round || n > len
					       || *skipwhite(ચાવીઓ + n) != NUL))
				{
				    mpp = &(mp->m_next);
				    ચાલુ;
				}
				// In keyround for simplified keys, don't unmap
				// a mapping without m_simplified flag.
				જો (keyround1_simplified && !mp->m_simplified)
				    તોડો;
				// We reset the indicated mode bits. If nothing
				// is left the entry is deleted below.
				mp->m_mode &= ~mode;
				did_it = TRUE;	// remember we did something
			    }
			    નહીંતર જો (!hasarg)	// show matching entry
			    {
				જો (!mp->m_simplified)
				{
				    showmap(mp, map_table != maphash);
				    did_it = TRUE;
				}
			    }
			    નહીંતર જો (n != len)	// new entry is ambiguous
			    {
				mpp = &(mp->m_next);
				ચાલુ;
			    }
			    નહીંતર જો (unique)
			    {
				જો (abbrev)
				    semsg(
				      _(e_abbreviation_already_exists_for_str),
					    p);
				નહીંતર
				    semsg(_(e_mapping_already_exists_for_str),
					    p);
				retval = 5;
				જાઓ theend;
			    }
			    નહીંતર
			    {
				// new rhs for existing entry
				mp->m_mode &= ~mode;	// remove mode bits
				જો (mp->m_mode == 0 && !did_it) // reuse entry
				{
				    char_u *newstr = vim_strsave(rhs);

				    જો (newstr == ખાલી)
				    {
					retval = 4;		// no mem
					જાઓ theend;
				    }
				    જો (mp->m_alt != ખાલી)
					mp->m_alt = mp->m_alt->m_alt = ખાલી;
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
				    mp->m_પદાવલિ = પદાવલિ;
				    mp->m_script_ctx = current_sctx;
				    જો (cmdmod.cmod_flags & CMOD_LEGACY)
					mp->m_script_ctx.sc_version = 1;
				    mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
#endif
				    mp_result[keyround - 1] = mp;
				    did_it = TRUE;
				}
			    }
			    જો (mp->m_mode == 0)  // entry can be deleted
			    {
				map_free(mpp);
				ચાલુ;	// continue with *mpp
			    }

			    // May need to put this entry into another hash
			    // list.
			    પૂર્ણાંક new_hash = MAP_HASH(mp->m_mode, mp->m_ચાવીઓ[0]);
			    જો (!abbrev && new_hash != hash)
			    {
				*mpp = mp->m_next;
				mp->m_next = map_table[new_hash];
				map_table[new_hash] = mp;

				ચાલુ;	// continue with *mpp
			    }
			}
		    }
		    mpp = &(mp->m_next);
		}
	    }
	}

	જો (maptype == MAPTYPE_UNMAP)
	{
	    // delete entry
	    જો (!did_it)
	    {
		જો (!keyround1_simplified)
		    retval = 2;		// no match
	    }
	    નહીંતર જો (*ચાવીઓ == Ctrl_C)
	    {
		// If CTRL-C has been unmapped, reuse it for Interrupting.
		જો (map_table == curbuf->b_maphash)
		    curbuf->b_mapped_ctrl_c &= ~mode;
		નહીંતર
		    mapped_ctrl_c &= ~mode;
	    }
	    ચાલુ;
	}

	જો (!haskey || !hasarg)
	{
	    // print entries
	    જો (!did_it && !did_local)
	    {
		જો (abbrev)
		    સંદેશ(_("No abbreviation found"));
		નહીંતર
		    સંદેશ(_("No mapping found"));
	    }
	    જાઓ theend;    // listing finished
	}

	જો (did_it)
	    ચાલુ;	// have added the new entry already

	// Get here when adding a new entry to the maphash[] list or abbrlist.
	mp_result[keyround - 1] = map_add(map_table, abbr_table, ચાવીઓ,
		    rhs, orig_rhs, noremap, nowait, silent, mode, abbrev,
#ifdef FEAT_EVAL
		    પદાવલિ, /* sid */ 0, /* scriptversion */ 0, /* lnum */ 0,
#endif
		    keyround1_simplified);
	જો (mp_result[keyround - 1] == ખાલી)
	{
	    retval = 4;	    // no mem
	    જાઓ theend;
	}
    }

    જો (mp_result[0] != ખાલી && mp_result[1] != ખાલી)
    {
	mp_result[0]->m_alt = mp_result[1];
	mp_result[1]->m_alt = mp_result[0];
    }

theend:
    vim_free(ચાવીઓ_buf);
    vim_free(alt_ચાવીઓ_buf);
    vim_free(arg_buf);
    પાછા retval;
}

/*
 * Get the mapping mode from the command name.
 */
    સ્થિર પૂર્ણાંક
get_map_mode(char_u **cmdp, પૂર્ણાંક forceit)
{
    char_u	*p;
    પૂર્ણાંક		modec;
    પૂર્ણાંક		mode;

    p = *cmdp;
    modec = *p++;
    જો (modec == 'i')
	mode = MODE_ઉમેરો;				// :imap
    નહીંતર જો (modec == 'l')
	mode = MODE_LANGMAP;				// :lmap
    નહીંતર જો (modec == 'c')
	mode = MODE_CMDLINE;				// :cmap
    નહીંતર જો (modec == 'n' && *p != 'o')		    // avoid :noremap
	mode = MODE_NORMAL;				// :nmap
    નહીંતર જો (modec == 'v')
	mode = MODE_VISUAL | MODE_SELECT;		// :vmap
    નહીંતર જો (modec == 'x')
	mode = MODE_VISUAL;				// :xmap
    નહીંતર જો (modec == 's')
	mode = MODE_SELECT;			// :smap
    નહીંતર જો (modec == 'o')
	mode = MODE_OP_PENDING;			// :omap
    નહીંતર જો (modec == 't')
	mode = MODE_TERMINAL;			// :tmap
    નહીંતર
    {
	--p;
	જો (forceit)
	    mode = MODE_ઉમેરો | MODE_CMDLINE;		// :map !
	નહીંતર
	    mode = MODE_VISUAL | MODE_SELECT | MODE_NORMAL | MODE_OP_PENDING;
							// :map
    }

    *cmdp = p;
    પાછા mode;
}

/*
 * Clear all mappings (":mapclear") or abbreviations (":abclear").
 * "abbr" should be FALSE for mappings, TRUE for abbreviations.
 */
    સ્થિર શૂન્ય
map_clear(
    char_u	*cmdp,
    char_u	*arg,
    પૂર્ણાંક		forceit,
    પૂર્ણાંક		abbr)
{
    પૂર્ણાંક		mode;
    પૂર્ણાંક		local;

    local = (STRCMP(arg, "<buffer>") == 0);
    જો (!local && *arg != NUL)
    {
	emsg(_(e_invalid_argument));
	પાછા;
    }

    mode = get_map_mode(&cmdp, forceit);
    map_clear_mode(curbuf, mode, local, abbr);
}

/*
 * If "map_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    સ્થિર પૂર્ણાંક
is_map_locked(શૂન્ય)
{
    જો (map_locked > 0)
    {
	emsg(_(e_cannot_change_mappings_while_listing));
	પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Clear all mappings in "mode".
 */
    શૂન્ય
map_clear_mode(
    buf_T	*buf,		// buffer for local mappings
    પૂર્ણાંક		mode,		// mode in which to delete
    પૂર્ણાંક		local,		// TRUE for buffer-local mappings
    પૂર્ણાંક		abbr)		// TRUE for abbreviations
{
    mapblock_T	*mp, **mpp;
    પૂર્ણાંક		hash;
    પૂર્ણાંક		new_hash;

    જો (is_map_locked())
	પાછા;

    validate_maphash();

    માટે (hash = 0; hash < 256; ++hash)
    {
	જો (abbr)
	{
	    જો (hash > 0)	// there is only one abbrlist
		તોડો;
	    જો (local)
		mpp = &buf->b_first_abbr;
	    નહીંતર
		mpp = &first_abbr;
	}
	નહીંતર
	{
	    જો (local)
		mpp = &buf->b_maphash[hash];
	    નહીંતર
		mpp = &maphash[hash];
	}
	જ્યારે (*mpp != ખાલી)
	{
	    mp = *mpp;
	    જો (mp->m_mode & mode)
	    {
		mp->m_mode &= ~mode;
		જો (mp->m_mode == 0) // entry can be deleted
		{
		    map_free(mpp);
		    ચાલુ;
		}
		// May need to put this entry into another hash list.
		new_hash = MAP_HASH(mp->m_mode, mp->m_ચાવીઓ[0]);
		જો (!abbr && new_hash != hash)
		{
		    *mpp = mp->m_next;
		    જો (local)
		    {
			mp->m_next = buf->b_maphash[new_hash];
			buf->b_maphash[new_hash] = mp;
		    }
		    નહીંતર
		    {
			mp->m_next = maphash[new_hash];
			maphash[new_hash] = mp;
		    }
		    ચાલુ;		// continue with *mpp
		}
	    }
	    mpp = &(mp->m_next);
	}
    }
}

#જો defined(FEAT_EVAL)
    પૂર્ણાંક
mode_str2flags(char_u *modechars)
{
    પૂર્ણાંક		mode = 0;

    જો (vim_strchr(modechars, 'n') != ખાલી)
	mode |= MODE_NORMAL;
    જો (vim_strchr(modechars, 'v') != ખાલી)
	mode |= MODE_VISUAL | MODE_SELECT;
    જો (vim_strchr(modechars, 'x') != ખાલી)
	mode |= MODE_VISUAL;
    જો (vim_strchr(modechars, 's') != ખાલી)
	mode |= MODE_SELECT;
    જો (vim_strchr(modechars, 'o') != ખાલી)
	mode |= MODE_OP_PENDING;
    જો (vim_strchr(modechars, 'i') != ખાલી)
	mode |= MODE_ઉમેરો;
    જો (vim_strchr(modechars, 'l') != ખાલી)
	mode |= MODE_LANGMAP;
    જો (vim_strchr(modechars, 'c') != ખાલી)
	mode |= MODE_CMDLINE;

    પાછા mode;
}

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "modechars".
 * Recognize termcap codes in "str".
 * Also checks mappings local to the current buffer.
 */
    પૂર્ણાંક
map_to_exists(char_u *str, char_u *modechars, પૂર્ણાંક abbr)
{
    char_u	*rhs;
    char_u	*buf;
    પૂર્ણાંક		retval;

    rhs = replace_termcodes(str, &buf, 0, REPTERM_DO_LT, ખાલી);

    retval = map_to_exists_mode(rhs, mode_str2flags(modechars), abbr);
    vim_free(buf);

    પાછા retval;
}
#endif

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "mode".
 * Also checks mappings local to the current buffer.
 */
    પૂર્ણાંક
map_to_exists_mode(char_u *rhs, પૂર્ણાંક mode, પૂર્ણાંક abbr)
{
    mapblock_T	*mp;
    પૂર્ણાંક		hash;
    પૂર્ણાંક		exp_buffer = FALSE;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    માટે (;;)
    {
	માટે (hash = 0; hash < 256; ++hash)
	{
	    જો (abbr)
	    {
		જો (hash > 0)		// there is only one abbr list
		    તોડો;
		જો (exp_buffer)
		    mp = curbuf->b_first_abbr;
		નહીંતર
		    mp = first_abbr;
	    }
	    નહીંતર જો (exp_buffer)
		mp = curbuf->b_maphash[hash];
	    નહીંતર
		mp = maphash[hash];
	    માટે (; mp; mp = mp->m_next)
	    {
		જો ((mp->m_mode & mode)
			&& strstr((અક્ષર *)mp->m_str, (અક્ષર *)rhs) != ખાલી)
		    પાછા TRUE;
	    }
	}
	જો (exp_buffer)
	    તોડો;
	exp_buffer = TRUE;
    }

    પાછા FALSE;
}

/*
 * Used below when expanding mapping/abbreviation names.
 */
સ્થિર પૂર્ણાંક	expand_mapmodes = 0;
સ્થિર પૂર્ણાંક	expand_isabbrev = 0;
સ્થિર પૂર્ણાંક	expand_buffer = FALSE;

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
    સ્થિર char_u *
translate_mapping(char_u *str)
{
    garray_T	ga;
    પૂર્ણાંક		c;
    પૂર્ણાંક		modifiers;
    પૂર્ણાંક		cpo_bslash;
    પૂર્ણાંક		cpo_special;

    ga_init(&ga);
    ga.ga_itemsize = 1;
    ga.ga_growsize = 40;

    cpo_bslash = (vim_strchr(p_cpo, CPO_BSLASH) != ખાલી);
    cpo_special = (vim_strchr(p_cpo, CPO_SPECI) != ખાલી);

    માટે (; *str; ++str)
    {
	c = *str;
	જો (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	{
	    modifiers = 0;
	    જો (str[1] == KS_MODIFIER)
	    {
		str++;
		modifiers = *++str;
		c = *++str;
	    }
	    જો (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	    {
		જો (cpo_special)
		{
		    ga_clear(&ga);
		    પાછા ખાલી;
		}
		c = TO_SPECIAL(str[1], str[2]);
		જો (c == K_ZERO)	// display <Nul> as ^@
		    c = NUL;
		str += 2;
	    }
	    જો (IS_SPECIAL(c) || modifiers)	// special key
	    {
		જો (cpo_special)
		{
		    ga_clear(&ga);
		    પાછા ખાલી;
		}
		ga_concat(&ga, get_special_ચાવી_name(c, modifiers));
		ચાલુ; // for (str)
	    }
	}
	જો (c == ' ' || c == '\t' || c == Ctrl_J || c == Ctrl_V
	    || (c == '<' && !cpo_special) || (c == '\\' && !cpo_bslash))
	    ga_append(&ga, cpo_bslash ? Ctrl_V : '\\');
	જો (c)
	    ga_append(&ga, c);
    }
    ga_append(&ga, NUL);
    પાછા (char_u *)(ga.ga_data);
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
    પૂર્ણાંક		forceit,	// TRUE if '!' given
    પૂર્ણાંક		isabbrev,	// TRUE if abbreviation
    પૂર્ણાંક		isunmap,	// TRUE if unmap/unabbrev command
    cmdidx_T	cmdidx)
{
    જો (forceit && cmdidx != CMD_map && cmdidx != CMD_unmap)
	xp->xp_context = EXPAND_NOTHING;
    નહીંતર
    {
	જો (isunmap)
	    expand_mapmodes = get_map_mode(&cmd, forceit || isabbrev);
	નહીંતર
	{
	    expand_mapmodes = MODE_ઉમેરો | MODE_CMDLINE;
	    જો (!isabbrev)
		expand_mapmodes += MODE_VISUAL | MODE_SELECT | MODE_NORMAL
							     | MODE_OP_PENDING;
	}
	expand_isabbrev = isabbrev;
	xp->xp_context = EXPAND_MAPPINGS;
	expand_buffer = FALSE;
	માટે (;;)
	{
	    જો (STRNCMP(arg, "<buffer>", 8) == 0)
	    {
		expand_buffer = TRUE;
		arg = skipwhite(arg + 8);
		ચાલુ;
	    }
	    જો (STRNCMP(arg, "<unique>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		ચાલુ;
	    }
	    જો (STRNCMP(arg, "<nowait>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		ચાલુ;
	    }
	    જો (STRNCMP(arg, "<silent>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		ચાલુ;
	    }
	    જો (STRNCMP(arg, "<special>", 9) == 0)
	    {
		arg = skipwhite(arg + 9);
		ચાલુ;
	    }
#ifdef FEAT_EVAL
	    જો (STRNCMP(arg, "<script>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		ચાલુ;
	    }
	    જો (STRNCMP(arg, "<expr>", 6) == 0)
	    {
		arg = skipwhite(arg + 6);
		ચાલુ;
	    }
#endif
	    તોડો;
	}
	xp->xp_pattern = arg;
    }

    પાછા ખાલી;
}

/*
 * Find all mapping/abbreviation names that match regexp "regmatch"'.
 * For command line expansion of ":[un]map" and ":[un]abbrev" in all modes.
 * Return OK if matches found, FAIL otherwise.
 */
    પૂર્ણાંક
ExpandMappings(
    char_u	*pat,
    regmatch_T	*regmatch,
    પૂર્ણાંક		*અંક_Matches,
    char_u	***matches)
{
    mapblock_T	*mp;
    garray_T	ga;
    પૂર્ણાંક		hash;
    પૂર્ણાંક		ગણતરી;
    char_u	*p;
    પૂર્ણાંક		i;
    પૂર્ણાંક		fuzzy;
    પૂર્ણાંક		match;
    પૂર્ણાંક		score = 0;
    fuzmatch_str_T  *fuzmatch;

    fuzzy = cmdline_fuzzy_complete(pat);

    validate_maphash();

    *અંક_Matches = 0;		    // return values in case of FAIL
    *matches = ખાલી;

    જો (!fuzzy)
	ga_init2(&ga, કદ(અક્ષર *), 3);
    નહીંતર
	ga_init2(&ga, કદ(fuzmatch_str_T), 3);

    // First search in map modifier arguments
    માટે (i = 0; i < 7; ++i)
    {
	જો (i == 0)
	    p = (char_u *)"<silent>";
	નહીંતર જો (i == 1)
	    p = (char_u *)"<unique>";
#ifdef FEAT_EVAL
	નહીંતર જો (i == 2)
	    p = (char_u *)"<script>";
	નહીંતર જો (i == 3)
	    p = (char_u *)"<expr>";
#endif
	નહીંતર જો (i == 4 && !expand_buffer)
	    p = (char_u *)"<buffer>";
	નહીંતર જો (i == 5)
	    p = (char_u *)"<nowait>";
	નહીંતર જો (i == 6)
	    p = (char_u *)"<special>";
	નહીંતર
	    ચાલુ;

	જો (!fuzzy)
	    match = vim_regexec(regmatch, p, (colnr_T)0);
	નહીંતર
	{
	    score = fuzzy_match_str(p, pat);
	    match = (score != FUZZY_SCORE_NONE);
	}

	જો (!match)
	    ચાલુ;

	જો (ga_grow(&ga, 1) == FAIL)
	    તોડો;

	જો (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = vim_strsave(p);
	    fuzmatch->score = score;
	}
	નહીંતર
	    ((char_u **)ga.ga_data)[ga.ga_len] = vim_strsave(p);
	++ga.ga_len;
    }

    માટે (hash = 0; hash < 256; ++hash)
    {
	જો (expand_isabbrev)
	{
	    જો (hash > 0)	// only one abbrev list
		તોડો; // for (hash)
	    mp = first_abbr;
	}
	નહીંતર જો (expand_buffer)
	    mp = curbuf->b_maphash[hash];
	નહીંતર
	    mp = maphash[hash];
	માટે (; mp; mp = mp->m_next)
	{
	    જો (mp->m_simplified || !(mp->m_mode & expand_mapmodes))
		ચાલુ;

	    p = translate_mapping(mp->m_ચાવીઓ);
	    જો (p == ખાલી)
		ચાલુ;

	    જો (!fuzzy)
		match = vim_regexec(regmatch, p, (colnr_T)0);
	    નહીંતર
	    {
		score = fuzzy_match_str(p, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }

	    જો (!match)
	    {
		vim_free(p);
		ચાલુ;
	    }

	    જો (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		તોડો;
	    }

	    જો (fuzzy)
	    {
		fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    નહીંતર
		((char_u **)ga.ga_data)[ga.ga_len] = p;

	    ++ga.ga_len;
	} // for (mp)
    } // for (hash)

    જો (ga.ga_len == 0)
	પાછા FAIL;

    જો (!fuzzy)
    {
	*matches = ga.ga_data;
	*અંક_Matches = ga.ga_len;
    }
    નહીંતર
    {
	જો (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
							FALSE) == FAIL)
	    પાછા FAIL;
	*અંક_Matches = ga.ga_len;
    }

    ગણતરી = *અંક_Matches;
    જો (ગણતરી > 1)
    {
	char_u	**ptr1;
	char_u	**ptr2;
	char_u	**ptr3;

	// Sort the matches
	// Fuzzy matching already sorts the matches
	જો (!fuzzy)
	    sort_strings(*matches, ગણતરી);

	// Remove multiple entries
	ptr1 = *matches;
	ptr2 = ptr1 + 1;
	ptr3 = ptr1 + ગણતરી;

	જ્યારે (ptr2 < ptr3)
	{
	    જો (STRCMP(*ptr1, *ptr2) != 0)
		*++ptr1 = *ptr2++;
	    નહીંતર
	    {
		vim_free(*ptr2++);
		ગણતરી--;
	    }
	}
    }

    *અંક_Matches = ગણતરી;
    પાછા (ગણતરી == 0 ? FAIL : OK);
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
    પૂર્ણાંક
check_abbr(
    પૂર્ણાંક		c,
    char_u	*ptr,
    પૂર્ણાંક		col,
    પૂર્ણાંક		mincol)
{
    પૂર્ણાંક		len;
    પૂર્ણાંક		scol;		// starting column of the abbr.
    પૂર્ણાંક		j;
    char_u	*s;
    char_u	tb[MB_MAXBYTES + 4];
    mapblock_T	*mp;
    mapblock_T	*mp2;
    પૂર્ણાંક		clen = 0;	// length in characters
    પૂર્ણાંક		is_id = TRUE;
    પૂર્ણાંક		vim_abbr;

    જો (typebuf.tb_no_abbr_cnt)	// abbrev. are not recursive
	પાછા FALSE;

    // no remapping implies no abbreviation, except for CTRL-]
    જો (noremap_ચાવીઓ() && c != Ctrl_RSB)
	પાછા FALSE;

    // Check for word before the cursor: If it ends in a keyword char all
    // chars before it must be keyword chars or non-keyword chars, but not
    // white space. If it ends in a non-keyword char we accept any characters
    // before it except white space.
    જો (col == 0)				// cannot be an abbr.
	પાછા FALSE;

    જો (has_mbyte)
    {
	char_u *p;

	p = mb_prevptr(ptr, ptr + col);
	જો (!vim_iswordp(p))
	    vim_abbr = TRUE;			// Vim added abbr.
	નહીંતર
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    જો (p > ptr)
		is_id = vim_iswordp(mb_prevptr(ptr, p));
	}
	clen = 1;
	જ્યારે (p > ptr + mincol)
	{
	    p = mb_prevptr(ptr, p);
	    જો (vim_isspace(*p) || (!vim_abbr && is_id != vim_iswordp(p)))
	    {
		p += (*mb_ptr2len)(p);
		તોડો;
	    }
	    ++clen;
	}
	scol = (પૂર્ણાંક)(p - ptr);
    }
    નહીંતર
    {
	જો (!vim_iswordc(ptr[col - 1]))
	    vim_abbr = TRUE;			// Vim added abbr.
	નહીંતર
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    જો (col > 1)
		is_id = vim_iswordc(ptr[col - 2]);
	}
	માટે (scol = col - 1; scol > 0 && !vim_isspace(ptr[scol - 1])
		&& (vim_abbr || is_id == vim_iswordc(ptr[scol - 1])); --scol)
	    ;
    }

    જો (scol < mincol)
	scol = mincol;
    જો (scol < col)		// there is a word in front of the cursor
    {
	ptr += scol;
	len = col - scol;
	mp = curbuf->b_first_abbr;
	mp2 = first_abbr;
	જો (mp == ખાલી)
	{
	    mp = mp2;
	    mp2 = ખાલી;
	}
	માટે ( ; mp; mp->m_next == ખાલી
				  ? (mp = mp2, mp2 = ખાલી) : (mp = mp->m_next))
	{
	    પૂર્ણાંક		qlen = mp->m_keylen;
	    char_u	*q = mp->m_ચાવીઓ;
	    પૂર્ણાંક		match;

	    જો (vim_strbyte(mp->m_ચાવીઓ, K_SPECIAL) != ખાલી)
	    {
		char_u *qe = vim_strsave(mp->m_ચાવીઓ);

		// might have CSI escaped mp->m_keys
		જો (qe != ખાલી)
		{
		    q = qe;
		    qlen = (પૂર્ણાંક)vim_unescape_csi(q);
		}
	    }

	    // find entries with right mode and keys
	    match =    (mp->m_mode & State)
		    && qlen == len
		    && STRNCMP(q, ptr, (size_t)len) == 0;
	    જો (q != mp->m_ચાવીઓ)
		vim_free(q);
	    જો (match)
		તોડો;
	}
	જો (mp != ખાલી)
	{
	    પૂર્ણાંક	noremap;
	    પૂર્ણાંક silent;
#ifdef FEAT_EVAL
	    પૂર્ણાંક પદાવલિ;
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
	    જો (c != Ctrl_RSB)
	    {
					// special key code, split up
		જો (IS_SPECIAL(c) || c == K_SPECIAL)
		{
		    tb[j++] = K_SPECIAL;
		    tb[j++] = K_SECOND(c);
		    tb[j++] = K_THIRD(c);
		}
		નહીંતર
		{
		    જો (c < ABBR_OFF && (c < ' ' || c > '~'))
			tb[j++] = Ctrl_V;	// special char needs CTRL-V
		    જો (has_mbyte)
		    {
			પૂર્ણાંક	newlen;
			char_u	*escaped;

			// if ABBR_OFF has been added, remove it here
			જો (c >= ABBR_OFF)
			    c -= ABBR_OFF;
			newlen = (*mb_char2bytes)(c, tb + j);
			tb[j + newlen] = NUL;
			// Need to escape K_SPECIAL.
			escaped = vim_strsave_escape_csi(tb + j);
			જો (escaped != ખાલી)
			{
			    newlen = (પૂર્ણાંક)STRLEN(escaped);
			    mch_memmove(tb + j, escaped, newlen);
			    j += newlen;
			    vim_free(escaped);
			}
		    }
		    નહીંતર
			tb[j++] = c;
		}
		tb[j] = NUL;
					// insert the last typed char
		(શૂન્ય)ins_typebuf(tb, 1, 0, TRUE, mp->m_silent);
	    }

	    // copy values here, calling eval_map_expr() may make "mp" invalid!
	    noremap = mp->m_noremap;
	    silent = mp->m_silent;
#ifdef FEAT_EVAL
	    પદાવલિ = mp->m_પદાવલિ;

	    જો (પદાવલિ)
		s = eval_map_પદાવલિ(mp, c);
	    નહીંતર
#endif
		s = mp->m_str;
	    જો (s != ખાલી)
	    {
					// insert the to string
		(શૂન્ય)ins_typebuf(s, noremap, 0, TRUE, silent);
					// no abbrev. for these chars
		typebuf.tb_no_abbr_cnt += (પૂર્ણાંક)STRLEN(s) + j + 1;
#ifdef FEAT_EVAL
		જો (પદાવલિ)
		    vim_free(s);
#endif
	    }

	    tb[0] = Ctrl_H;
	    tb[1] = NUL;
	    જો (has_mbyte)
		len = clen;	// Delete characters instead of bytes
	    જ્યારે (len-- > 0)		// delete the from string
		(શૂન્ય)ins_typebuf(tb, 1, 0, TRUE, silent);
	    પાછા TRUE;
	}
    }
    પાછા FALSE;
}

#ifdef FEAT_EVAL
/*
 * Evaluate the RHS of a mapping or abbreviations and take care of escaping
 * special characters.
 * Careful: after this "mp" will be invalid if the mapping was deleted.
 */
    char_u *
eval_map_પદાવલિ(
    mapblock_T	*mp,
    પૂર્ણાંક		c)	    // NUL or typed character for abbreviation
{
    char_u	*res;
    char_u	*p;
    char_u	*પદાવલિ;
    pos_T	save_cursor;
    પૂર્ણાંક		save_સંદેશ_col;
    પૂર્ણાંક		save_સંદેશ_row;
    scid_T	save_sctx_sid = current_sctx.sc_sid;
    પૂર્ણાંક		save_sctx_version = current_sctx.sc_version;

    // Remove escaping of CSI, because "str" is in a format to be used as
    // typeahead.
    પદાવલિ = vim_strsave(mp->m_str);
    જો (પદાવલિ == ખાલી)
	પાછા ખાલી;
    vim_unescape_csi(પદાવલિ);

    // Forbid changing text or using ":normal" to avoid most of the bad side
    // effects.  Also restore the cursor position.
    ++textlock;
    ++ex_normal_lock;
    set_vim_var_char(c);  // set v:char to the typed character
    save_cursor = curwin->w_cursor;
    save_સંદેશ_col = સંદેશ_col;
    save_સંદેશ_row = સંદેશ_row;

    current_sctx.sc_version = mp->m_script_ctx.sc_version;
    જો (mp->m_script_ctx.sc_version == SCRIPT_VERSION_VIM9)
	current_sctx.sc_sid = mp->m_script_ctx.sc_sid;

    // Note: the evaluation may make "mp" invalid.
    p = eval_to_string(પદાવલિ, FALSE, FALSE);

    --textlock;
    --ex_normal_lock;
    curwin->w_cursor = save_cursor;
    સંદેશ_col = save_સંદેશ_col;
    સંદેશ_row = save_સંદેશ_row;
    current_sctx.sc_sid = save_sctx_sid;
    current_sctx.sc_version = save_sctx_version;

    vim_free(પદાવલિ);

    જો (p == ખાલી)
	પાછા ખાલી;
    // Escape CSI in the result to be able to use the string as typeahead.
    res = vim_strsave_escape_csi(p);
    vim_free(p);

    પાછા res;
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
    જો (res == ખાલી)
	પાછા ખાલી;

    d = res;
    માટે (s = p; *s != NUL; )
    {
	જો ((s[0] == K_SPECIAL
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
	નહીંતર
	{
	    // Add character, possibly multi-byte to destination, escaping
	    // CSI and K_SPECIAL. Be careful, it can be an illegal byte!
	    d = add_char2buf(PTR2CHAR(s), d);
	    s += MB_CPTR2LEN(s);
	}
    }
    *d = NUL;

    પાછા res;
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

    જ્યારે (*s != NUL)
    {
	જો (s[0] == K_SPECIAL && s[1] == KS_SPECIAL && s[2] == KE_FILLER)
	{
	    *d++ = K_SPECIAL;
	    s += 3;
	}
	નહીંતર જો ((s[0] == K_SPECIAL || s[0] == CSI)
				   && s[1] == KS_EXTRA && s[2] == (પૂર્ણાંક)KE_CSI)
	{
	    *d++ = CSI;
	    s += 3;
	}
	નહીંતર
	    *d++ = *s++;
    }
    *d = NUL;
    પાછા (size_t)(d - p);
}

/*
 * Write map commands for the current mappings to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    પૂર્ણાંક
makemap(
    FILE	*fd,
    buf_T	*buf)	    // buffer for local mappings or NULL
{
    mapblock_T	*mp;
    char_u	c1, c2, c3;
    char_u	*p;
    અક્ષર	*cmd;
    પૂર્ણાંક		abbr;
    પૂર્ણાંક		hash;
    પૂર્ણાંક		did_cpo = FALSE;
    પૂર્ણાંક		i;

    validate_maphash();

    // Do the loop twice: Once for mappings, once for abbreviations.
    // Then loop over all map hash lists.
    માટે (abbr = 0; abbr < 2; ++abbr)
	માટે (hash = 0; hash < 256; ++hash)
	{
	    જો (abbr)
	    {
		જો (hash > 0)		// there is only one abbr list
		    તોડો;
		જો (buf != ખાલી)
		    mp = buf->b_first_abbr;
		નહીંતર
		    mp = first_abbr;
	    }
	    નહીંતર
	    {
		જો (buf != ખાલી)
		    mp = buf->b_maphash[hash];
		નહીંતર
		    mp = maphash[hash];
	    }

	    માટે ( ; mp; mp = mp->m_next)
	    {
		// simplified map blocks are not created explicitly
		જો (mp->m_simplified)
		    ચાલુ;

		// skip script-local mappings
		જો (mp->m_noremap == REMAP_SCRIPT)
		    ચાલુ;

		// skip mappings that contain a <SNR> (script-local thing),
		// they probably don't work when loaded again
		માટે (p = mp->m_str; *p != NUL; ++p)
		    જો (p[0] == K_SPECIAL && p[1] == KS_EXTRA
						       && p[2] == (પૂર્ણાંક)KE_SNR)
			તોડો;
		જો (*p != NUL)
		    ચાલુ;

		// It's possible to create a mapping and then ":unmap" certain
		// modes.  We recreate this here by mapping the individual
		// modes, which requires up to three of them.
		c1 = NUL;
		c2 = NUL;
		c3 = NUL;
		જો (abbr)
		    cmd = "abbr";
		નહીંતર
		    cmd = "map";
		પસંદગી (mp->m_mode)
		{
		    સ્થિતિ MODE_NORMAL | MODE_VISUAL | MODE_SELECT
							     | MODE_OP_PENDING:
			તોડો;
		    સ્થિતિ MODE_NORMAL:
			c1 = 'n';
			તોડો;
		    સ્થિતિ MODE_VISUAL:
			c1 = 'x';
			તોડો;
		    સ્થિતિ MODE_SELECT:
			c1 = 's';
			તોડો;
		    સ્થિતિ MODE_OP_PENDING:
			c1 = 'o';
			તોડો;
		    સ્થિતિ MODE_NORMAL | MODE_VISUAL:
			c1 = 'n';
			c2 = 'x';
			તોડો;
		    સ્થિતિ MODE_NORMAL | MODE_SELECT:
			c1 = 'n';
			c2 = 's';
			તોડો;
		    સ્થિતિ MODE_NORMAL | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 'o';
			તોડો;
		    સ્થિતિ MODE_VISUAL | MODE_SELECT:
			c1 = 'v';
			તોડો;
		    સ્થિતિ MODE_VISUAL | MODE_OP_PENDING:
			c1 = 'x';
			c2 = 'o';
			તોડો;
		    સ્થિતિ MODE_SELECT | MODE_OP_PENDING:
			c1 = 's';
			c2 = 'o';
			તોડો;
		    સ્થિતિ MODE_NORMAL | MODE_VISUAL | MODE_SELECT:
			c1 = 'n';
			c2 = 'v';
			તોડો;
		    સ્થિતિ MODE_NORMAL | MODE_VISUAL | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 'x';
			c3 = 'o';
			તોડો;
		    સ્થિતિ MODE_NORMAL | MODE_SELECT | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 's';
			c3 = 'o';
			તોડો;
		    સ્થિતિ MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING:
			c1 = 'v';
			c2 = 'o';
			તોડો;
		    સ્થિતિ MODE_CMDLINE | MODE_ઉમેરો:
			જો (!abbr)
			    cmd = "map!";
			તોડો;
		    સ્થિતિ MODE_CMDLINE:
			c1 = 'c';
			તોડો;
		    સ્થિતિ MODE_ઉમેરો:
			c1 = 'i';
			તોડો;
		    સ્થિતિ MODE_LANGMAP:
			c1 = 'l';
			તોડો;
		    સ્થિતિ MODE_TERMINAL:
			c1 = 't';
			તોડો;
		    મૂળભૂત:
			iemsg(e_makemap_illegal_mode);
			પાછા FAIL;
		}
		કરો	// do this twice if c2 is set, 3 times with c3
		{
		    // When outputting <> form, need to make sure that 'cpo'
		    // is set to the Vim default.
		    જો (!did_cpo)
		    {
			જો (*mp->m_str == NUL)		// will use <Nop>
			    did_cpo = TRUE;
			નહીંતર
			    માટે (i = 0; i < 2; ++i)
				માટે (p = (i ? mp->m_str : mp->m_ચાવીઓ); *p; ++p)
				    જો (*p == K_SPECIAL || *p == NL)
					did_cpo = TRUE;
			જો (did_cpo)
			{
			    જો (fprintf(fd, "cpo_save = &cpo") < 0
				    || put_eol(fd) < 0
				    || fprintf(fd, "set cpo&vim") < 0
				    || put_eol(fd) < 0)
				પાછા FAIL;
			}
		    }
#ifdef FEAT_EVAL
		    // If it is not vim9 use legacy
		    જો (mp->m_પદાવલિ && mp->m_script_ctx.sc_version < SCRIPT_VERSION_VIM9
			    && fputs("legacy ", fd) < 0)
			પાછા FAIL;
#endif
		    જો (c1 && putc(c1, fd) < 0)
			પાછા FAIL;
		    જો (mp->m_noremap != REMAP_YES && fprintf(fd, "nore") < 0)
			પાછા FAIL;
		    જો (fputs(cmd, fd) < 0)
			પાછા FAIL;
		    જો (buf != ખાલી && fputs(" <buffer>", fd) < 0)
			પાછા FAIL;
		    જો (mp->m_nowait && fputs(" <nowait>", fd) < 0)
			પાછા FAIL;
		    જો (mp->m_silent && fputs(" <silent>", fd) < 0)
			પાછા FAIL;
#ifdef FEAT_EVAL
		    જો (mp->m_noremap == REMAP_SCRIPT
						 && fputs("<script>", fd) < 0)
			પાછા FAIL;
		    જો (mp->m_પદાવલિ && fputs(" <expr>", fd) < 0)
			પાછા FAIL;
#endif

		    જો (       putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_ચાવીઓ, 0) == FAIL
			    || putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_str, 1) == FAIL
			    || put_eol(fd) < 0)
			પાછા FAIL;
		    c1 = c2;
		    c2 = c3;
		    c3 = NUL;
		} જ્યારે (c1 != NUL);
	    }
	}

    જો (did_cpo)
	જો (fprintf(fd, "&cpo = cpo_save") < 0
		|| put_eol(fd) < 0)
	    પાછા FAIL;
    પાછા OK;
}

/*
 * write escape string to file
 * "what": 0 for :map lhs, 1 for :map rhs, 2 for :set
 *
 * return FAIL for failure, OK otherwise
 */
    પૂર્ણાંક
put_escstr(FILE *fd, char_u *strstart, પૂર્ણાંક what)
{
    char_u	*str = strstart;
    પૂર્ણાંક		c;
    પૂર્ણાંક		modifiers;

    // :map xx <Nop>
    જો (*str == NUL && what == 1)
    {
	જો (fprintf(fd, "<Nop>") < 0)
	    પાછા FAIL;
	પાછા OK;
    }

    માટે ( ; *str != NUL; ++str)
    {
	char_u	*p;

	// Check for a multi-byte character, which may contain escaped
	// K_SPECIAL and CSI bytes
	p = mb_unescape(&str);
	જો (p != ખાલી)
	{
	    જ્યારે (*p != NUL)
		જો (fputc(*p++, fd) < 0)
		    પાછા FAIL;
	    --str;
	    ચાલુ;
	}

	c = *str;
	// Special key codes have to be translated to be able to make sense
	// when they are read back.
	જો (c == K_SPECIAL && what != 2)
	{
	    modifiers = 0;
	    જો (str[1] == KS_MODIFIER)
	    {
		modifiers = str[2];
		str += 3;

		// Modifiers can be applied too to multi-byte characters.
		p = mb_unescape(&str);

		જો (p == ખાલી)
		    c = *str;
		નહીંતર
		{
		    // retrieve codepoint (character number) from unescaped string
		    c = (*mb_ptr2char)(p);
		    --str;
		}
	    }
	    જો (c == K_SPECIAL)
	    {
		c = TO_SPECIAL(str[1], str[2]);
		str += 2;
	    }
	    જો (IS_SPECIAL(c) || modifiers)	// special key
	    {
		જો (fputs((અક્ષર *)get_special_ચાવી_name(c, modifiers), fd) < 0)
		    પાછા FAIL;
		ચાલુ;
	    }
	}

	// A '\n' in a map command should be written as <NL>.
	// A '\n' in a set command should be written as \^V^J.
	જો (c == NL)
	{
	    જો (what == 2)
	    {
		જો (fprintf(fd, "\\\026\n") < 0)
		    પાછા FAIL;
	    }
	    નહીંતર
	    {
		જો (fprintf(fd, "<NL>") < 0)
		    પાછા FAIL;
	    }
	    ચાલુ;
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
	જો (what == 2 && (VIM_ISWHITE(c) || c == '"' || c == '\\'))
	{
	    જો (putc('\\', fd) < 0)
		પાછા FAIL;
	}
	નહીંતર જો (c < ' ' || c > '~' || c == '|'
		|| (what == 0 && c == ' ')
		|| (what == 1 && str == strstart && c == ' ')
		|| (what != 2 && c == '<'))
	{
	    જો (putc(Ctrl_V, fd) < 0)
		પાછા FAIL;
	}
	જો (putc(c, fd) < 0)
	    પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Check all mappings for the presence of special key codes.
 * Used after ":set term=xxx".
 */
    શૂન્ય
check_map_keycodes(શૂન્ય)
{
    mapblock_T	*mp;
    char_u	*p;
    પૂર્ણાંક		i;
    char_u	buf[3];
    પૂર્ણાંક		abbr;
    પૂર્ણાંક		hash;
    buf_T	*bp;
    ESTACK_CHECK_DECLARATION;

    validate_maphash();
    // avoids giving error messages
    estack_ધકેલો(ETYPE_INTERNAL, (char_u *)"mappings", 0);
    ESTACK_CHECK_SETUP;

    // Do this once for each buffer, and then once for global
    // mappings/abbreviations with bp == NULL
    માટે (bp = firstbuf; ; bp = bp->b_next)
    {
	// Do the loop twice: Once for mappings, once for abbreviations.
	// Then loop over all map hash lists.
	માટે (abbr = 0; abbr <= 1; ++abbr)
	    માટે (hash = 0; hash < 256; ++hash)
	    {
		જો (abbr)
		{
		    જો (hash)	    // there is only one abbr list
			તોડો;
		    જો (bp != ખાલી)
			mp = bp->b_first_abbr;
		    નહીંતર
			mp = first_abbr;
		}
		નહીંતર
		{
		    જો (bp != ખાલી)
			mp = bp->b_maphash[hash];
		    નહીંતર
			mp = maphash[hash];
		}
		માટે ( ; mp != ખાલી; mp = mp->m_next)
		{
		    માટે (i = 0; i <= 1; ++i)	// do this twice
		    {
			જો (i == 0)
			    p = mp->m_ચાવીઓ;	// once for the "from" part
			નહીંતર
			    p = mp->m_str;	// and once for the "to" part
			જ્યારે (*p)
			{
			    જો (*p == K_SPECIAL)
			    {
				++p;
				જો (*p < 128)   // for "normal" tcap entries
				{
				    buf[0] = p[0];
				    buf[1] = p[1];
				    buf[2] = NUL;
				    (શૂન્ય)add_termcap_entry(buf, FALSE);
				}
				++p;
			    }
			    ++p;
			}
		    }
		}
	    }
	જો (bp == ખાલી)
	    તોડો;
    }
    ESTACK_CHECK_NOW;
    estack_કાઢો();
}

#જો defined(FEAT_EVAL)
/*
 * Check the string "keys" against the lhs of all mappings.
 * Return pointer to rhs of mapping (mapblock->m_str).
 * NULL when no mapping found.
 */
    char_u *
check_map(
    char_u	*ચાવીઓ,
    પૂર્ણાંક		mode,
    પૂર્ણાંક		exact,		// require exact match
    પૂર્ણાંક		ign_mod,	// ignore preceding modifier
    પૂર્ણાંક		abbr,		// do abbreviations
    mapblock_T	**mp_ptr,	// return: pointer to mapblock or NULL
    પૂર્ણાંક		*local_ptr)	// return: buffer-local mapping or NULL
{
    પૂર્ણાંક		hash;
    પૂર્ણાંક		len, minlen;
    mapblock_T	*mp;
    char_u	*s;
    પૂર્ણાંક		local;

    validate_maphash();

    len = (પૂર્ણાંક)STRLEN(ચાવીઓ);
    માટે (local = 1; local >= 0; --local)
	// loop over all hash lists
	માટે (hash = 0; hash < 256; ++hash)
	{
	    જો (abbr)
	    {
		જો (hash > 0)		// there is only one list.
		    તોડો;
		જો (local)
		    mp = curbuf->b_first_abbr;
		નહીંતર
		    mp = first_abbr;
	    }
	    નહીંતર જો (local)
		mp = curbuf->b_maphash[hash];
	    નહીંતર
		mp = maphash[hash];
	    માટે ( ; mp != ખાલી; mp = mp->m_next)
	    {
		// skip entries with wrong mode, wrong length and not matching
		// ones
		જો ((mp->m_mode & mode) && (!exact || mp->m_keylen == len))
		{
		    જો (len > mp->m_keylen)
			minlen = mp->m_keylen;
		    નહીંતર
			minlen = len;
		    s = mp->m_ચાવીઓ;
		    જો (ign_mod && s[0] == K_SPECIAL && s[1] == KS_MODIFIER
							       && s[2] != NUL)
		    {
			s += 3;
			જો (len > mp->m_keylen - 3)
			    minlen = mp->m_keylen - 3;
		    }
		    જો (STRNCMP(s, ચાવીઓ, minlen) == 0)
		    {
			જો (mp_ptr != ખાલી)
			    *mp_ptr = mp;
			જો (local_ptr != ખાલી)
			    *local_ptr = local;
			પાછા mp->m_str;
		    }
		}
	    }
	}

    પાછા ખાલી;
}

/*
 * "hasmapto()" function
 */
    શૂન્ય
f_hasmapto(typval_T *argvars, typval_T *rettv)
{
    char_u	*name;
    char_u	*mode;
    char_u	buf[NUMBUFLEN];
    પૂર્ણાંક		abbr = FALSE;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	પાછા;

    name = tv_get_string(&argvars[0]);
    જો (argvars[1].v_type == VAR_UNKNOWN)
	mode = (char_u *)"nvo";
    નહીંતર
    {
	mode = tv_get_string_buf(&argvars[1], buf);
	જો (argvars[2].v_type != VAR_UNKNOWN)
	    abbr = (પૂર્ણાંક)tv_get_bool(&argvars[2]);
    }

    જો (map_to_exists(name, mode, abbr))
	rettv->vval.v_સંખ્યા = TRUE;
    નહીંતર
	rettv->vval.v_સંખ્યા = FALSE;
}

/*
 * Fill in the empty dictionary with items as defined by maparg builtin.
 */
    સ્થિર શૂન્ય
mapblock2dict(
	mapblock_T  *mp,
	dict_T	    *dict,
	char_u	    *lhsrawalt,	    // may be NULL
	પૂર્ણાંક	    buffer_local,   // false if not buffer local mapping
	પૂર્ણાંક	    abbr)	    // true if abbreviation
{
    char_u	    *lhs = str2special_save(mp->m_ચાવીઓ, TRUE, FALSE);
    char_u	    *mapmode = map_mode_to_chars(mp->m_mode);

    dict_add_string(dict, "lhs", lhs);
    vim_free(lhs);
    dict_add_string(dict, "lhsraw", mp->m_ચાવીઓ);
    જો (lhsrawalt)
	// Also add the value for the simplified entry.
	dict_add_string(dict, "lhsrawalt", lhsrawalt);
    dict_add_string(dict, "rhs", mp->m_orig_str);
    dict_add_સંખ્યા(dict, "noremap", mp->m_noremap ? 1L : 0L);
    dict_add_સંખ્યા(dict, "script", mp->m_noremap == REMAP_SCRIPT
		    ? 1L : 0L);
    dict_add_સંખ્યા(dict, "expr", mp->m_પદાવલિ ? 1L : 0L);
    dict_add_સંખ્યા(dict, "silent", mp->m_silent ? 1L : 0L);
    dict_add_સંખ્યા(dict, "sid", (long)mp->m_script_ctx.sc_sid);
    dict_add_સંખ્યા(dict, "scriptversion",
		    (long)mp->m_script_ctx.sc_version);
    dict_add_સંખ્યા(dict, "lnum", (long)mp->m_script_ctx.sc_lnum);
    dict_add_સંખ્યા(dict, "buffer", (long)buffer_local);
    dict_add_સંખ્યા(dict, "nowait", mp->m_nowait ? 1L : 0L);
    dict_add_string(dict, "mode", mapmode);
    dict_add_સંખ્યા(dict, "abbr", abbr ? 1L : 0L);
    dict_add_સંખ્યા(dict, "mode_bits", mp->m_mode);

    vim_free(mapmode);
}

    સ્થિર શૂન્ય
get_maparg(typval_T *argvars, typval_T *rettv, પૂર્ણાંક exact)
{
    char_u	*ચાવીઓ;
    char_u	*ચાવીઓ_simplified;
    char_u	*which;
    char_u	buf[NUMBUFLEN];
    char_u	*ચાવીઓ_buf = ખાલી;
    char_u	*alt_ચાવીઓ_buf = ખાલી;
    પૂર્ણાંક		did_simplify = FALSE;
    char_u	*rhs;
    પૂર્ણાંક		mode;
    પૂર્ણાંક		abbr = FALSE;
    પૂર્ણાંક		get_dict = FALSE;
    mapblock_T	*mp = ખાલી;
    પૂર્ણાંક		buffer_local;
    પૂર્ણાંક		flags = REPTERM_FROM_PART | REPTERM_DO_LT;

    // return empty string for failure
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;

    ચાવીઓ = tv_get_string(&argvars[0]);
    જો (*ચાવીઓ == NUL)
	પાછા;

    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	which = tv_get_string_buf_chk(&argvars[1], buf);
	જો (argvars[2].v_type != VAR_UNKNOWN)
	{
	    abbr = (પૂર્ણાંક)tv_get_bool(&argvars[2]);
	    જો (argvars[3].v_type != VAR_UNKNOWN)
		get_dict = (પૂર્ણાંક)tv_get_bool(&argvars[3]);
	}
    }
    નહીંતર
	which = (char_u *)"";
    જો (which == ખાલી)
	પાછા;

    mode = get_map_mode(&which, 0);

    ચાવીઓ_simplified = replace_termcodes(ચાવીઓ, &ચાવીઓ_buf, 0, flags,
								&did_simplify);
    rhs = check_map(ચાવીઓ_simplified, mode, exact, FALSE, abbr,
							   &mp, &buffer_local);
    જો (did_simplify)
    {
	// When the lhs is being simplified the not-simplified keys are
	// preferred for printing, like in do_map().
	(શૂન્ય)replace_termcodes(ચાવીઓ, &alt_ચાવીઓ_buf, 0,
					flags | REPTERM_NO_SIMPLIFY, ખાલી);
	rhs = check_map(alt_ચાવીઓ_buf, mode, exact, FALSE, abbr, &mp,
								&buffer_local);
    }

    જો (!get_dict)
    {
	// Return a string.
	જો (rhs != ખાલી)
	{
	    જો (*rhs == NUL)
		rettv->vval.v_string = vim_strsave((char_u *)"<Nop>");
	    નહીંતર
		rettv->vval.v_string = str2special_save(rhs, FALSE, FALSE);
	}

    }
    નહીંતર જો (rettv_dict_alloc(rettv) == OK && rhs != ખાલી)
	mapblock2dict(mp, rettv->vval.v_dict,
			  did_simplify ? ચાવીઓ_simplified : ખાલી,
			  buffer_local, abbr);

    vim_free(ચાવીઓ_buf);
    vim_free(alt_ચાવીઓ_buf);
}

/*
 * "maplist()" function
 */
    શૂન્ય
f_maplist(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    mapblock_T	*mp;
    પૂર્ણાંક		buffer_local;
    char_u	*ચાવીઓ_buf;
    પૂર્ણાંક		did_simplify;
    પૂર્ણાંક		hash;
    char_u	*lhs;
    અચલ પૂર્ણાંક	flags = REPTERM_FROM_PART | REPTERM_DO_LT;
    પૂર્ણાંક		abbr = FALSE;

    જો (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	પાછા;
    જો (argvars[0].v_type != VAR_UNKNOWN)
	abbr = tv_get_bool(&argvars[0]);

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    માટે (buffer_local = 0; buffer_local <= 1; ++buffer_local)
    {
	માટે (hash = 0; hash < 256; ++hash)
	{
	    જો (abbr)
	    {
		જો (hash > 0)		// there is only one abbr list
		    તોડો;
		જો (buffer_local)
		    mp = curbuf->b_first_abbr;
		નહીંતર
		    mp = first_abbr;
	    }
	    નહીંતર જો (buffer_local)
		mp = curbuf->b_maphash[hash];
	    નહીંતર
		mp = maphash[hash];
	    માટે (; mp; mp = mp->m_next)
	    {
		જો (mp->m_simplified)
		    ચાલુ;
		જો ((d = dict_alloc()) == ખાલી)
		    પાછા;
		જો (list_append_dict(rettv->vval.v_list, d) == FAIL)
		{
		    dict_unref(d);
		    પાછા;
		}

		ચાવીઓ_buf = ખાલી;
		did_simplify = FALSE;

		lhs = str2special_save(mp->m_ચાવીઓ, TRUE, FALSE);
		(શૂન્ય)replace_termcodes(lhs, &ચાવીઓ_buf, 0, flags,
								&did_simplify);
		vim_free(lhs);

		mapblock2dict(mp, d,
				 did_simplify ? ચાવીઓ_buf : ખાલી,
				 buffer_local, abbr);
		vim_free(ચાવીઓ_buf);
	    }
	}
    }
}

/*
 * "maparg()" function
 */
    શૂન્ય
f_maparg(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
		પાછા;

    get_maparg(argvars, rettv, TRUE);
}

/*
 * "mapcheck()" function
 */
    શૂન્ય
f_mapcheck(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	પાછા;

    get_maparg(argvars, rettv, FALSE);
}

/*
 * Get the mapping mode from the mode string.
 * It may contain multiple characters, eg "nox", or "!", or ' '
 * Return 0 if there is an error.
 */
    સ્થિર પૂર્ણાંક
get_map_mode_string(char_u *mode_string, પૂર્ણાંક abbr)
{
    char_u	*p = mode_string;
    પૂર્ણાંક		mode = 0;
    પૂર્ણાંક		tmode;
    પૂર્ણાંક		modec;
    અચલ પૂર્ણાંક	MASK_V = MODE_VISUAL | MODE_SELECT;
    અચલ પૂર્ણાંક	MASK_MAP = MODE_VISUAL | MODE_SELECT | MODE_NORMAL
							     | MODE_OP_PENDING;
    અચલ પૂર્ણાંક	MASK_BANG = MODE_ઉમેરો | MODE_CMDLINE;

    જો (*p == NUL)
	p = (char_u *)" ";	// compatibility
    જ્યારે ((modec = *p++))
    {
	પસંદગી (modec)
	{
	    સ્થિતિ 'i': tmode = MODE_ઉમેરો;	તોડો;
	    સ્થિતિ 'l': tmode = MODE_LANGMAP;	તોડો;
	    સ્થિતિ 'c': tmode = MODE_CMDLINE;	તોડો;
	    સ્થિતિ 'n': tmode = MODE_NORMAL;	તોડો;
	    સ્થિતિ 'x': tmode = MODE_VISUAL;	તોડો;
	    સ્થિતિ 's': tmode = MODE_SELECT;	તોડો;
	    સ્થિતિ 'o': tmode = MODE_OP_PENDING;	તોડો;
	    સ્થિતિ 't': tmode = MODE_TERMINAL;	તોડો;
	    સ્થિતિ 'v': tmode = MASK_V;		તોડો;
	    સ્થિતિ '!': tmode = MASK_BANG;	તોડો;
	    સ્થિતિ ' ': tmode = MASK_MAP;		તોડો;
	    મૂળભૂત:
		      પાછા 0; // error, unknown mode character
	}
	mode |= tmode;
    }
    જો ((abbr && (mode & ~MASK_BANG) != 0)
	|| (!abbr && (mode & (mode-1)) != 0 // more than one bit set
	    && (
		// false if multiple bits set in mode and mode is fully
		// contained in one mask
		!(((mode & MASK_BANG) != 0 && (mode & ~MASK_BANG) == 0)
		    || ((mode & MASK_MAP) != 0 && (mode & ~MASK_MAP) == 0)))))
	પાછા 0;

    પાછા mode;
}

/*
 * "mapset()" function
 */
    શૂન્ય
f_mapset(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*which;
    પૂર્ણાંક		mode;
    char_u	buf[NUMBUFLEN];
    પૂર્ણાંક		is_abbr;
    dict_T	*d;
    char_u	*lhs;
    char_u	*lhsraw;
    char_u	*lhsrawalt;
    char_u	*rhs;
    char_u	*orig_rhs;
    char_u	*arg_buf = ખાલી;
    પૂર્ણાંક		noremap;
    પૂર્ણાંક		પદાવલિ;
    પૂર્ણાંક		silent;
    પૂર્ણાંક		buffer;
    scid_T	sid;
    પૂર્ણાંક		scriptversion;
    linenr_T	lnum;
    mapblock_T	**map_table = maphash;
    mapblock_T  **abbr_table = &first_abbr;
    પૂર્ણાંક		nowait;
    char_u	*arg;
    પૂર્ણાંક		dict_only;
    mapblock_T	*mp_result[2] = {ખાલી, ખાલી};

    જો (check_secure())
	પાછા;

    // If first arg is a dict, then that's the only arg permitted.
    dict_only = argvars[0].v_type == VAR_DICT;
    જો (in_vim9script()
	    && (check_for_string_or_dict_arg(argvars, 0) == FAIL
		|| (dict_only && check_for_unknown_arg(argvars, 1) == FAIL)
		|| (!dict_only
		    && (check_for_string_arg(argvars, 0) == FAIL
			|| check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_dict_arg(argvars, 2) == FAIL))))
	પાછા;

    જો (dict_only)
    {
	d = argvars[0].vval.v_dict;
	which = dict_get_string(d, "mode", FALSE);
	is_abbr = dict_get_bool(d, "abbr", -1);
	જો (which == ખાલી || is_abbr < 0)
	{
	    emsg(_(e_entries_missing_in_mapset_dict_argument));
	    પાછા;
	}
    }
    નહીંતર
    {
	which = tv_get_string_buf_chk(&argvars[0], buf);
	જો (which == ખાલી)
	    પાછા;
	is_abbr = (પૂર્ણાંક)tv_get_bool(&argvars[1]);

	જો (check_for_dict_arg(argvars, 2) == FAIL)
	    પાછા;
	d = argvars[2].vval.v_dict;
    }
    mode = get_map_mode_string(which, is_abbr);
    જો (mode == 0)
    {
	semsg(_(e_illegal_map_mode_string_str), which);
	પાછા;
    }


    // Get the values in the same order as above in get_maparg().
    lhs = dict_get_string(d, "lhs", FALSE);
    lhsraw = dict_get_string(d, "lhsraw", FALSE);
    lhsrawalt = dict_get_string(d, "lhsrawalt", FALSE);
    rhs = dict_get_string(d, "rhs", FALSE);
    જો (lhs == ખાલી || lhsraw == ખાલી || rhs == ખાલી)
    {
	emsg(_(e_entries_missing_in_mapset_dict_argument));
	પાછા;
    }
    orig_rhs = rhs;

    noremap = dict_get_સંખ્યા(d, "noremap") ? REMAP_NONE: 0;
    જો (dict_get_સંખ્યા(d, "script") != 0)
	noremap = REMAP_SCRIPT;
    પદાવલિ = dict_get_સંખ્યા(d, "expr") != 0;
    silent = dict_get_સંખ્યા(d, "silent") != 0;
    sid = dict_get_સંખ્યા(d, "sid");
    scriptversion = dict_get_સંખ્યા(d, "scriptversion");
    lnum = dict_get_સંખ્યા(d, "lnum");
    buffer = dict_get_સંખ્યા(d, "buffer");
    nowait = dict_get_સંખ્યા(d, "nowait") != 0;
    // mode from the dict is not used

    જો (STRICMP(rhs, "<nop>") == 0)	// "<Nop>" means nothing
	rhs = (char_u *)"";
    નહીંતર
	rhs = replace_termcodes(rhs, &arg_buf, sid,
					REPTERM_DO_LT | REPTERM_SPECIAL, ખાલી);

    જો (buffer)
    {
	map_table = curbuf->b_maphash;
	abbr_table = &curbuf->b_first_abbr;
    }

    // Delete any existing mapping for this lhs and mode.
    જો (buffer)
    {
	arg = alloc(STRLEN(lhs) + STRLEN("<buffer>") + 1);
	જો (arg == ખાલી)
	    પાછા;
	STRCPY(arg, "<buffer>");
	STRCPY(arg + 8, lhs);
    }
    નહીંતર
    {
	arg = vim_strsave(lhs);
	જો (arg == ખાલી)
	    પાછા;
    }
    do_map(MAPTYPE_UNMAP_LHS, arg, mode, is_abbr);
    vim_free(arg);

    mp_result[0] = map_add(map_table, abbr_table, lhsraw, rhs, orig_rhs,
			    noremap, nowait, silent, mode, is_abbr, પદાવલિ, sid,
			    scriptversion, lnum, 0);
    જો (lhsrawalt != ખાલી)
	mp_result[1] = map_add(map_table, abbr_table, lhsrawalt, rhs, orig_rhs,
			    noremap, nowait, silent, mode, is_abbr, પદાવલિ, sid,
			    scriptversion, lnum, 1);

    જો (mp_result[0] != ખાલી && mp_result[1] != ખાલી)
    {
	mp_result[0]->m_alt = mp_result[1];
	mp_result[1]->m_alt = mp_result[0];
    }

    vim_free(arg_buf);
}
#endif


#જો defined(MSWIN) || defined(MACOS_X)

# define VIS_SEL	(MODE_VISUAL | MODE_SELECT)	// abbreviation

/*
 * Default mappings for some often used keys.
 */
સંરચના initmap
{
    char_u	*arg;
    પૂર્ણાંક		mode;
};

# ifdef FEAT_GUI_MSWIN
// Use the Windows (CUA) keybindings. (GUI)
સ્થિર સંરચના initmap initmappings[] =
{
	// paste, copy and cut
	{(char_u *)"<S-Insert> \"*P", MODE_NORMAL},
	{(char_u *)"<S-Insert> \"-d\"*P", VIS_SEL},
	{(char_u *)"<S-Insert> <C-R><C-O>*", MODE_ઉમેરો | MODE_CMDLINE},
	{(char_u *)"<C-Insert> \"*y", VIS_SEL},
	{(char_u *)"<S-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-X> \"*d", VIS_SEL},
	// Missing: CTRL-C (cancel) and CTRL-V (block selection)
};
# endif

# જો defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
// Use the Windows (CUA) keybindings. (Console)
સ્થિર સંરચના initmap cinitmappings[] =
{
	{(char_u *)"\316w <C-Home>", MODE_NORMAL | VIS_SEL},
	{(char_u *)"\316w <C-Home>", MODE_ઉમેરો | MODE_CMDLINE},
	{(char_u *)"\316u <C-End>", MODE_NORMAL | VIS_SEL},
	{(char_u *)"\316u <C-End>", MODE_ઉમેરો | MODE_CMDLINE},

	// paste, copy and cut
#  ifdef FEAT_CLIPBOARD
	{(char_u *)"\316\324 \"*P", MODE_NORMAL},   // SHIFT-Insert is "*P
	{(char_u *)"\316\324 \"-d\"*P", VIS_SEL},   // SHIFT-Insert is "-d"*P
	{(char_u *)"\316\324 \022\017*", MODE_ઉમેરો},  // SHIFT-Insert is ^R^O*
	{(char_u *)"\316\325 \"*y", VIS_SEL},	    // CTRL-Insert is "*y
	{(char_u *)"\316\327 \"*d", VIS_SEL},	    // SHIFT-Del is "*d
	{(char_u *)"\316\330 \"*d", VIS_SEL},	    // CTRL-Del is "*d
	{(char_u *)"\030 \"*d", VIS_SEL},	    // CTRL-X is "*d
#  નહીંતર
	{(char_u *)"\316\324 P", MODE_NORMAL},	    // SHIFT-Insert is P
	{(char_u *)"\316\324 \"-dP", VIS_SEL},	    // SHIFT-Insert is "-dP
	{(char_u *)"\316\324 \022\017\"", MODE_ઉમેરો}, // SHIFT-Insert is ^R^O"
	{(char_u *)"\316\325 y", VIS_SEL},	    // CTRL-Insert is y
	{(char_u *)"\316\327 d", VIS_SEL},	    // SHIFT-Del is d
	{(char_u *)"\316\330 d", VIS_SEL},	    // CTRL-Del is d
#  endif
};
# endif

# જો defined(MACOS_X)
સ્થિર સંરચના initmap initmappings[] =
{
	// Use the Standard MacOS binding.
	// paste, copy and cut
	{(char_u *)"<D-v> \"*P", MODE_NORMAL},
	{(char_u *)"<D-v> \"-d\"*P", VIS_SEL},
	{(char_u *)"<D-v> <C-R>*", MODE_ઉમેરો | MODE_CMDLINE},
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
    શૂન્ય
init_mappings(શૂન્ય)
{
#જો defined(MSWIN) || defined(MACOS_X)
    પૂર્ણાંક		i;

# જો defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    જો (!gui.starting)
#  endif
    {
	માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(cinitmappings); ++i)
	    add_map(cinitmappings[i].arg, cinitmappings[i].mode, FALSE);
    }
# endif
# જો defined(FEAT_GUI_MSWIN) || defined(MACOS_X)
    માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(initmappings); ++i)
	add_map(initmappings[i].arg, initmappings[i].mode, FALSE);
# endif
#endif
}

/*
 * Add a mapping "map" for mode "mode".
 * When "nore" is TRUE use MAPTYPE_NOREMAP.
 * Need to put string in allocated memory, because do_map() will modify it.
 */
    શૂન્ય
add_map(char_u *map, પૂર્ણાંક mode, પૂર્ણાંક nore)
{
    char_u	*s;
    char_u	*cpo_save = p_cpo;

    p_cpo = empty_option;	// Allow <> notation
    s = vim_strsave(map);
    જો (s != ખાલી)
    {
	(શૂન્ય)do_map(nore ? MAPTYPE_NOREMAP : MAPTYPE_MAP, s, mode, FALSE);
	vim_free(s);
    }
    p_cpo = cpo_save;
}

#જો defined(FEAT_LANGMAP)
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
પ્રકારવ્યાખ્યા સંરચના
{
    પૂર્ણાંક	    from;
    પૂર્ણાંક     to;
} langmap_entry_T;

સ્થિર garray_T langmap_mapga;

/*
 * Search for an entry in "langmap_mapga" for "from".  If found set the "to"
 * field.  If not found insert a new entry at the appropriate location.
 */
    સ્થિર શૂન્ય
langmap_set_entry(પૂર્ણાંક from, પૂર્ણાંક to)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_data);
    પૂર્ણાંક		    a = 0;
    પૂર્ણાંક		    b = langmap_mapga.ga_len;

    // Do a binary search for an existing entry.
    જ્યારે (a != b)
    {
	પૂર્ણાંક i = (a + b) / 2;
	પૂર્ણાંક d = entries[i].from - from;

	જો (d == 0)
	{
	    entries[i].to = to;
	    પાછા;
	}
	જો (d < 0)
	    a = i + 1;
	નહીંતર
	    b = i;
    }

    જો (ga_grow(&langmap_mapga, 1) == FAIL)
	પાછા;  // out of memory

    // insert new entry at position "a"
    entries = (langmap_entry_T *)(langmap_mapga.ga_data) + a;
    mch_memmove(entries + 1, entries,
			(langmap_mapga.ga_len - a) * કદ(langmap_entry_T));
    ++langmap_mapga.ga_len;
    entries[0].from = from;
    entries[0].to = to;
}

/*
 * Apply 'langmap' to multi-byte character "c" and return the result.
 */
    પૂર્ણાંક
langmap_adjust_mb(પૂર્ણાંક c)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_data);
    પૂર્ણાંક a = 0;
    પૂર્ણાંક b = langmap_mapga.ga_len;

    જ્યારે (a != b)
    {
	પૂર્ણાંક i = (a + b) / 2;
	પૂર્ણાંક d = entries[i].from - c;

	જો (d == 0)
	    પાછા entries[i].to;  // found matching entry
	જો (d < 0)
	    a = i + 1;
	નહીંતર
	    b = i;
    }
    પાછા c;  // no entry found, return "c" unmodified
}

    શૂન્ય
langmap_init(શૂન્ય)
{
    પૂર્ણાંક i;

    માટે (i = 0; i < 256; i++)
	langmap_mapchar[i] = i;	 // we init with a one-to-one map
    ga_init2(&langmap_mapga, કદ(langmap_entry_T), 8);
}

/*
 * Called when langmap option is set; the language map can be
 * changed at any time!
 */
    અક્ષર *
did_set_langmap(optset_T *args UNUSED)
{
    char_u  *p;
    char_u  *p2;
    પૂર્ણાંક	    from, to;

    ga_clear(&langmap_mapga);		    // clear the previous map first
    langmap_init();			    // back to one-to-one map

    માટે (p = p_langmap; p[0] != NUL; )
    {
	માટે (p2 = p; p2[0] != NUL && p2[0] != ',' && p2[0] != ';';
							       MB_PTR_ADV(p2))
	{
	    જો (p2[0] == '\\' && p2[1] != NUL)
		++p2;
	}
	જો (p2[0] == ';')
	    ++p2;	    // abcd;ABCD form, p2 points to A
	નહીંતર
	    p2 = ખાલી;	    // aAbBcCdD form, p2 is NULL
	જ્યારે (p[0])
	{
	    જો (p[0] == ',')
	    {
		++p;
		તોડો;
	    }
	    જો (p[0] == '\\' && p[1] != NUL)
		++p;
	    from = (*mb_ptr2char)(p);
	    to = NUL;
	    જો (p2 == ખાલી)
	    {
		MB_PTR_ADV(p);
		જો (p[0] != ',')
		{
		    જો (p[0] == '\\')
			++p;
		    to = (*mb_ptr2char)(p);
		}
	    }
	    નહીંતર
	    {
		જો (p2[0] != ',')
		{
		    જો (p2[0] == '\\')
			++p2;
		    to = (*mb_ptr2char)(p2);
		}
	    }
	    જો (to == NUL)
	    {
		sprintf(args->os_errbuf,
			_(e_langmap_matching_character_missing_for_str),
			transchar(from));
		પાછા args->os_errbuf;
	    }

	    જો (from >= 256)
		langmap_set_entry(from, to);
	    નહીંતર
		langmap_mapchar[from & 255] = to;

	    // Advance to next pair
	    MB_PTR_ADV(p);
	    જો (p2 != ખાલી)
	    {
		MB_PTR_ADV(p2);
		જો (*p == ';')
		{
		    p = p2;
		    જો (p[0] != NUL)
		    {
			જો (p[0] != ',')
			{
			    vim_snprintf(args->os_errbuf, args->os_errbuflen,
				    _(e_langmap_extra_characters_after_semicolon_str),
				    p);
			    પાછા args->os_errbuf;
			}
			++p;
		    }
		    તોડો;
		}
	    }
	}
    }

    પાછા ખાલી;
}
#endif

    સ્થિર શૂન્ય
do_exmap(exarg_T *eap, પૂર્ણાંક isabbrev)
{
    પૂર્ણાંક	    mode;
    char_u  *cmdp;

    cmdp = eap->cmd;
    mode = get_map_mode(&cmdp, eap->forceit || isabbrev);

    પસંદગી (do_map(*cmdp == 'n' ? MAPTYPE_NOREMAP
				: *cmdp == 'u' ? MAPTYPE_UNMAP : MAPTYPE_MAP,
						    eap->arg, mode, isabbrev))
    {
	સ્થિતિ 1: emsg(_(e_invalid_argument));
		તોડો;
	સ્થિતિ 2: emsg((isabbrev ? _(e_no_such_abbreviation)
						      : _(e_no_such_mapping)));
		તોડો;
    }
}

/*
 * ":abbreviate" and friends.
 */
    શૂન્ય
ex_abbreviate(exarg_T *eap)
{
    do_exmap(eap, TRUE);	// almost the same as mapping
}

/*
 * ":map" and friends.
 */
    શૂન્ય
ex_map(exarg_T *eap)
{
    // If we are sourcing .exrc or .vimrc in current directory we
    // print the mappings for security reasons.
    જો (secure)
    {
	secure = 2;
	સંદેશ_outtrans(eap->cmd);
	સંદેશ_putchar('\n');
    }
    do_exmap(eap, FALSE);
}

/*
 * ":unmap" and friends.
 */
    શૂન્ય
ex_unmap(exarg_T *eap)
{
    do_exmap(eap, FALSE);
}

/*
 * ":mapclear" and friends.
 */
    શૂન્ય
ex_mapclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, eap->forceit, FALSE);
}

/*
 * ":abclear" and friends.
 */
    શૂન્ય
ex_abclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, TRUE, TRUE);
}
