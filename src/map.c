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

#அந்தர்பாவ "vim.h"

/*
 * List used for abbreviations.
 */
நிலையான mapblock_T	*first_abbr = வெற்று; // first entry in abbrlist

/*
 * Each mapping is put in one of the 256 hash lists, to speed up finding it.
 */
நிலையான mapblock_T	*(maphash[256]);
நிலையான முழு		maphash_valid = FALSE;

// When non-zero then no mappings can be added or removed.  Prevents mappings
// to change while listing them.
நிலையான முழு		map_locked = 0;

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
get_maphash_list(முழு state, முழு c)
{
    திரும்பு maphash[MAP_HASH(state, c)];
}

/*
 * Get the buffer-local hashed map list for "state" and first character "c".
 */
    mapblock_T *
get_buf_maphash_list(முழு state, முழு c)
{
    திரும்பு curbuf->b_maphash[MAP_HASH(state, c)];
}

    முழு
is_maphash_valid(வெறுமை)
{
    திரும்பு maphash_valid;
}

/*
 * Initialize maphash[] for first use.
 */
    நிலையான வெறுமை
validate_maphash(வெறுமை)
{
    எனில் (maphash_valid)
	திரும்பு;

    CLEAR_FIELD(maphash);
    maphash_valid = TRUE;
}

/*
 * Delete one entry from the abbrlist or maphash[].
 * "mpp" is a pointer to the m_next field of the PREVIOUS entry!
 */
    நிலையான வெறுமை
map_free(mapblock_T **mpp)
{
    mapblock_T	*mp;

    mp = *mpp;
    vim_free(mp->m_சாவிகள்);
    எனில் (mp->m_alt != வெற்று)
	mp->m_alt->m_alt = வெற்று;
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
    நிலையான char_u *
map_mode_to_chars(முழு mode)
{
    garray_T    mapmode;

    ga_init2(&mapmode, 1, 7);

    எனில் ((mode & (MODE_செருகு | MODE_CMDLINE)) == (MODE_செருகு | MODE_CMDLINE))
	ga_append(&mapmode, '!');			// :map!
    இல்லையெனில் எனில் (mode & MODE_செருகு)
	ga_append(&mapmode, 'i');			// :imap
    இல்லையெனில் எனில் (mode & MODE_LANGMAP)
	ga_append(&mapmode, 'l');			// :lmap
    இல்லையெனில் எனில் (mode & MODE_CMDLINE)
	ga_append(&mapmode, 'c');			// :cmap
    இல்லையெனில் எனில் ((mode
		 & (MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING))
		== (MODE_NORMAL | MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING))
	ga_append(&mapmode, ' ');			// :map
    இல்லையெனில்
    {
	எனில் (mode & MODE_NORMAL)
	    ga_append(&mapmode, 'n');			// :nmap
	எனில் (mode & MODE_OP_PENDING)
	    ga_append(&mapmode, 'o');			// :omap
	எனில் (mode & MODE_TERMINAL)
	    ga_append(&mapmode, 't');			// :tmap
	எனில் ((mode & (MODE_VISUAL | MODE_SELECT)) == (MODE_VISUAL | MODE_SELECT))
	    ga_append(&mapmode, 'v');			// :vmap
	இல்லையெனில்
	{
	    எனில் (mode & MODE_VISUAL)
		ga_append(&mapmode, 'x');		// :xmap
	    எனில் (mode & MODE_SELECT)
		ga_append(&mapmode, 's');		// :smap
	}
    }

    ga_append(&mapmode, NUL);
    திரும்பு (char_u *)mapmode.ga_data;
}

/*
 * Output a line for one mapping.
 */
    நிலையான வெறுமை
showmap(
    mapblock_T	*mp,
    முழு		local)	    // TRUE for buffer-local map
{
    முழு		len = 1;
    char_u	*mapchars;

    எனில் (message_filtered(mp->m_சாவிகள்) && message_filtered(mp->m_str))
	திரும்பு;

    // Prevent mappings to be cleared while at the more prompt.
    // Must jump to "theend" instead of returning.
    ++map_locked;

    எனில் (செய்தி_didout || செய்தி_silent != 0)
    {
	செய்தி_putchar('\n');
	எனில் (got_int)	    // 'q' typed at MORE prompt
	    செல் theend;
    }

    mapchars = map_mode_to_chars(mp->m_mode);
    எனில் (mapchars != வெற்று)
    {
	செய்தி_puts((எழுத்து *)mapchars);
	len = (முழு)STRLEN(mapchars);
	vim_free(mapchars);
    }

    போது (++len <= 3)
	செய்தி_putchar(' ');

    // Display the LHS.  Get length of what we write.
    len = செய்தி_outtrans_special(mp->m_சாவிகள், TRUE, 0);
    செய்
    {
	செய்தி_putchar(' ');		// pad with blanks
	++len;
    } போது (len < 12);

    எனில் (mp->m_noremap == REMAP_NONE)
	செய்தி_puts_attr("*", HL_ATTR(HLF_8));
    இல்லையெனில் எனில் (mp->m_noremap == REMAP_SCRIPT)
	செய்தி_puts_attr("&", HL_ATTR(HLF_8));
    இல்லையெனில்
	செய்தி_putchar(' ');

    எனில் (local)
	செய்தி_putchar('@');
    இல்லையெனில்
	செய்தி_putchar(' ');

    // Use FALSE below if we only want things like <Up> to show up as such on
    // the rhs, and not M-x etc, TRUE gets both -- webb
    எனில் (*mp->m_str == NUL)
	செய்தி_puts_attr("<Nop>", HL_ATTR(HLF_8));
    இல்லையெனில்
	செய்தி_outtrans_special(mp->m_str, FALSE, 0);
#ifdef FEAT_EVAL
    எனில் (p_verbose > 0)
	last_set_செய்தி(mp->m_script_ctx);
#endif
    செய்தி_clr_eos();
    out_flush();			// show one line at a time

theend:
    --map_locked;
}

    நிலையான mapblock_T *
map_add(
	mapblock_T  **map_table,
	mapblock_T  **abbr_table,
	char_u	    *சாவிகள்,
	char_u	    *rhs,
	char_u	    *orig_rhs,
	முழு	    noremap,
	முழு	    nowait,
	முழு	    silent,
	முழு	    mode,
	முழு	    is_abbr,
#ifdef FEAT_EVAL
	முழு	    கோவை,
	scid_T	    sid,	    // 0 to use current_sctx
	முழு	    scriptversion,
	linenr_T    lnum,
#endif
	முழு	    simplified)
{
    mapblock_T	*mp = ALLOC_CLEAR_ONE(mapblock_T);

    எனில் (mp == வெற்று)
	திரும்பு வெற்று;

    // If CTRL-C has been mapped, don't always use it for Interrupting.
    எனில் (*சாவிகள் == Ctrl_C)
    {
	எனில் (map_table == curbuf->b_maphash)
	    curbuf->b_mapped_ctrl_c |= mode;
	இல்லையெனில்
	    mapped_ctrl_c |= mode;
    }

    mp->m_சாவிகள் = vim_strsave(சாவிகள்);
    mp->m_str = vim_strsave(rhs);
    mp->m_orig_str = vim_strsave(orig_rhs);
    எனில் (mp->m_சாவிகள் == வெற்று || mp->m_str == வெற்று)
    {
	vim_free(mp->m_சாவிகள்);
	vim_free(mp->m_str);
	vim_free(mp->m_orig_str);
	vim_free(mp);
	திரும்பு வெற்று;
    }
    mp->m_keylen = (முழு)STRLEN(mp->m_சாவிகள்);
    mp->m_noremap = noremap;
    mp->m_nowait = nowait;
    mp->m_silent = silent;
    mp->m_mode = mode;
    mp->m_simplified = simplified;
#ifdef FEAT_EVAL
    mp->m_கோவை = கோவை;
    எனில் (sid != 0)
    {
	mp->m_script_ctx.sc_sid = sid;
	mp->m_script_ctx.sc_lnum = lnum;
	mp->m_script_ctx.sc_version = scriptversion;
    }
    இல்லையெனில்
    {
	mp->m_script_ctx = current_sctx;
	எனில் (cmdmod.cmod_flags & CMOD_LEGACY)
	    mp->m_script_ctx.sc_version = 1;
	mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
    }
#endif

    // add the new entry in front of the abbrlist or maphash[] list
    எனில் (is_abbr)
    {
	mp->m_next = *abbr_table;
	*abbr_table = mp;
    }
    இல்லையெனில்
    {
	முழு n = MAP_HASH(mp->m_mode, mp->m_சாவிகள்[0]);

	mp->m_next = map_table[n];
	map_table[n] = mp;
    }
    திரும்பு mp;
}

/*
 * List mappings.  When "haskey" is FALSE all mappings, otherwise mappings that
 * match "keys[keys_len]".
 */
    நிலையான வெறுமை
list_mappings(
	முழு	keyround,
	முழு	abbrev,
	முழு	haskey,
	char_u	*சாவிகள்,
	முழு	சாவிகள்_len,
	முழு	mode,
	முழு	*did_local)
{
    // Prevent mappings to be cleared while at the more prompt.
    ++map_locked;

    எனில் (p_verbose > 0 && keyround == 1)
    {
	எனில் (seen_Modify_Other_சாவிகள்)
	    செய்தி_puts(_("Seen modifyOtherKeys: true\n"));

	எனில் (modify_otherkeys_state != MOKS_INITIAL)
	{
	    எழுத்து *name = _("Unknown");
	    தேர்வு (modify_otherkeys_state)
	    {
		நிலை MOKS_INITIAL: நிறுத்து;
		நிலை MOKS_OFF: name = _("Off"); நிறுத்து;
		நிலை MOKS_ENABLED: name = _("On"); நிறுத்து;
		நிலை MOKS_DISABLED: name = _("Disabled"); நிறுத்து;
		நிலை MOKS_AFTER_T_TE: name = _("Cleared"); நிறுத்து;
	    }

	    எழுத்து buf[200];
	    vim_snprintf(buf, அளவு(buf),
				    _("modifyOtherKeys detected: %s\n"), name);
	    செய்தி_puts(buf);
	}

	எனில் (kitty_protocol_state != KKPS_INITIAL)
	{
	    எழுத்து *name = _("Unknown");
	    தேர்வு (kitty_protocol_state)
	    {
		நிலை KKPS_INITIAL: நிறுத்து;
		நிலை KKPS_OFF: name = _("Off"); நிறுத்து;
		நிலை KKPS_ENABLED: name = _("On"); நிறுத்து;
		நிலை KKPS_DISABLED: name = _("Disabled"); நிறுத்து;
		நிலை KKPS_AFTER_T_TE: name = _("Cleared"); நிறுத்து;
	    }

	    எழுத்து buf[200];
	    vim_snprintf(buf, அளவு(buf),
				     _("Kitty keyboard protocol: %s\n"), name);
	    செய்தி_puts(buf);
	}
    }

    // need to loop over all global hash lists
    க்கு (முழு hash = 0; hash < 256 && !got_int; ++hash)
    {
	mapblock_T	*mp;

	எனில் (abbrev)
	{
	    எனில் (hash != 0)	// there is only one abbreviation list
		நிறுத்து;
	    mp = curbuf->b_first_abbr;
	}
	இல்லையெனில்
	    mp = curbuf->b_maphash[hash];
	க்கு ( ; mp != வெற்று && !got_int; mp = mp->m_next)
	{
	    // check entries with the same mode
	    எனில் (!mp->m_simplified && (mp->m_mode & mode) != 0)
	    {
		எனில் (!haskey)		    // show all entries
		{
		    showmap(mp, TRUE);
		    *did_local = TRUE;
		}
		இல்லையெனில்
		{
		    முழு n = mp->m_keylen;
		    எனில் (STRNCMP(mp->m_சாவிகள், சாவிகள்,
				   (size_t)(n < சாவிகள்_len ? n : சாவிகள்_len)) == 0)
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
    முழு
do_map(
    முழு		maptype,
    char_u	*arg,
    முழு		mode,
    முழு		abbrev)		// not a mapping but an abbreviation
{
    char_u	*சாவிகள்;
    mapblock_T	*mp, **mpp;
    mapblock_T	*mp_result[2] = {வெற்று, வெற்று};
    char_u	*rhs;
    char_u	*p;
    முழு		n;
    முழு		len = 0;	// init for GCC
    முழு		hasarg;
    முழு		haskey;
    முழு		do_print;
    முழு		keyround;
    char_u	*சாவிகள்_buf = வெற்று;
    char_u	*alt_சாவிகள்_buf = வெற்று;
    char_u	*arg_buf = வெற்று;
    முழு		retval = 0;
    முழு		do_backslash;
    mapblock_T	**abbr_table;
    mapblock_T	**map_table;
    முழு		unique = FALSE;
    முழு		nowait = FALSE;
    முழு		silent = FALSE;
    முழு		special = FALSE;
#ifdef FEAT_EVAL
    முழு		கோவை = FALSE;
#endif
    முழு		did_simplify = FALSE;
    முழு		unmap_lhs_only = FALSE;
    முழு		noremap;
    char_u      *orig_rhs;

    சாவிகள் = arg;
    map_table = maphash;
    abbr_table = &first_abbr;

    எனில் (maptype == MAPTYPE_UNMAP_LHS)
    {
	unmap_lhs_only = TRUE;
	maptype = MAPTYPE_UNMAP;
    }

    // For ":noremap" don't remap, otherwise do remap.
    எனில் (maptype == MAPTYPE_NOREMAP)
	noremap = REMAP_NONE;
    இல்லையெனில்
	noremap = REMAP_YES;

    // Accept <buffer>, <nowait>, <silent>, <expr> <script> and <unique> in
    // any order.
    க்கு (;;)
    {
	// Check for "<buffer>": mapping local to buffer.
	எனில் (STRNCMP(சாவிகள், "<buffer>", 8) == 0)
	{
	    சாவிகள் = skipwhite(சாவிகள் + 8);
	    map_table = curbuf->b_maphash;
	    abbr_table = &curbuf->b_first_abbr;
	    தொடர்;
	}

	// Check for "<nowait>": don't wait for more characters.
	எனில் (STRNCMP(சாவிகள், "<nowait>", 8) == 0)
	{
	    சாவிகள் = skipwhite(சாவிகள் + 8);
	    nowait = TRUE;
	    தொடர்;
	}

	// Check for "<silent>": don't echo commands.
	எனில் (STRNCMP(சாவிகள், "<silent>", 8) == 0)
	{
	    சாவிகள் = skipwhite(சாவிகள் + 8);
	    silent = TRUE;
	    தொடர்;
	}

	// Check for "<special>": accept special keys in <>
	எனில் (STRNCMP(சாவிகள், "<special>", 9) == 0)
	{
	    சாவிகள் = skipwhite(சாவிகள் + 9);
	    special = TRUE;
	    தொடர்;
	}

#ifdef FEAT_EVAL
	// Check for "<script>": remap script-local mappings only
	எனில் (STRNCMP(சாவிகள், "<script>", 8) == 0)
	{
	    சாவிகள் = skipwhite(சாவிகள் + 8);
	    noremap = REMAP_SCRIPT;
	    தொடர்;
	}

	// Check for "<expr>": {rhs} is an expression.
	எனில் (STRNCMP(சாவிகள், "<expr>", 6) == 0)
	{
	    சாவிகள் = skipwhite(சாவிகள் + 6);
	    கோவை = TRUE;
	    தொடர்;
	}
#endif
	// Check for "<unique>": don't overwrite an existing mapping.
	எனில் (STRNCMP(சாவிகள், "<unique>", 8) == 0)
	{
	    சாவிகள் = skipwhite(சாவிகள் + 8);
	    unique = TRUE;
	    தொடர்;
	}
	நிறுத்து;
    }

    validate_maphash();

    // Find end of keys and skip CTRL-Vs (and backslashes) in it.
    // Accept backslash like CTRL-V when 'cpoptions' does not contain 'B'.
    // with :unmap white space is included in the keys, no argument possible.
    p = சாவிகள்;
    do_backslash = (vim_strchr(p_cpo, CPO_BSLASH) == வெற்று);
    போது (*p && (maptype == MAPTYPE_UNMAP || !VIM_ISWHITE(*p)))
    {
	எனில் ((p[0] == Ctrl_V || (do_backslash && p[0] == '\\')) &&
								  p[1] != NUL)
	    ++p;		// skip CTRL-V or backslash
	++p;
    }
    எனில் (*p != NUL)
	*p++ = NUL;

    p = skipwhite(p);
    rhs = p;
    hasarg = (*rhs != NUL);
    haskey = (*சாவிகள் != NUL);
    do_print = !haskey || (maptype != MAPTYPE_UNMAP && !hasarg);

    // check for :unmap without argument
    எனில் (maptype == MAPTYPE_UNMAP && !haskey)
    {
	retval = 1;
	செல் theend;
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
    எனில் (haskey)
    {
	char_u	*new_சாவிகள்;
	முழு	flags = REPTERM_FROM_PART | REPTERM_DO_LT;

	எனில் (special)
	    flags |= REPTERM_SPECIAL;
	new_சாவிகள் = replace_termcodes(சாவிகள், &சாவிகள்_buf, 0, flags, &did_simplify);
	எனில் (did_simplify)
	    (வெறுமை)replace_termcodes(சாவிகள், &alt_சாவிகள்_buf, 0,
					    flags | REPTERM_NO_SIMPLIFY, வெற்று);
	சாவிகள் = new_சாவிகள்;
    }
    orig_rhs = rhs;
    எனில் (hasarg)
    {
	எனில் (STRICMP(rhs, "<nop>") == 0)	    // "<Nop>" means nothing
	    rhs = (char_u *)"";
	இல்லையெனில்
	    rhs = replace_termcodes(rhs, &arg_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), வெற்று);
    }

    /*
     * The following is done twice if we have two versions of keys:
     * "alt_keys_buf" is not NULL.
     */
    க்கு (keyround = 1; keyround <= 2; ++keyround)
    {
	முழு	did_it = FALSE;
	முழு	did_local = FALSE;
	முழு	keyround1_simplified = keyround == 1 && did_simplify;
	முழு	round;
	முழு	இலக்கம்_rounds;

	எனில் (keyround == 2)
	{
	    எனில் (alt_சாவிகள்_buf == வெற்று)
		நிறுத்து;
	    சாவிகள் = alt_சாவிகள்_buf;
	}
	இல்லையெனில் எனில் (alt_சாவிகள்_buf != வெற்று && do_print)
	    // when printing always use the not-simplified map
	    சாவிகள் = alt_சாவிகள்_buf;

	// check arguments and translate function keys
	எனில் (haskey)
	{
	    len = (முழு)STRLEN(சாவிகள்);
	    எனில் (len > MAXMAPLEN)	// maximum length of MAXMAPLEN chars
	    {
		retval = 1;
		செல் theend;
	    }

	    எனில் (abbrev && maptype != MAPTYPE_UNMAP)
	    {
		// If an abbreviation ends in a keyword character, the
		// rest must be all keyword-char or all non-keyword-char.
		// Otherwise we won't be able to find the start of it in a
		// vi-compatible way.
		எனில் (has_mbyte)
		{
		    முழு		first, last;
		    முழு		same = -1;
		    char_u	சாவிகள்_unescaped[MAXMAPLEN + 1];
		    size_t	சாவிகள்_unescaped_len;

		    mch_memmove(சாவிகள்_unescaped, சாவிகள், (size_t)(len + 1));
		    சாவிகள்_unescaped_len = vim_unescape_csi(சாவிகள்_unescaped);
		    p = சாவிகள்_unescaped;

		    first = vim_iswordp(p);
		    last = first;
		    MB_PTR_ADV(p);
		    n = 1;
		    போது (p < சாவிகள்_unescaped + சாவிகள்_unescaped_len)
		    {
			++n;			// nr of (multi-byte) chars
			last = vim_iswordp(p);	// type of last char
			எனில் (same == -1 && last != first)
			    same = n - 1;	// count of same char type
			MB_PTR_ADV(p);
		    }
		    எனில் (last && n > 2 && same >= 0 && same < n - 1)
		    {
			retval = 1;
			செல் theend;
		    }
		}
		இல்லையெனில் எனில் (vim_iswordc(சாவிகள்[len - 1]))
		    // ends in keyword char
		    க்கு (n = 0; n < len - 2; ++n)
			எனில் (vim_iswordc(சாவிகள்[n]) != vim_iswordc(சாவிகள்[len - 2]))
			{
			    retval = 1;
			    செல் theend;
			}
		// An abbreviation cannot contain white space.
		க்கு (n = 0; n < len; ++n)
		    எனில் (VIM_ISWHITE(சாவிகள்[n]))
		    {
			retval = 1;
			செல் theend;
		    }
	    }
	}

	எனில் (haskey && hasarg && abbrev)	// if we will add an abbreviation
	    no_abbr = FALSE;		// reset flag that indicates there are
					// no abbreviations

	எனில் (do_print)
	    செய்தி_start();

	// Check if a new local mapping wasn't already defined globally.
	எனில் (unique && map_table == curbuf->b_maphash
			       && haskey && hasarg && maptype != MAPTYPE_UNMAP)
	{
	    // need to loop over all global hash lists
	    க்கு (முழு hash = 0; hash < 256 && !got_int; ++hash)
	    {
		எனில் (abbrev)
		{
		    எனில் (hash != 0)	// there is only one abbreviation list
			நிறுத்து;
		    mp = first_abbr;
		}
		இல்லையெனில்
		    mp = maphash[hash];
		க்கு ( ; mp != வெற்று && !got_int; mp = mp->m_next)
		{
		    // check entries with the same mode
		    எனில் ((mp->m_mode & mode) != 0
			    && mp->m_keylen == len
			    && STRNCMP(mp->m_சாவிகள், சாவிகள், (size_t)len) == 0)
		    {
			எனில் (abbrev)
			    semsg(
			       _(e_global_abbreviation_already_exists_for_str),
				    mp->m_சாவிகள்);
			இல்லையெனில்
			    semsg(_(e_global_mapping_already_exists_for_str),
				    mp->m_சாவிகள்);
			retval = 5;
			செல் theend;
		    }
		}
	    }
	}

	// When listing global mappings, also list buffer-local ones here.
	எனில் (map_table != curbuf->b_maphash && !hasarg
						   && maptype != MAPTYPE_UNMAP)
	    list_mappings(keyround, abbrev, haskey, சாவிகள், len,
							     mode, &did_local);

	// Find an entry in the maphash[] list that matches.
	// For :unmap we may loop two times: once to try to unmap an entry with
	// a matching 'from' part, a second time, if the first fails, to unmap
	// an entry with a matching 'to' part. This was done to allow
	// ":ab foo bar" to be unmapped by typing ":unab foo", where "foo" will
	// be replaced by "bar" because of the abbreviation.
	இலக்கம்_rounds = maptype == MAPTYPE_UNMAP && !unmap_lhs_only ? 2 : 1;
	க்கு (round = 0; round < இலக்கம்_rounds && !did_it && !got_int; ++round)
	{
	    // need to loop over all hash lists
	    க்கு (முழு hash = 0; hash < 256 && !got_int; ++hash)
	    {
		எனில் (abbrev)
		{
		    எனில் (hash > 0)	// there is only one abbreviation list
			நிறுத்து;
		    mpp = abbr_table;
		}
		இல்லையெனில்
		    mpp = &(map_table[hash]);
		க்கு (mp = *mpp; mp != வெற்று && !got_int; mp = *mpp)
		{

		    எனில் ((mp->m_mode & mode) == 0)
		    {
			// skip entries with wrong mode
			mpp = &(mp->m_next);
			தொடர்;
		    }
		    எனில் (!haskey)	// show all entries
		    {
			எனில் (!mp->m_simplified)
			{
			    showmap(mp, map_table != maphash);
			    did_it = TRUE;
			}
		    }
		    இல்லையெனில்	// do we have a match?
		    {
			எனில் (round)	// second round: Try unmap "rhs" string
			{
			    n = (முழு)STRLEN(mp->m_str);
			    p = mp->m_str;
			}
			இல்லையெனில்
			{
			    n = mp->m_keylen;
			    p = mp->m_சாவிகள்;
			}
			எனில் (STRNCMP(p, சாவிகள், (size_t)(n < len ? n : len)) == 0)
			{
			    எனில் (maptype == MAPTYPE_UNMAP)
			    {
				// Delete entry.
				// Only accept a full match.  For abbreviations
				// we ignore trailing space when matching with
				// the "lhs", since an abbreviation can't have
				// trailing space.
				எனில் (n != len && (!abbrev || round || n > len
					       || *skipwhite(சாவிகள் + n) != NUL))
				{
				    mpp = &(mp->m_next);
				    தொடர்;
				}
				// In keyround for simplified keys, don't unmap
				// a mapping without m_simplified flag.
				எனில் (keyround1_simplified && !mp->m_simplified)
				    நிறுத்து;
				// We reset the indicated mode bits. If nothing
				// is left the entry is deleted below.
				mp->m_mode &= ~mode;
				did_it = TRUE;	// remember we did something
			    }
			    இல்லையெனில் எனில் (!hasarg)	// show matching entry
			    {
				எனில் (!mp->m_simplified)
				{
				    showmap(mp, map_table != maphash);
				    did_it = TRUE;
				}
			    }
			    இல்லையெனில் எனில் (n != len)	// new entry is ambiguous
			    {
				mpp = &(mp->m_next);
				தொடர்;
			    }
			    இல்லையெனில் எனில் (unique)
			    {
				எனில் (abbrev)
				    semsg(
				      _(e_abbreviation_already_exists_for_str),
					    p);
				இல்லையெனில்
				    semsg(_(e_mapping_already_exists_for_str),
					    p);
				retval = 5;
				செல் theend;
			    }
			    இல்லையெனில்
			    {
				// new rhs for existing entry
				mp->m_mode &= ~mode;	// remove mode bits
				எனில் (mp->m_mode == 0 && !did_it) // reuse entry
				{
				    char_u *newstr = vim_strsave(rhs);

				    எனில் (newstr == வெற்று)
				    {
					retval = 4;		// no mem
					செல் theend;
				    }
				    எனில் (mp->m_alt != வெற்று)
					mp->m_alt = mp->m_alt->m_alt = வெற்று;
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
				    mp->m_கோவை = கோவை;
				    mp->m_script_ctx = current_sctx;
				    எனில் (cmdmod.cmod_flags & CMOD_LEGACY)
					mp->m_script_ctx.sc_version = 1;
				    mp->m_script_ctx.sc_lnum += SOURCING_LNUM;
#endif
				    mp_result[keyround - 1] = mp;
				    did_it = TRUE;
				}
			    }
			    எனில் (mp->m_mode == 0)  // entry can be deleted
			    {
				map_free(mpp);
				தொடர்;	// continue with *mpp
			    }

			    // May need to put this entry into another hash
			    // list.
			    முழு new_hash = MAP_HASH(mp->m_mode, mp->m_சாவிகள்[0]);
			    எனில் (!abbrev && new_hash != hash)
			    {
				*mpp = mp->m_next;
				mp->m_next = map_table[new_hash];
				map_table[new_hash] = mp;

				தொடர்;	// continue with *mpp
			    }
			}
		    }
		    mpp = &(mp->m_next);
		}
	    }
	}

	எனில் (maptype == MAPTYPE_UNMAP)
	{
	    // delete entry
	    எனில் (!did_it)
	    {
		எனில் (!keyround1_simplified)
		    retval = 2;		// no match
	    }
	    இல்லையெனில் எனில் (*சாவிகள் == Ctrl_C)
	    {
		// If CTRL-C has been unmapped, reuse it for Interrupting.
		எனில் (map_table == curbuf->b_maphash)
		    curbuf->b_mapped_ctrl_c &= ~mode;
		இல்லையெனில்
		    mapped_ctrl_c &= ~mode;
	    }
	    தொடர்;
	}

	எனில் (!haskey || !hasarg)
	{
	    // print entries
	    எனில் (!did_it && !did_local)
	    {
		எனில் (abbrev)
		    செய்தி(_("No abbreviation found"));
		இல்லையெனில்
		    செய்தி(_("No mapping found"));
	    }
	    செல் theend;    // listing finished
	}

	எனில் (did_it)
	    தொடர்;	// have added the new entry already

	// Get here when adding a new entry to the maphash[] list or abbrlist.
	mp_result[keyround - 1] = map_add(map_table, abbr_table, சாவிகள்,
		    rhs, orig_rhs, noremap, nowait, silent, mode, abbrev,
#ifdef FEAT_EVAL
		    கோவை, /* sid */ 0, /* scriptversion */ 0, /* lnum */ 0,
#endif
		    keyround1_simplified);
	எனில் (mp_result[keyround - 1] == வெற்று)
	{
	    retval = 4;	    // no mem
	    செல் theend;
	}
    }

    எனில் (mp_result[0] != வெற்று && mp_result[1] != வெற்று)
    {
	mp_result[0]->m_alt = mp_result[1];
	mp_result[1]->m_alt = mp_result[0];
    }

theend:
    vim_free(சாவிகள்_buf);
    vim_free(alt_சாவிகள்_buf);
    vim_free(arg_buf);
    திரும்பு retval;
}

/*
 * Get the mapping mode from the command name.
 */
    நிலையான முழு
get_map_mode(char_u **cmdp, முழு forceit)
{
    char_u	*p;
    முழு		modec;
    முழு		mode;

    p = *cmdp;
    modec = *p++;
    எனில் (modec == 'i')
	mode = MODE_செருகு;				// :imap
    இல்லையெனில் எனில் (modec == 'l')
	mode = MODE_LANGMAP;				// :lmap
    இல்லையெனில் எனில் (modec == 'c')
	mode = MODE_CMDLINE;				// :cmap
    இல்லையெனில் எனில் (modec == 'n' && *p != 'o')		    // avoid :noremap
	mode = MODE_NORMAL;				// :nmap
    இல்லையெனில் எனில் (modec == 'v')
	mode = MODE_VISUAL | MODE_SELECT;		// :vmap
    இல்லையெனில் எனில் (modec == 'x')
	mode = MODE_VISUAL;				// :xmap
    இல்லையெனில் எனில் (modec == 's')
	mode = MODE_SELECT;			// :smap
    இல்லையெனில் எனில் (modec == 'o')
	mode = MODE_OP_PENDING;			// :omap
    இல்லையெனில் எனில் (modec == 't')
	mode = MODE_TERMINAL;			// :tmap
    இல்லையெனில்
    {
	--p;
	எனில் (forceit)
	    mode = MODE_செருகு | MODE_CMDLINE;		// :map !
	இல்லையெனில்
	    mode = MODE_VISUAL | MODE_SELECT | MODE_NORMAL | MODE_OP_PENDING;
							// :map
    }

    *cmdp = p;
    திரும்பு mode;
}

/*
 * Clear all mappings (":mapclear") or abbreviations (":abclear").
 * "abbr" should be FALSE for mappings, TRUE for abbreviations.
 */
    நிலையான வெறுமை
map_clear(
    char_u	*cmdp,
    char_u	*arg,
    முழு		forceit,
    முழு		abbr)
{
    முழு		mode;
    முழு		local;

    local = (STRCMP(arg, "<buffer>") == 0);
    எனில் (!local && *arg != NUL)
    {
	emsg(_(e_invalid_argument));
	திரும்பு;
    }

    mode = get_map_mode(&cmdp, forceit);
    map_clear_mode(curbuf, mode, local, abbr);
}

/*
 * If "map_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    நிலையான முழு
is_map_locked(வெறுமை)
{
    எனில் (map_locked > 0)
    {
	emsg(_(e_cannot_change_mappings_while_listing));
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Clear all mappings in "mode".
 */
    வெறுமை
map_clear_mode(
    buf_T	*buf,		// buffer for local mappings
    முழு		mode,		// mode in which to delete
    முழு		local,		// TRUE for buffer-local mappings
    முழு		abbr)		// TRUE for abbreviations
{
    mapblock_T	*mp, **mpp;
    முழு		hash;
    முழு		new_hash;

    எனில் (is_map_locked())
	திரும்பு;

    validate_maphash();

    க்கு (hash = 0; hash < 256; ++hash)
    {
	எனில் (abbr)
	{
	    எனில் (hash > 0)	// there is only one abbrlist
		நிறுத்து;
	    எனில் (local)
		mpp = &buf->b_first_abbr;
	    இல்லையெனில்
		mpp = &first_abbr;
	}
	இல்லையெனில்
	{
	    எனில் (local)
		mpp = &buf->b_maphash[hash];
	    இல்லையெனில்
		mpp = &maphash[hash];
	}
	போது (*mpp != வெற்று)
	{
	    mp = *mpp;
	    எனில் (mp->m_mode & mode)
	    {
		mp->m_mode &= ~mode;
		எனில் (mp->m_mode == 0) // entry can be deleted
		{
		    map_free(mpp);
		    தொடர்;
		}
		// May need to put this entry into another hash list.
		new_hash = MAP_HASH(mp->m_mode, mp->m_சாவிகள்[0]);
		எனில் (!abbr && new_hash != hash)
		{
		    *mpp = mp->m_next;
		    எனில் (local)
		    {
			mp->m_next = buf->b_maphash[new_hash];
			buf->b_maphash[new_hash] = mp;
		    }
		    இல்லையெனில்
		    {
			mp->m_next = maphash[new_hash];
			maphash[new_hash] = mp;
		    }
		    தொடர்;		// continue with *mpp
		}
	    }
	    mpp = &(mp->m_next);
	}
    }
}

#எனில் defined(FEAT_EVAL)
    முழு
mode_str2flags(char_u *modechars)
{
    முழு		mode = 0;

    எனில் (vim_strchr(modechars, 'n') != வெற்று)
	mode |= MODE_NORMAL;
    எனில் (vim_strchr(modechars, 'v') != வெற்று)
	mode |= MODE_VISUAL | MODE_SELECT;
    எனில் (vim_strchr(modechars, 'x') != வெற்று)
	mode |= MODE_VISUAL;
    எனில் (vim_strchr(modechars, 's') != வெற்று)
	mode |= MODE_SELECT;
    எனில் (vim_strchr(modechars, 'o') != வெற்று)
	mode |= MODE_OP_PENDING;
    எனில் (vim_strchr(modechars, 'i') != வெற்று)
	mode |= MODE_செருகு;
    எனில் (vim_strchr(modechars, 'l') != வெற்று)
	mode |= MODE_LANGMAP;
    எனில் (vim_strchr(modechars, 'c') != வெற்று)
	mode |= MODE_CMDLINE;

    திரும்பு mode;
}

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "modechars".
 * Recognize termcap codes in "str".
 * Also checks mappings local to the current buffer.
 */
    முழு
map_to_exists(char_u *str, char_u *modechars, முழு abbr)
{
    char_u	*rhs;
    char_u	*buf;
    முழு		retval;

    rhs = replace_termcodes(str, &buf, 0, REPTERM_DO_LT, வெற்று);

    retval = map_to_exists_mode(rhs, mode_str2flags(modechars), abbr);
    vim_free(buf);

    திரும்பு retval;
}
#endif

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "mode".
 * Also checks mappings local to the current buffer.
 */
    முழு
map_to_exists_mode(char_u *rhs, முழு mode, முழு abbr)
{
    mapblock_T	*mp;
    முழு		hash;
    முழு		exp_buffer = FALSE;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    க்கு (;;)
    {
	க்கு (hash = 0; hash < 256; ++hash)
	{
	    எனில் (abbr)
	    {
		எனில் (hash > 0)		// there is only one abbr list
		    நிறுத்து;
		எனில் (exp_buffer)
		    mp = curbuf->b_first_abbr;
		இல்லையெனில்
		    mp = first_abbr;
	    }
	    இல்லையெனில் எனில் (exp_buffer)
		mp = curbuf->b_maphash[hash];
	    இல்லையெனில்
		mp = maphash[hash];
	    க்கு (; mp; mp = mp->m_next)
	    {
		எனில் ((mp->m_mode & mode)
			&& strstr((எழுத்து *)mp->m_str, (எழுத்து *)rhs) != வெற்று)
		    திரும்பு TRUE;
	    }
	}
	எனில் (exp_buffer)
	    நிறுத்து;
	exp_buffer = TRUE;
    }

    திரும்பு FALSE;
}

/*
 * Used below when expanding mapping/abbreviation names.
 */
நிலையான முழு	expand_mapmodes = 0;
நிலையான முழு	expand_isabbrev = 0;
நிலையான முழு	expand_buffer = FALSE;

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
    நிலையான char_u *
translate_mapping(char_u *str)
{
    garray_T	ga;
    முழு		c;
    முழு		modifiers;
    முழு		cpo_bslash;
    முழு		cpo_special;

    ga_init(&ga);
    ga.ga_itemsize = 1;
    ga.ga_growsize = 40;

    cpo_bslash = (vim_strchr(p_cpo, CPO_BSLASH) != வெற்று);
    cpo_special = (vim_strchr(p_cpo, CPO_SPECI) != வெற்று);

    க்கு (; *str; ++str)
    {
	c = *str;
	எனில் (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	{
	    modifiers = 0;
	    எனில் (str[1] == KS_MODIFIER)
	    {
		str++;
		modifiers = *++str;
		c = *++str;
	    }
	    எனில் (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	    {
		எனில் (cpo_special)
		{
		    ga_clear(&ga);
		    திரும்பு வெற்று;
		}
		c = TO_SPECIAL(str[1], str[2]);
		எனில் (c == K_ZERO)	// display <Nul> as ^@
		    c = NUL;
		str += 2;
	    }
	    எனில் (IS_SPECIAL(c) || modifiers)	// special key
	    {
		எனில் (cpo_special)
		{
		    ga_clear(&ga);
		    திரும்பு வெற்று;
		}
		ga_concat(&ga, get_special_சாவி_name(c, modifiers));
		தொடர்; // for (str)
	    }
	}
	எனில் (c == ' ' || c == '\t' || c == Ctrl_J || c == Ctrl_V
	    || (c == '<' && !cpo_special) || (c == '\\' && !cpo_bslash))
	    ga_append(&ga, cpo_bslash ? Ctrl_V : '\\');
	எனில் (c)
	    ga_append(&ga, c);
    }
    ga_append(&ga, NUL);
    திரும்பு (char_u *)(ga.ga_data);
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
    முழு		forceit,	// TRUE if '!' given
    முழு		isabbrev,	// TRUE if abbreviation
    முழு		isunmap,	// TRUE if unmap/unabbrev command
    cmdidx_T	cmdidx)
{
    எனில் (forceit && cmdidx != CMD_map && cmdidx != CMD_unmap)
	xp->xp_context = EXPAND_NOTHING;
    இல்லையெனில்
    {
	எனில் (isunmap)
	    expand_mapmodes = get_map_mode(&cmd, forceit || isabbrev);
	இல்லையெனில்
	{
	    expand_mapmodes = MODE_செருகு | MODE_CMDLINE;
	    எனில் (!isabbrev)
		expand_mapmodes += MODE_VISUAL | MODE_SELECT | MODE_NORMAL
							     | MODE_OP_PENDING;
	}
	expand_isabbrev = isabbrev;
	xp->xp_context = EXPAND_MAPPINGS;
	expand_buffer = FALSE;
	க்கு (;;)
	{
	    எனில் (STRNCMP(arg, "<buffer>", 8) == 0)
	    {
		expand_buffer = TRUE;
		arg = skipwhite(arg + 8);
		தொடர்;
	    }
	    எனில் (STRNCMP(arg, "<unique>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		தொடர்;
	    }
	    எனில் (STRNCMP(arg, "<nowait>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		தொடர்;
	    }
	    எனில் (STRNCMP(arg, "<silent>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		தொடர்;
	    }
	    எனில் (STRNCMP(arg, "<special>", 9) == 0)
	    {
		arg = skipwhite(arg + 9);
		தொடர்;
	    }
#ifdef FEAT_EVAL
	    எனில் (STRNCMP(arg, "<script>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		தொடர்;
	    }
	    எனில் (STRNCMP(arg, "<expr>", 6) == 0)
	    {
		arg = skipwhite(arg + 6);
		தொடர்;
	    }
#endif
	    நிறுத்து;
	}
	xp->xp_pattern = arg;
    }

    திரும்பு வெற்று;
}

/*
 * Find all mapping/abbreviation names that match regexp "regmatch"'.
 * For command line expansion of ":[un]map" and ":[un]abbrev" in all modes.
 * Return OK if matches found, FAIL otherwise.
 */
    முழு
ExpandMappings(
    char_u	*pat,
    regmatch_T	*regmatch,
    முழு		*இலக்கம்_Matches,
    char_u	***matches)
{
    mapblock_T	*mp;
    garray_T	ga;
    முழு		hash;
    முழு		மொத்தம்;
    char_u	*p;
    முழு		i;
    முழு		fuzzy;
    முழு		match;
    முழு		score = 0;
    fuzmatch_str_T  *fuzmatch;

    fuzzy = cmdline_fuzzy_complete(pat);

    validate_maphash();

    *இலக்கம்_Matches = 0;		    // return values in case of FAIL
    *matches = வெற்று;

    எனில் (!fuzzy)
	ga_init2(&ga, அளவு(எழுத்து *), 3);
    இல்லையெனில்
	ga_init2(&ga, அளவு(fuzmatch_str_T), 3);

    // First search in map modifier arguments
    க்கு (i = 0; i < 7; ++i)
    {
	எனில் (i == 0)
	    p = (char_u *)"<silent>";
	இல்லையெனில் எனில் (i == 1)
	    p = (char_u *)"<unique>";
#ifdef FEAT_EVAL
	இல்லையெனில் எனில் (i == 2)
	    p = (char_u *)"<script>";
	இல்லையெனில் எனில் (i == 3)
	    p = (char_u *)"<expr>";
#endif
	இல்லையெனில் எனில் (i == 4 && !expand_buffer)
	    p = (char_u *)"<buffer>";
	இல்லையெனில் எனில் (i == 5)
	    p = (char_u *)"<nowait>";
	இல்லையெனில் எனில் (i == 6)
	    p = (char_u *)"<special>";
	இல்லையெனில்
	    தொடர்;

	எனில் (!fuzzy)
	    match = vim_regexec(regmatch, p, (colnr_T)0);
	இல்லையெனில்
	{
	    score = fuzzy_match_str(p, pat);
	    match = (score != FUZZY_SCORE_NONE);
	}

	எனில் (!match)
	    தொடர்;

	எனில் (ga_grow(&ga, 1) == FAIL)
	    நிறுத்து;

	எனில் (fuzzy)
	{
	    fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
	    fuzmatch->idx = ga.ga_len;
	    fuzmatch->str = vim_strsave(p);
	    fuzmatch->score = score;
	}
	இல்லையெனில்
	    ((char_u **)ga.ga_data)[ga.ga_len] = vim_strsave(p);
	++ga.ga_len;
    }

    க்கு (hash = 0; hash < 256; ++hash)
    {
	எனில் (expand_isabbrev)
	{
	    எனில் (hash > 0)	// only one abbrev list
		நிறுத்து; // for (hash)
	    mp = first_abbr;
	}
	இல்லையெனில் எனில் (expand_buffer)
	    mp = curbuf->b_maphash[hash];
	இல்லையெனில்
	    mp = maphash[hash];
	க்கு (; mp; mp = mp->m_next)
	{
	    எனில் (mp->m_simplified || !(mp->m_mode & expand_mapmodes))
		தொடர்;

	    p = translate_mapping(mp->m_சாவிகள்);
	    எனில் (p == வெற்று)
		தொடர்;

	    எனில் (!fuzzy)
		match = vim_regexec(regmatch, p, (colnr_T)0);
	    இல்லையெனில்
	    {
		score = fuzzy_match_str(p, pat);
		match = (score != FUZZY_SCORE_NONE);
	    }

	    எனில் (!match)
	    {
		vim_free(p);
		தொடர்;
	    }

	    எனில் (ga_grow(&ga, 1) == FAIL)
	    {
		vim_free(p);
		நிறுத்து;
	    }

	    எனில் (fuzzy)
	    {
		fuzmatch = &((fuzmatch_str_T *)ga.ga_data)[ga.ga_len];
		fuzmatch->idx = ga.ga_len;
		fuzmatch->str = p;
		fuzmatch->score = score;
	    }
	    இல்லையெனில்
		((char_u **)ga.ga_data)[ga.ga_len] = p;

	    ++ga.ga_len;
	} // for (mp)
    } // for (hash)

    எனில் (ga.ga_len == 0)
	திரும்பு FAIL;

    எனில் (!fuzzy)
    {
	*matches = ga.ga_data;
	*இலக்கம்_Matches = ga.ga_len;
    }
    இல்லையெனில்
    {
	எனில் (fuzzymatches_to_strmatches(ga.ga_data, matches, ga.ga_len,
							FALSE) == FAIL)
	    திரும்பு FAIL;
	*இலக்கம்_Matches = ga.ga_len;
    }

    மொத்தம் = *இலக்கம்_Matches;
    எனில் (மொத்தம் > 1)
    {
	char_u	**ptr1;
	char_u	**ptr2;
	char_u	**ptr3;

	// Sort the matches
	// Fuzzy matching already sorts the matches
	எனில் (!fuzzy)
	    sort_strings(*matches, மொத்தம்);

	// Remove multiple entries
	ptr1 = *matches;
	ptr2 = ptr1 + 1;
	ptr3 = ptr1 + மொத்தம்;

	போது (ptr2 < ptr3)
	{
	    எனில் (STRCMP(*ptr1, *ptr2) != 0)
		*++ptr1 = *ptr2++;
	    இல்லையெனில்
	    {
		vim_free(*ptr2++);
		மொத்தம்--;
	    }
	}
    }

    *இலக்கம்_Matches = மொத்தம்;
    திரும்பு (மொத்தம் == 0 ? FAIL : OK);
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
    முழு
check_abbr(
    முழு		c,
    char_u	*ptr,
    முழு		col,
    முழு		mincol)
{
    முழு		len;
    முழு		scol;		// starting column of the abbr.
    முழு		j;
    char_u	*s;
    char_u	tb[MB_MAXBYTES + 4];
    mapblock_T	*mp;
    mapblock_T	*mp2;
    முழு		clen = 0;	// length in characters
    முழு		is_id = TRUE;
    முழு		vim_abbr;

    எனில் (typebuf.tb_no_abbr_cnt)	// abbrev. are not recursive
	திரும்பு FALSE;

    // no remapping implies no abbreviation, except for CTRL-]
    எனில் (noremap_சாவிகள்() && c != Ctrl_RSB)
	திரும்பு FALSE;

    // Check for word before the cursor: If it ends in a keyword char all
    // chars before it must be keyword chars or non-keyword chars, but not
    // white space. If it ends in a non-keyword char we accept any characters
    // before it except white space.
    எனில் (col == 0)				// cannot be an abbr.
	திரும்பு FALSE;

    எனில் (has_mbyte)
    {
	char_u *p;

	p = mb_prevptr(ptr, ptr + col);
	எனில் (!vim_iswordp(p))
	    vim_abbr = TRUE;			// Vim added abbr.
	இல்லையெனில்
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    எனில் (p > ptr)
		is_id = vim_iswordp(mb_prevptr(ptr, p));
	}
	clen = 1;
	போது (p > ptr + mincol)
	{
	    p = mb_prevptr(ptr, p);
	    எனில் (vim_isspace(*p) || (!vim_abbr && is_id != vim_iswordp(p)))
	    {
		p += (*mb_ptr2len)(p);
		நிறுத்து;
	    }
	    ++clen;
	}
	scol = (முழு)(p - ptr);
    }
    இல்லையெனில்
    {
	எனில் (!vim_iswordc(ptr[col - 1]))
	    vim_abbr = TRUE;			// Vim added abbr.
	இல்லையெனில்
	{
	    vim_abbr = FALSE;			// vi compatible abbr.
	    எனில் (col > 1)
		is_id = vim_iswordc(ptr[col - 2]);
	}
	க்கு (scol = col - 1; scol > 0 && !vim_isspace(ptr[scol - 1])
		&& (vim_abbr || is_id == vim_iswordc(ptr[scol - 1])); --scol)
	    ;
    }

    எனில் (scol < mincol)
	scol = mincol;
    எனில் (scol < col)		// there is a word in front of the cursor
    {
	ptr += scol;
	len = col - scol;
	mp = curbuf->b_first_abbr;
	mp2 = first_abbr;
	எனில் (mp == வெற்று)
	{
	    mp = mp2;
	    mp2 = வெற்று;
	}
	க்கு ( ; mp; mp->m_next == வெற்று
				  ? (mp = mp2, mp2 = வெற்று) : (mp = mp->m_next))
	{
	    முழு		qlen = mp->m_keylen;
	    char_u	*q = mp->m_சாவிகள்;
	    முழு		match;

	    எனில் (vim_strbyte(mp->m_சாவிகள், K_SPECIAL) != வெற்று)
	    {
		char_u *qe = vim_strsave(mp->m_சாவிகள்);

		// might have CSI escaped mp->m_keys
		எனில் (qe != வெற்று)
		{
		    q = qe;
		    qlen = (முழு)vim_unescape_csi(q);
		}
	    }

	    // find entries with right mode and keys
	    match =    (mp->m_mode & State)
		    && qlen == len
		    && STRNCMP(q, ptr, (size_t)len) == 0;
	    எனில் (q != mp->m_சாவிகள்)
		vim_free(q);
	    எனில் (match)
		நிறுத்து;
	}
	எனில் (mp != வெற்று)
	{
	    முழு	noremap;
	    முழு silent;
#ifdef FEAT_EVAL
	    முழு கோவை;
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
	    எனில் (c != Ctrl_RSB)
	    {
					// special key code, split up
		எனில் (IS_SPECIAL(c) || c == K_SPECIAL)
		{
		    tb[j++] = K_SPECIAL;
		    tb[j++] = K_SECOND(c);
		    tb[j++] = K_THIRD(c);
		}
		இல்லையெனில்
		{
		    எனில் (c < ABBR_OFF && (c < ' ' || c > '~'))
			tb[j++] = Ctrl_V;	// special char needs CTRL-V
		    எனில் (has_mbyte)
		    {
			முழு	newlen;
			char_u	*escaped;

			// if ABBR_OFF has been added, remove it here
			எனில் (c >= ABBR_OFF)
			    c -= ABBR_OFF;
			newlen = (*mb_char2bytes)(c, tb + j);
			tb[j + newlen] = NUL;
			// Need to escape K_SPECIAL.
			escaped = vim_strsave_escape_csi(tb + j);
			எனில் (escaped != வெற்று)
			{
			    newlen = (முழு)STRLEN(escaped);
			    mch_memmove(tb + j, escaped, newlen);
			    j += newlen;
			    vim_free(escaped);
			}
		    }
		    இல்லையெனில்
			tb[j++] = c;
		}
		tb[j] = NUL;
					// insert the last typed char
		(வெறுமை)ins_typebuf(tb, 1, 0, TRUE, mp->m_silent);
	    }

	    // copy values here, calling eval_map_expr() may make "mp" invalid!
	    noremap = mp->m_noremap;
	    silent = mp->m_silent;
#ifdef FEAT_EVAL
	    கோவை = mp->m_கோவை;

	    எனில் (கோவை)
		s = eval_map_கோவை(mp, c);
	    இல்லையெனில்
#endif
		s = mp->m_str;
	    எனில் (s != வெற்று)
	    {
					// insert the to string
		(வெறுமை)ins_typebuf(s, noremap, 0, TRUE, silent);
					// no abbrev. for these chars
		typebuf.tb_no_abbr_cnt += (முழு)STRLEN(s) + j + 1;
#ifdef FEAT_EVAL
		எனில் (கோவை)
		    vim_free(s);
#endif
	    }

	    tb[0] = Ctrl_H;
	    tb[1] = NUL;
	    எனில் (has_mbyte)
		len = clen;	// Delete characters instead of bytes
	    போது (len-- > 0)		// delete the from string
		(வெறுமை)ins_typebuf(tb, 1, 0, TRUE, silent);
	    திரும்பு TRUE;
	}
    }
    திரும்பு FALSE;
}

#ifdef FEAT_EVAL
/*
 * Evaluate the RHS of a mapping or abbreviations and take care of escaping
 * special characters.
 * Careful: after this "mp" will be invalid if the mapping was deleted.
 */
    char_u *
eval_map_கோவை(
    mapblock_T	*mp,
    முழு		c)	    // NUL or typed character for abbreviation
{
    char_u	*res;
    char_u	*p;
    char_u	*கோவை;
    pos_T	save_cursor;
    முழு		save_செய்தி_col;
    முழு		save_செய்தி_row;
    scid_T	save_sctx_sid = current_sctx.sc_sid;
    முழு		save_sctx_version = current_sctx.sc_version;

    // Remove escaping of CSI, because "str" is in a format to be used as
    // typeahead.
    கோவை = vim_strsave(mp->m_str);
    எனில் (கோவை == வெற்று)
	திரும்பு வெற்று;
    vim_unescape_csi(கோவை);

    // Forbid changing text or using ":normal" to avoid most of the bad side
    // effects.  Also restore the cursor position.
    ++textlock;
    ++ex_normal_lock;
    set_vim_var_char(c);  // set v:char to the typed character
    save_cursor = curwin->w_cursor;
    save_செய்தி_col = செய்தி_col;
    save_செய்தி_row = செய்தி_row;

    current_sctx.sc_version = mp->m_script_ctx.sc_version;
    எனில் (mp->m_script_ctx.sc_version == SCRIPT_VERSION_VIM9)
	current_sctx.sc_sid = mp->m_script_ctx.sc_sid;

    // Note: the evaluation may make "mp" invalid.
    p = eval_to_string(கோவை, FALSE, FALSE);

    --textlock;
    --ex_normal_lock;
    curwin->w_cursor = save_cursor;
    செய்தி_col = save_செய்தி_col;
    செய்தி_row = save_செய்தி_row;
    current_sctx.sc_sid = save_sctx_sid;
    current_sctx.sc_version = save_sctx_version;

    vim_free(கோவை);

    எனில் (p == வெற்று)
	திரும்பு வெற்று;
    // Escape CSI in the result to be able to use the string as typeahead.
    res = vim_strsave_escape_csi(p);
    vim_free(p);

    திரும்பு res;
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
    எனில் (res == வெற்று)
	திரும்பு வெற்று;

    d = res;
    க்கு (s = p; *s != NUL; )
    {
	எனில் ((s[0] == K_SPECIAL
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
	இல்லையெனில்
	{
	    // Add character, possibly multi-byte to destination, escaping
	    // CSI and K_SPECIAL. Be careful, it can be an illegal byte!
	    d = add_char2buf(PTR2CHAR(s), d);
	    s += MB_CPTR2LEN(s);
	}
    }
    *d = NUL;

    திரும்பு res;
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

    போது (*s != NUL)
    {
	எனில் (s[0] == K_SPECIAL && s[1] == KS_SPECIAL && s[2] == KE_FILLER)
	{
	    *d++ = K_SPECIAL;
	    s += 3;
	}
	இல்லையெனில் எனில் ((s[0] == K_SPECIAL || s[0] == CSI)
				   && s[1] == KS_EXTRA && s[2] == (முழு)KE_CSI)
	{
	    *d++ = CSI;
	    s += 3;
	}
	இல்லையெனில்
	    *d++ = *s++;
    }
    *d = NUL;
    திரும்பு (size_t)(d - p);
}

/*
 * Write map commands for the current mappings to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    முழு
makemap(
    FILE	*fd,
    buf_T	*buf)	    // buffer for local mappings or NULL
{
    mapblock_T	*mp;
    char_u	c1, c2, c3;
    char_u	*p;
    எழுத்து	*cmd;
    முழு		abbr;
    முழு		hash;
    முழு		did_cpo = FALSE;
    முழு		i;

    validate_maphash();

    // Do the loop twice: Once for mappings, once for abbreviations.
    // Then loop over all map hash lists.
    க்கு (abbr = 0; abbr < 2; ++abbr)
	க்கு (hash = 0; hash < 256; ++hash)
	{
	    எனில் (abbr)
	    {
		எனில் (hash > 0)		// there is only one abbr list
		    நிறுத்து;
		எனில் (buf != வெற்று)
		    mp = buf->b_first_abbr;
		இல்லையெனில்
		    mp = first_abbr;
	    }
	    இல்லையெனில்
	    {
		எனில் (buf != வெற்று)
		    mp = buf->b_maphash[hash];
		இல்லையெனில்
		    mp = maphash[hash];
	    }

	    க்கு ( ; mp; mp = mp->m_next)
	    {
		// simplified map blocks are not created explicitly
		எனில் (mp->m_simplified)
		    தொடர்;

		// skip script-local mappings
		எனில் (mp->m_noremap == REMAP_SCRIPT)
		    தொடர்;

		// skip mappings that contain a <SNR> (script-local thing),
		// they probably don't work when loaded again
		க்கு (p = mp->m_str; *p != NUL; ++p)
		    எனில் (p[0] == K_SPECIAL && p[1] == KS_EXTRA
						       && p[2] == (முழு)KE_SNR)
			நிறுத்து;
		எனில் (*p != NUL)
		    தொடர்;

		// It's possible to create a mapping and then ":unmap" certain
		// modes.  We recreate this here by mapping the individual
		// modes, which requires up to three of them.
		c1 = NUL;
		c2 = NUL;
		c3 = NUL;
		எனில் (abbr)
		    cmd = "abbr";
		இல்லையெனில்
		    cmd = "map";
		தேர்வு (mp->m_mode)
		{
		    நிலை MODE_NORMAL | MODE_VISUAL | MODE_SELECT
							     | MODE_OP_PENDING:
			நிறுத்து;
		    நிலை MODE_NORMAL:
			c1 = 'n';
			நிறுத்து;
		    நிலை MODE_VISUAL:
			c1 = 'x';
			நிறுத்து;
		    நிலை MODE_SELECT:
			c1 = 's';
			நிறுத்து;
		    நிலை MODE_OP_PENDING:
			c1 = 'o';
			நிறுத்து;
		    நிலை MODE_NORMAL | MODE_VISUAL:
			c1 = 'n';
			c2 = 'x';
			நிறுத்து;
		    நிலை MODE_NORMAL | MODE_SELECT:
			c1 = 'n';
			c2 = 's';
			நிறுத்து;
		    நிலை MODE_NORMAL | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 'o';
			நிறுத்து;
		    நிலை MODE_VISUAL | MODE_SELECT:
			c1 = 'v';
			நிறுத்து;
		    நிலை MODE_VISUAL | MODE_OP_PENDING:
			c1 = 'x';
			c2 = 'o';
			நிறுத்து;
		    நிலை MODE_SELECT | MODE_OP_PENDING:
			c1 = 's';
			c2 = 'o';
			நிறுத்து;
		    நிலை MODE_NORMAL | MODE_VISUAL | MODE_SELECT:
			c1 = 'n';
			c2 = 'v';
			நிறுத்து;
		    நிலை MODE_NORMAL | MODE_VISUAL | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 'x';
			c3 = 'o';
			நிறுத்து;
		    நிலை MODE_NORMAL | MODE_SELECT | MODE_OP_PENDING:
			c1 = 'n';
			c2 = 's';
			c3 = 'o';
			நிறுத்து;
		    நிலை MODE_VISUAL | MODE_SELECT | MODE_OP_PENDING:
			c1 = 'v';
			c2 = 'o';
			நிறுத்து;
		    நிலை MODE_CMDLINE | MODE_செருகு:
			எனில் (!abbr)
			    cmd = "map!";
			நிறுத்து;
		    நிலை MODE_CMDLINE:
			c1 = 'c';
			நிறுத்து;
		    நிலை MODE_செருகு:
			c1 = 'i';
			நிறுத்து;
		    நிலை MODE_LANGMAP:
			c1 = 'l';
			நிறுத்து;
		    நிலை MODE_TERMINAL:
			c1 = 't';
			நிறுத்து;
		    இயல்பு:
			iemsg(e_makemap_illegal_mode);
			திரும்பு FAIL;
		}
		செய்	// do this twice if c2 is set, 3 times with c3
		{
		    // When outputting <> form, need to make sure that 'cpo'
		    // is set to the Vim default.
		    எனில் (!did_cpo)
		    {
			எனில் (*mp->m_str == NUL)		// will use <Nop>
			    did_cpo = TRUE;
			இல்லையெனில்
			    க்கு (i = 0; i < 2; ++i)
				க்கு (p = (i ? mp->m_str : mp->m_சாவிகள்); *p; ++p)
				    எனில் (*p == K_SPECIAL || *p == NL)
					did_cpo = TRUE;
			எனில் (did_cpo)
			{
			    எனில் (fprintf(fd, "cpo_save = &cpo") < 0
				    || put_eol(fd) < 0
				    || fprintf(fd, "set cpo&vim") < 0
				    || put_eol(fd) < 0)
				திரும்பு FAIL;
			}
		    }
#ifdef FEAT_EVAL
		    // If it is not vim9 use legacy
		    எனில் (mp->m_கோவை && mp->m_script_ctx.sc_version < SCRIPT_VERSION_VIM9
			    && fputs("legacy ", fd) < 0)
			திரும்பு FAIL;
#endif
		    எனில் (c1 && putc(c1, fd) < 0)
			திரும்பு FAIL;
		    எனில் (mp->m_noremap != REMAP_YES && fprintf(fd, "nore") < 0)
			திரும்பு FAIL;
		    எனில் (fputs(cmd, fd) < 0)
			திரும்பு FAIL;
		    எனில் (buf != வெற்று && fputs(" <buffer>", fd) < 0)
			திரும்பு FAIL;
		    எனில் (mp->m_nowait && fputs(" <nowait>", fd) < 0)
			திரும்பு FAIL;
		    எனில் (mp->m_silent && fputs(" <silent>", fd) < 0)
			திரும்பு FAIL;
#ifdef FEAT_EVAL
		    எனில் (mp->m_noremap == REMAP_SCRIPT
						 && fputs("<script>", fd) < 0)
			திரும்பு FAIL;
		    எனில் (mp->m_கோவை && fputs(" <expr>", fd) < 0)
			திரும்பு FAIL;
#endif

		    எனில் (       putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_சாவிகள், 0) == FAIL
			    || putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_str, 1) == FAIL
			    || put_eol(fd) < 0)
			திரும்பு FAIL;
		    c1 = c2;
		    c2 = c3;
		    c3 = NUL;
		} போது (c1 != NUL);
	    }
	}

    எனில் (did_cpo)
	எனில் (fprintf(fd, "&cpo = cpo_save") < 0
		|| put_eol(fd) < 0)
	    திரும்பு FAIL;
    திரும்பு OK;
}

/*
 * write escape string to file
 * "what": 0 for :map lhs, 1 for :map rhs, 2 for :set
 *
 * return FAIL for failure, OK otherwise
 */
    முழு
put_escstr(FILE *fd, char_u *strstart, முழு what)
{
    char_u	*str = strstart;
    முழு		c;
    முழு		modifiers;

    // :map xx <Nop>
    எனில் (*str == NUL && what == 1)
    {
	எனில் (fprintf(fd, "<Nop>") < 0)
	    திரும்பு FAIL;
	திரும்பு OK;
    }

    க்கு ( ; *str != NUL; ++str)
    {
	char_u	*p;

	// Check for a multi-byte character, which may contain escaped
	// K_SPECIAL and CSI bytes
	p = mb_unescape(&str);
	எனில் (p != வெற்று)
	{
	    போது (*p != NUL)
		எனில் (fputc(*p++, fd) < 0)
		    திரும்பு FAIL;
	    --str;
	    தொடர்;
	}

	c = *str;
	// Special key codes have to be translated to be able to make sense
	// when they are read back.
	எனில் (c == K_SPECIAL && what != 2)
	{
	    modifiers = 0;
	    எனில் (str[1] == KS_MODIFIER)
	    {
		modifiers = str[2];
		str += 3;

		// Modifiers can be applied too to multi-byte characters.
		p = mb_unescape(&str);

		எனில் (p == வெற்று)
		    c = *str;
		இல்லையெனில்
		{
		    // retrieve codepoint (character number) from unescaped string
		    c = (*mb_ptr2char)(p);
		    --str;
		}
	    }
	    எனில் (c == K_SPECIAL)
	    {
		c = TO_SPECIAL(str[1], str[2]);
		str += 2;
	    }
	    எனில் (IS_SPECIAL(c) || modifiers)	// special key
	    {
		எனில் (fputs((எழுத்து *)get_special_சாவி_name(c, modifiers), fd) < 0)
		    திரும்பு FAIL;
		தொடர்;
	    }
	}

	// A '\n' in a map command should be written as <NL>.
	// A '\n' in a set command should be written as \^V^J.
	எனில் (c == NL)
	{
	    எனில் (what == 2)
	    {
		எனில் (fprintf(fd, "\\\026\n") < 0)
		    திரும்பு FAIL;
	    }
	    இல்லையெனில்
	    {
		எனில் (fprintf(fd, "<NL>") < 0)
		    திரும்பு FAIL;
	    }
	    தொடர்;
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
	எனில் (what == 2 && (VIM_ISWHITE(c) || c == '"' || c == '\\'))
	{
	    எனில் (putc('\\', fd) < 0)
		திரும்பு FAIL;
	}
	இல்லையெனில் எனில் (c < ' ' || c > '~' || c == '|'
		|| (what == 0 && c == ' ')
		|| (what == 1 && str == strstart && c == ' ')
		|| (what != 2 && c == '<'))
	{
	    எனில் (putc(Ctrl_V, fd) < 0)
		திரும்பு FAIL;
	}
	எனில் (putc(c, fd) < 0)
	    திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Check all mappings for the presence of special key codes.
 * Used after ":set term=xxx".
 */
    வெறுமை
check_map_keycodes(வெறுமை)
{
    mapblock_T	*mp;
    char_u	*p;
    முழு		i;
    char_u	buf[3];
    முழு		abbr;
    முழு		hash;
    buf_T	*bp;
    ESTACK_CHECK_DECLARATION;

    validate_maphash();
    // avoids giving error messages
    estack_தள்ளு(ETYPE_INTERNAL, (char_u *)"mappings", 0);
    ESTACK_CHECK_SETUP;

    // Do this once for each buffer, and then once for global
    // mappings/abbreviations with bp == NULL
    க்கு (bp = firstbuf; ; bp = bp->b_next)
    {
	// Do the loop twice: Once for mappings, once for abbreviations.
	// Then loop over all map hash lists.
	க்கு (abbr = 0; abbr <= 1; ++abbr)
	    க்கு (hash = 0; hash < 256; ++hash)
	    {
		எனில் (abbr)
		{
		    எனில் (hash)	    // there is only one abbr list
			நிறுத்து;
		    எனில் (bp != வெற்று)
			mp = bp->b_first_abbr;
		    இல்லையெனில்
			mp = first_abbr;
		}
		இல்லையெனில்
		{
		    எனில் (bp != வெற்று)
			mp = bp->b_maphash[hash];
		    இல்லையெனில்
			mp = maphash[hash];
		}
		க்கு ( ; mp != வெற்று; mp = mp->m_next)
		{
		    க்கு (i = 0; i <= 1; ++i)	// do this twice
		    {
			எனில் (i == 0)
			    p = mp->m_சாவிகள்;	// once for the "from" part
			இல்லையெனில்
			    p = mp->m_str;	// and once for the "to" part
			போது (*p)
			{
			    எனில் (*p == K_SPECIAL)
			    {
				++p;
				எனில் (*p < 128)   // for "normal" tcap entries
				{
				    buf[0] = p[0];
				    buf[1] = p[1];
				    buf[2] = NUL;
				    (வெறுமை)add_termcap_entry(buf, FALSE);
				}
				++p;
			    }
			    ++p;
			}
		    }
		}
	    }
	எனில் (bp == வெற்று)
	    நிறுத்து;
    }
    ESTACK_CHECK_NOW;
    estack_எடு();
}

#எனில் defined(FEAT_EVAL)
/*
 * Check the string "keys" against the lhs of all mappings.
 * Return pointer to rhs of mapping (mapblock->m_str).
 * NULL when no mapping found.
 */
    char_u *
check_map(
    char_u	*சாவிகள்,
    முழு		mode,
    முழு		exact,		// require exact match
    முழு		ign_mod,	// ignore preceding modifier
    முழு		abbr,		// do abbreviations
    mapblock_T	**mp_ptr,	// return: pointer to mapblock or NULL
    முழு		*local_ptr)	// return: buffer-local mapping or NULL
{
    முழு		hash;
    முழு		len, minlen;
    mapblock_T	*mp;
    char_u	*s;
    முழு		local;

    validate_maphash();

    len = (முழு)STRLEN(சாவிகள்);
    க்கு (local = 1; local >= 0; --local)
	// loop over all hash lists
	க்கு (hash = 0; hash < 256; ++hash)
	{
	    எனில் (abbr)
	    {
		எனில் (hash > 0)		// there is only one list.
		    நிறுத்து;
		எனில் (local)
		    mp = curbuf->b_first_abbr;
		இல்லையெனில்
		    mp = first_abbr;
	    }
	    இல்லையெனில் எனில் (local)
		mp = curbuf->b_maphash[hash];
	    இல்லையெனில்
		mp = maphash[hash];
	    க்கு ( ; mp != வெற்று; mp = mp->m_next)
	    {
		// skip entries with wrong mode, wrong length and not matching
		// ones
		எனில் ((mp->m_mode & mode) && (!exact || mp->m_keylen == len))
		{
		    எனில் (len > mp->m_keylen)
			minlen = mp->m_keylen;
		    இல்லையெனில்
			minlen = len;
		    s = mp->m_சாவிகள்;
		    எனில் (ign_mod && s[0] == K_SPECIAL && s[1] == KS_MODIFIER
							       && s[2] != NUL)
		    {
			s += 3;
			எனில் (len > mp->m_keylen - 3)
			    minlen = mp->m_keylen - 3;
		    }
		    எனில் (STRNCMP(s, சாவிகள், minlen) == 0)
		    {
			எனில் (mp_ptr != வெற்று)
			    *mp_ptr = mp;
			எனில் (local_ptr != வெற்று)
			    *local_ptr = local;
			திரும்பு mp->m_str;
		    }
		}
	    }
	}

    திரும்பு வெற்று;
}

/*
 * "hasmapto()" function
 */
    வெறுமை
f_hasmapto(typval_T *argvars, typval_T *rettv)
{
    char_u	*name;
    char_u	*mode;
    char_u	buf[NUMBUFLEN];
    முழு		abbr = FALSE;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	திரும்பு;

    name = tv_get_string(&argvars[0]);
    எனில் (argvars[1].v_type == VAR_UNKNOWN)
	mode = (char_u *)"nvo";
    இல்லையெனில்
    {
	mode = tv_get_string_buf(&argvars[1], buf);
	எனில் (argvars[2].v_type != VAR_UNKNOWN)
	    abbr = (முழு)tv_get_bool(&argvars[2]);
    }

    எனில் (map_to_exists(name, mode, abbr))
	rettv->vval.v_எண் = TRUE;
    இல்லையெனில்
	rettv->vval.v_எண் = FALSE;
}

/*
 * Fill in the empty dictionary with items as defined by maparg builtin.
 */
    நிலையான வெறுமை
mapblock2dict(
	mapblock_T  *mp,
	dict_T	    *dict,
	char_u	    *lhsrawalt,	    // may be NULL
	முழு	    buffer_local,   // false if not buffer local mapping
	முழு	    abbr)	    // true if abbreviation
{
    char_u	    *lhs = str2special_save(mp->m_சாவிகள், TRUE, FALSE);
    char_u	    *mapmode = map_mode_to_chars(mp->m_mode);

    dict_add_string(dict, "lhs", lhs);
    vim_free(lhs);
    dict_add_string(dict, "lhsraw", mp->m_சாவிகள்);
    எனில் (lhsrawalt)
	// Also add the value for the simplified entry.
	dict_add_string(dict, "lhsrawalt", lhsrawalt);
    dict_add_string(dict, "rhs", mp->m_orig_str);
    dict_add_எண்(dict, "noremap", mp->m_noremap ? 1L : 0L);
    dict_add_எண்(dict, "script", mp->m_noremap == REMAP_SCRIPT
		    ? 1L : 0L);
    dict_add_எண்(dict, "expr", mp->m_கோவை ? 1L : 0L);
    dict_add_எண்(dict, "silent", mp->m_silent ? 1L : 0L);
    dict_add_எண்(dict, "sid", (long)mp->m_script_ctx.sc_sid);
    dict_add_எண்(dict, "scriptversion",
		    (long)mp->m_script_ctx.sc_version);
    dict_add_எண்(dict, "lnum", (long)mp->m_script_ctx.sc_lnum);
    dict_add_எண்(dict, "buffer", (long)buffer_local);
    dict_add_எண்(dict, "nowait", mp->m_nowait ? 1L : 0L);
    dict_add_string(dict, "mode", mapmode);
    dict_add_எண்(dict, "abbr", abbr ? 1L : 0L);
    dict_add_எண்(dict, "mode_bits", mp->m_mode);

    vim_free(mapmode);
}

    நிலையான வெறுமை
get_maparg(typval_T *argvars, typval_T *rettv, முழு exact)
{
    char_u	*சாவிகள்;
    char_u	*சாவிகள்_simplified;
    char_u	*which;
    char_u	buf[NUMBUFLEN];
    char_u	*சாவிகள்_buf = வெற்று;
    char_u	*alt_சாவிகள்_buf = வெற்று;
    முழு		did_simplify = FALSE;
    char_u	*rhs;
    முழு		mode;
    முழு		abbr = FALSE;
    முழு		get_dict = FALSE;
    mapblock_T	*mp = வெற்று;
    முழு		buffer_local;
    முழு		flags = REPTERM_FROM_PART | REPTERM_DO_LT;

    // return empty string for failure
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;

    சாவிகள் = tv_get_string(&argvars[0]);
    எனில் (*சாவிகள் == NUL)
	திரும்பு;

    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	which = tv_get_string_buf_chk(&argvars[1], buf);
	எனில் (argvars[2].v_type != VAR_UNKNOWN)
	{
	    abbr = (முழு)tv_get_bool(&argvars[2]);
	    எனில் (argvars[3].v_type != VAR_UNKNOWN)
		get_dict = (முழு)tv_get_bool(&argvars[3]);
	}
    }
    இல்லையெனில்
	which = (char_u *)"";
    எனில் (which == வெற்று)
	திரும்பு;

    mode = get_map_mode(&which, 0);

    சாவிகள்_simplified = replace_termcodes(சாவிகள், &சாவிகள்_buf, 0, flags,
								&did_simplify);
    rhs = check_map(சாவிகள்_simplified, mode, exact, FALSE, abbr,
							   &mp, &buffer_local);
    எனில் (did_simplify)
    {
	// When the lhs is being simplified the not-simplified keys are
	// preferred for printing, like in do_map().
	(வெறுமை)replace_termcodes(சாவிகள், &alt_சாவிகள்_buf, 0,
					flags | REPTERM_NO_SIMPLIFY, வெற்று);
	rhs = check_map(alt_சாவிகள்_buf, mode, exact, FALSE, abbr, &mp,
								&buffer_local);
    }

    எனில் (!get_dict)
    {
	// Return a string.
	எனில் (rhs != வெற்று)
	{
	    எனில் (*rhs == NUL)
		rettv->vval.v_string = vim_strsave((char_u *)"<Nop>");
	    இல்லையெனில்
		rettv->vval.v_string = str2special_save(rhs, FALSE, FALSE);
	}

    }
    இல்லையெனில் எனில் (rettv_dict_alloc(rettv) == OK && rhs != வெற்று)
	mapblock2dict(mp, rettv->vval.v_dict,
			  did_simplify ? சாவிகள்_simplified : வெற்று,
			  buffer_local, abbr);

    vim_free(சாவிகள்_buf);
    vim_free(alt_சாவிகள்_buf);
}

/*
 * "maplist()" function
 */
    வெறுமை
f_maplist(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    mapblock_T	*mp;
    முழு		buffer_local;
    char_u	*சாவிகள்_buf;
    முழு		did_simplify;
    முழு		hash;
    char_u	*lhs;
    மாறிலி முழு	flags = REPTERM_FROM_PART | REPTERM_DO_LT;
    முழு		abbr = FALSE;

    எனில் (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	திரும்பு;
    எனில் (argvars[0].v_type != VAR_UNKNOWN)
	abbr = tv_get_bool(&argvars[0]);

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    validate_maphash();

    // Do it twice: once for global maps and once for local maps.
    க்கு (buffer_local = 0; buffer_local <= 1; ++buffer_local)
    {
	க்கு (hash = 0; hash < 256; ++hash)
	{
	    எனில் (abbr)
	    {
		எனில் (hash > 0)		// there is only one abbr list
		    நிறுத்து;
		எனில் (buffer_local)
		    mp = curbuf->b_first_abbr;
		இல்லையெனில்
		    mp = first_abbr;
	    }
	    இல்லையெனில் எனில் (buffer_local)
		mp = curbuf->b_maphash[hash];
	    இல்லையெனில்
		mp = maphash[hash];
	    க்கு (; mp; mp = mp->m_next)
	    {
		எனில் (mp->m_simplified)
		    தொடர்;
		எனில் ((d = dict_alloc()) == வெற்று)
		    திரும்பு;
		எனில் (list_append_dict(rettv->vval.v_list, d) == FAIL)
		{
		    dict_unref(d);
		    திரும்பு;
		}

		சாவிகள்_buf = வெற்று;
		did_simplify = FALSE;

		lhs = str2special_save(mp->m_சாவிகள், TRUE, FALSE);
		(வெறுமை)replace_termcodes(lhs, &சாவிகள்_buf, 0, flags,
								&did_simplify);
		vim_free(lhs);

		mapblock2dict(mp, d,
				 did_simplify ? சாவிகள்_buf : வெற்று,
				 buffer_local, abbr);
		vim_free(சாவிகள்_buf);
	    }
	}
    }
}

/*
 * "maparg()" function
 */
    வெறுமை
f_maparg(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_opt_bool_arg(argvars, 2) == FAIL
			|| (argvars[2].v_type != VAR_UNKNOWN
			    && check_for_opt_bool_arg(argvars, 3) == FAIL)))))
		திரும்பு;

    get_maparg(argvars, rettv, TRUE);
}

/*
 * "mapcheck()" function
 */
    வெறுமை
f_mapcheck(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 2) == FAIL)))
	திரும்பு;

    get_maparg(argvars, rettv, FALSE);
}

/*
 * Get the mapping mode from the mode string.
 * It may contain multiple characters, eg "nox", or "!", or ' '
 * Return 0 if there is an error.
 */
    நிலையான முழு
get_map_mode_string(char_u *mode_string, முழு abbr)
{
    char_u	*p = mode_string;
    முழு		mode = 0;
    முழு		tmode;
    முழு		modec;
    மாறிலி முழு	MASK_V = MODE_VISUAL | MODE_SELECT;
    மாறிலி முழு	MASK_MAP = MODE_VISUAL | MODE_SELECT | MODE_NORMAL
							     | MODE_OP_PENDING;
    மாறிலி முழு	MASK_BANG = MODE_செருகு | MODE_CMDLINE;

    எனில் (*p == NUL)
	p = (char_u *)" ";	// compatibility
    போது ((modec = *p++))
    {
	தேர்வு (modec)
	{
	    நிலை 'i': tmode = MODE_செருகு;	நிறுத்து;
	    நிலை 'l': tmode = MODE_LANGMAP;	நிறுத்து;
	    நிலை 'c': tmode = MODE_CMDLINE;	நிறுத்து;
	    நிலை 'n': tmode = MODE_NORMAL;	நிறுத்து;
	    நிலை 'x': tmode = MODE_VISUAL;	நிறுத்து;
	    நிலை 's': tmode = MODE_SELECT;	நிறுத்து;
	    நிலை 'o': tmode = MODE_OP_PENDING;	நிறுத்து;
	    நிலை 't': tmode = MODE_TERMINAL;	நிறுத்து;
	    நிலை 'v': tmode = MASK_V;		நிறுத்து;
	    நிலை '!': tmode = MASK_BANG;	நிறுத்து;
	    நிலை ' ': tmode = MASK_MAP;		நிறுத்து;
	    இயல்பு:
		      திரும்பு 0; // error, unknown mode character
	}
	mode |= tmode;
    }
    எனில் ((abbr && (mode & ~MASK_BANG) != 0)
	|| (!abbr && (mode & (mode-1)) != 0 // more than one bit set
	    && (
		// false if multiple bits set in mode and mode is fully
		// contained in one mask
		!(((mode & MASK_BANG) != 0 && (mode & ~MASK_BANG) == 0)
		    || ((mode & MASK_MAP) != 0 && (mode & ~MASK_MAP) == 0)))))
	திரும்பு 0;

    திரும்பு mode;
}

/*
 * "mapset()" function
 */
    வெறுமை
f_mapset(typval_T *argvars, typval_T *rettv UNUSED)
{
    char_u	*which;
    முழு		mode;
    char_u	buf[NUMBUFLEN];
    முழு		is_abbr;
    dict_T	*d;
    char_u	*lhs;
    char_u	*lhsraw;
    char_u	*lhsrawalt;
    char_u	*rhs;
    char_u	*orig_rhs;
    char_u	*arg_buf = வெற்று;
    முழு		noremap;
    முழு		கோவை;
    முழு		silent;
    முழு		buffer;
    scid_T	sid;
    முழு		scriptversion;
    linenr_T	lnum;
    mapblock_T	**map_table = maphash;
    mapblock_T  **abbr_table = &first_abbr;
    முழு		nowait;
    char_u	*arg;
    முழு		dict_only;
    mapblock_T	*mp_result[2] = {வெற்று, வெற்று};

    எனில் (check_secure())
	திரும்பு;

    // If first arg is a dict, then that's the only arg permitted.
    dict_only = argvars[0].v_type == VAR_DICT;
    எனில் (in_vim9script()
	    && (check_for_string_or_dict_arg(argvars, 0) == FAIL
		|| (dict_only && check_for_unknown_arg(argvars, 1) == FAIL)
		|| (!dict_only
		    && (check_for_string_arg(argvars, 0) == FAIL
			|| check_for_bool_arg(argvars, 1) == FAIL
			|| check_for_dict_arg(argvars, 2) == FAIL))))
	திரும்பு;

    எனில் (dict_only)
    {
	d = argvars[0].vval.v_dict;
	which = dict_get_string(d, "mode", FALSE);
	is_abbr = dict_get_bool(d, "abbr", -1);
	எனில் (which == வெற்று || is_abbr < 0)
	{
	    emsg(_(e_entries_missing_in_mapset_dict_argument));
	    திரும்பு;
	}
    }
    இல்லையெனில்
    {
	which = tv_get_string_buf_chk(&argvars[0], buf);
	எனில் (which == வெற்று)
	    திரும்பு;
	is_abbr = (முழு)tv_get_bool(&argvars[1]);

	எனில் (check_for_dict_arg(argvars, 2) == FAIL)
	    திரும்பு;
	d = argvars[2].vval.v_dict;
    }
    mode = get_map_mode_string(which, is_abbr);
    எனில் (mode == 0)
    {
	semsg(_(e_illegal_map_mode_string_str), which);
	திரும்பு;
    }


    // Get the values in the same order as above in get_maparg().
    lhs = dict_get_string(d, "lhs", FALSE);
    lhsraw = dict_get_string(d, "lhsraw", FALSE);
    lhsrawalt = dict_get_string(d, "lhsrawalt", FALSE);
    rhs = dict_get_string(d, "rhs", FALSE);
    எனில் (lhs == வெற்று || lhsraw == வெற்று || rhs == வெற்று)
    {
	emsg(_(e_entries_missing_in_mapset_dict_argument));
	திரும்பு;
    }
    orig_rhs = rhs;

    noremap = dict_get_எண்(d, "noremap") ? REMAP_NONE: 0;
    எனில் (dict_get_எண்(d, "script") != 0)
	noremap = REMAP_SCRIPT;
    கோவை = dict_get_எண்(d, "expr") != 0;
    silent = dict_get_எண்(d, "silent") != 0;
    sid = dict_get_எண்(d, "sid");
    scriptversion = dict_get_எண்(d, "scriptversion");
    lnum = dict_get_எண்(d, "lnum");
    buffer = dict_get_எண்(d, "buffer");
    nowait = dict_get_எண்(d, "nowait") != 0;
    // mode from the dict is not used

    எனில் (STRICMP(rhs, "<nop>") == 0)	// "<Nop>" means nothing
	rhs = (char_u *)"";
    இல்லையெனில்
	rhs = replace_termcodes(rhs, &arg_buf, sid,
					REPTERM_DO_LT | REPTERM_SPECIAL, வெற்று);

    எனில் (buffer)
    {
	map_table = curbuf->b_maphash;
	abbr_table = &curbuf->b_first_abbr;
    }

    // Delete any existing mapping for this lhs and mode.
    எனில் (buffer)
    {
	arg = alloc(STRLEN(lhs) + STRLEN("<buffer>") + 1);
	எனில் (arg == வெற்று)
	    திரும்பு;
	STRCPY(arg, "<buffer>");
	STRCPY(arg + 8, lhs);
    }
    இல்லையெனில்
    {
	arg = vim_strsave(lhs);
	எனில் (arg == வெற்று)
	    திரும்பு;
    }
    do_map(MAPTYPE_UNMAP_LHS, arg, mode, is_abbr);
    vim_free(arg);

    mp_result[0] = map_add(map_table, abbr_table, lhsraw, rhs, orig_rhs,
			    noremap, nowait, silent, mode, is_abbr, கோவை, sid,
			    scriptversion, lnum, 0);
    எனில் (lhsrawalt != வெற்று)
	mp_result[1] = map_add(map_table, abbr_table, lhsrawalt, rhs, orig_rhs,
			    noremap, nowait, silent, mode, is_abbr, கோவை, sid,
			    scriptversion, lnum, 1);

    எனில் (mp_result[0] != வெற்று && mp_result[1] != வெற்று)
    {
	mp_result[0]->m_alt = mp_result[1];
	mp_result[1]->m_alt = mp_result[0];
    }

    vim_free(arg_buf);
}
#endif


#எனில் defined(MSWIN) || defined(MACOS_X)

# define VIS_SEL	(MODE_VISUAL | MODE_SELECT)	// abbreviation

/*
 * Default mappings for some often used keys.
 */
கட்டமைப்பு initmap
{
    char_u	*arg;
    முழு		mode;
};

# ifdef FEAT_GUI_MSWIN
// Use the Windows (CUA) keybindings. (GUI)
நிலையான கட்டமைப்பு initmap initmappings[] =
{
	// paste, copy and cut
	{(char_u *)"<S-Insert> \"*P", MODE_NORMAL},
	{(char_u *)"<S-Insert> \"-d\"*P", VIS_SEL},
	{(char_u *)"<S-Insert> <C-R><C-O>*", MODE_செருகு | MODE_CMDLINE},
	{(char_u *)"<C-Insert> \"*y", VIS_SEL},
	{(char_u *)"<S-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-X> \"*d", VIS_SEL},
	// Missing: CTRL-C (cancel) and CTRL-V (block selection)
};
# endif

# எனில் defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
// Use the Windows (CUA) keybindings. (Console)
நிலையான கட்டமைப்பு initmap cinitmappings[] =
{
	{(char_u *)"\316w <C-Home>", MODE_NORMAL | VIS_SEL},
	{(char_u *)"\316w <C-Home>", MODE_செருகு | MODE_CMDLINE},
	{(char_u *)"\316u <C-End>", MODE_NORMAL | VIS_SEL},
	{(char_u *)"\316u <C-End>", MODE_செருகு | MODE_CMDLINE},

	// paste, copy and cut
#  ifdef FEAT_CLIPBOARD
	{(char_u *)"\316\324 \"*P", MODE_NORMAL},   // SHIFT-Insert is "*P
	{(char_u *)"\316\324 \"-d\"*P", VIS_SEL},   // SHIFT-Insert is "-d"*P
	{(char_u *)"\316\324 \022\017*", MODE_செருகு},  // SHIFT-Insert is ^R^O*
	{(char_u *)"\316\325 \"*y", VIS_SEL},	    // CTRL-Insert is "*y
	{(char_u *)"\316\327 \"*d", VIS_SEL},	    // SHIFT-Del is "*d
	{(char_u *)"\316\330 \"*d", VIS_SEL},	    // CTRL-Del is "*d
	{(char_u *)"\030 \"*d", VIS_SEL},	    // CTRL-X is "*d
#  இல்லையெனில்
	{(char_u *)"\316\324 P", MODE_NORMAL},	    // SHIFT-Insert is P
	{(char_u *)"\316\324 \"-dP", VIS_SEL},	    // SHIFT-Insert is "-dP
	{(char_u *)"\316\324 \022\017\"", MODE_செருகு}, // SHIFT-Insert is ^R^O"
	{(char_u *)"\316\325 y", VIS_SEL},	    // CTRL-Insert is y
	{(char_u *)"\316\327 d", VIS_SEL},	    // SHIFT-Del is d
	{(char_u *)"\316\330 d", VIS_SEL},	    // CTRL-Del is d
#  endif
};
# endif

# எனில் defined(MACOS_X)
நிலையான கட்டமைப்பு initmap initmappings[] =
{
	// Use the Standard MacOS binding.
	// paste, copy and cut
	{(char_u *)"<D-v> \"*P", MODE_NORMAL},
	{(char_u *)"<D-v> \"-d\"*P", VIS_SEL},
	{(char_u *)"<D-v> <C-R>*", MODE_செருகு | MODE_CMDLINE},
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
    வெறுமை
init_mappings(வெறுமை)
{
#எனில் defined(MSWIN) || defined(MACOS_X)
    முழு		i;

# எனில் defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    எனில் (!gui.starting)
#  endif
    {
	க்கு (i = 0; i < (முழு)ARRAY_LENGTH(cinitmappings); ++i)
	    add_map(cinitmappings[i].arg, cinitmappings[i].mode, FALSE);
    }
# endif
# எனில் defined(FEAT_GUI_MSWIN) || defined(MACOS_X)
    க்கு (i = 0; i < (முழு)ARRAY_LENGTH(initmappings); ++i)
	add_map(initmappings[i].arg, initmappings[i].mode, FALSE);
# endif
#endif
}

/*
 * Add a mapping "map" for mode "mode".
 * When "nore" is TRUE use MAPTYPE_NOREMAP.
 * Need to put string in allocated memory, because do_map() will modify it.
 */
    வெறுமை
add_map(char_u *map, முழு mode, முழு nore)
{
    char_u	*s;
    char_u	*cpo_save = p_cpo;

    p_cpo = empty_option;	// Allow <> notation
    s = vim_strsave(map);
    எனில் (s != வெற்று)
    {
	(வெறுமை)do_map(nore ? MAPTYPE_NOREMAP : MAPTYPE_MAP, s, mode, FALSE);
	vim_free(s);
    }
    p_cpo = cpo_save;
}

#எனில் defined(FEAT_LANGMAP)
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
வகைவரை கட்டமைப்பு
{
    முழு	    from;
    முழு     to;
} langmap_entry_T;

நிலையான garray_T langmap_mapga;

/*
 * Search for an entry in "langmap_mapga" for "from".  If found set the "to"
 * field.  If not found insert a new entry at the appropriate location.
 */
    நிலையான வெறுமை
langmap_set_entry(முழு from, முழு to)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_data);
    முழு		    a = 0;
    முழு		    b = langmap_mapga.ga_len;

    // Do a binary search for an existing entry.
    போது (a != b)
    {
	முழு i = (a + b) / 2;
	முழு d = entries[i].from - from;

	எனில் (d == 0)
	{
	    entries[i].to = to;
	    திரும்பு;
	}
	எனில் (d < 0)
	    a = i + 1;
	இல்லையெனில்
	    b = i;
    }

    எனில் (ga_grow(&langmap_mapga, 1) == FAIL)
	திரும்பு;  // out of memory

    // insert new entry at position "a"
    entries = (langmap_entry_T *)(langmap_mapga.ga_data) + a;
    mch_memmove(entries + 1, entries,
			(langmap_mapga.ga_len - a) * அளவு(langmap_entry_T));
    ++langmap_mapga.ga_len;
    entries[0].from = from;
    entries[0].to = to;
}

/*
 * Apply 'langmap' to multi-byte character "c" and return the result.
 */
    முழு
langmap_adjust_mb(முழு c)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_data);
    முழு a = 0;
    முழு b = langmap_mapga.ga_len;

    போது (a != b)
    {
	முழு i = (a + b) / 2;
	முழு d = entries[i].from - c;

	எனில் (d == 0)
	    திரும்பு entries[i].to;  // found matching entry
	எனில் (d < 0)
	    a = i + 1;
	இல்லையெனில்
	    b = i;
    }
    திரும்பு c;  // no entry found, return "c" unmodified
}

    வெறுமை
langmap_init(வெறுமை)
{
    முழு i;

    க்கு (i = 0; i < 256; i++)
	langmap_mapchar[i] = i;	 // we init with a one-to-one map
    ga_init2(&langmap_mapga, அளவு(langmap_entry_T), 8);
}

/*
 * Called when langmap option is set; the language map can be
 * changed at any time!
 */
    எழுத்து *
did_set_langmap(optset_T *args UNUSED)
{
    char_u  *p;
    char_u  *p2;
    முழு	    from, to;

    ga_clear(&langmap_mapga);		    // clear the previous map first
    langmap_init();			    // back to one-to-one map

    க்கு (p = p_langmap; p[0] != NUL; )
    {
	க்கு (p2 = p; p2[0] != NUL && p2[0] != ',' && p2[0] != ';';
							       MB_PTR_ADV(p2))
	{
	    எனில் (p2[0] == '\\' && p2[1] != NUL)
		++p2;
	}
	எனில் (p2[0] == ';')
	    ++p2;	    // abcd;ABCD form, p2 points to A
	இல்லையெனில்
	    p2 = வெற்று;	    // aAbBcCdD form, p2 is NULL
	போது (p[0])
	{
	    எனில் (p[0] == ',')
	    {
		++p;
		நிறுத்து;
	    }
	    எனில் (p[0] == '\\' && p[1] != NUL)
		++p;
	    from = (*mb_ptr2char)(p);
	    to = NUL;
	    எனில் (p2 == வெற்று)
	    {
		MB_PTR_ADV(p);
		எனில் (p[0] != ',')
		{
		    எனில் (p[0] == '\\')
			++p;
		    to = (*mb_ptr2char)(p);
		}
	    }
	    இல்லையெனில்
	    {
		எனில் (p2[0] != ',')
		{
		    எனில் (p2[0] == '\\')
			++p2;
		    to = (*mb_ptr2char)(p2);
		}
	    }
	    எனில் (to == NUL)
	    {
		sprintf(args->os_errbuf,
			_(e_langmap_matching_character_missing_for_str),
			transchar(from));
		திரும்பு args->os_errbuf;
	    }

	    எனில் (from >= 256)
		langmap_set_entry(from, to);
	    இல்லையெனில்
		langmap_mapchar[from & 255] = to;

	    // Advance to next pair
	    MB_PTR_ADV(p);
	    எனில் (p2 != வெற்று)
	    {
		MB_PTR_ADV(p2);
		எனில் (*p == ';')
		{
		    p = p2;
		    எனில் (p[0] != NUL)
		    {
			எனில் (p[0] != ',')
			{
			    vim_snprintf(args->os_errbuf, args->os_errbuflen,
				    _(e_langmap_extra_characters_after_semicolon_str),
				    p);
			    திரும்பு args->os_errbuf;
			}
			++p;
		    }
		    நிறுத்து;
		}
	    }
	}
    }

    திரும்பு வெற்று;
}
#endif

    நிலையான வெறுமை
do_exmap(exarg_T *eap, முழு isabbrev)
{
    முழு	    mode;
    char_u  *cmdp;

    cmdp = eap->cmd;
    mode = get_map_mode(&cmdp, eap->forceit || isabbrev);

    தேர்வு (do_map(*cmdp == 'n' ? MAPTYPE_NOREMAP
				: *cmdp == 'u' ? MAPTYPE_UNMAP : MAPTYPE_MAP,
						    eap->arg, mode, isabbrev))
    {
	நிலை 1: emsg(_(e_invalid_argument));
		நிறுத்து;
	நிலை 2: emsg((isabbrev ? _(e_no_such_abbreviation)
						      : _(e_no_such_mapping)));
		நிறுத்து;
    }
}

/*
 * ":abbreviate" and friends.
 */
    வெறுமை
ex_abbreviate(exarg_T *eap)
{
    do_exmap(eap, TRUE);	// almost the same as mapping
}

/*
 * ":map" and friends.
 */
    வெறுமை
ex_map(exarg_T *eap)
{
    // If we are sourcing .exrc or .vimrc in current directory we
    // print the mappings for security reasons.
    எனில் (secure)
    {
	secure = 2;
	செய்தி_outtrans(eap->cmd);
	செய்தி_putchar('\n');
    }
    do_exmap(eap, FALSE);
}

/*
 * ":unmap" and friends.
 */
    வெறுமை
ex_unmap(exarg_T *eap)
{
    do_exmap(eap, FALSE);
}

/*
 * ":mapclear" and friends.
 */
    வெறுமை
ex_mapclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, eap->forceit, FALSE);
}

/*
 * ":abclear" and friends.
 */
    வெறுமை
ex_abclear(exarg_T *eap)
{
    map_clear(eap->cmd, eap->arg, TRUE, TRUE);
}
